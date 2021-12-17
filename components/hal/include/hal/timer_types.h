/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPTimer clock source
 * @note The clock source listed here is not supported on all targets
 * @note User should select the clock source based on real requirements:
 * @verbatim embed:rst:leading-asterisk
 * +----------------------+----------------------------------+--------------------------+
 * | GPTimer clock source | Features                         | Power Management         |
 * +======================+==================================+==========================+
 * | GPTIMER_CLK_SRC_APB  | High resolution                  | ESP_PM_APB_FREQ_MAX lock |
 * +----------------------+----------------------------------+--------------------------+
 * | GPTIMER_CLK_SRC_XTAL | Medium resolution, high accuracy | No PM lock               |
 * +----------------------+----------------------------------+--------------------------+
 * @endverbatim
 */
typedef enum {
    GPTIMER_CLK_SRC_APB,  /*!< Select APB as the source clock */
#if SOC_TIMER_GROUP_SUPPORT_XTAL
    GPTIMER_CLK_SRC_XTAL, /*!< Select XTAL as the source clock */
#endif
} gptimer_clock_source_t;

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
