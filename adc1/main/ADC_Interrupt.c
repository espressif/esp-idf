#include <stdio.h>
#include "driver/adc.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ADC-Interrupt";

// ADC configuration
#define ADC_CHANNEL ADC_CHANNEL_0     // GPIO4 for ADC1
#define ADC_WIDTH   ADC_WIDTH_BIT_12  // 12-bit width

// ADC reading variable
volatile int adc_value = 0;

// ADC interrupt callback
void adc_timer_callback(void* arg) {
    // Trigger an ADC reading
    adc_value = adc1_get_raw(ADC_CHANNEL);
    ESP_LOGI(TAG, "ADC Value: %d,  Voltage: %.2fV", adc_value, ((adc_value * 3.3) / 4095));
}

// Function to initialize ADC
static void init_adc() {
    ESP_LOGI(TAG, "Initializing ADC...");
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);  // 0dB attenuation (0-3.3V range)
}

// Function to initialize timer
static void init_timer() {
    ESP_LOGI(TAG, "Initializing Timer...");

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &adc_timer_callback,
        .name = "adc_timer"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000));  // 1 second (1,000,000 microseconds)
}

void app_main(void) {
    ESP_LOGI(TAG, "Application Started");

    // Initialize ADC
    init_adc();

    // Initialize Timer to trigger ADC readings
    init_timer();

    while (1) {
        // Main loop (could be used for other tasks)
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay
    }
}
