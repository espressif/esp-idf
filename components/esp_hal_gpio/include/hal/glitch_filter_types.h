/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER || (SOC_GPIO_FLEX_GLITCH_FILTER_NUM > 0)
typedef soc_periph_glitch_filter_clk_src_t glitch_filter_clock_source_t; // glitch filter clock source
#else
typedef int glitch_filter_clock_source_t; // glitch filter clock source, fallback to integer type
#endif // SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER || (SOC_GPIO_FLEX_GLITCH_FILTER_NUM > 0)

#ifdef __cplusplus
}
#endif
