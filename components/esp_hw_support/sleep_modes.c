/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_rom_caps.h"
#include "esp_memory_utils.h"
#include "esp_sleep.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_timer_private.h"
#include "esp_private/rtc_clk.h"
#include "esp_private/sleep_event.h"
#include "esp_private/system_internal.h"
#include "esp_private/io_mux.h"
#include "esp_log.h"
#include "esp_newlib.h"
#include "esp_timer.h"
#include "esp_ipc_isr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "soc/chip_revision.h"
#include "driver/rtc_io.h"
#include "hal/efuse_hal.h"
#include "hal/rtc_io_hal.h"
#include "hal/clk_tree_hal.h"

#if SOC_SLEEP_SYSTIMER_STALL_WORKAROUND
#include "hal/systimer_ll.h"
#endif

#if SOC_SLEEP_TGWDT_STOP_WORKAROUND
#include "hal/mwdt_ll.h"
#include "hal/timer_ll.h"
#endif

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
#include "esp_private/pm_impl.h"
#endif

#if !SOC_PMU_SUPPORTED
#include "hal/rtc_cntl_ll.h"
#endif
#include "hal/rtc_hal.h"

#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "regi2c_ctrl.h"    //For `REGI2C_ANA_CALI_PD_WORKAROUND`, temp

#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/wdt_hal.h"
#include "hal/uart_hal.h"
#if SOC_TOUCH_SENSOR_SUPPORTED
#include "hal/touch_sensor_hal.h"
#endif

#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_private/brownout.h"
#include "esp_private/sleep_console.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_modem.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_task_wdt.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_private/mspi_timing_tuning.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#include "esp32/rom/rtc.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#include "soc/extmem_reg.h"
#include "esp_private/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#include "esp_private/mspi_timing_tuning.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/rtc.h"
#include "hal/gpio_ll.h"
#elif CONFIG_IDF_TARGET_ESP32C5
#include "esp32c5/rom/rtc.h"
#include "hal/gpio_ll.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#include "esp32h2/rom/cache.h"
#include "esp32h2/rom/rtc.h"
#include "soc/extmem_reg.h"
#include "hal/gpio_ll.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/rtc.h"
#include "hal/gpio_ll.h"
#endif

#if SOC_LP_TIMER_SUPPORTED
#include "hal/lp_timer_hal.h"
#endif

#if SOC_PMU_SUPPORTED
#include "esp_private/esp_pmu.h"
#include "esp_private/sleep_sys_periph.h"
#include "esp_private/sleep_clock.h"
#endif

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
#include "esp_private/sleep_retention.h"
#endif

// If light sleep time is less than that, don't power down flash
#define FLASH_PD_MIN_SLEEP_TIME_US  2000

// Default waiting time for the software to wait for Flash ready after waking up from sleep
#define ESP_SLEEP_WAIT_FLASH_READY_DEFAULT_DELAY_US 700

// Cycles for RTC Timer clock source (internal oscillator) calibrate
#define RTC_CLK_SRC_CAL_CYCLES      (10)
#define FAST_CLK_SRC_CAL_CYCLES     (2048)  /* ~ 127.4 us */

#ifdef CONFIG_IDF_TARGET_ESP32
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (212)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (60)
#elif CONFIG_IDF_TARGET_ESP32S2
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (147)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (28)
#elif CONFIG_IDF_TARGET_ESP32S3
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (382)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (133)
#elif CONFIG_IDF_TARGET_ESP32C3
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (105)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (37)
#elif CONFIG_IDF_TARGET_ESP32C2
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (118)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (9)
#elif CONFIG_IDF_TARGET_ESP32C6
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (318)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (56)
#elif CONFIG_IDF_TARGET_ESP32C5  // TODO: [ESP32C5] IDF-8638
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (318)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (56)
#elif CONFIG_IDF_TARGET_ESP32H2
#define DEFAULT_SLEEP_OUT_OVERHEAD_US       (118)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US    (9)
#elif CONFIG_IDF_TARGET_ESP32P4
#define DEFAULT_SLEEP_OUT_OVERHEAD_US           (324)
#define DEFAULT_HARDWARE_OUT_OVERHEAD_US        (240)
#define LDO_POWER_TAKEOVER_PREPARATION_TIME_US  (185)
#endif

// Actually costs 80us, using the fastest slow clock 150K calculation takes about 16 ticks
#define SLEEP_TIMER_ALARM_TO_SLEEP_TICKS    (16)

#define SLEEP_UART_FLUSH_DONE_TO_SLEEP_US   (450)

#if SOC_PM_SUPPORT_TOP_PD
// IDF console uses 8 bits data mode without parity, so each char occupy 8(data)+1(start)+1(stop)=10bits
#define UART_FLUSH_US_PER_CHAR              (10*1000*1000 / CONFIG_ESP_CONSOLE_UART_BAUDRATE)
#define CONCATENATE_HELPER(x, y)            (x##y)
#define CONCATENATE(x, y)                   CONCATENATE_HELPER(x, y)
#define CONSOLE_UART_DEV                    (&CONCATENATE(UART, CONFIG_ESP_CONSOLE_UART_NUM))
#endif

#define LIGHT_SLEEP_TIME_OVERHEAD_US        DEFAULT_HARDWARE_OUT_OVERHEAD_US
#ifdef CONFIG_ESP_SYSTEM_RTC_EXT_XTAL
#define DEEP_SLEEP_TIME_OVERHEAD_US         (650 + 100 * 240 / CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
#else
#define DEEP_SLEEP_TIME_OVERHEAD_US         (250 + 100 * 240 / CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
#endif

// Minimal amount of time we can sleep for
#define LIGHT_SLEEP_MIN_TIME_US     200

#define RTC_MODULE_SLEEP_PREPARE_CYCLES (6)

#define CHECK_SOURCE(source, value, mask) ((s_config.wakeup_triggers & mask) && \
                                            (source == value))

#define MAX_DSLP_HOOKS      3

static esp_deep_sleep_cb_t s_dslp_cb[MAX_DSLP_HOOKS] = {0};
#if CONFIG_ESP_PHY_ENABLED
static esp_deep_sleep_cb_t s_dslp_phy_cb[MAX_DSLP_HOOKS] = {0};
#endif

/**
 * Internal structure which holds all requested sleep parameters
 */
typedef struct {
    struct {
        esp_sleep_pd_option_t pd_option;
        int16_t     refs;
        uint16_t    reserved;   /* reserved for 4 bytes aligned */
    } domain[ESP_PD_DOMAIN_MAX];
    portMUX_TYPE lock;
    uint64_t sleep_duration;
    uint32_t wakeup_triggers : 15;
#if SOC_PM_SUPPORT_EXT1_WAKEUP
    uint32_t ext1_trigger_mode : 22;  // 22 is the maximum RTCIO number in all chips
    uint32_t ext1_rtc_gpio_mask : 22;
#endif
#if SOC_PM_SUPPORT_EXT0_WAKEUP
    uint32_t ext0_trigger_level : 1;
    uint32_t ext0_rtc_gpio_num : 5;
#endif
#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
    uint32_t gpio_wakeup_mask : SOC_GPIO_DEEP_SLEEP_WAKE_SUPPORTED_PIN_CNT;  // Only RTC_GPIO supports wakeup deepsleep
    uint32_t gpio_trigger_mode : SOC_GPIO_DEEP_SLEEP_WAKE_SUPPORTED_PIN_CNT;
#endif
    uint32_t sleep_time_adjustment;
    uint32_t ccount_ticks_record;
    uint32_t sleep_time_overhead_out;
    uint32_t rtc_clk_cal_period;
    uint32_t fast_clk_cal_period;
    uint64_t rtc_ticks_at_sleep_start;
#if SOC_DCDC_SUPPORTED
    uint64_t rtc_ticks_at_ldo_prepare;
#endif
} sleep_config_t;


#if CONFIG_ESP_SLEEP_DEBUG
static esp_sleep_context_t *s_sleep_ctx = NULL;

void esp_sleep_set_sleep_context(esp_sleep_context_t *sleep_ctx)
{
    s_sleep_ctx = sleep_ctx;
}
#endif

static uint32_t s_lightsleep_cnt = 0;

_Static_assert(22 >= SOC_RTCIO_PIN_COUNT, "Chip has more RTCIOs than 22, should increase ext1_rtc_gpio_mask field size");

static sleep_config_t s_config = {
    .domain = {
        [0 ... ESP_PD_DOMAIN_MAX - 1] = {
            .pd_option = ESP_PD_OPTION_AUTO,
            .refs = 0
        }
    },
    .lock = portMUX_INITIALIZER_UNLOCKED,
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
static RTC_FAST_ATTR bool s_adc_tsen_enabled = false;
//in this mode, 2uA is saved, but RTC memory can't use at high temperature, and RTCIO can't be used as INPUT.
static bool s_ultra_low_enabled = false;

static bool s_periph_use_8m_flag = false;

void esp_sleep_periph_use_8m(bool use_or_not)
{
    s_periph_use_8m_flag = use_or_not;
}

static uint32_t get_power_down_flags(void);
#if SOC_PM_SUPPORT_EXT0_WAKEUP
static void ext0_wakeup_prepare(void);
#endif
#if SOC_PM_SUPPORT_EXT1_WAKEUP
static void ext1_wakeup_prepare(void);
#endif
static esp_err_t timer_wakeup_prepare(int64_t sleep_duration);
#if SOC_TOUCH_SENSOR_SUPPORTED && SOC_TOUCH_SENSOR_VERSION != 1
static void touch_wakeup_prepare(void);
#endif
#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
static void gpio_deep_sleep_wakeup_prepare(void);
#endif

#if ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB && SOC_DEEP_SLEEP_SUPPORTED
#if SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY
static RTC_FAST_ATTR esp_deep_sleep_wake_stub_fn_t wake_stub_fn_handler = NULL;

static void RTC_IRAM_ATTR __attribute__((used, noinline)) esp_wake_stub_start(void)
{
    if (wake_stub_fn_handler) {
        (*wake_stub_fn_handler)();
    }
}

/* We must have a default deep sleep wake stub entry function, which must be
 * located at the start address of the RTC fast memory, and its implementation
 * must be simple enough to ensure that there is no literal data before the
 * wake stub entry, otherwise, the literal data before the wake stub entry
 * will not be CRC checked. */
static void __attribute__((section(".rtc.entry.text"))) esp_wake_stub_entry(void)
{
#define _SYM2STR(s) # s
#define SYM2STR(s)  _SYM2STR(s)

#ifdef __riscv
    __asm__ __volatile__ (
            "addi sp, sp, -16 \n"
            "sw   ra, 0(sp)   \n"
            "jal  ra, " SYM2STR(esp_wake_stub_start) "\n"
            "lw   ra, 0(sp)  \n"
            "addi sp, sp, 16 \n"
            );
#else
    // call4 has a larger effective addressing range (-524284 to 524288 bytes),
    // which is sufficient for instruction addressing in RTC fast memory.
    __asm__ __volatile__ ("call4 " SYM2STR(esp_wake_stub_start) "\n");
#endif

}

void RTC_IRAM_ATTR esp_set_deep_sleep_wake_stub_default_entry(void)
{
    extern char _rtc_text_start[];
#if CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM
    extern char _rtc_noinit_end[];
    size_t rtc_fast_length = (size_t)_rtc_noinit_end - (size_t)_rtc_text_start;
#else
    extern char _rtc_force_fast_end[];
    size_t rtc_fast_length = (size_t)_rtc_force_fast_end - (size_t)_rtc_text_start;
#endif
    esp_rom_set_rtc_wake_addr((esp_rom_wake_func_t)esp_wake_stub_entry, rtc_fast_length);
}
#endif // SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY

/* Wake from deep sleep stub
   See esp_deepsleep.h esp_wake_deep_sleep() comments for details.
*/
esp_deep_sleep_wake_stub_fn_t esp_get_deep_sleep_wake_stub(void)
{
#if SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY
    esp_deep_sleep_wake_stub_fn_t stub_ptr = wake_stub_fn_handler;
#else
    esp_deep_sleep_wake_stub_fn_t stub_ptr = (esp_deep_sleep_wake_stub_fn_t) REG_READ(RTC_ENTRY_ADDR_REG);
#endif
    if (!esp_ptr_executable(stub_ptr)) {
        return NULL;
    }
    return stub_ptr;
}

#if CONFIG_IDF_TARGET_ESP32
/* APP core of esp32 can't access to RTC FAST MEMORY, do not define it with RTC_IRAM_ATTR */
void
#else
void RTC_IRAM_ATTR
#endif
esp_set_deep_sleep_wake_stub(esp_deep_sleep_wake_stub_fn_t new_stub)
{
#if SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY
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
#if CONFIG_ESP_SLEEP_WAIT_FLASH_READY_EXTRA_DELAY > 0
    // ROM code has not started yet, so we need to set delay factor
    // used by esp_rom_delay_us first.
    ets_update_cpu_frequency_rom(ets_get_detected_xtal_freq() / 1000000);
    // Time from VDD_SDIO power up to first flash read in ROM code is 700 us,
    // for some flash chips is not sufficient, this delay is configured in menuconfig,
    // it can be used to give the flash chip some extra time to become ready.
    // For later chips, we have EFUSE_FLASH_TPUW field to configure it and do
    // this delay in the ROM.
    esp_rom_delay_us(CONFIG_ESP_SLEEP_WAIT_FLASH_READY_EXTRA_DELAY);
#endif
#elif CONFIG_IDF_TARGET_ESP32S2
    REG_SET_BIT(EXTMEM_CACHE_DBG_INT_ENA_REG, EXTMEM_CACHE_DBG_EN);
#endif
}

void __attribute__((weak, alias("esp_default_wake_deep_sleep"))) esp_wake_deep_sleep(void);
#endif // SOC_RTC_FAST_MEM_SUPPORTED

void esp_deep_sleep(uint64_t time_in_us)
{
    esp_sleep_enable_timer_wakeup(time_in_us);
    esp_deep_sleep_start();
}

esp_err_t esp_deep_sleep_try(uint64_t time_in_us)
{
    esp_sleep_enable_timer_wakeup(time_in_us);
    return esp_deep_sleep_try_to_start();
}

static esp_err_t s_sleep_hook_register(esp_deep_sleep_cb_t new_cb, esp_deep_sleep_cb_t s_cb_array[MAX_DSLP_HOOKS])
{
    portENTER_CRITICAL(&spinlock_rtc_deep_sleep);
    for (int n = 0; n < MAX_DSLP_HOOKS; n++) {
        if (s_cb_array[n]==NULL || s_cb_array[n]==new_cb) {
            s_cb_array[n]=new_cb;
            portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
            return ESP_OK;
        }
    }
    portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
    ESP_LOGE(TAG, "Registered deepsleep callbacks exceeds MAX_DSLP_HOOKS");
    return ESP_ERR_NO_MEM;
}

static void s_sleep_hook_deregister(esp_deep_sleep_cb_t old_cb, esp_deep_sleep_cb_t s_cb_array[MAX_DSLP_HOOKS])
{
    portENTER_CRITICAL(&spinlock_rtc_deep_sleep);
    for (int n = 0; n < MAX_DSLP_HOOKS; n++) {
        if(s_cb_array[n] == old_cb) {
            s_cb_array[n] = NULL;
        }
    }
    portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
}

esp_err_t esp_deep_sleep_register_hook(esp_deep_sleep_cb_t new_dslp_cb)
{
    return s_sleep_hook_register(new_dslp_cb, s_dslp_cb);
}

void esp_deep_sleep_deregister_hook(esp_deep_sleep_cb_t old_dslp_cb)
{
    s_sleep_hook_deregister(old_dslp_cb, s_dslp_cb);
}

#if CONFIG_ESP_PHY_ENABLED
esp_err_t esp_deep_sleep_register_phy_hook(esp_deep_sleep_cb_t new_dslp_cb)
{
    return s_sleep_hook_register(new_dslp_cb, s_dslp_phy_cb);
}

void esp_deep_sleep_deregister_phy_hook(esp_deep_sleep_cb_t old_dslp_cb)
{
    s_sleep_hook_deregister(old_dslp_cb, s_dslp_phy_cb);
}

static void s_do_deep_sleep_phy_callback(void)
{
    for (int n = 0; n < MAX_DSLP_HOOKS; n++) {
        if (s_dslp_phy_cb[n] != NULL) {
            s_dslp_phy_cb[n]();
        }
    }
}
#endif

static int s_cache_suspend_cnt = 0;

// Must be called from critical sections.
static void IRAM_ATTR suspend_cache(void) {
    s_cache_suspend_cnt++;
    if (s_cache_suspend_cnt == 1) {
        cache_hal_suspend(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
    }
}

// Must be called from critical sections.
static void IRAM_ATTR resume_cache(void) {
    s_cache_suspend_cnt--;
    assert(s_cache_suspend_cnt >= 0 && DRAM_STR("cache resume doesn't match suspend ops"));
    if (s_cache_suspend_cnt == 0) {
        cache_hal_resume(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
    }
}

#if SOC_SLEEP_TGWDT_STOP_WORKAROUND
static uint32_t s_stopped_tgwdt_bmap = 0;
#endif

// Must be called from critical sections.
static void IRAM_ATTR suspend_timers(uint32_t pd_flags) {
    if (!(pd_flags & RTC_SLEEP_PD_XTAL)) {
#if SOC_SLEEP_TGWDT_STOP_WORKAROUND
        /* If timegroup implemented task watchdog or interrupt watchdog is running, we have to stop it. */
        for (uint32_t tg_num = 0; tg_num < SOC_TIMER_GROUPS; ++tg_num) {
            if (mwdt_ll_check_if_enabled(TIMER_LL_GET_HW(tg_num))) {
                mwdt_ll_write_protect_disable(TIMER_LL_GET_HW(tg_num));
                mwdt_ll_disable(TIMER_LL_GET_HW(tg_num));
                mwdt_ll_write_protect_enable(TIMER_LL_GET_HW(tg_num));
                s_stopped_tgwdt_bmap |= BIT(tg_num);
            }
        }
#endif
#if SOC_SLEEP_SYSTIMER_STALL_WORKAROUND
        for (uint32_t counter_id = 0; counter_id < SOC_SYSTIMER_COUNTER_NUM; ++counter_id) {
            systimer_ll_enable_counter(&SYSTIMER, counter_id, false);
        }
#endif
    }
}

// Must be called from critical sections.
static void IRAM_ATTR resume_timers(uint32_t pd_flags) {
    if (!(pd_flags & RTC_SLEEP_PD_XTAL)) {
#if SOC_SLEEP_SYSTIMER_STALL_WORKAROUND
        for (uint32_t counter_id = 0; counter_id < SOC_SYSTIMER_COUNTER_NUM; ++counter_id) {
            systimer_ll_enable_counter(&SYSTIMER, counter_id, true);
        }
#endif
#if SOC_SLEEP_TGWDT_STOP_WORKAROUND
        for (uint32_t tg_num = 0; tg_num < SOC_TIMER_GROUPS; ++tg_num) {
            if (s_stopped_tgwdt_bmap & BIT(tg_num)) {
                mwdt_ll_write_protect_disable(TIMER_LL_GET_HW(tg_num));
                mwdt_ll_enable(TIMER_LL_GET_HW(tg_num));
                mwdt_ll_write_protect_enable(TIMER_LL_GET_HW(tg_num));
            }
        }
#endif
    }
}

// [refactor-todo] provide target logic for body of uart functions below
static void IRAM_ATTR flush_uarts(void)
{
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
#ifdef CONFIG_IDF_TARGET_ESP32
        esp_rom_output_tx_wait_idle(i);
#else
        if (uart_ll_is_enabled(i)) {
            esp_rom_output_tx_wait_idle(i);
        }
#endif
    }
}

static uint32_t s_suspended_uarts_bmap = 0;

/**
 * Suspend enabled uarts and return suspended uarts bit map.
 * Must be called from critical sections.
 */
FORCE_INLINE_ATTR void suspend_uarts(void)
{
    s_suspended_uarts_bmap = 0;
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
#ifndef CONFIG_IDF_TARGET_ESP32
        if (!uart_ll_is_enabled(i)) {
            continue;
        }
#endif
        uart_ll_force_xoff(i);
        s_suspended_uarts_bmap |= BIT(i);
#if SOC_UART_SUPPORT_FSM_TX_WAIT_SEND
        uint32_t uart_fsm = 0;
        do {
            uart_fsm = uart_ll_get_tx_fsm_status(i);
        } while (!(uart_fsm == UART_LL_FSM_IDLE || uart_fsm == UART_LL_FSM_TX_WAIT_SEND));
#else
        while (uart_ll_get_tx_fsm_status(i) != 0) {}
#endif
    }
}

// Must be called from critical sections
FORCE_INLINE_ATTR void resume_uarts(void)
{
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (s_suspended_uarts_bmap & 0x1) {
            uart_ll_force_xon(i);
        }
        s_suspended_uarts_bmap >>= 1;
    }
}

/*
  UART prepare strategy in sleep:
    Deepsleep : flush the fifo before enter sleep to avoid data loss

    Lightsleep:
      Chips not support PD_TOP: Suspend uart before cpu freq switch

      Chips support PD_TOP:
        For sleep which will not power down the TOP domain (uart belongs it), we can just suspend the UART.

        For sleep which will power down the TOP domain, we need to consider whether the uart flushing will
        block the sleep process and cause the rtos target tick to be missed upon waking up. It's need to
        estimate the flush time based on the number of bytes in the uart FIFO,  if the predicted flush
        completion time has exceeded the wakeup time, we should abandon the flush, skip the sleep and
        return ESP_ERR_SLEEP_REJECT.
 */
FORCE_INLINE_ATTR bool light_sleep_uart_prepare(uint32_t pd_flags, int64_t sleep_duration)
{
    bool should_skip_sleep = false;
#if !SOC_PM_SUPPORT_TOP_PD || !CONFIG_ESP_CONSOLE_UART
    suspend_uarts();
#else
#ifdef CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION
#define FORCE_FLUSH_CONSOLE_UART 1
#else
#define FORCE_FLUSH_CONSOLE_UART 0
#endif
    if (FORCE_FLUSH_CONSOLE_UART || (pd_flags & PMU_SLEEP_PD_TOP)) {
        if ((s_config.wakeup_triggers & RTC_TIMER_TRIG_EN) &&
            // +1 is for cover the last character flush time
            (sleep_duration < (int64_t)((UART_LL_FIFO_DEF_LEN - uart_ll_get_txfifo_len(CONSOLE_UART_DEV) + 1) * UART_FLUSH_US_PER_CHAR) + SLEEP_UART_FLUSH_DONE_TO_SLEEP_US)) {
            should_skip_sleep = true;
        } else {
            /* Only flush the uart_num configured to console, the transmission integrity of
               other uarts is guaranteed by the UART driver */
            if (CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM != -1) {
                esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
            }
        }
    } else {
        suspend_uarts();
    }
#endif
    return should_skip_sleep;
}

/**
 * These save-restore workaround should be moved to lower layer
 */
FORCE_INLINE_ATTR void misc_modules_sleep_prepare(bool deep_sleep)
{
    if (deep_sleep){
        for (int n = 0; n < MAX_DSLP_HOOKS; n++) {
            if (s_dslp_cb[n] != NULL) {
                s_dslp_cb[n]();
            }
        }
    } else {
#if SOC_USB_SERIAL_JTAG_SUPPORTED && !SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP
        // Only avoid USJ pad leakage here, USB OTG pad leakage is prevented through USB Host driver.
        sleep_console_usj_pad_backup_and_disable();
#endif
#if CONFIG_MAC_BB_PD
        mac_bb_power_down_cb_execute();
#endif
#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
        gpio_sleep_mode_config_apply();
#endif
#if CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP && SOC_PM_CPU_RETENTION_BY_RTCCNTL
        sleep_enable_cpu_retention();
#endif
#if REGI2C_ANA_CALI_PD_WORKAROUND
        regi2c_analog_cali_reg_read();
#endif
    }

#if !CONFIG_IDF_TARGET_ESP32P4
    // TODO: IDF-7370
    if (!(deep_sleep && s_adc_tsen_enabled)){
        sar_periph_ctrl_power_disable();
    }
#endif
}

/**
 * These save-restore workaround should be moved to lower layer
 */
FORCE_INLINE_ATTR void misc_modules_wake_prepare(uint32_t pd_flags)
{
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    if (pd_flags & PMU_SLEEP_PD_TOP) {
        // There is no driver to manage the flashboot watchdog, and it is definitely be in off state when
        // the system is running, after waking up from pd_top sleep, shut it down by software here.
        wdt_hal_context_t mwdt_ctx = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
        wdt_hal_write_protect_disable(&mwdt_ctx);
        wdt_hal_set_flashboot_en(&mwdt_ctx, false);
        wdt_hal_write_protect_enable(&mwdt_ctx);
    }
#endif

#if SOC_USB_SERIAL_JTAG_SUPPORTED && !SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP
    sleep_console_usj_pad_restore();
#endif
    sar_periph_ctrl_power_enable();
#if CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP && SOC_PM_CPU_RETENTION_BY_RTCCNTL
    sleep_disable_cpu_retention();
#endif
#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
    gpio_sleep_mode_config_unapply();
#endif
#if CONFIG_MAC_BB_PD
    mac_bb_power_up_cb_execute();
#endif
#if REGI2C_ANA_CALI_PD_WORKAROUND
    regi2c_analog_cali_reg_write();
#endif
}

static IRAM_ATTR void sleep_low_power_clock_calibration(bool is_dslp)
{
    // Calibrate rtc slow clock
#ifdef CONFIG_ESP_SYSTEM_RTC_EXT_XTAL
    if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        uint64_t time_per_us = 1000000ULL;
        s_config.rtc_clk_cal_period = (time_per_us << RTC_CLK_CAL_FRACT) / rtc_clk_slow_freq_get_hz();
    } else {
        // If the external 32 kHz XTAL does not exist, use the internal 150 kHz RC oscillator
        // as the RTC slow clock source.
        s_config.rtc_clk_cal_period = rtc_clk_cal(RTC_CAL_RTC_MUX, RTC_CLK_SRC_CAL_CYCLES);
        esp_clk_slowclk_cal_set(s_config.rtc_clk_cal_period);
    }
#elif CONFIG_RTC_CLK_SRC_INT_RC && CONFIG_IDF_TARGET_ESP32S2
    s_config.rtc_clk_cal_period = rtc_clk_cal_cycling(RTC_CAL_RTC_MUX, RTC_CLK_SRC_CAL_CYCLES);
    esp_clk_slowclk_cal_set(s_config.rtc_clk_cal_period);
#else
#if CONFIG_PM_ENABLE
    if ((s_lightsleep_cnt % CONFIG_PM_LIGHTSLEEP_RTC_OSC_CAL_INTERVAL == 0) || is_dslp)
#endif
    {
        s_config.rtc_clk_cal_period = rtc_clk_cal(RTC_CAL_RTC_MUX, RTC_CLK_SRC_CAL_CYCLES);
        esp_clk_slowclk_cal_set(s_config.rtc_clk_cal_period);
    }
#endif

    // Calibrate rtc fast clock, only PMU supported chips sleep process is needed.
#if SOC_PMU_SUPPORTED
#if CONFIG_PM_ENABLE
    if ((s_lightsleep_cnt % CONFIG_PM_LIGHTSLEEP_RTC_OSC_CAL_INTERVAL == 0) || is_dslp)
#endif
    {
        s_config.fast_clk_cal_period = rtc_clk_cal(RTC_CAL_RC_FAST, FAST_CLK_SRC_CAL_CYCLES);
    }
#endif
}


inline static uint32_t call_rtc_sleep_start(uint32_t reject_triggers, uint32_t lslp_mem_inf_fpu, bool dslp);

static esp_err_t IRAM_ATTR esp_sleep_start(uint32_t pd_flags, esp_sleep_mode_t mode, bool allow_sleep_rejection)
{
    // Stop UART output so that output is not lost due to APB frequency change.
    // For light sleep, suspend UART output — it will resume after wakeup.
    // For deep sleep, wait for the contents of UART FIFO to be sent.
    bool deep_sleep = (mode == ESP_SLEEP_MODE_DEEP_SLEEP);
    bool should_skip_sleep = false;

    int64_t sleep_duration = (int64_t) s_config.sleep_duration - (int64_t) s_config.sleep_time_adjustment;

#if SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256
    //Keep the RTC8M_CLK on if RTC clock is rc_fast_d256.
    bool rtc_using_8md256 = (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256);
#else
    bool rtc_using_8md256 = false;
#endif
    //Keep the RTC8M_CLK on if the ledc low-speed channel is clocked by RTC8M_CLK in lightsleep mode
    bool periph_using_8m = !deep_sleep && s_periph_use_8m_flag;

    //Override user-configured power modes.
    if (rtc_using_8md256 || periph_using_8m) {
        pd_flags &= ~RTC_SLEEP_PD_INT_8M;
    }

    // Sleep UART prepare
    if (deep_sleep) {
        flush_uarts();
    } else {
        should_skip_sleep = light_sleep_uart_prepare(pd_flags, sleep_duration);
    }

#if CONFIG_ESP_PHY_ENABLED
    // Do deep-sleep PHY related callback, which need to be executed when the PLL clock is exists.
    // For light-sleep, PHY state is managed by the upper layer of the wifi/bt protocol stack.
    if (deep_sleep) {
        s_do_deep_sleep_phy_callback();
    }
#endif

    // Will switch to XTAL turn down MSPI speed
    mspi_timing_change_speed_mode_cache_safe(true);

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
    if (!deep_sleep && (pd_flags & PMU_SLEEP_PD_TOP)) {
        sleep_retention_do_system_retention(true);
    }
#endif

    // Save current frequency and switch to XTAL
    rtc_cpu_freq_config_t cpu_freq_config;
    rtc_clk_cpu_freq_get_config(&cpu_freq_config);
    rtc_clk_cpu_freq_set_xtal();

#if SOC_PM_SUPPORT_EXT0_WAKEUP
    // Configure pins for external wakeup
    if (s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
        ext0_wakeup_prepare();
    }
    // for !(s_config.wakeup_triggers & RTC_EXT0_TRIG_EN), ext0 wakeup will be turned off in hardware in the real call to sleep
#endif
#if SOC_PM_SUPPORT_EXT1_WAKEUP
    if (s_config.wakeup_triggers & RTC_EXT1_TRIG_EN) {
        ext1_wakeup_prepare();
    }
    // for !(s_config.wakeup_triggers & RTC_EXT1_TRIG_EN), ext1 wakeup will be turned off in hardware in the real call to sleep
#endif

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
    if (deep_sleep && (s_config.wakeup_triggers & RTC_GPIO_TRIG_EN)) {
        gpio_deep_sleep_wakeup_prepare();
    }
#endif

#if CONFIG_ULP_COPROC_ENABLED
    // Enable ULP wakeup
#if CONFIG_ULP_COPROC_TYPE_FSM
    if (s_config.wakeup_triggers & RTC_ULP_TRIG_EN) {
#elif CONFIG_ULP_COPROC_TYPE_RISCV
    if (s_config.wakeup_triggers & (RTC_COCPU_TRIG_EN | RTC_COCPU_TRAP_TRIG_EN)) {
#elif CONFIG_ULP_COPROC_TYPE_LP_CORE
    if (s_config.wakeup_triggers & RTC_LP_CORE_TRIG_EN) {
#endif
#ifdef CONFIG_IDF_TARGET_ESP32
        rtc_hal_ulp_wakeup_enable();
#elif CONFIG_ULP_COPROC_TYPE_LP_CORE
        pmu_ll_hp_clear_sw_intr_status(&PMU);
#else
        rtc_hal_ulp_int_clear();
#endif
    }
#endif // CONFIG_ULP_COPROC_ENABLED

    misc_modules_sleep_prepare(deep_sleep);

#if SOC_TOUCH_SENSOR_VERSION >= 2
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
#if SOC_TOUCH_SENSOR_VERSION == 3
        bool keep_rtc_power_on = touch_ll_is_fsm_repeated_timer_enabled();
#else
        bool keep_rtc_power_on = touch_ll_get_fsm_state();
#endif
        if (keep_rtc_power_on) { // Check if the touch sensor is working properly.
            pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
        }
    }
#elif CONFIG_IDF_TARGET_ESP32P4
    /* Due to esp32p4 eco0 hardware bug, if LP peripheral power domain is powerdowned in sleep, there will be a possibility of
       triggering the EFUSE_CRC reset, so disable the power-down of this power domain on lightsleep for ECO0 version. */
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 1)) {
        pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
    }
#endif

    uint32_t reject_triggers = allow_sleep_rejection ? (s_config.wakeup_triggers & RTC_SLEEP_REJECT_MASK) : 0;

    if (!deep_sleep) {
        /* Enable sleep reject for faster return from this function,
         * in case the wakeup is already triggered.
         */
        reject_triggers |= sleep_modem_reject_triggers();
    }

    //Append some flags in addition to power domains
    uint32_t sleep_flags = pd_flags;
    if (s_adc_tsen_enabled) {
        sleep_flags |= RTC_SLEEP_USE_ADC_TESEN_MONITOR;
    }
    if (!s_ultra_low_enabled) {
        sleep_flags |= RTC_SLEEP_NO_ULTRA_LOW;
    }
    if (periph_using_8m) {
        sleep_flags |= RTC_SLEEP_DIG_USE_8M;
    }

#if CONFIG_ESP_SLEEP_DEBUG
    if (s_sleep_ctx != NULL) {
        s_sleep_ctx->sleep_flags = sleep_flags;
    }
#endif

    // Enter sleep
    esp_err_t result;
#if SOC_PMU_SUPPORTED

#if SOC_DCDC_SUPPORTED
#if CONFIG_ESP_SLEEP_KEEP_DCDC_ALWAYS_ON
    if (!deep_sleep) {
        // Keep DCDC always on during light sleep, no need to adjust LDO voltage.
    } else
#endif
    {
        s_config.rtc_ticks_at_ldo_prepare = rtc_time_get();
        pmu_sleep_increase_ldo_volt();
    }
#endif

    pmu_sleep_config_t config;
    pmu_sleep_init(pmu_sleep_config_default(&config, sleep_flags, s_config.sleep_time_adjustment,
            s_config.rtc_clk_cal_period, s_config.fast_clk_cal_period,
            deep_sleep), deep_sleep);
#else
    rtc_sleep_config_t config;
    rtc_sleep_get_default_config(sleep_flags, &config);
    rtc_sleep_init(config);

    // Set state machine time for light sleep
    if (!deep_sleep) {
        rtc_sleep_low_init(s_config.rtc_clk_cal_period);
    }
#endif

    // Configure timer wakeup
    if (!should_skip_sleep && (s_config.wakeup_triggers & RTC_TIMER_TRIG_EN)) {
        if (timer_wakeup_prepare(sleep_duration) != ESP_OK) {
            should_skip_sleep = allow_sleep_rejection ? true : false;
        }
    }


    if (should_skip_sleep) {
        result = ESP_ERR_SLEEP_REJECT;
#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE && SOC_PM_CPU_RETENTION_BY_SW
        esp_sleep_cpu_skip_retention();
#endif
    } else {
#if CONFIG_ESP_SLEEP_DEBUG
        if (s_sleep_ctx != NULL) {
            s_sleep_ctx->wakeup_triggers = s_config.wakeup_triggers;
        }
#endif
        if (deep_sleep) {
#if SOC_GPIO_SUPPORT_HOLD_IO_IN_DSLP && !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
            esp_sleep_isolate_digital_gpio();
#endif

#if ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB && SOC_DEEP_SLEEP_SUPPORTED
#if SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY
            esp_set_deep_sleep_wake_stub_default_entry();
#elif !CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP && SOC_RTC_FAST_MEM_SUPPORTED
            /* If not possible stack is in RTC FAST memory, use the ROM function to calculate the CRC and save ~140 bytes IRAM */
            set_rtc_memory_crc();
#endif // SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY
#endif

            // Enter Deep Sleep
#if!ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB || SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY || !CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
#if SOC_PMU_SUPPORTED
            result = call_rtc_sleep_start(reject_triggers, config.power.hp_sys.dig_power.mem_dslp, deep_sleep);
#else
            result = call_rtc_sleep_start(reject_triggers, config.lslp_mem_inf_fpu, deep_sleep);
#endif
#else
            /* Otherwise, need to call the dedicated soc function for this */
            result = rtc_deep_sleep_start(s_config.wakeup_triggers, reject_triggers);
#endif
        } else {
            suspend_timers(pd_flags);
            /* Cache Suspend 1: will wait cache idle in cache suspend */
            suspend_cache();
            /* On esp32c6, only the lp_aon pad hold function can only hold the GPIO state in the active mode.
               In order to avoid the leakage of the SPI cs pin, hold it here */
#if (CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND)
#if !CONFIG_IDF_TARGET_ESP32H2 // ESP32H2 TODO IDF-7359
            if(!(pd_flags & RTC_SLEEP_PD_VDDSDIO)) {
                /* Cache suspend also means SPI bus IDLE, then we can hold SPI CS pin safely */
                gpio_ll_hold_en(&GPIO, SPI_CS0_GPIO_NUM);
            }
#endif
#endif

#if SOC_DCDC_SUPPORTED
#if CONFIG_ESP_SLEEP_KEEP_DCDC_ALWAYS_ON
            if (!deep_sleep) {
                // Keep DCDC always on during light sleep, no need to adjust LDO voltage.
            } else
#endif
            {
                uint64_t ldo_increased_us = rtc_time_slowclk_to_us(rtc_time_get() - s_config.rtc_ticks_at_ldo_prepare, s_config.rtc_clk_cal_period);
                if (ldo_increased_us < LDO_POWER_TAKEOVER_PREPARATION_TIME_US) {
                    esp_rom_delay_us(LDO_POWER_TAKEOVER_PREPARATION_TIME_US - ldo_increased_us);
                }
                pmu_sleep_shutdown_dcdc();
            }
#endif

#if SOC_PMU_SUPPORTED
#if SOC_PM_CPU_RETENTION_BY_SW && ESP_SLEEP_POWER_DOWN_CPU
            esp_sleep_execute_event_callbacks(SLEEP_EVENT_HW_GOTO_SLEEP, (void *)0);
            if (pd_flags & (PMU_SLEEP_PD_CPU | PMU_SLEEP_PD_TOP)) {
                result = esp_sleep_cpu_retention(pmu_sleep_start, s_config.wakeup_triggers, reject_triggers, config.power.hp_sys.dig_power.mem_dslp, deep_sleep);
            } else
#endif
            {
#if !CONFIG_FREERTOS_UNICORE && ESP_SLEEP_POWER_DOWN_CPU && SOC_PM_CPU_RETENTION_BY_SW
                // Skip smp retention if CPU power domain power-down is not allowed
                esp_sleep_cpu_skip_retention();
#endif
                result = call_rtc_sleep_start(reject_triggers, config.power.hp_sys.dig_power.mem_dslp, deep_sleep);
            }
            esp_sleep_execute_event_callbacks(SLEEP_EVENT_HW_EXIT_SLEEP, (void *)0);
#else
            result = call_rtc_sleep_start(reject_triggers, config.lslp_mem_inf_fpu, deep_sleep);
#endif

            /* Unhold the SPI CS pin */
#if (CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND)
#if !CONFIG_IDF_TARGET_ESP32H2 // ESP32H2 TODO IDF-7359
            if(!(pd_flags & RTC_SLEEP_PD_VDDSDIO)) {
                gpio_ll_hold_dis(&GPIO, SPI_CS0_GPIO_NUM);
            }
#endif
#endif
            /* Cache Resume 1: Resume cache for continue running*/
            resume_cache();
            resume_timers(pd_flags);
        }
    }
#if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION
    if (pd_flags & RTC_SLEEP_PD_VDDSDIO) {
        /* Cache Suspend 2: If previous sleep powerdowned the flash, suspend cache here so that the
           access to flash before flash ready can be explicitly exposed. */
        suspend_cache();
    }
#endif
    // Restore CPU frequency
#if SOC_PM_SUPPORT_PMU_MODEM_STATE
    if (pmu_sleep_pll_already_enabled()) {
        rtc_clk_cpu_freq_to_pll_and_pll_lock_release(esp_pm_impl_get_cpu_freq(PM_MODE_CPU_MAX));
    } else
#endif
    {
        rtc_clk_cpu_freq_set_config(&cpu_freq_config);
    }

    esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_CLK_READY, (void *)0);

    if (!deep_sleep) {
        s_config.ccount_ticks_record = esp_cpu_get_cycle_count();
#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
        if (pd_flags & PMU_SLEEP_PD_TOP) {
            sleep_retention_do_system_retention(false);
        }
#endif
        misc_modules_wake_prepare(pd_flags);
    }

#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    if (cpu_freq_config.source == SOC_CPU_CLK_SRC_PLL) {
        // Turn up MSPI speed if switch to PLL
        mspi_timing_change_speed_mode_cache_safe(false);
    }
#endif

    // re-enable UART output
    resume_uarts();
    return result ? ESP_ERR_SLEEP_REJECT : ESP_OK;
}

inline static uint32_t IRAM_ATTR call_rtc_sleep_start(uint32_t reject_triggers, uint32_t lslp_mem_inf_fpu, bool dslp)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    return rtc_sleep_start(s_config.wakeup_triggers, reject_triggers);
#elif SOC_PMU_SUPPORTED
    return pmu_sleep_start(s_config.wakeup_triggers, reject_triggers, lslp_mem_inf_fpu, dslp);
#else
    return rtc_sleep_start(s_config.wakeup_triggers, reject_triggers, lslp_mem_inf_fpu);
#endif
}

static esp_err_t IRAM_ATTR deep_sleep_start(bool allow_sleep_rejection)
{
#if CONFIG_IDF_TARGET_ESP32S2
    /* Due to hardware limitations, on S2 the brownout detector sometimes trigger during deep sleep
       to circumvent this we disable the brownout detector before sleeping  */
    esp_brownout_disable();
#endif //CONFIG_IDF_TARGET_ESP32S2

    esp_sync_timekeeping_timers();

    /* Disable interrupts and stall another core in case another task writes
     * to RTC memory while we calculate RTC memory CRC.
     */
    portENTER_CRITICAL(&spinlock_rtc_deep_sleep);
    esp_ipc_isr_stall_other_cpu();
    esp_ipc_isr_stall_pause();

    // record current RTC time
    s_config.rtc_ticks_at_sleep_start = rtc_time_get();

#if ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB
    // Configure wake stub
    if (esp_get_deep_sleep_wake_stub() == NULL) {
        esp_set_deep_sleep_wake_stub(esp_wake_deep_sleep);
    }
#endif // ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

    // Re-calibrate the RTC clock
    sleep_low_power_clock_calibration(true);

    // Correct the sleep time
    s_config.sleep_time_adjustment = DEEP_SLEEP_TIME_OVERHEAD_US;

#if SOC_PMU_SUPPORTED
    uint32_t force_pd_flags = PMU_SLEEP_PD_TOP | PMU_SLEEP_PD_VDDSDIO | PMU_SLEEP_PD_MODEM | PMU_SLEEP_PD_HP_PERIPH \
                            | PMU_SLEEP_PD_CPU | PMU_SLEEP_PD_MEM | PMU_SLEEP_PD_XTAL;
#if SOC_PM_SUPPORT_HP_AON_PD
    force_pd_flags |= PMU_SLEEP_PD_HP_AON;
#endif
#if SOC_PM_SUPPORT_CNNT_PD
    force_pd_flags |= PMU_SLEEP_PD_CNNT;
#endif
#else
    uint32_t force_pd_flags = RTC_SLEEP_PD_DIG | RTC_SLEEP_PD_VDDSDIO | RTC_SLEEP_PD_INT_8M | RTC_SLEEP_PD_XTAL;
#endif
    /**
     * If all wireless modules share one power domain, we name this power domain "modem".
     * If wireless modules have their own power domain, we give these power domains separate
     * names.
     */
#if SOC_PM_SUPPORT_MODEM_PD
    force_pd_flags |= RTC_SLEEP_PD_MODEM;
#endif

#if SOC_PM_SUPPORT_WIFI_PD
    force_pd_flags |= RTC_SLEEP_PD_WIFI;
#endif

#if SOC_PM_SUPPORT_BT_PD
    force_pd_flags |= RTC_SLEEP_PD_BT;
#endif

    // Enter sleep
    esp_err_t err = ESP_OK;
    if (esp_sleep_start(force_pd_flags | pd_flags, ESP_SLEEP_MODE_DEEP_SLEEP, allow_sleep_rejection) == ESP_ERR_SLEEP_REJECT) {
        err = ESP_ERR_SLEEP_REJECT;
#if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION
        /* Cache Resume 2: if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION is enabled, cache has been suspended in esp_sleep_start */
        resume_cache();
#endif
        ESP_EARLY_LOGE(TAG, "Deep sleep request is rejected");
    } else {
        // Because RTC is in a slower clock domain than the CPU, it
        // can take several CPU cycles for the sleep mode to start.
        while (1) {
            ;
        }
    }
    // Never returns here, except that the sleep is rejected.
    esp_ipc_isr_stall_resume();
    esp_ipc_isr_release_other_cpu();
    portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
    return err;
}

void IRAM_ATTR esp_deep_sleep_start(void)
{
    bool allow_sleep_rejection = true;
    deep_sleep_start(!allow_sleep_rejection);
    // Never returns here
    abort();
}

esp_err_t IRAM_ATTR esp_deep_sleep_try_to_start(void)
{
    bool allow_sleep_rejection = true;
    return deep_sleep_start(allow_sleep_rejection);
}

/**
 * Helper function which handles entry to and exit from light sleep
 * Placed into IRAM as flash may need some time to be powered on.
 */
static esp_err_t esp_light_sleep_inner(uint32_t pd_flags,
                                       uint32_t flash_enable_time_us) IRAM_ATTR __attribute__((noinline));

static esp_err_t esp_light_sleep_inner(uint32_t pd_flags,
                                       uint32_t flash_enable_time_us)
{
#if SOC_CONFIGURABLE_VDDSDIO_SUPPORTED
    rtc_vddsdio_config_t vddsdio_config = rtc_vddsdio_get_config();
#endif

    // Enter sleep
    esp_err_t reject = esp_sleep_start(pd_flags, ESP_SLEEP_MODE_LIGHT_SLEEP, true);

#if SOC_CONFIGURABLE_VDDSDIO_SUPPORTED
    // If VDDSDIO regulator was controlled by RTC registers before sleep,
    // restore the configuration.
    if (vddsdio_config.force) {
        rtc_vddsdio_set_config(vddsdio_config);
    }
#endif

    // If SPI flash was powered down, wait for it to become ready
    if (pd_flags & RTC_SLEEP_PD_VDDSDIO) {
#if SOC_PM_SUPPORT_TOP_PD
        if (pd_flags & PMU_SLEEP_PD_TOP) {
            uint32_t flash_ready_hw_waited_time_us = pmu_sleep_get_wakup_retention_cost();
            uint32_t flash_ready_sw_waited_time_us = (esp_cpu_get_cycle_count() - s_config.ccount_ticks_record) / (esp_clk_cpu_freq() / MHZ);
            uint32_t flash_ready_waited_time_us = flash_ready_hw_waited_time_us + flash_ready_sw_waited_time_us;
            if (flash_enable_time_us > flash_ready_waited_time_us){
                flash_enable_time_us -= flash_ready_waited_time_us;
            } else {
                flash_enable_time_us = 0;
            }
        }
#endif
        // Wait for the flash chip to start up
        esp_rom_delay_us(flash_enable_time_us);
    }

#if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION
    if (pd_flags & RTC_SLEEP_PD_VDDSDIO) {
        /* Cache Resume 2: flash is ready now, we can resume the cache and access flash safely after */
        resume_cache();
    }
#endif

    return reject;
}

/**
 * vddsdio is used for power supply of spi flash
 *
 * pd flash via menuconfig  |  pd flash via `esp_sleep_pd_config`  |  result
 * ---------------------------------------------------------------------------------------------------
 * 0                        |  0                                   |  no pd flash
 * x                        |  1                                   |  pd flash with relaxed conditions(force_pd)
 * 1                        |  0                                   |  pd flash with strict  conditions(safe_pd)
 */
FORCE_INLINE_ATTR bool can_power_down_vddsdio(uint32_t pd_flags, const uint32_t vddsdio_pd_sleep_duration)
{
    bool force_pd = !(s_config.wakeup_triggers & RTC_TIMER_TRIG_EN) || (s_config.sleep_duration > vddsdio_pd_sleep_duration);
    bool safe_pd  = (s_config.wakeup_triggers == RTC_TIMER_TRIG_EN) && (s_config.sleep_duration > vddsdio_pd_sleep_duration);
    return (pd_flags & RTC_SLEEP_PD_VDDSDIO) ? force_pd : safe_pd;
}

esp_err_t esp_light_sleep_start(void)
{
    s_config.ccount_ticks_record = esp_cpu_get_cycle_count();
    esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_GOTO_SLEEP, (void *)0);
#if CONFIG_ESP_TASK_WDT_USE_ESP_TIMER
    esp_err_t timerret = ESP_OK;

    /* If a task watchdog timer is running, we have to stop it. */
    timerret = esp_task_wdt_stop();
#endif // CONFIG_ESP_TASK_WDT_USE_ESP_TIMER

    portENTER_CRITICAL(&s_config.lock);
    /*
    Note: We are about to stall the other CPU via the esp_ipc_isr_stall_other_cpu(). However, there is a chance of
    deadlock if after stalling the other CPU, we attempt to take spinlocks already held by the other CPU that is.

    Thus any functions that we call after stalling the other CPU will need to have the locks taken first to avoid
    deadlock.

    Todo: IDF-5257
    */

    /* We will be calling esp_timer_private_set inside DPORT access critical
     * section. Make sure the code on the other CPU is not holding esp_timer
     * lock, otherwise there will be deadlock.
     */
    esp_timer_private_lock();

    /* We will be calling esp_rtc_get_time_us() below. Make sure the code on the other CPU is not holding the
     * esp_rtc_get_time_us() lock, otherwise there will be deadlock. esp_rtc_get_time_us() is called via:
     *
     * - esp_clk_slowclk_cal_set() -> esp_rtc_get_time_us()
     */
    esp_clk_private_lock();

    s_config.rtc_ticks_at_sleep_start = rtc_time_get();
    uint32_t ccount_at_sleep_start = esp_cpu_get_cycle_count();
    esp_sleep_execute_event_callbacks(SLEEP_EVENT_HW_TIME_START, (void *)0);
    uint64_t high_res_time_at_start = esp_timer_get_time();
    uint32_t sleep_time_overhead_in = (ccount_at_sleep_start - s_config.ccount_ticks_record) / (esp_clk_cpu_freq() / 1000000ULL);

#if CONFIG_ESP_SLEEP_DEBUG
    if (s_sleep_ctx != NULL) {
        s_sleep_ctx->sleep_in_rtc_time_stamp = s_config.rtc_ticks_at_sleep_start;
    }
#endif

#if !CONFIG_FREERTOS_UNICORE
#if ESP_SLEEP_POWER_DOWN_CPU && SOC_PM_CPU_RETENTION_BY_SW
    sleep_smp_cpu_sleep_prepare();
#else
    esp_ipc_isr_stall_other_cpu();
#endif
    esp_ipc_isr_stall_pause();
#endif

#if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION && CONFIG_PM_SLP_IRAM_OPT
    /* Cache Suspend 0: if CONFIG_PM_SLP_IRAM_OPT is enabled, suspend cache here so that the access to flash
       during the sleep process can be explicitly exposed. */
    suspend_cache();
#endif

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

#ifdef CONFIG_ESP_SLEEP_RTC_BUS_ISO_WORKAROUND
    pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
#endif

    // Re-calibrate the RTC clock
    sleep_low_power_clock_calibration(false);

    /*
     * Adjustment time consists of parts below:
     * 1. Hardware time waiting for internal 8M oscillate clock and XTAL;
     * 2. Hardware state switching time of the rtc main state machine;
     * 3. Code execution time when clock is not stable;
     * 4. Code execution time which can be measured;
     */
#if SOC_PMU_SUPPORTED
    int sleep_time_sw_adjustment = LIGHT_SLEEP_TIME_OVERHEAD_US + sleep_time_overhead_in + s_config.sleep_time_overhead_out;
    int sleep_time_hw_adjustment = pmu_sleep_calculate_hw_wait_time(pd_flags, s_config.rtc_clk_cal_period, s_config.fast_clk_cal_period);
    s_config.sleep_time_adjustment = sleep_time_sw_adjustment + sleep_time_hw_adjustment;
#else
    uint32_t rtc_cntl_xtl_buf_wait_slp_cycles = rtc_time_us_to_slowclk(RTC_CNTL_XTL_BUF_WAIT_SLP_US, s_config.rtc_clk_cal_period);
    s_config.sleep_time_adjustment = LIGHT_SLEEP_TIME_OVERHEAD_US + sleep_time_overhead_in + s_config.sleep_time_overhead_out
                                     + rtc_time_slowclk_to_us(rtc_cntl_xtl_buf_wait_slp_cycles + RTC_CNTL_CK8M_WAIT_SLP_CYCLES + RTC_CNTL_WAKEUP_DELAY_CYCLES, s_config.rtc_clk_cal_period);
#endif

    // Decide if VDD_SDIO needs to be powered down;
    // If it needs to be powered down, adjust sleep time.
    const uint32_t flash_enable_time_us = ESP_SLEEP_WAIT_FLASH_READY_DEFAULT_DELAY_US + CONFIG_ESP_SLEEP_WAIT_FLASH_READY_EXTRA_DELAY;

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

        if (can_power_down_vddsdio(pd_flags, vddsdio_pd_sleep_duration)) {
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

    // Safety net: enable WDT in case exit from light sleep fails
    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    bool wdt_was_enabled = wdt_hal_is_enabled(&rtc_wdt_ctx);    // If WDT was enabled in the user code, then do not change it here.
    if (!wdt_was_enabled) {
        wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
        uint32_t stage_timeout_ticks = (uint32_t)(1000ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
        wdt_hal_enable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }

    esp_err_t err = ESP_OK;
    int64_t final_sleep_duration_us = (int64_t)s_config.sleep_duration - (int64_t)s_config.sleep_time_adjustment;
    int64_t min_sleep_duration_us = rtc_time_slowclk_to_us(RTC_CNTL_MIN_SLP_VAL_MIN, s_config.rtc_clk_cal_period);

    // reset light sleep wakeup flag before a new light sleep
    s_light_sleep_wakeup = false;

    s_lightsleep_cnt++;
#if CONFIG_ESP_SLEEP_DEBUG
    if (s_sleep_ctx != NULL) {
        s_sleep_ctx->lightsleep_cnt = s_lightsleep_cnt;
    }
#endif

    // if rtc timer wakeup source is enabled, need to compare final sleep duration and min sleep duration to avoid late wakeup
    if ((s_config.wakeup_triggers & RTC_TIMER_TRIG_EN) && (final_sleep_duration_us <= min_sleep_duration_us)) {
        err = ESP_ERR_SLEEP_TOO_SHORT_SLEEP_DURATION;
    } else {
        // Enter sleep, then wait for flash to be ready on wakeup
        err = esp_light_sleep_inner(pd_flags, flash_enable_time_us);
    }
#if !CONFIG_FREERTOS_UNICORE && ESP_SLEEP_POWER_DOWN_CPU && SOC_PM_CPU_RETENTION_BY_SW
        if (err != ESP_OK) {
            esp_sleep_cpu_skip_retention();
        }
#endif

    // light sleep wakeup flag only makes sense after a successful light sleep
    s_light_sleep_wakeup = (err == ESP_OK);

    // System timer has been stopped for the duration of the sleep, correct for that.
    uint64_t rtc_ticks_at_end = rtc_time_get();
    uint64_t rtc_time_diff = rtc_time_slowclk_to_us(rtc_ticks_at_end - s_config.rtc_ticks_at_sleep_start, s_config.rtc_clk_cal_period);

#if CONFIG_ESP_SLEEP_DEBUG
    if (s_sleep_ctx != NULL) {
        s_sleep_ctx->sleep_out_rtc_time_stamp = rtc_ticks_at_end;
    }
#endif

    /**
     * If sleep duration is too small(less than 1 rtc_slow_clk cycle), rtc_time_diff will be zero.
     * In this case, just ignore the time compensation and keep esp_timer monotonic.
     */
    if (rtc_time_diff > 0) {
        esp_timer_private_set(high_res_time_at_start + rtc_time_diff);
    }
    esp_set_time_from_rtc();

    esp_clk_private_unlock();
    esp_timer_private_unlock();

#if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION && CONFIG_PM_SLP_IRAM_OPT
    /* Cache Resume 0: sleep process done, resume cache for continue running */
    resume_cache();
#endif

#if !CONFIG_FREERTOS_UNICORE
    esp_ipc_isr_stall_resume();
#if ESP_SLEEP_POWER_DOWN_CPU && SOC_PM_CPU_RETENTION_BY_SW
    sleep_smp_cpu_wakeup_prepare();
#else
    esp_ipc_isr_release_other_cpu();
#endif
#endif

    if (!wdt_was_enabled) {
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }

#if CONFIG_ESP_TASK_WDT_USE_ESP_TIMER
    /* Restart the Task Watchdog timer as it was stopped before sleeping. */
    if (timerret == ESP_OK) {
        esp_task_wdt_restart();
    }
#endif // CONFIG_ESP_TASK_WDT_USE_ESP_TIMER

    esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_EXIT_SLEEP, (void *)0);
    s_config.sleep_time_overhead_out = (esp_cpu_get_cycle_count() - s_config.ccount_ticks_record) / (esp_clk_cpu_freq() / 1000000ULL);

#if CONFIG_ESP_SLEEP_DEBUG
    if (s_sleep_ctx != NULL) {
        s_sleep_ctx->sleep_request_result = err;
    }
#endif

    portEXIT_CRITICAL(&s_config.lock);
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
#if SOC_PM_SUPPORT_EXT0_WAKEUP
    } else if (CHECK_SOURCE(source, ESP_SLEEP_WAKEUP_EXT0, RTC_EXT0_TRIG_EN)) {
        s_config.ext0_rtc_gpio_num = 0;
        s_config.ext0_trigger_level = 0;
        s_config.wakeup_triggers &= ~RTC_EXT0_TRIG_EN;
#endif
#if SOC_PM_SUPPORT_EXT1_WAKEUP
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
#if CONFIG_ULP_COPROC_TYPE_FSM
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
#ifndef CONFIG_ULP_COPROC_ENABLED
    return ESP_ERR_INVALID_STATE;
#endif // CONFIG_ULP_COPROC_ENABLED
#if CONFIG_IDF_TARGET_ESP32
#if ((defined CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT) || (defined CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT_V2))
    ESP_LOGE(TAG, "Failed to enable wakeup when provide current to external 32kHz crystal");
    return ESP_ERR_NOT_SUPPORTED;
#endif
    if (s_config.wakeup_triggers & RTC_EXT0_TRIG_EN) {
        ESP_LOGE(TAG, "Conflicting wake-up trigger: ext0");
        return ESP_ERR_INVALID_STATE;
    }
#endif //CONFIG_IDF_TARGET_ESP32

#if CONFIG_ULP_COPROC_TYPE_FSM
    s_config.wakeup_triggers |= RTC_ULP_TRIG_EN;
    return ESP_OK;
#elif CONFIG_ULP_COPROC_TYPE_RISCV
    s_config.wakeup_triggers |= (RTC_COCPU_TRIG_EN | RTC_COCPU_TRAP_TRIG_EN);
    return ESP_OK;
#elif CONFIG_ULP_COPROC_TYPE_LP_CORE
    s_config.wakeup_triggers |= RTC_LP_CORE_TRIG_EN;
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif //CONFIG_ULP_COPROC_TYPE_FSM
}

esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us)
{
    s_config.wakeup_triggers |= RTC_TIMER_TRIG_EN;
    s_config.sleep_duration = time_in_us;
    return ESP_OK;
}

static esp_err_t timer_wakeup_prepare(int64_t sleep_duration)
{
    if (sleep_duration < 0) {
        sleep_duration = 0;
    }

    int64_t ticks = rtc_time_us_to_slowclk(sleep_duration, s_config.rtc_clk_cal_period);
    int64_t target_wakeup_tick = s_config.rtc_ticks_at_sleep_start + ticks;

#if SOC_LP_TIMER_SUPPORTED
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    // Last timer wake-up validity check
    if ((sleep_duration == 0) || \
        (target_wakeup_tick < rtc_time_get() + SLEEP_TIMER_ALARM_TO_SLEEP_TICKS)) {
        // Treat too short sleep duration setting as timer reject
        return ESP_ERR_SLEEP_REJECT;
    }
#endif
    lp_timer_hal_set_alarm_target(0, target_wakeup_tick);
#else
    rtc_hal_set_wakeup_timer(target_wakeup_tick);
#endif

    return ESP_OK;
}

#if SOC_TOUCH_SENSOR_VERSION == 2
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
#elif SOC_TOUCH_SENSOR_VERSION == 3
static void touch_wakeup_prepare(void)
{
    touch_hal_prepare_deep_sleep();
}
#endif

#if SOC_TOUCH_SENSOR_SUPPORTED

esp_err_t esp_sleep_enable_touchpad_wakeup(void)
{
#if CONFIG_IDF_TARGET_ESP32
#if ((defined CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT) || (defined CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT_V2))
    ESP_LOGE(TAG, "Failed to enable wakeup when provide current to external 32kHz crystal");
    return ESP_ERR_NOT_SUPPORTED;
#endif
    if (s_config.wakeup_triggers & (RTC_EXT0_TRIG_EN)) {
        ESP_LOGE(TAG, "Conflicting wake-up trigger: ext0");
        return ESP_ERR_INVALID_STATE;
    }
#endif //CONFIG_IDF_TARGET_ESP32

    s_config.wakeup_triggers |= RTC_TOUCH_TRIG_EN;
    return ESP_OK;
}

touch_pad_t esp_sleep_get_touchpad_wakeup_status(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TOUCHPAD) {
        return TOUCH_PAD_MAX;
    }
    touch_pad_t pad_num;
#if SOC_TOUCH_SENSOR_VERSION == 3
    touch_ll_sleep_get_channel_num((uint32_t *)(&pad_num));
#else
    touch_hal_get_wakeup_status(&pad_num);
#endif
    return pad_num;
}

#endif // SOC_TOUCH_SENSOR_SUPPORTED

bool esp_sleep_is_valid_wakeup_gpio(gpio_num_t gpio_num)
{
#if SOC_RTCIO_PIN_COUNT > 0
    return RTC_GPIO_IS_VALID_GPIO(gpio_num);
#else
    return GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(gpio_num);
#endif
}

#if SOC_PM_SUPPORT_EXT0_WAKEUP
esp_err_t esp_sleep_enable_ext0_wakeup(gpio_num_t gpio_num, int level)
{
    if (level < 0 || level > 1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!esp_sleep_is_valid_wakeup_gpio(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
#if CONFIG_IDF_TARGET_ESP32
    if (s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) {
        ESP_LOGE(TAG, "Conflicting wake-up triggers: touch / ULP");
        return ESP_ERR_INVALID_STATE;
    }
#endif //CONFIG_IDF_TARGET_ESP32

    s_config.ext0_rtc_gpio_num = rtc_io_number_get(gpio_num);
    s_config.ext0_trigger_level = level;
    s_config.wakeup_triggers |= RTC_EXT0_TRIG_EN;
    return ESP_OK;
}

static void ext0_wakeup_prepare(void)
{
    int rtc_gpio_num = s_config.ext0_rtc_gpio_num;
#if SOC_LP_IO_CLOCK_IS_INDEPENDENT
    io_mux_enable_lp_io_clock(rtc_gpio_num, true);
#endif
    rtcio_hal_ext0_set_wakeup_pin(rtc_gpio_num, s_config.ext0_trigger_level);
    rtcio_hal_function_select(rtc_gpio_num, RTCIO_LL_FUNC_RTC);
    rtcio_hal_input_enable(rtc_gpio_num);
}

#endif // SOC_PM_SUPPORT_EXT0_WAKEUP

#if SOC_PM_SUPPORT_EXT1_WAKEUP
esp_err_t esp_sleep_enable_ext1_wakeup(uint64_t io_mask, esp_sleep_ext1_wakeup_mode_t level_mode)
{
    if (io_mask == 0 && level_mode > ESP_EXT1_WAKEUP_ANY_HIGH) {
        return ESP_ERR_INVALID_ARG;
    }
    // Reset all EXT1 configs
    esp_sleep_disable_ext1_wakeup_io(0);

    return esp_sleep_enable_ext1_wakeup_io(io_mask, level_mode);
}


esp_err_t esp_sleep_enable_ext1_wakeup_io(uint64_t io_mask, esp_sleep_ext1_wakeup_mode_t level_mode)
{
    if (io_mask == 0 && level_mode > ESP_EXT1_WAKEUP_ANY_HIGH) {
        return ESP_ERR_INVALID_ARG;
    }
    // Translate bit map of GPIO numbers into the bit map of RTC IO numbers
    uint32_t rtc_gpio_mask = 0;
    for (int gpio = 0; io_mask; ++gpio, io_mask >>= 1) {
        if ((io_mask & 1) == 0) {
            continue;
        }
        if (!esp_sleep_is_valid_wakeup_gpio(gpio)) {
            ESP_LOGE(TAG, "Not an RTC IO: GPIO%d", gpio);
            return ESP_ERR_INVALID_ARG;
        }
        rtc_gpio_mask |= BIT(rtc_io_number_get(gpio));
    }

#if !SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN
    uint32_t ext1_rtc_gpio_mask = 0;
    uint32_t ext1_trigger_mode = 0;

    ext1_rtc_gpio_mask = s_config.ext1_rtc_gpio_mask | rtc_gpio_mask;
    if (level_mode) {
        ext1_trigger_mode = s_config.ext1_trigger_mode | rtc_gpio_mask;
    } else {
        ext1_trigger_mode = s_config.ext1_trigger_mode & (~rtc_gpio_mask);
    }
    if (((ext1_rtc_gpio_mask & ext1_trigger_mode) != ext1_rtc_gpio_mask) &&
    ((ext1_rtc_gpio_mask & ext1_trigger_mode) != 0)) {
        return ESP_ERR_NOT_ALLOWED;
    }
#endif

    s_config.ext1_rtc_gpio_mask |= rtc_gpio_mask;
    if (level_mode) {
        s_config.ext1_trigger_mode |= rtc_gpio_mask;
    } else {
        s_config.ext1_trigger_mode &= (~rtc_gpio_mask);
    }
    s_config.wakeup_triggers |= RTC_EXT1_TRIG_EN;
    return ESP_OK;
}

esp_err_t esp_sleep_disable_ext1_wakeup_io(uint64_t io_mask)
{
    if (io_mask == 0) {
        s_config.ext1_rtc_gpio_mask = 0;
        s_config.ext1_trigger_mode = 0;
    } else {
        // Translate bit map of GPIO numbers into the bit map of RTC IO numbers
        uint32_t rtc_gpio_mask = 0;
        for (int gpio = 0; io_mask; ++gpio, io_mask >>= 1) {
            if ((io_mask & 1) == 0) {
                continue;
            }
            if (!esp_sleep_is_valid_wakeup_gpio(gpio)) {
                ESP_LOGE(TAG, "Not an RTC IO Considering io_mask: GPIO%d", gpio);
                return ESP_ERR_INVALID_ARG;
            }
            rtc_gpio_mask |= BIT(rtc_io_number_get(gpio));
        }
        s_config.ext1_rtc_gpio_mask &= (~rtc_gpio_mask);
        s_config.ext1_trigger_mode &= (~rtc_gpio_mask);
    }

    if (s_config.ext1_rtc_gpio_mask == 0) {
        s_config.wakeup_triggers &= (~RTC_EXT1_TRIG_EN);
    }
    return ESP_OK;
}

#if SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN
esp_err_t esp_sleep_enable_ext1_wakeup_with_level_mask(uint64_t io_mask, uint64_t level_mask)
{
    if ((level_mask & io_mask) != level_mask) {
        return ESP_ERR_INVALID_ARG;
    }
    // Translate bit map of GPIO numbers into the bit map of RTC IO numbers
    // Translate bit map of GPIO wakeup mode into the bit map of RTC IO wakeup mode
    uint32_t rtc_gpio_mask = 0, rtc_gpio_wakeup_mode_mask = 0;
    for (int gpio = 0; io_mask; ++gpio, io_mask >>= 1, level_mask >>= 1) {
        if ((io_mask & 1) == 0) {
            continue;
        }
        if (!esp_sleep_is_valid_wakeup_gpio(gpio)) {
            ESP_LOGE(TAG, "Not an RTC IO Considering io_mask: GPIO%d", gpio);
            return ESP_ERR_INVALID_ARG;
        }
        rtc_gpio_mask |= BIT(rtc_io_number_get(gpio));
        if ((level_mask & 1) == 1) {
            rtc_gpio_wakeup_mode_mask |= BIT(rtc_io_number_get(gpio));
        }
    }
    s_config.ext1_rtc_gpio_mask = rtc_gpio_mask;
    s_config.ext1_trigger_mode = rtc_gpio_wakeup_mode_mask;
    s_config.wakeup_triggers |= RTC_EXT1_TRIG_EN;
    return ESP_OK;
}
#endif

static void ext1_wakeup_prepare(void)
{
    // Configure all RTC IOs selected as ext1 wakeup inputs
    uint32_t rtc_gpio_mask = s_config.ext1_rtc_gpio_mask;
    for (int gpio = 0; gpio < GPIO_PIN_COUNT && rtc_gpio_mask != 0; ++gpio) {
        int rtc_pin = rtc_io_number_get(gpio);
        if ((rtc_gpio_mask & BIT(rtc_pin)) == 0) {
            continue;
        }
#if SOC_LP_IO_CLOCK_IS_INDEPENDENT
        io_mux_enable_lp_io_clock(rtc_pin, true);
#endif
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        // Route pad to RTC
        rtcio_hal_function_select(rtc_pin, RTCIO_LL_FUNC_RTC);
        // set input enable in sleep mode
        rtcio_hal_input_enable(rtc_pin);
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
        // Pad configuration depends on RTC_PERIPH state in sleep mode
        if (s_config.domain[ESP_PD_DOMAIN_RTC_PERIPH].pd_option != ESP_PD_OPTION_ON) {
            rtcio_hal_hold_enable(rtc_pin);
        }
#endif
#else
        /* ESP32H2 use hp iomux to config rtcio, and there is no complete
        * rtcio functionality. In the case of EXT1 wakeup, rtcio only provides
        * a pathway to EXT1. */

        // Route pad to DIGITAL
        rtcio_hal_function_select(rtc_pin, RTCIO_LL_FUNC_DIGITAL);
        // set input enable
        gpio_ll_input_enable(&GPIO, gpio);
        // hold rtc_pin to use it during sleep state
        rtcio_hal_hold_enable(rtc_pin);
#endif
        // Keep track of pins which are processed to bail out early
        rtc_gpio_mask &= ~BIT(rtc_pin);
    }

    // Clear state from previous wakeup
    rtc_hal_ext1_clear_wakeup_status();
    // Set RTC IO pins and mode to be used for wakeup
    rtc_hal_ext1_set_wakeup_pins(s_config.ext1_rtc_gpio_mask, s_config.ext1_trigger_mode);
}

uint64_t esp_sleep_get_ext1_wakeup_status(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT1) {
        return 0;
    }
    uint32_t status = rtc_hal_ext1_get_wakeup_status();
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

#endif // SOC_PM_SUPPORT_EXT1_WAKEUP

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
uint64_t esp_sleep_get_gpio_wakeup_status(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_GPIO) {
        return 0;
    }
    return rtc_hal_gpio_get_wakeup_status();
}

static void gpio_deep_sleep_wakeup_prepare(void)
{
    for (gpio_num_t gpio_idx = GPIO_NUM_0; gpio_idx < GPIO_NUM_MAX; gpio_idx++) {
        if (((1ULL << gpio_idx) & s_config.gpio_wakeup_mask) == 0) {
            continue;
        }
#if CONFIG_ESP_SLEEP_GPIO_ENABLE_INTERNAL_RESISTORS
        if (s_config.gpio_trigger_mode & BIT(gpio_idx)) {
            ESP_ERROR_CHECK(gpio_pullup_dis(gpio_idx));
            ESP_ERROR_CHECK(gpio_pulldown_en(gpio_idx));
        } else {
            ESP_ERROR_CHECK(gpio_pullup_en(gpio_idx));
            ESP_ERROR_CHECK(gpio_pulldown_dis(gpio_idx));
        }
#endif
        ESP_ERROR_CHECK(gpio_hold_en(gpio_idx));
    }
    // Clear state from previous wakeup
    rtc_hal_gpio_clear_wakeup_status();
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
            ESP_LOGE(TAG, "gpio %d is an invalid deep sleep wakeup IO", gpio_idx);
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

esp_err_t esp_sleep_enable_wifi_beacon_wakeup(void)
{
#if SOC_PM_SUPPORT_BEACON_WAKEUP
    s_config.wakeup_triggers |= PMU_WIFI_BEACON_WAKEUP_EN;
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_sleep_disable_wifi_beacon_wakeup(void)
{
#if SOC_PM_SUPPORT_BEACON_WAKEUP
    s_config.wakeup_triggers &= (~PMU_WIFI_BEACON_WAKEUP_EN);
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_sleep_enable_bt_wakeup(void)
{
#if SOC_PM_SUPPORT_BT_WAKEUP
    s_config.wakeup_triggers |= RTC_BT_TRIG_EN;
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_sleep_disable_bt_wakeup(void)
{
#if SOC_PM_SUPPORT_BT_WAKEUP
    s_config.wakeup_triggers &= (~RTC_BT_TRIG_EN);
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

#if SOC_PMU_SUPPORTED
    uint32_t wakeup_cause = pmu_ll_hp_get_wakeup_cause(&PMU);
#else
    uint32_t wakeup_cause = rtc_cntl_ll_get_wakeup_cause();
#endif

    if (wakeup_cause & RTC_TIMER_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_TIMER;
    } else if (wakeup_cause & RTC_GPIO_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_GPIO;
    } else if (wakeup_cause & (RTC_UART0_TRIG_EN | RTC_UART1_TRIG_EN)) {
        return ESP_SLEEP_WAKEUP_UART;
#if SOC_PM_SUPPORT_EXT0_WAKEUP
    } else if (wakeup_cause & RTC_EXT0_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_EXT0;
#endif
#if SOC_PM_SUPPORT_EXT1_WAKEUP
    } else if (wakeup_cause & RTC_EXT1_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_EXT1;
#endif
#if SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP
    } else if (wakeup_cause & RTC_TOUCH_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_TOUCHPAD;
#endif
#if SOC_ULP_FSM_SUPPORTED
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
#if SOC_RISCV_COPROC_SUPPORTED
    } else if (wakeup_cause & RTC_COCPU_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_ULP;
    } else if (wakeup_cause & RTC_COCPU_TRAP_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG;
#endif
#if SOC_LP_CORE_SUPPORTED
    } else if (wakeup_cause & RTC_LP_CORE_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_ULP;
#endif
    } else {
        return ESP_SLEEP_WAKEUP_UNDEFINED;
    }
}

esp_err_t esp_sleep_pd_config(esp_sleep_pd_domain_t domain, esp_sleep_pd_option_t option)
{
    if (domain >= ESP_PD_DOMAIN_MAX || option > ESP_PD_OPTION_AUTO) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL_SAFE(&s_config.lock);

    int refs = (option == ESP_PD_OPTION_ON)  ? s_config.domain[domain].refs++ \
             : (option == ESP_PD_OPTION_OFF) ? --s_config.domain[domain].refs \
             : s_config.domain[domain].refs;
    if (refs == 0) {
        s_config.domain[domain].pd_option = option;
    }
    portEXIT_CRITICAL_SAFE(&s_config.lock);
    assert(refs >= 0);
    return ESP_OK;
}

/**
 * The modules in the CPU and modem power domains still depend on the top power domain.
 * To be safe, the CPU and Modem power domains must also be powered off and saved when
 * the TOP is powered off. If not power down XTAL, power down TOP is meaningless, and
 * the XTAL clock control of some chips(esp32c6/esp32h2) depends on the top domain.
 */
#if SOC_PM_SUPPORT_TOP_PD
FORCE_INLINE_ATTR bool top_domain_pd_allowed(void) {
    bool top_pd_allowed = true;
#if ESP_SLEEP_POWER_DOWN_CPU
    top_pd_allowed &= cpu_domain_pd_allowed();
#else
    top_pd_allowed = false;
#endif
    top_pd_allowed &= clock_domain_pd_allowed();
    top_pd_allowed &= peripheral_domain_pd_allowed();
#if SOC_PM_SUPPORT_MODEM_PD
    top_pd_allowed &= modem_domain_pd_allowed();
#endif
    top_pd_allowed &= (s_config.domain[ESP_PD_DOMAIN_XTAL].pd_option != ESP_PD_OPTION_ON);

    return top_pd_allowed;
}
#endif

static uint32_t get_power_down_flags(void)
{
    // Where needed, convert AUTO options to ON. Later interpret AUTO as OFF.

    // RTC_SLOW_MEM is needed for the ULP, so keep RTC_SLOW_MEM powered up if ULP
    // is used and RTC_SLOW_MEM is Auto.
    // If there is any data placed into .rtc.data or .rtc.bss segments, and
    // RTC_SLOW_MEM is Auto, keep it powered up as well.

#if SOC_PM_SUPPORT_RTC_SLOW_MEM_PD && SOC_ULP_SUPPORTED
    // Labels are defined in the linker script
    extern int _rtc_slow_length, _rtc_reserved_length;
    /**
     * Compiler considers "(size_t) &_rtc_slow_length > 0" to always be true.
     * So use a volatile variable to prevent compiler from doing this optimization.
     */
    volatile size_t rtc_slow_mem_used = (size_t)&_rtc_slow_length + (size_t)&_rtc_reserved_length;

    if ((s_config.domain[ESP_PD_DOMAIN_RTC_SLOW_MEM].pd_option == ESP_PD_OPTION_AUTO) &&
            (rtc_slow_mem_used > 0 || (s_config.wakeup_triggers & RTC_ULP_TRIG_EN))) {
        s_config.domain[ESP_PD_DOMAIN_RTC_SLOW_MEM].pd_option = ESP_PD_OPTION_ON;
    }
#endif

#if SOC_PM_SUPPORT_RTC_FAST_MEM_PD
#if !CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    /* RTC_FAST_MEM is needed for deep sleep stub.
       If RTC_FAST_MEM is Auto, keep it powered on, so that deep sleep stub can run.
       In the new chip revision, deep sleep stub will be optional, and this can be changed. */
    if (s_config.domain[ESP_PD_DOMAIN_RTC_FAST_MEM].pd_option == ESP_PD_OPTION_AUTO) {
        s_config.domain[ESP_PD_DOMAIN_RTC_FAST_MEM].pd_option = ESP_PD_OPTION_ON;
    }
#else
    /* If RTC_FAST_MEM is used for heap, force RTC_FAST_MEM to be powered on. */
    s_config.domain[ESP_PD_DOMAIN_RTC_FAST_MEM].pd_option = ESP_PD_OPTION_ON;
#endif
#endif

#if SOC_PM_SUPPORT_RTC_PERIPH_PD
    // RTC_PERIPH is needed for EXT0 wakeup and GPIO wakeup.
    // If RTC_PERIPH is left auto (EXT0/GPIO aren't enabled), RTC_PERIPH will be powered off by default.
    if (s_config.domain[ESP_PD_DOMAIN_RTC_PERIPH].pd_option == ESP_PD_OPTION_AUTO) {
        if (s_config.wakeup_triggers & (RTC_EXT0_TRIG_EN | RTC_GPIO_TRIG_EN)) {
            s_config.domain[ESP_PD_DOMAIN_RTC_PERIPH].pd_option = ESP_PD_OPTION_ON;
        }
#if CONFIG_IDF_TARGET_ESP32
        else if (s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) {
            // On ESP32, forcing power up of RTC_PERIPH
            // prevents ULP timer and touch FSMs from working correctly.
            s_config.domain[ESP_PD_DOMAIN_RTC_PERIPH].pd_option = ESP_PD_OPTION_OFF;
        }
#endif //CONFIG_IDF_TARGET_ESP32
#if SOC_LP_CORE_SUPPORTED
        else if (s_config.wakeup_triggers &  RTC_LP_CORE_TRIG_EN) {
            // Need to keep RTC_PERIPH on to allow lp core to wakeup during sleep (e.g. from lp timer)
            s_config.domain[ESP_PD_DOMAIN_RTC_PERIPH].pd_option = ESP_PD_OPTION_ON;
        }
#endif //CONFIG_IDF_TARGET_ESP32
    }
#endif // SOC_PM_SUPPORT_RTC_PERIPH_PD

    /**
     * VDD_SDIO power domain shall be kept on during the light sleep
     * when CONFIG_ESP_SLEEP_POWER_DOWN_FLASH is not set and off when it is set.
     * The application can still force the power domain to remain on by calling
     * `esp_sleep_pd_config` before getting into light sleep mode.
     *
     * In deep sleep mode, the power domain will be turned off, regardless the
     * value of this field.
     */
#if SOC_PM_SUPPORT_VDDSDIO_PD
    if (s_config.domain[ESP_PD_DOMAIN_VDDSDIO].pd_option == ESP_PD_OPTION_AUTO) {
#ifndef CONFIG_ESP_SLEEP_POWER_DOWN_FLASH
        s_config.domain[ESP_PD_DOMAIN_VDDSDIO].pd_option = ESP_PD_OPTION_ON;
#endif
    }
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
    s_config.domain[ESP_PD_DOMAIN_XTAL].pd_option = ESP_PD_OPTION_OFF;
#endif

    // Prepare flags based on the selected options
    uint32_t pd_flags = 0;
#if SOC_PM_SUPPORT_RTC_FAST_MEM_PD
    if (s_config.domain[ESP_PD_DOMAIN_RTC_FAST_MEM].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_FAST_MEM;
    }
#endif
#if SOC_PM_SUPPORT_RTC_SLOW_MEM_PD
    if (s_config.domain[ESP_PD_DOMAIN_RTC_SLOW_MEM].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_SLOW_MEM;
    }
#endif
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
    if (s_config.domain[ESP_PD_DOMAIN_RTC_PERIPH].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_RTC_PERIPH;
    }
#endif

#if SOC_PM_SUPPORT_CPU_PD && ESP_SLEEP_POWER_DOWN_CPU
    if ((s_config.domain[ESP_PD_DOMAIN_CPU].pd_option != ESP_PD_OPTION_ON) && cpu_domain_pd_allowed()) {
        pd_flags |= RTC_SLEEP_PD_CPU;
    }
#endif
#if SOC_PM_SUPPORT_XTAL32K_PD
    if (s_config.domain[ESP_PD_DOMAIN_XTAL32K].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= PMU_SLEEP_PD_XTAL32K;
    }
#endif
#if SOC_PM_SUPPORT_RC32K_PD
    if (s_config.domain[ESP_PD_DOMAIN_RC32K].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= PMU_SLEEP_PD_RC32K;
    }
#endif
#if SOC_PM_SUPPORT_RC_FAST_PD
    if (s_config.domain[ESP_PD_DOMAIN_RC_FAST].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_INT_8M;
    }
#endif
    if (s_config.domain[ESP_PD_DOMAIN_XTAL].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_XTAL;
    }
#if SOC_PM_SUPPORT_TOP_PD
    if ((s_config.domain[ESP_PD_DOMAIN_TOP].pd_option != ESP_PD_OPTION_ON) && top_domain_pd_allowed()) {
        pd_flags |= PMU_SLEEP_PD_TOP;
    }
#endif

#if SOC_PM_SUPPORT_MODEM_PD
    if ((s_config.domain[ESP_PD_DOMAIN_MODEM].pd_option != ESP_PD_OPTION_ON) && modem_domain_pd_allowed()
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
        && clock_domain_pd_allowed()
#endif
        ) {
        pd_flags |= RTC_SLEEP_PD_MODEM;
    }
#endif

#if SOC_PM_SUPPORT_CNNT_PD
    if (s_config.domain[ESP_PD_DOMAIN_CNNT].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= PMU_SLEEP_PD_CNNT;
    }
#endif

#if SOC_PM_SUPPORT_VDDSDIO_PD
    if (s_config.domain[ESP_PD_DOMAIN_VDDSDIO].pd_option != ESP_PD_OPTION_ON) {
        pd_flags |= RTC_SLEEP_PD_VDDSDIO;
    }
#endif

#if ((defined CONFIG_RTC_CLK_SRC_EXT_CRYS) && (defined CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT) && (SOC_PM_SUPPORT_RTC_PERIPH_PD))
    if ((s_config.wakeup_triggers & (RTC_TOUCH_TRIG_EN | RTC_ULP_TRIG_EN)) == 0) {
        // If enabled EXT1 only and enable the additional current by touch, should be keep RTC_PERIPH power on.
        pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
    }
#endif

    return pd_flags;
}

#if CONFIG_IDF_TARGET_ESP32
/* APP core of esp32 can't access to RTC FAST MEMORY, do not define it with RTC_IRAM_ATTR */
void
#else
void RTC_IRAM_ATTR
#endif
esp_deep_sleep_disable_rom_logging(void)
{
    rtc_suppress_rom_log();
}

void esp_sleep_enable_adc_tsens_monitor(bool enable)
{
    s_adc_tsen_enabled = enable;
}

void rtc_sleep_enable_ultra_low(bool enable)
{
    s_ultra_low_enabled = enable;
}
