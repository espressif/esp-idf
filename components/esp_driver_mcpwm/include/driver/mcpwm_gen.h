/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "esp_err.h"
#include "driver/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MCPWM generator configuration
 */
typedef struct {
    int gen_gpio_num;           /*!< The GPIO number used to output the PWM signal */
    struct {
        uint32_t invert_pwm: 1;   /*!< Whether to invert the PWM signal (done by GPIO matrix) */
        uint32_t io_loop_back: 1; /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
        uint32_t io_od_mode: 1;   /*!< Configure the GPIO as open-drain mode */
        uint32_t pull_up: 1;      /*!< Whether to pull up internally */
        uint32_t pull_down: 1;    /*!< Whether to pull down internally */
    } flags;                      /*!< Extra configuration flags for generator */
} mcpwm_generator_config_t;

/**
 * @brief Allocate MCPWM generator from given operator
 *
 * @param[in] oper MCPWM operator, allocated by `mcpwm_new_operator()`
 * @param[in] config MCPWM generator configuration
 * @param[out] ret_gen Returned MCPWM generator
 * @return
 *      - ESP_OK: Create MCPWM generator successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM generator failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM generator failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM generator failed because can't find free resource
 *      - ESP_FAIL: Create MCPWM generator failed because of other error
 */
esp_err_t mcpwm_new_generator(mcpwm_oper_handle_t oper, const mcpwm_generator_config_t *config, mcpwm_gen_handle_t *ret_gen);

/**
 * @brief Delete MCPWM generator
 *
 * @param[in] gen MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @return
 *      - ESP_OK: Delete MCPWM generator successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM generator failed because of invalid argument
 *      - ESP_FAIL: Delete MCPWM generator failed because of other error
 */
esp_err_t mcpwm_del_generator(mcpwm_gen_handle_t gen);

/**
 * @brief Set force level for MCPWM generator
 *
 * @note The force level will be applied to the generator immediately, regardless any other events that would change the generator's behaviour.
 * @note If the `hold_on` is true, the force level will retain forever, until user removes the force level by setting the force level to `-1`.
 * @note If the `hold_on` is false, the force level can be overridden by the next event action.
 * @note The force level set by this function can be inverted by GPIO matrix or dead-time module. So the level set here doesn't equal to the final output level.
 *
 * @param[in] gen MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] level GPIO level to be applied to MCPWM generator, specially, -1 means to remove the force level
 * @param[in] hold_on Whether the forced PWM level should retain (i.e. will remain unchanged until manually remove the force level)
 * @return
 *      - ESP_OK: Set force level for MCPWM generator successfully
 *      - ESP_ERR_INVALID_ARG: Set force level for MCPWM generator failed because of invalid argument
 *      - ESP_FAIL: Set force level for MCPWM generator failed because of other error
 */
esp_err_t mcpwm_generator_set_force_level(mcpwm_gen_handle_t gen, int level, bool hold_on);

/**
 * @brief Generator action on specific timer event
 */
typedef struct {
    mcpwm_timer_direction_t direction; /*!< Timer direction */
    mcpwm_timer_event_t event;         /*!< Timer event */
    mcpwm_generator_action_t action;   /*!< Generator action should perform */
} mcpwm_gen_timer_event_action_t;

/**
 * @brief Help macros to construct a mcpwm_gen_timer_event_action_t entry
 */
#define MCPWM_GEN_TIMER_EVENT_ACTION(dir, ev, act) \
    (mcpwm_gen_timer_event_action_t) { .direction = dir, .event = ev, .action = act }
#define MCPWM_GEN_TIMER_EVENT_ACTION_END() \
    (mcpwm_gen_timer_event_action_t) { .event = MCPWM_TIMER_EVENT_INVALID }

/**
 * @brief Set generator action on MCPWM timer event
 *
 * @param[in] gen MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM timer event action, can be constructed by `MCPWM_GEN_TIMER_EVENT_ACTION` helper macro
 * @return
 *      - ESP_OK: Set generator action successfully
 *      - ESP_ERR_INVALID_ARG: Set generator action failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set generator action failed because of timer is not connected to operator
 *      - ESP_FAIL: Set generator action failed because of other error
 */
esp_err_t mcpwm_generator_set_action_on_timer_event(mcpwm_gen_handle_t gen, mcpwm_gen_timer_event_action_t ev_act);

/**
 * @brief Set generator actions on multiple MCPWM timer events
 *
 * @note This is an aggregation version of `mcpwm_generator_set_action_on_timer_event`, which allows user to set multiple actions in one call.
 *
 * @param[in] gen MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM timer event action list, must be terminated by `MCPWM_GEN_TIMER_EVENT_ACTION_END()`
 * @return
 *      - ESP_OK: Set generator actions successfully
 *      - ESP_ERR_INVALID_ARG: Set generator actions failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set generator actions failed because of timer is not connected to operator
 *      - ESP_FAIL: Set generator actions failed because of other error
 */
esp_err_t mcpwm_generator_set_actions_on_timer_event(mcpwm_gen_handle_t gen, mcpwm_gen_timer_event_action_t ev_act, ...);

/**
 * @brief Generator action on specific comparator event
 */
typedef struct {
    mcpwm_timer_direction_t direction; /*!< Timer direction */
    mcpwm_cmpr_handle_t comparator;    /*!< Comparator handle */
    mcpwm_generator_action_t action;   /*!< Generator action should perform */
} mcpwm_gen_compare_event_action_t;

/**
 * @brief Help macros to construct a mcpwm_gen_compare_event_action_t entry
 */
#define MCPWM_GEN_COMPARE_EVENT_ACTION(dir, cmp, act) \
    (mcpwm_gen_compare_event_action_t) { .direction = dir, .comparator = cmp, .action = act }
#define MCPWM_GEN_COMPARE_EVENT_ACTION_END() \
    (mcpwm_gen_compare_event_action_t) { .comparator = NULL }

/**
 * @brief Set generator action on MCPWM compare event
 *
 * @param[in] generator MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM compare event action, can be constructed by `MCPWM_GEN_COMPARE_EVENT_ACTION` helper macro
 * @return
 *      - ESP_OK: Set generator action successfully
 *      - ESP_ERR_INVALID_ARG: Set generator action failed because of invalid argument
 *      - ESP_FAIL: Set generator action failed because of other error
 */
esp_err_t mcpwm_generator_set_action_on_compare_event(mcpwm_gen_handle_t generator, mcpwm_gen_compare_event_action_t ev_act);

/**
 * @brief Set generator actions on multiple MCPWM compare events
 *
 * @note This is an aggregation version of `mcpwm_generator_set_action_on_compare_event`, which allows user to set multiple actions in one call.
 *
 * @param[in] generator MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM compare event action list, must be terminated by `MCPWM_GEN_COMPARE_EVENT_ACTION_END()`
 * @return
 *      - ESP_OK: Set generator actions successfully
 *      - ESP_ERR_INVALID_ARG: Set generator actions failed because of invalid argument
 *      - ESP_FAIL: Set generator actions failed because of other error
 */
esp_err_t mcpwm_generator_set_actions_on_compare_event(mcpwm_gen_handle_t generator, mcpwm_gen_compare_event_action_t ev_act, ...);

/**
 * @brief Generator action on specific brake event
 */
typedef struct {
    mcpwm_timer_direction_t direction;       /*!< Timer direction */
    mcpwm_operator_brake_mode_t brake_mode;  /*!< Brake mode */
    mcpwm_generator_action_t action;         /*!< Generator action should perform */
} mcpwm_gen_brake_event_action_t;

/**
 * @brief Help macros to construct a mcpwm_gen_brake_event_action_t entry
 */
#define MCPWM_GEN_BRAKE_EVENT_ACTION(dir, mode, act) \
    (mcpwm_gen_brake_event_action_t) { .direction = dir, .brake_mode = mode, .action = act }
#define MCPWM_GEN_BRAKE_EVENT_ACTION_END() \
    (mcpwm_gen_brake_event_action_t) { .brake_mode = MCPWM_OPER_BRAKE_MODE_INVALID }

/**
 * @brief Set generator action on MCPWM brake event
 *
 * @param[in] generator MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM brake event action, can be constructed by `MCPWM_GEN_BRAKE_EVENT_ACTION` helper macro
 * @return
 *      - ESP_OK: Set generator action successfully
 *      - ESP_ERR_INVALID_ARG: Set generator action failed because of invalid argument
 *      - ESP_FAIL: Set generator action failed because of other error
 */
esp_err_t mcpwm_generator_set_action_on_brake_event(mcpwm_gen_handle_t generator, mcpwm_gen_brake_event_action_t ev_act);

/**
 * @brief Set generator actions on multiple MCPWM brake events
 *
 * @note This is an aggregation version of `mcpwm_generator_set_action_on_brake_event`, which allows user to set multiple actions in one call.
 *
 * @param[in] generator MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM brake event action list, must be terminated by `MCPWM_GEN_BRAKE_EVENT_ACTION_END()`
 * @return
 *      - ESP_OK: Set generator actions successfully
 *      - ESP_ERR_INVALID_ARG: Set generator actions failed because of invalid argument
 *      - ESP_FAIL: Set generator actions failed because of other error
 */
esp_err_t mcpwm_generator_set_actions_on_brake_event(mcpwm_gen_handle_t generator, mcpwm_gen_brake_event_action_t ev_act, ...);

/**
 * @brief Generator action on specific fault event
 */
typedef struct {
    mcpwm_timer_direction_t direction;       /*!< Timer direction */
    mcpwm_fault_handle_t fault;              /*!< Which fault as the trigger. Only support GPIO fault */
    mcpwm_generator_action_t action;         /*!< Generator action should perform */
} mcpwm_gen_fault_event_action_t;

/**
 * @brief Help macros to construct a mcpwm_gen_fault_event_action_t entry
 */
#define MCPWM_GEN_FAULT_EVENT_ACTION(dir, flt, act) \
    (mcpwm_gen_fault_event_action_t) { .direction = dir, .fault = flt, .action = act }

/**
 * @brief Set generator action on MCPWM Fault event
 *
 * @param[in] generator MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM trigger event action, can be constructed by `MCPWM_GEN_FAULT_EVENT_ACTION` helper macro
 * @return
 *      - ESP_OK: Set generator action successfully
 *      - ESP_ERR_INVALID_ARG: Set generator action failed because of invalid argument
 *      - ESP_FAIL: Set generator action failed because of other error
 */
esp_err_t mcpwm_generator_set_action_on_fault_event(mcpwm_gen_handle_t generator, mcpwm_gen_fault_event_action_t ev_act);

/**
 * @brief Generator action on specific sync event
 */
typedef struct {
    mcpwm_timer_direction_t direction;       /*!< Timer direction */
    mcpwm_sync_handle_t sync;                /*!< Which sync as the trigger*/
    mcpwm_generator_action_t action;         /*!< Generator action should perform */
} mcpwm_gen_sync_event_action_t;

/**
 * @brief Help macros to construct a mcpwm_gen_sync_event_action_t entry
 */
#define MCPWM_GEN_SYNC_EVENT_ACTION(dir, syn, act) \
    (mcpwm_gen_sync_event_action_t) { .direction = dir, .sync = syn, .action = act }

/**
 * @brief Set generator action on MCPWM Sync event
 *
 * @note The trigger only support one sync action, regardless of the kinds. Should not call this function more than once.
 *
 * @param[in] generator MCPWM generator handle, allocated by `mcpwm_new_generator()`
 * @param[in] ev_act MCPWM trigger event action, can be constructed by `MCPWM_GEN_SYNC_EVENT_ACTION` helper macro
 * @return
 *      - ESP_OK: Set generator action successfully
 *      - ESP_ERR_INVALID_ARG: Set generator action failed because of invalid argument
 *      - ESP_FAIL: Set generator action failed because of other error
 */
esp_err_t mcpwm_generator_set_action_on_sync_event(mcpwm_gen_handle_t generator, mcpwm_gen_sync_event_action_t ev_act);

/**
 * @brief MCPWM dead time configuration structure
 */
typedef struct {
    uint32_t posedge_delay_ticks; /*!< delay time applied to rising edge, 0 means no rising delay time */
    uint32_t negedge_delay_ticks; /*!< delay time applied to falling edge, 0 means no falling delay time */
    struct {
        uint32_t invert_output: 1; /*!< Invert the signal after applied the dead time */
    } flags;                       /*!< Extra flags for dead time configuration */
} mcpwm_dead_time_config_t;

/**
 * @brief Set dead time for MCPWM generator
 *
 * @note Due to a hardware limitation, you can't set rising edge delay for both MCPWM generator 0 and 1 at the same time,
 *       otherwise, there will be a conflict inside the dead time module. The same goes for the falling edge setting.
 *       But you can set both the rising edge and falling edge delay for the same MCPWM generator.
 *
 * @param[in] in_generator MCPWM generator, before adding the dead time
 * @param[in] out_generator MCPWM generator, after adding the dead time
 * @param[in] config MCPWM dead time configuration
 * @return
 *      - ESP_OK: Set dead time for MCPWM generator successfully
 *      - ESP_ERR_INVALID_ARG: Set dead time for MCPWM generator failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set dead time for MCPWM generator failed because of invalid state (e.g. delay module is already in use by other generator)
 *      - ESP_FAIL: Set dead time for MCPWM generator failed because of other error
 */
esp_err_t mcpwm_generator_set_dead_time(mcpwm_gen_handle_t in_generator, mcpwm_gen_handle_t out_generator, const mcpwm_dead_time_config_t *config);

#ifdef __cplusplus
}
#endif
