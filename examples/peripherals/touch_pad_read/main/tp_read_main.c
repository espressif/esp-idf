/* Touch Pad Read Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"

#define TOUCH_TEST_LOOP_NUM   (10)
#define TOUCH_PAD_NO_CHANGE   (-1)
#define TOUCH_THRESH_NO_USE   (0)
/*
  Read values sensed at all available touch pads.
 Print out values in a loop on a serial monitor.
 */
static void tp_example_read_task(void *pvParameter)
{
    while (1) {
        uint16_t touch_value;
        //set reference voltage for charging/discharging
        // In this case, the high reference valtage will be 2.7V - 0V = 2.7V
        // The low reference voltage will be 0.5
        // So the charing/discharging time would be longer, so the counter value would be smaller.
        touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V);

        vTaskDelay(100 / portTICK_PERIOD_MS);
        printf("Case[1], set default measure time\n");
        for (int j = 0; j < TOUCH_TEST_LOOP_NUM; j++) {
            for (int i = 0; i < TOUCH_PAD_MAX; i++) {
                ESP_ERROR_CHECK(touch_pad_read(i, &touch_value));
                printf("T%d:%5d ", i, touch_value);
            }
            printf("\n");
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        printf("Case[2], set max measure time\n");
        //set reference voltage for charging/discharging
        // In this case, the high reference valtage will be 2.4V - 1.5V = 0.9V
        // The low reference voltage will be 0.8
        // So the charing/discharging time would be shorter, so the counter value would be larger.
        touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V8, TOUCH_HVOLT_ATTEN_1V5);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        for (int j = 0; j < TOUCH_TEST_LOOP_NUM; j++) {
            for (int i = 0; i < TOUCH_PAD_MAX; i++) {
                ESP_ERROR_CHECK(touch_pad_read(i, &touch_value));
                printf("T%d:%5d ", i, touch_value);
            }
            printf("\n");
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }


        touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V);
        vTaskDelay(100/portTICK_PERIOD_MS);

        printf("Case[3], set differen slope for each channel\n");
        for (int i = 0;i<TOUCH_PAD_MAX;i++) {
            touch_pad_set_cnt_mode(i, (i % TOUCH_PAD_SLOPE_7) + 1, TOUCH_PAD_TIE_OPT_HIGH);
        }
        for (int j = 0; j < TOUCH_TEST_LOOP_NUM; j++) {
            for (int i = 0; i < TOUCH_PAD_MAX; i++) {
                ESP_ERROR_CHECK(touch_pad_read(i, &touch_value));
                printf("T%d:%5d ", i, touch_value);
            }
            printf("\n");
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        for (int i = 0;i<TOUCH_PAD_MAX;i++) {
            touch_pad_set_cnt_mode(i, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_HIGH);
        }
    }
}

static void tp_example_touch_pad_init()
{
    for (int i = 0;i< TOUCH_PAD_MAX;i++) {
        touch_pad_config(i, TOUCH_THRESH_NO_USE);
    }
}

void app_main()
{
    // Initialize touch pad peripheral
    touch_pad_init();
    tp_example_touch_pad_init();

    // Start task to read values sensed by pads
    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);

}
