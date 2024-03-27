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

/** Group: configure_register */
/** Type of lp_clk_conf register
 *  need_des
 */
typedef union {
    struct {
        /** slow_clk_sel : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t slow_clk_sel:2;
        /** fast_clk_sel : R/W; bitpos: [3:2]; default: 1;
         *  need_des
         */
        uint32_t fast_clk_sel:2;
        /** lp_peri_div_num : R/W; bitpos: [9:4]; default: 0;
         *  need_des
         */
        uint32_t lp_peri_div_num:6;
        /** ana_sel_ref_pll8m : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t ana_sel_ref_pll8m:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_clkrst_lp_clk_conf_reg_t;

/** Type of lp_clk_po_en register
 *  need_des
 */
typedef union {
    struct {
        /** clk_core_efuse_oen : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t clk_core_efuse_oen:1;
        /** clk_lp_bus_oen : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t clk_lp_bus_oen:1;
        /** clk_aon_slow_oen : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t clk_aon_slow_oen:1;
        /** clk_aon_fast_oen : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t clk_aon_fast_oen:1;
        /** clk_slow_oen : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t clk_slow_oen:1;
        /** clk_fast_oen : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t clk_fast_oen:1;
        /** clk_fosc_oen : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t clk_fosc_oen:1;
        /** clk_rc32k_oen : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t clk_rc32k_oen:1;
        /** clk_sxtal_oen : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t clk_sxtal_oen:1;
        /** clk_sosc_oen : R/W; bitpos: [9]; default: 0;
         *  1'b1: probe sosc clk on
         *  1'b0: probe sosc clk off
         */
        uint32_t clk_sosc_oen:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} lp_clkrst_lp_clk_po_en_reg_t;

/** Type of lp_clk_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** lp_rtc_xtal_force_on : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t lp_rtc_xtal_force_on:1;
        /** ck_en_lp_ram : R/W; bitpos: [27]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_ram:1;
        /** etm_event_tick_en : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t etm_event_tick_en:1;
        /** pll8m_clk_force_on : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t pll8m_clk_force_on:1;
        /** xtal_clk_force_on : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t xtal_clk_force_on:1;
        /** fosc_clk_force_on : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t fosc_clk_force_on:1;
    };
    uint32_t val;
} lp_clkrst_lp_clk_en_reg_t;

/** Type of lp_rst_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** rst_en_lp_huk : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_huk:1;
        /** rst_en_lp_anaperi : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_anaperi:1;
        /** rst_en_lp_wdt : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_wdt:1;
        /** rst_en_lp_timer : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_timer:1;
        /** rst_en_lp_rtc : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_rtc:1;
        /** rst_en_lp_mailbox : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_mailbox:1;
        /** rst_en_lp_aonefusereg : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_aonefusereg:1;
        /** rst_en_lp_ram : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_ram:1;
    };
    uint32_t val;
} lp_clkrst_lp_rst_en_reg_t;

/** Type of reset_cause register
 *  need_des
 */
typedef union {
    struct {
        /** lpcore_reset_cause : RO; bitpos: [5:0]; default: 0;
         *  6'h1: POR reset
         *  6'h9: PMU LP PERI power down reset
         *  6'ha: PMU LP CPU reset
         *  6'hf: brown out reset
         *  6'h10: LP watchdog chip reset
         *  6'h12: super watch dog reset
         *  6'h13: glitch reset
         *  6'h14: software reset
         */
        uint32_t lpcore_reset_cause:6;
        /** lpcore_reset_flag : RO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t lpcore_reset_flag:1;
        /** hpcore0_reset_cause : RO; bitpos: [12:7]; default: 0;
         *  6'h1: POR reset
         *  6'h3: digital system software reset
         *  6'h5: PMU HP system power down reset
         *  6'h7: HP system reset from HP watchdog
         *  6'h9: HP system reset from LP watchdog
         *  6'hb: HP core reset from HP watchdog
         *  6'hc: HP core software reset
         *  6'hd: HP core reset from LP watchdog
         *  6'hf: brown out reset
         *  6'h10: LP watchdog chip reset
         *  6'h12: super watch dog reset
         *  6'h13: glitch reset
         *  6'h14: efuse crc error reset
         *  6'h16: HP usb jtag chip reset
         *  6'h17: HP usb uart chip reset
         *  6'h18: HP jtag reset
         *  6'h1a: HP core lockup
         */
        uint32_t hpcore0_reset_cause:6;
        /** hpcore0_reset_flag : RO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_reset_flag:1;
        /** hpcore1_reset_cause : RO; bitpos: [19:14]; default: 0;
         *  6'h1: POR reset
         *  6'h3: digital system software reset
         *  6'h5: PMU HP system power down reset
         *  6'h7: HP system reset from HP watchdog
         *  6'h9: HP system reset from LP watchdog
         *  6'hb: HP core reset from HP watchdog
         *  6'hc: HP core software reset
         *  6'hd: HP core reset from LP watchdog
         *  6'hf: brown out reset
         *  6'h10: LP watchdog chip reset
         *  6'h12: super watch dog reset
         *  6'h13: glitch reset
         *  6'h14: efuse crc error reset
         *  6'h16: HP usb jtag chip reset
         *  6'h17: HP usb uart chip reset
         *  6'h18: HP jtag reset
         *  6'h1a: HP core lockup
         */
        uint32_t hpcore1_reset_cause:6;
        /** hpcore1_reset_flag : RO; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_reset_flag:1;
        uint32_t reserved_21:4;
        /** lpcore_reset_cause_pmu_lp_cpu_mask : R/W; bitpos: [25]; default: 1;
         *  1'b0: enable lpcore pmu_lp_cpu_reset reset_cause, 1'b1: disable lpcore
         *  pmu_lp_cpu_reset reset_cause
         */
        uint32_t lpcore_reset_cause_pmu_lp_cpu_mask:1;
        /** lpcore_reset_cause_clr : WT; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t lpcore_reset_cause_clr:1;
        /** lpcore_reset_flag_clr : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t lpcore_reset_flag_clr:1;
        /** hpcore0_reset_cause_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_reset_cause_clr:1;
        /** hpcore0_reset_flag_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_reset_flag_clr:1;
        /** hpcore1_reset_cause_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_reset_cause_clr:1;
        /** hpcore1_reset_flag_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_reset_flag_clr:1;
    };
    uint32_t val;
} lp_clkrst_reset_cause_reg_t;

/** Type of hpcpu_reset_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** hpcore0_lockup_reset_en : R/W; bitpos: [0]; default: 0;
         *  write 1 to enable hpcore0 lockup reset feature, write 0 to disable hpcore0 lockup
         *  reset feature
         */
        uint32_t hpcore0_lockup_reset_en:1;
        /** lp_wdt_hpcore0_reset_length : R/W; bitpos: [3:1]; default: 1;
         *  need_des
         */
        uint32_t lp_wdt_hpcore0_reset_length:3;
        /** lp_wdt_hpcore0_reset_en : R/W; bitpos: [4]; default: 0;
         *  write 1 to enable lp_wdt reset hpcore0 feature, write 0 to disable lp_wdt reset
         *  hpcore0 feature
         */
        uint32_t lp_wdt_hpcore0_reset_en:1;
        /** hpcore0_stall_wait : R/W; bitpos: [11:5]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_stall_wait:7;
        /** hpcore0_stall_en : R/W; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_stall_en:1;
        /** hpcore0_sw_reset : WT; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_sw_reset:1;
        /** hpcore0_ocd_halt_on_reset : R/W; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_ocd_halt_on_reset:1;
        /** hpcore0_stat_vector_sel : R/W; bitpos: [15]; default: 1;
         *  1'b1: boot from HP TCM ROM: 0x4FC00000
         *  1'b0: boot from LP TCM RAM:  0x50108000
         */
        uint32_t hpcore0_stat_vector_sel:1;
        /** hpcore1_lockup_reset_en : R/W; bitpos: [16]; default: 0;
         *  write 1 to enable hpcore1 lockup reset feature, write 0 to disable hpcore1 lockup
         *  reset feature
         */
        uint32_t hpcore1_lockup_reset_en:1;
        /** lp_wdt_hpcore1_reset_length : R/W; bitpos: [19:17]; default: 1;
         *  need_des
         */
        uint32_t lp_wdt_hpcore1_reset_length:3;
        /** lp_wdt_hpcore1_reset_en : R/W; bitpos: [20]; default: 0;
         *  write 1 to enable lp_wdt reset hpcore1 feature, write 0 to disable lp_wdt reset
         *  hpcore1 feature
         */
        uint32_t lp_wdt_hpcore1_reset_en:1;
        /** hpcore1_stall_wait : R/W; bitpos: [27:21]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_stall_wait:7;
        /** hpcore1_stall_en : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_stall_en:1;
        /** hpcore1_sw_reset : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_sw_reset:1;
        /** hpcore1_ocd_halt_on_reset : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_ocd_halt_on_reset:1;
        /** hpcore1_stat_vector_sel : R/W; bitpos: [31]; default: 1;
         *  1'b1: boot from HP TCM ROM: 0x4FC00000
         *  1'b0: boot from LP TCM RAM:  0x50108000
         */
        uint32_t hpcore1_stat_vector_sel:1;
    };
    uint32_t val;
} lp_clkrst_hpcpu_reset_ctrl0_reg_t;

/** Type of hpcpu_reset_ctrl1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** hpcore0_sw_stall_code : R/W; bitpos: [23:16]; default: 0;
         *  HP core0 software stall when set to 8'h86
         */
        uint32_t hpcore0_sw_stall_code:8;
        /** hpcore1_sw_stall_code : R/W; bitpos: [31:24]; default: 0;
         *  HP core1 software stall when set to 8'h86
         */
        uint32_t hpcore1_sw_stall_code:8;
    };
    uint32_t val;
} lp_clkrst_hpcpu_reset_ctrl1_reg_t;

/** Type of fosc_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** fosc_dfreq : R/W; bitpos: [31:22]; default: 400;
         *  need_des
         */
        uint32_t fosc_dfreq:10;
    };
    uint32_t val;
} lp_clkrst_fosc_cntl_reg_t;

/** Type of rc32k_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** rc32k_dfreq : R/W; bitpos: [31:0]; default: 650;
         *  need_des
         */
        uint32_t rc32k_dfreq:32;
    };
    uint32_t val;
} lp_clkrst_rc32k_cntl_reg_t;

/** Type of sosc_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** sosc_dfreq : R/W; bitpos: [31:22]; default: 172;
         *  need_des
         */
        uint32_t sosc_dfreq:10;
    };
    uint32_t val;
} lp_clkrst_sosc_cntl_reg_t;

/** Type of clk_to_hp register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** icg_hp_xtal32k : R/W; bitpos: [28]; default: 1;
         *  reserved
         */
        uint32_t icg_hp_xtal32k:1;
        /** icg_hp_sosc : R/W; bitpos: [29]; default: 1;
         *  reserved
         */
        uint32_t icg_hp_sosc:1;
        /** icg_hp_osc32k : R/W; bitpos: [30]; default: 1;
         *  reserved
         */
        uint32_t icg_hp_osc32k:1;
        /** icg_hp_fosc : R/W; bitpos: [31]; default: 1;
         *  reserved
         */
        uint32_t icg_hp_fosc:1;
    };
    uint32_t val;
} lp_clkrst_clk_to_hp_reg_t;

/** Type of lpmem_force register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lpmem_clk_force_on : R/W; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t lpmem_clk_force_on:1;
    };
    uint32_t val;
} lp_clkrst_lpmem_force_reg_t;

/** Type of xtal32k register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** dres_xtal32k : R/W; bitpos: [24:22]; default: 3;
         *  need_des
         */
        uint32_t dres_xtal32k:3;
        /** dgm_xtal32k : R/W; bitpos: [27:25]; default: 3;
         *  need_des
         */
        uint32_t dgm_xtal32k:3;
        /** dbuf_xtal32k : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t dbuf_xtal32k:1;
        /** dac_xtal32k : R/W; bitpos: [31:29]; default: 3;
         *  need_des
         */
        uint32_t dac_xtal32k:3;
    };
    uint32_t val;
} lp_clkrst_xtal32k_reg_t;

/** Type of mux_hpsys_reset_bypass register
 *  need_des
 */
typedef union {
    struct {
        /** mux_hpsys_reset_bypass : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t mux_hpsys_reset_bypass:32;
    };
    uint32_t val;
} lp_clkrst_mux_hpsys_reset_bypass_reg_t;

/** Type of hpsys_0_reset_bypass register
 *  need_des
 */
typedef union {
    struct {
        /** hpsys_0_reset_bypass : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t hpsys_0_reset_bypass:32;
    };
    uint32_t val;
} lp_clkrst_hpsys_0_reset_bypass_reg_t;

/** Type of hpsys_apm_reset_bypass register
 *  need_des
 */
typedef union {
    struct {
        /** hpsys_apm_reset_bypass : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t hpsys_apm_reset_bypass:32;
    };
    uint32_t val;
} lp_clkrst_hpsys_apm_reset_bypass_reg_t;

/** Type of hp_clk_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        /** hp_root_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  HP SoC Root Clock Source Select. 2'd0: xtal_40m, 2'd1: cpll_400m, 2'd2: fosc_20m.
         */
        uint32_t hp_root_clk_src_sel:2;
        /** hp_root_clk_en : R/W; bitpos: [2]; default: 1;
         *  HP SoC Root Clock Enable.
         */
        uint32_t hp_root_clk_en:1;
        /** hp_pad_parlio_tx_clk_en : R/W; bitpos: [3]; default: 1;
         *  PARLIO TX Clock From Pad Enable.
         */
        uint32_t hp_pad_parlio_tx_clk_en:1;
        /** hp_pad_parlio_rx_clk_en : R/W; bitpos: [4]; default: 1;
         *  PARLIO RX Clock From Pad Enable.
         */
        uint32_t hp_pad_parlio_rx_clk_en:1;
        /** hp_pad_uart4_slp_clk_en : R/W; bitpos: [5]; default: 1;
         *  UART4 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart4_slp_clk_en:1;
        /** hp_pad_uart3_slp_clk_en : R/W; bitpos: [6]; default: 1;
         *  UART3 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart3_slp_clk_en:1;
        /** hp_pad_uart2_slp_clk_en : R/W; bitpos: [7]; default: 1;
         *  UART2 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart2_slp_clk_en:1;
        /** hp_pad_uart1_slp_clk_en : R/W; bitpos: [8]; default: 1;
         *  UART1 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart1_slp_clk_en:1;
        /** hp_pad_uart0_slp_clk_en : R/W; bitpos: [9]; default: 1;
         *  UART0 SLP Clock From Pad Enable.
         */
        uint32_t hp_pad_uart0_slp_clk_en:1;
        /** hp_pad_i2s2_mclk_en : R/W; bitpos: [10]; default: 1;
         *  I2S2 MCLK Clock From Pad Enable.
         */
        uint32_t hp_pad_i2s2_mclk_en:1;
        /** hp_pad_i2s1_mclk_en : R/W; bitpos: [11]; default: 1;
         *  I2S1 MCLK Clock From Pad Enable.
         */
        uint32_t hp_pad_i2s1_mclk_en:1;
        /** hp_pad_i2s0_mclk_en : R/W; bitpos: [12]; default: 1;
         *  I2S0 MCLK Clock From Pad Enable.
         */
        uint32_t hp_pad_i2s0_mclk_en:1;
        /** hp_pad_emac_tx_clk_en : R/W; bitpos: [13]; default: 1;
         *  EMAC RX Clock From Pad Enable.
         */
        uint32_t hp_pad_emac_tx_clk_en:1;
        /** hp_pad_emac_rx_clk_en : R/W; bitpos: [14]; default: 1;
         *  EMAC TX Clock From Pad Enable.
         */
        uint32_t hp_pad_emac_rx_clk_en:1;
        /** hp_pad_emac_txrx_clk_en : R/W; bitpos: [15]; default: 1;
         *  EMAC TXRX Clock From Pad Enable.
         */
        uint32_t hp_pad_emac_txrx_clk_en:1;
        /** hp_xtal_32k_clk_en : R/W; bitpos: [16]; default: 1;
         *  XTAL 32K Clock Enable.
         */
        uint32_t hp_xtal_32k_clk_en:1;
        /** hp_rc_32k_clk_en : R/W; bitpos: [17]; default: 1;
         *  RC 32K Clock Enable.
         */
        uint32_t hp_rc_32k_clk_en:1;
        /** hp_sosc_150k_clk_en : R/W; bitpos: [18]; default: 1;
         *  SOSC 150K Clock Enable.
         */
        uint32_t hp_sosc_150k_clk_en:1;
        /** hp_pll_8m_clk_en : R/W; bitpos: [19]; default: 1;
         *  PLL 8M Clock Enable.
         */
        uint32_t hp_pll_8m_clk_en:1;
        /** hp_audio_pll_clk_en : R/W; bitpos: [20]; default: 1;
         *  AUDIO PLL Clock Enable.
         */
        uint32_t hp_audio_pll_clk_en:1;
        /** hp_sdio_pll2_clk_en : R/W; bitpos: [21]; default: 1;
         *  SDIO PLL2 Clock Enable.
         */
        uint32_t hp_sdio_pll2_clk_en:1;
        /** hp_sdio_pll1_clk_en : R/W; bitpos: [22]; default: 1;
         *  SDIO PLL1 Clock Enable.
         */
        uint32_t hp_sdio_pll1_clk_en:1;
        /** hp_sdio_pll0_clk_en : R/W; bitpos: [23]; default: 1;
         *  SDIO PLL0 Clock Enable.
         */
        uint32_t hp_sdio_pll0_clk_en:1;
        /** hp_fosc_20m_clk_en : R/W; bitpos: [24]; default: 1;
         *  FOSC 20M Clock Enable.
         */
        uint32_t hp_fosc_20m_clk_en:1;
        /** hp_xtal_40m_clk_en : R/W; bitpos: [25]; default: 1;
         *  XTAL 40M Clock Enalbe.
         */
        uint32_t hp_xtal_40m_clk_en:1;
        /** hp_cpll_400m_clk_en : R/W; bitpos: [26]; default: 1;
         *  CPLL 400M Clock Enable.
         */
        uint32_t hp_cpll_400m_clk_en:1;
        /** hp_spll_480m_clk_en : R/W; bitpos: [27]; default: 1;
         *  SPLL 480M Clock Enable.
         */
        uint32_t hp_spll_480m_clk_en:1;
        /** hp_mpll_500m_clk_en : R/W; bitpos: [28]; default: 1;
         *  MPLL 500M Clock Enable.
         */
        uint32_t hp_mpll_500m_clk_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} lp_clkrst_hp_clk_ctrl_reg_t;

/** Type of hp_usb_clkrst_ctrl0 register
 *  HP USB Clock Reset Control Register.
 */
typedef union {
    struct {
        /** usb_otg20_sleep_mode : R/W; bitpos: [0]; default: 0;
         *  unused.
         */
        uint32_t usb_otg20_sleep_mode:1;
        /** usb_otg20_bk_sys_clk_en : R/W; bitpos: [1]; default: 1;
         *  unused.
         */
        uint32_t usb_otg20_bk_sys_clk_en:1;
        /** usb_otg11_sleep_mode : R/W; bitpos: [2]; default: 0;
         *  unused.
         */
        uint32_t usb_otg11_sleep_mode:1;
        /** usb_otg11_bk_sys_clk_en : R/W; bitpos: [3]; default: 1;
         *  unused.
         */
        uint32_t usb_otg11_bk_sys_clk_en:1;
        /** usb_otg11_48m_clk_en : R/W; bitpos: [4]; default: 1;
         *  usb otg11 fs phy clock enable.
         */
        uint32_t usb_otg11_48m_clk_en:1;
        /** usb_device_48m_clk_en : R/W; bitpos: [5]; default: 1;
         *  usb device fs phy clock enable.
         */
        uint32_t usb_device_48m_clk_en:1;
        /** usb_48m_div_num : R/W; bitpos: [13:6]; default: 9;
         *  usb 480m to 25m divide number.
         */
        uint32_t usb_48m_div_num:8;
        /** usb_25m_div_num : R/W; bitpos: [21:14]; default: 19;
         *  usb 500m to 25m divide number.
         */
        uint32_t usb_25m_div_num:8;
        /** usb_12m_div_num : R/W; bitpos: [29:22]; default: 39;
         *  usb 480m to 12m divide number.
         */
        uint32_t usb_12m_div_num:8;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} lp_clkrst_hp_usb_clkrst_ctrl0_reg_t;

/** Type of hp_usb_clkrst_ctrl1 register
 *  HP USB Clock Reset Control Register.
 */
typedef union {
    struct {
        /** rst_en_usb_otg20_adp : R/W; bitpos: [0]; default: 0;
         *  usb otg20 adp reset en
         */
        uint32_t rst_en_usb_otg20_adp:1;
        /** rst_en_usb_otg20_phy : R/W; bitpos: [1]; default: 0;
         *  usb otg20 phy reset en
         */
        uint32_t rst_en_usb_otg20_phy:1;
        /** rst_en_usb_otg20 : R/W; bitpos: [2]; default: 0;
         *  usb otg20 reset en
         */
        uint32_t rst_en_usb_otg20:1;
        /** rst_en_usb_otg11 : R/W; bitpos: [3]; default: 0;
         *  usb org11 reset en
         */
        uint32_t rst_en_usb_otg11:1;
        /** rst_en_usb_device : R/W; bitpos: [4]; default: 0;
         *  usb device reset en
         */
        uint32_t rst_en_usb_device:1;
        uint32_t reserved_5:23;
        /** usb_otg20_phyref_clk_src_sel : R/W; bitpos: [29:28]; default: 0;
         *  usb otg20 hs phy src sel. 2'd0: 12m, 2'd1: 25m, 2'd2: pad_hsphy_refclk.
         */
        uint32_t usb_otg20_phyref_clk_src_sel:2;
        /** usb_otg20_phyref_clk_en : R/W; bitpos: [30]; default: 1;
         *  usb otg20 hs phy refclk enable.
         */
        uint32_t usb_otg20_phyref_clk_en:1;
        /** usb_otg20_ulpi_clk_en : R/W; bitpos: [31]; default: 1;
         *  usb otg20 ulpi clock enable.
         */
        uint32_t usb_otg20_ulpi_clk_en:1;
    };
    uint32_t val;
} lp_clkrst_hp_usb_clkrst_ctrl1_reg_t;

/** Type of hp_sdmmc_emac_rst_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** rst_en_sdmmc : R/W; bitpos: [28]; default: 0;
         *  hp sdmmc reset en
         */
        uint32_t rst_en_sdmmc:1;
        /** force_norst_sdmmc : R/W; bitpos: [29]; default: 0;
         *  hp sdmmc force norst
         */
        uint32_t force_norst_sdmmc:1;
        /** rst_en_emac : R/W; bitpos: [30]; default: 0;
         *  hp emac reset en
         */
        uint32_t rst_en_emac:1;
        /** force_norst_emac : R/W; bitpos: [31]; default: 0;
         *  hp emac force norst
         */
        uint32_t force_norst_emac:1;
    };
    uint32_t val;
} lp_clkrst_hp_sdmmc_emac_rst_ctrl_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_clkrst_date_reg_t;


typedef struct {
    volatile lp_clkrst_lp_clk_conf_reg_t lp_clk_conf;
    volatile lp_clkrst_lp_clk_po_en_reg_t lp_clk_po_en;
    volatile lp_clkrst_lp_clk_en_reg_t lp_clk_en;
    volatile lp_clkrst_lp_rst_en_reg_t lp_rst_en;
    volatile lp_clkrst_reset_cause_reg_t reset_cause;
    volatile lp_clkrst_hpcpu_reset_ctrl0_reg_t hpcpu_reset_ctrl0;
    volatile lp_clkrst_hpcpu_reset_ctrl1_reg_t hpcpu_reset_ctrl1;
    volatile lp_clkrst_fosc_cntl_reg_t fosc_cntl;
    volatile lp_clkrst_rc32k_cntl_reg_t rc32k_cntl;
    volatile lp_clkrst_sosc_cntl_reg_t sosc_cntl;
    volatile lp_clkrst_clk_to_hp_reg_t clk_to_hp;
    volatile lp_clkrst_lpmem_force_reg_t lpmem_force;
    volatile lp_clkrst_xtal32k_reg_t xtal32k;
    volatile lp_clkrst_mux_hpsys_reset_bypass_reg_t mux_hpsys_reset_bypass;
    volatile lp_clkrst_hpsys_0_reset_bypass_reg_t hpsys_0_reset_bypass;
    volatile lp_clkrst_hpsys_apm_reset_bypass_reg_t hpsys_apm_reset_bypass;
    volatile lp_clkrst_hp_clk_ctrl_reg_t hp_clk_ctrl;
    volatile lp_clkrst_hp_usb_clkrst_ctrl0_reg_t hp_usb_clkrst_ctrl0;
    volatile lp_clkrst_hp_usb_clkrst_ctrl1_reg_t hp_usb_clkrst_ctrl1;
    volatile lp_clkrst_hp_sdmmc_emac_rst_ctrl_reg_t hp_sdmmc_emac_rst_ctrl;
    uint32_t reserved_050[235];
    volatile lp_clkrst_date_reg_t date;
} lp_clkrst_dev_t;

extern lp_clkrst_dev_t LP_AON_CLKRST;


#ifndef __cplusplus
_Static_assert(sizeof(lp_clkrst_dev_t) == 0x400, "Invalid size of lp_clkrst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
