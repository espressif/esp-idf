/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "soc/soc_caps.h"

/**
 * @brief The structure of the counter value in lower power timer
 */
#if SOC_LP_TIMER_SUPPORTED
typedef struct {
    union {
        struct {
            uint32_t lo: SOC_LP_TIMER_BIT_WIDTH_LO;    /*!< Low part of counter value */
            uint32_t hi: SOC_LP_TIMER_BIT_WIDTH_HI;    /*!< High part of counter value */
            uint32_t reserved: 16;
        };
        uint64_t val; /*!< counter value */
    };
} lp_timer_counter_value_t;
#endif

#ifdef __cplusplus
}
#endif
