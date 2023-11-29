/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: clk_en */
/** Type of clk_en0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} usb_otg_misc_clk_en0_reg_t;


/** Group: date */
/** Type of date0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [31:0]; default: 23050900;
         *  Reserved
         */
        uint32_t reg_date:32;
    };
    uint32_t val;
} usb_otg_misc_date0_reg_t;


/** Group: core_ahb_ctrl */
/** Type of core_ahb_ctrl0 register
 *  USB OTG core AHB bus control.
 */
typedef union {
    struct {
        /** reg_core_s_hbigendian : R/W; bitpos: [0]; default: 0;
         *  USB OTG core AHB slave big endian mode. 1'b0: Little, 1'b1: Big.
         */
        uint32_t reg_core_s_hbigendian:1;
        /** reg_core_m_hbigendian : R/W; bitpos: [1]; default: 0;
         *  USB OTG core AHB master big endian mode. 1'b0: Little, 1'b1: Big.
         */
        uint32_t reg_core_m_hbigendian:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} usb_otg_misc_core_ahb_ctrl0_reg_t;


/** Group: dfifo_ctrl */
/** Type of dfifo_ctrl0 register
 *  dfifo control.
 */
typedef union {
    struct {
        /** reg_dfifo_hclk_force_on : R/W; bitpos: [0]; default: 0;
         *  enable dfifo hclk always on.
         */
        uint32_t reg_dfifo_hclk_force_on:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} usb_otg_misc_dfifo_ctrl0_reg_t;


/** Group: core_ss_ctrl */
/** Type of core_ss_ctrl0 register
 *  USB OTG core simulation scale control.
 */
typedef union {
    struct {
        /** reg_ss_scaledown_mode : R/W; bitpos: [1:0]; default: 0;
         *  USB OTG 2.0 Core Simulation Scale Down Mode, Scale-down timing values, resulting in
         *  <faster> simulations.
         */
        uint32_t reg_ss_scaledown_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} usb_otg_misc_core_ss_ctrl0_reg_t;


/** Group: phy_ctrl */
/** Type of phy_ctrl0 register
 *  USB PHY auxiliary control.
 */
typedef union {
    struct {
        /** reg_phy_pll_en_override : R/W; bitpos: [0]; default: 0;
         *  Use software to override phy_pll_en.
         */
        uint32_t reg_phy_pll_en_override:1;
        /** reg_phy_pll_en : R/W; bitpos: [1]; default: 0;
         *  Software phy_pll_en.
         */
        uint32_t reg_phy_pll_en:1;
        /** reg_phy_suspendm_override : R/W; bitpos: [2]; default: 0;
         *  Use software to override phy_suspendm.
         */
        uint32_t reg_phy_suspendm_override:1;
        /** reg_phy_suspendm : R/W; bitpos: [3]; default: 0;
         *  Software phy_suspendm.
         */
        uint32_t reg_phy_suspendm:1;
        /** reg_phy_reset_n_override : R/W; bitpos: [4]; default: 0;
         *  Use software to override phy_reset_n.
         */
        uint32_t reg_phy_reset_n_override:1;
        /** reg_phy_reset_n : R/W; bitpos: [5]; default: 0;
         *  Software phy_reset_n.
         */
        uint32_t reg_phy_reset_n:1;
        /** reg_phy_bist_ok : RO; bitpos: [6]; default: 0;
         *  USB PHY self test done.
         */
        uint32_t reg_phy_bist_ok:1;
        /** reg_phy_otg_suspendm : R/W; bitpos: [7]; default: 0;
         *  USB PHY otg_suspendm.
         */
        uint32_t reg_phy_otg_suspendm:1;
        /** reg_phy_refclk_mode : R/W; bitpos: [8]; default: 1;
         *  Select USB PHY refclk mode. 0: refclk is 25MHz, 1: refclk is 12MHz.
         */
        uint32_t reg_phy_refclk_mode:1;
        /** reg_phy_self_test : R/W; bitpos: [9]; default: 0;
         *  USB PHY self test enable.
         */
        uint32_t reg_phy_self_test:1;
        /** reg_phy_txbitstuff_enable : R/W; bitpos: [10]; default: 0;
         *  USB PHY tx bitstuff enable.
         */
        uint32_t reg_phy_txbitstuff_enable:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} usb_otg_misc_phy_ctrl0_reg_t;


/** Group: phy_dbg_probe */
/** Type of phy_dbg_probe0 register
 *  USB PHY debug probe register.
 */
typedef union {
    struct {
        /** reg_phy_dbg_line_state : RO; bitpos: [1:0]; default: 0;
         *  Reserved.
         */
        uint32_t reg_phy_dbg_line_state:2;
        /** reg_phy_dbg_rx_valid : RO; bitpos: [2]; default: 0;
         *  Reserved.
         */
        uint32_t reg_phy_dbg_rx_valid:1;
        /** reg_phy_dbg_rx_validh : RO; bitpos: [3]; default: 0;
         *  Reserved.
         */
        uint32_t reg_phy_dbg_rx_validh:1;
        /** reg_phy_dbg_rx_active : RO; bitpos: [4]; default: 0;
         *  Reserved.
         */
        uint32_t reg_phy_dbg_rx_active:1;
        /** reg_phy_dbg_rx_error : RO; bitpos: [5]; default: 0;
         *  Reserved.
         */
        uint32_t reg_phy_dbg_rx_error:1;
        /** reg_phy_dbg_tx_ready : RO; bitpos: [6]; default: 0;
         *  Reserved.
         */
        uint32_t reg_phy_dbg_tx_ready:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} usb_otg_misc_phy_dbg_probe0_reg_t;


/** Group: Interrupt */
/** Type of phy_int_raw register
 *  Interrupt raw of USB PHY interrupt register.
 */
typedef union {
    struct {
        /** reg_iddig_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  Interrupt raw of reg_iddig_int_st
         */
        uint32_t reg_iddig_int_raw:1;
        /** reg_vbus_valid_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
         *  Interrupt raw of reg_vbus_valid_int_st
         */
        uint32_t reg_vbus_valid_int_raw:1;
        /** reg_sessvalid_int_raw : R/SS/WTC; bitpos: [2]; default: 0;
         *  Interrupt raw of reg_sessvalid_int_st
         */
        uint32_t reg_sessvalid_int_raw:1;
        /** reg_sessend_int_raw : R/SS/WTC; bitpos: [3]; default: 0;
         *  Interrupt raw of reg_sessend_int_st
         */
        uint32_t reg_sessend_int_raw:1;
        /** reg_bvalid_int_raw : R/SS/WTC; bitpos: [4]; default: 0;
         *  Interrupt raw of reg_bvalid_int_st
         */
        uint32_t reg_bvalid_int_raw:1;
        /** reg_host_disconnect_int_raw : R/SS/WTC; bitpos: [5]; default: 0;
         *  Interrupt raw of reg_host_disconnect_int_st
         */
        uint32_t reg_host_disconnect_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} usb_otg_misc_phy_int_raw_reg_t;

/** Type of phy_int_clr register
 *  Interrupt clear of USB PHY interrupt register.
 */
typedef union {
    struct {
        /** reg_iddig_int_clr : WT; bitpos: [0]; default: 0;
         *  Interrupt clear of reg_iddig_int_st
         */
        uint32_t reg_iddig_int_clr:1;
        /** reg_vbus_valid_int_clr : WT; bitpos: [1]; default: 0;
         *  Interrupt clear of reg_vbus_valid_int_st
         */
        uint32_t reg_vbus_valid_int_clr:1;
        /** reg_sessvalid_int_clr : WT; bitpos: [2]; default: 0;
         *  Interrupt clear of reg_sessvalid_int_st
         */
        uint32_t reg_sessvalid_int_clr:1;
        /** reg_sessend_int_clr : WT; bitpos: [3]; default: 0;
         *  Interrupt clear of reg_sessend_int_st
         */
        uint32_t reg_sessend_int_clr:1;
        /** reg_bvalid_int_clr : WT; bitpos: [4]; default: 0;
         *  Interrupt clear of reg_bvalid_int_st
         */
        uint32_t reg_bvalid_int_clr:1;
        /** reg_host_disconnect_int_clr : WT; bitpos: [5]; default: 0;
         *  Interrupt clear of reg_host_disconnect_int_st
         */
        uint32_t reg_host_disconnect_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} usb_otg_misc_phy_int_clr_reg_t;

/** Type of phy_int_ena register
 *  Interrupt enable of USB PHY interrupt register.
 */
typedef union {
    struct {
        /** reg_iddig_int_ena : R/W; bitpos: [0]; default: 0;
         *  Interrupt enable of reg_iddig_int_st
         */
        uint32_t reg_iddig_int_ena:1;
        /** reg_vbus_valid_int_ena : R/W; bitpos: [1]; default: 0;
         *  Interrupt enable of reg_vbus_valid_int_st
         */
        uint32_t reg_vbus_valid_int_ena:1;
        /** reg_sessvalid_int_ena : R/W; bitpos: [2]; default: 0;
         *  Interrupt enable of reg_sessvalid_int_st
         */
        uint32_t reg_sessvalid_int_ena:1;
        /** reg_sessend_int_ena : R/W; bitpos: [3]; default: 0;
         *  Interrupt enable of reg_sessend_int_st
         */
        uint32_t reg_sessend_int_ena:1;
        /** reg_bvalid_int_ena : R/W; bitpos: [4]; default: 0;
         *  Interrupt enable of reg_bvalid_int_st
         */
        uint32_t reg_bvalid_int_ena:1;
        /** reg_host_disconnect_int_ena : R/W; bitpos: [5]; default: 0;
         *  Interrupt enable of reg_host_disconnect_int_st
         */
        uint32_t reg_host_disconnect_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} usb_otg_misc_phy_int_ena_reg_t;

/** Type of phy_int_st register
 *  USB PHY interrupt register.
 */
typedef union {
    struct {
        /** reg_iddig_int_st : RO; bitpos: [0]; default: 0;
         *  indicates connected plug is a mini-A or mini-B.
         */
        uint32_t reg_iddig_int_st:1;
        /** reg_vbus_valid_int_st : RO; bitpos: [1]; default: 0;
         *  indicates if the voltage on VBUS is at a valid level for operation, 0: VBUS < 4.4V,
         *  1: VBUS > 4.75V.
         */
        uint32_t reg_vbus_valid_int_st:1;
        /** reg_sessvalid_int_st : RO; bitpos: [2]; default: 0;
         *  indicates if the session for an peripheral is valid, 0: VBUS < 0.8V, 1: VBUS > 2.0V.
         */
        uint32_t reg_sessvalid_int_st:1;
        /** reg_sessend_int_st : RO; bitpos: [3]; default: 0;
         *  indicates the voltage on VBUS, 1: VBUS < 0.2V, 0: VBUS > 0.8V.
         */
        uint32_t reg_sessend_int_st:1;
        /** reg_bvalid_int_st : RO; bitpos: [4]; default: 0;
         *  indicates the voltage on VBUS, 0: VBUS < 0.8V, 1: VBUS > 4.0V.
         */
        uint32_t reg_bvalid_int_st:1;
        /** reg_host_disconnect_int_st : RO; bitpos: [5]; default: 0;
         *  host disconnect.
         */
        uint32_t reg_host_disconnect_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} usb_otg_misc_phy_int_st_reg_t;


/** Group: wakeup_ctrl */
/** Type of wakeup_ctrl0 register
 *  USB wakeup control.
 */
typedef union {
    struct {
        /** reg_usb_in_suspend : R/W; bitpos: [0]; default: 0;
         *  indicate usb is in suspend state
         */
        uint32_t reg_usb_in_suspend:1;
        /** reg_usb_wkup_clr : WT; bitpos: [1]; default: 0;
         *  clear usb wakeup signals.
         */
        uint32_t reg_usb_wkup_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} usb_otg_misc_wakeup_ctrl0_reg_t;


typedef struct {
    volatile usb_otg_misc_clk_en0_reg_t clk_en0;
    volatile usb_otg_misc_date0_reg_t date0;
    volatile usb_otg_misc_core_ahb_ctrl0_reg_t core_ahb_ctrl0;
    volatile usb_otg_misc_dfifo_ctrl0_reg_t dfifo_ctrl0;
    volatile usb_otg_misc_core_ss_ctrl0_reg_t core_ss_ctrl0;
    volatile usb_otg_misc_phy_ctrl0_reg_t phy_ctrl0;
    volatile usb_otg_misc_phy_dbg_probe0_reg_t phy_dbg_probe0;
    volatile usb_otg_misc_phy_int_raw_reg_t phy_int_raw;
    volatile usb_otg_misc_phy_int_clr_reg_t phy_int_clr;
    volatile usb_otg_misc_phy_int_ena_reg_t phy_int_ena;
    volatile usb_otg_misc_phy_int_st_reg_t phy_int_st;
    volatile usb_otg_misc_wakeup_ctrl0_reg_t wakeup_ctrl0;
} usb_otg_misc_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(usb_otg_misc_dev_t) == 0x30, "Invalid size of usb_otg_misc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
