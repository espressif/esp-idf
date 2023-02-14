/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "riscv/csr.h"

/*fast gpio*/
#define CSR_GPIO_OEN_USER   0x803
#define CSR_GPIO_IN_USER    0x804
#define CSR_GPIO_OUT_USER   0x805

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline void dedic_gpio_cpu_ll_enable_output(uint32_t mask)
{
    RV_WRITE_CSR(CSR_GPIO_OEN_USER, mask);
}

static inline void dedic_gpio_cpu_ll_write_all(uint32_t value)
{
    RV_WRITE_CSR(CSR_GPIO_OUT_USER, value);
}

__attribute__((always_inline))
static inline uint32_t dedic_gpio_cpu_ll_read_in(void)
{
    uint32_t value = RV_READ_CSR(CSR_GPIO_IN_USER);
    return value;
}

__attribute__((always_inline))
static inline uint32_t dedic_gpio_cpu_ll_read_out(void)
{
    uint32_t value = RV_READ_CSR(CSR_GPIO_OUT_USER);
    return value;
}

__attribute__((always_inline))
static inline void dedic_gpio_cpu_ll_write_mask(uint32_t mask, uint32_t value)
{
    RV_SET_CSR(CSR_GPIO_OUT_USER, mask & value);
    RV_CLEAR_CSR(CSR_GPIO_OUT_USER, mask & ~(value));
}

#ifdef __cplusplus
}
#endif
