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
/** Type of lpcpu_cfg register
 *  config register for lpcpu power control
 */
typedef union {
    struct {
        /** lpcpu_stall_wait_timer : R/W; bitpos: [7:0]; default: 0;
         *  stall wait timer for lpcpu during lpcpu power off sequence
         */
        uint32_t lpcpu_stall_wait_timer:8;
        /** lpcpu_stall_flag_en : R/W; bitpos: [8]; default: 1;
         *  1: wait for stall rdy input during lpcpu power off sequence
         *  0:do not wait for stall rdy during lpcpu power off sequence
         */
        uint32_t lpcpu_stall_flag_en:1;
        /** lpcpu_stall_en : R/W; bitpos: [9]; default: 1;
         *  1: enable lpcpu stall during lpcpu power off sequence
         *  0: disable lpcpu stall during lpcpu power off sequence
         */
        uint32_t lpcpu_stall_en:1;
        /** lpcpu_reset_en : R/W; bitpos: [10]; default: 1;
         *  1: enable lpcpu reset during lpcpu power off sequence
         *  0: disable lpcpu reset during lpcpu power off sequence
         */
        uint32_t lpcpu_reset_en:1;
        /** lpcpu_slp_waiti_flag_en : R/W; bitpos: [11]; default: 0;
         *  1: validates lp cpu waiti rdy signal during lpcpu power off sequence
         *  0: invalidates lp cpu waiti rdy signal during lpcpu power off sequence
         */
        uint32_t lpcpu_slp_waiti_flag_en:1;
        /** lpcpu_slp_bypass_intr_en : R/W; bitpos: [12]; default: 0;
         *  1: enable interrupt bypass signal for lpcpu during lpcpu power off sequence
         *  0: disable interrupt bypass signal for lpcpu during lpcpu power off sequence
         */
        uint32_t lpcpu_slp_bypass_intr_en:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} lp_pwr_lpcpu_cfg_reg_t;

/** Type of lpcpu_sts register
 *  status register for LPCPU PWR
 */
typedef union {
    struct {
        /** lpcpu_cur_st : RO; bitpos: [3:0]; default: 4;
         *  current power state register for lpcpu
         */
        uint32_t lpcpu_cur_st:4;
        /** lpcpu_in_power_on : RO; bitpos: [4]; default: 0;
         *  1:lpcpu current in power on mode
         *  0:lpcpu not in power on mode
         */
        uint32_t lpcpu_in_power_on:1;
        /** lpcpu_stall_rdy : RO; bitpos: [5]; default: 0;
         *  1:lpcpu stall not ready
         *  0:lpcpu stall ready
         */
        uint32_t lpcpu_stall_rdy:1;
        /** lpcpu_waiti_rdy : RO; bitpos: [6]; default: 0;
         *  1:lpcpu stall waiti ready
         *  0:lpcpu stall waiti ready
         */
        uint32_t lpcpu_waiti_rdy:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_pwr_lpcpu_sts_reg_t;

/** Type of lpcpu_ctrl register
 *  ctrl register for lpcpu power control
 */
typedef union {
    struct {
        /** lpcpu_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for lpcpu
         */
        uint32_t lpcpu_sw_sleep_req:1;
        /** lpcpu_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for lpcpu
         */
        uint32_t lpcpu_sw_wakeup_req:1;
        /** lpcpu_force_stall : R/W; bitpos: [2]; default: 0;
         *  1: force stall lpcpu
         *  0: no change
         */
        uint32_t lpcpu_force_stall:1;
        /** lpcpu_force_reset : R/W; bitpos: [3]; default: 0;
         *  1: force reset lpcpu
         *  0: no change
         */
        uint32_t lpcpu_force_reset:1;
        /** lpcpu_force_no_reset : R/W; bitpos: [4]; default: 0;
         *  1: force no reset lpcpu
         *  0: no change
         */
        uint32_t lpcpu_force_no_reset:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lp_pwr_lpcpu_ctrl_reg_t;

/** Type of lpcpu_wakeup_src register
 *  wakeup source register for lpcpu
 */
typedef union {
    struct {
        /** lpcpu_wakeup_source_en : R/W; bitpos: [30:0]; default: 0;
         *  wakeup source enable signal for lpcpu
         */
        uint32_t lpcpu_wakeup_source_en:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_lpcpu_wakeup_src_reg_t;

/** Type of lpcpu_reject_src register
 *  reject source register for lpcpu
 */
typedef union {
    struct {
        /** lpcpu_reject_source_en : R/W; bitpos: [30:0]; default: 0;
         *  reject source enable signal for lpcpu
         */
        uint32_t lpcpu_reject_source_en:31;
        /** lpcpu_sleep_reject_en : R/W; bitpos: [31]; default: 0;
         *  1: sleep request can be rejected
         *  0: sleep request cannot be rejected
         */
        uint32_t lpcpu_sleep_reject_en:1;
    };
    uint32_t val;
} lp_pwr_lpcpu_reject_src_reg_t;

/** Type of lpcpu_wakeup_cause register
 *  wakeup cause register for lpcpu
 */
typedef union {
    struct {
        /** lpcpu_wakeup_cause : RO; bitpos: [30:0]; default: 0;
         *  wakeup cause for lpcpu
         */
        uint32_t lpcpu_wakeup_cause:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_lpcpu_wakeup_cause_reg_t;

/** Type of lpcpu_reject_cause register
 *  reject cause register for lpcpu
 */
typedef union {
    struct {
        /** lpcpu_reject_cause : RO; bitpos: [30:0]; default: 0;
         *  reject cause for lpcpu
         */
        uint32_t lpcpu_reject_cause:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_lpcpu_reject_cause_reg_t;

/** Type of lpcpu_cause_clr register
 *  cause clear register for lpcpu
 */
typedef union {
    struct {
        /** lpcpu_reject_cause_clr : WT; bitpos: [0]; default: 0;
         *  reject cause clear signal for lpcpu
         */
        uint32_t lpcpu_reject_cause_clr:1;
        /** lpcpu_wakeup_cause_clr : WT; bitpos: [1]; default: 0;
         *  wakeup cause clear signal for lpcpu
         */
        uint32_t lpcpu_wakeup_cause_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_lpcpu_cause_clr_reg_t;

/** Type of appwr_pwr_cfg register
 *  config register for apsys pwr and clk mode
 */
typedef union {
    struct {
        /** cpu_pwr_mode : R/W; bitpos: [1:0]; default: 2;
         *  cpu pwr mode config register for apsys
         */
        uint32_t cpu_pwr_mode:2;
        /** top_pwr_mode : R/W; bitpos: [3:2]; default: 2;
         *  top pwr mode config register for apsys
         */
        uint32_t top_pwr_mode:2;
        /** cnnt_pwr_mode : R/W; bitpos: [5:4]; default: 2;
         *  cnnt pwr mode config register for apsys
         */
        uint32_t cnnt_pwr_mode:2;
        /** hpalive_pwr_mode : R/W; bitpos: [7:6]; default: 2;
         *  hpalive pwr mode config register for apsys
         */
        uint32_t hpalive_pwr_mode:2;
        /** mem_0_pwr_mode : R/W; bitpos: [9:8]; default: 2;
         *  mem_0 pwr mode config register for apsys
         */
        uint32_t mem_0_pwr_mode:2;
        /** mem_1_pwr_mode : R/W; bitpos: [11:10]; default: 2;
         *  mem_1 pwr mode config register for apsys
         */
        uint32_t mem_1_pwr_mode:2;
        /** mem_2_pwr_mode : R/W; bitpos: [13:12]; default: 2;
         *  mem_2 pwr mode config register for apsys
         */
        uint32_t mem_2_pwr_mode:2;
        /** mem_3_pwr_mode : R/W; bitpos: [15:14]; default: 2;
         *  mem_3 pwr mode config register for apsys
         */
        uint32_t mem_3_pwr_mode:2;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_pwr_appwr_pwr_cfg_reg_t;

/** Type of appwr_clk_cfg register
 *  config register for apsys pwr and clk mode
 */
typedef union {
    struct {
        /** cpu_clk_mode : R/W; bitpos: [1:0]; default: 3;
         *  cpu clk mode config register for apsys
         */
        uint32_t cpu_clk_mode:2;
        /** top_clk_mode : R/W; bitpos: [3:2]; default: 3;
         *  top clk mode config register for apsys
         */
        uint32_t top_clk_mode:2;
        /** cnnt_clk_mode : R/W; bitpos: [5:4]; default: 3;
         *  cnnt clk mode config register for apsys
         */
        uint32_t cnnt_clk_mode:2;
        /** hpalive_clk_mode : R/W; bitpos: [7:6]; default: 3;
         *  hpalive clk mode config register for apsys
         */
        uint32_t hpalive_clk_mode:2;
        /** mem_0_clk_mode : R/W; bitpos: [9:8]; default: 3;
         *  mem_0 clk mode config register for apsys
         */
        uint32_t mem_0_clk_mode:2;
        /** mem_1_clk_mode : R/W; bitpos: [11:10]; default: 3;
         *  mem_1 clk mode config register for apsys
         */
        uint32_t mem_1_clk_mode:2;
        /** mem_2_clk_mode : R/W; bitpos: [13:12]; default: 3;
         *  mem_2 clk mode config register for apsys
         */
        uint32_t mem_2_clk_mode:2;
        /** mem_3_clk_mode : R/W; bitpos: [15:14]; default: 3;
         *  mem_3 clk mode config register for apsys
         */
        uint32_t mem_3_clk_mode:2;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_pwr_appwr_clk_cfg_reg_t;

/** Type of appwr_cfg register
 *  config register for apsys pwr
 */
typedef union {
    struct {
        /** stall_wait_timer : R/W; bitpos: [7:0]; default: 0;
         *  the wait timer config for ap sys
         */
        uint32_t stall_wait_timer:8;
        /** backup_en : R/W; bitpos: [8]; default: 0;
         *  the backup enable config register for ap sys
         */
        uint32_t backup_en:1;
        /** restore_mode : R/W; bitpos: [13:9]; default: 0;
         *  restore mode config register for apsys
         */
        uint32_t restore_mode:5;
        /** backup_mode : R/W; bitpos: [18:14]; default: 0;
         *  backup mode config register for apsys
         */
        uint32_t backup_mode:5;
        /** hpcpu_stall_en : R/W; bitpos: [19]; default: 1;
         *  1: enable hpcpu stall
         *  0: disable hpcpu stall
         */
        uint32_t hpcpu_stall_en:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_pwr_appwr_cfg_reg_t;

/** Type of appwr_sts register
 *  status register for apsys pwr
 */
typedef union {
    struct {
        /** appwr_cur_st : RO; bitpos: [3:0]; default: 6;
         *  current power state register for apsys
         */
        uint32_t appwr_cur_st:4;
        /** apsys_in_power_on : RO; bitpos: [4]; default: 0;
         *  1:apsys current in power on mode
         *  0:apsys not in power on mode
         */
        uint32_t apsys_in_power_on:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lp_pwr_appwr_sts_reg_t;

/** Type of appwr_ctrl register
 *  ctrl register for appwr power control
 */
typedef union {
    struct {
        /** appwr_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for appwr
         */
        uint32_t appwr_sw_sleep_req:1;
        /** appwr_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for appwr
         */
        uint32_t appwr_sw_wakeup_req:1;
        /** hpcpu_force_stall : R/W; bitpos: [2]; default: 0;
         *  1: force stall hpcpu
         *  0: no change
         */
        uint32_t hpcpu_force_stall:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_pwr_appwr_ctrl_reg_t;

/** Type of appwr_wakeup_src register
 *  wakeup source register for appwr
 */
typedef union {
    struct {
        /** appwr_wakeup_source_en : R/W; bitpos: [30:0]; default: 0;
         *  wakeup enable signal for appwr
         */
        uint32_t appwr_wakeup_source_en:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_appwr_wakeup_src_reg_t;

/** Type of appwr_reject_src register
 *  reject source register for appwr
 */
typedef union {
    struct {
        /** appwr_reject_source_en : R/W; bitpos: [30:0]; default: 0;
         *  reject enable signal for appwr
         */
        uint32_t appwr_reject_source_en:31;
        /** appwr_sleep_reject_en : R/W; bitpos: [31]; default: 0;
         *  1: sleep request can be rejected
         *  0: sleep request cannot be rejected
         */
        uint32_t appwr_sleep_reject_en:1;
    };
    uint32_t val;
} lp_pwr_appwr_reject_src_reg_t;

/** Type of appwr_wakeup_cause register
 *  wakeup cause register for appwr
 */
typedef union {
    struct {
        /** appwr_wakeup_cause : RO; bitpos: [30:0]; default: 0;
         *  wakeup cause for appwr
         */
        uint32_t appwr_wakeup_cause:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_appwr_wakeup_cause_reg_t;

/** Type of appwr_reject_cause register
 *  reject cause register for appwr
 */
typedef union {
    struct {
        /** appwr_reject_cause : RO; bitpos: [30:0]; default: 0;
         *  reject cause for appwr
         */
        uint32_t appwr_reject_cause:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_appwr_reject_cause_reg_t;

/** Type of appwr_cause_clr register
 *  cause clear register for appwr
 */
typedef union {
    struct {
        /** appwr_reject_cause_clr : WT; bitpos: [0]; default: 0;
         *  reject cause clear signal for appwr
         */
        uint32_t appwr_reject_cause_clr:1;
        /** appwr_wakeup_cause_clr : WT; bitpos: [1]; default: 0;
         *  wakeup cause clear signal for appwr
         */
        uint32_t appwr_wakeup_cause_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_appwr_cause_clr_reg_t;

/** Type of wifi_ctrl register
 *  ctrl register for wifi power control
 */
typedef union {
    struct {
        /** wifi_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for wifi
         */
        uint32_t wifi_sw_sleep_req:1;
        /** wifi_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for wifi
         */
        uint32_t wifi_sw_wakeup_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_wifi_ctrl_reg_t;

/** Type of wifi_wakeup_src register
 *  wakeup source register for wifi
 */
typedef union {
    struct {
        /** wifi_wakeup_source_en : R/W; bitpos: [30:0]; default: 0;
         *  wakeup enable signal for wifi
         */
        uint32_t wifi_wakeup_source_en:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_wifi_wakeup_src_reg_t;

/** Type of ble_ctrl register
 *  ctrl register for ble power control
 */
typedef union {
    struct {
        /** ble_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for ble
         */
        uint32_t ble_sw_sleep_req:1;
        /** ble_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for ble
         */
        uint32_t ble_sw_wakeup_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_ble_ctrl_reg_t;

/** Type of ble_wakeup_src register
 *  wakeup source register for ble
 */
typedef union {
    struct {
        /** ble_wakeup_source_en : R/W; bitpos: [30:0]; default: 0;
         *  wakeup enable signal for ble
         */
        uint32_t ble_wakeup_source_en:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_ble_wakeup_src_reg_t;

/** Type of zb_ctrl register
 *  ctrl register for zb power control
 */
typedef union {
    struct {
        /** zb_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for zb
         */
        uint32_t zb_sw_sleep_req:1;
        /** zb_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for zb
         */
        uint32_t zb_sw_wakeup_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_zb_ctrl_reg_t;

/** Type of zb_wakeup_src register
 *  wakeup source register for zb
 */
typedef union {
    struct {
        /** zb_wakeup_source_en : R/W; bitpos: [30:0]; default: 0;
         *  wakeup enazb signal for zb
         */
        uint32_t zb_wakeup_source_en:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_zb_wakeup_src_reg_t;

/** Type of modempwr_ctrl register
 *  ctrl register for modempwr power control
 */
typedef union {
    struct {
        /** modempwr_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for modempwr
         */
        uint32_t modempwr_sw_sleep_req:1;
        /** modempwr_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for modempwr
         */
        uint32_t modempwr_sw_wakeup_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_modempwr_ctrl_reg_t;

/** Type of modempwr_wakeup_src register
 *  wakeup source register for modempwr
 */
typedef union {
    struct {
        /** modempwr_wakeup_source_en : R/W; bitpos: [30:0]; default: 0;
         *  wakeup enable signal for modempwr
         */
        uint32_t modempwr_wakeup_source_en:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_pwr_modempwr_wakeup_src_reg_t;

/** Type of peri_pad_wake_slp_en register
 *  used for future potential eco, others don't care
 */
typedef union {
    struct {
        /** uart_wakeup_en : R/W; bitpos: [0]; default: 0;
         *  1:enables uart wakeup after hpcpu stall
         *  0:disables uart wakeup after hpcpu stall
         */
        uint32_t uart_wakeup_en:1;
        /** hp_pad_slp_sel : R/W; bitpos: [1]; default: 0;
         *  1:hppad will sleep after hpcpu stall
         *  0:hppad will not sleep after hpcpu stall
         */
        uint32_t hp_pad_slp_sel:1;
        /** lp_pad_slp_sel : R/W; bitpos: [2]; default: 0;
         *  1:hppad will sleep after hpcpu stall
         *  0:hppad will not sleep after hpcpu stall
         */
        uint32_t lp_pad_slp_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_pwr_peri_pad_wake_slp_en_reg_t;

/** Type of touch_ctrl register
 *  ctrl register for touch power control
 */
typedef union {
    struct {
        /** touch_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for touch
         */
        uint32_t touch_sw_sleep_req:1;
        /** touch_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for touch
         */
        uint32_t touch_sw_wakeup_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_touch_ctrl_reg_t;

/** Type of touch_cfg register
 *  ctrl register for touch power control
 */
typedef union {
    struct {
        /** touch_sleep_cycles : R/W; bitpos: [25:0]; default: 100;
         *  sleep cycles config for touch
         */
        uint32_t touch_sleep_cycles:26;
        /** touch_sleep_timer_en : R/W; bitpos: [26]; default: 0;
         *  1:enables touch timer wakeup source
         *  0:disables touch timer wakeup source
         */
        uint32_t touch_sleep_timer_en:1;
        /** touch_force_done : R/W; bitpos: [27]; default: 0;
         *  1: force touch task to done state, turn off touch
         *  0: no effect
         */
        uint32_t touch_force_done:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_pwr_touch_cfg_reg_t;

/** Type of touch_sts register
 *  status register for TOUCH PWR
 */
typedef union {
    struct {
        /** touch_cur_st : RO; bitpos: [2:0]; default: 4;
         *  current power state register for touch
         */
        uint32_t touch_cur_st:3;
        /** touch_in_power_on : RO; bitpos: [3]; default: 0;
         *  1:touch current in power on mode
         *  0:touch not in power on mode
         */
        uint32_t touch_in_power_on:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_pwr_touch_sts_reg_t;

/** Type of cali_ctrl register
 *  ctrl register for cali power control
 */
typedef union {
    struct {
        /** cali_sw_sleep_req : WT; bitpos: [0]; default: 0;
         *  software sleep request config for cali
         */
        uint32_t cali_sw_sleep_req:1;
        /** cali_sw_wakeup_req : WT; bitpos: [1]; default: 0;
         *  software wakeup request config for cali
         */
        uint32_t cali_sw_wakeup_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_cali_ctrl_reg_t;

/** Type of cali_sts register
 *  status register for CALI PWR
 */
typedef union {
    struct {
        /** cali_cur_st : RO; bitpos: [2:0]; default: 4;
         *  current power state register for cali
         */
        uint32_t cali_cur_st:3;
        /** cali_in_power_on : RO; bitpos: [3]; default: 0;
         *  1:cali current in power on mode
         *  0:cali not in power on mode
         */
        uint32_t cali_in_power_on:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_pwr_cali_sts_reg_t;

/** Type of lpperi_pwr_clk_cfg register
 *  ctrl register for lpcpu power clock mode control
 */
typedef union {
    struct {
        /** lpperi_sw_clk_mode : R/W; bitpos: [1:0]; default: 3;
         *  software configs LPPERI clk mode
         */
        uint32_t lpperi_sw_clk_mode:2;
        /** lpperi_sw_pwr_mode : R/W; bitpos: [3:2]; default: 2;
         *  software configs LPPERI pwr mode
         */
        uint32_t lpperi_sw_pwr_mode:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_pwr_lpperi_pwr_clk_cfg_reg_t;

/** Type of lpperi_ctrl register
 *  ctrl register for lpperi power control
 */
typedef union {
    struct {
        /** lpperi_sw_force_on : R/W; bitpos: [0]; default: 1;
         *  1: software wake lpperi
         */
        uint32_t lpperi_sw_force_on:1;
        /** lpperi_sw_req : WT; bitpos: [1]; default: 0;
         *  software req  pulse for following mode register
         */
        uint32_t lpperi_sw_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_pwr_lpperi_ctrl_reg_t;

/** Type of lp_pad_cfg register
 *  config register for lppad hold signal
 */
typedef union {
    struct {
        /** lppad_hold_cpu_pause_en : R/W; bitpos: [0]; default: 0;
         *  1: hold lppad when hpcpu stalls
         *  0: not hold lppad when hpcpu stalls
         */
        uint32_t lppad_hold_cpu_pause_en:1;
        /** force_high_lp_pad_hold : R/W; bitpos: [1]; default: 0;
         *  1:force pull up lppad hold signal
         *  0:no effect
         */
        uint32_t force_high_lp_pad_hold:1;
        /** force_low_lp_pad_hold : R/W; bitpos: [2]; default: 0;
         *  1:force pull low lppad hold signal
         *  0:no effect
         */
        uint32_t force_low_lp_pad_hold:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_pwr_lp_pad_cfg_reg_t;

/** Type of cnnt_pad_cfg register
 *  config register for cnntpad hold signal
 */
typedef union {
    struct {
        /** cnnt_pad_hold_cpu_pause_en : R/W; bitpos: [0]; default: 0;
         *  1: hold cnntpad when hpcpu stalls
         *  0: not hold cnntpad when hpcpu stalls
         */
        uint32_t cnnt_pad_hold_cpu_pause_en:1;
        /** force_high_cnnt_pad_hold : R/W; bitpos: [1]; default: 0;
         *  1:force pull up cnntpad hold signal
         *  0:no effect
         */
        uint32_t force_high_cnnt_pad_hold:1;
        /** force_low_cnnt_pad_hold : R/W; bitpos: [2]; default: 0;
         *  1:force pull low cnntpad hold signal
         *  0:no effect
         */
        uint32_t force_low_cnnt_pad_hold:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_pwr_cnnt_pad_cfg_reg_t;

/** Type of hp_pad_cfg register
 *  config register for hppad hold signal
 */
typedef union {
    struct {
        /** hppad_hold_cpu_pause_en : R/W; bitpos: [0]; default: 0;
         *  1: hold hppad when hpcpu stalls
         *  0: not hold hppad when hpcpu stalls
         */
        uint32_t hppad_hold_cpu_pause_en:1;
        /** force_high_hp_pad_hold : R/W; bitpos: [1]; default: 0;
         *  1:force pull up hppad hold signal
         *  0:no effect
         */
        uint32_t force_high_hp_pad_hold:1;
        /** force_low_hp_pad_hold : R/W; bitpos: [2]; default: 0;
         *  1:force pull low hppad hold signal
         *  0:no effect
         */
        uint32_t force_low_hp_pad_hold:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_pwr_hp_pad_cfg_reg_t;

/** Type of int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** zb_wake_process_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t zb_wake_process_int_raw:1;
        /** modempwr_wake_process_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t modempwr_wake_process_int_raw:1;
        /** ble_wake_process_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t ble_wake_process_int_raw:1;
        /** wifi_wake_process_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t wifi_wake_process_int_raw:1;
        /** xtal_cali_wake_process_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t xtal_cali_wake_process_int_raw:1;
        /** touch_wake_process_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t touch_wake_process_int_raw:1;
        /** lpcpu_wake_process_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lpcpu_wake_process_int_raw:1;
        /** appwr_wake_process_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t appwr_wake_process_int_raw:1;
    };
    uint32_t val;
} lp_pwr_int_raw_reg_t;

/** Type of int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** zb_wake_process_int_st : RO; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t zb_wake_process_int_st:1;
        /** modempwr_wake_process_int_st : RO; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t modempwr_wake_process_int_st:1;
        /** ble_wake_process_int_st : RO; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t ble_wake_process_int_st:1;
        /** wifi_wake_process_int_st : RO; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t wifi_wake_process_int_st:1;
        /** xtal_cali_wake_process_int_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t xtal_cali_wake_process_int_st:1;
        /** touch_wake_process_int_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t touch_wake_process_int_st:1;
        /** lpcpu_wake_process_int_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lpcpu_wake_process_int_st:1;
        /** appwr_wake_process_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t appwr_wake_process_int_st:1;
    };
    uint32_t val;
} lp_pwr_int_st_reg_t;

/** Type of int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** zb_wake_process_int_ena : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t zb_wake_process_int_ena:1;
        /** modempwr_wake_process_int_ena : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t modempwr_wake_process_int_ena:1;
        /** ble_wake_process_int_ena : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t ble_wake_process_int_ena:1;
        /** wifi_wake_process_int_ena : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t wifi_wake_process_int_ena:1;
        /** xtal_cali_wake_process_int_ena : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t xtal_cali_wake_process_int_ena:1;
        /** touch_wake_process_int_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t touch_wake_process_int_ena:1;
        /** lpcpu_wake_process_int_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lpcpu_wake_process_int_ena:1;
        /** appwr_wake_process_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t appwr_wake_process_int_ena:1;
    };
    uint32_t val;
} lp_pwr_int_ena_reg_t;

/** Type of int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** zb_wake_process_int_clr : WT; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t zb_wake_process_int_clr:1;
        /** modempwr_wake_process_int_clr : WT; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t modempwr_wake_process_int_clr:1;
        /** ble_wake_process_int_clr : WT; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t ble_wake_process_int_clr:1;
        /** wifi_wake_process_int_clr : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t wifi_wake_process_int_clr:1;
        /** xtal_cali_wake_process_int_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t xtal_cali_wake_process_int_clr:1;
        /** touch_wake_process_int_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t touch_wake_process_int_clr:1;
        /** lpcpu_wake_process_int_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lpcpu_wake_process_int_clr:1;
        /** appwr_wake_process_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t appwr_wake_process_int_clr:1;
    };
    uint32_t val;
} lp_pwr_int_clr_reg_t;

/** Type of ver_date register
 *  need_des
 */
typedef union {
    struct {
        /** ver_date : R/W; bitpos: [30:0]; default: 2426648;
         *  need_des
         */
        uint32_t ver_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_pwr_ver_date_reg_t;


typedef struct {
    volatile lp_pwr_lpcpu_cfg_reg_t lpcpu_cfg;
    volatile lp_pwr_lpcpu_sts_reg_t lpcpu_sts;
    volatile lp_pwr_lpcpu_ctrl_reg_t lpcpu_ctrl;
    volatile lp_pwr_lpcpu_wakeup_src_reg_t lpcpu_wakeup_src;
    volatile lp_pwr_lpcpu_reject_src_reg_t lpcpu_reject_src;
    volatile lp_pwr_lpcpu_wakeup_cause_reg_t lpcpu_wakeup_cause;
    volatile lp_pwr_lpcpu_reject_cause_reg_t lpcpu_reject_cause;
    volatile lp_pwr_lpcpu_cause_clr_reg_t lpcpu_cause_clr;
    volatile lp_pwr_appwr_pwr_cfg_reg_t appwr_pwr_cfg;
    volatile lp_pwr_appwr_clk_cfg_reg_t appwr_clk_cfg;
    volatile lp_pwr_appwr_cfg_reg_t appwr_cfg;
    volatile lp_pwr_appwr_sts_reg_t appwr_sts;
    volatile lp_pwr_appwr_ctrl_reg_t appwr_ctrl;
    volatile lp_pwr_appwr_wakeup_src_reg_t appwr_wakeup_src;
    volatile lp_pwr_appwr_reject_src_reg_t appwr_reject_src;
    volatile lp_pwr_appwr_wakeup_cause_reg_t appwr_wakeup_cause;
    volatile lp_pwr_appwr_reject_cause_reg_t appwr_reject_cause;
    volatile lp_pwr_appwr_cause_clr_reg_t appwr_cause_clr;
    volatile lp_pwr_wifi_ctrl_reg_t wifi_ctrl;
    volatile lp_pwr_wifi_wakeup_src_reg_t wifi_wakeup_src;
    volatile lp_pwr_ble_ctrl_reg_t ble_ctrl;
    volatile lp_pwr_ble_wakeup_src_reg_t ble_wakeup_src;
    volatile lp_pwr_zb_ctrl_reg_t zb_ctrl;
    volatile lp_pwr_zb_wakeup_src_reg_t zb_wakeup_src;
    volatile lp_pwr_modempwr_ctrl_reg_t modempwr_ctrl;
    volatile lp_pwr_modempwr_wakeup_src_reg_t modempwr_wakeup_src;
    volatile lp_pwr_peri_pad_wake_slp_en_reg_t peri_pad_wake_slp_en;
    volatile lp_pwr_touch_ctrl_reg_t touch_ctrl;
    volatile lp_pwr_touch_cfg_reg_t touch_cfg;
    volatile lp_pwr_touch_sts_reg_t touch_sts;
    volatile lp_pwr_cali_ctrl_reg_t cali_ctrl;
    volatile lp_pwr_cali_sts_reg_t cali_sts;
    volatile lp_pwr_lpperi_pwr_clk_cfg_reg_t lpperi_pwr_clk_cfg;
    volatile lp_pwr_lpperi_ctrl_reg_t lpperi_ctrl;
    volatile lp_pwr_lp_pad_cfg_reg_t lp_pad_cfg;
    volatile lp_pwr_cnnt_pad_cfg_reg_t cnnt_pad_cfg;
    volatile lp_pwr_hp_pad_cfg_reg_t hp_pad_cfg;
    volatile lp_pwr_int_raw_reg_t int_raw;
    volatile lp_pwr_int_st_reg_t int_st;
    volatile lp_pwr_int_ena_reg_t int_ena;
    volatile lp_pwr_int_clr_reg_t int_clr;
    volatile lp_pwr_ver_date_reg_t ver_date;
} lp_pwr_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_pwr_dev_t) == 0xa8, "Invalid size of lp_pwr_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
