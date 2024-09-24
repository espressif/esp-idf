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

/** Group: Configuration and Control Register */
/** Type of txconfig register
 *  Timer x configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0: 10;
        /** tx_alarm_en : R/W; bitpos: [10]; default: 0;
         *  When set, the alarm is enabled. This bit is automatically cleared once an
         *
         *  alarm occurs.
         */
        uint32_t tx_alarm_en: 1;
        /** tx_level_int_en : R/W; bitpos: [11]; default: 0;
         *  When set, an alarm will generate a level type interrupt.
         */
        uint32_t tx_level_int_en: 1;
        /** tx_edge_int_en : R/W; bitpos: [12]; default: 0;
         *  When set, an alarm will generate an edge type interrupt.
         */
        uint32_t tx_edge_int_en: 1;
        /** tx_divider : R/W; bitpos: [28:13]; default: 1;
         *  Timer x clock (Tx_clk) prescaler value.
         */
        uint32_t tx_divider: 16;
        /** tx_autoreload : R/W; bitpos: [29]; default: 1;
         *  When set, timer x auto-reload at alarm is enabled.
         */
        uint32_t tx_autoreload: 1;
        /** tx_increase : R/W; bitpos: [30]; default: 1;
         *  When set, the timer x time-base counter will increment every clock tick. When
         *
         *  cleared, the timer x time-base counter will decrement.
         */
        uint32_t tx_increase: 1;
        /** tx_en : R/W; bitpos: [31]; default: 0;
         *  When set, the timer x time-base counter is enabled.
         */
        uint32_t tx_en: 1;
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
         *
         *  of timer x can be read here.
         */
        uint32_t tx_lo: 32;
    };
    uint32_t val;
} timg_txlo_reg_t;

/** Type of txhi register
 *  Timer x current value, high 32 bits
 */
typedef union {
    struct {
        /** tx_hi : RO; bitpos: [31:0]; default: 0;
         *  After writing to TIMG_TxUPDATE_REG, the high 32 bits of the time-base counter
         *
         *  of timer x can be read here.
         */
        uint32_t tx_hi: 32;
    };
    uint32_t val;
} timg_txhi_reg_t;

/** Type of txupdate register
 *  Write to copy current timer value to TIMGn_Tx_(LO/HI)_REG
 */
typedef union {
    struct {
        uint32_t reserved_0: 31;
        /** tx_update : R/W; bitpos: [31]; default: 0;
         *  After writing 0 or 1 to TIMG_TxUPDATE_REG, the counter value is latched.
         */
        uint32_t tx_update: 1;
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
        uint32_t tx_alarm_lo: 32;
    };
    uint32_t val;
} timg_txalarmlo_reg_t;

/** Type of txalarmhi register
 *  Timer x alarm value, high bits
 */
typedef union {
    struct {
        /** tx_alarm_hi : R/W; bitpos: [31:0]; default: 0;
         *
         *
         *  Timer x alarm trigger time-base counter value, high 32 bits.
         */
        uint32_t tx_alarm_hi: 32;
    };
    uint32_t val;
} timg_txalarmhi_reg_t;

/** Type of txloadlo register
 *  Timer x reload value, low 32 bits
 */
typedef union {
    struct {
        /** tx_load_lo : R/W; bitpos: [31:0]; default: 0;
         *
         *
         *  Low 32 bits of the value that a reload will load onto timer x time-base
         *
         *  Counter.
         */
        uint32_t tx_load_lo: 32;
    };
    uint32_t val;
} timg_txloadlo_reg_t;

/** Type of txloadhi register
 *  Timer x reload value, high 32 bits
 */
typedef union {
    struct {
        /** tx_load_hi : R/W; bitpos: [31:0]; default: 0;
         *
         *
         *  High 32 bits of the value that a reload will load onto timer x time-base
         *
         *  counter.
         */
        uint32_t tx_load_hi: 32;
    };
    uint32_t val;
} timg_txloadhi_reg_t;

/** Type of txload register
 *  Write to reload timer from TIMG_T0_(LOADLOLOADHI)_REG
 */
typedef union {
    struct {
        /** tx_load : WO; bitpos: [31:0]; default: 0;
         *
         *
         *  Write any value to trigger a timer x time-base counter reload.
         */
        uint32_t tx_load: 32;
    };
    uint32_t val;
} timg_txload_reg_t;


/** Group: Configuration and Control Register for WDT */
/** Type of wdtconfig0 register
 *  Watchdog timer configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0: 14;
        /** wdt_flashboot_mod_en : R/W; bitpos: [14]; default: 1;
         *  When set, Flash boot protection is enabled.
         */
        uint32_t wdt_flashboot_mod_en: 1;
        /** wdt_sys_reset_length : R/W; bitpos: [17:15]; default: 1;
         *  System reset signal length selection. 0: 100 ns, 1: 200 ns,
         *
         *  2: 300 ns, 3: 400 ns, 4: 500 ns, 5: 800 ns, 6: 1.6 us, 7: 3.2 us.
         */
        uint32_t wdt_sys_reset_length: 3;
        /** wdt_cpu_reset_length : R/W; bitpos: [20:18]; default: 1;
         *  CPU reset signal length selection. 0: 100 ns, 1: 200 ns,
         *
         *  2: 300 ns, 3: 400 ns, 4: 500 ns, 5: 800 ns, 6: 1.6 us, 7: 3.2 us.
         */
        uint32_t wdt_cpu_reset_length: 3;
        /** wdt_level_int_en : R/W; bitpos: [21]; default: 0;
         *  When set, a level type interrupt will occur at the timeout of a stage
         *
         *  configured to generate an interrupt.
         */
        uint32_t wdt_level_int_en: 1;
        /** wdt_edge_int_en : R/W; bitpos: [22]; default: 0;
         *  When set, an edge type interrupt will occur at the timeout of a stage
         *
         *  configured to generate an interrupt.
         */
        uint32_t wdt_edge_int_en: 1;
        /** wdt_stg3 : R/W; bitpos: [24:23]; default: 0;
         *  Stage 3 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         *
         */
        uint32_t wdt_stg3: 2;
        /** wdt_stg2 : R/W; bitpos: [26:25]; default: 0;
         *  Stage 2 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         *
         */
        uint32_t wdt_stg2: 2;
        /** wdt_stg1 : R/W; bitpos: [28:27]; default: 0;
         *  Stage 1 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         *
         */
        uint32_t wdt_stg1: 2;
        /** wdt_stg0 : R/W; bitpos: [30:29]; default: 0;
         *  Stage 0 configuration. 0: off, 1: interrupt, 2: reset CPU, 3: reset system.
         *
         */
        uint32_t wdt_stg0: 2;
        /** wdt_en : R/W; bitpos: [31]; default: 0;
         *  When set, MWDT is enabled.
         */
        uint32_t wdt_en: 1;
    };
    uint32_t val;
} timg_wdtconfig0_reg_t;

/** Type of wdtconfig1 register
 *  Watchdog timer prescaler register
 */
typedef union {
    struct {
        uint32_t reserved_0: 16;
        /** wdt_clk_prescaler : R/W; bitpos: [31:16]; default: 1;
         *  MWDT clock prescaler value. MWDT clock period = 12.5 ns *
         *
         *  TIMG_WDT_CLK_PRESCALE.
         */
        uint32_t wdt_clk_prescaler: 16;
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
        uint32_t wdt_stg0_hold: 32;
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
        uint32_t wdt_stg1_hold: 32;
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
        uint32_t wdt_stg2_hold: 32;
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
        uint32_t wdt_stg3_hold: 32;
    };
    uint32_t val;
} timg_wdtconfig5_reg_t;

/** Type of wdtfeed register
 *  Write to feed the watchdog timer
 */
typedef union {
    struct {
        /** wdt_feed : WO; bitpos: [31:0]; default: 0;
         *  Write any value to feed the MWDT. (WO)
         */
        uint32_t wdt_feed: 32;
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
         *
         *  protection is enabled.
         */
        uint32_t wdt_wkey: 32;
    };
    uint32_t val;
} timg_wdtwprotect_reg_t;


/** Group: Configuration and Control Register for RTC CALI */
/** Type of rtccalicfg register
 *  RTC calibration configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0: 12;
        /** rtc_cali_start_cycling : R/W; bitpos: [12]; default: 1;
         *  Reserved
         */
        uint32_t rtc_cali_start_cycling: 1;
        /** rtc_cali_clk_sel : R/W; bitpos: [14:13]; default: 1;
         *  0:rtcslowclock. 1:clk_80m.  2:xtal_32k.
         */
        uint32_t rtc_cali_clk_sel: 2;
        /** rtc_cali_rdy : RO; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t rtc_cali_rdy: 1;
        /** rtc_cali_max : R/W; bitpos: [30:16]; default: 1;
         *  Reserved
         */
        uint32_t rtc_cali_max: 15;
        /** rtc_cali_start : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t rtc_cali_start: 1;
    };
    uint32_t val;
} timg_rtccalicfg_reg_t;

/** Type of rtccalicfg1 register
 *  RTC calibration configuration1 register
 */
typedef union {
    struct {
        uint32_t reserved_0: 7;
        /** rtc_cali_value : RO; bitpos: [31:7]; default: 0;
         *  Reserved
         */
        uint32_t rtc_cali_value: 25;
    };
    uint32_t val;
} timg_rtccalicfg1_reg_t;

/** Group: Configuration and Control Register for LACT */
/** Type of lactconfig register
 *  LACT configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0: 7;
        /** lact_rtc_only : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t lact_rtc_only: 1;
        /** lact_cpst_en : R/W; bitpos: [8]; default: 1;
         *  Reserved
         */
        uint32_t lact_cpst_en: 1;
        /** lact_lac_en : R/W; bitpos: [9]; default: 1;
         *  Reserved
         */
        uint32_t lact_lac_en: 1;
        /** lact_alarm_en : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t lact_alarm_en: 1;
        /** lact_level_int_en : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t lact_level_int_en: 1;
        /** lact_edge_int_en : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t lact_edge_int_en: 1;
        /** lact_divider : R/W; bitpos: [28:13]; default: 1;
         *  Reserved
         */
        uint32_t lact_divider: 16;
        /** lact_autoreload : R/W; bitpos: [29]; default: 1;
         *  Reserved
         */
        uint32_t lact_autoreload: 1;
        /** lact_increase : R/W; bitpos: [30]; default: 1;
         *  Reserved
         */
        uint32_t lact_increase: 1;
        /** lact_en : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t lact_en: 1;
    };
    uint32_t val;
} timg_lactconfig_reg_t;

/** Type of lactrtc register
 *  LACT RTC register
 */
typedef union {
    struct {
        uint32_t reserved_0: 6;
        /** lact_rtc_step_len : R/W; bitpos: [31:6]; default: 0;
         *  Reserved
         */
        uint32_t lact_rtc_step_len: 26;
    };
    uint32_t val;
} timg_lactrtc_reg_t;

/** Type of lactlo register
 *  LACT low register
 */
typedef union {
    struct {
        /** lact_lo : RO; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_lo: 32;
    };
    uint32_t val;
} timg_lactlo_reg_t;

/** Type of lacthi register
 *  LACT high register
 */
typedef union {
    struct {
        /** lact_hi : RO; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_hi: 32;
    };
    uint32_t val;
} timg_lacthi_reg_t;

/** Type of lactupdate register
 *  LACT update register
 */
typedef union {
    struct {
        /** lact_update : WO; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_update: 32;
    };
    uint32_t val;
} timg_lactupdate_reg_t;

/** Type of lactalarmlo register
 *  LACT alarm low register
 */
typedef union {
    struct {
        /** lact_alarm_lo : R/W; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_alarm_lo: 32;
    };
    uint32_t val;
} timg_lactalarmlo_reg_t;

/** Type of lactalarmhi register
 *  LACT alarm high register
 */
typedef union {
    struct {
        /** lact_alarm_hi : R/W; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_alarm_hi: 32;
    };
    uint32_t val;
} timg_lactalarmhi_reg_t;

/** Type of lactloadlo register
 *  LACT load low register
 */
typedef union {
    struct {
        /** lact_load_lo : R/W; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_load_lo: 32;
    };
    uint32_t val;
} timg_lactloadlo_reg_t;

/** Type of lactloadhi register
 *  Timer LACT load high register
 */
typedef union {
    struct {
        /** lact_load_hi : R/W; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_load_hi: 32;
    };
    uint32_t val;
} timg_lactloadhi_reg_t;

/** Type of lactload register
 *  Timer LACT load register
 */
typedef union {
    struct {
        /** lact_load : WO; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lact_load: 32;
    };
    uint32_t val;
} timg_lactload_reg_t;


/** Group: Interrupt Register */
/** Type of int_ena_timers register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** tx_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the TIMG_T0_INT interrupt.
         */
        uint32_t t0_int_ena: 1;
        /** t1_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the TIMG_T1_INT interrupt.
         */
        uint32_t t1_int_ena: 1;
        /** wdt_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_ena: 1;
        /** lact_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the TIMG_LACT_INT interrupt.
         */
        uint32_t lact_int_ena: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} timg_int_ena_timers_reg_t;

/** Type of int_raw_timers register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** t0_int_raw : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the TIMG_T0_INT interrupt.
         */
        uint32_t t0_int_raw: 1;
        /** t1_int_raw : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the TIMG_T1_INT interrupt.
         */
        uint32_t t1_int_raw: 1;
        /** wdt_int_raw : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_raw: 1;
        /** lact_int_raw : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the TIMG_LACT_INT interrupt.
         */
        uint32_t lact_int_raw: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} timg_int_raw_timers_reg_t;

/** Type of int_st_timers register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** t0_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit for the TIMG_T0_INT interrupt.
         */
        uint32_t t0_int_st: 1;
        /** t1_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit for the TIMG_T1_INT interrupt.
         */
        uint32_t t1_int_st: 1;
        /** wdt_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit for the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_st: 1;
        /** lact_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status bit for the TIMG_LACT_INT interrupt.
         */
        uint32_t lact_int_st: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} timg_int_st_timers_reg_t;

/** Type of int_clr_timers register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** t0_int_clr : WO; bitpos: [0]; default: 0;
         *  Set this bit to clear the TIMG_T0_INT interrupt.
         */
        uint32_t t0_int_clr: 1;
        /** t1_int_clr : WO; bitpos: [1]; default: 0;
         *  Set this bit to clear the TIMG_T1_INT interrupt.
         */
        uint32_t t1_int_clr: 1;
        /** wdt_int_clr : WO; bitpos: [2]; default: 0;
         *  Set this bit to clear the TIMG_WDT_INT interrupt.
         */
        uint32_t wdt_int_clr: 1;
        /** lact_int_clr : WO; bitpos: [3]; default: 0;
         *  Set this bit to clear the TIMG_LACT_INT interrupt.
         */
        uint32_t lact_int_clr: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} timg_int_clr_timers_reg_t;


/** Group: Version Register */
/** Type of timers_date register
 *  Version control register
 */
typedef union {
    struct {
        /** timers_date : R/W; bitpos: [27:0]; default: 26243681;
         *  Version control register.
         */
        uint32_t timers_date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} timg_timers_date_reg_t;


/** Group: Configuration Register */
/** Type of regclk register
 *  Timer group clock gate register
 */
typedef union {
    struct {
        uint32_t reserved_0: 31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Register clock gate signal. 1: Registers can be read and written to by software. 0:
         *  Registers can not be read or written to by software.
         */
        uint32_t clk_en: 1;
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
    volatile timg_hwtimer_reg_t hw_timer[2];
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
    volatile timg_lactconfig_reg_t lactconfig;
    volatile timg_lactrtc_reg_t lactrtc;
    volatile timg_lactlo_reg_t lactlo;
    volatile timg_lacthi_reg_t lacthi;
    volatile timg_lactupdate_reg_t lactupdate;
    volatile timg_lactalarmlo_reg_t lactalarmlo;
    volatile timg_lactalarmhi_reg_t lactalarmhi;
    volatile timg_lactloadlo_reg_t lactloadlo;
    volatile timg_lactloadhi_reg_t lactloadhi;
    volatile timg_lactload_reg_t lactload;
    volatile timg_int_ena_timers_reg_t int_ena_timers;
    volatile timg_int_raw_timers_reg_t int_raw_timers;
    volatile timg_int_st_timers_reg_t int_st_timers;
    volatile timg_int_clr_timers_reg_t int_clr_timers;
    uint32_t reserved_0ac[20];
    volatile timg_timers_date_reg_t timers_date;
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
