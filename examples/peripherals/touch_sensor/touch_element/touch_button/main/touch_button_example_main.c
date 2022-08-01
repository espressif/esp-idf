/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touch_element/touch_button.h"
#include "esp_log.h"

static const char *TAG = "Touch Button Example";
#define TOUCH_BUTTON_NUM     14

/* Touch buttons handle */
static touch_button_handle_t button_handle[TOUCH_BUTTON_NUM];

/* Touch buttons channel array */
static const touch_pad_t channel_array[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM10,
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM13,
    TOUCH_PAD_NUM14,
};

/* Touch buttons channel sensitivity array */
static const float channel_sens_array[TOUCH_BUTTON_NUM] = {
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F,
};

#ifdef CONFIG_TOUCH_ELEM_EVENT
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
            ESP_LOGI(TAG, "Button[%d] Press", (int)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
            ESP_LOGI(TAG, "Button[%d] Release", (int)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
            ESP_LOGI(TAG, "Button[%d] LongPress", (int)element_message.arg);
        }
    }
}
#elif CONFIG_TOUCH_ELEM_CALLBACK
/* Button callback routine */
static void button_handler(touch_button_handle_t out_handle, touch_button_message_t *out_message, void *arg)
{
    (void) out_handle; //Unused
    if (out_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
        ESP_LOGI(TAG, "Button[%d] Press", (int)arg);
    } else if (out_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
        ESP_LOGI(TAG, "Button[%d] Release", (int)arg);
    } else if (out_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
        ESP_LOGI(TAG, "Button[%d] LongPress", (int)arg);
    }
}
#endif

void app_main(void)
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
        /* Subscribe touch button events (On Press, On Release, On LongPress) */
        ESP_ERROR_CHECK(touch_button_subscribe_event(button_handle[i],
                        TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS,
                        (void *)channel_array[i]));
#ifdef CONFIG_TOUCH_ELEM_EVENT
        /* Set EVENT as the dispatch method */
        ESP_ERROR_CHECK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
#elif CONFIG_TOUCH_ELEM_CALLBACK
        /* Set EVENT as the dispatch method */
        ESP_ERROR_CHECK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_CALLBACK));
        /* Register a handler function to handle event messages */
        ESP_ERROR_CHECK(touch_button_set_callback(button_handle[i], button_handler));
#endif
        /* Set LongPress event trigger threshold time */
        ESP_ERROR_CHECK(touch_button_set_longpress(button_handle[i], 2000));
    }
    ESP_LOGI(TAG, "Touch buttons created");

#ifdef CONFIG_TOUCH_ELEM_EVENT
    /* Create a handler task to handle event messages */
    xTaskCreate(&button_handler_task, "button_handler_task", 4 * 1024, NULL, 5, NULL);
#endif

    touch_element_start();
    ESP_LOGI(TAG, "Touch element library start");
}
