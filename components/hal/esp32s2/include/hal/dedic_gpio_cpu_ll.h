/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline uint32_t dedic_gpio_cpu_ll_read_in(void)
{
    uint32_t value = 0;
    asm volatile("get_gpio_in %0" : "=r"(value) : :);
    return value;
}

__attribute__((always_inline))
static inline uint32_t dedic_gpio_cpu_ll_read_out(void)
{
    uint32_t value = 0;
    asm volatile("rur.gpio_out %0" : "=r"(value) : :);
    return value;
}

__attribute__((always_inline))
static inline void dedic_gpio_cpu_ll_write_all(uint32_t value)
{
    asm volatile("wur.gpio_out %0"::"r"(value):);
}

__attribute__((always_inline))
static inline void dedic_gpio_cpu_ll_write_mask(uint32_t mask, uint32_t value)
{
    asm volatile("wr_mask_gpio_out %0, %1" : : "r"(value), "r"(mask):);
}

#ifdef __cplusplus
}
#endif
