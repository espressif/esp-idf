/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/dac_oneshot.h"
#include "esp_log.h"
#include "dac_continuous_example.h"

#define EXAMPLE_TIMER_RESOLUTION    1000000                 // 1MHz, 1 tick = 1us
#define EXAMPLE_WAVE_FREQ_HZ        50                      // Default wave frequency 50 Hz, it can't be too high
#define EXAMPLE_CONVERT_FREQ_HZ     (EXAMPLE_ARRAY_LEN * EXAMPLE_WAVE_FREQ_HZ) // The frequency that DAC convert every data in the wave array
#define EXAMPLE_TIMER_ALARM_COUNT   (EXAMPLE_TIMER_RESOLUTION / EXAMPLE_CONVERT_FREQ_HZ)    // The count value that trigger the timer alarm callback

static const char *TAG = "dac continuous(timer)";
static const char wav_name[DAC_WAVE_MAX][15] = {"sine", "triangle", "sawtooth",  "square"};
static dac_oneshot_handle_t chan0_handle;
static dac_oneshot_handle_t chan1_handle;

extern uint8_t sin_wav[EXAMPLE_ARRAY_LEN];                      // Used to store sine wave values
extern uint8_t tri_wav[EXAMPLE_ARRAY_LEN];                      // Used to store triangle wave values
extern uint8_t saw_wav[EXAMPLE_ARRAY_LEN];                      // Used to store sawtooth wave values
extern uint8_t squ_wav[EXAMPLE_ARRAY_LEN];                      // Used to store square wave values

/* Timer interrupt service routine */
static bool IRAM_ATTR on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    static uint32_t point_cnt = 0;  // For counting the output points of one wave
    static uint32_t index = 0;      // The current index of the wave buffer
    static dac_example_wave_type_t wav_sel = DAC_SINE_WAVE; // Start from sine wave

    // Switch wave every CONFIG_EXAMPLE_WAVE_PERIOD_SEC second
    if (point_cnt < EXAMPLE_CONVERT_FREQ_HZ * CONFIG_EXAMPLE_WAVE_PERIOD_SEC) {
        switch (wav_sel) {
        case DAC_SINE_WAVE:
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, sin_wav[index]));
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan1_handle, sin_wav[index]));
            break;
        case DAC_TRIANGLE_WAVE:
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, tri_wav[index]));
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan1_handle, tri_wav[index]));
            break;
        case DAC_SAWTOOTH_WAVE:
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, saw_wav[index]));
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan1_handle, saw_wav[index]));
            break;
        case DAC_SQUARE_WAVE:
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, squ_wav[index]));
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan1_handle, squ_wav[index]));
            break;
        default:
            break;
        }
        point_cnt++;
        index++;
        index %= EXAMPLE_ARRAY_LEN;
    } else {
        wav_sel++;
        wav_sel %= DAC_WAVE_MAX;
        point_cnt = 0;
        index = 0;
        ESP_EARLY_LOGI(TAG, "%s wave start", wav_name[wav_sel]);
    }
    return false;
}

void example_dac_continuous_by_timer(void)
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = EXAMPLE_TIMER_RESOLUTION, // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    dac_oneshot_config_t dac0_cfg = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac0_cfg, &chan0_handle));
    dac_oneshot_config_t dac1_cfg = {
        .chan_id = DAC_CHAN_1,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac1_cfg, &chan1_handle));

    example_log_info(EXAMPLE_CONVERT_FREQ_HZ, EXAMPLE_WAVE_FREQ_HZ);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = EXAMPLE_TIMER_ALARM_COUNT,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
