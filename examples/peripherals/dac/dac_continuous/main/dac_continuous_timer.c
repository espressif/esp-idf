/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
#include "driver/adc.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "dac_continuous.h"

#define EXAMPLE_TIEMR_RESOLUTION    1000000                 // 1MHz, 1 tick = 1us
#define EXAMPLE_WAVE_FREQ_HZ        50                      // Default wave frequency 50 Hz, it can't be too high
#define EXAMPLE_CONVERT_FREQ_HZ     (EXAMPLE_ARRAY_LEN * EXAMPLE_WAVE_FREQ_HZ) // The frequency that DAC convert every data in the wave array
#define EXAMPLE_TIMER_ALARM_COUNT   (EXAMPLE_TIEMR_RESOLUTION / EXAMPLE_CONVERT_FREQ_HZ)    // The count value that trigger the timer alarm callback

static const char *TAG = "dac continuous(timer)";
static const char wav_name[DAC_WAVE_MAX][15] = {"sine", "triangle", "sawtooth",  "square"};
static dac_channels_handle_t dac_handle;

static uint8_t sin_wav[EXAMPLE_ARRAY_LEN];  // sine wave values
static uint8_t tri_wav[EXAMPLE_ARRAY_LEN];  // triangle wave values
static uint8_t saw_wav[EXAMPLE_ARRAY_LEN];  // sawtooth wave values
static uint8_t squ_wav[EXAMPLE_ARRAY_LEN];  // square wave values

/* Timer interrupt service routine */
static bool IRAM_ATTR on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    static uint32_t point_cnt = 0;  // For counting the output points of one wave
    static uint32_t index = 0;      // The current index of the wave buffer
    static dac_example_wave_type_t wav_sel = DAC_SINE_WAVE; // Start from sine wave

    // Switch wave every EXAMPLE_WAVE_PERIOD_SEC sencond
    if (point_cnt < EXAMPLE_CONVERT_FREQ_HZ * EXAMPLE_WAVE_PERIOD_SEC) {
        switch (wav_sel) {
            case DAC_SINE_WAVE:
                ESP_ERROR_CHECK(dac_channels_set_voltage(dac_handle, sin_wav[index]));
                break;
            case DAC_TRIANGLE_WAVE:
                ESP_ERROR_CHECK(dac_channels_set_voltage(dac_handle, tri_wav[index]));
                break;
            case DAC_SAWTOOTH_WAVE:
                ESP_ERROR_CHECK(dac_channels_set_voltage(dac_handle, saw_wav[index]));
                break;
            case DAC_SQUARE_WAVE:
                ESP_ERROR_CHECK(dac_channels_set_voltage(dac_handle, squ_wav[index]));
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

static void example_generate_wave(void)
{
    uint32_t pnt_num = EXAMPLE_ARRAY_LEN;
    for (int i = 0; i < pnt_num; i ++) {
        sin_wav[i] = (uint8_t)((sin( i * CONST_PERIOD_2_PI / pnt_num) + 1) * (double)(EXAMPLE_DAC_AMPLITUDE) / 2 + 0.5);
        tri_wav[i] = (i > (pnt_num / 2)) ? (2 * EXAMPLE_DAC_AMPLITUDE * (pnt_num - i) / pnt_num) : (2 * EXAMPLE_DAC_AMPLITUDE * i / pnt_num);
        saw_wav[i] = (i == pnt_num) ? 0 : (i * EXAMPLE_DAC_AMPLITUDE / pnt_num);
        squ_wav[i] = (i < (pnt_num / 2)) ? EXAMPLE_DAC_AMPLITUDE : 0;
    }
}

void dac_continuous_by_timer(void)
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = EXAMPLE_TIEMR_RESOLUTION, // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    dac_channels_config_t dac_cfg = {
        .chan_sel = EXAMPLE_DAC_CHANNEL,
    };
    ESP_ERROR_CHECK(dac_new_channels(&dac_cfg, &dac_handle));
    ESP_ERROR_CHECK(dac_channels_enable(dac_handle));

    example_log_info(EXAMPLE_CONVERT_FREQ_HZ, EXAMPLE_WAVE_FREQ_HZ);
    example_generate_wave();

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
