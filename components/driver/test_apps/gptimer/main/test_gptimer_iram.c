/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "driver/gptimer.h"
#include "esp_spi_flash.h"
#include "soc/soc_caps.h"

void test_app_include_gptimer_iram(void)
{
}

#if CONFIG_GPTIMER_ISR_IRAM_SAFE

typedef struct {
    size_t buf_size;
    uint8_t *buf;
    size_t flash_addr;
    size_t repeat_count;
    SemaphoreHandle_t done_sem;
} read_task_arg_t;

typedef struct {
    size_t delay_time_us;
    size_t repeat_count;
} block_task_arg_t;

static bool IRAM_ATTR on_gptimer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    block_task_arg_t *arg = (block_task_arg_t *)user_ctx;
    esp_rom_delay_us(arg->delay_time_us);
    arg->repeat_count++;
    return false;
}

static void flash_read_task(void *varg)
{
    read_task_arg_t *arg = (read_task_arg_t *)varg;
    for (size_t i = 0; i < arg->repeat_count; i++) {
        TEST_ESP_OK(spi_flash_read(arg->flash_addr, arg->buf, arg->buf_size));
    }
    xSemaphoreGive(arg->done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("gptimer_iram_interrupt_safe", "[gptimer]")
{
    gptimer_handle_t gptimer = NULL;
    const size_t size = 128;
    uint8_t *buf = malloc(size);
    TEST_ASSERT_NOT_NULL(buf);
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);
    read_task_arg_t read_arg = {
        .buf_size = size,
        .buf = buf,
        .flash_addr = 0,
        .repeat_count = 1000,
        .done_sem = done_sem,
    };

    block_task_arg_t block_arg = {
        .repeat_count = 0,
        .delay_time_us = 100,
    };

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_APB,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_gptimer_alarm_cb,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 120,
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, &block_arg));
    TEST_ESP_OK(gptimer_start(gptimer));

    xTaskCreatePinnedToCore(flash_read_task, "read_flash", 2048, &read_arg, 3, NULL, portNUM_PROCESSORS - 1);
    // wait for task done
    xSemaphoreTake(done_sem, portMAX_DELAY);
    printf("alarm callback runs %d times\r\n", block_arg.repeat_count);
    TEST_ASSERT_GREATER_THAN(1000, block_arg.repeat_count);
    // delete gptimer
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
    vSemaphoreDelete(done_sem);
    free(buf);
    // leave time for IDLE task to recycle deleted task
    vTaskDelay(2);
}

#endif // CONFIG_GPTIMER_ISR_IRAM_SAFE
