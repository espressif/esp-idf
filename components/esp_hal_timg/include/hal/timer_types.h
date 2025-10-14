/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GPTIMER_SUPPORTED
/**
 * @brief GPTimer clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
typedef soc_periph_gptimer_clk_src_t gptimer_clock_source_t;
#else
/**
 * @brief Default type
 */
typedef int                          gptimer_clock_source_t;
#endif

/**
 * @brief GPTimer count direction
 */
typedef enum {
    GPTIMER_COUNT_DOWN, /*!< Decrease count value */
    GPTIMER_COUNT_UP,   /*!< Increase count value */
} gptimer_count_direction_t;

#if SOC_TIMER_SUPPORT_ETM
/**
 * @brief GPTimer specific tasks that supported by the ETM module
 */
typedef enum {
    GPTIMER_ETM_TASK_START_COUNT, /*!< Start the counter */
    GPTIMER_ETM_TASK_STOP_COUNT,  /*!< Stop the counter */
    GPTIMER_ETM_TASK_EN_ALARM,    /*!< Enable the alarm */
    GPTIMER_ETM_TASK_RELOAD,      /*!< Reload preset value into counter */
    GPTIMER_ETM_TASK_CAPTURE,     /*!< Capture current count value into specific register */
    GPTIMER_ETM_TASK_MAX,         /*!< Maximum number of tasks */
} gptimer_etm_task_type_t;

/**
 * @brief GPTimer specific events that supported by the ETM module
 */
typedef enum {
    GPTIMER_ETM_EVENT_ALARM_MATCH, /*!< Count value matches the alarm target value */
    GPTIMER_ETM_EVENT_MAX,         /*!< Maximum number of events */
} gptimer_etm_event_type_t;
#endif // SOC_TIMER_SUPPORT_ETM

#ifdef __cplusplus
}
#endif
