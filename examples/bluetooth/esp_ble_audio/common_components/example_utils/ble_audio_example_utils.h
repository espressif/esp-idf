/*
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2024 Demant A/S
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLE_AUDIO_EXAMPLE_UTILS_H_
#define BLE_AUDIO_EXAMPLE_UTILS_H_

#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"

#include "esp_err.h"

#include "zephyr/kernel.h"

#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "host/ble_store.h"

#include "esp_ble_audio_bap_api.h"

#define EXAMPLE_AD_TYPE_FLAGS               BT_DATA_FLAGS
#define EXAMPLE_AD_TYPE_UUID16_SOME         BT_DATA_UUID16_SOME
#define EXAMPLE_AD_TYPE_UUID16_ALL          BT_DATA_UUID16_ALL
#define EXAMPLE_AD_TYPE_NAME_SHORTENED      BT_DATA_NAME_SHORTENED
#define EXAMPLE_AD_TYPE_NAME_COMPLETE       BT_DATA_NAME_COMPLETE
#define EXAMPLE_AD_TYPE_SERVICE_DATA16      BT_DATA_SVC_DATA16
#define EXAMPLE_AD_TYPE_GAP_APPEARANCE      BT_DATA_GAP_APPEARANCE
#define EXAMPLE_AD_TYPE_CSIS_RSI            BT_DATA_CSIS_RSI
#define EXAMPLE_AD_TYPE_BROADCAST_NAME      BT_DATA_BROADCAST_NAME

#define EXAMPLE_AD_FLAGS_LIMITED            BT_LE_AD_LIMITED
#define EXAMPLE_AD_FLAGS_GENERAL            BT_LE_AD_GENERAL
#define EXAMPLE_AD_FLAGS_NO_BREDR           BT_LE_AD_NO_BREDR

#define EXAMPLE_ADV_PROP_CONNECTABLE        BT_GAP_ADV_PROP_CONNECTABLE
#define EXAMPLE_ADV_PROP_SCANNABLE          BT_GAP_ADV_PROP_SCANNABLE
#define EXAMPLE_ADV_PROP_DIRECTED           BT_GAP_ADV_PROP_DIRECTED
#define EXAMPLE_ADV_PROP_SCAN_RESPONSE      BT_GAP_ADV_PROP_SCAN_RESPONSE
#define EXAMPLE_ADV_PROP_EXT_ADV            BT_GAP_ADV_PROP_EXT_ADV

#define EXAMPLE_BYTES_LIST_LE16             BT_BYTES_LIST_LE16
#define EXAMPLE_BYTES_LIST_LE24             BT_BYTES_LIST_LE24
#define EXAMPLE_BYTES_LIST_LE32             BT_BYTES_LIST_LE32
#define EXAMPLE_BYTES_LIST_LE40             BT_BYTES_LIST_LE40
#define EXAMPLE_BYTES_LIST_LE48             BT_BYTES_LIST_LE48
#define EXAMPLE_BYTES_LIST_LE64             BT_BYTES_LIST_LE64

int example_audio_gap_event_cb(struct ble_gap_event *event, void *arg);

void example_audio_security_failed_recover(const char *tag, uint16_t conn_handle, uint8_t status);

void example_print_codec_cfg(const char *tag, const esp_ble_audio_codec_cfg_t *codec_cfg);

void example_print_codec_cap(const char *tag, const esp_ble_audio_codec_cap_t *codec_cap);

void example_print_meta(const char *tag, const uint8_t *meta, size_t meta_len);

void example_print_qos(const char *tag, const esp_ble_audio_bap_qos_cfg_t *qos);

void example_print_qos_pref(const char *tag, const esp_ble_audio_bap_qos_cfg_pref_t *pref);

bool example_is_substring(const char *substr, const char *str);

/**
 * @brief TX scheduler for periodic audio data transmission.
 *
 * Uses k_work_delayable to schedule send callbacks in the ISO task context,
 * ensuring thread safety without mutexes since both the timer handler and
 * BLE audio callbacks execute in the same task.
 */
typedef void (*example_audio_tx_send_cb_t)(void *ctx);

typedef struct {
    struct k_work_delayable timer;
    size_t drift;
    uint32_t count;
    /* Called periodically in ISO task context to transmit data */
    example_audio_tx_send_cb_t cb;
    void *arg;
} example_audio_tx_scheduler_t;

typedef struct {
    uint32_t recv_count;
    uint32_t valid_count;
    uint32_t error_count;
    uint32_t lost_count;
    uint32_t null_sdu_count;
    uint16_t last_sdu_len;
} example_audio_rx_metrics_t;

int example_audio_tx_scheduler_init(example_audio_tx_scheduler_t *scheduler,
                                    example_audio_tx_send_cb_t cb,
                                    void *arg);

void example_audio_tx_scheduler_reset(example_audio_tx_scheduler_t *scheduler);

int example_audio_tx_scheduler_start(example_audio_tx_scheduler_t *scheduler,
                                     uint64_t period_us);

int example_audio_tx_scheduler_stop(example_audio_tx_scheduler_t *scheduler);

void example_audio_tx_scheduler_on_sent(example_audio_tx_scheduler_t *scheduler,
                                        const esp_ble_iso_tx_cb_info_t *info,
                                        const char *tag,
                                        const char *obj_name);

void example_audio_rx_metrics_reset(example_audio_rx_metrics_t *metrics);

void example_audio_rx_metrics_on_recv(const esp_ble_iso_recv_info_t *info,
                                      example_audio_rx_metrics_t *metrics,
                                      const char *tag,
                                      const char *obj_name);

#endif /* BLE_AUDIO_EXAMPLE_UTILS_H_ */
