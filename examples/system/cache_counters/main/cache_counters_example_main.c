/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Cache access counters example.
 *
 * Runs several memory access workloads with a known cache footprint and uses
 * the esp_cache_cnt API to show how the cache responds. Which workloads run
 * depends on the target: reading flash goes through the cache on every chip,
 * internal SRAM only on chips with SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE, and
 * the PSRAM workloads need PSRAM. Where there are two cache levels, the PSRAM
 * working sets are sized against both of them; where there is one, against
 * that one cache.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_cache_cnt.h"
#include "hal/cache_hal.h"
#include "soc/soc_caps.h"

/* Read-only data, so it is placed in .rodata and read from flash through the
 * cache. Larger than the data cache of any current chip, so that every pass
 * has to fetch the lines from flash again. */
#define FLASH_BUF_SIZE  (128 * 1024)
static const uint32_t s_flash_buf[FLASH_BUF_SIZE / sizeof(uint32_t)];

/* Read one word from every cache line of the buffer, 'passes' times */
static uint32_t read_workload(const uint32_t *buf, size_t size_bytes, int passes)
{
    const size_t line_size = cache_hal_get_cache_line_size(1, CACHE_TYPE_DATA);
    const size_t words_per_line = line_size / sizeof(uint32_t);
    const volatile uint32_t *p = buf;
    uint32_t acc = 0;
    for (int pass = 0; pass < passes; pass++) {
        for (size_t i = 0; i < size_bytes / sizeof(uint32_t); i += words_per_line) {
            acc += p[i];
        }
    }
    return acc;
}

/* Counting is stopped before the results are printed, so that the printing
 * itself does not show up in the counters. */
static void run_phase(const char *name, const uint32_t *buf, size_t size, int passes)
{
    ESP_ERROR_CHECK(esp_cache_cnt_start());
    read_workload(buf, size, passes);
    ESP_ERROR_CHECK(esp_cache_cnt_stop());

    printf("Workload: %s\n", name);
    ESP_ERROR_CHECK(esp_cache_cnt_dump(NULL));
    printf("\n");
}

void app_main(void)
{
    /* Reading data out of flash always goes through the cache, on every chip. */
    assert(esp_ptr_in_drom(s_flash_buf));
    run_phase("flash rodata, 128 KB x 10 passes", s_flash_buf, FLASH_BUF_SIZE, 10);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    /* Only on chips which have a cache in front of internal memory. Elsewhere
     * the CPU reaches internal SRAM directly and this workload would produce
     * no data cache traffic at all. */
    const size_t internal_size = 8 * 1024;
    uint32_t *internal_buf = heap_caps_malloc(internal_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(internal_buf != NULL);
    memset(internal_buf, 0xA5, internal_size);
    run_phase("internal SRAM, 8 KB x 1000 passes", internal_buf, internal_size, 1000);
    free(internal_buf);
#endif

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE && CONFIG_SPIRAM
    /* One PSRAM allocation, used with three different working set sizes:
     * smaller than the L1 data cache (64 KB), between L1 and L2 size,
     * and larger than the L2 cache. */
    const size_t small_size = 48 * 1024;
    const size_t mid_size = CONFIG_CACHE_L2_CACHE_SIZE * 3 / 8;
    const size_t big_size = CONFIG_CACHE_L2_CACHE_SIZE * 4;

    uint32_t *psram_buf = heap_caps_malloc(big_size, MALLOC_CAP_SPIRAM);
    assert(psram_buf != NULL);
    memset(psram_buf, 0x5A, big_size);

    run_phase("PSRAM, 48 KB x 100 passes (fits in L1)", psram_buf, small_size, 100);
    run_phase("PSRAM, 192 KB x 20 passes (fits in L2)", psram_buf, mid_size, 20);
    run_phase("PSRAM, 2 MB x 5 passes (exceeds L2)", psram_buf, big_size, 5);
    free(psram_buf);
#elif CONFIG_SPIRAM
    /* Single cache level: one working set smaller than the cache and one
     * larger than it. */
    const size_t small_size = 8 * 1024;
    const size_t big_size = 512 * 1024;

    uint32_t *psram_buf = heap_caps_malloc(big_size, MALLOC_CAP_SPIRAM);
    assert(psram_buf != NULL);
    memset(psram_buf, 0x5A, big_size);

    run_phase("PSRAM, 8 KB x 500 passes (fits in cache)", psram_buf, small_size, 500);
    run_phase("PSRAM, 512 KB x 5 passes (exceeds cache)", psram_buf, big_size, 5);
    free(psram_buf);
#endif

    printf("Cache counters example done\n");
}
