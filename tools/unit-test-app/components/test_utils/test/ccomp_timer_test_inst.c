#include <stdlib.h>
#include <stdint.h>

#include "esp_timer.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "ccomp_timer.h"

#include "freertos/FreeRTOS.h"

#include "unity.h"

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#define CACHE_WAYS              2
#define CACHE_LINE_SIZE         32
#define CACHE_SIZE              (1 << 15)
// Only test half due to lack of memory
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// Default cache configuration - no override specified on
// test_utils config
#define CACHE_WAYS              8
#define CACHE_LINE_SIZE         32
#define CACHE_SIZE              (1 << 13)
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
#define CACHE_WAYS              8
#define CACHE_LINE_SIZE         32
#define CACHE_SIZE              (1 << 14)
#endif

typedef void (*ccomp_test_func_t)(void);

static const char* TAG = "test_ccomp_timer";

typedef struct {
    int64_t wall;
    int64_t ccomp;
} ccomp_test_time_t;

typedef struct {
    ccomp_test_func_t *funcs;
    size_t len;
} ccomp_test_call_t;

#define FUNC()              \
    do                      \
    {                       \
        volatile int a = 0; \
        a++;                \
    } while (0);

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func1(void)
{
    FUNC();
}

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func2(void)
{
    FUNC();
}

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func3(void)
{
    FUNC();
}

#if TEMPORARY_DISABLED_FOR_TARGETS(ESP32)
__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func4(void)
{
    FUNC();
}

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func5(void)
{
    FUNC();
}

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func6(void)
{
    FUNC();
}

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func7(void)
{
    FUNC();
}

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func8(void)
{
    FUNC();
}

__aligned(CACHE_SIZE / CACHE_WAYS) static void test_func9(void)
{
    FUNC();
}
#endif

static void IRAM_ATTR iram_func(void)
{
    FUNC();
}

static void IRAM_ATTR perform_calls(ccomp_test_call_t *call)
{
    for (int i = 0; i < call->len; i++) {
        call->funcs[i]();
    }
}

static void IRAM_ATTR prepare_cache(ccomp_test_call_t *call)
{
    perform_calls(call);
}

static void IRAM_ATTR prepare_calls(int hit_rate, ccomp_test_func_t *alts, size_t alts_len, size_t len, ccomp_test_call_t *out)
{
    assert(hit_rate <= 100);
    assert(hit_rate >= 0);

    int misses = (100 - hit_rate);
    int hits = hit_rate;

    ccomp_test_func_t *funcs = calloc(len, sizeof(ccomp_test_func_t));

    for (int i = 0, h = 0, i_h = 1, m = -1, i_m = 0, l = 0; i < len; i++, h += i_h, m += i_m) {
        funcs[i] = alts[l % alts_len];

        if (i_m) {
            l++;
        }

        if (h >= hits) {
            h = -1;
            i_h = 0;

            m = 0;
            i_m = 1;
        }

        if (m >= misses) {
            m = -1;
            i_m = 0;

            h = 0;
            i_h = 1;
        }
    }

    out->funcs = funcs;
    out->len = len;
}

static ccomp_test_time_t IRAM_ATTR perform_test_at_hit_rate(int hit_rate)
{
    static portMUX_TYPE m = portMUX_INITIALIZER_UNLOCKED;
    ccomp_test_call_t calls;
    ccomp_test_func_t alts[] = {test_func1, test_func2, test_func3,
#if TEMPORARY_DISABLED_FOR_TARGETS(ESP32)
    test_func4, test_func5, test_func6, test_func7, test_func8, test_func9,
#endif
    };
    prepare_calls(hit_rate, alts, sizeof(alts)/sizeof(alts[0]), 10000, &calls);

    ccomp_test_func_t f[] = {test_func1, test_func2};
    ccomp_test_call_t cache = {
        .funcs = f,
        .len = sizeof(f) / sizeof(f[0])};

    portENTER_CRITICAL(&m);

    prepare_cache(&cache);

    int64_t start = esp_timer_get_time();
    ccomp_timer_start();
    perform_calls(&calls);
    ccomp_test_time_t t = {
        .ccomp = ccomp_timer_stop(),
        .wall = esp_timer_get_time() - start
    };

    portEXIT_CRITICAL(&m);

    free(calls.funcs);

    return t;
}

static ccomp_test_time_t ccomp_test_ref_time(void)
{
    ccomp_test_call_t calls;
    ccomp_test_func_t alts[] = {iram_func};
    prepare_calls(0, alts, 1, 10000, &calls);

    int64_t start = esp_timer_get_time();
    ccomp_timer_start();
    perform_calls(&calls);
    ccomp_test_time_t t = {
        .ccomp = ccomp_timer_stop(),
        .wall = esp_timer_get_time() - start
    };

    free(calls.funcs);

    return t;
}

TEST_CASE("instruction cache hit rate sweep test", "[test_utils][ccomp_timer]")
{
    ccomp_test_time_t t_ref;
    ccomp_test_time_t t_hr;

    // Perform accesses on RAM. The time recorded here serves as
    // reference.
    t_ref = ccomp_test_ref_time();

    ESP_LOGI(TAG, "Reference Time(us): %lld", (long long)t_ref.ccomp);

    // Measure time at particular hit rates
    for (int i = 0; i <= 100; i += 5)
    {
        t_hr = perform_test_at_hit_rate(i);
        float error = (llabs(t_ref.ccomp - t_hr.ccomp) / (float)t_ref.wall) * 100.0f;

        ESP_LOGI(TAG, "Hit Rate(%%): %d    Wall Time(us): %lld    Compensated Time(us): %lld    Error(%%): %f", i, (long long)t_hr.wall, (long long)t_hr.ccomp, error);

        // Check if the measured time is at least within some percent of the
        // reference.
        TEST_ASSERT(error <= 5.0f);
    }
}
