/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Function Tracing Example - demo library (second excluded file)
*/

#include "ft_demo.h"

/* A second file added to the component's -finstrument-functions-exclude-file-list
 * as a separate, comma-separated entry. It shows the exclude list accepts more
 * than one substring. Like ft_demo_hot.c, none of its functions are instrumented. */
uint32_t ft_demo_quiet_path(uint32_t n)
{
    uint32_t acc = 1;
    for (uint32_t i = 1; i <= n; i++) {
        acc = (acc * i) % 1000u;
    }
    return acc;
}
