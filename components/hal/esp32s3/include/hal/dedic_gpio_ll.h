/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/system_struct.h"

static inline void _dedic_gpio_ll_enable_bus_clock(bool enable)
{
    SYSTEM.cpu_peri_clk_en.clk_en_dedicated_gpio = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define dedic_gpio_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _dedic_gpio_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

static inline void _dedic_gpio_ll_reset_register(void)
{
    SYSTEM.cpu_peri_rst_en.rst_en_dedicated_gpio = 1;
    SYSTEM.cpu_peri_rst_en.rst_en_dedicated_gpio = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define dedic_gpio_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _dedic_gpio_ll_reset_register(__VA_ARGS__); \
    } while(0)

#ifdef __cplusplus
}
#endif
