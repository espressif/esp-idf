/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#define TEST_MAIN_TASK_PRIO 6

/* Memory shared between the U-mode code and the tests, passed as arg1 */
typedef struct {
    volatile uint32_t counter;
    volatile uint32_t stop;
    volatile uint32_t out;
} privilege_shared_t;

/* Functions running in U-mode, they must not call any other function */

/* Stores arg0 * 3 in shared->out, and returns arg0 + 1 */
int privilege_arith(uint32_t arg0, uint32_t arg1);

/* Returns the stack pointer the code runs with */
int privilege_get_sp(uint32_t arg0, uint32_t arg1);

/* Returns the global pointer the code starts with */
int privilege_get_gp(uint32_t arg0, uint32_t arg1);

/* Increments shared->counter until shared->stop is set, returns the final counter value */
int privilege_count_until_stop(uint32_t arg0, uint32_t arg1);
