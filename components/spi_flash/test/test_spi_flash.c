#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>

struct flash_test_ctx {
    uint32_t offset[2];
    bool fail[2];
    SemaphoreHandle_t done;
};

static void flash_test_task(void *arg)
{
    const uint32_t coreid = xPortGetCoreID();
    ets_printf("t%d\n", coreid);
    struct flash_test_ctx *ctx = (struct flash_test_ctx *) arg;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    const uint32_t sector = ctx->offset[coreid];
    ets_printf("es%d\n", coreid);
    if (spi_flash_erase_sector(sector) != ESP_OK) {
        ctx->fail[coreid] = true;
        ets_printf("Erase failed\r\n");
        xSemaphoreGive(ctx->done);
        vTaskDelete(NULL);
    }
    ets_printf("ed%d\n", coreid);

    vTaskDelay(0 / portTICK_PERIOD_MS);

    uint32_t val = 0xabcd1234;
    const uint32_t n = 4096;
    for (uint32_t offset = 0; offset < n; offset += 4) {
        if (spi_flash_write(sector * SPI_FLASH_SEC_SIZE + offset, (const uint8_t *) &val, 4) != ESP_OK) {
            ets_printf("Write failed at offset=%d\r\n", offset);
            ctx->fail[coreid] = true;
            break;
        }
    }
    ets_printf("wd%d\n", coreid);

    vTaskDelay(0 / portTICK_PERIOD_MS);

    uint32_t val_read;
    for (uint32_t offset = 0; offset < n; offset += 4) {
        if (spi_flash_read(sector * SPI_FLASH_SEC_SIZE + offset, (uint8_t *) &val_read, 4) != ESP_OK) {
            ets_printf("Read failed at offset=%d\r\n", offset);
            ctx->fail[coreid] = true;
            break;
        }
        if (val_read != val) {
            ets_printf("Read invalid value=%08x at offset=%d\r\n", val_read, offset);
            ctx->fail[coreid] = true;
            break;
        }
    }
    ets_printf("td%d\n", coreid);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

TEST_CASE("flash write and erase work both on PRO CPU and on APP CPU", "[spi_flash]")
{
    TaskHandle_t procpu_task;
    TaskHandle_t appcpu_task;
    struct flash_test_ctx ctx;

    ctx.offset[0] = 6;
    ctx.offset[1] = 7;
    ctx.fail[0] = 0;
    ctx.fail[1] = 0;
    ctx.done = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(flash_test_task, "1", 2048, &ctx, 3, &procpu_task, 0);
    if (portNUM_PROCESSORS == 2) {
        xTaskCreatePinnedToCore(flash_test_task, "2", 2048, &ctx, 3, &appcpu_task, 1);
    }

    xSemaphoreTake(ctx.done, portMAX_DELAY);
    if (portNUM_PROCESSORS == 2) {
        xSemaphoreTake(ctx.done, portMAX_DELAY);
    }

    TEST_ASSERT_EQUAL(false, ctx.fail[0]);
    if (portNUM_PROCESSORS == 2) {
        TEST_ASSERT_EQUAL(false, ctx.fail[1]);
    }
}

