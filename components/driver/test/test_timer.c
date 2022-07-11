#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "unity.h"
#include "nvs_flash.h"
#include "driver/timer.h"

#define TIMER_DIVIDER 16
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_DELTA 0.001
static bool alarm_flag;

typedef struct {
    timer_group_t timer_group;
    timer_idx_t timer_idx;
} timer_info_t;

#define TIMER_INFO_INIT(TG, TID)    {.timer_group = (TG), .timer_idx = (TID),}

static timer_info_t timer_info[4] = {
    TIMER_INFO_INIT(TIMER_GROUP_0, TIMER_0),
    TIMER_INFO_INIT(TIMER_GROUP_0, TIMER_1),
    TIMER_INFO_INIT(TIMER_GROUP_1, TIMER_0),
    TIMER_INFO_INIT(TIMER_GROUP_1, TIMER_1),
};

#define GET_TIMER_INFO(TG, TID) (&timer_info[(TG)*2+(TID)])

// timer group interruption
static void test_timer_group_isr(void *para)
{
    timer_info_t* info = (timer_info_t*) para;
    const timer_group_t timer_group = info->timer_group;
    const timer_idx_t timer_idx = info->timer_idx;
    uint64_t timer_val;
    double time;
    uint64_t alarm_value;
    alarm_flag = true;
    if (timer_group_get_auto_reload_in_isr(timer_group, timer_idx)) {
        timer_group_clr_intr_status_in_isr(timer_group, timer_idx);
        ets_printf("This is TG%d timer[%d] reload-timer alarm!\n", timer_group, timer_idx);
        timer_get_counter_value(timer_group, timer_idx, &timer_val);
        timer_get_counter_time_sec(timer_group, timer_idx, &time);
        ets_printf("time: %.8f S\n", time);
    } else {
        timer_group_clr_intr_status_in_isr(timer_group, timer_idx);
        ets_printf("This is TG%d timer[%d] count-up-timer alarm!\n", timer_group, timer_idx);
        timer_get_counter_value(timer_group, timer_idx, &timer_val);
        timer_get_counter_time_sec(timer_group, timer_idx, &time);
        timer_get_alarm_value(timer_group, timer_idx, &alarm_value);
        ets_printf("time: %.8f S\n", time);
        double alarm_time = (double) alarm_value / TIMER_SCALE;
        ets_printf("alarm_time: %.8f S\n", alarm_time);
    }
}

// initialize exact timer group
static void tg_timer_init(int timer_group, int timer_idx, double alarm_time)
{
    timer_pause(timer_group, timer_idx);
    timer_set_counter_value(timer_group, timer_idx, 0x0);
    timer_set_alarm_value(timer_group, timer_idx, alarm_time * TIMER_SCALE);
    timer_enable_intr(timer_group, timer_idx);
    timer_isr_register(timer_group, timer_idx, test_timer_group_isr, GET_TIMER_INFO(timer_group, timer_idx), ESP_INTR_FLAG_LOWMED, NULL);
    timer_start(timer_group, timer_idx);
}

// initialize all timer
static void all_timer_init(timer_config_t config, bool flag)
{
    esp_err_t ret;
    ret = timer_init(TIMER_GROUP_0, TIMER_0, &config);
    if (flag) {
        TEST_ASSERT(ret == ESP_OK);
    } else {
        TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    }
    ret = timer_init(TIMER_GROUP_0, TIMER_1, &config);
    if (flag) {
        TEST_ASSERT(ret == ESP_OK);
    } else {
        TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    }
    ret = timer_init(TIMER_GROUP_1, TIMER_0, &config);
    if (flag) {
        TEST_ASSERT(ret == ESP_OK);
    } else {
        TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    }
    ret = timer_init(TIMER_GROUP_1, TIMER_1, &config);
    if (flag) {
        TEST_ASSERT(ret == ESP_OK);
    } else {
        TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    }
}

// start all of timer
static void all_timer_start(void)
{
    esp_err_t ret;
    ret = timer_start(TIMER_GROUP_0, TIMER_0);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_start(TIMER_GROUP_0, TIMER_1);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_start(TIMER_GROUP_1, TIMER_0);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_start(TIMER_GROUP_1, TIMER_1);
    TEST_ASSERT(ret == ESP_OK);
}

static void all_timer_set_counter_value(uint64_t set_timer_val)
{
    esp_err_t ret;
    ret = timer_set_counter_value(TIMER_GROUP_0, TIMER_0, set_timer_val);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_counter_value(TIMER_GROUP_0, TIMER_1, set_timer_val);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_counter_value(TIMER_GROUP_1, TIMER_0, set_timer_val);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_counter_value(TIMER_GROUP_1, TIMER_1, set_timer_val);
    TEST_ASSERT(ret == ESP_OK);
}

static void all_timer_pause(void)
{
    esp_err_t ret;
    ret = timer_pause(TIMER_GROUP_0, TIMER_0);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_pause(TIMER_GROUP_0, TIMER_1);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_pause(TIMER_GROUP_1, TIMER_0);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_pause(TIMER_GROUP_1, TIMER_1);
    TEST_ASSERT(ret == ESP_OK);
}

static void all_timer_get_counter_value(uint64_t set_timer_val, bool flag,
                                        uint64_t *counter_val)
{
    esp_err_t ret;
    uint64_t time_val;
    ret = timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &time_val);
    TEST_ASSERT(ret == ESP_OK);
    if (flag == true) {
        TEST_ASSERT(set_timer_val == time_val);
    } else {
        TEST_ASSERT(set_timer_val != time_val);
        if (counter_val != NULL) {
            counter_val[0] = time_val;
        }
    }
    ret = timer_get_counter_value(TIMER_GROUP_0, TIMER_1, &time_val);
    TEST_ASSERT(ret == ESP_OK);
    if (flag) {
        TEST_ASSERT(set_timer_val == time_val);
    } else {
        TEST_ASSERT(set_timer_val != time_val);
        if (counter_val != NULL) {
            counter_val[1] = time_val;
        }
    }
    ret = timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &time_val);
    TEST_ASSERT(ret == ESP_OK);
    if (flag) {
        TEST_ASSERT(set_timer_val == time_val);
    } else {
        TEST_ASSERT(set_timer_val != time_val);
        if (counter_val != NULL) {
            counter_val[2] = time_val;
        }
    }
    ret = timer_get_counter_value(TIMER_GROUP_1, TIMER_1, &time_val);
    TEST_ASSERT(ret == ESP_OK);
    if (flag) {
        TEST_ASSERT(set_timer_val == time_val);
    } else {
        TEST_ASSERT(set_timer_val != time_val);
        if (counter_val != NULL) {
            counter_val[3] = time_val;
        }
    }
}

static void all_timer_get_counter_time_sec(bool flag, int delay_time)
{
    double time;
    esp_err_t ret;
    ret = timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &time);
    TEST_ASSERT(ret == ESP_OK);
    if (!flag) {
        TEST_ASSERT_FLOAT_WITHIN(TIMER_DELTA, delay_time, time);
    }
    ret = timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_1, &time);
    TEST_ASSERT(ret == ESP_OK);
    if (!flag) {
        TEST_ASSERT_FLOAT_WITHIN(TIMER_DELTA, delay_time, time);
    }
    ret = timer_get_counter_time_sec(TIMER_GROUP_1, TIMER_0, &time);
    TEST_ASSERT(ret == ESP_OK);
    if (!flag) {
        TEST_ASSERT_FLOAT_WITHIN(TIMER_DELTA, delay_time, time);
    }
    ret = timer_get_counter_time_sec(TIMER_GROUP_1, TIMER_1, &time);
    TEST_ASSERT(ret == ESP_OK);
    if (!flag) {
        TEST_ASSERT_FLOAT_WITHIN(TIMER_DELTA, delay_time, time);
    }
}

static void all_timer_set_counter_mode(timer_count_dir_t counter_dir)
{
    esp_err_t ret;
    ret = timer_set_counter_mode(TIMER_GROUP_0, TIMER_0, counter_dir);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_counter_mode(TIMER_GROUP_0, TIMER_1, counter_dir);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_counter_mode(TIMER_GROUP_1, TIMER_0, counter_dir);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_counter_mode(TIMER_GROUP_1, TIMER_1, counter_dir);
    TEST_ASSERT(ret == ESP_OK);
}

static void all_timer_set_divider(uint32_t divider)
{
    esp_err_t ret;
    ret = timer_set_divider(TIMER_GROUP_0, TIMER_0, divider);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_divider(TIMER_GROUP_0, TIMER_1, divider);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_divider(TIMER_GROUP_1, TIMER_0, divider);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_divider(TIMER_GROUP_1, TIMER_1, divider);
    TEST_ASSERT(ret == ESP_OK);
}

static void all_timer_set_alarm_value(double alarm_time)
{
    esp_err_t ret;
    ret = timer_set_alarm_value(TIMER_GROUP_0, TIMER_0,
                                alarm_time * TIMER_SCALE);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_alarm_value(TIMER_GROUP_0, TIMER_1,
                                alarm_time * TIMER_SCALE);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_alarm_value(TIMER_GROUP_1, TIMER_0,
                                alarm_time * TIMER_SCALE);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_set_alarm_value(TIMER_GROUP_1, TIMER_1,
                                alarm_time * TIMER_SCALE);
    TEST_ASSERT(ret == ESP_OK);
}

TEST_CASE("Timer init", "[hw_timer]")
{
    esp_err_t ret;

    // Test init 1:config para
    // empty para
    timer_config_t config0 = { };
    all_timer_init(config0, false);

    // only one para
    timer_config_t config1 = {
        .auto_reload = 1
    };
    all_timer_init(config1, false);

    // lack one para
    timer_config_t config2 = {
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(config2, true);

    config2.counter_en = 0;
    all_timer_init(config2, true);


    // error config para
    timer_config_t config3 = {
        .alarm_en = 3,     //error para
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(config3, true);
    timer_config_t get_config;
    timer_get_config(TIMER_GROUP_1, TIMER_1, &get_config);
    printf("Error config alarm_en is %d\n", get_config.alarm_en);
    TEST_ASSERT(config3.alarm_en != get_config.alarm_en);

    // Test init 2:  init
    uint64_t set_timer_val = 0x0;
    timer_config_t config = {
        .alarm_en = 0,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };

    // judge get config parameters
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_get_config(TIMER_GROUP_0, TIMER_0, &get_config);
    TEST_ASSERT(config.alarm_en == get_config.alarm_en);
    TEST_ASSERT(config.auto_reload == get_config.auto_reload);
    TEST_ASSERT(config.counter_dir == get_config.counter_dir);
    TEST_ASSERT(config.counter_en == get_config.counter_en);
    TEST_ASSERT(config.intr_type == get_config.intr_type);
    TEST_ASSERT(config.divider == get_config.divider);

    all_timer_init(config, true);
    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    all_timer_get_counter_value(set_timer_val, false, NULL);

    // Test init 3:  wrong para
    ret = timer_init(-1, TIMER_1, &config);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_init(TIMER_GROUP_1, 2, &config);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_init(TIMER_GROUP_1, -1, &config);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_init(2, TIMER_1, &config);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);

}

/**
 * read count case:
 * 1. start timer compare value
 * 2. pause timer compare value
 * 3. delay some time */
TEST_CASE("Timer read counter value", "[hw_timer]")
{
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;

    all_timer_init(config, true);

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
    all_timer_get_counter_time_sec(true, 1);
}

/**
 * start timer case:
 * 1. normal start
 * 2. error start para
 * */
TEST_CASE("Timer start", "[hw_timer]")
{
    esp_err_t ret;
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(config, true);

    //Test start 1: normal start
    all_timer_start();
    all_timer_set_counter_value(set_timer_val);
    all_timer_get_counter_value(set_timer_val, false, NULL);

    //Test start 2:wrong para
    ret = timer_start(2, TIMER_1);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_start(-1, TIMER_1);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_start(TIMER_GROUP_1, 2);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_start(TIMER_GROUP_1, -1);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
}

/**
 * pause timer case:
 * 1. normal pause, read value
 * 2. error pause error
 */
TEST_CASE("Timer pause", "[hw_timer]")
{
    esp_err_t ret;
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(config, true);

    //Test pause 1: right para
    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);
    all_timer_get_counter_value(set_timer_val, true, NULL);

    //Test pause 2: wrong para
    ret = timer_pause(-1, TIMER_0);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_pause(TIMER_GROUP_0, -1);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_pause(2, TIMER_0);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_pause(TIMER_GROUP_1, 2);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
}

// positive mode and negative mode
TEST_CASE("Timer counter mode (up / down)", "[hw_timer]")
{
    esp_err_t ret;
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(config, true);
    all_timer_pause();

    // Test counter mode 1: TIMER_COUNT_UP
    all_timer_set_counter_mode(TIMER_COUNT_UP);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_time_sec(true, 1);

    // Test counter mode 2: TIMER_COUNT_DOWN
    all_timer_pause();
    set_timer_val = 0x00E4E1C0ULL;          // 3s clock counter value
    all_timer_set_counter_mode(TIMER_COUNT_DOWN);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_time_sec(true, 2);

    // Test counter mode 3 : wrong para
    ret = timer_set_counter_mode(TIMER_GROUP_0, TIMER_0, -1);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
    ret = timer_set_counter_mode(TIMER_GROUP_0, TIMER_0, 2);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG);
}

/**
 * divider case:
 * 1. different divider, read value
 *       Note: divide 0 = divide max, divide 1 = divide 2
 * 2. error para
 *
 * the frequency(timer counts in one sec):
 *  80M/divider = 800*100000
 *  max divider value is 65536, its frequency is 1220 (nearly about 1KHz)
 */
TEST_CASE("Timer divider", "[hw_timer]")
{
    int i;
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0;
    uint64_t time_val[4];
    uint64_t comp_time_val[4];
    all_timer_init(config, true);

    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);

    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_value(set_timer_val, false, time_val);

    // compare divider  16 and 8, value should be double
    all_timer_pause();
    all_timer_set_divider(8);
    all_timer_set_counter_value(set_timer_val);

    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);         //delay the same time
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (i = 0; i < 4; i++) {
        TEST_ASSERT_INT_WITHIN(5000, 5000000, time_val[i]);
        TEST_ASSERT_INT_WITHIN(10000, 10000000, comp_time_val[i]);
    }

    // divider is 256, value should be 2^4
    all_timer_pause();
    all_timer_set_divider(256);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);         //delay the same time
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (i = 0; i < 4; i++) {
        TEST_ASSERT_INT_WITHIN(5000, 5000000, time_val[i]);
        TEST_ASSERT_INT_WITHIN(3126, 312500, comp_time_val[i]);
    }

    // extrem value test
    all_timer_pause();
    all_timer_set_divider(2);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (i = 0; i < 4; i++) {
        TEST_ASSERT_INT_WITHIN(5000, 5000000, time_val[i]);
        TEST_ASSERT_INT_WITHIN(40000 , 40000000, comp_time_val[i]);
    }

    all_timer_pause();
    all_timer_set_divider(65536);
    all_timer_set_counter_value(set_timer_val);
    all_timer_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);         //delay the same time
    all_timer_get_counter_value(set_timer_val, false, comp_time_val);
    for (i = 0; i < 4; i++) {
        TEST_ASSERT_INT_WITHIN(5000, 5000000, time_val[i]);
        TEST_ASSERT_INT_WITHIN(2 , 1220, comp_time_val[i]);
    }

    // divider is 1 should be equal with 2
    all_timer_pause();
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_0, TIMER_0, 1) == ESP_ERR_INVALID_ARG) ;
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_1, TIMER_0, 1) == ESP_ERR_INVALID_ARG) ;
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_0, TIMER_1, 1) == ESP_ERR_INVALID_ARG) ;
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_1, TIMER_1, 1) == ESP_ERR_INVALID_ARG) ;

    all_timer_pause();
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_0, TIMER_0, 65537) == ESP_ERR_INVALID_ARG) ;
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_1, TIMER_0, 65537) == ESP_ERR_INVALID_ARG) ;
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_0, TIMER_1, 65537) == ESP_ERR_INVALID_ARG) ;
    TEST_ASSERT(timer_set_divider(TIMER_GROUP_1, TIMER_1, 65537) == ESP_ERR_INVALID_ARG) ;
}

/**
 * enable alarm case:
 * 1. enable alarm ,set alarm value and get value
 * 2. disable alarm ,set alarm value and get value
 */
TEST_CASE("Timer enable alarm", "[hw_timer]")
{
    timer_config_t config_test = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(config_test, true);

    // enable alarm
    alarm_flag = false;
    tg_timer_init(TIMER_GROUP_0, TIMER_1, 1.2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == true);

    // disable alarm
    alarm_flag = false;
    timer_set_alarm(TIMER_GROUP_0, TIMER_1, TIMER_ALARM_DIS);
    tg_timer_init(TIMER_GROUP_0, TIMER_1, 1.2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == false);

    // enable alarm
    alarm_flag = false;
    timer_set_alarm(TIMER_GROUP_1, TIMER_0, TIMER_ALARM_EN);
    tg_timer_init(TIMER_GROUP_1, TIMER_0, 1.2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == true);

    // disable alarm
    alarm_flag = false;
    timer_set_alarm(TIMER_GROUP_1, TIMER_0, TIMER_ALARM_DIS);
    tg_timer_init(TIMER_GROUP_1, TIMER_0, 1.2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == false);
}

/**
 * alarm value case:
 * 1. set alarm value and get value
 * 2. interrupt test time
 */
TEST_CASE("Timer set alarm value", "[hw_timer]")
{
    esp_err_t ret;
    int i;
    uint64_t alarm_val[4];
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 0,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(config, true);

    // set and get alarm value
    all_timer_set_alarm_value(3);
    ret = timer_get_alarm_value(TIMER_GROUP_0, TIMER_0, &alarm_val[0]);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_get_alarm_value(TIMER_GROUP_0, TIMER_1, &alarm_val[1]);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_get_alarm_value(TIMER_GROUP_1, TIMER_0, &alarm_val[2]);
    TEST_ASSERT(ret == ESP_OK);
    ret = timer_get_alarm_value(TIMER_GROUP_1, TIMER_1, &alarm_val[3]);
    TEST_ASSERT(ret == ESP_OK);
    for (i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_UINT32(alarm_val[i] , TIMER_SCALE * 3);
    }

    // set interrupt read alarm value
    tg_timer_init(TIMER_GROUP_0, TIMER_1, 2.4);
    tg_timer_init(TIMER_GROUP_1, TIMER_0, 1.4);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}

/**
 * auto reload case:
 * 1. no reload
 * 2. auto reload
 */
TEST_CASE("Timer auto reload", "[hw_timer]")
{
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 1,
        .intr_type = TIMER_INTR_LEVEL
    };
    all_timer_init(config, true);

    // test disable auto_reload
    tg_timer_init(TIMER_GROUP_0, TIMER_0, 1.14);
    tg_timer_init(TIMER_GROUP_1, TIMER_1, 1.14);
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    //test enable auto_reload
    timer_set_auto_reload(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_EN);
    tg_timer_init(TIMER_GROUP_0, TIMER_1, 1.4);
    timer_set_auto_reload(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_EN);
    tg_timer_init(TIMER_GROUP_1, TIMER_0, 1.4);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

/**
 * timer_enable_intr case:
 * 1. enable timer_intr
 * 2. disable timer_intr
 */
TEST_CASE("Timer enable timer interrupt", "[hw_timer]")
{
    alarm_flag = false;
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(config, true);
    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);
    all_timer_set_alarm_value(1.2);

    // enable  timer_intr0
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, set_timer_val);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1.2 * TIMER_SCALE);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, test_timer_group_isr,
                       GET_TIMER_INFO(TIMER_GROUP_0, TIMER_0), ESP_INTR_FLAG_LOWMED, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == true)

    // disable timer_intr0
    alarm_flag = false;
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, set_timer_val);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1.2 * TIMER_SCALE);
    timer_disable_intr(TIMER_GROUP_0, TIMER_0);
    timer_start(TIMER_GROUP_0, TIMER_0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == false)

    // enable  timer_intr1
    timer_set_counter_value(TIMER_GROUP_1, TIMER_1, set_timer_val);
    timer_set_alarm_value(TIMER_GROUP_1, TIMER_1, 1.2 * TIMER_SCALE);
    timer_isr_register(TIMER_GROUP_1, TIMER_1, test_timer_group_isr,
                       GET_TIMER_INFO(TIMER_GROUP_1, TIMER_1), ESP_INTR_FLAG_LOWMED, NULL);
    timer_start(TIMER_GROUP_1, TIMER_1);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == true)

    // disable timer_intr1
    alarm_flag = false;
    timer_set_counter_value(TIMER_GROUP_1, TIMER_1, set_timer_val);
    timer_set_alarm_value(TIMER_GROUP_1, TIMER_1, 1.2 * TIMER_SCALE);
    timer_disable_intr(TIMER_GROUP_1, TIMER_1);
    timer_start(TIMER_GROUP_1, TIMER_1);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == false);

    //enable timer_intr1 again
    timer_init(TIMER_GROUP_1, TIMER_1, &config);
    timer_set_counter_value(TIMER_GROUP_1, TIMER_1, set_timer_val);
    timer_set_alarm_value(TIMER_GROUP_1, TIMER_1, 1.2 * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_1, TIMER_1);
    timer_start(TIMER_GROUP_1, TIMER_1);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == true)
}

/**
 * enable timer group case:
 * 1. enable timer group
 * 2. disable timer group
 */
TEST_CASE("Timer enable timer group interrupt", "[hw_timer][ignore]")
{
    alarm_flag = false;
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = 0,
        .intr_type = TIMER_INTR_LEVEL
    };
    uint64_t set_timer_val = 0x0;
    all_timer_init(config, true);
    all_timer_pause();
    all_timer_set_counter_value(set_timer_val);
    all_timer_set_alarm_value(1.2);

    // enable timer group
    timer_group_intr_enable(TIMER_GROUP_0, TIMER_INTR_T0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, test_timer_group_isr, GET_TIMER_INFO(TIMER_GROUP_0, TIMER_0), ESP_INTR_FLAG_LOWMED, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == true);

    //test enable auto_reload
    alarm_flag = false;
    timer_group_intr_disable(TIMER_GROUP_0, TIMER_INTR_T0);
    timer_start(TIMER_GROUP_0, TIMER_0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == false);

    timer_group_intr_enable(TIMER_GROUP_0, TIMER_INTR_T0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, test_timer_group_isr, GET_TIMER_INFO(TIMER_GROUP_0, TIMER_0), ESP_INTR_FLAG_LOWMED, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ASSERT(alarm_flag == true);
}

/**
 * isr_register case:
 * Cycle register 15 times, compare the heap size to ensure no memory leaks
 */
TEST_CASE("Timer interrupt register", "[hw_timer]")
{
    int i;
    int heap_size = 0;
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL
    };

    for (i = 0; i < 15; i++) {
        all_timer_init(config, true);
        tg_timer_init(TIMER_GROUP_0, TIMER_0, 0.54);
        tg_timer_init(TIMER_GROUP_1, TIMER_1, 0.34);

        timer_set_auto_reload(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_EN);
        tg_timer_init(TIMER_GROUP_0, TIMER_1, 0.4);
        timer_set_auto_reload(TIMER_GROUP_1, TIMER_0, TIMER_AUTORELOAD_EN);
        tg_timer_init(TIMER_GROUP_1, TIMER_0, 0.6);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (heap_size == 0) {
            heap_size = esp_get_free_heap_size();
        }
    }
    TEST_ASSERT_INT_WITHIN(100, heap_size, esp_get_free_heap_size());
}

// The following test cases are used to check if the timer_group fix works.
// Some applications use a software reset, at the reset time, timer_group happens to generate an interrupt.
// but software reset does not clear interrupt status, this is not safe for application when enable the interrupt of timer_group.
// This case will check under this fix, whether the interrupt status is cleared after timer_group initialization.
static void timer_group_test_init(void)
{
    static const uint32_t time_ms = 100;  // Alarm value 100ms.
    static const uint16_t timer_div = TIMER_DIVIDER; // Timer prescaler
    static const uint32_t ste_val = time_ms * (TIMER_BASE_CLK / timer_div / 1000);
    timer_config_t config = {
        .divider = timer_div,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .intr_type = TIMER_INTR_LEVEL,
        .auto_reload = true,
    };
    ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &config));
    ESP_ERROR_CHECK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL));
    ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, ste_val));
    //Now the timer is ready.
    //We only need to check the interrupt status and don't have to register a interrupt routine.
}

static void timer_group_test_first_stage(void)
{
    static uint8_t loop_cnt = 0;
    timer_group_test_init();
    //Start timer
    ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    ESP_ERROR_CHECK(timer_start(TIMER_GROUP_0, TIMER_0));
    //Waiting for timer_group to generate an interrupt
    while( !(timer_group_get_intr_status_in_isr(TIMER_GROUP_0) & TIMER_INTR_T0) &&
            loop_cnt++ < 100) {
        vTaskDelay(200);
    }
    //TIMERG0.int_raw.t0 == 1 means an interruption has occurred
    TEST_ASSERT(timer_group_get_intr_status_in_isr(TIMER_GROUP_0) & TIMER_INTR_T0);
    esp_restart();
}

static void timer_group_test_second_stage(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
    timer_group_test_init();
    TEST_ASSERT_EQUAL(0, timer_group_get_intr_status_in_isr(TIMER_GROUP_0) & TIMER_INTR_T0);
    // After enable the interrupt, timer alarm should not trigger immediately
    TEST_ESP_OK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    //After the timer_group is initialized, TIMERG0.int_raw.t0 should be cleared.
    TEST_ASSERT_EQUAL(0, timer_group_get_intr_status_in_isr(TIMER_GROUP_0) & TIMER_INTR_T0);
}

TEST_CASE_MULTIPLE_STAGES("timer_group software reset test",

                          "[intr_status][intr_status = 0]",
                          timer_group_test_first_stage,
                          timer_group_test_second_stage);
//
// Timer check reinitialization sequence
//
TEST_CASE("Timer check reinitialization sequence", "[hw_timer]")
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
        .divider = TIMER_DIVIDER,
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
