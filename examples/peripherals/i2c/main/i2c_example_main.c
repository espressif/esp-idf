/* i2c - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "driver/i2c.h"

/**
 * TEST CODE BRIEF
 *
 * This example will show you how to use I2C module by running two tasks on i2c bus:
 *
 * - read external i2c sensor, here we use a BH1750 light sensor(GY-30 module) for instance.
 * - Use one I2C port(master mode) to read or write the other I2C port(slave mode) on one ESP32 chip.
 *
 * Pin assignment:
 *
 * - slave :
 *    GPIO25 is assigned as the data signal of i2c slave port
 *    GPIO26 is assigned as the clock signal of i2c slave port
 * - master:
 *    GPIO18 is assigned as the data signal of i2c master port
 *    GPIO19 is assigned as the clock signal of i2c master port
 *
 * Connection:
 *
 * - connect GPIO18 with GPIO25
 * - connect GPIO19 with GPIO26
 * - connect sda/scl of sensor with GPIO18/GPIO19
 * - no need to add external pull-up resistors, driver will enable internal pull-up resistors.
 *
 * Test items:
 *
 * - read the sensor data, if connected.
 * - i2c master(ESP32) will write data to i2c slave(ESP32).
 * - i2c master(ESP32) will read data from i2c slave(ESP32).
 */

#define DELAY_TIME_BETWEEN_ITEMS_MS        3000             /*!< delay time between different test items in ms */

#define I2C_EXAMPLE_SLAVE_SCL_IO           26               /*!<gpio number for i2c slave clock  */
#define I2C_EXAMPLE_SLAVE_SDA_IO           25               /*!<gpio number for i2c slave data */
#define I2C_EXAMPLE_SLAVE_NUM              I2C_NUM_0        /*!<I2C port number for slave dev */
#define I2C_EXAMPLE_SLAVE_BUF_LEN          256              /*!<I2C slave buffer size, 128-4096 bytes */
#define I2C_EXAMPLE_SLAVE_RO_LEN           16               /*!<I2C slave read-only size */

#define I2C_EXAMPLE_MASTER_SCL_IO          19               /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO          18               /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_NUM             I2C_NUM_1        /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_BUF_DISABLE     0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RO_DISABLE      0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_FREQ_HZ         100000           /*!< I2C master clock frequency */

#define BH1750_SENSOR_ADDR                 0x23             /*!< slave address for BH1750 sensor */
#define BH1750_CMD_START                   0x23             /*!< Command to set measure mode */
#define ESP_SLAVE_ADDR                     0x28             /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */

SemaphoreHandle_t print_mux = NULL;
static TaskHandle_t i2c_slave_task_handle = NULL;
static i2c_slave_state_t slave_state;


static void i2c_slave_task(void* arg)
{
    uint32_t notify_val = 0;
    int notify_res = 0;
    int rdlen;
    uint8_t *data;

    printf("[SLV_TASK] Started\n");
    if (i2c_slave_add_task(I2C_EXAMPLE_SLAVE_NUM, &i2c_slave_task_handle, &slave_state) != ESP_OK) {
        printf("[SLV_TASK] Error registering slave task\n");
        goto exit;
    }

    while (1) {
        // === Wait for notification from I2C interrupt routine ===
        notify_val = 0;
        notify_res = xTaskNotifyWait(0, ULONG_MAX, &notify_val, 1000 / portTICK_RATE_MS);
        if (notify_res != pdPASS) continue;

        // notification received
        // Check if task exit requested
        if (notify_val == I2C_SLAVE_DRIVER_DELETED) {
            printf("[SLV_TASK] i2c driver deleted\n");
            break;
        }

        data = NULL;
        rdlen = 0;
        if ((slave_state.rxptr == 0) && (slave_state.txptr == 0)) {
            // address received from master
            printf("[SLV_TASK] Address set by master: %d\n", slave_state.rxaddr);
        }
        else if (slave_state.status & 0x02) {
            // read transaction, data sent to master
            if (slave_state.txptr) {
                data = malloc(slave_state.txptr+1);
                if (data) {
                    rdlen = i2c_slave_read_buffer(I2C_EXAMPLE_SLAVE_NUM, data, slave_state.txaddr, slave_state.txptr, 200 / portTICK_RATE_MS);
                    if (rdlen != slave_state.txptr) {
                        free(data);
                        data = NULL;
                    }
                    data[slave_state.txptr] = 0;
                }
            }
            printf("[SLV_TASK] Data sent to master: address=%d, length=%d, overflow=%d\n", slave_state.txaddr, slave_state.txptr, slave_state.txovf);
            if (data) {
                printf("  Data: [%s]\n", data);
                free(data);
            }
            else printf("  [No data]\n");
        }
        else {
            // write transaction, data received from master
            if (slave_state.rxptr) {
                data = malloc(slave_state.rxptr+1);
                if (data) {
                    rdlen = i2c_slave_read_buffer(I2C_EXAMPLE_SLAVE_NUM, data, slave_state.rxaddr, slave_state.rxptr, 200 / portTICK_RATE_MS);
                    if (rdlen != slave_state.rxptr) {
                        free(data);
                        data = NULL;
                    }
                    data[slave_state.rxptr] = 0;
                }
            }
            printf("[SLV_TASK] Data received from master: address=%d, length=%d, overflow=%d\n", slave_state.rxaddr, slave_state.rxptr, slave_state.rxovf);
            if (data) {
                printf("  Data: [%s]\n", data);
                free(data);
            }
            else printf("  [No data]\n");
        }
    }

exit:
    i2c_slave_remove_task(I2C_EXAMPLE_SLAVE_NUM);
    printf("[SLV_TASK] Deleted\n");
    i2c_slave_task_handle = NULL;
    vTaskDelete(NULL);
}


/**
 * @brief test code to read from esp-i2c-slave
 *        Master device reads data from slave(both esp32),
 *        the data will be read from slave buffer at given address.
 *        We can set the slave buffer data using i2c_slave_write_buffer() function.
 *
 * For slave buffer size <= 256 8-bit addressing is used
 * _________________________________________________________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | buff_addr + ack | start | slave_addr + rd_bit + ack | read n-1 bytes + ack | read n-th byte + nack | stop |
 * --------|---------------------------|-----------------|-------|---------------------------|----------------------|-----------------------|------|
 *
 * For slave buffer size > 256 16-bit addressing is used
 * _________________________________________________________________________________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | buff_addr_hi + ack | buff_addr_lo + ack | start | slave_addr + rd_bit + ack | read n-1 bytes + ack | read n-th byte + nack | stop |
 * --------|---------------------------|--------------------|--------------------|-------|---------------------------|---------  -----------|-----------------------|------|
 *
 */
static esp_err_t i2c_example_master_read_slave(i2c_port_t i2c_num, uint16_t addr, uint8_t* data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    // send slave buffer address, 1 or 2 bytes
    if (I2C_EXAMPLE_SLAVE_BUF_LEN > 256) {
        // use 16-bit addressing, send MSByte first
        i2c_master_write_byte(cmd, (uint8_t)(addr>>8), ACK_CHECK_EN);
        i2c_master_write_byte(cmd, (uint8_t)addr, ACK_CHECK_EN);
    }
    else {
        // use 8-bit addressing
        i2c_master_write_byte(cmd, (uint8_t)addr, ACK_CHECK_EN);
    }
    // repeated-start, switch to read mode
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);
    // read data
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Test code to write to esp-i2c-slave
 *        Master device writes data to slave(both esp32),
 *        the data will be stored in slave buffer at given address.
 *        We can read them out from slave buffer.
 *
 * For slave buffer size <= 256 8-bit addressing is used
 * _____________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | buff_addr + ack | write n bytes + ack  | stop |
 * --------|---------------------------|-----------------|----------------------|------|
 *
 * For slave buffer size > 256 16-bit addressing is used
 * _____________________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | buff_addr_hi + ack | buff_addr_lo + ack | write n bytes + ack  | stop |
 * --------|---------------------------|--------------------|--------------------|----------------------|------|
 *
 */
static esp_err_t i2c_example_master_write_slave(i2c_port_t i2c_num, uint16_t addr, uint8_t* data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    // send slave buffer address
    if (I2C_EXAMPLE_SLAVE_BUF_LEN > 256) {
        // use 16-bit addressing, send MSByte first
        i2c_master_write_byte(cmd, (uint8_t)(addr>>8), ACK_CHECK_EN);
        i2c_master_write_byte(cmd, (uint8_t)addr, ACK_CHECK_EN);
    }
    else {
        // use 8-bit addressing
        i2c_master_write_byte(cmd, (uint8_t)addr, ACK_CHECK_EN);
    }
    // send data
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief test code to read from esp-i2c-slave
 *
 * 1. set mode
 * _________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte + ack  | stop |
 * --------|---------------------------|---------------------|------|
 * 2. wait more than 24 ms
 * 3. read data
 * ______________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | read 1 byte + ack  | read 1 byte + nack | stop |
 * --------|---------------------------|--------------------|--------------------|------|
 */
static esp_err_t i2c_example_master_sensor_test(i2c_port_t i2c_num, uint8_t* data_h, uint8_t* data_l)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BH1750_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, BH1750_CMD_START, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }
    vTaskDelay(30 / portTICK_RATE_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BH1750_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data_h, ACK_VAL);
    i2c_master_read_byte(cmd, data_l, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief i2c master initialization
 */
static void i2c_example_master_init()
{
    int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_EXAMPLE_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_EXAMPLE_MASTER_BUF_DISABLE,
                       I2C_EXAMPLE_MASTER_RO_DISABLE, 0);
}

/**
 * @brief i2c slave initialization
 */
static void i2c_example_slave_init()
{
    int i2c_slave_port = I2C_EXAMPLE_SLAVE_NUM;
    i2c_config_t conf_slave;
    conf_slave.sda_io_num = I2C_EXAMPLE_SLAVE_SDA_IO;
    conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.scl_io_num = I2C_EXAMPLE_SLAVE_SCL_IO;
    conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.mode = I2C_MODE_SLAVE;
    conf_slave.slave.addr_10bit_en = 0;
    conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;
    i2c_param_config(i2c_slave_port, &conf_slave);
    i2c_driver_install(i2c_slave_port, conf_slave.mode,
                       I2C_EXAMPLE_SLAVE_BUF_LEN,
                       I2C_EXAMPLE_SLAVE_RO_LEN, 0);
    // Create the slave task
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 1024 * 2, (void* ) 0, 12, &i2c_slave_task_handle);
}

static void i2c_test_task(void* arg)
{
    int ret;
    uint32_t task_idx = (uint32_t) arg;
    char data[128] = {'\0'};
    uint8_t sensor_data_h, sensor_data_l;
    int cnt = 0;
    while (1) {
        cnt++;

        //=== Sensor test ===========================
        ret = i2c_example_master_sensor_test( I2C_EXAMPLE_MASTER_NUM, &sensor_data_h, &sensor_data_l);
        xSemaphoreTake(print_mux, portMAX_DELAY);
        printf("*************************************\n");

        printf("TASK[%d]  MASTER READ SENSOR( BH1750 )\n", task_idx);
        printf("*************************************\n");
        if(ret == ESP_ERR_TIMEOUT) {
            printf("I2C timeout\n");
            printf("*************************************\n");
        } else if(ret == ESP_OK) {
            printf("data_h: %02x\n", sensor_data_h);
            printf("data_l: %02x\n", sensor_data_l);
            printf("sensor val: %f\n", (sensor_data_h << 8 | sensor_data_l) / 1.2);
        } else {
            printf("%s: No ack, sensor not connected...skip...\n", esp_err_to_name(ret));
        }
        printf("*************************************\n\n");
        xSemaphoreGive(print_mux);

        vTaskDelay(( DELAY_TIME_BETWEEN_ITEMS_MS * ( task_idx + 1 ) ) / portTICK_RATE_MS);

        //=== Slave read test ===========================
        i2c_slave_set_buffer(I2C_EXAMPLE_SLAVE_NUM, 0, 500 / portTICK_RATE_MS);
        // Set some data for master to read
        sprintf(data, "Master read: count=%d, task_id=%d", cnt, task_idx);
        i2c_slave_write_buffer(I2C_EXAMPLE_SLAVE_NUM, (uint8_t *)data, 0, strlen(data)+1, 500 / portTICK_RATE_MS);

        xSemaphoreTake(print_mux, portMAX_DELAY);
        printf("*******************************\n");
        printf("TASK[%d]  MASTER READ FROM SLAVE\n", task_idx);
        printf("*******************************\n");
        ret = i2c_example_master_read_slave(I2C_EXAMPLE_MASTER_NUM, 0, (uint8_t *)data, strlen(data)+1);
        vTaskDelay(100 / portTICK_RATE_MS);

        if (ret == ESP_ERR_TIMEOUT) {
            printf("Master read timeout\n");
        } else if (ret == ESP_OK) {
            printf("Master read OK\n  data: [");
            for (int i=0; i<strlen(data)+1; i++) {
                if (i>0) printf(" ");
                printf("%2X", data[i]);
            }
            printf("]\n");
        } else {
            printf("%s: Master read slave error, IO not connected...\n", esp_err_to_name(ret));
        }
        printf("*******************************\n\n");
        xSemaphoreGive(print_mux);

        vTaskDelay(( DELAY_TIME_BETWEEN_ITEMS_MS * ( task_idx + 1 ) ) / portTICK_RATE_MS);

        //=== Slave read test (with overflow) ===========
        i2c_slave_set_buffer(I2C_EXAMPLE_SLAVE_NUM, 0, 500 / portTICK_RATE_MS);
        i2c_slave_write_buffer(I2C_EXAMPLE_SLAVE_NUM, (uint8_t *)"1234567890abcdef", I2C_EXAMPLE_SLAVE_BUF_LEN-16, 16, 500 / portTICK_RATE_MS);

        xSemaphoreTake(print_mux, portMAX_DELAY);
        printf("******************************************\n");
        printf("TASK[%d]  MASTER READ FROM SLAVE (OVERFLOW)\n", task_idx);
        printf("******************************************\n");
        ret = i2c_example_master_read_slave(I2C_EXAMPLE_MASTER_NUM, I2C_EXAMPLE_SLAVE_BUF_LEN-16, (uint8_t *)data, 32);
        vTaskDelay(100 / portTICK_RATE_MS);

        if (ret == ESP_ERR_TIMEOUT) {
            printf("Master read timeout\n");
        } else if (ret == ESP_OK) {
            printf("Master read OK\n  data: [");
            for (int i=0; i<32; i++) {
                if (i>0) printf(" ");
                printf("%2X", data[i]);
            }
            printf("]\n");
        } else {
            printf("%s: Master read slave error, IO not connected...\n", esp_err_to_name(ret));
        }
        printf("******************************************\n\n");
        xSemaphoreGive(print_mux);

        vTaskDelay(( DELAY_TIME_BETWEEN_ITEMS_MS * ( task_idx + 1 ) ) / portTICK_RATE_MS);

        //=== Slave write test ===========================
        // Set the data to be sent to the slave by master
        sprintf(data, "Master write: count=%d, task_id=%d", cnt, task_idx);
        i2c_slave_write_buffer(I2C_EXAMPLE_SLAVE_NUM, (uint8_t *)data, 128, strlen(data)+1, 500 / portTICK_RATE_MS);

        xSemaphoreTake(print_mux, portMAX_DELAY);
        printf("******************************\n");
        printf("TASK[%d]  MASTER WRITE TO SLAVE\n", task_idx);
        printf("******************************\n");

        ret = i2c_example_master_write_slave( I2C_EXAMPLE_MASTER_NUM, 128, (uint8_t *)data, strlen(data)+1);
        vTaskDelay(100 / portTICK_RATE_MS);

        if (ret == ESP_ERR_TIMEOUT) {
            printf("Master write timeout\n");
        } else if (ret == ESP_OK) {
            printf("Master write OK\n");
        } else {
            printf("%s: Master write slave error, IO not connected....\n", esp_err_to_name(ret));
        }
        printf("******************************\n\n");
        xSemaphoreGive(print_mux);

        vTaskDelay(( DELAY_TIME_BETWEEN_ITEMS_MS * ( task_idx + 1 ) ) / portTICK_RATE_MS);
    }
}

void app_main()
{
    print_mux = xSemaphoreCreateMutex();
    i2c_example_slave_init();
    i2c_example_master_init();
    xTaskCreate(i2c_test_task, "i2c_test_task_0", 1024 * 2, (void* ) 0, 10, NULL);
    xTaskCreate(i2c_test_task, "i2c_test_task_1", 1024 * 2, (void* ) 1, 10, NULL);

}

