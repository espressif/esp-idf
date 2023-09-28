/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touch_element/touch_button.h"
#include "touch_element/touch_slider.h"
#include "esp_log.h"

static const char *TAG = "Touch Elements Combination Example";
#define TOUCH_BUTTON_NUM     3
#define TOUCH_SLIDER_CHANNEL_NUM     5

static touch_button_handle_t button_handle[TOUCH_BUTTON_NUM]; //Touch buttons handle
static touch_slider_handle_t slider_handle; //Touch slider handle

/* Touch buttons channel array */
static const touch_pad_t button_channel_array[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM10
};

/* Touch buttons channel sensitivity array */
static const float button_channel_sens_array[TOUCH_BUTTON_NUM] = {
    0.1F,
    0.1F,
    0.1F
};

/* Touch slider channels array */
static const touch_pad_t slider_channel_array[TOUCH_SLIDER_CHANNEL_NUM] = {
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
};

/* Touch slider channels sensitivity array */
static const float slider_channel_sens_array[TOUCH_SLIDER_CHANNEL_NUM] = {
    0.252F,
    0.246F,
    0.277F,
    0.250F,
    0.257F,
};

static void button_handler(touch_elem_message_t element_message)
{
    const touch_button_message_t *button_message = touch_button_get_message(&element_message);
    if (button_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
        ESP_LOGI(TAG, "Button[%d] Press", (int)element_message.arg);
    } else if (button_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
        ESP_LOGI(TAG, "Button[%d] Release", (int)element_message.arg);
    } else if (button_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
        ESP_LOGI(TAG, "Button[%d] LongPress", (int)element_message.arg);
    }
}

static void slider_handler(touch_elem_message_t element_message)
{
    const touch_slider_message_t *slider_message = touch_slider_get_message(&element_message);
    if (slider_message->event == TOUCH_SLIDER_EVT_ON_PRESS) {
        ESP_LOGI(TAG, "Slider Press, position: %"PRIu32, slider_message->position);
    } else if (slider_message->event == TOUCH_SLIDER_EVT_ON_RELEASE) {
        ESP_LOGI(TAG, "Slider Release, position: %"PRIu32, slider_message->position);
    } else if (slider_message->event == TOUCH_SLIDER_EVT_ON_CALCULATION) {
        ESP_LOGI(TAG, "Slider Calculate, position: %"PRIu32, slider_message->position);
    }
}

static void event_handler_task(void *arg)
{
    (void) arg; //Unused
    touch_elem_message_t element_message;
    while (1) {
        /* Waiting for touch element messages */
        touch_element_message_receive(&element_message, portMAX_DELAY);
        switch (element_message.element_type) {
        case TOUCH_ELEM_TYPE_BUTTON:
            button_handler(element_message);
            break;
        case TOUCH_ELEM_TYPE_SLIDER:
            slider_handler(element_message);
            break;
        default:
            ESP_LOGW(TAG, "Unknown element message");
            break;
        }
    }
}

void button_example_init(void)
{
    touch_button_global_config_t global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_button_install(&global_config));
    ESP_LOGI(TAG, "Touch button installed");
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = button_channel_sens_array[i]
        };
        /* Create Touch buttons */
        ESP_ERROR_CHECK(touch_button_create(&button_config, &button_handle[i]));
        /* Subscribe touch button events (On Press, On Release, On LongPress) */
        ESP_ERROR_CHECK(touch_button_subscribe_event(button_handle[i],
                                                     TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS,
                                                     (void *)button_channel_array[i]));
        /* Set EVENT as the dispatch method */
        ESP_ERROR_CHECK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
        /* Set LongPress event trigger threshold time */
        ESP_ERROR_CHECK(touch_button_set_longpress(button_handle[i], 2000));
    }
    ESP_LOGI(TAG, "Touch buttons created");
}

void slider_example_init(void)
{
    touch_slider_global_config_t global_config = TOUCH_SLIDER_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_slider_install(&global_config));
    ESP_LOGI(TAG, "Touch slider installed");
    /* Create Touch slider */
    touch_slider_config_t slider_config = {
        .channel_array = slider_channel_array,
        .sensitivity_array = slider_channel_sens_array,
        .channel_num = (sizeof(slider_channel_array) / sizeof(slider_channel_array[0])),
        .position_range = 101
    };
    ESP_ERROR_CHECK(touch_slider_create(&slider_config, &slider_handle));
    /* Subscribe touch slider events (On Press, On Release, On Calculation) */
    ESP_ERROR_CHECK(touch_slider_subscribe_event(slider_handle,
                                                 TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_CALCULATION, NULL));
    /* Set EVENT as the dispatch method */
    ESP_ERROR_CHECK(touch_slider_set_dispatch_method(slider_handle, TOUCH_ELEM_DISP_EVENT));
    ESP_LOGI(TAG, "Touch slider created");
}

void app_main(void)
{
    /* Initialize Touch Element library */
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_element_install(&global_config));
    ESP_LOGI(TAG, "Touch element library installed");

    button_example_init();
    slider_example_init();

    touch_element_start();
    ESP_LOGI(TAG, "Touch element library start");
    /* Create a handler task to handle event messages */
    xTaskCreate(&event_handler_task, "event_handler_task", 4 * 1024, NULL, 5, NULL);
}
