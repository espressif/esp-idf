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
 * This test code will configure sigma-delta and set GPIO4 as a signal output pin.
 * If you connect this GPIO4 with a LED, you will see the LED blinking slowly.
 */

/*
 * Configure and initialize the sigma delta modulation
 * on channel 0 to output signal on GPIO4
 */
static void sigmadelta_example_init(void)
{
    sigmadelta_config_t sigmadelta_cfg = {
        .channel = SIGMADELTA_CHANNEL_0,
        .sigmadelta_prescale = 80,
        .sigmadelta_duty = 0,
        .sigmadelta_gpio = 4,
    };
    sigmadelta_config(&sigmadelta_cfg);
}

/*
 *  Perform the sigma-delta modulation test
 *  by changing the duty of the output signal.
 */
void app_main()
{
    sigmadelta_example_init();

    int8_t duty = 0;
    int inc = 1;
    while (1) {
        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, duty);
        /* By changing delay time, you can change the blink frequency of LED */
        vTaskDelay(10 / portTICK_PERIOD_MS);

        duty += inc;
        if (duty == 127 || duty == -127) {
            inc = (-1) * inc;
        }
    }
}
