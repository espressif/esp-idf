#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO      (3)                  // Define the output GPIO (pin 3)
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_DUTY_RES       LEDC_TIMER_13_BIT    // Set duty resolution to 13 bits
#define LEDC_MAX_DUTY       ((1 << LEDC_DUTY_RES) - 1) // Max duty (2^13 - 1 = 8191)
#define LEDC_FREQUENCY      (4000)               // Frequency in Hertz (4 kHz)
#define DIMMING_DELAY_MS    10                   // Delay between duty cycle changes (in milliseconds)

static void example_ledc_init(void)
{
    // Configure the LEDC PWM timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,      // Set frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configure the LEDC PWM channel
    ledc_channel_config_t ledc_channel = {
        .speed_mode       = LEDC_MODE,
        .channel          = LEDC_CHANNEL,
        .timer_sel        = LEDC_TIMER,
        .intr_type        = LEDC_INTR_DISABLE,
        .gpio_num         = LEDC_OUTPUT_IO,
        .duty             = 0,                   // Start with 0% duty cycle (off)
        .hpoint           = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


void app_main(void)
{
    // Initialize the LEDC peripheral
    example_ledc_init();

    while (1) {
        // Gradually increase brightness (0% to 100%)
        for (int duty = 10; duty <= LEDC_MAX_DUTY; duty += 50) {
            // Set new duty cycle
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
            // printing the updated duty cycle in percentage
            printf("Updated duty cycle: %d \n", duty);
            // Apply the updated duty cycle
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
            // Add a small delay to control the dimming speed
            vTaskDelay(DIMMING_DELAY_MS / portTICK_PERIOD_MS);
        }

        // Gradually decrease brightness (100% to 0%)
        for (int duty = LEDC_MAX_DUTY; duty >= 10; duty -= 50) {
            // Set new duty cycle
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
            // printing the updated duty cycle in percentage
            printf("Updated duty cycle: %d\n", duty);
            // Apply the updated duty cycle
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
            // Add a small delay to control the dimming speed
            vTaskDelay(DIMMING_DELAY_MS / portTICK_PERIOD_MS);
        }
    }
}
