/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Stub for spi_flash_is_octal_mode_enabled() when building for non-OS contexts
 * (bootloader, TEE, PURE_RAM_APP). Full flash_ops.c is not built there, but
 * esp_hw_support may still reference this symbol; returning false is safe.
 */

#include <stdbool.h>

bool spi_flash_is_octal_mode_enabled(void)
{
    return false;
}
