/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <inttypes.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_log.h"

#if CONFIG_COMPILER_OPTIMIZATION_CHECKS_ADDRESS_ONLY

NOINLINE_ATTR void _esp_check_failed(const char *tag, esp_err_t rc)
{
    uintptr_t addr = (uintptr_t)esp_cpu_get_call_addr((intptr_t)__builtin_return_address(0));
    if (rc == ESP_OK) {
        ESP_LOGE(tag, "check failed at 0x%08" PRIxPTR, addr);
    } else {
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
        ESP_LOGE(tag, "check failed: esp_err_t 0x%x (%s) at 0x%08" PRIxPTR, rc, esp_err_to_name(rc), addr);
#else
        ESP_LOGE(tag, "check failed: esp_err_t 0x%x at 0x%08" PRIxPTR, rc, addr);
#endif
    }
}

NOINLINE_ATTR IRAM_ATTR void _esp_check_failed_isr(esp_err_t rc)
{
    uintptr_t addr = (uintptr_t)esp_cpu_get_call_addr((intptr_t)__builtin_return_address(0));
    // The caller's tag may be in flash, so a fixed one is used
    if (rc == ESP_OK) {
        ESP_DRAM_LOGE(DRAM_STR("esp_check"), "check failed at 0x%08" PRIxPTR, addr);
    } else {
        ESP_DRAM_LOGE(DRAM_STR("esp_check"), "check failed: esp_err_t 0x%x at 0x%08" PRIxPTR, rc, addr);
    }
}

#endif // CONFIG_COMPILER_OPTIMIZATION_CHECKS_ADDRESS_ONLY
