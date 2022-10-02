/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/param.h>
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "esp_async_memcpy.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"

#define ALIGN_UP(addr, align) (((addr) + (align)-1) & ~((align)-1))
#define ALIGN_DOWN(size, align)  ((size) & ~((align) - 1))

typedef struct {
    uint32_t seed;
    size_t buffer_size;
    uint8_t *src_buf;
    uint8_t *dst_buf;
    uint8_t *from_addr;
    uint8_t *to_addr;
    uint32_t align;
    uint32_t offset;
    bool src_in_psram;
    bool dst_in_psram;
} memcpy_testbench_context_t;

static void async_memcpy_setup_testbench(memcpy_testbench_context_t *test_context)
{
    srand(test_context->seed);
    printf("allocating memory buffer...\r\n");
    size_t buffer_size = test_context->buffer_size;
    uint8_t *src_buf = NULL;
    uint8_t *dst_buf = NULL;
    uint8_t *from_addr = NULL;
    uint8_t *to_addr = NULL;
#if CONFIG_SPIRAM && SOC_GDMA_SUPPORT_PSRAM
    if (test_context->src_in_psram) {
        src_buf = heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
    } else {
        src_buf = heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    }
    if (test_context->dst_in_psram) {
        dst_buf = heap_caps_calloc(1, buffer_size, MALLOC_CAP_SPIRAM);
    } else {
        dst_buf = heap_caps_calloc(1, buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    }
#else
    src_buf = heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    dst_buf = heap_caps_calloc(1, buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
#endif
    TEST_ASSERT_NOT_NULL_MESSAGE(src_buf, "allocate source buffer failed");
    TEST_ASSERT_NOT_NULL_MESSAGE(dst_buf, "allocate destination buffer failed");
    // address alignment
    from_addr = (uint8_t *)ALIGN_UP((uint32_t)(src_buf), test_context->align);
    to_addr = (uint8_t *)ALIGN_UP((uint32_t)(dst_buf), test_context->align);
    uint8_t gap = MAX(from_addr - src_buf, to_addr - dst_buf);
    buffer_size -= gap;
    // size alignment
    buffer_size = ALIGN_DOWN(buffer_size, test_context->align);
    // adding extra offset
    from_addr += test_context->offset;
    to_addr += test_context->offset;
    buffer_size -= test_context->offset;

    printf("...size %zu Bytes, src@%p, dst@%p\r\n", buffer_size, from_addr, to_addr);
    printf("fill src buffer with random data\r\n");
    for (int i = 0; i < buffer_size; i++) {
        from_addr[i] = rand() % 256;
    }
    // return value
    test_context->buffer_size = buffer_size;
    test_context->src_buf = src_buf;
    test_context->dst_buf = dst_buf;
    test_context->from_addr = from_addr;
    test_context->to_addr = to_addr;
}

static void async_memcpy_verify_and_clear_testbench(uint32_t seed, uint32_t buffer_size, uint8_t *src_buf, uint8_t *dst_buf, uint8_t *from_addr, uint8_t *to_addr)
{
    srand(seed);
    for (int i = 0; i < buffer_size; i++) {
        // check if source date has been copied to destination and source data not broken
        TEST_ASSERT_EQUAL_MESSAGE(rand() % 256, to_addr[i], "destination data doesn't match generator data");
    }
    srand(seed);
    for (int i = 0; i < buffer_size; i++) {
        // check if source data has been copied to destination
        TEST_ASSERT_EQUAL_MESSAGE(rand() % 256, to_addr[i], "destination data doesn't match source data");
    }
    free(src_buf);
    free(dst_buf);
}

TEST_CASE("memory copy the same buffer with different content", "[async mcp]")
{
    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    config.backlog = 1;
    async_memcpy_t driver = NULL;
    TEST_ESP_OK(esp_async_memcpy_install(&config, &driver));
    uint8_t sbuf[256] = {0};
    uint8_t dbuf[256] = {0};
    for (int j = 0; j < 20; j++) {
        TEST_ESP_OK(esp_async_memcpy(driver, dbuf, sbuf, 256, NULL, NULL));
        for (int i = 0; i < 256; i++) {
            if (sbuf[i] != dbuf[i]) {
                printf("location[%d]:s=%d,d=%d\r\n", i, sbuf[i], dbuf[i]);
                TEST_FAIL_MESSAGE("destination data doesn't match source data");
            } else {
                sbuf[i] += 1;
            }
        }
    }
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
}

TEST_CASE("memory copy by DMA one by one", "[async mcp]")
{
    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    config.backlog = 4;
    async_memcpy_t driver = NULL;
    TEST_ESP_OK(esp_async_memcpy_install(&config, &driver));

    uint32_t test_buffer_len[] = {256, 512, 1024, 2048, 4096, 5011};
    memcpy_testbench_context_t test_context = {
        .align = 4,
    };

    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        // Test different align edge
        for (int off = 0; off < 4; off++) {
            test_context.buffer_size = test_buffer_len[i];
            test_context.seed = i;
            async_memcpy_setup_testbench(&test_context);
            TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.buffer_size, NULL, NULL));
            async_memcpy_verify_and_clear_testbench(test_context.seed, test_context.buffer_size, test_context.src_buf, test_context.dst_buf, test_context.from_addr, test_context.to_addr);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
}

TEST_CASE("memory copy by DMA on the fly", "[async mcp]")
{
    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    async_memcpy_t driver = NULL;
    TEST_ESP_OK(esp_async_memcpy_install(&config, &driver));

    uint32_t test_buffer_len[] = {512, 1024, 2048, 4096, 5011};
    memcpy_testbench_context_t test_context[] = {
        {.align = 4}, {.align = 4}, {.align = 4}, {.align = 4}, {.align = 4},
    };

    // Aligned case
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        test_context[i].seed = i;
        test_context[i].buffer_size = test_buffer_len[i];
        async_memcpy_setup_testbench(&test_context[i]);
    }
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        TEST_ESP_OK(esp_async_memcpy(driver, test_context[i].to_addr, test_context[i].from_addr, test_context[i].buffer_size, NULL, NULL));
    }
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        async_memcpy_verify_and_clear_testbench(i, test_context[i].buffer_size, test_context[i].src_buf, test_context[i].dst_buf, test_context[i].from_addr, test_context[i].to_addr);
    }

    // Non-aligned case
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        test_context[i].seed = i;
        test_context[i].buffer_size = test_buffer_len[i];
        test_context[i].offset = 3;
        async_memcpy_setup_testbench(&test_context[i]);
    }
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        TEST_ESP_OK(esp_async_memcpy(driver, test_context[i].to_addr, test_context[i].from_addr, test_context[i].buffer_size, NULL, NULL));
    }
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        async_memcpy_verify_and_clear_testbench(i, test_context[i].buffer_size, test_context[i].src_buf, test_context[i].dst_buf, test_context[i].from_addr, test_context[i].to_addr);
    }

    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
}

#define TEST_ASYNC_MEMCPY_BENCH_COUNTS   (16)
static int s_count = 0;

static IRAM_ATTR bool test_async_memcpy_isr_cb(async_memcpy_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args;
    BaseType_t high_task_wakeup = pdFALSE;
    s_count++;
    if (s_count == TEST_ASYNC_MEMCPY_BENCH_COUNTS) {
        xSemaphoreGiveFromISR(sem, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

static void memcpy_performance_test(uint32_t buffer_size)
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();

    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    config.backlog = (buffer_size / DMA_DESCRIPTOR_BUFFER_MAX_SIZE + 1) * TEST_ASYNC_MEMCPY_BENCH_COUNTS;
    config.sram_trans_align = 4;   // at least 4 bytes aligned for SRAM transfer
    config.psram_trans_align = 64; // at least 64 bytes aligned for PSRAM transfer
    async_memcpy_t driver = NULL;
    int64_t elapse_us = 0;
    float throughput = 0.0;
    TEST_ESP_OK(esp_async_memcpy_install(&config, &driver));

    // 1. SRAM->SRAM
    memcpy_testbench_context_t test_context = {
        .align = config.psram_trans_align,
        .buffer_size = buffer_size,
        .src_in_psram = false,
        .dst_in_psram = false,
    };
    async_memcpy_setup_testbench(&test_context);
    s_count = 0;
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.buffer_size, test_async_memcpy_isr_cb, sem));
    }
    // wait for done semaphore
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("DMA_COPY", "%.2f MB/s, dir: SRAM->SRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        memcpy(test_context.to_addr, test_context.from_addr, test_context.buffer_size);
    }
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("CPU_COPY", "%.2f MB/s, dir: SRAM->SRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    async_memcpy_verify_and_clear_testbench(test_context.seed, test_context.buffer_size, test_context.src_buf, test_context.dst_buf, test_context.from_addr, test_context.to_addr);

#if CONFIG_SPIRAM && SOC_GDMA_SUPPORT_PSRAM
    // 2. PSRAM->PSRAM
    test_context.src_in_psram = true;
    test_context.dst_in_psram = true;
    async_memcpy_setup_testbench(&test_context);
    s_count = 0;
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.buffer_size, test_async_memcpy_isr_cb, sem));
    }
    // wait for done semaphore
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("DMA_COPY", "%.2f MB/s, dir: PSRAM->PSRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        memcpy(test_context.to_addr, test_context.from_addr, test_context.buffer_size);
    }
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("CPU_COPY", "%.2f MB/s, dir: PSRAM->PSRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    async_memcpy_verify_and_clear_testbench(test_context.seed, test_context.buffer_size, test_context.src_buf, test_context.dst_buf, test_context.from_addr, test_context.to_addr);

    // 3. PSRAM->SRAM
    test_context.src_in_psram = true;
    test_context.dst_in_psram = false;
    async_memcpy_setup_testbench(&test_context);
    s_count = 0;
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.buffer_size, test_async_memcpy_isr_cb, sem));
    }
    // wait for done semaphore
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("DMA_COPY", "%.2f MB/s, dir: PSRAM->SRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        memcpy(test_context.to_addr, test_context.from_addr, test_context.buffer_size);
    }
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("CPU_COPY", "%.2f MB/s, dir: PSRAM->SRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    async_memcpy_verify_and_clear_testbench(test_context.seed, test_context.buffer_size, test_context.src_buf, test_context.dst_buf, test_context.from_addr, test_context.to_addr);

    // 4. SRAM->PSRAM
    test_context.src_in_psram = false;
    test_context.dst_in_psram = true;
    async_memcpy_setup_testbench(&test_context);
    s_count = 0;
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.buffer_size, test_async_memcpy_isr_cb, sem));
    }
    // wait for done semaphore
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("DMA_COPY", "%.2f MB/s, dir: SRAM->PSRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        memcpy(test_context.to_addr, test_context.from_addr, test_context.buffer_size);
    }
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("CPU_COPY", "%.2f MB/s, dir: SRAM->PSRAM, size: %zu Bytes", throughput, test_context.buffer_size);
    async_memcpy_verify_and_clear_testbench(test_context.seed, test_context.buffer_size, test_context.src_buf, test_context.dst_buf, test_context.from_addr, test_context.to_addr);
#endif

    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
    vSemaphoreDelete(sem);
}

TEST_CASE("memory copy performance test 40KB", "[async mcp]")
{
    memcpy_performance_test(40 * 1024);
}

TEST_CASE("memory copy performance test 4KB", "[async mcp]")
{
    memcpy_performance_test(4 * 1024);
}
