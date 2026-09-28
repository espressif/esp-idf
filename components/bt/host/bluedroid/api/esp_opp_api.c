/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_bt_main.h"
#include "esp_opp_api.h"
#include "btc/btc_manage.h"
#include "btc_opp.h"

#if BTC_OPP_INCLUDED

static esp_err_t btc_opp_transfer(uint8_t pid, btc_opp_act_t act, btc_opp_args_t *args)
{
    btc_msg_t msg = {0};

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = pid;
    msg.act = act;
    bt_status_t stat = btc_transfer_context(&msg, args, args ? sizeof(btc_opp_args_t) : 0,
                                            btc_opp_arg_deep_copy, btc_opp_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

#if BTC_OPP_SERVER_INCLUDED

esp_err_t esp_opp_server_register_callback(esp_opp_server_callback_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_profile_cb_set(BTC_PID_OPP_SERVER, callback);
    return ESP_OK;
}

esp_err_t esp_opp_server_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    return btc_opp_transfer(BTC_PID_OPP_SERVER, BTC_OPP_SERVER_INIT_EVT, NULL);
}

esp_err_t esp_opp_server_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    return btc_opp_transfer(BTC_PID_OPP_SERVER, BTC_OPP_SERVER_DEINIT_EVT, NULL);
}

esp_err_t esp_opp_server_start(const esp_opp_server_cfg_t *cfg)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cfg->mtu != 0 && (cfg->mtu < ESP_OPP_MTU_MIN || cfg->mtu > ESP_OPP_MTU_MAX)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cfg->supported_formats_len && cfg->supported_formats == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&args.start_server.cfg, cfg, sizeof(esp_opp_server_cfg_t));
    return btc_opp_transfer(BTC_PID_OPP_SERVER, BTC_OPP_START_SERVER_EVT, &args);
}

esp_err_t esp_opp_server_stop(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    return btc_opp_transfer(BTC_PID_OPP_SERVER, BTC_OPP_STOP_SERVER_EVT, NULL);
}

esp_err_t esp_opp_server_accept(esp_opp_conn_hdl_t handle)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (handle == ESP_OPP_INVALID_HANDLE) {
        return ESP_ERR_INVALID_ARG;
    }

    args.by_handle.handle = handle;
    return btc_opp_transfer(BTC_PID_OPP_SERVER, BTC_OPP_SERVER_ACCEPT_EVT, &args);
}

esp_err_t esp_opp_server_reject(esp_opp_conn_hdl_t handle)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (handle == ESP_OPP_INVALID_HANDLE) {
        return ESP_ERR_INVALID_ARG;
    }

    args.by_handle.handle = handle;
    return btc_opp_transfer(BTC_PID_OPP_SERVER, BTC_OPP_SERVER_REJECT_EVT, &args);
}

esp_err_t esp_opp_server_cancel(esp_opp_conn_hdl_t handle)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (handle == ESP_OPP_INVALID_HANDLE) {
        return ESP_ERR_INVALID_ARG;
    }

    args.by_handle.handle = handle;
    return btc_opp_transfer(BTC_PID_OPP_SERVER, BTC_OPP_SERVER_CANCEL_EVT, &args);
}

#endif /* BTC_OPP_SERVER_INCLUDED */

#if BTC_OPP_CLIENT_INCLUDED

esp_err_t esp_opp_client_register_callback(esp_opp_client_callback_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_profile_cb_set(BTC_PID_OPP_CLIENT, callback);
    return ESP_OK;
}

esp_err_t esp_opp_client_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    return btc_opp_transfer(BTC_PID_OPP_CLIENT, BTC_OPP_CLIENT_INIT_EVT, NULL);
}

esp_err_t esp_opp_client_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    return btc_opp_transfer(BTC_PID_OPP_CLIENT, BTC_OPP_CLIENT_DEINIT_EVT, NULL);
}

esp_err_t esp_opp_client_connect(const esp_opp_client_connect_param_t *param)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (param->mtu != 0 && (param->mtu < ESP_OPP_MTU_MIN || param->mtu > ESP_OPP_MTU_MAX)) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&args.connect.param, param, sizeof(esp_opp_client_connect_param_t));
    return btc_opp_transfer(BTC_PID_OPP_CLIENT, BTC_OPP_CLIENT_CONNECT_EVT, &args);
}

esp_err_t esp_opp_client_disconnect(esp_opp_conn_hdl_t handle)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (handle == ESP_OPP_INVALID_HANDLE) {
        return ESP_ERR_INVALID_ARG;
    }

    args.by_handle.handle = handle;
    return btc_opp_transfer(BTC_PID_OPP_CLIENT, BTC_OPP_CLIENT_DISCONNECT_EVT, &args);
}

esp_err_t esp_opp_client_open_object(const esp_opp_client_object_cfg_t *cfg)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (cfg == NULL || cfg->handle == ESP_OPP_INVALID_HANDLE || cfg->name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(cfg->name) > ESP_OPP_MAX_NAME_LEN) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cfg->type != NULL && strlen(cfg->type) > ESP_OPP_MAX_TYPE_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&args.open_object.object, cfg, sizeof(esp_opp_client_object_cfg_t));
    return btc_opp_transfer(BTC_PID_OPP_CLIENT, BTC_OPP_OPEN_OBJECT_EVT, &args);
}

esp_err_t esp_opp_client_cancel(esp_opp_conn_hdl_t handle)
{
    btc_opp_args_t args = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (handle == ESP_OPP_INVALID_HANDLE) {
        return ESP_ERR_INVALID_ARG;
    }

    args.by_handle.handle = handle;
    return btc_opp_transfer(BTC_PID_OPP_CLIENT, BTC_OPP_CLIENT_CANCEL_EVT, &args);
}

#endif /* BTC_OPP_CLIENT_INCLUDED */

esp_err_t esp_opp_get_profile_status(esp_opp_profile_status_t *profile_status)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (profile_status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(profile_status, 0, sizeof(esp_opp_profile_status_t));
    btc_opp_get_profile_status(profile_status);
    return ESP_OK;
}

#endif /* BTC_OPP_INCLUDED */
