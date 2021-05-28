/* Touch Sensor - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "touch_element/touch_button.h"

static const char *TAG = "Touch Button Example";

#define TOUCH_BUTTON_NUM     5

/* Touch buttons handle */
static touch_button_handle_t button_handle[TOUCH_BUTTON_NUM];
static touch_button_handle_t wakeup_button_handle = NULL;
static touch_button_handle_t entry_button_handle = NULL;


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

        if (element_message.handle == entry_button_handle) {
            if (button_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
                ESP_LOGI(TAG, "Entering Deep sleep ...");
                fflush(stdout);
                esp_deep_sleep_start();
            }
        }

        if (button_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
            ESP_LOGI(TAG, "Button[%d] Press", (uint32_t)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
            ESP_LOGI(TAG, "Button[%d] Release", (uint32_t)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
            ESP_LOGI(TAG, "Button[%d] LongPress", (uint32_t)element_message.arg);
        }
    }
}

void app_main(void)
{
    esp_sleep_wakeup_cause_t wakeup_ret = esp_sleep_get_wakeup_cause();
    printf("------%d\n", wakeup_ret);
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
        if (i == CONFIG_TE_WAKEUP_BUTTON_INDEX || i == CONFIG_TE_ENTRY_BUTTON_INDEX) {
            continue;
        } else {
            ESP_ERROR_CHECK(touch_button_subscribe_event(button_handle[i], TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, (void *)channel_array[i]));
        }

        /* Set EVENT as the dispatch method */
        ESP_ERROR_CHECK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
    }
    wakeup_button_handle = button_handle[CONFIG_TE_WAKEUP_BUTTON_INDEX];
    entry_button_handle = button_handle[CONFIG_TE_ENTRY_BUTTON_INDEX];
    ESP_ERROR_CHECK(touch_button_subscribe_event(entry_button_handle, TOUCH_ELEM_EVENT_ON_PRESS, (void *)channel_array[CONFIG_TE_ENTRY_BUTTON_INDEX]));
    ESP_ERROR_CHECK(touch_button_set_dispatch_method(entry_button_handle, TOUCH_ELEM_DISP_EVENT));
    ESP_LOGI(TAG, "Touch buttons created");

    touch_elem_sleep_config_t sleep_config = {
        .scan_time = global_config.hardware.sample_count,
        .sleep_time = global_config.hardware.sleep_cycle,
    };
    ESP_ERROR_CHECK(touch_element_sleep_install(&sleep_config));
    ESP_ERROR_CHECK(touch_element_sleep_add_wakeup(wakeup_button_handle));
#ifdef CONFIG_TE_SKIP_CALIBRATION
    ESP_ERROR_CHECK(touch_element_sleep_config_wakeup_calibration(wakeup_button_handle, true));
#endif

    touch_element_start();
    ESP_LOGI(TAG, "Touch element library start");
    xTaskCreate(&button_handler_task, "button_handler_task", 4 * 1024, NULL, 5, NULL);
    ESP_LOGI(TAG, "Press Button[%d] to enter sleep and press Button[%d] to wakeup system", channel_array[CONFIG_TE_ENTRY_BUTTON_INDEX], channel_array[CONFIG_TE_WAKEUP_BUTTON_INDEX]);
}
