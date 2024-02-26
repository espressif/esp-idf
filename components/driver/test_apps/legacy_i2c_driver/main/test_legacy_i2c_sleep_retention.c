/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "unity_config.h"
#include "driver/i2c.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_system.h"
#include "hal/i2c_types.h"
#include "test_utils.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_ieee802154.h"
#include "esp_pm.h"

#define DATA_LENGTH          100  /*!<Data buffer length for test buffer*/

#define I2C_SLAVE_SCL_IO     4     /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     5     /*!<gpio number for i2c slave data  */

#define I2C_SLAVE_NUM I2C_NUM_0    /*!<I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave rx buffer size */

#define I2C_MASTER_SCL_IO     4     /*!<gpio number for i2c master clock  */
#define I2C_MASTER_SDA_IO     5     /*!<gpio number for i2c master data  */

#define I2C_MASTER_NUM I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ    100000     /*!< I2C master clock frequency */

#define ESP_SLAVE_ADDR 0x28         /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT  I2C_MASTER_WRITE /*!< I2C master write */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/

static esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    TEST_ESP_OK(i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN));
    TEST_ESP_OK(i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN));
    TEST_ESP_OK(i2c_master_stop(cmd));
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 5000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static i2c_config_t i2c_master_init(void)
{
    i2c_config_t conf_master = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_flags = 0,
    };
    return conf_master;
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
        .slave.slave_addr = ESP_SLAVE_ADDR,
    };
    return conf_slave;
}

// print the reading buffer
static void disp_buf(uint8_t *buf, int len)
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

static void i2c_master_write_sleep_test(void)
{
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    int i;

    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));

    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write and sleep");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    i2c_master_write_slave(I2C_MASTER_NUM, data_wr, DATA_LENGTH);
    disp_buf(data_wr, i + 1);

    TEST_ESP_OK(esp_ieee802154_enable());
    TEST_ESP_OK(sleep_cpu_configure(true));
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(3 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up!!\n");
    unity_wait_for_signal("i2c slave receive once");

    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    i2c_master_write_slave(I2C_MASTER_NUM, data_wr, DATA_LENGTH);
    disp_buf(data_wr, i + 1);
    unity_send_signal("master write again");

    free(data_wr);
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(sleep_cpu_configure(false));
    TEST_ESP_OK(esp_ieee802154_disable());
    TEST_ESP_OK(i2c_driver_delete(I2C_MASTER_NUM));
}

static void i2c_slave_read_sleep_test(void)
{
    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    int size_rd = 0;
    int len = 0;

    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write and sleep");
    while (1) {
        len = i2c_slave_read_buffer(I2C_SLAVE_NUM, data_rd + size_rd, DATA_LENGTH, 10000 / portTICK_PERIOD_MS);
        if (len == 0) {
            break;
        }
        size_rd += len;
    }
    disp_buf(data_rd, size_rd);
    for (int i = 0; i < size_rd; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }

    TEST_ESP_OK(esp_ieee802154_enable());
    TEST_ESP_OK(sleep_cpu_configure(true));
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    unity_send_signal("i2c slave receive once");
    unity_wait_for_signal("master write again");

    memset(data_rd, 0, DATA_LENGTH);
    size_rd = 0;
    while (1) {
        len = i2c_slave_read_buffer(I2C_SLAVE_NUM, data_rd + size_rd, DATA_LENGTH, 10000 / portTICK_PERIOD_MS);
        if (len == 0) {
            break;
        }
        size_rd += len;
    }
    disp_buf(data_rd, size_rd);
    for (int i = 0; i < size_rd; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }

    free(data_rd);
    unity_send_signal("ready to delete");
    TEST_ESP_OK(sleep_cpu_configure(false));
    TEST_ESP_OK(esp_ieee802154_disable());
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

TEST_CASE_MULTIPLE_DEVICES("I2C legacy sleep retention test", "[i2c][test_env=generic_multi_device][timeout=250]", i2c_master_write_sleep_test, i2c_slave_read_sleep_test);
