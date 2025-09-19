/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HMAC_SET_START_REG register
 *  Process control register 0.
 */
#define HMAC_SET_START_REG (DR_REG_HMAC_BASE + 0x40)
/** HMAC_SET_START : WS; bitpos: [0]; default: 0;
 *  Start hmac operation.
 */
#define HMAC_SET_START    (BIT(0))
#define HMAC_SET_START_M  (HMAC_SET_START_V << HMAC_SET_START_S)
#define HMAC_SET_START_V  0x00000001U
#define HMAC_SET_START_S  0

/** HMAC_SET_PARA_PURPOSE_REG register
 *  Configure purpose.
 */
#define HMAC_SET_PARA_PURPOSE_REG (DR_REG_HMAC_BASE + 0x44)
/** HMAC_PURPOSE_SET : WO; bitpos: [3:0]; default: 0;
 *  Set hmac parameter purpose.
 */
#define HMAC_PURPOSE_SET    0x0000000FU
#define HMAC_PURPOSE_SET_M  (HMAC_PURPOSE_SET_V << HMAC_PURPOSE_SET_S)
#define HMAC_PURPOSE_SET_V  0x0000000FU
#define HMAC_PURPOSE_SET_S  0

/** HMAC_SET_PARA_KEY_REG register
 *  Configure key.
 */
#define HMAC_SET_PARA_KEY_REG (DR_REG_HMAC_BASE + 0x48)
/** HMAC_KEY_SET : WO; bitpos: [2:0]; default: 0;
 *  Set hmac parameter key.
 */
#define HMAC_KEY_SET    0x00000007U
#define HMAC_KEY_SET_M  (HMAC_KEY_SET_V << HMAC_KEY_SET_S)
#define HMAC_KEY_SET_V  0x00000007U
#define HMAC_KEY_SET_S  0

/** HMAC_SET_PARA_FINISH_REG register
 *  Finish initial configuration.
 */
#define HMAC_SET_PARA_FINISH_REG (DR_REG_HMAC_BASE + 0x4c)
/** HMAC_SET_PARA_END : WS; bitpos: [0]; default: 0;
 *  Finish hmac configuration.
 */
#define HMAC_SET_PARA_END    (BIT(0))
#define HMAC_SET_PARA_END_M  (HMAC_SET_PARA_END_V << HMAC_SET_PARA_END_S)
#define HMAC_SET_PARA_END_V  0x00000001U
#define HMAC_SET_PARA_END_S  0

/** HMAC_SET_MESSAGE_ONE_REG register
 *  Process control register 1.
 */
#define HMAC_SET_MESSAGE_ONE_REG (DR_REG_HMAC_BASE + 0x50)
/** HMAC_SET_TEXT_ONE : WS; bitpos: [0]; default: 0;
 *  Call SHA to calculate one message block.
 */
#define HMAC_SET_TEXT_ONE    (BIT(0))
#define HMAC_SET_TEXT_ONE_M  (HMAC_SET_TEXT_ONE_V << HMAC_SET_TEXT_ONE_S)
#define HMAC_SET_TEXT_ONE_V  0x00000001U
#define HMAC_SET_TEXT_ONE_S  0

/** HMAC_SET_MESSAGE_ING_REG register
 *  Process control register 2.
 */
#define HMAC_SET_MESSAGE_ING_REG (DR_REG_HMAC_BASE + 0x54)
/** HMAC_SET_TEXT_ING : WS; bitpos: [0]; default: 0;
 *  Continue typical hmac.
 */
#define HMAC_SET_TEXT_ING    (BIT(0))
#define HMAC_SET_TEXT_ING_M  (HMAC_SET_TEXT_ING_V << HMAC_SET_TEXT_ING_S)
#define HMAC_SET_TEXT_ING_V  0x00000001U
#define HMAC_SET_TEXT_ING_S  0

/** HMAC_SET_MESSAGE_END_REG register
 *  Process control register 3.
 */
#define HMAC_SET_MESSAGE_END_REG (DR_REG_HMAC_BASE + 0x58)
/** HMAC_SET_TEXT_END : WS; bitpos: [0]; default: 0;
 *  Start hardware padding.
 */
#define HMAC_SET_TEXT_END    (BIT(0))
#define HMAC_SET_TEXT_END_M  (HMAC_SET_TEXT_END_V << HMAC_SET_TEXT_END_S)
#define HMAC_SET_TEXT_END_V  0x00000001U
#define HMAC_SET_TEXT_END_S  0

/** HMAC_SET_RESULT_FINISH_REG register
 *  Process control register 4.
 */
#define HMAC_SET_RESULT_FINISH_REG (DR_REG_HMAC_BASE + 0x5c)
/** HMAC_SET_RESULT_END : WS; bitpos: [0]; default: 0;
 *  After read result from upstream, then let hmac back to idle.
 */
#define HMAC_SET_RESULT_END    (BIT(0))
#define HMAC_SET_RESULT_END_M  (HMAC_SET_RESULT_END_V << HMAC_SET_RESULT_END_S)
#define HMAC_SET_RESULT_END_V  0x00000001U
#define HMAC_SET_RESULT_END_S  0

/** HMAC_SET_INVALIDATE_JTAG_REG register
 *  Invalidate register 0.
 */
#define HMAC_SET_INVALIDATE_JTAG_REG (DR_REG_HMAC_BASE + 0x60)
/** HMAC_SET_INVALIDATE_JTAG : WS; bitpos: [0]; default: 0;
 *  Clear result from hmac downstream JTAG.
 */
#define HMAC_SET_INVALIDATE_JTAG    (BIT(0))
#define HMAC_SET_INVALIDATE_JTAG_M  (HMAC_SET_INVALIDATE_JTAG_V << HMAC_SET_INVALIDATE_JTAG_S)
#define HMAC_SET_INVALIDATE_JTAG_V  0x00000001U
#define HMAC_SET_INVALIDATE_JTAG_S  0

/** HMAC_SET_INVALIDATE_DS_REG register
 *  Invalidate register 1.
 */
#define HMAC_SET_INVALIDATE_DS_REG (DR_REG_HMAC_BASE + 0x64)
/** HMAC_SET_INVALIDATE_DS : WS; bitpos: [0]; default: 0;
 *  Clear result from hmac downstream DS.
 */
#define HMAC_SET_INVALIDATE_DS    (BIT(0))
#define HMAC_SET_INVALIDATE_DS_M  (HMAC_SET_INVALIDATE_DS_V << HMAC_SET_INVALIDATE_DS_S)
#define HMAC_SET_INVALIDATE_DS_V  0x00000001U
#define HMAC_SET_INVALIDATE_DS_S  0

/** HMAC_QUERY_ERROR_REG register
 *  Error register.
 */
#define HMAC_QUERY_ERROR_REG (DR_REG_HMAC_BASE + 0x68)
/** HMAC_QUREY_CHECK : RO; bitpos: [0]; default: 0;
 *  Hmac configuration state. 0: key are agree with purpose. 1: error
 */
#define HMAC_QUREY_CHECK    (BIT(0))
#define HMAC_QUREY_CHECK_M  (HMAC_QUREY_CHECK_V << HMAC_QUREY_CHECK_S)
#define HMAC_QUREY_CHECK_V  0x00000001U
#define HMAC_QUREY_CHECK_S  0

/** HMAC_QUERY_BUSY_REG register
 *  Busy register.
 */
#define HMAC_QUERY_BUSY_REG (DR_REG_HMAC_BASE + 0x6c)
/** HMAC_BUSY_STATE : RO; bitpos: [0]; default: 0;
 *  Hmac state. 1'b0: idle. 1'b1: busy
 */
#define HMAC_BUSY_STATE    (BIT(0))
#define HMAC_BUSY_STATE_M  (HMAC_BUSY_STATE_V << HMAC_BUSY_STATE_S)
#define HMAC_BUSY_STATE_V  0x00000001U
#define HMAC_BUSY_STATE_S  0

/** HMAC_WR_MESSAGE_MEM register
 *  Message block memory.
 */
#define HMAC_WR_MESSAGE_MEM (DR_REG_HMAC_BASE + 0x80)
#define HMAC_WR_MESSAGE_MEM_SIZE_BYTES 64

/** HMAC_RD_RESULT_MEM register
 *  Result from upstream.
 */
#define HMAC_RD_RESULT_MEM (DR_REG_HMAC_BASE + 0xc0)
#define HMAC_RD_RESULT_MEM_SIZE_BYTES 32

/** HMAC_SET_MESSAGE_PAD_REG register
 *  Process control register 5.
 */
#define HMAC_SET_MESSAGE_PAD_REG (DR_REG_HMAC_BASE + 0xf0)
/** HMAC_SET_TEXT_PAD : WO; bitpos: [0]; default: 0;
 *  Start software padding.
 */
#define HMAC_SET_TEXT_PAD    (BIT(0))
#define HMAC_SET_TEXT_PAD_M  (HMAC_SET_TEXT_PAD_V << HMAC_SET_TEXT_PAD_S)
#define HMAC_SET_TEXT_PAD_V  0x00000001U
#define HMAC_SET_TEXT_PAD_S  0

/** HMAC_ONE_BLOCK_REG register
 *  Process control register 6.
 */
#define HMAC_ONE_BLOCK_REG (DR_REG_HMAC_BASE + 0xf4)
/** HMAC_SET_ONE_BLOCK : WS; bitpos: [0]; default: 0;
 *  Don't have to do padding.
 */
#define HMAC_SET_ONE_BLOCK    (BIT(0))
#define HMAC_SET_ONE_BLOCK_M  (HMAC_SET_ONE_BLOCK_V << HMAC_SET_ONE_BLOCK_S)
#define HMAC_SET_ONE_BLOCK_V  0x00000001U
#define HMAC_SET_ONE_BLOCK_S  0

/** HMAC_SOFT_JTAG_CTRL_REG register
 *  Jtag register 0.
 */
#define HMAC_SOFT_JTAG_CTRL_REG (DR_REG_HMAC_BASE + 0xf8)
/** HMAC_SOFT_JTAG_CTRL : WS; bitpos: [0]; default: 0;
 *  Turn on JTAG verification.
 */
#define HMAC_SOFT_JTAG_CTRL    (BIT(0))
#define HMAC_SOFT_JTAG_CTRL_M  (HMAC_SOFT_JTAG_CTRL_V << HMAC_SOFT_JTAG_CTRL_S)
#define HMAC_SOFT_JTAG_CTRL_V  0x00000001U
#define HMAC_SOFT_JTAG_CTRL_S  0

/** HMAC_WR_JTAG_REG register
 *  Jtag register 1.
 */
#define HMAC_WR_JTAG_REG (DR_REG_HMAC_BASE + 0xfc)
/** HMAC_WR_JTAG : WO; bitpos: [31:0]; default: 0;
 *  32-bit of key to be compared.
 */
#define HMAC_WR_JTAG    0xFFFFFFFFU
#define HMAC_WR_JTAG_M  (HMAC_WR_JTAG_V << HMAC_WR_JTAG_S)
#define HMAC_WR_JTAG_V  0xFFFFFFFFU
#define HMAC_WR_JTAG_S  0

/** HMAC_DATE_REG register
 *  Date register.
 */
#define HMAC_DATE_REG (DR_REG_HMAC_BASE + 0x1fc)
/** HMAC_DATE : R/W; bitpos: [29:0]; default: 538969624;
 *  Hmac date information/ hmac version information.
 */
#define HMAC_DATE    0x3FFFFFFFU
#define HMAC_DATE_M  (HMAC_DATE_V << HMAC_DATE_S)
#define HMAC_DATE_V  0x3FFFFFFFU
#define HMAC_DATE_S  0

#ifdef __cplusplus
}
#endif
