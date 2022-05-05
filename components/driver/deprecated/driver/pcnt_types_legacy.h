/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_intr_alloc.h"
#include "soc/soc_caps.h"
#include "hal/pcnt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCNT_PIN_NOT_USED     (-1)  /*!< When selected for a pin, this pin will not be used */

/**
 * @brief PCNT interrupt handle
 */
typedef intr_handle_t pcnt_isr_handle_t;

/**
 * @brief PCNT port number, the max port number is (PCNT_PORT_MAX - 1).
 */
typedef enum {
    PCNT_PORT_0,   /*!< PCNT port 0 */
    PCNT_PORT_MAX, /*!< PCNT port max */
} pcnt_port_t;

/**
 * @brief Selection of all available PCNT units
 */
typedef enum {
    PCNT_UNIT_0, /*!< PCNT unit 0 */
    PCNT_UNIT_1, /*!< PCNT unit 1 */
    PCNT_UNIT_2, /*!< PCNT unit 2 */
    PCNT_UNIT_3, /*!< PCNT unit 3 */
#if SOC_PCNT_UNITS_PER_GROUP > 4
    PCNT_UNIT_4, /*!< PCNT unit 4 */
    PCNT_UNIT_5, /*!< PCNT unit 5 */
    PCNT_UNIT_6, /*!< PCNT unit 6 */
    PCNT_UNIT_7, /*!< PCNT unit 7 */
#endif
    PCNT_UNIT_MAX,
} pcnt_unit_t;

/**
 * @brief Selection of channels available for a single PCNT unit
 */
typedef enum {
    PCNT_CHANNEL_0, /*!< PCNT channel 0 */
    PCNT_CHANNEL_1, /*!< PCNT channel 1 */
    PCNT_CHANNEL_MAX,
} pcnt_channel_t;

/**
 * @brief Selection of counter's events the may trigger an interrupt
 */
typedef enum {
    PCNT_EVT_THRES_1 = 1 << 2, /*!< PCNT watch point event: threshold1 value event */
    PCNT_EVT_THRES_0 = 1 << 3, /*!< PCNT watch point event: threshold0 value event */
    PCNT_EVT_L_LIM = 1 << 4,   /*!< PCNT watch point event: Minimum counter value */
    PCNT_EVT_H_LIM = 1 << 5,   /*!< PCNT watch point event: Maximum counter value */
    PCNT_EVT_ZERO = 1 << 6,    /*!< PCNT watch point event: counter value zero event */
    PCNT_EVT_MAX
} pcnt_evt_type_t;

/**
 * @brief Selection of available modes that determine the counter's action depending on the state of the control signal's input GPIO
 * @note  Configuration covers two actions, one for high, and one for low level on the control input
 */
typedef pcnt_channel_level_action_t pcnt_ctrl_mode_t;
#define PCNT_MODE_KEEP    PCNT_CHANNEL_LEVEL_ACTION_KEEP    /*!< Control mode: won't change counter mode*/
#define PCNT_MODE_REVERSE PCNT_CHANNEL_LEVEL_ACTION_INVERSE /*!< Control mode: invert counter mode(increase -> decrease, decrease -> increase) */
#define PCNT_MODE_DISABLE PCNT_CHANNEL_LEVEL_ACTION_HOLD    /*!< Control mode: Inhibit counter(counter value will not change in this condition) */
#define PCNT_MODE_MAX     3

/**
 * @brief Selection of available modes that determine the counter's action on the edge of the pulse signal's input GPIO
 * @note  Configuration covers two actions, one for positive, and one for negative edge on the pulse input
 */
typedef pcnt_channel_edge_action_t pcnt_count_mode_t;
#define PCNT_COUNT_DIS PCNT_CHANNEL_EDGE_ACTION_HOLD     /*!< Counter mode: Inhibit counter(counter value will not change in this condition) */
#define PCNT_COUNT_INC PCNT_CHANNEL_EDGE_ACTION_INCREASE /*!< Counter mode: Increase counter value */
#define PCNT_COUNT_DEC PCNT_CHANNEL_EDGE_ACTION_DECREASE /*!< Counter mode: Decrease counter value */
#define PCNT_COUNT_MAX 3

/**
 * @brief Pulse Counter configuration for a single channel
 */
typedef struct {
    int pulse_gpio_num;          /*!< Pulse input GPIO number, if you want to use GPIO16, enter pulse_gpio_num = 16, a negative value will be ignored */
    int ctrl_gpio_num;           /*!< Control signal input GPIO number, a negative value will be ignored */
    pcnt_ctrl_mode_t lctrl_mode; /*!< PCNT low control mode */
    pcnt_ctrl_mode_t hctrl_mode; /*!< PCNT high control mode */
    pcnt_count_mode_t pos_mode;  /*!< PCNT positive edge count mode */
    pcnt_count_mode_t neg_mode;  /*!< PCNT negative edge count mode */
    int16_t counter_h_lim;       /*!< Maximum counter value */
    int16_t counter_l_lim;       /*!< Minimum counter value */
    pcnt_unit_t unit;            /*!< PCNT unit number */
    pcnt_channel_t channel;      /*!< the PCNT channel */
} pcnt_config_t;

#ifdef __cplusplus
}
#endif
