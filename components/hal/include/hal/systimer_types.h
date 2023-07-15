/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief The structure of the counter value in systimer
 *
 */
#if SOC_SYSTIMER_SUPPORTED
typedef struct {
    union {
        struct {
            uint64_t lo : SOC_SYSTIMER_BIT_WIDTH_LO; /*!< Low part of counter value */
            uint64_t hi : SOC_SYSTIMER_BIT_WIDTH_HI; /*!< High part of counter value */
#if (SOC_SYSTIMER_BIT_WIDTH_LO  + SOC_SYSTIMER_BIT_WIDTH_HI) < 64
            uint64_t reserved: (64 - (SOC_SYSTIMER_BIT_WIDTH_LO  + SOC_SYSTIMER_BIT_WIDTH_HI));
#endif
        };
        uint64_t val; /*!< counter value */
    };
} systimer_counter_value_t;


/** @cond */
ESP_STATIC_ASSERT(sizeof(systimer_counter_value_t) == 8, "systimer_counter_value_t should occupy 8 bytes in memory");
/** @endcond */

#endif

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
