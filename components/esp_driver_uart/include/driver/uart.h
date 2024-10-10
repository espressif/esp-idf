/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "hal/uart_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief When calling `uart_set_pin`, instead of GPIO number, `UART_PIN_NO_CHANGE`
 *        can be provided to keep the currently allocated pin.
 */
#define UART_PIN_NO_CHANGE      (-1)

#define UART_FIFO_LEN _Pragma ("GCC warning \"'UART_FIFO_LEN' macro is deprecated, please use 'UART_HW_FIFO_LEN' instead\"") SOC_UART_FIFO_LEN ///< Length of the HP UART HW FIFO
#if (SOC_UART_LP_NUM >= 1)
#define UART_HW_FIFO_LEN(uart_num) ((uart_num < SOC_UART_HP_NUM) ? SOC_UART_FIFO_LEN : SOC_LP_UART_FIFO_LEN) ///< Length of the UART HW FIFO
#else
#define UART_HW_FIFO_LEN(uart_num) SOC_UART_FIFO_LEN                                                         ///< Length of the UART HW FIFO
#endif
#define UART_BITRATE_MAX        SOC_UART_BITRATE_MAX    ///< Maximum configurable bitrate

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
    union {
        uart_sclk_t source_clk;             /*!< UART source clock selection */
#if (SOC_UART_LP_NUM >= 1)
        lp_uart_sclk_t lp_source_clk;       /*!< LP_UART source clock selection */
#endif
    };
    struct {
        uint32_t allow_pd: 1;               /*!< If set, driver allows the power domain to be powered off when system enters sleep mode.
                                                 This can save power, but at the expense of more RAM being consumed to save register context. */
        uint32_t backup_before_sleep: 1;    /*!< @deprecated, same meaning as allow_pd */
    } flags;                                /*!< Configuration flags */
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
#if SOC_UART_SUPPORT_WAKEUP_INT
    UART_WAKEUP,            /*!< UART wakeup event */
#endif
    UART_EVENT_MAX,         /*!< UART event max index*/
} uart_event_type_t;

/**
 * @brief Event structure used in UART event queue
 */
typedef struct {
    uart_event_type_t type; /*!< UART event type */
    size_t size;            /*!< UART data size for UART_DATA event*/
    bool timeout_flag;      /*!< UART data read timeout flag for UART_DATA event (no new data received during configured RX TOUT)*/
    /*!< If the event is caused by FIFO-full interrupt, then there will be no event with the timeout flag before the next byte coming.*/
} uart_event_t;

typedef intr_handle_t uart_isr_handle_t;

/**
 * @brief Install UART driver and set the UART to the default configuration.
 *
 * UART ISR handler will be attached to the same CPU core that this function is running on.
 *
 * @note  Rx_buffer_size should be greater than UART_HW_FIFO_LEN(uart_num). Tx_buffer_size should be either zero or greater than UART_HW_FIFO_LEN(uart_num).
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_driver_delete(uart_port_t uart_num);

/**
 * @brief Checks whether the driver is installed or not
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return
 *     - true  driver is installed
 *     - false driver is not installed
 */
bool uart_is_driver_installed(uart_port_t uart_num);

/**
 * @brief Set UART data bits.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param data_bit UART data bits
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_word_length(uart_port_t uart_num, uart_word_length_t data_bit);

/**
 * @brief Get the UART data bit configuration.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
 * @param stop_bits  UART stop bits
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Fail
 */
esp_err_t uart_set_stop_bits(uart_port_t uart_num, uart_stop_bits_t stop_bits);

/**
 * @brief Get the UART stop bit configuration.
 *
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param parity_mode the enum of uart parity configuration
 *
 * @return
 *     - ESP_FAIL  Parameter error
 *     - ESP_OK    Success
 */
esp_err_t uart_set_parity(uart_port_t uart_num, uart_parity_t parity_mode);

/**
 * @brief Get the UART parity mode configuration.
 *
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
 * @param parity_mode Pointer to accept value of UART parity mode.
 *
 * @return
 *     - ESP_FAIL  Parameter error
 *     - ESP_OK    Success, result will be put in (*parity_mode)
 *
 */
esp_err_t uart_get_parity(uart_port_t uart_num, uart_parity_t* parity_mode);

/**
 * @brief Get the frequency of a clock source for the HP UART port
 *
 * @param sclk Clock source
 * @param[out] out_freq_hz Output of frequency, in Hz
 *
 * @return
 *  - ESP_ERR_INVALID_ARG: if the clock source is not supported
 *  - otherwise ESP_OK
 */
esp_err_t uart_get_sclk_freq(uart_sclk_t sclk, uint32_t* out_freq_hz);

/**
 * @brief Set UART baud rate.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param baudrate UART baud rate.
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t uart_set_baudrate(uart_port_t uart_num, uint32_t baudrate);

/**
 * @brief Get the UART baud rate configuration.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
 * @param inverse_mask Choose the wires that need to be inverted. Using the ORred mask of `uart_signal_inv_t`
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_line_inverse(uart_port_t uart_num, uint32_t inverse_mask);

/**
 * @brief Set hardware flow control.
 *
 * @param uart_num   UART port number, the max port number is (UART_NUM_MAX -1).
 * @param flow_ctrl Hardware flow control mode
 * @param rx_thresh Threshold of Hardware RX flow control (0 ~ UART_HW_FIFO_LEN(uart_num)).
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
 * @param uart_num   UART port number, the max port number is (UART_NUM_MAX -1)
 * @param enable     switch on or off
 * @param rx_thresh_xon  low water mark
 * @param rx_thresh_xoff high water mark
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_sw_flow_ctrl(uart_port_t uart_num, bool enable,  uint8_t rx_thresh_xon,  uint8_t rx_thresh_xoff);

/**
 * @brief Get the UART hardware flow control configuration.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
 * @param clr_mask  Bit mask of the interrupt status to be cleared.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_clear_intr_status(uart_port_t uart_num, uint32_t clr_mask);

/**
 * @brief Set UART interrupt enable
 *
 * @param uart_num     UART port number, the max port number is (UART_NUM_MAX -1).
 * @param enable_mask  Bit mask of the enable bits.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_intr_mask(uart_port_t uart_num, uint32_t enable_mask);

/**
 * @brief Clear UART interrupt enable bits
 *
 * @param uart_num      UART port number, the max port number is (UART_NUM_MAX -1).
 * @param disable_mask  Bit mask of the disable bits.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_intr_mask(uart_port_t uart_num, uint32_t disable_mask);

/**
 * @brief Enable UART RX interrupt (RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_rx_intr(uart_port_t uart_num);

/**
 * @brief Disable UART RX interrupt (RX_FULL & RX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_rx_intr(uart_port_t uart_num);

/**
 * @brief Disable UART TX interrupt (TX_FULL & TX_TIMEOUT INTERRUPT)
 *
 * @param uart_num  UART port number
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_disable_tx_intr(uart_port_t uart_num);

/**
 * @brief Enable UART TX interrupt (TX_FULL & TX_TIMEOUT INTERRUPT)
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param enable  1: enable; 0: disable
 * @param thresh  Threshold of TX interrupt, 0 ~ UART_HW_FIFO_LEN(uart_num)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_tx_intr(uart_port_t uart_num, int enable, int thresh);

/**
 * @brief Assign signals of a UART peripheral to GPIO pins
 *
 * @note If the GPIO number configured for a UART signal matches one of the
 *       IOMUX signals for that GPIO, the signal will be connected directly
 *       via the IOMUX. Otherwise the GPIO and signal will be connected via
 *       the GPIO Matrix. For example, if on an ESP32 the call
 *       `uart_set_pin(0, 1, 3, -1, -1)` is performed, as GPIO1 is UART0's
 *       default TX pin and GPIO3 is UART0's default RX pin, both will be
 *       connected to respectively U0TXD and U0RXD through the IOMUX, totally
 *       bypassing the GPIO matrix.
 *       The check is performed on a per-pin basis. Thus, it is possible to have
 *       RX pin binded to a GPIO through the GPIO matrix, whereas TX is binded
 *       to its GPIO through the IOMUX.
 *
 * @note Internal signal can be output to multiple GPIO pads.
 *       Only one GPIO pad can connect with input signal.
 *
 * @param uart_num   UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num    UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num  UART port number, the max port number is (UART_NUM_MAX -1).
 * @param intr_conf UART interrupt settings
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_intr_config(uart_port_t uart_num, const uart_intr_config_t *intr_conf);

/**
 * @brief Wait until UART TX FIFO is empty.
 *
 * @param uart_num      UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param src   data buffer address
 * @param size  data length to send
 *
 * @return
 *     - (-1) Parameter error
 *     - OTHERS (>=0) The number of bytes pushed to the TX FIFO
 */
int uart_write_bytes(uart_port_t uart_num, const void* src, size_t size);

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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param src   data buffer address
 * @param size  data length to send
 * @param brk_len break signal duration(unit: the time it takes to send one bit at current baudrate)
 *
 * @return
 *     - (-1) Parameter error
 *     - OTHERS (>=0) The number of bytes pushed to the TX FIFO
 */
int uart_write_bytes_with_break(uart_port_t uart_num, const void* src, size_t size, int brk_len);

/**
 * @brief UART read bytes from UART buffer
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param buf     pointer to the buffer.
 * @param length  data length
 * @param ticks_to_wait sTimeout, count in RTOS ticks
 *
 * @return
 *     - (-1) Error
 *     - OTHERS (>=0) The number of bytes read from UART buffer
 */
int uart_read_bytes(uart_port_t uart_num, void* buf, uint32_t length, TickType_t ticks_to_wait);

/**
 * @brief Alias of uart_flush_input.
 *        UART ring buffer flush. This will discard all data in the UART RX buffer.
 * @note  Instead of waiting the data sent out, this function will clear UART rx buffer.
 *        In order to send all the data in tx FIFO, we can use uart_wait_tx_done function.
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_flush(uart_port_t uart_num);

/**
 * @brief Clear input buffer, discard all the data is in the ring-buffer.
 * @note  In order to send all the data in tx FIFO, we can use uart_wait_tx_done function.
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_flush_input(uart_port_t uart_num);

/**
 * @brief   UART get RX ring buffer cached data length
 *
 * @param   uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param   size Pointer of size_t to accept cached data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_get_buffered_data_len(uart_port_t uart_num, size_t* size);

/**
 * @brief   UART get TX ring buffer free space size
 *
 * @param   uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param   size Pointer of size_t to accept the free space size
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t uart_get_tx_buffer_free_size(uart_port_t uart_num, size_t *size);

/**
 * @brief   UART disable pattern detect function.
 *          Designed for applications like 'AT commands'.
 *          When the hardware detects a series of one same character, the interrupt will be triggered.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param pattern_chr character of the pattern.
 * @param chr_num number of the character, 8bit value.
 * @param chr_tout timeout of the interval between each pattern characters, 16bit value, unit is the baud-rate cycle you configured.
 *        When the duration is more than this value, it will not take this data as at_cmd char.
 * @param post_idle idle time after the last pattern character, 16bit value, unit is the baud-rate cycle you configured.
 *        When the duration is less than this value, it will not take the previous data as the last at_cmd char
 * @param pre_idle idle time before the first pattern character, 16bit value, unit is the baud-rate cycle you configured.
 *        When the duration is less than this value, it will not take this data as the first at_cmd char.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_enable_pattern_det_baud_intr(uart_port_t uart_num, char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle);

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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @return
 *     - (-1) No pattern found for current index or parameter error
 *     - others the pattern position in rx buffer.
 */
int uart_pattern_get_pos(uart_port_t uart_num);

/**
 * @brief Allocate a new memory with the given length to save record the detected pattern position in rx buffer.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
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
 *
 * @note  This function must be executed after uart_driver_install(), when the driver object is initialized.
 * @param uart_num     Uart number to configure, the max port number is (UART_NUM_MAX -1).
 * @param mode UART    UART mode to set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode);

/**
 * @brief Set uart threshold value for RX fifo full
 * @note If application is using higher baudrate and it is observed that bytes
 *       in hardware RX fifo are overwritten then this threshold can be reduced
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1)
 * @param threshold Threshold value above which RX fifo full interrupt is generated
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_INVALID_STATE Driver is not installed
 */
esp_err_t uart_set_rx_full_threshold(uart_port_t uart_num, int threshold);

/**
 * @brief Set uart threshold values for TX fifo empty
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1)
 * @param threshold Threshold value below which TX fifo empty interrupt is generated
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_INVALID_STATE Driver is not installed
 */
esp_err_t uart_set_tx_empty_threshold(uart_port_t uart_num, int threshold);

/**
 * @brief UART set threshold timeout for TOUT feature
 *
 * @param uart_num     Uart number to configure, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num  Uart number to configure the max port number is (UART_NUM_MAX -1).
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
 * light sleep mode, APB frequency will be changing. To ensure that UART has
 * correct Baud rate all the time, it is necessary to select a source clock which has
 * a fixed frequency and remains active during sleep. For the supported clock sources
 * of the chips, please refer to `uart_sclk_t` or `soc_periph_uart_clk_src_legacy_t`
 *
 * @note in ESP32, the wakeup signal can only be input via IO_MUX (i.e.
 *       GPIO3 should be configured as function_1 to wake up UART0,
 *       GPIO9 should be configured as function_5 to wake up UART1), UART2
 *       does not support light sleep wakeup feature.
 *
 * @param uart_num  UART number, the max port number is (UART_NUM_MAX -1).
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
 * @param uart_num  UART number, the max port number is (UART_NUM_MAX -1).
 * @param[out] out_wakeup_threshold  output, set to the current value of wakeup
 *                                   threshold for the given UART.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if out_wakeup_threshold is NULL
 */
esp_err_t uart_get_wakeup_threshold(uart_port_t uart_num, int* out_wakeup_threshold);

/**
  * @brief Wait until UART tx memory empty and the last char send ok (polling mode).
  *
  * @param uart_num UART number
  *
  * * @return
  *      - ESP_OK on success
  *      - ESP_ERR_INVALID_ARG Parameter error
  *      - ESP_FAIL Driver not installed
  */
esp_err_t uart_wait_tx_idle_polling(uart_port_t uart_num);

/**
  * @brief Configure TX signal loop back to RX module, just for the test usage.
  *
  * @param uart_num UART number
  * @param loop_back_en Set true to enable the loop back function, else set it false.
  *
  * * @return
  *      - ESP_OK on success
  *      - ESP_ERR_INVALID_ARG Parameter error
  *      - ESP_FAIL Driver not installed
  */
esp_err_t uart_set_loop_back(uart_port_t uart_num, bool loop_back_en);

/**
  * @brief Configure behavior of UART RX timeout interrupt.
  *
  * When always_rx_timeout is true, timeout interrupt is triggered even if FIFO is full.
  * This function can cause extra timeout interrupts triggered only to send the timeout event.
  * Call this function only if you want to ensure timeout interrupt will always happen after a byte stream.
  *
  * @param uart_num UART number
  * @param always_rx_timeout_en Set to false enable the default behavior of timeout interrupt,
  *                             set it to true to always trigger timeout interrupt.
  *
  */
void uart_set_always_rx_timeout(uart_port_t uart_num, bool always_rx_timeout_en);

#ifdef __cplusplus
}
#endif
