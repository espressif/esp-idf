/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPTimer clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
typedef soc_periph_gptimer_clk_src_t gptimer_clock_source_t;

/**
 * @brief GPTimer count direction
 */
typedef enum {
    GPTIMER_COUNT_DOWN, /*!< Decrease count value */
    GPTIMER_COUNT_UP,   /*!< Increase count value */
} gptimer_count_direction_t;

#ifdef __cplusplus
}
#endif
