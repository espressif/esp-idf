/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"

#if BTC_A2DP_AUDIO_STATS

#include <stdint.h>
#include "btc_a2dp.h"
#include "btc_av.h"

/* Match the number of A2DP connections BTC can track. */
#define BTC_A2DP_AUDIO_STATS_MAX_CONN    BTC_AV_MAX_CONN

typedef enum {
    AUDIO_STATS_IDLE = 0,
    AUDIO_STATS_PLAYING,
    AUDIO_STATS_STOPPED,
} audio_stats_state_t;

typedef struct {
    audio_stats_state_t state;
    esp_a2d_conn_hdl_t  conn_hdl;
    uint32_t            handled_pkts;
    uint32_t            lost_pkts;
    uint32_t            lost_pkts_burst_max;
    uint32_t            glitch_count;
} btc_a2dp_audio_stats_slot_t;

/* Packet and glitch updates, reset, stop, and get all run on the BTC task. */
static btc_a2dp_audio_stats_slot_t btc_audio_stats[BTC_A2DP_AUDIO_STATS_MAX_CONN] = {0};

static void audio_stats_slot_clear(btc_a2dp_audio_stats_slot_t *slot)
{
    slot->state = AUDIO_STATS_IDLE;
    slot->conn_hdl = 0;
    slot->handled_pkts = 0;
    slot->lost_pkts = 0;
    slot->lost_pkts_burst_max = 0;
    slot->glitch_count = 0;
}

static btc_a2dp_audio_stats_slot_t *audio_stats_find(esp_a2d_conn_hdl_t conn_hdl)
{
    if (conn_hdl == 0) {
        return NULL;
    }
    for (int i = 0; i < BTC_A2DP_AUDIO_STATS_MAX_CONN; i++) {
        if ((btc_audio_stats[i].state != AUDIO_STATS_IDLE) && (btc_audio_stats[i].conn_hdl == conn_hdl)) {
            return &btc_audio_stats[i];
        }
    }
    return NULL;
}

/* Reuses the slot for conn_hdl, an idle one, or a stopped one if the table is full. */
static btc_a2dp_audio_stats_slot_t *audio_stats_alloc(esp_a2d_conn_hdl_t conn_hdl)
{
    btc_a2dp_audio_stats_slot_t *slot = audio_stats_find(conn_hdl);
    if (slot != NULL) {
        return slot;
    }
    for (int i = 0; i < BTC_A2DP_AUDIO_STATS_MAX_CONN; i++) {
        if (btc_audio_stats[i].state == AUDIO_STATS_IDLE) {
            btc_audio_stats[i].conn_hdl = conn_hdl;
            return &btc_audio_stats[i];
        }
    }
    for (int i = 0; i < BTC_A2DP_AUDIO_STATS_MAX_CONN; i++) {
        if (btc_audio_stats[i].state == AUDIO_STATS_STOPPED) {
            audio_stats_slot_clear(&btc_audio_stats[i]);
            btc_audio_stats[i].conn_hdl = conn_hdl;
            return &btc_audio_stats[i];
        }
    }
    return NULL;
}

void btc_a2dp_audio_stats_init(void)
{
}

void btc_a2dp_audio_stats_deinit(void)
{
    for (int i = 0; i < BTC_A2DP_AUDIO_STATS_MAX_CONN; i++) {
        audio_stats_slot_clear(&btc_audio_stats[i]);
    }
}

void btc_a2dp_audio_stats_reset(esp_a2d_conn_hdl_t conn_hdl)
{
    btc_a2dp_audio_stats_slot_t *slot;

    if (conn_hdl == 0) {
        return;
    }
    slot = audio_stats_alloc(conn_hdl);
    if (slot != NULL) {
        slot->state = AUDIO_STATS_PLAYING;
        slot->conn_hdl = conn_hdl;
        slot->handled_pkts = 0;
        slot->lost_pkts = 0;
        slot->lost_pkts_burst_max = 0;
        slot->glitch_count = 0;
    }
}

void btc_a2dp_audio_stats_stop(esp_a2d_conn_hdl_t conn_hdl)
{
    btc_a2dp_audio_stats_slot_t *slot = audio_stats_find(conn_hdl);

    if ((slot != NULL) && (slot->state == AUDIO_STATS_PLAYING)) {
        slot->state = AUDIO_STATS_STOPPED;
    }
}

esp_err_t btc_a2dp_audio_stats_get(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_stats_t *stats)
{
    btc_a2dp_audio_stats_slot_t *slot;

    if (stats == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    slot = audio_stats_find(conn_hdl);
    if (slot == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    stats->conn_hdl = slot->conn_hdl;
    stats->handled_pkts = slot->handled_pkts;
    stats->lost_pkts = slot->lost_pkts;
    stats->lost_pkts_burst_max = slot->lost_pkts_burst_max;
    stats->glitch_count = slot->glitch_count;
    return ESP_OK;
}

void btc_a2dp_audio_stats_on_lost(esp_a2d_conn_hdl_t conn_hdl, uint32_t pkts)
{
    btc_a2dp_audio_stats_slot_t *slot;

    if (pkts == 0) {
        return;
    }
    slot = audio_stats_find(conn_hdl);
    if ((slot != NULL) && (slot->state == AUDIO_STATS_PLAYING)) {
        slot->glitch_count++;
        slot->lost_pkts += pkts;
        if (pkts > slot->lost_pkts_burst_max) {
            slot->lost_pkts_burst_max = pkts;
        }
    }
}

void btc_a2dp_audio_stats_on_handled(esp_a2d_conn_hdl_t conn_hdl)
{
    btc_a2dp_audio_stats_slot_t *slot = audio_stats_find(conn_hdl);

    if ((slot != NULL) && (slot->state == AUDIO_STATS_PLAYING)) {
        slot->handled_pkts++;
    }
}

#endif /* BTC_A2DP_AUDIO_STATS */
