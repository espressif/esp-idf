/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "lp_core_test_app_i2c.h"
#include "ulp_lp_core.h"
#include "lp_core_i2c.h"

#include "test_shared.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"

#include "driver/i2c_slave.h"
#include "hal/i2c_ll.h"
#include "soc/i2c_struct.h"

extern const uint8_t lp_core_main_i2c_bin_start[] asm("_binary_lp_core_test_app_i2c_bin_start");
extern const uint8_t lp_core_main_i2c_bin_end[]   asm("_binary_lp_core_test_app_i2c_bin_end");

static const char* TAG = "lp_core_i2c_test";

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);

}

#define I2C_SCL_IO           LP_I2C_SCL_IO   /*!<gpio number for i2c clock */
#define I2C_SDA_IO           LP_I2C_SDA_IO   /*!<gpio number for i2c data */
#define I2C_SLAVE_NUM        I2C_NUM_0       /*!<I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN (2*DATA_LENGTH) /*!<I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2*DATA_LENGTH) /*!<I2C slave rx buffer size */

static uint8_t expected_master_write_data[DATA_LENGTH];
static uint8_t expected_master_read_data[DATA_LENGTH];
static uint8_t s_slave_rx_data[DATA_LENGTH];
static size_t s_slave_rx_len;
static QueueHandle_t s_i2c_slave_event_queue;

typedef enum {
    I2C_SLAVE_EVT_RX,
} i2c_slave_test_event_t;

static bool IRAM_ATTR i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave,
                                           const i2c_slave_rx_done_event_data_t *evt_data,
                                           void *arg)
{
    BaseType_t task_woken = pdFALSE;
    i2c_slave_test_event_t evt = I2C_SLAVE_EVT_RX;
    s_slave_rx_len = evt_data->length;
    if (evt_data->length <= sizeof(s_slave_rx_data)) {
        memcpy(s_slave_rx_data, evt_data->buffer, evt_data->length);
    }
    xQueueSendFromISR(s_i2c_slave_event_queue, &evt, &task_woken);
    return task_woken == pdTRUE;
}

static void init_test_data(size_t len)
{
    /* Set up test data with some predictable patterns */
    for (int i = 0; i < len; i++) {
        expected_master_write_data[i] = i % 3;
    }

    for (int i = 0; i < len; i++) {
        expected_master_read_data[i] = i / 2;
    }
}

static void i2c_master_write_read_test(void)
{
    init_test_data(DATA_LENGTH);

    esp_err_t ret = ESP_OK;
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    const lp_core_i2c_cfg_t i2c_cfg = LP_CORE_I2C_DEFAULT_CONFIG();
    ret = lp_core_i2c_master_init(LP_I2C_NUM_0, &i2c_cfg);
    TEST_ASSERT_EQUAL(ret, ESP_OK);

    unity_wait_for_signal("i2c slave init finish");

    load_and_start_lp_core_firmware(&cfg, lp_core_main_i2c_bin_start, lp_core_main_i2c_bin_end);

    /* Wait for ULP to finish reading */
    while (ulp_read_test_reply == LP_CORE_COMMAND_INVALID) {
    }

    ESP_LOGI(TAG, "LP I2C read returned %ld", (long)ulp_read_test_ret);
    TEST_ASSERT_EQUAL(ESP_OK, (esp_err_t)ulp_read_test_ret);

    uint8_t *read_data = (uint8_t*)&ulp_data_rd;
    ESP_LOGI(TAG, "Master read data:");
    ESP_LOG_BUFFER_HEX(TAG, read_data, RW_TEST_LENGTH);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_master_read_data, read_data, RW_TEST_LENGTH);

    uint8_t *wr_data = (uint8_t*)&ulp_data_wr;
    for (int i = 0; i < RW_TEST_LENGTH; i++) {
        wr_data[i] = expected_master_write_data[i];
    }

    unity_wait_for_signal("master write");

    volatile lp_core_test_command_reply_t* write_test_cmd = (lp_core_test_command_reply_t*)&ulp_write_test_cmd;
    *write_test_cmd = LP_CORE_COMMAND_OK;
    unity_send_signal("slave read");

    /* Wait for ULP to finish writing */
    while (*write_test_cmd != LP_CORE_COMMAND_NOK) {
    }

    ESP_LOGI(TAG, "LP I2C write returned %ld", (long)ulp_write_test_ret);
    TEST_ASSERT_EQUAL(ESP_OK, (esp_err_t)ulp_write_test_ret);
}

static i2c_slave_config_t i2c_slave_init(void)
{
    i2c_slave_config_t conf_slave = {
        .i2c_port = I2C_SLAVE_NUM,
        .sda_io_num = I2C_SDA_IO,
        .scl_io_num = I2C_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = I2C_SLAVE_TX_BUF_LEN,
        .receive_buf_depth = I2C_SLAVE_RX_BUF_LEN,
        .slave_addr = I2C_SLAVE_ADDRESS,
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .flags.enable_internal_pullup = true,
    };
    return conf_slave;
}

static void i2c_slave_disable_clock_stretch(void)
{
    /* TODO(IDF-15683): remove this workaround once the new I2C slave driver handles LP I2C repeated-start reads. */
    i2c_ll_slave_enable_scl_stretch(&I2C0, false);
    i2c_ll_slave_clear_stretch(&I2C0);
    i2c_ll_update(&I2C0);
}

static void i2c_slave_read_write_test(void)
{
    init_test_data(DATA_LENGTH);

    uint32_t size_wr;

    i2c_slave_dev_handle_t slave_handle = NULL;
    i2c_slave_config_t conf_slave = i2c_slave_init();
    s_i2c_slave_event_queue = xQueueCreate(4, sizeof(i2c_slave_test_event_t));
    TEST_ASSERT_NOT_NULL(s_i2c_slave_event_queue);
    s_slave_rx_len = 0;
    memset(s_slave_rx_data, 0, sizeof(s_slave_rx_data));

    TEST_ESP_OK(i2c_new_slave_device(&conf_slave, &slave_handle));
    i2c_slave_disable_clock_stretch();

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
    };
    TEST_ESP_OK(i2c_slave_register_event_callbacks(slave_handle, &cbs, NULL));

    TEST_ESP_OK(i2c_slave_write(slave_handle, expected_master_read_data, RW_TEST_LENGTH, &size_wr, 2000));
    ESP_LOGI(TAG, "Slave queued read data length: %"PRIu32, size_wr);
    TEST_ASSERT_EQUAL_UINT32(RW_TEST_LENGTH, size_wr);
    ESP_LOG_BUFFER_HEX(TAG, expected_master_read_data, RW_TEST_LENGTH);

    unity_send_signal("i2c slave init finish");

    unity_send_signal("master write");
    unity_wait_for_signal("slave read");

    i2c_slave_test_event_t evt;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(s_i2c_slave_event_queue, &evt, pdMS_TO_TICKS(10000)));
    TEST_ASSERT_EQUAL(I2C_SLAVE_EVT_RX, evt);
    ESP_LOGI(TAG, "Slave read length: %d", s_slave_rx_len);
    ESP_LOGI(TAG, "Slave read data:");
    ESP_LOG_BUFFER_HEX(TAG, s_slave_rx_data, s_slave_rx_len);
    TEST_ASSERT_EQUAL_INT(RW_TEST_LENGTH, s_slave_rx_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_master_write_data, s_slave_rx_data, RW_TEST_LENGTH);

    vQueueDelete(s_i2c_slave_event_queue);
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("LP-Core I2C read and write test", "[lp_core][test_env=generic_multi_device][timeout=150]", i2c_master_write_read_test, i2c_slave_read_write_test);
