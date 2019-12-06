#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include "unity.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "test_utils.h"
#include "esp_private/esp_timer_impl.h"

#ifdef CONFIG_ESP_TIMER_PROFILING
#define WITH_PROFILING 1
#endif

extern uint32_t esp_timer_impl_get_overflow_val(void);
extern void esp_timer_impl_set_overflow_val(uint32_t overflow_val);

static uint32_t s_old_overflow_val;

static void setup_overflow(void)
{
    s_old_overflow_val = esp_timer_impl_get_overflow_val();
    /* Overflow every 0.1 sec.
     * Chosen so that it is 0 modulo s_timer_ticks_per_us (which is 80),
     * to prevent roundoff error on each overflow.
     */
    esp_timer_impl_set_overflow_val(8000000);
}

static void teardown_overflow(void)
{
    esp_timer_impl_set_overflow_val(s_old_overflow_val);
}

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
    setup_overflow();
    for (size_t i = 0; i < num_timers; ++i) {
        asprintf(&names[i], "timer%d", i);
        esp_timer_create_args_t args = {
                .callback = &dummy_cb,
                .name = names[i]
        };
        TEST_ESP_OK(esp_timer_create(&args, &handles[i]));
        TEST_ESP_OK(esp_timer_start_once(handles[i], timeouts[i] * 100));
    }
    teardown_overflow();
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

TEST_CASE("esp_timer_impl_set_alarm stress test", "[esp_timer]")
{
    const int test_time_sec = 10;

    void set_alarm_task(void* arg)
    {
        SemaphoreHandle_t done = (SemaphoreHandle_t) arg;

        uint64_t start = esp_timer_impl_get_time();
        uint64_t now = start;
        int count = 0;
        const int delays[] = {50, 5000, 10000000};
        const int delays_count = sizeof(delays)/sizeof(delays[0]);
        while (now - start < test_time_sec * 1000000) {
            now = esp_timer_impl_get_time();
            esp_timer_impl_set_alarm(now + delays[count % delays_count]);
            ++count;
        }
        xSemaphoreGive(done);
        vTaskDelete(NULL);
    }

    SemaphoreHandle_t done = xSemaphoreCreateCounting(portNUM_PROCESSORS, 0);
    setup_overflow();
    xTaskCreatePinnedToCore(&set_alarm_task, "set_alarm_0", 4096, done, UNITY_FREERTOS_PRIORITY, NULL, 0);
#if portNUM_PROCESSORS == 2
    xTaskCreatePinnedToCore(&set_alarm_task, "set_alarm_1", 4096, done, UNITY_FREERTOS_PRIORITY, NULL, 1);
#endif

    TEST_ASSERT(xSemaphoreTake(done, test_time_sec * 2 * 1000 / portTICK_PERIOD_MS));
#if portNUM_PROCESSORS == 2
    TEST_ASSERT(xSemaphoreTake(done, test_time_sec * 2 * 1000 / portTICK_PERIOD_MS));
#endif
    teardown_overflow();
    vSemaphoreDelete(done);
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
    setup_overflow();
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
    teardown_overflow();
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
    setup_overflow();
    args.timer = timer1;
    args.t_start = ref_clock_get();
    args.done = xSemaphoreCreateBinary();
    TEST_ESP_OK(esp_timer_start_periodic(timer1, delay_ms * 1000));

    TEST_ASSERT(xSemaphoreTake(args.done, delay_ms * NUM_INTERVALS * 2));

    TEST_ASSERT_EQUAL_UINT32(NUM_INTERVALS, args.cur_interval);
    for (size_t i = 0; i < NUM_INTERVALS; ++i) {
        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, (i + 1) * delay_ms, args.intervals[i]);
    }
    teardown_overflow();
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
    setup_overflow();
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

    teardown_overflow();
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
    TEST_PERFORMANCE_LESS_THAN(ESP_TIMER_GET_TIME_PER_CALL, "%dns", ns_per_call);
}

static int64_t IRAM_ATTR __attribute__((noinline)) get_clock_diff(void)
{
    uint64_t hs_time = esp_timer_get_time();
    uint64_t ref_time = ref_clock_get();
    return hs_time - ref_time;
}

TEST_CASE("esp_timer_get_time returns monotonic values", "[esp_timer]")
{
    typedef struct {
        SemaphoreHandle_t done;
        bool pass;
        int test_cnt;
        int error_cnt;
        int64_t max_error;
        int64_t avg_diff;
        int64_t dummy;
    } test_state_t;

    void timer_test_task(void* arg) {
        test_state_t* state = (test_state_t*) arg;
        state->pass = true;

        /* make sure both functions are in cache */
        state->dummy = get_clock_diff();

        /* calculate the difference between the two clocks */
        portDISABLE_INTERRUPTS();
        int64_t delta = get_clock_diff();
        portENABLE_INTERRUPTS();
        int64_t start_time = ref_clock_get();
        int error_repeat_cnt = 0;
        while (ref_clock_get() - start_time < 10000000) {  /* 10 seconds */
            /* Get values of both clocks again, and check that they are close to 'delta'.
             * We don't disable interrupts here, because esp_timer_get_time doesn't lock
             * interrupts internally, so we check if it can get "broken" by a well placed
             * interrupt.
             */
            int64_t diff = get_clock_diff() - delta;
            /* Allow some difference due to rtos tick interrupting task between
             * getting 'hs_now' and 'now'.
             */
            if (abs(diff) > 100) {
                error_repeat_cnt++;
                state->error_cnt++;
            } else {
                error_repeat_cnt = 0;
            }
            if (error_repeat_cnt > 2) {
                printf("diff=%lld\n", diff);
                state->pass = false;
            }
            state->avg_diff += diff;
            state->max_error = MAX(state->max_error, abs(diff));
            state->test_cnt++;
        }
        state->avg_diff /= state->test_cnt;
        xSemaphoreGive(state->done);
        vTaskDelete(NULL);
    }

    ref_clock_init();
    setup_overflow();

    test_state_t states[portNUM_PROCESSORS] = {0};
    SemaphoreHandle_t done = xSemaphoreCreateCounting(portNUM_PROCESSORS, 0);
    for (int i = 0; i < portNUM_PROCESSORS; ++i) {
        states[i].done = done;
        xTaskCreatePinnedToCore(&timer_test_task, "test", 4096, &states[i], 6, NULL, i);
    }

    for (int i = 0; i < portNUM_PROCESSORS; ++i) {
        TEST_ASSERT_TRUE( xSemaphoreTake(done, portMAX_DELAY) );
        printf("CPU%d: %s test_cnt=%d error_cnt=%d avg_diff=%d |max_error|=%d\n",
                i, states[i].pass ? "PASS" : "FAIL",
                states[i].test_cnt, states[i].error_cnt,
                (int) states[i].avg_diff, (int) states[i].max_error);
    }

    vSemaphoreDelete(done);
    teardown_overflow();
    ref_clock_deinit();

    for (int i = 0; i < portNUM_PROCESSORS; ++i) {
        TEST_ASSERT(states[i].pass);
    }
}

TEST_CASE("Can dump esp_timer stats", "[esp_timer]")
{
    esp_timer_dump(stdout);
}

TEST_CASE("Can delete timer from callback", "[esp_timer]")
{
    typedef struct {
        SemaphoreHandle_t notify_from_timer_cb;
        esp_timer_handle_t timer;
    } test_arg_t;

    void timer_func(void* varg)
    {
        test_arg_t arg = *(test_arg_t*) varg;
        esp_timer_delete(arg.timer);
        printf("Timer %p is deleted\n", arg.timer);
        xSemaphoreGive(arg.notify_from_timer_cb);
    }

    test_arg_t args = {
            .notify_from_timer_cb = xSemaphoreCreateBinary(),
    };

    esp_timer_create_args_t timer_args = {
            .callback = &timer_func,
            .arg = &args,
            .name = "self_deleter"
    };
    esp_timer_create(&timer_args, &args.timer);
    esp_timer_start_once(args.timer, 10000);

    TEST_ASSERT_TRUE(xSemaphoreTake(args.notify_from_timer_cb, 1000 / portTICK_PERIOD_MS));
    printf("Checking heap at %p\n", args.timer);
    TEST_ASSERT_TRUE(heap_caps_check_integrity_addr((intptr_t) args.timer, true));

    vSemaphoreDelete(args.notify_from_timer_cb);
}


typedef struct {
    SemaphoreHandle_t delete_start;
    SemaphoreHandle_t delete_done;
    SemaphoreHandle_t test_done;
    esp_timer_handle_t timer;
} timer_delete_test_args_t;

static void timer_delete_task(void* arg)
{
    timer_delete_test_args_t* args = (timer_delete_test_args_t*) arg;
    xSemaphoreTake(args->delete_start, portMAX_DELAY);
    printf("Deleting the timer\n");
    esp_timer_delete(args->timer);
    printf("Timer deleted\n");
    xSemaphoreGive(args->delete_done);
    vTaskDelete(NULL);
}

static void timer_delete_test_callback(void* arg)
{
    timer_delete_test_args_t* args = (timer_delete_test_args_t*) arg;
    printf("Timer callback called\n");
    xSemaphoreGive(args->delete_start);
    xSemaphoreTake(args->delete_done, portMAX_DELAY);
    printf("Callback complete\n");
    xSemaphoreGive(args->test_done);
}

TEST_CASE("Can delete timer from a separate task, triggered from callback", "[esp_timer]")
{
    timer_delete_test_args_t args = {
        .delete_start = xSemaphoreCreateBinary(),
        .delete_done = xSemaphoreCreateBinary(),
        .test_done = xSemaphoreCreateBinary(),
    };

    esp_timer_create_args_t timer_args = {
            .callback = &timer_delete_test_callback,
            .arg = &args
    };
    esp_timer_handle_t timer;
    TEST_ESP_OK(esp_timer_create(&timer_args, &timer));
    args.timer = timer;

    xTaskCreate(timer_delete_task, "deleter", 4096, &args, 5, NULL);

    esp_timer_start_once(timer, 100);
    TEST_ASSERT(xSemaphoreTake(args.test_done, pdMS_TO_TICKS(1000)));

    vSemaphoreDelete(args.delete_done);
    vSemaphoreDelete(args.delete_start);
    vSemaphoreDelete(args.test_done);
}

TEST_CASE("esp_timer_impl_advance moves time base correctly", "[esp_timer]")
{
    ref_clock_init();
    int64_t t0 = esp_timer_get_time();
    const int64_t diff_us = 1000000;
    esp_timer_impl_advance(diff_us);
    int64_t t1 = esp_timer_get_time();
    int64_t t_delta = t1 - t0;
    printf("diff_us=%lld t1-t0=%lld\n", diff_us, t_delta);
    TEST_ASSERT_INT_WITHIN(1000, diff_us, (int) t_delta);
    ref_clock_deinit();
}


TEST_CASE("after esp_timer_impl_advance, timers run when expected", "[esp_timer]")
{
    typedef struct {
        int64_t cb_time;
    } test_state_t;

    void timer_func(void* varg) {
        test_state_t* arg = (test_state_t*) varg;
        arg->cb_time = ref_clock_get();
    }

    ref_clock_init();

    test_state_t state = { 0 };

    esp_timer_create_args_t timer_args = {
            .callback = &timer_func,
            .arg = &state
    };
    esp_timer_handle_t timer;
    TEST_ESP_OK(esp_timer_create(&timer_args, &timer));

    const int64_t interval = 10000;
    const int64_t advance = 2000;

    printf("test 1\n");
    int64_t t_start = ref_clock_get();
    esp_timer_start_once(timer, interval);
    esp_timer_impl_advance(advance);
    vTaskDelay(2 * interval / 1000 / portTICK_PERIOD_MS);

    TEST_ASSERT_INT_WITHIN(portTICK_PERIOD_MS * 1000, interval - advance, state.cb_time - t_start);

    printf("test 2\n");
    state.cb_time = 0;
    t_start = ref_clock_get();
    esp_timer_start_once(timer, interval);
    esp_timer_impl_advance(interval);
    vTaskDelay(1);

    TEST_ASSERT(state.cb_time > t_start);

    ref_clock_deinit();
}

#if !defined(CONFIG_FREERTOS_UNICORE) && defined(CONFIG_ESP32_DPORT_WORKAROUND)

#include "soc/dport_reg.h"
#include "soc/frc_timer_reg.h"
#include "esp_ipc.h"
static bool task_stop;
static bool time_jumped;

static void task_check_time(void *p)
{
    int64_t  t1 = 0, t2 = 0;
    while (task_stop == false) {

        t1 = t2;
        t2 = esp_timer_get_time();
        if (t1 > t2) {
            int64_t shift_us = t2 - t1;
            time_jumped = true;
            printf("System clock jumps back: %lli us\n", shift_us);
        }

        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

static void timer_callback(void* arg)
{

}

static void dport_task(void *pvParameters)
{
    while (task_stop == false) {
        DPORT_STALL_OTHER_CPU_START();
        ets_delay_us(3);
        DPORT_STALL_OTHER_CPU_END();
    }
    vTaskDelete(NULL);
}

TEST_CASE("esp_timer_impl_set_alarm does not set an alarm below the current time", "[esp_timer][timeout=62]")
{
    const int max_timers = 2;
    time_jumped = false;
    task_stop   = false;

    xTaskCreatePinnedToCore(task_check_time, "task_check_time", 4096, NULL, 5, NULL, 0);
    // dport_task is used here to interrupt the esp_timer_impl_set_alarm function.
    // To interrupt it we can use an interrupt with 4 or 5 levels which will run on CPU0.
    // Instead, an interrupt we use the dport workaround which has 4 interrupt level for stall CPU0.
    xTaskCreatePinnedToCore(dport_task, "dport_task", 4096, NULL, 5, NULL, 1);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &timer_callback,
    };

    esp_timer_handle_t periodic_timer[max_timers];
    printf("timers created\n");

    esp_timer_create(&periodic_timer_args, &periodic_timer[0]);
    esp_timer_start_periodic(periodic_timer[0], 9000);

    esp_timer_create(&periodic_timer_args, &periodic_timer[1]);
    esp_timer_start_periodic(periodic_timer[1], 9000);

    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
    task_stop = true;

    esp_timer_stop(periodic_timer[0]);
    esp_timer_delete(periodic_timer[0]);
    esp_timer_stop(periodic_timer[1]);
    esp_timer_delete(periodic_timer[1]);
    printf("timers deleted\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    TEST_ASSERT(time_jumped == false);
}


static esp_timer_handle_t oneshot_timer;

static void oneshot_timer_callback(void* arg)
{
    esp_timer_start_once(oneshot_timer, 5000);
}

static const esp_timer_create_args_t oneshot_timer_args = {
    .callback = &oneshot_timer_callback,
};

TEST_CASE("esp_timer_impl_set_alarm and using start_once do not lead that the System time jumps back", "[esp_timer][timeout=62]")
{
    time_jumped = false;
    task_stop   = false;

    xTaskCreatePinnedToCore(task_check_time, "task_check_time", 4096, NULL, 5, NULL, 0);
    // dport_task is used here to interrupt the esp_timer_impl_set_alarm function.
    // To interrupt it we can use an interrupt with 4 or 5 levels which will run on CPU0.
    // Instead, an interrupt we use the dport workaround which has 4 interrupt level for stall CPU0.
    xTaskCreatePinnedToCore(dport_task, "dport_task", 4096, NULL, 5, NULL, 1);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &timer_callback,
    };

    esp_timer_handle_t periodic_timer;

    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, 5000);

    esp_timer_create(&oneshot_timer_args, &oneshot_timer);
    esp_timer_start_once(oneshot_timer, 9990);
    printf("timers created\n");

    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
    task_stop = true;

    esp_timer_stop(oneshot_timer);
    esp_timer_delete(oneshot_timer);
    printf("timers deleted\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    TEST_ASSERT(time_jumped == false);
}

#endif // !defined(CONFIG_FREERTOS_UNICORE) && defined(CONFIG_ESP32_DPORT_WORKAROUND)
