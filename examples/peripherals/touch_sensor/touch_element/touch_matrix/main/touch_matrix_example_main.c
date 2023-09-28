/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touch_element/touch_matrix.h"
#include "esp_log.h"

static const char *TAG = "Touch Matrix Example";
#define X_AXIS_CHANNEL_NUM     3
#define Y_AXIS_CHANNEL_NUM     3

static touch_matrix_handle_t matrix_handle;

/* Touch Matrix Button x-axis channels array */
static const touch_pad_t x_axis_channel[X_AXIS_CHANNEL_NUM] = {
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM9,
};

/* Touch Matrix Button y-axis channels array */
static const touch_pad_t y_axis_channel[Y_AXIS_CHANNEL_NUM] = {
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM14,
};

/* Touch Matrix Button x-axis channels sensitivity array */
static const float x_axis_channel_sens[X_AXIS_CHANNEL_NUM] = {
    0.1F,
    0.1F,
    0.1F,
};

/* Touch Matrix Button y-axis channel sensitivity array */
static const float y_axis_channel_sens[Y_AXIS_CHANNEL_NUM] = {
    0.1F,
    0.1F,
    0.1F,
};

#ifdef CONFIG_TOUCH_ELEM_EVENT
/* Matrix event handler task */
static void matrix_handler_task(void *arg)
{
    (void) arg; //Unused
    touch_elem_message_t element_message;
    while (1) {
        /* Waiting for touch element messages */
        touch_element_message_receive(&element_message, portMAX_DELAY); //Block take
        if (element_message.element_type != TOUCH_ELEM_TYPE_MATRIX) {
            continue;
        }
        /* Decode message */
        const touch_matrix_message_t *matrix_message = touch_matrix_get_message(&element_message);
        if (matrix_message->event == TOUCH_MATRIX_EVT_ON_PRESS) {
            ESP_LOGI(TAG, "Matrix Press, axis: (%"PRIu8", %"PRIu8") index: %"PRIu8, matrix_message->position.x_axis,
                     matrix_message->position.y_axis, matrix_message->position.index);
        } else if (matrix_message->event == TOUCH_MATRIX_EVT_ON_RELEASE) {
            ESP_LOGI(TAG, "Matrix Release, axis: (%"PRIu8", %"PRIu8") index: %"PRIu8, matrix_message->position.x_axis,
                     matrix_message->position.y_axis, matrix_message->position.index);
        } else if (matrix_message->event == TOUCH_MATRIX_EVT_ON_LONGPRESS) {
            ESP_LOGI(TAG, "Matrix LongPress, axis: (%"PRIu8", %"PRIu8") index: %"PRIu8, matrix_message->position.x_axis,
                     matrix_message->position.y_axis, matrix_message->position.index);
        }
    }
}
#elif CONFIG_TOUCH_ELEM_CALLBACK
/* Matrix callback routine */
void matrix_handler(touch_matrix_handle_t out_handle, touch_matrix_message_t *out_message, void *arg)
{
    (void) arg; //Unused
    if (out_handle != matrix_handle) {
        return;
    }
    if (out_message->event == TOUCH_MATRIX_EVT_ON_PRESS) {
        ESP_LOGI(TAG, "Matrix Press, axis: (%"PRIu8", %"PRIu8") index: %"PRIu8, out_message->position.x_axis,
                 out_message->position.y_axis, out_message->position.index);
    } else if (out_message->event == TOUCH_MATRIX_EVT_ON_RELEASE) {
        ESP_LOGI(TAG, "Matrix Release, axis: (%"PRIu8", %"PRIu8") index: %"PRIu8, out_message->position.x_axis,
                 out_message->position.y_axis, out_message->position.index);
    } else if (out_message->event == TOUCH_MATRIX_EVT_ON_LONGPRESS) {
        ESP_LOGI(TAG, "Matrix LongPress, axis: (%"PRIu8", %"PRIu8") index: %"PRIu8, out_message->position.x_axis,
                 out_message->position.y_axis, out_message->position.index);
    }
}
#endif

void app_main(void)
{
    /* Initialize Touch Element library */
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_element_install(&global_config));
    ESP_LOGI(TAG, "Touch element library installed");

    touch_matrix_global_config_t matrix_global_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_matrix_install(&matrix_global_config));
    ESP_LOGI(TAG, "Touch matrix installed");
    /* Create Touch Matrix Button */
    touch_matrix_config_t matrix_config = {
        .x_channel_array = x_axis_channel,
        .y_channel_array = y_axis_channel,
        .x_sensitivity_array = x_axis_channel_sens,
        .y_sensitivity_array = y_axis_channel_sens,
        .x_channel_num = (sizeof(x_axis_channel) / sizeof(x_axis_channel[0])),
        .y_channel_num = (sizeof(y_axis_channel) / sizeof(y_axis_channel[0]))
    };
    ESP_ERROR_CHECK(touch_matrix_create(&matrix_config, &matrix_handle));
    /* Subscribe touch matrix events (On Press, On Release, On LongPress) */
    ESP_ERROR_CHECK(touch_matrix_subscribe_event(matrix_handle,
                                                 TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS, NULL));
#ifdef CONFIG_TOUCH_ELEM_EVENT
    /* Set EVENT as the dispatch method */
    ESP_ERROR_CHECK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_EVENT));
    /* Create a handler task to handle event messages */
    xTaskCreate(&matrix_handler_task, "matrix_handler_task", 4 * 1024, NULL, 5, NULL);
#elif CONFIG_TOUCH_ELEM_CALLBACK
    /* Set CALLBACK as the dispatch method */
    ESP_ERROR_CHECK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_CALLBACK));
    /* Register a handler function to handle event messages */
    ESP_ERROR_CHECK(touch_matrix_set_callback(matrix_handle, matrix_handler));
#endif
    ESP_LOGI(TAG, "Touch matrix created");

    touch_element_start();
    ESP_LOGI(TAG, "Touch element library start");
}
