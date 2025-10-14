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

/** Group: Memory data */

/** Group: Interrupt registers */
/** Type of mult_int_raw register
 *  ECC raw interrupt status register
 */
typedef union {
    struct {
        /** mult_calc_done_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of the ECC_CALC_DONE_INT interrupt.
         */
        uint32_t mult_calc_done_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_raw_reg_t;

/** Type of mult_int_st register
 *  ECC masked interrupt status register
 */
typedef union {
    struct {
        /** mult_calc_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of the ECC_CALC_DONE_INT interrupt.
         */
        uint32_t mult_calc_done_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_st_reg_t;

/** Type of mult_int_ena register
 *  ECC interrupt enable register
 */
typedef union {
    struct {
        /** mult_calc_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable the ECC_CALC_DONE_INT interrupt.
         */
        uint32_t mult_calc_done_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_ena_reg_t;

/** Type of mult_int_clr register
 *  ECC interrupt clear register
 */
typedef union {
    struct {
        /** mult_calc_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear the ECC_CALC_DONE_INT interrupt.
         */
        uint32_t mult_calc_done_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_clr_reg_t;


/** Group: RX Control and configuration registers */
/** Type of mult_conf register
 *  ECC configuration register
 */
typedef union {
    struct {
        /** mult_start : R/W/SC; bitpos: [0]; default: 0;
         *  Configures whether to start calculation of ECC Accelerator. This bit will be
         *  self-cleared after the calculation is done.
         *  0: No effect
         *  1: Start calculation of ECC Accelerator
         */
        uint32_t mult_start:1;
        /** mult_reset : WT; bitpos: [1]; default: 0;
         *  Configures whether to reset ECC Accelerator.
         *  0: No effect
         *  1: Reset
         */
        uint32_t mult_reset:1;
        /** mult_key_length : R/W; bitpos: [3:2]; default: 0;
         *  Configures the key length mode bit of ECC Accelerator.
         *  0: P-192
         *  1: P-256
         *  2: P-384
         *  3: SM2.
         */
        uint32_t mult_key_length:2;
        /** mult_mod_base : R/W; bitpos: [4]; default: 0;
         *  Configures the mod base of mod operation, only valid in work_mode 8-11.
         *  0: n(order of curve)
         *  1: p(mod base of curve)
         */
        uint32_t mult_mod_base:1;
        /** mult_work_mode : R/W; bitpos: [8:5]; default: 0;
         *  Configures the work mode of ECC Accelerator.
         *  0: Point Multi mode
         *  1: Reserved
         *  2: Point Verif mode
         *  3: Point Verif + Multi mode
         *  4: Jacobian Point Multi mode
         *  5: Reserved
         *  6: Jacobian Point Verif mode
         *  7: Point Verif + Jacobian Point Multi mode
         *  8: Mod Add mode
         *  9. Mod Sub mode
         *  10: Mod Multi mode
         *  11: Mod Div mode
         */
        uint32_t mult_work_mode:4;
        /** mult_security_mode : R/W; bitpos: [9]; default: 0;
         *  Configures the security mode of ECC Accelerator.
         *  0: no secure function enabled.
         *  1: enable constant-time calculation in all point multiplication modes.
         */
        uint32_t mult_security_mode:1;
        uint32_t reserved_10:19;
        /** mult_verification_result : RO/SS; bitpos: [29]; default: 0;
         *  Represents the verification result of ECC Accelerator, valid only when calculation
         *  is done.
         */
        uint32_t mult_verification_result:1;
        /** mult_clk_en : R/W; bitpos: [30]; default: 0;
         *  Configures whether to force on register clock gate.
         *  0: No effect
         *  1: Force on
         */
        uint32_t mult_clk_en:1;
        /** mult_mem_clock_gate_force_on : R/W; bitpos: [31]; default: 0;
         *  Configures whether to force on ECC memory clock gate.
         *  0: No effect
         *  1: Force on
         */
        uint32_t mult_mem_clock_gate_force_on:1;
    };
    uint32_t val;
} ecc_mult_conf_reg_t;


/** Group: Version register */
/** Type of mult_date register
 *  Version control register
 */
typedef union {
    struct {
        /** mult_date : R/W; bitpos: [27:0]; default: 37781792;
         *  ECC mult version control register
         */
        uint32_t mult_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ecc_mult_date_reg_t;


typedef struct {
    uint32_t reserved_000[3];
    volatile ecc_mult_int_raw_reg_t mult_int_raw;
    volatile ecc_mult_int_st_reg_t mult_int_st;
    volatile ecc_mult_int_ena_reg_t mult_int_ena;
    volatile ecc_mult_int_clr_reg_t mult_int_clr;
    volatile ecc_mult_conf_reg_t mult_conf;
    uint32_t reserved_020[55];
    volatile ecc_mult_date_reg_t mult_date;
    volatile uint32_t mult_k[12];
    volatile uint32_t mult_px[12];
    volatile uint32_t mult_py[12];
    volatile uint32_t mult_qx[12];
    volatile uint32_t mult_qy[12];
    volatile uint32_t mult_qz[12];
} ecc_dev_t;

extern ecc_dev_t ECC;

#ifndef __cplusplus
_Static_assert(sizeof(ecc_dev_t) == 0x220, "Invalid size of ecc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
