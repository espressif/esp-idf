/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "esp_rom_sys.h"
#include "esp_adc/adc_oneshot.h"
#include "test_common_adc.h"

const __attribute__((unused)) static char *TAG = "TEST_ADC";

/*---------------------------------------------------------------
        ADC General Macros
---------------------------------------------------------------*/
//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_TEST_CHAN0          ADC_CHANNEL_4
#else
#define ADC1_TEST_CHAN0          ADC_CHANNEL_2
#endif

/*---------------------------------------------------------------
        ADC work with ISR
---------------------------------------------------------------*/
typedef struct {
    TaskHandle_t task_handle;    //Task handle
    adc_oneshot_unit_handle_t adc_handle;    //ADC handle
    bool level;    //ADC level
} test_adc_isr_ctx_t;

static bool IRAM_ATTR s_alarm_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    test_adc_isr_ctx_t *test_ctx = (test_adc_isr_ctx_t *)user_data;
    BaseType_t high_task_wakeup;
    int adc_raw = 0;

    /**
     * This test won't disable the cache, so having some code on Flash is OK.
     * If you copy this test callback with cache disabled, do remeber to put all code in internal RAM.
     */

    esp_rom_printf("alarm isr count=%llu\r\n", edata->count_value);
    TEST_ESP_OK(adc_oneshot_read_isr(test_ctx->adc_handle, ADC1_TEST_CHAN0, &adc_raw));
    esp_rom_printf("adc raw: %d\r\n", adc_raw);
    if (test_ctx->level) {
        TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw);
    } else {
        TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw);
    }


    // check the count value at alarm event
    vTaskNotifyGiveFromISR(test_ctx->task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}


TEST_CASE("ADC oneshot fast work with ISR", "[adc_oneshot]")
{
    static test_adc_isr_ctx_t isr_test_ctx = {};
    isr_test_ctx.adc_handle = NULL;
    isr_test_ctx.task_handle = xTaskGetCurrentTaskHandle();

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = false,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &isr_test_ctx.adc_handle));

    //-------------ADC1 TEST Channel 0 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(isr_test_ctx.adc_handle, ADC1_TEST_CHAN0, &config));

    //-------------GPTimer Init & Config---------------//
    gptimer_handle_t timer = NULL;
    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &timer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = s_alarm_callback,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100000, // 100ms
    };
    TEST_ESP_OK(gptimer_set_alarm_action(timer, &alarm_config));
    TEST_ESP_OK(gptimer_register_event_callbacks(timer, &cbs, &isr_test_ctx));

    //ADC IO tile low
    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 0);
    isr_test_ctx.level = 0;
    printf("start timer\r\n");
    TEST_ESP_OK(gptimer_enable(timer));
    TEST_ESP_OK(gptimer_start(timer));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ESP_OK(gptimer_stop(timer));


    //ADC IO tile high
    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 1);
    isr_test_ctx.level = 1;
    //Reset counter value to zero
    TEST_ESP_OK(gptimer_set_raw_count(timer, 0));
    printf("start timer\r\n");
    TEST_ESP_OK(gptimer_start(timer));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));

    //Tear Down
    TEST_ESP_OK(gptimer_disable(timer));
    TEST_ESP_OK(gptimer_del_timer(timer));
    TEST_ESP_OK(adc_oneshot_del_unit(isr_test_ctx.adc_handle));
}
