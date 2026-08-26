/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>

/**
 * Minimal baseline integration app for size metrics.
 *
 * This app exercises only the absolute minimum SDK stack - app_main + esp_system.
 * It establishes a floor measurement for the core SDK and catches regressions in:
 * - Toolchain changes
 * - newlib updates
 * - Linker script changes
 * - Core esp_system dependencies
 *
 * This app keeps runtime behavior minimal and source-frozen.
 * Any size change is attributable to SDK changes, not application code changes.
 */

void app_main(void)
{
    printf("minimal_baseline app_main started\n");
}
