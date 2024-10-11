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
/** Type of lp_sys_ver_date register
 *  need_des
 */
typedef union {
    struct {
        /** ver_date : R/W; bitpos: [31:0]; default: 539165961;
         *  need_des
         */
        uint32_t ver_date:32;
    };
    uint32_t val;
} lp_system_reg_lp_sys_ver_date_reg_t;

/** Type of clk_sel_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** ena_sw_sel_sys_clk : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t ena_sw_sel_sys_clk:1;
        /** sw_sys_clk_src_sel : R/W; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t sw_sys_clk_src_sel:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} lp_system_reg_clk_sel_ctrl_reg_t;

/** Type of sys_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** lp_core_disable : R/W; bitpos: [0]; default: 0;
         *  lp cpu disable
         */
        uint32_t lp_core_disable:1;
        /** sys_sw_rst : WT; bitpos: [1]; default: 0;
         *  digital system software reset bit
         */
        uint32_t sys_sw_rst:1;
        /** force_download_boot : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t force_download_boot:1;
        /** dig_fib : R/W; bitpos: [10:3]; default: 255;
         *  need_des
         */
        uint32_t dig_fib:8;
        /** io_mux_reset_disable : R/W; bitpos: [11]; default: 0;
         *  reset disable bit for LP IOMUX
         */
        uint32_t io_mux_reset_disable:1;
        uint32_t reserved_12:2;
        /** ana_fib : RO; bitpos: [20:14]; default: 127;
         *  need_des
         */
        uint32_t ana_fib:7;
        /** lp_fib_sel : R/W; bitpos: [28:21]; default: 255;
         *  need_des
         */
        uint32_t lp_fib_sel:8;
        /** lp_core_etm_wakeup_flag_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t lp_core_etm_wakeup_flag_clr:1;
        /** lp_core_etm_wakeup_flag : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_core_etm_wakeup_flag:1;
        /** systimer_stall_sel : R/W; bitpos: [31]; default: 0;
         *  0: use systimer_stall signal from hp_core0, 1: use systimer_stall signal from
         *  hp_core1
         */
        uint32_t systimer_stall_sel:1;
    };
    uint32_t val;
} lp_system_reg_sys_ctrl_reg_t;

/** Type of lp_clk_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:13;
        /** lp_fosc_hp_cken : R/W; bitpos: [14]; default: 1;
         *  reserved
         */
        uint32_t lp_fosc_hp_cken:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_system_reg_lp_clk_ctrl_reg_t;

/** Type of lp_rst_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** ana_rst_bypass : R/W; bitpos: [0]; default: 1;
         *  analog source reset bypass : wdt,brown out,super wdt,glitch
         */
        uint32_t ana_rst_bypass:1;
        /** sys_rst_bypass : R/W; bitpos: [1]; default: 1;
         *  system source reset bypass : software reset,hp wdt,lp wdt,efuse
         */
        uint32_t sys_rst_bypass:1;
        /** efuse_force_norst : R/W; bitpos: [2]; default: 0;
         *  efuse force no reset control
         */
        uint32_t efuse_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_system_reg_lp_rst_ctrl_reg_t;

/** Type of lp_core_boot_addr register
 *  need_des
 */
typedef union {
    struct {
        /** lp_cpu_boot_addr : R/W; bitpos: [31:0]; default: 1343225856;
         *  need_des
         */
        uint32_t lp_cpu_boot_addr:32;
    };
    uint32_t val;
} lp_system_reg_lp_core_boot_addr_reg_t;

/** Type of ext_wakeup1 register
 *  need_des
 */
typedef union {
    struct {
        /** ext_wakeup1_sel : R/W; bitpos: [15:0]; default: 0;
         *  Bitmap to select RTC pads for ext wakeup1
         */
        uint32_t ext_wakeup1_sel:16;
        /** ext_wakeup1_status_clr : WT; bitpos: [16]; default: 0;
         *  clear ext wakeup1 status
         */
        uint32_t ext_wakeup1_status_clr:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_system_reg_ext_wakeup1_reg_t;

/** Type of ext_wakeup1_status register
 *  need_des
 */
typedef union {
    struct {
        /** ext_wakeup1_status : RO; bitpos: [15:0]; default: 0;
         *  ext wakeup1 status
         */
        uint32_t ext_wakeup1_status:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_system_reg_ext_wakeup1_status_reg_t;

/** Type of lp_tcm_pwr_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** lp_tcm_rom_clk_force_on : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_rom_clk_force_on:1;
        uint32_t reserved_6:1;
        /** lp_tcm_ram_clk_force_on : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_ram_clk_force_on:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_system_reg_lp_tcm_pwr_ctrl_reg_t;

/** Type of boot_addr_hp_lp_reg register
 *  need_des
 */
typedef union {
    struct {
        /** boot_addr_hp_lp : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t boot_addr_hp_lp:32;
    };
    uint32_t val;
} lp_system_reg_boot_addr_hp_lp_reg_reg_t;

/** Type of lp_store0 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch0 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch0:32;
    };
    uint32_t val;
} lp_system_reg_lp_store0_reg_t;

/** Type of lp_store1 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch1 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch1:32;
    };
    uint32_t val;
} lp_system_reg_lp_store1_reg_t;

/** Type of lp_store2 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch2 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch2:32;
    };
    uint32_t val;
} lp_system_reg_lp_store2_reg_t;

/** Type of lp_store3 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch3 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch3:32;
    };
    uint32_t val;
} lp_system_reg_lp_store3_reg_t;

/** Type of lp_store4 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch4 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch4:32;
    };
    uint32_t val;
} lp_system_reg_lp_store4_reg_t;

/** Type of lp_store5 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch5 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch5:32;
    };
    uint32_t val;
} lp_system_reg_lp_store5_reg_t;

/** Type of lp_store6 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch6 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch6:32;
    };
    uint32_t val;
} lp_system_reg_lp_store6_reg_t;

/** Type of lp_store7 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch7 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch7:32;
    };
    uint32_t val;
} lp_system_reg_lp_store7_reg_t;

/** Type of lp_store8 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch8 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch8:32;
    };
    uint32_t val;
} lp_system_reg_lp_store8_reg_t;

/** Type of lp_store9 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch9 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch9:32;
    };
    uint32_t val;
} lp_system_reg_lp_store9_reg_t;

/** Type of lp_store10 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch10 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch10:32;
    };
    uint32_t val;
} lp_system_reg_lp_store10_reg_t;

/** Type of lp_store11 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch11 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch11:32;
    };
    uint32_t val;
} lp_system_reg_lp_store11_reg_t;

/** Type of lp_store12 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch12 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch12:32;
    };
    uint32_t val;
} lp_system_reg_lp_store12_reg_t;

/** Type of lp_store13 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch13 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch13:32;
    };
    uint32_t val;
} lp_system_reg_lp_store13_reg_t;

/** Type of lp_store14 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch14 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch14:32;
    };
    uint32_t val;
} lp_system_reg_lp_store14_reg_t;

/** Type of lp_store15 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_scratch15 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_scratch15:32;
    };
    uint32_t val;
} lp_system_reg_lp_store15_reg_t;

/** Type of lp_probea_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** probe_a_mod_sel : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t probe_a_mod_sel:16;
        /** probe_a_top_sel : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t probe_a_top_sel:8;
        /** probe_l_sel : R/W; bitpos: [25:24]; default: 0;
         *  need_des
         */
        uint32_t probe_l_sel:2;
        /** probe_h_sel : R/W; bitpos: [27:26]; default: 0;
         *  need_des
         */
        uint32_t probe_h_sel:2;
        /** probe_global_en : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t probe_global_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} lp_system_reg_lp_probea_ctrl_reg_t;

/** Type of lp_probeb_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** probe_b_mod_sel : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t probe_b_mod_sel:16;
        /** probe_b_top_sel : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t probe_b_top_sel:8;
        /** probe_b_en : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t probe_b_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} lp_system_reg_lp_probeb_ctrl_reg_t;

/** Type of lp_probe_out register
 *  need_des
 */
typedef union {
    struct {
        /** probe_top_out : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t probe_top_out:32;
    };
    uint32_t val;
} lp_system_reg_lp_probe_out_reg_t;

/** Type of f2s_apb_brg_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** f2s_apb_postw_en : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t f2s_apb_postw_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_system_reg_f2s_apb_brg_cntl_reg_t;

/** Type of usb_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sw_hw_usb_phy_sel : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t sw_hw_usb_phy_sel:1;
        /** sw_usb_phy_sel : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sw_usb_phy_sel:1;
        /** usbotg20_wakeup_clr : WT; bitpos: [2]; default: 0;
         *  clear usb wakeup to PMU.
         */
        uint32_t usbotg20_wakeup_clr:1;
        /** usbotg20_in_suspend : R/W; bitpos: [3]; default: 0;
         *  indicate usb otg2.0 is in suspend state.
         */
        uint32_t usbotg20_in_suspend:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_system_reg_usb_ctrl_reg_t;

/** Type of ana_xpd_pad_group register
 *  need_des
 */
typedef union {
    struct {
        /** ana_reg_xpd_pad_group : R/W; bitpos: [7:0]; default: 255;
         *  Set 1 to power up pad group
         */
        uint32_t ana_reg_xpd_pad_group:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_system_reg_ana_xpd_pad_group_reg_t;

/** Type of lp_tcm_ram_rdn_eco_cs register
 *  need_des
 */
typedef union {
    struct {
        /** lp_tcm_ram_rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_ram_rdn_eco_en:1;
        /** lp_tcm_ram_rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_ram_rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_system_reg_lp_tcm_ram_rdn_eco_cs_reg_t;

/** Type of lp_tcm_ram_rdn_eco_low register
 *  need_des
 */
typedef union {
    struct {
        /** lp_tcm_ram_rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_ram_rdn_eco_low:32;
    };
    uint32_t val;
} lp_system_reg_lp_tcm_ram_rdn_eco_low_reg_t;

/** Type of lp_tcm_ram_rdn_eco_high register
 *  need_des
 */
typedef union {
    struct {
        /** lp_tcm_ram_rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t lp_tcm_ram_rdn_eco_high:32;
    };
    uint32_t val;
} lp_system_reg_lp_tcm_ram_rdn_eco_high_reg_t;

/** Type of lp_tcm_rom_rdn_eco_cs register
 *  need_des
 */
typedef union {
    struct {
        /** lp_tcm_rom_rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_rom_rdn_eco_en:1;
        /** lp_tcm_rom_rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_rom_rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_system_reg_lp_tcm_rom_rdn_eco_cs_reg_t;

/** Type of lp_tcm_rom_rdn_eco_low register
 *  need_des
 */
typedef union {
    struct {
        /** lp_tcm_rom_rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_tcm_rom_rdn_eco_low:32;
    };
    uint32_t val;
} lp_system_reg_lp_tcm_rom_rdn_eco_low_reg_t;

/** Type of lp_tcm_rom_rdn_eco_high register
 *  need_des
 */
typedef union {
    struct {
        /** lp_tcm_rom_rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t lp_tcm_rom_rdn_eco_high:32;
    };
    uint32_t val;
} lp_system_reg_lp_tcm_rom_rdn_eco_high_reg_t;

/** Type of hp_root_clk_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** cpu_clk_en : R/W; bitpos: [0]; default: 1;
         *  clock gate enable for hp cpu root 400M clk
         */
        uint32_t cpu_clk_en:1;
        /** sys_clk_en : R/W; bitpos: [1]; default: 1;
         *  clock gate enable for hp sys root 480M clk
         */
        uint32_t sys_clk_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_system_reg_hp_root_clk_ctrl_reg_t;

/** Type of lp_pmu_rdn_eco_low register
 *  need_des
 */
typedef union {
    struct {
        /** pmu_rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t pmu_rdn_eco_low:32;
    };
    uint32_t val;
} lp_system_reg_lp_pmu_rdn_eco_low_reg_t;

/** Type of lp_pmu_rdn_eco_high register
 *  need_des
 */
typedef union {
    struct {
        /** pmu_rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t pmu_rdn_eco_high:32;
    };
    uint32_t val;
} lp_system_reg_lp_pmu_rdn_eco_high_reg_t;

/** Type of pad_comp0 register
 *  need_des
 */
typedef union {
    struct {
        /** dref_comp : R/W; bitpos: [2:0]; default: 0;
         *  pad comp dref
         */
        uint32_t dref_comp:3;
        /** mode_comp : R/W; bitpos: [3]; default: 0;
         *  pad comp mode
         */
        uint32_t mode_comp:1;
        /** xpd_comp : R/W; bitpos: [4]; default: 0;
         *  pad comp xpd
         */
        uint32_t xpd_comp:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lp_system_reg_pad_comp_reg_t;

/** Type of backup_dma_cfg0 register
 *  need_des
 */
typedef union {
    struct {
        /** burst_limit_aon : R/W; bitpos: [4:0]; default: 10;
         *  need_des
         */
        uint32_t burst_limit_aon:5;
        /** read_interval_aon : R/W; bitpos: [11:5]; default: 10;
         *  need_des
         */
        uint32_t read_interval_aon:7;
        /** link_backup_tout_thres_aon : R/W; bitpos: [21:12]; default: 100;
         *  need_des
         */
        uint32_t link_backup_tout_thres_aon:10;
        /** link_tout_thres_aon : R/W; bitpos: [31:22]; default: 100;
         *  need_des
         */
        uint32_t link_tout_thres_aon:10;
    };
    uint32_t val;
} lp_system_reg_backup_dma_cfg0_reg_t;

/** Type of backup_dma_cfg1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** aon_bypass : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_bypass:1;
    };
    uint32_t val;
} lp_system_reg_backup_dma_cfg1_reg_t;

/** Type of backup_dma_cfg2 register
 *  need_des
 */
typedef union {
    struct {
        /** link_addr_aon : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t link_addr_aon:32;
    };
    uint32_t val;
} lp_system_reg_backup_dma_cfg2_reg_t;

/** Type of boot_addr_hp_core1 register
 *  need_des
 */
typedef union {
    struct {
        /** boot_addr_hp_core1 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t boot_addr_hp_core1:32;
    };
    uint32_t val;
} lp_system_reg_boot_addr_hp_core1_reg_t;

/** Type of hp_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** hp_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t hp_mem_aux_ctrl:32;
    };
    uint32_t val;
} lp_system_reg_hp_mem_aux_ctrl_reg_t;

/** Type of lp_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** lp_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t lp_mem_aux_ctrl:32;
    };
    uint32_t val;
} lp_system_reg_lp_mem_aux_ctrl_reg_t;

/** Type of hp_rom_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** hp_rom_aux_ctrl : R/W; bitpos: [31:0]; default: 112;
         *  need_des
         */
        uint32_t hp_rom_aux_ctrl:32;
    };
    uint32_t val;
} lp_system_reg_hp_rom_aux_ctrl_reg_t;

/** Type of lp_rom_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** lp_rom_aux_ctrl : R/W; bitpos: [31:0]; default: 112;
         *  need_des
         */
        uint32_t lp_rom_aux_ctrl:32;
    };
    uint32_t val;
} lp_system_reg_lp_rom_aux_ctrl_reg_t;

/** Type of hp_por_rst_bypass_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** hp_po_cnnt_rstn_bypass_ctrl : R/W; bitpos: [15:8]; default: 255;
         *  [15] 1'b1: po_cnnt_rstn bypass sys_sw_rstn
         *  [14] 1'b1: po_cnnt_rstn bypass hp_wdt_sys_rstn
         *  [13] 1'b1: po_cnnt_rstn bypass hp_cpu_intrusion_rstn
         *  [12] 1'b1: po_cnnt_rstn bypass hp_sdio_sys_rstn
         *  [11] 1'b1: po_cnnt_rstn bypass usb_jtag_chip_rst
         *  [10] 1'b1: po_cnnt_rstn bypass usb_uart_chip_rst
         *  [9] 1'b1: po_cnnt_rstn bypass lp_wdt_hp_sys_rstn
         *  [8] 1'b1: po_cnnt_rstn bypass efuse_err_rstn
         */
        uint32_t hp_po_cnnt_rstn_bypass_ctrl:8;
        uint32_t reserved_16:8;
        /** hp_po_rstn_bypass_ctrl : R/W; bitpos: [31:24]; default: 255;
         *  [31] 1'b1: po_rstn bypass sys_sw_rstn
         *  [30] 1'b1: po_rstn bypass hp_wdt_sys_rstn
         *  [29] 1'b1: po_rstn bypass hp_cpu_intrusion_rstn
         *  [28] 1'b1: po_rstn bypass hp_sdio_sys_rstn
         *  [27] 1'b1: po_rstn bypass usb_jtag_chip_rst
         *  [26] 1'b1: po_rstn bypass usb_uart_chip_rst
         *  [25] 1'b1: po_rstn bypass lp_wdt_hp_sys_rstn
         *  [24] 1'b1: po_rstn bypass efuse_err_rstn
         */
        uint32_t hp_po_rstn_bypass_ctrl:8;
    };
    uint32_t val;
} lp_system_reg_hp_por_rst_bypass_ctrl_reg_t;

/** Type of lp_core_ahb_timeout register
 *  need_des
 */
typedef union {
    struct {
        /** lp_core_ahb_timeout_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable lp core ahb timeout handle
         */
        uint32_t lp_core_ahb_timeout_en:1;
        /** lp_core_ahb_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set lp core ahb bus timeout  threshold
         */
        uint32_t lp_core_ahb_timeout_thres:16;
        /** lp2hp_ahb_timeout_en : R/W; bitpos: [17]; default: 1;
         *  set this field to 1 to enable lp2hp ahb timeout handle
         */
        uint32_t lp2hp_ahb_timeout_en:1;
        /** lp2hp_ahb_timeout_thres : R/W; bitpos: [22:18]; default: 31;
         *  This field used to set lp2hp ahb bus timeout  threshold
         */
        uint32_t lp2hp_ahb_timeout_thres:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lp_system_reg_lp_core_ahb_timeout_reg_t;

/** Type of lp_core_ibus_timeout register
 *  need_des
 */
typedef union {
    struct {
        /** lp_core_ibus_timeout_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable lp core ibus timeout handle
         */
        uint32_t lp_core_ibus_timeout_en:1;
        /** lp_core_ibus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set lp core ibus timeout  threshold
         */
        uint32_t lp_core_ibus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_system_reg_lp_core_ibus_timeout_reg_t;

/** Type of lp_core_dbus_timeout register
 *  need_des
 */
typedef union {
    struct {
        /** lp_core_dbus_timeout_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable lp core dbus timeout handle
         */
        uint32_t lp_core_dbus_timeout_en:1;
        /** lp_core_dbus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set lp core dbus timeout  threshold
         */
        uint32_t lp_core_dbus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_system_reg_lp_core_dbus_timeout_reg_t;


/** Group: status_register */
/** Type of lp_addrhole_addr register
 *  need_des
 */
typedef union {
    struct {
        /** lp_addrhole_addr : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_addrhole_addr:32;
    };
    uint32_t val;
} lp_system_reg_lp_addrhole_addr_reg_t;

/** Type of lp_addrhole_info register
 *  need_des
 */
typedef union {
    struct {
        /** lp_addrhole_id : RO; bitpos: [4:0]; default: 0;
         *  master id: 5'h0: hp core0, 5'h1:hp core1, 5'h2:lp core, 5'h3:usb otg11, 5'h4:
         *  regdma, 5'h5: gmac, 5'h5 sdmmc, 5'h7: usbotg20, 5'h8: trace0, 5'h9: trace1, 5'ha
         *  tcm monitor, 5'hb: l2mem monitor. 5'h10~5'h1f: ahb pdma.
         */
        uint32_t lp_addrhole_id:5;
        /** lp_addrhole_wr : RO; bitpos: [5]; default: 0;
         *  1:write trans, 0: read trans.
         */
        uint32_t lp_addrhole_wr:1;
        /** lp_addrhole_secure : RO; bitpos: [6]; default: 0;
         *  1: illegal address access, 0: access without permission
         */
        uint32_t lp_addrhole_secure:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_system_reg_lp_addrhole_info_reg_t;

/** Type of lp_cpu_dbg_pc register
 *  need_des
 */
typedef union {
    struct {
        /** lp_cpu_dbg_pc : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_dbg_pc:32;
    };
    uint32_t val;
} lp_system_reg_lp_cpu_dbg_pc_reg_t;

/** Type of lp_cpu_exc_pc register
 *  need_des
 */
typedef union {
    struct {
        /** lp_cpu_exc_pc : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_exc_pc:32;
    };
    uint32_t val;
} lp_system_reg_lp_cpu_exc_pc_reg_t;

/** Type of idbus_addrhole_addr register
 *  need_des
 */
typedef union {
    struct {
        /** idbus_addrhole_addr : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t idbus_addrhole_addr:32;
    };
    uint32_t val;
} lp_system_reg_idbus_addrhole_addr_reg_t;

/** Type of idbus_addrhole_info register
 *  need_des
 */
typedef union {
    struct {
        /** idbus_addrhole_id : RO; bitpos: [4:0]; default: 0;
         *  need_des
         */
        uint32_t idbus_addrhole_id:5;
        /** idbus_addrhole_wr : RO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t idbus_addrhole_wr:1;
        /** idbus_addrhole_secure : RO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t idbus_addrhole_secure:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_system_reg_idbus_addrhole_info_reg_t;

/** Type of rng_data register
 *  rng data register
 */
typedef union {
    struct {
        /** rnd_data : RO; bitpos: [31:0]; default: 0;
         *  result of rng output
         */
        uint32_t rnd_data:32;
    };
    uint32_t val;
} lp_system_reg_rng_data_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  raw interrupt register
 */
typedef union {
    struct {
        /** lp_addrhole_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  the raw interrupt status of lp addrhole(for lp peri  and lp ram tee apm, and lp
         *  matrix default slave)
         */
        uint32_t lp_addrhole_int_raw:1;
        /** idbus_addrhole_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
         *  the raw interrupt status of idbus addrhole(only for lp cpu ibus and dbus)
         */
        uint32_t idbus_addrhole_int_raw:1;
        /** lp_core_ahb_timeout_int_raw : R/SS/WTC; bitpos: [2]; default: 0;
         *  the raw interrupt status of lp core ahb bus timeout
         */
        uint32_t lp_core_ahb_timeout_int_raw:1;
        /** lp_core_ibus_timeout_int_raw : R/SS/WTC; bitpos: [3]; default: 0;
         *  the raw interrupt status of lp core  ibus timeout
         */
        uint32_t lp_core_ibus_timeout_int_raw:1;
        /** lp_core_dbus_timeout_int_raw : R/SS/WTC; bitpos: [4]; default: 0;
         *  the raw interrupt status of lp core  dbus timeout
         */
        uint32_t lp_core_dbus_timeout_int_raw:1;
        /** etm_task_ulp_int_raw : R/SS/WTC; bitpos: [5]; default: 0;
         *  the raw interrupt status of etm task ulp
         */
        uint32_t etm_task_ulp_int_raw:1;
        /** slow_clk_tick_int_raw : R/SS/WTC; bitpos: [6]; default: 0;
         *  the raw interrupt status of slow_clk_tick
         */
        uint32_t slow_clk_tick_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_system_reg_int_raw_reg_t;

/** Type of int_st register
 *  masked interrupt register
 */
typedef union {
    struct {
        /** lp_addrhole_int_st : RO; bitpos: [0]; default: 0;
         *  the masked interrupt status of lp addrhole (for lp peri  and lp ram tee apm, and lp
         *  matrix default slave)
         */
        uint32_t lp_addrhole_int_st:1;
        /** idbus_addrhole_int_st : RO; bitpos: [1]; default: 0;
         *  the masked interrupt status of idbus addrhole(only for lp cpu ibus and dbus)
         */
        uint32_t idbus_addrhole_int_st:1;
        /** lp_core_ahb_timeout_int_st : RO; bitpos: [2]; default: 0;
         *  the masked interrupt status of lp core ahb bus timeout
         */
        uint32_t lp_core_ahb_timeout_int_st:1;
        /** lp_core_ibus_timeout_int_st : RO; bitpos: [3]; default: 0;
         *  the masked interrupt status of lp core  ibus timeout
         */
        uint32_t lp_core_ibus_timeout_int_st:1;
        /** lp_core_dbus_timeout_int_st : RO; bitpos: [4]; default: 0;
         *  the masked interrupt status of lp core  dbus timeout
         */
        uint32_t lp_core_dbus_timeout_int_st:1;
        /** etm_task_ulp_int_st : RO; bitpos: [5]; default: 0;
         *  the masked interrupt status of etm task ulp
         */
        uint32_t etm_task_ulp_int_st:1;
        /** slow_clk_tick_int_st : RO; bitpos: [6]; default: 0;
         *  the masked interrupt status of slow_clk_tick
         */
        uint32_t slow_clk_tick_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_system_reg_int_st_reg_t;

/** Type of int_ena register
 *  masked interrupt register
 */
typedef union {
    struct {
        /** lp_addrhole_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable lp addrhole int
         */
        uint32_t lp_addrhole_int_ena:1;
        /** idbus_addrhole_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable idbus addrhole int
         */
        uint32_t idbus_addrhole_int_ena:1;
        /** lp_core_ahb_timeout_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable lp_core_ahb_timeout int
         */
        uint32_t lp_core_ahb_timeout_int_ena:1;
        /** lp_core_ibus_timeout_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable lp_core_ibus_timeout int
         */
        uint32_t lp_core_ibus_timeout_int_ena:1;
        /** lp_core_dbus_timeout_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable lp_core_dbus_timeout int
         */
        uint32_t lp_core_dbus_timeout_int_ena:1;
        /** etm_task_ulp_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable etm task ulp int
         */
        uint32_t etm_task_ulp_int_ena:1;
        /** slow_clk_tick_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable slow_clk_tick int
         */
        uint32_t slow_clk_tick_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_system_reg_int_ena_reg_t;

/** Type of int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        /** lp_addrhole_int_clr : WT; bitpos: [0]; default: 0;
         *  write 1 to clear lp addrhole int
         */
        uint32_t lp_addrhole_int_clr:1;
        /** idbus_addrhole_int_clr : WT; bitpos: [1]; default: 0;
         *  write 1 to clear idbus addrhole int
         */
        uint32_t idbus_addrhole_int_clr:1;
        /** lp_core_ahb_timeout_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear lp_core_ahb_timeout int
         */
        uint32_t lp_core_ahb_timeout_int_clr:1;
        /** lp_core_ibus_timeout_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear lp_core_ibus_timeout int
         */
        uint32_t lp_core_ibus_timeout_int_clr:1;
        /** lp_core_dbus_timeout_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear lp_core_dbus_timeout int
         */
        uint32_t lp_core_dbus_timeout_int_clr:1;
        /** etm_task_ulp_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear etm tasl ulp int
         */
        uint32_t etm_task_ulp_int_clr:1;
        /** slow_clk_tick_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear slow_clk_tick int
         */
        uint32_t slow_clk_tick_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_system_reg_int_clr_reg_t;


/** Group: control registers */
/** Type of lp_core_err_resp_dis register
 *  need_des
 */
typedef union {
    struct {
        /** lp_core_err_resp_dis : R/W; bitpos: [2:0]; default: 0;
         *  Set bit0 to disable ibus err resp;Set bit1 to disable dbus err resp; Set bit 2 to
         *  disable ahb err resp.
         */
        uint32_t lp_core_err_resp_dis:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_system_reg_lp_core_err_resp_dis_reg_t;

/** Type of rng_cfg register
 *  rng cfg register
 */
typedef union {
    struct {
        /** rng_timer_en : R/W; bitpos: [0]; default: 1;
         *  enable rng timer
         */
        uint32_t rng_timer_en:1;
        /** rng_timer_pscale : R/W; bitpos: [8:1]; default: 1;
         *  configure ng timer pscale
         */
        uint32_t rng_timer_pscale:8;
        /** rng_sar_enable : R/W; bitpos: [9]; default: 0;
         *  enable rng_saradc
         */
        uint32_t rng_sar_enable:1;
        uint32_t reserved_10:6;
        /** rng_sar_data : RO; bitpos: [28:16]; default: 0;
         *  debug rng sar sample cnt
         */
        uint32_t rng_sar_data:13;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} lp_system_reg_rng_cfg_reg_t;


typedef struct {
    volatile lp_system_reg_lp_sys_ver_date_reg_t lp_sys_ver_date;
    volatile lp_system_reg_clk_sel_ctrl_reg_t clk_sel_ctrl;
    volatile lp_system_reg_sys_ctrl_reg_t sys_ctrl;
    volatile lp_system_reg_lp_clk_ctrl_reg_t lp_clk_ctrl;
    volatile lp_system_reg_lp_rst_ctrl_reg_t lp_rst_ctrl;
    uint32_t reserved_014;
    volatile lp_system_reg_lp_core_boot_addr_reg_t lp_core_boot_addr;
    volatile lp_system_reg_ext_wakeup1_reg_t ext_wakeup1;
    volatile lp_system_reg_ext_wakeup1_status_reg_t ext_wakeup1_status;
    volatile lp_system_reg_lp_tcm_pwr_ctrl_reg_t lp_tcm_pwr_ctrl;
    volatile lp_system_reg_boot_addr_hp_lp_reg_reg_t boot_addr_hp_lp_reg;
    volatile lp_system_reg_lp_store0_reg_t lp_store0;
    volatile lp_system_reg_lp_store1_reg_t lp_store1;
    volatile lp_system_reg_lp_store2_reg_t lp_store2;
    volatile lp_system_reg_lp_store3_reg_t lp_store3;
    volatile lp_system_reg_lp_store4_reg_t lp_store4;
    volatile lp_system_reg_lp_store5_reg_t lp_store5;
    volatile lp_system_reg_lp_store6_reg_t lp_store6;
    volatile lp_system_reg_lp_store7_reg_t lp_store7;
    volatile lp_system_reg_lp_store8_reg_t lp_store8;
    volatile lp_system_reg_lp_store9_reg_t lp_store9;
    volatile lp_system_reg_lp_store10_reg_t lp_store10;
    volatile lp_system_reg_lp_store11_reg_t lp_store11;
    volatile lp_system_reg_lp_store12_reg_t lp_store12;
    volatile lp_system_reg_lp_store13_reg_t lp_store13;
    volatile lp_system_reg_lp_store14_reg_t lp_store14;
    volatile lp_system_reg_lp_store15_reg_t lp_store15;
    volatile lp_system_reg_lp_probea_ctrl_reg_t lp_probea_ctrl;
    volatile lp_system_reg_lp_probeb_ctrl_reg_t lp_probeb_ctrl;
    volatile lp_system_reg_lp_probe_out_reg_t lp_probe_out;
    uint32_t reserved_078[9];
    volatile lp_system_reg_f2s_apb_brg_cntl_reg_t f2s_apb_brg_cntl;
    uint32_t reserved_0a0[24];
    volatile lp_system_reg_usb_ctrl_reg_t usb_ctrl;
    uint32_t reserved_104[2];
    volatile lp_system_reg_ana_xpd_pad_group_reg_t ana_xpd_pad_group;
    volatile lp_system_reg_lp_tcm_ram_rdn_eco_cs_reg_t lp_tcm_ram_rdn_eco_cs;
    volatile lp_system_reg_lp_tcm_ram_rdn_eco_low_reg_t lp_tcm_ram_rdn_eco_low;
    volatile lp_system_reg_lp_tcm_ram_rdn_eco_high_reg_t lp_tcm_ram_rdn_eco_high;
    volatile lp_system_reg_lp_tcm_rom_rdn_eco_cs_reg_t lp_tcm_rom_rdn_eco_cs;
    volatile lp_system_reg_lp_tcm_rom_rdn_eco_low_reg_t lp_tcm_rom_rdn_eco_low;
    volatile lp_system_reg_lp_tcm_rom_rdn_eco_high_reg_t lp_tcm_rom_rdn_eco_high;
    uint32_t reserved_128[2];
    volatile lp_system_reg_hp_root_clk_ctrl_reg_t hp_root_clk_ctrl;
    uint32_t reserved_134;
    volatile lp_system_reg_lp_pmu_rdn_eco_low_reg_t lp_pmu_rdn_eco_low;
    volatile lp_system_reg_lp_pmu_rdn_eco_high_reg_t lp_pmu_rdn_eco_high;
    uint32_t reserved_140[2];
    volatile lp_system_reg_pad_comp_reg_t pad_comp[2];
    uint32_t reserved_150;
    volatile lp_system_reg_backup_dma_cfg0_reg_t backup_dma_cfg0;
    volatile lp_system_reg_backup_dma_cfg1_reg_t backup_dma_cfg1;
    volatile lp_system_reg_backup_dma_cfg2_reg_t backup_dma_cfg2;
    uint32_t reserved_160;
    volatile lp_system_reg_boot_addr_hp_core1_reg_t boot_addr_hp_core1;
    volatile lp_system_reg_lp_addrhole_addr_reg_t lp_addrhole_addr;
    volatile lp_system_reg_lp_addrhole_info_reg_t lp_addrhole_info;
    volatile lp_system_reg_int_raw_reg_t int_raw;
    volatile lp_system_reg_int_st_reg_t int_st;
    volatile lp_system_reg_int_ena_reg_t int_ena;
    volatile lp_system_reg_int_clr_reg_t int_clr;
    volatile lp_system_reg_hp_mem_aux_ctrl_reg_t hp_mem_aux_ctrl;
    volatile lp_system_reg_lp_mem_aux_ctrl_reg_t lp_mem_aux_ctrl;
    volatile lp_system_reg_hp_rom_aux_ctrl_reg_t hp_rom_aux_ctrl;
    volatile lp_system_reg_lp_rom_aux_ctrl_reg_t lp_rom_aux_ctrl;
    volatile lp_system_reg_lp_cpu_dbg_pc_reg_t lp_cpu_dbg_pc;
    volatile lp_system_reg_lp_cpu_exc_pc_reg_t lp_cpu_exc_pc;
    volatile lp_system_reg_idbus_addrhole_addr_reg_t idbus_addrhole_addr;
    volatile lp_system_reg_idbus_addrhole_info_reg_t idbus_addrhole_info;
    volatile lp_system_reg_hp_por_rst_bypass_ctrl_reg_t hp_por_rst_bypass_ctrl;
    volatile lp_system_reg_rng_data_reg_t rng_data;
    uint32_t reserved_1a8[2];
    volatile lp_system_reg_lp_core_ahb_timeout_reg_t lp_core_ahb_timeout;
    volatile lp_system_reg_lp_core_ibus_timeout_reg_t lp_core_ibus_timeout;
    volatile lp_system_reg_lp_core_dbus_timeout_reg_t lp_core_dbus_timeout;
    volatile lp_system_reg_lp_core_err_resp_dis_reg_t lp_core_err_resp_dis;
    volatile lp_system_reg_rng_cfg_reg_t rng_cfg;
} lp_system_reg_dev_t;

extern lp_system_reg_dev_t LP_SYS;

#ifndef __cplusplus
_Static_assert(sizeof(lp_system_reg_dev_t) == 0x1c4, "Invalid size of lp_system_reg_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
