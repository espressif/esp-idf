/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Function Tracing Example - demo library
*/
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Runs a small nested call graph (ft_demo_run -> level1 -> level2). These
 * functions are instrumented and appear as function_enter/function_exit. */
void ft_demo_run(uint32_t iteration);

/* Instrumented file, but excluded by name through the component's
 * -finstrument-functions-exclude-function-list flag: it produces no events. */
void ft_demo_secret(void);

/* Lives in ft_demo_hot.c, which is excluded as a whole through the component's
 * -finstrument-functions-exclude-file-list flag: it produces no events. */
uint32_t ft_demo_hot_loop(uint32_t n);

/* Lives in ft_demo_quiet.c, a second comma-separated entry in the component's
 * -finstrument-functions-exclude-file-list flag: it also produces no events. */
uint32_t ft_demo_quiet_path(uint32_t n);

#ifdef __cplusplus
}
#endif
