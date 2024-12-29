/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_err.h"
#include "esp_bt_main.h"
#include "esp_pbac_api.h"
#include "btc/btc_manage.h"
#include "btc_pba_client.h"

#if BTC_PBA_CLIENT_INCLUDED

esp_err_t esp_pbac_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_pbac_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_pbac_register_callback(esp_pbac_callback_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_PBA_CLIENT, callback);
    return ESP_OK;
}

esp_err_t esp_pbac_connect(esp_bd_addr_t bd_addr)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (bd_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_CONNECT_EVT;

    btc_pba_client_args_t args = {0};
    memcpy(args.connect.bd_addr.address, bd_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_pba_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_pbac_disconnect(esp_pbac_conn_hdl_t handle)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (handle == ESP_PBAC_INVALID_HANDLE) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_DISCONNECT_EVT;

    btc_pba_client_args_t args = {0};
    args.disconnect.handle = handle;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_pba_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}


esp_err_t esp_pbac_pull_phone_book(esp_pbac_conn_hdl_t handle, const char *name, esp_pbac_pull_phone_book_app_param_t *app_param)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (handle == ESP_PBAC_INVALID_HANDLE || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_PULL_PHONE_BOOK_EVT;

    btc_pba_client_args_t args = {0};
    args.pull_phone_book.handle = handle;
    args.pull_phone_book.name = (char *)name;
    if (app_param != NULL) {
        args.pull_phone_book.include_app_param = true;
        memcpy(&args.pull_phone_book.app_param, app_param, sizeof(esp_pbac_pull_phone_book_app_param_t));
    }
    else {
        args.pull_phone_book.include_app_param = false;
    }

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_pba_client_args_t), btc_pba_client_args_deep_copy, btc_pba_client_args_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_pbac_set_phone_book(esp_pbac_conn_hdl_t handle, esp_pbac_set_phone_book_flags_t flags, const char *name)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    /* since ESP_PBAC_SET_PHONE_BOOK_FLAGS_ROOT is equal to ESP_PBAC_SET_PHONE_BOOK_FLAGS_DOWN, we dont check XXX_DOWN */
    if (handle == ESP_PBAC_INVALID_HANDLE || (flags != ESP_PBAC_SET_PHONE_BOOK_FLAGS_ROOT && flags != ESP_PBAC_SET_PHONE_BOOK_FLAGS_UP)) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_SET_PHONE_BOOK_EVT;

    btc_pba_client_args_t args = {0};
    args.set_phone_book.handle = handle;
    args.set_phone_book.flags =  flags;
    /* set phone book name is allowed to be NULL */
    args.set_phone_book.name = (char *)name;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_pba_client_args_t), btc_pba_client_args_deep_copy, btc_pba_client_args_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_pbac_set_phone_book2(esp_pbac_conn_hdl_t handle, const char *path)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_SET_PHONE_BOOK2_EVT;

    btc_pba_client_args_t args = {0};
    args.set_phone_book.handle = handle;
    args.set_phone_book.name = (char *)path;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_pba_client_args_t), btc_pba_client_args_deep_copy, btc_pba_client_args_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_pbac_pull_vcard_listing(esp_pbac_conn_hdl_t handle, const char *name, esp_pbac_pull_vcard_listing_app_param_t *app_param)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (handle == ESP_PBAC_INVALID_HANDLE || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_PULL_VCARD_LISTING_EVT;

    btc_pba_client_args_t args = {0};
    args.pull_vcard_listing.handle = handle;
    args.pull_vcard_listing.name = (char *)name;
    if (app_param != NULL) {
        args.pull_vcard_listing.include_app_param = true;
        memcpy(&args.pull_vcard_listing.app_param, app_param, sizeof(esp_pbac_pull_vcard_listing_app_param_t));
    }
    else {
        args.pull_vcard_listing.include_app_param = false;
    }

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_pba_client_args_t), btc_pba_client_args_deep_copy, btc_pba_client_args_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_pbac_pull_vcard_entry(esp_pbac_conn_hdl_t handle, const char *name, esp_pbac_pull_vcard_entry_app_param_t *app_param)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (handle == ESP_PBAC_INVALID_HANDLE || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = BTC_PBA_CLIENT_PULL_VCARD_ENTRY_EVT;

    btc_pba_client_args_t args = {0};
    args.pull_vcard_entry.handle = handle;
    args.pull_vcard_entry.name = (char *)name;
    if (app_param != NULL) {
        args.pull_vcard_entry.include_app_param = true;
        memcpy(&args.pull_vcard_entry.app_param, app_param, sizeof(esp_pbac_pull_vcard_entry_app_param_t));
    }
    else {
        args.pull_vcard_entry.include_app_param = false;
    }

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_pba_client_args_t), btc_pba_client_args_deep_copy, btc_pba_client_args_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

#endif
