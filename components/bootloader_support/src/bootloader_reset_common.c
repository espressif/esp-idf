/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "soc/soc_caps.h"
#if SOC_CPU_LOCKUP_DEBUG_SUPPORTED
#include "hal/assist_debug_ll.h"
#include "esp_rom_caps.h"
#endif
#include "bootloader_init.h"

ESP_LOG_ATTR_TAG(TAG, "boot");

#if SOC_CPU_LOCKUP_DEBUG_SUPPORTED && !ESP_ROM_PRINTS_LOCKUP_STATUS
static void lockup_info_dump(int cpu)
{
    const char *cpu_name = cpu ? "APP" : "PRO";

    ESP_LOGW(TAG, "%s CPU reset due to CPU lockup (exception inside exception handler).", cpu_name);
    ESP_LOGW(TAG, "%s CPU trap chain:", cpu_name);
    for (int i = 0; i < 2; i++) {
        uint32_t cause = assist_debug_ll_lockup_get_cause(cpu, i);
        uint32_t tval  = assist_debug_ll_lockup_get_tval(cpu, i);
        uint32_t iaddr = assist_debug_ll_lockup_get_iaddr(cpu, i);
        uint32_t priv  = assist_debug_ll_lockup_get_priv(cpu, i);
        const char *label = i == 0 ? "latest" : "previous";

        ESP_LOGW(TAG, "  [%s trap] cause=0x%02"PRIx32" PCAddr=0x%08"PRIx32" tval=0x%08"PRIx32" priv=%"PRIu32,
                 label, cause, iaddr, tval, priv);
    }
}
#endif

void bootloader_check_reset(void)
{
    bool any_wdt_reset = false;
    soc_reset_reason_t reset_reason;

    reset_reason = esp_rom_get_reset_reason(0);
    any_wdt_reset |= bootloader_check_if_wdt_reset(0, reset_reason);
#if SOC_CPU_LOCKUP_DEBUG_SUPPORTED && !ESP_ROM_PRINTS_LOCKUP_STATUS
    if (reset_reason == RESET_REASON_CPU_LOCKUP) {
        lockup_info_dump(0);
    }
#endif

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    reset_reason = esp_rom_get_reset_reason(1);
    any_wdt_reset |= bootloader_check_if_wdt_reset(1, reset_reason);
#if SOC_CPU_LOCKUP_DEBUG_SUPPORTED && !ESP_ROM_PRINTS_LOCKUP_STATUS
    if (reset_reason == RESET_REASON_CPU_LOCKUP) {
        lockup_info_dump(1);
    }
#endif
#endif

    if (any_wdt_reset) {
        bootloader_dump_wdt_reset_info(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        bootloader_dump_wdt_reset_info(1);
#endif
    }

    bootloader_enable_cpu_reset_info();
}
