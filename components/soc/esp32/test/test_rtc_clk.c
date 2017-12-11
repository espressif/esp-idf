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
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);

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


