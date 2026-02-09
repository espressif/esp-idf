/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/param.h>
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "ccomp_timer.h"
#include "esp_async_memcpy.h"
#include "hal/efuse_hal.h"

#if SOC_GDMA_SUPPORTED
#include "hal/gdma_ll.h"
#endif

#define IDF_LOG_PERFORMANCE(item, value_fmt, value, ...) \
    printf("[Performance][%s]: " value_fmt "\n", item, value, ##__VA_ARGS__)

typedef struct {
    uint32_t seed;
    size_t buffer_size;
    size_t copy_size;
    uint8_t *src_buf;
    uint8_t *dst_buf;
    uint8_t *from_addr;
    uint8_t *to_addr;
    uint32_t align; // alignment required by DMA engine
    uint32_t src_offset;
    uint32_t dst_offset;
    bool src_in_psram;
    bool dst_in_psram;
    bool src_dst_same;
} memcpy_testbench_context_t;

static void async_memcpy_setup_testbench(memcpy_testbench_context_t *test_context)
{
    srand(test_context->seed);
    size_t buffer_size = test_context->buffer_size;
    size_t copy_size = buffer_size;
    uint8_t *src_buf = NULL;
    uint8_t *dst_buf = NULL;
    uint8_t *from_addr = NULL;
    uint8_t *to_addr = NULL;

    uint32_t mem_caps = test_context->src_in_psram ? MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA | MALLOC_CAP_8BIT :  MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT ;
    src_buf = heap_caps_aligned_calloc(test_context->align, 1, buffer_size, mem_caps);
    TEST_ASSERT_NOT_NULL(src_buf);
    if (test_context->src_dst_same) {
        dst_buf = src_buf;
    } else {
        mem_caps = test_context->dst_in_psram ? MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA | MALLOC_CAP_8BIT :  MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT ;
        dst_buf = heap_caps_aligned_calloc(test_context->align, 1, buffer_size, mem_caps);
        TEST_ASSERT_NOT_NULL(dst_buf);
    }

    // adding extra offset
    from_addr = src_buf + test_context->src_offset;
    to_addr = dst_buf + test_context->dst_offset;
    copy_size -= MAX(test_context->src_offset, test_context->dst_offset);

    printf("copy @%p --> @%p, %zu Bytes\r\n", from_addr, to_addr, copy_size);
    for (int i = 0; i < copy_size; i++) {
        from_addr[i] = rand() % 256;
    }

    // save context
    test_context->copy_size = copy_size;
    test_context->src_buf = src_buf;
    test_context->dst_buf = dst_buf;
    test_context->from_addr = from_addr;
    test_context->to_addr = to_addr;
}

static void async_memcpy_verify_and_clear_testbench(uint32_t copy_size, uint8_t *src_buf, uint8_t *dst_buf, uint8_t *from_addr, uint8_t *to_addr)
{
    // check if source date has been copied to destination and source data not broken
    for (int i = 0; i < copy_size; i++) {
        if (from_addr[i] != to_addr[i]) {
            printf("location[%d]:s=%d,d=%d\r\n", i, from_addr[i], to_addr[i]);
            TEST_FAIL_MESSAGE("destination data doesn't match source data");
        }
    }
    free(src_buf);
    free(dst_buf);
}

static void test_memory_copy_with_same_buffer(async_memcpy_handle_t driver, async_memcpy_config_t *config)
{
    uint8_t *sbuf = heap_caps_aligned_calloc(config->dma_burst_size, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dbuf = heap_caps_aligned_calloc(config->dma_burst_size, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(sbuf);
    TEST_ASSERT_NOT_NULL(dbuf);

    for (int j = 0; j < 20; j++) {
        TEST_ESP_OK(esp_async_memcpy(driver, dbuf, sbuf, 256, NULL, NULL));
        vTaskDelay(pdMS_TO_TICKS(10));
        for (int i = 0; i < 256; i++) {
            if (sbuf[i] != dbuf[i]) {
                printf("location[%d]:s=%d,d=%d\r\n", i, sbuf[i], dbuf[i]);
                TEST_FAIL_MESSAGE("destination data doesn't match source data");
            } else {
                sbuf[i] += 1;
            }
        }
    }
    free(sbuf);
    free(dbuf);
}

TEST_CASE("memory copy the same buffer with different content", "[async mcp]")
{
    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    async_memcpy_handle_t driver = NULL;

#if SOC_HAS(AHB_GDMA)
    printf("Testing memcpy by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&config, &driver));
    test_memory_copy_with_same_buffer(driver, &config);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
    printf("Testing memcpy by AXI GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_axi(&config, &driver));
    test_memory_copy_with_same_buffer(driver, &config);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AXI_GDMA)

#if SOC_CP_DMA_SUPPORTED
    printf("Testing memcpy by CP DMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_cpdma(&config, &driver));
    test_memory_copy_with_same_buffer(driver, &config);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_CP_DMA_SUPPORTED

#if SOC_HAS(LP_AHB_GDMA)
    printf("Testing memcpy by LP AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_lp_ahb(&config, &driver));
    test_memory_copy_with_same_buffer(driver, &config);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(LP_AHB_GDMA)
}

static bool test_async_memcpy_cb_v1(async_memcpy_handle_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args;
    BaseType_t high_task_wakeup = pdFALSE;
    xSemaphoreGiveFromISR(sem, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void test_memory_copy_blocking(async_memcpy_handle_t driver)
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    const uint32_t test_buffer_size[] = {256, 512, 1024, 2048, 4096, 5008};
    memcpy_testbench_context_t test_context = {
        .align = 16,
    };
    for (int i = 0; i < sizeof(test_buffer_size) / sizeof(test_buffer_size[0]); i++) {
        // Test different align edge
        for (int off = 0; off < 4; off++) {
            test_context.buffer_size = test_buffer_size[i];
            test_context.seed = i;
            if (!efuse_hal_flash_encryption_enabled()) {
                test_context.src_offset = off;
                test_context.dst_offset = off;
            }
            async_memcpy_setup_testbench(&test_context);

            TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.copy_size, test_async_memcpy_cb_v1, sem));
            TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(10)));
            async_memcpy_verify_and_clear_testbench(test_context.copy_size, test_context.src_buf, test_context.dst_buf,
                                                    test_context.from_addr, test_context.to_addr);
        }
    }
    vSemaphoreDelete(sem);
}

TEST_CASE("memory copy by DMA (blocking)", "[async mcp]")
{
    async_memcpy_config_t config = {
        .backlog = 1,
        .dma_burst_size = 0,
    };
    async_memcpy_handle_t driver = NULL;

#if SOC_HAS(AHB_GDMA)
    printf("Testing memcpy by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&config, &driver));
    test_memory_copy_blocking(driver);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
    printf("Testing memcpy by AXI GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_axi(&config, &driver));
    test_memory_copy_blocking(driver);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AXI_GDMA)

#if SOC_CP_DMA_SUPPORTED
    printf("Testing memcpy by CP DMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_cpdma(&config, &driver));
    test_memory_copy_blocking(driver);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_CP_DMA_SUPPORTED

#if SOC_HAS(LP_AHB_GDMA)
    printf("Testing memcpy by LP AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_lp_ahb(&config, &driver));
    test_memory_copy_blocking(driver);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(LP_AHB_GDMA)
}

[[maybe_unused]] static void test_memcpy_with_dest_addr_unaligned(async_memcpy_handle_t driver, bool src_in_psram, bool dst_in_psram)
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    const uint32_t test_buffer_size[] = {256, 512, 1024, 2048, 4096, 5012};
    memcpy_testbench_context_t test_context = {
        .align = 4,
        .src_in_psram = src_in_psram,
        .dst_in_psram = dst_in_psram,
    };
    for (int i = 0; i < sizeof(test_buffer_size) / sizeof(test_buffer_size[0]); i++) {
        // Test different alignment
        for (int off = 0; off < 4; off++) {
            test_context.buffer_size = test_buffer_size[i];
            test_context.seed = i;
            test_context.src_offset = off;
            test_context.dst_offset = off + 1;
            async_memcpy_setup_testbench(&test_context);

            TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.copy_size, test_async_memcpy_cb_v1, sem));
            TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(10)));
            async_memcpy_verify_and_clear_testbench(test_context.copy_size, test_context.src_buf, test_context.dst_buf,
                                                    test_context.from_addr, test_context.to_addr);
        }
    }
    vSemaphoreDelete(sem);
}

TEST_CASE("memory copy with dest address unaligned", "[async mcp]")
{
    [[maybe_unused]] async_memcpy_config_t driver_config = {
        .backlog = 4,
        .dma_burst_size = 32,
    };
    [[maybe_unused]] async_memcpy_handle_t driver = NULL;

    if (efuse_hal_flash_encryption_enabled()) {
        TEST_PASS_MESSAGE("Flash encryption is enabled, skip this test");
    }

#if SOC_CP_DMA_SUPPORTED
    printf("Testing memcpy by CP DMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_cpdma(&driver_config, &driver));
    test_memcpy_with_dest_addr_unaligned(driver, false, false);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_CP_DMA_SUPPORTED

#if SOC_HAS(AHB_GDMA) && !GDMA_LL_AHB_RX_BURST_NEEDS_ALIGNMENT && !CONFIG_GDMA_ENABLE_WEIGHTED_ARBITRATION
    printf("Testing memcpy by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&driver_config, &driver));
    test_memcpy_with_dest_addr_unaligned(driver, false, false);
#if GDMA_LL_GET(AHB_PSRAM_CAPABLE) && SOC_HAS(SPIRAM)
    test_memcpy_with_dest_addr_unaligned(driver, true, true);
#endif // GDMA_LL_GET(AHB_PSRAM_CAPABLE) && SOC_HAS(SPIRAM)
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA) && !CONFIG_GDMA_ENABLE_WEIGHTED_ARBITRATION
    printf("Testing memcpy by AXI GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_axi(&driver_config, &driver));
    test_memcpy_with_dest_addr_unaligned(driver, false, false);
#if GDMA_LL_GET(AXI_PSRAM_CAPABLE) && SOC_HAS(SPIRAM)
    test_memcpy_with_dest_addr_unaligned(driver, true, true);
#endif // GDMA_LL_GET(AXI_PSRAM_CAPABLE) && SOC_HAS(SPIRAM)
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AXI_GDMA)

#if SOC_HAS(LP_AHB_GDMA) && !CONFIG_GDMA_ENABLE_WEIGHTED_ARBITRATION
    printf("Testing memcpy by LP AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_lp_ahb(&driver_config, &driver));
    test_memcpy_with_dest_addr_unaligned(driver, false, false);
#if GDMA_LL_GET(LP_AHB_PSRAM_CAPABLE) && SOC_HAS(SPIRAM)
    test_memcpy_with_dest_addr_unaligned(driver, true, true);
#endif // GDMA_LL_GET(LP_AHB_PSRAM_CAPABLE) && SOC_HAS(SPIRAM)
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(LP_AHB_GDMA)
}

#define TEST_ASYNC_MEMCPY_BENCH_COUNTS 16

typedef struct {
    int perf_count;
    SemaphoreHandle_t sem;
} mcp_perf_user_context_t;

static IRAM_ATTR bool test_async_memcpy_perf_cb(async_memcpy_handle_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
{
    mcp_perf_user_context_t* user = (mcp_perf_user_context_t*)cb_args;
    BaseType_t high_task_wakeup = pdFALSE;
    user->perf_count++;
    if (user->perf_count == TEST_ASYNC_MEMCPY_BENCH_COUNTS) {
        xSemaphoreGiveFromISR(user->sem, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

static void test_memcpy_performance(async_memcpy_handle_t driver, uint32_t buffer_size, bool src_in_psram, bool dst_in_psram)
{
    int64_t elapse_us = 0;
    float throughput = 0.0;

    memcpy_testbench_context_t test_context = {
        .align = 32, // set alignment same as the burst size, to achieve the best performance
        .buffer_size = buffer_size,
        .src_in_psram = src_in_psram,
        .dst_in_psram = dst_in_psram,
    };
    async_memcpy_setup_testbench(&test_context);

    // get CPU memcpy performance
    ccomp_timer_start();
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        memcpy(test_context.to_addr, test_context.from_addr, test_context.buffer_size);
    }
    elapse_us = ccomp_timer_stop();
    throughput = (float)test_context.buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("CPU_COPY", "%.2f MB/s, dir: %s->%s", throughput, src_in_psram ? "PSRAM" : "SRAM", dst_in_psram ? "PSRAM" : "SRAM");

    // get DMA memcpy performance
    ccomp_timer_start();
    mcp_perf_user_context_t user_context = {
        .perf_count = 0,
        .sem = xSemaphoreCreateBinary()
    };
    for (int i = 0; i < TEST_ASYNC_MEMCPY_BENCH_COUNTS; i++) {
        TEST_ESP_OK(esp_async_memcpy(driver, test_context.to_addr, test_context.from_addr, test_context.copy_size, test_async_memcpy_perf_cb, &user_context));
    }
    // wait for done semaphore
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(user_context.sem, pdMS_TO_TICKS(1000)));
    elapse_us = ccomp_timer_stop();
    async_memcpy_verify_and_clear_testbench(test_context.copy_size, test_context.src_buf, test_context.dst_buf, test_context.from_addr, test_context.to_addr);
    throughput = (float)buffer_size * 1e6 * TEST_ASYNC_MEMCPY_BENCH_COUNTS / 1024 / 1024 / elapse_us;
    IDF_LOG_PERFORMANCE("DMA_COPY", "%.2f MB/s, dir: %s->%s", throughput, src_in_psram ? "PSRAM" : "SRAM", dst_in_psram ? "PSRAM" : "SRAM");

    vSemaphoreDelete(user_context.sem);
}

TEST_CASE("memory copy performance 40KB: SRAM->SRAM", "[async mcp]")
{
    async_memcpy_config_t driver_config = {
        .backlog = TEST_ASYNC_MEMCPY_BENCH_COUNTS,
        .dma_burst_size = 32,
    };
    async_memcpy_handle_t driver = NULL;

#if SOC_HAS(AHB_GDMA)
    printf("Testing memcpy by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&driver_config, &driver));
    test_memcpy_performance(driver, 40 * 1024, false, false);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
    printf("Testing memcpy by AXI GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_axi(&driver_config, &driver));
    test_memcpy_performance(driver, 40 * 1024, false, false);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(AXI_GDMA)

#if SOC_CP_DMA_SUPPORTED
    printf("Testing memcpy by CP DMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_cpdma(&driver_config, &driver));
    test_memcpy_performance(driver, 40 * 1024, false, false);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_CP_DMA_SUPPORTED

#if SOC_HAS(LP_AHB_GDMA)
    printf("Testing memcpy by LP AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_lp_ahb(&driver_config, &driver));
    test_memcpy_performance(driver, 40 * 1024, false, false);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // SOC_HAS(LP_AHB_GDMA)
}

#if SOC_SPIRAM_SUPPORTED
TEST_CASE("memory copy performance 40KB: PSRAM->PSRAM", "[async mcp]")
{
    [[maybe_unused]] async_memcpy_config_t driver_config = {
        .backlog = TEST_ASYNC_MEMCPY_BENCH_COUNTS,
        .dma_burst_size = 32,
    };
    [[maybe_unused]] async_memcpy_handle_t driver = NULL;

#if SOC_HAS(AHB_GDMA)
#if GDMA_LL_GET(AHB_PSRAM_CAPABLE)
    printf("Testing memcpy by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&driver_config, &driver));
    test_memcpy_performance(driver, 40 * 1024, true, true);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
#if GDMA_LL_GET(AXI_PSRAM_CAPABLE)
    printf("Testing memcpy by AXI GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_axi(&driver_config, &driver));
    test_memcpy_performance(driver, 40 * 1024, true, true);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif
#endif // SOC_HAS(AXI_GDMA)

#if SOC_HAS(LP_AHB_GDMA)
#if GDMA_LL_GET(LP_AHB_PSRAM_CAPABLE)
    printf("Testing memcpy by LP AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_memcpy_install_gdma_lp_ahb(&driver_config, &driver));
    test_memcpy_performance(driver, 40 * 1024, true, true);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver));
#endif // GDMA_LL_GET(LP_AHB_PSRAM_CAPABLE)
#endif // SOC_HAS(LP_AHB_GDMA)
}
#endif

#if CONFIG_GDMA_ENABLE_WEIGHTED_ARBITRATION
typedef struct {
    SemaphoreHandle_t sem;
    int64_t elapse_us;
} test_weighted_arb_context_t;
static IRAM_ATTR bool test_weighted_arb_isr_cb(async_memcpy_handle_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
{
    test_weighted_arb_context_t *ctx = (test_weighted_arb_context_t *)cb_args;
    BaseType_t high_task_wakeup = pdFALSE;
    ctx->elapse_us = ccomp_timer_get_time();
    xSemaphoreGiveFromISR(ctx->sem, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void memcpy_weighted_arb_test(async_memcpy_handle_t driver[2], size_t burst_size, uint32_t buffer_size, bool buffer_in_psram)
{
    SemaphoreHandle_t sem[2] = {xSemaphoreCreateBinary(), xSemaphoreCreateBinary()};
    int64_t elapse_us[2] = {0};
    float throughput[2] = {0.0};

    memcpy_testbench_context_t test_context = {
        .align = burst_size,
        .buffer_size = buffer_size,
        .src_dst_same = !buffer_in_psram, // if buffer is in PSRAM, no memory size limitation
        .src_in_psram = buffer_in_psram,
        .dst_in_psram = buffer_in_psram,
    };
    async_memcpy_setup_testbench(&test_context);
    test_weighted_arb_context_t ctx[2] = {
        [0] = {
            .sem = sem[0],
        },
        [1] = {
            .sem = sem[1],
        }
    };

    ccomp_timer_start();
    TEST_ESP_OK(esp_async_memcpy(driver[0], test_context.to_addr, test_context.from_addr, test_context.copy_size, test_weighted_arb_isr_cb, &ctx[0]));
    TEST_ESP_OK(esp_async_memcpy(driver[1], test_context.to_addr, test_context.from_addr, test_context.copy_size, test_weighted_arb_isr_cb, &ctx[1]));

    // get channel_1 spent time
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem[1], pdMS_TO_TICKS(1000)));
    elapse_us[1] = ctx[1].elapse_us;

    // wait for channel_0 done, keep channel_1 busy to do arbitration
    while (xSemaphoreTake(sem[0], 0) == pdFALSE) {
        TEST_ESP_OK(esp_async_memcpy(driver[1], test_context.to_addr, test_context.from_addr, test_context.copy_size, test_weighted_arb_isr_cb, &ctx[1]));
        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem[1], pdMS_TO_TICKS(1000)));
    }
    // get channel_0 spent time
    elapse_us[0] = ctx[0].elapse_us;

    ccomp_timer_stop();
    throughput[0] = (float)test_context.buffer_size * 1e6  / 1024 / 1024 / elapse_us[0];
    IDF_LOG_PERFORMANCE("DMA0_COPY", "%.2f MB/s, size: %zu Bytes", throughput[0], test_context.buffer_size);

    throughput[1] = (float)test_context.buffer_size * 1e6  / 1024 / 1024 / elapse_us[1];
    IDF_LOG_PERFORMANCE("DMA1_COPY", "%.2f MB/s, size: %zu Bytes", throughput[1], test_context.buffer_size);

    // the bandwidth of channel_1 should be at least 10 times of channel_0
    TEST_ASSERT_EQUAL(throughput[1] / throughput[0] > 10, true);

    async_memcpy_verify_and_clear_testbench(test_context.copy_size, test_context.src_buf, buffer_in_psram ? test_context.dst_buf : NULL,
                                            test_context.from_addr, test_context.to_addr);

    vSemaphoreDelete(sem[0]);
    vSemaphoreDelete(sem[1]);
}

TEST_CASE("GDMA M2M Weighted Arbitration Test SRAM->SRAM", "[GDMA][M2M][async mcp]")
{
    async_memcpy_config_t driver_config = {
        .backlog = TEST_ASYNC_MEMCPY_BENCH_COUNTS,
        .dma_burst_size = 32,
    };

    async_memcpy_handle_t driver[2] = {NULL};

#if SOC_HAS(AHB_GDMA)
    driver_config.weight = 1;
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&driver_config, &driver[0]));
    driver_config.weight = 15;
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&driver_config, &driver[1]));
    memcpy_weighted_arb_test(driver, driver_config.dma_burst_size, 200 * 1024, false);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver[0]));
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver[1]));
#endif // SOC_HAS(AHB_GDMA)
}

#if SOC_SPIRAM_SUPPORTED
TEST_CASE("GDMA M2M Weighted Arbitration Test PSRAM->PSRAM", "[GDMA][M2M][async mcp]")
{
    [[maybe_unused]] async_memcpy_config_t driver_config = {
        .backlog = TEST_ASYNC_MEMCPY_BENCH_COUNTS,
        .dma_burst_size = 32,   // PSRAM bandwidth may be not enough if burst size is 64
    };

    [[maybe_unused]] async_memcpy_handle_t driver[2] = {NULL};

#if SOC_HAS(AHB_GDMA)
#if GDMA_LL_GET(AHB_PSRAM_CAPABLE)
    driver_config.weight = 1;
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&driver_config, &driver[0]));
    driver_config.weight = 15;
    TEST_ESP_OK(esp_async_memcpy_install_gdma_ahb(&driver_config, &driver[1]));
    memcpy_weighted_arb_test(driver, driver_config.dma_burst_size, 200 * 1024, true);
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver[0]));
    TEST_ESP_OK(esp_async_memcpy_uninstall(driver[1]));
#endif
#endif // SOC_HAS(AHB_GDMA)
}
#endif // SOC_SPIRAM_SUPPORTED

#endif // CONFIG_GDMA_ENABLE_WEIGHTED_ARBITRATION
