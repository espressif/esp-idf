/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "unity.h"
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "soc/rtc.h"
#include "esp_system.h"
#include "test_utils.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_private/system_internal.h"
#include "esp_private/esp_timer_private.h"
#include "../priv_include/esp_time_impl.h"

#include "esp_private/system_internal.h"
#include "esp_private/esp_clk.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rtc.h"
#endif

#if portNUM_PROCESSORS == 2
#include "soc/rtc_cntl_reg.h"

// This runs on APP CPU:
static void time_adc_test_task(void* arg)
{
    for (int i = 0; i < 200000; ++i) {
        // wait for 20us, reading one of RTC registers
        uint32_t ccount = xthal_get_ccount();
        while (xthal_get_ccount() - ccount < 20 * CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ) {
            volatile uint32_t val = REG_READ(RTC_CNTL_STATE0_REG);
            (void) val;
        }
    }
    SemaphoreHandle_t * p_done = (SemaphoreHandle_t *) arg;
    xSemaphoreGive(*p_done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

// https://github.com/espressif/arduino-esp32/issues/120
TEST_CASE("Reading RTC registers on APP CPU doesn't affect clock", "[newlib]")
{
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
        printf("(%d) time taken: %f sec\n", i, time_sec);
        TEST_ASSERT_TRUE(fabs(time_sec - 1.0f) < 0.1);
    }
    TEST_ASSERT_TRUE(xSemaphoreTake(done, 5000 / portTICK_PERIOD_MS));
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
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec, 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_usec, 0);
    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_LESS_THAN(-800000, tv_outdelta.tv_usec);

    tv_delta.tv_sec = -4;
    tv_delta.tv_usec = -900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, NULL), 0);
    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  -4);
    TEST_ASSERT_LESS_THAN(-800000, tv_outdelta.tv_usec);

    // after settimeofday() adjtime() is stopped
    tv_delta.tv_sec = 15;
    tv_delta.tv_usec = 900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec, -4);
    TEST_ASSERT_LESS_THAN(-800000, tv_outdelta.tv_usec);
    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  15);
    TEST_ASSERT_GREATER_OR_EQUAL(800000, tv_outdelta.tv_usec);

    TEST_ASSERT_EQUAL(gettimeofday(&tv_time, NULL), 0);
    TEST_ASSERT_EQUAL(settimeofday(&tv_time, NULL), 0);

    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_usec, 0);

    // after gettimeofday() adjtime() is not stopped
    tv_delta.tv_sec = 15;
    tv_delta.tv_usec = 900000;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_usec, 0);
    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  15);
    TEST_ASSERT_GREATER_OR_EQUAL(800000, tv_outdelta.tv_usec);

    TEST_ASSERT_EQUAL(gettimeofday(&tv_time, NULL), 0);

    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec,  15);
    TEST_ASSERT_GREATER_OR_EQUAL(800000, tv_outdelta.tv_usec);

    tv_delta.tv_sec = 1;
    tv_delta.tv_usec = 0;
    TEST_ASSERT_EQUAL(adjtime(&tv_delta, NULL), 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(adjtime(NULL, &tv_outdelta), 0);
    TEST_ASSERT_EQUAL(tv_outdelta.tv_sec, 0);
    // the correction will be equal to (1_000_000us >> 6) = 15_625 us.

    TEST_ASSERT_TRUE(1000000L - tv_outdelta.tv_usec >= 15600);
    TEST_ASSERT_TRUE(1000000L - tv_outdelta.tv_usec <= 15650);
}

static volatile bool exit_flag;

static void adjtimeTask2(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
    struct timeval delta = {.tv_sec = 0, .tv_usec = 0};
    struct timeval outdelta;

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        delta.tv_sec += 1;
        delta.tv_usec = 900000;
        if (delta.tv_sec >= 2146) delta.tv_sec = 1;
        adjtime(&delta, &outdelta);
    }
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void timeTask(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
    struct timeval tv_time = { .tv_sec = 1520000000, .tv_usec = 900000 };

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        tv_time.tv_sec += 1;
        settimeofday(&tv_time, NULL);
        gettimeofday(&tv_time, NULL);
    }
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("test for no interlocking adjtime, gettimeofday and settimeofday functions", "[newlib]")
{
    TaskHandle_t th[4];
    exit_flag = false;
    struct timeval tv_time = { .tv_sec = 1520000000, .tv_usec = 900000 };
    TEST_ASSERT_EQUAL(settimeofday(&tv_time, NULL), 0);

    const int max_tasks = 2;
    SemaphoreHandle_t exit_sema[max_tasks];

    for (int i = 0; i < max_tasks; ++i) {
        exit_sema[i] = xSemaphoreCreateBinary();
    }

#ifndef CONFIG_FREERTOS_UNICORE
    printf("CPU0 and CPU1. Tasks run: 1 - adjtimeTask, 2 - gettimeofdayTask, 3 - settimeofdayTask \n");
    xTaskCreatePinnedToCore(adjtimeTask2, "adjtimeTask2", 2048, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, &th[0], 0);
    xTaskCreatePinnedToCore(timeTask, "timeTask", 2048, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, &th[1], 1);
#else
    printf("Only one CPU. Tasks run: 1 - adjtimeTask, 2 - gettimeofdayTask, 3 - settimeofdayTask\n");
    xTaskCreate(adjtimeTask2, "adjtimeTask2", 2048, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, &th[0]);
    xTaskCreate(timeTask, "timeTask", 2048, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, &th[1]);
#endif

    printf("start wait for 5 seconds\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    for (int i = 0; i < max_tasks; ++i) {
        if (!xSemaphoreTake(exit_sema[i], 2000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sema not released by test task");
        }
        vSemaphoreDelete(exit_sema[i]);
    }
}

#ifndef CONFIG_FREERTOS_UNICORE
#define ADJTIME_CORRECTION_FACTOR 6

static int64_t result_adjtime_correction_us[2];

static void get_time_task(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
    struct timeval tv_time;
    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        gettimeofday(&tv_time, NULL);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
    }
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void start_measure(int64_t* sys_time, int64_t* real_time)
{
    struct timeval tv_time;
    // there shouldn't be much time between gettimeofday and esp_timer_get_time
    gettimeofday(&tv_time, NULL);
    *real_time = esp_timer_get_time();
    *sys_time = (int64_t)tv_time.tv_sec * 1000000L + tv_time.tv_usec;
}

static int64_t calc_correction(const char* tag, int64_t* sys_time, int64_t* real_time)
{
    int64_t dt_real_time_us = real_time[1] - real_time[0];
    int64_t dt_sys_time_us  = sys_time[1] - sys_time[0];
    int64_t calc_correction_us = dt_real_time_us >> ADJTIME_CORRECTION_FACTOR;
    int64_t real_correction_us = dt_sys_time_us - dt_real_time_us;
    int64_t error_us = calc_correction_us - real_correction_us;
    printf("%s: dt_real_time = %lli us, dt_sys_time = %lli us, calc_correction = %lli us, error = %lli us\n",
            tag, dt_real_time_us, dt_sys_time_us, calc_correction_us, error_us);

    TEST_ASSERT_TRUE(dt_sys_time_us > 0 && dt_real_time_us > 0);
    TEST_ASSERT_INT_WITHIN(100, 0, error_us);
    return real_correction_us;
}

static void measure_time_task(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;
    int64_t main_real_time_us[2];
    int64_t main_sys_time_us[2];
    struct timeval tv_time = {.tv_sec = 1550000000, .tv_usec = 0};
    TEST_ASSERT_EQUAL(0, settimeofday(&tv_time, NULL));
    struct timeval delta = {.tv_sec = 2000, .tv_usec = 900000};
    adjtime(&delta, NULL);
    gettimeofday(&tv_time, NULL);
    start_measure(&main_sys_time_us[0], &main_real_time_us[0]);

    {
        int64_t real_time_us[2] = { main_real_time_us[0], 0};
        int64_t sys_time_us[2] = { main_sys_time_us[0], 0};
        // although exit flag is set in another task, checking (exit_flag == false) is safe
        while (exit_flag == false) {
            vTaskDelay(2000 / portTICK_PERIOD_MS);

            start_measure(&sys_time_us[1], &real_time_us[1]);
            result_adjtime_correction_us[1] += calc_correction("measure", sys_time_us, real_time_us);

            sys_time_us[0]  = sys_time_us[1];
            real_time_us[0] = real_time_us[1];
        }
        main_sys_time_us[1] = sys_time_us[1];
        main_real_time_us[1] = real_time_us[1];
    }

    result_adjtime_correction_us[0] = calc_correction("main", main_sys_time_us, main_real_time_us);
    int64_t delta_us = result_adjtime_correction_us[0] - result_adjtime_correction_us[1];
    printf("\nresult of adjtime correction: %lli us, %lli us. delta = %lli us\n", result_adjtime_correction_us[0], result_adjtime_correction_us[1], delta_us);
    TEST_ASSERT_INT_WITHIN(100, 0, delta_us);

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("test time adjustment happens linearly", "[newlib][timeout=15]")
{
    exit_flag = false;

    SemaphoreHandle_t exit_sema[2];
    for (int i = 0; i < 2; ++i) {
        exit_sema[i] = xSemaphoreCreateBinary();
        result_adjtime_correction_us[i] = 0;
    }

    xTaskCreatePinnedToCore(get_time_task, "get_time_task", 4096, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(measure_time_task, "measure_time_task", 4096, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);

    printf("start waiting for 10 seconds\n");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;

    for (int i = 0; i < 2; ++i) {
        if (!xSemaphoreTake(exit_sema[i], 2100/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sema not released by test task");
        }
    }

    for (int i = 0; i < 2; ++i) {
        vSemaphoreDelete(exit_sema[i]);
    }
}
#endif

void test_posix_timers_clock (void)
{
#ifndef _POSIX_TIMERS
    TEST_ASSERT_MESSAGE(false, "_POSIX_TIMERS - is not defined");
#endif

#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER )
    printf("CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER    ");
#endif

#if defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )
    printf("CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER    ");
#endif

#ifdef CONFIG_RTC_CLK_SRC_EXT_CRYS
    printf("External (crystal) Frequency = %d Hz\n", rtc_clk_slow_freq_get_hz());
#else
    printf("Internal Frequency = %" PRIu32 " Hz\n", rtc_clk_slow_freq_get_hz());
#endif

    TEST_ASSERT(clock_settime(CLOCK_REALTIME, NULL) == -1);
    TEST_ASSERT(clock_gettime(CLOCK_REALTIME, NULL) == -1);
    TEST_ASSERT(clock_getres(CLOCK_REALTIME,  NULL) == -1);

    TEST_ASSERT(clock_settime(CLOCK_MONOTONIC, NULL) == -1);
    TEST_ASSERT(clock_gettime(CLOCK_MONOTONIC, NULL) == -1);
    TEST_ASSERT(clock_getres(CLOCK_MONOTONIC,  NULL) == -1);

#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER ) || defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )
    struct timeval now = {0};
    now.tv_sec  = 10L;
    now.tv_usec = 100000L;
    TEST_ASSERT(settimeofday(&now, NULL) == 0);
    TEST_ASSERT(gettimeofday(&now, NULL) == 0);

    struct timespec ts = {0};

    TEST_ASSERT(clock_settime(0xFFFFFFFF, &ts) == -1);
    TEST_ASSERT(clock_gettime(0xFFFFFFFF, &ts) == -1);
    TEST_ASSERT(clock_getres(0xFFFFFFFF,  &ts) == 0);

    TEST_ASSERT(clock_gettime(CLOCK_REALTIME, &ts) == 0);
    TEST_ASSERT(now.tv_sec == ts.tv_sec);
    TEST_ASSERT_INT_WITHIN(5000000L, ts.tv_nsec, now.tv_usec * 1000L);

    ts.tv_sec  = 20;
    ts.tv_nsec = 100000000L;
    TEST_ASSERT(clock_settime(CLOCK_REALTIME, &ts) == 0);
    TEST_ASSERT(gettimeofday(&now, NULL) == 0);
    TEST_ASSERT_EQUAL(ts.tv_sec, now.tv_sec);
    TEST_ASSERT_INT_WITHIN(5000L, ts.tv_nsec / 1000L, now.tv_usec);

    TEST_ASSERT(clock_settime(CLOCK_MONOTONIC, &ts) == -1);

    uint64_t delta_monotonic_us = 0;
#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER )

    TEST_ASSERT(clock_getres(CLOCK_REALTIME, &ts) == 0);
    TEST_ASSERT_EQUAL_INT(1000, ts.tv_nsec);
    TEST_ASSERT(clock_getres(CLOCK_MONOTONIC, &ts) == 0);
    TEST_ASSERT_EQUAL_INT(1000, ts.tv_nsec);

    TEST_ASSERT(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
    delta_monotonic_us = esp_system_get_time() - (ts.tv_sec * 1000000L + ts.tv_nsec / 1000L);
    TEST_ASSERT(delta_monotonic_us > 0 || delta_monotonic_us == 0);
    TEST_ASSERT_INT_WITHIN(5000L, 0, delta_monotonic_us);

#elif defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )

    TEST_ASSERT(clock_getres(CLOCK_REALTIME, &ts) == 0);
    TEST_ASSERT_EQUAL_INT(1000000000L / rtc_clk_slow_freq_get_hz(), ts.tv_nsec);
    TEST_ASSERT(clock_getres(CLOCK_MONOTONIC, &ts) == 0);
    TEST_ASSERT_EQUAL_INT(1000000000L / rtc_clk_slow_freq_get_hz(), ts.tv_nsec);

    TEST_ASSERT(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
    delta_monotonic_us = esp_clk_rtc_time() - (ts.tv_sec * 1000000L + ts.tv_nsec / 1000L);
    TEST_ASSERT(delta_monotonic_us > 0 || delta_monotonic_us == 0);
    TEST_ASSERT_INT_WITHIN(5000L, 0, delta_monotonic_us);

#endif // CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER

#else
    struct timespec ts = {0};
    TEST_ASSERT(clock_settime(CLOCK_REALTIME, &ts) == -1);
    TEST_ASSERT(clock_gettime(CLOCK_REALTIME, &ts) == -1);
    TEST_ASSERT(clock_getres(CLOCK_REALTIME,  &ts) == -1);

    TEST_ASSERT(clock_settime(CLOCK_MONOTONIC, &ts) == -1);
    TEST_ASSERT(clock_gettime(CLOCK_MONOTONIC, &ts) == -1);
    TEST_ASSERT(clock_getres(CLOCK_MONOTONIC,  &ts) == -1);
#endif // defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER ) || defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )
}

TEST_CASE("test posix_timers clock_... functions", "[newlib]")
{
    test_posix_timers_clock();
}

#ifndef _USE_LONG_TIME_T

static struct timeval get_time(const char *desc, char *buffer)
{
    struct timeval timestamp;
    gettimeofday(&timestamp, NULL);
    struct tm* tm_info = localtime(&timestamp.tv_sec);
    strftime(buffer, 32, "%c", tm_info);
#if !CONFIG_NEWLIB_NANO_FORMAT
    ESP_LOGI("TAG", "%s: %016llX (%s)", desc, timestamp.tv_sec, buffer);
#endif
    return timestamp;
}

TEST_CASE("test time_t wide 64 bits", "[newlib]")
{
    static char buffer[32];
    ESP_LOGI("TAG", "sizeof(time_t): %d (%d-bit)", sizeof(time_t), sizeof(time_t)*8);
    TEST_ASSERT_EQUAL(8, sizeof(time_t));

    // mktime takes current timezone into account, this test assumes it's UTC+0
    setenv("TZ", "UTC+0", 1);
    tzset();
    struct tm tm = {4, 14, 3, 19, 0, 138, 0, 0, 0};
    struct timeval timestamp = { mktime(&tm), 0 };
#if !CONFIG_NEWLIB_NANO_FORMAT
    ESP_LOGI("TAG", "timestamp: %016llX", timestamp.tv_sec);
#endif
    settimeofday(&timestamp, NULL);
    get_time("Set time", buffer);

    while (timestamp.tv_sec < 0x80000003LL) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        timestamp = get_time("Time now", buffer);
    }
    TEST_ASSERT_EQUAL_MEMORY("Tue Jan 19 03:14:11 2038", buffer, strlen(buffer));
}

TEST_CASE("test time functions wide 64 bits", "[newlib]")
{
    static char origin_buffer[32];
    char strftime_buf[64];

    // mktime takes current timezone into account, this test assumes it's UTC+0
    setenv("TZ", "UTC+0", 1);
    tzset();
    int year = 2018;
    struct tm tm = {0, 14, 3, 19, 0, year - 1900, 0, 0, 0};
    time_t t = mktime(&tm);
    while (year < 2119) {
        struct timeval timestamp = { t, 0 };
        ESP_LOGI("TAG", "year: %d", year);
        settimeofday(&timestamp, NULL);
        get_time("Time now", origin_buffer);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        t += 86400 * 366;
        struct tm timeinfo = { 0 };
        time_t now;
        time(&now);
        localtime_r(&now, &timeinfo);

        time_t t = mktime(&timeinfo);
#if !CONFIG_NEWLIB_NANO_FORMAT
        ESP_LOGI("TAG", "Test mktime(). Time: %016llX", t);
#endif
        TEST_ASSERT_EQUAL(timestamp.tv_sec, t);
        // mktime() has error in newlib-3.0.0. It fixed in newlib-3.0.0.20180720
        TEST_ASSERT_EQUAL((timestamp.tv_sec >> 32), (t >> 32));

        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI("TAG", "Test time() and localtime_r(). Time: %s", strftime_buf);
        TEST_ASSERT_EQUAL(timeinfo.tm_year, year - 1900);
        TEST_ASSERT_EQUAL_MEMORY(origin_buffer, strftime_buf, strlen(origin_buffer));

        struct tm *tm2 = localtime(&now);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", tm2);
        ESP_LOGI("TAG", "Test localtime(). Time: %s", strftime_buf);
        TEST_ASSERT_EQUAL(tm2->tm_year, year - 1900);
        TEST_ASSERT_EQUAL_MEMORY(origin_buffer, strftime_buf, strlen(origin_buffer));

        struct tm *gm = gmtime(&now);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", gm);
        ESP_LOGI("TAG", "Test gmtime(). Time: %s", strftime_buf);
        TEST_ASSERT_EQUAL_MEMORY(origin_buffer, strftime_buf, strlen(origin_buffer));

        const char* time_str1 = ctime(&now);
        ESP_LOGI("TAG", "Test ctime(). Time: %s", time_str1);
        TEST_ASSERT_EQUAL_MEMORY(origin_buffer, time_str1, strlen(origin_buffer));

        const char* time_str2 = asctime(&timeinfo);
        ESP_LOGI("TAG", "Test asctime(). Time: %s", time_str2);
        TEST_ASSERT_EQUAL_MEMORY(origin_buffer, time_str2, strlen(origin_buffer));

        printf("\n");
        ++year;
    }
}

#endif // !_USE_LONG_TIME_T

#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER ) && defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )

extern int64_t s_microseconds_offset;
static const uint64_t s_start_timestamp  = 1606838354;


static __NOINIT_ATTR uint64_t s_saved_time;
static __NOINIT_ATTR uint64_t s_time_in_reboot;

typedef enum {
    TYPE_REBOOT_ABORT = 0,
    TYPE_REBOOT_RESTART,
} type_reboot_t;

static void print_counters(void)
{
    int64_t high_res_time = esp_system_get_time();
    int64_t rtc = esp_rtc_get_time_us();
    uint64_t boot_time = esp_time_impl_get_boot_time();
    printf("\tHigh-res time %lld (us)\n", high_res_time);
    printf("\tRTC %lld (us)\n", rtc);
    printf("\tBOOT %lld (us)\n", boot_time);
    printf("\ts_microseconds_offset %lld (us)\n", s_microseconds_offset);
    printf("delta RTC - high-res time counters %lld (us)\n", rtc - high_res_time);
}

static void set_initial_condition(type_reboot_t type_reboot, int error_time)
{
    s_saved_time = 0;
    s_time_in_reboot = 0;

    print_counters();

    struct timeval tv = { .tv_sec = s_start_timestamp, .tv_usec = 0, };
    settimeofday(&tv, NULL);
    printf("set timestamp %lld (s)\n", s_start_timestamp);

    print_counters();

    int delay_s = abs(error_time) * 2;
    printf("Waiting for %d (s) ...\n", delay_s);
    vTaskDelay(delay_s * 1000 / portTICK_PERIOD_MS);

    print_counters();

    printf("High res counter increased to %d (s)\n", error_time);
    esp_timer_private_advance(error_time * 1000000ULL);

    print_counters();

    gettimeofday(&tv, NULL);
    s_saved_time = tv.tv_sec;
    printf("s_saved_time %lld (s)\n", s_saved_time);
    int dt = s_saved_time - s_start_timestamp;
    printf("delta timestamp = %d (s)\n", dt);
    TEST_ASSERT_GREATER_OR_EQUAL(error_time, dt);
    s_time_in_reboot = esp_rtc_get_time_us();

    if (type_reboot == TYPE_REBOOT_ABORT) {
        printf("Update boot time based on diff\n");
        esp_sync_timekeeping_timers();
        print_counters();
        printf("reboot as abort\n");
        abort();
    } else if (type_reboot == TYPE_REBOOT_RESTART) {
        printf("reboot as restart\n");
        esp_restart();
    }
}

static void set_timestamp1(void)
{
    set_initial_condition(TYPE_REBOOT_ABORT, 5);
}

static void set_timestamp2(void)
{
    set_initial_condition(TYPE_REBOOT_RESTART, 5);
}

static void set_timestamp3(void)
{
    set_initial_condition(TYPE_REBOOT_RESTART, -5);
}

static void check_time(void)
{
    print_counters();
    int latency_before_run_ut = 1 + (esp_rtc_get_time_us() - s_time_in_reboot) / 1000000;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("timestamp %jd (s)\n", (intmax_t)tv.tv_sec);
    int dt = tv.tv_sec - s_saved_time;
    printf("delta timestamp = %d (s)\n", dt);
    TEST_ASSERT_GREATER_OR_EQUAL(0, dt);
    TEST_ASSERT_LESS_OR_EQUAL(latency_before_run_ut, dt);
}


TEST_CASE_MULTIPLE_STAGES("Timestamp after abort is correct in case RTC & High-res timer have + big error", "[newlib][reset=abort,SW_CPU_RESET]", set_timestamp1, check_time);
TEST_CASE_MULTIPLE_STAGES("Timestamp after restart is correct in case RTC & High-res timer have + big error", "[newlib][reset=SW_CPU_RESET]", set_timestamp2, check_time);
TEST_CASE_MULTIPLE_STAGES("Timestamp after restart is correct in case RTC & High-res timer have - big error", "[newlib][reset=SW_CPU_RESET]", set_timestamp3, check_time);
#endif // CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER && CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
