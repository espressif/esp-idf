/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_ANA_CMPR_UNIT               0     // Analog Comparator unit
#define EXAMPLE_WAIT_TIME_PROP              (0.1)               // The wait time proportion in one relative signal period
#define EXAMPLE_WAITE_TIME_US(freq_approx)  (uint32_t)(1000000 * EXAMPLE_WAIT_TIME_PROP / (freq_approx))

#if CONFIG_IDF_TARGET_ESP32P4
#define EXAMPLE_MONITOR_GPIO_NUM            (32)                // The gpio to monitor the on cross callback
#else
#define EXAMPLE_MONITOR_GPIO_NUM            (0)                 // The gpio to monitor the on cross callback
#endif

void example_init_monitor_gpio(void);

#if CONFIG_EXAMPLE_USE_ETM
/**
 * @brief   Set or clear the monitor GPIO via Event Task Matrix when cross interrupt triggers.
 * @note    The interrupt of analog comparator is regarded as Event,
 *          and the the operation of setting/clearing the GPIO is regarded as the corresponding task of the event.
*          CPU won't be involved by using Event Task Matrix, so it can achieve relatively higher interrupt frequency
 */
void example_analog_comparator_etm_app(void);
#endif

/**
 * @brief   Set or clear the monitor GPIO in the cross interrupt callback.
 * @note    The GPIO level is set manually in the callback.
 *          It's more flexible so that we can realize some operation like hysteresis,
 *          But as the operations are done in callback, CPU is involved, so it can only achieve a low interrupt frequency
 */
void example_analog_comparator_intr_app(void);

#ifdef __cplusplus
}
#endif
