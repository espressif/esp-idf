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

#pragma once

/// Interrupts for MCPWM
typedef enum {
    MCPWM_LL_INTR_CAP0 = BIT(27), ///< Capture 0 happened
    MCPWM_LL_INTR_CAP1 = BIT(28), ///< Capture 1 happened
    MCPWM_LL_INTR_CAP2 = BIT(29), ///< Capture 2 happened
} mcpwm_intr_t;

/**
 * @brief Select type of MCPWM counter
 */
typedef enum {
    MCPWM_UP_COUNTER = 1,   /*!<For asymmetric MCPWM*/
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
 * @brief MCPWM select action to be taken on the output when event happens
 */
typedef enum {
    MCPWM_ACTION_NO_CHANGE = 0,  /*!<No change in the output*/
    MCPWM_ACTION_FORCE_LOW,      /*!<Make output low*/
    MCPWM_ACTION_FORCE_HIGH,     /*!<Make output high*/
    MCPWM_ACTION_TOGGLE,         /*!<Make output toggle*/
} mcpwm_output_action_t;

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
 * @brief MCPWM select sync signal input
 */
typedef enum {
    MCPWM_SELECT_SYNC0 = 4,  /*!<Select SYNC0 as input*/
    MCPWM_SELECT_SYNC1,      /*!<Select SYNC1 as input*/
    MCPWM_SELECT_SYNC2,      /*!<Select SYNC2 as input*/
} mcpwm_sync_signal_t;

/**
 * @brief MCPWM select capture starts from which edge
 */
typedef enum {
    MCPWM_NEG_EDGE = BIT(0),          /*!<Capture the negative edge*/
    MCPWM_POS_EDGE = BIT(1),          /*!<Capture the positive edge*/
    MCPWM_BOTH_EDGE = BIT(1)|BIT(0),  /*!<Capture both edges*/
} mcpwm_capture_on_edge_t;
