/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RMT group clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
#if SOC_RMT_SUPPORTED
typedef soc_periph_rmt_clk_src_t rmt_clock_source_t;
#else
typedef int rmt_clock_source_t;
#endif

/**
 * @brief The layout of RMT symbol stored in memory, which is decided by the hardware design
 */
typedef union {
    struct {
        uint16_t duration0 : 15; /*!< Duration of level0 */
        uint16_t level0 : 1;     /*!< Level of the first part */
        uint16_t duration1 : 15; /*!< Duration of level1 */
        uint16_t level1 : 1;     /*!< Level of the second part */
    };
    uint32_t val; /*!< Equivalent unsigned value for the RMT symbol */
} rmt_symbol_word_t;

#ifdef __cplusplus
}
#endif
