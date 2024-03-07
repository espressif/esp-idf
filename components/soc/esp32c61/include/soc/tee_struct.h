/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Tee mode control register */
/** Type of mn_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** mn_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures Mn security level mode.\\
         *  0: tee_mode \\
         *  1: ree_mode0 \\
         *  2: ree_mode1 \\
         *  3: ree_mode2  \\
         */
        uint32_t mn_mode:2;
        /** mn_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m0 tee configuration
         */
        uint32_t mn_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} tee_mn_mode_ctrl_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.\\
         *  0: enable automatic clock gating \\
         *  1: keep the clock always on  \\
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
        /** date_reg : R/W; bitpos: [27:0]; default: 35725664;
         *  Version control register
         */
        uint32_t date_reg:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} tee_date_reg_t;


typedef struct {
    volatile tee_mn_mode_ctrl_reg_t mn_mode_ctrl[32];
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
