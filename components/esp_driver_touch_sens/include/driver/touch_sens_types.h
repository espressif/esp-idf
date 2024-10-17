/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "soc/soc_caps.h"
#include "hal/touch_sens_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOUCH_TOTAL_CHAN_NUM        SOC_TOUCH_SENSOR_NUM        /*!< The total channel number of the touch sensor */
#define TOUCH_SAMPLE_CFG_NUM        SOC_TOUCH_SAMPLE_CFG_NUM    /*!< The supported max sample configuration number */
#if SOC_TOUCH_SUPPORT_PROX_SENSING
#define TOUCH_PROXIMITY_CHAN_NUM    SOC_TOUCH_PROXIMITY_CHANNEL_NUM     /*!< The supported proximity channel number in proximity sensing mode */
#endif

/**
 * @brief The chip sleep level that allows the touch sensor to wake-up
 *
 */
typedef enum {
    TOUCH_LIGHT_SLEEP_WAKEUP,                /*!< Only enable the touch sensor to wake up the chip from light sleep */
    TOUCH_DEEP_SLEEP_WAKEUP,                 /*!< Enable the touch sensor to wake up the chip from deep sleep or light sleep */
} touch_sleep_wakeup_level_t;

/**
 * @brief Touch sensor shield channel drive capability level
 *
 */
typedef enum {
    TOUCH_SHIELD_CAP_40PF,                  /*!< The max equivalent capacitance in shield channel is 40pf */
    TOUCH_SHIELD_CAP_80PF,                  /*!< The max equivalent capacitance in shield channel is 80pf */
    TOUCH_SHIELD_CAP_120PF,                 /*!< The max equivalent capacitance in shield channel is 120pf */
    TOUCH_SHIELD_CAP_160PF,                 /*!< The max equivalent capacitance in shield channel is 160pf */
    TOUCH_SHIELD_CAP_200PF,                 /*!< The max equivalent capacitance in shield channel is 200pf */
    TOUCH_SHIELD_CAP_240PF,                 /*!< The max equivalent capacitance in shield channel is 240pf */
    TOUCH_SHIELD_CAP_280PF,                 /*!< The max equivalent capacitance in shield channel is 280pf */
    TOUCH_SHIELD_CAP_320PF,                 /*!< The max equivalent capacitance in shield channel is 320pf */
} touch_chan_shield_cap_t;

/**
 * @brief Touch channel Infinite Impulse Response (IIR) filter or Jitter filter for benchmark
 * @note Recommended filter coefficient selection is `IIR_16`.
 */
typedef enum {
    TOUCH_BM_IIR_FILTER_4,                  /*!< IIR Filter for benchmark, 1/4 raw_value + 3/4 benchmark */
    TOUCH_BM_IIR_FILTER_8,                  /*!< IIR Filter for benchmark, 1/8 raw_value + 7/8 benchmark */
    TOUCH_BM_IIR_FILTER_16,                 /*!< IIR Filter for benchmark, 1/16 raw_value + 15/16 benchmark (typical) */
    TOUCH_BM_IIR_FILTER_32,                 /*!< IIR Filter for benchmark, 1/32 raw_value + 31/32 benchmark */
    TOUCH_BM_IIR_FILTER_64,                 /*!< IIR Filter for benchmark, 1/64 raw_value + 63/64 benchmark */
    TOUCH_BM_IIR_FILTER_128,                /*!< IIR Filter for benchmark, 1/128 raw_value + 127/128 benchmark */
#if SOC_TOUCH_SENSOR_VERSION == 2
    TOUCH_BM_IIR_FILTER_256,                /*!< IIR Filter for benchmark, 1/256 raw_value + 255/256 benchmark */
#endif
    TOUCH_BM_JITTER_FILTER,                 /*!< Jitter Filter for benchmark, raw value +/- jitter_step */
} touch_benchmark_filter_mode_t;

/**
 * @brief Touch channel Infinite Impulse Response (IIR) filter for smooth data
 *
 */
typedef enum {
    TOUCH_SMOOTH_NO_FILTER,                 /*!< No filter adopted for smooth data, smooth data equals raw data */
    TOUCH_SMOOTH_IIR_FILTER_2,              /*!< IIR filter adopted for smooth data, smooth data equals 1/2 raw data + 1/2 last smooth data (typical) */
    TOUCH_SMOOTH_IIR_FILTER_4,              /*!< IIR filter adopted for smooth data, smooth data equals 1/4 raw data + 3/4 last smooth data */
    TOUCH_SMOOTH_IIR_FILTER_8,              /*!< IIR filter adopted for smooth data, smooth data equals 1/8 raw data + 7/8 last smooth data */
} touch_smooth_filter_mode_t;

/**
 * @brief Interrupt events
 *
 */
typedef enum {
    TOUCH_INTR_EVENT_ACTIVE,                /*!< Touch channel active event */
    TOUCH_INTR_EVENT_INACTIVE,              /*!< Touch channel inactive event */
    TOUCH_INTR_EVENT_MEASURE_DONE,          /*!< Touch channel measure done event */
    TOUCH_INTR_EVENT_SCAN_DONE,             /*!< All touch channels scan done event */
    TOUCH_INTR_EVENT_TIMEOUT,               /*!< Touch channel measurement timeout event */
    TOUCH_INTR_EVENT_PROXIMITY_DONE,        /*!< Proximity channel measurement done event */
} touch_intr_event_t;

typedef struct touch_sensor_s       *touch_sensor_handle_t;     /*!< The handle of touch sensor controller */
typedef struct touch_channel_s      *touch_channel_handle_t;    /*!< The handle of touch channel */

#ifdef __cplusplus
}
#endif
