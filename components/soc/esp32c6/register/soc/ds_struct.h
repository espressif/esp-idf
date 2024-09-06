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

/** Group: memory type */

/** Group: Control/Status registers */
/** Type of set_start register
 *  DS start control register
 */
typedef union {
    struct {
        /** set_start : WT; bitpos: [0]; default: 0;
         *  set this bit to start DS operation.
         */
        uint32_t set_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ds_set_start_reg_t;

/** Type of set_continue register
 *  DS continue control register
 */
typedef union {
    struct {
        /** set_continue : WT; bitpos: [0]; default: 0;
         *  set this bit to continue DS operation.
         */
        uint32_t set_continue:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ds_set_continue_reg_t;

/** Type of set_finish register
 *  DS finish control register
 */
typedef union {
    struct {
        /** set_finish : WT; bitpos: [0]; default: 0;
         *  Set this bit to finish DS process.
         */
        uint32_t set_finish:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ds_set_finish_reg_t;

/** Type of query_busy register
 *  DS query busy register
 */
typedef union {
    struct {
        /** query_busy : RO; bitpos: [0]; default: 0;
         *  digital signature state. 1'b0: idle, 1'b1: busy
         */
        uint32_t query_busy:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ds_query_busy_reg_t;

/** Type of query_key_wrong register
 *  DS query key-wrong counter register
 */
typedef union {
    struct {
        /** query_key_wrong : RO; bitpos: [3:0]; default: 0;
         *  digital signature key wrong counter
         */
        uint32_t query_key_wrong:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ds_query_key_wrong_reg_t;

/** Type of query_check register
 *  DS query check result register
 */
typedef union {
    struct {
        /** md_error : RO; bitpos: [0]; default: 0;
         *  MD checkout result. 1'b0: MD check pass, 1'b1: MD check fail
         */
        uint32_t md_error:1;
        /** padding_bad : RO; bitpos: [1]; default: 0;
         *  padding checkout result. 1'b0: a good padding, 1'b1: a bad padding
         */
        uint32_t padding_bad:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} ds_query_check_reg_t;


/** Group: version control register */
/** Type of date register
 *  DS version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [29:0]; default: 538969624;
         *  ds version information
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} ds_date_reg_t;


typedef struct ds_dev_t {
    volatile uint32_t y[128];
    volatile uint32_t m[128];
    volatile uint32_t rb[128];
    volatile uint32_t box[12];
    volatile uint32_t iv[4];
    uint32_t reserved_640[112];
    volatile uint32_t x[128];
    volatile uint32_t z[128];
    uint32_t reserved_c00[128];
    volatile ds_set_start_reg_t set_start;
    volatile ds_set_continue_reg_t set_continue;
    volatile ds_set_finish_reg_t set_finish;
    volatile ds_query_busy_reg_t query_busy;
    volatile ds_query_key_wrong_reg_t query_key_wrong;
    volatile ds_query_check_reg_t query_check;
    uint32_t reserved_e18[2];
    volatile ds_date_reg_t date;
} ds_dev_t;

extern ds_dev_t DS;

#ifndef __cplusplus
_Static_assert(sizeof(ds_dev_t) == 0xe24, "Invalid size of ds_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
