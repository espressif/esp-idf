/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief temperature sensor clock source
 */
#if SOC_TEMP_SENSOR_SUPPORTED
typedef soc_periph_temperature_sensor_clk_src_t temperature_sensor_clk_src_t;
#else
typedef int temperature_sensor_clk_src_t;
#endif // SOC_TEMP_SENSOR_SUPPORTED

#ifdef __cplusplus
}
#endif
