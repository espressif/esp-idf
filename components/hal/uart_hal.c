/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for UART (common part)
#include "hal/uart_hal.h"
#include "soc/soc_caps.h"

void uart_hal_get_sclk(uart_hal_context_t *hal, soc_module_clk_t *sclk)
{
    uart_ll_get_sclk(hal->dev, sclk);
}

void uart_hal_get_baudrate(uart_hal_context_t *hal, uint32_t *baud_rate, uint32_t sclk_freq)
{
    *baud_rate = uart_ll_get_baudrate(hal->dev, sclk_freq);
}

void uart_hal_set_stop_bits(uart_hal_context_t *hal, uart_stop_bits_t stop_bit)
{
    uart_ll_set_stop_bits(hal->dev, stop_bit);
}

void uart_hal_get_stop_bits(uart_hal_context_t *hal, uart_stop_bits_t *stop_bit)
{
    uart_ll_get_stop_bits(hal->dev, stop_bit);
}

void uart_hal_set_data_bit_num(uart_hal_context_t *hal, uart_word_length_t data_bit)
{
    uart_ll_set_data_bit_num(hal->dev, data_bit);
}

void uart_hal_get_data_bit_num(uart_hal_context_t *hal, uart_word_length_t *data_bit)
{
    uart_ll_get_data_bit_num(hal->dev, data_bit);
}

void uart_hal_set_parity(uart_hal_context_t *hal, uart_parity_t parity_mode)
{
    uart_ll_set_parity(hal->dev, parity_mode);
}

void uart_hal_get_parity(uart_hal_context_t *hal, uart_parity_t *parity_mode)
{
    uart_ll_get_parity(hal->dev, parity_mode);
}

void uart_hal_set_hw_flow_ctrl(uart_hal_context_t *hal, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh)
{
    uart_ll_set_hw_flow_ctrl(hal->dev, flow_ctrl, rx_thresh);
}

void uart_hal_get_hw_flow_ctrl(uart_hal_context_t *hal, uart_hw_flowcontrol_t *flow_ctrl)
{
    uart_ll_get_hw_flow_ctrl(hal->dev, flow_ctrl);
}

void uart_hal_set_sw_flow_ctrl(uart_hal_context_t *hal, uart_sw_flowctrl_t *flow_ctrl, bool sw_flow_ctrl_en)
{
    uart_ll_set_sw_flow_ctrl(hal->dev, flow_ctrl, sw_flow_ctrl_en);
}

void uart_hal_set_at_cmd_char(uart_hal_context_t *hal, uart_at_cmd_t *at_cmd)
{
    uart_ll_set_at_cmd_char(hal->dev, at_cmd);
}

void uart_hal_set_tx_idle_num(uart_hal_context_t *hal, uint16_t idle_num)
{
    uart_ll_set_tx_idle_num(hal->dev, idle_num);
}

void uart_hal_set_dtr(uart_hal_context_t *hal, int active_level)
{
    uart_ll_set_dtr_active_level(hal->dev, active_level);
}

void uart_hal_set_rxfifo_full_thr(uart_hal_context_t *hal, uint32_t full_thrhd)
{
    uart_ll_set_rxfifo_full_thr(hal->dev, full_thrhd);
}

void uart_hal_set_txfifo_empty_thr(uart_hal_context_t *hal, uint32_t empty_thrhd)
{
    uart_ll_set_txfifo_empty_thr(hal->dev, empty_thrhd);
}

void uart_hal_set_wakeup_thrd(uart_hal_context_t *hal, uint32_t wakeup_thrd)
{
    uart_ll_set_wakeup_thrd(hal->dev, wakeup_thrd);
}

void uart_hal_get_wakeup_thrd(uart_hal_context_t *hal, uint32_t *wakeup_thrd)
{
   *wakeup_thrd = uart_ll_get_wakeup_thrd(hal->dev);
}

void uart_hal_set_mode(uart_hal_context_t *hal, uart_mode_t mode)
{
    uart_ll_set_mode(hal->dev, mode);
}

bool uart_hal_is_hw_rts_en(uart_hal_context_t *hal)
{
    return uart_ll_is_hw_rts_en(hal->dev);
}

void uart_hal_inverse_signal(uart_hal_context_t *hal, uint32_t inv_mask)
{
    uart_ll_inverse_signal(hal->dev, inv_mask);
}

void uart_hal_set_loop_back(uart_hal_context_t *hal, bool loop_back_en)
{
    uart_ll_set_loop_back(hal->dev, loop_back_en);
}

void uart_hal_init(uart_hal_context_t *hal, uart_port_t uart_num)
{
    // Set UART mode.
    uart_ll_set_mode(hal->dev, UART_MODE_UART);
    // Disable UART parity
    uart_ll_set_parity(hal->dev, UART_PARITY_DISABLE);
    // 8-bit world
    uart_ll_set_data_bit_num(hal->dev, UART_DATA_8_BITS);
    // 1-bit stop bit
    uart_ll_set_stop_bits(hal->dev, UART_STOP_BITS_1);
    // Set tx idle
    uart_ll_set_tx_idle_num(hal->dev, 0);
    // Disable hw-flow control
    uart_ll_set_hw_flow_ctrl(hal->dev, UART_HW_FLOWCTRL_DISABLE, 100);
}

uint8_t uart_hal_get_symb_len(uart_hal_context_t *hal)
{
    uint8_t symbol_len = 1; // number of bits per symbol including start
    uart_parity_t parity_mode;
    uart_stop_bits_t stop_bit;
    uart_word_length_t data_bit;
    uart_ll_get_data_bit_num(hal->dev, &data_bit);
    uart_ll_get_stop_bits(hal->dev, &stop_bit);
    uart_ll_get_parity(hal->dev, &parity_mode);
    symbol_len += (data_bit < UART_DATA_BITS_MAX) ? (uint8_t)data_bit + 5 : 8;
    symbol_len += (stop_bit > UART_STOP_BITS_1) ? 2 : 1;
    symbol_len += (parity_mode > UART_PARITY_DISABLE) ? 1 : 0;
    return symbol_len;
}

void uart_hal_set_rx_timeout(uart_hal_context_t *hal, const uint8_t tout)
{
    uint8_t symb_len = uart_hal_get_symb_len(hal);
    uart_ll_set_rx_tout(hal->dev, symb_len * tout);
}

uint16_t uart_hal_get_max_rx_timeout_thrd(uart_hal_context_t *hal)
{
    uint8_t symb_len = uart_hal_get_symb_len(hal);
    uint16_t max_tout_thresh = uart_ll_max_tout_thrd(hal->dev);
    return (max_tout_thresh / symb_len);
}
