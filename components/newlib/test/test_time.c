#include <stdio.h>
#include <math.h>
#include "unity.h"
#include "driver/adc.h"
#include <time.h>
#include <sys/time.h>
#include "soc/rtc_cntl_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"


#if portNUM_PROCESSORS == 2

// https://github.com/espressif/arduino-esp32/issues/120
TEST_CASE("Reading RTC registers on APP CPU doesn't affect clock", "[newlib]")
{
    // This runs on APP CPU:
    void time_adc_test_task(void* arg)
    {
        for (int i = 0; i < 200000; ++i) {
            // wait for 20us, reading one of RTC registers
            uint32_t ccount = xthal_get_ccount();
            while (xthal_get_ccount() - ccount < 20 * CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ) {
                volatile uint32_t val = REG_READ(RTC_CNTL_STATE0_REG);
                (void) val;
            }
        }
        SemaphoreHandle_t * p_done = (SemaphoreHandle_t *) arg;
        xSemaphoreGive(*p_done);
        vTaskDelay(1);
        vTaskDelete(NULL);
    }

    SemaphoreHandle_t done = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(&time_adc_test_task, "time_adc", 4096, &done, 5, NULL, 1);

    // This runs on PRO CPU:
    for (int i = 0; i < 4; ++i) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        struct timeval tv_stop;
        gettimeofday(&tv_stop, NULL);
        float time_sec = tv_stop.tv_sec - tv_start.tv_sec + 1e-6f * (tv_stop.tv_usec - tv_start.tv_usec);
        printf("(0) time taken: %f sec\n", time_sec);
        TEST_ASSERT_TRUE(fabs(time_sec - 1.0f) < 0.1);
    }
    TEST_ASSERT_TRUE(xSemaphoreTake(done, 5000 / portTICK_RATE_MS));
}

#endif // portNUM_PROCESSORS == 2

TEST_CASE("test adjtime function", "[newlib]")
{
    struct timeval tv_time;
    struct timeval tv_delta;
    struct timeval tv_outdelta;

    TEST_ASSERT_EQUAL(adjtime(NULL, NULL), 0);

    tv_time.tv_sec = 5000;
    tv_time.tv_usec = 5000;
    TEST_ASSERT_EQUAL(settimeofday(&tv_time, NULL), 0);

    tv_outdelta.tv_sec = 5;
    tv_outdelta.tv_usec = 5;
    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_usec, 0);

    tv_delta.tv_sec = INT_MAX / 1000000L;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), -1);

    tv_delta.tv_sec = INT_MIN / 1000000L;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), -1);

    tv_delta.tv_sec = 0;
    tv_delta.tv_usec = -900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), 0);
    TEST_ASSERT_TRUE(tv_outdelta.tv_usec <= 0);

    tv_delta.tv_sec = 0;
    tv_delta.tv_usec = 900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), 0);
    TEST_ASSERT_TRUE(tv_outdelta.tv_usec >= 0);

    tv_delta.tv_sec = -4;
    tv_delta.tv_usec = -900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  -4);
    TEST_ASSERT_TRUE(tv_outdelta.tv_usec <= 0);

    // after settimeofday() adjtime() is stopped
    tv_delta.tv_sec = 15;
    tv_delta.tv_usec = 900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  15);
    TEST_ASSERT_TRUE(tv_outdelta.tv_usec >= 0);

    TEST_ASSERT_EQUAL(gettimeofday(&tv_time, NULL), 0);
    TEST_ASSERT_EQUAL(settimeofday(&tv_time, NULL), 0);

    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_usec, 0);

    // after gettimeofday() adjtime() is not stopped
    tv_delta.tv_sec = 15;
    tv_delta.tv_usec = 900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  15);
    TEST_ASSERT_TRUE(tv_outdelta.tv_usec >= 0);

    TEST_ASSERT_EQUAL(gettimeofday(&tv_time, NULL), 0);

    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  15);
    TEST_ASSERT_TRUE(tv_outdelta.tv_usec >= 0);

    tv_delta.tv_sec = 1;
    tv_delta.tv_usec = 0;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, NULL), 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_TRUE(tv_outdelta.tv_sec == 0);
    // the correction will be equal to (1_000_000us >> 6) = 15_625 us.
    TEST_ASSERT_TRUE(1000000L - tv_outdelta.tv_usec >= 15600);
    TEST_ASSERT_TRUE(1000000L - tv_outdelta.tv_usec <= 15650);
}

static volatile bool exit_flag;
static bool adjtime_test_result;
static bool gettimeofday_test_result;
static uint64_t count_adjtime;
static uint64_t count_settimeofday;
static uint64_t count_gettimeofday;

static void adjtimeTask2(void *pvParameters)
{
    struct timeval delta = {.tv_sec = 0, .tv_usec = 0};
    struct timeval outdelta;

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        delta.tv_sec += 1;
        delta.tv_usec = 900000;
        if (delta.tv_sec >= 2146) delta.tv_sec = 1;
        adjtime(&delta, &outdelta);
        count_adjtime++;
    }
    vTaskDelete(NULL);
}

static void settimeofdayTask2(void *pvParameters)
{
    struct timeval tv_time = { .tv_sec = 1520000000, .tv_usec = 900000 };

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        tv_time.tv_sec += 1;
        settimeofday(&tv_time, NULL);
        count_settimeofday++;
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

static void gettimeofdayTask2(void *pvParameters)
{
    struct timeval tv_time;
    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        gettimeofday(&tv_time, NULL);
        count_gettimeofday++;
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

TEST_CASE("test for no interlocking adjtime, gettimeofday and settimeofday functions", "[newlib]")
{
    TaskHandle_t th[4];
    exit_flag = false;
    count_adjtime = 0;
    count_settimeofday = 0;
    count_gettimeofday = 0;
    struct timeval tv_time = { .tv_sec = 1520000000, .tv_usec = 900000 };
    TEST_ASSERT_EQUAL(settimeofday(&tv_time, NULL), 0);

#ifndef CONFIG_FREERTOS_UNICORE
    printf("CPU0 and CPU1. Tasks run: 1 - adjtimeTask, 2 - gettimeofdayTask, 3 - settimeofdayTask \n");
    xTaskCreatePinnedToCore(adjtimeTask2, "adjtimeTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[0], 0);
    xTaskCreatePinnedToCore(gettimeofdayTask2, "gettimeofdayTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[1], 1);
    xTaskCreatePinnedToCore(settimeofdayTask2, "settimeofdayTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[2], 0);
#else
    printf("Only one CPU. Tasks run: 1 - adjtimeTask, 2 - gettimeofdayTask, 3 - settimeofdayTask\n");
    xTaskCreate(adjtimeTask2, "adjtimeTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[0]);
    xTaskCreate(gettimeofdayTask2, "gettimeofdayTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[1]);
    xTaskCreate(settimeofdayTask2, "settimeofdayTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[2]);
#endif

    printf("start wait for 10 seconds\n");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    vTaskDelay(20 / portTICK_PERIOD_MS);
    printf("count_adjtime %lld, count_settimeofday %lld, count_gettimeofday %lld\n", count_adjtime, count_settimeofday, count_gettimeofday);
    TEST_ASSERT(count_adjtime > 1000LL && count_settimeofday > 1000LL && count_gettimeofday > 1000LL);
}

static void adjtimeTask(void *pvParameters)
{
    struct timeval delta = {.tv_sec = 0, .tv_usec = 0};
    struct timeval outdelta = {.tv_sec = 0, .tv_usec = 0};

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        delta.tv_sec = 1000;
        delta.tv_usec = 0;
        if(adjtime(&delta, &outdelta) != 0) {
            adjtime_test_result = true;
            exit_flag = true;
        }
        delta.tv_sec = 0;
        delta.tv_usec = 1000;
        if(adjtime(&delta, &outdelta) != 0) {
            adjtime_test_result = true;
            exit_flag = true;
        }
    }
    vTaskDelete(NULL);
}

static void gettimeofdayTask(void *pvParameters)
{
    struct timeval tv_time;

    gettimeofday(&tv_time, NULL);
    uint64_t time_old = (uint64_t)tv_time.tv_sec * 1000000L + tv_time.tv_usec;
    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        gettimeofday(&tv_time, NULL);
        uint64_t time   = (uint64_t)tv_time.tv_sec * 1000000L + tv_time.tv_usec;
        if(((time - time_old) > 1000000LL) || (time_old > time)) {
            printf("ERROR: time jumped for %lld/1000 seconds. No locks. Need to use locks.\n", (time - time_old)/1000000LL);
            gettimeofday_test_result = true;
            exit_flag = true;
        }
        time_old = time;
    }
    vTaskDelete(NULL);
}

TEST_CASE("test for thread safety adjtime and gettimeofday functions", "[newlib]")
{
    TaskHandle_t th[4];
    exit_flag = false;
    adjtime_test_result = false;
    gettimeofday_test_result = false;

    struct timeval tv_time = { .tv_sec = 1520000000, .tv_usec = 900000 };
    TEST_ASSERT_EQUAL(settimeofday(&tv_time, NULL), 0);

#ifndef CONFIG_FREERTOS_UNICORE
    printf("CPU0 and CPU1. Tasks run: 1 - adjtimeTask, 2 - gettimeofdayTask\n");
    xTaskCreatePinnedToCore(adjtimeTask, "adjtimeTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[0], 0);
    xTaskCreatePinnedToCore(gettimeofdayTask, "gettimeofdayTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[1], 1);

    xTaskCreatePinnedToCore(adjtimeTask, "adjtimeTask2", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[2], 0);
    xTaskCreatePinnedToCore(gettimeofdayTask, "gettimeofdayTask2", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[3], 1);
#else
    printf("Only one CPU. Tasks run: 1 - adjtimeTask, 2 - gettimeofdayTask\n");
    xTaskCreate(adjtimeTask, "adjtimeTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[0]);
    xTaskCreate(gettimeofdayTask, "gettimeofdayTask1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[1]);

    xTaskCreate(adjtimeTask, "adjtimeTask2", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[2]);
    xTaskCreate(gettimeofdayTask, "gettimeofdayTask2", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, &th[3]);
#endif

    printf("start wait for 10 seconds\n");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    vTaskDelay(20 / portTICK_PERIOD_MS);

    TEST_ASSERT(adjtime_test_result == false && gettimeofday_test_result == false);
}
