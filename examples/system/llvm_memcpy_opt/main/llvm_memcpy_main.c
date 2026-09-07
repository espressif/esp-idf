/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_cpu.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"

#define COPY_SIZE 1024
#define BENCHMARK_RUNS 512

#if CONFIG_COMPILER_LLVM_MEMCPY_OPTIMIZATION
#define LLVM_MEMCPY_STATUS "enabled"
#else
#define LLVM_MEMCPY_STATUS "disabled"
#endif

void memcpy_baseline_1024(void *dst, const void *src);
void memcpy_optimized_1024(void *dst, const void *src);

typedef void (*copy_fn_t)(void *, const void *);

static _Alignas(16) uint8_t s_source[COPY_SIZE];
static _Alignas(16) uint8_t s_baseline_destination[COPY_SIZE];
static _Alignas(16) uint8_t s_optimized_destination[COPY_SIZE];
static portMUX_TYPE s_benchmark_lock = portMUX_INITIALIZER_UNLOCKED;

static IRAM_ATTR uint32_t benchmark(copy_fn_t copy, uint8_t *destination)
{
    uint32_t best = UINT32_MAX;

    for (int i = 0; i < 16; ++i) {
        copy(destination, s_source);
    }

    for (int i = 0; i < BENCHMARK_RUNS; ++i) {
        portENTER_CRITICAL(&s_benchmark_lock);
        __asm__ volatile ("" ::: "memory");
        uint32_t start = esp_cpu_get_cycle_count();
        copy(destination, s_source);
        uint32_t end = esp_cpu_get_cycle_count();
        __asm__ volatile ("" ::: "memory");
        portEXIT_CRITICAL(&s_benchmark_lock);

        uint32_t cycles = end - start;
        if (cycles < best) {
            best = cycles;
        }
    }

    return best;
}

void app_main(void)
{
    for (int i = 0; i < COPY_SIZE; ++i) {
        s_source[i] = (uint8_t)(i * 3 + 1);
    }

    memcpy_baseline_1024(s_baseline_destination, s_source);
    memcpy_optimized_1024(s_optimized_destination, s_source);
    bool result_matches = memcmp(s_baseline_destination, s_optimized_destination, COPY_SIZE) == 0
                          && memcmp(s_source, s_optimized_destination, COPY_SIZE) == 0;

    uint32_t baseline_cycles = benchmark(memcpy_baseline_1024, s_baseline_destination);
    uint32_t optimized_cycles = benchmark(memcpy_optimized_1024, s_optimized_destination);

    printf("LLVM memcpy optimization: %s\n", LLVM_MEMCPY_STATUS);
    printf("Copy: %d bytes, source/destination alignment: 16 bytes\n", COPY_SIZE);
    printf("Baseline cycles:  %" PRIu32 "\n", baseline_cycles);
    printf("Optimized cycles: %" PRIu32 "\n", optimized_cycles);
    printf("Result: %s\n", result_matches ? "PASS" : "FAIL");
}
