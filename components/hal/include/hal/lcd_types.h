/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD clock source
 * @note User should select the clock source based on the real requirement:
 * @verbatim embed:rst:leading-asterisk
 * +---------------------+-------------------------+----------------------------+
 * | LCD clock source    | Features                | Power Management           |
 * +=====================+=========================+============================+
 * | LCD_CLK_SRC_PLL160M | High resolution         | ESP_PM_APB_FREQ_MAX lock   |
 * +---------------------+-------------------------+----------------------------+
 * | LCD_CLK_SRC_APLL    | Configurable resolution | ESP_PM_NO_LIGHT_SLEEP lock |
 * +---------------------+-------------------------+----------------------------+
 * | LCD_CLK_SRC_XTAL    | Medium resolution       | No PM lock                 |
 * +---------------------+-------------------------+----------------------------+
 * @endverbatim
 */
typedef enum {
    LCD_CLK_SRC_PLL160M, /*!< Select PLL160M as the source clock */
    LCD_CLK_SRC_APLL,    /*!< Select APLL as the source clock */
    LCD_CLK_SRC_XTAL,    /*!< Select XTAL as the source clock */
} lcd_clock_source_t;

#ifdef __cplusplus
}
#endif
