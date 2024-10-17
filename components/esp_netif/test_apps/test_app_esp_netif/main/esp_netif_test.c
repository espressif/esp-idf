/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "unity_fixture.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_netif_net_stack.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_wifi_netif.h"
#include "sdkconfig.h"
#include "test_utils.h"
#include "memory_checks.h"
#include "lwip/netif.h"

//// This is a private esp-netif API, but include here to test it
bool esp_netif_is_netif_listed(esp_netif_t *esp_netif);

void create_delete_multiple_netifs(void)
{
    // interface key has to be a unique identifier
    const char* if_keys[] = { "if1", "if2", "if3", "if4", "if5", "if6", "if7", "if8", "if9" };
    const int nr_of_netifs = sizeof(if_keys)/sizeof(char*);
    esp_netif_t *netifs[nr_of_netifs];

    // create 10 wifi stations
    for (int i=0; i<nr_of_netifs; ++i) {
        esp_netif_inherent_config_t base_netif_config = { .if_key = if_keys[i]};
        esp_netif_config_t cfg = { .base = &base_netif_config, .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA };
        netifs[i] = esp_netif_new(&cfg);
        TEST_ASSERT_NOT_NULL(netifs[i]);
    }

    // there's no AP within created stations
    TEST_ASSERT_EQUAL(NULL, esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"));

    // check that the created netifs are correctly found by their interface keys and globally listed
    for (int i=0; i<nr_of_netifs; ++i) {
        TEST_ASSERT_EQUAL(netifs[i], esp_netif_get_handle_from_ifkey(if_keys[i]));
        TEST_ASSERT_TRUE(esp_netif_is_netif_listed(netifs[i]));
    }

    // destroy one by one and check it's been removed
    for (int i=0; i<nr_of_netifs; ++i) {
        esp_netif_destroy(netifs[i]);
        TEST_ASSERT_FALSE(esp_netif_is_netif_listed(netifs[i]));
    }
}


void get_from_if_key(void)
{
    // init default netif
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *esp_netif = esp_netif_new(&cfg);
    TEST_ASSERT_NOT_NULL(esp_netif);

    // check it's accessible by key
    TEST_ASSERT_EQUAL(esp_netif, esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));

    // destroy it
    esp_netif_destroy(esp_netif);

    // check it's also destroyed in list
    TEST_ASSERT_EQUAL(NULL, esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));

}
