/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_GAP_BLE_H__
#define __BTC_GAP_BLE_H__

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#if BTC_DYNAMIC_MEMORY == TRUE
#include "bta/bta_api.h"
extern tBTA_BLE_ADV_DATA *gl_bta_adv_data_ptr;
extern tBTA_BLE_ADV_DATA *gl_bta_scan_rsp_data_ptr;
#define gl_bta_adv_data     (*gl_bta_adv_data_ptr)
#define gl_bta_scan_rsp_data    (*gl_bta_scan_rsp_data_ptr)
#endif

#define BLE_ISVALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)))

typedef enum {
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_CFG_ADV_DATA = 0,
    BTC_GAP_BLE_ACT_SET_SCAN_PARAM,
    BTC_GAP_BLE_ACT_START_SCAN,
    BTC_GAP_BLE_ACT_STOP_SCAN,
    BTC_GAP_BLE_ACT_START_ADV,
    BTC_GAP_BLE_ACT_STOP_ADV,
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM = 6,
    BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN,
    BTC_GAP_BLE_ACT_SET_RAND_ADDRESS,
    BTC_GAP_BLE_ACT_CLEAR_RAND_ADDRESS,
    BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY,
    BTC_GAP_BLE_ACT_CONFIG_LOCAL_ICON,
    BTC_GAP_BLE_ACT_UPDATE_WHITE_LIST,
    BTC_GAP_BLE_ACT_CLEAR_WHITE_LIST,
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_SET_CONN_PARAMS,
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_SET_DEV_NAME = 15,
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW,
    BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW,
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_READ_RSSI = 18,
    BTC_GAP_BLE_SET_ENCRYPTION_EVT,
    BTC_GAP_BLE_SET_SECURITY_PARAM_EVT,
    BTC_GAP_BLE_SECURITY_RSP_EVT,
    BTC_GAP_BLE_PASSKEY_REPLY_EVT,
    BTC_GAP_BLE_CONFIRM_REPLY_EVT,
    BTC_GAP_BLE_DISCONNECT_EVT,
    BTC_GAP_BLE_REMOVE_BOND_DEV_EVT,
    BTC_GAP_BLE_OOB_REQ_REPLY_EVT,
    BTC_GAP_BLE_SC_OOB_REQ_REPLY_EVT,
    BTC_GAP_BLE_SC_CR_OOB_DATA_EVT,
    BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST,
    BTC_GAP_BLE_SET_AFH_CHANNELS,
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_READ_PHY,
    BTC_GAP_BLE_SET_PREFERED_DEF_PHY,
    BTC_GAP_BLE_SET_DEF_PHY,
    BTC_GAP_BLE_SET_EXT_ADV_RAND_ADDR,
    BTC_GAP_BLE_SET_EXT_ADV_PARAMS,
    BTC_GAP_BLE_CFG_EXT_ADV_DATA_RAW,
    BTC_GAP_BLE_CFG_EXT_SCAN_RSP_DATA_RAW,
    BTC_GAP_BLE_EXT_ADV_START,
    BTC_GAP_BLE_EXT_ADV_STOP,
    BTC_GAP_BLE_EXT_ADV_SET_REMOVE,
    BTC_GAP_BLE_EXT_ADV_SET_CLEAR,
    BTC_GAP_BLE_SET_PERIODIC_ADV_PARAMS,
    BTC_GAP_BLE_CFG_PERIODIC_ADV_DATA_RAW,
    BTC_GAP_BLE_PERIODIC_ADV_START,
    BTC_GAP_BLE_PERIODIC_ADV_STOP,
    BTC_GAP_BLE_PERIODIC_ADV_CREATE_SYNC,
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL,
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE,
    BTC_GAP_BLE_PERIODIC_ADV_ADD_DEV_TO_LIST,
    BTC_GAP_BLE_PERIODIC_REMOVE_ADD_DEV_FROM_LIST,
    BTC_GAP_BLE_PERIODIC_CLEAR_DEV,
    BTC_GAP_BLE_SET_EXT_SCAN_PARAMS,
    BTC_GAP_BLE_START_EXT_SCAN,
    BTC_GAP_BLE_STOP_EXT_SCAN,
    BTC_GAP_BLE_SET_EXT_PEFER_CONNET_PARAMS,
    BTC_GAP_BLE_DTM_ENH_TX_START,
    BTC_GAP_BLE_DTM_ENH_RX_START,
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_GET_DEV_NAME,
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    BTC_GAP_BLE_PERIODIC_ADV_RECV_ENABLE,
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_TRANS,
    BTC_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS,
    BTC_GAP_BLE_SET_PERIODIC_ADV_SYNC_TRANS_PARAMS,
#endif //#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_DTM_TX_START,
    BTC_GAP_BLE_DTM_RX_START,
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_DTM_STOP,
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_CLEAR_ADV,
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT,
    BTC_GAP_BLE_ACT_ADD_DEVICE_TO_RESOLVING_LIST,
    BTC_GAP_BLE_ACT_VENDOR_HCI_CMD_EVT,
} btc_gap_ble_act_t;

/* btc_ble_gap_args_t */
typedef union {
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    //BTC_GAP_BLE_ACT_CFG_ADV_DATA = 0,
    struct config_adv_data_args {
        esp_ble_adv_data_t adv_data;
    } cfg_adv_data;
    //BTC_GAP_BLE_ACT_SET_SCAN_PARAM,
    struct set_scan_params_args {
        esp_ble_scan_params_t  scan_params;
    } set_scan_param;
    //BTC_GAP_BLE_ACT_START_SCAN,
    struct start_scan_args {
        uint32_t duration;
    } start_scan;
    //BTC_GAP_BLE_ACT_STOP_SCAN, no args
    //BTC_GAP_BLE_ACT_START_ADV,
    struct start_adv_args {
        esp_ble_adv_params_t adv_params;
    } start_adv;
    //BTC_GAP_BLE_ACT_STOP_ADV, no args
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    //BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM,
    struct conn_update_params_args {
        esp_ble_conn_update_params_t conn_params;
    } conn_update_params;
    //BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN
    struct set_pkt_data_len_args {
        esp_bd_addr_t remote_device;
        uint16_t tx_data_length;
    } set_pkt_data_len;
    //BTC_GAP_BLE_ACT_SET_RAND_ADDRESS,
    struct set_rand_addr_args {
        esp_bd_addr_t rand_addr;
    } set_rand_addr;
    // BTC_GAP_BLE_ACT_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT
    struct set_rpa_timeout_args {
        uint16_t rpa_timeout;
    } set_rpa_timeout;
    //BTC_GAP_BLE_ACT_ADD_DEVICE_TO_RESOLVING_LIST
    struct add_dev_to_resolving_list_args {
        esp_bd_addr_t addr;
        uint8_t addr_type;
        uint8_t irk[PEER_IRK_LEN];
    } add_dev_to_resolving_list;
    //BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY,
    struct cfg_local_privacy_args {
        bool privacy_enable;
    } cfg_local_privacy;
    //BTC_GAP_BLE_ACT_CONFIG_LOCAL_ICON,
    struct cfg_local_icon_args {
        uint16_t icon;
    } cfg_local_icon;
    //BTC_GAP_BLE_ACT_UPDATE_WHITE_LIST
    struct update_white_list_args {
        bool add_remove;
        esp_bd_addr_t remote_bda;
        esp_ble_wl_addr_type_t wl_addr_type;
    } update_white_list;
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    //BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST
    struct update_duplicate_exceptional_list_args {
        uint8_t  subcode;
        uint32_t  info_type;
        esp_duplicate_info_t device_info;
    } update_duplicate_exceptional_list;
    //BTC_GAP_BLE_ACT_SET_CONN_PARAMS
    struct set_conn_params_args {
        esp_bd_addr_t bd_addr;
        uint16_t min_conn_int;
        uint16_t max_conn_int;
        uint16_t slave_latency;
        uint16_t supervision_tout;
    } set_conn_params;
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    //BTC_GAP_BLE_ACT_SET_DEV_NAME,
    struct set_dev_name_args {
#define ESP_GAP_DEVICE_NAME_MAX (32)
        char *device_name;
    } set_dev_name;
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    //BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW,
    struct config_adv_data_raw_args {
        uint8_t *raw_adv;
        uint32_t raw_adv_len;
    } cfg_adv_data_raw;
    //BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW,
    struct config_scan_rsp_data_raw_args {
        uint8_t *raw_scan_rsp;
        uint32_t raw_scan_rsp_len;
    } cfg_scan_rsp_data_raw;
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    //BTC_GAP_BLE_SET_ENCRYPTION_EVT
    struct set_encryption_args {
        esp_bd_addr_t bd_addr;
        esp_ble_sec_act_t sec_act;
    } set_encryption;
    //BTC_GAP_BLE_SET_SECURITY_PARAM_EVT
    struct set_security_param_args {
        esp_ble_sm_param_t param_type;
        uint8_t len;
        uint8_t *value;
    } set_security_param;
    //BTC_GAP_BLE_SECURITY_RSP_EVT
    struct enc_rsp_args {
        esp_bd_addr_t bd_addr;
        bool accept;
    } sec_rsp;
    //BTC_GAP_BLE_PASSKEY_REPLY_EVT
    struct enc_passkey_reply_args {
        esp_bd_addr_t bd_addr;
        bool accept;
        uint32_t passkey;
    } enc_passkey_replay;
    //BTC_GAP_BLE_CONFIRM_REPLY_EVT
    struct enc_comfirm_reply_args {
        esp_bd_addr_t bd_addr;
        bool accept;
    } enc_comfirm_replay;
    //BTC_GAP_BLE_OOB_DATA_REPLY_EVT
    struct oob_req_reply_args {
        esp_bd_addr_t bd_addr;
        uint8_t len;
        uint8_t *p_value;
    } oob_req_reply;
    struct sc_oob_req_reply_args {
        esp_bd_addr_t bd_addr;
        uint8_t *p_c;
        uint8_t *p_r;
    } sc_oob_req_reply;
    //BTC_GAP_BLE_DISCONNECT_EVT
    struct disconnect_args {
        esp_bd_addr_t remote_device;
    } disconnect;
    //BTC_GAP_BLE_REMOVE_BOND_DEV_EVT
    struct remove_bond_device_args {
        esp_bd_addr_t bd_addr;
    } remove_bond_device;
    //BTC_GAP_BLE_ACT_READ_RSSI
    struct read_rssi_args {
        esp_bd_addr_t remote_addr;
    } read_rssi;
    // BTC_GAP_BLE_SET_AFH_CHANNELS
    struct set_channels_args {
       esp_gap_ble_channels channels;
    } set_channels;
    struct dtm_tx_start_args {
        uint8_t tx_channel;
        uint8_t len_of_data;
        uint8_t pkt_payload;
    } dtm_tx_start;
    struct dtm_rx_start_args {
        uint8_t rx_channel;
    } dtm_rx_start;
    //BTC_DEV_VENDOR_HCI_CMD_EVT
    struct vendor_cmd_send_args {
        uint16_t  opcode;
        uint8_t  param_len;
        uint8_t *p_param_buf;
    } vendor_cmd_send;
} btc_ble_gap_args_t;
#if (BLE_50_FEATURE_SUPPORT == TRUE)

typedef union {
    struct read_phy_args {
        esp_bd_addr_t bd_addr;
    } read_phy;

    struct set_perf_def_phy_args {
        esp_ble_gap_phy_mask_t tx_phy_mask;
        esp_ble_gap_phy_mask_t rx_phy_mask;
    } set_perf_def_phy;

    struct set_def_phy_args {
        esp_bd_addr_t bd_addr;
        esp_ble_gap_all_phys_t all_phys_mask;
        esp_ble_gap_phy_mask_t tx_phy_mask;
        esp_ble_gap_phy_mask_t rx_phy_mask;
        uint16_t phy_options;
    } set_def_phy;

    struct ext_adv_set_rand_addr_args {
        uint8_t instance;
        esp_bd_addr_t rand_addr;
    } ext_adv_set_rand_addr;

    struct ext_adv_set_params_args {
        uint8_t instance;
        esp_ble_gap_ext_adv_params_t params;
    } ext_adv_set_params;

    struct ext_adv_cfg_data_args {
        uint8_t instance;
        uint16_t length;
        uint8_t *data;
    } ext_adv_cfg_data;

    struct ext_adv_cfg_scan_rsp_args {
        uint8_t instance;
        uint16_t length;
        uint8_t *data;
    } cfg_scan_rsp;

    struct ext_adv_start_args {
        uint8_t num_adv;
        esp_ble_gap_ext_adv_t *ext_adv;
    } ext_adv_start;

    struct ext_adv_stop_args {
        uint8_t num_adv;
        uint8_t *ext_adv_inst;
    } ext_adv_stop;

    struct ext_adv_set_remove_args {
        uint8_t instance;
    } ext_adv_set_remove;

    struct peridic_adv_set_params_args {
        uint8_t instance;
        esp_ble_gap_periodic_adv_params_t params;
    } peridic_adv_set_params;

    struct periodic_adv_cfg_data_args {
        uint8_t instance;
        uint16_t len;
        uint8_t *data;
        bool only_update_did;
    } periodic_adv_cfg_data;

    struct periodic_adv_start_args {
        bool    include_adi;
        uint8_t instance;
    } periodic_adv_start;

    struct periodic_adv_stop_args {
        uint8_t instance;
    } periodic_adv_stop;

    struct periodic_adv_create_sync_args {
        esp_ble_gap_periodic_adv_sync_params_t params;
    } periodic_adv_create_sync;

    struct periodic_adv_sync_term_args {
        uint16_t sync_handle;
    } periodic_adv_sync_term;

    struct periodic_adv_add_dev_args {
        esp_ble_addr_type_t addr_type;
        esp_bd_addr_t addr;
        uint16_t sid;
    } periodic_adv_add_dev;

    struct periodic_adv_remove_dev_args {
        esp_ble_addr_type_t addr_type;
        esp_bd_addr_t addr;
        uint16_t sid;
    } periodic_adv_remove_dev;

    struct set_ext_scan_params_args {
        esp_ble_ext_scan_params_t params;
    } set_ext_scan_params;

    struct start_ext_scan_args {
        uint32_t duration;
        uint16_t period;
    } start_ext_scan;

    struct set_ext_conn_params_args {
        esp_bd_addr_t addr;
        uint8_t phy_mask;
        esp_ble_gap_conn_params_t phy_1m_conn_params;
        esp_ble_gap_conn_params_t phy_2m_conn_params;
        esp_ble_gap_conn_params_t phy_coded_conn_params;
    } set_ext_conn_params;

    struct ext_conn_args {
        esp_ble_addr_type_t own_addr_type;
        esp_bd_addr_t peer_addr;
    } ext_conn;

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    struct periodic_adv_recv_en_args {
        uint16_t sync_handle;
        uint8_t enable;
    } periodic_adv_recv_en;

    struct periodic_adv_sync_trans_args {
        esp_bd_addr_t addr;
        uint16_t service_data;
        uint16_t sync_handle;
    } periodic_adv_sync_trans;

    struct periodic_adv_set_info_trans_args {
        esp_bd_addr_t addr;
        uint16_t service_data;
        uint16_t adv_handle;
    } periodic_adv_set_info_trans;

    struct set_periodic_adv_sync_trans_params_args {
        esp_bd_addr_t addr;
        esp_ble_gap_past_params_t params;
    } set_periodic_adv_sync_trans_params;
#endif
     struct dtm_enh_tx_start_args {
        uint8_t tx_channel;
        uint8_t len_of_data;
        uint8_t pkt_payload;
        uint8_t phy;
    } dtm_enh_tx_start;
    struct dtm_enh_rx_start_args {
        uint8_t rx_channel;
        uint8_t phy;
        uint8_t modulation_index;
    } dtm_enh_rx_start;
} btc_ble_5_gap_args_t;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

void btc_gap_ble_call_handler(btc_msg_t *msg);
void btc_gap_ble_cb_handler(btc_msg_t *msg);
void btc_get_whitelist_size(uint16_t *length);
void btc_gap_ble_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_gap_ble_arg_deep_free(btc_msg_t *msg);
void btc_gap_ble_cb_deep_free(btc_msg_t *msg);
void btc_gap_ble_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_gap_callback_init(void);
bool btc_gap_ble_init(void);
void btc_gap_ble_deinit(void);
void btc_adv_list_init(void);
void btc_adv_list_deinit(void);

#endif /* __BTC_GAP_BLE_H__ */
