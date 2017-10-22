#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "test_utils.h"

TEST_CASE("esp_timer orders timers correctly", "[esp_timer]")
{
    void dummy_cb(void* arg)
    {
    }

    uint64_t timeouts[] = { 10000, 1000, 10000, 5000, 20000, 1000 };
    size_t indices[] = { 3, 0, 4, 2, 5, 1 };
    const size_t num_timers = sizeof(timeouts)/sizeof(timeouts[0]);
    esp_timer_handle_t handles[num_timers];
    char* names[num_timers];
    for (size_t i = 0; i < num_timers; ++i) {
        asprintf(&names[i], "timer%d", i);
        esp_timer_create_args_t args = {
                .callback = &dummy_cb,
                .name = names[i]
        };
        TEST_ESP_OK(esp_timer_create(&args, &handles[i]));
        TEST_ESP_OK(esp_timer_start_once(handles[i], timeouts[i] * 100));
    }
    char* stream_str[1024];
    FILE* stream = fmemopen(stream_str, sizeof(stream_str), "r+");
    TEST_ESP_OK(esp_timer_dump(stream));
    for (size_t i = 0; i < num_timers; ++i) {
        TEST_ESP_OK(esp_timer_stop(handles[i]));
        TEST_ESP_OK(esp_timer_delete(handles[i]));
        free(names[i]);
    }
    fflush(stream);
    fseek(stream, 0, SEEK_SET);
    for (size_t i = 0; i < num_timers; ++i) {
        char line[128];
        TEST_ASSERT_NOT_NULL(fgets(line, sizeof(line), stream));
#if WITH_PROFILING
        int timer_id;
        sscanf(line, "timer%d", &timer_id);
        TEST_ASSERT_EQUAL(indices[timer_id], i);
#else
        intptr_t timer_ptr;
        sscanf(line, "timer@0x%x", &timer_ptr);
        for (size_t j = 0; j < num_timers; ++j) {
            if (indices[j] == i) {
                TEST_ASSERT_EQUAL_PTR(handles[j], timer_ptr);
                break;
            }
        }
#endif

    }
    fclose(stream);
}


TEST_CASE("esp_timer produces correct delay", "[esp_timer]")
{
    void timer_func(void* arg)
    {
        int64_t* p_end = (int64_t*) arg;
        *p_end = ref_clock_get();
    }

    int64_t t_end;
    esp_timer_handle_t timer1;
    esp_timer_create_args_t args = {
            .callback = &timer_func,
            .arg = &t_end,
            .name = "timer1"
    };
    TEST_ESP_OK(esp_timer_create(&args, &timer1));

    const int delays_ms[] = {20, 100, 200, 250};
    const size_t delays_count = sizeof(delays_ms)/sizeof(delays_ms[0]);

    ref_clock_init();
    for (size_t i = 0; i < delays_count; ++i) {
        t_end = 0;
        int64_t t_start = ref_clock_get();

        TEST_ESP_OK(esp_timer_start_once(timer1, delays_ms[i] * 1000));

        vTaskDelay(delays_ms[i] * 2 / portTICK_PERIOD_MS);
        TEST_ASSERT(t_end != 0);
        int32_t ms_diff = (t_end - t_start) / 1000;
        printf("%d %d\n", delays_ms[i], ms_diff);

        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, delays_ms[i], ms_diff);
    }
    ref_clock_deinit();

    TEST_ESP_OK( esp_timer_dump(stdout) );

    esp_timer_delete(timer1);
}

TEST_CASE("periodic esp_timer produces correct delays", "[esp_timer]")
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
        int64_t t_end = ref_clock_get();
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

    const int delay_ms = 100;
    test_args_t args = {0};
    esp_timer_handle_t timer1;
    esp_timer_create_args_t create_args = {
            .callback = &timer_func,
            .arg = &args,
            .name = "timer1",
    };
    TEST_ESP_OK(esp_timer_create(&create_args, &timer1));
    ref_clock_init();
    args.timer = timer1;
    args.t_start = ref_clock_get();
    args.done = xSemaphoreCreateBinary();
    TEST_ESP_OK(esp_timer_start_periodic(timer1, delay_ms * 1000));

    TEST_ASSERT(xSemaphoreTake(args.done, delay_ms * NUM_INTERVALS * 2));

    TEST_ASSERT_EQUAL_UINT32(NUM_INTERVALS, args.cur_interval);
    for (size_t i = 0; i < NUM_INTERVALS; ++i) {
        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, (i + 1) * delay_ms, args.intervals[i]);
    }
    ref_clock_deinit();
    TEST_ESP_OK( esp_timer_dump(stdout) );

    TEST_ESP_OK( esp_timer_delete(timer1) );
    vSemaphoreDelete(args.done);
#undef NUM_INTERVALS
}

TEST_CASE("multiple timers are ordered correctly", "[esp_timer]")
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
        esp_timer_handle_t timer;
        test_common_t* common;
        bool pass;
        SemaphoreHandle_t done;
        int64_t t_start;
    } test_args_t;

    void timer_func(void* arg)
    {
        test_args_t* p_args = (test_args_t*) arg;
        // check order
        size_t count = p_args->common->count;
        int expected_index = p_args->common->order[count];
        int ms_since_start = (ref_clock_get() - p_args->t_start) / 1000;
        printf("Time %dms, at count %d, expected timer %d, got timer %d\n",
                ms_since_start, count, expected_index, p_args->timer_index);
        if (expected_index != p_args->timer_index) {
            p_args->pass = false;
            esp_timer_stop(p_args->timer);
            xSemaphoreGive(p_args->done);
            return;
        }
        p_args->common->count++;
        if (++p_args->intervals_count == N) {
            esp_timer_stop(p_args->timer);
            xSemaphoreGive(p_args->done);
            return;
        }
        int next_interval = p_args->intervals[p_args->intervals_count];
        printf("starting timer %d interval #%d, %d ms\n",
                p_args->timer_index, p_args->intervals_count, next_interval);
        esp_timer_start_once(p_args->timer, next_interval * 1000);
    }


    test_common_t common = {
        .order = {1, 2, 3, 2, 1, 3, 1, 2, 1, 3, 2, 1, 3, 3, 2},
        .count = 0
    };

    SemaphoreHandle_t done = xSemaphoreCreateCounting(3, 0);

    ref_clock_init();
    int64_t now = ref_clock_get();

    test_args_t args1 = {
            .timer_index = 1,
            .intervals = {10, 40, 20, 40, 30},
            .common = &common,
            .pass = true,
            .done = done,
            .t_start = now
    };

    test_args_t args2 = {
            .timer_index = 2,
            .intervals = {20, 20, 60, 30, 40},
            .common = &common,
            .pass = true,
            .done = done,
            .t_start = now
    };

    test_args_t args3 = {
            .timer_index = 3,
            .intervals = {30, 30, 60, 30, 10},
            .common = &common,
            .pass = true,
            .done = done,
            .t_start = now
    };


    esp_timer_create_args_t create_args = {
            .callback = &timer_func,
            .arg = &args1,
            .name = "1"
    };
    TEST_ESP_OK(esp_timer_create(&create_args, &args1.timer));

    create_args.name = "2";
    create_args.arg = &args2;
    TEST_ESP_OK(esp_timer_create(&create_args, &args2.timer));

    create_args.name = "3";
    create_args.arg = &args3;
    TEST_ESP_OK(esp_timer_create(&create_args, &args3.timer));

    esp_timer_start_once(args1.timer, args1.intervals[0] * 1000);
    esp_timer_start_once(args2.timer, args2.intervals[0] * 1000);
    esp_timer_start_once(args3.timer, args3.intervals[0] * 1000);

    for (int i = 0; i < 3; ++i) {
        int result = xSemaphoreTake(done, 1000 / portTICK_PERIOD_MS);
        TEST_ASSERT_TRUE(result == pdPASS);
    }

    TEST_ASSERT_TRUE(args1.pass);
    TEST_ASSERT_TRUE(args2.pass);
    TEST_ASSERT_TRUE(args3.pass);

    ref_clock_deinit();

    TEST_ESP_OK( esp_timer_dump(stdout) );

    TEST_ESP_OK( esp_timer_delete(args1.timer) );
    TEST_ESP_OK( esp_timer_delete(args2.timer) );
    TEST_ESP_OK( esp_timer_delete(args3.timer) );

#undef N
}

/* Create two timers, start them around the same time, and search through
 * timeout delta values to reproduce the case when timeouts occur close to
 * each other, testing the "multiple timers triggered" code path in timer_process_alarm.
 */
TEST_CASE("esp_timer for very short intervals", "[esp_timer]")
{
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(2, 0);

    void timer_func(void* arg) {
        SemaphoreHandle_t done = (SemaphoreHandle_t) arg;
        xSemaphoreGive(done);
        printf(".");
    }

    esp_timer_create_args_t timer_args = {
            .callback = &timer_func,
            .arg = (void*) semaphore,
            .name = "foo"
    };

    esp_timer_handle_t timer1, timer2;
    ESP_ERROR_CHECK( esp_timer_create(&timer_args, &timer1) );
    ESP_ERROR_CHECK( esp_timer_create(&timer_args, &timer2) );
    const int timeout_ms = 10;
    for (int timeout_delta_us = -150; timeout_delta_us < 150; timeout_delta_us++) {
        printf("delta=%d", timeout_delta_us);
        ESP_ERROR_CHECK( esp_timer_start_once(timer1, timeout_ms * 1000) );
        ESP_ERROR_CHECK( esp_timer_start_once(timer2, timeout_ms * 1000 + timeout_delta_us) );
        TEST_ASSERT_EQUAL(pdPASS, xSemaphoreTake(semaphore, timeout_ms * 2));
        TEST_ASSERT_EQUAL(pdPASS, xSemaphoreTake(semaphore, timeout_ms * 2));
        printf("\n");
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_timer_stop(timer1));
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_timer_stop(timer2));
    }

    vSemaphoreDelete(semaphore);
}


TEST_CASE("esp_timer_get_time call takes less than 1us", "[esp_timer]")
{
    int64_t begin = esp_timer_get_time();
    volatile int64_t end;
    const int iter_count = 10000;
    for (int i = 0; i < iter_count; ++i) {
        end = esp_timer_get_time();
    }
    int ns_per_call = (int) ((end - begin) * 1000 / iter_count);
    printf("esp_timer_get_time: %dns per call\n", ns_per_call);
    TEST_ASSERT(ns_per_call < 1000);
}

/* This test runs for about 10 minutes and is disabled in CI.
 * Such run time is needed to have FRC2 timer overflow a few times.
 */
TEST_CASE("esp_timer_get_time returns monotonic values", "[esp_timer][ignore]")
{
    void timer_test_task(void* arg) {
        int64_t delta = esp_timer_get_time() - ref_clock_get();

        const int iter_count = 1000000000;
        for (int i = 0; i < iter_count; ++i) {
            int64_t now = esp_timer_get_time();
            int64_t ref_now = ref_clock_get();
            int64_t diff = now - (ref_now + delta);
            /* Allow some difference due to rtos tick interrupting task between
             * getting 'now' and 'ref_now'.
             */
            TEST_ASSERT_INT32_WITHIN(100, 0, (int) diff);
        }

        xSemaphoreGive((SemaphoreHandle_t) arg);
        vTaskDelete(NULL);
    }
    ref_clock_init();
    SemaphoreHandle_t done_1 = xSemaphoreCreateBinary();
    SemaphoreHandle_t done_2 = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(&timer_test_task, "t1", 4096, (void*) done_1, 6, NULL, 0);
    xTaskCreatePinnedToCore(&timer_test_task, "t2", 4096, (void*) done_2, 6, NULL, 1);

    TEST_ASSERT_TRUE( xSemaphoreTake(done_1, portMAX_DELAY) );
    TEST_ASSERT_TRUE( xSemaphoreTake(done_2, portMAX_DELAY) );
    vSemaphoreDelete(done_1);
    vSemaphoreDelete(done_2);
    ref_clock_deinit();
}

TEST_CASE("Can dump esp_timer stats", "[esp_timer]")
{
    esp_timer_dump(stdout);
}
