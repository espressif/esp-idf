/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert ticks to microseconds
 *
 * @param ticks ticks to convert
 * @return microseconds
 */
uint64_t systimer_ticks_to_us(uint64_t ticks) __attribute__((const));

/**
 * @brief Convert microseconds to ticks
 *
 * @param us microseconds to convert
 * @return ticks
 */
uint64_t systimer_us_to_ticks(uint64_t us) __attribute__((const));

#ifdef __cplusplus
}
#endif
