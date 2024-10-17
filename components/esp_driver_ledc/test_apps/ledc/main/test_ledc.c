/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* LEDC tested by PCNT in some case
 * PCNT can get the LEDC waveform frequency
 *
 * some calculation related with duty:
 * real duty = duty/2^duty_resolution
 */
#include <esp_types.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "hal/gpio_ll.h"
#include "soc/gpio_periph.h"
#include "soc/io_mux_reg.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/ledc.h"
#include "soc/ledc_struct.h"
#include "esp_clk_tree.h"

#define PULSE_IO      5

#define TEST_PWM_FREQ 2000

#if SOC_LEDC_SUPPORT_HS_MODE
#define TEST_SPEED_MODE LEDC_HIGH_SPEED_MODE
#define SPEED_MODE_LIST {LEDC_HIGH_SPEED_MODE, LEDC_LOW_SPEED_MODE}
#else
#define TEST_SPEED_MODE LEDC_LOW_SPEED_MODE
#define SPEED_MODE_LIST {LEDC_LOW_SPEED_MODE}
#endif

#if SOC_LEDC_SUPPORT_APB_CLOCK
#define TEST_DEFAULT_CLK_CFG LEDC_USE_APB_CLK
#elif SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
#if SOC_CLK_TREE_SUPPORTED
#define TEST_DEFAULT_CLK_CFG LEDC_USE_PLL_DIV_CLK
#else
#define TEST_DEFAULT_CLK_CFG LEDC_USE_XTAL_CLK
#endif
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
    ledc_time_config.clk_cfg = TEST_DEFAULT_CLK_CFG;
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
    vTaskDelay(5 / portTICK_PERIOD_MS);
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
    vTaskDelay(5 / portTICK_PERIOD_MS);

    // duty ratio: (duty)/(2^timer_bit)
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 0);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 1);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 1 << (timer_bit - 1)); // 50% duty
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, (1 << timer_bit) - 1);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, (1 << timer_bit) - 2);
}

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

TEST_CASE("LEDC output idle level test", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    uint32_t current_level = LEDC.channel_group[test_speed_mode].channel[LEDC_CHANNEL_0].conf0.idle_lv;
    TEST_ESP_OK(ledc_stop(test_speed_mode, LEDC_CHANNEL_0, !current_level));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT32(!current_level, LEDC.channel_group[test_speed_mode].channel[LEDC_CHANNEL_0].conf0.idle_lv);
}

TEST_CASE("LEDC iterate over all channel and timer configs", "[ledc]")
{
    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_timer_config_t ledc_time_config = create_default_timer_config();

    // use all kinds of speed mode, channel, timer combination to test all of possible configuration
    ledc_mode_t speed_mode[LEDC_SPEED_MODE_MAX] = SPEED_MODE_LIST;

    for (int i = 0; i < LEDC_SPEED_MODE_MAX; i++) {
        ledc_ch_config.speed_mode = speed_mode[i];
        ledc_time_config.speed_mode = speed_mode[i];
        for (int j = 0; j < LEDC_CHANNEL_MAX; j++) {
            ledc_ch_config.channel = (ledc_channel_t)j;
            for (int k = 0; k < LEDC_TIMER_MAX; k++) {
                ledc_ch_config.timer_sel = (ledc_timer_t)k;
                ledc_time_config.timer_num = (ledc_timer_t)k;
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
// this test is enabled only for testing the settings
TEST_CASE("LEDC set and get duty", "[ledc]")
{
    ledc_timer_t timer_list[4] = {LEDC_TIMER_0, LEDC_TIMER_1, LEDC_TIMER_2, LEDC_TIMER_3};
    ledc_mode_t speed_mode_list[LEDC_SPEED_MODE_MAX] = SPEED_MODE_LIST;
    for (int i = 0; i < LEDC_TIMER_MAX; i++) {
        for (int j = 0; j < LEDC_SPEED_MODE_MAX; j++) {
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
    int64_t time_ms = (fade_stop - fade_start) / 1000;
    TEST_ASSERT_TRUE(llabs(time_ms - 350) < 20);

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
    int64_t time_ms = (first_fade_complete - fade_start) / 1000;
    TEST_ASSERT_TRUE(llabs(time_ms - 200) < 20);
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

#if SOC_LEDC_SUPPORT_FADE_STOP
TEST_CASE("LEDC fade stop test", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    fade_setup();

    int64_t fade_start, fade_stop;
    int64_t time_ms = 0;
    fade_start = esp_timer_get_time();
    TEST_ESP_OK(ledc_set_fade_with_time(test_speed_mode, LEDC_CHANNEL_0, 4000, 500));
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    // Add some delay before stopping the fade
    vTaskDelay(127 / portTICK_PERIOD_MS);
    // Get duty value right before stopping the fade
    uint32_t duty_before_stop = ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0);
    TEST_ESP_OK(ledc_fade_stop(test_speed_mode, LEDC_CHANNEL_0));
    // PWM signal is 2000 Hz. It may take one cycle (500 us) at maximum to stabilize the duty.
    esp_rom_delay_us(500);
    // Get duty value now, which is at least one cycle after the ledc_fade_stop function returns
    uint32_t duty_after_stop = ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0);
    fade_stop = esp_timer_get_time();
    time_ms = (fade_stop - fade_start) / 1000;
    TEST_ASSERT_TRUE(llabs(time_ms - 127) < 20);
    TEST_ASSERT_INT32_WITHIN(4, duty_before_stop, duty_after_stop); // 4 is the scale for one step in the last fade
    vTaskDelay(300 / portTICK_PERIOD_MS);
    // Duty should not change any more after ledc_fade_stop returns
    TEST_ASSERT_EQUAL_INT32(duty_after_stop, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));
    TEST_ASSERT_NOT_EQUAL(4000, duty_after_stop);

    //deinitialize fade service
    ledc_fade_func_uninstall();
}
#endif // SOC_LEDC_SUPPORT_FADE_STOP

#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
TEST_CASE("LEDC gamma ram write and read test", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    fade_setup();

    // Construct fade parameters
    ledc_fade_param_config_t *fade_params = (ledc_fade_param_config_t *) heap_caps_calloc(SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX, sizeof(ledc_fade_param_config_t), MALLOC_CAP_DEFAULT);
    for (int i = 0; i < SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX; i++) {
        fade_params[i].dir = (i + 1) % 2;
        fade_params[i].step_num = i + 1;
        fade_params[i].cycle_num = i + 2;
        fade_params[i].scale = i + 3;
    }

    // Write into gamma ram
    TEST_ESP_OK(ledc_set_multi_fade(test_speed_mode, LEDC_CHANNEL_0, 0, fade_params, SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX));

    // Read out from gamma ram and check correctness
    for (int i = 0; i < SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX; i++) {
        uint32_t dir, step, cycle, scale;
        ledc_read_fade_param(test_speed_mode, LEDC_CHANNEL_0, i, &dir, &cycle, &scale, &step);
        TEST_ASSERT_EQUAL_INT32((i + 1) % 2, dir);
        TEST_ASSERT_EQUAL_INT32(i + 1, step);
        TEST_ASSERT_EQUAL_INT32(i + 2, cycle);
        TEST_ASSERT_EQUAL_INT32(i + 3, scale);
    }

    // Deinitialize fade service
    ledc_fade_func_uninstall();
}

TEST_CASE("LEDC multi fade test", "[ledc]")
{
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    fade_setup();

    // Construct fade parameters
    const ledc_fade_param_config_t fade_params[] = {
        {.dir = 1, .step_num = 100, .cycle_num = 1, .scale = 1},
        {.dir = 1, .step_num = 50, .cycle_num = 2, .scale = 2},
        {.dir = 1, .step_num = 200, .cycle_num = 10, .scale = 5},
        {.dir = 0, .step_num = 100, .cycle_num = 5, .scale = 5},
        {.dir = 1, .step_num = 1000, .cycle_num = 1, .scale = 1},
        {.dir = 0, .step_num = 200, .cycle_num = 1, .scale = 1},
        {.dir = 1, .step_num = 1, .cycle_num = 1000, .scale = 1000},
    };
    uint32_t fade_range = 7;
    int32_t start_duty = 2000;
    int32_t end_duty = start_duty;
    uint32_t total_cycles = 0;
    for (int i = 0; i < fade_range; i++) {
        end_duty += ((fade_params[i].dir == 1) ? (1) : (-1)) * fade_params[i].step_num * fade_params[i].scale;
        total_cycles += fade_params[i].step_num * fade_params[i].cycle_num;
    }

    TEST_ESP_OK(ledc_set_multi_fade(test_speed_mode, LEDC_CHANNEL_0, start_duty, fade_params, fade_range));

    int64_t fade_start, fade_end;
    fade_start = esp_timer_get_time();
    TEST_ESP_OK(ledc_fade_start(test_speed_mode, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    fade_end = esp_timer_get_time();
    int64_t time_ms = (fade_end - fade_start) / 1000;
    // Check time escaped is expected
    // The time it takes to fade should exactly match with the given parameters, therefore, acceptable error range is small
    TEST_ASSERT_TRUE(llabs(time_ms - total_cycles * 1000 / TEST_PWM_FREQ) < 2);
    // Check the duty at the end of the fade
    TEST_ASSERT_EQUAL_INT32((uint32_t)end_duty, ledc_get_duty(test_speed_mode, LEDC_CHANNEL_0));

    // Deinitialize fade service
    ledc_fade_func_uninstall();
}
#endif // SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED

#if SOC_PCNT_SUPPORTED // Note. C61, C3, C2 do not have PCNT peripheral, the following test cases cannot be tested

#include "driver/pulse_cnt.h"

#define HIGHEST_LIMIT 10000
#define LOWEST_LIMIT -10000

static pcnt_unit_handle_t pcnt_unit;
static pcnt_channel_handle_t pcnt_chan;

static void setup_testbench(void)
{
    pcnt_unit_config_t unit_config = {
        .high_limit = HIGHEST_LIMIT,
        .low_limit = LOWEST_LIMIT,
    };
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &pcnt_unit));
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = PULSE_IO,
        .level_gpio_num = -1,
    };
    TEST_ESP_OK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
    TEST_ESP_OK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_unit_enable(pcnt_unit));
}

static void tear_testbench(void)
{
    TEST_ESP_OK(pcnt_unit_disable(pcnt_unit));
    TEST_ESP_OK(pcnt_del_channel(pcnt_chan));
    TEST_ESP_OK(pcnt_del_unit(pcnt_unit));
}

// use PCNT to test the waveform of LEDC
static int wave_count(int last_time)
{
    // The input ability of PULSE_IO is disabled after ledc driver install, so we need to re-enable it again
    gpio_ll_input_enable(&GPIO, PULSE_IO);
    int test_counter = 0;
    TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
    vTaskDelay(pdMS_TO_TICKS(last_time));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &test_counter));
    return test_counter;
}

// the PCNT will count the frequency of it
static void frequency_set_get(ledc_mode_t speed_mode, ledc_timer_t timer, uint32_t desired_freq, int16_t theoretical_freq, int16_t error)
{
    int real_freq;
    TEST_ESP_OK(ledc_set_freq(speed_mode, timer, desired_freq));
    vTaskDelay(10 / portTICK_PERIOD_MS);
    real_freq = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(error, theoretical_freq, real_freq);
    TEST_ASSERT_EQUAL_INT32(theoretical_freq, ledc_get_freq(speed_mode, timer));
}

static void timer_frequency_test(ledc_channel_t channel, ledc_timer_bit_t timer_bit, ledc_timer_t timer, ledc_mode_t speed_mode)
{
    ledc_timer_config_t ledc_time_config = {
        .speed_mode = speed_mode,
        .duty_resolution = timer_bit,
        .timer_num = timer,
        .freq_hz = TEST_PWM_FREQ,
        .clk_cfg = TEST_DEFAULT_CLK_CFG,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    TEST_ESP_OK(ledc_bind_channel_timer(speed_mode, channel, timer));

    TEST_ESP_OK(ledc_set_duty(speed_mode, channel, (1 << (timer_bit - 1)))); // 50% duty cycle
    TEST_ESP_OK(ledc_update_duty(speed_mode, channel));

    frequency_set_get(speed_mode, timer, 100, 100, 20);
#if SOC_CLK_TREE_SUPPORTED
    frequency_set_get(speed_mode, timer, 5000, 5000, 50);
    // Try a frequency that couldn't be exactly achieved, requires rounding
    uint32_t theoretical_freq = 9000;
    uint32_t clk_src_freq = 0;
    esp_clk_tree_src_get_freq_hz((soc_module_clk_t)TEST_DEFAULT_CLK_CFG, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &clk_src_freq);
    if (clk_src_freq == 80 * 1000 * 1000) {
        theoretical_freq = 8993;
    } else if (clk_src_freq == 96 * 1000 * 1000) {
        theoretical_freq = 9009;
    }
    frequency_set_get(speed_mode, timer, 9000, theoretical_freq, 50);
#endif

    // Pause and de-configure the timer so that it won't affect the following test cases
    TEST_ESP_OK(ledc_timer_pause(speed_mode, timer));
    ledc_time_config.deconfigure = 1;
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
}

TEST_CASE("LEDC set and get frequency", "[ledc][timeout=60]")
{
    setup_testbench();

    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
#if SOC_LEDC_SUPPORT_HS_MODE
    ledc_ch_config.speed_mode = LEDC_HIGH_SPEED_MODE;
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_1, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_2, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_3, LEDC_HIGH_SPEED_MODE);
#endif // SOC_LEDC_SUPPORT_HS_MODE
    ledc_ch_config.speed_mode = LEDC_LOW_SPEED_MODE;
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_0, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_1, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_2, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_3, LEDC_LOW_SPEED_MODE);

    tear_testbench();
}

#if SOC_CLK_TREE_SUPPORTED
static void timer_set_clk_src_and_freq_test(ledc_mode_t speed_mode, ledc_clk_cfg_t clk_src, uint32_t duty_res,
                                            uint32_t freq_hz)
{
    ledc_timer_config_t ledc_time_config = {
        .speed_mode = speed_mode,
        .duty_resolution = duty_res,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = freq_hz,
        .clk_cfg = clk_src,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    TEST_ESP_OK(ledc_update_duty(speed_mode, LEDC_CHANNEL_0)); // Start
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (clk_src == LEDC_USE_RC_FAST_CLK) {
        // RC_FAST_CLK freq is get from calibration, it is reasonable that divider calculation does a rounding
        TEST_ASSERT_UINT32_WITHIN(5, freq_hz, ledc_get_freq(speed_mode, LEDC_TIMER_0));
    } else {
        TEST_ASSERT_EQUAL_INT32(freq_hz, ledc_get_freq(speed_mode, LEDC_TIMER_0));
    }
    int count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(10, freq_hz, count);
}

TEST_CASE("LEDC timer select specific clock source", "[ledc]")
{
    setup_testbench();
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

    if (test_speed_mode == LEDC_LOW_SPEED_MODE) {
#if SOC_CLK_RC_FAST_SUPPORT_CALIBRATION // Otherwise, the frequency of output PWM signal may not be very accurate
        printf("Check LEDC_USE_RC_FAST_CLK for a 100Hz signal\n");
        timer_set_clk_src_and_freq_test(test_speed_mode, LEDC_USE_RC_FAST_CLK, 10, 100);
#endif
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
        printf("Check LEDC_USE_XTAL_CLK for a 400Hz signal\n");
        timer_set_clk_src_and_freq_test(test_speed_mode, LEDC_USE_XTAL_CLK, 13, 400);
#endif
    }
#if SOC_LEDC_SUPPORT_REF_TICK
    printf("Check LEDC_USE_REF_TICK for a 250Hz signal\n");
    timer_set_clk_src_and_freq_test(test_speed_mode, LEDC_USE_REF_TICK, 10, 250);
#endif
#if SOC_LEDC_SUPPORT_APB_CLOCK
    printf("Check LEDC_USE_APB_CLK for a 500Hz signal\n");
    timer_set_clk_src_and_freq_test(test_speed_mode, LEDC_USE_APB_CLK, 13, 500);
#endif
#if SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
    printf("Check LEDC_USE_PLL_DIV_CLK for a 500Hz signal\n");
    timer_set_clk_src_and_freq_test(test_speed_mode, LEDC_USE_PLL_DIV_CLK, 13, 500);
#endif

    printf("Bind channel 0 to timer 0\n");
    TEST_ESP_OK(ledc_bind_channel_timer(test_speed_mode, LEDC_CHANNEL_0, LEDC_TIMER_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(test_speed_mode, LEDC_TIMER_0), 500);

    tear_testbench();
}
#endif  //SOC_CLK_TREE_SUPPORTED

TEST_CASE("LEDC timer pause and resume", "[ledc]")
{
    setup_testbench();
    const ledc_mode_t test_speed_mode = TEST_SPEED_MODE;
    int count;

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = test_speed_mode,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = TEST_PWM_FREQ,
        .clk_cfg = TEST_DEFAULT_CLK_CFG,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

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

    vTaskDelay(10 / portTICK_PERIOD_MS);
    count = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(5, TEST_PWM_FREQ, count);

    //pause ledc timer, when pause it, will get no waveform count
    printf("Pause ledc timer\n");
    TEST_ESP_OK(ledc_timer_pause(test_speed_mode, LEDC_TIMER_0));
    vTaskDelay(10 / portTICK_PERIOD_MS);
    count = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(5, 0, count);

    //resume ledc timer
    printf("Resume ledc timer\n");
    TEST_ESP_OK(ledc_timer_resume(test_speed_mode, LEDC_TIMER_0));
    vTaskDelay(10 / portTICK_PERIOD_MS);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(5, TEST_PWM_FREQ, count);

    //reset ledc timer
    printf("reset ledc timer\n");
    TEST_ESP_OK(ledc_timer_rst(test_speed_mode, LEDC_TIMER_0));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(5, TEST_PWM_FREQ, count);
    tear_testbench();
}

static void ledc_cpu_reset_test_first_stage(void)
{
    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    vTaskDelay(50 / portTICK_PERIOD_MS);
    esp_restart();
}

static void ledc_cpu_reset_test_second_stage(void)
{
    int count;
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
    setup_testbench();
    // reconfigure the GPIO again, as the GPIO output ability has been disabled during initialize pcnt peripheral
    ledc_set_pin(PULSE_IO, TEST_SPEED_MODE, LEDC_CHANNEL_0);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(5, TEST_PWM_FREQ, count);
    tear_testbench();
}

TEST_CASE_MULTIPLE_STAGES("LEDC continue work after software reset", "[ledc]",
                          ledc_cpu_reset_test_first_stage,
                          ledc_cpu_reset_test_second_stage);

#endif // SOC_PCNT_SUPPORTED
