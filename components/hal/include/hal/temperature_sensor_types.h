/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief temperature sensor clock source
 * @note User should select the clock source based on the real requirement:
 * @verbatim embed:rst:leading-asterisk
 * +------------------------------------+-------------------------+----------------------------+
 * | temperature sensor clock source    | Features                | Power Management           |
 * +====================================+=========================+============================+
 * | TEMPERATURE_SENSOR_CLK_SRC_XTAL    | external clock source   |          no lock           |
 * +------------------------------------+-------------------------+----------------------------+
 * | TEMPERATURE_SENSOR_CLK_SRC_FAST_RC | On board clock source   |          no lock           |
 * +------------------------------------+-------------------------+----------------------------+
 * @endverbatim
 */
typedef enum {
    TEMPERATURE_SENSOR_CLK_SRC_DEFAULT = 0,
#if SOC_TEMPERATURE_SENSOR_SUPPORT_XTAL
    TEMPERATURE_SENSOR_CLK_SRC_XTAL = 1,  /*!< Select XTAL as the source clock */
#endif
#if SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC
    TEMPERATURE_SENSOR_CLK_SRC_FAST_RC = 2,  /*!< Select FOSC as the source clock */
#endif
} temperature_sensor_clk_src_t;

#ifdef __cplusplus
}
#endif
