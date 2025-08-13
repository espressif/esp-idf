/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** SHA_MODE_REG register
 *  Configures SHA algorithm
 */
#define SHA_MODE_REG (DR_REG_SHA_BASE + 0x0)
/** SHA_MODE : R/W; bitpos: [3:0]; default: 2;
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
#define SHA_MODE    0x0000000FU
#define SHA_MODE_M  (SHA_MODE_V << SHA_MODE_S)
#define SHA_MODE_V  0x0000000FU
#define SHA_MODE_S  0

/** SHA_T_STRING_REG register
 *  SHA 512/t configuration register 0.
 */
#define SHA_T_STRING_REG (DR_REG_SHA_BASE + 0x4)
/** SHA_T_STRING : R/W; bitpos: [31:0]; default: 0;
 *  Sha t_string (used if and only if mode == SHA_512/t).
 */
#define SHA_T_STRING    0xFFFFFFFFU
#define SHA_T_STRING_M  (SHA_T_STRING_V << SHA_T_STRING_S)
#define SHA_T_STRING_V  0xFFFFFFFFU
#define SHA_T_STRING_S  0

/** SHA_T_LENGTH_REG register
 *  SHA 512/t configuration register 1.
 */
#define SHA_T_LENGTH_REG (DR_REG_SHA_BASE + 0x8)
/** SHA_T_LENGTH : R/W; bitpos: [6:0]; default: 0;
 *  Sha t_length (used if and only if mode == SHA_512/t).
 */
#define SHA_T_LENGTH    0x0000007FU
#define SHA_T_LENGTH_M  (SHA_T_LENGTH_V << SHA_T_LENGTH_S)
#define SHA_T_LENGTH_V  0x0000007FU
#define SHA_T_LENGTH_S  0

/** SHA_DMA_BLOCK_NUM_REG register
 *  Block number register (only effective for DMA-SHA)
 */
#define SHA_DMA_BLOCK_NUM_REG (DR_REG_SHA_BASE + 0xc)
/** SHA_DMA_BLOCK_NUM : R/W; bitpos: [15:0]; default: 0;
 *  Configures the DMA-SHA block number.
 */
#define SHA_DMA_BLOCK_NUM    0x0000FFFFU
#define SHA_DMA_BLOCK_NUM_M  (SHA_DMA_BLOCK_NUM_V << SHA_DMA_BLOCK_NUM_S)
#define SHA_DMA_BLOCK_NUM_V  0x0000FFFFU
#define SHA_DMA_BLOCK_NUM_S  0

/** SHA_START_REG register
 *  Starts the SHA accelerator for Typical SHA operation
 */
#define SHA_START_REG (DR_REG_SHA_BASE + 0x10)
/** SHA_START : WO; bitpos: [0]; default: 0;
 *  Start typical sha.
 */
#define SHA_START    (BIT(0))
#define SHA_START_M  (SHA_START_V << SHA_START_S)
#define SHA_START_V  0x00000001U
#define SHA_START_S  0

/** SHA_CONTINUE_REG register
 *  Continues SHA operation (only effective in Typical SHA mode)
 */
#define SHA_CONTINUE_REG (DR_REG_SHA_BASE + 0x14)
/** SHA_CONTINUE : WO; bitpos: [0]; default: 0;
 *  Continue typical sha.
 */
#define SHA_CONTINUE    (BIT(0))
#define SHA_CONTINUE_M  (SHA_CONTINUE_V << SHA_CONTINUE_S)
#define SHA_CONTINUE_V  0x00000001U
#define SHA_CONTINUE_S  0

/** SHA_BUSY_REG register
 *  Represents if SHA Accelerator is busy or not
 */
#define SHA_BUSY_REG (DR_REG_SHA_BASE + 0x18)
/** SHA_BUSY_STATE : RO; bitpos: [0]; default: 0;
 *  Represents the states of SHA accelerator.
 *  0: idle
 *  1: busy
 */
#define SHA_BUSY_STATE    (BIT(0))
#define SHA_BUSY_STATE_M  (SHA_BUSY_STATE_V << SHA_BUSY_STATE_S)
#define SHA_BUSY_STATE_V  0x00000001U
#define SHA_BUSY_STATE_S  0

/** SHA_DMA_START_REG register
 *  Starts the SHA accelerator for DMA-SHA operation
 */
#define SHA_DMA_START_REG (DR_REG_SHA_BASE + 0x1c)
/** SHA_DMA_START : WO; bitpos: [0]; default: 0;
 *  Write 1 to start DMA-SHA calculation.
 */
#define SHA_DMA_START    (BIT(0))
#define SHA_DMA_START_M  (SHA_DMA_START_V << SHA_DMA_START_S)
#define SHA_DMA_START_V  0x00000001U
#define SHA_DMA_START_S  0

/** SHA_DMA_CONTINUE_REG register
 *  Continues SHA operation (only effective in DMA-SHA mode)
 */
#define SHA_DMA_CONTINUE_REG (DR_REG_SHA_BASE + 0x20)
/** SHA_DMA_CONTINUE : WO; bitpos: [0]; default: 0;
 *  Write 1 to continue DMA-SHA calculation.
 */
#define SHA_DMA_CONTINUE    (BIT(0))
#define SHA_DMA_CONTINUE_M  (SHA_DMA_CONTINUE_V << SHA_DMA_CONTINUE_S)
#define SHA_DMA_CONTINUE_V  0x00000001U
#define SHA_DMA_CONTINUE_S  0

/** SHA_CLEAR_IRQ_REG register
 *  DMA-SHA interrupt clear register
 */
#define SHA_CLEAR_IRQ_REG (DR_REG_SHA_BASE + 0x24)
/** SHA_CLEAR_INTERRUPT : WO; bitpos: [0]; default: 0;
 *  Write 1 to clear DMA-SHA interrupt.
 */
#define SHA_CLEAR_INTERRUPT    (BIT(0))
#define SHA_CLEAR_INTERRUPT_M  (SHA_CLEAR_INTERRUPT_V << SHA_CLEAR_INTERRUPT_S)
#define SHA_CLEAR_INTERRUPT_V  0x00000001U
#define SHA_CLEAR_INTERRUPT_S  0

/** SHA_IRQ_ENA_REG register
 *  DMA-SHA interrupt enable register
 */
#define SHA_IRQ_ENA_REG (DR_REG_SHA_BASE + 0x28)
/** SHA_INTERRUPT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable DMA-SHA interrupt.
 */
#define SHA_INTERRUPT_ENA    (BIT(0))
#define SHA_INTERRUPT_ENA_M  (SHA_INTERRUPT_ENA_V << SHA_INTERRUPT_ENA_S)
#define SHA_INTERRUPT_ENA_V  0x00000001U
#define SHA_INTERRUPT_ENA_S  0

/** SHA_DATE_REG register
 *  Version control register
 */
#define SHA_DATE_REG (DR_REG_SHA_BASE + 0x2c)
/** SHA_DATE : R/W; bitpos: [29:0]; default: 539232291;
 *  Version control register.
 */
#define SHA_DATE    0x3FFFFFFFU
#define SHA_DATE_M  (SHA_DATE_V << SHA_DATE_S)
#define SHA_DATE_V  0x3FFFFFFFU
#define SHA_DATE_S  0

/** SHA_DMA_RX_RESET_REG register
 *  DMA RX FIFO Reset Signal
 */
#define SHA_DMA_RX_RESET_REG (DR_REG_SHA_BASE + 0x30)
/** SHA_DMA_RX_RESET : WO; bitpos: [0]; default: 0;
 *  Write 1 to reset DMA RX FIFO
 */
#define SHA_DMA_RX_RESET    (BIT(0))
#define SHA_DMA_RX_RESET_M  (SHA_DMA_RX_RESET_V << SHA_DMA_RX_RESET_S)
#define SHA_DMA_RX_RESET_V  0x00000001U
#define SHA_DMA_RX_RESET_S  0

/** SHA_2_SM_3_H_MEM register
 *  SHA1, SHA2-256, SM3 H memory which contains intermediate hash or final hash.
 *  SHA1, SHA2-256, SM3 : 0x00~0x20 (R/W)
 *  SHA2-512 : 0x00~0x40 (R/W)
 */
#define SHA_2_SM_3_H_MEM (DR_REG_SHA_BASE + 0x40)
#define SHA_2_SM_3_H_MEM_SIZE_BYTES 64

/** SHA_2_SM_3_M_MEM register
 *  SHA1, SHA2-256, SM3 M memory which contains message.
 *  SHA1, SHA2-256, SM3 : 0x00~0x40
 *  SHA2-512 : 0x00~0x80
 */
#define SHA_2_SM_3_M_MEM (DR_REG_SHA_BASE + 0x80)
#define SHA_2_SM_3_M_MEM_SIZE_BYTES 128

#ifdef __cplusplus
}
#endif
