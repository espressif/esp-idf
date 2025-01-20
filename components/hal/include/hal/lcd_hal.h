/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/hal_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD peripheral SOC layer handle
 */
typedef struct lcd_cam_dev_t *lcd_soc_handle_t;

/**
 * @brief LCD HAL layer context
 */
typedef struct {
    lcd_soc_handle_t dev; // SOC layer handle
} lcd_hal_context_t;

/**
 * @brief LCD HAL layer initialization
 *
 * @param hal LCD HAL layer context
 * @param id LCD peripheral ID
 */
void lcd_hal_init(lcd_hal_context_t *hal, int id);

/**
 * @brief LCD PCLK clock calculation
 * @note Currently this function is only used by RGB LCD driver, I80 driver still uses a fixed clock division
 *
 * @param hal LCD HAL layer context
 * @param src_freq_hz LCD source clock frequency in Hz
 * @param expect_pclk_freq_hz Expected LCD PCLK frequency in Hz
 * @param lcd_clk_div Returned LCD clock divider parameter
 * @return Actual LCD PCLK frequency in Hz
 */
uint32_t lcd_hal_cal_pclk_freq(lcd_hal_context_t *hal, uint32_t src_freq_hz, uint32_t expect_pclk_freq_hz, hal_utils_clk_div_t* lcd_clk_div);

#ifdef __cplusplus
}
#endif
