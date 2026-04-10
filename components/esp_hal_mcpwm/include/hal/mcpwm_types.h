/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MCPWM timer clock source
 */
#if SOC_MCPWM_SUPPORTED
typedef soc_periph_mcpwm_timer_clk_src_t mcpwm_timer_clock_source_t;
#else
typedef int mcpwm_timer_clock_source_t;
#endif // SOC_MCPWM_SUPPORTED

/**
 * @brief MCPWM capture clock source
 */
#if SOC_MCPWM_SUPPORTED
typedef soc_periph_mcpwm_capture_clk_src_t mcpwm_capture_clock_source_t;
#else
typedef int mcpwm_capture_clock_source_t;
#endif // SOC_MCPWM_SUPPORTED

/**
 * @brief MCPWM carrier clock source
 */
#if SOC_MCPWM_SUPPORTED
typedef soc_periph_mcpwm_carrier_clk_src_t mcpwm_carrier_clock_source_t;
#else
typedef int mcpwm_carrier_clock_source_t;
#endif // SOC_MCPWM_SUPPORTED

/**
 * @brief MCPWM timer count direction
 */
typedef enum {
    MCPWM_TIMER_DIRECTION_UP,   /*!< Counting direction: Increase */
    MCPWM_TIMER_DIRECTION_DOWN, /*!< Counting direction: Decrease */
} mcpwm_timer_direction_t;

/**
 * @brief MCPWM timer events
 */
typedef enum {
    MCPWM_TIMER_EVENT_EMPTY,   /*!< MCPWM timer counts to zero (i.e. counter is empty) */
    MCPWM_TIMER_EVENT_FULL,    /*!< MCPWM timer counts to peak (i.e. counter is full) */
    MCPWM_TIMER_EVENT_INVALID, /*!< MCPWM timer invalid event */
} mcpwm_timer_event_t;

/**
 * @brief MCPWM timer count modes
 */
typedef enum {
    MCPWM_TIMER_COUNT_MODE_PAUSE,   /*!< MCPWM timer paused */
    MCPWM_TIMER_COUNT_MODE_UP,      /*!< MCPWM timer counting up */
    MCPWM_TIMER_COUNT_MODE_DOWN,    /*!< MCPWM timer counting down */
    MCPWM_TIMER_COUNT_MODE_UP_DOWN, /*!< MCPWM timer counting up and down */
} mcpwm_timer_count_mode_t;

/**
 * @brief MCPWM timer commands, specify the way to start or stop the timer
 */
typedef enum {
    MCPWM_TIMER_STOP_EMPTY,       /*!< MCPWM timer stops when next count reaches zero */
    MCPWM_TIMER_STOP_FULL,        /*!< MCPWM timer stops when next count reaches peak */
    MCPWM_TIMER_START_NO_STOP,    /*!< MCPWM timer starts counting, and don't stop until received stop command */
    MCPWM_TIMER_START_STOP_EMPTY, /*!< MCPWM timer starts counting and stops when next count reaches zero */
    MCPWM_TIMER_START_STOP_FULL,  /*!< MCPWM timer starts counting and stops when next count reaches peak */
} mcpwm_timer_start_stop_cmd_t;

/**
 * @brief MCPWM generator actions
 */
typedef enum {
    MCPWM_GEN_ACTION_KEEP,   /*!< Generator action: Keep the same level */
    MCPWM_GEN_ACTION_LOW,    /*!< Generator action: Force to low level */
    MCPWM_GEN_ACTION_HIGH,   /*!< Generator action: Force to high level */
    MCPWM_GEN_ACTION_TOGGLE, /*!< Generator action: Toggle level */
} mcpwm_generator_action_t;

/**
 * @brief MCPWM operator brake mode
 */
typedef enum {
    MCPWM_OPER_BRAKE_MODE_CBC,     /*!< Brake mode: CBC (cycle by cycle)*/
    MCPWM_OPER_BRAKE_MODE_OST,     /*!< Brake mode: OST (one shot) */
    MCPWM_OPER_BRAKE_MODE_INVALID, /*!< MCPWM operator invalid brake mode */
} mcpwm_operator_brake_mode_t;

/**
 * @brief MCPWM capture edge
 */
typedef enum {
    MCPWM_CAP_EDGE_POS, /*!< Capture on the positive edge */
    MCPWM_CAP_EDGE_NEG, /*!< Capture on the negative edge */
} mcpwm_capture_edge_t;

/**
 * @brief MCPWM timer specific events that supported by the ETM module
 */
typedef enum {
    MCPWM_TIMER_ETM_EVENT_TEZ,  /*!< The timer reaches zero */
    MCPWM_TIMER_ETM_EVENT_TEP,  /*!< The timer reaches peak */
    MCPWM_TIMER_ETM_EVENT_MAX,  /*!< Maximum number of timer events */
} mcpwm_timer_etm_event_type_t;

/**
 * @brief MCPWM comparator specific events that supported by the ETM module
 */
typedef enum {
    MCPWM_CMPR_ETM_EVENT_EQUAL, /*!< The count value equals the value of comparator */
    MCPWM_CMPR_ETM_EVENT_MAX,   /*!< Maximum number of comparator events */
} mcpwm_comparator_etm_event_type_t;

#ifdef __cplusplus
}
#endif
