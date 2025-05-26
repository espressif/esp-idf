/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "hal/uart_ll.h"
#include "esp_private/periph_ctrl.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_log.h"
#include "test_utils.h"
#include "test_board.h"

#define DATA_LENGTH 100

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

TEST_CASE("LP I2C initialize on i2c slave", "[i2c]")
{
    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = LP_I2C_SCLK_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .send_buf_depth = 256,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, i2c_new_slave_device(&i2c_slv_config, &slave_handle));
}

#if CONFIG_IDF_TARGET_ESP32C6

TEST_CASE("LP I2C initialize with wrong IO", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config = {
        .lp_source_clk = LP_I2C_SCLK_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .scl_io_num = 5,
        .sda_io_num = 6,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}

#endif

TEST_CASE("LP I2C initialize with wrong clock source", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config = {
        .lp_source_clk = I2C_CLK_SRC_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}

static void lp_i2c_master_write_test(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {
        .lp_source_clk = LP_I2C_SCLK_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

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

static void hp_i2c_slave_read_test(void)
{
    i2c_slave_dev_handle_t handle;
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);
    temp_data = heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT);
    assert(temp_data);

    i2c_slave_config_t i2c_slv_config = {
        .i2c_port = TEST_I2C_PORT,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .slave_addr = 0x58,
        .send_buf_depth = DATA_LENGTH,
        .receive_buf_depth = DATA_LENGTH,
        .flags.enable_internal_pullup = true,
    };

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
        .on_request = i2c_slave_request_cb,
    };

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

TEST_CASE_MULTIPLE_DEVICES("LP_I2C master write slave test", "[i2c][test_env=generic_multi_device][timeout=150]", lp_i2c_master_write_test, hp_i2c_slave_read_test);
