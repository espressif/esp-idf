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
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

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

/* bt_le_addr.val carries the host stack's native byte order: NimBLE is
 * LSB-first (BT spec wire order), Bluedroid is MSB-first (BD_ADDR). Use
 * this with "%02x:%02x:%02x:%02x:%02x:%02x" to print MSB-first regardless
 * of host. Examples can otherwise pass val[] straight to their host APIs
 * without conversion. */
#if CONFIG_BT_BLUEDROID_ENABLED
#define EXAMPLE_BT_ADDR_PRINT_ARGS(_v) \
    (_v)[0], (_v)[1], (_v)[2], (_v)[3], (_v)[4], (_v)[5]

/* Fire-and-wait helper for Bluedroid GAP APIs that pair an async call with
 * a matching ESP_GAP_BLE_*_COMPLETE_EVT. The example's GAP event handler
 * is expected to xSemaphoreGive(_sem) on that COMPLETE_EVT. `TAG` and the
 * enclosing function's `esp_err_t` return are taken from the call site.
 *
 * !!! Demo-only — DO NOT copy into production code. !!!
 * Risks (acceptable in linear init sequences, not in customer apps):
 *   - Same-task deadlock: must NOT be called from the task that dispatches
 *     the COMPLETE_EVT (BTC task by default; lib forwarding can route some
 *     events through other tasks — verify the path for your event).
 *   - portMAX_DELAY = no timeout: hangs forever on controller/firmware
 *     misbehavior. Production must use a bounded timeout + recovery.
 *   - Shared binary sem: concurrent callers race; first take consumes
 *     whichever give arrived. Examples avoid this by serializing init.
 *   - No abort hook: nothing can break a portMAX_DELAY wait at shutdown.
 *   - Status not checked: see EXAMPLE_WAIT_API_CHECK below.
 *
 * Production should drive BLE init/teardown via an event-driven state
 * machine (one task owns the BLE flow, callbacks advance state, no
 * blocking from inside callbacks). NimBLE host returns ctrl errors
 * synchronously and sidesteps most of this complexity. */
#define EXAMPLE_WAIT_API(_call, _sem, _delay) do {                      \
        esp_err_t _err = (_call);                                       \
        if (_err != ESP_OK) {                                           \
            ESP_LOGE(TAG, #_call " failed, err %d", _err);              \
            return _err;                                                \
        }                                                               \
        xSemaphoreTake(_sem, _delay);                                   \
    } while (0)

/* As EXAMPLE_WAIT_API but also checks the controller status latched by the
 * gap_event_handler into `_op_status` after the sem is taken. Returns
 * ESP_FAIL on async failure. Caller must provide a static esp_bt_status_t
 * and write it from each COMPLETE_EVT param before xSemaphoreGive. */
#define EXAMPLE_WAIT_API_CHECK(_call, _sem, _delay, _op_status) do {    \
        EXAMPLE_WAIT_API(_call, _sem, _delay);                          \
        if ((_op_status) != ESP_BT_STATUS_SUCCESS) {                    \
            ESP_LOGE(TAG, #_call " ctrl status %d", (_op_status));      \
            return ESP_FAIL;                                            \
        }                                                               \
    } while (0)
#else
#define EXAMPLE_BT_ADDR_PRINT_ARGS(_v) \
    (_v)[5], (_v)[4], (_v)[3], (_v)[2], (_v)[1], (_v)[0]
#endif

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
