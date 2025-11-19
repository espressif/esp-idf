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

/** Group: CNNT SYS VER DATE REG */
/** Type of sys_ver_date register
 *  NA
 */
typedef union {
    struct {
        /** sys_ver_date : R/W; bitpos: [31:0]; default: 539298086;
         *  NA
         */
        uint32_t sys_ver_date:32;
    };
    uint32_t val;
} cnnt_sys_ver_date_reg_t;


/** Group: CNNT CLK EN REG */
/** Type of clk_en register
 *  NA
 */
typedef union {
    struct {
        /** sys_date : R/W; bitpos: [30:0]; default: 38826240;
         *  need_des
         */
        uint32_t sys_date:31;
        /** sys_clk_en : R/W; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t sys_clk_en:1;
    };
    uint32_t val;
} cnnt_clk_en_reg_t;


/** Group: Configuration Register */
/** Type of sys_sdmmc_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_sdmmc_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures sdmmc memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_sdmmc_mem_lp_mode:2;
        /** sys_sdmmc_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down sdmmc memory.
         */
        uint32_t sys_sdmmc_mem_lp_en:1;
        /** sys_sdmmc_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control sdmmc memory, disable hardware control.
         */
        uint32_t sys_sdmmc_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cnnt_sys_sdmmc_mem_lp_ctrl_reg_t;

/** Type of sys_usb_otghs_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_usb_otghs_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures usb_otghs memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_usb_otghs_mem_lp_mode:2;
        /** sys_usb_otghs_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down usb_otghs memory.
         */
        uint32_t sys_usb_otghs_mem_lp_en:1;
        /** sys_usb_otghs_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control usb_otghs memory, disable hardware control.
         */
        uint32_t sys_usb_otghs_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cnnt_sys_usb_otghs_mem_lp_ctrl_reg_t;

/** Type of sys_usb_device_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_usb_device_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures usb_device memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_usb_device_mem_lp_mode:2;
        /** sys_usb_device_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down usb_device memory.
         */
        uint32_t sys_usb_device_mem_lp_en:1;
        /** sys_usb_device_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control usb_device (serial jtag) memory, disable
         *  hardware control.
         */
        uint32_t sys_usb_device_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cnnt_sys_usb_device_mem_lp_ctrl_reg_t;

/** Type of sys_gmac_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_gmac_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures gmac memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_gmac_mem_lp_mode:2;
        /** sys_gmac_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down gmac memory.
         */
        uint32_t sys_gmac_mem_lp_en:1;
        /** sys_gmac_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control gmac memory, disable hardware control.
         */
        uint32_t sys_gmac_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cnnt_sys_gmac_mem_lp_ctrl_reg_t;


/** Group: configure_register */
/** Type of sys_sprf_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t sys_sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} cnnt_sys_sprf_mem_aux_ctrl_reg_t;

/** Type of sys_sdprf_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_sdprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t sys_sdprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} cnnt_sys_sdprf_mem_aux_ctrl_reg_t;

/** Type of sys_hp_pad_ulpi_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** sys_hp_pad_ulpi_clk_out_inv_en : R/W; bitpos: [30]; default: 0;
         *  PAD ULPI Clock output inv Enable, valid only when CNNT_SYS_HP_PAD_ULPI_CLK_OUT_EN
         *  is 1.
         */
        uint32_t sys_hp_pad_ulpi_clk_out_inv_en:1;
        /** sys_hp_pad_ulpi_clk_out_en : R/W; bitpos: [31]; default: 0;
         *  PAD ULPI Clock output Enable. 0: pad_ulpi_clk is used as input clk, 1: pad_ulpi_clk
         *  is used as output clk
         */
        uint32_t sys_hp_pad_ulpi_clk_out_en:1;
    };
    uint32_t val;
} cnnt_sys_hp_pad_ulpi_ctrl_reg_t;

/** Type of sys_usb_clk_ctrl register
 *  HP USB Clock Reset Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** sys_usb_48m_div_num : R/W; bitpos: [15:8]; default: 9;
         *  usb 480m to 25m divide number.
         */
        uint32_t sys_usb_48m_div_num:8;
        /** sys_usb_25m_div_num : R/W; bitpos: [23:16]; default: 19;
         *  usb 500m to 25m divide number.
         */
        uint32_t sys_usb_25m_div_num:8;
        /** sys_usb_12m_div_num : R/W; bitpos: [31:24]; default: 39;
         *  usb 480m to 12m divide number.
         */
        uint32_t sys_usb_12m_div_num:8;
    };
    uint32_t val;
} cnnt_sys_usb_clk_ctrl_reg_t;

/** Type of sys_usb_otg20_ctrl register
 *  HP USB Clock Reset Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** sys_usb_otg20_utmifs_clk_en : R/W; bitpos: [23]; default: 1;
         *  usb otg20 utmifs clk enable.
         */
        uint32_t sys_usb_otg20_utmifs_clk_en:1;
        /** sys_usb_otg20_ulpi_clk_en : R/W; bitpos: [24]; default: 0;
         *  usb otg20 ulpi clk enable.
         */
        uint32_t sys_usb_otg20_ulpi_clk_en:1;
        /** sys_usb_otg20_phyref_clk_src_sel : R/W; bitpos: [26:25]; default: 0;
         *  usb otg20 hs phy src sel. 2''d0: 12m, 2''d1: 25m, 2''d2: pad_hsphy_refclk.'
         */
        uint32_t sys_usb_otg20_phyref_clk_src_sel:2;
        /** sys_usb_otg20_phyref_clk_en : R/W; bitpos: [27]; default: 0;
         *  usb otg20 hs phy refclk enable.
         */
        uint32_t sys_usb_otg20_phyref_clk_en:1;
        uint32_t reserved_28:1;
        /** sys_usb_otg20_phy_rst_en : R/W; bitpos: [29]; default: 1;
         *  usb otg20 phy reset en
         */
        uint32_t sys_usb_otg20_phy_rst_en:1;
        /** sys_usb_otg20_ahb_rst_en : R/W; bitpos: [30]; default: 1;
         *  usb otg20 ahb reset en
         */
        uint32_t sys_usb_otg20_ahb_rst_en:1;
        /** sys_usb_otg20_apb_rst_en : R/W; bitpos: [31]; default: 1;
         *  usb otg20 apb reset en
         */
        uint32_t sys_usb_otg20_apb_rst_en:1;
    };
    uint32_t val;
} cnnt_sys_usb_otg20_ctrl_reg_t;

/** Type of sys_hp_usb_device_ctrl register
 *  HP USB Clock Reset Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** sys_usb_device_48m_clk_en : R/W; bitpos: [30]; default: 1;
         *  usb device fs phy clock enable.
         */
        uint32_t sys_usb_device_48m_clk_en:1;
        /** sys_usb_device_rst_en : R/W; bitpos: [31]; default: 0;
         *  usb device reset en
         */
        uint32_t sys_usb_device_rst_en:1;
    };
    uint32_t val;
} cnnt_sys_hp_usb_device_ctrl_reg_t;

/** Type of sys_hp_sdmmc_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** sys_sdmmc_ahb2axi_post_write_en : R/W; bitpos: [29]; default: 0;
         *  this reg is used to enable the post_write function of the ahb2axi bridge for sdmmc
         */
        uint32_t sys_sdmmc_ahb2axi_post_write_en:1;
        /** sys_sdmmc_rst_en : R/W; bitpos: [30]; default: 0;
         *  hp sdmmc reset en
         */
        uint32_t sys_sdmmc_rst_en:1;
        /** sys_sdmmc_force_norst : R/W; bitpos: [31]; default: 0;
         *  hp sdmmc force norst
         */
        uint32_t sys_sdmmc_force_norst:1;
    };
    uint32_t val;
} cnnt_sys_hp_sdmmc_ctrl_reg_t;

/** Type of sys_hp_emac_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_emac_useless_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_useless_clk_en:1;
        /** sys_emac_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rst_en:1;
        /** sys_emac_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cnnt_sys_hp_emac_ctrl_reg_t;

/** Type of sys_hp_emac_ref_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_emac_ref_clk_sel : R/W; bitpos: [1:0]; default: 0;
         *  select emac reference clock source. 0: mpll 500MHz, 1: cpll 300MHz.
         */
        uint32_t sys_emac_ref_clk_sel:2;
        /** sys_emac_ref_clk_en : R/W; bitpos: [2]; default: 0;
         *  Set as 1 to enable emac reference clock source selection
         */
        uint32_t sys_emac_ref_clk_en:1;
        /** sys_emac_ref_50m_pad_out_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_ref_50m_pad_out_clk_en:1;
        uint32_t reserved_4:4;
        /** sys_emac_ref_clk_div_num : R/W; bitpos: [15:8]; default: 0;
         *  Divide number of the divider which is used to divide reference clock source.
         */
        uint32_t sys_emac_ref_clk_div_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cnnt_sys_hp_emac_ref_ctrl_reg_t;

/** Type of sys_hp_emac_rmii_pad_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** sys_emac_rmii_pad_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rmii_pad_clk_en:1;
        /** sys_emac_rmii_pad_clk_inv_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rmii_pad_clk_inv_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cnnt_sys_hp_emac_rmii_pad_ctrl_reg_t;

/** Type of sys_hp_emac_rmii_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_emac_rmii_clk_sel : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rmii_clk_sel:1;
        /** sys_emac_rmii_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rmii_clk_en:1;
        /** sys_emac_rmii_pad_out_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rmii_pad_out_clk_en:1;
        uint32_t reserved_3:5;
        /** sys_emac_rxtx_clk_div_num : R/W; bitpos: [12:8]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rxtx_clk_div_num:5;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_sys_hp_emac_rmii_ctrl_reg_t;

/** Type of sys_hp_emac_rx_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_emac_rx_pad_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rx_pad_clk_en:1;
        /** sys_emac_rx_pad_clk_inv_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rx_pad_clk_inv_en:1;
        /** sys_emac_rx_clk_sel : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rx_clk_sel:1;
        /** sys_emac_rx_180_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_rx_180_clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cnnt_sys_hp_emac_rx_ctrl_reg_t;

/** Type of sys_hp_emac_tx_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_emac_tx_pad_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_tx_pad_clk_en:1;
        /** sys_emac_tx_pad_clk_inv_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_tx_pad_clk_inv_en:1;
        /** sys_emac_tx_clk_sel : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_tx_clk_sel:1;
        /** sys_emac_tx_180_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t sys_emac_tx_180_clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cnnt_sys_hp_emac_tx_ctrl_reg_t;

/** Type of sys_hp_emac_ptp_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_emac_ptp_ref_clk_en : R/W; bitpos: [0]; default: 0;
         *  Set as 1 to enable emac PTP clock source selection.
         */
        uint32_t sys_emac_ptp_ref_clk_en:1;
        /** sys_emac_ptp_ref_clk_sel : R/W; bitpos: [1]; default: 0;
         *  select emac PTP clock source. 0: xtal clock, 1: 80MHz internal clock.
         */
        uint32_t sys_emac_ptp_ref_clk_sel:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cnnt_sys_hp_emac_ptp_ctrl_reg_t;

/** Type of sys_spram_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t sys_spram_mem_aux_ctrl:32;
    };
    uint32_t val;
} cnnt_sys_spram_mem_aux_ctrl_reg_t;


/** Group: cnnt_iomux_ctrl */
/** Type of sys_cnnt_iomux_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** sys_reg_cnnt_iomux_apb_rst_en : R/W; bitpos: [0]; default: 0;
         *  cnnt_iomux rst en
         */
        uint32_t sys_reg_cnnt_iomux_apb_rst_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cnnt_sys_cnnt_iomux_ctrl0_reg_t;


/** Group: GMAC Control Register */
/** Type of sys_gmac_ctrl0 register
 *  N/A
 */
typedef union {
    struct {
        /** sys_ptp_pps : RO; bitpos: [0]; default: 0;
         *  N/A
         */
        uint32_t sys_ptp_pps:1;
        /** sys_sbd_flowctrl : R/W; bitpos: [1]; default: 0;
         *  N/A
         */
        uint32_t sys_sbd_flowctrl:1;
        /** sys_phy_intf_sel : R/W; bitpos: [4:2]; default: 0;
         *  N/A
         */
        uint32_t sys_phy_intf_sel:3;
        /** sys_gmac_mem_clk_force_on : R/W; bitpos: [5]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_mem_clk_force_on:1;
        /** sys_gmac_rst_clk_tx_n : RO; bitpos: [6]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_rst_clk_tx_n:1;
        /** sys_gmac_rst_clk_rx_n : RO; bitpos: [7]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_rst_clk_rx_n:1;
        /** sys_gmac_cactive : RO; bitpos: [8]; default: 0;
         *  this signal indicates that the GMAC-AXI requires its clock signal , 1 : required ,0
         *  : not required
         */
        uint32_t sys_gmac_cactive:1;
        /** sys_gmac_csysack : RO; bitpos: [9]; default: 0;
         *  this signal is the acknowledgement from the GMAC-AXI to the system for low-power
         *  request ,active high
         */
        uint32_t sys_gmac_csysack:1;
        /** sys_gmac_csysreq : R/W; bitpos: [10]; default: 0;
         *  clock controller system low-power request , when asserted low ,indicates the
         *  controller hs requested the GMAX-AXI to enter a lowpower state
         */
        uint32_t sys_gmac_csysreq:1;
        /** sys_gmac_apb_postw_en : R/W; bitpos: [11]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_apb_postw_en:1;
        /** sys_gmac_mem_prdyn : RO; bitpos: [12]; default: 1;
         *  record memory power status
         */
        uint32_t sys_gmac_mem_prdyn:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_sys_gmac_ctrl0_reg_t;

/** Type of sys_gmac_ctrl1 register
 *  N/A
 */
typedef union {
    struct {
        /** sys_ptp_timestamp_l : RO; bitpos: [31:0]; default: 0;
         *  N/A
         */
        uint32_t sys_ptp_timestamp_l:32;
    };
    uint32_t val;
} cnnt_sys_gmac_ctrl1_reg_t;

/** Type of sys_gmac_ctrl2 register
 *  N/A
 */
typedef union {
    struct {
        /** sys_ptp_timestamp_h : RO; bitpos: [31:0]; default: 0;
         *  N/A
         */
        uint32_t sys_ptp_timestamp_h:32;
    };
    uint32_t val;
} cnnt_sys_gmac_ctrl2_reg_t;


typedef struct {
    volatile cnnt_sys_ver_date_reg_t sys_ver_date;
    volatile cnnt_clk_en_reg_t clk_en;
    uint32_t reserved_008[2];
    volatile cnnt_sys_sdmmc_mem_lp_ctrl_reg_t sys_sdmmc_mem_lp_ctrl;
    volatile cnnt_sys_usb_otghs_mem_lp_ctrl_reg_t sys_usb_otghs_mem_lp_ctrl;
    volatile cnnt_sys_usb_device_mem_lp_ctrl_reg_t sys_usb_device_mem_lp_ctrl;
    volatile cnnt_sys_gmac_mem_lp_ctrl_reg_t sys_gmac_mem_lp_ctrl;
    volatile cnnt_sys_sprf_mem_aux_ctrl_reg_t sys_sprf_mem_aux_ctrl;
    volatile cnnt_sys_sdprf_mem_aux_ctrl_reg_t sys_sdprf_mem_aux_ctrl;
    volatile cnnt_sys_hp_pad_ulpi_ctrl_reg_t sys_hp_pad_ulpi_ctrl;
    volatile cnnt_sys_usb_clk_ctrl_reg_t sys_usb_clk_ctrl;
    volatile cnnt_sys_usb_otg20_ctrl_reg_t sys_usb_otg20_ctrl;
    volatile cnnt_sys_hp_usb_device_ctrl_reg_t sys_hp_usb_device_ctrl;
    volatile cnnt_sys_hp_sdmmc_ctrl_reg_t sys_hp_sdmmc_ctrl;
    volatile cnnt_sys_hp_emac_ctrl_reg_t sys_hp_emac_ctrl;
    volatile cnnt_sys_hp_emac_ref_ctrl_reg_t sys_hp_emac_ref_ctrl;
    volatile cnnt_sys_hp_emac_rmii_pad_ctrl_reg_t sys_hp_emac_rmii_pad_ctrl;
    volatile cnnt_sys_hp_emac_rmii_ctrl_reg_t sys_hp_emac_rmii_ctrl;
    volatile cnnt_sys_hp_emac_rx_ctrl_reg_t sys_hp_emac_rx_ctrl;
    volatile cnnt_sys_hp_emac_tx_ctrl_reg_t sys_hp_emac_tx_ctrl;
    volatile cnnt_sys_hp_emac_ptp_ctrl_reg_t sys_hp_emac_ptp_ctrl;
    volatile cnnt_sys_cnnt_iomux_ctrl0_reg_t sys_cnnt_iomux_ctrl0;
    volatile cnnt_sys_spram_mem_aux_ctrl_reg_t sys_spram_mem_aux_ctrl;
    volatile cnnt_sys_gmac_ctrl0_reg_t sys_gmac_ctrl0;
    volatile cnnt_sys_gmac_ctrl1_reg_t sys_gmac_ctrl1;
    volatile cnnt_sys_gmac_ctrl2_reg_t sys_gmac_ctrl2;
} cnnt_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(cnnt_dev_t) == 0x6c, "Invalid size of cnnt_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
