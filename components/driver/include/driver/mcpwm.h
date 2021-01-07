/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#if SOC_MCPWM_SUPPORTED
#include "esp_err.h"
#include "soc/soc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_intr_alloc.h"
#include "hal/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IO signals for the MCPWM
 *
 *        - 6 MCPWM output pins that generate PWM signals
 *        - 3 MCPWM fault input pins to detect faults like overcurrent, overvoltage, etc.
 *        - 3 MCPWM sync input pins to synchronize MCPWM outputs signals
 *        - 3 MCPWM capture input pins to gather feedback from controlled motors, using e.g. hall sensors
 */
typedef enum {
    MCPWM0A = 0,        /*!<PWM0A output pin*/
    MCPWM0B,            /*!<PWM0B output pin*/
    MCPWM1A,            /*!<PWM1A output pin*/
    MCPWM1B,            /*!<PWM1B output pin*/
    MCPWM2A,            /*!<PWM2A output pin*/
    MCPWM2B,            /*!<PWM2B output pin*/
    MCPWM_SYNC_0,       /*!<SYNC0  input pin*/
    MCPWM_SYNC_1,       /*!<SYNC1  input pin*/
    MCPWM_SYNC_2,       /*!<SYNC2  input pin*/
    MCPWM_FAULT_0,      /*!<FAULT0 input pin*/
    MCPWM_FAULT_1,      /*!<FAULT1 input pin*/
    MCPWM_FAULT_2,      /*!<FAULT2 input pin*/
    MCPWM_CAP_0 = 84,   /*!<CAP0   input pin*/
    MCPWM_CAP_1,        /*!<CAP1   input pin*/
    MCPWM_CAP_2,        /*!<CAP2   input pin*/
} mcpwm_io_signals_t;

/**
 * @brief MCPWM pin number for
 *
 */
typedef struct {
    int mcpwm0a_out_num;       /*!<MCPWM0A out pin*/
    int mcpwm0b_out_num;       /*!<MCPWM0A out pin*/
    int mcpwm1a_out_num;       /*!<MCPWM0A out pin*/
    int mcpwm1b_out_num;       /*!<MCPWM0A out pin*/
    int mcpwm2a_out_num;       /*!<MCPWM0A out pin*/
    int mcpwm2b_out_num;       /*!<MCPWM0A out pin*/
    int mcpwm_sync0_in_num;    /*!<SYNC0  in pin*/
    int mcpwm_sync1_in_num;    /*!<SYNC1  in pin*/
    int mcpwm_sync2_in_num;    /*!<SYNC2  in pin*/
    int mcpwm_fault0_in_num;   /*!<FAULT0 in pin*/
    int mcpwm_fault1_in_num;   /*!<FAULT1 in pin*/
    int mcpwm_fault2_in_num;   /*!<FAULT2 in pin*/
    int mcpwm_cap0_in_num;     /*!<CAP0   in pin*/
    int mcpwm_cap1_in_num;     /*!<CAP1   in pin*/
    int mcpwm_cap2_in_num;     /*!<CAP2   in pin*/
} mcpwm_pin_config_t;

/**
 * @brief Select MCPWM unit
 */
typedef enum {
    MCPWM_UNIT_0 = 0,  /*!<MCPWM unit0 selected*/
    MCPWM_UNIT_1,      /*!<MCPWM unit1 selected*/
    MCPWM_UNIT_MAX,    /*!<Num of MCPWM units on ESP32*/
} mcpwm_unit_t;

_Static_assert(MCPWM_UNIT_MAX == SOC_MCPWM_GROUPS, "MCPWM unit number not equal to chip capabilities");

/**
 * @brief Select MCPWM timer
 */
typedef enum {
    MCPWM_TIMER_0 = 0,  /*!<Select MCPWM timer0*/
    MCPWM_TIMER_1,      /*!<Select MCPWM timer1*/
    MCPWM_TIMER_2,      /*!<Select MCPWM timer2*/
    MCPWM_TIMER_MAX,    /*!<Num of MCPWM timers on ESP32*/
} mcpwm_timer_t;

/**
 * @brief Select MCPWM operator
 */
typedef enum {
    MCPWM_GEN_A = 0,  /*!<Select MCPWMXA, where 'X' is operator number*/
    MCPWM_GEN_B,      /*!<Select MCPWMXB, where 'X' is operator number*/
    MCPWM_GEN_MAX,    /*!<Num of generators to each operator of MCPWM*/
} mcpwm_generator_t;

//definitions and macros to be back-compatible before IDFv4.1
#define MCPWM_OPR_A     MCPWM_GEN_A         ///< @deprecated
#define MCPWM_OPR_B     MCPWM_GEN_B         ///< @deprecated
#define MCPWM_OPR_MAX   MCPWM_GEN_MAX       ///< @deprecated
typedef mcpwm_generator_t mcpwm_operator_t; ///< @deprecated

/**
 * @brief MCPWM carrier oneshot mode, in this mode the width of the first pulse of carrier can be programmed
 */
typedef enum {
    MCPWM_ONESHOT_MODE_DIS = 0,  /*!<Enable oneshot mode*/
    MCPWM_ONESHOT_MODE_EN,       /*!<Disable oneshot mode*/
} mcpwm_carrier_os_t;

/**
 * @brief MCPWM carrier output inversion, high frequency carrier signal active with MCPWM signal is high
 */
typedef enum {
    MCPWM_CARRIER_OUT_IVT_DIS = 0,  /*!<Enable  carrier output inversion*/
    MCPWM_CARRIER_OUT_IVT_EN,       /*!<Disable carrier output inversion*/
} mcpwm_carrier_out_ivt_t;

/**
 * @brief MCPWM select fault signal input
 */
typedef enum {
    MCPWM_SELECT_F0 = 0,  /*!<Select F0 as input*/
    MCPWM_SELECT_F1,      /*!<Select F1 as input*/
    MCPWM_SELECT_F2,      /*!<Select F2 as input*/
} mcpwm_fault_signal_t;

/**
 * @brief MCPWM select sync signal input
 */
typedef enum {
    MCPWM_SELECT_SYNC0 = 4,  /*!<Select SYNC0 as input*/
    MCPWM_SELECT_SYNC1,      /*!<Select SYNC1 as input*/
    MCPWM_SELECT_SYNC2,      /*!<Select SYNC2 as input*/
} mcpwm_sync_signal_t;

/**
 * @brief MCPWM select triggering level of fault signal
 */
typedef enum {
    MCPWM_LOW_LEVEL_TGR = 0,  /*!<Fault condition occurs when fault input signal goes from high to low, currently not supported*/
    MCPWM_HIGH_LEVEL_TGR,     /*!<Fault condition occurs when fault input signal goes low to high*/
} mcpwm_fault_input_level_t;

/**
 * @brief MCPWM select capture starts from which edge
 */
typedef enum {
    MCPWM_NEG_EDGE = BIT(0),           /*!<Capture the negative edge*/
    MCPWM_POS_EDGE = BIT(1),           /*!<Capture the positive edge*/
    MCPWM_BOTH_EDGE = BIT(1) | BIT(0), /*!<Capture both edges*/
} mcpwm_capture_on_edge_t;

/**
 * @brief Select type of MCPWM counter
 */
typedef enum {
    MCPWM_FREEZE_COUNTER,   /*!<Counter freeze */
    MCPWM_UP_COUNTER,       /*!<For asymmetric MCPWM*/
    MCPWM_DOWN_COUNTER,     /*!<For asymmetric MCPWM*/
    MCPWM_UP_DOWN_COUNTER,  /*!<For symmetric MCPWM, frequency is half of MCPWM frequency set*/
    MCPWM_COUNTER_MAX,      /*!<Maximum counter mode*/
} mcpwm_counter_type_t;

/**
 * @brief Select type of MCPWM duty cycle mode
 */
typedef enum {
    MCPWM_DUTY_MODE_0 = 0, /*!<Active high duty, i.e. duty cycle proportional to high time for asymmetric MCPWM*/
    MCPWM_DUTY_MODE_1,     /*!<Active low duty,  i.e. duty cycle proportional to low  time for asymmetric MCPWM, out of phase(inverted) MCPWM*/
    MCPWM_HAL_GENERATOR_MODE_FORCE_LOW,
    MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH,
    MCPWM_DUTY_MODE_MAX,   /*!<Num of duty cycle modes*/
} mcpwm_duty_type_t;

/**
 * @brief MCPWM deadtime types, used to generate deadtime, RED refers to rising edge delay and FED refers to falling edge delay
 */
typedef enum {
    MCPWM_DEADTIME_BYPASS = 0,          /*!<Bypass the deadtime*/
    MCPWM_BYPASS_RED,                   /*!<MCPWMXA = no change, MCPWMXB = falling edge delay*/
    MCPWM_BYPASS_FED,                   /*!<MCPWMXA = rising edge delay, MCPWMXB = no change*/
    MCPWM_ACTIVE_HIGH_MODE,             /*!<MCPWMXA = rising edge delay,  MCPWMXB = falling edge delay*/
    MCPWM_ACTIVE_LOW_MODE,              /*!<MCPWMXA = compliment of rising edge delay,  MCPWMXB = compliment of falling edge delay*/
    MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE,  /*!<MCPWMXA = rising edge delay,  MCPWMXB = compliment of falling edge delay*/
    MCPWM_ACTIVE_LOW_COMPLIMENT_MODE,   /*!<MCPWMXA = compliment of rising edge delay,  MCPWMXB = falling edge delay*/
    MCPWM_ACTIVE_RED_FED_FROM_PWMXA,    /*!<MCPWMXA = MCPWMXB = rising edge delay as well as falling edge delay, generated from MCPWMXA*/
    MCPWM_ACTIVE_RED_FED_FROM_PWMXB,    /*!<MCPWMXA = MCPWMXB = rising edge delay as well as falling edge delay, generated from MCPWMXB*/
    MCPWM_DEADTIME_TYPE_MAX,
} mcpwm_deadtime_type_t;

/**
 * @brief MCPWM select action to be taken on the output when event happens
 */
typedef enum {
    MCPWM_ACTION_NO_CHANGE = 0,  /*!<No change in the output*/
    MCPWM_ACTION_FORCE_LOW,      /*!<Make output low*/
    MCPWM_ACTION_FORCE_HIGH,     /*!<Make output high*/
    MCPWM_ACTION_TOGGLE,         /*!<Make output toggle*/
} mcpwm_output_action_t;

/// @deprecated MCPWM select action to be taken on MCPWMXA when fault occurs
typedef mcpwm_output_action_t mcpwm_action_on_pwmxa_t;
#define MCPWM_NO_CHANGE_IN_MCPWMXA  MCPWM_ACTION_NO_CHANGE      /*!< @deprecated No change in MCPWMXA output*/
#define MCPWM_FORCE_MCPWMXA_LOW     MCPWM_ACTION_FORCE_LOW      /*!< @deprecated Make MCPWMXA output low*/
#define MCPWM_FORCE_MCPWMXA_HIGH    MCPWM_ACTION_FORCE_HIGH     /*!< @deprecated Make MCPWMXA output high*/
#define MCPWM_TOG_MCPWMXA           MCPWM_ACTION_TOGGLE         /*!< @deprecated Make MCPWMXA output toggle*/

/// @deprecated MCPWM select action to be taken on MCPWMXB when fault occurs
typedef mcpwm_output_action_t mcpwm_action_on_pwmxb_t;
#define MCPWM_NO_CHANGE_IN_MCPWMXB  MCPWM_ACTION_NO_CHANGE      /*!< @deprecated No change in MCPWMXB output*/
#define MCPWM_FORCE_MCPWMXB_LOW     MCPWM_ACTION_FORCE_LOW      /*!< @deprecated Make MCPWMXB output low*/
#define MCPWM_FORCE_MCPWMXB_HIGH    MCPWM_ACTION_FORCE_HIGH     /*!< @deprecated Make MCPWMXB output high*/
#define MCPWM_TOG_MCPWMXB           MCPWM_ACTION_TOGGLE         /*!< @deprecated Make MCPWMXB output toggle*/

/**
 * @brief MCPWM select capture signal input
 */
typedef enum {
    MCPWM_SELECT_CAP0 = 0, /*!<Select CAP0 as input*/
    MCPWM_SELECT_CAP1,     /*!<Select CAP1 as input*/
    MCPWM_SELECT_CAP2,     /*!<Select CAP2 as input*/
} mcpwm_capture_signal_t;

/**
 * @brief MCPWM config structure
 */
typedef struct {
    uint32_t frequency;              /*!<Set frequency of MCPWM in Hz*/
    float cmpr_a;                    /*!<Set % duty cycle for operator a(MCPWMXA), i.e for 62.3% duty cycle, duty_a = 62.3*/
    float cmpr_b;                    /*!<Set % duty cycle for operator b(MCPWMXB), i.e for 48% duty cycle, duty_b = 48.0*/
    mcpwm_duty_type_t duty_mode;     /*!<Set type of duty cycle*/
    mcpwm_counter_type_t counter_mode;  /*!<Set  type of MCPWM counter*/
} mcpwm_config_t;

/**
 * @brief MCPWM config carrier structure
 */
typedef struct {
    uint8_t carrier_period;                    /*!<Set carrier period = (carrier_period + 1)*800ns, carrier_period should be < 16*/
    uint8_t carrier_duty;                      /*!<Set carrier duty cycle, carrier_duty should be less than 8 (increment every 12.5%)*/
    uint8_t pulse_width_in_os;                 /*!<Set pulse width of first pulse in one shot mode = (carrier period)*(pulse_width_in_os + 1), should be less then 16*/
    mcpwm_carrier_os_t carrier_os_mode;        /*!<Enable or disable carrier oneshot mode*/
    mcpwm_carrier_out_ivt_t carrier_ivt_mode;  /*!<Invert output of carrier*/
} mcpwm_carrier_config_t;

/**
 * @brief This function initializes each gpio signal for MCPWM
 *        @note
 *        This function initializes one gpio at a time.
 *
 * @param mcpwm_num set MCPWM unit(0-1)
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
 *        @note
 *        This function can be used to initialize more then one gpio at a time.
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param mcpwm_pin MCPWM pin structure
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_pin(mcpwm_unit_t mcpwm_num, const mcpwm_pin_config_t *mcpwm_pin);

/**
 * @brief Initialize MCPWM parameters
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers.
 * @param mcpwm_conf configure structure mcpwm_config_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_init( mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_config_t  *mcpwm_conf);

/**
 * @brief Set frequency(in Hz) of MCPWM timer
 *
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 *        @note
 *        Call this function every time after mcpwm_set_signal_high or mcpwm_set_signal_low to resume with previously set duty cycle
 *
 * @param mcpwm_num set MCPWM unit(0-1)
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
* @param mcpwm_num set MCPWM unit(0-1)
* @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
*
* @return
*     - frequency of timer
*/
uint32_t mcpwm_get_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Get duty cycle of each operator
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the generator(MCPWMXA/MCPWMXB), 'x' is operator number selected
 *
 * @return
 *     - duty cycle in % of each operator(56.7 means duty is 56.7%)
 */
float mcpwm_get_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t gen);

/**
 * @brief Use this function to set MCPWM signal high
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the operator(MCPWMXA/MCPWMXB), 'x' is timer number selected

 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_signal_high(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen);

/**
 * @brief Use this function to set MCPWM signal low
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param gen set the operator(MCPWMXA/MCPWMXB), 'x' is timer number selected

 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_set_signal_low(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen);

/**
 * @brief Start MCPWM signal on timer 'x'
 *
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
* @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @brief Disable oneshot mode, width of first pulse = carrier period
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_carrier_oneshot_mode_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Enable or disable carrier output inversion
 *
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param dt_mode set deadtime mode
 * @param red set rising edge delay = red*100ns
 * @param fed set rising edge delay = fed*100ns
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
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
 *        @note
 *        currently low level triggering is not supported
 *
 * @param mcpwm_num set MCPWM unit(0-1)
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
 *        @note
 *        currently low level triggering is not supported
 *
 * @param mcpwm_num set MCPWM unit(0-1)
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
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param fault_sig fault pin, which needs to be disabled
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_fault_deinit(mcpwm_unit_t mcpwm_num, mcpwm_fault_signal_t fault_sig);

/**
 * @brief Initialize capture submodule
 *
 * @note Enabling capture feature could also enable the capture interrupt,
 *       users have to register an interrupt handler by `mcpwm_isr_register`, and in there, query the capture data.
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param cap_edge set capture edge, BIT(0) - negative edge, BIT(1) - positive edge
 * @param cap_sig capture pin, which needs to be enabled
 * @param num_of_pulse count time between rising/falling edge between 2 *(pulses mentioned), counter uses APB_CLK
 *                     [0~MCPWM_LL_MAX_PRESCALE] (MCPWM_LL_MAX_PRESCALE = 255 on ESP32);
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */

esp_err_t mcpwm_capture_enable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig, mcpwm_capture_on_edge_t cap_edge,
                               uint32_t num_of_pulse);

/**
 * @brief Disable capture signal
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param cap_sig capture pin, which needs to be disabled
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_capture_disable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig);

/**
 * @brief Get capture value
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param cap_sig capture pin on which value is to be measured
 *
 * @return
 *     Captured value
 */
uint32_t mcpwm_capture_signal_get_value(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig);

/**
 * @brief Get edge of capture signal
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param cap_sig capture pin of whose edge is to be determined
 *
 * @return
 *     Capture signal edge: 1 - positive edge, 2 - negtive edge
 */
uint32_t mcpwm_capture_signal_get_edge(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig);

/**
 * @brief Initialize sync submodule
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 * @param sync_sig set the synchronization pin, which needs to be enabled
 * @param phase_val phase value in 1/1000 (for 86.7%, phase_val = 867) which timer moves to on sync signal
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_sync_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_sync_signal_t sync_sig,
                            uint32_t phase_val);

/**
 * @brief Disable sync submodule on given timer
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param timer_num set timer number(0-2) of MCPWM, each MCPWM unit has 3 timers
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mcpwm_sync_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

/**
 * @brief Register MCPWM interrupt handler, the handler is an ISR.
 *        the handler will be attached to the same CPU core that this function is running on.
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param fn interrupt handler function.
 * @param arg user-supplied argument passed to the handler function.
 * @param intr_alloc_flags flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. see esp_intr_alloc.h for more info.
 * @param handle pointer to return handle. If non-NULL, a handle for the interrupt will
 *        be returned here.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t mcpwm_isr_register(mcpwm_unit_t mcpwm_num, void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle);


#ifdef __cplusplus
}
#endif

#endif  //SOC_MCPWM_SUPPORTED
