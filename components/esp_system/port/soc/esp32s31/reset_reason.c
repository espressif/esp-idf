/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_system.h"
#include "esp_rom_sys.h"
#include "esp_private/system_internal.h"
#include "soc/rtc_periph.h"
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"
#include "esp32s31/rom/rtc.h"

// TODO: ["ESP32S31"] IDF-14672

static void esp_reset_reason_clear_hint(void);

static esp_reset_reason_t s_reset_reason;

static esp_reset_reason_t get_reset_reason(soc_reset_reason_t rtc_reset_reason, esp_reset_reason_t reset_reason_hint)
{
    switch (rtc_reset_reason) {
    case RESET_REASON_CHIP_POWER_ON:
        return ESP_RST_POWERON;

    case RESET_REASON_CPU0_SW:
    case RESET_REASON_CORE_SW:
        if (reset_reason_hint == ESP_RST_PANIC ||
                reset_reason_hint == ESP_RST_BROWNOUT ||
                reset_reason_hint == ESP_RST_TASK_WDT ||
                reset_reason_hint == ESP_RST_INT_WDT) {
            return reset_reason_hint;
        }
        return ESP_RST_SW;

    case RESET_REASON_CORE_PMU_PWR_DOWN:
        return ESP_RST_DEEPSLEEP;

    case RESET_REASON_CPU_MWDT:
    case RESET_REASON_CPU_RWDT:
    case RESET_REASON_SYS_SUPER_WDT:
    case RESET_REASON_SYS_RWDT:
    case RESET_REASON_CORE_MWDT:
    case RESET_REASON_CORE_RWDT:
        /* Code is the same for INT vs Task WDT */
        return ESP_RST_WDT;

    case RESET_REASON_SYS_BROWN_OUT:
        return ESP_RST_BROWNOUT;

    case RESET_REASON_CORE_USB_UART:
    case RESET_REASON_CORE_USB_JTAG:
        return ESP_RST_USB;

    case RESET_REASON_CPU_JTAG:
        return ESP_RST_JTAG;

    case RESET_REASON_CPU_LOCKUP:
        return ESP_RST_CPU_LOCKUP;

    case RESET_REASON_CORE_EFUSE_CRC:
        return ESP_RST_EFUSE;

    case RESET_REASON_CORE_PWR_GLITCH:
        return ESP_RST_PWR_GLITCH;

    default:
        return ESP_RST_UNKNOWN;
    }
}

static void __attribute__((constructor)) esp_reset_reason_init(void)
{
    esp_reset_reason_t hint = esp_reset_reason_get_hint();
    s_reset_reason = get_reset_reason(esp_rom_get_reset_reason(PRO_CPU_NUM), hint);
    if (hint != ESP_RST_UNKNOWN) {
        esp_reset_reason_clear_hint();
    }
}

esp_reset_reason_t esp_reset_reason(void)
{
    return s_reset_reason;
}

/* Reset reason hint is stored in RTC_RESET_CAUSE_REG, a.k.a. RTC_CNTL_STORE6_REG,
 * a.k.a. RTC_ENTRY_ADDR_REG. It is safe to use this register both for the
 * deep sleep wake stub entry address and for reset reason hint, since wake stub
 * is only used for deep sleep reset, and in this case the reason provided by
 * esp_rom_get_reset_reason is unambiguous.
 *
 * Same layout is used as for RTC_APB_FREQ_REG (a.k.a. RTC_CNTL_STORE5_REG):
 * the value is replicated in low and high half-words. In addition to that,
 * MSB is set to 1, which doesn't happen when RTC_CNTL_STORE6_REG contains
 * deep sleep wake stub address.
 */

/* in IRAM, can be called from panic handler */
void esp_reset_reason_set_hint(esp_reset_reason_t hint)
{
    // TODO: ["ESP32S31"] IDF-14672
}

esp_reset_reason_t esp_reset_reason_get_hint(void)
{
    return 0; // TODO: ["ESP32S31"] IDF-14672
}
static inline void esp_reset_reason_clear_hint(void)
{
    // TODO: ["ESP32S31"] IDF-14672
}
