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
#include "hal/touch_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOUCH_TOTAL_CHAN_NUM        SOC_TOUCH_SENSOR_NUM        /*!< The total channel number of the touch sensor */
#define TOUCH_SAMPLE_CFG_NUM        SOC_TOUCH_SAMPLE_CFG_NUM    /*!< The supported max sample configuration number */
#if SOC_TOUCH_SUPPORT_PROX_SENSING
#define TOUCH_PROXIMITY_CHAN_NUM    SOC_TOUCH_PROXIMITY_CHANNEL_NUM     /*!< The supported proximity channel number in proximity sensing mode */
#endif

typedef struct touch_sensor_s       *touch_sensor_handle_t;     /*!< The handle of touch sensor controller */
typedef struct touch_channel_s      *touch_channel_handle_t;    /*!< The handle of touch channel */

#ifdef __cplusplus
}
#endif
