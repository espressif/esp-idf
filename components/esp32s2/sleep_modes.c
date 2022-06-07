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
#include "esp_log.h"
#include "esp32s2/clk.h"
#include "esp_newlib.h"
#include "esp_spi_flash.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rom/uart.h"
#include "esp32s2/rom/ets_sys.h"
#include "esp32s2/brownout.h"
#include "hal/touch_sensor_hal.h"
#include "driver/touch_sensor.h"
#include "driver/touch_sensor_common.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/spi_periph.h"
#include "soc/dport_reg.h"
#include "soc/extmem_reg.h"
#include "soc/soc_memory_layout.h"
#include "soc/uart_caps.h"
#include "hal/wdt_hal.h"
#include "hal/clk_gate_ll.h"
#include "driver/rtc_io.h"
#include "hal/touch_sensor_hal.h"
#include "driver/touch_sensor.h"
#include "driver/touch_sensor_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

// If light sleep time is less than that, don't power down flash
#define FLASH_PD_MIN_SLEEP_TIME_US  2000

// Time from VDD_SDIO power up to first flash read in ROM code
#define VDD_SDIO_POWERUP_TO_FLASH_READ_US 700

// Extra time it takes to enter and exit light sleep and deep sleep
// For deep sleep, this is until the wake stub runs (not the app).
#ifdef CONFIG_ESP32S2_RTC_CLK_SRC_EXT_CRYS
#define LIGHT_SLEEP_TIME_OVERHEAD_US (1650 + 30 * 240 / CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ)
#define DEEP_SLEEP_TIME_OVERHEAD_US (650 + 100 * 240 / CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ)
#else
#define LIGHT_SLEEP_TIME_OVERHEAD_US (1250 + 30 * 240 / CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ)
#define DEEP_SLEEP_TIME_OVERHEAD_US (250 + 100 * 240 / CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ)
#endif // CONFIG_ESP32S2_RTC_CLK_SRC_EXT_CRYS

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
static void touch_wakeup_prepare(void);

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
    REG_SET_BIT(EXTMEM_CACHE_DBG_INT_ENA_REG, EXTMEM_CACHE_DBG_EN);
}

void __attribute__((weak, alias("esp_default_wake_deep_sleep"))) esp_wake_deep_sleep(void);

void esp_deep_sleep(uint64_t time_in_us)
{
    esp_sleep_enable_timer_wakeup(time_in_us);
    esp_deep_sleep_start();
}

static void IRAM_ATTR flush_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
        if (periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            uart_tx_wait_idle(i);
        }
    }
}

static void IRAM_ATTR suspend_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
        if (periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
            REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_SW_FLOW_CON_EN | UART_FORCE_XOFF);
            while (REG_GET_FIELD(UART_FSM_STATUS_REG(i), UART_ST_UTX_OUT) != 0) {
                ;
            }
        }
    }
}

static void IRAM_ATTR resume_uarts(void)
{
    for (int i = 0; i < SOC_UART_NUM; ++i) {
        if (periph_ll_periph_enabled(PERIPH_UART0_MODULE + i)) {
            REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
            REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
            REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_SW_FLOW_CON_EN | UART_FORCE_XON);
        }
    }
}

inline static uint32_t IRAM_ATTR call_rtc_sleep_start(uint32_t reject_triggers);

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
    // Enable ULP wakeup
    if (s_config.wakeup_triggers & RTC_ULP_TRIG_EN) {
        REG_SET_BIT(RTC_CNTL_INT_CLR_REG, RTC_CNTL_ULP_CP_INT_CLR);
        REG_SET_BIT(RTC_CNTL_INT_CLR_REG, RTC_CNTL_COCPU_INT_CLR);
        REG_SET_BIT(RTC_CNTL_INT_CLR_REG, RTC_CNTL_COCPU_TRAP_INT_CLR);
    }

    extern void regi2c_analog_cali_reg_read(void);
    regi2c_analog_cali_reg_read();

    if (deep_sleep) {
        if (s_config.wakeup_triggers & RTC_TOUCH_TRIG_EN) {
            touch_wakeup_prepare();
            /* Workaround: In deep sleep, for ESP32S2, Power down the RTC_PERIPH will change the slope configuration of Touch sensor sleep pad.
             * The configuration change will change the reading of the sleep pad, which will cause the touch wake-up sensor to trigger falsely.
             */
            pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
        }
    } else {
        /* In light sleep, the RTC_PERIPH power domain should be in the power-on state (Power on the touch circuit in light sleep),
         * otherwise the touch sensor FSM will be cleared, causing touch sensor false triggering.
         */
        if (touch_ll_get_fsm_state()) { // Check if the touch sensor is working properly.
            pd_flags &= ~RTC_SLEEP_PD_RTC_PERIPH;
        }
    }

    // Enter sleep
    rtc_sleep_config_t config = RTC_SLEEP_CONFIG_DEFAULT(pd_flags);
    rtc_sleep_init(config);

    // Configure timer wakeup
    if (s_config.wakeup_triggers & RTC_TIMER_TRIG_EN) {
        timer_wakeup_prepare();
    }

    uint32_t result;
    if (deep_sleep) {
        /* Disable interrupts in case another task writes to RTC memory while we
         * calculate RTC memory CRC
         */
        portENTER_CRITICAL(&spinlock_rtc_deep_sleep);

#if !CONFIG_ESP32S2_ALLOW_RTC_FAST_MEM_AS_HEAP
        /* If not possible stack is in RTC FAST memory, use the ROM function to calculate the CRC and save ~140 bytes IRAM */
        set_rtc_memory_crc();
        result = call_rtc_sleep_start(0);
#else
        /* Otherwise, need to call the dedicated soc function for this */
        result = rtc_deep_sleep_start(s_config.wakeup_triggers, 0);
#endif

        portEXIT_CRITICAL(&spinlock_rtc_deep_sleep);
    } else {
        result = call_rtc_sleep_start(0);
    }

    // Restore CPU frequency
    rtc_clk_cpu_freq_set_config(&cpu_freq_config);

    extern void regi2c_analog_cali_reg_write(void);
    regi2c_analog_cali_reg_write();

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
    /* Due to hardware limitations, on S2 the brownout detector sometimes trigger during deep sleep
       to circumvent this we disable the brownout detector before sleeping  */
    esp_brownout_disable();

    // record current RTC time
    s_config.rtc_ticks_at_sleep_start = rtc_time_get();
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
        ets_delay_us(flash_enable_time_us);
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
    uint64_t frc_time_at_start = esp_timer_get_time();
    DPORT_STALL_OTHER_CPU_START();

    // Decide which power domains can be powered down
    uint32_t pd_flags = get_power_down_flags();

    // Amount of time to subtract from actual sleep time.
    // This is spent on entering and leaving light sleep.
    s_config.sleep_time_adjustment = LIGHT_SLEEP_TIME_OVERHEAD_US;

    // Decide if VDD_SDIO needs to be powered down;
    // If it needs to be powered down, adjust sleep time.
    const uint32_t flash_enable_time_us = VDD_SDIO_POWERUP_TO_FLASH_READ_US;

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
    uint64_t frc_time_at_end = esp_timer_get_time();

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
#ifdef CONFIG_ESP32S2_ULP_COPROC_ENABLED
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
    return ESP_ERR_NOT_SUPPORTED;
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
    int64_t rtc_count_delta = rtc_time_us_to_slowclk(sleep_duration, period);
    rtc_sleep_set_wakeup_time(s_config.rtc_ticks_at_sleep_start + rtc_count_delta);
    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG, RTC_CNTL_MAIN_TIMER_INT_CLR_M);
    SET_PERI_REG_MASK(RTC_CNTL_SLP_TIMER1_REG, RTC_CNTL_MAIN_TIMER_ALARM_EN_M);
}

/* In deep sleep mode, only the sleep channel is supported, and other touch channels should be turned off. */
static void touch_wakeup_prepare(void)
{
    touch_pad_t touch_num = TOUCH_PAD_NUM0;
    touch_ll_sleep_get_channel_num(&touch_num); // Check if the sleep pad is enabled.
    if ((touch_num > TOUCH_PAD_NUM0) && (touch_num < TOUCH_PAD_MAX) && touch_ll_get_fsm_state()) {
        touch_ll_stop_fsm();
        touch_ll_clear_channel_mask(TOUCH_PAD_BIT_MASK_MAX);
        touch_ll_set_channel_mask(BIT(touch_num));
        touch_ll_start_fsm();
    }
}

esp_err_t esp_sleep_enable_touchpad_wakeup(void)
{
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
    // Set GPIO to be used for wakeup
    REG_SET_FIELD(RTC_IO_EXT_WAKEUP0_REG, RTC_IO_EXT_WAKEUP0_SEL, rtc_gpio_num);
    // Set level which will trigger wakeup
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
            s_config.ext0_trigger_level, RTC_CNTL_EXT_WAKEUP0_LV_S);
    // Find GPIO descriptor in the rtc_io_desc table and configure the pad
    const rtc_io_desc_t* desc = &rtc_io_desc[rtc_gpio_num];
    REG_SET_BIT(desc->reg, desc->mux);
    SET_PERI_REG_BITS(desc->reg, 0x3, 0, desc->func);
    REG_SET_BIT(desc->reg, desc->ie);
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
        const rtc_io_desc_t* desc = &rtc_io_desc[rtc_pin];
        // Route pad to RTC
        REG_SET_BIT(desc->reg, desc->mux);
        SET_PERI_REG_BITS(desc->reg, 0x3, 0, desc->func);
        // set input enable in sleep mode
        REG_SET_BIT(desc->reg, desc->ie);
        // Pad configuration depends on RTC_PERIPH state in sleep mode
        if (s_config.pd_options[ESP_PD_DOMAIN_RTC_PERIPH] != ESP_PD_OPTION_ON) {
            // RTC_PERIPH will be powered down, so RTC_IO_ registers will
            // loose their state. Lock pad configuration.
            // Pullups/pulldowns also need to be disabled.
            REG_CLR_BIT(desc->reg, desc->pulldown);
            REG_CLR_BIT(desc->reg, desc->pullup);
            REG_SET_BIT(RTC_CNTL_PAD_HOLD_REG, desc->hold_force);
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

uint64_t esp_sleep_get_ext1_wakeup_status(void)
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
    if (uart_num == 0) {
        s_config.wakeup_triggers |= RTC_UART0_TRIG_EN;
    } else if (uart_num == 1) {
        s_config.wakeup_triggers |= RTC_UART1_TRIG_EN;
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t esp_sleep_enable_wifi_wakeup(void)
{
    s_config.wakeup_triggers |= RTC_WIFI_TRIG_EN;
    return ESP_OK;
}

esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause(void)
{
    if (rtc_get_reset_reason(0) != DEEPSLEEP_RESET && !s_light_sleep_wakeup) {
        return ESP_SLEEP_WAKEUP_UNDEFINED;
    }

    uint32_t wakeup_cause = REG_GET_FIELD(RTC_CNTL_SLP_WAKEUP_CAUSE_REG, RTC_CNTL_WAKEUP_CAUSE);
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
    } else if (wakeup_cause & RTC_WIFI_TRIG_EN) {
        return ESP_SLEEP_WAKEUP_WIFI;
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

    // Labels are defined in the linker script, see esp32s2.ld.
    extern int _rtc_slow_length;
    /**
     * Compiler considers "(size_t) &_rtc_slow_length > 0" to always be true.
     * So use a volatile variable to prevent compiler from doing this optimization.
     */
    volatile size_t rtc_slow_mem_used = (size_t)&_rtc_slow_length;

    if ((s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] == ESP_PD_OPTION_AUTO) &&
            (rtc_slow_mem_used > 0 || (s_config.wakeup_triggers & RTC_ULP_TRIG_EN))) {
        s_config.pd_options[ESP_PD_DOMAIN_RTC_SLOW_MEM] = ESP_PD_OPTION_ON;
    }

#if !CONFIG_ESP32S2_ALLOW_RTC_FAST_MEM_AS_HEAP
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
    /* This function is called from a critical section, log with ESP_EARLY_LOGD. */
    ESP_EARLY_LOGD(TAG, "RTC_PERIPH: %s, RTC_SLOW_MEM: %s, RTC_FAST_MEM: %s",
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
//    if (s_config.pd_options[ESP_PD_DOMAIN_XTAL] != ESP_PD_OPTION_ON) {
//        pd_flags |= RTC_SLEEP_PD_XTAL;
//    }
    return pd_flags;
}
