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

#ifndef _DRIVER_UART_H_
#define _DRIVER_UART_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include <esp_types.h>

#define UART_FIFO_LEN           (128)        /*!< Length of the hardware FIFO buffers */
#define UART_INTR_MASK          0x1ff        /*!< mask of all UART interrupts */
#define UART_LINE_INV_MASK      (0x3f << 19) /*!< TBD */
#define UART_BITRATE_MAX        5000000      /*!< Max bit rate supported by UART */
#define UART_PIN_NO_CHANGE      (-1)         /*!< Constant for uart_set_pin function which indicates that UART pin should not be changed */

#define UART_INVERSE_DISABLE  (0x0)            /*!< Disable UART signal inverse*/
#define UART_INVERSE_RXD   (UART_RXD_INV_M)    /*!< UART RXD input inverse*/
#define UART_INVERSE_CTS   (UART_CTS_INV_M)    /*!< UART CTS input inverse*/
#define UART_INVERSE_TXD   (UART_TXD_INV_M)    /*!< UART TXD output inverse*/
#define UART_INVERSE_RTS   (UART_RTS_INV_M)    /*!< UART RTS output inverse*/

/**
 * @brief UART word length constants
 */
typedef enum {
    UART_DATA_5_BITS = 0x0,    /*!< word length: 5bits*/
    UART_DATA_6_BITS = 0x1,    /*!< word length: 6bits*/
    UART_DATA_7_BITS = 0x2,    /*!< word length: 7bits*/
    UART_DATA_8_BITS = 0x3,    /*!< word length: 8bits*/
    UART_DATA_BITS_MAX = 0X4,
} uart_word_length_t;

/**
 * @brief UART stop bits number
 */
typedef enum {
    UART_STOP_BITS_1   = 0x1,  /*!< stop bit: 1bit*/
    UART_STOP_BITS_1_5 = 0x2,  /*!< stop bit: 1.5bits*/
    UART_STOP_BITS_2   = 0x3,  /*!< stop bit: 2bits*/
    UART_STOP_BITS_MAX = 0x4,
} uart_stop_bits_t;

/**
 * @brief UART peripheral number
 */
typedef enum {
    UART_NUM_0 = 0x0,  /*!< UART base address 0x3ff40000*/
    UART_NUM_1 = 0x1,  /*!< UART base address 0x3ff50000*/
    UART_NUM_2 = 0x2,  /*!< UART base address 0x3ff6E000*/
    UART_NUM_MAX,
} uart_port_t;

/**
 * @brief UART parity constants
 */
typedef enum {
    UART_PARITY_DISABLE = 0x0,   /*!< Disable UART parity*/
    UART_PARITY_EVEN = 0x2,     /*!< Enable UART even parity*/
    UART_PARITY_ODD  = 0x3      /*!< Enable UART odd parity*/
} uart_parity_t;

/**
 * @brief UART hardware flow control modes
 */
typedef enum {
    UART_HW_FLOWCTRL_DISABLE = 0x0,   /*!< disable hardware flow control*/
    UART_HW_FLOWCTRL_RTS     = 0x1,   /*!< enable RX hardware flow control (rts)*/
    UART_HW_FLOWCTRL_CTS     = 0x2,   /*!< enable TX hardware flow control (cts)*/
    UART_HW_FLOWCTRL_CTS_RTS = 0x3,   /*!< enable hardware flow control*/
    UART_HW_FLOWCTRL_MAX     = 0x4,
} uart_hw_flowcontrol_t;

/**
 * @brief UART configuration parameters for uart_param_config function
 */
typedef struct {
    int baud_rate;                      /*!< UART baudrate*/
    uart_word_length_t data_bits;       /*!< UART byte size*/
    uart_parity_t parity;               /*!< UART parity mode*/
    uart_stop_bits_t stop_bits;         /*!< UART stop bits*/
    uart_hw_flowcontrol_t flow_ctrl;    /*!< UART HW flow control mode(cts/rts)*/
    uint8_t rx_flow_ctrl_thresh ;       /*!< UART HW RTS threshold*/
} uart_config_t;

/**
 * @brief UART interrupt configuration parameters for uart_intr_config function
 */
typedef struct {
    uint32_t intr_enable_mask;          /*!< UART interrupt enable mask, choose from UART_XXXX_INT_ENA_M under UART_INT_ENA_REG(i), connect with bit-or operator*/
    uint8_t  rx_timeout_thresh;         /*!< UART timeout interrupt threshold(unit: time of sending one byte)*/
    uint8_t  txfifo_empty_intr_thresh;  /*!< UART TX empty interrupt threshold.*/
    uint8_t  rxfifo_full_thresh;        /*!< UART RX full interrupt threshold.*/
} uart_intr_config_t;

/**
 * @brief UART event types used in the ringbuffer
 */
typedef enum {
    UART_DATA,              /*!< UART data event*/
    UART_BREAK,             /*!< UART break event*/
    UART_BUFFER_FULL,       /*!< UART RX buffer full event*/
    UART_FIFO_OVF,          /*!< UART FIFO overflow event*/
    UART_FRAME_ERR,         /*!< UART RX frame error event*/
    UART_PARITY_ERR,        /*!< UART RX parity event*/
    UART_DATA_BREAK,        /*!< UART TX data and break event*/
    UART_EVENT_MAX,         /*!< UART event max index*/
    UART_PATTERN_DET,        /*!< UART pattern detected */
} uart_event_type_t;

/**
 * @brief Event structure used in UART event queue
 */
typedef struct {
    uart_event_type_t type; /*!< UART event type */
    size_t size;            /*!< UART data size for UART_DATA event*/
} uart_event_t;

typedef intr_handle_t uart_isr_handle_t;

/**
 * @brief Set UART data bits.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param data_bit UART data bits
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_word_length(uart_port_t uart_num, uart_word_length_t data_bit);

/**
 * @brief Get UART data bits.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param data_bit Pointer to accept value of UART data bits.
 *
 * @return
 *     - ESP_FAIL  Parameter error
 *     - ESP_OK    Success, result will be put in (*data_bit)
 */
esp_err_t uart_get_word_length(uart_port_t uart_num, uart_word_length_t* data_bit);

/**
 * @brief Set UART stop bits.
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param stop_bits  UART stop bits
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Fail
 */
esp_err_t uart_set_stop_bits(uart_port_t uart_num, uart_stop_bits_t stop_bits);

/**
 * @brief Set UART stop bits.
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param stop_bits  Pointer to accept value of UART stop bits.
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success, result will be put in (*stop_bit)
 */
esp_err_t uart_get_stop_bits(uart_port_t uart_num, uart_stop_bits_t* stop_bits);

/**
 * @brief Set UART parity.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param parity_mode the enum of uart parity configuration
 *
 * @return
 *     - ESP_FAIL  Parameter error
 *     - ESP_OK    Success
 */
esp_err_t uart_set_parity(uart_port_t uart_num, uart_parity_t parity_mode);

/**
 * @brief Get UART parity mode.
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param parity_mode Pointer to accept value of UART parity mode.
 *
 * @return
 *     - ESP_FAIL  Parameter error
 *     - ESP_OK    Success, result will be put in (*parity_mode)
 *
 */
esp_err_t uart_get_parity(uart_port_t uart_num, uart_parity_t* parity_mode);

/**
 * @brief Set UART baud rate.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param baudrate UART baud rate.
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t uart_set_baudrate(uart_port_t uart_num, uint32_t baudrate);

/**
 * @brief Get UART bit-rate.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param baudrate Pointer to accept value of UART baud rate
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success, result will be put in (*baudrate)
 *
 */
esp_err_t uart_get_baudrate(uart_port_t uart_num, uint32_t* baudrate);

/**
 * @brief Set UART line inverse mode
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param inverse_mask Choose the wires that need to be inverted.
 *        Inverse_mask should be chosen from UART_INVERSE_RXD/UART_INVERSE_TXD/UART_INVERSE_RTS/UART_INVERSE_CTS, combine with OR operation.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_line_inverse(uart_port_t uart_num, uint32_t inverse_mask);

/**
 * @brief Set hardware flow control.
 *
 * @param uart_num   UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param flow_ctrl Hardware flow control mode
 * @param rx_thresh Threshold of Hardware RX flow control(0 ~ UART_FIFO_LEN).
 *          Only when UART_HW_FLOWCTRL_RTS is set, will the rx_thresh value be set.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh);

/**
 * @brief Get hardware flow control mode
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param flow_ctrl Option for different flow control mode.
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success, result will be put in (*flow_ctrl)
 */
esp_err_t uart_get_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t* flow_ctrl);

/**
 * @brief Clear UART interrupt status
 *
 * @param uart_num   UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param clr_mask  Bit mask of the status that to be cleared.
 *                  enable_mask should be chosen from the fields of register UART_INT_CLR_REG.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_clear_intr_status(uart_port_t uart_num, uint32_t clr_mask);

/**
 * @brief Set UART interrupt enable
 *
 * @param uart_num      UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param enable_mask  Bit mask of the enable bits.
 *                     enable_mask should be chosen from the fields of register UART_INT_ENA_REG.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_intr_mask(uart_port_t uart_num, uint32_t enable_mask);

/**
 * @brief Clear UART interrupt enable bits
 *
 * @param uart_num       UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param disable_mask  Bit mask of the disable bits.
 *                      disable_mask should be chosen from the fields of register UART_INT_ENA_REG.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_intr_mask(uart_port_t uart_num, uint32_t disable_mask);


/**
 * @brief Enable UART RX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_rx_intr(uart_port_t uart_num);

/**
 * @brief Disable UART RX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_rx_intr(uart_port_t uart_num);

/**
 * @brief Disable UART TX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_tx_intr(uart_port_t uart_num);

/**
 * @brief Enable UART TX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param enable  1: enable; 0: disable
 * @param thresh  Threshold of TX interrupt, 0 ~ UART_FIFO_LEN
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_tx_intr(uart_port_t uart_num, int enable, int thresh);

/**
 * @brief register UART interrupt handler(ISR).
 *
 * @note UART ISR handler will be attached to the same CPU core that this function is running on.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param fn  Interrupt handler function.
 * @param arg parameter for handler function
 * @param  intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info. 
 * @param handle Pointer to return handle. If non-NULL, a handle for the interrupt will
 *        be returned here.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_isr_register(uart_port_t uart_num, void (*fn)(void*), void * arg, int intr_alloc_flags,  uart_isr_handle_t *handle);


/**
 * @brief Free UART interrupt handler registered by uart_isr_register. Must be called on the same core as
 * uart_isr_register was called.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_isr_free(uart_port_t uart_num);

/**
 * @brief Set UART pin number
 *
 * @note Internal signal can be output to multiple GPIO pads.
 * Only one GPIO pad can connect with input signal.
 *
 * @param uart_num    UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param tx_io_num  UART TX pin GPIO number, if set to UART_PIN_NO_CHANGE, use the current pin.
 * @param rx_io_num  UART RX pin GPIO number, if set to UART_PIN_NO_CHANGE, use the current pin.
 * @param rts_io_num UART RTS pin GPIO number, if set to UART_PIN_NO_CHANGE, use the current pin.
 * @param cts_io_num UART CTS pin GPIO number, if set to UART_PIN_NO_CHANGE, use the current pin.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);

/**
 * @brief UART set RTS level (before inverse)
 *          UART rx hardware flow control should not be set.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param level   1: RTS output low(active); 0: RTS output high(block)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_rts(uart_port_t uart_num, int level);

/**
 * @brief UART set DTR level (before inverse)
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param level    1: DTR output low; 0: DTR output high
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_dtr(uart_port_t uart_num, int level);

/**
* @brief UART parameter configure
 *
 * @param uart_num     UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param uart_config UART parameter settings
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config);

/**
* @brief UART interrupt configure
 *
 * @param uart_num     UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param intr_conf UART interrupt settings
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_intr_config(uart_port_t uart_num, const uart_intr_config_t *intr_conf);

/**
 * @brief Install UART driver.
 *
 * UART ISR handler will be attached to the same CPU core that this function is running on.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param rx_buffer_size UART RX ring buffer size, rx_buffer_size should be greater than UART_FIFO_LEN.
 * @param tx_buffer_size UART TX ring buffer size.
 *        If set to zero, driver will not use TX buffer, TX function will block task until all data have been sent out..
 * @param queue_size UART event queue size/depth.
 * @param uart_queue UART event queue handle (out param). On success, a new queue handle is written here to provide
 *        access to UART events. If set to NULL, driver will not use an event queue.
 * @param  intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags);

/**
 * @brief Uninstall UART driver.
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_driver_delete(uart_port_t uart_num);

/**
 * @brief Wait UART TX FIFO empty
 *
 * @param uart_num       UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param ticks_to_wait Timeout, count in RTOS ticks
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 *     - ESP_ERR_TIMEOUT  Timeout
 */
esp_err_t uart_wait_tx_done(uart_port_t uart_num, TickType_t ticks_to_wait);

/**
 * @brief Send data to the UART port from a given buffer and length.
 * 
 * This function will not wait for the space in TX FIFO, just fill the TX FIFO and return when the FIFO is full.
 * @note This function should only be used when UART TX buffer is not enabled.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param buffer data buffer address
 * @param len    data length to send
 *
 * @return
 *     - (-1)  Parameter error
 *     - OTHERS(>=0)  The number of data that pushed to the TX FIFO
 */
int uart_tx_chars(uart_port_t uart_num, const char* buffer, uint32_t len);

/**
 * @brief Send data to the UART port from a given buffer and length,
 *
 * If parameter tx_buffer_size is set to zero:
 * This function will not return until all the data have been sent out, or at least pushed into TX FIFO.
 *
 * Otherwise, if tx_buffer_size > 0, this function will return after copying all the data to tx ringbuffer,
 * then, UART ISR will move data from ring buffer to TX FIFO gradually.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param src   data buffer address
 * @param size  data length to send
 *
 * @return
 *     - (-1) Parameter error
 *     - OTHERS(>=0)  The number of data that pushed to the TX FIFO
 */
int uart_write_bytes(uart_port_t uart_num, const char* src, size_t size);

/**
 * @brief Send data to the UART port from a given buffer and length,
 *
 * If parameter tx_buffer_size is set to zero:
 * This function will not return until all the data and the break signal have been sent out.
 * After all data send out, send a break signal.
 *
 * Otherwise, if tx_buffer_size > 0, this function will return after copying all the data to tx ringbuffer,
 * then, UART ISR will move data from ring buffer to TX FIFO gradually.
 * After all data send out, send a break signal.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param src   data buffer address
 * @param size  data length to send
 * @param brk_len break signal length (unit: time of one data bit at current_baudrate)
 *
 * @return
 *     - (-1) Parameter error
 *     - OTHERS(>=0) The number of data that pushed to the TX FIFO
 */

int uart_write_bytes_with_break(uart_port_t uart_num, const char* src, size_t size, int brk_len);

/**
 * @brief UART read bytes from UART buffer
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param buf     pointer to the buffer.
 * @param length  data length
 * @param ticks_to_wait sTimeout, count in RTOS ticks
 *
 * @return
 *     - (-1) Error
 *     - Others return a char data from uart fifo.
 */
int uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, TickType_t ticks_to_wait);

/**
 * @brief UART ring buffer flush
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_flush(uart_port_t uart_num);

/**
 * @brief   UART get RX ring buffer cached data length
 *
 * @param   uart_num UART port number.
 * @param   size Pointer of size_t to accept cached data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_get_buffered_data_len(uart_port_t uart_num, size_t* size);

/**
 * @brief   UART disable pattern detect function.
 *          Designed for applications like 'AT commands'.
 *          When the hardware detect a series of one same character, the interrupt will be triggered.
 *
 * @param   uart_num UART port number.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_pattern_det_intr(uart_port_t uart_num);

/**
 * @brief   UART enable pattern detect function.
 *          Designed for applications like 'AT commands'.
 *          When the hardware detect a series of one same character, the interrupt will be triggered.
 *
 * @param uart_num UART port number.
 * @param pattern_chr character of the pattern
 * @param chr_num number of the character, 8bit value.
 * @param chr_tout timeout of the interval between each pattern characters, 24bit value, unit is APB(80Mhz) clock cycle.
 * @param post_idle idle time after the last pattern character, 24bit value, unit is APB(80Mhz) clock cycle.
 * @param pre_idle idle time before the first pattern character, 24bit value, unit is APB(80Mhz) clock cycle.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_pattern_det_intr(uart_port_t uart_num, char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle);
/***************************EXAMPLE**********************************
 *
 *
 * ----------------EXAMPLE OF UART SETTING ---------------------
 * @code{c}
 * //1. Setup UART
 * #include "freertos/queue.h"
 * //a. Set UART parameter
 * int uart_num = 0;                                       //uart port number
 * uart_config_t uart_config = {
 *    .baud_rate = 115200,                                 //baudrate
 *    .data_bits = UART_DATA_8_BITS,                       //data bit mode
 *    .parity = UART_PARITY_DISABLE,                       //parity mode
 *    .stop_bits = UART_STOP_BITS_1,                       //stop bit mode
 *    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,               //hardware flow control(cts/rts)
 *    .rx_flow_ctrl_thresh = 120,                          //flow control threshold
 * };
 * uart_param_config(uart_num, &uart_config);
 * //b1. Setup UART driver(with UART queue)
 * QueueHandle_t uart_queue;
 * //parameters here are just an example, tx buffer size is 2048
 * uart_driver_install(uart_num, 1024 * 2, 1024 * 2, 10, &uart_queue, 0);
 * //b2. Setup UART driver(without UART queue)
 * //parameters here are just an example, tx buffer size is 0
 * uart_driver_install(uart_num, 1024 * 2, 0, 10, NULL, 0);
 *@endcode
 *-----------------------------------------------------------------------------*
 * @code{c}
 * //2. Set UART pin
 * //set UART pin, not needed if use default pins.
 * uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 15, 13);
 * @endcode
 *-----------------------------------------------------------------------------*
 * @code{c}
 * //3. Read data from UART.
 * uint8_t data[128];
 * int length = 0;
 * length = uart_read_bytes(uart_num, data, sizeof(data), 100);
 * @endcode
 *-----------------------------------------------------------------------------*
 * @code{c}
 * //4. Write data to UART.
 * char* test_str = "This is a test string.\n"
 * uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));
 * @endcode
 *-----------------------------------------------------------------------------*
 * @code{c}
 * //5. Write data to UART, end with a break signal.
 * uart_write_bytes_with_break(0, "test break\n",strlen("test break\n"), 100);
 * @endcode
 *-----------------------------------------------------------------------------*
 * @code{c}
 * //6. an example of echo test with hardware flow control on UART1
 * void uart_loop_back_test()
 * {
 *     int uart_num = 1;
 *     uart_config_t uart_config = {
 *         .baud_rate = 115200,
 *         .data_bits = UART_DATA_8_BITS,
 *         .parity = UART_PARITY_DISABLE,
 *         .stop_bits = UART_STOP_BITS_1,
 *         .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
 *         .rx_flow_ctrl_thresh = 122,
 *     };
 *     //Configure UART1 parameters
 *     uart_param_config(uart_num, &uart_config);
 *     //Set UART1 pins(TX: IO16, RX: IO17, RTS: IO18, CTS: IO19)
 *     uart_set_pin(uart_num, 16, 17, 18, 19);
 *     //Install UART driver( We don't need an event queue here)
 *     uart_driver_install(uart_num, 1024 * 2, 1024*4, 0, NULL, 0);
 *     uint8_t data[1000];
 *     while(1) {
 *         //Read data from UART
 *         int len = uart_read_bytes(uart_num, data, sizeof(data), 10);
 *         //Write data back to UART
 *         uart_write_bytes(uart_num, (const char*)data, len);
 *     }
 * }
 * @endcode
 *-----------------------------------------------------------------------------*
 * @code{c}
 * //7. An example of using UART event queue on UART0.
 * #include "freertos/queue.h"
 * //A queue to handle UART event.
 * QueueHandle_t uart0_queue;
 * static const char *TAG = "uart_example";
 * void uart_task(void *pvParameters)
 * {
 *     int uart_num = (int)pvParameters;
 *     uart_event_t event;
 *     size_t size = 1024;
 *     uint8_t* dtmp = (uint8_t*)malloc(size);
 *     for(;;) {
 *         //Waiting for UART event.
 *         if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
 *             ESP_LOGI(TAG, "uart[%d] event:", uart_num);
 *             switch(event.type) {
 *                 memset(dtmp, 0, size);
 *                 //Event of UART receving data
 *                 case UART_DATA:
 *                     ESP_LOGI(TAG,"data, len: %d", event.size);
 *                     int len = uart_read_bytes(uart_num, dtmp, event.size, 10);
 *                     ESP_LOGI(TAG, "uart read: %d", len);
 *                     break;
 *                 //Event of HW FIFO overflow detected
 *                 case UART_FIFO_OVF:
 *                     ESP_LOGI(TAG, "hw fifo overflow\n");
 *                     break;
 *                 //Event of UART ring buffer full
 *                 case UART_BUFFER_FULL:
 *                     ESP_LOGI(TAG, "ring buffer full\n");
 *                     break;
 *                 //Event of UART RX break detected
 *                 case UART_BREAK:
 *                     ESP_LOGI(TAG, "uart rx break\n");
 *                     break;
 *                 //Event of UART parity check error
 *                 case UART_PARITY_ERR:
 *                     ESP_LOGI(TAG, "uart parity error\n");
 *                     break;
 *                 //Event of UART frame error
 *                 case UART_FRAME_ERR:
 *                     ESP_LOGI(TAG, "uart frame error\n");
 *                     break;
 *                 //Others
 *                 default:
 *                     ESP_LOGI(TAG, "uart event type: %d\n", event.type);
 *                     break;
 *             }
 *        }
 *     }
 *     free(dtmp);
 *     dtmp = NULL;
 *     vTaskDelete(NULL);
 * }
 *
 * void uart_queue_test()
 * {
 *     int uart_num = 0;
 *     uart_config_t uart_config = {
 *        .baud_rate = 115200,
 *        .data_bits = UART_DATA_8_BITS,
 *        .parity = UART_PARITY_DISABLE,
 *        .stop_bits = UART_STOP_BITS_1,
 *        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
 *        .rx_flow_ctrl_thresh = 122,
 *     };
 *     //Set UART parameters
 *     uart_param_config(uart_num, &uart_config);
 *     //Set UART pins,(-1: default pin, no change.)
 *     uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
 *     //Set UART log level
 *     esp_log_level_set(TAG, ESP_LOG_INFO);
 *     //Install UART driver, and get the queue.
 *     uart_driver_install(uart_num, 1024 * 2, 1024*4, 10, &uart0_queue, 0);
 *     //Create a task to handler UART event from ISR
 *     xTaskCreate(uart_task, "uTask", 1024, (void*)uart_num, 10, NULL);
 * }
 * @endcode
 *
 ***************************END OF EXAMPLE**********************************/

#ifdef __cplusplus
}
#endif

#endif /*_DRIVER_UART_H_*/
