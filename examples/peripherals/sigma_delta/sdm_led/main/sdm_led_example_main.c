/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/sdm.h"

#define EXAMPLE_SIGMA_DELTA_GPIO_NUM 0
#define EXAMPLE_LED_DIM_PERIOD_MS    1000
#define EXAMPLE_LED_DIM_DELAY_MS     10
#define EXAMPLE_LED_DIM_DUTY_STEP    2
#define EXAMPLE_LED_DIM_DUTY_MAX     90
#define EXAMPLE_LED_DIM_DUTY_MIN     (EXAMPLE_LED_DIM_DUTY_MAX - EXAMPLE_LED_DIM_PERIOD_MS / EXAMPLE_LED_DIM_DELAY_MS * EXAMPLE_LED_DIM_DUTY_STEP)

static const char *TAG = "sdm_led";

void app_main(void)
{
    ESP_LOGI(TAG, "Install sigma delta channel");
    sdm_channel_handle_t sdm_chan = NULL;
    sdm_config_t config = {
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .gpio_num = EXAMPLE_SIGMA_DELTA_GPIO_NUM,
        .sample_rate_hz = 1 * 1000 * 1000, // 1MHz sample rate
    };
    ESP_ERROR_CHECK(sdm_new_channel(&config, &sdm_chan));

    ESP_LOGI(TAG, "Enable sigma delta channel");
    ESP_ERROR_CHECK(sdm_channel_enable(sdm_chan));

    ESP_LOGI(TAG, "Change duty cycle continuously");
    int8_t duty = 0;
    int step = EXAMPLE_LED_DIM_DUTY_STEP;
    while (1) {
        ESP_ERROR_CHECK(sdm_channel_set_pulse_density(sdm_chan, duty));
        /* By changing delay time, you can change the blink frequency of LED */
        vTaskDelay(pdMS_TO_TICKS(EXAMPLE_LED_DIM_DELAY_MS));

        duty += step;
        if (duty == EXAMPLE_LED_DIM_DUTY_MAX || duty == EXAMPLE_LED_DIM_DUTY_MIN) {
            step *= -1;
        }
    }
}
