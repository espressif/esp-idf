/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
// For clock checking
#include "hal/uart_hal.h"
#include "soc/uart_periph.h"
#include "hal/clk_tree_hal.h"
#include "esp_private/gpio.h"
#include "hal/uart_ll.h"
#include "esp_clk_tree.h"

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

#define DATA_LENGTH 100

static QueueHandle_t s_receive_queue;

static IRAM_ATTR bool test_i2c_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void i2c_master_write_test(void)
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

static void i2c_slave_read_test(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = test_i2c_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    i2c_slave_rx_done_event_data_t rx_data;
    TEST_ESP_OK(i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");
    xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
    disp_buf(data_rd, DATA_LENGTH);
    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }
    vQueueDelete(s_receive_queue);
    unity_send_signal("ready to delete");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test, i2c_slave_read_test);

static void i2c_master_write_test_large_write_small_read(void)
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

static void i2c_slave_read_test_large_write_small_read(void)
{
    uint8_t data_rd[7] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = test_i2c_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    i2c_slave_rx_done_event_data_t rx_data;
    TEST_ESP_OK(i2c_slave_receive(slave_handle, data_rd, 7));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");
    xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
    disp_buf(data_rd, 7);
    for (int i = 0; i < 7; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }
    for (int i = 0; i < 7; i++) {
        TEST_ASSERT(rx_data.buffer[i] == i);
    }
    vQueueDelete(s_receive_queue);
    unity_send_signal("ready to delete");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test (large write small read)", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test_large_write_small_read, i2c_slave_read_test_large_write_small_read);

static void master_read_slave_test(void)
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
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    printf("Slave please write data to buffer\n");

    unity_send_signal("slave write");
    unity_wait_for_signal("master read");

    TEST_ESP_OK(i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (int i = 0; i < DATA_LENGTH; i++) {
        printf("%d\n", data_rd[i]);
        TEST_ASSERT(data_rd[i] == i);
    }
    unity_send_signal("ready to delete master read test");

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void slave_write_buffer_test(void)
{
    uint8_t data_wr[DATA_LENGTH] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("slave write");
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    TEST_ESP_OK(i2c_slave_transmit(slave_handle, data_wr, DATA_LENGTH, -1));
    disp_buf(data_wr, DATA_LENGTH);
    unity_send_signal("master read");
    unity_wait_for_signal("ready to delete master read test");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master read slave test", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_test, slave_write_buffer_test);

static void i2c_master_write_read_test(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    uint8_t data_wr[DATA_LENGTH] = {0};

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

    printf("master read buffer\n");

    unity_send_signal("slave write");
    unity_wait_for_signal("master read and write");

    TEST_ESP_OK(i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    disp_buf(data_rd, DATA_LENGTH);
    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
    unity_send_signal("slave read");
    unity_wait_for_signal("ready to read");
    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    unity_send_signal("sent done");
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void i2c_slave_read_write_test(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    uint8_t data_wr[DATA_LENGTH] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = test_i2c_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    unity_send_signal("i2c slave init finish");
    unity_wait_for_signal("slave write");

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(i2c_slave_transmit(slave_handle, data_wr, DATA_LENGTH, -1));
    disp_buf(data_wr, DATA_LENGTH);
    unity_send_signal("master read and write");
    unity_wait_for_signal("slave read");
    TEST_ESP_OK(i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH));
    unity_send_signal("ready to read");
    unity_wait_for_signal("sent done");
    i2c_slave_rx_done_event_data_t rx_data;
    xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(1000));

    printf("slave read data is:\n");
    disp_buf(data_rd, DATA_LENGTH);
    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }
    unity_send_signal("ready to delete");
    vQueueDelete(s_receive_queue);
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C read and write test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_read_test, i2c_slave_read_write_test);

static void i2c_master_repeat_write(void)
{
    uint8_t data_wr[DATA_LENGTH] = {0};
    int times = 3;

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

static void i2c_slave_repeat_read(void)
{
    uint32_t size = 0;
    int times = 3;
    uint8_t data_rd[DATA_LENGTH * 3] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = test_i2c_rx_done_callback,
    };
    TEST_ESP_OK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    unity_send_signal("i2c slave init finish");

    while (times > 0) {
        i2c_slave_rx_done_event_data_t rx_data;
        TEST_ESP_OK(i2c_slave_receive(slave_handle, data_rd + size, DATA_LENGTH));
        xQueueReceive(s_receive_queue, &rx_data, (TickType_t)portMAX_DELAY);

        size += DATA_LENGTH;
        times--;
    }

    disp_buf(data_rd, size);
    for (int j = 0; j < times; j++) {
        for (int i = 0; i < DATA_LENGTH; i++) {
            TEST_ASSERT(data_rd[i + DATA_LENGTH * j] == (i + j));
        }
    }
    unity_send_signal("ready to delete");
    vQueueDelete(s_receive_queue);
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C repeat write test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_repeat_write, i2c_slave_repeat_read);

static void master_read_slave_1b_test(void)
{
    uint8_t data_rd[1] = {0};
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

    TEST_ESP_OK(i2c_master_receive(dev_handle, data_rd, 1, -1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    printf("%x\n", data_rd[0]);
    TEST_ASSERT(data_rd[0] == 0xe);
    unity_send_signal("ready to delete master read test");

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void slave_write_buffer_1b_test(void)
{
    uint8_t data_wr[1] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("slave write");
    data_wr[0] = 0xe;

    TEST_ESP_OK(i2c_slave_transmit(slave_handle, data_wr, 1, -1));
    disp_buf(data_wr, 1);
    unity_send_signal("master read");
    unity_wait_for_signal("ready to delete master read test");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master read slave 1 byte test", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_1b_test, slave_write_buffer_1b_test);

static void master_probe_slave(void)
{
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
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
    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("probe finish");

    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master probe slave test", "[i2c][test_env=generic_multi_device][timeout=150]", master_probe_slave, slave_init_for_probe);

#if SOC_HP_I2C_NUM > 1
// Now chips with multiple I2C controllers are up to 2, can test more ports when we have more I2C controllers.
static void i2c_master_write_test_more_port(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 1,
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
    uint8_t data_rd[DATA_LENGTH] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 1,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = test_i2c_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    i2c_slave_rx_done_event_data_t rx_data;
    TEST_ESP_OK(i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");
    xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
    disp_buf(data_rd, DATA_LENGTH);
    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }
    vQueueDelete(s_receive_queue);
    unity_send_signal("ready to delete");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test, more ports", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test_more_port, i2c_slave_read_test_more_port);
#endif

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
// For now, we tested the chip which has such problem.
// This test can be extended to all chip when how uart baud rate
// works has been figured out.

#if SOC_RCC_IS_INDEPENDENT
#define HP_UART_BUS_CLK_ATOMIC()
#else
#define HP_UART_BUS_CLK_ATOMIC()       PERIPH_RCC_ATOMIC()
#endif

//Init uart baud rate detection
static void uart_aut_baud_det_init(int rxd_io_num)
{
    gpio_func_sel(rxd_io_num, PIN_FUNC_GPIO);
    gpio_set_direction(rxd_io_num, GPIO_MODE_INPUT);
    gpio_pullup_en(rxd_io_num);
    esp_rom_gpio_connect_in_signal(rxd_io_num, UART_PERIPH_SIGNAL(1, SOC_UART_RX_PIN_IDX), 0);
    HP_UART_BUS_CLK_ATOMIC() {
        uart_ll_enable_bus_clock(1, true);
        uart_ll_reset_register(1);
    }
    /* Reset all the bits */
    uart_ll_disable_intr_mask(&UART1, ~0);
    uart_ll_clr_intsts_mask(&UART1, ~0);
    uart_ll_set_autobaud_en(&UART1, true);
}

static void i2c_master_write_fsm_reset(void)
{
    uint8_t data_wr[3] = { 0 };

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
        .scl_speed_hz = 10000, // Not a typical value for I2C
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    // Nack will reset the bus
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_master_transmit(dev_handle, data_wr, 3, -1));

    unity_send_signal("i2c transmit fail--connect uart");
    unity_wait_for_signal("uart connected");

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_master_transmit(dev_handle, data_wr, 3, -1));

    unity_send_signal("i2c transmit after fsm reset");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void uart_test_i2c_master_freq(void)
{
    unity_wait_for_signal("i2c transmit fail--connect uart");
    uart_aut_baud_det_init(I2C_MASTER_SCL_IO);

    unity_send_signal("uart connected");
    unity_wait_for_signal("i2c transmit after fsm reset");
    int pospulse_cnt = uart_ll_get_pos_pulse_cnt(&UART1);
    int negpulse_cnt = uart_ll_get_neg_pulse_cnt(&UART1);
    // Uart uses XTAL as default clock source
    int freq_hz = (clk_hal_xtal_get_freq_mhz() * 1 * 1000 * 1000) / (pospulse_cnt + negpulse_cnt);
    printf("The tested I2C SCL frequency is %d\n", freq_hz);
    TEST_ASSERT_INT_WITHIN(500, 10000, freq_hz);
    uart_ll_set_autobaud_en(&UART1, false);
    HP_UART_BUS_CLK_ATOMIC() {
        uart_ll_enable_bus_clock(1, false);
    }
}

TEST_CASE_MULTIPLE_DEVICES("I2C master clock frequency test", "[i2c][test_env=generic_multi_device][timeout=150]", uart_test_i2c_master_freq, i2c_master_write_fsm_reset);

#endif // CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
