#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>
#include "driver/timer.h"
#include "esp_intr_alloc.h"

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



typedef struct {
    size_t buf_size;
    uint8_t* buf;
    size_t flash_addr;
    size_t repeat_count;
    SemaphoreHandle_t done;
} read_task_arg_t;


typedef struct {
    size_t delay_time_us;
    size_t repeat_count;
} block_task_arg_t;

static void IRAM_ATTR timer_isr(void* varg) {
    block_task_arg_t* arg = (block_task_arg_t*) varg;
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[0].config.alarm_en = 1;
    ets_delay_us(arg->delay_time_us);
    arg->repeat_count++;
}

static void read_task(void* varg) {
    read_task_arg_t* arg = (read_task_arg_t*) varg;
    for (size_t i = 0; i < arg->repeat_count; ++i) {
        ESP_ERROR_CHECK( spi_flash_read(arg->flash_addr, arg->buf, arg->buf_size) );
    }
    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST_CASE("spi flash functions can run along with IRAM interrupts", "[spi_flash]")
{
    const size_t size = 128;
    read_task_arg_t read_arg = {
            .buf_size = size,
            .buf = (uint8_t*) malloc(size),
            .flash_addr = 0,
            .repeat_count = 1000,
            .done = xSemaphoreCreateBinary()
    };

    timer_config_t config = {
            .alarm_en = true,
            .counter_en = false,
            .intr_type = TIMER_INTR_LEVEL,
            .counter_dir = TIMER_COUNT_UP,
            .auto_reload = true,
            .divider = 80
    };

    block_task_arg_t block_arg = {
            .repeat_count = 0,
            .delay_time_us = 100
    };

    ESP_ERROR_CHECK( timer_init(TIMER_GROUP_0, TIMER_0, &config) );
    timer_pause(TIMER_GROUP_0, TIMER_0);
    ESP_ERROR_CHECK( timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 120) );
    intr_handle_t handle;
    ESP_ERROR_CHECK( timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_isr, &block_arg, ESP_INTR_FLAG_IRAM, &handle) );
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_start(TIMER_GROUP_0, TIMER_0);

    xTaskCreatePinnedToCore(read_task, "r", 2048, &read_arg, 3, NULL, 1);
    xSemaphoreTake(read_arg.done, portMAX_DELAY);

    timer_pause(TIMER_GROUP_0, TIMER_0);
    timer_disable_intr(TIMER_GROUP_0, TIMER_0);
    esp_intr_free(handle);
    vSemaphoreDelete(read_arg.done);
    free(read_arg.buf);
}

