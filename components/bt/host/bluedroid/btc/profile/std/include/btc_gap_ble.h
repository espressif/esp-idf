/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_GAP_BLE_H__
#define __BTC_GAP_BLE_H__

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#if BTC_DYNAMIC_MEMORY == TRUE
#if (BLE_42_FEATURE_SUPPORT == TRUE)
#if (BLE_42_ADV_EN == TRUE)
#include "bta/bta_api.h"
extern tBTA_BLE_ADV_DATA *gl_bta_adv_data_ptr;
extern tBTA_BLE_ADV_DATA *gl_bta_scan_rsp_data_ptr;
#define gl_bta_adv_data     (*gl_bta_adv_data_ptr)
#define gl_bta_scan_rsp_data    (*gl_bta_scan_rsp_data_ptr)
#endif // #if (BLE_42_ADV_EN == TRUE)
#endif // BLE_42_FEATURE_SUPPORT
#endif

#define BLE_ISVALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)))

#define BLE_VENDOR_PDU_RECV_EVT         (0xC0)
#define BLE_VENDOR_CHMAP_UPDATE_EVT     (0xC1)
#define BLE_VENDOR_SLEEP_WAKEUP_EVT     (0xC3)

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
#if (BLE_50_EXTEND_ADV_EN == TRUE)
    BTC_GAP_BLE_SET_EXT_ADV_RAND_ADDR,
    BTC_GAP_BLE_SET_EXT_ADV_PARAMS,
    BTC_GAP_BLE_CFG_EXT_ADV_DATA_RAW,
    BTC_GAP_BLE_CFG_EXT_SCAN_RSP_DATA_RAW,
    BTC_GAP_BLE_EXT_ADV_START,
    BTC_GAP_BLE_EXT_ADV_STOP,
    BTC_GAP_BLE_EXT_ADV_SET_REMOVE,
    BTC_GAP_BLE_EXT_ADV_SET_CLEAR,
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
#if (BLE_50_PERIODIC_ADV_EN == TRUE)
    BTC_GAP_BLE_SET_PERIODIC_ADV_PARAMS,
    BTC_GAP_BLE_CFG_PERIODIC_ADV_DATA_RAW,
    BTC_GAP_BLE_PERIODIC_ADV_START,
    BTC_GAP_BLE_PERIODIC_ADV_STOP,
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
    BTC_GAP_BLE_PERIODIC_ADV_CREATE_SYNC,
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL,
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE,
    BTC_GAP_BLE_PERIODIC_ADV_ADD_DEV_TO_LIST,
    BTC_GAP_BLE_PERIODIC_REMOVE_ADD_DEV_FROM_LIST,
    BTC_GAP_BLE_PERIODIC_CLEAR_DEV,
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
    BTC_GAP_BLE_SET_EXT_SCAN_PARAMS,
    BTC_GAP_BLE_START_EXT_SCAN,
    BTC_GAP_BLE_STOP_EXT_SCAN,
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
    BTC_GAP_BLE_SET_EXT_PEFER_CONNET_PARAMS,
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_50_DTM_TEST_EN == TRUE)
    BTC_GAP_BLE_DTM_ENH_TX_START,
    BTC_GAP_BLE_DTM_ENH_RX_START,
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)
    BTC_GAP_BLE_ACT_GET_DEV_NAME,
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    BTC_GAP_BLE_PERIODIC_ADV_RECV_ENABLE,
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_TRANS,
    BTC_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS,
    BTC_GAP_BLE_SET_PERIODIC_ADV_SYNC_TRANS_PARAMS,
#endif //#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#if (BLE_42_DTM_TEST_EN == TRUE)
    BTC_GAP_BLE_DTM_TX_START,
    BTC_GAP_BLE_DTM_RX_START,
#endif // #if (BLE_42_DTM_TEST_EN == TRUE)
    BTC_GAP_BLE_DTM_STOP,
#if (BLE_VENDOR_HCI_EN == TRUE)
    BTC_GAP_BLE_ACT_CLEAR_ADV,
    BTC_GAP_BLE_ACT_VENDOR_HCI_CMD_EVT,
    BTC_GAP_BLE_SET_CSA_SUPPORT,
    BTC_GAP_BLE_ACT_SET_VENDOR_EVT_MASK,
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)
    BTC_GAP_BLE_ACT_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT,
    BTC_GAP_BLE_ACT_ADD_DEVICE_TO_RESOLVING_LIST,
    BTC_GAP_BLE_SET_PRIVACY_MODE,
#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
    BTC_GAP_BLE_ENH_READ_TRANS_POWER_LEVEL,
    BTC_GAP_BLE_READ_REM_TRANS_POWER_LEVEL,
    BTC_GAP_BLE_SET_PATH_LOSS_REPORT_PARAMS,
    BTC_GAP_BLE_SET_PATH_LOSS_REPORTING_EN,
    BTC_GAP_BLE_SET_TRANS_POWER_REPORTING_EN,
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
    BTC_GAP_BLE_SET_DEFALT_SUBRATE,
    BTC_GAP_BLE_SUBRATE_REQUEST,
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    BTC_GAP_ACT_SET_HOST_FEATURE,
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_READ_CHANNEL_MAP,
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    BTC_GAP_BLE_SET_PA_SUBEVT_DATA,
    BTC_GAP_BLE_SET_PA_RSP_DATA,
    BTC_GAP_BLE_SET_PA_SYNC_SUBEVT,
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    BTC_GAP_BLE_CS_READ_LOCAL_SUPPORTED_CAPS,
    BTC_GAP_BLE_CS_READ_REMOTE_SUPPORTED_CAPS,
    BTC_GAP_BLE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPS,
    BTC_GAP_BLE_CS_SECURITY_ENABLE,
    BTC_GAP_BLE_CS_SET_DEFAULT_SETTINGS,
    BTC_GAP_BLE_CS_READ_REMOTE_FAE_TABLE,
    BTC_GAP_BLE_CS_WRITE_CACHED_REMOTE_FAE_TABLE,
    BTC_GAP_BLE_CS_CREATE_CONFIG,
    BTC_GAP_BLE_CS_REMOVE_CONFIG,
    BTC_GAP_BLE_CS_SET_CAHNNEL_CLASSIFICATION,
    BTC_GAP_BLE_CS_SET_PROCEDURE_PARAMS,
    BTC_GAP_BLE_CS_PROCEDURE_ENABLE,
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
    BTC_GAP_BLE_ACT_SET_KEY_MATERIAL,
#endif
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
#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
    //BTC_GAP_BLE_ACT_SET_KEY_MATERIAL
    struct set_key_material_args {
        uint8_t session_key[16];
        uint8_t iv[8];
    } set_key_material;
#endif
    //BTC_GAP_BLE_ACT_UPDATE_WHITE_LIST
    struct update_white_list_args {
        bool add_remove;
        esp_bd_addr_t remote_bda;
        esp_ble_wl_addr_type_t wl_addr_type;
    } update_white_list;
#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
    //BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST
    struct update_duplicate_exceptional_list_args {
        uint8_t  subcode;
        uint32_t  info_type;
        esp_duplicate_info_t device_info;
    } update_duplicate_exceptional_list;
#endif // ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
#if (BLE_42_FEATURE_SUPPORT == TRUE)
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
#if (BLE_42_DTM_TEST_EN == TRUE)
    struct dtm_tx_start_args {
        uint8_t tx_channel;
        uint8_t len_of_data;
        uint8_t pkt_payload;
    } dtm_tx_start;
    struct dtm_rx_start_args {
        uint8_t rx_channel;
    } dtm_rx_start;
#endif // #if (BLE_42_DTM_TEST_EN == TRUE)
    //BTC_DEV_VENDOR_HCI_CMD_EVT
    struct vendor_cmd_send_args {
        uint16_t  opcode;
        uint8_t  param_len;
        uint8_t *p_param_buf;
    } vendor_cmd_send;
    // BTC_GAP_BLE_SET_PRIVACY_MODE
    struct set_privacy_mode {
        esp_ble_addr_type_t addr_type;
        esp_bd_addr_t addr;
        uint8_t privacy_mode;
    } set_privacy_mode;
    // BTC_GAP_BLE_SET_CSA_SUPPORT
    struct set_csa_support_args {
        uint8_t csa_select;
    } set_csa_support;
    // BTC_GAP_BLE_ACT_SET_VENDOR_EVT_MASK
    struct set_vendor_evt_mask_args {
        uint32_t evt_mask;
    } set_vendor_evt_mask;
    struct read_channel_map_args {
        esp_bd_addr_t bd_addr;
    } read_channel_map;
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
#if (BLE_50_EXTEND_ADV_EN == TRUE)
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
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

#if (BLE_50_PERIODIC_ADV_EN == TRUE)
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
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)

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
#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
    // BTC_GAP_BLE_ENH_READ_TRANS_POWER_LEVEL
    struct enh_read_trans_pwr_level_args {
        uint8_t conn_handle;
        uint8_t phy;
    } enh_read_trans_pwr_level;
    // BTC_GAP_BLE_READ_REM_TRANS_POWER_LEVEL
    struct read_rem_trans_pwr_level_args {
        uint8_t conn_handle;
        uint8_t phy;
    } read_rem_trans_pwr_level;
    // BTC_GAP_BLE_SET_PATH_LOSS_REPORT_PARAMS
    struct set_path_loss_rpt_params_args {
        uint16_t conn_handle;
        uint8_t high_threshold;
	    uint8_t high_hysteresis;
	    uint8_t low_threshold;
	    uint8_t low_hysteresis;
	    uint16_t min_time_spent;
    } set_path_loss_rpt_params;
    // BTC_GAP_BLE_SET_PATH_LOSS_REPORTING_EN
    struct set_path_loss_rpt_en_args {
        uint16_t conn_handle;
        uint8_t enable;
    } set_path_loss_rpt_en;
    // BTC_GAP_BLE_SET_TRANS_POWER_REPORTING_EN
    struct set_trans_pwr_rpting_en_args {
        uint16_t conn_handle;
        uint8_t local_enable;
        uint8_t remote_enable;
    } set_trans_pwr_rpting_en;
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
    // BTC_GAP_BLE_SET_DEFALT_SUBRATE
    struct default_subrate_param_args {
        uint16_t subrate_min;
        uint16_t subrate_max;
        uint16_t max_latency;
        uint16_t continuation_number;
        uint16_t supervision_timeout;
    } default_subrate_param;
    // BTC_GAP_BLE_SUBRATE_REQUEST
    struct subrate_req_param_args {
        uint16_t conn_handle;
        uint16_t subrate_min;
        uint16_t subrate_max;
        uint16_t max_latency;
        uint16_t continuation_number;
        uint16_t supervision_timeout;
    } subrate_req_param;
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
    struct set_host_feature_arg {
        uint16_t bit_num;
        uint8_t bit_val;
    } set_host_feature_params;
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    // BTC_GAP_BLE_SET_PA_SUBEVT_DATA
    struct per_adv_subevent_data_params_args {
        uint8_t adv_handle;
        uint8_t num_subevents_with_data;
        esp_ble_subevent_params *subevent_params;
    } per_adv_subevent_data_params;
    // BTC_GAP_BLE_SET_PA_RSP_DATA
    struct per_adv_response_data_params_args {
        uint16_t sync_handle;
        uint16_t request_event;
        uint8_t request_subevent;
        uint8_t response_subevent;
        uint8_t response_slot;
        uint8_t response_data_len;
        uint8_t *response_data;
    } per_adv_response_data_params;
    // BTC_GAP_BLE_SET_PA_SYNC_SUBEVT
    struct per_sync_subevent_params_args {
        uint16_t sync_handle;
        uint16_t periodic_adv_properties;
        uint8_t num_subevents_to_sync;
        uint8_t *subevent;
    } per_sync_subevent_params;
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    struct cs_read_remote_supp_caps_args {
        uint16_t conn_handle;
    } cs_read_remote_supp_caps;

    struct cs_write_cached_remote_supp_caps_args {
        uint16_t conn_handle;
        uint8_t num_config_supported;
        uint16_t max_consecutive_proc_supported;
        uint8_t num_ant_supported;
        uint8_t max_ant_paths_supported;
        uint8_t roles_supported;
        uint8_t modes_supported;
        uint8_t rtt_capability;
        uint8_t rtt_aa_only_n;
        uint8_t rtt_sounding_n;
        uint8_t rtt_random_payload_n;
        uint16_t NADM_sounding_capability;
        uint16_t NADM_random_capability;
        uint8_t  cs_sync_phys_supported;
        uint16_t subfeatures_supported;
        uint16_t T_IP1_times_supported;
        uint16_t T_IP2_times_supported;
        uint16_t T_FCS_times_supported;
        uint16_t T_PM_times_supported;
        uint8_t T_SW_times_supported;
        uint8_t TX_SNR_capability;
    } cs_write_cached_remote_supp_caps;

    struct cs_security_enable_args {
        uint16_t conn_handle;
    } cs_security_enable;

    struct cs_set_default_settings_params_args {
        uint16_t conn_handle;
        uint8_t role_enable;
        uint8_t cs_sync_ant_selection;
        int8_t max_tx_power;
    } cs_set_default_settings_params;

    struct cs_read_remote_tab_args {
        uint16_t conn_handle;
    } cs_read_remote_tab;

    struct cs_write_cached_remote_fae_table_params_args {
        uint16_t conn_handle;
        uint8_t remote_fae_table[72];
    } cs_write_cached_remote_fae_table_params;

    struct cs_create_config_params_args {
        uint16_t conn_handle;
        uint8_t config_id;
        uint8_t create_context;
        uint8_t main_mode_type;
        uint8_t sub_mode_type;
        uint8_t min_main_mode_steps;
        uint8_t max_main_mode_steps;
        uint8_t main_mode_repetition;
        uint8_t mode_0_steps;
        uint8_t role;
        uint8_t rtt_type;
        uint8_t cs_sync_phy;
        uint8_t channel_map[10];
        uint8_t channel_map_repetition;
        uint8_t channel_selection_type;
        uint8_t ch3c_shape;
        uint8_t ch3c_jump;
        uint8_t reserved;
    } cs_create_config_params;

    struct cs_remove_config_params_args {
        uint16_t conn_handle;
        uint8_t config_id;
    } cs_remove_config_params;

    struct cs_set_channel_class_params_args {
        uint8_t channel_class[10];
    } cs_set_channel_class_params;

    struct cs_set_procedure_params_args {
        uint16_t conn_handle;
        uint8_t config_id;
        uint16_t max_procedure_len;
        uint16_t min_procedure_interval;
        uint16_t max_procedure_interval;
        uint16_t max_procedure_count;
        uint32_t min_subevent_len;
        uint32_t max_subevent_len;
        uint8_t tone_ant_config_selection;
        uint8_t phy;
        uint8_t tx_power_delta;
        uint8_t preferred_peer_antenna;
        uint8_t SNR_control_initiator;
        uint8_t SNR_control_reflector;
    } cs_set_procedure_params;

    struct cs_procedure_enable_params_args {
        uint16_t conn_handle;
        uint8_t config_id;
        uint8_t enable;
    } cs_procedure_enable_params;
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
} btc_ble_5_gap_args_t;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

void btc_gap_ble_call_handler(btc_msg_t *msg);
void btc_gap_ble_cb_handler(btc_msg_t *msg);
void btc_get_whitelist_size(uint16_t *length);
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
void btc_get_periodic_list_size(uint8_t *length);
#endif //#if (BLE_50_EXTEND_SYNC_EN == TRUE)
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
