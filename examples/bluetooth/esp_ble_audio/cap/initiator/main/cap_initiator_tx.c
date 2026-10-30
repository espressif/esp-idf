/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cap_initiator.h"

/* One slot per stream the sample can transmit on, in every enabled mode: the
 * unicast sink streams and the broadcast source streams are distinct objects.
 */
static struct tx_stream tx_streams[SINK_STREAM_COUNT *
                                   (IS_ENABLED(CONFIG_EXAMPLE_UNICAST) +
                                    IS_ENABLED(CONFIG_EXAMPLE_BROADCAST))];

/* One scheduler for the unicast group or broadcast source, running while any
 * stream sends: each tick sends one SDU on every stream.
 */
static example_audio_tx_scheduler_t tx_scheduler;
static size_t tx_stream_count;
#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
static example_audio_tx_sync_t tx_sync;
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

static const char *cap_stream_tx_label(const esp_ble_audio_cap_stream_t *cap_stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};

    if (cap_stream == NULL || cap_stream->bap_stream.ep == NULL) {
        /* Broadcast source streams do not expose an ep; always TX from SRC. */
        return "SRC";
    }

    if (esp_ble_audio_bap_ep_get_info(cap_stream->bap_stream.ep, &ep_info) != 0) {
        return "SRC";
    }

    return (ep_info.dir == ESP_BLE_AUDIO_DIR_SINK) ? "SNK" : "SRC";
}

#if !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
static bool tx_stream_is_streaming(const struct tx_stream *tx_stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    int err;

    if (tx_stream == NULL || tx_stream->stream == NULL) {
        return false;
    }

    /* Broadcast streams have no ep; registration from started_cb implies streaming. */
    if (tx_stream->is_broadcast) {
        return true;
    }

    if (tx_stream->stream->bap_stream.ep == NULL) {
        return false;
    }

    err = esp_ble_audio_bap_ep_get_info(tx_stream->stream->bap_stream.ep, &ep_info);
    if (err) {
        return false;
    }

    return (ep_info.state == ESP_BLE_AUDIO_BAP_EP_STATE_STREAMING);
}

static void cap_initiator_tx_send(struct tx_stream *tx_stream)
{
    const char *label;
    int err;

    if (tx_stream == NULL || tx_stream->stream == NULL) {
        return;
    }

    if (tx_stream_is_streaming(tx_stream) == false) {
        return;
    }

    label = cap_stream_tx_label(tx_stream->stream);

    if (tx_stream->stream->bap_stream.qos == NULL ||
            tx_stream->stream->bap_stream.qos->sdu == 0) {
        ESP_LOGE(TAG, "[%s] Invalid QoS", label);
        return;
    }

    if (tx_stream->data == NULL) {
        ESP_LOGE(TAG, "[%s] Buffer unavailable (SDU %u)",
                 label, tx_stream->stream->bap_stream.qos->sdu);
        return;
    }

    memset(tx_stream->data, (uint8_t)tx_stream->seq_num, tx_stream->stream->bap_stream.qos->sdu);

    err = esp_ble_audio_cap_stream_send(tx_stream->stream, tx_stream->data,
                                        tx_stream->stream->bap_stream.qos->sdu,
                                        tx_stream->seq_num);
    if (err) {
        ESP_LOGD(TAG, "[%s] send failed, err %d", label, err);
        return;
    }

    tx_stream->seq_num++;
}
#else /* !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
static int tx_send_cb(esp_ble_audio_bap_stream_t *stream, uint16_t seq_num,
                      uint32_t timestamp, bool ts_valid)
{
    esp_ble_audio_cap_stream_t *cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t,
                                                          bap_stream);
    struct tx_stream *tx_stream = NULL;
    int err;

    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == cap_stream) {
            tx_stream = &tx_streams[i];
            break;
        }
    }

    memset(tx_stream->data, (uint8_t)seq_num, stream->qos->sdu);
    /* The timestamp, 0 without one, tells the receiver which SDUs of the
     * streams belong together, e.g. the left and right audio of one frame.
     */
    if (!ts_valid) {
        timestamp = 0;
    }
    memcpy(tx_stream->data, &timestamp, MIN(sizeof(timestamp), stream->qos->sdu));

    if (ts_valid) {
        err = esp_ble_audio_cap_stream_send_ts(cap_stream, tx_stream->data, stream->qos->sdu,
                                               seq_num, timestamp);
    } else {
        err = esp_ble_audio_cap_stream_send(cap_stream, tx_stream->data, stream->qos->sdu,
                                            seq_num);
    }
    if (err) {
        ESP_LOGW(TAG, "[%s #%d] Send failed, err %d", cap_stream_tx_label(cap_stream),
                 (int)(tx_stream - tx_streams), err);
    }

    return err;
}
#endif /* !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

void cap_initiator_tx_stream_sent(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream && &tx_streams[i].stream->bap_stream == stream) {
            char name[24];

            snprintf(name, sizeof(name), "%s #%zu",
                     cap_stream_tx_label(tx_streams[i].stream), i);
            example_audio_tx_scheduler_on_sent(&tx_scheduler, user_data, &tx_streams[i].tx_count,
                                               TAG, name);
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
        cap_initiator_tx_send(&tx_streams[i]);
    }
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
}

int cap_initiator_tx_register_stream(esp_ble_audio_cap_stream_t *cap_stream, bool is_broadcast)
{
    int err;

    if (cap_stream == NULL) {
        return -EINVAL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == NULL) {
            const char *label = cap_stream_tx_label(cap_stream);

            if (cap_stream->bap_stream.qos == NULL || cap_stream->bap_stream.qos->sdu == 0) {
                ESP_LOGE(TAG, "[%s #%zu] Invalid QoS", label, i);
                return -EINVAL;
            }

            if (tx_streams[i].data == NULL) {
                tx_streams[i].data = calloc(1, cap_stream->bap_stream.qos->sdu);
                if (tx_streams[i].data == NULL) {
                    ESP_LOGE(TAG, "[%s #%zu] Failed to alloc buffer (SDU %u)",
                             label, i, cap_stream->bap_stream.qos->sdu);
                    return -ENOMEM;
                }
            }

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
            err = example_audio_tx_sync_start(&tx_sync, &cap_stream->bap_stream);
            if (err) {
                ESP_LOGE(TAG, "[%s #%zu] Timestamp TX start failed, err %d", label, i, err);
                return err;
            }
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

            if (tx_stream_count == 0) {
                example_audio_tx_scheduler_reset(&tx_scheduler);

                /* Note: esp timer is not accurate enough */
                err = example_audio_tx_scheduler_start(&tx_scheduler,
                                                       cap_stream->bap_stream.qos->interval);
                if (err) {
                    ESP_LOGE(TAG, "Scheduler start failed, err %d", err);
#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
                    example_audio_tx_sync_stop(&tx_sync, &cap_stream->bap_stream);
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
                    return err;
                }
            }

            tx_streams[i].stream = cap_stream;
            tx_streams[i].is_broadcast = is_broadcast;
            tx_streams[i].seq_num = 0;
            tx_streams[i].tx_count = (example_audio_tx_count_t){0};
            tx_stream_count++;

            ESP_LOGI(TAG, "[%s #%zu] Started TX %s timestamps (SDU %u, interval %u us)",
                     label, i, IS_ENABLED(CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP) ? "with" : "without",
                     cap_stream->bap_stream.qos->sdu,
                     cap_stream->bap_stream.qos->interval);

#if !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
            cap_initiator_tx_send(&tx_streams[i]);
#endif /* !CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */
            return 0;
        }
    }

    ESP_LOGE(TAG, "No free TX stream slot");

    return -ENOMEM;
}

int cap_initiator_tx_unregister_stream(esp_ble_audio_cap_stream_t *cap_stream)
{
    int err;

    if (cap_stream == NULL) {
        return -EINVAL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == cap_stream) {
            const char *label = cap_stream_tx_label(tx_streams[i].stream);

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
            example_audio_tx_sync_stop(&tx_sync, &cap_stream->bap_stream);
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

            /* The other streams keep sending. */
            if (--tx_stream_count == 0) {
                err = example_audio_tx_scheduler_stop(&tx_scheduler);
                if (err) {
                    ESP_LOGE(TAG, "Scheduler stop failed, err %d", err);
                }
            }

            tx_streams[i].stream = NULL;
            tx_streams[i].is_broadcast = false;
            if (tx_streams[i].data != NULL) {
                free(tx_streams[i].data);
                tx_streams[i].data = NULL;
            }

            ESP_LOGI(TAG, "[%s #%zu] Stopped", label, i);
            return 0;
        }
    }

    return -ENODATA;
}

void cap_initiator_tx_init(void)
{
    int err;

    memset(tx_streams, 0, sizeof(tx_streams));

#if CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP
    tx_sync.send = tx_send_cb;
#endif /* CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP */

    err = example_audio_tx_scheduler_init(&tx_scheduler, tx_scheduler_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize tx scheduler, err %d", err);
    }
}
