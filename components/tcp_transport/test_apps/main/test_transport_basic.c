/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity_fixture.h"
#include "memory_checks.h"

#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"
#include "esp_log.h"


TEST_GROUP(transport_basic);

TEST_SETUP(transport_basic)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(transport_basic)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(transport_basic, transport_list_init_and_destroy)
{
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_list_destroy(transport_list));
}

TEST(transport_basic, transport_ssl_init_destroy_no_list)
{
    esp_transport_handle_t h = esp_transport_ssl_init();
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_destroy(h));
}

TEST(transport_basic, transport_ws_init_destroy_no_list)
{
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_handle_t ws = esp_transport_ws_init(tcp);
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_destroy(ws));
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_destroy(tcp));
}

TEST(transport_basic, transport_list_init_multiple_and_destroy)
{
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    esp_transport_list_add(transport_list, ssl, "ssl");
    esp_transport_handle_t ws = esp_transport_ws_init(tcp);
    esp_transport_list_add(transport_list, ws, "ws");
    esp_transport_handle_t wss = esp_transport_ws_init(ssl);
    esp_transport_list_add(transport_list, wss, "wss");
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_list_destroy(transport_list));
}

TEST_GROUP_RUNNER(transport_basic)
{
    RUN_TEST_CASE(transport_basic, transport_list_init_and_destroy);
    RUN_TEST_CASE(transport_basic, transport_ssl_init_destroy_no_list);
    RUN_TEST_CASE(transport_basic, transport_ws_init_destroy_no_list);
    RUN_TEST_CASE(transport_basic, transport_list_init_multiple_and_destroy);
}
