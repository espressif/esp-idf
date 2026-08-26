/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "cap_initiator.h"

#define LOCAL_BROADCAST_ID      0x123456

ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_16_2_1_DEFINE(broadcast_preset_16_2_1,
                                                     ESP_BLE_AUDIO_LOCATION_MONO_AUDIO,
                                                     ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED);

static esp_ble_audio_cap_broadcast_source_t *broadcast_source;
static esp_ble_audio_cap_stream_t broadcast_stream;

static void broadcast_stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_cap_stream_t *cap_stream;
    int err;

    ESP_LOGI(TAG, "[SRC #0] Stream started");

    cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

    err = cap_initiator_tx_register_stream(cap_stream, true);
    if (err) {
        ESP_LOGE(TAG, "[SRC #0] Failed to register TX, err %d", err);
    }
}

static void broadcast_stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_ble_audio_cap_stream_t *cap_stream;

    ESP_LOGI(TAG, "[SRC #0] Stream stopped, reason 0x%02x", reason);

    cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

    (void)cap_initiator_tx_unregister_stream(cap_stream);
}

static void broadcast_stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_ble_audio_cap_stream_t *cap_stream;

    ESP_LOGI(TAG, "[SRC #0] ISO disconnected, reason 0x%02x", reason);

    cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

    (void)cap_initiator_tx_unregister_stream(cap_stream);
}

static void broadcast_stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    cap_initiator_tx_stream_sent(stream, user_data);
}

static esp_ble_audio_bap_stream_ops_t broadcast_stream_ops = {
    .started      = broadcast_stream_started_cb,
    .stopped      = broadcast_stream_stopped_cb,
    .sent         = broadcast_stream_sent_cb,
    .disconnected = broadcast_stream_disconnected_cb,
};

static uint8_t *ext_adv_data_get(uint8_t *data_len)
{
    uint32_t broadcast_id;
    uint8_t *data;

    broadcast_id = LOCAL_BROADCAST_ID;

    /* - Broadcast Audio Announcement Service UUID (2 octets)
     * - Broadcast ID (3 octets)
     * - Complete Device Name
     */
    *data_len = 7 + 2 + strlen(LOCAL_DEVICE_NAME);

    data = calloc(1, *data_len);
    if (data == NULL) {
        return NULL;
    }

    data[0] = 0x06; /* 1 + 2 + 3 */
    data[1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    data[2] = (ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL & 0xFF);
    data[3] = ((ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL >> 8) & 0xFF);
    data[4] = (broadcast_id & 0xFF);
    data[5] = ((broadcast_id >> 8) & 0xFF);
    data[6] = ((broadcast_id >> 16) & 0xFF);

    data[7] = strlen(LOCAL_DEVICE_NAME) + 1;
    data[8] = EXAMPLE_AD_TYPE_NAME_COMPLETE;
    memcpy(data + 9, LOCAL_DEVICE_NAME, strlen(LOCAL_DEVICE_NAME));

    return data;
}

static uint8_t *per_adv_data_get(uint8_t *data_len)
{
    NET_BUF_SIMPLE_DEFINE(base_buf, 128);
    uint8_t *data;
    esp_err_t err;

    /* Broadcast Audio Announcement Service UUID (2 octets) and
     * Broadcast Audio Source Endpoint (BASE)
     */

    err = esp_ble_audio_cap_initiator_broadcast_get_base(broadcast_source, &base_buf);
    if (err) {
        ESP_LOGE(TAG, "Failed to get encoded BASE, err %d", err);
        return NULL;
    }

    *data_len = 2 + base_buf.len;

    data = calloc(1, *data_len);
    if (data == NULL) {
        return NULL;
    }

    /* base_buf.len has included the UUID length (2 octets) */
    data[0] = 1 + base_buf.len;
    data[1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    memcpy(data + 2, base_buf.data, base_buf.len);

    return data;
}

int cap_initiator_broadcast_start(void)
{
    esp_ble_audio_cap_initiator_broadcast_stream_param_t stream_params = {
        .stream = &broadcast_stream,
    };
    esp_ble_audio_cap_initiator_broadcast_subgroup_param_t subgroup_param = {
        .codec_cfg = &broadcast_preset_16_2_1.codec_cfg,
        .stream_params = &stream_params,
        .stream_count = 1,
    };
    const esp_ble_audio_cap_initiator_broadcast_create_param_t create_param = {
        .qos = &broadcast_preset_16_2_1.qos,
        .subgroup_params = &subgroup_param,
        .subgroup_count = 1,
    };
    esp_ble_audio_bap_broadcast_adv_info_t info = {
        .adv_handle = ADV_HANDLE,
    };
    /* BASE (per_adv_data_get) needs broadcast_source to exist — build after create. */
    uint8_t *ext_data = NULL;
    uint8_t *per_data = NULL;
    uint8_t ext_len = 0;
    uint8_t per_len = 0;
    int err;

    ESP_LOGI(TAG, "Creating broadcast source");

    err = esp_ble_audio_cap_initiator_broadcast_audio_create(&create_param, &broadcast_source);
    if (err) {
        ESP_LOGE(TAG, "Failed to create broadcast source, err %d", err);
        return err;
    }

    ext_data = ext_adv_data_get(&ext_len);
    if (ext_data == NULL) {
        err = -ENOMEM;
        goto end;
    }

    per_data = per_adv_data_get(&per_len);
    if (per_data == NULL) {
        err = -ENOMEM;
        goto end;
    }

    err = ext_adv_start(ext_data, ext_len, per_data, per_len);
    if (err) {
        goto end;
    }

    err = esp_ble_audio_bap_broadcast_adv_add(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to add adv for broadcast source, err %d", err);
        goto end;
    }

    err = esp_ble_audio_cap_initiator_broadcast_audio_start(broadcast_source, ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to start broadcast source, err %d", err);
    }

end:
    if (ext_data != NULL) {
        free(ext_data);
    }
    if (per_data != NULL) {
        free(per_data);
    }
    if (err) {
        esp_err_t e = esp_ble_audio_cap_initiator_broadcast_audio_delete(broadcast_source);
        if (e) {
            ESP_LOGE(TAG, "Failed to delete broadcast source, err %d", e);
        }
    }
    return err;
}

int cap_initiator_broadcast_init(void)
{
    esp_ble_audio_cap_stream_ops_register(&broadcast_stream, &broadcast_stream_ops);

    ESP_LOGI(TAG, "CAP initiator broadcast initialized");

    return 0;
}
