/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_check.h"
#include "dac_continuous.h"

/**
 *  There are two ways to convert digital data to analog signal continuously:
 *  - Using a timer: setting DAC voltage periodically in the timer interrupt
 *                   in this way, DAC can achieve a relatively low conversion frequency
 *                   but it is not a efficient way comparing to using the DMA
 *  - Using DMA: tansmitting the data buffer via DMA,
 *               the conversion frequency is controlled by how fast it is transmitted by DMA
 *               in this way, the conversion frequency can reach sevral MHz,
 *               but it can't achieve a very low conversion frequency because it is limited by the DMA clock source
 *  Generally, recommand to use DMA, if the DMA peripheral is occupied or the required conversion frequency is very low,
 *  then use timer instead
 */
#define EXAMPLE_DAC_CONTINUOUS_BY_TIMER     0
#define EXAMPLE_DAC_CONTINUOUS_BY_DMA       1
#define EXAMPLE_DAC_CONTINUOUS_MODE         EXAMPLE_DAC_CONTINUOUS_BY_DMA

/* ADC configuration */
#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_DAC_CHAN0_ADC_CHAN          ADC2_CHANNEL_8   // GPIO25, same as DAC channel 0
#define EXAMPLE_DAC_CHAN1_ADC_CHAN          ADC2_CHANNEL_9   // GPIO26, same as DAC channel 1
#define EXAMPLE_DAC_CHAN0_IO                (25)             // DAC channel 0 io number
#define EXAMPLE_DAC_CHAN1_IO                (26)             // DAC channel 1 io number
#define EXAMPLE_ADC_WIDTH                   ADC_WIDTH_BIT_12
#elif CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_DAC_CHAN0_ADC_CHAN          ADC2_CHANNEL_6   // GPIO17, same as DAC channel 0
#define EXAMPLE_DAC_CHAN1_ADC_CHAN          ADC2_CHANNEL_7   // GPIO18, same as DAC channel 1
#define EXAMPLE_DAC_CHAN0_IO                (17)             // DAC channel 0 io number
#define EXAMPLE_DAC_CHAN1_IO                (18)             // DAC channel 1 io number
#define EXAMPLE_ADC_WIDTH                   ADC_WIDTH_BIT_13
#endif
#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_11

_Static_assert(EXAMPLE_DAC_AMPLITUDE < 256, "The DAC accuracy is 8 bit-width, doesn't support the amplitude beyond 255");

static const char *TAG = "dac continuous";

void example_log_info(uint32_t conv_freq, uint32_t wave_freq)
{
    ESP_LOGI(TAG, "--------------------------------------------------");
    ESP_LOGI(TAG, "DAC continuous output by %s", EXAMPLE_DAC_CONTINUOUS_MODE ? "DMA" : "Timer");
    ESP_LOGI(TAG, "DAC channel 0 io: GPIO_NUM_%d", EXAMPLE_DAC_CHAN0_IO);
    ESP_LOGI(TAG, "DAC channel 1 io: GPIO_NUM_%d", EXAMPLE_DAC_CHAN1_IO);
    ESP_LOGI(TAG, "Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE");
    ESP_LOGI(TAG, "DAC conversion frequency (Hz): %d", conv_freq);
    ESP_LOGI(TAG, "DAC wave frequency (Hz): %d", wave_freq);
    ESP_LOGI(TAG, "--------------------------------------------------");
}

static void adc_monitor_task(void *args)
{
    /* Set the ADC channels, these channels are connected to the DAC channels internally */
    ESP_ERROR_CHECK(adc2_config_channel_atten(EXAMPLE_DAC_CHAN0_ADC_CHAN, EXAMPLE_ADC_ATTEN));
    ESP_ERROR_CHECK(adc2_config_channel_atten(EXAMPLE_DAC_CHAN1_ADC_CHAN, EXAMPLE_ADC_ATTEN));

    int chan1_val = 0;
    int chan2_val = 0;
    while (1) {
        /* Read the DAC output voltage */
        ESP_ERROR_CHECK(adc2_get_raw(EXAMPLE_DAC_CHAN0_ADC_CHAN, EXAMPLE_ADC_WIDTH, &chan1_val));
        ESP_ERROR_CHECK(adc2_get_raw(EXAMPLE_DAC_CHAN1_ADC_CHAN, EXAMPLE_ADC_WIDTH, &chan2_val));
        printf("DAC channel 0 vaule: %4d\tDAC channel 1 vaule: %4d\n", chan1_val, chan2_val);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
#if EXAMPLE_DAC_CONTINUOUS_MODE == EXAMPLE_DAC_CONTINUOUS_BY_DMA
    /* Output 2 kHz waves using DMA */
    dac_continuous_by_dma();
#else
    /* Output 50 Hz waves using timer interrupt */
    dac_continuous_by_timer();
#endif
    /* Create ADC monitor task to detect the voltage on DAC pin every 500 ms */
    xTaskCreate(adc_monitor_task, "adc_monitor_task", 4096, NULL, 5, NULL);
}
