#include <stdlib.h>
#include <stdint.h>

#include "esp_timer.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "ccomp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#ifndef CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif

#include "unity.h"

#ifndef CONFIG_FREERTOS_UNICORE
static void start_timer(void *param)
{
    esp_err_t *err = (esp_err_t *)param;
    *err = ccomp_timer_start();
}

static void stop_timer(void *param)
{
    int64_t *t = (int64_t *)param;
    *t = ccomp_timer_stop();
}
#endif

static void computation(void *param)
{
    int *l = (int *)param;
    for (volatile int i = 0, a = 0; i < *l; i++)
    {
        a += i;
    }
}

TEST_CASE("starting and stopping works", "[test_utils][ccomp_timer]")
{
    esp_err_t err;
    int64_t t;

    /*
   * Test on the same task
   */
    err = ccomp_timer_start();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Start an already started timer
    err = ccomp_timer_start();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, err);

    t = ccomp_timer_stop();
    TEST_ASSERT_GREATER_OR_EQUAL(0, t);

    // Stopping a non started timer
    t = ccomp_timer_stop();
    TEST_ASSERT_EQUAL(-1, t);

#ifndef CONFIG_FREERTOS_UNICORE
    /*
   * Test on different task on same core
   */
    err = ccomp_timer_start();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_ipc_call_blocking(xPortGetCoreID(), start_timer, &err);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, err);

    t = ccomp_timer_stop();
    TEST_ASSERT_GREATER_OR_EQUAL(0, t);

    esp_ipc_call_blocking(xPortGetCoreID(), stop_timer, &t);
    TEST_ASSERT_EQUAL(-1, t);

    /*
   * Timer being stopped from another task on the same core
   */
    err = ccomp_timer_start();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_ipc_call_blocking(xPortGetCoreID(), stop_timer, &t);
    TEST_ASSERT_GREATER_OR_EQUAL(0, t);

    /*
   * Test on different task on same core
   */
    err = ccomp_timer_start();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_ipc_call_blocking(xPortGetCoreID() == 0 ? 1 : 0, start_timer, &err);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    t = ccomp_timer_stop();
    TEST_ASSERT_GREATER_OR_EQUAL(0, t);

    esp_ipc_call_blocking(xPortGetCoreID() == 0 ? 1 : 0, stop_timer, &t);
    TEST_ASSERT_GREATER_OR_EQUAL(0, t);
#endif
}

TEST_CASE("getting the time works", "[test_utils][ccomp_timer]")
{
    // Get wall time and start ccomp timer
    int64_t start = esp_timer_get_time();
    ccomp_timer_start();

    int64_t t_a = ccomp_timer_get_time();

    int temp = 10000;
    computation(&temp);

    int64_t t_b = ccomp_timer_get_time();

    // Check that ccomp time after computation is more than
    // ccomp time before computation.
    TEST_ASSERT_LESS_THAN(t_b, t_a);

    // Get time diff between wall time and ccomp time
    int64_t t_1 = ccomp_timer_stop();
    int64_t t_2 = esp_timer_get_time() - start;

    // The times should at least be in the same ballpark (at least within 10%)
    float diff = (llabs(t_1 - t_2)) / ((float)t_2);
    TEST_ASSERT(diff <= 10.0f);

    // Since the timer was already stopped, test that ccomp_timer_get_time
    // returns the same time as ccomp_timer_stop
    int64_t t_c = ccomp_timer_get_time();
    TEST_ASSERT_EQUAL(t_1, t_c);
}

#ifndef CONFIG_FREERTOS_UNICORE
TEST_CASE("timers for each core counts independently", "[test_utils][ccomp_timer]")
{
    esp_err_t err;

    // Start a timer on this core
    err = ccomp_timer_start();
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Do some work on this core
    int temp = 10000;
    computation(&temp);

    // Start a timer on the other core
    esp_ipc_call_blocking(xPortGetCoreID() == 0 ? 1 : 0, start_timer, &err);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Do some work on other core (less work than this core did)
    temp = 5000;
    esp_ipc_call_blocking(xPortGetCoreID() == 0 ? 1 : 0, computation, &temp);

    // Stop timers from both cores
    int64_t t_1 = ccomp_timer_stop();
    TEST_ASSERT_GREATER_OR_EQUAL(0, t_1);

    int64_t t_2;
    esp_ipc_call_blocking(xPortGetCoreID() == 0 ? 1 : 0, stop_timer, &t_2);
    TEST_ASSERT_GREATER_OR_EQUAL(0, t_2);

    // Since this core did more work, it probably has longer measured time
    TEST_ASSERT_GREATER_THAN(t_2, t_1);
}
#endif
