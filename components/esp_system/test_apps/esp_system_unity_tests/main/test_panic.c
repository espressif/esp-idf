/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdbool.h>
#include "sdkconfig.h"
#include "unity.h"
#include "soc/soc_caps.h"

#if SOC_CPU_LOCKUP_DEBUG_SUPPORTED

static bool s_trigger_lockup_in_panic;

/* __wrap_esp_panic_handler intercepts all panics in this test binary.
 * Normally it calls through to the real handler. When s_trigger_lockup_in_panic
 * is set it causes a second exception inside the handler, which triggers a
 * hardware CPU lockup reset.
 */
void __real_esp_panic_handler(void *info);
void __wrap_esp_panic_handler(void *info)
{
    if (s_trigger_lockup_in_panic) {
        __asm__ volatile("unimp");  /* exception inside exception handler -> lockup */
    }
    __real_esp_panic_handler(info);
}

TEST_CASE("CPU lockup output", "[cpu_lockup][ignore]")
{
    s_trigger_lockup_in_panic = true;
    __asm__ volatile("unimp");  /* first exception -> panic handler -> second exception -> lockup */
}

#endif // SOC_CPU_LOCKUP_DEBUG_SUPPORTED
