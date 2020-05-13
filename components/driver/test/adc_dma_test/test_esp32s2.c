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
#include "soc/spi_reg.h"
#include "soc/adc_periph.h"

#if !DISABLED_FOR_TARGETS(ESP8266, ESP32) // This testcase for ESP32S2

#include "soc/system_reg.h"

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
/** Sample rate = APB_CLK(80 MHz) / CLK_DIV / TRIGGER_INTERVAL */
#define TEST_ADC_TRIGGER_INTERVAL_DEFAULT (80)
#define TEST_ADC_DIGI_CLK_DIV_DEFAULT     (40)
#define TEST_ADC_COUNT_NUM    (10)
#define TEST_ADC_CHANNEL      (10)
static adc_channel_t adc_list[TEST_ADC_CHANNEL] = {
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
};
/* For ESP32S2, it should use same atten, or, it will have error. */
#define TEST_ADC_ATTEN_DEFAULT (ADC_ATTEN_11db)
/*******************************************/
/**           SPI DMA INIT CODE            */
/*******************************************/
extern esp_err_t adc_digi_reset(void);

typedef struct dma_link {
    struct {
        uint32_t size :    12;      //the size of buf, must be able to be divisible by 4
        uint32_t length:   12;      //in link,
        uint32_t reversed: 6;       //reversed
        uint32_t eof:      1;       //if this dma link is the last one, you shoule set this bit 1.
        uint32_t owner:    1;       //the owner of buf, bit 1 : DMA, bit 0 : CPU.
    } des;
    uint8_t *buf;                   //the pointer of buf
    struct dma_link *pnext;    //point to the next dma linker, if this link is the last one, set it NULL.
} dma_link_t;
/* Work mode.
 * single: eof_num;
 * double: SAR_EOF_NUMBER/2;
 * alter: eof_num;
 * */
#define SAR_SIMPLE_NUM  64  // Set sample number of enabled unit.
#define SAR_DMA_DATA_SIZE(unit, sample_num)     (SAR_EOF_NUMBER(unit, sample_num)) // 1 adc -> 2 byte data -> 2 link buf
#define SAR_EOF_NUMBER(unit, sample_num)        ((sample_num) * (unit))
#define SAR_MEAS_LIMIT_NUM(unit, sample_num)    (SAR_EOF_NUMBER(unit, sample_num) / unit)

static uint8_t link_buf[2][SAR_DMA_DATA_SIZE(2, SAR_SIMPLE_NUM)] = {0};
static dma_link_t dma1;
static dma_link_t dma2;

static void dma_linker_init(adc_unit_t adc, bool is_loop)
{
    dma1.des.eof = 0;
    dma1.des.owner = 1;
    dma1.pnext = &dma2;
    dma1.des.size = SAR_DMA_DATA_SIZE((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM);
    dma1.des.length = 0;        //For input buffer, this field is no use.
    dma1.buf = &link_buf[0][0];

    dma2.des.owner = 1;
    if (is_loop) {
        dma2.des.eof = 0;
        dma2.pnext = &dma1;
    } else {
        dma2.des.eof = 1;
        dma2.pnext = NULL;
    }
    dma2.des.size = SAR_DMA_DATA_SIZE((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM);
    dma2.des.length = 0;        //For input buffer, this field is no use.
    dma2.buf = &link_buf[1][0];

    REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_APB_SARADC_CLK_EN_M);
    REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_DMA_CLK_EN_M);
    REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
    REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_DMA_RST_M);
    REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST_M);
    uint32_t dma_pointer = (uint32_t)&dma1;
    SET_PERI_REG_BITS(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_ADDR, dma_pointer, 0);
    REG_SET_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_STOP);
    REG_CLR_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_START);
    REG_CLR_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_STOP);
    REG_SET_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_START);
    REG_SET_BIT(SPI_DMA_INT_ENA_REG(3), SPI_IN_SUC_EOF_INT_ENA);
    printf("reg addr 0x%08x 0x%08x \n", SPI_DMA_IN_LINK_REG(3), dma_pointer);
}

static void dma_linker_restart(void)
{
    REG_SET_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_STOP);
    REG_CLR_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_START);
    REG_CLR_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_STOP);
    REG_SET_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_START);
    adc_digi_reset();
}

/*******************************************/
/**           SPI DMA INIT CODE END        */
/*******************************************/

/**
 * TEST TOOLS
 * Note: internal pullup/pulldown is weak energy. if enabled WiFi, it should be need outside pullup/pulldown.
 */
#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

static void adc_fake_tie_middle(adc_unit_t adc)
{
    if (adc & ADC_UNIT_1) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
            TEST_ESP_OK(rtc_gpio_pullup_en(ADC_GET_IO_NUM(0, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_pulldown_en(ADC_GET_IO_NUM(0, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_set_direction(ADC_GET_IO_NUM(0, adc_list[i]), RTC_GPIO_MODE_INPUT_ONLY));
        }
    }
    if (adc & ADC_UNIT_2) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_2, adc_list[i]);
            TEST_ESP_OK(rtc_gpio_pullup_en(ADC_GET_IO_NUM(1, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_pulldown_en(ADC_GET_IO_NUM(1, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_set_direction(ADC_GET_IO_NUM(1, adc_list[i]), RTC_GPIO_MODE_INPUT_ONLY));
        }
    }
    vTaskDelay(10 / portTICK_RATE_MS); // To wait stable of IO.
}

static void adc_fake_tie_high(adc_unit_t adc)
{
    if (adc & ADC_UNIT_1) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
            TEST_ESP_OK(rtc_gpio_pullup_en(ADC_GET_IO_NUM(0, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_pulldown_dis(ADC_GET_IO_NUM(0, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_set_direction(ADC_GET_IO_NUM(0, adc_list[i]), RTC_GPIO_MODE_OUTPUT_ONLY));
            TEST_ESP_OK(rtc_gpio_set_level(ADC_GET_IO_NUM(0, adc_list[i]), 1));
        }
    }
    if (adc & ADC_UNIT_2) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_2, adc_list[i]);
            TEST_ESP_OK(rtc_gpio_pullup_en(ADC_GET_IO_NUM(1, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_pulldown_dis(ADC_GET_IO_NUM(1, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_set_direction(ADC_GET_IO_NUM(1, adc_list[i]), RTC_GPIO_MODE_OUTPUT_ONLY));
            TEST_ESP_OK(rtc_gpio_set_level(ADC_GET_IO_NUM(1, adc_list[i]), 1));
        }
    }
    vTaskDelay(10 / portTICK_RATE_MS); // To wait stable of IO.
}

static void adc_fake_tie_low(adc_unit_t adc)
{
    if (adc & ADC_UNIT_1) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
            TEST_ESP_OK(rtc_gpio_pullup_dis(ADC_GET_IO_NUM(0, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_pulldown_en(ADC_GET_IO_NUM(0, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_set_direction(ADC_GET_IO_NUM(0, adc_list[i]), RTC_GPIO_MODE_OUTPUT_ONLY));
            TEST_ESP_OK(rtc_gpio_set_level(ADC_GET_IO_NUM(0, adc_list[i]), 0));
        }
    }
    if (adc & ADC_UNIT_2) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_2, adc_list[i]);
            TEST_ESP_OK(rtc_gpio_pullup_dis(ADC_GET_IO_NUM(1, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_pulldown_en(ADC_GET_IO_NUM(1, adc_list[i])));
            TEST_ESP_OK(rtc_gpio_set_direction(ADC_GET_IO_NUM(1, adc_list[i]), RTC_GPIO_MODE_OUTPUT_ONLY));
            TEST_ESP_OK(rtc_gpio_set_level(ADC_GET_IO_NUM(1, adc_list[i]), 0));
        }
    }
    vTaskDelay(10 / portTICK_RATE_MS); // To wait stable of IO.
}

static void adc_io_normal(adc_unit_t adc)
{
    if (adc & ADC_UNIT_1) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
        }
    }
    if (adc & ADC_UNIT_2) {
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc_gpio_init(ADC_UNIT_2, adc_list[i]);
        }
    }
    vTaskDelay(10 / portTICK_RATE_MS); // To wait stable of IO.
}

#define DEBUG_CHECK_ENABLE  0
#define DEBUG_PRINT_ENABLE  1
#define DEBUG_CHECK_ERROR   10

static esp_err_t adc_dma_data_check(adc_unit_t adc, int ideal_level)
{
#if DEBUG_CHECK_ENABLE
    int unit_old = 1;
    int ch_cnt = 0;
#endif
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
                    int cur_ch = ((ch_cnt++ / 2) % TEST_ADC_CHANNEL);
                    TEST_ASSERT_EQUAL( data->type2.channel, adc_list[cur_ch] );
                }
                /*Check data channel unit*/
                if (ideal_level == 1) {         // high level
                    TEST_ASSERT_INT_WITHIN( DEBUG_CHECK_ERROR, 0x7FF, data->type2.data );
                } else if (ideal_level == 0) {  // low level
                    TEST_ASSERT_INT_WITHIN( DEBUG_CHECK_ERROR, 0, data->type2.data );
                } else if (ideal_level == 2) {  // middle level
                    TEST_ASSERT_INT_WITHIN( 300, 1100, data->type1.data );
                } else if (ideal_level == 3) {  // normal level
                } else {
                    // no check
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
                /*Check data channel */
                if (ideal_level == 1) {         // high level
                    TEST_ASSERT_INT_WITHIN( DEBUG_CHECK_ERROR, 0XFFF, data->type1.data );
                } else if (ideal_level == 0) {  // low level
                    TEST_ASSERT_INT_WITHIN( DEBUG_CHECK_ERROR, 0, data->type1.data );
                } else if (ideal_level == 2) {  // middle level
                    TEST_ASSERT_INT_WITHIN( 300, 2200, data->type1.data );
                } else if (ideal_level == 3) {  // normal level
                } else {
                    // no check
                }
                if (ideal_level >= 0) {
                    int cur_ch = ((ch_cnt++) % TEST_ADC_CHANNEL);
                    TEST_ASSERT_EQUAL( adc_list[cur_ch], data->type1.channel );
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

static esp_err_t adc_dma_data_multi_st_check(adc_unit_t adc)
{
    ESP_LOGI(TAG, "adc IO fake tie low, test ...");
    adc_fake_tie_low(adc);
    dma_linker_restart();
    TEST_ESP_OK( adc_digi_start() );
    while (REG_GET_BIT(SPI_DMA_INT_ST_REG(3), SPI_IN_SUC_EOF_INT_ST) == 0) {};
    REG_SET_BIT(SPI_DMA_INT_CLR_REG(3), SPI_IN_SUC_EOF_INT_CLR);
    TEST_ESP_OK( adc_digi_stop() );
    if ( adc_dma_data_check(adc, 0) != ESP_OK ) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "adc IO fake tie high, test ...");
    adc_fake_tie_high(adc);
    dma_linker_restart();
    TEST_ESP_OK( adc_digi_start() );
    while (REG_GET_BIT(SPI_DMA_INT_ST_REG(3), SPI_IN_SUC_EOF_INT_ST) == 0) {};
    REG_SET_BIT(SPI_DMA_INT_CLR_REG(3), SPI_IN_SUC_EOF_INT_CLR);
    TEST_ESP_OK( adc_digi_stop() );
    if ( adc_dma_data_check(adc, 1) != ESP_OK ) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "adc IO fake tie middle, test ...");
    adc_fake_tie_middle(adc);
    dma_linker_restart();
    TEST_ESP_OK( adc_digi_start() );
    while (REG_GET_BIT(SPI_DMA_INT_ST_REG(3), SPI_IN_SUC_EOF_INT_ST) == 0) {};
    REG_SET_BIT(SPI_DMA_INT_CLR_REG(3), SPI_IN_SUC_EOF_INT_CLR);
    TEST_ESP_OK( adc_digi_stop() );
    if ( adc_dma_data_check(adc, 2) != ESP_OK ) {
        return ESP_FAIL;
    }

    adc_io_normal(adc);

    return ESP_OK;
}

#include "soc/apb_saradc_struct.h"
/**
 * @brief Test the partten table setting. It's easy wrong.
 *
 * @param adc_n ADC unit.
 * @param in_partten_len The length of partten be set.
 * @param in_partten_len The channel number of the last message.
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
        .dig_clk.div_num = TEST_ADC_DIGI_CLK_DIV_DEFAULT,   // 80 MHz / 2 = 40 MHz
        .dig_clk.div_b = 1,
        .dig_clk.div_a = 1,
        .dma_eof_num = SAR_EOF_NUMBER((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM),
    };
    /* Config pattern table */
    adc_digi_pattern_table_t adc1_patt[TEST_ADC_CHANNEL] = {0};
    adc_digi_pattern_table_t adc2_patt[TEST_ADC_CHANNEL] = {0};
    if (adc & ADC_UNIT_1) {
        config.adc1_pattern_len = TEST_ADC_CHANNEL;
        config.adc1_pattern = adc1_patt;
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc1_patt[i].atten = TEST_ADC_ATTEN_DEFAULT;
            adc1_patt[i].channel = adc_list[i];
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
        }
    }
    if (adc & ADC_UNIT_2) {
        config.adc2_pattern_len = TEST_ADC_CHANNEL;
        config.adc2_pattern = adc2_patt;
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
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

    dma_linker_init(adc, false);
    TEST_ESP_OK( adc_check_patt_table(adc, TEST_ADC_CHANNEL, adc_list[TEST_ADC_CHANNEL - 1]) );

    ESP_LOGI(TAG, "adc IO normal, test ...");
    dma_linker_restart();
    TEST_ESP_OK( adc_digi_start() );
    while (REG_GET_BIT(SPI_DMA_INT_ST_REG(3), SPI_IN_SUC_EOF_INT_ST) == 0) {};
    REG_SET_BIT(SPI_DMA_INT_CLR_REG(3), SPI_IN_SUC_EOF_INT_CLR);
    TEST_ESP_OK( adc_digi_stop() );
    if ( adc_dma_data_check(adc, -1) != ESP_OK ) {
        return ESP_FAIL;
    }

    adc_dma_data_multi_st_check(adc);

    TEST_ESP_OK( adc_digi_deinit() );
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
 */
#define SCOPE_DEBUG_TYPE            0
#define SCOPE_DEBUG_CHANNEL_MAX    (10)
#define SCOPE_DEBUG_ENABLE         (0)
#define SCOPE_UART_BUADRATE        (256000)
#define SCOPE_DEBUG_FREQ_MS        (50)
#define SCOPE_OUTPUT_UART          (0)

int test_adc_dig_scope_debug_unit(adc_unit_t adc)
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
        .dig_clk.div_num = TEST_ADC_DIGI_CLK_DIV_DEFAULT,   // 80 MHz / 80 = 1 MHz
        .dig_clk.div_b = 1,
        .dig_clk.div_a = 1,
        .dma_eof_num = SAR_EOF_NUMBER((adc > 2) ? 2 : 1, SAR_SIMPLE_NUM),
    };
    /* Config pattern table */
    adc_digi_pattern_table_t adc1_patt[TEST_ADC_CHANNEL] = {0};
    adc_digi_pattern_table_t adc2_patt[TEST_ADC_CHANNEL] = {0};
    if (adc & ADC_UNIT_1) {
        config.adc1_pattern_len = TEST_ADC_CHANNEL;
        config.adc1_pattern = adc1_patt;
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
            adc1_patt[i].atten = TEST_ADC_ATTEN_DEFAULT;
            adc1_patt[i].channel = adc_list[i];
            adc_gpio_init(ADC_UNIT_1, adc_list[i]);
        }
    }
    if (adc & ADC_UNIT_2) {
        config.adc2_pattern_len = TEST_ADC_CHANNEL;
        config.adc2_pattern = adc2_patt;
        for (int i = 0; i < TEST_ADC_CHANNEL; i++) {
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

    dma_linker_init(adc, false);
    TEST_ESP_OK( adc_check_patt_table(adc, TEST_ADC_CHANNEL, adc_list[TEST_ADC_CHANNEL - 1]) );

    ESP_LOGI(TAG, "adc IO fake tie middle, test ...");
    adc_fake_tie_middle(adc);

    TEST_ESP_OK( adc_digi_start() );

    return 0;
}

static void scope_output(int adc_num, int channel, int data)
{
    static float scope_temp[TEST_ADC_CHANNEL] = {0};  // max scope channel is 10.
    static int scope_cnt = 0;
    /** can replace by uart log.*/
#if SCOPE_OUTPUT_UART
    static int i = 0;
    if (i++ % 8 == 0) {
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
    /* adc Read */
    if (adc_num == 0) {
        scope_temp[channel] = data;
        if (++scope_cnt >= TEST_ADC_CHANNEL) {
            scope_cnt = 0;
            test_tp_print_to_scope(scope_temp, TEST_ADC_CHANNEL);
            vTaskDelay(SCOPE_DEBUG_FREQ_MS / portTICK_RATE_MS);
            for (int i=0; i<TEST_ADC_CHANNEL; i++) {
                scope_temp[i] = 0;
            }
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
    test_tp_scope_debug_init(0, -1, -1, SCOPE_UART_BUADRATE);

    adc_unit_t adc = ADC_UNIT_1;
    test_adc_dig_scope_debug_unit(adc);

    while (1) {
        REG_SET_BIT(SPI_DMA_INT_CLR_REG(3), SPI_IN_SUC_EOF_INT_CLR);
        dma_linker_restart();
        TEST_ESP_OK( adc_digi_start() );
        while (REG_GET_BIT(SPI_DMA_INT_ST_REG(3), SPI_IN_SUC_EOF_INT_ST) == 0) {};
        TEST_ESP_OK( adc_digi_stop() );

        for (int cnt = 0; cnt < 2; cnt++) {
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

#endif // !DISABLED_FOR_TARGETS(ESP8266, ESP32)