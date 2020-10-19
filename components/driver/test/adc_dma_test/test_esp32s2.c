// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 Tests for the adc device driver
*/

#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "soc/adc_periph.h"
#include "test/test_common_adc.h"

#if !DISABLED_FOR_TARGETS(ESP8266, ESP32) // This testcase for ESP32S2

#include "soc/system_reg.h"
#include "soc/lldesc.h"
#include "test/test_adc_dac_dma.h"

static const char *TAG = "test_adc";

#define PLATFORM_SELECT            (1)  //0: pxp; 1: chip
#if (PLATFORM_SELECT == 0)              //PXP platform
#include "soc/apb_ctrl_reg.h"
#define SET_BREAK_POINT(flag) REG_WRITE(APB_CTRL_DATE_REG, flag)
//PXP clk is slower.
#define SYS_DELAY_TIME_MOM    (1/40)
#define RTC_SLOW_CLK_FLAG     1     // Slow clock is 32KHz.
static void test_pxp_deinit_io(void)
{
    for (int i = 0; i < 22; i++) {
        rtc_gpio_init(i);
    }
}
#else
//PXP clk is slower.
#define SET_BREAK_POINT(flag)
#define SYS_DELAY_TIME_MOM    (1)
#define RTC_SLOW_CLK_FLAG     0     // Slow clock is 32KHz.
#endif

#define ADC_REG_BASE_TEST() ({    \
   TEST_ASSERT_EQUAL_UINT32(REG_GET_FIELD(APB_SARADC_APB_CTRL_DATE_REG, APB_SARADC_APB_CTRL_DATE), APB_SARADC.apb_ctrl_date);   \
   TEST_ASSERT_EQUAL_UINT32(REG_GET_FIELD(SENS_SARDATE_REG, SENS_SAR_DATE), SENS.sardate.sar_date);     \
   TEST_ASSERT_EQUAL_UINT32(REG_GET_FIELD(RTC_IO_DATE_REG, RTC_IO_IO_DATE), RTCIO.date.date);           \
})
/** Sample rate = APB_CLK(80 MHz) / (CLK_DIV + 1) / TRIGGER_INTERVAL / 2. */
#define TEST_ADC_TRIGGER_INTERVAL_DEFAULT (40)
#define TEST_ADC_DIGI_CLK_DIV_DEFAULT     (9)
static uint8_t adc_test_num = 9;
static adc_channel_t adc_list[SOC_ADC_PATT_LEN_MAX] = {
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    // ADC_CHANNEL_7, // Workaround: IO18 is pullup outside in ESP32S2-Saola Runner.
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
};
/* For ESP32S2, it should use same atten, or, it will have error. */
#define TEST_ADC_ATTEN_DEFAULT (ADC_ATTEN_11db)

extern esp_err_t adc_digi_reset(void);

/* Work mode.
 * single: eof_num;
 * double: SAR_EOF_NUMBER/2;
 * alter: eof_num;
 * */
#define SAR_SIMPLE_NUM  512  // Set sample number of enabled unit.
/* Use two DMA linker to save ADC data. ADC sample 1 times -> 2 byte data -> 2 DMA link buf. */
#define SAR_DMA_DATA_SIZE(unit, sample_num)     (SAR_EOF_NUMBER(unit, sample_num))
#define SAR_EOF_NUMBER(unit, sample_num)        ((sample_num) * (unit))
#define SAR_MEAS_LIMIT_NUM(unit, sample_num)    (SAR_SIMPLE_NUM)
#define SAR_SIMPLE_TIMEOUT_MS  1000

typedef struct dma_msg {
    uint32_t int_msk;
    uint8_t *data;
    uint32_t data_len;
} adc_dma_event_t;

static uint8_t link_buf[2][SAR_DMA_DATA_SIZE(2, SAR_SIMPLE_NUM)] = {0};
static lldesc_t dma1 = {0};
static lldesc_t dma2 = {0};
static QueueHandle_t que_adc = NULL;
static adc_dma_event_t adc_evt;

/** ADC-DMA ISR handler. */
static IRAM_ATTR void adc_dma_isr(void *arg)
{
    uint32_t int_st = REG_READ(SPI_DMA_INT_ST_REG(3));
    int task_awoken = pdFALSE;
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), int_st);
    if (int_st & SPI_IN_SUC_EOF_INT_ST_M) {
        adc_evt.int_msk = int_st;
        xQueueSendFromISR(que_adc, &adc_evt, &task_awoken);
    }
    if (int_st & SPI_IN_DONE_INT_ST) {
        adc_evt.int_msk = int_st;
        xQueueSendFromISR(que_adc, &adc_evt, &task_awoken);
    }
    ESP_EARLY_LOGV(TAG, "int msk%x\n", int_st);
    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * DMA liner initialization and start.
 * @param is_loop
 *     - true: The two dma linked lists are connected end to end, with no end mark (eof).
 *     - false: The two dma linked lists are connected end to end, with end mark (eof).
 */
static uint32_t adc_dma_linker_init(adc_unit_t adc, bool is_loop)
{
    dma1 = (lldesc_t) {
        .size = SAR_DMA_DATA_SIZE((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM),
        .owner = 1,
        .buf = &link_buf[0][0],
        .qe.stqe_next = &dma2,
    };
    dma2 = (lldesc_t) {
        .size = SAR_DMA_DATA_SIZE((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM),
        .owner = 1,
        .buf = &link_buf[1][0],
    };
    if (is_loop) {
        dma2.qe.stqe_next = &dma1;
    } else {
        dma2.qe.stqe_next = NULL;
    }
    return (uint32_t)&dma1;
}

#define DEBUG_CHECK_ENABLE  1
#define DEBUG_PRINT_ENABLE  1
#define DEBUG_CHECK_ERROR   10

/**
 * Check the ADC-DMA data in linker buffer by input level.
 * ideal_level
 *     - -1: Don't check data.
 *     -  0: ADC channel voltage is 0v.
 *     -  1: ADC channel voltage is 3.3v.
 *     -  2: ADC channel voltage is 1.4v.
 */
static esp_err_t adc_dma_data_check(adc_unit_t adc, int ideal_level)
{
    int unit_old = 1;
    int ch_cnt = 0;
    for (int cnt = 0; cnt < 2; cnt++) {
        ets_printf("\n[%s] link_buf[%d]: \n", __func__, cnt % 2);
        for (int i = 0; i < SAR_DMA_DATA_SIZE((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM); i += 2) {
            uint8_t h = link_buf[cnt % 2][i + 1], l = link_buf[cnt % 2][i];
            uint16_t temp = (h << 8 | l);
            adc_digi_output_data_t *data = (adc_digi_output_data_t *)&temp;

            if (adc > ADC_UNIT_2) {  //ADC_ENCODE_11BIT
#if DEBUG_PRINT_ENABLE
                if (i % 16 == 0) {
                    ets_printf("\n");
                }
                ets_printf("[%d_%d_%04x] ", data->type2.unit, data->type2.channel, data->type2.data);
#endif
#if DEBUG_CHECK_ENABLE
                if (ideal_level >= 0) {
                    TEST_ASSERT_NOT_EQUAL(unit_old, data->type2.unit);
                    unit_old = data->type2.unit;
                    if (data->type2.channel > ADC_CHANNEL_MAX) {
                        printf("Data invalid [%d]\n", data->type2.channel);
                        continue;
                    }
                    int cur_ch = ((ch_cnt++ / 2) % adc_test_num);
                    TEST_ASSERT_EQUAL( data->type2.channel, adc_list[cur_ch] );
                }
                if (ideal_level == 1) {         // high level 3.3v
                    TEST_ASSERT_EQUAL( 0x7FF, data->type2.data );
                } else if (ideal_level == 0) {  // low level 0v
                    TEST_ASSERT_LESS_THAN( 10, data->type2.data );
                } else if (ideal_level == 2) {  // middle level 1.4v
                    TEST_ASSERT_INT_WITHIN( 128, 1100, data->type2.data );
                } else if (ideal_level == 3) {  // normal level
                } else { // no check
                }
#endif
            } else {        //ADC_ENCODE_12BIT
#if DEBUG_PRINT_ENABLE
                if (i % 16 == 0) {
                    ets_printf("\n");
                }
                ets_printf("[%d_%04x] ", data->type1.channel, data->type1.data);
#endif
#if DEBUG_CHECK_ENABLE
                if (ideal_level >= 0) {
                    int cur_ch = ((ch_cnt++) % adc_test_num);
                    TEST_ASSERT_EQUAL( adc_list[cur_ch], data->type1.channel );
                }
                if (ideal_level == 1) {         // high level 3.3v
                    TEST_ASSERT_EQUAL( 0XFFF, data->type1.data );
                } else if (ideal_level == 0) {  // low level 0v
                    TEST_ASSERT_LESS_THAN( 10, data->type1.data );
                } else if (ideal_level == 2) {  // middle level 1.4v
                    TEST_ASSERT_INT_WITHIN( 256, 2200, data->type1.data );
                } else if (ideal_level == 3) {  // normal level
                } else { // no check
                }
#endif
            }
            link_buf[cnt % 2][i] = 0;
            link_buf[cnt % 2][i + 1] = 0;
        }
        ets_printf("\n");
    }
    return ESP_OK;
}

static esp_err_t adc_dma_data_multi_st_check(adc_unit_t adc, void *dma_addr, uint32_t int_mask)
{
    adc_dma_event_t evt;

    ESP_LOGI(TAG, "adc IO normal, test ...");
    for (int i = 0; i < adc_test_num; i++) {
        adc_io_normal(adc, adc_list[i]);
    }
    TEST_ESP_OK( adc_digi_start() );
    while (1) {
        TEST_ASSERT_EQUAL( xQueueReceive(que_adc, &evt, SAR_SIMPLE_TIMEOUT_MS / portTICK_RATE_MS), pdTRUE );
        if (evt.int_msk & SPI_IN_SUC_EOF_INT_ENA) {
            break;
        }
    }
    TEST_ESP_OK( adc_digi_stop() );
    adc_dac_dma_linker_start(DMA_ONLY_ADC_INLINK, (void *)dma_addr, int_mask);
    adc_digi_reset();
    TEST_ESP_OK( adc_dma_data_check(adc, -1) ); // Don't check data.

    ESP_LOGI(TAG, "adc IO fake tie high, test ...");
    for (int i = 0; i < adc_test_num; i++) {
        adc_fake_tie_high(adc, adc_list[i]);
    }
    TEST_ESP_OK( adc_digi_start() );
    while (1) {
        TEST_ASSERT_EQUAL( xQueueReceive(que_adc, &evt, SAR_SIMPLE_TIMEOUT_MS / portTICK_RATE_MS), pdTRUE );
        if (evt.int_msk & SPI_IN_SUC_EOF_INT_ENA) {
            break;
        }
    }
    TEST_ESP_OK( adc_digi_stop() );
    adc_dac_dma_linker_start(DMA_ONLY_ADC_INLINK, (void *)dma_addr, int_mask);
    adc_digi_reset();
    TEST_ESP_OK( adc_dma_data_check(adc, 1) );

    ESP_LOGI(TAG, "adc IO fake tie low, test ...");
    for (int i = 0; i < adc_test_num; i++) {
        adc_fake_tie_low(adc, adc_list[i]);
    }
    TEST_ESP_OK( adc_digi_start() );
    while (1) {
        TEST_ASSERT_EQUAL( xQueueReceive(que_adc, &evt, SAR_SIMPLE_TIMEOUT_MS / portTICK_RATE_MS), pdTRUE );
        if (evt.int_msk & SPI_IN_SUC_EOF_INT_ENA) {
            break;
        }
    }
    TEST_ESP_OK( adc_digi_stop() );
    adc_dac_dma_linker_start(DMA_ONLY_ADC_INLINK, (void *)dma_addr, int_mask);
    adc_digi_reset();
    TEST_ESP_OK( adc_dma_data_check(adc, 0) );

    ESP_LOGI(TAG, "adc IO fake tie middle, test ...");
    for (int i = 0; i < adc_test_num; i++) {
        adc_fake_tie_middle(adc, adc_list[i]);
    }
    TEST_ESP_OK( adc_digi_start() );
    while (1) {
        TEST_ASSERT_EQUAL( xQueueReceive(que_adc, &evt, SAR_SIMPLE_TIMEOUT_MS / portTICK_RATE_MS), pdTRUE );
        if (evt.int_msk & SPI_IN_SUC_EOF_INT_ENA) {
            break;
        }
    }
    TEST_ESP_OK( adc_digi_stop() );
    adc_dac_dma_linker_start(DMA_ONLY_ADC_INLINK, (void *)dma_addr, int_mask);
    adc_digi_reset();
    TEST_ESP_OK( adc_dma_data_check(adc, 2) );

    return ESP_OK;
}

#include "soc/apb_saradc_struct.h"
/**
 * Test the partten table setting. It's easy wrong.
 *
 * @param adc_n ADC unit.
 * @param in_partten_len The length of partten be set.
 * @param in_last_ch The channel number of the last message.
 */
static esp_err_t adc_check_patt_table(adc_unit_t adc, uint32_t in_partten_len, adc_channel_t in_last_ch)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t index = (in_partten_len - 1) / 4;
    uint8_t offset = 24 - ((in_partten_len - 1) % 4) * 8;
    uint32_t temp = 0, len;

    if (adc & ADC_UNIT_1) {
        len = APB_SARADC.ctrl.sar1_patt_len + 1;
        temp = APB_SARADC.sar1_patt_tab[index];
        printf("patt1 len %d\n", len);
        printf("patt1 0x%08x\n", APB_SARADC.sar1_patt_tab[0]);
        printf("patt1 0x%08x\n", APB_SARADC.sar1_patt_tab[1]);
        printf("patt1 0x%08x\n", APB_SARADC.sar1_patt_tab[2]);
        printf("patt1 0x%08x\n", APB_SARADC.sar1_patt_tab[3]);
        if (in_partten_len == len) {
            if (in_last_ch == (((temp >> (offset + 4))) & 0xf)) {
                ret = ESP_OK;
            }
        }
    }
    if (adc & ADC_UNIT_2) {
        len = APB_SARADC.ctrl.sar2_patt_len + 1;
        temp = APB_SARADC.sar2_patt_tab[index];
        printf("patt2 len %d\n", len);
        printf("patt2 0x%08x\n", APB_SARADC.sar2_patt_tab[0]);
        printf("patt2 0x%08x\n", APB_SARADC.sar2_patt_tab[1]);
        printf("patt2 0x%08x\n", APB_SARADC.sar2_patt_tab[2]);
        printf("patt2 0x%08x\n", APB_SARADC.sar2_patt_tab[3]);
        if (in_partten_len == len) {
            if (in_last_ch == (((temp >> (offset + 4))) & 0xf)) {
                ret = ESP_OK;
            }
        }
    }
    return ret;
}

/**
 * Testcase: Check the base function of ADC-DMA. Include:
 * - Various conversion modes.
 * - Whether the channel and data are lost.
 * - Whether the data is the same as the channel voltage.
 */
int test_adc_dig_dma_single_unit(adc_unit_t adc)
{
    ESP_LOGI(TAG, "  >> %s <<  ", __func__);
    ESP_LOGI(TAG, "  >> adc unit: %x <<  ", adc);

    TEST_ESP_OK( adc_digi_init() );
    /* arbiter config */
    adc_arbiter_t arb_cfg = {
        .mode = ADC_ARB_MODE_FIX,
        .dig_pri = 0,
        .pwdet_pri = 2,
        .rtc_pri = 1,
    };
    TEST_ESP_OK( adc_arbiter_config(ADC_UNIT_2, &arb_cfg) );   // If you want use force

    adc_digi_config_t config = {
        .conv_limit_en = false,
        .conv_limit_num = 0,
        .interval = TEST_ADC_TRIGGER_INTERVAL_DEFAULT,
        .dig_clk.use_apll = 0,  // APB clk
        .dig_clk.div_num = TEST_ADC_DIGI_CLK_DIV_DEFAULT,
        .dig_clk.div_b = 0,
        .dig_clk.div_a = 0,
        .dma_eof_num = SAR_EOF_NUMBER((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM),
    };
    /* Config pattern table */
    adc_digi_pattern_table_t adc1_patt[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_digi_pattern_table_t adc2_patt[SOC_ADC_PATT_LEN_MAX] = {0};
    if (adc & ADC_UNIT_1) {
        config.adc1_pattern_len = adc_test_num;
        config.adc1_pattern = adc1_patt;
        for (int i = 0; i < adc_test_num; i++) {
            adc1_patt[i].atten = TEST_ADC_ATTEN_DEFAULT;
            adc1_patt[i].channel = adc_list[i];
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
        }
    }
    if (adc & ADC_UNIT_2) {
        config.adc2_pattern_len = adc_test_num;
        config.adc2_pattern = adc2_patt;
        for (int i = 0; i < adc_test_num; i++) {
            adc2_patt[i].atten = TEST_ADC_ATTEN_DEFAULT;
            adc2_patt[i].channel = adc_list[i];
            adc_gpio_init(ADC_UNIT_2, adc_list[i]);
        }
    }
    if (adc == ADC_UNIT_1) {
        config.conv_mode = ADC_CONV_SINGLE_UNIT_1;
        config.format = ADC_DIGI_FORMAT_12BIT;
    } else if (adc == ADC_UNIT_2) {
        config.conv_mode = ADC_CONV_SINGLE_UNIT_2;
        config.format = ADC_DIGI_FORMAT_12BIT;
    } else if (adc == ADC_UNIT_BOTH) {
        config.conv_mode = ADC_CONV_BOTH_UNIT;
        config.format = ADC_DIGI_FORMAT_11BIT;
    } else if (adc == ADC_UNIT_ALTER) {
        config.conv_mode = ADC_CONV_ALTER_UNIT;
        config.format = ADC_DIGI_FORMAT_11BIT;
    }
    TEST_ESP_OK( adc_digi_controller_config(&config) );

    /* ADC-DMA linker init */
    if (que_adc == NULL) {
        que_adc = xQueueCreate(5, sizeof(adc_dma_event_t));
    } else {
        xQueueReset(que_adc);
    }
    uint32_t int_mask = SPI_IN_SUC_EOF_INT_ENA;
    uint32_t dma_addr = adc_dma_linker_init(adc, false);
    adc_dac_dma_isr_register(adc_dma_isr, NULL, int_mask);
    adc_dac_dma_linker_start(DMA_ONLY_ADC_INLINK, (void *)dma_addr, int_mask);

    TEST_ESP_OK( adc_check_patt_table(adc, adc_test_num, adc_list[adc_test_num - 1]) );
    adc_dma_data_multi_st_check(adc, (void *)dma_addr, int_mask);
    
    adc_dac_dma_linker_deinit();
    adc_dac_dma_isr_deregister(adc_dma_isr, NULL);
    TEST_ESP_OK( adc_digi_deinit() );
    vTaskDelay(10 / portTICK_RATE_MS);

    return 0;
}

TEST_CASE("ADC DMA single read", "[ADC]")
{
    test_adc_dig_dma_single_unit(ADC_UNIT_BOTH);

    test_adc_dig_dma_single_unit(ADC_UNIT_ALTER);

    test_adc_dig_dma_single_unit(ADC_UNIT_1);

    test_adc_dig_dma_single_unit(ADC_UNIT_2);
}

#include "touch_scope.h"
/**
 * 0: ADC1 channels raw data debug.
 * 1: ADC2 channels raw data debug.
 * 2: ADC1 one channel raw data debug.
 */
#define SCOPE_DEBUG_TYPE            0
#define SCOPE_DEBUG_CHANNEL_MAX    (10)
#define SCOPE_DEBUG_ENABLE         (0)
#define SCOPE_UART_BUADRATE        (256000)
#define SCOPE_DEBUG_FREQ_MS        (50)
#define SCOPE_OUTPUT_UART          (0)
static float scope_temp[SCOPE_DEBUG_CHANNEL_MAX] = {0};  // max scope channel is 10.

int test_adc_dig_scope_debug_unit(adc_unit_t adc)
{
    ESP_LOGI(TAG, "  >> %s <<  ", __func__);
    ESP_LOGI(TAG, "  >> adc unit: %x <<  ", adc);

    TEST_ESP_OK( adc_digi_init() );
    if (adc & ADC_UNIT_2) {
        /* arbiter config */
        adc_arbiter_t arb_cfg = {
            .mode = ADC_ARB_MODE_FIX,
            .dig_pri = 0,
            .pwdet_pri = 2,
            .rtc_pri = 1,
        };
        TEST_ESP_OK( adc_arbiter_config(ADC_UNIT_2, &arb_cfg) );   // If you want use force
    }
    adc_digi_config_t config = {
        .conv_limit_en = false,
        .conv_limit_num = 0,
        .interval = TEST_ADC_TRIGGER_INTERVAL_DEFAULT,
        .dig_clk.use_apll = 0,  // APB clk
        .dig_clk.div_num = TEST_ADC_DIGI_CLK_DIV_DEFAULT,
        .dig_clk.div_a = 0,
        .dig_clk.div_b = 0,
        .dma_eof_num = SAR_EOF_NUMBER((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM),
    };
    /* Config pattern table */
    adc_digi_pattern_table_t adc1_patt[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_digi_pattern_table_t adc2_patt[SOC_ADC_PATT_LEN_MAX] = {0};
    if (adc & ADC_UNIT_1) {
        config.adc1_pattern_len = adc_test_num;
        config.adc1_pattern = adc1_patt;
        for (int i = 0; i < adc_test_num; i++) {
            adc1_patt[i].atten = TEST_ADC_ATTEN_DEFAULT;
            adc1_patt[i].channel = adc_list[i];
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
        }
    }
    if (adc & ADC_UNIT_2) {
        config.adc2_pattern_len = adc_test_num;
        config.adc2_pattern = adc2_patt;
        for (int i = 0; i < adc_test_num; i++) {
            adc2_patt[i].atten = TEST_ADC_ATTEN_DEFAULT;
            adc2_patt[i].channel = adc_list[i];
            adc_gpio_init(ADC_UNIT_2, adc_list[i]);
        }
    }
    if (adc == ADC_UNIT_1) {
        config.conv_mode = ADC_CONV_SINGLE_UNIT_1;
        config.format = ADC_DIGI_FORMAT_12BIT;
    } else if (adc == ADC_UNIT_2) {
        config.conv_mode = ADC_CONV_SINGLE_UNIT_2;
        config.format = ADC_DIGI_FORMAT_12BIT;
    } else if (adc == ADC_UNIT_BOTH) {
        config.conv_mode = ADC_CONV_BOTH_UNIT;
        config.format = ADC_DIGI_FORMAT_11BIT;
    } else if (adc == ADC_UNIT_ALTER) {
        config.conv_mode = ADC_CONV_ALTER_UNIT;
        config.format = ADC_DIGI_FORMAT_11BIT;
    }
    TEST_ESP_OK( adc_digi_controller_config(&config) );

        /* ADC-DMA linker init */
    if (que_adc == NULL) {
        que_adc = xQueueCreate(5, sizeof(adc_dma_event_t));
    } else {
        xQueueReset(que_adc);
    }
    uint32_t int_mask = SPI_IN_SUC_EOF_INT_ENA;
    uint32_t dma_addr = adc_dma_linker_init(adc, false);
    adc_dac_dma_isr_register(adc_dma_isr, NULL, int_mask);
    adc_dac_dma_linker_start(DMA_ONLY_ADC_INLINK, (void *)dma_addr, int_mask);

    ESP_LOGI(TAG, "adc IO fake tie middle, test ...");
    for (int i = 0; i < adc_test_num; i++) {
        adc_fake_tie_middle(adc, adc_list[i]);
    }

    return 0;
}

static void scope_output(int adc_num, int channel, int data)
{
    /** can replace by uart log.*/
#if SCOPE_OUTPUT_UART
    static int icnt = 0;
    if (icnt++ % 8 == 0) {
        ets_printf("\n");
    }
    ets_printf("[%d_%d_%04x] ", adc_num, channel, data);
    return;
#endif
#if SCOPE_DEBUG_TYPE == 0
    if (adc_num != 0) {
        return;
    }
#elif SCOPE_DEBUG_TYPE == 1
    if (adc_num != 1) {
        return;
    }
#endif
    int i;
    /* adc Read */
    for (i = 0; i < adc_test_num; i++) {
        if (adc_list[i] == channel && scope_temp[i] == 0) {
            scope_temp[i] = data;
            break;
        }
    }
    if (i == adc_test_num) {
        test_tp_print_to_scope(scope_temp, adc_test_num);
        vTaskDelay(SCOPE_DEBUG_FREQ_MS / portTICK_RATE_MS);
        for (int i = 0; i < adc_test_num; i++) {
            scope_temp[i] = 0;
        }
    }
}

/**
 * Manual test: Capture ADC-DMA data and display it on the serial oscilloscope. Used to observe the stability of the data.
 * Use step:
 *      1. Run this test from the unit test app.
 *      2. Use `ESP-Tuning Tool`(download from `www.espressif.com`) to capture.
 *      3. The readings of multiple channels will be displayed on the tool.
 */
TEST_CASE("test_adc_digi_slope_debug", "[adc_dma][ignore]")
{
    adc_dma_event_t evt;
    test_tp_scope_debug_init(0, -1, -1, SCOPE_UART_BUADRATE);
    adc_unit_t adc = ADC_CONV_BOTH_UNIT;
    test_adc_dig_scope_debug_unit(adc);
    while (1) {
        TEST_ESP_OK( adc_digi_start() );
        TEST_ASSERT_EQUAL( xQueueReceive(que_adc, &evt, portMAX_DELAY), pdTRUE );
        if (evt.int_msk & SPI_IN_SUC_EOF_INT_ST) {
            TEST_ESP_OK( adc_digi_stop() );
            adc_digi_reset();
            for (int cnt = 0; cnt < 2; cnt++) {
                ets_printf("cnt%d\n", cnt);
                for (int i = 0; i < SAR_DMA_DATA_SIZE((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM); i += 2) {
                    uint8_t h = link_buf[cnt % 2][i + 1], l = link_buf[cnt % 2][i];
                    uint16_t temp = (h << 8 | l);
                    adc_digi_output_data_t *data = (adc_digi_output_data_t *)&temp;
                    if (adc > ADC_UNIT_2) {  //ADC_ENCODE_11BIT
                        scope_output(data->type2.unit, data->type2.channel, data->type2.data);
                    } else {        //ADC_ENCODE_12BIT
                        if (adc == ADC_UNIT_1) {
                            scope_output(0, data->type1.channel, data->type1.data);
                        } else if (adc == ADC_UNIT_2) {
                            scope_output(1, data->type1.channel, data->type1.data);
                        }
                    }
                    link_buf[cnt % 2][i] = 0;
                    link_buf[cnt % 2][i + 1] = 0;
                }
            }
        }
    }
}

#endif // !DISABLED_FOR_TARGETS(ESP8266, ESP32)