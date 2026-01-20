/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_etm.h"
#include "driver/gpio_etm.h"
#include "driver/gpio.h"
#include "esp_async_memcpy.h"

TEST_CASE("async_memcpy_eof_event", "[GDMA][ETM]")
{
    const uint32_t output_gpio = 1;
    // async_memcpy done ---> ETM channel A ---> GPIO toggle
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    printf("allocate GPIO etm task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));
    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));

    printf("initialize gpio\r\n");
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = 1ULL << output_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));

    // put the GPIO into initial state
    TEST_ESP_OK(gpio_set_level(output_gpio, 1));

    printf("install async memcpy context\r\n");
    async_memcpy_handle_t mcp_ctx = NULL;
    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    TEST_ESP_OK(esp_async_memcpy_install(&config, &mcp_ctx));

    printf("get async memcpy etm event handle\r\n");
    esp_etm_event_handle_t mcp_event = NULL;
    TEST_ESP_OK(esp_async_memcpy_new_etm_event(mcp_ctx, ASYNC_MEMCPY_ETM_EVENT_COPY_DONE, &mcp_event));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, mcp_event, gpio_task));
    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    TEST_ESP_OK(esp_etm_dump(stdout));

    const uint32_t buffer_size = 1024;
    uint8_t *src_buf = heap_caps_aligned_alloc(64, buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(src_buf);
    uint8_t *dst_buf = heap_caps_aligned_alloc(64, buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(dst_buf);

    printf("start memcpy\r\n");
    for (int j = 0; j < 19; j++) {
        TEST_ESP_OK(esp_async_memcpy(mcp_ctx, dst_buf, src_buf, buffer_size, NULL, NULL));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    // simply wait for the last memcpy to finish
    vTaskDelay(pdMS_TO_TICKS(1000));

    // check the final GPIO level
    TEST_ASSERT_EQUAL(0, gpio_get_level(output_gpio));

    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task, output_gpio));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_task(gpio_task));
    TEST_ESP_OK(esp_etm_del_event(mcp_event));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
    TEST_ESP_OK(esp_async_memcpy_uninstall(mcp_ctx));
    free(src_buf);
    free(dst_buf);
}
