/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_ISO_BLE_H__
#define __BTC_ISO_BLE_H__

#if (BLE_FEAT_ISO_EN == TRUE)
#include "esp_bt_defs.h"
#include "esp_ble_iso_api.h"
#include "btc/btc_manage.h"

struct btc_iso_cis_params {
    uint8_t cis_id;
    uint16_t max_sdu_c_to_p;
    uint16_t max_sdu_p_to_c;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t rtn_c_to_p;
    uint8_t rtn_p_to_c;
} __attribute__((packed));

struct btc_iso_cis_params_test {
    uint8_t cis_id;
    uint8_t nse;
    uint16_t max_sdu_c_to_p;
    uint16_t max_sdu_p_to_c;
    uint16_t max_pdu_c_to_p;
    uint16_t max_pdu_p_to_c;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t bn_c_to_p;
    uint8_t bn_p_to_c;
} __attribute__((packed));

struct btc_iso_cis_hdls {
    uint16_t cis_hdl;
    uint16_t acl_hdl;
}__attribute__((packed));

typedef enum {
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
    BTC_ISO_ACT_BIG_CREATE,
    BTC_ISO_ACT_BIG_CREATE_TEST,
    BTC_ISO_ACT_BIG_TERMINATE,
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    BTC_ISO_ACT_BIG_SYNC_CREATE,
    BTC_ISO_ACT_BIG_SYNC_TERMINATE,
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    BTC_ISO_ACT_SET_DATA_PATH,
    BTC_ISO_ACT_REMOVE_DATA_PATH,
    BTC_ISO_ACT_READ_ISO_TX_SYNC,
    BTC_ISO_ACT_READ_ISO_LINK_QUALITY,
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    BTC_ISO_ACT_SET_CIG_PARAMS,
    BTC_ISO_ACT_SET_CIG_PARAMS_TEST,
    BTC_ISO_ACT_CREATE_CIS,
    BTC_ISO_ACT_REMOVE_CIG,
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
    BTC_ISO_ACT_ACCEPT_CIS_REQ,
    BTC_ISO_ACT_REJECT_CIS_REQ,
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
    BTC_ISO_ACT_DISCON_CIS,
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
} btc_iso_ble_act_t;

/* btc_ble_iso_args_t */
typedef union {
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
    struct iso_big_create_args {
        uint8_t big_handle;
        uint8_t adv_handle;
        uint8_t num_bis;
        uint32_t sdu_interval;
        uint16_t max_sdu;
        uint16_t max_transport_latency;
        uint8_t rtn;
        uint8_t phy;
        uint8_t packing;
        uint8_t framing;
        uint8_t encryption;
        uint8_t broadcast_code[16];
    } iso_big_creat_params;

    struct iso_big_create_test_args {
        uint8_t big_handle;
        uint8_t adv_handle;
        uint8_t num_bis;
        uint32_t sdu_interval;
        uint16_t iso_interval;
        uint8_t nse;
        uint16_t max_sdu;
        uint16_t max_pdu;
        uint8_t phy;
        uint8_t packing;
        uint8_t framing;
        uint8_t bn;
        uint8_t irc;
        uint8_t pto;
        uint8_t encryption;
        uint8_t broadcast_code[16];
    } iso_big_creat_test_params;

    struct iso_big_terminate_args {
        uint8_t big_handle;
        uint8_t reason;
    } iso_big_terminate_params;
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    struct iso_big_sync_creat_arg {
        uint8_t big_handle;
        uint16_t sync_handle;
        uint8_t encryption;
        uint8_t bc_code[16];
        uint8_t mse;
        uint16_t big_sync_timeout;
        uint8_t num_bis;
        uint8_t bis[BLE_ISO_BIS_MAX_COUNT];
    } iso_big_sync_creat_params;

    struct iso_big_sync_terminate_arg {
        uint8_t big_handle;
    } iso_big_sync_terminate_params;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    struct iso_set_data_path_arg {
        uint16_t conn_handle;
        uint8_t data_path_dir;
        uint8_t data_path_id;
        uint8_t coding_fmt;
        uint16_t company_id;
        uint16_t vs_codec_id;
        uint32_t controller_delay;
        uint8_t codec_cfg_len;
        uint8_t *codec_cfg;
    } iso_set_data_path_params;

    struct iso_remove_data_path_arg {
        uint16_t conn_handle;
        uint8_t data_path_dir;
    } iso_remove_data_path_params;

    struct iso_read_tx_sync_arg {
        uint16_t iso_handle;
    } iso_read_tx_sync_params;

    struct iso_read_link_quality_arg {
        uint16_t iso_handle;
    } iso_read_link_quality_params;

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    struct set_cig_params_arg {
        uint8_t cig_id;
        uint32_t sdu_int_c_to_p;
        uint32_t sdu_int_p_to_c;
        uint8_t worse_case_SCA;
        uint8_t packing;
        uint8_t framing;
        uint16_t mtl_c_to_p;
        uint16_t mtl_p_to_c;
        uint8_t cis_cnt;
        struct btc_iso_cis_params cis_params[BLE_ISO_CIS_MAX_COUNT];
    } set_cig_params;

    struct set_cig_params_test_arg {
        uint8_t cig_id;
        uint32_t sdu_int_c_to_p;
        uint32_t sdu_int_p_to_c;
        uint8_t ft_c_to_p;
        uint8_t ft_p_to_c;
        uint16_t iso_interval;
        uint8_t worse_case_SCA;
        uint8_t packing;
        uint8_t framing;
        uint8_t cis_cnt;
        struct btc_iso_cis_params_test cis_params_test[ESP_BLE_MAX_CIS_NUM];
    } set_cig_params_test;

    struct creat_cis_arg {
        uint8_t cis_count;
        struct btc_iso_cis_hdls cis_hdls[BLE_ISO_CIS_MAX_COUNT];
    } creat_cis_params;
    struct remove_cig_arg {
        uint8_t cig_id;
    } remove_cig_params;
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
     struct accept_cis_req_arg {
        uint16_t cis_handle;
    } accept_cis_req_params;

    struct reject_cis_req_arg {
        uint16_t cis_handle;
        uint8_t  reason;
    } reject_cis_req_params;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
    struct discon_cis_arg {
        uint16_t cis_handle;
        uint8_t  reason;
    } discon_cis_params;
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
} btc_ble_iso_args_t;

void btc_iso_ble_call_handler(btc_msg_t *msg);
void btc_iso_ble_cb_handler(btc_msg_t *msg);
void btc_iso_ble_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_iso_ble_arg_deep_free(btc_msg_t *msg);
void btc_iso_ble_cb_deep_free(btc_msg_t *msg);
void btc_iso_ble_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_iso_callback_init(void);

#endif // #if (BLE_FEAT_ISO_EN == TRUE)

#endif /* __BTC_ISO_BLE_H__ */
