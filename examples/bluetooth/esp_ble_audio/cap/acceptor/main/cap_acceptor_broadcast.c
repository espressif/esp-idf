/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "cap_acceptor.h"

#if CONFIG_EXAMPLE_SCAN_SELF
#define TARGET_DEVICE_NAME      "CAP Broadcast Source"
#define TARGET_DEVICE_NAME_LEN  (sizeof(TARGET_DEVICE_NAME) - 1)
#define TARGET_BROADCAST_CODE   "1234"
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

#define PA_SYNC_HANDLE_INIT     UINT16_MAX

enum broadcast_flag {
    FLAG_BROADCAST_SYNC_REQUESTED,
    FLAG_BROADCAST_CODE_REQUIRED,
    FLAG_BROADCAST_CODE_RECEIVED,
    FLAG_BROADCAST_SYNCABLE,
    FLAG_BROADCAST_SYNCING,
    FLAG_BROADCAST_SYNCED,
    FLAG_BROADCAST_RESYNC_PENDING,
    FLAG_BASE_RECEIVED,
    FLAG_PA_SYNCING,
    FLAG_PA_SYNCED,
    FLAG_SCANNING,
};
static uint32_t flags;

static inline bool flag_test(uint8_t bit)
{
    return (flags & (1u << bit)) != 0;
}

static inline void flag_set(uint8_t bit)
{
    flags |= (1u << bit);
}

static inline void flag_clear(uint8_t bit)
{
    flags &= ~(1u << bit);
}

static inline bool flag_test_and_set(uint8_t bit)
{
    bool was = flag_test(bit);
    flag_set(bit);
    return was;
}

static struct broadcast_sink {
    esp_ble_audio_cap_stream_t cap_streams[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];
    const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state;
    uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE];
    esp_ble_audio_bap_broadcast_sink_t *sink;
    uint8_t received_base[UINT8_MAX];
    uint32_t requested_bis_sync;
    uint32_t broadcast_id;
    uint16_t sync_handle;
    uint8_t active_streams;
} broadcast_sink = {
    .sync_handle = PA_SYNC_HANDLE_INIT,
};

static example_audio_rx_metrics_t rx_metrics[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];

#if CONFIG_EXAMPLE_SCAN_SELF
int check_start_scan(void)
{
    int err;

    if (flag_test(FLAG_SCANNING)) {
        ESP_LOGW(TAG, "FLAG_SCANNING");
        return -EALREADY;
    }

    if (flag_test(FLAG_PA_SYNCED)) {
        ESP_LOGW(TAG, "FLAG_PA_SYNCED");
        return -EALREADY;
    }

    if (flag_test(FLAG_BROADCAST_SYNCED)) {
        ESP_LOGW(TAG, "FLAG_BROADCAST_SYNCED");
        return -EALREADY;
    }

    err = ext_scan_start();
    if (err) {
        return err;
    }

    flag_set(FLAG_SCANNING);

    return 0;
}

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

void broadcast_scan_recv(esp_ble_audio_gap_app_event_t *event)
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

    if (flag_test(FLAG_PA_SYNCING) == false &&
            broadcast_sink.recv_state == NULL) {
        /* Since we are scanning ourselves, we consider this as
         * broadcast sync has been requested.
         */
        broadcast_sink.requested_bis_sync = ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF;
        broadcast_sink.broadcast_id = sr.broadcast_id;

        err = pa_sync_create(event->ext_scan_recv.addr.type,
                             event->ext_scan_recv.addr.val,
                             event->ext_scan_recv.sid);
        if (err) {
            return;
        }

        ESP_LOGI(TAG, "Syncing without PAST from scan");

        flag_set(FLAG_PA_SYNCING);
        flag_set(FLAG_BROADCAST_SYNC_REQUESTED);
    }
}
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

static void broadcast_sink_reset(void)
{
    broadcast_sink.recv_state = NULL;
    broadcast_sink.sink = NULL;
    broadcast_sink.requested_bis_sync = 0;
    broadcast_sink.sync_handle = PA_SYNC_HANDLE_INIT;
    broadcast_sink.active_streams = 0;

    flag_clear(FLAG_BROADCAST_SYNCABLE);
    flag_clear(FLAG_BROADCAST_SYNCING);
    flag_clear(FLAG_BROADCAST_SYNCED);
    flag_clear(FLAG_PA_SYNCED);
    flag_clear(FLAG_PA_SYNCING);
    flag_clear(FLAG_BASE_RECEIVED);
    flag_clear(FLAG_BROADCAST_CODE_REQUIRED);
    flag_clear(FLAG_BROADCAST_CODE_RECEIVED);
    flag_clear(FLAG_BROADCAST_SYNC_REQUESTED);
    flag_clear(FLAG_BROADCAST_RESYNC_PENDING);
    flag_clear(FLAG_SCANNING);

#if CONFIG_EXAMPLE_SCAN_SELF
    check_start_scan();
#endif /* CONFIG_EXAMPLE_SCAN_SELF */
}

static void check_sync_broadcast(void)
{
    esp_ble_audio_bap_stream_t *streams[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];
    uint32_t sync_bitfield;
    uint8_t stream_count;
    int err;

    if (flag_test(FLAG_BASE_RECEIVED) == false) {
        ESP_LOGI(TAG, "Waiting for BASE");
        return;
    }

    if (flag_test(FLAG_BROADCAST_SYNCABLE) == false) {
        ESP_LOGI(TAG, "Waiting for BIGInfo");
        return;
    }

    if (flag_test(FLAG_BROADCAST_CODE_REQUIRED) &&
            flag_test(FLAG_BROADCAST_CODE_RECEIVED) == false) {
        ESP_LOGI(TAG, "Waiting for broadcast code");
        return;
    }

    if (flag_test(FLAG_BROADCAST_SYNC_REQUESTED) == false) {
        ESP_LOGI(TAG, "Waiting for sync request");
        return;
    }

    if (flag_test(FLAG_PA_SYNCED) == false) {
        ESP_LOGI(TAG, "Waiting for PA sync");
        return;
    }

    if (flag_test(FLAG_BROADCAST_SYNCING)) {
        ESP_LOGI(TAG, "Already syncing");
        return;
    }

    if (flag_test(FLAG_BROADCAST_SYNCED)) {
        ESP_LOGI(TAG, "Already synced");
        return;
    }

    if (broadcast_sink.requested_bis_sync == ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF) {
        uint32_t base_bis;

        /* Get the first BIS index from the BASE */
        err = esp_ble_audio_bap_base_get_bis_indexes(
                  (esp_ble_audio_bap_base_t *)broadcast_sink.received_base, &base_bis);
        if (err) {
            ESP_LOGE(TAG, "Failed to get BIS indexes from BASE, err %d", err);
            return;
        }

        sync_bitfield = 0;

        for (uint8_t i = ESP_BLE_ISO_BIS_INDEX_MIN; i <= ESP_BLE_ISO_BIS_INDEX_MAX; i++) {
            if (base_bis & ESP_BLE_ISO_BIS_INDEX_BIT(i)) {
                sync_bitfield = ESP_BLE_ISO_BIS_INDEX_BIT(i);
                break;
            }
        }

        if (sync_bitfield == 0) {
            ESP_LOGE(TAG, "No valid BIS index found in BASE");
            return;
        }
    } else {
        sync_bitfield = broadcast_sink.requested_bis_sync;
    }

    stream_count = __builtin_popcount(sync_bitfield);
    if (stream_count > CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT) {
        ESP_LOGE(TAG, "Bitfield 0x%08X needs %u streams, have %u",
                 sync_bitfield, stream_count, CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT);
        return;
    }

    for (uint8_t i = 0; i < stream_count; i++) {
        streams[i] = &broadcast_sink.cap_streams[i].bap_stream;
    }

    ESP_LOGI(TAG, "Syncing to broadcast %p with bitfield 0x%08X (%u streams)",
             broadcast_sink.sink, sync_bitfield, stream_count);

    /* Sync the BIG */
    err = esp_ble_audio_bap_broadcast_sink_sync(broadcast_sink.sink,
                                                sync_bitfield, streams,
                                                broadcast_sink.broadcast_code);
    if (err) {
        ESP_LOGE(TAG, "Failed to sync the broadcast sink, err %d", err);
        return;
    }

    flag_set(FLAG_BROADCAST_SYNCING);
}

static uint8_t broadcast_stream_idx(const esp_ble_audio_bap_stream_t *stream)
{
    for (uint8_t i = 0; i < CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT; i++) {
        if (&broadcast_sink.cap_streams[i].bap_stream == stream) {
            return i;
        }
    }
    return 0xFF;
}

static void broadcast_stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    uint8_t idx = broadcast_stream_idx(stream);

    ESP_LOGI(TAG, "[SNK #%u] Stream started", idx);

    example_audio_rx_metrics_reset(&rx_metrics[idx]);

    broadcast_sink.active_streams++;
    flag_clear(FLAG_BROADCAST_SYNCING);
    flag_set(FLAG_BROADCAST_SYNCED);
}

static void broadcast_stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[SNK #%u] Stream stopped, reason 0x%02x",
             broadcast_stream_idx(stream), reason);

    if (broadcast_sink.active_streams > 0) {
        broadcast_sink.active_streams--;
    }

    /* Multi-BIS: defer teardown / re-sync until every stream of this sink
     * has stopped. Calling _delete or _sync while another BIS is still
     * active hits the lib's "sink not idle" path.
     */
    if (broadcast_sink.active_streams > 0) {
        return;
    }

    flag_clear(FLAG_BROADCAST_SYNCING);
    flag_clear(FLAG_BROADCAST_SYNCED);

    if (flag_test(FLAG_PA_SYNCED) == false) {
        /* Both PA and BIS are gone — no path to recover BIGInfo, so the
         * sink can no longer drive a new BIG sync. Delete it and clear all
         * state.
         */
        if (broadcast_sink.sink) {
            err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink.sink);
            if (err) {
                ESP_LOGE(TAG, "Failed to delete broadcast sink, err %d", err);
                return;
            }
        }

        broadcast_sink_reset();
        return;
    }

    /* PA still synced. Drive the re-sync only when WE initiated the stop
     * for a bitmap change (bis_sync_req_cb sets FLAG_BROADCAST_RESYNC_PENDING). On
     * spontaneous BIG drops per BASS § 3.2.1.9 we just expose the loss via
     * BIS_Sync_State and wait for the assistant to act (Modify Source).
     */
    if (flag_test(FLAG_BROADCAST_RESYNC_PENDING)) {
        flag_clear(FLAG_BROADCAST_RESYNC_PENDING);
        check_sync_broadcast();
    }
}

static void broadcast_stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                                     const esp_ble_iso_recv_info_t *info,
                                     const uint8_t *data, uint16_t len)
{
    uint8_t idx = broadcast_stream_idx(stream);
    char obj_name[10];

    rx_metrics[idx].last_sdu_len = len;
    snprintf(obj_name, sizeof(obj_name), "SNK #%u", idx);
    example_audio_rx_metrics_on_recv(info, &rx_metrics[idx], TAG, obj_name);
}

static void base_recv_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                         const esp_ble_audio_bap_base_t *base,
                         size_t base_size)
{
    if (base_size > sizeof(broadcast_sink.received_base)) {
        ESP_LOGE(TAG, "Too large BASE (%u > %u)",
                 base_size, sizeof(broadcast_sink.received_base));
        return;
    }

    memcpy(broadcast_sink.received_base, base, base_size);

    if (!flag_test_and_set(FLAG_BASE_RECEIVED)) {
        ESP_LOGI(TAG, "BASE received");

        check_sync_broadcast();
    }
}

static void syncable_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                        const esp_ble_iso_biginfo_t *biginfo)
{
    if (biginfo->encryption == false) {
        flag_clear(FLAG_BROADCAST_CODE_REQUIRED);
    } else {
        flag_set(FLAG_BROADCAST_CODE_REQUIRED);

#if CONFIG_EXAMPLE_SCAN_SELF
        /* If self-scanning is enabled, local broadcast code will be used */
        memset(broadcast_sink.broadcast_code, 0, ESP_BLE_ISO_BROADCAST_CODE_SIZE);
        memcpy(broadcast_sink.broadcast_code, TARGET_BROADCAST_CODE,
               MIN(ESP_BLE_ISO_BROADCAST_CODE_SIZE, strlen(TARGET_BROADCAST_CODE)));

        flag_set(FLAG_BROADCAST_CODE_RECEIVED);
#endif /* CONFIG_EXAMPLE_SCAN_SELF */
    }

    if (!flag_test_and_set(FLAG_BROADCAST_SYNCABLE)) {
        ESP_LOGI(TAG, "BIGInfo received");

        check_sync_broadcast();
    }
}

static void recv_state_updated_cb(esp_ble_conn_t *conn,
                                  const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state)
{
    ESP_LOGI(TAG, "Receive state updated, pa_sync 0x%02x encrypt 0x%02x",
             recv_state->pa_sync_state, recv_state->encrypt_state);

    for (uint8_t i = 0; i < recv_state->num_subgroups; i++) {
        ESP_LOGI(TAG, "subgroup %d bis_sync 0x%08x", i, recv_state->subgroups[i].bis_sync);
    }

    if (recv_state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_SYNCED) {
        broadcast_sink.recv_state = recv_state;
    }
}

static int pa_sync_req_cb(esp_ble_conn_t *conn,
                          const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state,
                          bool past_available, uint16_t pa_interval)
{
    int err;

    ESP_LOGI(TAG, "Received request to sync to PA (PAST %savailable): %u",
             past_available ? "" : "not ", recv_state->pa_sync_state);

    broadcast_sink.recv_state = recv_state;

    if (recv_state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_SYNCED ||
            recv_state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ ||
            broadcast_sink.sync_handle != PA_SYNC_HANDLE_INIT) {
        /* Already syncing */
        ESP_LOGW(TAG, "Rejecting PA sync request");
        return -EALREADY;
    }

    if (past_available) {
        err = pa_sync_with_past(conn->handle,
                                recv_state->addr.type, recv_state->addr.a.val);
        if (err) {
            return -EIO;
        }

        /* Tell the Assistant we are waiting for PAST so it sends
         * HCI_LE_Periodic_Advertising_Sync_Transfer. */
        err = esp_ble_audio_bap_scan_delegator_set_pa_state(recv_state->src_id,
                                                            ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ);
        if (err) {
            ESP_LOGE(TAG, "Failed to set PA state to INFO_REQ, err %d", err);
            /* Disarm the PAST receive we just enabled — Assistant was never
             * notified, so any late-arriving PAST would land on stale state
             * (broadcast_id=0, FLAG_PA_SYNCING unset). */
            pa_past_cancel(conn->handle);
            return -EIO;
        }

        ESP_LOGI(TAG, "Waiting for PAST...");
    } else {
        err = pa_sync_create(recv_state->addr.type, recv_state->addr.a.val,
                             recv_state->adv_sid);
        if (err) {
            return err;
        }

        ESP_LOGI(TAG, "Syncing without PAST");
    }

    broadcast_sink.broadcast_id = recv_state->broadcast_id;
    flag_set(FLAG_PA_SYNCING);

    return 0;
}

static int pa_sync_term_req_cb(esp_ble_conn_t *conn,
                               const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state)
{
    int err;

    ESP_LOGI(TAG, "Received request to terminate PA sync");

    broadcast_sink.recv_state = recv_state;

    /* Nothing to terminate if PAST/PA-sync never landed (e.g., PAST setup failed
     * earlier). Issuing the HCI terminate with the sentinel handle gets 0x12
     * (Invalid HCI Command Parameters) back from the controller. */
    if (broadcast_sink.sync_handle == PA_SYNC_HANDLE_INIT) {
        ESP_LOGI(TAG, "PA sync never established, skip terminate");
        return 0;
    }

    err = pa_sync_terminate(broadcast_sink.sync_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to terminate PA sync, err %d", err);
        return -EIO;
    }

    /* Let BLE_GAP_EVENT_PERIODIC_SYNC_LOST drive cleanup via broadcast_pa_lost
     * (it gates on the original sync_handle). Resetting sync_handle here would
     * make that gate miss and leak broadcast_sink.sink.
     */
    return 0;
}

static void broadcast_code_cb(esp_ble_conn_t *conn,
                              const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state,
                              const uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE])
{
    ESP_LOGI(TAG, "Broadcast code received (src_id %u)", recv_state->src_id);

    broadcast_sink.recv_state = recv_state;

    memcpy(broadcast_sink.broadcast_code, broadcast_code,
           ESP_BLE_ISO_BROADCAST_CODE_SIZE);

    flag_set(FLAG_BROADCAST_CODE_RECEIVED);

    /* If BIGInfo arrived before the code, nothing else will retry the sync. */
    check_sync_broadcast();
}

static uint32_t get_req_bis_sync(const uint32_t bis_sync_req[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS])
{
    uint32_t bis_sync = 0;

    for (size_t i = 0; i < CONFIG_BT_BAP_BASS_MAX_SUBGROUPS; i++) {
        bis_sync |= bis_sync_req[i];
    }

    return bis_sync;
}

static int bis_sync_req_cb(esp_ble_conn_t *conn,
                           const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state,
                           const uint32_t bis_sync_req[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS])
{
    const uint32_t new_bis_sync_req = get_req_bis_sync(bis_sync_req);
    esp_err_t err;

    ESP_LOGI(TAG, "BIS sync request received (src_id %u): 0x%08lx",
             recv_state->src_id, bis_sync_req[0]);

    if (new_bis_sync_req != ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF &&
            __builtin_popcount(new_bis_sync_req) > CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT) {
        ESP_LOGW(TAG, "Rejecting BIS sync 0x%08lx: needs %u streams, max %u",
                 new_bis_sync_req, __builtin_popcount(new_bis_sync_req),
                 CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT);
        return -ENOMEM;
    }

    if (broadcast_sink.requested_bis_sync == new_bis_sync_req) {
        ESP_LOGI(TAG, "BIS sync request unchanged (0x%08lx), no-op", new_bis_sync_req);
        return 0;
    }

    /* Update requested state BEFORE calling _stop. The lib can fire
     * stream_stopped_cb synchronously from within _stop, and that
     * callback drives the deferred re-sync via check_sync_broadcast.
     * Updating first ensures the callback reads the new bitfield rather
     * than the stale one.
     */
    ESP_LOGI(TAG, "BIS sync request 0x%08lx -> 0x%08lx",
             broadcast_sink.requested_bis_sync, new_bis_sync_req);

    broadcast_sink.requested_bis_sync = new_bis_sync_req;

    if (broadcast_sink.requested_bis_sync != 0) {
        flag_set(FLAG_BROADCAST_SYNC_REQUESTED);
    } else {
        flag_clear(FLAG_BROADCAST_SYNC_REQUESTED);
    }

    if (flag_test(FLAG_BROADCAST_SYNCED)) {
        /* BIS sync request changed while streaming. Stop the BIG sync but
         * keep the sink object: BIGINFO_RECEIVED / qos / BASE are still
         * valid for the next sync on the same PA + broadcast_id.
         *
         * Leave FLAG_BROADCAST_SYNCED set; stream_stopped_cb clears it.
         * If the new bitmap is non-zero, mark FLAG_BROADCAST_RESYNC_PENDING so that
         * stream_stopped_cb drives a re-sync once BIG teardown completes.
         * Otherwise (bitmap = 0, assistant unsubscribed) there is nothing
         * to re-sync to.
         */
        if (broadcast_sink.requested_bis_sync != 0) {
            flag_set(FLAG_BROADCAST_RESYNC_PENDING);
        }
        err = esp_ble_audio_bap_broadcast_sink_stop(broadcast_sink.sink);
        if (err) {
            flag_clear(FLAG_BROADCAST_RESYNC_PENDING);
            ESP_LOGE(TAG, "Failed to stop Broadcast Sink, err %d", err);
            return -EIO;
        }
    } else if (flag_test(FLAG_BROADCAST_SYNC_REQUESTED)) {
        check_sync_broadcast();
    }

    return 0;
}

void broadcast_pa_synced(esp_ble_audio_gap_app_event_t *event)
{
    bt_addr_le_t addr = {0};
    int err;

    addr.type = event->pa_sync.addr.type;
    memcpy(addr.a.val, event->pa_sync.addr.val, sizeof(addr.a.val));

    if (broadcast_sink.sync_handle == PA_SYNC_HANDLE_INIT ||
            (broadcast_sink.recv_state &&
             broadcast_sink.recv_state->addr.type == addr.type &&
             memcmp(broadcast_sink.recv_state->addr.a.val, addr.a.val, sizeof(addr.a.val)) == 0 &&
             broadcast_sink.recv_state->adv_sid == event->pa_sync.sid)) {
        ESP_LOGI(TAG, "PA sync %u synced for broadcast sink", event->pa_sync.sync_handle);

        if (broadcast_sink.sync_handle == PA_SYNC_HANDLE_INIT) {
            broadcast_sink.sync_handle = event->pa_sync.sync_handle;
        }

        flag_set(FLAG_PA_SYNCED);
        flag_clear(FLAG_PA_SYNCING);

#if CONFIG_EXAMPLE_SCAN_SELF
        err = ext_scan_stop();
        if (err) {
            ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
            /* Continue anyway - scan stop failure should not block sink creation */
        } else {
            flag_clear(FLAG_SCANNING);
        }
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

        if (broadcast_sink.sink == NULL) {
            ESP_LOGI(TAG, "Creating broadcast sink for broadcast ID 0x%06X",
                     broadcast_sink.broadcast_id);

            err = esp_ble_audio_bap_broadcast_sink_create(broadcast_sink.sync_handle,
                                                          broadcast_sink.broadcast_id,
                                                          &broadcast_sink.sink);
            if (err) {
                ESP_LOGE(TAG, "Failed to create broadcast sink, err %d", err);
                return;
            }
        }

        check_sync_broadcast();
    }
}

void broadcast_pa_sync_failed(esp_ble_audio_gap_app_event_t *event)
{
    esp_ble_audio_bap_pa_state_t pa_state;
    esp_err_t err;

    (void)event;

    flag_clear(FLAG_PA_SYNCING);

    if (broadcast_sink.recv_state == NULL) {
        ESP_LOGI(TAG, "PA sync failed with no recv_state to report to");
        return;
    }

    /* BASS § 3.2.1.7: report failure to the Assistant. Use NO_PAST when
     * we were waiting for an Assistant-driven PAST (recv state had
     * pa_sync_state = INFO_REQ), FAILED otherwise.
     */
    if (broadcast_sink.recv_state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ) {
        pa_state = ESP_BLE_AUDIO_BAP_PA_STATE_NO_PAST;
    } else {
        pa_state = ESP_BLE_AUDIO_BAP_PA_STATE_FAILED;
    }

    err = esp_ble_audio_bap_scan_delegator_set_pa_state(broadcast_sink.recv_state->src_id,
                                                        pa_state);
    if (err) {
        ESP_LOGE(TAG, "Failed to set PA state on sync failure, err %d", err);
    }
}

void broadcast_pa_lost(uint16_t sync_handle)
{
    esp_err_t err;

    if (sync_handle != broadcast_sink.sync_handle) {
        return;
    }

    /* Per BASS § 3.2.1.6 / § 3.2.1.9: PA_Sync_State and BIS_Sync_State are
     * independent. PA loss only requires updating PA_Sync_State; the BIG
     * keeps running as long as the broadcaster still transmits it.
     *
     * Skip set_pa_state when an Assistant-driven Modify Source already moved
     * the state to NOT_SYNCED (BASS updates recv_state in place before
     * PERIODIC_SYNC_LOST fires) — calling it again hits the lib's
     * "no state by SID" error path.
     */
    if (broadcast_sink.recv_state &&
            broadcast_sink.recv_state->pa_sync_state != ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED) {
        err = esp_ble_audio_bap_scan_delegator_set_pa_state(broadcast_sink.recv_state->src_id,
                                                            ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED);
        if (err) {
            ESP_LOGE(TAG, "Failed to set PA state to ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED, err %d", err);
        }
    }

    broadcast_sink.sync_handle = PA_SYNC_HANDLE_INIT;
    flag_clear(FLAG_PA_SYNCED);
    flag_clear(FLAG_PA_SYNCING);

    /* BIS still active → BIG keeps running per § 3.2.1.9, leave the sink;
     * stream_stopped_cb will tear it down when BIS eventually stops.
     * Otherwise the sink is bound to a dead PA handle and its cached
     * BASE / BIGInfo are stale: delete the sink and clear the PA-derived
     * flags so the next PA sync creates a fresh sink and lets the lib
     * redeliver BASE / BIGInfo.
     */
    if (flag_test(FLAG_BROADCAST_SYNCING) || flag_test(FLAG_BROADCAST_SYNCED)) {
        return;
    }

    if (broadcast_sink.sink) {
        err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink.sink);
        if (err) {
            ESP_LOGW(TAG, "Deferred sink delete pending after PA loss, err %d", err);
            return;
        }
        broadcast_sink.sink = NULL;
    }

    flag_clear(FLAG_BASE_RECEIVED);
    flag_clear(FLAG_BROADCAST_SYNCABLE);
    flag_clear(FLAG_BROADCAST_CODE_REQUIRED);

#if CONFIG_EXAMPLE_SCAN_SELF
    /* Clear cached recv_state so broadcast_scan_recv's gate can drive
     * a fresh PA sync. */
    broadcast_sink.recv_state = NULL;

    check_start_scan();
#endif /* CONFIG_EXAMPLE_SCAN_SELF */
}

int cap_acceptor_broadcast_init(void)
{
    static bool cbs_registered;
    int err;

    if (cbs_registered == false) {
        static esp_ble_audio_bap_scan_delegator_cb_t scan_delegator_cbs = {
            .recv_state_updated = recv_state_updated_cb,
            .pa_sync_req        = pa_sync_req_cb,
            .pa_sync_term_req   = pa_sync_term_req_cb,
            .broadcast_code     = broadcast_code_cb,
            .bis_sync_req       = bis_sync_req_cb,
        };
        static esp_ble_audio_bap_broadcast_sink_cb_t broadcast_sink_cbs = {
            .base_recv = base_recv_cb,
            .syncable  = syncable_cb,
        };
        static esp_ble_audio_bap_stream_ops_t broadcast_stream_ops = {
            .started = broadcast_stream_started_cb,
            .stopped = broadcast_stream_stopped_cb,
            .recv    = broadcast_stream_recv_cb,
        };

        err = esp_ble_audio_bap_scan_delegator_register(&scan_delegator_cbs);
        if (err) {
            ESP_LOGE(TAG, "Failed to register scan delegator callbacks, err %d", err);
            return err;
        }

        err = esp_ble_audio_bap_broadcast_sink_register_cb(&broadcast_sink_cbs);
        if (err) {
            ESP_LOGE(TAG, "Failed to register broadcast sink callbacks, err %d", err);
            return err;
        }

        for (uint8_t i = 0; i < CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT; i++) {
            err = esp_ble_audio_cap_stream_ops_register(&broadcast_sink.cap_streams[i],
                                                        &broadcast_stream_ops);
            if (err) {
                ESP_LOGE(TAG, "Failed to register broadcast stream ops [%u], err %d", i, err);
                return err;
            }
        }

        cbs_registered = true;
    }

    ESP_LOGI(TAG, "CAP acceptor broadcast initialized");

    return 0;
}
