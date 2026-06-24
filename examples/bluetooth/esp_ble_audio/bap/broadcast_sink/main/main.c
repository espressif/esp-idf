/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_pacs_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#include "scan.h"

#define TARGET_DEVICE_NAME      "BAP Broadcast Source"
#define TARGET_DEVICE_NAME_LEN  (sizeof(TARGET_DEVICE_NAME) - 1)

#define TARGET_BROADCAST_CODE   "1234"

#define PA_SYNC_HANDLE_INIT     UINT16_MAX

#define CONN_HANDLE_INIT        UINT16_MAX

#define SINK_CONTEXT            (ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                                 ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA)

static struct broadcast_sink_stream {
    esp_ble_audio_bap_stream_t stream;
    example_audio_rx_metrics_t rx_metrics;
} streams[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];

static esp_ble_audio_bap_stream_t *streams_p[ARRAY_SIZE(streams)];

static const esp_ble_audio_bap_scan_delegator_recv_state_t *req_recv_state;
static esp_ble_audio_bap_broadcast_sink_t *broadcast_sink;
static uint16_t sync_handle = PA_SYNC_HANDLE_INIT;
static bool pa_syncing;

static uint16_t conn_handle = CONN_HANDLE_INIT;
static volatile bool stream_started;
static volatile bool base_received;
static uint32_t bis_index_bitfield;

static const uint32_t bis_index_mask = BIT_MASK(ARRAY_SIZE(streams) + 1);
static uint8_t sink_broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE];
static uint32_t broadcaster_broadcast_id;
static uint32_t requested_bis_sync;

static volatile uint8_t stream_count;
static volatile uint8_t stream_count_started;
static volatile uint8_t stream_count_stopped;

static uint8_t codec_data[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_DATA(
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_16KHZ | \
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_24KHZ,             /* Sampling frequency 16kHz/24kHz */
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_10,            /* Frame duration 10ms */
        ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1),  /* Supported channels 1 */
        40,                                             /* Minimum 40 octets per frame */
        60,                                             /* Maximum 60 octets per frame */
        1);                                             /* Maximum 1 codec frame per SDU */

static uint8_t codec_meta[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_META(SINK_CONTEXT);

static const esp_ble_audio_codec_cap_t codec_cap =
    ESP_BLE_AUDIO_CODEC_CAP_LC3(codec_data, codec_meta);

static esp_ble_audio_pacs_cap_t cap = {
    .codec_cap = &codec_cap,
};

static void recv_state_updated_cb(esp_ble_conn_t *conn,
                                  const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state)
{
    ESP_LOGI(TAG, "Receive state updated, pa_sync 0x%02x encrypt 0x%02x",
             recv_state->pa_sync_state, recv_state->encrypt_state);

    for (uint8_t i = 0; i < recv_state->num_subgroups; i++) {
        ESP_LOGI(TAG, "subgroup %d bis_sync 0x%08lx", i, recv_state->subgroups[i].bis_sync);
    }

    if (recv_state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_SYNCED) {
        req_recv_state = recv_state;
    }
}

static int pa_sync_req_cb(esp_ble_conn_t *conn,
                          const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state,
                          bool past_available, uint16_t pa_interval)
{
    ESP_LOGI(TAG, "Received request to sync to PA (PAST %savailable): %u",
             past_available ? "" : "not ",
             recv_state->pa_sync_state);

    req_recv_state = recv_state;

    if (recv_state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_SYNCED ||
            recv_state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ ||
            sync_handle != PA_SYNC_HANDLE_INIT) {
        /* Already syncing */
        ESP_LOGW(TAG, "Rejecting PA sync request");
        return -EALREADY;
    }

    if (past_available) {
        ESP_LOGW(TAG, "Currently not support PAST");
        return -ENOTSUP;
    }

    return 0;
}

static int pa_sync_term_req_cb(esp_ble_conn_t *conn,
                               const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state)
{
    int err;

    ESP_LOGI(TAG, "Received request to terminate PA sync");

    for (uint8_t i = 0; i < recv_state->num_subgroups; i++) {
        ESP_LOGI(TAG, "subgroup %d bis_sync 0x%08lx", i, recv_state->subgroups[i].bis_sync);
    }

    req_recv_state = recv_state;

    /* Nothing to terminate if PAST/PA-sync never landed (e.g., PAST setup failed
     * earlier). Issuing the HCI terminate with the sentinel handle gets 0x12
     * (Invalid HCI Command Parameters) back from the controller. Mirrors
     * cap_acceptor_broadcast.c. */
    if (sync_handle == PA_SYNC_HANDLE_INIT) {
        ESP_LOGI(TAG, "PA sync never established, skip terminate");
        return 0;
    }

    err = pa_sync_terminate(sync_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to terminate PA sync, err %d", err);
        return -EIO;
    }

    ESP_LOGI(TAG, "PA sync terminated");

    /* Let the synthesized PA_SYNC_LOST event drive cleanup via pa_sync_lost
     * (it gates on the original sync_handle). Resetting sync_handle here would
     * make that gate miss and leak broadcast_sink. Mirrors
     * cap_acceptor_broadcast.c. */
    return 0;
}

static void broadcast_code_cb(esp_ble_conn_t *conn,
                              const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state,
                              const uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE])
{
    ESP_LOGI(TAG, "Broadcast code received");

    req_recv_state = recv_state;

    memcpy(sink_broadcast_code, broadcast_code, ESP_BLE_ISO_BROADCAST_CODE_SIZE);
}

static int bis_sync_req_cb(esp_ble_conn_t *conn,
                           const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state,
                           const uint32_t bis_sync_req[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS])
{
    /* Bit field indicating from which subgroup(s) BIS sync is requested */
    uint32_t requested_subgroup_sync = 0;
    esp_err_t err;

    requested_bis_sync = 0;

    assert(bis_sync_req);

    for (uint8_t subgroup = 0; subgroup < recv_state->num_subgroups &&
            subgroup < CONFIG_BT_BAP_BASS_MAX_SUBGROUPS; subgroup++) {
        if (bis_sync_req[subgroup]) {
            if (requested_bis_sync == 0) {
                requested_bis_sync = bis_sync_req[subgroup];
            } else {
                if (requested_bis_sync != ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF &&
                        bis_sync_req[subgroup] != ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF) {
                    /* Spec a little bit unclear. Here we choose to say that BIS sync
                     * request from more than 1 subgroup is not possible unless sync
                     * value is 0 or ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF.
                     */
                    ESP_LOGE(TAG, "Unsupported BIS sync request from more than 1 subgroup");
                    return -EINVAL;
                }
            }

            requested_subgroup_sync |= BIT(subgroup);
        }
    }

    ESP_LOGI(TAG, "BIS sync req: broadcast_id 0x%06x BIS mask 0x%08x subgroup mask 0x%08x (%s)",
             recv_state->broadcast_id, requested_bis_sync,
             requested_subgroup_sync,
             stream_started ? "streaming" : "not streaming");

    if (stream_started && requested_bis_sync == 0) {
        /* The stream stopped callback will be called as part of this, and
         * we do not need to wait for any events from the controller. Thus,
         * when this returns, the `stream_started` is back to false.
         */
        err = esp_ble_audio_bap_broadcast_sink_stop(broadcast_sink);
        if (err) {
            ESP_LOGE(TAG, "Failed to stop broadcast sink, err %d", err);
            return -EIO;
        }

        err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink);
        if (err) {
            ESP_LOGE(TAG, "Failed to delete broadcast sink, err %d", err);
            return -EIO;
        }

        broadcast_sink = NULL;
    }

    return 0;
}

static esp_ble_audio_bap_scan_delegator_cb_t scan_delegator_cbs = {
    .recv_state_updated = recv_state_updated_cb,
    .pa_sync_req        = pa_sync_req_cb,
    .pa_sync_term_req   = pa_sync_term_req_cb,
    .broadcast_code     = broadcast_code_cb,
    .bis_sync_req       = bis_sync_req_cb,
};

static void base_recv_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                         const esp_ble_audio_bap_base_t *base,
                         size_t base_size)
{
    uint32_t base_bis_index_bitfield = 0;
    uint8_t base_subgroup_count;
    esp_err_t err;

    if (base_received) {
        return;
    }

    err = esp_ble_audio_bap_base_get_subgroup_count(base, &base_subgroup_count);
    if (err) {
        ESP_LOGE(TAG, "Failed to get subgroup count");
        return;
    }

    ESP_LOGI(TAG, "BASE received (%d subgroup(s))", base_subgroup_count);

    err = esp_ble_audio_bap_base_get_bis_indexes(base, &base_bis_index_bitfield);
    if (err) {
        ESP_LOGE(TAG, "Failed to get BIS indexes, err %d", err);
        return;
    }

    bis_index_bitfield = (base_bis_index_bitfield & bis_index_mask);

    ESP_LOGI(TAG, "bis_index_bitfield = 0x%08lx", bis_index_bitfield);

    if (conn_handle == CONN_HANDLE_INIT) {
        /* No broadcast assistant requesting anything */
        requested_bis_sync = ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF;
    }

    base_received = true;
}

static void syncable_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                        const esp_ble_iso_biginfo_t *biginfo)
{
    uint32_t sync_bitfield;
    esp_err_t err;

    ESP_LOGI(TAG, "Broadcast sink syncable, BIG %s",
             biginfo->encryption ? "encrypted" : "not encrypted");

    sync_bitfield = (bis_index_bitfield & requested_bis_sync);
    if (sync_bitfield == 0) {
        ESP_LOGW(TAG, "No matching BIS indexes, skipping sync");
        return;
    }

    stream_count = 0;
    stream_count_started = 0;
    stream_count_stopped = 0;

    for (size_t i = 0; i < ESP_BLE_ISO_MAX_GROUP_ISO_COUNT; i++) {
        if (sync_bitfield & BIT(i)) {
            stream_count++;
        }
    }

    ESP_LOGI(TAG, "Syncing to broadcast: BIS mask 0x%08x (%u stream(s))",
             sync_bitfield, stream_count);

    if (biginfo->encryption) {
        memset(sink_broadcast_code, 0, ESP_BLE_ISO_BROADCAST_CODE_SIZE);
        memcpy(sink_broadcast_code, TARGET_BROADCAST_CODE,
               MIN(ESP_BLE_ISO_BROADCAST_CODE_SIZE, strlen(TARGET_BROADCAST_CODE)));
    }

    err = esp_ble_audio_bap_broadcast_sink_sync(broadcast_sink, sync_bitfield,
                                                streams_p, sink_broadcast_code);
    if (err) {
        ESP_LOGE(TAG, "Failed to sync to broadcast source, err %d", err);
        return;
    }
}

static esp_ble_audio_bap_broadcast_sink_cb_t broadcast_sink_cbs = {
    .base_recv = base_recv_cb,
    .syncable  = syncable_cb,
};

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        if (&streams[i].stream == stream) {
            return (int)i;
        }
    }
    return -1;
}

static void stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    struct broadcast_sink_stream *sink_stream = CONTAINER_OF(stream,
                                                             struct broadcast_sink_stream,
                                                             stream);

    ESP_LOGI(TAG, "[SNK #%d] Stream started (%u/%u)",
             stream_index(stream), stream_count_started, stream_count);

    example_audio_rx_metrics_reset(&sink_stream->rx_metrics);

    if (++stream_count_started == stream_count) {
        stream_started = true;
    }
}

static void stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[SNK #%d] Stream stopped, reason 0x%02x (%u/%u)",
             stream_index(stream), reason, stream_count_stopped, stream_count);

    if (++stream_count_stopped == stream_count) {
        stream_started = false;

        err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink);
        if (err) {
            ESP_LOGE(TAG, "Failed to delete broadcast sink, err %d", err);
            return;
        }

        broadcast_sink = NULL;
    }
}

static void stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                           const esp_ble_iso_recv_info_t *info,
                           const uint8_t *data, uint16_t len)
{
    struct broadcast_sink_stream *sink_stream = CONTAINER_OF(stream,
                                                             struct broadcast_sink_stream,
                                                             stream);
    char name[24];

    snprintf(name, sizeof(name), "SNK #%d", stream_index(stream));
    sink_stream->rx_metrics.last_sdu_len = len;
    example_audio_rx_metrics_on_recv(info, &sink_stream->rx_metrics, TAG, name);
}

static esp_ble_audio_bap_stream_ops_t stream_ops = {
    .started = stream_started_cb,
    .stopped = stream_stopped_cb,
    .recv    = stream_recv_cb,
};

struct scan_recv_data {
    bool target_matched;
    bool broadcast_id_found;
    uint32_t broadcast_id;
};

static bool data_cb(uint8_t type, const uint8_t *data,
                    uint8_t data_len, void *user_data)
{
    struct scan_recv_data *sr = user_data;
    uint16_t uuid;

    switch (type) {
    case EXAMPLE_AD_TYPE_NAME_SHORTENED:
    case EXAMPLE_AD_TYPE_NAME_COMPLETE:
    case EXAMPLE_AD_TYPE_BROADCAST_NAME:
        sr->target_matched = (data_len == TARGET_DEVICE_NAME_LEN) &&
                             !memcmp(data, TARGET_DEVICE_NAME, TARGET_DEVICE_NAME_LEN);
        if (!sr->target_matched) {
            return false;
        }
        return true;
    case EXAMPLE_AD_TYPE_SERVICE_DATA16:
        if (data_len < ESP_BLE_AUDIO_UUID_SIZE_16 + ESP_BLE_AUDIO_BROADCAST_ID_SIZE) {
            return true;
        }
        uuid = sys_get_le16(data);
        if (uuid != ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL) {
            return true;
        }
        sr->broadcast_id = sys_get_le24(data + ESP_BLE_AUDIO_UUID_SIZE_16);
        sr->broadcast_id_found = true;
        return true;
    default:
        return true;
    }
}

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    struct scan_recv_data sr = {0};
    int err;

    /* Periodic advertising interval. 0 if no periodic advertising. */
    if (event->ext_scan_recv.per_adv_itvl == 0) {
        return;
    }

    esp_ble_audio_data_parse(event->ext_scan_recv.data,
                             event->ext_scan_recv.data_len,
                             data_cb, &sr);

    if (!sr.target_matched || !sr.broadcast_id_found) {
        return;
    }

    if (pa_syncing == false && req_recv_state == NULL) {
        broadcaster_broadcast_id = sr.broadcast_id;

        err = pa_sync_create(event->ext_scan_recv.addr.type,
                             event->ext_scan_recv.addr.val,
                             event->ext_scan_recv.sid);
        if (err) {
            ESP_LOGE(TAG, "Failed to create PA sync, err %d", err);
            return;
        }

        pa_syncing = true;
    }
}

static void pa_sync(esp_ble_audio_gap_app_event_t *event)
{
    esp_err_t err;
    int rc;

    pa_syncing = false;

    if (event->pa_sync.status) {
        ESP_LOGE(TAG, "PA sync failed, status %d", event->pa_sync.status);
        return;
    }

    sync_handle = event->pa_sync.sync_handle;

    ESP_LOGI(TAG, "Broadcast source PA synced, creating Broadcast Sink");

    /* PA sync is established; the BASE / BIGInfo reports will arrive
     * via the PA sync channel, so the extended scanner is no longer
     * needed. Stop it now — pa_sync_lost() will restart it on loss.
     */
    rc = ext_scan_stop();
    if (rc) {
        ESP_LOGW(TAG, "Failed to stop scanning, err %d", rc);
    }

    err = esp_ble_audio_bap_broadcast_sink_create(event->pa_sync.sync_handle,
                                                  broadcaster_broadcast_id,
                                                  &broadcast_sink);
    if (err) {
        ESP_LOGE(TAG, "Failed to create broadcast sink, err %d", err);
        return;
    }
}

static void pa_sync_lost(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "PA sync lost: sync_handle %u reason 0x%02x",
             event->pa_sync_lost.sync_handle, event->pa_sync_lost.reason);

    if (sync_handle == event->pa_sync_lost.sync_handle) {
        sync_handle = PA_SYNC_HANDLE_INIT;
        pa_syncing = false;
        base_received = false;
        stream_started = false;
        stream_count = 0;
        stream_count_started = 0;
        stream_count_stopped = 0;
        /* Clear the stale scan-delegator state pointer captured by
         * recv_state_updated_cb() while we were synced. Without this,
         * ext_scan_recv() would keep rejecting new source ads because
         * its gate is `pa_syncing == false && req_recv_state == NULL`.
         */
        req_recv_state = NULL;

        if (broadcast_sink != NULL) {
            esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink);
            broadcast_sink = NULL;
        }

        ext_scan_start();
    }
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

void app_main(void)
{
    const esp_ble_audio_pacs_register_param_t pacs_param = {
        .snk_pac = true,
        .snk_loc = true,
    };
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

    err = esp_ble_audio_pacs_register(&pacs_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs, err %d", err);
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(streams); i++) {
        streams[i].stream.ops = &stream_ops;
        streams_p[i] = &streams[i].stream;
    }

    err = esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SINK, &cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return;
    }

    err = esp_ble_audio_bap_scan_delegator_register(&scan_delegator_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register scan delegator, err %d", err);
        return;
    }

    err = esp_ble_audio_bap_broadcast_sink_register_cb(&broadcast_sink_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register broadcast sink callbacks, err %d", err);
        return;
    }

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    ext_scan_start();
}
