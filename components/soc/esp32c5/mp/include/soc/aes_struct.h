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

/** Group: Key Registers */
/** Type of key_n register
 *  AES key data register 0
 */
typedef union {
    struct {
        /** key_0 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_0 that is a part of key material.
         */
        uint32_t key_0:32;
    };
    uint32_t val;
} aes_key_n_reg_t;


/** Group: TEXT_IN Registers */
/** Type of text_in_n register
 *  Source text data register 0
 */
typedef union {
    struct {
        /** text_in_0 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_in_0 that is a part of source text material.
         */
        uint32_t text_in_0:32;
    };
    uint32_t val;
} aes_text_in_n_reg_t;


/** Group: TEXT_OUT Registers */
/** Type of text_out_n register
 *  Result text data register 0
 */
typedef union {
    struct {
        /** text_out_0 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_out_0 that is a part of result text material.
         */
        uint32_t text_out_0:32;
    };
    uint32_t val;
} aes_text_out_n_reg_t;


/** Group: Control / Configuration Registers */
/** Type of mode register
 *  Defines key length and encryption / decryption
 */
typedef union {
    struct {
        /** mode : R/W; bitpos: [2:0]; default: 0;
         *  Configures the key length and encryption / decryption of the AES accelerator.\\
         *  0: AES-128 encryption\\
         *  1: AES-192 encryption\\
         *  2: AES-256 encryption\\
         *  3: Reserved\\
         *  4: AES-128 decryption\\
         *  5: AES-192 decryption\\
         *  6: AES-256 decryption\\
         *  7: Reserved\\
         */
        uint32_t mode:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} aes_mode_reg_t;

/** Type of trigger register
 *  Operation start controlling register
 */
typedef union {
    struct {
        /** trigger : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to start AES operation. \\
         *  0: No effect\\
         *  1: Start\\
         */
        uint32_t trigger:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_trigger_reg_t;

/** Type of dma_enable register
 *  Selects the working mode of the AES accelerator
 */
typedef union {
    struct {
        /** dma_enable : R/W; bitpos: [0]; default: 0;
         *  Configures the working mode of the AES accelerator. \\
         *  0: Typical AES\\
         *  1: DMA-AES\\
         */
        uint32_t dma_enable:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_dma_enable_reg_t;

/** Type of block_mode register
 *  Defines the block cipher mode
 */
typedef union {
    struct {
        /** block_mode : R/W; bitpos: [2:0]; default: 0;
         *  Configures the block cipher mode of the AES accelerator operating under the DMA-AES
         *  working mode. \\
         *  0: ECB (Electronic Code Block)\\
         *  1: CBC (Cipher Block Chaining)\\
         *  2: OFB (Output FeedBack)\\
         *  3: CTR (Counter)\\
         *  4: CFB8 (8-bit Cipher FeedBack)\\
         *  5: CFB128 (128-bit Cipher FeedBack)\\
         *  6: GCM\\
         *  7: Reserved\\
         */
        uint32_t block_mode:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} aes_block_mode_reg_t;

/** Type of block_num register
 *  Block number configuration register
 */
typedef union {
    struct {
        /** block_num : R/W; bitpos: [31:0]; default: 0;
         *  Represents the Block Number of plaintext or ciphertext when the AES accelerator
         *  operates under the DMA-AES working mode. For details, see Section <a
         *  href=sec:aes-block-number">link</a>. "
         */
        uint32_t block_num:32;
    };
    uint32_t val;
} aes_block_num_reg_t;

/** Type of inc_sel register
 *  Standard incrementing function register
 */
typedef union {
    struct {
        /** inc_sel : R/W; bitpos: [0]; default: 0;
         *  Configures the Standard Incrementing Function for CTR block operation. \\
         *  0: INC<SUB>32</SUB>\\
         *  1: INC<SUB>128</SUB>\\
         */
        uint32_t inc_sel:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_inc_sel_reg_t;

/** Type of dma_exit register
 *  Operation exit controlling register
 */
typedef union {
    struct {
        /** dma_exit : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to exit AES operation. \\
         *  0: No effect\\
         *  1: Exit\\
         *  Only valid for DMA-AES operation.
         */
        uint32_t dma_exit:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_dma_exit_reg_t;

/** Type of rx_reset register
 *  AES-DMA reset rx-fifo register
 */
typedef union {
    struct {
        /** rx_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset rx_fifo under dma_aes working mode.
         */
        uint32_t rx_reset:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_rx_reset_reg_t;

/** Type of tx_reset register
 *  AES-DMA reset tx-fifo register
 */
typedef union {
    struct {
        /** tx_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset tx_fifo under dma_aes working mode.
         */
        uint32_t tx_reset:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_tx_reset_reg_t;


/** Group: Configuration register */
/** Type of endian register
 *  AES Endian configure register
 */
typedef union {
    struct {
        /** endian : R/W; bitpos: [5:0]; default: 0;
         *  endian. [1:0] key endian, [3:2] text_in endian or in_stream endian,  [5:4] text_out
         *  endian or out_stream endian
         */
        uint32_t endian:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} aes_endian_reg_t;

/** Type of aad_block_num register
 *  Additional Authential Data block number register
 */
typedef union {
    struct {
        /** aad_block_num : R/W; bitpos: [31:0]; default: 0;
         *  Those bits stores the number of AAD block.
         */
        uint32_t aad_block_num:32;
    };
    uint32_t val;
} aes_aad_block_num_reg_t;

/** Type of remainder_bit_num register
 *  AES remainder bit number register
 */
typedef union {
    struct {
        /** remainder_bit_num : R/W; bitpos: [6:0]; default: 0;
         *  Those bits stores the number of remainder bit.
         */
        uint32_t remainder_bit_num:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} aes_remainder_bit_num_reg_t;

/** Type of pseudo register
 *  AES PSEUDO function configure register
 */
typedef union {
    struct {
        /** pseudo_en : R/W; bitpos: [0]; default: 0;
         *  This bit decides whether the pseudo round function is enable or not.
         */
        uint32_t pseudo_en:1;
        /** pseudo_base : R/W; bitpos: [4:1]; default: 2;
         *  Those bits decides the basic number of pseudo round number.
         */
        uint32_t pseudo_base:4;
        /** pseudo_inc : R/W; bitpos: [6:5]; default: 2;
         *  Those bits decides the increment number of pseudo round number
         */
        uint32_t pseudo_inc:2;
        /** pseudo_rng_cnt : R/W; bitpos: [9:7]; default: 7;
         *  Those bits decides the update frequency of the pseudo-key.
         */
        uint32_t pseudo_rng_cnt:3;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} aes_pseudo_reg_t;


/** Group: Status Register */
/** Type of state register
 *  Operation status register
 */
typedef union {
    struct {
        /** state : RO; bitpos: [1:0]; default: 0;
         *  Represents the working status of the AES accelerator. \\
         *  In Typical AES working mode:\\
         *  0: IDLE\\
         *  1: WORK\\
         *  2: No effect\\
         *  3: No effect\\
         *  In DMA-AES working mode:\\
         *  0: IDLE\\
         *  1: WORK\\
         *  2: DONE\\
         *  3: No effect\\
         */
        uint32_t state:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} aes_state_reg_t;


/** Group: memory type */

/** Group: Control/Status register */
/** Type of continue register
 *  AES continue register
 */
typedef union {
    struct {
        /** conti : WT; bitpos: [0]; default: 0;
         *  Set this bit to continue GCM operation.
         */
        uint32_t conti:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_continue_reg_t;


/** Group: Interrupt Registers */
/** Type of int_clear register
 *  DMA-AES interrupt clear register
 */
typedef union {
    struct {
        /** int_clear : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear AES interrupt. \\
         *  0: No effect \\
         *  1: Clear \\
         */
        uint32_t int_clear:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_int_clear_reg_t;

/** Type of int_ena register
 *  DMA-AES interrupt enable register
 */
typedef union {
    struct {
        /** int_ena : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable AES interrupt.\\
         *  0: Disable\\
         *  1: Enable \\
         */
        uint32_t int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_int_ena_reg_t;


/** Group: Version control register */
/** Type of date register
 *  AES version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36774000;
         *  This bits stores the version information of AES.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} aes_date_reg_t;


typedef struct {
    volatile aes_key_n_reg_t key_n[8];
    volatile aes_text_in_n_reg_t text_in_n[4];
    volatile aes_text_out_n_reg_t text_out_n[4];
    volatile aes_mode_reg_t mode;
    volatile aes_endian_reg_t endian;
    volatile aes_trigger_reg_t trigger;
    volatile aes_state_reg_t state;
    volatile uint32_t iv[4];
    volatile uint32_t h[4];
    volatile uint32_t j0[4];
    volatile uint32_t t0[4];
    volatile aes_dma_enable_reg_t dma_enable;
    volatile aes_block_mode_reg_t block_mode;
    volatile aes_block_num_reg_t block_num;
    volatile aes_inc_sel_reg_t inc_sel;
    volatile aes_aad_block_num_reg_t aad_block_num;
    volatile aes_remainder_bit_num_reg_t remainder_bit_num;
    volatile aes_continue_reg_t conti;
    volatile aes_int_clear_reg_t int_clear;
    volatile aes_int_ena_reg_t int_ena;
    volatile aes_date_reg_t date;
    volatile aes_dma_exit_reg_t dma_exit;
    uint32_t reserved_0bc;
    volatile aes_rx_reset_reg_t rx_reset;
    volatile aes_tx_reset_reg_t tx_reset;
    uint32_t reserved_0c8[2];
    volatile aes_pseudo_reg_t pseudo;
} aes_dev_t;

extern aes_dev_t AES;

#ifndef __cplusplus
_Static_assert(sizeof(aes_dev_t) == 0xd4, "Invalid size of aes_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
