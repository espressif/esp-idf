/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <inttypes.h>
#include "unity.h"

#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#if SOC_ADC_RTC_CTRL_SUPPORTED
#include "soc/sens_reg.h"
#endif
#include "soc/gpio_periph.h"
#include "hal/gpio_ll.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"
#include "test_utils.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_private/esp_clk.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rtc.h"
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rtc.h"
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rtc.h"
#include "esp32s3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rtc.h"
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rtc.h"
#include "esp32c2/rom/rtc.h"
#endif

// ESP32C2 does not support SLOW_CLK_32K_XTAL, so no need to test related test cases
// Please notice this when enabling the rtc_clk test for ESP32C2!
#if !CONFIG_IDF_TARGET_ESP32C2
extern void rtc_clk_select_rtc_slow_clk(void);
#endif

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3, ESP32C3, ESP32C2)

#define CALIBRATE_ONE(cali_clk) calibrate_one(cali_clk, #cali_clk)

static uint32_t calibrate_one(rtc_cal_sel_t cal_clk, const char* name)
{
    const uint32_t cal_count = 1000;
    const float factor = (1 << 19) * 1000.0f;
    uint32_t cali_val;
    printf("%s:\n", name);
    for (int i = 0; i < 5; ++i) {
        printf("calibrate (%d): ", i);
        cali_val = rtc_clk_cal(cal_clk, cal_count);
        printf("%.3f kHz\n", factor / (float) cali_val);
    }
    return cali_val;
}

TEST_CASE("RTC_SLOW_CLK sources calibration", "[rtc_clk]")
{
#if !CONFIG_IDF_TARGET_ESP32C2
    rtc_clk_32k_enable(true);
#endif
    rtc_clk_8m_enable(true, true);

    CALIBRATE_ONE(RTC_CAL_RTC_MUX);
    CALIBRATE_ONE(RTC_CAL_8MD256);

#if CONFIG_IDF_TARGET_ESP32C2
    uint32_t cal_ext_slow_clk = CALIBRATE_ONE(RTC_CAL_32K_OSC_SLOW);
    if (cal_ext_slow_clk == 0) {
        printf("EXT CLOCK by PIN has not started up");
    } else {
        printf("switching to SOC_RTC_SLOW_CLK_SRC_OSC_SLOW: ");
        rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_OSC_SLOW);
        printf("done\n");

        CALIBRATE_ONE(RTC_CAL_RTC_MUX);
        CALIBRATE_ONE(RTC_CAL_8MD256);
        CALIBRATE_ONE(RTC_CAL_32K_OSC_SLOW);
    }
#else
    uint32_t cal_32k = CALIBRATE_ONE(RTC_CAL_32K_XTAL);
    if (cal_32k == 0) {
        printf("32K XTAL OSC has not started up");
    } else {
        printf("switching to SOC_RTC_SLOW_CLK_SRC_XTAL32K: ");
        rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_XTAL32K);
        printf("done\n");
        CALIBRATE_ONE(RTC_CAL_RTC_MUX);
        CALIBRATE_ONE(RTC_CAL_8MD256);
        CALIBRATE_ONE(RTC_CAL_32K_XTAL);
    }
#endif

    printf("switching to SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256: ");
    rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256);
    printf("done\n");

    CALIBRATE_ONE(RTC_CAL_RTC_MUX);
    CALIBRATE_ONE(RTC_CAL_8MD256);
#if CONFIG_IDF_TARGET_ESP32C2
    CALIBRATE_ONE(RTC_CAL_32K_OSC_SLOW);
#else
    CALIBRATE_ONE(RTC_CAL_32K_XTAL);
#endif
}

/* The following two are not unit tests, but are added here to make it easy to
 * check the frequency of 150k/32k oscillators. The following two "tests" will
 * output either 32k or 150k clock to GPIO25.
 */

static void pull_out_clk(int sel)
{
    REG_SET_BIT(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_MUX_SEL_M);
    REG_CLR_BIT(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_RDE_M | RTC_IO_PDAC1_RUE_M);
    REG_SET_FIELD(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_FUN_SEL, 1);
    REG_SET_FIELD(SENS_SAR_DAC_CTRL1_REG, SENS_DEBUG_BIT_SEL, 0);
    REG_SET_FIELD(RTC_IO_RTC_DEBUG_SEL_REG, RTC_IO_DEBUG_SEL0, sel);
}

TEST_CASE("Output 150k clock to GPIO25", "[ignore]")
{
    pull_out_clk(RTC_IO_DEBUG_SEL0_150K_OSC);
}

TEST_CASE("Output 32k XTAL clock to GPIO25", "[ignore]")
{
    rtc_clk_32k_enable(true);
    pull_out_clk(RTC_IO_DEBUG_SEL0_32K_XTAL);
}

TEST_CASE("Output 8M XTAL clock to GPIO25", "[ignore]")
{
    rtc_clk_8m_enable(true, true);
    SET_PERI_REG_MASK(RTC_IO_RTC_DEBUG_SEL_REG, RTC_IO_DEBUG_12M_NO_GATING);
    pull_out_clk(RTC_IO_DEBUG_SEL0_8M);
}

static void test_clock_switching(void (*switch_func)(const rtc_cpu_freq_config_t* config))
{
    esp_rom_uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    const int test_duration_sec = 10;
    ref_clock_init();
    uint64_t t_start = ref_clock_get();

    rtc_cpu_freq_config_t cur_config;
    rtc_clk_cpu_freq_get_config(&cur_config);

    rtc_cpu_freq_config_t xtal_config;
    rtc_clk_cpu_freq_mhz_to_config((uint32_t) rtc_clk_xtal_freq_get(), &xtal_config);

    int count = 0;
    while (ref_clock_get() - t_start < test_duration_sec * 1000000) {
        switch_func(&xtal_config);
        switch_func(&cur_config);
        ++count;
    }
    uint64_t t_end = ref_clock_get();
    printf("Switch count: %d. Average time to switch PLL -> XTAL -> PLL: %d us\n", count, (int) ((t_end - t_start) / count));
    ref_clock_deinit();
}

TEST_CASE("Calculate 8M clock frequency", "[rtc_clk]")
{
    // calibrate 8M/256 clock against XTAL, get 8M/256 clock period
    uint32_t rtc_8md256_period = rtc_clk_cal(RTC_CAL_8MD256, 100);
    uint32_t rtc_fast_freq_hz = 1000000ULL * (1 << RTC_CLK_CAL_FRACT) * 256 / rtc_8md256_period;
    printf("RTC_FAST_CLK=%"PRIu32" Hz\n", rtc_fast_freq_hz);
    TEST_ASSERT_INT32_WITHIN(650000, SOC_CLK_RC_FAST_FREQ_APPROX, rtc_fast_freq_hz);
}

TEST_CASE("Test switching between PLL and XTAL", "[rtc_clk]")
{
    test_clock_switching(rtc_clk_cpu_freq_set_config);
}

TEST_CASE("Test fast switching between PLL and XTAL", "[rtc_clk]")
{
    test_clock_switching(rtc_clk_cpu_freq_set_config_fast);
}

/* In CI environments, the 32kXTAL runners don't have 8MB psram for bank switching.
   So can only test one config or the other. */
#if !defined(CONFIG_IDF_CI_BUILD) || !CONFIG_SPIRAM_BANKSWITCH_ENABLE

#define COUNT_TEST      3
#define TIMEOUT_TEST_MS (5 + CONFIG_RTC_CLK_CAL_CYCLES / 16)

void stop_rtc_external_quartz(void){
    const uint8_t pin_32 = 32;
    const uint8_t pin_33 = 33;

    rtc_clk_32k_enable(false);

    esp_rom_gpio_pad_select_gpio(pin_32);
    esp_rom_gpio_pad_select_gpio(pin_33);
    gpio_ll_output_enable(&GPIO, pin_32);
    gpio_ll_output_enable(&GPIO, pin_33);
    gpio_ll_set_level(&GPIO, pin_32, 0);
    gpio_ll_set_level(&GPIO, pin_33, 0);
    esp_rom_delay_us(500000);
    gpio_ll_output_disable(&GPIO, pin_32);
    gpio_ll_output_disable(&GPIO, pin_33);
}

static void start_freq(soc_rtc_slow_clk_src_t required_src, uint32_t start_delay_ms)
{
    int i = 0, fail = 0;
    uint32_t start_time;
    uint32_t end_time;
    soc_rtc_slow_clk_src_t selected_src;
    stop_rtc_external_quartz();
#ifdef CONFIG_RTC_CLK_SRC_EXT_CRYS
    uint32_t bootstrap_cycles = CONFIG_ESP_SYSTEM_RTC_EXT_XTAL_BOOTSTRAP_CYCLES;
    printf("Test is started. Kconfig settings:\n External 32K crystal is selected,\n Oscillation cycles = %"PRIu32",\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_RTC_CLK_CAL_CYCLES);
#else
    uint32_t bootstrap_cycles = 5;
    printf("Test is started. Kconfig settings:\n Internal RC is selected,\n Oscillation cycles = %"PRIu32",\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_RTC_CLK_CAL_CYCLES);
#endif // CONFIG_RTC_CLK_SRC_EXT_CRYS
    if (start_delay_ms == 0 && CONFIG_RTC_CLK_CAL_CYCLES < 1500){
        start_delay_ms = 50;
        printf("Recommended increase Number of cycles for RTC_SLOW_CLK calibration to 3000!\n");
    }
    while(i < COUNT_TEST){
        start_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        i++;
        printf("attempt #%d/%d...", i, COUNT_TEST);
        rtc_clk_32k_bootstrap(bootstrap_cycles);
        esp_rom_delay_us(start_delay_ms * 1000);
        rtc_clk_select_rtc_slow_clk();
        selected_src = rtc_clk_slow_src_get();
        end_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        printf(" [time=%"PRIu32"] ", (end_time - start_time) - start_delay_ms);
        if(selected_src != required_src){
            printf("FAIL. Time measurement...");
            fail = 1;
        } else {
            printf("PASS. Time measurement...");
        }
        uint64_t clk_rtc_time;
        uint32_t fail_measure = 0;
        for (int j = 0; j < 3; ++j) {
            clk_rtc_time = esp_clk_rtc_time();
            esp_rom_delay_us(1000000);
            uint64_t delta = esp_clk_rtc_time() - clk_rtc_time;
            if (delta < 900000LL || delta > 1100000){
                printf("FAIL");
                fail = 1;
                fail_measure = 1;
                break;
            }
        }
        if(fail_measure == 0) {
            printf("PASS");
        }
        printf(" [calibration val = %"PRIu32"] \n", esp_clk_slowclk_cal_get());
        stop_rtc_external_quartz();
        esp_rom_delay_us(500000);
    }
    TEST_ASSERT_MESSAGE(fail == 0, "Test failed");
    printf("Test passed successfully\n");
}

TEST_CASE("Test starting external RTC quartz", "[test_env=xtal32k]")
{
    int i = 0, fail = 0;
    uint32_t start_time;
    uint32_t end_time;
    stop_rtc_external_quartz();
#ifdef CONFIG_RTC_CLK_SRC_EXT_CRYS
    uint32_t bootstrap_cycles = CONFIG_ESP_SYSTEM_RTC_EXT_XTAL_BOOTSTRAP_CYCLES;
    printf("Test is started. Kconfig settings:\n External 32K crystal is selected,\n Oscillation cycles = %"PRIu32",\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_RTC_CLK_CAL_CYCLES);
#else
    uint32_t bootstrap_cycles = 5;
    printf("Test is started. Kconfig settings:\n Internal RC is selected,\n Oscillation cycles = %"PRIu32",\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_RTC_CLK_CAL_CYCLES);
#endif // CONFIG_RTC_CLK_SRC_EXT_CRYS
    if (CONFIG_RTC_CLK_CAL_CYCLES < 1500){
        printf("Recommended increase Number of cycles for RTC_SLOW_CLK calibration to 3000!\n");
    }
    while(i < COUNT_TEST){
        start_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        i++;
        printf("attempt #%d/%d...", i, COUNT_TEST);
        rtc_clk_32k_bootstrap(bootstrap_cycles);
        rtc_clk_select_rtc_slow_clk();
        end_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        printf(" [time=%"PRIu32"] ", end_time - start_time);
        if((end_time - start_time) > TIMEOUT_TEST_MS){
            printf("FAIL\n");
            fail = 1;
        } else {
            printf("PASS\n");
        }
        stop_rtc_external_quartz();
        esp_rom_delay_us(100000);
    }
    TEST_ASSERT_MESSAGE(fail == 0, "Test failed");
    printf("Test passed successfully\n");
}

TEST_CASE("Test starting 'External 32kHz XTAL' on the board with it.", "[test_env=xtal32k]")
{
    start_freq(SOC_RTC_SLOW_CLK_SRC_XTAL32K, 200);
    start_freq(SOC_RTC_SLOW_CLK_SRC_XTAL32K, 0);
}

TEST_CASE("Test starting 'External 32kHz XTAL' on the board without it.", "[test_env=noXtal32k]")
{
    printf("Tries to start the 'External 32kHz XTAL' on the board without it. "
            "Clock switching to 'Internal 150 kHz RC oscillator'.\n");

    printf("This test will be successful for boards without an external crystal or non-working crystal. "
            "First, there will be an attempt to start from the external crystal after a failure "
            "will switch to the internal RC circuit. If the switch to the internal RC circuit "
            "was successful then the test succeeded.\n");

    start_freq(SOC_RTC_SLOW_CLK_SRC_RC_SLOW, 200);
    start_freq(SOC_RTC_SLOW_CLK_SRC_RC_SLOW, 0);
}

#endif // !defined(CONFIG_IDF_CI_BUILD) || !CONFIG_SPIRAM_BANKSWITCH_ENABLE

#endif // !TEMPORARY_DISABLED_FOR_TARGETS(...)

TEST_CASE("Test rtc clk calibration compensation", "[rtc_clk]")
{
    int64_t t1 = esp_rtc_get_time_us();

    // Modify calibration value
    esp_clk_slowclk_cal_set(esp_clk_slowclk_cal_get() / 2);

    // Delay for error accumulation.
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Internally, the origin point of rtc clk has been adjusted
    // so that t2 > t1 remains true
    int64_t t2 = esp_rtc_get_time_us();

    TEST_ASSERT_GREATER_THAN(t1, t2);

    // Restore calibration value
    esp_clk_slowclk_cal_set(esp_clk_slowclk_cal_get() * 2);

    // Delay for error accumulation.
    vTaskDelay(pdMS_TO_TICKS(1000));

    t2 = esp_rtc_get_time_us();

    TEST_ASSERT_GREATER_THAN(t1, t2);
}

static RTC_NOINIT_ATTR int64_t start = 0;

static void trigger_deepsleep(void)
{
    printf("Trigger deep sleep. Waiting for 10 sec ...\n");

    // Simulate the dispersion of the calibration coefficients at start-up.
    // Corrupt the calibration factor.
    esp_clk_slowclk_cal_set(esp_clk_slowclk_cal_get() / 2);

    // Delay for error accumulation.
    vTaskDelay(pdMS_TO_TICKS(10*1000));

    // Save start time. Deep sleep.
    start = esp_rtc_get_time_us();
    esp_sleep_enable_timer_wakeup(5000);
    // In function esp_deep_sleep_start() uses function esp_sync_timekeeping_timers()
    // to prevent a negative time after wake up.
    esp_deep_sleep_start();
}

static void check_time_deepsleep_1(void)
{
    soc_reset_reason_t reason = esp_rom_get_reset_reason(0);
    TEST_ASSERT(reason == RESET_REASON_CORE_DEEP_SLEEP);
    int64_t end = esp_rtc_get_time_us();
    TEST_ASSERT_GREATER_THAN(start, end);

    esp_clk_slowclk_cal_set(esp_clk_slowclk_cal_get() * 2);

    // Delay for error accumulation.
    vTaskDelay(pdMS_TO_TICKS(10*1000));

    start = esp_rtc_get_time_us();

    esp_sleep_enable_timer_wakeup(5000);
    // In function esp_deep_sleep_start() uses function esp_sync_timekeeping_timers()
    // to prevent a negative time after wake up.
    esp_deep_sleep_start();
}

static void check_time_deepsleep_2(void)
{
    soc_reset_reason_t reason = esp_rom_get_reset_reason(0);
    TEST_ASSERT(reason == RESET_REASON_CORE_DEEP_SLEEP);
    int64_t end = esp_rtc_get_time_us();
    TEST_ASSERT_GREATER_THAN(start, end);
}

TEST_CASE_MULTIPLE_STAGES("Test rtc clk calibration compensation across deep sleep", "", trigger_deepsleep, check_time_deepsleep_1, check_time_deepsleep_2);
