/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Register */
/** Type of un_conf0 register
 *  Configuration register 0 for unit n
 */
typedef union {
    struct {
        /** filter_thres : R/W; bitpos: [9:0]; default: 16;
         *  This sets the maximum threshold, in APB_CLK cycles, for the filter.
         *
         *  Any pulses with width less than this will be ignored when the filter is enabled.
         */
        uint32_t filter_thres:10;
        /** filter_en : R/W; bitpos: [10]; default: 1;
         *  This is the enable bit for unit n's input filter.
         */
        uint32_t filter_en:1;
        /** thr_zero_en : R/W; bitpos: [11]; default: 1;
         *  This is the enable bit for unit n's zero comparator.
         */
        uint32_t thr_zero_en:1;
        /** thr_h_lim_en : R/W; bitpos: [12]; default: 1;
         *  This is the enable bit for unit n's thr_h_lim comparator.
         */
        uint32_t thr_h_lim_en:1;
        /** thr_l_lim_en : R/W; bitpos: [13]; default: 1;
         *  This is the enable bit for unit n's thr_l_lim comparator.
         */
        uint32_t thr_l_lim_en:1;
        /** thr_thres0_en : R/W; bitpos: [14]; default: 0;
         *  This is the enable bit for unit n's thres0 comparator.
         */
        uint32_t thr_thres0_en:1;
        /** thr_thres1_en : R/W; bitpos: [15]; default: 0;
         *  This is the enable bit for unit n's thres1 comparator.
         */
        uint32_t thr_thres1_en:1;
        /** ch0_neg_mode : R/W; bitpos: [17:16]; default: 0;
         *  This register sets the behavior when the signal input of channel 0 detects a
         *  negative edge.
         *
         *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
         */
        uint32_t ch0_neg_mode:2;
        /** ch0_pos_mode : R/W; bitpos: [19:18]; default: 0;
         *  This register sets the behavior when the signal input of channel 0 detects a
         *  positive edge.
         *
         *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
         */
        uint32_t ch0_pos_mode:2;
        /** ch0_hctrl_mode : R/W; bitpos: [21:20]; default: 0;
         *  This register configures how the CHn_POS_MODE/CHn_NEG_MODE settings will be
         *  modified when the control signal is high.
         *
         *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
         *  increase).2, 3: Inhibit counter modification
         */
        uint32_t ch0_hctrl_mode:2;
        /** ch0_lctrl_mode : R/W; bitpos: [23:22]; default: 0;
         *  This register configures how the CHn_POS_MODE/CHn_NEG_MODE settings will be
         *  modified when the control signal is low.
         *
         *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
         *  increase).2, 3: Inhibit counter modification
         */
        uint32_t ch0_lctrl_mode:2;
        /** ch1_neg_mode : R/W; bitpos: [25:24]; default: 0;
         *  This register sets the behavior when the signal input of channel 1 detects a
         *  negative edge.
         *
         *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
         */
        uint32_t ch1_neg_mode:2;
        /** ch1_pos_mode : R/W; bitpos: [27:26]; default: 0;
         *  This register sets the behavior when the signal input of channel 1 detects a
         *  positive edge.
         *
         *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
         */
        uint32_t ch1_pos_mode:2;
        /** ch1_hctrl_mode : R/W; bitpos: [29:28]; default: 0;
         *  This register configures how the CHn_POS_MODE/CHn_NEG_MODE settings will be
         *  modified when the control signal is high.
         *
         *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
         *  increase).2, 3: Inhibit counter modification
         */
        uint32_t ch1_hctrl_mode:2;
        /** ch1_lctrl_mode : R/W; bitpos: [31:30]; default: 0;
         *  This register configures how the CHn_POS_MODE/CHn_NEG_MODE settings will be
         *  modified when the control signal is low.
         *
         *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
         *  increase).2, 3: Inhibit counter modification
         */
        uint32_t ch1_lctrl_mode:2;
    };
    uint32_t val;
} pcnt_un_conf0_reg_t;

/** Type of un_conf1 register
 *  Configuration register 1 for unit n
 */
typedef union {
    struct {
        /** cnt_thres0 : R/W; bitpos: [15:0]; default: 0;
         *  This register is used to configure the thres0 value for unit n.
         */
        uint32_t cnt_thres0:16;
        /** cnt_thres1 : R/W; bitpos: [31:16]; default: 0;
         *  This register is used to configure the thres1 value for unit n.
         */
        uint32_t cnt_thres1:16;
    };
    uint32_t val;
} pcnt_un_conf1_reg_t;

/** Type of un_conf2 register
 *  Configuration register 2 for unit n
 */
typedef union {
    struct {
        /** cnt_h_lim : R/W; bitpos: [15:0]; default: 0;
         *  This register is used to configure the thr_h_lim value for unit n.
         */
        uint32_t cnt_h_lim:16;
        /** cnt_l_lim : R/W; bitpos: [31:16]; default: 0;
         *  This register is used to configure the thr_l_lim value for unit n.
         */
        uint32_t cnt_l_lim:16;
    };
    uint32_t val;
} pcnt_un_conf2_reg_t;

/** Type of ctrl register
 *  Control register for all counters
 */
typedef union {
    struct {
        /** pulse_cnt_rst_u0 : R/W; bitpos: [0]; default: 1;
         *  Set this bit to clear unit 0's counter.
         */
        uint32_t pulse_cnt_rst_u0:1;
        /** cnt_pause_u0 : R/W; bitpos: [1]; default: 0;
         *  Set this bit to freeze unit 0's counter.
         */
        uint32_t cnt_pause_u0:1;
        /** pulse_cnt_rst_u1 : R/W; bitpos: [2]; default: 1;
         *  Set this bit to clear unit 1's counter.
         */
        uint32_t pulse_cnt_rst_u1:1;
        /** cnt_pause_u1 : R/W; bitpos: [3]; default: 0;
         *  Set this bit to freeze unit 1's counter.
         */
        uint32_t cnt_pause_u1:1;
        /** pulse_cnt_rst_u2 : R/W; bitpos: [4]; default: 1;
         *  Set this bit to clear unit 2's counter.
         */
        uint32_t pulse_cnt_rst_u2:1;
        /** cnt_pause_u2 : R/W; bitpos: [5]; default: 0;
         *  Set this bit to freeze unit 2's counter.
         */
        uint32_t cnt_pause_u2:1;
        /** pulse_cnt_rst_u3 : R/W; bitpos: [6]; default: 1;
         *  Set this bit to clear unit 3's counter.
         */
        uint32_t pulse_cnt_rst_u3:1;
        /** cnt_pause_u3 : R/W; bitpos: [7]; default: 0;
         *  Set this bit to freeze unit 3's counter.
         */
        uint32_t cnt_pause_u3:1;
        uint32_t reserved_8:8;
        /** clk_en : R/W; bitpos: [16]; default: 0;
         *  The registers clock gate enable signal of PCNT module. 1: the registers can be read
         *  and written by application. 0: the registers can not be read or written by
         *  application
         */
        uint32_t clk_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pcnt_ctrl_reg_t;


/** Group: Status Register */
/** Type of un_cnt register
 *  Counter value for unit n
 */
typedef union {
    struct {
        /** pulse_cnt : RO; bitpos: [15:0]; default: 0;
         *  This register stores the current pulse count value for unit n.
         */
        uint32_t pulse_cnt:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} pcnt_un_cnt_reg_t;

/** Type of un_status register
 *  PNCT UNITn status register
 */
typedef union {
    struct {
        /** cnt_thr_zero_mode : RO; bitpos: [1:0]; default: 0;
         *  The pulse counter status of PCNT_Un corresponding to 0. 0: pulse counter decreases
         *  from positive to 0. 1: pulse counter increases from negative to 0. 2: pulse counter
         *  is negative. 3: pulse counter is positive.
         */
        uint32_t cnt_thr_zero_mode:2;
        /** cnt_thr_thres1_lat : RO; bitpos: [2]; default: 0;
         *  The latched value of thres1 event of PCNT_Un when threshold event interrupt is
         *  valid. 1: the current pulse counter equals to thres1 and thres1 event is valid. 0:
         *  others
         */
        uint32_t cnt_thr_thres1_lat:1;
        /** cnt_thr_thres0_lat : RO; bitpos: [3]; default: 0;
         *  The latched value of thres0 event of PCNT_Un when threshold event interrupt is
         *  valid. 1: the current pulse counter equals to thres0 and thres0 event is valid. 0:
         *  others
         */
        uint32_t cnt_thr_thres0_lat:1;
        /** cnt_thr_l_lim_lat : RO; bitpos: [4]; default: 0;
         *  The latched value of low limit event of PCNT_Un when threshold event interrupt is
         *  valid. 1: the current pulse counter equals to thr_l_lim and low limit event is
         *  valid. 0: others
         */
        uint32_t cnt_thr_l_lim_lat:1;
        /** cnt_thr_h_lim_lat : RO; bitpos: [5]; default: 0;
         *  The latched value of high limit event of PCNT_Un when threshold event interrupt is
         *  valid. 1: the current pulse counter equals to thr_h_lim and high limit event is
         *  valid. 0: others
         */
        uint32_t cnt_thr_h_lim_lat:1;
        /** cnt_thr_zero_lat : RO; bitpos: [6]; default: 0;
         *  The latched value of zero threshold event of PCNT_Un when threshold event interrupt
         *  is valid. 1: the current pulse counter equals to 0 and zero threshold event is
         *  valid. 0: others
         */
        uint32_t cnt_thr_zero_lat:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} pcnt_un_status_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  Interrupt raw status register
 */
typedef union {
    struct {
        /** cnt_thr_event_u0_int_raw : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U0_INT interrupt.
         */
        uint32_t cnt_thr_event_u0_int_raw:1;
        /** cnt_thr_event_u1_int_raw : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U1_INT interrupt.
         */
        uint32_t cnt_thr_event_u1_int_raw:1;
        /** cnt_thr_event_u2_int_raw : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U2_INT interrupt.
         */
        uint32_t cnt_thr_event_u2_int_raw:1;
        /** cnt_thr_event_u3_int_raw : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U3_INT interrupt.
         */
        uint32_t cnt_thr_event_u3_int_raw:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} pcnt_int_raw_reg_t;

/** Type of int_st register
 *  Interrupt status register
 */
typedef union {
    struct {
        /** cnt_thr_event_u0_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U0_INT interrupt.
         */
        uint32_t cnt_thr_event_u0_int_st:1;
        /** cnt_thr_event_u1_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U1_INT interrupt.
         */
        uint32_t cnt_thr_event_u1_int_st:1;
        /** cnt_thr_event_u2_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U2_INT interrupt.
         */
        uint32_t cnt_thr_event_u2_int_st:1;
        /** cnt_thr_event_u3_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U3_INT interrupt.
         */
        uint32_t cnt_thr_event_u3_int_st:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} pcnt_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable register
 */
typedef union {
    struct {
        /** cnt_thr_event_u0_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U0_INT interrupt.
         */
        uint32_t cnt_thr_event_u0_int_ena:1;
        /** cnt_thr_event_u1_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U1_INT interrupt.
         */
        uint32_t cnt_thr_event_u1_int_ena:1;
        /** cnt_thr_event_u2_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U2_INT interrupt.
         */
        uint32_t cnt_thr_event_u2_int_ena:1;
        /** cnt_thr_event_u3_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U3_INT interrupt.
         */
        uint32_t cnt_thr_event_u3_int_ena:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} pcnt_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear register
 */
typedef union {
    struct {
        /** cnt_thr_event_u0_int_clr : WO; bitpos: [0]; default: 0;
         *  Set this bit to clear the PCNT_CNT_THR_EVENT_U0_INT interrupt.
         */
        uint32_t cnt_thr_event_u0_int_clr:1;
        /** cnt_thr_event_u1_int_clr : WO; bitpos: [1]; default: 0;
         *  Set this bit to clear the PCNT_CNT_THR_EVENT_U1_INT interrupt.
         */
        uint32_t cnt_thr_event_u1_int_clr:1;
        /** cnt_thr_event_u2_int_clr : WO; bitpos: [2]; default: 0;
         *  Set this bit to clear the PCNT_CNT_THR_EVENT_U2_INT interrupt.
         */
        uint32_t cnt_thr_event_u2_int_clr:1;
        /** cnt_thr_event_u3_int_clr : WO; bitpos: [3]; default: 0;
         *  Set this bit to clear the PCNT_CNT_THR_EVENT_U3_INT interrupt.
         */
        uint32_t cnt_thr_event_u3_int_clr:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} pcnt_int_clr_reg_t;


/** Group: Version Register */
/** Type of date register
 *  PCNT version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 419898881;
         *  This is the PCNT version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} pcnt_date_reg_t;

typedef struct pcnt_dev_t {
    volatile struct {
        pcnt_un_conf0_reg_t conf0;
        pcnt_un_conf1_reg_t conf1;
        pcnt_un_conf2_reg_t conf2;
    } conf_unit[4];
    volatile pcnt_un_cnt_reg_t cnt_unit[4];
    volatile pcnt_int_raw_reg_t int_raw;
    volatile pcnt_int_st_reg_t int_st;
    volatile pcnt_int_ena_reg_t int_ena;
    volatile pcnt_int_clr_reg_t int_clr;
    volatile pcnt_un_status_reg_t status_unit[4];
    volatile pcnt_ctrl_reg_t ctrl;
    uint32_t reserved_064[38];
    volatile pcnt_date_reg_t date;
} pcnt_dev_t;

extern pcnt_dev_t PCNT;

#ifndef __cplusplus
_Static_assert(sizeof(pcnt_dev_t) == 0x100, "Invalid size of pcnt_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
