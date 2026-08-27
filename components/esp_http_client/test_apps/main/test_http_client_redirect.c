/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_redirect.c
 * @brief P0 Critical Tests: auto redirect, disabled redirect, max redirect count
 *
 * This file characterizes:
 * - A 302 response is followed automatically within a single perform() call,
 *   and the second request lands on the Location target.
 * - disable_auto_redirect surfaces the 302 to the caller instead of following it.
 * - max_redirection_count stops a redirect loop and reports it as an error.
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

static const char *resp_302 =
    "HTTP/1.1 302 Found\r\n"
    "Location: http://test-server.local/new\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

static const char *resp_200 =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 4\r\n"
    "\r\n"
    "done";

TEST_CASE("auto redirect follows 302 within one perform", "[esp_http_client][redirect][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_302;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    mock_http_transport_queue_response(mock, resp_200, 0);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/old",
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    char req[2048];
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_last_request(mock, req, sizeof(req), NULL));
    /* characterization: master behavior, see refactor spec
     * resp_302 has neither "Connection: close" nor an HTTP/1.0 status line,
     * so http_should_keep_alive() keeps the connection open and
     * esp_http_client.c never closes/reconnects between the two requests
     * (observed in the QEMU transport log: a single "Mock connect
     * succeeded" and two "Mock write" calls for this case). The mock's
     * request capture therefore never resets via mock_connect(), and
     * mock_write()'s FIFO-advance-on-boundary logic pops resp_200 for the
     * second write, so this assert depends on the connection staying open
     * across the redirect, not merely on Location being followed. */
    TEST_ASSERT_NOT_NULL(strstr(req, "GET /new"));   /* second request hit the new path */

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("disable_auto_redirect returns the 302 to the caller", "[esp_http_client][redirect][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_302;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/old",
        .disable_auto_redirect = true,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(302, esp_http_client_get_status_code(client));

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("max_redirection_count stops a redirect loop", "[esp_http_client][redirect][p0][negative]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_302;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    mock_http_transport_queue_response(mock, resp_302, 0);
    mock_http_transport_queue_response(mock, resp_302, 0);
    mock_http_transport_queue_response(mock, resp_302, 0);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/loop",
        .max_redirection_count = 2,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    /* characterization: master behavior, see refactor spec
     * with max_redirection_count=2, redirect_counter starts at 0 and is
     * checked (>= max) BEFORE each redirect is taken: 1st 302 -> counter
     * 0>=2 false, redirect, counter=1; 2nd 302 -> 1>=2 false, redirect,
     * counter=2; 3rd 302 -> 2>=2 true, esp_http_check_response() returns
     * ESP_ERR_HTTP_MAX_REDIRECT. Only 3 of the 4 queued 302 responses are
     * ever consumed - the loop guard trips before the 4th is read. */
    TEST_ASSERT_EQUAL(ESP_ERR_HTTP_MAX_REDIRECT, err);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
