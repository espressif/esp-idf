/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "lp_core_test_app_i2c.h"
#include "ulp_lp_core.h"
#include "lp_core_i2c.h"

#include "test_shared.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"

#include "driver/i2c.h"

extern const uint8_t lp_core_main_i2c_bin_start[] asm("_binary_lp_core_test_app_i2c_bin_start");
extern const uint8_t lp_core_main_i2c_bin_end[]   asm("_binary_lp_core_test_app_i2c_bin_end");

static const char* TAG = "lp_core_i2c_test";

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);

}

#define I2C_SCL_IO     7                        /*!<gpio number for i2c clock, for C6 only GPIO7 is valid  */
#define I2C_SDA_IO     6                        /*!<gpio number for i2c data, for C6 only GPIO6 is valid */
#define I2C_SLAVE_NUM I2C_NUM_0                 /*!<I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN  (2*DATA_LENGTH)   /*!<I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN  (2*DATA_LENGTH)   /*!<I2C slave rx buffer size */

static uint8_t expected_master_write_data[DATA_LENGTH];
static uint8_t expected_master_read_data[DATA_LENGTH];

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
}

static i2c_config_t i2c_slave_init(void)
{
    i2c_config_t conf_slave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SDA_IO,
        .scl_io_num = I2C_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDRESS,
    };
    return conf_slave;
}

static void i2c_slave_read_write_test(void)
{
    init_test_data(DATA_LENGTH);

    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    memset(data_rd, 0, DATA_LENGTH);
    int size_rd;

    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));

    unity_send_signal("i2c slave init finish");

    size_rd = i2c_slave_write_buffer(I2C_SLAVE_NUM, expected_master_read_data, RW_TEST_LENGTH, 2000 / portTICK_PERIOD_MS);
    ESP_LOG_BUFFER_HEX(TAG, expected_master_read_data, size_rd);

    unity_send_signal("master write");
    unity_wait_for_signal("slave read");

    size_rd = i2c_slave_read_buffer(I2C_SLAVE_NUM, data_rd, RW_TEST_LENGTH, 10000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Slave read data:");
    ESP_LOG_BUFFER_HEX(TAG, data_rd, size_rd);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_master_write_data, data_rd, RW_TEST_LENGTH);

    free(data_rd);
    i2c_driver_delete(I2C_SLAVE_NUM);
}

TEST_CASE_MULTIPLE_DEVICES("LP-Core I2C read and write test", "[lp_core][test_env=generic_multi_device][timeout=150]", i2c_master_write_read_test, i2c_slave_read_write_test);
