/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

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

#define PAN_TAG                 "PAN_PANU_DEMO"
#define EXAMPLE_DEVICE_NAME     "ESP_PAN_PANU"
#define ETH_ADDR_LEN            6

static const char remote_device_name[] = CONFIG_EXAMPLE_PEER_DEVICE_NAME;

static esp_bd_addr_t peer_bd_addr = {0};
static uint8_t peer_bdname_len = 0;
static char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1] = {0};

static uint8_t s_local_mac[ETH_ADDR_LEN] = {0};
static bool s_peer_found = false;

static const esp_bt_inq_mode_t inq_mode = ESP_BT_INQ_MODE_GENERAL_INQUIRY;
static const uint8_t inq_len = 5;
static const uint8_t inq_num_rsps = 0;

static char *bda2str(uint8_t *bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
    return str;
}

static void bdaddr_to_eth_mac(const uint8_t *bda, uint8_t *mac)
{
    memcpy(mac, bda, ETH_ADDR_LEN);
    mac[0] = (mac[0] | 0x02) & 0xFE;
}

static bool get_name_from_eir(uint8_t *eir, char *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}

static void start_pan_connect(void)
{
    char bda_str[18] = {0};
    ESP_LOGI(PAN_TAG, "Connect PANU -> NAP, peer [%s]",
             bda2str(peer_bd_addr, bda_str, sizeof(bda_str)));
    esp_err_t ret = esp_pan_connect(peer_bd_addr, ESP_PAN_ROLE_PANU, ESP_PAN_ROLE_NAP);
    if (ret != ESP_OK) {
        ESP_LOGE(PAN_TAG, "esp_pan_connect failed: %s", esp_err_to_name(ret));
        s_peer_found = false;
        esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);
    }
}

static void restart_peer_inquiry(const char *reason)
{
    ESP_LOGW(PAN_TAG, "%s, restart inquiry for \"%s\"", reason, remote_device_name);
    s_peer_found = false;
    esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);
}

static void esp_hdl_pan_cb_evt(uint16_t event, void *p_param)
{
    esp_pan_cb_param_t *param = (esp_pan_cb_param_t *)p_param;
    char bda_str[18] = {0};

    switch (event) {
    case ESP_PAN_INIT_EVT:
        if (param->init.status == ESP_PAN_SUCCESS) {
            ESP_LOGI(PAN_TAG, "PAN initialized, start inquiry for \"%s\"", remote_device_name);
            esp_bt_gap_set_device_name(EXAMPLE_DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);
        } else {
            ESP_LOGE(PAN_TAG, "ESP_PAN_INIT_EVT failed, status=%d", param->init.status);
        }
        break;

    case ESP_PAN_SET_ROLE_EVT:
        ESP_LOGI(PAN_TAG, "PAN role set: 0x%02x, status=%d",
                 param->set_role.role, param->set_role.status);
        break;

    case ESP_PAN_OPENING_EVT:
        ESP_LOGI(PAN_TAG, "PAN opening, handle=%u, peer=[%s]",
                 param->opening.handle,
                 bda2str(param->opening.remote_bda, bda_str, sizeof(bda_str)));
        break;

    case ESP_PAN_OPEN_EVT:
        if (param->open.status == ESP_PAN_SUCCESS) {
            ESP_LOGI(PAN_TAG, "PAN connected, handle=%u, peer=[%s], local_role=0x%x, peer_role=0x%x",
                     param->open.handle,
                     bda2str(param->open.remote_bda, bda_str, sizeof(bda_str)),
                     param->open.local_role, param->open.peer_role);
            pan_netif_on_connected(param->open.handle);
        } else {
            ESP_LOGE(PAN_TAG, "PAN connect failed, status=%d", param->open.status);
            restart_peer_inquiry("connect failed");
        }
        break;

    case ESP_PAN_CLOSE_EVT:
        ESP_LOGW(PAN_TAG, "PAN disconnected, handle=%u", param->close.handle);
        pan_netif_on_disconnected();
        restart_peer_inquiry("disconnected");
        break;

    case ESP_PAN_CONG_EVT:
        ESP_LOGI(PAN_TAG, "PAN congestion, handle=%u, cong=%d",
                 param->cong.handle, param->cong.cong);
        break;

    case ESP_PAN_DEINIT_EVT:
        ESP_LOGI(PAN_TAG, "PAN deinitialized, status=%d", param->deinit.status);
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
        pan_netif_input(param->data_ind.dst, param->data_ind.src, param->data_ind.protocol,
                        param->data_ind.data, param->data_ind.len);
        break;
    case ESP_PAN_INIT_EVT:
    case ESP_PAN_SET_ROLE_EVT:
    case ESP_PAN_OPENING_EVT:
    case ESP_PAN_OPEN_EVT:
    case ESP_PAN_CLOSE_EVT:
    case ESP_PAN_CONG_EVT:
    case ESP_PAN_DEINIT_EVT:
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
    case ESP_BT_GAP_DISC_RES_EVT:
        /* peer_bd_addr already saved in GAP callback before dispatch */
        ESP_LOGI(PAN_TAG, "Found NAP \"%s\"", peer_bdname);
        esp_bt_gap_cancel_discovery();
        start_pan_connect();
        break;

    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED && !s_peer_found) {
            ESP_LOGW(PAN_TAG, "NAP \"%s\" not found, restart inquiry", remote_device_name);
            esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);
        }
        break;

    case ESP_BT_GAP_AUTH_CMPL_EVT:
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(PAN_TAG, "Pairing success: %s", param->auth_cmpl.device_name);
        } else {
            ESP_LOGE(PAN_TAG, "Pairing failed, status=%d", param->auth_cmpl.stat);
        }
        break;

    case ESP_BT_GAP_PIN_REQ_EVT:
        ESP_LOGI(PAN_TAG, "PIN requested, reply 1234");
        {
            esp_bt_pin_code_t pin_code = {'1', '2', '3', '4'};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;

    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(PAN_TAG, "SSP confirm: %" PRIu32, param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;

    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(PAN_TAG, "SSP passkey: %06" PRIu32, param->key_notif.passkey);
        break;

    default:
        break;
    }
}

static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_DISC_RES_EVT:
        /*
         * Discovery properties (EIR) are only valid in the GAP callback context.
         * Match the peer name here, then dispatch API work (cancel/connect) to app_core.
         */
        for (int i = 0; i < param->disc_res.num_prop; i++) {
            if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_EIR
                    && get_name_from_eir(param->disc_res.prop[i].val, peer_bdname, &peer_bdname_len)) {
                if (!s_peer_found
                        && strlen(remote_device_name) == peer_bdname_len
                        && strncmp(peer_bdname, remote_device_name, peer_bdname_len) == 0) {
                    s_peer_found = true;
                    memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
                    bt_app_work_dispatch(esp_hdl_bt_gap_cb_evt, event, NULL, 0, NULL, NULL);
                }
            }
        }
        break;

    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
    case ESP_BT_GAP_AUTH_CMPL_EVT:
    case ESP_BT_GAP_PIN_REQ_EVT:
    case ESP_BT_GAP_CFM_REQ_EVT:
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        bt_app_work_dispatch(esp_hdl_bt_gap_cb_evt, event, param, sizeof(esp_bt_gap_cb_param_t), NULL, NULL);
        break;

    default:
        break;
    }
}

void app_main(void)
{
    esp_err_t ret;
    char bda_str[18] = {0};

    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

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

    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

    bdaddr_to_eth_mac(esp_bt_dev_get_address(), s_local_mac);
    ESP_ERROR_CHECK(pan_netif_init(s_local_mac));

    esp_pan_cfg_t pan_cfg = ESP_PAN_DEFAULT_CONFIG();
    pan_cfg.role = ESP_PAN_ROLE_PANU;
    ESP_ERROR_CHECK(esp_pan_init(&pan_cfg));

    ESP_LOGI(PAN_TAG, "Own BD_ADDR [%s]", bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
    ESP_LOGI(PAN_TAG, "Looking for NAP \"%s\"", remote_device_name);
    ESP_LOGI(PAN_TAG, "Will ping %s after DHCP", CONFIG_EXAMPLE_PING_TARGET);
}
