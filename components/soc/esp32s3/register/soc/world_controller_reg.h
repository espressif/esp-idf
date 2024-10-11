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

/** WCL_Core_0_ENTRY_1_ADDR_REG register
 *  Core_0 Entry 1 address configuration Register
 */
#define WCL_CORE_0_ENTRY_1_ADDR_REG (DR_REG_WCL_BASE + 0x0)
/** WCL_CORE_0_ENTRY_1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 1 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_1_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_1_ADDR_M  (WCL_CORE_0_ENTRY_1_ADDR_V << WCL_CORE_0_ENTRY_1_ADDR_S)
#define WCL_CORE_0_ENTRY_1_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_1_ADDR_S  0

/** WCL_Core_0_ENTRY_2_ADDR_REG register
 *  Core_0 Entry 2 address configuration Register
 */
#define WCL_CORE_0_ENTRY_2_ADDR_REG (DR_REG_WCL_BASE + 0x4)
/** WCL_CORE_0_ENTRY_2_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 2 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_2_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_2_ADDR_M  (WCL_CORE_0_ENTRY_2_ADDR_V << WCL_CORE_0_ENTRY_2_ADDR_S)
#define WCL_CORE_0_ENTRY_2_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_2_ADDR_S  0

/** WCL_Core_0_ENTRY_3_ADDR_REG register
 *  Core_0 Entry 3 address configuration Register
 */
#define WCL_CORE_0_ENTRY_3_ADDR_REG (DR_REG_WCL_BASE + 0x8)
/** WCL_CORE_0_ENTRY_3_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 3 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_3_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_3_ADDR_M  (WCL_CORE_0_ENTRY_3_ADDR_V << WCL_CORE_0_ENTRY_3_ADDR_S)
#define WCL_CORE_0_ENTRY_3_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_3_ADDR_S  0

/** WCL_Core_0_ENTRY_4_ADDR_REG register
 *  Core_0 Entry 4 address configuration Register
 */
#define WCL_CORE_0_ENTRY_4_ADDR_REG (DR_REG_WCL_BASE + 0xc)
/** WCL_CORE_0_ENTRY_4_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 4 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_4_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_4_ADDR_M  (WCL_CORE_0_ENTRY_4_ADDR_V << WCL_CORE_0_ENTRY_4_ADDR_S)
#define WCL_CORE_0_ENTRY_4_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_4_ADDR_S  0

/** WCL_Core_0_ENTRY_5_ADDR_REG register
 *  Core_0 Entry 5 address configuration Register
 */
#define WCL_CORE_0_ENTRY_5_ADDR_REG (DR_REG_WCL_BASE + 0x10)
/** WCL_CORE_0_ENTRY_5_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 5 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_5_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_5_ADDR_M  (WCL_CORE_0_ENTRY_5_ADDR_V << WCL_CORE_0_ENTRY_5_ADDR_S)
#define WCL_CORE_0_ENTRY_5_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_5_ADDR_S  0

/** WCL_Core_0_ENTRY_6_ADDR_REG register
 *  Core_0 Entry 6 address configuration Register
 */
#define WCL_CORE_0_ENTRY_6_ADDR_REG (DR_REG_WCL_BASE + 0x14)
/** WCL_CORE_0_ENTRY_6_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 6 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_6_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_6_ADDR_M  (WCL_CORE_0_ENTRY_6_ADDR_V << WCL_CORE_0_ENTRY_6_ADDR_S)
#define WCL_CORE_0_ENTRY_6_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_6_ADDR_S  0

/** WCL_Core_0_ENTRY_7_ADDR_REG register
 *  Core_0 Entry 7 address configuration Register
 */
#define WCL_CORE_0_ENTRY_7_ADDR_REG (DR_REG_WCL_BASE + 0x18)
/** WCL_CORE_0_ENTRY_7_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 7 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_7_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_7_ADDR_M  (WCL_CORE_0_ENTRY_7_ADDR_V << WCL_CORE_0_ENTRY_7_ADDR_S)
#define WCL_CORE_0_ENTRY_7_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_7_ADDR_S  0

/** WCL_Core_0_ENTRY_8_ADDR_REG register
 *  Core_0 Entry 8 address configuration Register
 */
#define WCL_CORE_0_ENTRY_8_ADDR_REG (DR_REG_WCL_BASE + 0x1c)
/** WCL_CORE_0_ENTRY_8_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 8 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_8_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_8_ADDR_M  (WCL_CORE_0_ENTRY_8_ADDR_V << WCL_CORE_0_ENTRY_8_ADDR_S)
#define WCL_CORE_0_ENTRY_8_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_8_ADDR_S  0

/** WCL_Core_0_ENTRY_9_ADDR_REG register
 *  Core_0 Entry 9 address configuration Register
 */
#define WCL_CORE_0_ENTRY_9_ADDR_REG (DR_REG_WCL_BASE + 0x20)
/** WCL_CORE_0_ENTRY_9_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 9 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_9_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_9_ADDR_M  (WCL_CORE_0_ENTRY_9_ADDR_V << WCL_CORE_0_ENTRY_9_ADDR_S)
#define WCL_CORE_0_ENTRY_9_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_9_ADDR_S  0

/** WCL_Core_0_ENTRY_10_ADDR_REG register
 *  Core_0 Entry 10 address configuration Register
 */
#define WCL_CORE_0_ENTRY_10_ADDR_REG (DR_REG_WCL_BASE + 0x24)
/** WCL_CORE_0_ENTRY_10_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 10 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_10_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_10_ADDR_M  (WCL_CORE_0_ENTRY_10_ADDR_V << WCL_CORE_0_ENTRY_10_ADDR_S)
#define WCL_CORE_0_ENTRY_10_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_10_ADDR_S  0

/** WCL_Core_0_ENTRY_11_ADDR_REG register
 *  Core_0 Entry 11 address configuration Register
 */
#define WCL_CORE_0_ENTRY_11_ADDR_REG (DR_REG_WCL_BASE + 0x28)
/** WCL_CORE_0_ENTRY_11_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 11 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_11_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_11_ADDR_M  (WCL_CORE_0_ENTRY_11_ADDR_V << WCL_CORE_0_ENTRY_11_ADDR_S)
#define WCL_CORE_0_ENTRY_11_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_11_ADDR_S  0

/** WCL_Core_0_ENTRY_12_ADDR_REG register
 *  Core_0 Entry 12 address configuration Register
 */
#define WCL_CORE_0_ENTRY_12_ADDR_REG (DR_REG_WCL_BASE + 0x2c)
/** WCL_CORE_0_ENTRY_12_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 12 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_12_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_12_ADDR_M  (WCL_CORE_0_ENTRY_12_ADDR_V << WCL_CORE_0_ENTRY_12_ADDR_S)
#define WCL_CORE_0_ENTRY_12_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_12_ADDR_S  0

/** WCL_Core_0_ENTRY_13_ADDR_REG register
 *  Core_0 Entry 13 address configuration Register
 */
#define WCL_CORE_0_ENTRY_13_ADDR_REG (DR_REG_WCL_BASE + 0x30)
/** WCL_CORE_0_ENTRY_13_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_0 Entry 13 address from WORLD1 to WORLD0
 */
#define WCL_CORE_0_ENTRY_13_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_13_ADDR_M  (WCL_CORE_0_ENTRY_13_ADDR_V << WCL_CORE_0_ENTRY_13_ADDR_S)
#define WCL_CORE_0_ENTRY_13_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_ENTRY_13_ADDR_S  0

/** WCL_Core_0_ENTRY_CHECK_REG register
 *  Core_0 Entry check configuration Register
 */
#define WCL_CORE_0_ENTRY_CHECK_REG (DR_REG_WCL_BASE + 0x7c)
/** WCL_CORE_0_ENTRY_CHECK : R/W; bitpos: [13:1]; default: 1;
 *  This filed is used to enable entry address check
 */
#define WCL_CORE_0_ENTRY_CHECK    0x00001FFFU
#define WCL_CORE_0_ENTRY_CHECK_M  (WCL_CORE_0_ENTRY_CHECK_V << WCL_CORE_0_ENTRY_CHECK_S)
#define WCL_CORE_0_ENTRY_CHECK_V  0x00001FFFU
#define WCL_CORE_0_ENTRY_CHECK_S  1

/** WCL_Core_0_STATUSTABLE1_REG register
 *  Status register of world switch of entry 1
 */
#define WCL_CORE_0_STATUSTABLE1_REG (DR_REG_WCL_BASE + 0x80)
/** WCL_CORE_0_FROM_WORLD_1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 1
 */
#define WCL_CORE_0_FROM_WORLD_1    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_1_M  (WCL_CORE_0_FROM_WORLD_1_V << WCL_CORE_0_FROM_WORLD_1_S)
#define WCL_CORE_0_FROM_WORLD_1_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_1_S  0
/** WCL_CORE_0_FROM_ENTRY_1 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 1
 */
#define WCL_CORE_0_FROM_ENTRY_1    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_1_M  (WCL_CORE_0_FROM_ENTRY_1_V << WCL_CORE_0_FROM_ENTRY_1_S)
#define WCL_CORE_0_FROM_ENTRY_1_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_1_S  1
/** WCL_CORE_0_CURRENT_1 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 1
 */
#define WCL_CORE_0_CURRENT_1    (BIT(5))
#define WCL_CORE_0_CURRENT_1_M  (WCL_CORE_0_CURRENT_1_V << WCL_CORE_0_CURRENT_1_S)
#define WCL_CORE_0_CURRENT_1_V  0x00000001U
#define WCL_CORE_0_CURRENT_1_S  5

/** WCL_Core_0_STATUSTABLE2_REG register
 *  Status register of world switch of entry 2
 */
#define WCL_CORE_0_STATUSTABLE2_REG (DR_REG_WCL_BASE + 0x84)
/** WCL_CORE_0_FROM_WORLD_2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 2
 */
#define WCL_CORE_0_FROM_WORLD_2    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_2_M  (WCL_CORE_0_FROM_WORLD_2_V << WCL_CORE_0_FROM_WORLD_2_S)
#define WCL_CORE_0_FROM_WORLD_2_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_2_S  0
/** WCL_CORE_0_FROM_ENTRY_2 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 2
 */
#define WCL_CORE_0_FROM_ENTRY_2    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_2_M  (WCL_CORE_0_FROM_ENTRY_2_V << WCL_CORE_0_FROM_ENTRY_2_S)
#define WCL_CORE_0_FROM_ENTRY_2_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_2_S  1
/** WCL_CORE_0_CURRENT_2 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 2
 */
#define WCL_CORE_0_CURRENT_2    (BIT(5))
#define WCL_CORE_0_CURRENT_2_M  (WCL_CORE_0_CURRENT_2_V << WCL_CORE_0_CURRENT_2_S)
#define WCL_CORE_0_CURRENT_2_V  0x00000001U
#define WCL_CORE_0_CURRENT_2_S  5

/** WCL_Core_0_STATUSTABLE3_REG register
 *  Status register of world switch of entry 3
 */
#define WCL_CORE_0_STATUSTABLE3_REG (DR_REG_WCL_BASE + 0x88)
/** WCL_CORE_0_FROM_WORLD_3 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 3
 */
#define WCL_CORE_0_FROM_WORLD_3    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_3_M  (WCL_CORE_0_FROM_WORLD_3_V << WCL_CORE_0_FROM_WORLD_3_S)
#define WCL_CORE_0_FROM_WORLD_3_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_3_S  0
/** WCL_CORE_0_FROM_ENTRY_3 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 3
 */
#define WCL_CORE_0_FROM_ENTRY_3    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_3_M  (WCL_CORE_0_FROM_ENTRY_3_V << WCL_CORE_0_FROM_ENTRY_3_S)
#define WCL_CORE_0_FROM_ENTRY_3_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_3_S  1
/** WCL_CORE_0_CURRENT_3 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 3
 */
#define WCL_CORE_0_CURRENT_3    (BIT(5))
#define WCL_CORE_0_CURRENT_3_M  (WCL_CORE_0_CURRENT_3_V << WCL_CORE_0_CURRENT_3_S)
#define WCL_CORE_0_CURRENT_3_V  0x00000001U
#define WCL_CORE_0_CURRENT_3_S  5

/** WCL_Core_0_STATUSTABLE4_REG register
 *  Status register of world switch of entry 4
 */
#define WCL_CORE_0_STATUSTABLE4_REG (DR_REG_WCL_BASE + 0x8c)
/** WCL_CORE_0_FROM_WORLD_4 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 4
 */
#define WCL_CORE_0_FROM_WORLD_4    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_4_M  (WCL_CORE_0_FROM_WORLD_4_V << WCL_CORE_0_FROM_WORLD_4_S)
#define WCL_CORE_0_FROM_WORLD_4_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_4_S  0
/** WCL_CORE_0_FROM_ENTRY_4 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 4
 */
#define WCL_CORE_0_FROM_ENTRY_4    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_4_M  (WCL_CORE_0_FROM_ENTRY_4_V << WCL_CORE_0_FROM_ENTRY_4_S)
#define WCL_CORE_0_FROM_ENTRY_4_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_4_S  1
/** WCL_CORE_0_CURRENT_4 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 4
 */
#define WCL_CORE_0_CURRENT_4    (BIT(5))
#define WCL_CORE_0_CURRENT_4_M  (WCL_CORE_0_CURRENT_4_V << WCL_CORE_0_CURRENT_4_S)
#define WCL_CORE_0_CURRENT_4_V  0x00000001U
#define WCL_CORE_0_CURRENT_4_S  5

/** WCL_Core_0_STATUSTABLE5_REG register
 *  Status register of world switch of entry 5
 */
#define WCL_CORE_0_STATUSTABLE5_REG (DR_REG_WCL_BASE + 0x90)
/** WCL_CORE_0_FROM_WORLD_5 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 5
 */
#define WCL_CORE_0_FROM_WORLD_5    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_5_M  (WCL_CORE_0_FROM_WORLD_5_V << WCL_CORE_0_FROM_WORLD_5_S)
#define WCL_CORE_0_FROM_WORLD_5_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_5_S  0
/** WCL_CORE_0_FROM_ENTRY_5 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 5
 */
#define WCL_CORE_0_FROM_ENTRY_5    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_5_M  (WCL_CORE_0_FROM_ENTRY_5_V << WCL_CORE_0_FROM_ENTRY_5_S)
#define WCL_CORE_0_FROM_ENTRY_5_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_5_S  1
/** WCL_CORE_0_CURRENT_5 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 5
 */
#define WCL_CORE_0_CURRENT_5    (BIT(5))
#define WCL_CORE_0_CURRENT_5_M  (WCL_CORE_0_CURRENT_5_V << WCL_CORE_0_CURRENT_5_S)
#define WCL_CORE_0_CURRENT_5_V  0x00000001U
#define WCL_CORE_0_CURRENT_5_S  5

/** WCL_Core_0_STATUSTABLE6_REG register
 *  Status register of world switch of entry 6
 */
#define WCL_CORE_0_STATUSTABLE6_REG (DR_REG_WCL_BASE + 0x94)
/** WCL_CORE_0_FROM_WORLD_6 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 6
 */
#define WCL_CORE_0_FROM_WORLD_6    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_6_M  (WCL_CORE_0_FROM_WORLD_6_V << WCL_CORE_0_FROM_WORLD_6_S)
#define WCL_CORE_0_FROM_WORLD_6_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_6_S  0
/** WCL_CORE_0_FROM_ENTRY_6 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 6
 */
#define WCL_CORE_0_FROM_ENTRY_6    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_6_M  (WCL_CORE_0_FROM_ENTRY_6_V << WCL_CORE_0_FROM_ENTRY_6_S)
#define WCL_CORE_0_FROM_ENTRY_6_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_6_S  1
/** WCL_CORE_0_CURRENT_6 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 6
 */
#define WCL_CORE_0_CURRENT_6    (BIT(5))
#define WCL_CORE_0_CURRENT_6_M  (WCL_CORE_0_CURRENT_6_V << WCL_CORE_0_CURRENT_6_S)
#define WCL_CORE_0_CURRENT_6_V  0x00000001U
#define WCL_CORE_0_CURRENT_6_S  5

/** WCL_Core_0_STATUSTABLE7_REG register
 *  Status register of world switch of entry 7
 */
#define WCL_CORE_0_STATUSTABLE7_REG (DR_REG_WCL_BASE + 0x98)
/** WCL_CORE_0_FROM_WORLD_7 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 7
 */
#define WCL_CORE_0_FROM_WORLD_7    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_7_M  (WCL_CORE_0_FROM_WORLD_7_V << WCL_CORE_0_FROM_WORLD_7_S)
#define WCL_CORE_0_FROM_WORLD_7_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_7_S  0
/** WCL_CORE_0_FROM_ENTRY_7 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 7
 */
#define WCL_CORE_0_FROM_ENTRY_7    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_7_M  (WCL_CORE_0_FROM_ENTRY_7_V << WCL_CORE_0_FROM_ENTRY_7_S)
#define WCL_CORE_0_FROM_ENTRY_7_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_7_S  1
/** WCL_CORE_0_CURRENT_7 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 7
 */
#define WCL_CORE_0_CURRENT_7    (BIT(5))
#define WCL_CORE_0_CURRENT_7_M  (WCL_CORE_0_CURRENT_7_V << WCL_CORE_0_CURRENT_7_S)
#define WCL_CORE_0_CURRENT_7_V  0x00000001U
#define WCL_CORE_0_CURRENT_7_S  5

/** WCL_Core_0_STATUSTABLE8_REG register
 *  Status register of world switch of entry 8
 */
#define WCL_CORE_0_STATUSTABLE8_REG (DR_REG_WCL_BASE + 0x9c)
/** WCL_CORE_0_FROM_WORLD_8 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 8
 */
#define WCL_CORE_0_FROM_WORLD_8    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_8_M  (WCL_CORE_0_FROM_WORLD_8_V << WCL_CORE_0_FROM_WORLD_8_S)
#define WCL_CORE_0_FROM_WORLD_8_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_8_S  0
/** WCL_CORE_0_FROM_ENTRY_8 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 8
 */
#define WCL_CORE_0_FROM_ENTRY_8    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_8_M  (WCL_CORE_0_FROM_ENTRY_8_V << WCL_CORE_0_FROM_ENTRY_8_S)
#define WCL_CORE_0_FROM_ENTRY_8_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_8_S  1
/** WCL_CORE_0_CURRENT_8 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 8
 */
#define WCL_CORE_0_CURRENT_8    (BIT(5))
#define WCL_CORE_0_CURRENT_8_M  (WCL_CORE_0_CURRENT_8_V << WCL_CORE_0_CURRENT_8_S)
#define WCL_CORE_0_CURRENT_8_V  0x00000001U
#define WCL_CORE_0_CURRENT_8_S  5

/** WCL_Core_0_STATUSTABLE9_REG register
 *  Status register of world switch of entry 9
 */
#define WCL_CORE_0_STATUSTABLE9_REG (DR_REG_WCL_BASE + 0xa0)
/** WCL_CORE_0_FROM_WORLD_9 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 9
 */
#define WCL_CORE_0_FROM_WORLD_9    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_9_M  (WCL_CORE_0_FROM_WORLD_9_V << WCL_CORE_0_FROM_WORLD_9_S)
#define WCL_CORE_0_FROM_WORLD_9_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_9_S  0
/** WCL_CORE_0_FROM_ENTRY_9 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 9
 */
#define WCL_CORE_0_FROM_ENTRY_9    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_9_M  (WCL_CORE_0_FROM_ENTRY_9_V << WCL_CORE_0_FROM_ENTRY_9_S)
#define WCL_CORE_0_FROM_ENTRY_9_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_9_S  1
/** WCL_CORE_0_CURRENT_9 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 9
 */
#define WCL_CORE_0_CURRENT_9    (BIT(5))
#define WCL_CORE_0_CURRENT_9_M  (WCL_CORE_0_CURRENT_9_V << WCL_CORE_0_CURRENT_9_S)
#define WCL_CORE_0_CURRENT_9_V  0x00000001U
#define WCL_CORE_0_CURRENT_9_S  5

/** WCL_Core_0_STATUSTABLE10_REG register
 *  Status register of world switch of entry 10
 */
#define WCL_CORE_0_STATUSTABLE10_REG (DR_REG_WCL_BASE + 0xa4)
/** WCL_CORE_0_FROM_WORLD_10 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 10
 */
#define WCL_CORE_0_FROM_WORLD_10    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_10_M  (WCL_CORE_0_FROM_WORLD_10_V << WCL_CORE_0_FROM_WORLD_10_S)
#define WCL_CORE_0_FROM_WORLD_10_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_10_S  0
/** WCL_CORE_0_FROM_ENTRY_10 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 10
 */
#define WCL_CORE_0_FROM_ENTRY_10    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_10_M  (WCL_CORE_0_FROM_ENTRY_10_V << WCL_CORE_0_FROM_ENTRY_10_S)
#define WCL_CORE_0_FROM_ENTRY_10_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_10_S  1
/** WCL_CORE_0_CURRENT_10 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 10
 */
#define WCL_CORE_0_CURRENT_10    (BIT(5))
#define WCL_CORE_0_CURRENT_10_M  (WCL_CORE_0_CURRENT_10_V << WCL_CORE_0_CURRENT_10_S)
#define WCL_CORE_0_CURRENT_10_V  0x00000001U
#define WCL_CORE_0_CURRENT_10_S  5

/** WCL_Core_0_STATUSTABLE11_REG register
 *  Status register of world switch of entry 11
 */
#define WCL_CORE_0_STATUSTABLE11_REG (DR_REG_WCL_BASE + 0xa8)
/** WCL_CORE_0_FROM_WORLD_11 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 11
 */
#define WCL_CORE_0_FROM_WORLD_11    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_11_M  (WCL_CORE_0_FROM_WORLD_11_V << WCL_CORE_0_FROM_WORLD_11_S)
#define WCL_CORE_0_FROM_WORLD_11_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_11_S  0
/** WCL_CORE_0_FROM_ENTRY_11 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 11
 */
#define WCL_CORE_0_FROM_ENTRY_11    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_11_M  (WCL_CORE_0_FROM_ENTRY_11_V << WCL_CORE_0_FROM_ENTRY_11_S)
#define WCL_CORE_0_FROM_ENTRY_11_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_11_S  1
/** WCL_CORE_0_CURRENT_11 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 11
 */
#define WCL_CORE_0_CURRENT_11    (BIT(5))
#define WCL_CORE_0_CURRENT_11_M  (WCL_CORE_0_CURRENT_11_V << WCL_CORE_0_CURRENT_11_S)
#define WCL_CORE_0_CURRENT_11_V  0x00000001U
#define WCL_CORE_0_CURRENT_11_S  5

/** WCL_Core_0_STATUSTABLE12_REG register
 *  Status register of world switch of entry 12
 */
#define WCL_CORE_0_STATUSTABLE12_REG (DR_REG_WCL_BASE + 0xac)
/** WCL_CORE_0_FROM_WORLD_12 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 12
 */
#define WCL_CORE_0_FROM_WORLD_12    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_12_M  (WCL_CORE_0_FROM_WORLD_12_V << WCL_CORE_0_FROM_WORLD_12_S)
#define WCL_CORE_0_FROM_WORLD_12_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_12_S  0
/** WCL_CORE_0_FROM_ENTRY_12 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 12
 */
#define WCL_CORE_0_FROM_ENTRY_12    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_12_M  (WCL_CORE_0_FROM_ENTRY_12_V << WCL_CORE_0_FROM_ENTRY_12_S)
#define WCL_CORE_0_FROM_ENTRY_12_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_12_S  1
/** WCL_CORE_0_CURRENT_12 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 12
 */
#define WCL_CORE_0_CURRENT_12    (BIT(5))
#define WCL_CORE_0_CURRENT_12_M  (WCL_CORE_0_CURRENT_12_V << WCL_CORE_0_CURRENT_12_S)
#define WCL_CORE_0_CURRENT_12_V  0x00000001U
#define WCL_CORE_0_CURRENT_12_S  5

/** WCL_Core_0_STATUSTABLE13_REG register
 *  Status register of world switch of entry 13
 */
#define WCL_CORE_0_STATUSTABLE13_REG (DR_REG_WCL_BASE + 0xb0)
/** WCL_CORE_0_FROM_WORLD_13 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 13
 */
#define WCL_CORE_0_FROM_WORLD_13    (BIT(0))
#define WCL_CORE_0_FROM_WORLD_13_M  (WCL_CORE_0_FROM_WORLD_13_V << WCL_CORE_0_FROM_WORLD_13_S)
#define WCL_CORE_0_FROM_WORLD_13_V  0x00000001U
#define WCL_CORE_0_FROM_WORLD_13_S  0
/** WCL_CORE_0_FROM_ENTRY_13 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 13
 */
#define WCL_CORE_0_FROM_ENTRY_13    0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_13_M  (WCL_CORE_0_FROM_ENTRY_13_V << WCL_CORE_0_FROM_ENTRY_13_S)
#define WCL_CORE_0_FROM_ENTRY_13_V  0x0000000FU
#define WCL_CORE_0_FROM_ENTRY_13_S  1
/** WCL_CORE_0_CURRENT_13 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 13
 */
#define WCL_CORE_0_CURRENT_13    (BIT(5))
#define WCL_CORE_0_CURRENT_13_M  (WCL_CORE_0_CURRENT_13_V << WCL_CORE_0_CURRENT_13_S)
#define WCL_CORE_0_CURRENT_13_V  0x00000001U
#define WCL_CORE_0_CURRENT_13_S  5

/** WCL_Core_0_STATUSTABLE_CURRENT_REG register
 *  Status register of statustable current
 */
#define WCL_CORE_0_STATUSTABLE_CURRENT_REG (DR_REG_WCL_BASE + 0xfc)
/** WCL_CORE_0_STATUSTABLE_CURRENT : R/W; bitpos: [13:1]; default: 0;
 *  This field is used to quickly read and rewrite the current field of all STATUSTABLE
 *  registers,for example,bit 1 represents the current field of STATUSTABLE1,bit2
 *  represents the current field of STATUSTABLE2
 */
#define WCL_CORE_0_STATUSTABLE_CURRENT    0x00001FFFU
#define WCL_CORE_0_STATUSTABLE_CURRENT_M  (WCL_CORE_0_STATUSTABLE_CURRENT_V << WCL_CORE_0_STATUSTABLE_CURRENT_S)
#define WCL_CORE_0_STATUSTABLE_CURRENT_V  0x00001FFFU
#define WCL_CORE_0_STATUSTABLE_CURRENT_S  1

/** WCL_Core_0_MESSAGE_ADDR_REG register
 *  Clear writer_buffer write address configuration register
 */
#define WCL_CORE_0_MESSAGE_ADDR_REG (DR_REG_WCL_BASE + 0x100)
/** WCL_CORE_0_MESSAGE_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  This field is used to set address that need to write when enter WORLD0
 */
#define WCL_CORE_0_MESSAGE_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_MESSAGE_ADDR_M  (WCL_CORE_0_MESSAGE_ADDR_V << WCL_CORE_0_MESSAGE_ADDR_S)
#define WCL_CORE_0_MESSAGE_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_MESSAGE_ADDR_S  0

/** WCL_Core_0_MESSAGE_MAX_REG register
 *  Clear writer_buffer write number configuration register
 */
#define WCL_CORE_0_MESSAGE_MAX_REG (DR_REG_WCL_BASE + 0x104)
/** WCL_CORE_0_MESSAGE_MAX : R/W; bitpos: [3:0]; default: 0;
 *  This filed is used to set the max value of clear write_buffer
 */
#define WCL_CORE_0_MESSAGE_MAX    0x0000000FU
#define WCL_CORE_0_MESSAGE_MAX_M  (WCL_CORE_0_MESSAGE_MAX_V << WCL_CORE_0_MESSAGE_MAX_S)
#define WCL_CORE_0_MESSAGE_MAX_V  0x0000000FU
#define WCL_CORE_0_MESSAGE_MAX_S  0

/** WCL_Core_0_MESSAGE_PHASE_REG register
 *  Clear writer_buffer status register
 */
#define WCL_CORE_0_MESSAGE_PHASE_REG (DR_REG_WCL_BASE + 0x108)
/** WCL_CORE_0_MESSAGE_MATCH : RO; bitpos: [0]; default: 0;
 *  This bit indicates whether the check is successful
 */
#define WCL_CORE_0_MESSAGE_MATCH    (BIT(0))
#define WCL_CORE_0_MESSAGE_MATCH_M  (WCL_CORE_0_MESSAGE_MATCH_V << WCL_CORE_0_MESSAGE_MATCH_S)
#define WCL_CORE_0_MESSAGE_MATCH_V  0x00000001U
#define WCL_CORE_0_MESSAGE_MATCH_S  0
/** WCL_CORE_0_MESSAGE_EXPECT : RO; bitpos: [4:1]; default: 0;
 *  This field indicates the data to be written next time
 */
#define WCL_CORE_0_MESSAGE_EXPECT    0x0000000FU
#define WCL_CORE_0_MESSAGE_EXPECT_M  (WCL_CORE_0_MESSAGE_EXPECT_V << WCL_CORE_0_MESSAGE_EXPECT_S)
#define WCL_CORE_0_MESSAGE_EXPECT_V  0x0000000FU
#define WCL_CORE_0_MESSAGE_EXPECT_S  1
/** WCL_CORE_0_MESSAGE_DATAPHASE : RO; bitpos: [5]; default: 0;
 *  If this bit is 1, it means that is checking clear write_buffer operation,and is
 *  checking data
 */
#define WCL_CORE_0_MESSAGE_DATAPHASE    (BIT(5))
#define WCL_CORE_0_MESSAGE_DATAPHASE_M  (WCL_CORE_0_MESSAGE_DATAPHASE_V << WCL_CORE_0_MESSAGE_DATAPHASE_S)
#define WCL_CORE_0_MESSAGE_DATAPHASE_V  0x00000001U
#define WCL_CORE_0_MESSAGE_DATAPHASE_S  5
/** WCL_CORE_0_MESSAGE_ADDRESSPHASE : RO; bitpos: [6]; default: 0;
 *  If this bit is 1, it means that is checking clear write_buffer operation,and is
 *  checking address.
 */
#define WCL_CORE_0_MESSAGE_ADDRESSPHASE    (BIT(6))
#define WCL_CORE_0_MESSAGE_ADDRESSPHASE_M  (WCL_CORE_0_MESSAGE_ADDRESSPHASE_V << WCL_CORE_0_MESSAGE_ADDRESSPHASE_S)
#define WCL_CORE_0_MESSAGE_ADDRESSPHASE_V  0x00000001U
#define WCL_CORE_0_MESSAGE_ADDRESSPHASE_S  6

/** WCL_Core_0_World_TRIGGER_ADDR_REG register
 *  Core_0 trigger address configuration Register
 */
#define WCL_CORE_0_WORLD_TRIGGER_ADDR_REG (DR_REG_WCL_BASE + 0x140)
/** WCL_CORE_0_WORLD_TRIGGER_ADDR : RW; bitpos: [31:0]; default: 0;
 *  This field is used to configure the entry address from WORLD0 to WORLD1,when the
 *  CPU executes to this address,switch to WORLD1
 */
#define WCL_CORE_0_WORLD_TRIGGER_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_WORLD_TRIGGER_ADDR_M  (WCL_CORE_0_WORLD_TRIGGER_ADDR_V << WCL_CORE_0_WORLD_TRIGGER_ADDR_S)
#define WCL_CORE_0_WORLD_TRIGGER_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_WORLD_TRIGGER_ADDR_S  0

/** WCL_Core_0_World_PREPARE_REG register
 *  Core_0 prepare world configuration Register
 */
#define WCL_CORE_0_WORLD_PREPARE_REG (DR_REG_WCL_BASE + 0x144)
/** WCL_CORE_0_WORLD_PREPARE : R/W; bitpos: [1:0]; default: 0;
 *  This field to used to set world to enter,  2'b01 means WORLD0, 2'b10 means WORLD1
 */
#define WCL_CORE_0_WORLD_PREPARE    0x00000003U
#define WCL_CORE_0_WORLD_PREPARE_M  (WCL_CORE_0_WORLD_PREPARE_V << WCL_CORE_0_WORLD_PREPARE_S)
#define WCL_CORE_0_WORLD_PREPARE_V  0x00000003U
#define WCL_CORE_0_WORLD_PREPARE_S  0

/** WCL_Core_0_World_UPDATE_REG register
 *  Core_0 configuration update register
 */
#define WCL_CORE_0_WORLD_UPDATE_REG (DR_REG_WCL_BASE + 0x148)
/** WCL_CORE_0_UPDATE : WO; bitpos: [31:0]; default: 0;
 *  This field is used to update configuration completed, can write any value,the
 *  hardware only checks the write operation of this register and does not case about
 *  its value
 */
#define WCL_CORE_0_UPDATE    0xFFFFFFFFU
#define WCL_CORE_0_UPDATE_M  (WCL_CORE_0_UPDATE_V << WCL_CORE_0_UPDATE_S)
#define WCL_CORE_0_UPDATE_V  0xFFFFFFFFU
#define WCL_CORE_0_UPDATE_S  0

/** WCL_Core_0_World_Cancel_REG register
 *  Core_0 configuration cancel register
 */
#define WCL_CORE_0_WORLD_CANCEL_REG (DR_REG_WCL_BASE + 0x14c)
/** WCL_CORE_0_WORLD_CANCEL : WO; bitpos: [31:0]; default: 0;
 *  This field is used to cancel switch world configuration,if the trigger address and
 *  update configuration complete,use this register to cancel world switch, jujst need
 *  write any value,the hardware only checks the write operation of this register and
 *  does not case about its value
 */
#define WCL_CORE_0_WORLD_CANCEL    0xFFFFFFFFU
#define WCL_CORE_0_WORLD_CANCEL_M  (WCL_CORE_0_WORLD_CANCEL_V << WCL_CORE_0_WORLD_CANCEL_S)
#define WCL_CORE_0_WORLD_CANCEL_V  0xFFFFFFFFU
#define WCL_CORE_0_WORLD_CANCEL_S  0

/** WCL_Core_0_World_IRam0_REG register
 *  Core_0 Iram0 world register
 */
#define WCL_CORE_0_WORLD_IRAM0_REG (DR_REG_WCL_BASE + 0x150)
/** WCL_CORE_0_WORLD_IRAM0 : R/W; bitpos: [1:0]; default: 0;
 *  this field is used to read current world of Iram0 bus
 */
#define WCL_CORE_0_WORLD_IRAM0    0x00000003U
#define WCL_CORE_0_WORLD_IRAM0_M  (WCL_CORE_0_WORLD_IRAM0_V << WCL_CORE_0_WORLD_IRAM0_S)
#define WCL_CORE_0_WORLD_IRAM0_V  0x00000003U
#define WCL_CORE_0_WORLD_IRAM0_S  0

/** WCL_Core_0_World_DRam0_PIF_REG register
 *  Core_0 dram0 and PIF world register
 */
#define WCL_CORE_0_WORLD_DRAM0_PIF_REG (DR_REG_WCL_BASE + 0x154)
/** WCL_CORE_0_WORLD_DRAM0_PIF : R/W; bitpos: [1:0]; default: 0;
 *  this field is used to read current world of Dram0 bus and PIF bus
 */
#define WCL_CORE_0_WORLD_DRAM0_PIF    0x00000003U
#define WCL_CORE_0_WORLD_DRAM0_PIF_M  (WCL_CORE_0_WORLD_DRAM0_PIF_V << WCL_CORE_0_WORLD_DRAM0_PIF_S)
#define WCL_CORE_0_WORLD_DRAM0_PIF_V  0x00000003U
#define WCL_CORE_0_WORLD_DRAM0_PIF_S  0

/** WCL_Core_0_World_Phase_REG register
 *  Core_0 world status register
 */
#define WCL_CORE_0_WORLD_PHASE_REG (DR_REG_WCL_BASE + 0x158)
/** WCL_CORE_0_WORLD_PHASE : RO; bitpos: [0]; default: 0;
 *  This bit indicates whether is preparing to switch to WORLD1, 1 means value.
 */
#define WCL_CORE_0_WORLD_PHASE    (BIT(0))
#define WCL_CORE_0_WORLD_PHASE_M  (WCL_CORE_0_WORLD_PHASE_V << WCL_CORE_0_WORLD_PHASE_S)
#define WCL_CORE_0_WORLD_PHASE_V  0x00000001U
#define WCL_CORE_0_WORLD_PHASE_S  0

/** WCL_Core_0_NMI_MASK_ENABLE_REG register
 *  Core_0 NMI mask enable register
 */
#define WCL_CORE_0_NMI_MASK_ENABLE_REG (DR_REG_WCL_BASE + 0x180)
/** WCL_CORE_0_NMI_MASK_ENABLE : WO; bitpos: [31:0]; default: 0;
 *  this field is used to set NMI mask,it can write any value,when write this
 *  register,the hardware start masking NMI interrupt
 */
#define WCL_CORE_0_NMI_MASK_ENABLE    0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_ENABLE_M  (WCL_CORE_0_NMI_MASK_ENABLE_V << WCL_CORE_0_NMI_MASK_ENABLE_S)
#define WCL_CORE_0_NMI_MASK_ENABLE_V  0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_ENABLE_S  0

/** WCL_Core_0_NMI_MASK_TRIGGER_ADDR_REG register
 *  Core_0 NMI mask trigger address register
 */
#define WCL_CORE_0_NMI_MASK_TRIGGER_ADDR_REG (DR_REG_WCL_BASE + 0x184)
/** WCL_CORE_0_NMI_MASK_TRIGGER_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  this field to used to set trigger address, when CPU executes to this address,NMI
 *  mask automatically fails
 */
#define WCL_CORE_0_NMI_MASK_TRIGGER_ADDR    0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_TRIGGER_ADDR_M  (WCL_CORE_0_NMI_MASK_TRIGGER_ADDR_V << WCL_CORE_0_NMI_MASK_TRIGGER_ADDR_S)
#define WCL_CORE_0_NMI_MASK_TRIGGER_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_TRIGGER_ADDR_S  0

/** WCL_Core_0_NMI_MASK_DISABLE_REG register
 *  Core_0 NMI mask disable register
 */
#define WCL_CORE_0_NMI_MASK_DISABLE_REG (DR_REG_WCL_BASE + 0x188)
/** WCL_CORE_0_NMI_MASK_DISABLE : WO; bitpos: [31:0]; default: 0;
 *  this field is used to disable NMI mask,it will not take effect immediately,only
 *  when the CPU executes to the trigger address will it start to cancel NMI mask
 */
#define WCL_CORE_0_NMI_MASK_DISABLE    0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_DISABLE_M  (WCL_CORE_0_NMI_MASK_DISABLE_V << WCL_CORE_0_NMI_MASK_DISABLE_S)
#define WCL_CORE_0_NMI_MASK_DISABLE_V  0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_DISABLE_S  0

/** WCL_Core_0_NMI_MASK_CANCLE_REG register
 *  Core_0 NMI mask disable register
 */
#define WCL_CORE_0_NMI_MASK_CANCLE_REG (DR_REG_WCL_BASE + 0x18c)
/** WCL_CORE_0_NMI_MASK_CANCEL : WO; bitpos: [31:0]; default: 0;
 *  this field is used to cancel NMI mask disable function.
 */
#define WCL_CORE_0_NMI_MASK_CANCEL    0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_CANCEL_M  (WCL_CORE_0_NMI_MASK_CANCEL_V << WCL_CORE_0_NMI_MASK_CANCEL_S)
#define WCL_CORE_0_NMI_MASK_CANCEL_V  0xFFFFFFFFU
#define WCL_CORE_0_NMI_MASK_CANCEL_S  0

/** WCL_Core_0_NMI_MASK_REG register
 *  Core_0 NMI mask register
 */
#define WCL_CORE_0_NMI_MASK_REG (DR_REG_WCL_BASE + 0x190)
/** WCL_CORE_0_NMI_MASK : R/W; bitpos: [0]; default: 0;
 *  this bit is used to mask NMI interrupt,it can directly mask NMI interrupt
 */
#define WCL_CORE_0_NMI_MASK    (BIT(0))
#define WCL_CORE_0_NMI_MASK_M  (WCL_CORE_0_NMI_MASK_V << WCL_CORE_0_NMI_MASK_S)
#define WCL_CORE_0_NMI_MASK_V  0x00000001U
#define WCL_CORE_0_NMI_MASK_S  0

/** WCL_Core_0_NMI_MASK_PHASE_REG register
 *  Core_0 NMI mask phase register
 */
#define WCL_CORE_0_NMI_MASK_PHASE_REG (DR_REG_WCL_BASE + 0x194)
/** WCL_CORE_0_NMI_MASK_PHASE : RO; bitpos: [0]; default: 0;
 *  this bit is used to indicates whether the NMI interrupt is being masked, 1 means
 *  NMI interrupt is being masked
 */
#define WCL_CORE_0_NMI_MASK_PHASE    (BIT(0))
#define WCL_CORE_0_NMI_MASK_PHASE_M  (WCL_CORE_0_NMI_MASK_PHASE_V << WCL_CORE_0_NMI_MASK_PHASE_S)
#define WCL_CORE_0_NMI_MASK_PHASE_V  0x00000001U
#define WCL_CORE_0_NMI_MASK_PHASE_S  0

/** WCL_Core_1_ENTRY_1_ADDR_REG register
 *  Core_1 Entry 1 address configuration Register
 */
#define WCL_CORE_1_ENTRY_1_ADDR_REG (DR_REG_WCL_BASE + 0x400)
/** WCL_CORE_1_ENTRY_1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 1 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_1_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_1_ADDR_M  (WCL_CORE_1_ENTRY_1_ADDR_V << WCL_CORE_1_ENTRY_1_ADDR_S)
#define WCL_CORE_1_ENTRY_1_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_1_ADDR_S  0

/** WCL_Core_1_ENTRY_2_ADDR_REG register
 *  Core_1 Entry 2 address configuration Register
 */
#define WCL_CORE_1_ENTRY_2_ADDR_REG (DR_REG_WCL_BASE + 0x404)
/** WCL_CORE_1_ENTRY_2_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 2 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_2_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_2_ADDR_M  (WCL_CORE_1_ENTRY_2_ADDR_V << WCL_CORE_1_ENTRY_2_ADDR_S)
#define WCL_CORE_1_ENTRY_2_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_2_ADDR_S  0

/** WCL_Core_1_ENTRY_3_ADDR_REG register
 *  Core_1 Entry 3 address configuration Register
 */
#define WCL_CORE_1_ENTRY_3_ADDR_REG (DR_REG_WCL_BASE + 0x408)
/** WCL_CORE_1_ENTRY_3_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 3 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_3_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_3_ADDR_M  (WCL_CORE_1_ENTRY_3_ADDR_V << WCL_CORE_1_ENTRY_3_ADDR_S)
#define WCL_CORE_1_ENTRY_3_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_3_ADDR_S  0

/** WCL_Core_1_ENTRY_4_ADDR_REG register
 *  Core_1 Entry 4 address configuration Register
 */
#define WCL_CORE_1_ENTRY_4_ADDR_REG (DR_REG_WCL_BASE + 0x40c)
/** WCL_CORE_1_ENTRY_4_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 4 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_4_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_4_ADDR_M  (WCL_CORE_1_ENTRY_4_ADDR_V << WCL_CORE_1_ENTRY_4_ADDR_S)
#define WCL_CORE_1_ENTRY_4_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_4_ADDR_S  0

/** WCL_Core_1_ENTRY_5_ADDR_REG register
 *  Core_1 Entry 5 address configuration Register
 */
#define WCL_CORE_1_ENTRY_5_ADDR_REG (DR_REG_WCL_BASE + 0x410)
/** WCL_CORE_1_ENTRY_5_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 5 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_5_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_5_ADDR_M  (WCL_CORE_1_ENTRY_5_ADDR_V << WCL_CORE_1_ENTRY_5_ADDR_S)
#define WCL_CORE_1_ENTRY_5_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_5_ADDR_S  0

/** WCL_Core_1_ENTRY_6_ADDR_REG register
 *  Core_1 Entry 6 address configuration Register
 */
#define WCL_CORE_1_ENTRY_6_ADDR_REG (DR_REG_WCL_BASE + 0x414)
/** WCL_CORE_1_ENTRY_6_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 6 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_6_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_6_ADDR_M  (WCL_CORE_1_ENTRY_6_ADDR_V << WCL_CORE_1_ENTRY_6_ADDR_S)
#define WCL_CORE_1_ENTRY_6_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_6_ADDR_S  0

/** WCL_Core_1_ENTRY_7_ADDR_REG register
 *  Core_1 Entry 7 address configuration Register
 */
#define WCL_CORE_1_ENTRY_7_ADDR_REG (DR_REG_WCL_BASE + 0x418)
/** WCL_CORE_1_ENTRY_7_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 7 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_7_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_7_ADDR_M  (WCL_CORE_1_ENTRY_7_ADDR_V << WCL_CORE_1_ENTRY_7_ADDR_S)
#define WCL_CORE_1_ENTRY_7_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_7_ADDR_S  0

/** WCL_Core_1_ENTRY_8_ADDR_REG register
 *  Core_1 Entry 8 address configuration Register
 */
#define WCL_CORE_1_ENTRY_8_ADDR_REG (DR_REG_WCL_BASE + 0x41c)
/** WCL_CORE_1_ENTRY_8_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 8 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_8_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_8_ADDR_M  (WCL_CORE_1_ENTRY_8_ADDR_V << WCL_CORE_1_ENTRY_8_ADDR_S)
#define WCL_CORE_1_ENTRY_8_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_8_ADDR_S  0

/** WCL_Core_1_ENTRY_9_ADDR_REG register
 *  Core_1 Entry 9 address configuration Register
 */
#define WCL_CORE_1_ENTRY_9_ADDR_REG (DR_REG_WCL_BASE + 0x420)
/** WCL_CORE_1_ENTRY_9_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 9 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_9_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_9_ADDR_M  (WCL_CORE_1_ENTRY_9_ADDR_V << WCL_CORE_1_ENTRY_9_ADDR_S)
#define WCL_CORE_1_ENTRY_9_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_9_ADDR_S  0

/** WCL_Core_1_ENTRY_10_ADDR_REG register
 *  Core_1 Entry 10 address configuration Register
 */
#define WCL_CORE_1_ENTRY_10_ADDR_REG (DR_REG_WCL_BASE + 0x424)
/** WCL_CORE_1_ENTRY_10_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 10 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_10_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_10_ADDR_M  (WCL_CORE_1_ENTRY_10_ADDR_V << WCL_CORE_1_ENTRY_10_ADDR_S)
#define WCL_CORE_1_ENTRY_10_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_10_ADDR_S  0

/** WCL_Core_1_ENTRY_11_ADDR_REG register
 *  Core_1 Entry 11 address configuration Register
 */
#define WCL_CORE_1_ENTRY_11_ADDR_REG (DR_REG_WCL_BASE + 0x428)
/** WCL_CORE_1_ENTRY_11_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 11 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_11_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_11_ADDR_M  (WCL_CORE_1_ENTRY_11_ADDR_V << WCL_CORE_1_ENTRY_11_ADDR_S)
#define WCL_CORE_1_ENTRY_11_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_11_ADDR_S  0

/** WCL_Core_1_ENTRY_12_ADDR_REG register
 *  Core_1 Entry 12 address configuration Register
 */
#define WCL_CORE_1_ENTRY_12_ADDR_REG (DR_REG_WCL_BASE + 0x42c)
/** WCL_CORE_1_ENTRY_12_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 12 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_12_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_12_ADDR_M  (WCL_CORE_1_ENTRY_12_ADDR_V << WCL_CORE_1_ENTRY_12_ADDR_S)
#define WCL_CORE_1_ENTRY_12_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_12_ADDR_S  0

/** WCL_Core_1_ENTRY_13_ADDR_REG register
 *  Core_1 Entry 13 address configuration Register
 */
#define WCL_CORE_1_ENTRY_13_ADDR_REG (DR_REG_WCL_BASE + 0x430)
/** WCL_CORE_1_ENTRY_13_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Core_1 Entry 13 address from WORLD1 to WORLD0
 */
#define WCL_CORE_1_ENTRY_13_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_13_ADDR_M  (WCL_CORE_1_ENTRY_13_ADDR_V << WCL_CORE_1_ENTRY_13_ADDR_S)
#define WCL_CORE_1_ENTRY_13_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_ENTRY_13_ADDR_S  0

/** WCL_Core_1_ENTRY_CHECK_REG register
 *  Core_1 Entry check configuration Register
 */
#define WCL_CORE_1_ENTRY_CHECK_REG (DR_REG_WCL_BASE + 0x47c)
/** WCL_CORE_1_ENTRY_CHECK : R/W; bitpos: [13:1]; default: 1;
 *  This filed is used to enable entry address check
 */
#define WCL_CORE_1_ENTRY_CHECK    0x00001FFFU
#define WCL_CORE_1_ENTRY_CHECK_M  (WCL_CORE_1_ENTRY_CHECK_V << WCL_CORE_1_ENTRY_CHECK_S)
#define WCL_CORE_1_ENTRY_CHECK_V  0x00001FFFU
#define WCL_CORE_1_ENTRY_CHECK_S  1

/** WCL_Core_1_STATUSTABLE1_REG register
 *  Status register of world switch of entry 1
 */
#define WCL_CORE_1_STATUSTABLE1_REG (DR_REG_WCL_BASE + 0x480)
/** WCL_CORE_1_FROM_WORLD_1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 1
 */
#define WCL_CORE_1_FROM_WORLD_1    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_1_M  (WCL_CORE_1_FROM_WORLD_1_V << WCL_CORE_1_FROM_WORLD_1_S)
#define WCL_CORE_1_FROM_WORLD_1_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_1_S  0
/** WCL_CORE_1_FROM_ENTRY_1 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 1
 */
#define WCL_CORE_1_FROM_ENTRY_1    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_1_M  (WCL_CORE_1_FROM_ENTRY_1_V << WCL_CORE_1_FROM_ENTRY_1_S)
#define WCL_CORE_1_FROM_ENTRY_1_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_1_S  1
/** WCL_CORE_1_CURRENT_1 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 1
 */
#define WCL_CORE_1_CURRENT_1    (BIT(5))
#define WCL_CORE_1_CURRENT_1_M  (WCL_CORE_1_CURRENT_1_V << WCL_CORE_1_CURRENT_1_S)
#define WCL_CORE_1_CURRENT_1_V  0x00000001U
#define WCL_CORE_1_CURRENT_1_S  5

/** WCL_Core_1_STATUSTABLE2_REG register
 *  Status register of world switch of entry 2
 */
#define WCL_CORE_1_STATUSTABLE2_REG (DR_REG_WCL_BASE + 0x484)
/** WCL_CORE_1_FROM_WORLD_2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 2
 */
#define WCL_CORE_1_FROM_WORLD_2    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_2_M  (WCL_CORE_1_FROM_WORLD_2_V << WCL_CORE_1_FROM_WORLD_2_S)
#define WCL_CORE_1_FROM_WORLD_2_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_2_S  0
/** WCL_CORE_1_FROM_ENTRY_2 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 2
 */
#define WCL_CORE_1_FROM_ENTRY_2    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_2_M  (WCL_CORE_1_FROM_ENTRY_2_V << WCL_CORE_1_FROM_ENTRY_2_S)
#define WCL_CORE_1_FROM_ENTRY_2_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_2_S  1
/** WCL_CORE_1_CURRENT_2 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 2
 */
#define WCL_CORE_1_CURRENT_2    (BIT(5))
#define WCL_CORE_1_CURRENT_2_M  (WCL_CORE_1_CURRENT_2_V << WCL_CORE_1_CURRENT_2_S)
#define WCL_CORE_1_CURRENT_2_V  0x00000001U
#define WCL_CORE_1_CURRENT_2_S  5

/** WCL_Core_1_STATUSTABLE3_REG register
 *  Status register of world switch of entry 3
 */
#define WCL_CORE_1_STATUSTABLE3_REG (DR_REG_WCL_BASE + 0x488)
/** WCL_CORE_1_FROM_WORLD_3 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 3
 */
#define WCL_CORE_1_FROM_WORLD_3    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_3_M  (WCL_CORE_1_FROM_WORLD_3_V << WCL_CORE_1_FROM_WORLD_3_S)
#define WCL_CORE_1_FROM_WORLD_3_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_3_S  0
/** WCL_CORE_1_FROM_ENTRY_3 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 3
 */
#define WCL_CORE_1_FROM_ENTRY_3    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_3_M  (WCL_CORE_1_FROM_ENTRY_3_V << WCL_CORE_1_FROM_ENTRY_3_S)
#define WCL_CORE_1_FROM_ENTRY_3_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_3_S  1
/** WCL_CORE_1_CURRENT_3 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 3
 */
#define WCL_CORE_1_CURRENT_3    (BIT(5))
#define WCL_CORE_1_CURRENT_3_M  (WCL_CORE_1_CURRENT_3_V << WCL_CORE_1_CURRENT_3_S)
#define WCL_CORE_1_CURRENT_3_V  0x00000001U
#define WCL_CORE_1_CURRENT_3_S  5

/** WCL_Core_1_STATUSTABLE4_REG register
 *  Status register of world switch of entry 4
 */
#define WCL_CORE_1_STATUSTABLE4_REG (DR_REG_WCL_BASE + 0x48c)
/** WCL_CORE_1_FROM_WORLD_4 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 4
 */
#define WCL_CORE_1_FROM_WORLD_4    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_4_M  (WCL_CORE_1_FROM_WORLD_4_V << WCL_CORE_1_FROM_WORLD_4_S)
#define WCL_CORE_1_FROM_WORLD_4_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_4_S  0
/** WCL_CORE_1_FROM_ENTRY_4 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 4
 */
#define WCL_CORE_1_FROM_ENTRY_4    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_4_M  (WCL_CORE_1_FROM_ENTRY_4_V << WCL_CORE_1_FROM_ENTRY_4_S)
#define WCL_CORE_1_FROM_ENTRY_4_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_4_S  1
/** WCL_CORE_1_CURRENT_4 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 4
 */
#define WCL_CORE_1_CURRENT_4    (BIT(5))
#define WCL_CORE_1_CURRENT_4_M  (WCL_CORE_1_CURRENT_4_V << WCL_CORE_1_CURRENT_4_S)
#define WCL_CORE_1_CURRENT_4_V  0x00000001U
#define WCL_CORE_1_CURRENT_4_S  5

/** WCL_Core_1_STATUSTABLE5_REG register
 *  Status register of world switch of entry 5
 */
#define WCL_CORE_1_STATUSTABLE5_REG (DR_REG_WCL_BASE + 0x490)
/** WCL_CORE_1_FROM_WORLD_5 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 5
 */
#define WCL_CORE_1_FROM_WORLD_5    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_5_M  (WCL_CORE_1_FROM_WORLD_5_V << WCL_CORE_1_FROM_WORLD_5_S)
#define WCL_CORE_1_FROM_WORLD_5_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_5_S  0
/** WCL_CORE_1_FROM_ENTRY_5 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 5
 */
#define WCL_CORE_1_FROM_ENTRY_5    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_5_M  (WCL_CORE_1_FROM_ENTRY_5_V << WCL_CORE_1_FROM_ENTRY_5_S)
#define WCL_CORE_1_FROM_ENTRY_5_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_5_S  1
/** WCL_CORE_1_CURRENT_5 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 5
 */
#define WCL_CORE_1_CURRENT_5    (BIT(5))
#define WCL_CORE_1_CURRENT_5_M  (WCL_CORE_1_CURRENT_5_V << WCL_CORE_1_CURRENT_5_S)
#define WCL_CORE_1_CURRENT_5_V  0x00000001U
#define WCL_CORE_1_CURRENT_5_S  5

/** WCL_Core_1_STATUSTABLE6_REG register
 *  Status register of world switch of entry 6
 */
#define WCL_CORE_1_STATUSTABLE6_REG (DR_REG_WCL_BASE + 0x494)
/** WCL_CORE_1_FROM_WORLD_6 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 6
 */
#define WCL_CORE_1_FROM_WORLD_6    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_6_M  (WCL_CORE_1_FROM_WORLD_6_V << WCL_CORE_1_FROM_WORLD_6_S)
#define WCL_CORE_1_FROM_WORLD_6_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_6_S  0
/** WCL_CORE_1_FROM_ENTRY_6 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 6
 */
#define WCL_CORE_1_FROM_ENTRY_6    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_6_M  (WCL_CORE_1_FROM_ENTRY_6_V << WCL_CORE_1_FROM_ENTRY_6_S)
#define WCL_CORE_1_FROM_ENTRY_6_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_6_S  1
/** WCL_CORE_1_CURRENT_6 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 6
 */
#define WCL_CORE_1_CURRENT_6    (BIT(5))
#define WCL_CORE_1_CURRENT_6_M  (WCL_CORE_1_CURRENT_6_V << WCL_CORE_1_CURRENT_6_S)
#define WCL_CORE_1_CURRENT_6_V  0x00000001U
#define WCL_CORE_1_CURRENT_6_S  5

/** WCL_Core_1_STATUSTABLE7_REG register
 *  Status register of world switch of entry 7
 */
#define WCL_CORE_1_STATUSTABLE7_REG (DR_REG_WCL_BASE + 0x498)
/** WCL_CORE_1_FROM_WORLD_7 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 7
 */
#define WCL_CORE_1_FROM_WORLD_7    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_7_M  (WCL_CORE_1_FROM_WORLD_7_V << WCL_CORE_1_FROM_WORLD_7_S)
#define WCL_CORE_1_FROM_WORLD_7_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_7_S  0
/** WCL_CORE_1_FROM_ENTRY_7 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 7
 */
#define WCL_CORE_1_FROM_ENTRY_7    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_7_M  (WCL_CORE_1_FROM_ENTRY_7_V << WCL_CORE_1_FROM_ENTRY_7_S)
#define WCL_CORE_1_FROM_ENTRY_7_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_7_S  1
/** WCL_CORE_1_CURRENT_7 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 7
 */
#define WCL_CORE_1_CURRENT_7    (BIT(5))
#define WCL_CORE_1_CURRENT_7_M  (WCL_CORE_1_CURRENT_7_V << WCL_CORE_1_CURRENT_7_S)
#define WCL_CORE_1_CURRENT_7_V  0x00000001U
#define WCL_CORE_1_CURRENT_7_S  5

/** WCL_Core_1_STATUSTABLE8_REG register
 *  Status register of world switch of entry 8
 */
#define WCL_CORE_1_STATUSTABLE8_REG (DR_REG_WCL_BASE + 0x49c)
/** WCL_CORE_1_FROM_WORLD_8 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 8
 */
#define WCL_CORE_1_FROM_WORLD_8    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_8_M  (WCL_CORE_1_FROM_WORLD_8_V << WCL_CORE_1_FROM_WORLD_8_S)
#define WCL_CORE_1_FROM_WORLD_8_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_8_S  0
/** WCL_CORE_1_FROM_ENTRY_8 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 8
 */
#define WCL_CORE_1_FROM_ENTRY_8    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_8_M  (WCL_CORE_1_FROM_ENTRY_8_V << WCL_CORE_1_FROM_ENTRY_8_S)
#define WCL_CORE_1_FROM_ENTRY_8_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_8_S  1
/** WCL_CORE_1_CURRENT_8 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 8
 */
#define WCL_CORE_1_CURRENT_8    (BIT(5))
#define WCL_CORE_1_CURRENT_8_M  (WCL_CORE_1_CURRENT_8_V << WCL_CORE_1_CURRENT_8_S)
#define WCL_CORE_1_CURRENT_8_V  0x00000001U
#define WCL_CORE_1_CURRENT_8_S  5

/** WCL_Core_1_STATUSTABLE9_REG register
 *  Status register of world switch of entry 9
 */
#define WCL_CORE_1_STATUSTABLE9_REG (DR_REG_WCL_BASE + 0x4a0)
/** WCL_CORE_1_FROM_WORLD_9 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 9
 */
#define WCL_CORE_1_FROM_WORLD_9    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_9_M  (WCL_CORE_1_FROM_WORLD_9_V << WCL_CORE_1_FROM_WORLD_9_S)
#define WCL_CORE_1_FROM_WORLD_9_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_9_S  0
/** WCL_CORE_1_FROM_ENTRY_9 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 9
 */
#define WCL_CORE_1_FROM_ENTRY_9    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_9_M  (WCL_CORE_1_FROM_ENTRY_9_V << WCL_CORE_1_FROM_ENTRY_9_S)
#define WCL_CORE_1_FROM_ENTRY_9_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_9_S  1
/** WCL_CORE_1_CURRENT_9 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 9
 */
#define WCL_CORE_1_CURRENT_9    (BIT(5))
#define WCL_CORE_1_CURRENT_9_M  (WCL_CORE_1_CURRENT_9_V << WCL_CORE_1_CURRENT_9_S)
#define WCL_CORE_1_CURRENT_9_V  0x00000001U
#define WCL_CORE_1_CURRENT_9_S  5

/** WCL_Core_1_STATUSTABLE10_REG register
 *  Status register of world switch of entry 10
 */
#define WCL_CORE_1_STATUSTABLE10_REG (DR_REG_WCL_BASE + 0x4a4)
/** WCL_CORE_1_FROM_WORLD_10 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 10
 */
#define WCL_CORE_1_FROM_WORLD_10    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_10_M  (WCL_CORE_1_FROM_WORLD_10_V << WCL_CORE_1_FROM_WORLD_10_S)
#define WCL_CORE_1_FROM_WORLD_10_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_10_S  0
/** WCL_CORE_1_FROM_ENTRY_10 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 10
 */
#define WCL_CORE_1_FROM_ENTRY_10    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_10_M  (WCL_CORE_1_FROM_ENTRY_10_V << WCL_CORE_1_FROM_ENTRY_10_S)
#define WCL_CORE_1_FROM_ENTRY_10_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_10_S  1
/** WCL_CORE_1_CURRENT_10 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 10
 */
#define WCL_CORE_1_CURRENT_10    (BIT(5))
#define WCL_CORE_1_CURRENT_10_M  (WCL_CORE_1_CURRENT_10_V << WCL_CORE_1_CURRENT_10_S)
#define WCL_CORE_1_CURRENT_10_V  0x00000001U
#define WCL_CORE_1_CURRENT_10_S  5

/** WCL_Core_1_STATUSTABLE11_REG register
 *  Status register of world switch of entry 11
 */
#define WCL_CORE_1_STATUSTABLE11_REG (DR_REG_WCL_BASE + 0x4a8)
/** WCL_CORE_1_FROM_WORLD_11 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 11
 */
#define WCL_CORE_1_FROM_WORLD_11    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_11_M  (WCL_CORE_1_FROM_WORLD_11_V << WCL_CORE_1_FROM_WORLD_11_S)
#define WCL_CORE_1_FROM_WORLD_11_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_11_S  0
/** WCL_CORE_1_FROM_ENTRY_11 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 11
 */
#define WCL_CORE_1_FROM_ENTRY_11    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_11_M  (WCL_CORE_1_FROM_ENTRY_11_V << WCL_CORE_1_FROM_ENTRY_11_S)
#define WCL_CORE_1_FROM_ENTRY_11_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_11_S  1
/** WCL_CORE_1_CURRENT_11 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 11
 */
#define WCL_CORE_1_CURRENT_11    (BIT(5))
#define WCL_CORE_1_CURRENT_11_M  (WCL_CORE_1_CURRENT_11_V << WCL_CORE_1_CURRENT_11_S)
#define WCL_CORE_1_CURRENT_11_V  0x00000001U
#define WCL_CORE_1_CURRENT_11_S  5

/** WCL_Core_1_STATUSTABLE12_REG register
 *  Status register of world switch of entry 12
 */
#define WCL_CORE_1_STATUSTABLE12_REG (DR_REG_WCL_BASE + 0x4ac)
/** WCL_CORE_1_FROM_WORLD_12 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 12
 */
#define WCL_CORE_1_FROM_WORLD_12    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_12_M  (WCL_CORE_1_FROM_WORLD_12_V << WCL_CORE_1_FROM_WORLD_12_S)
#define WCL_CORE_1_FROM_WORLD_12_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_12_S  0
/** WCL_CORE_1_FROM_ENTRY_12 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 12
 */
#define WCL_CORE_1_FROM_ENTRY_12    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_12_M  (WCL_CORE_1_FROM_ENTRY_12_V << WCL_CORE_1_FROM_ENTRY_12_S)
#define WCL_CORE_1_FROM_ENTRY_12_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_12_S  1
/** WCL_CORE_1_CURRENT_12 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 12
 */
#define WCL_CORE_1_CURRENT_12    (BIT(5))
#define WCL_CORE_1_CURRENT_12_M  (WCL_CORE_1_CURRENT_12_V << WCL_CORE_1_CURRENT_12_S)
#define WCL_CORE_1_CURRENT_12_V  0x00000001U
#define WCL_CORE_1_CURRENT_12_S  5

/** WCL_Core_1_STATUSTABLE13_REG register
 *  Status register of world switch of entry 13
 */
#define WCL_CORE_1_STATUSTABLE13_REG (DR_REG_WCL_BASE + 0x4b0)
/** WCL_CORE_1_FROM_WORLD_13 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to confirm world before enter entry 13
 */
#define WCL_CORE_1_FROM_WORLD_13    (BIT(0))
#define WCL_CORE_1_FROM_WORLD_13_M  (WCL_CORE_1_FROM_WORLD_13_V << WCL_CORE_1_FROM_WORLD_13_S)
#define WCL_CORE_1_FROM_WORLD_13_V  0x00000001U
#define WCL_CORE_1_FROM_WORLD_13_S  0
/** WCL_CORE_1_FROM_ENTRY_13 : R/W; bitpos: [4:1]; default: 0;
 *  This filed is used to confirm in which entry before enter entry 13
 */
#define WCL_CORE_1_FROM_ENTRY_13    0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_13_M  (WCL_CORE_1_FROM_ENTRY_13_V << WCL_CORE_1_FROM_ENTRY_13_S)
#define WCL_CORE_1_FROM_ENTRY_13_V  0x0000000FU
#define WCL_CORE_1_FROM_ENTRY_13_S  1
/** WCL_CORE_1_CURRENT_13 : R/W; bitpos: [5]; default: 0;
 *  This bit is used to confirm whether the current state is in entry 13
 */
#define WCL_CORE_1_CURRENT_13    (BIT(5))
#define WCL_CORE_1_CURRENT_13_M  (WCL_CORE_1_CURRENT_13_V << WCL_CORE_1_CURRENT_13_S)
#define WCL_CORE_1_CURRENT_13_V  0x00000001U
#define WCL_CORE_1_CURRENT_13_S  5

/** WCL_Core_1_STATUSTABLE_CURRENT_REG register
 *  Status register of statustable current
 */
#define WCL_CORE_1_STATUSTABLE_CURRENT_REG (DR_REG_WCL_BASE + 0x4fc)
/** WCL_CORE_1_STATUSTABLE_CURRENT : R/W; bitpos: [13:1]; default: 0;
 *  This field is used to quickly read and rewrite the current field of all STATUSTABLE
 *  registers,for example,bit 1 represents the current field of STATUSTABLE1
 */
#define WCL_CORE_1_STATUSTABLE_CURRENT    0x00001FFFU
#define WCL_CORE_1_STATUSTABLE_CURRENT_M  (WCL_CORE_1_STATUSTABLE_CURRENT_V << WCL_CORE_1_STATUSTABLE_CURRENT_S)
#define WCL_CORE_1_STATUSTABLE_CURRENT_V  0x00001FFFU
#define WCL_CORE_1_STATUSTABLE_CURRENT_S  1

/** WCL_Core_1_MESSAGE_ADDR_REG register
 *  Clear writer_buffer write address configuration register
 */
#define WCL_CORE_1_MESSAGE_ADDR_REG (DR_REG_WCL_BASE + 0x500)
/** WCL_CORE_1_MESSAGE_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  This field is used to set address that need to write when enter WORLD0
 */
#define WCL_CORE_1_MESSAGE_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_MESSAGE_ADDR_M  (WCL_CORE_1_MESSAGE_ADDR_V << WCL_CORE_1_MESSAGE_ADDR_S)
#define WCL_CORE_1_MESSAGE_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_MESSAGE_ADDR_S  0

/** WCL_Core_1_MESSAGE_MAX_REG register
 *  Clear writer_buffer write number configuration register
 */
#define WCL_CORE_1_MESSAGE_MAX_REG (DR_REG_WCL_BASE + 0x504)
/** WCL_CORE_1_MESSAGE_MAX : R/W; bitpos: [3:0]; default: 0;
 *  This filed is used to set the max value of clear write_buffer
 */
#define WCL_CORE_1_MESSAGE_MAX    0x0000000FU
#define WCL_CORE_1_MESSAGE_MAX_M  (WCL_CORE_1_MESSAGE_MAX_V << WCL_CORE_1_MESSAGE_MAX_S)
#define WCL_CORE_1_MESSAGE_MAX_V  0x0000000FU
#define WCL_CORE_1_MESSAGE_MAX_S  0

/** WCL_Core_1_MESSAGE_PHASE_REG register
 *  Clear writer_buffer status register
 */
#define WCL_CORE_1_MESSAGE_PHASE_REG (DR_REG_WCL_BASE + 0x508)
/** WCL_CORE_1_MESSAGE_MATCH : RO; bitpos: [0]; default: 0;
 *  This bit indicates whether the check is successful
 */
#define WCL_CORE_1_MESSAGE_MATCH    (BIT(0))
#define WCL_CORE_1_MESSAGE_MATCH_M  (WCL_CORE_1_MESSAGE_MATCH_V << WCL_CORE_1_MESSAGE_MATCH_S)
#define WCL_CORE_1_MESSAGE_MATCH_V  0x00000001U
#define WCL_CORE_1_MESSAGE_MATCH_S  0
/** WCL_CORE_1_MESSAGE_EXPECT : RO; bitpos: [4:1]; default: 0;
 *  This field indicates the data to be written next time
 */
#define WCL_CORE_1_MESSAGE_EXPECT    0x0000000FU
#define WCL_CORE_1_MESSAGE_EXPECT_M  (WCL_CORE_1_MESSAGE_EXPECT_V << WCL_CORE_1_MESSAGE_EXPECT_S)
#define WCL_CORE_1_MESSAGE_EXPECT_V  0x0000000FU
#define WCL_CORE_1_MESSAGE_EXPECT_S  1
/** WCL_CORE_1_MESSAGE_DATAPHASE : RO; bitpos: [5]; default: 0;
 *  If this bit is 1, it means that is checking clear write_buffer operation, and is
 *  checking data
 */
#define WCL_CORE_1_MESSAGE_DATAPHASE    (BIT(5))
#define WCL_CORE_1_MESSAGE_DATAPHASE_M  (WCL_CORE_1_MESSAGE_DATAPHASE_V << WCL_CORE_1_MESSAGE_DATAPHASE_S)
#define WCL_CORE_1_MESSAGE_DATAPHASE_V  0x00000001U
#define WCL_CORE_1_MESSAGE_DATAPHASE_S  5
/** WCL_CORE_1_MESSAGE_ADDRESSPHASE : RO; bitpos: [6]; default: 0;
 *  If this bit is 1, it means that is checking clear write_buffer operation, and is
 *  checking address.
 */
#define WCL_CORE_1_MESSAGE_ADDRESSPHASE    (BIT(6))
#define WCL_CORE_1_MESSAGE_ADDRESSPHASE_M  (WCL_CORE_1_MESSAGE_ADDRESSPHASE_V << WCL_CORE_1_MESSAGE_ADDRESSPHASE_S)
#define WCL_CORE_1_MESSAGE_ADDRESSPHASE_V  0x00000001U
#define WCL_CORE_1_MESSAGE_ADDRESSPHASE_S  6

/** WCL_Core_1_World_TRIGGER_ADDR_REG register
 *  Core_1 trigger address configuration Register
 */
#define WCL_CORE_1_WORLD_TRIGGER_ADDR_REG (DR_REG_WCL_BASE + 0x540)
/** WCL_CORE_1_WORLD_TRIGGER_ADDR : RW; bitpos: [31:0]; default: 0;
 *  This field is used to configure the entry address from WORLD0 to WORLD1,when the
 *  CPU executes to this address,switch to WORLD1
 */
#define WCL_CORE_1_WORLD_TRIGGER_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_WORLD_TRIGGER_ADDR_M  (WCL_CORE_1_WORLD_TRIGGER_ADDR_V << WCL_CORE_1_WORLD_TRIGGER_ADDR_S)
#define WCL_CORE_1_WORLD_TRIGGER_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_WORLD_TRIGGER_ADDR_S  0

/** WCL_Core_1_World_PREPARE_REG register
 *  Core_1 prepare world configuration Register
 */
#define WCL_CORE_1_WORLD_PREPARE_REG (DR_REG_WCL_BASE + 0x544)
/** WCL_CORE_1_WORLD_PREPARE : R/W; bitpos: [1:0]; default: 0;
 *  This field to used to set world to enter,2'b01 means WORLD0, 2'b10 means WORLD1
 */
#define WCL_CORE_1_WORLD_PREPARE    0x00000003U
#define WCL_CORE_1_WORLD_PREPARE_M  (WCL_CORE_1_WORLD_PREPARE_V << WCL_CORE_1_WORLD_PREPARE_S)
#define WCL_CORE_1_WORLD_PREPARE_V  0x00000003U
#define WCL_CORE_1_WORLD_PREPARE_S  0

/** WCL_Core_1_World_UPDATE_REG register
 *  Core_1 configuration update register
 */
#define WCL_CORE_1_WORLD_UPDATE_REG (DR_REG_WCL_BASE + 0x548)
/** WCL_CORE_1_UPDATE : WO; bitpos: [31:0]; default: 0;
 *  This field is used to update configuration completed, can write any value,the
 *  hardware only checks the write operation of this register and does not case about
 *  its value
 */
#define WCL_CORE_1_UPDATE    0xFFFFFFFFU
#define WCL_CORE_1_UPDATE_M  (WCL_CORE_1_UPDATE_V << WCL_CORE_1_UPDATE_S)
#define WCL_CORE_1_UPDATE_V  0xFFFFFFFFU
#define WCL_CORE_1_UPDATE_S  0

/** WCL_Core_1_World_Cancel_REG register
 *  Core_1 configuration cancel register
 */
#define WCL_CORE_1_WORLD_CANCEL_REG (DR_REG_WCL_BASE + 0x54c)
/** WCL_CORE_1_WORLD_CANCEL : WO; bitpos: [31:0]; default: 0;
 *  This field is used to cancel switch world configuration,if the trigger address and
 *  update configuration complete,can use this register to cancel world switch. can
 *  write any value, the hardware only checks the write operation of this register and
 *  does not case about its value
 */
#define WCL_CORE_1_WORLD_CANCEL    0xFFFFFFFFU
#define WCL_CORE_1_WORLD_CANCEL_M  (WCL_CORE_1_WORLD_CANCEL_V << WCL_CORE_1_WORLD_CANCEL_S)
#define WCL_CORE_1_WORLD_CANCEL_V  0xFFFFFFFFU
#define WCL_CORE_1_WORLD_CANCEL_S  0

/** WCL_Core_1_World_IRam0_REG register
 *  Core_1 Iram0 world register
 */
#define WCL_CORE_1_WORLD_IRAM0_REG (DR_REG_WCL_BASE + 0x550)
/** WCL_CORE_1_WORLD_IRAM0 : R/W; bitpos: [1:0]; default: 0;
 *  this field is used to read current world of Iram0 bus
 */
#define WCL_CORE_1_WORLD_IRAM0    0x00000003U
#define WCL_CORE_1_WORLD_IRAM0_M  (WCL_CORE_1_WORLD_IRAM0_V << WCL_CORE_1_WORLD_IRAM0_S)
#define WCL_CORE_1_WORLD_IRAM0_V  0x00000003U
#define WCL_CORE_1_WORLD_IRAM0_S  0

/** WCL_Core_1_World_DRam0_PIF_REG register
 *  Core_1 dram0 and PIF world register
 */
#define WCL_CORE_1_WORLD_DRAM0_PIF_REG (DR_REG_WCL_BASE + 0x554)
/** WCL_CORE_1_WORLD_DRAM0_PIF : R/W; bitpos: [1:0]; default: 0;
 *  this field is used to read current world of Dram0 bus and PIF bus
 */
#define WCL_CORE_1_WORLD_DRAM0_PIF    0x00000003U
#define WCL_CORE_1_WORLD_DRAM0_PIF_M  (WCL_CORE_1_WORLD_DRAM0_PIF_V << WCL_CORE_1_WORLD_DRAM0_PIF_S)
#define WCL_CORE_1_WORLD_DRAM0_PIF_V  0x00000003U
#define WCL_CORE_1_WORLD_DRAM0_PIF_S  0

/** WCL_Core_1_World_Phase_REG register
 *  Core_0 world status register
 */
#define WCL_CORE_1_WORLD_PHASE_REG (DR_REG_WCL_BASE + 0x558)
/** WCL_CORE_1_WORLD_PHASE : RO; bitpos: [0]; default: 0;
 *  This bit indicates whether is preparing to switch to WORLD1,1 means value.
 */
#define WCL_CORE_1_WORLD_PHASE    (BIT(0))
#define WCL_CORE_1_WORLD_PHASE_M  (WCL_CORE_1_WORLD_PHASE_V << WCL_CORE_1_WORLD_PHASE_S)
#define WCL_CORE_1_WORLD_PHASE_V  0x00000001U
#define WCL_CORE_1_WORLD_PHASE_S  0

/** WCL_Core_1_NMI_MASK_ENABLE_REG register
 *  Core_1 NMI mask enable register
 */
#define WCL_CORE_1_NMI_MASK_ENABLE_REG (DR_REG_WCL_BASE + 0x580)
/** WCL_CORE_1_NMI_MASK_ENABLE : WO; bitpos: [31:0]; default: 0;
 *  this field is used to set NMI mask, it can write any value, when write this
 *  register,the hardware start masking NMI interrupt
 */
#define WCL_CORE_1_NMI_MASK_ENABLE    0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_ENABLE_M  (WCL_CORE_1_NMI_MASK_ENABLE_V << WCL_CORE_1_NMI_MASK_ENABLE_S)
#define WCL_CORE_1_NMI_MASK_ENABLE_V  0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_ENABLE_S  0

/** WCL_Core_1_NMI_MASK_TRIGGER_ADDR_REG register
 *  Core_1 NMI mask trigger addr register
 */
#define WCL_CORE_1_NMI_MASK_TRIGGER_ADDR_REG (DR_REG_WCL_BASE + 0x584)
/** WCL_CORE_1_NMI_MASK_TRIGGER_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  this field to used to set trigger address
 */
#define WCL_CORE_1_NMI_MASK_TRIGGER_ADDR    0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_TRIGGER_ADDR_M  (WCL_CORE_1_NMI_MASK_TRIGGER_ADDR_V << WCL_CORE_1_NMI_MASK_TRIGGER_ADDR_S)
#define WCL_CORE_1_NMI_MASK_TRIGGER_ADDR_V  0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_TRIGGER_ADDR_S  0

/** WCL_Core_1_NMI_MASK_DISABLE_REG register
 *  Core_1 NMI mask disable register
 */
#define WCL_CORE_1_NMI_MASK_DISABLE_REG (DR_REG_WCL_BASE + 0x588)
/** WCL_CORE_1_NMI_MASK_DISABLE : WO; bitpos: [31:0]; default: 0;
 *  this field is used to disable NMI mask, it will not take effect immediately,only
 *  when the CPU executes to the trigger address will it start to cancel NMI mask
 */
#define WCL_CORE_1_NMI_MASK_DISABLE    0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_DISABLE_M  (WCL_CORE_1_NMI_MASK_DISABLE_V << WCL_CORE_1_NMI_MASK_DISABLE_S)
#define WCL_CORE_1_NMI_MASK_DISABLE_V  0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_DISABLE_S  0

/** WCL_Core_1_NMI_MASK_CANCLE_REG register
 *  Core_1 NMI mask disable register
 */
#define WCL_CORE_1_NMI_MASK_CANCLE_REG (DR_REG_WCL_BASE + 0x58c)
/** WCL_CORE_1_NMI_MASK_CANCEL : WO; bitpos: [31:0]; default: 0;
 *  this field is used to cancel NMI mask disable function.
 */
#define WCL_CORE_1_NMI_MASK_CANCEL    0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_CANCEL_M  (WCL_CORE_1_NMI_MASK_CANCEL_V << WCL_CORE_1_NMI_MASK_CANCEL_S)
#define WCL_CORE_1_NMI_MASK_CANCEL_V  0xFFFFFFFFU
#define WCL_CORE_1_NMI_MASK_CANCEL_S  0

/** WCL_Core_1_NMI_MASK_REG register
 *  Core_1 NMI mask register
 */
#define WCL_CORE_1_NMI_MASK_REG (DR_REG_WCL_BASE + 0x590)
/** WCL_CORE_1_NMI_MASK : R/W; bitpos: [0]; default: 0;
 *  this bit is used to mask NMI interrupt,it can directly mask NMI interrupt
 */
#define WCL_CORE_1_NMI_MASK    (BIT(0))
#define WCL_CORE_1_NMI_MASK_M  (WCL_CORE_1_NMI_MASK_V << WCL_CORE_1_NMI_MASK_S)
#define WCL_CORE_1_NMI_MASK_V  0x00000001U
#define WCL_CORE_1_NMI_MASK_S  0

/** WCL_Core_1_NMI_MASK_PHASE_REG register
 *  Core_1 NMI mask phase register
 */
#define WCL_CORE_1_NMI_MASK_PHASE_REG (DR_REG_WCL_BASE + 0x594)
/** WCL_CORE_1_NMI_MASK_PHASE : RO; bitpos: [0]; default: 0;
 *  this bit is used to indicates whether the NMI interrupt is being masked, 1 means
 *  NMI interrupt is being masked
 */
#define WCL_CORE_1_NMI_MASK_PHASE    (BIT(0))
#define WCL_CORE_1_NMI_MASK_PHASE_M  (WCL_CORE_1_NMI_MASK_PHASE_V << WCL_CORE_1_NMI_MASK_PHASE_S)
#define WCL_CORE_1_NMI_MASK_PHASE_V  0x00000001U
#define WCL_CORE_1_NMI_MASK_PHASE_S  0

#ifdef __cplusplus
}
#endif
