/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of aon_store0 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store0 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store0:32;
    };
    uint32_t val;
} lp_aon_store0_reg_t;

/** Type of aon_store1 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store1 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store1:32;
    };
    uint32_t val;
} lp_aon_store1_reg_t;

/** Type of aon_store2 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store2 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store2:32;
    };
    uint32_t val;
} lp_aon_store2_reg_t;

/** Type of aon_store3 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store3 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store3:32;
    };
    uint32_t val;
} lp_aon_store3_reg_t;

/** Type of aon_store4 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store4 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store4:32;
    };
    uint32_t val;
} lp_aon_store4_reg_t;

/** Type of aon_store5 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store5 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store5:32;
    };
    uint32_t val;
} lp_aon_store5_reg_t;

/** Type of aon_store6 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store6 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store6:32;
    };
    uint32_t val;
} lp_aon_store6_reg_t;

/** Type of aon_store7 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store7 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store7:32;
    };
    uint32_t val;
} lp_aon_store7_reg_t;

/** Type of aon_store8 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store8 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store8:32;
    };
    uint32_t val;
} lp_aon_store8_reg_t;

/** Type of aon_store9 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lp_aon_store9 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_lp_aon_store9:32;
    };
    uint32_t val;
} lp_aon_store9_reg_t;

/** Type of aon_gpio_mux register
 *  need_des
 */
typedef union {
    struct {
        /** aon_gpio_mux_sel : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t aon_gpio_mux_sel:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_aon_gpio_mux_reg_t;

/** Type of aon_gpio_hold0 register
 *  need_des
 */
typedef union {
    struct {
        /** gpio_hold0 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t gpio_hold0:32;
    };
    uint32_t val;
} lp_aon_gpio_hold0_reg_t;

/** Type of aon_gpio_hold1 register
 *  need_des
 */
typedef union {
    struct {
        /** aon_gpio_hold1 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t aon_gpio_hold1:32;
    };
    uint32_t val;
} lp_aon_gpio_hold1_reg_t;

/** Type of aon_sys_cfg register
 *  need_des
 */
typedef union {
    struct {
        /** aon_ana_fib_swd_enable : RO; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t aon_ana_fib_swd_enable:1;
        /** aon_ana_fib_ck_glitch_enable : RO; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t aon_ana_fib_ck_glitch_enable:1;
        /** aon_ana_fib_bod_enable : RO; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t aon_ana_fib_bod_enable:1;
        uint32_t reserved_3:27;
        /** aon_force_download_boot : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t aon_force_download_boot:1;
        /** aon_hpsys_sw_reset : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_hpsys_sw_reset:1;
    };
    uint32_t val;
} lp_aon_sys_cfg_reg_t;

/** Type of aon_cpucore0_cfg register
 *  need_des
 */
typedef union {
    struct {
        /** aon_cpu_core0_sw_stall : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t aon_cpu_core0_sw_stall:8;
        uint32_t reserved_8:20;
        /** aon_cpu_core0_sw_reset : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t aon_cpu_core0_sw_reset:1;
        /** aon_cpu_core0_ocd_halt_on_reset : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t aon_cpu_core0_ocd_halt_on_reset:1;
        /** aon_cpu_core0_stat_vector_sel : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t aon_cpu_core0_stat_vector_sel:1;
        /** aon_cpu_core0_dreset_mask : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_cpu_core0_dreset_mask:1;
    };
    uint32_t val;
} lp_aon_cpucore0_cfg_reg_t;

/** Type of aon_io_mux register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** aon_io_mux_pull_ldo : R/W; bitpos: [30:28]; default: 0;
         *  need_des
         */
        uint32_t aon_io_mux_pull_ldo:3;
        /** aon_io_mux_reset_disable : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_io_mux_reset_disable:1;
    };
    uint32_t val;
} lp_aon_io_mux_reg_t;

/** Type of aon_ext_wakeup_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** aon_ext_wakeup_status : RO; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t aon_ext_wakeup_status:8;
        uint32_t reserved_8:6;
        /** aon_ext_wakeup_status_clr : WT; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t aon_ext_wakeup_status_clr:1;
        /** aon_ext_wakeup_sel : R/W; bitpos: [22:15]; default: 0;
         *  need_des
         */
        uint32_t aon_ext_wakeup_sel:8;
        /** aon_ext_wakeup_lv : R/W; bitpos: [30:23]; default: 0;
         *  need_des
         */
        uint32_t aon_ext_wakeup_lv:8;
        /** aon_ext_wakeup_filter : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_ext_wakeup_filter:1;
    };
    uint32_t val;
} lp_aon_ext_wakeup_cntl_reg_t;

/** Type of aon_usb register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** aon_usb_reset_disable : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_usb_reset_disable:1;
    };
    uint32_t val;
} lp_aon_usb_reg_t;

/** Type of aon_sdio_active register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** aon_sdio_act_dnum : R/W; bitpos: [31:22]; default: 10;
         *  need_des
         */
        uint32_t aon_sdio_act_dnum:10;
    };
    uint32_t val;
} lp_aon_sdio_active_reg_t;

/** Type of aon_lpcore register
 *  need_des
 */
typedef union {
    struct {
        /** aon_lpcore_etm_wakeup_flag_clr : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t aon_lpcore_etm_wakeup_flag_clr:1;
        /** aon_lpcore_etm_wakeup_flag : R/WTC/SS; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t aon_lpcore_etm_wakeup_flag:1;
        uint32_t reserved_2:29;
        /** aon_lpcore_disable : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_lpcore_disable:1;
    };
    uint32_t val;
} lp_aon_lpcore_reg_t;

/** Type of aon_sar_cct register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** aon_sar2_pwdet_cct : R/W; bitpos: [31:29]; default: 0;
         *  need_des
         */
        uint32_t aon_sar2_pwdet_cct:3;
    };
    uint32_t val;
} lp_aon_sar_cct_reg_t;

/** Type of aon_jtag_sel register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** aon_jtag_sel_soft : R/W; bitpos: [31]; default: 1;
         *  If strapping_sel_jtag feature is disabled by efuse, and if neither pad_jtag or
         *  usb_jtag is disabled by efuse, this field determines which one jtag between
         *  usb_jtag and pad_jtag will be used. 1'b1(default): usb_jtag, 1'b0: pad_jtag.
         */
        uint32_t aon_jtag_sel_soft:1;
    };
    uint32_t val;
} lp_aon_jtag_sel_reg_t;

/** Type of aon_backup_dma_cfg0 register
 *  configure regdma always on register
 */
typedef union {
    struct {
        /** aon_burst_limit_aon : R/W; bitpos: [4:0]; default: 10;
         *  Set this field to configure max value of burst in single transfer.
         */
        uint32_t aon_burst_limit_aon:5;
        /** aon_read_interval_aon : R/W; bitpos: [11:5]; default: 10;
         *  Set this field to configure read registers' interval time in reading mode.
         */
        uint32_t aon_read_interval_aon:7;
        /** aon_branch_link_length_aon : R/W; bitpos: [15:12]; default: 0;
         *  Set this field to configure link address.
         */
        uint32_t aon_branch_link_length_aon:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_aon_backup_dma_cfg0_reg_t;

/** Type of aon_backup_dma_cfg1 register
 *  configure regdma always on register
 */
typedef union {
    struct {
        /** aon_link_wait_tout_thres_aon : R/W; bitpos: [9:0]; default: 100;
         *  Set this field to configure the number of consecutive links of link list.
         */
        uint32_t aon_link_wait_tout_thres_aon:10;
        /** aon_link_work_tout_thres_aon : R/W; bitpos: [19:10]; default: 100;
         *  Set this field to configure maximum waiting time in waiting mode.
         */
        uint32_t aon_link_work_tout_thres_aon:10;
        /** aon_link_backup_tout_thres_aon : R/W; bitpos: [29:20]; default: 100;
         *  Set this field to configure maximum waiting time in backup mode.
         */
        uint32_t aon_link_backup_tout_thres_aon:10;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} lp_aon_backup_dma_cfg1_reg_t;

/** Type of aon_backup_dma_cfg2 register
 *  configure regdma always on register
 */
typedef union {
    struct {
        /** aon_link_addr_aon : R/W; bitpos: [31:0]; default: 0;
         *  Set this field to configure link address.
         */
        uint32_t aon_link_addr_aon:32;
    };
    uint32_t val;
} lp_aon_backup_dma_cfg2_reg_t;

/** Type of aon_date register
 *  need_des
 */
typedef union {
    struct {
        /** aon_date : R/W; bitpos: [30:0]; default: 37781648;
         *  need_des
         */
        uint32_t aon_date:31;
        /** aon_clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_clk_en:1;
    };
    uint32_t val;
} lp_aon_date_reg_t;


typedef struct {
    volatile lp_aon_store0_reg_t store0;
    volatile lp_aon_store1_reg_t store1;
    volatile lp_aon_store2_reg_t store2;
    volatile lp_aon_store3_reg_t store3;
    volatile lp_aon_store4_reg_t store4;
    volatile lp_aon_store5_reg_t store5;
    volatile lp_aon_store6_reg_t store6;
    volatile lp_aon_store7_reg_t store7;
    volatile lp_aon_store8_reg_t store8;
    volatile lp_aon_store9_reg_t store9;
    volatile lp_aon_gpio_mux_reg_t gpio_mux;
    volatile lp_aon_gpio_hold0_reg_t gpio_hold0;
    volatile lp_aon_gpio_hold1_reg_t gpio_hold1;
    volatile lp_aon_sys_cfg_reg_t sys_cfg;
    volatile lp_aon_cpucore0_cfg_reg_t cpucore0_cfg;
    volatile lp_aon_io_mux_reg_t io_mux;
    volatile lp_aon_ext_wakeup_cntl_reg_t ext_wakeup_cntl;
    volatile lp_aon_usb_reg_t usb;
    uint32_t reserved_048;
    volatile lp_aon_sdio_active_reg_t sdio_active;
    volatile lp_aon_lpcore_reg_t lpcore;
    volatile lp_aon_sar_cct_reg_t sar_cct;
    volatile lp_aon_jtag_sel_reg_t _jtag_sel;
    uint32_t reserved_05c[5];
    volatile lp_aon_backup_dma_cfg0_reg_t backup_dma_cfg0;
    volatile lp_aon_backup_dma_cfg1_reg_t backup_dma_cfg1;
    volatile lp_aon_backup_dma_cfg2_reg_t backup_dma_cfg2;
    uint32_t reserved_07c[224];
    volatile lp_aon_date_reg_t date;
} lp_aon_dev_t;

extern lp_aon_dev_t LP_AON;

#ifndef __cplusplus
_Static_assert(sizeof(lp_aon_dev_t) == 0x400, "Invalid size of lp_aon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
