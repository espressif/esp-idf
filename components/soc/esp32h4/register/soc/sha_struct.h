/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Control/Configuration Registers */
/** Type of mode register
 *  Configures SHA algorithm
 */
typedef union {
    struct {
        /** mode : R/W; bitpos: [3:0]; default: 2;
         *  Configures the SHA algorithm.
         *  0: SHA-1
         *  1: SHA2-224
         *  2: SHA2-256
         *  3: SHA2-384
         *  4: SHA2-512
         *  5: SHA2-512/224
         *  6: SHA2-512/256
         *  7: SHA2-512/t
         *  8: SHA3-224
         *  9: SHA3-256
         *  10: SHA3-384
         *  11: SHA3-512
         *  12: SHAKE128
         *  13: SHAKE256
         *  14: SM3
         */
        uint32_t mode:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sha_mode_reg_t;

/** Type of dma_block_num register
 *  Block number register (only effective for DMA-SHA)
 */
typedef union {
    struct {
        /** dma_block_num : R/W; bitpos: [15:0]; default: 0;
         *  Configures the DMA-SHA block number.
         */
        uint32_t dma_block_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} sha_dma_block_num_reg_t;

/** Type of start register
 *  Starts the SHA accelerator for Typical SHA operation
 */
typedef union {
    struct {
        /** start : WO; bitpos: [0]; default: 0;
         *  Start typical sha.
         */
        uint32_t start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_start_reg_t;

/** Type of continue register
 *  Continues SHA operation (only effective in Typical SHA mode)
 */
typedef union {
    struct {
        /** conti : WO; bitpos: [0]; default: 0;
         *  Continue typical sha.
         */
        uint32_t conti:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_continue_reg_t;

/** Type of dma_start register
 *  Starts the SHA accelerator for DMA-SHA operation
 */
typedef union {
    struct {
        /** dma_start : WO; bitpos: [0]; default: 0;
         *  Write 1 to start DMA-SHA calculation.
         */
        uint32_t dma_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_dma_start_reg_t;

/** Type of dma_continue register
 *  Continues SHA operation (only effective in DMA-SHA mode)
 */
typedef union {
    struct {
        /** dma_continue : WO; bitpos: [0]; default: 0;
         *  Write 1 to continue DMA-SHA calculation.
         */
        uint32_t dma_continue:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_dma_continue_reg_t;

/** Type of dma_rx_reset register
 *  DMA RX FIFO Reset Signal
 */
typedef union {
    struct {
        /** dma_rx_reset : WO; bitpos: [0]; default: 0;
         *  Write 1 to reset DMA RX FIFO
         */
        uint32_t dma_rx_reset:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_dma_rx_reset_reg_t;


/** Group: Configuration Register */
/** Type of t_string register
 *  SHA 512/t configuration register 0.
 */
typedef union {
    struct {
        /** t_string : R/W; bitpos: [31:0]; default: 0;
         *  Sha t_string (used if and only if mode == SHA_512/t).
         *  This field is only for internal debugging purposes. Do not use it in applications.
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
        /** t_length : R/W; bitpos: [6:0]; default: 0;
         *  Sha t_length (used if and only if mode == SHA_512/t).
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t t_length:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} sha_t_length_reg_t;


/** Group: Status Registers */
/** Type of busy register
 *  Represents if SHA Accelerator is busy or not
 */
typedef union {
    struct {
        /** busy_state : RO; bitpos: [0]; default: 0;
         *  Represents the states of SHA accelerator.
         *  0: idle
         *  1: busy
         */
        uint32_t busy_state:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_busy_reg_t;


/** Group: Interrupt Registers */
/** Type of clear_irq register
 *  DMA-SHA interrupt clear register
 */
typedef union {
    struct {
        /** clear_interrupt : WO; bitpos: [0]; default: 0;
         *  Write 1 to clear DMA-SHA interrupt.
         */
        uint32_t clear_interrupt:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_clear_irq_reg_t;

/** Type of irq_ena register
 *  DMA-SHA interrupt enable register
 */
typedef union {
    struct {
        /** interrupt_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable DMA-SHA interrupt.
         */
        uint32_t interrupt_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_irq_ena_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [29:0]; default: 539232291;
         *  Version control register.
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sha_date_reg_t;


/** Group: memory type */

typedef struct {
    volatile sha_mode_reg_t mode;
    uint32_t reserved_004[2];
    volatile sha_dma_block_num_reg_t dma_block_num;
    volatile sha_start_reg_t start;
    volatile sha_continue_reg_t conti;
    volatile sha_busy_reg_t busy;
    volatile sha_dma_start_reg_t dma_start;
    volatile sha_dma_continue_reg_t dma_continue;
    volatile sha_clear_irq_reg_t clear_irq;
    volatile sha_irq_ena_reg_t irq_ena;
    volatile sha_date_reg_t date;
    volatile sha_dma_rx_reset_reg_t dma_rx_reset;
    uint32_t reserved_034[3];
    volatile uint32_t h[16];
    volatile uint32_t m[32];
} sha_dev_t;

extern sha_dev_t SHA;

#ifndef __cplusplus
_Static_assert(sizeof(sha_dev_t) == 0x100, "Invalid size of sha_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
