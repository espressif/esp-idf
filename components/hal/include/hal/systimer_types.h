// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include "soc/soc_caps.h"

/*
 * @brief The structure of the counter value in systimer
 *
 */
typedef struct {
    union {
        struct {
            uint64_t lo : SOC_SYSTIMER_BIT_WIDTH_LO; /*!< Low part of counter value */
            uint64_t hi : SOC_SYSTIMER_BIT_WIDTH_HI; /*!< High part of counter value */
        };
        uint64_t val; /*!< counter value */
    };
} systimer_counter_value_t;

/** @cond */
_Static_assert(sizeof(systimer_counter_value_t) == 8, "systimer_counter_value_t should occupy 8 bytes in memory");
/** @endcond */

/**
 * @brief systimer counter ID
 *
 */
typedef enum {
    SYSTIMER_COUNTER_0, /*!< systimer counter 0 */
#if SOC_SYSTIMER_COUNTER_NUM > 1
    SYSTIMER_COUNTER_1, /*!< systimer counter 1 */
#endif
} systimer_counter_id_t;

/**
 * @brief systimer alarm ID
 *
 */
typedef enum {
    SYSTIMER_ALARM_0, /*!< systimer alarm 0 */
    SYSTIMER_ALARM_1, /*!< systimer alarm 1 */
    SYSTIMER_ALARM_2, /*!< systimer alarm 2 */
} systimer_alarm_id_t;

/**
 * @brief systimer alarm mode
 *
 */
typedef enum {
    SYSTIMER_ALARM_MODE_ONESHOT, /*!< systimer alarm oneshot mode */
    SYSTIMER_ALARM_MODE_PERIOD,  /*!< systimer alarm period mode */
} systimer_alarm_mode_t;

#ifdef __cplusplus
}
#endif
