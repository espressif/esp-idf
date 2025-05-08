/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash.h"
#include "test_flash_utils.h"
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "test_utils.h"
#include "driver/gptimer.h"
#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
#include "hal/spimem_flash_ll.h"

#define TIMER_RESOLUTION_HZ     (1 * 1000 * 1000) // 1MHz resolution

typedef struct {
    flash_test_handle_t *flash_handle;
    SemaphoreHandle_t sem;
} spi_flash_test_context_t;

void spi_flash_suspend_test_task(void *arg)
{
    spi_flash_test_context_t *context = (spi_flash_test_context_t *)arg;

    uint32_t cnt = 20;
    while (cnt--) {
        if (context->flash_handle->suspend_times != 0) {
            break;
        }
        esp_err_t ret = esp_flash_erase_region(NULL, context->flash_handle->operation_address, context->flash_handle->operation_size);
        if (ret != ESP_OK) {
            abort();
        }
    }

    // Give Semaphore and delete task.
    xSemaphoreGive(context->sem);
    vTaskDelete(NULL);
}

static bool IRAM_ATTR gptimer_alarm_suspend_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    i2c_master_dev_handle_t dev_handle = (i2c_master_dev_handle_t) user_ctx;
    spi_flash_suspend_test_invalidate_cache();
    uint8_t buf[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    i2c_master_transmit(dev_handle, buf, 6, 200);

    return false;
}

TEST_CASE("Flash suspend support on i2c", "[i2c]")
{
    // Init context.
    spi_flash_test_context_t *context = heap_caps_calloc(1, sizeof(spi_flash_test_context_t), MALLOC_CAP_DEFAULT);
    assert(context != NULL);

    context->sem = xSemaphoreCreateBinary();
    assert(context->sem != NULL);

    spi_flash_suspend_test_init(&context->flash_handle);

    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0,
        .scl_io_num = 6,
        .sda_io_num = 7,
        .glitch_ignore_cnt = 7,
        .trans_queue_depth = 37,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 400000,
        .flags.disable_ack_check = true,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_HZ,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1000,
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = gptimer_alarm_suspend_cb,
    };

    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, dev_handle));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    // Then use gptimer for trigger testing function every time.
    // Quit when suspend is triggered.
    xTaskCreatePinnedToCore(spi_flash_suspend_test_task, "flash_task", 4096, context, 2, NULL, 0);

    xSemaphoreTake(context->sem, portMAX_DELAY);

    printf("test finishes, suspend for %ld times\n", context->flash_handle->suspend_times);
    TEST_ASSERT_NOT_EQUAL(0, context->flash_handle->suspend_times);

    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));

    // Clear sources
    vTaskDelay(10);
    vSemaphoreDelete(context->sem);
    spi_flash_suspend_test_deinit(context->flash_handle);
    free(context);
}

#endif //SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
