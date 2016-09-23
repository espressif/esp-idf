// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "tcpip_adapter.h"
#include "esp_log.h"

#define ESP32_WORKAROUND 1

#if CONFIG_WIFI_ENABLED
static const char* TAG = "event";
static bool event_init_flag = false;
static xQueueHandle g_event_handler = NULL;
static system_event_cb_t g_event_handler_cb;
static void *g_event_ctx;

#define WIFI_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        ESP_LOGE(TAG, "%s %d %s ret=%d", __FUNCTION__, __LINE__, (info), __err);\
        return __err;\
    }\
} while(0)

typedef esp_err_t (*system_event_handle_fn_t)(system_event_t *e);

typedef struct {
    system_event_id_t event_id;
    system_event_handle_fn_t event_handle;
} system_event_handle_t;


static esp_err_t system_event_ap_start_handle_default(system_event_t *event);
static esp_err_t system_event_ap_stop_handle_default(system_event_t *event);
static esp_err_t system_event_sta_start_handle_default(system_event_t *event);
static esp_err_t system_event_sta_stop_handle_default(system_event_t *event);
static esp_err_t system_event_sta_connected_handle_default(system_event_t *event);
static esp_err_t system_event_sta_disconnected_handle_default(system_event_t *event);
static esp_err_t system_event_sta_got_ip_default(system_event_t *event);

static system_event_handle_t g_system_event_handle_table[] = {
    {SYSTEM_EVENT_WIFI_READY,          NULL},
    {SYSTEM_EVENT_SCAN_DONE,           NULL},
    {SYSTEM_EVENT_STA_START,           system_event_sta_start_handle_default},
    {SYSTEM_EVENT_STA_STOP,            system_event_sta_stop_handle_default},
    {SYSTEM_EVENT_STA_CONNECTED,       system_event_sta_connected_handle_default},
    {SYSTEM_EVENT_STA_DISCONNECTED,    system_event_sta_disconnected_handle_default},
    {SYSTEM_EVENT_STA_AUTHMODE_CHANGE, NULL},
    {SYSTEM_EVENT_STA_GOT_IP,          system_event_sta_got_ip_default},
    {SYSTEM_EVENT_AP_START,            system_event_ap_start_handle_default},
    {SYSTEM_EVENT_AP_STOP,             system_event_ap_stop_handle_default},
    {SYSTEM_EVENT_AP_STACONNECTED,     NULL},
    {SYSTEM_EVENT_AP_STADISCONNECTED,  NULL},
    {SYSTEM_EVENT_AP_PROBEREQRECVED,   NULL},
    {SYSTEM_EVENT_MAX,                 NULL},
};

static esp_err_t system_event_sta_got_ip_default(system_event_t *event)
{
    extern esp_err_t esp_wifi_set_sta_ip(void);
    WIFI_API_CALL_CHECK("esp_wifi_set_sta_ip", esp_wifi_set_sta_ip(), ESP_OK);

    ESP_LOGI(TAG, "ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR,
           IP2STR(&event->event_info.got_ip.ip_info.ip),
           IP2STR(&event->event_info.got_ip.ip_info.netmask),
           IP2STR(&event->event_info.got_ip.ip_info.gw));

    return ESP_OK;
}

esp_err_t system_event_ap_start_handle_default(system_event_t *event)
{
    tcpip_adapter_ip_info_t ap_ip;
    uint8_t ap_mac[6];

    WIFI_API_CALL_CHECK("esp_wifi_reg_rxcb", esp_wifi_reg_rxcb(WIFI_IF_AP, (wifi_rxcb_t)tcpip_adapter_ap_input), ESP_OK);
    WIFI_API_CALL_CHECK("esp_wifi_mac_get",  esp_wifi_get_mac(WIFI_IF_AP, ap_mac), ESP_OK);

    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip);
    tcpip_adapter_start(TCPIP_ADAPTER_IF_AP, ap_mac, &ap_ip);

    return ESP_OK;
}

esp_err_t system_event_ap_stop_handle_default(system_event_t *event)
{
    WIFI_API_CALL_CHECK("esp_wifi_reg_rxcb", esp_wifi_reg_rxcb(WIFI_IF_AP, NULL), ESP_OK);

    tcpip_adapter_stop(TCPIP_ADAPTER_IF_AP);

    return ESP_OK;
}

esp_err_t system_event_sta_start_handle_default(system_event_t *event)
{
    tcpip_adapter_ip_info_t sta_ip;
    uint8_t sta_mac[6];

    WIFI_API_CALL_CHECK("esp_wifi_mac_get",  esp_wifi_get_mac(WIFI_IF_STA, sta_mac), ESP_OK);
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);
    tcpip_adapter_start(TCPIP_ADAPTER_IF_STA, sta_mac, &sta_ip);

    return ESP_OK;
}

esp_err_t system_event_sta_stop_handle_default(system_event_t *event)
{
    tcpip_adapter_stop(TCPIP_ADAPTER_IF_STA);

    return ESP_OK;
}

esp_err_t system_event_sta_connected_handle_default(system_event_t *event)
{
    tcpip_adapter_dhcp_status_t status;

    WIFI_API_CALL_CHECK("esp_wifi_reg_rxcb", esp_wifi_reg_rxcb(WIFI_IF_STA, (wifi_rxcb_t)tcpip_adapter_sta_input), ESP_OK);

    tcpip_adapter_up(TCPIP_ADAPTER_IF_STA);

    tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA, &status);

    if (status == TCPIP_ADAPTER_DHCP_INIT) {
        tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
    } else if (status == TCPIP_ADAPTER_DHCP_STOPPED) {
        tcpip_adapter_ip_info_t sta_ip;

        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);

        if (!(ip4_addr_isany_val(sta_ip.ip) || ip4_addr_isany_val(sta_ip.netmask) || ip4_addr_isany_val(sta_ip.gw))) {
            system_event_t evt;

            //notify event
            evt.event_id = SYSTEM_EVENT_STA_GOT_IP;
            memcpy(&evt.event_info.got_ip.ip_info, &sta_ip, sizeof(tcpip_adapter_ip_info_t));

            esp_event_send(&evt);
        } else {
            ESP_LOGE(TAG, "invalid static ip");
        }
    }

    return ESP_OK;
}

esp_err_t system_event_sta_disconnected_handle_default(system_event_t *event)
{
    tcpip_adapter_down(TCPIP_ADAPTER_IF_STA);
    WIFI_API_CALL_CHECK("esp_wifi_reg_rxcb", esp_wifi_reg_rxcb(WIFI_IF_STA, NULL), ESP_OK);
    return ESP_OK;
}

static esp_err_t esp_wifi_post_event_to_user(system_event_t *event)
{
    if (g_event_handler_cb) {
        return (*g_event_handler_cb)(g_event_ctx, event);
    }

    return ESP_OK;
}

static esp_err_t esp_system_event_debug(system_event_t *event)
{
    if (event == NULL) {
        ESP_LOGE(TAG, "event is null!");
        return ESP_FAIL;
    }

    switch (event->event_id) {
    case SYSTEM_EVENT_WIFI_READY: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_WIFI_READY");
        break;
    }
    case SYSTEM_EVENT_SCAN_DONE: {
        system_event_sta_scan_done_t *scan_done = &event->event_info.scan_done;
        ESP_LOGD(TAG, "SYSTEM_EVENT_SCAN_DONE, status:%d, number:%d",  scan_done->status, scan_done->number);
        break;
    }
    case SYSTEM_EVENT_STA_START: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_START");
        break;
    }
    case SYSTEM_EVENT_STA_STOP: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_STOP");
        break;
    }
    case SYSTEM_EVENT_STA_CONNECTED: {
        system_event_sta_connected_t *connected = &event->event_info.connected;
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_CONNECTED, ssid:%s, ssid_len:%d, bssid:%02x:%02x:%02x:%02x:%02x:%02x, channel:%d, authmode:%d", \
                   connected->ssid, connected->ssid_len, connected->bssid[0], connected->bssid[0], connected->bssid[1], \
                   connected->bssid[3], connected->bssid[4], connected->bssid[5], connected->channel, connected->authmode);
        break;
    }
    case SYSTEM_EVENT_STA_DISCONNECTED: {
        system_event_sta_disconnected_t *disconnected = &event->event_info.disconnected;
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_DISCONNECTED, ssid:%s, ssid_len:%d, bssid:%02x:%02x:%02x:%02x:%02x:%02x, reason:%d", \
                   disconnected->ssid, disconnected->ssid_len, disconnected->bssid[0], disconnected->bssid[0], disconnected->bssid[1], \
                   disconnected->bssid[3], disconnected->bssid[4], disconnected->bssid[5], disconnected->reason);
        break;
    }
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: {
        system_event_sta_authmode_change_t *auth_change = &event->event_info.auth_change;
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_AUTHMODE_CHNAGE, old_mode:%d, new_mode:%d", auth_change->old_mode, auth_change->new_mode);
        break;
    }
    case SYSTEM_EVENT_STA_GOT_IP: {
        system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_GOTIP, ip:" IPSTR ", mask:" IPSTR ", gw:" IPSTR,
            IP2STR(&got_ip->ip_info.ip),
            IP2STR(&got_ip->ip_info.netmask),
            IP2STR(&got_ip->ip_info.gw));
        break;
    }
    case SYSTEM_EVENT_AP_START: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_START");
        break;
    }
    case SYSTEM_EVENT_AP_STOP: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STOP");
        break;
    }
    case SYSTEM_EVENT_AP_STACONNECTED: {
        system_event_ap_staconnected_t *staconnected = &event->event_info.sta_connected;
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STACONNECTED, mac:%02x:%02x:%02x:%02x:%02x:%02x, aid:%d", \
                   staconnected->mac[0], staconnected->mac[0], staconnected->mac[1], \
                   staconnected->mac[3], staconnected->mac[4], staconnected->mac[5], staconnected->aid);
        break;
    }
    case SYSTEM_EVENT_AP_STADISCONNECTED: {
        system_event_ap_stadisconnected_t *stadisconnected = &event->event_info.sta_disconnected;
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STADISCONNECTED, mac:%02x:%02x:%02x:%02x:%02x:%02x, aid:%d", \
                   stadisconnected->mac[0], stadisconnected->mac[0], stadisconnected->mac[1], \
                   stadisconnected->mac[3], stadisconnected->mac[4], stadisconnected->mac[5], stadisconnected->aid);
        break;
    }
    case SYSTEM_EVENT_AP_PROBEREQRECVED: {
        system_event_ap_probe_req_rx_t *ap_probereqrecved = &event->event_info.ap_probereqrecved;
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_PROBEREQRECVED, rssi:%d, mac:%02x:%02x:%02x:%02x:%02x:%02x", \
                   ap_probereqrecved->rssi, ap_probereqrecved->mac[0], ap_probereqrecved->mac[0], ap_probereqrecved->mac[1], \
                   ap_probereqrecved->mac[3], ap_probereqrecved->mac[4], ap_probereqrecved->mac[5]);
        break;
    }
    default: {
        ESP_LOGW(TAG, "no such kind of event!");
        break;
    }
    }

    return ESP_OK;
}

static esp_err_t esp_system_event_handler(system_event_t *event)
{
    if (event == NULL) {
        ESP_LOGE(TAG, "Error: event is null!");
        return ESP_FAIL;
    }

    esp_system_event_debug(event);
    if ((event->event_id < SYSTEM_EVENT_MAX) && (event->event_id == g_system_event_handle_table[event->event_id].event_id)) {
        if (g_system_event_handle_table[event->event_id].event_handle) {
            ESP_LOGV(TAG, "enter default callback");
            g_system_event_handle_table[event->event_id].event_handle(event);
            ESP_LOGV(TAG, "exit default callback");
        }
    } else {
        ESP_LOGE(TAG, "mismatch or invalid event, id=%d", event->event_id);
    }

    return esp_wifi_post_event_to_user(event);
}

static void esp_system_event_task(void *pvParameters)
{
    system_event_t evt;
    esp_err_t ret;

    while (1) {
        if (xQueueReceive(g_event_handler, &evt, portMAX_DELAY) == pdPASS) {
            ret = esp_system_event_handler(&evt);
            if (ret == ESP_FAIL) {
                ESP_LOGE(TAG, "post event to user fail!");
            }
        }
    }
}

system_event_cb_t esp_event_set_cb(system_event_cb_t cb, void *ctx)
{
    system_event_cb_t old_cb = g_event_handler_cb;

    g_event_handler_cb = cb;
    g_event_ctx = ctx;

    return old_cb;
}

esp_err_t esp_event_send(system_event_t *event)
{
    portBASE_TYPE ret;

    ret = xQueueSendToBack((xQueueHandle)g_event_handler, event, 0);

    if (pdPASS != ret) {
        if (event) {
            ESP_LOGE(TAG, "e=%d f", event->event_id);
        } else {
            ESP_LOGE(TAG, "e null");
        }
        return ESP_FAIL;
    }

    return ESP_OK;
}

void *esp_event_get_handler(void)
{
    return (void *)g_event_handler;
}

esp_err_t esp_event_init(system_event_cb_t cb, void *ctx)
{
    if (event_init_flag) {
        return ESP_FAIL;
    }

    g_event_handler_cb = cb;
    g_event_ctx = ctx;

    g_event_handler = xQueueCreate(CONFIG_SYSTEM_EVENT_QUEUE_SIZE, sizeof(system_event_t));

    xTaskCreatePinnedToCore(esp_system_event_task, "eventTask", ESP_TASKD_EVENT_STACK, NULL, ESP_TASKD_EVENT_PRIO, NULL, 0);
    return ESP_OK;
}

#endif
