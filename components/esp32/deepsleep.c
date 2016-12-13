// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stddef.h>
#include <sys/lock.h>
#include "esp_attr.h"
#include "esp_deep_sleep.h"
#include "esp_log.h"
#include "rom/cache.h"
#include "rom/rtc.h"
#include "soc/cpu.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/dport_reg.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtc.h"
#include "sdkconfig.h"

/* Updating RTC_MEMORY_CRC_REG register via set_rtc_memory_crc()
   is not thread-safe. */
static _lock_t lock_rtc_memory_crc;
static uint32_t s_wakeup_options = 0;
static uint64_t s_sleep_duration = 0;

static const char* TAG = "deepsleep";

/* Wake from deep sleep stub
   See esp_deepsleep.h esp_wake_deep_sleep() comments for details.
*/
esp_deep_sleep_wake_stub_fn_t esp_get_deep_sleep_wake_stub(void)
{
    _lock_acquire(&lock_rtc_memory_crc);
    uint32_t stored_crc = REG_READ(RTC_MEMORY_CRC_REG);
    set_rtc_memory_crc();
    uint32_t calc_crc = REG_READ(RTC_MEMORY_CRC_REG);
    REG_WRITE(RTC_MEMORY_CRC_REG, stored_crc);
    _lock_release(&lock_rtc_memory_crc);

    if(stored_crc == calc_crc) {
        return (esp_deep_sleep_wake_stub_fn_t)REG_READ(RTC_ENTRY_ADDR_REG);
    } else {
        return NULL;
    }
}

void esp_set_deep_sleep_wake_stub(esp_deep_sleep_wake_stub_fn_t new_stub)
{
    _lock_acquire(&lock_rtc_memory_crc);
    REG_WRITE(RTC_ENTRY_ADDR_REG, (uint32_t)new_stub);
    set_rtc_memory_crc();
    _lock_release(&lock_rtc_memory_crc);
}

void RTC_IRAM_ATTR esp_default_wake_deep_sleep(void) {
    /* Clear MMU for CPU 0 */
    REG_SET_BIT(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MMU_IA_CLR);
    REG_CLR_BIT(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MMU_IA_CLR);
#if CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY > 0
    // ROM code has not started yet, so we need to set delay factor
    // used by ets_delay_us first.
    ets_update_cpu_frequency(ets_get_detected_xtal_freq() / 1000000);
    ets_delay_us(CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY);
#endif
}

void __attribute__((weak, alias("esp_default_wake_deep_sleep"))) esp_wake_deep_sleep(void);

void esp_deep_sleep(uint64_t time_in_us)
{
    esp_deep_sleep_enable_timer_wakeup(time_in_us);
    esp_deep_sleep_start();
}

void IRAM_ATTR esp_deep_sleep_start()
{
    if (esp_get_deep_sleep_wake_stub() == NULL) {
        esp_set_deep_sleep_wake_stub(esp_wake_deep_sleep);
    }

    rtc_set_cpu_freq(CPU_XTAL);
    uint32_t cycle_h = 0;
    uint32_t cycle_l = 0;
    if (s_sleep_duration > 0) {
        uint32_t period = rtc_slowck_cali(CALI_RTC_MUX, 128);
        rtc_usec2rtc(s_sleep_duration >> 32, s_sleep_duration & 0xffffffff, period, &cycle_h, &cycle_l);
    }
    rtc_slp_prep_lite(DEEP_SLEEP_PD_NORMAL, 0);
    rtc_sleep(cycle_h, cycle_l, s_wakeup_options, 0);
    while (1) {
        ;
    }
}

void system_deep_sleep(uint64_t) __attribute__((alias("esp_deep_sleep")));

esp_err_t esp_deep_sleep_enable_ulp_wakeup()
{
#ifdef CONFIG_ULP_COPROC_ENABLED
    s_wakeup_options |= SAR_TRIG_EN;
    return ESP_OK;
#else
    return ESP_ERR_INVALID_STATE;
#endif
}

esp_err_t esp_deep_sleep_enable_timer_wakeup(uint64_t time_in_us)
{
    s_wakeup_options |= TIMER_EXPIRE_EN;
    s_sleep_duration = time_in_us;
    return ESP_OK;
}

esp_err_t esp_deep_sleep_enable_ext0_wakeup(gpio_num_t gpio_num, int level)
{
    if (level < 0 || level > 1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    const rtc_gpio_desc_t* desc = &rtc_gpio_desc[gpio_num];
    REG_SET_FIELD(RTC_IO_EXT_WAKEUP0_REG, RTC_IO_EXT_WAKEUP0_SEL, desc->rtc_num);
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1, level, RTC_CNTL_EXT_WAKEUP0_LV_S);
    REG_SET_BIT(desc->reg, desc->slpsel);
    REG_SET_BIT(desc->reg, desc->slpie);
    s_wakeup_options |= RTC_EXT_EVENT0_TRIG_EN;
    return ESP_OK;
}

esp_err_t esp_deep_sleep_enable_ext1_wakeup(uint64_t mask, esp_ext1_wakeup_mode_t mode)
{
    if (mode > EXT1_WAKEUP_ANY_HIGH) {
        return ESP_ERR_INVALID_ARG;
    }
    // Translate bit map of GPIO numbers into the bit map of RTC IO numbers
    uint32_t rtc_gpio_mask = 0;
    for (int gpio = 0; mask; ++gpio, mask >>= 1) {
        if ((mask & 1) == 0) {
            continue;
        }
        if (!RTC_GPIO_IS_VALID_GPIO(gpio)) {
            ESP_LOGE(TAG, "Not an RTC IO: GPIO%d", gpio);
            return ESP_ERR_INVALID_ARG;
        }
        const rtc_gpio_desc_t* desc = &rtc_gpio_desc[gpio];
        int rtc_pin = desc->rtc_num;
        rtc_gpio_mask |= BIT(rtc_pin);
        REG_SET_BIT(desc->reg, desc->slpsel);
        REG_SET_BIT(desc->reg, desc->slpie);
    }
    REG_SET_BIT(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_STATUS_CLR);
    REG_SET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL, rtc_gpio_mask);
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1, mode, RTC_CNTL_EXT_WAKEUP1_LV_S);
    s_wakeup_options |= RTC_EXT_EVENT1_TRIG_EN;
    return ESP_OK;
}

uint64_t esp_deep_sleep_get_ext1_wakeup_status()
{
    int wakeup_reason = REG_GET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_CAUSE);
    if (wakeup_reason != RTC_EXT_EVENT1_TRIG) {
        return 0;
    }
    uint32_t status = REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_STATUS_REG, RTC_CNTL_EXT_WAKEUP1_STATUS);
    // Translate bit map of RTC IO numbers into the bit map of GPIO numbers
    uint64_t gpio_mask = 0;
    for (int gpio = 0; gpio < 40; ++gpio) {
        if (!RTC_GPIO_IS_VALID_GPIO(gpio)) {
            continue;
        }
        int rtc_pin = rtc_gpio_desc[gpio].rtc_num;
        if ((status & BIT(rtc_pin)) == 0) {
            continue;
        }
        gpio_mask |= BIT(gpio);
    }
    return gpio_mask;
}
