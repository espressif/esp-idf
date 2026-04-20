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

static struct tx_stream tx_streams[CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT];

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

void stream_tx_sent(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == stream) {
            char name[24];

            snprintf(name, sizeof(name), "SNK #%zu", i);
            example_audio_tx_scheduler_on_sent(&tx_streams[i].scheduler, user_data,
                                               TAG, name);
            break;
        }
    }
}

static void tx_scheduler_cb(void *arg)
{
    struct tx_stream *tx_stream = arg;

    stream_tx_send(tx_stream);
}

int stream_tx_register(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    if (stream == NULL) {
        return -EINVAL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == NULL) {
            if (stream->qos == NULL || stream->qos->sdu == 0) {
                ESP_LOGE(TAG, "[SNK #%zu] Invalid QoS", i);
                return -EINVAL;
            }

            if (tx_streams[i].data == NULL) {
                tx_streams[i].data = calloc(1, stream->qos->sdu);
                if (tx_streams[i].data == NULL) {
                    ESP_LOGE(TAG, "[SNK #%zu] Failed to alloc buffer (SDU %u)",
                             i, stream->qos->sdu);
                    return -ENOMEM;
                }
            }

            tx_streams[i].stream = stream;
            tx_streams[i].seq_num = 0;
            example_audio_tx_scheduler_reset(&tx_streams[i].scheduler);

            err = example_audio_tx_scheduler_start(&tx_streams[i].scheduler,
                                                   stream->qos->interval);
            if (err) {
                ESP_LOGE(TAG, "[SNK #%zu] Scheduler start failed, err %d", i, err);
                tx_streams[i].stream = NULL;
                return err;
            }

            ESP_LOGI(TAG, "[SNK #%zu] Started (SDU %u, interval %u us)",
                     i, stream->qos->sdu, stream->qos->interval);

            stream_tx_send(&tx_streams[i]);
            return 0;
        }
    }

    ESP_LOGE(TAG, "[SNK] No free TX slot");

    return -ENOMEM;
}

int stream_tx_unregister(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    if (stream == NULL) {
        return -EINVAL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream == stream) {
            err = example_audio_tx_scheduler_stop(&tx_streams[i].scheduler);
            if (err) {
                ESP_LOGE(TAG, "[SNK #%zu] Scheduler stop failed, err %d", i, err);
                return err;
            }

            tx_streams[i].stream = NULL;
            if (tx_streams[i].data != NULL) {
                free(tx_streams[i].data);
                tx_streams[i].data = NULL;
            }

            ESP_LOGI(TAG, "[SNK #%zu] Stopped", i);
            return 0;
        }
    }

    return -ENODATA;
}

void stream_tx_init(void)
{
    int err;

    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        err = example_audio_tx_scheduler_init(&tx_streams[i].scheduler,
                                              tx_scheduler_cb,
                                              &tx_streams[i]);
        if (err) {
            ESP_LOGE(TAG, "[SNK #%zu] Scheduler init failed, err %d", i, err);
            return;
        }
    }
}
