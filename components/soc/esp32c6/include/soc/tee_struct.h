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

/** Group: Tee mode control register */
/** Type of m0_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m0_mode : R/W; bitpos: [1:0]; default: 0;
         *  M0 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m0_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m0_mode_ctrl_reg_t;

/** Type of m1_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m1_mode : R/W; bitpos: [1:0]; default: 3;
         *  M1 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m1_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m1_mode_ctrl_reg_t;

/** Type of m2_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m2_mode : R/W; bitpos: [1:0]; default: 0;
         *  M2 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m2_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m2_mode_ctrl_reg_t;

/** Type of m3_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m3_mode : R/W; bitpos: [1:0]; default: 3;
         *  M3 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m3_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m3_mode_ctrl_reg_t;

/** Type of m4_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m4_mode : R/W; bitpos: [1:0]; default: 3;
         *  M4 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m4_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m4_mode_ctrl_reg_t;

/** Type of m5_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m5_mode : R/W; bitpos: [1:0]; default: 3;
         *  M5 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m5_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m5_mode_ctrl_reg_t;

/** Type of m6_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m6_mode : R/W; bitpos: [1:0]; default: 3;
         *  M6 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m6_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m6_mode_ctrl_reg_t;

/** Type of m7_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m7_mode : R/W; bitpos: [1:0]; default: 3;
         *  M7 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m7_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m7_mode_ctrl_reg_t;

/** Type of m8_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m8_mode : R/W; bitpos: [1:0]; default: 3;
         *  M8 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m8_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m8_mode_ctrl_reg_t;

/** Type of m9_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m9_mode : R/W; bitpos: [1:0]; default: 3;
         *  M9 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m9_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m9_mode_ctrl_reg_t;

/** Type of m10_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m10_mode : R/W; bitpos: [1:0]; default: 3;
         *  M10 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m10_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m10_mode_ctrl_reg_t;

/** Type of m11_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m11_mode : R/W; bitpos: [1:0]; default: 3;
         *  M11 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m11_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m11_mode_ctrl_reg_t;

/** Type of m12_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m12_mode : R/W; bitpos: [1:0]; default: 3;
         *  M12 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m12_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m12_mode_ctrl_reg_t;

/** Type of m13_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m13_mode : R/W; bitpos: [1:0]; default: 3;
         *  M13 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m13_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m13_mode_ctrl_reg_t;

/** Type of m14_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m14_mode : R/W; bitpos: [1:0]; default: 3;
         *  M14 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m14_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m14_mode_ctrl_reg_t;

/** Type of m15_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m15_mode : R/W; bitpos: [1:0]; default: 3;
         *  M15 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m15_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m15_mode_ctrl_reg_t;

/** Type of m16_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m16_mode : R/W; bitpos: [1:0]; default: 3;
         *  M16 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m16_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m16_mode_ctrl_reg_t;

/** Type of m17_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m17_mode : R/W; bitpos: [1:0]; default: 3;
         *  M17 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m17_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m17_mode_ctrl_reg_t;

/** Type of m18_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m18_mode : R/W; bitpos: [1:0]; default: 3;
         *  M18 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m18_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m18_mode_ctrl_reg_t;

/** Type of m19_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m19_mode : R/W; bitpos: [1:0]; default: 3;
         *  M19 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m19_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m19_mode_ctrl_reg_t;

/** Type of m20_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m20_mode : R/W; bitpos: [1:0]; default: 3;
         *  M20 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m20_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m20_mode_ctrl_reg_t;

/** Type of m21_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m21_mode : R/W; bitpos: [1:0]; default: 3;
         *  M21 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m21_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m21_mode_ctrl_reg_t;

/** Type of m22_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m22_mode : R/W; bitpos: [1:0]; default: 3;
         *  M22 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m22_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m22_mode_ctrl_reg_t;

/** Type of m23_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m23_mode : R/W; bitpos: [1:0]; default: 3;
         *  M23 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m23_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m23_mode_ctrl_reg_t;

/** Type of m24_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m24_mode : R/W; bitpos: [1:0]; default: 3;
         *  M24 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m24_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m24_mode_ctrl_reg_t;

/** Type of m25_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m25_mode : R/W; bitpos: [1:0]; default: 3;
         *  M25 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m25_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m25_mode_ctrl_reg_t;

/** Type of m26_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m26_mode : R/W; bitpos: [1:0]; default: 3;
         *  M26 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m26_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m26_mode_ctrl_reg_t;

/** Type of m27_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m27_mode : R/W; bitpos: [1:0]; default: 3;
         *  M27 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m27_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m27_mode_ctrl_reg_t;

/** Type of m28_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m28_mode : R/W; bitpos: [1:0]; default: 3;
         *  M28 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m28_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m28_mode_ctrl_reg_t;

/** Type of m29_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m29_mode : R/W; bitpos: [1:0]; default: 3;
         *  M29 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m29_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m29_mode_ctrl_reg_t;

/** Type of m30_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m30_mode : R/W; bitpos: [1:0]; default: 3;
         *  M30 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m30_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m30_mode_ctrl_reg_t;

/** Type of m31_mode_ctrl register
 *  Tee mode control register
 */
typedef union {
    struct {
        /** m31_mode : R/W; bitpos: [1:0]; default: 3;
         *  M31 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
         *  tee_mode
         */
        uint32_t m31_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_m31_mode_ctrl_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  reg_clk_en
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tee_clock_gate_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version register
 */
typedef union {
    struct {
        /** date_reg : R/W; bitpos: [27:0]; default: 35672706;
         *  reg_tee_date
         */
        uint32_t date_reg:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} tee_date_reg_t;


typedef struct tee_dev_t {
    volatile tee_m0_mode_ctrl_reg_t m0_mode_ctrl;
    volatile tee_m1_mode_ctrl_reg_t m1_mode_ctrl;
    volatile tee_m2_mode_ctrl_reg_t m2_mode_ctrl;
    volatile tee_m3_mode_ctrl_reg_t m3_mode_ctrl;
    volatile tee_m4_mode_ctrl_reg_t m4_mode_ctrl;
    volatile tee_m5_mode_ctrl_reg_t m5_mode_ctrl;
    volatile tee_m6_mode_ctrl_reg_t m6_mode_ctrl;
    volatile tee_m7_mode_ctrl_reg_t m7_mode_ctrl;
    volatile tee_m8_mode_ctrl_reg_t m8_mode_ctrl;
    volatile tee_m9_mode_ctrl_reg_t m9_mode_ctrl;
    volatile tee_m10_mode_ctrl_reg_t m10_mode_ctrl;
    volatile tee_m11_mode_ctrl_reg_t m11_mode_ctrl;
    volatile tee_m12_mode_ctrl_reg_t m12_mode_ctrl;
    volatile tee_m13_mode_ctrl_reg_t m13_mode_ctrl;
    volatile tee_m14_mode_ctrl_reg_t m14_mode_ctrl;
    volatile tee_m15_mode_ctrl_reg_t m15_mode_ctrl;
    volatile tee_m16_mode_ctrl_reg_t m16_mode_ctrl;
    volatile tee_m17_mode_ctrl_reg_t m17_mode_ctrl;
    volatile tee_m18_mode_ctrl_reg_t m18_mode_ctrl;
    volatile tee_m19_mode_ctrl_reg_t m19_mode_ctrl;
    volatile tee_m20_mode_ctrl_reg_t m20_mode_ctrl;
    volatile tee_m21_mode_ctrl_reg_t m21_mode_ctrl;
    volatile tee_m22_mode_ctrl_reg_t m22_mode_ctrl;
    volatile tee_m23_mode_ctrl_reg_t m23_mode_ctrl;
    volatile tee_m24_mode_ctrl_reg_t m24_mode_ctrl;
    volatile tee_m25_mode_ctrl_reg_t m25_mode_ctrl;
    volatile tee_m26_mode_ctrl_reg_t m26_mode_ctrl;
    volatile tee_m27_mode_ctrl_reg_t m27_mode_ctrl;
    volatile tee_m28_mode_ctrl_reg_t m28_mode_ctrl;
    volatile tee_m29_mode_ctrl_reg_t m29_mode_ctrl;
    volatile tee_m30_mode_ctrl_reg_t m30_mode_ctrl;
    volatile tee_m31_mode_ctrl_reg_t m31_mode_ctrl;
    volatile tee_clock_gate_reg_t clock_gate;
    uint32_t reserved_084[990];
    volatile tee_date_reg_t date;
} tee_dev_t;

extern tee_dev_t TEE;

#ifndef __cplusplus
_Static_assert(sizeof(tee_dev_t) == 0x1000, "Invalid size of tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
