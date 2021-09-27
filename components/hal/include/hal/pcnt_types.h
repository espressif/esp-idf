// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
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

/**
 * @brief PCNT channel action on control level
 *
 */
typedef enum {
    PCNT_CHANNEL_LEVEL_ACTION_KEEP,    /*!< Keep current count mode */
    PCNT_CHANNEL_LEVEL_ACTION_INVERSE, /*!< Invert current count mode (increase -> decrease, decrease -> increase) */
    PCNT_CHANNEL_LEVEL_ACTION_HOLD,    /*!< Hold current count value */
} pcnt_channel_level_action_t;

/**
 * @brief PCNT channel action on signal edge
 *
 */
typedef enum {
    PCNT_CHANNEL_EDGE_ACTION_HOLD,     /*!< Hold current count value */
    PCNT_CHANNEL_EDGE_ACTION_INCREASE, /*!< Increase count value */
    PCNT_CHANNEL_EDGE_ACTION_DECREASE, /*!< Decrease count value */
} pcnt_channel_edge_action_t;

/**
 * @brief PCNT unit counter value's sign
 *
 */
typedef enum {
    PCNT_UNIT_COUNT_SIGN_ZERO_POS, /*!< positive value to zero */
    PCNT_UNIT_COUNT_SIGN_ZERO_NEG, /*!< negative value to zero */
    PCNT_UNIT_COUNT_SIGN_NEG,      /*!< counter value negative */
    PCNT_UNIT_COUNT_SIGN_POS,      /*!< counter value positive */
} pcnt_unit_count_sign_t;

#ifdef __cplusplus
}
#endif
