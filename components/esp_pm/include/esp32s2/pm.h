/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Power management config for ESP32
 *
 * Pass a pointer to this structure as an argument to esp_pm_configure function.
 */
typedef struct {
    int max_freq_mhz;   /*!< Maximum CPU frequency, in MHz */
    int min_freq_mhz;   /*!< Minimum CPU frequency to use when no locks are taken, in MHz */
    bool light_sleep_enable;        /*!< Enter light sleep when no locks are taken */
} esp_pm_config_esp32s2_t;


#ifdef __cplusplus
}
#endif
