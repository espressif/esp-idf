// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for MCPWM (common part)

/*
 * MCPWM HAL usages:
 *
 * Initialization:
 *  1. Fill the parameters in `mcpwm_hal_context_t`.
 *  2. Call `mcpwm_hal_init` to initialize the context.
 *  3. Call `mcpwm_hal_hw_init` to initialize the hardware.
 *
 * Basic PWM:
 *  1. Update parameters for the timers, comparators and generators.
 *  2. Call `mcpwm_hal_timer_update_basic` to update the timer used.
 *  3. Call `mcpwm_hal_operator_update_basic` to update all the parameters of a operator.
 *
 *     Alternatively, if only the comparator is updated (duty rate), call
 *     `mcpwm_hal_operator_update_comparator` to update the comparator parameters; if only the
 *     generator is updated (output style), call `mcpwm_hal_operator_update_generator` to update the
 *     generator parameters.
 *
 *  4. At any time, call `mcpwm_hal_timer_start` to start the timer (so that PWM output will toggle
 *    according to settings), or call `mcpwm_hal_timer_stop` to stop the timer (so that the PWM output
 *    will be kept as called).
 *
 * Timer settings:
 *  - Sync: Call `mcpwm_hal_timer_enable_sync` to enable the sync for the timer, and call
 *  `mcpwm_hal_timer_disable_sync` to disable it.
 *
 * Operator settings:
 *  - Carrier: Call `mcpwm_hal_operator_enable_carrier` to enable carrier for an operator, and call
 *  `mcpwm_hal_operator_disable_carrier` to disable it.
 *
 *  - Deadzone: Call `mcpwm_hal_operator_update_deadzone` to update settings of deadzone for an operator.
 *
 * Fault handling settings:
 *  1. Call `mcpwm_hal_fault_init` to initialize an fault signal to be detected.
 *  2. Call `mcpwm_hal_operator_update_fault` to update the behavior of an operator when fault is
 *     detected.
 *  3. If the operator selects oneshot mode to handle the fault event, call
 *     `mcpwm_hal_fault_oneshot_clear` to clear that fault event after the fault is handled properly.
 *  4. Call `mcpwm_hal_fault_disable` to deinitialize the fault signal when it's no longer used.
 *
 * Capture:
 *  1. Call `mcpwm_hal_capture_enable` to enable the capture for one capture signal.
 *  2. Call `mcpwm_hal_capture_get_result` to get the last captured result.
 *  3. Call `mcpwm_hal_capture_disable` to disable the capture for a signal.
 */


#pragma once

#include <esp_err.h>
#include "hal/mcpwm_ll.h"

#define MCPWM_BASE_CLK (2 * APB_CLK_FREQ)   //2*APB_CLK_FREQ 160Mhz

/// Configuration of HAL that used only once.
typedef struct {
    int host_id;    ///< Which MCPWM peripheral to use, 0-1.
} mcpwm_hal_init_config_t;

/// Configuration of each generator (output of operator)
typedef struct {
    mcpwm_duty_type_t   duty_type;  ///< How the generator output
    int comparator; ///< for mode `MCPWM_DUTY_MODE_*`, which comparator it refers to.
} mcpwm_hal_generator_config_t;

/// Configuration of each operator
typedef struct {
    mcpwm_hal_generator_config_t gen[SOC_MCPWM_GENERATORS_PER_OPERATOR];  ///< Configuration of the generators
    float duty[SOC_MCPWM_COMPARATORS_PER_OPERATOR];   ///< Duty rate for each comparator, 10 means 10%.
    int timer;      ///< The timer this operator is using
} mcpwm_hal_operator_config_t;

/// Configuration of each timer
typedef struct {
    uint32_t timer_prescale;    ///< The prescale from the MCPWM main clock to the timer clock, TIMER_FREQ=(MCPWM_FREQ/(timer_prescale+1))
    uint32_t freq;              ///< Frequency desired, will be updated to actual value after the `mcpwm_hal_timer_update_freq` is called.
    mcpwm_counter_type_t count_mode;    ///< Counting mode
} mcpwm_hal_timer_config_t;

typedef struct {
    mcpwm_dev_t *dev;           ///< Beginning address of the MCPWM peripheral registers. Call `mcpwm_hal_init` to initialize it.
    uint32_t    prescale;       ///< Prescale from the 160M clock to MCPWM main clock.
    mcpwm_hal_timer_config_t    timer[SOC_MCPWM_TIMERS_PER_GROUP]; ///< Configuration of the timers
    mcpwm_hal_operator_config_t op[SOC_MCPWM_OPERATORS_PER_GROUP];       ///< Configuration of the operators
} mcpwm_hal_context_t;

/// Configuration of the carrier
typedef struct {
    bool inverted;  ///< Whether to invert the output
    uint8_t  duty;  ///< Duty of the carrier, 0-7. Duty rate = duty/8.
    uint8_t oneshot_pulse_width;    ///< oneshot pulse width, in carrier periods. 0 to disable. 0-15.
    uint32_t period; ///< Prescale from the MCPWM main clock to the carrier clock. CARRIER_FREQ=(MCPWM_FREQ/(period+1)/8.)
} mcpwm_hal_carrier_conf_t;

/// Configuration of the deadzone
typedef struct {
    mcpwm_deadtime_type_t mode; ///< Deadzone mode, `MCPWM_DEADTIME_BYPASS` to disable.
    uint32_t fed;   ///< Delay on falling edge. By MCPWM main clock.
    uint32_t red;   ///< Delay on rising edge. By MCPWM main clock.
} mcpwm_hal_deadzone_conf_t;

/// Configuration of the fault handling for each operator
typedef struct {
    uint32_t cbc_enabled_mask;  ///< Whether the cycle-by-cycle fault handling is enabled on each fault signal. BIT(n) stands for signal n.
    uint32_t ost_enabled_mask;  ///< Whether the oneshot fault handling is enabled on each on each fault signal. BIT(n) stands for signal n.
    mcpwm_output_action_t   action_on_fault[SOC_MCPWM_GENERATORS_PER_OPERATOR];   ///< Action to perform on each generator when any one of the fault signal triggers.
} mcpwm_hal_fault_conf_t;

/// Configuration of the synchronization of each clock
typedef struct {
    mcpwm_sync_signal_t sync_sig;   ///< Sync signal to use
    uint32_t reload_permillage;     ///< Reload permillage when the sync is triggered. 100 means the timer will be reload to (period * 100)/1000=10% period value.
} mcpwm_hal_sync_config_t;

/// Configuration of the capture feature on each capture signal
typedef struct {
    mcpwm_capture_on_edge_t cap_edge;   ///< Whether the edges is captured, bitwise.
    uint32_t prescale;          ///< Prescale of the input signal.
} mcpwm_hal_capture_config_t;

/**
 * @brief Initialize the internal state of the HAL. Call after settings are set and before other functions are called.
 *
 * @note Since There are several individual parts (timers + operators, captures), this funciton is
 *  allowed to called several times.
 *
 * @param hal Context of the HAL layer.
 * @param init_config Configuration for the HAL to be used only once.
 */
void mcpwm_hal_init(mcpwm_hal_context_t *hal, const mcpwm_hal_init_config_t *init_config);

/**
 * @brief Initialize the hardware, call after `mcpwm_hal_init` and before other functions.
 *
 * @param hal Context of the HAL layer.
 */
void mcpwm_hal_hw_init(mcpwm_hal_context_t *hal);

/**
 * @brief Start a timer
 *
 * @param hal Context of the HAL layer.
 * @param timer Timer to start, 0-2.
 */
void mcpwm_hal_timer_start(mcpwm_hal_context_t *hal, int timer);

/**
 * @brief Stop a timer.
 *
 * @param hal Context of the HAL layer.
 * @param timer Timer to stop, 0-2.
 */
void mcpwm_hal_timer_stop(mcpwm_hal_context_t *hal, int timer);

/**
 * @brief Update the basic parameters of a timer.
 *
 * @note This will influence the duty rate and count mode of each operator relies on this timer.
 *       Call `mcpwm_hal_operator_update_basic` for each of the operator that relies on this timer after
 *       to update the duty rate and generator output.
 *
 * @param hal Context of the HAL layer.
 * @param timer Timer to update, 0-2.
 */
void mcpwm_hal_timer_update_basic(mcpwm_hal_context_t *hal, int timer);

/**
 * @brief Start the synchronization for a timer.
 *
 * @param hal Context of the HAL layer.
 * @param timer Timer to enable, 0-2.
 * @param sync_conf Configuration of the sync operation.
 */
void mcpwm_hal_timer_enable_sync(mcpwm_hal_context_t *hal, int timer, const mcpwm_hal_sync_config_t *sync_conf);

/**
 * @brief Stop the synchronization for a timer.
 *
 * @param hal Context of the HAL layer.
 * @param timer Timer to disable sync, 0-2.
 */
void mcpwm_hal_timer_disable_sync(mcpwm_hal_context_t *hal, int timer);

/**
 * @brief Update the basic settings (duty, output mode) for an operator.
 *
 * Will call `mcpwm_hal_operator_update_comparator` and `mcpwm_hal_operator_update_generator`
 * recursively to update each of their duty and output mode.
 *
 * @param hal Context of the HAL layer.
 * @param op Operator to update, 0-2.
 */
void mcpwm_hal_operator_update_basic(mcpwm_hal_context_t *hal, int op);

/**
 * @brief Update a comparator (duty) for an operator.
 *
 * @param hal Context of the HAL layer.
 * @param op Operator to update, 0-2.
 * @param cmp Comparator to update, 0-1.
 */
void mcpwm_hal_operator_update_comparator(mcpwm_hal_context_t *hal, int op, int cmp);

/**
 * @brief Update a generator (output mode) for an operator.
 *
 * @param hal Context of the HAL layer.
 * @param op Operator to update, 0-2.
 * @param cmp Comparator to update, 0-1.
 */
void mcpwm_hal_operator_update_generator(mcpwm_hal_context_t *hal, int op, int gen_num);

/**
 * @brief Enable the carrier for an operator.
 *
 * @param hal Context of the HAL layer.
 * @param op Operator to enable carrier, 0-2.
 * @param carrier_conf Configuration of the carrier.
 */
void mcpwm_hal_operator_enable_carrier(mcpwm_hal_context_t *hal, int op, const mcpwm_hal_carrier_conf_t *carrier_conf);

/**
 * @brief Disable the carrier for an operator.
 *
 * @param hal Context of the HAL layer.
 * @param op  Operator to disable carrier, 0-2.
 */
void mcpwm_hal_operator_disable_carrier(mcpwm_hal_context_t *hal, int op);

/**
 * @brief Update the deadzone for an operator.
 *
 * @param hal Context of the HAL layer.
 * @param op Operator to update the deadzone, 0-2.
 * @param deadzone Configuration of the deadzone. Set member `mode` to `MCPWM_DEADTIME_BYPASS` will bypass the deadzone.
 */
void mcpwm_hal_operator_update_deadzone(mcpwm_hal_context_t *hal, int op, const mcpwm_hal_deadzone_conf_t *deadzone);

/**
 * @brief Enable one of the fault signal.
 *
 * @param hal Context of the HAL layer.
 * @param fault_sig The signal to enable, 0-2.
 * @param level The active level for the fault signal, true for high and false for low.
 */
void mcpwm_hal_fault_init(mcpwm_hal_context_t *hal, int fault_sig, bool level);

/**
 * @brief Configure how the operator behave to the fault signals.
 *
 * Call after the fault signal is enabled by `mcpwm_hal_fault_init`.
 *
 * @param hal Context of the HAL layer.
 * @param op Operator to configure, 0-2.
 * @param fault_conf Configuration of the behavior of the operator when fault. Clear member `cbc_enabled_mask` and `ost_enabled_mask` will disable the fault detection of this operator.
 */
void mcpwm_hal_operator_update_fault(mcpwm_hal_context_t *hal, int op, const mcpwm_hal_fault_conf_t *fault_conf);

/**
 * @brief Clear the oneshot fault status for an operator.
 *
 * @param hal Context of the HAL layer.
 * @param op The operator to clear oneshot fault status, 0-2.
 */
void mcpwm_hal_fault_oneshot_clear(mcpwm_hal_context_t *hal, int op);

/**
 * @brief Disable one of the fault signal.
 *
 * @param hal Context of the HAL layer.
 * @param fault_sig The fault signal to disable, 0-2.
 */
void mcpwm_hal_fault_disable(mcpwm_hal_context_t *hal, int fault_sig);

/**
 * @brief Enable one of the capture signal.
 *
 * @param hal Context of the HAL layer.
 * @param cap_sig Capture signal to enable, 0-2.
 * @param conf Configuration on how to capture the signal.
 */
void mcpwm_hal_capture_enable(mcpwm_hal_context_t *hal, int cap_sig, const mcpwm_hal_capture_config_t *conf);

/**
 * @brief Get the capture result.
 *
 * @note The output value will always be updated with the register value, no matter event triggered or not.
 *
 * @param hal Context of the HAL layer.
 * @param cap_sig Signal to get capture result, 0-2.
 * @param out_count Output of the captured counter.
 * @param out_edge  Output of the captured edge.
 * @return
 *  - ESP_OK: if a signal is captured
 *  - ESP_ERR_NOT_FOUND: if no capture event happened.
 */
esp_err_t mcpwm_hal_capture_get_result(mcpwm_hal_context_t *hal, int cap_sig, uint32_t *out_count,
                                       mcpwm_capture_on_edge_t *out_edge);

/**
 * @brief Disable one of the capture signal.
 *
 * @param hal Context of the HAL layer.
 * @param cap_sig The signal to capture, 0-2.
 */
void mcpwm_hal_capture_disable(mcpwm_hal_context_t *hal, int cap_sig);
