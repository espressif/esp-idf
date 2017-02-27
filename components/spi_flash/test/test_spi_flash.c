#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>

#include "test_config.h"

struct flash_test_ctx {
    uint32_t offset;
    bool fail;
    SemaphoreHandle_t done;
};

static void flash_test_task(void *arg)
{
    struct flash_test_ctx *ctx = (struct flash_test_ctx *) arg;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    const uint32_t sector = ctx->offset;
    printf("t%d\n", sector);
    printf("es%d\n", sector);
    if (spi_flash_erase_sector(sector) != ESP_OK) {
        ctx->fail = true;
        printf("Erase failed\r\n");
        xSemaphoreGive(ctx->done);
        vTaskDelete(NULL);
    }
    printf("ed%d\n", sector);

    vTaskDelay(0 / portTICK_PERIOD_MS);

    uint32_t val = 0xabcd1234;
    for (uint32_t offset = 0; offset < SPI_FLASH_SEC_SIZE; offset += 4) {
        if (spi_flash_write(sector * SPI_FLASH_SEC_SIZE + offset, (const uint8_t *) &val, 4) != ESP_OK) {
            printf("Write failed at offset=%d\r\n", offset);
            ctx->fail = true;
            break;
        }
    }
    printf("wd%d\n", sector);

    vTaskDelay(0 / portTICK_PERIOD_MS);

    uint32_t val_read;
    for (uint32_t offset = 0; offset < SPI_FLASH_SEC_SIZE; offset += 4) {
        if (spi_flash_read(sector * SPI_FLASH_SEC_SIZE + offset, (uint8_t *) &val_read, 4) != ESP_OK) {
            printf("Read failed at offset=%d\r\n", offset);
            ctx->fail = true;
            break;
        }
        if (val_read != val) {
            printf("Read invalid value=%08x at offset=%d\r\n", val_read, offset);
            ctx->fail = true;
            break;
        }
    }
    printf("td%d\n", sector);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

TEST_CASE("flash write and erase work both on PRO CPU and on APP CPU", "[spi_flash][ignore]")
{
    SemaphoreHandle_t done = xSemaphoreCreateCounting(4, 0);
    struct flash_test_ctx ctx[] = {
            { .offset = 0x100 + 6, .done = done },
            { .offset = 0x100 + 7, .done = done },
            { .offset = 0x100 + 8, .done = done },
#ifndef CONFIG_FREERTOS_UNICORE
            { .offset = 0x100 + 9, .done = done }
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
        TEST_ASSERT_FALSE(ctx[i].fail);
    }
    vSemaphoreDelete(done);
}

