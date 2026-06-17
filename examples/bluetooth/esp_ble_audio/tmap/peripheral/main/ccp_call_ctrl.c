/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2022 Codecoup
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "tmap_peripheral.h"

#define URI_SEPARATOR   ":"
#define CALLER_ID       "friend"

static uint8_t new_call_index;
static char remote_uri[CONFIG_BT_TBS_MAX_URI_LENGTH];

static void discover_cb(esp_ble_conn_t *conn,
                        int err,
                        uint8_t tbs_count,
                        bool gtbs_found)
{
    if (err) {
        ESP_LOGE(TAG, "Discover GTBS failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Discovered GTBS");

    if (gtbs_found == false) {
        ESP_LOGE(TAG, "No GTBS found");
        return;
    }

    /* Read Bearer URI Schemes Supported List Characteristic */
    err = esp_ble_audio_tbs_client_read_uri_list(default_conn_handle_get(),
                                                 ESP_BLE_AUDIO_TBS_GTBS_INDEX);
    if (err) {
        ESP_LOGE(TAG, "Failed to read URI list, err %d", err);
    }
}

static void originate_call_cb(esp_ble_conn_t *conn,
                              int err,
                              uint8_t inst_index,
                              uint8_t call_index)
{
    if (err) {
        ESP_LOGE(TAG, "Originate call failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Call %u originated", call_index);

    if (inst_index != ESP_BLE_AUDIO_TBS_GTBS_INDEX) {
        ESP_LOGW(TAG, "Unexpected instance index %u", inst_index);
        return;
    }

    new_call_index = call_index;
}

static void terminate_call_cb(esp_ble_conn_t *conn,
                              int err,
                              uint8_t inst_index,
                              uint8_t call_index)
{
    if (err) {
        ESP_LOGE(TAG, "Terminate call failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Call %u terminated", call_index);

    if (inst_index != ESP_BLE_AUDIO_TBS_GTBS_INDEX) {
        ESP_LOGW(TAG, "Unexpected instance index %u", inst_index);
        return;
    }
}

static void read_uri_schemes_string_cb(esp_ble_conn_t *conn,
                                       int err,
                                       uint8_t inst_index,
                                       const char *value)
{
    size_t i;

    if (err) {
        ESP_LOGE(TAG, "Read URI schemes string failed, err %d", err);
        return;
    }

    if (inst_index != ESP_BLE_AUDIO_TBS_GTBS_INDEX) {
        ESP_LOGW(TAG, "Unexpected instance index %u", inst_index);
        return;
    }

    /* Save first remote URI
     *
     * First search for the first comma (separator), and use that to determine the end of the
     * first (or only) URI. Then use that length to copy the URI to `remote_uri` for later use.
     */
    for (i = 0; i < strlen(value); i++) {
        if (value[i] == ',') {
            break;
        }
    }

    if (i >= sizeof(remote_uri)) {
        ESP_LOGW(TAG, "Cannot store URI of length %zu: %s", i, value);
        return;
    }

    strncpy(remote_uri, value, i);
    remote_uri[i] = '\0';

    ESP_LOGI(TAG, "Discovered remote URI %s", remote_uri);

    /* CCP related GATT procedures done, start MCP discovery */
    mcp_discover_mcs();
}

static esp_ble_audio_tbs_client_cb_t tbs_client_cb = {
    .discover       = discover_cb,
    .uri_list       = read_uri_schemes_string_cb,
    .originate_call = originate_call_cb,
    .terminate_call = terminate_call_cb,
};

int ccp_call_ctrl_init(void)
{
    int err;

    err = esp_ble_audio_tbs_client_register_cb(&tbs_client_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register TBS client, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CCP call controller initialized");

    return err;
}

int ccp_discover_tbs(void)
{
    uint16_t conn_handle;
    int err;

    conn_handle = default_conn_handle_get();
    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    err = esp_ble_audio_tbs_client_discover(conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover TBS, err %d", err);
        return err;
    }

    return err;
}

int ccp_originate_call(void)
{
    char uri[CONFIG_BT_TBS_MAX_URI_LENGTH] = {0};
    uint16_t conn_handle;
    int uri_len;
    int err;

    conn_handle = default_conn_handle_get();
    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    uri_len = snprintf(uri, sizeof(uri), "%s%s%s", remote_uri, URI_SEPARATOR, CALLER_ID);
    if (uri_len < 0 || uri_len >= sizeof(uri)) {
        ESP_LOGE(TAG, "Invalid URI length %d", uri_len);
        return -EINVAL;
    }

    err = esp_ble_audio_tbs_client_originate_call(conn_handle,
                                                  ESP_BLE_AUDIO_TBS_GTBS_INDEX,
                                                  uri);
    if (err) {
        ESP_LOGE(TAG, "Failed to originate TBS call, err %d", err);
        return err;
    }

    return 0;
}

int ccp_terminate_call(void)
{
    uint16_t conn_handle;
    int err;

    conn_handle = default_conn_handle_get();
    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    err = esp_ble_audio_tbs_client_terminate_call(conn_handle,
                                                  ESP_BLE_AUDIO_TBS_GTBS_INDEX,
                                                  new_call_index);
    if (err) {
        ESP_LOGE(TAG, "Failed to terminate TBS call, err %d", err);
        return err;
    }

    return 0;
}
