/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for UHCI register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once
#include <stdio.h>
#include "hal/uhci_types.h"
#include "soc/uhci_struct.h"
#include "soc/system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UHCI_LL_GET_HW(num) (((num) == 0) ? (&UHCI0) : (NULL))

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
static inline void _uhci_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    SYSTEM.perip_clk_en0.uhci0_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define uhci_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _uhci_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the UHCI module
 *
 * @param group_id Group ID
 */
static inline void _uhci_ll_reset_register(int group_id)
{
    (void)group_id;
    SYSTEM.perip_rst_en0.uhci0_rst = 1;
    SYSTEM.perip_rst_en0.uhci0_rst = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define uhci_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; _uhci_ll_reset_register(__VA_ARGS__)

static inline void uhci_ll_init(uhci_dev_t *hw)
{
    typeof(hw->conf0) conf0_reg;
    hw->conf0.clk_en = 1;
    conf0_reg.val = 0;
    conf0_reg.clk_en = 1;
    hw->conf0.val = conf0_reg.val;
    hw->conf1.val = 0;
}

static inline void uhci_ll_attach_uart_port(uhci_dev_t *hw, int uart_num)
{
    hw->conf0.uart0_ce = (uart_num == 0) ? 1 : 0;
    hw->conf0.uart1_ce = (uart_num == 1) ? 1 : 0;
    hw->conf0.uart2_ce = (uart_num == 2) ? 1 : 0;
}

static inline void uhci_ll_set_seper_chr(uhci_dev_t *hw, uhci_seper_chr_t *seper_char)
{
    if (seper_char->sub_chr_en) {
        typeof(hw->esc_conf0) esc_conf0_reg;
        esc_conf0_reg.val = hw->esc_conf0.val;

        esc_conf0_reg.seper_char = seper_char->seper_chr;
        esc_conf0_reg.seper_esc_char0 = seper_char->sub_chr1;
        esc_conf0_reg.seper_esc_char1 = seper_char->sub_chr2;
        hw->esc_conf0.val = esc_conf0_reg.val;
        hw->escape_conf.tx_c0_esc_en = 1;
        hw->escape_conf.rx_c0_esc_en = 1;
    } else {
        hw->escape_conf.tx_c0_esc_en = 0;
        hw->escape_conf.rx_c0_esc_en = 0;
    }
}

static inline void uhci_ll_get_seper_chr(uhci_dev_t *hw, uhci_seper_chr_t *seper_chr)
{
    (void)hw;
    (void)seper_chr;
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

        esc_conf2_reg.seq1 = sub_ctr->xon_chr;
        esc_conf2_reg.seq1_char0 = sub_ctr->xon_sub1;
        esc_conf2_reg.seq1_char1 = sub_ctr->xon_sub2;
        esc_conf3_reg.seq2 = sub_ctr->xoff_chr;
        esc_conf3_reg.seq2_char0 = sub_ctr->xoff_sub1;
        esc_conf3_reg.seq2_char1 = sub_ctr->xoff_sub2;
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

static inline void uhci_ll_set_eof_mode(uhci_dev_t *hw, uint32_t eof_mode)
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

#ifdef __cplusplus
}
#endif
