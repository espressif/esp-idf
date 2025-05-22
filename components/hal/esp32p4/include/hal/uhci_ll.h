/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for UHCI register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once
#include <stdio.h>
#include "hal/uhci_types.h"
#include "soc/uhci_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
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
static inline void _uhci_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_uhci_apb_clk_en = enable;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_uhci_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define uhci_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _uhci_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the UHCI module
 *
 * @param group_id Group ID
 */
static inline void _uhci_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uhci = 1;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uhci = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define uhci_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _uhci_ll_reset_register(__VA_ARGS__); \
    } while(0)

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
    hw->conf0.uart_sel = uart_num;
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
