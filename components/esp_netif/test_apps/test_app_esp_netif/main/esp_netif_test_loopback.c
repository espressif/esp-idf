/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "unity_fixture.h"
#include "esp_netif.h"
#include "memory_checks.h"
#include "esp_netif_test.h"

TEST_GROUP(esp_netif);

TEST_SETUP(esp_netif)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(esp_netif)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(esp_netif, create_delete_multiple_netifs)
{
    create_delete_multiple_netifs();
}

TEST(esp_netif, get_from_if_key)
{
    get_from_if_key();
}

TEST_GROUP_RUNNER(esp_netif)
{
    RUN_TEST_CASE(esp_netif, create_delete_multiple_netifs)
    RUN_TEST_CASE(esp_netif, get_from_if_key)
}

void app_main(void)
{
    UNITY_MAIN(esp_netif);
}

const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_sta = (esp_netif_netstack_config_t*)1;
