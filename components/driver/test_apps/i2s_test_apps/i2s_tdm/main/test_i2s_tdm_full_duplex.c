/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
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

static const char *TAG = "i2s_tdm_full_duplex_test";

#define TEST_I2S_FRAME_SIZE             (128)       // Frame numbers in every writing / reading
#define TEST_I2S_ARRAY_LENGTH           (1024)      // The loop data length for verification
#define TEST_I2S_MAX_FAIL_CNT           (3)         // Max broken packet count
#define TEST_I2S_FRAME_TIMEOUT_SEC      (10.0f)     // Timeout seconds of waiting for a correct frame

#define TEST_I2S_NUM            (I2S_NUM_0) // ESP32-C3 has only I2S0
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
    TaskHandle_t maintask_handle;
    QueueHandle_t tx_queue;
    i2s_chan_handle_t tx_channel_handle;
    i2s_data_bit_width_t tx_data_bit_width;
    i2s_tdm_slot_mask_t tdm_slot_mask;
} test_i2s_tdm_write_task_args_t;

typedef struct {
    uint32_t *buffer;
    uint32_t buffer_size;
} test_i2s_tdm_write_buffer_t;

static void test_i2s_tdm_master_write_task(void *args)
{
    test_i2s_tdm_write_task_args_t *task_args = (test_i2s_tdm_write_task_args_t*)args;

    /* Allocate I2S tx buffer */
    uint32_t channel_count = 32 - __builtin_clz(task_args->tdm_slot_mask);
    uint32_t tx_buffer_size = TEST_I2S_FRAME_SIZE * channel_count * (task_args->tx_data_bit_width / 8);
    ESP_LOGI(TAG, "Allocating I2S TDM master tx buffer, size=%ld", tx_buffer_size);
    uint32_t *tx_buffer = malloc(tx_buffer_size);
    TEST_ASSERT(tx_buffer);

    uint32_t data_cnt = 0;
    size_t bytes_written = 0;

    /* Block here waiting for the main thread receiving Slave Ready signals */
    xTaskNotifyWait(0, ULONG_MAX, NULL, portMAX_DELAY);

    ESP_LOGI(TAG, "I2S TDM master send start");
    while (xTaskNotifyWait(0, ULONG_MAX, NULL, 0) == pdFALSE) { // if main task sends terminate signal, exit the loop
        /* Fill in the tx buffer */
        for (uint32_t i = 0; i < tx_buffer_size / sizeof(uint32_t); i ++) {
            tx_buffer[i] = data_cnt;
            data_cnt++;
            data_cnt %= TEST_I2S_ARRAY_LENGTH;
        }
        TEST_ESP_OK(i2s_channel_write(task_args->tx_channel_handle, tx_buffer, tx_buffer_size,
                                      &bytes_written, 1000));
    }

    ESP_LOGI(TAG, "Freeing I2S TDM master tx buffer");
    free(tx_buffer);

    xTaskNotifyGive(task_args->maintask_handle); // notify main task that cleanup is done
    vTaskSuspend(NULL); // wait to be deleted
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
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,
            .bclk = TEST_I2S_BCK_IO,
            .ws = TEST_I2S_WS_IO,
            .dout = TEST_I2S_DO_IO,
            .din = TEST_I2S_DI_IO
        },
    };
    i2s_tdm_config.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_tx_handle, &i2s_tdm_config));
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_rx_handle, &i2s_tdm_config));

    /* Create TDM write task */
    TaskHandle_t subtask_handle = NULL;
    /* Make the variable static in case it become invalid in the write task */
    static test_i2s_tdm_write_task_args_t task_args;
    task_args.tx_channel_handle = i2s_tdm_tx_handle;
    task_args.maintask_handle = xTaskGetCurrentTaskHandle();
    task_args.tx_data_bit_width = bit_width;
    task_args.tdm_slot_mask = slot_mask;
    xTaskCreate(test_i2s_tdm_master_write_task, "I2S TDM Write Task", 4096, &task_args, 5, &subtask_handle);

    /* Allocate I2S rx buffer */
    uint32_t channel_count = 32 - __builtin_clz(slot_mask);
    uint32_t rx_buffer_size = channel_count * TEST_I2S_FRAME_SIZE * (bit_width / 8);
    ESP_LOGI(TAG, "Allocating I2S TDM master rx buffer, size=%"PRIu32, rx_buffer_size);
    uint32_t *rx_buffer = malloc(rx_buffer_size);
    TEST_ASSERT(rx_buffer);

    uint32_t count = 1;
    bool is_start = false;
    uint8_t fail_cnt = 0;
    size_t bytes_read = 0;
    float time = 0;
    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_tx_handle));
    unity_send_signal("Master Ready");
    unity_wait_for_signal("Slave Ready");

    /* Slave is ready, start the writing task */
    ESP_LOGI(TAG, "I2S TDM master receive & send start");
    esp_err_t read_ret = ESP_OK;
    xTaskNotifyGive(subtask_handle);
    while (count < TEST_I2S_ARRAY_LENGTH && fail_cnt < TEST_I2S_MAX_FAIL_CNT && time < TEST_I2S_FRAME_TIMEOUT_SEC) {
        read_ret = i2s_channel_read(i2s_tdm_rx_handle, rx_buffer, rx_buffer_size, &bytes_read, 1000);
        if (read_ret != ESP_OK) {
            break;
        }
        for (int i = 0; i < rx_buffer_size / sizeof(uint32_t); i++) {
            if (rx_buffer[i] == count) {
                count++;
                if (count >= TEST_I2S_ARRAY_LENGTH) {
                    break;
                }
                if (!is_start) {
                    is_start = true;
                }
            } else if (is_start) {
                ESP_LOGE(TAG, "Failed at index: %d real: %"PRIu32" expect: %"PRIu32"\n", i, rx_buffer[i], count);
                is_start = false;
                count = 1;
                fail_cnt++;
            }
        }
        time += (float)TEST_I2S_MAX_FAIL_CNT / (float)sample_rate;
    }
    unity_send_signal("Master Finished");

    ESP_LOGI(TAG, "Send signal to terminate subtask");
    xTaskNotifyGive(subtask_handle); // notify subtask to exit
    xTaskNotifyWait(0, ULONG_MAX, NULL, portMAX_DELAY); // wait subtask to do some cleanups
    ESP_LOGI(TAG, "Deleting subtask");
    unity_utils_task_delete(subtask_handle); // delete subtask

    ESP_LOGI(TAG, "I2S TDM master send stop");
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_tx_handle));
    ESP_LOGI(TAG, "I2S TDM master receive stop");
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_rx_handle));

    ESP_LOGI(TAG, "Freeing I2S TDM master rx buffer");
    free(rx_buffer);
    ESP_LOGI(TAG, "Deleting i2s tx and rx channels");
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_tx_handle));
    TEST_ASSERT_TRUE_MESSAGE(read_ret == ESP_OK, "Master read timeout ");
    TEST_ASSERT_TRUE_MESSAGE(fail_cnt < TEST_I2S_MAX_FAIL_CNT, "Broken data received ");
    TEST_ASSERT_TRUE_MESSAGE(time < TEST_I2S_FRAME_TIMEOUT_SEC, "Waiting for valid data timeout ");
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
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,
            .bclk = TEST_I2S_BCK_IO,
            .ws = TEST_I2S_WS_IO,
            .dout = TEST_I2S_DI_IO,
            .din = TEST_I2S_DO_IO // on slave, swap DI and DO pin
        },
    };
    if (sample_rate >= 96000) {
        i2s_tdm_config.clk_cfg.bclk_div = 12;
    }
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_tx_handle, &i2s_tdm_config));
    TEST_ESP_OK(i2s_channel_init_tdm_mode(i2s_tdm_rx_handle, &i2s_tdm_config));

    /* Allocate I2S rx buffer */
    uint32_t channel_count = 32 - __builtin_clz(slot_mask);
    uint32_t rx_buffer_size = TEST_I2S_FRAME_SIZE * channel_count * (bit_width / 8);
    ESP_LOGI(TAG, "Allocating I2S TDM slave buffer, size=%ld", rx_buffer_size);
    uint32_t *rx_buffer = malloc(rx_buffer_size);
    TEST_ASSERT(rx_buffer);

    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_channel_enable(i2s_tdm_tx_handle));
    unity_send_signal("Slave Ready");
    unity_wait_for_signal("Master Ready");

    ESP_LOGI(TAG, "I2S TDM slave receive & send start");
    size_t bytes_read = 0, bytes_written = 0;
    /* Loop until reading or writing failed, which indicates the master has finished and deleted the I2S peripheral */
    while (true) {
        if (i2s_channel_read(i2s_tdm_rx_handle, rx_buffer, rx_buffer_size, &bytes_read, 500) != ESP_OK) {
            break;
        }
        if (i2s_channel_write(i2s_tdm_tx_handle, rx_buffer, rx_buffer_size, &bytes_written, 500) != ESP_OK) {
            break;
        }
    }
    unity_wait_for_signal("Master Finished");

    ESP_LOGI(TAG, "I2S TDM slave receive stop");
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_channel_disable(i2s_tdm_tx_handle));

    ESP_LOGI(TAG, "Freeing I2S TDM slave buffer");
    free(rx_buffer);

    ESP_LOGI(TAG, "Deleting i2s tx and rx channels");
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_rx_handle));
    TEST_ESP_OK(i2s_del_channel(i2s_tdm_tx_handle));
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
