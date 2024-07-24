/**
 * @file SPDX-License-Identifier: Apache-2.0
 * 
 * @brief SPDX-FileCopyrightText:
 *        2015-2024 Espressif Systems (Shanghai) CO LTD
 */

#ifndef __BTC_GAP_BLE_H__
#define __BTC_GAP_BLE_H__

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#if BTC_DYNAMIC_MEMORY == TRUE
#include "bta/bta_api.h"
extern tBTA_BLE_ADV_DATA *gl_bta_adv_data_ptr;      // Declaration of a pointer called Global Bluetooth Advanced Advertising Data Pointer to the structure Type of Bluetooth Advanced Advertising Data.
extern tBTA_BLE_ADV_DATA *gl_bta_scan_rsp_data_ptr;     // Declaration of a pointer called Global Bluetooth Scan Response Data Pointer to the structure Type of Bluetooth Advanced Advertising Data.
#define gl_bta_adv_data     (*gl_bta_adv_data_ptr)
#define gl_bta_scan_rsp_data    (*gl_bta_scan_rsp_data_ptr)
#endif

/**
 * @brief Macro to check if a parameter x is within the range [min, max].
 * 
 * @param x   Parameter to be checked.
 * @param min Minimum allowed value.
 * @param max Maximum allowed value.
 * @return true if x is within the range [min, max], false otherwise.
 */
#define BLE_ISVALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)))

/**
 * @brief Bluetooth Generic Access Profile (GAP) action types for Bluetooth Low Energy operations.
 * 
 * This enumeration defines various actions supported by BLE, including advertisement,
 * scanning, connection parameter updates, security operations, and vendor-specific HCI commands.
 */
typedef enum {
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_CFG_ADV_DATA = 0,       //Bluetooth GAP action type for configuring advertisement data.
    BTC_GAP_BLE_ACT_SET_SCAN_PARAM,         //Bluetooth GAP action type for setting scan parameters.
    BTC_GAP_BLE_ACT_START_SCAN,             //Bluetooth GAP action type for starting scanning.
    BTC_GAP_BLE_ACT_STOP_SCAN,              //Bluetooth GAP action type for stopping scanning.
    BTC_GAP_BLE_ACT_START_ADV,              //Bluetooth GAP action type for starting advertising.
    BTC_GAP_BLE_ACT_STOP_ADV,               //Bluetooth GAP action type for stopping advertising.
#endif
//@}
    BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM = 6,  //Bluetooth GAP action type for updating connection parameters.
    BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN,       //Bluetooth GAP action type for setting packet data length.
    BTC_GAP_BLE_ACT_SET_RAND_ADDRESS,       //Bluetooth GAP action type for setting random address.
    BTC_GAP_BLE_ACT_CLEAR_RAND_ADDRESS,     //Bluetooth GAP action type for clearing random address.
    BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY,   //Bluetooth GAP action type for configuring local privacy.
    BTC_GAP_BLE_ACT_CONFIG_LOCAL_ICON,      //Bluetooth GAP action type for configuring local icon.
    BTC_GAP_BLE_ACT_UPDATE_WHITE_LIST,      //Bluetooth GAP action type for updating white list.
    BTC_GAP_BLE_ACT_CLEAR_WHITE_LIST,       // Bluetooth GAP action type for clearing white list.
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_SET_CONN_PARAMS,        //Bluetooth GAP action type for setting connection parameters.
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
//@}
    BTC_GAP_BLE_ACT_SET_DEV_NAME = 15,      //Bluetooth GAP action type for setting device name.
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW,       //Bluetooth GAP action type for configuring raw advertisement data.
    BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW,  // Bluetooth GAP action type for configuring raw scan response data.
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
//@}
    BTC_GAP_BLE_ACT_READ_RSSI = 18,         //Bluetooth GAP action type for reading Received Signal Strength Indication (RSSI).
    BTC_GAP_BLE_SET_ENCRYPTION_EVT,         //Bluetooth GAP event for setting encryption.
    BTC_GAP_BLE_SET_SECURITY_PARAM_EVT,     //Bluetooth GAP event for setting security parameters.
    BTC_GAP_BLE_SECURITY_RSP_EVT,           //Bluetooth GAP event for security response.
    BTC_GAP_BLE_PASSKEY_REPLY_EVT,          //Bluetooth GAP event for passkey reply.
    BTC_GAP_BLE_CONFIRM_REPLY_EVT,          //Bluetooth GAP event for confirmation reply.
    BTC_GAP_BLE_DISCONNECT_EVT,             //Bluetooth GAP event for disconnection.
    BTC_GAP_BLE_REMOVE_BOND_DEV_EVT,        //Bluetooth GAP event for removing bonded device.
    BTC_GAP_BLE_OOB_REQ_REPLY_EVT,          //Bluetooth GAP event for Out-of-Band (OOB) request reply.
    BTC_GAP_BLE_SC_OOB_REQ_REPLY_EVT,       //Bluetooth GAP event for Secure Connections OOB request reply.
    BTC_GAP_BLE_SC_CR_OOB_DATA_EVT,         //Bluetooth GAP event for Secure Connections OOB data.
    BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST,     //Bluetooth GAP action type for updating duplicate scan exceptional list.
    BTC_GAP_BLE_SET_AFH_CHANNELS,           //Bluetooth GAP action type for setting Adaptive Frequency Hopping (AFH) channels.
//@{
/** if Bluetooth Low Energy 5.0 Feature Support */
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_READ_PHY,                   //Bluetooth GAP action type for reading PHY (Physical Layer) information.
    BTC_GAP_BLE_SET_PREFERED_DEF_PHY,       //Bluetooth GAP action type for setting preferred default PHY.
    BTC_GAP_BLE_SET_DEF_PHY,                //Bluetooth GAP action type for setting default PHY.
    BTC_GAP_BLE_SET_EXT_ADV_RAND_ADDR,      //Bluetooth GAP action type for setting extended advertising random address.
    BTC_GAP_BLE_SET_EXT_ADV_PARAMS,         //Bluetooth GAP action type for setting extended advertising parameters.
    BTC_GAP_BLE_CFG_EXT_ADV_DATA_RAW,       //Bluetooth GAP action type for configuring raw extended advertising data.
    BTC_GAP_BLE_CFG_EXT_SCAN_RSP_DATA_RAW,  //Bluetooth GAP action type for configuring raw extended scan response data.
    BTC_GAP_BLE_EXT_ADV_START,              //Bluetooth GAP action type for starting extended advertising.
    BTC_GAP_BLE_EXT_ADV_STOP,               //Bluetooth GAP action type for stopping extended advertising.
    BTC_GAP_BLE_EXT_ADV_SET_REMOVE,         //Bluetooth GAP action type for setting extended advertising set for removal.
    BTC_GAP_BLE_EXT_ADV_SET_CLEAR,          //Bluetooth GAP action type for clearing extended advertising set.
    BTC_GAP_BLE_SET_PERIODIC_ADV_PARAMS,    //Bluetooth GAP action type for setting periodic advertising parameters.
    BTC_GAP_BLE_CFG_PERIODIC_ADV_DATA_RAW,  //Bluetooth GAP action type for configuring raw periodic advertising data.
    BTC_GAP_BLE_PERIODIC_ADV_START,         //Bluetooth GAP action type for starting periodic advertising.
    BTC_GAP_BLE_PERIODIC_ADV_STOP,          //Bluetooth GAP action type for stopping periodic advertising.
    BTC_GAP_BLE_PERIODIC_ADV_CREATE_SYNC,   //Bluetooth GAP action type for creating periodic advertising synchronization.
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL,   //Bluetooth GAP action type for canceling periodic advertising synchronization.
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE,                //Bluetooth GAP action type for terminating periodic advertising synchronization.
    BTC_GAP_BLE_PERIODIC_ADV_ADD_DEV_TO_LIST,               //Bluetooth GAP action type for adding device to periodic advertising list.
    BTC_GAP_BLE_PERIODIC_REMOVE_ADD_DEV_FROM_LIST,          //Bluetooth GAP action type for removing or adding device from/to periodic advertising list.
    BTC_GAP_BLE_PERIODIC_CLEAR_DEV,         //Bluetooth GAP action type for clearing periodic advertising device.
    BTC_GAP_BLE_SET_EXT_SCAN_PARAMS,        //Bluetooth GAP action type for setting extended scan parameters.
    BTC_GAP_BLE_START_EXT_SCAN,             //Bluetooth GAP action type for starting extended scan.
    BTC_GAP_BLE_STOP_EXT_SCAN,              //Bluetooth GAP action type for stopping extended scan.
    BTC_GAP_BLE_SET_EXT_PEFER_CONNET_PARAMS,//Bluetooth GAP action type for setting extended preferred connection parameters.
    BTC_GAP_BLE_DTM_ENH_TX_START,           //Bluetooth GAP action type for starting Enhanced Data Transmission (DTM) transmit.
    BTC_GAP_BLE_DTM_ENH_RX_START,           //Bluetooth GAP action type for starting Enhanced Data Transmission (DTM) receive.
#endif
//@}
    BTC_GAP_BLE_ACT_GET_DEV_NAME,           //Bluetooth GAP action type for getting device name.
/** if Bluetooth Low Energy Periodic Advertising Sync Transfer Feature Support */
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    BTC_GAP_BLE_PERIODIC_ADV_RECV_ENABLE,   //Bluetooth GAP action type for enabling periodic advertising receive.
    BTC_GAP_BLE_PERIODIC_ADV_SYNC_TRANS,    //Bluetooth GAP action type for periodic advertising synchronization transfer.
    BTC_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS,// Bluetooth GAP action type for periodic advertising set information transfer.
    BTC_GAP_BLE_SET_PERIODIC_ADV_SYNC_TRANS_PARAMS,         //Bluetooth GAP action type for setting periodic advertising sync transfer parameters.
#endif
//@}
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_DTM_TX_START,               //Bluetooth GAP action type for Data Transfer Mode (DTM) transmit start.
    BTC_GAP_BLE_DTM_RX_START,               //Bluetooth GAP action type for Data Transfer Mode (DTM) receive start.
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
//@}
    BTC_GAP_BLE_DTM_STOP,                   //Bluetooth GAP action type for Data Transfer Mode (DTM) stop.
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_CLEAR_ADV,              //Bluetooth GAP action type for clearing advertisement.
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    BTC_GAP_BLE_ACT_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT,
    BTC_GAP_BLE_ACT_ADD_DEVICE_TO_RESOLVING_LIST,
    BTC_GAP_BLE_ACT_VENDOR_HCI_CMD_EVT,
    BTC_GAP_BLE_SET_PRIVACY_MODE,
} btc_gap_ble_act_t;


/**
 * @brief Bluetooth Generic Access Profile (GAP) action types for BLE operations.
 * 
 * This union defines various structures for handling different BLE operations,
 * including advertisement, scanning, connection parameter updates, security operations,
 * and vendor-specific HCI commands.
 */
typedef union {
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
//Structure for configuring advertisement data.
    struct config_adv_data_args {
        esp_ble_adv_data_t adv_data;        //Advertisement data
    } cfg_adv_data;
//Structure for setting scan parameters.
    struct set_scan_params_args {
        esp_ble_scan_params_t  scan_params;//Scan parameters
    } set_scan_param;
 //Structure for starting scanning.
    struct start_scan_args {
        uint32_t duration;                 //Duration of scanning
    } start_scan;
//Structure for starting advertising.
    struct start_adv_args {
        esp_ble_adv_params_t adv_params;  //Advertising parameters
    } start_adv;
#endif
//@}
//Structure for connection parameter update.
    struct conn_update_params_args {
        esp_ble_conn_update_params_t conn_params;           //Connection update parameters 
    } conn_update_params;
//Structure for setting packet data length.
    struct set_pkt_data_len_args {
        esp_bd_addr_t remote_device;      //Remote device address
        uint16_t tx_data_length;          //Transmit data length,For BLE 4.x without Data Length Extension (DLE) support, common default maximum value is 27 bytes (if no negotiated MTU extension).
                                          //With BLE 5.x and DLE support, common default maximum values can extend up to 251 bytes or more, based on the negotiated MTU (Maximum Transmission Unit) size.
    } set_pkt_data_len;
//Structure for setting random address.
    struct set_rand_addr_args {
        esp_bd_addr_t rand_addr;          //Random address
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
        bool privacy_enable;            //Enable or disable local privacy.
    } cfg_local_privacy;
//Structure for configuring local icon.
    struct cfg_local_icon_args {
        uint16_t icon;                //Local icon
    } cfg_local_icon;
//Structure for updating white list.
    struct update_white_list_args {
        bool add_remove;             //Add or remove from white list.
        esp_bd_addr_t remote_bda;    //Remote device address.
        esp_ble_wl_addr_type_t wl_addr_type;//White list address type
    } update_white_list;
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    //BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST
    struct update_duplicate_exceptional_list_args {
        update_duplicate_exceptional_event_type  subcode;
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
#endif
//@}
    //BTC_GAP_BLE_ACT_SET_DEV_NAME,
    struct set_dev_name_args {
#define ESP_GAP_DEVICE_NAME_MAX (32)
        char *device_name;
    } set_dev_name;
//@{
/** if Bluetooth Low Energy 4.2 Feature Support */
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
#endif
//@}
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
    // BTC_GAP_BLE_SET_PRIVACY_MODE
    struct set_privacy_mode {
        esp_ble_addr_type_t addr_type;
        esp_bd_addr_t addr;
        uint8_t privacy_mode;
    } set_privacy_mode;
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
