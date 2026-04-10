/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "lp_core_test_app.h"
#include "lp_core_test_app_counter.h"
#if CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB
#include "lp_core_test_app_wake_stub.h"
#endif
#include "lp_core_test_app_isr.h"
#include "ulp_lp_core_cpu_freq_shared.h"

#if SOC_RTC_TIMER_SUPPORTED
#include "lp_core_test_app_set_timer_wakeup.h"
#endif

#include "lp_core_test_app_gpio.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_lp_timer_shared.h"
#include "test_shared.h"
#include "unity.h"
#include "esp_sleep.h"
#if CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB
#include "esp_wake_stub.h"
#endif
#include "esp_timer.h"
#include "esp_private/esp_clk.h"
#include "hal/rtc_timer_ll.h"
#include "soc/rtc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal/lp_core_ll.h"
#include "hal/rtc_io_ll.h"
#include "driver/rtc_io.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_test_app_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_test_app_bin_end");

extern const uint8_t lp_core_main_counter_bin_start[] asm("_binary_lp_core_test_app_counter_bin_start");
extern const uint8_t lp_core_main_counter_bin_end[]   asm("_binary_lp_core_test_app_counter_bin_end");

#if CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB
extern const uint8_t lp_core_main_wake_stub_bin_start[] asm("_binary_lp_core_test_app_wake_stub_bin_start");
extern const uint8_t lp_core_main_wake_stub_bin_end[]   asm("_binary_lp_core_test_app_wake_stub_bin_end");
#endif

extern const uint8_t lp_core_main_set_timer_wakeup_bin_start[] asm("_binary_lp_core_test_app_set_timer_wakeup_bin_start");
extern const uint8_t lp_core_main_set_timer_wakeup_bin_end[]   asm("_binary_lp_core_test_app_set_timer_wakeup_bin_end");

extern const uint8_t lp_core_main_gpio_bin_start[] asm("_binary_lp_core_test_app_gpio_bin_start");
extern const uint8_t lp_core_main_gpio_bin_end[]   asm("_binary_lp_core_test_app_gpio_bin_end");

extern const uint8_t lp_core_main_isr_bin_start[] asm("_binary_lp_core_test_app_isr_bin_start");
extern const uint8_t lp_core_main_isr_bin_end[]   asm("_binary_lp_core_test_app_isr_bin_end");

extern const uint8_t lp_core_main_exception_bin_start[] asm("_binary_lp_core_test_app_exception_bin_start");
extern const uint8_t lp_core_main_exception_bin_end[]   asm("_binary_lp_core_test_app_exception_bin_end");

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);

}

static void clear_test_cmds(void)
{
    ulp_main_cpu_command = LP_CORE_NO_COMMAND;
    ulp_command_resp = LP_CORE_NO_COMMAND;
}

TEST_CASE("LP core and main CPU are able to exchange data", "[lp_core]")
{
    const uint32_t test_data = 0x12345678;

    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_bin_start, lp_core_main_bin_end);

    /* Setup test data */
    ulp_test_data_in = test_data ^ XOR_MASK;
    ulp_main_cpu_command = LP_CORE_READ_WRITE_TEST;

    /* Wait till we receive the correct command response */
    while (ulp_command_resp != LP_CORE_READ_WRITE_TEST) {
    }

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == LP_CORE_READ_WRITE_TEST);

    /* Wait till we receive COMMAND_OK reply */
    while (ulp_main_cpu_reply != LP_CORE_COMMAND_OK) {
    }

    printf("data out: 0x%" PRIx32 ", expected: 0x%" PRIx32 " \n", ulp_test_data_out, test_data);
    TEST_ASSERT(test_data == ulp_test_data_out);

    clear_test_cmds();
}

TEST_CASE("Test LP core delay", "[lp_core]")
{
    int64_t start, diff;
    const uint32_t delay_period_us = 5000000;
    const uint32_t delta_us = 500000; // RTC FAST is not very accurate

    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_bin_start, lp_core_main_bin_end);

    /* Setup test data */
    ulp_test_data_in = delay_period_us;
    ulp_main_cpu_command = LP_CORE_DELAY_TEST;

    /* Wait till we receive the correct command response */
    while (ulp_command_resp != LP_CORE_DELAY_TEST) {
    }

    start = esp_timer_get_time();

    /* Wait till we receive COMMAND_OK reply */
    while (ulp_main_cpu_reply != LP_CORE_COMMAND_OK) {
    }

    diff = esp_timer_get_time() - start;

    printf("Waited for %" PRIi64 "us, expected: %" PRIi32 "us\n", diff, delay_period_us);
    TEST_ASSERT_INT_WITHIN(delta_us, delay_period_us, diff);

    clear_test_cmds();
}

static void print_table_header(void)
{
    printf("%10s %10s %10s %10s %10s\n",
           "----------", "----------", "----------", "----------", "----------");
    printf("%10s %10s %10s %10s %10s\n",
           "delay(us)", "expec_tk", "avg_tk", "error_tk", "in_range");
    printf("%10s %10s %10s %10s %10s\n",
           "----------", "----------", "----------", "----------", "----------");
}

static void run_delay_calibration_test(lp_core_test_commands_t delay_calibration_cmd, const char *delay_api_name)
{
    volatile uint32_t *command_resp = (volatile uint32_t *)&ulp_command_resp;
    volatile uint32_t *main_cpu_command = (volatile uint32_t *)&ulp_main_cpu_command;
    volatile uint32_t *delay_sub_command = (volatile uint32_t *)&ulp_delay_sub_command;
    volatile uint32_t *test_data_in = (volatile uint32_t *)&ulp_test_data_in;
    const uint32_t test_delays_us[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 40, 50, 70, 90,
        100, 200, 500, 1000, 2000, 5000,
        10000, 20000, 50000, 100000,
    };
    const uint32_t lp_cpu_freq_hz = LP_CORE_CPU_FREQUENCY_HZ;
    const double lp_tick_time_us = 1000000.0 / lp_cpu_freq_hz;
    const double lp_ticks_per_us = 1.0 / lp_tick_time_us;
    const double error_limit_time_us = 0.30;
    const int32_t error_limit_ticks = (int32_t)(error_limit_time_us * lp_ticks_per_us + 0.5);
    const uint32_t warmup_exclude_ticks = 0;
    int32_t correction_ticks_checked = 0;
    uint32_t out_of_range_count = 0;
    uint32_t checked_measurements = 0;
    uint32_t skipped_warmup_measurements = 0;
    printf("Delay API under test: %s\n", delay_api_name);
    printf("LP CPU frequency: %" PRIu32 " Hz, tick time: %.6f us, ticks/us: %.3f\n",
           lp_cpu_freq_hz, lp_tick_time_us, lp_ticks_per_us);
    printf("Tolerance: +/- %" PRId32 " ticks (%.3f us)\n", error_limit_ticks, error_limit_time_us);

    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_bin_start, lp_core_main_bin_end);

    *delay_sub_command = LP_CORE_DELAY_SUBCMD_NONE;
    *command_resp = LP_CORE_NO_COMMAND;
    *main_cpu_command = delay_calibration_cmd;

    while (*command_resp != delay_calibration_cmd) { };

    print_table_header();
    const uint32_t num_delays = sizeof(test_delays_us) / sizeof(test_delays_us[0]);
    for (uint32_t delay_idx = 0; delay_idx < num_delays; delay_idx++) {
        const uint32_t requested_delay_us = test_delays_us[delay_idx];
        uint64_t sum_delay_ticks = 0;
        const uint32_t samples = 3;
        const uint32_t expected_ticks = requested_delay_us * lp_ticks_per_us;

        for (uint32_t sample_idx = 0; sample_idx < samples; sample_idx++) {

            *test_data_in = (delay_calibration_cmd == LP_CORE_DELAY_CYCLES_CALIBRATION_TEST) ? expected_ticks : requested_delay_us;
            while (*delay_sub_command != LP_CORE_DELAY_SUBCMD_READY) { };
            *delay_sub_command = LP_CORE_DELAY_SUBCMD_RUN;

            /* Wait for the ULP to finish its delay before reading the cycle counters from RTC memory.
             * The HP core delay must be at least as long as the requested ULP delay, with some margin
             * to ensure that we do not read RTC memory while the ULP is running the delay calibration code. */
            vTaskDelay(((requested_delay_us / 1000 + portTICK_PERIOD_MS + 1) / portTICK_PERIOD_MS) * 3);
            sum_delay_ticks += ulp_delay_end_cycles - ulp_delay_start_cycles;
        }

        const uint64_t avg_delay_ticks = sum_delay_ticks / samples;
        const int32_t error_tk = expected_ticks - avg_delay_ticks;
        const char *range_status = "PASS";

        if (expected_ticks <= warmup_exclude_ticks) {
            skipped_warmup_measurements++;
            range_status = "SKIP";
        } else {
            checked_measurements++;
            correction_ticks_checked += error_tk;
            if ((error_tk < -error_limit_ticks) || (error_tk > error_limit_ticks)) {
                out_of_range_count++;
                range_status = "FAIL";
            }
        }

        printf("%10" PRIu32 " %10" PRIu32 " %10" PRIu64 " %10" PRId32 " %10s\n",
               requested_delay_us, expected_ticks, avg_delay_ticks, error_tk, range_status);
    }
    print_table_header();

    /* Release LP core from delay-test loop */
    *main_cpu_command = LP_CORE_NO_COMMAND;
    ulp_test_data_in = 0;
    *delay_sub_command = LP_CORE_DELAY_SUBCMD_RUN;

    TEST_ASSERT_MESSAGE(checked_measurements > 0, "No delay calibration points were checked");
    double total_correct_ticks = (double) correction_ticks_checked / (double) checked_measurements;
    printf("Average correction ticks: %.1f (%.3f us)\n", total_correct_ticks, total_correct_ticks * lp_tick_time_us);
    printf("Range-check summary (%s): checked=%" PRIu32 ", skipped_warmup=%" PRIu32 ", out_of_range=%" PRIu32
           ", limit=+/- %" PRId32 " ticks\n",
           delay_api_name, checked_measurements, skipped_warmup_measurements, out_of_range_count, error_limit_ticks);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, out_of_range_count, "Delay calibration has out-of-range error_tk values");
    TEST_ASSERT_DOUBLE_WITHIN(5.0, 0.0, total_correct_ticks);

    clear_test_cmds();
}

TEST_CASE("Test delay calibration for ulp_lp_core_delay_cycles", "[lp_core]")
{
    run_delay_calibration_test(LP_CORE_DELAY_CYCLES_CALIBRATION_TEST, "ulp_lp_core_delay_cycles");
}

TEST_CASE("Test delay calibration for ulp_lp_core_delay_us", "[lp_core]")
{
    run_delay_calibration_test(LP_CORE_DELAY_US_CALIBRATION_TEST, "ulp_lp_core_delay_us");
}

#define LP_TIMER_TEST_DURATION_S        (5)
#define LP_TIMER_TEST_SLEEP_DURATION_US (20000)

#if SOC_DEEP_SLEEP_SUPPORTED

static void do_ulp_wakeup_deepsleep(lp_core_test_commands_t ulp_cmd)
{
    /* Load ULP firmware and start the ULP RISC-V Coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_bin_start, lp_core_main_bin_end);

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_main_cpu_command = ulp_cmd;

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

static void check_reset_reason_ulp_wakeup(void)
{
    TEST_ASSERT_EQUAL(BIT(ESP_SLEEP_WAKEUP_ULP), esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_ULP));
}

static void do_ulp_wakeup_after_short_delay_deepsleep(void)
{
    do_ulp_wakeup_deepsleep(LP_CORE_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST);
}

static void do_ulp_wakeup_after_long_delay_deepsleep(void)
{
    do_ulp_wakeup_deepsleep(LP_CORE_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST);
}

TEST_CASE_MULTIPLE_STAGES("LP-core is able to wakeup main CPU from deep sleep after a short delay", "[ulp]",
                          do_ulp_wakeup_after_short_delay_deepsleep,
                          check_reset_reason_ulp_wakeup);

/* Certain erroneous wake-up triggers happen only after sleeping for a few seconds  */
TEST_CASE_MULTIPLE_STAGES("LP-core is able to wakeup main CPU from deep sleep after a long delay", "[ulp]",
                          do_ulp_wakeup_after_long_delay_deepsleep,
                          check_reset_reason_ulp_wakeup);

RTC_FAST_ATTR static struct timeval tv_start;

#define ULP_COUNTER_WAKEUP_LIMIT_CNT 50

static void do_ulp_wakeup_with_lp_timer_deepsleep(void)
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = LP_TIMER_TEST_SLEEP_DURATION_US,
#if ESP_ROM_HAS_LP_ROM
        /* ROM Boot takes quite a bit longer, which skews the numbers of wake-ups. skip rom boot to keep the calculation simple */
        .skip_lp_rom_boot = true,
#endif
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_counter_bin_start, lp_core_main_counter_bin_end);
    ulp_counter_wakeup_limit = ULP_COUNTER_WAKEUP_LIMIT_CNT;

    gettimeofday(&tv_start, NULL);

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();

    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

static void check_reset_reason_and_sleep_duration(void)
{
    struct timeval tv_stop = {};
    gettimeofday(&tv_stop, NULL);

    TEST_ASSERT_EQUAL(BIT(ESP_SLEEP_WAKEUP_ULP), esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_ULP));

    int64_t sleep_duration = (tv_stop.tv_sec - tv_start.tv_sec) * 1000 + (tv_stop.tv_usec - tv_start.tv_usec) / 1000;
    int64_t expected_sleep_duration_ms = ulp_counter_wakeup_limit * LP_TIMER_TEST_SLEEP_DURATION_US / 1000;

    printf("CPU slept for %"PRIi64" ms, expected it to sleep approx %"PRIi64" ms\n", sleep_duration, expected_sleep_duration_ms);
    /* Rough estimate, as CPU spends quite some time waking up, but will test if lp core is waking up way too often etc */
    TEST_ASSERT_INT_WITHIN_MESSAGE(1000, expected_sleep_duration_ms, sleep_duration, "LP Core did not wake up the expected number of times");

    clear_test_cmds();
}

TEST_CASE_MULTIPLE_STAGES("LP Timer can wakeup lp core periodically during deep sleep", "[ulp]",
                          do_ulp_wakeup_with_lp_timer_deepsleep,
                          check_reset_reason_and_sleep_duration);

#if CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB
/* Number of LP core wakeup cycles the stub handles before falling through to full boot */
#define WAKE_STUB_LP_WAKEUP_COUNT       10
/* LP timer period for wake stub test */
#define WAKE_STUB_LP_TIMER_PERIOD_US    2000000  /* 200 ms */
/* Safety backup timer in case LP core never wakes up HP */
#define WAKE_STUB_BACKUP_TIMER_US       5000000 /* 5 s */

static RTC_DATA_ATTR uint32_t s_wake_stub_run_count;
static RTC_DATA_ATTR uint64_t s_stub_first_rtc_tick;
static RTC_DATA_ATTR uint64_t s_stub_last_rtc_tick;

static void RTC_IRAM_ATTR wake_stub_lp_core_test(void)
{
    s_wake_stub_run_count++;
    if (s_wake_stub_run_count == 1) {
        s_stub_first_rtc_tick = rtc_timer_ll_get_cycle_count(0);
    }
    if (s_wake_stub_run_count < WAKE_STUB_LP_WAKEUP_COUNT) {
        esp_wake_stub_uart_tx_wait_idle(0);
        esp_wake_stub_sleep(&wake_stub_lp_core_test);
    }
    /* On the Nth run record the final tick and fall through to full boot */
    s_stub_last_rtc_tick = rtc_timer_ll_get_cycle_count(0);
}

static void do_lp_core_wake_stub_deepsleep(void)
{
    s_wake_stub_run_count = 0;
    s_stub_first_rtc_tick = 0;
    s_stub_last_rtc_tick = 0;

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = WAKE_STUB_LP_TIMER_PERIOD_US,
    };
    load_and_start_lp_core_firmware(&cfg, lp_core_main_wake_stub_bin_start, lp_core_main_wake_stub_bin_end);

    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    /* Backup timer so test doesn't hang if LP core never wakes HP */
    //TEST_ASSERT(esp_sleep_enable_timer_wakeup(WAKE_STUB_BACKUP_TIMER_US) == ESP_OK);

    esp_set_deep_sleep_wake_stub(&wake_stub_lp_core_test);

    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

static void check_lp_core_wake_stub_ran_correctly(void)
{
    /* Must have woken via ULP (stub fell through on Nth run), not the backup timer */
    TEST_ASSERT_EQUAL(BIT(ESP_SLEEP_WAKEUP_ULP), esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_ULP));

    /* Stub should have run exactly WAKE_STUB_LP_WAKEUP_COUNT times */
    TEST_ASSERT_EQUAL(WAKE_STUB_LP_WAKEUP_COUNT, s_wake_stub_run_count);

    /* Compute the time between first and last stub run using RTC timer ticks
     * recorded inside the stub itself, excluding any boot or menu latency.
     * Between run 1 and run N there are (N-1) LP timer periods.
     * With the PMU re-wakeup bug the PMU re-triggers immediately after
     * esp_wake_stub_sleep(), so all 10 runs complete in ~162ms.
     * With the fix each cycle respects the 200ms LP timer, so the inter-run
     * span is at least (N-1)*period/2 = 900ms. */
    uint64_t rtc_ticks = s_stub_last_rtc_tick - s_stub_first_rtc_tick;
    uint32_t cal = esp_clk_slowclk_cal_get();
    uint64_t elapsed_ms = (((uint64_t)rtc_ticks * cal) >> RTC_CLK_CAL_FRACT) / 1000;
    uint64_t min_expected_ms = (uint64_t)(WAKE_STUB_LP_WAKEUP_COUNT - 1) * WAKE_STUB_LP_TIMER_PERIOD_US / 2000;

    printf("Wake stub ran %" PRIu32 " times, inter-run span: %" PRIu64 "ms (min expected: %" PRIu64 "ms)\n",
           s_wake_stub_run_count, elapsed_ms, min_expected_ms);
    TEST_ASSERT_MESSAGE(elapsed_ms >= min_expected_ms,
                        "Wake stub ran too quickly - possible PMU re-wakeup bug");
}

TEST_CASE_MULTIPLE_STAGES("LP core wake stub can sleep again without immediate re-wakeup", "[ulp]",
                          do_lp_core_wake_stub_deepsleep,
                          check_lp_core_wake_stub_ran_correctly);

#endif //#if CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB

#endif //#if SOC_DEEP_SLEEP_SUPPORTED

TEST_CASE("LP Timer can wakeup lp core periodically", "[lp_core]")
{
    int64_t start, test_duration;
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = LP_TIMER_TEST_SLEEP_DURATION_US,
#if ESP_ROM_HAS_LP_ROM
        /* ROM Boot takes quite a bit longer, which skews the numbers of wake-ups. skip rom boot to keep the calculation simple */
        .skip_lp_rom_boot = true,
#endif
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_counter_bin_start, lp_core_main_counter_bin_end);

    start = esp_timer_get_time();
    vTaskDelay(pdMS_TO_TICKS(LP_TIMER_TEST_DURATION_S * 1000));

    test_duration = esp_timer_get_time() - start;
    uint32_t expected_run_count = test_duration / LP_TIMER_TEST_SLEEP_DURATION_US;
    printf("LP core ran %"PRIu32" times in %"PRIi64" ms, expected it to run approx %"PRIu32" times\n", ulp_counter, test_duration / 1000, expected_run_count);

    TEST_ASSERT_INT_WITHIN_MESSAGE(5, expected_run_count, ulp_counter, "LP Core did not wake up the expected number of times");
}

static bool ulp_is_running(uint32_t *counter_variable)
{
    uint32_t start_cnt = *counter_variable;

    /* Wait a few ULP wakeup cycles to ensure ULP has run */
    vTaskDelay((5 * LP_TIMER_TEST_SLEEP_DURATION_US / 1000) / portTICK_PERIOD_MS);

    uint32_t end_cnt = *counter_variable;
    printf("start run count: %" PRIu32 ", end run count %" PRIu32 "\n", start_cnt, end_cnt);

    /* If the ulp is running the counter should have been incremented */
    return (start_cnt != end_cnt);
}

#define STOP_TEST_ITERATIONS 10

TEST_CASE("LP core can be stopped and and started again from main CPU", "[ulp]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = LP_TIMER_TEST_SLEEP_DURATION_US,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_counter_bin_start, lp_core_main_counter_bin_end);

    TEST_ASSERT(ulp_is_running(&ulp_counter));

    for (int i = 0; i < STOP_TEST_ITERATIONS; i++) {
        ulp_lp_core_stop();
        TEST_ASSERT(!ulp_is_running(&ulp_counter));

        ulp_lp_core_run(&cfg);
        TEST_ASSERT(ulp_is_running(&ulp_counter));
    }
}

#if SOC_RTC_TIMER_SUPPORTED
TEST_CASE("LP core can schedule next wake-up time by itself", "[ulp]")
{
    int64_t start, test_duration;

    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
#if ESP_ROM_HAS_LP_ROM
        /* ROM Boot takes quite a bit longer, which skews the numbers of wake-ups. skip rom boot to keep the calculation simple */
        .skip_lp_rom_boot = true,
#endif
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_set_timer_wakeup_bin_start, lp_core_main_set_timer_wakeup_bin_end);

    start = esp_timer_get_time();
    vTaskDelay(pdMS_TO_TICKS(LP_TIMER_TEST_DURATION_S * 1000));

    test_duration = esp_timer_get_time() - start;
    /* ULP will alternative between setting WAKEUP_PERIOD_BASE_US and 2*WAKEUP_PERIOD_BASE_US
       as a wakeup period which should give an average wakeup time of 1.5*WAKEUP_PERIOD_BASE_US */
    uint32_t expected_run_count = test_duration / (1.5 * ulp_WAKEUP_PERIOD_BASE_US);
    printf("LP core ran %"PRIu32" times in %"PRIi64" ms, expected it to run approx %"PRIu32" times\n", ulp_set_timer_wakeup_counter, test_duration / 1000, expected_run_count);

    TEST_ASSERT_INT_WITHIN_MESSAGE(5, expected_run_count, ulp_set_timer_wakeup_counter, "LP Core did not wake up the expected number of times");
}

#if SOC_RTCIO_PIN_COUNT > 0
TEST_CASE("LP core gpio tests", "[ulp]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = LP_TIMER_TEST_SLEEP_DURATION_US,
    };

    rtc_gpio_init(GPIO_NUM_0);
    load_and_start_lp_core_firmware(&cfg, lp_core_main_gpio_bin_start, lp_core_main_gpio_bin_end);

    while (!ulp_gpio_test_finished) {
    }

    TEST_ASSERT_TRUE(ulp_gpio_test_succeeded);
}
#endif //SOC_RTCIO_PIN_COUNT > 0

#endif // SOC_RTC_TIMER_SUPPORTED

#define ISR_TEST_ITERATIONS 100
#define IO_TEST_PIN 0

TEST_CASE("LP core ISR tests", "[ulp]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_isr_bin_start, lp_core_main_isr_bin_end);

    while (!ulp_isr_test_started) {
    }

    for (int i = 0; i < ISR_TEST_ITERATIONS; i++) {
        lp_core_ll_hp_wake_lp();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    printf("ULP PMU ISR triggered %"PRIu32" times\n", ulp_pmu_isr_counter);
    TEST_ASSERT_EQUAL(ISR_TEST_ITERATIONS, ulp_pmu_isr_counter);

#if SOC_RTCIO_PIN_COUNT > 0
    /* Test LP IO interrupt */
    rtc_gpio_init(IO_TEST_PIN);
    rtc_gpio_set_direction(IO_TEST_PIN, RTC_GPIO_MODE_INPUT_ONLY);
    TEST_ASSERT_EQUAL(0, ulp_io_isr_counter);

    for (int i = 0; i < ISR_TEST_ITERATIONS; i++) {
#if CONFIG_IDF_TARGET_ESP32C6
        LP_IO.status_w1ts.val = 0x00000001; // Set GPIO 0 intr status to high
#else
        LP_GPIO.status_w1ts.val = 0x00000001; // Set GPIO 0 intr status to high
#endif
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    printf("ULP LP IO ISR triggered %"PRIu32" times\n", ulp_io_isr_counter);
    TEST_ASSERT_EQUAL(ISR_TEST_ITERATIONS, ulp_io_isr_counter);
#endif //SOC_RTCIO_PIN_COUNT > 0
}

#if SOC_DEEP_SLEEP_SUPPORTED

void lp_core_prep_exception_wakeup(void)
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_exception_bin_start, lp_core_main_exception_bin_end);

    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    /* Setup test data */

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
}

static void check_reset_reason_ulp_trap_wakeup(void)
{
    printf("Wakeup cause: 0x%"PRIx32"\n", esp_sleep_get_wakeup_causes());
    TEST_ASSERT(esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG));
}

TEST_CASE_MULTIPLE_STAGES("LP-core exception can wakeup main cpu", "[ulp]",
                          lp_core_prep_exception_wakeup,
                          check_reset_reason_ulp_trap_wakeup);
#endif //SOC_DEEP_SLEEP_SUPPORTED
