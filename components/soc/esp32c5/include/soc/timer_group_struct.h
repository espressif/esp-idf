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

/** Group: T0 Control and configuration registers */
/** Type of txconfig register
 *  Timer x configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** tx_alarm_en : R/W/SC; bitpos: [10]; default: 0;
         *  When set, the alarm is enabled. This bit is automatically cleared once an
         *  alarm occurs.
         */
        uint32_t tx_alarm_en:1;
        uint32_t reserved_11:1;
        /** tx_divcnt_rst : WT; bitpos: [12]; default: 0;
         *  When set, Timer x 's clock divider counter will be reset.
         */
        uint32_t tx_divcnt_rst:1;
        /** tx_divider : R/W; bitpos: [28:13]; default: 1;
         *  Timer x clock (Tx_clk) prescaler value.
         */
        uint32_t tx_divider:16;
        /** tx_autoreload : R/W; bitpos: [29]; default: 1;
         *  When set, timer x auto-reload at alarm is enabled.
         */
        uint32_t tx_autoreload:1;
        /** tx_increase : R/W; bitpos: [30]; default: 1;
         *  When set, the timer x time-base counter will increment every clock tick. When
         *  cleared, the timer x time-base counter will decrement.
         */
        uint32_t tx_increase:1;
        /** tx_en : R/W/SS/SC; bitpos: [31]; default: 0;
         *  When set, the timer x time-base counter is enabled.
         */
        uint32_t tx_en:1;
    };
    uint32_t val;
} timg_txconfig_reg_t;

/** Type of txlo register
 *  Timer x current value, low 32 bits
 */
typedef union {
    struct {
        /** tx_lo : RO; bitpos: [31:0]; default: 0;
         *  After writing to TIMG_TxUPDATE_REG, the low 32 bits of the time-base counter
         *  of timer x can be read here.
         */
        uint32_t tx_lo:32;
    };
    uint32_t val;
} timg_txlo_reg_t;

/** Type of txhi register
 *  Timer x current value, high 22 bits
 */
typedef union {
    struct {
        /** tx_hi : RO; bitpos: [21:0]; default: 0;
         *  After writing to TIMG_TxUPDATE_REG, the high 22 bits of the time-base counter
         *  of timer x can be read here.
         */
        uint32_t tx_hi:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} timg_txhi_reg_t;

/** Type of txupdate register
 *  Write to copy current timer value to TIMGn_Tx_(LO/HI)_REG
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** tx_update : R/W/SC; bitpos: [31]; default: 0;
         *  After writing 0 or 1 to TIMG_TxUPDATE_REG, the counter value is latched.
         */
        uint32_t tx_update:1;
    };
    uint32_t val;
} timg_txupdate_reg_t;

/** Type of txalarmlo register
 *  Timer x alarm value, low 32 bits
 */
typedef union {
    struct {
        /** tx_alarm_lo : R/W; bitpos: [31:0]; default: 0;
         *  Timer x alarm trigger time-base counter value, low 32 bits.
         */
        uint32_t tx_alarm_lo:32;
    };
    uint32_t val;
} timg_txalarmlo_reg_t;

/** Type of txalarmhi register
 *  Timer x alarm value, high bits
 */
typedef union {
    struct {
        /** tx_alarm_hi : R/W; bitpos: [21:0]; default: 0;
         *  Timer x alarm trigger time-base counter value, high 22 bits.
         */
        uint32_t tx_alarm_hi:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} timg_txalarmhi_reg_t;

/** Type of txloadlo register
 *  Timer x reload value, low 32 bits
 */
typedef union {
    struct {
        /** tx_load_lo : R/W; bitpos: [31:0]; default: 0;
         *  Low 32 bits of the value that a reload will load onto timer x time-base
         *  Counter.
         */
        uint32_t tx_load_lo:32;
    };
    uint32_t val;
} timg_txloadlo_reg_t;

/** Type of txloadhi register
 *  Timer x reload value, high 22 bits
 */
typedef union {
    struct {
        /** tx_load_hi : R/W; bitpos: [21:0]; default: 0;
         *  High 22 bits of the value that a reload will load onto timer x time-base
         *  counter.
         */
        uint32_t tx_load_hi:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} timg_txloadhi_reg_t;

/** Type of txload register
 *  Write to reload timer from TIMG_Tx_(LOADLOLOADHI)_REG
 */
typedef union {
    struct {
        /** tx_load : WT; bitpos: [31:0]; default: 0;
         *
         *  Write any value to trigger a timer x time-base counter reload.
         */
        uint32_t tx_load:32;
    };
    uint32_t val;
} timg_txload_reg_t;


/** Group: WDT Control and configuration registers */
/** Type of wdtconfig0 register
 *  Watchdog timer configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** wdt_appcpu_reset_en : R/W; bitpos: [12]; default: 0;
         *  WDT reset CPU enable.
         */
        uint32_t wdt_appcpu_reset_en:1;
        /** wdt_procpu_reset_en : R/W; bitpos: [13]; default: 0;
         *  WDT reset CPU enable.
         */
        uint32_t wdt_procpu_reset_en:1;
        /** wdt_flashboot_mod_en : R/W; bitpos: [14]; default: 1;
         *  When set, Flash boot protection is enabled.
         */
        uint32_t wdt_flashboot_mod_en:1;
        /** wdt_sys_reset_length : R/W; bitpos: [17:15]; default: 1;
         *  System reset signal length selection. 0: 100 ns, 1: 200 ns,
         *  2: 300 ns, 3: 400 ns, 4: 500 ns, 5: 800 ns, 6: 1.6 us, 7: 3.2 us.
         */
        uint32_t wdt_sys_reset_length:3;
        /** wdt_cpu_reset_length : R/W; bitpos: [20:18]; default: 1;
         *  CPU reset signal length selection. 0: 100 ns, 1: 200 ns,
         *  2: 300 ns, 3: 400 ns, 4: 500 ns, 5: 800 ns, 6: 1.6 us, 7: 3.2 us.
         */
        uint32_t wdt_cpu_reset_length:3;
        uint32_t reserved_21:1;
        /** wdt_conf_update_en : WT; bitpos: [22]; default: 0;
         *  update the WDT configuration registers
         */
        uint32_t wdt_conf_update_en:1;
        /** wdt_stg3 : R/W; bitpos: [24:23]; default: 0;
         *  Stage 3 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         */
        uint32_t wdt_stg3:2;
        /** wdt_stg2 : R/W; bitpos: [26:25]; default: 0;
         *  Stage 2 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         */
        uint32_t wdt_stg2:2;
        /** wdt_stg1 : R/W; bitpos: [28:27]; default: 0;
         *  Stage 1 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         */
        uint32_t wdt_stg1:2;
        /** wdt_stg0 : R/W; bitpos: [30:29]; default: 0;
         *  Stage 0 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         */
        uint32_t wdt_stg0:2;
        /** wdt_en : R/W; bitpos: [31]; default: 0;
         *  When set, MWDT is enabled.
         */
        uint32_t wdt_en:1;
    };
    uint32_t val;
} timg_wdtconfig0_reg_t;

/** Type of wdtconfig1 register
 *  Watchdog timer prescaler register
 */
typedef union {
    struct {
        /** wdt_divcnt_rst : WT; bitpos: [0]; default: 0;
         *  When set, WDT 's clock divider counter will be reset.
         */
        uint32_t wdt_divcnt_rst:1;
        uint32_t reserved_1:15;
        /** wdt_clk_prescale : R/W; bitpos: [31:16]; default: 1;
         *  MWDT clock prescaler value. MWDT clock period = 12.5 ns *
         *  TIMG_WDT_CLK_PRESCALE.
         */
        uint32_t wdt_clk_prescale:16;
    };
    uint32_t val;
} timg_wdtconfig1_reg_t;

/** Type of wdtconfig2 register
 *  Watchdog timer stage 0 timeout value
 */
typedef union {
    struct {
        /** wdt_stg0_hold : R/W; bitpos: [31:0]; default: 26000000;
         *  Stage 0 timeout value, in MWDT clock cycles.
         */
        uint32_t wdt_stg0_hold:32;
    };
    uint32_t val;
} timg_wdtconfig2_reg_t;

/** Type of wdtconfig3 register
 *  Watchdog timer stage 1 timeout value
 */
typedef union {
    struct {
        /** wdt_stg1_hold : R/W; bitpos: [31:0]; default: 134217727;
         *  Stage 1 timeout value, in MWDT clock cycles.
         */
        uint32_t wdt_stg1_hold:32;
    };
    uint32_t val;
} timg_wdtconfig3_reg_t;

/** Type of wdtconfig4 register
 *  Watchdog timer stage 2 timeout value
 */
typedef union {
    struct {
        /** wdt_stg2_hold : R/W; bitpos: [31:0]; default: 1048575;
         *  Stage 2 timeout value, in MWDT clock cycles.
         */
        uint32_t wdt_stg2_hold:32;
    };
    uint32_t val;
} timg_wdtconfig4_reg_t;

/** Type of wdtconfig5 register
 *  Watchdog timer stage 3 timeout value
 */
typedef union {
    struct {
        /** wdt_stg3_hold : R/W; bitpos: [31:0]; default: 1048575;
         *  Stage 3 timeout value, in MWDT clock cycles.
         */
        uint32_t wdt_stg3_hold:32;
    };
    uint32_t val;
} timg_wdtconfig5_reg_t;

/** Type of wdtfeed register
 *  Write to feed the watchdog timer
 */
typedef union {
    struct {
        /** wdt_feed : WT; bitpos: [31:0]; default: 0;
         *  Write any value to feed the MWDT. (WO)
         */
        uint32_t wdt_feed:32;
    };
    uint32_t val;
} timg_wdtfeed_reg_t;

/** Type of wdtwprotect register
 *  Watchdog write protect register
 */
typedef union {
    struct {
        /** wdt_wkey : R/W; bitpos: [31:0]; default: 1356348065;
         *  If the register contains a different value than its reset value, write
         *  protection is enabled.
         */
        uint32_t wdt_wkey:32;
    };
    uint32_t val;
} timg_wdtwprotect_reg_t;


/** Group: RTC CALI Control and configuration registers */
/** Type of rtccalicfg register
 *  RTC calibration configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** rtc_cali_start_cycling : R/W; bitpos: [12]; default: 1;
         *  0: one-shot frequency calculation,1: periodic frequency calculation,
         */
        uint32_t rtc_cali_start_cycling:1;
        /** rtc_cali_clk_sel : R/W; bitpos: [14:13]; default: 0;
         *  0:rtc slow clock. 1:clk_8m, 2:xtal_32k.
         */
        uint32_t rtc_cali_clk_sel:2;
        /** rtc_cali_rdy : RO; bitpos: [15]; default: 0;
         *  indicate one-shot frequency calculation is done.
         */
        uint32_t rtc_cali_rdy:1;
        /** rtc_cali_max : R/W; bitpos: [30:16]; default: 1;
         *  Configure the time to calculate RTC slow clock's frequency.
         */
        uint32_t rtc_cali_max:15;
        /** rtc_cali_start : R/W; bitpos: [31]; default: 0;
         *  Set this bit to start one-shot frequency calculation.
         */
        uint32_t rtc_cali_start:1;
    };
    uint32_t val;
} timg_rtccalicfg_reg_t;

/** Type of rtccalicfg1 register
 *  RTC calibration configure1 register
 */
typedef union {
    struct {
        /** rtc_cali_cycling_data_vld : RO; bitpos: [0]; default: 0;
         *  indicate periodic frequency calculation is done.
         */
        uint32_t rtc_cali_cycling_data_vld:1;
        uint32_t reserved_1:6;
        /** rtc_cali_value : RO; bitpos: [31:7]; default: 0;
         *  When one-shot or periodic frequency calculation is done, read this value to
         *  calculate RTC slow clock's frequency.
         */
        uint32_t rtc_cali_value:25;
    };
    uint32_t val;
} timg_rtccalicfg1_reg_t;

/** Type of rtccalicfg2 register
 *  Timer group calibration register
 */
typedef union {
    struct {
        /** rtc_cali_timeout : RO; bitpos: [0]; default: 0;
         *  RTC calibration timeout indicator
         */
        uint32_t rtc_cali_timeout:1;
        uint32_t reserved_1:2;
        /** rtc_cali_timeout_rst_cnt : R/W; bitpos: [6:3]; default: 3;
         *  Cycles that release calibration timeout reset
         */
        uint32_t rtc_cali_timeout_rst_cnt:4;
        /** rtc_cali_timeout_thres : R/W; bitpos: [31:7]; default: 33554431;
         *  Threshold value for the RTC calibration timer. If the calibration timer's value
         *  exceeds this threshold, a timeout is triggered.
         */
        uint32_t rtc_cali_timeout_thres:25;
    };
    uint32_t val;
} timg_rtccalicfg2_reg_t;


/** Group: Interrupt registers */
/** Type of int_ena_timers register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** t0_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the TIMG_T$x_INT interrupt.
         */
        uint32_t t0_int_ena:1;
        uint32_t reserved_1:1;
        /** wdt_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} timg_int_ena_timers_reg_t;

/** Type of int_raw_timers register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** t0_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the TIMG_T$x_INT interrupt.
         */
        uint32_t t0_int_raw:1;
        uint32_t reserved_1:1;
        /** wdt_int_raw : R/SS/WTC; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} timg_int_raw_timers_reg_t;

/** Type of int_st_timers register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** t0_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit for the TIMG_T$x_INT interrupt.
         */
        uint32_t t0_int_st:1;
        uint32_t reserved_1:1;
        /** wdt_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit for the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} timg_int_st_timers_reg_t;

/** Type of int_clr_timers register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** t0_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the TIMG_T$x_INT interrupt.
         */
        uint32_t t0_int_clr:1;
        uint32_t reserved_1:1;
        /** wdt_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} timg_int_clr_timers_reg_t;


/** Group: Version register */
/** Type of ntimers_date register
 *  Timer version control register
 */
typedef union {
    struct {
        /** ntimgs_date : R/W; bitpos: [27:0]; default: 35688770;
         *  Timer version control register
         */
        uint32_t ntimgs_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} timg_ntimers_date_reg_t;


/** Group: Clock configuration registers */
/** Type of regclk register
 *  Timer group clock gate register
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** etm_en : R/W; bitpos: [28]; default: 1;
         *  enable timer's etm task and event
         */
        uint32_t etm_en:1;
        uint32_t reserved_29:2;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Register clock gate signal. 1: Registers can be read and written to by software. 0:
         *  Registers can not be read or written to by software.
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} timg_regclk_reg_t;


typedef struct {
    volatile timg_txconfig_reg_t config;
    volatile timg_txlo_reg_t lo;
    volatile timg_txhi_reg_t hi;
    volatile timg_txupdate_reg_t update;
    volatile timg_txalarmlo_reg_t alarmlo;
    volatile timg_txalarmhi_reg_t alarmhi;
    volatile timg_txloadlo_reg_t loadlo;
    volatile timg_txloadhi_reg_t loadhi;
    volatile timg_txload_reg_t load;
} timg_hwtimer_reg_t;

typedef struct timg_dev_t {
    volatile timg_hwtimer_reg_t hw_timer[1];
    uint32_t reserved_024[9];
    volatile timg_wdtconfig0_reg_t wdtconfig0;
    volatile timg_wdtconfig1_reg_t wdtconfig1;
    volatile timg_wdtconfig2_reg_t wdtconfig2;
    volatile timg_wdtconfig3_reg_t wdtconfig3;
    volatile timg_wdtconfig4_reg_t wdtconfig4;
    volatile timg_wdtconfig5_reg_t wdtconfig5;
    volatile timg_wdtfeed_reg_t wdtfeed;
    volatile timg_wdtwprotect_reg_t wdtwprotect;
    volatile timg_rtccalicfg_reg_t rtccalicfg;
    volatile timg_rtccalicfg1_reg_t rtccalicfg1;
    volatile timg_int_ena_timers_reg_t int_ena_timers;
    volatile timg_int_raw_timers_reg_t int_raw_timers;
    volatile timg_int_st_timers_reg_t int_st_timers;
    volatile timg_int_clr_timers_reg_t int_clr_timers;
    volatile timg_rtccalicfg2_reg_t rtccalicfg2;
    uint32_t reserved_084[29];
    volatile timg_ntimers_date_reg_t ntimers_date;
    volatile timg_regclk_reg_t regclk;
} timg_dev_t;

extern timg_dev_t TIMERG0;
extern timg_dev_t TIMERG1;

#ifndef __cplusplus
_Static_assert(sizeof(timg_dev_t) == 0x100, "Invalid size of timg_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
