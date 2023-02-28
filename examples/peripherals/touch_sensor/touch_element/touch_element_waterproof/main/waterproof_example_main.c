/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "touch_element/touch_button.h"

static const char *TAG = "Touch Element Waterproof Example";
#define TOUCH_BUTTON_NUM     3

/*< Touch buttons handle */
static touch_button_handle_t button_handle[TOUCH_BUTTON_NUM]; //Button handler

/* Touch buttons channel array */
static const touch_pad_t channel_array[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM11,
};

/* Touch buttons channel sensitivity array */
static const float channel_sens_array[TOUCH_BUTTON_NUM] = {
    0.15F,
    0.15F,
    0.15F,
};

static void button_handler_task(void *arg)
{
    touch_elem_message_t element_message;
    while (1) {
        touch_element_message_receive(&element_message, portMAX_DELAY); //Block take
        const touch_button_message_t *button_message = touch_button_get_message(&element_message);
        if (button_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
            ESP_LOGI(TAG, "Button[%d] Press", (int)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
            ESP_LOGI(TAG, "Button[%d] Release", (int)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
            ESP_LOGI(TAG, "Button[%d] LongPress", (int)element_message.arg);
        }
    }
}

void app_main(void)
{
    /*< Initialize Touch Element library */
    touch_elem_global_config_t element_global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_element_install(&element_global_config));
    ESP_LOGI(TAG, "Touch Element library install");
    /*< Create and configure touch element waterproof */
    touch_elem_waterproof_config_t waterproof_config = {
#ifdef CONFIG_TOUCH_WATERPROOF_GUARD_ENABLE
        .guard_channel = TOUCH_PAD_NUM13,
#else
        .guard_channel = TOUCH_WATERPROOF_GUARD_NOUSE,
#endif
        .guard_sensitivity = 0.05F  //The guard sensor sensitivity has to be explored in experiments
    };
    ESP_ERROR_CHECK(touch_element_waterproof_install(&waterproof_config));
    ESP_LOGI(TAG, "Touch Element waterproof install");

    touch_button_global_config_t button_global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_button_install(&button_global_config));
    ESP_LOGI(TAG, "Touch button install");
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = channel_array[i],
            .channel_sens = channel_sens_array[i]
        };
        /* Create touch button */
        ESP_ERROR_CHECK(touch_button_create(&button_config, &button_handle[i]));
        /* Subscribe touch button event(Press, Release, LongPress) */
        ESP_ERROR_CHECK(touch_button_subscribe_event(button_handle[i],
                        TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS,
                        (void *)channel_array[i]));
        /* Button set dispatch method */
        ESP_ERROR_CHECK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
#ifdef CONFIG_TOUCH_WATERPROOF_GUARD_ENABLE
        /* Add button element into waterproof guard sensor's protection */
        ESP_ERROR_CHECK(touch_element_waterproof_add(button_handle[i]));
#endif
    }
    ESP_LOGI(TAG, "Touch buttons create");
    /*< Create a monitor task to take Touch Button event */
    xTaskCreate(&button_handler_task, "button_handler_task", 4 * 1024, NULL, 5, NULL);
    touch_element_start();
}
