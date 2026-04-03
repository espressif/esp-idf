/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file defines system init functions whose execution is verified
 * by the test cases in test_sys_init_fn.c.
 *
 * The functions are placed into the esp_sys_init_fn linker section via the
 * system init macro.  On Linux this uses ELF section sorting; on macOS
 * the same section is resolved at runtime via getsectiondata().
 *
 * Important: this file must be compiled into the same binary as the test
 * runner so the linker/loader sees the section entries.
 */

#include "esp_private/startup_internal.h"
#include "test_init_fn_defs.h"

/* ---- Global state read back by the test cases ---- */

int  trace_log[INIT_FN_TRACE_MAX];
int  trace_count = 0;

bool core_prio_200_executed      = false;
bool core_prio_250_executed      = false;
bool secondary_prio_200_executed = false;
bool secondary_prio_250_executed = false;

/* ---- Helper ---- */
static void trace_append(int tag)
{
    if (trace_count < INIT_FN_TRACE_MAX) {
        trace_log[trace_count++] = tag;
    }
}

/* ---- CORE-stage init functions (executed during do_core_init) ---- */

/*
 * Two CORE-stage functions with different priorities.
 * Priority 200 must execute before priority 250.
 * We use tag values equal to the priority for easy identification.
 */
ESP_SYSTEM_INIT_FN(test_core_prio_200, CORE, BIT(0), 200)
{
    core_prio_200_executed = true;
    trace_append(200);
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(test_core_prio_250, CORE, BIT(0), 250)
{
    core_prio_250_executed = true;
    trace_append(250);
    return ESP_OK;
}

/* ---- SECONDARY-stage init functions (executed during do_secondary_init) ---- */

ESP_SYSTEM_INIT_FN(test_secondary_prio_200, SECONDARY, BIT(0), 200)
{
    secondary_prio_200_executed = true;
    trace_append(1200);   /* offset by 1000 so we can distinguish stage in trace */
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(test_secondary_prio_250, SECONDARY, BIT(0), 250)
{
    secondary_prio_250_executed = true;
    trace_append(1250);
    return ESP_OK;
}
