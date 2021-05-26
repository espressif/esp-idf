/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Tests for the adc device driver on ESP32 only
*/
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32

#include "esp_system.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "esp_rom_sys.h"
#include "driver/dac.h"

/*
 * ADC DMA testcase
 */
#include "driver/i2s.h"
#include "test/test_common_adc.h"

//i2s number
#define EXAMPLE_I2S_NUM           (0)
//i2s sample rate
#define EXAMPLE_I2S_SAMPLE_RATE   (150000)
//i2s data bits
#define EXAMPLE_I2S_SAMPLE_BITS   (16)
//enable display buffer for debug
#define EXAMPLE_I2S_BUF_DEBUG     (0)
//I2S read buffer length
#define EXAMPLE_I2S_READ_LEN      (16 * 1024)
//I2S data format, ADC-I2S only support mono.
#define EXAMPLE_I2S_FORMAT        I2S_CHANNEL_FMT_ONLY_RIGHT
//I2S built-in ADC unit
#define I2S_ADC_UNIT              ADC_UNIT_1
//I2S built-in ADC channel
#define I2S_ADC_CHANNEL           ADC1_CHANNEL_4

/**
 * @brief I2S ADC/DAC mode init.
 */
static void example_i2s_init(void)
{
    int i2s_num = EXAMPLE_I2S_NUM;
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN,
        .sample_rate =  EXAMPLE_I2S_SAMPLE_RATE,
        .bits_per_sample = EXAMPLE_I2S_SAMPLE_BITS,
        .channel_format = EXAMPLE_I2S_FORMAT,
        .intr_alloc_flags = 0,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = 0,
    };

    //install and start i2s driver
    TEST_ESP_OK( i2s_driver_install(i2s_num, &i2s_config, 0, NULL) );
    //init ADC pad
    TEST_ESP_OK( i2s_set_adc_mode(I2S_ADC_UNIT, I2S_ADC_CHANNEL) );
}

static void example_i2s_deinit(void)
{
    TEST_ESP_OK( i2s_driver_uninstall(EXAMPLE_I2S_NUM) );
}

/**
 * @brief debug buffer data
 */
static void example_disp_buf(uint8_t *buf, int length)
{
    printf("\n======");
    for (int i = 0; i < length; i += 2) {
        uint16_t data = ((uint16_t)buf[i+1] << 8) | (uint16_t)buf[i];
        adc_digi_output_data_t *p = (adc_digi_output_data_t *)&data;
        if ((i) % 16 == 0) printf("\n");
        printf("[%d_%d] ", p->type1.channel, p->type1.data);
    }
    printf("\n======\n");
}

static esp_err_t adc_dma_data_check(uint8_t *buf, int length, int ideal_level)
{
    for (int i = 0; i < length; i += 2) {
        uint16_t data = ((uint16_t)buf[i+1] << 8) | (uint16_t)buf[i];
        adc_digi_output_data_t *p = (adc_digi_output_data_t *)&data;
        if (p->type1.channel != I2S_ADC_CHANNEL) {
            TEST_FAIL_MESSAGE("I2S-DMA data channel error!");
        }
        if (ideal_level == 1) {         // high level 3.3v
            TEST_ASSERT_EQUAL( 0xFFF, p->type1.data );
        } else if (ideal_level == 0) {  // low level 0v
            TEST_ASSERT_LESS_THAN( 10, p->type1.data );
        } else if (ideal_level == 2) {  // middle level 1.4v
            TEST_ASSERT_INT_WITHIN( 128, 1586, p->type1.data );
        } else if (ideal_level == 3) {  // normal level
        } else { // no check
        }
    }
    return ESP_OK;
}

static void adc_dma_read(uint8_t *buf, int length)
{
    size_t bytes_read = 0;
    int flash_wr_size = 0;

    vTaskDelay(pdTICKS_TO_MS(100));
    while (flash_wr_size < length) {
        //read data from I2S bus, in this case, from ADC.
        TEST_ESP_OK( i2s_read(EXAMPLE_I2S_NUM, (void *) buf + flash_wr_size, length - flash_wr_size, &bytes_read, portMAX_DELAY) );
        flash_wr_size += bytes_read;
        example_disp_buf((uint8_t *) buf, 128);
    }
}

TEST_CASE("ADC DMA read", "[adc dma]")
{
    int i2s_read_len = EXAMPLE_I2S_READ_LEN;
    char *i2s_read_buff = (char *) calloc(i2s_read_len, sizeof(char));

    example_i2s_init();
    TEST_ESP_OK( i2s_adc_enable(EXAMPLE_I2S_NUM) );

    adc_fake_tie_low(I2S_ADC_UNIT, I2S_ADC_CHANNEL);
    adc_dma_read((uint8_t *)i2s_read_buff, i2s_read_len);
    adc_dma_data_check((uint8_t *)i2s_read_buff, i2s_read_len, 0);

    adc_fake_tie_middle(I2S_ADC_UNIT, I2S_ADC_CHANNEL);
    adc_dma_read((uint8_t *)i2s_read_buff, i2s_read_len);
    adc_dma_data_check((uint8_t *)i2s_read_buff, i2s_read_len, 2);

    adc_fake_tie_high(I2S_ADC_UNIT, I2S_ADC_CHANNEL);
    adc_dma_read((uint8_t *)i2s_read_buff, i2s_read_len);
    adc_dma_data_check((uint8_t *)i2s_read_buff, i2s_read_len, 1);

    adc_io_normal(I2S_ADC_UNIT, I2S_ADC_CHANNEL);

    TEST_ESP_OK( i2s_adc_disable(EXAMPLE_I2S_NUM) );
    if (i2s_read_buff) {
        free(i2s_read_buff);
        i2s_read_buff = NULL;
    }

    example_i2s_deinit();
}

#endif // CONFIG_IDF_TARGET_ESP32
