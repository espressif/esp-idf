/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_pan_api.h"
#include "bt_app_core_utils.h"
#include "pan_netif.h"

#define PAN_NAP_TAG             "PAN_NAP_DEMO"

/* PAN downlink subnet for DHCP clients. */
#define PAN_NAP_SUBNET_BASE     "192.168.100"
#define PAN_NAP_HOST_OCTET      1

#define WIFI_CONNECTED_BIT      BIT0
#define WIFI_FAIL_BIT           BIT1

static EventGroupHandle_t s_wifi_event_group;
static esp_netif_t *s_wifi_sta_netif;
static int s_wifi_retry;
static bool s_wifi_uplink_ready;
static uint8_t s_pan_mac[6];

static void bdaddr_to_eth_mac(const uint8_t *bda, uint8_t *mac)
{
    memcpy(mac, bda, 6);
    mac[0] = (mac[0] | 0x02) & 0xFE;
}

static void build_ipv4(char *buf, size_t len, int host_octet)
{
    snprintf(buf, len, "%s.%d", PAN_NAP_SUBNET_BASE, host_octet);
}

static char *bda2str(uint8_t *bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
    return str;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        /*
         * Bound retries only during the initial wifi_uplink_init() wait.
         * After the uplink has been up once, keep reconnecting so PANU clients
         * do not permanently lose internet when the AP drops briefly.
         */
        if (s_wifi_uplink_ready) {
            ESP_LOGW(PAN_NAP_TAG, "Wi-Fi uplink lost, reconnecting...");
            esp_wifi_connect();
        } else if (s_wifi_retry < CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY) {
            esp_wifi_connect();
            s_wifi_retry++;
            ESP_LOGI(PAN_NAP_TAG, "Wi-Fi retry %d/%d", s_wifi_retry, CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
}

static void wifi_got_ip_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

    s_wifi_retry = 0;
    s_wifi_uplink_ready = true;
    ESP_LOGI(PAN_NAP_TAG, "Wi-Fi uplink IP: " IPSTR ", gw: " IPSTR,
             IP2STR(&event->ip_info.ip), IP2STR(&event->ip_info.gw));

    esp_netif_set_default_netif(s_wifi_sta_netif);

    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
}

static esp_err_t wifi_uplink_init(void)
{
    s_wifi_event_group = xEventGroupCreate();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    s_wifi_sta_netif = esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_got_ip_handler, NULL));

    wifi_config_t wifi_config = {0};
    /* ssid[32]/password[64] are fixed-size octets; do not reserve a trailing NUL. */
    strncpy((char *)wifi_config.sta.ssid, CONFIG_EXAMPLE_WIFI_SSID,
            sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, CONFIG_EXAMPLE_WIFI_PASSWORD,
            sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(PAN_NAP_TAG, "Connecting Wi-Fi SSID \"%s\" ...", CONFIG_EXAMPLE_WIFI_SSID);

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, portMAX_DELAY);
    if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(PAN_NAP_TAG, "Wi-Fi connect failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void setup_pan_netif(void)
{
    pan_netif_cfg_t cfg = {0};
    char ip[16];
    char mask[16];
    char gw[16];

    build_ipv4(ip, sizeof(ip), PAN_NAP_HOST_OCTET);
    snprintf(mask, sizeof(mask), "255.255.255.0");
    strncpy(gw, ip, sizeof(gw) - 1);

    cfg.mac = s_pan_mac;
    strncpy(cfg.ip_str, ip, sizeof(cfg.ip_str) - 1);
    strncpy(cfg.netmask_str, mask, sizeof(cfg.netmask_str) - 1);
    strncpy(cfg.gw_str, gw, sizeof(cfg.gw_str) - 1);

    ESP_ERROR_CHECK(pan_netif_init(&cfg));
}

static void esp_hdl_pan_cb_evt(uint16_t event, void *p_param)
{
    esp_pan_cb_param_t *param = (esp_pan_cb_param_t *)p_param;
    char bda_str[18] = {0};

    switch (event) {
    case ESP_PAN_INIT_EVT:
        if (param->init.status == ESP_PAN_SUCCESS) {
            ESP_LOGI(PAN_NAP_TAG, "PAN NAP ready, waiting for PANU peers...");
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        } else {
            ESP_LOGE(PAN_NAP_TAG, "PAN init failed, status=%d", param->init.status);
        }
        break;

    case ESP_PAN_SET_ROLE_EVT:
        ESP_LOGI(PAN_NAP_TAG, "NAP role registered: 0x%02x", param->set_role.role);
        break;

    case ESP_PAN_OPENING_EVT:
        ESP_LOGI(PAN_NAP_TAG, "Incoming PANU, handle=%u peer=[%s]",
                 param->opening.handle,
                 bda2str(param->opening.remote_bda, bda_str, sizeof(bda_str)));
        break;

    case ESP_PAN_OPEN_EVT:
        if (param->open.status == ESP_PAN_SUCCESS) {
            ESP_LOGI(PAN_NAP_TAG, "PANU connected handle=%u peer=[%s]",
                     param->open.handle,
                     bda2str(param->open.remote_bda, bda_str, sizeof(bda_str)));
            pan_netif_on_connected(param->open.handle, param->open.remote_bda);
        } else {
            ESP_LOGE(PAN_NAP_TAG, "PANU connect failed, status=%d", param->open.status);
        }
        break;

    case ESP_PAN_CLOSE_EVT:
        ESP_LOGW(PAN_NAP_TAG, "PANU disconnected handle=%u", param->close.handle);
        pan_netif_on_disconnected(param->close.handle);
        break;

    case ESP_PAN_CONG_EVT:
        /* Wi-Fi uplink is much faster than Classic BT; pause TX while BNEP is congested. */
        pan_netif_on_cong(param->cong.handle, param->cong.cong);
        break;

    default:
        break;
    }
}

static void esp_pan_cb(esp_pan_cb_event_t event, esp_pan_cb_param_t *param)
{
    switch (event) {
    case ESP_PAN_DATA_IND_EVT:
        /* Feed RX to lwIP in callback to avoid an extra app_core deep-copy.
         * esp_netif_receive is non-blocking by default (queues to tcpip thread). */
        pan_netif_input(param->data_ind.handle, param->data_ind.dst, param->data_ind.src,
                        param->data_ind.protocol, param->data_ind.data, param->data_ind.len);
        break;
    case ESP_PAN_INIT_EVT:
    case ESP_PAN_SET_ROLE_EVT:
    case ESP_PAN_OPENING_EVT:
    case ESP_PAN_OPEN_EVT:
    case ESP_PAN_CLOSE_EVT:
    case ESP_PAN_CONG_EVT:
        bt_app_work_dispatch(esp_hdl_pan_cb_evt, event, param, sizeof(esp_pan_cb_param_t), NULL, NULL);
        break;
    default:
        break;
    }
}

static void esp_hdl_bt_gap_cb_evt(uint16_t event, void *p_param)
{
    esp_bt_gap_cb_param_t *param = (esp_bt_gap_cb_param_t *)p_param;

    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(PAN_NAP_TAG, "Paired with %s", param->auth_cmpl.device_name);
        } else {
            ESP_LOGE(PAN_NAP_TAG, "Pairing failed, status=%d", param->auth_cmpl.stat);
        }
        break;

    case ESP_BT_GAP_CFM_REQ_EVT:
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;

    default:
        break;
    }
}

static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
    case ESP_BT_GAP_CFM_REQ_EVT:
        bt_app_work_dispatch(esp_hdl_bt_gap_cb_evt, event, param, sizeof(esp_bt_gap_cb_param_t), NULL, NULL);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    char bda_str[18] = {0};

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(wifi_uplink_init());
    pan_netif_set_uplink(s_wifi_sta_netif);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));

    esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bluedroid_init_with_cfg(&bluedroid_cfg));
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    bt_app_task_start_up();

    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(ESP_BT_SP_IOCAP_MODE, &iocap, sizeof(uint8_t));

    ESP_ERROR_CHECK(esp_bt_gap_register_callback(esp_bt_gap_cb));
    ESP_ERROR_CHECK(esp_pan_register_callback(esp_pan_cb));
    ESP_ERROR_CHECK(esp_bt_gap_set_device_name(CONFIG_EXAMPLE_PAN_NAP_BT_NAME));

    bdaddr_to_eth_mac(esp_bt_dev_get_address(), s_pan_mac);
    setup_pan_netif();

    esp_pan_cfg_t pan_cfg = ESP_PAN_DEFAULT_CONFIG();
    pan_cfg.role = ESP_PAN_ROLE_NAP;
    ESP_ERROR_CHECK(esp_pan_init(&pan_cfg));

    ESP_LOGI(PAN_NAP_TAG, "NAP \"%s\" BD_ADDR [%s]",
             CONFIG_EXAMPLE_PAN_NAP_BT_NAME,
             bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
    ESP_LOGI(PAN_NAP_TAG, "Wi-Fi uplink: %s | PAN DHCP: %s.%d/24",
             CONFIG_EXAMPLE_WIFI_SSID,
             PAN_NAP_SUBNET_BASE, PAN_NAP_HOST_OCTET);
    ESP_LOGI(PAN_NAP_TAG, "Pair phone/PANU and connect Bluetooth network to this device");
}
