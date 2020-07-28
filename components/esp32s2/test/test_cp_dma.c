#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "soc/cp_dma_caps.h"
#include "cp_dma.h"

#define ALIGN_UP(addr, align) (((addr) + (align)-1) & ~((align)-1))

static void cp_dma_setup_testbench(uint32_t seed, uint32_t *buffer_size, uint8_t **src_buf, uint8_t **dst_buf, uint8_t **from_addr, uint8_t **to_addr, uint32_t align)
{
    srand(seed);
    printf("allocating memory buffer...\r\n");
    // memory copy from/to PSRAM is not allowed
    *src_buf = heap_caps_malloc(*buffer_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    *dst_buf = heap_caps_calloc(1, *buffer_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    TEST_ASSERT_NOT_NULL_MESSAGE(*src_buf, "allocate source buffer failed");
    TEST_ASSERT_NOT_NULL_MESSAGE(*dst_buf, "allocate destination buffer failed");

    *from_addr = (uint8_t *)ALIGN_UP((uint32_t)(*src_buf), 4);
    *to_addr = (uint8_t *)ALIGN_UP((uint32_t)(*dst_buf), 4);
    uint8_t gap = MAX(*from_addr - *src_buf, *to_addr - *dst_buf);
    *buffer_size -= gap;

    *from_addr += align;
    *to_addr += align;
    *buffer_size -= align;

    printf("...size %d Bytes, src@%p, dst@%p\r\n", *buffer_size, *from_addr, *to_addr);

    printf("fill src buffer with random data\r\n");
    for (int i = 0; i < *buffer_size; i++) {
        (*from_addr)[i] = rand() % 256;
    }
}

static void cp_dma_verify_and_clear_testbench(uint32_t seed, uint32_t buffer_size, uint8_t *src_buf, uint8_t *dst_buf, uint8_t *from_addr, uint8_t *to_addr)
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

TEST_CASE("memory copy by DMA one by one", "[CP_DMA]")
{
    cp_dma_config_t config = CP_DMA_DEFAULT_CONFIG();
    config.max_in_stream = 4;
    config.max_out_stream = 4;
    cp_dma_driver_t driver = NULL;
    TEST_ESP_OK(cp_dma_driver_install(&config, &driver));

    uint32_t test_buffer_len[] = {256, 512, 1024, 2048, 4096, 5011};
    uint8_t *sbuf = NULL;
    uint8_t *dbuf = NULL;
    uint8_t *from = NULL;
    uint8_t *to = NULL;

    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        // Test different align edge
        for (int align = 0; align < 4; align++) {
            cp_dma_setup_testbench(i, &test_buffer_len[i], &sbuf, &dbuf, &from, &to, align);
            TEST_ESP_OK(cp_dma_memcpy(driver, to, from, test_buffer_len[i], NULL, NULL));
            cp_dma_verify_and_clear_testbench(i, test_buffer_len[i], sbuf, dbuf, from, to);

            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    TEST_ESP_OK(cp_dma_driver_uninstall(driver));
}

TEST_CASE("memory copy by DMA on the fly", "[CP_DMA]")
{
    cp_dma_config_t config = CP_DMA_DEFAULT_CONFIG();
    config.max_in_stream = 4;
    config.max_out_stream = 4;
    cp_dma_driver_t driver = NULL;
    TEST_ESP_OK(cp_dma_driver_install(&config, &driver));

    uint32_t test_buffer_len[] = {512, 1024, 2048, 4096, 5011};
    uint8_t *sbufs[] = {0, 0, 0, 0, 0};
    uint8_t *dbufs[] = {0, 0, 0, 0, 0};
    uint8_t *froms[] = {0, 0, 0, 0, 0};
    uint8_t *tos[] = {0, 0, 0, 0, 0};

    // Aligned case
    for (int i = 0; i < sizeof(sbufs) / sizeof(sbufs[0]); i++) {
        cp_dma_setup_testbench(i, &test_buffer_len[i], &sbufs[i], &dbufs[i], &froms[i], &tos[i], 0);
    }
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        TEST_ESP_OK(cp_dma_memcpy(driver, tos[i], froms[i], test_buffer_len[i], NULL, NULL));
    }
    for (int i = 0; i < sizeof(sbufs) / sizeof(sbufs[0]); i++) {
        cp_dma_verify_and_clear_testbench(i, test_buffer_len[i], sbufs[i], dbufs[i], froms[i], tos[i]);
    }

    // Non-aligned case
    for (int i = 0; i < sizeof(sbufs) / sizeof(sbufs[0]); i++) {
        cp_dma_setup_testbench(i, &test_buffer_len[i], &sbufs[i], &dbufs[i], &froms[i], &tos[i], 3);
    }
    for (int i = 0; i < sizeof(test_buffer_len) / sizeof(test_buffer_len[0]); i++) {
        TEST_ESP_OK(cp_dma_memcpy(driver, tos[i], froms[i], test_buffer_len[i], NULL, NULL));
    }
    for (int i = 0; i < sizeof(sbufs) / sizeof(sbufs[0]); i++) {
        cp_dma_verify_and_clear_testbench(i, test_buffer_len[i], sbufs[i], dbufs[i], froms[i], tos[i]);
    }

    TEST_ESP_OK(cp_dma_driver_uninstall(driver));
}

#define TEST_CP_DMA_MECP_DMAY_BENCH_COUNTS (16)
static uint32_t test_cp_dma_memcpy_bench_len = 4096;
static int count = 0;

static IRAM_ATTR bool test_cp_dma_memcpy_cb(cp_dma_driver_t drv_hdl, cp_dma_event_t *event, void *cb_args)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args;
    BaseType_t high_task_wakeup = pdFALSE;
    switch (event->id) {
    case CP_DMA_EVENT_M2M_DONE:
        count++;
        if (count == TEST_CP_DMA_MECP_DMAY_BENCH_COUNTS) {
            xSemaphoreGiveFromISR(sem, &high_task_wakeup);
        }
        break;
    default:
        break;
    }
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("memory copy by DMA with callback", "[CP_DMA][performance]")
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();

    cp_dma_config_t config = CP_DMA_DEFAULT_CONFIG();
    cp_dma_driver_t driver = NULL;
    TEST_ESP_OK(cp_dma_driver_install(&config, &driver));

    uint8_t *sbuf = NULL;
    uint8_t *dbuf = NULL;
    uint8_t *from = NULL;
    uint8_t *to = NULL;

    cp_dma_setup_testbench(0, &test_cp_dma_memcpy_bench_len, &sbuf, &dbuf, &from, &to, 0);
    count = 0;
    ccomp_timer_start();
    for (int i = 0; i < TEST_CP_DMA_MECP_DMAY_BENCH_COUNTS; i++) {
        TEST_ESP_OK(cp_dma_memcpy(driver, to, from, test_cp_dma_memcpy_bench_len, test_cp_dma_memcpy_cb, sem));
    }

    // wait for done semaphore
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));
    esp_rom_printf("memcpy %d Bytes data by HW costs %lldus\r\n", test_cp_dma_memcpy_bench_len, ccomp_timer_stop() / TEST_CP_DMA_MECP_DMAY_BENCH_COUNTS);

    ccomp_timer_start();
    for (int i = 0; i < TEST_CP_DMA_MECP_DMAY_BENCH_COUNTS; i++) {
        memcpy(to, from, test_cp_dma_memcpy_bench_len);
    }
    esp_rom_printf("memcpy %d Bytes data by SW costs %lldus\r\n", test_cp_dma_memcpy_bench_len, ccomp_timer_stop() / TEST_CP_DMA_MECP_DMAY_BENCH_COUNTS);

    cp_dma_verify_and_clear_testbench(0, test_cp_dma_memcpy_bench_len, sbuf, dbuf, from, to);

    TEST_ESP_OK(cp_dma_driver_uninstall(driver));
    vSemaphoreDelete(sem);
}
