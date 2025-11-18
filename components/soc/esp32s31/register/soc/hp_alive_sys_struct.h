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

/** Group: configure_register */
/** Type of hp_clk_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:17;
        /** hp_xtalx2_80m_clk_en : R/W; bitpos: [17]; default: 1;
         *  XTALx2 80M Clock Enable.
         */
        uint32_t hp_xtalx2_80m_clk_en:1;
        /** hp_root_clk_en : R/W; bitpos: [18]; default: 1;
         *  HP SoC Root Clock Enable.
         */
        uint32_t hp_root_clk_en:1;
        /** hp_xtal_32k_clk_en : R/W; bitpos: [19]; default: 1;
         *  XTAL 32K Clock Enable.
         */
        uint32_t hp_xtal_32k_clk_en:1;
        /** hp_rc_32k_clk_en : R/W; bitpos: [20]; default: 1;
         *  RC 32K Clock Enable.
         */
        uint32_t hp_rc_32k_clk_en:1;
        /** hp_sosc_150k_clk_en : R/W; bitpos: [21]; default: 1;
         *  SOSC 150K Clock Enable.
         */
        uint32_t hp_sosc_150k_clk_en:1;
        /** hp_pll_80m_clk_en : R/W; bitpos: [22]; default: 1;
         *  PLL 8M Clock Enable.
         */
        uint32_t hp_pll_80m_clk_en:1;
        /** hp_audio_pll_clk_en : R/W; bitpos: [23]; default: 1;
         *  AUDIO PLL Clock Enable.
         */
        uint32_t hp_audio_pll_clk_en:1;
        /** hp_sdio_pll2_clk_en : R/W; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t hp_sdio_pll2_clk_en:1;
        /** hp_sdio_pll1_clk_en : R/W; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t hp_sdio_pll1_clk_en:1;
        /** hp_sdio_pll0_clk_en : R/W; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t hp_sdio_pll0_clk_en:1;
        /** hp_fosc_20m_clk_en : R/W; bitpos: [27]; default: 1;
         *  FOSC 20M Clock Enable.
         */
        uint32_t hp_fosc_20m_clk_en:1;
        /** hp_xtal_40m_clk_en : R/W; bitpos: [28]; default: 1;
         *  XTAL 40M Clock Enable.
         */
        uint32_t hp_xtal_40m_clk_en:1;
        /** hp_cpll_300m_clk_en : R/W; bitpos: [29]; default: 1;
         *  CPLL 300M Clock Enable.
         */
        uint32_t hp_cpll_300m_clk_en:1;
        /** hp_spll_480m_clk_en : R/W; bitpos: [30]; default: 1;
         *  SPLL 480M Clock Enable.
         */
        uint32_t hp_spll_480m_clk_en:1;
        /** hp_mpll_500m_clk_en : R/W; bitpos: [31]; default: 1;
         *  MPLL 500M Clock Enable.
         */
        uint32_t hp_mpll_500m_clk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_clk_ctrl_reg_t;

/** Type of hp_pad_parlio_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** hp_pad_parlio_tx_clk_en : R/W; bitpos: [30]; default: 1;
         *  PARLIO TX Clock From Pad Enable.
         */
        uint32_t hp_pad_parlio_tx_clk_en:1;
        /** hp_pad_parlio_rx_clk_en : R/W; bitpos: [31]; default: 1;
         *  PARLIO RX Clock From Pad Enable.
         */
        uint32_t hp_pad_parlio_rx_clk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_pad_parlio_ctrl_reg_t;

/** Type of hp_pad_i2s0_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_pad_i2s0_mclk_en : R/W; bitpos: [31]; default: 1;
         *  I2S0 MCLK Clock From Pad Enable.
         */
        uint32_t hp_pad_i2s0_mclk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_pad_i2s0_ctrl_reg_t;

/** Type of hp_pad_i2s1_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_pad_i2s1_mclk_en : R/W; bitpos: [31]; default: 1;
         *  I2S1 MCLK Clock From Pad Enable.
         */
        uint32_t hp_pad_i2s1_mclk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_pad_i2s1_ctrl_reg_t;

/** Type of hp_pad_uart0_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_pad_uart0_slp_clk_en : R/W; bitpos: [31]; default: 1;
         *  UART0 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart0_slp_clk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_pad_uart0_ctrl_reg_t;

/** Type of hp_pad_uart1_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_pad_uart1_slp_clk_en : R/W; bitpos: [31]; default: 1;
         *  UART1 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart1_slp_clk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_pad_uart1_ctrl_reg_t;

/** Type of hp_pad_uart2_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_pad_uart2_slp_clk_en : R/W; bitpos: [31]; default: 1;
         *  UART2 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart2_slp_clk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_pad_uart2_ctrl_reg_t;

/** Type of hp_pad_uart3_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_pad_uart3_slp_clk_en : R/W; bitpos: [31]; default: 1;
         *  UART3 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart3_slp_clk_en:1;
    };
    uint32_t val;
} hp_alive_sys_hp_pad_uart3_ctrl_reg_t;

/** Type of modem_clk_aon register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** modem_clk_aon_sel : R/W; bitpos: [31]; default: 1;
         *  Modem clk aon sel. 0: foscl  1:xtal
         */
        uint32_t modem_clk_aon_sel:1;
    };
    uint32_t val;
} hp_alive_sys_modem_clk_aon_reg_t;

/** Type of usb_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** usb_otghs_phy_chrgvbus : R/W; bitpos: [0]; default: 0;
         *  phy chrgvbus
         */
        uint32_t usb_otghs_phy_chrgvbus:1;
        /** usb_otghs_phy_dischrgvbus : R/W; bitpos: [1]; default: 0;
         *  phy dischrgvbus
         */
        uint32_t usb_otghs_phy_dischrgvbus:1;
        /** usb_otghs_phy_dmpulldown : R/W; bitpos: [2]; default: 0;
         *  phy dmpulldown
         */
        uint32_t usb_otghs_phy_dmpulldown:1;
        /** usb_otghs_phy_dppulldown : R/W; bitpos: [3]; default: 0;
         *  phy dppulldown
         */
        uint32_t usb_otghs_phy_dppulldown:1;
        /** usb_otghs_phy_idpullup : R/W; bitpos: [4]; default: 0;
         *  phy idpullup
         */
        uint32_t usb_otghs_phy_idpullup:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_alive_sys_usb_ctrl_reg_t;

/** Type of hp_tcm_mem_lp_ctrl register
 *  configure rmemory power in lp system register
 */
typedef union {
    struct {
        /** hp_tcm_l0_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures layer0  hp_sram memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t hp_tcm_l0_mem_lp_mode:2;
        /** hp_tcm_l0_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down layer0 hp_sram memory.
         */
        uint32_t hp_tcm_l0_mem_lp_en:1;
        /** hp_tcm_mem_l0_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control layer0 hp_sram memory, disable hardware
         *  control.
         */
        uint32_t hp_tcm_mem_l0_lp_force_ctrl:1;
        /** hp_tcm_l1_mem_lp_mode : R/W; bitpos: [5:4]; default: 0;
         *  Configures layer1  hp_sram memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t hp_tcm_l1_mem_lp_mode:2;
        /** hp_tcm_l1_mem_lp_en : R/W; bitpos: [6]; default: 0;
         *  Set this bit to power down layer1 hp_sram memory.
         */
        uint32_t hp_tcm_l1_mem_lp_en:1;
        /** hp_tcm_mem_l1_lp_force_ctrl : R/W; bitpos: [7]; default: 0;
         *  Set this bit to force software control layer1 hp_sram memory, disable hardware
         *  control.
         */
        uint32_t hp_tcm_mem_l1_lp_force_ctrl:1;
        /** hp_tcm_l2_mem_lp_mode : R/W; bitpos: [9:8]; default: 0;
         *  Configures layer2  hp_sram memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t hp_tcm_l2_mem_lp_mode:2;
        /** hp_tcm_l2_mem_lp_en : R/W; bitpos: [10]; default: 0;
         *  Set this bit to power down layer2 hp_sram memory.
         */
        uint32_t hp_tcm_l2_mem_lp_en:1;
        /** hp_tcm_mem_l2_lp_force_ctrl : R/W; bitpos: [11]; default: 0;
         *  Set this bit to force software control layer2 hp_sram memory, disable hardware
         *  control.
         */
        uint32_t hp_tcm_mem_l2_lp_force_ctrl:1;
        /** hp_tcm_l3_mem_lp_mode : R/W; bitpos: [13:12]; default: 0;
         *  Configures layer3  hp_sram memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t hp_tcm_l3_mem_lp_mode:2;
        /** hp_tcm_l3_mem_lp_en : R/W; bitpos: [14]; default: 0;
         *  Set this bit to power down layer3 hp_sram memory.
         */
        uint32_t hp_tcm_l3_mem_lp_en:1;
        /** hp_tcm_mem_l3_lp_force_ctrl : R/W; bitpos: [15]; default: 0;
         *  Set this bit to force software control layer3 hp_sram memory, disable hardware
         *  control.
         */
        uint32_t hp_tcm_mem_l3_lp_force_ctrl:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} hp_alive_sys_hp_tcm_mem_lp_ctrl_reg_t;

/** Type of flash_mmu_mem_lp_ctrl register
 *  configure rmemory power in lp system register
 */
typedef union {
    struct {
        /** flash_mmu_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures flash_mmu memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t flash_mmu_mem_lp_mode:2;
        /** flash_mmu_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down flash_mmu memory.
         */
        uint32_t flash_mmu_mem_lp_en:1;
        /** flash_mmu_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control flash_mmu memory, disable hardware control.
         */
        uint32_t flash_mmu_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_alive_sys_flash_mmu_mem_lp_ctrl_reg_t;

/** Type of psram_mmu_mem_lp_ctrl register
 *  configure rmemory power in lp system register
 */
typedef union {
    struct {
        /** psram_mmu_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures psram_mmu memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t psram_mmu_mem_lp_mode:2;
        /** psram_mmu_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down psram_mmu memory.
         */
        uint32_t psram_mmu_mem_lp_en:1;
        /** psram_mmu_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control psram_mmu memory, disable hardware control.
         */
        uint32_t psram_mmu_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_alive_sys_psram_mmu_mem_lp_ctrl_reg_t;

/** Type of spram_mem_aux_ctrl register
 *  spram mem aux ctrl , control TOP/CNNT/MODEM power domain
 */
typedef union {
    struct {
        /** spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  spram mem aux ctrl , control TOP/CNNT/MODEM power domain
         */
        uint32_t spram_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_alive_sys_spram_mem_aux_ctrl_reg_t;

/** Type of sprf_mem_aux_ctrl register
 *  sprf mem aux ctrl , control TOP/CNNT power domain
 */
typedef union {
    struct {
        /** sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  sprf mem aux ctrl , control TOP/CNNT/MODEM power domain
         */
        uint32_t sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_alive_sys_sprf_mem_aux_ctrl_reg_t;

/** Type of sdprf_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sdprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 0;
         *  sdprf mem aux ctrl , control TOP/CNNT/MODEM power domain
         */
        uint32_t sdprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_alive_sys_sdprf_mem_aux_ctrl_reg_t;

/** Type of cpu_sprom_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** cpu_sprom_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 112;
         *  sprom mem aux ctrl , control CPU power domain
         */
        uint32_t cpu_sprom_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_alive_sys_cpu_sprom_mem_aux_ctrl_reg_t;

/** Type of cpu_sprf_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** cpu_sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t cpu_sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_alive_sys_cpu_sprf_mem_aux_ctrl_reg_t;

/** Type of cpu_spram_mem_aux_ctrl register
 *  spram mem aux ctrl , control CPU power domain
 */
typedef union {
    struct {
        /** cpu_spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  spram mem aux ctrl , control CPU power domain
         */
        uint32_t cpu_spram_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_alive_sys_cpu_spram_mem_aux_ctrl_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 38834448;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} hp_alive_sys_date_reg_t;


/** Group: Configuration Register */
/** Type of intr_hp2lp_conf_0 register
 *  intr hp2lp configuration register 0
 */
typedef union {
    struct {
        /** intr_hp2lp_en_0 : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t intr_hp2lp_en_0:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_conf_0_reg_t;

/** Type of intr_hp2lp_conf_1 register
 *  intr hp2lp configuration register 1
 */
typedef union {
    struct {
        /** intr_hp2lp_en_1 : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t intr_hp2lp_en_1:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_conf_1_reg_t;

/** Type of intr_hp2lp_conf_2 register
 *  intr hp2lp configuration register 2
 */
typedef union {
    struct {
        /** intr_hp2lp_en_2 : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t intr_hp2lp_en_2:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_conf_2_reg_t;

/** Type of intr_hp2lp_conf_3 register
 *  intr hp2lp configuration register 3
 */
typedef union {
    struct {
        /** intr_hp2lp_en_3 : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t intr_hp2lp_en_3:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_conf_3_reg_t;

/** Type of intr_hp2lp_conf_4 register
 *  intr hp2lp configuration register 4
 */
typedef union {
    struct {
        /** intr_hp2lp_en_4 : R/W; bitpos: [7:0]; default: 255;
         *  reserved
         */
        uint32_t intr_hp2lp_en_4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_conf_4_reg_t;

/** Type of bus_clock_gate_bypass register
 *  bus clock gating bypass configuration register
 */
typedef union {
    struct {
        /** ahb_clk_gating_bypass : R/W; bitpos: [0]; default: 1;
         *  Set 1 to bypass the clock gating for ahb bus
         */
        uint32_t ahb_clk_gating_bypass:1;
        /** apb_clk_gating_bypass : R/W; bitpos: [1]; default: 1;
         *  Set 1 to bypass the clock gating for apb bus
         */
        uint32_t apb_clk_gating_bypass:1;
        /** axi_clk_gating_bypass : R/W; bitpos: [2]; default: 1;
         *  Set 1 to bypass the clock gating for axi bus
         */
        uint32_t axi_clk_gating_bypass:1;
        /** mem_clk_gating_bypass : R/W; bitpos: [3]; default: 1;
         *  Set 1 to bypass the clock gating for mem bus
         */
        uint32_t mem_clk_gating_bypass:1;
        /** ahb_mtx_clk_gating_bypass : R/W; bitpos: [4]; default: 1;
         *  Set 1 to bypass the clock gating for ahb mtx
         */
        uint32_t ahb_mtx_clk_gating_bypass:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_alive_sys_bus_clock_gate_bypass_reg_t;

/** Type of rst_event_bypass register
 *  bus clock gating bypass configuration register
 */
typedef union {
    struct {
        /** rst_event_bypass_icmapb : R/W; bitpos: [0]; default: 0;
         *  Set 1 to bypass none-power_on reset source
         */
        uint32_t rst_event_bypass_icmapb:1;
        /** rst_event_bypass_icmsys : R/W; bitpos: [1]; default: 0;
         *  Set 1 to bypass none-power_on reset source
         */
        uint32_t rst_event_bypass_icmsys:1;
        /** rst_event_bypass_icmcpu : R/W; bitpos: [2]; default: 0;
         *  Set 1 to bypass none-power_on reset source
         */
        uint32_t rst_event_bypass_icmcpu:1;
        /** rst_event_bypass_clkrst : R/W; bitpos: [3]; default: 1;
         *  Set 1 to bypass none-power_on reset source
         */
        uint32_t rst_event_bypass_clkrst:1;
        /** rst_event_bypass_clkrst_reg : R/W; bitpos: [4]; default: 0;
         *  Set 1 to bypass none-power_on reset source
         */
        uint32_t rst_event_bypass_clkrst_reg:1;
        /** rst_event_bypass_iomux : R/W; bitpos: [5]; default: 0;
         *  Set 1 to bypass none-power_on reset source
         */
        uint32_t rst_event_bypass_iomux:1;
        /** rst_bypass_lock : R/W; bitpos: [6]; default: 0;
         *  Set 1 to lock reset event bypass configuration
         */
        uint32_t rst_bypass_lock:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_alive_sys_rst_event_bypass_reg_t;

/** Type of usb_otghs_ctrl register
 *  usb_otghs Control configuration register
 */
typedef union {
    struct {
        /** reg_usb_otghs_phy_pll_force_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to force use reg_usb_otghs_phy_pll_en
         */
        uint32_t reg_usb_otghs_phy_pll_force_en:1;
        /** reg_usb_otghs_phy_pll_en : R/W; bitpos: [1]; default: 0;
         *  pll_en for phy inf
         */
        uint32_t reg_usb_otghs_phy_pll_en:1;
        /** reg_usb_otghs_phy_suspendm_force_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit as 1 to force use reg_usb_otghs_phy_suspendm
         */
        uint32_t reg_usb_otghs_phy_suspendm_force_en:1;
        /** reg_usb_otghs_phy_suspendm : R/W; bitpos: [3]; default: 0;
         *  phy suspendm
         */
        uint32_t reg_usb_otghs_phy_suspendm:1;
        /** reg_usb_otghs_phy_reset_force_en : R/W; bitpos: [4]; default: 0;
         *  Set this bit as 1 to force use reg_usb_otghs_phy_reset
         */
        uint32_t reg_usb_otghs_phy_reset_force_en:1;
        /** reg_usb_otghs_phy_reset : R/W; bitpos: [5]; default: 0;
         *  phy reset
         */
        uint32_t reg_usb_otghs_phy_reset:1;
        /** reg_usb_otghs_phy_txbitstuff_en : R/W; bitpos: [6]; default: 0;
         *  phy txbitstuff_en
         */
        uint32_t reg_usb_otghs_phy_txbitstuff_en:1;
        /** reg_usb_otghs_phy_otg_suspendm : R/W; bitpos: [7]; default: 0;
         *  phy otg_suspendm
         */
        uint32_t reg_usb_otghs_phy_otg_suspendm:1;
        /** reg_usb_otghs_phy_refclk_mode : R/W; bitpos: [8]; default: 1;
         *  phy refclk_mode
         */
        uint32_t reg_usb_otghs_phy_refclk_mode:1;
        /** reg_usb_otghs_core_ss_scaledown_mode : R/W; bitpos: [10:9]; default: 0;
         *  usb otghs core ss_scaledown_mode
         */
        uint32_t reg_usb_otghs_core_ss_scaledown_mode:2;
        /** reg_usb_otghs_phy_self_test : R/W; bitpos: [11]; default: 0;
         *  phy self_test
         */
        uint32_t reg_usb_otghs_phy_self_test:1;
        /** reg_usb_otghs_phy_test_done : RO; bitpos: [12]; default: 0;
         *  phy test_bist
         */
        uint32_t reg_usb_otghs_phy_test_done:1;
        /** reg_usb_otghs_phy_dto : RO; bitpos: [13]; default: 0;
         *  phy dto
         */
        uint32_t reg_usb_otghs_phy_dto:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_alive_sys_usb_otghs_ctrl_reg_t;


/** Group: Status Register */
/** Type of intr_hp2lp_status_0 register
 *  intr hp2lp status register 0
 */
typedef union {
    struct {
        /** intr_hp2lp_status_0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t intr_hp2lp_status_0:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_status_0_reg_t;

/** Type of intr_hp2lp_status_1 register
 *  intr hp2lp status register 1
 */
typedef union {
    struct {
        /** intr_hp2lp_status_1 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t intr_hp2lp_status_1:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_status_1_reg_t;

/** Type of intr_hp2lp_status_2 register
 *  intr hp2lp status register 2
 */
typedef union {
    struct {
        /** intr_hp2lp_status_2 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t intr_hp2lp_status_2:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_status_2_reg_t;

/** Type of intr_hp2lp_status_3 register
 *  intr hp2lp status register 3
 */
typedef union {
    struct {
        /** intr_hp2lp_status_3 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t intr_hp2lp_status_3:32;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_status_3_reg_t;

/** Type of intr_hp2lp_status_4 register
 *  intr hp2lp status register 4
 */
typedef union {
    struct {
        /** intr_hp2lp_status_4 : RO; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t intr_hp2lp_status_4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_alive_sys_intr_hp2lp_status_4_reg_t;


typedef struct {
    volatile hp_alive_sys_hp_clk_ctrl_reg_t hp_clk_ctrl;
    volatile hp_alive_sys_hp_pad_parlio_ctrl_reg_t hp_pad_parlio_ctrl;
    volatile hp_alive_sys_hp_pad_i2s0_ctrl_reg_t hp_pad_i2s0_ctrl;
    volatile hp_alive_sys_hp_pad_i2s1_ctrl_reg_t hp_pad_i2s1_ctrl;
    volatile hp_alive_sys_hp_pad_uart0_ctrl_reg_t hp_pad_uart0_ctrl;
    volatile hp_alive_sys_hp_pad_uart1_ctrl_reg_t hp_pad_uart1_ctrl;
    volatile hp_alive_sys_hp_pad_uart2_ctrl_reg_t hp_pad_uart2_ctrl;
    volatile hp_alive_sys_hp_pad_uart3_ctrl_reg_t hp_pad_uart3_ctrl;
    volatile hp_alive_sys_modem_clk_aon_reg_t modem_clk_aon;
    volatile hp_alive_sys_usb_ctrl_reg_t usb_ctrl;
    volatile hp_alive_sys_hp_tcm_mem_lp_ctrl_reg_t hp_tcm_mem_lp_ctrl;
    volatile hp_alive_sys_flash_mmu_mem_lp_ctrl_reg_t flash_mmu_mem_lp_ctrl;
    volatile hp_alive_sys_psram_mmu_mem_lp_ctrl_reg_t psram_mmu_mem_lp_ctrl;
    volatile hp_alive_sys_spram_mem_aux_ctrl_reg_t spram_mem_aux_ctrl;
    volatile hp_alive_sys_sprf_mem_aux_ctrl_reg_t sprf_mem_aux_ctrl;
    uint32_t reserved_03c;
    volatile hp_alive_sys_sdprf_mem_aux_ctrl_reg_t sdprf_mem_aux_ctrl;
    volatile hp_alive_sys_cpu_sprom_mem_aux_ctrl_reg_t cpu_sprom_mem_aux_ctrl;
    volatile hp_alive_sys_cpu_sprf_mem_aux_ctrl_reg_t cpu_sprf_mem_aux_ctrl;
    uint32_t reserved_04c[12];
    volatile hp_alive_sys_intr_hp2lp_conf_0_reg_t intr_hp2lp_conf_0;
    volatile hp_alive_sys_intr_hp2lp_conf_1_reg_t intr_hp2lp_conf_1;
    volatile hp_alive_sys_intr_hp2lp_conf_2_reg_t intr_hp2lp_conf_2;
    volatile hp_alive_sys_intr_hp2lp_conf_3_reg_t intr_hp2lp_conf_3;
    volatile hp_alive_sys_intr_hp2lp_conf_4_reg_t intr_hp2lp_conf_4;
    volatile hp_alive_sys_intr_hp2lp_status_0_reg_t intr_hp2lp_status_0;
    volatile hp_alive_sys_intr_hp2lp_status_1_reg_t intr_hp2lp_status_1;
    volatile hp_alive_sys_intr_hp2lp_status_2_reg_t intr_hp2lp_status_2;
    volatile hp_alive_sys_intr_hp2lp_status_3_reg_t intr_hp2lp_status_3;
    volatile hp_alive_sys_intr_hp2lp_status_4_reg_t intr_hp2lp_status_4;
    volatile hp_alive_sys_cpu_spram_mem_aux_ctrl_reg_t cpu_spram_mem_aux_ctrl;
    volatile hp_alive_sys_bus_clock_gate_bypass_reg_t bus_clock_gate_bypass;
    volatile hp_alive_sys_rst_event_bypass_reg_t rst_event_bypass;
    volatile hp_alive_sys_usb_otghs_ctrl_reg_t usb_otghs_ctrl;
    uint32_t reserved_0b4[210];
    volatile hp_alive_sys_date_reg_t date;
} hp_alive_sys_dev_t;

extern hp_alive_sys_dev_t HP_ALIVE_SYS;

#ifndef __cplusplus
_Static_assert(sizeof(hp_alive_sys_dev_t) == 0x400, "Invalid size of hp_alive_sys_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
