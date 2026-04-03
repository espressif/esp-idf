/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdio.h>
#include "hal/uhci_types.h"
#include "soc/uhci_struct.h"
#include "soc/pcr_struct.h"
#include "hal/misc.h"

#define UHCI_LL_NUM (1UL)
#define UHCI_LL_GET_HW(num) (((num) == 0) ? (&UHCI0) : (NULL))
#define UHCI_LL_MAX_RECEIVE_PACKET_THRESHOLD  (8192)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UHCI_RX_BREAK_CHR_EOF = 0x1,
    UHCI_RX_IDLE_EOF      = 0x2,
    UHCI_RX_LEN_EOF       = 0x4,
    UHCI_RX_EOF_MAX       = 0x7,
} uhci_rxeof_cfg_t;

/**
 * @brief Enable the bus clock for UHCI module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void uhci_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    PCR.uhci_conf.uhci_clk_en = enable;
}

/**
 * @brief Reset the UHCI module
 *
 * @param group_id Group ID
 */
static inline void uhci_ll_reset_register(int group_id)
{
    (void)group_id;
    PCR.uhci_conf.uhci_rst_en = 1;
    PCR.uhci_conf.uhci_rst_en = 0;
}

/**
 * @brief Initialize the UHCI module. Reset configuration registers and keep the clock enabled
 *
 * @param hw Pointer to the UHCI device
 */
static inline void uhci_ll_init(uhci_dev_t *hw)
{
    uhci_conf0_reg_t conf0_reg;
    conf0_reg.val = 0;
    conf0_reg.clk_en = 1;
    hw->conf0.val = conf0_reg.val;
    hw->conf1.val = 0;
}

/**
 * @brief Attach the UHCI module to a UART port
 *
 * @param hw Pointer to the UHCI device
 * @param uart_num The UART port number to attach the UHCI module to
 */
static inline void uhci_ll_attach_uart_port(uhci_dev_t *hw, int uart_num)
{
    hw->conf0.uart_sel = uart_num;
}

/**
 * @brief UHCI escape sequence configuration
 *
 * @param hw Pointer to the UHCI device
 * @param seper_char UHCI escape sequence configuration
 */
static inline void uhci_ll_set_seper_chr(uhci_dev_t *hw, uhci_seper_chr_t *seper_char)
{
    if (seper_char->sub_chr_en) {
        hw->conf0.seper_en = 1;
        uhci_esc_conf0_reg_t esc_conf0_reg;
        esc_conf0_reg.val = hw->esc_conf0.val;

        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf0_reg, seper_char, seper_char->seper_chr);
        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf0_reg, seper_esc_char0, seper_char->sub_chr1);
        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf0_reg, seper_esc_char1, seper_char->sub_chr2);
        hw->esc_conf0.val = esc_conf0_reg.val;
        hw->escape_conf.tx_c0_esc_en = 1;
        hw->escape_conf.rx_c0_esc_en = 1;
    } else {
        hw->conf0.seper_en = 0;
        hw->escape_conf.val = 0;
    }
}

/**
 * @brief UHCI software flow control configuration
 *
 * @param hw Pointer to the UHCI device
 * @param sub_ctr UHCI software flow control configuration
 */
static inline void uhci_ll_set_swflow_ctrl_sub_chr(uhci_dev_t *hw, uhci_swflow_ctrl_sub_chr_t *sub_ctr)
{
    uhci_escape_conf_reg_t escape_conf_reg;
    escape_conf_reg.val = hw->escape_conf.val;

    if (sub_ctr->flow_en == 1) {
        uhci_esc_conf2_reg_t esc_conf2_reg;
        esc_conf2_reg.val = hw->esc_conf2.val;
        uhci_esc_conf3_reg_t esc_conf3_reg;
        esc_conf3_reg.val = hw->esc_conf3.val;

        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf2_reg, esc_seq1, sub_ctr->xon_chr);
        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf2_reg, esc_seq1_char0, sub_ctr->xon_sub1);
        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf2_reg, esc_seq1_char1, sub_ctr->xon_sub2);
        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf3_reg, esc_seq2, sub_ctr->xoff_chr);
        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf3_reg, esc_seq2_char0, sub_ctr->xoff_sub1);
        HAL_FORCE_MODIFY_U32_REG_FIELD(esc_conf3_reg, esc_seq2_char1, sub_ctr->xoff_sub2);
        escape_conf_reg.tx_11_esc_en = 1;
        escape_conf_reg.tx_13_esc_en = 1;
        escape_conf_reg.rx_11_esc_en = 1;
        escape_conf_reg.rx_13_esc_en = 1;
        hw->esc_conf2.val = esc_conf2_reg.val;
        hw->esc_conf3.val = esc_conf3_reg.val;
    } else {
        escape_conf_reg.tx_11_esc_en = 0;
        escape_conf_reg.tx_13_esc_en = 0;
        escape_conf_reg.rx_11_esc_en = 0;
        escape_conf_reg.rx_13_esc_en = 0;
    }
    hw->escape_conf.val = escape_conf_reg.val;
}

/**
 * @brief Enable UHCI interrupt
 *
 * @param hw Pointer to the UHCI device
 * @param intr_mask The interrupt mask to enable
 */
static inline void uhci_ll_enable_intr(uhci_dev_t *hw, uint32_t intr_mask)
{
    hw->int_ena.val |= intr_mask;
}

/**
 * @brief Disable UHCI interrupt
 *
 * @param hw Pointer to the UHCI device
 * @param intr_mask The interrupt mask to disable
 */
static inline void uhci_ll_disable_intr(uhci_dev_t *hw, uint32_t intr_mask)
{
    hw->int_ena.val &= (~intr_mask);
}

/**
 * @brief Clear UHCI interrupt
 *
 * @param hw Pointer to the UHCI device
 * @param intr_mask The interrupt mask to clear
 */
static inline void uhci_ll_clear_intr(uhci_dev_t *hw, uint32_t intr_mask)
{
    hw->int_clr.val = intr_mask;
}

/**
 * @brief Get UHCI interrupt masked status
 *
 * @param hw Pointer to the UHCI device
 * @return The interrupt masked status
 */
static inline uint32_t uhci_ll_get_intr(uhci_dev_t *hw)
{
    return hw->int_st.val;
}

/**
 * @brief Set the EOF mode for payload receive
 *
 * @param hw Pointer to the UHCI device
 * @param eof_mode The EOF mode to set. The following modes are supported:
 * - UHCI_RX_BREAK_CHR_EOF: UHCI will end payload receive process when NULL frame is received by UART.
 * - UHCI_RX_IDLE_EOF: UHCI will end payload receive process when UART has been in idle state.
 * - UHCI_RX_LEN_EOF: UHCI will end payload receive process when the receiving byte count has reached the specific value.
 */
static inline void uhci_ll_rx_set_eof_mode(uhci_dev_t *hw, uint32_t eof_mode)
{
    if (eof_mode & UHCI_RX_BREAK_CHR_EOF) {
        hw->conf0.uart_rx_brk_eof_en = 1;
    }
    if (eof_mode & UHCI_RX_IDLE_EOF) {
        hw->conf0.uart_idle_eof_en = 1;
    }
    if (eof_mode & UHCI_RX_LEN_EOF) {
        hw->conf0.len_eof_en = 1;
    }
}

/**
 * @brief Set the packet threshold (maximum value of the packet length)
 *
 * @param hw Pointer to the UHCI device
 * @param length Measurement unit: byte.
 */
static inline void uhci_ll_rx_set_packet_threshold(uhci_dev_t *hw, uint16_t length)
{
    hw->pkt_thres.pkt_thrs = length;
}

#ifdef __cplusplus
}
#endif
