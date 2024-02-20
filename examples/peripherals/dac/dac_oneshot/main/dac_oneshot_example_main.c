/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac_oneshot.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_check.h"

#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_DAC_CHAN0_ADC_CHAN          ADC_CHANNEL_8   // GPIO25, same as DAC channel 0
#define EXAMPLE_DAC_CHAN1_ADC_CHAN          ADC_CHANNEL_9   // GPIO26, same as DAC channel 1
#define EXAMPLE_ADC_WIDTH                   ADC_WIDTH_BIT_12
#elif CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_DAC_CHAN0_ADC_CHAN          ADC_CHANNEL_6   // GPIO17, same as DAC channel 0
#define EXAMPLE_DAC_CHAN1_ADC_CHAN          ADC_CHANNEL_7   // GPIO18, same as DAC channel 1
#define EXAMPLE_ADC_WIDTH                   ADC_WIDTH_BIT_13
#endif
#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_12

static void adc_monitor_task(void *args)
{
    adc_oneshot_unit_handle_t adc2_handle = (adc_oneshot_unit_handle_t)args;
    int chan0_val = 0;
    int chan1_val = 0;
    while (1) {
        /* Read the DAC output voltage */
        ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, EXAMPLE_DAC_CHAN0_ADC_CHAN, &chan0_val));
        ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, EXAMPLE_DAC_CHAN1_ADC_CHAN, &chan1_val));
        printf("DAC channel 0 value: %4d\tDAC channel 1 value: %4d\n", chan0_val, chan1_val);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void dac_output_task(void *args)
{
    dac_oneshot_handle_t handle = (dac_oneshot_handle_t)args;
    uint32_t val = 0;
    while (1) {
        /* Set the voltage every 100 ms */
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(handle, val));
        val += 10;
        val %= 250;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    /* DAC oneshot init */
    dac_oneshot_handle_t chan0_handle;
    dac_oneshot_config_t chan0_cfg = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&chan0_cfg, &chan0_handle));

    dac_oneshot_handle_t chan1_handle;
    dac_oneshot_config_t chan1_cfg = {
        .chan_id = DAC_CHAN_1,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&chan1_cfg, &chan1_handle));

    /* DAC oneshot outputting threads */
    xTaskCreate(dac_output_task, "dac_chan0_output_task", 4096, chan0_handle, 5, NULL);
    vTaskDelay(pdMS_TO_TICKS(500)); // To differential the output of two channels
    xTaskCreate(dac_output_task, "dac_chan1_output_task", 4096, chan1_handle, 5, NULL);

    /* ADC init, these channels are connected to the DAC channels internally */
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t adc_cfg = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = false,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_cfg, &adc2_handle));
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = EXAMPLE_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, EXAMPLE_DAC_CHAN0_ADC_CHAN, &chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, EXAMPLE_DAC_CHAN1_ADC_CHAN, &chan_cfg));
    /* ADC monitor thread */
    xTaskCreate(adc_monitor_task, "adc_monitor_task", 4096, adc2_handle, 5, NULL);
}
