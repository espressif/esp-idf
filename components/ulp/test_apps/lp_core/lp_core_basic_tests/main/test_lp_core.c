/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "lp_core_test_app_isr.h"

#if SOC_LP_TIMER_SUPPORTED
#include "lp_core_test_app_set_timer_wakeup.h"
#endif

#include "lp_core_test_app_gpio.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_lp_timer_shared.h"
#include "test_shared.h"
#include "unity.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal/lp_core_ll.h"
#include "hal/rtc_io_ll.h"
#include "driver/rtc_io.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_test_app_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_test_app_bin_end");

extern const uint8_t lp_core_main_counter_bin_start[] asm("_binary_lp_core_test_app_counter_bin_start");
extern const uint8_t lp_core_main_counter_bin_end[]   asm("_binary_lp_core_test_app_counter_bin_end");

extern const uint8_t lp_core_main_set_timer_wakeup_bin_start[] asm("_binary_lp_core_test_app_set_timer_wakeup_bin_start");
extern const uint8_t lp_core_main_set_timer_wakeup_bin_end[]   asm("_binary_lp_core_test_app_set_timer_wakeup_bin_end");

extern const uint8_t lp_core_main_gpio_bin_start[] asm("_binary_lp_core_test_app_gpio_bin_start");
extern const uint8_t lp_core_main_gpio_bin_end[]   asm("_binary_lp_core_test_app_gpio_bin_end");

extern const uint8_t lp_core_main_isr_bin_start[] asm("_binary_lp_core_test_app_isr_bin_start");
extern const uint8_t lp_core_main_isr_bin_end[]   asm("_binary_lp_core_test_app_isr_bin_end");

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

#define LP_TIMER_TEST_DURATION_S        (5)
#define LP_TIMER_TEST_SLEEP_DURATION_US (20000)

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C5)
#if SOC_DEEP_SLEEP_SUPPORTED && CONFIG_RTC_FAST_CLK_SRC_RC_FAST

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
    TEST_ASSERT_EQUAL(ESP_SLEEP_WAKEUP_ULP, esp_sleep_get_wakeup_cause());
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

    TEST_ASSERT_EQUAL(ESP_SLEEP_WAKEUP_ULP, esp_sleep_get_wakeup_cause());

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

#endif //#if SOC_DEEP_SLEEP_SUPPORTED && CONFIG_RTC_FAST_CLK_SRC_RC_FAST
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C5)

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

#if SOC_LP_TIMER_SUPPORTED
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

#endif //SOC_LP_TIMER_SUPPORTED

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
