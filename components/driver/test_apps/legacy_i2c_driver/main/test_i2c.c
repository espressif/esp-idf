/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#include "hal/i2c_periph.h"
#include "esp_system.h"
#include "soc/uart_struct.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_types.h"
#include "hal/uart_ll.h"
#include "hal/i2c_types.h"
#include "hal/uart_periph.h"
#include "test_utils.h"
#include "esp_private/gpio.h"
#include "driver/gpio.h"

#define DATA_LENGTH          512  /*!<Data buffer length for test buffer*/
#define RW_TEST_LENGTH       129  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/

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
#define READ_BIT   I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/

static DRAM_ATTR i2c_dev_t *const I2C[SOC_HP_I2C_NUM] = { &I2C0,
#if SOC_HP_I2C_NUM > 1
                                                          &I2C1,
#endif
                                                        };

#define ACK_VAL 0
#define NACK_VAL 1

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

#if SOC_I2C_SUPPORT_SLAVE // i2c test can't work without slave

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

#endif

TEST_CASE("I2C i2c_set_pin() fails if sda and scl gpios are same", "[i2c]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, i2c_set_pin(0, 0, 0, true, true, I2C_MODE_MASTER));
}

TEST_CASE("I2C config test", "[i2c]")
{
    // master test
    i2c_config_t conf_master = i2c_master_init();
    gpio_pullup_t sda_pull_up_en[2] = {GPIO_PULLUP_DISABLE, GPIO_PULLUP_ENABLE};
    gpio_pullup_t scl_pull_up_en[2] = {GPIO_PULLUP_DISABLE, GPIO_PULLUP_ENABLE};

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            conf_master.sda_pullup_en = sda_pull_up_en[i];
            conf_master.scl_pullup_en = scl_pull_up_en[j];
            TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                           I2C_MASTER_RX_BUF_DISABLE,
                                           I2C_MASTER_TX_BUF_DISABLE, 0));
            TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));
            TEST_ASSERT_EQUAL_INT32(I2C[I2C_MASTER_NUM]->ctr.ms_mode, 1);
            TEST_ESP_OK(i2c_driver_delete(I2C_MASTER_NUM));
        }
    }

    // slave test

#if SOC_I2C_SUPPORT_SLAVE // i2c test can't work without slave
    i2c_config_t conf_slave = i2c_slave_init();
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            conf_slave.sda_pullup_en = sda_pull_up_en[i];
            conf_slave.scl_pullup_en = scl_pull_up_en[j];
            TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                           I2C_SLAVE_RX_BUF_LEN,
                                           I2C_SLAVE_TX_BUF_LEN, 0));
            TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
            TEST_ASSERT_EQUAL_INT32(I2C[I2C_SLAVE_NUM] -> ctr.ms_mode, 0);
            TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
        }
    }
#endif

}

TEST_CASE("I2C set and get period test", "[i2c]")
{
    int high_period, low_period;
    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));

    TEST_ESP_OK(i2c_set_period(I2C_MASTER_NUM, I2C_SCL_HIGH_PERIOD_V, I2C_SCL_HIGH_PERIOD_V));
    TEST_ESP_OK(i2c_get_period(I2C_MASTER_NUM, &high_period, &low_period));
    TEST_ASSERT_EQUAL_INT(I2C_SCL_HIGH_PERIOD_V, high_period);
    TEST_ASSERT_EQUAL_INT(I2C_SCL_HIGH_PERIOD_V, low_period);

    TEST_ASSERT_NOT_NULL((void *)i2c_set_period(I2C_MASTER_NUM, I2C_SCL_HIGH_PERIOD_V + 1, I2C_SCL_HIGH_PERIOD_V + 1));

    TEST_ESP_OK(i2c_set_period(I2C_MASTER_NUM, 300, 400));
    TEST_ESP_OK(i2c_get_period(I2C_MASTER_NUM, &high_period, &low_period));
    TEST_ASSERT_EQUAL_INT(300, high_period);
    TEST_ASSERT_EQUAL_INT(400, low_period);
    TEST_ESP_OK(i2c_driver_delete(I2C_MASTER_NUM));
}

#if SOC_I2C_SUPPORT_SLAVE // i2c test can't work without slave

TEST_CASE("I2C config FIFO test", "[i2c]")
{
    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    TEST_ASSERT_BIT_LOW(1, I2C[I2C_SLAVE_NUM]->fifo_conf.tx_fifo_rst);
    TEST_ESP_OK(i2c_reset_tx_fifo(I2C_SLAVE_NUM));
    TEST_ASSERT_BIT_LOW(0, I2C[I2C_SLAVE_NUM]->fifo_conf.tx_fifo_rst);

    TEST_ESP_OK(i2c_reset_rx_fifo(I2C_SLAVE_NUM));
    TEST_ASSERT_BIT_LOW(0, I2C[I2C_SLAVE_NUM]->fifo_conf.rx_fifo_rst);
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

#endif

TEST_CASE("I2C timing test", "[i2c]")
{
    int test_setup_time, test_data_time, test_stop_time, test_hold_time;
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));

    TEST_ESP_OK(i2c_set_start_timing(I2C_MASTER_NUM, 50, 60));
    TEST_ESP_OK(i2c_set_data_timing(I2C_MASTER_NUM, 80, 60));
    TEST_ESP_OK(i2c_set_stop_timing(I2C_MASTER_NUM, 100, 60));

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    i2c_master_write_slave(I2C_MASTER_NUM, data_wr, RW_TEST_LENGTH);
    TEST_ESP_OK(i2c_get_start_timing(I2C_MASTER_NUM, &test_setup_time, &test_hold_time));
    TEST_ESP_OK(i2c_get_data_timing(I2C_MASTER_NUM, &test_data_time, &test_hold_time));
    TEST_ESP_OK(i2c_get_stop_timing(I2C_MASTER_NUM, &test_stop_time, &test_hold_time));
    TEST_ASSERT_EQUAL_INT32(50, test_setup_time);
    TEST_ASSERT_EQUAL_INT32(80, test_data_time);
    TEST_ASSERT_EQUAL_INT32(100, test_stop_time);
    TEST_ASSERT_EQUAL_INT32(60, test_hold_time);
    free(data_wr);
    i2c_driver_delete(I2C_MASTER_NUM);
}

TEST_CASE("I2C driver memory leaking check", "[i2c]")
{
    esp_err_t ret;

    int size = esp_get_free_heap_size();
    for (uint32_t i = 0; i <= 5; i++) {
        ret = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                 I2C_SLAVE_RX_BUF_LEN,
                                 I2C_SLAVE_TX_BUF_LEN, 0);
        TEST_ASSERT(ret == ESP_OK);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        i2c_driver_delete(I2C_MASTER_NUM);
        TEST_ASSERT(ret == ESP_OK);
    }

    TEST_ASSERT_INT_WITHIN(100, size, esp_get_free_heap_size());
}

#if SOC_I2C_SUPPORT_SLAVE

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

static void i2c_master_write_test(void)
{
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    int i;

    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));

    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH / 2; i++) {
        data_wr[i] = i;
    }
    i2c_master_write_slave(I2C_MASTER_NUM, data_wr, DATA_LENGTH / 2);
    disp_buf(data_wr, i + 1);
    free(data_wr);
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_driver_delete(I2C_MASTER_NUM));
}

static void i2c_slave_read_test(void)
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

    unity_wait_for_signal("master write");
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
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test, i2c_slave_read_test);

static void master_read_slave_test(void)
{
    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    memset(data_rd, 0, DATA_LENGTH);
    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));
    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    unity_wait_for_signal("i2c slave init finish");

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | READ_BIT, ACK_CHECK_EN);

    unity_send_signal("slave write");
    unity_wait_for_signal("master read");
    i2c_master_read(cmd, data_rd, RW_TEST_LENGTH - 1, ACK_VAL);
    i2c_master_read_byte(cmd, data_rd + RW_TEST_LENGTH - 1, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 5000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (int i = 0; i < RW_TEST_LENGTH; i++) {
        printf("%d\n", data_rd[i]);
        TEST_ASSERT(data_rd[i] == i);
    }
    free(data_rd);
    unity_send_signal("ready to delete");
    i2c_driver_delete(I2C_MASTER_NUM);
}

static void slave_write_buffer_test(void)
{
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    int size_rd;

    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("slave write");
    for (int i = 0; i < DATA_LENGTH / 2; i++) {
        data_wr[i] = i;
    }
    size_rd = i2c_slave_write_buffer(I2C_SLAVE_NUM, data_wr, RW_TEST_LENGTH, 2000 / portTICK_PERIOD_MS);
    disp_buf(data_wr, size_rd);
    unity_send_signal("master read");
    unity_wait_for_signal("ready to delete");
    free(data_wr);
    i2c_driver_delete(I2C_SLAVE_NUM);
}

TEST_CASE_MULTIPLE_DEVICES("I2C master read slave test", "[i2c][test_env=generic_multi_device][timeout=150]", master_read_slave_test, slave_write_buffer_test);

static void i2c_master_write_read_test(void)
{
    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    memset(data_rd, 0, DATA_LENGTH);
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);

    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));
    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    unity_wait_for_signal("i2c slave init finish");
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | READ_BIT, ACK_CHECK_EN);

    unity_send_signal("slave write");
    unity_wait_for_signal("master read and write");
    i2c_master_read(cmd, data_rd, RW_TEST_LENGTH, ACK_VAL);
    i2c_master_read_byte(cmd, data_rd + RW_TEST_LENGTH, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 5000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    disp_buf(data_rd, RW_TEST_LENGTH);
    for (int i = 0; i < RW_TEST_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i / 2);
    }

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i % 3;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
    i2c_master_write_slave(I2C_MASTER_NUM, data_wr, RW_TEST_LENGTH);
    free(data_wr);
    free(data_rd);
    unity_send_signal("slave read");
    unity_wait_for_signal("ready to delete");
    i2c_driver_delete(I2C_MASTER_NUM);
}

static void i2c_slave_read_write_test(void)
{
    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    memset(data_rd, 0, DATA_LENGTH);
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    int size_rd;

    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");
    unity_wait_for_signal("slave write");

    for (int i = 0; i < DATA_LENGTH / 2; i++) {
        data_wr[i] = i / 2;
    }
    size_rd = i2c_slave_write_buffer(I2C_SLAVE_NUM, data_wr, RW_TEST_LENGTH, 2000 / portTICK_PERIOD_MS);
    disp_buf(data_wr, size_rd);
    unity_send_signal("master read and write");
    unity_wait_for_signal("slave read");
    size_rd = i2c_slave_read_buffer(I2C_SLAVE_NUM, data_rd, RW_TEST_LENGTH, 1000 / portTICK_PERIOD_MS);
    printf("slave read data is:\n");
    disp_buf(data_rd, size_rd);
    for (int i = 0; i < RW_TEST_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i % 3);
    }
    free(data_wr);
    free(data_rd);
    unity_send_signal("ready to delete");
    i2c_driver_delete(I2C_SLAVE_NUM);
}

TEST_CASE_MULTIPLE_DEVICES("I2C read and write test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_read_test, i2c_slave_read_write_test);

static void i2c_master_repeat_write(void)
{
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    int times = 3;

    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));

    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    unity_wait_for_signal("i2c slave init finish");

    for (int j = 0; j < times; j++) {
        for (int i = 0; i < DATA_LENGTH; i++) {
            data_wr[i] = j + i;
        }
        i2c_master_write_slave(I2C_MASTER_NUM, data_wr, RW_TEST_LENGTH);
        disp_buf(data_wr, RW_TEST_LENGTH);
    }
    free(data_wr);
    unity_send_signal("master write");
    unity_wait_for_signal("ready to delete");
    i2c_driver_delete(I2C_MASTER_NUM);
}

static void i2c_slave_repeat_read(void)
{
    int size_rd = 0;
    int times = 3;
    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH * 3);

    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");
    unity_wait_for_signal("master write");

    while (1) {
        int len = i2c_slave_read_buffer(I2C_SLAVE_NUM, data_rd + size_rd, RW_TEST_LENGTH * 3, 10000 / portTICK_PERIOD_MS);
        if (len == 0) {
            break;
        }
        size_rd += len;
    }
    disp_buf(data_rd, size_rd);
    for (int j = 0; j < times; j++) {
        for (int i = 0; i < RW_TEST_LENGTH; i++) {
            printf("data: %d, %d\n", data_rd[j * RW_TEST_LENGTH + i], (i % 129 + j));
            TEST_ASSERT(data_rd[j * RW_TEST_LENGTH + i] == (i % 129 + j));
        }
    }
    free(data_rd);
    unity_send_signal("ready to delete");
    i2c_driver_delete(I2C_SLAVE_NUM);
}

TEST_CASE_MULTIPLE_DEVICES("I2C repeat write test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_repeat_write, i2c_slave_repeat_read);

#if SOC_HP_I2C_NUM > 1

static void i2c_master_write_test_more_ports(void)
{
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    int i;

    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_param_config(I2C_NUM_1, &conf_master));

    TEST_ESP_OK(i2c_driver_install(I2C_NUM_1, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH / 2; i++) {
        data_wr[i] = i;
    }
    i2c_master_write_slave(I2C_NUM_1, data_wr, DATA_LENGTH / 2);
    disp_buf(data_wr, i);
    free(data_wr);
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_driver_delete(I2C_NUM_1));
}

static void i2c_slave_read_test_more_ports(void)
{
    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    int size_rd = 0;
    int len = 0;

    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_param_config(I2C_NUM_1, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_NUM_1, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");
    while (1) {
        len = i2c_slave_read_buffer(I2C_NUM_1, data_rd + size_rd, DATA_LENGTH, 10000 / portTICK_PERIOD_MS);
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
    TEST_ESP_OK(i2c_driver_delete(I2C_NUM_1));
}

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test, more ports", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test_more_ports, i2c_slave_read_test_more_ports);

#endif

static volatile bool exit_flag;
static bool test_read_func;

static void test_task(void *pvParameters)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;

    uint8_t *data = (uint8_t *) malloc(DATA_LENGTH);
    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    TEST_ESP_OK(i2c_param_config(I2C_SLAVE_NUM, &conf_slave));
    while (exit_flag == false) {
        if (test_read_func) {
            i2c_slave_read_buffer(I2C_SLAVE_NUM, data, DATA_LENGTH, 0);
        } else {
            i2c_slave_write_buffer(I2C_SLAVE_NUM, data, DATA_LENGTH, 0);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    free(data);
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("test i2c_slave_read_buffer is not blocked when ticks_to_wait=0", "[i2c]")
{
    SemaphoreHandle_t exit_sema = xSemaphoreCreateBinary();
    exit_flag = false;

    test_read_func = true;
    xTaskCreate(test_task, "tsk1", 2048, &exit_sema, 5, NULL);

    printf("Waiting for 5 sec\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    exit_flag = true;
    if (xSemaphoreTake(exit_sema, 1000 / portTICK_PERIOD_MS) == pdTRUE) {
        vSemaphoreDelete(exit_sema);
    } else {
        TEST_FAIL_MESSAGE("i2c_slave_read_buffer is blocked");
    }
    vTaskDelay(2); // wait for task finish
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

TEST_CASE("test i2c_slave_write_buffer is not blocked when ticks_to_wait=0", "[i2c]")
{
    SemaphoreHandle_t exit_sema = xSemaphoreCreateBinary();
    exit_flag = false;

    test_read_func = false;
    xTaskCreate(test_task, "tsk1", 2048, &exit_sema, 5, NULL);

    printf("Waiting for 5 sec\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    exit_flag = true;
    if (xSemaphoreTake(exit_sema, 1000 / portTICK_PERIOD_MS) == pdTRUE) {
        vSemaphoreDelete(exit_sema);
    } else {
        TEST_FAIL_MESSAGE("i2c_slave_write_buffer is blocked");
    }
    vTaskDelay(2); // wait for task finish
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

TEST_CASE("I2C general API test", "[i2c]")
{
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define I2C_TEST_TIME 0x3ff
#else
#define I2C_TEST_TIME 0x1f
#endif
    const int i2c_num = 0;
    i2c_config_t conf_master = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
    };
    TEST_ESP_OK(i2c_param_config(i2c_num, &conf_master));
    int time_get0, time_get1;
    for (int i = 10; i < I2C_TEST_TIME; i++) {
        //set period test
        TEST_ESP_OK(i2c_set_period(i2c_num, i, i));
        TEST_ESP_OK(i2c_get_period(i2c_num, &time_get0, &time_get1));
        TEST_ASSERT((time_get0 == i) && (time_get1 == i));
        //set start timing test
        TEST_ESP_OK(i2c_set_start_timing(i2c_num, i, i));
        TEST_ESP_OK(i2c_get_start_timing(i2c_num, &time_get0, &time_get1));
        TEST_ASSERT((time_get0 == i) && (time_get1 == i));
        //set stop timing test
        TEST_ESP_OK(i2c_set_stop_timing(i2c_num, i, i));
        TEST_ESP_OK(i2c_get_stop_timing(i2c_num, &time_get0, &time_get1));
        TEST_ASSERT((time_get0 == i) && (time_get1 == i));
        //set data timing test
        TEST_ESP_OK(i2c_set_data_timing(i2c_num, i, i));
        TEST_ESP_OK(i2c_get_data_timing(i2c_num, &time_get0, &time_get1));
        TEST_ASSERT((time_get0 == i) && (time_get1 == i));
        //set time out test
        TEST_ESP_OK(i2c_set_timeout(i2c_num, i));
        TEST_ESP_OK(i2c_get_timeout(i2c_num, &time_get0));
        TEST_ASSERT(time_get0 == i);
    }
}

//Init uart baud rate detection
static void uart_aut_baud_det_init(int rxd_io_num)
{
    gpio_func_sel(rxd_io_num, PIN_FUNC_GPIO);
    gpio_set_direction(rxd_io_num, GPIO_MODE_INPUT_OUTPUT);
    esp_rom_gpio_connect_out_signal(rxd_io_num, i2c_periph_signal[0].scl_out_sig, 0, 0);
    esp_rom_gpio_connect_in_signal(rxd_io_num, UART_PERIPH_SIGNAL(1, SOC_UART_PERIPH_SIGNAL_RX), 0);
    PERIPH_RCC_ATOMIC() {
        uart_ll_enable_bus_clock(1, true);
        uart_ll_reset_register(1);
    }
    /* Reset all the bits */
    uart_ll_disable_intr_mask(&UART1, ~0);
    uart_ll_clr_intsts_mask(&UART1, ~0);
    uart_ll_set_autobaud_en(&UART1, true);
}

//Calculate I2C scl freq
static void i2c_scl_freq_cal(void)
{
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
    const int i2c_source_clk_freq = 80000000;
    const float i2c_cource_clk_period = 0.0125;
    int expt_cnt = 542;
#else
    const int i2c_source_clk_freq = 18000000; // Clock source: RTC
    const float i2c_cource_clk_period = 0.056;
    int expt_cnt = 540;
#endif
    int edg_cnt = uart_ll_get_rxd_edge_cnt(&UART1);
    int pospulse_cnt = uart_ll_get_pos_pulse_cnt(&UART1);
    int negpulse_cnt = uart_ll_get_neg_pulse_cnt(&UART1);
    int high_period_cnt = uart_ll_get_high_pulse_cnt(&UART1);
    int low_period_cnt = uart_ll_get_low_pulse_cnt(&UART1);
    if (edg_cnt != expt_cnt) {
        printf("\nedg_cnt != %d, test fail\n", expt_cnt);
        return;
    }
    printf("\nDetected SCL frequency: %d Hz\n", i2c_source_clk_freq / ((pospulse_cnt + negpulse_cnt) / 2));

    printf("\nSCL high period %.3f (us), SCL low_period %.3f (us)\n\n", (float)(i2c_cource_clk_period * high_period_cnt), (float)(i2c_cource_clk_period * low_period_cnt));
    uart_ll_set_autobaud_en(&UART1, false);
    PERIPH_RCC_ATOMIC() {
        uart_ll_enable_bus_clock(1, false);
    }
}

TEST_CASE("I2C SCL freq test (local test)", "[i2c][ignore]")
{
    //Use the UART baud rate detection function to detect the I2C SCL frequency.
    const int i2c_num = 0;
    const int uart1_rxd_io = 5;
    i2c_config_t conf_master = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
    };
    uint8_t *data = (uint8_t *)malloc(30);
    TEST_ESP_OK(i2c_param_config(i2c_num, &conf_master));
    TEST_ESP_OK(i2c_driver_install(i2c_num, I2C_MODE_MASTER, 0, 0, 0));
    memset(data, 0, 0);
    uart_aut_baud_det_init(uart1_rxd_io);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write(cmd, data, 30, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_num, cmd, 5000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    i2c_scl_freq_cal();
    free(data);
    TEST_ESP_OK(i2c_driver_delete(i2c_num));
}

#endif // SOC_I2C_SUPPORT_SLAVE
