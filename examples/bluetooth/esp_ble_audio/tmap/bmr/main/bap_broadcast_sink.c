/*
 * SPDX-FileCopyrightText: 2022-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tmap_bmr.h"

#define PA_SYNC_HANDLE_INIT     UINT16_MAX

static uint16_t sync_handle = PA_SYNC_HANDLE_INIT;
static bool pa_syncing;

static bool tmap_bms_found;

static esp_ble_audio_bap_broadcast_sink_t *broadcast_sink;
static uint32_t bcast_id;

static esp_ble_audio_bap_stream_t streams[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];
static esp_ble_audio_bap_stream_t *streams_p[ARRAY_SIZE(streams)];

static uint8_t codec_data[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_DATA(
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_48KHZ,             /* Sampling frequency 48kHz */
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_10,            /* Frame duration 10ms */
        ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1),  /* Supported channels 1 (mono) */
        /* SDU size range covers the 48_2_1 LC3 broadcast preset (100 octets/frame)
         * that the paired BMS sends. */
        100,                                            /* Minimum 100 octets per frame */
        100,                                            /* Maximum 100 octets per frame */
        1);                                             /* Maximum 1 codec frame per SDU */

static uint8_t codec_meta[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_META(ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);

static const esp_ble_audio_codec_cap_t codec =
    ESP_BLE_AUDIO_CODEC_CAP_LC3(codec_data, codec_meta);

/* Create a mask for the maximum BIS we can sync to using the number of
 * streams we have.
 * We add an additional 1 since the bis indexes start from 1 and not 0.
 */
static const uint32_t bis_index_mask = BIT_MASK(ARRAY_SIZE(streams) + 1);
static uint32_t bis_index_bitfield;

static example_audio_rx_metrics_t rx_metrics;

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        if (&streams[i] == stream) {
            return (int)i;
        }
    }
    return -1;
}

static void stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[SNK #%d] Stream started", stream_index(stream));

    example_audio_rx_metrics_reset(&rx_metrics);
}

static void stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[SNK #%d] Stream stopped, reason 0x%02x",
             stream_index(stream), reason);
}

static void stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                           const esp_ble_iso_recv_info_t *info,
                           const uint8_t *data, uint16_t len)
{
    char name[24];

    snprintf(name, sizeof(name), "SNK #%d", stream_index(stream));
    rx_metrics.last_sdu_len = len;
    example_audio_rx_metrics_on_recv(info, &rx_metrics, TAG, name);
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .started = stream_started_cb,
    .stopped = stream_stopped_cb,
    .recv    = stream_recv_cb
};

static esp_ble_audio_pacs_cap_t cap = {
    .codec_cap = &codec,
};

static void base_recv_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                         const esp_ble_audio_bap_base_t *base,
                         size_t base_size)
{
    uint32_t base_bis_index_bitfield = 0;
    int err;

    assert(base);

    err = esp_ble_audio_bap_base_get_bis_indexes(base, &base_bis_index_bitfield);
    if (err) {
        ESP_LOGE(TAG, "Failed to get bis indexes from BASE, err %d", err);
        return;
    }

    bis_index_bitfield = (base_bis_index_bitfield & bis_index_mask);
}

static void syncable_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                        const esp_ble_iso_biginfo_t *biginfo)
{
    int err;

    ESP_LOGI(TAG, "BASE received, creating broadcast sink");

    err = esp_ble_audio_bap_broadcast_sink_sync(broadcast_sink,
                                                bis_index_bitfield,
                                                streams_p, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to sync to broadcast source, err %d", err);
        return;
    }
}

static esp_ble_audio_bap_broadcast_sink_cb_t broadcast_sink_cbs = {
    .base_recv = base_recv_cb,
    .syncable  = syncable_cb,
};

static esp_ble_audio_bap_scan_delegator_cb_t scan_delegator_cbs;

static bool scan_check_and_sync_broadcast(uint8_t type, const uint8_t *data,
                                          uint8_t data_len, void *user_data)
{
    uint32_t *broadcast_id = user_data;
    uint16_t tmap_role;
    uint16_t uuid_val;

    if (type != EXAMPLE_AD_TYPE_SERVICE_DATA16) {
        return true;
    }

    if (data_len < sizeof(uuid_val)) {
        ESP_LOGW(TAG, "Invalid ad size %u (uuid)", data_len);
        return true;
    }

    uuid_val = sys_get_le16(data);

    if (uuid_val == ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL) {
        if (data_len < sizeof(uuid_val) + 3) {
            ESP_LOGW(TAG, "Invalid ad size %u (Broadcast ID)", data_len);
            return true;
        }

        *broadcast_id = sys_get_le24(data + sizeof(uuid_val));
        return true;
    }

    if (uuid_val != ESP_BLE_AUDIO_UUID_TMAS_VAL) {
        return true;
    }

    if (data_len < sizeof(uuid_val) + sizeof(tmap_role)) {
        ESP_LOGW(TAG, "Invalid ad size %u (tmap role)", data_len);
        return false;   /* Stop parsing */
    }

    tmap_role = sys_get_le16(data + sizeof(uuid_val));

    if (tmap_role & ESP_BLE_AUDIO_TMAP_ROLE_BMS) {
        tmap_bms_found = true;
        return true;
    }

    return false;   /* Stop parsing */
}

void bap_broadcast_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    uint32_t broadcast_id;
    int err;

    if ((event->ext_scan_recv.event_type & EXAMPLE_ADV_PROP_CONNECTABLE) ||
            event->ext_scan_recv.per_adv_itvl == 0) {
        return;
    }

    tmap_bms_found = false;

    broadcast_id = ESP_BLE_AUDIO_BAP_INVALID_BROADCAST_ID;

    esp_ble_audio_data_parse(event->ext_scan_recv.data,
                             event->ext_scan_recv.data_len,
                             scan_check_and_sync_broadcast,
                             (void *)&broadcast_id);

    if (broadcast_id != ESP_BLE_AUDIO_BAP_INVALID_BROADCAST_ID &&
            tmap_bms_found &&
            pa_syncing == false) {
        ESP_LOGI(TAG, "Found TMAP BMS, starting PA sync (broadcast ID 0x%06lx)",
                 broadcast_id);

        err = pa_sync_create(event->ext_scan_recv.addr.type,
                             event->ext_scan_recv.addr.val,
                             event->ext_scan_recv.sid);
        if (err) {
            ESP_LOGE(TAG, "Failed to create PA sync, err %d", err);
            return;
        }

        bcast_id = broadcast_id;
        pa_syncing = true;
    }
}

void bap_broadcast_pa_sync(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    pa_syncing = false; /* Mark PA sync as completed */

    if (event->pa_sync.status) {
        ESP_LOGE(TAG, "PA sync failed, status %d", event->pa_sync.status);
        return;
    }

    ESP_LOGI(TAG, "PA sync %u synced with broadcast ID 0x%06x",
             event->pa_sync.sync_handle, bcast_id);

    err = ext_scan_stop();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Broadcast source PA synced, waiting for BASE");

    err = esp_ble_audio_bap_broadcast_sink_create(event->pa_sync.sync_handle,
                                                  bcast_id, &broadcast_sink);
    if (err) {
        ESP_LOGE(TAG, "Failed to create broadcast sink, err %d", err);
        return;
    }

    sync_handle = event->pa_sync.sync_handle;
}

void bap_broadcast_pa_lost(esp_ble_audio_gap_app_event_t *event)
{
    if (sync_handle == event->pa_sync_lost.sync_handle) {
        ESP_LOGI(TAG, "PA sync %u lost with reason %u",
                 sync_handle, event->pa_sync_lost.reason);

        sync_handle = PA_SYNC_HANDLE_INIT;

        if (broadcast_sink != NULL) {
            esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink);
            broadcast_sink = NULL;
        }

        bap_broadcast_sink_scan();
    }
}

int bap_broadcast_sink_scan(void)
{
    return ext_scan_start();
}

int bap_broadcast_sink_init(void)
{
    const esp_ble_audio_pacs_register_param_t pacs_param = {
        .snk_pac = true,
        .snk_loc = true,
    };
    int err;

    err = esp_ble_audio_pacs_register(&pacs_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs, err %d", err);
        return err;
    }

    err = esp_ble_audio_bap_broadcast_sink_register_cb(&broadcast_sink_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register broadcast sink cb, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SINK, &cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return err;
    }

    err = esp_ble_audio_bap_scan_delegator_register(&scan_delegator_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register scan delegator, err %d", err);
        return err;
    }

    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        streams[i].ops = &stream_ops;
    }

    for (size_t i = 0; i < ARRAY_SIZE(streams_p); i++) {
        streams_p[i] = &streams[i];
    }

    ESP_LOGI(TAG, "BAP broadcast sink initialized");

    return 0;
}
