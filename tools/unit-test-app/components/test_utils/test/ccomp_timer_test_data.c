#include <stdlib.h>
#include <stdint.h>

#include "esp_timer.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "ccomp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/esp_clk.h"
#include "test_utils.h"

#include "unity.h"

#include "sdkconfig.h"


#if CONFIG_IDF_TARGET_ESP32
#define CACHE_WAYS              2
#define CACHE_LINE_SIZE         32
#define CACHE_SIZE              (1 << 15)
// Only test half due to lack of memory
#define TEST_SIZE               (CACHE_SIZE / 2)
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// Default cache configuration - no override specified on
// test_utils config
#define CACHE_WAYS              8
#define CACHE_LINE_SIZE         32
#define CACHE_SIZE              (1 << 13)
#define TEST_SIZE               (CACHE_SIZE)
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C6
#define CACHE_WAYS              8
#define CACHE_LINE_SIZE         32
#define CACHE_SIZE              (1 << 14)
#define TEST_SIZE               (CACHE_SIZE)
#endif

typedef struct {
    uint8_t **accesses;
    size_t len;
} ccomp_test_access_t;

typedef struct {
    int64_t wall;
    int64_t ccomp;
} ccomp_test_time_t;

/* No performance monitor in RISCV for now
 */
#if !__riscv
//IDF-5052

static const char* TAG = "test_ccomp_timer";

#if CONFIG_SPIRAM
static uint8_t *flash_mem;
#else
static const uint8_t flash_mem[2 * CACHE_SIZE] = {0};
#endif

static IRAM_ATTR void perform_accesses(ccomp_test_access_t *access)
{
    volatile int a = 0;
    for (int i = 0; i < access->len; i++) {
        a += (int)(*(access->accesses[i]));
    }
}

static void prepare_cache(const uint8_t *to_cache)
{
    volatile int a = 0;
    for (int i = 0; i < CACHE_SIZE; i++) {
        a += to_cache[i];
    }
}

static void prepare_access_pattern(int hit_rate, const uint8_t *cached, ccomp_test_access_t *out)
{
    assert(hit_rate <= 100);
    assert(hit_rate >= 0);

    int misses = (100 - hit_rate) * CACHE_LINE_SIZE;
    int hits = hit_rate * CACHE_LINE_SIZE;

    uint8_t **accesses = calloc(TEST_SIZE, sizeof(uint8_t *));

    for (int i = 0, h = 0, i_h = 1, m = -1, i_m = 0; i < TEST_SIZE; i++, h += i_h, m += i_m) {
        if (i_m) {
            accesses[i] = (uint8_t*) (cached + CACHE_SIZE + i);
        }
        else {
            accesses[i] = (uint8_t*) (cached + i);
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

    out->accesses = accesses;
    out->len = TEST_SIZE;
}

static ccomp_test_time_t perform_test_at_hit_rate(int hit_rate, const uint8_t *mem)
{
    ccomp_test_access_t access;
    prepare_access_pattern(hit_rate, mem, &access);

    prepare_cache(mem);

    int64_t start = esp_timer_get_time();
    ccomp_timer_start();
    perform_accesses(&access);
    ccomp_test_time_t t = {
        .ccomp = ccomp_timer_stop(),
        .wall = esp_timer_get_time() - start
    };

    free(access.accesses);

    return t;
}

static ccomp_test_time_t ccomp_test_ref_time(void)
{
#if CONFIG_SPIRAM
    uint8_t *mem = heap_caps_malloc(2 * CACHE_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT);
#else
    uint8_t *mem = heap_caps_malloc(sizeof(flash_mem), MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT);
#endif
    ccomp_test_time_t t = perform_test_at_hit_rate(0, mem);
    free(mem);
    return t;
}

TEST_CASE("data cache hit rate sweep", "[test_utils][ccomp_timer]")
{
    ccomp_test_time_t t_ref;
    ccomp_test_time_t t_hr;

#if CONFIG_SPIRAM
    flash_mem = heap_caps_malloc(2 * CACHE_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
#endif

    // Perform accesses on RAM. The time recorded here serves as
    // reference.
    t_ref = ccomp_test_ref_time();

    ESP_LOGI(TAG, "Reference Time(us): %lld", (long long)t_ref.ccomp);

    // Measure time at particular hit rates
    for (int i = 0; i <= 100; i += 5)
    {
        t_hr = perform_test_at_hit_rate(i, flash_mem);
        float error = (llabs(t_ref.ccomp - t_hr.ccomp) / (float)t_ref.ccomp) * 100.0f;

        ESP_LOGI(TAG, "Hit Rate(%%): %d    Wall Time(us): %lld    Compensated Time(us): %lld    Error(%%): %f", i, (long long)t_hr.wall, (long long)t_hr.ccomp, error);

        // Check if the measured time is at least within some percent of the
        // reference.
        TEST_ASSERT(error <= 5.0f);
    }

#if CONFIG_SPIRAM
    free(flash_mem);
#endif
}
#endif // __riscv
