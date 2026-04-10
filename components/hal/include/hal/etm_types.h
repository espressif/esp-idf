/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ETM_SUPPORTED
/**
 * @brief ETM clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
typedef soc_periph_etm_clk_src_t etm_clock_source_t;
#else
/**
 * @brief Default type
 */
typedef int                      etm_clock_source_t;
#endif

#ifdef __cplusplus
}
#endif
