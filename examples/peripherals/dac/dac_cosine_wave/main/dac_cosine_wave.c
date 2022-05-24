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

#define EXAMPLE_DAC_USE_SEPARATE_CHANNEL    1   // Whether to register two DAC channels in separate control group

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

static void dac_init_channel(dac_channel_mask_t mask, dac_cosine_config_t *cos_cfg, dac_channels_handle_t *dac_handle)
{
    dac_channels_handle_t handle = NULL;
    dac_channels_config_t cfg = {
        .chan_sel = mask,
    };
    /* Allocate the channel group */
    ESP_ERROR_CHECK(dac_new_channels(&cfg, &handle));
    /* Enable the channels in the group */
    ESP_ERROR_CHECK(dac_channels_enable(handle));
    /* Initialize DAC cosine wave genarator */
    ESP_ERROR_CHECK(dac_channels_init_cosine_mode(handle, cos_cfg));
    /* Start to output the cosine wave */
    ESP_ERROR_CHECK(dac_channels_start_cosine_output(handle));

    *dac_handle = handle;
}

static void __attribute__((unused)) dac_deinit_channel(dac_channels_handle_t dac_handle)
{
    /* Stop outputting the cosine wave */
    ESP_ERROR_CHECK(dac_channels_stop_cosine_output(dac_handle));
    /* Deinitialize the cosine wave genarator */
    ESP_ERROR_CHECK(dac_channels_deinit_cosine_mode(dac_handle));
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
    /* Adopt different cosine wave configuration to the two DAC channel
     * But take care that the two channels can only be configured to one frequence
     * If they are differt, the latter frequency will cover the former one
     * The example here will produce cosine wave at 8 KHz on both channels */
    dac_cosine_config_t cos1_cfg = {
        .freq_hz = 1000, // It will be covered by 8000 in the latter configuration
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .scale = DAC_COSINE_NO_ATTEN,
    };
    dac_cosine_config_t cos2_cfg = {
        .freq_hz = 8000,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_180,
        .scale = DAC_COSINE_ATTEN_2,
    };
    dac_init_channel(DAC_CHANNEL_MASK_CH0, &cos1_cfg, &chan1_handle);
    dac_init_channel(DAC_CHANNEL_MASK_CH1, &cos2_cfg, &chan2_handle);
    xTaskCreate(adc_monitor_task, "adc_monitor_task", 4096, NULL, 5, NULL);
#else
    dac_channels_handle_t chan12_handle;
    dac_cosine_config_t cos_cfg = {
        .freq_hz = 1000, // 130 Hz ~ 200 KHz
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .scale = DAC_COSINE_NO_ATTEN,
    };
    dac_init_channel(DAC_CHANNEL_MASK_BOTH, &cos_cfg, &chan12_handle);
    xTaskCreate(adc_monitor_task, "adc_monitor_task", 4096, NULL, 5, NULL);
#endif
}
