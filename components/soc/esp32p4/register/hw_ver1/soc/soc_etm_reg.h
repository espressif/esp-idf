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

/** SOC_ETM_CH_ENA_AD0_REG register
 *  Channel enable status register
 */
#define SOC_ETM_CH_ENA_AD0_REG (DR_REG_SOC_ETM_BASE + 0x0)
/** SOC_ETM_CH_ENA0 : R/WTC/WTS; bitpos: [0]; default: 0;
 *  Represents ch0 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA0    (BIT(0))
#define SOC_ETM_CH_ENA0_M  (SOC_ETM_CH_ENA0_V << SOC_ETM_CH_ENA0_S)
#define SOC_ETM_CH_ENA0_V  0x00000001U
#define SOC_ETM_CH_ENA0_S  0
/** SOC_ETM_CH_ENA1 : R/WTC/WTS; bitpos: [1]; default: 0;
 *  Represents ch1 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA1    (BIT(1))
#define SOC_ETM_CH_ENA1_M  (SOC_ETM_CH_ENA1_V << SOC_ETM_CH_ENA1_S)
#define SOC_ETM_CH_ENA1_V  0x00000001U
#define SOC_ETM_CH_ENA1_S  1
/** SOC_ETM_CH_ENA2 : R/WTC/WTS; bitpos: [2]; default: 0;
 *  Represents ch2 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA2    (BIT(2))
#define SOC_ETM_CH_ENA2_M  (SOC_ETM_CH_ENA2_V << SOC_ETM_CH_ENA2_S)
#define SOC_ETM_CH_ENA2_V  0x00000001U
#define SOC_ETM_CH_ENA2_S  2
/** SOC_ETM_CH_ENA3 : R/WTC/WTS; bitpos: [3]; default: 0;
 *  Represents ch3 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA3    (BIT(3))
#define SOC_ETM_CH_ENA3_M  (SOC_ETM_CH_ENA3_V << SOC_ETM_CH_ENA3_S)
#define SOC_ETM_CH_ENA3_V  0x00000001U
#define SOC_ETM_CH_ENA3_S  3
/** SOC_ETM_CH_ENA4 : R/WTC/WTS; bitpos: [4]; default: 0;
 *  Represents ch4 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA4    (BIT(4))
#define SOC_ETM_CH_ENA4_M  (SOC_ETM_CH_ENA4_V << SOC_ETM_CH_ENA4_S)
#define SOC_ETM_CH_ENA4_V  0x00000001U
#define SOC_ETM_CH_ENA4_S  4
/** SOC_ETM_CH_ENA5 : R/WTC/WTS; bitpos: [5]; default: 0;
 *  Represents ch5 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA5    (BIT(5))
#define SOC_ETM_CH_ENA5_M  (SOC_ETM_CH_ENA5_V << SOC_ETM_CH_ENA5_S)
#define SOC_ETM_CH_ENA5_V  0x00000001U
#define SOC_ETM_CH_ENA5_S  5
/** SOC_ETM_CH_ENA6 : R/WTC/WTS; bitpos: [6]; default: 0;
 *  Represents ch6 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA6    (BIT(6))
#define SOC_ETM_CH_ENA6_M  (SOC_ETM_CH_ENA6_V << SOC_ETM_CH_ENA6_S)
#define SOC_ETM_CH_ENA6_V  0x00000001U
#define SOC_ETM_CH_ENA6_S  6
/** SOC_ETM_CH_ENA7 : R/WTC/WTS; bitpos: [7]; default: 0;
 *  Represents ch7 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA7    (BIT(7))
#define SOC_ETM_CH_ENA7_M  (SOC_ETM_CH_ENA7_V << SOC_ETM_CH_ENA7_S)
#define SOC_ETM_CH_ENA7_V  0x00000001U
#define SOC_ETM_CH_ENA7_S  7
/** SOC_ETM_CH_ENA8 : R/WTC/WTS; bitpos: [8]; default: 0;
 *  Represents ch8 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA8    (BIT(8))
#define SOC_ETM_CH_ENA8_M  (SOC_ETM_CH_ENA8_V << SOC_ETM_CH_ENA8_S)
#define SOC_ETM_CH_ENA8_V  0x00000001U
#define SOC_ETM_CH_ENA8_S  8
/** SOC_ETM_CH_ENA9 : R/WTC/WTS; bitpos: [9]; default: 0;
 *  Represents ch9 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA9    (BIT(9))
#define SOC_ETM_CH_ENA9_M  (SOC_ETM_CH_ENA9_V << SOC_ETM_CH_ENA9_S)
#define SOC_ETM_CH_ENA9_V  0x00000001U
#define SOC_ETM_CH_ENA9_S  9
/** SOC_ETM_CH_ENA10 : R/WTC/WTS; bitpos: [10]; default: 0;
 *  Represents ch10 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA10    (BIT(10))
#define SOC_ETM_CH_ENA10_M  (SOC_ETM_CH_ENA10_V << SOC_ETM_CH_ENA10_S)
#define SOC_ETM_CH_ENA10_V  0x00000001U
#define SOC_ETM_CH_ENA10_S  10
/** SOC_ETM_CH_ENA11 : R/WTC/WTS; bitpos: [11]; default: 0;
 *  Represents ch11 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA11    (BIT(11))
#define SOC_ETM_CH_ENA11_M  (SOC_ETM_CH_ENA11_V << SOC_ETM_CH_ENA11_S)
#define SOC_ETM_CH_ENA11_V  0x00000001U
#define SOC_ETM_CH_ENA11_S  11
/** SOC_ETM_CH_ENA12 : R/WTC/WTS; bitpos: [12]; default: 0;
 *  Represents ch12 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA12    (BIT(12))
#define SOC_ETM_CH_ENA12_M  (SOC_ETM_CH_ENA12_V << SOC_ETM_CH_ENA12_S)
#define SOC_ETM_CH_ENA12_V  0x00000001U
#define SOC_ETM_CH_ENA12_S  12
/** SOC_ETM_CH_ENA13 : R/WTC/WTS; bitpos: [13]; default: 0;
 *  Represents ch13 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA13    (BIT(13))
#define SOC_ETM_CH_ENA13_M  (SOC_ETM_CH_ENA13_V << SOC_ETM_CH_ENA13_S)
#define SOC_ETM_CH_ENA13_V  0x00000001U
#define SOC_ETM_CH_ENA13_S  13
/** SOC_ETM_CH_ENA14 : R/WTC/WTS; bitpos: [14]; default: 0;
 *  Represents ch14 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA14    (BIT(14))
#define SOC_ETM_CH_ENA14_M  (SOC_ETM_CH_ENA14_V << SOC_ETM_CH_ENA14_S)
#define SOC_ETM_CH_ENA14_V  0x00000001U
#define SOC_ETM_CH_ENA14_S  14
/** SOC_ETM_CH_ENA15 : R/WTC/WTS; bitpos: [15]; default: 0;
 *  Represents ch15 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA15    (BIT(15))
#define SOC_ETM_CH_ENA15_M  (SOC_ETM_CH_ENA15_V << SOC_ETM_CH_ENA15_S)
#define SOC_ETM_CH_ENA15_V  0x00000001U
#define SOC_ETM_CH_ENA15_S  15
/** SOC_ETM_CH_ENA16 : R/WTC/WTS; bitpos: [16]; default: 0;
 *  Represents ch16 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA16    (BIT(16))
#define SOC_ETM_CH_ENA16_M  (SOC_ETM_CH_ENA16_V << SOC_ETM_CH_ENA16_S)
#define SOC_ETM_CH_ENA16_V  0x00000001U
#define SOC_ETM_CH_ENA16_S  16
/** SOC_ETM_CH_ENA17 : R/WTC/WTS; bitpos: [17]; default: 0;
 *  Represents ch17 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA17    (BIT(17))
#define SOC_ETM_CH_ENA17_M  (SOC_ETM_CH_ENA17_V << SOC_ETM_CH_ENA17_S)
#define SOC_ETM_CH_ENA17_V  0x00000001U
#define SOC_ETM_CH_ENA17_S  17
/** SOC_ETM_CH_ENA18 : R/WTC/WTS; bitpos: [18]; default: 0;
 *  Represents ch18 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA18    (BIT(18))
#define SOC_ETM_CH_ENA18_M  (SOC_ETM_CH_ENA18_V << SOC_ETM_CH_ENA18_S)
#define SOC_ETM_CH_ENA18_V  0x00000001U
#define SOC_ETM_CH_ENA18_S  18
/** SOC_ETM_CH_ENA19 : R/WTC/WTS; bitpos: [19]; default: 0;
 *  Represents ch19 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA19    (BIT(19))
#define SOC_ETM_CH_ENA19_M  (SOC_ETM_CH_ENA19_V << SOC_ETM_CH_ENA19_S)
#define SOC_ETM_CH_ENA19_V  0x00000001U
#define SOC_ETM_CH_ENA19_S  19
/** SOC_ETM_CH_ENA20 : R/WTC/WTS; bitpos: [20]; default: 0;
 *  Represents ch20 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA20    (BIT(20))
#define SOC_ETM_CH_ENA20_M  (SOC_ETM_CH_ENA20_V << SOC_ETM_CH_ENA20_S)
#define SOC_ETM_CH_ENA20_V  0x00000001U
#define SOC_ETM_CH_ENA20_S  20
/** SOC_ETM_CH_ENA21 : R/WTC/WTS; bitpos: [21]; default: 0;
 *  Represents ch21 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA21    (BIT(21))
#define SOC_ETM_CH_ENA21_M  (SOC_ETM_CH_ENA21_V << SOC_ETM_CH_ENA21_S)
#define SOC_ETM_CH_ENA21_V  0x00000001U
#define SOC_ETM_CH_ENA21_S  21
/** SOC_ETM_CH_ENA22 : R/WTC/WTS; bitpos: [22]; default: 0;
 *  Represents ch22 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA22    (BIT(22))
#define SOC_ETM_CH_ENA22_M  (SOC_ETM_CH_ENA22_V << SOC_ETM_CH_ENA22_S)
#define SOC_ETM_CH_ENA22_V  0x00000001U
#define SOC_ETM_CH_ENA22_S  22
/** SOC_ETM_CH_ENA23 : R/WTC/WTS; bitpos: [23]; default: 0;
 *  Represents ch23 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA23    (BIT(23))
#define SOC_ETM_CH_ENA23_M  (SOC_ETM_CH_ENA23_V << SOC_ETM_CH_ENA23_S)
#define SOC_ETM_CH_ENA23_V  0x00000001U
#define SOC_ETM_CH_ENA23_S  23
/** SOC_ETM_CH_ENA24 : R/WTC/WTS; bitpos: [24]; default: 0;
 *  Represents ch24 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA24    (BIT(24))
#define SOC_ETM_CH_ENA24_M  (SOC_ETM_CH_ENA24_V << SOC_ETM_CH_ENA24_S)
#define SOC_ETM_CH_ENA24_V  0x00000001U
#define SOC_ETM_CH_ENA24_S  24
/** SOC_ETM_CH_ENA25 : R/WTC/WTS; bitpos: [25]; default: 0;
 *  Represents ch25 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA25    (BIT(25))
#define SOC_ETM_CH_ENA25_M  (SOC_ETM_CH_ENA25_V << SOC_ETM_CH_ENA25_S)
#define SOC_ETM_CH_ENA25_V  0x00000001U
#define SOC_ETM_CH_ENA25_S  25
/** SOC_ETM_CH_ENA26 : R/WTC/WTS; bitpos: [26]; default: 0;
 *  Represents ch26 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA26    (BIT(26))
#define SOC_ETM_CH_ENA26_M  (SOC_ETM_CH_ENA26_V << SOC_ETM_CH_ENA26_S)
#define SOC_ETM_CH_ENA26_V  0x00000001U
#define SOC_ETM_CH_ENA26_S  26
/** SOC_ETM_CH_ENA27 : R/WTC/WTS; bitpos: [27]; default: 0;
 *  Represents ch27 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA27    (BIT(27))
#define SOC_ETM_CH_ENA27_M  (SOC_ETM_CH_ENA27_V << SOC_ETM_CH_ENA27_S)
#define SOC_ETM_CH_ENA27_V  0x00000001U
#define SOC_ETM_CH_ENA27_S  27
/** SOC_ETM_CH_ENA28 : R/WTC/WTS; bitpos: [28]; default: 0;
 *  Represents ch28 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA28    (BIT(28))
#define SOC_ETM_CH_ENA28_M  (SOC_ETM_CH_ENA28_V << SOC_ETM_CH_ENA28_S)
#define SOC_ETM_CH_ENA28_V  0x00000001U
#define SOC_ETM_CH_ENA28_S  28
/** SOC_ETM_CH_ENA29 : R/WTC/WTS; bitpos: [29]; default: 0;
 *  Represents ch29 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA29    (BIT(29))
#define SOC_ETM_CH_ENA29_M  (SOC_ETM_CH_ENA29_V << SOC_ETM_CH_ENA29_S)
#define SOC_ETM_CH_ENA29_V  0x00000001U
#define SOC_ETM_CH_ENA29_S  29
/** SOC_ETM_CH_ENA30 : R/WTC/WTS; bitpos: [30]; default: 0;
 *  Represents ch30 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA30    (BIT(30))
#define SOC_ETM_CH_ENA30_M  (SOC_ETM_CH_ENA30_V << SOC_ETM_CH_ENA30_S)
#define SOC_ETM_CH_ENA30_V  0x00000001U
#define SOC_ETM_CH_ENA30_S  30
/** SOC_ETM_CH_ENA31 : R/WTC/WTS; bitpos: [31]; default: 0;
 *  Represents ch31 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA31    (BIT(31))
#define SOC_ETM_CH_ENA31_M  (SOC_ETM_CH_ENA31_V << SOC_ETM_CH_ENA31_S)
#define SOC_ETM_CH_ENA31_V  0x00000001U
#define SOC_ETM_CH_ENA31_S  31

/** SOC_ETM_CH_ENA_AD0_SET_REG register
 *  Channel enable set register
 */
#define SOC_ETM_CH_ENA_AD0_SET_REG (DR_REG_SOC_ETM_BASE + 0x4)
/** SOC_ETM_CH_SET0 : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to enable ch0.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET0    (BIT(0))
#define SOC_ETM_CH_SET0_M  (SOC_ETM_CH_SET0_V << SOC_ETM_CH_SET0_S)
#define SOC_ETM_CH_SET0_V  0x00000001U
#define SOC_ETM_CH_SET0_S  0
/** SOC_ETM_CH_SET1 : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to enable ch1.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET1    (BIT(1))
#define SOC_ETM_CH_SET1_M  (SOC_ETM_CH_SET1_V << SOC_ETM_CH_SET1_S)
#define SOC_ETM_CH_SET1_V  0x00000001U
#define SOC_ETM_CH_SET1_S  1
/** SOC_ETM_CH_SET2 : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to enable ch2.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET2    (BIT(2))
#define SOC_ETM_CH_SET2_M  (SOC_ETM_CH_SET2_V << SOC_ETM_CH_SET2_S)
#define SOC_ETM_CH_SET2_V  0x00000001U
#define SOC_ETM_CH_SET2_S  2
/** SOC_ETM_CH_SET3 : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to enable ch3.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET3    (BIT(3))
#define SOC_ETM_CH_SET3_M  (SOC_ETM_CH_SET3_V << SOC_ETM_CH_SET3_S)
#define SOC_ETM_CH_SET3_V  0x00000001U
#define SOC_ETM_CH_SET3_S  3
/** SOC_ETM_CH_SET4 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to enable ch4.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET4    (BIT(4))
#define SOC_ETM_CH_SET4_M  (SOC_ETM_CH_SET4_V << SOC_ETM_CH_SET4_S)
#define SOC_ETM_CH_SET4_V  0x00000001U
#define SOC_ETM_CH_SET4_S  4
/** SOC_ETM_CH_SET5 : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to enable ch5.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET5    (BIT(5))
#define SOC_ETM_CH_SET5_M  (SOC_ETM_CH_SET5_V << SOC_ETM_CH_SET5_S)
#define SOC_ETM_CH_SET5_V  0x00000001U
#define SOC_ETM_CH_SET5_S  5
/** SOC_ETM_CH_SET6 : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to enable ch6.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET6    (BIT(6))
#define SOC_ETM_CH_SET6_M  (SOC_ETM_CH_SET6_V << SOC_ETM_CH_SET6_S)
#define SOC_ETM_CH_SET6_V  0x00000001U
#define SOC_ETM_CH_SET6_S  6
/** SOC_ETM_CH_SET7 : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ch7.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET7    (BIT(7))
#define SOC_ETM_CH_SET7_M  (SOC_ETM_CH_SET7_V << SOC_ETM_CH_SET7_S)
#define SOC_ETM_CH_SET7_V  0x00000001U
#define SOC_ETM_CH_SET7_S  7
/** SOC_ETM_CH_SET8 : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to enable ch8.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET8    (BIT(8))
#define SOC_ETM_CH_SET8_M  (SOC_ETM_CH_SET8_V << SOC_ETM_CH_SET8_S)
#define SOC_ETM_CH_SET8_V  0x00000001U
#define SOC_ETM_CH_SET8_S  8
/** SOC_ETM_CH_SET9 : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to enable ch9.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET9    (BIT(9))
#define SOC_ETM_CH_SET9_M  (SOC_ETM_CH_SET9_V << SOC_ETM_CH_SET9_S)
#define SOC_ETM_CH_SET9_V  0x00000001U
#define SOC_ETM_CH_SET9_S  9
/** SOC_ETM_CH_SET10 : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to enable ch10.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET10    (BIT(10))
#define SOC_ETM_CH_SET10_M  (SOC_ETM_CH_SET10_V << SOC_ETM_CH_SET10_S)
#define SOC_ETM_CH_SET10_V  0x00000001U
#define SOC_ETM_CH_SET10_S  10
/** SOC_ETM_CH_SET11 : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to enable ch11.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET11    (BIT(11))
#define SOC_ETM_CH_SET11_M  (SOC_ETM_CH_SET11_V << SOC_ETM_CH_SET11_S)
#define SOC_ETM_CH_SET11_V  0x00000001U
#define SOC_ETM_CH_SET11_S  11
/** SOC_ETM_CH_SET12 : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to enable ch12.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET12    (BIT(12))
#define SOC_ETM_CH_SET12_M  (SOC_ETM_CH_SET12_V << SOC_ETM_CH_SET12_S)
#define SOC_ETM_CH_SET12_V  0x00000001U
#define SOC_ETM_CH_SET12_S  12
/** SOC_ETM_CH_SET13 : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to enable ch13.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET13    (BIT(13))
#define SOC_ETM_CH_SET13_M  (SOC_ETM_CH_SET13_V << SOC_ETM_CH_SET13_S)
#define SOC_ETM_CH_SET13_V  0x00000001U
#define SOC_ETM_CH_SET13_S  13
/** SOC_ETM_CH_SET14 : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to enable ch14.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET14    (BIT(14))
#define SOC_ETM_CH_SET14_M  (SOC_ETM_CH_SET14_V << SOC_ETM_CH_SET14_S)
#define SOC_ETM_CH_SET14_V  0x00000001U
#define SOC_ETM_CH_SET14_S  14
/** SOC_ETM_CH_SET15 : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to enable ch15.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET15    (BIT(15))
#define SOC_ETM_CH_SET15_M  (SOC_ETM_CH_SET15_V << SOC_ETM_CH_SET15_S)
#define SOC_ETM_CH_SET15_V  0x00000001U
#define SOC_ETM_CH_SET15_S  15
/** SOC_ETM_CH_SET16 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to enable ch16.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET16    (BIT(16))
#define SOC_ETM_CH_SET16_M  (SOC_ETM_CH_SET16_V << SOC_ETM_CH_SET16_S)
#define SOC_ETM_CH_SET16_V  0x00000001U
#define SOC_ETM_CH_SET16_S  16
/** SOC_ETM_CH_SET17 : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to enable ch17.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET17    (BIT(17))
#define SOC_ETM_CH_SET17_M  (SOC_ETM_CH_SET17_V << SOC_ETM_CH_SET17_S)
#define SOC_ETM_CH_SET17_V  0x00000001U
#define SOC_ETM_CH_SET17_S  17
/** SOC_ETM_CH_SET18 : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to enable ch18.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET18    (BIT(18))
#define SOC_ETM_CH_SET18_M  (SOC_ETM_CH_SET18_V << SOC_ETM_CH_SET18_S)
#define SOC_ETM_CH_SET18_V  0x00000001U
#define SOC_ETM_CH_SET18_S  18
/** SOC_ETM_CH_SET19 : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to enable ch19.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET19    (BIT(19))
#define SOC_ETM_CH_SET19_M  (SOC_ETM_CH_SET19_V << SOC_ETM_CH_SET19_S)
#define SOC_ETM_CH_SET19_V  0x00000001U
#define SOC_ETM_CH_SET19_S  19
/** SOC_ETM_CH_SET20 : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to enable ch20.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET20    (BIT(20))
#define SOC_ETM_CH_SET20_M  (SOC_ETM_CH_SET20_V << SOC_ETM_CH_SET20_S)
#define SOC_ETM_CH_SET20_V  0x00000001U
#define SOC_ETM_CH_SET20_S  20
/** SOC_ETM_CH_SET21 : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to enable ch21.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET21    (BIT(21))
#define SOC_ETM_CH_SET21_M  (SOC_ETM_CH_SET21_V << SOC_ETM_CH_SET21_S)
#define SOC_ETM_CH_SET21_V  0x00000001U
#define SOC_ETM_CH_SET21_S  21
/** SOC_ETM_CH_SET22 : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to enable ch22.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET22    (BIT(22))
#define SOC_ETM_CH_SET22_M  (SOC_ETM_CH_SET22_V << SOC_ETM_CH_SET22_S)
#define SOC_ETM_CH_SET22_V  0x00000001U
#define SOC_ETM_CH_SET22_S  22
/** SOC_ETM_CH_SET23 : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to enable ch23.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET23    (BIT(23))
#define SOC_ETM_CH_SET23_M  (SOC_ETM_CH_SET23_V << SOC_ETM_CH_SET23_S)
#define SOC_ETM_CH_SET23_V  0x00000001U
#define SOC_ETM_CH_SET23_S  23
/** SOC_ETM_CH_SET24 : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to enable ch24.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET24    (BIT(24))
#define SOC_ETM_CH_SET24_M  (SOC_ETM_CH_SET24_V << SOC_ETM_CH_SET24_S)
#define SOC_ETM_CH_SET24_V  0x00000001U
#define SOC_ETM_CH_SET24_S  24
/** SOC_ETM_CH_SET25 : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to enable ch25.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET25    (BIT(25))
#define SOC_ETM_CH_SET25_M  (SOC_ETM_CH_SET25_V << SOC_ETM_CH_SET25_S)
#define SOC_ETM_CH_SET25_V  0x00000001U
#define SOC_ETM_CH_SET25_S  25
/** SOC_ETM_CH_SET26 : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to enable ch26.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET26    (BIT(26))
#define SOC_ETM_CH_SET26_M  (SOC_ETM_CH_SET26_V << SOC_ETM_CH_SET26_S)
#define SOC_ETM_CH_SET26_V  0x00000001U
#define SOC_ETM_CH_SET26_S  26
/** SOC_ETM_CH_SET27 : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to enable ch27.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET27    (BIT(27))
#define SOC_ETM_CH_SET27_M  (SOC_ETM_CH_SET27_V << SOC_ETM_CH_SET27_S)
#define SOC_ETM_CH_SET27_V  0x00000001U
#define SOC_ETM_CH_SET27_S  27
/** SOC_ETM_CH_SET28 : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to enable ch28.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET28    (BIT(28))
#define SOC_ETM_CH_SET28_M  (SOC_ETM_CH_SET28_V << SOC_ETM_CH_SET28_S)
#define SOC_ETM_CH_SET28_V  0x00000001U
#define SOC_ETM_CH_SET28_S  28
/** SOC_ETM_CH_SET29 : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to enable ch29.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET29    (BIT(29))
#define SOC_ETM_CH_SET29_M  (SOC_ETM_CH_SET29_V << SOC_ETM_CH_SET29_S)
#define SOC_ETM_CH_SET29_V  0x00000001U
#define SOC_ETM_CH_SET29_S  29
/** SOC_ETM_CH_SET30 : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to enable ch30.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET30    (BIT(30))
#define SOC_ETM_CH_SET30_M  (SOC_ETM_CH_SET30_V << SOC_ETM_CH_SET30_S)
#define SOC_ETM_CH_SET30_V  0x00000001U
#define SOC_ETM_CH_SET30_S  30
/** SOC_ETM_CH_SET31 : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to enable ch31.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET31    (BIT(31))
#define SOC_ETM_CH_SET31_M  (SOC_ETM_CH_SET31_V << SOC_ETM_CH_SET31_S)
#define SOC_ETM_CH_SET31_V  0x00000001U
#define SOC_ETM_CH_SET31_S  31

/** SOC_ETM_CH_ENA_AD0_CLR_REG register
 *  Channel enable clear register
 */
#define SOC_ETM_CH_ENA_AD0_CLR_REG (DR_REG_SOC_ETM_BASE + 0x8)
/** SOC_ETM_CH_CLR0 : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear ch0 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR0    (BIT(0))
#define SOC_ETM_CH_CLR0_M  (SOC_ETM_CH_CLR0_V << SOC_ETM_CH_CLR0_S)
#define SOC_ETM_CH_CLR0_V  0x00000001U
#define SOC_ETM_CH_CLR0_S  0
/** SOC_ETM_CH_CLR1 : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear ch1 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR1    (BIT(1))
#define SOC_ETM_CH_CLR1_M  (SOC_ETM_CH_CLR1_V << SOC_ETM_CH_CLR1_S)
#define SOC_ETM_CH_CLR1_V  0x00000001U
#define SOC_ETM_CH_CLR1_S  1
/** SOC_ETM_CH_CLR2 : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear ch2 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR2    (BIT(2))
#define SOC_ETM_CH_CLR2_M  (SOC_ETM_CH_CLR2_V << SOC_ETM_CH_CLR2_S)
#define SOC_ETM_CH_CLR2_V  0x00000001U
#define SOC_ETM_CH_CLR2_S  2
/** SOC_ETM_CH_CLR3 : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear ch3 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR3    (BIT(3))
#define SOC_ETM_CH_CLR3_M  (SOC_ETM_CH_CLR3_V << SOC_ETM_CH_CLR3_S)
#define SOC_ETM_CH_CLR3_V  0x00000001U
#define SOC_ETM_CH_CLR3_S  3
/** SOC_ETM_CH_CLR4 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear ch4 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR4    (BIT(4))
#define SOC_ETM_CH_CLR4_M  (SOC_ETM_CH_CLR4_V << SOC_ETM_CH_CLR4_S)
#define SOC_ETM_CH_CLR4_V  0x00000001U
#define SOC_ETM_CH_CLR4_S  4
/** SOC_ETM_CH_CLR5 : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear ch5 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR5    (BIT(5))
#define SOC_ETM_CH_CLR5_M  (SOC_ETM_CH_CLR5_V << SOC_ETM_CH_CLR5_S)
#define SOC_ETM_CH_CLR5_V  0x00000001U
#define SOC_ETM_CH_CLR5_S  5
/** SOC_ETM_CH_CLR6 : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear ch6 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR6    (BIT(6))
#define SOC_ETM_CH_CLR6_M  (SOC_ETM_CH_CLR6_V << SOC_ETM_CH_CLR6_S)
#define SOC_ETM_CH_CLR6_V  0x00000001U
#define SOC_ETM_CH_CLR6_S  6
/** SOC_ETM_CH_CLR7 : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear ch7 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR7    (BIT(7))
#define SOC_ETM_CH_CLR7_M  (SOC_ETM_CH_CLR7_V << SOC_ETM_CH_CLR7_S)
#define SOC_ETM_CH_CLR7_V  0x00000001U
#define SOC_ETM_CH_CLR7_S  7
/** SOC_ETM_CH_CLR8 : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear ch8 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR8    (BIT(8))
#define SOC_ETM_CH_CLR8_M  (SOC_ETM_CH_CLR8_V << SOC_ETM_CH_CLR8_S)
#define SOC_ETM_CH_CLR8_V  0x00000001U
#define SOC_ETM_CH_CLR8_S  8
/** SOC_ETM_CH_CLR9 : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear ch9 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR9    (BIT(9))
#define SOC_ETM_CH_CLR9_M  (SOC_ETM_CH_CLR9_V << SOC_ETM_CH_CLR9_S)
#define SOC_ETM_CH_CLR9_V  0x00000001U
#define SOC_ETM_CH_CLR9_S  9
/** SOC_ETM_CH_CLR10 : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear ch10 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR10    (BIT(10))
#define SOC_ETM_CH_CLR10_M  (SOC_ETM_CH_CLR10_V << SOC_ETM_CH_CLR10_S)
#define SOC_ETM_CH_CLR10_V  0x00000001U
#define SOC_ETM_CH_CLR10_S  10
/** SOC_ETM_CH_CLR11 : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear ch11 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR11    (BIT(11))
#define SOC_ETM_CH_CLR11_M  (SOC_ETM_CH_CLR11_V << SOC_ETM_CH_CLR11_S)
#define SOC_ETM_CH_CLR11_V  0x00000001U
#define SOC_ETM_CH_CLR11_S  11
/** SOC_ETM_CH_CLR12 : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear ch12 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR12    (BIT(12))
#define SOC_ETM_CH_CLR12_M  (SOC_ETM_CH_CLR12_V << SOC_ETM_CH_CLR12_S)
#define SOC_ETM_CH_CLR12_V  0x00000001U
#define SOC_ETM_CH_CLR12_S  12
/** SOC_ETM_CH_CLR13 : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear ch13 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR13    (BIT(13))
#define SOC_ETM_CH_CLR13_M  (SOC_ETM_CH_CLR13_V << SOC_ETM_CH_CLR13_S)
#define SOC_ETM_CH_CLR13_V  0x00000001U
#define SOC_ETM_CH_CLR13_S  13
/** SOC_ETM_CH_CLR14 : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear ch14 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR14    (BIT(14))
#define SOC_ETM_CH_CLR14_M  (SOC_ETM_CH_CLR14_V << SOC_ETM_CH_CLR14_S)
#define SOC_ETM_CH_CLR14_V  0x00000001U
#define SOC_ETM_CH_CLR14_S  14
/** SOC_ETM_CH_CLR15 : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear ch15 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR15    (BIT(15))
#define SOC_ETM_CH_CLR15_M  (SOC_ETM_CH_CLR15_V << SOC_ETM_CH_CLR15_S)
#define SOC_ETM_CH_CLR15_V  0x00000001U
#define SOC_ETM_CH_CLR15_S  15
/** SOC_ETM_CH_CLR16 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear ch16 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR16    (BIT(16))
#define SOC_ETM_CH_CLR16_M  (SOC_ETM_CH_CLR16_V << SOC_ETM_CH_CLR16_S)
#define SOC_ETM_CH_CLR16_V  0x00000001U
#define SOC_ETM_CH_CLR16_S  16
/** SOC_ETM_CH_CLR17 : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear ch17 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR17    (BIT(17))
#define SOC_ETM_CH_CLR17_M  (SOC_ETM_CH_CLR17_V << SOC_ETM_CH_CLR17_S)
#define SOC_ETM_CH_CLR17_V  0x00000001U
#define SOC_ETM_CH_CLR17_S  17
/** SOC_ETM_CH_CLR18 : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear ch18 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR18    (BIT(18))
#define SOC_ETM_CH_CLR18_M  (SOC_ETM_CH_CLR18_V << SOC_ETM_CH_CLR18_S)
#define SOC_ETM_CH_CLR18_V  0x00000001U
#define SOC_ETM_CH_CLR18_S  18
/** SOC_ETM_CH_CLR19 : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear ch19 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR19    (BIT(19))
#define SOC_ETM_CH_CLR19_M  (SOC_ETM_CH_CLR19_V << SOC_ETM_CH_CLR19_S)
#define SOC_ETM_CH_CLR19_V  0x00000001U
#define SOC_ETM_CH_CLR19_S  19
/** SOC_ETM_CH_CLR20 : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear ch20 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR20    (BIT(20))
#define SOC_ETM_CH_CLR20_M  (SOC_ETM_CH_CLR20_V << SOC_ETM_CH_CLR20_S)
#define SOC_ETM_CH_CLR20_V  0x00000001U
#define SOC_ETM_CH_CLR20_S  20
/** SOC_ETM_CH_CLR21 : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear ch21 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR21    (BIT(21))
#define SOC_ETM_CH_CLR21_M  (SOC_ETM_CH_CLR21_V << SOC_ETM_CH_CLR21_S)
#define SOC_ETM_CH_CLR21_V  0x00000001U
#define SOC_ETM_CH_CLR21_S  21
/** SOC_ETM_CH_CLR22 : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear ch22 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR22    (BIT(22))
#define SOC_ETM_CH_CLR22_M  (SOC_ETM_CH_CLR22_V << SOC_ETM_CH_CLR22_S)
#define SOC_ETM_CH_CLR22_V  0x00000001U
#define SOC_ETM_CH_CLR22_S  22
/** SOC_ETM_CH_CLR23 : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear ch23 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR23    (BIT(23))
#define SOC_ETM_CH_CLR23_M  (SOC_ETM_CH_CLR23_V << SOC_ETM_CH_CLR23_S)
#define SOC_ETM_CH_CLR23_V  0x00000001U
#define SOC_ETM_CH_CLR23_S  23
/** SOC_ETM_CH_CLR24 : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear ch24 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR24    (BIT(24))
#define SOC_ETM_CH_CLR24_M  (SOC_ETM_CH_CLR24_V << SOC_ETM_CH_CLR24_S)
#define SOC_ETM_CH_CLR24_V  0x00000001U
#define SOC_ETM_CH_CLR24_S  24
/** SOC_ETM_CH_CLR25 : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear ch25 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR25    (BIT(25))
#define SOC_ETM_CH_CLR25_M  (SOC_ETM_CH_CLR25_V << SOC_ETM_CH_CLR25_S)
#define SOC_ETM_CH_CLR25_V  0x00000001U
#define SOC_ETM_CH_CLR25_S  25
/** SOC_ETM_CH_CLR26 : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear ch26 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR26    (BIT(26))
#define SOC_ETM_CH_CLR26_M  (SOC_ETM_CH_CLR26_V << SOC_ETM_CH_CLR26_S)
#define SOC_ETM_CH_CLR26_V  0x00000001U
#define SOC_ETM_CH_CLR26_S  26
/** SOC_ETM_CH_CLR27 : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear ch27 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR27    (BIT(27))
#define SOC_ETM_CH_CLR27_M  (SOC_ETM_CH_CLR27_V << SOC_ETM_CH_CLR27_S)
#define SOC_ETM_CH_CLR27_V  0x00000001U
#define SOC_ETM_CH_CLR27_S  27
/** SOC_ETM_CH_CLR28 : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear ch28 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR28    (BIT(28))
#define SOC_ETM_CH_CLR28_M  (SOC_ETM_CH_CLR28_V << SOC_ETM_CH_CLR28_S)
#define SOC_ETM_CH_CLR28_V  0x00000001U
#define SOC_ETM_CH_CLR28_S  28
/** SOC_ETM_CH_CLR29 : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear ch29 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR29    (BIT(29))
#define SOC_ETM_CH_CLR29_M  (SOC_ETM_CH_CLR29_V << SOC_ETM_CH_CLR29_S)
#define SOC_ETM_CH_CLR29_V  0x00000001U
#define SOC_ETM_CH_CLR29_S  29
/** SOC_ETM_CH_CLR30 : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear ch30 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR30    (BIT(30))
#define SOC_ETM_CH_CLR30_M  (SOC_ETM_CH_CLR30_V << SOC_ETM_CH_CLR30_S)
#define SOC_ETM_CH_CLR30_V  0x00000001U
#define SOC_ETM_CH_CLR30_S  30
/** SOC_ETM_CH_CLR31 : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear ch31 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR31    (BIT(31))
#define SOC_ETM_CH_CLR31_M  (SOC_ETM_CH_CLR31_V << SOC_ETM_CH_CLR31_S)
#define SOC_ETM_CH_CLR31_V  0x00000001U
#define SOC_ETM_CH_CLR31_S  31

/** SOC_ETM_CH_ENA_AD1_REG register
 *  Channel enable status register
 */
#define SOC_ETM_CH_ENA_AD1_REG (DR_REG_SOC_ETM_BASE + 0xc)
/** SOC_ETM_CH_ENA32 : R/WTC/WTS; bitpos: [0]; default: 0;
 *  Represents ch32 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA32    (BIT(0))
#define SOC_ETM_CH_ENA32_M  (SOC_ETM_CH_ENA32_V << SOC_ETM_CH_ENA32_S)
#define SOC_ETM_CH_ENA32_V  0x00000001U
#define SOC_ETM_CH_ENA32_S  0
/** SOC_ETM_CH_ENA33 : R/WTC/WTS; bitpos: [1]; default: 0;
 *  Represents ch33 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA33    (BIT(1))
#define SOC_ETM_CH_ENA33_M  (SOC_ETM_CH_ENA33_V << SOC_ETM_CH_ENA33_S)
#define SOC_ETM_CH_ENA33_V  0x00000001U
#define SOC_ETM_CH_ENA33_S  1
/** SOC_ETM_CH_ENA34 : R/WTC/WTS; bitpos: [2]; default: 0;
 *  Represents ch34 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA34    (BIT(2))
#define SOC_ETM_CH_ENA34_M  (SOC_ETM_CH_ENA34_V << SOC_ETM_CH_ENA34_S)
#define SOC_ETM_CH_ENA34_V  0x00000001U
#define SOC_ETM_CH_ENA34_S  2
/** SOC_ETM_CH_ENA35 : R/WTC/WTS; bitpos: [3]; default: 0;
 *  Represents ch35 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA35    (BIT(3))
#define SOC_ETM_CH_ENA35_M  (SOC_ETM_CH_ENA35_V << SOC_ETM_CH_ENA35_S)
#define SOC_ETM_CH_ENA35_V  0x00000001U
#define SOC_ETM_CH_ENA35_S  3
/** SOC_ETM_CH_ENA36 : R/WTC/WTS; bitpos: [4]; default: 0;
 *  Represents ch36 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA36    (BIT(4))
#define SOC_ETM_CH_ENA36_M  (SOC_ETM_CH_ENA36_V << SOC_ETM_CH_ENA36_S)
#define SOC_ETM_CH_ENA36_V  0x00000001U
#define SOC_ETM_CH_ENA36_S  4
/** SOC_ETM_CH_ENA37 : R/WTC/WTS; bitpos: [5]; default: 0;
 *  Represents ch37 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA37    (BIT(5))
#define SOC_ETM_CH_ENA37_M  (SOC_ETM_CH_ENA37_V << SOC_ETM_CH_ENA37_S)
#define SOC_ETM_CH_ENA37_V  0x00000001U
#define SOC_ETM_CH_ENA37_S  5
/** SOC_ETM_CH_ENA38 : R/WTC/WTS; bitpos: [6]; default: 0;
 *  Represents ch38 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA38    (BIT(6))
#define SOC_ETM_CH_ENA38_M  (SOC_ETM_CH_ENA38_V << SOC_ETM_CH_ENA38_S)
#define SOC_ETM_CH_ENA38_V  0x00000001U
#define SOC_ETM_CH_ENA38_S  6
/** SOC_ETM_CH_ENA39 : R/WTC/WTS; bitpos: [7]; default: 0;
 *  Represents ch39 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA39    (BIT(7))
#define SOC_ETM_CH_ENA39_M  (SOC_ETM_CH_ENA39_V << SOC_ETM_CH_ENA39_S)
#define SOC_ETM_CH_ENA39_V  0x00000001U
#define SOC_ETM_CH_ENA39_S  7
/** SOC_ETM_CH_ENA40 : R/WTC/WTS; bitpos: [8]; default: 0;
 *  Represents ch40 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA40    (BIT(8))
#define SOC_ETM_CH_ENA40_M  (SOC_ETM_CH_ENA40_V << SOC_ETM_CH_ENA40_S)
#define SOC_ETM_CH_ENA40_V  0x00000001U
#define SOC_ETM_CH_ENA40_S  8
/** SOC_ETM_CH_ENA41 : R/WTC/WTS; bitpos: [9]; default: 0;
 *  Represents ch41 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA41    (BIT(9))
#define SOC_ETM_CH_ENA41_M  (SOC_ETM_CH_ENA41_V << SOC_ETM_CH_ENA41_S)
#define SOC_ETM_CH_ENA41_V  0x00000001U
#define SOC_ETM_CH_ENA41_S  9
/** SOC_ETM_CH_ENA42 : R/WTC/WTS; bitpos: [10]; default: 0;
 *  Represents ch42 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA42    (BIT(10))
#define SOC_ETM_CH_ENA42_M  (SOC_ETM_CH_ENA42_V << SOC_ETM_CH_ENA42_S)
#define SOC_ETM_CH_ENA42_V  0x00000001U
#define SOC_ETM_CH_ENA42_S  10
/** SOC_ETM_CH_ENA43 : R/WTC/WTS; bitpos: [11]; default: 0;
 *  Represents ch43 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA43    (BIT(11))
#define SOC_ETM_CH_ENA43_M  (SOC_ETM_CH_ENA43_V << SOC_ETM_CH_ENA43_S)
#define SOC_ETM_CH_ENA43_V  0x00000001U
#define SOC_ETM_CH_ENA43_S  11
/** SOC_ETM_CH_ENA44 : R/WTC/WTS; bitpos: [12]; default: 0;
 *  Represents ch44 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA44    (BIT(12))
#define SOC_ETM_CH_ENA44_M  (SOC_ETM_CH_ENA44_V << SOC_ETM_CH_ENA44_S)
#define SOC_ETM_CH_ENA44_V  0x00000001U
#define SOC_ETM_CH_ENA44_S  12
/** SOC_ETM_CH_ENA45 : R/WTC/WTS; bitpos: [13]; default: 0;
 *  Represents ch45 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA45    (BIT(13))
#define SOC_ETM_CH_ENA45_M  (SOC_ETM_CH_ENA45_V << SOC_ETM_CH_ENA45_S)
#define SOC_ETM_CH_ENA45_V  0x00000001U
#define SOC_ETM_CH_ENA45_S  13
/** SOC_ETM_CH_ENA46 : R/WTC/WTS; bitpos: [14]; default: 0;
 *  Represents ch46 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA46    (BIT(14))
#define SOC_ETM_CH_ENA46_M  (SOC_ETM_CH_ENA46_V << SOC_ETM_CH_ENA46_S)
#define SOC_ETM_CH_ENA46_V  0x00000001U
#define SOC_ETM_CH_ENA46_S  14
/** SOC_ETM_CH_ENA47 : R/WTC/WTS; bitpos: [15]; default: 0;
 *  Represents ch47 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA47    (BIT(15))
#define SOC_ETM_CH_ENA47_M  (SOC_ETM_CH_ENA47_V << SOC_ETM_CH_ENA47_S)
#define SOC_ETM_CH_ENA47_V  0x00000001U
#define SOC_ETM_CH_ENA47_S  15
/** SOC_ETM_CH_ENA48 : R/WTC/WTS; bitpos: [16]; default: 0;
 *  Represents ch48 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA48    (BIT(16))
#define SOC_ETM_CH_ENA48_M  (SOC_ETM_CH_ENA48_V << SOC_ETM_CH_ENA48_S)
#define SOC_ETM_CH_ENA48_V  0x00000001U
#define SOC_ETM_CH_ENA48_S  16
/** SOC_ETM_CH_ENA49 : R/WTC/WTS; bitpos: [17]; default: 0;
 *  Represents ch49 enable status.\\0: Disable\\1: Enable
 */
#define SOC_ETM_CH_ENA49    (BIT(17))
#define SOC_ETM_CH_ENA49_M  (SOC_ETM_CH_ENA49_V << SOC_ETM_CH_ENA49_S)
#define SOC_ETM_CH_ENA49_V  0x00000001U
#define SOC_ETM_CH_ENA49_S  17

/** SOC_ETM_CH_ENA_AD1_SET_REG register
 *  Channel enable set register
 */
#define SOC_ETM_CH_ENA_AD1_SET_REG (DR_REG_SOC_ETM_BASE + 0x10)
/** SOC_ETM_CH_SET32 : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to enable ch32.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET32    (BIT(0))
#define SOC_ETM_CH_SET32_M  (SOC_ETM_CH_SET32_V << SOC_ETM_CH_SET32_S)
#define SOC_ETM_CH_SET32_V  0x00000001U
#define SOC_ETM_CH_SET32_S  0
/** SOC_ETM_CH_SET33 : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to enable ch33.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET33    (BIT(1))
#define SOC_ETM_CH_SET33_M  (SOC_ETM_CH_SET33_V << SOC_ETM_CH_SET33_S)
#define SOC_ETM_CH_SET33_V  0x00000001U
#define SOC_ETM_CH_SET33_S  1
/** SOC_ETM_CH_SET34 : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to enable ch34.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET34    (BIT(2))
#define SOC_ETM_CH_SET34_M  (SOC_ETM_CH_SET34_V << SOC_ETM_CH_SET34_S)
#define SOC_ETM_CH_SET34_V  0x00000001U
#define SOC_ETM_CH_SET34_S  2
/** SOC_ETM_CH_SET35 : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to enable ch35.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET35    (BIT(3))
#define SOC_ETM_CH_SET35_M  (SOC_ETM_CH_SET35_V << SOC_ETM_CH_SET35_S)
#define SOC_ETM_CH_SET35_V  0x00000001U
#define SOC_ETM_CH_SET35_S  3
/** SOC_ETM_CH_SET36 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to enable ch36.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET36    (BIT(4))
#define SOC_ETM_CH_SET36_M  (SOC_ETM_CH_SET36_V << SOC_ETM_CH_SET36_S)
#define SOC_ETM_CH_SET36_V  0x00000001U
#define SOC_ETM_CH_SET36_S  4
/** SOC_ETM_CH_SET37 : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to enable ch37.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET37    (BIT(5))
#define SOC_ETM_CH_SET37_M  (SOC_ETM_CH_SET37_V << SOC_ETM_CH_SET37_S)
#define SOC_ETM_CH_SET37_V  0x00000001U
#define SOC_ETM_CH_SET37_S  5
/** SOC_ETM_CH_SET38 : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to enable ch38.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET38    (BIT(6))
#define SOC_ETM_CH_SET38_M  (SOC_ETM_CH_SET38_V << SOC_ETM_CH_SET38_S)
#define SOC_ETM_CH_SET38_V  0x00000001U
#define SOC_ETM_CH_SET38_S  6
/** SOC_ETM_CH_SET39 : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ch39.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET39    (BIT(7))
#define SOC_ETM_CH_SET39_M  (SOC_ETM_CH_SET39_V << SOC_ETM_CH_SET39_S)
#define SOC_ETM_CH_SET39_V  0x00000001U
#define SOC_ETM_CH_SET39_S  7
/** SOC_ETM_CH_SET40 : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to enable ch40.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET40    (BIT(8))
#define SOC_ETM_CH_SET40_M  (SOC_ETM_CH_SET40_V << SOC_ETM_CH_SET40_S)
#define SOC_ETM_CH_SET40_V  0x00000001U
#define SOC_ETM_CH_SET40_S  8
/** SOC_ETM_CH_SET41 : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to enable ch41.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET41    (BIT(9))
#define SOC_ETM_CH_SET41_M  (SOC_ETM_CH_SET41_V << SOC_ETM_CH_SET41_S)
#define SOC_ETM_CH_SET41_V  0x00000001U
#define SOC_ETM_CH_SET41_S  9
/** SOC_ETM_CH_SET42 : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to enable ch42.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET42    (BIT(10))
#define SOC_ETM_CH_SET42_M  (SOC_ETM_CH_SET42_V << SOC_ETM_CH_SET42_S)
#define SOC_ETM_CH_SET42_V  0x00000001U
#define SOC_ETM_CH_SET42_S  10
/** SOC_ETM_CH_SET43 : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to enable ch43.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET43    (BIT(11))
#define SOC_ETM_CH_SET43_M  (SOC_ETM_CH_SET43_V << SOC_ETM_CH_SET43_S)
#define SOC_ETM_CH_SET43_V  0x00000001U
#define SOC_ETM_CH_SET43_S  11
/** SOC_ETM_CH_SET44 : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to enable ch44.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET44    (BIT(12))
#define SOC_ETM_CH_SET44_M  (SOC_ETM_CH_SET44_V << SOC_ETM_CH_SET44_S)
#define SOC_ETM_CH_SET44_V  0x00000001U
#define SOC_ETM_CH_SET44_S  12
/** SOC_ETM_CH_SET45 : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to enable ch45.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET45    (BIT(13))
#define SOC_ETM_CH_SET45_M  (SOC_ETM_CH_SET45_V << SOC_ETM_CH_SET45_S)
#define SOC_ETM_CH_SET45_V  0x00000001U
#define SOC_ETM_CH_SET45_S  13
/** SOC_ETM_CH_SET46 : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to enable ch46.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET46    (BIT(14))
#define SOC_ETM_CH_SET46_M  (SOC_ETM_CH_SET46_V << SOC_ETM_CH_SET46_S)
#define SOC_ETM_CH_SET46_V  0x00000001U
#define SOC_ETM_CH_SET46_S  14
/** SOC_ETM_CH_SET47 : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to enable ch47.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET47    (BIT(15))
#define SOC_ETM_CH_SET47_M  (SOC_ETM_CH_SET47_V << SOC_ETM_CH_SET47_S)
#define SOC_ETM_CH_SET47_V  0x00000001U
#define SOC_ETM_CH_SET47_S  15
/** SOC_ETM_CH_SET48 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to enable ch48.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET48    (BIT(16))
#define SOC_ETM_CH_SET48_M  (SOC_ETM_CH_SET48_V << SOC_ETM_CH_SET48_S)
#define SOC_ETM_CH_SET48_V  0x00000001U
#define SOC_ETM_CH_SET48_S  16
/** SOC_ETM_CH_SET49 : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to enable ch49.\\0: Invalid, No effect\\1: Enable
 */
#define SOC_ETM_CH_SET49    (BIT(17))
#define SOC_ETM_CH_SET49_M  (SOC_ETM_CH_SET49_V << SOC_ETM_CH_SET49_S)
#define SOC_ETM_CH_SET49_V  0x00000001U
#define SOC_ETM_CH_SET49_S  17

/** SOC_ETM_CH_ENA_AD1_CLR_REG register
 *  Channel enable clear register
 */
#define SOC_ETM_CH_ENA_AD1_CLR_REG (DR_REG_SOC_ETM_BASE + 0x14)
/** SOC_ETM_CH_CLR32 : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear ch32 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR32    (BIT(0))
#define SOC_ETM_CH_CLR32_M  (SOC_ETM_CH_CLR32_V << SOC_ETM_CH_CLR32_S)
#define SOC_ETM_CH_CLR32_V  0x00000001U
#define SOC_ETM_CH_CLR32_S  0
/** SOC_ETM_CH_CLR33 : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear ch33 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR33    (BIT(1))
#define SOC_ETM_CH_CLR33_M  (SOC_ETM_CH_CLR33_V << SOC_ETM_CH_CLR33_S)
#define SOC_ETM_CH_CLR33_V  0x00000001U
#define SOC_ETM_CH_CLR33_S  1
/** SOC_ETM_CH_CLR34 : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear ch34 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR34    (BIT(2))
#define SOC_ETM_CH_CLR34_M  (SOC_ETM_CH_CLR34_V << SOC_ETM_CH_CLR34_S)
#define SOC_ETM_CH_CLR34_V  0x00000001U
#define SOC_ETM_CH_CLR34_S  2
/** SOC_ETM_CH_CLR35 : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear ch35 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR35    (BIT(3))
#define SOC_ETM_CH_CLR35_M  (SOC_ETM_CH_CLR35_V << SOC_ETM_CH_CLR35_S)
#define SOC_ETM_CH_CLR35_V  0x00000001U
#define SOC_ETM_CH_CLR35_S  3
/** SOC_ETM_CH_CLR36 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear ch36 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR36    (BIT(4))
#define SOC_ETM_CH_CLR36_M  (SOC_ETM_CH_CLR36_V << SOC_ETM_CH_CLR36_S)
#define SOC_ETM_CH_CLR36_V  0x00000001U
#define SOC_ETM_CH_CLR36_S  4
/** SOC_ETM_CH_CLR37 : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear ch37 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR37    (BIT(5))
#define SOC_ETM_CH_CLR37_M  (SOC_ETM_CH_CLR37_V << SOC_ETM_CH_CLR37_S)
#define SOC_ETM_CH_CLR37_V  0x00000001U
#define SOC_ETM_CH_CLR37_S  5
/** SOC_ETM_CH_CLR38 : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear ch38 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR38    (BIT(6))
#define SOC_ETM_CH_CLR38_M  (SOC_ETM_CH_CLR38_V << SOC_ETM_CH_CLR38_S)
#define SOC_ETM_CH_CLR38_V  0x00000001U
#define SOC_ETM_CH_CLR38_S  6
/** SOC_ETM_CH_CLR39 : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear ch39 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR39    (BIT(7))
#define SOC_ETM_CH_CLR39_M  (SOC_ETM_CH_CLR39_V << SOC_ETM_CH_CLR39_S)
#define SOC_ETM_CH_CLR39_V  0x00000001U
#define SOC_ETM_CH_CLR39_S  7
/** SOC_ETM_CH_CLR40 : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear ch40 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR40    (BIT(8))
#define SOC_ETM_CH_CLR40_M  (SOC_ETM_CH_CLR40_V << SOC_ETM_CH_CLR40_S)
#define SOC_ETM_CH_CLR40_V  0x00000001U
#define SOC_ETM_CH_CLR40_S  8
/** SOC_ETM_CH_CLR41 : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear ch41 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR41    (BIT(9))
#define SOC_ETM_CH_CLR41_M  (SOC_ETM_CH_CLR41_V << SOC_ETM_CH_CLR41_S)
#define SOC_ETM_CH_CLR41_V  0x00000001U
#define SOC_ETM_CH_CLR41_S  9
/** SOC_ETM_CH_CLR42 : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear ch42 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR42    (BIT(10))
#define SOC_ETM_CH_CLR42_M  (SOC_ETM_CH_CLR42_V << SOC_ETM_CH_CLR42_S)
#define SOC_ETM_CH_CLR42_V  0x00000001U
#define SOC_ETM_CH_CLR42_S  10
/** SOC_ETM_CH_CLR43 : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear ch43 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR43    (BIT(11))
#define SOC_ETM_CH_CLR43_M  (SOC_ETM_CH_CLR43_V << SOC_ETM_CH_CLR43_S)
#define SOC_ETM_CH_CLR43_V  0x00000001U
#define SOC_ETM_CH_CLR43_S  11
/** SOC_ETM_CH_CLR44 : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear ch44 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR44    (BIT(12))
#define SOC_ETM_CH_CLR44_M  (SOC_ETM_CH_CLR44_V << SOC_ETM_CH_CLR44_S)
#define SOC_ETM_CH_CLR44_V  0x00000001U
#define SOC_ETM_CH_CLR44_S  12
/** SOC_ETM_CH_CLR45 : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear ch45 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR45    (BIT(13))
#define SOC_ETM_CH_CLR45_M  (SOC_ETM_CH_CLR45_V << SOC_ETM_CH_CLR45_S)
#define SOC_ETM_CH_CLR45_V  0x00000001U
#define SOC_ETM_CH_CLR45_S  13
/** SOC_ETM_CH_CLR46 : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear ch46 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR46    (BIT(14))
#define SOC_ETM_CH_CLR46_M  (SOC_ETM_CH_CLR46_V << SOC_ETM_CH_CLR46_S)
#define SOC_ETM_CH_CLR46_V  0x00000001U
#define SOC_ETM_CH_CLR46_S  14
/** SOC_ETM_CH_CLR47 : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear ch47 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR47    (BIT(15))
#define SOC_ETM_CH_CLR47_M  (SOC_ETM_CH_CLR47_V << SOC_ETM_CH_CLR47_S)
#define SOC_ETM_CH_CLR47_V  0x00000001U
#define SOC_ETM_CH_CLR47_S  15
/** SOC_ETM_CH_CLR48 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear ch48 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR48    (BIT(16))
#define SOC_ETM_CH_CLR48_M  (SOC_ETM_CH_CLR48_V << SOC_ETM_CH_CLR48_S)
#define SOC_ETM_CH_CLR48_V  0x00000001U
#define SOC_ETM_CH_CLR48_S  16
/** SOC_ETM_CH_CLR49 : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear ch49 enable.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_CH_CLR49    (BIT(17))
#define SOC_ETM_CH_CLR49_M  (SOC_ETM_CH_CLR49_V << SOC_ETM_CH_CLR49_S)
#define SOC_ETM_CH_CLR49_V  0x00000001U
#define SOC_ETM_CH_CLR49_S  17

/** SOC_ETM_CH0_EVT_ID_REG register
 *  Channel0 event id register
 */
#define SOC_ETM_CH0_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x18)
/** SOC_ETM_CH0_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch0_evt_id
 */
#define SOC_ETM_CH0_EVT_ID    0x000000FFU
#define SOC_ETM_CH0_EVT_ID_M  (SOC_ETM_CH0_EVT_ID_V << SOC_ETM_CH0_EVT_ID_S)
#define SOC_ETM_CH0_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH0_EVT_ID_S  0

/** SOC_ETM_CH0_TASK_ID_REG register
 *  Channel0 task id register
 */
#define SOC_ETM_CH0_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x1c)
/** SOC_ETM_CH0_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch0_task_id
 */
#define SOC_ETM_CH0_TASK_ID    0x000000FFU
#define SOC_ETM_CH0_TASK_ID_M  (SOC_ETM_CH0_TASK_ID_V << SOC_ETM_CH0_TASK_ID_S)
#define SOC_ETM_CH0_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH0_TASK_ID_S  0

/** SOC_ETM_CH1_EVT_ID_REG register
 *  Channel1 event id register
 */
#define SOC_ETM_CH1_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x20)
/** SOC_ETM_CH1_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch1_evt_id
 */
#define SOC_ETM_CH1_EVT_ID    0x000000FFU
#define SOC_ETM_CH1_EVT_ID_M  (SOC_ETM_CH1_EVT_ID_V << SOC_ETM_CH1_EVT_ID_S)
#define SOC_ETM_CH1_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH1_EVT_ID_S  0

/** SOC_ETM_CH1_TASK_ID_REG register
 *  Channel1 task id register
 */
#define SOC_ETM_CH1_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x24)
/** SOC_ETM_CH1_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch1_task_id
 */
#define SOC_ETM_CH1_TASK_ID    0x000000FFU
#define SOC_ETM_CH1_TASK_ID_M  (SOC_ETM_CH1_TASK_ID_V << SOC_ETM_CH1_TASK_ID_S)
#define SOC_ETM_CH1_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH1_TASK_ID_S  0

/** SOC_ETM_CH2_EVT_ID_REG register
 *  Channel2 event id register
 */
#define SOC_ETM_CH2_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x28)
/** SOC_ETM_CH2_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch2_evt_id
 */
#define SOC_ETM_CH2_EVT_ID    0x000000FFU
#define SOC_ETM_CH2_EVT_ID_M  (SOC_ETM_CH2_EVT_ID_V << SOC_ETM_CH2_EVT_ID_S)
#define SOC_ETM_CH2_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH2_EVT_ID_S  0

/** SOC_ETM_CH2_TASK_ID_REG register
 *  Channel2 task id register
 */
#define SOC_ETM_CH2_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x2c)
/** SOC_ETM_CH2_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch2_task_id
 */
#define SOC_ETM_CH2_TASK_ID    0x000000FFU
#define SOC_ETM_CH2_TASK_ID_M  (SOC_ETM_CH2_TASK_ID_V << SOC_ETM_CH2_TASK_ID_S)
#define SOC_ETM_CH2_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH2_TASK_ID_S  0

/** SOC_ETM_CH3_EVT_ID_REG register
 *  Channel3 event id register
 */
#define SOC_ETM_CH3_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x30)
/** SOC_ETM_CH3_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch3_evt_id
 */
#define SOC_ETM_CH3_EVT_ID    0x000000FFU
#define SOC_ETM_CH3_EVT_ID_M  (SOC_ETM_CH3_EVT_ID_V << SOC_ETM_CH3_EVT_ID_S)
#define SOC_ETM_CH3_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH3_EVT_ID_S  0

/** SOC_ETM_CH3_TASK_ID_REG register
 *  Channel3 task id register
 */
#define SOC_ETM_CH3_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x34)
/** SOC_ETM_CH3_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch3_task_id
 */
#define SOC_ETM_CH3_TASK_ID    0x000000FFU
#define SOC_ETM_CH3_TASK_ID_M  (SOC_ETM_CH3_TASK_ID_V << SOC_ETM_CH3_TASK_ID_S)
#define SOC_ETM_CH3_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH3_TASK_ID_S  0

/** SOC_ETM_CH4_EVT_ID_REG register
 *  Channel4 event id register
 */
#define SOC_ETM_CH4_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x38)
/** SOC_ETM_CH4_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch4_evt_id
 */
#define SOC_ETM_CH4_EVT_ID    0x000000FFU
#define SOC_ETM_CH4_EVT_ID_M  (SOC_ETM_CH4_EVT_ID_V << SOC_ETM_CH4_EVT_ID_S)
#define SOC_ETM_CH4_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH4_EVT_ID_S  0

/** SOC_ETM_CH4_TASK_ID_REG register
 *  Channel4 task id register
 */
#define SOC_ETM_CH4_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x3c)
/** SOC_ETM_CH4_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch4_task_id
 */
#define SOC_ETM_CH4_TASK_ID    0x000000FFU
#define SOC_ETM_CH4_TASK_ID_M  (SOC_ETM_CH4_TASK_ID_V << SOC_ETM_CH4_TASK_ID_S)
#define SOC_ETM_CH4_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH4_TASK_ID_S  0

/** SOC_ETM_CH5_EVT_ID_REG register
 *  Channel5 event id register
 */
#define SOC_ETM_CH5_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x40)
/** SOC_ETM_CH5_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch5_evt_id
 */
#define SOC_ETM_CH5_EVT_ID    0x000000FFU
#define SOC_ETM_CH5_EVT_ID_M  (SOC_ETM_CH5_EVT_ID_V << SOC_ETM_CH5_EVT_ID_S)
#define SOC_ETM_CH5_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH5_EVT_ID_S  0

/** SOC_ETM_CH5_TASK_ID_REG register
 *  Channel5 task id register
 */
#define SOC_ETM_CH5_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x44)
/** SOC_ETM_CH5_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch5_task_id
 */
#define SOC_ETM_CH5_TASK_ID    0x000000FFU
#define SOC_ETM_CH5_TASK_ID_M  (SOC_ETM_CH5_TASK_ID_V << SOC_ETM_CH5_TASK_ID_S)
#define SOC_ETM_CH5_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH5_TASK_ID_S  0

/** SOC_ETM_CH6_EVT_ID_REG register
 *  Channel6 event id register
 */
#define SOC_ETM_CH6_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x48)
/** SOC_ETM_CH6_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch6_evt_id
 */
#define SOC_ETM_CH6_EVT_ID    0x000000FFU
#define SOC_ETM_CH6_EVT_ID_M  (SOC_ETM_CH6_EVT_ID_V << SOC_ETM_CH6_EVT_ID_S)
#define SOC_ETM_CH6_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH6_EVT_ID_S  0

/** SOC_ETM_CH6_TASK_ID_REG register
 *  Channel6 task id register
 */
#define SOC_ETM_CH6_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x4c)
/** SOC_ETM_CH6_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch6_task_id
 */
#define SOC_ETM_CH6_TASK_ID    0x000000FFU
#define SOC_ETM_CH6_TASK_ID_M  (SOC_ETM_CH6_TASK_ID_V << SOC_ETM_CH6_TASK_ID_S)
#define SOC_ETM_CH6_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH6_TASK_ID_S  0

/** SOC_ETM_CH7_EVT_ID_REG register
 *  Channel7 event id register
 */
#define SOC_ETM_CH7_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x50)
/** SOC_ETM_CH7_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch7_evt_id
 */
#define SOC_ETM_CH7_EVT_ID    0x000000FFU
#define SOC_ETM_CH7_EVT_ID_M  (SOC_ETM_CH7_EVT_ID_V << SOC_ETM_CH7_EVT_ID_S)
#define SOC_ETM_CH7_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH7_EVT_ID_S  0

/** SOC_ETM_CH7_TASK_ID_REG register
 *  Channel7 task id register
 */
#define SOC_ETM_CH7_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x54)
/** SOC_ETM_CH7_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch7_task_id
 */
#define SOC_ETM_CH7_TASK_ID    0x000000FFU
#define SOC_ETM_CH7_TASK_ID_M  (SOC_ETM_CH7_TASK_ID_V << SOC_ETM_CH7_TASK_ID_S)
#define SOC_ETM_CH7_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH7_TASK_ID_S  0

/** SOC_ETM_CH8_EVT_ID_REG register
 *  Channel8 event id register
 */
#define SOC_ETM_CH8_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x58)
/** SOC_ETM_CH8_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch8_evt_id
 */
#define SOC_ETM_CH8_EVT_ID    0x000000FFU
#define SOC_ETM_CH8_EVT_ID_M  (SOC_ETM_CH8_EVT_ID_V << SOC_ETM_CH8_EVT_ID_S)
#define SOC_ETM_CH8_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH8_EVT_ID_S  0

/** SOC_ETM_CH8_TASK_ID_REG register
 *  Channel8 task id register
 */
#define SOC_ETM_CH8_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x5c)
/** SOC_ETM_CH8_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch8_task_id
 */
#define SOC_ETM_CH8_TASK_ID    0x000000FFU
#define SOC_ETM_CH8_TASK_ID_M  (SOC_ETM_CH8_TASK_ID_V << SOC_ETM_CH8_TASK_ID_S)
#define SOC_ETM_CH8_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH8_TASK_ID_S  0

/** SOC_ETM_CH9_EVT_ID_REG register
 *  Channel9 event id register
 */
#define SOC_ETM_CH9_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x60)
/** SOC_ETM_CH9_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch9_evt_id
 */
#define SOC_ETM_CH9_EVT_ID    0x000000FFU
#define SOC_ETM_CH9_EVT_ID_M  (SOC_ETM_CH9_EVT_ID_V << SOC_ETM_CH9_EVT_ID_S)
#define SOC_ETM_CH9_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH9_EVT_ID_S  0

/** SOC_ETM_CH9_TASK_ID_REG register
 *  Channel9 task id register
 */
#define SOC_ETM_CH9_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x64)
/** SOC_ETM_CH9_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch9_task_id
 */
#define SOC_ETM_CH9_TASK_ID    0x000000FFU
#define SOC_ETM_CH9_TASK_ID_M  (SOC_ETM_CH9_TASK_ID_V << SOC_ETM_CH9_TASK_ID_S)
#define SOC_ETM_CH9_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH9_TASK_ID_S  0

/** SOC_ETM_CH10_EVT_ID_REG register
 *  Channel10 event id register
 */
#define SOC_ETM_CH10_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x68)
/** SOC_ETM_CH10_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch10_evt_id
 */
#define SOC_ETM_CH10_EVT_ID    0x000000FFU
#define SOC_ETM_CH10_EVT_ID_M  (SOC_ETM_CH10_EVT_ID_V << SOC_ETM_CH10_EVT_ID_S)
#define SOC_ETM_CH10_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH10_EVT_ID_S  0

/** SOC_ETM_CH10_TASK_ID_REG register
 *  Channel10 task id register
 */
#define SOC_ETM_CH10_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x6c)
/** SOC_ETM_CH10_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch10_task_id
 */
#define SOC_ETM_CH10_TASK_ID    0x000000FFU
#define SOC_ETM_CH10_TASK_ID_M  (SOC_ETM_CH10_TASK_ID_V << SOC_ETM_CH10_TASK_ID_S)
#define SOC_ETM_CH10_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH10_TASK_ID_S  0

/** SOC_ETM_CH11_EVT_ID_REG register
 *  Channel11 event id register
 */
#define SOC_ETM_CH11_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x70)
/** SOC_ETM_CH11_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch11_evt_id
 */
#define SOC_ETM_CH11_EVT_ID    0x000000FFU
#define SOC_ETM_CH11_EVT_ID_M  (SOC_ETM_CH11_EVT_ID_V << SOC_ETM_CH11_EVT_ID_S)
#define SOC_ETM_CH11_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH11_EVT_ID_S  0

/** SOC_ETM_CH11_TASK_ID_REG register
 *  Channel11 task id register
 */
#define SOC_ETM_CH11_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x74)
/** SOC_ETM_CH11_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch11_task_id
 */
#define SOC_ETM_CH11_TASK_ID    0x000000FFU
#define SOC_ETM_CH11_TASK_ID_M  (SOC_ETM_CH11_TASK_ID_V << SOC_ETM_CH11_TASK_ID_S)
#define SOC_ETM_CH11_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH11_TASK_ID_S  0

/** SOC_ETM_CH12_EVT_ID_REG register
 *  Channel12 event id register
 */
#define SOC_ETM_CH12_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x78)
/** SOC_ETM_CH12_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch12_evt_id
 */
#define SOC_ETM_CH12_EVT_ID    0x000000FFU
#define SOC_ETM_CH12_EVT_ID_M  (SOC_ETM_CH12_EVT_ID_V << SOC_ETM_CH12_EVT_ID_S)
#define SOC_ETM_CH12_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH12_EVT_ID_S  0

/** SOC_ETM_CH12_TASK_ID_REG register
 *  Channel12 task id register
 */
#define SOC_ETM_CH12_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x7c)
/** SOC_ETM_CH12_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch12_task_id
 */
#define SOC_ETM_CH12_TASK_ID    0x000000FFU
#define SOC_ETM_CH12_TASK_ID_M  (SOC_ETM_CH12_TASK_ID_V << SOC_ETM_CH12_TASK_ID_S)
#define SOC_ETM_CH12_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH12_TASK_ID_S  0

/** SOC_ETM_CH13_EVT_ID_REG register
 *  Channel13 event id register
 */
#define SOC_ETM_CH13_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x80)
/** SOC_ETM_CH13_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch13_evt_id
 */
#define SOC_ETM_CH13_EVT_ID    0x000000FFU
#define SOC_ETM_CH13_EVT_ID_M  (SOC_ETM_CH13_EVT_ID_V << SOC_ETM_CH13_EVT_ID_S)
#define SOC_ETM_CH13_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH13_EVT_ID_S  0

/** SOC_ETM_CH13_TASK_ID_REG register
 *  Channel13 task id register
 */
#define SOC_ETM_CH13_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x84)
/** SOC_ETM_CH13_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch13_task_id
 */
#define SOC_ETM_CH13_TASK_ID    0x000000FFU
#define SOC_ETM_CH13_TASK_ID_M  (SOC_ETM_CH13_TASK_ID_V << SOC_ETM_CH13_TASK_ID_S)
#define SOC_ETM_CH13_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH13_TASK_ID_S  0

/** SOC_ETM_CH14_EVT_ID_REG register
 *  Channel14 event id register
 */
#define SOC_ETM_CH14_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x88)
/** SOC_ETM_CH14_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch14_evt_id
 */
#define SOC_ETM_CH14_EVT_ID    0x000000FFU
#define SOC_ETM_CH14_EVT_ID_M  (SOC_ETM_CH14_EVT_ID_V << SOC_ETM_CH14_EVT_ID_S)
#define SOC_ETM_CH14_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH14_EVT_ID_S  0

/** SOC_ETM_CH14_TASK_ID_REG register
 *  Channel14 task id register
 */
#define SOC_ETM_CH14_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x8c)
/** SOC_ETM_CH14_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch14_task_id
 */
#define SOC_ETM_CH14_TASK_ID    0x000000FFU
#define SOC_ETM_CH14_TASK_ID_M  (SOC_ETM_CH14_TASK_ID_V << SOC_ETM_CH14_TASK_ID_S)
#define SOC_ETM_CH14_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH14_TASK_ID_S  0

/** SOC_ETM_CH15_EVT_ID_REG register
 *  Channel15 event id register
 */
#define SOC_ETM_CH15_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x90)
/** SOC_ETM_CH15_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch15_evt_id
 */
#define SOC_ETM_CH15_EVT_ID    0x000000FFU
#define SOC_ETM_CH15_EVT_ID_M  (SOC_ETM_CH15_EVT_ID_V << SOC_ETM_CH15_EVT_ID_S)
#define SOC_ETM_CH15_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH15_EVT_ID_S  0

/** SOC_ETM_CH15_TASK_ID_REG register
 *  Channel15 task id register
 */
#define SOC_ETM_CH15_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x94)
/** SOC_ETM_CH15_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch15_task_id
 */
#define SOC_ETM_CH15_TASK_ID    0x000000FFU
#define SOC_ETM_CH15_TASK_ID_M  (SOC_ETM_CH15_TASK_ID_V << SOC_ETM_CH15_TASK_ID_S)
#define SOC_ETM_CH15_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH15_TASK_ID_S  0

/** SOC_ETM_CH16_EVT_ID_REG register
 *  Channel16 event id register
 */
#define SOC_ETM_CH16_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x98)
/** SOC_ETM_CH16_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch16_evt_id
 */
#define SOC_ETM_CH16_EVT_ID    0x000000FFU
#define SOC_ETM_CH16_EVT_ID_M  (SOC_ETM_CH16_EVT_ID_V << SOC_ETM_CH16_EVT_ID_S)
#define SOC_ETM_CH16_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH16_EVT_ID_S  0

/** SOC_ETM_CH16_TASK_ID_REG register
 *  Channel16 task id register
 */
#define SOC_ETM_CH16_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x9c)
/** SOC_ETM_CH16_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch16_task_id
 */
#define SOC_ETM_CH16_TASK_ID    0x000000FFU
#define SOC_ETM_CH16_TASK_ID_M  (SOC_ETM_CH16_TASK_ID_V << SOC_ETM_CH16_TASK_ID_S)
#define SOC_ETM_CH16_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH16_TASK_ID_S  0

/** SOC_ETM_CH17_EVT_ID_REG register
 *  Channel17 event id register
 */
#define SOC_ETM_CH17_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xa0)
/** SOC_ETM_CH17_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch17_evt_id
 */
#define SOC_ETM_CH17_EVT_ID    0x000000FFU
#define SOC_ETM_CH17_EVT_ID_M  (SOC_ETM_CH17_EVT_ID_V << SOC_ETM_CH17_EVT_ID_S)
#define SOC_ETM_CH17_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH17_EVT_ID_S  0

/** SOC_ETM_CH17_TASK_ID_REG register
 *  Channel17 task id register
 */
#define SOC_ETM_CH17_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xa4)
/** SOC_ETM_CH17_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch17_task_id
 */
#define SOC_ETM_CH17_TASK_ID    0x000000FFU
#define SOC_ETM_CH17_TASK_ID_M  (SOC_ETM_CH17_TASK_ID_V << SOC_ETM_CH17_TASK_ID_S)
#define SOC_ETM_CH17_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH17_TASK_ID_S  0

/** SOC_ETM_CH18_EVT_ID_REG register
 *  Channel18 event id register
 */
#define SOC_ETM_CH18_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xa8)
/** SOC_ETM_CH18_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch18_evt_id
 */
#define SOC_ETM_CH18_EVT_ID    0x000000FFU
#define SOC_ETM_CH18_EVT_ID_M  (SOC_ETM_CH18_EVT_ID_V << SOC_ETM_CH18_EVT_ID_S)
#define SOC_ETM_CH18_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH18_EVT_ID_S  0

/** SOC_ETM_CH18_TASK_ID_REG register
 *  Channel18 task id register
 */
#define SOC_ETM_CH18_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xac)
/** SOC_ETM_CH18_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch18_task_id
 */
#define SOC_ETM_CH18_TASK_ID    0x000000FFU
#define SOC_ETM_CH18_TASK_ID_M  (SOC_ETM_CH18_TASK_ID_V << SOC_ETM_CH18_TASK_ID_S)
#define SOC_ETM_CH18_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH18_TASK_ID_S  0

/** SOC_ETM_CH19_EVT_ID_REG register
 *  Channel19 event id register
 */
#define SOC_ETM_CH19_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xb0)
/** SOC_ETM_CH19_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch19_evt_id
 */
#define SOC_ETM_CH19_EVT_ID    0x000000FFU
#define SOC_ETM_CH19_EVT_ID_M  (SOC_ETM_CH19_EVT_ID_V << SOC_ETM_CH19_EVT_ID_S)
#define SOC_ETM_CH19_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH19_EVT_ID_S  0

/** SOC_ETM_CH19_TASK_ID_REG register
 *  Channel19 task id register
 */
#define SOC_ETM_CH19_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xb4)
/** SOC_ETM_CH19_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch19_task_id
 */
#define SOC_ETM_CH19_TASK_ID    0x000000FFU
#define SOC_ETM_CH19_TASK_ID_M  (SOC_ETM_CH19_TASK_ID_V << SOC_ETM_CH19_TASK_ID_S)
#define SOC_ETM_CH19_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH19_TASK_ID_S  0

/** SOC_ETM_CH20_EVT_ID_REG register
 *  Channel20 event id register
 */
#define SOC_ETM_CH20_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xb8)
/** SOC_ETM_CH20_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch20_evt_id
 */
#define SOC_ETM_CH20_EVT_ID    0x000000FFU
#define SOC_ETM_CH20_EVT_ID_M  (SOC_ETM_CH20_EVT_ID_V << SOC_ETM_CH20_EVT_ID_S)
#define SOC_ETM_CH20_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH20_EVT_ID_S  0

/** SOC_ETM_CH20_TASK_ID_REG register
 *  Channel20 task id register
 */
#define SOC_ETM_CH20_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xbc)
/** SOC_ETM_CH20_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch20_task_id
 */
#define SOC_ETM_CH20_TASK_ID    0x000000FFU
#define SOC_ETM_CH20_TASK_ID_M  (SOC_ETM_CH20_TASK_ID_V << SOC_ETM_CH20_TASK_ID_S)
#define SOC_ETM_CH20_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH20_TASK_ID_S  0

/** SOC_ETM_CH21_EVT_ID_REG register
 *  Channel21 event id register
 */
#define SOC_ETM_CH21_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xc0)
/** SOC_ETM_CH21_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch21_evt_id
 */
#define SOC_ETM_CH21_EVT_ID    0x000000FFU
#define SOC_ETM_CH21_EVT_ID_M  (SOC_ETM_CH21_EVT_ID_V << SOC_ETM_CH21_EVT_ID_S)
#define SOC_ETM_CH21_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH21_EVT_ID_S  0

/** SOC_ETM_CH21_TASK_ID_REG register
 *  Channel21 task id register
 */
#define SOC_ETM_CH21_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xc4)
/** SOC_ETM_CH21_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch21_task_id
 */
#define SOC_ETM_CH21_TASK_ID    0x000000FFU
#define SOC_ETM_CH21_TASK_ID_M  (SOC_ETM_CH21_TASK_ID_V << SOC_ETM_CH21_TASK_ID_S)
#define SOC_ETM_CH21_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH21_TASK_ID_S  0

/** SOC_ETM_CH22_EVT_ID_REG register
 *  Channel22 event id register
 */
#define SOC_ETM_CH22_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xc8)
/** SOC_ETM_CH22_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch22_evt_id
 */
#define SOC_ETM_CH22_EVT_ID    0x000000FFU
#define SOC_ETM_CH22_EVT_ID_M  (SOC_ETM_CH22_EVT_ID_V << SOC_ETM_CH22_EVT_ID_S)
#define SOC_ETM_CH22_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH22_EVT_ID_S  0

/** SOC_ETM_CH22_TASK_ID_REG register
 *  Channel22 task id register
 */
#define SOC_ETM_CH22_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xcc)
/** SOC_ETM_CH22_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch22_task_id
 */
#define SOC_ETM_CH22_TASK_ID    0x000000FFU
#define SOC_ETM_CH22_TASK_ID_M  (SOC_ETM_CH22_TASK_ID_V << SOC_ETM_CH22_TASK_ID_S)
#define SOC_ETM_CH22_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH22_TASK_ID_S  0

/** SOC_ETM_CH23_EVT_ID_REG register
 *  Channel23 event id register
 */
#define SOC_ETM_CH23_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xd0)
/** SOC_ETM_CH23_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch23_evt_id
 */
#define SOC_ETM_CH23_EVT_ID    0x000000FFU
#define SOC_ETM_CH23_EVT_ID_M  (SOC_ETM_CH23_EVT_ID_V << SOC_ETM_CH23_EVT_ID_S)
#define SOC_ETM_CH23_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH23_EVT_ID_S  0

/** SOC_ETM_CH23_TASK_ID_REG register
 *  Channel23 task id register
 */
#define SOC_ETM_CH23_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xd4)
/** SOC_ETM_CH23_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch23_task_id
 */
#define SOC_ETM_CH23_TASK_ID    0x000000FFU
#define SOC_ETM_CH23_TASK_ID_M  (SOC_ETM_CH23_TASK_ID_V << SOC_ETM_CH23_TASK_ID_S)
#define SOC_ETM_CH23_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH23_TASK_ID_S  0

/** SOC_ETM_CH24_EVT_ID_REG register
 *  Channel24 event id register
 */
#define SOC_ETM_CH24_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xd8)
/** SOC_ETM_CH24_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch24_evt_id
 */
#define SOC_ETM_CH24_EVT_ID    0x000000FFU
#define SOC_ETM_CH24_EVT_ID_M  (SOC_ETM_CH24_EVT_ID_V << SOC_ETM_CH24_EVT_ID_S)
#define SOC_ETM_CH24_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH24_EVT_ID_S  0

/** SOC_ETM_CH24_TASK_ID_REG register
 *  Channel24 task id register
 */
#define SOC_ETM_CH24_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xdc)
/** SOC_ETM_CH24_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch24_task_id
 */
#define SOC_ETM_CH24_TASK_ID    0x000000FFU
#define SOC_ETM_CH24_TASK_ID_M  (SOC_ETM_CH24_TASK_ID_V << SOC_ETM_CH24_TASK_ID_S)
#define SOC_ETM_CH24_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH24_TASK_ID_S  0

/** SOC_ETM_CH25_EVT_ID_REG register
 *  Channel25 event id register
 */
#define SOC_ETM_CH25_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xe0)
/** SOC_ETM_CH25_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch25_evt_id
 */
#define SOC_ETM_CH25_EVT_ID    0x000000FFU
#define SOC_ETM_CH25_EVT_ID_M  (SOC_ETM_CH25_EVT_ID_V << SOC_ETM_CH25_EVT_ID_S)
#define SOC_ETM_CH25_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH25_EVT_ID_S  0

/** SOC_ETM_CH25_TASK_ID_REG register
 *  Channel25 task id register
 */
#define SOC_ETM_CH25_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xe4)
/** SOC_ETM_CH25_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch25_task_id
 */
#define SOC_ETM_CH25_TASK_ID    0x000000FFU
#define SOC_ETM_CH25_TASK_ID_M  (SOC_ETM_CH25_TASK_ID_V << SOC_ETM_CH25_TASK_ID_S)
#define SOC_ETM_CH25_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH25_TASK_ID_S  0

/** SOC_ETM_CH26_EVT_ID_REG register
 *  Channel26 event id register
 */
#define SOC_ETM_CH26_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xe8)
/** SOC_ETM_CH26_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch26_evt_id
 */
#define SOC_ETM_CH26_EVT_ID    0x000000FFU
#define SOC_ETM_CH26_EVT_ID_M  (SOC_ETM_CH26_EVT_ID_V << SOC_ETM_CH26_EVT_ID_S)
#define SOC_ETM_CH26_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH26_EVT_ID_S  0

/** SOC_ETM_CH26_TASK_ID_REG register
 *  Channel26 task id register
 */
#define SOC_ETM_CH26_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xec)
/** SOC_ETM_CH26_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch26_task_id
 */
#define SOC_ETM_CH26_TASK_ID    0x000000FFU
#define SOC_ETM_CH26_TASK_ID_M  (SOC_ETM_CH26_TASK_ID_V << SOC_ETM_CH26_TASK_ID_S)
#define SOC_ETM_CH26_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH26_TASK_ID_S  0

/** SOC_ETM_CH27_EVT_ID_REG register
 *  Channel27 event id register
 */
#define SOC_ETM_CH27_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xf0)
/** SOC_ETM_CH27_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch27_evt_id
 */
#define SOC_ETM_CH27_EVT_ID    0x000000FFU
#define SOC_ETM_CH27_EVT_ID_M  (SOC_ETM_CH27_EVT_ID_V << SOC_ETM_CH27_EVT_ID_S)
#define SOC_ETM_CH27_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH27_EVT_ID_S  0

/** SOC_ETM_CH27_TASK_ID_REG register
 *  Channel27 task id register
 */
#define SOC_ETM_CH27_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xf4)
/** SOC_ETM_CH27_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch27_task_id
 */
#define SOC_ETM_CH27_TASK_ID    0x000000FFU
#define SOC_ETM_CH27_TASK_ID_M  (SOC_ETM_CH27_TASK_ID_V << SOC_ETM_CH27_TASK_ID_S)
#define SOC_ETM_CH27_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH27_TASK_ID_S  0

/** SOC_ETM_CH28_EVT_ID_REG register
 *  Channel28 event id register
 */
#define SOC_ETM_CH28_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xf8)
/** SOC_ETM_CH28_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch28_evt_id
 */
#define SOC_ETM_CH28_EVT_ID    0x000000FFU
#define SOC_ETM_CH28_EVT_ID_M  (SOC_ETM_CH28_EVT_ID_V << SOC_ETM_CH28_EVT_ID_S)
#define SOC_ETM_CH28_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH28_EVT_ID_S  0

/** SOC_ETM_CH28_TASK_ID_REG register
 *  Channel28 task id register
 */
#define SOC_ETM_CH28_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xfc)
/** SOC_ETM_CH28_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch28_task_id
 */
#define SOC_ETM_CH28_TASK_ID    0x000000FFU
#define SOC_ETM_CH28_TASK_ID_M  (SOC_ETM_CH28_TASK_ID_V << SOC_ETM_CH28_TASK_ID_S)
#define SOC_ETM_CH28_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH28_TASK_ID_S  0

/** SOC_ETM_CH29_EVT_ID_REG register
 *  Channel29 event id register
 */
#define SOC_ETM_CH29_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x100)
/** SOC_ETM_CH29_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch29_evt_id
 */
#define SOC_ETM_CH29_EVT_ID    0x000000FFU
#define SOC_ETM_CH29_EVT_ID_M  (SOC_ETM_CH29_EVT_ID_V << SOC_ETM_CH29_EVT_ID_S)
#define SOC_ETM_CH29_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH29_EVT_ID_S  0

/** SOC_ETM_CH29_TASK_ID_REG register
 *  Channel29 task id register
 */
#define SOC_ETM_CH29_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x104)
/** SOC_ETM_CH29_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch29_task_id
 */
#define SOC_ETM_CH29_TASK_ID    0x000000FFU
#define SOC_ETM_CH29_TASK_ID_M  (SOC_ETM_CH29_TASK_ID_V << SOC_ETM_CH29_TASK_ID_S)
#define SOC_ETM_CH29_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH29_TASK_ID_S  0

/** SOC_ETM_CH30_EVT_ID_REG register
 *  Channel30 event id register
 */
#define SOC_ETM_CH30_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x108)
/** SOC_ETM_CH30_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch30_evt_id
 */
#define SOC_ETM_CH30_EVT_ID    0x000000FFU
#define SOC_ETM_CH30_EVT_ID_M  (SOC_ETM_CH30_EVT_ID_V << SOC_ETM_CH30_EVT_ID_S)
#define SOC_ETM_CH30_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH30_EVT_ID_S  0

/** SOC_ETM_CH30_TASK_ID_REG register
 *  Channel30 task id register
 */
#define SOC_ETM_CH30_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x10c)
/** SOC_ETM_CH30_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch30_task_id
 */
#define SOC_ETM_CH30_TASK_ID    0x000000FFU
#define SOC_ETM_CH30_TASK_ID_M  (SOC_ETM_CH30_TASK_ID_V << SOC_ETM_CH30_TASK_ID_S)
#define SOC_ETM_CH30_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH30_TASK_ID_S  0

/** SOC_ETM_CH31_EVT_ID_REG register
 *  Channel31 event id register
 */
#define SOC_ETM_CH31_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x110)
/** SOC_ETM_CH31_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch31_evt_id
 */
#define SOC_ETM_CH31_EVT_ID    0x000000FFU
#define SOC_ETM_CH31_EVT_ID_M  (SOC_ETM_CH31_EVT_ID_V << SOC_ETM_CH31_EVT_ID_S)
#define SOC_ETM_CH31_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH31_EVT_ID_S  0

/** SOC_ETM_CH31_TASK_ID_REG register
 *  Channel31 task id register
 */
#define SOC_ETM_CH31_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x114)
/** SOC_ETM_CH31_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch31_task_id
 */
#define SOC_ETM_CH31_TASK_ID    0x000000FFU
#define SOC_ETM_CH31_TASK_ID_M  (SOC_ETM_CH31_TASK_ID_V << SOC_ETM_CH31_TASK_ID_S)
#define SOC_ETM_CH31_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH31_TASK_ID_S  0

/** SOC_ETM_CH32_EVT_ID_REG register
 *  Channel32 event id register
 */
#define SOC_ETM_CH32_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x118)
/** SOC_ETM_CH32_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch32_evt_id
 */
#define SOC_ETM_CH32_EVT_ID    0x000000FFU
#define SOC_ETM_CH32_EVT_ID_M  (SOC_ETM_CH32_EVT_ID_V << SOC_ETM_CH32_EVT_ID_S)
#define SOC_ETM_CH32_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH32_EVT_ID_S  0

/** SOC_ETM_CH32_TASK_ID_REG register
 *  Channel32 task id register
 */
#define SOC_ETM_CH32_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x11c)
/** SOC_ETM_CH32_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch32_task_id
 */
#define SOC_ETM_CH32_TASK_ID    0x000000FFU
#define SOC_ETM_CH32_TASK_ID_M  (SOC_ETM_CH32_TASK_ID_V << SOC_ETM_CH32_TASK_ID_S)
#define SOC_ETM_CH32_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH32_TASK_ID_S  0

/** SOC_ETM_CH33_EVT_ID_REG register
 *  Channel33 event id register
 */
#define SOC_ETM_CH33_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x120)
/** SOC_ETM_CH33_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch33_evt_id
 */
#define SOC_ETM_CH33_EVT_ID    0x000000FFU
#define SOC_ETM_CH33_EVT_ID_M  (SOC_ETM_CH33_EVT_ID_V << SOC_ETM_CH33_EVT_ID_S)
#define SOC_ETM_CH33_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH33_EVT_ID_S  0

/** SOC_ETM_CH33_TASK_ID_REG register
 *  Channel33 task id register
 */
#define SOC_ETM_CH33_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x124)
/** SOC_ETM_CH33_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch33_task_id
 */
#define SOC_ETM_CH33_TASK_ID    0x000000FFU
#define SOC_ETM_CH33_TASK_ID_M  (SOC_ETM_CH33_TASK_ID_V << SOC_ETM_CH33_TASK_ID_S)
#define SOC_ETM_CH33_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH33_TASK_ID_S  0

/** SOC_ETM_CH34_EVT_ID_REG register
 *  Channel34 event id register
 */
#define SOC_ETM_CH34_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x128)
/** SOC_ETM_CH34_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch34_evt_id
 */
#define SOC_ETM_CH34_EVT_ID    0x000000FFU
#define SOC_ETM_CH34_EVT_ID_M  (SOC_ETM_CH34_EVT_ID_V << SOC_ETM_CH34_EVT_ID_S)
#define SOC_ETM_CH34_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH34_EVT_ID_S  0

/** SOC_ETM_CH34_TASK_ID_REG register
 *  Channel34 task id register
 */
#define SOC_ETM_CH34_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x12c)
/** SOC_ETM_CH34_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch34_task_id
 */
#define SOC_ETM_CH34_TASK_ID    0x000000FFU
#define SOC_ETM_CH34_TASK_ID_M  (SOC_ETM_CH34_TASK_ID_V << SOC_ETM_CH34_TASK_ID_S)
#define SOC_ETM_CH34_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH34_TASK_ID_S  0

/** SOC_ETM_CH35_EVT_ID_REG register
 *  Channel35 event id register
 */
#define SOC_ETM_CH35_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x130)
/** SOC_ETM_CH35_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch35_evt_id
 */
#define SOC_ETM_CH35_EVT_ID    0x000000FFU
#define SOC_ETM_CH35_EVT_ID_M  (SOC_ETM_CH35_EVT_ID_V << SOC_ETM_CH35_EVT_ID_S)
#define SOC_ETM_CH35_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH35_EVT_ID_S  0

/** SOC_ETM_CH35_TASK_ID_REG register
 *  Channel35 task id register
 */
#define SOC_ETM_CH35_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x134)
/** SOC_ETM_CH35_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch35_task_id
 */
#define SOC_ETM_CH35_TASK_ID    0x000000FFU
#define SOC_ETM_CH35_TASK_ID_M  (SOC_ETM_CH35_TASK_ID_V << SOC_ETM_CH35_TASK_ID_S)
#define SOC_ETM_CH35_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH35_TASK_ID_S  0

/** SOC_ETM_CH36_EVT_ID_REG register
 *  Channel36 event id register
 */
#define SOC_ETM_CH36_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x138)
/** SOC_ETM_CH36_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch36_evt_id
 */
#define SOC_ETM_CH36_EVT_ID    0x000000FFU
#define SOC_ETM_CH36_EVT_ID_M  (SOC_ETM_CH36_EVT_ID_V << SOC_ETM_CH36_EVT_ID_S)
#define SOC_ETM_CH36_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH36_EVT_ID_S  0

/** SOC_ETM_CH36_TASK_ID_REG register
 *  Channel36 task id register
 */
#define SOC_ETM_CH36_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x13c)
/** SOC_ETM_CH36_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch36_task_id
 */
#define SOC_ETM_CH36_TASK_ID    0x000000FFU
#define SOC_ETM_CH36_TASK_ID_M  (SOC_ETM_CH36_TASK_ID_V << SOC_ETM_CH36_TASK_ID_S)
#define SOC_ETM_CH36_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH36_TASK_ID_S  0

/** SOC_ETM_CH37_EVT_ID_REG register
 *  Channel37 event id register
 */
#define SOC_ETM_CH37_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x140)
/** SOC_ETM_CH37_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch37_evt_id
 */
#define SOC_ETM_CH37_EVT_ID    0x000000FFU
#define SOC_ETM_CH37_EVT_ID_M  (SOC_ETM_CH37_EVT_ID_V << SOC_ETM_CH37_EVT_ID_S)
#define SOC_ETM_CH37_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH37_EVT_ID_S  0

/** SOC_ETM_CH37_TASK_ID_REG register
 *  Channel37 task id register
 */
#define SOC_ETM_CH37_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x144)
/** SOC_ETM_CH37_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch37_task_id
 */
#define SOC_ETM_CH37_TASK_ID    0x000000FFU
#define SOC_ETM_CH37_TASK_ID_M  (SOC_ETM_CH37_TASK_ID_V << SOC_ETM_CH37_TASK_ID_S)
#define SOC_ETM_CH37_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH37_TASK_ID_S  0

/** SOC_ETM_CH38_EVT_ID_REG register
 *  Channel38 event id register
 */
#define SOC_ETM_CH38_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x148)
/** SOC_ETM_CH38_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch38_evt_id
 */
#define SOC_ETM_CH38_EVT_ID    0x000000FFU
#define SOC_ETM_CH38_EVT_ID_M  (SOC_ETM_CH38_EVT_ID_V << SOC_ETM_CH38_EVT_ID_S)
#define SOC_ETM_CH38_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH38_EVT_ID_S  0

/** SOC_ETM_CH38_TASK_ID_REG register
 *  Channel38 task id register
 */
#define SOC_ETM_CH38_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x14c)
/** SOC_ETM_CH38_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch38_task_id
 */
#define SOC_ETM_CH38_TASK_ID    0x000000FFU
#define SOC_ETM_CH38_TASK_ID_M  (SOC_ETM_CH38_TASK_ID_V << SOC_ETM_CH38_TASK_ID_S)
#define SOC_ETM_CH38_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH38_TASK_ID_S  0

/** SOC_ETM_CH39_EVT_ID_REG register
 *  Channel39 event id register
 */
#define SOC_ETM_CH39_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x150)
/** SOC_ETM_CH39_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch39_evt_id
 */
#define SOC_ETM_CH39_EVT_ID    0x000000FFU
#define SOC_ETM_CH39_EVT_ID_M  (SOC_ETM_CH39_EVT_ID_V << SOC_ETM_CH39_EVT_ID_S)
#define SOC_ETM_CH39_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH39_EVT_ID_S  0

/** SOC_ETM_CH39_TASK_ID_REG register
 *  Channel39 task id register
 */
#define SOC_ETM_CH39_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x154)
/** SOC_ETM_CH39_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch39_task_id
 */
#define SOC_ETM_CH39_TASK_ID    0x000000FFU
#define SOC_ETM_CH39_TASK_ID_M  (SOC_ETM_CH39_TASK_ID_V << SOC_ETM_CH39_TASK_ID_S)
#define SOC_ETM_CH39_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH39_TASK_ID_S  0

/** SOC_ETM_CH40_EVT_ID_REG register
 *  Channel40 event id register
 */
#define SOC_ETM_CH40_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x158)
/** SOC_ETM_CH40_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch40_evt_id
 */
#define SOC_ETM_CH40_EVT_ID    0x000000FFU
#define SOC_ETM_CH40_EVT_ID_M  (SOC_ETM_CH40_EVT_ID_V << SOC_ETM_CH40_EVT_ID_S)
#define SOC_ETM_CH40_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH40_EVT_ID_S  0

/** SOC_ETM_CH40_TASK_ID_REG register
 *  Channel40 task id register
 */
#define SOC_ETM_CH40_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x15c)
/** SOC_ETM_CH40_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch40_task_id
 */
#define SOC_ETM_CH40_TASK_ID    0x000000FFU
#define SOC_ETM_CH40_TASK_ID_M  (SOC_ETM_CH40_TASK_ID_V << SOC_ETM_CH40_TASK_ID_S)
#define SOC_ETM_CH40_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH40_TASK_ID_S  0

/** SOC_ETM_CH41_EVT_ID_REG register
 *  Channel41 event id register
 */
#define SOC_ETM_CH41_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x160)
/** SOC_ETM_CH41_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch41_evt_id
 */
#define SOC_ETM_CH41_EVT_ID    0x000000FFU
#define SOC_ETM_CH41_EVT_ID_M  (SOC_ETM_CH41_EVT_ID_V << SOC_ETM_CH41_EVT_ID_S)
#define SOC_ETM_CH41_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH41_EVT_ID_S  0

/** SOC_ETM_CH41_TASK_ID_REG register
 *  Channel41 task id register
 */
#define SOC_ETM_CH41_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x164)
/** SOC_ETM_CH41_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch41_task_id
 */
#define SOC_ETM_CH41_TASK_ID    0x000000FFU
#define SOC_ETM_CH41_TASK_ID_M  (SOC_ETM_CH41_TASK_ID_V << SOC_ETM_CH41_TASK_ID_S)
#define SOC_ETM_CH41_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH41_TASK_ID_S  0

/** SOC_ETM_CH42_EVT_ID_REG register
 *  Channel42 event id register
 */
#define SOC_ETM_CH42_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x168)
/** SOC_ETM_CH42_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch42_evt_id
 */
#define SOC_ETM_CH42_EVT_ID    0x000000FFU
#define SOC_ETM_CH42_EVT_ID_M  (SOC_ETM_CH42_EVT_ID_V << SOC_ETM_CH42_EVT_ID_S)
#define SOC_ETM_CH42_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH42_EVT_ID_S  0

/** SOC_ETM_CH42_TASK_ID_REG register
 *  Channel42 task id register
 */
#define SOC_ETM_CH42_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x16c)
/** SOC_ETM_CH42_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch42_task_id
 */
#define SOC_ETM_CH42_TASK_ID    0x000000FFU
#define SOC_ETM_CH42_TASK_ID_M  (SOC_ETM_CH42_TASK_ID_V << SOC_ETM_CH42_TASK_ID_S)
#define SOC_ETM_CH42_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH42_TASK_ID_S  0

/** SOC_ETM_CH43_EVT_ID_REG register
 *  Channel43 event id register
 */
#define SOC_ETM_CH43_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x170)
/** SOC_ETM_CH43_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch43_evt_id
 */
#define SOC_ETM_CH43_EVT_ID    0x000000FFU
#define SOC_ETM_CH43_EVT_ID_M  (SOC_ETM_CH43_EVT_ID_V << SOC_ETM_CH43_EVT_ID_S)
#define SOC_ETM_CH43_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH43_EVT_ID_S  0

/** SOC_ETM_CH43_TASK_ID_REG register
 *  Channel43 task id register
 */
#define SOC_ETM_CH43_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x174)
/** SOC_ETM_CH43_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch43_task_id
 */
#define SOC_ETM_CH43_TASK_ID    0x000000FFU
#define SOC_ETM_CH43_TASK_ID_M  (SOC_ETM_CH43_TASK_ID_V << SOC_ETM_CH43_TASK_ID_S)
#define SOC_ETM_CH43_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH43_TASK_ID_S  0

/** SOC_ETM_CH44_EVT_ID_REG register
 *  Channel44 event id register
 */
#define SOC_ETM_CH44_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x178)
/** SOC_ETM_CH44_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch44_evt_id
 */
#define SOC_ETM_CH44_EVT_ID    0x000000FFU
#define SOC_ETM_CH44_EVT_ID_M  (SOC_ETM_CH44_EVT_ID_V << SOC_ETM_CH44_EVT_ID_S)
#define SOC_ETM_CH44_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH44_EVT_ID_S  0

/** SOC_ETM_CH44_TASK_ID_REG register
 *  Channel44 task id register
 */
#define SOC_ETM_CH44_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x17c)
/** SOC_ETM_CH44_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch44_task_id
 */
#define SOC_ETM_CH44_TASK_ID    0x000000FFU
#define SOC_ETM_CH44_TASK_ID_M  (SOC_ETM_CH44_TASK_ID_V << SOC_ETM_CH44_TASK_ID_S)
#define SOC_ETM_CH44_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH44_TASK_ID_S  0

/** SOC_ETM_CH45_EVT_ID_REG register
 *  Channel45 event id register
 */
#define SOC_ETM_CH45_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x180)
/** SOC_ETM_CH45_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch45_evt_id
 */
#define SOC_ETM_CH45_EVT_ID    0x000000FFU
#define SOC_ETM_CH45_EVT_ID_M  (SOC_ETM_CH45_EVT_ID_V << SOC_ETM_CH45_EVT_ID_S)
#define SOC_ETM_CH45_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH45_EVT_ID_S  0

/** SOC_ETM_CH45_TASK_ID_REG register
 *  Channel45 task id register
 */
#define SOC_ETM_CH45_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x184)
/** SOC_ETM_CH45_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch45_task_id
 */
#define SOC_ETM_CH45_TASK_ID    0x000000FFU
#define SOC_ETM_CH45_TASK_ID_M  (SOC_ETM_CH45_TASK_ID_V << SOC_ETM_CH45_TASK_ID_S)
#define SOC_ETM_CH45_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH45_TASK_ID_S  0

/** SOC_ETM_CH46_EVT_ID_REG register
 *  Channel46 event id register
 */
#define SOC_ETM_CH46_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x188)
/** SOC_ETM_CH46_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch46_evt_id
 */
#define SOC_ETM_CH46_EVT_ID    0x000000FFU
#define SOC_ETM_CH46_EVT_ID_M  (SOC_ETM_CH46_EVT_ID_V << SOC_ETM_CH46_EVT_ID_S)
#define SOC_ETM_CH46_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH46_EVT_ID_S  0

/** SOC_ETM_CH46_TASK_ID_REG register
 *  Channel46 task id register
 */
#define SOC_ETM_CH46_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x18c)
/** SOC_ETM_CH46_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch46_task_id
 */
#define SOC_ETM_CH46_TASK_ID    0x000000FFU
#define SOC_ETM_CH46_TASK_ID_M  (SOC_ETM_CH46_TASK_ID_V << SOC_ETM_CH46_TASK_ID_S)
#define SOC_ETM_CH46_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH46_TASK_ID_S  0

/** SOC_ETM_CH47_EVT_ID_REG register
 *  Channel47 event id register
 */
#define SOC_ETM_CH47_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x190)
/** SOC_ETM_CH47_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch47_evt_id
 */
#define SOC_ETM_CH47_EVT_ID    0x000000FFU
#define SOC_ETM_CH47_EVT_ID_M  (SOC_ETM_CH47_EVT_ID_V << SOC_ETM_CH47_EVT_ID_S)
#define SOC_ETM_CH47_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH47_EVT_ID_S  0

/** SOC_ETM_CH47_TASK_ID_REG register
 *  Channel47 task id register
 */
#define SOC_ETM_CH47_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x194)
/** SOC_ETM_CH47_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch47_task_id
 */
#define SOC_ETM_CH47_TASK_ID    0x000000FFU
#define SOC_ETM_CH47_TASK_ID_M  (SOC_ETM_CH47_TASK_ID_V << SOC_ETM_CH47_TASK_ID_S)
#define SOC_ETM_CH47_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH47_TASK_ID_S  0

/** SOC_ETM_CH48_EVT_ID_REG register
 *  Channel48 event id register
 */
#define SOC_ETM_CH48_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x198)
/** SOC_ETM_CH48_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch48_evt_id
 */
#define SOC_ETM_CH48_EVT_ID    0x000000FFU
#define SOC_ETM_CH48_EVT_ID_M  (SOC_ETM_CH48_EVT_ID_V << SOC_ETM_CH48_EVT_ID_S)
#define SOC_ETM_CH48_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH48_EVT_ID_S  0

/** SOC_ETM_CH48_TASK_ID_REG register
 *  Channel48 task id register
 */
#define SOC_ETM_CH48_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x19c)
/** SOC_ETM_CH48_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch48_task_id
 */
#define SOC_ETM_CH48_TASK_ID    0x000000FFU
#define SOC_ETM_CH48_TASK_ID_M  (SOC_ETM_CH48_TASK_ID_V << SOC_ETM_CH48_TASK_ID_S)
#define SOC_ETM_CH48_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH48_TASK_ID_S  0

/** SOC_ETM_CH49_EVT_ID_REG register
 *  Channel49 event id register
 */
#define SOC_ETM_CH49_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x1a0)
/** SOC_ETM_CH49_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch49_evt_id
 */
#define SOC_ETM_CH49_EVT_ID    0x000000FFU
#define SOC_ETM_CH49_EVT_ID_M  (SOC_ETM_CH49_EVT_ID_V << SOC_ETM_CH49_EVT_ID_S)
#define SOC_ETM_CH49_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH49_EVT_ID_S  0

/** SOC_ETM_CH49_TASK_ID_REG register
 *  Channel49 task id register
 */
#define SOC_ETM_CH49_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x1a4)
/** SOC_ETM_CH49_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  Configures ch49_task_id
 */
#define SOC_ETM_CH49_TASK_ID    0x000000FFU
#define SOC_ETM_CH49_TASK_ID_M  (SOC_ETM_CH49_TASK_ID_V << SOC_ETM_CH49_TASK_ID_S)
#define SOC_ETM_CH49_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH49_TASK_ID_S  0

/** SOC_ETM_EVT_ST0_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST0_REG (DR_REG_SOC_ETM_BASE + 0x1a8)
/** SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents GPIO_evt_ch0_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST    (BIT(0))
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_S  0
/** SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents GPIO_evt_ch1_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST    (BIT(1))
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_S  1
/** SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents GPIO_evt_ch2_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST    (BIT(2))
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_S  2
/** SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents GPIO_evt_ch3_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST    (BIT(3))
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_S  3
/** SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents GPIO_evt_ch4_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST    (BIT(4))
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_S  4
/** SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents GPIO_evt_ch5_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST    (BIT(5))
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_S  5
/** SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents GPIO_evt_ch6_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST    (BIT(6))
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_S  6
/** SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents GPIO_evt_ch7_rise_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST    (BIT(7))
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_S  7
/** SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents GPIO_evt_ch0_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST    (BIT(8))
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_S  8
/** SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents GPIO_evt_ch1_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST    (BIT(9))
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_S  9
/** SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents GPIO_evt_ch2_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST    (BIT(10))
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_S  10
/** SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents GPIO_evt_ch3_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST    (BIT(11))
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_S  11
/** SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents GPIO_evt_ch4_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST    (BIT(12))
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_S  12
/** SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents GPIO_evt_ch5_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST    (BIT(13))
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_S  13
/** SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents GPIO_evt_ch6_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST    (BIT(14))
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_S  14
/** SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents GPIO_evt_ch7_fall_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST    (BIT(15))
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_S  15
/** SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents GPIO_evt_ch0_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST    (BIT(16))
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_S  16
/** SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents GPIO_evt_ch1_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST    (BIT(17))
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_S  17
/** SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents GPIO_evt_ch2_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST    (BIT(18))
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_S  18
/** SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents GPIO_evt_ch3_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST    (BIT(19))
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_S  19
/** SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents GPIO_evt_ch4_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST    (BIT(20))
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_S  20
/** SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents GPIO_evt_ch5_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST    (BIT(21))
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_S  21
/** SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents GPIO_evt_ch6_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST    (BIT(22))
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_S  22
/** SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents GPIO_evt_ch7_any_edge trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST    (BIT(23))
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_M  (SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_V << SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_S)
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_S  23
/** SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents GPIO_evt_zero_det_pos0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST    (BIT(24))
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_M  (SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_V << SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_S  24
/** SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents GPIO_evt_zero_det_neg0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST    (BIT(25))
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_M  (SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_V << SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_S  25
/** SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents GPIO_evt_zero_det_pos1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST    (BIT(26))
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_M  (SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_V << SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_S  26
/** SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents GPIO_evt_zero_det_neg1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST    (BIT(27))
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_M  (SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_V << SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_S  27
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST    (BIT(28))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_S  28
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST    (BIT(29))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_S  29
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST    (BIT(30))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_S  30
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch3 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST    (BIT(31))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_S  31

/** SOC_ETM_EVT_ST0_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST0_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1ac)
/** SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch0_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_CLR    (BIT(0))
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH0_RISE_EDGE_ST_CLR_S  0
/** SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch1_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_CLR    (BIT(1))
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH1_RISE_EDGE_ST_CLR_S  1
/** SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch2_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_CLR    (BIT(2))
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH2_RISE_EDGE_ST_CLR_S  2
/** SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch3_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_CLR    (BIT(3))
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH3_RISE_EDGE_ST_CLR_S  3
/** SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch4_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_CLR    (BIT(4))
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH4_RISE_EDGE_ST_CLR_S  4
/** SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch5_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_CLR    (BIT(5))
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH5_RISE_EDGE_ST_CLR_S  5
/** SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch6_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_CLR    (BIT(6))
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH6_RISE_EDGE_ST_CLR_S  6
/** SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch7_rise_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_CLR    (BIT(7))
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH7_RISE_EDGE_ST_CLR_S  7
/** SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch0_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_CLR    (BIT(8))
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH0_FALL_EDGE_ST_CLR_S  8
/** SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch1_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_CLR    (BIT(9))
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH1_FALL_EDGE_ST_CLR_S  9
/** SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch2_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_CLR    (BIT(10))
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH2_FALL_EDGE_ST_CLR_S  10
/** SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch3_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_CLR    (BIT(11))
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH3_FALL_EDGE_ST_CLR_S  11
/** SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch4_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_CLR    (BIT(12))
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH4_FALL_EDGE_ST_CLR_S  12
/** SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch5_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_CLR    (BIT(13))
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH5_FALL_EDGE_ST_CLR_S  13
/** SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch6_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_CLR    (BIT(14))
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH6_FALL_EDGE_ST_CLR_S  14
/** SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch7_fall_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_CLR    (BIT(15))
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH7_FALL_EDGE_ST_CLR_S  15
/** SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch0_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_CLR    (BIT(16))
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH0_ANY_EDGE_ST_CLR_S  16
/** SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch1_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_CLR    (BIT(17))
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH1_ANY_EDGE_ST_CLR_S  17
/** SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch2_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_CLR    (BIT(18))
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH2_ANY_EDGE_ST_CLR_S  18
/** SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch3_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_CLR    (BIT(19))
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH3_ANY_EDGE_ST_CLR_S  19
/** SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch4_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_CLR    (BIT(20))
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH4_ANY_EDGE_ST_CLR_S  20
/** SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch5_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_CLR    (BIT(21))
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH5_ANY_EDGE_ST_CLR_S  21
/** SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch6_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_CLR    (BIT(22))
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH6_ANY_EDGE_ST_CLR_S  22
/** SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear GPIO_evt_ch7_any_edge trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_CLR    (BIT(23))
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_CLR_M  (SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_CLR_V << SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_CH7_ANY_EDGE_ST_CLR_S  23
/** SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear GPIO_evt_zero_det_pos0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_CLR    (BIT(24))
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_CLR_M  (SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_CLR_V << SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS0_ST_CLR_S  24
/** SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear GPIO_evt_zero_det_neg0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_CLR    (BIT(25))
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_CLR_M  (SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_CLR_V << SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG0_ST_CLR_S  25
/** SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear GPIO_evt_zero_det_pos1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_CLR    (BIT(26))
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_CLR_M  (SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_CLR_V << SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_POS1_ST_CLR_S  26
/** SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear GPIO_evt_zero_det_neg1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_CLR    (BIT(27))
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_CLR_M  (SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_CLR_V << SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_CLR_S)
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_EVT_ZERO_DET_NEG1_ST_CLR_S  27
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_CLR    (BIT(28))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH0_ST_CLR_S  28
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_CLR    (BIT(29))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH1_ST_CLR_S  29
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_CLR    (BIT(30))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH2_ST_CLR_S  30
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_CLR    (BIT(31))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH3_ST_CLR_S  31

/** SOC_ETM_EVT_ST1_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST1_REG (DR_REG_SOC_ETM_BASE + 0x1b0)
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch4 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST    (BIT(0))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_S  0
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch5 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST    (BIT(1))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_S  1
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch6 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST    (BIT(2))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_S  2
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents LEDC_evt_duty_chng_end_ch7 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST    (BIT(3))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_S  3
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST    (BIT(4))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_S  4
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST    (BIT(5))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_S  5
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST    (BIT(6))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_S  6
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST    (BIT(7))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_S  7
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch4 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST    (BIT(8))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_S  8
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch5 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST    (BIT(9))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_S  9
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch6 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST    (BIT(10))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_S  10
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents LEDC_evt_ovf_cnt_pls_ch7 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST    (BIT(11))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_S  11
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents LEDC_evt_time_ovf_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST    (BIT(12))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_S  12
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents LEDC_evt_time_ovf_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST    (BIT(13))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_S  13
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents LEDC_evt_time_ovf_timer2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST    (BIT(14))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_S  14
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents LEDC_evt_time_ovf_timer3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST    (BIT(15))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_S  15
/** SOC_ETM_LEDC_EVT_TIMER0_CMP_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents LEDC_evt_timer0_cmp trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST    (BIT(16))
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_M  (SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_V << SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_S)
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_S  16
/** SOC_ETM_LEDC_EVT_TIMER1_CMP_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents LEDC_evt_timer1_cmp trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST    (BIT(17))
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_M  (SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_V << SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_S)
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_S  17
/** SOC_ETM_LEDC_EVT_TIMER2_CMP_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents LEDC_evt_timer2_cmp trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST    (BIT(18))
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_M  (SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_V << SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_S)
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_S  18
/** SOC_ETM_LEDC_EVT_TIMER3_CMP_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents LEDC_evt_timer3_cmp trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST    (BIT(19))
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_M  (SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_V << SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_S)
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_S  19
/** SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents TG0_evt_cnt_cmp_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST    (BIT(20))
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_M  (SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_V << SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_S)
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_S  20
/** SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents TG0_evt_cnt_cmp_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST    (BIT(21))
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_M  (SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_V << SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_S)
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_S  21
/** SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents TG1_evt_cnt_cmp_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST    (BIT(22))
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_M  (SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_V << SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_S)
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_S  22
/** SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents TG1_evt_cnt_cmp_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST    (BIT(23))
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_M  (SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_V << SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_S)
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_S  23
/** SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents SYSTIMER_evt_cnt_cmp0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST    (BIT(24))
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_M  (SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_V << SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_S)
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_V  0x00000001U
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_S  24
/** SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents SYSTIMER_evt_cnt_cmp1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST    (BIT(25))
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_M  (SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_V << SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_S)
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_V  0x00000001U
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_S  25
/** SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents SYSTIMER_evt_cnt_cmp2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST    (BIT(26))
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_M  (SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_V << SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_S)
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_V  0x00000001U
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_S  26
/** SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents MCPWM0_evt_timer0_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST    (BIT(27))
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_V << SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_S  27
/** SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents MCPWM0_evt_timer1_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST    (BIT(28))
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_V << SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_S  28
/** SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents MCPWM0_evt_timer2_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST    (BIT(29))
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_V << SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_S  29
/** SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents MCPWM0_evt_timer0_tez trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST    (BIT(30))
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_V << SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_S  30
/** SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents MCPWM0_evt_timer1_tez trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST    (BIT(31))
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_V << SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_S  31

/** SOC_ETM_EVT_ST1_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST1_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1b4)
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch4 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_CLR    (BIT(0))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH4_ST_CLR_S  0
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch5 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_CLR    (BIT(1))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH5_ST_CLR_S  1
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch6 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_CLR    (BIT(2))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH6_ST_CLR_S  2
/** SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch7 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_CLR    (BIT(3))
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_CLR_M  (SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_CLR_V << SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_DUTY_CHNG_END_CH7_ST_CLR_S  3
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_CLR    (BIT(4))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH0_ST_CLR_S  4
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_CLR    (BIT(5))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH1_ST_CLR_S  5
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_CLR    (BIT(6))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH2_ST_CLR_S  6
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_CLR    (BIT(7))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH3_ST_CLR_S  7
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch4 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_CLR    (BIT(8))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH4_ST_CLR_S  8
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch5 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_CLR    (BIT(9))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH5_ST_CLR_S  9
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch6 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_CLR    (BIT(10))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH6_ST_CLR_S  10
/** SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch7 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_CLR    (BIT(11))
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_CLR_M  (SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_CLR_V << SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_OVF_CNT_PLS_CH7_ST_CLR_S  11
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear LEDC_evt_time_ovf_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_CLR    (BIT(12))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_CLR_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER0_ST_CLR_S  12
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear LEDC_evt_time_ovf_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_CLR    (BIT(13))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_CLR_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER1_ST_CLR_S  13
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear LEDC_evt_time_ovf_timer2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_CLR    (BIT(14))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_CLR_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER2_ST_CLR_S  14
/** SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear LEDC_evt_time_ovf_timer3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_CLR    (BIT(15))
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_CLR_V << SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIME_OVF_TIMER3_ST_CLR_S  15
/** SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear LEDC_evt_timer0_cmp trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_CLR    (BIT(16))
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_CLR_V << SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER0_CMP_ST_CLR_S  16
/** SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear LEDC_evt_timer1_cmp trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_CLR    (BIT(17))
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_CLR_V << SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER1_CMP_ST_CLR_S  17
/** SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear LEDC_evt_timer2_cmp trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_CLR    (BIT(18))
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_CLR_V << SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER2_CMP_ST_CLR_S  18
/** SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear LEDC_evt_timer3_cmp trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_CLR    (BIT(19))
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_CLR_M  (SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_CLR_V << SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_CLR_S)
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_EVT_TIMER3_CMP_ST_CLR_S  19
/** SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear TG0_evt_cnt_cmp_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_CLR    (BIT(20))
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_CLR_M  (SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_CLR_V << SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_CLR_S)
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER0_ST_CLR_S  20
/** SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear TG0_evt_cnt_cmp_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_CLR    (BIT(21))
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_CLR_M  (SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_CLR_V << SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_CLR_S)
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_EVT_CNT_CMP_TIMER1_ST_CLR_S  21
/** SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear TG1_evt_cnt_cmp_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_CLR    (BIT(22))
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_CLR_M  (SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_CLR_V << SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_CLR_S)
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER0_ST_CLR_S  22
/** SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear TG1_evt_cnt_cmp_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_CLR    (BIT(23))
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_CLR_M  (SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_CLR_V << SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_CLR_S)
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_EVT_CNT_CMP_TIMER1_ST_CLR_S  23
/** SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear SYSTIMER_evt_cnt_cmp0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_CLR    (BIT(24))
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_CLR_M  (SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_CLR_V << SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_CLR_S)
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_CLR_V  0x00000001U
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP0_ST_CLR_S  24
/** SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear SYSTIMER_evt_cnt_cmp1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_CLR    (BIT(25))
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_CLR_M  (SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_CLR_V << SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_CLR_S)
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_CLR_V  0x00000001U
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP1_ST_CLR_S  25
/** SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear SYSTIMER_evt_cnt_cmp2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_CLR    (BIT(26))
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_CLR_M  (SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_CLR_V << SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_CLR_S)
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_CLR_V  0x00000001U
#define SOC_ETM_SYSTIMER_EVT_CNT_CMP2_ST_CLR_S  26
/** SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer0_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_CLR    (BIT(27))
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER0_STOP_ST_CLR_S  27
/** SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer1_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_CLR    (BIT(28))
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER1_STOP_ST_CLR_S  28
/** SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer2_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_CLR    (BIT(29))
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER2_STOP_ST_CLR_S  29
/** SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer0_tez trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_CLR    (BIT(30))
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEZ_ST_CLR_S  30
/** SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer1_tez trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_CLR    (BIT(31))
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEZ_ST_CLR_S  31

/** SOC_ETM_EVT_ST2_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST2_REG (DR_REG_SOC_ETM_BASE + 0x1b8)
/** SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents MCPWM0_evt_timer2_tez trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST    (BIT(0))
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_V << SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_S  0
/** SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents MCPWM0_evt_timer0_tep trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST    (BIT(1))
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_V << SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_S  1
/** SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents MCPWM0_evt_timer1_tep trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST    (BIT(2))
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_V << SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_S  2
/** SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents MCPWM0_evt_timer2_tep trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST    (BIT(3))
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_M  (SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_V << SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_S)
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_S  3
/** SOC_ETM_MCPWM0_EVT_OP0_TEA_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents MCPWM0_evt_op0_tea trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST    (BIT(4))
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_M  (SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_V << SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_S  4
/** SOC_ETM_MCPWM0_EVT_OP1_TEA_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents MCPWM0_evt_op1_tea trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST    (BIT(5))
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_M  (SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_V << SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_S  5
/** SOC_ETM_MCPWM0_EVT_OP2_TEA_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents MCPWM0_evt_op2_tea trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST    (BIT(6))
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_M  (SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_V << SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_S  6
/** SOC_ETM_MCPWM0_EVT_OP0_TEB_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents MCPWM0_evt_op0_teb trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST    (BIT(7))
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_M  (SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_V << SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_S  7
/** SOC_ETM_MCPWM0_EVT_OP1_TEB_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents MCPWM0_evt_op1_teb trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST    (BIT(8))
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_M  (SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_V << SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_S  8
/** SOC_ETM_MCPWM0_EVT_OP2_TEB_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents MCPWM0_evt_op2_teb trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST    (BIT(9))
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_M  (SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_V << SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_S  9
/** SOC_ETM_MCPWM0_EVT_F0_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents MCPWM0_evt_f0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_F0_ST    (BIT(10))
#define SOC_ETM_MCPWM0_EVT_F0_ST_M  (SOC_ETM_MCPWM0_EVT_F0_ST_V << SOC_ETM_MCPWM0_EVT_F0_ST_S)
#define SOC_ETM_MCPWM0_EVT_F0_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F0_ST_S  10
/** SOC_ETM_MCPWM0_EVT_F1_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents MCPWM0_evt_f1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_F1_ST    (BIT(11))
#define SOC_ETM_MCPWM0_EVT_F1_ST_M  (SOC_ETM_MCPWM0_EVT_F1_ST_V << SOC_ETM_MCPWM0_EVT_F1_ST_S)
#define SOC_ETM_MCPWM0_EVT_F1_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F1_ST_S  11
/** SOC_ETM_MCPWM0_EVT_F2_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents MCPWM0_evt_f2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_F2_ST    (BIT(12))
#define SOC_ETM_MCPWM0_EVT_F2_ST_M  (SOC_ETM_MCPWM0_EVT_F2_ST_V << SOC_ETM_MCPWM0_EVT_F2_ST_S)
#define SOC_ETM_MCPWM0_EVT_F2_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F2_ST_S  12
/** SOC_ETM_MCPWM0_EVT_F0_CLR_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents MCPWM0_evt_f0_clr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST    (BIT(13))
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST_M  (SOC_ETM_MCPWM0_EVT_F0_CLR_ST_V << SOC_ETM_MCPWM0_EVT_F0_CLR_ST_S)
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST_S  13
/** SOC_ETM_MCPWM0_EVT_F1_CLR_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents MCPWM0_evt_f1_clr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST    (BIT(14))
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST_M  (SOC_ETM_MCPWM0_EVT_F1_CLR_ST_V << SOC_ETM_MCPWM0_EVT_F1_CLR_ST_S)
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST_S  14
/** SOC_ETM_MCPWM0_EVT_F2_CLR_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents MCPWM0_evt_f2_clr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST    (BIT(15))
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST_M  (SOC_ETM_MCPWM0_EVT_F2_CLR_ST_V << SOC_ETM_MCPWM0_EVT_F2_CLR_ST_S)
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST_S  15
/** SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents MCPWM0_evt_tz0_cbc trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST    (BIT(16))
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_M  (SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_V << SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_S)
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_S  16
/** SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents MCPWM0_evt_tz1_cbc trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST    (BIT(17))
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_M  (SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_V << SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_S)
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_S  17
/** SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents MCPWM0_evt_tz2_cbc trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST    (BIT(18))
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_M  (SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_V << SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_S)
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_S  18
/** SOC_ETM_MCPWM0_EVT_TZ0_OST_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents MCPWM0_evt_tz0_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST    (BIT(19))
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_M  (SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_V << SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_S)
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_S  19
/** SOC_ETM_MCPWM0_EVT_TZ1_OST_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents MCPWM0_evt_tz1_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST    (BIT(20))
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_M  (SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_V << SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_S)
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_S  20
/** SOC_ETM_MCPWM0_EVT_TZ2_OST_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents MCPWM0_evt_tz2_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST    (BIT(21))
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_M  (SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_V << SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_S)
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_S  21
/** SOC_ETM_MCPWM0_EVT_CAP0_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents MCPWM0_evt_cap0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_CAP0_ST    (BIT(22))
#define SOC_ETM_MCPWM0_EVT_CAP0_ST_M  (SOC_ETM_MCPWM0_EVT_CAP0_ST_V << SOC_ETM_MCPWM0_EVT_CAP0_ST_S)
#define SOC_ETM_MCPWM0_EVT_CAP0_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_CAP0_ST_S  22
/** SOC_ETM_MCPWM0_EVT_CAP1_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents MCPWM0_evt_cap1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_CAP1_ST    (BIT(23))
#define SOC_ETM_MCPWM0_EVT_CAP1_ST_M  (SOC_ETM_MCPWM0_EVT_CAP1_ST_V << SOC_ETM_MCPWM0_EVT_CAP1_ST_S)
#define SOC_ETM_MCPWM0_EVT_CAP1_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_CAP1_ST_S  23
/** SOC_ETM_MCPWM0_EVT_CAP2_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents MCPWM0_evt_cap2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_CAP2_ST    (BIT(24))
#define SOC_ETM_MCPWM0_EVT_CAP2_ST_M  (SOC_ETM_MCPWM0_EVT_CAP2_ST_V << SOC_ETM_MCPWM0_EVT_CAP2_ST_S)
#define SOC_ETM_MCPWM0_EVT_CAP2_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_CAP2_ST_S  24
/** SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents MCPWM0_evt_op0_tee1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST    (BIT(25))
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_M  (SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_V << SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_S  25
/** SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents MCPWM0_evt_op1_tee1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST    (BIT(26))
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_M  (SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_V << SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_S  26
/** SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents MCPWM0_evt_op2_tee1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST    (BIT(27))
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_M  (SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_V << SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_S  27
/** SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents MCPWM0_evt_op0_tee2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST    (BIT(28))
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_M  (SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_V << SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_S  28
/** SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents MCPWM0_evt_op1_tee2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST    (BIT(29))
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_M  (SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_V << SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_S  29
/** SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents MCPWM0_evt_op2_tee2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST    (BIT(30))
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_M  (SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_V << SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_S  30
/** SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents MCPWM1_evt_timer0_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST    (BIT(31))
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_V << SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_S  31

/** SOC_ETM_EVT_ST2_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST2_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1bc)
/** SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer2_tez trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_CLR    (BIT(0))
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEZ_ST_CLR_S  0
/** SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer0_tep trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_CLR    (BIT(1))
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER0_TEP_ST_CLR_S  1
/** SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer1_tep trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_CLR    (BIT(2))
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER1_TEP_ST_CLR_S  2
/** SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_timer2_tep trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_CLR    (BIT(3))
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TIMER2_TEP_ST_CLR_S  3
/** SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op0_tea trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_CLR    (BIT(4))
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEA_ST_CLR_S  4
/** SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op1_tea trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_CLR    (BIT(5))
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEA_ST_CLR_S  5
/** SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op2_tea trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_CLR    (BIT(6))
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEA_ST_CLR_S  6
/** SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op0_teb trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_CLR    (BIT(7))
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEB_ST_CLR_S  7
/** SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op1_teb trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_CLR    (BIT(8))
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEB_ST_CLR_S  8
/** SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op2_teb trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_CLR    (BIT(9))
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEB_ST_CLR_S  9
/** SOC_ETM_MCPWM0_EVT_F0_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_f0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_F0_ST_CLR    (BIT(10))
#define SOC_ETM_MCPWM0_EVT_F0_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_F0_ST_CLR_V << SOC_ETM_MCPWM0_EVT_F0_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_F0_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F0_ST_CLR_S  10
/** SOC_ETM_MCPWM0_EVT_F1_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_f1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_F1_ST_CLR    (BIT(11))
#define SOC_ETM_MCPWM0_EVT_F1_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_F1_ST_CLR_V << SOC_ETM_MCPWM0_EVT_F1_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_F1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F1_ST_CLR_S  11
/** SOC_ETM_MCPWM0_EVT_F2_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_f2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_F2_ST_CLR    (BIT(12))
#define SOC_ETM_MCPWM0_EVT_F2_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_F2_ST_CLR_V << SOC_ETM_MCPWM0_EVT_F2_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_F2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F2_ST_CLR_S  12
/** SOC_ETM_MCPWM0_EVT_F0_CLR_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_f0_clr trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST_CLR    (BIT(13))
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_F0_CLR_ST_CLR_V << SOC_ETM_MCPWM0_EVT_F0_CLR_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F0_CLR_ST_CLR_S  13
/** SOC_ETM_MCPWM0_EVT_F1_CLR_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_f1_clr trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST_CLR    (BIT(14))
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_F1_CLR_ST_CLR_V << SOC_ETM_MCPWM0_EVT_F1_CLR_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F1_CLR_ST_CLR_S  14
/** SOC_ETM_MCPWM0_EVT_F2_CLR_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_f2_clr trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST_CLR    (BIT(15))
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_F2_CLR_ST_CLR_V << SOC_ETM_MCPWM0_EVT_F2_CLR_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_F2_CLR_ST_CLR_S  15
/** SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_tz0_cbc trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_CLR    (BIT(16))
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ0_CBC_ST_CLR_S  16
/** SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_tz1_cbc trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_CLR    (BIT(17))
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ1_CBC_ST_CLR_S  17
/** SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_tz2_cbc trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_CLR    (BIT(18))
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ2_CBC_ST_CLR_S  18
/** SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_tz0_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_CLR    (BIT(19))
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ0_OST_ST_CLR_S  19
/** SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_tz1_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_CLR    (BIT(20))
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ1_OST_ST_CLR_S  20
/** SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_tz2_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_CLR    (BIT(21))
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_CLR_V << SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_TZ2_OST_ST_CLR_S  21
/** SOC_ETM_MCPWM0_EVT_CAP0_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_cap0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_CAP0_ST_CLR    (BIT(22))
#define SOC_ETM_MCPWM0_EVT_CAP0_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_CAP0_ST_CLR_V << SOC_ETM_MCPWM0_EVT_CAP0_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_CAP0_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_CAP0_ST_CLR_S  22
/** SOC_ETM_MCPWM0_EVT_CAP1_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_cap1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_CAP1_ST_CLR    (BIT(23))
#define SOC_ETM_MCPWM0_EVT_CAP1_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_CAP1_ST_CLR_V << SOC_ETM_MCPWM0_EVT_CAP1_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_CAP1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_CAP1_ST_CLR_S  23
/** SOC_ETM_MCPWM0_EVT_CAP2_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_cap2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_CAP2_ST_CLR    (BIT(24))
#define SOC_ETM_MCPWM0_EVT_CAP2_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_CAP2_ST_CLR_V << SOC_ETM_MCPWM0_EVT_CAP2_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_CAP2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_CAP2_ST_CLR_S  24
/** SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op0_tee1 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_CLR    (BIT(25))
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEE1_ST_CLR_S  25
/** SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op1_tee1 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_CLR    (BIT(26))
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEE1_ST_CLR_S  26
/** SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op2_tee1 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_CLR    (BIT(27))
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEE1_ST_CLR_S  27
/** SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op0_tee2 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_CLR    (BIT(28))
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP0_TEE2_ST_CLR_S  28
/** SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op1_tee2 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_CLR    (BIT(29))
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP1_TEE2_ST_CLR_S  29
/** SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear MCPWM0_evt_op2_tee2 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_CLR    (BIT(30))
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_CLR_M  (SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_CLR_V << SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_CLR_S)
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_EVT_OP2_TEE2_ST_CLR_S  30
/** SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer0_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_CLR    (BIT(31))
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER0_STOP_ST_CLR_S  31

/** SOC_ETM_EVT_ST3_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST3_REG (DR_REG_SOC_ETM_BASE + 0x1c0)
/** SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents MCPWM1_evt_timer1_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST    (BIT(0))
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_V << SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_S  0
/** SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents MCPWM1_evt_timer2_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST    (BIT(1))
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_V << SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_S  1
/** SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents MCPWM1_evt_timer0_tez trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST    (BIT(2))
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_V << SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_S  2
/** SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents MCPWM1_evt_timer1_tez trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST    (BIT(3))
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_V << SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_S  3
/** SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents MCPWM1_evt_timer2_tez trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST    (BIT(4))
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_V << SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_S  4
/** SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents MCPWM1_evt_timer0_tep trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST    (BIT(5))
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_V << SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_S  5
/** SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents MCPWM1_evt_timer1_tep trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST    (BIT(6))
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_V << SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_S  6
/** SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents MCPWM1_evt_timer2_tep trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST    (BIT(7))
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_M  (SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_V << SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_S)
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_S  7
/** SOC_ETM_MCPWM1_EVT_OP0_TEA_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents MCPWM1_evt_op0_tea trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST    (BIT(8))
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_M  (SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_V << SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_S  8
/** SOC_ETM_MCPWM1_EVT_OP1_TEA_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents MCPWM1_evt_op1_tea trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST    (BIT(9))
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_M  (SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_V << SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_S  9
/** SOC_ETM_MCPWM1_EVT_OP2_TEA_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents MCPWM1_evt_op2_tea trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST    (BIT(10))
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_M  (SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_V << SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_S  10
/** SOC_ETM_MCPWM1_EVT_OP0_TEB_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents MCPWM1_evt_op0_teb trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST    (BIT(11))
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_M  (SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_V << SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_S  11
/** SOC_ETM_MCPWM1_EVT_OP1_TEB_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents MCPWM1_evt_op1_teb trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST    (BIT(12))
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_M  (SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_V << SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_S  12
/** SOC_ETM_MCPWM1_EVT_OP2_TEB_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents MCPWM1_evt_op2_teb trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST    (BIT(13))
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_M  (SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_V << SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_S  13
/** SOC_ETM_MCPWM1_EVT_F0_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents MCPWM1_evt_f0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_F0_ST    (BIT(14))
#define SOC_ETM_MCPWM1_EVT_F0_ST_M  (SOC_ETM_MCPWM1_EVT_F0_ST_V << SOC_ETM_MCPWM1_EVT_F0_ST_S)
#define SOC_ETM_MCPWM1_EVT_F0_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F0_ST_S  14
/** SOC_ETM_MCPWM1_EVT_F1_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents MCPWM1_evt_f1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_F1_ST    (BIT(15))
#define SOC_ETM_MCPWM1_EVT_F1_ST_M  (SOC_ETM_MCPWM1_EVT_F1_ST_V << SOC_ETM_MCPWM1_EVT_F1_ST_S)
#define SOC_ETM_MCPWM1_EVT_F1_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F1_ST_S  15
/** SOC_ETM_MCPWM1_EVT_F2_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents MCPWM1_evt_f2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_F2_ST    (BIT(16))
#define SOC_ETM_MCPWM1_EVT_F2_ST_M  (SOC_ETM_MCPWM1_EVT_F2_ST_V << SOC_ETM_MCPWM1_EVT_F2_ST_S)
#define SOC_ETM_MCPWM1_EVT_F2_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F2_ST_S  16
/** SOC_ETM_MCPWM1_EVT_F0_CLR_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents MCPWM1_evt_f0_clr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST    (BIT(17))
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST_M  (SOC_ETM_MCPWM1_EVT_F0_CLR_ST_V << SOC_ETM_MCPWM1_EVT_F0_CLR_ST_S)
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST_S  17
/** SOC_ETM_MCPWM1_EVT_F1_CLR_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents MCPWM1_evt_f1_clr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST    (BIT(18))
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST_M  (SOC_ETM_MCPWM1_EVT_F1_CLR_ST_V << SOC_ETM_MCPWM1_EVT_F1_CLR_ST_S)
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST_S  18
/** SOC_ETM_MCPWM1_EVT_F2_CLR_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents MCPWM1_evt_f2_clr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST    (BIT(19))
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST_M  (SOC_ETM_MCPWM1_EVT_F2_CLR_ST_V << SOC_ETM_MCPWM1_EVT_F2_CLR_ST_S)
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST_S  19
/** SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents MCPWM1_evt_tz0_cbc trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST    (BIT(20))
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_M  (SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_V << SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_S)
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_S  20
/** SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents MCPWM1_evt_tz1_cbc trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST    (BIT(21))
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_M  (SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_V << SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_S)
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_S  21
/** SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents MCPWM1_evt_tz2_cbc trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST    (BIT(22))
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_M  (SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_V << SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_S)
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_S  22
/** SOC_ETM_MCPWM1_EVT_TZ0_OST_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents MCPWM1_evt_tz0_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST    (BIT(23))
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_M  (SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_V << SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_S)
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_S  23
/** SOC_ETM_MCPWM1_EVT_TZ1_OST_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents MCPWM1_evt_tz1_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST    (BIT(24))
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_M  (SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_V << SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_S)
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_S  24
/** SOC_ETM_MCPWM1_EVT_TZ2_OST_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents MCPWM1_evt_tz2_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST    (BIT(25))
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_M  (SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_V << SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_S)
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_S  25
/** SOC_ETM_MCPWM1_EVT_CAP0_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents MCPWM1_evt_cap0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_CAP0_ST    (BIT(26))
#define SOC_ETM_MCPWM1_EVT_CAP0_ST_M  (SOC_ETM_MCPWM1_EVT_CAP0_ST_V << SOC_ETM_MCPWM1_EVT_CAP0_ST_S)
#define SOC_ETM_MCPWM1_EVT_CAP0_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_CAP0_ST_S  26
/** SOC_ETM_MCPWM1_EVT_CAP1_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents MCPWM1_evt_cap1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_CAP1_ST    (BIT(27))
#define SOC_ETM_MCPWM1_EVT_CAP1_ST_M  (SOC_ETM_MCPWM1_EVT_CAP1_ST_V << SOC_ETM_MCPWM1_EVT_CAP1_ST_S)
#define SOC_ETM_MCPWM1_EVT_CAP1_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_CAP1_ST_S  27
/** SOC_ETM_MCPWM1_EVT_CAP2_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents MCPWM1_evt_cap2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_CAP2_ST    (BIT(28))
#define SOC_ETM_MCPWM1_EVT_CAP2_ST_M  (SOC_ETM_MCPWM1_EVT_CAP2_ST_V << SOC_ETM_MCPWM1_EVT_CAP2_ST_S)
#define SOC_ETM_MCPWM1_EVT_CAP2_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_CAP2_ST_S  28
/** SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents MCPWM1_evt_op0_tee1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST    (BIT(29))
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_M  (SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_V << SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_S  29
/** SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents MCPWM1_evt_op1_tee1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST    (BIT(30))
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_M  (SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_V << SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_S  30
/** SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents MCPWM1_evt_op2_tee1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST    (BIT(31))
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_M  (SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_V << SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_S  31

/** SOC_ETM_EVT_ST3_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST3_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1c4)
/** SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer1_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_CLR    (BIT(0))
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER1_STOP_ST_CLR_S  0
/** SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer2_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_CLR    (BIT(1))
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER2_STOP_ST_CLR_S  1
/** SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer0_tez trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_CLR    (BIT(2))
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEZ_ST_CLR_S  2
/** SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer1_tez trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_CLR    (BIT(3))
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEZ_ST_CLR_S  3
/** SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer2_tez trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_CLR    (BIT(4))
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEZ_ST_CLR_S  4
/** SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer0_tep trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_CLR    (BIT(5))
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER0_TEP_ST_CLR_S  5
/** SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer1_tep trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_CLR    (BIT(6))
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER1_TEP_ST_CLR_S  6
/** SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_timer2_tep trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_CLR    (BIT(7))
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TIMER2_TEP_ST_CLR_S  7
/** SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op0_tea trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_CLR    (BIT(8))
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEA_ST_CLR_S  8
/** SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op1_tea trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_CLR    (BIT(9))
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEA_ST_CLR_S  9
/** SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op2_tea trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_CLR    (BIT(10))
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEA_ST_CLR_S  10
/** SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op0_teb trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_CLR    (BIT(11))
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEB_ST_CLR_S  11
/** SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op1_teb trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_CLR    (BIT(12))
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEB_ST_CLR_S  12
/** SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op2_teb trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_CLR    (BIT(13))
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEB_ST_CLR_S  13
/** SOC_ETM_MCPWM1_EVT_F0_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_f0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_F0_ST_CLR    (BIT(14))
#define SOC_ETM_MCPWM1_EVT_F0_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_F0_ST_CLR_V << SOC_ETM_MCPWM1_EVT_F0_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_F0_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F0_ST_CLR_S  14
/** SOC_ETM_MCPWM1_EVT_F1_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_f1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_F1_ST_CLR    (BIT(15))
#define SOC_ETM_MCPWM1_EVT_F1_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_F1_ST_CLR_V << SOC_ETM_MCPWM1_EVT_F1_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_F1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F1_ST_CLR_S  15
/** SOC_ETM_MCPWM1_EVT_F2_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_f2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_F2_ST_CLR    (BIT(16))
#define SOC_ETM_MCPWM1_EVT_F2_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_F2_ST_CLR_V << SOC_ETM_MCPWM1_EVT_F2_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_F2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F2_ST_CLR_S  16
/** SOC_ETM_MCPWM1_EVT_F0_CLR_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_f0_clr trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST_CLR    (BIT(17))
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_F0_CLR_ST_CLR_V << SOC_ETM_MCPWM1_EVT_F0_CLR_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F0_CLR_ST_CLR_S  17
/** SOC_ETM_MCPWM1_EVT_F1_CLR_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_f1_clr trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST_CLR    (BIT(18))
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_F1_CLR_ST_CLR_V << SOC_ETM_MCPWM1_EVT_F1_CLR_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F1_CLR_ST_CLR_S  18
/** SOC_ETM_MCPWM1_EVT_F2_CLR_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_f2_clr trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST_CLR    (BIT(19))
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_F2_CLR_ST_CLR_V << SOC_ETM_MCPWM1_EVT_F2_CLR_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_F2_CLR_ST_CLR_S  19
/** SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_tz0_cbc trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_CLR    (BIT(20))
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ0_CBC_ST_CLR_S  20
/** SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_tz1_cbc trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_CLR    (BIT(21))
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ1_CBC_ST_CLR_S  21
/** SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_tz2_cbc trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_CLR    (BIT(22))
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ2_CBC_ST_CLR_S  22
/** SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_tz0_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_CLR    (BIT(23))
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ0_OST_ST_CLR_S  23
/** SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_tz1_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_CLR    (BIT(24))
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ1_OST_ST_CLR_S  24
/** SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_tz2_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_CLR    (BIT(25))
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_CLR_V << SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_TZ2_OST_ST_CLR_S  25
/** SOC_ETM_MCPWM1_EVT_CAP0_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_cap0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_CAP0_ST_CLR    (BIT(26))
#define SOC_ETM_MCPWM1_EVT_CAP0_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_CAP0_ST_CLR_V << SOC_ETM_MCPWM1_EVT_CAP0_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_CAP0_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_CAP0_ST_CLR_S  26
/** SOC_ETM_MCPWM1_EVT_CAP1_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_cap1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_CAP1_ST_CLR    (BIT(27))
#define SOC_ETM_MCPWM1_EVT_CAP1_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_CAP1_ST_CLR_V << SOC_ETM_MCPWM1_EVT_CAP1_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_CAP1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_CAP1_ST_CLR_S  27
/** SOC_ETM_MCPWM1_EVT_CAP2_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_cap2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_CAP2_ST_CLR    (BIT(28))
#define SOC_ETM_MCPWM1_EVT_CAP2_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_CAP2_ST_CLR_V << SOC_ETM_MCPWM1_EVT_CAP2_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_CAP2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_CAP2_ST_CLR_S  28
/** SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op0_tee1 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_CLR    (BIT(29))
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEE1_ST_CLR_S  29
/** SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op1_tee1 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_CLR    (BIT(30))
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEE1_ST_CLR_S  30
/** SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op2_tee1 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_CLR    (BIT(31))
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEE1_ST_CLR_S  31

/** SOC_ETM_EVT_ST4_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST4_REG (DR_REG_SOC_ETM_BASE + 0x1c8)
/** SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents MCPWM1_evt_op0_tee2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST    (BIT(0))
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_M  (SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_V << SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_S  0
/** SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents MCPWM1_evt_op1_tee2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST    (BIT(1))
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_M  (SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_V << SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_S  1
/** SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents MCPWM1_evt_op2_tee2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST    (BIT(2))
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_M  (SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_V << SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_S  2
/** SOC_ETM_ADC_EVT_CONV_CMPLT0_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents ADC_evt_conv_cmplt0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST    (BIT(3))
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_M  (SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_V << SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_S)
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_S  3
/** SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents ADC_evt_eq_above_thresh0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST    (BIT(4))
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_M  (SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_V << SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_S)
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_S  4
/** SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents ADC_evt_eq_above_thresh1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST    (BIT(5))
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_M  (SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_V << SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_S)
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_S  5
/** SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents ADC_evt_eq_below_thresh0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST    (BIT(6))
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_M  (SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_V << SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_S)
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_S  6
/** SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents ADC_evt_eq_below_thresh1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST    (BIT(7))
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_M  (SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_V << SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_S)
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_S  7
/** SOC_ETM_ADC_EVT_RESULT_DONE0_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents ADC_evt_result_done0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST    (BIT(8))
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST_M  (SOC_ETM_ADC_EVT_RESULT_DONE0_ST_V << SOC_ETM_ADC_EVT_RESULT_DONE0_ST_S)
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST_S  8
/** SOC_ETM_ADC_EVT_STOPPED0_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents ADC_evt_stopped0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_STOPPED0_ST    (BIT(9))
#define SOC_ETM_ADC_EVT_STOPPED0_ST_M  (SOC_ETM_ADC_EVT_STOPPED0_ST_V << SOC_ETM_ADC_EVT_STOPPED0_ST_S)
#define SOC_ETM_ADC_EVT_STOPPED0_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_STOPPED0_ST_S  9
/** SOC_ETM_ADC_EVT_STARTED0_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents ADC_evt_started0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_EVT_STARTED0_ST    (BIT(10))
#define SOC_ETM_ADC_EVT_STARTED0_ST_M  (SOC_ETM_ADC_EVT_STARTED0_ST_V << SOC_ETM_ADC_EVT_STARTED0_ST_S)
#define SOC_ETM_ADC_EVT_STARTED0_ST_V  0x00000001U
#define SOC_ETM_ADC_EVT_STARTED0_ST_S  10
/** SOC_ETM_REGDMA_EVT_DONE0_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents REGDMA_evt_done0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_DONE0_ST    (BIT(11))
#define SOC_ETM_REGDMA_EVT_DONE0_ST_M  (SOC_ETM_REGDMA_EVT_DONE0_ST_V << SOC_ETM_REGDMA_EVT_DONE0_ST_S)
#define SOC_ETM_REGDMA_EVT_DONE0_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE0_ST_S  11
/** SOC_ETM_REGDMA_EVT_DONE1_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents REGDMA_evt_done1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_DONE1_ST    (BIT(12))
#define SOC_ETM_REGDMA_EVT_DONE1_ST_M  (SOC_ETM_REGDMA_EVT_DONE1_ST_V << SOC_ETM_REGDMA_EVT_DONE1_ST_S)
#define SOC_ETM_REGDMA_EVT_DONE1_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE1_ST_S  12
/** SOC_ETM_REGDMA_EVT_DONE2_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents REGDMA_evt_done2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_DONE2_ST    (BIT(13))
#define SOC_ETM_REGDMA_EVT_DONE2_ST_M  (SOC_ETM_REGDMA_EVT_DONE2_ST_V << SOC_ETM_REGDMA_EVT_DONE2_ST_S)
#define SOC_ETM_REGDMA_EVT_DONE2_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE2_ST_S  13
/** SOC_ETM_REGDMA_EVT_DONE3_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents REGDMA_evt_done3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_DONE3_ST    (BIT(14))
#define SOC_ETM_REGDMA_EVT_DONE3_ST_M  (SOC_ETM_REGDMA_EVT_DONE3_ST_V << SOC_ETM_REGDMA_EVT_DONE3_ST_S)
#define SOC_ETM_REGDMA_EVT_DONE3_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE3_ST_S  14
/** SOC_ETM_REGDMA_EVT_ERR0_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents REGDMA_evt_err0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_ERR0_ST    (BIT(15))
#define SOC_ETM_REGDMA_EVT_ERR0_ST_M  (SOC_ETM_REGDMA_EVT_ERR0_ST_V << SOC_ETM_REGDMA_EVT_ERR0_ST_S)
#define SOC_ETM_REGDMA_EVT_ERR0_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR0_ST_S  15
/** SOC_ETM_REGDMA_EVT_ERR1_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents REGDMA_evt_err1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_ERR1_ST    (BIT(16))
#define SOC_ETM_REGDMA_EVT_ERR1_ST_M  (SOC_ETM_REGDMA_EVT_ERR1_ST_V << SOC_ETM_REGDMA_EVT_ERR1_ST_S)
#define SOC_ETM_REGDMA_EVT_ERR1_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR1_ST_S  16
/** SOC_ETM_REGDMA_EVT_ERR2_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents REGDMA_evt_err2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_ERR2_ST    (BIT(17))
#define SOC_ETM_REGDMA_EVT_ERR2_ST_M  (SOC_ETM_REGDMA_EVT_ERR2_ST_V << SOC_ETM_REGDMA_EVT_ERR2_ST_S)
#define SOC_ETM_REGDMA_EVT_ERR2_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR2_ST_S  17
/** SOC_ETM_REGDMA_EVT_ERR3_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents REGDMA_evt_err3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_EVT_ERR3_ST    (BIT(18))
#define SOC_ETM_REGDMA_EVT_ERR3_ST_M  (SOC_ETM_REGDMA_EVT_ERR3_ST_V << SOC_ETM_REGDMA_EVT_ERR3_ST_S)
#define SOC_ETM_REGDMA_EVT_ERR3_ST_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR3_ST_S  18
/** SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents TMPSNSR_evt_over_limit trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST    (BIT(19))
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_M  (SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_V << SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_S)
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_V  0x00000001U
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_S  19
/** SOC_ETM_I2S0_EVT_RX_DONE_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents I2S0_evt_rx_done trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_EVT_RX_DONE_ST    (BIT(20))
#define SOC_ETM_I2S0_EVT_RX_DONE_ST_M  (SOC_ETM_I2S0_EVT_RX_DONE_ST_V << SOC_ETM_I2S0_EVT_RX_DONE_ST_S)
#define SOC_ETM_I2S0_EVT_RX_DONE_ST_V  0x00000001U
#define SOC_ETM_I2S0_EVT_RX_DONE_ST_S  20
/** SOC_ETM_I2S0_EVT_TX_DONE_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents I2S0_evt_tx_done trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_EVT_TX_DONE_ST    (BIT(21))
#define SOC_ETM_I2S0_EVT_TX_DONE_ST_M  (SOC_ETM_I2S0_EVT_TX_DONE_ST_V << SOC_ETM_I2S0_EVT_TX_DONE_ST_S)
#define SOC_ETM_I2S0_EVT_TX_DONE_ST_V  0x00000001U
#define SOC_ETM_I2S0_EVT_TX_DONE_ST_S  21
/** SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents I2S0_evt_x_words_received trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST    (BIT(22))
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_M  (SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_V << SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_S)
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_V  0x00000001U
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_S  22
/** SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents I2S0_evt_x_words_sent trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST    (BIT(23))
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_M  (SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_V << SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_S)
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_V  0x00000001U
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_S  23
/** SOC_ETM_I2S1_EVT_RX_DONE_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents I2S1_evt_rx_done trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_EVT_RX_DONE_ST    (BIT(24))
#define SOC_ETM_I2S1_EVT_RX_DONE_ST_M  (SOC_ETM_I2S1_EVT_RX_DONE_ST_V << SOC_ETM_I2S1_EVT_RX_DONE_ST_S)
#define SOC_ETM_I2S1_EVT_RX_DONE_ST_V  0x00000001U
#define SOC_ETM_I2S1_EVT_RX_DONE_ST_S  24
/** SOC_ETM_I2S1_EVT_TX_DONE_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents I2S1_evt_tx_done trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_EVT_TX_DONE_ST    (BIT(25))
#define SOC_ETM_I2S1_EVT_TX_DONE_ST_M  (SOC_ETM_I2S1_EVT_TX_DONE_ST_V << SOC_ETM_I2S1_EVT_TX_DONE_ST_S)
#define SOC_ETM_I2S1_EVT_TX_DONE_ST_V  0x00000001U
#define SOC_ETM_I2S1_EVT_TX_DONE_ST_S  25
/** SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents I2S1_evt_x_words_received trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST    (BIT(26))
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_M  (SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_V << SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_S)
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_V  0x00000001U
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_S  26
/** SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents I2S1_evt_x_words_sent trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST    (BIT(27))
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_M  (SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_V << SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_S)
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_V  0x00000001U
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_S  27
/** SOC_ETM_I2S2_EVT_RX_DONE_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents I2S2_evt_rx_done trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_EVT_RX_DONE_ST    (BIT(28))
#define SOC_ETM_I2S2_EVT_RX_DONE_ST_M  (SOC_ETM_I2S2_EVT_RX_DONE_ST_V << SOC_ETM_I2S2_EVT_RX_DONE_ST_S)
#define SOC_ETM_I2S2_EVT_RX_DONE_ST_V  0x00000001U
#define SOC_ETM_I2S2_EVT_RX_DONE_ST_S  28
/** SOC_ETM_I2S2_EVT_TX_DONE_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents I2S2_evt_tx_done trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_EVT_TX_DONE_ST    (BIT(29))
#define SOC_ETM_I2S2_EVT_TX_DONE_ST_M  (SOC_ETM_I2S2_EVT_TX_DONE_ST_V << SOC_ETM_I2S2_EVT_TX_DONE_ST_S)
#define SOC_ETM_I2S2_EVT_TX_DONE_ST_V  0x00000001U
#define SOC_ETM_I2S2_EVT_TX_DONE_ST_S  29
/** SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents I2S2_evt_x_words_received trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST    (BIT(30))
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_M  (SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_V << SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_S)
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_V  0x00000001U
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_S  30
/** SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents I2S2_evt_x_words_sent trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST    (BIT(31))
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_M  (SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_V << SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_S)
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_V  0x00000001U
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_S  31

/** SOC_ETM_EVT_ST4_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST4_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1cc)
/** SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op0_tee2 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_CLR    (BIT(0))
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP0_TEE2_ST_CLR_S  0
/** SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op1_tee2 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_CLR    (BIT(1))
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP1_TEE2_ST_CLR_S  1
/** SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear MCPWM1_evt_op2_tee2 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_CLR    (BIT(2))
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_CLR_M  (SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_CLR_V << SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_CLR_S)
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_EVT_OP2_TEE2_ST_CLR_S  2
/** SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear ADC_evt_conv_cmplt0 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_CLR    (BIT(3))
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_CLR_M  (SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_CLR_V << SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_CLR_S)
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_CONV_CMPLT0_ST_CLR_S  3
/** SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear ADC_evt_eq_above_thresh0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_CLR    (BIT(4))
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_CLR_M  (SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_CLR_V << SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_CLR_S)
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH0_ST_CLR_S  4
/** SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear ADC_evt_eq_above_thresh1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_CLR    (BIT(5))
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_CLR_M  (SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_CLR_V << SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_CLR_S)
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_ABOVE_THRESH1_ST_CLR_S  5
/** SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear ADC_evt_eq_below_thresh0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_CLR    (BIT(6))
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_CLR_M  (SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_CLR_V << SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_CLR_S)
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH0_ST_CLR_S  6
/** SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear ADC_evt_eq_below_thresh1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_CLR    (BIT(7))
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_CLR_M  (SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_CLR_V << SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_CLR_S)
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_EQ_BELOW_THRESH1_ST_CLR_S  7
/** SOC_ETM_ADC_EVT_RESULT_DONE0_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear ADC_evt_result_done0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST_CLR    (BIT(8))
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST_CLR_M  (SOC_ETM_ADC_EVT_RESULT_DONE0_ST_CLR_V << SOC_ETM_ADC_EVT_RESULT_DONE0_ST_CLR_S)
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_RESULT_DONE0_ST_CLR_S  8
/** SOC_ETM_ADC_EVT_STOPPED0_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear ADC_evt_stopped0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_STOPPED0_ST_CLR    (BIT(9))
#define SOC_ETM_ADC_EVT_STOPPED0_ST_CLR_M  (SOC_ETM_ADC_EVT_STOPPED0_ST_CLR_V << SOC_ETM_ADC_EVT_STOPPED0_ST_CLR_S)
#define SOC_ETM_ADC_EVT_STOPPED0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_STOPPED0_ST_CLR_S  9
/** SOC_ETM_ADC_EVT_STARTED0_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear ADC_evt_started0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ADC_EVT_STARTED0_ST_CLR    (BIT(10))
#define SOC_ETM_ADC_EVT_STARTED0_ST_CLR_M  (SOC_ETM_ADC_EVT_STARTED0_ST_CLR_V << SOC_ETM_ADC_EVT_STARTED0_ST_CLR_S)
#define SOC_ETM_ADC_EVT_STARTED0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_EVT_STARTED0_ST_CLR_S  10
/** SOC_ETM_REGDMA_EVT_DONE0_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_done0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_DONE0_ST_CLR    (BIT(11))
#define SOC_ETM_REGDMA_EVT_DONE0_ST_CLR_M  (SOC_ETM_REGDMA_EVT_DONE0_ST_CLR_V << SOC_ETM_REGDMA_EVT_DONE0_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_DONE0_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE0_ST_CLR_S  11
/** SOC_ETM_REGDMA_EVT_DONE1_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_done1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_DONE1_ST_CLR    (BIT(12))
#define SOC_ETM_REGDMA_EVT_DONE1_ST_CLR_M  (SOC_ETM_REGDMA_EVT_DONE1_ST_CLR_V << SOC_ETM_REGDMA_EVT_DONE1_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_DONE1_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE1_ST_CLR_S  12
/** SOC_ETM_REGDMA_EVT_DONE2_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_done2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_DONE2_ST_CLR    (BIT(13))
#define SOC_ETM_REGDMA_EVT_DONE2_ST_CLR_M  (SOC_ETM_REGDMA_EVT_DONE2_ST_CLR_V << SOC_ETM_REGDMA_EVT_DONE2_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_DONE2_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE2_ST_CLR_S  13
/** SOC_ETM_REGDMA_EVT_DONE3_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_done3 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_DONE3_ST_CLR    (BIT(14))
#define SOC_ETM_REGDMA_EVT_DONE3_ST_CLR_M  (SOC_ETM_REGDMA_EVT_DONE3_ST_CLR_V << SOC_ETM_REGDMA_EVT_DONE3_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_DONE3_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_DONE3_ST_CLR_S  14
/** SOC_ETM_REGDMA_EVT_ERR0_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_err0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_ERR0_ST_CLR    (BIT(15))
#define SOC_ETM_REGDMA_EVT_ERR0_ST_CLR_M  (SOC_ETM_REGDMA_EVT_ERR0_ST_CLR_V << SOC_ETM_REGDMA_EVT_ERR0_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_ERR0_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR0_ST_CLR_S  15
/** SOC_ETM_REGDMA_EVT_ERR1_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_err1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_ERR1_ST_CLR    (BIT(16))
#define SOC_ETM_REGDMA_EVT_ERR1_ST_CLR_M  (SOC_ETM_REGDMA_EVT_ERR1_ST_CLR_V << SOC_ETM_REGDMA_EVT_ERR1_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_ERR1_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR1_ST_CLR_S  16
/** SOC_ETM_REGDMA_EVT_ERR2_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_err2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_ERR2_ST_CLR    (BIT(17))
#define SOC_ETM_REGDMA_EVT_ERR2_ST_CLR_M  (SOC_ETM_REGDMA_EVT_ERR2_ST_CLR_V << SOC_ETM_REGDMA_EVT_ERR2_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_ERR2_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR2_ST_CLR_S  17
/** SOC_ETM_REGDMA_EVT_ERR3_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear REGDMA_evt_err3 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_REGDMA_EVT_ERR3_ST_CLR    (BIT(18))
#define SOC_ETM_REGDMA_EVT_ERR3_ST_CLR_M  (SOC_ETM_REGDMA_EVT_ERR3_ST_CLR_V << SOC_ETM_REGDMA_EVT_ERR3_ST_CLR_S)
#define SOC_ETM_REGDMA_EVT_ERR3_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_EVT_ERR3_ST_CLR_S  18
/** SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear TMPSNSR_evt_over_limit trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_CLR    (BIT(19))
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_CLR_M  (SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_CLR_V << SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_CLR_S)
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_CLR_V  0x00000001U
#define SOC_ETM_TMPSNSR_EVT_OVER_LIMIT_ST_CLR_S  19
/** SOC_ETM_I2S0_EVT_RX_DONE_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear I2S0_evt_rx_done trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_I2S0_EVT_RX_DONE_ST_CLR    (BIT(20))
#define SOC_ETM_I2S0_EVT_RX_DONE_ST_CLR_M  (SOC_ETM_I2S0_EVT_RX_DONE_ST_CLR_V << SOC_ETM_I2S0_EVT_RX_DONE_ST_CLR_S)
#define SOC_ETM_I2S0_EVT_RX_DONE_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_EVT_RX_DONE_ST_CLR_S  20
/** SOC_ETM_I2S0_EVT_TX_DONE_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear I2S0_evt_tx_done trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_I2S0_EVT_TX_DONE_ST_CLR    (BIT(21))
#define SOC_ETM_I2S0_EVT_TX_DONE_ST_CLR_M  (SOC_ETM_I2S0_EVT_TX_DONE_ST_CLR_V << SOC_ETM_I2S0_EVT_TX_DONE_ST_CLR_S)
#define SOC_ETM_I2S0_EVT_TX_DONE_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_EVT_TX_DONE_ST_CLR_S  21
/** SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear I2S0_evt_x_words_received trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_CLR    (BIT(22))
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_CLR_M  (SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_CLR_V << SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_CLR_S)
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_EVT_X_WORDS_RECEIVED_ST_CLR_S  22
/** SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear I2S0_evt_x_words_sent trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_CLR    (BIT(23))
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_CLR_M  (SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_CLR_V << SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_CLR_S)
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_EVT_X_WORDS_SENT_ST_CLR_S  23
/** SOC_ETM_I2S1_EVT_RX_DONE_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear I2S1_evt_rx_done trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_I2S1_EVT_RX_DONE_ST_CLR    (BIT(24))
#define SOC_ETM_I2S1_EVT_RX_DONE_ST_CLR_M  (SOC_ETM_I2S1_EVT_RX_DONE_ST_CLR_V << SOC_ETM_I2S1_EVT_RX_DONE_ST_CLR_S)
#define SOC_ETM_I2S1_EVT_RX_DONE_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_EVT_RX_DONE_ST_CLR_S  24
/** SOC_ETM_I2S1_EVT_TX_DONE_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear I2S1_evt_tx_done trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_I2S1_EVT_TX_DONE_ST_CLR    (BIT(25))
#define SOC_ETM_I2S1_EVT_TX_DONE_ST_CLR_M  (SOC_ETM_I2S1_EVT_TX_DONE_ST_CLR_V << SOC_ETM_I2S1_EVT_TX_DONE_ST_CLR_S)
#define SOC_ETM_I2S1_EVT_TX_DONE_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_EVT_TX_DONE_ST_CLR_S  25
/** SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear I2S1_evt_x_words_received trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_CLR    (BIT(26))
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_CLR_M  (SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_CLR_V << SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_CLR_S)
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_EVT_X_WORDS_RECEIVED_ST_CLR_S  26
/** SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear I2S1_evt_x_words_sent trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_CLR    (BIT(27))
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_CLR_M  (SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_CLR_V << SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_CLR_S)
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_EVT_X_WORDS_SENT_ST_CLR_S  27
/** SOC_ETM_I2S2_EVT_RX_DONE_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear I2S2_evt_rx_done trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_I2S2_EVT_RX_DONE_ST_CLR    (BIT(28))
#define SOC_ETM_I2S2_EVT_RX_DONE_ST_CLR_M  (SOC_ETM_I2S2_EVT_RX_DONE_ST_CLR_V << SOC_ETM_I2S2_EVT_RX_DONE_ST_CLR_S)
#define SOC_ETM_I2S2_EVT_RX_DONE_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_EVT_RX_DONE_ST_CLR_S  28
/** SOC_ETM_I2S2_EVT_TX_DONE_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear I2S2_evt_tx_done trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_I2S2_EVT_TX_DONE_ST_CLR    (BIT(29))
#define SOC_ETM_I2S2_EVT_TX_DONE_ST_CLR_M  (SOC_ETM_I2S2_EVT_TX_DONE_ST_CLR_V << SOC_ETM_I2S2_EVT_TX_DONE_ST_CLR_S)
#define SOC_ETM_I2S2_EVT_TX_DONE_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_EVT_TX_DONE_ST_CLR_S  29
/** SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear I2S2_evt_x_words_received trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_CLR    (BIT(30))
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_CLR_M  (SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_CLR_V << SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_CLR_S)
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_EVT_X_WORDS_RECEIVED_ST_CLR_S  30
/** SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear I2S2_evt_x_words_sent trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_CLR    (BIT(31))
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_CLR_M  (SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_CLR_V << SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_CLR_S)
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_EVT_X_WORDS_SENT_ST_CLR_S  31

/** SOC_ETM_EVT_ST5_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST5_REG (DR_REG_SOC_ETM_BASE + 0x1d0)
/** SOC_ETM_ULP_EVT_ERR_INTR_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents ULP_evt_err_intr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ULP_EVT_ERR_INTR_ST    (BIT(0))
#define SOC_ETM_ULP_EVT_ERR_INTR_ST_M  (SOC_ETM_ULP_EVT_ERR_INTR_ST_V << SOC_ETM_ULP_EVT_ERR_INTR_ST_S)
#define SOC_ETM_ULP_EVT_ERR_INTR_ST_V  0x00000001U
#define SOC_ETM_ULP_EVT_ERR_INTR_ST_S  0
/** SOC_ETM_ULP_EVT_HALT_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents ULP_evt_halt trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ULP_EVT_HALT_ST    (BIT(1))
#define SOC_ETM_ULP_EVT_HALT_ST_M  (SOC_ETM_ULP_EVT_HALT_ST_V << SOC_ETM_ULP_EVT_HALT_ST_S)
#define SOC_ETM_ULP_EVT_HALT_ST_V  0x00000001U
#define SOC_ETM_ULP_EVT_HALT_ST_S  1
/** SOC_ETM_ULP_EVT_START_INTR_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents ULP_evt_start_intr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ULP_EVT_START_INTR_ST    (BIT(2))
#define SOC_ETM_ULP_EVT_START_INTR_ST_M  (SOC_ETM_ULP_EVT_START_INTR_ST_V << SOC_ETM_ULP_EVT_START_INTR_ST_S)
#define SOC_ETM_ULP_EVT_START_INTR_ST_V  0x00000001U
#define SOC_ETM_ULP_EVT_START_INTR_ST_S  2
/** SOC_ETM_RTC_EVT_TICK_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents RTC_evt_tick trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_RTC_EVT_TICK_ST    (BIT(3))
#define SOC_ETM_RTC_EVT_TICK_ST_M  (SOC_ETM_RTC_EVT_TICK_ST_V << SOC_ETM_RTC_EVT_TICK_ST_S)
#define SOC_ETM_RTC_EVT_TICK_ST_V  0x00000001U
#define SOC_ETM_RTC_EVT_TICK_ST_S  3
/** SOC_ETM_RTC_EVT_OVF_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents RTC_evt_ovf trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_RTC_EVT_OVF_ST    (BIT(4))
#define SOC_ETM_RTC_EVT_OVF_ST_M  (SOC_ETM_RTC_EVT_OVF_ST_V << SOC_ETM_RTC_EVT_OVF_ST_S)
#define SOC_ETM_RTC_EVT_OVF_ST_V  0x00000001U
#define SOC_ETM_RTC_EVT_OVF_ST_S  4
/** SOC_ETM_RTC_EVT_CMP_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents RTC_evt_cmp trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_RTC_EVT_CMP_ST    (BIT(5))
#define SOC_ETM_RTC_EVT_CMP_ST_M  (SOC_ETM_RTC_EVT_CMP_ST_V << SOC_ETM_RTC_EVT_CMP_ST_S)
#define SOC_ETM_RTC_EVT_CMP_ST_V  0x00000001U
#define SOC_ETM_RTC_EVT_CMP_ST_S  5
/** SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents PDMA_AHB_evt_in_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST    (BIT(6))
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_S  6
/** SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents PDMA_AHB_evt_in_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST    (BIT(7))
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_S  7
/** SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents PDMA_AHB_evt_in_done_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST    (BIT(8))
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_S  8
/** SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents PDMA_AHB_evt_in_suc_eof_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST    (BIT(9))
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_S  9
/** SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents PDMA_AHB_evt_in_suc_eof_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST    (BIT(10))
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_S  10
/** SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents PDMA_AHB_evt_in_suc_eof_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST    (BIT(11))
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_S  11
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents PDMA_AHB_evt_in_fifo_empty_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST    (BIT(12))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_S  12
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents PDMA_AHB_evt_in_fifo_empty_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST    (BIT(13))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_S  13
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents PDMA_AHB_evt_in_fifo_empty_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST    (BIT(14))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_S  14
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents PDMA_AHB_evt_in_fifo_full_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST    (BIT(15))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_S  15
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents PDMA_AHB_evt_in_fifo_full_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST    (BIT(16))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_S  16
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents PDMA_AHB_evt_in_fifo_full_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST    (BIT(17))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_S  17
/** SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents PDMA_AHB_evt_out_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST    (BIT(18))
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_S  18
/** SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents PDMA_AHB_evt_out_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST    (BIT(19))
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_S  19
/** SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents PDMA_AHB_evt_out_done_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST    (BIT(20))
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_S  20
/** SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents PDMA_AHB_evt_out_eof_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST    (BIT(21))
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_S  21
/** SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents PDMA_AHB_evt_out_eof_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST    (BIT(22))
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_S  22
/** SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents PDMA_AHB_evt_out_eof_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST    (BIT(23))
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_S  23
/** SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents PDMA_AHB_evt_out_total_eof_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST    (BIT(24))
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_S  24
/** SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents PDMA_AHB_evt_out_total_eof_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST    (BIT(25))
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_S  25
/** SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents PDMA_AHB_evt_out_total_eof_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST    (BIT(26))
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_S  26
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents PDMA_AHB_evt_out_fifo_empty_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST    (BIT(27))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_S  27
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents PDMA_AHB_evt_out_fifo_empty_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST    (BIT(28))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_S  28
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents PDMA_AHB_evt_out_fifo_empty_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST    (BIT(29))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_S  29
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents PDMA_AHB_evt_out_fifo_full_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST    (BIT(30))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_S  30
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents PDMA_AHB_evt_out_fifo_full_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST    (BIT(31))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_S  31

/** SOC_ETM_EVT_ST5_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST5_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1d4)
/** SOC_ETM_ULP_EVT_ERR_INTR_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear ULP_evt_err_intr trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ULP_EVT_ERR_INTR_ST_CLR    (BIT(0))
#define SOC_ETM_ULP_EVT_ERR_INTR_ST_CLR_M  (SOC_ETM_ULP_EVT_ERR_INTR_ST_CLR_V << SOC_ETM_ULP_EVT_ERR_INTR_ST_CLR_S)
#define SOC_ETM_ULP_EVT_ERR_INTR_ST_CLR_V  0x00000001U
#define SOC_ETM_ULP_EVT_ERR_INTR_ST_CLR_S  0
/** SOC_ETM_ULP_EVT_HALT_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear ULP_evt_halt trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ULP_EVT_HALT_ST_CLR    (BIT(1))
#define SOC_ETM_ULP_EVT_HALT_ST_CLR_M  (SOC_ETM_ULP_EVT_HALT_ST_CLR_V << SOC_ETM_ULP_EVT_HALT_ST_CLR_S)
#define SOC_ETM_ULP_EVT_HALT_ST_CLR_V  0x00000001U
#define SOC_ETM_ULP_EVT_HALT_ST_CLR_S  1
/** SOC_ETM_ULP_EVT_START_INTR_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear ULP_evt_start_intr trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_ULP_EVT_START_INTR_ST_CLR    (BIT(2))
#define SOC_ETM_ULP_EVT_START_INTR_ST_CLR_M  (SOC_ETM_ULP_EVT_START_INTR_ST_CLR_V << SOC_ETM_ULP_EVT_START_INTR_ST_CLR_S)
#define SOC_ETM_ULP_EVT_START_INTR_ST_CLR_V  0x00000001U
#define SOC_ETM_ULP_EVT_START_INTR_ST_CLR_S  2
/** SOC_ETM_RTC_EVT_TICK_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear RTC_evt_tick trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_RTC_EVT_TICK_ST_CLR    (BIT(3))
#define SOC_ETM_RTC_EVT_TICK_ST_CLR_M  (SOC_ETM_RTC_EVT_TICK_ST_CLR_V << SOC_ETM_RTC_EVT_TICK_ST_CLR_S)
#define SOC_ETM_RTC_EVT_TICK_ST_CLR_V  0x00000001U
#define SOC_ETM_RTC_EVT_TICK_ST_CLR_S  3
/** SOC_ETM_RTC_EVT_OVF_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear RTC_evt_ovf trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_RTC_EVT_OVF_ST_CLR    (BIT(4))
#define SOC_ETM_RTC_EVT_OVF_ST_CLR_M  (SOC_ETM_RTC_EVT_OVF_ST_CLR_V << SOC_ETM_RTC_EVT_OVF_ST_CLR_S)
#define SOC_ETM_RTC_EVT_OVF_ST_CLR_V  0x00000001U
#define SOC_ETM_RTC_EVT_OVF_ST_CLR_S  4
/** SOC_ETM_RTC_EVT_CMP_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear RTC_evt_cmp trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_RTC_EVT_CMP_ST_CLR    (BIT(5))
#define SOC_ETM_RTC_EVT_CMP_ST_CLR_M  (SOC_ETM_RTC_EVT_CMP_ST_CLR_V << SOC_ETM_RTC_EVT_CMP_ST_CLR_S)
#define SOC_ETM_RTC_EVT_CMP_ST_CLR_V  0x00000001U
#define SOC_ETM_RTC_EVT_CMP_ST_CLR_S  5
/** SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_done_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_CLR    (BIT(6))
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH0_ST_CLR_S  6
/** SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_done_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_CLR    (BIT(7))
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH1_ST_CLR_S  7
/** SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_done_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_CLR    (BIT(8))
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_DONE_CH2_ST_CLR_S  8
/** SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_suc_eof_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_CLR    (BIT(9))
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH0_ST_CLR_S  9
/** SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_suc_eof_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_CLR    (BIT(10))
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH1_ST_CLR_S  10
/** SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_suc_eof_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_CLR    (BIT(11))
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_SUC_EOF_CH2_ST_CLR_S  11
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_fifo_empty_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_CLR    (BIT(12))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_S  12
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_fifo_empty_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_CLR    (BIT(13))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_S  13
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_fifo_empty_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_CLR    (BIT(14))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_S  14
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_fifo_full_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_CLR    (BIT(15))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH0_ST_CLR_S  15
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_fifo_full_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_CLR    (BIT(16))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH1_ST_CLR_S  16
/** SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_in_fifo_full_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_CLR    (BIT(17))
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_IN_FIFO_FULL_CH2_ST_CLR_S  17
/** SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_done_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_CLR    (BIT(18))
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH0_ST_CLR_S  18
/** SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_done_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_CLR    (BIT(19))
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH1_ST_CLR_S  19
/** SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_done_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_CLR    (BIT(20))
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_DONE_CH2_ST_CLR_S  20
/** SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_eof_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_CLR    (BIT(21))
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH0_ST_CLR_S  21
/** SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_eof_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_CLR    (BIT(22))
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH1_ST_CLR_S  22
/** SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_eof_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_CLR    (BIT(23))
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_EOF_CH2_ST_CLR_S  23
/** SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_total_eof_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_CLR    (BIT(24))
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_S  24
/** SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_total_eof_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_CLR    (BIT(25))
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_S  25
/** SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_total_eof_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_CLR    (BIT(26))
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_S  26
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_fifo_empty_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR    (BIT(27))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_S  27
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_fifo_empty_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR    (BIT(28))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_S  28
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_fifo_empty_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR    (BIT(29))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_S  29
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_fifo_full_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_CLR    (BIT(30))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH0_ST_CLR_S  30
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_fifo_full_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_CLR    (BIT(31))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH1_ST_CLR_S  31

/** SOC_ETM_EVT_ST6_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST6_REG (DR_REG_SOC_ETM_BASE + 0x1d8)
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents PDMA_AHB_evt_out_fifo_full_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST    (BIT(0))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_S  0
/** SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents PDMA_AXI_evt_in_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST    (BIT(1))
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_S  1
/** SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents PDMA_AXI_evt_in_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST    (BIT(2))
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_S  2
/** SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents PDMA_AXI_evt_in_done_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST    (BIT(3))
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_S  3
/** SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents PDMA_AXI_evt_in_suc_eof_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST    (BIT(4))
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_S  4
/** SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents PDMA_AXI_evt_in_suc_eof_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST    (BIT(5))
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_S  5
/** SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents PDMA_AXI_evt_in_suc_eof_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST    (BIT(6))
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_S  6
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents PDMA_AXI_evt_in_fifo_empty_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST    (BIT(7))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_S  7
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents PDMA_AXI_evt_in_fifo_empty_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST    (BIT(8))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_S  8
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents PDMA_AXI_evt_in_fifo_empty_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST    (BIT(9))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_S  9
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents PDMA_AXI_evt_in_fifo_full_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST    (BIT(10))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_S  10
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents PDMA_AXI_evt_in_fifo_full_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST    (BIT(11))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_S  11
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents PDMA_AXI_evt_in_fifo_full_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST    (BIT(12))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_S  12
/** SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents PDMA_AXI_evt_out_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST    (BIT(13))
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_S  13
/** SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents PDMA_AXI_evt_out_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST    (BIT(14))
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_S  14
/** SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents PDMA_AXI_evt_out_done_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST    (BIT(15))
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_S  15
/** SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents PDMA_AXI_evt_out_eof_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST    (BIT(16))
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_S  16
/** SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents PDMA_AXI_evt_out_eof_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST    (BIT(17))
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_S  17
/** SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents PDMA_AXI_evt_out_eof_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST    (BIT(18))
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_S  18
/** SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents PDMA_AXI_evt_out_total_eof_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST    (BIT(19))
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_S  19
/** SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents PDMA_AXI_evt_out_total_eof_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST    (BIT(20))
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_S  20
/** SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents PDMA_AXI_evt_out_total_eof_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST    (BIT(21))
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_S  21
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents PDMA_AXI_evt_out_fifo_empty_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST    (BIT(22))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_S  22
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents PDMA_AXI_evt_out_fifo_empty_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST    (BIT(23))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_S  23
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents PDMA_AXI_evt_out_fifo_empty_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST    (BIT(24))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_S  24
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents PDMA_AXI_evt_out_fifo_full_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST    (BIT(25))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_S  25
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents PDMA_AXI_evt_out_fifo_full_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST    (BIT(26))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_S  26
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents PDMA_AXI_evt_out_fifo_full_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST    (BIT(27))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_S  27
/** SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents PMU_evt_sleep_weekup trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST    (BIT(28))
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_M  (SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_V << SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_S)
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_V  0x00000001U
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_S  28
/** SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents DMA2D_evt_in_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST    (BIT(29))
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_M  (SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_V << SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_S)
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_S  29
/** SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents DMA2D_evt_in_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST    (BIT(30))
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_M  (SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_V << SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_S)
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_S  30
/** SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents DMA2D_evt_in_suc_eof_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST    (BIT(31))
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_M  (SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_V << SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_S)
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_S  31

/** SOC_ETM_EVT_ST6_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST6_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1dc)
/** SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_evt_out_fifo_full_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_CLR    (BIT(0))
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_EVT_OUT_FIFO_FULL_CH2_ST_CLR_S  0
/** SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_done_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_CLR    (BIT(1))
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH0_ST_CLR_S  1
/** SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_done_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_CLR    (BIT(2))
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH1_ST_CLR_S  2
/** SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_done_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_CLR    (BIT(3))
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_DONE_CH2_ST_CLR_S  3
/** SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_suc_eof_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_CLR    (BIT(4))
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH0_ST_CLR_S  4
/** SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_suc_eof_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_CLR    (BIT(5))
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH1_ST_CLR_S  5
/** SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_suc_eof_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_CLR    (BIT(6))
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_SUC_EOF_CH2_ST_CLR_S  6
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_fifo_empty_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_CLR    (BIT(7))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH0_ST_CLR_S  7
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_fifo_empty_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_CLR    (BIT(8))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH1_ST_CLR_S  8
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_fifo_empty_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_CLR    (BIT(9))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_EMPTY_CH2_ST_CLR_S  9
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_fifo_full_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_CLR    (BIT(10))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH0_ST_CLR_S  10
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_fifo_full_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_CLR    (BIT(11))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH1_ST_CLR_S  11
/** SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_in_fifo_full_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_CLR    (BIT(12))
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_IN_FIFO_FULL_CH2_ST_CLR_S  12
/** SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_done_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_CLR    (BIT(13))
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH0_ST_CLR_S  13
/** SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_done_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_CLR    (BIT(14))
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH1_ST_CLR_S  14
/** SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_done_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_CLR    (BIT(15))
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_DONE_CH2_ST_CLR_S  15
/** SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_eof_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_CLR    (BIT(16))
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH0_ST_CLR_S  16
/** SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_eof_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_CLR    (BIT(17))
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH1_ST_CLR_S  17
/** SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_eof_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_CLR    (BIT(18))
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_EOF_CH2_ST_CLR_S  18
/** SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_total_eof_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_CLR    (BIT(19))
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_S  19
/** SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_total_eof_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_CLR    (BIT(20))
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_S  20
/** SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_total_eof_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_CLR    (BIT(21))
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_S  21
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_fifo_empty_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR    (BIT(22))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH0_ST_CLR_S  22
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_fifo_empty_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR    (BIT(23))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH1_ST_CLR_S  23
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_fifo_empty_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR    (BIT(24))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_EMPTY_CH2_ST_CLR_S  24
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_fifo_full_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_CLR    (BIT(25))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH0_ST_CLR_S  25
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_fifo_full_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_CLR    (BIT(26))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH1_ST_CLR_S  26
/** SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_evt_out_fifo_full_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_CLR    (BIT(27))
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_EVT_OUT_FIFO_FULL_CH2_ST_CLR_S  27
/** SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear PMU_evt_sleep_weekup trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_CLR    (BIT(28))
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_CLR_M  (SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_CLR_V << SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_CLR_S)
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_CLR_V  0x00000001U
#define SOC_ETM_PMU_EVT_SLEEP_WEEKUP_ST_CLR_S  28
/** SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_in_done_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_CLR    (BIT(29))
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_CLR_M  (SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_CLR_V << SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH0_ST_CLR_S  29
/** SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_in_done_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_CLR    (BIT(30))
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_CLR_M  (SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_CLR_V << SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_DONE_CH1_ST_CLR_S  30
/** SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_in_suc_eof_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_CLR    (BIT(31))
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_CLR_M  (SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_CLR_V << SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH0_ST_CLR_S  31

/** SOC_ETM_EVT_ST7_REG register
 *  Events trigger status register
 */
#define SOC_ETM_EVT_ST7_REG (DR_REG_SOC_ETM_BASE + 0x1e0)
/** SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents DMA2D_evt_in_suc_eof_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST    (BIT(0))
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_M  (SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_V << SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_S)
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_S  0
/** SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents DMA2D_evt_out_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST    (BIT(1))
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_M  (SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_V << SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_S  1
/** SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents DMA2D_evt_out_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST    (BIT(2))
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_M  (SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_V << SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_S  2
/** SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents DMA2D_evt_out_done_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST    (BIT(3))
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_M  (SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_V << SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_S  3
/** SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents DMA2D_evt_out_eof_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST    (BIT(4))
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_M  (SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_V << SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_S  4
/** SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents DMA2D_evt_out_eof_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST    (BIT(5))
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_M  (SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_V << SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_S  5
/** SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents DMA2D_evt_out_eof_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST    (BIT(6))
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_M  (SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_V << SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_S  6
/** SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents DMA2D_evt_out_total_eof_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST    (BIT(7))
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_M  (SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_V << SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_S  7
/** SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents DMA2D_evt_out_total_eof_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST    (BIT(8))
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_M  (SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_V << SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_S  8
/** SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents DMA2D_evt_out_total_eof_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST    (BIT(9))
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_M  (SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_V << SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_S)
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_S  9

/** SOC_ETM_EVT_ST7_CLR_REG register
 *  Events trigger status clear register
 */
#define SOC_ETM_EVT_ST7_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1e4)
/** SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_in_suc_eof_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_CLR    (BIT(0))
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_CLR_M  (SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_CLR_V << SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_IN_SUC_EOF_CH1_ST_CLR_S  0
/** SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_done_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_CLR    (BIT(1))
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH0_ST_CLR_S  1
/** SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_done_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_CLR    (BIT(2))
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH1_ST_CLR_S  2
/** SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_done_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_CLR    (BIT(3))
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_DONE_CH2_ST_CLR_S  3
/** SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_eof_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_CLR    (BIT(4))
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH0_ST_CLR_S  4
/** SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_eof_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_CLR    (BIT(5))
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH1_ST_CLR_S  5
/** SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_eof_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_CLR    (BIT(6))
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_EOF_CH2_ST_CLR_S  6
/** SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_total_eof_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_CLR    (BIT(7))
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH0_ST_CLR_S  7
/** SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_total_eof_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_CLR    (BIT(8))
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH1_ST_CLR_S  8
/** SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear DMA2D_evt_out_total_eof_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_CLR    (BIT(9))
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_M  (SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_V << SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_S)
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_EVT_OUT_TOTAL_EOF_CH2_ST_CLR_S  9

/** SOC_ETM_TASK_ST0_REG register
 *  Tasks trigger status register
 */
#define SOC_ETM_TASK_ST0_REG (DR_REG_SOC_ETM_BASE + 0x1e8)
/** SOC_ETM_GPIO_TASK_CH0_SET_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents GPIO_task_ch0_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH0_SET_ST    (BIT(0))
#define SOC_ETM_GPIO_TASK_CH0_SET_ST_M  (SOC_ETM_GPIO_TASK_CH0_SET_ST_V << SOC_ETM_GPIO_TASK_CH0_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH0_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH0_SET_ST_S  0
/** SOC_ETM_GPIO_TASK_CH1_SET_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents GPIO_task_ch1_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH1_SET_ST    (BIT(1))
#define SOC_ETM_GPIO_TASK_CH1_SET_ST_M  (SOC_ETM_GPIO_TASK_CH1_SET_ST_V << SOC_ETM_GPIO_TASK_CH1_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH1_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH1_SET_ST_S  1
/** SOC_ETM_GPIO_TASK_CH2_SET_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents GPIO_task_ch2_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH2_SET_ST    (BIT(2))
#define SOC_ETM_GPIO_TASK_CH2_SET_ST_M  (SOC_ETM_GPIO_TASK_CH2_SET_ST_V << SOC_ETM_GPIO_TASK_CH2_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH2_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH2_SET_ST_S  2
/** SOC_ETM_GPIO_TASK_CH3_SET_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents GPIO_task_ch3_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH3_SET_ST    (BIT(3))
#define SOC_ETM_GPIO_TASK_CH3_SET_ST_M  (SOC_ETM_GPIO_TASK_CH3_SET_ST_V << SOC_ETM_GPIO_TASK_CH3_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH3_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH3_SET_ST_S  3
/** SOC_ETM_GPIO_TASK_CH4_SET_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents GPIO_task_ch4_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH4_SET_ST    (BIT(4))
#define SOC_ETM_GPIO_TASK_CH4_SET_ST_M  (SOC_ETM_GPIO_TASK_CH4_SET_ST_V << SOC_ETM_GPIO_TASK_CH4_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH4_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH4_SET_ST_S  4
/** SOC_ETM_GPIO_TASK_CH5_SET_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents GPIO_task_ch5_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH5_SET_ST    (BIT(5))
#define SOC_ETM_GPIO_TASK_CH5_SET_ST_M  (SOC_ETM_GPIO_TASK_CH5_SET_ST_V << SOC_ETM_GPIO_TASK_CH5_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH5_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH5_SET_ST_S  5
/** SOC_ETM_GPIO_TASK_CH6_SET_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents GPIO_task_ch6_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH6_SET_ST    (BIT(6))
#define SOC_ETM_GPIO_TASK_CH6_SET_ST_M  (SOC_ETM_GPIO_TASK_CH6_SET_ST_V << SOC_ETM_GPIO_TASK_CH6_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH6_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH6_SET_ST_S  6
/** SOC_ETM_GPIO_TASK_CH7_SET_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents GPIO_task_ch7_set trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH7_SET_ST    (BIT(7))
#define SOC_ETM_GPIO_TASK_CH7_SET_ST_M  (SOC_ETM_GPIO_TASK_CH7_SET_ST_V << SOC_ETM_GPIO_TASK_CH7_SET_ST_S)
#define SOC_ETM_GPIO_TASK_CH7_SET_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH7_SET_ST_S  7
/** SOC_ETM_GPIO_TASK_CH0_CLEAR_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents GPIO_task_ch0_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST    (BIT(8))
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_S  8
/** SOC_ETM_GPIO_TASK_CH1_CLEAR_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents GPIO_task_ch1_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST    (BIT(9))
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_S  9
/** SOC_ETM_GPIO_TASK_CH2_CLEAR_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents GPIO_task_ch2_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST    (BIT(10))
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_S  10
/** SOC_ETM_GPIO_TASK_CH3_CLEAR_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents GPIO_task_ch3_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST    (BIT(11))
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_S  11
/** SOC_ETM_GPIO_TASK_CH4_CLEAR_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents GPIO_task_ch4_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST    (BIT(12))
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_S  12
/** SOC_ETM_GPIO_TASK_CH5_CLEAR_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents GPIO_task_ch5_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST    (BIT(13))
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_S  13
/** SOC_ETM_GPIO_TASK_CH6_CLEAR_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents GPIO_task_ch6_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST    (BIT(14))
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_S  14
/** SOC_ETM_GPIO_TASK_CH7_CLEAR_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents GPIO_task_ch7_clear trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST    (BIT(15))
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_M  (SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_V << SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_S)
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_S  15
/** SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents GPIO_task_ch0_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST    (BIT(16))
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_S  16
/** SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents GPIO_task_ch1_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST    (BIT(17))
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_S  17
/** SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents GPIO_task_ch2_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST    (BIT(18))
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_S  18
/** SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents GPIO_task_ch3_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST    (BIT(19))
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_S  19
/** SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents GPIO_task_ch4_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST    (BIT(20))
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_S  20
/** SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents GPIO_task_ch5_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST    (BIT(21))
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_S  21
/** SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents GPIO_task_ch6_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST    (BIT(22))
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_S  22
/** SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents GPIO_task_ch7_toggle trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST    (BIT(23))
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_M  (SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_V << SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_S)
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_S  23
/** SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents LEDC_task_timer0_res_update trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST    (BIT(24))
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_M  (SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_V << SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_S  24
/** SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents LEDC_task_timer1_res_update trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST    (BIT(25))
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_M  (SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_V << SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_S  25
/** SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents LEDC_task_timer2_res_update trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST    (BIT(26))
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_M  (SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_V << SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_S  26
/** SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents LEDC_task_timer3_res_update trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST    (BIT(27))
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_M  (SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_V << SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_S  27
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST    (BIT(28))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_S  28
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST    (BIT(29))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_S  29
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST    (BIT(30))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_S  30
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch3 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST    (BIT(31))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_S  31

/** SOC_ETM_TASK_ST0_CLR_REG register
 *  Tasks trigger status clear register
 */
#define SOC_ETM_TASK_ST0_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1ec)
/** SOC_ETM_GPIO_TASK_CH0_SET_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch0_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH0_SET_ST_CLR    (BIT(0))
#define SOC_ETM_GPIO_TASK_CH0_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH0_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH0_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH0_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH0_SET_ST_CLR_S  0
/** SOC_ETM_GPIO_TASK_CH1_SET_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch1_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH1_SET_ST_CLR    (BIT(1))
#define SOC_ETM_GPIO_TASK_CH1_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH1_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH1_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH1_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH1_SET_ST_CLR_S  1
/** SOC_ETM_GPIO_TASK_CH2_SET_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch2_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH2_SET_ST_CLR    (BIT(2))
#define SOC_ETM_GPIO_TASK_CH2_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH2_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH2_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH2_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH2_SET_ST_CLR_S  2
/** SOC_ETM_GPIO_TASK_CH3_SET_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch3_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH3_SET_ST_CLR    (BIT(3))
#define SOC_ETM_GPIO_TASK_CH3_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH3_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH3_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH3_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH3_SET_ST_CLR_S  3
/** SOC_ETM_GPIO_TASK_CH4_SET_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch4_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH4_SET_ST_CLR    (BIT(4))
#define SOC_ETM_GPIO_TASK_CH4_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH4_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH4_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH4_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH4_SET_ST_CLR_S  4
/** SOC_ETM_GPIO_TASK_CH5_SET_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch5_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH5_SET_ST_CLR    (BIT(5))
#define SOC_ETM_GPIO_TASK_CH5_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH5_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH5_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH5_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH5_SET_ST_CLR_S  5
/** SOC_ETM_GPIO_TASK_CH6_SET_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch6_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH6_SET_ST_CLR    (BIT(6))
#define SOC_ETM_GPIO_TASK_CH6_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH6_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH6_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH6_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH6_SET_ST_CLR_S  6
/** SOC_ETM_GPIO_TASK_CH7_SET_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch7_set trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH7_SET_ST_CLR    (BIT(7))
#define SOC_ETM_GPIO_TASK_CH7_SET_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH7_SET_ST_CLR_V << SOC_ETM_GPIO_TASK_CH7_SET_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH7_SET_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH7_SET_ST_CLR_S  7
/** SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch0_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_CLR    (BIT(8))
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH0_CLEAR_ST_CLR_S  8
/** SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch1_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_CLR    (BIT(9))
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH1_CLEAR_ST_CLR_S  9
/** SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch2_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_CLR    (BIT(10))
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH2_CLEAR_ST_CLR_S  10
/** SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch3_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_CLR    (BIT(11))
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH3_CLEAR_ST_CLR_S  11
/** SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch4_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_CLR    (BIT(12))
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH4_CLEAR_ST_CLR_S  12
/** SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch5_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_CLR    (BIT(13))
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH5_CLEAR_ST_CLR_S  13
/** SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch6_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_CLR    (BIT(14))
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH6_CLEAR_ST_CLR_S  14
/** SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch7_clear trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_CLR    (BIT(15))
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_CLR_V << SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH7_CLEAR_ST_CLR_S  15
/** SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch0_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_CLR    (BIT(16))
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH0_TOGGLE_ST_CLR_S  16
/** SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch1_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_CLR    (BIT(17))
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH1_TOGGLE_ST_CLR_S  17
/** SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch2_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_CLR    (BIT(18))
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH2_TOGGLE_ST_CLR_S  18
/** SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch3_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_CLR    (BIT(19))
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH3_TOGGLE_ST_CLR_S  19
/** SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch4_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_CLR    (BIT(20))
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH4_TOGGLE_ST_CLR_S  20
/** SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch5_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_CLR    (BIT(21))
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH5_TOGGLE_ST_CLR_S  21
/** SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch6_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_CLR    (BIT(22))
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH6_TOGGLE_ST_CLR_S  22
/** SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear GPIO_task_ch7_toggle trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_CLR    (BIT(23))
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_CLR_M  (SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_CLR_V << SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_CLR_S)
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_CLR_V  0x00000001U
#define SOC_ETM_GPIO_TASK_CH7_TOGGLE_ST_CLR_S  23
/** SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer0_res_update trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_CLR    (BIT(24))
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_RES_UPDATE_ST_CLR_S  24
/** SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer1_res_update trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_CLR    (BIT(25))
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_RES_UPDATE_ST_CLR_S  25
/** SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer2_res_update trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_CLR    (BIT(26))
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_RES_UPDATE_ST_CLR_S  26
/** SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer3_res_update trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_CLR    (BIT(27))
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_RES_UPDATE_ST_CLR_S  27
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_CLR    (BIT(28))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH0_ST_CLR_S  28
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_CLR    (BIT(29))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH1_ST_CLR_S  29
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_CLR    (BIT(30))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH2_ST_CLR_S  30
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch3 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_CLR    (BIT(31))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH3_ST_CLR_S  31

/** SOC_ETM_TASK_ST1_REG register
 *  Tasks trigger status register
 */
#define SOC_ETM_TASK_ST1_REG (DR_REG_SOC_ETM_BASE + 0x1f0)
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch4 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST    (BIT(0))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_S  0
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch5 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST    (BIT(1))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_S  1
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch6 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST    (BIT(2))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_S  2
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents LEDC_task_duty_scale_update_ch7 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST    (BIT(3))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_S  3
/** SOC_ETM_LEDC_TASK_TIMER0_CAP_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents LEDC_task_timer0_cap trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST    (BIT(4))
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_M  (SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_V << SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_S  4
/** SOC_ETM_LEDC_TASK_TIMER1_CAP_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents LEDC_task_timer1_cap trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST    (BIT(5))
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_M  (SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_V << SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_S  5
/** SOC_ETM_LEDC_TASK_TIMER2_CAP_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents LEDC_task_timer2_cap trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST    (BIT(6))
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_M  (SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_V << SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_S  6
/** SOC_ETM_LEDC_TASK_TIMER3_CAP_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents LEDC_task_timer3_cap trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST    (BIT(7))
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_M  (SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_V << SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_S  7
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST    (BIT(8))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_S  8
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST    (BIT(9))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_S  9
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST    (BIT(10))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_S  10
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST    (BIT(11))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_S  11
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch4 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST    (BIT(12))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_S  12
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch5 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST    (BIT(13))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_S  13
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch6 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST    (BIT(14))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_S  14
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents LEDC_task_sig_out_dis_ch7 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST    (BIT(15))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_S  15
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST    (BIT(16))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_S  16
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST    (BIT(17))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_S  17
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST    (BIT(18))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_S  18
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST    (BIT(19))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_S  19
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch4 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST    (BIT(20))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_S  20
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch5 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST    (BIT(21))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_S  21
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch6 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST    (BIT(22))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_S  22
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents LEDC_task_ovf_cnt_rst_ch7 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST    (BIT(23))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_S  23
/** SOC_ETM_LEDC_TASK_TIMER0_RST_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents LEDC_task_timer0_rst trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST    (BIT(24))
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST_M  (SOC_ETM_LEDC_TASK_TIMER0_RST_ST_V << SOC_ETM_LEDC_TASK_TIMER0_RST_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST_S  24
/** SOC_ETM_LEDC_TASK_TIMER1_RST_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents LEDC_task_timer1_rst trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST    (BIT(25))
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST_M  (SOC_ETM_LEDC_TASK_TIMER1_RST_ST_V << SOC_ETM_LEDC_TASK_TIMER1_RST_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST_S  25
/** SOC_ETM_LEDC_TASK_TIMER2_RST_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents LEDC_task_timer2_rst trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST    (BIT(26))
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST_M  (SOC_ETM_LEDC_TASK_TIMER2_RST_ST_V << SOC_ETM_LEDC_TASK_TIMER2_RST_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST_S  26
/** SOC_ETM_LEDC_TASK_TIMER3_RST_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents LEDC_task_timer3_rst trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST    (BIT(27))
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST_M  (SOC_ETM_LEDC_TASK_TIMER3_RST_ST_V << SOC_ETM_LEDC_TASK_TIMER3_RST_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST_S  27
/** SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents LEDC_task_timer0_resume trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST    (BIT(28))
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_M  (SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_V << SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_S  28
/** SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents LEDC_task_timer1_resume trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST    (BIT(29))
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_M  (SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_V << SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_S  29
/** SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents LEDC_task_timer2_resume trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST    (BIT(30))
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_M  (SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_V << SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_S  30
/** SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents LEDC_task_timer3_resume trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST    (BIT(31))
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_M  (SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_V << SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_S  31

/** SOC_ETM_TASK_ST1_CLR_REG register
 *  Tasks trigger status clear register
 */
#define SOC_ETM_TASK_ST1_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1f4)
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch4 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_CLR    (BIT(0))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH4_ST_CLR_S  0
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch5 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_CLR    (BIT(1))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH5_ST_CLR_S  1
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch6 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_CLR    (BIT(2))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH6_ST_CLR_S  2
/** SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear LEDC_task_duty_scale_update_ch7 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_CLR    (BIT(3))
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_CLR_M  (SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_CLR_V << SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_DUTY_SCALE_UPDATE_CH7_ST_CLR_S  3
/** SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer0_cap trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_CLR    (BIT(4))
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_CAP_ST_CLR_S  4
/** SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer1_cap trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_CLR    (BIT(5))
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_CAP_ST_CLR_S  5
/** SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer2_cap trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_CLR    (BIT(6))
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_CAP_ST_CLR_S  6
/** SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer3_cap trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_CLR    (BIT(7))
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_CAP_ST_CLR_S  7
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_CLR    (BIT(8))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH0_ST_CLR_S  8
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_CLR    (BIT(9))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH1_ST_CLR_S  9
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_CLR    (BIT(10))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH2_ST_CLR_S  10
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_CLR    (BIT(11))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH3_ST_CLR_S  11
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch4 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_CLR    (BIT(12))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH4_ST_CLR_S  12
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch5 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_CLR    (BIT(13))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH5_ST_CLR_S  13
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch6 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_CLR    (BIT(14))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH6_ST_CLR_S  14
/** SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear LEDC_task_sig_out_dis_ch7 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_CLR    (BIT(15))
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_CLR_M  (SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_CLR_V << SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_SIG_OUT_DIS_CH7_ST_CLR_S  15
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_CLR    (BIT(16))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH0_ST_CLR_S  16
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_CLR    (BIT(17))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH1_ST_CLR_S  17
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_CLR    (BIT(18))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH2_ST_CLR_S  18
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_CLR    (BIT(19))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH3_ST_CLR_S  19
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch4 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_CLR    (BIT(20))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH4_ST_CLR_S  20
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch5 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_CLR    (BIT(21))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH5_ST_CLR_S  21
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch6 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_CLR    (BIT(22))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH6_ST_CLR_S  22
/** SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch7 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_CLR    (BIT(23))
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_CLR_M  (SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_CLR_V << SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_OVF_CNT_RST_CH7_ST_CLR_S  23
/** SOC_ETM_LEDC_TASK_TIMER0_RST_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer0_rst trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST_CLR    (BIT(24))
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER0_RST_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER0_RST_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_RST_ST_CLR_S  24
/** SOC_ETM_LEDC_TASK_TIMER1_RST_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer1_rst trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST_CLR    (BIT(25))
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER1_RST_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER1_RST_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_RST_ST_CLR_S  25
/** SOC_ETM_LEDC_TASK_TIMER2_RST_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer2_rst trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST_CLR    (BIT(26))
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER2_RST_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER2_RST_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_RST_ST_CLR_S  26
/** SOC_ETM_LEDC_TASK_TIMER3_RST_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer3_rst trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST_CLR    (BIT(27))
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER3_RST_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER3_RST_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_RST_ST_CLR_S  27
/** SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer0_resume trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_CLR    (BIT(28))
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_RESUME_ST_CLR_S  28
/** SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer1_resume trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_CLR    (BIT(29))
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_RESUME_ST_CLR_S  29
/** SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer2_resume trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_CLR    (BIT(30))
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_RESUME_ST_CLR_S  30
/** SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer3_resume trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_CLR    (BIT(31))
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_RESUME_ST_CLR_S  31

/** SOC_ETM_TASK_ST2_REG register
 *  Tasks trigger status register
 */
#define SOC_ETM_TASK_ST2_REG (DR_REG_SOC_ETM_BASE + 0x1f8)
/** SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents LEDC_task_timer0_pause trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST    (BIT(0))
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_M  (SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_V << SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_S  0
/** SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents LEDC_task_timer1_pause trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST    (BIT(1))
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_M  (SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_V << SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_S  1
/** SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents LEDC_task_timer2_pause trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST    (BIT(2))
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_M  (SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_V << SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_S  2
/** SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents LEDC_task_timer3_pause trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST    (BIT(3))
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_M  (SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_V << SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_S)
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_S  3
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST    (BIT(4))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_S  4
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST    (BIT(5))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_S  5
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST    (BIT(6))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_S  6
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch3 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST    (BIT(7))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_S  7
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch4 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST    (BIT(8))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_S  8
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch5 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST    (BIT(9))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_S  9
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch6 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST    (BIT(10))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_S  10
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents LEDC_task_gamma_restart_ch7 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST    (BIT(11))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_S  11
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST    (BIT(12))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_S  12
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST    (BIT(13))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_S  13
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST    (BIT(14))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_S  14
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST    (BIT(15))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_S  15
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch4 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST    (BIT(16))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_S  16
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch5 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST    (BIT(17))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_S  17
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch6 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST    (BIT(18))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_S  18
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents LEDC_task_gamma_pause_ch7 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST    (BIT(19))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_S  19
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST    (BIT(20))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_S  20
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST    (BIT(21))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_S  21
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST    (BIT(22))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_S  22
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch3 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST    (BIT(23))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_S  23
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch4 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST    (BIT(24))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_S  24
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch5 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST    (BIT(25))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_S  25
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch6 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST    (BIT(26))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_S  26
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents LEDC_task_gamma_resume_ch7 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST    (BIT(27))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_S  27
/** SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents TG0_task_cnt_start_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST    (BIT(28))
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_M  (SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_V << SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_S)
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_S  28
/** SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents TG0_task_alarm_start_timer0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST    (BIT(29))
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_M  (SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_V << SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_S)
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_S  29
/** SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents TG0_task_cnt_stop_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST    (BIT(30))
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_M  (SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_V << SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_S)
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_S  30
/** SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents TG0_task_cnt_reload_timer0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST    (BIT(31))
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_M  (SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_V << SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_S)
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_S  31

/** SOC_ETM_TASK_ST2_CLR_REG register
 *  Tasks trigger status clear register
 */
#define SOC_ETM_TASK_ST2_CLR_REG (DR_REG_SOC_ETM_BASE + 0x1fc)
/** SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer0_pause trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_CLR    (BIT(0))
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER0_PAUSE_ST_CLR_S  0
/** SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer1_pause trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_CLR    (BIT(1))
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER1_PAUSE_ST_CLR_S  1
/** SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer2_pause trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_CLR    (BIT(2))
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER2_PAUSE_ST_CLR_S  2
/** SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear LEDC_task_timer3_pause trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_CLR    (BIT(3))
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_CLR_M  (SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_CLR_V << SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_TIMER3_PAUSE_ST_CLR_S  3
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_CLR    (BIT(4))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH0_ST_CLR_S  4
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_CLR    (BIT(5))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH1_ST_CLR_S  5
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_CLR    (BIT(6))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH2_ST_CLR_S  6
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_CLR    (BIT(7))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH3_ST_CLR_S  7
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch4 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_CLR    (BIT(8))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH4_ST_CLR_S  8
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch5 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_CLR    (BIT(9))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH5_ST_CLR_S  9
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch6 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_CLR    (BIT(10))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH6_ST_CLR_S  10
/** SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_restart_ch7 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_CLR    (BIT(11))
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESTART_CH7_ST_CLR_S  11
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_CLR    (BIT(12))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH0_ST_CLR_S  12
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_CLR    (BIT(13))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH1_ST_CLR_S  13
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_CLR    (BIT(14))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH2_ST_CLR_S  14
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_CLR    (BIT(15))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH3_ST_CLR_S  15
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch4 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_CLR    (BIT(16))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH4_ST_CLR_S  16
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch5 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_CLR    (BIT(17))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH5_ST_CLR_S  17
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch6 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_CLR    (BIT(18))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH6_ST_CLR_S  18
/** SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_pause_ch7 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_CLR    (BIT(19))
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_PAUSE_CH7_ST_CLR_S  19
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_CLR    (BIT(20))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH0_ST_CLR_S  20
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_CLR    (BIT(21))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH1_ST_CLR_S  21
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_CLR    (BIT(22))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH2_ST_CLR_S  22
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch3 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_CLR    (BIT(23))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH3_ST_CLR_S  23
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch4 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_CLR    (BIT(24))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH4_ST_CLR_S  24
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch5 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_CLR    (BIT(25))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH5_ST_CLR_S  25
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch6 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_CLR    (BIT(26))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH6_ST_CLR_S  26
/** SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear LEDC_task_gamma_resume_ch7 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_CLR    (BIT(27))
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_CLR_M  (SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_CLR_V << SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_CLR_S)
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_CLR_V  0x00000001U
#define SOC_ETM_LEDC_TASK_GAMMA_RESUME_CH7_ST_CLR_S  27
/** SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_start_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_CLR    (BIT(28))
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_START_TIMER0_ST_CLR_S  28
/** SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear TG0_task_alarm_start_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_CLR    (BIT(29))
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_CLR_M  (SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_CLR_V << SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_CLR_S)
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER0_ST_CLR_S  29
/** SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_stop_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_CLR    (BIT(30))
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER0_ST_CLR_S  30
/** SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_reload_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_CLR    (BIT(31))
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER0_ST_CLR_S  31

/** SOC_ETM_TASK_ST3_REG register
 *  Tasks trigger status register
 */
#define SOC_ETM_TASK_ST3_REG (DR_REG_SOC_ETM_BASE + 0x200)
/** SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents TG0_task_cnt_cap_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST    (BIT(0))
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_M  (SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_V << SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_S)
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_S  0
/** SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents TG0_task_cnt_start_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST    (BIT(1))
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_M  (SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_V << SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_S)
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_S  1
/** SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents TG0_task_alarm_start_timer1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST    (BIT(2))
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_M  (SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_V << SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_S)
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_S  2
/** SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents TG0_task_cnt_stop_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST    (BIT(3))
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_M  (SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_V << SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_S)
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_S  3
/** SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents TG0_task_cnt_reload_timer1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST    (BIT(4))
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_M  (SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_V << SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_S)
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_S  4
/** SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents TG0_task_cnt_cap_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST    (BIT(5))
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_M  (SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_V << SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_S)
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_S  5
/** SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents TG1_task_cnt_start_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST    (BIT(6))
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_M  (SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_V << SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_S)
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_S  6
/** SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents TG1_task_alarm_start_timer0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST    (BIT(7))
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_M  (SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_V << SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_S)
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_S  7
/** SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents TG1_task_cnt_stop_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST    (BIT(8))
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_M  (SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_V << SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_S)
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_S  8
/** SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents TG1_task_cnt_reload_timer0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST    (BIT(9))
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_M  (SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_V << SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_S)
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_S  9
/** SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents TG1_task_cnt_cap_timer0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST    (BIT(10))
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_M  (SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_V << SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_S)
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_S  10
/** SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents TG1_task_cnt_start_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST    (BIT(11))
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_M  (SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_V << SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_S)
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_S  11
/** SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents TG1_task_alarm_start_timer1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST    (BIT(12))
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_M  (SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_V << SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_S)
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_S  12
/** SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents TG1_task_cnt_stop_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST    (BIT(13))
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_M  (SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_V << SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_S)
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_S  13
/** SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents TG1_task_cnt_reload_timer1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST    (BIT(14))
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_M  (SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_V << SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_S)
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_S  14
/** SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents TG1_task_cnt_cap_timer1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST    (BIT(15))
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_M  (SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_V << SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_S)
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_S  15
/** SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents MCPWM0_task_cmpr0_a_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST    (BIT(16))
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_M  (SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_V << SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_S  16
/** SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents MCPWM0_task_cmpr1_a_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST    (BIT(17))
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_M  (SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_V << SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_S  17
/** SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents MCPWM0_task_cmpr2_a_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST    (BIT(18))
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_M  (SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_V << SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_S  18
/** SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents MCPWM0_task_cmpr0_b_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST    (BIT(19))
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_M  (SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_V << SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_S  19
/** SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents MCPWM0_task_cmpr1_b_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST    (BIT(20))
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_M  (SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_V << SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_S  20
/** SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents MCPWM0_task_cmpr2_b_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST    (BIT(21))
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_M  (SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_V << SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_S  21
/** SOC_ETM_MCPWM0_TASK_GEN_STOP_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents MCPWM0_task_gen_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST    (BIT(22))
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_M  (SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_V << SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_S)
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_S  22
/** SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents MCPWM0_task_timer0_syn trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST    (BIT(23))
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_M  (SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_V << SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_S)
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_S  23
/** SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents MCPWM0_task_timer1_syn trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST    (BIT(24))
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_M  (SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_V << SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_S)
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_S  24
/** SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents MCPWM0_task_timer2_syn trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST    (BIT(25))
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_M  (SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_V << SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_S)
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_S  25
/** SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents MCPWM0_task_timer0_period_up trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST    (BIT(26))
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_M  (SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_V << SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_S  26
/** SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents MCPWM0_task_timer1_period_up trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST    (BIT(27))
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_M  (SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_V << SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_S  27
/** SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents MCPWM0_task_timer2_period_up trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST    (BIT(28))
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_M  (SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_V << SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_S)
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_S  28
/** SOC_ETM_MCPWM0_TASK_TZ0_OST_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents MCPWM0_task_tz0_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST    (BIT(29))
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_M  (SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_V << SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_S)
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_S  29
/** SOC_ETM_MCPWM0_TASK_TZ1_OST_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents MCPWM0_task_tz1_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST    (BIT(30))
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_M  (SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_V << SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_S)
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_S  30
/** SOC_ETM_MCPWM0_TASK_TZ2_OST_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents MCPWM0_task_tz2_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST    (BIT(31))
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_M  (SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_V << SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_S)
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_S  31

/** SOC_ETM_TASK_ST3_CLR_REG register
 *  Tasks trigger status clear register
 */
#define SOC_ETM_TASK_ST3_CLR_REG (DR_REG_SOC_ETM_BASE + 0x204)
/** SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_cap_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_CLR    (BIT(0))
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER0_ST_CLR_S  0
/** SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_start_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_CLR    (BIT(1))
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_START_TIMER1_ST_CLR_S  1
/** SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear TG0_task_alarm_start_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_CLR    (BIT(2))
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_CLR_M  (SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_CLR_V << SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_CLR_S)
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_ALARM_START_TIMER1_ST_CLR_S  2
/** SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_stop_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_CLR    (BIT(3))
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_STOP_TIMER1_ST_CLR_S  3
/** SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_reload_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_CLR    (BIT(4))
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_RELOAD_TIMER1_ST_CLR_S  4
/** SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear TG0_task_cnt_cap_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_CLR    (BIT(5))
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_CLR_M  (SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_CLR_V << SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_CLR_S)
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG0_TASK_CNT_CAP_TIMER1_ST_CLR_S  5
/** SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_start_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_CLR    (BIT(6))
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_START_TIMER0_ST_CLR_S  6
/** SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear TG1_task_alarm_start_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_CLR    (BIT(7))
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_CLR_M  (SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_CLR_V << SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_CLR_S)
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER0_ST_CLR_S  7
/** SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_stop_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_CLR    (BIT(8))
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER0_ST_CLR_S  8
/** SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_reload_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_CLR    (BIT(9))
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER0_ST_CLR_S  9
/** SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_cap_timer0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_CLR    (BIT(10))
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER0_ST_CLR_S  10
/** SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_start_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_CLR    (BIT(11))
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_START_TIMER1_ST_CLR_S  11
/** SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear TG1_task_alarm_start_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_CLR    (BIT(12))
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_CLR_M  (SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_CLR_V << SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_CLR_S)
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_ALARM_START_TIMER1_ST_CLR_S  12
/** SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_stop_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_CLR    (BIT(13))
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_STOP_TIMER1_ST_CLR_S  13
/** SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_reload_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_CLR    (BIT(14))
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_RELOAD_TIMER1_ST_CLR_S  14
/** SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear TG1_task_cnt_cap_timer1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_CLR    (BIT(15))
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_CLR_M  (SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_CLR_V << SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_CLR_S)
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_CLR_V  0x00000001U
#define SOC_ETM_TG1_TASK_CNT_CAP_TIMER1_ST_CLR_S  15
/** SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cmpr0_a_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_CLR    (BIT(16))
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR0_A_UP_ST_CLR_S  16
/** SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cmpr1_a_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_CLR    (BIT(17))
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR1_A_UP_ST_CLR_S  17
/** SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cmpr2_a_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_CLR    (BIT(18))
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR2_A_UP_ST_CLR_S  18
/** SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cmpr0_b_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_CLR    (BIT(19))
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR0_B_UP_ST_CLR_S  19
/** SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cmpr1_b_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_CLR    (BIT(20))
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR1_B_UP_ST_CLR_S  20
/** SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cmpr2_b_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_CLR    (BIT(21))
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CMPR2_B_UP_ST_CLR_S  21
/** SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_gen_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_CLR    (BIT(22))
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_GEN_STOP_ST_CLR_S  22
/** SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_timer0_syn trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_CLR    (BIT(23))
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER0_SYN_ST_CLR_S  23
/** SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_timer1_syn trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_CLR    (BIT(24))
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER1_SYN_ST_CLR_S  24
/** SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_timer2_syn trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_CLR    (BIT(25))
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER2_SYN_ST_CLR_S  25
/** SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_timer0_period_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_CLR    (BIT(26))
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER0_PERIOD_UP_ST_CLR_S  26
/** SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_timer1_period_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_CLR    (BIT(27))
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER1_PERIOD_UP_ST_CLR_S  27
/** SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_timer2_period_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_CLR    (BIT(28))
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TIMER2_PERIOD_UP_ST_CLR_S  28
/** SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_tz0_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_CLR    (BIT(29))
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TZ0_OST_ST_CLR_S  29
/** SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_tz1_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_CLR    (BIT(30))
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TZ1_OST_ST_CLR_S  30
/** SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_tz2_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_CLR    (BIT(31))
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_CLR_V << SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_TZ2_OST_ST_CLR_S  31

/** SOC_ETM_TASK_ST4_REG register
 *  Tasks trigger status register
 */
#define SOC_ETM_TASK_ST4_REG (DR_REG_SOC_ETM_BASE + 0x208)
/** SOC_ETM_MCPWM0_TASK_CLR0_OST_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents MCPWM0_task_clr0_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST    (BIT(0))
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_M  (SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_V << SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_S)
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_S  0
/** SOC_ETM_MCPWM0_TASK_CLR1_OST_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents MCPWM0_task_clr1_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST    (BIT(1))
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_M  (SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_V << SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_S)
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_S  1
/** SOC_ETM_MCPWM0_TASK_CLR2_OST_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents MCPWM0_task_clr2_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST    (BIT(2))
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_M  (SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_V << SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_S)
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_S  2
/** SOC_ETM_MCPWM0_TASK_CAP0_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents MCPWM0_task_cap0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CAP0_ST    (BIT(3))
#define SOC_ETM_MCPWM0_TASK_CAP0_ST_M  (SOC_ETM_MCPWM0_TASK_CAP0_ST_V << SOC_ETM_MCPWM0_TASK_CAP0_ST_S)
#define SOC_ETM_MCPWM0_TASK_CAP0_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CAP0_ST_S  3
/** SOC_ETM_MCPWM0_TASK_CAP1_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents MCPWM0_task_cap1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CAP1_ST    (BIT(4))
#define SOC_ETM_MCPWM0_TASK_CAP1_ST_M  (SOC_ETM_MCPWM0_TASK_CAP1_ST_V << SOC_ETM_MCPWM0_TASK_CAP1_ST_S)
#define SOC_ETM_MCPWM0_TASK_CAP1_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CAP1_ST_S  4
/** SOC_ETM_MCPWM0_TASK_CAP2_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents MCPWM0_task_cap2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM0_TASK_CAP2_ST    (BIT(5))
#define SOC_ETM_MCPWM0_TASK_CAP2_ST_M  (SOC_ETM_MCPWM0_TASK_CAP2_ST_V << SOC_ETM_MCPWM0_TASK_CAP2_ST_S)
#define SOC_ETM_MCPWM0_TASK_CAP2_ST_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CAP2_ST_S  5
/** SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents MCPWM1_task_cmpr0_a_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST    (BIT(6))
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_M  (SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_V << SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_S  6
/** SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents MCPWM1_task_cmpr1_a_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST    (BIT(7))
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_M  (SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_V << SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_S  7
/** SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents MCPWM1_task_cmpr2_a_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST    (BIT(8))
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_M  (SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_V << SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_S  8
/** SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents MCPWM1_task_cmpr0_b_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST    (BIT(9))
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_M  (SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_V << SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_S  9
/** SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents MCPWM1_task_cmpr1_b_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST    (BIT(10))
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_M  (SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_V << SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_S  10
/** SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents MCPWM1_task_cmpr2_b_up trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST    (BIT(11))
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_M  (SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_V << SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_S  11
/** SOC_ETM_MCPWM1_TASK_GEN_STOP_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents MCPWM1_task_gen_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST    (BIT(12))
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_M  (SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_V << SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_S)
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_S  12
/** SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents MCPWM1_task_timer0_syn trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST    (BIT(13))
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_M  (SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_V << SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_S)
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_S  13
/** SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents MCPWM1_task_timer1_syn trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST    (BIT(14))
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_M  (SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_V << SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_S)
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_S  14
/** SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents MCPWM1_task_timer2_syn trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST    (BIT(15))
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_M  (SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_V << SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_S)
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_S  15
/** SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents MCPWM1_task_timer0_period_up trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST    (BIT(16))
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_M  (SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_V << SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_S  16
/** SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents MCPWM1_task_timer1_period_up trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST    (BIT(17))
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_M  (SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_V << SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_S  17
/** SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents MCPWM1_task_timer2_period_up trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST    (BIT(18))
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_M  (SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_V << SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_S)
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_S  18
/** SOC_ETM_MCPWM1_TASK_TZ0_OST_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents MCPWM1_task_tz0_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST    (BIT(19))
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_M  (SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_V << SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_S)
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_S  19
/** SOC_ETM_MCPWM1_TASK_TZ1_OST_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents MCPWM1_task_tz1_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST    (BIT(20))
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_M  (SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_V << SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_S)
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_S  20
/** SOC_ETM_MCPWM1_TASK_TZ2_OST_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents MCPWM1_task_tz2_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST    (BIT(21))
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_M  (SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_V << SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_S)
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_S  21
/** SOC_ETM_MCPWM1_TASK_CLR0_OST_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents MCPWM1_task_clr0_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST    (BIT(22))
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_M  (SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_V << SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_S)
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_S  22
/** SOC_ETM_MCPWM1_TASK_CLR1_OST_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents MCPWM1_task_clr1_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST    (BIT(23))
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_M  (SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_V << SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_S)
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_S  23
/** SOC_ETM_MCPWM1_TASK_CLR2_OST_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents MCPWM1_task_clr2_ost trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST    (BIT(24))
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_M  (SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_V << SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_S)
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_S  24
/** SOC_ETM_MCPWM1_TASK_CAP0_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents MCPWM1_task_cap0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CAP0_ST    (BIT(25))
#define SOC_ETM_MCPWM1_TASK_CAP0_ST_M  (SOC_ETM_MCPWM1_TASK_CAP0_ST_V << SOC_ETM_MCPWM1_TASK_CAP0_ST_S)
#define SOC_ETM_MCPWM1_TASK_CAP0_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CAP0_ST_S  25
/** SOC_ETM_MCPWM1_TASK_CAP1_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents MCPWM1_task_cap1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CAP1_ST    (BIT(26))
#define SOC_ETM_MCPWM1_TASK_CAP1_ST_M  (SOC_ETM_MCPWM1_TASK_CAP1_ST_V << SOC_ETM_MCPWM1_TASK_CAP1_ST_S)
#define SOC_ETM_MCPWM1_TASK_CAP1_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CAP1_ST_S  26
/** SOC_ETM_MCPWM1_TASK_CAP2_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents MCPWM1_task_cap2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_MCPWM1_TASK_CAP2_ST    (BIT(27))
#define SOC_ETM_MCPWM1_TASK_CAP2_ST_M  (SOC_ETM_MCPWM1_TASK_CAP2_ST_V << SOC_ETM_MCPWM1_TASK_CAP2_ST_S)
#define SOC_ETM_MCPWM1_TASK_CAP2_ST_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CAP2_ST_S  27
/** SOC_ETM_ADC_TASK_SAMPLE0_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents ADC_task_sample0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_TASK_SAMPLE0_ST    (BIT(28))
#define SOC_ETM_ADC_TASK_SAMPLE0_ST_M  (SOC_ETM_ADC_TASK_SAMPLE0_ST_V << SOC_ETM_ADC_TASK_SAMPLE0_ST_S)
#define SOC_ETM_ADC_TASK_SAMPLE0_ST_V  0x00000001U
#define SOC_ETM_ADC_TASK_SAMPLE0_ST_S  28
/** SOC_ETM_ADC_TASK_SAMPLE1_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents ADC_task_sample1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_TASK_SAMPLE1_ST    (BIT(29))
#define SOC_ETM_ADC_TASK_SAMPLE1_ST_M  (SOC_ETM_ADC_TASK_SAMPLE1_ST_V << SOC_ETM_ADC_TASK_SAMPLE1_ST_S)
#define SOC_ETM_ADC_TASK_SAMPLE1_ST_V  0x00000001U
#define SOC_ETM_ADC_TASK_SAMPLE1_ST_S  29
/** SOC_ETM_ADC_TASK_START0_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents ADC_task_start0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_TASK_START0_ST    (BIT(30))
#define SOC_ETM_ADC_TASK_START0_ST_M  (SOC_ETM_ADC_TASK_START0_ST_V << SOC_ETM_ADC_TASK_START0_ST_S)
#define SOC_ETM_ADC_TASK_START0_ST_V  0x00000001U
#define SOC_ETM_ADC_TASK_START0_ST_S  30
/** SOC_ETM_ADC_TASK_STOP0_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents ADC_task_stop0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ADC_TASK_STOP0_ST    (BIT(31))
#define SOC_ETM_ADC_TASK_STOP0_ST_M  (SOC_ETM_ADC_TASK_STOP0_ST_V << SOC_ETM_ADC_TASK_STOP0_ST_S)
#define SOC_ETM_ADC_TASK_STOP0_ST_V  0x00000001U
#define SOC_ETM_ADC_TASK_STOP0_ST_S  31

/** SOC_ETM_TASK_ST4_CLR_REG register
 *  Tasks trigger status clear register
 */
#define SOC_ETM_TASK_ST4_CLR_REG (DR_REG_SOC_ETM_BASE + 0x20c)
/** SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_clr0_ost trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_CLR    (BIT(0))
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CLR0_OST_ST_CLR_S  0
/** SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_clr1_ost trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_CLR    (BIT(1))
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CLR1_OST_ST_CLR_S  1
/** SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_clr2_ost trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_CLR    (BIT(2))
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CLR2_OST_ST_CLR_S  2
/** SOC_ETM_MCPWM0_TASK_CAP0_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cap0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CAP0_ST_CLR    (BIT(3))
#define SOC_ETM_MCPWM0_TASK_CAP0_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CAP0_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CAP0_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CAP0_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CAP0_ST_CLR_S  3
/** SOC_ETM_MCPWM0_TASK_CAP1_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cap1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CAP1_ST_CLR    (BIT(4))
#define SOC_ETM_MCPWM0_TASK_CAP1_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CAP1_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CAP1_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CAP1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CAP1_ST_CLR_S  4
/** SOC_ETM_MCPWM0_TASK_CAP2_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear MCPWM0_task_cap2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM0_TASK_CAP2_ST_CLR    (BIT(5))
#define SOC_ETM_MCPWM0_TASK_CAP2_ST_CLR_M  (SOC_ETM_MCPWM0_TASK_CAP2_ST_CLR_V << SOC_ETM_MCPWM0_TASK_CAP2_ST_CLR_S)
#define SOC_ETM_MCPWM0_TASK_CAP2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM0_TASK_CAP2_ST_CLR_S  5
/** SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cmpr0_a_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_CLR    (BIT(6))
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR0_A_UP_ST_CLR_S  6
/** SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cmpr1_a_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_CLR    (BIT(7))
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR1_A_UP_ST_CLR_S  7
/** SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cmpr2_a_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_CLR    (BIT(8))
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR2_A_UP_ST_CLR_S  8
/** SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cmpr0_b_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_CLR    (BIT(9))
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR0_B_UP_ST_CLR_S  9
/** SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cmpr1_b_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_CLR    (BIT(10))
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR1_B_UP_ST_CLR_S  10
/** SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cmpr2_b_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_CLR    (BIT(11))
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CMPR2_B_UP_ST_CLR_S  11
/** SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_gen_stop trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_CLR    (BIT(12))
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_GEN_STOP_ST_CLR_S  12
/** SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_timer0_syn trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_CLR    (BIT(13))
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER0_SYN_ST_CLR_S  13
/** SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_timer1_syn trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_CLR    (BIT(14))
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER1_SYN_ST_CLR_S  14
/** SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_timer2_syn trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_CLR    (BIT(15))
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER2_SYN_ST_CLR_S  15
/** SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_timer0_period_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_CLR    (BIT(16))
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER0_PERIOD_UP_ST_CLR_S  16
/** SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_timer1_period_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_CLR    (BIT(17))
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER1_PERIOD_UP_ST_CLR_S  17
/** SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_timer2_period_up trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_CLR    (BIT(18))
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TIMER2_PERIOD_UP_ST_CLR_S  18
/** SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_tz0_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_CLR    (BIT(19))
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TZ0_OST_ST_CLR_S  19
/** SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_tz1_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_CLR    (BIT(20))
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TZ1_OST_ST_CLR_S  20
/** SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_tz2_ost trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_CLR    (BIT(21))
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_CLR_V << SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_TZ2_OST_ST_CLR_S  21
/** SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_clr0_ost trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_CLR    (BIT(22))
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CLR0_OST_ST_CLR_S  22
/** SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_clr1_ost trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_CLR    (BIT(23))
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CLR1_OST_ST_CLR_S  23
/** SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_clr2_ost trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_CLR    (BIT(24))
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CLR2_OST_ST_CLR_S  24
/** SOC_ETM_MCPWM1_TASK_CAP0_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cap0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CAP0_ST_CLR    (BIT(25))
#define SOC_ETM_MCPWM1_TASK_CAP0_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CAP0_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CAP0_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CAP0_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CAP0_ST_CLR_S  25
/** SOC_ETM_MCPWM1_TASK_CAP1_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cap1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CAP1_ST_CLR    (BIT(26))
#define SOC_ETM_MCPWM1_TASK_CAP1_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CAP1_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CAP1_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CAP1_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CAP1_ST_CLR_S  26
/** SOC_ETM_MCPWM1_TASK_CAP2_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear MCPWM1_task_cap2 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_MCPWM1_TASK_CAP2_ST_CLR    (BIT(27))
#define SOC_ETM_MCPWM1_TASK_CAP2_ST_CLR_M  (SOC_ETM_MCPWM1_TASK_CAP2_ST_CLR_V << SOC_ETM_MCPWM1_TASK_CAP2_ST_CLR_S)
#define SOC_ETM_MCPWM1_TASK_CAP2_ST_CLR_V  0x00000001U
#define SOC_ETM_MCPWM1_TASK_CAP2_ST_CLR_S  27
/** SOC_ETM_ADC_TASK_SAMPLE0_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear ADC_task_sample0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ADC_TASK_SAMPLE0_ST_CLR    (BIT(28))
#define SOC_ETM_ADC_TASK_SAMPLE0_ST_CLR_M  (SOC_ETM_ADC_TASK_SAMPLE0_ST_CLR_V << SOC_ETM_ADC_TASK_SAMPLE0_ST_CLR_S)
#define SOC_ETM_ADC_TASK_SAMPLE0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_TASK_SAMPLE0_ST_CLR_S  28
/** SOC_ETM_ADC_TASK_SAMPLE1_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear ADC_task_sample1 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ADC_TASK_SAMPLE1_ST_CLR    (BIT(29))
#define SOC_ETM_ADC_TASK_SAMPLE1_ST_CLR_M  (SOC_ETM_ADC_TASK_SAMPLE1_ST_CLR_V << SOC_ETM_ADC_TASK_SAMPLE1_ST_CLR_S)
#define SOC_ETM_ADC_TASK_SAMPLE1_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_TASK_SAMPLE1_ST_CLR_S  29
/** SOC_ETM_ADC_TASK_START0_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear ADC_task_start0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ADC_TASK_START0_ST_CLR    (BIT(30))
#define SOC_ETM_ADC_TASK_START0_ST_CLR_M  (SOC_ETM_ADC_TASK_START0_ST_CLR_V << SOC_ETM_ADC_TASK_START0_ST_CLR_S)
#define SOC_ETM_ADC_TASK_START0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_TASK_START0_ST_CLR_S  30
/** SOC_ETM_ADC_TASK_STOP0_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear ADC_task_stop0 trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ADC_TASK_STOP0_ST_CLR    (BIT(31))
#define SOC_ETM_ADC_TASK_STOP0_ST_CLR_M  (SOC_ETM_ADC_TASK_STOP0_ST_CLR_V << SOC_ETM_ADC_TASK_STOP0_ST_CLR_S)
#define SOC_ETM_ADC_TASK_STOP0_ST_CLR_V  0x00000001U
#define SOC_ETM_ADC_TASK_STOP0_ST_CLR_S  31

/** SOC_ETM_TASK_ST5_REG register
 *  Tasks trigger status register
 */
#define SOC_ETM_TASK_ST5_REG (DR_REG_SOC_ETM_BASE + 0x210)
/** SOC_ETM_REGDMA_TASK_START0_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents REGDMA_task_start0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_TASK_START0_ST    (BIT(0))
#define SOC_ETM_REGDMA_TASK_START0_ST_M  (SOC_ETM_REGDMA_TASK_START0_ST_V << SOC_ETM_REGDMA_TASK_START0_ST_S)
#define SOC_ETM_REGDMA_TASK_START0_ST_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START0_ST_S  0
/** SOC_ETM_REGDMA_TASK_START1_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents REGDMA_task_start1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_TASK_START1_ST    (BIT(1))
#define SOC_ETM_REGDMA_TASK_START1_ST_M  (SOC_ETM_REGDMA_TASK_START1_ST_V << SOC_ETM_REGDMA_TASK_START1_ST_S)
#define SOC_ETM_REGDMA_TASK_START1_ST_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START1_ST_S  1
/** SOC_ETM_REGDMA_TASK_START2_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents REGDMA_task_start2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_TASK_START2_ST    (BIT(2))
#define SOC_ETM_REGDMA_TASK_START2_ST_M  (SOC_ETM_REGDMA_TASK_START2_ST_V << SOC_ETM_REGDMA_TASK_START2_ST_S)
#define SOC_ETM_REGDMA_TASK_START2_ST_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START2_ST_S  2
/** SOC_ETM_REGDMA_TASK_START3_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents REGDMA_task_start3 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_REGDMA_TASK_START3_ST    (BIT(3))
#define SOC_ETM_REGDMA_TASK_START3_ST_M  (SOC_ETM_REGDMA_TASK_START3_ST_V << SOC_ETM_REGDMA_TASK_START3_ST_S)
#define SOC_ETM_REGDMA_TASK_START3_ST_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START3_ST_S  3
/** SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents TMPSNSR_task_start_sample trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST    (BIT(4))
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_M  (SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_V << SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_S)
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_V  0x00000001U
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_S  4
/** SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents TMPSNSR_task_stop_sample trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST    (BIT(5))
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_M  (SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_V << SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_S)
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_V  0x00000001U
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_S  5
/** SOC_ETM_I2S0_TASK_START_RX_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents I2S0_task_start_rx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_TASK_START_RX_ST    (BIT(6))
#define SOC_ETM_I2S0_TASK_START_RX_ST_M  (SOC_ETM_I2S0_TASK_START_RX_ST_V << SOC_ETM_I2S0_TASK_START_RX_ST_S)
#define SOC_ETM_I2S0_TASK_START_RX_ST_V  0x00000001U
#define SOC_ETM_I2S0_TASK_START_RX_ST_S  6
/** SOC_ETM_I2S0_TASK_START_TX_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents I2S0_task_start_tx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_TASK_START_TX_ST    (BIT(7))
#define SOC_ETM_I2S0_TASK_START_TX_ST_M  (SOC_ETM_I2S0_TASK_START_TX_ST_V << SOC_ETM_I2S0_TASK_START_TX_ST_S)
#define SOC_ETM_I2S0_TASK_START_TX_ST_V  0x00000001U
#define SOC_ETM_I2S0_TASK_START_TX_ST_S  7
/** SOC_ETM_I2S0_TASK_STOP_RX_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents I2S0_task_stop_rx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_TASK_STOP_RX_ST    (BIT(8))
#define SOC_ETM_I2S0_TASK_STOP_RX_ST_M  (SOC_ETM_I2S0_TASK_STOP_RX_ST_V << SOC_ETM_I2S0_TASK_STOP_RX_ST_S)
#define SOC_ETM_I2S0_TASK_STOP_RX_ST_V  0x00000001U
#define SOC_ETM_I2S0_TASK_STOP_RX_ST_S  8
/** SOC_ETM_I2S0_TASK_STOP_TX_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents I2S0_task_stop_tx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S0_TASK_STOP_TX_ST    (BIT(9))
#define SOC_ETM_I2S0_TASK_STOP_TX_ST_M  (SOC_ETM_I2S0_TASK_STOP_TX_ST_V << SOC_ETM_I2S0_TASK_STOP_TX_ST_S)
#define SOC_ETM_I2S0_TASK_STOP_TX_ST_V  0x00000001U
#define SOC_ETM_I2S0_TASK_STOP_TX_ST_S  9
/** SOC_ETM_I2S1_TASK_START_RX_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents I2S1_task_start_rx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_TASK_START_RX_ST    (BIT(10))
#define SOC_ETM_I2S1_TASK_START_RX_ST_M  (SOC_ETM_I2S1_TASK_START_RX_ST_V << SOC_ETM_I2S1_TASK_START_RX_ST_S)
#define SOC_ETM_I2S1_TASK_START_RX_ST_V  0x00000001U
#define SOC_ETM_I2S1_TASK_START_RX_ST_S  10
/** SOC_ETM_I2S1_TASK_START_TX_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents I2S1_task_start_tx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_TASK_START_TX_ST    (BIT(11))
#define SOC_ETM_I2S1_TASK_START_TX_ST_M  (SOC_ETM_I2S1_TASK_START_TX_ST_V << SOC_ETM_I2S1_TASK_START_TX_ST_S)
#define SOC_ETM_I2S1_TASK_START_TX_ST_V  0x00000001U
#define SOC_ETM_I2S1_TASK_START_TX_ST_S  11
/** SOC_ETM_I2S1_TASK_STOP_RX_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents I2S1_task_stop_rx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_TASK_STOP_RX_ST    (BIT(12))
#define SOC_ETM_I2S1_TASK_STOP_RX_ST_M  (SOC_ETM_I2S1_TASK_STOP_RX_ST_V << SOC_ETM_I2S1_TASK_STOP_RX_ST_S)
#define SOC_ETM_I2S1_TASK_STOP_RX_ST_V  0x00000001U
#define SOC_ETM_I2S1_TASK_STOP_RX_ST_S  12
/** SOC_ETM_I2S1_TASK_STOP_TX_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents I2S1_task_stop_tx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S1_TASK_STOP_TX_ST    (BIT(13))
#define SOC_ETM_I2S1_TASK_STOP_TX_ST_M  (SOC_ETM_I2S1_TASK_STOP_TX_ST_V << SOC_ETM_I2S1_TASK_STOP_TX_ST_S)
#define SOC_ETM_I2S1_TASK_STOP_TX_ST_V  0x00000001U
#define SOC_ETM_I2S1_TASK_STOP_TX_ST_S  13
/** SOC_ETM_I2S2_TASK_START_RX_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents I2S2_task_start_rx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_TASK_START_RX_ST    (BIT(14))
#define SOC_ETM_I2S2_TASK_START_RX_ST_M  (SOC_ETM_I2S2_TASK_START_RX_ST_V << SOC_ETM_I2S2_TASK_START_RX_ST_S)
#define SOC_ETM_I2S2_TASK_START_RX_ST_V  0x00000001U
#define SOC_ETM_I2S2_TASK_START_RX_ST_S  14
/** SOC_ETM_I2S2_TASK_START_TX_ST : R/WTC/SS; bitpos: [15]; default: 0;
 *  Represents I2S2_task_start_tx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_TASK_START_TX_ST    (BIT(15))
#define SOC_ETM_I2S2_TASK_START_TX_ST_M  (SOC_ETM_I2S2_TASK_START_TX_ST_V << SOC_ETM_I2S2_TASK_START_TX_ST_S)
#define SOC_ETM_I2S2_TASK_START_TX_ST_V  0x00000001U
#define SOC_ETM_I2S2_TASK_START_TX_ST_S  15
/** SOC_ETM_I2S2_TASK_STOP_RX_ST : R/WTC/SS; bitpos: [16]; default: 0;
 *  Represents I2S2_task_stop_rx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_TASK_STOP_RX_ST    (BIT(16))
#define SOC_ETM_I2S2_TASK_STOP_RX_ST_M  (SOC_ETM_I2S2_TASK_STOP_RX_ST_V << SOC_ETM_I2S2_TASK_STOP_RX_ST_S)
#define SOC_ETM_I2S2_TASK_STOP_RX_ST_V  0x00000001U
#define SOC_ETM_I2S2_TASK_STOP_RX_ST_S  16
/** SOC_ETM_I2S2_TASK_STOP_TX_ST : R/WTC/SS; bitpos: [17]; default: 0;
 *  Represents I2S2_task_stop_tx trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_I2S2_TASK_STOP_TX_ST    (BIT(17))
#define SOC_ETM_I2S2_TASK_STOP_TX_ST_M  (SOC_ETM_I2S2_TASK_STOP_TX_ST_V << SOC_ETM_I2S2_TASK_STOP_TX_ST_S)
#define SOC_ETM_I2S2_TASK_STOP_TX_ST_V  0x00000001U
#define SOC_ETM_I2S2_TASK_STOP_TX_ST_S  17
/** SOC_ETM_ULP_TASK_WAKEUP_CPU_ST : R/WTC/SS; bitpos: [18]; default: 0;
 *  Represents ULP_task_wakeup_cpu trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST    (BIT(18))
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_M  (SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_V << SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_S)
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_V  0x00000001U
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_S  18
/** SOC_ETM_ULP_TASK_INT_CPU_ST : R/WTC/SS; bitpos: [19]; default: 0;
 *  Represents ULP_task_int_cpu trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_ULP_TASK_INT_CPU_ST    (BIT(19))
#define SOC_ETM_ULP_TASK_INT_CPU_ST_M  (SOC_ETM_ULP_TASK_INT_CPU_ST_V << SOC_ETM_ULP_TASK_INT_CPU_ST_S)
#define SOC_ETM_ULP_TASK_INT_CPU_ST_V  0x00000001U
#define SOC_ETM_ULP_TASK_INT_CPU_ST_S  19
/** SOC_ETM_RTC_TASK_START_ST : R/WTC/SS; bitpos: [20]; default: 0;
 *  Represents RTC_task_start trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_RTC_TASK_START_ST    (BIT(20))
#define SOC_ETM_RTC_TASK_START_ST_M  (SOC_ETM_RTC_TASK_START_ST_V << SOC_ETM_RTC_TASK_START_ST_S)
#define SOC_ETM_RTC_TASK_START_ST_V  0x00000001U
#define SOC_ETM_RTC_TASK_START_ST_S  20
/** SOC_ETM_RTC_TASK_STOP_ST : R/WTC/SS; bitpos: [21]; default: 0;
 *  Represents RTC_task_stop trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_RTC_TASK_STOP_ST    (BIT(21))
#define SOC_ETM_RTC_TASK_STOP_ST_M  (SOC_ETM_RTC_TASK_STOP_ST_V << SOC_ETM_RTC_TASK_STOP_ST_S)
#define SOC_ETM_RTC_TASK_STOP_ST_V  0x00000001U
#define SOC_ETM_RTC_TASK_STOP_ST_S  21
/** SOC_ETM_RTC_TASK_CLR_ST : R/WTC/SS; bitpos: [22]; default: 0;
 *  Represents RTC_task_clr trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_RTC_TASK_CLR_ST    (BIT(22))
#define SOC_ETM_RTC_TASK_CLR_ST_M  (SOC_ETM_RTC_TASK_CLR_ST_V << SOC_ETM_RTC_TASK_CLR_ST_S)
#define SOC_ETM_RTC_TASK_CLR_ST_V  0x00000001U
#define SOC_ETM_RTC_TASK_CLR_ST_S  22
/** SOC_ETM_RTC_TASK_TRIGGERFLW_ST : R/WTC/SS; bitpos: [23]; default: 0;
 *  Represents RTC_task_triggerflw trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST    (BIT(23))
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST_M  (SOC_ETM_RTC_TASK_TRIGGERFLW_ST_V << SOC_ETM_RTC_TASK_TRIGGERFLW_ST_S)
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST_V  0x00000001U
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST_S  23
/** SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST : R/WTC/SS; bitpos: [24]; default: 0;
 *  Represents PDMA_AHB_task_in_start_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST    (BIT(24))
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_M  (SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_V << SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_S  24
/** SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST : R/WTC/SS; bitpos: [25]; default: 0;
 *  Represents PDMA_AHB_task_in_start_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST    (BIT(25))
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_M  (SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_V << SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_S  25
/** SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST : R/WTC/SS; bitpos: [26]; default: 0;
 *  Represents PDMA_AHB_task_in_start_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST    (BIT(26))
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_M  (SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_V << SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_S  26
/** SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST : R/WTC/SS; bitpos: [27]; default: 0;
 *  Represents PDMA_AHB_task_out_start_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST    (BIT(27))
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_M  (SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_V << SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_S)
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_S  27
/** SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST : R/WTC/SS; bitpos: [28]; default: 0;
 *  Represents PDMA_AHB_task_out_start_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST    (BIT(28))
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_M  (SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_V << SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_S)
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_S  28
/** SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST : R/WTC/SS; bitpos: [29]; default: 0;
 *  Represents PDMA_AHB_task_out_start_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST    (BIT(29))
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_M  (SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_V << SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_S)
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_S  29
/** SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST : R/WTC/SS; bitpos: [30]; default: 0;
 *  Represents PDMA_AXI_task_in_start_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST    (BIT(30))
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_M  (SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_V << SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_S  30
/** SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST : R/WTC/SS; bitpos: [31]; default: 0;
 *  Represents PDMA_AXI_task_in_start_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST    (BIT(31))
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_M  (SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_V << SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_S  31

/** SOC_ETM_TASK_ST5_CLR_REG register
 *  Tasks trigger status clear register
 */
#define SOC_ETM_TASK_ST5_CLR_REG (DR_REG_SOC_ETM_BASE + 0x214)
/** SOC_ETM_REGDMA_TASK_START0_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear REGDMA_task_start0 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_REGDMA_TASK_START0_ST_CLR    (BIT(0))
#define SOC_ETM_REGDMA_TASK_START0_ST_CLR_M  (SOC_ETM_REGDMA_TASK_START0_ST_CLR_V << SOC_ETM_REGDMA_TASK_START0_ST_CLR_S)
#define SOC_ETM_REGDMA_TASK_START0_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START0_ST_CLR_S  0
/** SOC_ETM_REGDMA_TASK_START1_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear REGDMA_task_start1 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_REGDMA_TASK_START1_ST_CLR    (BIT(1))
#define SOC_ETM_REGDMA_TASK_START1_ST_CLR_M  (SOC_ETM_REGDMA_TASK_START1_ST_CLR_V << SOC_ETM_REGDMA_TASK_START1_ST_CLR_S)
#define SOC_ETM_REGDMA_TASK_START1_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START1_ST_CLR_S  1
/** SOC_ETM_REGDMA_TASK_START2_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear REGDMA_task_start2 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_REGDMA_TASK_START2_ST_CLR    (BIT(2))
#define SOC_ETM_REGDMA_TASK_START2_ST_CLR_M  (SOC_ETM_REGDMA_TASK_START2_ST_CLR_V << SOC_ETM_REGDMA_TASK_START2_ST_CLR_S)
#define SOC_ETM_REGDMA_TASK_START2_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START2_ST_CLR_S  2
/** SOC_ETM_REGDMA_TASK_START3_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear REGDMA_task_start3 trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_REGDMA_TASK_START3_ST_CLR    (BIT(3))
#define SOC_ETM_REGDMA_TASK_START3_ST_CLR_M  (SOC_ETM_REGDMA_TASK_START3_ST_CLR_V << SOC_ETM_REGDMA_TASK_START3_ST_CLR_S)
#define SOC_ETM_REGDMA_TASK_START3_ST_CLR_V  0x00000001U
#define SOC_ETM_REGDMA_TASK_START3_ST_CLR_S  3
/** SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear TMPSNSR_task_start_sample trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_CLR    (BIT(4))
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_CLR_M  (SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_CLR_V << SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_CLR_S)
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_CLR_V  0x00000001U
#define SOC_ETM_TMPSNSR_TASK_START_SAMPLE_ST_CLR_S  4
/** SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear TMPSNSR_task_stop_sample trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_CLR    (BIT(5))
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_CLR_M  (SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_CLR_V << SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_CLR_S)
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_CLR_V  0x00000001U
#define SOC_ETM_TMPSNSR_TASK_STOP_SAMPLE_ST_CLR_S  5
/** SOC_ETM_I2S0_TASK_START_RX_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear I2S0_task_start_rx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S0_TASK_START_RX_ST_CLR    (BIT(6))
#define SOC_ETM_I2S0_TASK_START_RX_ST_CLR_M  (SOC_ETM_I2S0_TASK_START_RX_ST_CLR_V << SOC_ETM_I2S0_TASK_START_RX_ST_CLR_S)
#define SOC_ETM_I2S0_TASK_START_RX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_TASK_START_RX_ST_CLR_S  6
/** SOC_ETM_I2S0_TASK_START_TX_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear I2S0_task_start_tx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S0_TASK_START_TX_ST_CLR    (BIT(7))
#define SOC_ETM_I2S0_TASK_START_TX_ST_CLR_M  (SOC_ETM_I2S0_TASK_START_TX_ST_CLR_V << SOC_ETM_I2S0_TASK_START_TX_ST_CLR_S)
#define SOC_ETM_I2S0_TASK_START_TX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_TASK_START_TX_ST_CLR_S  7
/** SOC_ETM_I2S0_TASK_STOP_RX_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear I2S0_task_stop_rx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S0_TASK_STOP_RX_ST_CLR    (BIT(8))
#define SOC_ETM_I2S0_TASK_STOP_RX_ST_CLR_M  (SOC_ETM_I2S0_TASK_STOP_RX_ST_CLR_V << SOC_ETM_I2S0_TASK_STOP_RX_ST_CLR_S)
#define SOC_ETM_I2S0_TASK_STOP_RX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_TASK_STOP_RX_ST_CLR_S  8
/** SOC_ETM_I2S0_TASK_STOP_TX_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear I2S0_task_stop_tx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S0_TASK_STOP_TX_ST_CLR    (BIT(9))
#define SOC_ETM_I2S0_TASK_STOP_TX_ST_CLR_M  (SOC_ETM_I2S0_TASK_STOP_TX_ST_CLR_V << SOC_ETM_I2S0_TASK_STOP_TX_ST_CLR_S)
#define SOC_ETM_I2S0_TASK_STOP_TX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S0_TASK_STOP_TX_ST_CLR_S  9
/** SOC_ETM_I2S1_TASK_START_RX_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear I2S1_task_start_rx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S1_TASK_START_RX_ST_CLR    (BIT(10))
#define SOC_ETM_I2S1_TASK_START_RX_ST_CLR_M  (SOC_ETM_I2S1_TASK_START_RX_ST_CLR_V << SOC_ETM_I2S1_TASK_START_RX_ST_CLR_S)
#define SOC_ETM_I2S1_TASK_START_RX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_TASK_START_RX_ST_CLR_S  10
/** SOC_ETM_I2S1_TASK_START_TX_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear I2S1_task_start_tx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S1_TASK_START_TX_ST_CLR    (BIT(11))
#define SOC_ETM_I2S1_TASK_START_TX_ST_CLR_M  (SOC_ETM_I2S1_TASK_START_TX_ST_CLR_V << SOC_ETM_I2S1_TASK_START_TX_ST_CLR_S)
#define SOC_ETM_I2S1_TASK_START_TX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_TASK_START_TX_ST_CLR_S  11
/** SOC_ETM_I2S1_TASK_STOP_RX_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear I2S1_task_stop_rx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S1_TASK_STOP_RX_ST_CLR    (BIT(12))
#define SOC_ETM_I2S1_TASK_STOP_RX_ST_CLR_M  (SOC_ETM_I2S1_TASK_STOP_RX_ST_CLR_V << SOC_ETM_I2S1_TASK_STOP_RX_ST_CLR_S)
#define SOC_ETM_I2S1_TASK_STOP_RX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_TASK_STOP_RX_ST_CLR_S  12
/** SOC_ETM_I2S1_TASK_STOP_TX_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear I2S1_task_stop_tx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S1_TASK_STOP_TX_ST_CLR    (BIT(13))
#define SOC_ETM_I2S1_TASK_STOP_TX_ST_CLR_M  (SOC_ETM_I2S1_TASK_STOP_TX_ST_CLR_V << SOC_ETM_I2S1_TASK_STOP_TX_ST_CLR_S)
#define SOC_ETM_I2S1_TASK_STOP_TX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S1_TASK_STOP_TX_ST_CLR_S  13
/** SOC_ETM_I2S2_TASK_START_RX_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear I2S2_task_start_rx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S2_TASK_START_RX_ST_CLR    (BIT(14))
#define SOC_ETM_I2S2_TASK_START_RX_ST_CLR_M  (SOC_ETM_I2S2_TASK_START_RX_ST_CLR_V << SOC_ETM_I2S2_TASK_START_RX_ST_CLR_S)
#define SOC_ETM_I2S2_TASK_START_RX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_TASK_START_RX_ST_CLR_S  14
/** SOC_ETM_I2S2_TASK_START_TX_ST_CLR : WT; bitpos: [15]; default: 0;
 *  Configures whether or not to clear I2S2_task_start_tx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S2_TASK_START_TX_ST_CLR    (BIT(15))
#define SOC_ETM_I2S2_TASK_START_TX_ST_CLR_M  (SOC_ETM_I2S2_TASK_START_TX_ST_CLR_V << SOC_ETM_I2S2_TASK_START_TX_ST_CLR_S)
#define SOC_ETM_I2S2_TASK_START_TX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_TASK_START_TX_ST_CLR_S  15
/** SOC_ETM_I2S2_TASK_STOP_RX_ST_CLR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear I2S2_task_stop_rx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S2_TASK_STOP_RX_ST_CLR    (BIT(16))
#define SOC_ETM_I2S2_TASK_STOP_RX_ST_CLR_M  (SOC_ETM_I2S2_TASK_STOP_RX_ST_CLR_V << SOC_ETM_I2S2_TASK_STOP_RX_ST_CLR_S)
#define SOC_ETM_I2S2_TASK_STOP_RX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_TASK_STOP_RX_ST_CLR_S  16
/** SOC_ETM_I2S2_TASK_STOP_TX_ST_CLR : WT; bitpos: [17]; default: 0;
 *  Configures whether or not to clear I2S2_task_stop_tx trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_I2S2_TASK_STOP_TX_ST_CLR    (BIT(17))
#define SOC_ETM_I2S2_TASK_STOP_TX_ST_CLR_M  (SOC_ETM_I2S2_TASK_STOP_TX_ST_CLR_V << SOC_ETM_I2S2_TASK_STOP_TX_ST_CLR_S)
#define SOC_ETM_I2S2_TASK_STOP_TX_ST_CLR_V  0x00000001U
#define SOC_ETM_I2S2_TASK_STOP_TX_ST_CLR_S  17
/** SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_CLR : WT; bitpos: [18]; default: 0;
 *  Configures whether or not to clear ULP_task_wakeup_cpu trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_CLR    (BIT(18))
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_CLR_M  (SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_CLR_V << SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_CLR_S)
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_CLR_V  0x00000001U
#define SOC_ETM_ULP_TASK_WAKEUP_CPU_ST_CLR_S  18
/** SOC_ETM_ULP_TASK_INT_CPU_ST_CLR : WT; bitpos: [19]; default: 0;
 *  Configures whether or not to clear ULP_task_int_cpu trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_ULP_TASK_INT_CPU_ST_CLR    (BIT(19))
#define SOC_ETM_ULP_TASK_INT_CPU_ST_CLR_M  (SOC_ETM_ULP_TASK_INT_CPU_ST_CLR_V << SOC_ETM_ULP_TASK_INT_CPU_ST_CLR_S)
#define SOC_ETM_ULP_TASK_INT_CPU_ST_CLR_V  0x00000001U
#define SOC_ETM_ULP_TASK_INT_CPU_ST_CLR_S  19
/** SOC_ETM_RTC_TASK_START_ST_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether or not to clear RTC_task_start trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_RTC_TASK_START_ST_CLR    (BIT(20))
#define SOC_ETM_RTC_TASK_START_ST_CLR_M  (SOC_ETM_RTC_TASK_START_ST_CLR_V << SOC_ETM_RTC_TASK_START_ST_CLR_S)
#define SOC_ETM_RTC_TASK_START_ST_CLR_V  0x00000001U
#define SOC_ETM_RTC_TASK_START_ST_CLR_S  20
/** SOC_ETM_RTC_TASK_STOP_ST_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether or not to clear RTC_task_stop trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_RTC_TASK_STOP_ST_CLR    (BIT(21))
#define SOC_ETM_RTC_TASK_STOP_ST_CLR_M  (SOC_ETM_RTC_TASK_STOP_ST_CLR_V << SOC_ETM_RTC_TASK_STOP_ST_CLR_S)
#define SOC_ETM_RTC_TASK_STOP_ST_CLR_V  0x00000001U
#define SOC_ETM_RTC_TASK_STOP_ST_CLR_S  21
/** SOC_ETM_RTC_TASK_CLR_ST_CLR : WT; bitpos: [22]; default: 0;
 *  Configures whether or not to clear RTC_task_clr trigger status.\\0: Invalid, No
 *  effect\\1: Clear
 */
#define SOC_ETM_RTC_TASK_CLR_ST_CLR    (BIT(22))
#define SOC_ETM_RTC_TASK_CLR_ST_CLR_M  (SOC_ETM_RTC_TASK_CLR_ST_CLR_V << SOC_ETM_RTC_TASK_CLR_ST_CLR_S)
#define SOC_ETM_RTC_TASK_CLR_ST_CLR_V  0x00000001U
#define SOC_ETM_RTC_TASK_CLR_ST_CLR_S  22
/** SOC_ETM_RTC_TASK_TRIGGERFLW_ST_CLR : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to clear RTC_task_triggerflw trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST_CLR    (BIT(23))
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST_CLR_M  (SOC_ETM_RTC_TASK_TRIGGERFLW_ST_CLR_V << SOC_ETM_RTC_TASK_TRIGGERFLW_ST_CLR_S)
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST_CLR_V  0x00000001U
#define SOC_ETM_RTC_TASK_TRIGGERFLW_ST_CLR_S  23
/** SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_CLR : WT; bitpos: [24]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_task_in_start_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_CLR    (BIT(24))
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH0_ST_CLR_S  24
/** SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_CLR : WT; bitpos: [25]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_task_in_start_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_CLR    (BIT(25))
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH1_ST_CLR_S  25
/** SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_CLR : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_task_in_start_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_CLR    (BIT(26))
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_IN_START_CH2_ST_CLR_S  26
/** SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_CLR : WT; bitpos: [27]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_task_out_start_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_CLR    (BIT(27))
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_CLR_M  (SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_CLR_V << SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH0_ST_CLR_S  27
/** SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_CLR : WT; bitpos: [28]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_task_out_start_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_CLR    (BIT(28))
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_CLR_M  (SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_CLR_V << SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH1_ST_CLR_S  28
/** SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_CLR : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to clear PDMA_AHB_task_out_start_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_CLR    (BIT(29))
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_CLR_M  (SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_CLR_V << SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AHB_TASK_OUT_START_CH2_ST_CLR_S  29
/** SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_CLR : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_task_in_start_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_CLR    (BIT(30))
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH0_ST_CLR_S  30
/** SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_CLR : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_task_in_start_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_CLR    (BIT(31))
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH1_ST_CLR_S  31

/** SOC_ETM_TASK_ST6_REG register
 *  Tasks trigger status register
 */
#define SOC_ETM_TASK_ST6_REG (DR_REG_SOC_ETM_BASE + 0x218)
/** SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST : R/WTC/SS; bitpos: [0]; default: 0;
 *  Represents PDMA_AXI_task_in_start_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST    (BIT(0))
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_M  (SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_V << SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_S  0
/** SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST : R/WTC/SS; bitpos: [1]; default: 0;
 *  Represents PDMA_AXI_task_out_start_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST    (BIT(1))
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_M  (SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_V << SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_S)
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_S  1
/** SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST : R/WTC/SS; bitpos: [2]; default: 0;
 *  Represents PDMA_AXI_task_out_start_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST    (BIT(2))
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_M  (SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_V << SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_S)
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_S  2
/** SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST : R/WTC/SS; bitpos: [3]; default: 0;
 *  Represents PDMA_AXI_task_out_start_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST    (BIT(3))
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_M  (SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_V << SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_S)
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_S  3
/** SOC_ETM_PMU_TASK_SLEEP_REQ_ST : R/WTC/SS; bitpos: [4]; default: 0;
 *  Represents PMU_task_sleep_req trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST    (BIT(4))
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST_M  (SOC_ETM_PMU_TASK_SLEEP_REQ_ST_V << SOC_ETM_PMU_TASK_SLEEP_REQ_ST_S)
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST_V  0x00000001U
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST_S  4
/** SOC_ETM_DMA2D_TASK_IN_START_CH0_ST : R/WTC/SS; bitpos: [5]; default: 0;
 *  Represents DMA2D_task_in_start_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST    (BIT(5))
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_M  (SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_V << SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_S)
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_S  5
/** SOC_ETM_DMA2D_TASK_IN_START_CH1_ST : R/WTC/SS; bitpos: [6]; default: 0;
 *  Represents DMA2D_task_in_start_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST    (BIT(6))
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_M  (SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_V << SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_S)
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_S  6
/** SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST : R/WTC/SS; bitpos: [7]; default: 0;
 *  Represents DMA2D_task_in_dscr_ready_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST    (BIT(7))
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_M  (SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_V << SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_S)
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_S  7
/** SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST : R/WTC/SS; bitpos: [8]; default: 0;
 *  Represents DMA2D_task_in_dscr_ready_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST    (BIT(8))
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_M  (SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_V << SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_S)
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_S  8
/** SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST : R/WTC/SS; bitpos: [9]; default: 0;
 *  Represents DMA2D_task_out_start_ch0 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST    (BIT(9))
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_M  (SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_V << SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_S)
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_S  9
/** SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST : R/WTC/SS; bitpos: [10]; default: 0;
 *  Represents DMA2D_task_out_start_ch1 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST    (BIT(10))
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_M  (SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_V << SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_S)
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_S  10
/** SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST : R/WTC/SS; bitpos: [11]; default: 0;
 *  Represents DMA2D_task_out_start_ch2 trigger status.\\0: Not triggered\\1: Triggered
 */
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST    (BIT(11))
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_M  (SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_V << SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_S)
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_S  11
/** SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST : R/WTC/SS; bitpos: [12]; default: 0;
 *  Represents DMA2D_task_out_dscr_ready_ch0 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST    (BIT(12))
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_M  (SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_V << SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_S)
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_S  12
/** SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST : R/WTC/SS; bitpos: [13]; default: 0;
 *  Represents DMA2D_task_out_dscr_ready_ch1 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST    (BIT(13))
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_M  (SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_V << SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_S)
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_S  13
/** SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST : R/WTC/SS; bitpos: [14]; default: 0;
 *  Represents DMA2D_task_out_dscr_ready_ch2 trigger status.\\0: Not triggered\\1:
 *  Triggered
 */
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST    (BIT(14))
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_M  (SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_V << SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_S)
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_S  14

/** SOC_ETM_TASK_ST6_CLR_REG register
 *  Tasks trigger status clear register
 */
#define SOC_ETM_TASK_ST6_CLR_REG (DR_REG_SOC_ETM_BASE + 0x21c)
/** SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_task_in_start_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_CLR    (BIT(0))
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_IN_START_CH2_ST_CLR_S  0
/** SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_CLR : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_task_out_start_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_CLR    (BIT(1))
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_CLR_M  (SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_CLR_V << SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH0_ST_CLR_S  1
/** SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_CLR : WT; bitpos: [2]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_task_out_start_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_CLR    (BIT(2))
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_CLR_M  (SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_CLR_V << SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH1_ST_CLR_S  2
/** SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_CLR : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to clear PDMA_AXI_task_out_start_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_CLR    (BIT(3))
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_CLR_M  (SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_CLR_V << SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_CLR_S)
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_PDMA_AXI_TASK_OUT_START_CH2_ST_CLR_S  3
/** SOC_ETM_PMU_TASK_SLEEP_REQ_ST_CLR : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to clear PMU_task_sleep_req trigger status.\\0: Invalid,
 *  No effect\\1: Clear
 */
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST_CLR    (BIT(4))
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST_CLR_M  (SOC_ETM_PMU_TASK_SLEEP_REQ_ST_CLR_V << SOC_ETM_PMU_TASK_SLEEP_REQ_ST_CLR_S)
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST_CLR_V  0x00000001U
#define SOC_ETM_PMU_TASK_SLEEP_REQ_ST_CLR_S  4
/** SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_CLR : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to clear DMA2D_task_in_start_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_CLR    (BIT(5))
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_CLR_M  (SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_CLR_V << SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_START_CH0_ST_CLR_S  5
/** SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_CLR : WT; bitpos: [6]; default: 0;
 *  Configures whether or not to clear DMA2D_task_in_start_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_CLR    (BIT(6))
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_CLR_M  (SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_CLR_V << SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_START_CH1_ST_CLR_S  6
/** SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_CLR : WT; bitpos: [7]; default: 0;
 *  Configures whether or not to clear DMA2D_task_in_dscr_ready_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_CLR    (BIT(7))
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_CLR_M  (SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_CLR_V << SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH0_ST_CLR_S  7
/** SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_CLR : WT; bitpos: [8]; default: 0;
 *  Configures whether or not to clear DMA2D_task_in_dscr_ready_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_CLR    (BIT(8))
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_CLR_M  (SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_CLR_V << SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_IN_DSCR_READY_CH1_ST_CLR_S  8
/** SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_CLR : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to clear DMA2D_task_out_start_ch0 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_CLR    (BIT(9))
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_CLR_M  (SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_CLR_V << SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_START_CH0_ST_CLR_S  9
/** SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_CLR : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to clear DMA2D_task_out_start_ch1 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_CLR    (BIT(10))
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_CLR_M  (SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_CLR_V << SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_START_CH1_ST_CLR_S  10
/** SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_CLR : WT; bitpos: [11]; default: 0;
 *  Configures whether or not to clear DMA2D_task_out_start_ch2 trigger status.\\0:
 *  Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_CLR    (BIT(11))
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_CLR_M  (SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_CLR_V << SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_START_CH2_ST_CLR_S  11
/** SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_CLR : WT; bitpos: [12]; default: 0;
 *  Configures whether or not to clear DMA2D_task_out_dscr_ready_ch0 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_CLR    (BIT(12))
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_CLR_M  (SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_CLR_V << SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH0_ST_CLR_S  12
/** SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_CLR : WT; bitpos: [13]; default: 0;
 *  Configures whether or not to clear DMA2D_task_out_dscr_ready_ch1 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_CLR    (BIT(13))
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_CLR_M  (SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_CLR_V << SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH1_ST_CLR_S  13
/** SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_CLR : WT; bitpos: [14]; default: 0;
 *  Configures whether or not to clear DMA2D_task_out_dscr_ready_ch2 trigger
 *  status.\\0: Invalid, No effect\\1: Clear
 */
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_CLR    (BIT(14))
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_CLR_M  (SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_CLR_V << SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_CLR_S)
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_CLR_V  0x00000001U
#define SOC_ETM_DMA2D_TASK_OUT_DSCR_READY_CH2_ST_CLR_S  14

/** SOC_ETM_CLK_EN_REG register
 *  ETM clock enable register
 */
#define SOC_ETM_CLK_EN_REG (DR_REG_SOC_ETM_BASE + 0x220)
/** SOC_ETM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to open register clock gate.\\0: Open the clock gate only
 *  when application writes registers\\1: Force open the clock gate for register
 */
#define SOC_ETM_CLK_EN    (BIT(0))
#define SOC_ETM_CLK_EN_M  (SOC_ETM_CLK_EN_V << SOC_ETM_CLK_EN_S)
#define SOC_ETM_CLK_EN_V  0x00000001U
#define SOC_ETM_CLK_EN_S  0

/** SOC_ETM_DATE_REG register
 *  ETM date register
 */
#define SOC_ETM_DATE_REG (DR_REG_SOC_ETM_BASE + 0x224)
/** SOC_ETM_DATE : R/W; bitpos: [27:0]; default: 36712497;
 *  Configures the version.
 */
#define SOC_ETM_DATE    0x0FFFFFFFU
#define SOC_ETM_DATE_M  (SOC_ETM_DATE_V << SOC_ETM_DATE_S)
#define SOC_ETM_DATE_V  0x0FFFFFFFU
#define SOC_ETM_DATE_S  0

#ifdef __cplusplus
}
#endif
