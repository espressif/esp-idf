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

/** HMAC_SET_START_REG register
 *  HMAC start control register
 */
#define HMAC_SET_START_REG (DR_REG_HMAC_BASE + 0x40)
/** HMAC_SET_START : WS; bitpos: [0]; default: 0;
 *  Configures whether or not to enable HMAC.
 *  \\0: Disable HMAC
 *  \\1: Enable HMAC
 */
#define HMAC_SET_START    (BIT(0))
#define HMAC_SET_START_M  (HMAC_SET_START_V << HMAC_SET_START_S)
#define HMAC_SET_START_V  0x00000001U
#define HMAC_SET_START_S  0

/** HMAC_SET_PARA_PURPOSE_REG register
 *  HMAC parameter configuration register
 */
#define HMAC_SET_PARA_PURPOSE_REG (DR_REG_HMAC_BASE + 0x44)
/** HMAC_PURPOSE_SET : WO; bitpos: [3:0]; default: 0;
 *  Configures the HMAC purpose, refer to the Table <a
 *  href=tab:hmac-key-purpose">link</a>. "
 */
#define HMAC_PURPOSE_SET    0x0000000FU
#define HMAC_PURPOSE_SET_M  (HMAC_PURPOSE_SET_V << HMAC_PURPOSE_SET_S)
#define HMAC_PURPOSE_SET_V  0x0000000FU
#define HMAC_PURPOSE_SET_S  0

/** HMAC_SET_PARA_KEY_REG register
 *  HMAC parameters configuration register
 */
#define HMAC_SET_PARA_KEY_REG (DR_REG_HMAC_BASE + 0x48)
/** HMAC_KEY_SET : WO; bitpos: [2:0]; default: 0;
 *  Configures HMAC key. There are six keys with index 0~5. Write the index of the
 *  selected key to this field.
 */
#define HMAC_KEY_SET    0x00000007U
#define HMAC_KEY_SET_M  (HMAC_KEY_SET_V << HMAC_KEY_SET_S)
#define HMAC_KEY_SET_V  0x00000007U
#define HMAC_KEY_SET_S  0

/** HMAC_SET_PARA_FINISH_REG register
 *  HMAC configuration completion register
 */
#define HMAC_SET_PARA_FINISH_REG (DR_REG_HMAC_BASE + 0x4c)
/** HMAC_SET_PARA_END : WS; bitpos: [0]; default: 0;
 *  Configures whether to finish HMAC configuration.
 *  \\0: No effect
 *  \\1: Finish configuration
 */
#define HMAC_SET_PARA_END    (BIT(0))
#define HMAC_SET_PARA_END_M  (HMAC_SET_PARA_END_V << HMAC_SET_PARA_END_S)
#define HMAC_SET_PARA_END_V  0x00000001U
#define HMAC_SET_PARA_END_S  0

/** HMAC_SET_MESSAGE_ONE_REG register
 *  HMAC message control register
 */
#define HMAC_SET_MESSAGE_ONE_REG (DR_REG_HMAC_BASE + 0x50)
/** HMAC_SET_TEXT_ONE : WS; bitpos: [0]; default: 0;
 *  Calls SHA to calculate one message block.
 */
#define HMAC_SET_TEXT_ONE    (BIT(0))
#define HMAC_SET_TEXT_ONE_M  (HMAC_SET_TEXT_ONE_V << HMAC_SET_TEXT_ONE_S)
#define HMAC_SET_TEXT_ONE_V  0x00000001U
#define HMAC_SET_TEXT_ONE_S  0

/** HMAC_SET_MESSAGE_ING_REG register
 *  HMAC message continue register
 */
#define HMAC_SET_MESSAGE_ING_REG (DR_REG_HMAC_BASE + 0x54)
/** HMAC_SET_TEXT_ING : WS; bitpos: [0]; default: 0;
 *  Configures whether or not there are unprocessed message blocks.
 *  \\0: No unprocessed message block
 *  \\1: There are still some message blocks to be processed.
 */
#define HMAC_SET_TEXT_ING    (BIT(0))
#define HMAC_SET_TEXT_ING_M  (HMAC_SET_TEXT_ING_V << HMAC_SET_TEXT_ING_S)
#define HMAC_SET_TEXT_ING_V  0x00000001U
#define HMAC_SET_TEXT_ING_S  0

/** HMAC_SET_MESSAGE_END_REG register
 *  HMAC message end register
 */
#define HMAC_SET_MESSAGE_END_REG (DR_REG_HMAC_BASE + 0x58)
/** HMAC_SET_TEXT_END : WS; bitpos: [0]; default: 0;
 *  Configures whether to start hardware padding.
 *  \\0: No effect
 *  \\1: Start hardware padding
 */
#define HMAC_SET_TEXT_END    (BIT(0))
#define HMAC_SET_TEXT_END_M  (HMAC_SET_TEXT_END_V << HMAC_SET_TEXT_END_S)
#define HMAC_SET_TEXT_END_V  0x00000001U
#define HMAC_SET_TEXT_END_S  0

/** HMAC_SET_RESULT_FINISH_REG register
 *  HMAC result reading finish register
 */
#define HMAC_SET_RESULT_FINISH_REG (DR_REG_HMAC_BASE + 0x5c)
/** HMAC_SET_RESULT_END : WS; bitpos: [0]; default: 0;
 *  Configures whether to exit upstream mode and clear calculation results.
 *  \\0: Not exit
 *  \\1: Exit upstream mode and clear calculation results.
 */
#define HMAC_SET_RESULT_END    (BIT(0))
#define HMAC_SET_RESULT_END_M  (HMAC_SET_RESULT_END_V << HMAC_SET_RESULT_END_S)
#define HMAC_SET_RESULT_END_V  0x00000001U
#define HMAC_SET_RESULT_END_S  0

/** HMAC_SET_INVALIDATE_JTAG_REG register
 *  Invalidate JTAG result register
 */
#define HMAC_SET_INVALIDATE_JTAG_REG (DR_REG_HMAC_BASE + 0x60)
/** HMAC_SET_INVALIDATE_JTAG : WS; bitpos: [0]; default: 0;
 *  Configures whether or not to clear calculation results when re-enabling JTAG in
 *  downstream mode.
 *  \\0: Not clear
 *  \\1: Clear calculation results
 */
#define HMAC_SET_INVALIDATE_JTAG    (BIT(0))
#define HMAC_SET_INVALIDATE_JTAG_M  (HMAC_SET_INVALIDATE_JTAG_V << HMAC_SET_INVALIDATE_JTAG_S)
#define HMAC_SET_INVALIDATE_JTAG_V  0x00000001U
#define HMAC_SET_INVALIDATE_JTAG_S  0

/** HMAC_SET_INVALIDATE_DS_REG register
 *  Invalidate digital signature result register
 */
#define HMAC_SET_INVALIDATE_DS_REG (DR_REG_HMAC_BASE + 0x64)
/** HMAC_SET_INVALIDATE_DS : WS; bitpos: [0]; default: 0;
 *  Configures whether or not to clear calculation results of the DS module in
 *  downstream mode.
 *  \\0: Not clear
 *  \\1: Clear calculation results
 */
#define HMAC_SET_INVALIDATE_DS    (BIT(0))
#define HMAC_SET_INVALIDATE_DS_M  (HMAC_SET_INVALIDATE_DS_V << HMAC_SET_INVALIDATE_DS_S)
#define HMAC_SET_INVALIDATE_DS_V  0x00000001U
#define HMAC_SET_INVALIDATE_DS_S  0

/** HMAC_QUERY_ERROR_REG register
 *  Stores matching results between keys generated by users and corresponding purposes
 */
#define HMAC_QUERY_ERROR_REG (DR_REG_HMAC_BASE + 0x68)
/** HMAC_QUREY_CHECK : RO; bitpos: [0]; default: 0;
 *  Represents whether or not an HMAC key matches the purpose.
 *  \\0: Match
 *  \\1: Error
 */
#define HMAC_QUREY_CHECK    (BIT(0))
#define HMAC_QUREY_CHECK_M  (HMAC_QUREY_CHECK_V << HMAC_QUREY_CHECK_S)
#define HMAC_QUREY_CHECK_V  0x00000001U
#define HMAC_QUREY_CHECK_S  0

/** HMAC_QUERY_BUSY_REG register
 *  Busy state of HMAC module
 */
#define HMAC_QUERY_BUSY_REG (DR_REG_HMAC_BASE + 0x6c)
/** HMAC_BUSY_STATE : RO; bitpos: [0]; default: 0;
 *  Represents whether or not HMAC is in a busy state. Before configuring HMAC, please
 *  make sure HMAC is in an IDLE state.
 *  \\0: Idle
 *  \\1: HMAC is still working on the calculation
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
 *  Software padding register
 */
#define HMAC_SET_MESSAGE_PAD_REG (DR_REG_HMAC_BASE + 0xf0)
/** HMAC_SET_TEXT_PAD : WO; bitpos: [0]; default: 0;
 *  Configures whether or not the padding is applied by software.
 *  \\0: Not applied by software
 *  \\1: Applied by software
 */
#define HMAC_SET_TEXT_PAD    (BIT(0))
#define HMAC_SET_TEXT_PAD_M  (HMAC_SET_TEXT_PAD_V << HMAC_SET_TEXT_PAD_S)
#define HMAC_SET_TEXT_PAD_V  0x00000001U
#define HMAC_SET_TEXT_PAD_S  0

/** HMAC_ONE_BLOCK_REG register
 *  One block message register
 */
#define HMAC_ONE_BLOCK_REG (DR_REG_HMAC_BASE + 0xf4)
/** HMAC_SET_ONE_BLOCK : WS; bitpos: [0]; default: 0;
 *  Write 1 to indicate there is only one block which already contains padding bits and
 *  there is no need for padding.
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
 *  Configures whether or not to enable JTAG authentication mode.
 *  \\0: Disable
 *  \\1: Enable
 *  \\
 */
#define HMAC_SOFT_JTAG_CTRL    (BIT(0))
#define HMAC_SOFT_JTAG_CTRL_M  (HMAC_SOFT_JTAG_CTRL_V << HMAC_SOFT_JTAG_CTRL_S)
#define HMAC_SOFT_JTAG_CTRL_V  0x00000001U
#define HMAC_SOFT_JTAG_CTRL_S  0

/** HMAC_WR_JTAG_REG register
 *  Re-enable JTAG register 1
 */
#define HMAC_WR_JTAG_REG (DR_REG_HMAC_BASE + 0xfc)
/** HMAC_WR_JTAG : WO; bitpos: [31:0]; default: 0;
 *  Writes the comparing input used for re-enabling JTAG.
 */
#define HMAC_WR_JTAG    0xFFFFFFFFU
#define HMAC_WR_JTAG_M  (HMAC_WR_JTAG_V << HMAC_WR_JTAG_S)
#define HMAC_WR_JTAG_V  0xFFFFFFFFU
#define HMAC_WR_JTAG_S  0

/** HMAC_DATE_REG register
 *  Version control register
 */
#define HMAC_DATE_REG (DR_REG_HMAC_BASE + 0x1fc)
/** HMAC_DATE : R/W; bitpos: [29:0]; default: 539166977;
 *  Hmac date information/ hmac version information.
 */
#define HMAC_DATE    0x3FFFFFFFU
#define HMAC_DATE_M  (HMAC_DATE_V << HMAC_DATE_S)
#define HMAC_DATE_V  0x3FFFFFFFU
#define HMAC_DATE_S  0

#ifdef __cplusplus
}
#endif
