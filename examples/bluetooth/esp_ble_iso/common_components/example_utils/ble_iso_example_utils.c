/*
 * SPDX-FileCopyrightText: 2017-2021 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "esp_log.h"

#include "ble_iso_example_utils.h"

#include "esp_ble_iso_common_api.h"

/* Packet count between throughput summary logs.
 * At 10 ms SDU interval this is ~60 s; at 7.5 ms ~45 s.
 */
#define LOG_INTERVAL_PACKETS    6000

int example_iso_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    if (event->type == BLE_GAP_EVENT_EXT_DISC ||
            event->type == BLE_GAP_EVENT_PERIODIC_SYNC ||
            event->type == BLE_GAP_EVENT_PERIODIC_REPORT ||
            event->type == BLE_GAP_EVENT_PERIODIC_SYNC_LOST ||
            event->type == BLE_GAP_EVENT_CONNECT ||
            event->type == BLE_GAP_EVENT_DISCONNECT ||
            event->type == BLE_GAP_EVENT_ENC_CHANGE) {
        esp_ble_iso_gap_app_post_event(event->type, event);
    }

    return 0;
}

static void example_iso_tx_work_handler(struct k_work *work)
{
    example_iso_tx_scheduler_t *scheduler = work->user_data;
    size_t count;

    assert(scheduler);
    assert(scheduler->cb);

    count = 1 + scheduler->drift;
    scheduler->drift = 0;

    for (size_t i = 0; i < count; i++) {
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
    scheduler->count = 0;
}

int example_iso_tx_scheduler_start(example_iso_tx_scheduler_t *scheduler,
                                   uint64_t period_us)
{
    assert(scheduler);
    assert(period_us > 0);

    return k_work_schedule_periodic(&scheduler->timer, (uint32_t)(period_us / 1000));
}

int example_iso_tx_scheduler_stop(example_iso_tx_scheduler_t *scheduler)
{
    assert(scheduler);

    return k_work_cancel_delayable(&scheduler->timer);
}

void example_iso_tx_scheduler_on_sent(example_iso_tx_scheduler_t *scheduler,
                                      const esp_ble_iso_tx_cb_info_t *info,
                                      const char *tag,
                                      const char *obj_name)
{
    size_t drift_count = 0;

    assert(scheduler);
    assert(info);

    for (size_t i = 0; i < info->pkt_cnt; i++) {
        if (info->pkt[i].drift) {
            drift_count++;
        }
    }

    scheduler->drift += drift_count;
    scheduler->count++;

    if (scheduler->count % LOG_INTERVAL_PACKETS == 0) {
        ESP_LOGI(tag, "[%s] TX: %u packets",
                 obj_name ? obj_name : "ISO", scheduler->count);
    }
}

void example_iso_rx_metrics_reset(example_iso_rx_metrics_t *metrics)
{
    assert(metrics);

    metrics->recv_count = 0;
    metrics->valid_count = 0;
    metrics->error_count = 0;
    metrics->lost_count = 0;
    metrics->null_sdu_count = 0;
    metrics->last_sdu_len = 0;
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
            metrics->null_sdu_count++;
        }
    }

    metrics->recv_count++;

    if (metrics->recv_count % LOG_INTERVAL_PACKETS == 0) {
        ESP_LOGI(tag, "[%s] RX: %u packets",
                 obj_name ? obj_name : "ISO", metrics->recv_count);
    }
}
