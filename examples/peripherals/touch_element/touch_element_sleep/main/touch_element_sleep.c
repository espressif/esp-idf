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
#include "touch_element/touch_button.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_pm.h"

static const char *TAG = "Touch Button Example";
#define TOUCH_BUTTON_NUM     5

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
            ESP_LOGI(TAG, "Button[%d] Press", (uint32_t)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
            ESP_LOGI(TAG, "Button[%d] Release", (uint32_t)element_message.arg);
        } else if (button_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
            ESP_LOGI(TAG, "Button[%d] LongPress", (uint32_t)element_message.arg);
        }
    }
}
#elif CONFIG_TOUCH_ELEM_CALLBACK
/* Button callback routine */
static void button_handler(touch_button_handle_t out_handle, touch_button_message_t *out_message, void *arg)
{
    (void) out_handle; //Unused
    if (out_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
        ESP_LOGI(TAG, "Button[%d] Press", (uint32_t)arg);
        if (out_handle == button_handle[0]) {
//            esp_deep_sleep_start();
        } else if (out_handle == button_handle[1]) {
            esp_deep_sleep_start();
        }
    } else if (out_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
        ESP_LOGI(TAG, "Button[%d] Release", (uint32_t)arg);
    } else if (out_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
        ESP_LOGI(TAG, "Button[%d] LongPress", (uint32_t)arg);
    }
}
#endif

void app_main(void)
{
//    esp_pm_config_esp32s2_t pm_config = {
//        .max_freq_mhz = 160,
//        .min_freq_mhz = 160,
//        .light_sleep_enable = true
//    };
//    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
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
        ESP_ERROR_CHECK(touch_button_subscribe_event(button_handle[i], TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS,
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
        ESP_ERROR_CHECK(touch_button_set_longpress(button_handle[i], 1000));
    }
    ESP_LOGI(TAG, "Touch buttons created");
    touch_elem_sleep_config_t sleep_config = {
        .scan_time = global_config.hardware.sample_count,
        .sleep_time = global_config.hardware.sleep_cycle,
    };
    ESP_ERROR_CHECK(touch_element_sleep_install(&sleep_config));
    ESP_ERROR_CHECK(touch_element_sleep_add_wakeup(button_handle[0]));
    ESP_ERROR_CHECK(touch_element_sleep_config_wakeup_calibration(button_handle[0], true));
    touch_pad_sleep_channel_t sleep_channel_info;
    touch_pad_sleep_channel_get_info(&sleep_channel_info);
    printf("----------%d\n", sleep_channel_info.touch_num);
    touch_element_start();
    ESP_LOGI(TAG, "Touch element library start");
    vTaskDelay(pdMS_TO_TICKS(1000));
}
