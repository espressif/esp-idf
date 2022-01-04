/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Border Router Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_netif_types.h"
#include "esp_openthread_border_router.h"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_openthread_lock.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_net_stack.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "openthread/cli.h"

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_IP4_BIT = BIT0;
const int CONNECTED_IP6_BIT = BIT1;
static bool s_wifi_connected = false;

void esp_ot_wifi_netif_init(void)
{
    esp_netif_t *esp_netif = esp_netif_create_default_wifi_sta();
    assert(esp_netif);
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(wifi_event_group, CONNECTED_IP4_BIT);
        xEventGroupClearBits(wifi_event_group, CONNECTED_IP6_BIT);
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_IP4_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        esp_netif_create_ip6_linklocal(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_GOT_IP6) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_IP6_BIT);
    }
}

static void wifi_join(const char *ssid, const char *psk)
{
    static bool s_initialized = false;
    if (!s_initialized) {
        wifi_event_group = xEventGroupCreate();
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init(&cfg);
        ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &event_handler, NULL) );
        ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
        ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &event_handler, NULL) );
        ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &event_handler, NULL) );
        esp_wifi_set_storage(WIFI_STORAGE_RAM);
        esp_wifi_set_mode(WIFI_MODE_NULL);
        s_initialized = true;
    }
    esp_wifi_start();
    wifi_config_t wifi_config = { 0 };
    strncpy((char *) wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    if (psk) {
        strncpy((char *) wifi_config.sta.password, psk, sizeof(wifi_config.sta.password));
    }

    ESP_ERROR_CHECK( esp_wifi_set_ps(WIFI_PS_NONE) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    esp_wifi_connect();

    int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_IP4_BIT | CONNECTED_IP6_BIT, pdFALSE, pdTRUE, 10000 / portTICK_PERIOD_MS);

    if (((bits & CONNECTED_IP4_BIT) != 0) && ((bits & CONNECTED_IP6_BIT) != 0)) {
        s_wifi_connected = true;
        xEventGroupClearBits(wifi_event_group, CONNECTED_IP4_BIT);
        xEventGroupClearBits(wifi_event_group, CONNECTED_IP6_BIT);
        esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &event_handler);
        esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &event_handler);
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler);
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_GOT_IP6, &event_handler);
        vEventGroupDelete(wifi_event_group);
        s_initialized = false;
        esp_openthread_lock_acquire(portMAX_DELAY);
        esp_openthread_border_router_init();
        esp_openthread_lock_release();
        otCliOutputFormat("wifi sta is connected successfully\n");
    } else {
        esp_wifi_disconnect();
        esp_wifi_stop();
        ESP_LOGW("","Connection time out, please check your ssid & password, then retry.");
        otCliOutputFormat("wifi sta connection is failed\n");
    }
}

void esp_ot_process_wifi_cmd(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    char ssid[100] = "";
    char psk[100] = "";
    (void)(aContext);
    if (aArgsLength == 0) {
        otCliOutputFormat("---wifi parameter---\n");
        otCliOutputFormat("connect\n");
        otCliOutputFormat("-s                   :     wifi ssid\n");
        otCliOutputFormat("-p                   :     wifi psk\n");
        otCliOutputFormat("---example---\n");
        otCliOutputFormat("join a wifi:\nssid: threadcertAP \npsk: threadcertAP    :     wifi connect -s threadcertAP -p threadcertAP\n");
        otCliOutputFormat("state                :     get wifi state, disconnect or connect\n");
        otCliOutputFormat("---example---\n");
        otCliOutputFormat("get wifi state       :     wifi state\n");
    } else if (strcmp(aArgs[0], "connect") == 0) {
        for (int i = 1; i < aArgsLength; i++) {
            if (strcmp(aArgs[i], "-s") == 0) {
                i++;
                strcpy(ssid, aArgs[i]);
                otCliOutputFormat("ssid: %s\n", ssid);
            } else if (strcmp(aArgs[i], "-p") == 0) {
                i++;
                strcpy(psk, aArgs[i]);
                otCliOutputFormat("psk: %s\n", psk);
            }
        }
        if (!s_wifi_connected) {
            wifi_join(ssid, psk);
        } else {
            otCliOutputFormat("wifi has already connected\n");
        }
    } else if (strcmp(aArgs[0], "state") == 0) {
        if (s_wifi_connected) {
            otCliOutputFormat("connected\n");
        } else {
            otCliOutputFormat("disconnected\n");
        }
    } else {
        otCliOutputFormat("invalid commands\n");
    }
}
