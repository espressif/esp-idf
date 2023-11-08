/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * I2S test environment UT_T1_I2S:
 * We use internal signals instead of external wiring, but please keep the following IO connections, or connect nothing to prevent the signal from being disturbed.
 * connect GPIO15 and GPIO19, GPIO25(ESP32)/GPIO17(ESP32-S2) and GPIO26, GPIO21 and GPIO22(ESP32)/GPIO20(ESP32-S2)
 * Please do not connect GPIO32(ESP32) any pull-up resistors externally, it will be used to test i2s adc function.
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2s.h"
#include "hal/i2s_hal.h"
#include "soc/i2s_periph.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "unity.h"
#include "math.h"
#include "esp_rom_gpio.h"
#if SOC_PCNT_SUPPORTED
#include "driver/pulse_cnt.h"
#include "soc/pcnt_periph.h"
#endif
#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#include "../../test_inc/test_i2s.h"

#define PERCENT_DIFF 0.0001

#define I2S_TEST_MODE_SLAVE_TO_MASTER 0
#define I2S_TEST_MODE_MASTER_TO_SLAVE 1
#define I2S_TEST_MODE_LOOPBACK        2

// mode: 0, master rx, slave tx. mode: 1, master tx, slave rx. mode: 2, master tx rx loop-back
// Since ESP32-S2 has only one I2S, only loop back test can be tested.
static void i2s_test_io_config(int mode)
{
    // Connect internal signals using IO matrix.
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[MASTER_BCK_IO], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[MASTER_WS_IO], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[DATA_OUT_IO], PIN_FUNC_GPIO);

    gpio_set_direction(MASTER_BCK_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(MASTER_WS_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(DATA_OUT_IO, GPIO_MODE_INPUT_OUTPUT);

    switch (mode) {
#if SOC_I2S_NUM > 1
    case I2S_TEST_MODE_SLAVE_TO_MASTER: {
        esp_rom_gpio_connect_out_signal(MASTER_BCK_IO, i2s_periph_signal[0].m_rx_bck_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_BCK_IO, i2s_periph_signal[1].s_tx_bck_sig, 0);

        esp_rom_gpio_connect_out_signal(MASTER_WS_IO, i2s_periph_signal[0].m_rx_ws_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_WS_IO, i2s_periph_signal[1].s_tx_ws_sig, 0);

        esp_rom_gpio_connect_out_signal(DATA_OUT_IO, i2s_periph_signal[1].data_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(DATA_OUT_IO, i2s_periph_signal[0].data_in_sig, 0);
    }
    break;

    case I2S_TEST_MODE_MASTER_TO_SLAVE: {
        esp_rom_gpio_connect_out_signal(MASTER_BCK_IO, i2s_periph_signal[0].m_tx_bck_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_BCK_IO, i2s_periph_signal[1].s_rx_bck_sig, 0);

        esp_rom_gpio_connect_out_signal(MASTER_WS_IO, i2s_periph_signal[0].m_tx_ws_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_WS_IO, i2s_periph_signal[1].s_rx_ws_sig, 0);

        esp_rom_gpio_connect_out_signal(DATA_OUT_IO, i2s_periph_signal[0].data_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(DATA_OUT_IO, i2s_periph_signal[1].data_in_sig, 0);
    }
    break;
#endif
    case I2S_TEST_MODE_LOOPBACK: {
        esp_rom_gpio_connect_out_signal(DATA_OUT_IO, i2s_periph_signal[0].data_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(DATA_OUT_IO, i2s_periph_signal[0].data_in_sig, 0);
    }
    break;

    default: {
        TEST_FAIL_MESSAGE("error: mode not supported");
    }
    break;
    }
}

#if SOC_I2S_SUPPORTS_ADC
#define ADC1_CHANNEL_4_IO 32
/* Only ESP32 need I2S adc/dac test */
TEST_CASE("I2S_adc_test", "[i2s_legacy]")
{
    // init I2S ADC
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN,
        .sample_rate =  SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .intr_alloc_flags = 0,
        .dma_desc_num = 2,
        .dma_frame_num = 1024,
        .use_apll = 0,
    };
    // install and start I2S driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    // init ADC pad
    i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_4);
    // enable adc sampling, ADC_WIDTH_BIT_12, ADC_ATTEN_DB_12 hard-coded in adc_i2s_mode_init
    i2s_adc_enable(I2S_NUM_0);
    // init read buffer
    uint16_t *i2sReadBuffer = (uint16_t *)calloc(1024, sizeof(uint16_t));
    size_t bytesRead;

    for (int loop = 0; loop < 10; loop++) {
        for (int level = 0; level <= 1; level++) {
            if (level == 0) {
                gpio_set_pull_mode(ADC1_CHANNEL_4_IO, GPIO_PULLDOWN_ONLY);
            } else {
                gpio_set_pull_mode(ADC1_CHANNEL_4_IO, GPIO_PULLUP_ONLY);
            }
            vTaskDelay(pdMS_TO_TICKS(200));
            // read data from adc, will block until buffer is full
            i2s_read(I2S_NUM_0, (void *)i2sReadBuffer, 1024 * sizeof(uint16_t), &bytesRead, portMAX_DELAY);

            // calc average
            int64_t adcSumValue = 0;
            for (size_t i = 0; i < 1024; i++) {
                adcSumValue += i2sReadBuffer[i] & 0xfff;
            }
            int adcAvgValue = adcSumValue / 1024;
            printf("adc average val: %d\n", adcAvgValue);

            if (level == 0) {
                if (adcAvgValue > 100) {
                    i2s_adc_disable(I2S_NUM_0);
                    free(i2sReadBuffer);
                    i2s_driver_uninstall(I2S_NUM_0);
                    TEST_ASSERT_LESS_THAN(100, adcAvgValue);
                }
            } else {
                if (adcAvgValue < 4000) {
                    i2s_adc_disable(I2S_NUM_0);
                    free(i2sReadBuffer);
                    i2s_driver_uninstall(I2S_NUM_0);
                    TEST_ASSERT_GREATER_THAN(4000, adcAvgValue);
                }
            }
        }
    }

    i2s_adc_disable(I2S_NUM_0);
    free(i2sReadBuffer);
    i2s_driver_uninstall(I2S_NUM_0);
}
#endif

/**
 * i2s initialize test
 * 1. i2s_driver_install
 * 2. i2s_set_pin
 */
TEST_CASE("I2S_basic_driver_installation_uninstallation_and_settings_test", "[i2s_legacy]")
{
    // dac, adc  i2s
    i2s_config_t  i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 60,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };

    // normal  i2s
    i2s_pin_config_t pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };
    QueueHandle_t evt_que;
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 16, &evt_que));
    TEST_ASSERT(evt_que);
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &pin_config));
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));

    //error param test
    TEST_ASSERT(i2s_driver_install(SOC_I2S_NUM, &i2s_config, 0, NULL) == ESP_ERR_INVALID_ARG);
    TEST_ASSERT(i2s_driver_install(I2S_NUM_0, NULL, 0, NULL) == ESP_ERR_INVALID_ARG);
    i2s_config.dma_desc_num = 1;
    TEST_ASSERT(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) == ESP_ERR_INVALID_ARG);
    i2s_config.dma_desc_num = 129;
    TEST_ASSERT(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) == ESP_ERR_INVALID_ARG);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, i2s_driver_uninstall(I2S_NUM_0));
}

static bool whether_contains_exapected_data(uint16_t *src, uint32_t src_len, uint32_t src_step, uint32_t start_val, uint32_t val_step)
{
    uint32_t val = start_val;
    uint32_t index_step = 1;
    for (int i = 0; val < 100 && i < src_len; i += index_step) {
        if (src[i] == val) {
            if (val == start_val && i < src_len - 8) {
                printf("start index: %d ---> \n%d %d %d %d %d %d %d %d\n", i,
                        src[i], src[i+1], src[i+2], src[i+3],
                        src[i+4], src[i+5], src[i+6], src[i+7]);
            }
            index_step = src_step;
            val += val_step;
        } else {
            index_step = 1;
            val = start_val;
        }
    }

    return val >= 100;
}

/**
 * @brief Test mono and stereo mode of I2S by loopback
 * @note  Only rx channel distinguish left mono and right mono, tx channel does not
 * @note  1. Check switch mono/stereo by 'i2s_set_clk'
 *        2. Check rx right mono and left mono (requiring tx works in stereo mode)
 *        3. Check tx mono (requiring rx works in stereo mode)
 */
TEST_CASE("I2S_legacy_mono_stereo_loopback_test", "[i2s_legacy]")
{
#define WRITE_BUF_LEN  2000
#define READ_BUF_LEN   4000
#define RETEY_TIMES    3
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };
    i2s_pin_config_t master_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = DATA_IN_IO
    };
    /* Install I2S in duplex mode */
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_stop(I2S_NUM_0));
    /* Config TX as stereo channel directly, because legacy driver can't support config tx&rx separately */
#if SOC_I2S_HW_VERSION_1
    i2s_ll_tx_select_std_slot(&I2S0, I2S_STD_SLOT_BOTH, false);
#else
    i2s_ll_tx_select_std_slot(&I2S0, I2S_STD_SLOT_BOTH);
#endif
    i2s_ll_tx_enable_mono_mode(&I2S0, false);

    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_LOOPBACK);

    TEST_ESP_OK(i2s_start(I2S_NUM_0));

    uint16_t *w_buf = calloc(1, WRITE_BUF_LEN);
    uint16_t *r_buf = calloc(1, READ_BUF_LEN);
    size_t w_bytes = 0;
    size_t r_bytes = 0;
    uint32_t retry = 0;
    bool is_failed = false;
    for (int n = 0; n < WRITE_BUF_LEN / 2; n++) {
        w_buf[n] = n%100;
    }
    /* rx right mono test
     * tx format: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ...
     * rx receive: 0x01[R] 0x03[R] ... */
    TEST_ESP_OK(i2s_write(I2S_NUM_0, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_read(I2S_NUM_0, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
    #if CONFIG_IDF_TARGET_ESP32
        /* The data of tx/rx channels are flipped on ESP32 */
        for (int n = 0; n < READ_BUF_LEN / 2; n += 2) {
            int16_t temp = r_buf[n];
            r_buf[n] = r_buf[n+1];
            r_buf[n+1] = temp;
        }
    #endif
        /* Expected: 1 3 5 7 9 ... 97 99 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 1, 1, 2)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("rx right mono test failed\n");
        is_failed = true;
        goto err;
    }
    printf("rx right mono test passed\n");

    /* tx/rx stereo test
     * tx format: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ...
     * rx receive: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ... */
    TEST_ESP_OK(i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, SAMPLE_BITS, I2S_CHANNEL_STEREO));
    TEST_ESP_OK(i2s_write(I2S_NUM_0, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_read(I2S_NUM_0, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
        /* Expected: 1 2 3 4 ... 98 99 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 1, 1, 1)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("tx/rx stereo test failed\n");
        is_failed = true;
        goto err;
    }
    printf("tx/rx stereo test passed\n");

    /* tx mono rx right mono test
     * tx format: 0x01[L] 0x01[R] 0x02[L] 0x02[R] ...
     * rx receive: 0x01[R] 0x02[R] ... */
    TEST_ESP_OK(i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_MONO));
    TEST_ESP_OK(i2s_write(I2S_NUM_0, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_read(I2S_NUM_0, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
        /* Expected: 1 2 3 4 ... 98 99 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 1, 1, 1)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("tx/rx mono test failed\n");
        is_failed = true;
        goto err;
    }
    printf("tx/rx mono test passed\n");

    /* Reinstalling I2S to test rx left mono */
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
    master_i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_stop(I2S_NUM_0));
#if SOC_I2S_HW_VERSION_1
    i2s_ll_tx_select_std_slot(&I2S0, I2S_STD_SLOT_BOTH, false);
#else
    i2s_ll_tx_select_std_slot(&I2S0, I2S_STD_SLOT_BOTH);
#endif
    i2s_ll_tx_enable_mono_mode(&I2S0, false);

    TEST_ESP_OK(i2s_start(I2S_NUM_0));

    /* rx left mono test
     * tx format: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ...
     * rx receive: 0x00[R] 0x02[R] ... */
    TEST_ESP_OK(i2s_write(I2S_NUM_0, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_read(I2S_NUM_0, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
    #if CONFIG_IDF_TARGET_ESP32
        /* The data of tx/rx channels are flipped on ESP32 */
        for (int n = 0; n < READ_BUF_LEN / 2; n += 2) {
            int16_t temp = r_buf[n];
            r_buf[n] = r_buf[n+1];
            r_buf[n+1] = temp;
        }
    #endif
        /* Expected: 2 4 6 8 10 ... 96 98 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 1, 2, 2)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("rx left mono test failed\n");
        is_failed = true;
        goto err;
    }
    printf("rx left mono test passed\n");

err:
    if (is_failed) {
        for (int i = 0; i < READ_BUF_LEN / 2; i++) {
            printf("%x ", r_buf[i]);
        }
        printf("\n");
    }
    free(w_buf);
    free(r_buf);
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
    TEST_ASSERT_FALSE(is_failed);
}

#if SOC_I2S_SUPPORTS_TDM
TEST_CASE("I2S_TDM_loopback_test_with_master_tx_and_rx", "[i2s_legacy]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_MULTIPLE,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .total_chan = 4,
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1 | I2S_TDM_ACTIVE_CH2 | I2S_TDM_ACTIVE_CH3,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    };
    i2s_pin_config_t master_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = DATA_IN_IO
    };
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_LOOPBACK);
    printf("\r\nheap size: %"PRIu32"\n", esp_get_free_heap_size());

    uint8_t *data_wr = (uint8_t *)malloc(sizeof(uint8_t) * 400);
    size_t i2s_bytes_write = 0;
    size_t bytes_read = 0;
    int length = 0;
    uint8_t *i2s_read_buff = (uint8_t *)malloc(sizeof(uint8_t) * 10000);

    for (int i = 0; i < 100; i++) {
        data_wr[i] = i + 1;
    }
    int flag = 0; // break loop flag
    int end_position = 0;
    // write data to slave
    i2s_write(I2S_NUM_0, data_wr, sizeof(uint8_t) * 400, &i2s_bytes_write, 1000 / portTICK_PERIOD_MS);
    while (!flag) {
        if (length >= 10000 - 500) {
            break;
        }
        i2s_read(I2S_NUM_0, i2s_read_buff + length, sizeof(uint8_t) * 500, &bytes_read, 1000 / portMAX_DELAY);
        if (bytes_read > 0) {
            for (int i = length; i < length + bytes_read; i++) {
                if (i2s_read_buff[i] == 100) {
                    flag = 1;
                    end_position = i;
                    break;
                }
            }
        }
        length = length + bytes_read;
    }
    // test the read data right or not
    for (int i = end_position - 99; i <= end_position; i++) {
        TEST_ASSERT_EQUAL_UINT8((i - end_position + 100), *(i2s_read_buff + i));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
}
#endif

#if SOC_I2S_NUM > 1
/* ESP32S2 and ESP32C3 has only single I2S port and hence following test cases are not applicable */
TEST_CASE("I2S_write_and_read_test_with_master_tx_and_slave_rx", "[i2s_legacy]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };
    i2s_pin_config_t master_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_MASTER_TO_SLAVE);
    printf("\r\nheap size: %"PRIu32"\n", esp_get_free_heap_size());

    i2s_config_t slave_i2s_config = {
        .mode = I2S_MODE_SLAVE | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };
    i2s_pin_config_t slave_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = SLAVE_BCK_IO,
        .ws_io_num = SLAVE_WS_IO,
        .data_out_num = -1,
        .data_in_num = DATA_IN_IO,
    };
    // slave driver installed and receive data
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_1, &slave_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_1, &slave_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_MASTER_TO_SLAVE);
    printf("\r\nheap size: %"PRIu32"\n", esp_get_free_heap_size());

    uint8_t *data_wr = (uint8_t *)malloc(sizeof(uint8_t) * 400);
    size_t i2s_bytes_write = 0;
    size_t bytes_read = 0;
    int length = 0;
    uint8_t *i2s_read_buff = (uint8_t *)malloc(sizeof(uint8_t) * 10000);

    for (int i = 0; i < 100; i++) {
        data_wr[i] = i + 1;
    }
    int flag = 0; // break loop flag
    int end_position = 0;
    // write data to slave
    i2s_write(I2S_NUM_0, data_wr, sizeof(uint8_t) * 400, &i2s_bytes_write, 1000 / portTICK_PERIOD_MS);
    printf("write data size: %d\n", i2s_bytes_write);
    while (!flag) {
        i2s_read(I2S_NUM_1, i2s_read_buff + length, sizeof(uint8_t) * 500, &bytes_read, 1000 / portTICK_PERIOD_MS);
        if (bytes_read > 0) {
            printf("read data size: %d\n", bytes_read);
            for (int i = length; i < length + bytes_read; i++) {
                if (i2s_read_buff[i] == 100) {
                    flag = 1;
                    end_position = i;
                    break;
                }
            }
        }
        length = length + bytes_read;
    }
    // test the readed data right or not
    for (int i = end_position - 99; i <= end_position; i++) {
        TEST_ASSERT_EQUAL_UINT8((i - end_position + 100), *(i2s_read_buff + i));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_1);
}

TEST_CASE("I2S_write_and_read_test_master_rx_and_slave_tx", "[i2s_legacy]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 1,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };
    i2s_pin_config_t master_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = -1,
        .data_in_num = DATA_IN_IO,
    };
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_SLAVE_TO_MASTER);
    printf("\r\nheap size: %"PRIu32"\n", esp_get_free_heap_size());

    i2s_config_t slave_i2s_config = {
        .mode = I2S_MODE_SLAVE | I2S_MODE_TX,                                  // Only RX
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           //2-channels
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 1,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };
    i2s_pin_config_t slave_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = SLAVE_BCK_IO,
        .ws_io_num = SLAVE_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };
    // slave driver installed and receive data
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_1, &slave_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_1, &slave_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_SLAVE_TO_MASTER);
    printf("\r\nheap size: %"PRIu32"\n", esp_get_free_heap_size());

    uint8_t *data_wr = (uint8_t *)malloc(sizeof(uint8_t) * 400);
    size_t i2s_bytes_write = 0;
    size_t bytes_read = 0;
    int length = 0;
    uint8_t *i2s_read_buff = (uint8_t *)malloc(sizeof(uint8_t) * 10000);

    for (int i = 0; i < 100; i++) {
        data_wr[i] = i + 1;
    }
    // slave write data to master
    i2s_write(I2S_NUM_1, data_wr, sizeof(uint8_t) * 400, &i2s_bytes_write, 1000 / portTICK_PERIOD_MS);
    printf("write data size: %d\n", i2s_bytes_write);
    int flag = 0; // break loop flag
    volatile int end_position = 0;
    // write data to slave
    while (!flag) {
        TEST_ESP_OK(i2s_read(I2S_NUM_0, i2s_read_buff + length, 10000 - length, &bytes_read, 1000 / portTICK_PERIOD_MS));
        if (bytes_read > 0) {
            printf("read data size: %d\n", bytes_read);
            for (int i = length; i < length + bytes_read; i++) {
                if (i2s_read_buff[i] == 100) {
                    flag = 1;
                    end_position = i;
                    break;
                }
            }
        }
        length = length + bytes_read;
    }
    // test the readed data right or not
    for (int i = end_position - 99; i <= end_position; i++) {
        TEST_ASSERT_EQUAL_UINT8((i - end_position + 100), *(i2s_read_buff + i));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_1);
}
#endif

TEST_CASE("I2S_memory_leaking_test", "[i2s_legacy]")
{
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };
    i2s_pin_config_t master_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = -1,
        .data_in_num = DATA_IN_IO
    };
    uint8_t *w_buf = calloc(1, 2000);
    TEST_ASSERT(w_buf);
    uint8_t *r_buf = calloc(1, 2000);
    TEST_ASSERT(r_buf);
    size_t w_bytes = 0;
    size_t r_bytes = 0;

    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    TEST_ESP_OK(i2s_write(I2S_NUM_0, w_buf, 2000, &w_bytes, portMAX_DELAY));
    TEST_ESP_OK(i2s_read(I2S_NUM_0, r_buf, 2000, &r_bytes, portMAX_DELAY));
    i2s_driver_uninstall(I2S_NUM_0);
    int initial_size = esp_get_free_heap_size();

    for (int i = 0; i < 50; i++) {
        TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
        TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
        TEST_ESP_OK(i2s_write(I2S_NUM_0, w_buf, 2000, &w_bytes, portMAX_DELAY));
        TEST_ESP_OK(i2s_read(I2S_NUM_0, r_buf, 2000, &r_bytes, portMAX_DELAY));
        i2s_driver_uninstall(I2S_NUM_0);
        TEST_ASSERT(initial_size == esp_get_free_heap_size());
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(initial_size == esp_get_free_heap_size());

    free(w_buf);
    free(r_buf);
}

#if SOC_I2S_SUPPORTS_APLL
/*
 *   The I2S APLL clock variation test used to test the difference between the different sample rates, different bits per sample
 *   and the APLL clock generate for it. The TEST_CASE passes PERCENT_DIFF variation from the provided sample rate in APLL generated clock
 *   The percentage difference calculated as (mod((obtained clock rate - desired clock rate)/(desired clock rate))) * 100.
 */
TEST_CASE("I2S_APLL_clock_variation_test", "[i2s_legacy]")
{
    i2s_pin_config_t pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 60,
        .use_apll = true,
        .intr_alloc_flags = 0,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };

    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &pin_config));
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
    int initial_size = esp_get_free_heap_size();

    uint32_t sample_rate_arr[8] = { 10675, 11025, 16000, 22050, 32000, 44100, 48000, 96000 };
    int bits_per_sample_arr[3] = { 16, 24, 32 };

    for (int i = 0; i < (sizeof(sample_rate_arr) / sizeof(sample_rate_arr[0])); i++) {
        for (int j = 0; j < (sizeof(bits_per_sample_arr) / sizeof(bits_per_sample_arr[0])); j++) {
            i2s_config.sample_rate = sample_rate_arr[i];
            i2s_config.bits_per_sample = bits_per_sample_arr[j];

            TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
            TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &pin_config));
            TEST_ASSERT((fabs((i2s_get_clk(I2S_NUM_0) - sample_rate_arr[i])) / (sample_rate_arr[i])) * 100 < PERCENT_DIFF);
            TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
            TEST_ASSERT(initial_size == esp_get_free_heap_size());
        }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(initial_size == esp_get_free_heap_size());
}
#endif

#if SOC_I2S_SUPPORTS_DAC
TEST_CASE("I2S_dac_test", "[i2s_legacy]")
{
    // dac, adc  i2s
    i2s_config_t  i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 60,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    };

    //install and start i2s driver
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    //for internal DAC, this will enable both of the internal channels
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, NULL));
    //stop & destroy i2s driver
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
}
#endif

/*------------------------------ Clock Test --------------------------------*/
#if SOC_PCNT_SUPPORTED

#define TEST_I2S_PERIOD_MS      100

static void i2s_test_common_sample_rate(i2s_port_t id)
{
    /* Prepare configuration for the PCNT unit */
    pcnt_unit_handle_t pcnt_unit = NULL;
    pcnt_channel_handle_t pcnt_chan = NULL;

    pcnt_unit_config_t unit_config = {
        .high_limit = (int16_t)0x7fff,
        .low_limit = (int16_t)0x8000,
    };
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = MASTER_WS_IO,
        .level_gpio_num = -1,
    };
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &pcnt_unit));
    TEST_ESP_OK(pcnt_unit_set_glitch_filter(pcnt_unit, NULL));
    TEST_ESP_OK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
    TEST_ESP_OK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_unit_enable(pcnt_unit));

    // Reconfig GPIO signal
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[MASTER_WS_IO], PIN_FUNC_GPIO);
    gpio_set_direction(MASTER_WS_IO, GPIO_MODE_INPUT_OUTPUT);
    esp_rom_gpio_connect_out_signal(MASTER_WS_IO, i2s_periph_signal[0].m_tx_ws_sig, 0, 0);
    esp_rom_gpio_connect_in_signal(MASTER_WS_IO, pcnt_periph_signals.groups[0].units[0].channels[0].pulse_sig, 0);

    const uint32_t test_freq[] = {
        8000,  10000,  11025, 12000, 16000, 22050,
        24000, 32000,  44100, 48000, 64000, 88200,
        96000, 128000, 144000,196000};
    int real_pulse = 0;
#if CONFIG_IDF_ENV_FPGA
    // Limit the test sample rate on FPGA platform due to the low frequency it supports.
    int case_cnt = 10;
#else
    int case_cnt = sizeof(test_freq) / sizeof(uint32_t);
#endif

#if SOC_I2S_SUPPORTS_PLL_F96M
    // 196000 Hz sample rate doesn't support on PLL_96M target
    case_cnt = 15;
#endif

    // Acquire the PM lock incase Dynamic Frequency Scaling(DFS) lower the frequency
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
    esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
    TEST_ESP_OK(esp_pm_lock_create(pm_type, 0, "legacy_i2s_test", &pm_lock));
    esp_pm_lock_acquire(pm_lock);
#endif
    for (int i = 0; i < case_cnt; i++) {
        int expt_pulse = (int16_t)((float)test_freq[i] * (TEST_I2S_PERIOD_MS / 1000.0));
        TEST_ESP_OK(i2s_set_clk(id, test_freq[i], SAMPLE_BITS, I2S_CHANNEL_STEREO));
        vTaskDelay(1); // Waiting for hardware totally started
        // pcnt will count the pulse number on WS signal in 100ms
        TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
        TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
        vTaskDelay(pdMS_TO_TICKS(TEST_I2S_PERIOD_MS));
        TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
        TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &real_pulse));
        printf("[%"PRIu32" Hz] %d pulses, expected %d, err %d\n", test_freq[i], real_pulse, expt_pulse, real_pulse - expt_pulse);
        // Check if the error between real pulse number and expected pulse number is within 1%
        TEST_ASSERT_INT_WITHIN(expt_pulse * 0.01, expt_pulse, real_pulse);
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(pm_lock);
    esp_pm_lock_delete(pm_lock);
#endif
    TEST_ESP_OK(pcnt_del_channel(pcnt_chan));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_disable(pcnt_unit));
    TEST_ESP_OK(pcnt_del_unit(pcnt_unit));
}

TEST_CASE("I2S clock freqency test", "[i2s_legacy]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_desc_num = 6,
        .dma_frame_num = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if SOC_I2S_SUPPORTS_TDM
        .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1,
        .total_chan = 2,
        .left_align = false,
        .big_edin = false,
        .bit_order_msb = false,
        .skip_msk = false
#endif
    };
    i2s_pin_config_t master_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };
    /* Non-APLL test */
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));

    i2s_test_common_sample_rate(I2S_NUM_0);

    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));

    /* APLL test */
#if SOC_I2S_SUPPORTS_APLL
    master_i2s_config.use_apll = true;
    master_i2s_config.mclk_multiple = I2S_MCLK_MULTIPLE_256;

    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));

    i2s_test_common_sample_rate(I2S_NUM_0);
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
#endif
}

#endif // SOC_PCNT_SUPPORTED
