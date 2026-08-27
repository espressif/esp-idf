/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_async.c
 * @brief P0 characterization tests: async (is_async) behavior on master
 *
 * These tests document how esp_http_client_perform() behaves today when
 * is_async is set and the transport reports EAGAIN/would-block at various
 * stages (connect, read, write). They are characterization tests, not
 * behavioral specs: assert values reflect what master actually does.
 */

#include <string.h>
#include <errno.h>
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

TEST_CASE("async connect yields EAGAIN then completes", "[esp_http_client][async][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_200;
    mc.async_connect_polls = 2;   /* two CONNECTING polls before success */
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/",
        .is_async = true,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err;
    int eagain_count = 0;
    while ((err = esp_http_client_perform(client)) == ESP_ERR_HTTP_EAGAIN) {
        eagain_count++;
        TEST_ASSERT_LESS_THAN(20, eagain_count);   /* no livelock */
    }
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));
    TEST_ASSERT_EQUAL(2, eagain_count);            /* characterization: one per CONNECTING poll */

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

TEST_CASE("async read would-block yields EAGAIN mid-response", "[esp_http_client][async][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_200;
    mc.would_block_reads = 1;     /* first read returns -1/EAGAIN */
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/",
        .is_async = true,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err;
    int eagain_count = 0;
    while ((err = esp_http_client_perform(client)) == ESP_ERR_HTTP_EAGAIN) {
        eagain_count++;
        TEST_ASSERT_LESS_THAN(20, eagain_count);
    }
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));
    TEST_ASSERT_GREATER_OR_EQUAL(1, eagain_count);

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

/*
 * NOTE on the name/assertions below: the brief for this test originally
 * assumed write would-block gets the same async EAGAIN retry treatment as
 * connect/read (see the other two cases in this file). It does not.
 *
 * esp_http_client_request_send()'s header-write loop (esp_http_client.c,
 * the esp_transport_write() call that sends the first-line + headers)
 * treats *any* non-positive return from esp_transport_write() as a
 * permanent failure: it unconditionally calls esp_http_client_close() and
 * returns ESP_ERR_HTTP_WRITE_DATA, with no client->is_async / errno==EAGAIN
 * check at all. That check only exists on the "leftover data" continuation
 * path and on the POST-body write path (esp_http_client_write()) -- not on
 * the very first header write. So a would-block on the first transport
 * write is NOT retried in async mode: it aborts the request and tears down
 * the connection on the very first esp_http_client_perform() call, with no
 * EAGAIN loop at all. (Even if that check were reached, mock_write()'s
 * EAGAIN is set before esp_http_client_close()/ESP_LOGE run in the client,
 * and errno is not guaranteed to still read EAGAIN by the time a caller
 * downstream inspects it -- it was observed to be clobbered here.)
 *
 * The test name and assertions are updated to match this observed
 * master behavior rather than the originally assumed one.
 */
TEST_CASE("async write would-block on header send aborts the request", "[esp_http_client][async][p0]")
{
    mock_http_transport_config_t mc = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mc.response_data = resp_200;
    mc.would_block_writes = 1;
    esp_transport_handle_t mock = mock_http_transport_create(&mc);
    TEST_ASSERT_NOT_NULL(mock);

    esp_http_client_config_t cfg = {
        .url = "http://test-server.local/submit",
        .method = HTTP_METHOD_POST,
        .is_async = true,
        .transport = mock,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    TEST_ASSERT_NOT_NULL(client);
    esp_http_client_set_post_field(client, "k=v", 3);

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT(err == ESP_ERR_HTTP_EAGAIN || err == ESP_ERR_HTTP_WRITE_DATA);

    mock_http_transport_stats_t stats;
    mock_http_transport_get_stats(mock, &stats);
    // characterization: master behavior, see refactor spec
    TEST_ASSERT_EQUAL(0, stats.write_calls);   /* would-blocked write is never counted as completed */
    TEST_ASSERT_EQUAL(1, stats.close_calls);   /* connection force-closed on the failed header write */

    char req[2048];
    size_t req_len = 0;
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_last_request(mock, req, sizeof(req), &req_len));
    // characterization: master behavior, see refactor spec
    TEST_ASSERT_EQUAL(0, req_len);             /* nothing reached the wire: no body, so no duplicate either */

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
