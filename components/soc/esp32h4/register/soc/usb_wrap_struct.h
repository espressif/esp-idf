/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: USB wrapper registers. */
/** Type of wrap_otg_conf register
 *  USB wrapper configuration registers.
 */
typedef union {
    struct {
        /** wrap_srp_sessend_override : R/W; bitpos: [0]; default: 0;
         *  This bit is used to enable the software over-ride of srp session end signal. 1'b0:
         *  the signal is controlled by the chip input, 1'b1: the signal is controlled by the
         *  software.
         */
        uint32_t wrap_srp_sessend_override:1;
        /** wrap_srp_sessend_value : R/W; bitpos: [1]; default: 0;
         *  Software over-ride value of srp session end signal.
         */
        uint32_t wrap_srp_sessend_value:1;
        /** wrap_phy_sel : R/W; bitpos: [2]; default: 0;
         *  Select internal external PHY. 1'b0: Select internal PHY, 1'b1: Select external PHY.
         */
        uint32_t wrap_phy_sel:1;
        uint32_t reserved_3:1;
        /** wrap_dbnce_fltr_bypass : R/W; bitpos: [4]; default: 0;
         *  Bypass Debounce filters for avalid,bvalid,vbusvalid,session end, id signals
         */
        uint32_t wrap_dbnce_fltr_bypass:1;
        /** wrap_exchg_pins_override : R/W; bitpos: [5]; default: 0;
         *  Enable software controlle USB D+ D- exchange
         */
        uint32_t wrap_exchg_pins_override:1;
        /** wrap_exchg_pins : R/W; bitpos: [6]; default: 0;
         *  USB D+ D- exchange. 1'b0: don't change, 1'b1: exchange D+ D-.
         */
        uint32_t wrap_exchg_pins:1;
        /** wrap_vrefh : R/W; bitpos: [8:7]; default: 0;
         *  Control single-end input high threshold,1.76V to 2V, step 80mV.
         */
        uint32_t wrap_vrefh:2;
        /** wrap_vrefl : R/W; bitpos: [10:9]; default: 0;
         *  Control single-end input low threshold,0.8V to 1.04V, step 80mV.
         */
        uint32_t wrap_vrefl:2;
        /** wrap_vref_override : R/W; bitpos: [11]; default: 0;
         *  Enable software controlle input  threshold.
         */
        uint32_t wrap_vref_override:1;
        /** wrap_pad_pull_override : R/W; bitpos: [12]; default: 0;
         *  Enable software controlle USB D+ D- pullup pulldown.
         */
        uint32_t wrap_pad_pull_override:1;
        /** wrap_dp_pullup : R/W; bitpos: [13]; default: 0;
         *  Controlle USB D+ pullup.
         */
        uint32_t wrap_dp_pullup:1;
        /** wrap_dp_pulldown : R/W; bitpos: [14]; default: 0;
         *  Controlle USB D+ pulldown.
         */
        uint32_t wrap_dp_pulldown:1;
        /** wrap_dm_pullup : R/W; bitpos: [15]; default: 0;
         *  Controlle USB D+ pullup.
         */
        uint32_t wrap_dm_pullup:1;
        /** wrap_dm_pulldown : R/W; bitpos: [16]; default: 0;
         *  Controlle USB D+ pulldown.
         */
        uint32_t wrap_dm_pulldown:1;
        /** wrap_pullup_value : R/W; bitpos: [17]; default: 0;
         *  Controlle pullup value. 1'b0: typical value is 2.4K, 1'b1: typical value is 1.2K.
         */
        uint32_t wrap_pullup_value:1;
        /** wrap_usb_pad_enable : R/W; bitpos: [18]; default: 0;
         *  Enable USB pad function.
         */
        uint32_t wrap_usb_pad_enable:1;
        /** wrap_ahb_clk_force_on : R/W; bitpos: [19]; default: 0;
         *  Force ahb clock always on.
         */
        uint32_t wrap_ahb_clk_force_on:1;
        /** wrap_phy_clk_force_on : R/W; bitpos: [20]; default: 1;
         *  Force phy clock always on.
         */
        uint32_t wrap_phy_clk_force_on:1;
        uint32_t reserved_21:10;
        /** wrap_clk_en : R/W; bitpos: [31]; default: 0;
         *  Disable auto clock gating of CSR registers.
         */
        uint32_t wrap_clk_en:1;
    };
    uint32_t val;
} usb_wrap_otg_conf_reg_t;

/** Type of wrap_date register
 *  Date register.
 */
typedef union {
    struct {
        /** wrap_usb_wrap_date : R/W; bitpos: [31:0]; default: 37761536;
         *  Date register.
         */
        uint32_t wrap_usb_wrap_date:32;
    };
    uint32_t val;
} usb_wrap_date_reg_t;


typedef struct {
    volatile usb_wrap_otg_conf_reg_t wrap_otg_conf;
    uint32_t reserved_004[254];
    volatile usb_wrap_date_reg_t wrap_date;
} usb_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(usb_dev_t) == 0x400, "Invalid size of usb_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
