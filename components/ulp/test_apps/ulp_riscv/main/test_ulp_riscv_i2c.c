/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "ulp_riscv.h"
#include "ulp_riscv_i2c.h"
#include "ulp_test_app_i2c.h"
#include "ulp_test_shared.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "driver/i2c_slave.h"
#include "hal/i2c_ll.h"
#include "soc/i2c_struct.h"

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

#define I2C_SLAVE_SCL_IO     2                  /*!<I2C gpio number for SCL, must match RTC I2C master */
#define I2C_SLAVE_SDA_IO     3                  /*!<I2C gpio number for SDA, must match RTC I2C master */
#define I2C_SLAVE_NUM I2C_NUM_0                 /*!<I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN  (2*DATA_LENGTH)   /*!<I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN  (2*DATA_LENGTH)   /*!<I2C slave rx buffer size */

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

    /* Verify that the ULP I2C read succeeded */
    TEST_ASSERT_EQUAL(RISCV_COMMAND_OK, ulp_read_test_reply & 0xFFFF);

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

    /* Let the slave side finish the read transaction before starting the write. */
    unity_send_signal("master read done");
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

static i2c_slave_config_t i2c_slave_init(void)
{
    i2c_slave_config_t conf_slave = {
        .i2c_port = I2C_SLAVE_NUM,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
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
    /* TODO(IDF-15683): remove this workaround once the new I2C slave driver handles RTC I2C repeated-start reads. */
    i2c_ll_slave_enable_scl_stretch(&I2C0, false);
    i2c_ll_slave_clear_stretch(&I2C0);
    i2c_ll_update(&I2C0);
}

/* This runs on the I2C slave device */
static void i2c_slave_read_write_test(void)
{
    /* Initialize test data */
    init_test_data(DATA_LENGTH);

    uint32_t size_wr;

    /* Initialize I2C slave device */
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

    /* Prepare the test data to be read by the DUT */
    TEST_ESP_OK(i2c_slave_write(slave_handle, expected_master_read_data, RW_TEST_LENGTH, &size_wr, 2000));
    ESP_LOGI(TAG, "Slave queued read data length: %"PRIu32, size_wr);
    TEST_ASSERT_EQUAL_UINT32(RW_TEST_LENGTH, size_wr);
    ESP_LOG_BUFFER_HEX(TAG, expected_master_read_data, RW_TEST_LENGTH);

    /* Signal the DUT that the I2C slave device is ready */
    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master read done");
    i2c_slave_test_event_t evt;

    /* Signal the DUT to write test data */
    unity_send_signal("master write");

    /* Wait for DUT to write test data before reading it */
    unity_wait_for_signal("slave read");

    /* Verify the test data written by the DUT */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(s_i2c_slave_event_queue, &evt, pdMS_TO_TICKS(10000)));

    ESP_LOGI(TAG, "Slave read length: %d", s_slave_rx_len);
    ESP_LOGI(TAG, "Slave read data:");
    ESP_LOG_BUFFER_HEX(TAG, s_slave_rx_data, s_slave_rx_len);
    TEST_ASSERT_EQUAL_INT(RW_TEST_LENGTH + 1, s_slave_rx_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_master_write_data, &s_slave_rx_data[1], RW_TEST_LENGTH);

    /* Clean up */
    vQueueDelete(s_i2c_slave_event_queue);
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("ULP RISC-V RTC I2C read and write test", "[ulp][test_env=generic_multi_device][timeout=150]", i2c_master_write_read_test, i2c_slave_read_write_test);
