/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_filter.h"

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
    adc_oneshot_unit_handle_t oneshot_handle;    //oneshot handle
    adc_continuous_handle_t continuous_handle;   //continuous handle
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
    TEST_ESP_OK(adc_oneshot_read_isr(test_ctx->oneshot_handle, ADC1_TEST_CHAN0, &adc_raw));
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
    isr_test_ctx.oneshot_handle = NULL;
    isr_test_ctx.task_handle = xTaskGetCurrentTaskHandle();

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &isr_test_ctx.oneshot_handle));

    //-------------ADC1 TEST Channel 0 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(isr_test_ctx.oneshot_handle, ADC1_TEST_CHAN0, &config));

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
    TEST_ESP_OK(gptimer_stop(timer));

    //Tear Down
    TEST_ESP_OK(gptimer_disable(timer));
    TEST_ESP_OK(gptimer_del_timer(timer));
    TEST_ESP_OK(adc_oneshot_del_unit(isr_test_ctx.oneshot_handle));
}

#if SOC_ADC_DMA_SUPPORTED
#if (SOC_ADC_DIGI_RESULT_BYTES == 2)
#define ADC_DRIVER_TEST_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define ADC_DRIVER_TEST_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
#define ADC_DRIVER_TEST_GET_DATA(p_data)        ((p_data)->type1.data)
#else
#define ADC_DRIVER_TEST_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define ADC_DRIVER_TEST_GET_CHANNEL(p_data)     ((p_data)->type2.channel)
#define ADC_DRIVER_TEST_GET_DATA(p_data)        ((p_data)->type2.data)
#endif

#define ADC_FRAME_TEST_SIZE    8192

static bool IRAM_ATTR NOINLINE_ATTR s_conv_done_cb_frame_size_test(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    test_adc_isr_ctx_t *test_ctx = (test_adc_isr_ctx_t *)user_data;
    BaseType_t high_task_wakeup;

    vTaskNotifyGiveFromISR(test_ctx->task_handle, &high_task_wakeup);

    return high_task_wakeup == pdTRUE;
}

TEST_CASE("ADC continuous big conv_frame_size test", "[adc_continuous]")
{
    static test_adc_isr_ctx_t isr_test_ctx = {};
    isr_test_ctx.continuous_handle = NULL;
    isr_test_ctx.task_handle = xTaskGetCurrentTaskHandle();

    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = ADC_FRAME_TEST_SIZE,
        .conv_frame_size = ADC_FRAME_TEST_SIZE,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));
    isr_test_ctx.continuous_handle = handle;

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 50 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DRIVER_TEST_OUTPUT_TYPE,
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_pattern[0].atten = ADC_ATTEN_DB_12;
    adc_pattern[0].channel = ADC1_TEST_CHAN0;
    adc_pattern[0].unit = ADC_UNIT_1;
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    dig_cfg.adc_pattern = adc_pattern;
    dig_cfg.pattern_num = 1;
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb_frame_size_test,
    };
    TEST_ESP_OK(adc_continuous_register_event_callbacks(handle, &cbs, &isr_test_ctx));

    uint8_t* result = malloc(ADC_FRAME_TEST_SIZE);
    TEST_ASSERT(result);

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 0);
    TEST_ESP_OK(adc_continuous_start(handle));

    for (int i = 0; i < 5; i++) {
        uint32_t ret_num = 0;
        uint32_t sum = 0;
        uint32_t cnt = 0;
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        TEST_ESP_OK(adc_continuous_read(handle, result, ADC_FRAME_TEST_SIZE, &ret_num, ADC_MAX_DELAY));
        esp_rom_printf("ret_num: %d\n", ret_num);
        for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
            adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
            sum += ADC_DRIVER_TEST_GET_DATA(p);
            cnt++;
        }
        esp_rom_printf("avg: %d\n", sum / cnt);
        TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, sum / cnt);
    }

    TEST_ESP_OK(adc_continuous_stop(handle));
    TEST_ESP_OK(adc_continuous_deinit(handle));
    free(result);
}

#define ADC_FLUSH_TEST_SIZE    64

TEST_CASE("ADC continuous flush internal pool", "[adc_continuous][mannual][ignore]")
{
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = ADC_FLUSH_TEST_SIZE,
        .conv_frame_size = ADC_FLUSH_TEST_SIZE,
        .flags.flush_pool = true,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 50 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DRIVER_TEST_OUTPUT_TYPE,
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_pattern[0].atten = ADC_ATTEN_DB_12;
    adc_pattern[0].channel = ADC1_TEST_CHAN0;
    adc_pattern[0].unit = ADC_UNIT_1;
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    dig_cfg.adc_pattern = adc_pattern;
    dig_cfg.pattern_num = 1;
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

    uint8_t result[ADC_FLUSH_TEST_SIZE] = {0};
    uint32_t ret_num = 0;
    TEST_ESP_OK(adc_continuous_start(handle));

    while (1) {
        TEST_ESP_OK(adc_continuous_read(handle, result, ADC_FLUSH_TEST_SIZE, &ret_num, ADC_MAX_DELAY));
        printf("ret_num: 0d%"PRIu32" bytes\n", ret_num);
        TEST_ASSERT(ret_num == ADC_FLUSH_TEST_SIZE);

        for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
            adc_digi_output_data_t *p = (void*)&result[i];
#if (SOC_ADC_DIGI_RESULT_BYTES == 2)
            printf("ADC1, Channel: %d, Value: %d\n", p->type1.channel, p->type1.data);
#else
            printf("ADC1, Channel: %d, Value: %d\n", p->type2.channel, p->type2.data);
#endif
        }
        /**
         * With this delay, check the read out data to be the newest data
         */
        vTaskDelay(2000);
    }

    TEST_ESP_OK(adc_continuous_stop(handle));
    TEST_ESP_OK(adc_continuous_deinit(handle));
}

#define ADC_RESTART_TEST_SIZE   4096
#define ADC_READ_TEST_COUNT     100

TEST_CASE("ADC continuous test after restarting", "[adc_continuous]")
{
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = ADC_RESTART_TEST_SIZE,
        .conv_frame_size = ADC_RESTART_TEST_SIZE,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 50 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DRIVER_TEST_OUTPUT_TYPE,
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_pattern[0].atten = ADC_ATTEN_DB_12;
    adc_pattern[0].channel = ADC1_TEST_CHAN0;
    adc_pattern[0].unit = ADC_UNIT_1;
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    dig_cfg.adc_pattern = adc_pattern;
    dig_cfg.pattern_num = 1;
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

    uint8_t* result = malloc(ADC_RESTART_TEST_SIZE);
    TEST_ASSERT(result);

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 0);

    for (int i = 0; i < ADC_READ_TEST_COUNT; i++) {
        uint32_t ret_num = 0;
        TEST_ESP_OK(adc_continuous_start(handle));
        TEST_ESP_OK(adc_continuous_read(handle, result, ADC_RESTART_TEST_SIZE, &ret_num, ADC_MAX_DELAY));
        TEST_ASSERT_EQUAL(ADC_RESTART_TEST_SIZE, ret_num);
        for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
            adc_digi_output_data_t *p = (void*)&result[i];
            uint32_t chan_num = ADC_DRIVER_TEST_GET_CHANNEL(p);
            TEST_ASSERT(chan_num < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1));
        }
        TEST_ESP_OK(adc_continuous_stop(handle));
    }

    TEST_ESP_OK(adc_continuous_deinit(handle));
    free(result);
}

#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
TEST_CASE("ADC filter exhausted allocation", "[adc_continuous]")
{
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 1024,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_iir_filter_handle_t filter_hdl[SOC_ADC_DIGI_IIR_FILTER_NUM + 1] = {};
    adc_continuous_iir_filter_config_t filter_config = {
        .unit = ADC_UNIT_1,
        .channel = ADC_CHANNEL_0,
        .coeff = ADC_DIGI_IIR_FILTER_COEFF_2,
    };
    for (int i = 0; i < SOC_ADC_DIGI_IIR_FILTER_NUM; i++) {
#if SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED
        //On these chips, the unit and the filter_id should be the same
        filter_config.unit = i;
#endif
        TEST_ESP_OK(adc_new_continuous_iir_filter(handle, &filter_config, &filter_hdl[i]));
    }

    filter_config.unit = ADC_UNIT_1;
    TEST_ASSERT(adc_new_continuous_iir_filter(handle, &filter_config, &filter_hdl[SOC_ADC_DIGI_IIR_FILTER_NUM]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_ADC_DIGI_IIR_FILTER_NUM; i++) {
        TEST_ESP_OK(adc_del_continuous_iir_filter(filter_hdl[i]));
    }

    TEST_ESP_OK(adc_continuous_deinit(handle));
}
#endif  //#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
#endif  //#if SOC_ADC_DMA_SUPPORTED
