/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_lifecycle.c
 * @brief P0 Critical Tests: keep-alive connection reuse and get_state sequence
 *
 * This file characterizes:
 * - Connection reuse across two esp_http_client_perform() calls when the
 *   response carries "Connection: keep-alive"
 * - Forced reconnect on the next perform() when the response carries
 *   "Connection: close"
 * - The exact esp_http_client_get_state() sequence observed from inside the
 *   event handler during a simple GET, pinned as a regression baseline for
 *   the state-machine refactor.
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

static const char *resp_ka =
    "HTTP/1.1 200 OK\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: 2\r\n"
    "\r\n"
    "ok";

static const char *resp_close =
    "HTTP/1.1 200 OK\r\n"
    "Connection: close\r\n"
    "Content-Length: 2\r\n"
    "\r\n"
    "ok";

TEST_CASE("keep-alive reuses connection across two performs", "[esp_http_client][lifecycle][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_ka;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    mock_http_transport_queue_response(mock, resp_ka, 0);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/a",
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));
    /* characterization: parked state between requests is CONNECTED */
    TEST_ASSERT_EQUAL(HTTP_STATE_CONNECTED, esp_http_client_get_state(client));

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));

    mock_http_transport_stats_t stats;
    mock_http_transport_get_stats(mock, &stats);
    TEST_ASSERT_EQUAL(1, stats.connect_calls);   /* the whole point of keep-alive */

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("Connection: close forces reconnect on next perform", "[esp_http_client][lifecycle][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_close;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    /* characterization: this queued response is never popped - the forced
     * reconnect calls mock_close() then mock_connect(), both of which reset
     * read_offset to 0, so the second perform() re-serves the identical
     * initial buffer instead of advancing the queue; kept here for
     * intent-documentation. */
    mock_http_transport_queue_response(mock, resp_close, 0);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/a",
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));

    mock_http_transport_stats_t stats;
    mock_http_transport_get_stats(mock, &stats);
    TEST_ASSERT_EQUAL(2, stats.connect_calls);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

/* state sequence observed via events — pins the getter mapping for the refactor */
static esp_http_state_t s_state_log[16];
static int s_state_log_n;

static esp_err_t state_logger(esp_http_client_event_t *evt)
{
    if (s_state_log_n < 16) {
        s_state_log[s_state_log_n++] = esp_http_client_get_state(evt->client);
    }
    return ESP_OK;
}

TEST_CASE("get_state sequence during a simple GET", "[esp_http_client][lifecycle][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_ka;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    s_state_log_n = 0;
    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/a",
        .event_handler = state_logger,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);
    TEST_ASSERT_EQUAL(HTTP_STATE_INIT, esp_http_client_get_state(client));

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));

    /* characterization: master behavior, see refactor spec
     * exact sequence observed by the event handler, frozen as a regression
     * baseline for the state-machine refactor. get_state() lags the event
     * that triggered it in several places: it still reports
     * REQ_COMPLETE_DATA at ON_STATUS_CODE and both ON_HEADER callbacks, and
     * still reports RES_COMPLETE_HEADER at ON_HEADERS_COMPLETE and ON_DATA;
     * it only advances to RES_ON_DATA_START at ON_FINISH, after all
     * response data has already been delivered to the handler. */
    TEST_ASSERT_EQUAL(8, s_state_log_n);
    TEST_ASSERT_EQUAL(HTTP_STATE_CONNECTED, s_state_log[0]);           /* at ON_CONNECTED */
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_HEADER, s_state_log[1]); /* at HEADERS_SENT */
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_DATA, s_state_log[2]);   /* at ON_STATUS_CODE */
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_DATA, s_state_log[3]);   /* at ON_HEADER (Connection) */
    TEST_ASSERT_EQUAL(HTTP_STATE_REQ_COMPLETE_DATA, s_state_log[4]);   /* at ON_HEADER (Content-Length) */
    TEST_ASSERT_EQUAL(HTTP_STATE_RES_COMPLETE_HEADER, s_state_log[5]); /* at ON_HEADERS_COMPLETE */
    TEST_ASSERT_EQUAL(HTTP_STATE_RES_COMPLETE_HEADER, s_state_log[6]); /* at ON_DATA */
    TEST_ASSERT_EQUAL(HTTP_STATE_RES_ON_DATA_START, s_state_log[7]);   /* at ON_FINISH */

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
