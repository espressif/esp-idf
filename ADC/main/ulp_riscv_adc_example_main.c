#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_adc/adc_continuous.h"

#define TAG "ADC_POT_DMA"
#define ADC_CHANNEL ADC_CHANNEL_2 // GPIO2
#define ADC_ATTEN ADC_ATTEN_DB_11 // 11dB attenuation for 3.3V range
#define MAX_ADC_READINGS 512      // Size of buffer for DMA

static adc_continuous_handle_t adc_handle;

void configure_adc_dma()
{
    // ADC continuous configuration
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = MAX_ADC_READINGS, // Buffer size
        .conv_frame_size = MAX_ADC_READINGS,    // Size of each frame of data collected by DMA
    };

    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle));

    // Configure ADC channels
    adc_continuous_config_t adc_chan_config = {
        .sample_freq_hz = 10000, // Set the sampling frequency (10kHz here)
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,   // Use ADC1
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1, // Set the output format
    };

    adc_digi_pattern_table_t adc_pattern = {
        .atten = ADC_ATTEN,             // Set attenuation for the channel
        .channel = ADC_CHANNEL,         // Set the channel (GPIO2)
        .bit_width = ADC_WIDTH_BIT_12,  // 12-bit resolution
    };

    // Apply the configuration to the ADC
    adc_chan_config.adc_pattern = &adc_pattern;
    adc_chan_config.pattern_num = 1;

    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &adc_chan_config));
    ESP_LOGI(TAG, "ADC DMA Configuration complete!");
}

void adc_dma_task(void *arg)
{
    uint8_t adc_raw[MAX_ADC_READINGS] = {0};  // Buffer to store ADC data
    uint32_t ret_num = 0;                     // To store the number of bytes read

    // Start the ADC in continuous mode
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));

    while (1)
    {
        // Read data from ADC DMA buffer
        esp_err_t result = adc_continuous_read(adc_handle, adc_raw, MAX_ADC_READINGS, &ret_num, 1000);
        if (result == ESP_OK)
        {
            for (int i = 0; i < ret_num / sizeof(adc_digi_output_data_t); i++)
            {
                adc_digi_output_data_t *adc_data = (adc_digi_output_data_t *)&adc_raw[i * sizeof(adc_digi_output_data_t)];
                ESP_LOGI(TAG, "ADC Raw: %d, Voltage: %f", adc_data->type1.data, ((adc_data->type1.data * 3.3) / 4095));
            }
        }
        else
        {
            ESP_LOGW(TAG, "ADC DMA read error");
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Wait for 100 ms
    }

    // Stop the ADC when task is deleted
    ESP_ERROR_CHECK(adc_continuous_stop(adc_handle));
}

void app_main(void)
{
    configure_adc_dma();   // Configure the ADC with DMA

    // Start the task to handle ADC DMA readings
    xTaskCreate(adc_dma_task, "adc_dma_task", 2048, NULL, 5, NULL);
}
