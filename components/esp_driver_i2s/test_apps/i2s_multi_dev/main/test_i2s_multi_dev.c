/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "test_utils.h"
#include "driver/gpio.h"
#include "driver/i2s_tdm.h"
#include "driver/i2s_std.h"
#include "soc/i2s_struct.h"

static const char *TAG = "i2s_multi_dev_test";

#define TEST_I2S_FRAME_SIZE             (128)       // Frame numbers in every writing / reading
#define TEST_I2S_ARRAY_LENGTH           (1024)      // The loop data length for verification
#define TEST_I2S_MAX_DATA               (256)       // The maximum data value in the data buffer
#define TEST_I2S_MAX_FAIL_CNT           (3)         // Max broken packet count
#define TEST_I2S_FRAME_TIMEOUT_SEC      (10.0f)     // Timeout seconds of waiting for a correct frame

#define TEST_I2S_NUM            (I2S_NUM_0) // ESP32-C3 has only I2S0
#define TEST_I2S_MCK_IO         (GPIO_NUM_1)
#define TEST_I2S_BCK_IO         (GPIO_NUM_4)
#define TEST_I2S_WS_IO          (GPIO_NUM_5)
#if CONFIG_IDF_TARGET_ESP32H2
#define TEST_I2S_DO_IO          (GPIO_NUM_2)
#define TEST_I2S_DI_IO          (GPIO_NUM_3) // DI and DO gpio will be reversed on slave runner
#else
#define TEST_I2S_DO_IO          (GPIO_NUM_6)
#define TEST_I2S_DI_IO          (GPIO_NUM_7) // DI and DO gpio will be reversed on slave runner
#endif  // CONFIG_IDF_TARGET_ESP32H2

typedef struct {
    uint32_t *buffer;
    uint32_t buffer_size;
} test_i2s_tdm_write_buffer_t;

#define TEST_I2S_DEFAULT_GPIO(mclk_pin, is_master) {  \
    .mclk = mclk_pin,  \
    .bclk = TEST_I2S_BCK_IO,  \
    .ws = TEST_I2S_WS_IO,  \
    .dout = is_master ? TEST_I2S_DO_IO : TEST_I2S_DI_IO,  \
    .din = is_master ? TEST_I2S_DI_IO : TEST_I2S_DO_IO,  \
}

static void test_i2s_tdm_master(uint32_t sample_rate, i2s_data_bit_width_t bit_width, i2s_tdm_slot_mask_t slot_mask)
{
    i2s_chan_handle_t i2s_tdm_tx_handle = NULL;
    i2s_chan_handle_t i2s_tdm_rx_handle = NULL;

    /* Create I2S tx and rx channels */
    i2s_chan_config_t i2s_channel_config = {
        .id = TEST_I2S_NUM,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 4,
        .dma_frame_num = TEST_I2S_FRAME_SIZE,
        .auto_clear = false
    };
    TEST_ESP_OK(i2s_new_channel(&i2s_channel_config, &i2s_tdm_tx_handle, &i2s_tdm_rx_handle));

    /* Configure channels to TDM mode */
    i2s_tdm_config_t i2s_tdm_config = {
        .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG(bit_width, I2S_SLOT_MODE_STEREO, slot_mask),
        .gpio_cfg = TEST_I2S_DEFAULT_GPIO(GPIO_NUM_NC, true),
    };
    i2s_tdm_config.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_tx_handle, &i2s_tdm_config));
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_rx_handle, &i2s_tdm_config));

    uint32_t channel_count = 32 - __builtin_clz(slot_mask);
    size_t buf_size = channel_count * TEST_I2S_FRAME_SIZE * (bit_width / 8);
    /* Allocate I2S rx buffer */
    ESP_LOGI(TAG, "Allocating I2S TDM master rx buffer, size=%u", buf_size);
    uint32_t *rx_buffer = malloc(buf_size);
    TEST_ASSERT(rx_buffer);
    /* Allocate I2S tx buffer */
    ESP_LOGI(TAG, "Allocating I2S TDM master tx buffer, size=%u", buf_size);
    uint32_t *tx_buffer = malloc(buf_size);
    TEST_ASSERT(tx_buffer);
    /* Fill in the tx buffer */
    for (uint32_t i = 0, data_cnt = 0; i < buf_size / sizeof(uint32_t); i ++) {
        tx_buffer[i] = data_cnt;
        data_cnt++;
        data_cnt %= TEST_I2S_MAX_DATA;
    }
    size_t w_bytes = buf_size;
    while (w_bytes != 0) {
        ESP_ERROR_CHECK(i2s_channel_preload_data(i2s_tdm_tx_handle, tx_buffer, buf_size, &w_bytes));
    }

    unity_wait_for_signal("Slave Ready");
    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_tx_handle));
    vTaskDelay(pdMS_TO_TICKS(100));  // Waiting the data be steady on line
    unity_send_signal("Master Ready");

    /* Slave is ready, start the writing task */
    ESP_LOGI(TAG, "I2S TDM master receive & send start");
    esp_err_t read_ret = ESP_OK;
    uint32_t count = 1;
    uint8_t fail_cnt = 0;
    size_t bytes_read = 0;
    for (fail_cnt = 0; fail_cnt < TEST_I2S_MAX_FAIL_CNT && count < TEST_I2S_MAX_DATA; fail_cnt++) {
        if (i2s_channel_read(i2s_tdm_rx_handle, rx_buffer, buf_size, &bytes_read, 1000) != ESP_OK) {
            continue;
        }
        for (int i = 0; i < buf_size && count < TEST_I2S_MAX_DATA; i++) {
            if (rx_buffer[i] == count) {
                count++;
            } else if (count != 1) {
                ESP_LOGE(TAG, "Failed at index: %d real: %" PRIu32 " expect: %" PRIu32, i, rx_buffer[i], count);
                count = 1;
            }
        }
    }

    ESP_LOGI(TAG, "I2S TDM master stop");
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_tx_handle));
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_rx_handle));
    free(rx_buffer);
    free(tx_buffer);
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_tx_handle));
    ESP_LOGI(TAG, "I2S TDM master resources freed");
    TEST_ASSERT_TRUE_MESSAGE(read_ret == ESP_OK, "Master read timeout ");
    TEST_ASSERT_TRUE_MESSAGE(fail_cnt < TEST_I2S_MAX_FAIL_CNT, "Exceed retry times ");
    TEST_ASSERT_EQUAL_UINT32(TEST_I2S_MAX_DATA, count);
}

static void test_i2s_tdm_slave(uint32_t sample_rate, i2s_data_bit_width_t bit_width, i2s_tdm_slot_mask_t slot_mask)
{
    i2s_chan_handle_t i2s_tdm_tx_handle = NULL;
    i2s_chan_handle_t i2s_tdm_rx_handle = NULL;

    /* Create I2S tx and rx channels */
    i2s_chan_config_t i2s_channel_config = {
        .id = TEST_I2S_NUM,
        .role = I2S_ROLE_SLAVE,
        .dma_desc_num = 4,
        .dma_frame_num = TEST_I2S_FRAME_SIZE,
        .auto_clear = false
    };
    TEST_ESP_OK(i2s_new_channel(&i2s_channel_config, &i2s_tdm_tx_handle, &i2s_tdm_rx_handle));

    /* Configure channels to TDM mode */
    i2s_tdm_config_t i2s_tdm_config = {
        .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG(bit_width, I2S_SLOT_MODE_STEREO, slot_mask),
        .gpio_cfg = TEST_I2S_DEFAULT_GPIO(GPIO_NUM_NC, false),
    };
    if (sample_rate >= 96000) {
        i2s_tdm_config.clk_cfg.bclk_div = 12;
    }
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_tx_handle, &i2s_tdm_config));
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_rx_handle, &i2s_tdm_config));

    /* Allocate I2S rx buffer */
    uint32_t channel_count = 32 - __builtin_clz(slot_mask);
    uint32_t buffer_size = TEST_I2S_FRAME_SIZE * channel_count * (bit_width / 8);
    ESP_LOGI(TAG, "Allocating I2S TDM slave buffer, size=%"PRIu32, buffer_size);
    uint32_t *echo_buffer = malloc(buffer_size);
    TEST_ASSERT(echo_buffer);

    unity_send_signal("Slave Ready");
    unity_wait_for_signal("Master Ready");
    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_tx_handle));

    ESP_LOGI(TAG, "I2S TDM slave receive & send start");
    size_t bytes_read = 0, bytes_written = 0;
    /* Loop until reading or writing failed, which indicates the master has finished and deleted the I2S peripheral */
    while (true) {
        if (i2s_channel_read(i2s_tdm_rx_handle, echo_buffer, buffer_size, &bytes_read, 500) != ESP_OK) {
            break;
        }
        if (i2s_channel_write(i2s_tdm_tx_handle, echo_buffer, buffer_size, &bytes_written, 500) != ESP_OK) {
            break;
        }
    }

    ESP_LOGI(TAG, "I2S TDM slave receive stop");
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_tx_handle));
    free(echo_buffer);
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_tx_handle));
    ESP_LOGI(TAG, "I2S TDM slave resources freed");
}

static void test_i2s_tdm_master_48k_32bits_4slots(void)
{
    test_i2s_tdm_master(48000, I2S_DATA_BIT_WIDTH_32BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

static void test_i2s_tdm_slave_48k_32bits_4slots(void)
{
    test_i2s_tdm_slave(48000, I2S_DATA_BIT_WIDTH_32BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

TEST_CASE_MULTIPLE_DEVICES("I2S_TDM_full_duplex_test_in_48k_32bits_4slots", "[I2S_TDM]",
                           test_i2s_tdm_master_48k_32bits_4slots, test_i2s_tdm_slave_48k_32bits_4slots);

static void test_i2s_tdm_master_48k_16bits_4slots(void)
{
    test_i2s_tdm_master(48000, I2S_DATA_BIT_WIDTH_16BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

static void test_i2s_tdm_slave_48k_16bits_4slots(void)
{
    test_i2s_tdm_slave(48000, I2S_DATA_BIT_WIDTH_16BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

TEST_CASE_MULTIPLE_DEVICES("I2S_TDM_full_duplex_test_in_48k_16bits_4slots", "[I2S_TDM]",
                           test_i2s_tdm_master_48k_16bits_4slots, test_i2s_tdm_slave_48k_16bits_4slots);

static void test_i2s_tdm_master_48k_8bits_4slots(void)
{
    test_i2s_tdm_master(48000, I2S_DATA_BIT_WIDTH_8BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

static void test_i2s_tdm_slave_48k_8bits_4slots(void)
{
    test_i2s_tdm_slave(48000, I2S_DATA_BIT_WIDTH_8BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

TEST_CASE_MULTIPLE_DEVICES("I2S_TDM_full_duplex_test_in_48k_8bits_4slots", "[I2S_TDM]",
                           test_i2s_tdm_master_48k_8bits_4slots, test_i2s_tdm_slave_48k_8bits_4slots);

/* The I2S source clock can only reach 96Mhz on ESP32H2,
   which can't satisfy the following configurations in slave mode */
#if !CONFIG_IDF_TARGET_ESP32H2
static void test_i2s_tdm_master_48k_16bits_8slots(void)
{
    test_i2s_tdm_master(48000, I2S_DATA_BIT_WIDTH_16BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3 |
                        I2S_TDM_SLOT4 | I2S_TDM_SLOT5 | I2S_TDM_SLOT6 | I2S_TDM_SLOT7);
}

static void test_i2s_tdm_slave_48k_16bits_8slots(void)
{
    test_i2s_tdm_slave(48000, I2S_DATA_BIT_WIDTH_16BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3 |
                       I2S_TDM_SLOT4 | I2S_TDM_SLOT5 | I2S_TDM_SLOT6 | I2S_TDM_SLOT7);
}

TEST_CASE_MULTIPLE_DEVICES("I2S_TDM_full_duplex_test_in_48k_16bits_8slots", "[I2S_TDM]",
                           test_i2s_tdm_master_48k_16bits_8slots, test_i2s_tdm_slave_48k_16bits_8slots);

static void test_i2s_tdm_master_96k_16bits_4slots(void)
{
    test_i2s_tdm_master(96000, I2S_DATA_BIT_WIDTH_16BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

static void test_i2s_tdm_slave_96k_16bits_4slots(void)
{
    test_i2s_tdm_slave(96000, I2S_DATA_BIT_WIDTH_16BIT, I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3);
}

TEST_CASE_MULTIPLE_DEVICES("I2S_TDM_full_duplex_test_in_96k_16bits_4slots", "[I2S_TDM]",
                           test_i2s_tdm_master_96k_16bits_4slots, test_i2s_tdm_slave_96k_16bits_4slots);
#endif  // !CONFIG_IDF_TARGET_ESP32H2

static void test_i2s_external_clk_src(bool is_master, bool is_external)
{
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, is_master ? I2S_ROLE_MASTER : I2S_ROLE_SLAVE);
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(16, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = TEST_I2S_DEFAULT_GPIO(TEST_I2S_MCK_IO, is_master),
    };
    if (is_external) {
        std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_EXTERNAL;
        std_cfg.clk_cfg.ext_clk_freq_hz = 11289600;
    }
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    if (is_master && !is_external) {
        i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(16, I2S_SLOT_MODE_STEREO);
        memcpy(&std_cfg.slot_cfg, &slot_cfg, sizeof(i2s_std_slot_config_t));
    }
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));

    if (is_master) {
        if (!is_external) {
            // Delay bclk to get compensate the data delay
            I2S0.rx_timing.rx_bck_out_dm = 1;
        }
        uint8_t mst_tx_data[4] = {0x12, 0x34, 0x56, 0x78};
        size_t w_bytes = 4;
        while (w_bytes == 4) {
            ESP_ERROR_CHECK(i2s_channel_preload_data(tx_handle, mst_tx_data, 4, &w_bytes));
        }
    }

    uint8_t *recv_buff = (uint8_t *)calloc(1, TEST_I2S_ARRAY_LENGTH);
    TEST_ASSERT(recv_buff);

    unity_send_signal(is_master ? "Master Ready" : "Slave Ready");
    unity_wait_for_signal(is_master ? "Slave Ready" : "Master Ready");
    if (is_external) {
        unity_wait_for_signal("External Clock Ready");
    }
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    if (!is_external) {
        unity_send_signal("External Clock Ready");
    }

    bool is_success = false;
    size_t bytes_read = 0;
    if (is_master) {
        if (is_external) {
            unity_send_signal("Master Data Ready");
        }
        // Wait the Slave data ready on line
        unity_wait_for_signal("Slave Data Ready");
        vTaskDelay(pdMS_TO_TICKS(100)); // Wait another 100 ms for the data to be steady in DMA buffer
        TEST_ESP_OK(i2s_channel_read(rx_handle, recv_buff, TEST_I2S_ARRAY_LENGTH, &bytes_read, 1000));
    } else {
        if (!is_external) {
            unity_wait_for_signal("Master Data Ready");
        }
        TEST_ESP_OK(i2s_channel_read(rx_handle, recv_buff, TEST_I2S_ARRAY_LENGTH, &bytes_read, 1000));
        // Fill the DMA buffer
        for (int i = 0; i < 6; i++) {
            TEST_ESP_OK(i2s_channel_write(tx_handle, recv_buff, TEST_I2S_ARRAY_LENGTH, &bytes_read, 1000));
        }
        // Send the signal indicates the data have been ready on line
        unity_send_signal("Slave Data Ready");
    }
    // Check the data
    for (int i = 0; i < TEST_I2S_ARRAY_LENGTH; i++) {
        if (recv_buff[i] == 0x12 && recv_buff[i + 1] == 0x34 &&
                recv_buff[i + 2] == 0x56 && recv_buff[i + 3] == 0x78) {
            is_success = true;
            break;
        }
        printf("%x ", recv_buff[i]);
    }
    printf("\n");

    if (is_master) {
        unity_send_signal("Master Finished");
    } else {
        unity_wait_for_signal("Master Finished");
    }
    // Disable and free the resources
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    free(recv_buff);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    // Assert whether the test success
    TEST_ASSERT(is_success);
}

static void test_i2s_master_clock_out(void)
{
    test_i2s_external_clk_src(true, false);
}

static void test_i2s_slave_clock_in(void)
{
    test_i2s_external_clk_src(false, true);
}

TEST_CASE_MULTIPLE_DEVICES("I2S_external_clock_master_output_slave_input", "[I2S]",
                           test_i2s_master_clock_out, test_i2s_slave_clock_in);

static void test_i2s_master_clock_in(void)
{
    test_i2s_external_clk_src(true, true);
}

static void test_i2s_slave_clock_out(void)
{
    test_i2s_external_clk_src(false, false);
}

TEST_CASE_MULTIPLE_DEVICES("I2S_external_clock_master_input_slave_output", "[I2S]",
                           test_i2s_master_clock_in, test_i2s_slave_clock_out);
