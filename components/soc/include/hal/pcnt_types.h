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

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/pcnt_caps.h"

#define PCNT_PIN_NOT_USED     (-1)  /*!< When selected for a pin, this pin will not be used */

/**
 * @brief PCNT port number, the max port number is (PCNT_PORT_MAX - 1).
 */
typedef enum {
    PCNT_PORT_0 = 0,                 /*!< PCNT port 0 */
    PCNT_PORT_MAX,                   /*!< PCNT port max */
} pcnt_port_t;

/**
 * @brief Selection of all available PCNT units
 */
typedef enum {
    PCNT_UNIT_0 = 0,                 /*!< PCNT unit 0 */
    PCNT_UNIT_1 = 1,                 /*!< PCNT unit 1 */
    PCNT_UNIT_2 = 2,                 /*!< PCNT unit 2 */
    PCNT_UNIT_3 = 3,                 /*!< PCNT unit 3 */
#if SOC_PCNT_UNIT_NUM > 4
    PCNT_UNIT_4 = 4,                 /*!< PCNT unit 4 */
    PCNT_UNIT_5 = 5,                 /*!< PCNT unit 5 */
    PCNT_UNIT_6 = 6,                 /*!< PCNT unit 6 */
    PCNT_UNIT_7 = 7,                 /*!< PCNT unit 7 */
#endif
    PCNT_UNIT_MAX,
} pcnt_unit_t;

/**
 * @brief Selection of available modes that determine the counter's action depending on the state of the control signal's input GPIO
 * @note  Configuration covers two actions, one for high, and one for low level on the control input
 */
typedef enum {
    PCNT_MODE_KEEP = 0,             /*!< Control mode: won't change counter mode*/
    PCNT_MODE_REVERSE = 1,          /*!< Control mode: invert counter mode(increase -> decrease, decrease -> increase) */
    PCNT_MODE_DISABLE = 2,          /*!< Control mode: Inhibit counter(counter value will not change in this condition) */
    PCNT_MODE_MAX
} pcnt_ctrl_mode_t;

/**
 * @brief Selection of available modes that determine the counter's action on the edge of the pulse signal's input GPIO
 * @note  Configuration covers two actions, one for positive, and one for negative edge on the pulse input
 */
typedef enum {
    PCNT_COUNT_DIS = 0,            /*!< Counter mode: Inhibit counter(counter value will not change in this condition) */
    PCNT_COUNT_INC = 1,            /*!< Counter mode: Increase counter value */
    PCNT_COUNT_DEC = 2,            /*!< Counter mode: Decrease counter value */
    PCNT_COUNT_MAX
} pcnt_count_mode_t;

/**
 * @brief Selection of channels available for a single PCNT unit
 */
typedef enum {
    PCNT_CHANNEL_0 = 0x00,           /*!< PCNT channel 0 */
    PCNT_CHANNEL_1 = 0x01,           /*!< PCNT channel 1 */
    PCNT_CHANNEL_MAX,
} pcnt_channel_t;

/**
 * @brief Selection of counter's events the may trigger an interrupt
 */
typedef enum {
    PCNT_EVT_THRES_1 = BIT(2),           /*!< PCNT watch point event: threshold1 value event */
    PCNT_EVT_THRES_0 = BIT(3),           /*!< PCNT watch point event: threshold0 value event */
    PCNT_EVT_L_LIM = BIT(4),             /*!< PCNT watch point event: Minimum counter value */
    PCNT_EVT_H_LIM = BIT(5),             /*!< PCNT watch point event: Maximum counter value */
    PCNT_EVT_ZERO = BIT(6),              /*!< PCNT watch point event: counter value zero event */
    PCNT_EVT_MAX
} pcnt_evt_type_t;

/**
 * @brief Pulse Counter configuration for a single channel
 */
typedef struct {
    int pulse_gpio_num;             /*!< Pulse input GPIO number, if you want to use GPIO16, enter pulse_gpio_num = 16, a negative value will be ignored */
    int ctrl_gpio_num;              /*!< Control signal input GPIO number, a negative value will be ignored */
    pcnt_ctrl_mode_t lctrl_mode;    /*!< PCNT low control mode */
    pcnt_ctrl_mode_t hctrl_mode;    /*!< PCNT high control mode */
    pcnt_count_mode_t pos_mode;     /*!< PCNT positive edge count mode */
    pcnt_count_mode_t neg_mode;     /*!< PCNT negative edge count mode */
    int16_t counter_h_lim;          /*!< Maximum counter value */
    int16_t counter_l_lim;          /*!< Minimum counter value */
    pcnt_unit_t unit;               /*!< PCNT unit number */
    pcnt_channel_t channel;         /*!< the PCNT channel */
} pcnt_config_t;

#ifdef __cplusplus
}
#endif