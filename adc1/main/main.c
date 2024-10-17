#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#define TAG "ADC_POT"
#define ADC_CHANNEL ADC1_CHANNEL_2 // GPIO2
#define ADC_ATTEN ADC_ATTEN_DB_11  // 11dB attenuation for 3.3V range



void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);               // 12-bit resolution
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN); // 11dB attenuation for 0-3.3V input range

    while (1)
    {
        int raw_adc = adc1_get_raw(ADC_CHANNEL);
        ESP_LOGI(TAG, "ADC: %d ,voltage: %f", raw_adc, ((raw_adc * 3.3) / 4095));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
