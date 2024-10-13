/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PCNT channel action on control level
 */
typedef enum {
    PCNT_CHANNEL_LEVEL_ACTION_KEEP,    /*!< Keep current count mode */
    PCNT_CHANNEL_LEVEL_ACTION_INVERSE, /*!< Invert current count mode (increase -> decrease, decrease -> increase) */
    PCNT_CHANNEL_LEVEL_ACTION_HOLD,    /*!< Hold current count value */
} pcnt_channel_level_action_t;

/**
 * @brief PCNT channel action on signal edge
 */
typedef enum {
    PCNT_CHANNEL_EDGE_ACTION_HOLD,     /*!< Hold current count value */
    PCNT_CHANNEL_EDGE_ACTION_INCREASE, /*!< Increase count value */
    PCNT_CHANNEL_EDGE_ACTION_DECREASE, /*!< Decrease count value */
} pcnt_channel_edge_action_t;

/**
 * @brief PCNT unit zero cross mode
 */
typedef enum {
    PCNT_UNIT_ZERO_CROSS_POS_ZERO, /*!< start from positive value, end to zero, i.e. +N->0 */
    PCNT_UNIT_ZERO_CROSS_NEG_ZERO, /*!< start from negative value, end to zero, i.e. -N->0 */
    PCNT_UNIT_ZERO_CROSS_NEG_POS,  /*!< start from negative value, end to positive value, i.e. -N->+M */
    PCNT_UNIT_ZERO_CROSS_POS_NEG,  /*!< start from positive value, end to negative value, i.e. +N->-M */
    PCNT_UNIT_ZERO_CROSS_INVALID,  /*!< invalid zero cross mode */
} pcnt_unit_zero_cross_mode_t;

#ifdef __cplusplus
}
#endif
