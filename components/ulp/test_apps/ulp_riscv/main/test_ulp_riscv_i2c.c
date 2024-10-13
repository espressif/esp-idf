/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "ulp_riscv.h"
#include "ulp_riscv_i2c.h"
#include "ulp_test_app_i2c.h"
#include "ulp_test_shared.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define ULP_WAKEUP_PERIOD 1000000 // 1 second
static const char* TAG = "ulp_riscv_i2c_test";

// ULP RISC-V RTC I2C firmware
extern const uint8_t ulp_test_app_i2c_bin_start[] asm("_binary_ulp_test_app_i2c_bin_start");
extern const size_t ulp_test_app_i2c_bin_length asm("ulp_test_app_i2c_bin_length");

static void load_and_start_ulp_riscv_firmware(const uint8_t* ulp_bin, size_t ulp_bin_len)
{
    TEST_ASSERT(ulp_riscv_load_binary(ulp_bin, ulp_bin_len) == ESP_OK);
    TEST_ASSERT(ulp_set_wakeup_period(0, ULP_WAKEUP_PERIOD) == ESP_OK);
    TEST_ASSERT(ulp_riscv_run() == ESP_OK);
}

#define I2C_SLAVE_SCL_IO     7                  /*!<I2C gpio number for SCL */
#define I2C_SLAVE_SDA_IO     6                  /*!<I2C gpio number for SDA */
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

static esp_err_t init_i2c(void)
{
    /* Configure RTC I2C */
    printf("Initializing RTC I2C ...\n");
    ulp_riscv_i2c_cfg_t i2c_cfg = ULP_RISCV_I2C_DEFAULT_CONFIG();
    esp_err_t ret = ulp_riscv_i2c_master_init(&i2c_cfg);
    if (ret != ESP_OK) {
        printf("ERROR: Failed to initialize RTC I2C. Aborting...\n");
    }

    return ret;
}

/* This runs on the DUT */
static void i2c_master_write_read_test(void)
{
    /* Initialize the test data */
    init_test_data(DATA_LENGTH);

    /* Initialize RTC I2C */
    TEST_ASSERT(init_i2c() == ESP_OK);

    /* Wait for the I2C slave device to be ready */
    unity_wait_for_signal("i2c slave init finish");

    /* Load and Run the ULP RISC-V firmware */
    load_and_start_ulp_riscv_firmware(ulp_test_app_i2c_bin_start, ulp_test_app_i2c_bin_length);

    /* Wait for ULP RISC-V to finish reading */
    while (ulp_read_test_reply == RISCV_COMMAND_INVALID) {
    }

    /* Verify the test data read by the DUT */
    uint8_t *read_data = (uint8_t*)&ulp_data_rd;
    ESP_LOGI(TAG, "Master read data:");
    ESP_LOG_BUFFER_HEX(TAG, read_data, RW_TEST_LENGTH);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_master_read_data, read_data, RW_TEST_LENGTH);

    /* Prepare the test data to be written to the I2C slave device */
    uint8_t *wr_data = (uint8_t*)&ulp_data_wr;
    for (int i = 0; i < RW_TEST_LENGTH; i++) {
        wr_data[i] = expected_master_write_data[i];
    }

    /* Wait for the I2C slave device to be ready */
    unity_wait_for_signal("master write");

    /* Signal the ULR RISC-V to perform the write to the I2C slave device */
    volatile riscv_test_command_reply_t* write_test_cmd = (riscv_test_command_reply_t*)&ulp_write_test_cmd;
    *write_test_cmd = RISCV_COMMAND_OK;

    /* Wait for ULP to finish writing */
    while (*write_test_cmd != RISCV_COMMAND_NOK) {
    }

    /* Signal the I2C slave device to read the data */
    unity_send_signal("slave read");
}

static i2c_config_t i2c_slave_init(void)
{
    i2c_config_t conf_slave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDRESS,
    };
    return conf_slave;
}

/* This runs on the I2C slave device */
static void i2c_slave_read_write_test(void)
{
    /* Initialize test data */
    init_test_data(DATA_LENGTH);

    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    memset(data_rd, 0, DATA_LENGTH);
    int size_rd;

    /* Initialize I2C slave device */
    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));

    /* Signal the DUT that the I2C slave device is ready */
    unity_send_signal("i2c slave init finish");

    /* Prepare the test data to be read by the DUT */
    size_rd = i2c_slave_write_buffer(I2C_SLAVE_NUM, expected_master_read_data, RW_TEST_LENGTH, 2000 / portTICK_PERIOD_MS);
    ESP_LOG_BUFFER_HEX(TAG, expected_master_read_data, size_rd);

    /* Signal the DUT to write test data */
    unity_send_signal("master write");

    /* Wait for DUT to write test data before reading it */
    unity_wait_for_signal("slave read");

    /* Verify the test data written by the DUT */
    size_rd = i2c_slave_read_buffer(I2C_SLAVE_NUM, data_rd, RW_TEST_LENGTH, 10000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Slave read data:");
    ESP_LOG_BUFFER_HEX(TAG, data_rd, size_rd);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_master_write_data, data_rd, RW_TEST_LENGTH);

    /* Clean up */
    free(data_rd);
    i2c_driver_delete(I2C_SLAVE_NUM);
}

TEST_CASE_MULTIPLE_DEVICES("ULP RISC-V RTC I2C read and write test", "[ulp][test_env=generic_multi_device][timeout=150]", i2c_master_write_read_test, i2c_slave_read_write_test);
