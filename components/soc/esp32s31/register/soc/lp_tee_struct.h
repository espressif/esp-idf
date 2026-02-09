/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Tee mode control register */
/** Type of m2_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m2_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures M$n security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t m2_mode:2;
        /** m2_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m2 tee configuration
         */
        uint32_t m2_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_tee_m2_mode_ctrl_reg_t;

/** Type of m16_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m16_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures M$n security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t m16_mode:2;
        /** m16_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m16 tee configuration
         */
        uint32_t m16_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_tee_m16_mode_ctrl_reg_t;

/** Type of m17_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m17_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures M$n security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t m17_mode:2;
        /** m17_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m17 tee configuration
         */
        uint32_t m17_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_tee_m17_mode_ctrl_reg_t;

/** Type of m18_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m18_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures M$n security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t m18_mode:2;
        /** m18_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m18 tee configuration
         */
        uint32_t m18_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_tee_m18_mode_ctrl_reg_t;

/** Type of m19_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** m19_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures M$n security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t m19_mode:2;
        /** m19_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m19 tee configuration
         */
        uint32_t m19_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_tee_m19_mode_ctrl_reg_t;


/** Group: Force access to hpmem configuration register */
/** Type of force_acc_hp register
 *  Force access to hpmem configuration register
 */
typedef union {
    struct {
        /** force_acc_hpmem_en : R/W; bitpos: [7:0]; default: 0;
         *  Configures whether to allow LP CPU to force access to HP_MEM regardless of
         *  permission management.
         *  0x5a: enable force access HP_MEM
         *  other: disable force access HP_MEM
         */
        uint32_t force_acc_hpmem_en:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_force_acc_hp_reg_t;


/** Group: configure_register */
/** Type of lp_gpio_security register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_lock : R/W; bitpos: [7:0]; default: 0;
         *  This field decides whether lp_gpio_config can be locked, or not. 0 (default):
         *  unlocked. 1: locked.
         */
        uint32_t lp_gpio_lock:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_gpio_security_reg_t;

/** Type of hp_gpio_security_1 register
 *  need des
 */
typedef union {
    struct {
        /** hp_gpio_lock_p1 : R/W; bitpos: [31:0]; default: 0;
         *  This field decides whether hp_gpio_config of PIN0~31 can be locked, or not. 0
         *  (default): unlocked. 1: locked.
         */
        uint32_t hp_gpio_lock_p1:32;
    };
    uint32_t val;
} lp_tee_hp_gpio_security_1_reg_t;

/** Type of hp_gpio_security_2 register
 *  need des
 */
typedef union {
    struct {
        /** hp_gpio_lock_p2 : R/W; bitpos: [30:0]; default: 0;
         *  This field decides whether hp_gpio_config of PIN32~62 can be locked, or not. 0
         *  (default): unlocked. 1: locked.
         */
        uint32_t hp_gpio_lock_p2:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_tee_hp_gpio_security_2_reg_t;


/** Group: Clock gating register */
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
} lp_tee_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38822464;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_tee_date_reg_t;


typedef struct {
    volatile lp_tee_m2_mode_ctrl_reg_t m2_mode_ctrl;
    volatile lp_tee_m16_mode_ctrl_reg_t m16_mode_ctrl;
    volatile lp_tee_m17_mode_ctrl_reg_t m17_mode_ctrl;
    volatile lp_tee_m18_mode_ctrl_reg_t m18_mode_ctrl;
    volatile lp_tee_m19_mode_ctrl_reg_t m19_mode_ctrl;
    volatile lp_tee_force_acc_hp_reg_t force_acc_hp;
    volatile lp_tee_lp_gpio_security_reg_t lp_gpio_security;
    volatile lp_tee_hp_gpio_security_1_reg_t hp_gpio_security_1;
    volatile lp_tee_hp_gpio_security_2_reg_t hp_gpio_security_2;
    uint32_t reserved_024[245];
    volatile lp_tee_clock_gate_reg_t clock_gate;
    volatile lp_tee_date_reg_t date;
} lp_tee_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_tee_dev_t) == 0x400, "Invalid size of lp_tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
