/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Function Tracing Example - demo library (excluded file)
*/

#include "ft_demo.h"

/* This whole file is listed in the component's -finstrument-functions-exclude-file-list
 * flag, so none of its functions are instrumented even though the component opts
 * in. Use this for hot paths you do not want to trace. */
uint32_t ft_demo_hot_loop(uint32_t n)
{
    uint32_t acc = 0;
    for (uint32_t i = 0; i < n; i++) {
        acc += i * i;
    }
    return acc;
}
