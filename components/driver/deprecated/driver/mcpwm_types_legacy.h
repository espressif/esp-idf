/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"
#include "hal/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IO signals for the MCPWM
 *
 *        - 6 MCPWM output pins that generate PWM signals
 *        - 3 MCPWM fault input pins to detect faults like over-current, over-voltage, etc.
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
 * @brief pin number for MCPWM
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
    MCPWM_UNIT_0,   /*!<MCPWM unit0 selected*/
#if SOC_MCPWM_GROUPS > 1
    MCPWM_UNIT_1,   /*!<MCPWM unit1 selected*/
#endif
    MCPWM_UNIT_MAX, /*!<Max number of MCPWM units*/
} mcpwm_unit_t;

/**
 * @brief Select MCPWM timer
 */
typedef enum {
    MCPWM_TIMER_0,   /*!<Select MCPWM timer0*/
    MCPWM_TIMER_1,   /*!<Select MCPWM timer1*/
    MCPWM_TIMER_2,   /*!<Select MCPWM timer2*/
    MCPWM_TIMER_MAX, /*!<Max number of timers in a unit*/
} mcpwm_timer_t;

/**
 * @brief Select MCPWM operator
 */
typedef enum {
    MCPWM_GEN_A,   /*!<Select MCPWMXA, where 'X' is operator number*/
    MCPWM_GEN_B,   /*!<Select MCPWMXB, where 'X' is operator number*/
    MCPWM_GEN_MAX, /*!<Num of generators to each operator of MCPWM*/
} mcpwm_generator_t;

//definitions and macros to be back-compatible before IDFv4.1
#define MCPWM_OPR_A     MCPWM_GEN_A         ///< @deprecated
#define MCPWM_OPR_B     MCPWM_GEN_B         ///< @deprecated
#define MCPWM_OPR_MAX   MCPWM_GEN_MAX       ///< @deprecated
typedef mcpwm_generator_t mcpwm_operator_t; ///< @deprecated

/**
 * @brief MCPWM carrier output inversion, high frequency carrier signal active with MCPWM signal is high
 */
typedef enum {
    MCPWM_CARRIER_OUT_IVT_DIS, /*!<Enable  carrier output inversion*/
    MCPWM_CARRIER_OUT_IVT_EN,  /*!<Disable carrier output inversion*/
} mcpwm_carrier_out_ivt_t;

/**
 * @brief MCPWM select fault signal input
 */
typedef enum {
    MCPWM_SELECT_F0, /*!<Select F0 as input*/
    MCPWM_SELECT_F1, /*!<Select F1 as input*/
    MCPWM_SELECT_F2, /*!<Select F2 as input*/
} mcpwm_fault_signal_t;

/**
 * @brief MCPWM select sync signal input
 */
typedef enum {
    MCPWM_SELECT_NO_INPUT,        /*!<No sync input selected*/
    MCPWM_SELECT_TIMER0_SYNC,     /*!<Select software sync signal from timer0 as input*/
    MCPWM_SELECT_TIMER1_SYNC,     /*!<Select software sync signal from timer1 as input*/
    MCPWM_SELECT_TIMER2_SYNC,     /*!<Select software sync signal from timer2 as input*/
    MCPWM_SELECT_GPIO_SYNC0,      /*!<Select GPIO SYNC0 as input*/
    MCPWM_SELECT_GPIO_SYNC1,      /*!<Select GPIO SYNC1 as input*/
    MCPWM_SELECT_GPIO_SYNC2,      /*!<Select GPIO SYNC2 as input*/
} mcpwm_sync_signal_t;

// backward compatibility
#define MCPWM_SELCT_SYNC0 MCPWM_SELCT_GPIO_SYNC0
#define MCPWM_SELCT_SYNC1 MCPWM_SELCT_GPIO_SYNC1
#define MCPWM_SELCT_SYNC2 MCPWM_SELCT_GPIO_SYNC2

/**
 * @brief MCPWM timer sync event trigger
 */
typedef enum {
    MCPWM_SWSYNC_SOURCE_SYNCIN,      /*!<the input sync signal will be routed to its sync output path*/
    MCPWM_SWSYNC_SOURCE_TEZ,         /*!<sync signal generated when timer counts to zero*/
    MCPWM_SWSYNC_SOURCE_TEP,         /*!<sync signal generated when timer counts to peak*/
    MCPWM_SWSYNC_SOURCE_DISABLED,    /*!<timer does not generate sync signals*/
} mcpwm_timer_sync_trigger_t;

/**
 * @brief MCPWM select triggering level of fault signal
 */
typedef enum {
    MCPWM_LOW_LEVEL_TGR,  /*!<Fault condition occurs when fault input signal goes from high to low*/
    MCPWM_HIGH_LEVEL_TGR, /*!<Fault condition occurs when fault input signal goes low to high*/
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
    MCPWM_DUTY_MODE_0 = 0,      /*!<Active high duty, i.e. duty cycle proportional to high time for asymmetric MCPWM*/
    MCPWM_DUTY_MODE_1,          /*!<Active low duty,  i.e. duty cycle proportional to low  time for asymmetric MCPWM, out of phase(inverted) MCPWM*/
    MCPWM_DUTY_MODE_FORCE_LOW,  /*!< Forced to output low level */
    MCPWM_DUTY_MODE_FORCE_HIGH, /*!< Forced to output high level */
    MCPWM_DUTY_MODE_MAX,        /*!<Num of duty cycle modes*/
} mcpwm_duty_type_t;

#define MCPWM_HAL_GENERATOR_MODE_FORCE_LOW MCPWM_DUTY_MODE_FORCE_LOW    /*!< @deprecated Forced to output low level */
#define MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH MCPWM_DUTY_MODE_FORCE_HIGH  /*!< @deprecated Forced to output low level */

/**
 * @brief MCPWM deadtime types, used to generate deadtime, RED refers to rising edge delay and FED refers to falling edge delay
 */
typedef enum {
    MCPWM_DEADTIME_BYPASS = 0,          /*!<Bypass the deadtime*/
    MCPWM_BYPASS_RED,                   /*!<MCPWMXA Out = MCPWMXA In with no delay, MCPWMXB Out = MCPWMXA In with falling edge delay*/
    MCPWM_BYPASS_FED,                   /*!<MCPWMXA Out = MCPWMXA In with rising edge delay, MCPWMXB Out = MCPWMXB In with no delay*/
    MCPWM_ACTIVE_HIGH_MODE,             /*!<MCPWMXA Out = MCPWMXA In with rising edge delay,  MCPWMXB Out = MCPWMXA In with falling edge delay*/
    MCPWM_ACTIVE_LOW_MODE,              /*!<MCPWMXA Out = MCPWMXA In with compliment of rising edge delay,  MCPWMXB Out = MCPWMXA In with compliment of falling edge delay*/
    MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE,  /*!<MCPWMXA Out = MCPWMXA In with rising edge delay,  MCPWMXB = MCPWMXA In with compliment of falling edge delay*/
    MCPWM_ACTIVE_LOW_COMPLIMENT_MODE,   /*!<MCPWMXA Out = MCPWMXA In with compliment of rising edge delay,  MCPWMXB Out = MCPWMXA In with falling edge delay*/
    MCPWM_ACTIVE_RED_FED_FROM_PWMXA,    /*!<MCPWMXA Out = MCPWMXB Out = MCPWMXA In with rising edge delay as well as falling edge delay*/
    MCPWM_ACTIVE_RED_FED_FROM_PWMXB,    /*!<MCPWMXA Out = MCPWMXB Out = MCPWMXB In with rising edge delay as well as falling edge delay*/
    MCPWM_DEADTIME_TYPE_MAX,            /*!<Maximum number of supported dead time modes*/
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
    MCPWM_SELECT_CAP0, /*!<Select CAP0 as input*/
    MCPWM_SELECT_CAP1, /*!<Select CAP1 as input*/
    MCPWM_SELECT_CAP2, /*!<Select CAP2 as input*/
} mcpwm_capture_signal_t;

/**
 * @brief MCPWM capture channel ID alias
 */
typedef mcpwm_capture_signal_t mcpwm_capture_channel_id_t;

/**
 * @brief event data that will be passed into ISR callback
 */
typedef struct {
    mcpwm_capture_on_edge_t cap_edge;   /*!<Which signal edge is detected*/
    uint32_t cap_value;                 /*!<Corresponding timestamp when event occurs. Clock rate = APB(usually 80M)*/
} cap_event_data_t;

/**
 * @brief Type of capture event callback
 * @param mcpwm MCPWM unit(0-1)
 * @param cap_channel capture channel ID
 * @param edata Capture event data, contains capture edge and capture value, fed by the driver
 * @param user_data User registered data, passed from `mcpwm_capture_config_t`
 *
 * @note Since this an ISR callback so do not do anything that may block and call APIs that is designed to be used within ISR(usually has '_ISR' postfix)
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 *
 */
typedef bool (*cap_isr_cb_t)(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata,
                             void *user_data);

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
 * @brief MCPWM carrier configuration structure
 */
typedef struct {
    uint8_t carrier_period;                    /*!<Set carrier period = (carrier_period + 1)*800ns, carrier_period should be < 16*/
    uint8_t carrier_duty;                      /*!<Set carrier duty cycle, carrier_duty should be less than 8 (increment every 12.5%)*/
    uint8_t pulse_width_in_os;                 /*!<Set pulse width of first pulse in one shot mode = (carrier period)*(pulse_width_in_os + 1), should be less then 16*/
    mcpwm_carrier_out_ivt_t carrier_ivt_mode;  /*!<Invert output of carrier*/
} mcpwm_carrier_config_t;

/**
 * @brief MCPWM config capture structure
 */
typedef struct {
    mcpwm_capture_on_edge_t cap_edge;      /*!<Set capture edge*/
    uint32_t cap_prescale;                 /*!<Prescale of capture signal, ranging from 1 to 256*/
    cap_isr_cb_t capture_cb;               /*!<User defined capture event callback, running under interrupt context */
    void *user_data;                       /*!<User defined ISR callback function args*/
} mcpwm_capture_config_t;

/**
 * @brief MCPWM config sync structure
 */
typedef struct {
    mcpwm_sync_signal_t sync_sig;              /*!<Set sync input signal that will cause timer to sync*/
    uint32_t timer_val;                        /*!<Counter value to be set after sync, in 0 ~ 999, unit: 1 / 1000 * peak*/
    mcpwm_timer_direction_t count_direction;   /*!<Counting direction to be set after sync */
} mcpwm_sync_config_t;

#ifdef __cplusplus
}
#endif
