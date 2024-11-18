/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_rom_gpio.h"
#include "esp_log.h"
#include "test_utils.h"
#include "test_board.h"

#if SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE

static QueueHandle_t event_queue;
static uint8_t *temp_data;
static size_t temp_len = 0;

typedef enum {
    I2C_SLAVE_EVT_RX,
    I2C_SLAVE_EVT_TX
} i2c_slave_event_t;

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

static bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
    BaseType_t xTaskWoken;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_TX;
    xQueueSendFromISR(event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    BaseType_t xTaskWoken;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
    memcpy(temp_data, evt_data->buffer, evt_data->length);
    temp_len = evt_data->length;
    xQueueSendFromISR(event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static void i2c_slave_read_test_v2(void)
{
    i2c_slave_dev_handle_t handle;
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);
    temp_data = malloc(DATA_LENGTH);
    assert(temp_data);

    i2c_slave_config_t i2c_slv_config = {
        .i2c_port = TEST_I2C_PORT,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = ESP_SLAVE_ADDR,
        .send_buf_depth = DATA_LENGTH,
        .receive_buf_depth = DATA_LENGTH,
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
    if (xQueueReceive(event_queue, &evt, 1) == pdTRUE) {
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

static void i2c_master_write_test_v2(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ESP_SLAVE_ADDR,
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

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave v2 test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test_v2, i2c_slave_read_test_v2);

static void master_read_slave_test_v2(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ESP_SLAVE_ADDR,
        .scl_speed_hz = 100000,
        .scl_wait_us = 20000,
    };

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

static void slave_write_buffer_test_v2(void)
{
    i2c_slave_dev_handle_t handle;
    uint8_t data_wr[DATA_LENGTH];
    event_queue = xQueueCreate(2, sizeof(i2c_slave_event_t));
    assert(event_queue);

    i2c_slave_config_t i2c_slv_config = {
        .i2c_port = TEST_I2C_PORT,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = ESP_SLAVE_ADDR,
        .send_buf_depth = DATA_LENGTH,
        .receive_buf_depth = DATA_LENGTH,
    };

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &handle));

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
        .on_request = i2c_slave_request_cb,
    };

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

TEST_CASE_MULTIPLE_DEVICES("I2C master read slave test", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_test_v2, slave_write_buffer_test_v2);

#endif // SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE
