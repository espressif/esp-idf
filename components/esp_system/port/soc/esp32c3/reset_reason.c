/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_system.h"
#include "esp_rom_sys.h"
#include "esp_private/system_internal.h"
#include "soc/rtc_periph.h"
#include "esp32c3/rom/rtc.h"

static void esp_reset_reason_clear_hint(void);

static esp_reset_reason_t s_reset_reason;

static esp_reset_reason_t get_reset_reason(soc_reset_reason_t rtc_reset_reason, esp_reset_reason_t reset_reason_hint)
{
    switch (rtc_reset_reason) {
    case RESET_REASON_CHIP_POWER_ON:
#if SOC_EFUSE_HAS_EFUSE_RST_BUG
    case RESET_REASON_CORE_EFUSE_CRC:
#endif
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

    case RESET_REASON_CORE_DEEP_SLEEP:
        return ESP_RST_DEEPSLEEP;

    case RESET_REASON_CORE_MWDT0:
        return ESP_RST_TASK_WDT;

    case RESET_REASON_CORE_MWDT1:
        return ESP_RST_INT_WDT;

    case RESET_REASON_CORE_RTC_WDT:
    case RESET_REASON_SYS_RTC_WDT:
    case RESET_REASON_SYS_SUPER_WDT:
    case RESET_REASON_CPU0_RTC_WDT:
    case RESET_REASON_CPU0_MWDT0:
    case RESET_REASON_CPU0_MWDT1:
        return ESP_RST_WDT;

    case RESET_REASON_SYS_BROWN_OUT:
        return ESP_RST_BROWNOUT;

    case RESET_REASON_CORE_USB_UART:
    case RESET_REASON_CORE_USB_JTAG:
        return ESP_RST_USB;

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

#define RST_REASON_BIT  0x80000000
#define RST_REASON_MASK 0x7FFF
#define RST_REASON_SHIFT 16

/* in IRAM, can be called from panic handler */
void IRAM_ATTR esp_reset_reason_set_hint(esp_reset_reason_t hint)
{
    assert((hint & (~RST_REASON_MASK)) == 0);
    uint32_t val = hint | (hint << RST_REASON_SHIFT) | RST_REASON_BIT;
    REG_WRITE(RTC_RESET_CAUSE_REG, val);
}

/* in IRAM, can be called from panic handler */
esp_reset_reason_t esp_reset_reason_get_hint(void)
{
    uint32_t reset_reason_hint = REG_READ(RTC_RESET_CAUSE_REG);
    uint32_t high = (reset_reason_hint >> RST_REASON_SHIFT) & RST_REASON_MASK;
    uint32_t low = reset_reason_hint & RST_REASON_MASK;
    if ((reset_reason_hint & RST_REASON_BIT) == 0 || high != low) {
        return ESP_RST_UNKNOWN;
    }
    return (esp_reset_reason_t) low;
}
static inline void esp_reset_reason_clear_hint(void)
{
    REG_WRITE(RTC_RESET_CAUSE_REG, 0);
}
