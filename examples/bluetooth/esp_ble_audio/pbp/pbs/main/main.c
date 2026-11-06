/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2026 Infineon Technologies AG, or an affiliate of Infineon Technologies AG.
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"

#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_bap_lc3_preset_defs.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_codec_api.h"
#include "esp_ble_audio_pbp_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#include "adv.h"

#define LOCAL_BROADCAST_NAME     "PBP Broadcast Source"
#define LOCAL_BROADCAST_ID       0x123456

ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_2_1_DEFINE(broadcast_preset_48_2_1,
                                                     ESP_BLE_AUDIO_LOCATION_FRONT_LEFT,
                                                     ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);

static esp_ble_audio_cap_broadcast_source_t *broadcast_source;
static esp_ble_audio_cap_stream_t broadcast_stream;

static example_audio_tx_scheduler_t tx_scheduler;
static example_audio_tx_count_t tx_count;
static uint16_t tx_seq_num;
static uint8_t *iso_data;

static const uint8_t pba_metadata[] = {
    ESP_BLE_AUDIO_CODEC_DATA(ESP_BLE_AUDIO_METADATA_TYPE_PROGRAM_INFO, 'P', 'B', 'P'),
};

static bool stream_is_streaming(const esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};

    if (stream->ep == NULL) {
        return false;
    }

    if (esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info)) {
        return false;
    }

    return (ep_info.state == ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING);
}

static void broadcast_source_tx(esp_ble_audio_cap_stream_t *stream)
{
    esp_err_t err;

    if (iso_data == NULL || !stream_is_streaming(&stream->bap_stream)) {
        return;
    }

    if (stream->bap_stream.qos == NULL || stream->bap_stream.qos->sdu == 0) {
        ESP_LOGE(TAG, "[SRC #0] Invalid QoS");
        return;
    }

    memset(iso_data, (uint8_t)tx_seq_num, stream->bap_stream.qos->sdu);

    err = esp_ble_audio_cap_stream_send(stream, iso_data,
                                        stream->bap_stream.qos->sdu, tx_seq_num);
    if (err) {
        ESP_LOGD(TAG, "[SRC #0] send failed, err %d", err);
        return;
    }

    tx_seq_num++;
}

static void stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[SRC #0] Stream started");

    if (stream->qos == NULL || stream->qos->sdu == 0) {
        ESP_LOGE(TAG, "[SRC #0] Invalid QoS");
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
    tx_count = (example_audio_tx_count_t){0};

    err = example_audio_tx_scheduler_start(&tx_scheduler, stream->qos->interval);
    if (err) {
        ESP_LOGE(TAG, "[SRC #0] Scheduler start failed, err %d", err);
        return;
    }

    broadcast_source_tx(&broadcast_stream);
}

static void stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_err_t err;

    (void)stream;

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

static void stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_err_t err;

    (void)stream;

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

static void stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    (void)stream;

    example_audio_tx_scheduler_on_sent(&tx_scheduler, user_data, &tx_count, TAG, "SRC #0");
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .started      = stream_started_cb,
    .stopped      = stream_stopped_cb,
    .sent         = stream_sent_cb,
    .disconnected = stream_disconnected_cb,
};

static uint8_t *ext_adv_data_get(uint8_t *data_len)
{
    NET_BUF_SIMPLE_DEFINE(pba_buf, 32);
    const esp_ble_audio_pbp_announcement_feature_t features =
        ESP_BLE_AUDIO_PBP_ANNOUNCEMENT_FEATURE_HIGH_QUALITY;
    uint32_t broadcast_id;
    uint8_t *data;
    uint8_t offset;
    esp_err_t err;

    broadcast_id = LOCAL_BROADCAST_ID;

    err = esp_ble_audio_pbp_get_announcement(pba_metadata, sizeof(pba_metadata), features, &pba_buf);
    if (err) {
        ESP_LOGE(TAG, "Failed to get encoded PBA, err %d", err);
        return NULL;
    }

    /* Appearance, Broadcast Name, Broadcast Audio Announcement (UUID + ID),
     * and Public Broadcast Announcement (including its UUID).
     */
    *data_len = 4 + 2 + strlen(LOCAL_BROADCAST_NAME) + 7 + 2 + pba_buf.len;

    data = calloc(1, *data_len);
    if (data == NULL) {
        return NULL;
    }

    data[0] = 0x03;
    data[1] = EXAMPLE_AD_TYPE_GAP_APPEARANCE;
    data[2] = (ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_BROADCASTING_DEVICE & 0xFF);
    data[3] = ((ESP_BLE_AUDIO_APPEARANCE_AUDIO_SOURCE_BROADCASTING_DEVICE >> 8) & 0xFF);

    data[4] = strlen(LOCAL_BROADCAST_NAME) + 1;
    data[5] = EXAMPLE_AD_TYPE_BROADCAST_NAME;
    memcpy(data + 6, LOCAL_BROADCAST_NAME, strlen(LOCAL_BROADCAST_NAME));

    offset = 6 + strlen(LOCAL_BROADCAST_NAME);
    data[offset] = 0x06;
    data[offset + 1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    data[offset + 2] = (ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL & 0xFF);
    data[offset + 3] = ((ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL >> 8) & 0xFF);
    data[offset + 4] = (broadcast_id & 0xFF);
    data[offset + 5] = ((broadcast_id >> 8) & 0xFF);
    data[offset + 6] = ((broadcast_id >> 16) & 0xFF);

    offset += 7;
    data[offset] = 1 + pba_buf.len;
    data[offset + 1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    memcpy(data + offset + 2, pba_buf.data, pba_buf.len);

    return data;
}

static uint8_t *per_adv_data_get(uint8_t *data_len)
{
    NET_BUF_SIMPLE_DEFINE(base_buf, 128);
    uint8_t *data;
    esp_err_t err;

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

    /* base_buf.len includes the Service UUID. */
    data[0] = 1 + base_buf.len;
    data[1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    memcpy(data + 2, base_buf.data, base_buf.len);

    return data;
}

static int broadcast_start(void)
{
    esp_ble_audio_cap_initiator_broadcast_stream_param_t stream_param = {
        .stream = &broadcast_stream,
    };
    esp_ble_audio_cap_initiator_broadcast_subgroup_param_t subgroup_param = {
        .stream_count = 1,
        .stream_params = &stream_param,
        .codec_cfg = &broadcast_preset_48_2_1.codec_cfg,
    };
    const esp_ble_audio_cap_initiator_broadcast_create_param_t create_param = {
        .subgroup_count = 1,
        .subgroup_params = &subgroup_param,
        .qos = &broadcast_preset_48_2_1.qos,
        .packing = ESP_BLE_ISO_PACKING_SEQUENTIAL,
        .encryption = false,
    };
    esp_ble_audio_bap_broadcast_adv_info_t adv_info = {
        .adv_handle = ADV_HANDLE,
    };
    bool adv_started = false;
    bool adv_added = false;
    uint8_t *ext_data = NULL;
    uint8_t *per_data = NULL;
    uint8_t ext_len = 0;
    uint8_t per_len = 0;
    esp_err_t err;

    ESP_LOGI(TAG, "Starting high quality announcement, Broadcast ID 0x%06lx",
             (unsigned long)LOCAL_BROADCAST_ID);

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

    err = esp_ble_audio_bap_broadcast_adv_add(&adv_info);
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
    if (err) {
        esp_err_t e;

        if (adv_added) {
            e = esp_ble_audio_bap_broadcast_adv_delete(&adv_info);
            if (e) {
                ESP_LOGE(TAG, "Failed to delete adv for broadcast source, err %d", e);
            }
        }

        if (adv_started) {
            ext_adv_stop();
        }

        e = esp_ble_audio_cap_initiator_broadcast_audio_delete(broadcast_source);
        if (e) {
            ESP_LOGE(TAG, "Failed to delete broadcast source, err %d", e);
        } else {
            broadcast_source = NULL;
        }
    }
    return err;
}

static void tx_scheduler_cb(void *arg)
{
    esp_ble_audio_cap_stream_t *stream = arg;

    broadcast_source_tx(stream);
}

static int pbp_source_init(void)
{
    esp_err_t err;

    err = esp_ble_audio_cap_stream_ops_register(&broadcast_stream, &stream_ops);
    if (err) {
        ESP_LOGE(TAG, "Failed to register stream callbacks, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "PBP broadcast source initialized");

    return 0;
}

void app_main(void)
{
    esp_err_t err;

    /* Initialize NVS — it is used to store PHY calibration data */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize BLE, err %d", err);
        return;
    }

    err = app_host_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to init host, err %d", err);
        return;
    }

    err = esp_ble_audio_common_init(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = pbp_source_init();
    if (err) {
        return;
    }

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    err = example_audio_tx_scheduler_init(&tx_scheduler,
                                          tx_scheduler_cb,
                                          &broadcast_stream);
    if (err) {
        ESP_LOGE(TAG, "[SRC #0] Scheduler init failed, err %d", err);
        return;
    }

    err = set_device_name();
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    err = broadcast_start();
    if (err) {
        ESP_LOGE(TAG, "Failed to start broadcast, err %d", err);
        return;
    }
}
