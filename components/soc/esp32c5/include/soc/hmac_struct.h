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

/** Group: Configuration Register */
/** Type of set_start register
 *  Process control register 0.
 */
typedef union {
    struct {
        /** set_start : WS; bitpos: [0]; default: 0;
         *  Start hmac operation.
         */
        uint32_t set_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_start_reg_t;

/** Type of set_para_purpose register
 *  Configure purpose.
 */
typedef union {
    struct {
        /** purpose_set : WO; bitpos: [3:0]; default: 0;
         *  Set hmac parameter purpose.
         */
        uint32_t purpose_set:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hmac_set_para_purpose_reg_t;

/** Type of set_para_key register
 *  Configure key.
 */
typedef union {
    struct {
        /** key_set : WO; bitpos: [2:0]; default: 0;
         *  Set hmac parameter key.
         */
        uint32_t key_set:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hmac_set_para_key_reg_t;

/** Type of set_para_finish register
 *  Finish initial configuration.
 */
typedef union {
    struct {
        /** set_para_end : WS; bitpos: [0]; default: 0;
         *  Finish hmac configuration.
         */
        uint32_t set_para_end:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_para_finish_reg_t;

/** Type of set_message_one register
 *  Process control register 1.
 */
typedef union {
    struct {
        /** set_text_one : WS; bitpos: [0]; default: 0;
         *  Call SHA to calculate one message block.
         */
        uint32_t set_text_one:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_message_one_reg_t;

/** Type of set_message_ing register
 *  Process control register 2.
 */
typedef union {
    struct {
        /** set_text_ing : WS; bitpos: [0]; default: 0;
         *  Continue typical hmac.
         */
        uint32_t set_text_ing:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_message_ing_reg_t;

/** Type of set_message_end register
 *  Process control register 3.
 */
typedef union {
    struct {
        /** set_text_end : WS; bitpos: [0]; default: 0;
         *  Start hardware padding.
         */
        uint32_t set_text_end:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_message_end_reg_t;

/** Type of set_result_finish register
 *  Process control register 4.
 */
typedef union {
    struct {
        /** set_result_end : WS; bitpos: [0]; default: 0;
         *  After read result from upstream, then let hmac back to idle.
         */
        uint32_t set_result_end:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_result_finish_reg_t;

/** Type of set_invalidate_jtag register
 *  Invalidate register 0.
 */
typedef union {
    struct {
        /** set_invalidate_jtag : WS; bitpos: [0]; default: 0;
         *  Clear result from hmac downstream JTAG.
         */
        uint32_t set_invalidate_jtag:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_invalidate_jtag_reg_t;

/** Type of set_invalidate_ds register
 *  Invalidate register 1.
 */
typedef union {
    struct {
        /** set_invalidate_ds : WS; bitpos: [0]; default: 0;
         *  Clear result from hmac downstream DS.
         */
        uint32_t set_invalidate_ds:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_invalidate_ds_reg_t;

/** Type of set_message_pad register
 *  Process control register 5.
 */
typedef union {
    struct {
        /** set_text_pad : WO; bitpos: [0]; default: 0;
         *  Start software padding.
         */
        uint32_t set_text_pad:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_set_message_pad_reg_t;

/** Type of one_block register
 *  Process control register 6.
 */
typedef union {
    struct {
        /** set_one_block : WS; bitpos: [0]; default: 0;
         *  Don't have to do padding.
         */
        uint32_t set_one_block:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_one_block_reg_t;

/** Type of soft_jtag_ctrl register
 *  Jtag register 0.
 */
typedef union {
    struct {
        /** soft_jtag_ctrl : WS; bitpos: [0]; default: 0;
         *  Turn on JTAG verification.
         */
        uint32_t soft_jtag_ctrl:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_soft_jtag_ctrl_reg_t;

/** Type of wr_jtag register
 *  Jtag register 1.
 */
typedef union {
    struct {
        /** wr_jtag : WO; bitpos: [31:0]; default: 0;
         *  32-bit of key to be compared.
         */
        uint32_t wr_jtag:32;
    };
    uint32_t val;
} hmac_wr_jtag_reg_t;


/** Group: Status Register */
/** Type of query_error register
 *  Error register.
 */
typedef union {
    struct {
        /** qurey_check : RO; bitpos: [0]; default: 0;
         *  Hmac configuration state. 0: key are agree with purpose. 1: error
         */
        uint32_t qurey_check:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_query_error_reg_t;

/** Type of query_busy register
 *  Busy register.
 */
typedef union {
    struct {
        /** busy_state : RO; bitpos: [0]; default: 0;
         *  Hmac state. 1'b0: idle. 1'b1: busy
         */
        uint32_t busy_state:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hmac_query_busy_reg_t;


/** Group: Memory Type */

/** Group: Version Register */
/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [29:0]; default: 538969624;
         *  Hmac date information/ hmac version information.
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hmac_date_reg_t;


typedef struct hmac_dev_t {
    uint32_t reserved_000[16];
    volatile hmac_set_start_reg_t set_start;
    volatile hmac_set_para_purpose_reg_t set_para_purpose;
    volatile hmac_set_para_key_reg_t set_para_key;
    volatile hmac_set_para_finish_reg_t set_para_finish;
    volatile hmac_set_message_one_reg_t set_message_one;
    volatile hmac_set_message_ing_reg_t set_message_ing;
    volatile hmac_set_message_end_reg_t set_message_end;
    volatile hmac_set_result_finish_reg_t set_result_finish;
    volatile hmac_set_invalidate_jtag_reg_t set_invalidate_jtag;
    volatile hmac_set_invalidate_ds_reg_t set_invalidate_ds;
    volatile hmac_query_error_reg_t query_error;
    volatile hmac_query_busy_reg_t query_busy;
    uint32_t reserved_070[4];
    volatile uint32_t wr_message[16];
    volatile uint32_t rd_result[8];
    uint32_t reserved_0e0[4];
    volatile hmac_set_message_pad_reg_t set_message_pad;
    volatile hmac_one_block_reg_t one_block;
    volatile hmac_soft_jtag_ctrl_reg_t soft_jtag_ctrl;
    volatile hmac_wr_jtag_reg_t wr_jtag;
    uint32_t reserved_100[63];
    volatile hmac_date_reg_t date;
} hmac_dev_t;

extern hmac_dev_t HMAC;

#ifndef __cplusplus
_Static_assert(sizeof(hmac_dev_t) == 0x200, "Invalid size of hmac_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
