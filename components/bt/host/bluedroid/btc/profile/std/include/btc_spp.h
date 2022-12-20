/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_SPP_H__
#define __BTC_SPP_H__

#include "btc/btc_task.h"
#include "esp_bt_defs.h"
#include "esp_spp_api.h"
#include "common/bt_target.h"
#include "bta/bta_jv_api.h"

#if (defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE)

#define ESP_SPP_MAX_SESSION     BTA_JV_MAX_RFC_SR_SESSION
#define ESP_SPP_SERVER_NAME_MAX 32

#define BTC_SPP_INVALID_SCN 0x00

typedef enum {
    BTC_SPP_ACT_INIT = 0,
    BTC_SPP_ACT_UNINIT,
    BTC_SPP_ACT_START_DISCOVERY,
    BTC_SPP_ACT_CONNECT,
    BTC_SPP_ACT_DISCONNECT,
    BTC_SPP_ACT_START_SRV,
    BTC_SPP_ACT_STOP_SRV,
    BTC_SPP_ACT_WRITE,
    BTC_SPP_ACT_VFS_REGISTER,
    BTC_SPP_ACT_VFS_UNREGISTER,
} btc_spp_act_t;

/* btc_spp_args_t */
typedef union {
    //BTC_SPP_ACT_INIT
    struct init_arg {
        esp_spp_mode_t mode;
        bool enable_l2cap_ertm;
        UINT16 tx_buffer_size;
    } init;
    //BTC_SPP_ACT_UNINIT
    struct uninit_arg {
    } uninit;

    //BTC_SPP_ACT_START_DISCOVERY
    struct start_discovery_arg {
        BD_ADDR bd_addr;
        UINT16 num_uuid;
        tSDP_UUID *p_uuid_list;
    } start_discovery;
    //BTC_SPP_ACT_CONNECT
    struct conn_arg {
        esp_spp_sec_t sec_mask;
        esp_spp_role_t role;
        UINT8 remote_scn;
        esp_bd_addr_t peer_bd_addr;
    } connect;
    //BTC_SPP_ACT_DISCONNECT
    struct disconn_arg {
        UINT32 handle;
    } disconnect;
    //BTC_SPP_ACT_START_SRV
    struct start_srv_arg {
        esp_spp_sec_t sec_mask;
        esp_spp_role_t role;
        UINT8 local_scn;
        UINT8 max_session;
        char name[ESP_SPP_SERVER_NAME_MAX + 1];
    } start_srv;
    //BTC_SPP_ACT_STOP_SRV
    struct stop_srv_arg {
        UINT8 scn;
    } stop_srv;
    //BTC_SPP_ACT_WRITE
    struct write_arg {
        UINT32 handle;
        int len;
        UINT8 *p_data;
    } write;

} btc_spp_args_t;


void btc_spp_call_handler(btc_msg_t *msg);
void btc_spp_cb_handler(btc_msg_t *msg);
void btc_spp_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_spp_arg_deep_free(btc_msg_t *msg);

esp_err_t spp_send_data_to_btc(uint32_t handle, int len, uint8_t *p_data, esp_spp_mode_t spp_mode);
#endif ///defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE
#endif ///__BTC_SPP_H__
