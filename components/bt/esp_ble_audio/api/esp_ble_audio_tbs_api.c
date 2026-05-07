/*
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_tbs_api.h"

#if CONFIG_BT_TBS
esp_err_t esp_ble_audio_tbs_accept(uint8_t call_index)
{
    int err;

    err = bt_tbs_accept_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_hold(uint8_t call_index)
{
    int err;

    err = bt_tbs_hold_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_retrieve(uint8_t call_index)
{
    int err;

    err = bt_tbs_retrieve_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_terminate(uint8_t call_index)
{
    int err;

    err = bt_tbs_terminate_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_originate(uint8_t bearer_index,
                                      char *uri,
                                      uint8_t *call_index)
{
    int err;

    if (uri == NULL || call_index == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_originate_safe(bearer_index, uri, call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_join(uint8_t call_index_cnt, uint8_t *call_indexes)
{
    int err;

    if (call_index_cnt == 0 || call_indexes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_join_safe(call_index_cnt, call_indexes);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_remote_answer(uint8_t call_index)
{
    int err;

    err = bt_tbs_remote_answer_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_remote_hold(uint8_t call_index)
{
    int err;

    err = bt_tbs_remote_hold_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_remote_retrieve(uint8_t call_index)
{
    int err;

    err = bt_tbs_remote_retrieve_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_remote_terminate(uint8_t call_index)
{
    int err;

    err = bt_tbs_remote_terminate_safe(call_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_remote_incoming(uint8_t bearer_index,
                                            const char *to,
                                            const char *from,
                                            const char *friendly_name,
                                            uint8_t *call_index)
{
    int ret;

    if (to == NULL || from == NULL ||
            friendly_name == NULL || call_index == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_tbs_remote_incoming_safe(bearer_index, to, from, friendly_name);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *call_index = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_set_bearer_provider_name(uint8_t bearer_index, const char *name)
{
    int err;

    if (name == NULL || strlen(name) == 0 ||
            strlen(name) >= CONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_set_bearer_provider_name_safe(bearer_index, name);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_set_bearer_technology(uint8_t bearer_index, uint8_t new_technology)
{
    int err;

    if (new_technology < ESP_BLE_AUDIO_TBS_TECHNOLOGY_3G ||
            new_technology > ESP_BLE_AUDIO_TBS_TECHNOLOGY_WCDMA) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_set_bearer_technology_safe(bearer_index, new_technology);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_set_signal_strength(uint8_t bearer_index, uint8_t new_signal_strength)
{
    int err;

    if (new_signal_strength > ESP_BLE_AUDIO_TBS_SIGNAL_STRENGTH_MAX &&
            new_signal_strength != ESP_BLE_AUDIO_TBS_SIGNAL_STRENGTH_UNKNOWN) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_set_signal_strength_safe(bearer_index, new_signal_strength);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#define TBS_VALID_STATUS_FLAGS(val)     ((val) <= (BIT(0) | BIT(1)))

esp_err_t esp_ble_audio_tbs_set_status_flags(uint8_t bearer_index, uint16_t status_flags)
{
    int err;

    if (TBS_VALID_STATUS_FLAGS(status_flags) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_set_status_flags_safe(bearer_index, status_flags);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_set_uri_scheme_list(uint8_t bearer_index,
                                                const char **uri_list,
                                                uint8_t uri_count)
{
    uint8_t count = CONFIG_BT_TBS_BEARER_COUNT;
    int err;

    if (count == 0 || bearer_index >= count ||
            (uri_count && uri_list == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_set_uri_scheme_list_safe(bearer_index, uri_list, uri_count);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

void esp_ble_audio_tbs_register_cb(esp_ble_audio_tbs_cb_t *cbs)
{
    bt_tbs_register_cb_safe(cbs);
}

static bool valid_register_param(const esp_ble_audio_tbs_register_param_t *param)
{
    if (param == NULL) {
        return false;
    }

    if (param->provider_name == NULL) {
        return false;
    }

    if (strlen(param->provider_name) == 0 ||
            strlen(param->provider_name) >= CONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH) {
        /* Provider name empty or length not less than CONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH */
        return false;
    }

    if (param->uci == NULL) {
        return false;
    }

    if (param->uri_schemes_supported == NULL) {
        return false;
    }

    if (IN_RANGE(param->technology,
                 ESP_BLE_AUDIO_TBS_TECHNOLOGY_3G,
                 ESP_BLE_AUDIO_TBS_TECHNOLOGY_WCDMA) == false) {
        return false;
    }

    if (param->supported_features > ESP_BLE_AUDIO_TBS_FEATURE_ALL) {
        return false;
    }

    if (CONFIG_BT_TBS_BEARER_COUNT == 0 && param->gtbs == false) {
        return false;
    }

    return true;
}

esp_err_t esp_ble_audio_tbs_register_bearer(const esp_ble_audio_tbs_register_param_t *param,
                                            uint8_t *bearer_index)
{
    int ret;

    if (param == NULL || valid_register_param(param) == false || bearer_index == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Note: currently only GTBS is supported */
    if (param->gtbs == false) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_tbs_register_bearer_safe(param);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *bearer_index = ret;

#if BLE_AUDIO_SVC_SEP_ADD
    if (bt_le_gtbs_init()) {
        bt_tbs_unregister_bearer_safe(*bearer_index);
        return ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}

esp_err_t esp_ble_audio_tbs_unregister_bearer(uint8_t bearer_index)
{
    int err;

    err = bt_tbs_unregister_bearer_safe(bearer_index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_TBS_CLIENT
esp_err_t esp_ble_audio_tbs_client_discover(uint16_t conn_handle)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_discover(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

#if CONFIG_BT_TBS_CLIENT_SET_BEARER_SIGNAL_INTERVAL
esp_err_t esp_ble_audio_tbs_client_set_signal_strength_interval(uint16_t conn_handle,
                                                                uint8_t inst_index,
                                                                uint8_t interval)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_set_signal_strength_interval(conn, inst_index, interval);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_SET_BEARER_SIGNAL_INTERVAL */

#if CONFIG_BT_TBS_CLIENT_ORIGINATE_CALL
#define TBS_MIN_URI_LEN             3   /* a:b */
#define FIRST_PRINTABLE_ASCII_CHAR  ' ' /* space */

static inline bool tbs_valid_uri(const uint8_t *uri, size_t uri_len)
{
    if (uri_len > CONFIG_BT_TBS_MAX_URI_LENGTH || uri_len < TBS_MIN_URI_LEN) {
        return false;
    }

    if (uri[0] < FIRST_PRINTABLE_ASCII_CHAR) {
        /* Invalid first char */
        return false;
    }

    for (size_t i = 1; i < uri_len; i++) {
        if (uri[i] == ':' && uri[i + 1] >= FIRST_PRINTABLE_ASCII_CHAR) {
            return true;
        }
    }

    return false;
}

esp_err_t esp_ble_audio_tbs_client_originate_call(uint16_t conn_handle,
                                                  uint8_t inst_index,
                                                  const char *uri)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (uri == NULL ||
            tbs_valid_uri((const uint8_t *)uri, strlen(uri)) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_originate_call(conn, inst_index, uri);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_ORIGINATE_CALL */

#if CONFIG_BT_TBS_CLIENT_TERMINATE_CALL
esp_err_t esp_ble_audio_tbs_client_terminate_call(uint16_t conn_handle,
                                                  uint8_t inst_index,
                                                  uint8_t call_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_terminate_call(conn, inst_index, call_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_TERMINATE_CALL */

#if CONFIG_BT_TBS_CLIENT_HOLD_CALL
esp_err_t esp_ble_audio_tbs_client_hold_call(uint16_t conn_handle,
                                             uint8_t inst_index,
                                             uint8_t call_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_hold_call(conn, inst_index, call_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_HOLD_CALL */

#if CONFIG_BT_TBS_CLIENT_ACCEPT_CALL
esp_err_t esp_ble_audio_tbs_client_accept_call(uint16_t conn_handle,
                                               uint8_t inst_index,
                                               uint8_t call_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_accept_call(conn, inst_index, call_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_ACCEPT_CALL */

#if CONFIG_BT_TBS_CLIENT_RETRIEVE_CALL
esp_err_t esp_ble_audio_tbs_client_retrieve_call(uint16_t conn_handle,
                                                 uint8_t inst_index,
                                                 uint8_t call_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_retrieve_call(conn, inst_index, call_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_RETRIEVE_CALL */

#if CONFIG_BT_TBS_CLIENT_JOIN_CALLS
esp_err_t esp_ble_audio_tbs_client_join_calls(uint16_t conn_handle,
                                              uint8_t inst_index,
                                              const uint8_t *call_indexes,
                                              uint8_t count)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (call_indexes == NULL ||
            count < 2 || count > CONFIG_BT_TBS_CLIENT_MAX_CALLS) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_join_calls(conn, inst_index, call_indexes, count);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_JOIN_CALLS */

#if CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME
esp_err_t esp_ble_audio_tbs_client_read_bearer_provider_name(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_bearer_provider_name(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME */

#if CONFIG_BT_TBS_CLIENT_BEARER_UCI
esp_err_t esp_ble_audio_tbs_client_read_bearer_uci(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_bearer_uci(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_UCI */

#if CONFIG_BT_TBS_CLIENT_BEARER_TECHNOLOGY
esp_err_t esp_ble_audio_tbs_client_read_technology(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_technology(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_TECHNOLOGY */

#if CONFIG_BT_TBS_CLIENT_BEARER_URI_SCHEMES_SUPPORTED_LIST
esp_err_t esp_ble_audio_tbs_client_read_uri_list(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_uri_list(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_URI_SCHEMES_SUPPORTED_LIST */

#if CONFIG_BT_TBS_CLIENT_BEARER_SIGNAL_STRENGTH
esp_err_t esp_ble_audio_tbs_client_read_signal_strength(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_signal_strength(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_SIGNAL_STRENGTH */

#if CONFIG_BT_TBS_CLIENT_READ_BEARER_SIGNAL_INTERVAL
esp_err_t esp_ble_audio_tbs_client_read_signal_interval(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_signal_interval(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_READ_BEARER_SIGNAL_INTERVAL */

#if CONFIG_BT_TBS_CLIENT_BEARER_LIST_CURRENT_CALLS
esp_err_t esp_ble_audio_tbs_client_read_current_calls(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_current_calls(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_LIST_CURRENT_CALLS */

#if CONFIG_BT_TBS_CLIENT_CCID
esp_err_t esp_ble_audio_tbs_client_read_ccid(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_ccid(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_CCID */

#if CONFIG_BT_TBS_CLIENT_INCOMING_URI
esp_err_t esp_ble_audio_tbs_client_read_call_uri(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_call_uri(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_INCOMING_URI */

#if CONFIG_BT_TBS_CLIENT_STATUS_FLAGS
esp_err_t esp_ble_audio_tbs_client_read_status_flags(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_status_flags(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_STATUS_FLAGS */

esp_err_t esp_ble_audio_tbs_client_read_call_state(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_call_state(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

#if CONFIG_BT_TBS_CLIENT_INCOMING_CALL
esp_err_t esp_ble_audio_tbs_client_read_remote_uri(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_remote_uri(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_INCOMING_CALL */

#if CONFIG_BT_TBS_CLIENT_CALL_FRIENDLY_NAME
esp_err_t esp_ble_audio_tbs_client_read_friendly_name(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_friendly_name(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_CALL_FRIENDLY_NAME */

#if CONFIG_BT_TBS_CLIENT_OPTIONAL_OPCODES
esp_err_t esp_ble_audio_tbs_client_read_optional_opcodes(uint16_t conn_handle, uint8_t inst_index)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tbs_client_read_optional_opcodes(conn, inst_index);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_OPTIONAL_OPCODES */

esp_err_t esp_ble_audio_tbs_client_register_cb(esp_ble_audio_tbs_client_cb_t *cbs)
{
    int err;

    if (cbs == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_tbs_client_register_cb_safe(cbs);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_TBS_CLIENT_CCID
esp_ble_audio_tbs_instance_t *esp_ble_audio_tbs_client_get_by_ccid(uint16_t conn_handle, uint8_t ccid)
{
    esp_ble_audio_tbs_instance_t *ret = NULL;
    void *conn;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        goto unlock;
    }

    ret = lib_tbs_client_get_by_ccid(conn, ccid);

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_TBS_CLIENT_CCID */
#endif /* CONFIG_BT_TBS_CLIENT */
