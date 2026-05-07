/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_ccp_api.h"

#if CONFIG_BT_CCP_CALL_CONTROL_SERVER
esp_err_t esp_ble_audio_ccp_call_control_server_register_bearer(
    const esp_ble_audio_tbs_register_param_t *param,
    esp_ble_audio_ccp_call_control_server_bearer_t **bearer)
{
    esp_err_t err;

    if (param == NULL || bearer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_server_register_bearer_safe(param, bearer);
    if (err) {
        /* Map Zephyr error codes to ESP-IDF error codes */
        switch (err) {
        case -EINVAL:
            return ESP_ERR_INVALID_ARG;
        case -ENOMEM:
            return ESP_ERR_NO_MEM;
        case -EALREADY:
            return ESP_ERR_INVALID_STATE;
        case -EAGAIN:
            return ESP_ERR_INVALID_STATE;
        default:
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_server_unregister_bearer(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer)
{
    esp_err_t err;

    if (bearer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_server_unregister_bearer_safe(bearer);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_server_set_bearer_provider_name(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer, const char *name)
{
    esp_err_t err;

    if (bearer == NULL || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_server_set_bearer_provider_name_safe(bearer, name);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_server_get_bearer_provider_name(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer, const char **name)
{
    esp_err_t err;

    if (bearer == NULL || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_server_get_bearer_provider_name_safe(bearer, name);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_CCP_CALL_CONTROL_SERVER */

#if CONFIG_BT_CCP_CALL_CONTROL_CLIENT
esp_err_t esp_ble_audio_ccp_call_control_client_discover(
    esp_ble_conn_t *conn, esp_ble_audio_ccp_call_control_client_t **out_client)
{
    esp_err_t err;

    if (conn == NULL || out_client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_client_discover_safe(conn, out_client);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_client_register_cb(
    esp_ble_audio_ccp_call_control_client_cb_t *cb)
{
    esp_err_t err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_client_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_client_unregister_cb(
    esp_ble_audio_ccp_call_control_client_cb_t *cb)
{
    esp_err_t err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_client_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_client_get_bearers(
    esp_ble_audio_ccp_call_control_client_t *client,
    esp_ble_audio_ccp_call_control_client_bearers_t *bearers)
{
    esp_err_t err;

    if (client == NULL || bearers == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_client_get_bearers_safe(client, bearers);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_CCP_CALL_CONTROL_CLIENT */

#if CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME
esp_err_t esp_ble_audio_ccp_call_control_client_read_bearer_provider_name(
    esp_ble_audio_ccp_call_control_client_bearer_t *bearer)
{
    esp_err_t err;

    if (bearer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_ccp_call_control_client_read_bearer_provider_name_safe(bearer);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME */
