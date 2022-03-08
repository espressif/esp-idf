/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TSENS_DAC_L0 = 0, /*!< offset = -2, measure range: 50℃ ~ 125℃, error < 3℃. */
    TSENS_DAC_L1,     /*!< offset = -1, measure range: 20℃ ~ 100℃, error < 2℃. */
    TSENS_DAC_L2,     /*!< offset =  0, measure range:-10℃ ~  80℃, error < 1℃. */
    TSENS_DAC_L3,     /*!< offset =  1, measure range:-30℃ ~  50℃, error < 2℃. */
    TSENS_DAC_L4,     /*!< offset =  2, measure range:-40℃ ~  20℃, error < 3℃. */
    TSENS_DAC_MAX,
    TSENS_DAC_DEFAULT = TSENS_DAC_L2,
} temp_sensor_dac_offset_t;


/**
 * @brief Configuration for temperature sensor reading
 */
typedef struct {
    temp_sensor_dac_offset_t dac_offset;    /*!< The temperature measurement range is configured with a built-in temperature offset DAC. */
    uint8_t clk_div;                        /*!< Default: 6 */
} temp_sensor_config_t;

#define TSENS_CONFIG_DEFAULT() {.dac_offset = TSENS_DAC_L2, \
                                .clk_div = 6}

#ifdef __cplusplus
}
#endif
