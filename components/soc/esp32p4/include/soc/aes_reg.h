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
 *  Key material key_0 configure register
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
 *  Key material key_1 configure register
 */
#define AES_KEY_1_REG (DR_REG_AES_BASE + 0x4)
/** AES_KEY_1 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_1 that is a part of key material.
 */
#define AES_KEY_1    0xFFFFFFFFU
#define AES_KEY_1_M  (AES_KEY_1_V << AES_KEY_1_S)
#define AES_KEY_1_V  0xFFFFFFFFU
#define AES_KEY_1_S  0

/** AES_KEY_2_REG register
 *  Key material key_2 configure register
 */
#define AES_KEY_2_REG (DR_REG_AES_BASE + 0x8)
/** AES_KEY_2 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_2 that is a part of key material.
 */
#define AES_KEY_2    0xFFFFFFFFU
#define AES_KEY_2_M  (AES_KEY_2_V << AES_KEY_2_S)
#define AES_KEY_2_V  0xFFFFFFFFU
#define AES_KEY_2_S  0

/** AES_KEY_3_REG register
 *  Key material key_3 configure register
 */
#define AES_KEY_3_REG (DR_REG_AES_BASE + 0xc)
/** AES_KEY_3 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_3 that is a part of key material.
 */
#define AES_KEY_3    0xFFFFFFFFU
#define AES_KEY_3_M  (AES_KEY_3_V << AES_KEY_3_S)
#define AES_KEY_3_V  0xFFFFFFFFU
#define AES_KEY_3_S  0

/** AES_KEY_4_REG register
 *  Key material key_4 configure register
 */
#define AES_KEY_4_REG (DR_REG_AES_BASE + 0x10)
/** AES_KEY_4 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_4 that is a part of key material.
 */
#define AES_KEY_4    0xFFFFFFFFU
#define AES_KEY_4_M  (AES_KEY_4_V << AES_KEY_4_S)
#define AES_KEY_4_V  0xFFFFFFFFU
#define AES_KEY_4_S  0

/** AES_KEY_5_REG register
 *  Key material key_5 configure register
 */
#define AES_KEY_5_REG (DR_REG_AES_BASE + 0x14)
/** AES_KEY_5 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_5 that is a part of key material.
 */
#define AES_KEY_5    0xFFFFFFFFU
#define AES_KEY_5_M  (AES_KEY_5_V << AES_KEY_5_S)
#define AES_KEY_5_V  0xFFFFFFFFU
#define AES_KEY_5_S  0

/** AES_KEY_6_REG register
 *  Key material key_6 configure register
 */
#define AES_KEY_6_REG (DR_REG_AES_BASE + 0x18)
/** AES_KEY_6 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_6 that is a part of key material.
 */
#define AES_KEY_6    0xFFFFFFFFU
#define AES_KEY_6_M  (AES_KEY_6_V << AES_KEY_6_S)
#define AES_KEY_6_V  0xFFFFFFFFU
#define AES_KEY_6_S  0

/** AES_KEY_7_REG register
 *  Key material key_7 configure register
 */
#define AES_KEY_7_REG (DR_REG_AES_BASE + 0x1c)
/** AES_KEY_7 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores key_7 that is a part of key material.
 */
#define AES_KEY_7    0xFFFFFFFFU
#define AES_KEY_7_M  (AES_KEY_7_V << AES_KEY_7_S)
#define AES_KEY_7_V  0xFFFFFFFFU
#define AES_KEY_7_S  0

/** AES_TEXT_IN_0_REG register
 *  source text material text_in_0 configure register
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
 *  source text material text_in_1 configure register
 */
#define AES_TEXT_IN_1_REG (DR_REG_AES_BASE + 0x24)
/** AES_TEXT_IN_1 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_in_1 that is a part of source text material.
 */
#define AES_TEXT_IN_1    0xFFFFFFFFU
#define AES_TEXT_IN_1_M  (AES_TEXT_IN_1_V << AES_TEXT_IN_1_S)
#define AES_TEXT_IN_1_V  0xFFFFFFFFU
#define AES_TEXT_IN_1_S  0

/** AES_TEXT_IN_2_REG register
 *  source text material text_in_2 configure register
 */
#define AES_TEXT_IN_2_REG (DR_REG_AES_BASE + 0x28)
/** AES_TEXT_IN_2 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_in_2 that is a part of source text material.
 */
#define AES_TEXT_IN_2    0xFFFFFFFFU
#define AES_TEXT_IN_2_M  (AES_TEXT_IN_2_V << AES_TEXT_IN_2_S)
#define AES_TEXT_IN_2_V  0xFFFFFFFFU
#define AES_TEXT_IN_2_S  0

/** AES_TEXT_IN_3_REG register
 *  source text material text_in_3 configure register
 */
#define AES_TEXT_IN_3_REG (DR_REG_AES_BASE + 0x2c)
/** AES_TEXT_IN_3 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_in_3 that is a part of source text material.
 */
#define AES_TEXT_IN_3    0xFFFFFFFFU
#define AES_TEXT_IN_3_M  (AES_TEXT_IN_3_V << AES_TEXT_IN_3_S)
#define AES_TEXT_IN_3_V  0xFFFFFFFFU
#define AES_TEXT_IN_3_S  0

/** AES_TEXT_OUT_0_REG register
 *  result text material text_out_0 configure register
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
 *  result text material text_out_1 configure register
 */
#define AES_TEXT_OUT_1_REG (DR_REG_AES_BASE + 0x34)
/** AES_TEXT_OUT_1 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_out_1 that is a part of result text material.
 */
#define AES_TEXT_OUT_1    0xFFFFFFFFU
#define AES_TEXT_OUT_1_M  (AES_TEXT_OUT_1_V << AES_TEXT_OUT_1_S)
#define AES_TEXT_OUT_1_V  0xFFFFFFFFU
#define AES_TEXT_OUT_1_S  0

/** AES_TEXT_OUT_2_REG register
 *  result text material text_out_2 configure register
 */
#define AES_TEXT_OUT_2_REG (DR_REG_AES_BASE + 0x38)
/** AES_TEXT_OUT_2 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_out_2 that is a part of result text material.
 */
#define AES_TEXT_OUT_2    0xFFFFFFFFU
#define AES_TEXT_OUT_2_M  (AES_TEXT_OUT_2_V << AES_TEXT_OUT_2_S)
#define AES_TEXT_OUT_2_V  0xFFFFFFFFU
#define AES_TEXT_OUT_2_S  0

/** AES_TEXT_OUT_3_REG register
 *  result text material text_out_3 configure register
 */
#define AES_TEXT_OUT_3_REG (DR_REG_AES_BASE + 0x3c)
/** AES_TEXT_OUT_3 : R/W; bitpos: [31:0]; default: 0;
 *  This bits stores text_out_3 that is a part of result text material.
 */
#define AES_TEXT_OUT_3    0xFFFFFFFFU
#define AES_TEXT_OUT_3_M  (AES_TEXT_OUT_3_V << AES_TEXT_OUT_3_S)
#define AES_TEXT_OUT_3_V  0xFFFFFFFFU
#define AES_TEXT_OUT_3_S  0

/** AES_MODE_REG register
 *  AES Mode register
 */
#define AES_MODE_REG (DR_REG_AES_BASE + 0x40)
/** AES_MODE : R/W; bitpos: [2:0]; default: 0;
 *  This bits decides which one operation mode will be used. 3'd0: AES-EN-128, 3'd1:
 *  AES-EN-192, 3'd2: AES-EN-256, 3'd4: AES-DE-128, 3'd5: AES-DE-192, 3'd6: AES-DE-256.
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
 *  AES trigger register
 */
#define AES_TRIGGER_REG (DR_REG_AES_BASE + 0x48)
/** AES_TRIGGER : WT; bitpos: [0]; default: 0;
 *  Set this bit to start AES calculation.
 */
#define AES_TRIGGER    (BIT(0))
#define AES_TRIGGER_M  (AES_TRIGGER_V << AES_TRIGGER_S)
#define AES_TRIGGER_V  0x00000001U
#define AES_TRIGGER_S  0

/** AES_STATE_REG register
 *  AES state register
 */
#define AES_STATE_REG (DR_REG_AES_BASE + 0x4c)
/** AES_STATE : RO; bitpos: [1:0]; default: 0;
 *  Those bits shows AES status. For typical AES, 0: idle, 1: busy. For DMA-AES, 0:
 *  idle, 1: busy, 2: calculation_done.
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
 *  DMA-AES working mode register
 */
#define AES_DMA_ENABLE_REG (DR_REG_AES_BASE + 0x90)
/** AES_DMA_ENABLE : R/W; bitpos: [0]; default: 0;
 *  1'b0: typical AES working mode, 1'b1: DMA-AES working mode.
 */
#define AES_DMA_ENABLE    (BIT(0))
#define AES_DMA_ENABLE_M  (AES_DMA_ENABLE_V << AES_DMA_ENABLE_S)
#define AES_DMA_ENABLE_V  0x00000001U
#define AES_DMA_ENABLE_S  0

/** AES_BLOCK_MODE_REG register
 *  AES cipher block mode register
 */
#define AES_BLOCK_MODE_REG (DR_REG_AES_BASE + 0x94)
/** AES_BLOCK_MODE : R/W; bitpos: [2:0]; default: 0;
 *  Those bits decides which block mode will be used. 0x0: ECB, 0x1: CBC, 0x2: OFB,
 *  0x3: CTR, 0x4: CFB-8, 0x5: CFB-128, 0x6: GCM, 0x7: reserved.
 */
#define AES_BLOCK_MODE    0x00000007U
#define AES_BLOCK_MODE_M  (AES_BLOCK_MODE_V << AES_BLOCK_MODE_S)
#define AES_BLOCK_MODE_V  0x00000007U
#define AES_BLOCK_MODE_S  0

/** AES_BLOCK_NUM_REG register
 *  AES block number register
 */
#define AES_BLOCK_NUM_REG (DR_REG_AES_BASE + 0x98)
/** AES_BLOCK_NUM : R/W; bitpos: [31:0]; default: 0;
 *  Those bits stores the number of Plaintext/ciphertext block.
 */
#define AES_BLOCK_NUM    0xFFFFFFFFU
#define AES_BLOCK_NUM_M  (AES_BLOCK_NUM_V << AES_BLOCK_NUM_S)
#define AES_BLOCK_NUM_V  0xFFFFFFFFU
#define AES_BLOCK_NUM_S  0

/** AES_INC_SEL_REG register
 *  Standard incrementing function configure register
 */
#define AES_INC_SEL_REG (DR_REG_AES_BASE + 0x9c)
/** AES_INC_SEL : R/W; bitpos: [0]; default: 0;
 *  This bit decides the standard incrementing function. 0: INC32. 1: INC128.
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
 *  AES Interrupt clear register
 */
#define AES_INT_CLEAR_REG (DR_REG_AES_BASE + 0xac)
/** AES_INT_CLEAR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the AES interrupt.
 */
#define AES_INT_CLEAR    (BIT(0))
#define AES_INT_CLEAR_M  (AES_INT_CLEAR_V << AES_INT_CLEAR_S)
#define AES_INT_CLEAR_V  0x00000001U
#define AES_INT_CLEAR_S  0

/** AES_INT_ENA_REG register
 *  AES Interrupt enable register
 */
#define AES_INT_ENA_REG (DR_REG_AES_BASE + 0xb0)
/** AES_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable interrupt that occurs when DMA-AES calculation is done.
 */
#define AES_INT_ENA    (BIT(0))
#define AES_INT_ENA_M  (AES_INT_ENA_V << AES_INT_ENA_S)
#define AES_INT_ENA_V  0x00000001U
#define AES_INT_ENA_S  0

/** AES_DATE_REG register
 *  AES version control register
 */
#define AES_DATE_REG (DR_REG_AES_BASE + 0xb4)
/** AES_DATE : R/W; bitpos: [29:0]; default: 538513936;
 *  This bits stores the version information of AES.
 */
#define AES_DATE    0x3FFFFFFFU
#define AES_DATE_M  (AES_DATE_V << AES_DATE_S)
#define AES_DATE_V  0x3FFFFFFFU
#define AES_DATE_S  0

/** AES_DMA_EXIT_REG register
 *  AES-DMA exit config
 */
#define AES_DMA_EXIT_REG (DR_REG_AES_BASE + 0xb8)
/** AES_DMA_EXIT : WT; bitpos: [0]; default: 0;
 *  Set this register to leave calculation done stage. Recommend to use it after
 *  software finishes reading DMA's output buffer.
 */
#define AES_DMA_EXIT    (BIT(0))
#define AES_DMA_EXIT_M  (AES_DMA_EXIT_V << AES_DMA_EXIT_S)
#define AES_DMA_EXIT_V  0x00000001U
#define AES_DMA_EXIT_S  0

#ifdef __cplusplus
}
#endif
