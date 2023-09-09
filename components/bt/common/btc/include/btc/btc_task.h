/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_TASK_H__
#define __BTC_TASK_H__

#include <stdint.h>
#include "bt_common.h"
#include "osi/thread.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "common/bt_target.h"
#endif

typedef struct btc_msg {
    uint8_t sig;    //event signal
    uint8_t aid;    //application id
    uint8_t pid;    //profile id
    uint8_t act;    //profile action, defined in seprerate header files
    UINT8   arg[0]; //param for btc function or function param
} btc_msg_t;

typedef struct btc_adv_packet {
    uint8_t addr[6];
    uint8_t addr_type;
} btc_adv_packet_t;

typedef enum {
    BTC_SIG_API_CALL = 0,   // APP TO STACK
    BTC_SIG_API_CB,         // STACK TO APP
    BTC_SIG_NUM,
} btc_sig_t; //btc message type

typedef enum {
    BTC_PID_MAIN_INIT = 0,
    BTC_PID_DEV,
    BTC_PID_GATTS,
#if (GATTC_INCLUDED == TRUE)
    BTC_PID_GATTC,
#endif  ///GATTC_INCLUDED == TRUE
    BTC_PID_GATT_COMMON,
    BTC_PID_GAP_BLE,
    BTC_PID_BLE_HID,
    BTC_PID_SPPLIKE,
#if (BLUFI_INCLUDED == TRUE)
    BTC_PID_BLUFI,
#endif  ///BLUFI_INCLUDED == TRUE
    BTC_PID_DM_SEC,
    BTC_PID_ALARM,
#if (CLASSIC_BT_INCLUDED == TRUE)
    BTC_PID_GAP_BT,
    BTC_PID_PRF_QUE,
    BTC_PID_A2DP,
    BTC_PID_AVRC_CT,
    BTC_PID_AVRC_TG,
    BTC_PID_SPP,
    BTC_PID_HD,
    BTC_PID_HH,
    BTC_PID_L2CAP,
    BTC_PID_SDP,
#if (BTC_HF_INCLUDED == TRUE)
    BTC_PID_HF,
#endif /* BTC_HF_INCLUDED */
#if (BTC_HF_CLIENT_INCLUDED == TRUE)
    BTC_PID_HF_CLIENT,
#endif /* BTC_HF_CLIENT_INCLUDED */
#endif  /* CLASSIC_BT_INCLUDED */
#if CONFIG_BLE_MESH
    BTC_PID_PROV,
    BTC_PID_MODEL,
    BTC_PID_HEALTH_CLIENT,
    BTC_PID_HEALTH_SERVER,
    BTC_PID_CONFIG_CLIENT,
    BTC_PID_CONFIG_SERVER,
    BTC_PID_AGG_CLIENT,
    BTC_PID_AGG_SERVER,
    BTC_PID_BRC_CLIENT,
    BTC_PID_BRC_SERVER,
    BTC_PID_DF_CLIENT,
    BTC_PID_DF_SERVER,
    BTC_PID_LCD_CLIENT,
    BTC_PID_LCD_SERVER,
    BTC_PID_ODP_CLIENT,
    BTC_PID_ODP_SERVER,
    BTC_PID_PRB_CLIENT,
    BTC_PID_PRB_SERVER,
    BTC_PID_RPR_CLIENT,
    BTC_PID_RPR_SERVER,
    BTC_PID_SAR_CLIENT,
    BTC_PID_SAR_SERVER,
    BTC_PID_SRPL_CLIENT,
    BTC_PID_SRPL_SERVER,
    BTC_PID_GENERIC_CLIENT,
    BTC_PID_LIGHTING_CLIENT,
    BTC_PID_SENSOR_CLIENT,
    BTC_PID_TIME_SCENE_CLIENT,
    BTC_PID_GENERIC_SERVER,
    BTC_PID_LIGHTING_SERVER,
    BTC_PID_SENSOR_SERVER,
    BTC_PID_TIME_SCENE_SERVER,
    BTC_PID_MBT_CLIENT,
    BTC_PID_MBT_SERVER,
    BTC_PID_BLE_MESH_BLE_COEX,
#endif /* CONFIG_BLE_MESH */
    BTC_PID_NUM,
} btc_pid_t; //btc profile id

typedef struct {
    void (* btc_call)(btc_msg_t *msg);
    void (* btc_cb)(btc_msg_t *msg);
} btc_func_t;

typedef void (* btc_arg_deep_copy_t)(btc_msg_t *msg, void *dst, void *src);
typedef void (* btc_arg_deep_free_t)(btc_msg_t *msg);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * transfer an message to another module in the different task.
 * @param  msg       message
 * @param  arg       paramter
 * @param  arg_len   length of paramter
 * @param  copy_func deep copy function
 * @param  free_func deep free function
 * @return           BT_STATUS_SUCCESS: success
 *                   others: fail
 */
bt_status_t btc_transfer_context(btc_msg_t *msg, void *arg, int arg_len, btc_arg_deep_copy_t copy_func,
                                    btc_arg_deep_free_t free_func);

/**
 * transfer an message to another module in tha same task.
 * @param  msg       message
 * @return           BT_STATUS_SUCCESS: success
 *                   others: fail
 */
bt_status_t btc_inter_profile_call(btc_msg_t *msg);

bt_status_t btc_init(void);
void btc_deinit(void);
bool btc_check_queue_is_congest(void);
int get_btc_work_queue_size(void);

/**
 * get the BTC thread handle
 * @return           NULL: fail
 *                   others: pointer of osi_thread structure of BTC
 */
osi_thread_t *btc_get_current_thread(void);

#ifdef __cplusplus
}
#endif

#endif /* __BTC_TASK_H__ */
