/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_etm.h"
#include "sdkconfig.h"

#define REFERENCE_WAVE_IO              (3) // Reference wave GPIO
#define GATE_TRIGGER_PULSE_IO          (2) // Gate trigger pulse GPIO
#define LEDC_TIMER                     LEDC_TIMER_0
#define LEDC_MODE                      LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL                   LEDC_CHANNEL_0
#define LEDC_DUTY_RES                  LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY                      (400) // Set duty to 400 out of 8192 (13-bit)
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32H4 || CONFIG_IDF_TARGET_ESP32H21
// On such targets. due to hardware limitation, LEDC timer reset ETM task won't sync newly configured duty parameters.
// Therefore, the LEDC timer frequency should be higher than 100 Hz (2 * 50 Hz) to ensure a timer overflow happens before the timer reset to update the duty parameters.
#define LEDC_FREQUENCY                 (120) // Frequency in Hertz. Set frequency at 120 Hz
#define LEDC_HPOINT_INITIAL            (1640) // Initial hpoint value (to ensure no pulse between timer overflow and timer reset)
#else
#define LEDC_FREQUENCY                 (80) // Frequency in Hertz. Set frequency at 80 Hz
#define LEDC_HPOINT_INITIAL            (0) // Initial hpoint value
#endif

// For C6/C5/H2/P4/H4/H21:
// 120Hz 8192-step PWM cycle in 100Hz -> (1640, 8192 - LEDC_DUTY) steps for a 84% ~ 21% dimming (this is the limitation due to the hardware)
// For other targets:
// 100Hz in a 80Hz 8192-step PWM cycle -> (0, 6554) steps for a 100% ~ 0% dimming
// Overall, we want to increase and decrease the brightness in 10 steps, so each step is:
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32H4 || CONFIG_IDF_TARGET_ESP32H21
#define LEDC_HPOINT_STEP               (615)
#else
#define LEDC_HPOINT_STEP               (655)
#endif

// Simulate a signal comes from the 50Hz mains AC supply
// Use GPIO + vTaskDelay to output a 50Hz square wave as the reference signal
static void reference_wave_init(void)
{
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << REFERENCE_WAVE_IO);
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}

static void reference_wave_task(void *arg)
{
    while (1) {
        // Set the reference wave high
        ESP_ERROR_CHECK(gpio_set_level(REFERENCE_WAVE_IO, 1));
        vTaskDelay(pdMS_TO_TICKS(10)); // High for 10ms

        // Set the reference wave low
        ESP_ERROR_CHECK(gpio_set_level(REFERENCE_WAVE_IO, 0));
        vTaskDelay(pdMS_TO_TICKS(10)); // Low for 10ms
    }
}

// Initialize the gate trigger pulse signal using LEDC
static void gate_trigger_pulse_signal_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .gpio_num       = GATE_TRIGGER_PULSE_IO,
        .duty           = LEDC_DUTY,
        .hpoint         = LEDC_HPOINT_INITIAL
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void app_main(void)
{
    gate_trigger_pulse_signal_init();
    vTaskDelay(pdMS_TO_TICKS(50)); // Wait for the LEDC to run for at least one cycle

    reference_wave_init();
    xTaskCreate(reference_wave_task, "reference_wave_task", 2048, NULL, 5, NULL);

    // Connect reference wave GPIO toggle event to LEDC timer reset task, so that the timing frame of the gate trigger pulse
    // can be synchronized to the reference wave signal
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel = NULL;
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_config, &etm_channel));

    esp_etm_event_handle_t gpio_event = NULL;
    gpio_etm_event_config_t gpio_event_config = {
        .edge = GPIO_ETM_EVENT_EDGE_ANY,
    };
    ESP_ERROR_CHECK(gpio_new_etm_event(&gpio_event_config, &gpio_event));
    ESP_ERROR_CHECK(gpio_etm_event_bind_gpio(gpio_event, REFERENCE_WAVE_IO));

    esp_etm_task_handle_t ledc_task = NULL;
    ledc_timer_etm_task_config_t ledc_task_config = {
        .task_type = LEDC_TIMER_ETM_TASK_RST,
    };
    ESP_ERROR_CHECK(ledc_timer_new_etm_task(LEDC_MODE, LEDC_TIMER, &ledc_task_config, &ledc_task));

    ESP_ERROR_CHECK(esp_etm_channel_connect(etm_channel, gpio_event, ledc_task));

    ESP_ERROR_CHECK(esp_etm_channel_enable(etm_channel));

    int cnt = 0;
    bool dimming = true; // controls the dimming direction
    uint32_t hpoint = LEDC_HPOINT_INITIAL;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (dimming) {
            // Decrease brightness
            if (cnt < 10) {
                hpoint += LEDC_HPOINT_STEP;
                cnt++;
            } else if (cnt == 10) {
                dimming = false;
            }
        } else {
            // Increase brightness
            if (cnt > 0) {
                hpoint -= LEDC_HPOINT_STEP;
                cnt--;
            } else if (cnt == 0) {
                dimming = true;
            }
        }
        ESP_ERROR_CHECK(ledc_set_duty_with_hpoint(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY, hpoint));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    }
}
