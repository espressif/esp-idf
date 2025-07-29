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

/** Group: configure_register */
/** Type of cfg register
 *  configure rng register
 */
typedef union {
    struct {
        /** sample_enable : R/W; bitpos: [0]; default: 0;
         *  enable rng RO
         *  1: enable RO
         *  0: disable RO
         */
        uint32_t sample_enable:1;
        /** timer_pscale : R/W; bitpos: [8:1]; default: 255;
         *  configure rng timer clk div
         */
        uint32_t timer_pscale:8;
        /** timer_en : R/W; bitpos: [9]; default: 1;
         *  enable rng xor async rng timer
         */
        uint32_t timer_en:1;
        uint32_t reserved_10:14;
        /** sample_cnt : RO; bitpos: [31:24]; default: 0;
         *  get rng RO sample cnt
         */
        uint32_t sample_cnt:8;
    };
    uint32_t val;
} rng_cfg_reg_t;

/** Type of data register
 *  RNG result register
 */
typedef union {
    struct {
        /** data : RO; bitpos: [31:0]; default: 0;
         *  get rng data
         */
        uint32_t data:32;
    };
    uint32_t val;
} rng_data_reg_t;

/** Type of rstn register
 *  rng rstn register
 */
typedef union {
    struct {
        /** rstn : R/W; bitpos: [0]; default: 1;
         *  enable rng system reset: 1: not reset, 0: reset
         */
        uint32_t rstn:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rng_rstn_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 2425091;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} rng_date_reg_t;


typedef struct {
    volatile rng_cfg_reg_t cfg;
    volatile rng_data_reg_t data;
    volatile rng_rstn_reg_t rstn;
    volatile rng_date_reg_t date;
} rng_dev_t;

extern rng_dev_t LP_TRNG;

#ifndef __cplusplus
_Static_assert(sizeof(rng_dev_t) == 0x10, "Invalid size of rng_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
