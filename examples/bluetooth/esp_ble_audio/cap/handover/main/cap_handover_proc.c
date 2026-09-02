/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "esp_timer.h"

#include "cap_handover.h"

/* 1.25 ms units. The real interval, not "unknown": the Acceptor's PA sync needs it. */
#define HANDOVER_PA_INTERVAL    ((PER_ADV_INTERVAL_MS * 1000) / 1250)

/* Must stay valid until the procedure completes, so not on the stack. */
static esp_ble_audio_cap_initiator_broadcast_stream_param_t bcast_stream_params[SINK_STREAM_COUNT];
static esp_ble_audio_cap_initiator_broadcast_subgroup_param_t bcast_subgroup_param;
static esp_ble_audio_cap_initiator_broadcast_create_param_t bcast_create_param;
static uint8_t bcast_bis_data[SINK_STREAM_COUNT][6];

static esp_ble_audio_cap_unicast_group_stream_param_t uni_stream_params[SINK_STREAM_COUNT];
static esp_ble_audio_cap_unicast_group_stream_pair_param_t uni_pair_params[SINK_STREAM_COUNT];
static esp_ble_audio_cap_unicast_group_param_t uni_group_param;
static esp_ble_audio_cap_unicast_audio_start_stream_param_t uni_start_stream_params[SINK_STREAM_COUNT];
static esp_ble_audio_cap_unicast_audio_start_param_t uni_start_param;

static esp_ble_audio_cap_commander_broadcast_reception_stop_member_param_t stop_member_param;
static esp_ble_audio_cap_commander_broadcast_reception_stop_param_t stop_param;

static esp_ble_audio_cap_broadcast_source_t *broadcast_source;
static esp_ble_audio_bap_broadcast_adv_info_t adv_info;
static esp_timer_handle_t switch_timer;
static esp_timer_handle_t start_timer;

static bool adv_registered;
static bool proc_active;
static bool on_broadcast;

/* Source ID of our broadcast on the Acceptor, needed to stop its reception. */
static uint8_t recv_state_src_id;
static bool recv_state_valid;

/* Sweep of the Acceptor's receive states, run once per connection. */
static uint8_t recv_state_cnt;
static uint8_t recv_state_next;
static bool recv_state_sweeping;

static void switch_timer_arm(void)
{
    esp_err_t err;

    if (CONFIG_EXAMPLE_HANDOVER_PERIOD_S == 0 || switch_timer == NULL) {
        return;
    }

    (void)esp_timer_stop(switch_timer);

    err = esp_timer_start_once(switch_timer,
                               (uint64_t)CONFIG_EXAMPLE_HANDOVER_PERIOD_S * 1000000);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to arm handover timer, err %d", err);
    }
}

static void start_timer_cb(void *arg)
{
    if (peer.conn == NULL) {
        return;
    }

    (void)cap_handover_unicast_setup_and_start();
}

/* CAP holds one procedure at a time and the library shares our callback lists,
 * so starting a procedure from inside a callback changes the active procedure
 * under a library handler that has not run yet. Leave the dispatch first.
 */
static void unicast_start_defer(void)
{
    esp_err_t err;

    (void)esp_timer_stop(start_timer);

    err = esp_timer_start_once(start_timer, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to schedule unicast start, err %d", err);
    }
}

/* What the Acceptor's decoder must keep doing across the switch. Channel
 * allocation is absent on purpose: it differs between the two paths by design.
 */
struct codec_shape {
    esp_ble_audio_codec_cfg_freq_t freq;
    esp_ble_audio_codec_cfg_frame_dur_t frame_dur;
    uint16_t octets_per_frame;
    uint8_t frame_blocks;
};

static int codec_shape_get(const esp_ble_audio_codec_cfg_t *codec_cfg,
                           struct codec_shape *out)
{
    if (esp_ble_audio_codec_cfg_get_freq(codec_cfg, &out->freq) != ESP_OK ||
            esp_ble_audio_codec_cfg_get_frame_dur(codec_cfg, &out->frame_dur) != ESP_OK ||
            esp_ble_audio_codec_cfg_get_octets_per_frame(codec_cfg,
                                                         &out->octets_per_frame) != ESP_OK ||
            esp_ble_audio_codec_cfg_get_frame_blocks_per_sdu(codec_cfg, &out->frame_blocks,
                                                             true) != ESP_OK) {
        return -EINVAL;
    }

    return 0;
}

/* Never memcmp: the trailing padding is uninitialised stack. */
static bool codec_shape_eq(const struct codec_shape *a, const struct codec_shape *b)
{
    return a->freq == b->freq &&
           a->frame_dur == b->frame_dur &&
           a->octets_per_frame == b->octets_per_frame &&
           a->frame_blocks == b->frame_blocks;
}

/* A mismatch makes the Acceptor reconfigure its decoder at the switching point,
 * which is audible. One macro feeds both paths, so this only fires if someone
 * later points one of them elsewhere - better loudly than silently.
 */
static int handover_audio_config_check(void)
{
    const esp_ble_audio_bap_lc3_preset_t *bcast = broadcast_preset_get();
    struct codec_shape bcast_shape;

    if (bcast == NULL || codec_shape_get(&bcast->codec_cfg, &bcast_shape) != 0) {
        ESP_LOGE(TAG, "Cannot read broadcast codec configuration");
        return -EINVAL;
    }

    for (size_t i = 0; i < SINK_STREAM_COUNT; i++) {
        const esp_ble_audio_bap_lc3_preset_t *uni = sink_preset_get(i);
        struct codec_shape uni_shape;

        if (uni == NULL || codec_shape_get(&uni->codec_cfg, &uni_shape) != 0) {
            ESP_LOGE(TAG, "Cannot read sink %u codec configuration", (unsigned)i);
            return -EINVAL;
        }

        if (!codec_shape_eq(&uni_shape, &bcast_shape)) {
            ESP_LOGE(TAG, "Codec config differs on sink %u: "
                     "unicast freq %d dur %d octets %u blocks %u vs "
                     "broadcast freq %d dur %d octets %u blocks %u",
                     (unsigned)i,
                     uni_shape.freq, uni_shape.frame_dur,
                     uni_shape.octets_per_frame, uni_shape.frame_blocks,
                     bcast_shape.freq, bcast_shape.frame_dur,
                     bcast_shape.octets_per_frame, bcast_shape.frame_blocks);
            return -EINVAL;
        }

        /* Same SDU rate and framing, or the paths cannot carry the same frame
         * sequence. ISO_Interval is the controller's and only observable once
         * the streams run - cap_handover_diag_sample() prints it.
         */
        if (uni->qos.interval != bcast->qos.interval ||
                uni->qos.framing != bcast->qos.framing) {
            ESP_LOGE(TAG, "QoS differs on sink %u: unicast interval %lu framing %d vs "
                     "broadcast interval %lu framing %d", (unsigned)i,
                     (unsigned long)uni->qos.interval, uni->qos.framing,
                     (unsigned long)bcast->qos.interval, bcast->qos.framing);
            return -EINVAL;
        }

        /* PD may legitimately differ: it is how a rendering-point offset would
         * be absorbed. Nothing uses that yet, so warn rather than fail.
         */
        if (uni->qos.pd != bcast->qos.pd) {
            ESP_LOGW(TAG, "Presentation delay differs on sink %u: %lu vs %lu us",
                     (unsigned)i, (unsigned long)uni->qos.pd, (unsigned long)bcast->qos.pd);
        }
    }

    return 0;
}

static int handover_unicast_to_broadcast(void)
{
    esp_ble_audio_cap_handover_unicast_to_broadcast_param_t param = {0};
    uint8_t *ext_data = NULL;
    uint8_t ext_len = 0;
    size_t count = 0;
    int err;

    err = handover_audio_config_check();
    if (err) {
        return err;
    }

    /* All of them: CAP 7.3.1.10 applies to every CIS carrying Initiator-to-Acceptor audio. */
    for (size_t i = 0; i < peer.sink_ep_count && count < ARRAY_SIZE(bcast_stream_params); i++) {
        esp_ble_audio_cap_stream_t *stream = &peer.sink_streams[i];
        esp_ble_audio_bap_ep_info_t ep_info = {0};
        esp_ble_audio_location_t loc;

        if (stream->bap_stream.ep == NULL ||
                esp_ble_audio_bap_ep_get_info(stream->bap_stream.ep, &ep_info) != ESP_OK ||
                ep_info.state != ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING) {
            continue;
        }

        /* Shared subgroup codec cfg, so the channel allocation goes per BIS. */
        loc = broadcast_bis_location_get(count);
        bcast_bis_data[count][0] = 5;
        bcast_bis_data[count][1] = ESP_BLE_AUDIO_CODEC_CFG_CHAN_ALLOC;
        bcast_bis_data[count][2] = (uint8_t)loc;
        bcast_bis_data[count][3] = (uint8_t)(loc >> 8);
        bcast_bis_data[count][4] = (uint8_t)(loc >> 16);
        bcast_bis_data[count][5] = (uint8_t)(loc >> 24);

        bcast_stream_params[count].stream = stream;
        bcast_stream_params[count].data = bcast_bis_data[count];
        bcast_stream_params[count].data_len = sizeof(bcast_bis_data[count]);
        count++;
    }

    if (count == 0) {
        ESP_LOGW(TAG, "No streaming sink stream to hand over");
        return -ENODEV;
    }

    bcast_subgroup_param.stream_count = count;
    bcast_subgroup_param.stream_params = bcast_stream_params;
    bcast_subgroup_param.codec_cfg = &broadcast_preset_get()->codec_cfg;

    bcast_create_param.subgroup_count = 1;
    bcast_create_param.subgroup_params = &bcast_subgroup_param;
    bcast_create_param.qos = &broadcast_preset_get()->qos;

    /* PA stays stopped: the BASE only exists once the procedure created the source. */
    ext_data = broadcast_ext_adv_data_get(&ext_len);
    if (ext_data == NULL) {
        ESP_LOGE(TAG, "No adv data, cannot hand over to broadcast");
        return -ENOMEM;
    }

    err = ext_adv_start_without_base(ext_data, ext_len);
    free(ext_data);
    if (err) {
        ESP_LOGE(TAG, "Failed to start advertising for handover, err %d", err);
        return err;
    }

    if (adv_registered == false) {
        adv_info.adv_handle = ADV_HANDLE;
        adv_info.addr_type = 0; /* public */
        adv_info.sid = ADV_SID;

        if (local_public_addr_get(adv_info.addr) != 0) {
            ESP_LOGE(TAG, "No local address for the BASS Add Source");
            return -EIO;
        }

        err = esp_ble_audio_bap_broadcast_adv_add(&adv_info);
        if (err) {
            ESP_LOGE(TAG, "Failed to add adv for broadcast source, err %d", err);
            return err;
        }

        adv_registered = true;
    }

    param.type = ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC;
    param.unicast_group = unicast_group_get();
    param.adv_handle = ADV_HANDLE;
    param.pa_interval = HANDOVER_PA_INTERVAL;
    param.broadcast_id = LOCAL_BROADCAST_ID;
    param.broadcast_create_param = &bcast_create_param;

    ESP_LOGI(TAG, "Handover: unicast -> broadcast (%u stream(s))", (unsigned)count);

    cap_handover_diag_sample("u2b-pre", true);

    err = esp_ble_audio_cap_handover_unicast_to_broadcast(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to hand over to broadcast, err %d", err);
        return err;
    }

    return 0;
}

static int handover_broadcast_to_unicast(void)
{
    esp_ble_audio_cap_handover_broadcast_to_unicast_param_t param = {0};
    size_t count = 0;
    int err;

    if (recv_state_valid == false) {
        ESP_LOGW(TAG, "Acceptor has no receive state for our broadcast");
        return -ENODEV;
    }

    /* Must be the broadcast source's own streams. Sink only: a BIS has no return path. */
    for (size_t i = 0; i < peer.sink_ep_count && count < ARRAY_SIZE(uni_stream_params); i++) {
        esp_ble_audio_bap_lc3_preset_t *preset = sink_preset_get(i);

        if (preset == NULL || peer.sink_eps[i] == NULL) {
            continue;
        }

        uni_stream_params[count].qos_cfg = &preset->qos;
        uni_stream_params[count].stream = &peer.sink_streams[i];

        uni_pair_params[count].rx_param = NULL;
        uni_pair_params[count].tx_param = &uni_stream_params[count];

        uni_start_stream_params[count].member.member = peer.conn;
        uni_start_stream_params[count].stream = &peer.sink_streams[i];
        uni_start_stream_params[count].ep = peer.sink_eps[i];
        uni_start_stream_params[count].codec_cfg = &preset->codec_cfg;
        count++;
    }

    if (count == 0) {
        ESP_LOGW(TAG, "No endpoint to hand back to unicast");
        return -ENODEV;
    }

    uni_group_param.params_count = count;
    uni_group_param.params = uni_pair_params;

    uni_start_param.type = ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC;
    uni_start_param.count = count;
    uni_start_param.stream_params = uni_start_stream_params;

    stop_member_param.member.member = peer.conn;
    stop_member_param.src_id = recv_state_src_id;
    stop_member_param.num_subgroups = bcast_create_param.subgroup_count;

    stop_param.type = ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC;
    stop_param.param = &stop_member_param;
    stop_param.count = 1;

    /* Documented as ignored once reception_stop_param is set, but the procedure
     * still matches receive state notifications against this triple. Leaving it
     * zero stalls the handover after the broadcast source stops.
     */
    param.broadcast_id = LOCAL_BROADCAST_ID;
    param.adv_sid = ADV_SID;
    param.adv_type = 0; /* public */

    param.reception_stop_param = &stop_param;
    param.broadcast_source = broadcast_source;
    param.unicast_group_param = &uni_group_param;
    param.unicast_start_param = &uni_start_param;

    ESP_LOGI(TAG, "Handover: broadcast -> unicast (%u stream(s))", (unsigned)count);

    cap_handover_diag_sample("b2u-pre", true);

    err = esp_ble_audio_cap_handover_broadcast_to_unicast(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to hand back to unicast, err %d", err);
        return err;
    }

    /* Stop feeding the broadcast now, not from the stream callbacks: the ISO
     * channel goes down with the BIG terminate, but the disconnected callback
     * trails the controller's event by ~100 ms and every SDU in between is
     * rejected. Unicast re-registers from its own started callback. The other
     * direction does not need this - ASCS delivers CIS callbacks promptly.
     */
    for (size_t i = 0; i < ARRAY_SIZE(peer.sink_streams); i++) {
        (void)cap_handover_tx_unregister_stream(&peer.sink_streams[i]);
    }

    return 0;
}

static void switch_timer_cb(void *arg)
{
    int err;

    if (proc_active) {
        /* CAP runs one procedure at a time; retry on the next tick. */
        switch_timer_arm();
        return;
    }

    if (peer.conn == NULL) {
        return;
    }

    proc_active = true;

    err = on_broadcast ? handover_broadcast_to_unicast()
          : handover_unicast_to_broadcast();
    if (err) {
        proc_active = false;
        switch_timer_arm();
    }
}

static void unicast_to_broadcast_created_cb(esp_ble_audio_cap_broadcast_source_t *source)
{
    uint8_t *per_data;
    uint8_t per_len = 0;
    int err;

    /* The source exists now, so publish its BASE before the Acceptor syncs. */
    per_data = broadcast_base_data_get(source, &per_len);
    if (per_data == NULL) {
        ESP_LOGE(TAG, "Failed to build BASE for handover");
        return;
    }

    err = per_adv_data_start(per_data, per_len);
    free(per_data);
    if (err) {
        ESP_LOGE(TAG, "Failed to publish BASE, err %d", err);
    }
}

static void unicast_to_broadcast_complete_cb(int err, esp_ble_conn_t *conn,
                                             esp_ble_audio_cap_unicast_group_t *group,
                                             esp_ble_audio_cap_broadcast_source_t *source)
{
    /* On success the procedure deleted the unicast group. */
    unicast_group_set(group);
    broadcast_source = source;
    proc_active = false;

    if (err) {
        ESP_LOGE(TAG, "Handover to broadcast failed, err %d", err);

        /* The group is already gone, so a failure leaves us broadcasting to
         * nobody. The stop callback deletes the source and restores unicast.
         */
        if (source != NULL &&
                esp_ble_audio_cap_initiator_broadcast_audio_stop(source) != 0) {
            ESP_LOGE(TAG, "Failed to stop broadcast source");
        }
    } else {
        on_broadcast = true;

        ESP_LOGI(TAG, "Handover to broadcast completed");

        /* BIG terms are readable now; its timestamp is not, until the first
         * SDU. The next b2u-pre sample catches that side with real data.
         */
        cap_handover_diag_sample("u2b-post", false);
    }

    switch_timer_arm();
}

static void broadcast_to_unicast_complete_cb(int err, esp_ble_conn_t *conn,
                                             esp_ble_audio_cap_broadcast_source_t *source,
                                             esp_ble_audio_cap_unicast_group_t *group)
{
    /* On success the procedure deleted the broadcast source. */
    unicast_group_set(group);
    broadcast_source = source;
    proc_active = false;

    if (err) {
        ESP_LOGE(TAG, "Handover to unicast failed, err %d", err);
    } else {
        on_broadcast = false;

        /* Nothing left to announce: drop the announcement and the periodic
         * advertising carrying its BASE before declaring the handover done.
         */
        if (adv_stop() != 0) {
            ESP_LOGW(TAG, "Failed to stop advertising after handover");
        }

        ESP_LOGI(TAG, "Handover to unicast completed");

        /* Mirror of the above: CIS terms readable, timestamp not yet. */
        cap_handover_diag_sample("b2u-post", false);
    }

    switch_timer_arm();
}

static esp_ble_audio_cap_handover_cb_t handover_cb = {
    .unicast_to_broadcast_created  = unicast_to_broadcast_created_cb,
    .unicast_to_broadcast_complete = unicast_to_broadcast_complete_cb,
    .broadcast_to_unicast_complete = broadcast_to_unicast_complete_cb,
};

/* Discovery only subscribes, so read the receive states once to see what the
 * Acceptor already holds. They outlive our reboot, and BAP 6.5.4 rejects an Add
 * Source duplicating the {address, SID, Broadcast_ID} triple - an entry left by
 * an earlier run of ours would fail every handover.
 */
static void recv_state_sweep_next(void)
{
    while (recv_state_next < recv_state_cnt) {
        uint8_t idx = recv_state_next++;
        int err;

        err = esp_ble_audio_bap_broadcast_assistant_read_recv_state(peer.conn_handle, idx);
        if (err == 0) {
            return;     /* Continues in assistant_recv_state_cb */
        }

        ESP_LOGW(TAG, "Failed to read receive state %u, err %d", idx, err);
    }

    recv_state_sweeping = false;

    unicast_start_defer();
}

static void assistant_discover_cb(esp_ble_conn_t *conn, int err, uint8_t recv_state_count)
{
    if (err) {
        ESP_LOGE(TAG, "BASS discovery failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "BASS discovered (%u receive state(s))", recv_state_count);

    recv_state_cnt = recv_state_count;
    recv_state_next = 0;
    recv_state_sweeping = true;

    recv_state_sweep_next();
}

static void assistant_recv_state_cb(esp_ble_conn_t *conn, int err,
                                    const esp_ble_audio_bap_scan_delegator_recv_state_t *state)
{
    bool ours = (err == 0 && state != NULL && state->broadcast_id == LOCAL_BROADCAST_ID);
    uint32_t bis_sync = 0;

    if (recv_state_sweeping) {
        if (ours) {
            ESP_LOGW(TAG, "Removing leftover receive state (src_id %u)", state->src_id);

            if (esp_ble_audio_bap_broadcast_assistant_rem_src(peer.conn_handle,
                                                              state->src_id) == 0) {
                return;     /* Continues in assistant_rem_src_cb */
            }

            ESP_LOGE(TAG, "Failed to remove leftover receive state");
        }

        recv_state_sweep_next();
        return;
    }

    if (!ours) {
        return;
    }

    recv_state_src_id = state->src_id;
    recv_state_valid = true;

    /* The Acceptor notifies per field change, so the same pa_sync arrives more
     * than once (PA synced, then BIG synced). bis_sync tells them apart, and is
     * also what says whether audio is flowing.
     */
    for (uint8_t i = 0; i < state->num_subgroups; i++) {
        bis_sync |= state->subgroups[i].bis_sync;
    }

    ESP_LOGI(TAG, "Acceptor receive state: src_id %u pa_sync %u bis_sync 0x%08lx",
             state->src_id, state->pa_sync_state, (unsigned long)bis_sync);

    /* The Acceptor asks for the periodic advertising train it was told about.
     * We are the advertiser, so hand it over instead of making it scan.
     */
    if (state->pa_sync_state == ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ) {
        /* The Source ID goes in the high octet of the service data; the Acceptor
         * reads it from there to match the transfer to a receive state. */
        err = pa_set_info_transfer(peer.conn_handle, peer.dst,
                                   (uint16_t)state->src_id << 8);
        if (err) {
            ESP_LOGE(TAG, "Failed to transfer PA sync info, err %d", err);
        } else {
            ESP_LOGI(TAG, "PA sync info transferred (src_id %u)", state->src_id);
        }
    }
}

static void assistant_recv_state_removed_cb(esp_ble_conn_t *conn, uint8_t src_id)
{
    if (recv_state_valid && src_id == recv_state_src_id) {
        recv_state_valid = false;
    }
}

static void assistant_rem_src_cb(esp_ble_conn_t *conn, int err)
{
    if (err) {
        ESP_LOGE(TAG, "Remove source failed, err %d", err);
    }

    if (recv_state_sweeping) {
        recv_state_sweep_next();
    }
}

static esp_ble_audio_bap_broadcast_assistant_cb_t assistant_cb = {
    .discover           = assistant_discover_cb,
    .recv_state         = assistant_recv_state_cb,
    .recv_state_removed = assistant_recv_state_removed_cb,
    .rem_src            = assistant_rem_src_cb,
};

int cap_handover_proc_discover(uint16_t conn_handle)
{
    int err;

    err = esp_ble_audio_bap_broadcast_assistant_discover(conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover BASS, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Discovering BASS");
    return 0;
}

void cap_handover_proc_unicast_started(void)
{
    if (on_broadcast) {
        return;
    }

    switch_timer_arm();
}

void cap_handover_proc_broadcast_stopped(esp_ble_audio_cap_broadcast_source_t *source)
{
    if (source != broadcast_source) {
        return;
    }

    /* A source is only deletable once its BIG is down, so finish it here. */
    if (esp_ble_audio_cap_initiator_broadcast_audio_delete(source) != 0) {
        ESP_LOGE(TAG, "Failed to delete broadcast source");
        return;
    }

    broadcast_source = NULL;
    on_broadcast = false;

    (void)adv_stop();

    /* A failed handover left the ASEs released; restore unicast while connected. */
    if (peer.conn != NULL) {
        unicast_start_defer();
    }
}

void cap_handover_proc_reset(void)
{
    if (switch_timer != NULL) {
        (void)esp_timer_stop(switch_timer);
    }

    if (start_timer != NULL) {
        (void)esp_timer_stop(start_timer);
    }

    if (broadcast_source != NULL) {
        /* Deleted by cap_handover_proc_broadcast_stopped once the BIG is down. */
        if (esp_ble_audio_cap_initiator_broadcast_audio_stop(broadcast_source) != 0) {
            ESP_LOGE(TAG, "Failed to stop broadcast source");
        }
    }

    proc_active = false;
    on_broadcast = false;
    recv_state_valid = false;
    recv_state_sweeping = false;
}

int cap_handover_proc_init(void)
{
    const esp_timer_create_args_t timer_args = {
        .callback = switch_timer_cb,
        .name = "cap_handover",
    };
    const esp_timer_create_args_t start_timer_args = {
        .callback = start_timer_cb,
        .name = "cap_uni_start",
    };
    int err;

    err = esp_ble_audio_cap_handover_register_cb(&handover_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register handover callbacks, err %d", err);
        return err;
    }

    err = esp_ble_audio_bap_broadcast_assistant_register_cb(&assistant_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register broadcast assistant callbacks, err %d", err);
        return err;
    }

    err = esp_timer_create(&timer_args, &switch_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create handover timer, err %d", err);
        return err;
    }

    err = esp_timer_create(&start_timer_args, &start_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create unicast start timer, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CAP initiator handover initialized (period %u s)",
             (unsigned)CONFIG_EXAMPLE_HANDOVER_PERIOD_S);

    return 0;
}
