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
#include "soc/uart_struct.h"
#include "driver/periph_ctrl.h"


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

static DRAM_ATTR i2c_dev_t *const I2C[I2C_NUM_MAX] = { &I2C0, &I2C1 };

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
    i2c_config_t conf_slave = i2c_slave_init();
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            conf_slave.sda_pullup_en = sda_pull_up_en[i];
            conf_slave.scl_pullup_en = scl_pull_up_en[j];
            TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                           I2C_SLAVE_RX_BUF_LEN,
                                           I2C_SLAVE_TX_BUF_LEN, 0));
            TEST_ESP_OK(i2c_param_config( I2C_SLAVE_NUM, &conf_slave));
            TEST_ASSERT_EQUAL_INT32(I2C[I2C_SLAVE_NUM] -> ctr.ms_mode, 0);
            TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
        }
    }


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

TEST_CASE("I2C config FIFO test", "[i2c]")
{
    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                           I2C_SLAVE_RX_BUF_LEN,
                                           I2C_SLAVE_TX_BUF_LEN, 0));
    TEST_ESP_OK(i2c_param_config( I2C_SLAVE_NUM, &conf_slave));
    TEST_ASSERT_BIT_LOW(1, I2C[I2C_SLAVE_NUM]->fifo_conf.tx_fifo_rst);
    TEST_ESP_OK(i2c_reset_tx_fifo(I2C_SLAVE_NUM));
    TEST_ASSERT_BIT_LOW(0, I2C[I2C_SLAVE_NUM]->fifo_conf.tx_fifo_rst);

    TEST_ESP_OK(i2c_reset_rx_fifo(I2C_SLAVE_NUM));
    TEST_ASSERT_BIT_LOW(0, I2C[I2C_SLAVE_NUM]->fifo_conf.rx_fifo_rst);
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

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

TEST_CASE("I2C data mode test", "[i2c]")
{
    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);
    i2c_trans_mode_t test_tx_trans_mode, test_rx_trans_mode;
    i2c_config_t conf_master = i2c_master_init();
    TEST_ESP_OK(i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,
                                   I2C_MASTER_RX_BUF_DISABLE,
                                   I2C_MASTER_TX_BUF_DISABLE, 0));
    TEST_ESP_OK(i2c_param_config(I2C_MASTER_NUM, &conf_master));
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(i2c_set_data_mode(I2C_MASTER_NUM, I2C_DATA_MODE_LSB_FIRST, I2C_DATA_MODE_LSB_FIRST));
    TEST_ESP_OK(i2c_get_data_mode(I2C_MASTER_NUM, &test_tx_trans_mode, &test_rx_trans_mode));
    TEST_ASSERT_EQUAL_INT(I2C_DATA_MODE_LSB_FIRST, test_tx_trans_mode);
    TEST_ASSERT_EQUAL_INT(I2C_DATA_MODE_LSB_FIRST, test_rx_trans_mode);
    i2c_master_write_slave(I2C_MASTER_NUM, data_wr, RW_TEST_LENGTH);
    TEST_ESP_OK(i2c_set_data_mode(I2C_MASTER_NUM, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST));
    TEST_ESP_OK(i2c_get_data_mode(I2C_MASTER_NUM, &test_tx_trans_mode, &test_rx_trans_mode));
    TEST_ASSERT_EQUAL_INT(I2C_DATA_MODE_MSB_FIRST, test_tx_trans_mode);
    TEST_ASSERT_EQUAL_INT(I2C_DATA_MODE_MSB_FIRST, test_rx_trans_mode);
    i2c_master_write_slave(I2C_MASTER_NUM, data_wr, RW_TEST_LENGTH);
    free(data_wr);
    i2c_driver_delete(I2C_MASTER_NUM);
}


TEST_CASE("I2C driver memory leaking check", "[i2c]")
{
    esp_err_t ret;

    int size = esp_get_free_heap_size();
    for (uint32_t i = 0; i <= 1000; i++) {
        ret = i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                 I2C_SLAVE_RX_BUF_LEN,
                                 I2C_SLAVE_TX_BUF_LEN, 0);
        TEST_ASSERT(ret == ESP_OK);
        vTaskDelay(10 / portTICK_RATE_MS);
        i2c_driver_delete(I2C_SLAVE_NUM);
        TEST_ASSERT(ret == ESP_OK);
    }

    TEST_ASSERT_INT_WITHIN(100, size, esp_get_free_heap_size());
}

static volatile bool exit_flag;
static bool test_read_func;

static void test_task(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;

    uint8_t *data = (uint8_t *) malloc(DATA_LENGTH);
    i2c_config_t conf_slave = i2c_slave_init();
    TEST_ESP_OK(i2c_driver_install(I2C_SLAVE_NUM, I2C_MODE_SLAVE,
                                   I2C_SLAVE_RX_BUF_LEN,
                                   I2C_SLAVE_TX_BUF_LEN, 0));
    TEST_ESP_OK(i2c_param_config( I2C_SLAVE_NUM, &conf_slave));
    while (exit_flag == false) {
        if (test_read_func) {
            i2c_slave_read_buffer(I2C_SLAVE_NUM, data, DATA_LENGTH, 0);
        } else {
            i2c_slave_write_buffer(I2C_SLAVE_NUM, data, DATA_LENGTH, 0);
        }
        vTaskDelay(10/portTICK_RATE_MS);
    }

    free(data);
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("test i2c_slave_read_buffer is not blocked when ticks_to_wait=0", "[i2c]")
{
    xSemaphoreHandle exit_sema = xSemaphoreCreateBinary();
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
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

TEST_CASE("test i2c_slave_write_buffer is not blocked when ticks_to_wait=0", "[i2c]")
{
    xSemaphoreHandle exit_sema = xSemaphoreCreateBinary();
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
    TEST_ESP_OK(i2c_driver_delete(I2C_SLAVE_NUM));
}

TEST_CASE("I2C general API test", "[i2c]")
{
    const int i2c_num = 1;
    i2c_config_t conf_master = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
    };
    TEST_ESP_OK(i2c_param_config( i2c_num, &conf_master));
    int time_get0, time_get1;
    for(int i = 10; i < 0x3ff; i++) {
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
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[rxd_io_num], PIN_FUNC_GPIO);
    gpio_set_direction(rxd_io_num, GPIO_MODE_INPUT_OUTPUT);
    gpio_matrix_out(rxd_io_num, I2CEXT1_SCL_OUT_IDX, 0, 0);
    gpio_matrix_in(rxd_io_num, U1RXD_IN_IDX, 0);
    periph_module_enable(PERIPH_UART1_MODULE);
    UART1.int_ena.val = 0;
    UART1.int_clr.val = ~0;
    UART1.auto_baud.en = 1;
}

//Calculate I2C scl freq
static void i2c_scl_freq_cal(void)
{
    const int i2c_source_clk_freq = 80000000;
    const float i2c_cource_clk_period = 0.0125;
    int edg_cnt = UART1.rxd_cnt.edge_cnt;
    int pospulse_cnt =  UART1.pospulse.min_cnt;
    int negpulse_cnt =  UART1.negpulse.min_cnt;
    int high_period_cnt =  UART1.highpulse.min_cnt;
    int low_period_cnt =  UART1.lowpulse.min_cnt;
    if(edg_cnt != 542) {
        printf("\nedg_cnt != 542, test fail\n");
        return;
    }
    printf("\nDetected SCL frequency: %d Hz\n", i2c_source_clk_freq / ((pospulse_cnt + negpulse_cnt) / 2) );

    printf("\nSCL high period %.3f (us), SCL low_period %.3f (us)\n\n", (float)(i2c_cource_clk_period * high_period_cnt), (float)(i2c_cource_clk_period * low_period_cnt));
    UART1.auto_baud.en = 0;
    periph_module_disable(PERIPH_UART1_MODULE);
}

TEST_CASE("I2C SCL freq test (local test)", "[i2c][ignore]")
{
    //Use the UART baud rate detection function to detect the I2C SCL frequency.
    const int i2c_num = 1;
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
    TEST_ESP_OK(i2c_param_config( i2c_num, &conf_master));
    TEST_ESP_OK(i2c_driver_install(i2c_num, I2C_MODE_MASTER, 0, 0, 0));
    memset(data, 0, 0);
    uart_aut_baud_det_init(uart1_rxd_io);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write(cmd, data, 30, ACK_CHECK_DIS);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_num, cmd, 5000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    i2c_scl_freq_cal();
    free(data);
    TEST_ESP_OK(i2c_driver_delete(i2c_num));
}