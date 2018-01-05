/* Stepper Motor Control DRV8825 with Step Count Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_sig_map.h"

/**
 * TEST CODE BRIEF
 * Stepper Motor Control Library for DRV8825
 * GPIO18 is used as output pin generates reqired pulse frequencies. The frequency is equal to the steps per second of the motor.
 */

#define LEDC_OUTPUT_IO_0    18 /* Output GPIO */
#define STEPPER_CONTROL_LOOP_RATE 100

ledc_timer_config_t ledc_timer;
int8_t motorstopped=0, motordirection=0;
int32_t motorfrequency=0;

static void ledc_init(void)
{

    ledc_channel_config_t ledc_channel;
    /*use LEDC_OUTPUT_IO as output pin*/
    ledc_channel.gpio_num = LEDC_OUTPUT_IO_0;
    /*LEDC high speed mode */
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    /*use LEDC channel 0*/
    ledc_channel.channel = LEDC_CHANNEL_0;
    /*Disable LEDC interrupt*/
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    /*Select LEDC timer 0 */
    ledc_channel.timer_sel = LEDC_TIMER_0;
    /*Set duty 100 */
    ledc_channel.duty = 10;
    ledc_channel_config(&ledc_channel); //ledc channel 0 config motor GPIO 18

    //ledc_timer_config_t ledc_timer;
    /*LEDC timer high speed mode*/
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    /*10 bit PWM*/
    ledc_timer.bit_num = LEDC_TIMER_10_BIT;
    /*Select timer 0*/
    ledc_timer.timer_num = LEDC_TIMER_0;
    /*Set frequency 1 Hz  */
    ledc_timer.freq_hz = 25;
    ledc_timer_config(&ledc_timer);
}


void stepper_task(void *pvParameter)
{
    // Stepper Control Task
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = STEPPER_CONTROL_LOOP_RATE;
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    while(1) {
        if(motorfrequency==0){
            ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
            motorstopped=1;
        }
        else{
            if(motorstopped==1){
            ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
            motorstopped=0;
            }
            ledc_timer.freq_hz = motorfrequency;
            ledc_timer_config(&ledc_timer);
        }
        vTaskDelayUntil( &xLastWakeTime, xFrequency );// Wait for the next cycle.
    }
}

void app_main()
{
    /*Init LEDC for pulse signal */
    ledc_init();
    xTaskCreate(&stepper_task, "stepper_task", 4096, NULL, 5, NULL);

    while(1)
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        motorfrequency=motorfrequency+50;
        if(motorfrequency>200)
        {
            motorfrequency=0;
        }
        printf("Current frequency value :%d\n", motorfrequency);
    }
}
