/**
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
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
        uint32_t reserved0          : 20;
        uint32_t vdd_spi_pd_en      : 1;
        uint32_t pd_hp_alive_pd_en  : 1;
        uint32_t hp_mem_dslp        : 1;
        uint32_t hp_mem_pd_en       : 4;
        uint32_t modem_top_pd_en    : 1;
        uint32_t hp_cnnt_pd_en      : 1;
        uint32_t hp_cpu_pd_en       : 1;
        uint32_t modem_pwr_pd_en    : 1;
        uint32_t top_pd_en          : 1;
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
        uint32_t reserved0      : 23;
        uint32_t c_channel      : 1;
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
        uint32_t reserved0    : 19;
        uint32_t xpd_xtalx2   : 1;
        uint32_t i2c_iso_en   : 1;
        uint32_t i2c_retention: 1;
        uint32_t xpd_bb_i2c   : 1;
        uint32_t xpd_pll_i2c  : 4;
        uint32_t xpd_pll      : 4;
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
        uint32_t hp_sleep2active_backup_mode          : 5;
        uint32_t hp_modem2active_backup_mode          : 5;
        uint32_t reserved3                            : 1;
        uint32_t hp_sleep2active_backup_en            : 1;
        uint32_t hp_modem2active_backup_en            : 1;
        uint32_t reserved4                            : 1;
    };
    struct {    /* HP: Modem State */
        uint32_t reserved5                            : 4;
        uint32_t hp_sleep2modem_backup_modem_clk_code : 2;
        uint32_t reserved6                            : 4;
        uint32_t hp_modem_retention_mode              : 1;
        uint32_t hp_sleep2modem_retention_en          : 1;
        uint32_t reserved7                            : 2;
        uint32_t hp_sleep2modem_backup_clk_sel        : 2;
        uint32_t reserved8                            : 4;
        uint32_t hp_sleep2modem_backup_mode           : 5;
        uint32_t reserved9                            : 4;
        uint32_t hp_sleep2modem_backup_en             : 1;
        uint32_t reserved10                           : 2;
    };
    struct {    /* HP: Sleep State */
        uint32_t reserved11                           : 6;
        uint32_t hp_modem2sleep_backup_modem_clk_code : 2;
        uint32_t hp_active2sleep_backup_modem_clk_code: 2;
        uint32_t hp_sleep_retention_mode              : 1;
        uint32_t reserved12                           : 1;
        uint32_t hp_modem2sleep_retention_en          : 1;
        uint32_t hp_active2sleep_retention_en         : 1;
        uint32_t reserved13                           : 2;
        uint32_t hp_modem2sleep_backup_clk_sel       : 2;
        uint32_t hp_active2sleep_backup_clk_sel       : 2;
        uint32_t hp_modem2sleep_backup_mode           : 5;
        uint32_t hp_active2sleep_backup_mode          : 5;
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

/* HP System: HP Regulator0 Register */
typedef union {
    struct {    /* HP: Active State */
        uint32_t reserved0      : 3;
        uint32_t dig_dbias_init : 1;    /* Only HP_ACTIVE mode under hp system is valid */
        uint32_t lp_dbias_vol   : 5;    /* Only HP_ACTIVE mode under hp system is valid */
        uint32_t hp_dbias_vol   : 5;    /* Only HP_ACTIVE mode under hp system is valid */
        uint32_t dbias_sel      : 1;    /* Only HP_ACTIVE mode under hp system is valid */
        uint32_t slp_connect_en : 1;
        uint32_t slp_mem_xpd    : 1;
        uint32_t slp_logic_xpd  : 1;
        uint32_t xpd            : 1;
        uint32_t slp_mem_dbias  : 4;
        uint32_t slp_logic_dbias: 4;
        uint32_t dbias: 5;
    };
    uint32_t val;
} pmu_hp_regulator0_reg_t;

/* HP System: HP Regulator1 Register */
typedef union {
    struct {
        uint32_t reserved0: 8;
        uint32_t drv_b    : 24;
    };
    uint32_t val;
} pmu_hp_regulator1_reg_t;

/* HP System: Xtal Register */
typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t xpd_xtal : 1;
    };
    uint32_t val;
} pmu_hp_xtal_reg_t;

/* HP System Hardware Register Map */
typedef struct pmu_hp_hw_regmap_t {
    pmu_hp_dig_power_reg_t     dig_power;
    struct {
        uint32_t               icg_func;
        uint32_t               icg_apb;
    } icg[2];
    pmu_hp_icg_modem_reg_t     icg_modem;
    pmu_hp_sys_cntl_reg_t      syscntl;
    pmu_hp_clk_power_reg_t     clk_power;
    pmu_hp_bias_reg_t          bias;
    pmu_hp_backup_reg_t        backup_cfg;
    uint32_t                   backup_clock[2];
    pmu_hp_sysclk_reg_t        sysclk;
    pmu_hp_regulator0_reg_t    regulator0;
    pmu_hp_regulator1_reg_t    regulator1;
    pmu_hp_xtal_reg_t          xtal;
} pmu_hp_hw_regmap_t;

/* LP System: LP Regulator0 Register */
typedef union {
    struct {
        uint32_t reserved0:21;
        uint32_t slp_xpd:1;
        uint32_t xpd:1;
        uint32_t slp_dbias:4;
        uint32_t dbias:5;
    };
    uint32_t val;
} pmu_lp_regulator0_reg_t;

/* LP System: LP Regulator1 Register */
typedef union {
    struct {
        uint32_t reserved0:28;
        uint32_t drv_b:4;
    };
    uint32_t val;
} pmu_lp_regulator1_reg_t;

/* LP System: Xtal Register */
typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t xpd_xtal : 1;
    };
    uint32_t val;
} pmu_lp_xtal_reg_t;

/* LP System: LP Dig Power Register */
typedef union {
    struct {
        uint32_t reserved0:30;
        uint32_t mem_dslp:1;
        uint32_t peri_pd_en:1;
    };
    uint32_t val;
} pmu_lp_dig_power_reg_t;

/* LP System: LP CK Power Register */
typedef union {
    struct {
        uint32_t reserved0:28;
        uint32_t xpd_xtal32k:1;
        uint32_t xpd_rc32k:1;
        uint32_t xpd_fosc:1;
        uint32_t pd_osc:1;
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

/* LP System Hardware Register Map */
typedef struct pmu_lp_hw_regmap_t{
    pmu_lp_regulator0_reg_t    regulator0;
    pmu_lp_regulator1_reg_t    regulator1;
    pmu_lp_xtal_reg_t          xtal;   /* Only LP_SLEEP mode under lp system is valid */
    pmu_lp_dig_power_reg_t     dig_power;
    pmu_lp_clk_power_reg_t     clk_power;
    pmu_lp_bias_reg_t          bias;   /* Only LP_SLEEP mode under lp system is valid */
} pmu_lp_hw_regmap_t;

/* Immediate Control: HP CK Power Register */
typedef union {
    struct {
        uint32_t tie_low_global_xtalx2_icg:1;
        uint32_t tie_low_global_xtal_icg:1;
        uint32_t tie_low_i2c_retention:1;
        uint32_t tie_low_xpd_bb_i2c:1;
        uint32_t reserved0                : 1;
        uint32_t tie_low_xpd_xtalx2:1;
        uint32_t tie_low_xpd_xtal:1;
        uint32_t reserved1                : 18;
        uint32_t tie_high_global_xtalx2_icg:1;
        uint32_t tie_high_global_xtal_icg:1;
        uint32_t tie_high_i2c_retention:1;
        uint32_t tie_high_xpd_bb_i2c:1;
        uint32_t reserved2                : 1;
        uint32_t tie_high_xpd_xtalx2:1;
        uint32_t tie_high_xpd_xtal:1;
    };
    uint32_t val;
} pmu_imm_hp_clk_power_0_reg_t;

typedef union {
    struct {
        uint32_t tie_low_xpd_pll_i2c:4;
        uint32_t tie_low_xpd_pll:4;
        uint32_t tie_low_global_pll_icg:4;
        uint32_t reserved0                : 7;
        uint32_t tie_high_global_pll_icg:4;
        uint32_t tie_high_xpd_pll_i2c:4;
        uint32_t tie_high_xpd_pll:4;
        uint32_t reserved1                : 1;
    };
    uint32_t val;
} pmu_imm_hp_clk_power_1_reg_t;

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
} pmu_imm_i2c_iso_reg_t;

typedef struct pmu_imm_hw_regmap_t{
    pmu_imm_hp_clk_power_0_reg_t   clk_power_0;
    pmu_imm_hp_clk_power_1_reg_t   clk_power_1;
    pmu_imm_sleep_sysclk_reg_t      sleep_sysclk;
    pmu_imm_hp_func_icg_reg_t       hp_func_icg;
    pmu_imm_hp_apb_icg_reg_t        hp_apb_icg;
    pmu_imm_modem_icg_reg_t         modem_icg;
    pmu_imm_lp_icg_reg_t            lp_icg;
    pmu_imm_pad_hold_all_reg_t      pad_hold_all;
    pmu_imm_i2c_iso_reg_t           i2c_iso;
} pmu_imm_hw_regmap_t;

/* Power Domain Control Register */
typedef union {
    struct {
        uint32_t force_top_reset:1;
        uint32_t force_top_iso:1;
        uint32_t force_top_pu:1;
        uint32_t force_top_no_reset:1;
        uint32_t force_top_no_iso:1;
        uint32_t force_top_pd:1;
        uint32_t pd_top_mask:5;
        uint32_t reserved0     : 16;
        uint32_t pd_top_pd_mask: 5;
    };
    struct {
        uint32_t force_hp_alive_reset: 1;
        uint32_t force_hp_alive_iso: 1;
        uint32_t force_hp_alive_pu: 1;
        uint32_t force_hp_alive_no_reset: 1;
        uint32_t force_hp_alive_no_iso: 1;
        uint32_t force_hp_alive_pd: 1;
        uint32_t pd_hp_alive_mask: 5;
        uint32_t reserved1     : 16;
        uint32_t pd_hp_alive_pd_mask: 5;
    };
    struct {
        uint32_t force_modem_pwr_reset: 1;
        uint32_t force_modem_pwr_iso: 1;
        uint32_t force_modem_pwr_pu: 1;
        uint32_t force_modem_pwr_no_reset: 1;
        uint32_t force_modem_pwr_no_iso: 1;
        uint32_t force_modem_pwr_pd: 1;
        uint32_t pd_modem_pwr_mask: 5;
        uint32_t reserved2     : 16;
        uint32_t pd_modem_pwr_pd_mask: 5;
    };
    struct {
        uint32_t force_hp_cpu_reset: 1;
        uint32_t force_hp_cpu_iso: 1;
        uint32_t force_hp_cpu_pu: 1;
        uint32_t force_hp_cpu_no_reset: 1;
        uint32_t force_hp_cpu_no_iso: 1;
        uint32_t force_hp_cpu_pd: 1;
        uint32_t pd_hp_cpu_mask: 5;
        uint32_t reserved3     : 16;
        uint32_t pd_hp_cpu_pd_mask: 5;
    };
    struct {
        uint32_t force_hp_cnnt_reset: 1;
        uint32_t force_hp_cnnt_iso: 1;
        uint32_t force_hp_cnnt_pu: 1;
        uint32_t force_hp_cnnt_no_reset: 1;
        uint32_t force_hp_cnnt_no_iso: 1;
        uint32_t force_hp_cnnt_pd: 1;
        uint32_t pd_hp_cnnt_mask: 5;
        uint32_t reserved4     : 16;
        uint32_t pd_hp_cnnt_pd_mask: 5;
    };
    struct {
        uint32_t force_modem_top_reset: 1;
        uint32_t force_modem_top_iso: 1;
        uint32_t force_modem_top_pu: 1;
        uint32_t force_modem_top_no_reset: 1;
        uint32_t force_modem_top_no_iso: 1;
        uint32_t force_modem_top_pd: 1;
        uint32_t pd_modem_top_mask: 5;
        uint32_t reserved5     : 16;
        uint32_t pd_modem_top_pd_mask: 5;
    };
    struct {
        uint32_t force_lp_peri_reset: 1;
        uint32_t force_lp_peri_iso: 1;
        uint32_t force_lp_peri_pu: 1;
        uint32_t force_lp_peri_no_reset: 1;
        uint32_t force_lp_peri_no_iso: 1;
        uint32_t force_lp_peri_pd: 1;
        uint32_t reserved6     : 26;
    };
    uint32_t val;
} pmu_power_domain_cntl_reg_t;

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
        uint32_t dg_lp_iso_wait_timer: 8;
        uint32_t dg_lp_rst_wait_timer: 8;
        uint32_t dg_hp_iso_wait_timer: 8;
        uint32_t dg_hp_rst_wait_timer: 8;
    };
    uint32_t val;
} pmu_power_wait_timer2_reg_t;

typedef union {
    struct {
        uint32_t force_hp_mem_iso:4;
        uint32_t force_hp_mem_pd:4;
        uint32_t reserved0          : 16;
        uint32_t force_hp_mem_no_iso:4;
        uint32_t force_hp_mem_pu:4;
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
        uint32_t force_hp_pad_no_iso_all:1;
        uint32_t force_hp_pad_iso_all:1;
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
    pmu_power_wait_timer0_reg_t        wait_timer0;
    pmu_power_wait_timer1_reg_t        wait_timer1;
    pmu_power_wait_timer2_reg_t        wait_timer2;
    pmu_power_domain_cntl_reg_t        hp_pd[6];
    pmu_power_domain_cntl_reg_t        lp_peri;
    pmu_power_memory_cntl_reg_t        mem_cntl;
    pmu_power_memory_mask_reg_t        mem_mask;
    pmu_power_hp_pad_reg_t             hp_pad;
    pmu_power_vdd_spi_cntl_reg_t       vdd_spi;
    pmu_power_clk_wait_cntl_reg_t      clk_wait;
} pmu_power_hw_regmap_t;

/* Sleep/Wakeup Control Registers */
typedef union {
    struct {
        uint32_t reserved0: 30;
        uint32_t slp_reject_en: 1;
        uint32_t sleep_req: 1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl0_reg_t;

typedef union {
    struct {
        uint32_t sleep_reject_ena:32;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl1_reg_t;

typedef union {
    struct {
        uint32_t lp_min_slp_val:8;
        uint32_t hp_min_slp_val:8;
        uint32_t sleep_prt_sel:2;
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
        uint32_t modem_wait_target:20;
        uint32_t reserved0         : 4;
        uint32_t lp_ana_wait_target: 8;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl5_reg_t;

typedef union {
    struct {
        uint32_t soc_wakeup_wait:20;
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

/* HP Extended Registers */
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
        uint32_t reserved0     : 16;
        uint32_t xpd_rf_circuit: 16;
    };
    uint32_t val;
} pmu_rf_pwc_reg_t;

typedef union {
    struct {
        uint32_t reserved0     : 31;
        uint32_t backup_sys_clk_no_div: 1;
    };
    uint32_t val;
} pmu_backup_cfg_reg_t;

typedef union {
    struct {
        uint32_t reserved0:27;
        uint32_t lp_cpu_exc:1;
        uint32_t sdio_idle:1;
        uint32_t sw:1;
        uint32_t soc_sleep_reject:1;
        uint32_t soc_wakeup:1;
    };
    uint32_t val;
} pmu_hp_intr_reg_t;

typedef struct pmu_hp_ext_hw_regmap_t {
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

/* LP Extended Registers */
typedef union {
    struct {
        uint32_t reserved0                : 20;
        uint32_t lp_cpu_wakeup            : 1;
        uint32_t modem_switch_active_end  : 1;
        uint32_t sleep_switch_active_end  : 1;
        uint32_t sleep_switch_modem_end   : 1;
        uint32_t modem_switch_sleep_end   : 1;
        uint32_t active_switch_sleep_end  : 1;
        uint32_t modem_switch_active_start: 1;
        uint32_t sleep_switch_active_start: 1;
        uint32_t sleep_switch_modem_start : 1;
        uint32_t modem_switch_sleep_start : 1;
        uint32_t active_switch_sleep_start: 1;
        uint32_t hp_sw_trigger            : 1;
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
        uint32_t reserved0: 31;
        uint32_t sleep_req: 1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr1_reg_t;

typedef union {
    struct {
        uint32_t wakeup_en: 32;
    };
    uint32_t val;
} pmu_lp_cpu_pwr2_reg_t;

typedef struct pmu_lp_ext_hw_regmap_t{
    pmu_lp_intr_reg_t          int_raw;
    pmu_lp_intr_reg_t          int_st;
    pmu_lp_intr_reg_t          int_ena;
    pmu_lp_intr_reg_t          int_clr;
    pmu_lp_cpu_pwr0_reg_t      pwr0;
    pmu_lp_cpu_pwr1_reg_t      pwr1;
    pmu_lp_cpu_pwr2_reg_t      pwr2;
} pmu_lp_ext_hw_regmap_t;

/* Additional Registers */
typedef union {
    struct {
        uint32_t reserved0    : 30;
        uint32_t lp_trigger_hp: 1;
        uint32_t hp_trigger_lp: 1;
    };
    uint32_t val;
} pmu_hp_lp_cpu_comm_reg_t;

typedef union {
    struct {
        uint32_t reserved0           : 31;
        uint32_t dig_regulator_en_cal: 1;
    };
    uint32_t val;
} pmu_hp_regulator_cfg_reg_t;

typedef union {
    struct {
        uint32_t reserved0 : 11;
        uint32_t last_st   : 7;
        uint32_t target_st : 7;
        uint32_t current_st: 7;
    };
    uint32_t val;
} pmu_main_state_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 13;
        uint32_t backup_st: 5;
        uint32_t lp_pwr_st: 5;
        uint32_t hp_pwr_st: 9;
    };
    uint32_t val;
} pmu_pwr_state_reg_t;

typedef union {
    struct {
        uint32_t ext_wakeup_lv:21;
        uint32_t reserved0: 11;
    };
    uint32_t val;
} pmu_ext_wakeup_lv_reg_t;

typedef union {
    struct {
        uint32_t ext_wakeup_sel:21;
        uint32_t reserved0: 11;
    };
    uint32_t val;
} pmu_ext_wakeup_sel_reg_t;

typedef union {
    struct {
        uint32_t ext_wakeup_status:21;
        uint32_t reserved0: 11;
    };
    uint32_t val;
} pmu_ext_wakeup_st_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 30;
        uint32_t ext_wakeup_status_clr: 1;
        uint32_t ext_wakeup_filter: 1;
    };
    uint32_t val;
} pmu_ext_wakeup_cntl_reg_t;

typedef union {
    struct {
        uint32_t sdio_act_dnum:10;
        uint32_t reserved0: 22;
    };
    uint32_t val;
} pmu_sdio_wakeup_cntl_reg_t;

typedef union {
    struct {
        uint32_t touch_sleep_cycles:16;
        uint32_t reserved0     : 5;
        uint32_t touch_wait_cycles:9;
        uint32_t touch_sleep_timer_en:1;
        uint32_t touch_force_done:1;
    };
    uint32_t val;
} pmu_touch_pwr_ctrl_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 30;
        uint32_t xpd_perif_i2c: 1;
        uint32_t rstb_perif_i2c: 1;
    };
    uint32_t val;
} pmu_ana_peri_pwr_ctrl_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t pmu_rdn_eco_en: 1;
    };
    uint32_t val;
} pmu_rdn_eco_reg_t;

typedef union {
    struct {
        uint32_t puf_mem_psw:1;
        uint32_t puf_mem_iso:1;
        uint32_t puf_mem_discharge:1;
        uint32_t reserved0: 29;
    };
    uint32_t val;
} pmu_puf_mem_ctrl_reg_t;

typedef union {
    struct {
        uint32_t pd_top_pu_delt_timer:3;
        uint32_t pd_hp_cpu_pu_delt_timer:3;
        uint32_t pd_modem_top_pu_delt_timer:3;
        uint32_t pd_modem_pwr_pu_delt_timer:3;
        uint32_t pd_hp_cnnt_pu_delt_timer:3;
        uint32_t pd_hp_alive_pu_delt_timer:3;
        uint32_t reserved0     : 14;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl8_reg_t;

typedef union {
    struct {
        uint32_t ext_ldo_tie_high:1;
        uint32_t ext_ldo_en_vdet:1;
        uint32_t ext_cur_lim:1;
        uint32_t ext_ldo_mul:3;
        uint32_t ext_ldo_dref:4;
        uint32_t reserved0: 22;
    };
    uint32_t val;
} pmu_ext_ldo_ctrl_reg_t;

typedef union {
    struct {
        uint32_t cpu_pwr_state:6;
        uint32_t io_0p2a_ovdet:1;
        uint32_t reserved0: 25;
    };
    uint32_t val;
} pmu_debug_ro_reg_t;

typedef union {
    struct {
        uint32_t lpcpu_wakeup_cause:32;
    };
    uint32_t val;
} pmu_lpcpu_wakeup_cause_reg_t;

typedef union {
    struct {
        uint32_t backup_wait_target:4;
        uint32_t reserved0: 28;
    };
    uint32_t val;
} pmu_backup_wait_target_reg_t;

typedef union {
    struct {
        uint32_t pmu_date: 31;
        uint32_t clk_en  : 1;
    };
    uint32_t val;
} pmu_date_reg_t;

typedef union {
    struct {
        uint32_t reserved0        : 1;
        uint32_t stable_xpd_xtal  : 1;
        uint32_t reserved1        : 13;
        uint32_t sysclk_slp_sel  : 1;
        uint32_t sysclk_sel      : 2;
        uint32_t sysclk_nodiv    : 1;
        uint32_t icg_sysclk_en   : 1;
        uint32_t icg_modem_switch: 1;
        uint32_t icg_modem_code  : 2;
        uint32_t icg_slp_sel     : 1;
        uint32_t icg_global_xtal : 1;
        uint32_t reserved2       : 1;
        uint32_t ana_i2c_iso_en  : 1;
        uint32_t ana_i2c_retention: 1;
        uint32_t ana_xpd_bb_i2c  : 1;
        uint32_t reserved3       : 2;
        uint32_t ana_xpd_xtal    : 1;
    };
    uint32_t val;
} pmu_clk_state0_reg_t;


typedef union {
    struct {
        uint32_t reserved0        : 16;
        uint32_t stable_xpd_pll_state: 4;
        uint32_t icg_global_pll_state: 4;
        uint32_t ana_xpd_pll_i2c_state: 4;
        uint32_t ana_xpd_pll_state: 4;
    };
    uint32_t val;
} pmu_clk_state3_reg_t;

typedef union {
    struct {
        uint32_t reserved0             : 29;
        uint32_t flash_power_ready: 1;
        uint32_t flash_power_sel_done: 1;
        uint32_t stable_vdd_spi_pwr_drv: 1;
    };
    uint32_t val;
} pmu_vdd_spi_status_reg_t;

typedef union {
    struct {
        uint32_t flash_sw_power_sel:1;
        uint32_t flash_power_sel_sw_ctrl:1;
        uint32_t flash_switch_time:10;
        uint32_t flash_stable_time:10;
        uint32_t flash_cooldown_time:10;
    };
    uint32_t val;
} pmu_vdd_spi_cfg_reg_t;

typedef union {
    struct {
        uint32_t flash_discharge_time:10;
        uint32_t reserved0: 22;
    };
    uint32_t val;
} pmu_vdd_spi_discharge_cfg_reg_t;

typedef union {
    struct {
        uint32_t sdio_sw_power_sel:1;
        uint32_t sdio_power_sel_sw_ctrl:1;
        uint32_t sdio_switch_time:10;
        uint32_t reserved0: 20;
    };
    uint32_t val;
} pmu_vdd_sdio_cfg_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t psram_xpd: 1;
    };
    uint32_t val;
} pmu_psram_cfg_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 16;
        uint32_t hpcore1_sw_stall_code: 8;
        uint32_t hpcore0_sw_stall_code: 8;
    };
    uint32_t val;
} pmu_cpu_stall_sw_reg_t;

/* Main PMU Device Structure */
typedef struct pmu_dev_t {
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

    volatile pmu_clk_state0_reg_t            clk_state0;
    volatile uint32_t                        clk_state1;
    volatile uint32_t                        clk_state2;
    volatile pmu_clk_state3_reg_t            clk_state3;
    volatile pmu_vdd_spi_status_reg_t        vdd_spi_status;
    volatile pmu_vdd_spi_cfg_reg_t           vdd_spi_cfg;
    volatile pmu_vdd_spi_discharge_cfg_reg_t vdd_spi_discharge_cfg;
    volatile pmu_vdd_sdio_cfg_reg_t          vdd_sdio_cfg;
    volatile pmu_psram_cfg_reg_t             psram_cfg;
    volatile pmu_cpu_stall_sw_reg_t          cpu_stall_sw;
    volatile pmu_ext_wakeup_lv_reg_t         ext_wakeup_lv;
    volatile pmu_ext_wakeup_sel_reg_t        ext_wakeup_sel;
    volatile pmu_ext_wakeup_st_reg_t         ext_wakeup_st;
    volatile pmu_ext_wakeup_cntl_reg_t       ext_wakeup_cntl;
    volatile pmu_sdio_wakeup_cntl_reg_t      sdio_wakeup_cntl;
    volatile pmu_touch_pwr_ctrl_reg_t        touch_pwr_ctrl;
    volatile pmu_ana_peri_pwr_ctrl_reg_t     ana_peri_pwr_ctrl;
    volatile pmu_rdn_eco_reg_t               rdn_eco;
    volatile pmu_puf_mem_ctrl_reg_t          puf_mem_ctrl;
    volatile pmu_slp_wakeup_cntl8_reg_t      slp_wakeup_cntl8;
    volatile pmu_ext_ldo_ctrl_reg_t          ext_ldo_ctrl;
    volatile pmu_debug_ro_reg_t              debug_ro;
    volatile pmu_lpcpu_wakeup_cause_reg_t    lpcpu_wakeup_cause;
    volatile pmu_backup_wait_target_reg_t    backup_wait_target;
    uint32_t reserved[117];
    volatile pmu_date_reg_t pmu_date;
} pmu_dev_t;

extern pmu_dev_t PMU;

#ifndef __cplusplus
_Static_assert(sizeof(pmu_dev_t) == 0x400, "Invalid size of pmu_dev_t structure");
#endif

_Static_assert(offsetof(pmu_dev_t, reserved) == (PMU_BACKUP_WAIT_TARGET_REG - DR_REG_PMU_BASE) + 4, "Invalid size of pmu_dev_t structure");

#ifdef __cplusplus
}
#endif
