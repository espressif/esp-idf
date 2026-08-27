/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_chunked.c
 * @brief P0 Critical Tests: chunked transfer-encoding decode and mid-chunk FIN handling
 *
 * This file characterizes:
 * - A well-formed chunked response decodes correctly and is reported via
 *   esp_http_client_is_chunked_response(), and the decoded body content
 *   matches the concatenation of the chunk payloads.
 * - A connection that drops mid-chunk (FIN before the chunked stream is
 *   terminated) surfaces as an error from esp_http_client_perform(), not a
 *   silent ESP_OK.
 */

#include <string.h>
#include "esp_http_client.h"
#include "unity.h"
#include "sdkconfig.h"
#include "test_http_client_mock_transport.h"

/*
 * Every case in this file drives the client through a mock transport injected
 * via esp_http_client_config_t::transport. Without custom transport support
 * the clients would fall back to a real transport aimed at test-server.local,
 * which does not exist, so the whole file compiles out.
 */
#if CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT

static const char *resp_chunked =
    "HTTP/1.1 200 OK\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\nhello\r\n"
    "6\r\n world\r\n"
    "0\r\n\r\n";

/* Accumulates HTTP_EVENT_ON_DATA payloads into a NUL-terminated buffer so the
 * decode test can assert on the actual decoded chunked body, not just that
 * decoding "succeeded" per the status/return code. */
static char s_body_buf[32];
static size_t s_body_len;

static esp_err_t body_capture_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        size_t space = sizeof(s_body_buf) - 1 - s_body_len;
        size_t copy = (size_t)evt->data_len < space ? (size_t)evt->data_len : space;
        memcpy(s_body_buf + s_body_len, evt->data, copy);
        s_body_len += copy;
        s_body_buf[s_body_len] = '\0';
    }
    return ESP_OK;
}

TEST_CASE("chunked response decodes and reports is_chunked", "[esp_http_client][chunked][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_chunked;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    s_body_len = 0;
    s_body_buf[0] = '\0';
    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/stream",
        .event_handler = body_capture_handler,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));
    TEST_ASSERT_TRUE(esp_http_client_is_chunked_response(client));

    /* The two chunks ("hello" + " world") must decode to the concatenated
     * payload, stripped of chunk-size lines and CRLF framing. */
    TEST_ASSERT_EQUAL(11, s_body_len);
    TEST_ASSERT_EQUAL_STRING("hello world", s_body_buf);

    int chunk_len = 0;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_chunk_length(client, &chunk_len));
    /* characterization: master behavior, see refactor spec
     * after the terminating "0\r\n\r\n" chunk has been consumed,
     * get_chunk_length() reports 0 - it reflects the last-seen chunk-size
     * line (the zero-length terminator), not "unknown"/-1. */
    TEST_ASSERT_EQUAL(0, chunk_len);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("FIN mid-chunk surfaces an error, not success", "[esp_http_client][chunked][p0][negative]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.mode = MOCK_TRANSPORT_MODE_INCOMPLETE_READ;
    mc.response_data = resp_chunked;
    /* READ-side-only budget (independent of the 84-byte request write):
     * resp_chunked's headers are 47 bytes; "5\r\nhell" is 7 more bytes into
     * the first chunk's data ("hello" is 5 bytes, missing the last "o" and
     * the trailing CRLF, and the whole second chunk). 47+7=54 truncates
     * genuinely inside the first chunk's body, not at a header/chunk
     * boundary and not before any response byte is read. */
    mc.read_bytes_before_error = 54;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/stream",
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    /* characterization: master behavior, see refactor spec
     * master returns ESP_ERR_HTTP_INCOMPLETE_DATA for a chunked body
     * genuinely truncated mid-chunk (as opposed to ESP_ERR_HTTP_EAGAIN,
     * which is what a request-write-vs-read-budget race produces instead -
     * see the commit message for how those two differ). On real hardware,
     * a raw transport read of 0 for "connection closed" collides with the
     * raw "read timeout" sentinel (both 0); which classification wins
     * depends on leftover errno state, so master also returns
     * ESP_ERR_HTTP_READ_TIMEOUT for this same input on some targets. Both
     * are accepted here; the refactor branch's Stage 4 work fixes the
     * underlying sentinel collision. */
    TEST_ASSERT_TRUE(err == ESP_ERR_HTTP_INCOMPLETE_DATA || err == ESP_ERR_HTTP_READ_TIMEOUT);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
