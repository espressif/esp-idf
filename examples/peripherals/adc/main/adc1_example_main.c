/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "esp_adc_cal.h"

/*Note: Different ESP32 modules may have different reference voltages varying from
 * 1000mV to 1200mV. Use #define GET_VREF to route v_ref to a GPIO
 */
#define V_REF   1100
#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_6)      //GPIO 34
//#define V_REF_TO_GPIO  //Remove comment on define to route v_ref to GPIO

void app_main(void)
{
#ifndef V_REF_TO_GPIO
    //Init ADC and Characteristics
    esp_adc_cal_characteristics_t characteristics;
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_0db);
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_0db, ADC_WIDTH_12Bit, &characteristics);
    uint32_t voltage;
    while(1){
        voltage = adc1_to_voltage(ADC1_TEST_CHANNEL, &characteristics);
        printf("%d mV\n",voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
#else
    //Get v_ref
    esp_err_t status;
    status = adc2_vref_to_gpio(GPIO_NUM_25);
    if (status == ESP_OK){
        printf("v_ref routed to GPIO\n");
    }else{
        printf("failed to route v_ref\n");
    }
    fflush(stdout);
#endif
}
