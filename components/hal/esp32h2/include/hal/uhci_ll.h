/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdio.h>
#include "hal/uhci_types.h"
#include "soc/uhci_struct.h"
#include "soc/pcr_struct.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UHCI_LL_GET_HW(num) (((num) == 0) ? (&UHCI0) : (NULL))
#define UHCI_LL_MAX_RECEIVE_PACKET_THRESHOLD  (8192)

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

static inline void uhci_ll_init(uhci_dev_t *hw)
{
    typeof(hw->conf0) conf0_reg;
    conf0_reg.val = 0;
    conf0_reg.clk_en = 1;
    hw->conf0.val = conf0_reg.val;
    hw->conf1.val = 0;
}

static inline void uhci_ll_attach_uart_port(uhci_dev_t *hw, int uart_num)
{
    hw->conf0.uart0_ce = (uart_num == 0) ? 1 : 0;
    hw->conf0.uart1_ce = (uart_num == 1) ? 1 : 0;
}

static inline void uhci_ll_set_seper_chr(uhci_dev_t *hw, uhci_seper_chr_t *seper_char)
{
    if (seper_char->sub_chr_en) {
        hw->conf0.seper_en = 1;
        typeof(hw->esc_conf0) esc_conf0_reg;
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

static inline void uhci_ll_set_swflow_ctrl_sub_chr(uhci_dev_t *hw, uhci_swflow_ctrl_sub_chr_t *sub_ctr)
{
    typeof(hw->escape_conf) escape_conf_reg;
    escape_conf_reg.val = hw->escape_conf.val;

    if (sub_ctr->flow_en == 1) {
        typeof(hw->esc_conf2) esc_conf2_reg;
        esc_conf2_reg.val = hw->esc_conf2.val;
        typeof(hw->esc_conf3) esc_conf3_reg;
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

static inline void uhci_ll_enable_intr(uhci_dev_t *hw, uint32_t intr_mask)
{
    hw->int_ena.val |= intr_mask;
}

static inline void uhci_ll_disable_intr(uhci_dev_t *hw, uint32_t intr_mask)
{
    hw->int_ena.val &= (~intr_mask);
}

static inline void uhci_ll_clear_intr(uhci_dev_t *hw, uint32_t intr_mask)
{
    hw->int_clr.val = intr_mask;
}

static inline uint32_t uhci_ll_get_intr(uhci_dev_t *hw)
{
    return hw->int_st.val;
}

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

static inline void uhci_ll_rx_set_packet_threshold(uhci_dev_t *hw, uint16_t length)
{
    hw->pkt_thres.pkt_thrs = length;
}

#ifdef __cplusplus
}
#endif
