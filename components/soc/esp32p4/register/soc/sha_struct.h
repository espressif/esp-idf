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
/** Type of mode register
 *  Initial configuration register.
 */
typedef union {
    struct {
        /** mode : R/W; bitpos: [2:0]; default: 0;
         *  Sha mode.
         */
        uint32_t mode:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} sha_mode_reg_t;

/** Type of t_string register
 *  SHA 512/t configuration register 0.
 */
typedef union {
    struct {
        /** t_string : R/W; bitpos: [31:0]; default: 0;
         *  Sha t_string (used if and only if mode == SHA_512/t).
         */
        uint32_t t_string:32;
    };
    uint32_t val;
} sha_t_string_reg_t;

/** Type of t_length register
 *  SHA 512/t configuration register 1.
 */
typedef union {
    struct {
        /** t_length : R/W; bitpos: [5:0]; default: 0;
         *  Sha t_length (used if and only if mode == SHA_512/t).
         */
        uint32_t t_length:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} sha_t_length_reg_t;

/** Type of dma_block_num register
 *  DMA configuration register 0.
 */
typedef union {
    struct {
        /** dma_block_num : R/W; bitpos: [5:0]; default: 0;
         *  Dma-sha block number.
         */
        uint32_t dma_block_num:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} sha_dma_block_num_reg_t;

/** Type of start register
 *  Typical SHA configuration register 0.
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** start : RO; bitpos: [31:1]; default: 0;
         *  Reserved.
         */
        uint32_t start:31;
    };
    uint32_t val;
} sha_start_reg_t;

/** Type of continue register
 *  Typical SHA configuration register 1.
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** continue : RO; bitpos: [31:1]; default: 0;
         *  Reserved.
         */
        uint32_t conti:31;
    };
    uint32_t val;
} sha_continue_reg_t;

/** Type of dma_start register
 *  DMA configuration register 1.
 */
typedef union {
    struct {
        /** dma_start : WO; bitpos: [0]; default: 0;
         *  Start dma-sha.
         */
        uint32_t dma_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_dma_start_reg_t;

/** Type of dma_continue register
 *  DMA configuration register 2.
 */
typedef union {
    struct {
        /** dma_continue : WO; bitpos: [0]; default: 0;
         *  Continue dma-sha.
         */
        uint32_t dma_continue:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_dma_continue_reg_t;


/** Group: Status Register */
/** Type of busy register
 *  Busy register.
 */
typedef union {
    struct {
        /** busy_state : RO; bitpos: [0]; default: 0;
         *  Sha busy state. 1'b0: idle. 1'b1: busy.
         */
        uint32_t busy_state:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_busy_reg_t;


/** Group: Interrupt Register */
/** Type of clear_irq register
 *  Interrupt clear register.
 */
typedef union {
    struct {
        /** clear_interrupt : WO; bitpos: [0]; default: 0;
         *  Clear sha interrupt.
         */
        uint32_t clear_interrupt:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_clear_irq_reg_t;

/** Type of irq_ena register
 *  Interrupt enable register.
 */
typedef union {
    struct {
        /** interrupt_ena : R/W; bitpos: [0]; default: 0;
         *  Sha interrupt enable register. 1'b0: disable(default). 1'b1: enable.
         */
        uint32_t interrupt_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_irq_ena_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [29:0]; default: 538972713;
         *  Sha date information/ sha version information.
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sha_date_reg_t;


/** Group: memory type */

typedef struct {
    volatile sha_mode_reg_t mode;
    volatile sha_t_string_reg_t t_string;
    volatile sha_t_length_reg_t t_length;
    volatile sha_dma_block_num_reg_t dma_block_num;
    volatile sha_start_reg_t start;
    volatile sha_continue_reg_t conti;
    volatile sha_busy_reg_t busy;
    volatile sha_dma_start_reg_t dma_start;
    volatile sha_dma_continue_reg_t dma_continue;
    volatile sha_clear_irq_reg_t clear_irq;
    volatile sha_irq_ena_reg_t irq_ena;
    volatile sha_date_reg_t date;
    uint32_t reserved_030[4];
    volatile uint32_t h[16];
    volatile uint32_t m[16];
} sha_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(sha_dev_t) == 0xc0, "Invalid size of sha_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
