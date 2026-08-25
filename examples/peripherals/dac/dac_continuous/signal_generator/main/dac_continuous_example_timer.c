/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/gptimer.h"
#include "driver/dac_oneshot.h"
#include "esp_log.h"
#include "dac_continuous_example.h"

#define EXAMPLE_TIMER_RESOLUTION    1000000                 // 1MHz, 1 tick = 1us
#define EXAMPLE_WAVE_FREQ_HZ        50                      // Default wave frequency 50 Hz, it can't be too high
#define EXAMPLE_UPDATE_RATE_HZ      (EXAMPLE_ARRAY_LEN * EXAMPLE_WAVE_FREQ_HZ) // The frequency at which the DAC samples (codes) are updated
#define EXAMPLE_TIMER_ALARM_COUNT   (EXAMPLE_TIMER_RESOLUTION / EXAMPLE_UPDATE_RATE_HZ)    // The count value that trigger the timer alarm callback

/* Timer interrupt service routine */
static bool IRAM_ATTR on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    static uint32_t point_cnt = 0;  // For counting the output points of one wave
    static uint32_t index = 0;      // The current index of the wave buffer
    static dac_example_wave_type_t wave_sel = DAC_SINE_WAVE; // Start from sine wave

    dac_oneshot_handle_t dac_handle = user_data;

    if (point_cnt == 0) {
        ESP_EARLY_LOGI(TAG, "%s wave start", wave_name[wave_sel]);
    }

    dac_example_sample_t code = wave_data[wave_sel][index];
    dac_oneshot_output_voltage(dac_handle, code);

    point_cnt++;
    if (point_cnt < EXAMPLE_UPDATE_RATE_HZ * CONFIG_EXAMPLE_WAVE_PERIOD_SEC) {
        index = (index + 1) % EXAMPLE_ARRAY_LEN;
    } else {
        point_cnt = 0;
        index = 0;
        wave_sel = (wave_sel + 1) % DAC_WAVE_MAX;
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

    dac_oneshot_handle_t dac_handle = NULL;
    dac_oneshot_config_t dac_cfg = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_cfg, &dac_handle));

    example_generate_wave(dac_oneshot_get_bitwidth(dac_handle));

    example_log_info(EXAMPLE_UPDATE_RATE_HZ, EXAMPLE_WAVE_FREQ_HZ);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = EXAMPLE_TIMER_ALARM_COUNT,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, dac_handle));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
