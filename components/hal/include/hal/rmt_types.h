/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RMT group clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
typedef soc_periph_rmt_clk_src_t rmt_clock_source_t;

/**
 * @brief The layout of RMT symbol stored in memory, which is decided by the hardware design
 */
typedef union {
    struct {
        unsigned int duration0 : 15; /*!< Duration of level0 */
        unsigned int level0 : 1;     /*!< Level of the first part */
        unsigned int duration1 : 15; /*!< Duration of level1 */
        unsigned int level1 : 1;     /*!< Level of the second part */
    };
    unsigned int val; /*!< Equivelent unsigned value for the RMT symbol */
} rmt_symbol_word_t;

#ifdef __cplusplus
}
#endif
