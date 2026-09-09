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

#include "sdkconfig.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "esp_bt_defs.h"
#else
#include "nimble/ble.h"
#endif

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
#if CONFIG_EXAMPLE_PAST
static uint8_t peer_addr[6];
#endif /* CONFIG_EXAMPLE_PAST */
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

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
/* Connectable advertising so a Broadcast Assistant can find us and drive sync
 * over BASS. The Assistant matches on the BASS UUID; PACS is advertised too so
 * it can check our capabilities before picking a source. */
static uint8_t ext_adv_data[] = {
    /* Flags */
    0x02, EXAMPLE_AD_TYPE_FLAGS, (EXAMPLE_AD_FLAGS_GENERAL | EXAMPLE_AD_FLAGS_NO_BREDR),
    /* Incomplete List of 16-bit Service UUIDs */
    0x05, EXAMPLE_AD_TYPE_UUID16_SOME,
    (ESP_BLE_AUDIO_UUID_BASS_VAL & 0xFF), ((ESP_BLE_AUDIO_UUID_BASS_VAL >> 8) & 0xFF),
    (ESP_BLE_AUDIO_UUID_PACS_VAL & 0xFF), ((ESP_BLE_AUDIO_UUID_PACS_VAL >> 8) & 0xFF),
    /* Service Data - Broadcast Audio Scan Service */
    0x03, EXAMPLE_AD_TYPE_SERVICE_DATA16,
    (ESP_BLE_AUDIO_UUID_BASS_VAL & 0xFF), ((ESP_BLE_AUDIO_UUID_BASS_VAL >> 8) & 0xFF),
    /* Complete Device Name */
    0x13, EXAMPLE_AD_TYPE_NAME_COMPLETE,
    'B', 'A', 'P', ' ', 'B', 'r', 'o', 'a', 'd', 'c', 'a', 's', 't', ' ', 'S', 'i', 'n', 'k',
};
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */

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

/* recv_state->addr carries the on-air (LSB-first) order the BASS PDU used;
 * pa_sync_create() takes the active host's own order — MSB-first under
 * Bluedroid, on-air under NimBLE. */
static void addr_le_to_host(uint8_t dst[6], const uint8_t src[6])
{
#if CONFIG_BT_BLUEDROID_ENABLED
    for (size_t i = 0; i < 6; i++) {
        dst[i] = src[5 - i];
    }
#else
    memcpy(dst, src, 6);
#endif
}

/* Likewise the address type: BASS 3.1.1.4 carries only 0x00 public (device or
 * identity) and 0x01 random (device or static identity); pa_sync_create() wants
 * the host's own enum. */
static uint8_t addr_type_le_to_host(uint8_t type)
{
#if CONFIG_BT_BLUEDROID_ENABLED
    return (type == BT_ADDR_LE_PUBLIC ||
            type == BT_ADDR_LE_PUBLIC_ID) ? BLE_ADDR_TYPE_PUBLIC : BLE_ADDR_TYPE_RANDOM;
#else
    return (type == BT_ADDR_LE_PUBLIC ||
            type == BT_ADDR_LE_PUBLIC_ID) ? BLE_ADDR_PUBLIC : BLE_ADDR_RANDOM;
#endif
}

static int pa_sync_req_cb(esp_ble_conn_t *conn,
                          const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state,
                          bool past_available, uint16_t pa_interval)
{
    uint8_t addr[6];
    int err;

    ESP_LOGI(TAG, "Assistant requests PA sync to 0x%06lx (PAST %savailable)",
             (unsigned long)recv_state->broadcast_id,
             past_available ? "" : "not ");

    /* BASS 3.1.1.4 lets the server answer either way for 0x01 and 0x02 alike:
     * request SyncInfo, or establish the sync itself. Rejecting is not an
     * option, so EXAMPLE_PAST only picks which of the two we take. */

    if (pa_syncing || sync_handle != PA_SYNC_HANDLE_INIT) {
        if (recv_state->broadcast_id == broadcaster_broadcast_id) {
            /* Already on the requested train — nothing left to do. */
            req_recv_state = recv_state;
            return 0;
        }

        ESP_LOGW(TAG, "Busy with 0x%06lx, rejecting",
                 (unsigned long)broadcaster_broadcast_id);
        return -EBUSY;
    }

    /* Drive the sync straight off the receive state instead of waiting to
     * stumble across the source in our own scan: ext_scan_recv() stops
     * creating syncs the moment req_recv_state is set, so nothing else would.
     */
    addr_le_to_host(addr, recv_state->addr.a.val);

#if CONFIG_EXAMPLE_PAST
    if (past_available) {
        err = pa_sync_with_past(conn_handle, peer_addr);
        if (err) {
            ESP_LOGE(TAG, "Failed to enable PAST receive, err %d", err);
            return -EIO;
        }

        /* Ask for the transfer; the sync arrives on PA_SYNC_PAST. */
        err = esp_ble_audio_bap_scan_delegator_set_pa_state(
                  recv_state->src_id, ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ);
        if (err) {
            ESP_LOGE(TAG, "Failed to set PA state to INFO_REQ, err %d", err);
            return -EIO;
        }

        ESP_LOGI(TAG, "Waiting for SyncInfo transfer...");
        goto pending;
    }
#endif /* CONFIG_EXAMPLE_PAST */

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
    /* Without a transfer the SyncInfo only exists on air, and this controller
     * needs the scanner up to pick it out; pa_sync() takes it back down. */
    err = ext_scan_start();
    if (err) {
        ESP_LOGE(TAG, "Failed to start scanning, err %d", err);
        return -EIO;
    }
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */

    err = pa_sync_create(addr_type_le_to_host(recv_state->addr.type), addr,
                         recv_state->adv_sid);
    if (err) {
        ESP_LOGE(TAG, "Failed to create PA sync, err %d", err);
#if CONFIG_EXAMPLE_SCAN_OFFLOAD
        ext_scan_stop();
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */
        return -EIO;
    }

#if CONFIG_EXAMPLE_PAST
pending:
#endif /* CONFIG_EXAMPLE_PAST */
    req_recv_state = recv_state;
    broadcaster_broadcast_id = recv_state->broadcast_id;
    pa_syncing = true;

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
        /* stop() tears down the BIG; broadcast_sink_stopped_cb deletes the
         * sink after BASS bis_sync has been cleared.
         */
        err = esp_ble_audio_bap_broadcast_sink_stop(broadcast_sink);
        if (err) {
            ESP_LOGE(TAG, "Failed to stop broadcast sink, err %d", err);
            return -EIO;
        }
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

static void broadcast_sink_stopped_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                                      uint8_t reason)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Broadcast sink stopped, reason 0x%02x", reason);

    stream_started = false;

    /* Called from big_stopped after update_recv_state_big_cleared(), so
     * BASS bis_sync is already 0 and rem_src inside delete can succeed.
     * Do not delete from stream_ops.stopped — that runs before bis_sync clear.
     */
    if (broadcast_sink == NULL) {
        return;
    }

    err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete broadcast sink, err %d", err);
        return;
    }

    broadcast_sink = NULL;

#if !CONFIG_EXAMPLE_SCAN_OFFLOAD
    /* No Assistant owns this source, so the receive state was ours and the
     * delete took it with it. Drop the pointer before pa_sync_lost() reads
     * a src_id that no longer exists. */
    req_recv_state = NULL;
#endif /* !CONFIG_EXAMPLE_SCAN_OFFLOAD */
}

static esp_ble_audio_bap_broadcast_sink_cb_t broadcast_sink_cbs = {
    .base_recv = base_recv_cb,
    .syncable  = syncable_cb,
    .stopped   = broadcast_sink_stopped_cb,
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
    ESP_LOGI(TAG, "[SNK #%d] Stream stopped, reason 0x%02x (%u/%u)",
             stream_index(stream), reason, stream_count_stopped, stream_count);

    if (++stream_count_stopped == stream_count) {
        stream_started = false;
        /* Sink delete is deferred to broadcast_sink_stopped_cb (after BASS
         * bis_sync clear). Deleting here races rem_src and leaves WRNs.
         */
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

    /* A connected Assistant owns source selection: without this gate, the
     * synthesized PA_SYNC_LOST that follows its terminate request would clear
     * req_recv_state and we would immediately re-sync the very train it just
     * told us to drop. Anything already streaming keeps running — the gate
     * only stops us from starting something new. */
    if (pa_syncing == false && req_recv_state == NULL &&
            conn_handle == CONN_HANDLE_INIT) {
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

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
        /* Nothing left for the scanner armed by pa_sync_req_cb() to do. */
        if (event->type == ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC) {
            rc = ext_scan_stop();
            if (rc) {
                ESP_LOGW(TAG, "Failed to stop scanning, err %d", rc);
            }
        }
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */

        /* Report it so the assistant can retry or pick another source. Clearing
         * req_recv_state also lets our own scan take another run at it —
         * ext_scan_recv() is gated on it being NULL. */
        if (req_recv_state != NULL) {
            esp_ble_audio_bap_scan_delegator_set_pa_state(req_recv_state->src_id,
                                                          ESP_BLE_AUDIO_BAP_PA_STATE_FAILED);
            req_recv_state = NULL;
        }
        return;
    }

    sync_handle = event->pa_sync.sync_handle;

    ESP_LOGI(TAG, "Broadcast source PA synced, creating Broadcast Sink");

    /* BASE / BIGInfo arrive over the PA channel from here on. A transferred
     * sync never started a scanner, so there is nothing to take down. */
    if (event->type == ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC) {
        rc = ext_scan_stop();
        if (rc) {
            ESP_LOGW(TAG, "Failed to stop scanning, err %d", rc);
        }
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
        /* Publish it before dropping the pointer. A Modify Source asking us to
         * sync again is ignored while the receive state still reads SYNCED, and
         * set_pa_state() is also what re-arms the pa_sync_req callback. */
        if (req_recv_state != NULL) {
            esp_ble_audio_bap_scan_delegator_set_pa_state(
                req_recv_state->src_id, ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED);
        }

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

#if !CONFIG_EXAMPLE_SCAN_OFFLOAD
        ext_scan_start();
#endif /* !CONFIG_EXAMPLE_SCAN_OFFLOAD */
    }
}

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        ext_adv_start(ext_adv_data, sizeof(ext_adv_data));
        return;
    }

    ESP_LOGI(TAG, "Broadcast Assistant connected: handle %u",
             event->acl_connect.conn_handle);

#if CONFIG_EXAMPLE_PAST
    memcpy(peer_addr, event->acl_connect.dst.val, sizeof(peer_addr));
#endif /* CONFIG_EXAMPLE_PAST */

    /* base_recv_cb() only falls back to BIS_SYNC_NO_PREF while nobody is
     * driving us over BASS — without this the fallback would keep overwriting
     * whatever the assistant asked for in bis_sync_req_cb(). */
    conn_handle = event->acl_connect.conn_handle;
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Broadcast Assistant disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    conn_handle = CONN_HANDLE_INIT;

    /* Extended advertising stops on connect; re-arm so the assistant (or
     * another one) can come back. */
    ext_adv_start(ext_adv_data, sizeof(ext_adv_data));
}
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC:
#if CONFIG_EXAMPLE_PAST
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_PAST:
#endif /* CONFIG_EXAMPLE_PAST */
        pa_sync(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_LOST:
        pa_sync_lost(event);
        break;
#if CONFIG_EXAMPLE_SCAN_OFFLOAD
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT:
        acl_connect(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_DISCONNECT:
        acl_disconnect(event);
        break;
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */
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

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
    err = set_device_name();
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    err = ext_adv_start(ext_adv_data, sizeof(ext_adv_data));
    if (err) {
        ESP_LOGE(TAG, "Failed to start advertising, err %d", err);
        return;
    }
#else
    /* Scanning is the thing being offloaded, so it only runs when nobody else
     * is doing it for us. Self-syncing would register a local receive state
     * for the source and the Assistant's Add Source for the same one would
     * come back as a duplicate. */
    ext_scan_start();
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */
}
