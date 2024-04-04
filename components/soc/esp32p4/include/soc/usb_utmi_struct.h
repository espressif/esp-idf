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
        /** adj_osc : R/W; bitpos: [4]; default: 2'b00
          * PLL adjustment signal
          */
        uint32_t adj_osc:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} usb_utmi_fc_03_reg_t;

typedef union {
    struct {
        /** reserved_out5 : R/W; bitpos: [0]; default: 8'b0
          * RESERVED_OUT5
          */
        uint32_t reserved_out5:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_utmi_fc_04_reg_t;

typedef struct usb_utmi_dev_t {
    volatile usb_utmi_fc_00_reg_t fc_00;
    volatile usb_utmi_fc_01_reg_t fc_01;
    volatile usb_utmi_fc_02_reg_t fc_02;
    volatile usb_utmi_fc_03_reg_t fc_03;
    usb_utmi_fc_04_reg_t fc_04;
} usb_utmi_dev_t;

extern usb_utmi_dev_t USB_UTMI;

#ifndef __cplusplus
_Static_assert(sizeof(usb_utmi_dev_t) == 0x14, "Invalid size of usb_utmi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
