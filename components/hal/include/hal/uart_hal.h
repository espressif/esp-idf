/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for UART.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/uart_ll.h"
#include "hal/uart_types.h"

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    uart_dev_t *dev;
} uart_hal_context_t;

/**
 * @brief  Configure the UART baud-rate and select the source clock
 *
 * @param  hal Context of the HAL layer
 * @param  baud_rate The baud-rate to be set
 * @param  sclk_freq Frequency of the clock source of UART, in Hz.
 *
 * @return None
 */
#define uart_hal_set_baudrate(hal, baud_rate, sclk_freq) uart_ll_set_baudrate((hal)->dev, baud_rate, sclk_freq)

/**
 * @brief Set the UART source clock type
 * @param  hal Context of the HAL layer
 * @param  sclk The UART source clock type.
 *
 * @return None
 */
#define uart_hal_set_sclk(hal, sclk) uart_ll_set_sclk((hal)->dev, sclk);

/**
 * @brief Clear the UART interrupt status
 *
 * @param  hal Context of the HAL layer
 * @param  mask The interrupt status mask to be cleared. Using the ORred mask of `UART_INTR_RXFIFO_FULL ... UART_INTR_CMD_CHAR_DET`
 *
 * @return None
 */
#define uart_hal_clr_intsts_mask(hal, mask)  uart_ll_clr_intsts_mask((hal)->dev, mask)

/**
 * @brief Disable the UART interrupt
 *
 * @param  hal Context of the HAL layer
 * @param  mask The interrupt mask to be disabled. Using the ORred mask of `UART_INTR_RXFIFO_FULL ... UART_INTR_CMD_CHAR_DET`
 *
 * @return None
 */
#define uart_hal_disable_intr_mask(hal, mask)  uart_ll_disable_intr_mask((hal)->dev, mask)

/**
 * @brief Enable the UART interrupt
 *
 * @param  hal Context of the HAL layer
 * @param  mask The UART interrupt mask to be enabled. Using the ORred mask of `UART_INTR_RXFIFO_FULL ... UART_INTR_CMD_CHAR_DET`
 *
 * @return None
 */
#define uart_hal_ena_intr_mask(hal, mask)  uart_ll_ena_intr_mask((hal)->dev, mask)

/**
 * @brief Get the UART raw interrupt status
 *
 * @param  hal Context of the HAL layer
 *
 * @return UART raw interrupt status
 */
#define uart_hal_get_intraw_mask(hal) uart_ll_get_intraw_mask((hal)->dev)

/**
 * @brief Get the UART interrupt status
 *
 * @param  hal Context of the HAL layer
 *
 * @return UART interrupt status
 */
#define uart_hal_get_intsts_mask(hal)  uart_ll_get_intsts_mask((hal)->dev)

/**
 * @brief Get status of enabled interrupt
 *
 * @param  hal Context of the HAL layer
 *
 * @return UART Interrupt enabled value
 */
#define uart_hal_get_intr_ena_status(hal) uart_ll_get_intr_ena_status((hal)->dev)

/**
 * @brief Get the UART pattern char configuration
 *
 * @param  hal Context of the HAL layer
 * @param  cmd_char Pointer to accept UART AT cmd char
 * @param  char_num Pointer to accept the `UART_CHAR_NUM` configuration
 *
 * @return None
 */
#define uart_hal_get_at_cmd_char(hal, cmd_char, char_num)  uart_ll_get_at_cmd_char((hal)->dev, cmd_char, char_num)

/**
 * @brief Set the UART rst signal active level
 *
 * @param  hal Context of the HAL layer
 * @param  active_level The rts active level. The active level is low if set to 0. The active level is high if set to 1
 *
 * @return None
 */
#define uart_hal_set_rts(hal, active_level)  uart_ll_set_rts_active_level((hal)->dev, active_level)

/**
 * @brief Get the txfifo writeable length(in byte)
 *
 * @param  hal Context of the HAL layer
 *
 * @return UART txfifo writeable length
 */
#define uart_hal_get_txfifo_len(hal)  uart_ll_get_txfifo_len((hal)->dev)

/**
 * @brief Check if the UART sending state machine is in the IDLE state.
 *
 * @param  hal Context of the HAL layer
 *
 * @return True if the state machine is in the IDLE state, otherwise false will be returned.
 */
#define uart_hal_is_tx_idle(hal)  uart_ll_is_tx_idle((hal)->dev)

/**
 * @brief  Read data from the UART rxfifo
 *
 * @param[in] hal Context of the HAL layer
 * @param[in] buf Pointer to the buffer used to store the read data. The buffer size should be large than 128 byte
 * @param[inout] inout_rd_len As input, the size of output buffer to read (set to 0 to read all available data).
 *                            As output, returns the actual size written into the output buffer.
 *
 * @return None
 */
void uart_hal_read_rxfifo(uart_hal_context_t *hal, uint8_t *buf, int *inout_rd_len);

/**
 * @brief  Write data into the UART txfifo
 *
 * @param  hal Context of the HAL layer
 * @param  buf Pointer of the data buffer need to be written to txfifo
 * @param  data_size The data size(in byte) need to be written
 * @param  write_size The size has been written
 *
 * @return None
 */
void uart_hal_write_txfifo(uart_hal_context_t *hal, const uint8_t *buf, uint32_t data_size, uint32_t *write_size);

/**
 * @brief  Reset the UART txfifo
 * @note   On ESP32, this function is reserved for UART1 and UART2.
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void uart_hal_txfifo_rst(uart_hal_context_t *hal);

/**
 * @brief  Reset the UART rxfifo
 *
 * @param  hal Context of the HAL layer
 *
 * @return None
 */
void uart_hal_rxfifo_rst(uart_hal_context_t *hal);

/**
 * @brief Init the UART hal and set the UART to the default configuration.
 *
 * @param  hal Context of the HAL layer
 * @param  uart_num The uart port number, the max port number is (UART_NUM_MAX -1)
 *
 * @return None
 */
void uart_hal_init(uart_hal_context_t *hal, uart_port_t uart_num);

/**
 * @brief Get the UART source clock type
 *
 * @param  hal Context of the HAL layer
 * @param  sclk The poiter to accept the UART source clock type
 *
 * @return None
 */
void uart_hal_get_sclk(uart_hal_context_t *hal, soc_module_clk_t *sclk);

/**
 * @brief  Configure the UART stop bit
 *
 * @param  hal Context of the HAL layer
 * @param  stop_bit The stop bit to be set
 *
 * @return None
 */
void uart_hal_set_stop_bits(uart_hal_context_t *hal, uart_stop_bits_t stop_bit);

/**
 * @brief Configure the UART data bit
 *
 * @param  hal Context of the HAL layer
 * @param  data_bit The data bit to be set
 *
 * @return None
 */
void uart_hal_set_data_bit_num(uart_hal_context_t *hal, uart_word_length_t data_bit);

/**
 * @brief Configure the UART parity mode
 *
 * @param  hal Context of the HAL layer
 * @param  parity_mode The UART parity mode to be set
 *
 * @return None
 */
void uart_hal_set_parity(uart_hal_context_t *hal, uart_parity_t parity_mode);

/**
 * @brief Configure the UART hardware flow control
 *
 * @param  hal Context of the HAL layer
 * @param  flow_ctrl The flow control mode to be set
 * @param  rx_thresh The rts flow control signal will be active if the data length in rxfifo is large than this value
 *
 * @return None
 */
void uart_hal_set_hw_flow_ctrl(uart_hal_context_t *hal, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh);

/**
 * @brief Configure the UART AT cmd char detect function. When the receiver receives a continuous AT cmd char, it will produce a interrupt
 *
 * @param  hal Context of the HAL layer
 * @param  at_cmd The AT cmd char detect configuration
 *
 * @return None.
 */
void uart_hal_set_at_cmd_char(uart_hal_context_t *hal, uart_at_cmd_t *at_cmd);

/**
 * @brief Set the timeout value of the UART receiver
 *
 * @param  hal Context of the HAL layer
 * @param  tout The timeout value for receiver to receive a data
 *
 * @return None
 */
void uart_hal_set_rx_timeout(uart_hal_context_t *hal, const uint8_t tout);

/**
 * @brief Set the UART dtr signal active level
 *
 * @param  hal Context of the HAL layer
 * @param  active_level The dtr active level. The active level is low if set to 0. The active level is high if set to 1
 *
 * @return None
 */
void uart_hal_set_dtr(uart_hal_context_t *hal, int active_level);

/**
 * @brief Set the UART software flow control
 *
 * @param  hal Context of the HAL layer
 * @param  flow_ctrl The software flow control configuration
 * @param  sw_flow_ctrl_en Set true to enable the software flow control, otherwise set it false
 *
 * @return None
 */
void uart_hal_set_sw_flow_ctrl(uart_hal_context_t *hal, uart_sw_flowctrl_t *flow_ctrl, bool sw_flow_ctrl_en);

/**
 * @brief Set the UART tx idle number
 *
 * @param  hal Context of the HAL layer
 * @param  idle_num The cycle number betwin the two transmission
 *
 * @return None
 */
void uart_hal_set_tx_idle_num(uart_hal_context_t *hal, uint16_t idle_num);

/**
 * @brief Set the UART rxfifo full threshold
 *
 * @param  hal Context of the HAL layer
 * @param  full_thrhd The rxfifo full threshold. If the `UART_RXFIFO_FULL` interrupt is enabled and
 *         the data length in rxfifo is more than this value, it will generate `UART_RXFIFO_FULL` interrupt
 *
 * @return None
 */
void uart_hal_set_rxfifo_full_thr(uart_hal_context_t *hal, uint32_t full_thrhd);

/**
 * @brief Set the UART txfifo empty threshold
 *
 * @param  hal Context of the HAL layer
 * @param  empty_thrhd The txfifo empty threshold to be set. If the `UART_TXFIFO_EMPTY` interrupt is enabled and
 *         the data length in txfifo is less than this value, it will generate `UART_TXFIFO_EMPTY` interrupt
 *
 * @return None
 */
void uart_hal_set_txfifo_empty_thr(uart_hal_context_t *hal, uint32_t empty_thrhd);

/**
 * @brief Configure the UART to send a number of break(NULL) chars
 *
 * @param  hal Context of the HAL layer
 * @param  break_num The number of the break char need to be send
 *
 * @return None
 */
void uart_hal_tx_break(uart_hal_context_t *hal, uint32_t break_num);

/**
 * @brief Configure the UART wake up function.
 *     Note that RXD cannot be input through GPIO Matrix but only through IO_MUX when use this function
 *
 * @param  hal Context of the HAL layer
 * @param  wakeup_thrd The wake up threshold to be set. The system will be woken up from light-sleep when the input RXD edge changes more times than `wakeup_thrd+2`
 *
 * @return None
 */
void uart_hal_set_wakeup_thrd(uart_hal_context_t *hal, uint32_t wakeup_thrd);

/**
 * @brief Configure the UART mode
 *
 * @param  hal Context of the HAL layer
 * @param  mode The UART mode to be set
 *
 * @return None
 */
void uart_hal_set_mode(uart_hal_context_t *hal, uart_mode_t mode);

/**
 * @brief Configure the UART hardware to inverse the signals
 *
 * @param  hal Context of the HAL layer
 * @param  inv_mask The sigal mask needs to be inversed. Use the ORred mask of type `uart_signal_inv_t`
 *
 * @return None
 */
void uart_hal_inverse_signal(uart_hal_context_t *hal, uint32_t inv_mask);

/**
 * @brief Get the UART wakeup threshold configuration
 *
 * @param  hal Context of the HAL layer
 * @param  wakeup_thrd Pointer to accept the value of UART wakeup threshold configuration
 *
 * @return None
 */
void uart_hal_get_wakeup_thrd(uart_hal_context_t *hal, uint32_t *wakeup_thrd);

/**
 * @brief Get the UART data bit configuration
 *
 * @param  hal Context of the HAL layer
 * @param  data_bit Pointer to accept the value of UART data bit configuration
 *
 * @return None
 */
void uart_hal_get_data_bit_num(uart_hal_context_t *hal, uart_word_length_t *data_bit);

/**
 * @brief Get the UART stop bit configuration
 *
 * @param  hal Context of the HAL layer
 * @param  stop_bit Pointer to accept the value of UART stop bit configuration
 *
 * @return None
 */
void uart_hal_get_stop_bits(uart_hal_context_t *hal, uart_stop_bits_t *stop_bit);

/**
 * @brief Get the UART parity mode configuration
 *
 * @param  hal Context of the HAL layer
 * @param  parity_mode Pointer to accept the UART parity mode configuration
 *
 * @return None
 */
void uart_hal_get_parity(uart_hal_context_t *hal, uart_parity_t *parity_mode);

/**
 * @brief Get the UART baud-rate configuration
 *
 * @param  hal Context of the HAL layer
 * @param  baud_rate Pointer to accept the current baud-rate
 * @param  sclk_freq Frequency of the clock source of UART, in Hz.
 *
 * @return None
 */
void uart_hal_get_baudrate(uart_hal_context_t *hal, uint32_t *baud_rate, uint32_t sclk_freq);

/**
 * @brief Get the hw flow control configuration
 *
 * @param  hal Context of the HAL layer
 * @param  flow_ctrl Pointer to accept the UART flow control configuration
 *
 * @return None
 */
void uart_hal_get_hw_flow_ctrl(uart_hal_context_t *hal, uart_hw_flowcontrol_t *flow_ctrl);

/**
 * @brief Check if the UART rts flow control is enabled
 *
 * @param  hal Context of the HAL layer
 *
 * @return True if rts flow control is enabled, otherwise false will be returned
 */
bool uart_hal_is_hw_rts_en(uart_hal_context_t *hal);

/**
 * @brief Configure TX signal loop back to RX module, just for the testing purposes
 *
 * @param  hal Context of the HAL layer
 * @param  loop_back_en Set ture to enable the loop back function, else set it false.
 *
 * @return None
 */
void uart_hal_set_loop_back(uart_hal_context_t *hal, bool loop_back_en);

/**
 * @brief  Calculate uart symbol bit length, as defined in configuration.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return number of bits per UART symbol.
 */
uint8_t uart_hal_get_symb_len(uart_hal_context_t *hal);

/**
 * @brief  Get UART maximum timeout threshold.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return maximum timeout threshold value for target.
 */
uint16_t uart_hal_get_max_rx_timeout_thrd(uart_hal_context_t *hal);

/**
 * @brief  Get the timeout threshold value set for receiver.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return tout_thr The timeout value. If timeout is disabled then returns 0.
 */
#define uart_hal_get_rx_tout_thr(hal) uart_ll_get_rx_tout_thr((hal)->dev)

/**
 * @brief  Get the length of readable data in UART rxfifo.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return The readable data length in rxfifo.
 */
#define uart_hal_get_rxfifo_len(hal) uart_ll_get_rxfifo_len((hal)->dev)

#ifdef __cplusplus
}
#endif
