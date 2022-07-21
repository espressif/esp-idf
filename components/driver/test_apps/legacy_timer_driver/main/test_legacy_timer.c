/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "unity.h"
#include "driver/timer.h"
#include "esp_private/esp_clk.h"
#include "soc/soc_caps.h"
#include "esp_rom_sys.h"
#include "soc/soc.h"

#define TEST_TIMER_RESOLUTION_HZ 1000000 // 1MHz resolution
#define TIMER_DELTA              0.001

static bool alarm_flag;
static QueueHandle_t timer_queue;

typedef struct {
    timer_group_t timer_group;
    timer_idx_t timer_idx;
} timer_info_t;

typedef struct {
    timer_autoreload_t type;  // the type of timer's event
    timer_group_t timer_group;
    timer_idx_t timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

#define TIMER_INFO_INIT(TG, TID)    {.timer_group = (TG), .timer_idx = (TID),}

static timer_info_t timer_info[] = {
#if SOC_TIMER_GROUP_TOTAL_TIMERS >= 4
    TIMER_INFO_INIT(TIMER_GROUP_0, TIMER_0),
    TIMER_INFO_INIT(TIMER_GROUP_0, TIMER_1),
    TIMER_INFO_INIT(TIMER_GROUP_1, TIMER_0),
    TIMER_INFO_INIT(TIMER_GROUP_1, TIMER_1),
#elif SOC_TIMER_GROUP_TOTAL_TIMERS >= 2
    TIMER_INFO_INIT(TIMER_GROUP_0, TIMER_0),
    TIMER_INFO_INIT(TIMER_GROUP_1, TIMER_0),
#else
    TIMER_INFO_INIT(TIMER_GROUP_0, TIMER_0),
#endif
};

static intr_handle_t timer_isr_handles[SOC_TIMER_GROUP_TOTAL_TIMERS];

#define GET_TIMER_INFO(TG, TID) (&timer_info[(TG)*SOC_TIMER_GROUP_TIMERS_PER_GROUP+(TID)])

// timer group interruption handle callback
static bool test_timer_group_isr_cb(void *arg)
{
    bool is_awoken = false;
    timer_info_t *info = (timer_info_t *) arg;
    const timer_group_t timer_group = info->timer_group;
    const timer_idx_t timer_idx = info->timer_idx;
    uint64_t timer_val;
    double time;
    uint64_t alarm_value;
    timer_event_t evt;
    alarm_flag = true;
    if (timer_group_get_auto_reload_in_isr(timer_group, timer_idx)) { // For autoreload mode, the counter value has been cleared
        timer_group_clr_intr_status_in_isr(timer_group, timer_idx);
        esp_rom_printf("This is TG%d timer[%d] reload-timer alarm!\n", timer_group, timer_idx);
        timer_get_counter_value(timer_group, timer_idx, &timer_val);
        timer_get_counter_time_sec(timer_group, timer_idx, &time);
        evt.type = TIMER_AUTORELOAD_EN;
    } else {
        timer_group_clr_intr_status_in_isr(timer_group, timer_idx);
        esp_rom_printf("This is TG%d timer[%d] count-up-timer alarm!\n", timer_group, timer_idx);
        timer_get_counter_value(timer_group, timer_idx, &timer_val);
        timer_get_counter_time_sec(timer_group, timer_idx, &time);
        timer_get_alarm_value(timer_group, timer_idx, &alarm_value);
        timer_set_counter_value(timer_group, timer_idx, 0);
        evt.type = TIMER_AUTORELOAD_DIS;
    }
    evt.timer_group = timer_group;
    evt.timer_idx = timer_idx;
    evt.timer_counter_value = timer_val;
    if (timer_queue != NULL) {
        BaseType_t awoken = pdFALSE;
        BaseType_t ret = xQueueSendFromISR(timer_queue, &evt, &awoken);
        TEST_ASSERT_EQUAL(pdTRUE, ret);
        if (awoken) {
            is_awoken = true;
        }
    }
    return is_awoken;
}

// timer group interruption handle
static void test_timer_group_isr(void *arg)
{
    if (test_timer_group_isr_cb(arg)) {
        portYIELD_FROM_ISR();
    }
}

// initialize all timer
static void all_timer_init(timer_config_t *config, bool expect_init)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ASSERT_EQUAL((expect_init ? ESP_OK : ESP_ERR_INVALID_ARG), timer_init(tg_idx, timer_idx, config));
        }
    }
    if (timer_queue == NULL) {
        timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    }
}

// deinitialize all timer
static void all_timer_deinit(void)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_deinit(tg_idx, timer_idx));
        }
    }
    if (timer_queue != NULL) {
        vQueueDelete(timer_queue);
        timer_queue = NULL;
    }
}

// start all of timer
static void all_timer_start(void)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_start(tg_idx, timer_idx));
        }
    }
}

static void all_timer_set_counter_value(uint64_t set_cnt_val)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_set_counter_value(tg_idx, timer_idx, set_cnt_val));
        }
    }
}

static void all_timer_pause(void)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_pause(tg_idx, timer_idx));
        }
    }
}

static void all_timer_get_counter_value(uint64_t set_cnt_val, bool expect_equal_set_val,
                                        uint64_t *actual_cnt_val)
{
    uint64_t current_cnt_val;
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_get_counter_value(tg_idx, timer_idx, &current_cnt_val));
            if (expect_equal_set_val) {
                TEST_ASSERT_EQUAL(set_cnt_val, current_cnt_val);
            } else {
                TEST_ASSERT_NOT_EQUAL(set_cnt_val, current_cnt_val);
                if (actual_cnt_val != NULL) {
                    actual_cnt_val[tg_idx * SOC_TIMER_GROUP_TIMERS_PER_GROUP + timer_idx] = current_cnt_val;
                }
            }
        }
    }
}

static void all_timer_get_counter_time_sec(int expect_time)
{
    double time;
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_get_counter_time_sec(tg_idx, timer_idx, &time));
            TEST_ASSERT_FLOAT_WITHIN(TIMER_DELTA, expect_time, time);
        }
    }
}

static void all_timer_set_counter_mode(timer_count_dir_t counter_dir)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_set_counter_mode(tg_idx, timer_idx, counter_dir));
        }
    }
}

static void all_timer_set_divider(uint32_t divider)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_set_divider(tg_idx, timer_idx, divider));
        }
    }
}

static void all_timer_set_alarm_value(uint64_t alarm_cnt_val)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_set_alarm_value(tg_idx, timer_idx, alarm_cnt_val));
        }
    }
}

static void all_timer_get_alarm_value(uint64_t *alarm_vals)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_get_alarm_value(tg_idx, timer_idx, &alarm_vals[tg_idx * SOC_TIMER_GROUP_TIMERS_PER_GROUP + timer_idx]));
        }
    }
}

static void all_timer_isr_reg(void)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(timer_isr_register(tg_idx, timer_idx, test_timer_group_isr,
                                           GET_TIMER_INFO(tg_idx, timer_idx), ESP_INTR_FLAG_LOWMED, &timer_isr_handles[tg_idx * SOC_TIMER_GROUP_TIMERS_PER_GROUP + timer_idx]));
        }
    }
}

static void all_timer_isr_unreg(void)
{
    for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
        for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
            TEST_ESP_OK(esp_intr_free(timer_isr_handles[tg_idx * SOC_TIMER_GROUP_TIMERS_PER_GROUP + timer_idx]));
        }
    }
}

// enable interrupt and start timer
static void timer_intr_enable_and_start(int timer_group, int timer_idx, double alarm_time)
{
    TEST_ESP_OK(timer_pause(timer_group, timer_idx));
    TEST_ESP_OK(timer_set_counter_value(timer_group, timer_idx, 0x0));
    TEST_ESP_OK(timer_set_alarm_value(timer_group, timer_idx, alarm_time * TEST_TIMER_RESOLUTION_HZ));
    TEST_ESP_OK(timer_set_alarm(timer_group, timer_idx, TIMER_ALARM_EN));
    TEST_ESP_OK(timer_enable_intr(timer_group, timer_idx));
    TEST_ESP_OK(timer_start(timer_group, timer_idx));
}

static void timer_isr_check(timer_group_t group_num, timer_idx_t timer_num, timer_autoreload_t autoreload, uint64_t alarm_cnt_val)
{
    timer_event_t evt;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(timer_queue, &evt, 3000 / portTICK_PERIOD_MS));
    TEST_ASSERT_EQUAL(autoreload, evt.type);
    TEST_ASSERT_EQUAL(group_num, evt.timer_group);
    TEST_ASSERT_EQUAL(timer_num, evt.timer_idx);
    TEST_ASSERT_EQUAL((uint32_t)(alarm_cnt_val >> 32), (uint32_t)(evt.timer_counter_value >> 32));
    TEST_ASSERT_UINT32_WITHIN(1000, (uint32_t)(alarm_cnt_val), (uint32_t)(evt.timer_counter_value));
}

static void timer_intr_enable_disable_test(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_cnt_val)
{
    alarm_flag = false;
    TEST_ESP_OK(timer_set_counter_value(group_num, timer_num, 0));
    TEST_ESP_OK(timer_set_alarm(group_num, timer_num, TIMER_ALARM_EN));
    TEST_ESP_OK(timer_enable_intr(group_num, timer_num));
    TEST_ESP_OK(timer_start(group_num, timer_num));
    timer_isr_check(group_num, timer_num, TIMER_AUTORELOAD_DIS, alarm_cnt_val);
    TEST_ASSERT_EQUAL(true, alarm_flag);

    // disable interrupt of tg0_timer0
    alarm_flag = false;
    TEST_ESP_OK(timer_pause(group_num, timer_num));
    TEST_ESP_OK(timer_set_counter_value(group_num, timer_num, 0));
    TEST_ESP_OK(timer_disable_intr(group_num, timer_num));
    TEST_ESP_OK(timer_start(group_num, timer_num));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(false, alarm_flag);
}

TEST_CASE("Timer_init", "[hw_timer]")
{
    // Test init 1:config parameter
    // empty parameter
    timer_config_t config0 = { };
    all_timer_init(&config0, false);

    // only one parameter
    timer_config_t config1 = {
        .auto_reload = TIMER_AUTORELOAD_EN
    };
    all_timer_init(&config1, false);

    // lack one parameter
    timer_config_t config2 = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(&config2, true);

    config2.counter_en = TIMER_PAUSE;
    all_timer_init(&config2, true);

    // Test init 2:  init
    uint64_t set_timer_val = 0x0;
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL
    };

    // judge get config parameters
    timer_config_t get_config;
    TEST_ESP_OK(timer_init(TIMER_GROUP_0, TIMER_0, &config));
    TEST_ESP_OK(timer_get_config(TIMER_GROUP_0, TIMER_0, &get_config));
    TEST_ASSERT_EQUAL(config.alarm_en, get_config.alarm_en);
    TEST_ASSERT_EQUAL(config.auto_reload, get_config.auto_reload);
    TEST_ASSERT_EQUAL(config.counter_dir, get_config.counter_dir);
    TEST_ASSERT_EQUAL(config.counter_en, get_config.counter_en);
    TEST_ASSERT_EQUAL(config.intr_type, get_config.intr_type);
    TEST_ASSERT_EQUAL(config.divider, get_config.divider);

    all_timer_init(&config, true);
    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    all_timer_get_counter_value(set_timer_val, false, NULL);

    // Test init 3:  wrong parameter
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_init(-1, TIMER_0, &config));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_init(TIMER_GROUP_0, 2, &config));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_init(TIMER_GROUP_0, -1, &config));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_init(2, TIMER_0, &config));
    all_timer_deinit();
}

/**
 * read count case:
 * 1. start timer compare value
 * 2. pause timer compare value
 * 3. delay some time */
TEST_CASE("Timer_read_counter_value", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;

    all_timer_init(&config, true);

    // Test read value 1: start timer get counter value
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    all_timer_get_counter_value(set_timer_val, false, NULL);

    // Test read value 2: pause timer get counter value
    all_timer_pause();
    set_timer_val = 0x30405000ULL;
    all_timer_set_counter_value(set_timer_val);
    all_timer_get_counter_value(set_timer_val, true, NULL);

    // Test read value 3:delay 1s get counter value
    set_timer_val = 0x0;
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_time_sec(1);
    all_timer_deinit();
}

/**
 * start timer case:
 * 1. normal start
 * 2. error start parameter
 * */
TEST_CASE("Timer_start", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(&config, true);

    //Test start 1: normal start
    all_timer_start();
    all_timer_set_counter_value(set_timer_val);
    all_timer_get_counter_value(set_timer_val, false, NULL);

    //Test start 2:wrong parameter
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_start(2, TIMER_0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_start(-1, TIMER_0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_start(TIMER_GROUP_0, 2));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_start(TIMER_GROUP_0, -1));
    all_timer_deinit();
}

/**
 * pause timer case:
 * 1. normal pause, read value
 * 2. error pause error
 */
TEST_CASE("Timer_pause", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(&config, true);

    //Test pause 1: right parameter
    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);
    all_timer_get_counter_value(set_timer_val, true, NULL);

    //Test pause 2: wrong parameter
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_pause(-1, TIMER_0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_pause(TIMER_GROUP_0, -1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_pause(2, TIMER_0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_pause(TIMER_GROUP_0, 2));
    all_timer_deinit();
}

// positive mode and negative mode
TEST_CASE("Timer_counter_direction", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(&config, true);
    all_timer_pause();

    // Test counter mode 1: TIMER_COUNT_UP
    all_timer_set_counter_mode(TIMER_COUNT_UP);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_time_sec(1);

    // Test counter mode 2: TIMER_COUNT_DOWN
    all_timer_pause();
    set_timer_val = TEST_TIMER_RESOLUTION_HZ * 3; // 3s clock counter value
    all_timer_set_counter_mode(TIMER_COUNT_DOWN);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_time_sec(2);

    // Test counter mode 3 : wrong parameter
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_set_counter_mode(TIMER_GROUP_0, TIMER_0, -1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_set_counter_mode(TIMER_GROUP_0, TIMER_0, 2));
    all_timer_deinit();
}

TEST_CASE("Timer_divider", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0;
    uint64_t time_val[TIMER_GROUP_MAX * TIMER_MAX];
    uint64_t comp_time_val[TIMER_GROUP_MAX * TIMER_MAX];
    all_timer_init(&config, true);

    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);

    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_value(set_timer_val, false, time_val);

    all_timer_pause();
    all_timer_set_divider(config.divider / 2); // half of original divider
    all_timer_set_counter_value(set_timer_val);

    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);         //delay the same time
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (int i = 0; i < TIMER_GROUP_MAX * TIMER_MAX; i++) {
        TEST_ASSERT_INT_WITHIN(2000, 1000000, time_val[i]);
        TEST_ASSERT_INT_WITHIN(2000, 2000000, comp_time_val[i]);
    }

    all_timer_pause();
    all_timer_set_divider(256);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);         //delay the same time
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (int i = 0; i < TIMER_GROUP_MAX * TIMER_MAX; i++) {
        TEST_ASSERT_INT_WITHIN(100, APB_CLK_FREQ / 256, comp_time_val[i]);
    }

    all_timer_pause();
    all_timer_set_divider(2);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (int i = 0; i < TIMER_GROUP_MAX * TIMER_MAX; i++) {
        TEST_ASSERT_INT_WITHIN(5000, APB_CLK_FREQ / 2, comp_time_val[i]);
    }

    all_timer_pause();
    all_timer_set_divider(65536);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);         //delay the same time
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (int i = 0; i < TIMER_GROUP_MAX * TIMER_MAX; i++) {
        TEST_ASSERT_INT_WITHIN(10, APB_CLK_FREQ / 65536, comp_time_val[i]);
    }

    all_timer_pause();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_set_divider(TIMER_GROUP_0, TIMER_0, 1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, timer_set_divider(TIMER_GROUP_0, TIMER_0, 65537));
    all_timer_deinit();
}

/**
 * enable alarm case:
 * 1. enable alarm ,set alarm value and get value
 * 2. disable alarm ,set alarm value and get value
 */
TEST_CASE("Timer_enable_alarm", "[hw_timer]")
{
    timer_config_t config_test = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(&config_test, true);
    all_timer_isr_reg();

    // enable alarm of tg0_timer1
    alarm_flag = false;
    TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN));
    timer_intr_enable_and_start(TIMER_GROUP_0, TIMER_0, 1.2);
    timer_isr_check(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_DIS, 1.2 * TEST_TIMER_RESOLUTION_HZ);
    TEST_ASSERT_EQUAL(true, alarm_flag);

    // disable alarm of tg0_timer1
    alarm_flag = false;
    timer_intr_enable_and_start(TIMER_GROUP_0, TIMER_0, 1.2);
    TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_DIS));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(false, alarm_flag);

#if SOC_TIMER_GROUPS > 1
    // enable alarm of tg1_timer0
    alarm_flag = false;
    TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_1, TIMER_0, TIMER_ALARM_EN));
    timer_intr_enable_and_start(TIMER_GROUP_1, TIMER_0, 1.2);
    timer_isr_check(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_DIS, 1.2 * TEST_TIMER_RESOLUTION_HZ);
    TEST_ASSERT_EQUAL(true, alarm_flag);

    // disable alarm of tg1_timer0
    alarm_flag = false;
    timer_intr_enable_and_start(TIMER_GROUP_1, TIMER_0, 1.2);
    TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_1, TIMER_0, TIMER_ALARM_DIS));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(false, alarm_flag);
#endif
    all_timer_isr_unreg();
    all_timer_deinit();
}

/**
 * alarm value case:
 * 1. set alarm value and get value
 * 2. interrupt test time
 */
TEST_CASE("Timer_set_alarm_value", "[hw_timer]")
{
    uint64_t alarm_val[SOC_TIMER_GROUP_TOTAL_TIMERS];
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(&config, true);
    all_timer_isr_reg();

    // set and get alarm value
    all_timer_set_alarm_value(3 * TEST_TIMER_RESOLUTION_HZ);
    all_timer_get_alarm_value(alarm_val);
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT_EQUAL_UINT32(3 * TEST_TIMER_RESOLUTION_HZ, (uint32_t)alarm_val[i]);
    }

    // set interrupt read alarm value
    timer_intr_enable_and_start(TIMER_GROUP_0, TIMER_0, 2.4);
    timer_isr_check(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_DIS, 2.4 * TEST_TIMER_RESOLUTION_HZ);
#if SOC_TIMER_GROUPS > 1
    timer_intr_enable_and_start(TIMER_GROUP_1, TIMER_0, 1.4);
    timer_isr_check(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_DIS, 1.4 * TEST_TIMER_RESOLUTION_HZ);
#endif
    all_timer_isr_unreg();
    all_timer_deinit();
}

/**
 * auto reload case:
 * 1. no reload
 * 2. auto reload
 */
TEST_CASE("Timer_auto_reload", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(&config, true);
    all_timer_isr_reg();

    // test disable auto_reload
    timer_intr_enable_and_start(TIMER_GROUP_0, TIMER_0, 1.14);
    timer_isr_check(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_DIS, 1.14 * TEST_TIMER_RESOLUTION_HZ);
#if SOC_TIMER_GROUPS > 1
    timer_intr_enable_and_start(TIMER_GROUP_1, TIMER_0, 1.14);
    timer_isr_check(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_DIS, 1.14 * TEST_TIMER_RESOLUTION_HZ);
#endif

    //test enable auto_reload
    TEST_ESP_OK(timer_set_auto_reload(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN));
    timer_intr_enable_and_start(TIMER_GROUP_0, TIMER_0, 1.4);
    timer_isr_check(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN, 0);
#if SOC_TIMER_GROUPS > 1
    TEST_ESP_OK(timer_set_auto_reload(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_EN));
    timer_intr_enable_and_start(TIMER_GROUP_1, TIMER_0, 1.4);
    timer_isr_check(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_EN, 0);
#endif
    all_timer_isr_unreg();
    all_timer_deinit();
}

/**
 * timer_enable_intr case:
 * 1. enable timer_intr
 * 2. disable timer_intr
 */
TEST_CASE("Timer_enable_timer_interrupt", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };

    all_timer_init(&config, true);
    all_timer_pause();
    all_timer_set_alarm_value(1.2 * TEST_TIMER_RESOLUTION_HZ);
    all_timer_set_counter_value(0);
    all_timer_isr_reg();
    timer_intr_enable_disable_test(TIMER_GROUP_0, TIMER_0, 1.2 * TEST_TIMER_RESOLUTION_HZ);
#if SOC_TIMER_GROUPS > 1
    timer_intr_enable_disable_test(TIMER_GROUP_1, TIMER_0, 1.2 * TEST_TIMER_RESOLUTION_HZ);
#endif

    // enable interrupt of tg0_timer0 again
    alarm_flag = false;
    TEST_ESP_OK(timer_pause(TIMER_GROUP_0, TIMER_0));
    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0));
    TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN));
    TEST_ESP_OK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    TEST_ESP_OK(timer_start(TIMER_GROUP_0, TIMER_0));
    timer_isr_check(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_DIS, 1.2 * TEST_TIMER_RESOLUTION_HZ);
    TEST_ASSERT_EQUAL(true, alarm_flag);
    all_timer_isr_unreg();
    all_timer_deinit();
}

/**
 * enable timer group case:
 * 1. enable timer group
 * 2. disable timer group
 */
TEST_CASE("Timer_enable_timer_group_interrupt", "[hw_timer][ignore]")
{
    intr_handle_t isr_handle = NULL;
    alarm_flag = false;
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(&config, true);
    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);
    all_timer_set_alarm_value(1.2 * TEST_TIMER_RESOLUTION_HZ);

    // enable interrupt of tg0_timer0
    TEST_ESP_OK(timer_group_intr_enable(TIMER_GROUP_0, TIMER_INTR_T0));
    TEST_ESP_OK(timer_isr_register(TIMER_GROUP_0, TIMER_0, test_timer_group_isr,
                                   GET_TIMER_INFO(TIMER_GROUP_0, TIMER_0), ESP_INTR_FLAG_LOWMED, &isr_handle));
    TEST_ESP_OK(timer_start(TIMER_GROUP_0, TIMER_0));
    timer_isr_check(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_DIS, 1.2 * TEST_TIMER_RESOLUTION_HZ);
    TEST_ASSERT_EQUAL(true, alarm_flag);

    // disable interrupt of tg0_timer0
    alarm_flag = false;
    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, set_timer_val));
    TEST_ESP_OK(timer_group_intr_disable(TIMER_GROUP_0, TIMER_INTR_T0));
    TEST_ESP_OK(timer_start(TIMER_GROUP_0, TIMER_0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(false, alarm_flag);
    esp_intr_free(isr_handle);
}

/**
 * isr_register case:
 * Cycle register 15 times, compare the heap size to ensure no memory leaks
 */
TEST_CASE("Timer_interrupt_register", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };

    for (int i = 0; i < 15; i++) {
        all_timer_init(&config, true);
        timer_isr_handle_t timer_isr_handle[TIMER_GROUP_MAX * TIMER_MAX];
        for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
            for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
                TEST_ESP_OK(timer_isr_register(tg_idx, timer_idx, test_timer_group_isr,
                                               GET_TIMER_INFO(tg_idx, timer_idx), ESP_INTR_FLAG_LOWMED, &timer_isr_handle[tg_idx * SOC_TIMER_GROUP_TIMERS_PER_GROUP + timer_idx]));
            }
        }

        TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN));
        timer_intr_enable_and_start(TIMER_GROUP_0, TIMER_0, 0.54);
#if SOC_TIMER_GROUPS > 1
        TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_1, TIMER_0, TIMER_ALARM_EN));
        timer_intr_enable_and_start(TIMER_GROUP_1, TIMER_0, 0.34);
#endif

        TEST_ESP_OK(timer_set_auto_reload(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN));
        TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN));
        timer_intr_enable_and_start(TIMER_GROUP_0, TIMER_0, 0.4);
#if SOC_TIMER_GROUPS > 1
        TEST_ESP_OK(timer_set_auto_reload(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_EN));
        TEST_ESP_OK(timer_set_alarm(TIMER_GROUP_1, TIMER_0, TIMER_ALARM_EN));
        timer_intr_enable_and_start(TIMER_GROUP_1, TIMER_0, 0.6);
#endif
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // ISR hanlde function should be free before next ISR register.
        for (uint32_t tg_idx = 0; tg_idx < TIMER_GROUP_MAX; tg_idx++) {
            for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
                TEST_ESP_OK(esp_intr_free(timer_isr_handle[tg_idx * SOC_TIMER_GROUP_TIMERS_PER_GROUP + timer_idx]));
            }
        }
        all_timer_deinit();
    }
}

#if SOC_TIMER_GROUP_SUPPORT_XTAL
/**
 * Timer clock source:
 * 1. configure clock source as APB clock, and enable timer interrupt
 * 2. configure clock source as XTAL clock, adn enable timer interrupt
 */
TEST_CASE("Timer_clock_source", "[hw_timer]")
{
    // configure clock source as APB clock
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
    };
    all_timer_init(&config, true);
    all_timer_pause();
    all_timer_set_alarm_value(1.2 * TEST_TIMER_RESOLUTION_HZ);
    all_timer_set_counter_value(0);
    all_timer_isr_reg();

    timer_intr_enable_disable_test(TIMER_GROUP_0, TIMER_0, 1.2 * TEST_TIMER_RESOLUTION_HZ);
#if SOC_TIMER_GROUPS > 1
    timer_intr_enable_disable_test(TIMER_GROUP_1, TIMER_0, 1.2 * TEST_TIMER_RESOLUTION_HZ);
#endif

    // configure clock source as XTAL clock
    all_timer_pause();
    config.clk_src = TIMER_SRC_CLK_XTAL;
    config.divider = esp_clk_xtal_freq() / TEST_TIMER_RESOLUTION_HZ;
    all_timer_init(&config, true);
    all_timer_set_alarm_value(1.2 * TEST_TIMER_RESOLUTION_HZ);

    timer_intr_enable_disable_test(TIMER_GROUP_0, TIMER_0, 1.2 * TEST_TIMER_RESOLUTION_HZ);
#if SOC_TIMER_GROUPS > 1
    timer_intr_enable_disable_test(TIMER_GROUP_1, TIMER_0, 1.2 * TEST_TIMER_RESOLUTION_HZ);
#endif

    all_timer_isr_unreg();
    all_timer_deinit();
}
#endif

/**
 * Timer ISR callback test
 */
TEST_CASE("Timer_ISR_callback", "[hw_timer]")
{
    alarm_flag = false;
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
    };
    uint64_t alarm_cnt_val = 1.2 * TEST_TIMER_RESOLUTION_HZ;
    uint64_t set_timer_val = 0x0;
    all_timer_init(&config, true);
    all_timer_pause();
    all_timer_set_alarm_value(alarm_cnt_val);
    all_timer_set_counter_value(set_timer_val);

    // add isr callback for tg0_timer0
    TEST_ESP_OK(timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, test_timer_group_isr_cb,
                                       GET_TIMER_INFO(TIMER_GROUP_0, TIMER_0), ESP_INTR_FLAG_LOWMED));
    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, set_timer_val));
    TEST_ESP_OK(timer_start(TIMER_GROUP_0, TIMER_0));
    timer_isr_check(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_DIS, alarm_cnt_val);
    TEST_ASSERT_EQUAL(true, alarm_flag);

    // remove isr callback for tg0_timer0
    TEST_ESP_OK(timer_pause(TIMER_GROUP_0, TIMER_0));
    TEST_ESP_OK(timer_isr_callback_remove(TIMER_GROUP_0, TIMER_0));
    alarm_flag = false;
    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, set_timer_val));
    TEST_ESP_OK(timer_start(TIMER_GROUP_0, TIMER_0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(false, alarm_flag);

#if SOC_TIMER_GROUPS > 1
    // add isr callback for tg1_timer0
    TEST_ESP_OK(timer_pause(TIMER_GROUP_1, TIMER_0));
    TEST_ESP_OK(timer_isr_callback_add(TIMER_GROUP_1, TIMER_0, test_timer_group_isr_cb,
                                       GET_TIMER_INFO(TIMER_GROUP_1, TIMER_0), ESP_INTR_FLAG_LOWMED));
    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_1, TIMER_0, set_timer_val));
    TEST_ESP_OK(timer_start(TIMER_GROUP_1, TIMER_0));
    timer_isr_check(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_DIS, alarm_cnt_val);
    TEST_ASSERT_EQUAL(true, alarm_flag);

    // remove isr callback for tg1_timer0
    TEST_ESP_OK(timer_pause(TIMER_GROUP_1, TIMER_0));
    TEST_ESP_OK(timer_isr_callback_remove(TIMER_GROUP_1, TIMER_0));
    alarm_flag = false;
    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_1, TIMER_0, set_timer_val));
    TEST_ESP_OK(timer_start(TIMER_GROUP_1, TIMER_0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(false, alarm_flag);
#endif
    all_timer_deinit();
}

/**
 * Timer memory test
 */
TEST_CASE("Timer_init_deinit_stress_test", "[hw_timer]")
{
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
    };
    for (uint32_t i = 0; i < 100; i++) {
        all_timer_init(&config, true);
        all_timer_deinit();
    }
}

// The following test cases are used to check if the timer_group fix works.
// Some applications use a software reset, at the reset time, timer_group happens to generate an interrupt.
// but software reset does not clear interrupt status, this is not safe for application when enable the interrupt of timer_group.
// This case will check under this fix, whether the interrupt status is cleared after timer_group initialization.
static void timer_group_test_init(void)
{
    static const uint32_t time_ms = 100;  // Alarm value 100ms.
    static const uint32_t ste_val = time_ms * TEST_TIMER_RESOLUTION_HZ / 1000;
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .intr_type = TIMER_INTR_LEVEL,
        .auto_reload = TIMER_AUTORELOAD_EN,
    };
    TEST_ESP_OK(timer_init(TIMER_GROUP_0, TIMER_0, &config));
    TEST_ESP_OK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL));
    TEST_ESP_OK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, ste_val));
    //Now the timer is ready.
    //We only need to check the interrupt status and don't have to register a interrupt routine.
}

//
// Timer check reinitialization sequence
//
TEST_CASE("Timer_check_reinitialization_sequence", "[hw_timer]")
{
    // 1. step - install driver
    timer_group_test_init();
    // 2 - register interrupt and start timer
    TEST_ESP_OK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    TEST_ESP_OK(timer_start(TIMER_GROUP_0, TIMER_0));
    // Do some work
    vTaskDelay(80 / portTICK_PERIOD_MS);
    // 3 - deinit timer driver
    TEST_ESP_OK(timer_deinit(TIMER_GROUP_0, TIMER_0));
    timer_config_t config = {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / TEST_TIMER_RESOLUTION_HZ,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .alarm_en = TIMER_ALARM_EN,
        .intr_type = TIMER_INTR_LEVEL,
        .auto_reload = TIMER_AUTORELOAD_EN,
    };
    // 4 - reinstall driver
    TEST_ESP_OK(timer_init(TIMER_GROUP_0, TIMER_0, &config));
    // 5 - enable interrupt
    TEST_ESP_OK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    vTaskDelay(30 / portTICK_PERIOD_MS);
    // The pending timer interrupt should not be triggered
    TEST_ASSERT_EQUAL(0, timer_group_get_intr_status_in_isr(TIMER_GROUP_0) & TIMER_INTR_T0);
}
