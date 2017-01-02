/* Sigma-delta Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/sigmadelta.h"

/*
 * This test code intended to configure sigma-delta and set GPIO4 as signal output pin.
 * If you connect this GPIO4 with an LED, you will see the LED blinking slowly.
 */

/**
 * @brief Sigma-delta initialization.
 */
static void sigmadelta_init(void)
{
   sigmadelta_config_t sigmadelta_cfg = {
       /* Sigma-delta channel0*/
       .channel = SIGMADELTA_CHANNEL_0,
       /* Sigma-delta set duty 10*/
       .sigmadelta_duty = 10,
       /* Set prescale 30 */
       .sigmadelta_prescale = 80,
       /*select GPIO4 as output_io */
       .sigmadelta_gpio = 4,
     };
   sigmadelta_config(&sigmadelta_cfg);
}

/**
 * @brief Sigma-delta test to change duty of output signal.
 */
void app_main()
{
    sigmadelta_init();
    int8_t duty = 0;
    int inc = 1;
    while(1) {
        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, duty);
        /*by changing delay time, you can change the blink frequency of LED. */
        vTaskDelay(10 / portTICK_PERIOD_MS);

        duty += inc;
        if(duty == 127 || duty == -127) inc = (-1) * inc;
    }
}
