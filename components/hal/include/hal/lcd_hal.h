/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

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

#define LCD_HAL_PCLK_FLAG_ALLOW_EQUAL_SYSCLK (1 << 0)

/**
 * @brief LCD PCLK clock calculation
 * @note Currently this function is only used by RGB LCD driver, I80 driver still uses a fixed clock division
 *
 * @param hal LCD HAL layer context
 * @param src_freq_hz LCD source clock frequency in Hz
 * @param expect_pclk_freq_hz Expected LCD PCLK frequency in Hz
 * @param lcd_clk_flags Extra flags to control LCD PCLK clock calculation, supported flags are prefixed with LCD_HAL_PCLK_FLAG_
 * @return Actual LCD PCLK frequency in Hz
 */
uint32_t lcd_hal_cal_pclk_freq(lcd_hal_context_t *hal, uint32_t src_freq_hz, uint32_t expect_pclk_freq_hz, int lcd_clk_flags);

#ifdef __cplusplus
}
#endif
