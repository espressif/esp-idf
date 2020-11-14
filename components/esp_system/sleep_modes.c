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
#include "esp_private/esp_timer_private.h"
#include "esp_private/system_internal.h"
#include "esp_log.h"
#include "esp_newlib.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_sensor.h"
#include "driver/touch_sensor_common.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"

#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/dport_reg.h"
#include "soc/soc_caps.h"

#include "hal/wdt_hal.h"
#include "hal/rtc_io_hal.h"
#include "hal/rtc_hal.h"
#include "hal/uart_hal.h"
#include "hal/touch_sensor_hal.h"
#include "hal/clk_gate_ll.h"

#include "sdkconfig.h"
#include "esp_rom_uart.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#include "esp32/clk.h"
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/clk.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/rtc.h"
#include "soc/extmem_reg.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/clk.h"
#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/rtc.h"
#include "soc/extmem_reg.h"
#endif

// If light sleep time is less than that, don't power down flash
#define FLASH_PD_MIN_SLEEP_TIME_US  2000

// Time from VDD_SDIO power up to first flash read in ROM code
#define VDD_SDIO_POWERUP_TO_FLASH_READ_US 700

#ifdef CONFIG_IDF_TARGET_ESP32
#define DEFAULT_CPU_FREQ_MHZ        CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#elif CONFIG_IDF_TARGET_ESP32S2
#define DEFAULT_CPU_FREQ_MHZ        CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ
#elif CONFIG_IDF_TARGET_ESP32S3
#define DEFAULT_CPU_FREQ_MHZ        CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ
#endif

#if defined(CONFIG_IDF_TARGET_ESP32)
#if defined(CONFIG_ESP32_RTC_CLK_SRC_EXT_CRYS)
#define LIGHT_SLEEP_TIME_OVERHEAD_US (650 + 30 * 240 / DEFAULT_CPU_FREQ_MHZ)
#define DEEP_SLEEP_TIME_OVERHEAD_US (650 + 100 * 240 / DEFAULT_CPU_FREQ_MHZ)
#else // CONFIG_ESP32_RTC_CLK_SRC_EXT_CRYS
#define LIGHT_SLEEP_TIME_OVERHEAD_US (250 + 30 * 240 / DEFAULT_CPU_FREQ_MHZ)
#define DEEP_SLEEP_TIME_OVERHEAD_US (250 + 100 * 240 / DEFAULT_CPU_FREQ_MHZ)
#endif // CONFIG_ESP32_RTC_CLK_SRC_EXT_CRYS

#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#if defined(CONFIG_ESP32S2_RTC_CLK_SRC_EXT_CRYS)
#define LIGHT_SLEEP_TIME_OVERHEAD_US (1650 + 30 * 240 / DEFAULT_CPU_FREQ_MHZ)
#define DEEP_SLEEP_TIME_OVERHEAD_US (650 + 100 * 240 / DEFAULT_CPU_FREQ_MHZ)
#else // CONFIG_ESP32S2_RTC_CLK_SRC_EXT_CRYS
#define LIGHT_SLEEP_TIME_OVERHEAD_US (1250 + 30 * 240 / DEFAULT_CPU_FREQ_MHZ)
#define DEEP_SLEEP_TIME_OVERHEAD_US (250 + 100 * 240 / DEFAULT_CPU_FREQ_MHZ)
#endif // CONFIG_ESP32S2_RTC_CLK_SRC_EXT_CRYS

#else // other target
#define LIGHT_SLEEP_TIME_OVERHEAD_US 0
#define DEEP_SLEEP_TIME_OVERHEAD_US 0
#endif // CONFIG_IDF_TARGET_*


#if defined(CONFIG_IDF_TARGET_ESP32) && defined(CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY)
#define DEEP_SLEEP_WAKEUP_DELAY     CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY
#else
#define DEEP_SLEEP_WAKEUP_DELAY     0
#endif

// Minimal amount of time we can sleep for
#define LIGHT_SLEEP_MIN_TIME_US 200

#define CHECK_SOURCE(source, value, mask) ((s_config.wakeup_triggers & mask) && \
                                            (source == value))

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
    uint32_t sleep_time_adjustment;
    uint64_t rtc_ticks_at_sleep_start;
} sleep_config_t;

static sleep_config_t s_config = {
    .pd_options = { ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO },
    .wakeup_triggers = 0
};

/* Internal variable used to track if light sleep wakeup sources are to be
   expected when determining wakeup cause. */
static bool s_light_sleep_wakeup = false;

/* Updating RTC_MEMORY_CRC_REG register via set_rtc_memory_crc()
   is not thread-safe, so we need to disable interrupts before going to deep sleep. */
static portMUX_TYPE spinlock_rtc_deep_sleep = portMUX_INITIALIZER_UNLOCKED;

static const char* TAG = "sleep";

static uint32_t get_power_down_flags(void);
static void ext0_wakeup_prepare(void);
static void ext1_wakeup_prepare(void);
static void timer_wakeup_prepare(void);


#ifdef CONFIG_IDF_TARGET_ESP32S2
static void touch_wakeup_prepare(void);
#endif

/* Wake from deep sleep stub
   See esp_deepsleep.h esp_wake_deep_sleep() comments for details.
*/
esp_deep_sleep_wake_stub_fn_t esp_get_deep_sleep_wake_stub(void)
{
    esp_deep_sleep_wake_stub_fn_t stub_ptr = (esp_deep_sleep_wake_stub_fn_t) REG_READ(RTC_ENTRY_ADDR_REG);
    if (!esp_ptr_executable(stub_ptr)) {
        return NULL;
    }
    return stub_ptr;
}

void esp_set_deep_sleep_wake_stub(esp_deep_sleep_wake_stub_fn_t new_stub)
{
    REG_WRITE(RTC_ENTRY_ADDR_REG, (uint32_t)new_stub);
}

void RTC_IRAM_ATTR esp_default_wake_deep_sleep(void) {
    /* Clear MMU for CPU 0 */
#if CONFIG_IDF_TARGET_ESP32
    _DPORT_REG_WRITE(DPORT_PRO_CACHE_CTRL1_REG,
            _DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG) | DPORT_PRO_CACHE_MMU_IA_CLR);
    _DPORT_REG_WRITE(DPORT_PRO_CACHE_CTRL1_REG,
            _DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG) & (~DPORT_PRO_CACHE_MMU_IA_CLR));
#if DEEP_SLEEP_WAKEUP_DELAY > 0
    // ROM code has not started yet, so we need to set delay factor
    // used by esp_rom_delay_us first.
    ets_update_cpu_frequency_rom(ets_get_detected_xtal_freq() / 1000000);
    // This delay is configured in menuconfig, it can be used to give
    // the flash chip some time to become ready.
    esp_rom_delay_us(DEEP_SLEEP_WAKEUP_DELAY);
#endif
#elif CONFIG_IDF_TARGET_ESP32S2
    REG_SET_BIT(EXTMEM_CACHE_DBG_INT_ENA_REG, EXTMEM_CACHE_DBG_EN);
#endif
}

void __attribute__((weak, alias("esp_default_wake_deep_sleep"))) esp_wake_deep_sleep(void);

void esp_deep_sleep(uint64_t time_in_us)
{
    esp_sleep_enable_timer_wakeup(time_in_us);
    esp_deep_sleep_start();
}

// [refactor-todo] provide target logic for body of uart functions below
static void IRAM_ATTR flush_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
#ifdef CONFIG_IDF_TARGET_ESP32
        esp_rom_uart_tx_wait_idle(i);
#elif CONFIG_IDF_TARGET_ESP32S2
        if (periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            esp_rom_uart_tx_wait_idle(i);
        }
#endif
    }
}

static void IRAM_ATTR suspend_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
#ifdef CONFIG_IDF_TARGET_ESP32
        /* Note: Set `UART_FORCE_XOFF` can't stop new Tx request. */
        REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
        while (REG_GET_FIELD(UART_STATUS_REG(i), UART_ST_UTX_OUT) != 0) {
            ;
        }
#elif CONFIG_IDF_TARGET_ESP32S2
        if (periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
            REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_SW_FLOW_CON_EN | UART_FORCE_XOFF);
            while (REG_GET_FIELD(UART_FSM_STATUS_REG(i), UART_ST_UTX_OUT) != 0) {
                ;
            }
        }
#endif
    }
}

static void IRAM_ATTR resume_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
#ifdef CONFIG_IDF_TARGET_ESP32
        REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
        REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
        REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
#elif CONFIG_IDF_TARGET_ESP32S2
        if (periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
            REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
            REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_SW_FLOW_CON_EN | UART_FORCE_XON);
        }
#endif
    }
}

inline static uint32_t IRAM_ATTR call_rtc_sleep_start(uint32_t reject_triggers);

static uint32_t IRAM_ATTR esp_sleep_start(uint32_t pd_flags)
{
    // Stop UART output so that output is not lost due to APB frequency change.
    // For light sleep, suspend UART output — it will resume after wakeup.
    // For deep sleep, wait for the contents of UART FIFO to be sent.
    bool deep_sleep = pd_flags & RTC_SLEEP_PD_DIG;

#if !CONFIG_FREERTOS_UNICORE && ESP32S3_ALLOW_RTC_FAST_MEM_AS_HEAP
    /* Currently only safe to use deep sleep wake stub & RTC memory as heap in single core mode.

       For ESP32-S3, either disable ESP32S3_ALLOW_RTC_FAST_MEM_AS_HEAP in config or find a way to set the
       deep sleep wake stub to NULL.
     */
    assert(!deep_sleep || esp_get_deep_sleep_wake_stub() == NULL);
#endif

    if (deep_sleep) {
        flush_uarts();
    } else {
        suspend_uarts();
    }

    // Save current frequency and switch to XTAL
    rtc_cpu_freq_config_t cpu_freq_config;
    rtc_clk_cpu_freq_get_config(&cpu_freq_config);
    rtc_clk_cpu_freq_set_xtal();

    // Configure pins for external wakeup
    if (s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
        ext0_wakeup_prepare();
    }
    if (s_config.wakeup_triggers & RTC_EXT1_TRIG_EN) {
        ext1_wakeup_prepare();
    }

#ifdef CONFIG_IDF_TARGET_ESP32
    // Enable ULP wakeup
    if (s_config.wakeup_triggers & RTC_ULP_TRIG_EN) {
        rtc_hal_ulp_wakeup_enable();
    }
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2
    // Enable Touch wakeup
    if (s_config.wakeup_triggers & RTC_TOUCH_TRIG_EN) {
        touch_wakeup_prepare();
    }
#endif

    uint32_t reject_triggers = 0;
    if ((pd_flags & RTC_SLEEP_PD_DIG) == 0 && (s_config.wakeup_triggers & RTC_GPIO_TRIG_EN)) {
        /* Light sleep, enable sleep reject for faster return from this function,
         * in case the wakeup is already triggerred.
         */
#if CONFIG_IDF_TARGET_ESP32
        reject_triggers = RTC_CNTL_LIGHT_SLP_REJECT_EN_M | RTC_CNTL_GPIO_REJECT_EN_M;
#elif CONFIG_IDF_TARGET_ESP32S2
        reject_triggers = s_config.wakeup_triggers;
#endif
    }

    // Enter sleep
    rtc_sleep_config_t config = RTC_SLEEP_CONFIG_DEFAULT(pd_flags);
    rtc_sleep_init(config);

    // Configure timer wakeup
    if ((s_config.wakeup_triggers & RTC_TIMER_TRIG_EN) &&
        s_config.sleep_duration > 0) {
        timer_wakeup_prepare();
    }

    uint32_t result;
    if (deep_sleep) {
        /* Disable interrupts in case another task writes to RTC memory while we
         * calculate RTC memory CRC
         *
         * Note: for ESP32-S3 running in dual core mode this is currently not enough,
         * see the assert at top of this function.
         */
        portENTER_CRITICAL(&spinlock_rtc_deep_sleep);

#if !CONFIG_ESP32_ALLOW_RTC_FAST_MEM_AS_HEAP && !CONFIG_ESP32S2_ALLOW_RTC_FAST_MEM_AS_HEAP && !CONFIG_ESP32S3_ALLOW_RTC_FAST_MEM_AS_HEAP
        /* If not possible stack is in RTC FAST memory, use the ROM function to calculate the CRC and save ~140 bytes IRAM */
        set_rtc_memory_crc();
        result = call_rtc_sleep_start(reject_triggers);
#else
        /* Otherwise, need to call the dedicated soc function for this */
        result = rtc_deep_sleep_start(s_config.wakeup_triggers, reject_triggers);
#endif

        portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
    } else {
        result = call_rtc_sleep_start(reject_triggers);
    }

    // Restore CPU frequency
    rtc_clk_cpu_freq_set_config(&cpu_freq_config);

    // re-enable UART output
    resume_uarts();

    return result;
}

inline static uint32_t IRAM_ATTR call_rtc_sleep_start(uint32_t reject_triggers)
{
#ifdef CONFIG_IDF_TARGET_ESP32
        return rtc_sleep_start(s_config.wakeup_triggers, reject_triggers);
#else
        return rtc_sleep_start(s_config.wakeup_triggers, reject_triggers, 1);
#endif
}

void IRAM_ATTR esp_deep_sleep_start(void)
{
    // record current RTC time
    s_config.rtc_ticks_at_sleep_start = rtc_time_get();

    // record current RTC time
    esp_sync_counters_rtc_and_frc();
    // Configure wake stub
    if (esp_get_deep_sleep_wake_stub() == NULL) {
        esp_set_deep_sleep_wake_stub(esp_wake_deep_sleep);
    }

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

    // Correct the sleep time
    s_config.sleep_time_adjustment = DEEP_SLEEP_TIME_OVERHEAD_US;

    // Enter sleep
    esp_sleep_start(RTC_SLEEP_PD_DIG | RTC_SLEEP_PD_VDDSDIO | pd_flags);

    // Because RTC is in a slower clock domain than the CPU, it
    // can take several CPU cycles for the sleep mode to start.
    while (1) {
        ;
    }
}

/**
 * Helper function which handles entry to and exit from light sleep
 * Placed into IRAM as flash may need some time to be powered on.
 */
static esp_err_t esp_light_sleep_inner(uint32_t pd_flags,
        uint32_t flash_enable_time_us,
        rtc_vddsdio_config_t vddsdio_config) IRAM_ATTR __attribute__((noinline));

static esp_err_t esp_light_sleep_inner(uint32_t pd_flags,
        uint32_t flash_enable_time_us,
        rtc_vddsdio_config_t vddsdio_config)
{
    // Enter sleep
    esp_err_t err = esp_sleep_start(pd_flags);

    // If VDDSDIO regulator was controlled by RTC registers before sleep,
    // restore the configuration.
    if (vddsdio_config.force) {
        rtc_vddsdio_set_config(vddsdio_config);
    }

    // If SPI flash was powered down, wait for it to become ready
    if (pd_flags & RTC_SLEEP_PD_VDDSDIO) {
        // Wait for the flash chip to start up
        esp_rom_delay_us(flash_enable_time_us);
    }
    return err;
}

esp_err_t esp_light_sleep_start(void)
{
    static portMUX_TYPE light_sleep_lock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&light_sleep_lock);
    /* We will be calling esp_timer_private_advance inside DPORT access critical
     * section. Make sure the code on the other CPU is not holding esp_timer
     * lock, otherwise there will be deadlock.
     */
    esp_timer_private_lock();
    s_config.rtc_ticks_at_sleep_start = rtc_time_get();
    uint64_t frc_time_at_start = esp_system_get_time();
    DPORT_STALL_OTHER_CPU_START();

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

    // Amount of time to subtract from actual sleep time.
    // This is spent on entering and leaving light sleep.
    s_config.sleep_time_adjustment = LIGHT_SLEEP_TIME_OVERHEAD_US;

    // Decide if VDD_SDIO needs to be powered down;
    // If it needs to be powered down, adjust sleep time.
    const uint32_t flash_enable_time_us = VDD_SDIO_POWERUP_TO_FLASH_READ_US + DEEP_SLEEP_WAKEUP_DELAY;

#ifndef CONFIG_SPIRAM
    const uint32_t vddsdio_pd_sleep_duration = MAX(FLASH_PD_MIN_SLEEP_TIME_US,
            flash_enable_time_us + LIGHT_SLEEP_TIME_OVERHEAD_US + LIGHT_SLEEP_MIN_TIME_US);

    if (s_config.sleep_duration > vddsdio_pd_sleep_duration) {
        pd_flags |= RTC_SLEEP_PD_VDDSDIO;
        s_config.sleep_time_adjustment += flash_enable_time_us;
    }
#endif //CONFIG_SPIRAM

    rtc_vddsdio_config_t vddsdio_config = rtc_vddsdio_get_config();

    // Safety net: enable WDT in case exit from light sleep fails
    wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
    bool wdt_was_enabled = wdt_hal_is_enabled(&rtc_wdt_ctx);    // If WDT was enabled in the user code, then do not change it here.
    if (!wdt_was_enabled) {
        wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
        uint32_t stage_timeout_ticks = (uint32_t)(1000ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
        wdt_hal_enable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }

    // Enter sleep, then wait for flash to be ready on wakeup
    esp_err_t err = esp_light_sleep_inner(pd_flags,
            flash_enable_time_us, vddsdio_config);

    s_light_sleep_wakeup = true;

    // FRC1 has been clock gated for the duration of the sleep, correct for that.
    uint64_t rtc_ticks_at_end = rtc_time_get();
    uint64_t frc_time_at_end = esp_system_get_time();

    uint64_t rtc_time_diff = rtc_time_slowclk_to_us(rtc_ticks_at_end - s_config.rtc_ticks_at_sleep_start,
                                    esp_clk_slowclk_cal_get());
    uint64_t frc_time_diff = frc_time_at_end - frc_time_at_start;

    int64_t time_diff = rtc_time_diff - frc_time_diff;
    /* Small negative values (up to 1 RTC_SLOW clock period) are possible,
     * for very small values of sleep_duration. Ignore those to keep esp_timer
     * monotonic.
     */
    if (time_diff > 0) {
        esp_timer_private_advance(time_diff);
    }
    esp_set_time_from_rtc();

    esp_timer_private_unlock();
    DPORT_STALL_OTHER_CPU_END();
    if (!wdt_was_enabled) {
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }
    portEXIT_CRITICAL(&light_sleep_lock);
    return err;
}

esp_err_t esp_sleep_disable_wakeup_source(esp_sleep_source_t source)
{
    // For most of sources it is enough to set trigger mask in local
    // configuration structure. The actual RTC wake up options
    // will be updated by esp_sleep_start().
    if (source == ESP_SLEEP_WAKEUP_ALL) {
        s_config.wakeup_triggers = 0;
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_TIMER, RTC_TIMER_TRIG_EN)) {
        s_config.wakeup_triggers &= ~RTC_TIMER_TRIG_EN;
        s_config.sleep_duration = 0;
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_EXT0, RTC_EXT0_TRIG_EN)) {
        s_config.ext0_rtc_gpio_num = 0;
        s_config.ext0_trigger_level = 0;
        s_config.wakeup_triggers &= ~RTC_EXT0_TRIG_EN;
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_EXT1, RTC_EXT1_TRIG_EN)) {
        s_config.ext1_rtc_gpio_mask = 0;
        s_config.ext1_trigger_mode = 0;
        s_config.wakeup_triggers &= ~RTC_EXT1_TRIG_EN;
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_TOUCHPAD, RTC_TOUCH_TRIG_EN)) {
        s_config.wakeup_triggers &= ~RTC_TOUCH_TRIG_EN;
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_GPIO, RTC_GPIO_TRIG_EN)) {
        s_config.wakeup_triggers &= ~RTC_GPIO_TRIG_EN;
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_UART, (RTC_UART0_TRIG_EN | RTC_UART1_TRIG_EN))) {
        s_config.wakeup_triggers &= ~(RTC_UART0_TRIG_EN | RTC_UART1_TRIG_EN);
    }
#if defined(CONFIG_ESP32_ULP_COPROC_ENABLED) || defined(CONFIG_ESP32S2_ULP_COPROC_ENABLED)
    else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_ULP, RTC_ULP_TRIG_EN)) {
        s_config.wakeup_triggers &= ~RTC_ULP_TRIG_EN;
    }
#endif
    else {
        ESP_LOGE(TAG, "Incorrect wakeup source (%d) to disable.", (int) source);
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t esp_sleep_enable_ulp_wakeup(void)
{
#if CONFIG_IDF_TARGET_ESP32
#ifdef CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT
    return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT
#ifdef CONFIG_ESP32_ULP_COPROC_ENABLED
    if(s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
        ESP_LOGE(TAG, "Conflicting wake-up trigger: ext0");
        return ESP_ERR_INVALID_STATE;
    }
    s_config.wakeup_triggers |= RTC_ULP_TRIG_EN;
    return ESP_OK;
#else // CONFIG_ESP32_ULP_COPROC_ENABLED
    return ESP_ERR_INVALID_STATE;
#endif // CONFIG_ESP32_ULP_COPROC_ENABLED
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    s_config.wakeup_triggers |= (RTC_ULP_TRIG_EN | RTC_COCPU_TRIG_EN | RTC_COCPU_TRAP_TRIG_EN);
    return ESP_OK;
#endif
}

esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us)
{
    s_config.wakeup_triggers |= RTC_TIMER_TRIG_EN;
    s_config.sleep_duration = time_in_us;
    return ESP_OK;
}

static void timer_wakeup_prepare(void)
{
    uint32_t period = esp_clk_slowclk_cal_get();
    int64_t sleep_duration = (int64_t) s_config.sleep_duration - (int64_t) s_config.sleep_time_adjustment;
    if (sleep_duration < 0) {
        sleep_duration = 0;
    }

    int64_t ticks = rtc_time_us_to_slowclk(sleep_duration, period);
    rtc_hal_set_wakeup_timer(s_config.rtc_ticks_at_sleep_start + ticks);
}


#ifdef CONFIG_IDF_TARGET_ESP32S2
/* In deep sleep mode, only the sleep channel is supported, and other touch channels should be turned off. */
static void touch_wakeup_prepare(void)
{
    touch_pad_sleep_channel_t slp_config;
    touch_pad_fsm_stop();
    touch_pad_clear_channel_mask(TOUCH_PAD_BIT_MASK_ALL);
    touch_pad_sleep_channel_get_info(&slp_config);
    touch_pad_set_channel_mask(BIT(slp_config.touch_num));
    touch_pad_fsm_start();
}
#endif

esp_err_t esp_sleep_enable_touchpad_wakeup(void)
{
#ifdef CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT
    return ESP_ERR_NOT_SUPPORTED;
#endif
    if (s_config.wakeup_triggers & (RTC_EXT0_TRIG_EN)) {
        ESP_LOGE(TAG, "Conflicting wake-up trigger: ext0");
        return ESP_ERR_INVALID_STATE;
    }
    s_config.wakeup_triggers |= RTC_TOUCH_TRIG_EN;
    return ESP_OK;
}

touch_pad_t esp_sleep_get_touchpad_wakeup_status(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TOUCHPAD) {
        return TOUCH_PAD_MAX;
    }
    touch_pad_t pad_num;
    esp_err_t ret = touch_pad_get_wakeup_status(&pad_num); //TODO 723diff commit id:fda9ada1b
    assert(ret == ESP_OK && "wakeup reason is RTC_TOUCH_TRIG_EN but SENS_TOUCH_MEAS_EN is zero");
    return pad_num;
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
    s_config.ext0_rtc_gpio_num = rtc_io_number_get(gpio_num);
    s_config.ext0_trigger_level = level;
    s_config.wakeup_triggers |= RTC_EXT0_TRIG_EN;
    return ESP_OK;
}

static void ext0_wakeup_prepare(void)
{
    int rtc_gpio_num = s_config.ext0_rtc_gpio_num;
    rtcio_hal_ext0_set_wakeup_pin(rtc_gpio_num, s_config.ext0_trigger_level);
    rtcio_hal_function_select(rtc_gpio_num, RTCIO_FUNC_RTC);
    rtcio_hal_input_enable(rtc_gpio_num);
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
        rtc_gpio_mask |= BIT(rtc_io_number_get(gpio));
    }
    s_config.ext1_rtc_gpio_mask = rtc_gpio_mask;
    s_config.ext1_trigger_mode = mode;
    s_config.wakeup_triggers |= RTC_EXT1_TRIG_EN;
    return ESP_OK;
}

static void ext1_wakeup_prepare(void)
{
    // Configure all RTC IOs selected as ext1 wakeup inputs
    uint32_t rtc_gpio_mask = s_config.ext1_rtc_gpio_mask;
    for (int gpio = 0; gpio < GPIO_PIN_COUNT && rtc_gpio_mask != 0; ++gpio) {
        int rtc_pin = rtc_io_number_get(gpio);
        if ((rtc_gpio_mask & BIT(rtc_pin)) == 0) {
            continue;
        }
        // Route pad to RTC
        rtcio_hal_function_select(rtc_pin, RTCIO_FUNC_RTC);
        // set input enable in sleep mode
        rtcio_hal_input_enable(rtc_pin);

        // Pad configuration depends on RTC_PERIPH state in sleep mode
        if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] != ESP_PD_OPTION_ON) {
            // RTC_PERIPH will be powered down, so RTC_IO_ registers will
            // loose their state. Lock pad configuration.
            // Pullups/pulldowns also need to be disabled.
            rtcio_hal_pullup_disable(rtc_pin);
            rtcio_hal_pulldown_disable(rtc_pin);
            rtcio_hal_hold_enable(rtc_pin);
        }
        // Keep track of pins which are processed to bail out early
        rtc_gpio_mask &= ~BIT(rtc_pin);
    }

    // Clear state from previous wakeup
    rtc_hal_ext1_clear_wakeup_pins();
    // Set RTC IO pins and mode (any high, all low) to be used for wakeup
    rtc_hal_ext1_set_wakeup_pins(s_config.ext1_rtc_gpio_mask, s_config.ext1_trigger_mode);
}

uint64_t esp_sleep_get_ext1_wakeup_status(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT1) {
        return 0;
    }
    uint32_t status = rtc_hal_ext1_get_wakeup_pins();
    // Translate bit map of RTC IO numbers into the bit map of GPIO numbers
    uint64_t gpio_mask = 0;
    for (int gpio = 0; gpio < GPIO_PIN_COUNT; ++gpio) {
        if (!RTC_GPIO_IS_VALID_GPIO(gpio)) {
            continue;
        }
        int rtc_pin = rtc_io_number_get(gpio);
        if ((status & BIT(rtc_pin)) == 0) {
            continue;
        }
        gpio_mask |= 1ULL << gpio;
    }
    return gpio_mask;
}

esp_err_t esp_sleep_enable_gpio_wakeup(void)
{
    if (s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) {
        ESP_LOGE(TAG, "Conflicting wake-up triggers: touch / ULP");
        return ESP_ERR_INVALID_STATE;
    }
    s_config.wakeup_triggers |= RTC_GPIO_TRIG_EN;
    return ESP_OK;
}

esp_err_t esp_sleep_enable_uart_wakeup(int uart_num)
{
    if (uart_num == UART_NUM_0) {
        s_config.wakeup_triggers |= RTC_UART0_TRIG_EN;
    } else if (uart_num == UART_NUM_1) {
        s_config.wakeup_triggers |= RTC_UART1_TRIG_EN;
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t esp_sleep_enable_wifi_wakeup(void)
{
#if CONFIG_IDF_TARGET_ESP32
    return ESP_ERR_NOT_SUPPORTED;
#elif CONFIG_IDF_TARGET_ESP32S2
    s_config.wakeup_triggers |= RTC_WIFI_TRIG_EN;
    return ESP_OK;
#elif CONFIG_IDF_TARGET_ESP32S3
    s_config.wakeup_triggers |= RTC_MAC_TRIG_EN;
    return ESP_OK;
#endif
}


esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause(void)
{
    if (rtc_get_reset_reason(0) != DEEPSLEEP_RESET && !s_light_sleep_wakeup) {
        return ESP_SLEEP_WAKEUP_UNDEFINED;
    }

#ifdef CONFIG_IDF_TARGET_ESP32
    uint32_t wakeup_cause = REG_GET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_CAUSE);
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    uint32_t wakeup_cause = REG_GET_FIELD(RTC_CNTL_SLP_WAKEUP_CAUSE_REG, RTC_CNTL_WAKEUP_CAUSE);
#endif

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
    } else if (wakeup_cause & RTC_GPIO_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_GPIO;
    } else if (wakeup_cause & (RTC_UART0_TRIG_EN | RTC_UART1_TRIG_EN)) {
        return ESP_SLEEP_WAKEUP_UART;
#if CONFIG_IDF_TARGET_ESP32S2
    } else if (wakeup_cause & RTC_WIFI_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_WIFI;
    } else if (wakeup_cause & RTC_COCPU_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_ULP;
    } else if (wakeup_cause & RTC_COCPU_TRAP_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG;
#endif
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

static uint32_t get_power_down_flags(void)
{
    // Where needed, convert AUTO options to ON. Later interpret AUTO as OFF.

    // RTC_SLOW_MEM is needed for the ULP, so keep RTC_SLOW_MEM powered up if ULP
    // is used and RTC_SLOW_MEM is Auto.
    // If there is any data placed into .rtc.data or .rtc.bss segments, and
    // RTC_SLOW_MEM is Auto, keep it powered up as well.

    // Labels are defined in the linker script
    extern int _rtc_slow_length;

    if ((s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] == ESP_PD_OPTION_AUTO) &&
            ((size_t) &_rtc_slow_length > 0 ||
             (s_config.wakeup_triggers & RTC_ULP_TRIG_EN))) {
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

    // RTC_PERIPH is needed for EXT0 wakeup and GPIO wakeup.
    // If RTC_PERIPH is auto, and EXT0/GPIO aren't enabled, power down RTC_PERIPH.
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] == ESP_PD_OPTION_AUTO) {
        if (s_config.wakeup_triggers & (RTC_EXT0_TRIG_EN | RTC_GPIO_TRIG_EN)) {
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_ON;
        } else if (s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) {
            // In both rev. 0 and rev. 1 of ESP32, forcing power up of RTC_PERIPH
            // prevents ULP timer and touch FSMs from working correctly.
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_OFF;
        }
    }

    if (s_config.pd_options[ESP_PD_DOMAIN_XTAL] == ESP_PD_OPTION_AUTO) {
        s_config.pd_options[ESP_PD_DOMAIN_XTAL] = ESP_PD_OPTION_OFF;
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

#ifdef CONFIG_IDF_TARGET_ESP32
    pd_flags |= RTC_SLEEP_PD_XTAL;
#endif

#if ((defined CONFIG_ESP32_RTC_CLK_SRC_EXT_CRYS) && (defined CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT))
    if ((s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) == 0) {
    // If enabled EXT1 only and enable the additional current by touch, should be keep RTC_PERIPH power on.
        pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
    }
#endif

    return pd_flags;
}

void esp_deep_sleep_disable_rom_logging(void)
{
    esp_rom_disable_logging();
}

