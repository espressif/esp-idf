/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "spi_flash_mmap.h"
#include "esp_private/cache_utils.h"

#include "esp_rom_sys.h"

#ifndef CONFIG_IDF_TARGET_LINUX
    #include "esp_cpu.h"
#else
    /* esp_cpu.h isn't available when building for Linux */
    static intptr_t esp_cpu_get_call_addr(intptr_t return_address)
    {
        /* on x86, there is no hope to get the address of the previous instruction */
        return return_address;
    }
#endif

static void esp_error_check_failed_print(const char *msg, esp_err_t rc, const char *file, int line, const char *function, const char *expression, intptr_t addr)
{
    esp_rom_printf("%s failed: esp_err_t 0x%x", msg, rc);
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    esp_rom_printf(" (%s)", esp_err_to_name(rc));
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP
    esp_rom_printf(" at 0x%08x\n", esp_cpu_get_call_addr(addr));
#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    if (spi_flash_cache_enabled())  // strings may be in flash cache
#endif
    {
        esp_rom_printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n", file, line, function, expression);
    }
}

void _esp_error_check_failed_without_abort(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    esp_error_check_failed_print("ESP_ERROR_CHECK_WITHOUT_ABORT", rc, file, line, function, expression, (intptr_t)__builtin_return_address(0));
}

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    esp_error_check_failed_print("ESP_ERROR_CHECK", rc, file, line, function, expression, (intptr_t)__builtin_return_address(0));
    abort();
}
