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

/** Group: Memory data */

/** Group: Interrupt registers */
/** Type of int_raw register
 *  ECC interrupt raw register, valid in level.
 */
typedef union {
    struct {
        /** calc_done_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status bit  for the ecc_calc_done_int interrupt
         */
        uint32_t calc_done_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_raw_reg_t;

/** Type of int_st register
 *  ECC interrupt status register.
 */
typedef union {
    struct {
        /** calc_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit  for the ecc_calc_done_int interrupt
         */
        uint32_t calc_done_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_st_reg_t;

/** Type of int_ena register
 *  ECC interrupt enable register.
 */
typedef union {
    struct {
        /** calc_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit  for the ecc_calc_done_int interrupt
         */
        uint32_t calc_done_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_ena_reg_t;

/** Type of int_clr register
 *  ECC interrupt clear register.
 */
typedef union {
    struct {
        /** calc_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the ecc_calc_done_int interrupt
         */
        uint32_t calc_done_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_clr_reg_t;


/** Group: RX Control and configuration registers */
/** Type of conf register
 *  ECC configure register
 */
typedef union {
    struct {
        /** start : R/W/SC; bitpos: [0]; default: 0;
         *  Write 1 to start caculation of ECC Accelerator. This bit will be self-cleared after
         *  the caculatrion is done.
         */
        uint32_t start:1;
        /** reset : WT; bitpos: [1]; default: 0;
         *  Write 1 to reset ECC Accelerator.
         */
        uint32_t reset:1;
        /** key_length : R/W; bitpos: [2]; default: 0;
         *  The key length mode bit of ECC Accelerator.  0: P-192.  1: P-256.
         */
        uint32_t key_length:1;
        /** mod_base : R/W; bitpos: [3]; default: 0;
         *  The mod base of mod operation, only valid in work_mode 8-11. 0: n(order of curve).
         *  1: p(mod base of curve)
         */
        uint32_t mod_base:1;
        /** work_mode : R/W; bitpos: [7:4]; default: 0;
         *  The work mode bits of ECC Accelerator. 0: Point Mult Mode. 1: Reserved. 2: Point
         *  verification mode. 3: Point Verif+mult mode. 4: Jacobian Point Mult Mode. 5: Point
         *  Add Mode. 6: Jacobian Point Verification Mode. 7: Point Verif + Jacobian Mult Mode.
         *  8: mod addition. 9. mod substraction. 10: mod multiplication. 11: mod division.
         */
        uint32_t work_mode:4;
        /** security_mode : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t security_mode:1;
        uint32_t reserved_9:20;
        /** verification_result : RO/SS; bitpos: [29]; default: 0;
         *  The verification result bit of ECC Accelerator, only valid when calculation is done.
         */
        uint32_t verification_result:1;
        /** clk_en : R/W; bitpos: [30]; default: 0;
         *  Write 1 to force on register clock gate.
         */
        uint32_t clk_en:1;
        /** mem_clock_gate_force_on : R/W; bitpos: [31]; default: 0;
         *  ECC memory clock gate force on register
         */
        uint32_t mem_clock_gate_force_on:1;
    };
    uint32_t val;
} ecc_mult_conf_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36720704;
         *  ECC mult version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ecc_mult_date_reg_t;


typedef struct ecc_mult_dev_t {
    uint32_t reserved_000[3];
    volatile ecc_mult_int_raw_reg_t int_raw;
    volatile ecc_mult_int_st_reg_t int_st;
    volatile ecc_mult_int_ena_reg_t int_ena;
    volatile ecc_mult_int_clr_reg_t int_clr;
    volatile ecc_mult_conf_reg_t conf;
    uint32_t reserved_020[55];
    volatile ecc_mult_date_reg_t date;
    volatile uint32_t k[8];
    volatile uint32_t px[8];
    volatile uint32_t py[8];
} ecc_mult_dev_t;

extern ecc_mult_dev_t ECC;

#ifndef __cplusplus
_Static_assert(sizeof(ecc_mult_dev_t) == 0x160, "Invalid size of ecc_mult_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
