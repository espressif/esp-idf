/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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

#define TOUCH_SAMPLE_CFG_NUM        SOC_TOUCH_SAMPLE_CFG_NUM            /*!< The supported max sample configuration number */
#if SOC_TOUCH_SUPPORT_PROX_SENSING
#define TOUCH_PROXIMITY_CHAN_NUM    SOC_TOUCH_PROXIMITY_CHANNEL_NUM     /*!< The supported proximity channel number in proximity sensing mode */
#endif

#define TOUCH_MIN_CHAN_ID           SOC_TOUCH_MIN_CHAN_ID               /*!< The minimum available channel id of the touch pad */
#define TOUCH_MAX_CHAN_ID           SOC_TOUCH_MAX_CHAN_ID               /*!< The maximum available channel id of the touch pad */
#define TOUCH_TOTAL_CHAN_NUM        (TOUCH_MAX_CHAN_ID - TOUCH_MIN_CHAN_ID + 1) /*!< The total channel number of the touch sensor */

/**
 * @brief The chip sleep level that allows the touch sensor to wake-up
 *
 */
typedef enum {
    TOUCH_LIGHT_SLEEP_WAKEUP,                /*!< Only enable the touch sensor to wake up the chip from light sleep */
    TOUCH_DEEP_SLEEP_WAKEUP,                 /*!< Enable the touch sensor to wake up the chip from deep sleep or light sleep */
} touch_sleep_wakeup_level_t;

typedef struct touch_sensor_s       *touch_sensor_handle_t;             /*!< The handle of touch sensor controller */
typedef struct touch_channel_s      *touch_channel_handle_t;            /*!< The handle of touch channel */

#ifdef __cplusplus
}
#endif
