/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Regression guard for ESP_FAULT_ASSERT() being silently optimised away.
 *
 * ESP_FAULT_ASSERT(C) must emit three independent "evaluate C -> reset if false"
 * checks. When C is a value the optimiser can already prove (e.g. a flag pinned
 * by a preceding "if (!C) return"), a naive implementation lets GCC constant-fold
 * C and delete all three checks, removing the fault-injection protection with no
 * warning.
 *
 * The functions below place ESP_FAULT_ASSERT in exactly that
 * "proven-true cached value" shape. check_fault_asserts.py disassembles the built
 * app and fails the build if any of them lost its reset blocks. Keep them
 * noinline+used so each is an independent symbol the checker can find.
 */

#include <stdbool.h>
#include "esp_fault.h"
#include "esp_attr.h"

/* volatile so the initial value is opaque: only the early-return "proves" it,
 * which is the precise condition that triggers the optimiser elimination. */
volatile bool fa_test_flag = true;
volatile int  fa_test_status = 0;
volatile int  fa_test_sink;

/* Cached bool guarded by an early return -> the original elimination case. */
bool NOINLINE_ATTR test_fa_guarded_flag(void)
{
    bool valid = fa_test_flag;
    if (!valid) {
        return false;
    }
    ESP_FAULT_ASSERT(valid);
    return true;
}

/* Cached status compared to a constant, guarded by an early return. */
int NOINLINE_ATTR test_fa_guarded_status(void)
{
    int status = fa_test_status;
    if (status != 0) {
        return status;
    }
    ESP_FAULT_ASSERT(status == 0);
    return 0;
}

void app_main(void)
{
    /* Reference the test functions so they are linked (not GC'd). */
    fa_test_sink = (int)test_fa_guarded_flag() + test_fa_guarded_status();
}
