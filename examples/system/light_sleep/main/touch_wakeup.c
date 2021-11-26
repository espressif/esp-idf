/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "touch_element/touch_button.h"

static const char *TAG = "touch_wakeup";

#define TOUCH_BUTTON_NUM            5

/* Touch buttons handle */
static touch_button_handle_t button_handle[TOUCH_BUTTON_NUM];


/* Touch buttons channel array */
static const touch_pad_t channel_array[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
};

/* Touch buttons channel sensitivity array */
static const float channel_sens_array[TOUCH_BUTTON_NUM] = {
    0.03F,
    0.03F,
    0.03F,
    0.03F,
    0.03F,
};

/* Button event handler task */
static void button_handler_task(void *arg)
{
    (void) arg; //Unused
    touch_elem_message_t element_message;
    while (1) {
        /* Waiting for touch element messages */
        touch_element_message_receive(&element_message, portMAX_DELAY);
        if (element_message.element_type != TOUCH_ELEM_TYPE_BUTTON) {
            continue;
        }
        /* Decode message */
        const touch_button_message_t *button_message = touch_button_get_message(&element_message);
        if (button_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
            ESP_LOGI(TAG, "Button[%"PRIu32"] Press", (uint32_t)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
            ESP_LOGI(TAG, "Button[%"PRIu32"] Release", (uint32_t)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
            ESP_LOGI(TAG, "Button[%"PRIu32"] LongPress", (uint32_t)element_message.arg);
        }
    }
    vTaskDelete(NULL);
}

esp_err_t example_register_touch_wakeup(void)
{
    /* Initialize Touch Element library */
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_element_install(&global_config));
    ESP_LOGI(TAG, "Touch element library installed");

    touch_button_global_config_t button_global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_button_install(&button_global_config));
    ESP_LOGI(TAG, "Touch button installed");
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = channel_array[i],
            .channel_sens = channel_sens_array[i]
        };
        /* Create Touch buttons */
        ESP_ERROR_CHECK(touch_button_create(&button_config, &button_handle[i]));
        /* Set EVENT as the dispatch method */
        ESP_ERROR_CHECK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
        /* Subscribe touch button events (On Press, On Release, On LongPress) */
        ESP_ERROR_CHECK(touch_button_subscribe_event(button_handle[i],
                                                     TOUCH_ELEM_EVENT_ON_PRESS |
                                                     TOUCH_ELEM_EVENT_ON_RELEASE |
                                                     TOUCH_ELEM_EVENT_ON_LONGPRESS,
                                                     (void *)channel_array[i]));
    }
    ESP_LOGI(TAG, "Touch buttons created");

    touch_elem_sleep_config_t sleep_config = {
        .sample_count = global_config.hardware.sample_count,
        .sleep_cycle = global_config.hardware.sleep_cycle,
    };
    /* Enable one of registered touch button as light/deep sleep wake-up source */
    ESP_ERROR_CHECK(touch_element_enable_light_sleep(&sleep_config));

    touch_element_start();
    xTaskCreate(&button_handler_task, "button_handler_task", 4 * 1024, NULL, 6, NULL);
    ESP_LOGI(TAG, "touch wakeup source is ready");
    return ESP_OK;
}
