/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_auth.c
 * @brief P0 Critical Tests: 401 Basic auth retry within one perform()
 *
 * This test app's sdkconfig.ci.default sets
 * CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH=y (upstream Kconfig default is
 * "n" - Basic auth is unencrypted). That option is required for
 * esp_http_client_add_auth() to recognize a "WWW-Authenticate: Basic ..."
 * header at all: with it off, the "#if CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH"
 * branch that sets auth_type and process_again is compiled out entirely, and
 * a 401 falls into add_auth()'s unguarded "not supported" else-branch
 * instead - perform() returns ESP_ERR_NOT_SUPPORTED and the 401 is never
 * retried. That compiled-out shape is a static #ifdef fact (it cannot
 * silently regress) and is documented in the commit history for this file
 * rather than pinned as a permanent runtime test here. Stage 0 pins the
 * TRUE retry path instead, since the refactor's auth-retry handling and
 * counter-split fix build on it.
 *
 * This file characterizes:
 * - A 401 response carrying a WWW-Authenticate: Basic header is answered
 *   automatically with a retried request that carries an Authorization:
 *   Basic header, within a single esp_http_client_perform() call, when the
 *   URL embeds credentials, auth_type is set to HTTP_AUTH_TYPE_BASIC, and
 *   CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH is enabled.
 * - Separately: a caller who preconfigures auth_type = HTTP_AUTH_TYPE_BASIC
 *   with credentials in the URL gets an Authorization: Basic header on the
 *   very FIRST request, before any 401 is ever seen - this path
 *   (esp_http_client_prepare_basic_auth(), called from
 *   esp_http_client_prepare() whenever auth_type == BASIC and a username is
 *   set) has no Kconfig guard at all, unlike the WWW-Authenticate-driven
 *   detection in add_auth(). Preconfigured auth and auto-detected auth are
 *   gated independently in master.
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

#if !CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH
#error "These cases require CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH=y; keep every sdkconfig.ci.* with CUSTOM_TRANSPORT in sync"
#endif

static const char *resp_401 =
    "HTTP/1.1 401 Unauthorized\r\n"
    "WWW-Authenticate: Basic realm=\"Test\"\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

static const char *resp_200 =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 2\r\n"
    "\r\n"
    "ok";

TEST_CASE("401 with credentials retries with Authorization header", "[esp_http_client][auth][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_401;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);
    mock_http_transport_queue_response(mock, resp_200, 0);

    esp_http_client_config_t cfg = {
        .url = "http://user:pass@test-server.local/secure",
        .auth_type = HTTP_AUTH_TYPE_BASIC,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    /* characterization: master behavior, see refactor spec
     * With CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH enabled,
     * esp_http_check_response() matches status 401 and calls
     * esp_http_client_add_auth() (esp_http_client.c ~L1239-1240), which
     * finds a non-NULL auth_header populated from the WWW-Authenticate
     * response header, recognizes the "Basic" scheme, sets process_again=1,
     * and the client transparently retries within this same perform() call.
     * Confirmed as a checked fact (not just inferred from the two asserts
     * above): exactly 2 writes reach the transport, one per request. This
     * assumes one mock_write() call per request's header block, which held
     * for every GET-with-no-body case observed in this suite so far
     * (single-write requests); a refactor that splits header writes across
     * multiple esp_transport_write() calls would need to update this count
     * without necessarily changing behavior, so treat it as a coupled-to-
     * buffering assertion, not a load-bearing behavior pin by itself. */
    mock_http_transport_stats_t stats;
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_stats(mock, &stats));
    TEST_ASSERT_EQUAL(2, stats.write_calls);

    char req[2048];
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_last_request(mock, req, sizeof(req), NULL));
    /* characterization: master behavior, see refactor spec
     * resp_401 has Content-Length: 0 and no "Connection: close", so
     * http_should_keep_alive() keeps the connection open across the retry -
     * esp_http_client.c never calls esp_http_client_close() between the two
     * requests, the same keep-alive shape as Task 7's redirect flow.
     * Because the connection never closes, the mock's request-capture buffer
     * is never reset via mock_connect(); mock_write()'s FIFO-advance-on-
     * boundary logic pops resp_200 into the active buffer on the second
     * write, and the capture buffer resets at that same boundary. So this
     * assert observes only the SECOND (retried) request. Note this alone
     * cannot distinguish "the retry added Authorization" from "it was
     * already there" - see the companion test case below, which pins the
     * preconfigured-auth path in isolation with no 401 involved at all. */
    TEST_ASSERT_NOT_NULL(strstr(req, "Authorization: Basic "));

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("preconfigured Basic auth is sent before any 401 is seen", "[esp_http_client][auth][p0]")
{
    /* Companion to the retry test above: isolates the preconfigured-auth
     * path by never sending a 401 at all - only a single 200 response. */
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_200;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://user:pass@test-server.local/secure",
        .auth_type = HTTP_AUTH_TYPE_BASIC,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_perform(client));
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    char req[2048];
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_last_request(mock, req, sizeof(req), NULL));
    /* characterization: master behavior, see refactor spec
     * esp_http_client_prepare() (esp_http_client.c ~L800-803) calls
     * esp_http_client_prepare_basic_auth() whenever
     * connection_info.auth_type == HTTP_AUTH_TYPE_BASIC and a username is
     * set - both true here purely from cfg.auth_type and the URL's embedded
     * credentials - before the first request is ever sent, and with no
     * CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH guard around that call. So
     * Authorization: Basic is present on the only request this case sends,
     * even though no 401/WWW-Authenticate exchange happened. */
    TEST_ASSERT_NOT_NULL(strstr(req, "Authorization: Basic "));

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
