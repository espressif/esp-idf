/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of MCPWM timer handle
 */
typedef struct mcpwm_timer_t *mcpwm_timer_handle_t;

/**
 * @brief Type of MCPWM operator handle
 */
typedef struct mcpwm_oper_t *mcpwm_oper_handle_t;

/**
 * @brief Type of MCPWM comparator handle
 */
typedef struct mcpwm_cmpr_t *mcpwm_cmpr_handle_t;

/**
 * @brief Type of MCPWM generator handle
 */
typedef struct mcpwm_gen_t *mcpwm_gen_handle_t;

/**
 * @brief Type of MCPWM fault handle
 */
typedef struct mcpwm_fault_t *mcpwm_fault_handle_t;

/**
 * @brief Type of MCPWM sync handle
 */
typedef struct mcpwm_sync_t *mcpwm_sync_handle_t;

/**
 * @brief Type of MCPWM capture timer handle
 */
typedef struct mcpwm_cap_timer_t *mcpwm_cap_timer_handle_t;

/**
 * @brief Type of MCPWM capture channel handle
 */
typedef struct mcpwm_cap_channel_t *mcpwm_cap_channel_handle_t;

/**
 * @brief MCPWM timer event data
 */
typedef struct {
    uint32_t count_value;              /*!< MCPWM timer count value */
    mcpwm_timer_direction_t direction; /*!< MCPWM timer count direction */
} mcpwm_timer_event_data_t;

/**
 * @brief MCPWM timer event callback function
 *
 * @param[in] timer MCPWM timer handle
 * @param[in] edata MCPWM timer event data, fed by driver
 * @param[in] user_ctx User data, set in `mcpwm_timer_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*mcpwm_timer_event_cb_t)(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_ctx);

/**
 * @brief MCPWM brake event data
 */
typedef struct {
} mcpwm_brake_event_data_t;

/**
 * @brief MCPWM operator brake event callback function
 *
 * @param[in] oper MCPWM operator handle
 * @param[in] edata MCPWM brake event data, fed by driver
 * @param[in] user_ctx User data, set in `mcpwm_operator_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*mcpwm_brake_event_cb_t)(mcpwm_oper_handle_t oper, const mcpwm_brake_event_data_t *edata, void *user_ctx);

/**
 * @brief MCPWM fault event data
 */
typedef struct {
} mcpwm_fault_event_data_t;

/**
 * @brief MCPWM fault event callback function
 *
 * @param fault MCPWM fault handle
 * @param edata MCPWM fault event data, fed by driver
 * @param user_ctx User data, set in `mcpwm_fault_register_event_callbacks()`
 * @return whether a task switch is needed after the callback returns
 */
typedef bool (*mcpwm_fault_event_cb_t)(mcpwm_fault_handle_t fault, const mcpwm_fault_event_data_t *edata, void *user_ctx);

/**
 * @brief MCPWM compare event data
 */
typedef struct {
    uint32_t compare_ticks;            /*!< Compare value */
    mcpwm_timer_direction_t direction; /*!< Count direction */
} mcpwm_compare_event_data_t;

/**
 * @brief MCPWM comparator event callback function
 *
 * @param comparator MCPWM comparator handle
 * @param edata MCPWM comparator event data, fed by driver
 * @param user_ctx User data, set in `mcpwm_comparator_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*mcpwm_compare_event_cb_t)(mcpwm_cmpr_handle_t comparator, const mcpwm_compare_event_data_t *edata, void *user_ctx);

/**
 * @brief MCPWM capture event data
 */
typedef struct {
    uint32_t cap_value;            /*!< Captured value */
    mcpwm_capture_edge_t cap_edge; /*!< Capture edge */
} mcpwm_capture_event_data_t;

/**
 * @brief MCPWM capture event callback function
 *
 * @param cap_channel MCPWM capture channel handle
 * @param edata MCPWM capture event data, fed by driver
 * @param user_ctx User data, set in `mcpwm_capture_channel_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*mcpwm_capture_event_cb_t)(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_ctx);

#ifdef __cplusplus
}
#endif
