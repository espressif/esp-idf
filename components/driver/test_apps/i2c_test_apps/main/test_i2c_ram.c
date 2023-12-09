/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/gpio_periph.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"
#include "hal/uart_ll.h"
#include "esp_private/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_rom_gpio.h"
#include "esp_log.h"
#include "test_utils.h"
#include "test_board.h"

static esp_err_t i2c_master_write_to_addr(i2c_master_dev_handle_t device_handle, uint8_t address, const uint8_t *data, uint32_t size, int xfer_timeout_ms)
{
    esp_err_t ret = ESP_OK;
    uint8_t *buffer = (uint8_t*)calloc(1, size + 1);
    buffer[0] = address;
    memcpy(buffer + 1, data, size);
    ret = i2c_master_transmit(device_handle, buffer, 1 + size, xfer_timeout_ms);
    return ret;
}

#define DATA_LENGTH_RAM 6

static void i2c_master_write_to_ram_test(void)
{
    uint8_t data_wr[DATA_LENGTH_RAM] = { 0 };
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
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH_RAM; i++) {
        data_wr[i] = i + 1;
    }
    disp_buf(data_wr, i);
    // Write slave ram address is 0x5.
    i2c_master_write_to_addr(dev_handle, 0x5, data_wr, DATA_LENGTH_RAM, -1);

    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void i2c_slave_read_from_ram_test(void)
{
    uint8_t data_rd[DATA_LENGTH_RAM] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
        .flags.access_ram_en = true,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));


    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");

    TEST_ESP_OK(i2c_slave_read_ram(slave_handle, 0x5, data_rd, DATA_LENGTH_RAM));

    disp_buf(data_rd, DATA_LENGTH_RAM);
    for (int i = 0; i < DATA_LENGTH_RAM; i++) {
        TEST_ASSERT(data_rd[i] == (i + 1));
    }
    unity_send_signal("ready to delete");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test - slave directly read from ram", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_to_ram_test, i2c_slave_read_from_ram_test);


static void master_read_slave_from_ram_test(void)
{
    uint8_t data_rd[DATA_LENGTH_RAM] = {0};
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
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    printf("Slave please write data to buffer\n");

    unity_send_signal("slave write");
    unity_wait_for_signal("master read");

    uint8_t *buffer = (uint8_t*)malloc(sizeof(uint8_t));
    buffer[0] = 0x2; // Means write offset 2 in I2C ram.
    TEST_ESP_OK(i2c_master_transmit_receive(dev_handle, buffer, 1, data_rd, DATA_LENGTH_RAM, -1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (int i = 0; i < DATA_LENGTH_RAM; i++) {
        printf("%d\n", data_rd[i]);
        TEST_ASSERT(data_rd[i]==i);
    }
    unity_send_signal("ready to delete ram test");

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void slave_write_buffer_to_ram_test(void)
{
    uint8_t data_wr[DATA_LENGTH_RAM] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
        .flags.access_ram_en = true,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("slave write");
    for (int i = 0; i < DATA_LENGTH_RAM; i++) {
        data_wr[i] = i;
    }

    TEST_ESP_OK(i2c_slave_write_ram(slave_handle, 0x2, data_wr, DATA_LENGTH_RAM));
    disp_buf(data_wr, DATA_LENGTH_RAM);
    unity_send_signal("master read");
    unity_wait_for_signal("ready to delete ram test");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}


TEST_CASE_MULTIPLE_DEVICES("I2C master read slave ram test", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_from_ram_test, slave_write_buffer_to_ram_test);

TEST_CASE("I2C slave init as ram but read by fifo", "[i2c]")
{
    uint8_t data_rd[10] = {0};
    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
        .flags.access_ram_en = true,
    };
    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, i2c_slave_receive(slave_handle, data_rd, 10));
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}
