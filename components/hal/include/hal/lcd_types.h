/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCD_I80_SUPPORTED || SOC_LCD_RGB_SUPPORTED
/**
 * @brief LCD clock source
 */
typedef soc_periph_lcd_clk_src_t lcd_clock_source_t;
#endif

#ifdef __cplusplus
}
#endif
