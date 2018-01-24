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
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define PRINT_VAL_SUPPORT(support, type) ({                                 \
            if(support == ESP_OK){                                          \
                printf("%s: supported\n", (type));                          \
            } else {                                                        \
                printf("%s: not supported\n", (type));                      \
            }                                                               \
})

#define PRINT_VAL_TYPE(type, mode) ({                                       \
            if(type == ESP_ADC_CAL_VAL_EFUSE_TP){                           \
                printf("%s mode: Two Point Value\n", (mode));               \
            } else if (type == ESP_ADC_CAL_VAL_EFUSE_VREF){                 \
                printf("%s mode: eFuse Vref\n", (mode));                    \
            } else {                                                        \
                printf("%s mode: Default Vref\n", (mode));                  \
            }                                                               \
})

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64

static esp_adc_cal_characteristics_t *adc_linear_chars;
static esp_adc_cal_characteristics_t *adc_lut_chars;
static adc1_channel_t channel = ADC1_CHANNEL_6;

void app_main(void)
{
    //Check if Two Point or Vref are burned into eFuse
    esp_err_t efuse_vref_support = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
    esp_err_t efuse_tp_support = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP);
    PRINT_VAL_SUPPORT(efuse_vref_support, "eFuse Vref");
    PRINT_VAL_SUPPORT(efuse_tp_support, "eFuse Two Point");


    //Configure ADC1
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_0);

    //Characterize ADC1 in both linear and lut mode
    adc_linear_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    adc_lut_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));

    esp_adc_cal_value_t lin_val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ESP_ADC_CAL_MODE_LIN, DEFAULT_VREF, adc_linear_chars);
    esp_adc_cal_value_t lut_val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ESP_ADC_CAL_MODE_LUT, DEFAULT_VREF, adc_lut_chars);
    PRINT_VAL_TYPE(lin_val_type, "Linear");
    PRINT_VAL_TYPE(lut_val_type, "LUT");

    //Continuously sample ADC1
    while(1){
        uint32_t adc1_raw = 0;
        //Multisample
        for(int i = 0; i < NO_OF_SAMPLES; i++){
            adc1_raw += adc1_get_raw(channel);
        }
        adc1_raw /= NO_OF_SAMPLES;

        uint32_t corrected_linear = esp_adc_cal_raw_to_voltage(adc1_raw, ADC_WIDTH_BIT_12, adc_linear_chars);
        uint32_t corrected_lut = esp_adc_cal_raw_to_voltage(adc1_raw, ADC_WIDTH_BIT_12, adc_lut_chars);
        printf("Raw: %d\tLinear: %dmV\tLUT: %dmV\n", adc1_raw, corrected_linear, corrected_lut);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
