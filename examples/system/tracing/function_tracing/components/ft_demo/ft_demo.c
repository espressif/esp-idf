/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Function Tracing Example - demo library
*/

#include "ft_demo.h"

/* noinline keeps each function as a distinct call so the compiler inserts
 * enter/exit hooks instead of inlining them away. */
static uint32_t __attribute__((noinline)) ft_demo_level2(uint32_t v)
{
    return v * 3u + 1u;
}

static uint32_t __attribute__((noinline)) ft_demo_level1(uint32_t v)
{
    return ft_demo_level2(v) + ft_demo_level2(v + 1u);
}

void ft_demo_run(uint32_t iteration)
{
    volatile uint32_t r = ft_demo_level1(iteration);
    (void)r;
}

void __attribute__((noinline)) ft_demo_secret(void)
{
    /* Excluded from instrumentation by name, so no enter/exit is recorded even
     * though this file is compiled with -finstrument-functions. */
    __asm__ volatile("");
}
