/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "test_utils.h"
#include "test_mmap_utils.h"
#include "spi_flash_mmap.h"
#include "esp_flash.h"
#include "esp_cache.h"


//XIP_PSRAM or RAM_APP enabled
#if (CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA) || CONFIG_APP_BUILD_TYPE_RAM
#define EXECUTE_IN_FLASH   0
#else
#define EXECUTE_IN_FLASH   1
#endif

TEST_CASE("flash_mmap allows esp_flash erase/write/read when mapped", "[spi_flash][mmap]")
{
    const void *ptr1;
    const size_t test_size = 128;

    spi_flash_mmap_handle_t handle1;
    setup_mmap_tests();

    /* map erased test region to ptr1 */
    TEST_ESP_OK( spi_flash_mmap(test_start, test_size, SPI_FLASH_MMAP_FLAG_DATA, &ptr1, &handle1) );
    printf("mmap_res ptr1: handle=%"PRIx32" ptr=%p\n", (uint32_t)handle1, ptr1);

    //Make sure ptr1 is cached
    volatile uint8_t val = 0xff;
    for (int i = 0; i < test_size; i++) {
        val = val ^ ((uint8_t *)ptr1)[i];
    }

    TEST_ESP_OK( esp_flash_erase_region(NULL, test_start, SPI_FLASH_SEC_SIZE) );

    /* verify it's all 0xFF after the erase operation */
    for (int i = 0; i < test_size; i++) {
        TEST_ASSERT_EQUAL_HEX(0xFF, ((uint8_t *)ptr1)[i]);
    }

    /* unmap the erased region */
    spi_flash_munmap(handle1);
    handle1 = 0;

    /* write flash region to 0xEE */
    uint8_t buf[test_size];
    uint8_t read_buf[test_size];
    memset(buf, 0xEE, test_size);
    TEST_ESP_OK( esp_flash_write(NULL, buf, test_start, test_size) );

    /* re-map the test region at ptr1.

       this is a fresh mmap call so should trigger a cache flush,
       ensuring we see the updated flash.
    */
    TEST_ESP_OK( spi_flash_mmap(test_start, test_size, SPI_FLASH_MMAP_FLAG_DATA, &ptr1, &handle1) );
    printf("mmap_res ptr1 #2: handle=%"PRIx32" ptr=%p\n", (uint32_t)handle1, ptr1);

    /* assert that ptr1 now maps to the new values on flash,
       ie contents of buf array.
    */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, ptr1, test_size);

    memset(buf, 0x66, test_size); //this data is select on purpose (can be modified from 0xEE set above)
    TEST_ESP_OK( esp_flash_write(NULL, buf, test_start, test_size) );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, ptr1, test_size);

    memset(read_buf, 0x33, test_size);
    TEST_ESP_OK( esp_flash_read(NULL, read_buf, test_start, test_size) );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, read_buf, test_size);

    spi_flash_munmap(handle1);
    handle1 = 0;
}

typedef struct {
    uint8_t expected_data[128];
    const size_t test_size;
    SemaphoreHandle_t mmap_start; //Given from main to mmap, to start mmap
    SemaphoreHandle_t mmap_end;   //Given from mmap to main indicating end of mmap and start another round
    bool finish;
    bool use_flag_blocks_write;
} test_mmap_concurrent_ctx_t;

//Delay time should be longer than the erasing time
#define MMAP_DELAY  (1000 / portTICK_PERIOD_MS)

static void mmap_task(void* args)
{
    test_mmap_concurrent_ctx_t *ctx = (test_mmap_concurrent_ctx_t*)args;
    const uint32_t mmap_start = test_start;
    const size_t mmap_len = CONFIG_MMU_PAGE_SIZE;

    while (1) {
        xSemaphoreTake(ctx->mmap_start, portMAX_DELAY);
        if (ctx->finish) {
            break;
        }
        uint32_t extra_flags = ctx->use_flag_blocks_write? SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE: 0;

        const void *ptr1;
        TEST_ESP_OK( spi_flash_mmap(mmap_start, mmap_len, SPI_FLASH_MMAP_FLAG_DATA | extra_flags, &ptr1, &handle1) );
        printf("mmap_res ptr1: handle=%"PRIx32" ptr=%p\n", (uint32_t)handle1, ptr1);

        TEST_ASSERT_EQUAL_UINT8_ARRAY(ctx->expected_data, ptr1, ctx->test_size);

        //Delay enough time so that API on other task must happen if it can.
        vTaskDelay(MMAP_DELAY);

        TEST_ASSERT_EQUAL_UINT8_ARRAY(ctx->expected_data, ptr1, ctx->test_size);

        /* unmap the region for test*/
        spi_flash_munmap(handle1);
        handle1 = 0;

        xSemaphoreGive(ctx->mmap_end);
    }
    vTaskDelete(NULL);
}

/*
```mermaid
sequenceDiagram

activate main

main ->>+ mmap : Semphr start

mmap -> mmap : mmap
mmap -> mmap : read & verify data
mmap ->>+ mmap : Delay start

main ->>+ main : flash erase (block starts)

mmap ->>- mmap : Delay end


mmap-> mmap : read & verify data

mmap -> mmap : unmap
mmap ->> main : unblocks
deactivate main

main -> main : flash erase starts
main ->- main : check if unmapped

mmap ->>- main : Semphr ret

activate main
main ->>+ mmap : Semphr start
mmap -> mmap : ...
deactivate mmap

deactivate main
```
*/

static void check_mmap_executed_and_wait(test_mmap_concurrent_ctx_t* ctx, bool expect_unmap)
{
    portBASE_TYPE unmapped = xSemaphoreTake(ctx->mmap_end, 0);
    if (expect_unmap) {
        TEST_ASSERT_EQUAL_INT(pdTRUE, unmapped);
    } else {
        TEST_ASSERT_EQUAL_INT(pdFALSE, unmapped);

        //wait until we can start the next operation
        xSemaphoreTake(ctx->mmap_end, portMAX_DELAY);
    }
}

static void test_concurrent_mmap_core(test_mmap_concurrent_ctx_t *ctx, int test_size, bool use_flag_blocks_write)
{
    const uint32_t api_addr = test_start + CONFIG_MMU_PAGE_SIZE;
    uint8_t read_buf[test_size];
    uint8_t buf[test_size];
    bool expect_blocked = false;

    ctx->use_flag_blocks_write = use_flag_blocks_write;

    // For erase operation, it should take the mmap mutex, and will start after unmap.
    printf("test erase in mmap...\n");
    xSemaphoreGive(ctx->mmap_start);
    esp_rom_delay_us(1000); //delay 1ms to make sure mmap is done
    TEST_ESP_OK(esp_flash_erase_region(NULL, api_addr, SPI_FLASH_SEC_SIZE));
    expect_blocked = use_flag_blocks_write;
#if CONFIG_IDF_TARGET_ESP32
    expect_blocked = false;
#endif
    check_mmap_executed_and_wait(ctx, expect_blocked);

    // Read operations do NOT set ESP_FLASH_START_FLAG_NO_READ, so they are never blocked by mmap.
    printf("test read in mmap...\n");
    xSemaphoreGive(ctx->mmap_start);
    esp_rom_delay_us(1000); //delay 1ms to make sure mmap is done
    TEST_ESP_OK(esp_flash_read(NULL, read_buf, api_addr, test_size));
    check_mmap_executed_and_wait(ctx, false);

    // For write operation, it's same as erase operation.
    printf("test write in mmap...\n");
    xSemaphoreGive(ctx->mmap_start);
    /* write flash region to 0xEE */
    memset(buf, 0xEE, test_size);
    esp_rom_delay_us(1000); //delay 1ms to make sure mmap is done
    TEST_ESP_OK(esp_flash_write(NULL, buf, api_addr, test_size));
    expect_blocked = use_flag_blocks_write;
#if CONFIG_IDF_TARGET_ESP32
    expect_blocked = false;
#endif
    check_mmap_executed_and_wait(ctx, expect_blocked);

    printf("test read in mmap...\n");
    xSemaphoreGive(ctx->mmap_start);
    esp_rom_delay_us(1000); //delay 1ms to make sure mmap is done
    TEST_ESP_OK(esp_flash_read(NULL, read_buf, api_addr, test_size));
    check_mmap_executed_and_wait(ctx, false);
}

TEST_CASE("flash_mmap concurrent access to flash erase/prog", "[spi_flash][mmap]")
{
    setup_mmap_tests();
    const int test_size = 128;

    test_mmap_concurrent_ctx_t ctx = {
        .finish = false,
        .test_size = test_size,
        .use_flag_blocks_write = true
    };
    ctx.mmap_start = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(ctx.mmap_start);
    ctx.mmap_end = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(ctx.mmap_end);
    TEST_ESP_OK(esp_flash_read(NULL, ctx.expected_data, test_start, test_size));

    TaskHandle_t task_handle;
    //Create task with higher priority so that once semphr given, task is unblocked immediately.
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreate(mmap_task, "mmap_task", 4096, &ctx, 5, &task_handle));

    for (int i = 0; i < 3; i++) {
        test_concurrent_mmap_core(&ctx, test_size, true);
        test_concurrent_mmap_core(&ctx, test_size, false);
    }

    ctx.finish = true;
    xSemaphoreGive(ctx.mmap_start);

    vTaskDelay(10 / portTICK_PERIOD_MS);//wait for mmap task to delete

    //recycle the context
    vSemaphoreDelete(ctx.mmap_start);
    vSemaphoreDelete(ctx.mmap_end);
}

/* Number of erase rounds for the non-overlapping concurrent erase test */
#define CONCURRENT_ERASE_ROUNDS 10

typedef struct {
    uint8_t expected_data[128];
    uint32_t region_a_start;
    uint32_t region_b_start;
    size_t test_size;
    bool finish;
    bool use_flag_blocks_write;
    bool data_ok;
    SemaphoreHandle_t erase_done;
    SemaphoreHandle_t erase_op_start;
} test_mmap_erase_concurrent_ctx_t;

static void mmap_verify_task(void *args)
{
    test_mmap_erase_concurrent_ctx_t *ctx = (test_mmap_erase_concurrent_ctx_t *)args;
    uint32_t extra_flags = ctx->use_flag_blocks_write? SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE: 0;

    xSemaphoreTake(ctx->erase_op_start, portMAX_DELAY);

    while (!ctx->finish) {
        //For single core case, delay 1ms to allow erase task to start and finish
        vTaskDelay(1);
        spi_flash_mmap_handle_t verify_handle;
        const void *ptr;
        esp_err_t err = spi_flash_mmap(ctx->region_a_start, ctx->test_size, SPI_FLASH_MMAP_FLAG_DATA | extra_flags, &ptr, &verify_handle);
        if (err != ESP_OK) {
            ctx->data_ok = false;
            break;
        }

        if (memcmp(ctx->expected_data, ptr, ctx->test_size) != 0) {
            ctx->data_ok = false;
            spi_flash_munmap(verify_handle);
            break;
        }

        spi_flash_munmap(verify_handle);
        printf("mmap_verify_task: data ok, calling munmap\n");
    }

    vTaskDelete(NULL);
}

static void erase_task(void *args)
{
    test_mmap_erase_concurrent_ctx_t *ctx = (test_mmap_erase_concurrent_ctx_t *)args;
    xSemaphoreGive(ctx->erase_op_start);

    for (int i = 0; i < CONCURRENT_ERASE_ROUNDS; i++) {
        printf("erase region B: round %d/%d\n", i + 1, CONCURRENT_ERASE_ROUNDS);
        TEST_ESP_OK(esp_flash_erase_region(NULL, ctx->region_b_start, SPI_FLASH_SEC_SIZE));
        printf("erase region B: round %d done\n", i + 1);
    }

    ctx->finish = true;
    xSemaphoreGive(ctx->erase_done);
    vTaskDelete(NULL);
}

static void test_concurrent_erase_core(test_mmap_erase_concurrent_ctx_t *ctx, bool use_flag_blocks_write)
{
    ctx->finish = false;
    ctx->use_flag_blocks_write = use_flag_blocks_write;
    ctx->erase_op_start = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(ctx->erase_op_start);
    ctx->erase_done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(ctx->erase_done);

    printf("test_concurrent_erase_core: use_flag_blocks_write=%d\n", use_flag_blocks_write);

    //Let mmap happen during erasing
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreate(mmap_verify_task, "mmap_verify", 4096, ctx, 6, NULL));
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreate(erase_task, "erase_task", 4096, ctx, 5, NULL));

    //Wait for erase task to complete all rounds, then stop mmap task
    xSemaphoreTake(ctx->erase_done, portMAX_DELAY);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    vSemaphoreDelete(ctx->erase_done);
    vSemaphoreDelete(ctx->erase_op_start);
}

TEST_CASE("flash_mmap region unaffected by concurrent flash erase of separate region", "[spi_flash][mmap]")
{
    setup_mmap_tests();

    const size_t test_size = 128;

    /* Region A: beginning of test partition (mmap + verify target) */
    uint32_t region_a = test_start;
    /* Region B: second sector of test partition (erase target), does not overlap with region A */
    uint32_t region_b = test_start + SPI_FLASH_SEC_SIZE;

    //Prepare random data to write to region A
    uint8_t write_buf[128];
    srand(789);
    for (int i = 0; i < test_size; i ++) {
        write_buf[i] = rand() % 0xff;
    }
    TEST_ESP_OK(esp_flash_erase_region(NULL, region_a, SPI_FLASH_SEC_SIZE));
    TEST_ESP_OK(esp_flash_write(NULL, write_buf, region_a, test_size));

    test_mmap_erase_concurrent_ctx_t ctx = {
        .region_a_start = region_a,
        .region_b_start = region_b,
        .test_size = test_size,
        .data_ok = true,
    };
    memcpy(ctx.expected_data, write_buf, test_size);

    test_concurrent_erase_core(&ctx, false);
    test_concurrent_erase_core(&ctx, true);

    TEST_ASSERT_TRUE_MESSAGE(ctx.data_ok, "Region A data was corrupted during concurrent erase of region B");
}
