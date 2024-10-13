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

/** Group: Tsens control registers. */
/** Type of ctrl register
 *  Tsens configuration.
 */
typedef union {
    struct {
        /** out : RO; bitpos: [7:0]; default: 0;
         *  Temperature sensor data out.
         */
        uint32_t out:8;
        /** ready : RO; bitpos: [8]; default: 0;
         *  Indicate temperature sensor out ready.
         */
        uint32_t ready:1;
        /** sample_en : R/W; bitpos: [9]; default: 0;
         *  Enable sample signal for wakeup module.
         */
        uint32_t sample_en:1;
        /** wakeup_mask : R/W; bitpos: [10]; default: 1;
         *  Wake up signal  mask.
         */
        uint32_t wakeup_mask:1;
        uint32_t reserved_11:1;
        /** int_en : R/W; bitpos: [12]; default: 1;
         *  Enable temperature sensor to send out interrupt.
         */
        uint32_t int_en:1;
        /** in_inv : R/W; bitpos: [13]; default: 0;
         *  Invert temperature sensor data.
         */
        uint32_t in_inv:1;
        /** clk_div : R/W; bitpos: [21:14]; default: 6;
         *  Temperature sensor clock divider.
         */
        uint32_t clk_div:8;
        /** power_up : R/W; bitpos: [22]; default: 0;
         *  Temperature sensor power up.
         */
        uint32_t power_up:1;
        /** power_up_force : R/W; bitpos: [23]; default: 0;
         *  1: dump out & power up controlled by SW,  0: by FSM.
         */
        uint32_t power_up_force:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} tsens_ctrl_reg_t;


/** Group: Tsens interrupt registers. */
/** Type of int_raw register
 *  Tsens interrupt raw registers.
 */
typedef union {
    struct {
        /** cocpu_tsens_wake_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  Tsens wakeup interrupt raw.
         */
        uint32_t cocpu_tsens_wake_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tsens_int_raw_reg_t;

/** Type of int_st register
 *  Tsens interrupt status registers.
 */
typedef union {
    struct {
        /** cocpu_tsens_wake_int_st : RO; bitpos: [0]; default: 0;
         *  Tsens wakeup interrupt status.
         */
        uint32_t cocpu_tsens_wake_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tsens_int_st_reg_t;

/** Type of int_ena register
 *  Tsens interrupt enable registers.
 */
typedef union {
    struct {
        /** cocpu_tsens_wake_int_ena : R/WTC; bitpos: [0]; default: 0;
         *  Tsens wakeup interrupt enable.
         */
        uint32_t cocpu_tsens_wake_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tsens_int_ena_reg_t;

/** Type of int_clr register
 *  Tsens interrupt clear registers.
 */
typedef union {
    struct {
        /** cocpu_tsens_wake_int_clr : WT; bitpos: [0]; default: 0;
         *  Tsens wakeup interrupt clear.
         */
        uint32_t cocpu_tsens_wake_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tsens_int_clr_reg_t;

/** Type of int_ena_w1ts register
 *  Tsens wakeup interrupt enable assert.
 */
typedef union {
    struct {
        /** cocpu_tsens_wake_int_ena_w1ts : WT; bitpos: [0]; default: 0;
         *  Write 1 to this field to assert interrupt enable.
         */
        uint32_t cocpu_tsens_wake_int_ena_w1ts:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tsens_int_ena_w1ts_reg_t;

/** Type of int_ena_w1tc register
 *  Tsens wakeup interrupt enable deassert.
 */
typedef union {
    struct {
        /** cocpu_tsens_wake_int_ena_w1tc : WT; bitpos: [0]; default: 0;
         *  Write 1 to this field to deassert interrupt enable.
         */
        uint32_t cocpu_tsens_wake_int_ena_w1tc:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tsens_int_ena_w1tc_reg_t;


/** Group: Tsens regbank clock control registers. */
/** Type of clk_conf register
 *  Tsens regbank configuration registers.
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Tsens regbank clock gating enable.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tsens_clk_conf_reg_t;


/** Group: Tsens wakeup control registers. */
/** Type of wakeup_ctrl register
 *  Tsens wakeup control registers.
 */
typedef union {
    struct {
        /** wakeup_th_low : R/W; bitpos: [7:0]; default: 0;
         *  Lower threshold.
         */
        uint32_t wakeup_th_low:8;
        uint32_t reserved_8:6;
        /** wakeup_th_high : R/W; bitpos: [21:14]; default: 255;
         *  Upper threshold.
         */
        uint32_t wakeup_th_high:8;
        uint32_t reserved_22:7;
        /** wakeup_over_upper_th : RO; bitpos: [29]; default: 0;
         *  Indicates that this wakeup event arose from exceeding upper threshold.
         */
        uint32_t wakeup_over_upper_th:1;
        /** wakeup_en : R/W; bitpos: [30]; default: 0;
         *  Tsens wakeup enable.
         */
        uint32_t wakeup_en:1;
        /** wakeup_mode : R/W; bitpos: [31]; default: 0;
         *  0:absolute value comparison mode. 1: relative value comparison mode.
         */
        uint32_t wakeup_mode:1;
    };
    uint32_t val;
} tsens_wakeup_ctrl_reg_t;

/** Type of sample_rate register
 *  Hardware automatic sampling control registers.
 */
typedef union {
    struct {
        /** sample_rate : R/W; bitpos: [15:0]; default: 20;
         *  Hardware automatic sampling rate.
         */
        uint32_t sample_rate:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} tsens_sample_rate_reg_t;


typedef struct {
    volatile tsens_ctrl_reg_t ctrl;
    uint32_t reserved_004;
    volatile tsens_int_raw_reg_t int_raw;
    volatile tsens_int_st_reg_t int_st;
    volatile tsens_int_ena_reg_t int_ena;
    volatile tsens_int_clr_reg_t int_clr;
    volatile tsens_clk_conf_reg_t clk_conf;
    volatile tsens_int_ena_w1ts_reg_t int_ena_w1ts;
    volatile tsens_int_ena_w1tc_reg_t int_ena_w1tc;
    volatile tsens_wakeup_ctrl_reg_t wakeup_ctrl;
    volatile tsens_sample_rate_reg_t sample_rate;
} tsens_dev_t;

extern tsens_dev_t LP_TSENS;

#ifndef __cplusplus
_Static_assert(sizeof(tsens_dev_t) == 0x2c, "Invalid size of tsens_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
