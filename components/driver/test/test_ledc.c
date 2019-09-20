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
#include "esp32/rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/ledc_periph.h"
#include "esp_system.h"
#include "driver/pcnt.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#define PULSE_IO 18
#define PCNT_INPUT_IO 4
#define PCNT_CTRL_FLOATING_IO 5
#define HIGHEST_LIMIT 10000
#define LOWEST_LIMIT -10000

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
    };
    ledc_timer_config_t ledc_time_config = {
        .speed_mode = speed_mode,
        .duty_resolution = timer_bit,
        .timer_num = timer,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    if(ledc_ch_config.speed_mode == LEDC_HIGH_SPEED_MODE) {
        frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 100, 100, 2);
        frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 5000, 5000, 5);
        frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 9000, 9025, 5);
    } else {
        frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 10, 10, 1);
//        comment it because this is a bug that when ledc in low speed mode, its frequency can't be changed
//        frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 50, 50, 2);
//        frequency_set_get(ledc_ch_config.speed_mode, ledc_ch_config.timer_sel, 100, 100, 2);
    }
}

static void timer_duty_set_get(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    TEST_ESP_OK(ledc_set_duty(speed_mode, channel, duty));
    TEST_ESP_OK(ledc_update_duty(speed_mode, channel));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_duty(speed_mode, channel), duty);
}

// use logic analyzer to view
static void timer_duty_test(ledc_channel_t channel, ledc_timer_bit_t timer_bit, ledc_timer_t timer, ledc_mode_t speed_mode)
{
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = speed_mode,
        .channel  = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer,
        .duty = 4000,
    };
    ledc_timer_config_t ledc_time_config = {
        .speed_mode = speed_mode,
        .duty_resolution = timer_bit,
        .timer_num = timer,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    // duty ratio: (2^duty)/(2^timer_bit)
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 0);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 1);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, 1 << 12); // 50% duty
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, (1 << 13) - 1);
    timer_duty_set_get(ledc_ch_config.speed_mode, ledc_ch_config.channel, (1 << 13) - 2);
}

TEST_CASE("LEDC error log channel and timer config", "[ledc][test_env=UT_T1_LEDC]")
{
    //channel configuration test
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 4000,
    };

    // basic right parameter test
    ledc_channel_config_t temp_ch_config = ledc_ch_config;
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    // with wrong GPIO using
    ledc_ch_config.gpio_num = 41;
    TEST_ASSERT(ledc_channel_config(&ledc_ch_config) == ESP_ERR_INVALID_ARG);

    // with wrong speed
    ledc_ch_config = temp_ch_config;
    ledc_ch_config.speed_mode = LEDC_SPEED_MODE_MAX;
    TEST_ASSERT(ledc_channel_config(&ledc_ch_config) == ESP_ERR_INVALID_ARG);

    // with wrong channel
    ledc_ch_config = temp_ch_config;
    ledc_ch_config.channel = LEDC_CHANNEL_MAX;
    TEST_ASSERT(ledc_channel_config(&ledc_ch_config) == ESP_ERR_INVALID_ARG);

    // with wrong interruption type
    ledc_ch_config = temp_ch_config;
    ledc_ch_config.intr_type = 2;
    TEST_ASSERT_FALSE((void *)ledc_channel_config(&ledc_ch_config));

    // with wrong timer
    ledc_ch_config = temp_ch_config;
    ledc_ch_config.timer_sel = 4;
    TEST_ASSERT(ledc_channel_config(&ledc_ch_config) == ESP_ERR_INVALID_ARG);

    ledc_ch_config = temp_ch_config;
    ledc_ch_config.duty = 12000;
    TEST_ASSERT_FALSE((void *)ledc_channel_config(&ledc_ch_config));

    // timer configuration test
    ledc_timer_config_t ledc_time_config;
    ledc_time_config.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_time_config.duty_resolution = LEDC_TIMER_13_BIT;
    ledc_time_config.timer_num = LEDC_TIMER_0;
    ledc_time_config.freq_hz = 5000;
    ledc_time_config.clk_cfg = LEDC_AUTO_CLK;

    ledc_timer_config_t temp_timer_config = ledc_time_config;
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    ledc_time_config.speed_mode = LEDC_SPEED_MODE_MAX;
    TEST_ASSERT(ledc_timer_config(&ledc_time_config) == ESP_ERR_INVALID_ARG);

    // wrong duty_resolution
    ledc_time_config = temp_timer_config;
    ledc_time_config.duty_resolution = LEDC_TIMER_BIT_MAX;
    TEST_ASSERT(ledc_timer_config(&ledc_time_config) == ESP_ERR_INVALID_ARG);

    // wrong timer
    ledc_time_config = temp_timer_config;
    ledc_time_config.timer_num = 4;
    TEST_ASSERT(ledc_timer_config(&ledc_time_config) == ESP_ERR_INVALID_ARG);

    uint32_t current_level = LEDC.channel_group[LEDC_HIGH_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.idle_lv;
    TEST_ESP_OK(ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, !current_level));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32( LEDC.channel_group[LEDC_HIGH_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.idle_lv, !current_level);
}

TEST_CASE("LEDC normal channel and timer config", "[ledc][test_env=UT_T1_LEDC]")
{
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 4000,
    };
    ledc_channel_config_t temp_ch_config = ledc_ch_config;

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config_t temp_time_config = ledc_time_config;

    // use all kinds of speed mode, channel, timer combination to test all of possible configuration
    ledc_mode_t speed_mode[2] = {LEDC_HIGH_SPEED_MODE, LEDC_LOW_SPEED_MODE};
    ledc_channel_t channel_type[8] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3, LEDC_CHANNEL_4, LEDC_CHANNEL_5, LEDC_CHANNEL_6, LEDC_CHANNEL_7};
    ledc_timer_t timer_select[4] = {LEDC_TIMER_0, LEDC_TIMER_1, LEDC_TIMER_2, LEDC_TIMER_3};

    for (int i = 0; i < 2; i++) {
        ledc_ch_config.speed_mode = speed_mode[i];
        ledc_time_config.speed_mode = speed_mode[i];
        for (int j = 0; j < 8; j++) {
            ledc_ch_config.channel = channel_type[j];
            for (int k = 0; k < 4; k++) {
                ledc_ch_config.timer_sel = timer_select[k];
                ledc_time_config.timer_num = timer_select[k];
                TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));
                TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
                ledc_ch_config = temp_ch_config;
                ledc_time_config = temp_time_config;
            }
        }
    }
}

// set it ignore: need to debug
TEST_CASE("LEDC set and get frequency", "[ledc][test_env=UT_T1_LEDC][timeout=60][ignore]")
{
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_1, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_2, LEDC_HIGH_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_3, LEDC_HIGH_SPEED_MODE);

    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_0, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_1, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_2, LEDC_LOW_SPEED_MODE);
    timer_frequency_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, LEDC_TIMER_3, LEDC_LOW_SPEED_MODE);
}

// the duty need to be detected by waveform given by the logic analyzer
// can't get it directly, so set it "ignore"
TEST_CASE("LEDC set and get dut(with logic analyzer)", "[ledc][ignore]")
{
    ledc_timer_t timer_list[4] = {LEDC_TIMER_0, LEDC_TIMER_1, LEDC_TIMER_2, LEDC_TIMER_3};
    ledc_mode_t speed_mode_list[2] = {LEDC_HIGH_SPEED_MODE, LEDC_LOW_SPEED_MODE};
    for(int i=0; i<LEDC_TIMER_MAX-1; i++) {
        for(int j=0; j<LEDC_SPEED_MODE_MAX-1; j++) {
            timer_duty_test(LEDC_CHANNEL_0, LEDC_TIMER_13_BIT, timer_list[i], speed_mode_list[j]);
        }
    }
}

TEST_CASE("LEDC timer set", "[ledc][test_env=UT_T1_LEDC]")
{
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 800,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = 13,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    uint32_t freq_get;
    uint32_t count;
    //set timer 0 as 250Hz, use REF_TICK
    TEST_ESP_OK(ledc_timer_set(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, 1000, 10, LEDC_REF_TICK));
    TEST_ESP_OK(ledc_timer_rst(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0), 250);
    freq_get = ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(10, count, freq_get);

    //set timer 3 as 500Hz, use APB_CLK
    TEST_ESP_OK(ledc_timer_set(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, 5000, 13, LEDC_APB_CLK));
    TEST_ESP_OK(ledc_timer_rst(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0), 500);
    freq_get = ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(50, count, freq_get);

    printf("Bind channel 0 to timer 0\n");
    TEST_ESP_OK(ledc_bind_channel_timer(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_TIMER_0));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0), 500);

    uint32_t current_level = LEDC.channel_group[LEDC_HIGH_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.idle_lv;
    TEST_ESP_OK(ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, !current_level));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32( LEDC.channel_group[LEDC_HIGH_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.idle_lv, !current_level);
}

TEST_CASE("LEDC timer pause and resume", "[ledc][test_env=UT_T1_LEDC]")
{
    int16_t count;
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 4000,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    count = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(5, count, 5000);

    //pause ledc timer, when pause it, will get no waveform count
    printf("Pause ledc timer\n");
    TEST_ESP_OK(ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    vTaskDelay(10 / portTICK_RATE_MS);
    count = wave_count(1000);
    TEST_ASSERT_INT16_WITHIN(5, count, 0);
    TEST_ASSERT_EQUAL_UINT32(count, 0);

    //resume ledc timer
    printf("Resume ledc timer\n");
    TEST_ESP_OK(ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    vTaskDelay(10 / portTICK_RATE_MS);
    count = wave_count(1000);
    TEST_ASSERT_UINT32_WITHIN(5, count, 5000);

    //reset ledc timer
    printf("reset ledc timer\n");
    TEST_ESP_OK(ledc_timer_rst(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_UINT32_WITHIN(5, count, 5000);
}

TEST_CASE("LEDC fade with time(logic analyzer)", "[ledc][ignore]")
{
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    //initialize fade service
    TEST_ESP_OK(ledc_fade_func_install(0));

    TEST_ESP_OK(ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 4000, 1000));
    TEST_ESP_OK(ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0), 4000);

    TEST_ESP_OK(ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0, 1000));
    TEST_ESP_OK(ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0), 0);

    //deinitial fade service
    ledc_fade_func_uninstall();
}

TEST_CASE("LEDC fade with step(logic analyzer)", "[ledc][ignore]")
{
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    //initialize fade service.
    TEST_ESP_OK(ledc_fade_func_install(0));

    TEST_ESP_OK(ledc_set_fade_with_step(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 4000, 2, 1));
    TEST_ESP_OK(ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0), 4000);

    TEST_ESP_OK(ledc_set_fade_with_step(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0, 4, 2));
    TEST_ESP_OK(ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT32(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0), 0);

    //scaler=0 check
    TEST_ASSERT(ledc_set_fade_with_step(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 4000, 0, 1) == ESP_ERR_INVALID_ARG);

    //deinitial fade service
    ledc_fade_func_uninstall();
}

// memory leaking problem detecting
TEST_CASE("LEDC memory test", "[ledc][test_env=UT_T1_LEDC]")
{
    ledc_channel_config_t ledc_ch_config = {
        .gpio_num = PULSE_IO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel  = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    ledc_timer_config_t ledc_time_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    uint32_t size = esp_get_free_heap_size();
    uint32_t i;

    // install and uninstall for 1000 times to test whether memory leaking exists
    for (i = 0; i <= 1000; i++) {
        TEST_ESP_OK(ledc_fade_func_install(0));
        ledc_fade_func_uninstall();
    }
    TEST_ASSERT_INT32_WITHIN(100, size, esp_get_free_heap_size());
    TEST_ESP_OK(ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0));
}


