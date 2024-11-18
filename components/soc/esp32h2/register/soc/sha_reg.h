/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** SHA_MODE_REG register
 *  Initial configuration register.
 */
#define SHA_MODE_REG (DR_REG_SHA_BASE + 0x0)
/** SHA_MODE : R/W; bitpos: [2:0]; default: 0;
 *  Sha mode.
 */
#define SHA_MODE    0x00000007U
#define SHA_MODE_M  (SHA_MODE_V << SHA_MODE_S)
#define SHA_MODE_V  0x00000007U
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
/** SHA_T_LENGTH : R/W; bitpos: [5:0]; default: 0;
 *  Sha t_length (used if and only if mode == SHA_512/t).
 */
#define SHA_T_LENGTH    0x0000003FU
#define SHA_T_LENGTH_M  (SHA_T_LENGTH_V << SHA_T_LENGTH_S)
#define SHA_T_LENGTH_V  0x0000003FU
#define SHA_T_LENGTH_S  0

/** SHA_DMA_BLOCK_NUM_REG register
 *  DMA configuration register 0.
 */
#define SHA_DMA_BLOCK_NUM_REG (DR_REG_SHA_BASE + 0xc)
/** SHA_DMA_BLOCK_NUM : R/W; bitpos: [5:0]; default: 0;
 *  Dma-sha block number.
 */
#define SHA_DMA_BLOCK_NUM    0x0000003FU
#define SHA_DMA_BLOCK_NUM_M  (SHA_DMA_BLOCK_NUM_V << SHA_DMA_BLOCK_NUM_S)
#define SHA_DMA_BLOCK_NUM_V  0x0000003FU
#define SHA_DMA_BLOCK_NUM_S  0

/** SHA_START_REG register
 *  Typical SHA configuration register 0.
 */
#define SHA_START_REG (DR_REG_SHA_BASE + 0x10)
/** SHA_START : RO; bitpos: [31:1]; default: 0;
 *  Reserved.
 */
#define SHA_START    0x7FFFFFFFU
#define SHA_START_M  (SHA_START_V << SHA_START_S)
#define SHA_START_V  0x7FFFFFFFU
#define SHA_START_S  1

/** SHA_CONTINUE_REG register
 *  Typical SHA configuration register 1.
 */
#define SHA_CONTINUE_REG (DR_REG_SHA_BASE + 0x14)
/** SHA_CONTINUE : RO; bitpos: [31:1]; default: 0;
 *  Reserved.
 */
#define SHA_CONTINUE    0x7FFFFFFFU
#define SHA_CONTINUE_M  (SHA_CONTINUE_V << SHA_CONTINUE_S)
#define SHA_CONTINUE_V  0x7FFFFFFFU
#define SHA_CONTINUE_S  1

/** SHA_BUSY_REG register
 *  Busy register.
 */
#define SHA_BUSY_REG (DR_REG_SHA_BASE + 0x18)
/** SHA_BUSY_STATE : RO; bitpos: [0]; default: 0;
 *  Sha busy state. 1'b0: idle. 1'b1: busy.
 */
#define SHA_BUSY_STATE    (BIT(0))
#define SHA_BUSY_STATE_M  (SHA_BUSY_STATE_V << SHA_BUSY_STATE_S)
#define SHA_BUSY_STATE_V  0x00000001U
#define SHA_BUSY_STATE_S  0

/** SHA_DMA_START_REG register
 *  DMA configuration register 1.
 */
#define SHA_DMA_START_REG (DR_REG_SHA_BASE + 0x1c)
/** SHA_DMA_START : WO; bitpos: [0]; default: 0;
 *  Start dma-sha.
 */
#define SHA_DMA_START    (BIT(0))
#define SHA_DMA_START_M  (SHA_DMA_START_V << SHA_DMA_START_S)
#define SHA_DMA_START_V  0x00000001U
#define SHA_DMA_START_S  0

/** SHA_DMA_CONTINUE_REG register
 *  DMA configuration register 2.
 */
#define SHA_DMA_CONTINUE_REG (DR_REG_SHA_BASE + 0x20)
/** SHA_DMA_CONTINUE : WO; bitpos: [0]; default: 0;
 *  Continue dma-sha.
 */
#define SHA_DMA_CONTINUE    (BIT(0))
#define SHA_DMA_CONTINUE_M  (SHA_DMA_CONTINUE_V << SHA_DMA_CONTINUE_S)
#define SHA_DMA_CONTINUE_V  0x00000001U
#define SHA_DMA_CONTINUE_S  0

/** SHA_CLEAR_IRQ_REG register
 *  Interrupt clear register.
 */
#define SHA_CLEAR_IRQ_REG (DR_REG_SHA_BASE + 0x24)
/** SHA_CLEAR_INTERRUPT : WO; bitpos: [0]; default: 0;
 *  Clear sha interrupt.
 */
#define SHA_CLEAR_INTERRUPT    (BIT(0))
#define SHA_CLEAR_INTERRUPT_M  (SHA_CLEAR_INTERRUPT_V << SHA_CLEAR_INTERRUPT_S)
#define SHA_CLEAR_INTERRUPT_V  0x00000001U
#define SHA_CLEAR_INTERRUPT_S  0

/** SHA_IRQ_ENA_REG register
 *  Interrupt enable register.
 */
#define SHA_IRQ_ENA_REG (DR_REG_SHA_BASE + 0x28)
/** SHA_INTERRUPT_ENA : R/W; bitpos: [0]; default: 0;
 *  Sha interrupt enable register. 1'b0: disable(default). 1'b1: enable.
 */
#define SHA_INTERRUPT_ENA    (BIT(0))
#define SHA_INTERRUPT_ENA_M  (SHA_INTERRUPT_ENA_V << SHA_INTERRUPT_ENA_S)
#define SHA_INTERRUPT_ENA_V  0x00000001U
#define SHA_INTERRUPT_ENA_S  0

/** SHA_DATE_REG register
 *  Date register.
 */
#define SHA_DATE_REG (DR_REG_SHA_BASE + 0x2c)
/** SHA_DATE : R/W; bitpos: [29:0]; default: 538972713;
 *  Sha date information/ sha version information.
 */
#define SHA_DATE    0x3FFFFFFFU
#define SHA_DATE_M  (SHA_DATE_V << SHA_DATE_S)
#define SHA_DATE_V  0x3FFFFFFFU
#define SHA_DATE_S  0

/** SHA_H_MEM register
 *  Sha H memory which contains intermediate hash or final hash.
 */
#define SHA_H_MEM_REG (DR_REG_SHA_BASE + 0x40)
#define SHA_H_MEM_SIZE_BYTES 64

/** SHA_M_MEM register
 *  Sha M memory which contains message.
 */
#define SHA_M_MEM_REG (DR_REG_SHA_BASE + 0x80)
#define SHA_M_MEM_SIZE_BYTES 64

#ifdef __cplusplus
}
#endif
