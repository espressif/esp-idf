/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RMT group clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
typedef enum {
    RMT_CLK_SRC_NONE,     /*!< No clock source is selected */
    RMT_CLK_SRC_REFTICK,  /*!< Select REF_TICK as the source clock */
    RMT_CLK_SRC_APB,      /*!< Select APB as the source clock */
    RMT_CLK_SRC_FAST_RC,  /*!< Select internal fast RC oscillator as the source clock */
    RMT_CLK_SRC_XTAL,     /*!< Select XTAL as the source clock */
} rmt_clock_source_t;

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
