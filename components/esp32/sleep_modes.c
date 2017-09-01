// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include <sys/param.h>
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_clk.h"
#include "esp_newlib.h"
#include "esp_spi_flash.h"
#include "rom/cache.h"
#include "rom/rtc.h"
#include "rom/uart.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/spi_reg.h"
#include "soc/sens_reg.h"
#include "soc/dport_reg.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

// If light sleep time is less than that, don't power down flash
#define FLASH_PD_MIN_SLEEP_TIME_US  2000

// Time from VDD_SDIO power up to first flash read in ROM code
#define VDD_SDIO_POWERUP_TO_FLASH_READ_US 700

/**
 * Internal structure which holds all requested deep sleep parameters
 */
typedef struct {
    esp_sleep_pd_option_t pd_options[ESP_PD_DOMAIN_MAX];
    uint64_t sleep_duration;
    uint32_t wakeup_triggers : 11;
    uint32_t ext1_trigger_mode : 1;
    uint32_t ext1_rtc_gpio_mask : 18;
    uint32_t ext0_trigger_level : 1;
    uint32_t ext0_rtc_gpio_num : 5;
} deep_sleep_config_t;

static deep_sleep_config_t s_config = {
    .pd_options = { ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO },
    .wakeup_triggers = 0
};

/* Updating RTC_MEMORY_CRC_REG register via set_rtc_memory_crc()
   is not thread-safe. */
static _lock_t lock_rtc_memory_crc;

static const char* TAG = "sleep";

static uint32_t get_power_down_flags();
static void ext0_wakeup_prepare();
static void ext1_wakeup_prepare();
static void timer_wakeup_prepare();

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
    _DPORT_REG_WRITE(DPORT_PRO_CACHE_CTRL1_REG,
            _DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG) | DPORT_PRO_CACHE_MMU_IA_CLR);
    _DPORT_REG_WRITE(DPORT_PRO_CACHE_CTRL1_REG,
            _DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG) & (~DPORT_PRO_CACHE_MMU_IA_CLR));
#if CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY > 0
    // ROM code has not started yet, so we need to set delay factor
    // used by ets_delay_us first.
    ets_update_cpu_frequency_rom(ets_get_detected_xtal_freq() / 1000000);
    // This delay is configured in menuconfig, it can be used to give
    // the flash chip some time to become ready.
    ets_delay_us(CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY);
#endif
}

void __attribute__((weak, alias("esp_default_wake_deep_sleep"))) esp_wake_deep_sleep(void);

void esp_deep_sleep(uint64_t time_in_us)
{
    esp_sleep_enable_timer_wakeup(time_in_us);
    esp_deep_sleep_start();
}

static uint32_t IRAM_ATTR esp_sleep_start(uint32_t pd_flags)
{
    // Flush UARTs so that output is not lost due to APB frequency change
    uart_tx_wait_idle(0);
    uart_tx_wait_idle(1);
    uart_tx_wait_idle(2);

    // Configure pins for external wakeup
    if (s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
        ext0_wakeup_prepare();
    }
    if (s_config.wakeup_triggers & RTC_EXT1_TRIG_EN) {
        ext1_wakeup_prepare();
    }
    // Enable ULP wakeup
    if (s_config.wakeup_triggers & RTC_ULP_TRIG_EN) {
        SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN);
    }
    // Configure timer wakeup
    if ((s_config.wakeup_triggers & RTC_TIMER_TRIG_EN) &&
        s_config.sleep_duration > 0) {
        timer_wakeup_prepare();
    }

    // Enter sleep
    rtc_sleep_config_t config = RTC_SLEEP_CONFIG_DEFAULT(pd_flags);
    rtc_sleep_init(config);
    return rtc_sleep_start(s_config.wakeup_triggers, 0);
}

void IRAM_ATTR esp_deep_sleep_start()
{
    // Configure wake stub
    if (esp_get_deep_sleep_wake_stub() == NULL) {
        esp_set_deep_sleep_wake_stub(esp_wake_deep_sleep);
    }

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

    // Enter sleep
    esp_sleep_start(RTC_SLEEP_PD_DIG | RTC_SLEEP_PD_VDDSDIO | pd_flags);

    // Because RTC is in a slower clock domain than the CPU, it
    // can take several CPU cycles for the sleep mode to start.
    while (1) {
        ;
    }
}

static void rtc_wdt_enable(int time_ms)
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    WRITE_PERI_REG(RTC_CNTL_WDTFEED_REG, 1);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_SYS_RESET_LENGTH, 7);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_CPU_RESET_LENGTH, 7);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_RESET_RTC);
    WRITE_PERI_REG(RTC_CNTL_WDTCONFIG1_REG, rtc_clk_slow_freq_get_hz() * time_ms / 1000);
    SET_PERI_REG_MASK(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN | RTC_CNTL_WDT_PAUSE_IN_SLP);
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}

static void rtc_wdt_disable()
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    WRITE_PERI_REG(RTC_CNTL_WDTFEED_REG, 1);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_OFF);
    REG_CLR_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN);
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}

/**
 * Helper function which handles entry to and exit from light sleep
 * Placed into IRAM as flash may need some time to be powered on.
 */
static esp_err_t IRAM_ATTR esp_light_sleep_inner(uint32_t pd_flags,
        rtc_cpu_freq_t cpu_freq, uint32_t flash_enable_time_us)
{
    // Enter sleep
    esp_err_t err = esp_sleep_start(pd_flags);

    // Restore CPU frequency
    rtc_clk_cpu_freq_set(cpu_freq);

    // If SPI flash was powered down, wait for it to become ready
    if (pd_flags & RTC_SLEEP_PD_VDDSDIO) {
        // Wait for the flash chip to start up
        ets_delay_us(flash_enable_time_us);
    }
    return err;
}

esp_err_t esp_light_sleep_start()
{
    static portMUX_TYPE light_sleep_lock = portMUX_INITIALIZER_UNLOCKED;

    portENTER_CRITICAL(&light_sleep_lock);
    int other_cpu = xPortGetCoreID() ? 0 : 1;
    esp_cpu_stall(other_cpu);

    // Other CPU is stalled, need to disable DPORT protection
    esp_dport_access_int_pause();

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

    // Decide if flash needs to be powered down;
    // If it needs to be powered down, adjust sleep time
    const uint32_t flash_enable_time_us = VDD_SDIO_POWERUP_TO_FLASH_READ_US
                                          + CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY;

    if (s_config.sleep_duration > FLASH_PD_MIN_SLEEP_TIME_US &&
        s_config.sleep_duration > flash_enable_time_us) {
        pd_flags |= RTC_SLEEP_PD_VDDSDIO;
        s_config.sleep_duration -= flash_enable_time_us;
    }

    // Safety net: enable WDT in case exit from light sleep fails
    rtc_wdt_enable(1000);

    // Save current CPU frequency, light sleep will switch to XTAL
    rtc_cpu_freq_t cpu_freq = rtc_clk_cpu_freq_get();

    // Enter sleep, then wait for flash to be ready on wakeup
    esp_err_t err = esp_light_sleep_inner(pd_flags, cpu_freq, flash_enable_time_us);

    // At this point, if FRC1 is used for timekeeping, time will be lagging behind.
    // This will update the microsecond count based on RTC timer.
    esp_set_time_from_rtc();

    // However, we do not advance RTOS ticks here; doing so would be rather messy,
    // as ticks can only be advanced on CPU0.
    // If this is needed by the application, automatic light sleep (tickless idle)
    // will handle that better.

    esp_cpu_unstall(other_cpu);
    esp_dport_access_int_resume();
    rtc_wdt_disable();
    portEXIT_CRITICAL(&light_sleep_lock);
    return err;
}

void system_deep_sleep(uint64_t) __attribute__((alias("esp_deep_sleep")));

esp_err_t esp_sleep_enable_ulp_wakeup()
{
#ifdef CONFIG_ULP_COPROC_ENABLED
    if(s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
        ESP_LOGE(TAG, "Conflicting wake-up trigger: ext0");
        return ESP_ERR_INVALID_STATE;
    }
    s_config.wakeup_triggers |= RTC_ULP_TRIG_EN;
    return ESP_OK;
#else
    return ESP_ERR_INVALID_STATE;
#endif
}

esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us)
{
    s_config.wakeup_triggers |= RTC_TIMER_TRIG_EN;
    s_config.sleep_duration = time_in_us;
    return ESP_OK;
}

static void timer_wakeup_prepare()
{
    uint32_t period = esp_clk_slowclk_cal_get();
    uint64_t rtc_count_delta = rtc_time_us_to_slowclk(s_config.sleep_duration, period);
    uint64_t cur_rtc_count = rtc_time_get();
    rtc_sleep_set_wakeup_time(cur_rtc_count + rtc_count_delta);
}

esp_err_t esp_sleep_enable_touchpad_wakeup()
{
    if (s_config.wakeup_triggers & (RTC_EXT0_TRIG_EN)) {
        ESP_LOGE(TAG, "Conflicting wake-up trigger: ext0");
        return ESP_ERR_INVALID_STATE;
    }
    s_config.wakeup_triggers |= RTC_TOUCH_TRIG_EN;
    return ESP_OK;
}

touch_pad_t esp_sleep_get_touchpad_wakeup_status()
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TOUCHPAD) {
        return TOUCH_PAD_MAX;
    }
    uint32_t touch_mask = REG_GET_FIELD(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_MEAS_EN);
    assert(touch_mask != 0 && "wakeup reason is RTC_TOUCH_TRIG_EN but SENS_TOUCH_MEAS_EN is zero");
    return (touch_pad_t) (__builtin_ffs(touch_mask) - 1);
}

esp_err_t esp_sleep_enable_ext0_wakeup(gpio_num_t gpio_num, int level)
{
    if (level < 0 || level > 1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) {
        ESP_LOGE(TAG, "Conflicting wake-up triggers: touch / ULP");
        return ESP_ERR_INVALID_STATE;
    }
    s_config.ext0_rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;
    s_config.ext0_trigger_level = level;
    s_config.wakeup_triggers |= RTC_EXT0_TRIG_EN;
    return ESP_OK;
}

static void ext0_wakeup_prepare()
{
    int rtc_gpio_num = s_config.ext0_rtc_gpio_num;
    // Set GPIO to be used for wakeup
    REG_SET_FIELD(RTC_IO_EXT_WAKEUP0_REG, RTC_IO_EXT_WAKEUP0_SEL, rtc_gpio_num);
    // Set level which will trigger wakeup
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
            s_config.ext0_trigger_level, RTC_CNTL_EXT_WAKEUP0_LV_S);
    // Find GPIO descriptor in the rtc_gpio_desc table and configure the pad
    for (size_t gpio_num = 0; gpio_num < GPIO_PIN_COUNT; ++gpio_num) {
        const rtc_gpio_desc_t* desc = &rtc_gpio_desc[gpio_num];
        if (desc->rtc_num == rtc_gpio_num) {
            REG_SET_BIT(desc->reg, desc->mux);
            SET_PERI_REG_BITS(desc->reg, 0x3, 0, desc->func);
            REG_SET_BIT(desc->reg, desc->slpsel);
            REG_SET_BIT(desc->reg, desc->slpie);
            break;
        }
    }
}

esp_err_t esp_sleep_enable_ext1_wakeup(uint64_t mask, esp_sleep_ext1_wakeup_mode_t mode)
{
    if (mode > ESP_EXT1_WAKEUP_ANY_HIGH) {
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
        rtc_gpio_mask |= BIT(rtc_gpio_desc[gpio].rtc_num);
    }
    s_config.ext1_rtc_gpio_mask = rtc_gpio_mask;
    s_config.ext1_trigger_mode = mode;
    s_config.wakeup_triggers |= RTC_EXT1_TRIG_EN;
    return ESP_OK;
}

static void ext1_wakeup_prepare()
{
    // Configure all RTC IOs selected as ext1 wakeup inputs
    uint32_t rtc_gpio_mask = s_config.ext1_rtc_gpio_mask;
    for (int gpio = 0; gpio < GPIO_PIN_COUNT && rtc_gpio_mask != 0; ++gpio) {
        int rtc_pin = rtc_gpio_desc[gpio].rtc_num;
        if ((rtc_gpio_mask & BIT(rtc_pin)) == 0) {
            continue;
        }
        const rtc_gpio_desc_t* desc = &rtc_gpio_desc[gpio];
        // Route pad to RTC
        REG_SET_BIT(desc->reg, desc->mux);
        SET_PERI_REG_BITS(desc->reg, 0x3, 0, desc->func);
        // Pad configuration depends on RTC_PERIPH state in sleep mode
        if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] == ESP_PD_OPTION_ON) {
            // set input enable in sleep mode
            REG_SET_BIT(desc->reg, desc->slpie);
            // allow sleep status signal to control IE/SLPIE mux
            REG_SET_BIT(desc->reg, desc->slpsel);
        } else {
            // RTC_PERIPH will be disabled, so need to enable input and
            // lock pad configuration. Pullups/pulldowns also need to be disabled.
            REG_SET_BIT(desc->reg, desc->ie);
            REG_CLR_BIT(desc->reg, desc->pulldown);
            REG_CLR_BIT(desc->reg, desc->pullup);
            REG_SET_BIT(RTC_CNTL_HOLD_FORCE_REG, desc->hold_force);
        }
        // Keep track of pins which are processed to bail out early
        rtc_gpio_mask &= ~BIT(rtc_pin);
    }
    // Clear state from previous wakeup
    REG_SET_BIT(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_STATUS_CLR);
    // Set pins to be used for wakeup
    REG_SET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL, s_config.ext1_rtc_gpio_mask);
    // Set logic function (any low, all high)
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
            s_config.ext1_trigger_mode, RTC_CNTL_EXT_WAKEUP1_LV_S);
}

uint64_t esp_sleep_get_ext1_wakeup_status()
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT1) {
        return 0;
    }
    uint32_t status = REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_STATUS_REG, RTC_CNTL_EXT_WAKEUP1_STATUS);
    // Translate bit map of RTC IO numbers into the bit map of GPIO numbers
    uint64_t gpio_mask = 0;
    for (int gpio = 0; gpio < GPIO_PIN_COUNT; ++gpio) {
        if (!RTC_GPIO_IS_VALID_GPIO(gpio)) {
            continue;
        }
        int rtc_pin = rtc_gpio_desc[gpio].rtc_num;
        if ((status & BIT(rtc_pin)) == 0) {
            continue;
        }
        gpio_mask |= 1ULL << gpio;
    }
    return gpio_mask;
}

esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause()
{
    if (rtc_get_reset_reason(0) != DEEPSLEEP_RESET) {
        return ESP_SLEEP_WAKEUP_UNDEFINED;
    }

    uint32_t wakeup_cause = REG_GET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_CAUSE);
    if (wakeup_cause & RTC_EXT0_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_EXT0;
    } else if (wakeup_cause & RTC_EXT1_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_EXT1;
    } else if (wakeup_cause & RTC_TIMER_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_TIMER;
    } else if (wakeup_cause & RTC_TOUCH_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_TOUCHPAD;
    } else if (wakeup_cause & RTC_ULP_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_ULP;
    } else {
        return ESP_SLEEP_WAKEUP_UNDEFINED;
    }
}

esp_err_t esp_sleep_pd_config(esp_sleep_pd_domain_t domain,
                                   esp_sleep_pd_option_t option)
{
    if (domain >= ESP_PD_DOMAIN_MAX || option > ESP_PD_OPTION_AUTO) {
        return ESP_ERR_INVALID_ARG;
    }
    s_config.pd_options[domain] = option;
    return ESP_OK;
}

static uint32_t get_power_down_flags()
{
    // Where needed, convert AUTO options to ON. Later interpret AUTO as OFF.

    // RTC_SLOW_MEM is needed for the ULP, so keep RTC_SLOW_MEM powered up if ULP
    // is used and RTC_SLOW_MEM is Auto.
    // If there is any data placed into .rtc.data or .rtc.bss segments, and
    // RTC_SLOW_MEM is Auto, keep it powered up as well.

    // These labels are defined in the linker script:
    extern int _rtc_data_start, _rtc_data_end, _rtc_bss_start, _rtc_bss_end;

    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] == ESP_PD_OPTION_AUTO ||
            &_rtc_data_end > &_rtc_data_start ||
            &_rtc_bss_end > &_rtc_bss_start) {
        s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] = ESP_PD_OPTION_ON;
    }

    // RTC_FAST_MEM is needed for deep sleep stub.
    // If RTC_FAST_MEM is Auto, keep it powered on, so that deep sleep stub
    // can run.
    // In the new chip revision, deep sleep stub will be optional,
    // and this can be changed.
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM] == ESP_PD_OPTION_AUTO) {
        s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM] = ESP_PD_OPTION_ON;
    }

    // RTC_PERIPH is needed for EXT0 wakeup.
    // If RTC_PERIPH is auto, and EXT0 isn't enabled, power down RTC_PERIPH.
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] == ESP_PD_OPTION_AUTO) {
        if (s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_ON;
        } else if (s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) {
            // In both rev. 0 and rev. 1 of ESP32, forcing power up of RTC_PERIPH
            // prevents ULP timer and touch FSMs from working correctly.
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_OFF;
        }
    }

    const char* option_str[] = {"OFF", "ON", "AUTO(OFF)" /* Auto works as OFF */};
    ESP_LOGD(TAG, "RTC_PERIPH: %s, RTC_SLOW_MEM: %s, RTC_FAST_MEM: %s",
            option_str[s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH]],
            option_str[s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM]],
            option_str[s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM]]);

    // Prepare flags based on the selected options
    uint32_t pd_flags = 0;
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_FAST_MEM;
    }
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_SLOW_MEM;
    }
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_PERIPH;
    }
    return pd_flags;
}
