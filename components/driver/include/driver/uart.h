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
#include "soc/uart_channel.h"

#define UART_FIFO_LEN           (128)        /*!< Length of the hardware FIFO buffers */
#define UART_INTR_MASK          0x1ff        /*!< Mask of all UART interrupts */
#define UART_LINE_INV_MASK      (0x3f << 19) /*!< TBD */
#define UART_BITRATE_MAX        5000000      /*!< Max bit rate supported by UART */
#define UART_PIN_NO_CHANGE      (-1)         /*!< Constant for uart_set_pin function which indicates that UART pin should not be changed */

#define UART_INVERSE_DISABLE  (0x0)            /*!< Disable UART signal inverse*/
#define UART_INVERSE_RXD   (UART_RXD_INV_M)    /*!< UART RXD input inverse*/
#define UART_INVERSE_CTS   (UART_CTS_INV_M)    /*!< UART CTS input inverse*/
#define UART_INVERSE_TXD   (UART_TXD_INV_M)    /*!< UART TXD output inverse*/
#define UART_INVERSE_RTS   (UART_RTS_INV_M)    /*!< UART RTS output inverse*/

/**
 * @brief UART mode selection
 */
typedef enum {
    UART_MODE_UART = 0x00,                      /*!< mode: regular UART mode*/
    UART_MODE_RS485_HALF_DUPLEX = 0x01,         /*!< mode: half duplex RS485 UART mode control by RTS pin */
    UART_MODE_IRDA = 0x02,                      /*!< mode: IRDA  UART mode*/
    UART_MODE_RS485_COLLISION_DETECT = 0x03,    /*!< mode: RS485 collision detection UART mode (used for test purposes)*/
    UART_MODE_RS485_APP_CTRL = 0x04,            /*!< mode: application control RS485 UART mode (used for test purposes)*/
} uart_mode_t;

/**
 * @brief UART word length constants
 */
typedef enum {
    UART_DATA_5_BITS = 0x0,    /*!< word length: 5bits*/
    UART_DATA_6_BITS = 0x1,    /*!< word length: 6bits*/
    UART_DATA_7_BITS = 0x2,    /*!< word length: 7bits*/
    UART_DATA_8_BITS = 0x3,    /*!< word length: 8bits*/
    UART_DATA_BITS_MAX = 0x4,
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
    UART_NUM_2 = 0x2,  /*!< UART base address 0x3ff6e000*/
    UART_NUM_MAX,
} uart_port_t;

/**
 * @brief UART parity constants
 */
typedef enum {
    UART_PARITY_DISABLE = 0x0,  /*!< Disable UART parity*/
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
    int baud_rate;                      /*!< UART baud rate*/
    uart_word_length_t data_bits;       /*!< UART byte size*/
    uart_parity_t parity;               /*!< UART parity mode*/
    uart_stop_bits_t stop_bits;         /*!< UART stop bits*/
    uart_hw_flowcontrol_t flow_ctrl;    /*!< UART HW flow control mode (cts/rts)*/
    uint8_t rx_flow_ctrl_thresh;        /*!< UART HW RTS threshold*/
    bool use_ref_tick;                  /*!< Set to true if UART should be clocked from REF_TICK */
} uart_config_t;

/**
 * @brief UART interrupt configuration parameters for uart_intr_config function
 */
typedef struct {
    uint32_t intr_enable_mask;          /*!< UART interrupt enable mask, choose from UART_XXXX_INT_ENA_M under UART_INT_ENA_REG(i), connect with bit-or operator*/
    uint8_t  rx_timeout_thresh;         /*!< UART timeout interrupt threshold (unit: time of sending one byte)*/
    uint8_t  txfifo_empty_intr_thresh;  /*!< UART TX empty interrupt threshold.*/
    uint8_t  rxfifo_full_thresh;        /*!< UART RX full interrupt threshold.*/
} uart_intr_config_t;

/**
 * @brief UART event types used in the ring buffer
 */
typedef enum {
    UART_DATA,              /*!< UART data event*/
    UART_BREAK,             /*!< UART break event*/
    UART_BUFFER_FULL,       /*!< UART RX buffer full event*/
    UART_FIFO_OVF,          /*!< UART FIFO overflow event*/
    UART_FRAME_ERR,         /*!< UART RX frame error event*/
    UART_PARITY_ERR,        /*!< UART RX parity event*/
    UART_DATA_BREAK,        /*!< UART TX data and break event*/
    UART_PATTERN_DET,       /*!< UART pattern detected */
    UART_EVENT_MAX,         /*!< UART event max index*/
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
 * @brief Get UART stop bits.
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
 * @brief Set UART parity mode.
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
 * @brief Get UART baud rate.
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
 *        Inverse_mask should be chosen from 
 *        UART_INVERSE_RXD / UART_INVERSE_TXD / UART_INVERSE_RTS / UART_INVERSE_CTS,
 *        combined with OR operation.
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
 * @param rx_thresh Threshold of Hardware RX flow control (0 ~ UART_FIFO_LEN).
 *        Only when UART_HW_FLOWCTRL_RTS is set, will the rx_thresh value be set.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh);

/**
 * @brief Set software flow control.
 *
 * @param uart_num   UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param enable     switch on or off
 * @param rx_thresh_xon  low water mark
 * @param rx_thresh_xoff high water mark
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
 esp_err_t uart_set_sw_flow_ctrl(uart_port_t uart_num, bool enable, uint8_t rx_thresh_xon,  uint8_t rx_thresh_xoff);

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
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param clr_mask  Bit mask of the interrupt status to be cleared.
 *                  The bit mask should be composed from the fields of register UART_INT_CLR_REG.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_clear_intr_status(uart_port_t uart_num, uint32_t clr_mask);

/**
 * @brief Set UART interrupt enable
 *
 * @param uart_num     UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param enable_mask  Bit mask of the enable bits.
 *                     The bit mask should be composed from the fields of register UART_INT_ENA_REG.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_intr_mask(uart_port_t uart_num, uint32_t enable_mask);

/**
 * @brief Clear UART interrupt enable bits
 *
 * @param uart_num      UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param disable_mask  Bit mask of the disable bits.
 *                      The bit mask should be composed from the fields of register UART_INT_ENA_REG.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_intr_mask(uart_port_t uart_num, uint32_t disable_mask);

/**
 * @brief Enable UART RX interrupt (RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_rx_intr(uart_port_t uart_num);

/**
 * @brief Disable UART RX interrupt (RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_rx_intr(uart_port_t uart_num);

/**
 * @brief Disable UART TX interrupt (TX_FULL & TX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_tx_intr(uart_port_t uart_num);

/**
 * @brief Enable UART TX interrupt (TX_FULL & TX_TIMEOUT INTERRUPT)
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
 * @brief Register UART interrupt handler (ISR).
 *
 * @note UART ISR handler will be attached to the same CPU core that this function is running on.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param fn  Interrupt handler function.
 * @param arg parameter for handler function
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info. 
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
 *       Only one GPIO pad can connect with input signal.
 *
 * @note Instead of GPIO number a macro 'UART_PIN_NO_CHANGE' may be provided 
         to keep the currently allocated pin.
 *
 * @param uart_num   UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param tx_io_num  UART TX pin GPIO number.
 * @param rx_io_num  UART RX pin GPIO number.
 * @param rts_io_num UART RTS pin GPIO number.
 * @param cts_io_num UART CTS pin GPIO number.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);

/**
 * @brief Manually set the UART RTS pin level.
 * @note  UART must be configured with hardware flow control disabled.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param level    1: RTS output low (active); 0: RTS output high (block)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_rts(uart_port_t uart_num, int level);

/**
 * @brief Manually set the UART DTR pin level.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param level    1: DTR output low; 0: DTR output high
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_dtr(uart_port_t uart_num, int level);

/**
 * @brief Set UART idle interval after tx FIFO is empty
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param idle_num idle interval after tx FIFO is empty(unit: the time it takes to send one bit
 *        under current baudrate)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_tx_idle_num(uart_port_t uart_num, uint16_t idle_num);

/**
 * @brief Set UART configuration parameters.
 *
 * @param uart_num    UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param uart_config UART parameter settings
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config);

/**
 * @brief Configure UART interrupts.
 *
 * @param uart_num  UART_NUM_0, UART_NUM_1 or UART_NUM_2
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
 * @note  Rx_buffer_size should be greater than UART_FIFO_LEN. Tx_buffer_size should be either zero or greater than UART_FIFO_LEN.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param rx_buffer_size UART RX ring buffer size.
 * @param tx_buffer_size UART TX ring buffer size.
 *        If set to zero, driver will not use TX buffer, TX function will block task until all data have been sent out.
 * @param queue_size UART event queue size/depth.
 * @param uart_queue UART event queue handle (out param). On success, a new queue handle is written here to provide
 *        access to UART events. If set to NULL, driver will not use an event queue.
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info. Do not set ESP_INTR_FLAG_IRAM here
 *        (the driver's ISR handler is not located in IRAM)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags);

/**
 * @brief Uninstall UART driver.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_driver_delete(uart_port_t uart_num);

/**
 * @brief Wait until UART TX FIFO is empty.
 *
 * @param uart_num      UART_NUM_0, UART_NUM_1 or UART_NUM_2
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
 * This function will not wait for enough space in TX FIFO. It will just fill the available TX FIFO and return when the FIFO is full.
 * @note This function should only be used when UART TX buffer is not enabled.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param buffer data buffer address
 * @param len    data length to send
 *
 * @return
 *     - (-1)  Parameter error
 *     - OTHERS (>=0) The number of bytes pushed to the TX FIFO
 */
int uart_tx_chars(uart_port_t uart_num, const char* buffer, uint32_t len);

/**
 * @brief Send data to the UART port from a given buffer and length,
 *
 * If the UART driver's parameter 'tx_buffer_size' is set to zero:
 * This function will not return until all the data have been sent out, or at least pushed into TX FIFO.
 *
 * Otherwise, if the 'tx_buffer_size' > 0, this function will return after copying all the data to tx ring buffer,
 * UART ISR will then move data from the ring buffer to TX FIFO gradually.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param src   data buffer address
 * @param size  data length to send
 *
 * @return
 *     - (-1) Parameter error
 *     - OTHERS (>=0) The number of bytes pushed to the TX FIFO
 */
int uart_write_bytes(uart_port_t uart_num, const char* src, size_t size);

/**
 * @brief Send data to the UART port from a given buffer and length,
 *
 * If the UART driver's parameter 'tx_buffer_size' is set to zero:
 * This function will not return until all the data and the break signal have been sent out.
 * After all data is sent out, send a break signal.
 *
 * Otherwise, if the 'tx_buffer_size' > 0, this function will return after copying all the data to tx ring buffer,
 * UART ISR will then move data from the ring buffer to TX FIFO gradually.
 * After all data sent out, send a break signal.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param src   data buffer address
 * @param size  data length to send
 * @param brk_len break signal duration(unit: the time it takes to send one bit at current baudrate)
 *
 * @return
 *     - (-1) Parameter error
 *     - OTHERS (>=0) The number of bytes pushed to the TX FIFO
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
 *     - OTHERS (>=0) The number of bytes read from UART FIFO
 */
int uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, TickType_t ticks_to_wait);

/**
 * @brief Alias of uart_flush_input.
 *        UART ring buffer flush. This will discard all data in the UART RX buffer.
 * @note  Instead of waiting the data sent out, this function will clear UART rx buffer.
 *        In order to send all the data in tx FIFO, we can use uart_wait_tx_done function.
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_flush(uart_port_t uart_num);

/**
 * @brief Clear input buffer, discard all the data is in the ring-buffer.
 * @note  In order to send all the data in tx FIFO, we can use uart_wait_tx_done function.
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_flush_input(uart_port_t uart_num);

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
 *          When the hardware detects a series of one same character, the interrupt will be triggered.
 *
 * @param uart_num UART port number.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_pattern_det_intr(uart_port_t uart_num);

/**
 * @brief UART enable pattern detect function.
 *        Designed for applications like 'AT commands'.
 *        When the hardware detect a series of one same character, the interrupt will be triggered.
 *
 * @param uart_num UART port number.
 * @param pattern_chr character of the pattern
 * @param chr_num number of the character, 8bit value.
 * @param chr_tout timeout of the interval between each pattern characters, 24bit value, unit is APB (80Mhz) clock cycle.
 *        When the duration is less than this value, it will not take this data as at_cmd char
 * @param post_idle idle time after the last pattern character, 24bit value, unit is APB (80Mhz) clock cycle.
 *        When the duration is less than this value, it will not take the previous data as the last at_cmd char
 * @param pre_idle idle time before the first pattern character, 24bit value, unit is APB (80Mhz) clock cycle.
 *        When the duration is less than this value, it will not take this data as the first at_cmd char
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_pattern_det_intr(uart_port_t uart_num, char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle);

/**
 * @brief Return the nearest detected pattern position in buffer.
 *        The positions of the detected pattern are saved in a queue,
 *        this function will dequeue the first pattern position and move the pointer to next pattern position.
 * @note  If the RX buffer is full and flow control is not enabled,
 *        the detected pattern may not be found in the rx buffer due to overflow.
 *
 *        The following APIs will modify the pattern position info:
 *        uart_flush_input, uart_read_bytes, uart_driver_delete, uart_pop_pattern_pos
 *        It is the application's responsibility to ensure atomic access to the pattern queue and the rx data buffer
 *        when using pattern detect feature.
 *
 * @param uart_num UART port number
 * @return
 *     - (-1) No pattern found for current index or parameter error
 *     - others the pattern position in rx buffer.
 */
int uart_pattern_pop_pos(uart_port_t uart_num);

/**
 * @brief Return the nearest detected pattern position in buffer.
 *        The positions of the detected pattern are saved in a queue,
 *        This function do nothing to the queue.
 * @note  If the RX buffer is full and flow control is not enabled,
 *        the detected pattern may not be found in the rx buffer due to overflow.
 *
 *        The following APIs will modify the pattern position info:
 *        uart_flush_input, uart_read_bytes, uart_driver_delete, uart_pop_pattern_pos
 *        It is the application's responsibility to ensure atomic access to the pattern queue and the rx data buffer
 *        when using pattern detect feature.
 *
 * @param uart_num UART port number
 * @return
 *     - (-1) No pattern found for current index or parameter error
 *     - others the pattern position in rx buffer.
 */
int uart_pattern_get_pos(uart_port_t uart_num);

/**
 * @brief Allocate a new memory with the given length to save record the detected pattern position in rx buffer.
 * @param uart_num UART port number
 * @param queue_length Max queue length for the detected pattern.
 *        If the queue length is not large enough, some pattern positions might be lost.
 *        Set this value to the maximum number of patterns that could be saved in data buffer at the same time.
 * @return
 *     - ESP_ERR_NO_MEM No enough memory
 *     - ESP_ERR_INVALID_STATE Driver not installed
 *     - ESP_FAIL Parameter error
 *     - ESP_OK Success
 */
esp_err_t uart_pattern_queue_reset(uart_port_t uart_num, int queue_length);

/**
 * @brief UART set communication mode 
 * @note  This function must be executed after uart_driver_install(), when the driver object is initialized.
 * @param uart_num     Uart number to configure
 * @param mode UART    UART mode to set 
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode);

/**
 * @brief UART set threshold timeout for TOUT feature
 *
 * @param uart_num     Uart number to configure
 * @param tout_thresh  This parameter defines timeout threshold in uart symbol periods. The maximum value of threshold is 126. 
 *        tout_thresh = 1, defines TOUT interrupt timeout equal to transmission time of one symbol (~11 bit) on current baudrate. 
 *        If the time is expired the UART_RXFIFO_TOUT_INT interrupt is triggered. If tout_thresh == 0,
 *        the TOUT feature is disabled.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_INVALID_STATE Driver is not installed
 */
esp_err_t uart_set_rx_timeout(uart_port_t uart_num, const uint8_t tout_thresh);

/**
 * @brief Returns collision detection flag for RS485 mode
 *        Function returns the collision detection flag into variable pointed by collision_flag.
 *        *collision_flag = true, if collision detected else it is equal to false.
 *        This function should be executed when actual transmission is completed (after uart_write_bytes()).
 *
 * @param uart_num       Uart number to configure
 * @param collision_flag Pointer to variable of type bool to return collision flag.
 *
 * @return
 *     - ESP_OK Success 
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t uart_get_collision_flag(uart_port_t uart_num, bool* collision_flag);

/**
 * @brief Set the number of RX pin signal edges for light sleep wakeup
 *
 * UART can be used to wake up the system from light sleep. This feature works
 * by counting the number of positive edges on RX pin and comparing the count to
 * the threshold. When the count exceeds the threshold, system is woken up from
 * light sleep. This function allows setting the threshold value.
 *
 * Stop bit and parity bits (if enabled) also contribute to the number of edges.
 * For example, letter 'a' with ASCII code 97 is encoded as 0100001101 on the wire
 * (with 8n1 configuration), start and stop bits included. This sequence has 3
 * positive edges (transitions from 0 to 1). Therefore, to wake up the system
 * when 'a' is sent, set wakeup_threshold=3.
 *
 * The character that triggers wakeup is not received by UART (i.e. it can not
 * be obtained from UART FIFO). Depending on the baud rate, a few characters
 * after that will also not be received. Note that when the chip enters and exits
 * light sleep mode, APB frequency will be changing. To make sure that UART has
 * correct baud rate all the time, select REF_TICK as UART clock source,
 * by setting use_ref_tick field in uart_config_t to true.
 *
 * @note in ESP32, the wakeup signal can only be input via IO_MUX (i.e.
 *       GPIO3 should be configured as function_1 to wake up UART0,
 *       GPIO9 should be configured as function_5 to wake up UART1), UART2
 *       does not support light sleep wakeup feature.
 *
 * @param uart_num  UART number
 * @param wakeup_threshold  number of RX edges for light sleep wakeup, value is 3 .. 0x3ff.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if uart_num is incorrect or wakeup_threshold is
 *        outside of [3, 0x3ff] range.
 */
esp_err_t uart_set_wakeup_threshold(uart_port_t uart_num, int wakeup_threshold);

/**
 * @brief Get the number of RX pin signal edges for light sleep wakeup.
 *
 * See description of uart_set_wakeup_threshold for the explanation of UART
 * wakeup feature.
 *
 * @param uart_num  UART number
 * @param[out] out_wakeup_threshold  output, set to the current value of wakeup
 *                                   threshold for the given UART.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if out_wakeup_threshold is NULL
 */
esp_err_t uart_get_wakeup_threshold(uart_port_t uart_num, int* out_wakeup_threshold);

#ifdef __cplusplus
}
#endif

#endif /*_DRIVER_UART_H_*/
