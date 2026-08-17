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
#include <stdlib.h>
#include <string.h>

#include "esp_ble_audio_bap_lc3_preset_defs.h"

#include "tmap_central.h"

/* ASE ids repeat across peripherals: an endpoint only means something with its
 * connection. */
struct peer_eps {
    esp_ble_conn_t         *conn;
    uint16_t                conn_handle;
    uint8_t                 sink_ep_count;
    esp_ble_audio_bap_ep_t *sink_eps[TMAP_CEN_SINK_EP_MAX];
    esp_ble_audio_bap_ep_t *source_ep;
    /* PACS Sink Audio Locations, i.e. which ear this peripheral actually is. */
    esp_ble_audio_location_t sink_loc;
    /* CSIS instance inside the peer's CAS, NULL when it is not a set member. */
    esp_ble_audio_csip_set_coordinator_csis_inst_t *csis_inst;
};

static struct peer_eps peers[TMAP_CEN_PEER_COUNT];

/* Which CIS carries which endpoint — the only place the topologies differ. */
struct cis_binding {
    uint8_t peer;       /* index into peers[], ordered by set rank */
    uint8_t sink_idx;   /* which sink endpoint of that peer */
    bool    with_rx;    /* pair the peer's source endpoint into the same CIS */
};

static const struct cis_binding bindings[TMAP_CEN_CIS_COUNT] = {
    { .peer = 0, .sink_idx = 0, .with_rx = true },
#if CONFIG_EXAMPLE_TMAP_CEN_TOPO_1P_2CIS
    { .peer = 0, .sink_idx = 1, .with_rx = false },
#elif CONFIG_EXAMPLE_TMAP_CEN_TOPO_2P_2CIS
    { .peer = 1, .sink_idx = 0, .with_rx = false },
#endif
};

static esp_ble_audio_cap_stream_t unicast_sink_streams[TMAP_CEN_CIS_COUNT];
static esp_ble_audio_cap_stream_t unicast_source_streams[1];

static esp_ble_audio_cap_unicast_group_t *unicast_group;

/* Counts the discoveries this round actually dispatched, so a member that kept
 * its endpoints through a partial teardown is not waited for again. */
static size_t discover_pending;

/* Per stream: a shared sequence number jumps by the stream count. */
struct sink_tx {
    example_audio_tx_scheduler_t scheduler;
    uint16_t                     seq_num;
    uint8_t                     *data;
    uint8_t                      index;
    bool                         running;
};

static struct sink_tx sink_tx[TMAP_CEN_CIS_COUNT];

/* Only CIS #0 is bidirectional, so one RX metrics block is enough. */
static example_audio_rx_metrics_t rx_metrics;

static void unicast_audio_tx(struct sink_tx *tx);

static void peer_discovery_done(void);

/* One preset per rendered channel; only the channel allocation differs. */
ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_2_1_DEFINE(unicast_preset_mono,
                                                   ESP_BLE_AUDIO_LOCATION_MONO_AUDIO,
                                                   ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);
ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_2_1_DEFINE(unicast_preset_left,
                                                   ESP_BLE_AUDIO_LOCATION_FRONT_LEFT,
                                                   ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);
ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_2_1_DEFINE(unicast_preset_right,
                                                   ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT,
                                                   ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA);

/* How many sink streams this binding's peer carries, and which of them this is. */
static void binding_sink_position(size_t binding, size_t *nth, size_t *total)
{
    *nth = 0;
    *total = 0;

    for (size_t i = 0; i < ARRAY_SIZE(bindings); i++) {
        if (bindings[i].peer != bindings[binding].peer) {
            continue;
        }

        if (i < binding) {
            (*nth)++;
        }

        (*total)++;
    }
}

/* Channel follows the peer's PACS Sink Audio Locations — rank says nothing about
 * which ear a device is. Two streams on one peer still split L/R. */
static esp_ble_audio_bap_lc3_preset_t *sink_preset(size_t binding)
{
    esp_ble_audio_location_t loc = peers[bindings[binding].peer].sink_loc;
    size_t nth;
    size_t total;

    binding_sink_position(binding, &nth, &total);

    if (total > 1) {
        return (nth == 0) ? &unicast_preset_left : &unicast_preset_right;
    }

    if (loc == ESP_BLE_AUDIO_LOCATION_FRONT_LEFT) {
        return &unicast_preset_left;
    }

    if (loc == ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT) {
        return &unicast_preset_right;
    }

    return &unicast_preset_mono;
}

static const char *sink_channel_str(size_t binding)
{
    const esp_ble_audio_bap_lc3_preset_t *preset = sink_preset(binding);

    if (preset == &unicast_preset_left) {
        return "front left";
    }

    if (preset == &unicast_preset_right) {
        return "front right";
    }

    return "mono";
}

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static bool stream_is_sink(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_streams); i++) {
        if (&unicast_sink_streams[i].bap_stream == stream) {
            return true;
        }
    }
    return false;
}

static const char *stream_dir_str(const esp_ble_audio_bap_stream_t *stream)
{
    return stream_is_sink(stream) ? "SNK" : "SRC";
}

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_streams); i++) {
        if (&unicast_sink_streams[i].bap_stream == stream) {
            return (int)i;
        }
    }
    for (size_t i = 0; i < ARRAY_SIZE(unicast_source_streams); i++) {
        if (&unicast_source_streams[i].bap_stream == stream) {
            return (int)i;
        }
    }
    return -1;
}

static struct sink_tx *sink_tx_of(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_streams); i++) {
        if (&unicast_sink_streams[i].bap_stream == stream) {
            return &sink_tx[i];
        }
    }

    return NULL;
}

/* Still attached to an endpoint that has not gone back to idle. Streams of a
 * peer that dropped are released by the stack before its disconnect is
 * reported, so this only ever picks out the ones still worth stopping. */
static bool stream_is_live(const esp_ble_audio_cap_stream_t *cap_stream)
{
    const esp_ble_audio_bap_stream_t *bap_stream = &cap_stream->bap_stream;
    esp_ble_audio_bap_ep_info_t ep_info = {0};

    if (bap_stream->conn == NULL || bap_stream->ep == NULL) {
        return false;
    }

    if (esp_ble_audio_bap_ep_get_info(bap_stream->ep, &ep_info) != 0) {
        return false;
    }

    return ep_info.state != ESP_BLE_AUDIO_BAP_EP_STATE_IDLE;
}

/* Handle 0 is valid, so free slots are CONN_HANDLE_INIT rather than zeroed. */
static void peers_reset(void)
{
    memset(peers, 0, sizeof(peers));

    for (size_t i = 0; i < ARRAY_SIZE(peers); i++) {
        peers[i].conn_handle = CONN_HANDLE_INIT;
    }
}

static struct peer_eps *peer_by_handle(uint16_t conn_handle)
{
    if (conn_handle == CONN_HANDLE_INIT) {
        return NULL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(peers); i++) {
        if (peers[i].conn_handle == conn_handle) {
            return &peers[i];
        }
    }

    return NULL;
}

/* Everything recorded for a peer dies with its connection: the endpoints, the
 * conn reference and the CSIS instance are all owned by the link. */
void peer_forget(uint16_t conn_handle)
{
    struct peer_eps *peer = peer_by_handle(conn_handle);

    if (peer == NULL) {
        return;
    }

    memset(peer, 0, sizeof(*peer));
    peer->conn_handle = CONN_HANDLE_INIT;
}

/* A peer that can be streamed on as-is, so a rebuild does not rediscover it. */
static bool peer_is_ready(const struct peer_eps *peer)
{
    return peer != NULL && peer->conn != NULL && peer->sink_ep_count > 0;
}

/* Re-seat into the rank order main.c settled on; collection is by handle. */
static void peers_reorder(void)
{
    struct peer_eps ordered[TMAP_CEN_PEER_COUNT];

    memset(ordered, 0, sizeof(ordered));

    for (size_t i = 0; i < ARRAY_SIZE(ordered); i++) {
        struct peer_eps *peer = peer_by_handle(set_member_handle(i));

        if (peer != NULL) {
            ordered[i] = *peer;
        } else {
            ordered[i].conn_handle = CONN_HANDLE_INIT;
        }
    }

    memcpy(peers, ordered, sizeof(peers));
}

/* CSIP once every member exposes a CSIS instance; ad-hoc is the only form that
 * works without one. Both resolve to the same connections. */
esp_ble_audio_cap_set_type_t set_type_get(void)
{
    size_t count = set_member_count();

    if (count == 0) {
        return ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC;
    }

    for (size_t i = 0; i < count; i++) {
        const struct peer_eps *peer = peer_by_handle(set_member_handle(i));

        if (peer == NULL || peer->csis_inst == NULL) {
            return ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC;
        }
    }

    return ESP_BLE_AUDIO_CAP_SET_TYPE_CSIP;
}

bool set_member_fill(esp_ble_audio_cap_set_member_t *member,
                     esp_ble_audio_cap_set_type_t type,
                     uint16_t conn_handle)
{
    const struct peer_eps *peer = peer_by_handle(conn_handle);

    if (peer == NULL) {
        return false;
    }

    if (type == ESP_BLE_AUDIO_CAP_SET_TYPE_CSIP) {
        member->csip = peer->csis_inst;
        return member->csip != NULL;
    }

    member->member = peer->conn;

    return member->member != NULL;
}

static struct peer_eps *peer_slot_alloc(uint16_t conn_handle)
{
    struct peer_eps *peer = peer_by_handle(conn_handle);

    if (peer != NULL) {
        return peer;
    }

    for (size_t i = 0; i < ARRAY_SIZE(peers); i++) {
        if (peers[i].conn_handle == CONN_HANDLE_INIT) {
            peers[i].conn_handle = conn_handle;
            return &peers[i];
        }
    }

    return NULL;
}

static void unicast_stream_configured_cb(esp_ble_audio_bap_stream_t *stream,
                                         const esp_ble_audio_bap_qos_cfg_pref_t *pref)
{
    ESP_LOGI(TAG, "[%s #%d] Stream configured, QoS preference:",
             stream_dir_str(stream), stream_index(stream));
    example_print_qos_pref(TAG, pref);
}

static void unicast_stream_qos_set_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] QoS set",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_enabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream enabled",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    struct sink_tx *tx;
    esp_err_t err;

    ESP_LOGI(TAG, "[%s #%d] Stream started",
             stream_dir_str(stream), stream_index(stream));

    tx = sink_tx_of(stream);
    if (tx == NULL) {
        /* Source stream: the peripheral transmits, we only count. */
        example_audio_rx_metrics_reset(&rx_metrics);
        return;
    }

    if (stream->qos == NULL || stream->qos->sdu == 0 || stream->qos->interval == 0) {
        ESP_LOGE(TAG, "[SNK #%u] Invalid stream qos", tx->index);
        return;
    }

    if (tx->data == NULL) {
        tx->data = calloc(1, stream->qos->sdu);
        if (tx->data == NULL) {
            ESP_LOGE(TAG, "[SNK #%u] Failed to alloc TX buffer, SDU %u",
                     tx->index, stream->qos->sdu);
            return;
        }
    }

    ESP_LOGI(TAG, "[SNK #%u] Streaming, interval %u, length %u",
             tx->index, stream->qos->interval, stream->qos->sdu);

    tx->seq_num = 0;
    example_audio_tx_scheduler_reset(&tx->scheduler);

    /* Note: esp timer is not accurate enough */
    err = example_audio_tx_scheduler_start(&tx->scheduler, stream->qos->interval);
    if (err) {
        ESP_LOGE(TAG, "[SNK #%u] Failed to start tx scheduler, err %d", tx->index, err);
        return;
    }

    tx->running = true;

    unicast_audio_tx(tx);
}

static void unicast_stream_metadata_updated_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Metadata updated",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_disabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream disabled",
             stream_dir_str(stream), stream_index(stream));
}

/* Keyed on the stream, not the endpoint state: the ISO disconnected callback runs
 * before the endpoint leaves streaming, so a state query would still match. */
static void sink_tx_stop(esp_ble_audio_bap_stream_t *stream)
{
    struct sink_tx *tx;
    esp_err_t err;

    tx = sink_tx_of(stream);
    if (tx == NULL || !tx->running) {
        return;
    }

    err = example_audio_tx_scheduler_stop(&tx->scheduler);
    if (err) {
        ESP_LOGE(TAG, "[SNK #%u] Failed to stop tx scheduler, err %d", tx->index, err);
    }

    tx->running = false;

    if (tx->data != NULL) {
        free(tx->data);
        tx->data = NULL;
    }
}

static void unicast_stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Stream stopped, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    sink_tx_stop(stream);
}

static void unicast_stream_released_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream released",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                                   const esp_ble_iso_recv_info_t *info,
                                   const uint8_t *data, uint16_t len)
{
    char name[24];

    snprintf(name, sizeof(name), "%s #%d",
             stream_dir_str(stream), stream_index(stream));
    rx_metrics.last_sdu_len = len;
    example_audio_rx_metrics_on_recv(info, &rx_metrics, TAG, name);
}

static void unicast_stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    struct sink_tx *tx;
    char name[24];

    tx = sink_tx_of(stream);
    if (tx == NULL) {
        return;
    }

    snprintf(name, sizeof(name), "%s #%d",
             stream_dir_str(stream), stream_index(stream));
    example_audio_tx_scheduler_on_sent(&tx->scheduler, user_data, TAG, name);
}

static void unicast_stream_connected_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream connected",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] ISO disconnected, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    sink_tx_stop(stream);
}

static esp_ble_audio_bap_stream_ops_t unicast_stream_ops = {
    .configured       = unicast_stream_configured_cb,
    .qos_set          = unicast_stream_qos_set_cb,
    .enabled          = unicast_stream_enabled_cb,
    .started          = unicast_stream_started_cb,
    .metadata_updated = unicast_stream_metadata_updated_cb,
    .disabled         = unicast_stream_disabled_cb,
    .stopped          = unicast_stream_stopped_cb,
    .released         = unicast_stream_released_cb,
    .recv             = unicast_stream_recv_cb,
    .sent             = unicast_stream_sent_cb,
    .connected        = unicast_stream_connected_cb,
    .disconnected     = unicast_stream_disconnected_cb,
};

static void unicast_discovery_complete_cb(esp_ble_conn_t *conn, int err,
                                          const esp_ble_audio_csip_set_coordinator_set_member_t *member,
                                          const esp_ble_audio_csip_set_coordinator_csis_inst_t *csis_inst)
{
    struct peer_eps *peer;

    if (err) {
        ESP_LOGE(TAG, "Unicast discovery completed, err %d", err);
        peer_discovery_done();
        return;
    }

    if (csis_inst != NULL) {
        ESP_LOGI(TAG, "Found CAS with CSIS: handle %u", conn->handle);
    } else if (TMAP_CEN_PEER_COUNT > 1) {
        ESP_LOGW(TAG, "Failed to discover CAS CSIS on handle %u", conn->handle);
    } else {
        ESP_LOGI(TAG, "Found CAS: handle %u", conn->handle);
    }

    /* Keep the CSIS instance: it is how CAP names a coordinated set member. */
    peer = peer_slot_alloc(conn->handle);
    if (peer != NULL) {
        peer->conn = conn;
        peer->csis_inst = (esp_ble_audio_csip_set_coordinator_csis_inst_t *)csis_inst;
    }

    err = esp_ble_audio_bap_unicast_client_discover(conn->handle, ESP_BLE_AUDIO_DIR_SINK);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover sink, err %d", err);
        peer_discovery_done();
    }
}

static void unicast_start_complete_cb(int err, esp_ble_conn_t *conn)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast start completed, err %d", err);
    } else {
        ESP_LOGI(TAG, "Unicast start completed");
    }

    /* The lock covers the stream setup procedure, so give it back either way. */
    set_release_lock();

    /* Latching this on a failed start would drive the volume of a set that
     * never came up: the flag stays set until a disconnect clears it. */
    if (!err) {
        vcp_vol_ctlr_streams_started();
    }
}

static void unicast_update_complete_cb(int err, esp_ble_conn_t *conn)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast update completed, err %d", err);
    } else {
        ESP_LOGI(TAG, "Unicast update completed");
    }
}

static void unicast_stop_complete_cb(int err, esp_ble_conn_t *conn)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast stop completed, err %d", err);
    } else {
        ESP_LOGI(TAG, "Unicast stop completed");
    }
}

static esp_ble_audio_cap_initiator_cb_t cap_cb = {
    .unicast_discovery_complete = unicast_discovery_complete_cb,
    .unicast_start_complete     = unicast_start_complete_cb,
    .unicast_update_complete    = unicast_update_complete_cb,
    .unicast_stop_complete      = unicast_stop_complete_cb,
};

static int unicast_group_create(void)
{
    esp_ble_audio_cap_unicast_group_stream_param_t tx_params[TMAP_CEN_CIS_COUNT] = {0};
    esp_ble_audio_cap_unicast_group_stream_param_t rx_params[TMAP_CEN_CIS_COUNT] = {0};
    esp_ble_audio_cap_unicast_group_stream_pair_param_t pair_params[TMAP_CEN_CIS_COUNT] = {0};
    esp_ble_audio_cap_unicast_group_param_t group_param = {0};
    int err;

    /* Reused rather than rebuilt: the CIG stays up while a member is away, so
     * the one that comes back re-enters the slot it left. Releasing a stream
     * keeps it in the group (bt_bap_stream_detach() leaves stream->group and
     * stream->iso alone), and configuring it again rebinds the new endpoint to
     * the CIS the slot already owns. */
    if (unicast_group != NULL) {
        return 0;
    }

    /* One pair is one CIS. tx_param only -> TX-only CIS; both -> bidirectional,
     * which only works for a sink and source on the same peripheral. */
    for (size_t i = 0; i < ARRAY_SIZE(bindings); i++) {
        const struct peer_eps *peer = &peers[bindings[i].peer];

        if (bindings[i].sink_idx >= peer->sink_ep_count) {
            ESP_LOGE(TAG, "[SNK #%zu] Peer %u has no sink endpoint %u",
                     i, bindings[i].peer, bindings[i].sink_idx);
            return -ENODEV;
        }

        ESP_LOGI(TAG, "[SNK #%zu] Peer %u (loc 0x%08lx) renders %s",
                 i, bindings[i].peer, peer->sink_loc, sink_channel_str(i));

        tx_params[i].qos_cfg = &sink_preset(i)->qos;
        tx_params[i].stream = &unicast_sink_streams[i];
        pair_params[i].tx_param = &tx_params[i];

        if (bindings[i].with_rx && peer->source_ep != NULL) {
            rx_params[i].qos_cfg = &unicast_preset_mono.qos;
            rx_params[i].stream = &unicast_source_streams[0];
            pair_params[i].rx_param = &rx_params[i];
        }
    }

    group_param.packing = ESP_BLE_ISO_PACKING_SEQUENTIAL;
    group_param.params_count = ARRAY_SIZE(pair_params);
    group_param.params = pair_params;

    err = esp_ble_audio_cap_unicast_group_create(&group_param, &unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to create unicast group, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Created unicast group: %u CIS", (unsigned)ARRAY_SIZE(pair_params));

    return 0;
}

/* Nothing is connected any more, or the session is being abandoned. The CIG only
 * ever goes away here: while a member is merely absent it stays up, carrying the
 * others and holding that member's slot. */
void cap_initiator_reset(void)
{
    int err;

    /* Endpoints go stale with the connection; stale slots warn "Unhandled endpoint". */
    peers_reset();

    discover_pending = 0;

    if (unicast_group == NULL) {
        return;
    }

    /* Every stream has to be released first, which the departing links did. */
    err = esp_ble_audio_cap_unicast_group_delete(unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete unicast group, err %d", err);
        return;
    }

    unicast_group = NULL;

    ESP_LOGI(TAG, "Deleted unicast group");
}

static int unicast_audio_start(void)
{
    esp_ble_audio_cap_unicast_audio_start_stream_param_t stream_params[TMAP_CEN_CIS_COUNT + 1] = {0};
    esp_ble_audio_cap_unicast_audio_start_param_t param = {0};
    int err;

    param.type = set_type_get();

    /* Endpoint and connection must match, or CAP reports an ep conn mismatch. */
    for (size_t i = 0; i < ARRAY_SIZE(bindings); i++) {
        const struct peer_eps *peer = &peers[bindings[i].peer];

        /* A member that never left is still on air in this slot. Its endpoint
         * would be rejected as not startable, and restarting it would break
         * the audio this whole path exists to preserve. */
        if (stream_is_live(&unicast_sink_streams[i])) {
            ESP_LOGI(TAG, "[SNK #%zu] Already streaming, left alone", i);
            continue;
        }

        if (!set_member_fill(&stream_params[param.count].member, param.type,
                             peer->conn_handle)) {
            ESP_LOGE(TAG, "No set member for handle %u", peer->conn_handle);
            return -ENOENT;
        }

        stream_params[param.count].stream = &unicast_sink_streams[i];
        stream_params[param.count].ep = peer->sink_eps[bindings[i].sink_idx];
        stream_params[param.count].codec_cfg = &sink_preset(i)->codec_cfg;
        param.count++;

        if (bindings[i].with_rx && peer->source_ep != NULL) {
            stream_params[param.count].member = stream_params[param.count - 1].member;
            stream_params[param.count].stream = &unicast_source_streams[0];
            stream_params[param.count].ep = peer->source_ep;
            stream_params[param.count].codec_cfg = &unicast_preset_mono.codec_cfg;
            param.count++;
        }
    }

    if (param.count == 0) {
        ESP_LOGW(TAG, "Nothing left to start, every stream is already up");
        return -EALREADY;
    }

    param.stream_params = stream_params;

    err = esp_ble_audio_cap_initiator_unicast_audio_start(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to start unicast audio, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Started %u unicast stream(s) as %s set", (unsigned)param.count,
             (param.type == ESP_BLE_AUDIO_CAP_SET_TYPE_CSIP) ? "a coordinated" : "an ad-hoc");

    return 0;
}

static void location_cb(esp_ble_conn_t *conn,
                        esp_ble_audio_dir_t dir,
                        esp_ble_audio_location_t loc)
{
    struct peer_eps *peer;

    ESP_LOGI(TAG, "Location, handle %u dir %u loc 0x%08lx", conn->handle, dir, loc);

    if (dir != ESP_BLE_AUDIO_DIR_SINK) {
        return;
    }

    /* Remember which ear this peripheral is; it decides the channel allocation. */
    peer = peer_slot_alloc(conn->handle);
    if (peer != NULL) {
        peer->conn = conn;
        peer->sink_loc = loc;
    }
}

static void available_contexts_cb(esp_ble_conn_t *conn,
                                  esp_ble_audio_context_t snk_ctx,
                                  esp_ble_audio_context_t src_ctx)
{
    ESP_LOGI(TAG, "Available contexts, sink 0x%04x source 0x%04x", snk_ctx, src_ctx);
}

static void config_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                      esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Config response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void qos_cb(esp_ble_audio_bap_stream_t *stream,
                   esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                   esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] QoS response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void enable_cb(esp_ble_audio_bap_stream_t *stream,
                      esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                      esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Enable response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void start_cb(esp_ble_audio_bap_stream_t *stream,
                     esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                     esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Start response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void stop_cb(esp_ble_audio_bap_stream_t *stream,
                    esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                    esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Stop response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void disable_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                       esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Disable response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void metadata_cb(esp_ble_audio_bap_stream_t *stream,
                        esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                        esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Metadata response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

static void release_cb(esp_ble_audio_bap_stream_t *stream,
                       esp_ble_audio_bap_ascs_rsp_code_t rsp_code,
                       esp_ble_audio_bap_ascs_reason_t reason)
{
    ESP_LOGI(TAG, "[%s #%d] Release response, rsp_code %u reason %u",
             stream_dir_str(stream), stream_index(stream), rsp_code, reason);
}

/* peers_reorder() must not move a member that is still on air into another
 * slot: bindings[] would then aim its live CIS at the wrong peripheral. Rank
 * ordering is stable, so this only ever fires on an unranked set. */
static bool live_streams_match_peers(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(bindings); i++) {
        const struct peer_eps *peer = &peers[bindings[i].peer];

        if (stream_is_live(&unicast_sink_streams[i]) &&
                unicast_sink_streams[i].bap_stream.conn != peer->conn) {
            ESP_LOGE(TAG, "[SNK #%zu] Streaming member moved slots, cannot rebuild", i);
            return false;
        }

        if (!bindings[i].with_rx) {
            continue;
        }

        if (stream_is_live(&unicast_source_streams[0]) &&
                unicast_source_streams[0].bap_stream.conn != peer->conn) {
            ESP_LOGE(TAG, "[SRC #0] Streaming member moved slots, cannot rebuild");
            return false;
        }
    }

    return true;
}

/* One CIG covers every peripheral, so it is filled only once the last
 * outstanding discovery is in. The set lock is held until the start procedure
 * reports back, so every failure that returns early has to give it back. */
static void peers_ready(void)
{
    /* A member can go away between cap_initiator_setup() dispatching the
     * discoveries and the last one reporting back, leaving bindings[] pointing
     * at an empty slot. The next member event runs this again. */
    if (set_member_count() < ARRAY_SIZE(peers)) {
        ESP_LOGW(TAG, "Set no longer whole, %zu of %u member(s); not starting",
                 set_member_count(), TMAP_CEN_PEER_COUNT);
        set_release_lock();

        return;
    }

    peers_reorder();

    if (!live_streams_match_peers()) {
        set_release_lock();
        return;
    }

    if (unicast_group_create() != 0) {
        set_release_lock();
        return;
    }

    /* The group is left in place on failure: a member still on air owns a CIS
     * in it, and the next member event reuses it rather than rebuilding. */
    if (unicast_audio_start() != 0) {
        set_release_lock();
    }
}

/* Single exit for every peer, whether its discovery succeeded or not: a peer that
 * contributed no endpoint just makes the group build fail instead of stalling the
 * others. */
static void peer_discovery_done(void)
{
    if (discover_pending > 0) {
        discover_pending--;
    }

    if (discover_pending > 0) {
        return;
    }

    peers_ready();
}

static void discover_cb(esp_ble_conn_t *conn, int err, esp_ble_audio_dir_t dir)
{
    struct peer_eps *peer;

    peer = peer_slot_alloc(conn->handle);
    if (peer == NULL) {
        return;
    }

    peer->conn = conn;

    if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        if (err) {
            ESP_LOGE(TAG, "Sink discovery failed on handle %u, err %d", conn->handle, err);
            peer_discovery_done();
            return;
        }

        ESP_LOGI(TAG, "Sink discover complete: handle %u", conn->handle);

        err = esp_ble_audio_bap_unicast_client_discover(conn->handle, ESP_BLE_AUDIO_DIR_SOURCE);
        if (err) {
            ESP_LOGE(TAG, "Failed to discover source, err %d", err);
            peer_discovery_done();
        }

        return;
    }

    if (dir != ESP_BLE_AUDIO_DIR_SOURCE) {
        return;
    }

    if (err) {
        ESP_LOGE(TAG, "Source discovery failed on handle %u, err %d", conn->handle, err);
    } else {
        ESP_LOGI(TAG, "Source discover complete: handle %u", conn->handle);
    }

    peer_discovery_done();
}

static void pac_record_cb(esp_ble_conn_t *conn, esp_ble_audio_dir_t dir,
                          const esp_ble_audio_codec_cap_t *codec_cap)
{
    example_print_codec_cap(TAG, codec_cap);
}

static void endpoint_cb(esp_ble_conn_t *conn,
                        esp_ble_audio_dir_t dir,
                        esp_ble_audio_bap_ep_t *ep)
{
    struct peer_eps *peer;

    /* Record every endpoint; bindings[] picks which ones are streamed on. */
    peer = peer_slot_alloc(conn->handle);
    if (peer == NULL) {
        ESP_LOGW(TAG, "[%s] Unhandled endpoint, peer table full", dir_str(dir));
        return;
    }

    peer->conn = conn;

    if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        if (peer->source_ep == NULL) {
            ESP_LOGI(TAG, "[%s #0] Endpoint discovered on handle %u",
                     dir_str(dir), conn->handle);
            peer->source_ep = ep;
            return;
        }
    } else if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        if (peer->sink_ep_count < ARRAY_SIZE(peer->sink_eps)) {
            ESP_LOGI(TAG, "[%s #%u] Endpoint discovered on handle %u",
                     dir_str(dir), peer->sink_ep_count, conn->handle);
            peer->sink_eps[peer->sink_ep_count++] = ep;
            return;
        }
    } else {
        ESP_LOGW(TAG, "Endpoint with unknown dir %u on handle %u", dir, conn->handle);
        return;
    }

    /* A peer may expose more ASEs than this topology streams on. */
    ESP_LOGI(TAG, "[%s] Spare endpoint on handle %u", dir_str(dir), conn->handle);
}

static esp_ble_audio_bap_unicast_client_cb_t unicast_client_cbs = {
    .location           = location_cb,
    .available_contexts = available_contexts_cb,
    .config             = config_cb,
    .qos                = qos_cb,
    .enable             = enable_cb,
    .start              = start_cb,
    .stop               = stop_cb,
    .disable            = disable_cb,
    .metadata           = metadata_cb,
    .release            = release_cb,
    .pac_record         = pac_record_cb,
    .endpoint           = endpoint_cb,
    .discover           = discover_cb,
};

static void unicast_audio_tx(struct sink_tx *tx)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_ble_audio_cap_stream_t *cap_stream;
    esp_ble_audio_bap_stream_t *bap_stream;
    esp_err_t err;

    cap_stream = &unicast_sink_streams[tx->index];
    bap_stream = &cap_stream->bap_stream;

    if (bap_stream->ep == NULL) {
        return;
    }

    err = esp_ble_audio_bap_ep_get_info(bap_stream->ep, &ep_info);
    if (err) {
        return;
    }

    if (ep_info.state != ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING) {
        return;
    }

    if (bap_stream->qos == NULL || bap_stream->qos->sdu == 0) {
        ESP_LOGE(TAG, "[SNK #%u] Invalid stream qos", tx->index);
        return;
    }

    if (tx->data == NULL) {
        ESP_LOGE(TAG, "[SNK #%u] TX buffer unavailable, SDU %u",
                 tx->index, bap_stream->qos->sdu);
        return;
    }

    memset(tx->data, (uint8_t)tx->seq_num, bap_stream->qos->sdu);

    err = esp_ble_audio_cap_stream_send(cap_stream, tx->data,
                                        bap_stream->qos->sdu,
                                        tx->seq_num);
    if (err) {
        /* Backpressure is normal; a stuck stream shows up as its TX count stalling. */
        return;
    }

    tx->seq_num++;
}

/* Each stream has its own timer, so this only ever feeds its own stream. */
static void tx_scheduler_cb(void *arg)
{
    unicast_audio_tx(arg);
}

int cap_initiator_init(void)
{
    int err;

    peers_reset();

    err = esp_ble_audio_cap_initiator_register_cb(&cap_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register CAP callbacks (err %d)", err);
        return err;
    }

    err = esp_ble_audio_bap_unicast_client_register_cb(&unicast_client_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register BAP unicast client callbacks (err %d)", err);
        return err;
    }

    for (size_t i = 0; i < ARRAY_SIZE(unicast_sink_streams); i++) {
        esp_ble_audio_cap_stream_ops_register(&unicast_sink_streams[i], &unicast_stream_ops);

        /* Own timer per stream, fed with its own context. */
        sink_tx[i].index = (uint8_t)i;

        err = example_audio_tx_scheduler_init(&sink_tx[i].scheduler,
                                              tx_scheduler_cb,
                                              &sink_tx[i]);
        if (err) {
            ESP_LOGE(TAG, "Failed to init tx scheduler[%zu], err %d", i, err);
            return err;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(unicast_source_streams); i++) {
        esp_ble_audio_cap_stream_ops_register(&unicast_source_streams[i], &unicast_stream_ops);
    }

    ESP_LOGI(TAG, "CAP initiator initialized: %u peer(s), %u CIS",
             TMAP_CEN_PEER_COUNT, TMAP_CEN_CIS_COUNT);

    return 0;
}

int cap_initiator_setup(void)
{
    size_t count;
    int err;

    count = set_member_count();
    if (count == 0) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    discover_pending = 0;

    /* Per member; the group is built when the last one reports back. A member
     * that kept its link through a partial teardown kept its endpoints too, so
     * only the one that came back is rediscovered. */
    for (size_t i = 0; i < count; i++) {
        uint16_t conn_handle = set_member_handle(i);

        if (peer_is_ready(peer_by_handle(conn_handle))) {
            continue;
        }

        err = esp_ble_audio_cap_initiator_unicast_discover(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Failed to discover CAS on handle %u, err %d", conn_handle, err);
            return err;
        }

        discover_pending++;
    }

    ESP_LOGI(TAG, "CAP initiator setup for %zu member(s), %zu to discover",
             count, discover_pending);

    /* Everything was already discovered, so nothing will report back. */
    if (discover_pending == 0) {
        peers_ready();
    }

    return 0;
}
