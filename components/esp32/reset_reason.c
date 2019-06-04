// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_system.h"
#include "esp32/rom/rtc.h"
#include "esp_private/system_internal.h"
#include "soc/rtc_periph.h"

static void esp_reset_reason_clear_hint();

static esp_reset_reason_t s_reset_reason;

static esp_reset_reason_t get_reset_reason(RESET_REASON rtc_reset_reason, esp_reset_reason_t reset_reason_hint)
{
    switch (rtc_reset_reason) {
        case POWERON_RESET:
            return ESP_RST_POWERON;

        /* For ESP32, ESP_RST_EXT is never returned */


        case SW_CPU_RESET:
        case SW_RESET:
        case EXT_CPU_RESET: /* unused */
            if (reset_reason_hint == ESP_RST_PANIC ||
                reset_reason_hint == ESP_RST_BROWNOUT ||
                reset_reason_hint == ESP_RST_TASK_WDT ||
                reset_reason_hint == ESP_RST_INT_WDT) {
                return reset_reason_hint;
            }
            return ESP_RST_SW;

        case DEEPSLEEP_RESET:
            return ESP_RST_DEEPSLEEP;

        case TG0WDT_SYS_RESET:
            return ESP_RST_TASK_WDT;

        case TG1WDT_SYS_RESET:
            return ESP_RST_INT_WDT;

        case OWDT_RESET:
        case RTCWDT_SYS_RESET:
        case RTCWDT_RTC_RESET:
        case RTCWDT_CPU_RESET:  /* unused */
        case TGWDT_CPU_RESET:   /* unused */
            return ESP_RST_WDT;

        case RTCWDT_BROWN_OUT_RESET:    /* unused */
            return ESP_RST_BROWNOUT;

        case SDIO_RESET:
            return ESP_RST_SDIO;

        case INTRUSION_RESET: /* unused */
        default:
            return ESP_RST_UNKNOWN;
    }
}

static void __attribute__((constructor)) esp_reset_reason_init(void)
{
    esp_reset_reason_t hint = esp_reset_reason_get_hint();
    s_reset_reason = get_reset_reason(rtc_get_reset_reason(PRO_CPU_NUM),
                                      hint);
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
 * rtc_get_reset_reason is unambiguous.
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
esp_reset_reason_t IRAM_ATTR esp_reset_reason_get_hint(void)
{
    uint32_t reset_reason_hint = REG_READ(RTC_RESET_CAUSE_REG);
    uint32_t high = (reset_reason_hint >> RST_REASON_SHIFT) & RST_REASON_MASK;
    uint32_t low = reset_reason_hint & RST_REASON_MASK;
    if ((reset_reason_hint & RST_REASON_BIT) == 0 || high != low) {
        return ESP_RST_UNKNOWN;
    }
    return (esp_reset_reason_t) low;
}
static void esp_reset_reason_clear_hint()
{
    REG_WRITE(RTC_RESET_CAUSE_REG, 0);
}

