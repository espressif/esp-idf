/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "esp_rom_sys.h"
#include "test_common_adc.h"
#include "esp_adc/adc_oneshot.h"

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

typedef struct {
    void *adc_handle;                            //ADC handle, could be oneshot / continuous handle
    bool level;                                  //ADC level
    int adc_raw_high;                            //ADC reading raw when IO tie high
    int cb_exe_times_high;                       //Callback running times when IO tie high and $ disabled
    int adc_raw_low;                             //ADC reading raw when IO tie low
    int cb_exe_times_low;                        //Callback running times when IO tie low and $ disabled
    bool cache_disable_flag;                     //Indicating cache is disabled
} test_adc_iram_ctx_t;

extern void spi_flash_disable_interrupts_caches_and_other_cpu(void);
extern void spi_flash_enable_interrupts_caches_and_other_cpu(void);
__attribute__((unused))
static void s_test_cache_disable_period_us(test_adc_iram_ctx_t *ctx, uint32_t period_us);

#if CONFIG_ADC_ONESHOT_CTRL_FUNC_IN_IRAM && CONFIG_GPTIMER_ISR_IRAM_SAFE
/*---------------------------------------------------------------
        ADC oneshot work with cache safe ISR
---------------------------------------------------------------*/
static bool IRAM_ATTR NOINLINE_ATTR s_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    test_adc_iram_ctx_t *test_ctx = (test_adc_iram_ctx_t *)user_data;
    adc_oneshot_unit_handle_t oneshot_handle = *(adc_oneshot_unit_handle_t *)(test_ctx->adc_handle);

    if (test_ctx->cache_disable_flag) {

        if (test_ctx->level) {
            adc_oneshot_read_isr(oneshot_handle, ADC1_TEST_CHAN0, &test_ctx->adc_raw_high);
            esp_rom_printf(DRAM_STR("adc raw: %d\n"), test_ctx->adc_raw_high);
            test_ctx->cb_exe_times_high++;
        } else {
            adc_oneshot_read_isr(oneshot_handle, ADC1_TEST_CHAN0, &test_ctx->adc_raw_low);
            esp_rom_printf(DRAM_STR("adc raw: %d\n"), test_ctx->adc_raw_low);
            test_ctx->cb_exe_times_low++;
        }
    }

    return false;
}

TEST_CASE("ADC oneshot fast work with ISR and Flash", "[adc_oneshot]")
{
    adc_oneshot_unit_handle_t oneshot_handle;
    static DRAM_ATTR test_adc_iram_ctx_t isr_test_ctx = {};
    isr_test_ctx.adc_handle = &oneshot_handle;

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &oneshot_handle));

    //-------------ADC1 TEST Channel 0 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(oneshot_handle, ADC1_TEST_CHAN0, &config));

    //-------------GPTimer Init & Config---------------//
    gptimer_handle_t timer = NULL;
    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &timer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = s_alarm_cb,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100000, // 100ms
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(timer, &alarm_config));
    TEST_ESP_OK(gptimer_register_event_callbacks(timer, &cbs, &isr_test_ctx));

    //ADC IO tile low
    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 0);
    isr_test_ctx.level = 0;
    printf("start timer\r\n");
    TEST_ESP_OK(gptimer_enable(timer));
    TEST_ESP_OK(gptimer_start(timer));
    s_test_cache_disable_period_us(&isr_test_ctx, 100 * 1000);
    TEST_ESP_OK(gptimer_stop(timer));
    //Checks
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, isr_test_ctx.adc_raw_low);
    esp_rom_printf("callback runs %d times when $ disabled\n", isr_test_ctx.cb_exe_times_low);
    TEST_ASSERT_GREATER_OR_EQUAL(1, isr_test_ctx.cb_exe_times_low);

    //ADC IO tile high
    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 1);
    isr_test_ctx.level = 1;
    //Reset counter value to zero
    TEST_ESP_OK(gptimer_set_raw_count(timer, 0));
    printf("start timer\r\n");
    TEST_ESP_OK(gptimer_start(timer));
    s_test_cache_disable_period_us(&isr_test_ctx, 100 * 1000);
    TEST_ESP_OK(gptimer_stop(timer));
    //Checks
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, isr_test_ctx.adc_raw_high);
    esp_rom_printf("callback runs %d times when $ disabled\n", isr_test_ctx.cb_exe_times_high);
    TEST_ASSERT_GREATER_OR_EQUAL(1, isr_test_ctx.cb_exe_times_high);

    //Tear Down
    TEST_ESP_OK(gptimer_disable(timer));
    TEST_ESP_OK(gptimer_del_timer(timer));
    TEST_ESP_OK(adc_oneshot_del_unit(oneshot_handle));
}
#endif  //#if CONFIG_ADC_ONESHOT_CTRL_FUNC_IN_IRAM && CONFIG_GPTIMER_ISR_IRAM_SAFE

#if CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE || CONFIG_GDMA_ISR_IRAM_SAFE
#include "esp_adc/adc_continuous.h"
/*---------------------------------------------------------------
        ADC continuous work with cache safe ISR
---------------------------------------------------------------*/
#if (SOC_ADC_DIGI_RESULT_BYTES == 2)
#define ADC_TEST_OUTPUT_TYPE    ADC_DIGI_OUTPUT_FORMAT_TYPE1
#else
#define ADC_TEST_OUTPUT_TYPE    ADC_DIGI_OUTPUT_FORMAT_TYPE2
#endif

#define ADC_TEST_FREQ_HZ        (50 * 1000)
#define ADC_TEST_PKG_SIZE       512
static bool IRAM_ATTR NOINLINE_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    test_adc_iram_ctx_t *test_ctx = (test_adc_iram_ctx_t *)user_data;

    if (test_ctx->cache_disable_flag) {
        int raw = 0;

        for (int i = 0; i < edata->size; i += SOC_ADC_DIGI_RESULT_BYTES) {
            adc_digi_output_data_t *p = (void*) & (edata->conv_frame_buffer[i]);
#if (SOC_ADC_DIGI_RESULT_BYTES == 2)
            raw += p->type1.data;
#else
            raw += p->type2.data;
#endif
        }
        if (test_ctx->level) {
            test_ctx->adc_raw_high = raw / (edata->size / SOC_ADC_DIGI_RESULT_BYTES);
            test_ctx->cb_exe_times_high++;
        } else {
            test_ctx->adc_raw_low = raw / (edata->size / SOC_ADC_DIGI_RESULT_BYTES);
            test_ctx->cb_exe_times_low++;
        }
    }

    return false;
}

TEST_CASE("ADC continuous work with ISR and Flash", "[adc_continuous]")
{
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = ADC_TEST_PKG_SIZE,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = ADC_TEST_FREQ_HZ,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_TEST_OUTPUT_TYPE,
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_pattern[0].atten = ADC_ATTEN_DB_0;
    adc_pattern[0].channel = ADC1_TEST_CHAN0;
    adc_pattern[0].unit = ADC_UNIT_1;
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    dig_cfg.adc_pattern = adc_pattern;
    dig_cfg.pattern_num = 1;
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

    static DRAM_ATTR test_adc_iram_ctx_t isr_test_ctx = {};
    isr_test_ctx.adc_handle = &handle;
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    TEST_ESP_OK(adc_continuous_register_event_callbacks(handle, &cbs, &isr_test_ctx));

    /**
     * Ideal time consumption for one conversion_frame done is `(1000 * 1000 / ADC_TEST_FREQ_HZ * ADC_TEST_PKG_SIZE / SOC_ADC_DIGI_RESULT_BYTES)`
     * Here we just wait for 1 second
     */
    uint32_t wait_time_us = 1 * 1000 * 1000;
    printf("period is %"PRId32" us\n", wait_time_us);

    //ADC IO tile low
    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 0);
    isr_test_ctx.level = 0;
    TEST_ESP_OK(adc_continuous_start(handle));
    s_test_cache_disable_period_us(&isr_test_ctx, wait_time_us);
    TEST_ESP_OK(adc_continuous_stop(handle));
    //Checks
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, isr_test_ctx.adc_raw_low);
    esp_rom_printf("callback runs %d times when $ disabled\n", isr_test_ctx.cb_exe_times_low);
    //At least 1 time conv_done callback happens during this period is ok
    TEST_ASSERT_GREATER_OR_EQUAL(1, isr_test_ctx.cb_exe_times_low);

    vTaskDelay(10);

    printf("to set high\n");
    //ADC IO tile high
    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 1);
    isr_test_ctx.level = 1;
    TEST_ESP_OK(adc_continuous_start(handle));
    s_test_cache_disable_period_us(&isr_test_ctx, wait_time_us);
    TEST_ESP_OK(adc_continuous_stop(handle));
    //Checks
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL_DMA, isr_test_ctx.adc_raw_high);
    esp_rom_printf("callback runs %d times when $ disabled\n", isr_test_ctx.cb_exe_times_high);
    //At least 1 time conv_done callback happens during this period is ok
    TEST_ASSERT_GREATER_OR_EQUAL(1, isr_test_ctx.cb_exe_times_high);

    TEST_ESP_OK(adc_continuous_deinit(handle));

}
#endif  //#if CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE || CONFIG_GDMA_ISR_IRAM_SAFE

static void IRAM_ATTR NOINLINE_ATTR s_test_cache_disable_period_us(test_adc_iram_ctx_t *ctx, uint32_t period_us)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    ctx->cache_disable_flag = 1;

    esp_rom_delay_us(period_us);

    ctx->cache_disable_flag = 0;
    spi_flash_enable_interrupts_caches_and_other_cpu();
}
