/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_CTE_BLE_H__
#define __BTC_CTE_BLE_H__

#if (BLE_FEAT_CTE_EN == TRUE)
#include "esp_bt_defs.h"
#include "esp_ble_cte_api.h"
#include "btc/btc_manage.h"

typedef enum {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    BTC_CTE_ACT_SET_TRANS_PARAMS,
    BTC_CTE_ACT_SET_TRANS_ENABLE,
    BTC_CTE_ACT_SET_IQ_SAMPLING_EN,
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    BTC_CTE_ACT_SET_CONN_CTE_RECV_PARAMS,
    BTC_CTE_ACT_SET_CONN_CTE_TRANS_PARAMS,
    BTC_CTE_ACT_SET_CONN_CTE_REQUEST_EN,
    BTC_CTE_ACT_SET_CONN_CTE_RESPONSE_EN,
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    BTC_CTE_ACT_READ_ANTENNA_INFOR,
} btc_cte_ble_act_t;

/* btc_ble_cte_args_t */
typedef union {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    struct cte_set_trans_params_arg {
        uint8_t adv_handle;
        uint8_t cte_len;
        uint8_t cte_type;
        uint8_t cte_count;
        uint8_t switching_pattern_len;
        uint8_t *antenna_ids;
    } cte_trans_params;

    struct cte_set_trans_enable_arg {
        uint8_t adv_handle;
        uint8_t cte_enable;
    } cte_trans_enable;

    struct cte_iq_sampling_en_arg {
        uint16_t sync_handle;
        uint8_t sampling_en;
        uint8_t slot_dur;
        uint8_t max_sampled_ctes;
        uint8_t switching_pattern_len;
        uint8_t *antenna_ids;
    } cte_iq_sampling_en;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    struct cte_recv_params_arg {
        uint16_t conn_handle;
        uint8_t sampling_en;
        uint8_t slot_dur;
        uint8_t switching_pattern_len;
        uint8_t *antenna_ids;
    } cte_recv_params;

    struct cte_set_conn_trans_params_arg {
        uint16_t conn_handle;
        uint8_t cte_types;
        uint8_t switching_pattern_len;
        uint8_t *antenna_ids;
    } cte_conn_trans_params;

    struct cte_req_en_arg {
        uint16_t conn_handle;
        uint8_t enable;
        uint16_t cte_req_interval;
        uint8_t req_cte_len;
        uint8_t req_cte_Type;
    } cte_req_en;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

    struct cte_rsp_en_arg {
        uint16_t conn_handle;
        uint8_t enable;
    } cte_rsp_en;

} btc_ble_cte_args_t;

void btc_ble_cte_call_handler(btc_msg_t *msg);
void btc_ble_cte_cb_handler(btc_msg_t *msg);
void btc_ble_cte_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_ble_cte_arg_deep_free(btc_msg_t *msg);
void btc_ble_cte_cb_deep_free(btc_msg_t *msg);
void btc_ble_cte_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_cte_callback_init(void);

#endif // #if (BLE_FEAT_CTE_EN == TRUE)

#endif /* __BTC_CTE_BLE_H__ */
