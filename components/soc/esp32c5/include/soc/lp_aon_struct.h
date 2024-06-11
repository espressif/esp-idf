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
/** Type of store0 register
 *  store the software massege0 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store0 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege0 in always-on field
         */
        uint32_t lp_aon_store0:32;
    };
    uint32_t val;
} lp_aon_store0_reg_t;

/** Type of store1 register
 *  store the software massege1 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store1 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege1 in always-on field
         */
        uint32_t lp_aon_store1:32;
    };
    uint32_t val;
} lp_aon_store1_reg_t;

/** Type of store2 register
 *  store the software massege2 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store2 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege2 in always-on field
         */
        uint32_t lp_aon_store2:32;
    };
    uint32_t val;
} lp_aon_store2_reg_t;

/** Type of store3 register
 *  store the software massege3 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store3 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege3 in always-on field
         */
        uint32_t lp_aon_store3:32;
    };
    uint32_t val;
} lp_aon_store3_reg_t;

/** Type of store4 register
 *  store the software massege4 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store4 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege4 in always-on field
         */
        uint32_t lp_aon_store4:32;
    };
    uint32_t val;
} lp_aon_store4_reg_t;

/** Type of store5 register
 *  store the software massege5 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store5 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege5 in always-on field
         */
        uint32_t lp_aon_store5:32;
    };
    uint32_t val;
} lp_aon_store5_reg_t;

/** Type of store6 register
 *  store the software massege6 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store6 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege6 in always-on field
         */
        uint32_t lp_aon_store6:32;
    };
    uint32_t val;
} lp_aon_store6_reg_t;

/** Type of store7 register
 *  store the software massege7 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store7 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege7 in always-on field
         */
        uint32_t lp_aon_store7:32;
    };
    uint32_t val;
} lp_aon_store7_reg_t;

/** Type of store8 register
 *  store the software massege8 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store8 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege8 in always-on field
         */
        uint32_t lp_aon_store8:32;
    };
    uint32_t val;
} lp_aon_store8_reg_t;

/** Type of store9 register
 *  store the software massege9 in always-on field
 */
typedef union {
    struct {
        /** lp_aon_store9 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege9 in always-on field
         */
        uint32_t lp_aon_store9:32;
    };
    uint32_t val;
} lp_aon_store9_reg_t;

/** Type of gpio_mux register
 *  select the lp io controlled by hp iomux or lp iomux
 */
typedef union {
    struct {
        /** gpio_mux_sel : R/W; bitpos: [7:0]; default: 0;
         *  select the lp io 0~7 controlled by hp iomux or lp iomux
         *  1: controlled by lp iomux
         *  0: controlled by hp iomux
         */
        uint32_t gpio_mux_sel:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_aon_gpio_mux_reg_t;

/** Type of gpio_hold0 register
 *  configure all io hold
 */
typedef union {
    struct {
        /** gpio_hold0 : R/W; bitpos: [31:0]; default: 0;
         *  configure io0~28 hold enable,when io in hold status, all io configure and output
         *  will be latch , input function is useful
         */
        uint32_t gpio_hold0:32;
    };
    uint32_t val;
} lp_aon_gpio_hold0_reg_t;

/** Type of gpio_hold1 register
 *  reserved
 */
typedef union {
    struct {
        /** gpio_hold1 : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t gpio_hold1:32;
    };
    uint32_t val;
} lp_aon_gpio_hold1_reg_t;

/** Type of sys_cfg register
 *  configure system register
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** force_download_boot_status : RO; bitpos: [29]; default: 0;
         *  get force download mode status
         */
        uint32_t force_download_boot_status:1;
        /** force_download_boot : R/W; bitpos: [30]; default: 0;
         *  enable chip entry download mode or not
         *  1: enable
         *  0: no operation
         */
        uint32_t force_download_boot:1;
        /** hpsys_sw_reset : WT; bitpos: [31]; default: 0;
         *  enable hp system reset by software or not
         *  1: reset
         *  0: no operation
         */
        uint32_t hpsys_sw_reset:1;
    };
    uint32_t val;
} lp_aon_sys_cfg_reg_t;

/** Type of cpucore0_cfg register
 *  configure core reset register
 */
typedef union {
    struct {
        /** cpu_core0_sw_stall : R/W; bitpos: [7:0]; default: 0;
         *  enable cpu entry stall status
         *  0x86: entry stall status
         *  Others : no operation
         */
        uint32_t cpu_core0_sw_stall:8;
        uint32_t reserved_8:20;
        /** cpu_core0_sw_reset : WT; bitpos: [28]; default: 0;
         *  enable core reset by software
         *  1: reset
         *  0: no operation
         */
        uint32_t cpu_core0_sw_reset:1;
        /** cpu_core0_ocd_halt_on_reset : R/W; bitpos: [29]; default: 0;
         *  reserved
         */
        uint32_t cpu_core0_ocd_halt_on_reset:1;
        /** cpu_core0_stat_vector_sel : R/W; bitpos: [30]; default: 1;
         *  configure core boot address
         *  1: ROM
         *  0: lp memory
         */
        uint32_t cpu_core0_stat_vector_sel:1;
        /** cpu_core0_dreset_mask : R/W; bitpos: [31]; default: 0;
         *  disable bypass core dreset
         *  1: enable bypass
         *  0: disable bypass
         */
        uint32_t cpu_core0_dreset_mask:1;
    };
    uint32_t val;
} lp_aon_cpucore0_cfg_reg_t;

/** Type of io_mux register
 *  configure hp iomux reset bypass
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** io_mux_reset_disable : R/W; bitpos: [31]; default: 0;
         *  bypass hp iomux reset from hp system reset event
         *  1: bypass
         *  0: no operation
         */
        uint32_t io_mux_reset_disable:1;
    };
    uint32_t val;
} lp_aon_io_mux_reg_t;

/** Type of ext_wakeup_cntl register
 *  configure alwayson external io wakeup
 */
typedef union {
    struct {
        /** ext_wakeup_status : RO; bitpos: [7:0]; default: 0;
         *  get external wakeup status bitmap
         */
        uint32_t ext_wakeup_status:8;
        uint32_t reserved_8:6;
        /** ext_wakeup_status_clr : WT; bitpos: [14]; default: 0;
         *  clear external wakeup status
         *  1: clear
         *  0: no operation
         */
        uint32_t ext_wakeup_status_clr:1;
        /** ext_wakeup_sel : R/W; bitpos: [22:15]; default: 0;
         *  enable io0~7 bit map use to external wakeup
         *  1: enable
         *  0: disable
         */
        uint32_t ext_wakeup_sel:8;
        /** ext_wakeup_lv : R/W; bitpos: [30:23]; default: 0;
         *  select external wakeup io level
         *  1: io high level wakeup
         *  0: io low level wakeup
         */
        uint32_t ext_wakeup_lv:8;
        /** ext_wakeup_filter : R/W; bitpos: [31]; default: 0;
         *  enable external filter or not
         *  1: enable
         *  0: disable
         */
        uint32_t ext_wakeup_filter:1;
    };
    uint32_t val;
} lp_aon_ext_wakeup_cntl_reg_t;

/** Type of usb register
 *  configure usb reset bypass
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** usb_reset_disable : R/W; bitpos: [31]; default: 0;
         *  bypass usb reset from hp system reset event
         *  1: bypass
         *  0: no operation
         */
        uint32_t usb_reset_disable:1;
    };
    uint32_t val;
} lp_aon_usb_reg_t;

/** Type of lpbus register
 *  Select lp memory bus
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** fast_mem_mux_fsm_idle : RO; bitpos: [28]; default: 1;
         *  get current lp memory bus fsm status
         */
        uint32_t fast_mem_mux_fsm_idle:1;
        /** fast_mem_mux_sel_status : RO; bitpos: [29]; default: 1;
         *  get current lp memory bus mode
         */
        uint32_t fast_mem_mux_sel_status:1;
        /** fast_mem_mux_sel_update : WT; bitpos: [30]; default: 0;
         *  enable reg_fast_mem_sel configure
         *  1: enable
         *  0: no operation
         */
        uint32_t fast_mem_mux_sel_update:1;
        /** fast_mem_mux_sel : R/W; bitpos: [31]; default: 1;
         *  select lp memory bus is high speed mode or low speed mode
         *  1: high speed from hp system ahb
         *  0: low speed from lp system
         */
        uint32_t fast_mem_mux_sel:1;
    };
    uint32_t val;
} lp_aon_lpbus_reg_t;

/** Type of sdio_active register
 *  configure sdio act dnum
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** sdio_act_dnum : R/W; bitpos: [31:22]; default: 10;
         *  reserved
         */
        uint32_t sdio_act_dnum:10;
    };
    uint32_t val;
} lp_aon_sdio_active_reg_t;

/** Type of lpcore register
 *  configure etm wakeup register
 */
typedef union {
    struct {
        /** lpcore_etm_wakeup_flag_clr : WT; bitpos: [0]; default: 0;
         *  clear etm wakeup latch
         */
        uint32_t lpcore_etm_wakeup_flag_clr:1;
        /** lpcore_etm_wakeup_flag : R/WTC/SS; bitpos: [1]; default: 0;
         *  latch etmwakeup event
         */
        uint32_t lpcore_etm_wakeup_flag:1;
        uint32_t reserved_2:29;
        /** lpcore_disable : R/W; bitpos: [31]; default: 0;
         *  disable lp core
         *  1:disable
         *  0:no operation
         */
        uint32_t lpcore_disable:1;
    };
    uint32_t val;
} lp_aon_lpcore_reg_t;

/** Type of sar_cct register
 *  configure sar cct
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** sar2_pwdet_cct : R/W; bitpos: [31:29]; default: 0;
         *  configure sar cct
         */
        uint32_t sar2_pwdet_cct:3;
    };
    uint32_t val;
} lp_aon_sar_cct_reg_t;

/** Type of modem_bus register
 *  configure modem sync bridge
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** modem_sync_bridge_en : R/W; bitpos: [31]; default: 0;
         *  enable modem sync bridge or not
         *  1: enable
         *  0: disable
         */
        uint32_t modem_sync_bridge_en:1;
    };
    uint32_t val;
} lp_aon_modem_bus_reg_t;

/** Type of debug_sel0 register
 *  reserved
 */
typedef union {
    struct {
        /** lp_debug_sel0 : R/W; bitpos: [6:0]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel0:7;
        /** lp_debug_sel1 : R/W; bitpos: [13:7]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel1:7;
        /** lp_debug_sel2 : R/W; bitpos: [20:14]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel2:7;
        /** lp_debug_sel3 : R/W; bitpos: [27:21]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel3:7;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_aon_debug_sel0_reg_t;

/** Type of debug_sel1 register
 *  need des
 */
typedef union {
    struct {
        /** lp_debug_sel4 : R/W; bitpos: [6:0]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel4:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_aon_debug_sel1_reg_t;

/** Type of backup_dma_cfg0 register
 *  configure regdma always on register
 */
typedef union {
    struct {
        /** burst_limit_aon : R/W; bitpos: [4:0]; default: 10;
         *  Set this field to configure max value of burst in single transfer.
         */
        uint32_t burst_limit_aon:5;
        /** read_interval_aon : R/W; bitpos: [11:5]; default: 10;
         *  Set this field to configure read registers' interval time in reading mode.
         */
        uint32_t read_interval_aon:7;
        /** branch_link_length_aon : R/W; bitpos: [15:12]; default: 0;
         *  Set this field to configure link address.
         */
        uint32_t branch_link_length_aon:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_aon_backup_dma_cfg0_reg_t;

/** Type of backup_dma_cfg1 register
 *  configure regdma always on register
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
        /** link_backup_tout_thres_aon : R/W; bitpos: [29:20]; default: 100;
         *  Set this field to configure maximum waiting time in backup mode.
         */
        uint32_t link_backup_tout_thres_aon:10;
        uint32_t reserved_30:1;
        /** aon_bypass : R/W; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t aon_bypass:1;
    };
    uint32_t val;
} lp_aon_backup_dma_cfg1_reg_t;

/** Type of backup_dma_cfg2 register
 *  configure regdma always on register
 */
typedef union {
    struct {
        /** link_addr_aon : R/W; bitpos: [31:0]; default: 0;
         *  Set this field to configure link address.
         */
        uint32_t link_addr_aon:32;
    };
    uint32_t val;
} lp_aon_backup_dma_cfg2_reg_t;

/** Type of mem_ctrl register
 *  configure rmemory power in lp system register
 */
typedef union {
    struct {
        /** lp_mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  force off lp memory
         */
        uint32_t lp_mem_force_pd:1;
        /** lp_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  force on lp memory
         */
        uint32_t lp_mem_force_pu:1;
        /** huk_mem_force_pd : R/W; bitpos: [2]; default: 1;
         *  force off huk memory
         */
        uint32_t huk_mem_force_pd:1;
        /** huk_mem_force_pu : R/W; bitpos: [3]; default: 0;
         *  force on huk memory
         */
        uint32_t huk_mem_force_pu:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_aon_mem_ctrl_reg_t;

/** Type of date register
 *  reserved
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 36774512;
         *  version register
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  version register
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_aon_date_reg_t;


/** Group: Configuration Register */
/** Type of spram_ctrl register
 *  configure lp memory power status
 */
typedef union {
    struct {
        /** spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  configure lp memory power status
         */
        uint32_t spram_mem_aux_ctrl:32;
    };
    uint32_t val;
} lp_aon_spram_ctrl_reg_t;

/** Type of sprf_ctrl register
 *  configure memory in lp system power status
 */
typedef union {
    struct {
        /** sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  configure memory in lp system power status
         */
        uint32_t sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} lp_aon_sprf_ctrl_reg_t;


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
    volatile lp_aon_lpbus_reg_t lpbus;
    volatile lp_aon_sdio_active_reg_t sdio_active;
    volatile lp_aon_lpcore_reg_t lpcore;
    volatile lp_aon_sar_cct_reg_t sar_cct;
    volatile lp_aon_modem_bus_reg_t modem_bus;
    uint32_t reserved_05c;
    volatile lp_aon_spram_ctrl_reg_t spram_ctrl;
    volatile lp_aon_sprf_ctrl_reg_t sprf_ctrl;
    volatile lp_aon_debug_sel0_reg_t debug_sel0;
    volatile lp_aon_debug_sel1_reg_t debug_sel1;
    volatile lp_aon_backup_dma_cfg0_reg_t backup_dma_cfg0;
    volatile lp_aon_backup_dma_cfg1_reg_t backup_dma_cfg1;
    volatile lp_aon_backup_dma_cfg2_reg_t backup_dma_cfg2;
    volatile lp_aon_mem_ctrl_reg_t mem_ctrl;
    uint32_t reserved_080[223];
    volatile lp_aon_date_reg_t date;
} lp_aon_dev_t;

extern lp_aon_dev_t LP_AON;

#ifndef __cplusplus
_Static_assert(sizeof(lp_aon_dev_t) == 0x400, "Invalid size of lp_aon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
