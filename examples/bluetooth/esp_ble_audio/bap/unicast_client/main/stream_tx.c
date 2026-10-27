/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream_tx.h"

static struct tx_stream tx_streams[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT];

/* One scheduler for the unicast group, running while any stream sends: each
 * tick sends one SDU on every stream.
 */
static example_audio_tx_scheduler_t tx_scheduler;
static size_t tx_stream_count;
#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
static example_audio_tx_sync_t tx_sync;
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

static struct tx_stream *tx_stream_find(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == stream) {
            return &tx_streams[i];
        }
    }

    return NULL;
}

#if !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
static bool stream_is_streaming(const esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    int err;

    /* No-op if stream is not configured */
    if (stream->ep == NULL) {
        return false;
    }

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        return false;
    }

    return (ep_info.state == ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING);
}

static void stream_tx_send(struct tx_stream *tx_stream)
{
    int err;

    if (tx_stream == NULL || tx_stream->stream == NULL) {
        return;
    }

    if (stream_is_streaming(tx_stream->stream) == false) {
        return;
    }

    if (tx_stream->stream->qos == NULL || tx_stream->stream->qos->sdu == 0) {
        ESP_LOGE(TAG, "[SNK TX] Invalid QoS");
        return;
    }

    if (tx_stream->data == NULL) {
        ESP_LOGE(TAG, "[SNK TX] Buffer unavailable (SDU %u)",
                 tx_stream->stream->qos->sdu);
        return;
    }

    memset(tx_stream->data, (uint8_t)tx_stream->seq_num,
           tx_stream->stream->qos->sdu);

    err = esp_ble_audio_bap_stream_send(tx_stream->stream,
                                        tx_stream->data,
                                        tx_stream->stream->qos->sdu,
                                        tx_stream->seq_num);
    if (err) {
        ESP_LOGD(TAG, "[SNK TX] send failed, err %d", err);
        return;
    }

    tx_stream->seq_num++;
}
#else /* !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
static int tx_send_cb(esp_ble_audio_bap_stream_t *stream, uint16_t seq_num,
                      uint32_t timestamp, bool ts_valid)
{
    struct tx_stream *tx_stream = tx_stream_find(stream);
    int err;

    memset(tx_stream->data, (uint8_t)seq_num, stream->qos->sdu);
    /* The timestamp, 0 without one, tells the receiver which SDUs of the
     * streams belong together, e.g. the left and right audio of one frame.
     */
    if (!ts_valid) {
        timestamp = 0;
    }
    memcpy(tx_stream->data, &timestamp, MIN(sizeof(timestamp), stream->qos->sdu));

    if (ts_valid) {
        err = esp_ble_audio_bap_stream_send_ts(stream, tx_stream->data, stream->qos->sdu,
                                               seq_num, timestamp);
    } else {
        err = esp_ble_audio_bap_stream_send(stream, tx_stream->data, stream->qos->sdu, seq_num);
    }
    if (err) {
        ESP_LOGW(TAG, "[SNK #%d] Send failed, err %d", (int)(tx_stream - tx_streams), err);
    }

    return err;
}
#endif /* !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

void stream_tx_sent(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == stream) {
            char name[24];

            snprintf(name, sizeof(name), "SNK #%zu", i);
            example_audio_tx_scheduler_on_sent(&tx_scheduler, user_data,
                                               &tx_streams[i].tx_count, TAG, name);
            break;
        }
    }
}

static void tx_scheduler_cb(void *arg)
{
#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
    example_audio_tx_sync_send_next(&tx_sync);
#else /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        stream_tx_send(&tx_streams[i]);
    }
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
}

int stream_tx_register(esp_ble_audio_bap_stream_t *stream)
{
    struct tx_stream *tx_stream;
    int err;

    if (stream == NULL) {
        return -EINVAL;
    }

    if (tx_stream_find(stream)) {
        return 0; /* already armed */
    }

    tx_stream = tx_stream_find(NULL);
    if (tx_stream == NULL) {
        ESP_LOGE(TAG, "[SNK] No free TX slot");
        return -ENOMEM;
    }

    if (stream->qos == NULL || stream->qos->sdu == 0) {
        ESP_LOGE(TAG, "[SNK #%d] Invalid QoS", (int)(tx_stream - tx_streams));
        return -EINVAL;
    }

    if (tx_stream->data == NULL) {
        tx_stream->data = calloc(1, stream->qos->sdu);
        if (tx_stream->data == NULL) {
            ESP_LOGE(TAG, "[SNK #%d] Failed to alloc buffer (SDU %u)",
                     (int)(tx_stream - tx_streams), stream->qos->sdu);
            return -ENOMEM;
        }
    }

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
    err = example_audio_tx_sync_start(&tx_sync, stream);
    if (err) {
        ESP_LOGE(TAG, "[SNK #%d] Timestamp TX start failed, err %d",
                 (int)(tx_stream - tx_streams), err);
        return err;
    }
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

    if (tx_stream_count == 0) {
        example_audio_tx_scheduler_reset(&tx_scheduler);

        /* Note: esp timer is not accurate enough */
        err = example_audio_tx_scheduler_start(&tx_scheduler, stream->qos->interval);
        if (err) {
            ESP_LOGE(TAG, "Scheduler start failed, err %d", err);
#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
            example_audio_tx_sync_stop(&tx_sync, stream);
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
            return err;
        }
    }

    tx_stream->stream = stream;
    tx_stream->seq_num = 0;
    tx_stream->tx_count = (example_audio_tx_count_t){0};
    tx_stream_count++;

    ESP_LOGI(TAG, "[SNK #%d] Started TX %s timestamps (SDU %u, interval %u us)",
             (int)(tx_stream - tx_streams),
             IS_ENABLED(CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP) ? "with" : "without",
             stream->qos->sdu, stream->qos->interval);

#if !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
    stream_tx_send(tx_stream);
#endif /* !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

    return 0;
}

int stream_tx_unregister(esp_ble_audio_bap_stream_t *stream)
{
    struct tx_stream *tx_stream;
    int err;

    if (stream == NULL) {
        return -EINVAL;
    }

    tx_stream = tx_stream_find(stream);
    if (tx_stream == NULL) {
        return -ENODATA;
    }

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
    example_audio_tx_sync_stop(&tx_sync, stream);
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

    tx_stream->stream = NULL;
    if (tx_stream->data != NULL) {
        free(tx_stream->data);
        tx_stream->data = NULL;
    }

    /* The other streams keep sending. */
    if (--tx_stream_count == 0) {
        err = example_audio_tx_scheduler_stop(&tx_scheduler);
        if (err) {
            ESP_LOGE(TAG, "Scheduler stop failed, err %d", err);
        }
    }

    ESP_LOGI(TAG, "[SNK #%d] Stopped", (int)(tx_stream - tx_streams));

    return 0;
}

void stream_tx_init(void)
{
    int err;

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
    tx_sync.send = tx_send_cb;
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

    err = example_audio_tx_scheduler_init(&tx_scheduler, tx_scheduler_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Scheduler init failed, err %d", err);
    }
}
