/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* LEDC tested by PCNT in some case
 * PCNT can get the LEDC waveform frequency
 *
 * test environment of UT_T1_LEDC:
 * 1. connect GPIO18 with GPIO4
 * 2. connect GPIO5 to 3.3v (in case of it is pulled down by default)
 *
 * some calculation related with duty:
 * real duty = duty/2^duty_resolution
 */
#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "soc/ledc_periph.h"
#include "esp_system.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#define PULSE_IO 18
#define PCNT_INPUT_IO 4
#define PCNT_CTRL_FLOATING_IO 5
#define HIGHEST_LIMIT 10000
#define LOWEST_LIMIT -10000

#define TEST_PWM_FREQ 2000

#if SOC_LEDC_SUPPORT_HS_MODE
#define TEST_SPEED_MODE LEDC_HIGH_SPEED_MODE
#define SPEED_MODE_LIST {LEDC_HIGH_SPEED_MODE, LEDC_LOW_SPEED_MODE}
#else
#define TEST_SPEED_MODE LEDC_LOW_SPEED_MODE
#define SPEED_MODE_LIST {LEDC_LOW_SPEED_MODE}
#endif

static ledc_channel_config_t initialize_channel_config(void)
{
    ledc_channel_config_t config;
    memset(&config, 0, sizeof(ledc_channel_config_t));
    config.gpio_num = PULSE_IO;
    config.speed_mode = TEST_SPEED_MODE;
    config.channel  = LEDC_CHANNEL_0;
    config.intr_type = LEDC_INTR_DISABLE;
    config.timer_sel = LEDC_TIMER_0;
    config.duty = 4000;
    config.hpoint = 0;
    return config;
}

static ledc_timer_config_t create_default_timer_config(void)
{
    ledc_timer_config_t ledc_time_config;
    memset(&ledc_time_config, 0, sizeof(ledc_timer_config_t));
    ledc_time_config.speed_mode = TEST_SPEED_MODE;
    ledc_time_config.duty_resolution = LEDC_TIMER_13_BIT;
    ledc_time_config.timer_num = LEDC_TIMER_0;
    ledc_time_config.freq_hz = TEST_PWM_FREQ;
    ledc_time_config.clk_cfg = LEDC_USE_APB_CLK;
    return ledc_time_config;
}

static void fade_setup(void)
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.duty = 0;
    ledc_timer_config_t ledc_time_config = create_default_timer_config();

    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    vTaskDelay(5 / portTICK_PERIOD_MS);

    //initialize fade service
    TEST_ESP_OK(ledc_fade_func_install(0));
}

static void timer_duty_set_get(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    TEST_ESP_OK(ledc_set_duty(speed_mode, channel, duty));
    TEST_ESP_OK(ledc_update_duty(speed_mode, channel));
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(duty, ledc_get_duty(speed_mode, channel));
}

// use logic analyzer to view
static void timer_duty_test(ledc_channel_t channel, ledc_timer_bit_t timer_bit, ledc_timer_t timer, ledc_mode_t speed_mode)
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.speed_mode = speed_mode;
    ledc_ch_config.channel  = channel;
    ledc_ch_config.timer_sel = timer;

    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    ledc_time_config.speed_mode = speed_mode;
    ledc_time_config.duty_resolution = timer_bit;
    ledc_time_config.timer_num = timer;

    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    // duty ratio: (2^duty)/(2^timer_bit)
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 0);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 1);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 1 << 12); // 50% duty
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, (1 << 13) - 1);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, (1 << 13) - 2);
}

#if SOC_PCNT_SUPPORTED
#include "driver/pcnt.h" // TODO: C3 doesn't have PCNT peripheral

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)
//no runners

// use PCNT to test the waveform of LEDC
static int16_t wave_count(int last_time)
{
    int16_t test_counter;
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = PCNT_INPUT_IO,
        .ctrl_gpio_num = PCNT_CTRL_FLOATING_IO,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = HIGHEST_LIMIT,
        .counter_l_lim = LOWEST_LIMIT,
    };
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));

    // initialize first
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));

    vTaskDelay(last_time / portTICK_RATE_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    return test_counter;
}

// the PCNT will count the frequency of it
static void frequency_set_get(ledc_mode_t speed_mode, ledc_timer_t timer, uint32_t freq_hz, int16_t real_freq, int16_t error)
{
    int16_t count;
    TEST_ESP_OK(ledc_set_freq(speed_mode, timer, freq_hz));
    count = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(error, count, real_freq);
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(speed_mode, timer), real_freq);
}

static void timer_frequency_test(ledc_channel_t channel, ledc_timer_bit_t timer_bit, ledc_timer_t timer, ledc_mode_t speed_mode)
{
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = speed_mode,
        .channel  = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer,
        .duty = 4000,
        .hpoint = 0,
    };
    ledc_timer_config_t ledc_time_config = {
        .speed_mode = speed_mode,
        .duty_resolution = timer_bit,
        .timer_num = timer,
        .freq_hz = 5000,
        .clk_cfg = LEDC_USE_APB_CLK,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 100, 100, 2);
    frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 5000, 5000, 5);
    frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 9000, 9025, 5);
}

#endif // !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)

#endif // SOC_PCNT_SUPPORTED

TEST_CASE("LEDC channel config wrong gpio", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.gpio_num = GPIO_NUM_MAX;
    TEST_ASSERT(ledc_channel_config(&ledc_ch_config) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("LEDC channel config wrong speed", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.speed_mode = LEDC_SPEED_MODE_MAX;
    TEST_ASSERT(ledc_channel_config(&ledc_ch_config) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("LEDC channel config wrong channel", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.channel = LEDC_CHANNEL_MAX;
    TEST_ASSERT(ledc_channel_config(&ledc_ch_config) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("LEDC channel config wrong interrupt type", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.intr_type = 2;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ledc_channel_config(&ledc_ch_config));
}

TEST_CASE("LEDC wrong timer", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.timer_sel = LEDC_TIMER_MAX;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ledc_channel_config(&ledc_ch_config));
}

TEST_CASE("LEDC channel config basic parameter test", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    TEST_ASSERT_EQUAL(ESP_OK, ledc_channel_config(&ledc_ch_config));
}

TEST_CASE("LEDC wrong duty resolution", "[ledc]")
{
    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    ledc_time_config.duty_resolution = LEDC_TIMER_BIT_MAX;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ledc_timer_config(&ledc_time_config));
}

TEST_CASE("LEDC timer config wrong timer", "[ledc]")
{
    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    ledc_time_config.timer_num = LEDC_TIMER_MAX;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ledc_timer_config(&ledc_time_config));
}

TEST_CASE("LEDC timer config wrong speed mode", "[ledc]")
{
    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    ledc_time_config.speed_mode = LEDC_SPEED_MODE_MAX;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ledc_timer_config(&ledc_time_config));
}

TEST_CASE("LEDC timer config basic parameter test", "[ledc]")
{
    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
}

TEST_CASE("LEDC error log channel and timer config", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    uint32_t current_level = LEDC.channel_group[test_speed_mode].channel[LEDC_CHANNEL_0].conf0.idle_lv;
    TEST_ESP_OK(ledc_stop(test_speed_mode, LEDC_CHANNEL_0, !current_level));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(LEDC.channel_group[test_speed_mode].channel[LEDC_CHANNEL_0].conf0.idle_lv, !current_level);
}

TEST_CASE("LEDC iterate over all channel and timer configs", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_timer_config_t ledc_time_config = create_default_timer_config();

    // use all kinds of speed mode, channel, timer combination to test all of possible configuration
    ledc_mode_t speed_mode[LEDC_SPEED_MODE_MAX] = SPEED_MODE_LIST;
    ledc_channel_t channels[8] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3, LEDC_CHANNEL_4, LEDC_CHANNEL_5};
    ledc_timer_t timer_select[4] = {LEDC_TIMER_0, LEDC_TIMER_1, LEDC_TIMER_2, LEDC_TIMER_3};

    for (int i = 0; i < LEDC_SPEED_MODE_MAX; i++) {
        ledc_ch_config.speed_mode = speed_mode[i];
        ledc_time_config.speed_mode = speed_mode[i];
        for (int j = 0; j < 8; j++) {
            ledc_ch_config.channel = channels[j];
            for (int k = 0; k < 4; k++) {
                ledc_ch_config.timer_sel = timer_select[k];
                ledc_time_config.timer_num = timer_select[k];
                TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
                TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
            }
        }
    }
}

TEST_CASE("LEDC memory leak test", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    uint32_t size = esp_get_free_heap_size();
    uint32_t i;

    // install and uninstall for 1000 times to test whether memory leaking exists
    for (i = 0; i <= 1000; i++) {
        TEST_ESP_OK(ledc_fade_func_install(0));
        ledc_fade_func_uninstall();
    }
    TEST_ASSERT_INT32_WITHIN(100, size, esp_get_free_heap_size());
    TEST_ESP_OK(ledc_stop(ledc_time_config.speed_mode, LEDC_CHANNEL_0, 0));
}

// duty should be manually checked from the waveform using a logic analyzer
// this test is enabled only for testting the settings
TEST_CASE("LEDC set and get duty", "[ledc]")
{
    ledc_timer_t timer_list[4] = {LEDC_TIMER_0, LEDC_TIMER_1, LEDC_TIMER_2, LEDC_TIMER_3};
    ledc_mode_t speed_mode_list[LEDC_SPEED_MODE_MAX] = SPEED_MODE_LIST;
    for(int i=0; i<LEDC_TIMER_MAX-1; i++) {
        for(int j=0; j<LEDC_SPEED_MODE_MAX; j++) {
            timer_duty_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, timer_list[i], speed_mode_list[j]);
        }
    }
}

TEST_CASE("LEDC fade with time", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    fade_setup();

    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 4000, 200));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    TEST_ASSERT_EQUAL_INT32(4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 0, 200));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    // duty should not be too far from initial value
    TEST_ASSERT_INT32_WITHIN(20, 4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    vTaskDelay(210 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT32(0, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    //deinitialize fade service
    ledc_fade_func_uninstall();
}

TEST_CASE("LEDC fade with step", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    fade_setup();

    TEST_ESP_OK(ledc_set_fade_with_step(test_speed_mode, LEDC_CHANNEL_0, 4000, 4, 1));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    TEST_ASSERT_EQUAL_INT32(4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    TEST_ESP_OK(ledc_set_fade_with_step(test_speed_mode, LEDC_CHANNEL_0, 0, 4, 1));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    // duty should not be too far from initial value
    TEST_ASSERT_INT32_WITHIN(20, 4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    vTaskDelay(525 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT32(0, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    //scaler=0 check
    TEST_ASSERT(ledc_set_fade_with_step(test_speed_mode, LEDC_CHANNEL_0, 4000, 0, 1) == ESP_ERR_INVALID_ARG);

    //deinitialize fade service
    ledc_fade_func_uninstall();
}

TEST_CASE("LEDC fast switching duty with fade_wait_done", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    fade_setup();

    // fade function will block until fading to the target duty
    int64_t fade_start, fade_stop;
    fade_start = esp_timer_get_time();
    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 4000, 200));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    TEST_ASSERT_EQUAL_INT32(4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 1000, 150));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    TEST_ASSERT_EQUAL_INT32(1000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    fade_stop = esp_timer_get_time();
    int time_ms = (fade_stop - fade_start) / 1000;
    TEST_ASSERT_TRUE(fabs(time_ms - 350) < 20);

    // next duty update will not take place until last fade reaches its target duty
    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 4000, 200));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    TEST_ASSERT_EQUAL_INT32(4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    TEST_ESP_OK(ledc_set_duty(test_speed_mode, LEDC_CHANNEL_0, 500));
    TEST_ESP_OK(ledc_update_duty(test_speed_mode, LEDC_CHANNEL_0));
    vTaskDelay(5 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT32(500, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    //deinitialize fade service
    ledc_fade_func_uninstall();
}

TEST_CASE("LEDC fast switching duty with fade_no_wait", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    fade_setup();

    // fade function returns immediately, but next fade still needs to wait for last fade ends
    int64_t fade_start, first_fade_complete;
    fade_start = esp_timer_get_time();
    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 4000, 200));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    TEST_ASSERT_LESS_THAN(4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 1000, 150));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    first_fade_complete = esp_timer_get_time();
    // duty should not be too far from first fade target duty
    TEST_ASSERT_INT32_WITHIN(20, 4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    int time_ms = (first_fade_complete - fade_start) / 1000;
    TEST_ASSERT_TRUE(fabs(time_ms - 200) < 20);
    vTaskDelay(158 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT32(1000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    // next duty update will not take place until last fade reaches its target duty
    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 4000, 200));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    TEST_ASSERT_LESS_THAN(4000, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    TEST_ESP_OK(ledc_set_duty(test_speed_mode, LEDC_CHANNEL_0, 500));
    TEST_ESP_OK(ledc_update_duty(test_speed_mode, LEDC_CHANNEL_0));
    vTaskDelay(5 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT32(500, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    //deinitialize fade service
    ledc_fade_func_uninstall();
}

#if SOC_PCNT_SUPPORTED

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)

TEST_CASE("LEDC set and get frequency", "[ledc][test_env=UT_T1_LEDC][timeout=60][ignore]")
{
#if SOC_LEDC_SUPPORT_HS_MODE
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_1, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_2, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_3, LEDC_HIGH_SPEED_MODE);
#endif // SOC_LEDC_SUPPORT_HS_MODE
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_0, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_1, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_2, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_3, LEDC_LOW_SPEED_MODE);
}

TEST_CASE("LEDC timer set", "[ledc][test_env=UT_T1_LEDC]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = test_speed_mode,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 800,
        .hpoint = 0,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = test_speed_mode,
        .duty_resolution = 13,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_USE_APB_CLK,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    uint32_t freq_get;
    uint32_t count;
    //set timer 0 as 250Hz, use REF_TICK
    TEST_ESP_OK(ledc_timer_set(test_speed_mode, LEDC_TIMER_0, 1000, 10, LEDC_REF_TICK));
    TEST_ESP_OK(ledc_timer_rst(test_speed_mode, LEDC_TIMER_0));
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(test_speed_mode, LEDC_TIMER_0), 250);
    freq_get = ledc_get_freq(test_speed_mode, LEDC_TIMER_0);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(10, count, freq_get);

    //set timer 3 as 500Hz, use APB_CLK
    TEST_ESP_OK(ledc_timer_set(test_speed_mode, LEDC_TIMER_0, 5000, 13, LEDC_APB_CLK));
    TEST_ESP_OK(ledc_timer_rst(test_speed_mode, LEDC_TIMER_0));
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(test_speed_mode, LEDC_TIMER_0), 500);
    freq_get = ledc_get_freq(test_speed_mode, LEDC_TIMER_0);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(50, count, freq_get);

    printf("Bind channel 0 to timer 0\n");
    TEST_ESP_OK(ledc_bind_channel_timer(test_speed_mode, LEDC_CHANNEL_0, LEDC_TIMER_0));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(test_speed_mode, LEDC_TIMER_0), 500);

    uint32_t current_level = LEDC.channel_group[test_speed_mode].channel[LEDC_CHANNEL_0].conf0.idle_lv;
    TEST_ESP_OK(ledc_stop(test_speed_mode, LEDC_CHANNEL_0, !current_level));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32( LEDC.channel_group[test_speed_mode].channel[LEDC_CHANNEL_0].conf0.idle_lv, !current_level);
}

TEST_CASE("LEDC timer pause and resume", "[ledc][test_env=UT_T1_LEDC]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    int16_t count;
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = test_speed_mode,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 4000,
        .hpoint = 0,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = test_speed_mode,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_USE_APB_CLK,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    count = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(5, count, 5000);

    //pause ledc timer, when pause it, will get no waveform count
    printf("Pause ledc timer\n");
    TEST_ESP_OK(ledc_timer_pause(test_speed_mode, LEDC_TIMER_0));
    vTaskDelay(10 / portTICK_RATE_MS);
    count = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(5, count, 0);
    TEST_ASSERT_EQUAL_UINT32(count, 0);

    //resume ledc timer
    printf("Resume ledc timer\n");
    TEST_ESP_OK(ledc_timer_resume(test_speed_mode, LEDC_TIMER_0));
    vTaskDelay(10 / portTICK_RATE_MS);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(5, count, 5000);

    //reset ledc timer
    printf("reset ledc timer\n");
    TEST_ESP_OK(ledc_timer_rst(test_speed_mode, LEDC_TIMER_0));
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_UINT32_WITHIN(5, count, 5000);
}

static void ledc_cpu_reset_test_first_stage(void)
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    vTaskDelay(50 / portTICK_RATE_MS);
    esp_restart();
}

static void ledc_cpu_reset_test_second_stage(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
    int16_t count;
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(5, count, TEST_PWM_FREQ);
}

TEST_CASE_MULTIPLE_STAGES("LEDC software reset test",
                          "[ledc][test_env=UT_T1_LEDC]",
                          ledc_cpu_reset_test_first_stage,
                          ledc_cpu_reset_test_second_stage);

#endif  //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)

#endif // SOC_PCNT_SUPPORTED
