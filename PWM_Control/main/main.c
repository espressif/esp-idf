#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH0_GPIO       3   // GPIO 3 for LED control
#define LEDC_LS_CH0_CHANNEL    LEDC_CHANNEL_0

#define LEDC_TEST_DUTY         (4000) // Duty cycle
#define LEDC_TEST_FADE_TIME    (3000) // Fade time in milliseconds

static IRAM_ATTR bool cb_ledc_fade_end_event(const ledc_cb_param_t *param, void *user_arg)
{
    BaseType_t taskAwoken = pdFALSE;

    if (param->event == LEDC_FADE_END_EVT) {
        SemaphoreHandle_t counting_sem = (SemaphoreHandle_t) user_arg;
        xSemaphoreGiveFromISR(counting_sem, &taskAwoken);
    }

    return (taskAwoken == pdTRUE);
}

void app_main(void)
{
    // Timer configuration for LEDC
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 4000,
        .speed_mode = LEDC_LS_MODE,
        .timer_num = LEDC_LS_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    // Channel configuration for LEDC (GPIO 3)
    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_LS_CH0_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_LS_CH0_GPIO,
        .speed_mode = LEDC_LS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_LS_TIMER,
        .flags.output_invert = 0
    };

    // Set the channel configuration
    ledc_channel_config(&ledc_channel);

    // Initialize fade service
    ledc_fade_func_install(0);
    ledc_cbs_t callbacks = {
        .fade_cb = cb_ledc_fade_end_event
    };
    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(1, 0);

    // Register callback for fade end event
    ledc_cb_register(ledc_channel.speed_mode, ledc_channel.channel, &callbacks, (void *)counting_sem);

    while (1) {
        // Fade LED up to duty = LEDC_TEST_DUTY
        printf("1. LEDC fade up to duty = %d\n", LEDC_TEST_DUTY);
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, LEDC_TEST_DUTY, LEDC_TEST_FADE_TIME);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);
        xSemaphoreTake(counting_sem, portMAX_DELAY);

        // Fade LED down to duty = 0
        printf("2. LEDC fade down to duty = 0\n");
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, LEDC_TEST_FADE_TIME);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);
        xSemaphoreTake(counting_sem, portMAX_DELAY);

        // Set duty to LEDC_TEST_DUTY without fade
        printf("3. LEDC set duty = %d without fade\n", LEDC_TEST_DUTY);
        ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, LEDC_TEST_DUTY);
        ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Set duty to 0 without fade
        printf("4. LEDC set duty = 0 without fade\n");
        ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 0);
        ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
