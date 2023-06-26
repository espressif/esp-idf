/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/mcpwm_types_legacy.h"

#if !CONFIG_MCPWM_SUPPRESS_DEPRECATE_WARN
#warning "legacy MCPWM driver is deprecated, please migrate to the new driver (include driver/mcpwm_prelude.h)"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes each gpio signal for MCPWM
 *
 * @note This function initializes one gpio at a time.
 *
 * @param mcpwm_num set MCPWM unit
 * @param io_signal set MCPWM signals, each MCPWM unit has 6 output(MCPWMXA, MCPWMXB) and 9 input(SYNC_X, FAULT_X, CAP_X)
 *                  'X' is timer_num(0-2)
 * @param gpio_num set this to configure gpio for MCPWM, if you want to use gpio16, gpio_num = 16
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_gpio_init(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num);

/**
 * @brief Initialize MCPWM gpio structure
 *
 * @note This function initialize a group of MCPWM GPIOs at a time.
 *
 * @param mcpwm_num set MCPWM unit
 * @param mcpwm_pin MCPWM pin structure
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_pin(mcpwm_unit_t mcpwm_num, const mcpwm_pin_config_t *mcpwm_pin);

/**
 * @brief Initialize MCPWM parameters
 * @note The default resolution configured for MCPWM timer is 1M, it can be changed by `mcpwm_timer_set_resolution`.
 * @note The default resolution configured for MCPWM group can be different on different esp targets (because of different clock source).
 *       You can change the group resolution by mcpwm_group_set_resolution()
 * @note If you want to change the preset resolution of MCPWM group and timer, please call them before this function.
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers.
 * @param mcpwm_conf configure structure mcpwm_config_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_init( mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_config_t  *mcpwm_conf);

/**
 * @brief Set resolution of the MCPWM group
 * @note This will override default resolution of MCPWM group.
 * @note This WILL NOT automatically update PWM frequency and duty. Please call `mcpwm_set_frequency` and `mcpwm_set_duty` manually to reflect the change.
 * @note The group resolution must be an integral multiple of timer resolution.
 *
 * @param mcpwm_num set MCPWM unit
 * @param resolution set expected frequency resolution
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_group_set_resolution(mcpwm_unit_t mcpwm_num, unsigned long int resolution);

/**
 * @brief Set resolution of each timer
 * @note This will override default resolution of timer(=1,000,000).
 * @note This WILL NOT automatically update PWM frequency and duty. Please call `mcpwm_set_frequency` and `mcpwm_set_duty` manually to reflect the change.
 * @note The group resolution must be an integral multiple of timer resolution.
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param resolution set expected frequency resolution
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_timer_set_resolution(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, unsigned long int resolution);

/**
 * @brief Set frequency(in Hz) of MCPWM timer
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param frequency set the frequency in Hz of each timer
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint32_t frequency);

/**
 * @brief Set duty cycle of each operator(MCPWMXA/MCPWMXB)
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the generator(MCPWMXA/MCPWMXB), 'X' is operator number selected
 * @param duty set duty cycle in %(i.e for 62.3% duty cycle, duty = 62.3) of each operator
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, float duty);

/**
 * @brief Set duty cycle of each operator(MCPWMXA/MCPWMXB) in us
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the generator(MCPWMXA/MCPWMXB), 'x' is operator number selected
 * @param duty_in_us set duty value in microseconds of each operator
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_duty_in_us(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, uint32_t duty_in_us);

/**
 * @brief Set duty either active high or active low(out of phase/inverted)
 * @note
 *        Call this function every time after mcpwm_set_signal_high or mcpwm_set_signal_low to resume with previously set duty cycle
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the generator(MCPWMXA/MCPWMXB), 'x' is operator number selected
 * @param duty_type set active low or active high duty type
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_duty_type(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, mcpwm_duty_type_t duty_type);

/**
 * @brief Get frequency of timer
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - frequency of timer
 */
uint32_t mcpwm_get_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Get duty cycle of each operator
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the generator(MCPWMXA/MCPWMXB), 'x' is operator number selected
 *
 * @return
 *     - duty cycle in % of each operator(56.7 means duty is 56.7%)
 */
float mcpwm_get_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t gen);

/**
 * @brief Get duty cycle of each operator in us
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the generator(MCPWMXA/MCPWMXB), 'x' is operator number selected
 *
 * @return
 *     - duty cycle in us of each operator
 */
uint32_t mcpwm_get_duty_in_us(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t gen);

/**
 * @brief Use this function to set MCPWM signal high
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the operator(MCPWMXA/MCPWMXB), 'x' is timer number selected
 *
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_signal_high(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen);

/**
 * @brief Use this function to set MCPWM signal low
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the operator(MCPWMXA/MCPWMXB), 'x' is timer number selected
 *
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_signal_low(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen);

/**
 * @brief Start MCPWM signal on timer 'x'
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_start(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Start MCPWM signal on timer 'x'
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief  Initialize carrier configuration
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param carrier_conf configure structure mcpwm_carrier_config_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_carrier_config_t *carrier_conf);

/**
 * @brief Enable MCPWM carrier submodule, for respective timer
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Disable MCPWM carrier submodule, for respective timer
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Set period of carrier
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param carrier_period set the carrier period of each timer, carrier period = (carrier_period + 1)*800ns
 *                    (carrier_period <= 15)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_set_period(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_period);

/**
 * @brief Set duty_cycle of carrier
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param carrier_duty set duty_cycle of carrier , carrier duty cycle = carrier_duty*12.5%
 *                  (chop_duty <= 7)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_set_duty_cycle(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_duty);

/**
 * @brief Enable and set width of first pulse in carrier oneshot mode
 *
 * @note The carrier oneshot pulse can't disabled.
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param pulse_width set pulse width of first pulse in oneshot mode, width = (carrier period)*(pulse_width +1)
 *                    (pulse_width <= 15)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_oneshot_mode_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t pulse_width);

/**
 * @brief Enable or disable carrier output inversion
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param carrier_ivt_mode enable or disable carrier output inversion
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_output_invert(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num,
                                      mcpwm_carrier_out_ivt_t carrier_ivt_mode);

/**
 * @brief Enable and initialize deadtime for each MCPWM timer
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param dt_mode set deadtime mode
 * @param red set rising edge delay = (red + 1) * MCPWM Group Resolution (default to 100ns, can be changed by `mcpwm_group_set_resolution`)
 * @param fed set rising edge delay = (fed + 1) * MCPWM Group Resolution (default to 100ns, can be changed by `mcpwm_group_set_resolution`)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_deadtime_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_deadtime_type_t dt_mode,
                                uint32_t red, uint32_t fed);

/**
 * @brief Disable deadtime on MCPWM timer
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_deadtime_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Initialize fault submodule, currently low level triggering is not supported
 *
 * @param mcpwm_num set MCPWM unit
 * @param intput_level set fault signal level, which will cause fault to occur
 * @param fault_sig set the fault pin, which needs to be enabled
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_fault_init(mcpwm_unit_t mcpwm_num, mcpwm_fault_input_level_t intput_level, mcpwm_fault_signal_t fault_sig);

/**
 * @brief Set oneshot mode on fault detection, once fault occur in oneshot mode reset is required to resume MCPWM signals
 * @note
 *        currently low level triggering is not supported
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param fault_sig set the fault pin, which needs to be enabled for oneshot mode
 * @param action_on_pwmxa action to be taken on MCPWMXA when fault occurs, either no change or high or low or toggle
 * @param action_on_pwmxb action to be taken on MCPWMXB when fault occurs, either no change or high or low or toggle
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_fault_set_oneshot_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                       mcpwm_output_action_t action_on_pwmxa, mcpwm_output_action_t action_on_pwmxb);

/**
 * @brief Set cycle-by-cycle mode on fault detection, once fault occur in cyc mode MCPWM signal resumes as soon as fault signal becomes inactive
 * @note
 *        currently low level triggering is not supported
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param fault_sig set the fault pin, which needs to be enabled for cyc mode
 * @param action_on_pwmxa action to be taken on MCPWMXA when fault occurs, either no change or high or low or toggle
 * @param action_on_pwmxb action to be taken on MCPWMXB when fault occurs, either no change or high or low or toggle
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_fault_set_cyc_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                   mcpwm_output_action_t action_on_pwmxa, mcpwm_output_action_t action_on_pwmxb);

/**
 * @brief Disable fault signal
 *
 * @param mcpwm_num set MCPWM unit
 * @param fault_sig fault pin, which needs to be disabled
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_fault_deinit(mcpwm_unit_t mcpwm_num, mcpwm_fault_signal_t fault_sig);

/**
 * @brief Enable capture channel
 *
 * @param mcpwm_num set MCPWM unit
 * @param cap_channel capture channel, which needs to be enabled
 * @param cap_conf capture channel configuration
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_capture_enable_channel(mcpwm_unit_t mcpwm_num, mcpwm_capture_channel_id_t cap_channel, const mcpwm_capture_config_t *cap_conf);

/**
 * @brief Disable capture channel
 *
 * @param mcpwm_num set MCPWM unit
 * @param cap_channel capture channel, which needs to be disabled
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_capture_disable_channel(mcpwm_unit_t mcpwm_num, mcpwm_capture_channel_id_t cap_channel);

/**
 * @brief Get capture value
 *
 * @param mcpwm_num set MCPWM unit
 * @param cap_sig capture channel on which value is to be measured
 *
 * @return
 *     Captured value
 */
uint32_t mcpwm_capture_signal_get_value(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig);

/**
 * @brief Get capture timer's resolution
 *
 * @param mcpwm_num set MCPWM unit
 * @return Capture timer's resolution
 */
uint32_t mcpwm_capture_get_resolution(mcpwm_unit_t mcpwm_num);

/**
 * @brief Get edge of capture signal
 *
 * @param mcpwm_num set MCPWM unit
 * @param cap_sig capture channel of whose edge is to be determined
 *
 * @return
 *     Capture signal edge: 1 - positive edge, 2 - negative edge, 0 - Invalid
 */
uint32_t mcpwm_capture_signal_get_edge(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig);

/**
 * @brief Initialize sync submodule and sets the signal that will cause the timer be loaded with pre-defined value
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param sync_conf sync configuration on this timer
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_sync_configure(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_sync_config_t *sync_conf);

/**
 * @brief Disable sync submodule on given timer
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_sync_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Set sync output on given timer
 *        Configures what event triggers MCPWM timer to output a sync signal.
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param trigger set the trigger that will cause the timer to generate a software sync signal.
 *                Specifically, `MCPWM_SWSYNC_SOURCE_DISABLED` will disable the timer from generating sync signal.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_timer_sync_output(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_timer_sync_trigger_t trigger);

/**
 * @brief Trigger a software sync event and sends it to a specific timer.
 *
 * @param mcpwm_num set MCPWM unit
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @note This software sync event will have the same effect as hw one, except that:
 *         - On esp32s3 the soft sync event can be routed to its output if `MCPWM_SWSYNC_SOURCE_SYNCIN` is selected via `mcpwm_set_timer_sync_output()`
 *         - On esp32 there is no such behavior and soft sync event will only take effect on this timer and can not be propagated to others.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t mcpwm_timer_trigger_soft_sync(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Set external GPIO sync input inverter
 *
 * @param mcpwm_num set MCPWM unit
 * @param sync_sig set sync signal of MCPWM, only supports GPIO sync signal
 * @param invert whether GPIO sync source input is inverted (to get negative edge trigger)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t mcpwm_sync_invert_gpio_synchro(mcpwm_unit_t mcpwm_num, mcpwm_sync_signal_t sync_sig, bool invert);

#ifdef __cplusplus
}
#endif
