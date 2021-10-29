/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_private/esp_timer_private.h"
#include "esp_private/system_internal.h"
#include "esp_log.h"
#include "esp_newlib.h"
#include "esp_timer.h"
#include "esp_ipc_isr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "driver/rtc_io.h"
#include "hal/rtc_io_hal.h"

#include "driver/uart.h"

#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"

#include "hal/wdt_hal.h"
#include "hal/rtc_hal.h"
#include "hal/uart_hal.h"
#if SOC_TOUCH_SENSOR_NUM > 0
#include "hal/touch_sensor_hal.h"
#include "driver/touch_sensor.h"
#include "driver/touch_sensor_common.h"
#endif
#include "hal/clk_gate_ll.h"

#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "brownout.h"
#include "esp_private/sleep_retention.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#include "esp32/rom/rtc.h"
#include "esp32/clk.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/clk.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/rtc.h"
#include "soc/extmem_reg.h"
#include "esp_private/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/clk.h"
#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/rtc.h"
#include "soc/extmem_reg.h"
#include "esp_private/sleep_mac_bb.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/clk.h"
#include "esp32c3/rom/cache.h"
#include "esp32c3/rom/rtc.h"
#include "soc/extmem_reg.h"
#include "esp_private/sleep_mac_bb.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/clk.h"
#include "esp32h2/rom/cache.h"
#include "esp32h2/rom/rtc.h"
#include "soc/extmem_reg.h"
#endif

// If light sleep time is less than that, don't power down flash
#define FLASH_PD_MIN_SLEEP_TIME_US  2000

// Time from VDD_SDIO power up to first flash read in ROM code
#define VDD_SDIO_POWERUP_TO_FLASH_READ_US 700

// Cycles for RTC Timer clock source (internal oscillator) calibrate
#define RTC_CLK_SRC_CAL_CYCLES      (10)

#ifdef CONFIG_IDF_TARGET_ESP32
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (212)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (60)
#elif CONFIG_IDF_TARGET_ESP32S2
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (147)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (28)
#elif CONFIG_IDF_TARGET_ESP32S3
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (382)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (133)
#elif CONFIG_IDF_TARGET_ESP32C3
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (105)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (37)
#elif CONFIG_IDF_TARGET_ESP32H2
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32H2_DEFAULT_CPU_FREQ_MHZ
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (105)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (37)
#endif

#define LIGHT_SLEEP_TIME_OVERHEAD_US        DEFAULT_HARDWARE_OUT_OVERHEAD_US
#ifdef CONFIG_ESP_SYSTEM_RTC_EXT_XTAL
#define DEEP_SLEEP_TIME_OVERHEAD_US         (650 + 100 * 240 / DEFAULT_CPU_FREQ_MHZ)
#else
#define DEEP_SLEEP_TIME_OVERHEAD_US         (250 + 100 * 240 / DEFAULT_CPU_FREQ_MHZ)
#endif

#if defined(CONFIG_IDF_TARGET_ESP32) && defined(CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY)
#define DEEP_SLEEP_WAKEUP_DELAY     CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY
#elif defined(CONFIG_IDF_TARGET_ESP32S3) && defined(CONFIG_ESP32S3_DEEP_SLEEP_WAKEUP_DELAY)
#define DEEP_SLEEP_WAKEUP_DELAY     CONFIG_ESP32S3_DEEP_SLEEP_WAKEUP_DELAY
#else
#define DEEP_SLEEP_WAKEUP_DELAY     0
#endif

extern void periph_inform_out_light_sleep_overhead(uint32_t out_light_sleep_time);

// Minimal amount of time we can sleep for
#define LIGHT_SLEEP_MIN_TIME_US     200

#define RTC_MODULE_SLEEP_PREPARE_CYCLES (6)

#define CHECK_SOURCE(source, value, mask) ((s_config.wakeup_triggers & mask) && \
                                            (source == value))

/**
 * Internal structure which holds all requested deep sleep parameters
 */
typedef struct {
    esp_sleep_pd_option_t pd_options[ESP_PD_DOMAIN_MAX];
    uint64_t sleep_duration;
    uint32_t wakeup_triggers : 15;
    uint32_t ext1_trigger_mode : 1;
    uint32_t ext1_rtc_gpio_mask : 18;
    uint32_t ext0_trigger_level : 1;
    uint32_t ext0_rtc_gpio_num : 5;
    uint32_t gpio_wakeup_mask : 6;
    uint32_t gpio_trigger_mode : 6;
    uint32_t sleep_time_adjustment;
    uint32_t ccount_ticks_record;
    uint32_t sleep_time_overhead_out;
    uint32_t rtc_clk_cal_period;
    uint64_t rtc_ticks_at_sleep_start;
} sleep_config_t;

static sleep_config_t s_config = {
    .pd_options = {
        ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO,
#if SOC_PM_SUPPORT_CPU_PD
        ESP_PD_OPTION_AUTO,
#endif
        ESP_PD_OPTION_AUTO, ESP_PD_OPTION_AUTO
    },
    .ccount_ticks_record = 0,
    .sleep_time_overhead_out = DEFAULT_SLEEP_OUT_OVERHEAD_US,
    .wakeup_triggers = 0
};

/* Internal variable used to track if light sleep wakeup sources are to be
   expected when determining wakeup cause. */
static bool s_light_sleep_wakeup = false;

/* Updating RTC_MEMORY_CRC_REG register via set_rtc_memory_crc()
   is not thread-safe, so we need to disable interrupts before going to deep sleep. */
static portMUX_TYPE spinlock_rtc_deep_sleep = portMUX_INITIALIZER_UNLOCKED;

static const char *TAG = "sleep";

static uint32_t get_power_down_flags(void);
#if SOC_PM_SUPPORT_EXT_WAKEUP
static void ext0_wakeup_prepare(void);
static void ext1_wakeup_prepare(void);
#endif
static void timer_wakeup_prepare(void);
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
static void touch_wakeup_prepare(void);
#endif
#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
static void esp_deep_sleep_wakeup_prepare(void);
#endif

#if SOC_PM_SUPPORT_DEEPSLEEP_VERIFY_STUB_ONLY
static RTC_FAST_ATTR esp_deep_sleep_wake_stub_fn_t wake_stub_fn_handler = NULL;

static void RTC_IRAM_ATTR __attribute__((used, noinline)) esp_wake_stub_start(void)
{
    if (wake_stub_fn_handler) {
        (*wake_stub_fn_handler)();
    }
}

/* We must have a default deep sleep wake stub entry function, which must be
 * located at the start address of the RTC fast memory, and its implementation
 * must be simple enough to ensure that there is no litteral data before the
 * wake stub entry, otherwise, the litteral data before the wake stub entry
 * will not be CRC checked. */
static void __attribute__((section(".rtc.entry.text"))) esp_wake_stub_entry(void)
{
#define _SYM2STR(s) # s
#define SYM2STR(s)  _SYM2STR(s)
    // call4 has a larger effective addressing range (-524284 to 524288 bytes),
    // which is sufficient for instruction addressing in RTC fast memory.
    __asm__ __volatile__ ("call4 " SYM2STR(esp_wake_stub_start) "\n");
}
#endif // SOC_PM_SUPPORT_DEEPSLEEP_VERIFY_STUB_ONLY

/* Wake from deep sleep stub
   See esp_deepsleep.h esp_wake_deep_sleep() comments for details.
*/
esp_deep_sleep_wake_stub_fn_t esp_get_deep_sleep_wake_stub(void)
{
#if SOC_PM_SUPPORT_DEEPSLEEP_VERIFY_STUB_ONLY
    esp_deep_sleep_wake_stub_fn_t stub_ptr = wake_stub_fn_handler;
#else
    esp_deep_sleep_wake_stub_fn_t stub_ptr = (esp_deep_sleep_wake_stub_fn_t) REG_READ(RTC_ENTRY_ADDR_REG);
#endif
    if (!esp_ptr_executable(stub_ptr)) {
        return NULL;
    }
    return stub_ptr;
}

void esp_set_deep_sleep_wake_stub(esp_deep_sleep_wake_stub_fn_t new_stub)
{
#if SOC_PM_SUPPORT_DEEPSLEEP_VERIFY_STUB_ONLY
    wake_stub_fn_handler = new_stub;
#else
    REG_WRITE(RTC_ENTRY_ADDR_REG, (uint32_t)new_stub);
#endif
}

void RTC_IRAM_ATTR esp_default_wake_deep_sleep(void)
{
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
#else
        if (periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            esp_rom_uart_tx_wait_idle(i);
        }
#endif
    }
}

static void IRAM_ATTR suspend_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
#ifndef CONFIG_IDF_TARGET_ESP32
        if (!periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            continue;
        }
#endif
        uart_ll_force_xoff(i);
#if SOC_UART_SUPPORT_FSM_TX_WAIT_SEND
        uint32_t uart_fsm = 0;
        do {
            uart_fsm = uart_ll_get_fsm_status(i);
        } while (!(uart_fsm == UART_LL_FSM_IDLE || uart_fsm == UART_LL_FSM_TX_WAIT_SEND));
#else
        while (uart_ll_get_fsm_status(i) != 0) {}
#endif
    }
}

static void IRAM_ATTR resume_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
#ifndef CONFIG_IDF_TARGET_ESP32
        if (!periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            continue;
        }
#endif
        uart_ll_force_xon(i);
    }
}

inline static void IRAM_ATTR misc_modules_sleep_prepare(void)
{
#if CONFIG_MAC_BB_PD
    mac_bb_power_down_cb_execute();
#endif
#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
    gpio_sleep_mode_config_apply();
#endif
#if SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD
    sleep_enable_memory_retention();
#endif
}

inline static void IRAM_ATTR misc_modules_wake_prepare(void)
{
#if SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD
    sleep_disable_memory_retention();
#endif
#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
    gpio_sleep_mode_config_unapply();
#endif
#if CONFIG_MAC_BB_PD
    mac_bb_power_up_cb_execute();
#endif
}

inline static uint32_t IRAM_ATTR call_rtc_sleep_start(uint32_t reject_triggers, uint32_t lslp_mem_inf_fpu);

static uint32_t IRAM_ATTR esp_sleep_start(uint32_t pd_flags)
{
    // Stop UART output so that output is not lost due to APB frequency change.
    // For light sleep, suspend UART output — it will resume after wakeup.
    // For deep sleep, wait for the contents of UART FIFO to be sent.
    bool deep_sleep = pd_flags & RTC_SLEEP_PD_DIG;

    if (deep_sleep) {
        flush_uarts();
    } else {
        suspend_uarts();
    }

    // Save current frequency and switch to XTAL
    rtc_cpu_freq_config_t cpu_freq_config;
    rtc_clk_cpu_freq_get_config(&cpu_freq_config);
    rtc_clk_cpu_freq_set_xtal();

#if SOC_PM_SUPPORT_EXT_WAKEUP
    // Configure pins for external wakeup
    if (s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
        ext0_wakeup_prepare();
    }
    if (s_config.wakeup_triggers & RTC_EXT1_TRIG_EN) {
        ext1_wakeup_prepare();
    }
#endif

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
    if (s_config.wakeup_triggers & RTC_GPIO_TRIG_EN) {
        esp_deep_sleep_wakeup_prepare();
    }
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
    // Enable ULP wakeup
    if (s_config.wakeup_triggers & RTC_ULP_TRIG_EN) {
        rtc_hal_ulp_wakeup_enable();
    }
#endif

    misc_modules_sleep_prepare();

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    if (deep_sleep) {
        if (s_config.wakeup_triggers & RTC_TOUCH_TRIG_EN) {
            touch_wakeup_prepare();
#if CONFIG_IDF_TARGET_ESP32S2
            /* Workaround: In deep sleep, for ESP32S2, Power down the RTC_PERIPH will change the slope configuration of Touch sensor sleep pad.
             * The configuration change will change the reading of the sleep pad, which will cause the touch wake-up sensor to trigger falsely.
             */
            pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
#endif
        }
    } else {
        /* In light sleep, the RTC_PERIPH power domain should be in the power-on state (Power on the touch circuit in light sleep),
         * otherwise the touch sensor FSM will be cleared, causing touch sensor false triggering.
         */
        if (touch_ll_get_fsm_state()) { // Check if the touch sensor is working properly.
            pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
        }
    }
#endif
    uint32_t reject_triggers = 0;
    if ((pd_flags & RTC_SLEEP_PD_DIG) == 0 && (s_config.wakeup_triggers & RTC_GPIO_TRIG_EN)) {
        /* Light sleep, enable sleep reject for faster return from this function,
         * in case the wakeup is already triggerred.
         */
#if CONFIG_IDF_TARGET_ESP32
        reject_triggers = RTC_CNTL_LIGHT_SLP_REJECT_EN_M | RTC_CNTL_GPIO_REJECT_EN_M;
#else
        reject_triggers = s_config.wakeup_triggers;
#endif
    }

    // Enter sleep
    rtc_sleep_config_t config = RTC_SLEEP_CONFIG_DEFAULT(pd_flags);
    rtc_sleep_init(config);

    // Set state machine time for light sleep
    if (!deep_sleep) {
        rtc_sleep_low_init(s_config.rtc_clk_cal_period);
    }

    // Configure timer wakeup
    if (s_config.wakeup_triggers & RTC_TIMER_TRIG_EN) {
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

#if SOC_PM_SUPPORT_DEEPSLEEP_VERIFY_STUB_ONLY
        extern char _rtc_text_start[];
#if CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM
        extern char _rtc_noinit_end[];
        size_t rtc_fast_length = (size_t)_rtc_noinit_end - (size_t)_rtc_text_start;
#else
        extern char _rtc_force_fast_end[];
        size_t rtc_fast_length = (size_t)_rtc_force_fast_end - (size_t)_rtc_text_start;
#endif
        esp_rom_set_rtc_wake_addr((esp_rom_wake_func_t)esp_wake_stub_entry, rtc_fast_length);
        result = call_rtc_sleep_start(reject_triggers, config.lslp_mem_inf_fpu);
#else
#if !CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
        /* If not possible stack is in RTC FAST memory, use the ROM function to calculate the CRC and save ~140 bytes IRAM */
        set_rtc_memory_crc();
        result = call_rtc_sleep_start(reject_triggers, config.lslp_mem_inf_fpu);
#else
        /* Otherwise, need to call the dedicated soc function for this */
        result = rtc_deep_sleep_start(s_config.wakeup_triggers, reject_triggers);
#endif
#endif // SOC_PM_SUPPORT_DEEPSLEEP_VERIFY_STUB_ONLY

        portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
    } else {
        result = call_rtc_sleep_start(reject_triggers, config.lslp_mem_inf_fpu);
    }

    // Restore CPU frequency
    rtc_clk_cpu_freq_set_config(&cpu_freq_config);

    if (!deep_sleep) {
        s_config.ccount_ticks_record = cpu_ll_get_cycle_count();
    }

    misc_modules_wake_prepare();

    // re-enable UART output
    resume_uarts();

    return result;
}

inline static uint32_t IRAM_ATTR call_rtc_sleep_start(uint32_t reject_triggers, uint32_t lslp_mem_inf_fpu)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    return rtc_sleep_start(s_config.wakeup_triggers, reject_triggers);
#else
    return rtc_sleep_start(s_config.wakeup_triggers, reject_triggers, lslp_mem_inf_fpu);
#endif
}

void IRAM_ATTR esp_deep_sleep_start(void)
{
#if CONFIG_IDF_TARGET_ESP32S2
    /* Due to hardware limitations, on S2 the brownout detector sometimes trigger during deep sleep
       to circumvent this we disable the brownout detector before sleeping  */
    esp_brownout_disable();
#endif //CONFIG_IDF_TARGET_ESP32S2

    esp_sync_counters_rtc_and_frc();

    /* Disable interrupts and stall another core in case another task writes
     * to RTC memory while we calculate RTC memory CRC.
     */
    portENTER_CRITICAL(&spinlock_rtc_deep_sleep);
    esp_ipc_isr_stall_other_cpu();

    // record current RTC time
    s_config.rtc_ticks_at_sleep_start = rtc_time_get();

    // Configure wake stub
    if (esp_get_deep_sleep_wake_stub() == NULL) {
        esp_set_deep_sleep_wake_stub(esp_wake_deep_sleep);
    }

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

    s_config.rtc_clk_cal_period = esp_clk_slowclk_cal_get();

    // Correct the sleep time
    s_config.sleep_time_adjustment = DEEP_SLEEP_TIME_OVERHEAD_US;

    uint32_t force_pd_flags = RTC_SLEEP_PD_DIG | RTC_SLEEP_PD_VDDSDIO;

#if SOC_PM_SUPPORT_WIFI_PD
    force_pd_flags |= RTC_SLEEP_PD_WIFI;
#endif

#if SOC_PM_SUPPORT_BT_PD
    force_pd_flags |= RTC_SLEEP_PD_BT;
#endif

    // Enter sleep
    esp_sleep_start(force_pd_flags | pd_flags);

    // Because RTC is in a slower clock domain than the CPU, it
    // can take several CPU cycles for the sleep mode to start.
    while (1) {
        ;
    }
    // Never returns here
    esp_ipc_isr_release_other_cpu();
    portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
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
    s_config.ccount_ticks_record = cpu_ll_get_cycle_count();
    static portMUX_TYPE light_sleep_lock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&light_sleep_lock);
    /* We will be calling esp_timer_private_advance inside DPORT access critical
     * section. Make sure the code on the other CPU is not holding esp_timer
     * lock, otherwise there will be deadlock.
     */
    esp_timer_private_lock();

    s_config.rtc_ticks_at_sleep_start = rtc_time_get();
    uint32_t ccount_at_sleep_start = cpu_ll_get_cycle_count();
    uint64_t frc_time_at_start = esp_system_get_time();
    uint32_t sleep_time_overhead_in = (ccount_at_sleep_start - s_config.ccount_ticks_record) / (esp_clk_cpu_freq() / 1000000ULL);

    esp_ipc_isr_stall_other_cpu();

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

#ifdef CONFIG_ESP_SLEEP_RTC_BUS_ISO_WORKAROUND
    pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
#endif

    // Re-calibrate the RTC Timer clock
#ifdef CONFIG_ESP_SYSTEM_RTC_EXT_XTAL
    uint64_t time_per_us = 1000000ULL;
    s_config.rtc_clk_cal_period = (time_per_us << RTC_CLK_CAL_FRACT) / rtc_clk_slow_freq_get_hz();
#elif defined(CONFIG_ESP32S2_RTC_CLK_SRC_INT_RC)
    s_config.rtc_clk_cal_period = rtc_clk_cal_cycling(RTC_CAL_RTC_MUX, RTC_CLK_SRC_CAL_CYCLES);
    esp_clk_slowclk_cal_set(s_config.rtc_clk_cal_period);
#else
    s_config.rtc_clk_cal_period = rtc_clk_cal(RTC_CAL_RTC_MUX, RTC_CLK_SRC_CAL_CYCLES);
    esp_clk_slowclk_cal_set(s_config.rtc_clk_cal_period);
#endif

    /*
     * Adjustment time consists of parts below:
     * 1. Hardware time waiting for internal 8M oscilate clock and XTAL;
     * 2. Hardware state swithing time of the rtc main state machine;
     * 3. Code execution time when clock is not stable;
     * 4. Code execution time which can be measured;
     */

    uint32_t rtc_cntl_xtl_buf_wait_slp_cycles = rtc_time_us_to_slowclk(RTC_CNTL_XTL_BUF_WAIT_SLP_US, s_config.rtc_clk_cal_period);
    s_config.sleep_time_adjustment = LIGHT_SLEEP_TIME_OVERHEAD_US + sleep_time_overhead_in + s_config.sleep_time_overhead_out
                                     + rtc_time_slowclk_to_us(rtc_cntl_xtl_buf_wait_slp_cycles + RTC_CNTL_CK8M_WAIT_SLP_CYCLES + RTC_CNTL_WAKEUP_DELAY_CYCLES, s_config.rtc_clk_cal_period);

    // Decide if VDD_SDIO needs to be powered down;
    // If it needs to be powered down, adjust sleep time.
    const uint32_t flash_enable_time_us = VDD_SDIO_POWERUP_TO_FLASH_READ_US + DEEP_SLEEP_WAKEUP_DELAY;

    /**
     * If VDD_SDIO power domain is requested to be turned off, bit `RTC_SLEEP_PD_VDDSDIO`
     * will be set in `pd_flags`.
     */
    if (pd_flags & RTC_SLEEP_PD_VDDSDIO) {
        /*
        * When VDD_SDIO power domain has to be turned off, the minimum sleep time of the
        * system needs to meet the sum below:
        * 1. Wait time for the flash power-on after waking up;
        * 2. The execution time of codes between RTC Timer get start time
        *    with hardware starts to switch state to sleep;
        * 3. The hardware state switching time of the rtc state machine during
        *    sleep and wake-up. This process requires 6 cycles to complete.
        *    The specific hardware state switching process and the cycles
        *    consumed are rtc_cpu_run_stall(1), cut_pll_rtl(2), cut_8m(1),
        *    min_protect(2);
        * 4. All the adjustment time which is s_config.sleep_time_adjustment below.
        */
        const uint32_t vddsdio_pd_sleep_duration = MAX(FLASH_PD_MIN_SLEEP_TIME_US,
                        flash_enable_time_us + LIGHT_SLEEP_MIN_TIME_US + s_config.sleep_time_adjustment
                        + rtc_time_slowclk_to_us(RTC_MODULE_SLEEP_PREPARE_CYCLES, s_config.rtc_clk_cal_period));

        if (s_config.sleep_duration > vddsdio_pd_sleep_duration) {
            if (s_config.sleep_time_overhead_out < flash_enable_time_us) {
                s_config.sleep_time_adjustment += flash_enable_time_us;
            }
        } else {
            /**
             * Minimum sleep time is not enough, then keep the VDD_SDIO power
             * domain on.
             */
            pd_flags &= ~RTC_SLEEP_PD_VDDSDIO;
            if (s_config.sleep_time_overhead_out > flash_enable_time_us) {
                s_config.sleep_time_adjustment -= flash_enable_time_us;
            }
        }
    }

    periph_inform_out_light_sleep_overhead(s_config.sleep_time_adjustment - sleep_time_overhead_in);

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
#ifdef CONFIG_IDF_TARGET_ESP32C3
    /**
     * On esp32c3, rtc_time_get() is non-blocking, esp_system_get_time() is
     * blocking, and the measurement data shows that this order is better.
     */
    uint64_t frc_time_at_end = esp_system_get_time();
    uint64_t rtc_ticks_at_end = rtc_time_get();
#else
    uint64_t rtc_ticks_at_end = rtc_time_get();
    uint64_t frc_time_at_end = esp_system_get_time();
#endif

    uint64_t rtc_time_diff = rtc_time_slowclk_to_us(rtc_ticks_at_end - s_config.rtc_ticks_at_sleep_start, s_config.rtc_clk_cal_period);
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
    esp_ipc_isr_release_other_cpu();
    if (!wdt_was_enabled) {
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }
    portEXIT_CRITICAL(&light_sleep_lock);
    s_config.sleep_time_overhead_out = (cpu_ll_get_cycle_count() - s_config.ccount_ticks_record) / (esp_clk_cpu_freq() / 1000000ULL);
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
#if SOC_PM_SUPPORT_EXT_WAKEUP
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_EXT0, RTC_EXT0_TRIG_EN)) {
        s_config.ext0_rtc_gpio_num = 0;
        s_config.ext0_trigger_level = 0;
        s_config.wakeup_triggers &= ~RTC_EXT0_TRIG_EN;
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_EXT1, RTC_EXT1_TRIG_EN)) {
        s_config.ext1_rtc_gpio_mask = 0;
        s_config.ext1_trigger_mode = 0;
        s_config.wakeup_triggers &= ~RTC_EXT1_TRIG_EN;
#endif
#if SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_TOUCHPAD, RTC_TOUCH_TRIG_EN)) {
        s_config.wakeup_triggers &= ~RTC_TOUCH_TRIG_EN;
#endif
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
#if ((defined CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT) || (defined CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT_V2))
    ESP_LOGE(TAG, "Failed to enable wakeup when provide current to external 32kHz crystal");
    return ESP_ERR_NOT_SUPPORTED;
#endif
#ifdef CONFIG_ESP32_ULP_COPROC_ENABLED
    if (s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
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
#else
    return ESP_ERR_NOT_SUPPORTED;
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
    int64_t sleep_duration = (int64_t) s_config.sleep_duration - (int64_t) s_config.sleep_time_adjustment;
    if (sleep_duration < 0) {
        sleep_duration = 0;
    }

    int64_t ticks = rtc_time_us_to_slowclk(sleep_duration, s_config.rtc_clk_cal_period);
    rtc_hal_set_wakeup_timer(s_config.rtc_ticks_at_sleep_start + ticks);
}

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
/* In deep sleep mode, only the sleep channel is supported, and other touch channels should be turned off. */
static void touch_wakeup_prepare(void)
{
    uint16_t sleep_cycle = 0;
    uint16_t meas_times = 0;
    touch_pad_t touch_num = TOUCH_PAD_NUM0;
    touch_ll_sleep_get_channel_num(&touch_num); // Check if the sleep pad is enabled.
    if ((touch_num > TOUCH_PAD_NUM0) && (touch_num < TOUCH_PAD_MAX) && touch_ll_get_fsm_state()) {
        touch_ll_stop_fsm();
        touch_ll_clear_channel_mask(TOUCH_PAD_BIT_MASK_ALL);
        touch_ll_intr_clear(TOUCH_PAD_INTR_MASK_ALL); // Clear state from previous wakeup
        touch_hal_sleep_channel_get_work_time(&sleep_cycle, &meas_times);
        touch_ll_set_meas_times(meas_times);
        touch_ll_set_sleep_time(sleep_cycle);
        touch_ll_set_channel_mask(BIT(touch_num));
        touch_ll_start_fsm();
    }
}
#endif

#if SOC_TOUCH_SENSOR_NUM > 0

esp_err_t esp_sleep_enable_touchpad_wakeup(void)
{
#if ((defined CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT) || (defined CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT_V2))
    ESP_LOGE(TAG, "Failed to enable wakeup when provide current to external 32kHz crystal");
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
    return (ret == ESP_OK) ? pad_num : TOUCH_PAD_MAX;
}

#endif // SOC_TOUCH_SENSOR_NUM > 0

bool esp_sleep_is_valid_wakeup_gpio(gpio_num_t gpio_num)
{
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
    return RTC_GPIO_IS_VALID_GPIO(gpio_num);
#else
    return GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(gpio_num);
#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
}

#if SOC_PM_SUPPORT_EXT_WAKEUP

esp_err_t esp_sleep_enable_ext0_wakeup(gpio_num_t gpio_num, int level)
{
    if (level < 0 || level > 1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!esp_sleep_is_valid_wakeup_gpio(gpio_num)) {
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
        if (!esp_sleep_is_valid_wakeup_gpio(gpio)) {
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
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        // Route pad to RTC
        rtcio_hal_function_select(rtc_pin, RTCIO_FUNC_RTC);
        // set input enable in sleep mode
        rtcio_hal_input_enable(rtc_pin);
#endif

        // Pad configuration depends on RTC_PERIPH state in sleep mode
        if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] != ESP_PD_OPTION_ON) {
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
            // RTC_PERIPH will be powered down, so RTC_IO_ registers will
            // loose their state. Lock pad configuration.
            // Pullups/pulldowns also need to be disabled.
            rtcio_hal_pullup_disable(rtc_pin);
            rtcio_hal_pulldown_disable(rtc_pin);
#endif
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
        if (!esp_sleep_is_valid_wakeup_gpio(gpio)) {
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

#endif // SOC_PM_SUPPORT_EXT_WAKEUP

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
uint64_t esp_sleep_get_gpio_wakeup_status(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_GPIO) {
        return 0;
    }

    return rtc_hal_gpio_get_wakeup_pins();
}

static void esp_deep_sleep_wakeup_prepare(void)
{
    for (gpio_num_t gpio_idx = GPIO_NUM_0; gpio_idx < GPIO_NUM_MAX; gpio_idx++) {
        if (((1ULL << gpio_idx) & s_config.gpio_wakeup_mask) == 0) {
            continue;
        }
        if (s_config.gpio_trigger_mode & BIT(gpio_idx)) {
            ESP_ERROR_CHECK(gpio_pullup_dis(gpio_idx));
            ESP_ERROR_CHECK(gpio_pulldown_en(gpio_idx));
        } else {
            ESP_ERROR_CHECK(gpio_pullup_en(gpio_idx));
            ESP_ERROR_CHECK(gpio_pulldown_dis(gpio_idx));
        }
        rtc_hal_gpio_set_wakeup_pins();
        ESP_ERROR_CHECK(gpio_hold_en(gpio_idx));
    }
}

esp_err_t esp_deep_sleep_enable_gpio_wakeup(uint64_t gpio_pin_mask, esp_deepsleep_gpio_wake_up_mode_t mode)
{
    if (mode > ESP_GPIO_WAKEUP_GPIO_HIGH) {
        ESP_LOGE(TAG, "invalid mode");
        return ESP_ERR_INVALID_ARG;
    }
    gpio_int_type_t intr_type = ((mode == ESP_GPIO_WAKEUP_GPIO_LOW) ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
    esp_err_t err = ESP_OK;
    for (gpio_num_t gpio_idx = GPIO_NUM_0; gpio_idx < GPIO_NUM_MAX; gpio_idx++, gpio_pin_mask >>= 1) {
        if ((gpio_pin_mask & 1) == 0) {
            continue;
        }
        if (!esp_sleep_is_valid_wakeup_gpio(gpio_idx)) {
            ESP_LOGE(TAG, "invalid mask, please ensure gpio number is no more than 5");
            return ESP_ERR_INVALID_ARG;
        }
        err = gpio_deep_sleep_wakeup_enable(gpio_idx, intr_type);

        s_config.gpio_wakeup_mask |= BIT(gpio_idx);
        if (mode == ESP_GPIO_WAKEUP_GPIO_HIGH) {
            s_config.gpio_trigger_mode |= (mode << gpio_idx);
        } else {
            s_config.gpio_trigger_mode &= ~(mode << gpio_idx);
        }
    }
    s_config.wakeup_triggers |= RTC_GPIO_TRIG_EN;
    rtc_hal_gpio_clear_wakeup_pins();
    return err;
}

#endif //SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

esp_err_t esp_sleep_enable_gpio_wakeup(void)
{
#if CONFIG_IDF_TARGET_ESP32
    if (s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) {
        ESP_LOGE(TAG, "Conflicting wake-up triggers: touch / ULP");
        return ESP_ERR_INVALID_STATE;
    }
#endif
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
#if SOC_PM_SUPPORT_WIFI_WAKEUP
    s_config.wakeup_triggers |= RTC_WIFI_TRIG_EN;
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_sleep_disable_wifi_wakeup(void)
{
#if SOC_PM_SUPPORT_WIFI_WAKEUP
    s_config.wakeup_triggers &= (~RTC_WIFI_TRIG_EN);
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause(void)
{
    if (esp_rom_get_reset_reason(0) != RESET_REASON_CORE_DEEP_SLEEP && !s_light_sleep_wakeup) {
        return ESP_SLEEP_WAKEUP_UNDEFINED;
    }

#ifdef CONFIG_IDF_TARGET_ESP32
    uint32_t wakeup_cause = REG_GET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_CAUSE);
#else
    uint32_t wakeup_cause = REG_GET_FIELD(RTC_CNTL_SLP_WAKEUP_CAUSE_REG, RTC_CNTL_WAKEUP_CAUSE);
#endif

    if (wakeup_cause & RTC_TIMER_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_TIMER;
    } else if (wakeup_cause & RTC_GPIO_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_GPIO;
    } else if (wakeup_cause & (RTC_UART0_TRIG_EN | RTC_UART1_TRIG_EN)) {
        return ESP_SLEEP_WAKEUP_UART;
#if SOC_PM_SUPPORT_EXT_WAKEUP
    } else if (wakeup_cause & RTC_EXT0_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_EXT0;
    } else if (wakeup_cause & RTC_EXT1_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_EXT1;
#endif
#if SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP
    } else if (wakeup_cause & RTC_TOUCH_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_TOUCHPAD;
#endif
#if SOC_ULP_SUPPORTED
    } else if (wakeup_cause & RTC_ULP_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_ULP;
#endif
#if SOC_PM_SUPPORT_WIFI_WAKEUP
    } else if (wakeup_cause & RTC_WIFI_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_WIFI;
#endif
#if SOC_PM_SUPPORT_BT_WAKEUP
    } else if (wakeup_cause & RTC_BT_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_BT;
#endif
#if CONFIG_IDF_TARGET_ESP32S2
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

#if SOC_RTC_SLOW_MEM_SUPPORTED && SOC_ULP_SUPPORTED
    // Labels are defined in the linker script
    extern int _rtc_slow_length;

    if ((s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] == ESP_PD_OPTION_AUTO) &&
            ((size_t) &_rtc_slow_length > 0 ||
             (s_config.wakeup_triggers & RTC_ULP_TRIG_EN))) {
        s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] = ESP_PD_OPTION_ON;
    }
#endif

#if !CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    /* RTC_FAST_MEM is needed for deep sleep stub.
       If RTC_FAST_MEM is Auto, keep it powered on, so that deep sleep stub can run.
       In the new chip revision, deep sleep stub will be optional, and this can be changed. */
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM] == ESP_PD_OPTION_AUTO) {
        s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM] = ESP_PD_OPTION_ON;
    }
#else
    /* If RTC_FAST_MEM is used for heap, force RTC_FAST_MEM to be powered on. */
    s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM] = ESP_PD_OPTION_ON;
#endif

    // RTC_PERIPH is needed for EXT0 wakeup and GPIO wakeup.
    // If RTC_PERIPH is auto, and EXT0/GPIO aren't enabled, power down RTC_PERIPH.
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] == ESP_PD_OPTION_AUTO) {
#if SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP
        uint32_t wakeup_source = RTC_TOUCH_TRIG_EN;
#if SOC_ULP_SUPPORTED
        wakeup_source |= RTC_ULP_TRIG_EN;
#endif
        if (s_config.wakeup_triggers & (RTC_EXT0_TRIG_EN | RTC_GPIO_TRIG_EN)) {
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_ON;
        } else if (s_config.wakeup_triggers & wakeup_source) {
            // In both rev. 0 and rev. 1 of ESP32, forcing power up of RTC_PERIPH
            // prevents ULP timer and touch FSMs from working correctly.
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_OFF;
        }
#else
        if (s_config.wakeup_triggers & RTC_GPIO_TRIG_EN) {
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_ON;
        } else {
            s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] = ESP_PD_OPTION_OFF;
        }
#endif // SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP
    }

#if SOC_PM_SUPPORT_CPU_PD
    if (!cpu_domain_pd_allowed()) {
        s_config.pd_options[ESP_PD_DOMAIN_CPU] = ESP_PD_OPTION_ON;
    }
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
    s_config.pd_options[ESP_PD_DOMAIN_XTAL] = ESP_PD_OPTION_OFF;
#endif

    const char *option_str[] = {"OFF", "ON", "AUTO(OFF)" /* Auto works as OFF */};
    ESP_LOGD(TAG, "RTC_PERIPH: %s", option_str[s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH]]);
#if SOC_RTC_SLOW_MEM_SUPPORTED
    ESP_LOGD(TAG, "RTC_SLOW_MEM: %s", option_str[s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM]]);
#endif
    ESP_LOGD(TAG, "RTC_FAST_MEM: %s", option_str[s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM]]);

    // Prepare flags based on the selected options
    uint32_t pd_flags = 0;
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_FAST_MEM] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_FAST_MEM;
    }
#if SOC_RTC_SLOW_MEM_SUPPORTED
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_SLOW_MEM;
    }
#endif
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_PERIPH;
    }

#if SOC_PM_SUPPORT_CPU_PD
    if (s_config.pd_options[ESP_PD_DOMAIN_CPU] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_CPU;
    }
#endif
    if (s_config.pd_options[ESP_PD_DOMAIN_RTC8M] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_INT_8M;
    }
    if (s_config.pd_options[ESP_PD_DOMAIN_XTAL] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_XTAL;
    }

    /**
     * VDD_SDIO power domain shall be kept on during the light sleep
     * when CONFIG_ESP_SLEEP_POWER_DOWN_FLASH is not set and off when it is set.
     * The application can still force the power domain to remain on by calling
     * `esp_sleep_pd_config` before getting into light sleep mode.
     *
     * In deep sleep mode, the power domain will be turned off, regardless the
     * value of this field.
     */
    if (s_config.pd_options[ESP_PD_DOMAIN_VDDSDIO] == ESP_PD_OPTION_AUTO) {
#ifdef CONFIG_ESP_SLEEP_POWER_DOWN_FLASH
        s_config.pd_options[ESP_PD_DOMAIN_VDDSDIO] = ESP_PD_OPTION_OFF;
#else
        s_config.pd_options[ESP_PD_DOMAIN_VDDSDIO] = ESP_PD_OPTION_ON;
#endif
    }

    if (s_config.pd_options[ESP_PD_DOMAIN_VDDSDIO] != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_VDDSDIO;
    }

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
    rtc_suppress_rom_log();
}
