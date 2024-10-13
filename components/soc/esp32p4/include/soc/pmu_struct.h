/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/pmu_reg.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    struct {
        uint32_t reserved0    : 21;
        uint32_t dcdc_switch_pd_en :1;
        uint32_t mem_dslp     : 1;
        uint32_t mem_pd_en    : 1;
        uint32_t reserved1    : 6;
        uint32_t cnnt_pd_en   : 1;
        uint32_t top_pd_en    : 1;
    };
    uint32_t val;
} pmu_hp_dig_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0      : 23;
        uint32_t power_det_bypass : 1;
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
        uint32_t reserved0    : 21;
        uint32_t i2c_iso_en   : 1;
        uint32_t i2c_retention: 1;
        uint32_t xpd_pll_i2c  : 4;
        uint32_t xpd_pll      : 4;
        uint32_t reserved1    : 1;
    };
    uint32_t val;
} pmu_hp_clk_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0 : 18;
        uint32_t dcm_vset  : 5;
        uint32_t dcm_mode  : 2;
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
        uint32_t reserved6                            : 32;
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
        uint32_t reserved0: 26;
        uint32_t drv_b    : 6;
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

typedef struct pmu_hp_hw_regmap_t {
    pmu_hp_dig_power_reg_t     dig_power;
    uint32_t                   icg_func;
    uint32_t                   icg_apb;
    uint32_t                   icg_modem;
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

typedef union {
    struct {
        uint32_t reserved0: 21;
        uint32_t slp_xpd  : 1;
        uint32_t xpd      : 1;
        uint32_t slp_dbias: 4;
        uint32_t dbias    : 5;
    };
    uint32_t val;
} pmu_lp_regulator0_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 26;
        uint32_t drv_b    : 6;
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
        uint32_t reserved0 : 26;
        uint32_t lp_pad_slp_sel : 1;
        uint32_t bod_source_sel : 1;
        uint32_t vddbat_mode : 2;
        uint32_t mem_dslp  : 1;
        uint32_t peri_pd_en: 1;
    };
    uint32_t val;
} pmu_lp_dig_power_reg_t;

typedef union {
    struct {
        uint32_t reserved0  : 27;
        uint32_t xpd_lppll  : 1;
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

typedef struct pmu_lp_hw_regmap_t {
    pmu_lp_regulator0_reg_t    regulator0;
    pmu_lp_regulator1_reg_t    regulator1;
    pmu_lp_xtal_reg_t          xtal;   /* Only LP_SLEEP mode under lp system is valid */
    pmu_lp_dig_power_reg_t     dig_power;
    pmu_lp_clk_power_reg_t     clk_power;
    pmu_lp_bias_reg_t          bias;   /* Only LP_SLEEP mode under lp system is valid */
} pmu_lp_hw_regmap_t;


typedef union {
    struct {
        uint32_t tie_low_cali_xtal_icg      : 1;
        uint32_t tie_low_global_pll_icg     : 4;
        uint32_t tie_low_global_xtal_icg    : 1;
        uint32_t tie_low_i2c_retention      : 1;
        uint32_t tie_low_xpd_pll_i2c        : 4;
        uint32_t tie_low_xpd_pll            : 4;
        uint32_t tie_low_xpd_xtal           : 1;
        uint32_t tie_high_cali_xtal_icg     : 1;
        uint32_t tie_high_global_pll_icg    : 4;
        uint32_t tie_high_global_xtal_icg   : 1;
        uint32_t tie_high_i2c_retention     : 1;
        uint32_t tie_high_xpd_pll_i2c       : 4;
        uint32_t tie_high_xpd_pll           : 4;
        uint32_t tie_high_xpd_xtal          : 1;
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
        uint32_t pad_slp_sel             : 1;
        uint32_t lp_pad_hold_all         : 1;
        uint32_t hp_pad_hold_all         : 1;
        uint32_t reserved0               : 23;
        uint32_t tie_high_pad_slp_sel    : 1;
        uint32_t tie_low_pad_slp_sel     : 1;
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

typedef struct pmu_imm_hw_regmap_t {
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
        uint32_t reserved0      : 5;
        uint32_t powerdown_timer: 9;
        uint32_t powerup_timer  : 9;
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
        uint32_t reserved0     : 26;    /* Invalid of lp peripherals */
    };
    uint32_t val;
} pmu_power_domain_cntl_reg_t;

typedef union {
    struct {
        uint32_t pd_top_mask   : 5;
        uint32_t reserved0     : 22;    /* Invalid of lp peripherals */
        uint32_t top_pd_mask   : 5;
    };
    uint32_t val;
} pmu_power_domain_mask_reg_t;

typedef union {
    struct {
        uint32_t force_pu      :    1;
        uint32_t force_pd      :    1;
        uint32_t reserved2     :    30;
    };
    uint32_t val;
} pmu_power_dcdc_switch_reg_t;

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
        uint32_t wait_xtal_stable: 16;
        uint32_t wait_pll_stable : 16;
    };
    uint32_t val;
} pmu_power_clk_wait_cntl_reg_t;

typedef struct pmu_power_hw_regmap_t {
    pmu_power_wait_timer0_reg_t    wait_timer0;
    pmu_power_wait_timer1_reg_t    wait_timer1;
    pmu_power_domain_cntl_reg_t    hp_pd[3];
    pmu_power_domain_mask_reg_t    hp_pd_mask[3];
    pmu_power_dcdc_switch_reg_t    dcdc_switch;
    pmu_power_domain_cntl_reg_t    lp_peri;
    pmu_power_domain_mask_reg_t    lp_peri_mask;
    pmu_power_hp_pad_reg_t         hp_pad;
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
        uint32_t wakeup_ena: 31;
        uint32_t reserved0 : 1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl2_reg_t;

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

typedef union {
    struct {
        uint32_t reserved0          : 31;
        uint32_t lp_lite_wakeup_ena : 1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl8_reg_t;

typedef struct pmu_wakeup_hw_regmap_t {
    pmu_slp_wakeup_cntl0_reg_t     cntl0;
    pmu_slp_wakeup_cntl1_reg_t     cntl1;
    pmu_slp_wakeup_cntl2_reg_t     cntl2;
    pmu_slp_wakeup_cntl3_reg_t     cntl3;
    pmu_slp_wakeup_cntl4_reg_t     cntl4;
    pmu_slp_wakeup_cntl5_reg_t     cntl5;
    pmu_slp_wakeup_cntl6_reg_t     cntl6;
    pmu_slp_wakeup_cntl7_reg_t     cntl7;
    pmu_slp_wakeup_cntl8_reg_t     cntl8;
    uint32_t                       status0;
    uint32_t                       status1;
    uint32_t                       status2;
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
        uint32_t reserved0     : 24;
        uint32_t mspi_phy_xpd  : 1;
        uint32_t sdio_pll_xpd  : 1;
        uint32_t perif_i2c_rstb: 1;
        uint32_t xpd_perif_i2c : 1;
        uint32_t xpd_txrf_i2c  : 1;
        uint32_t xpd_rfrx_pbus : 1;
        uint32_t xpd_ckgen_i2c : 1;
        uint32_t reserved1     : 1;
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
        uint32_t reserved0   : 14;
        uint32_t pmu_0p1a_cnt_target0_reach_0 : 1;
        uint32_t pmu_0p1a_cnt_target1_reach_0 : 1;
        uint32_t pmu_0p1a_cnt_target0_reach_1 : 1;
        uint32_t pmu_0p1a_cnt_target1_reach_1 : 1;
        uint32_t pmu_0p2a_cnt_target0_reach_0 : 1;
        uint32_t pmu_0p2a_cnt_target1_reach_0 : 1;
        uint32_t pmu_0p2a_cnt_target0_reach_1 : 1;
        uint32_t pmu_0p2a_cnt_target1_reach_1 : 1;
        uint32_t pmu_0p3a_cnt_target0_reach_0 : 1;
        uint32_t pmu_0p3a_cnt_target1_reach_0 : 1;
        uint32_t pmu_0p3a_cnt_target0_reach_1 : 1;
        uint32_t pmu_0p3a_cnt_target1_reach_1 : 1;
        uint32_t reserved1   : 1;
        uint32_t lp_exception: 1;
        uint32_t sdio_idle: 1;
        uint32_t sw       : 1;
        uint32_t reject   : 1;
        uint32_t wakeup   : 1;
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

typedef union {
    struct {
        uint32_t reserved0                    : 13;
        uint32_t sleep_reject                 : 1;
        uint32_t pmu_0p1a_cnt_target0_reach_0 : 1;
        uint32_t pmu_0p1a_cnt_target1_reach_0 : 1;
        uint32_t pmu_0p1a_cnt_target0_reach_1 : 1;
        uint32_t pmu_0p1a_cnt_target1_reach_1 : 1;
        uint32_t pmu_0p2a_cnt_target0_reach_0 : 1;
        uint32_t pmu_0p2a_cnt_target1_reach_0 : 1;
        uint32_t pmu_0p2a_cnt_target0_reach_1 : 1;
        uint32_t pmu_0p2a_cnt_target1_reach_1 : 1;
        uint32_t pmu_0p3a_cnt_target0_reach_0 : 1;
        uint32_t pmu_0p3a_cnt_target1_reach_0 : 1;
        uint32_t pmu_0p3a_cnt_target0_reach_1 : 1;
        uint32_t pmu_0p3a_cnt_target1_reach_1 : 1;
        uint32_t lp_wakeup                    : 1;
        uint32_t sleep_switch_active_end      : 1;
        uint32_t active_switch_sleep_end      : 1;
        uint32_t sleep_switch_active_start    : 1;
        uint32_t active_switch_sleep_start    : 1;
        uint32_t hp_sw_trigger                : 1;
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
        uint32_t wakeup_en: 31;
        uint32_t reserved0: 1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr2_reg_t;

typedef union {
    struct {
        uint32_t wakeup_cause: 31;
        uint32_t reserved0: 1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr3_reg_t;

typedef union {
    struct {
        uint32_t sleep_reject: 31;
        uint32_t reserved0: 1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr4_reg_t;

typedef union {
    struct {
        uint32_t sleep_reject_cause: 31;
        uint32_t reserved0: 1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr5_reg_t;

typedef struct pmu_lp_ext_hw_regmap_t {
    pmu_lp_intr_reg_t          int_raw;
    pmu_lp_intr_reg_t          int_st;
    pmu_lp_intr_reg_t          int_ena;
    pmu_lp_intr_reg_t          int_clr;
    pmu_lp_cpu_pwr0_reg_t      pwr0;
    pmu_lp_cpu_pwr1_reg_t      pwr1;
    pmu_lp_cpu_pwr2_reg_t      pwr2;
    pmu_lp_cpu_pwr3_reg_t      pwr3;
    pmu_lp_cpu_pwr4_reg_t      pwr4;
    pmu_lp_cpu_pwr5_reg_t      pwr5;
} pmu_lp_ext_hw_regmap_t;

typedef union {
    struct {
        uint32_t reserved_0:7;
        uint32_t force_tieh_sel:1;
        uint32_t xpd:1;
        uint32_t tieh_sel:3;
        uint32_t tieh_pos_en:1;
        uint32_t tieh_neg_en:1;
        uint32_t tieh:1;
        uint32_t target1:8;
        uint32_t target0:8;
        uint32_t ldo_cnt_prescaler_sel:1;
    };
    uint32_t val;
} pmu_ext_ldo_reg_t;

typedef union {
    struct {
        uint32_t reserved_0:23;
        uint32_t mul:3;
        uint32_t en_vdet:1;
        uint32_t en_cur_lim:1;
        uint32_t dref:4;
    };
    uint32_t val;
} pmu_ext_ldo_ana_reg_t;


typedef struct pmu_ext_ldo_info_t {
    pmu_ext_ldo_reg_t pmu_ext_ldo;
    pmu_ext_ldo_ana_reg_t pmu_ext_ldo_ana;
} pmu_ext_ldo_info_t;


typedef union {
    struct {
        uint32_t on_req        : 1;
        uint32_t off_req       : 1;
        uint32_t lightslp_req  : 1;
        uint32_t deepslp_req   : 1;
        uint32_t reserved0     : 3;
        uint32_t done_force    : 1;
        uint32_t on_force_pu   : 1;
        uint32_t on_force_pd   : 1;
        uint32_t fb_res_force_pu   : 1;
        uint32_t fb_res_force_pd   : 1;
        uint32_t ls_force_pu   : 1;
        uint32_t ls_force_pd   : 1;
        uint32_t ds_force_pu   : 1;
        uint32_t ds_force_pd   : 1;
        uint32_t dcm_cur_st    : 8;
        uint32_t reserved1     : 5;
        uint32_t en_amux_test  : 1;
        uint32_t reserved2     : 2;
    };
    uint32_t val;
} pmu_dcm_ctrl_reg_t;

typedef union {
    struct {
        uint32_t pre_delay      : 8;
        uint32_t res_off_delay  : 8;
        uint32_t stable_delay   : 10;
        uint32_t reserved0      : 6;
    };
    uint32_t val;
} pmu_dcm_wait_delay_t;

typedef union {
    struct {
        uint32_t module     : 2;
        uint32_t reserved1  : 29;
        uint32_t sw_update  : 1;
    };
    uint32_t val;
} pmu_vddbat_cfg_t;

typedef union {
    struct {
        uint32_t reserved0      : 5;
        uint32_t wait_cycles    : 9;
        uint32_t sleep_cycles   : 16;
        uint32_t force_done     : 1;
        uint32_t sleep_timer_en : 1;
    };
    uint32_t val;
} pmu_touch_sensor_pwr_cntl_t;

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
            volatile uint32_t reserved0    : 30;
            volatile uint32_t lp_trigger_hp: 1;
            volatile uint32_t hp_trigger_lp: 1;
        };
        volatile uint32_t val;
    } hp_lp_cpu_comm;

    union {
        struct {
            volatile uint32_t reserved0           : 31;
            volatile uint32_t dig_regulator_en_cal: 1;
        };
        volatile uint32_t val;
    } hp_regulator_cfg;

    union {
        struct {
            volatile uint32_t en_cali_pmu_cntl : 1;
            volatile uint32_t reserved0 : 10;
            volatile uint32_t last_st   : 7;
            volatile uint32_t target_st : 7;
            volatile uint32_t current_st: 7;
        };
        volatile uint32_t val;
    } main_state;

    union {
        struct {
            volatile uint32_t reserved0: 13;
            volatile uint32_t backup_st: 5;
            volatile uint32_t lp_pwr_st: 5;
            volatile uint32_t hp_pwr_st: 9;
        };
        volatile int32_t val;
    } pwr_state;

    union {
        struct {
            volatile uint32_t stable_xpd_bbpll : 3;
            volatile uint32_t stable_xpd_xtal  : 1;
            volatile uint32_t ana_xpd_pll_i2c  : 3;
            volatile uint32_t reserved0        : 3;
            volatile uint32_t sysclk_slp_sel   : 1;
            volatile uint32_t sysclk_sel       : 2;
            volatile uint32_t sysclk_nodiv     : 1;
            volatile uint32_t icg_sysclk_en    : 1;
            volatile uint32_t icg_modem_switch : 1;
            volatile uint32_t icg_modem_code   : 2;
            volatile uint32_t icg_slp_sel      : 1;
            volatile uint32_t icg_global_xtal  : 1;
            volatile uint32_t icg_global_pll   : 4;
            volatile uint32_t ana_i2c_iso_en   : 1;
            volatile uint32_t ana_i2c_retention: 1;
            volatile uint32_t reserved1        : 1;
            volatile uint32_t ana_xpd_pll      : 4;
            volatile uint32_t ana_xpd_xtal     : 1;
        };
        volatile uint32_t val;
    } clk_state0;

    volatile uint32_t clk_state1;
    volatile uint32_t clk_state2;

    volatile pmu_ext_ldo_info_t ext_ldo[6];

    volatile uint32_t ext_wakeup_lv;
    volatile uint32_t ext_wakeup_sel;
    volatile uint32_t ext_wakeup_st;
    union {
        struct {
            volatile uint32_t reserved0      : 30;
            volatile uint32_t status_clr     : 1;
            volatile uint32_t filter         : 1;
        };
        volatile uint32_t val;
    } ext_wakeup_cntl;

    union {
        struct {
            volatile uint32_t act_dnum   : 10;
            volatile uint32_t reserved0  : 22;
        };
        volatile uint32_t val;
    } sdio_wakeup_cntl;

    union {
        struct {
            volatile uint32_t reserved0  : 16;
            volatile uint32_t cnt_target : 16;
        };
        volatile uint32_t val;
    } xtal_slp;

    union {
        struct {
            volatile uint32_t reserved0  : 16;
            volatile uint32_t hpcore1_stall_code : 8;
            volatile uint32_t hpcore0_stall_code : 8;
        };
        volatile uint32_t val;
    } cpu_sw_stall;

    volatile pmu_dcm_ctrl_reg_t dcm_ctrl;
    volatile pmu_dcm_wait_delay_t dcm_delay;
    volatile pmu_vddbat_cfg_t vbat_cfg;
    volatile pmu_touch_sensor_pwr_cntl_t touch_pwr_cntl;

    union {
        struct {
            volatile uint32_t eco_result:1;
            volatile uint32_t reserved0  : 30;
            volatile uint32_t eco_en: 1;

        };
        volatile uint32_t val;
    } pmu_rdn_eco;


    uint32_t reserved[121];

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

_Static_assert(offsetof(pmu_dev_t, reserved) == (PMU_RDN_ECO_REG - DR_REG_PMU_BASE) + 4, "Invalid size of pmu_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
