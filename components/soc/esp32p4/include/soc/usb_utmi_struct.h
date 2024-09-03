/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Following register description is taken from
 * U2OPHYT40LL USB 2.0 OTG PHY specification v2.0
 */

typedef union {
    struct {
        /** clk_gate_rx : R/W; bitpos: [0]; default 2'b0;
          * Clock Gating Control Signal for Rx.
          * 2'b0 Lower power consumption
          * 2'b1 Lowest power consumption mode
          * 2'b2 Normal power consumption mode
          */
        uint32_t clk_gate_rx:2;
        /** clk_gate_tx : R/W; bitpos: [2]; default: 1'b0;
          * Clock Gating Control Signal for Rx.
          * 1'b0 Low power consumption mode
          * 1'b1 Normal power consumption mode
          */
        uint32_t clk_gate_tx:1;
        /** adj_res_fs : Reserved; bitpos: [3]; default: 0;
          * Fine tune the 45ohm termination resistor (FS)
          * Reserved
          */
        uint32_t adj_res_fs:2;
        /** adj_res_hs : R/W; bitpos: [5]; default: 3'b100;
          * Fine tune the 45ohm termination resistor (HS)
          * 3'b000 40 Ohm
          * 3'b100 45 Ohm
          * 3'b110 50 Ohm
          */
        uint32_t adj_res_hs:3;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_utmi_fc_00_reg_t;

typedef union {
    struct {
        /** adj_vref_sq : R/W; bitpos: [0]; default: 4'b0010;
          * Squelch detection threshold voltage control bits
          * 4'b0000  92 mV
          * 4'b0010 124 mV
          * 4'b0011 152 mV
          */
        uint32_t adj_vref_sq:4;
        /** adj_pw_hs : R/W; bitpos: [4]; default: 4'b1111;
          * Super power saving with reduced output swing mode control bits (for HS mode only)
          * 4'b0001 100 mV output swing
          * 4'b0011 200 mV output swing
          * 4'b0111 300 mV output swing
          * 4'b1111 400 mV output swing
          */
        uint32_t adj_pw_hs:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_utmi_fc_01_reg_t;

typedef union {
    struct {
        /** adj_iref_res : R/W; bitpos: [0]; default: 4'b0111
          * Internal bias current adjustment control bits
          * 4'b0000 125 uA
          * 4'b0111 100 uA
          * 4'b1111  78 uA
          */
        uint32_t adj_iref_res:4;
        /** adj_vsw_hs : R/W; bitpos: [4]; default: 3'b100
          * Output eye shape adjustment control bits
          * 3'b000 320 mV
          * 3'b100 400 mV
          * 3'b111 460 mV
          */
        uint32_t adj_vsw_hs:3;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} usb_utmi_fc_02_reg_t;

typedef union {
    struct {
        /** adj_pll : R/W; bitpos: [0]; default: 4'b0101
          * PLL adjustment signal
          */
        uint32_t adj_pll:4;
        /** adj_osc : R/W; bitpos: [4]; default: 3'b000
          * TX Clock phase adjust signal
          */
        uint32_t adj_txclk_phase:3;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} usb_utmi_fc_03_reg_t;

typedef union {
    struct {
        /** test_sel : R/W; bitpos: [0]; default: 8'b0
          * The PHY has test_sel register here, which normally drives DTO (Digital Test Output) signal.
          * In our implementation output of this register is left floating and DTO is driven from Probe module.
          * Thus writing to this register has no effect and is renamed to 'reserved'
          */
        uint32_t reserved:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_utmi_fc_04_reg_t;

typedef union {
    struct {
        /** rxgap_fix_en : R/W; bitpos: [0]; default: 1'b1
          * RXGAP fix enable
          */
        uint32_t rxgap_fix_en:1;
        /** counter_sel : R/W; bitpos: [1]; default: 1'b0
          * SIE_input sample enable
          */
        uint32_t counter_sel:1;
        /** clk_sel : R/W; bitpos: [2]; default: 1'b0
          * CLK60_30 source select
          */
        uint32_t clk_sel:1;
        /** phy_mode_sel : R/W; bitpos: [3]; default: 1'b0
          * PHY MODE select
          */
        uint32_t phy_mode_sel:1;
        /** uni_bidi_i : R/W; bitpos: [4]; default: 1'b0
          * UNI_BIDI signal
          */
        uint32_t uni_bidi_i:1;
        /** short_5v : R/W; bitpos: [5]; default: 1'b0
          * SHORT_5V signal
          */
        uint32_t short_5v:1;
        /** short_5v_enable : R/W; bitpos: [6]; default: 1'b1
          * SHORT_5V_ENABLE signal
          */
        uint32_t short_5v_enable:1;
        /** usable_en : R/W; bitpos: [7]; default: 1'b1
          * compare_begin delay time select
          */
        uint32_t usable_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_utmi_fc_05_reg_t;

typedef union {
    struct {
        /** ls_par_en : R/W; bitpos: [0]; default: 1'b0
          * LS mode with parallel enable
          */
        uint32_t ls_par_en:1;
        /** det_fseop_en : R/W; bitpos: [1]; default: 1'b0
          * FS EOP detect enable
          */
        uint32_t det_fseop_en:1;
        /** pre_hphy_lsie : R/W; bitpos: [2]; default: 1'b0
          * Dis_preamble enable
          */
        uint32_t pre_hphy_lsie:1;
        /** ls_kpalv_en : R/W; bitpos: [3]; default: 1'b0
          * LS mode keep alive enable
          */
        uint32_t ls_kpalv_en:1;
        /** hs_tx2rx_dly_cnt_sel : R/W; bitpos: [4]; default: 3'b100
          * PHY High-SPeed bus turn-around time select
          */
        uint32_t hs_tx2rx_dly_cnt_sel:3;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} usb_utmi_fc_06_reg_t;

typedef union {
    struct {
        /** cnt_num : R/W; bitpos: [1:0]; default: 2'b00
          * 3 ms counter select
          * 00: 392us (Default)
          * 01: 682us
          * 10: 1.36ms
          * 11: 2.72ms
          */
        uint32_t cnt_num:2;
        /** clk480_sel : R/W; bitpos: [2]; default: 1'b0
          * CLK_480 output time select
          * 0: CLK_480 is valid after a delay time when PLL is locked
          * 1: CLK_480 is valid immediately after PLL is locked
          */
        uint32_t clk480_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} usb_utmi_fc_07_reg_t;

typedef struct usb_utmi_dev_t {
    volatile usb_utmi_fc_00_reg_t fc_00;
    volatile usb_utmi_fc_01_reg_t fc_01;
    volatile usb_utmi_fc_02_reg_t fc_02;
    volatile usb_utmi_fc_03_reg_t fc_03;
    volatile usb_utmi_fc_04_reg_t fc_04;
    volatile usb_utmi_fc_05_reg_t fc_05;
    volatile usb_utmi_fc_06_reg_t fc_06;
    volatile usb_utmi_fc_07_reg_t fc_07;
} usb_utmi_dev_t;

extern usb_utmi_dev_t USB_UTMI;

#ifndef __cplusplus
_Static_assert(sizeof(usb_utmi_dev_t) == 0x20, "Invalid size of usb_utmi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
