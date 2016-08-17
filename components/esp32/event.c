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

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "tcpip_adapter.h"

#define ESP32_WORKAROUND 1

#if CONFIG_WIFI_ENABLED

#ifdef ESP32_WORKAROUND

extern SemaphoreHandle_t stdio_mutex_tx;
#define os_printf(fmt, ...) do {\
    if (!stdio_mutex_tx) {\
        stdio_mutex_tx = xSemaphoreCreateMutex();\
    }\
\
    xSemaphoreTake(stdio_mutex_tx, portMAX_DELAY);\
    ets_printf(fmt, ##__VA_ARGS__);\
    xSemaphoreGive(stdio_mutex_tx);\
} while (0)

#endif

static xQueueHandle g_event_handler = NULL;
static system_event_cb_t g_event_handler_cb;

#define WIFI_DEBUG  os_printf
#define WIFI_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        WIFI_DEBUG("%s %d %s ret=%d\n", __FUNCTION__, __LINE__, (info), __err);\
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

static system_event_handle_t g_system_event_handle_table[] = {
    {SYSTEM_EVENT_WIFI_READY,          NULL},
    {SYSTEM_EVENT_SCAN_DONE,           NULL},
    {SYSTEM_EVENT_STA_START,           system_event_sta_start_handle_default},
    {SYSTEM_EVENT_STA_STOP,            system_event_sta_stop_handle_default},
    {SYSTEM_EVENT_STA_CONNECTED,       system_event_sta_connected_handle_default},
    {SYSTEM_EVENT_STA_DISCONNECTED,    system_event_sta_disconnected_handle_default},
    {SYSTEM_EVENT_STA_AUTHMODE_CHANGE, NULL},
    {SYSTEM_EVENT_AP_START,            system_event_ap_start_handle_default},
    {SYSTEM_EVENT_AP_STOP,             system_event_ap_stop_handle_default},
    {SYSTEM_EVENT_AP_STACONNECTED,     NULL},
    {SYSTEM_EVENT_AP_STADISCONNECTED,  NULL},
    {SYSTEM_EVENT_AP_PROBEREQRECVED,   NULL},
    {SYSTEM_EVENT_MAX,                 NULL},
};

esp_err_t system_event_ap_start_handle_default(system_event_t *event)
{
    struct ip_info ap_ip;
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
    struct ip_info sta_ip;
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
    WIFI_API_CALL_CHECK("esp_wifi_reg_rxcb", esp_wifi_reg_rxcb(WIFI_IF_STA, (wifi_rxcb_t)tcpip_adapter_sta_input), ESP_OK);

    tcpip_adapter_up(TCPIP_ADAPTER_IF_STA);
    tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);

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
        return (*g_event_handler_cb)(event);
    }

    return ESP_OK;
}

static esp_err_t esp_system_event_debug(system_event_t *event)
{
    system_event_sta_scan_done_t *scan_done;
    system_event_sta_connected_t *connected; 
    system_event_sta_disconnected_t *disconnected;
    system_event_sta_authmode_change_t *auth_change;
    system_event_ap_staconnected_t *staconnected;
    system_event_ap_stadisconnected_t *stadisconnected;
    system_event_ap_probe_req_rx_t *ap_probereqrecved;

    if (event == NULL) {
        os_printf("Error: event is null!\n");
        return ESP_FAIL;
    }

    os_printf("received event: ");
    switch (event->event_id) {
        case SYSTEM_EVENT_WIFI_READY:
            os_printf("SYSTEM_EVENT_WIFI_READY\n");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            scan_done = &event->event_info.scan_done;
            os_printf("SYSTEM_EVENT_SCAN_DONE\nstatus:%d, number:%d\n", \
                scan_done->status, scan_done->number);
            break;
        case SYSTEM_EVENT_STA_START:
            os_printf("SYSTEM_EVENT_STA_START\n");
            break;
        case SYSTEM_EVENT_STA_STOP:
            os_printf("SYSTEM_EVENT_STA_STOP\n");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            connected = &event->event_info.connected;
            os_printf("SYSTEM_EVENT_STA_CONNECTED\nssid:%s, ssid_len:%d, bssid:%02x:%02x:%02x:%02x:%02x:%02x, channel:%d\n", \
                connected->ssid, connected->ssid_len, connected->bssid[0], connected->bssid[0], connected->bssid[1], \
                connected->bssid[3], connected->bssid[4], connected->bssid[5], connected->channel);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            disconnected = &event->event_info.disconnected;
            os_printf("SYSTEM_EVENT_STA_DISCONNECTED\nssid:%s, ssid_len:%d, bssid:%02x:%02x:%02x:%02x:%02x:%02x, reason:%d\n", \
                disconnected->ssid, disconnected->ssid_len, disconnected->bssid[0], disconnected->bssid[0], disconnected->bssid[1], \
                disconnected->bssid[3], disconnected->bssid[4], disconnected->bssid[5], disconnected->reason);
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            auth_change = &event->event_info.auth_change;
            os_printf("SYSTEM_EVENT_STA_AUTHMODE_CHNAGE\nold_mode:%d, new_mode:%d\n", auth_change->old_mode, auth_change->new_mode);
            break;
        case SYSTEM_EVENT_AP_START:
            os_printf("SYSTEM_EVENT_AP_START\n");
            break;
        case SYSTEM_EVENT_AP_STOP:
            os_printf("SYSTEM_EVENT_AP_STOP\n");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            staconnected = &event->event_info.sta_connected;
            os_printf("SYSTEM_EVENT_AP_STACONNECTED\nmac:%02x:%02x:%02x:%02x:%02x:%02x, aid:%d\n", \
                staconnected->mac[0], staconnected->mac[0], staconnected->mac[1], \
                staconnected->mac[3], staconnected->mac[4], staconnected->mac[5], staconnected->aid);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            stadisconnected = &event->event_info.sta_disconnected;
            os_printf("SYSTEM_EVENT_AP_STADISCONNECTED\nmac:%02x:%02x:%02x:%02x:%02x:%02x, aid:%d\n", \
                stadisconnected->mac[0], stadisconnected->mac[0], stadisconnected->mac[1], \
                stadisconnected->mac[3], stadisconnected->mac[4], stadisconnected->mac[5], stadisconnected->aid);
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            ap_probereqrecved = &event->event_info.ap_probereqrecved;
            os_printf("SYSTEM_EVENT_AP_PROBEREQRECVED\nrssi:%d, mac:%02x:%02x:%02x:%02x:%02x:%02x\n", \
                ap_probereqrecved->rssi, ap_probereqrecved->mac[0], ap_probereqrecved->mac[0], ap_probereqrecved->mac[1], \
                ap_probereqrecved->mac[3], ap_probereqrecved->mac[4], ap_probereqrecved->mac[5]);
            break;
        default:
            os_printf("Error: no such kind of event!\n");
            break;
    }

    return ESP_OK;
}

static esp_err_t esp_system_event_handler(system_event_t *event)
{
    if (event == NULL) {
        os_printf("Error: event is null!\n");
        return ESP_FAIL;
    }

    esp_system_event_debug(event);
    if ((event->event_id < SYSTEM_EVENT_MAX) && (event->event_id == g_system_event_handle_table[event->event_id].event_id)){
        if (g_system_event_handle_table[event->event_id].event_handle){
            os_printf("enter default callback\n");
            g_system_event_handle_table[event->event_id].event_handle(event);
            os_printf("exit default callback\n");
        }
    } else {
        os_printf("mismatch or invalid event, id=%d\n", event->event_id);
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
            if (ret == ESP_FAIL)
                os_printf("esp wifi post event to user fail!\n");
        }
    }
}

system_event_cb_t esp_event_set_cb(system_event_cb_t cb)
{
    system_event_cb_t old_cb = g_event_handler_cb;
    g_event_handler_cb = cb;
    return old_cb;
}

esp_err_t esp_event_send(system_event_t *event)
{
    portBASE_TYPE ret;
    
    ret = xQueueSendToBack((xQueueHandle)g_event_handler, event, 0);
    if (pdPASS != ret){
        if (event) ets_printf("e=%d f\n", event->event_id);
        else ets_printf("e null\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void* esp_event_get_handler(void)
{
    return (void*)g_event_handler;
}

esp_err_t esp_event_init(system_event_cb_t cb)
{
    g_event_handler_cb = (system_event_cb_t)cb;
    g_event_handler = xQueueCreate(CONFIG_WIFI_ENENT_QUEUE_SIZE, sizeof(system_event_t));
    xTaskCreatePinnedToCore(esp_system_event_task, "eventTask", CONFIG_WIFI_EVENT_TASK_STACK_SIZE, NULL, 5, NULL, 0); // TODO: rearrange task priority
    return ESP_OK;
}

#endif
