/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** AES_KEY_0_REG register
 *  AES key data register 0
 */
#define AES_KEY_0_REG (DR_REG_AES_BASE + 0x0)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_KEY_1_REG register
 *  AES key data register 0
 */
#define AES_KEY_1_REG (DR_REG_AES_BASE + 0x4)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_KEY_2_REG register
 *  AES key data register 0
 */
#define AES_KEY_2_REG (DR_REG_AES_BASE + 0x8)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_KEY_3_REG register
 *  AES key data register 0
 */
#define AES_KEY_3_REG (DR_REG_AES_BASE + 0xc)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_KEY_4_REG register
 *  AES key data register 0
 */
#define AES_KEY_4_REG (DR_REG_AES_BASE + 0x10)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_KEY_5_REG register
 *  AES key data register 0
 */
#define AES_KEY_5_REG (DR_REG_AES_BASE + 0x14)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_KEY_6_REG register
 *  AES key data register 0
 */
#define AES_KEY_6_REG (DR_REG_AES_BASE + 0x18)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_KEY_7_REG register
 *  AES key data register 0
 */
#define AES_KEY_7_REG (DR_REG_AES_BASE + 0x1c)
/** AES_KEY_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_0 that is a part of key material.
 */
#define AES_KEY_0    0xFFFFFFFFU
#define AES_KEY_0_M  (AES_KEY_0_V << AES_KEY_0_S)
#define AES_KEY_0_V  0xFFFFFFFFU
#define AES_KEY_0_S  0

/** AES_TEXT_IN_0_REG register
 *  Source text data register 0
 */
#define AES_TEXT_IN_0_REG (DR_REG_AES_BASE + 0x20)
/** AES_TEXT_IN_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_in_0 that is a part of source text material.
 */
#define AES_TEXT_IN_0    0xFFFFFFFFU
#define AES_TEXT_IN_0_M  (AES_TEXT_IN_0_V << AES_TEXT_IN_0_S)
#define AES_TEXT_IN_0_V  0xFFFFFFFFU
#define AES_TEXT_IN_0_S  0

/** AES_TEXT_IN_1_REG register
 *  Source text data register 0
 */
#define AES_TEXT_IN_1_REG (DR_REG_AES_BASE + 0x24)
/** AES_TEXT_IN_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_in_0 that is a part of source text material.
 */
#define AES_TEXT_IN_0    0xFFFFFFFFU
#define AES_TEXT_IN_0_M  (AES_TEXT_IN_0_V << AES_TEXT_IN_0_S)
#define AES_TEXT_IN_0_V  0xFFFFFFFFU
#define AES_TEXT_IN_0_S  0

/** AES_TEXT_IN_2_REG register
 *  Source text data register 0
 */
#define AES_TEXT_IN_2_REG (DR_REG_AES_BASE + 0x28)
/** AES_TEXT_IN_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_in_0 that is a part of source text material.
 */
#define AES_TEXT_IN_0    0xFFFFFFFFU
#define AES_TEXT_IN_0_M  (AES_TEXT_IN_0_V << AES_TEXT_IN_0_S)
#define AES_TEXT_IN_0_V  0xFFFFFFFFU
#define AES_TEXT_IN_0_S  0

/** AES_TEXT_IN_3_REG register
 *  Source text data register 0
 */
#define AES_TEXT_IN_3_REG (DR_REG_AES_BASE + 0x2c)
/** AES_TEXT_IN_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_in_0 that is a part of source text material.
 */
#define AES_TEXT_IN_0    0xFFFFFFFFU
#define AES_TEXT_IN_0_M  (AES_TEXT_IN_0_V << AES_TEXT_IN_0_S)
#define AES_TEXT_IN_0_V  0xFFFFFFFFU
#define AES_TEXT_IN_0_S  0

/** AES_TEXT_OUT_0_REG register
 *  Result text data register 0
 */
#define AES_TEXT_OUT_0_REG (DR_REG_AES_BASE + 0x30)
/** AES_TEXT_OUT_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_out_0 that is a part of result text material.
 */
#define AES_TEXT_OUT_0    0xFFFFFFFFU
#define AES_TEXT_OUT_0_M  (AES_TEXT_OUT_0_V << AES_TEXT_OUT_0_S)
#define AES_TEXT_OUT_0_V  0xFFFFFFFFU
#define AES_TEXT_OUT_0_S  0

/** AES_TEXT_OUT_1_REG register
 *  Result text data register 0
 */
#define AES_TEXT_OUT_1_REG (DR_REG_AES_BASE + 0x34)
/** AES_TEXT_OUT_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_out_0 that is a part of result text material.
 */
#define AES_TEXT_OUT_0    0xFFFFFFFFU
#define AES_TEXT_OUT_0_M  (AES_TEXT_OUT_0_V << AES_TEXT_OUT_0_S)
#define AES_TEXT_OUT_0_V  0xFFFFFFFFU
#define AES_TEXT_OUT_0_S  0

/** AES_TEXT_OUT_2_REG register
 *  Result text data register 0
 */
#define AES_TEXT_OUT_2_REG (DR_REG_AES_BASE + 0x38)
/** AES_TEXT_OUT_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_out_0 that is a part of result text material.
 */
#define AES_TEXT_OUT_0    0xFFFFFFFFU
#define AES_TEXT_OUT_0_M  (AES_TEXT_OUT_0_V << AES_TEXT_OUT_0_S)
#define AES_TEXT_OUT_0_V  0xFFFFFFFFU
#define AES_TEXT_OUT_0_S  0

/** AES_TEXT_OUT_3_REG register
 *  Result text data register 0
 */
#define AES_TEXT_OUT_3_REG (DR_REG_AES_BASE + 0x3c)
/** AES_TEXT_OUT_0 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_out_0 that is a part of result text material.
 */
#define AES_TEXT_OUT_0    0xFFFFFFFFU
#define AES_TEXT_OUT_0_M  (AES_TEXT_OUT_0_V << AES_TEXT_OUT_0_S)
#define AES_TEXT_OUT_0_V  0xFFFFFFFFU
#define AES_TEXT_OUT_0_S  0

/** AES_MODE_REG register
 *  Defines key length and encryption / decryption
 */
#define AES_MODE_REG (DR_REG_AES_BASE + 0x40)
/** AES_MODE : R/W; bitpos: [2:0]; default: 0;
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
#define AES_MODE    0x00000007U
#define AES_MODE_M  (AES_MODE_V << AES_MODE_S)
#define AES_MODE_V  0x00000007U
#define AES_MODE_S  0

/** AES_ENDIAN_REG register
 *  AES Endian configure register
 */
#define AES_ENDIAN_REG (DR_REG_AES_BASE + 0x44)
/** AES_ENDIAN : R/W; bitpos: [5:0]; default: 0;
 *  endian. [1:0] key endian, [3:2] text_in endian or in_stream endian,  [5:4] text_out
 *  endian or out_stream endian
 */
#define AES_ENDIAN    0x0000003FU
#define AES_ENDIAN_M  (AES_ENDIAN_V << AES_ENDIAN_S)
#define AES_ENDIAN_V  0x0000003FU
#define AES_ENDIAN_S  0

/** AES_TRIGGER_REG register
 *  Operation start controlling register
 */
#define AES_TRIGGER_REG (DR_REG_AES_BASE + 0x48)
/** AES_TRIGGER : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to start AES operation. \\
 *  0: No effect\\
 *  1: Start\\
 */
#define AES_TRIGGER    (BIT(0))
#define AES_TRIGGER_M  (AES_TRIGGER_V << AES_TRIGGER_S)
#define AES_TRIGGER_V  0x00000001U
#define AES_TRIGGER_S  0

/** AES_STATE_REG register
 *  Operation status register
 */
#define AES_STATE_REG (DR_REG_AES_BASE + 0x4c)
/** AES_STATE : RO; bitpos: [1:0]; default: 0;
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
#define AES_STATE    0x00000003U
#define AES_STATE_M  (AES_STATE_V << AES_STATE_S)
#define AES_STATE_V  0x00000003U
#define AES_STATE_S  0

/** AES_IV_MEM register
 *  The memory that stores initialization vector
 */
#define AES_IV_MEM (DR_REG_AES_BASE + 0x50)
#define AES_IV_MEM_SIZE_BYTES 16

/** AES_H_MEM register
 *  The memory that stores GCM hash subkey
 */
#define AES_H_MEM (DR_REG_AES_BASE + 0x60)
#define AES_H_MEM_SIZE_BYTES 16

/** AES_J0_MEM register
 *  The memory that stores J0
 */
#define AES_J0_MEM (DR_REG_AES_BASE + 0x70)
#define AES_J0_MEM_SIZE_BYTES 16

/** AES_T0_MEM register
 *  The memory that stores T0
 */
#define AES_T0_MEM (DR_REG_AES_BASE + 0x80)
#define AES_T0_MEM_SIZE_BYTES 16

/** AES_DMA_ENABLE_REG register
 *  Selects the working mode of the AES accelerator
 */
#define AES_DMA_ENABLE_REG (DR_REG_AES_BASE + 0x90)
/** AES_DMA_ENABLE : R/W; bitpos: [0]; default: 0;
 *  Configures the working mode of the AES accelerator. \\
 *  0: Typical AES\\
 *  1: DMA-AES\\
 */
#define AES_DMA_ENABLE    (BIT(0))
#define AES_DMA_ENABLE_M  (AES_DMA_ENABLE_V << AES_DMA_ENABLE_S)
#define AES_DMA_ENABLE_V  0x00000001U
#define AES_DMA_ENABLE_S  0

/** AES_BLOCK_MODE_REG register
 *  Defines the block cipher mode
 */
#define AES_BLOCK_MODE_REG (DR_REG_AES_BASE + 0x94)
/** AES_BLOCK_MODE : R/W; bitpos: [2:0]; default: 0;
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
#define AES_BLOCK_MODE    0x00000007U
#define AES_BLOCK_MODE_M  (AES_BLOCK_MODE_V << AES_BLOCK_MODE_S)
#define AES_BLOCK_MODE_V  0x00000007U
#define AES_BLOCK_MODE_S  0

/** AES_BLOCK_NUM_REG register
 *  Block number configuration register
 */
#define AES_BLOCK_NUM_REG (DR_REG_AES_BASE + 0x98)
/** AES_BLOCK_NUM : R/W; bitpos: [31:0]; default: 0;
 *  Represents the Block Number of plaintext or ciphertext when the AES accelerator
 *  operates under the DMA-AES working mode. For details, see Section <a
 *  href=sec:aes-block-number">link</a>. "
 */
#define AES_BLOCK_NUM    0xFFFFFFFFU
#define AES_BLOCK_NUM_M  (AES_BLOCK_NUM_V << AES_BLOCK_NUM_S)
#define AES_BLOCK_NUM_V  0xFFFFFFFFU
#define AES_BLOCK_NUM_S  0

/** AES_INC_SEL_REG register
 *  Standard incrementing function register
 */
#define AES_INC_SEL_REG (DR_REG_AES_BASE + 0x9c)
/** AES_INC_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures the Standard Incrementing Function for CTR block operation. \\
 *  0: INC<SUB>32</SUB>\\
 *  1: INC<SUB>128</SUB>\\
 */
#define AES_INC_SEL    (BIT(0))
#define AES_INC_SEL_M  (AES_INC_SEL_V << AES_INC_SEL_S)
#define AES_INC_SEL_V  0x00000001U
#define AES_INC_SEL_S  0

/** AES_AAD_BLOCK_NUM_REG register
 *  Additional Authential Data block number register
 */
#define AES_AAD_BLOCK_NUM_REG (DR_REG_AES_BASE + 0xa0)
/** AES_AAD_BLOCK_NUM : R/W; bitpos: [31:0]; default: 0;
 *  Those bits stores the number of AAD block.
 */
#define AES_AAD_BLOCK_NUM    0xFFFFFFFFU
#define AES_AAD_BLOCK_NUM_M  (AES_AAD_BLOCK_NUM_V << AES_AAD_BLOCK_NUM_S)
#define AES_AAD_BLOCK_NUM_V  0xFFFFFFFFU
#define AES_AAD_BLOCK_NUM_S  0

/** AES_REMAINDER_BIT_NUM_REG register
 *  AES remainder bit number register
 */
#define AES_REMAINDER_BIT_NUM_REG (DR_REG_AES_BASE + 0xa4)
/** AES_REMAINDER_BIT_NUM : R/W; bitpos: [6:0]; default: 0;
 *  Those bits stores the number of remainder bit.
 */
#define AES_REMAINDER_BIT_NUM    0x0000007FU
#define AES_REMAINDER_BIT_NUM_M  (AES_REMAINDER_BIT_NUM_V << AES_REMAINDER_BIT_NUM_S)
#define AES_REMAINDER_BIT_NUM_V  0x0000007FU
#define AES_REMAINDER_BIT_NUM_S  0

/** AES_CONTINUE_REG register
 *  AES continue register
 */
#define AES_CONTINUE_REG (DR_REG_AES_BASE + 0xa8)
/** AES_CONTINUE : WT; bitpos: [0]; default: 0;
 *  Set this bit to continue GCM operation.
 */
#define AES_CONTINUE    (BIT(0))
#define AES_CONTINUE_M  (AES_CONTINUE_V << AES_CONTINUE_S)
#define AES_CONTINUE_V  0x00000001U
#define AES_CONTINUE_S  0

/** AES_INT_CLEAR_REG register
 *  DMA-AES interrupt clear register
 */
#define AES_INT_CLEAR_REG (DR_REG_AES_BASE + 0xac)
/** AES_INT_CLEAR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear AES interrupt. \\
 *  0: No effect \\
 *  1: Clear \\
 */
#define AES_INT_CLEAR    (BIT(0))
#define AES_INT_CLEAR_M  (AES_INT_CLEAR_V << AES_INT_CLEAR_S)
#define AES_INT_CLEAR_V  0x00000001U
#define AES_INT_CLEAR_S  0

/** AES_INT_ENA_REG register
 *  DMA-AES interrupt enable register
 */
#define AES_INT_ENA_REG (DR_REG_AES_BASE + 0xb0)
/** AES_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable AES interrupt.\\
 *  0: Disable\\
 *  1: Enable \\
 */
#define AES_INT_ENA    (BIT(0))
#define AES_INT_ENA_M  (AES_INT_ENA_V << AES_INT_ENA_S)
#define AES_INT_ENA_V  0x00000001U
#define AES_INT_ENA_S  0

/** AES_DATE_REG register
 *  AES version control register
 */
#define AES_DATE_REG (DR_REG_AES_BASE + 0xb4)
/** AES_DATE : R/W; bitpos: [27:0]; default: 36774000;
 *  This bits stores the version information of AES.
 */
#define AES_DATE    0x0FFFFFFFU
#define AES_DATE_M  (AES_DATE_V << AES_DATE_S)
#define AES_DATE_V  0x0FFFFFFFU
#define AES_DATE_S  0

/** AES_DMA_EXIT_REG register
 *  Operation exit controlling register
 */
#define AES_DMA_EXIT_REG (DR_REG_AES_BASE + 0xb8)
/** AES_DMA_EXIT : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to exit AES operation. \\
 *  0: No effect\\
 *  1: Exit\\
 *  Only valid for DMA-AES operation.
 */
#define AES_DMA_EXIT    (BIT(0))
#define AES_DMA_EXIT_M  (AES_DMA_EXIT_V << AES_DMA_EXIT_S)
#define AES_DMA_EXIT_V  0x00000001U
#define AES_DMA_EXIT_S  0

/** AES_RX_RESET_REG register
 *  AES-DMA reset rx-fifo register
 */
#define AES_RX_RESET_REG (DR_REG_AES_BASE + 0xc0)
/** AES_RX_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset rx_fifo under dma_aes working mode.
 */
#define AES_RX_RESET    (BIT(0))
#define AES_RX_RESET_M  (AES_RX_RESET_V << AES_RX_RESET_S)
#define AES_RX_RESET_V  0x00000001U
#define AES_RX_RESET_S  0

/** AES_TX_RESET_REG register
 *  AES-DMA reset tx-fifo register
 */
#define AES_TX_RESET_REG (DR_REG_AES_BASE + 0xc4)
/** AES_TX_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset tx_fifo under dma_aes working mode.
 */
#define AES_TX_RESET    (BIT(0))
#define AES_TX_RESET_M  (AES_TX_RESET_V << AES_TX_RESET_S)
#define AES_TX_RESET_V  0x00000001U
#define AES_TX_RESET_S  0

/** AES_PSEUDO_REG register
 *  AES PSEUDO function configure register
 */
#define AES_PSEUDO_REG (DR_REG_AES_BASE + 0xd0)
/** AES_PSEUDO_EN : R/W; bitpos: [0]; default: 0;
 *  This bit decides whether the pseudo round function is enable or not.
 */
#define AES_PSEUDO_EN    (BIT(0))
#define AES_PSEUDO_EN_M  (AES_PSEUDO_EN_V << AES_PSEUDO_EN_S)
#define AES_PSEUDO_EN_V  0x00000001U
#define AES_PSEUDO_EN_S  0
/** AES_PSEUDO_BASE : R/W; bitpos: [4:1]; default: 2;
 *  Those bits decides the basic number of pseudo round number.
 */
#define AES_PSEUDO_BASE    0x0000000FU
#define AES_PSEUDO_BASE_M  (AES_PSEUDO_BASE_V << AES_PSEUDO_BASE_S)
#define AES_PSEUDO_BASE_V  0x0000000FU
#define AES_PSEUDO_BASE_S  1
/** AES_PSEUDO_INC : R/W; bitpos: [6:5]; default: 2;
 *  Those bits decides the increment number of pseudo round number
 */
#define AES_PSEUDO_INC    0x00000003U
#define AES_PSEUDO_INC_M  (AES_PSEUDO_INC_V << AES_PSEUDO_INC_S)
#define AES_PSEUDO_INC_V  0x00000003U
#define AES_PSEUDO_INC_S  5
/** AES_PSEUDO_RNG_CNT : R/W; bitpos: [9:7]; default: 7;
 *  Those bits decides the update frequency of the pseudo-key.
 */
#define AES_PSEUDO_RNG_CNT    0x00000007U
#define AES_PSEUDO_RNG_CNT_M  (AES_PSEUDO_RNG_CNT_V << AES_PSEUDO_RNG_CNT_S)
#define AES_PSEUDO_RNG_CNT_V  0x00000007U
#define AES_PSEUDO_RNG_CNT_S  7

#ifdef __cplusplus
}
#endif
