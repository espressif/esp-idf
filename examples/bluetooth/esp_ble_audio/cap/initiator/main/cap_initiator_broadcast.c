/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "cap_initiator.h"

#define ADV_HANDLE              0x00
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_ADDRESS             BLE_OWN_ADDR_PUBLIC
#define ADV_PRIMARY_PHY         BLE_HCI_LE_PHY_1M
#define ADV_SECONDARY_PHY       BLE_HCI_LE_PHY_2M
#define ADV_INTERVAL            BLE_GAP_ADV_ITVL_MS(200)

#define PER_ADV_INTERVAL        BLE_GAP_ADV_ITVL_MS(100)

#define LOCAL_DEVICE_NAME       "CAP Broadcast Source"
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

static int ext_adv_start(void)
{
    struct ble_gap_periodic_adv_params per_params = {0};
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data = NULL;
    uint8_t *ext_data = NULL;
    uint8_t *per_data = NULL;
    uint8_t data_len = 0;
    int err;

    ext_params.connectable = 0;
    ext_params.scannable = 0;
    ext_params.legacy_pdu = 0;
    ext_params.own_addr_type = ADV_ADDRESS;
    ext_params.primary_phy = ADV_PRIMARY_PHY;
    ext_params.secondary_phy = ADV_SECONDARY_PHY;
    ext_params.tx_power = ADV_TX_POWER;
    ext_params.sid = ADV_SID;
    ext_params.itvl_min = ADV_INTERVAL;
    ext_params.itvl_max = ADV_INTERVAL;

    err = ble_gap_ext_adv_configure(ADV_HANDLE, &ext_params, NULL,
                                    example_audio_gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure ext adv params, err %d", err);
        goto end;
    }

    ext_data = ext_adv_data_get(&data_len);
    if (ext_data == NULL) {
        err = -ENOMEM;
        goto end;
    }

    data = os_msys_get_pkthdr(data_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get ext adv mbuf");
        err = -ENOMEM;
        goto end;
    }

    err = os_mbuf_append(data, ext_data, data_len);
    if (err) {
        ESP_LOGE(TAG, "Failed to append ext adv data, err %d", err);
        os_mbuf_free_chain(data);
        goto end;
    }

    err = ble_gap_ext_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set ext adv data, err %d", err);
        goto end;
    }

    per_params.include_tx_power = 0;
    per_params.itvl_min = PER_ADV_INTERVAL;
    per_params.itvl_max = PER_ADV_INTERVAL;

    err = ble_gap_periodic_adv_configure(ADV_HANDLE, &per_params);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure per adv params, err %d", err);
        goto end;
    }

    per_data = per_adv_data_get(&data_len);
    if (per_data == NULL) {
        err = -ENOMEM;
        goto end;
    }

    data = os_msys_get_pkthdr(data_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get per adv mbuf");
        err = -ENOMEM;
        goto end;
    }

    err = os_mbuf_append(data, per_data, data_len);
    if (err) {
        ESP_LOGE(TAG, "Failed to append per adv data, err %d", err);
        os_mbuf_free_chain(data);
        goto end;
    }

    err = ble_gap_periodic_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set per adv data, err %d", err);
        goto end;
    }

    err = ble_gap_periodic_adv_start(ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to start per advertising, err %d", err);
        goto end;
    }

    err = ble_gap_ext_adv_start(ADV_HANDLE, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "Failed to start ext advertising, err %d", err);
        goto end;
    }

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);

end:
    if (ext_data) {
        free(ext_data);
    }
    if (per_data) {
        free(per_data);
    }
    return err;
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
    int err;

    ESP_LOGI(TAG, "Creating broadcast source");

    err = esp_ble_audio_cap_initiator_broadcast_audio_create(&create_param, &broadcast_source);
    if (err) {
        ESP_LOGE(TAG, "Failed to create broadcast source, err %d", err);
        return err;
    }

    err = ext_adv_start();
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
        goto end;
    }

    return 0;

end:
    err = esp_ble_audio_cap_initiator_broadcast_audio_delete(broadcast_source);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete broadcast source, err %d", err);
    }
    return err;
}

int cap_initiator_broadcast_init(void)
{
    esp_ble_audio_cap_stream_ops_register(&broadcast_stream, &broadcast_stream_ops);

    ESP_LOGI(TAG, "CAP initiator broadcast initialized");

    return 0;
}
