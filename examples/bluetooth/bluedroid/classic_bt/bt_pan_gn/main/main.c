/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>

#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_pan_api.h"
#include "bt_app_core_utils.h"
#include "pan_netif.h"

#define PAN_GN_TAG              "PAN_GN_DEMO"

/* Static IPv4 on the PAN GN subnet. */
#define PAN_GN_SUBNET_BASE      "192.168.100"
#define PAN_GN_HUB_HOST_OCTET   1

static uint8_t s_local_mac[6] = {0};

static void bdaddr_to_eth_mac(const uint8_t *bda, uint8_t *mac)
{
    /* PAN/BNEP uses BD_ADDR as the Ethernet MAC. Do not flip the U/L bit:
     * the GN stack forwards unicast by matching dst MAC to rem_bda. */
    memcpy(mac, bda, 6);
}

static void build_ipv4(char *buf, size_t len, int host_octet)
{
    snprintf(buf, len, "%s.%d", PAN_GN_SUBNET_BASE, host_octet);
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

static void setup_pan_netif(void)
{
    pan_netif_cfg_t cfg = {0};
    char ip[16];
    char mask[16];
    char gw[16];

    build_ipv4(ip, sizeof(ip), PAN_GN_HUB_HOST_OCTET);
    snprintf(mask, sizeof(mask), "255.255.255.0");
    strncpy(gw, ip, sizeof(gw) - 1);

    cfg.mac = s_local_mac;
    strncpy(cfg.ip_str, ip, sizeof(cfg.ip_str) - 1);
    strncpy(cfg.netmask_str, mask, sizeof(cfg.netmask_str) - 1);
    strncpy(cfg.gw_str, gw, sizeof(cfg.gw_str) - 1);

    ESP_ERROR_CHECK(pan_netif_init(&cfg));
    ESP_LOGI(PAN_GN_TAG, "Network: IP=%s/24", ip);
}

static void esp_hdl_pan_cb_evt(uint16_t event, void *p_param)
{
    esp_pan_cb_param_t *param = (esp_pan_cb_param_t *)p_param;
    char bda_str[18] = {0};

    switch (event) {
    case ESP_PAN_INIT_EVT:
        if (param->init.status == ESP_PAN_SUCCESS) {
            ESP_LOGI(PAN_GN_TAG, "GN hub ready, waiting for PANU clients...");
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        } else {
            ESP_LOGE(PAN_GN_TAG, "PAN init failed, status=%d", param->init.status);
        }
        break;

    case ESP_PAN_SET_ROLE_EVT:
        ESP_LOGI(PAN_GN_TAG, "Role registered: 0x%02x", param->set_role.role);
        break;

    case ESP_PAN_OPENING_EVT:
        ESP_LOGI(PAN_GN_TAG, "Incoming PANU, handle=%u peer=[%s]",
                 param->opening.handle,
                 bda2str(param->opening.remote_bda, bda_str, sizeof(bda_str)));
        break;

    case ESP_PAN_OPEN_EVT:
        if (param->open.status == ESP_PAN_SUCCESS) {
            ESP_LOGI(PAN_GN_TAG, "PANU connected handle=%u peer=[%s] local=0x%x peer=0x%x",
                     param->open.handle,
                     bda2str(param->open.remote_bda, bda_str, sizeof(bda_str)),
                     param->open.local_role, param->open.peer_role);
            pan_netif_on_connected(param->open.handle, param->open.remote_bda);
        } else {
            ESP_LOGE(PAN_GN_TAG, "Connect failed, status=%d", param->open.status);
        }
        break;

    case ESP_PAN_CLOSE_EVT:
        ESP_LOGW(PAN_GN_TAG, "PANU disconnected handle=%u", param->close.handle);
        pan_netif_on_disconnected(param->close.handle);
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
            ESP_LOGI(PAN_GN_TAG, "Paired with %s", param->auth_cmpl.device_name);
        } else {
            ESP_LOGE(PAN_GN_TAG, "Pairing failed, status=%d", param->auth_cmpl.stat);
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
    ESP_ERROR_CHECK(esp_bt_gap_set_device_name(CONFIG_EXAMPLE_PAN_GN_BT_NAME));

    bdaddr_to_eth_mac(esp_bt_dev_get_address(), s_local_mac);
    setup_pan_netif();

    esp_pan_cfg_t pan_cfg = ESP_PAN_DEFAULT_CONFIG();
    pan_cfg.role = ESP_PAN_ROLE_GN;
    ESP_ERROR_CHECK(esp_pan_init(&pan_cfg));

    ESP_LOGI(PAN_GN_TAG, "GN \"%s\" BD_ADDR [%s]",
             CONFIG_EXAMPLE_PAN_GN_BT_NAME,
             bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
    ESP_LOGI(PAN_GN_TAG, "PAN subnet: %s.%d/24 (static IP, no DHCP)",
             PAN_GN_SUBNET_BASE, PAN_GN_HUB_HOST_OCTET);
}
