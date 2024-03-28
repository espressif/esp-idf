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

/** Group: ADC1 control registers. */
/** Type of reader1_ctrl register
 *  Control the read operation of ADC1.
 */
typedef union {
    struct {
        /** sar1_clk_div : R/W; bitpos: [7:0]; default: 2;
         *  Clock divider.
         */
        uint32_t sar1_clk_div:8;
        uint32_t reserved_8:20;
        /** sar1_data_inv : R/W; bitpos: [28]; default: 0;
         *  Invert SAR ADC1 data.
         */
        uint32_t sar1_data_inv:1;
        /** sar1_int_en : R/W; bitpos: [29]; default: 1;
         *  Enable saradc1 to send out interrupt.
         */
        uint32_t sar1_int_en:1;
        /** sar1_en_pad_force_enable : R/W; bitpos: [31:30]; default: 0;
         *  Force enable adc en_pad to analog circuit 2'b11: force enable .
         */
        uint32_t sar1_en_pad_force_enable:2;
    };
    uint32_t val;
} rtcadc_reader1_ctrl_reg_t;

/** Type of meas1_ctrl2 register
 *  ADC1 configuration registers.
 */
typedef union {
    struct {
        /** meas1_data_sar : RO; bitpos: [15:0]; default: 0;
         *  SAR ADC1 data.
         */
        uint32_t meas1_data_sar:16;
        /** meas1_done_sar : RO; bitpos: [16]; default: 0;
         *  SAR ADC1 conversion done indication.
         */
        uint32_t meas1_done_sar:1;
        /** meas1_start_sar : R/W; bitpos: [17]; default: 0;
         *  SAR ADC1 controller (in RTC) starts conversion.
         */
        uint32_t meas1_start_sar:1;
        /** meas1_start_force : R/W; bitpos: [18]; default: 0;
         *  1: SAR ADC1 controller (in RTC) is started by SW.
         */
        uint32_t meas1_start_force:1;
        /** sar1_en_pad : R/W; bitpos: [30:19]; default: 0;
         *  SAR ADC1 pad enable bitmap.
         */
        uint32_t sar1_en_pad:12;
        /** sar1_en_pad_force : R/W; bitpos: [31]; default: 0;
         *  1: SAR ADC1 pad enable bitmap is controlled by SW.
         */
        uint32_t sar1_en_pad_force:1;
    };
    uint32_t val;
} rtcadc_meas1_ctrl2_reg_t;

/** Type of meas1_mux register
 *  SAR ADC1 MUX register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** sar1_dig_force : R/W; bitpos: [31]; default: 0;
         *  1: SAR ADC1 controlled by DIG ADC1 CTRL.
         */
        uint32_t sar1_dig_force:1;
    };
    uint32_t val;
} rtcadc_meas1_mux_reg_t;

/** Type of atten1 register
 *  ADC1 attenuation registers.
 */
typedef union {
    struct {
        /** sar1_atten : R/W; bitpos: [31:0]; default: 4294967295;
         *  2-bit attenuation for each pad.
         */
        uint32_t sar1_atten:32;
    };
    uint32_t val;
} rtcadc_atten1_reg_t;


/** Group: ADC2 control registers. */
/** Type of reader2_ctrl register
 *  Control the read operation of ADC2.
 */
typedef union {
    struct {
        /** sar2_clk_div : R/W; bitpos: [7:0]; default: 2;
         *  Clock divider.
         */
        uint32_t sar2_clk_div:8;
        uint32_t reserved_8:8;
        /** sar2_wait_arb_cycle : R/W; bitpos: [17:16]; default: 1;
         *  Wait arbit stable after sar_done.
         */
        uint32_t sar2_wait_arb_cycle:2;
        uint32_t reserved_18:9;
        /** sar2_en_pad_force_enable : R/W; bitpos: [28:27]; default: 0;
         *  Force enable adc en_pad to analog circuit 2'b11: force enable .
         */
        uint32_t sar2_en_pad_force_enable:2;
        /** sar2_data_inv : R/W; bitpos: [29]; default: 0;
         *  Invert SAR ADC2 data.
         */
        uint32_t sar2_data_inv:1;
        /** sar2_int_en : R/W; bitpos: [30]; default: 1;
         *  Enable saradc2 to send out interrupt.
         */
        uint32_t sar2_int_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} rtcadc_reader2_ctrl_reg_t;

/** Type of meas2_ctrl1 register
 *  ADC2 configuration registers.
 */
typedef union {
    struct {
        /** sar2_cntl_state : RO; bitpos: [2:0]; default: 0;
         *  saradc2_cntl_fsm.
         */
        uint32_t sar2_cntl_state:3;
        /** sar2_pwdet_cal_en : R/W; bitpos: [3]; default: 0;
         *  RTC control pwdet enable.
         */
        uint32_t sar2_pwdet_cal_en:1;
        /** sar2_pkdet_cal_en : R/W; bitpos: [4]; default: 0;
         *  RTC control pkdet enable.
         */
        uint32_t sar2_pkdet_cal_en:1;
        /** sar2_en_test : R/W; bitpos: [5]; default: 0;
         *  SAR2_EN_TEST.
         */
        uint32_t sar2_en_test:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtcadc_meas2_ctrl1_reg_t;

/** Type of meas2_ctrl2 register
 *  ADC2 configuration registers.
 */
typedef union {
    struct {
        /** meas2_data_sar : RO; bitpos: [15:0]; default: 0;
         *  SAR ADC2 data.
         */
        uint32_t meas2_data_sar:16;
        /** meas2_done_sar : RO; bitpos: [16]; default: 0;
         *  SAR ADC2 conversion done indication.
         */
        uint32_t meas2_done_sar:1;
        /** meas2_start_sar : R/W; bitpos: [17]; default: 0;
         *  SAR ADC2 controller (in RTC) starts conversion.
         */
        uint32_t meas2_start_sar:1;
        /** meas2_start_force : R/W; bitpos: [18]; default: 0;
         *  1: SAR ADC2 controller (in RTC) is started by SW.
         */
        uint32_t meas2_start_force:1;
        /** sar2_en_pad : R/W; bitpos: [30:19]; default: 0;
         *  SAR ADC2 pad enable bitmap.
         */
        uint32_t sar2_en_pad:12;
        /** sar2_en_pad_force : R/W; bitpos: [31]; default: 0;
         *  1: SAR ADC2 pad enable bitmap is controlled by SW.
         */
        uint32_t sar2_en_pad_force:1;
    };
    uint32_t val;
} rtcadc_meas2_ctrl2_reg_t;

/** Type of meas2_mux register
 *  SAR ADC2 MUX register.
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** sar2_pwdet_cct : R/W; bitpos: [30:28]; default: 0;
         *  SAR2_PWDET_CCT.
         */
        uint32_t sar2_pwdet_cct:3;
        /** sar2_rtc_force : R/W; bitpos: [31]; default: 0;
         *  In sleep, force to use rtc to control ADC.
         */
        uint32_t sar2_rtc_force:1;
    };
    uint32_t val;
} rtcadc_meas2_mux_reg_t;

/** Type of atten2 register
 *  ADC1 attenuation registers.
 */
typedef union {
    struct {
        /** sar2_atten : R/W; bitpos: [31:0]; default: 4294967295;
         *  2-bit attenuation for each pad.
         */
        uint32_t sar2_atten:32;
    };
    uint32_t val;
} rtcadc_atten2_reg_t;


/** Group: ADC XPD control. */
/** Type of force_wpd_sar register
 *  In sleep, force to use rtc to control ADC
 */
typedef union {
    struct {
        /** force_xpd_sar1 : R/W; bitpos: [1:0]; default: 0;
         *  2'b11:software control, force on. 2'b10:software control, force off. 2'b0x:hardware
         *  control.
         */
        uint32_t force_xpd_sar1:2;
        /** force_xpd_sar2 : R/W; bitpos: [3:2]; default: 0;
         *  2'b11:software control, force on. 2'b10:software control, force off. 2'b0x:hardware
         *  control.
         */
        uint32_t force_xpd_sar2:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} rtcadc_force_wpd_sar_reg_t;


/** Group: RTCADC interrupt registers. */
/** Type of cocpu_int_raw register
 *  Interrupt raw registers.
 */
typedef union {
    struct {
        /** cocpu_saradc1_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  ADC1 Conversion is done, int raw.
         */
        uint32_t cocpu_saradc1_int_raw:1;
        /** cocpu_saradc2_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  ADC2 Conversion is done, int raw.
         */
        uint32_t cocpu_saradc2_int_raw:1;
        /** cocpu_saradc1_error_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  An error occurs from ADC1, int raw.
         */
        uint32_t cocpu_saradc1_error_int_raw:1;
        /** cocpu_saradc2_error_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  An error occurs from ADC2, int raw.
         */
        uint32_t cocpu_saradc2_error_int_raw:1;
        /** cocpu_saradc1_wake_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  A wakeup event is triggered from ADC1, int raw.
         */
        uint32_t cocpu_saradc1_wake_int_raw:1;
        /** cocpu_saradc2_wake_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  A wakeup event is triggered from ADC2, int raw.
         */
        uint32_t cocpu_saradc2_wake_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtcadc_cocpu_int_raw_reg_t;

/** Type of int_ena register
 *  Interrupt enable registers.
 */
typedef union {
    struct {
        /** cocpu_saradc1_int_ena : R/WTC; bitpos: [0]; default: 0;
         *  ADC1 Conversion is done, int enable.
         */
        uint32_t cocpu_saradc1_int_ena:1;
        /** cocpu_saradc2_int_ena : R/WTC; bitpos: [1]; default: 0;
         *  ADC2 Conversion is done, int enable.
         */
        uint32_t cocpu_saradc2_int_ena:1;
        /** cocpu_saradc1_error_int_ena : R/WTC; bitpos: [2]; default: 0;
         *  An error occurs from ADC1, int enable.
         */
        uint32_t cocpu_saradc1_error_int_ena:1;
        /** cocpu_saradc2_error_int_ena : R/WTC; bitpos: [3]; default: 0;
         *  An error occurs from ADC2, int enable.
         */
        uint32_t cocpu_saradc2_error_int_ena:1;
        /** cocpu_saradc1_wake_int_ena : R/WTC; bitpos: [4]; default: 0;
         *  A wakeup event is triggered from ADC1, int enable.
         */
        uint32_t cocpu_saradc1_wake_int_ena:1;
        /** cocpu_saradc2_wake_int_ena : R/WTC; bitpos: [5]; default: 0;
         *  A wakeup event is triggered from ADC2, int enable.
         */
        uint32_t cocpu_saradc2_wake_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtcadc_int_ena_reg_t;

/** Type of int_st register
 *  Interrupt status registers.
 */
typedef union {
    struct {
        /** cocpu_saradc1_int_st : RO; bitpos: [0]; default: 0;
         *  ADC1 Conversion is done, int status.
         */
        uint32_t cocpu_saradc1_int_st:1;
        /** cocpu_saradc2_int_st : RO; bitpos: [1]; default: 0;
         *  ADC2 Conversion is done, int status.
         */
        uint32_t cocpu_saradc2_int_st:1;
        /** cocpu_saradc1_error_int_st : RO; bitpos: [2]; default: 0;
         *  An error occurs from ADC1, int status.
         */
        uint32_t cocpu_saradc1_error_int_st:1;
        /** cocpu_saradc2_error_int_st : RO; bitpos: [3]; default: 0;
         *  An error occurs from ADC2, int status.
         */
        uint32_t cocpu_saradc2_error_int_st:1;
        /** cocpu_saradc1_wake_int_st : RO; bitpos: [4]; default: 0;
         *  A wakeup event is triggered from ADC1, int status.
         */
        uint32_t cocpu_saradc1_wake_int_st:1;
        /** cocpu_saradc2_wake_int_st : RO; bitpos: [5]; default: 0;
         *  A wakeup event is triggered from ADC2, int status.
         */
        uint32_t cocpu_saradc2_wake_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtcadc_int_st_reg_t;

/** Type of int_clr register
 *  Interrupt clear registers.
 */
typedef union {
    struct {
        /** cocpu_saradc1_int_clr : WT; bitpos: [0]; default: 0;
         *  ADC1 Conversion is done, int clear.
         */
        uint32_t cocpu_saradc1_int_clr:1;
        /** cocpu_saradc2_int_clr : WT; bitpos: [1]; default: 0;
         *  ADC2 Conversion is done, int clear.
         */
        uint32_t cocpu_saradc2_int_clr:1;
        /** cocpu_saradc1_error_int_clr : WT; bitpos: [2]; default: 0;
         *  An error occurs from ADC1, int clear.
         */
        uint32_t cocpu_saradc1_error_int_clr:1;
        /** cocpu_saradc2_error_int_clr : WT; bitpos: [3]; default: 0;
         *  An error occurs from ADC2, int clear.
         */
        uint32_t cocpu_saradc2_error_int_clr:1;
        /** cocpu_saradc1_wake_int_clr : WT; bitpos: [4]; default: 0;
         *  A wakeup event is triggered from ADC1, int clear.
         */
        uint32_t cocpu_saradc1_wake_int_clr:1;
        /** cocpu_saradc2_wake_int_clr : WT; bitpos: [5]; default: 0;
         *  A wakeup event is triggered from ADC2, int clear.
         */
        uint32_t cocpu_saradc2_wake_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtcadc_int_clr_reg_t;

/** Type of int_ena_w1ts register
 *  Interrupt enable assert registers.
 */
typedef union {
    struct {
        /** cocpu_saradc1_int_ena_w1ts : WT; bitpos: [0]; default: 0;
         *  ADC1 Conversion is done, write 1 to assert int enable.
         */
        uint32_t cocpu_saradc1_int_ena_w1ts:1;
        /** cocpu_saradc2_int_ena_w1ts : WT; bitpos: [1]; default: 0;
         *  ADC2 Conversion is done, write 1 to assert int enable.
         */
        uint32_t cocpu_saradc2_int_ena_w1ts:1;
        /** cocpu_saradc1_error_int_ena_w1ts : WT; bitpos: [2]; default: 0;
         *  An error occurs from ADC1, write 1 to assert int enable.
         */
        uint32_t cocpu_saradc1_error_int_ena_w1ts:1;
        /** cocpu_saradc2_error_int_ena_w1ts : WT; bitpos: [3]; default: 0;
         *  An error occurs from ADC2, write 1 to assert int enable.
         */
        uint32_t cocpu_saradc2_error_int_ena_w1ts:1;
        /** cocpu_saradc1_wake_int_ena_w1ts : WT; bitpos: [4]; default: 0;
         *  A wakeup event is triggered from ADC1, write 1 to assert int enable.
         */
        uint32_t cocpu_saradc1_wake_int_ena_w1ts:1;
        /** cocpu_saradc2_wake_int_ena_w1ts : WT; bitpos: [5]; default: 0;
         *  A wakeup event is triggered from ADC2, write 1 to assert int enable.
         */
        uint32_t cocpu_saradc2_wake_int_ena_w1ts:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtcadc_int_ena_w1ts_reg_t;

/** Type of int_ena_w1tc register
 *  Interrupt enable deassert registers.
 */
typedef union {
    struct {
        /** cocpu_saradc1_int_ena_w1tc : WT; bitpos: [0]; default: 0;
         *  ADC1 Conversion is done, write 1 to deassert int enable.
         */
        uint32_t cocpu_saradc1_int_ena_w1tc:1;
        /** cocpu_saradc2_int_ena_w1tc : WT; bitpos: [1]; default: 0;
         *  ADC2 Conversion is done, write 1 to deassert int enable.
         */
        uint32_t cocpu_saradc2_int_ena_w1tc:1;
        /** cocpu_saradc1_error_int_ena_w1tc : WT; bitpos: [2]; default: 0;
         *  An error occurs from ADC1, write 1 to deassert int enable.
         */
        uint32_t cocpu_saradc1_error_int_ena_w1tc:1;
        /** cocpu_saradc2_error_int_ena_w1tc : WT; bitpos: [3]; default: 0;
         *  An error occurs from ADC2, write 1 to deassert int enable.
         */
        uint32_t cocpu_saradc2_error_int_ena_w1tc:1;
        /** cocpu_saradc1_wake_int_ena_w1tc : WT; bitpos: [4]; default: 0;
         *  A wakeup event is triggered from ADC1, write 1 to deassert int enable.
         */
        uint32_t cocpu_saradc1_wake_int_ena_w1tc:1;
        /** cocpu_saradc2_wake_int_ena_w1tc : WT; bitpos: [5]; default: 0;
         *  A wakeup event is triggered from ADC2, write 1 to deassert int enable.
         */
        uint32_t cocpu_saradc2_wake_int_ena_w1tc:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtcadc_int_ena_w1tc_reg_t;


/** Group: RTCADC wakeup control registers. */
/** Type of wakeup1 register
 *  ADC1 wakeup configuration registers.
 */
typedef union {
    struct {
        /** sar1_wakeup_th_low : R/W; bitpos: [11:0]; default: 0;
         *  Lower threshold.
         */
        uint32_t sar1_wakeup_th_low:12;
        uint32_t reserved_12:2;
        /** sar1_wakeup_th_high : R/W; bitpos: [25:14]; default: 4095;
         *  Upper threshold.
         */
        uint32_t sar1_wakeup_th_high:12;
        uint32_t reserved_26:3;
        /** sar1_wakeup_over_upper_th : RO; bitpos: [29]; default: 0;
         *  Indicates that this wakeup event arose from exceeding upper threshold.
         */
        uint32_t sar1_wakeup_over_upper_th:1;
        /** sar1_wakeup_en : R/W; bitpos: [30]; default: 0;
         *  Wakeup function enable.
         */
        uint32_t sar1_wakeup_en:1;
        /** sar1_wakeup_mode : R/W; bitpos: [31]; default: 0;
         *  0:absolute value comparison mode. 1: relative value comparison mode.
         */
        uint32_t sar1_wakeup_mode:1;
    };
    uint32_t val;
} rtcadc_wakeup1_reg_t;

/** Type of wakeup2 register
 *  ADC2 wakeup configuration registers.
 */
typedef union {
    struct {
        /** sar2_wakeup_th_low : R/W; bitpos: [11:0]; default: 0;
         *  Lower threshold.
         */
        uint32_t sar2_wakeup_th_low:12;
        uint32_t reserved_12:2;
        /** sar2_wakeup_th_high : R/W; bitpos: [25:14]; default: 4095;
         *  Upper threshold.
         */
        uint32_t sar2_wakeup_th_high:12;
        uint32_t reserved_26:3;
        /** sar2_wakeup_over_upper_th : RO; bitpos: [29]; default: 0;
         *  Indicates that this wakeup event arose from exceeding upper threshold.
         */
        uint32_t sar2_wakeup_over_upper_th:1;
        /** sar2_wakeup_en : R/W; bitpos: [30]; default: 0;
         *  Wakeup function enable.
         */
        uint32_t sar2_wakeup_en:1;
        /** sar2_wakeup_mode : R/W; bitpos: [31]; default: 0;
         *  0:absolute value comparison mode. 1: relative value comparison mode.
         */
        uint32_t sar2_wakeup_mode:1;
    };
    uint32_t val;
} rtcadc_wakeup2_reg_t;

/** Type of wakeup_sel register
 *  Wakeup source select register.
 */
typedef union {
    struct {
        /** sar_wakeup_sel : R/W; bitpos: [0]; default: 0;
         *  0: ADC1. 1: ADC2.
         */
        uint32_t sar_wakeup_sel:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rtcadc_wakeup_sel_reg_t;

/** Type of sar1_hw_wakeup register
 *  Hardware automatic sampling registers for wakeup function.
 */
typedef union {
    struct {
        /** adc1_hw_read_en_i : R/W; bitpos: [0]; default: 0;
         *  Enable hardware automatic sampling.
         */
        uint32_t adc1_hw_read_en_i:1;
        /** adc1_hw_read_rate_i : R/W; bitpos: [16:1]; default: 100;
         *  Hardware automatic sampling rate.
         */
        uint32_t adc1_hw_read_rate_i:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} rtcadc_sar1_hw_wakeup_reg_t;

/** Type of sar2_hw_wakeup register
 *  Hardware automatic sampling registers for wakeup function.
 */
typedef union {
    struct {
        /** adc2_hw_read_en_i : R/W; bitpos: [0]; default: 0;
         *  Enable hardware automatic sampling.
         */
        uint32_t adc2_hw_read_en_i:1;
        /** adc2_hw_read_rate_i : R/W; bitpos: [16:1]; default: 100;
         *  Hardware automatic sampling rate.
         */
        uint32_t adc2_hw_read_rate_i:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} rtcadc_sar2_hw_wakeup_reg_t;


typedef struct {
    volatile rtcadc_reader1_ctrl_reg_t reader1_ctrl;
    uint32_t reserved_004[2];
    volatile rtcadc_meas1_ctrl2_reg_t meas1_ctrl2;
    volatile rtcadc_meas1_mux_reg_t meas1_mux;
    volatile rtcadc_atten1_reg_t atten1;
    uint32_t reserved_018[3];
    volatile rtcadc_reader2_ctrl_reg_t reader2_ctrl;
    uint32_t reserved_028;
    volatile rtcadc_meas2_ctrl1_reg_t meas2_ctrl1;
    volatile rtcadc_meas2_ctrl2_reg_t meas2_ctrl2;
    volatile rtcadc_meas2_mux_reg_t meas2_mux;
    volatile rtcadc_atten2_reg_t atten2;
    volatile rtcadc_force_wpd_sar_reg_t force_wpd_sar;
    uint32_t reserved_040[2];
    volatile rtcadc_cocpu_int_raw_reg_t cocpu_int_raw;
    volatile rtcadc_int_ena_reg_t int_ena;
    volatile rtcadc_int_st_reg_t int_st;
    volatile rtcadc_int_clr_reg_t int_clr;
    volatile rtcadc_int_ena_w1ts_reg_t int_ena_w1ts;
    volatile rtcadc_int_ena_w1tc_reg_t int_ena_w1tc;
    volatile rtcadc_wakeup1_reg_t wakeup1;
    volatile rtcadc_wakeup2_reg_t wakeup2;
    volatile rtcadc_wakeup_sel_reg_t wakeup_sel;
    volatile rtcadc_sar1_hw_wakeup_reg_t sar1_hw_wakeup;
    volatile rtcadc_sar2_hw_wakeup_reg_t sar2_hw_wakeup;
} rtcadc_dev_t;

extern rtcadc_dev_t LP_ADC;

#ifndef __cplusplus
_Static_assert(sizeof(rtcadc_dev_t) == 0x74, "Invalid size of rtcadc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
