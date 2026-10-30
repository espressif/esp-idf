/*
 * SPDX-FileCopyrightText: 2017-2021 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <string.h>

#include "esp_log.h"

#include "ble_iso_example_utils.h"

#include "esp_ble_iso_common_api.h"

/* Packet count between throughput summary logs.
 * At 10 ms SDU interval this is ~60 s; at 7.5 ms ~45 s.
 */
#define LOG_INTERVAL_PACKETS    6000

/* Lead: how long before its SDU reference time the controller handles an SDU.
 * The best lead of a window follows the difference between the TX clock and
 * the controller clock, while a stall lowers only some SDUs and the ticks then
 * catch up. The minimum is below the lead of a first SDU without timestamp.
 */
#define TX_LEAD_WINDOW_SDUS     50
#define TX_LEAD_MIN_US          200

#if CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
#define TX_SYNC_TAG             "ISO_TX_SYNC"
#endif /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */

static void example_iso_tx_work_handler(struct k_work *work)
{
    example_iso_tx_scheduler_t *scheduler = work->user_data;
    int32_t count;

    assert(scheduler);
    assert(scheduler->cb);

    count = 1 + scheduler->drift;
    scheduler->drift = 0;

    for (int32_t i = 0; i < count; i++) {
        scheduler->cb(scheduler->arg);
    }
}

int example_iso_tx_scheduler_init(example_iso_tx_scheduler_t *scheduler,
                                  example_iso_tx_send_cb_t cb,
                                  void *arg)
{
    assert(scheduler);
    assert(cb);

    memset(scheduler, 0, sizeof(*scheduler));
    scheduler->cb = cb;
    scheduler->arg = arg;

    k_work_init_delayable(&scheduler->timer, example_iso_tx_work_handler);
    scheduler->timer.work.user_data = scheduler;

    return 0;
}

void example_iso_tx_scheduler_reset(example_iso_tx_scheduler_t *scheduler)
{
    assert(scheduler);

    scheduler->drift = 0;
    scheduler->lead_cnt = 0;
    scheduler->lead_max = INT32_MIN;
}

int example_iso_tx_scheduler_start(example_iso_tx_scheduler_t *scheduler,
                                   uint64_t period_us)
{
    assert(scheduler);
    assert(period_us > 0);

    scheduler->period_us = period_us;

    return k_work_schedule_periodic_us(&scheduler->timer, period_us);
}

int example_iso_tx_scheduler_stop(example_iso_tx_scheduler_t *scheduler)
{
    assert(scheduler);

    return k_work_cancel_delayable(&scheduler->timer);
}

/* One frame more when even the best lead runs out, one less when it grows by
 * two SDU intervals. A clock difference moves the lead slowly and is caught at
 * the minimum, so a best lead below minus one SDU interval is a stall or a
 * backlog: one more frame would only add to it.
 */
static void tx_lead_check(example_iso_tx_scheduler_t *scheduler, int32_t lead)
{
    /* SDUs sent before a change are still reported: skip a window. */
    if (scheduler->lead_cnt < 0) {
        scheduler->lead_cnt++;
        return;
    }

    if (lead > scheduler->lead_max) {
        scheduler->lead_max = lead;
    }

    if (++scheduler->lead_cnt < TX_LEAD_WINDOW_SDUS) {
        return;
    }

    scheduler->lead_cnt = 0;
    if (scheduler->lead_max < TX_LEAD_MIN_US &&
            scheduler->lead_max > -(int32_t)scheduler->period_us) {
        scheduler->drift++;
        scheduler->lead_cnt = -TX_LEAD_WINDOW_SDUS;
    } else if (scheduler->lead_max > TX_LEAD_MIN_US + 2 * (int32_t)scheduler->period_us) {
        scheduler->drift--;
        scheduler->lead_cnt = -TX_LEAD_WINDOW_SDUS;
    }
    scheduler->lead_max = INT32_MIN;
}

void example_iso_tx_scheduler_on_sent(example_iso_tx_scheduler_t *scheduler,
                                      const esp_ble_iso_tx_cb_info_t *info,
                                      example_iso_tx_count_t *count,
                                      const char *tag,
                                      const char *obj_name)
{
    const char *name = obj_name ? obj_name : "ISO";
    uint32_t prev;
    bool unframed;
    bool late;

    assert(scheduler);
    assert(info);
    assert(count);

    prev = count->sdus;
    unframed = info->framing == ESP_BLE_ISO_FRAMING_UNFRAMED;

    for (size_t i = 0; i < info->sdu_cnt; i++) {
        /* An SDU reported early (more set) comes again; take the final report. */
        if (unframed && info->sdu[i].unf.more) {
            continue;
        }

        late = unframed && info->sdu[i].unf.late;

        if (late) {
            count->late++;
        }
        if (unframed) {
            tx_lead_check(scheduler, (int32_t)(info->sdu[i].sdu_ref_time -
                                               info->sdu[i].sdu_hdl_time));
        }
        count->sdus++;
    }

    count->sdus += info->sdu_dropped;

    /* The count can step by more than one. */
    if (prev / LOG_INTERVAL_PACKETS != count->sdus / LOG_INTERVAL_PACKETS) {
        if (count->late) {
            ESP_LOGI(tag, "[%s] TX: %u packets, %u late", name, count->sdus, count->late);
            count->late = 0;
        } else {
            ESP_LOGI(tag, "[%s] TX: %u packets", name, count->sdus);
        }
    }
}

#if CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
static example_iso_tx_sync_chan_t *tx_sync_chan(example_iso_tx_sync_t *tx,
                                                const esp_ble_iso_chan_t *chan)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx->chans); i++) {
        if (tx->chans[i].chan == chan) {
            return &tx->chans[i];
        }
    }

    return NULL;
}

/* The first channel started, or NULL. */
static example_iso_tx_sync_chan_t *tx_sync_chan_first(example_iso_tx_sync_t *tx)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx->chans); i++) {
        if (tx->chans[i].chan) {
            return &tx->chans[i];
        }
    }

    return NULL;
}

int example_iso_tx_sync_start(example_iso_tx_sync_t *tx,
                              esp_ble_iso_chan_t *chan,
                              uint32_t sdu_interval_us)
{
    example_iso_tx_sync_chan_t *c;
    esp_ble_iso_info_t info;
    int err;

    assert(tx && tx->send);

    err = esp_ble_iso_chan_get_info(chan, &info);
    if (err) {
        return err;
    }

    /* Peripheral anchors drift with the central's clock. */
    if (info.type != BT_ISO_CHAN_TYPE_BROADCASTER && info.type != BT_ISO_CHAN_TYPE_CENTRAL) {
        ESP_LOGE(TX_SYNC_TAG, "Only BIG broadcaster and CIS central are supported");
        return ESP_ERR_NOT_SUPPORTED;
    }

    c = tx_sync_chan(tx, NULL);
    if (c == NULL) {
        return ESP_ERR_NO_MEM;
    }

    c->chan = chan;
    c->seq_num = 0;
    tx->sdu_interval_us = sdu_interval_us;
    tx->iso_interval_us = (uint32_t)info.iso_interval * 1250;

    return 0;
}

void example_iso_tx_sync_stop(example_iso_tx_sync_t *tx, esp_ble_iso_chan_t *chan)
{
    example_iso_tx_sync_chan_t *c = tx_sync_chan(tx, chan);

    if (c == NULL) {
        return;
    }

    c->chan = NULL;
    /* Before sync the others have sent nothing, so another one can lead. */
    if (c == tx->lead) {
        tx->lead = NULL;
    }

    /* The anchors may change before a channel starts again. */
    if (tx_sync_chan_first(tx) == NULL) {
        tx->synced = false;
        tx->framed = false;
    }
}

static void tx_sync_send(example_iso_tx_sync_t *tx, example_iso_tx_sync_chan_t *c)
{
    tx->send(c->chan, c->seq_num++, tx->timestamp, tx->synced);
}

/* Untimestamped SDUs fill the USPI slots of each event in seq order, all reporting
 * its tx_time_stamp; the first of USPI SDUs at one tx_time_stamp starts the event.
 */
static void tx_sync_find(example_iso_tx_sync_t *tx)
{
    esp_ble_iso_tx_info_t info;
    uint16_t uspi;

    if (esp_ble_iso_chan_get_tx_sync(tx->lead->chan, &info)) {
        return;
    }

    /* Left unsynced, so the group does not start. */
    if (info.offset != 0) {
        ESP_LOGW(TX_SYNC_TAG, "Framed data is not supported yet");
        tx->framed = true;
        return;
    }

    uspi = tx->iso_interval_us / tx->sdu_interval_us;
    if (tx->anchor_seq == UINT16_MAX || info.ts != tx->timestamp) {
        tx->timestamp = info.ts;
        tx->anchor_seq = info.seq_num;
    }

    if ((uint16_t)(info.seq_num - tx->anchor_seq + 1) >= uspi) {
        tx->timestamp += (uint16_t)(tx->lead->seq_num - tx->anchor_seq) * tx->sdu_interval_us;
        tx->synced = true;
    }
}

void example_iso_tx_sync_send_next(example_iso_tx_sync_t *tx)
{
    example_iso_tx_sync_chan_t *first = tx_sync_chan_first(tx);

    if (first == NULL || tx->framed) {
        return;
    }

    /* TX Sync blocks about 2.5 ms, so it is read after sending, and only until
     * synced: from then on the controller places SDUs by timestamp.
     */
    if (!tx->synced) {
        if (tx->lead == NULL) {
            tx->lead = first;
            tx->anchor_seq = UINT16_MAX;
        }
        tx_sync_send(tx, tx->lead);
        tx_sync_find(tx);
    } else {
        /* A channel started late joins at this frame, as early as the others. */
        for (size_t i = 0; i < ARRAY_SIZE(tx->chans); i++) {
            if (tx->chans[i].chan) {
                tx_sync_send(tx, &tx->chans[i]);
            }
        }
        tx->timestamp += tx->sdu_interval_us;
    }
}
#endif /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */

#if CONFIG_EXAMPLE_ISO_RX_ALIGN_CHECK
/* In a group (BIG or CIG), SDUs of one SDU reference time share the RX timestamp,
 * so the TX timestamps in their first 4 bytes must match. RX timestamps may differ
 * by a few us (each CIS anchor is estimated on its own), while SDU reference times
 * are at least 255 us apart.
 */
#define RX_TS_TOLERANCE_US      100

static struct {
    uint32_t rx_ts;
    uint32_t tx_ts;
} rx_last;
static uint32_t rx_aligned;
static uint32_t rx_misaligned;

static void rx_ts_check(const esp_ble_iso_recv_info_t *info, const uint8_t *sdu,
                        uint16_t len, const char *tag, const char *name)
{
    uint32_t tx_ts;
    int32_t d;

    /* Lost, error and empty SDUs are in the channel's own counts. */
    if (!(info->flags & ESP_BLE_ISO_FLAGS_VALID) ||
            !(info->flags & ESP_BLE_ISO_FLAGS_TS) ||
            len < sizeof(tx_ts)) {
        return;
    }

    /* 0: sent without timestamp, at the start. */
    memcpy(&tx_ts, sdu, sizeof(tx_ts));
    if (tx_ts == 0) {
        return;
    }

    /* Beyond the tolerance, the two are of other SDU reference times: not
     * compared. With FT > 1, an SDU received in a later event keeps its RX
     * timestamp but may come after the other channel's next one.
     */
    d = (int32_t)(info->ts - rx_last.rx_ts);
    if (d >= -RX_TS_TOLERANCE_US && d <= RX_TS_TOLERANCE_US) {
        if (tx_ts == rx_last.tx_ts) {
            rx_aligned++;
        } else if (rx_misaligned++ < 5) {
            ESP_LOGW(tag, "[%s] RX: frame %u us with frame %u us", name, tx_ts, rx_last.tx_ts);
        }

        if ((rx_aligned + rx_misaligned) % LOG_INTERVAL_PACKETS == 0) {
            ESP_LOGI(tag, "RX group: total %u, aligned %u, misaligned %u",
                     rx_aligned + rx_misaligned, rx_aligned, rx_misaligned);
        }
    }

    rx_last.rx_ts = info->ts;
    rx_last.tx_ts = tx_ts;
}
#endif /* CONFIG_EXAMPLE_ISO_RX_ALIGN_CHECK */

void example_iso_rx_metrics_reset(example_iso_rx_metrics_t *metrics)
{
    assert(metrics);

    metrics->recv_count = 0;
    metrics->valid_count = 0;
    metrics->error_count = 0;
    metrics->lost_count = 0;
    metrics->empty_sdu_count = 0;
    metrics->last_sdu = NULL;
    metrics->last_sdu_len = 0;

#if CONFIG_EXAMPLE_ISO_RX_ALIGN_CHECK
    /* A channel (re)connects: check the group from scratch. */
    memset(&rx_last, 0, sizeof(rx_last));
    rx_aligned = 0;
    rx_misaligned = 0;
#endif /* CONFIG_EXAMPLE_ISO_RX_ALIGN_CHECK */
}

void example_iso_rx_metrics_on_recv(const esp_ble_iso_recv_info_t *info,
                                    example_iso_rx_metrics_t *metrics,
                                    const char *tag,
                                    const char *obj_name)
{
    assert(info);
    assert(metrics);

    if (info->flags & ESP_BLE_ISO_FLAGS_ERROR) {
        metrics->error_count++;
    }

    if (info->flags & ESP_BLE_ISO_FLAGS_LOST) {
        metrics->lost_count++;
    }

    if (info->flags & ESP_BLE_ISO_FLAGS_VALID) {
        metrics->valid_count++;
        if (metrics->last_sdu_len == 0) {
            metrics->empty_sdu_count++;
        }
    }

    metrics->recv_count++;

    if (metrics->recv_count % LOG_INTERVAL_PACKETS == 0) {
        ESP_LOGI(tag, "[%s] RX: %u packets, valid %u (empty %u), error %u, lost %u",
                 obj_name ? obj_name : "ISO", metrics->recv_count, metrics->valid_count,
                 metrics->empty_sdu_count, metrics->error_count, metrics->lost_count);
    }

#if CONFIG_EXAMPLE_ISO_RX_ALIGN_CHECK
    rx_ts_check(info, metrics->last_sdu, metrics->last_sdu_len, tag, obj_name ? obj_name : "ISO");
#endif /* CONFIG_EXAMPLE_ISO_RX_ALIGN_CHECK */
}
