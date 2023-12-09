/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <sys/param.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "unity.h"
#include "spi_flash_mmap.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "ccomp_timer.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "esp_timer.h"
#include "esp_partition.h"
#include "bootloader_flash.h"   //for bootloader_flash_xmc_startup
#include "test_utils.h"
#include "sdkconfig.h"

extern const esp_partition_t *get_test_data_partition(void);

struct flash_test_ctx {
    uint32_t offset;
    bool fail;
    SemaphoreHandle_t done;
};

/* Base offset in flash for tests. */
static size_t start;

static void setup_tests(void)
{
    if (start == 0) {
        const esp_partition_t *part = get_test_data_partition();
        start = part->address;
        printf("Test data partition @ 0x%x\n", start);
    }
}

static void flash_test_task(void *arg)
{
    struct flash_test_ctx *ctx = (struct flash_test_ctx *) arg;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    const uint32_t sector = start / SPI_FLASH_SEC_SIZE + ctx->offset;
    printf("t%ld\n", sector);
    printf("es%ld\n", sector);
    if (esp_flash_erase_region(NULL, sector * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE) != ESP_OK) {
        ctx->fail = true;
        printf("Erase failed\r\n");
        xSemaphoreGive(ctx->done);
        vTaskDelete(NULL);
    }
    printf("ed%ld\n", sector);

    vTaskDelay(0 / portTICK_PERIOD_MS);

    uint32_t val = 0xabcd1234;
    for (uint32_t offset = 0; offset < SPI_FLASH_SEC_SIZE; offset += 4) {
        if (esp_flash_write(NULL, (const uint8_t *) &val, sector * SPI_FLASH_SEC_SIZE + offset, 4) != ESP_OK) {
            printf("Write failed at offset=%ld\r\n", offset);
            ctx->fail = true;
            break;
        }
    }
    printf("wd%ld\n", sector);

    vTaskDelay(0 / portTICK_PERIOD_MS);

    uint32_t val_read;
    for (uint32_t offset = 0; offset < SPI_FLASH_SEC_SIZE; offset += 4) {
        if (esp_flash_read(NULL, (uint8_t *) &val_read, sector * SPI_FLASH_SEC_SIZE + offset, 4) != ESP_OK) {
            printf("Read failed at offset=%ld\r\n", offset);
            ctx->fail = true;
            break;
        }
        if (val_read != val) {
            printf("Read invalid value=%08lx at offset=%ld\r\n", val_read, offset);
            ctx->fail = true;
            break;
        }
    }
    printf("td%ld\n", sector);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

TEST_CASE("flash write and erase work both on PRO CPU and on APP CPU", "[spi_flash][ignore]")
{
    setup_tests();

    SemaphoreHandle_t done = xSemaphoreCreateCounting(4, 0);
    struct flash_test_ctx ctx[] = {
            { .offset = 0x10 + 6, .done = done },
            { .offset = 0x10 + 7, .done = done },
            { .offset = 0x10 + 8, .done = done },
#ifndef CONFIG_FREERTOS_UNICORE
            { .offset = 0x10 + 9, .done = done }
#endif
    };

    xTaskCreatePinnedToCore(flash_test_task, "t0", 2048, &ctx[0], 3, NULL, 0);
    xTaskCreatePinnedToCore(flash_test_task, "t1", 2048, &ctx[1], 3, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(flash_test_task, "t2", 2048, &ctx[2], 3, NULL, tskNO_AFFINITY);
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(flash_test_task, "t3", 2048, &ctx[3], 3, NULL, 1);
#endif

    const size_t task_count = sizeof(ctx)/sizeof(ctx[0]);
    for (int i = 0; i < task_count; ++i) {
        xSemaphoreTake(done, portMAX_DELAY);
    }
    for (int i = 0; i < task_count; ++i) {
        TEST_ASSERT_FALSE(ctx[i].fail);
    }
    vSemaphoreDelete(done);
}

//  TODO: This test is disabled on S3 with legacy impl - IDF-3505
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP32S2, ESP32S3, ESP32C3)

#if portNUM_PROCESSORS > 1
TEST_CASE("spi_flash deadlock with high priority busy-waiting task", "[spi_flash][esp_flash]")
{
    typedef struct {
        QueueHandle_t queue;
        volatile bool done;
    } deadlock_test_arg_t;

    /* Create two tasks: high-priority consumer on CPU0, low-priority producer on CPU1.
     * Consumer polls the queue until it gets some data, then yields.
     * Run flash operation on CPU0. Check that when IPC1 task blocks out the producer,
     * the task which does flash operation does not get blocked by the consumer.
     */

    void producer_task(void* varg)
    {
        int dummy = 0;
        deadlock_test_arg_t* arg = (deadlock_test_arg_t*) varg;
        while (!arg->done) {
            xQueueSend(arg->queue, &dummy, 0);
            vTaskDelay(1);
        }
        vTaskDelete(NULL);
    }

    void consumer_task(void* varg)
    {
        int dummy;
        deadlock_test_arg_t* arg = (deadlock_test_arg_t*) varg;
        while (!arg->done) {
            if (xQueueReceive(arg->queue, &dummy, 0) == pdTRUE) {
                vTaskDelay(1);
            }
        }
        vTaskDelete(NULL);
    }
    deadlock_test_arg_t arg = {
        .queue = xQueueCreate(32, sizeof(int)),
        .done = false
    };

    TEST_ASSERT(xTaskCreatePinnedToCore(&producer_task, "producer", 4096, &arg, 5, NULL, 1));
    TEST_ASSERT(xTaskCreatePinnedToCore(&consumer_task, "consumer", 4096, &arg, 10, NULL, 0));

    for (int i = 0; i < 1000; i++) {
        uint32_t dummy;
        TEST_ESP_OK(esp_flash_read(NULL, &dummy, 0, sizeof(dummy)));
    }

    arg.done = true;
    vTaskDelay(5);
    vQueueDelete(arg.queue);

    /* Check that current task priority is still correct */
    TEST_ASSERT_EQUAL_INT(uxTaskPriorityGet(NULL), UNITY_FREERTOS_PRIORITY);
}
#endif // portNUM_PROCESSORS > 1

#endif // !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP32S2, ESP32S3, ESP32C3)

TEST_CASE("WEL is cleared after boot", "[spi_flash]")
{
    esp_rom_spiflash_chip_t *legacy_chip = &g_rom_flashchip;
    uint32_t status;
    esp_rom_spiflash_read_status(legacy_chip, &status);

    TEST_ASSERT((status & 0x2) == 0);
}

#if CONFIG_ESPTOOLPY_FLASHMODE_QIO
// ISSI chip has its QE bit on other chips' BP4, which may get cleared by accident
TEST_CASE("rom unlock will not erase QE bit", "[spi_flash]")
{
    esp_rom_spiflash_chip_t *legacy_chip = &g_rom_flashchip;
    uint32_t status;
    printf("dev_id: %08lx \n", legacy_chip->device_id);

    if (((legacy_chip->device_id >> 16) & 0xff) != 0x9D) {
        TEST_IGNORE_MESSAGE("This test is only for ISSI chips. Ignore.");
    }
    bootloader_flash_unlock();
    esp_rom_spiflash_read_status(legacy_chip, &status);
    printf("status: %08lx\n", status);

    TEST_ASSERT(status & 0x40);
}
#endif

static IRAM_ATTR NOINLINE_ATTR void test_xmc_startup(void)
{
    extern void spi_flash_disable_interrupts_caches_and_other_cpu(void);
    extern void spi_flash_enable_interrupts_caches_and_other_cpu(void);
    esp_err_t ret = ESP_OK;

    spi_flash_disable_interrupts_caches_and_other_cpu();
    ret = bootloader_flash_xmc_startup();
    spi_flash_enable_interrupts_caches_and_other_cpu();

    TEST_ASSERT_EQUAL(ESP_OK, ret);
}

TEST_CASE("bootloader_flash_xmc_startup can be called when cache disabled", "[spi_flash]")
{
    test_xmc_startup();
}
