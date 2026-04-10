/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include "unity.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gptimer.h"
#include "driver/rtc_io.h"
#include "soc/rtc.h"
#include "esp_private/gptimer.h"
#include "esp_rom_sys.h"
#include "esp_private/esp_clk.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc_isr.h"
#endif

#include "sdkconfig.h"

#if CONFIG_ULP_COPROC_TYPE_FSM
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/ulp.h"
#include "soc/rtc_io_reg.h"
#include "hal/rtc_io_periph.h"
#include "soc/rtc_io_channel.h"
#include "soc/rtc_cntl_reg.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/ulp.h"
#endif
#endif //CONFIG_ULP_COPROC_TYPE_FSM

TEST_CASE("Can dump power management lock stats", "[pm]")
{
    esp_pm_dump_locks(stdout);
}

TEST_CASE("Test get PM lock statistics API", "[pm]")
{
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_stats_t init_stats[ESP_PM_LOCK_MAX], stats[ESP_PM_LOCK_MAX];

#if !CONFIG_FREERTOS_UNICORE
    // Stall other CPU to avoid another core's rtos lock interference with the test.
    esp_ipc_isr_stall_other_cpu();
    // Use TEST_PROTECT() to ensure the other CPU is released even if an assertion fails,
    // otherwise the CPU1 will dead.
    if (TEST_PROTECT()) {
#endif // !CONFIG_FREERTOS_UNICORE
        // Get initial stats
        TEST_ESP_OK(esp_pm_get_lock_stats_all(init_stats));

        // Create a few locks of different types
        esp_pm_lock_handle_t lock1, lock2, lock3;
        TEST_ESP_OK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "cpu_lock", &lock1));
        TEST_ESP_OK(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "apb_lock", &lock2));
        TEST_ESP_OK(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "sleep_lock", &lock3));

        // Check stats after creating locks
        TEST_ESP_OK(esp_pm_get_lock_stats_all(stats));
        TEST_ASSERT_EQUAL(1, stats[ESP_PM_CPU_FREQ_MAX].created - init_stats[ESP_PM_CPU_FREQ_MAX].created);
        TEST_ASSERT_EQUAL(1, stats[ESP_PM_APB_FREQ_MAX].created - init_stats[ESP_PM_APB_FREQ_MAX].created);
        TEST_ASSERT_EQUAL(1, stats[ESP_PM_NO_LIGHT_SLEEP].created - init_stats[ESP_PM_NO_LIGHT_SLEEP].created);

        // Acquire locks multiple times
        TEST_ESP_OK(esp_pm_lock_acquire(lock1));
        TEST_ESP_OK(esp_pm_lock_acquire(lock1)); // Acquire again (recursive)
        TEST_ESP_OK(esp_pm_lock_acquire(lock2));

        // Check stats after acquiring locks
        TEST_ESP_OK(esp_pm_get_lock_stats_all(stats));
        // Count total held locks (sum of all lock counts)
        TEST_ASSERT_EQUAL(2, stats[ESP_PM_CPU_FREQ_MAX].acquired - init_stats[ESP_PM_CPU_FREQ_MAX].acquired); // lock1 acquired twice
        TEST_ASSERT_EQUAL(1, stats[ESP_PM_APB_FREQ_MAX].acquired - init_stats[ESP_PM_APB_FREQ_MAX].acquired); // lock2 acquired once

        // Release locks
        TEST_ESP_OK(esp_pm_lock_release(lock1));
        TEST_ESP_OK(esp_pm_lock_release(lock1)); // Release second acquisition
        TEST_ESP_OK(esp_pm_lock_release(lock2));

        // Delete locks
        TEST_ESP_OK(esp_pm_lock_delete(lock1));
        TEST_ESP_OK(esp_pm_lock_delete(lock2));
        TEST_ESP_OK(esp_pm_lock_delete(lock3));

        // Check stats after deleting locks
        TEST_ESP_OK(esp_pm_get_lock_stats_all(stats));
        TEST_ASSERT_EQUAL(0, stats[ESP_PM_CPU_FREQ_MAX].created - init_stats[ESP_PM_CPU_FREQ_MAX].created);
        TEST_ASSERT_EQUAL(0, stats[ESP_PM_APB_FREQ_MAX].created - init_stats[ESP_PM_APB_FREQ_MAX].created);
        TEST_ASSERT_EQUAL(0, stats[ESP_PM_NO_LIGHT_SLEEP].created - init_stats[ESP_PM_NO_LIGHT_SLEEP].created);

        // Test error cases
        // NULL stats pointer
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pm_get_lock_stats_all(NULL));
#if !CONFIG_FREERTOS_UNICORE
    }
    esp_ipc_isr_release_other_cpu();
#endif // !CONFIG_FREERTOS_UNICORE
#else
    // When PM is not enabled, function should return ESP_ERR_NOT_SUPPORTED
    esp_pm_lock_stats_t stats[ESP_PM_LOCK_MAX];
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, esp_pm_get_lock_stats_all(stats));
#endif
}

TEST_CASE("Test get PM lock instance statistics API", "[pm]")
{
#ifdef CONFIG_PM_ENABLE
    // Create a lock
    esp_pm_lock_handle_t lock;
    TEST_ESP_OK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "test_lock", &lock));

    // Get initial stats for the lock
    esp_pm_lock_instance_stats_t lock_stats;
    TEST_ESP_OK(esp_pm_lock_get_stats(lock, &lock_stats));
    TEST_ASSERT_EQUAL(0, lock_stats.acquired);

#if CONFIG_PM_PROFILING
    TEST_ASSERT_EQUAL(0, lock_stats.times_taken);
    TEST_ASSERT_EQUAL(0, lock_stats.time_held);
#endif

    // Acquire the lock multiple times
    TEST_ESP_OK(esp_pm_lock_acquire(lock));
    TEST_ESP_OK(esp_pm_lock_acquire(lock));
    TEST_ESP_OK(esp_pm_lock_acquire(lock));

    // Get stats again
    TEST_ESP_OK(esp_pm_lock_get_stats(lock, &lock_stats));
    TEST_ASSERT_EQUAL(3, lock_stats.acquired);

#if CONFIG_PM_PROFILING
    TEST_ASSERT_EQUAL(1, lock_stats.times_taken);
    // The time_held should be greater than 0 if the lock is currently held
    // We can't predict the exact value, so we just check that it's non-negative
    TEST_ASSERT_GREATER_OR_EQUAL(0, lock_stats.time_held);

    // Store the time_held value for later comparison
    int32_t first_time_held = (int32_t)lock_stats.time_held;
    int64_t start_time = esp_timer_get_time();
    // Delay for a short period to increase the held time
    vTaskDelay(100 / portTICK_PERIOD_MS);
    int64_t end_time = esp_timer_get_time();
    uint32_t expected_time_held = first_time_held + (uint32_t)(end_time - start_time);

    // Get stats again to check that time_held has increased
    TEST_ESP_OK(esp_pm_lock_get_stats(lock, &lock_stats));
    TEST_ASSERT_GREATER_THAN(first_time_held, lock_stats.time_held);

    // Check that time_held is within a reasonable range using TEST_ASSERT_UINT64_WITHIN
    TEST_ASSERT_UINT32_WITHIN(10000, (uint32_t)expected_time_held, (uint32_t)lock_stats.time_held); // Allow 10ms tolerance
#endif

    // Release the lock once
    TEST_ESP_OK(esp_pm_lock_release(lock));

    // Get stats again
    TEST_ESP_OK(esp_pm_lock_get_stats(lock, &lock_stats));
    TEST_ASSERT_EQUAL(2, lock_stats.acquired);

    // Release remaining locks
    TEST_ESP_OK(esp_pm_lock_release(lock));
    TEST_ESP_OK(esp_pm_lock_release(lock));

#if CONFIG_PM_PROFILING
    // After releasing all locks, get stats to check time_held is updated correctly
    TEST_ESP_OK(esp_pm_lock_get_stats(lock, &lock_stats));
    TEST_ASSERT_EQUAL(0, lock_stats.acquired);

    // Store the time_held value after releasing all locks
    int64_t time_after_release = lock_stats.time_held;

    // Delay for a short period
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Get stats again - time_held should not change since lock is not held
    TEST_ESP_OK(esp_pm_lock_get_stats(lock, &lock_stats));
    TEST_ASSERT_EQUAL(time_after_release, lock_stats.time_held);
#endif

    // Test error cases
    // NULL handle
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pm_lock_get_stats(NULL, &lock_stats));
    // NULL stats pointer
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pm_lock_get_stats(lock, NULL));

    // Clean up
    TEST_ESP_OK(esp_pm_lock_delete(lock));
#else
    // When PM is not enabled, function should return ESP_ERR_NOT_SUPPORTED
    esp_pm_lock_handle_t lock;
    esp_pm_lock_instance_stats_t lock_stats;
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, esp_pm_lock_get_stats(lock, &lock_stats));
#endif
}

#ifdef CONFIG_PM_ENABLE

static void switch_freq(int mhz)
{
    int xtal_freq_mhz = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = mhz,
        .min_freq_mhz = MIN(mhz, xtal_freq_mhz),
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    TEST_ASSERT_EQUAL_UINT32(mhz, esp_clk_cpu_freq() / MHZ);
    printf("Frequency is %d MHz\n", esp_clk_cpu_freq() / MHZ);
}

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C61
static const int test_freqs[] = {40, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, 80, 40, 80, 10, 80, 20, 40};
#elif CONFIG_IDF_TARGET_ESP32C5
static const int test_freqs[] = {40, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, 80, 40, 80, 12, 80, 24, 40};
#elif CONFIG_IDF_TARGET_ESP32H2
static const int test_freqs[] = {32, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, 64, 48, 32, 64, 48, 8, 64, 48, 16, 32};
#elif CONFIG_IDF_TARGET_ESP32C2
static const int test_freqs[] = {CONFIG_XTAL_FREQ, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, 80, CONFIG_XTAL_FREQ, 80,
                                 CONFIG_XTAL_FREQ / 2, CONFIG_XTAL_FREQ}; // C2 xtal has 40/26MHz option
#elif CONFIG_IDF_TARGET_ESP32P4
#if CONFIG_ESP32P4_SELECTS_REV_LESS_V3
static const int test_freqs[] = {40, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, 90, 40, 90, 10, 90, 20, 40, 360, 90, 180, 90, 40};
#else
static const int test_freqs[] = {40, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, 100, 40, 100, 10, 100, 20, 40, 400, 100, 200, 100, 40};
#endif
#else
static const int test_freqs[] = {240, 40, 160, 240, 80, 40, 240, 40, 80, 10, 80, 20, 40};
#endif


TEST_CASE("Can switch frequency using esp_pm_configure", "[pm]")
{
    int orig_freq_mhz = esp_clk_cpu_freq() / MHZ;

    for (int i = 0; i < sizeof(test_freqs) / sizeof(int); i++) {
        switch_freq(test_freqs[i]);
    }

    switch_freq(orig_freq_mhz);
}

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE

static void light_sleep_enable(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;
    int xtal_freq = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = xtal_freq,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
}

static void light_sleep_disable(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = cur_freq_mhz,
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
}

TEST_CASE("Automatic light occurs when tasks are suspended", "[pm]")
{
    gptimer_handle_t gptimer = NULL;
    /* To figure out if light sleep takes place, use GPTimer
     * It will stop working while in light sleep.
     */
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, /* 1 us per tick */
    };
    TEST_ESP_OK(gptimer_new_timer(&config, &gptimer));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));
    // if GPTimer is clocked from APB, when PM is enabled, the driver will acquire the PM lock
    // causing the auto light sleep doesn't take effect
    // so we manually release the lock here
    esp_pm_lock_handle_t gptimer_pm_lock;
    TEST_ESP_OK(gptimer_get_pm_lock(gptimer, &gptimer_pm_lock));
    if (gptimer_pm_lock) {
        TEST_ESP_OK(esp_pm_lock_release(gptimer_pm_lock));
    }

    light_sleep_enable();

    for (int ticks_to_delay = CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP;
            ticks_to_delay < CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP * 10;
            ++ticks_to_delay) {

        /* Wait until next tick */
        vTaskDelay(1);

        /* The following delay should cause light sleep to start */
        uint64_t count_start;
        uint64_t count_end;
        TEST_ESP_OK(gptimer_get_raw_count(gptimer, &count_start));
        vTaskDelay(ticks_to_delay);
        TEST_ESP_OK(gptimer_get_raw_count(gptimer, &count_end));

        int timer_diff_us = (int) (count_end - count_start);
        const int us_per_tick = 1 * portTICK_PERIOD_MS * 1000;

        printf("%d %d\n", ticks_to_delay * us_per_tick, timer_diff_us);
        TEST_ASSERT(timer_diff_us <= ticks_to_delay * us_per_tick);
    }

    light_sleep_disable();
    if (gptimer_pm_lock) {
        TEST_ESP_OK(esp_pm_lock_acquire(gptimer_pm_lock));
    }
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}

#if CONFIG_ULP_COPROC_TYPE_FSM
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)

// Fix failure on ESP32 when running alone; passes when the previous test is run before this one
TEST_CASE("Can wake up from automatic light sleep by GPIO", "[pm][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 16 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    /* Set up GPIO used to wake up RTC */
    const int ext1_wakeup_gpio = 25;
    const int ext_rtc_io = RTCIO_GPIO25_CHANNEL;
    TEST_ESP_OK(rtc_gpio_init(ext1_wakeup_gpio));
    rtc_gpio_set_direction(ext1_wakeup_gpio, RTC_GPIO_MODE_INPUT_OUTPUT);
    rtc_gpio_set_level(ext1_wakeup_gpio, 0);

    /* Enable wakeup */
    TEST_ESP_OK(esp_sleep_enable_ext1_wakeup_io(1ULL << ext1_wakeup_gpio, ESP_EXT1_WAKEUP_ANY_HIGH));

    /* To simplify test environment, we'll use a ULP program to set GPIO high */
    ulp_insn_t ulp_code[] = {
        I_DELAY(65535), /* about 8ms, given 8MHz ULP clock */
        I_WR_REG_BIT(RTC_CNTL_HOLD_FORCE_REG, RTC_CNTL_PDAC1_HOLD_FORCE_S, 0),
        I_WR_REG_BIT(RTC_GPIO_OUT_REG, ext_rtc_io + RTC_GPIO_OUT_DATA_S, 1),
        I_DELAY(1000),
        I_WR_REG_BIT(RTC_GPIO_OUT_REG, ext_rtc_io + RTC_GPIO_OUT_DATA_S, 0),
        I_WR_REG_BIT(RTC_CNTL_HOLD_FORCE_REG, RTC_CNTL_PDAC1_HOLD_FORCE_S, 1),
        I_END(),
        I_HALT()
    };
    TEST_ESP_OK(ulp_set_wakeup_period(0, 1000 /* us */));
    size_t size = sizeof(ulp_code) / sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, ulp_code, &size));

    light_sleep_enable();
    int rtcio_num = rtc_io_number_get(ext1_wakeup_gpio);
    for (int i = 0; i < 10; ++i) {
        /* Set GPIO low */
        REG_CLR_BIT(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].hold_force);
        rtc_gpio_set_level(ext1_wakeup_gpio, 0);
        REG_SET_BIT(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].hold_force);

        /* Wait for the next tick */
        vTaskDelay(1);

        /* Start ULP program */
        ulp_run(0);

        const int delay_ms = 200;
        const int delay_ticks = delay_ms / portTICK_PERIOD_MS;

        int64_t start_rtc = esp_clk_rtc_time();
        int64_t start_hs = esp_timer_get_time();
        uint32_t start_tick = xTaskGetTickCount();
        /* Will enter sleep here */
        vTaskDelay(delay_ticks);
        int64_t end_rtc = esp_clk_rtc_time();
        int64_t end_hs = esp_timer_get_time();
        uint32_t end_tick = xTaskGetTickCount();

        printf("%lld %lld %u\n", end_rtc - start_rtc, end_hs - start_hs, end_tick - start_tick);

        TEST_ASSERT_INT32_WITHIN(3, delay_ticks, end_tick - start_tick);
        // Error tolerance is 2 tick + duration * 5%
        TEST_ASSERT_INT32_WITHIN((2 * portTICK_PERIOD_MS + delay_ms / 20) * 1000, delay_ms * 1000, end_hs - start_hs);
        TEST_ASSERT_INT32_WITHIN((2 * portTICK_PERIOD_MS + delay_ms / 20) * 1000, delay_ms * 1000, end_rtc - start_rtc);
    }
    REG_CLR_BIT(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].hold_force);
    rtc_gpio_deinit(ext1_wakeup_gpio);

    light_sleep_disable();
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)
#endif //CONFIG_ULP_COPROC_TYPE_FSM

typedef struct {
    int delay_us;
    int result;
    SemaphoreHandle_t done;
} delay_test_arg_t;

static void test_delay_task(void *p)
{
    delay_test_arg_t *arg = (delay_test_arg_t *) p;
    vTaskDelay(1);

    uint64_t start = esp_clk_rtc_time();
    vTaskDelay(arg->delay_us / portTICK_PERIOD_MS / 1000);
    uint64_t stop = esp_clk_rtc_time();

    arg->result = (int) (stop - start);
    xSemaphoreGive(arg->done);
    vTaskDelete(NULL);
}

TEST_CASE("vTaskDelay duration is correct with light sleep enabled", "[pm]")
{
    light_sleep_enable();
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);

    delay_test_arg_t args = {
        .done = done_sem,
    };

    const int delays[] = { 10, 20, 50, 100, 150, 200, 250 };
    const int delays_count = sizeof(delays) / sizeof(delays[0]);

    for (int i = 0; i < delays_count; ++i) {
        int delay_ms = delays[i];
        args.delay_us = delay_ms * 1000;

        xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void *) &args, 3, NULL, 0);
        TEST_ASSERT( xSemaphoreTake(done_sem, delay_ms * 10 / portTICK_PERIOD_MS) );
        printf("CPU0: %d %d\n", args.delay_us, args.result);
        // Error tolerance is 2 tick + duration * 5%
        TEST_ASSERT_INT32_WITHIN((portTICK_PERIOD_MS * 2 + args.delay_us / 20) * 1000, args.delay_us, args.result);

#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
        xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void *) &args, 3, NULL, 1);
        TEST_ASSERT( xSemaphoreTake(done_sem, delay_ms * 10 / portTICK_PERIOD_MS) );
        printf("CPU1: %d %d\n", args.delay_us, args.result);
        // Error tolerance is 2 tick + duration * 5%
        TEST_ASSERT_INT32_WITHIN((portTICK_PERIOD_MS * 2 + args.delay_us / 20) * 1000, args.delay_us, args.result);
#endif
    }
    vSemaphoreDelete(done_sem);

    light_sleep_disable();
}

/* This test is similar to the one in test_esp_timer.c, but since we can't use
 * ref_clock, this test uses RTC clock for timing. Also enables automatic
 * light sleep.
 */
TEST_CASE("esp_timer produces correct delays with light sleep", "[pm]")
{
    // no, we can't make this a const size_t (§6.7.5.2)
#define NUM_INTERVALS 16

    typedef struct {
        esp_timer_handle_t timer;
        size_t cur_interval;
        int intervals[NUM_INTERVALS];
        int64_t t_start;
        SemaphoreHandle_t done;
    } test_args_t;

    void timer_func(void *arg) {
        test_args_t *p_args = (test_args_t *) arg;
        int64_t t_end = esp_clk_rtc_time();
        int32_t ms_diff = (t_end - p_args->t_start) / 1000;
        printf("timer #%d %"PRIi32"ms\n", p_args->cur_interval, ms_diff);
        p_args->intervals[p_args->cur_interval++] = ms_diff;
        // Deliberately make timer handler run longer.
        // We check that this doesn't affect the result.
        esp_rom_delay_us(10 * 1000);
        if (p_args->cur_interval == NUM_INTERVALS) {
            printf("done\n");
            TEST_ESP_OK(esp_timer_stop(p_args->timer));
            xSemaphoreGive(p_args->done);
        }
    }

    light_sleep_enable();

    const int delay_ms = 100;
    test_args_t args = {0};
    esp_timer_handle_t timer1;
    esp_timer_create_args_t create_args = {
        .callback = timer_func,
        .arg = &args,
        .name = "timer1",
    };
    TEST_ESP_OK(esp_timer_create(&create_args, &timer1));

    args.timer = timer1;
    args.t_start = esp_clk_rtc_time();
    args.done = xSemaphoreCreateBinary();
    TEST_ESP_OK(esp_timer_start_periodic(timer1, delay_ms * 1000));

    TEST_ASSERT(xSemaphoreTake(args.done, delay_ms * NUM_INTERVALS * 2));

    TEST_ASSERT_EQUAL_UINT32(NUM_INTERVALS, args.cur_interval);
    for (size_t i = 0; i < NUM_INTERVALS; ++i) {
        // TODO: PM-214
        TEST_ASSERT_INT32_WITHIN(2 * portTICK_PERIOD_MS, (i + 1) * delay_ms, args.intervals[i]);
    }

    TEST_ESP_OK( esp_timer_dump(stdout) );

    TEST_ESP_OK( esp_timer_delete(timer1) );
    vSemaphoreDelete(args.done);

    light_sleep_disable();

#undef NUM_INTERVALS
}

static void timer_cb1(void *arg)
{
    ++*((int *) arg);
}

TEST_CASE("esp_timer with SKIP_UNHANDLED_EVENTS does not wake up CPU from sleep", "[pm]")
{
    int count_calls = 0;
    int timer_interval_ms = 50;

    const esp_timer_create_args_t timer_args = {
        .name = "timer_cb1",
        .arg  = &count_calls,
        .callback = &timer_cb1,
        .skip_unhandled_events = true,
    };
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&timer_args, &periodic_timer);
    TEST_ESP_OK(esp_timer_start_periodic(periodic_timer, timer_interval_ms * 1000));

    light_sleep_enable();

    const unsigned count_delays = 5;
    unsigned i = count_delays;
    while (i-- > 0) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    TEST_ASSERT_INT_WITHIN(1, count_delays, count_calls);

    light_sleep_disable();

    TEST_ESP_OK(esp_timer_stop(periodic_timer));
    TEST_ESP_OK(esp_timer_dump(stdout));
    TEST_ESP_OK(esp_timer_delete(periodic_timer));
}

TEST_CASE("Test USJ printing doesn't block CPU on chip wake-up", "[pm]")
{
    light_sleep_enable();
    fflush(stdout);
    fsync(fileno(stdout));
    int64_t printing_time_cost_us = 0, time_end, time_start;

    for (int i = 0; i < 20; ++i)
    {
        time_start = esp_timer_get_time();
        printf("Dummy print %02d\n", i);
        fflush(stdout);
        fsync(fileno(stdout));
        time_end = esp_timer_get_time();
        printing_time_cost_us += time_end - time_start;
        vTaskDelay(10);
    }
    int32_t avg_cost = (int32_t)(printing_time_cost_us / 20);
    printf("Average cost per print %ld\n", avg_cost);
    TEST_ASSERT_LESS_THAN(5000, avg_cost);
    light_sleep_disable();
}

#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

#endif // CONFIG_PM_ENABLE
