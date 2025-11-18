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
/** Type of ver_date register
 *  need_des
 */
typedef union {
    struct {
        /** ver_date : R/W; bitpos: [31:0]; default: 539298083;
         *  need_des
         */
        uint32_t ver_date:32;
    };
    uint32_t val;
} lpsysreg_ver_date_reg_t;

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
} lpsysreg_clk_sel_ctrl_reg_t;

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
} lpsysreg_sys_ctrl_reg_t;

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
} lpsysreg_lp_clk_ctrl_reg_t;

/** Type of lp_core_boot_addr register
 *  need_des
 */
typedef union {
    struct {
        /** lp_cpu_boot_addr : R/W; bitpos: [31:0]; default: 771751936;
         *  need_des
         */
        uint32_t lp_cpu_boot_addr:32;
    };
    uint32_t val;
} lpsysreg_lp_core_boot_addr_reg_t;

/** Type of ext_wakeup1 register
 *  need_des
 */
typedef union {
    struct {
        /** ext_wakeup1_sel : R/W; bitpos: [7:0]; default: 0;
         *  Bitmap to select RTC pads for ext wakeup1
         */
        uint32_t ext_wakeup1_sel:8;
        /** ext_wakeup1_status_clr : WT; bitpos: [8]; default: 0;
         *  clear ext wakeup1 status
         */
        uint32_t ext_wakeup1_status_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lpsysreg_ext_wakeup1_reg_t;

/** Type of ext_wakeup1_status register
 *  need_des
 */
typedef union {
    struct {
        /** ext_wakeup1_status : RO; bitpos: [7:0]; default: 0;
         *  ext wakeup1 status
         */
        uint32_t ext_wakeup1_status:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lpsysreg_ext_wakeup1_status_reg_t;

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
} lpsysreg_lp_tcm_pwr_ctrl_reg_t;

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
} lpsysreg_boot_addr_hp_lp_reg_reg_t;

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
} lpsysreg_lp_store0_reg_t;

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
} lpsysreg_lp_store1_reg_t;

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
} lpsysreg_lp_store2_reg_t;

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
} lpsysreg_lp_store3_reg_t;

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
} lpsysreg_lp_store4_reg_t;

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
} lpsysreg_lp_store5_reg_t;

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
} lpsysreg_lp_store6_reg_t;

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
} lpsysreg_lp_store7_reg_t;

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
} lpsysreg_lp_store8_reg_t;

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
} lpsysreg_lp_store9_reg_t;

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
} lpsysreg_lp_store10_reg_t;

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
} lpsysreg_lp_store11_reg_t;

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
} lpsysreg_lp_store12_reg_t;

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
} lpsysreg_lp_store13_reg_t;

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
} lpsysreg_lp_store14_reg_t;

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
} lpsysreg_lp_store15_reg_t;

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
} lpsysreg_lp_probea_ctrl_reg_t;

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
} lpsysreg_lp_probeb_ctrl_reg_t;

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
} lpsysreg_lp_probe_out_reg_t;

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
} lpsysreg_f2s_apb_brg_cntl_reg_t;

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
        /** usbotghs_wakeup_clr : WT; bitpos: [2]; default: 0;
         *  clear usb wakeup to PMU.
         */
        uint32_t usbotghs_wakeup_clr:1;
        /** usbotghs_in_suspend : R/W; bitpos: [3]; default: 0;
         *  indicate usb otg2.0 hs is in suspend state.
         */
        uint32_t usbotghs_in_suspend:1;
        /** usbotghs_ls_mode : R/W; bitpos: [4]; default: 0;
         *  indicate usb otg2.0 hs ls mode
         */
        uint32_t usbotghs_ls_mode:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lpsysreg_usb_ctrl_reg_t;

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
} lpsysreg_ana_xpd_pad_group_reg_t;

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
} lpsysreg_lp_tcm_ram_rdn_eco_cs_reg_t;

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
} lpsysreg_lp_tcm_ram_rdn_eco_low_reg_t;

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
} lpsysreg_lp_tcm_ram_rdn_eco_high_reg_t;

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
} lpsysreg_lp_tcm_rom_rdn_eco_cs_reg_t;

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
} lpsysreg_lp_tcm_rom_rdn_eco_low_reg_t;

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
} lpsysreg_lp_tcm_rom_rdn_eco_high_reg_t;

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
} lpsysreg_hp_root_clk_ctrl_reg_t;

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
} lpsysreg_lp_pmu_rdn_eco_low_reg_t;

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
} lpsysreg_lp_pmu_rdn_eco_high_reg_t;

/** Type of backup_dma_cfg0 register
 *  need_des
 */
typedef union {
    struct {
        /** burst_limit_aon : R/W; bitpos: [4:0]; default: 10;
         *  Set this field to configure max value of burst in signal transfer.
         */
        uint32_t burst_limit_aon:5;
        /** read_interval_aon : R/W; bitpos: [11:5]; default: 10;
         *  Set this field to configure read registers' interval time in reading mode.
         */
        uint32_t read_interval_aon:7;
        /** link_backup_tout_thres_aon : R/W; bitpos: [21:12]; default: 100;
         *  Set this field to configure maximum waiting time in backup mode.
         */
        uint32_t link_backup_tout_thres_aon:10;
        /** regdma_error : RO; bitpos: [24:22]; default: 0;
         *  regdma error code
         */
        uint32_t regdma_error:3;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} lpsysreg_backup_dma_cfg0_reg_t;

/** Type of backup_dma_cfg1 register
 *  need_des
 */
typedef union {
    struct {
        /** link_wait_tout_thres_aon : R/W; bitpos: [9:0]; default: 100;
         *  Set this field to configure the number of consecutive links of link list.
         */
        uint32_t link_wait_tout_thres_aon:10;
        /** link_work_tout_thres_aon : R/W; bitpos: [19:10]; default: 100;
         *  Set this field to configure maximum waiting time in waiting mode.
         */
        uint32_t link_work_tout_thres_aon:10;
        /** branch_link_length_aon : R/W; bitpos: [23:20]; default: 0;
         *  Set this field to configure link address.
         */
        uint32_t branch_link_length_aon:4;
        uint32_t reserved_24:7;
        /** aon_bypass : R/W; bitpos: [31]; default: 0;
         *  unused
         */
        uint32_t aon_bypass:1;
    };
    uint32_t val;
} lpsysreg_backup_dma_cfg1_reg_t;

/** Type of backup_dma_cfg2 register
 *  need_des
 */
typedef union {
    struct {
        /** link_addr_aon : R/W; bitpos: [31:0]; default: 0;
         *  Set this field to configure link address.
         */
        uint32_t link_addr_aon:32;
    };
    uint32_t val;
} lpsysreg_backup_dma_cfg2_reg_t;

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
} lpsysreg_boot_addr_hp_core1_reg_t;

/** Type of sprf_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} lpsysreg_sprf_mem_aux_ctrl_reg_t;

/** Type of hp_por_rst_bypass_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** hp_po_cpu_rstn_bypass_ctrl : R/W; bitpos: [7:0]; default: 255;
         *  [7] 1'b1: po_cpu_rstn bypass sys_sw_rstn
         *  [6] 1'b1: po_cpu_rstn bypass hp_wdt_sys_rstn
         *  [5] 1'b1: po_cpu_rstn bypass hp_cpu_intrusion_rstn
         *  [4] 1'b1: po_cpu_rstn bypass hp_sdio_sys_rstn
         *  [3] 1'b1: po_cpu_rstn bypass usb_jtag_chip_rst
         *  [2] 1'b1: po_cpu_rstn bypass usb_uart_chip_rst
         *  [1] 1'b1: po_cpu_rstn bypass lp_wdt_hp_sys_rstn
         *  [0] 1'b1: po_cpu_rstn bypass efuse_err_rstn
         */
        uint32_t hp_po_cpu_rstn_bypass_ctrl:8;
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
        /** hp_po_alive_rstn_bypass_ctrl : R/W; bitpos: [23:16]; default: 255;
         *  [23] 1'b1: po_alive_rstn bypass sys_sw_rstn
         *  [22] 1'b1: po_alive_rstn bypass hp_wdt_sys_rstn
         *  [21] 1'b1: po_alive_rstn bypass hp_cpu_intrusion_rstn
         *  [20] 1'b1: po_alive_rstn bypass hp_sdio_sys_rstn
         *  [19] 1'b1: po_alive_rstn bypass usb_jtag_chip_rst
         *  [18] 1'b1: po_alive_rstn bypass usb_uart_chip_rst
         *  [17] 1'b1: po_alive_rstn bypass lp_wdt_hp_sys_rstn
         *  [16] 1'b1: po_alive_rstn bypass efuse_err_rstn
         */
        uint32_t hp_po_alive_rstn_bypass_ctrl:8;
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
} lpsysreg_hp_por_rst_bypass_ctrl_reg_t;

/** Type of lp_core_ibus_timeout_conf register
 *  need_des
 */
typedef union {
    struct {
        /** lp_core_ibus_timeout_protect_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable lp core ibus timeout handle
         */
        uint32_t lp_core_ibus_timeout_protect_en:1;
        /** lp_core_ibus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set lp core ibus timeout  threshold
         */
        uint32_t lp_core_ibus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lpsysreg_lp_core_ibus_timeout_conf_reg_t;

/** Type of lp_core_dbus_timeout_conf register
 *  need_des
 */
typedef union {
    struct {
        /** lp_core_dbus_timeout_protect_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable lp core dbus timeout handle
         */
        uint32_t lp_core_dbus_timeout_protect_en:1;
        /** lp_core_dbus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set lp core dbus timeout  threshold
         */
        uint32_t lp_core_dbus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lpsysreg_lp_core_dbus_timeout_conf_reg_t;

/** Type of spram_mem_aux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  need_des
         */
        uint32_t spram_mem_aux_ctrl:32;
    };
    uint32_t val;
} lpsysreg_spram_mem_aux_ctrl_reg_t;

/** Type of uart_mem_lp_ctrl register
 *  config lp peri memory power in lp aon register
 */
typedef union {
    struct {
        /** uart_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures lp uart memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t uart_mem_lp_mode:2;
        /** uart_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down lp uart memory.
         */
        uint32_t uart_mem_lp_en:1;
        /** uart_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control lp uart memory, disable hardware control.
         */
        uint32_t uart_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lpsysreg_uart_mem_lp_ctrl_reg_t;

/** Type of huk_mem_lp_ctrl register
 *  config lp peri memory power in lp aon register
 */
typedef union {
    struct {
        /** huk_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures huk memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t huk_mem_lp_mode:2;
        /** huk_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down huk memory.
         */
        uint32_t huk_mem_lp_en:1;
        /** huk_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control huk memory, disable hardware control.
         */
        uint32_t huk_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lpsysreg_huk_mem_lp_ctrl_reg_t;

/** Type of efuse_mem_lp_ctrl register
 *  config lp peri memory power in lp aon register
 */
typedef union {
    struct {
        /** efuse_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures efuse memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t efuse_mem_lp_mode:2;
        /** efuse_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down efuse memory.
         */
        uint32_t efuse_mem_lp_en:1;
        /** efuse_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control efuse memory, disable hardware control.
         */
        uint32_t efuse_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lpsysreg_efuse_mem_lp_ctrl_reg_t;

/** Type of lp_tcm_mem_lp_ctrl register
 *  configure rmemory power in lp system register
 */
typedef union {
    struct {
        /** lp_tcm_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures lp_sram memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t lp_tcm_mem_lp_mode:2;
        /** lp_tcm_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down lp_sram memory.
         */
        uint32_t lp_tcm_mem_lp_en:1;
        /** lp_tcm_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control lp_sram memory, disable hardware control.
         */
        uint32_t lp_tcm_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lpsysreg_lp_tcm_mem_lp_ctrl_reg_t;

/** Type of modem_mem_clk_sel register
 *  configure clock source for modme_mem, this register must be config by regdma when
 *  power on
 */
typedef union {
    struct {
        /** modem_mem_clk_sel : R/W; bitpos: [0]; default: 0;
         *  configure clock source for modme_mem, this register must be config by regdma when
         *  power on
         */
        uint32_t modem_mem_clk_sel:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lpsysreg_modem_mem_clk_sel_reg_t;

/** Type of rtc_en_amux register
 *  RTC_EN_AMUX register
 */
typedef union {
    struct {
        /** rtc_en_amux : R/W; bitpos: [17:0]; default: 0;
         *  Configures the rtc_en_amux of the pad in hp_pad_top
         */
        uint32_t rtc_en_amux:18;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} lpsysreg_rtc_en_amux_reg_t;

/** Type of rtc_en_dmux register
 *  RTC_EN_DMUX register
 */
typedef union {
    struct {
        /** rtc_en_dmux : R/W; bitpos: [1:0]; default: 0;
         *  Configures the rtc_en_dmux of the pad(digmux) in hp_pad_top
         */
        uint32_t rtc_en_dmux:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lpsysreg_rtc_en_dmux_reg_t;

/** Type of digmux_pad_dsel register
 *  DIGMUX_PAD_DSEL register
 */
typedef union {
    struct {
        /** digmux_pad_dsel : R/W; bitpos: [5:0]; default: 0;
         *  Configures the rtc_en_dmux of the pad(digmux) in hp_pad_top
         */
        uint32_t digmux_pad_dsel:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lpsysreg_digmux_pad_dsel_reg_t;

/** Type of jtag_tdio_mux_sel register
 *  LP_JTAG_TDIO_MUX_SEL register
 */
typedef union {
    struct {
        /** jtag_tdio_mux_sel : R/W; bitpos: [1:0]; default: 0;
         *  Bit[0] selects the jtdi source for lp_core, 1'h0: from tdo of the hp_core; 1'h1:
         *  from pad tdi(lp_core on, hp_core off)
         *  Bit[1] selects the jtdo source for usj_mux, 1'h0: from tdo of the lp_core; 1'h1:
         *  from tdi of the lp_core(lp_core off, hp_core on)
         */
        uint32_t jtag_tdio_mux_sel:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lpsysreg_jtag_tdio_mux_sel_reg_t;

/** Type of touch_pad_rtc_en_amux register
 *  TOUCH_PAD_RTC_EN_AMUX register
 */
typedef union {
    struct {
        /** touch_pad_rtc_en_amux : R/W; bitpos: [13:0]; default: 0;
         *  Configures the rtc_en_amux of the touch_pads
         *  amux_reg[13:0] <->TOUCH_GPIO[19:6]
         */
        uint32_t touch_pad_rtc_en_amux:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} lpsysreg_touch_pad_rtc_en_amux_reg_t;

/** Type of hp_gpio_o_hold_ctrl1 register
 *  NA
 */
typedef union {
    struct {
        /** hp_gpio_0_hold_ctrl1 : R/W; bitpos: [22:0]; default: 0;
         *  hold control for gpio62~40
         */
        uint32_t hp_gpio_0_hold_ctrl1:23;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lpsysreg_hp_gpio_o_hold_ctrl1_reg_t;

/** Type of hp_gpio_o_hold_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** hp_gpio_0_hold_ctrl0 : R/W; bitpos: [31:0]; default: 0;
         *  hold control for gpio39~8
         */
        uint32_t hp_gpio_0_hold_ctrl0:32;
    };
    uint32_t val;
} lpsysreg_hp_gpio_o_hold_ctrl0_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  raw interrupt register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
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
} lpsysreg_int_raw_reg_t;

/** Type of int_st register
 *  masked interrupt register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
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
} lpsysreg_int_st_reg_t;

/** Type of int_ena register
 *  masked interrupt register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
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
} lpsysreg_int_ena_reg_t;

/** Type of int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
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
} lpsysreg_int_clr_reg_t;


/** Group: status_register */
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
} lpsysreg_lp_cpu_dbg_pc_reg_t;

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
} lpsysreg_lp_cpu_exc_pc_reg_t;

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
} lpsysreg_rng_data_reg_t;

/** Type of por_cur_st register
 *  need_des
 */
typedef union {
    struct {
        /** por_cur_st : RO; bitpos: [8:0]; default: 0;
         *  current por state
         */
        uint32_t por_cur_st:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lpsysreg_por_cur_st_reg_t;

/** Type of lp_core_dmactive_lpcore register
 *  LP_JTAG_TDI_SEL register
 */
typedef union {
    struct {
        /** lp_core_dmactive_lpcore : RO; bitpos: [0]; default: 0;
         *  Record lpcore_dmactive_lpcore status
         */
        uint32_t lp_core_dmactive_lpcore:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lpsysreg_lp_core_dmactive_lpcore_reg_t;


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
} lpsysreg_lp_core_err_resp_dis_reg_t;

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
} lpsysreg_rng_cfg_reg_t;

/** Type of padctrl register
 *  pad ctrl register
 */
typedef union {
    struct {
        /** pad_mux_sel : R/W; bitpos: [7:0]; default: 0;
         *  Pad 0 ~ 7 can be controlled by hp iomux or lp iomux
         *  0: pad is controlled by hp iomux
         *  1: pad is controlled by lp iomux
         */
        uint32_t pad_mux_sel:8;
        /** pad_hold : R/W; bitpos: [15:8]; default: 0;
         *  Pad 0 ~ 7 hold, direct control, won't be muxed with hp
         */
        uint32_t pad_hold:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lpsysreg_padctrl_reg_t;


/** Group: Configuration Register */
/** Type of sys_power_status register
 *  lp system pd ctrl register
 */
typedef union {
    struct {
        /** lp_tcm_ram_prdyn : RO; bitpos: [0]; default: 1;
         *  Record lp tcm ram power status.
         */
        uint32_t lp_tcm_ram_prdyn:1;
        /** lp_tcm_rom_prdyn : RO; bitpos: [1]; default: 1;
         *  Record lp tcm rom power status.
         */
        uint32_t lp_tcm_rom_prdyn:1;
        /** lp_huk_mem_prdyn : RO; bitpos: [2]; default: 1;
         *  Record lp huk mem power status.
         */
        uint32_t lp_huk_mem_prdyn:1;
        /** lp_efuse_mem_prdyn : RO; bitpos: [3]; default: 1;
         *  Record lp efuse mem power status.
         */
        uint32_t lp_efuse_mem_prdyn:1;
        /** lp_uart_mem_prdyn : RO; bitpos: [4]; default: 1;
         *  Record lp uart mem power status.
         */
        uint32_t lp_uart_mem_prdyn:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lpsysreg_sys_power_status_reg_t;

/** Type of ana_dcdc register
 *  need_des
 */
typedef union {
    struct {
        /** ramplevel_dcdc : HRO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t ramplevel_dcdc:1;
        /** ramp_dcdc : HRO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t ramp_dcdc:1;
        /** dcm2enb_dcdc : HRO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t dcm2enb_dcdc:1;
        /** dcmlevel_dcdc : HRO; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t dcmlevel_dcdc:2;
        /** fsw_dcdc : HRO; bitpos: [7:5]; default: 0;
         *  need_des
         */
        uint32_t fsw_dcdc:3;
        /** ccm_dcdc : HRO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t ccm_dcdc:1;
        /** sstime_dcdc : HRO; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t sstime_dcdc:1;
        /** pocpenb_dcdc : HRO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t pocpenb_dcdc:1;
        /** enable_trx_mode_dcdc : HRO; bitpos: [11]; default: 1;
         *  need_des
         */
        uint32_t enable_trx_mode_dcdc:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lpsysreg_ana_dcdc_reg_t;


/** Group: Timeout Register */
/** Type of lp_peri_timeout_conf register
 *  LP_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** lp_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing CPU peripheral
         *  register in the number of clock cycles of the clock domain.
         */
        uint32_t lp_peri_timeout_thres:16;
        /** lp_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear timeout interrupt.
         */
        uint32_t lp_peri_timeout_int_clear:1;
        /** lp_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing CPU peripheral
         *  registers.
         *  0: Disable
         *  1: Enable
         */
        uint32_t lp_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} lpsysreg_lp_peri_timeout_conf_reg_t;

/** Type of lp_peri_timeout_addr register
 *  LP_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** lp_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t lp_peri_timeout_addr:32;
    };
    uint32_t val;
} lpsysreg_lp_peri_timeout_addr_reg_t;

/** Type of lp_peri_timeout_uid register
 *  LP_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** lp_peri_timeout_uid : RO; bitpos: [7:0]; default: 0;
         *  Represents the master id[5:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t lp_peri_timeout_uid:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lpsysreg_lp_peri_timeout_uid_reg_t;


typedef struct {
    volatile lpsysreg_ver_date_reg_t ver_date;
    volatile lpsysreg_clk_sel_ctrl_reg_t clk_sel_ctrl;
    volatile lpsysreg_sys_ctrl_reg_t sys_ctrl;
    volatile lpsysreg_lp_clk_ctrl_reg_t lp_clk_ctrl;
    uint32_t reserved_010[2];
    volatile lpsysreg_lp_core_boot_addr_reg_t lp_core_boot_addr;
    volatile lpsysreg_ext_wakeup1_reg_t ext_wakeup1;
    volatile lpsysreg_ext_wakeup1_status_reg_t ext_wakeup1_status;
    volatile lpsysreg_lp_tcm_pwr_ctrl_reg_t lp_tcm_pwr_ctrl;
    volatile lpsysreg_boot_addr_hp_lp_reg_reg_t boot_addr_hp_lp_reg;
    volatile lpsysreg_lp_store0_reg_t lp_store0;
    volatile lpsysreg_lp_store1_reg_t lp_store1;
    volatile lpsysreg_lp_store2_reg_t lp_store2;
    volatile lpsysreg_lp_store3_reg_t lp_store3;
    volatile lpsysreg_lp_store4_reg_t lp_store4;
    volatile lpsysreg_lp_store5_reg_t lp_store5;
    volatile lpsysreg_lp_store6_reg_t lp_store6;
    volatile lpsysreg_lp_store7_reg_t lp_store7;
    volatile lpsysreg_lp_store8_reg_t lp_store8;
    volatile lpsysreg_lp_store9_reg_t lp_store9;
    volatile lpsysreg_lp_store10_reg_t lp_store10;
    volatile lpsysreg_lp_store11_reg_t lp_store11;
    volatile lpsysreg_lp_store12_reg_t lp_store12;
    volatile lpsysreg_lp_store13_reg_t lp_store13;
    volatile lpsysreg_lp_store14_reg_t lp_store14;
    volatile lpsysreg_lp_store15_reg_t lp_store15;
    volatile lpsysreg_lp_probea_ctrl_reg_t lp_probea_ctrl;
    volatile lpsysreg_lp_probeb_ctrl_reg_t lp_probeb_ctrl;
    volatile lpsysreg_lp_probe_out_reg_t lp_probe_out;
    uint32_t reserved_078[9];
    volatile lpsysreg_f2s_apb_brg_cntl_reg_t f2s_apb_brg_cntl;
    uint32_t reserved_0a0[24];
    volatile lpsysreg_usb_ctrl_reg_t usb_ctrl;
    uint32_t reserved_104[2];
    volatile lpsysreg_ana_xpd_pad_group_reg_t ana_xpd_pad_group;
    volatile lpsysreg_lp_tcm_ram_rdn_eco_cs_reg_t lp_tcm_ram_rdn_eco_cs;
    volatile lpsysreg_lp_tcm_ram_rdn_eco_low_reg_t lp_tcm_ram_rdn_eco_low;
    volatile lpsysreg_lp_tcm_ram_rdn_eco_high_reg_t lp_tcm_ram_rdn_eco_high;
    volatile lpsysreg_lp_tcm_rom_rdn_eco_cs_reg_t lp_tcm_rom_rdn_eco_cs;
    volatile lpsysreg_lp_tcm_rom_rdn_eco_low_reg_t lp_tcm_rom_rdn_eco_low;
    volatile lpsysreg_lp_tcm_rom_rdn_eco_high_reg_t lp_tcm_rom_rdn_eco_high;
    uint32_t reserved_128[2];
    volatile lpsysreg_hp_root_clk_ctrl_reg_t hp_root_clk_ctrl;
    uint32_t reserved_134;
    volatile lpsysreg_lp_pmu_rdn_eco_low_reg_t lp_pmu_rdn_eco_low;
    volatile lpsysreg_lp_pmu_rdn_eco_high_reg_t lp_pmu_rdn_eco_high;
    uint32_t reserved_140[5];
    volatile lpsysreg_backup_dma_cfg0_reg_t backup_dma_cfg0;
    volatile lpsysreg_backup_dma_cfg1_reg_t backup_dma_cfg1;
    volatile lpsysreg_backup_dma_cfg2_reg_t backup_dma_cfg2;
    uint32_t reserved_160;
    volatile lpsysreg_boot_addr_hp_core1_reg_t boot_addr_hp_core1;
    uint32_t reserved_168[2];
    volatile lpsysreg_int_raw_reg_t int_raw;
    volatile lpsysreg_int_st_reg_t int_st;
    volatile lpsysreg_int_ena_reg_t int_ena;
    volatile lpsysreg_int_clr_reg_t int_clr;
    volatile lpsysreg_sprf_mem_aux_ctrl_reg_t sprf_mem_aux_ctrl;
    uint32_t reserved_184[3];
    volatile lpsysreg_lp_cpu_dbg_pc_reg_t lp_cpu_dbg_pc;
    volatile lpsysreg_lp_cpu_exc_pc_reg_t lp_cpu_exc_pc;
    uint32_t reserved_198[2];
    volatile lpsysreg_hp_por_rst_bypass_ctrl_reg_t hp_por_rst_bypass_ctrl;
    volatile lpsysreg_rng_data_reg_t rng_data;
    uint32_t reserved_1a8[3];
    volatile lpsysreg_lp_core_ibus_timeout_conf_reg_t lp_core_ibus_timeout_conf;
    volatile lpsysreg_lp_core_dbus_timeout_conf_reg_t lp_core_dbus_timeout_conf;
    volatile lpsysreg_lp_core_err_resp_dis_reg_t lp_core_err_resp_dis;
    volatile lpsysreg_rng_cfg_reg_t rng_cfg;
    volatile lpsysreg_padctrl_reg_t padctrl;
    uint32_t reserved_1c8[8];
    volatile lpsysreg_sys_power_status_reg_t sys_power_status;
    volatile lpsysreg_ana_dcdc_reg_t ana_dcdc;
    volatile lpsysreg_por_cur_st_reg_t por_cur_st;
    uint32_t reserved_1f4;
    volatile lpsysreg_spram_mem_aux_ctrl_reg_t spram_mem_aux_ctrl;
    volatile lpsysreg_uart_mem_lp_ctrl_reg_t uart_mem_lp_ctrl;
    volatile lpsysreg_huk_mem_lp_ctrl_reg_t huk_mem_lp_ctrl;
    volatile lpsysreg_efuse_mem_lp_ctrl_reg_t efuse_mem_lp_ctrl;
    volatile lpsysreg_lp_tcm_mem_lp_ctrl_reg_t lp_tcm_mem_lp_ctrl;
    uint32_t reserved_20c[3];
    volatile lpsysreg_modem_mem_clk_sel_reg_t modem_mem_clk_sel;
    volatile lpsysreg_lp_peri_timeout_conf_reg_t lp_peri_timeout_conf;
    volatile lpsysreg_lp_peri_timeout_addr_reg_t lp_peri_timeout_addr;
    volatile lpsysreg_lp_peri_timeout_uid_reg_t lp_peri_timeout_uid;
    volatile lpsysreg_rtc_en_amux_reg_t rtc_en_amux;
    volatile lpsysreg_rtc_en_dmux_reg_t rtc_en_dmux;
    volatile lpsysreg_digmux_pad_dsel_reg_t digmux_pad_dsel;
    volatile lpsysreg_jtag_tdio_mux_sel_reg_t jtag_tdio_mux_sel;
    volatile lpsysreg_lp_core_dmactive_lpcore_reg_t lp_core_dmactive_lpcore;
    volatile lpsysreg_touch_pad_rtc_en_amux_reg_t touch_pad_rtc_en_amux;
    uint32_t reserved_240[12];
    volatile lpsysreg_hp_gpio_o_hold_ctrl1_reg_t hp_gpio_o_hold_ctrl1;
    volatile lpsysreg_hp_gpio_o_hold_ctrl0_reg_t hp_gpio_o_hold_ctrl0;
} lp_system_reg_dev_t;

extern lp_system_reg_dev_t LP_SYS;

#ifndef __cplusplus
_Static_assert(sizeof(lp_system_reg_dev_t) == 0x278, "Invalid size of lp_system_reg_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
