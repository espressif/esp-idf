/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/sdm.h"
#include "driver/gptimer.h"

#define EXAMPLE_SIGMA_DELTA_GPIO_NUM    (0)                 // Select GPIO_NUM_0 as the sigma-delta output pin
#define EXAMPLE_OVER_SAMPLE_RATE        (20 * 1000 * 1000)  // 20 MHz over sample rate
#define EXAMPLE_TIMER_RESOLUTION        (10 * 1000 * 1000)  // 10 MHz timer counting resolution
#define EXAMPLE_CALLBACK_INTERVAL_US    (10)                // 20 us interval of each timer callback
#define EXAMPLE_ALARM_COUNT             (EXAMPLE_CALLBACK_INTERVAL_US * (EXAMPLE_TIMER_RESOLUTION / 1000000))
#define EXAMPLE_SINE_WAVE_FREQ_HZ       (1000)              // 1 KHz wave
#define EXAMPLE_SINE_WAVE_AMPLITUDE     (127.0f)            // 1 ~ 127
#define EXAMPLE_SINE_WAVE_POINT_NUM     (1000000 / (EXAMPLE_CALLBACK_INTERVAL_US * EXAMPLE_SINE_WAVE_FREQ_HZ))
#define CONST_PI                        (3.1416f)           // Constant of PI, used for calculating the sine wave

_Static_assert(EXAMPLE_SINE_WAVE_POINT_NUM > 1, "Sine wave frequency is too high");
_Static_assert(EXAMPLE_CALLBACK_INTERVAL_US >= 7, "Timer callback interval is too short");

static const char *TAG = "sdm_dac";
static int8_t sine_wave[EXAMPLE_SINE_WAVE_POINT_NUM];   // Sine wave data buffer

static bool example_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    static uint32_t cnt = 0;
    sdm_channel_handle_t sdm_chan = (sdm_channel_handle_t)user_ctx;
    /* Set the pulse density */
    sdm_channel_set_duty(sdm_chan, sine_wave[cnt]);
    /* Loop the sine wave data buffer */
    cnt++;
    cnt %= EXAMPLE_SINE_WAVE_POINT_NUM;
    return false;
}

static gptimer_handle_t example_init_gptimer(void* args)
{
    /* Allocate GPTimer handle */
    gptimer_handle_t timer_handle;
    gptimer_config_t timer_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = EXAMPLE_TIMER_RESOLUTION,
        .flags.intr_shared = false,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_cfg, &timer_handle));
    ESP_LOGI(TAG, "Timer allocated with resolution %d Hz", EXAMPLE_TIMER_RESOLUTION);

    /* Set the timer alarm configuration */
    gptimer_alarm_config_t alarm_cfg = {
        .alarm_count = EXAMPLE_ALARM_COUNT,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle, &alarm_cfg));

    /* Register the alarm callback */
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle, &cbs, args));
    ESP_LOGI(TAG, "Timer callback registered, interval %d us", EXAMPLE_CALLBACK_INTERVAL_US);

    /* Clear the timer raw count value, make sure it'll count from 0 */
    ESP_ERROR_CHECK(gptimer_set_raw_count(timer_handle, 0));
    /* Enable the timer */
    ESP_ERROR_CHECK(gptimer_enable(timer_handle));

    ESP_LOGI(TAG, "Timer enabled");

    return timer_handle;
}

static sdm_channel_handle_t example_init_sdm(void)
{
    /* Allocate sdm channel handle */
    sdm_channel_handle_t sdm_chan = NULL;
    sdm_config_t config = {
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .gpio_num = EXAMPLE_SIGMA_DELTA_GPIO_NUM,
        .sample_rate_hz = EXAMPLE_OVER_SAMPLE_RATE,
    };
    ESP_ERROR_CHECK(sdm_new_channel(&config, &sdm_chan));
    /* Enable the sdm channel */
    ESP_ERROR_CHECK(sdm_channel_enable(sdm_chan));

    ESP_LOGI(TAG, "Sigma-delta output is attached to GPIO %d", EXAMPLE_SIGMA_DELTA_GPIO_NUM);

    return sdm_chan;
}

void app_main(void)
{
    /* Assign sine wave data */
    for (int i = 0; i < EXAMPLE_SINE_WAVE_POINT_NUM; i++) {
        sine_wave[i] = (int8_t)((sin(2 * (float)i * CONST_PI / EXAMPLE_SINE_WAVE_POINT_NUM)) * EXAMPLE_SINE_WAVE_AMPLITUDE);
    }
    /* Initialize sigma-delta modulation on the specific GPIO */
    sdm_channel_handle_t sdm_chan = example_init_sdm();
    /* Initialize GPTimer and register the timer alarm callback */
    gptimer_handle_t timer_handle = example_init_gptimer(sdm_chan);
    /* Start the GPTimer */
    ESP_LOGI(TAG, "Output start");
    ESP_ERROR_CHECK(gptimer_start(timer_handle));

    /* Forever loop */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
