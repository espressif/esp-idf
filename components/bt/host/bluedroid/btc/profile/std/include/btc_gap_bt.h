/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_GAP_BT_H__
#define __BTC_GAP_BT_H__

#include "common/bt_target.h"
#include "common/bt_defs.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "btc/btc_task.h"
#include "bta/utl.h"
#include "bta/bta_api.h"

#if (BTC_GAP_BT_INCLUDED == TRUE)
typedef enum {
    BTC_GAP_BT_SEARCH_DEVICES_EVT = 0,
    BTC_GAP_BT_SEARCH_SERVICES_EVT,
    BTC_GAP_BT_SEARCH_SERVICE_RECORD_EVT,
    BTC_GAP_BT_AUTH_CMPL_EVT,
    BTC_GAP_BT_PIN_REQ_EVT,
    BTC_GAP_BT_CFM_REQ_EVT,
    BTC_GAP_BT_KEY_NOTIF_EVT,
    BTC_GAP_BT_KEY_REQ_EVT,
    BTC_GAP_BT_READ_RSSI_DELTA_EVT,
    BTC_GAP_BT_CONFIG_EIR_DATA_EVT,
    BTC_GAP_BT_SET_AFH_CHANNELS_EVT,
    BTC_GAP_BT_READ_REMOTE_NAME_EVT,
    BTC_GAP_BT_MODE_CHG_EVT,
    BTC_GAP_BT_REMOVE_BOND_DEV_COMPLETE_EVT,
    BTC_GAP_BT_QOS_EVT,
    BTC_GAP_BT_SET_ACL_PKT_TYPES_EVT,
}btc_gap_bt_evt_t;

typedef enum {
    BTC_GAP_BT_ACT_SET_SCAN_MODE = 0,
    BTC_GAP_BT_ACT_START_DISCOVERY,
    BTC_GAP_BT_ACT_CANCEL_DISCOVERY,
    BTC_GAP_BT_ACT_GET_REMOTE_SERVICES,
    BTC_GAP_BT_ACT_GET_REMOTE_SERVICE_RECORD,
    BTC_GAP_BT_ACT_SET_COD,
    BTC_GAP_BT_ACT_READ_RSSI_DELTA,
    BTC_GAP_BT_ACT_REMOVE_BOND_DEVICE,
    BTC_GAP_BT_ACT_SET_PIN_TYPE,
    BTC_GAP_BT_ACT_PIN_REPLY,
    BTC_GAP_BT_ACT_SET_SECURITY_PARAM,
    BTC_GAP_BT_ACT_PASSKEY_REPLY,
    BTC_GAP_BT_ACT_CONFIRM_REPLY,
    BTC_GAP_BT_ACT_CONFIG_EIR,
    BTC_GAP_BT_ACT_SET_AFH_CHANNELS,
    BTC_GAP_BT_ACT_READ_REMOTE_NAME,
    BTC_GAP_BT_ACT_SET_QOS,
    BTC_GAP_BT_ACT_SET_ACL_PKT_TYPES,
} btc_gap_bt_act_t;

/* btc_bt_gap_args_t */
typedef union {
    // BTC_BT_GAP_ACT_SET_SCAN_MODE,
    struct set_bt_scan_mode_args {
        esp_bt_connection_mode_t c_mode;
        esp_bt_discovery_mode_t d_mode;
    } set_scan_mode;

    // BTC_GAP_BT_ACT_START_DISCOVERY
    struct start_disc_args {
        esp_bt_inq_mode_t mode;
        uint8_t inq_len;
        uint8_t num_rsps;
    } start_disc;

    // BTC_GAP_BT_ACT_GET_REMOTE_SERVICES
    bt_bdaddr_t bda;

    // BTC_GAP_BT_ACT_GET_REMOTE_SERVICE_RECORD
    struct get_rmt_srv_rcd_args {
        bt_bdaddr_t bda;
        esp_bt_uuid_t uuid;
    } get_rmt_srv_rcd;

    // BTC_GAP_BT_ACT_SET_COD
    struct set_cod_args {
       esp_bt_cod_t cod;
       esp_bt_cod_mode_t mode;
    } set_cod;

    //BTC_GAP_BT_ACT_READ_RSSI_DELTA,
    struct bt_read_rssi_delta_args {
        bt_bdaddr_t bda;
    } read_rssi_delta;

    // BTC_GAP_BT_ACT_REMOVE_BOND_DEVICE
    struct rm_bond_device_args {
       bt_bdaddr_t bda;
    } rm_bond_device;

    // BTC_GAP_BT_ACT_SET_PIN_TYPE
    struct set_pin_type_args {
        esp_bt_pin_type_t pin_type;
        uint8_t pin_code_len;
        esp_bt_pin_code_t pin_code;
    } set_pin_type;

    // BTC_GAP_BT_ACT_PIN_REPLY
    struct pin_reply_args {
        bt_bdaddr_t bda;
        bool accept;
        uint8_t pin_code_len;
        esp_bt_pin_code_t pin_code;
    } pin_reply;

    // BTC_GAP_BT_ACT_SET_SECURITY_PARAM
    struct set_sec_param_args {
        esp_bt_sp_param_t param_type;
        uint8_t len;
        uint8_t *value;
    } set_security_param;

    // BTC_GAP_BT_ACT_PASSKEY_REPLY
    struct passkey_reply_args {
       bt_bdaddr_t bda;
       bool accept;
       uint32_t passkey;
    } passkey_reply;

    // BTC_GAP_BT_ACT_CONFIRM_REPLY
    struct confirm_reply_args {
       bt_bdaddr_t bda;
       bool accept;
    } confirm_reply;

    // BTC_GAP_BT_ACT_CONFIG_EIR
    struct config_eir_args {
       esp_bt_eir_data_t eir_data;
    } config_eir;

    // BTC_GAP_BT_ACT_SET_AFH_CHANNELS
    struct set_afh_channels_args {
       esp_bt_gap_afh_channels channels;
    } set_afh_channels;

    // BTC_GAP_BT_ACT_READ_REMOTE_NAME
    bt_bdaddr_t rmt_name_bda;

    // BTC_GAP_BT_ACT_SET_QOS
    struct set_qos_args {
        bt_bdaddr_t bda;
        uint32_t t_poll;
    } set_qos;

    // BTC_GAP_BT_ACT_SET_ACL_PKT_TYPES
    struct set_acl_pkt_types_args {
        bt_bdaddr_t bda;
        uint16_t pkt_types;
    } set_acl_pkt_types;

} btc_gap_bt_args_t;

void btc_gap_bt_call_handler(btc_msg_t *msg);
void btc_gap_bt_cb_handler(btc_msg_t *msg);
void btc_gap_bt_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_gap_bt_arg_deep_free(btc_msg_t *msg);
void btc_gap_bt_busy_level_updated(uint8_t bl_flags);
void btc_gap_bt_init(void);
void btc_gap_bt_deinit(void);
void btc_gap_bt_acl_link_num_update(tBTA_DM_ACL_LINK_STAT *p_acl_link_stat);

esp_err_t btc_gap_bt_get_cod(esp_bt_cod_t *cod);
void btc_gap_bt_status_get(esp_bt_gap_profile_status_t *param);
#endif /* #if BTC_GAP_BT_INCLUDED */

#endif /* __BTC_GAP_BT_H__ */
