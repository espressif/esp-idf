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
            event->type == BLE_GAP_EVENT_PERIODIC_TRANSFER ||
            event->type == BLE_GAP_EVENT_PERIODIC_TRANSFER_V2 ||
            event->type == BLE_GAP_EVENT_CONNECT ||
            event->type == BLE_GAP_EVENT_DISCONNECT ||
            event->type == BLE_GAP_EVENT_ENC_CHANGE) {
        esp_ble_iso_gap_app_post_event(event->type, event);
    } else if (event->type == BLE_GAP_EVENT_REPEAT_PAIRING) {
        /* Peer wants to re-pair on top of an existing bond (e.g. phone
         * cleared its LTK via "Forget device" but our NVS still has it).
         * Delete our stale bond and tell NimBLE to retry pairing.
         */
        struct ble_gap_conn_desc desc = {0};
        int rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        if (rc == 0) {
            ble_store_util_delete_peer(&desc.peer_id_addr);
        }
        return BLE_GAP_REPEAT_PAIRING_RETRY;
    }

    return 0;
}

void example_iso_security_failed_recover(const char *tag, uint16_t conn_handle, uint8_t status)
{
    struct ble_gap_conn_desc desc = {0};
    int rc;

    /* status 13 = BLE_HS_ETIMEOUT: SMP exchange did not complete. Typical
     * cause is asymmetric bond state — the two sides bonded previously,
     * then one side erased its NVS while the other kept the LTK. The side
     * that still has the bond tries encryption with the cached key; the
     * other side has nothing to match it against, so SMP times out. Drop
     * our entry for this peer and disconnect; the next connection runs
     * fresh pairing (peer side recovers via REPEAT_PAIRING).
     */
    ESP_LOGE(tag, "Security change failed, status %u, clearing local bond and reconnecting", status);

    rc = ble_gap_conn_find(conn_handle, &desc);
    if (rc == 0) {
        ble_store_util_delete_peer(&desc.peer_id_addr);
    }

    /* Tear down the link: SMP cannot be retried on the same connection
     * after a failed/timed-out exchange. The next reconnect starts a
     * fresh SMP procedure with the now-cleared bond state.
     */
    ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
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
