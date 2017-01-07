/* Ledc fade example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "esp_attr.h"   
#include "esp_err.h"

/*
 * About this example
 * 1. init LEDC module:
 *    a. You need to set the timer of LEDC first, this decide the frequency and resolution of PWM.
 *    b. You need to set the LEDC channel you want to use, and bind with one of the timers.
 *
 * 2. You can install a default fade function, then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example use GPIO18/19/4/5 as LEDC ouput, and it will change the duty repeatedly.
 *
 *
 */

#define LEDC_IO_0    (18)
#define LEDC_IO_1    (19)
#define LEDC_IO_2    (4)
#define LEDC_IO_3    (5)

esp_err_t app_main() 
{	
    ledc_timer_config_t ledc_timer = {
        //set timer counter bit number
        .bit_num = LEDC_TIMER_13_BIT,
        //set frequency of pwm
        .freq_hz = 5000,
        //timer mode,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        //timer index
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        //set LEDC channel 0
        .channel = LEDC_CHANNEL_0,
        //set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
        .duty = 100,
        //GPIO number
        .gpio_num = LEDC_IO_0,
        //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
        .intr_type = LEDC_INTR_FADE_END,
        //set LEDC mode, from ledc_mode_t
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        //set LEDC timer source, if different channel use one timer,
        //the frequency and bit_num of these channels should be the same
        .timer_sel = LEDC_TIMER_0
    };
    //set the configuration
    ledc_channel_config(&ledc_channel);

    //config ledc channel1
    ledc_channel.channel = LEDC_CHANNEL_1;
    ledc_channel.gpio_num = LEDC_IO_1;
    ledc_channel_config(&ledc_channel);
    //config ledc channel2
    ledc_channel.channel = LEDC_CHANNEL_2;
    ledc_channel.gpio_num = LEDC_IO_2;
    ledc_channel_config(&ledc_channel);
    //config ledc channel3
    ledc_channel.channel = LEDC_CHANNEL_3;
    ledc_channel.gpio_num = LEDC_IO_3;
    ledc_channel_config(&ledc_channel);

    //initialize fade service.
    ledc_fade_func_install(0);

    while(1) {
        printf("LEDC fade up\n");
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 1000, 2000);
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 7000, 2000);
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 5000, 2000);
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 3000, 2000);
        ledc_fade_start(LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        ledc_fade_start(LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
        ledc_fade_start(LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
        ledc_fade_start(LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        printf("LEDC fade down\n");
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 100, 2000);
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 300, 2000);
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 500, 2000);
        ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 700, 2000);
        ledc_fade_start(LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        ledc_fade_start(LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
        ledc_fade_start(LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
        ledc_fade_start(LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        printf("LEDC set duty without fade\n");
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 1000);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 7000);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 5000);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 3000);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        printf("LEDC set duty without fade\n");
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
