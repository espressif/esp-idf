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
#include "driver/periph_ctrl.h"
#include <esp_types.h>

extern const char* UART_TAG;
#define UART_FIFO_LEN           (128)   //Do Not Change it
#define UART_INTR_MASK          0x1ff
#define UART_LINE_INV_MASK      (0x3f << 19)

typedef enum {
    UART_DATA_5_BITS = 0x0,    //word length: 5bits
    UART_DATA_6_BITS = 0x1,    //word length: 6bits
    UART_DATA_7_BITS = 0x2,    //word length: 7bits
    UART_DATA_8_BITS = 0x3,    //word length: 8bits
    UART_DATA_MAX_BITS = 0X4,
} uart_word_length_t;

typedef enum {
    UART_STOP_BITS_1   = 0x1,  //stop bit: 1bit
    UART_STOP_BITS_1_5 = 0x2,  //stop bit: 1.5bits
    UART_STOP_BITS_2   = 0x3,  //stop bit: 2bits
    UART_STOP_BITS_MAX = 0x4,
} uart_stop_bits_t;

typedef enum {
    UART_NUM_0 = 0x0,  //base address 0x3ff40000
    UART_NUM_1 = 0x1,  //base address 0x3ff50000
    UART_NUM_2 = 0x2,  //base address 0x3ff6E000
    UART_NUM_MAX,
} uart_port_t;

typedef enum {
    UART_PARITY_DISABLE = 0x0,   //Disable UART parity
    UART_PARITY_EVEN = 0x10,     //Enable UART even parity
    UART_PARITY_ODD  = 0x11      //Enable UART odd parity
} uart_parity_t;

typedef enum {
    UART_BITRATE_300     = 300,
    UART_BITRATE_600     = 600,
    UART_BITRATE_1200    = 1200,
    UART_BITRATE_2400    = 2400,
    UART_BITRATE_4800    = 4800,
    UART_BITRATE_9600    = 9600,
    UART_BITRATE_19200   = 19200,
    UART_BITRATE_38400   = 38400,
    UART_BITRATE_57600   = 57600,
    UART_BITRATE_74880   = 74880,
    UART_BITRATE_115200  = 115200,
    UART_BITRATE_230400  = 230400,
    UART_BITRATE_460800  = 460800,
    UART_BITRATE_921600  = 921600,
    UART_BITRATE_1843200 = 1843200,
    UART_BITRATE_3686400 = 3686400,
    UART_BITRATE_MAX     = 5000000,
} uart_baudrate_t; //you can set any rate you need in this range

typedef enum {
    UART_HW_FLOWCTRL_DISABLE = 0x0,   //disable hardware flow control
    UART_HW_FLOWCTRL_RTS     = 0x1,   //enable RX hardware flow control (rts)
    UART_HW_FLOWCTRL_CTS     = 0x2,   //enable TX hardware flow control (cts)
    UART_HW_FLOWCTRL_CTS_RTS = 0x3,   //enable hardware flow control
    UART_HW_FLOWCTRL_MAX     = 0x4,
} uart_hw_flowcontrol_t;

typedef enum {
    UART_INVERSE_DISABLE = 0x0,                     //Disable UART wire output inverse
    UART_INVERSE_RXD  = (uint32_t)UART_RXD_INV_M,   //UART RXD input inverse
    UART_INVERSE_CTS  = (uint32_t)UART_CTS_INV_M,   //UART CTS input inverse
    UART_INVERSE_TXD  = (uint32_t)UART_TXD_INV_M,   //UART TXD output inverse
    UART_INVERSE_RTS  = (uint32_t)UART_RTS_INV_M,   //UART RTS output inverse
} uart_inverse_t;

typedef struct {
    uart_baudrate_t baud_rate;          //UART baudrate
    uart_word_length_t data_bits;       //UART byte size
    uart_parity_t parity;               //UART parity mode
    uart_stop_bits_t stop_bits;         //UART stop bits
    uart_hw_flowcontrol_t flow_ctrl;    //UART hw flow control mode(cts/rts)
    uint8_t rx_flow_ctrl_thresh ;       //UART hw RTS threshold
} uart_config_t;

typedef struct {
    uint32_t intr_enable_mask;          //UART interrupt enable mask, choose from UART_XXXX_INT_ENA_M under UART_INT_ENA_REG(i), connect with bit-or operator
    uint8_t  rx_timeout_thresh;         //UART timeout interrupt threshold(unit: time of sending one byte)
    uint8_t  txfifo_empty_intr_thresh;  //UART TX empty interrupt threshold.
    uint8_t  rxfifo_full_thresh;        //UART RX full interrupt threshold.
} uart_intr_config_t;


typedef enum {
    UART_DATA,
    UART_BREAK,
    UART_BUFFER_FULL,
    UART_FIFO_OVF,
    UART_FRAME_ERR,
    UART_PARITY_ERR,
    UART_EVENT_MAX,
} uart_event_type_t;

typedef struct {
    uart_event_type_t type;
    union {
        struct {
            size_t size;
        } data;

    };
} uart_event_t;



/**
 * @brief   Set UART data bits.
 *
 * @param   uart_port_t uart_no         : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uart_word_length_t data_bit : UART data bits
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_set_word_length(uart_port_t uart_num, uart_word_length_t data_bit);

/**
 * @brief   Get UART data bits.
 *
 * @param   uart_port_t uart_no: UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_FAIL (-1)       : Parameter error
 *          UART_DATA_5_BITS (0): UART word length: 5 bits.
 *          UART_DATA_6_BITS (1): UART word length: 6 bits.
 *          UART_DATA_7_BITS (2): UART word length: 7 bits.
 *          UART_DATA_8_BITS (3): UART word length: 8 bits.
 */
int uart_get_word_length(uart_port_t uart_num);

/**
 * @brief   Set UART stop bits.
 *
 * @param   uart_port_t uart_no: UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uart_stop_bits_t bit_num : UART stop bits
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Fail
 */
esp_err_t uart_set_stop_bits(uart_port_t uart_no, uart_stop_bits_t bit_num);

/**
 * @brief   Set UART stop bits.
 *
 * @param   uart_port_t uart_no: UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_FAIL (-1): Parameter error
 *          UART_STOP_BITS_1   (1): 1 stop bit
 *          UART_STOP_BITS_1_5 (2): 1.5 stop bits
 *          UART_STOP_BITS_1   (3): 2 stop bits
 */
int uart_get_stop_bits(uart_port_t uart_num);

/**
 * @brief   Set UART parity.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uart_parity_t parity_mode : the enum of uart parity configuration
 *
 * @return  null
 */
esp_err_t uart_set_parity(uart_port_t uart_no, uart_parity_t parity_mode);

/**
 * @brief   Get UART parity mode.
 *
 * @param   uart_port_t uart_no: UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_FAIL (-1): Parameter error
 *          UART_PARITY_ODD    (0x11): Odd parity check mode
 *          UART_PARITY_EVEN   (0x10): Even parity check mode
 *          UART_PARITY_DISABLE(0x0) : parity check disabled
 *
 */
int uart_get_parity(uart_port_t uart_num);

/**
 * @brief   Set UART baud rate.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uint32_t baud_rate   : UART baud-rate, we can choose one from uart_baudrate_t, or set a value.
 *
 * @return  null
 */
esp_err_t uart_set_baudrate(uart_port_t uart_no, uint32_t baud_rate);

/**
 * @brief   Get UART bit-rate.
 *
 * @param   uart_port_t uart_no: UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_FAIL(-1): Parameter error
 *          Others  (>0): UART baud-rate
 *
 */
int uart_get_baudrate(uart_port_t uart_num);

/**
 * @brief   Set UART line inverse mode
 * @param   uart_port_t uart_no   : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uint32_t inverse_mask : Choose the wires that need to be inversed
 *                                  (Should be chosen from uart_inverse_t, combine with OR-OPERATION)
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_set_line_inverse(uart_port_t uart_no, uint32_t inverse_mask) ;


/**
 * @brief   Set hardware flow control.
 *
 * @param   uart_port_t uart_no             : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uart_hw_flowcontrol_t flow_ctrl : Hardware flow control mode
 * @param   uint8_t rx_thresh               : Threshold of Hardware RX flow control(0 ~ UART_FIFO_LEN)
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_set_hw_flow_ctrl(uart_port_t uart_no, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh);

/**
 * @brief   Get hardware flow control mode
 * @param   uart_port_t uart_no   : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_FAIL (-1): Parameter error
 *          UART_HW_FLOWCTRL_DISABLE (0): UART hw flow control disabled
 *          UART_HW_FLOWCTRL_RTS     (1): UART RX flow control enabled
 *          UART_HW_FLOWCTRL_CTS     (2): UART TX flow control enabled
 *          UART_HW_FLOWCTRL_CTS_RTS (3): UART TX/RX flow control enabled
 */
int uart_get_hw_flow_ctrl(uart_port_t uart_num);

/**
 * @brief   Clear UART interrupt status
 *
 * @param   uart_port_t uart_no     : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uint32_t clr_mask       : Bit mask of the status that to be cleared.
 *                                    enable_mask should be chosen from the fields of register UART_INT_CLR_REG
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_clear_intr_status(uart_port_t uart_num, uint32_t clr_mask);

/**
 * @brief   Set UART interrupt enable
 *
 * @param   uart_port_t uart_no     : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uint32_t enable_mask       : Bit mask of the enable bits.
 *                                       enable_mask should be chosen from the fields of register UART_INT_ENA_REG
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_enable_intr_mask(uart_port_t uart_num, uint32_t enable_mask);

/**
 * @brief   Clear UART interrupt enable bits
 *
 * @param   uart_port_t uart_no    : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uint32_t disable_mask  : Bit mask of the disable bits.
 *                                   Disable_mask should be chosen from the fields of register UART_INT_ENA_REG
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_disable_intr_mask(uart_port_t uart_num, uint32_t disable_mask);


/**
 * @brief   Enable UART RX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param   uart_port_t uart_no    : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_enable_rx_intr(uart_port_t uart_num);

/**
 * @brief   Disable UART RX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param   uart_port_t uart_no    : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_disable_rx_intr(uart_port_t uart_num);

/**
 * @brief   Disable UART TX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param   uart_port_t uart_no    : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_disable_tx_intr(uart_port_t uart_num);

/**
 * @brief   Enable UART TX interrupt(RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   int enable          : 1: enable; 0: disable
 * @param   int thresh          : Threshold of TX interrupt, 0 ~ UART_FIFO_LEN
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_enable_tx_intr(uart_port_t uart_num, int enable, int thresh);

/**
* @brief   register UART interrupt handler(ISR).
 *          UART ISR handler will be attached to the same CPU core that this function is running on.
 *          Users should know that which CPU is running and then pick a INUM that is not used by system.
 *          We can find the information of INUM and interrupt level in soc.h.
 *
 *
 * @param   uart_port_t uart_no   : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uint8_t uart_intr_num : UART interrupt number,check the info in soc.h, and please refer to core-isa.h for more details
 * @param   void (* fn)(void* )   : Interrupt handler function.
 *                                  Note that the handler function MUST be defined with attribution of "IRAM_ATTR" for now.
 * @param   void * arg                : parameter for handler function
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_isr_register(uart_port_t uart_num, uint8_t uart_intr_num, void (*fn)(void*), void * arg);

/**
 * @brief   Set UART pin number
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   tx_io_num           : UART TX pin GPIO number
 * @param   rx_io_num           : UART RX pin GPIO number
 * @param   rts_io_num          : UART RTS pin GPIO number
 * @param   cts_io_num          : UART CTS pin GPIO number
 *
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);

/**
 * @brief   UART set RTS level (before inverse)
 *          UART rx hardware flow control should not be set.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   int level           : 1: RTS output low(active)
 *                                0: RTS output high(block)
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_set_rts(uart_port_t uart_num, int level);

/**
 * @brief   UART set DTR level (before inverse)
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   int level           : 1: DTR output low
 *                                0: DTR output high
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_set_dtr(uart_port_t uart_num, int level);

/**
* @brief   UART parameter configure
 *
 * @param   uart_port_t uart_no       : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uart_config_t *uart_config: UART parameter settings
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_param_config(uart_port_t uart_num, uart_config_t *uart_config);

/**
* @brief   UART interrupt configure
 *
 * @param   uart_port_t uart_no            : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uart_intr_config_t *p_intr_conf: UART interrupt settings
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_intr_config(uart_port_t uart_num, uart_intr_config_t *p_intr_conf);

/**
 * @brief   Install UART driver.
 *          UART ISR handler will be attached to the same CPU core that this function is running on.
 *          Users should know that which CPU is running and then pick a INUM that is not used by system.
 *          We can find the information of INUM and interrupt level in soc.h.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   int buffer_size     : UART ring buffer size
 * @param   int queue_size      : UART event queue size/depth.
 * @param   int uart_intr_num   : UART interrupt number,check the info in soc.h, and please refer to core-isa.h for more details
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_driver_install(uart_port_t uart_num, int buffer_size, int queue_size, int uart_intr_num, void* uart_queue);

/**
 * @brief   Uninstall UART driver.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_driver_delete(uart_port_t uart_num);

/**
 * @brief   Wait UART TX FIFO empty
 *
 * @param   uart_port_t uart_no     : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   TickType_t ticks_to_wait:  Timeout, count in RTOS ticks
 *
 * @return  ESP_OK         : Success
 *          ESP_ERR_TIMEOUT: Timeout
 */
esp_err_t uart_wait_tx_fifo_empty(uart_port_t uart_num, TickType_t ticks_to_wait);

/**
 * @brief   Send data to the UART port from a given buffer and length,
 *          This function will not wait for the space in TX FIFO, just fill the TX FIFO and return when the FIFO is full.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   char* buffer        : data buffer address
 * @param   uint32_t len        : data length to send
 *
 * @return  The number of data that pushed to the TX FIFO
 */
int uart_tx_chars(uart_port_t uart_no, char* buffer, uint32_t len);

/**
 * @brief   Send data to the UART port from a given buffer and length,
 *          This function will not return until all the data have been sent out, or at least pushed into TX FIFO.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   char* src           : data buffer address
 * @param   size_t size         : data length to send
 *
 * @return  The number of data that sent out.
 */
int uart_tx_all_chars(uart_port_t uart_num, const char* src, size_t size);

/**
 * @brief   Send data to the UART port from a given buffer and length,
 *          This function will not return until all the data and the break signal have been sent out.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   char* src           : data buffer address
 * @param   size_t size         : data length to send
 * @param   int brk_len         : break signal length (unit: one bit's time@current_baudrate)
 *
 * @return  The number of data that sent out.
 */
int uart_tx_all_chars_with_break(uart_port_t uart_num, const char* src, size_t size, int brk_len);

/**
* @brief   UART read one char
 *
 * @param   uart_port_t uart_no       : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   TickType_t ticks_to_wait  : ticks to wait.
 *
 * @return  -1      : Error
 *          Others  : return a char data from uart fifo.
 */
int uart_read_char(uart_port_t uart_num, TickType_t ticks_to_wait);

/**
* @brief   UART read bytes from UART buffer
 *
 * @param   uart_port_t uart_no   : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param   uint8_t* buf          : pointer to the buffer.
 * @param   uint32_t length       : data length
 * @param   TickType_t ticks_to_wait: timeout time( FreeRTOS ti c
 *
 * @return  -1      : Error
 *          Others  : return a char data from uart fifo.
 */
int uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, TickType_t ticks_to_wait);

/**
 * @brief   UART ring buffer flush
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error
 */
esp_err_t uart_flush(uart_port_t uart_num);

/**
 * @brief   Set the serial output port for ets_printf function, not effective for ESP_LOGX macro.
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  ESP_OK  : Success
 *          ESP_FAIL: Parameter error, or UART driver not installed.
 */
esp_err_t uart_set_print_port(uart_port_t uart_no);

/**
 * @brief   Get the current serial port for ets_printf function
 *
 *
 * @param   uart_port_t uart_no : UART_NUM_0, UART_NUM_1 or UART_NUM_2
 *
 * @return  null
 */
int uart_get_print_port();

/***************************EXAMPLE**********************************
 *
 *
 * ----------------EXAMPLE OF UART SETTING ---------------------
 *     //1. Setup UART
 *     #include "freertos/queue.h"
 *     #define UART_INTR_NUM 17                                //choose one interrupt number from soc.h
 *     //a. Set UART parameter
 *     int uart_num = 0;                                       //uart port number
 *     uart_config_t uart_config = {
 *        .baud_rate = UART_BITRATE_115200,                    //baudrate
 *        .data_bits = UART_DATA_8_BITS,                       //data bit mode
 *        .parity = UART_PARITY_DISABLE,                       //parity mode
 *        .stop_bits = UART_STOP_BITS_1,                       //stop bit mode
 *        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,               //hardware flow control(cts/rts)
 *        .rx_flow_ctrl_thresh = 120,                          //flow control threshold
 *     };
 *     uart_param_config(uart_num, &uart_config);
 *     //b1. Setup UART driver(with UART queue)
 *     QueueHandle_t uart_queue;
 *     uart_driver_install(uart_num, 1024 * 2, 10, UART_INTR_NUM, &uart_queue);//parameters here are just an example
 *     //b2. Setup UART driver(without UART queue)
 *     uart_driver_install(uart_num, 1024 * 2, 10, UART_INTR_NUM, NULL);       //parameters here are just an example
 *
 *-----------------------------------------------------------------------------*
 *     //2. Set UART pin
 *     uart_set_pin(uart_num, -1, -1, 15, 13);                 //set UART pin, not needed if use default pins.
 *
 *-----------------------------------------------------------------------------*
 *     //3. Read data from UART.
 *     uint8_t data[128];
 *     int length = 0;
 *     length = uart_read_bytes(uart_num, data, sizeof(data), 100);
 *
 *-----------------------------------------------------------------------------*
 *     //4. Write data to UART.
 *     char* test_str = "This is a test string.\n"
 *     uart_tx_all_chars(uart_num, (const char*)test_str, strlen(test_str));
 *
 *-----------------------------------------------------------------------------*
 *     //5. Write data to UART, end with a break signal.
 *     uart_tx_all_chars_with_break(0, "test break\n",strlen("test break\n"), 100);
 *
 *-----------------------------------------------------------------------------*
 *
 *     //6. an example of echo test with hardware flow control on UART1
 *     void uart_loop_back_test()
 *     {
 *         int uart_num = 1;
 *         uart_config_t uart_config = {
 *             .baud_rate = 115200,
 *             .data_bits = UART_DATA_8_BITS,
 *             .parity = UART_PARITY_DISABLE,
 *             .stop_bits = UART_STOP_BITS_1,
 *             .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
 *             .rx_flow_ctrl_thresh = 122,
 *         };
 *         uart_param_config(uart_num, &uart_config);                       //Config UART1 parameters
 *         uart_set_pin(uart_num, 16, 17, 18, 19);                          //Set UART1 pins(TX: IO16, RX: IO17, RTS: IO18, CTS: IO19)
 *         esp_log_level_set(UART_TAG, ESP_LOG_ERROR);                      //Set UART log level
 *         uart_driver_install(uart_num, 1024 * 2, 10, 17, NULL);           //Install UART driver( We don't need an event queue here)
 *         uint8_t data[1000];
 *         while(1) {
 *             int len = uart_read_bytes(uart_num, data, sizeof(data), 10); //Read data from UART
 *             uart_tx_all_chars(uart_num, (const char*)data, len);         //Write data back to UART
 *         }
 *     }
 *
 *-----------------------------------------------------------------------------*
 *     //7. An example of using UART event queue on UART0.
 *
 *        #include "freertos/queue.h"
 *        QueueHandle_t uart0_queue;                                        //A queue to handle UART event.
 *        void uart_task(void *pvParameters)
 *        {
 *            int uart_num = (int)pvParameters;
 *            uart_event_t event;
 *            uint8_t dtmp[1000];
 *            for(;;) {
 *                if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {  //Waiting for UART event.
 *                    ESP_LOGI(UART_TAG, "uart[%d] event:", uart_num);
 *                    switch(event.type) {
 *                        case UART_DATA:                                                         //Event of UART receving data
 *                            ESP_LOGI(UART_TAG,"data, len: %d\n", event.data.size);
 *                            int len = uart_read_bytes(uart_num, dtmp, event.data.size, 10);
 *                            ESP_LOGI(UART_TAG, "uart read: %d\n", len);
 *                            break;
 *                        case UART_FIFO_OVF:                                                     //Event of HW FIFO overflow detected
 *                            ESP_LOGI(UART_TAG, "hw fifo overflow\n");
 *                            while(1);
 *                            break;
 *                        case UART_BUFFER_FULL:                                                  //Event of UART ring buffer full
 *                            ESP_LOGI(UART_TAG, "ring buffer full\n");
 *                            break;
 *                        case UART_BREAK:
 *                            ESP_LOGI(UART_TAG, "uart rx break\n");                              //Event of UART RX break detected
 *                            break;
 *                        case UART_PARITY_ERR:                                                   //Event of UART parity check error
 *                            ESP_LOGI(UART_TAG, "uart parity error\n");
 *                            break;
 *                        case UART_FRAME_ERR:                                                    //Event of UART frame error
 *                            ESP_LOGI(UART_TAG, "uart frame error\n");
 *                            break;
 *                        default:                                                                //Others
 *                            ESP_LOGI(UART_TAG, "uart event type: %d\n", event.type);
 *                            break;
 *                    }
 *                }
 *            }
 *            vTaskDelete(NULL);
 *        }
 *
 *        void uart_queue_test()
 *        {
 *            int uart_num = 0;
 *            uart_config_t uart_config = {
 *                .baud_rate = 115200,
 *                .data_bits = UART_DATA_8_BITS,
 *                .parity = UART_PARITY_DISABLE,
 *                .stop_bits = UART_STOP_BITS_1,
 *                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
 *                .rx_flow_ctrl_thresh = 122,
 *            };
 *            uart_param_config(uart_num, &uart_config);                                 //Set UART parameters
 *            uart_set_pin(uart_num, -1, -1, 15, 13);                                    //Set UART pins,(-1: default pin, no change.)
 *            esp_log_level_set(UART_TAG, ESP_LOG_INFO);                                 //Set UART log level
 *            uart_driver_install(uart_num, 1024 * 2, 10, 17, &uart0_queue);             //Install UART driver, and get the queue.
 *            xTaskCreate(uart_task, "uTask", 2048*8, (void*)uart_num, 10, NULL);        //Create a task to handler UART event from ISR
 *        }
 *
 *
 ***************************END OF EXAMPLE**********************************/

#ifdef __cplusplus
}
#endif

#endif /*_DRIVER_UART_H_*/
