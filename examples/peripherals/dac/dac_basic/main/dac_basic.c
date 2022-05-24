/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac_driver.h"
#include "driver/adc.h"
#include "esp_check.h"

#define EXAMPLE_DAC_USE_SEPARATE_CHANNEL    0   // Whether to register two DAC channels in separate control group

#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_DAC_CHAN1_ADC_CHAN          ADC2_CHANNEL_8   // GPIO25, same as DAC channel 0
#define EXAMPLE_DAC_CHAN2_ADC_CHAN          ADC2_CHANNEL_9   // GPIO26, same as DAC channel 1
#define EXAMPLE_ADC_WIDTH                   ADC_WIDTH_BIT_12
#elif CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_DAC_CHAN1_ADC_CHAN          ADC2_CHANNEL_6   // GPIO17, same as DAC channel 0
#define EXAMPLE_DAC_CHAN2_ADC_CHAN          ADC2_CHANNEL_7   // GPIO18, same as DAC channel 1
#define EXAMPLE_ADC_WIDTH                   ADC_WIDTH_BIT_13
#endif
#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_11

static void adc_monitor_task(void *args)
{
    /* Set the ADC channels, these channels are connected to the DAC channels internally */
    ESP_ERROR_CHECK(adc2_config_channel_atten(EXAMPLE_DAC_CHAN1_ADC_CHAN, EXAMPLE_ADC_ATTEN));
    ESP_ERROR_CHECK(adc2_config_channel_atten(EXAMPLE_DAC_CHAN2_ADC_CHAN, EXAMPLE_ADC_ATTEN));

    int chan1_val = 0;
    int chan2_val = 0;
    while (1) {
        /* Read the DAC output voltage */
        ESP_ERROR_CHECK(adc2_get_raw(EXAMPLE_DAC_CHAN1_ADC_CHAN, EXAMPLE_ADC_WIDTH, &chan1_val));
        ESP_ERROR_CHECK(adc2_get_raw(EXAMPLE_DAC_CHAN2_ADC_CHAN, EXAMPLE_ADC_WIDTH, &chan2_val));
        printf("DAC channel 0 vaule: %4d\tDAC channel 1 vaule: %4d\n", chan1_val, chan2_val);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void dac_output_task(void *args)
{
    dac_channels_handle_t handle = (dac_channels_handle_t)args;
    uint32_t val = 0;
    while (1) {
        /* Set the voltage every 100 ms */
        ESP_ERROR_CHECK(dac_channels_set_voltage(handle, val));
        val += 10;
        val %= 250;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void dac_init_channel(dac_channel_mask_t mask, dac_channels_handle_t *dac_handle)
{
    dac_channels_handle_t handle = NULL;
    dac_channels_config_t cfg = {
        .chan_sel = mask,
    };
    /* Allocate the channel group */
    ESP_ERROR_CHECK(dac_new_channels(&cfg, &handle));
    /* Enable the channels in the group */
    ESP_ERROR_CHECK(dac_channels_enable(handle));
    *dac_handle = handle;
}

static void __attribute__((unused)) dac_deinit_channel(dac_channels_handle_t dac_handle)
{
    /* Disable the DAC channels */
    ESP_ERROR_CHECK(dac_channels_disable(dac_handle));
    /* Delete the channel group */
    ESP_ERROR_CHECK(dac_del_channels(dac_handle));
}

void app_main(void)
{
#if EXAMPLE_DAC_USE_SEPARATE_CHANNEL
    dac_channels_handle_t chan1_handle;
    dac_channels_handle_t chan2_handle;
    /* Initialize the two channels separately */
    dac_init_channel(DAC_CHANNEL_MASK_CH0, &chan1_handle);
    dac_init_channel(DAC_CHANNEL_MASK_CH1, &chan2_handle);
    xTaskCreate(dac_output_task, "dac_chan1_output_task", 4096, chan1_handle, 5, NULL);
    vTaskDelay(pdMS_TO_TICKS(500)); // To differential the output of two channels
    xTaskCreate(dac_output_task, "dac_chan2_output_task", 4096, chan2_handle, 5, NULL);
    xTaskCreate(adc_monitor_task, "adc_monitor_task", 4096, NULL, 5, NULL);
#else
    dac_channels_handle_t chan12_handle;
    /* Initialize the two channels in a same group */
    dac_init_channel(DAC_CHANNEL_MASK_BOTH, &chan12_handle);
    xTaskCreate(dac_output_task, "dac_output_task", 4096, chan12_handle, 5, NULL);
    xTaskCreate(adc_monitor_task, "adc_monitor_task", 4096, NULL, 5, NULL);
#endif
}
