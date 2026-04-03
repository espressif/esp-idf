/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_chip_info.h"
#include "esp_private/esp_system_attr.h"
#include "esp_private/esp_int_wdt.h"
#include "esp_attr.h"

#define IWDT_LIVELOCK_TIMEOUT_MS    (20)

extern uint32_t _lx_intr_livelock_counter, _lx_intr_livelock_max;

void ESP_SYSTEM_IRAM_ATTR esp_int_wdt_reset_livelock_params(void)
{
    if (soc_has_cache_lock_bug()) {
        _lx_intr_livelock_counter = 0;
        _lx_intr_livelock_max = 0;
    }
}

_Static_assert((portTICK_PERIOD_MS << 1) <= IWDT_LIVELOCK_TIMEOUT_MS, "portTICK_PERIOD_MS must be less than or equal to IWDT_LIVELOCK_TIMEOUT_MS");
_Static_assert(CONFIG_ESP_INT_WDT_TIMEOUT_MS >= (IWDT_LIVELOCK_TIMEOUT_MS * 3), "CONFIG_ESP_INT_WDT_TIMEOUT_MS must be greater than or equal to IWDT_LIVELOCK_TIMEOUT_MS * 3");

ESP_SYSTEM_IRAM_ATTR void esp_int_wdt_set_livelock_params(uint32_t timeout_ms)
{
    _lx_intr_livelock_counter = 0;
    if (soc_has_cache_lock_bug()) {
        _lx_intr_livelock_max = timeout_ms / IWDT_LIVELOCK_TIMEOUT_MS - 1;
    }
}

uint32_t ESP_SYSTEM_IRAM_ATTR esp_int_wdt_livelock_get_feed_stage0_ticks(void)
{
    return IWDT_STAGE0_TIMEOUT_US / (_lx_intr_livelock_max + 1);
}

void ESP_SYSTEM_IRAM_ATTR esp_int_wdt_livelock_workaround(bool enable)
{
    uint32_t stage0_ticks = IWDT_STAGE0_TIMEOUT_US;
    if (enable) {
        esp_int_wdt_set_livelock_params(CONFIG_ESP_INT_WDT_TIMEOUT_MS);
        stage0_ticks = esp_int_wdt_livelock_get_feed_stage0_ticks();
    } else {
        esp_int_wdt_reset_livelock_params();
    }
    esp_int_wdt_reconfigure_ticks(stage0_ticks, IWDT_STAGE1_TIMEOUT_US);
}

void esp_int_wdt_init_for_livelock_fix(void)
{
#if CONFIG_BTDM_CTRL_HLI
#define APB_DCRSET      (0x200c)
#define APB_ITCTRL      (0x3f00)
#define ERI_ADDR(APB)   (0x100000 + (APB))
#define _SYM2STR(x)     # x
#define SYM2STR(x)      _SYM2STR(x)

    if (soc_has_cache_lock_bug()) {
        if (xPortGetCoreID() != CONFIG_BTDM_CTRL_PINNED_TO_CORE) {
            uint32_t eriadrs, scratch = 0, immediate = 0;
            __asm__ __volatile__(
                /* Enable Xtensa Debug Module Integration Mode */
                "movi   %[ERI], " SYM2STR(ERI_ADDR(APB_ITCTRL)) "\n"
                "rer    %[REG], %[ERI]\n"
                "movi   %[IMM], 1\n"
                "or     %[REG], %[IMM], %[REG]\n"
                "wer    %[REG], %[ERI]\n"
                /* Enable Xtensa Debug Module Break_In signal */
                "movi   %[ERI], " SYM2STR(ERI_ADDR(APB_DCRSET)) "\n"
                "rer    %[REG], %[ERI]\n"
                "movi   %[IMM], 0x10000\n"
                "or     %[REG], %[IMM], %[REG]\n"
                "wer    %[REG], %[ERI]\n"
                : [ERI] "=r"(eriadrs), [REG] "+r"(scratch), [IMM] "+r"(immediate)
            );
        }
    }
#endif // CONFIG_BTDM_CTRL_HLI
}
