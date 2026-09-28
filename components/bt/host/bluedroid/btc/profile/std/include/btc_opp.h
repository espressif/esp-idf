/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "common/bt_target.h"
#include "btc/btc_task.h"
#include "bta/bta_opp_api.h"
#include "esp_opp_api.h"

#if BTC_OPP_INCLUDED

typedef enum {
#if BTC_OPP_SERVER_INCLUDED
    BTC_OPP_SERVER_INIT_EVT,
    BTC_OPP_SERVER_DEINIT_EVT,
    BTC_OPP_START_SERVER_EVT,
    BTC_OPP_STOP_SERVER_EVT,
    BTC_OPP_SERVER_ACCEPT_EVT,
    BTC_OPP_SERVER_REJECT_EVT,
    BTC_OPP_SERVER_CANCEL_EVT,
#endif
#if BTC_OPP_CLIENT_INCLUDED
    BTC_OPP_CLIENT_INIT_EVT,
    BTC_OPP_CLIENT_DEINIT_EVT,
    BTC_OPP_CLIENT_CONNECT_EVT,
    BTC_OPP_CLIENT_DISCONNECT_EVT,
    BTC_OPP_OPEN_OBJECT_EVT,
    BTC_OPP_CLIENT_CANCEL_EVT,
#endif
} btc_opp_act_t;

typedef union {
#if BTC_OPP_SERVER_INCLUDED
    struct {
        esp_opp_server_cfg_t cfg;
        uint8_t *supported_formats;
        char *service_name;
    } start_server;
#endif
#if BTC_OPP_CLIENT_INCLUDED
    struct {
        esp_opp_client_connect_param_t param;
    } connect;
    struct {
        esp_opp_client_object_cfg_t object;
        char *name;
        char *type;
    } open_object;
#endif
    struct {
        esp_opp_conn_hdl_t handle;
    } by_handle;
} btc_opp_args_t;

esp_bt_status_t btc_opp_status_from_bta(tBTA_OPP_STATUS status);
void bte_opp_evt(tBTA_OPP_EVT event, tBTA_OPP *p_data);
void btc_opp_arg_deep_copy(btc_msg_t *msg, void *dst, void *src);
void btc_opp_arg_deep_free(btc_msg_t *msg);
void btc_opp_cb_arg_deep_free(btc_msg_t *msg);

/* VFS helpers shared by API / BTC / BTA.
 * acquire/release are refcounted across client and server init/deinit. */
bool btc_opp_vfs_is_registered(void);
esp_err_t btc_opp_vfs_acquire(void);
esp_err_t btc_opp_vfs_release(void);
#if BTC_OPP_CLIENT_INCLUDED
esp_err_t btc_opp_vfs_alloc_client_slot(esp_opp_conn_hdl_t handle, uint32_t len, int *out_fd);
int btc_opp_vfs_pull_tx(esp_opp_conn_hdl_t handle, uint8_t *buf, uint16_t max_len,
                        uint16_t *out_len, bool *is_final, bool *waiting);
#endif
#if BTC_OPP_SERVER_INCLUDED
esp_err_t btc_opp_vfs_alloc_server_slot(esp_opp_conn_hdl_t handle, int *out_fd);
esp_err_t btc_opp_vfs_get_fd(esp_opp_conn_hdl_t handle, int *out_fd);
bool btc_opp_vfs_enqueue_rx(esp_opp_conn_hdl_t handle, const uint8_t *data, uint16_t data_len, bool final);
#endif
void btc_opp_vfs_free_slot_by_handle(esp_opp_conn_hdl_t handle);
void btc_opp_vfs_on_conn_close(esp_opp_conn_hdl_t handle);
void btc_opp_vfs_on_transfer_complete(esp_opp_conn_hdl_t handle);

#if BTC_OPP_SERVER_INCLUDED
void btc_opp_server_call_handler(btc_msg_t *msg);
void btc_opp_server_cb_handler(btc_msg_t *msg);
void btc_opp_server_callback_to_app(esp_opp_server_cb_event_t event, esp_opp_server_param_t *param);
bool btc_opp_server_is_inited(void);
#endif
#if BTC_OPP_CLIENT_INCLUDED
void btc_opp_client_call_handler(btc_msg_t *msg);
void btc_opp_client_cb_handler(btc_msg_t *msg);
void btc_opp_client_callback_to_app(esp_opp_client_cb_event_t event, esp_opp_client_param_t *param);
bool btc_opp_client_is_inited(void);
#endif
void btc_opp_get_profile_status(esp_opp_profile_status_t *param);

#endif /* BTC_OPP_INCLUDED */
