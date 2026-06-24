/*
 * SPDX-FileCopyrightText: 2022-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tmap_bms.h"

#define LOCAL_BROADCAST_ID      0x123456

static esp_ble_audio_cap_initiator_broadcast_subgroup_param_t subgroup_param;
static esp_ble_audio_cap_initiator_broadcast_stream_param_t stream_params;
static esp_ble_audio_cap_initiator_broadcast_create_param_t create_param;
static esp_ble_audio_cap_broadcast_source_t *broadcast_source;
static esp_ble_audio_cap_stream_t broadcast_stream;

static uint8_t bis_codec_data[] = {
    ESP_BLE_AUDIO_CODEC_DATA(ESP_BLE_AUDIO_CODEC_CFG_FREQ,
                             EXAMPLE_BYTES_LIST_LE16(ESP_BLE_AUDIO_CODEC_CFG_FREQ_48KHZ))
};

static uint8_t new_metadata[] = {
    ESP_BLE_AUDIO_CODEC_DATA(ESP_BLE_AUDIO_METADATA_TYPE_STREAM_CONTEXT,
                             EXAMPLE_BYTES_LIST_LE16(ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA))
};

ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_2_1_DEFINE(broadcast_preset_48_2_1,
                                                     ESP_BLE_AUDIO_LOCATION_FRONT_LEFT,
                                                     ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);

static example_audio_tx_scheduler_t tx_scheduler;
static uint16_t tx_seq_num;
static uint8_t *iso_data;

static void broadcast_audio_tx(void)
{
    uint16_t sdu_len;
    int err;

    if (iso_data == NULL) {
        return;
    }

    if (broadcast_stream.bap_stream.qos == NULL || broadcast_stream.bap_stream.qos->sdu == 0) {
        ESP_LOGE(TAG, "Invalid stream qos");
        return;
    }

    sdu_len = broadcast_stream.bap_stream.qos->sdu;

    memset(iso_data, (uint8_t)tx_seq_num, sdu_len);

    err = esp_ble_audio_cap_stream_send(&broadcast_stream,
                                        iso_data, sdu_len,
                                        tx_seq_num);
    if (err) {
        ESP_LOGD(TAG, "Failed to transmit data on streams, err %d", err);
        return;
    }

    tx_seq_num++;
}

static void broadcast_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[SRC #0] Stream started");

    if (stream->qos == NULL || stream->qos->sdu == 0) {
        ESP_LOGE(TAG, "[SRC #0] Invalid stream qos");
        return;
    }

    if (iso_data == NULL) {
        iso_data = calloc(1, stream->qos->sdu);
        if (iso_data == NULL) {
            ESP_LOGE(TAG, "[SRC #0] Failed to alloc TX buffer, SDU %u", stream->qos->sdu);
            return;
        }
    }

    tx_seq_num = 0;
    example_audio_tx_scheduler_reset(&tx_scheduler);

    /* Note: esp timer is not accurate enough */
    err = example_audio_tx_scheduler_start(&tx_scheduler, stream->qos->interval);
    if (err) {
        ESP_LOGE(TAG, "[SRC #0] Scheduler start failed, err %d", err);
        return;
    }

    broadcast_audio_tx();
}

static void broadcast_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[SRC #0] Stream stopped, reason 0x%02x", reason);

    err = example_audio_tx_scheduler_stop(&tx_scheduler);
    if (err) {
        ESP_LOGE(TAG, "[SRC #0] Scheduler stop failed, err %d", err);
    }

    if (iso_data != NULL) {
        free(iso_data);
        iso_data = NULL;
    }
}

static void broadcast_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[SRC #0] ISO disconnected, reason 0x%02x", reason);

    err = example_audio_tx_scheduler_stop(&tx_scheduler);
    if (err) {
        ESP_LOGE(TAG, "[SRC #0] Scheduler stop failed, err %d", err);
    }

    if (iso_data != NULL) {
        free(iso_data);
        iso_data = NULL;
    }
}

static void broadcast_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    example_audio_tx_scheduler_on_sent(&tx_scheduler, user_data, TAG, "SRC #0");
}

static esp_ble_audio_bap_stream_ops_t broadcast_stream_ops = {
    .started      = broadcast_started_cb,
    .stopped      = broadcast_stopped_cb,
    .sent         = broadcast_sent_cb,
    .disconnected = broadcast_disconnected_cb,
};

static uint8_t *ext_adv_data_get(uint8_t *data_len)
{
    uint32_t broadcast_id;
    uint8_t *data;

    broadcast_id = LOCAL_BROADCAST_ID;

    /* - TMAP UUID (2 octets)
     * - TMAP Role (2 octets)
     * - Broadcast Audio Announcement Service UUID (2 octets)
     * - Broadcast ID (3 octets)
     * - Complete Device Name
     */
    *data_len = 6 + 7 + 2 + strlen(LOCAL_DEVICE_NAME);

    data = calloc(1, *data_len);
    if (data == NULL) {
        return NULL;
    }

    data[0] = 0x05; /* 1 + 2 + 2 */
    data[1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    data[2] = (ESP_BLE_AUDIO_UUID_TMAS_VAL & 0xFF);
    data[3] = ((ESP_BLE_AUDIO_UUID_TMAS_VAL >> 8) & 0xFF);
    data[4] = (ESP_BLE_AUDIO_TMAP_ROLE_BMS & 0xFF);
    data[5] = ((ESP_BLE_AUDIO_TMAP_ROLE_BMS >> 8) & 0xFF);

    data[6] = 0x06; /* 1 + 2 + 3 */
    data[7] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    data[8] = (ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL & 0xFF);
    data[9] = ((ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL >> 8) & 0xFF);
    data[10] = (broadcast_id & 0xFF);
    data[11] = ((broadcast_id >> 8) & 0xFF);
    data[12] = ((broadcast_id >> 16) & 0xFF);

    data[13] = strlen(LOCAL_DEVICE_NAME) + 1;
    data[14] = EXAMPLE_AD_TYPE_NAME_COMPLETE;
    memcpy(data + 15, LOCAL_DEVICE_NAME, strlen(LOCAL_DEVICE_NAME));

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

int cap_initiator_setup(void)
{
    esp_ble_audio_bap_broadcast_adv_info_t info = {
        .adv_handle = ADV_HANDLE,
    };
    bool adv_started = false;
    bool adv_added = false;
    /* BASE (per_adv_data_get) needs broadcast_source to exist — build after create. */
    uint8_t *ext_data = NULL;
    uint8_t *per_data = NULL;
    uint8_t ext_len = 0;
    uint8_t per_len = 0;
    int err;

    stream_params.stream = &broadcast_stream;
    stream_params.data_len = ARRAY_SIZE(bis_codec_data);
    stream_params.data = bis_codec_data;

    subgroup_param.stream_count = 1;
    subgroup_param.stream_params = &stream_params;
    subgroup_param.codec_cfg = &broadcast_preset_48_2_1.codec_cfg;

    create_param.subgroup_count = 1;
    create_param.subgroup_params = &subgroup_param;
    create_param.qos = &broadcast_preset_48_2_1.qos;
    create_param.packing = ESP_BLE_ISO_PACKING_SEQUENTIAL;
    create_param.encryption = false;

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

    adv_started = true;

    err = esp_ble_audio_bap_broadcast_adv_add(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to add adv for broadcast source, err %d", err);
        goto end;
    }

    adv_added = true;

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

    if (err == 0) {
        ESP_LOGI(TAG, "CAP initiator setup");
        return 0;
    }

    if (adv_added) {
        esp_err_t e = esp_ble_audio_bap_broadcast_adv_delete(&info);
        if (e) {
            ESP_LOGE(TAG, "Failed to delete adv for broadcast source, err %d", e);
        }
    }

    if (adv_started) {
        ext_adv_stop();
    }

    esp_ble_audio_cap_initiator_broadcast_audio_delete(broadcast_source);
    broadcast_source = NULL;

    if (iso_data != NULL) {
        free(iso_data);
        iso_data = NULL;
    }

    return err;
}

int cap_initiator_update(void)
{
    int err;

    err = esp_ble_audio_cap_initiator_broadcast_audio_update(broadcast_source,
                                                             new_metadata,
                                                             ARRAY_SIZE(new_metadata));
    if (err) {
        ESP_LOGE(TAG, "Failed to update broadcast source metadata, err %d", err);
    }

    return err;
}

int cap_initiator_stop(void)
{
    esp_ble_audio_bap_broadcast_adv_info_t info = {
        .adv_handle = ADV_HANDLE,
    };
    int err;

    err = esp_ble_audio_cap_initiator_broadcast_audio_stop(broadcast_source);
    if (err) {
        ESP_LOGE(TAG, "Failed to stop broadcast source, err %d", err);
        return err;
    }

    err = esp_ble_audio_cap_initiator_broadcast_audio_delete(broadcast_source);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete broadcast source, err %d", err);
        return err;
    }

    broadcast_source = NULL;

    err = esp_ble_audio_bap_broadcast_adv_delete(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete adv for broadcast source, err %d", err);
        return err;
    }

    err = ext_adv_stop();
    if (err) {
        return err;
    }

    if (iso_data != NULL) {
        free(iso_data);
        iso_data = NULL;
    }

    return 0;
}

static void tx_scheduler_cb(void *arg)
{
    (void)arg;
    broadcast_audio_tx();
}

int cap_initiator_init(void)
{
    esp_err_t err;

    esp_ble_audio_cap_stream_ops_register(&broadcast_stream, &broadcast_stream_ops);

    err = example_audio_tx_scheduler_init(&tx_scheduler,
                                          tx_scheduler_cb,
                                          NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to init tx scheduler, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CAP initiator initialized");

    return 0;
}
