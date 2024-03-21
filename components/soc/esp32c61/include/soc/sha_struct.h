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

/** Group: Control/Configuration Registers */
/** Type of mode register
 *  Configures SHA algorithm
 */
typedef union {
    struct {
        /** mode : R/W; bitpos: [2:0]; default: 0;
         *  Configures the SHA algorithm. \\
         *  0: SHA-1\\
         *  1: SHA-224\\
         *  2: SHA-256\\
         */
        uint32_t mode:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} sha_mode_reg_t;

/** Type of dma_block_num register
 *  Block number register (only effective for DMA-SHA)
 */
typedef union {
    struct {
        /** dma_block_num : R/W; bitpos: [5:0]; default: 0;
         *  Configures the DMA-SHA block number.
         */
        uint32_t dma_block_num:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} sha_dma_block_num_reg_t;

/** Type of start register
 *  Starts the SHA accelerator for Typical SHA operation
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** start : RO; bitpos: [31:1]; default: 0;
         *  Write 1 to start Typical SHA calculation.
         */
        uint32_t start:31;
    };
    uint32_t val;
} sha_start_reg_t;

/** Type of continue register
 *  Continues SHA operation (only effective in Typical SHA mode)
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** continue : RO; bitpos: [31:1]; default: 0;
         *  Write 1 to continue Typical SHA calculation.
         */
        uint32_t conti:31;
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


/** Group: Configuration Register */
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

/** Type of mode_3 register
 *  Initial configuration register 0.
 */
typedef union {
    struct {
        /** mode_3 : R/W; bitpos: [2:0]; default: 0;
         *  Sha3 mode
         */
        uint32_t mode_3:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} sha_3_mode_reg_t;

/** Type of clean_m_3 register
 *  Initial configuration register 1.
 */
typedef union {
    struct {
        /** clean_m_3 : WO; bitpos: [0]; default: 0;
         *  Clean Message.
         */
        uint32_t clean_m_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_clean_m_reg_t;

/** Type of dma_block_num_3 register
 *  DMA configuration register 0.
 */
typedef union {
    struct {
        /** dma_block_num_3 : R/W; bitpos: [5:0]; default: 0;
         *  DMA-SHA3 block number.
         */
        uint32_t dma_block_num_3:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} sha_3_dma_block_num_reg_t;

/** Type of start_3 register
 *  Typical SHA3 configuration register 0.
 */
typedef union {
    struct {
        /** start_3 : WO; bitpos: [0]; default: 0;
         *  Start typical sha3.
         */
        uint32_t start_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_start_reg_t;

/** Type of continue_3 register
 *  Typical SHA3 configuration register 1.
 */
typedef union {
    struct {
        /** continue_3 : WO; bitpos: [0]; default: 0;
         *  Continue typical sha3.
         */
        uint32_t continue_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_continue_reg_t;

/** Type of dma_start_3 register
 *  DMA configuration register 1.
 */
typedef union {
    struct {
        /** dma_start_3 : WO; bitpos: [0]; default: 0;
         *  Start dma-sha3.
         */
        uint32_t dma_start_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_dma_start_reg_t;

/** Type of dma_continue_3 register
 *  DMA configuration register 2.
 */
typedef union {
    struct {
        /** dma_continue_3 : WO; bitpos: [0]; default: 0;
         *  Continue dma-sha3.
         */
        uint32_t dma_continue_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_dma_continue_reg_t;

/** Type of shake_length_3 register
 *  DMA configuration register 3.
 */
typedef union {
    struct {
        /** shake_length_3 : WO; bitpos: [10:0]; default: 50;
         *  SHAKE output hash word length
         */
        uint32_t shake_length_3:11;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} sha_3_shake_length_reg_t;


/** Group: Status Registers */
/** Type of busy register
 *  Represents if SHA Accelerator is busy or not
 */
typedef union {
    struct {
        /** busy_state : RO; bitpos: [0]; default: 0;
         *  Represents the states of SHA accelerator. \\
         *  0: idle\\
         *  1: busy\\
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
        /** date : R/W; bitpos: [29:0]; default: 538972713;
         *  Version control register.
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sha_date_reg_t;


/** Group: memory type */

/** Group: Status Register */
/** Type of busy_3 register
 *  Busy register.
 */
typedef union {
    struct {
        /** busy_reg_3 : RO; bitpos: [0]; default: 0;
         *  Sha3 busy state. 1'b0: idle. 1'b1: busy.
         */
        uint32_t busy_reg_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_busy_reg_t;


/** Group: Interrupt Register */
/** Type of clear_int_3 register
 *  Interrupt clear register.
 */
typedef union {
    struct {
        /** clear_int_3 : WO; bitpos: [0]; default: 0;
         *  Clear sha3 interrupt.
         */
        uint32_t clear_int_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_clear_int_reg_t;

/** Type of int_ena_3 register
 *  Interrupt enable register.
 */
typedef union {
    struct {
        /** int_ena_3 : R/W; bitpos: [0]; default: 0;
         *  Sha3 interrupt enable register. 1'b0: disable(default). 1'b1:enable
         */
        uint32_t int_ena_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sha_3_int_ena_reg_t;


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
    uint32_t reserved_0c0[464];
    volatile sha_3_mode_reg_t mode_3;
    volatile sha_3_clean_m_reg_t clean_m_3;
    uint32_t reserved_808;
    volatile sha_3_dma_block_num_reg_t dma_block_num_3;
    volatile sha_3_start_reg_t start_3;
    volatile sha_3_continue_reg_t continue_3;
    volatile sha_3_busy_reg_t busy_3;
    volatile sha_3_dma_start_reg_t dma_start_3;
    volatile sha_3_dma_continue_reg_t dma_continue_3;
    volatile sha_3_clear_int_reg_t clear_int_3;
    volatile sha_3_int_ena_reg_t int_ena_3;
    volatile sha_3_shake_length_reg_t shake_length_3;
    uint32_t reserved_830[52];
    volatile uint32_t m_out_3[50];
    uint32_t reserved_9c8[14];
    volatile uint32_t m_3[50];
} sha_dev_t;

extern sha_dev_t SHA;

#ifndef __cplusplus
_Static_assert(sizeof(sha_dev_t) == 0xac8, "Invalid size of sha_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
