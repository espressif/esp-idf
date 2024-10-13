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

/** Group: key register */
/** Type of key_0 register
 *  Key material key_0 configure register
 */
typedef union {
    struct {
        /** key_0 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_0 that is a part of key material.
         */
        uint32_t key_0:32;
    };
    uint32_t val;
} aes_key_0_reg_t;

/** Type of key_1 register
 *  Key material key_1 configure register
 */
typedef union {
    struct {
        /** key_1 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_1 that is a part of key material.
         */
        uint32_t key_1:32;
    };
    uint32_t val;
} aes_key_1_reg_t;

/** Type of key_2 register
 *  Key material key_2 configure register
 */
typedef union {
    struct {
        /** key_2 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_2 that is a part of key material.
         */
        uint32_t key_2:32;
    };
    uint32_t val;
} aes_key_2_reg_t;

/** Type of key_3 register
 *  Key material key_3 configure register
 */
typedef union {
    struct {
        /** key_3 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_3 that is a part of key material.
         */
        uint32_t key_3:32;
    };
    uint32_t val;
} aes_key_3_reg_t;

/** Type of key_4 register
 *  Key material key_4 configure register
 */
typedef union {
    struct {
        /** key_4 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_4 that is a part of key material.
         */
        uint32_t key_4:32;
    };
    uint32_t val;
} aes_key_4_reg_t;

/** Type of key_5 register
 *  Key material key_5 configure register
 */
typedef union {
    struct {
        /** key_5 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_5 that is a part of key material.
         */
        uint32_t key_5:32;
    };
    uint32_t val;
} aes_key_5_reg_t;

/** Type of key_6 register
 *  Key material key_6 configure register
 */
typedef union {
    struct {
        /** key_6 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_6 that is a part of key material.
         */
        uint32_t key_6:32;
    };
    uint32_t val;
} aes_key_6_reg_t;

/** Type of key_7 register
 *  Key material key_7 configure register
 */
typedef union {
    struct {
        /** key_7 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores key_7 that is a part of key material.
         */
        uint32_t key_7:32;
    };
    uint32_t val;
} aes_key_7_reg_t;


/** Group: text in register */
/** Type of text_in_0 register
 *  source text material text_in_0 configure register
 */
typedef union {
    struct {
        /** text_in_0 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_in_0 that is a part of source text material.
         */
        uint32_t text_in_0:32;
    };
    uint32_t val;
} aes_text_in_0_reg_t;

/** Type of text_in_1 register
 *  source text material text_in_1 configure register
 */
typedef union {
    struct {
        /** text_in_1 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_in_1 that is a part of source text material.
         */
        uint32_t text_in_1:32;
    };
    uint32_t val;
} aes_text_in_1_reg_t;

/** Type of text_in_2 register
 *  source text material text_in_2 configure register
 */
typedef union {
    struct {
        /** text_in_2 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_in_2 that is a part of source text material.
         */
        uint32_t text_in_2:32;
    };
    uint32_t val;
} aes_text_in_2_reg_t;

/** Type of text_in_3 register
 *  source text material text_in_3 configure register
 */
typedef union {
    struct {
        /** text_in_3 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_in_3 that is a part of source text material.
         */
        uint32_t text_in_3:32;
    };
    uint32_t val;
} aes_text_in_3_reg_t;


/** Group: text out register */
/** Type of text_out_0 register
 *  result text material text_out_0 configure register
 */
typedef union {
    struct {
        /** text_out_0 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_out_0 that is a part of result text material.
         */
        uint32_t text_out_0:32;
    };
    uint32_t val;
} aes_text_out_0_reg_t;

/** Type of text_out_1 register
 *  result text material text_out_1 configure register
 */
typedef union {
    struct {
        /** text_out_1 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_out_1 that is a part of result text material.
         */
        uint32_t text_out_1:32;
    };
    uint32_t val;
} aes_text_out_1_reg_t;

/** Type of text_out_2 register
 *  result text material text_out_2 configure register
 */
typedef union {
    struct {
        /** text_out_2 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_out_2 that is a part of result text material.
         */
        uint32_t text_out_2:32;
    };
    uint32_t val;
} aes_text_out_2_reg_t;

/** Type of text_out_3 register
 *  result text material text_out_3 configure register
 */
typedef union {
    struct {
        /** text_out_3 : R/W; bitpos: [31:0]; default: 0;
         *  This bits stores text_out_3 that is a part of result text material.
         */
        uint32_t text_out_3:32;
    };
    uint32_t val;
} aes_text_out_3_reg_t;


/** Group: Configuration register */
/** Type of mode register
 *  AES Mode register
 */
typedef union {
    struct {
        /** mode : R/W; bitpos: [2:0]; default: 0;
         *  This bits decides which one operation mode will be used. 3'd0: AES-EN-128, 3'd1:
         *  AES-EN-192, 3'd2: AES-EN-256, 3'd4: AES-DE-128, 3'd5: AES-DE-192, 3'd6: AES-DE-256.
         */
        uint32_t mode:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} aes_mode_reg_t;

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

/** Type of block_mode register
 *  AES cipher block mode register
 */
typedef union {
    struct {
        /** block_mode : R/W; bitpos: [2:0]; default: 0;
         *  Those bits decides which block mode will be used. 0x0: ECB, 0x1: CBC, 0x2: OFB,
         *  0x3: CTR, 0x4: CFB-8, 0x5: CFB-128, 0x6: GCM, 0x7: reserved.
         */
        uint32_t block_mode:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} aes_block_mode_reg_t;

/** Type of block_num register
 *  AES block number register
 */
typedef union {
    struct {
        /** block_num : R/W; bitpos: [31:0]; default: 0;
         *  Those bits stores the number of Plaintext/ciphertext block.
         */
        uint32_t block_num:32;
    };
    uint32_t val;
} aes_block_num_reg_t;

/** Type of inc_sel register
 *  Standard incrementing function configure register
 */
typedef union {
    struct {
        /** inc_sel : R/W; bitpos: [0]; default: 0;
         *  This bit decides the standard incrementing function. 0: INC32. 1: INC128.
         */
        uint32_t inc_sel:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_inc_sel_reg_t;

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


/** Group: Control/Status register */
/** Type of trigger register
 *  AES trigger register
 */
typedef union {
    struct {
        /** trigger : WT; bitpos: [0]; default: 0;
         *  Set this bit to start AES calculation.
         */
        uint32_t trigger:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_trigger_reg_t;

/** Type of state register
 *  AES state register
 */
typedef union {
    struct {
        /** state : RO; bitpos: [1:0]; default: 0;
         *  Those bits shows AES status. For typical AES, 0: idle, 1: busy. For DMA-AES, 0:
         *  idle, 1: busy, 2: calculation_done.
         */
        uint32_t state:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} aes_state_reg_t;

/** Type of dma_enable register
 *  DMA-AES working mode register
 */
typedef union {
    struct {
        /** dma_enable : R/W; bitpos: [0]; default: 0;
         *  1'b0: typical AES working mode, 1'b1: DMA-AES working mode.
         */
        uint32_t dma_enable:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_dma_enable_reg_t;

/** Type of continue register
 *  AES continue register
 */
typedef union {
    struct {
        /** continue : WT; bitpos: [0]; default: 0;
         *  Set this bit to continue GCM operation.
         */
        uint32_t conti:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_continue_reg_t;

/** Type of dma_exit register
 *  AES-DMA exit config
 */
typedef union {
    struct {
        /** dma_exit : WT; bitpos: [0]; default: 0;
         *  Set this register to leave calculation done stage. Recommend to use it after
         *  software finishes reading DMA's output buffer.
         */
        uint32_t dma_exit:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_dma_exit_reg_t;


/** Group: memory type */

/** Group: interrupt register */
/** Type of int_clear register
 *  AES Interrupt clear register
 */
typedef union {
    struct {
        /** int_clear : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the AES interrupt.
         */
        uint32_t int_clear:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} aes_int_clear_reg_t;

/** Type of int_ena register
 *  AES Interrupt enable register
 */
typedef union {
    struct {
        /** int_ena : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable interrupt that occurs when DMA-AES calculation is done.
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
        /** date : R/W; bitpos: [29:0]; default: 538513936;
         *  This bits stores the version information of AES.
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} aes_date_reg_t;


typedef struct {
    volatile aes_key_0_reg_t key_0;
    volatile aes_key_1_reg_t key_1;
    volatile aes_key_2_reg_t key_2;
    volatile aes_key_3_reg_t key_3;
    volatile aes_key_4_reg_t key_4;
    volatile aes_key_5_reg_t key_5;
    volatile aes_key_6_reg_t key_6;
    volatile aes_key_7_reg_t key_7;
    volatile aes_text_in_0_reg_t text_in_0;
    volatile aes_text_in_1_reg_t text_in_1;
    volatile aes_text_in_2_reg_t text_in_2;
    volatile aes_text_in_3_reg_t text_in_3;
    volatile aes_text_out_0_reg_t text_out_0;
    volatile aes_text_out_1_reg_t text_out_1;
    volatile aes_text_out_2_reg_t text_out_2;
    volatile aes_text_out_3_reg_t text_out_3;
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
} aes_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(aes_dev_t) == 0xbc, "Invalid size of aes_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
