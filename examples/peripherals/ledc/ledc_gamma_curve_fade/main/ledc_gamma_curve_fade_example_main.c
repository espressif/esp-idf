/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

/**
 * About this example
 *
 * This example sets up two RGB LEDs to show how gamma correction can affect towards human eyes.
 *
 * RGB LED 1: R (IO0), G (IO1), B (IO2)
 *            Gamma correction is not applied to this LED
 *
 * RGB LED 2: R (IO3), G (IO4), B (IO5)
 *            Gamma correction is applied to this LED
 *
 * You should feel the fade in general is more gradual and smoother on RGB LED 2 than RGB LED 1.
 */

#define LEDC_TIMER                    LEDC_TIMER_0
#define LEDC_MODE                     LEDC_LOW_SPEED_MODE

#define LEDC_DUTY_RES                 LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY                (4000) // Frequency in Hertz. Set frequency at 4 kHz

// Define two RGB LEDs IOs and channels
#define LEDC_RED_IO                   (0)
#define LEDC_GREEN_IO                 (1)
#define LEDC_BLUE_IO                  (2)
#define LEDC_GAMMA_RED_IO             (3)
#define LEDC_GAMMA_GREEN_IO           (4)
#define LEDC_GAMMA_BLUE_IO            (5)

#define LEDC_CHANNEL_RED              (LEDC_CHANNEL_0)
#define LEDC_CHANNEL_GREEN            (LEDC_CHANNEL_1)
#define LEDC_CHANNEL_BLUE             (LEDC_CHANNEL_2)
#define LEDC_CHANNEL_GAMMA_RED        (LEDC_CHANNEL_3)
#define LEDC_CHANNEL_GAMMA_GREEN      (LEDC_CHANNEL_4)
#define LEDC_CHANNEL_GAMMA_BLUE       (LEDC_CHANNEL_5)

// Structure to store R, G, B channels
typedef struct {
    uint32_t red_channel;
    uint32_t green_channel;
    uint32_t blue_channel;
} rgb_channel_config_t;

static rgb_channel_config_t rgb_led_1_channels = {
    .red_channel = LEDC_CHANNEL_RED,
    .green_channel = LEDC_CHANNEL_GREEN,
    .blue_channel = LEDC_CHANNEL_BLUE,
};

static rgb_channel_config_t rgb_led_2_channels = {
    .red_channel = LEDC_CHANNEL_GAMMA_RED,
    .green_channel = LEDC_CHANNEL_GAMMA_GREEN,
    .blue_channel = LEDC_CHANNEL_GAMMA_BLUE,
};

// Define some colors R, G, B channel PWM duty cycles
#define RGB_TO_DUTY(x)  (x * (1 << LEDC_DUTY_RES) / 255)

#define OFF_R           0
#define OFF_G           0
#define OFF_B           0

// RED       - R: 255, G: 0  , B: 0
//             H: 0  , S: 100, V: 100
#define RED_R           RGB_TO_DUTY(255)
#define RED_G           RGB_TO_DUTY(0)
#define RED_B           RGB_TO_DUTY(0)

// YELLOW    - R: 255, G: 255, B: 0
//             H: 60 , S: 100, V: 100
#define YELLOW_R        RGB_TO_DUTY(255)
#define YELLOW_G        RGB_TO_DUTY(255)
#define YELLOW_B        RGB_TO_DUTY(0)

// GREEN    - R: 0  , G: 255, B: 0
//            H: 120, S: 100, V: 100
#define GREEN_R         RGB_TO_DUTY(0)
#define GREEN_G         RGB_TO_DUTY(255)
#define GREEN_B         RGB_TO_DUTY(0)

// CYAN      - R: 0  , G: 255, B: 255
//             H: 180, S: 100, V: 100
#define CYAN_R          RGB_TO_DUTY(0)
#define CYAN_G          RGB_TO_DUTY(255)
#define CYAN_B          RGB_TO_DUTY(255)

// BLUE     - R: 0  , G: 0   , B: 255
//            H: 240, S: 100, V: 100
#define BLUE_R          RGB_TO_DUTY(0)
#define BLUE_G          RGB_TO_DUTY(0)
#define BLUE_B          RGB_TO_DUTY(255)

// MAGENTA   - R: 255, G: 0  , B: 255
//             H: 300, S: 100, V: 100
#define MAGENTA_R       RGB_TO_DUTY(255)
#define MAGENTA_G       RGB_TO_DUTY(0)
#define MAGENTA_B       RGB_TO_DUTY(255)

// ORANGE   - R: 255, G: 128, B: 0
//            H: 30 , S: 100, V: 100
#define ORANGE_R        RGB_TO_DUTY(255)
#define ORANGE_G        RGB_TO_DUTY(128)
#define ORANGE_B        RGB_TO_DUTY(0)

// ROSE PINK - R: 255, G: 0  , B: 128
//             H: 330, S: 100, V: 100
#define ROSE_R          RGB_TO_DUTY(255)
#define ROSE_G          RGB_TO_DUTY(0)
#define ROSE_B          RGB_TO_DUTY(128)

// BLUEISH PURPLE - R: 178, G: 102, B: 255
//                  H: 270, S: 60, V: 100
#define BLUEISH_PURPLE_R      RGB_TO_DUTY(178)
#define BLUEISH_PURPLE_G      RGB_TO_DUTY(102)
#define BLUEISH_PURPLE_B      RGB_TO_DUTY(255)

#if CONFIG_GAMMA_CORRECTION_WITH_LUT
// Brightness 0 - 100% gamma correction look up table (gamma = 2.6)
// Y = B ^ 2.6
// Pre-computed LUT to save some runtime computation
static const float gamma_correction_lut[101] = {
    0.000000, 0.000006, 0.000038, 0.000110, 0.000232, 0.000414, 0.000666, 0.000994, 0.001406, 0.001910,
    0.002512, 0.003218, 0.004035, 0.004969, 0.006025, 0.007208, 0.008525, 0.009981, 0.011580, 0.013328,
    0.015229, 0.017289, 0.019512, 0.021902, 0.024465, 0.027205, 0.030125, 0.033231, 0.036527, 0.040016,
    0.043703, 0.047593, 0.051688, 0.055993, 0.060513, 0.065249, 0.070208, 0.075392, 0.080805, 0.086451,
    0.092333, 0.098455, 0.104821, 0.111434, 0.118298, 0.125416, 0.132792, 0.140428, 0.148329, 0.156498,
    0.164938, 0.173653, 0.182645, 0.191919, 0.201476, 0.211321, 0.221457, 0.231886, 0.242612, 0.253639,
    0.264968, 0.276603, 0.288548, 0.300805, 0.313378, 0.326268, 0.339480, 0.353016, 0.366879, 0.381073,
    0.395599, 0.410461, 0.425662, 0.441204, 0.457091, 0.473325, 0.489909, 0.506846, 0.524138, 0.541789,
    0.559801, 0.578177, 0.596920, 0.616032, 0.635515, 0.655374, 0.675610, 0.696226, 0.717224, 0.738608,
    0.760380, 0.782542, 0.805097, 0.828048, 0.851398, 0.875148, 0.899301, 0.923861, 0.948829, 0.974208,
    1.000000,
};

static uint32_t gamma_correction_calculator(uint32_t duty)
{
    return gamma_correction_lut[duty * 100 / (1 << LEDC_DUTY_RES)] * (1 << LEDC_DUTY_RES);
}
#else // !CONFIG_GAMMA_CORRECTION_WITH_LUT
#define GAMMA_FACTOR            (2.8)

static uint32_t gamma_correction_calculator(uint32_t duty)
{
    return pow((double)duty / (1 << LEDC_DUTY_RES), GAMMA_FACTOR) * (1 << LEDC_DUTY_RES);
}
#endif // CONFIG_GAMMA_CORRECTION_WITH_LUT

static void rgb_set_duty_and_update(rgb_channel_config_t rgb_channels,
                                    uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, rgb_channels.red_channel, target_r_duty));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, rgb_channels.green_channel, target_g_duty));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, rgb_channels.blue_channel, target_b_duty));

    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, rgb_channels.red_channel));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, rgb_channels.green_channel));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, rgb_channels.blue_channel));
}

static void rgb_set_linear_fade(rgb_channel_config_t rgb_channels,
                                uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty,
                                uint32_t duration)
{
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, rgb_channels.red_channel, target_r_duty, duration));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, rgb_channels.green_channel, target_g_duty, duration));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, rgb_channels.blue_channel, target_b_duty, duration));
}

static void rgb_set_gamma_curve_fade(rgb_channel_config_t rgb_channels,
                                     uint32_t start_r_duty, uint32_t start_g_duty, uint32_t start_b_duty,
                                     uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty,
                                     uint32_t duration)
{
    const uint32_t linear_fade_segments = 12;
    uint32_t actual_fade_ranges;
    ledc_fade_param_config_t fade_params_list[SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX] = {};

    ESP_ERROR_CHECK(ledc_fill_multi_fade_param_list(LEDC_MODE, rgb_channels.red_channel,
                                                    start_r_duty, target_r_duty,
                                                    linear_fade_segments, duration,
                                                    gamma_correction_calculator,
                                                    SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX,
                                                    fade_params_list, &actual_fade_ranges));
    ESP_ERROR_CHECK(ledc_set_multi_fade(LEDC_MODE, rgb_channels.red_channel, gamma_correction_calculator(start_r_duty), fade_params_list, actual_fade_ranges));

    ESP_ERROR_CHECK(ledc_fill_multi_fade_param_list(LEDC_MODE, rgb_channels.green_channel,
                                                    start_g_duty, target_g_duty,
                                                    linear_fade_segments, duration,
                                                    gamma_correction_calculator,
                                                    SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX,
                                                    fade_params_list, &actual_fade_ranges));
    ESP_ERROR_CHECK(ledc_set_multi_fade(LEDC_MODE, rgb_channels.green_channel, gamma_correction_calculator(start_g_duty), fade_params_list, actual_fade_ranges));

    ESP_ERROR_CHECK(ledc_fill_multi_fade_param_list(LEDC_MODE, rgb_channels.blue_channel,
                                                    start_b_duty, target_b_duty,
                                                    linear_fade_segments, duration,
                                                    gamma_correction_calculator,
                                                    SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX,
                                                    fade_params_list, &actual_fade_ranges));
    ESP_ERROR_CHECK(ledc_set_multi_fade(LEDC_MODE, rgb_channels.blue_channel, gamma_correction_calculator(start_b_duty), fade_params_list, actual_fade_ranges));
}

static void rgb_fade_start(rgb_channel_config_t rgb_channels)
{
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, rgb_channels.red_channel, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, rgb_channels.green_channel, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, rgb_channels.blue_channel, LEDC_FADE_NO_WAIT));
}

static void example_rgb_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM configuration to the six channels
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .duty           = 0, // Set initial duty to 0%
        .hpoint         = 0
    };
    ledc_channel.channel = LEDC_CHANNEL_RED;
    ledc_channel.gpio_num = LEDC_RED_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.channel = LEDC_CHANNEL_GREEN;
    ledc_channel.gpio_num = LEDC_GREEN_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.channel = LEDC_CHANNEL_BLUE;
    ledc_channel.gpio_num = LEDC_BLUE_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.channel = LEDC_CHANNEL_GAMMA_RED;
    ledc_channel.gpio_num = LEDC_GAMMA_RED_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.channel = LEDC_CHANNEL_GAMMA_GREEN;
    ledc_channel.gpio_num = LEDC_GAMMA_GREEN_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.channel = LEDC_CHANNEL_GAMMA_BLUE;
    ledc_channel.gpio_num = LEDC_GAMMA_BLUE_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void example_set_fade_and_start(uint32_t start_r_duty, uint32_t start_g_duty, uint32_t start_b_duty,
                                       uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty,
                                       uint32_t duration)
{
    rgb_set_linear_fade(rgb_led_1_channels, target_r_duty, target_g_duty, target_b_duty, duration);
    rgb_set_gamma_curve_fade(rgb_led_2_channels, start_r_duty, start_g_duty, start_b_duty, target_r_duty, target_g_duty, target_b_duty, duration);
    rgb_fade_start(rgb_led_1_channels);
    rgb_fade_start(rgb_led_2_channels);
    vTaskDelay(pdMS_TO_TICKS(duration));
}

void app_main(void)
{
    example_rgb_ledc_init();
    ledc_fade_func_install(0);

    // Light up RGB LEDs to GREEN in 500ms
    example_set_fade_and_start(OFF_R, OFF_G, OFF_B, GREEN_R, GREEN_G, GREEN_B, 500);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Dim to OFF in 2000ms
    example_set_fade_and_start(GREEN_R, GREEN_G, GREEN_B, OFF_R, OFF_G, OFF_B, 2000);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Set RGB LEDs to BLUEISH_PURPLE color
    rgb_set_duty_and_update(rgb_led_1_channels, BLUEISH_PURPLE_R, BLUEISH_PURPLE_G, BLUEISH_PURPLE_B);
    rgb_set_duty_and_update(rgb_led_2_channels, gamma_correction_calculator(BLUEISH_PURPLE_R), gamma_correction_calculator(BLUEISH_PURPLE_G), gamma_correction_calculator(BLUEISH_PURPLE_B));
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Dim to OFF in 5000ms
    example_set_fade_and_start(BLUEISH_PURPLE_R, BLUEISH_PURPLE_G, BLUEISH_PURPLE_B, OFF_R, OFF_G, OFF_B, 5000);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Light up to ORANGE in 5000ms
    example_set_fade_and_start(OFF_R, OFF_G, OFF_B, ORANGE_R, ORANGE_G, ORANGE_B, 5000);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Fade color to ROSE PINK in 5000ms
    example_set_fade_and_start(ORANGE_R, ORANGE_G, ORANGE_B, ROSE_R, ROSE_G, ROSE_B, 5000);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Cycle through color spectrum
    while (1) {
        example_set_fade_and_start(RED_R, RED_G, RED_B, YELLOW_R, YELLOW_G, YELLOW_B, 3000);
        vTaskDelay(pdMS_TO_TICKS(5));

        example_set_fade_and_start(YELLOW_R, YELLOW_G, YELLOW_B, GREEN_R, GREEN_G, GREEN_B, 3000);
        vTaskDelay(pdMS_TO_TICKS(5));

        example_set_fade_and_start(GREEN_R, GREEN_G, GREEN_B, CYAN_R, CYAN_G, CYAN_B, 3000);
        vTaskDelay(pdMS_TO_TICKS(5));

        example_set_fade_and_start(CYAN_R, CYAN_G, CYAN_B, BLUE_R, BLUE_G, BLUE_B, 3000);
        vTaskDelay(pdMS_TO_TICKS(5));

        example_set_fade_and_start(BLUE_R, BLUE_G, BLUE_B, MAGENTA_R, MAGENTA_G, MAGENTA_B, 3000);
        vTaskDelay(pdMS_TO_TICKS(5));

        example_set_fade_and_start(MAGENTA_R, MAGENTA_G, MAGENTA_B, RED_R, RED_G, RED_B, 3000);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
