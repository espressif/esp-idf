/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"

#include "esp_ble_audio_common_api.h"
#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_pbp_api.h"
#include "esp_ble_audio_lc3_defs.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#include "scan.h"

#define PA_SYNC_HANDLE_INIT     UINT16_MAX

_Static_assert(CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT == 1,
               "This example receives one BIS");

static esp_ble_audio_bap_broadcast_sink_t *broadcast_sink;
static esp_ble_audio_bap_stream_t broadcast_stream;
static esp_ble_audio_bap_stream_t *streams_p[] = { &broadcast_stream };
static example_audio_rx_metrics_t rx_metrics;

static uint16_t sync_handle = PA_SYNC_HANDLE_INIT;
static uint32_t broadcaster_broadcast_id;
static uint32_t bis_index_bitfield;
static bool pa_syncing;

static uint8_t codec_data[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_DATA(
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_48KHZ,             /* Sampling frequency 48kHz */
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_10,            /* Frame duration 10ms */
        ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1),  /* Supported channels 1 (mono) */
        100,                                            /* Minimum 100 octets per frame */
        100,                                            /* Maximum 100 octets per frame */
        1);                                             /* Maximum 1 codec frame per SDU */

static uint8_t codec_meta[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_META(ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);

static const esp_ble_audio_codec_cap_t codec_cap =
    ESP_BLE_AUDIO_CODEC_CAP_LC3(codec_data, codec_meta);

static esp_ble_audio_pacs_cap_t sink_cap = {
    .codec_cap = &codec_cap,
};

static bool codec_data_cb(uint8_t type, const uint8_t *data,
                          uint8_t data_len, void *user_data)
{
    bool *is_48khz = user_data;

    if (type == ESP_BLE_AUDIO_CODEC_CFG_FREQ && data_len == 1) {
        *is_48khz = (data[0] == ESP_BLE_AUDIO_CODEC_CFG_FREQ_48KHZ);
        return false;
    }

    return true;
}

/* A public broadcast may carry Standard Quality (16/24 kHz) subgroups next to the
 * High Quality (48 kHz) one: take the BIS of the first 48 kHz subgroup.
 */
static bool base_subgroup_cb(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                             void *user_data)
{
    uint32_t *bis_indexes = user_data;
    bool is_48khz = false;
    size_t data_len;
    uint8_t *data;

    if (*bis_indexes == 0 &&
            esp_ble_audio_bap_base_get_subgroup_codec_data(subgroup, &data, &data_len) == ESP_OK) {
        esp_ble_audio_data_parse(data, data_len, codec_data_cb, &is_48khz);
        if (is_48khz) {
            esp_ble_audio_bap_base_subgroup_get_bis_indexes(subgroup, bis_indexes);
        }
    }

    /* Always continue: stopping early makes the iterator return an error. */
    return true;
}

static void base_recv_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                         const esp_ble_audio_bap_base_t *base,
                         size_t base_size)
{
    uint32_t base_bis_index_bitfield = 0;
    esp_err_t err;

    (void)base_size;

    if (sink != broadcast_sink || bis_index_bitfield != 0) {
        return;
    }

    err = esp_ble_audio_bap_base_foreach_subgroup(base, base_subgroup_cb,
                                                  &base_bis_index_bitfield);
    if (err) {
        ESP_LOGE(TAG, "Failed to parse BASE subgroups, err %d", err);
        return;
    }

    if (base_bis_index_bitfield == 0) {
        ESP_LOGW(TAG, "BASE has no 48 kHz subgroup");
        return;
    }

    for (uint8_t index = 1; index <= ESP_BLE_ISO_BIS_INDEX_MAX; index++) {
        if (base_bis_index_bitfield & ESP_BLE_ISO_BIS_INDEX_BIT(index)) {
            bis_index_bitfield = ESP_BLE_ISO_BIS_INDEX_BIT(index);

            ESP_LOGI(TAG, "BASE received: selected BIS bitmap 0x%08lx",
                     (unsigned long)bis_index_bitfield);
            return;
        }
    }
}

static void syncable_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                        const esp_ble_iso_biginfo_t *biginfo)
{
    esp_err_t err;

    if (sink != broadcast_sink) {
        return;
    }

    if (biginfo->encryption) {
        ESP_LOGW(TAG, "Encrypted BIG is not supported by this example");
        return;
    }

    /* BIGInfo can arrive before BASE; wait for the next report in that case. */
    if (bis_index_bitfield == 0) {
        return;
    }

    err = esp_ble_audio_bap_broadcast_sink_sync(sink, bis_index_bitfield,
                                                streams_p, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to sync to broadcast source, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "BIG sync requested: BIS bitmap 0x%08lx",
             (unsigned long)bis_index_bitfield);
}

static void broadcast_sink_stopped_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                                      uint8_t reason)
{
    esp_err_t err;

    if (sink != broadcast_sink) {
        return;
    }

    ESP_LOGI(TAG, "Broadcast sink stopped, reason 0x%02x", reason);

    /* BASS bis_sync has been cleared before this sink-level callback. */
    err = esp_ble_audio_bap_broadcast_sink_delete(sink);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete broadcast sink, err %d", err);
        return;
    }

    broadcast_sink = NULL;
    bis_index_bitfield = 0;

    if (sync_handle != PA_SYNC_HANDLE_INIT) {
        err = pa_sync_terminate(sync_handle);
        if (err) {
            ESP_LOGE(TAG, "Failed to terminate PA sync, err %d", err);
        }
    } else {
        ext_scan_start();
    }
}

static esp_ble_audio_bap_broadcast_sink_cb_t broadcast_sink_cbs = {
    .base_recv = base_recv_cb,
    .syncable  = syncable_cb,
    .stopped   = broadcast_sink_stopped_cb,
};

static void stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[SNK #0] Stream started");

    example_audio_rx_metrics_reset(&rx_metrics);
    example_print_codec_cfg(TAG, stream->codec_cfg);
}

static void stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    (void)stream;

    ESP_LOGI(TAG, "[SNK #0] Stream stopped, reason 0x%02x", reason);
}

static void stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                           const esp_ble_iso_recv_info_t *info,
                           const uint8_t *data, uint16_t len)
{
    (void)stream;

    rx_metrics.last_sdu = data;
    rx_metrics.last_sdu_len = len;
    example_audio_rx_metrics_on_recv(info, &rx_metrics, TAG, "SNK #0");
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .started = stream_started_cb,
    .stopped = stream_stopped_cb,
    .recv    = stream_recv_cb,
};

struct scan_recv_data {
    uint32_t broadcast_id;
    esp_ble_audio_pbp_announcement_feature_t features;
    char name[ESP_BLE_AUDIO_BROADCAST_NAME_LEN_MAX + 1];
    const uint8_t *metadata;
    uint8_t metadata_len;
    bool has_baa;
    bool has_pba;
};

static bool data_cb(uint8_t type, const uint8_t *data,
                    uint8_t data_len, void *user_data)
{
    struct scan_recv_data *sr = user_data;
    esp_ble_audio_pbp_announcement_feature_t features;
    uint8_t *metadata;
    uint8_t metadata_len;
    esp_err_t err;

    if (type == EXAMPLE_AD_TYPE_BROADCAST_NAME) {
        if (data_len >= ESP_BLE_AUDIO_BROADCAST_NAME_LEN_MIN &&
                data_len <= ESP_BLE_AUDIO_BROADCAST_NAME_LEN_MAX) {
            memcpy(sr->name, data, data_len);
            sr->name[data_len] = '\0';
        }

        return true;
    }

    if (type != EXAMPLE_AD_TYPE_SERVICE_DATA16 ||
            data_len < ESP_BLE_AUDIO_UUID_SIZE_16) {
        return true;
    }

    if (sys_get_le16(data) == ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL) {
        if (data_len >= ESP_BLE_AUDIO_UUID_SIZE_16 + ESP_BLE_AUDIO_BROADCAST_ID_SIZE) {
            sr->broadcast_id = sys_get_le24(data + ESP_BLE_AUDIO_UUID_SIZE_16);
            sr->has_baa = true;
        }

        return true;
    }

    if (sys_get_le16(data) != ESP_BLE_AUDIO_UUID_PBA_VAL) {
        return true;
    }

    err = esp_ble_audio_pbp_parse_announcement(type, data, data_len,
                                               &features, &metadata, &metadata_len);
    if (err) {
        return true;
    }

    sr->features = features;
    sr->metadata = metadata;
    sr->metadata_len = metadata_len;
    sr->has_pba = true;

    return true;
}

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    struct scan_recv_data sr = {0};
    int err;

    if (pa_syncing || broadcast_sink != NULL || sync_handle != PA_SYNC_HANDLE_INIT ||
            (event->ext_scan_recv.event_type & EXAMPLE_ADV_PROP_CONNECTABLE) ||
            event->ext_scan_recv.per_adv_itvl == 0) {
        return;
    }

    esp_ble_audio_data_parse(event->ext_scan_recv.data,
                             event->ext_scan_recv.data_len,
                             data_cb, &sr);
    if (!sr.has_baa || !sr.has_pba ||
            !(sr.features & ESP_BLE_AUDIO_PBP_ANNOUNCEMENT_FEATURE_HIGH_QUALITY) ||
            (sr.features & ESP_BLE_AUDIO_PBP_ANNOUNCEMENT_FEATURE_ENCRYPTION)) {
        return;
    }

    ESP_LOGI(TAG, "Public source '%s': id 0x%06lx, features 0x%02x, peer "
             "%02x:%02x:%02x:%02x:%02x:%02x",
             sr.name[0] ? sr.name : "(unnamed)", (unsigned long)sr.broadcast_id,
             sr.features, EXAMPLE_BT_ADDR_PRINT_ARGS(event->ext_scan_recv.addr.val));
    example_print_meta(TAG, sr.metadata, sr.metadata_len);

    broadcaster_broadcast_id = sr.broadcast_id;
    pa_syncing = true;
    err = pa_sync_create(event->ext_scan_recv.addr.type,
                         event->ext_scan_recv.addr.val,
                         event->ext_scan_recv.sid);
    if (err) {
        ESP_LOGE(TAG, "Failed to create PA sync, err %d", err);
        pa_syncing = false;
        return;
    }
}

static void pa_sync(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    if (!pa_syncing) {
        return;
    }

    pa_syncing = false;
    if (event->pa_sync.status) {
        ESP_LOGW(TAG, "PA sync failed, status 0x%02x", event->pa_sync.status);
        return;
    }

    sync_handle = event->pa_sync.sync_handle;

    ESP_LOGI(TAG, "PA synced: handle %u, waiting for BASE and BIGInfo", sync_handle);

    err = ext_scan_stop();
    if (err) {
        ESP_LOGW(TAG, "Failed to stop scanning, err %d", err);
    }

    err = esp_ble_audio_bap_broadcast_sink_create(sync_handle,
                                                  broadcaster_broadcast_id,
                                                  &broadcast_sink);
    if (err) {
        ESP_LOGE(TAG, "Failed to create broadcast sink, err %d", err);
        /* Scanning resumes once the PA sync loss is reported. */
        err = pa_sync_terminate(sync_handle);
        if (err) {
            ESP_LOGE(TAG, "Failed to terminate PA sync, err %d", err);
        }
        return;
    }
}

static void pa_sync_lost(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    if (event->pa_sync_lost.sync_handle != sync_handle) {
        return;
    }

    ESP_LOGI(TAG, "PA sync lost: sync_handle %u reason 0x%02x",
             sync_handle, event->pa_sync_lost.reason);

    sync_handle = PA_SYNC_HANDLE_INIT;

    /* BIS sync does not depend on PA sync; the sink-level stopped callback resumes scanning */
    if (broadcast_sink != NULL && broadcast_stream.ep != NULL) {
        return;
    }

    if (broadcast_sink != NULL) {
        err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink);
        if (err) {
            ESP_LOGE(TAG, "Failed to delete broadcast sink, err %d", err);
            return;
        }

        broadcast_sink = NULL;
    }

    bis_index_bitfield = 0;
    ext_scan_start();
}

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC:
        pa_sync(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_LOST:
        pa_sync_lost(event);
        break;
    default:
        break;
    }
}

static int pbp_sink_init(void)
{
    const esp_ble_audio_pacs_register_param_t pacs_param = {
        .snk_pac = true,
        .snk_loc = true,
    };
    esp_err_t err;

    err = esp_ble_audio_pacs_register(&pacs_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SINK, &sink_cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_location(ESP_BLE_AUDIO_DIR_SINK,
                                          ESP_BLE_AUDIO_LOCATION_MONO_AUDIO);
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs location, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_supported_contexts(ESP_BLE_AUDIO_DIR_SINK,
                                                    ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);
    if (err) {
        ESP_LOGE(TAG, "Failed to set supported contexts, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_available_contexts(ESP_BLE_AUDIO_DIR_SINK,
                                                    ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);
    if (err) {
        ESP_LOGE(TAG, "Failed to set available contexts, err %d", err);
        return err;
    }

    broadcast_stream.ops = &stream_ops;

    err = esp_ble_audio_bap_broadcast_sink_register_cb(&broadcast_sink_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register broadcast sink callbacks, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "PBP broadcast sink initialized");

    return 0;
}

void app_main(void)
{
    esp_ble_audio_init_info_t info = {
        .gap_cb = iso_gap_app_cb,
    };
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

    err = esp_ble_audio_common_init(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = pbp_sink_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize PBP sink, err %d", err);
        return;
    }

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    ext_scan_start();
}
