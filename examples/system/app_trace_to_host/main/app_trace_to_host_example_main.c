/* Application Trace to Host Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_app_trace.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/dac_cosine.h"
#include "soc/adc_channel.h"
#include "soc/dac_channel.h"

#define ADC1_TEST_CHANNEL       (ADC_CHANNEL_6)
#define EXAMPLE_DAC_CHENNEL     DAC_CHAN_0

#define TEST_SAMPLING_PERIOD    20

static const char *TAG = "example";

/*
 * Enable cosine waveform generator (CW)
 * on DAC channel 0 to provide sinusoidal signal
 * It can be used instead of a live signal for testing
 * of speed of logging to the host
 * sequentially with data retrieval from ADC
 */
static void enable_cosine_generator(void)
{
    dac_cosine_handle_t dac_handle;
    dac_cosine_config_t cos_cfg = {
        .chan_id = EXAMPLE_DAC_CHENNEL,
        .freq_hz = 130,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .atten = DAC_COSINE_ATTEN_DEFAULT,
        .flags.force_set_freq = false,
    };
    ESP_ERROR_CHECK(dac_cosine_new_channel(&cos_cfg, &dac_handle));
    ESP_ERROR_CHECK(dac_cosine_start(dac_handle));
}

/*
 * Sample data an ADC1 channel 6
 * over specific 'sampling_period' in milliseconds.
 * Print out sampling result using standard ESP_LOGI() function.
 * Return the number of samples collected.
 */
static int adc1_sample_and_show(adc_oneshot_unit_handle_t adc1_handle, int sampling_period)
{
    int i = 0;
    uint32_t sampling_start = esp_log_timestamp();
    do {
        int adc_raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHANNEL, &adc_raw));
        ESP_LOGI(TAG, "Sample:%d, Value:%d", ++i, adc_raw);
    } while (esp_log_timestamp() - sampling_start < sampling_period);
    return i;
}

/*
 * Main program loop that is sampling data on ADC
 * and logging results with application tracing to the host
 * as well as for comparison printing out sampling result to UART
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Enabling ADC1 on channel 6 / GPIO%d.", ADC1_CHANNEL_6_GPIO_NUM);

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Channel Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_TEST_CHANNEL, &config));

    ESP_LOGI(TAG, "Enabling CW generator on DAC channel 0 / GPIO%d.", DAC_CHAN0_GPIO_NUM);
    enable_cosine_generator();

    while (1) {
        /*
         * Logging with the Application Trace
         */
        ESP_LOGI(TAG, "Sampling ADC and sending data to the host...");
        // Route LOGx() to the host
        esp_log_set_vprintf(esp_apptrace_vprintf);
        int samples_collected = adc1_sample_and_show(adc1_handle, TEST_SAMPLING_PERIOD);
        // Route LOGx() back to UART
        esp_log_set_vprintf(vprintf);
        // Flush collected data to the host
        esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, 100000);
        ESP_LOGI(TAG, "Collected %d samples in %d ms.\n", samples_collected, TEST_SAMPLING_PERIOD);

        /*
         * Logging to UART
         */
        ESP_LOGI(TAG, "Sampling ADC and sending data to the UART...");
        samples_collected = adc1_sample_and_show(adc1_handle, TEST_SAMPLING_PERIOD);
        ESP_LOGI(TAG, "Collected %d samples in %d ms.\n", samples_collected, TEST_SAMPLING_PERIOD);

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
}
