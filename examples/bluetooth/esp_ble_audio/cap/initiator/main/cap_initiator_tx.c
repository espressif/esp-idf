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
#include <assert.h>
#include <errno.h>

#include "cap_initiator.h"

static struct tx_stream tx_streams[IS_ENABLED(CONFIG_EXAMPLE_UNICAST) + IS_ENABLED(CONFIG_EXAMPLE_BROADCAST)];

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

void cap_initiator_tx_stream_sent(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        if (tx_streams[i].stream && &tx_streams[i].stream->bap_stream == stream) {
            char name[24];

            snprintf(name, sizeof(name), "%s #%zu",
                     cap_stream_tx_label(tx_streams[i].stream), i);
            example_audio_tx_scheduler_on_sent(&tx_streams[i].scheduler, user_data, TAG, name);
            break;
        }
    }
}

static void tx_scheduler_cb(void *arg)
{
    struct tx_stream *tx_stream = arg;

    cap_initiator_tx_send(tx_stream);
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

            tx_streams[i].stream = cap_stream;
            tx_streams[i].is_broadcast = is_broadcast;
            tx_streams[i].seq_num = 0;
            example_audio_tx_scheduler_reset(&tx_streams[i].scheduler);

            err = example_audio_tx_scheduler_start(&tx_streams[i].scheduler, cap_stream->bap_stream.qos->interval);
            if (err) {
                ESP_LOGE(TAG, "[%s #%zu] Scheduler start failed, err %d",
                         label, i, err);
                tx_streams[i].stream = NULL;
                tx_streams[i].is_broadcast = false;
                return err;
            }

            ESP_LOGI(TAG, "[%s #%zu] Started (SDU %u, interval %u us)",
                     label, i,
                     cap_stream->bap_stream.qos->sdu,
                     cap_stream->bap_stream.qos->interval);

            cap_initiator_tx_send(&tx_streams[i]);
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

            err = example_audio_tx_scheduler_stop(&tx_streams[i].scheduler);
            if (err) {
                ESP_LOGE(TAG, "[%s #%zu] Scheduler stop failed, err %d",
                         label, i, err);
                return err;
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

    for (size_t i = 0; i < ARRAY_SIZE(tx_streams); i++) {
        err = example_audio_tx_scheduler_init(&tx_streams[i].scheduler,
                                              tx_scheduler_cb,
                                              &tx_streams[i]);
        if (err) {
            ESP_LOGE(TAG, "Failed to initialize tx scheduler[%zu], err %d", i, err);
            return;
        }
    }
}
