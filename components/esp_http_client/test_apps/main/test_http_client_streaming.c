/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_streaming.c
 * @brief P0 Critical Tests: open/write/fetch_headers/read streaming path, partial
 *        transport writes, read timeout during header fetch, and the error code
 *        that HTTP_EVENT_ERROR carries on the read paths
 *
 * This file characterizes:
 * - The manual streaming sequence (open -> write -> fetch_headers -> read) works
 *   end to end against a mock transport, matching the one-shot perform() path.
 * - A transport that only accepts part of a write per call still delivers the
 *   full request body once the client has finished writing it out.
 * - A read timeout while waiting for response headers surfaces as an error from
 *   esp_http_client_perform() (exact code pinned per characterization rules).
 * - esp_http_client_event_t::error hands the handler the same esp_err_t that the
 *   failing call returns, for a failed header read, a body cut short, and a
 *   failed read on the streaming path.
 * - A parser error inside read() fails the call and leaves no state behind
 *   that a later read() or a reused handle would act on.
 */

#include <stdio.h>
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

/* Byte count of the status line plus the headers plus the empty line of resp_200. */
#define RESP_200_HEADER_LEN 38

static esp_err_t s_event_error;
static int s_error_events;

/* Records what HTTP_EVENT_ERROR carried, for the error-code cases below. */
static esp_err_t error_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ERROR) {
        s_error_events++;
        s_event_error = evt->error;
    }
    return ESP_OK;
}

static esp_http_client_handle_t make_error_client(esp_transport_handle_t mock)
{
    s_event_error = ESP_OK;
    s_error_events = 0;
    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/",
        .transport = mock,
        .event_handler = error_event_handler,
    };
    return esp_http_client_init(&cfg);
}

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
    // esp_http_client_open() (esp_http_client.c ~L1921) sets
    // state = HTTP_STATE_REQ_COMPLETE_HEADER right after writing the
    // request line and headers over the transport.
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_HEADER, esp_http_client_get_state(client));
    TEST_ASSERT_EQUAL(5, esp_http_client_write(client, body, 5));
    // The public esp_http_client_write() (~L1978) never touches
    // client->state - it only requires state >= REQ_COMPLETE_HEADER and
    // writes bytes directly over the transport, so the state observed
    // here is unchanged from the open() call above.
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_HEADER, esp_http_client_get_state(client));
    TEST_ASSERT_EQUAL(5, esp_http_client_fetch_headers(client));
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
    /* mock_read() under MOCK_TRANSPORT_MODE_READ_TIMEOUT returns -1/ETIMEDOUT
     * (not the ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT == 0 sentinel), so
     * esp_http_client_fetch_headers() takes its generic-failure branch
     * (returns ESP_FAIL) rather than its -ESP_ERR_HTTP_EAGAIN branch, and
     * esp_http_client_perform() surfaces that as ESP_ERR_HTTP_FETCH_HEADER. */
    TEST_ASSERT_EQUAL(ESP_ERR_HTTP_FETCH_HEADER, err);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("HTTP_EVENT_ERROR carries the code of a failed header read", "[esp_http_client][streaming][p0][negative]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.mode = MOCK_TRANSPORT_MODE_READ_TIMEOUT;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    esp_http_client_handle_t client = make_error_client(mock);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL_HEX32(ESP_ERR_HTTP_FETCH_HEADER, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(1, s_error_events);
    TEST_ASSERT_EQUAL_HEX32(ESP_ERR_HTTP_FETCH_HEADER, s_event_error);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("HTTP_EVENT_ERROR carries the code of a body cut short", "[esp_http_client][streaming][p0][negative]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.mode = MOCK_TRANSPORT_MODE_INCOMPLETE_READ;
    mc.response_data = resp_200;
    mc.read_bytes_before_error = RESP_200_HEADER_LEN + 2;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    esp_http_client_handle_t client = make_error_client(mock);
    TEST_ASSERT_NOT_NULL(client);

    /* The mock ends the body with a 0 read; the client maps that result,
     * ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT, to ESP_ERR_HTTP_READ_TIMEOUT. A real
     * transport ends a cut body with -1 or -2, so the literal here follows the
     * mock and must be revisited when the mock adopts the real convention. */
    TEST_ASSERT_EQUAL_HEX32(ESP_ERR_HTTP_READ_TIMEOUT, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(1, s_error_events);
    TEST_ASSERT_EQUAL_HEX32(ESP_ERR_HTTP_READ_TIMEOUT, s_event_error);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

/* esp_http_client_read() classifies a failed body read on its own, apart from
 * esp_http_client_perform(). This mock inverts the esp_tcp_transport_err_t
 * convention: it reports a read timeout as -1, the FIN sentinel, and a closed
 * connection as 0, the timeout sentinel. tcp_read() and ssl_read() do the
 * opposite. The row therefore lands on the FIN arm of the classification and
 * pins ESP_ERR_HTTP_CONNECTION_CLOSED. The mock never returns
 * ERR_TCP_TRANSPORT_CONNECTION_FAILED (-2), so the ESP_ERR_HTTP_INCOMPLETE_DATA
 * arm stays uncovered until a mock with the real convention is in this tree. */
TEST_CASE("HTTP_EVENT_ERROR carries the code of a failed streaming read", "[esp_http_client][streaming][p0][negative]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.mode = MOCK_TRANSPORT_MODE_INCOMPLETE_READ;
    mc.response_data = resp_200;
    mc.read_bytes_before_error = RESP_200_HEADER_LEN;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    esp_http_client_handle_t client = make_error_client(mock);
    TEST_ASSERT_NOT_NULL(client);

    /* The read budget stops the first read at the end of the headers, so the
     * client holds no buffered body byte when the body read starts. */
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_open(client, 0));
    TEST_ASSERT_EQUAL(5, esp_http_client_fetch_headers(client));
    TEST_ASSERT_EQUAL(0, s_error_events);

    mc.mode = MOCK_TRANSPORT_MODE_READ_TIMEOUT;
    mc.response_data = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_set_config(mock, &mc));

    char buf[16];
    TEST_ASSERT_EQUAL(-1, esp_http_client_read(client, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL(1, s_error_events);
    TEST_ASSERT_EQUAL_HEX32(ESP_ERR_HTTP_CONNECTION_CLOSED, s_event_error);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

/*
 * Parser error inside esp_http_client_read().
 *
 * With buffer_size 128, fetch_headers() reads the 47-byte head plus the first
 * 77 body bytes, which are cached. read() serves those, then reads the rest
 * itself: 23 more body bytes decode into the caller's buffer, and the bad
 * chunk size "ZZ" stops the parser. At that point the client holds a pointer
 * into the caller's buffer and a count of the bytes copied into it.
 *
 * Padding after "ZZ" keeps bytes on the wire past that first read(), so a
 * later read() reaches the parser instead of the mock's EOF, which reads as a
 * transport timeout.
 */
#define BAD_CHUNK_BODY_LEN 100
#define BAD_CHUNK_TAIL_PAD 128
#define BAD_CHUNK_READ_LEN 256
#define BAD_CHUNK_SENTINEL 0xA5

static char s_bad_chunk_resp[320];

static const char *bad_chunk_response(void)
{
    char *p = s_bad_chunk_resp;
    p += sprintf(p, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n%x\r\n", BAD_CHUNK_BODY_LEN);
    memset(p, 'a', BAD_CHUNK_BODY_LEN);
    p += BAD_CHUNK_BODY_LEN;
    p += sprintf(p, "\r\nZZ\r\n");
    memset(p, 'x', BAD_CHUNK_TAIL_PAD);
    p[BAD_CHUNK_TAIL_PAD] = '\0';
    return s_bad_chunk_resp;
}

/* Opens a GET on `mock` and runs read() into `buf` until the parser error. */
static esp_http_client_handle_t read_until_parser_error(esp_transport_handle_t mock, char *buf, int *read_ret)
{
    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/bad-chunk",
        .buffer_size = 128,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_open(client, 0));
    TEST_ASSERT_EQUAL(0, esp_http_client_fetch_headers(client));
    *read_ret = esp_http_client_read(client, buf, BAD_CHUNK_READ_LEN);
    return client;
}

static esp_transport_handle_t bad_chunk_mock_create(void)
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = bad_chunk_response();
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    return mock;
}

TEST_CASE("read returns ESP_FAIL on a parser error even after copying body bytes", "[esp_http_client][streaming][p0][negative]")
{
    /* static: on a broken client this buffer may be written after the call */
    static char buf[BAD_CHUNK_READ_LEN];
    esp_transport_handle_t mock = bad_chunk_mock_create();
    int ret = 0;
    esp_http_client_handle_t client = read_until_parser_error(mock, buf, &ret);

    /* 100 body bytes reached buf before the error; none of them may be
     * reported as valid data from a response that turned out malformed. */
    TEST_ASSERT_EQUAL(ESP_FAIL, ret);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("read after a parser error in read does not use stale state", "[esp_http_client][streaming][p0][negative]")
{
    static char buf[BAD_CHUNK_READ_LEN];
    esp_transport_handle_t mock = bad_chunk_mock_create();
    int ret = 0;
    esp_http_client_handle_t client = read_until_parser_error(mock, buf, &ret);
    TEST_ASSERT_EQUAL(ESP_FAIL, ret);

    /* A stale byte count with no cache behind it makes this copy from NULL.
     * Without it, the parser's error is sticky, so this read fails too. */
    char again[BAD_CHUNK_READ_LEN];
    TEST_ASSERT_EQUAL(ESP_FAIL, esp_http_client_read(client, again, sizeof(again)));

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("reuse after a parser error in read does not write to the old buffer", "[esp_http_client][streaming][p0][negative]")
{
    /* static so that a stale write lands somewhere harmless and observable */
    static char old_buf[BAD_CHUNK_READ_LEN];
    esp_transport_handle_t mock = bad_chunk_mock_create();
    int ret = 0;
    esp_http_client_handle_t client = read_until_parser_error(mock, old_buf, &ret);
    TEST_ASSERT_EQUAL(ESP_FAIL, ret);
    memset(old_buf, BAD_CHUNK_SENTINEL, sizeof(old_buf));

    /* Second request on the same handle; its body arrives with its head, so
     * fetch_headers() decodes it while the first read()'s pointer is live. */
    mock_http_transport_set_response(mock, resp_200, 0);
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_close(client));
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_open(client, 0));
    TEST_ASSERT_EQUAL(5, esp_http_client_fetch_headers(client));

    TEST_ASSERT_EACH_EQUAL_UINT8(BAD_CHUNK_SENTINEL, old_buf, sizeof(old_buf));

    char buf[16] = {0};
    TEST_ASSERT_EQUAL(5, esp_http_client_read(client, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("hello", buf);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
