/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of root_clk_conf register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** slow_clk_sel : R/W; bitpos: [25:23]; default: 0;
         *  need_des
         */
        uint32_t slow_clk_sel:3;
        /** fast_clk_sel : R/W; bitpos: [27:26]; default: 1;
         *  need_des
         */
        uint32_t fast_clk_sel:2;
        /** pll80m_clk_force_on : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t pll80m_clk_force_on:1;
        /** xtal_clk_force_on : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t xtal_clk_force_on:1;
        /** fosc_clk_force_on : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t fosc_clk_force_on:1;
        /** ana_sel_ref_pll8m : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t ana_sel_ref_pll8m:1;
    };
    uint32_t val;
} lp_aonclkrst_root_clk_conf_reg_t;

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
} lp_aonclkrst_lp_clk_po_en_reg_t;

/** Type of misc register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** etm_event_tick_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t etm_event_tick_en:1;
    };
    uint32_t val;
} lp_aonclkrst_misc_reg_t;

/** Type of timer register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_timer_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_timer_rst_en:1;
    };
    uint32_t val;
} lp_aonclkrst_timer_reg_t;

/** Type of wdt register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_wdt_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_wdt_rst_en:1;
    };
    uint32_t val;
} lp_aonclkrst_wdt_reg_t;

/** Type of clockcali register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** lp_clk_cali_xtal_force_on : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t lp_clk_cali_xtal_force_on:1;
        /** lp_clk_cali_fosc_rst_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_clk_cali_fosc_rst_en:1;
        /** lp_clk_cali_sosc_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_clk_cali_sosc_rst_en:1;
    };
    uint32_t val;
} lp_aonclkrst_clockcali_reg_t;

/** Type of anaperi register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_anaperi_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_anaperi_rst_en:1;
    };
    uint32_t val;
} lp_aonclkrst_anaperi_reg_t;

/** Type of huk register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_huk_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_huk_rst_en:1;
    };
    uint32_t val;
} lp_aonclkrst_huk_reg_t;

/** Type of fosc_dfreq register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** fosc_dfreq : R/W; bitpos: [31:22]; default: 261;
         *  need_des
         */
        uint32_t fosc_dfreq:10;
    };
    uint32_t val;
} lp_aonclkrst_fosc_dfreq_reg_t;

/** Type of rc32k_dfreq register
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
} lp_aonclkrst_rc32k_dfreq_reg_t;

/** Type of sosc_dfreq register
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
} lp_aonclkrst_sosc_dfreq_reg_t;

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
} lp_aonclkrst_xtal32k_reg_t;

/** Type of hpcore0_reset_cause register
 *  need_des
 */
typedef union {
    struct {
        /** hpcore0_reset_flag : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_reset_flag:1;
        /** hpcore0_reset_cause : RO; bitpos: [6:1]; default: 0;
         *  6'h1: POR reset
         *  6'h3: digital system software reset
         *  6'h5: PMU HP system power down reset
         *  6'h7: HP system reset from HP watchdog0
         *  6'h8: HP system reset from HP watchdog1
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
        uint32_t reserved_7:23;
        /** hpcore0_reset_cause_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_reset_cause_clr:1;
        /** hpcore0_reset_flag_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_reset_flag_clr:1;
    };
    uint32_t val;
} lp_aonclkrst_hpcore0_reset_cause_reg_t;

/** Type of hpcore1_reset_cause register
 *  need_des
 */
typedef union {
    struct {
        /** hpcore1_reset_flag : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_reset_flag:1;
        /** hpcore1_reset_cause : RO; bitpos: [6:1]; default: 0;
         *  6'h1: POR reset
         *  6'h3: digital system software reset
         *  6'h5: PMU HP system power down reset
         *  6'h7: HP system reset from HP watchdog0
         *  6'h8: HP system reset from HP watchdog1
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
        uint32_t reserved_7:23;
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
} lp_aonclkrst_hpcore1_reset_cause_reg_t;

/** Type of lpcore_reset_cause register
 *  need_des
 */
typedef union {
    struct {
        /** lpcore_reset_flag : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lpcore_reset_flag:1;
        /** lpcore_reset_cause : RO; bitpos: [6:1]; default: 0;
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
        uint32_t reserved_7:22;
        /** lpcore_reset_cause_pmu_lp_cpu_mask : R/W; bitpos: [29]; default: 1;
         *  1'b0: enable lpcore pmu_lp_cpu_reset reset_cause, 1'b1: disable lpcore
         *  pmu_lp_cpu_reset reset_cause
         */
        uint32_t lpcore_reset_cause_pmu_lp_cpu_mask:1;
        /** lpcore_reset_cause_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lpcore_reset_cause_clr:1;
        /** lpcore_reset_flag_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lpcore_reset_flag_clr:1;
    };
    uint32_t val;
} lp_aonclkrst_lpcore_reset_cause_reg_t;

/** Type of hpcore0_reset_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** hpcore0_sw_stall_code : R/W; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t hpcore0_sw_stall_code:8;
        /** hpcore0_stall_wait : R/W; bitpos: [14:8]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_stall_wait:7;
        /** hpcore0_stall_en : R/W; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_stall_en:1;
        /** hpcore0_reset_length : R/W; bitpos: [18:16]; default: 1;
         *  need_des
         */
        uint32_t hpcore0_reset_length:3;
        /** hpcore0_enable_lp_wdt_reset : R/W; bitpos: [19]; default: 0;
         *  write 1 to enable lp_wdt reset hpcore0 feature, write 0 to disable lp_wdt reset
         *  hpcore0 feature
         */
        uint32_t hpcore0_enable_lp_wdt_reset:1;
        /** hpcore0_sw_reset : WT; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_sw_reset:1;
        uint32_t reserved_21:8;
        /** hpcore0_ocd_halt_on_reset : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hpcore0_ocd_halt_on_reset:1;
        /** hpcore0_stat_vector_sel : R/W; bitpos: [30]; default: 1;
         *  1'b1: boot from HP TCM ROM: 0x4FC00000
         *  1'b0: boot from LP TCM RAM:  0x50108000
         */
        uint32_t hpcore0_stat_vector_sel:1;
        /** hpcore0_lockup_reset_en : R/W; bitpos: [31]; default: 0;
         *  write 1 to enable hpcore0 lockup reset feature, write 0 to disable hpcore0 lockup
         *  reset feature
         */
        uint32_t hpcore0_lockup_reset_en:1;
    };
    uint32_t val;
} lp_aonclkrst_hpcore0_reset_ctrl_reg_t;

/** Type of hpcore1_reset_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** hpcore1_sw_stall_code : R/W; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t hpcore1_sw_stall_code:8;
        /** hpcore1_stall_wait : R/W; bitpos: [14:8]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_stall_wait:7;
        /** hpcore1_stall_en : R/W; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_stall_en:1;
        /** hpcore1_reset_length : R/W; bitpos: [18:16]; default: 1;
         *  need_des
         */
        uint32_t hpcore1_reset_length:3;
        /** hpcore1_enable_lp_wdt_reset : R/W; bitpos: [19]; default: 0;
         *  write 1 to enable lp_wdt reset hpcore1 feature, write 0 to disable lp_wdt reset
         *  hpcore1 feature
         */
        uint32_t hpcore1_enable_lp_wdt_reset:1;
        /** hpcore1_sw_reset : WT; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_sw_reset:1;
        uint32_t reserved_21:8;
        /** hpcore1_ocd_halt_on_reset : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hpcore1_ocd_halt_on_reset:1;
        /** hpcore1_stat_vector_sel : R/W; bitpos: [30]; default: 1;
         *  1'b1: boot from HP TCM ROM: 0x4FC00000
         *  1'b0: boot from LP TCM RAM:  0x50108000
         */
        uint32_t hpcore1_stat_vector_sel:1;
        /** hpcore1_lockup_reset_en : R/W; bitpos: [31]; default: 0;
         *  write 1 to enable hpcore1 lockup reset feature, write 0 to disable hpcore1 lockup
         *  reset feature
         */
        uint32_t hpcore1_lockup_reset_en:1;
    };
    uint32_t val;
} lp_aonclkrst_hpcore1_reset_ctrl_reg_t;

/** Type of touch_aon register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** touch_aon_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t touch_aon_rst_en:1;
    };
    uint32_t val;
} lp_aonclkrst_touch_aon_reg_t;

/** Type of hp_clk_ctrl register
 *  HP Clock Control Register.
 */
typedef union {
    struct {
        /** clk_xtal_freq : RO; bitpos: [6:0]; default: 40;
         *  XTAL Clock Freq Indication. 7'd40: xtal_40m, 7'd48: xtal_48m
         */
        uint32_t clk_xtal_freq:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_aonclkrst_hp_clk_ctrl_reg_t;

/** Type of cpll_div register
 *  need_des
 */
typedef union {
    struct {
        /** cpll_ref_div : R/W; bitpos: [3:0]; default: 2;
         *  cpll ref div value
         */
        uint32_t cpll_ref_div:4;
        /** cpll_fb_div : R/W; bitpos: [11:4]; default: 15;
         *  cpll fb div value
         */
        uint32_t cpll_fb_div:8;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_aonclkrst_cpll_div_reg_t;

/** Type of apll_div register
 *  need_des
 */
typedef union {
    struct {
        /** apll_ref_div : R/W; bitpos: [2:0]; default: 5;
         *  apll ref div value
         */
        uint32_t apll_ref_div:3;
        /** apll_out_div : R/W; bitpos: [7:3]; default: 16;
         *  apll out div value
         */
        uint32_t apll_out_div:5;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_aonclkrst_apll_div_reg_t;

/** Type of mspi_div register
 *  need_des
 */
typedef union {
    struct {
        /** mspi_ref_div : R/W; bitpos: [2:0]; default: 1;
         *  MSPI ref div value
         */
        uint32_t mspi_ref_div:3;
        /** mspi_fb_div : R/W; bitpos: [7:3]; default: 24;
         *  MSPI fb div value
         */
        uint32_t mspi_fb_div:5;
        /** mspi_chgp_dcur : R/W; bitpos: [8]; default: 0;
         *  MSPI chgp_dcur value
         */
        uint32_t mspi_chgp_dcur:1;
        /** mspi_lf_res : R/W; bitpos: [9]; default: 0;
         *  MSPI lf_res value
         */
        uint32_t mspi_lf_res:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} lp_aonclkrst_mspi_div_reg_t;

/** Type of lp_adi register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_adi_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_adi_rst_en:1;
    };
    uint32_t val;
} lp_aonclkrst_lp_adi_reg_t;

/** Type of lproot_clk_div register
 *  need_des
 */
typedef union {
    struct {
        /** lpaon_clk_div_num : R/W; bitpos: [2:0]; default: 0;
         *  lp aon clock divide num
         */
        uint32_t lpaon_clk_div_num:3;
        uint32_t reserved_3:1;
        /** lpperi_clk_div_num : R/W; bitpos: [6:4]; default: 0;
         *  lp peri clock divide num
         */
        uint32_t lpperi_clk_div_num:3;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_aonclkrst_lproot_clk_div_reg_t;

/** Type of cpll_cfg register
 *  need_des
 */
typedef union {
    struct {
        /** cpll_dbias : R/W; bitpos: [2:0]; default: 0;
         *  cpll dbias value
         */
        uint32_t cpll_dbias:3;
        /** cpll_dchgp : R/W; bitpos: [5:3]; default: 0;
         *  cpll dchgp value
         */
        uint32_t cpll_dchgp:3;
        /** cpll_dr1 : R/W; bitpos: [8:6]; default: 0;
         *  cpll dr1 value
         */
        uint32_t cpll_dr1:3;
        /** cpll_dr3 : R/W; bitpos: [11:9]; default: 0;
         *  cpll dr3 value
         */
        uint32_t cpll_dr3:3;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_aonclkrst_cpll_cfg_reg_t;

/** Type of apll_sdm register
 *  need_des
 */
typedef union {
    struct {
        /** apll_sdm : R/W; bitpos: [21:0]; default: 0;
         *  apll sdm value
         */
        uint32_t apll_sdm:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} lp_aonclkrst_apll_sdm_reg_t;

/** Type of rtc_sar2_pwdet_cct register
 *  need_des
 */
typedef union {
    struct {
        /** rtc_sar2_pwdet_cct : R/W; bitpos: [2:0]; default: 0;
         *  rtc_sar2_pwdet_cct value
         */
        uint32_t rtc_sar2_pwdet_cct:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_aonclkrst_rtc_sar2_pwdet_cct_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 38826752;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_aonclkrst_date_reg_t;


typedef struct {
    volatile lp_aonclkrst_root_clk_conf_reg_t root_clk_conf;
    volatile lp_aonclkrst_lp_clk_po_en_reg_t lp_clk_po_en;
    volatile lp_aonclkrst_misc_reg_t misc;
    volatile lp_aonclkrst_timer_reg_t timer;
    volatile lp_aonclkrst_wdt_reg_t wdt;
    volatile lp_aonclkrst_clockcali_reg_t clockcali;
    volatile lp_aonclkrst_anaperi_reg_t anaperi;
    volatile lp_aonclkrst_huk_reg_t huk;
    volatile lp_aonclkrst_fosc_dfreq_reg_t fosc_dfreq;
    volatile lp_aonclkrst_rc32k_dfreq_reg_t rc32k_dfreq;
    volatile lp_aonclkrst_sosc_dfreq_reg_t sosc_dfreq;
    volatile lp_aonclkrst_xtal32k_reg_t xtal32k;
    volatile lp_aonclkrst_hpcore0_reset_cause_reg_t hpcore0_reset_cause;
    volatile lp_aonclkrst_hpcore1_reset_cause_reg_t hpcore1_reset_cause;
    volatile lp_aonclkrst_lpcore_reset_cause_reg_t lpcore_reset_cause;
    volatile lp_aonclkrst_hpcore0_reset_ctrl_reg_t hpcore0_reset_ctrl;
    volatile lp_aonclkrst_hpcore1_reset_ctrl_reg_t hpcore1_reset_ctrl;
    volatile lp_aonclkrst_touch_aon_reg_t touch_aon;
    volatile lp_aonclkrst_hp_clk_ctrl_reg_t hp_clk_ctrl;
    volatile lp_aonclkrst_cpll_div_reg_t cpll_div;
    volatile lp_aonclkrst_apll_div_reg_t apll_div;
    volatile lp_aonclkrst_mspi_div_reg_t mspi_div;
    volatile lp_aonclkrst_lp_adi_reg_t lp_adi;
    volatile lp_aonclkrst_lproot_clk_div_reg_t lproot_clk_div;
    volatile lp_aonclkrst_cpll_cfg_reg_t cpll_cfg;
    volatile lp_aonclkrst_apll_sdm_reg_t apll_sdm;
    volatile lp_aonclkrst_rtc_sar2_pwdet_cct_reg_t rtc_sar2_pwdet_cct;
    uint32_t reserved_06c[228];
    volatile lp_aonclkrst_date_reg_t date;
} lp_aonclkrst_dev_t;

extern lp_aonclkrst_dev_t LP_CLKRST;

#ifndef __cplusplus
_Static_assert(sizeof(lp_aonclkrst_dev_t) == 0x400, "Invalid size of lp_aonclkrst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
