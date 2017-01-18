// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_I2C_H_
#define _DRIVER_I2C_H_


#ifdef __cplusplus
extern "C" {
#endif
#include <esp_types.h>
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "driver/gpio.h"

#define I2C_APB_CLK_FREQ  APB_CLK_FREQ /*!< I2C source clock is APB clock, 80MHz */
#define I2C_FIFO_LEN   (32)  /*!< I2C hardware fifo length */
typedef enum{
    I2C_MODE_SLAVE = 0,   /*!< I2C slave mode */
    I2C_MODE_MASTER,      /*!< I2C master mode */
    I2C_MODE_MAX,
}i2c_mode_t;

typedef enum {
    I2C_MASTER_WRITE = 0,   /*!< I2C write data */
    I2C_MASTER_READ,        /*!< I2C read data */
} i2c_rw_t;

typedef enum {
    I2C_DATA_MODE_MSB_FIRST = 0,  /*!< I2C data msb first */
    I2C_DATA_MODE_LSB_FIRST = 1,  /*!< I2C data lsb first */
    I2C_DATA_MODE_MAX
} i2c_trans_mode_t;

typedef enum{
    I2C_CMD_RESTART = 0,   /*!<I2C restart command */
    I2C_CMD_WRITE,         /*!<I2C write command */
    I2C_CMD_READ,          /*!<I2C read command */
    I2C_CMD_STOP,          /*!<I2C stop command */
    I2C_CMD_END            /*!<I2C end command */
}i2c_opmode_t;

typedef enum{
    I2C_NUM_0 = 0,  /*!< I2C port 0 */
    I2C_NUM_1 ,     /*!< I2C port 1 */
    I2C_NUM_MAX
} i2c_port_t;

typedef enum {
    I2C_ADDR_BIT_7 = 0,    /*!< I2C 7bit address for slave mode */
    I2C_ADDR_BIT_10,       /*!< I2C 10bit address for slave mode */
    I2C_ADDR_BIT_MAX,
} i2c_addr_mode_t;

/**
 * @brief I2C initialization parameters
 */
typedef struct{
    i2c_mode_t mode;       /*!< I2C mode */
    gpio_num_t sda_io_num;        /*!< GPIO number for I2C sda signal */
    gpio_pullup_t sda_pullup_en;  /*!< Internal GPIO pull mode for I2C sda signal*/
    gpio_num_t scl_io_num;        /*!< GPIO number for I2C scl signal */
    gpio_pullup_t scl_pullup_en;  /*!< Internal GPIO pull mode for I2C scl signal*/

    union {
        struct {
            uint32_t clk_speed;     /*!< I2C clock frequency for master mode, (no higher than 1MHz for now) */
        } master;
        struct {
            uint8_t addr_10bit_en;  /*!< I2C 10bit address mode enable for slave mode */
            uint16_t slave_addr;    /*!< I2C address for slave mode */
        } slave;

    };
}i2c_config_t;

typedef void* i2c_cmd_handle_t;    /*!< I2C command handle  */

/**
 * @brief I2C driver install
 *
 * @param i2c_num I2C port number
 * @param mode I2C mode( master or slave )
 * @param slv_rx_buf_len receiving buffer size for slave mode
 *        @note
 *        Only slave mode will use this value, driver will ignore this value in master mode.
 * @param slv_tx_buf_len sending buffer size for slave mode
 *        @note
 *        Only slave mode will use this value, driver will ignore this value in master mode.
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Driver install error
 */
esp_err_t i2c_driver_install(i2c_port_t i2c_num, i2c_mode_t mode, size_t slv_rx_buf_len, size_t slv_tx_buf_len, int intr_alloc_flags);

/**
 * @brief I2C driver delete
 *
 * @param i2c_num I2C port number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_driver_delete(i2c_port_t i2c_num);

/**
 * @brief I2C parameter initialization
 *
 * @param i2c_num I2C port number
 * @param i2c_conf pointer to I2C parameter settings
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_param_config(i2c_port_t i2c_num, i2c_config_t* i2c_conf);

/**
 * @brief reset I2C tx hardware fifo
 *
 * @param i2c_num I2C port number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_reset_tx_fifo(i2c_port_t i2c_num);

/**
 * @brief reset I2C rx fifo
 *
 * @param i2c_num I2C port number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_reset_rx_fifo(i2c_port_t i2c_num);

/**
 * @brief I2C isr handler register
 *
 * @param i2c_num I2C port number
 * @param fn isr handler function
 * @param arg parameter for isr handler function
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 * @param handle handle return from esp_intr_alloc.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_isr_register(i2c_port_t i2c_num, void (*fn)(void*), void * arg, int intr_alloc_flags, intr_handle_t *handle);

/**
 * @brief to delete and free I2C isr.
 *
 * @param handle handle of isr.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_isr_free(intr_handle_t handle);

/**
 * @brief Configure GPIO signal for I2C sck and sda
 *
 * @param i2c_num I2C port number
 * @param sda_io_num GPIO number for I2C sda signal
 * @param scl_io_num GPIO number for I2C scl signal
 * @param sda_pullup_en Whether to enable the internal pullup for sda pin
 * @param scl_pullup_en Whether to enable the internal pullup for scl pin
 * @param mode I2C mode
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_set_pin(i2c_port_t i2c_num, gpio_num_t sda_io_num, gpio_num_t scl_io_num,
                      gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, i2c_mode_t mode);

/**
 * @brief Create and init I2C command link
 *        @note
 *        Before we build I2C command link, we need to call i2c_cmd_link_create() to create
 *        a command link.
 *        After we finish sending the commands, we need to call i2c_cmd_link_delete() to
 *        release and return the resources.
 *
 * @return i2c command link handler
 */
i2c_cmd_handle_t i2c_cmd_link_create();

/**
 * @brief Free I2C command link
 *        @note
 *        Before we build I2C command link, we need to call i2c_cmd_link_create() to create
 *        a command link.
 *        After we finish sending the commands, we need to call i2c_cmd_link_delete() to
 *        release and return the resources.
 *
 * @param cmd_handle I2C command handle
 */
void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle);

/**
 * @brief Queue command for I2C master to generate a start signal
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle);

/**
 * @brief Queue command for I2C master to write one byte to I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data I2C one byte command to write to bus
 * @param ack_en enable ack check for master
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en);

/**
 * @brief Queue command for I2C master to write buffer to I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data data to send
 * @param data_len data length
 * @param ack_en enable ack check for master
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, bool ack_en);

/**
 * @brief Queue command for I2C master to read one byte from I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data pointer accept the data byte
 * @param ack ack value for read command
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t* data, int ack);

/**
 * @brief Queue command for I2C master to read data from I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data data buffer to accept the data from bus
 * @param data_len read data length
 * @param ack ack value for read command
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, int ack);

/**
 * @brief Queue command for I2C master to generate a stop signal
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle);

/**
 * @brief I2C master send queued commands.
 *        This function will trigger sending all queued commands.
 *        The task will be blocked until all the commands have been sent out.
 *        The I2C APIs are not thread-safe, if you want to use one I2C port in different tasks,
 *        you need to take care of the multi-thread issue.
 *        @note
 *        Only call this function in I2C master mode
 *
 * @param i2c_num I2C port number
 * @param cmd_handle I2C command handler
 * @param ticks_to_wait maximum wait ticks.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_master_cmd_begin(i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle, portBASE_TYPE ticks_to_wait);

/**
 * @brief I2C slave write data to internal ringbuffer, when tx fifo empty, isr will fill the hardware
 *        fifo from the internal ringbuffer
 *        @note
 *        Only call this function in I2C slave mode
 *
 * @param i2c_num I2C port number
 * @param data data pointer to write into internal buffer
 * @param size data size
 * @param ticks_to_wait Maximum waiting ticks
 *
 * @return
 *     - ESP_FAIL(-1) Parameter error
 *     - Others(>=0) The number of data bytes that pushed to the I2C slave buffer.
 */
int i2c_slave_write_buffer(i2c_port_t i2c_num, uint8_t* data, int size, portBASE_TYPE ticks_to_wait);

/**
 * @brief I2C slave read data from internal buffer. When I2C slave receive data, isr will copy received data
 *        from hardware rx fifo to internal ringbuffer. Then users can read from internal ringbuffer.
 *        @note
 *        Only call this function in I2C slave mode
 *
 * @param i2c_num I2C port number
 * @param data data pointer to write into internal buffer
 * @param max_size Maximum data size to read
 * @param ticks_to_wait Maximum waiting ticks
 *
 * @return
 *     - ESP_FAIL(-1) Parameter error
 *     - Others(>=0) The number of data bytes that read from I2C slave buffer.
 */
int i2c_slave_read_buffer(i2c_port_t i2c_num, uint8_t* data, size_t max_size, portBASE_TYPE ticks_to_wait);

/**
 * @brief set I2C master clock period
 *
 * @param i2c_num I2C port number
 * @param high_period clock cycle number during SCL is high level, high_period is a 14 bit value
 * @param low_period clock cycle number during SCL is low level, low_period is a 14 bit value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_set_period(i2c_port_t i2c_num, int high_period, int low_period);

/**
 * @brief get I2C master clock period
 *
 * @param i2c_num I2C port number
 * @param high_period pointer to get clock cycle number during SCL is high level, will get a 14 bit value
 * @param low_period pointer to get clock cycle number during SCL is low level, will get a 14 bit value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_get_period(i2c_port_t i2c_num, int* high_period, int* low_period);

/**
 * @brief set I2C master start signal timing
 *
 * @param i2c_num I2C port number
 * @param setup_time clock number between the falling-edge of SDA and rising-edge of SCL for start mark, it's a 10-bit value.
 * @param hold_time clock num between the falling-edge of SDA and falling-edge of SCL for start mark, it's a 10-bit value.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_set_start_timing(i2c_port_t i2c_num, int setup_time, int hold_time);

/**
 * @brief get I2C master start signal timing
 *
 * @param i2c_num I2C port number
 * @param setup_time pointer to get setup time
 * @param hold_time pointer to get hold time
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_get_start_timing(i2c_port_t i2c_num, int* setup_time, int* hold_time);

/**
 * @brief set I2C master stop signal timing
 *
 * @param i2c_num I2C port number
 * @param setup_time clock num between the rising-edge of SCL and the rising-edge of SDA, it's a 10-bit value.
 * @param hold_time clock number after the STOP bit's rising-edge, it's a 14-bit value.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_set_stop_timing(i2c_port_t i2c_num, int setup_time, int hold_time);

/**
 * @brief get I2C master stop signal timing
 *
 * @param i2c_num I2C port number
 * @param setup_time pointer to get setup time.
 * @param hold_time pointer to get hold time.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_get_stop_timing(i2c_port_t i2c_num, int* setup_time, int* hold_time);

/**
 * @brief set I2C data signal timing
 *
 * @param i2c_num I2C port number
 * @param sample_time clock number I2C used to sample data on SDA after the rising-edge of SCL, it's a 10-bit value
 * @param hold_time clock number I2C used to hold the data after the falling-edge of SCL, it's a 10-bit value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_set_data_timing(i2c_port_t i2c_num, int sample_time, int hold_time);

/**
 * @brief get I2C data signal timing
 *
 * @param i2c_num I2C port number
 * @param sample_time pointer to get sample time
 * @param hold_time pointer to get hold time
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_get_data_timing(i2c_port_t i2c_num, int* sample_time, int* hold_time);

/**
 * @brief set I2C data transfer mode
 *
 * @param i2c_num I2C port number
 * @param tx_trans_mode I2C sending data mode
 * @param rx_trans_mode I2C receving data mode
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_set_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t tx_trans_mode, i2c_trans_mode_t rx_trans_mode);

/**
 * @brief get I2C data transfer mode
 *
 * @param i2c_num I2C port number
 * @param tx_trans_mode pointer to get I2C sending data mode
 * @param rx_trans_mode pointer to get I2C receiving data mode
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_get_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t *tx_trans_mode, i2c_trans_mode_t *rx_trans_mode);

#ifdef __cplusplus
}
#endif

#endif /*_DRIVER_I2C_H_*/
