/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt registers */
/** Type of int_raw register
 *  ECC interrupt raw register, valid in level.
 */
typedef union {
    struct {
        /** calc_done_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status bit  for the ecc calculate done interrupt
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
         *  The masked interrupt status bit  for the ecc calculate done interrupt
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
         *  The interrupt enable bit  for the ecc calculate done interrupt
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
         *  Set this bit to clear the ecc calculate done interrupt
         */
        uint32_t calc_done_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ecc_mult_int_clr_reg_t;


/** Group: Configuration registers */
/** Type of conf register
 *  ECC configure register
 */
typedef union {
    struct {
        /** start : R/W/SC; bitpos: [0]; default: 0;
         *  Set this bit to start a ECC operation.
         */
        uint32_t start:1;
        /** reset : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset ECC
         */
        uint32_t reset:1;
        /** key_length : R/W; bitpos: [2]; default: 0;
         *  0:192bit key length mode. 1:256bit key length mode
         */
        uint32_t key_length:1;
        /** security_mode : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t security_mode:1;
        /** clk_en : R/W; bitpos: [4]; default: 0;
         *  clk gate
         */
        uint32_t clk_en:1;
        /** work_mode : R/W; bitpos: [7:5]; default: 0;
         *  ECC operation mode register.
         */
        uint32_t work_mode:3;
        /** verification_result : RO/SS; bitpos: [8]; default: 0;
         *  ECC verification result register.
         */
        uint32_t verification_result:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} ecc_mult_conf_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 34636176;
         *  ECC mult version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ecc_mult_date_reg_t;


typedef struct {
    uint32_t reserved_000[3];
    volatile ecc_mult_int_raw_reg_t int_raw;
    volatile ecc_mult_int_st_reg_t int_st;
    volatile ecc_mult_int_ena_reg_t int_ena;
    volatile ecc_mult_int_clr_reg_t int_clr;
    volatile ecc_mult_conf_reg_t conf;
    uint32_t reserved_020[55];
    volatile ecc_mult_date_reg_t date;
} ecc_mult_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(ecc_mult_dev_t) == 0x100, "Invalid size of ecc_mult_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
