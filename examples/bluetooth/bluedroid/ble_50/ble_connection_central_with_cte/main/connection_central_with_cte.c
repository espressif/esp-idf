/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */



/****************************************************************************
*
* This file is for gatt_security_client demo. It can scan ble device, connect one device that needs to be encrypted.
* run gatt_security_server demo, the gatt_security_client demo will automatically connect the gatt_security_server,
* then paring and bonding.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_ble_cte_api.h"

#define LOG_TAG             "CTE_TEST"
#define REMOTE_SERVICE_UUID   0x00FF
#define REMOTE_NOTIFY_UUID    0xFF01
#define EXT_SCAN_DURATION     0
#define EXT_SCAN_PERIOD       0

///Declare static functions
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
};

static bool connect = false;
static bool get_service = false;
static char remote_device_name[ESP_BLE_ADV_NAME_LEN_MAX] = "ESP_CON_CTE_TEST";

static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK | ESP_BLE_GAP_EXT_SCAN_CFG_CODE_MASK,
    .uncoded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
    .coded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
};

const esp_ble_conn_params_t phy_1m_conn_params = {
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .interval_min = 320,
    .interval_max = 320,
    .latency = 0,
    .supervision_timeout = 600,
    .min_ce_len  = 0,
    .max_ce_len = 0,
};
const esp_ble_conn_params_t phy_2m_conn_params = {
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .interval_min = 320,
    .interval_max = 320,
    .latency = 0,
    .supervision_timeout = 600,
    .min_ce_len  = 0,
    .max_ce_len = 0,
};
const esp_ble_conn_params_t phy_coded_conn_params = {
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .interval_min = 320, // 306-> 362Kbps
    .interval_max = 320,
    .latency = 0,
    .supervision_timeout = 600,
    .min_ce_len  = 0,
    .max_ce_len = 0,
};

static uint8_t antenna_ids[2] = {0x00, 0x01};
esp_ble_cte_recv_params_params_t cte_recv_params = {
    .conn_handle = 0xff,
    .sampling_en = ESP_BLE_CTE_SAMPLING_ENABLE,
    .slot_dur = ESP_BLE_CTE_SLOT_DURATION_2US,
    .switching_pattern_len = sizeof(antenna_ids),
    .antenna_ids = &antenna_ids[0],
};

static esp_ble_cte_req_en_params_t cte_conn_req_en = {
    .conn_handle = 0xff,
    .enable = ESP_BLE_CTE_SAMPLING_ENABLE,
    .cte_req_interval = 0x05,
    .req_cte_len = ESP_BLE_CTE_MAX_REQUESTED_CTE_LENGTH,
    .req_cte_Type = ESP_BLE_CTE_TYPE_AOA,
};

uint16_t cur_conn_hdl = 0xff;

#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t notify_char_handle;
    esp_bd_addr_t remote_bda;
};

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static const char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
   const char *key_str = NULL;
   switch(key_type) {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;

    }
     return key_str;
}

static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req)
{
   char *auth_str = NULL;
   switch(auth_req) {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        break;
   }

   return auth_str;
}

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(LOG_TAG, "GATT client register, status %u", p_data->reg.status);
        esp_ble_gap_config_local_privacy(true);
        break;
    case ESP_GATTC_CONNECT_EVT:
        ESP_LOGI(LOG_TAG, "Connected, cur_conn_hdl %d conn_id %d, remote "ESP_BD_ADDR_STR"", p_data->connect.conn_handle,
                    p_data->connect.conn_id, ESP_BD_ADDR_HEX(p_data->connect.remote_bda));
        cur_conn_hdl = p_data->connect.conn_handle;
        break;
    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK){
            ESP_LOGE(LOG_TAG, "Open failed, status %x", p_data->open.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Open successfully, MTU %d", p_data->open.mtu);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->open.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->open.conn_id);
        if (mtu_ret){
            ESP_LOGE(LOG_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        ESP_LOGI(LOG_TAG, "MTU exchange, status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        if (!param->cfg_mtu.status) {
            ESP_LOGI(LOG_TAG, "Set CTE connection receive params, conn_handle %d", cur_conn_hdl);
            cte_recv_params.conn_handle = cur_conn_hdl;
            esp_ble_cte_set_connection_receive_params(&cte_recv_params);
        }
        break;
    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        if (param->dis_srvc_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(LOG_TAG, "Service discover failed, status %d", param->dis_srvc_cmpl.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Service discover complete, conn_id %d", param->dis_srvc_cmpl.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &remote_filter_service_uuid);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(LOG_TAG, "Service search result, conn_id %x, is primary service %d", p_data->search_res.conn_id, p_data->search_res.is_primary);
        ESP_LOGI(LOG_TAG, "start handle %d end handle %d current handle value %d", p_data->search_res.start_handle, p_data->search_res.end_handle, p_data->search_res.srvc_id.inst_id);
        if (p_data->search_res.srvc_id.uuid.len == ESP_UUID_LEN_16 && p_data->search_res.srvc_id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
            ESP_LOGI(LOG_TAG, "UUID16: %x", p_data->search_res.srvc_id.uuid.uuid.uuid16);
            get_service = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(LOG_TAG, "Service search failed, status %x", p_data->search_cmpl.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Service search complete");
        if(p_data->search_cmpl.searched_service_source == ESP_GATT_SERVICE_FROM_REMOTE_DEVICE) {
            ESP_LOGI(LOG_TAG, "Get service information from remote device");
        } else if (p_data->search_cmpl.searched_service_source == ESP_GATT_SERVICE_FROM_NVS_FLASH) {
            ESP_LOGI(LOG_TAG, "Get service information from flash");
        } else {
            ESP_LOGI(LOG_TAG, "unknown service source");
        }

        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(LOG_TAG, "Notification register failed, status %x", p_data->reg_for_notify.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Notification register successfully");
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        ESP_LOGI(LOG_TAG, "Notification received, value ");
        ESP_LOG_BUFFER_HEX(LOG_TAG, p_data->notify.value, p_data->notify.value_len);
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(LOG_TAG, "Descriptor write failed, status %x", p_data->write.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Descriptor write successfully");
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(LOG_TAG, "Service change from "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(bda));
        break;
    }
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(LOG_TAG, "Characteristic write failed, status %x", p_data->write.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Characteristic Write successfully");
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(LOG_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(p_data->disconnect.remote_bda), p_data->disconnect.reason);
        connect = false;
        get_service = false;
        break;
    default:
        break;
    }
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(LOG_TAG, "Local privacy config failed, status %x", param->local_privacy_cmpl.status);
            break;
        }
        esp_err_t scan_ret = esp_ble_gap_set_ext_scan_params(&ext_scan_params);
        if (scan_ret){
            ESP_LOGE(LOG_TAG, "Set extend scan params error, error code = %x", scan_ret);
        }
        break;
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT: {
        if (param->set_ext_scan_params.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(LOG_TAG, "Extend scan parameters set failed, error status = %x", param->set_ext_scan_params.status);
            break;
        }
        //the unit of the duration is second
        esp_ble_gap_start_ext_scan(EXT_SCAN_DURATION, EXT_SCAN_PERIOD);
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        if (param->ext_scan_start.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(LOG_TAG, "Extended scanning start failed, status %x", param->ext_scan_start.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Extended scanning start successfully");
        break;
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* passkey request event */
        /* Call the following function to input the passkey which is displayed on the remote device */
        //esp_ble_passkey_reply(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, true, 0x00);
        ESP_LOGI(LOG_TAG, "Passkey request");
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        ESP_LOGI(LOG_TAG, "OOB request");
        uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_LOCAL_IR_EVT:                               /* BLE local IR event */
        ESP_LOGI(LOG_TAG, "Local identity root");
        break;
    case ESP_GAP_BLE_LOCAL_ER_EVT:                               /* BLE local ER event */
        ESP_LOGI(LOG_TAG, "Local encryption root");
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        /* send the positive(true) security response to the peer device to accept the security request.
        If not accept the security request, should send the security response with negative(false) accept value*/
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_NC_REQ_EVT:
        /* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
        show the passkey number to the user to confirm it with the number displayed by peer device. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        ESP_LOGI(LOG_TAG, "Numeric Comparison request, passkey %" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
        ///show the passkey number to the user to input it in the peer device.
        ESP_LOGI(LOG_TAG, "Passkey notify, passkey %06" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_KEY_EVT:
        //shows the ble key info share with peer device to the user.
        ESP_LOGI(LOG_TAG, "Key exchanged, key_type %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(LOG_TAG, "Authentication complete, addr_type %d, addr "ESP_BD_ADDR_STR"",
                 param->ble_security.auth_cmpl.addr_type, ESP_BD_ADDR_HEX(bd_addr));
        if (!param->ble_security.auth_cmpl.success) {
            ESP_LOGI(LOG_TAG, "Pairing failed, reason 0x%x",param->ble_security.auth_cmpl.fail_reason);
        } else {
            ESP_LOGI(LOG_TAG, "Pairing successfully, auth mode %s",esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
            // Enable CTE

        }
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT: {
        uint8_t *adv_name = NULL;
        uint8_t adv_name_len = 0;
        adv_name = esp_ble_resolve_adv_data_by_type(param->ext_adv_report.params.adv_data,
                                            param->ext_adv_report.params.adv_data_len,
                                            ESP_BLE_AD_TYPE_NAME_CMPL,
                                            &adv_name_len);
        if (!connect && strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
            // Note: If there are multiple devices with the same device name, the device may connect to an unintended one.
            // It is recommended to change the default device name to ensure it is unique.
            connect = true;
            esp_ble_gap_stop_ext_scan();
            ESP_LOGI(LOG_TAG, "Device found "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(param->ext_adv_report.params.addr));
            ESP_LOG_BUFFER_CHAR("Adv name", adv_name, adv_name_len);
            ESP_LOGI(LOG_TAG, "Stop extend scan and create aux open, primary_phy %d secondary phy %d", param->ext_adv_report.params.primary_phy, param->ext_adv_report.params.secondly_phy);

            // create gattc virtual connection
            esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
            memcpy(&creat_conn_params.remote_bda, param->ext_adv_report.params.addr, ESP_BD_ADDR_LEN);
            creat_conn_params.remote_addr_type = param->ext_adv_report.params.addr_type;
            creat_conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
            creat_conn_params.is_direct = true;
            creat_conn_params.is_aux = true;
            creat_conn_params.phy_mask = ESP_BLE_PHY_1M_PREF_MASK | ESP_BLE_PHY_2M_PREF_MASK | ESP_BLE_PHY_CODED_PREF_MASK;
            creat_conn_params.phy_1m_conn_params = &phy_1m_conn_params;
            creat_conn_params.phy_2m_conn_params = &phy_2m_conn_params;
            creat_conn_params.phy_coded_conn_params = &phy_coded_conn_params;
            esp_ble_gattc_enh_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, &creat_conn_params);
        }

        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        if (param->ext_scan_stop.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(LOG_TAG, "Scanning stop failed, status %x", param->ext_scan_stop.status);
            break;
        }
        ESP_LOGI(LOG_TAG, "Scanning stop successfully");
        break;

    default:
        break;
    }
}

static void cte_event_handler(esp_ble_cte_cb_event_t event, esp_ble_cte_cb_param_t *param)
{
    switch (event) {
        case ESP_BLE_CTE_SET_CONN_TRANS_PARAMS_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "CTE set connection transmit params, status %d", param->conn_trans_params_cmpl.status);
            break;
        case ESP_BLE_CTE_SET_CONN_RECV_PARAMS_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "CTE set connection receive params, status %d", param->conn_recv_params_cmpl.status);
            if (!param->conn_recv_params_cmpl.status) {
                cte_conn_req_en.conn_handle = cur_conn_hdl;
                ESP_LOGI(LOG_TAG, "Enable CTE request, conn_handle %d", cur_conn_hdl);
                esp_ble_cte_connection_cte_request_enable(&cte_conn_req_en);
            }
            break;
        case ESP_BLE_CTE_SET_CONN_REQ_ENABLE_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "CTE set connection request enable, status %d", param->conn_req_en_cmpl.status);
            break;
        case ESP_BLE_CTE_SET_CONN_RSP_ENABLE_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "CTE set connection response enable, status %d", param->conn_rsp_en_cmpl.status);
            break;
        case ESP_BLE_CTE_READ_ANT_INFOR_CMPL_EVT:
             ESP_LOGI(LOG_TAG, "CTE read antenna information, status %d antenna_num %d", param->read_ant_infor_cmpl.status, param->read_ant_infor_cmpl.num_antennae);
            break;
        case ESP_BLE_CTE_CONN_IQ_REPORT_EVT:
            ESP_LOGI(LOG_TAG, "conn_handle %d channel_idx %d rssi %d rssi_ant_id %d cte_type %d slot_dur %d pkt_status %d conn_evt_counter %d sample_count %d",
                                param->conn_iq_rpt.conn_handle, param->conn_iq_rpt.data_channel_idx, param->conn_iq_rpt.rssi,
                                param->conn_iq_rpt.rssi_ant_id, param->conn_iq_rpt.cte_type, param->conn_iq_rpt.slot_dur,
                                param->conn_iq_rpt.pkt_status, param->conn_iq_rpt.conn_evt_counter, param->conn_iq_rpt.sample_count);

            ESP_LOG_BUFFER_HEX("i_sample", &param->conn_iq_rpt.i_sample[0], param->conn_iq_rpt.sample_count);
            ESP_LOG_BUFFER_HEX("q_sample", &param->conn_iq_rpt.i_sample[0], param->conn_iq_rpt.sample_count);
            break;
        case ESP_BLE_CTE_REQUEST_FAILED_EVT:
            ESP_LOGI(LOG_TAG, "CTE connection request failed, conn_handle %d reason 0x%x",  param->req_failed_evt.conn_handle, param->req_failed_evt.reason);
            break;
        default:
            break;
    }

}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    ESP_LOGD(LOG_TAG, "EVT %d, gattc if %d", event, gattc_if);

    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(LOG_TAG, "Reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

void app_main(void)
{
    // Initialize NVS.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    #if CONFIG_EXAMPLE_CI_PIPELINE_ID
    memcpy(remote_device_name, esp_bluedroid_get_example_name(), sizeof(remote_device_name));
    #endif

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_cte_register_callback(cte_event_handler);
    if (ret){
        ESP_LOGE(LOG_TAG, "CTE callback register error, error code = %x", ret);
        return;
    }

    //register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret){
        ESP_LOGE(LOG_TAG, "%s gap register error, error code = %x", __func__, ret);
        return;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if(ret){
        ESP_LOGE(LOG_TAG, "%s gattc register error, error code = %x", __func__, ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(LOG_TAG, "%s gattc app register error, error code = %x", __func__, ret);
    }

    ret = esp_ble_gatt_set_local_mtu(500);
    if (ret){
        ESP_LOGE(LOG_TAG, "set local  MTU failed, error code = %x", ret);
    }

    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;     //bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
    /* If your BLE device act as a Slave, the init_key means you hope which types of key of the master should distribute to you,
    and the response key means which key you can distribute to the Master;
    If your BLE device act as a master, the response key means you hope which types of key of the slave should distribute to you,
    and the init key means which key you can distribute to the slave. */
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

}
