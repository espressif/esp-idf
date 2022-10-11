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

/** DS_Y_MEM register
 *  memory that stores Y
 */
#define DS_Y_MEM (DR_REG_DS_BASE + 0x0)
#define DS_Y_MEM_SIZE_BYTES 512

/** DS_M_MEM register
 *  memory that stores M
 */
#define DS_M_MEM (DR_REG_DS_BASE + 0x200)
#define DS_M_MEM_SIZE_BYTES 512

/** DS_RB_MEM register
 *  memory that stores Rb
 */
#define DS_RB_MEM (DR_REG_DS_BASE + 0x400)
#define DS_RB_MEM_SIZE_BYTES 512

/** DS_BOX_MEM register
 *  memory that stores BOX
 */
#define DS_BOX_MEM (DR_REG_DS_BASE + 0x600)
#define DS_BOX_MEM_SIZE_BYTES 48

/** DS_IV_MEM register
 *  memory that stores IV
 */
#define DS_IV_MEM (DR_REG_DS_BASE + 0x630)
#define DS_IV_MEM_SIZE_BYTES 16

/** DS_X_MEM register
 *  memory that stores X
 */
#define DS_X_MEM (DR_REG_DS_BASE + 0x800)
#define DS_X_MEM_SIZE_BYTES 512

/** DS_Z_MEM register
 *  memory that stores Z
 */
#define DS_Z_MEM (DR_REG_DS_BASE + 0xa00)
#define DS_Z_MEM_SIZE_BYTES 512

/** DS_SET_START_REG register
 *  DS start control register
 */
#define DS_SET_START_REG (DR_REG_DS_BASE + 0xe00)
/** DS_SET_START : WT; bitpos: [0]; default: 0;
 *  set this bit to start DS operation.
 */
#define DS_SET_START    (BIT(0))
#define DS_SET_START_M  (DS_SET_START_V << DS_SET_START_S)
#define DS_SET_START_V  0x00000001U
#define DS_SET_START_S  0

/** DS_SET_CONTINUE_REG register
 *  DS continue control register
 */
#define DS_SET_CONTINUE_REG (DR_REG_DS_BASE + 0xe04)
/** DS_SET_CONTINUE : WT; bitpos: [0]; default: 0;
 *  set this bit to continue DS operation.
 */
#define DS_SET_CONTINUE    (BIT(0))
#define DS_SET_CONTINUE_M  (DS_SET_CONTINUE_V << DS_SET_CONTINUE_S)
#define DS_SET_CONTINUE_V  0x00000001U
#define DS_SET_CONTINUE_S  0

/** DS_SET_FINISH_REG register
 *  DS finish control register
 */
#define DS_SET_FINISH_REG (DR_REG_DS_BASE + 0xe08)
/** DS_SET_FINISH : WT; bitpos: [0]; default: 0;
 *  Set this bit to finish DS process.
 */
#define DS_SET_FINISH    (BIT(0))
#define DS_SET_FINISH_M  (DS_SET_FINISH_V << DS_SET_FINISH_S)
#define DS_SET_FINISH_V  0x00000001U
#define DS_SET_FINISH_S  0

/** DS_QUERY_BUSY_REG register
 *  DS query busy register
 */
#define DS_QUERY_BUSY_REG (DR_REG_DS_BASE + 0xe0c)
/** DS_QUERY_BUSY : RO; bitpos: [0]; default: 0;
 *  digital signature state. 1'b0: idle, 1'b1: busy
 */
#define DS_QUERY_BUSY    (BIT(0))
#define DS_QUERY_BUSY_M  (DS_QUERY_BUSY_V << DS_QUERY_BUSY_S)
#define DS_QUERY_BUSY_V  0x00000001U
#define DS_QUERY_BUSY_S  0

/** DS_QUERY_KEY_WRONG_REG register
 *  DS query key-wrong counter register
 */
#define DS_QUERY_KEY_WRONG_REG (DR_REG_DS_BASE + 0xe10)
/** DS_QUERY_KEY_WRONG : RO; bitpos: [3:0]; default: 0;
 *  digital signature key wrong counter
 */
#define DS_QUERY_KEY_WRONG    0x0000000FU
#define DS_QUERY_KEY_WRONG_M  (DS_QUERY_KEY_WRONG_V << DS_QUERY_KEY_WRONG_S)
#define DS_QUERY_KEY_WRONG_V  0x0000000FU
#define DS_QUERY_KEY_WRONG_S  0

/** DS_QUERY_CHECK_REG register
 *  DS query check result register
 */
#define DS_QUERY_CHECK_REG (DR_REG_DS_BASE + 0xe14)
/** DS_MD_ERROR : RO; bitpos: [0]; default: 0;
 *  MD checkout result. 1'b0: MD check pass, 1'b1: MD check fail
 */
#define DS_MD_ERROR    (BIT(0))
#define DS_MD_ERROR_M  (DS_MD_ERROR_V << DS_MD_ERROR_S)
#define DS_MD_ERROR_V  0x00000001U
#define DS_MD_ERROR_S  0
/** DS_PADDING_BAD : RO; bitpos: [1]; default: 0;
 *  padding checkout result. 1'b0: a good padding, 1'b1: a bad padding
 */
#define DS_PADDING_BAD    (BIT(1))
#define DS_PADDING_BAD_M  (DS_PADDING_BAD_V << DS_PADDING_BAD_S)
#define DS_PADDING_BAD_V  0x00000001U
#define DS_PADDING_BAD_S  1

/** DS_DATE_REG register
 *  DS version control register
 */
#define DS_DATE_REG (DR_REG_DS_BASE + 0xe20)
/** DS_DATE : R/W; bitpos: [29:0]; default: 538969624;
 *  ds version information
 */
#define DS_DATE    0x3FFFFFFFU
#define DS_DATE_M  (DS_DATE_V << DS_DATE_S)
#define DS_DATE_V  0x3FFFFFFFU
#define DS_DATE_S  0

#ifdef __cplusplus
}
#endif
