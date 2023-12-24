/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Tests for the dac device driver on ESP32 only
 Hardware connection:
 - ESP32: GPIO25  <--->  GPIO26
*/
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32

#include "esp_system.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "test_dac_audio_file.h"
#include "driver/i2s.h"
#include "driver/dac.h"

/*
 * DAC DMA config.
 */

//enable record sound and save in flash
#define RECORD_IN_FLASH_EN        (1)
//enable replay recorded sound in flash
#define REPLAY_FROM_FLASH_EN      (1)

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

/**
 * @brief I2S ADC/DAC mode init.
 */
static void example_i2s_init(void)
{
    int i2s_num = EXAMPLE_I2S_NUM;
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
        .sample_rate =  EXAMPLE_I2S_SAMPLE_RATE,
        .bits_per_sample = EXAMPLE_I2S_SAMPLE_BITS,
        .channel_format = EXAMPLE_I2S_FORMAT,
        .intr_alloc_flags = 0,
        .dma_desc_num = 2,
        .dma_frame_num = 1024,
        .use_apll = 0,
    };
    //install and start i2s driver
    TEST_ESP_OK(i2s_driver_install(i2s_num, &i2s_config, 0, NULL));
    //init DAC pad
    TEST_ESP_OK(i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN));
}

static void example_i2s_deinit(void)
{
    TEST_ESP_OK(i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE));
    TEST_ESP_OK(i2s_driver_uninstall(EXAMPLE_I2S_NUM));
}

/**
 * @brief Set i2s clock for example audio file
 */
static void example_set_file_play_mode(void)
{
    TEST_ESP_OK(i2s_set_clk(EXAMPLE_I2S_NUM, 16000, EXAMPLE_I2S_SAMPLE_BITS, 1));
}

/**
 * @brief Scale data to 16bit/32bit for I2S DMA output.
 *        DAC can only output 8bit data value.
 *        I2S DMA will still send 16 bit or 32bit data, the highest 8bit contains DAC data.
 */
static int example_i2s_dac_data_scale(uint8_t *d_buff, uint8_t *s_buff, uint32_t len)
{
    uint32_t j = 0;
#if (EXAMPLE_I2S_SAMPLE_BITS == 16)
    for (int i = 0; i < len; i++) {
        d_buff[j++] = 0;
        d_buff[j++] = s_buff[i];
    }
    return (len * 2);
#else
    for (int i = 0; i < len; i++) {
        d_buff[j++] = 0;
        d_buff[j++] = 0;
        d_buff[j++] = 0;
        d_buff[j++] = s_buff[i];
    }
    return (len * 4);
#endif
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

/**
 * @brief Reset i2s clock and mode
 */
static void example_reset_play_mode(void)
{
    TEST_ESP_OK(i2s_set_clk(EXAMPLE_I2S_NUM, EXAMPLE_I2S_SAMPLE_RATE, EXAMPLE_I2S_SAMPLE_BITS, EXAMPLE_I2S_CHANNEL_NUM));
}

TEST_CASE("DAC_DMA_output", "[dac]")
{
    size_t bytes_written;
    int i2s_read_len = EXAMPLE_I2S_READ_LEN;
    uint8_t *i2s_write_buff = (uint8_t *) calloc(i2s_read_len, sizeof(char));
    int offset = 0;
    int tot_size = sizeof(audio_table);
    printf("Playing file example: \n");

    example_i2s_init();
    example_set_file_play_mode();

    while (offset < tot_size) {
        int play_len = ((tot_size - offset) > (4 * 1024)) ? (4 * 1024) : (tot_size - offset);
        int i2s_wr_len = example_i2s_dac_data_scale(i2s_write_buff, (uint8_t *)(audio_table + offset), play_len);
        i2s_write(EXAMPLE_I2S_NUM, i2s_write_buff, i2s_wr_len, &bytes_written, portMAX_DELAY);
        offset += play_len;
        example_disp_buf((uint8_t *) i2s_write_buff, 32);
    }

    example_reset_play_mode();
    free(i2s_write_buff);

    example_i2s_deinit();
}

#endif // CONFIG_IDF_TARGET_ESP32
