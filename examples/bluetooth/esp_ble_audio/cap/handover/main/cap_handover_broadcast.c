/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "cap_handover.h"

/* Shared subgroup configuration; the per-BIS channel allocation goes in a
 * separate LTV, so the location here stays mono.
 */
HANDOVER_LC3_PRESET_DEFINE(broadcast_preset, ESP_BLE_AUDIO_LOCATION_MONO_AUDIO);

/* One BIS per channel; the handover module turns these into per-BIS LTVs. */
static const esp_ble_audio_location_t bis_locations[] = {
    ESP_BLE_AUDIO_LOCATION_FRONT_LEFT,
    ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT,
};

_Static_assert(ARRAY_SIZE(bis_locations) >= SINK_STREAM_COUNT,
               "Need one channel allocation per broadcast stream");

uint8_t *broadcast_ext_adv_data_get(uint8_t *data_len)
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
        ESP_LOGE(TAG, "Failed to alloc ext adv data (%u octets)", *data_len);
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

uint8_t *broadcast_base_data_get(esp_ble_audio_cap_broadcast_source_t *source,
                                 uint8_t *data_len)
{
    NET_BUF_SIMPLE_DEFINE(base_buf, 128);
    uint8_t *data;
    esp_err_t err;

    /* Broadcast Audio Announcement Service UUID (2 octets) and
     * Broadcast Audio Source Endpoint (BASE)
     */

    err = esp_ble_audio_cap_initiator_broadcast_get_base(source, &base_buf);
    if (err) {
        ESP_LOGE(TAG, "Failed to get encoded BASE, err %d", err);
        return NULL;
    }

    *data_len = 2 + base_buf.len;

    data = calloc(1, *data_len);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to alloc per adv data (%u octets)", *data_len);
        return NULL;
    }

    /* base_buf.len has included the UUID length (2 octets) */
    data[0] = 1 + base_buf.len;
    data[1] = EXAMPLE_AD_TYPE_SERVICE_DATA16;
    memcpy(data + 2, base_buf.data, base_buf.len);

    return data;
}

esp_ble_audio_bap_lc3_preset_t *broadcast_preset_get(void)
{
    return &broadcast_preset;
}

esp_ble_audio_location_t broadcast_bis_location_get(size_t idx)
{
    return (idx < ARRAY_SIZE(bis_locations)) ? bis_locations[idx]
           : ESP_BLE_AUDIO_LOCATION_MONO_AUDIO;
}
