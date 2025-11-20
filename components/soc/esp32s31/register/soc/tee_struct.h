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

/** Group: Tee mode control register */
/** Type of m0_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m0_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures M$n security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t m0_mode:2;
        /** m0_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m0 tee configuration
         */
        uint32_t m0_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} tee_m0_mode_ctrl_reg_t;

/** Type of m1_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m1_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures M$n security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t m1_mode:2;
        /** m1_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m1 tee configuration
         */
        uint32_t m1_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} tee_m1_mode_ctrl_reg_t;

/** Type of mn_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** mn_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures Mn security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t mn_mode:2;
        /** mn_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m3 tee configuration
         */
        uint32_t mn_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} tee_mn_mode_ctrl_reg_t;

/** Type of mb_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** mb_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures Mb security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t mb_mode:2;
        /** mb_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m20 tee configuration
         */
        uint32_t mb_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} tee_mb_mode_ctrl_reg_t;


/** Group: Core supervison mode control register */
/** Type of supervisor_priv_sel register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** core0_supervisor_priv_sel : R/W; bitpos: [0]; default: 0;
         *  Configures the privilege of the core0 supervisor mode .
         *  0: same as user_mode
         *  1: same as machine_mode
         */
        uint32_t core0_supervisor_priv_sel:1;
        /** core1_supervisor_priv_sel : R/W; bitpos: [1]; default: 0;
         *  Configures the privilege of the core0 supervisor mode .
         *  0: same as user_mode
         *  1: same as machine_mode
         */
        uint32_t core1_supervisor_priv_sel:1;
        /** core0_supervisor_priv_sel_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock reg_core0_supervisor_priv_sel configuration
         */
        uint32_t core0_supervisor_priv_sel_lock:1;
        /** core1_supervisor_priv_sel_lock : R/W; bitpos: [3]; default: 0;
         *  Set 1 to lock reg_core1_supervisor_priv_sel configuration
         */
        uint32_t core1_supervisor_priv_sel_lock:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} tee_supervisor_priv_sel_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: enable automatic clock gating
         *  1: keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tee_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38822480;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} tee_date_reg_t;


typedef struct {
    volatile tee_m0_mode_ctrl_reg_t m0_mode_ctrl;
    volatile tee_m1_mode_ctrl_reg_t m1_mode_ctrl;
    uint32_t reserved_008;
    volatile tee_mn_mode_ctrl_reg_t mn_mode_ctrl[13];
    uint32_t reserved_040[4];
    volatile tee_mb_mode_ctrl_reg_t mb_mode_ctrl[68];
    uint32_t reserved_160[40];
    volatile tee_supervisor_priv_sel_reg_t supervisor_priv_sel;
    uint32_t reserved_204[125];
    volatile tee_clock_gate_reg_t clock_gate;
    volatile tee_date_reg_t date;
} tee_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(tee_dev_t) == 0x400, "Invalid size of tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
