/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Host tests for the Linux WDT simulator (wdt_simulator.c).
 *
 * Both watchdogs are enabled in a single build.  The pytest selects each
 * test case from the Unity menu in turn.  Each test triggers a starvation
 * scenario: the WDT monitor pthread detects starvation, prints a marker
 * string, and calls esp_restart() (which is exit(0) on Linux).  The
 * pytest validates the expected marker.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_runner.h"

TEST_CASE("int_wdt_fires_on_tick_starvation", "[wdt]")
{
    /*
     * Trigger the interrupt watchdog.
     *
     * vPortEnterCritical() holds the port mutex, which prevents the
     * scheduler pthread from incrementing ticks.  The tick hook that feeds
     * the interrupt WDT never runs, and the WDT monitor pthread detects
     * starvation after CONFIG_ESP_INT_WDT_TIMEOUT_MS.
     */
    printf("Entering critical section to starve interrupt WDT\n");
    vPortEnterCritical();
    while (1) {
    }
    /* Never reached — WDT fires and calls esp_restart(). */
}

TEST_CASE("task_wdt_fires_on_idle_starvation", "[wdt]")
{
    /*
     * Trigger the task watchdog.
     *
     * Busy-looping prevents the idle task from running (any priority > 0
     * is enough).  The idle hook that feeds the task WDT never fires.
     * Ticks are NOT blocked (no critical section) so the interrupt WDT
     * stays fed because the tick hook still runs.
     */
    printf("Busy-looping to starve idle task (task WDT)\n");
    while (1) {
    }
    /* Never reached — WDT fires and calls esp_restart(). */
}

void app_main(void)
{
    printf("Running WDT linux host tests\n");
    unity_run_menu();
}
