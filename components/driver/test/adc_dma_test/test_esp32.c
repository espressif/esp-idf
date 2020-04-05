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

#if !DISABLED_FOR_TARGETS(ESP8266, ESP32S2) // This testcase for ESP32

/*
 * ADC DMA testcase
 */
#include "driver/i2s.h"

//i2s number
#define EXAMPLE_I2S_NUM           (0)
//i2s sample rate
#define EXAMPLE_I2S_SAMPLE_RATE   (16000)
//i2s data bits
#define EXAMPLE_I2S_SAMPLE_BITS   (16)
//enable display buffer for debug
#define EXAMPLE_I2S_BUF_DEBUG     (0)
//I2S read buffer length
#define EXAMPLE_I2S_READ_LEN      (16 * 1024)
//I2S data format
#define EXAMPLE_I2S_FORMAT        (I2S_CHANNEL_FMT_RIGHT_LEFT)
//I2S channel number
#define EXAMPLE_I2S_CHANNEL_NUM   ((EXAMPLE_I2S_FORMAT < I2S_CHANNEL_FMT_ONLY_RIGHT) ? (2) : (1))
//I2S built-in ADC unit
#define I2S_ADC_UNIT              ADC_UNIT_1
//I2S built-in ADC channel
//#define I2S_ADC_CHANNEL           //adc1_ch[0]

//flash record size, for recording 5 seconds' data
#define FLASH_RECORD_SIZE         (EXAMPLE_I2S_CHANNEL_NUM * EXAMPLE_I2S_SAMPLE_RATE * EXAMPLE_I2S_SAMPLE_BITS / 8 * 5)
#define FLASH_ERASE_SIZE          (FLASH_RECORD_SIZE % FLASH_SECTOR_SIZE == 0) ? FLASH_RECORD_SIZE : FLASH_RECORD_SIZE + (FLASH_SECTOR_SIZE - FLASH_RECORD_SIZE % FLASH_SECTOR_SIZE)
//sector size of flash
#define FLASH_SECTOR_SIZE         (0x1000)

#define ADC1_TEST_CHANNEL_NUM   3
#define ADC2_TEST_CHANNEL_NUM   3

static const int adc1_ch[ADC1_TEST_CHANNEL_NUM] = {
    ADC1_CHANNEL_2,
    ADC1_CHANNEL_3,
    ADC1_CHANNEL_4,
};

/**
 * @brief I2S ADC/DAC mode init.
 */
static void example_i2s_init(void)
{
    int i2s_num = EXAMPLE_I2S_NUM;
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN,
        .sample_rate =  EXAMPLE_I2S_SAMPLE_RATE,
        .bits_per_sample = EXAMPLE_I2S_SAMPLE_BITS,
        .communication_format = I2S_COMM_FORMAT_PCM,
        .channel_format = EXAMPLE_I2S_FORMAT,
        .intr_alloc_flags = 0,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = 1,
    };
    //install and start i2s driver
    TEST_ESP_OK( i2s_driver_install(i2s_num, &i2s_config, 0, NULL) );
    //init ADC pad
    TEST_ESP_OK( i2s_set_adc_mode(I2S_ADC_UNIT, adc1_ch[0]) );
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
    printf("======\n");
    for (int i = 0; i < length; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("======\n");
}

TEST_CASE("ADC DMA read", "[adc dma]")
{
    example_i2s_init();

    int i2s_read_len = EXAMPLE_I2S_READ_LEN;
    int flash_wr_size = 0;
    size_t bytes_read;

    char *i2s_read_buff = (char *) calloc(i2s_read_len, sizeof(char));
    TEST_ESP_OK( i2s_adc_enable(EXAMPLE_I2S_NUM) );
    while (flash_wr_size < FLASH_RECORD_SIZE) {
        //read data from I2S bus, in this case, from ADC.
        TEST_ESP_OK( i2s_read(EXAMPLE_I2S_NUM, (void *) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY) );
        example_disp_buf((uint8_t *) i2s_read_buff, 64);
        //save original data from I2S(ADC) into flash.
        flash_wr_size += i2s_read_len;
        ets_printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);
    }
    TEST_ESP_OK( i2s_adc_disable(EXAMPLE_I2S_NUM) );
    if (i2s_read_buff) {
        free(i2s_read_buff);
        i2s_read_buff = NULL;
    }

    example_i2s_deinit();
}

#endif // !DISABLED_FOR_TARGETS(ESP8266, ESP32S2)