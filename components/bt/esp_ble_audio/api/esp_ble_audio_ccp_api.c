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
    esp_ble_audio_tbs_register_param_t reg_param;
    esp_err_t ret = ESP_OK;
    int err;

    if (param == NULL || bearer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* The lib only reads optional_opcodes; keep apps that still set the old field working */
    reg_param = *param;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    reg_param.optional_opcodes |= param->supported_features;
#pragma GCC diagnostic pop

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_server_register_bearer(&reg_param, bearer);
    if (err) {
        /* Map Zephyr error codes to ESP-IDF error codes */
        switch (err) {
        case -EINVAL:
            ret = ESP_ERR_INVALID_ARG;
            break;
        case -ENOMEM:
            ret = ESP_ERR_NO_MEM;
            break;
        case -EALREADY:
        case -EAGAIN:
            ret = ESP_ERR_INVALID_STATE;
            break;
        default:
            ret = ESP_FAIL;
            break;
        }
        goto end;
    }

#if BLE_AUDIO_SVC_DEFERRED_ADD
    /* Same as esp_ble_audio_tbs_register_bearer(): the host has not added this service yet */
    if (param->gtbs ? bt_le_gtbs_init() : bt_le_tbs_init()) {
        bt_ccp_call_control_server_unregister_bearer(*bearer);
        *bearer = NULL;
        ret = ESP_FAIL;
        goto end;
    }
#endif /* BLE_AUDIO_SVC_DEFERRED_ADD */

end:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_ccp_call_control_server_unregister_bearer(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer)
{
    esp_err_t ret = ESP_OK;
    int tbs_index;
    int err;

    if (bearer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    /* Read before unregistering, which invalidates it */
    tbs_index = lib_ccp_call_control_server_tbs_index_get(bearer);

    err = bt_ccp_call_control_server_unregister_bearer(bearer);
    if (err) {
        ret = ESP_FAIL;
        goto end;
    }

#if BLE_AUDIO_SVC_DEFERRED_ADD
    if (tbs_index == ESP_BLE_AUDIO_TBS_GTBS_INDEX) {
        if (bt_le_gtbs_deinit()) {
            ret = ESP_FAIL;
        }
    } else if (bt_le_tbs_deinit((uint8_t)tbs_index)) {
        ret = ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_DEFERRED_ADD */

end:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_ccp_call_control_server_set_bearer_provider_name(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer, const char *name)
{
    esp_err_t err;

    if (bearer == NULL || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_server_set_bearer_provider_name(bearer, name);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_server_get_bearer_provider_name(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer, char *name, size_t name_size)
{
    esp_err_t err;

    if (bearer == NULL || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_server_get_bearer_provider_name(bearer, name, name_size);

    bt_le_host_unlock();

    if (err) {
        switch (err) {
        case -EFAULT:
            return ESP_ERR_INVALID_STATE;
        case -ENOMEM:
            return ESP_ERR_NO_MEM;
        default:
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_ccp_call_control_server_get_bearer_uci(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer, char *uci)
{
    esp_err_t err;

    if (bearer == NULL || uci == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_server_get_bearer_uci(bearer, uci);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_client_discover(conn, out_client);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_client_register_cb(cb);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_client_unregister_cb(cb);

    bt_le_host_unlock();

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

    if (client == NULL || bearers == NULL || bearers->tbs_bearers == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_client_get_bearers(client, bearers);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_ccp_call_control_client_read_bearer_provider_name(bearer);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME */
