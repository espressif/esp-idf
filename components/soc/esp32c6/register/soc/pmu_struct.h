/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "soc/soc.h"
#include "soc/pmu_reg.h"

typedef union {
    struct {
        uint32_t reserved0    : 21;
        uint32_t vdd_spi_pd_en: 1;
        uint32_t mem_dslp     : 1;
        uint32_t mem_pd_en    : 4;
        uint32_t wifi_pd_en   : 1;
        uint32_t reserved1    : 1;
        uint32_t cpu_pd_en    : 1;
        uint32_t aon_pd_en    : 1;
        uint32_t top_pd_en    : 1;
    };
    uint32_t val;
} pmu_hp_dig_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 30;
        uint32_t code     : 2;
    };
    uint32_t val;
} pmu_hp_icg_modem_reg_t;

typedef union {
    struct {
        uint32_t reserved0      : 24;
        uint32_t uart_wakeup_en : 1;
        uint32_t lp_pad_hold_all: 1;
        uint32_t hp_pad_hold_all: 1;
        uint32_t dig_pad_slp_sel: 1;
        uint32_t dig_pause_wdt  : 1;
        uint32_t dig_cpu_stall  : 1;
        uint32_t reserved1      : 2;
    };
    uint32_t val;
} pmu_hp_sys_cntl_reg_t;

typedef union {
    struct {
        uint32_t reserved0    : 26;
        uint32_t i2c_iso_en   : 1;
        uint32_t i2c_retention: 1;
        uint32_t xpd_bb_i2c   : 1;
        uint32_t xpd_bbpll_i2c: 1;
        uint32_t xpd_bbpll    : 1;
        uint32_t reserved1    : 1;
    };
    uint32_t val;
} pmu_hp_clk_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0 : 25;
        uint32_t xpd_bias  : 1;
        uint32_t dbg_atten : 4;
        uint32_t pd_cur    : 1;
        uint32_t bias_sleep: 1;
    };
    uint32_t val;
} pmu_hp_bias_reg_t;

typedef union {
    struct {    /* HP: Active State */
        uint32_t reserved0                            : 4;
        uint32_t hp_sleep2active_backup_modem_clk_code: 2;
        uint32_t hp_modem2active_backup_modem_clk_code: 2;
        uint32_t reserved1                            : 2;
        uint32_t hp_active_retention_mode             : 1;
        uint32_t hp_sleep2active_retention_en         : 1;
        uint32_t hp_modem2active_retention_en         : 1;
        uint32_t reserved2                            : 1;
        uint32_t hp_sleep2active_backup_clk_sel       : 2;
        uint32_t hp_modem2active_backup_clk_sel       : 2;
        uint32_t reserved3                            : 2;
        uint32_t hp_sleep2active_backup_mode          : 3;
        uint32_t hp_modem2active_backup_mode          : 3;
        uint32_t reserved4                            : 3;
        uint32_t hp_sleep2active_backup_en            : 1;
        uint32_t hp_modem2active_backup_en            : 1;
        uint32_t reserved5                            : 1;
    };
    struct {    /* HP: Modem State */
        uint32_t reserved6                            : 4;
        uint32_t hp_sleep2modem_backup_modem_clk_code : 2;
        uint32_t reserved7                            : 4;
        uint32_t hp_modem_retention_mode              : 1;
        uint32_t hp_sleep2modem_retention_en          : 1;
        uint32_t reserved8                            : 2;
        uint32_t hp_sleep2modem_backup_clk_sel        : 2;
        uint32_t reserved9                            : 4;
        uint32_t hp_sleep2modem_backup_mode           : 3;
        uint32_t reserved10                           : 6;
        uint32_t hp_sleep2modem_backup_en             : 1;
        uint32_t reserved11                           : 2;
    };
    struct {    /* HP: Sleep State */
        uint32_t reserved12                           : 6;
        uint32_t hp_modem2sleep_backup_modem_clk_code : 2;
        uint32_t hp_active2sleep_backup_modem_clk_code: 2;
        uint32_t hp_sleep_retention_mode              : 1;
        uint32_t reserved13                           : 1;
        uint32_t hp_modem2sleep_retention_en          : 1;
        uint32_t hp_active2sleep_retention_en         : 1;
        uint32_t reserved14                           : 2;
        uint32_t hp_modem2sleep_backup_clk_sel        : 2;
        uint32_t hp_active2sleep_backup_clk_sel       : 2;
        uint32_t reserved15                           : 3;
        uint32_t hp_modem2sleep_backup_mode           : 3;
        uint32_t hp_active2sleep_backup_mode          : 3;
        uint32_t reserved16                           : 1;
        uint32_t hp_modem2sleep_backup_en             : 1;
        uint32_t hp_active2sleep_backup_en            : 1;
    };
    uint32_t val;
} pmu_hp_backup_reg_t;

typedef union {
    struct {
        uint32_t reserved0       : 26;
        uint32_t dig_sysclk_nodiv: 1;
        uint32_t icg_sysclk_en   : 1;
        uint32_t sysclk_slp_sel  : 1;
        uint32_t icg_slp_sel     : 1;
        uint32_t dig_sysclk_sel  : 2;
    };
    uint32_t val;
} pmu_hp_sysclk_reg_t;

typedef union {
    struct {
        uint32_t reserved0      : 4;    /* Only HP_ACTIVE modem under hp system is valid */
        uint32_t lp_dbias_vol   : 5;    /* Only HP_ACTIVE modem under hp system is valid */
        uint32_t hp_dbias_vol   : 5;    /* Only HP_ACTIVE modem under hp system is valid */
        uint32_t dbias_sel      : 1;    /* Only HP_ACTIVE modem under hp system is valid */
        uint32_t dbias_init     : 1;    /* Only HP_ACTIVE modem under hp system is valid */
        uint32_t slp_mem_xpd    : 1;
        uint32_t slp_logic_xpd  : 1;
        uint32_t xpd            : 1;
        uint32_t slp_mem_dbias  : 4;
        uint32_t slp_logic_dbias: 4;
        uint32_t dbias          : 5;
    };
    uint32_t val;
} pmu_hp_regulator0_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 8;
        uint32_t drv_b    : 24;
    };
    uint32_t val;
} pmu_hp_regulator1_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t xpd_xtal : 1;
    };
    uint32_t val;
} pmu_hp_xtal_reg_t;

typedef struct pmu_hp_hw_regmap_t{
    pmu_hp_dig_power_reg_t     dig_power;
    uint32_t                   icg_func;
    uint32_t                   icg_apb;
    pmu_hp_icg_modem_reg_t     icg_modem;
    pmu_hp_sys_cntl_reg_t      syscntl;
    pmu_hp_clk_power_reg_t     clk_power;
    pmu_hp_bias_reg_t          bias;
    pmu_hp_backup_reg_t        backup;
    uint32_t                   backup_clk;
    pmu_hp_sysclk_reg_t        sysclk;
    pmu_hp_regulator0_reg_t    regulator0;
    pmu_hp_regulator1_reg_t    regulator1;
    pmu_hp_xtal_reg_t          xtal;
} pmu_hp_hw_regmap_t;

/** */
typedef union {
    struct {
        uint32_t reserved0: 21;
        uint32_t slp_xpd  : 1;
        uint32_t xpd	  : 1;
        uint32_t slp_dbias: 4;
        uint32_t dbias    : 5;
    };
    uint32_t val;
} pmu_lp_regulator0_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 28;
        uint32_t drv_b    : 4;
    };
    uint32_t val;
} pmu_lp_regulator1_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t xpd_xtal : 1;
    };
    uint32_t val;
} pmu_lp_xtal_reg_t;

typedef union {
    struct {
        uint32_t reserved0 : 30;
        uint32_t mem_dslp  : 1;
        uint32_t peri_pd_en: 1;
    };
    uint32_t val;
} pmu_lp_dig_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0  : 28;
        uint32_t xpd_xtal32k: 1;
        uint32_t xpd_rc32k  : 1;
        uint32_t xpd_fosc   : 1;
        uint32_t pd_osc     : 1;
    };
    uint32_t val;
} pmu_lp_clk_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0 : 25;
        uint32_t xpd_bias  : 1;
        uint32_t dbg_atten : 4;
        uint32_t pd_cur    : 1;
        uint32_t bias_sleep: 1;
    };
    uint32_t val;
} pmu_lp_bias_reg_t;

typedef struct pmu_lp_hw_regmap_t{
    pmu_lp_regulator0_reg_t    regulator0;
    pmu_lp_regulator1_reg_t    regulator1;
    pmu_lp_xtal_reg_t          xtal;   /* Only LP_SLEEP mode under lp system is valid */
    pmu_lp_dig_power_reg_t     dig_power;
    pmu_lp_clk_power_reg_t     clk_power;
    pmu_lp_bias_reg_t          bias;   /* Only LP_SLEEP mode under lp system is valid */
} pmu_lp_hw_regmap_t;


typedef union {
    struct {
        uint32_t tie_low_global_bbpll_icg : 1;
        uint32_t tie_low_global_xtal_icg  : 1;
        uint32_t tie_low_i2c_retention    : 1;
        uint32_t tie_low_xpd_bb_i2c       : 1;
        uint32_t tie_low_xpd_bbpll_i2c    : 1;
        uint32_t tie_low_xpd_bbpll        : 1;
        uint32_t tie_low_xpd_xtal         : 1;
        uint32_t reserved0                : 18;
        uint32_t tie_high_global_bbpll_icg: 1;
        uint32_t tie_high_global_xtal_icg : 1;
        uint32_t tie_high_i2c_retention   : 1;
        uint32_t tie_high_xpd_bb_i2c      : 1;
        uint32_t tie_high_xpd_bbpll_i2c   : 1;
        uint32_t tie_high_xpd_bbpll       : 1;
        uint32_t tie_high_xpd_xtal        : 1;
    };
    uint32_t val;
} pmu_imm_hp_clk_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0            : 28;
        uint32_t update_dig_icg_switch: 1;
        uint32_t tie_low_icg_slp_sel  : 1;
        uint32_t tie_high_icg_slp_sel : 1;
        uint32_t update_dig_sysclk_sel: 1;
    };
    uint32_t val;
} pmu_imm_sleep_sysclk_reg_t;

typedef union {
    struct {
        uint32_t reserved0             : 31;
        uint32_t update_dig_icg_func_en: 1;
    };
    uint32_t val;
} pmu_imm_hp_func_icg_reg_t;

typedef union {
    struct {
        uint32_t reserved0            : 31;
        uint32_t update_dig_icg_apb_en: 1;
    };
    uint32_t val;
} pmu_imm_hp_apb_icg_reg_t;

typedef union {
    struct {
        uint32_t reserved0              : 31;
        uint32_t update_dig_icg_modem_en: 1;
    };
    uint32_t val;
} pmu_imm_modem_icg_reg_t;

typedef union {
    struct {
        uint32_t reserved0              : 30;
        uint32_t tie_low_lp_rootclk_sel : 1;
        uint32_t tie_high_lp_rootclk_sel: 1;
    };
    uint32_t val;
} pmu_imm_lp_icg_reg_t;

typedef union {
    struct {
        uint32_t reserved0               : 28;
        uint32_t tie_high_lp_pad_hold_all: 1;
        uint32_t tie_low_lp_pad_hold_all : 1;
        uint32_t tie_high_hp_pad_hold_all: 1;
        uint32_t tie_low_hp_pad_hold_all : 1;
    };
    uint32_t val;
} pmu_imm_pad_hold_all_reg_t;

typedef union {
    struct {
        uint32_t reserved0          : 30;
        uint32_t tie_high_i2c_iso_en: 1;
        uint32_t tie_low_i2c_iso_en : 1;
    };
    uint32_t val;
} pmu_imm_i2c_isolate_reg_t;

typedef struct pmu_imm_hw_regmap_t{
    pmu_imm_hp_clk_power_reg_t     clk_power;
    pmu_imm_sleep_sysclk_reg_t     sleep_sysclk;
    pmu_imm_hp_func_icg_reg_t      hp_func_icg;
    pmu_imm_hp_apb_icg_reg_t       hp_apb_icg;
    pmu_imm_modem_icg_reg_t        modem_icg;
    pmu_imm_lp_icg_reg_t           lp_icg;
    pmu_imm_pad_hold_all_reg_t     pad_hold_all;
    pmu_imm_i2c_isolate_reg_t      i2c_iso;
} pmu_imm_hw_regmap_t;

typedef union {
    struct {
        uint32_t reserved0      : 5;
        uint32_t powerdown_timer: 9;
        uint32_t powerup_timer  : 9;
        uint32_t wait_timer     : 9;
    };
    uint32_t val;
} pmu_power_wait_timer0_reg_t;

typedef union {
    struct {
        uint32_t reserved0      : 9;
        uint32_t powerdown_timer: 7;
        uint32_t powerup_timer  : 7;
        uint32_t wait_timer     : 9;
    };
    uint32_t val;
} pmu_power_wait_timer1_reg_t;

typedef union {
    struct {
        uint32_t force_reset   : 1;
        uint32_t force_iso     : 1;
        uint32_t force_pu      : 1;
        uint32_t force_no_reset: 1;
        uint32_t force_no_iso  : 1;
        uint32_t force_pd      : 1;
        uint32_t mask          : 5;     /* Invalid of lp peripherals */
        uint32_t reserved0     : 16;    /* Invalid of lp peripherals */
        uint32_t pd_mask       : 5;     /* Invalid of lp peripherals */
    };
    uint32_t val;
} pmu_power_domain_cntl_reg_t;

typedef union {
    struct {
        uint32_t force_hp_mem_iso   : 4;
        uint32_t force_hp_mem_pd    : 4;
        uint32_t reserved0          : 16;
        uint32_t force_hp_mem_no_iso: 4;
        uint32_t force_hp_mem_pu    : 4;
    };
    uint32_t val;
} pmu_power_memory_cntl_reg_t;

typedef union {
    struct {
        uint32_t mem2_pd_mask: 5;
        uint32_t mem1_pd_mask: 5;
        uint32_t mem0_pd_mask: 5;
        uint32_t reserved0   : 2;
        uint32_t mem2_mask   : 5;
        uint32_t mem1_mask   : 5;
        uint32_t mem0_mask   : 5;
    };
    uint32_t val;
} pmu_power_memory_mask_reg_t;

typedef union {
    struct {
        uint32_t force_hp_pad_no_iso_all: 1;
        uint32_t force_hp_pad_iso_all   : 1;
        uint32_t reserved0              : 30;
    };
    uint32_t val;
} pmu_power_hp_pad_reg_t;

typedef union {
    struct {
        uint32_t reserved0 : 18;
        uint32_t pwr_wait  : 11;
        uint32_t pwr_sw    : 2;
        uint32_t pwr_sel_sw: 1;
    };
    uint32_t val;
} pmu_power_vdd_spi_cntl_reg_t;

typedef union {
    struct {
        uint32_t wait_xtal_stable: 16;
        uint32_t wait_pll_stable : 16;
    };
    uint32_t val;
} pmu_power_clk_wait_cntl_reg_t;

typedef struct pmu_power_hw_regmap_t{
    pmu_power_wait_timer0_reg_t    wait_timer0;
    pmu_power_wait_timer1_reg_t    wait_timer1;
    pmu_power_domain_cntl_reg_t    hp_pd[5];
    pmu_power_domain_cntl_reg_t    lp_peri;
    pmu_power_memory_cntl_reg_t    mem_cntl;
    pmu_power_memory_mask_reg_t    mem_mask;
    pmu_power_hp_pad_reg_t         hp_pad;
    pmu_power_vdd_spi_cntl_reg_t   vdd_spi;
    pmu_power_clk_wait_cntl_reg_t  clk_wait;
} pmu_power_hw_regmap_t;

typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t sleep_req: 1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl0_reg_t;

typedef union {
    struct {
        uint32_t sleep_reject_ena: 31;
        uint32_t slp_reject_en   : 1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl1_reg_t;

typedef union {
    struct {
        uint32_t lp_min_slp_val: 8;
        uint32_t hp_min_slp_val: 8;
        uint32_t sleep_prt_sel : 2;
        uint32_t reserved0     : 14;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl3_reg_t;

typedef union {
    struct {
        uint32_t reserved0           : 31;
        uint32_t slp_reject_cause_clr: 1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl4_reg_t;

typedef union {
    struct {
        uint32_t modem_wait_target : 20;
        uint32_t reserved0         : 4;
        uint32_t lp_ana_wait_target: 8;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl5_reg_t;

typedef union {
    struct {
        uint32_t soc_wakeup_wait    : 20;
        uint32_t reserved0          : 10;
        uint32_t soc_wakeup_wait_cfg: 2;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl6_reg_t;

typedef union {
    struct {
        uint32_t reserved0      : 16;
        uint32_t ana_wait_target: 16;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl7_reg_t;

typedef struct pmu_wakeup_hw_regmap_t{
    pmu_slp_wakeup_cntl0_reg_t     cntl0;
    pmu_slp_wakeup_cntl1_reg_t     cntl1;
    uint32_t                       cntl2;
    pmu_slp_wakeup_cntl3_reg_t     cntl3;
    pmu_slp_wakeup_cntl4_reg_t     cntl4;
    pmu_slp_wakeup_cntl5_reg_t     cntl5;
    pmu_slp_wakeup_cntl6_reg_t     cntl6;
    pmu_slp_wakeup_cntl7_reg_t     cntl7;
    uint32_t                       status0;
    uint32_t                       status1;
} pmu_wakeup_hw_regmap_t;

typedef union {
    struct {
        uint32_t i2c_por_wait_target: 8;
        uint32_t reserved0          : 24;
    };
    uint32_t val;
} pmu_hp_clk_poweron_reg_t;

typedef union {
    struct {
        uint32_t modify_icg_cntl_wait: 8;
        uint32_t switch_icg_cntl_wait: 8;
        uint32_t reserved0           : 16;
    };
    uint32_t val;
} pmu_hp_clk_cntl_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t por_done : 1;
    };
    uint32_t val;
} pmu_por_status_reg_t;

typedef union {
    struct {
        uint32_t reserved0     : 26;
        uint32_t perif_i2c_rstb: 1;
        uint32_t xpd_perif_i2c : 1;
        uint32_t xpd_txrf_i2c  : 1;
        uint32_t xpd_rfrx_pbus : 1;
        uint32_t xpd_ckgen_i2c : 1;
        uint32_t xpd_pll_i2c   : 1;
    };
    uint32_t val;
} pmu_rf_pwc_reg_t;

typedef union {
    struct {
        uint32_t reserved0          : 31;
        uint32_t backup_sysclk_nodiv: 1;
    };
    uint32_t val;
} pmu_backup_cfg_reg_t;

typedef union {
    struct {
        uint32_t reserved0 : 27;
        uint32_t lp_cpu_exc: 1;
        uint32_t sdio_idle : 1;
        uint32_t sw        : 1;
        uint32_t reject    : 1;
        uint32_t wakeup    : 1;
    };
    uint32_t val;
} pmu_hp_intr_reg_t;

typedef struct pmu_hp_ext_hw_regmap_t{
    pmu_hp_clk_poweron_reg_t   clk_poweron;
    pmu_hp_clk_cntl_reg_t      clk_cntl;
    pmu_por_status_reg_t       por_status;
    pmu_rf_pwc_reg_t           rf_pwc;
    pmu_backup_cfg_reg_t       backup_cfg;
    pmu_hp_intr_reg_t          int_raw;
    pmu_hp_intr_reg_t          int_st;
    pmu_hp_intr_reg_t          int_ena;
    pmu_hp_intr_reg_t          int_clr;
} pmu_hp_ext_hw_regmap_t;

typedef union {
    struct {
        uint32_t reserved0                : 20;
        uint32_t lp_cpu_wakeup            : 1;
        uint32_t modem_switch_active_end  : 1;
        uint32_t sleep_switch_active_end  : 1;
        uint32_t sleep_switch_modem_end   : 1;
        uint32_t modem_switch_sleep_end   : 1;
        uint32_t active_swtich_sleep_end  : 1;
        uint32_t modem_switch_active_start: 1;
        uint32_t sleep_switch_active_start: 1;
        uint32_t sleep_switch_modem_start : 1;
        uint32_t modem_switch_sleep_start : 1;
        uint32_t active_switch_sleep_start: 1;
        uint32_t sw_trigger               : 1;
    };
    uint32_t val;
} pmu_lp_intr_reg_t;

typedef union {
    struct {
        uint32_t waiti_rdy         : 1;
        uint32_t stall_rdy         : 1;
        uint32_t reserved0         : 16;
        uint32_t force_stall       : 1;
        uint32_t slp_waiti_flag_en : 1;
        uint32_t slp_stall_flag_en : 1;
        uint32_t slp_stall_wait    : 8;
        uint32_t slp_stall_en      : 1;
        uint32_t slp_reset_en      : 1;
        uint32_t slp_bypass_intr_en: 1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr0_reg_t;

typedef union {
    struct {
        uint32_t wakeup_en: 16;
        uint32_t reserved0: 15;
        uint32_t sleep_req: 1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr1_reg_t;

typedef struct pmu_lp_ext_hw_regmap_t{
    pmu_lp_intr_reg_t          int_raw;
    pmu_lp_intr_reg_t          int_st;
    pmu_lp_intr_reg_t          int_ena;
    pmu_lp_intr_reg_t          int_clr;
    pmu_lp_cpu_pwr0_reg_t      pwr0;
    pmu_lp_cpu_pwr1_reg_t      pwr1;
} pmu_lp_ext_hw_regmap_t;

typedef struct pmu_dev_t{
    volatile pmu_hp_hw_regmap_t     hp_sys[3];
    volatile pmu_lp_hw_regmap_t     lp_sys[2];
    volatile pmu_imm_hw_regmap_t    imm;
    volatile pmu_power_hw_regmap_t  power;
    volatile pmu_wakeup_hw_regmap_t wakeup;
    volatile pmu_hp_ext_hw_regmap_t hp_ext;
    volatile pmu_lp_ext_hw_regmap_t lp_ext;

    union {
        struct {
            uint32_t reserved0    : 30;
            volatile uint32_t lp_trigger_hp: 1;
            volatile uint32_t hp_trigger_lp: 1;
        };
        volatile uint32_t val;
    } hp_lp_cpu_comm;

    union {
        struct {
            uint32_t reserved0           : 31;
            volatile uint32_t dig_regulator_en_cal: 1;
        };
        volatile uint32_t val;
    } hp_regulator_cfg;

    union {
        struct {
            uint32_t reserved0 : 11;
            volatile uint32_t last_st   : 7;
            volatile uint32_t target_st : 7;
            volatile uint32_t current_st: 7;
        };
        volatile uint32_t val;
    } main_state;

    union {
        struct {
            uint32_t reserved0: 13;
            volatile uint32_t backup_st: 5;
            volatile uint32_t lp_pwr_st: 5;
            volatile uint32_t hp_pwr_st: 9;
        };
        volatile uint32_t val;
    } pwr_state;

    union {
        struct {
            volatile uint32_t stable_xpd_bbpll : 1;
            volatile uint32_t stable_xpd_xtal  : 1;
            volatile uint32_t reserved0        : 13;
            volatile uint32_t sysclk_slp_sel   : 1;
            volatile uint32_t sysclk_sel       : 2;
            volatile uint32_t sysclk_nodiv     : 1;
            volatile uint32_t icg_sysclk_en    : 1;
            volatile uint32_t icg_modem_switch : 1;
            volatile uint32_t icg_modem_code   : 2;
            volatile uint32_t icg_slp_sel      : 1;
            volatile uint32_t icg_global_xtal  : 1;
            volatile uint32_t icg_global_pll   : 1;
            volatile uint32_t ana_i2c_iso_en   : 1;
            volatile uint32_t ana_i2c_retention: 1;
            volatile uint32_t ana_xpd_bb_i2c   : 1;
            volatile uint32_t ana_xpd_bbpll_i2c: 1;
            volatile uint32_t ana_xpd_bbpll    : 1;
            volatile uint32_t ana_xpd_xtal     : 1;
        };
        volatile uint32_t val;
    } clk_state0;

    volatile uint32_t clk_state1;
    volatile uint32_t clk_state2;

    union {
        struct {
            uint32_t reserved0             : 31;
            volatile uint32_t stable_vdd_spi_pwr_drv: 1;
        };
        volatile uint32_t val;
    } vdd_spi_status;

    uint32_t reserved[150];

    union {
        struct {
            volatile uint32_t pmu_date: 31;
            volatile uint32_t clk_en  : 1;
        };
        volatile uint32_t val;
    } date;
} pmu_dev_t;

extern pmu_dev_t PMU;

#ifndef __cplusplus
_Static_assert(sizeof(pmu_dev_t) == 0x400, "Invalid size of pmu_dev_t structure");

_Static_assert(offsetof(pmu_dev_t, reserved) == (PMU_VDD_SPI_STATUS_REG - DR_REG_PMU_BASE) + 4, "Invalid size of pmu_dev_t structure");

#endif

#ifdef __cplusplus
}
#endif
