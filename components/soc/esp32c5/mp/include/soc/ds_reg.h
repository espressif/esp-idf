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
 *  Activates the DS module
 */
#define DS_SET_START_REG (DR_REG_DS_BASE + 0xe00)
/** DS_SET_START : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to activate the DS peripheral.\\
 *  0: Invalid\\
 *  1: Activate the DS peripheral\\
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
 *  Ends DS operation
 */
#define DS_SET_FINISH_REG (DR_REG_DS_BASE + 0xe08)
/** DS_SET_FINISH : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to end DS operation. \\
 *  0: Invalid\\
 *  1: End DS operation\\
 */
#define DS_SET_FINISH    (BIT(0))
#define DS_SET_FINISH_M  (DS_SET_FINISH_V << DS_SET_FINISH_S)
#define DS_SET_FINISH_V  0x00000001U
#define DS_SET_FINISH_S  0

/** DS_QUERY_BUSY_REG register
 *  Status of the DS module
 */
#define DS_QUERY_BUSY_REG (DR_REG_DS_BASE + 0xe0c)
/** DS_QUERY_BUSY : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the DS module is idle.\\
 *  0: The DS module is idle\\
 *  1: The DS module is busy\\
 */
#define DS_QUERY_BUSY    (BIT(0))
#define DS_QUERY_BUSY_M  (DS_QUERY_BUSY_V << DS_QUERY_BUSY_S)
#define DS_QUERY_BUSY_V  0x00000001U
#define DS_QUERY_BUSY_S  0

/** DS_QUERY_KEY_WRONG_REG register
 *  Checks the reason why \begin{math}DS_KEY\end{math} is not ready
 */
#define DS_QUERY_KEY_WRONG_REG (DR_REG_DS_BASE + 0xe10)
/** DS_QUERY_KEY_WRONG : RO; bitpos: [3:0]; default: 0;
 *  Represents the specific problem with HMAC initialization.\\
 *  0: HMAC is not called\\
 *  1-15: HMAC was activated, but the DS peripheral did not successfully receive the
 *  \begin{math}DS_KEY\end{math} from the HMAC peripheral. (The biggest value is 15)\\
 */
#define DS_QUERY_KEY_WRONG    0x0000000FU
#define DS_QUERY_KEY_WRONG_M  (DS_QUERY_KEY_WRONG_V << DS_QUERY_KEY_WRONG_S)
#define DS_QUERY_KEY_WRONG_V  0x0000000FU
#define DS_QUERY_KEY_WRONG_S  0

/** DS_QUERY_CHECK_REG register
 *  Queries DS check result
 */
#define DS_QUERY_CHECK_REG (DR_REG_DS_BASE + 0xe14)
/** DS_MD_ERROR : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the MD check passes.\\
 *  0: The MD check passes\\
 *  1: The MD check fails\\
 */
#define DS_MD_ERROR    (BIT(0))
#define DS_MD_ERROR_M  (DS_MD_ERROR_V << DS_MD_ERROR_S)
#define DS_MD_ERROR_V  0x00000001U
#define DS_MD_ERROR_S  0
/** DS_PADDING_BAD : RO; bitpos: [1]; default: 0;
 *  Represents whether or not the padding check passes.\\
 *  0: The padding check passes\\
 *  1: The padding check fails\\
 */
#define DS_PADDING_BAD    (BIT(1))
#define DS_PADDING_BAD_M  (DS_PADDING_BAD_V << DS_PADDING_BAD_S)
#define DS_PADDING_BAD_V  0x00000001U
#define DS_PADDING_BAD_S  1

/** DS_KEY_SOURCE_REG register
 *  DS configure key source register
 */
#define DS_KEY_SOURCE_REG (DR_REG_DS_BASE + 0xe18)
/** DS_KEY_SOURCE : R/W; bitpos: [0]; default: 0;
 *  digital signature key source bit. \\
 *  1'b0: key is from hmac.\\
 *  1'b1: key is from key manager. \\
 */
#define DS_KEY_SOURCE    (BIT(0))
#define DS_KEY_SOURCE_M  (DS_KEY_SOURCE_V << DS_KEY_SOURCE_S)
#define DS_KEY_SOURCE_V  0x00000001U
#define DS_KEY_SOURCE_S  0

/** DS_DATE_REG register
 *  DS version control register
 */
#define DS_DATE_REG (DR_REG_DS_BASE + 0xe20)
/** DS_DATE : R/W; bitpos: [29:0]; default: 539166977;
 *  ds version information
 */
#define DS_DATE    0x3FFFFFFFU
#define DS_DATE_M  (DS_DATE_V << DS_DATE_S)
#define DS_DATE_V  0x3FFFFFFFU
#define DS_DATE_S  0

#ifdef __cplusplus
}
#endif
