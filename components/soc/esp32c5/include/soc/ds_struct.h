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

/** Group: memory type */

/** Group: Control/Status registers */
/** Type of set_start register
 *  Activates the DS module
 */
typedef union {
    struct {
        /** set_start : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to activate the DS peripheral.\\
         *  0: Invalid\\
         *  1: Activate the DS peripheral\\
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
 *  Ends DS operation
 */
typedef union {
    struct {
        /** set_finish : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to end DS operation. \\
         *  0: Invalid\\
         *  1: End DS operation\\
         */
        uint32_t set_finish:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ds_set_finish_reg_t;

/** Type of query_busy register
 *  Status of the DS module
 */
typedef union {
    struct {
        /** query_busy : RO; bitpos: [0]; default: 0;
         *  Represents whether or not the DS module is idle.\\
         *  0: The DS module is idle\\
         *  1: The DS module is busy\\
         */
        uint32_t query_busy:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ds_query_busy_reg_t;

/** Type of query_key_wrong register
 *  Checks the reason why \begin{math}DS_KEY\end{math} is not ready
 */
typedef union {
    struct {
        /** query_key_wrong : RO; bitpos: [3:0]; default: 0;
         *  Represents the specific problem with HMAC initialization.\\
         *  0: HMAC is not called\\
         *  1-15: HMAC was activated, but the DS peripheral did not successfully receive the
         *  \begin{math}DS_KEY\end{math} from the HMAC peripheral. (The biggest value is 15)\\
         */
        uint32_t query_key_wrong:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ds_query_key_wrong_reg_t;

/** Type of query_check register
 *  Queries DS check result
 */
typedef union {
    struct {
        /** md_error : RO; bitpos: [0]; default: 0;
         *  Represents whether or not the MD check passes.\\
         *  0: The MD check passes\\
         *  1: The MD check fails\\
         */
        uint32_t md_error:1;
        /** padding_bad : RO; bitpos: [1]; default: 0;
         *  Represents whether or not the padding check passes.\\
         *  0: The padding check passes\\
         *  1: The padding check fails\\
         */
        uint32_t padding_bad:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} ds_query_check_reg_t;


/** Group: Configuration registers */
/** Type of key_source register
 *  DS configure key source register
 */
typedef union {
    struct {
        /** key_source : R/W; bitpos: [0]; default: 0;
         *  digital signature key source bit. \\
         *  1'b0: key is from hmac.\\
         *  1'b1: key is from key manager. \\
         */
        uint32_t key_source:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ds_key_source_reg_t;


/** Group: version control register */
/** Type of date register
 *  DS version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [29:0]; default: 539166977;
         *  ds version information
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} ds_date_reg_t;


typedef struct {
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
    volatile ds_key_source_reg_t key_source;
    uint32_t reserved_e1c;
    volatile ds_date_reg_t date;
} ds_dev_t;

extern ds_dev_t DS;

#ifndef __cplusplus
_Static_assert(sizeof(ds_dev_t) == 0xe24, "Invalid size of ds_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
