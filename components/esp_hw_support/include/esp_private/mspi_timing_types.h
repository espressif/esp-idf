/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 * This file is for MSPI timinig tuning private APIs
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MSPI Timing speed mode
 */
typedef enum {
    MSPI_TIMING_SPEED_MODE_LOW_PERF,    /*!< Low performance speed mode, this mode is safe for all the scenarios,
                                             unless the MSPI attached devices (Flash, PSRAM) are powered down.
                                             As a tradeoff, the performance of the MSPI (devices) are swithed to a very low speed */
    MSPI_TIMING_SPEED_MODE_NORMAL_PERF, /*!< Normal performance speed mode, MSPI speed is the same as you configured in menuconfig */
} mspi_timing_speed_mode_t;

#ifdef __cplusplus
}
#endif
