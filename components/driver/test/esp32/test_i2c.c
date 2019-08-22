/**
 * test environment UT_T2_I2C:
 * please prepare two ESP32-WROVER-KIT board.
 * Then connect GPIO18 and GPIO18, GPIO19 and GPIO19 between these two boards.
 */
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "test_utils.h"
#include "unity_config.h"
#include "driver/i2c.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_periph.h"
#include "soc/i2c_periph.h"
#include "esp_system.h"
#include "driver/pcnt.h"


#define DATA_LENGTH          512  /*!<Data buffer length for test buffer*/
#define RW_TEST_LENGTH       129  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/
#define DELAY_TIME_BETWEEN_ITEMS_MS   1234 /*!< delay time between different test items */

#define I2C_SLAVE_SCL_IO     19    /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     18    /*!<gpio number for i2c slave data */
#define I2C_SLAVE_NUM I2C_NUM_0    /*!<I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave rx buffer size */

#define I2C_MASTER_SCL_IO    19    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    18    /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1   /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ    100000     /*!< I2C master clock frequency */

#define ESP_SLAVE_ADDR 0x28         /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT  I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT   I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL    0x0         /*!< I2C ack value */
#define NACK_VAL   0x1         /*!< I2C nack value */

#define PULSE_IO 19
#define PCNT_INPUT_IO 4
#define PCNT_CTRL_FLOATING_IO 5
#define HIGHEST_LIMIT 10000
#define LOWEST_LIMIT -10000


static esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    TEST_ESP_OK(i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN));
    TEST_ESP_OK(i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN));
    TEST_ESP_OK(i2c_master_stop(cmd));
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 5000 / portTICK_RATE_MS);
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
    };
    return conf_master;
}

// print the reading buffer
static void disp_buf(uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if (( i + 1 ) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
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
    TEST_ESP_OK(i2c_param_config( I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");
    while (1) {
        len = i2c_slave_read_buffer( I2C_SLAVE_NUM, data_rd + size_rd, DATA_LENGTH, 10000 / portTICK_RATE_MS);
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

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test", "[i2c][test_env=UT_T2_I2C][timeout=150]", i2c_master_write_test, i2c_slave_read_test);

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
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);

    unity_send_signal("slave write");
    unity_wait_for_signal("master read");
    i2c_master_read(cmd, data_rd, RW_TEST_LENGTH-1, ACK_VAL);
    i2c_master_read_byte(cmd, data_rd + RW_TEST_LENGTH-1, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 5000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(100 / portTICK_RATE_MS);
    for (int i = 0; i < RW_TEST_LENGTH; i++) {
        printf("%d\n", data_rd[i]);
        TEST_ASSERT(data_rd[i]==i);
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
    TEST_ESP_OK(i2c_param_config( I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("slave write");
    for (int i = 0; i < DATA_LENGTH / 2; i++) {
        data_wr[i] = i;
    }
    size_rd = i2c_slave_write_buffer(I2C_SLAVE_NUM, data_wr, RW_TEST_LENGTH, 2000 / portTICK_RATE_MS);
    disp_buf(data_wr, size_rd);
    unity_send_signal("master read");
    unity_wait_for_signal("ready to delete");
    free(data_wr);
    i2c_driver_delete(I2C_SLAVE_NUM);
}


TEST_CASE_MULTIPLE_DEVICES("I2C master read slave test", "[i2c][test_env=UT_T2_I2C][timeout=150]", master_read_slave_test, slave_write_buffer_test);

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
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);

    unity_send_signal("slave write");
    unity_wait_for_signal("master read and write");
    i2c_master_read(cmd, data_rd, RW_TEST_LENGTH, ACK_VAL);
    i2c_master_read_byte(cmd, data_rd + RW_TEST_LENGTH, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 5000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(100 / portTICK_RATE_MS);
    disp_buf(data_rd, RW_TEST_LENGTH);
    for (int i = 0; i < RW_TEST_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i/2);
    }

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i % 3;
    }

    vTaskDelay(100 / portTICK_RATE_MS);
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
    TEST_ESP_OK(i2c_param_config( I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");
    unity_wait_for_signal("slave write");

    for (int i = 0; i < DATA_LENGTH / 2; i++) {
        data_wr[i] = i/2;
    }
    size_rd = i2c_slave_write_buffer(I2C_SLAVE_NUM, data_wr, RW_TEST_LENGTH, 2000 / portTICK_RATE_MS);
    disp_buf(data_wr, size_rd);
    unity_send_signal("master read and write");
    unity_wait_for_signal("slave read");
    size_rd = i2c_slave_read_buffer( I2C_SLAVE_NUM, data_rd, RW_TEST_LENGTH, 1000 / portTICK_RATE_MS);
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

TEST_CASE_MULTIPLE_DEVICES("I2C read and write test", "[i2c][test_env=UT_T2_I2C][timeout=150]", i2c_master_write_read_test, i2c_slave_read_write_test);

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
    TEST_ESP_OK(i2c_param_config( I2C_SLAVE_NUM, &conf_slave));
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    unity_send_signal("i2c slave init finish");
    unity_wait_for_signal("master write");

    while (1) {
        int len = i2c_slave_read_buffer( I2C_SLAVE_NUM, data_rd + size_rd, RW_TEST_LENGTH * 3, 10000 / portTICK_RATE_MS);
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

TEST_CASE_MULTIPLE_DEVICES("I2C repeat write test", "[i2c][test_env=UT_T2_I2C][timeout=150]", i2c_master_repeat_write, i2c_slave_repeat_read);