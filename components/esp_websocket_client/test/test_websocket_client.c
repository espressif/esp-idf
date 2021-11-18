/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * This test code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <esp_websocket_client.h>

#include "unity.h"
#include "memory_checks.h"

static void test_leak_setup(const char * file, long line)
{
    printf("%s:%ld\n", file, line);
    test_utils_record_free_mem();
}

TEST_CASE("websocket init and deinit", "[websocket][leaks=0]")
{
    test_leak_setup(__FILE__, __LINE__);
    const esp_websocket_client_config_t websocket_cfg = {
            // no connection takes place, but the uri has to be valid for init() to succeed
            .uri = "ws://echo.websocket.org",
    };
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, client);
    esp_websocket_client_destroy(client);
}

TEST_CASE("websocket init with invalid url", "[websocket][leaks=0]")
{
    test_leak_setup(__FILE__, __LINE__);
    const esp_websocket_client_config_t websocket_cfg = {
            .uri = "INVALID",
    };
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    TEST_ASSERT_NULL(client);
}

TEST_CASE("websocket set url with invalid url", "[websocket][leaks=0]")
{
    test_leak_setup(__FILE__, __LINE__);
    const esp_websocket_client_config_t websocket_cfg = {};
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, client);
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_websocket_client_set_uri(client, "INVALID"));
    esp_websocket_client_destroy(client);
}
