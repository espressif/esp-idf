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
/** Type of aon_store0 register
 *  store the software massege0 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store0 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege0 in always-on field
         */
        uint32_t aon_lp_aon_store0:32;
    };
    uint32_t val;
} lp_aon_store0_reg_t;

/** Type of aon_store1 register
 *  store the software massege1 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store1 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege1 in always-on field
         */
        uint32_t aon_lp_aon_store1:32;
    };
    uint32_t val;
} lp_aon_store1_reg_t;

/** Type of aon_store2 register
 *  store the software massege2 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store2 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege2 in always-on field
         */
        uint32_t aon_lp_aon_store2:32;
    };
    uint32_t val;
} lp_aon_store2_reg_t;

/** Type of aon_store3 register
 *  store the software massege3 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store3 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege3 in always-on field
         */
        uint32_t aon_lp_aon_store3:32;
    };
    uint32_t val;
} lp_aon_store3_reg_t;

/** Type of aon_store4 register
 *  store the software massege4 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store4 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege4 in always-on field
         */
        uint32_t aon_lp_aon_store4:32;
    };
    uint32_t val;
} lp_aon_store4_reg_t;

/** Type of aon_store5 register
 *  store the software massege5 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store5 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege5 in always-on field
         */
        uint32_t aon_lp_aon_store5:32;
    };
    uint32_t val;
} lp_aon_store5_reg_t;

/** Type of aon_store6 register
 *  store the software massege6 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store6 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege6 in always-on field
         */
        uint32_t aon_lp_aon_store6:32;
    };
    uint32_t val;
} lp_aon_store6_reg_t;

/** Type of aon_store7 register
 *  store the software massege7 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store7 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege7 in always-on field
         */
        uint32_t aon_lp_aon_store7:32;
    };
    uint32_t val;
} lp_aon_store7_reg_t;

/** Type of aon_store8 register
 *  store the software massege8 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store8 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege8 in always-on field
         */
        uint32_t aon_lp_aon_store8:32;
    };
    uint32_t val;
} lp_aon_store8_reg_t;

/** Type of aon_store9 register
 *  store the software massege9 in always-on field
 */
typedef union {
    struct {
        /** aon_lp_aon_store9 : R/W; bitpos: [31:0]; default: 0;
         *  store the software massege9 in always-on field
         */
        uint32_t aon_lp_aon_store9:32;
    };
    uint32_t val;
} lp_aon_store9_reg_t;

/** Type of aon_gpio_mux register
 *  select the lp io controlled by hp iomux or lp iomux
 */
typedef union {
    struct {
        /** aon_gpio_mux_sel : R/W; bitpos: [5:0]; default: 0;
         *  select the lp io 0~5 controlled by hp iomux or lp iomux
         *  1: controlled by lp iomux
         *  0: controlled by hp iomux
         */
        uint32_t gpio_mux_sel:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_aon_gpio_mux_reg_t;

/** Type of aon_gpio_hold0 register
 *  configure all io hold
 */
typedef union {
    struct {
        /** aon_gpio_hold0 : R/W; bitpos: [31:0]; default: 0;
         *  configure io0~31 hold enable,when io in hold status, all io configure and output
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
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t gpio_hold1:32;
    };
    uint32_t val;
} lp_aon_gpio_hold1_reg_t;

/** Type of aon_sys_cfg register
 *  configure system register
 */
typedef union {
    struct {
        /** aon_fib_reg : RO; bitpos: [7:0]; default: 255;
         *  get fib reg information
         */
        uint32_t aon_fib_reg:8;
        uint32_t reserved_8:23;
        /** aon_hpsys_sw_reset : WT; bitpos: [31]; default: 0;
         *  enable hp system reset by software or not
         *  1: reset
         *  0: no operation
         */
        uint32_t aon_hpsys_sw_reset:1;
    };
    uint32_t val;
} lp_aon_sys_cfg_reg_t;

/** Type of aon_cpucore_cfg register
 *  configure core reset register
 */
typedef union {
    struct {
        /** aon_cpu_core0_sw_stall : R/W; bitpos: [7:0]; default: 0;
         *  enable cpu 0 entry stall status
         *  0x86: entry stall status
         *  Others : no operation
         */
        uint32_t aon_cpu_core0_sw_stall:8;
        /** aon_cpu_core0_sw_reset : WT; bitpos: [8]; default: 0;
         *  enable core 0 reset by software
         *  1: reset
         *  0: no operation
         */
        uint32_t aon_cpu_core0_sw_reset:1;
        uint32_t reserved_9:7;
        /** aon_cpu_core1_sw_stall : R/W; bitpos: [23:16]; default: 0;
         *  enable core 1 entry stall status
         *  0x86: entry stall status
         *  Others : no operation
         */
        uint32_t aon_cpu_core1_sw_stall:8;
        /** aon_cpu_core1_sw_reset : WT; bitpos: [24]; default: 0;
         *  enable core1 reset by software
         *  1: reset
         *  0: no operation
         */
        uint32_t aon_cpu_core1_sw_reset:1;
        uint32_t reserved_25:6;
        /** aon_systimer_stall_sel : R/W; bitpos: [31]; default: 0;
         *  delete which core run_stall to lp_timer
         *  1: core1
         *  0: core0
         */
        uint32_t aon_systimer_stall_sel:1;
    };
    uint32_t val;
} lp_aon_cpucore_cfg_reg_t;

/** Type of aon_io_mux register
 *  configure hp iomux reset bypass
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** aon_io_mux_pull_ldo : R/W; bitpos: [30:28]; default: 0;
         *  need_des
         */
        uint32_t aon_io_mux_pull_ldo:3;
        /** aon_io_mux_reset_disable : R/W; bitpos: [31]; default: 0;
         *  bypass hp iomux reset from hp system reset event
         *  1: bypass
         *  0: no operation
         */
        uint32_t aon_io_mux_reset_disable:1;
    };
    uint32_t val;
} lp_aon_io_mux_reg_t;

/** Type of aon_ext_wakeup_cntl register
 *  configure alwayson external io wakeup
 */
typedef union {
    struct {
        /** aon_ext_wakeup_sel : R/W; bitpos: [15:0]; default: 0;
         *  enable io0~15 bit map use to external wakeup
         *  1: enable
         *  0: disable
         */
        uint32_t aon_ext_wakeup_sel:16;
        /** aon_ext_wakeup_lv : R/W; bitpos: [31:16]; default: 0;
         *  select external wakeup io level
         *  1: io high level wakeup
         *  0: io low level wakeup
         */
        uint32_t aon_ext_wakeup_lv:16;
    };
    uint32_t val;
} lp_aon_ext_wakeup_cntl_reg_t;

/** Type of aon_ext_wakeup_cntl1 register
 *  configure alwayson external io wakeup
 */
typedef union {
    struct {
        /** aon_ext_wakeup_status : RO; bitpos: [15:0]; default: 0;
         *  get external wakeup status bitmap
         */
        uint32_t aon_ext_wakeup_status:16;
        uint32_t reserved_16:14;
        /** aon_ext_wakeup_status_clr : WT; bitpos: [30]; default: 0;
         *  clear external wakeup status
         *  1: clear
         *  0: no operation
         */
        uint32_t aon_ext_wakeup_status_clr:1;
        /** aon_ext_wakeup_filter : R/W; bitpos: [31]; default: 0;
         *  enable external filter or not
         *  1: enable
         *  0: disable
         */
        uint32_t aon_ext_wakeup_filter:1;
    };
    uint32_t val;
} lp_aon_ext_wakeup_cntl1_reg_t;

/** Type of aon_usb register
 *  configure usb reset bypass
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** aon_usb_reset_disable : R/W; bitpos: [31]; default: 0;
         *  bypass usb reset from hp system reset event
         *  1: bypass
         *  0: no operation
         */
        uint32_t aon_usb_reset_disable:1;
    };
    uint32_t val;
} lp_aon_usb_reg_t;

/** Type of aon_lpbus register
 *  Select lp memory bus
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** aon_fast_mem_mux_fsm_idle : RO; bitpos: [28]; default: 1;
         *  get current lp memory bus fsm status
         */
        uint32_t aon_fast_mem_mux_fsm_idle:1;
        /** aon_fast_mem_mux_sel_status : RO; bitpos: [29]; default: 1;
         *  get current lp memory bus mode
         */
        uint32_t aon_fast_mem_mux_sel_status:1;
        /** aon_fast_mem_mux_sel_update : WT; bitpos: [30]; default: 0;
         *  enable reg_fast_mem_sel configure
         *  1: enable
         *  0: no operation
         */
        uint32_t aon_fast_mem_mux_sel_update:1;
        /** aon_fast_mem_mux_sel : R/W; bitpos: [31]; default: 1;
         *  select lp memory bus is high speed mode or low speed mode
         *  1: high speed from hp system ahb
         *  0: low speed from lp system
         */
        uint32_t aon_fast_mem_mux_sel:1;
    };
    uint32_t val;
} lp_aon_lpbus_reg_t;

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
        /** aon_regdma_error : RO; bitpos: [18:16]; default: 0;
         *  regdma error code
         */
        uint32_t aon_regdma_error:3;
        uint32_t reserved_19:13;
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

/** Type of aon_mem_ctrl register
 *  configure rmemory power in lp system register
 */
typedef union {
    struct {
        /** aon_efuse_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures efuse memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t aon_efuse_mem_lp_mode:2;
        /** aon_efuse_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down efuse memory.
         */
        uint32_t aon_efuse_mem_lp_en:1;
        /** aon_efuse_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control efuse memory, disable hardware control.
         */
        uint32_t aon_efuse_mem_force_ctrl:1;
        /** aon_huk_mem_lp_mode : R/W; bitpos: [5:4]; default: 2;
         *  Configures huk memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t aon_huk_mem_lp_mode:2;
        /** aon_huk_mem_lp_en : R/W; bitpos: [6]; default: 0;
         *  Set this bit to power down huk memory.
         */
        uint32_t aon_huk_mem_lp_en:1;
        /** aon_huk_mem_force_ctrl : R/W; bitpos: [7]; default: 0;
         *  Set this bit to force software control huk memory, disable hardware control.
         */
        uint32_t aon_huk_mem_force_ctrl:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_aon_mem_ctrl_reg_t;

/** Type of aon_hp_mem_ctrl register
 *  configure rmemory power in lp system register
 */
typedef union {
    struct {
        /** aon_modem_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures modem memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t aon_modem_mem_lp_mode:2;
        /** aon_modem_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down modem memory.
         */
        uint32_t aon_modem_mem_lp_en:1;
        /** aon_modem_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control modem memory, disable hardware control.
         */
        uint32_t aon_modem_mem_force_ctrl:1;
        /** aon_mmu_mem_lp_mode : R/W; bitpos: [5:4]; default: 0;
         *  Configures mmu memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t aon_mmu_mem_lp_mode:2;
        /** aon_mmu_mem_lp_en : R/W; bitpos: [6]; default: 0;
         *  Set this bit to power down mmu memory.
         */
        uint32_t aon_mmu_mem_lp_en:1;
        /** aon_mmu_mem_force_ctrl : R/W; bitpos: [7]; default: 0;
         *  Set this bit to force software control mmu memory, disable hardware control.
         */
        uint32_t aon_mmu_mem_force_ctrl:1;
        uint32_t reserved_8:8;
        /** aon_hp_sram_mem_lp_mode : R/W; bitpos: [17:16]; default: 0;
         *  Configures hp_sram memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t aon_hp_sram_mem_lp_mode:2;
        /** aon_hp_sram_mem_lp_en : R/W; bitpos: [24:18]; default: 0;
         *  Set this bit to power down hp_sram memory.
         */
        uint32_t aon_hp_sram_mem_lp_en:7;
        /** aon_hp_sram_mem_force_ctrl : R/W; bitpos: [31:25]; default: 0;
         *  Set this bit to force software control hp_sram memory, disable hardware control.
         */
        uint32_t aon_hp_sram_mem_force_ctrl:7;
    };
    uint32_t val;
} lp_aon_hp_mem_ctrl_reg_t;

/** Type of aon_io_ldo_cfg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** aon_io_ldo_3p3_sw : R/W; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t aon_io_ldo_3p3_sw:1;
        /** aon_io_ldo_3p3_sw_en : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t aon_io_ldo_3p3_sw_en:1;
        /** aon_io_ldo_adjust_sw : R/W; bitpos: [30:23]; default: 0;
         *  need_des
         */
        uint32_t aon_io_ldo_adjust_sw:8;
        /** aon_io_ldo_adjust_sw_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t aon_io_ldo_adjust_sw_en:1;
    };
    uint32_t val;
} lp_aon_io_ldo_cfg_reg_t;

/** Type of aon_lp_gpio_security register
 *  need des
 */
typedef union {
    struct {
        /** aon_lp_gpio_lock : R/W; bitpos: [5:0]; default: 0;
         *  This field decides whether lp_gpio_config can be locked, or not. 0 (default):
         *  unlocked. 1: locked.
         */
        uint32_t aon_lp_gpio_lock:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_aon_lp_gpio_security_reg_t;

/** Type of aon_hp_gpio_security_1 register
 *  need des
 */
typedef union {
    struct {
        /** aon_hp_gpio_lock_p1 : R/W; bitpos: [31:0]; default: 0;
         *  This field decides whether hp_gpio_config of PIN0~31 can be locked, or not. 0
         *  (default): unlocked. 1: locked.
         */
        uint32_t aon_hp_gpio_lock_p1:32;
    };
    uint32_t val;
} lp_aon_hp_gpio_security_1_reg_t;

/** Type of aon_hp_gpio_security_2 register
 *  need des
 */
typedef union {
    struct {
        /** aon_hp_gpio_lock_p2 : R/W; bitpos: [7:0]; default: 0;
         *  This field decides whether hp_gpio_config of PIN32~39 can be locked, or not. 0
         *  (default): unlocked. 1: locked.
         */
        uint32_t aon_hp_gpio_lock_p2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_aon_hp_gpio_security_2_reg_t;

/** Type of aon_sram_usage_conf register
 *  HP memory usage configuration register
 */
typedef union {
    struct {
        /** aon_dcache_usage : R/W; bitpos: [0]; default: 0;
         *  hp system memory is split to 7 layers(Layer0 ~ Layer6) in total, this field is
         *  used to control the first layer(Layer0) usage. 0: cpu use hp-memory. 1: dcache use
         *  hp-mmory.
         *  By default, dcache is closed, and typically users can enable dcache after
         *  boot-loader, but before user's BIN start running.
         */
        uint32_t aon_dcache_usage:1;
        /** aon_icache1_usage : R/W; bitpos: [1]; default: 1;
         *  hp system memory is split to 7 layers(Layer0 ~ Layer6) in total, this field is
         *  used to control the last layer(Layer6) usage. 0: cpu use hp-memory. 1: icache1 use
         *  hp-mmory.
         *  By default, icache1 is not disabled, and the last layer memory belongs to icache1.
         *  Typically users can set this bit to 0 to disable icache1 in boot-loader.
         */
        uint32_t aon_icache1_usage:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_aon_sram_usage_conf_reg_t;

/** Type of aon_date register
 *  reserved
 */
typedef union {
    struct {
        /** aon_date : R/W; bitpos: [30:0]; default: 37823056;
         *  version register
         */
        uint32_t aon_date:31;
        /** aon_clk_en : R/W; bitpos: [31]; default: 0;
         *  version register
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
    volatile lp_aon_cpucore_cfg_reg_t cpucore_cfg;
    volatile lp_aon_io_mux_reg_t io_mux;
    volatile lp_aon_ext_wakeup_cntl_reg_t ext_wakeup_cntl;
    volatile lp_aon_ext_wakeup_cntl1_reg_t ext_wakeup_cntl1;
    volatile lp_aon_usb_reg_t usb;
    volatile lp_aon_lpbus_reg_t lpbus;
    uint32_t reserved_050[8];
    volatile lp_aon_backup_dma_cfg0_reg_t backup_dma_cfg0;
    volatile lp_aon_backup_dma_cfg1_reg_t backup_dma_cfg1;
    volatile lp_aon_backup_dma_cfg2_reg_t backup_dma_cfg2;
    volatile lp_aon_mem_ctrl_reg_t mem_ctrl;
    volatile lp_aon_hp_mem_ctrl_reg_t hp_mem_ctrl;
    volatile lp_aon_io_ldo_cfg_reg_t io_ldo_cfg;
    uint32_t reserved_088;
    volatile lp_aon_lp_gpio_security_reg_t lp_gpio_security;
    volatile lp_aon_hp_gpio_security_1_reg_t hp_gpio_security_1;
    volatile lp_aon_hp_gpio_security_2_reg_t hp_gpio_security_2;
    volatile lp_aon_sram_usage_conf_reg_t sram_usage_conf;
    uint32_t reserved_09c[216];
    volatile lp_aon_date_reg_t date;
} lp_aon_dev_t;

extern lp_aon_dev_t LP_AON;

#ifndef __cplusplus
_Static_assert(sizeof(lp_aon_dev_t) == 0x400, "Invalid size of lp_aon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
