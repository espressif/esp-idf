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
/** Type of m0_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m0_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures M0 security level mode.\\
         *  0: tee_mode \\
         *  1: ree_mode0 \\
         *  2: ree_mode1 \\
         *  3: ree_mode2  \\
         */
        uint32_t m0_mode:2;
        /** m0_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m0 tee configuration
         */
        uint32_t m0_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_tee_m0_mode_ctrl_reg_t;


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
} lp_tee_clock_gate_reg_t;


/** Group: Force access to hpmem configuration register */
/** Type of force_acc_hp register
 *  Force access to hpmem configuration register
 */
typedef union {
    struct {
        /** force_acc_hpmem_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to allow LP CPU to force access to HP_MEM regardless of
         *  permission management.\\
         *  0: disable force access HP_MEM \\
         *  1: enable force access HP_MEM \\
         */
        uint32_t force_acc_hpmem_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_force_acc_hp_reg_t;


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
} lp_tee_date_reg_t;


typedef struct {
    volatile lp_tee_m0_mode_ctrl_reg_t m0_mode_ctrl;
    volatile lp_tee_clock_gate_reg_t clock_gate;
    uint32_t reserved_008[34];
    volatile lp_tee_force_acc_hp_reg_t force_acc_hp;
    uint32_t reserved_094[26];
    volatile lp_tee_date_reg_t date;
} lp_tee_dev_t;

extern lp_tee_dev_t LP_TEE;

#ifndef __cplusplus
_Static_assert(sizeof(lp_tee_dev_t) == 0x100, "Invalid size of lp_tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
