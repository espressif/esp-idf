#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_spi_flash.h"

TEST_CASE("ets_timer produces correct delay", "[ets_timer]")
{
    void timer_func(void* arg)
    {
        struct timeval* ptv = (struct timeval*) arg;
        gettimeofday(ptv, NULL);
    }

    ETSTimer timer1 = {0};

    const int delays_ms[] = {20, 100, 200, 250};
    const size_t delays_count = sizeof(delays_ms)/sizeof(delays_ms[0]);

    for (size_t i = 0; i < delays_count; ++i) {
        struct timeval tv_end = {0};

        ets_timer_setfn(&timer1, &timer_func, &tv_end);
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);

        ets_timer_arm(&timer1, delays_ms[i], false);

        vTaskDelay(delays_ms[i] * 2 / portTICK_PERIOD_MS);
        int32_t ms_diff = (tv_end.tv_sec - tv_start.tv_sec) * 1000 +
                (tv_end.tv_usec - tv_start.tv_usec) / 1000;
        printf("%d %d\n", delays_ms[i], ms_diff);

        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, delays_ms[i], ms_diff);
    }

    ets_timer_disarm(&timer1);
}

TEST_CASE("periodic ets_timer produces correct delays", "[ets_timer]")
{
    // no, we can't make this a const size_t (§6.7.5.2)
#define NUM_INTERVALS 16

    typedef struct {
        ETSTimer* timer;
        size_t cur_interval;
        int intervals[NUM_INTERVALS];
        struct timeval tv_start;
    } test_args_t;

    void timer_func(void* arg)
    {
        test_args_t* p_args = (test_args_t*) arg;
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        int32_t ms_diff = (tv_now.tv_sec - p_args->tv_start.tv_sec) * 1000 +
                        (tv_now.tv_usec - p_args->tv_start.tv_usec) / 1000;
        printf("timer #%d %dms\n", p_args->cur_interval, ms_diff);
        p_args->intervals[p_args->cur_interval++] = ms_diff;
        // Deliberately make timer handler run longer.
        // We check that this doesn't affect the result.
        ets_delay_us(10*1000);
        if (p_args->cur_interval == NUM_INTERVALS) {
            printf("done\n");
            ets_timer_disarm(p_args->timer);
        }
    }

    const int delay_ms = 100;
    ETSTimer timer1 = {0};
    test_args_t args = {0};

    args.timer = &timer1;
    gettimeofday(&args.tv_start, NULL);
    ets_timer_setfn(&timer1, &timer_func, &args);
    ets_timer_arm(&timer1, delay_ms, true);
    vTaskDelay(delay_ms * (NUM_INTERVALS + 1));

    TEST_ASSERT_EQUAL_UINT32(NUM_INTERVALS, args.cur_interval);
    for (size_t i = 0; i < NUM_INTERVALS; ++i) {
        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, (i + 1) * delay_ms, args.intervals[i]);
    }

#undef NUM_INTERVALS
}

TEST_CASE("multiple ETSTimers are ordered correctly", "[ets_timer]")
{
#define N 5

    typedef struct {
        const int order[N * 3];
        size_t count;
    } test_common_t;

    typedef struct {
        int timer_index;
        const int intervals[N];
        size_t intervals_count;
        ETSTimer* timer;
        test_common_t* common;
        bool pass;
        SemaphoreHandle_t done;
    } test_args_t;

    void timer_func(void* arg)
    {
        test_args_t* p_args = (test_args_t*) arg;
        // check order
        size_t count = p_args->common->count;
        int expected_index = p_args->common->order[count];
        printf("At count %d, expected timer %d, got timer %d\n",
                count, expected_index, p_args->timer_index);
        if (expected_index != p_args->timer_index) {
            p_args->pass = false;
            ets_timer_disarm(p_args->timer);
            xSemaphoreGive(p_args->done);
            return;
        }
        p_args->common->count++;
        if (++p_args->intervals_count == N) {
            ets_timer_disarm(p_args->timer);
            xSemaphoreGive(p_args->done);
            return;
        }
        int next_interval = p_args->intervals[p_args->intervals_count];
        printf("timer %d interval #%d, %d ms\n",
                p_args->timer_index, p_args->intervals_count, next_interval);
        ets_timer_arm(p_args->timer, next_interval, false);
    }

    ETSTimer timer1;
    ETSTimer timer2;
    ETSTimer timer3;

    test_common_t common = {
        .order = {1, 2, 3, 2, 1, 3, 1, 2, 1, 3, 2, 1, 3, 3, 2},
        .count = 0
    };

    SemaphoreHandle_t done = xSemaphoreCreateCounting(3, 0);

    test_args_t args1 = {
            .timer_index = 1,
            .intervals = {10, 40, 20, 40, 30},
            .timer = &timer1,
            .common = &common,
            .pass = true,
            .done = done
    };

    test_args_t args2 = {
            .timer_index = 2,
            .intervals = {20, 20, 60, 30, 40},
            .timer = &timer2,
            .common = &common,
            .pass = true,
            .done = done
    };

    test_args_t args3 = {
            .timer_index = 3,
            .intervals = {30, 30, 60, 30, 10},
            .timer = &timer3,
            .common = &common,
            .pass = true,
            .done = done
    };

    ets_timer_setfn(&timer1, &timer_func, &args1);
    ets_timer_setfn(&timer2, &timer_func, &args2);
    ets_timer_setfn(&timer3, &timer_func, &args3);

    ets_timer_arm(&timer1, args1.intervals[0], false);
    ets_timer_arm(&timer2, args2.intervals[0], false);
    ets_timer_arm(&timer3, args3.intervals[0], false);

    for (int i = 0; i < 3; ++i) {
        int result = xSemaphoreTake(done, 180 / portTICK_PERIOD_MS);
        TEST_ASSERT_TRUE(result == pdPASS);
    }

    TEST_ASSERT_TRUE(args1.pass);
    TEST_ASSERT_TRUE(args2.pass);
    TEST_ASSERT_TRUE(args3.pass);


#undef N
}

/* WiFi/BT coexistence will sometimes arm/disarm
   timers from an ISR where flash may be disabled. */
IRAM_ATTR TEST_CASE("ETSTimers arm & disarm run from IRAM", "[ets_timer]")
{
    void timer_func(void* arg)
    {
        volatile bool *b = (volatile bool *)arg;
        *b = true;
    }

    volatile bool flag = false;
    ETSTimer timer1;
    const int INTERVAL = 5;

    ets_timer_setfn(&timer1, &timer_func, (void *)&flag);

    /* arm a disabled timer, then disarm a live timer */

    spi_flash_guard_get()->start(); // Disables flash cache

    ets_timer_arm(&timer1, INTERVAL, false);
    // redundant call is deliberate (test code path if already armed)
    ets_timer_arm(&timer1, INTERVAL, false);
    ets_timer_disarm(&timer1);

    spi_flash_guard_get()->end(); // Re-enables flash cache

    TEST_ASSERT_FALSE(flag); // didn't expire yet

    /* do the same thing but wait for the timer to expire */

    spi_flash_guard_get()->start();
    ets_timer_arm(&timer1, INTERVAL, false);
    spi_flash_guard_get()->end();

    vTaskDelay(2 * INTERVAL / portTICK_PERIOD_MS);
    TEST_ASSERT_TRUE(flag);

    spi_flash_guard_get()->start();
    ets_timer_disarm(&timer1);
    spi_flash_guard_get()->end();
}
