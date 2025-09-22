/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "test_board.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_log.h"
#include "test_utils.h"

void disp_buf(uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

static QueueHandle_t event_queue;
static uint8_t *temp_data;
static size_t temp_len = 0;

static IRAM_ATTR bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
    BaseType_t xTaskWoken;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_TX;
    xQueueSendFromISR(event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static IRAM_ATTR bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    BaseType_t xTaskWoken;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
    memcpy(temp_data, evt_data->buffer, evt_data->length);
    temp_len = evt_data->length;
    xQueueSendFromISR(event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static void i2c_slave_read_test(void)
{
    unity_wait_for_signal("i2c master init first");
    i2c_slave_dev_handle_t handle;
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);
    temp_data = static_cast<uint8_t*>(heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT));
    assert(temp_data);

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = ESP_SLAVE_ADDR;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");

    i2c_slave_event_t evt;
    if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        if (evt == I2C_SLAVE_EVT_RX) {
            disp_buf(temp_data, temp_len);
            printf("length is %x\n", temp_len);
            for (int i = 0; i < temp_len; i++) {
                TEST_ASSERT(temp_data[i] == i);
            }
        }
    }

    unity_send_signal("ready to delete");
    free(temp_data);
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

static void i2c_master_write_test(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = ESP_SLAVE_ADDR;
    dev_cfg.scl_speed_hz = 100000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_send_signal("i2c master init first");

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    disp_buf(data_wr, i);
    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

#if CONFIG_IDF_TARGET_ESP32S2
// The test for s2 is unstable on ci, but it should not fail in local test
TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test", "[i2c][test_env=generic_multi_device][timeout=150][ignore]", i2c_master_write_test, i2c_slave_read_test);
#else
TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test, i2c_slave_read_test);
#endif

static void master_read_slave_test(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;
    i2c_master_bus_handle_t bus_handle;
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = ESP_SLAVE_ADDR;
    dev_cfg.scl_speed_hz = 100000;
    dev_cfg.scl_wait_us = 20000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    TEST_ESP_OK(i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (int i = 0; i < DATA_LENGTH; i++) {
        printf("%x\n", data_rd[i]);
        TEST_ASSERT(data_rd[i] == i);
    }
    unity_send_signal("ready to delete master read test");

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void slave_write_buffer_test(void)
{
    i2c_slave_dev_handle_t handle;
    uint8_t data_wr[DATA_LENGTH];
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = ESP_SLAVE_ADDR;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    i2c_slave_event_t evt;
    uint32_t write_len;
    while (true) {
        if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == I2C_SLAVE_EVT_TX) {
                TEST_ESP_OK(i2c_slave_write(handle, data_wr, DATA_LENGTH, &write_len, 1000));
                break;
            }
        }
    }

    unity_wait_for_signal("ready to delete master read test");
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master read slave test", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_test, slave_write_buffer_test);

static void i2c_master_write_test_with_customize_api(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = I2C_DEVICE_ADDRESS_NOT_USED;
    dev_cfg.scl_speed_hz = 100000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_send_signal("i2c master init first");

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    disp_buf(data_wr, i);

    uint8_t address = (ESP_SLAVE_ADDR << 1 | 0);

    i2c_operation_job_t i2c_ops[] = {
        { .command = I2C_MASTER_CMD_START, .write = {}},
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = true, .data = (uint8_t *) &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = true, .data = (uint8_t *) data_wr, .total_bytes = DATA_LENGTH } },
        { .command = I2C_MASTER_CMD_STOP, .write = {}},
    };

    TEST_ESP_OK(i2c_master_execute_defined_operations(dev_handle, i2c_ops, sizeof(i2c_ops) / sizeof(i2c_operation_job_t), -1));
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}
TEST_CASE_MULTIPLE_DEVICES("I2C master write slave with customize api", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test_with_customize_api, i2c_slave_read_test);

static void master_read_slave_test_single_byte(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t bus_handle;
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = I2C_DEVICE_ADDRESS_NOT_USED;
    dev_cfg.scl_speed_hz = 100000;
    dev_cfg.scl_wait_us = 20000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    uint8_t read_address = (ESP_SLAVE_ADDR << 1 | 1);

    i2c_operation_job_t i2c_ops[] = {
        { .command = I2C_MASTER_CMD_START, .write = {}},
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = true, .data = (uint8_t *) &read_address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_READ, .read = { .ack_value = I2C_NACK_VAL, .data = data_rd, .total_bytes = 1 }},
        { .command = I2C_MASTER_CMD_STOP, .write = {}},
    };

    i2c_master_execute_defined_operations(dev_handle, i2c_ops, sizeof(i2c_ops) / sizeof(i2c_operation_job_t), 1000);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(data_rd[0] == 6);
    unity_send_signal("ready to delete master read test");

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void slave_write_buffer_test_single_byte(void)
{
    i2c_slave_dev_handle_t handle;
    uint8_t data_wr[DATA_LENGTH];
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = ESP_SLAVE_ADDR;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    data_wr[0] = 6;

    i2c_slave_event_t evt;
    uint32_t write_len;
    while (true) {
        if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == I2C_SLAVE_EVT_TX) {
                TEST_ESP_OK(i2c_slave_write(handle, data_wr, 1, &write_len, 1000));
                break;
            }
        }
    }

    unity_wait_for_signal("ready to delete master read test");
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master read slave test single byte", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_test_single_byte, slave_write_buffer_test_single_byte);

static void master_read_slave_test_with_multi_read_job(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;
    i2c_master_bus_handle_t bus_handle;
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = I2C_DEVICE_ADDRESS_NOT_USED;
    dev_cfg.scl_speed_hz = 100000;
    dev_cfg.scl_wait_us = 20000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    uint8_t read_address = (ESP_SLAVE_ADDR << 1 | 1);

    i2c_operation_job_t i2c_ops[] = {
        { .command = I2C_MASTER_CMD_START, .write = {}},
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = true, .data = (uint8_t *) &read_address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_READ, .read = { .ack_value = I2C_ACK_VAL, .data = data_rd, .total_bytes = 1 }},
        { .command = I2C_MASTER_CMD_READ, .read = { .ack_value = I2C_ACK_VAL, .data = data_rd + 1, .total_bytes = 1 }},
        { .command = I2C_MASTER_CMD_READ, .read = { .ack_value = I2C_NACK_VAL, .data = data_rd + 2, .total_bytes = 1 }},
        { .command = I2C_MASTER_CMD_STOP, .write = {}},
    };

    i2c_master_execute_defined_operations(dev_handle, i2c_ops, sizeof(i2c_ops) / sizeof(i2c_operation_job_t), 1000);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(data_rd[0] == 6);
    TEST_ASSERT(data_rd[1] == 8);
    TEST_ASSERT(data_rd[2] == 10);
    unity_send_signal("ready to delete master read test");

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void slave_write_buffer_test_single_byte_for_multi_read_job(void)
{
    i2c_slave_dev_handle_t handle;
    uint8_t data_wr[DATA_LENGTH];
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = ESP_SLAVE_ADDR;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    data_wr[0] = 6;
    data_wr[1] = 8;
    data_wr[2] = 10;

    i2c_slave_event_t evt;
    uint32_t write_len;
    while (true) {
        if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == I2C_SLAVE_EVT_TX) {
                TEST_ESP_OK(i2c_slave_write(handle, data_wr, 3, &write_len, 1000));
                break;
            }
        }
    }

    unity_wait_for_signal("ready to delete master read test");
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master read slave test single byte 2", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_test_with_multi_read_job, slave_write_buffer_test_single_byte_for_multi_read_job);

static void i2c_slave_repeat_read(void)
{
    unity_wait_for_signal("i2c master init first");
    i2c_slave_dev_handle_t handle;
    event_queue = xQueueCreate(5, sizeof(i2c_slave_event_t));
    assert(event_queue);
    temp_data = static_cast<uint8_t*>(heap_caps_malloc(DATA_LENGTH * 3, MALLOC_CAP_DEFAULT));
    assert(temp_data);
    int times = 3;

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH * 3;
    i2c_slv_config.slave_addr = ESP_SLAVE_ADDR;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    i2c_slave_event_t evt;
    while (times > 0) {
        if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == I2C_SLAVE_EVT_RX) {
                disp_buf(temp_data, temp_len);
                printf("length is %x\n", temp_len);
                for (int i = 0; i < temp_len; i++) {
                    TEST_ASSERT(temp_data[i] == (i + 3 - times));
                }
            }
        }
        times--;
    }

    unity_send_signal("ready to delete");
    free(temp_data);
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

static void i2c_master_repeat_write(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;
    int times = 3;

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = ESP_SLAVE_ADDR;
    dev_cfg.scl_speed_hz = 100000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_send_signal("i2c master init first");

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    for (int j = 0; j < times; j++) {
        for (int i = 0; i < DATA_LENGTH; i++) {
            data_wr[i] = j + i;
        }
        TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
        disp_buf(data_wr, DATA_LENGTH);
    }
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C repeat write test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_repeat_write, i2c_slave_repeat_read);

static void master_probe_slave(void)
{
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t bus_handle;
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    unity_wait_for_signal("i2c slave init finish");

    esp_err_t ret = ESP_OK;
    for (uint8_t i = 0x01; i < 0x7F; i++) {
        ret = i2c_master_probe(bus_handle, i, -1);
        if (ret == ESP_OK) {
            printf("The slave has been found, the address is %x\n", i);
            TEST_ASSERT(i == 0x58);
            break;
        }
        TEST_ASSERT(ret == ESP_ERR_NOT_FOUND);
    }

    unity_send_signal("probe finish");

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void slave_init_for_probe(void)
{
    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = 0x58;
    i2c_slv_config.flags.enable_internal_pullup = true;

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("probe finish");

    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master probe slave test", "[i2c][test_env=generic_multi_device][timeout=150]", master_probe_slave, slave_init_for_probe);

static void i2c_master_write_multi_buffer_test(void)
{
    uint8_t data_wr[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    uint8_t data_wr2[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr2[i] = i + 0x05;
    }
    uint8_t data_wr3[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr3[i] = i + 0xf;
    }

    i2c_master_transmit_multi_buffer_info_t buffer_info[3] = {
        {.write_buffer = data_wr, .buffer_size = DATA_LENGTH},
        {.write_buffer = data_wr2, .buffer_size = DATA_LENGTH},
        {.write_buffer = data_wr3, .buffer_size = DATA_LENGTH},
    };

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = ESP_SLAVE_ADDR;
    dev_cfg.scl_speed_hz = 100000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_send_signal("i2c master init first");

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");

    disp_buf(data_wr, DATA_LENGTH);
    disp_buf(data_wr2, DATA_LENGTH);
    disp_buf(data_wr3, DATA_LENGTH);
    TEST_ESP_OK(i2c_master_multi_buffer_transmit(dev_handle, buffer_info, sizeof(buffer_info) / sizeof(i2c_master_transmit_multi_buffer_info_t), -1));
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void i2c_slave_read_multi_buffer_test(void)
{
    unity_wait_for_signal("i2c master init first");
    i2c_slave_dev_handle_t handle;
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);
    temp_data = static_cast<uint8_t*>(heap_caps_malloc(DATA_LENGTH * 3, MALLOC_CAP_DEFAULT));
    assert(temp_data);

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH * 3;
    i2c_slv_config.slave_addr = ESP_SLAVE_ADDR;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");

    i2c_slave_event_t evt;
    if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        if (evt == I2C_SLAVE_EVT_RX) {
            disp_buf(temp_data, temp_len);
            printf("length is %x\n", temp_len);
            for (int i = 0; i < DATA_LENGTH; i++) {
                TEST_ASSERT(temp_data[i] == i);
            }
            for (int i = DATA_LENGTH; i < DATA_LENGTH * 2; i++) {
                TEST_ASSERT(temp_data[i] == ((i - DATA_LENGTH) + 0x05));
            }
            for (int i = DATA_LENGTH * 2; i < DATA_LENGTH * 3; i++) {
                TEST_ASSERT(temp_data[i] == ((i - DATA_LENGTH * 2) + 0x0f));
            }
        }
    }

    unity_send_signal("ready to delete");
    free(temp_data);
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave with multi buffer api test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_multi_buffer_test, i2c_slave_read_multi_buffer_test);

static void i2c_slave_reset_tx_fifo_test_master(void)
{
    uint8_t data_rd[16] = {0}; // Buffer for reading data from slave

    // Configure I2C master bus
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t bus_handle;
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    // Configure I2C device
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = ESP_SLAVE_ADDR;
    dev_cfg.scl_speed_hz = 100000;
    dev_cfg.scl_wait_us = 20000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    // First read: Request 8 bytes from slave (slave prepared 16 bytes)
    memset(data_rd, 0, sizeof(data_rd));
    TEST_ESP_OK(i2c_master_receive(dev_handle, data_rd, 8, -1));

    printf("First read (8 bytes): ");
    for (int i = 0; i < 8; i++) {
        printf("0x%02x ", data_rd[i]);
    }
    printf("\n");

    // Verify first batch data (0x00 to 0x07)
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_HEX8(i, data_rd[i]);
    }

    unity_send_signal("first read complete");
    unity_wait_for_signal("slave reset tx fifo");

    // Second read: Request 8 bytes again (should get new data after reset)
    memset(data_rd, 0, sizeof(data_rd));
    TEST_ESP_OK(i2c_master_receive(dev_handle, data_rd, 8, -1));

    printf("Second read (8 bytes): ");
    for (int i = 0; i < 8; i++) {
        printf("0x%02x ", data_rd[i]);
    }
    printf("\n");

    // Verify second batch data (0x10 to 0x17)
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x10 + i, data_rd[i]);
    }

    unity_send_signal("second read complete");

    // Cleanup
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void i2c_slave_reset_tx_fifo_test_slave(void)
{
    i2c_slave_dev_handle_t handle;
    uint8_t data_wr_first[16];  // First batch of data (0x00 to 0x0F)
    uint8_t data_wr_second[16]; // Second batch of data (0x10 to 0x1F)

    event_queue = xQueueCreate(5, sizeof(i2c_slave_event_t));
    assert(event_queue);

    // Configure I2C slave
    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = 32; // Larger buffer for test
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = ESP_SLAVE_ADDR;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    // Register event callbacks
    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;
    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    // Prepare first batch of data (0x00 to 0x0F)
    for (int i = 0; i < 16; i++) {
        data_wr_first[i] = i;
    }

    // Prepare second batch of data (0x10 to 0x1F)
    for (int i = 0; i < 16; i++) {
        data_wr_second[i] = 0x10 + i;
    }

    unity_send_signal("i2c slave init finish");

    // Wait for first request and send first batch data
    i2c_slave_event_t evt;
    uint32_t write_len;

    if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        if (evt == I2C_SLAVE_EVT_TX) {
            printf("Slave: Preparing first batch data (16 bytes)\n");
            TEST_ESP_OK(i2c_slave_write(handle, data_wr_first, 16, &write_len, 1000));
            printf("Slave: Written %lu bytes to TX buffer\n", write_len);
        }
    }

    unity_wait_for_signal("first read complete");

    // Reset TX FIFO to clear remaining data
    printf("Slave: Resetting TX FIFO\n");
    TEST_ESP_OK(i2c_slave_reset_tx_fifo(handle));

    unity_send_signal("slave reset tx fifo");

    // Wait for second request and send second batch data
    if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        if (evt == I2C_SLAVE_EVT_TX) {
            printf("Slave: Preparing second batch data (16 bytes)\n");
            TEST_ESP_OK(i2c_slave_write(handle, data_wr_second, 16, &write_len, 1000));
            printf("Slave: Written %lu bytes to TX buffer\n", write_len);
        }
    }

    unity_wait_for_signal("second read complete");

    // Cleanup
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C slave reset TX FIFO test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_slave_reset_tx_fifo_test_master, i2c_slave_reset_tx_fifo_test_slave);

#if SOC_HP_I2C_NUM > 1
// Now chips with multiple I2C controllers are up to 2, can test more ports when we have more I2C controllers.
static void i2c_master_write_test_more_port(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = 1;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = 0x58;
    dev_cfg.scl_speed_hz = 100000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    disp_buf(data_wr, i);
    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void i2c_slave_read_test_more_port(void)
{
    i2c_slave_dev_handle_t handle;
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);
    temp_data = static_cast<uint8_t*>(heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT));
    assert(temp_data);

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = 1;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = 0x58;
    i2c_slv_config.flags.enable_internal_pullup = true;

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    TEST_ESP_OK(i2c_slave_register_event_callbacks(handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");

    i2c_slave_event_t evt;
    if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        if (evt == I2C_SLAVE_EVT_RX) {
            disp_buf(temp_data, temp_len);
            printf("length is %x\n", temp_len);
            for (int i = 0; i < temp_len; i++) {
                TEST_ASSERT(temp_data[i] == i);
            }
        }
    }

    unity_send_signal("ready to delete");
    free(temp_data);
    vQueueDelete(event_queue);
    TEST_ESP_OK(i2c_del_slave_device(handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test, more ports", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test_more_port, i2c_slave_read_test_more_port);
#endif
