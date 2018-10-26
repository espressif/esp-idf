#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include "unity.h"
#include "esp_pm.h"
#include "esp32/clk.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/timer.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
#include "soc/rtc_periph.h"

#define MHZ     1000000

TEST_CASE("Can dump power management lock stats", "[pm]")
{
    esp_pm_dump_locks(stdout);
}

#ifdef CONFIG_PM_ENABLE

static void switch_freq(int mhz)
{
    int xtal_freq = rtc_clk_xtal_freq_get();
    esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = mhz,
        .min_freq_mhz = MIN(mhz, xtal_freq),
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    printf("Waiting for frequency to be set to %d MHz...\n", mhz);
    while (esp_clk_cpu_freq() / MHZ != mhz) {
        vTaskDelay(pdMS_TO_TICKS(200));
        printf("Frequency is %d MHz\n", esp_clk_cpu_freq() / MHZ);
    }
}

TEST_CASE("Can switch frequency using esp_pm_configure", "[pm]")
{
    int orig_freq_mhz = esp_clk_cpu_freq() / MHZ;
    switch_freq(240);
    switch_freq(40);
    switch_freq(160);
    switch_freq(240);
    switch_freq(80);
    switch_freq(40);
    switch_freq(240);
    switch_freq(40);
    switch_freq(80);
    switch_freq(10);
    switch_freq(80);
    switch_freq(20);
    switch_freq(40);
    switch_freq(orig_freq_mhz);
}

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE

static void light_sleep_enable()
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;
    int xtal_freq = (int) rtc_clk_xtal_freq_get();

    const esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = xtal_freq,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
}

static void light_sleep_disable()
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;

    const esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = cur_freq_mhz,
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
}

TEST_CASE("Automatic light occurs when tasks are suspended", "[pm]")
{
    /* To figure out if light sleep takes place, use Timer Group timer.
     * It will stop working while in light sleep.
     */
    timer_config_t config = {
        .counter_dir = TIMER_COUNT_UP,
        .divider = 80 /* 1 us per tick */
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_start(TIMER_GROUP_0, TIMER_0);

    light_sleep_enable();

    for (int ticks_to_delay = CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP;
            ticks_to_delay < CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP * 10;
            ++ticks_to_delay) {

        /* Wait until next tick */
        vTaskDelay(1);

        /* The following delay should cause light sleep to start */
        uint64_t count_start;
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &count_start);
        vTaskDelay(ticks_to_delay);
        uint64_t count_end;
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &count_end);


        int timer_diff_us = (int) (count_end - count_start);
        const int us_per_tick = 1 * portTICK_PERIOD_MS * 1000;

        printf("%d %d\n", ticks_to_delay * us_per_tick, timer_diff_us);
        TEST_ASSERT(timer_diff_us < ticks_to_delay * us_per_tick);
    }

    light_sleep_disable();
}


TEST_CASE("Can wake up from automatic light sleep by GPIO", "[pm]")
{
    assert(CONFIG_ESP32_ULP_COPROC_RESERVE_MEM >= 16 && "this test needs ESP32_ULP_COPROC_RESERVE_MEM option set in menuconfig");

    /* Set up GPIO used to wake up RTC */
    const int ext1_wakeup_gpio = 25;
    const int ext_rtc_io = RTCIO_GPIO25_CHANNEL;
    TEST_ESP_OK(rtc_gpio_init(ext1_wakeup_gpio));
    rtc_gpio_set_direction(ext1_wakeup_gpio, RTC_GPIO_MODE_INPUT_OUTPUT);
    rtc_gpio_set_level(ext1_wakeup_gpio, 0);

   /* Enable wakeup */
    TEST_ESP_OK(esp_sleep_enable_ext1_wakeup(1ULL << ext1_wakeup_gpio, ESP_EXT1_WAKEUP_ANY_HIGH));

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
    size_t size = sizeof(ulp_code)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, ulp_code, &size));

    light_sleep_enable();

    for (int i = 0; i < 10; ++i) {
        /* Set GPIO low */
        REG_CLR_BIT(rtc_gpio_desc[ext1_wakeup_gpio].reg, rtc_gpio_desc[ext1_wakeup_gpio].hold_force);
        rtc_gpio_set_level(ext1_wakeup_gpio, 0);
        REG_SET_BIT(rtc_gpio_desc[ext1_wakeup_gpio].reg, rtc_gpio_desc[ext1_wakeup_gpio].hold_force);

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
        TEST_ASSERT_INT32_WITHIN(2 * portTICK_PERIOD_MS * 1000, delay_ms * 1000, end_hs - start_hs);
        TEST_ASSERT_INT32_WITHIN(2 * portTICK_PERIOD_MS * 1000, delay_ms * 1000, end_rtc - start_rtc);
    }
    REG_CLR_BIT(rtc_gpio_desc[ext1_wakeup_gpio].reg, rtc_gpio_desc[ext1_wakeup_gpio].hold_force);
    rtc_gpio_deinit(ext1_wakeup_gpio);

    light_sleep_disable();
}


typedef struct {
    int delay_us;
    int result;
    SemaphoreHandle_t done;
} delay_test_arg_t;

static void test_delay_task(void* p)
{
    delay_test_arg_t* arg = (delay_test_arg_t*) p;
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

    delay_test_arg_t args = {
            .done = xSemaphoreCreateBinary()
    };

    const int delays[] = { 10, 20, 50, 100, 150, 200, 250 };
    const int delays_count = sizeof(delays) / sizeof(delays[0]);

    for (int i = 0; i < delays_count; ++i) {
        int delay_ms = delays[i];
        args.delay_us = delay_ms * 1000;

        xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void*) &args, 3, NULL, 0);
        TEST_ASSERT( xSemaphoreTake(args.done, delay_ms * 10 / portTICK_PERIOD_MS) );
        printf("CPU0: %d %d\n", args.delay_us, args.result);
        TEST_ASSERT_INT32_WITHIN(1000 * portTICK_PERIOD_MS * 2, args.delay_us, args.result);

#if portNUM_PROCESSORS == 2
        xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void*) &args, 3, NULL, 1);
        TEST_ASSERT( xSemaphoreTake(args.done, delay_ms * 10 / portTICK_PERIOD_MS) );
        printf("CPU1: %d %d\n", args.delay_us, args.result);
        TEST_ASSERT_INT32_WITHIN(1000 * portTICK_PERIOD_MS * 2, args.delay_us, args.result);
#endif
    }
    vSemaphoreDelete(args.done);

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

    void timer_func(void* arg)
    {
        test_args_t* p_args = (test_args_t*) arg;
        int64_t t_end = esp_clk_rtc_time();
        int32_t ms_diff = (t_end - p_args->t_start) / 1000;
        printf("timer #%d %dms\n", p_args->cur_interval, ms_diff);
        p_args->intervals[p_args->cur_interval++] = ms_diff;
        // Deliberately make timer handler run longer.
        // We check that this doesn't affect the result.
        ets_delay_us(10*1000);
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
            .callback = &timer_func,
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
        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, (i + 1) * delay_ms, args.intervals[i]);
    }

    TEST_ESP_OK( esp_timer_dump(stdout) );

    TEST_ESP_OK( esp_timer_delete(timer1) );
    vSemaphoreDelete(args.done);

    light_sleep_disable();

#undef NUM_INTERVALS
}

#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

#endif // CONFIG_PM_ENABLE
