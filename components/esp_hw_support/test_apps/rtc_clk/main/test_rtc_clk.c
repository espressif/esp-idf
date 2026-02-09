/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <inttypes.h>
#include "unity.h"

#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "hal/gpio_ll.h"
#include "soc/io_mux_reg.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_rom_serial_output.h"
#include "test_utils.h"
#include "esp_random.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_private/esp_clk.h"
#include "esp_clk_tree.h"
#include "esp_rtc_time.h"
#include "rom/rtc.h"

#define CALIBRATE_ONE(cali_clk) calibrate_one(cali_clk, #cali_clk)

static uint32_t calibrate_one(soc_clk_freq_calculation_src_t cal_clk, const char* name)
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
    // Need to enable xtal32k/osc_slow explicitly. Other slow clocks enable/disable can be controlled internally.
#if SOC_CLK_XTAL32K_SUPPORTED
    rtc_clk_32k_enable(true);
#endif

#if SOC_CLK_RC_FAST_D256_SUPPORTED
    rtc_clk_8m_enable(true, true);
#endif

    // By default Kconfig, RTC_SLOW_CLK source is RC_SLOW
    soc_rtc_slow_clk_src_t default_rtc_slow_clk_src = rtc_clk_slow_src_get();
    TEST_ASSERT_NOT_EQUAL(0, CALIBRATE_ONE(CLK_CAL_RTC_SLOW));
#if SOC_CLK_RC_FAST_D256_SUPPORTED
    CALIBRATE_ONE(CLK_CAL_RC_FAST_D256);
#endif

#if SOC_CLK_XTAL32K_SUPPORTED
    uint32_t cal_32k = CALIBRATE_ONE(CLK_CAL_32K_XTAL);
    if (cal_32k == 0) {
        printf("32K XTAL OSC has not started up\n");
    } else {
        printf("switching to SOC_RTC_SLOW_CLK_SRC_XTAL32K: ");
        rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_XTAL32K);
        printf("done\n");

        TEST_ASSERT_NOT_EQUAL(0, CALIBRATE_ONE(CLK_CAL_RTC_SLOW));
#if SOC_CLK_RC_FAST_D256_SUPPORTED
        CALIBRATE_ONE(CLK_CAL_RC_FAST_D256);
#endif
        CALIBRATE_ONE(CLK_CAL_32K_XTAL);
    }
#endif

#if SOC_CLK_RC_FAST_D256_SUPPORTED
    printf("switching to SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256: ");
    rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256);
    printf("done\n");

    TEST_ASSERT_NOT_EQUAL(0, CALIBRATE_ONE(CLK_CAL_RTC_SLOW));
    CALIBRATE_ONE(CLK_CAL_RC_FAST_D256);
#if SOC_CLK_XTAL32K_SUPPORTED
    CALIBRATE_ONE(CLK_CAL_32K_XTAL);
#endif
#endif

#if SOC_CLK_OSC_SLOW_SUPPORTED
    rtc_clk_32k_enable_external();
    uint32_t cal_ext_slow_clk = CALIBRATE_ONE(CLK_CAL_32K_OSC_SLOW);
    if (cal_ext_slow_clk == 0) {
        printf("EXT CLOCK by PIN has not started up\n");
    } else {
        printf("switching to SOC_RTC_SLOW_CLK_SRC_OSC_SLOW: ");
        rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_OSC_SLOW);
        printf("done\n");

        TEST_ASSERT_NOT_EQUAL(0, CALIBRATE_ONE(CLK_CAL_RTC_SLOW));
#if SOC_CLK_RC_FAST_D256_SUPPORTED
        CALIBRATE_ONE(CLK_CAL_RC_FAST_D256);
#endif
        CALIBRATE_ONE(CLK_CAL_32K_OSC_SLOW);
    }
#endif

    // Set back to default source
    rtc_clk_slow_src_set(default_rtc_slow_clk_src);
}

#if SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256
TEST_CASE("Test RTC_SLOW_CLK sources switching", "[rtc_clk]")
{
    soc_rtc_slow_clk_src_t clk_src_before_switch = rtc_clk_slow_src_get();
    soc_rtc_slow_clk_src_t switching_sources[] = {SOC_RTC_SLOW_CLK_SRC_RC_SLOW, SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256};

    for (uint32_t test_cnt = 0; test_cnt < 100; test_cnt++) {
        uint32_t src_id = esp_random() % 2;
        if (switching_sources[src_id] == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {
            rtc_clk_8m_enable(true, true);
        }
        rtc_clk_slow_src_set(switching_sources[src_id]);
        esp_rom_delay_us(10*1000);
        TEST_ASSERT_EQUAL(switching_sources[src_id], rtc_clk_slow_src_get());
    }
    rtc_clk_slow_src_set(clk_src_before_switch);
    printf("done\n");
}
#endif

#if SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
TEST_CASE("Calculate RC_FAST clock frequency", "[rtc_clk]")
{
    uint32_t rc_fast_freq_hz;
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_RC_FAST, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &rc_fast_freq_hz);
    printf("RC_FAST_CLK = %"PRIu32" Hz\n", rc_fast_freq_hz);
    TEST_ASSERT_INT32_WITHIN((uint32_t)(SOC_CLK_RC_FAST_FREQ_APPROX * 0.15), SOC_CLK_RC_FAST_FREQ_APPROX, rc_fast_freq_hz);
}
#endif

static void test_clock_switching(void (*switch_func)(const rtc_cpu_freq_config_t* config))
{
    const int test_duration_sec = 10;
    ref_clock_init();
    esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);

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

TEST_CASE("Test switching between PLL and XTAL", "[rtc_clk]")
{
    test_clock_switching(rtc_clk_cpu_freq_set_config);
}

TEST_CASE("Test fast switching between PLL and XTAL", "[rtc_clk]")
{
    test_clock_switching(rtc_clk_cpu_freq_set_config_fast);
}

#if SOC_CLK_XTAL32K_SUPPORTED

extern void rtc_clk_select_rtc_slow_clk(void);

/* In CI environments, the 32kXTAL runners don't have 8MB psram for bank switching.
   So can only test one config or the other. */
#if !defined(CONFIG_IDF_CI_BUILD) || !CONFIG_SPIRAM_BANKSWITCH_ENABLE

#define COUNT_TEST      3
#define TIMEOUT_TEST_MS (5 + CONFIG_RTC_CLK_CAL_CYCLES / 16)

void stop_rtc_external_quartz(void){
#if CONFIG_IDF_TARGET_ESP32
    const uint8_t pin_xtal32k_p = XTAL32K_P_GPIO_NUM;
    const uint8_t pin_xtal32k_n = XTAL32K_N_GPIO_NUM;

    rtc_clk_32k_enable(false);

    esp_rom_gpio_pad_select_gpio(pin_xtal32k_p);
    esp_rom_gpio_pad_select_gpio(pin_xtal32k_n);
    gpio_ll_output_enable(&GPIO, pin_xtal32k_p);
    gpio_ll_output_enable(&GPIO, pin_xtal32k_n);
    gpio_ll_set_level(&GPIO, pin_xtal32k_p, 0);
    gpio_ll_set_level(&GPIO, pin_xtal32k_n, 0);
    esp_rom_delay_us(500000);
    gpio_ll_output_disable(&GPIO, pin_xtal32k_p);
    gpio_ll_output_disable(&GPIO, pin_xtal32k_n);
#endif
    // Other targets no need to do bootstrap
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
        uint32_t fail_measure = 0;
#if SOC_RTC_TIMER_V2_SUPPORTED
        uint64_t clk_rtc_time;
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
#endif
        if(fail_measure == 0) {
            printf("PASS");
        }
        printf(" [calibration val = %"PRIu32"] \n", esp_clk_slowclk_cal_get());
        rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_DEFAULT);
        stop_rtc_external_quartz();
        esp_rom_delay_us(500000);
    }
    TEST_ASSERT_MESSAGE(fail == 0, "Test failed");
    printf("Test passed successfully\n");
}

TEST_CASE("Test starting external RTC quartz", "[rtc_clk][test_env=xtal32k]")
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
    if (CONFIG_RTC_CLK_CAL_CYCLES < 1500) {
        printf("Recommended increase Number of cycles for RTC_SLOW_CLK calibration to 3000!\n");
    }
    while (i < COUNT_TEST) {
        start_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        i++;
        printf("attempt #%d/%d...", i, COUNT_TEST);
        rtc_clk_32k_bootstrap(bootstrap_cycles);
        rtc_clk_select_rtc_slow_clk();
        end_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        printf(" [time=%"PRIu32"] ", end_time - start_time);
        if ((end_time - start_time) > TIMEOUT_TEST_MS) {
            printf("FAIL\n");
            fail = 1;
        } else {
            printf("PASS\n");
        }
        rtc_clk_slow_src_set(SOC_RTC_SLOW_CLK_SRC_DEFAULT);
        stop_rtc_external_quartz();
        esp_rom_delay_us(100000);
    }
    TEST_ASSERT_MESSAGE(fail == 0, "Test failed");
    printf("Test passed successfully\n");
}

TEST_CASE("Test starting 'External 32kHz XTAL' on the board with it.", "[rtc_clk][test_env=xtal32k]")
{
    start_freq(SOC_RTC_SLOW_CLK_SRC_XTAL32K, 200);
    start_freq(SOC_RTC_SLOW_CLK_SRC_XTAL32K, 0);
}

TEST_CASE("Test starting 'External 32kHz XTAL' on the board without it.", "[rtc_clk][test_env=noXtal32k]")
{
    printf("Tries to start the 'External 32kHz XTAL' on the board without it. "
            "Clock switching to 'Internal RC SLOW oscillator'.\n");

    printf("This test will be successful for boards without an external crystal or non-working crystal. "
            "First, there will be an attempt to start from the external crystal after a failure "
            "will switch to the internal RC circuit. If the switch to the internal RC circuit "
            "was successful then the test succeeded.\n");

    start_freq(SOC_RTC_SLOW_CLK_SRC_DEFAULT, 200);
    start_freq(SOC_RTC_SLOW_CLK_SRC_DEFAULT, 0);
}

#endif // !defined(CONFIG_IDF_CI_BUILD) || !CONFIG_SPIRAM_BANKSWITCH_ENABLE
#endif // SOC_CLK_XTAL32K_SUPPORTED

#if SOC_RTC_TIMER_V2_SUPPORTED
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
#endif

#if SOC_DEEP_SLEEP_SUPPORTED
#if SOC_RTC_FAST_MEM_SUPPORTED || SOC_RTC_SLOW_MEM_SUPPORTED
RTC_NOINIT_ATTR
#else
#if CONFIG_IDF_TARGET_ESP32C2
#define TEMP_RTC_STORE_REG          RTC_CNTL_DATE_REG
#define TEMP_RTC_STORE_REG_M        RTC_CNTL_DATE_M
#elif CONFIG_IDF_TARGET_ESP32C61
#define TEMP_RTC_STORE_REG          LP_AON_DATE_REG
#define TEMP_RTC_STORE_REG_M        LP_AON_DATE_M
#elif CONFIG_IDF_TARGET_ESP32H4
#include "soc/pmu_reg.h"
#define TEMP_RTC_STORE_REG          PMU_DATE_REG
#define TEMP_RTC_STORE_REG_M        PMU_PMU_DATE_M
#endif
#endif
static int64_t start = 0;

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
#if !(SOC_RTC_FAST_MEM_SUPPORTED || SOC_RTC_SLOW_MEM_SUPPORTED)
    // Store start timestamp in RTC_CNTL_DATE reg if RTC MEM is not supported
    TEST_ASSERT(start <= TEMP_RTC_STORE_REG_M);
    REG_WRITE(TEMP_RTC_STORE_REG, start & TEMP_RTC_STORE_REG_M);
#endif
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
#if !(SOC_RTC_FAST_MEM_SUPPORTED || SOC_RTC_SLOW_MEM_SUPPORTED)
    start  = REG_READ(TEMP_RTC_STORE_REG);
#endif
    TEST_ASSERT_GREATER_THAN(start, end);

    esp_clk_slowclk_cal_set(esp_clk_slowclk_cal_get() * 2);

    // Delay for error accumulation.
    vTaskDelay(pdMS_TO_TICKS(10*1000));

    start = esp_rtc_get_time_us();
#if !(SOC_RTC_FAST_MEM_SUPPORTED || SOC_RTC_SLOW_MEM_SUPPORTED)
    TEST_ASSERT(start <= TEMP_RTC_STORE_REG_M);
    REG_WRITE(TEMP_RTC_STORE_REG, start & TEMP_RTC_STORE_REG_M);
#endif
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
#if !(SOC_RTC_FAST_MEM_SUPPORTED || SOC_RTC_SLOW_MEM_SUPPORTED)
    start  = REG_READ(TEMP_RTC_STORE_REG);
#endif
    TEST_ASSERT_GREATER_THAN(start, end);
}

TEST_CASE_MULTIPLE_STAGES("Test rtc clk calibration compensation across deep sleep", "[rtc_clk]", trigger_deepsleep, check_time_deepsleep_1, check_time_deepsleep_2);
#endif

/* The following three are not unit tests, but are added here to make it easy to
 * check the frequency of 150k/32k/8M oscillators. The following three "tests" will
 * output either 150k, 32k, 8M clock to GPIO25.
 */
#if CONFIG_IDF_TARGET_ESP32
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"

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

TEST_CASE("Output 8M clock to GPIO25", "[ignore]")
{
    rtc_clk_8m_enable(true, true);
    SET_PERI_REG_MASK(RTC_IO_RTC_DEBUG_SEL_REG, RTC_IO_DEBUG_12M_NO_GATING);
    pull_out_clk(RTC_IO_DEBUG_SEL0_8M);
}
#endif
