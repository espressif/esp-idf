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

#include "cap_acceptor.h"

#if CONFIG_EXAMPLE_SCAN_SELF
#define TARGET_DEVICE_NAME      "CAP Broadcast Source"
#define TARGET_DEVICE_NAME_LEN  (sizeof(TARGET_DEVICE_NAME) - 1)
#define TARGET_BROADCAST_CODE   "1234"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

#define PA_SYNC_SKIP            0
#define PA_SYNC_TIMEOUT         1000    /* 1000 * 10ms = 10s */
#define PA_SYNC_HANDLE_INIT     UINT16_MAX

enum broadcast_flag {
    FLAG_BROADCAST_SYNC_REQUESTED,
    FLAG_BROADCAST_CODE_REQUIRED,
    FLAG_BROADCAST_CODE_RECEIVED,
    FLAG_BROADCAST_SYNCABLE,
    FLAG_BROADCAST_SYNCING,
    FLAG_BROADCAST_SYNCED,
    FLAG_BASE_RECEIVED,
    FLAG_PA_SYNCING,
    FLAG_PA_SYNCED,
    FLAG_SCANNING,
    FLAG_NUM,
};
ATOMIC_DEFINE(flags, FLAG_NUM);

static struct broadcast_sink {
    const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state;
    uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE];
    esp_ble_audio_bap_broadcast_sink_t *sink;
    esp_ble_audio_cap_stream_t cap_stream;
    uint8_t received_base[UINT8_MAX];
    uint32_t requested_bis_sync;
    uint32_t broadcast_id;
    uint16_t sync_handle;
} broadcast_sink = {
    .sync_handle = PA_SYNC_HANDLE_INIT,
};

static example_audio_rx_metrics_t rx_metrics;

static int pa_sync_create(const bt_addr_le_t *addr, uint8_t adv_sid)
{
    struct ble_gap_periodic_sync_params params = {0};
    ble_addr_t sync_addr = {0};

    sync_addr.type = addr->type;
    memcpy(sync_addr.val, addr->a.val, sizeof(sync_addr.val));
    params.skip = PA_SYNC_SKIP;
    params.sync_timeout = PA_SYNC_TIMEOUT;

    return ble_gap_periodic_adv_sync_create(&sync_addr, adv_sid, &params,
                                            example_audio_gap_event_cb, NULL);
}

static int pa_sync_terminate(void)
{
    int err;

    err = ble_gap_periodic_adv_sync_terminate(broadcast_sink.sync_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to terminate PA sync, err %d", err);
    }

    return err;
}

#if CONFIG_EXAMPLE_SCAN_SELF
static int ext_scan_start(void)
{
    struct ble_gap_disc_params params = {0};
    uint8_t own_addr_type;
    int err;

    err = ble_hs_id_infer_auto(0, &own_addr_type);
    if (err) {
        ESP_LOGE(TAG, "Failed to determine own addr type, err %d", err);
        return err;
    }

    params.passive = 1;
    params.itvl = SCAN_INTERVAL;
    params.window = SCAN_WINDOW;

    err = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &params,
                       example_audio_gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start scanning, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Scanning for broadcast source...");
    return 0;
}

static int ext_scan_stop(void)
{
    return ble_gap_disc_cancel();
}

int check_start_scan(void)
{
    int err;

    if (atomic_test_bit(flags, FLAG_SCANNING)) {
        ESP_LOGW(TAG, "FLAG_SCANNING");
        return -EALREADY;
    }

    if (atomic_test_bit(flags, FLAG_PA_SYNCED)) {
        ESP_LOGW(TAG, "FLAG_PA_SYNCED");
        return -EALREADY;
    }

    if (atomic_test_bit(flags, FLAG_BROADCAST_SYNCED)) {
        ESP_LOGW(TAG, "FLAG_BROADCAST_SYNCED");
        return -EALREADY;
    }

    err = ext_scan_start();
    if (err) {
        return err;
    }

    atomic_set_bit(flags, FLAG_SCANNING);

    return 0;
}
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

static void broadcast_stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[SNK #0] Stream started");

    example_audio_rx_metrics_reset(&rx_metrics);

    atomic_clear_bit(flags, FLAG_BROADCAST_SYNCING);
    atomic_set_bit(flags, FLAG_BROADCAST_SYNCED);
}

static void broadcast_stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[SNK #0] Stream stopped, reason 0x%02x", reason);

    atomic_clear_bit(flags, FLAG_BROADCAST_SYNCING);
    atomic_clear_bit(flags, FLAG_BROADCAST_SYNCED);

#if CONFIG_EXAMPLE_SCAN_SELF
    /* Defer the scan restart to broadcast_pa_lost(): if PA is still
     * synced the broadcaster may just be switching streams, and a
     * premature scan would race the controller's PA-loss timeout. */
    if (atomic_test_bit(flags, FLAG_PA_SYNCED) == false) {
        check_start_scan();
    }
#endif /* CONFIG_EXAMPLE_SCAN_SELF */
}

static void broadcast_stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                                     const esp_ble_iso_recv_info_t *info,
                                     const uint8_t *data, uint16_t len)
{
    rx_metrics.last_sdu_len = len;
    example_audio_rx_metrics_on_recv(info, &rx_metrics, TAG, "SNK #0");
}

static int create_broadcast_sink(void)
{
    int err;

    if (broadcast_sink.sink) {
        return -EALREADY;
    }

    ESP_LOGI(TAG, "Creating broadcast sink for broadcast ID 0x%06X",
             broadcast_sink.broadcast_id);

    err = esp_ble_audio_bap_broadcast_sink_create(broadcast_sink.sync_handle,
                                                  broadcast_sink.broadcast_id,
                                                  &broadcast_sink.sink);
    if (err) {
        ESP_LOGE(TAG, "Failed to create broadcast sink, err %d", err);
        return err;
    }

    return 0;
}

static void check_sync_broadcast(void)
{
    esp_ble_audio_bap_stream_t *sync_stream = &broadcast_sink.cap_stream.bap_stream;
    uint32_t sync_bitfield;
    int err;

    if (atomic_test_bit(flags, FLAG_BASE_RECEIVED) == false) {
        ESP_LOGI(TAG, "FLAG_BASE_RECEIVED");
        return;
    }

    if (atomic_test_bit(flags, FLAG_BROADCAST_SYNCABLE) == false) {
        ESP_LOGI(TAG, "FLAG_BROADCAST_SYNCABLE");
        return;
    }

    if (atomic_test_bit(flags, FLAG_BROADCAST_CODE_REQUIRED) &&
            atomic_test_bit(flags, FLAG_BROADCAST_CODE_RECEIVED) == false) {
        ESP_LOGI(TAG, "FLAG_BROADCAST_CODE_REQUIRED");
        return;
    }

    if (atomic_test_bit(flags, FLAG_BROADCAST_SYNC_REQUESTED) == false) {
        ESP_LOGI(TAG, "FLAG_BROADCAST_SYNC_REQUESTED");
        return;
    }

    if (atomic_test_bit(flags, FLAG_PA_SYNCED) == false) {
        ESP_LOGI(TAG, "FLAG_PA_SYNCED");
        return;
    }

    if (atomic_test_bit(flags, FLAG_BROADCAST_SYNCED) ||
            atomic_test_bit(flags, FLAG_BROADCAST_SYNCING)) {
        ESP_LOGI(TAG, "FLAG_BROADCAST_SYNCED");
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

    ESP_LOGI(TAG, "Syncing to broadcast with bitfield 0x%08X", sync_bitfield);

    /* Sync the BIG */
    err = esp_ble_audio_bap_broadcast_sink_sync(broadcast_sink.sink,
                                                sync_bitfield, &sync_stream,
                                                broadcast_sink.broadcast_code);
    if (err) {
        ESP_LOGE(TAG, "Failed to sync the broadcast sink, err %d", err);
        return;
    }

    atomic_set_bit(flags, FLAG_BROADCAST_SYNCING);
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

    if (!atomic_test_and_set_bit(flags, FLAG_BASE_RECEIVED)) {
        ESP_LOGI(TAG, "BASE received");

        check_sync_broadcast();
    }
}

static void syncable_cb(esp_ble_audio_bap_broadcast_sink_t *sink,
                        const esp_ble_iso_biginfo_t *biginfo)
{
    if (biginfo->encryption == false) {
        atomic_clear_bit(flags, FLAG_BROADCAST_CODE_REQUIRED);
    } else {
        atomic_set_bit(flags, FLAG_BROADCAST_CODE_REQUIRED);

#if CONFIG_EXAMPLE_SCAN_SELF
        /* If self-scanning is enabled, local broadcast code will be used */
        memset(broadcast_sink.broadcast_code, 0, ESP_BLE_ISO_BROADCAST_CODE_SIZE);
        memcpy(broadcast_sink.broadcast_code, TARGET_BROADCAST_CODE,
               MIN(ESP_BLE_ISO_BROADCAST_CODE_SIZE, strlen(TARGET_BROADCAST_CODE)));

        atomic_set_bit(flags, FLAG_BROADCAST_CODE_RECEIVED);
#endif /* CONFIG_EXAMPLE_SCAN_SELF */
    }

    if (!atomic_test_and_set_bit(flags, FLAG_BROADCAST_SYNCABLE)) {
        ESP_LOGI(TAG, "BIGInfo received");

        check_sync_broadcast();
    }
}

static int pa_sync_without_past(const bt_addr_le_t *addr, uint8_t adv_sid)
{
    int err;

    err = pa_sync_create(addr, adv_sid);
    if (err) {
        ESP_LOGE(TAG, "Failed to create PA sync without past, err %d", err);
    }

    return err;
}

static void recv_state_updated_cb(esp_ble_conn_t *conn,
                                  const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state)
{
    ESP_LOGI(TAG, "Receive state updated, pa_sync 0x%02x encrypt 0x%02x",
             recv_state->pa_sync_state, recv_state->encrypt_state);

    for (uint8_t i = 0; i < recv_state->num_subgroups; i++) {
        ESP_LOGI(TAG, "subgroup %d bis_sync: 0x%08x", i, recv_state->subgroups[i].bis_sync);
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
        ESP_LOGW(TAG, "Currently not support PAST");
        return -ENOTSUP;
    } else {
        err = pa_sync_without_past(&recv_state->addr, recv_state->adv_sid);
        if (err) {
            return err;
        }

        ESP_LOGI(TAG, "Syncing without PAST");
    }

    broadcast_sink.broadcast_id = recv_state->broadcast_id;
    atomic_set_bit(flags, FLAG_PA_SYNCING);

    return 0;
}

static int pa_sync_term_req_cb(esp_ble_conn_t *conn,
                               const esp_ble_audio_bap_scan_delegator_recv_state_t *recv_state)
{
    int err;

    ESP_LOGI(TAG, "Received request to terminate PA sync");

    broadcast_sink.recv_state = recv_state;

    err = pa_sync_terminate();
    if (err) {
        return -EIO;
    }

    broadcast_sink.sync_handle = PA_SYNC_HANDLE_INIT;

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

    atomic_set_bit(flags, FLAG_BROADCAST_CODE_RECEIVED);
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
            __builtin_popcount(new_bis_sync_req) > 1) {
        ESP_LOGW(TAG, "Rejecting BIS sync request for 0x%08lx as we do not support that",
                 new_bis_sync_req);
        return -ENOTSUP;
    }

    if (broadcast_sink.requested_bis_sync == new_bis_sync_req) {
        return 0; /* no op */
    }

    if (atomic_test_bit(flags, FLAG_BROADCAST_SYNCED)) {
        /* If the BIS sync request is received while we are already
         * synced, it means that the requested BIS sync has changed.
         */

        /* The stream stopped callback will be called as part of this,
         * and we do not need to wait for any events from the controller.
         * Thus, when this returns, the broadcast sink is stopped.
         */
        err = esp_ble_audio_bap_broadcast_sink_stop(broadcast_sink.sink);
        if (err) {
            ESP_LOGE(TAG, "Failed to stop Broadcast Sink, err %d", err);
            return -EIO;
        }

        err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink.sink);
        if (err) {
            ESP_LOGE(TAG, "Failed to delete Broadcast Sink, err %d", err);
            return -EIO;
        }

        broadcast_sink.sink = NULL;

        atomic_clear_bit(flags, FLAG_BROADCAST_SYNCED);
    }

    broadcast_sink.requested_bis_sync = new_bis_sync_req;

    if (broadcast_sink.requested_bis_sync != 0) {
        atomic_set_bit(flags, FLAG_BROADCAST_SYNC_REQUESTED);

        check_sync_broadcast();
    } else {
        atomic_clear_bit(flags, FLAG_BROADCAST_SYNC_REQUESTED);
    }

    return 0;
}

#if CONFIG_EXAMPLE_SCAN_SELF
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
    bt_addr_le_t addr;
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

    if (atomic_test_bit(flags, FLAG_PA_SYNCING) == false &&
            broadcast_sink.recv_state == NULL) {
        /* Since we are scanning ourselves, we consider this as
         * broadcast sync has been requested.
         */
        broadcast_sink.requested_bis_sync = ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF;
        broadcast_sink.broadcast_id = sr.broadcast_id;

        addr.type = event->ext_scan_recv.addr.type;
        memcpy(addr.a.val, event->ext_scan_recv.addr.val, sizeof(addr.a.val));

        err = pa_sync_create(&addr, event->ext_scan_recv.sid);
        if (err) {
            ESP_LOGE(TAG, "Failed to create PA sync, err %d", err);
            return;
        }

        ESP_LOGI(TAG, "Syncing without PAST from scan");

        atomic_set_bit(flags, FLAG_PA_SYNCING);
        atomic_set_bit(flags, FLAG_BROADCAST_SYNC_REQUESTED);
    }
}
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

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

        atomic_set_bit(flags, FLAG_PA_SYNCED);
        atomic_clear_bit(flags, FLAG_PA_SYNCING);

#if CONFIG_EXAMPLE_SCAN_SELF
        err = ext_scan_stop();
        if (err) {
            ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
            /* Continue anyway - scan stop failure should not block sink creation */
        } else {
            atomic_clear_bit(flags, FLAG_SCANNING);
        }
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

        err = create_broadcast_sink();
        if (err && err != -EALREADY) {
            ESP_LOGE(TAG, "Failed to create broadcast sink, err %d", err);
            return;
        }

        check_sync_broadcast();
    }
}

void broadcast_pa_lost(esp_ble_audio_gap_app_event_t *event)
{
    esp_err_t err;

    if (event->pa_sync_lost.sync_handle != broadcast_sink.sync_handle) {
        return;
    }

    if (broadcast_sink.recv_state) {
        err = esp_ble_audio_bap_scan_delegator_set_pa_state(broadcast_sink.recv_state->src_id,
                                                            ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED);
        if (err) {
            ESP_LOGE(TAG, "Failed to set PA state to ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED, err %d", err);
        }

        err = esp_ble_audio_bap_scan_delegator_rem_src(broadcast_sink.recv_state->src_id);
        if (err) {
            ESP_LOGE(TAG, "Failed to remove receive state source, err %d", err);
        }

        if (broadcast_sink.sink) {
            err = esp_ble_audio_bap_broadcast_sink_delete(broadcast_sink.sink);
            if (err) {
                ESP_LOGE(TAG, "Failed to delete broadcast sink, err %d", err);
            }
        }
    }

    broadcast_sink.recv_state = NULL;
    broadcast_sink.sink = NULL;
    broadcast_sink.requested_bis_sync = 0;
    broadcast_sink.sync_handle = PA_SYNC_HANDLE_INIT;

    atomic_clear_bit(flags, FLAG_BROADCAST_SYNCABLE);
    atomic_clear_bit(flags, FLAG_BROADCAST_SYNCING);
    atomic_clear_bit(flags, FLAG_BROADCAST_SYNCED);
    atomic_clear_bit(flags, FLAG_PA_SYNCED);
    atomic_clear_bit(flags, FLAG_PA_SYNCING);
    atomic_clear_bit(flags, FLAG_BASE_RECEIVED);
    atomic_clear_bit(flags, FLAG_BROADCAST_CODE_REQUIRED);
    atomic_clear_bit(flags, FLAG_BROADCAST_CODE_RECEIVED);
    atomic_clear_bit(flags, FLAG_BROADCAST_SYNC_REQUESTED);

#if CONFIG_EXAMPLE_SCAN_SELF
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

        esp_ble_audio_cap_stream_ops_register(&broadcast_sink.cap_stream,
                                              &broadcast_stream_ops);

        cbs_registered = true;
    }

    ESP_LOGI(TAG, "CAP acceptor broadcast initialized");

    return 0;
}
