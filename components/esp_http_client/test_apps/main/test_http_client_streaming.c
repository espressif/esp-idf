/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_streaming.c
 * @brief P0 Critical Tests: open/write/fetch_headers/read streaming path, partial
 *        transport writes, and read timeout during header fetch
 *
 * This file characterizes:
 * - The manual streaming sequence (open -> write -> fetch_headers -> read) works
 *   end to end against a mock transport, matching the one-shot perform() path.
 * - A transport that only accepts part of a write per call still delivers the
 *   full request body once the client has finished writing it out.
 * - A read timeout while waiting for response headers surfaces as an error from
 *   esp_http_client_perform() (exact code pinned per characterization rules).
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

static const char *resp_200 =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 5\r\n"
    "\r\n"
    "hello";

TEST_CASE("open/write/fetch_headers/read sequence works", "[esp_http_client][streaming][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_200;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/upload",
        .method = HTTP_METHOD_POST,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    const char *body = "abcde";
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_open(client, 5));
    // characterization: master behavior, see refactor spec
    // esp_http_client_open() (esp_http_client.c ~L1921) sets
    // state = HTTP_STATE_REQ_COMPLETE_HEADER right after writing the
    // request line and headers over the transport.
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_HEADER, esp_http_client_get_state(client));
    TEST_ASSERT_EQUAL(5, esp_http_client_write(client, body, 5));
    // characterization: master behavior, see refactor spec
    // The public esp_http_client_write() (~L1978) never touches
    // client->state - it only requires state >= REQ_COMPLETE_HEADER and
    // writes bytes directly over the transport, so the state observed
    // here is unchanged from the open() call above.
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_HEADER, esp_http_client_get_state(client));
    TEST_ASSERT_EQUAL(5, esp_http_client_fetch_headers(client));
    // characterization: master behavior, see refactor spec
    // esp_http_client_fetch_headers() (~L1667-1697) unconditionally sets
    // state = HTTP_STATE_REQ_COMPLETE_DATA on entry, then reads/parses
    // until the response header-parse loop exits, then unconditionally
    // sets state = HTTP_STATE_RES_ON_DATA_START before returning - it
    // never stops at RES_COMPLETE_HEADER and does not depend on whether
    // any body bytes were actually read yet.
    TEST_ASSERT_EQUAL(HTTP_STATE_RES_ON_DATA_START, esp_http_client_get_state(client));
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    char buf[16] = {0};
    int rd = esp_http_client_read(client, buf, sizeof(buf));
    // characterization: master behavior, see refactor spec
    // esp_http_client_read() (~L1435) never assigns client->state at all.
    // Also, for this canned response the single mock transport read done
    // inside fetch_headers() above already delivered the whole 43-byte
    // buffer (headers + 5-byte body) to the parser in one
    // http_parser_execute() call, so this read() serves the body from the
    // already-cached response buffer without issuing a second transport
    // read - the state observed here is unchanged from fetch_headers().
    TEST_ASSERT_EQUAL(HTTP_STATE_RES_ON_DATA_START, esp_http_client_get_state(client));
    TEST_ASSERT_EQUAL(5, rd);
    TEST_ASSERT_EQUAL_STRING("hello", buf);
    TEST_ASSERT_TRUE(esp_http_client_is_complete_data_received(client));

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_close(client));
    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("partial transport writes still send the full request", "[esp_http_client][streaming][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.mode = MOCK_TRANSPORT_MODE_WRITE_PARTIAL;
    mc.response_data = resp_200;
    /* MOCK_TRANSPORT_MODE_WRITE_PARTIAL alone (default bytes_before_error = -1)
     * never actually splits a write - should_inject_error() short-circuits on
     * a negative budget, so without this the mode is inert and the test below
     * would pass for the wrong reason (it never exercises a short write).
     * max_write_chunk forces every mock_write() call, header or body, to
     * accept at most 4 bytes, so the client's write-retry loop genuinely has
     * to run multiple times to get the 10-byte body through.
     * See mock limitation note in test_http_client_mock_transport.h. */
    mc.max_write_chunk = 4;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/upload",
        .method = HTTP_METHOD_POST,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);
    esp_http_client_set_post_field(client, "0123456789", 10);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));

    char req[2048];
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_last_request(mock, req, sizeof(req), NULL));
    TEST_ASSERT_NOT_NULL(strstr(req, "0123456789"));

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("read timeout during headers returns fetch-header error", "[esp_http_client][streaming][p0][negative]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.mode = MOCK_TRANSPORT_MODE_READ_TIMEOUT;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/slow",
        .timeout_ms = 100,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    /* characterization: master behavior, see refactor spec
     * mock_read() under MOCK_TRANSPORT_MODE_READ_TIMEOUT returns -1/ETIMEDOUT
     * (not the ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT == 0 sentinel), so
     * esp_http_client_fetch_headers() takes its generic-failure branch
     * (returns ESP_FAIL) rather than its -ESP_ERR_HTTP_EAGAIN branch, and
     * esp_http_client_perform() surfaces that as ESP_ERR_HTTP_FETCH_HEADER. */
    TEST_ASSERT_EQUAL(ESP_ERR_HTTP_FETCH_HEADER, err);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
