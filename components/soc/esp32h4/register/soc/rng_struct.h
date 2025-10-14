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
/** Type of data register
 *  RNG result register
 */
typedef union {
    struct {
        /** rnd_data : RO; bitpos: [31:0]; default: 0;
         *  get rng data
         */
        uint32_t rnd_data:32;
    };
    uint32_t val;
} rng_data_reg_t;

/** Type of cfg register
 *  configure rng register
 */
typedef union {
    struct {
        /** sample_enable : R/W; bitpos: [0]; default: 0;
         *  reserved
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
        /** rtc_timer_en : R/W; bitpos: [11:10]; default: 3;
         *  reserved
         */
        uint32_t rtc_timer_en:2;
        uint32_t reserved_12:12;
        /** sample_cnt : RO; bitpos: [31:24]; default: 0;
         *  reserved
         */
        uint32_t sample_cnt:8;
    };
    uint32_t val;
} rng_cfg_reg_t;

/** Type of data_sync register
 *  rng result sync register
 */
typedef union {
    struct {
        /** rnd_sync_data : RO; bitpos: [31:0]; default: 0;
         *  get rnd sync result
         */
        uint32_t rnd_sync_data:32;
    };
    uint32_t val;
} rng_data_sync_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 36708608;
         *  RNG date information/ RNG version information.
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  force on reg clk
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} rng_date_reg_t;


typedef struct {
    volatile rng_data_reg_t data;
    volatile rng_cfg_reg_t cfg;
    volatile rng_data_sync_reg_t data_sync;
    uint32_t reserved_00c[252];
    volatile rng_date_reg_t date;
} rng_dev_t;

extern rng_dev_t TRNG;

#ifndef __cplusplus
_Static_assert(sizeof(rng_dev_t) == 0x400, "Invalid size of rng_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
