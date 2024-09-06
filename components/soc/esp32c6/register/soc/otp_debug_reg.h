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

/** OTP_DEBUG_WR_DIS_REG register
 *  Otp debugger block0 data register1.
 */
#define OTP_DEBUG_WR_DIS_REG (DR_REG_OTP_DEBUG_BASE + 0x0)
/** OTP_DEBUG_BLOCK0_WR_DIS : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 write disable data.
 */
#define OTP_DEBUG_BLOCK0_WR_DIS    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_WR_DIS_M  (OTP_DEBUG_BLOCK0_WR_DIS_V << OTP_DEBUG_BLOCK0_WR_DIS_S)
#define OTP_DEBUG_BLOCK0_WR_DIS_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_WR_DIS_S  0

/** OTP_DEBUG_BLK0_BACKUP1_W1_REG register
 *  Otp debugger block0 data register2.
 */
#define OTP_DEBUG_BLK0_BACKUP1_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x4)
/** OTP_DEBUG_BLOCK0_BACKUP1_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word1 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP1_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W1_M  (OTP_DEBUG_BLOCK0_BACKUP1_W1_V << OTP_DEBUG_BLOCK0_BACKUP1_W1_S)
#define OTP_DEBUG_BLOCK0_BACKUP1_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W1_S  0

/** OTP_DEBUG_BLK0_BACKUP1_W2_REG register
 *  Otp debugger block0 data register3.
 */
#define OTP_DEBUG_BLK0_BACKUP1_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x8)
/** OTP_DEBUG_BLOCK0_BACKUP1_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word2 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP1_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W2_M  (OTP_DEBUG_BLOCK0_BACKUP1_W2_V << OTP_DEBUG_BLOCK0_BACKUP1_W2_S)
#define OTP_DEBUG_BLOCK0_BACKUP1_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W2_S  0

/** OTP_DEBUG_BLK0_BACKUP1_W3_REG register
 *  Otp debugger block0 data register4.
 */
#define OTP_DEBUG_BLK0_BACKUP1_W3_REG (DR_REG_OTP_DEBUG_BASE + 0xc)
/** OTP_DEBUG_BLOCK0_BACKUP1_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word3 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP1_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W3_M  (OTP_DEBUG_BLOCK0_BACKUP1_W3_V << OTP_DEBUG_BLOCK0_BACKUP1_W3_S)
#define OTP_DEBUG_BLOCK0_BACKUP1_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W3_S  0

/** OTP_DEBUG_BLK0_BACKUP1_W4_REG register
 *  Otp debugger block0 data register5.
 */
#define OTP_DEBUG_BLK0_BACKUP1_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x10)
/** OTP_DEBUG_BLOCK0_BACKUP1_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word4 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP1_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W4_M  (OTP_DEBUG_BLOCK0_BACKUP1_W4_V << OTP_DEBUG_BLOCK0_BACKUP1_W4_S)
#define OTP_DEBUG_BLOCK0_BACKUP1_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W4_S  0

/** OTP_DEBUG_BLK0_BACKUP1_W5_REG register
 *  Otp debugger block0 data register6.
 */
#define OTP_DEBUG_BLK0_BACKUP1_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x14)
/** OTP_DEBUG_BLOCK0_BACKUP1_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word5 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP1_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W5_M  (OTP_DEBUG_BLOCK0_BACKUP1_W5_V << OTP_DEBUG_BLOCK0_BACKUP1_W5_S)
#define OTP_DEBUG_BLOCK0_BACKUP1_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP1_W5_S  0

/** OTP_DEBUG_BLK0_BACKUP2_W1_REG register
 *  Otp debugger block0 data register7.
 */
#define OTP_DEBUG_BLK0_BACKUP2_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x18)
/** OTP_DEBUG_BLOCK0_BACKUP2_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word1 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP2_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W1_M  (OTP_DEBUG_BLOCK0_BACKUP2_W1_V << OTP_DEBUG_BLOCK0_BACKUP2_W1_S)
#define OTP_DEBUG_BLOCK0_BACKUP2_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W1_S  0

/** OTP_DEBUG_BLK0_BACKUP2_W2_REG register
 *  Otp debugger block0 data register8.
 */
#define OTP_DEBUG_BLK0_BACKUP2_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x1c)
/** OTP_DEBUG_BLOCK0_BACKUP2_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word2 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP2_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W2_M  (OTP_DEBUG_BLOCK0_BACKUP2_W2_V << OTP_DEBUG_BLOCK0_BACKUP2_W2_S)
#define OTP_DEBUG_BLOCK0_BACKUP2_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W2_S  0

/** OTP_DEBUG_BLK0_BACKUP2_W3_REG register
 *  Otp debugger block0 data register9.
 */
#define OTP_DEBUG_BLK0_BACKUP2_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x20)
/** OTP_DEBUG_BLOCK0_BACKUP2_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word3 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP2_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W3_M  (OTP_DEBUG_BLOCK0_BACKUP2_W3_V << OTP_DEBUG_BLOCK0_BACKUP2_W3_S)
#define OTP_DEBUG_BLOCK0_BACKUP2_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W3_S  0

/** OTP_DEBUG_BLK0_BACKUP2_W4_REG register
 *  Otp debugger block0 data register10.
 */
#define OTP_DEBUG_BLK0_BACKUP2_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x24)
/** OTP_DEBUG_BLOCK0_BACKUP2_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word4 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP2_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W4_M  (OTP_DEBUG_BLOCK0_BACKUP2_W4_V << OTP_DEBUG_BLOCK0_BACKUP2_W4_S)
#define OTP_DEBUG_BLOCK0_BACKUP2_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W4_S  0

/** OTP_DEBUG_BLK0_BACKUP2_W5_REG register
 *  Otp debugger block0 data register11.
 */
#define OTP_DEBUG_BLK0_BACKUP2_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x28)
/** OTP_DEBUG_BLOCK0_BACKUP2_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word5 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP2_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W5_M  (OTP_DEBUG_BLOCK0_BACKUP2_W5_V << OTP_DEBUG_BLOCK0_BACKUP2_W5_S)
#define OTP_DEBUG_BLOCK0_BACKUP2_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP2_W5_S  0

/** OTP_DEBUG_BLK0_BACKUP3_W1_REG register
 *  Otp debugger block0 data register12.
 */
#define OTP_DEBUG_BLK0_BACKUP3_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x2c)
/** OTP_DEBUG_BLOCK0_BACKUP3_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word1 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP3_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W1_M  (OTP_DEBUG_BLOCK0_BACKUP3_W1_V << OTP_DEBUG_BLOCK0_BACKUP3_W1_S)
#define OTP_DEBUG_BLOCK0_BACKUP3_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W1_S  0

/** OTP_DEBUG_BLK0_BACKUP3_W2_REG register
 *  Otp debugger block0 data register13.
 */
#define OTP_DEBUG_BLK0_BACKUP3_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x30)
/** OTP_DEBUG_BLOCK0_BACKUP3_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word2 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP3_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W2_M  (OTP_DEBUG_BLOCK0_BACKUP3_W2_V << OTP_DEBUG_BLOCK0_BACKUP3_W2_S)
#define OTP_DEBUG_BLOCK0_BACKUP3_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W2_S  0

/** OTP_DEBUG_BLK0_BACKUP3_W3_REG register
 *  Otp debugger block0 data register14.
 */
#define OTP_DEBUG_BLK0_BACKUP3_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x34)
/** OTP_DEBUG_BLOCK0_BACKUP3_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word3 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP3_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W3_M  (OTP_DEBUG_BLOCK0_BACKUP3_W3_V << OTP_DEBUG_BLOCK0_BACKUP3_W3_S)
#define OTP_DEBUG_BLOCK0_BACKUP3_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W3_S  0

/** OTP_DEBUG_BLK0_BACKUP3_W4_REG register
 *  Otp debugger block0 data register15.
 */
#define OTP_DEBUG_BLK0_BACKUP3_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x38)
/** OTP_DEBUG_BLOCK0_BACKUP3_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word4 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP3_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W4_M  (OTP_DEBUG_BLOCK0_BACKUP3_W4_V << OTP_DEBUG_BLOCK0_BACKUP3_W4_S)
#define OTP_DEBUG_BLOCK0_BACKUP3_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W4_S  0

/** OTP_DEBUG_BLK0_BACKUP3_W5_REG register
 *  Otp debugger block0 data register16.
 */
#define OTP_DEBUG_BLK0_BACKUP3_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x3c)
/** OTP_DEBUG_BLOCK0_BACKUP3_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word5 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP3_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W5_M  (OTP_DEBUG_BLOCK0_BACKUP3_W5_V << OTP_DEBUG_BLOCK0_BACKUP3_W5_S)
#define OTP_DEBUG_BLOCK0_BACKUP3_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP3_W5_S  0

/** OTP_DEBUG_BLK0_BACKUP4_W1_REG register
 *  Otp debugger block0 data register17.
 */
#define OTP_DEBUG_BLK0_BACKUP4_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x40)
/** OTP_DEBUG_BLOCK0_BACKUP4_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word1 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP4_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W1_M  (OTP_DEBUG_BLOCK0_BACKUP4_W1_V << OTP_DEBUG_BLOCK0_BACKUP4_W1_S)
#define OTP_DEBUG_BLOCK0_BACKUP4_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W1_S  0

/** OTP_DEBUG_BLK0_BACKUP4_W2_REG register
 *  Otp debugger block0 data register18.
 */
#define OTP_DEBUG_BLK0_BACKUP4_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x44)
/** OTP_DEBUG_BLOCK0_BACKUP4_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word2 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP4_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W2_M  (OTP_DEBUG_BLOCK0_BACKUP4_W2_V << OTP_DEBUG_BLOCK0_BACKUP4_W2_S)
#define OTP_DEBUG_BLOCK0_BACKUP4_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W2_S  0

/** OTP_DEBUG_BLK0_BACKUP4_W3_REG register
 *  Otp debugger block0 data register19.
 */
#define OTP_DEBUG_BLK0_BACKUP4_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x48)
/** OTP_DEBUG_BLOCK0_BACKUP4_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word3 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP4_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W3_M  (OTP_DEBUG_BLOCK0_BACKUP4_W3_V << OTP_DEBUG_BLOCK0_BACKUP4_W3_S)
#define OTP_DEBUG_BLOCK0_BACKUP4_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W3_S  0

/** OTP_DEBUG_BLK0_BACKUP4_W4_REG register
 *  Otp debugger block0 data register20.
 */
#define OTP_DEBUG_BLK0_BACKUP4_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x4c)
/** OTP_DEBUG_BLOCK0_BACKUP4_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word4 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP4_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W4_M  (OTP_DEBUG_BLOCK0_BACKUP4_W4_V << OTP_DEBUG_BLOCK0_BACKUP4_W4_S)
#define OTP_DEBUG_BLOCK0_BACKUP4_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W4_S  0

/** OTP_DEBUG_BLK0_BACKUP4_W5_REG register
 *  Otp debugger block0 data register21.
 */
#define OTP_DEBUG_BLK0_BACKUP4_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x50)
/** OTP_DEBUG_BLOCK0_BACKUP4_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word5 data.
 */
#define OTP_DEBUG_BLOCK0_BACKUP4_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W5_M  (OTP_DEBUG_BLOCK0_BACKUP4_W5_V << OTP_DEBUG_BLOCK0_BACKUP4_W5_S)
#define OTP_DEBUG_BLOCK0_BACKUP4_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK0_BACKUP4_W5_S  0

/** OTP_DEBUG_BLK1_W1_REG register
 *  Otp debugger block1 data register1.
 */
#define OTP_DEBUG_BLK1_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x54)
/** OTP_DEBUG_BLOCK1_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word1 data.
 */
#define OTP_DEBUG_BLOCK1_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W1_M  (OTP_DEBUG_BLOCK1_W1_V << OTP_DEBUG_BLOCK1_W1_S)
#define OTP_DEBUG_BLOCK1_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W1_S  0

/** OTP_DEBUG_BLK1_W2_REG register
 *  Otp debugger block1 data register2.
 */
#define OTP_DEBUG_BLK1_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x58)
/** OTP_DEBUG_BLOCK1_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word2 data.
 */
#define OTP_DEBUG_BLOCK1_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W2_M  (OTP_DEBUG_BLOCK1_W2_V << OTP_DEBUG_BLOCK1_W2_S)
#define OTP_DEBUG_BLOCK1_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W2_S  0

/** OTP_DEBUG_BLK1_W3_REG register
 *  Otp debugger block1 data register3.
 */
#define OTP_DEBUG_BLK1_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x5c)
/** OTP_DEBUG_BLOCK1_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word3 data.
 */
#define OTP_DEBUG_BLOCK1_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W3_M  (OTP_DEBUG_BLOCK1_W3_V << OTP_DEBUG_BLOCK1_W3_S)
#define OTP_DEBUG_BLOCK1_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W3_S  0

/** OTP_DEBUG_BLK1_W4_REG register
 *  Otp debugger block1 data register4.
 */
#define OTP_DEBUG_BLK1_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x60)
/** OTP_DEBUG_BLOCK1_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word4 data.
 */
#define OTP_DEBUG_BLOCK1_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W4_M  (OTP_DEBUG_BLOCK1_W4_V << OTP_DEBUG_BLOCK1_W4_S)
#define OTP_DEBUG_BLOCK1_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W4_S  0

/** OTP_DEBUG_BLK1_W5_REG register
 *  Otp debugger block1 data register5.
 */
#define OTP_DEBUG_BLK1_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x64)
/** OTP_DEBUG_BLOCK1_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word5 data.
 */
#define OTP_DEBUG_BLOCK1_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W5_M  (OTP_DEBUG_BLOCK1_W5_V << OTP_DEBUG_BLOCK1_W5_S)
#define OTP_DEBUG_BLOCK1_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W5_S  0

/** OTP_DEBUG_BLK1_W6_REG register
 *  Otp debugger block1 data register6.
 */
#define OTP_DEBUG_BLK1_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x68)
/** OTP_DEBUG_BLOCK1_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word6 data.
 */
#define OTP_DEBUG_BLOCK1_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W6_M  (OTP_DEBUG_BLOCK1_W6_V << OTP_DEBUG_BLOCK1_W6_S)
#define OTP_DEBUG_BLOCK1_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W6_S  0

/** OTP_DEBUG_BLK1_W7_REG register
 *  Otp debugger block1 data register7.
 */
#define OTP_DEBUG_BLK1_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x6c)
/** OTP_DEBUG_BLOCK1_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word7 data.
 */
#define OTP_DEBUG_BLOCK1_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W7_M  (OTP_DEBUG_BLOCK1_W7_V << OTP_DEBUG_BLOCK1_W7_S)
#define OTP_DEBUG_BLOCK1_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W7_S  0

/** OTP_DEBUG_BLK1_W8_REG register
 *  Otp debugger block1 data register8.
 */
#define OTP_DEBUG_BLK1_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x70)
/** OTP_DEBUG_BLOCK1_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word8 data.
 */
#define OTP_DEBUG_BLOCK1_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W8_M  (OTP_DEBUG_BLOCK1_W8_V << OTP_DEBUG_BLOCK1_W8_S)
#define OTP_DEBUG_BLOCK1_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W8_S  0

/** OTP_DEBUG_BLK1_W9_REG register
 *  Otp debugger block1 data register9.
 */
#define OTP_DEBUG_BLK1_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x74)
/** OTP_DEBUG_BLOCK1_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word9 data.
 */
#define OTP_DEBUG_BLOCK1_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W9_M  (OTP_DEBUG_BLOCK1_W9_V << OTP_DEBUG_BLOCK1_W9_S)
#define OTP_DEBUG_BLOCK1_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK1_W9_S  0

/** OTP_DEBUG_BLK2_W1_REG register
 *  Otp debugger block2 data register1.
 */
#define OTP_DEBUG_BLK2_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x78)
/** OTP_DEBUG_BLOCK2_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word1 data.
 */
#define OTP_DEBUG_BLOCK2_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W1_M  (OTP_DEBUG_BLOCK2_W1_V << OTP_DEBUG_BLOCK2_W1_S)
#define OTP_DEBUG_BLOCK2_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W1_S  0

/** OTP_DEBUG_BLK2_W2_REG register
 *  Otp debugger block2 data register2.
 */
#define OTP_DEBUG_BLK2_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x7c)
/** OTP_DEBUG_BLOCK2_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word2 data.
 */
#define OTP_DEBUG_BLOCK2_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W2_M  (OTP_DEBUG_BLOCK2_W2_V << OTP_DEBUG_BLOCK2_W2_S)
#define OTP_DEBUG_BLOCK2_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W2_S  0

/** OTP_DEBUG_BLK2_W3_REG register
 *  Otp debugger block2 data register3.
 */
#define OTP_DEBUG_BLK2_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x80)
/** OTP_DEBUG_BLOCK2_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word3 data.
 */
#define OTP_DEBUG_BLOCK2_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W3_M  (OTP_DEBUG_BLOCK2_W3_V << OTP_DEBUG_BLOCK2_W3_S)
#define OTP_DEBUG_BLOCK2_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W3_S  0

/** OTP_DEBUG_BLK2_W4_REG register
 *  Otp debugger block2 data register4.
 */
#define OTP_DEBUG_BLK2_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x84)
/** OTP_DEBUG_BLOCK2_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word4 data.
 */
#define OTP_DEBUG_BLOCK2_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W4_M  (OTP_DEBUG_BLOCK2_W4_V << OTP_DEBUG_BLOCK2_W4_S)
#define OTP_DEBUG_BLOCK2_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W4_S  0

/** OTP_DEBUG_BLK2_W5_REG register
 *  Otp debugger block2 data register5.
 */
#define OTP_DEBUG_BLK2_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x88)
/** OTP_DEBUG_BLOCK2_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word5 data.
 */
#define OTP_DEBUG_BLOCK2_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W5_M  (OTP_DEBUG_BLOCK2_W5_V << OTP_DEBUG_BLOCK2_W5_S)
#define OTP_DEBUG_BLOCK2_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W5_S  0

/** OTP_DEBUG_BLK2_W6_REG register
 *  Otp debugger block2 data register6.
 */
#define OTP_DEBUG_BLK2_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x8c)
/** OTP_DEBUG_BLOCK2_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word6 data.
 */
#define OTP_DEBUG_BLOCK2_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W6_M  (OTP_DEBUG_BLOCK2_W6_V << OTP_DEBUG_BLOCK2_W6_S)
#define OTP_DEBUG_BLOCK2_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W6_S  0

/** OTP_DEBUG_BLK2_W7_REG register
 *  Otp debugger block2 data register7.
 */
#define OTP_DEBUG_BLK2_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x90)
/** OTP_DEBUG_BLOCK2_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word7 data.
 */
#define OTP_DEBUG_BLOCK2_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W7_M  (OTP_DEBUG_BLOCK2_W7_V << OTP_DEBUG_BLOCK2_W7_S)
#define OTP_DEBUG_BLOCK2_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W7_S  0

/** OTP_DEBUG_BLK2_W8_REG register
 *  Otp debugger block2 data register8.
 */
#define OTP_DEBUG_BLK2_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x94)
/** OTP_DEBUG_BLOCK2_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word8 data.
 */
#define OTP_DEBUG_BLOCK2_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W8_M  (OTP_DEBUG_BLOCK2_W8_V << OTP_DEBUG_BLOCK2_W8_S)
#define OTP_DEBUG_BLOCK2_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W8_S  0

/** OTP_DEBUG_BLK2_W9_REG register
 *  Otp debugger block2 data register9.
 */
#define OTP_DEBUG_BLK2_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x98)
/** OTP_DEBUG_BLOCK2_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word9 data.
 */
#define OTP_DEBUG_BLOCK2_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W9_M  (OTP_DEBUG_BLOCK2_W9_V << OTP_DEBUG_BLOCK2_W9_S)
#define OTP_DEBUG_BLOCK2_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W9_S  0

/** OTP_DEBUG_BLK2_W10_REG register
 *  Otp debugger block2 data register10.
 */
#define OTP_DEBUG_BLK2_W10_REG (DR_REG_OTP_DEBUG_BASE + 0x9c)
/** OTP_DEBUG_BLOCK2_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word10 data.
 */
#define OTP_DEBUG_BLOCK2_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W10_M  (OTP_DEBUG_BLOCK2_W10_V << OTP_DEBUG_BLOCK2_W10_S)
#define OTP_DEBUG_BLOCK2_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W10_S  0

/** OTP_DEBUG_BLK2_W11_REG register
 *  Otp debugger block2 data register11.
 */
#define OTP_DEBUG_BLK2_W11_REG (DR_REG_OTP_DEBUG_BASE + 0xa0)
/** OTP_DEBUG_BLOCK2_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word11 data.
 */
#define OTP_DEBUG_BLOCK2_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W11_M  (OTP_DEBUG_BLOCK2_W11_V << OTP_DEBUG_BLOCK2_W11_S)
#define OTP_DEBUG_BLOCK2_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK2_W11_S  0

/** OTP_DEBUG_BLK3_W1_REG register
 *  Otp debugger block3 data register1.
 */
#define OTP_DEBUG_BLK3_W1_REG (DR_REG_OTP_DEBUG_BASE + 0xa4)
/** OTP_DEBUG_BLOCK3_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word1 data.
 */
#define OTP_DEBUG_BLOCK3_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W1_M  (OTP_DEBUG_BLOCK3_W1_V << OTP_DEBUG_BLOCK3_W1_S)
#define OTP_DEBUG_BLOCK3_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W1_S  0

/** OTP_DEBUG_BLK3_W2_REG register
 *  Otp debugger block3 data register2.
 */
#define OTP_DEBUG_BLK3_W2_REG (DR_REG_OTP_DEBUG_BASE + 0xa8)
/** OTP_DEBUG_BLOCK3_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word2 data.
 */
#define OTP_DEBUG_BLOCK3_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W2_M  (OTP_DEBUG_BLOCK3_W2_V << OTP_DEBUG_BLOCK3_W2_S)
#define OTP_DEBUG_BLOCK3_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W2_S  0

/** OTP_DEBUG_BLK3_W3_REG register
 *  Otp debugger block3 data register3.
 */
#define OTP_DEBUG_BLK3_W3_REG (DR_REG_OTP_DEBUG_BASE + 0xac)
/** OTP_DEBUG_BLOCK3_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word3 data.
 */
#define OTP_DEBUG_BLOCK3_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W3_M  (OTP_DEBUG_BLOCK3_W3_V << OTP_DEBUG_BLOCK3_W3_S)
#define OTP_DEBUG_BLOCK3_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W3_S  0

/** OTP_DEBUG_BLK3_W4_REG register
 *  Otp debugger block3 data register4.
 */
#define OTP_DEBUG_BLK3_W4_REG (DR_REG_OTP_DEBUG_BASE + 0xb0)
/** OTP_DEBUG_BLOCK3_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word4 data.
 */
#define OTP_DEBUG_BLOCK3_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W4_M  (OTP_DEBUG_BLOCK3_W4_V << OTP_DEBUG_BLOCK3_W4_S)
#define OTP_DEBUG_BLOCK3_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W4_S  0

/** OTP_DEBUG_BLK3_W5_REG register
 *  Otp debugger block3 data register5.
 */
#define OTP_DEBUG_BLK3_W5_REG (DR_REG_OTP_DEBUG_BASE + 0xb4)
/** OTP_DEBUG_BLOCK3_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word5 data.
 */
#define OTP_DEBUG_BLOCK3_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W5_M  (OTP_DEBUG_BLOCK3_W5_V << OTP_DEBUG_BLOCK3_W5_S)
#define OTP_DEBUG_BLOCK3_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W5_S  0

/** OTP_DEBUG_BLK3_W6_REG register
 *  Otp debugger block3 data register6.
 */
#define OTP_DEBUG_BLK3_W6_REG (DR_REG_OTP_DEBUG_BASE + 0xb8)
/** OTP_DEBUG_BLOCK3_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word6 data.
 */
#define OTP_DEBUG_BLOCK3_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W6_M  (OTP_DEBUG_BLOCK3_W6_V << OTP_DEBUG_BLOCK3_W6_S)
#define OTP_DEBUG_BLOCK3_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W6_S  0

/** OTP_DEBUG_BLK3_W7_REG register
 *  Otp debugger block3 data register7.
 */
#define OTP_DEBUG_BLK3_W7_REG (DR_REG_OTP_DEBUG_BASE + 0xbc)
/** OTP_DEBUG_BLOCK3_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word7 data.
 */
#define OTP_DEBUG_BLOCK3_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W7_M  (OTP_DEBUG_BLOCK3_W7_V << OTP_DEBUG_BLOCK3_W7_S)
#define OTP_DEBUG_BLOCK3_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W7_S  0

/** OTP_DEBUG_BLK3_W8_REG register
 *  Otp debugger block3 data register8.
 */
#define OTP_DEBUG_BLK3_W8_REG (DR_REG_OTP_DEBUG_BASE + 0xc0)
/** OTP_DEBUG_BLOCK3_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word8 data.
 */
#define OTP_DEBUG_BLOCK3_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W8_M  (OTP_DEBUG_BLOCK3_W8_V << OTP_DEBUG_BLOCK3_W8_S)
#define OTP_DEBUG_BLOCK3_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W8_S  0

/** OTP_DEBUG_BLK3_W9_REG register
 *  Otp debugger block3 data register9.
 */
#define OTP_DEBUG_BLK3_W9_REG (DR_REG_OTP_DEBUG_BASE + 0xc4)
/** OTP_DEBUG_BLOCK3_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word9 data.
 */
#define OTP_DEBUG_BLOCK3_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W9_M  (OTP_DEBUG_BLOCK3_W9_V << OTP_DEBUG_BLOCK3_W9_S)
#define OTP_DEBUG_BLOCK3_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W9_S  0

/** OTP_DEBUG_BLK3_W10_REG register
 *  Otp debugger block3 data register10.
 */
#define OTP_DEBUG_BLK3_W10_REG (DR_REG_OTP_DEBUG_BASE + 0xc8)
/** OTP_DEBUG_BLOCK3_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word10 data.
 */
#define OTP_DEBUG_BLOCK3_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W10_M  (OTP_DEBUG_BLOCK3_W10_V << OTP_DEBUG_BLOCK3_W10_S)
#define OTP_DEBUG_BLOCK3_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W10_S  0

/** OTP_DEBUG_BLK3_W11_REG register
 *  Otp debugger block3 data register11.
 */
#define OTP_DEBUG_BLK3_W11_REG (DR_REG_OTP_DEBUG_BASE + 0xcc)
/** OTP_DEBUG_BLOCK3_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word11 data.
 */
#define OTP_DEBUG_BLOCK3_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W11_M  (OTP_DEBUG_BLOCK3_W11_V << OTP_DEBUG_BLOCK3_W11_S)
#define OTP_DEBUG_BLOCK3_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK3_W11_S  0

/** OTP_DEBUG_BLK4_W1_REG register
 *  Otp debugger block4 data register1.
 */
#define OTP_DEBUG_BLK4_W1_REG (DR_REG_OTP_DEBUG_BASE + 0xd0)
/** OTP_DEBUG_BLOCK4_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word1 data.
 */
#define OTP_DEBUG_BLOCK4_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W1_M  (OTP_DEBUG_BLOCK4_W1_V << OTP_DEBUG_BLOCK4_W1_S)
#define OTP_DEBUG_BLOCK4_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W1_S  0

/** OTP_DEBUG_BLK4_W2_REG register
 *  Otp debugger block4 data register2.
 */
#define OTP_DEBUG_BLK4_W2_REG (DR_REG_OTP_DEBUG_BASE + 0xd4)
/** OTP_DEBUG_BLOCK4_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word2 data.
 */
#define OTP_DEBUG_BLOCK4_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W2_M  (OTP_DEBUG_BLOCK4_W2_V << OTP_DEBUG_BLOCK4_W2_S)
#define OTP_DEBUG_BLOCK4_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W2_S  0

/** OTP_DEBUG_BLK4_W3_REG register
 *  Otp debugger block4 data register3.
 */
#define OTP_DEBUG_BLK4_W3_REG (DR_REG_OTP_DEBUG_BASE + 0xd8)
/** OTP_DEBUG_BLOCK4_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word3 data.
 */
#define OTP_DEBUG_BLOCK4_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W3_M  (OTP_DEBUG_BLOCK4_W3_V << OTP_DEBUG_BLOCK4_W3_S)
#define OTP_DEBUG_BLOCK4_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W3_S  0

/** OTP_DEBUG_BLK4_W4_REG register
 *  Otp debugger block4 data register4.
 */
#define OTP_DEBUG_BLK4_W4_REG (DR_REG_OTP_DEBUG_BASE + 0xdc)
/** OTP_DEBUG_BLOCK4_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word4 data.
 */
#define OTP_DEBUG_BLOCK4_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W4_M  (OTP_DEBUG_BLOCK4_W4_V << OTP_DEBUG_BLOCK4_W4_S)
#define OTP_DEBUG_BLOCK4_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W4_S  0

/** OTP_DEBUG_BLK4_W5_REG register
 *  Otp debugger block4 data register5.
 */
#define OTP_DEBUG_BLK4_W5_REG (DR_REG_OTP_DEBUG_BASE + 0xe0)
/** OTP_DEBUG_BLOCK4_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word5 data.
 */
#define OTP_DEBUG_BLOCK4_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W5_M  (OTP_DEBUG_BLOCK4_W5_V << OTP_DEBUG_BLOCK4_W5_S)
#define OTP_DEBUG_BLOCK4_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W5_S  0

/** OTP_DEBUG_BLK4_W6_REG register
 *  Otp debugger block4 data register6.
 */
#define OTP_DEBUG_BLK4_W6_REG (DR_REG_OTP_DEBUG_BASE + 0xe4)
/** OTP_DEBUG_BLOCK4_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word6 data.
 */
#define OTP_DEBUG_BLOCK4_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W6_M  (OTP_DEBUG_BLOCK4_W6_V << OTP_DEBUG_BLOCK4_W6_S)
#define OTP_DEBUG_BLOCK4_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W6_S  0

/** OTP_DEBUG_BLK4_W7_REG register
 *  Otp debugger block4 data register7.
 */
#define OTP_DEBUG_BLK4_W7_REG (DR_REG_OTP_DEBUG_BASE + 0xe8)
/** OTP_DEBUG_BLOCK4_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word7 data.
 */
#define OTP_DEBUG_BLOCK4_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W7_M  (OTP_DEBUG_BLOCK4_W7_V << OTP_DEBUG_BLOCK4_W7_S)
#define OTP_DEBUG_BLOCK4_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W7_S  0

/** OTP_DEBUG_BLK4_W8_REG register
 *  Otp debugger block4 data register8.
 */
#define OTP_DEBUG_BLK4_W8_REG (DR_REG_OTP_DEBUG_BASE + 0xec)
/** OTP_DEBUG_BLOCK4_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word8 data.
 */
#define OTP_DEBUG_BLOCK4_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W8_M  (OTP_DEBUG_BLOCK4_W8_V << OTP_DEBUG_BLOCK4_W8_S)
#define OTP_DEBUG_BLOCK4_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W8_S  0

/** OTP_DEBUG_BLK4_W9_REG register
 *  Otp debugger block4 data register9.
 */
#define OTP_DEBUG_BLK4_W9_REG (DR_REG_OTP_DEBUG_BASE + 0xf0)
/** OTP_DEBUG_BLOCK4_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word9 data.
 */
#define OTP_DEBUG_BLOCK4_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W9_M  (OTP_DEBUG_BLOCK4_W9_V << OTP_DEBUG_BLOCK4_W9_S)
#define OTP_DEBUG_BLOCK4_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W9_S  0

/** OTP_DEBUG_BLK4_W10_REG register
 *  Otp debugger block4 data registe10.
 */
#define OTP_DEBUG_BLK4_W10_REG (DR_REG_OTP_DEBUG_BASE + 0xf4)
/** OTP_DEBUG_BLOCK4_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word10 data.
 */
#define OTP_DEBUG_BLOCK4_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W10_M  (OTP_DEBUG_BLOCK4_W10_V << OTP_DEBUG_BLOCK4_W10_S)
#define OTP_DEBUG_BLOCK4_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W10_S  0

/** OTP_DEBUG_BLK4_W11_REG register
 *  Otp debugger block4 data register11.
 */
#define OTP_DEBUG_BLK4_W11_REG (DR_REG_OTP_DEBUG_BASE + 0xf8)
/** OTP_DEBUG_BLOCK4_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word11 data.
 */
#define OTP_DEBUG_BLOCK4_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W11_M  (OTP_DEBUG_BLOCK4_W11_V << OTP_DEBUG_BLOCK4_W11_S)
#define OTP_DEBUG_BLOCK4_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK4_W11_S  0

/** OTP_DEBUG_BLK5_W1_REG register
 *  Otp debugger block5 data register1.
 */
#define OTP_DEBUG_BLK5_W1_REG (DR_REG_OTP_DEBUG_BASE + 0xfc)
/** OTP_DEBUG_BLOCK5_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word1 data.
 */
#define OTP_DEBUG_BLOCK5_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W1_M  (OTP_DEBUG_BLOCK5_W1_V << OTP_DEBUG_BLOCK5_W1_S)
#define OTP_DEBUG_BLOCK5_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W1_S  0

/** OTP_DEBUG_BLK5_W2_REG register
 *  Otp debugger block5 data register2.
 */
#define OTP_DEBUG_BLK5_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x100)
/** OTP_DEBUG_BLOCK5_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word2 data.
 */
#define OTP_DEBUG_BLOCK5_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W2_M  (OTP_DEBUG_BLOCK5_W2_V << OTP_DEBUG_BLOCK5_W2_S)
#define OTP_DEBUG_BLOCK5_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W2_S  0

/** OTP_DEBUG_BLK5_W3_REG register
 *  Otp debugger block5 data register3.
 */
#define OTP_DEBUG_BLK5_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x104)
/** OTP_DEBUG_BLOCK5_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word3 data.
 */
#define OTP_DEBUG_BLOCK5_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W3_M  (OTP_DEBUG_BLOCK5_W3_V << OTP_DEBUG_BLOCK5_W3_S)
#define OTP_DEBUG_BLOCK5_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W3_S  0

/** OTP_DEBUG_BLK5_W4_REG register
 *  Otp debugger block5 data register4.
 */
#define OTP_DEBUG_BLK5_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x108)
/** OTP_DEBUG_BLOCK5_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word4 data.
 */
#define OTP_DEBUG_BLOCK5_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W4_M  (OTP_DEBUG_BLOCK5_W4_V << OTP_DEBUG_BLOCK5_W4_S)
#define OTP_DEBUG_BLOCK5_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W4_S  0

/** OTP_DEBUG_BLK5_W5_REG register
 *  Otp debugger block5 data register5.
 */
#define OTP_DEBUG_BLK5_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x10c)
/** OTP_DEBUG_BLOCK5_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word5 data.
 */
#define OTP_DEBUG_BLOCK5_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W5_M  (OTP_DEBUG_BLOCK5_W5_V << OTP_DEBUG_BLOCK5_W5_S)
#define OTP_DEBUG_BLOCK5_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W5_S  0

/** OTP_DEBUG_BLK5_W6_REG register
 *  Otp debugger block5 data register6.
 */
#define OTP_DEBUG_BLK5_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x110)
/** OTP_DEBUG_BLOCK5_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word6 data.
 */
#define OTP_DEBUG_BLOCK5_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W6_M  (OTP_DEBUG_BLOCK5_W6_V << OTP_DEBUG_BLOCK5_W6_S)
#define OTP_DEBUG_BLOCK5_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W6_S  0

/** OTP_DEBUG_BLK5_W7_REG register
 *  Otp debugger block5 data register7.
 */
#define OTP_DEBUG_BLK5_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x114)
/** OTP_DEBUG_BLOCK5_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word7 data.
 */
#define OTP_DEBUG_BLOCK5_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W7_M  (OTP_DEBUG_BLOCK5_W7_V << OTP_DEBUG_BLOCK5_W7_S)
#define OTP_DEBUG_BLOCK5_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W7_S  0

/** OTP_DEBUG_BLK5_W8_REG register
 *  Otp debugger block5 data register8.
 */
#define OTP_DEBUG_BLK5_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x118)
/** OTP_DEBUG_BLOCK5_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word8 data.
 */
#define OTP_DEBUG_BLOCK5_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W8_M  (OTP_DEBUG_BLOCK5_W8_V << OTP_DEBUG_BLOCK5_W8_S)
#define OTP_DEBUG_BLOCK5_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W8_S  0

/** OTP_DEBUG_BLK5_W9_REG register
 *  Otp debugger block5 data register9.
 */
#define OTP_DEBUG_BLK5_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x11c)
/** OTP_DEBUG_BLOCK5_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word9 data.
 */
#define OTP_DEBUG_BLOCK5_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W9_M  (OTP_DEBUG_BLOCK5_W9_V << OTP_DEBUG_BLOCK5_W9_S)
#define OTP_DEBUG_BLOCK5_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W9_S  0

/** OTP_DEBUG_BLK5_W10_REG register
 *  Otp debugger block5 data register10.
 */
#define OTP_DEBUG_BLK5_W10_REG (DR_REG_OTP_DEBUG_BASE + 0x120)
/** OTP_DEBUG_BLOCK5_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word10 data.
 */
#define OTP_DEBUG_BLOCK5_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W10_M  (OTP_DEBUG_BLOCK5_W10_V << OTP_DEBUG_BLOCK5_W10_S)
#define OTP_DEBUG_BLOCK5_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W10_S  0

/** OTP_DEBUG_BLK5_W11_REG register
 *  Otp debugger block5 data register11.
 */
#define OTP_DEBUG_BLK5_W11_REG (DR_REG_OTP_DEBUG_BASE + 0x124)
/** OTP_DEBUG_BLOCK5_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word11 data.
 */
#define OTP_DEBUG_BLOCK5_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W11_M  (OTP_DEBUG_BLOCK5_W11_V << OTP_DEBUG_BLOCK5_W11_S)
#define OTP_DEBUG_BLOCK5_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK5_W11_S  0

/** OTP_DEBUG_BLK6_W1_REG register
 *  Otp debugger block6 data register1.
 */
#define OTP_DEBUG_BLK6_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x128)
/** OTP_DEBUG_BLOCK6_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word1 data.
 */
#define OTP_DEBUG_BLOCK6_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W1_M  (OTP_DEBUG_BLOCK6_W1_V << OTP_DEBUG_BLOCK6_W1_S)
#define OTP_DEBUG_BLOCK6_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W1_S  0

/** OTP_DEBUG_BLK6_W2_REG register
 *  Otp debugger block6 data register2.
 */
#define OTP_DEBUG_BLK6_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x12c)
/** OTP_DEBUG_BLOCK6_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word2 data.
 */
#define OTP_DEBUG_BLOCK6_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W2_M  (OTP_DEBUG_BLOCK6_W2_V << OTP_DEBUG_BLOCK6_W2_S)
#define OTP_DEBUG_BLOCK6_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W2_S  0

/** OTP_DEBUG_BLK6_W3_REG register
 *  Otp debugger block6 data register3.
 */
#define OTP_DEBUG_BLK6_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x130)
/** OTP_DEBUG_BLOCK6_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word3 data.
 */
#define OTP_DEBUG_BLOCK6_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W3_M  (OTP_DEBUG_BLOCK6_W3_V << OTP_DEBUG_BLOCK6_W3_S)
#define OTP_DEBUG_BLOCK6_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W3_S  0

/** OTP_DEBUG_BLK6_W4_REG register
 *  Otp debugger block6 data register4.
 */
#define OTP_DEBUG_BLK6_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x134)
/** OTP_DEBUG_BLOCK6_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word4 data.
 */
#define OTP_DEBUG_BLOCK6_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W4_M  (OTP_DEBUG_BLOCK6_W4_V << OTP_DEBUG_BLOCK6_W4_S)
#define OTP_DEBUG_BLOCK6_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W4_S  0

/** OTP_DEBUG_BLK6_W5_REG register
 *  Otp debugger block6 data register5.
 */
#define OTP_DEBUG_BLK6_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x138)
/** OTP_DEBUG_BLOCK6_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word5 data.
 */
#define OTP_DEBUG_BLOCK6_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W5_M  (OTP_DEBUG_BLOCK6_W5_V << OTP_DEBUG_BLOCK6_W5_S)
#define OTP_DEBUG_BLOCK6_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W5_S  0

/** OTP_DEBUG_BLK6_W6_REG register
 *  Otp debugger block6 data register6.
 */
#define OTP_DEBUG_BLK6_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x13c)
/** OTP_DEBUG_BLOCK6_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word6 data.
 */
#define OTP_DEBUG_BLOCK6_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W6_M  (OTP_DEBUG_BLOCK6_W6_V << OTP_DEBUG_BLOCK6_W6_S)
#define OTP_DEBUG_BLOCK6_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W6_S  0

/** OTP_DEBUG_BLK6_W7_REG register
 *  Otp debugger block6 data register7.
 */
#define OTP_DEBUG_BLK6_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x140)
/** OTP_DEBUG_BLOCK6_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word7 data.
 */
#define OTP_DEBUG_BLOCK6_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W7_M  (OTP_DEBUG_BLOCK6_W7_V << OTP_DEBUG_BLOCK6_W7_S)
#define OTP_DEBUG_BLOCK6_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W7_S  0

/** OTP_DEBUG_BLK6_W8_REG register
 *  Otp debugger block6 data register8.
 */
#define OTP_DEBUG_BLK6_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x144)
/** OTP_DEBUG_BLOCK6_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word8 data.
 */
#define OTP_DEBUG_BLOCK6_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W8_M  (OTP_DEBUG_BLOCK6_W8_V << OTP_DEBUG_BLOCK6_W8_S)
#define OTP_DEBUG_BLOCK6_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W8_S  0

/** OTP_DEBUG_BLK6_W9_REG register
 *  Otp debugger block6 data register9.
 */
#define OTP_DEBUG_BLK6_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x148)
/** OTP_DEBUG_BLOCK6_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word9 data.
 */
#define OTP_DEBUG_BLOCK6_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W9_M  (OTP_DEBUG_BLOCK6_W9_V << OTP_DEBUG_BLOCK6_W9_S)
#define OTP_DEBUG_BLOCK6_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W9_S  0

/** OTP_DEBUG_BLK6_W10_REG register
 *  Otp debugger block6 data register10.
 */
#define OTP_DEBUG_BLK6_W10_REG (DR_REG_OTP_DEBUG_BASE + 0x14c)
/** OTP_DEBUG_BLOCK6_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word10 data.
 */
#define OTP_DEBUG_BLOCK6_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W10_M  (OTP_DEBUG_BLOCK6_W10_V << OTP_DEBUG_BLOCK6_W10_S)
#define OTP_DEBUG_BLOCK6_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W10_S  0

/** OTP_DEBUG_BLK6_W11_REG register
 *  Otp debugger block6 data register11.
 */
#define OTP_DEBUG_BLK6_W11_REG (DR_REG_OTP_DEBUG_BASE + 0x150)
/** OTP_DEBUG_BLOCK6_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word11 data.
 */
#define OTP_DEBUG_BLOCK6_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W11_M  (OTP_DEBUG_BLOCK6_W11_V << OTP_DEBUG_BLOCK6_W11_S)
#define OTP_DEBUG_BLOCK6_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK6_W11_S  0

/** OTP_DEBUG_BLK7_W1_REG register
 *  Otp debugger block7 data register1.
 */
#define OTP_DEBUG_BLK7_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x154)
/** OTP_DEBUG_BLOCK7_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word1 data.
 */
#define OTP_DEBUG_BLOCK7_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W1_M  (OTP_DEBUG_BLOCK7_W1_V << OTP_DEBUG_BLOCK7_W1_S)
#define OTP_DEBUG_BLOCK7_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W1_S  0

/** OTP_DEBUG_BLK7_W2_REG register
 *  Otp debugger block7 data register2.
 */
#define OTP_DEBUG_BLK7_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x158)
/** OTP_DEBUG_BLOCK7_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word2 data.
 */
#define OTP_DEBUG_BLOCK7_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W2_M  (OTP_DEBUG_BLOCK7_W2_V << OTP_DEBUG_BLOCK7_W2_S)
#define OTP_DEBUG_BLOCK7_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W2_S  0

/** OTP_DEBUG_BLK7_W3_REG register
 *  Otp debugger block7 data register3.
 */
#define OTP_DEBUG_BLK7_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x15c)
/** OTP_DEBUG_BLOCK7_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word3 data.
 */
#define OTP_DEBUG_BLOCK7_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W3_M  (OTP_DEBUG_BLOCK7_W3_V << OTP_DEBUG_BLOCK7_W3_S)
#define OTP_DEBUG_BLOCK7_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W3_S  0

/** OTP_DEBUG_BLK7_W4_REG register
 *  Otp debugger block7 data register4.
 */
#define OTP_DEBUG_BLK7_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x160)
/** OTP_DEBUG_BLOCK7_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word4 data.
 */
#define OTP_DEBUG_BLOCK7_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W4_M  (OTP_DEBUG_BLOCK7_W4_V << OTP_DEBUG_BLOCK7_W4_S)
#define OTP_DEBUG_BLOCK7_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W4_S  0

/** OTP_DEBUG_BLK7_W5_REG register
 *  Otp debugger block7 data register5.
 */
#define OTP_DEBUG_BLK7_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x164)
/** OTP_DEBUG_BLOCK7_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word5 data.
 */
#define OTP_DEBUG_BLOCK7_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W5_M  (OTP_DEBUG_BLOCK7_W5_V << OTP_DEBUG_BLOCK7_W5_S)
#define OTP_DEBUG_BLOCK7_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W5_S  0

/** OTP_DEBUG_BLK7_W6_REG register
 *  Otp debugger block7 data register6.
 */
#define OTP_DEBUG_BLK7_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x168)
/** OTP_DEBUG_BLOCK7_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word6 data.
 */
#define OTP_DEBUG_BLOCK7_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W6_M  (OTP_DEBUG_BLOCK7_W6_V << OTP_DEBUG_BLOCK7_W6_S)
#define OTP_DEBUG_BLOCK7_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W6_S  0

/** OTP_DEBUG_BLK7_W7_REG register
 *  Otp debugger block7 data register7.
 */
#define OTP_DEBUG_BLK7_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x16c)
/** OTP_DEBUG_BLOCK7_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word7 data.
 */
#define OTP_DEBUG_BLOCK7_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W7_M  (OTP_DEBUG_BLOCK7_W7_V << OTP_DEBUG_BLOCK7_W7_S)
#define OTP_DEBUG_BLOCK7_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W7_S  0

/** OTP_DEBUG_BLK7_W8_REG register
 *  Otp debugger block7 data register8.
 */
#define OTP_DEBUG_BLK7_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x170)
/** OTP_DEBUG_BLOCK7_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word8 data.
 */
#define OTP_DEBUG_BLOCK7_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W8_M  (OTP_DEBUG_BLOCK7_W8_V << OTP_DEBUG_BLOCK7_W8_S)
#define OTP_DEBUG_BLOCK7_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W8_S  0

/** OTP_DEBUG_BLK7_W9_REG register
 *  Otp debugger block7 data register9.
 */
#define OTP_DEBUG_BLK7_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x174)
/** OTP_DEBUG_BLOCK7_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word9 data.
 */
#define OTP_DEBUG_BLOCK7_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W9_M  (OTP_DEBUG_BLOCK7_W9_V << OTP_DEBUG_BLOCK7_W9_S)
#define OTP_DEBUG_BLOCK7_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W9_S  0

/** OTP_DEBUG_BLK7_W10_REG register
 *  Otp debugger block7 data register10.
 */
#define OTP_DEBUG_BLK7_W10_REG (DR_REG_OTP_DEBUG_BASE + 0x178)
/** OTP_DEBUG_BLOCK7_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word10 data.
 */
#define OTP_DEBUG_BLOCK7_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W10_M  (OTP_DEBUG_BLOCK7_W10_V << OTP_DEBUG_BLOCK7_W10_S)
#define OTP_DEBUG_BLOCK7_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W10_S  0

/** OTP_DEBUG_BLK7_W11_REG register
 *  Otp debugger block7 data register11.
 */
#define OTP_DEBUG_BLK7_W11_REG (DR_REG_OTP_DEBUG_BASE + 0x17c)
/** OTP_DEBUG_BLOCK7_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word11 data.
 */
#define OTP_DEBUG_BLOCK7_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W11_M  (OTP_DEBUG_BLOCK7_W11_V << OTP_DEBUG_BLOCK7_W11_S)
#define OTP_DEBUG_BLOCK7_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK7_W11_S  0

/** OTP_DEBUG_BLK8_W1_REG register
 *  Otp debugger block8 data register1.
 */
#define OTP_DEBUG_BLK8_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x180)
/** OTP_DEBUG_BLOCK8_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word1 data.
 */
#define OTP_DEBUG_BLOCK8_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W1_M  (OTP_DEBUG_BLOCK8_W1_V << OTP_DEBUG_BLOCK8_W1_S)
#define OTP_DEBUG_BLOCK8_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W1_S  0

/** OTP_DEBUG_BLK8_W2_REG register
 *  Otp debugger block8 data register2.
 */
#define OTP_DEBUG_BLK8_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x184)
/** OTP_DEBUG_BLOCK8_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word2 data.
 */
#define OTP_DEBUG_BLOCK8_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W2_M  (OTP_DEBUG_BLOCK8_W2_V << OTP_DEBUG_BLOCK8_W2_S)
#define OTP_DEBUG_BLOCK8_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W2_S  0

/** OTP_DEBUG_BLK8_W3_REG register
 *  Otp debugger block8 data register3.
 */
#define OTP_DEBUG_BLK8_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x188)
/** OTP_DEBUG_BLOCK8_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word3 data.
 */
#define OTP_DEBUG_BLOCK8_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W3_M  (OTP_DEBUG_BLOCK8_W3_V << OTP_DEBUG_BLOCK8_W3_S)
#define OTP_DEBUG_BLOCK8_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W3_S  0

/** OTP_DEBUG_BLK8_W4_REG register
 *  Otp debugger block8 data register4.
 */
#define OTP_DEBUG_BLK8_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x18c)
/** OTP_DEBUG_BLOCK8_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word4 data.
 */
#define OTP_DEBUG_BLOCK8_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W4_M  (OTP_DEBUG_BLOCK8_W4_V << OTP_DEBUG_BLOCK8_W4_S)
#define OTP_DEBUG_BLOCK8_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W4_S  0

/** OTP_DEBUG_BLK8_W5_REG register
 *  Otp debugger block8 data register5.
 */
#define OTP_DEBUG_BLK8_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x190)
/** OTP_DEBUG_BLOCK8_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word5 data.
 */
#define OTP_DEBUG_BLOCK8_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W5_M  (OTP_DEBUG_BLOCK8_W5_V << OTP_DEBUG_BLOCK8_W5_S)
#define OTP_DEBUG_BLOCK8_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W5_S  0

/** OTP_DEBUG_BLK8_W6_REG register
 *  Otp debugger block8 data register6.
 */
#define OTP_DEBUG_BLK8_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x194)
/** OTP_DEBUG_BLOCK8_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word6 data.
 */
#define OTP_DEBUG_BLOCK8_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W6_M  (OTP_DEBUG_BLOCK8_W6_V << OTP_DEBUG_BLOCK8_W6_S)
#define OTP_DEBUG_BLOCK8_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W6_S  0

/** OTP_DEBUG_BLK8_W7_REG register
 *  Otp debugger block8 data register7.
 */
#define OTP_DEBUG_BLK8_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x198)
/** OTP_DEBUG_BLOCK8_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word7 data.
 */
#define OTP_DEBUG_BLOCK8_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W7_M  (OTP_DEBUG_BLOCK8_W7_V << OTP_DEBUG_BLOCK8_W7_S)
#define OTP_DEBUG_BLOCK8_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W7_S  0

/** OTP_DEBUG_BLK8_W8_REG register
 *  Otp debugger block8 data register8.
 */
#define OTP_DEBUG_BLK8_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x19c)
/** OTP_DEBUG_BLOCK8_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word8 data.
 */
#define OTP_DEBUG_BLOCK8_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W8_M  (OTP_DEBUG_BLOCK8_W8_V << OTP_DEBUG_BLOCK8_W8_S)
#define OTP_DEBUG_BLOCK8_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W8_S  0

/** OTP_DEBUG_BLK8_W9_REG register
 *  Otp debugger block8 data register9.
 */
#define OTP_DEBUG_BLK8_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x1a0)
/** OTP_DEBUG_BLOCK8_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word9 data.
 */
#define OTP_DEBUG_BLOCK8_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W9_M  (OTP_DEBUG_BLOCK8_W9_V << OTP_DEBUG_BLOCK8_W9_S)
#define OTP_DEBUG_BLOCK8_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W9_S  0

/** OTP_DEBUG_BLK8_W10_REG register
 *  Otp debugger block8 data register10.
 */
#define OTP_DEBUG_BLK8_W10_REG (DR_REG_OTP_DEBUG_BASE + 0x1a4)
/** OTP_DEBUG_BLOCK8_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word10 data.
 */
#define OTP_DEBUG_BLOCK8_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W10_M  (OTP_DEBUG_BLOCK8_W10_V << OTP_DEBUG_BLOCK8_W10_S)
#define OTP_DEBUG_BLOCK8_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W10_S  0

/** OTP_DEBUG_BLK8_W11_REG register
 *  Otp debugger block8 data register11.
 */
#define OTP_DEBUG_BLK8_W11_REG (DR_REG_OTP_DEBUG_BASE + 0x1a8)
/** OTP_DEBUG_BLOCK8_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word11 data.
 */
#define OTP_DEBUG_BLOCK8_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W11_M  (OTP_DEBUG_BLOCK8_W11_V << OTP_DEBUG_BLOCK8_W11_S)
#define OTP_DEBUG_BLOCK8_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK8_W11_S  0

/** OTP_DEBUG_BLK9_W1_REG register
 *  Otp debugger block9 data register1.
 */
#define OTP_DEBUG_BLK9_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x1ac)
/** OTP_DEBUG_BLOCK9_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word1 data.
 */
#define OTP_DEBUG_BLOCK9_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W1_M  (OTP_DEBUG_BLOCK9_W1_V << OTP_DEBUG_BLOCK9_W1_S)
#define OTP_DEBUG_BLOCK9_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W1_S  0

/** OTP_DEBUG_BLK9_W2_REG register
 *  Otp debugger block9 data register2.
 */
#define OTP_DEBUG_BLK9_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x1b0)
/** OTP_DEBUG_BLOCK9_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word2 data.
 */
#define OTP_DEBUG_BLOCK9_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W2_M  (OTP_DEBUG_BLOCK9_W2_V << OTP_DEBUG_BLOCK9_W2_S)
#define OTP_DEBUG_BLOCK9_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W2_S  0

/** OTP_DEBUG_BLK9_W3_REG register
 *  Otp debugger block9 data register3.
 */
#define OTP_DEBUG_BLK9_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x1b4)
/** OTP_DEBUG_BLOCK9_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word3 data.
 */
#define OTP_DEBUG_BLOCK9_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W3_M  (OTP_DEBUG_BLOCK9_W3_V << OTP_DEBUG_BLOCK9_W3_S)
#define OTP_DEBUG_BLOCK9_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W3_S  0

/** OTP_DEBUG_BLK9_W4_REG register
 *  Otp debugger block9 data register4.
 */
#define OTP_DEBUG_BLK9_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x1b8)
/** OTP_DEBUG_BLOCK9_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word4 data.
 */
#define OTP_DEBUG_BLOCK9_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W4_M  (OTP_DEBUG_BLOCK9_W4_V << OTP_DEBUG_BLOCK9_W4_S)
#define OTP_DEBUG_BLOCK9_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W4_S  0

/** OTP_DEBUG_BLK9_W5_REG register
 *  Otp debugger block9 data register5.
 */
#define OTP_DEBUG_BLK9_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x1bc)
/** OTP_DEBUG_BLOCK9_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word5 data.
 */
#define OTP_DEBUG_BLOCK9_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W5_M  (OTP_DEBUG_BLOCK9_W5_V << OTP_DEBUG_BLOCK9_W5_S)
#define OTP_DEBUG_BLOCK9_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W5_S  0

/** OTP_DEBUG_BLK9_W6_REG register
 *  Otp debugger block9 data register6.
 */
#define OTP_DEBUG_BLK9_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x1c0)
/** OTP_DEBUG_BLOCK9_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word6 data.
 */
#define OTP_DEBUG_BLOCK9_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W6_M  (OTP_DEBUG_BLOCK9_W6_V << OTP_DEBUG_BLOCK9_W6_S)
#define OTP_DEBUG_BLOCK9_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W6_S  0

/** OTP_DEBUG_BLK9_W7_REG register
 *  Otp debugger block9 data register7.
 */
#define OTP_DEBUG_BLK9_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x1c4)
/** OTP_DEBUG_BLOCK9_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word7 data.
 */
#define OTP_DEBUG_BLOCK9_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W7_M  (OTP_DEBUG_BLOCK9_W7_V << OTP_DEBUG_BLOCK9_W7_S)
#define OTP_DEBUG_BLOCK9_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W7_S  0

/** OTP_DEBUG_BLK9_W8_REG register
 *  Otp debugger block9 data register8.
 */
#define OTP_DEBUG_BLK9_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x1c8)
/** OTP_DEBUG_BLOCK9_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word8 data.
 */
#define OTP_DEBUG_BLOCK9_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W8_M  (OTP_DEBUG_BLOCK9_W8_V << OTP_DEBUG_BLOCK9_W8_S)
#define OTP_DEBUG_BLOCK9_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W8_S  0

/** OTP_DEBUG_BLK9_W9_REG register
 *  Otp debugger block9 data register9.
 */
#define OTP_DEBUG_BLK9_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x1cc)
/** OTP_DEBUG_BLOCK9_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word9 data.
 */
#define OTP_DEBUG_BLOCK9_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W9_M  (OTP_DEBUG_BLOCK9_W9_V << OTP_DEBUG_BLOCK9_W9_S)
#define OTP_DEBUG_BLOCK9_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W9_S  0

/** OTP_DEBUG_BLK9_W10_REG register
 *  Otp debugger block9 data register10.
 */
#define OTP_DEBUG_BLK9_W10_REG (DR_REG_OTP_DEBUG_BASE + 0x1d0)
/** OTP_DEBUG_BLOCK9_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word10 data.
 */
#define OTP_DEBUG_BLOCK9_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W10_M  (OTP_DEBUG_BLOCK9_W10_V << OTP_DEBUG_BLOCK9_W10_S)
#define OTP_DEBUG_BLOCK9_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W10_S  0

/** OTP_DEBUG_BLK9_W11_REG register
 *  Otp debugger block9 data register11.
 */
#define OTP_DEBUG_BLK9_W11_REG (DR_REG_OTP_DEBUG_BASE + 0x1d4)
/** OTP_DEBUG_BLOCK9_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word11 data.
 */
#define OTP_DEBUG_BLOCK9_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W11_M  (OTP_DEBUG_BLOCK9_W11_V << OTP_DEBUG_BLOCK9_W11_S)
#define OTP_DEBUG_BLOCK9_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK9_W11_S  0

/** OTP_DEBUG_BLK10_W1_REG register
 *  Otp debugger block10 data register1.
 */
#define OTP_DEBUG_BLK10_W1_REG (DR_REG_OTP_DEBUG_BASE + 0x1d8)
/** OTP_DEBUG_BLOCK10_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word1 data.
 */
#define OTP_DEBUG_BLOCK10_W1    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W1_M  (OTP_DEBUG_BLOCK10_W1_V << OTP_DEBUG_BLOCK10_W1_S)
#define OTP_DEBUG_BLOCK10_W1_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W1_S  0

/** OTP_DEBUG_BLK10_W2_REG register
 *  Otp debugger block10 data register2.
 */
#define OTP_DEBUG_BLK10_W2_REG (DR_REG_OTP_DEBUG_BASE + 0x1dc)
/** OTP_DEBUG_BLOCK10_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word2 data.
 */
#define OTP_DEBUG_BLOCK10_W2    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W2_M  (OTP_DEBUG_BLOCK10_W2_V << OTP_DEBUG_BLOCK10_W2_S)
#define OTP_DEBUG_BLOCK10_W2_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W2_S  0

/** OTP_DEBUG_BLK10_W3_REG register
 *  Otp debugger block10 data register3.
 */
#define OTP_DEBUG_BLK10_W3_REG (DR_REG_OTP_DEBUG_BASE + 0x1e0)
/** OTP_DEBUG_BLOCK10_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word3 data.
 */
#define OTP_DEBUG_BLOCK10_W3    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W3_M  (OTP_DEBUG_BLOCK10_W3_V << OTP_DEBUG_BLOCK10_W3_S)
#define OTP_DEBUG_BLOCK10_W3_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W3_S  0

/** OTP_DEBUG_BLK10_W4_REG register
 *  Otp debugger block10 data register4.
 */
#define OTP_DEBUG_BLK10_W4_REG (DR_REG_OTP_DEBUG_BASE + 0x1e4)
/** OTP_DEBUG_BLOCK10_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word4 data.
 */
#define OTP_DEBUG_BLOCK10_W4    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W4_M  (OTP_DEBUG_BLOCK10_W4_V << OTP_DEBUG_BLOCK10_W4_S)
#define OTP_DEBUG_BLOCK10_W4_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W4_S  0

/** OTP_DEBUG_BLK10_W5_REG register
 *  Otp debugger block10 data register5.
 */
#define OTP_DEBUG_BLK10_W5_REG (DR_REG_OTP_DEBUG_BASE + 0x1e8)
/** OTP_DEBUG_BLOCK10_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word5 data.
 */
#define OTP_DEBUG_BLOCK10_W5    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W5_M  (OTP_DEBUG_BLOCK10_W5_V << OTP_DEBUG_BLOCK10_W5_S)
#define OTP_DEBUG_BLOCK10_W5_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W5_S  0

/** OTP_DEBUG_BLK10_W6_REG register
 *  Otp debugger block10 data register6.
 */
#define OTP_DEBUG_BLK10_W6_REG (DR_REG_OTP_DEBUG_BASE + 0x1ec)
/** OTP_DEBUG_BLOCK10_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word6 data.
 */
#define OTP_DEBUG_BLOCK10_W6    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W6_M  (OTP_DEBUG_BLOCK10_W6_V << OTP_DEBUG_BLOCK10_W6_S)
#define OTP_DEBUG_BLOCK10_W6_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W6_S  0

/** OTP_DEBUG_BLK10_W7_REG register
 *  Otp debugger block10 data register7.
 */
#define OTP_DEBUG_BLK10_W7_REG (DR_REG_OTP_DEBUG_BASE + 0x1f0)
/** OTP_DEBUG_BLOCK10_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word7 data.
 */
#define OTP_DEBUG_BLOCK10_W7    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W7_M  (OTP_DEBUG_BLOCK10_W7_V << OTP_DEBUG_BLOCK10_W7_S)
#define OTP_DEBUG_BLOCK10_W7_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W7_S  0

/** OTP_DEBUG_BLK10_W8_REG register
 *  Otp debugger block10 data register8.
 */
#define OTP_DEBUG_BLK10_W8_REG (DR_REG_OTP_DEBUG_BASE + 0x1f4)
/** OTP_DEBUG_BLOCK10_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word8 data.
 */
#define OTP_DEBUG_BLOCK10_W8    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W8_M  (OTP_DEBUG_BLOCK10_W8_V << OTP_DEBUG_BLOCK10_W8_S)
#define OTP_DEBUG_BLOCK10_W8_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W8_S  0

/** OTP_DEBUG_BLK10_W9_REG register
 *  Otp debugger block10 data register9.
 */
#define OTP_DEBUG_BLK10_W9_REG (DR_REG_OTP_DEBUG_BASE + 0x1f8)
/** OTP_DEBUG_BLOCK10_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word9 data.
 */
#define OTP_DEBUG_BLOCK10_W9    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W9_M  (OTP_DEBUG_BLOCK10_W9_V << OTP_DEBUG_BLOCK10_W9_S)
#define OTP_DEBUG_BLOCK10_W9_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W9_S  0

/** OTP_DEBUG_BLK10_W10_REG register
 *  Otp debugger block10 data register10.
 */
#define OTP_DEBUG_BLK10_W10_REG (DR_REG_OTP_DEBUG_BASE + 0x1fc)
/** OTP_DEBUG_BLOCK19_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word10 data.
 */
#define OTP_DEBUG_BLOCK19_W10    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK19_W10_M  (OTP_DEBUG_BLOCK19_W10_V << OTP_DEBUG_BLOCK19_W10_S)
#define OTP_DEBUG_BLOCK19_W10_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK19_W10_S  0

/** OTP_DEBUG_BLK10_W11_REG register
 *  Otp debugger block10 data register11.
 */
#define OTP_DEBUG_BLK10_W11_REG (DR_REG_OTP_DEBUG_BASE + 0x200)
/** OTP_DEBUG_BLOCK10_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word11 data.
 */
#define OTP_DEBUG_BLOCK10_W11    0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W11_M  (OTP_DEBUG_BLOCK10_W11_V << OTP_DEBUG_BLOCK10_W11_S)
#define OTP_DEBUG_BLOCK10_W11_V  0xFFFFFFFFU
#define OTP_DEBUG_BLOCK10_W11_S  0

/** OTP_DEBUG_CLK_REG register
 *  Otp debugger clk_en configuration register.
 */
#define OTP_DEBUG_CLK_REG (DR_REG_OTP_DEBUG_BASE + 0x204)
/** OTP_DEBUG_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Force clock on for this register file.
 */
#define OTP_DEBUG_CLK_EN    (BIT(0))
#define OTP_DEBUG_CLK_EN_M  (OTP_DEBUG_CLK_EN_V << OTP_DEBUG_CLK_EN_S)
#define OTP_DEBUG_CLK_EN_V  0x00000001U
#define OTP_DEBUG_CLK_EN_S  0

/** OTP_DEBUG_APB2OTP_EN_REG register
 *  Otp_debuger apb2otp enable configuration register.
 */
#define OTP_DEBUG_APB2OTP_EN_REG (DR_REG_OTP_DEBUG_BASE + 0x208)
/** OTP_DEBUG_APB2OTP_EN : R/W; bitpos: [0]; default: 0;
 *  Debug mode enable signal.
 */
#define OTP_DEBUG_APB2OTP_EN    (BIT(0))
#define OTP_DEBUG_APB2OTP_EN_M  (OTP_DEBUG_APB2OTP_EN_V << OTP_DEBUG_APB2OTP_EN_S)
#define OTP_DEBUG_APB2OTP_EN_V  0x00000001U
#define OTP_DEBUG_APB2OTP_EN_S  0

/** OTP_DEBUG_DATE_REG register
 *  eFuse version register.
 */
#define OTP_DEBUG_DATE_REG (DR_REG_OTP_DEBUG_BASE + 0x20c)
/** OTP_DEBUG_DATE : R/W; bitpos: [27:0]; default: 539037736;
 *  Stores otp_debug version.
 */
#define OTP_DEBUG_DATE    0x0FFFFFFFU
#define OTP_DEBUG_DATE_M  (OTP_DEBUG_DATE_V << OTP_DEBUG_DATE_S)
#define OTP_DEBUG_DATE_V  0x0FFFFFFFU
#define OTP_DEBUG_DATE_S  0

#ifdef __cplusplus
}
#endif
