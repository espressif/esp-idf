/* ADC2 Example

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
#include "driver/dac.h"
#include "esp_system.h"

#define DAC_EXAMPLE_CHANNEL     CONFIG_EXAMPLE_DAC_CHANNEL
#define ADC2_EXAMPLE_CHANNEL    CONFIG_EXAMPLE_ADC2_CHANNEL

void app_main(void)
{
    uint8_t output_data=0;
    int     read_raw;
    esp_err_t r;

    gpio_num_t adc_gpio_num, dac_gpio_num;

    r = adc2_pad_get_io_num( ADC2_EXAMPLE_CHANNEL, &adc_gpio_num );
    assert( r == ESP_OK );
    r = dac_pad_get_io_num( DAC_EXAMPLE_CHANNEL, &dac_gpio_num );
    assert( r == ESP_OK );

    printf("ADC channel %d @ GPIO %d, DAC channel %d @ GPIO %d.\n", ADC2_EXAMPLE_CHANNEL, adc_gpio_num,
                DAC_EXAMPLE_CHANNEL, dac_gpio_num );

    dac_output_enable( DAC_EXAMPLE_CHANNEL );

    //be sure to do the init before using adc2. 
    printf("adc2_init...\n");
    adc2_config_channel_atten( ADC2_EXAMPLE_CHANNEL, ADC_ATTEN_0db );

    vTaskDelay(2 * portTICK_PERIOD_MS);

    printf("start conversion.\n");
    while(1) {
        dac_output_voltage( DAC_EXAMPLE_CHANNEL, output_data++ );
        r = adc2_get_raw( ADC2_EXAMPLE_CHANNEL, ADC_WIDTH_12Bit, &read_raw);
        if ( r == ESP_OK ) {
            printf("%d: %d\n", output_data, read_raw );
        } else if ( r == ESP_ERR_INVALID_STATE ) {
            printf("%s: ADC2 not initialized yet.\n", esp_err_to_name(r));
        } else if ( r == ESP_ERR_TIMEOUT ) {
            //This can not happen in this example. But if WiFi is in use, such error code could be returned.
            printf("%s: ADC2 is in use by Wi-Fi.\n", esp_err_to_name(r));
        } else {
            printf("%s\n", esp_err_to_name(r));
        }

        vTaskDelay( 2 * portTICK_PERIOD_MS );
    }
}
