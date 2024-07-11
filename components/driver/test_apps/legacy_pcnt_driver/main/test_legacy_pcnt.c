/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#if SOC_LEDC_SUPPORTED
#include "driver/ledc.h"
#include "soc/ledc_periph.h"
#endif
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_periph.h"
#include "soc/pcnt_struct.h"
#include "soc/pcnt_periph.h"
#include "unity.h"
#include "esp_rom_gpio.h"

#if CONFIG_IDF_TARGET_ESP32P4
#define PULSE_IO 20
#define PCNT_INPUT_IO 21
#define PCNT_CTRL_VCC_IO 1
#define PCNT_CTRL_GND_IO 0
#else
#define PULSE_IO 0
#define PCNT_INPUT_IO 4
#define PCNT_CTRL_VCC_IO 5
#define PCNT_CTRL_GND_IO 2
#endif

#define PCNT_CTRL_HIGH_LEVEL 1
#define PCNT_CTRL_LOW_LEVEL 0

// test PCNT basic configuration
TEST_CASE("PCNT_test_config", "[pcnt]")
{
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = PCNT_INPUT_IO,
        .ctrl_gpio_num = PCNT_CTRL_VCC_IO,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 100,
        .counter_l_lim = 0,
    };
    // basic configuration
    pcnt_config_t temp_pcnt_config = pcnt_config;
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));

    // test SOC_PCNT_UNITS_PER_GROUP units, from 0-(SOC_PCNT_UNITS_PER_GROUP-1)
    pcnt_config = temp_pcnt_config;
    pcnt_config.unit = SOC_PCNT_UNITS_PER_GROUP;
    TEST_ASSERT_NOT_NULL((void *)pcnt_unit_config(&pcnt_config));
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        pcnt_config.unit = i;
        TEST_ESP_OK(pcnt_unit_config(&pcnt_config));
    }

    // test channels
    pcnt_config = temp_pcnt_config;
    pcnt_config.channel = PCNT_CHANNEL_MAX;
    TEST_ASSERT_NOT_NULL((void *)pcnt_unit_config(&pcnt_config));
    pcnt_config = temp_pcnt_config;
    pcnt_config.pulse_gpio_num = -1;
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));

    pcnt_config = temp_pcnt_config;
    pcnt_config.pulse_gpio_num = GPIO_NUM_MAX + 1;
    TEST_ASSERT_NOT_NULL((void *)pcnt_unit_config(&pcnt_config));

    // test pulse_gpio_num and ctrl_gpio_num is the same
    pcnt_config = temp_pcnt_config;
    pcnt_config.pulse_gpio_num = PCNT_INPUT_IO;
    pcnt_config.ctrl_gpio_num = PCNT_INPUT_IO;
    TEST_ASSERT_NOT_NULL((void *)pcnt_unit_config(&pcnt_config));

    pcnt_config = temp_pcnt_config;
    pcnt_config.pos_mode = PCNT_COUNT_MAX;
    TEST_ASSERT_NOT_NULL((void *)pcnt_unit_config(&pcnt_config));

    pcnt_config = temp_pcnt_config;
    pcnt_config.hctrl_mode = PCNT_MODE_MAX;
    TEST_ASSERT_NOT_NULL((void *)pcnt_unit_config(&pcnt_config));

    pcnt_config = temp_pcnt_config;
    pcnt_config.lctrl_mode = PCNT_MODE_MAX;
    TEST_ASSERT_NOT_NULL((void *)pcnt_unit_config(&pcnt_config));
}

// The following test cases rely on the support of LEDC
#if SOC_LEDC_SUPPORTED
// The following items only used in the cases that involve LEDC
static QueueHandle_t pcnt_evt_queue = NULL;

typedef struct {
    int zero_times;
    int h_limit;
    int l_limit;
    int h_threshold;
    int l_threshold;
    int filter_time;
} event_times;

static void pcnt_test_io_config(int ctrl_level)
{
    // Connect internal signals using IO matrix.
    gpio_set_direction(PULSE_IO, GPIO_MODE_INPUT_OUTPUT);
    esp_rom_gpio_connect_out_signal(PULSE_IO, ledc_periph_signal[LEDC_LOW_SPEED_MODE].sig_out0_idx + 1, 0, 0);  // LEDC_CHANNEL_1, LEDC_LOW_SPEED_MODE
    esp_rom_gpio_connect_in_signal(PULSE_IO, pcnt_periph_signals.groups[0].units[0].channels[0].pulse_sig, 0);      // PCNT_UNIT_0, PCNT_CHANNEL_0
    esp_rom_gpio_connect_in_signal(ctrl_level ? GPIO_MATRIX_CONST_ONE_INPUT : GPIO_MATRIX_CONST_ZERO_INPUT, pcnt_periph_signals.groups[0].units[0].channels[0].control_sig, 0); // PCNT_UNIT_0, PCNT_CHANNEL_0
}

/* use LEDC to produce pulse for PCNT
 * the frequency of LEDC is 1000, so every second will get 1000 count values
 * the PCNT count the LEDC pulse
 * */
static void produce_pulse(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num  = LEDC_TIMER_1,
        .duty_resolution = LEDC_TIMER_14_BIT,
        .freq_hz = 2,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_1,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PULSE_IO,
        .duty = 100,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void IRAM_ATTR pcnt_intr_handler(void *arg)
{
    uint32_t intr_status = PCNT.int_st.val;
    int i;
    uint32_t status;
    BaseType_t port_status = pdFALSE;

    for (i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        if (intr_status & (BIT(i))) {
            status = PCNT.status_unit[i].val;
            PCNT.int_clr.val = BIT(i);
            xQueueSendFromISR(pcnt_evt_queue, &status, &port_status);
            if (port_status == pdTRUE) {
                portYIELD_FROM_ISR();
            }
        }
    }
}

static void event_calculate(event_times *event)
{
    int16_t test_counter = 0;
    int times = 0;
    BaseType_t port_status;
    uint32_t status = 0;
    while (times < 10) {
        port_status = xQueueReceive(pcnt_evt_queue, &status, 1001 / portTICK_PERIOD_MS);
        if (port_status == pdTRUE) {
            event->filter_time++;
            TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
            printf("Current counter value :%d\n", test_counter);
            if (status & PCNT_EVT_THRES_1) {
                printf("THRES1 EVT\n");
                event->h_threshold++;
            }
            if (status & PCNT_EVT_THRES_0) {
                printf("THRES0 EVT\n");
                event->l_threshold++;
            }
            if (status & PCNT_EVT_L_LIM) {
                printf("L_LIM EVT\n");
                event->l_limit++;
            }
            if (status & PCNT_EVT_H_LIM) {
                printf("H_LIM EVT\n");
                event->h_limit++;
            }
            if (status & PCNT_EVT_ZERO) {
                printf("ZERO EVT\n");
                event->zero_times++;
            }
        } else {
            TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
            printf("Current counter value :%d\n", test_counter);
        }
        times++;
    }
    printf("%d, %d, %d, %d, %d, %d\n", event->h_threshold, event->l_threshold,
           event->l_limit, event->h_limit, event->zero_times, event->filter_time);
}

/*
 * There exist 2 kind of counting methods: positive and negative counting method
 * 1. when control IO is high level, PCNT is positive counting mode
 * 2. when control IO is low level, PCNT is positive negative mode
 * the positive method standard is as below:
 *   ----------------------------------------------------------------------------------
 *   POS_ MODE | LCTRL_ MODE | HCTRL_ MODE | sig l→h when ctrl=0 | sig l→h when ctrl=1
 *   NEG_ MODE |             |             |                     |
 *   ===================================================================================
 *   1 (inc)   | 0 (-)       | 0 (-)       | Inc ctr             | Inc ctr
 *   2 (dec)   | 0 (-)       | 0 (-)       | Dec ctr             | Dec ctr
 *   0 (-)     | x           | x           | No action           | No action
 *   1 (inc)   | 0 (-)       | 1 (inv)     | Inc ctr             | Dec ctr
 *   1 (inc)   | 1 (inv)     | 0 (-)       | Dec ctr             | Inc ctr
 *   2 (dec)   | 0 (-)       | 1 (inv)     | Dec ctr             | Inc ctr
 *   1 (inc)   | 0 (-)       | 2 (dis)     | Inc ctr             | No action
 *   1 (inc)   | 2 (dis)     | 0 (-)       | No action           | Inc ctr
 *  -----------------------------------------------------------------------------------
 * */

static void count_mode_test(gpio_num_t ctl_io)
{
    int16_t test_counter;
    //produce pulse, 100HZ
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num  = LEDC_TIMER_1,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 100,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_1,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PULSE_IO,
        .duty = 100,
        .hpoint = 0,
    };
    ledc_channel_config(&ledc_channel);

    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = PCNT_INPUT_IO,
        .ctrl_gpio_num = ctl_io,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 101,
        .counter_l_lim = -101,
    };
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));
    pcnt_test_io_config((ctl_io == PCNT_CTRL_VCC_IO) ? PCNT_CTRL_HIGH_LEVEL : PCNT_CTRL_LOW_LEVEL);
    int16_t result1[8] = {100, -100, 0, -100, 100, 100, 0, 100};
    int16_t result2[8] = {100, -100, 0, 100, -100, -100, 100, 0};
    int16_t *result;
    if (ctl_io == PCNT_CTRL_VCC_IO) {
        result = result1;
    } else {
        result = result2;
    }
    // Wait for ledc and pcnt settling down
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // 1, 0, 0, 0
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_INC, PCNT_COUNT_DIS,
                              PCNT_MODE_KEEP, PCNT_MODE_KEEP));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[0]);

    //2, 0, 0, 0
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_DEC, PCNT_COUNT_DIS,
                              PCNT_MODE_KEEP, PCNT_MODE_KEEP));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[1]);

    //0,0,0,0
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_DIS, PCNT_COUNT_DIS,
                              PCNT_MODE_KEEP, PCNT_MODE_KEEP));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[2]);

    //1,0,1,0
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_INC, PCNT_COUNT_DIS,
                              PCNT_MODE_REVERSE, PCNT_MODE_KEEP));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[3]);

    //1,0,0,1
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_INC, PCNT_COUNT_DIS,
                              PCNT_MODE_KEEP, PCNT_MODE_REVERSE));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[4]);

    //2,0,0,1
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_DEC, PCNT_COUNT_DIS,
                              PCNT_MODE_REVERSE, PCNT_MODE_KEEP));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[5]);

    //1,0,2,0
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_INC, PCNT_COUNT_DIS,
                              PCNT_MODE_DISABLE, PCNT_MODE_KEEP));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[6]);

    //1,0,0,2
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_mode(PCNT_UNIT_0, PCNT_CHANNEL_0,
                              PCNT_COUNT_INC, PCNT_COUNT_DIS,
                              PCNT_MODE_KEEP, PCNT_MODE_DISABLE));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("value: %d\n", test_counter);
    TEST_ASSERT_INT16_WITHIN(1, test_counter, result[7]);
}

/* PCNT basic property:
 * 1. pause counter
 * 2. resume counter
 * 3. clear counter
 * 4. check the counter value*/
TEST_CASE("PCNT_basic_function_test", "[pcnt]")
{
    int16_t test_counter;
    int16_t time = 0;
    int clear_count = 0;
    int resume_count = 0;
    int temp_value = 0;
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = PCNT_INPUT_IO,
        .ctrl_gpio_num = PCNT_CTRL_VCC_IO,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 10,
        .counter_l_lim = -10,
    };
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));

    // use LEDC to produce the pulse, then the PCNT to count it
    produce_pulse();
    vTaskDelay(10 / portTICK_PERIOD_MS);  // ensure LEDC is working
    pcnt_test_io_config(PCNT_CTRL_HIGH_LEVEL);

    // initialize first, the initial value should be 0
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    TEST_ASSERT_EQUAL_INT16(test_counter, 0);

    // resume the PCNT
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    TEST_ASSERT_EQUAL_INT16(test_counter, 0);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    TEST_ASSERT_EQUAL_INT16(test_counter, 0);

    //count now
    while (time != 10) {
        vTaskDelay(501 / portTICK_PERIOD_MS);  // in case of can't wait to get counter(edge effect)
        TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
        printf("COUNT: %d\n", test_counter);
        TEST_ASSERT_NOT_EQUAL(test_counter, temp_value);
        temp_value = test_counter;

        if (test_counter == 5 || test_counter == -5) {
            //test clear
            TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
            TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
            TEST_ASSERT_EQUAL_INT16(test_counter, 0);
            clear_count++;
        }
        if (test_counter == 0) {
            //test pause
            TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
            vTaskDelay(500 / portTICK_PERIOD_MS);
            TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
            printf("PAUSE: %d\n", test_counter);
            TEST_ASSERT_EQUAL_INT16(test_counter, 0);

            // test resume
            TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
            vTaskDelay(500 / portTICK_PERIOD_MS);
            TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
            printf("RESUME: %d\n", test_counter);
            TEST_ASSERT_EQUAL_INT16(test_counter, 1);
            resume_count++;
        }
        time++;
    }
    TEST_ASSERT_EQUAL_INT16(clear_count, 2);
    TEST_ASSERT_EQUAL_INT16(resume_count, 2);
}

/**
 * there are 4 situations will be tested:
 * 1. set event to interrupt triggered
 * 2. disable interrupt to stop triggering interrupt
 * 3. enable interrupt to interrupt triggered again
 * 4. disable event to stop triggering interrupt
 *
 * PCNT interrupt type:
 *   1. PCNT_EVT_THRES_1
 *   2. PCNT_EVT_THRES_0
 *   3. PCNT_EVT_ZERO
 *   4. PCNT_EVT_H_LIM
 *   5. PCNT_EVT_L_LIM
 * */
TEST_CASE("PCNT_interrupt_method_test_control_IO_high", "[pcnt][timeout=120]")
{
    pcnt_config_t config = {
        .pulse_gpio_num = PCNT_INPUT_IO,
        .ctrl_gpio_num = PCNT_CTRL_VCC_IO,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 10,
        .counter_l_lim = 0,
    };
    TEST_ESP_OK(pcnt_unit_config(&config));
    produce_pulse();
    pcnt_test_io_config(PCNT_CTRL_HIGH_LEVEL);

    event_times event = {
        .zero_times = 0,
        .h_limit = 0,
        .l_limit = 0,
        .h_threshold = 0,
        .l_threshold = 0,
        .filter_time = 0,
    };

    //interrupt set
    TEST_ESP_OK(pcnt_set_filter_value(PCNT_UNIT_0, 2));
    TEST_ESP_OK(pcnt_filter_enable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_event_value(PCNT_UNIT_0, PCNT_EVT_THRES_1, 8));  // when arrive to max threshold trigger
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_THRES_1));
    TEST_ESP_OK(pcnt_set_event_value(PCNT_UNIT_0, PCNT_EVT_THRES_0, 2));  // when arrive to minimum threshold trigger
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_THRES_0));
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_ZERO));
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_H_LIM));  // when arrive to max limit trigger
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_L_LIM));  // when arrive to minimum limit trigger

    // initialize first, the initial value should be 0
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));

    pcnt_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    pcnt_isr_handle_t pcnt_isr_service;
    TEST_ESP_OK(pcnt_isr_register(pcnt_intr_handler, NULL, 0, &pcnt_isr_service));
    TEST_ESP_OK(pcnt_intr_enable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));

    // test event
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 2);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 2);
    TEST_ASSERT(event.l_limit == 0);
    TEST_ASSERT_INT_WITHIN(2, event.h_limit, 2);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 2);
    TEST_ASSERT_INT_WITHIN(3, event.filter_time, 4);

    // test interrupt disable
    TEST_ESP_OK(pcnt_intr_disable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    // for the original control io disable interrupt status
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 2);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 2);
    TEST_ASSERT(event.l_limit == 0);
    TEST_ASSERT_INT_WITHIN(2, event.h_limit, 2);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 2);
    TEST_ASSERT_INT_WITHIN(3, event.filter_time, 4);

    // enable the intr
    TEST_ESP_OK(pcnt_intr_enable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 4);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 4);
    TEST_ASSERT(event.l_limit == 0);
    TEST_ASSERT_INT_WITHIN(2, event.h_limit, 4);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 4);
    TEST_ASSERT_INT_WITHIN(3, event.filter_time, 10);

    // disable part of events
    TEST_ESP_OK(pcnt_event_disable(PCNT_UNIT_0, PCNT_EVT_ZERO));
    TEST_ESP_OK(pcnt_event_disable(PCNT_UNIT_0, PCNT_EVT_L_LIM));
    TEST_ESP_OK(pcnt_event_disable(PCNT_UNIT_0, PCNT_EVT_THRES_0));
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 5);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 4);
    TEST_ASSERT(event.l_limit == 0);
    TEST_ASSERT_INT_WITHIN(3, event.h_limit, 6);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 4);
    TEST_ASSERT_INT_WITHIN(3, event.filter_time, 14);

    // Because this test uses its own ISR, we need to release it with `pcnt_isr_unregister` instead of `pcnt_isr_service_uninstall`
    TEST_ESP_OK(pcnt_isr_unregister(pcnt_isr_service));
    vQueueDelete(pcnt_evt_queue);
}

TEST_CASE("PCNT_interrupt_method_test_control_IO_low", "[pcnt][timeout=120]")
{
    pcnt_config_t config = {
        .pulse_gpio_num = PCNT_INPUT_IO,
        .ctrl_gpio_num = PCNT_CTRL_GND_IO,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 0,
        .counter_l_lim = -10,
    };
    TEST_ESP_OK(pcnt_unit_config(&config));
    produce_pulse();
    pcnt_test_io_config(PCNT_CTRL_LOW_LEVEL);

    event_times event = {
        .zero_times = 0,
        .h_limit = 0,
        .l_limit = 0,
        .h_threshold = 0,
        .l_threshold = 0,
        .filter_time = 0,
    };

    //interrupt set
    TEST_ESP_OK(pcnt_set_filter_value(PCNT_UNIT_0, 2));
    TEST_ESP_OK(pcnt_filter_enable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_set_event_value(PCNT_UNIT_0, PCNT_EVT_THRES_1, -8));  // when arrive to max threshold trigger
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_THRES_1));
    TEST_ESP_OK(pcnt_set_event_value(PCNT_UNIT_0, PCNT_EVT_THRES_0, 0));  // when arrive to minimum threshold trigger
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_THRES_0));
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_ZERO));
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_H_LIM));  // when arrive to max limit trigger
    TEST_ESP_OK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_L_LIM));  // when arrive to minimum limit trigger

    // to initialize for PCNT
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));

    pcnt_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    pcnt_isr_handle_t pcnt_isr_service;
    TEST_ESP_OK(pcnt_isr_register(pcnt_intr_handler, NULL, 0, &pcnt_isr_service));
    TEST_ESP_OK(pcnt_intr_enable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));

    // test event
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 1);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 1);
    TEST_ASSERT_INT_WITHIN(2, event.l_limit, 1);
    TEST_ASSERT_INT_WITHIN(2, event.h_limit, 0);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 1);
    TEST_ASSERT_INT_WITHIN(2, event.filter_time, 2);

    // test interrupt disable
    TEST_ESP_OK(pcnt_intr_disable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    // for the original control io disable interrupt status
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 1);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 1);
    TEST_ASSERT_INT_WITHIN(2, event.l_limit, 1);
    TEST_ASSERT_INT_WITHIN(2, event.h_limit, 0);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 1);
    TEST_ASSERT_INT_WITHIN(2, event.filter_time, 2);

    // enable the intr
    pcnt_unit_config(&config);
    pcnt_test_io_config(PCNT_CTRL_LOW_LEVEL);
    TEST_ESP_OK(pcnt_intr_enable(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 2);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 3);
    TEST_ASSERT_INT_WITHIN(2, event.l_limit, 2);
    TEST_ASSERT_INT_WITHIN(2, event.h_limit, 0);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 2);
    TEST_ASSERT_INT_WITHIN(2, event.filter_time, 6);

    // disable part of events
    TEST_ESP_OK(pcnt_event_disable(PCNT_UNIT_0, PCNT_EVT_ZERO));
    TEST_ESP_OK(pcnt_event_disable(PCNT_UNIT_0, PCNT_EVT_L_LIM));
    TEST_ESP_OK(pcnt_event_disable(PCNT_UNIT_0, PCNT_EVT_THRES_0));
    event_calculate(&event);
    TEST_ASSERT_INT_WITHIN(2, event.h_threshold, 4);
    TEST_ASSERT_INT_WITHIN(2, event.l_threshold, 3);
    TEST_ASSERT_INT_WITHIN(2, event.l_limit, 2);
    TEST_ASSERT_INT_WITHIN(2, event.h_limit, 0);
    TEST_ASSERT_INT_WITHIN(2, event.zero_times, 2);
    TEST_ASSERT_INT_WITHIN(2, event.filter_time, 8);

    // Because this test uses its own ISR, we need to release it with `pcnt_isr_unregister` instead of `pcnt_isr_service_uninstall`
    TEST_ESP_OK(pcnt_isr_unregister(pcnt_isr_service));
    vQueueDelete(pcnt_evt_queue);
}

TEST_CASE("PCNT_counting_mode_test", "[pcnt]")
{
    printf("PCNT mode test for positive count\n");
    count_mode_test(PCNT_CTRL_VCC_IO);
    printf("PCNT mode test for negative count\n");
    count_mode_test(PCNT_CTRL_GND_IO);
}
#endif // SOC_LEDC_SUPPORTED
