/*  Wave Generator Example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    DAC output channel, waveform, wave frequency can be customized in menuconfig.
    If any questions about this example or more information is needed, please read README.md before your start.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/dac.h"
#include "driver/gptimer.h"
#include "esp_log.h"

/*  The timer ISR has an execution time of 5.5 micro-seconds(us).
    Therefore, a timer period less than 5.5 us will cause trigger the interrupt watchdog.
    7 us is a safe interval that will not trigger the watchdog. No need to customize it.
*/

#define TIMER_INTR_US          7                                    // Execution time of each ISR interval in micro-seconds
#define POINT_ARR_LEN          200                                  // Length of points array
#define AMP_DAC                255                                  // Amplitude of DAC voltage. If it's more than 256 will causes dac_output_voltage() output 0.
#define VDD                    3300                                 // VDD is 3.3V, 3300mV
#define CONST_PERIOD_2_PI      6.2832
#define DAC_CHAN               CONFIG_EXAMPLE_DAC_CHANNEL           // DAC_CHANNEL_1 (GPIO25) by default
#define FREQ                   CONFIG_EXAMPLE_WAVE_FREQUENCY        // 3kHz by default
#define OUTPUT_POINT_NUM       (int)(1000000 / (TIMER_INTR_US * FREQ) + 0.5)     // The number of output wave points.

_Static_assert(OUTPUT_POINT_NUM <= POINT_ARR_LEN, "The CONFIG_EXAMPLE_WAVE_FREQUENCY is too low and using too long buffer.");

static int raw_val[POINT_ARR_LEN];                      // Used to store raw values
static int volt_val[POINT_ARR_LEN];                    // Used to store voltage values(in mV)
static const char *TAG = "wave_gen";

static int g_index = 0;

/* Timer interrupt service routine */
static bool IRAM_ATTR on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    int *head = (int *)user_data;

    /* DAC output ISR has an execution time of 4.4 us*/
    if (g_index >= OUTPUT_POINT_NUM) {
        g_index = 0;
    }
    dac_output_voltage(DAC_CHAN, *(head + g_index));
    g_index++;
    return false;
}

static void prepare_data(int pnt_num)
{
    for (int i = 0; i < pnt_num; i ++) {
#ifdef CONFIG_EXAMPLE_WAVEFORM_SINE
        raw_val[i] = (int)((sin( i * CONST_PERIOD_2_PI / pnt_num) + 1) * (double)(AMP_DAC) / 2 + 0.5);
#elif CONFIG_EXAMPLE_WAVEFORM_TRIANGLE
        raw_val[i] = (i > (pnt_num / 2)) ? (2 * AMP_DAC * (pnt_num - i) / pnt_num) : (2 * AMP_DAC * i / pnt_num);
#elif CONFIG_EXAMPLE_WAVEFORM_SAWTOOTH
        raw_val[i] = (i == pnt_num) ? 0 : (i * AMP_DAC / pnt_num);
#elif CONFIG_EXAMPLE_WAVEFORM_SQUARE
        raw_val[i] = (i < (pnt_num / 2)) ? AMP_DAC : 0;
#endif
        volt_val[i] = (int)(VDD * raw_val[i] / (float)AMP_DAC);
    }
}

static void log_info(void)
{
    ESP_LOGI(TAG, "DAC output channel: %d", DAC_CHAN);
    if (DAC_CHAN == DAC_CHANNEL_1) {
        ESP_LOGI(TAG, "GPIO:%d", GPIO_NUM_25);
    } else {
        ESP_LOGI(TAG, "GPIO:%d", GPIO_NUM_26);
    }
#ifdef CONFIG_EXAMPLE_WAVEFORM_SINE
    ESP_LOGI(TAG, "Waveform: SINE");
#elif CONFIG_EXAMPLE_WAVEFORM_TRIANGLE
    ESP_LOGI(TAG, "Waveform: TRIANGLE");
#elif CONFIG_EXAMPLE_WAVEFORM_SAWTOOTH
    ESP_LOGI(TAG, "Waveform: SAWTOOTH");
#elif CONFIG_EXAMPLE_WAVEFORM_SQUARE
    ESP_LOGI(TAG, "Waveform: SQUARE");
#endif

    ESP_LOGI(TAG, "Frequency(Hz): %d", FREQ);
    ESP_LOGI(TAG, "Output points num: %d\n", OUTPUT_POINT_NUM);
}

void app_main(void)
{
    g_index = 0;
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    ESP_ERROR_CHECK(dac_output_enable(DAC_CHAN));

    log_info();
    prepare_data(OUTPUT_POINT_NUM);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = TIMER_INTR_US,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, raw_val));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    while (1) {
        vTaskDelay(10);
#if CONFIG_EXAMPLE_LOG_VOLTAGE
        if (g_index < OUTPUT_POINT_NUM) {
            ESP_LOGI(TAG, "Output voltage(mV): %d", volt_val[g_index]);
            ESP_LOGD(TAG, "g_index: %d\n", g_index);
        }
#endif
    }
}
