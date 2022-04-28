/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of rtc_options0 register
 *  register description
 */
typedef union {
    struct {
        /** sw_stall_appcpu_c0 : R/W; bitpos: [1:0]; default: 0;
         *  {reg_sw_stall_appcpu_c1[5:0],  reg_sw_stall_appcpu_c0[1:0]} == 0x86 will stall APP
         *  CPU
         */
        uint32_t sw_stall_appcpu_c0:2;
        /** sw_stall_procpu_c0 : R/W; bitpos: [3:2]; default: 0;
         *  {reg_sw_stall_procpu_c1[5:0],  reg_sw_stall_procpu_c0[1:0]} == 0x86 will stall PRO
         *  CPU
         */
        uint32_t sw_stall_procpu_c0:2;
        /** sw_appcpu_rst : WO; bitpos: [4]; default: 0;
         *  APP CPU SW reset
         */
        uint32_t sw_appcpu_rst:1;
        /** sw_procpu_rst : WO; bitpos: [5]; default: 0;
         *  PRO CPU SW reset
         */
        uint32_t sw_procpu_rst:1;
        /** bb_i2c_force_pd : R/W; bitpos: [6]; default: 0;
         *  BB_I2C force power down
         */
        uint32_t bb_i2c_force_pd:1;
        /** bb_i2c_force_pu : R/W; bitpos: [7]; default: 0;
         *  BB_I2C force power up
         */
        uint32_t bb_i2c_force_pu:1;
        /** bbpll_i2c_force_pd : R/W; bitpos: [8]; default: 0;
         *  BB_PLL _I2C force power down
         */
        uint32_t bbpll_i2c_force_pd:1;
        /** bbpll_i2c_force_pu : R/W; bitpos: [9]; default: 0;
         *  BB_PLL_I2C force power up
         */
        uint32_t bbpll_i2c_force_pu:1;
        /** bbpll_force_pd : R/W; bitpos: [10]; default: 0;
         *  BB_PLL force power down
         */
        uint32_t bbpll_force_pd:1;
        /** bbpll_force_pu : R/W; bitpos: [11]; default: 0;
         *  BB_PLL force power up
         */
        uint32_t bbpll_force_pu:1;
        /** xtl_force_pd : R/W; bitpos: [12]; default: 0;
         *  crystall force power down
         */
        uint32_t xtl_force_pd:1;
        /** xtl_force_pu : R/W; bitpos: [13]; default: 1;
         *  crystall force power up
         */
        uint32_t xtl_force_pu:1;
        /** xtl_en_wait : R/W; bitpos: [17:14]; default: 2;
         *  wait bias_sleep and current source wakeup
         */
        uint32_t xtl_en_wait:4;
        /** xpd_rfpll : R/W; bitpos: [18]; default: 0;
         *  Need add description
         */
        uint32_t xpd_rfpll:1;
        /** xpd_rfpll_force : R/W; bitpos: [19]; default: 0;
         *  Need add description
         */
        uint32_t xpd_rfpll_force:1;
        /** xtl_ext_ctr_sel : R/W; bitpos: [22:20]; default: 0;
         *  Need add description
         */
        uint32_t xtl_ext_ctr_sel:3;
        /** xtl_force_iso : R/W; bitpos: [23]; default: 0;
         *  Need add description
         */
        uint32_t xtl_force_iso:1;
        /** pll_force_iso : R/W; bitpos: [24]; default: 0;
         *  Need add description
         */
        uint32_t pll_force_iso:1;
        /** analog_force_iso : R/W; bitpos: [25]; default: 0;
         *  Need add description
         */
        uint32_t analog_force_iso:1;
        /** xtl_force_noiso : R/W; bitpos: [26]; default: 1;
         *  Need add description
         */
        uint32_t xtl_force_noiso:1;
        /** pll_force_noiso : R/W; bitpos: [27]; default: 1;
         *  Need add description
         */
        uint32_t pll_force_noiso:1;
        /** analog_force_noiso : R/W; bitpos: [28]; default: 1;
         *  Need add description
         */
        uint32_t analog_force_noiso:1;
        /** dg_wrap_force_rst : R/W; bitpos: [29]; default: 0;
         *  digital wrap force reset in deep sleep
         */
        uint32_t dg_wrap_force_rst:1;
        /** dg_wrap_force_norst : R/W; bitpos: [30]; default: 0;
         *  digital core force no reset in deep sleep
         */
        uint32_t dg_wrap_force_norst:1;
        /** sw_sys_rst : WO; bitpos: [31]; default: 0;
         *  SW system  reset
         */
        uint32_t sw_sys_rst:1;
    };
    uint32_t val;
} rtc_cntl_options0_reg_t;

/** Type of rtc_slp_timer0 register
 *  register description
 */
typedef union {
    struct {
        /** slp_val_lo : R/W; bitpos: [31:0]; default: 0;
         *  RTC sleep timer low 32 bits
         */
        uint32_t slp_val_lo:32;
    };
    uint32_t val;
} rtc_cntl_slp_timer0_reg_t;

/** Type of rtc_slp_timer1 register
 *  register description
 */
typedef union {
    struct {
        /** slp_val_hi : R/W; bitpos: [15:0]; default: 0;
         *  RTC sleep timer high 16 bits
         */
        uint32_t slp_val_hi:16;
        /** main_timer_alarm_en : WO; bitpos: [16]; default: 0;
         *  timer alarm enable bit
         */
        uint32_t main_timer_alarm_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} rtc_cntl_slp_timer1_reg_t;

/** Type of rtc_time_update register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** timer_sys_stall : R/W; bitpos: [27]; default: 0;
         *  Enable to record system stall time
         */
        uint32_t timer_sys_stall:1;
        /** timer_xtl_off : R/W; bitpos: [28]; default: 0;
         *  Enable to record 40M XTAL OFF time
         */
        uint32_t timer_xtl_off:1;
        /** timer_sys_rst : R/W; bitpos: [29]; default: 0;
         *  enable to record system reset time
         */
        uint32_t timer_sys_rst:1;
        uint32_t reserved_30:1;
        /** update : WO; bitpos: [31]; default: 0;
         *  Set 1: to update register with RTC timer
         */
        uint32_t update:1;
    };
    uint32_t val;
} rtc_cntl_time_update_reg_t;

/** Type of rtc_time_low0 register
 *  register description
 */
typedef union {
    struct {
        /** rtc_timer_value0_low : RO; bitpos: [31:0]; default: 0;
         *  RTC timer low 32 bits
         */
        uint32_t rtc_timer_value0_low:32;
    };
    uint32_t val;
} rtc_cntl_time_low0_reg_t;

/** Type of rtc_time_high0 register
 *  register description
 */
typedef union {
    struct {
        /** rtc_timer_value0_high : RO; bitpos: [15:0]; default: 0;
         *  RTC timer high 16 bits
         */
        uint32_t rtc_timer_value0_high:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} rtc_cntl_time_high0_reg_t;

/** Type of rtc_state0 register
 *  register description
 */
typedef union {
    struct {
        /** rtc_sw_cpu_int : WO; bitpos: [0]; default: 0;
         *  rtc software interrupt to main cpu
         */
        uint32_t rtc_sw_cpu_int:1;
        /** rtc_slp_reject_cause_clr : WO; bitpos: [1]; default: 0;
         *  clear rtc sleep reject cause
         */
        uint32_t rtc_slp_reject_cause_clr:1;
        uint32_t reserved_2:20;
        /** apb2rtc_bridge_sel : R/W; bitpos: [22]; default: 0;
         *  1: APB to RTC using bridge,  0: APB to RTC using sync
         */
        uint32_t apb2rtc_bridge_sel:1;
        uint32_t reserved_23:5;
        /** sdio_active_ind : RO; bitpos: [28]; default: 0;
         *  SDIO active indication
         */
        uint32_t sdio_active_ind:1;
        /** slp_wakeup : R/W; bitpos: [29]; default: 0;
         *  leep wakeup bit
         */
        uint32_t slp_wakeup:1;
        /** slp_reject : R/W; bitpos: [30]; default: 0;
         *  leep reject bit
         */
        uint32_t slp_reject:1;
        /** sleep_en : R/W; bitpos: [31]; default: 0;
         *  sleep enable bit
         */
        uint32_t sleep_en:1;
    };
    uint32_t val;
} rtc_cntl_state0_reg_t;

/** Type of rtc_timer1 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_stall_en : R/W; bitpos: [0]; default: 1;
         *  CPU stall enable bit
         */
        uint32_t cpu_stall_en:1;
        /** cpu_stall_wait : R/W; bitpos: [5:1]; default: 1;
         *  CPU stall wait cycles in fast_clk_rtc
         */
        uint32_t cpu_stall_wait:5;
        /** ck8m_wait : R/W; bitpos: [13:6]; default: 16;
         *  CK8M wait cycles in slow_clk_rtc
         */
        uint32_t ck8m_wait:8;
        /** xtl_buf_wait : R/W; bitpos: [23:14]; default: 80;
         *  XTAL wait cycles in slow_clk_rtc
         */
        uint32_t xtl_buf_wait:10;
        /** pll_buf_wait : R/W; bitpos: [31:24]; default: 40;
         *  PLL wait cycles in slow_clk_rtc
         */
        uint32_t pll_buf_wait:8;
    };
    uint32_t val;
} rtc_cntl_timer1_reg_t;

/** Type of rtc_timer2 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** min_time_ck8m_off : R/W; bitpos: [31:24]; default: 1;
         *  minimal cycles in slow_clk_rtc for CK8M in power down state
         */
        uint32_t min_time_ck8m_off:8;
    };
    uint32_t val;
} rtc_cntl_timer2_reg_t;

/** Type of rtc_timer3 register
 *  register description
 */
typedef union {
    struct {
        /** wifi_wait_timer : R/W; bitpos: [8:0]; default: 8;
         *  Need add description
         */
        uint32_t wifi_wait_timer:9;
        /** wifi_powerup_timer : R/W; bitpos: [15:9]; default: 5;
         *  Need add description
         */
        uint32_t wifi_powerup_timer:7;
        /** bt_wait_timer : R/W; bitpos: [24:16]; default: 8;
         *  Need add description
         */
        uint32_t bt_wait_timer:9;
        /** bt_powerup_timer : R/W; bitpos: [31:25]; default: 5;
         *  Need add description
         */
        uint32_t bt_powerup_timer:7;
    };
    uint32_t val;
} rtc_cntl_timer3_reg_t;

/** Type of rtc_timer4 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_top_wait_timer : R/W; bitpos: [8:0]; default: 8;
         *  Need add description
         */
        uint32_t cpu_top_wait_timer:9;
        /** cpu_top_powerup_timer : R/W; bitpos: [15:9]; default: 5;
         *  Need add description
         */
        uint32_t cpu_top_powerup_timer:7;
        /** dg_wrap_wait_timer : R/W; bitpos: [24:16]; default: 32;
         *  Need add description
         */
        uint32_t dg_wrap_wait_timer:9;
        /** dg_wrap_powerup_timer : R/W; bitpos: [31:25]; default: 8;
         *  Need add description
         */
        uint32_t dg_wrap_powerup_timer:7;
    };
    uint32_t val;
} rtc_cntl_timer4_reg_t;

/** Type of rtc_timer5 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** min_slp_val : R/W; bitpos: [15:8]; default: 128;
         *  minimal sleep cycles in slow_clk_rtc
         */
        uint32_t min_slp_val:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} rtc_cntl_timer5_reg_t;

/** Type of rtc_timer6 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** dg_peri_wait_timer : R/W; bitpos: [24:16]; default: 8;
         *  Need add description
         */
        uint32_t dg_peri_wait_timer:9;
        /** dg_peri_powerup_timer : R/W; bitpos: [31:25]; default: 5;
         *  Need add description
         */
        uint32_t dg_peri_powerup_timer:7;
    };
    uint32_t val;
} rtc_cntl_timer6_reg_t;

/** Type of rtc_ana_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** xpd_trx_force_pd : R/W; bitpos: [16]; default: 0;
         *  Need add description
         */
        uint32_t xpd_trx_force_pd:1;
        /** xpd_trx_force_pu : R/W; bitpos: [17]; default: 1;
         *  Need add description
         */
        uint32_t xpd_trx_force_pu:1;
        /** i2c_reset_por_force_pd : R/W; bitpos: [18]; default: 1;
         *  Need add description
         */
        uint32_t i2c_reset_por_force_pd:1;
        /** i2c_reset_por_force_pu : R/W; bitpos: [19]; default: 0;
         *  Need add description
         */
        uint32_t i2c_reset_por_force_pu:1;
        /** glitch_rst_en : R/W; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t glitch_rst_en:1;
        uint32_t reserved_21:1;
        /** peri_i2c_pu : R/W; bitpos: [22]; default: 1;
         *  PLLA force power up
         */
        uint32_t peri_i2c_pu:1;
        /** plla_force_pd : R/W; bitpos: [23]; default: 1;
         *  PLLA force power down
         */
        uint32_t plla_force_pd:1;
        /** plla_force_pu : R/W; bitpos: [24]; default: 0;
         *  PLLA force power up
         */
        uint32_t plla_force_pu:1;
        /** bbpll_cal_slp_start : R/W; bitpos: [25]; default: 0;
         *  start BBPLL calibration during sleep
         */
        uint32_t bbpll_cal_slp_start:1;
        /** pvtmon_pu : R/W; bitpos: [26]; default: 0;
         *  1: PVTMON power up , otherwise power down
         */
        uint32_t pvtmon_pu:1;
        /** txrf_i2c_pu : R/W; bitpos: [27]; default: 0;
         *  1: TXRF_I2C power up , otherwise power down
         */
        uint32_t txrf_i2c_pu:1;
        /** rfrx_pbus_pu : R/W; bitpos: [28]; default: 0;
         *  1: RFRX_PBUS power up , otherwise power down
         */
        uint32_t rfrx_pbus_pu:1;
        uint32_t reserved_29:1;
        /** ckgen_i2c_pu : R/W; bitpos: [30]; default: 0;
         *  1: CKGEN_I2C power up , otherwise power down
         */
        uint32_t ckgen_i2c_pu:1;
        /** pll_i2c_pu : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t pll_i2c_pu:1;
    };
    uint32_t val;
} rtc_cntl_ana_conf_reg_t;

/** Type of rtc_reset_state register
 *  register description
 */
typedef union {
    struct {
        /** reset_cause_procpu : RO; bitpos: [5:0]; default: 0;
         *  reset cause of PRO CPU
         */
        uint32_t reset_cause_procpu:6;
        /** reset_cause_appcpu : RO; bitpos: [11:6]; default: 0;
         *  reset cause of APP CPU
         */
        uint32_t reset_cause_appcpu:6;
        /** stat_vector_sel_appcpu : R/W; bitpos: [12]; default: 1;
         *  APP CPU state vector sel
         */
        uint32_t stat_vector_sel_appcpu:1;
        /** stat_vector_sel_procpu : R/W; bitpos: [13]; default: 1;
         *  PRO CPU state vector sel
         */
        uint32_t stat_vector_sel_procpu:1;
        /** all_reset_flag_procpu : RO; bitpos: [14]; default: 0;
         *  PRO CPU reset_flag
         */
        uint32_t all_reset_flag_procpu:1;
        /** all_reset_flag_appcpu : RO; bitpos: [15]; default: 0;
         *  APP CPU reset flag
         */
        uint32_t all_reset_flag_appcpu:1;
        /** all_reset_flag_clr_procpu : WO; bitpos: [16]; default: 0;
         *  clear PRO CPU reset_flag
         */
        uint32_t all_reset_flag_clr_procpu:1;
        /** all_reset_flag_clr_appcpu : WO; bitpos: [17]; default: 0;
         *  clear APP CPU reset flag
         */
        uint32_t all_reset_flag_clr_appcpu:1;
        /** ocd_halt_on_reset_appcpu : R/W; bitpos: [18]; default: 0;
         *  APPCPU OcdHaltOnReset
         */
        uint32_t ocd_halt_on_reset_appcpu:1;
        /** ocd_halt_on_reset_procpu : R/W; bitpos: [19]; default: 0;
         *  PROCPU OcdHaltOnReset
         */
        uint32_t ocd_halt_on_reset_procpu:1;
        /** jtag_reset_flag_procpu : RO; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t jtag_reset_flag_procpu:1;
        /** jtag_reset_flag_appcpu : RO; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t jtag_reset_flag_appcpu:1;
        /** jtag_reset_flag_clr_procpu : WO; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t jtag_reset_flag_clr_procpu:1;
        /** jtag_reset_flag_clr_appcpu : WO; bitpos: [23]; default: 0;
         *  Need add description
         */
        uint32_t jtag_reset_flag_clr_appcpu:1;
        /** rtc_dreset_mask_appcpu : R/W; bitpos: [24]; default: 0;
         *  Need add description
         */
        uint32_t rtc_dreset_mask_appcpu:1;
        /** rtc_dreset_mask_procpu : R/W; bitpos: [25]; default: 0;
         *  Need add description
         */
        uint32_t rtc_dreset_mask_procpu:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} rtc_cntl_reset_state_reg_t;

/** Type of rtc_wakeup_state register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:13;
        /** rtc_wakeup_ena : R/W; bitpos: [31:13]; default: 12;
         *  wakeup enable bitmap
         */
        uint32_t rtc_wakeup_ena:19;
    };
    uint32_t val;
} rtc_cntl_wakeup_state_reg_t;

/** Type of int_ena register
 *  register description
 */
typedef union {
    struct {
        /** slp_wakeup : R/W; bitpos: [0]; default: 0;
         *  enable sleep wakeup interrupt
         */
        uint32_t slp_wakeup:1;
        /** slp_reject : R/W; bitpos: [1]; default: 0;
         *  enable sleep reject interrupt
         */
        uint32_t slp_reject:1;
        uint32_t reserved_2:1;
        /** rtc_wdt : R/W; bitpos: [3]; default: 0;
         *  enable RTC WDT interrupt
         */
        uint32_t rtc_wdt:1;
        uint32_t reserved_4:5;
        /** rtc_brown_out : R/W; bitpos: [9]; default: 0;
         *  enable brown out interrupt
         */
        uint32_t rtc_brown_out:1;
        /** rtc_main_timer : R/W; bitpos: [10]; default: 0;
         *  enable RTC main timer interrupt
         */
        uint32_t rtc_main_timer:1;
        uint32_t reserved_11:4;
        /** rtc_swd : R/W; bitpos: [15]; default: 0;
         *  enable super watch dog interrupt
         */
        uint32_t rtc_swd:1;
        /** rtc_xtal32k_dead : R/W; bitpos: [16]; default: 0;
         *  enable xtal32k_dead  interrupt
         */
        uint32_t rtc_xtal32k_dead:1;
        uint32_t reserved_17:2;
        /** rtc_glitch_det : R/W; bitpos: [19]; default: 0;
         *  enbale gitch det interrupt
         */
        uint32_t rtc_glitch_det:1;
        /** rtc_bbpll_cal : R/W; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_bbpll_cal:1;
        /** rtc_ble_compare_wake : RW; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_compare_wake:1;
        /** vset_dcdc_done : R/W; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_done:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_cntl_int_ena_reg_t;

/** Type of int_raw register
 *  register description
 */
typedef union {
    struct {
        /** slp_wakeup : RO; bitpos: [0]; default: 0;
         *  sleep wakeup interrupt raw
         */
        uint32_t slp_wakeup:1;
        /** slp_reject : RO; bitpos: [1]; default: 0;
         *  sleep reject interrupt raw
         */
        uint32_t slp_reject:1;
        uint32_t reserved_2:1;
        /** rtc_wdt : RO; bitpos: [3]; default: 0;
         *  RTC WDT interrupt raw
         */
        uint32_t rtc_wdt:1;
        uint32_t reserved_4:5;
        /** rtc_brown_out : RO; bitpos: [9]; default: 0;
         *  brown out interrupt raw
         */
        uint32_t rtc_brown_out:1;
        /** rtc_main_timer : RO; bitpos: [10]; default: 0;
         *  RTC main timer interrupt raw
         */
        uint32_t rtc_main_timer:1;
        uint32_t reserved_11:4;
        /** rtc_swd : RO; bitpos: [15]; default: 0;
         *  super watch dog interrupt raw
         */
        uint32_t rtc_swd:1;
        /** rtc_xtal32k_dead : RO; bitpos: [16]; default: 0;
         *  xtal32k dead detection interrupt raw
         */
        uint32_t rtc_xtal32k_dead:1;
        uint32_t reserved_17:2;
        /** rtc_glitch_det : RO; bitpos: [19]; default: 0;
         *  glitch_det_interrupt_raw
         */
        uint32_t rtc_glitch_det:1;
        /** rtc_bbpll_cal : RO; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_bbpll_cal:1;
        /** rtc_ble_compare_wake : RO; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_compare_wake:1;
        /** vset_dcdc_done : RO; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_done:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_cntl_int_raw_reg_t;

/** Type of int_st register
 *  register description
 */
typedef union {
    struct {
        /** slp_wakeup : RO; bitpos: [0]; default: 0;
         *  sleep wakeup interrupt state
         */
        uint32_t slp_wakeup:1;
        /** slp_reject : RO; bitpos: [1]; default: 0;
         *  sleep reject interrupt state
         */
        uint32_t slp_reject:1;
        uint32_t reserved_2:1;
        /** rtc_wdt : RO; bitpos: [3]; default: 0;
         *  RTC WDT interrupt state
         */
        uint32_t rtc_wdt:1;
        uint32_t reserved_4:5;
        /** rtc_brown_out : RO; bitpos: [9]; default: 0;
         *  brown out interrupt state
         */
        uint32_t rtc_brown_out:1;
        /** rtc_main_timer : RO; bitpos: [10]; default: 0;
         *  RTC main timer interrupt state
         */
        uint32_t rtc_main_timer:1;
        uint32_t reserved_11:4;
        /** rtc_swd : RO; bitpos: [15]; default: 0;
         *  super watch dog interrupt state
         */
        uint32_t rtc_swd:1;
        /** rtc_xtal32k_dead : RO; bitpos: [16]; default: 0;
         *  xtal32k dead detection interrupt state
         */
        uint32_t rtc_xtal32k_dead:1;
        uint32_t reserved_17:2;
        /** rtc_glitch_det : RO; bitpos: [19]; default: 0;
         *  glitch_det_interrupt state
         */
        uint32_t rtc_glitch_det:1;
        /** rtc_bbpll_cal : RO; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_bbpll_cal:1;
        /** rtc_ble_compare_wake : RO; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_compare_wake:1;
        /** vset_dcdc_done : RO; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_done:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_cntl_int_st_reg_t;

/** Type of int_clr register
 *  register description
 */
typedef union {
    struct {
        /** slp_wakeup : WO; bitpos: [0]; default: 0;
         *  Clear sleep wakeup interrupt state
         */
        uint32_t slp_wakeup:1;
        /** slp_reject : WO; bitpos: [1]; default: 0;
         *  Clear sleep reject interrupt state
         */
        uint32_t slp_reject:1;
        uint32_t reserved_2:1;
        /** rtc_wdt : WO; bitpos: [3]; default: 0;
         *  Clear RTC WDT interrupt state
         */
        uint32_t rtc_wdt:1;
        uint32_t reserved_4:5;
        /** rtc_brown_out : WO; bitpos: [9]; default: 0;
         *  Clear brown out interrupt state
         */
        uint32_t rtc_brown_out:1;
        /** rtc_main_timer : WO; bitpos: [10]; default: 0;
         *  Clear RTC main timer interrupt state
         */
        uint32_t rtc_main_timer:1;
        uint32_t reserved_11:4;
        /** rtc_swd : WO; bitpos: [15]; default: 0;
         *  Clear super watch dog interrupt state
         */
        uint32_t rtc_swd:1;
        /** rtc_xtal32k_dead : WO; bitpos: [16]; default: 0;
         *  Clear RTC WDT interrupt state
         */
        uint32_t rtc_xtal32k_dead:1;
        uint32_t reserved_17:2;
        /** rtc_glitch_det : WO; bitpos: [19]; default: 0;
         *  Clear glitch det interrupt state
         */
        uint32_t rtc_glitch_det:1;
        /** rtc_bbpll_cal : WO; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_bbpll_cal:1;
        /** rtc_ble_compare_wake : WO; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_compare_wake:1;
        /** vset_dcdc_done : WO; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_done:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_cntl_int_clr_reg_t;

/** Type of  register
 *  register description
 */
typedef union {
    struct {
        /** rtc_scratch : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t rtc_scratch:32;
    };
    uint32_t val;
} rtc_cntl_store_reg_t;


/** Type of rtc_ext_xtl_conf register
 *  register description
 */
typedef union {
    struct {
        /** xtal32k_wdt_en : R/W; bitpos: [0]; default: 0;
         *  xtal 32k watch dog enable
         */
        uint32_t xtal32k_wdt_en:1;
        /** xtal32k_wdt_clk_fo : R/W; bitpos: [1]; default: 0;
         *  xtal 32k watch dog clock force on
         */
        uint32_t xtal32k_wdt_clk_fo:1;
        /** xtal32k_wdt_reset : R/W; bitpos: [2]; default: 0;
         *  xtal 32k watch dog sw reset
         */
        uint32_t xtal32k_wdt_reset:1;
        /** xtal32k_ext_clk_fo : R/W; bitpos: [3]; default: 0;
         *  xtal 32k external xtal clock force on
         */
        uint32_t xtal32k_ext_clk_fo:1;
        /** xtal32k_auto_backup : R/W; bitpos: [4]; default: 0;
         *  xtal 32k switch to back up clock when xtal is dead
         */
        uint32_t xtal32k_auto_backup:1;
        /** xtal32k_auto_restart : R/W; bitpos: [5]; default: 0;
         *  xtal 32k restart xtal when xtal is dead
         */
        uint32_t xtal32k_auto_restart:1;
        /** xtal32k_auto_return : R/W; bitpos: [6]; default: 0;
         *  xtal 32k switch back xtal when xtal is restarted
         */
        uint32_t xtal32k_auto_return:1;
        /** xtal32k_xpd_force : R/W; bitpos: [7]; default: 1;
         *  Xtal 32k xpd control by sw or fsm
         */
        uint32_t xtal32k_xpd_force:1;
        /** enckinit_xtal_32k : R/W; bitpos: [8]; default: 0;
         *  apply an internal clock to help xtal 32k to start
         */
        uint32_t enckinit_xtal_32k:1;
        /** dbuf_xtal_32k : R/W; bitpos: [9]; default: 0;
         *  0: single-end buffer 1: differential buffer
         */
        uint32_t dbuf_xtal_32k:1;
        /** dgm_xtal_32k : R/W; bitpos: [12:10]; default: 3;
         *  xtal_32k gm control
         */
        uint32_t dgm_xtal_32k:3;
        /** dres_xtal_32k : R/W; bitpos: [15:13]; default: 3;
         *  DRES_XTAL_32K
         */
        uint32_t dres_xtal_32k:3;
        /** xpd_xtal_32k : R/W; bitpos: [16]; default: 0;
         *  XPD_XTAL_32K
         */
        uint32_t xpd_xtal_32k:1;
        /** dac_xtal_32k : R/W; bitpos: [19:17]; default: 3;
         *  DAC_XTAL_32K
         */
        uint32_t dac_xtal_32k:3;
        /** rtc_wdt_state : RO; bitpos: [22:20]; default: 0;
         *  state of 32k_wdt
         */
        uint32_t rtc_wdt_state:3;
        /** rtc_xtal32k_gpio_sel : R/W; bitpos: [23]; default: 0;
         *  XTAL_32K sel. 0: external XTAL_32K, 1: CLK from RTC pad X32P_C
         */
        uint32_t rtc_xtal32k_gpio_sel:1;
        uint32_t reserved_24:6;
        /** ctr_lv : R/W; bitpos: [30]; default: 0;
         *  0: power down XTAL at high level, 1: power down XTAL at low level
         */
        uint32_t ctr_lv:1;
        /** ctr_en : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t ctr_en:1;
    };
    uint32_t val;
} rtc_cntl_ext_xtl_conf_reg_t;

/** Type of rtc_ext_wakeup_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** gpio_wakeup_filter : R/W; bitpos: [31]; default: 0;
         *  enable filter for gpio wakeup event
         */
        uint32_t gpio_wakeup_filter:1;
    };
    uint32_t val;
} rtc_cntl_ext_wakeup_conf_reg_t;

/** Type of rtc_slp_reject_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:11;
        /** rtc_sleep_reject_ena : R/W; bitpos: [29:11]; default: 0;
         *  sleep reject enable
         */
        uint32_t rtc_sleep_reject_ena:19;
        /** light_slp_reject_en : R/W; bitpos: [30]; default: 0;
         *  enable reject for light sleep
         */
        uint32_t light_slp_reject_en:1;
        /** deep_slp_reject_en : R/W; bitpos: [31]; default: 0;
         *  enable reject for deep sleep
         */
        uint32_t deep_slp_reject_en:1;
    };
    uint32_t val;
} rtc_cntl_slp_reject_conf_reg_t;

/** Type of rtc_cpu_period_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** cpusel_conf : R/W; bitpos: [29]; default: 0;
         *  CPU sel option
         */
        uint32_t cpusel_conf:1;
        /** cpuperiod_sel : R/W; bitpos: [31:30]; default: 0;
         *  Need add description
         */
        uint32_t cpuperiod_sel:2;
    };
    uint32_t val;
} rtc_cntl_cpu_period_conf_reg_t;

/** Type of rtc_clk_conf register
 *  register description
 */
typedef union {
    struct {
        /** rtc_ble_tmr_rst : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_tmr_rst:1;
        /** efuse_clk_force_gating : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t efuse_clk_force_gating:1;
        /** efuse_clk_force_nogating : R/W; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t efuse_clk_force_nogating:1;
        /** ck8m_div_sel_vld : R/W; bitpos: [3]; default: 1;
         *  used to sync reg_ck8m_div_sel bus. Clear vld before set reg_ck8m_div_sel, then set
         *  vld to actually switch the clk
         */
        uint32_t ck8m_div_sel_vld:1;
        /** dig_xtal32k_en : R/W; bitpos: [4]; default: 0;
         *  enable CK_XTAL_32K for digital core (no relationship with RTC core)
         */
        uint32_t dig_xtal32k_en:1;
        /** dig_rc32k_en : R/W; bitpos: [5]; default: 1;
         *  enable RC32K for digital core (no relationship with RTC core)
         */
        uint32_t dig_rc32k_en:1;
        /** dig_clk8m_en : R/W; bitpos: [6]; default: 0;
         *  enable CK8M for digital core (no relationship with RTC core)
         */
        uint32_t dig_clk8m_en:1;
        /** rtc_ble_timer_sel : R/W; bitpos: [7]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_timer_sel:1;
        uint32_t reserved_8:2;
        /** ck8m_div_sel : R/W; bitpos: [12:10]; default: 3;
         *  divider = reg_ck8m_div_sel + 1
         */
        uint32_t ck8m_div_sel:3;
        /** xtal_force_nogating : R/W; bitpos: [13]; default: 0;
         *  XTAL force no gating during sleep
         */
        uint32_t xtal_force_nogating:1;
        /** ck8m_force_nogating : R/W; bitpos: [14]; default: 0;
         *  CK8M force no gating during sleep
         */
        uint32_t ck8m_force_nogating:1;
        /** ck8m_dfreq : R/W; bitpos: [24:15]; default: 600;
         *  CK8M_DFREQ
         */
        uint32_t ck8m_dfreq:10;
        /** ck8m_force_pd : R/W; bitpos: [25]; default: 0;
         *  CK8M force power down
         */
        uint32_t ck8m_force_pd:1;
        /** ck8m_force_pu : R/W; bitpos: [26]; default: 0;
         *  CK8M force power up
         */
        uint32_t ck8m_force_pu:1;
        /** xtal_global_force_gating : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t xtal_global_force_gating:1;
        /** xtal_global_force_nogating : R/W; bitpos: [28]; default: 1;
         *  Need add description
         */
        uint32_t xtal_global_force_nogating:1;
        /** fast_clk_rtc_sel : R/W; bitpos: [29]; default: 0;
         *  fast_clk_rtc sel. 0: XTAL div 2, 1: CK8M
         */
        uint32_t fast_clk_rtc_sel:1;
        /** ana_clk_rtc_sel : R/W; bitpos: [31:30]; default: 0;
         *  Need add description
         */
        uint32_t ana_clk_rtc_sel:2;
    };
    uint32_t val;
} rtc_cntl_clk_conf_reg_t;

/** Type of rtc_slow_clk_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:19;
        /** rtc_ana_clk_pd_slp : R/W; bitpos: [19]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ana_clk_pd_slp:1;
        /** rtc_ana_clk_pd_monitor : R/W; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ana_clk_pd_monitor:1;
        /** rtc_ana_clk_pd_idle : R/W; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ana_clk_pd_idle:1;
        /** rtc_ana_clk_div_vld : R/W; bitpos: [22]; default: 1;
         *  used to sync div bus. clear vld before set reg_rtc_ana_clk_div, then set vld to
         *  actually switch the clk
         */
        uint32_t rtc_ana_clk_div_vld:1;
        /** rtc_ana_clk_div : R/W; bitpos: [30:23]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ana_clk_div:8;
        /** slow_clk_next_edge : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t slow_clk_next_edge:1;
    };
    uint32_t val;
} rtc_cntl_slow_clk_conf_reg_t;

/** Type of rtc_sdio_conf register
 *  register description
 */
typedef union {
    struct {
        /** sdio_timer_target : R/W; bitpos: [7:0]; default: 10;
         *  timer count to apply reg_sdio_dcap after sdio power on
         */
        uint32_t sdio_timer_target:8;
        uint32_t reserved_8:1;
        /** sdio_dthdrv : R/W; bitpos: [10:9]; default: 3;
         *  Tieh = 1 mode drive ability. Initially set to 0 to limit charge current, set to 3
         *  after several us.
         */
        uint32_t sdio_dthdrv:2;
        /** sdio_dcap : R/W; bitpos: [12:11]; default: 3;
         *  ability to prevent LDO from overshoot
         */
        uint32_t sdio_dcap:2;
        /** sdio_initi : R/W; bitpos: [14:13]; default: 1;
         *  add resistor from ldo output to ground. 0: no res, 1: 6k, 2: 4k, 3: 2k
         */
        uint32_t sdio_initi:2;
        /** sdio_en_initi : R/W; bitpos: [15]; default: 1;
         *  0 to set init[1:0]=0
         */
        uint32_t sdio_en_initi:1;
        /** sdio_dcurlim : R/W; bitpos: [18:16]; default: 0;
         *  tune current limit threshold when tieh = 0. About 800mA/(8+d)
         */
        uint32_t sdio_dcurlim:3;
        /** sdio_modecurlim : R/W; bitpos: [19]; default: 0;
         *  select current limit mode
         */
        uint32_t sdio_modecurlim:1;
        /** sdio_encurlim : R/W; bitpos: [20]; default: 1;
         *  enable current limit
         */
        uint32_t sdio_encurlim:1;
        /** sdio_reg_pd_en : R/W; bitpos: [21]; default: 1;
         *  power down SDIO_REG in sleep. Only active when reg_sdio_force = 0
         */
        uint32_t sdio_reg_pd_en:1;
        /** sdio_force : R/W; bitpos: [22]; default: 0;
         *  1: use SW option to control SDIO_REG ,0: use state machine
         */
        uint32_t sdio_force:1;
        /** sdio_tieh : R/W; bitpos: [23]; default: 1;
         *  SW option for SDIO_TIEH. Only active when reg_sdio_force = 1
         */
        uint32_t sdio_tieh:1;
        /** reg1p8_ready : RO; bitpos: [24]; default: 0;
         *  read only register for REG1P8_READY
         */
        uint32_t reg1p8_ready:1;
        /** drefl_sdio : R/W; bitpos: [26:25]; default: 1;
         *  SW option for DREFL_SDIO. Only active when reg_sdio_force = 1
         */
        uint32_t drefl_sdio:2;
        /** drefm_sdio : R/W; bitpos: [28:27]; default: 1;
         *  SW option for DREFM_SDIO. Only active when reg_sdio_force = 1
         */
        uint32_t drefm_sdio:2;
        /** drefh_sdio : R/W; bitpos: [30:29]; default: 0;
         *  SW option for DREFH_SDIO. Only active when reg_sdio_force = 1
         */
        uint32_t drefh_sdio:2;
        /** xpd_sdio_reg : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t xpd_sdio_reg:1;
    };
    uint32_t val;
} rtc_cntl_sdio_conf_reg_t;

/** Type of rtc_bias_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** bias_buf_idle : R/W; bitpos: [10]; default: 0;
         *  Need add description
         */
        uint32_t bias_buf_idle:1;
        /** bias_buf_wake : R/W; bitpos: [11]; default: 1;
         *  Need add description
         */
        uint32_t bias_buf_wake:1;
        /** bias_buf_deep_slp : R/W; bitpos: [12]; default: 0;
         *  Need add description
         */
        uint32_t bias_buf_deep_slp:1;
        /** bias_buf_monitor : R/W; bitpos: [13]; default: 0;
         *  Need add description
         */
        uint32_t bias_buf_monitor:1;
        /** pd_cur_deep_slp : R/W; bitpos: [14]; default: 0;
         *  xpd cur when rtc in sleep_state
         */
        uint32_t pd_cur_deep_slp:1;
        /** pd_cur_monitor : R/W; bitpos: [15]; default: 0;
         *  xpd cur when rtc in monitor state
         */
        uint32_t pd_cur_monitor:1;
        /** bias_sleep_deep_slp : R/W; bitpos: [16]; default: 1;
         *  bias_sleep when rtc in sleep_state
         */
        uint32_t bias_sleep_deep_slp:1;
        /** bias_sleep_monitor : R/W; bitpos: [17]; default: 0;
         *  bias_sleep when rtc in monitor state
         */
        uint32_t bias_sleep_monitor:1;
        /** dbg_atten_deep_slp : R/W; bitpos: [21:18]; default: 0;
         *  DBG_ATTEN when rtc in sleep state
         */
        uint32_t dbg_atten_deep_slp:4;
        /** dbg_atten_monitor : R/W; bitpos: [25:22]; default: 0;
         *  DBG_ATTEN when rtc in monitor state
         */
        uint32_t dbg_atten_monitor:4;
        /** xpd_dcdc_slp : R/W; bitpos: [26]; default: 1;
         *  Need add description
         */
        uint32_t xpd_dcdc_slp:1;
        /** xpd_dcdc_monitor : R/W; bitpos: [27]; default: 1;
         *  Need add description
         */
        uint32_t xpd_dcdc_monitor:1;
        /** xpd_dcdc_idle : R/W; bitpos: [28]; default: 1;
         *  Need add description
         */
        uint32_t xpd_dcdc_idle:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} rtc_cntl_bias_conf_reg_t;

/** Type of rtc_regulator register
 *  register description
 */
typedef union {
    struct {
        /** dbias_switch_slp : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dbias_switch_slp:1;
        /** dbias_switch_monitor : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t dbias_switch_monitor:1;
        /** dbias_switch_idle : R/W; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t dbias_switch_idle:1;
        /** dig_reg_cal_en : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t dig_reg_cal_en:1;
        /** sck_dcap : R/W; bitpos: [11:4]; default: 0;
         *  Need add description
         */
        uint32_t sck_dcap:8;
        uint32_t reserved_12:3;
        /** rtc_vdd_drv_b_active : R/W; bitpos: [20:15]; default: 0;
         *  SCK_DCAP
         */
        uint32_t rtc_vdd_drv_b_active:6;
        /** rtc_vdd_drv_b_slp : R/W; bitpos: [26:21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_vdd_drv_b_slp:6;
        /** rtc_vdd_drv_b_slp_en : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t rtc_vdd_drv_b_slp_en:1;
        /** rtc_dboost_force_pd : R/W; bitpos: [28]; default: 0;
         *  RTC_DBOOST force power down
         */
        uint32_t rtc_dboost_force_pd:1;
        /** rtc_dboost_force_pu : R/W; bitpos: [29]; default: 1;
         *  RTC_DBOOST force power up
         */
        uint32_t rtc_dboost_force_pu:1;
        /** rtc_regulator_force_pd : R/W; bitpos: [30]; default: 0;
         *  RTC_REG force power down (for RTC_REG power down means decrease the voltage to 0.8v
         *  or lower )
         */
        uint32_t rtc_regulator_force_pd:1;
        /** rtc_regulator_force_pu : R/W; bitpos: [31]; default: 1;
         *  Need add description
         */
        uint32_t rtc_regulator_force_pu:1;
    };
    uint32_t val;
} rtc_cntl_regulator_reg_t;

/** Type of rtc_regulator0_dbias register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:15;
        /** pvt_rtc_dbias : RO; bitpos: [19:15]; default: 20;
         *  get pvt dbias value
         */
        uint32_t pvt_rtc_dbias:5;
        /** rtc_regulator0_dbias_slp : R/W; bitpos: [24:20]; default: 20;
         *  the rtc regulator0 dbias when chip in sleep state
         */
        uint32_t rtc_regulator0_dbias_slp:5;
        /** rtc_regulator0_dbias_active : R/W; bitpos: [29:25]; default: 20;
         *  the rtc regulator0 dbias when chip in active state
         */
        uint32_t rtc_regulator0_dbias_active:5;
        uint32_t reserved_30:1;
        /** rtc_regulator0_dbias_sel : R/W; bitpos: [31]; default: 1;
         *  1: select sw dbias_active 0: select pvt value
         */
        uint32_t rtc_regulator0_dbias_sel:1;
    };
    uint32_t val;
} rtc_cntl_regulator0_dbias_reg_t;

/** Type of rtc_regulator1_dbias register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** rtc_regulator1_dbias_slp : R/W; bitpos: [23:20]; default: 8;
         *  the rtc regulator1 dbias when chip in sleep state
         */
        uint32_t rtc_regulator1_dbias_slp:4;
        uint32_t reserved_24:1;
        /** rtc_regulator1_dbias_active : R/W; bitpos: [28:25]; default: 8;
         *  the rtc regulator1 dbias when chip in active state
         */
        uint32_t rtc_regulator1_dbias_active:4;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} rtc_cntl_regulator1_dbias_reg_t;

/** Type of dig_regulator register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** mem_regulator_slp_force_pd : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t mem_regulator_slp_force_pd:1;
        /** mem_regulator_slp_force_pu : R/W; bitpos: [2]; default: 1;
         *  Need add description
         */
        uint32_t mem_regulator_slp_force_pu:1;
        /** dg_vdd_drv_b_slp : R/W; bitpos: [26:3]; default: 0;
         *  Need add description
         */
        uint32_t dg_vdd_drv_b_slp:24;
        /** dg_vdd_drv_b_slp_en : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t dg_vdd_drv_b_slp_en:1;
        /** dg_regulator_slp_force_pd : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t dg_regulator_slp_force_pd:1;
        /** dg_regulator_slp_force_pu : R/W; bitpos: [29]; default: 1;
         *  Need add description
         */
        uint32_t dg_regulator_slp_force_pu:1;
        /** dg_regulator_force_pd : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t dg_regulator_force_pd:1;
        /** dg_regulator_force_pu : R/W; bitpos: [31]; default: 1;
         *  Need add description
         */
        uint32_t dg_regulator_force_pu:1;
    };
    uint32_t val;
} rtc_cntl_dig_regulator_reg_t;

/** Type of dig_regulator_drvb register
 *  register description
 */
typedef union {
    struct {
        /** dg_vdd_drv_b_active : R/W; bitpos: [23:0]; default: 0;
         *  Need add description
         */
        uint32_t dg_vdd_drv_b_active:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} rtc_cntl_dig_regulator_drvb_reg_t;

/** Type of dig_regulator0_dbias register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:15;
        /** pvt_dig_dbias : RO; bitpos: [19:15]; default: 20;
         *  get pvt dbias value
         */
        uint32_t pvt_dig_dbias:5;
        /** dig_regulator0_dbias_slp : R/W; bitpos: [24:20]; default: 20;
         *  the dig regulator0 dbias when chip in sleep state
         */
        uint32_t dig_regulator0_dbias_slp:5;
        /** dig_regulator0_dbias_active : R/W; bitpos: [29:25]; default: 20;
         *  the dig regulator0 dbias when chip in active state
         */
        uint32_t dig_regulator0_dbias_active:5;
        /** dig_regulator0_dbias_init : WO; bitpos: [30]; default: 0;
         *  initial pvt dbias value
         */
        uint32_t dig_regulator0_dbias_init:1;
        /** dig_regulator0_dbias_sel : R/W; bitpos: [31]; default: 1;
         *  1: select sw dbias_active 0: select pvt value
         */
        uint32_t dig_regulator0_dbias_sel:1;
    };
    uint32_t val;
} rtc_cntl_dig_regulator0_dbias_reg_t;

/** Type of dig_regulator1_dbias register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** mem_regulator1_dbias_slp : R/W; bitpos: [15:12]; default: 8;
         *  Need add description
         */
        uint32_t mem_regulator1_dbias_slp:4;
        /** mem_regulator1_dbias_active : R/W; bitpos: [19:16]; default: 8;
         *  Need add description
         */
        uint32_t mem_regulator1_dbias_active:4;
        /** dig_regulator1_dbias_slp : R/W; bitpos: [23:20]; default: 8;
         *  the dig regulator1 dbias when chip in sleep state
         */
        uint32_t dig_regulator1_dbias_slp:4;
        uint32_t reserved_24:1;
        /** dig_regulator1_dbias_active : R/W; bitpos: [28:25]; default: 8;
         *  the dig regulator1 dbias when chip in active state
         */
        uint32_t dig_regulator1_dbias_active:4;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} rtc_cntl_dig_regulator1_dbias_reg_t;

/** Type of rtc_pwc register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** rtc_pad_force_hold : R/W; bitpos: [21]; default: 0;
         *  rtc pad force hold
         */
        uint32_t rtc_pad_force_hold:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} rtc_cntl_pwc_reg_t;

/** Type of dig_pwc register
 *  register description
 */
typedef union {
    struct {
        /** vdd_spi_pwr_drv : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t vdd_spi_pwr_drv:2;
        /** vdd_spi_pwr_force : R/W; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t vdd_spi_pwr_force:1;
        /** lslp_mem_force_pd : R/W; bitpos: [3]; default: 0;
         *  memories in digital core force PD in sleep
         */
        uint32_t lslp_mem_force_pd:1;
        /** lslp_mem_force_pu : R/W; bitpos: [4]; default: 1;
         *  memories in digital core force no PD in sleep
         */
        uint32_t lslp_mem_force_pu:1;
        uint32_t reserved_5:2;
        /** dg_mem_force_pd : R/W; bitpos: [7]; default: 0;
         *  Need add description
         */
        uint32_t dg_mem_force_pd:1;
        /** dg_mem_force_pu : R/W; bitpos: [8]; default: 1;
         *  Need add description
         */
        uint32_t dg_mem_force_pu:1;
        /** dg_wrap_force_pd : R/W; bitpos: [9]; default: 0;
         *  Need add description
         */
        uint32_t dg_wrap_force_pd:1;
        /** dg_wrap_force_pu : R/W; bitpos: [10]; default: 1;
         *  Need add description
         */
        uint32_t dg_wrap_force_pu:1;
        /** bt_force_pd : R/W; bitpos: [11]; default: 0;
         *  Need add description
         */
        uint32_t bt_force_pd:1;
        /** bt_force_pu : R/W; bitpos: [12]; default: 1;
         *  Need add description
         */
        uint32_t bt_force_pu:1;
        /** dg_peri_force_pd : R/W; bitpos: [13]; default: 0;
         *  Need add description
         */
        uint32_t dg_peri_force_pd:1;
        /** dg_peri_force_pu : R/W; bitpos: [14]; default: 1;
         *  Need add description
         */
        uint32_t dg_peri_force_pu:1;
        /** fastmem_force_lpd : R/W; bitpos: [15]; default: 0;
         *  Need add description
         */
        uint32_t fastmem_force_lpd:1;
        /** fastmem_force_lpu : R/W; bitpos: [16]; default: 1;
         *  Need add description
         */
        uint32_t fastmem_force_lpu:1;
        /** wifi_force_pd : R/W; bitpos: [17]; default: 0;
         *  wifi force power down
         */
        uint32_t wifi_force_pd:1;
        /** wifi_force_pu : R/W; bitpos: [18]; default: 1;
         *  wifi force power up
         */
        uint32_t wifi_force_pu:1;
        uint32_t reserved_19:2;
        /** cpu_top_force_pd : R/W; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t cpu_top_force_pd:1;
        /** cpu_top_force_pu : R/W; bitpos: [22]; default: 1;
         *  Need add description
         */
        uint32_t cpu_top_force_pu:1;
        uint32_t reserved_23:3;
        /** dg_wrap_ret_pd_en : R/W; bitpos: [26]; default: 0;
         *  Need add description
         */
        uint32_t dg_wrap_ret_pd_en:1;
        /** bt_pd_en : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t bt_pd_en:1;
        /** dg_peri_pd_en : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t dg_peri_pd_en:1;
        /** cpu_top_pd_en : R/W; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t cpu_top_pd_en:1;
        /** wifi_pd_en : R/W; bitpos: [30]; default: 0;
         *  enable power down wifi in sleep
         */
        uint32_t wifi_pd_en:1;
        /** dg_wrap_pd_en : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t dg_wrap_pd_en:1;
    };
    uint32_t val;
} rtc_cntl_dig_pwc_reg_t;

/** Type of dig_power_slave0_pd register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** pd_dg_peri_switch_mask : R/W; bitpos: [6:2]; default: 0;
         *  Need add description
         */
        uint32_t pd_dg_peri_switch_mask:5;
        /** pd_dg_wrap_switch_mask : R/W; bitpos: [11:7]; default: 0;
         *  Need add description
         */
        uint32_t pd_dg_wrap_switch_mask:5;
        /** pd_mem_switch_mask : R/W; bitpos: [31:12]; default: 0;
         *  Need add description
         */
        uint32_t pd_mem_switch_mask:20;
    };
    uint32_t val;
} rtc_cntl_dig_power_slave0_pd_reg_t;

/** Type of dig_power_slave1_pd register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** pd_wifi_switch_mask : R/W; bitpos: [26:22]; default: 0;
         *  Need add description
         */
        uint32_t pd_wifi_switch_mask:5;
        /** pd_cpu_switch_mask : R/W; bitpos: [31:27]; default: 0;
         *  Need add description
         */
        uint32_t pd_cpu_switch_mask:5;
    };
    uint32_t val;
} rtc_cntl_dig_power_slave1_pd_reg_t;

/** Type of dig_power_slave0_fpu register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** xpd_dg_peri_switch_mask : R/W; bitpos: [6:2]; default: 31;
         *  Need add description
         */
        uint32_t xpd_dg_peri_switch_mask:5;
        /** xpd_dg_wrap_switch_mask : R/W; bitpos: [11:7]; default: 31;
         *  Need add description
         */
        uint32_t xpd_dg_wrap_switch_mask:5;
        /** xpd_mem_switch_mask : R/W; bitpos: [31:12]; default: 1048575;
         *  Need add description
         */
        uint32_t xpd_mem_switch_mask:20;
    };
    uint32_t val;
} rtc_cntl_dig_power_slave0_fpu_reg_t;

/** Type of dig_power_slave1_fpu register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** xpd_wifi_switch_mask : R/W; bitpos: [26:22]; default: 31;
         *  Need add description
         */
        uint32_t xpd_wifi_switch_mask:5;
        /** xpd_cpu_switch_mask : R/W; bitpos: [31:27]; default: 31;
         *  Need add description
         */
        uint32_t xpd_cpu_switch_mask:5;
    };
    uint32_t val;
} rtc_cntl_dig_power_slave1_fpu_reg_t;

/** Type of dig_iso register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** dg_mem_force_noiso : R/W; bitpos: [5]; default: 1;
         *  Need add description
         */
        uint32_t dg_mem_force_noiso:1;
        /** dg_mem_force_iso : R/W; bitpos: [6]; default: 0;
         *  Need add description
         */
        uint32_t dg_mem_force_iso:1;
        /** dig_iso_force_off : R/W; bitpos: [7]; default: 1;
         *  Need add description
         */
        uint32_t dig_iso_force_off:1;
        /** dig_iso_force_on : R/W; bitpos: [8]; default: 0;
         *  Need add description
         */
        uint32_t dig_iso_force_on:1;
        /** dg_pad_autohold : RO; bitpos: [9]; default: 0;
         *  read only register to indicate digital pad auto-hold status
         */
        uint32_t dg_pad_autohold:1;
        /** clr_dg_pad_autohold : WO; bitpos: [10]; default: 0;
         *  wtite only register to clear digital pad auto-hold
         */
        uint32_t clr_dg_pad_autohold:1;
        /** dg_pad_autohold_en : R/W; bitpos: [11]; default: 0;
         *  digital pad enable auto-hold
         */
        uint32_t dg_pad_autohold_en:1;
        /** dg_pad_force_noiso : R/W; bitpos: [12]; default: 1;
         *  digital pad force no ISO
         */
        uint32_t dg_pad_force_noiso:1;
        /** dg_pad_force_iso : R/W; bitpos: [13]; default: 0;
         *  digital pad force ISO
         */
        uint32_t dg_pad_force_iso:1;
        /** dg_pad_force_unhold : R/W; bitpos: [14]; default: 1;
         *  digital pad force un-hold
         */
        uint32_t dg_pad_force_unhold:1;
        /** dg_pad_force_hold : R/W; bitpos: [15]; default: 0;
         *  digital pad force hold
         */
        uint32_t dg_pad_force_hold:1;
        uint32_t reserved_16:6;
        /** bt_force_iso : R/W; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t bt_force_iso:1;
        /** bt_force_noiso : R/W; bitpos: [23]; default: 1;
         *  Need add description
         */
        uint32_t bt_force_noiso:1;
        /** dg_peri_force_iso : R/W; bitpos: [24]; default: 0;
         *  Need add description
         */
        uint32_t dg_peri_force_iso:1;
        /** dg_peri_force_noiso : R/W; bitpos: [25]; default: 1;
         *  Need add description
         */
        uint32_t dg_peri_force_noiso:1;
        /** cpu_top_force_iso : R/W; bitpos: [26]; default: 0;
         *  cpu force ISO
         */
        uint32_t cpu_top_force_iso:1;
        /** cpu_top_force_noiso : R/W; bitpos: [27]; default: 1;
         *  cpu force no ISO
         */
        uint32_t cpu_top_force_noiso:1;
        /** wifi_force_iso : R/W; bitpos: [28]; default: 0;
         *  wifi force ISO
         */
        uint32_t wifi_force_iso:1;
        /** wifi_force_noiso : R/W; bitpos: [29]; default: 1;
         *  wifi force no ISO
         */
        uint32_t wifi_force_noiso:1;
        /** dg_wrap_force_iso : R/W; bitpos: [30]; default: 0;
         *  digital core force ISO
         */
        uint32_t dg_wrap_force_iso:1;
        /** dg_wrap_force_noiso : R/W; bitpos: [31]; default: 1;
         *  Need add description
         */
        uint32_t dg_wrap_force_noiso:1;
    };
    uint32_t val;
} rtc_cntl_dig_iso_reg_t;

/** Type of rtc_wdtconfig0 register
 *  register description
 */
typedef union {
    struct {
        /** wdt_chip_reset_width : R/W; bitpos: [7:0]; default: 20;
         *  chip reset siginal pulse width
         */
        uint32_t chip_reset_width:8;
        /** chip_reset_en : R/W; bitpos: [8]; default: 0;
         *  wdt reset whole chip enable
         */
        uint32_t chip_reset_en:1;
        /** pause_in_slp : R/W; bitpos: [9]; default: 1;
         *  pause WDT in sleep
         */
        uint32_t pause_in_slp:1;
        /** appcpu_reset_en : R/W; bitpos: [10]; default: 0;
         *  enable WDT reset APP CPU
         */
        uint32_t appcpu_reset_en:1;
        /** procpu_reset_en : R/W; bitpos: [11]; default: 0;
         *  enable WDT reset PRO CPU
         */
        uint32_t procpu_reset_en:1;
        /** flashboot_mod_en : R/W; bitpos: [12]; default: 1;
         *  enable WDT in flash boot
         */
        uint32_t flashboot_mod_en:1;
        /** sys_reset_length : R/W; bitpos: [15:13]; default: 1;
         *  system reset counter length
         */
        uint32_t sys_reset_length:3;
        /** cpu_reset_length : R/W; bitpos: [18:16]; default: 1;
         *  CPU reset counter length
         */
        uint32_t cpu_reset_length:3;
        /** stg3 : R/W; bitpos: [21:19]; default: 0;
         *  1: interrupt stage en, 2: CPU reset stage en, 3: system reset stage en, 4: RTC
         *  reset stage en
         */
        uint32_t stg3:3;
        /** stg2 : R/W; bitpos: [24:22]; default: 0;
         *  1: interrupt stage en, 2: CPU reset stage en, 3: system reset stage en, 4: RTC
         *  reset stage en
         */
        uint32_t stg2:3;
        /** stg1 : R/W; bitpos: [27:25]; default: 0;
         *  1: interrupt stage en, 2: CPU reset stage en, 3: system reset stage en, 4: RTC
         *  reset stage en
         */
        uint32_t stg1:3;
        /** stg0 : R/W; bitpos: [30:28]; default: 0;
         *  1: interrupt stage en, 2: CPU reset stage en, 3: system reset stage en, 4: RTC
         *  reset stage en
         */
        uint32_t stg0:3;
        /** en : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t en:1;
    };
    uint32_t val;
} rtc_cntl_wdtconfig0_reg_t;

/** Type of rtc_wdtconfig1 register
 *  register description
 */
typedef union {
    struct {
        /** wdt_stg0_hold : R/W; bitpos: [31:0]; default: 200000;
         *  Need add description
         */
        uint32_t wdt_stg0_hold:32;
    };
    uint32_t val;
} rtc_cntl_wdtconfig1_reg_t;

/** Type of rtc_wdtconfig2 register
 *  register description
 */
typedef union {
    struct {
        /** wdt_stg1_hold : R/W; bitpos: [31:0]; default: 80000;
         *  Need add description
         */
        uint32_t wdt_stg1_hold:32;
    };
    uint32_t val;
} rtc_cntl_wdtconfig2_reg_t;

/** Type of rtc_wdtconfig3 register
 *  register description
 */
typedef union {
    struct {
        /** wdt_stg2_hold : R/W; bitpos: [31:0]; default: 4095;
         *  Need add description
         */
        uint32_t wdt_stg2_hold:32;
    };
    uint32_t val;
} rtc_cntl_wdtconfig3_reg_t;

/** Type of rtc_wdtconfig4 register
 *  register description
 */
typedef union {
    struct {
        /** wdt_stg3_hold : R/W; bitpos: [31:0]; default: 4095;
         *  Need add description
         */
        uint32_t wdt_stg3_hold:32;
    };
    uint32_t val;
} rtc_cntl_wdtconfig4_reg_t;

/** Type of rtc_wdtfeed register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** feed : WO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t feed:1;
    };
    uint32_t val;
} rtc_cntl_wdtfeed_reg_t;

/** Type of rtc_wdtwprotect register
 *  register description
 */
typedef union {
    struct {
        /** wdt_wkey : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t wdt_wkey:32;
    };
    uint32_t val;
} rtc_cntl_wdtwprotect_reg_t;

/** Type of rtc_wdtreset_chip register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** reset_chip_target : R/W; bitpos: [23:16]; default: 165;
         *  Need add description
         */
        uint32_t reset_chip_target:8;
        /** reset_chip_key : R/W; bitpos: [31:24]; default: 0;
         *  Need add description
         */
        uint32_t reset_chip_key:8;
    };
    uint32_t val;
} rtc_cntl_wdtreset_chip_reg_t;

/** Type of rtc_swd_conf register
 *  register description
 */
typedef union {
    struct {
        /** swd_reset_flag : RO; bitpos: [0]; default: 0;
         *  swd reset flag
         */
        uint32_t swd_reset_flag:1;
        /** swd_feed_int : RO; bitpos: [1]; default: 0;
         *  swd interrupt for feeding
         */
        uint32_t swd_feed_int:1;
        uint32_t reserved_2:15;
        /** swd_bypass_rst : R/W; bitpos: [17]; default: 0;
         *  Need add description
         */
        uint32_t swd_bypass_rst:1;
        /** swd_signal_width : R/W; bitpos: [27:18]; default: 300;
         *  adjust signal width send to swd
         */
        uint32_t swd_signal_width:10;
        /** swd_rst_flag_clr : WO; bitpos: [28]; default: 0;
         *  reset swd reset flag
         */
        uint32_t swd_rst_flag_clr:1;
        /** swd_feed : WO; bitpos: [29]; default: 0;
         *  Sw feed swd
         */
        uint32_t swd_feed:1;
        /** swd_disable : R/W; bitpos: [30]; default: 0;
         *  disabel SWD
         */
        uint32_t swd_disable:1;
        /** swd_auto_feed_en : R/W; bitpos: [31]; default: 0;
         *  automatically feed swd when int comes
         */
        uint32_t swd_auto_feed_en:1;
    };
    uint32_t val;
} rtc_cntl_swd_conf_reg_t;

/** Type of rtc_swd_wprotect register
 *  register description
 */
typedef union {
    struct {
        /** swd_wkey : R/W; bitpos: [31:0]; default: 0;
         *  swd write protect
         */
        uint32_t swd_wkey:32;
    };
    uint32_t val;
} rtc_cntl_swd_wprotect_reg_t;

/** Type of rtc_sw_cpu_stall register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** appcpu_c1 : R/W; bitpos: [25:20]; default: 0;
         *  {reg_sw_stall_appcpu_c1[5:0],  reg_sw_stall_appcpu_c0[1:0]} == 0x86 will stall APP
         *  CPU
         */
        uint32_t appcpu_c1:6;
        /** procpu_c1 : R/W; bitpos: [31:26]; default: 0;
         *  Need add description
         */
        uint32_t procpu_c1:6;
    };
    uint32_t val;
} rtc_cntl_sw_cpu_stall_reg_t;

/** Type of rtc_low_power_st register
 *  register description
 */
typedef union {
    struct {
        /** xpd_rom0 : RO; bitpos: [0]; default: 0;
         *  rom0 power down
         */
        uint32_t xpd_rom0:1;
        uint32_t reserved_1:1;
        /** xpd_dig_dcdc : RO; bitpos: [2]; default: 0;
         *  External DCDC power down
         */
        uint32_t xpd_dig_dcdc:1;
        /** rtc_peri_iso : RO; bitpos: [3]; default: 0;
         *  rtc peripheral iso
         */
        uint32_t rtc_peri_iso:1;
        /** xpd_rtc_peri : RO; bitpos: [4]; default: 0;
         *  rtc peripheral power down
         */
        uint32_t xpd_rtc_peri:1;
        /** wifi_iso : RO; bitpos: [5]; default: 0;
         *  wifi iso
         */
        uint32_t wifi_iso:1;
        /** xpd_wifi : RO; bitpos: [6]; default: 0;
         *  wifi wrap power down
         */
        uint32_t xpd_wifi:1;
        /** dig_iso : RO; bitpos: [7]; default: 0;
         *  digital wrap iso
         */
        uint32_t dig_iso:1;
        /** xpd_dig : RO; bitpos: [8]; default: 0;
         *  digital wrap power down
         */
        uint32_t xpd_dig:1;
        /** rtc_touch_state_start : RO; bitpos: [9]; default: 0;
         *  touch should start to work
         */
        uint32_t rtc_touch_state_start:1;
        /** rtc_touch_state_switch : RO; bitpos: [10]; default: 0;
         *  touch is about to working. Switch rtc main state
         */
        uint32_t rtc_touch_state_switch:1;
        /** rtc_touch_state_slp : RO; bitpos: [11]; default: 0;
         *  touch is in sleep state
         */
        uint32_t rtc_touch_state_slp:1;
        /** rtc_touch_state_done : RO; bitpos: [12]; default: 0;
         *  touch is done
         */
        uint32_t rtc_touch_state_done:1;
        /** rtc_cocpu_state_start : RO; bitpos: [13]; default: 0;
         *  ulp/cocpu should start to work
         */
        uint32_t rtc_cocpu_state_start:1;
        /** rtc_cocpu_state_switch : RO; bitpos: [14]; default: 0;
         *  ulp/cocpu is about to working. Switch rtc main state
         */
        uint32_t rtc_cocpu_state_switch:1;
        /** rtc_cocpu_state_slp : RO; bitpos: [15]; default: 0;
         *  ulp/cocpu is in sleep state
         */
        uint32_t rtc_cocpu_state_slp:1;
        /** rtc_cocpu_state_done : RO; bitpos: [16]; default: 0;
         *  ulp/cocpu is done
         */
        uint32_t rtc_cocpu_state_done:1;
        /** rtc_main_state_xtal_iso : RO; bitpos: [17]; default: 0;
         *  no use any more
         */
        uint32_t rtc_main_state_xtal_iso:1;
        /** rtc_main_state_pll_on : RO; bitpos: [18]; default: 0;
         *  rtc main state machine is in states that pll should be running
         */
        uint32_t rtc_main_state_pll_on:1;
        /** rtc_rdy_for_wakeup : RO; bitpos: [19]; default: 0;
         *  rtc is ready to receive wake up trigger from wake up source
         */
        uint32_t rtc_rdy_for_wakeup:1;
        /** rtc_main_state_wait_end : RO; bitpos: [20]; default: 0;
         *  rtc main state machine has been waited for some cycles
         */
        uint32_t rtc_main_state_wait_end:1;
        /** rtc_in_wakeup_state : RO; bitpos: [21]; default: 0;
         *  rtc main state machine is in the states of wakeup process
         */
        uint32_t rtc_in_wakeup_state:1;
        /** rtc_in_low_power_state : RO; bitpos: [22]; default: 0;
         *  rtc main state machine is in the states of low power
         */
        uint32_t rtc_in_low_power_state:1;
        /** rtc_main_state_in_wait_8m : RO; bitpos: [23]; default: 0;
         *  rtc main state machine is in wait 8m state
         */
        uint32_t rtc_main_state_in_wait_8m:1;
        /** rtc_main_state_in_wait_pll : RO; bitpos: [24]; default: 0;
         *  rtc main state machine is in wait pll state
         */
        uint32_t rtc_main_state_in_wait_pll:1;
        /** rtc_main_state_in_wait_xtl : RO; bitpos: [25]; default: 0;
         *  rtc main state machine is in wait xtal state
         */
        uint32_t rtc_main_state_in_wait_xtl:1;
        /** rtc_main_state_in_slp : RO; bitpos: [26]; default: 0;
         *  rtc main state machine is in sleep state
         */
        uint32_t rtc_main_state_in_slp:1;
        /** rtc_main_state_in_idle : RO; bitpos: [27]; default: 0;
         *  rtc main state machine is in idle state
         */
        uint32_t rtc_main_state_in_idle:1;
        /** rtc_main_state : RO; bitpos: [31:28]; default: 0;
         *  rtc main state machine status
         */
        uint32_t rtc_main_state:4;
    };
    uint32_t val;
} rtc_cntl_low_power_st_reg_t;

/** Type of rtc_diag0 register
 *  register description
 */
typedef union {
    struct {
        /** rtc_low_power_diag1 : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t rtc_low_power_diag1:32;
    };
    uint32_t val;
} rtc_cntl_diag0_reg_t;

/** Type of rtc_pad_hold register
 *  register description
 */
typedef union {
    struct {
        /** rtc_gpio_pin0_hold : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin0_hold:1;
        /** rtc_gpio_pin1_hold : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin1_hold:1;
        /** rtc_gpio_pin2_hold : R/W; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin2_hold:1;
        /** rtc_gpio_pin3_hold : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin3_hold:1;
        /** rtc_gpio_pin4_hold : R/W; bitpos: [4]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin4_hold:1;
        /** rtc_gpio_pin5_hold : R/W; bitpos: [5]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin5_hold:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtc_cntl_pad_hold_reg_t;

/** Type of dig_pad_hold register
 *  register description
 */
typedef union {
    struct {
        /** dig_pad_hold : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t dig_pad_hold:32;
    };
    uint32_t val;
} rtc_cntl_dig_pad_hold_reg_t;

/** Type of dig_pad_hold1 register
 *  register description
 */
typedef union {
    struct {
        /** dig_pad_hold1 : R/W; bitpos: [8:0]; default: 0;
         *  Need add description
         */
        uint32_t dig_pad_hold1:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} rtc_cntl_dig_pad_hold1_reg_t;

/** Type of rtc_brown_out register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** int_wait : R/W; bitpos: [13:4]; default: 1;
         *  brown out interrupt wait cycles
         */
        uint32_t int_wait:10;
        /** close_flash_ena : R/W; bitpos: [14]; default: 0;
         *  enable close flash when brown out happens
         */
        uint32_t close_flash_ena:1;
        /** pd_rf_ena : R/W; bitpos: [15]; default: 0;
         *  enable power down RF when brown out happens
         */
        uint32_t pd_rf_ena:1;
        /** rst_wait : R/W; bitpos: [25:16]; default: 1023;
         *  brown out reset wait cycles
         */
        uint32_t rst_wait:10;
        /** rst_ena : R/W; bitpos: [26]; default: 0;
         *  enable brown out reset
         */
        uint32_t rst_ena:1;
        /** rst_sel : R/W; bitpos: [27]; default: 0;
         *  1:  4-pos reset,    0:  sys_reset
         */
        uint32_t rst_sel:1;
        /** ana_rst_en : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t ana_rst_en:1;
        /** cnt_clr : WO; bitpos: [29]; default: 0;
         *  clear brown out counter
         */
        uint32_t cnt_clr:1;
        /** ena : R/W; bitpos: [30]; default: 1;
         *  enable brown out
         */
        uint32_t ena:1;
        /** det : RO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t det:1;
    };
    uint32_t val;
} rtc_cntl_brown_out_reg_t;

/** Type of rtc_time_low1 register
 *  register description
 */
typedef union {
    struct {
        /** rtc_timer_value1_low : RO; bitpos: [31:0]; default: 0;
         *  RTC timer low 32 bits
         */
        uint32_t rtc_timer_value1_low:32;
    };
    uint32_t val;
} rtc_cntl_time_low1_reg_t;

/** Type of rtc_time_high1 register
 *  register description
 */
typedef union {
    struct {
        /** rtc_timer_value1_high : RO; bitpos: [15:0]; default: 0;
         *  RTC timer high 16 bits
         */
        uint32_t rtc_timer_value1_high:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} rtc_cntl_time_high1_reg_t;

/** Type of rtc_xtal32k_clk_factor register
 *  register description
 */
typedef union {
    struct {
        /** xtal32k_clk_factor : R/W; bitpos: [31:0]; default: 0;
         *  xtal 32k watch dog backup clock factor
         */
        uint32_t xtal32k_clk_factor:32;
    };
    uint32_t val;
} rtc_cntl_xtal32k_clk_factor_reg_t;

/** Type of rtc_xtal32k_conf register
 *  register description
 */
typedef union {
    struct {
        /** xtal32k_return_wait : R/W; bitpos: [3:0]; default: 0;
         *  cycles to wait to return noral xtal 32k
         */
        uint32_t xtal32k_return_wait:4;
        /** xtal32k_restart_wait : R/W; bitpos: [19:4]; default: 0;
         *  cycles to wait to repower on xtal 32k
         */
        uint32_t xtal32k_restart_wait:16;
        /** xtal32k_wdt_timeout : R/W; bitpos: [27:20]; default: 255;
         *  If no clock detected for this amount of time, 32k is regarded as dead
         */
        uint32_t xtal32k_wdt_timeout:8;
        /** xtal32k_stable_thres : R/W; bitpos: [31:28]; default: 0;
         *  if restarted xtal32k period is smaller than this, it is regarded as stable
         */
        uint32_t xtal32k_stable_thres:4;
    };
    uint32_t val;
} rtc_cntl_xtal32k_conf_reg_t;

/** Type of rtc_usb_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:18;
        /** io_mux_reset_disable : R/W; bitpos: [18]; default: 0;
         *  Need add description
         */
        uint32_t io_mux_reset_disable:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} rtc_cntl_usb_conf_reg_t;

/** Type of rtc_slp_reject_cause register
 *  register description
 */
typedef union {
    struct {
        /** reject_cause : RO; bitpos: [18:0]; default: 0;
         *  sleep reject cause
         */
        uint32_t reject_cause:19;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} rtc_cntl_slp_reject_cause_reg_t;

/** Type of rtc_option1 register
 *  register description
 */
typedef union {
    struct {
        /** force_download_boot : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t force_download_boot:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rtc_cntl_option1_reg_t;

/** Type of rtc_slp_wakeup_cause register
 *  register description
 */
typedef union {
    struct {
        /** wakeup_cause : RO; bitpos: [18:0]; default: 0;
         *  sleep wakeup cause
         */
        uint32_t wakeup_cause:19;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} rtc_cntl_slp_wakeup_cause_reg_t;

/** Type of rtc_ulp_cp_timer_1 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** ulp_cp_timer_slp_cycle : R/W; bitpos: [31:8]; default: 200;
         *  sleep cycles for ULP-coprocessor timer
         */
        uint32_t ulp_cp_timer_slp_cycle:24;
    };
    uint32_t val;
} rtc_cntl_ulp_cp_timer_1_reg_t;

/** Type of int_ena_w1ts register
 *  register description
 */
typedef union {
    struct {
        /** slp_wakeup_w1ts : WO; bitpos: [0]; default: 0;
         *  enable sleep wakeup interrupt
         */
        uint32_t slp_wakeup_w1ts:1;
        /** slp_reject_w1ts : WO; bitpos: [1]; default: 0;
         *  enable sleep reject interrupt
         */
        uint32_t slp_reject_w1ts:1;
        uint32_t reserved_2:1;
        /** rtc_wdt_w1ts : WO; bitpos: [3]; default: 0;
         *  enable RTC WDT interrupt
         */
        uint32_t rtc_wdt_w1ts:1;
        uint32_t reserved_4:5;
        /** w1ts : WO; bitpos: [9]; default: 0;
         *  enable brown out interrupt
         */
        uint32_t w1ts:1;
        /** rtc_main_timer_w1ts : WO; bitpos: [10]; default: 0;
         *  enable RTC main timer interrupt
         */
        uint32_t rtc_main_timer_w1ts:1;
        uint32_t reserved_11:4;
        /** rtc_swd_w1ts : WO; bitpos: [15]; default: 0;
         *  enable super watch dog interrupt
         */
        uint32_t rtc_swd_w1ts:1;
        /** rtc_xtal32k_dead_w1ts : WO; bitpos: [16]; default: 0;
         *  enable xtal32k_dead  interrupt
         */
        uint32_t rtc_xtal32k_dead_w1ts:1;
        uint32_t reserved_17:2;
        /** rtc_glitch_det_w1ts : WO; bitpos: [19]; default: 0;
         *  enbale gitch det interrupt
         */
        uint32_t rtc_glitch_det_w1ts:1;
        /** rtc_bbpll_cal_w1ts : WO; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_bbpll_cal_w1ts:1;
        /** rtc_ble_compare_wake_w1ts : WO; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_compare_wake_w1ts:1;
        /** vset_dcdc_done_w1ts : WO; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_done_w1ts:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_cntl_int_ena_w1ts_reg_t;

/** Type of int_ena_w1tc register
 *  register description
 */
typedef union {
    struct {
        /** slp_wakeup_w1tc : WO; bitpos: [0]; default: 0;
         *  enable sleep wakeup interrupt
         */
        uint32_t slp_wakeup_w1tc:1;
        /** slp_reject_w1tc : WO; bitpos: [1]; default: 0;
         *  enable sleep reject interrupt
         */
        uint32_t slp_reject_w1tc:1;
        uint32_t reserved_2:1;
        /** rtc_wdt_w1tc : WO; bitpos: [3]; default: 0;
         *  enable RTC WDT interrupt
         */
        uint32_t rtc_wdt_w1tc:1;
        uint32_t reserved_4:5;
        /** rtc_brown_out_w1tc : WO; bitpos: [9]; default: 0;
         *  enable brown out interrupt
         */
        uint32_t rtc_brown_out_w1tc:1;
        /** rtc_main_timer_w1tc : WO; bitpos: [10]; default: 0;
         *  enable RTC main timer interrupt
         */
        uint32_t rtc_main_timer_w1tc:1;
        uint32_t reserved_11:4;
        /** rtc_swd_w1tc : WO; bitpos: [15]; default: 0;
         *  enable super watch dog interrupt
         */
        uint32_t rtc_swd_w1tc:1;
        /** rtc_xtal32k_dead_w1tc : WO; bitpos: [16]; default: 0;
         *  enable xtal32k_dead  interrupt
         */
        uint32_t rtc_xtal32k_dead_w1tc:1;
        uint32_t reserved_17:2;
        /** rtc_glitch_det_w1tc : WO; bitpos: [19]; default: 0;
         *  enbale gitch det interrupt
         */
        uint32_t rtc_glitch_det_w1tc:1;
        /** rtc_bbpll_cal_w1tc : WO; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_bbpll_cal_w1tc:1;
        /** rtc_ble_compare_wake_w1tc : WO; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t rtc_ble_compare_wake_w1tc:1;
        /** vset_dcdc_done_w1tc : WO; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_done_w1tc:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_cntl_int_ena_w1tc_reg_t;

/** Type of rtc_cntl_retention_ctrl register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:17;
        /** clk_en : R/W; bitpos: [17]; default: 0;
         *  Need add description
         */
        uint32_t clk_en:1;
        /** retention_clk_sel : R/W; bitpos: [18]; default: 0;
         *  Need add description
         */
        uint32_t retention_clk_sel:1;
        /** retention_done_wait : R/W; bitpos: [21:19]; default: 2;
         *  Need add description
         */
        uint32_t retention_done_wait:3;
        /** retention_clkoff_wait : R/W; bitpos: [25:22]; default: 3;
         *  Need add description
         */
        uint32_t retention_clkoff_wait:4;
        /** retention_en : R/W; bitpos: [26]; default: 0;
         *  Need add description
         */
        uint32_t retention_en:1;
        /** retention_wait : R/W; bitpos: [31:27]; default: 20;
         *  wait cycles for rention operation
         */
        uint32_t retention_wait:5;
    };
    uint32_t val;
} rtc_cntl_retention_ctrl_reg_t;

/** Type of rtc_cntl_retention_ctrl1 register
 *  register description
 */
typedef union {
    struct {
        /** retention_link_addr : R/W; bitpos: [26:0]; default: 0;
         *  Need add description
         */
        uint32_t retention_link_addr:27;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} rtc_cntl_retention_ctrl1_reg_t;

/** Type of rtc_fib_sel register
 *  register description
 */
typedef union {
    struct {
        /** rtc_fib_sel : R/W; bitpos: [2:0]; default: 7;
         *  select use analog fib signal
         */
        uint32_t rtc_fib_sel:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} rtc_cntl_fib_sel_reg_t;

/** Type of rtc_cntl_gpio_wakeup register
 *  register description
 */
typedef union {
    struct {
        /** rtc_gpio_wakeup_status : RO; bitpos: [5:0]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_wakeup_status:6;
        /** rtc_gpio_wakeup_status_clr : R/W; bitpos: [6]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_wakeup_status_clr:1;
        /** rtc_gpio_pin_clk_gate : R/W; bitpos: [7]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin_clk_gate:1;
        /** rtc_gpio_pin5_int_type : R/W; bitpos: [10:8]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin5_int_type:3;
        /** rtc_gpio_pin4_int_type : R/W; bitpos: [13:11]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin4_int_type:3;
        /** rtc_gpio_pin3_int_type : R/W; bitpos: [16:14]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin3_int_type:3;
        /** rtc_gpio_pin2_int_type : R/W; bitpos: [19:17]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin2_int_type:3;
        /** rtc_gpio_pin1_int_type : R/W; bitpos: [22:20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin1_int_type:3;
        /** rtc_gpio_pin0_int_type : R/W; bitpos: [25:23]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin0_int_type:3;
        /** rtc_gpio_pin5_wakeup_enable : R/W; bitpos: [26]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin5_wakeup_enable:1;
        /** rtc_gpio_pin4_wakeup_enable : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin4_wakeup_enable:1;
        /** rtc_gpio_pin3_wakeup_enable : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin3_wakeup_enable:1;
        /** rtc_gpio_pin2_wakeup_enable : R/W; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin2_wakeup_enable:1;
        /** rtc_gpio_pin1_wakeup_enable : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin1_wakeup_enable:1;
        /** rtc_gpio_pin0_wakeup_enable : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin0_wakeup_enable:1;
    };
    uint32_t val;
} rtc_cntl_gpio_wakeup_reg_t;

/** Type of rtc_cntl_dbg_sel register
 *  register description
 */
typedef union {
    struct {
        /** rtc_mtdi_enamux : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t rtc_mtdi_enamux:1;
        /** rtc_debug_12m_no_gating : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t rtc_debug_12m_no_gating:1;
        /** rtc_debug_bit_sel : R/W; bitpos: [6:2]; default: 0;
         *  Need add description
         */
        uint32_t rtc_debug_bit_sel:5;
        /** rtc_debug_sel0 : R/W; bitpos: [11:7]; default: 0;
         *  Need add description
         */
        uint32_t rtc_debug_sel0:5;
        /** rtc_debug_sel1 : R/W; bitpos: [16:12]; default: 0;
         *  Need add description
         */
        uint32_t rtc_debug_sel1:5;
        /** rtc_debug_sel2 : R/W; bitpos: [21:17]; default: 0;
         *  Need add description
         */
        uint32_t rtc_debug_sel2:5;
        /** rtc_debug_sel3 : R/W; bitpos: [26:22]; default: 0;
         *  Need add description
         */
        uint32_t rtc_debug_sel3:5;
        /** rtc_debug_sel4 : R/W; bitpos: [31:27]; default: 0;
         *  Need add description
         */
        uint32_t rtc_debug_sel4:5;
    };
    uint32_t val;
} rtc_cntl_dbg_sel_reg_t;

/** Type of rtc_cntl_dbg_map register
 *  register description
 */
typedef union {
    struct {
        /** vdd_dig_test : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t vdd_dig_test:2;
        /** rtc_gpio_pin5_mux_sel : R/W; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin5_mux_sel:1;
        /** rtc_gpio_pin4_mux_sel : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin4_mux_sel:1;
        /** rtc_gpio_pin3_mux_sel : R/W; bitpos: [4]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin3_mux_sel:1;
        /** rtc_gpio_pin2_mux_sel : R/W; bitpos: [5]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin2_mux_sel:1;
        /** rtc_gpio_pin1_mux_sel : R/W; bitpos: [6]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin1_mux_sel:1;
        /** rtc_gpio_pin0_mux_sel : R/W; bitpos: [7]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin0_mux_sel:1;
        /** rtc_gpio_pin5_fun_sel : R/W; bitpos: [11:8]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin5_fun_sel:4;
        /** rtc_gpio_pin4_fun_sel : R/W; bitpos: [15:12]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin4_fun_sel:4;
        /** rtc_gpio_pin3_fun_sel : R/W; bitpos: [19:16]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin3_fun_sel:4;
        /** rtc_gpio_pin2_fun_sel : R/W; bitpos: [23:20]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin2_fun_sel:4;
        /** rtc_gpio_pin1_fun_sel : R/W; bitpos: [27:24]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin1_fun_sel:4;
        /** rtc_gpio_pin0_fun_sel : R/W; bitpos: [31:28]; default: 0;
         *  Need add description
         */
        uint32_t rtc_gpio_pin0_fun_sel:4;
    };
    uint32_t val;
} rtc_cntl_dbg_map_reg_t;

/** Type of rtc_cntl_dbg_sar_sel register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** sar_debug_sel : R/W; bitpos: [31:27]; default: 0;
         *  Need add description
         */
        uint32_t sar_debug_sel:5;
    };
    uint32_t val;
} rtc_cntl_dbg_sar_sel_reg_t;

/** Type of rtc_cntl_pg_ctrl register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** power_glitch_dsense : R/W; bitpos: [27:26]; default: 0;
         *  Need add description
         */
        uint32_t power_glitch_dsense:2;
        /** power_glitch_force_pd : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t power_glitch_force_pd:1;
        /** power_glitch_force_pu : R/W; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t power_glitch_force_pu:1;
        /** power_glitch_efuse_sel : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t power_glitch_efuse_sel:1;
        /** power_glitch_en : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t power_glitch_en:1;
    };
    uint32_t val;
} rtc_cntl_pg_ctrl_reg_t;

/** Type of rtc_cntl_dcdc_ctrl0 register
 *  register description
 */
typedef union {
    struct {
        /** vset_dcdc_value : RO; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_value:5;
        /** power_good_dcdc : RO; bitpos: [5]; default: 1;
         *  Need add description
         */
        uint32_t power_good_dcdc:1;
        uint32_t reserved_6:13;
        /** pmu_mode : R/W; bitpos: [20:19]; default: 0;
         *  Need add description
         */
        uint32_t pmu_mode:2;
        /** ramplevel_dcdc : R/W; bitpos: [21]; default: 0;
         *  Need add description
         */
        uint32_t ramplevel_dcdc:1;
        /** ramp_dcdc : R/W; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t ramp_dcdc:1;
        /** dcm2enb_dcdc : R/W; bitpos: [23]; default: 0;
         *  Need add description
         */
        uint32_t dcm2enb_dcdc:1;
        /** dcmlevel_dcdc : R/W; bitpos: [25:24]; default: 0;
         *  Need add description
         */
        uint32_t dcmlevel_dcdc:2;
        /** fsw_dcdc : R/W; bitpos: [28:26]; default: 0;
         *  Need add description
         */
        uint32_t fsw_dcdc:3;
        /** ccm_dcdc : R/W; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t ccm_dcdc:1;
        /** sstime_dcdc : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t sstime_dcdc:1;
        /** pocpenb_dcdc : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t pocpenb_dcdc:1;
    };
    uint32_t val;
} rtc_cntl_dcdc_ctrl0_reg_t;

/** Type of rtc_cntl_dcdc_ctrl1 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** dcdc_mode_slp : R/W; bitpos: [25:23]; default: 4;
         *  Need add description
         */
        uint32_t dcdc_mode_slp:3;
        /** dcdc_mode_monitor : R/W; bitpos: [28:26]; default: 4;
         *  Need add description
         */
        uint32_t dcdc_mode_monitor:3;
        /** dcdc_mode_idle : R/W; bitpos: [31:29]; default: 4;
         *  Need add description
         */
        uint32_t dcdc_mode_idle:3;
    };
    uint32_t val;
} rtc_cntl_dcdc_ctrl1_reg_t;

/** Type of rtc_cntl_dcdc_ctrl2 register
 *  register description
 */
typedef union {
    struct {
        /** vset_dcdc_target_value1 : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_target_value1:5;
        /** vset_dcdc_target_value0 : R/W; bitpos: [9:5]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_target_value0:5;
        /** vset_dcdc_init_value : R/W; bitpos: [14:10]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_init_value:5;
        /** vset_dcdc_init : WO; bitpos: [15]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_init:1;
        /** vset_dcdc_fix : R/W; bitpos: [16]; default: 1;
         *  Need add description
         */
        uint32_t vset_dcdc_fix:1;
        /** vset_dcdc_step : R/W; bitpos: [21:17]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_step:5;
        /** vset_dcdc_gap : R/W; bitpos: [26:22]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_gap:5;
        /** vset_dcdc_sel_hw_sw : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_sel_hw_sw:1;
        /** vset_dcdc_sw_sel : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t vset_dcdc_sw_sel:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} rtc_cntl_dcdc_ctrl2_reg_t;

/** Type of rtc_cntl_rc32k_ctrl register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** rc32k_dfreq : R/W; bitpos: [30:21]; default: 511;
         *  Need add description
         */
        uint32_t rc32k_dfreq:10;
        /** rc32k_xpd : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t rc32k_xpd:1;
    };
    uint32_t val;
} rtc_cntl_rc32k_ctrl_reg_t;

/** Type of rtc_cntl_pll8m register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** ckref_pll8m_sel : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t ckref_pll8m_sel:1;
        /** xpd_pll8m : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t xpd_pll8m:1;
    };
    uint32_t val;
} rtc_cntl_pll8m_reg_t;

/** Type of rtc_cntl_date register
 *  register description
 */
typedef union {
    struct {
        /** rtc_cntl_date : R/W; bitpos: [27:0]; default: 34640480;
         *  Need add description
         */
        uint32_t rtc_cntl_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} rtc_cntl_date_reg_t;


typedef struct {
    volatile rtc_cntl_options0_reg_t options0;
    volatile rtc_cntl_slp_timer0_reg_t slp_timer0;
    volatile rtc_cntl_slp_timer1_reg_t slp_timer1;
    volatile rtc_cntl_time_update_reg_t time_update;
    volatile rtc_cntl_time_low0_reg_t time_low0;
    volatile rtc_cntl_time_high0_reg_t time_high0;
    volatile rtc_cntl_state0_reg_t state0;
    volatile rtc_cntl_timer1_reg_t timer1;
    volatile rtc_cntl_timer2_reg_t timer2;
    volatile rtc_cntl_timer3_reg_t timer3;
    volatile rtc_cntl_timer4_reg_t timer4;
    volatile rtc_cntl_timer5_reg_t timer5;
    volatile rtc_cntl_timer6_reg_t timer6;
    volatile rtc_cntl_ana_conf_reg_t ana_conf;
    volatile rtc_cntl_reset_state_reg_t reset_state;
    volatile rtc_cntl_wakeup_state_reg_t wakeup_state;
    volatile rtc_cntl_int_ena_reg_t int_ena;
    volatile rtc_cntl_int_raw_reg_t int_raw;
    volatile rtc_cntl_int_st_reg_t int_st;
    volatile rtc_cntl_int_clr_reg_t int_clr;
    volatile rtc_cntl_store_reg_t store[4];
    volatile rtc_cntl_ext_xtl_conf_reg_t ext_xtl_conf;
    volatile rtc_cntl_ext_wakeup_conf_reg_t ext_wakeup_conf;
    volatile rtc_cntl_slp_reject_conf_reg_t slp_reject_conf;
    volatile rtc_cntl_cpu_period_conf_reg_t cpu_period_conf;
    volatile rtc_cntl_clk_conf_reg_t clk_conf;
    volatile rtc_cntl_slow_clk_conf_reg_t slow_clk_conf;
    volatile rtc_cntl_sdio_conf_reg_t sdio_conf;
    volatile rtc_cntl_bias_conf_reg_t bias_conf;
    volatile rtc_cntl_regulator_reg_t regulator;
    volatile rtc_cntl_regulator0_dbias_reg_t regulator0_dbias;
    volatile rtc_cntl_regulator1_dbias_reg_t regulator1_dbias;
    volatile rtc_cntl_dig_regulator_reg_t dig_regulator;
    volatile rtc_cntl_dig_regulator_drvb_reg_t dig_regulator_drvb;
    volatile rtc_cntl_dig_regulator0_dbias_reg_t dig_regulator0_dbias;
    volatile rtc_cntl_dig_regulator1_dbias_reg_t dig_regulator1_dbias;
    volatile rtc_cntl_pwc_reg_t rtc_pwc;
    volatile rtc_cntl_dig_pwc_reg_t dig_pwc;
    volatile rtc_cntl_dig_power_slave0_pd_reg_t dig_power_slave0_pd;
    volatile rtc_cntl_dig_power_slave1_pd_reg_t dig_power_slave1_pd;
    volatile rtc_cntl_dig_power_slave0_fpu_reg_t dig_power_slave0_fpu;
    volatile rtc_cntl_dig_power_slave1_fpu_reg_t dig_power_slave1_fpu;
    volatile rtc_cntl_dig_iso_reg_t dig_iso;
    volatile rtc_cntl_wdtconfig0_reg_t wdt_config0;
    union  {
        volatile rtc_cntl_wdtconfig1_reg_t wdtconfig1;
        volatile uint32_t wdt_config1;
    };
    union  {
        volatile rtc_cntl_wdtconfig2_reg_t wdtconfig2;
        volatile uint32_t wdt_config2;
    };
    union  {
        volatile rtc_cntl_wdtconfig3_reg_t wdtconfig3;
        volatile uint32_t wdt_config3;
    };
    union  {
        volatile rtc_cntl_wdtconfig4_reg_t wdtconfig4;
        volatile uint32_t wdt_config4;
    };
    volatile rtc_cntl_wdtfeed_reg_t wdt_feed;
    union {
        volatile rtc_cntl_wdtwprotect_reg_t wdtwprotect;
        volatile uint32_t wdt_wprotect;
    };
    volatile rtc_cntl_wdtreset_chip_reg_t wdtreset_chip;
    volatile rtc_cntl_swd_conf_reg_t swd_conf;
    volatile rtc_cntl_swd_wprotect_reg_t swd_wprotect;
    volatile rtc_cntl_sw_cpu_stall_reg_t sw_cpu_stall;
    volatile rtc_cntl_store_reg_t store4[4];
    volatile rtc_cntl_low_power_st_reg_t low_power_st;
    volatile rtc_cntl_diag0_reg_t diag0;
    volatile rtc_cntl_pad_hold_reg_t pad_hold;
    volatile rtc_cntl_dig_pad_hold_reg_t dig_pad_hold;
    volatile rtc_cntl_dig_pad_hold1_reg_t dig_pad_hold1;
    volatile rtc_cntl_brown_out_reg_t brown_out;
    volatile rtc_cntl_time_low1_reg_t time_low1;
    volatile rtc_cntl_time_high1_reg_t time_high1;
    volatile rtc_cntl_xtal32k_clk_factor_reg_t xtal32k_clk_factor;
    volatile rtc_cntl_xtal32k_conf_reg_t xtal32k_conf;
    volatile rtc_cntl_usb_conf_reg_t usb_conf;
    volatile rtc_cntl_slp_reject_cause_reg_t slp_reject_cause;
    volatile rtc_cntl_option1_reg_t option1;
    volatile rtc_cntl_slp_wakeup_cause_reg_t slp_wakeup_cause;
    volatile rtc_cntl_ulp_cp_timer_1_reg_t ulp_cp_timer_1;
    volatile rtc_cntl_int_ena_w1ts_reg_t int_ena_w1ts;
    volatile rtc_cntl_int_ena_w1tc_reg_t int_ena_w1tc;
    volatile rtc_cntl_retention_ctrl_reg_t retention_ctrl;
    volatile rtc_cntl_retention_ctrl1_reg_t retention_ctrl1;
    volatile rtc_cntl_fib_sel_reg_t fib_sel;
    volatile rtc_cntl_gpio_wakeup_reg_t gpio_wakeup;
    volatile rtc_cntl_dbg_sel_reg_t dbg_sel;
    volatile rtc_cntl_dbg_map_reg_t dbg_map;
    volatile rtc_cntl_dbg_sar_sel_reg_t dbg_sar_sel;
    volatile rtc_cntl_pg_ctrl_reg_t pg_ctrl;
    volatile rtc_cntl_dcdc_ctrl0_reg_t dcdc_ctrl0;
    volatile rtc_cntl_dcdc_ctrl1_reg_t dcdc_ctrl1;
    volatile rtc_cntl_dcdc_ctrl2_reg_t dcdc_ctrl2;
    volatile rtc_cntl_rc32k_ctrl_reg_t rc32k_ctrl;
    volatile rtc_cntl_pll8m_reg_t pll8m;
    uint32_t reserved_16c[36];
    volatile rtc_cntl_date_reg_t date;
} rtc_cntl_dev_t;

extern rtc_cntl_dev_t RTCCNTL;

#ifndef __cplusplus
_Static_assert(sizeof(rtc_cntl_dev_t) == 0x200, "Invalid size of rtc_cntl_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
