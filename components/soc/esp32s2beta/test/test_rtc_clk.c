#include <stdio.h>
#include "unity.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "soc/io_mux_reg.h"
#include "driver/rtc_io.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "../esp_clk_internal.h"
#include "esp_clk.h"


#ifdef CONFIG_CHIP_IS_ESP32
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
    rtc_clk_32k_enable(true);
    rtc_clk_8m_enable(true, true);

    CALIBRATE_ONE(RTC_CAL_RTC_MUX);
    CALIBRATE_ONE(RTC_CAL_8MD256);
    uint32_t cal_32k = CALIBRATE_ONE(RTC_CAL_32K_XTAL);

    if (cal_32k == 0) {
        printf("32K XTAL OSC has not started up");
    } else {
        printf("switching to RTC_SLOW_FREQ_32K_XTAL: ");
        rtc_clk_slow_freq_set(RTC_SLOW_FREQ_32K_XTAL);
        printf("done\n");

        CALIBRATE_ONE(RTC_CAL_RTC_MUX);
        CALIBRATE_ONE(RTC_CAL_8MD256);
        CALIBRATE_ONE(RTC_CAL_32K_XTAL);
    }

    printf("switching to RTC_SLOW_FREQ_8MD256: ");
    rtc_clk_slow_freq_set(RTC_SLOW_FREQ_8MD256);
    printf("done\n");

    CALIBRATE_ONE(RTC_CAL_RTC_MUX);
    CALIBRATE_ONE(RTC_CAL_8MD256);
    CALIBRATE_ONE(RTC_CAL_32K_XTAL);
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

TEST_CASE("Output 150k clock to GPIO25", "[rtc_clk][ignore]")
{
    pull_out_clk(RTC_IO_DEBUG_SEL0_150K_OSC);
}

TEST_CASE("Output 32k XTAL clock to GPIO25", "[rtc_clk][ignore]")
{
    rtc_clk_32k_enable(true);
    pull_out_clk(RTC_IO_DEBUG_SEL0_32K_XTAL);
}

TEST_CASE("Output 8M XTAL clock to GPIO25", "[rtc_clk][ignore]")
{
    rtc_clk_8m_enable(true, true);
    SET_PERI_REG_MASK(RTC_IO_RTC_DEBUG_SEL_REG, RTC_IO_DEBUG_12M_NO_GATING);
    pull_out_clk(RTC_IO_DEBUG_SEL0_8M);
}

static void test_clock_switching(void (*switch_func)(rtc_cpu_freq_t))
{
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    const int test_duration_sec = 10;
    ref_clock_init();
    uint64_t t_start = ref_clock_get();

    rtc_cpu_freq_t cur_freq = rtc_clk_cpu_freq_get();
    int count = 0;
    while (ref_clock_get() - t_start < test_duration_sec * 1000000) {
        switch_func(RTC_CPU_FREQ_XTAL);
        switch_func(cur_freq);
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
    printf("RTC_FAST_CLK=%d Hz\n", rtc_fast_freq_hz);
    TEST_ASSERT_INT32_WITHIN(500000, RTC_FAST_CLK_FREQ_APPROX, rtc_fast_freq_hz);
}

TEST_CASE("Test switching between PLL and XTAL", "[rtc_clk]")
{
    test_clock_switching(rtc_clk_cpu_freq_set);
}

TEST_CASE("Test fast switching between PLL and XTAL", "[rtc_clk]")
{
    test_clock_switching(rtc_clk_cpu_freq_set_fast);
}

#define COUNT_TEST      3
#define TIMEOUT_TEST_MS (5 + CONFIG_ESP32_RTC_CLK_CAL_CYCLES / 16)

void stop_rtc_external_quartz(void){
    const uint8_t pin_32 = 32;
    const uint8_t pin_33 = 33;
    const uint8_t mask_32 = (1 << (pin_32 - 32));
    const uint8_t mask_33 = (1 << (pin_33 - 32));

    rtc_clk_32k_enable(false);

    gpio_pad_select_gpio(pin_32);
    gpio_pad_select_gpio(pin_33);
    gpio_output_set_high(0, mask_32 | mask_33, mask_32 | mask_33, 0);
    ets_delay_us(500000);
    gpio_output_set_high(0, 0, 0, mask_32 | mask_33); // disable pins
}

static void start_freq(rtc_slow_freq_t required_src_freq, uint32_t start_delay_ms)
{
    int i = 0, fail = 0;
    uint32_t start_time;
    uint32_t end_time;
    rtc_slow_freq_t selected_src_freq;
    stop_rtc_external_quartz();
#ifdef CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL
    uint32_t bootstrap_cycles = CONFIG_ESP32_RTC_XTAL_BOOTSTRAP_CYCLES;
    printf("Test is started. Kconfig settings:\n External 32K crystal is selected,\n Oscillation cycles = %d,\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_ESP32_RTC_CLK_CAL_CYCLES);
#else
    uint32_t bootstrap_cycles = 5;
    printf("Test is started. Kconfig settings:\n Internal RC is selected,\n Oscillation cycles = %d,\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_ESP32_RTC_CLK_CAL_CYCLES);
#endif
    if (start_delay_ms == 0 && CONFIG_ESP32_RTC_CLK_CAL_CYCLES < 1500){
        start_delay_ms = 50;
        printf("Recommended increase Number of cycles for RTC_SLOW_CLK calibration to 3000!\n");
    }
    while(i < COUNT_TEST){
        start_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        i++;
        printf("attempt #%d/%d...", i, COUNT_TEST);
        rtc_clk_32k_bootstrap(bootstrap_cycles);
        ets_delay_us(start_delay_ms * 1000);
        rtc_clk_select_rtc_slow_clk();
        selected_src_freq = rtc_clk_slow_freq_get();
        end_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        printf(" [time=%d] ", (end_time - start_time) - start_delay_ms);
        if(selected_src_freq != required_src_freq){
            printf("FAIL. Time measurement...");
            fail = 1;
        } else {
            printf("PASS. Time measurement...");
        }
        uint64_t clk_rtc_time;
        uint32_t fail_measure = 0;
        for (int j = 0; j < 3; ++j) {
            clk_rtc_time = esp_clk_rtc_time();
            ets_delay_us(1000000);
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
        printf(" [calibration val = %d] \n", esp_clk_slowclk_cal_get());
        stop_rtc_external_quartz();
        ets_delay_us(500000);
    }
    TEST_ASSERT_MESSAGE(fail == 0, "Test failed");
    printf("Test passed successfully\n");
}

TEST_CASE("Test starting external RTC quartz", "[rtc_clk][test_env=UT_T1_32kXTAL]")
{
    int i = 0, fail = 0;
    uint32_t start_time;
    uint32_t end_time;
    stop_rtc_external_quartz();
#ifdef CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL
    uint32_t bootstrap_cycles = CONFIG_ESP32_RTC_XTAL_BOOTSTRAP_CYCLES;
    printf("Test is started. Kconfig settings:\n External 32K crystal is selected,\n Oscillation cycles = %d,\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_ESP32_RTC_CLK_CAL_CYCLES);
#else
    uint32_t bootstrap_cycles = 5;
    printf("Test is started. Kconfig settings:\n Internal RC is selected,\n Oscillation cycles = %d,\n Calibration cycles = %d.\n",
            bootstrap_cycles,
            CONFIG_ESP32_RTC_CLK_CAL_CYCLES);
#endif
    if (CONFIG_ESP32_RTC_CLK_CAL_CYCLES < 1500){
        printf("Recommended increase Number of cycles for RTC_SLOW_CLK calibration to 3000!\n");
    }
    while(i < COUNT_TEST){
        start_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        i++;
        printf("attempt #%d/%d...", i, COUNT_TEST);
        rtc_clk_32k_bootstrap(bootstrap_cycles);
        rtc_clk_select_rtc_slow_clk();
        end_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
        printf(" [time=%d] ", end_time - start_time);
        if((end_time - start_time) > TIMEOUT_TEST_MS){
            printf("FAIL\n");
            fail = 1;
        } else {
            printf("PASS\n");
        }
        stop_rtc_external_quartz();
        ets_delay_us(100000);
    }
    TEST_ASSERT_MESSAGE(fail == 0, "Test failed");
    printf("Test passed successfully\n");
}

TEST_CASE("Test starting 'External 32kHz XTAL' on the board with it.", "[rtc_clk][test_env=UT_T1_32kXTAL]")
{
    start_freq(RTC_SLOW_FREQ_32K_XTAL, 200);
    start_freq(RTC_SLOW_FREQ_32K_XTAL, 0);
}

TEST_CASE("Test starting 'External 32kHz XTAL' on the board without it.", "[rtc_clk][test_env=UT_T1_no32kXTAL]")
{
    printf("Tries to start the 'External 32kHz XTAL' on the board without it. "
            "Clock switching to 'Internal 150 kHz RC oscillator'.\n");

    printf("This test will be successful for boards without an external crystal or non-working crystal. "
            "First, there will be an attempt to start from the external crystal after a failure "
            "will switch to the internal RC circuit. If the switch to the internal RC circuit "
            "was successful then the test succeeded.\n");

    start_freq(RTC_SLOW_FREQ_RTC, 200);
    start_freq(RTC_SLOW_FREQ_RTC, 0);
}

#endif
