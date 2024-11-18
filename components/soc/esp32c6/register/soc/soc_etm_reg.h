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

/** SOC_ETM_CH_ENA_AD0_REG register
 *  channel enable register
 */
#define SOC_ETM_CH_ENA_AD0_REG (DR_REG_SOC_ETM_BASE + 0x0)
/** SOC_ETM_CH_ENA0 : R/WTC/WTS; bitpos: [0]; default: 0;
 *  ch0 enable
 */
#define SOC_ETM_CH_ENA0    (BIT(0))
#define SOC_ETM_CH_ENA0_M  (SOC_ETM_CH_ENA0_V << SOC_ETM_CH_ENA0_S)
#define SOC_ETM_CH_ENA0_V  0x00000001U
#define SOC_ETM_CH_ENA0_S  0
/** SOC_ETM_CH_ENA1 : R/WTC/WTS; bitpos: [1]; default: 0;
 *  ch1 enable
 */
#define SOC_ETM_CH_ENA1    (BIT(1))
#define SOC_ETM_CH_ENA1_M  (SOC_ETM_CH_ENA1_V << SOC_ETM_CH_ENA1_S)
#define SOC_ETM_CH_ENA1_V  0x00000001U
#define SOC_ETM_CH_ENA1_S  1
/** SOC_ETM_CH_ENA2 : R/WTC/WTS; bitpos: [2]; default: 0;
 *  ch2 enable
 */
#define SOC_ETM_CH_ENA2    (BIT(2))
#define SOC_ETM_CH_ENA2_M  (SOC_ETM_CH_ENA2_V << SOC_ETM_CH_ENA2_S)
#define SOC_ETM_CH_ENA2_V  0x00000001U
#define SOC_ETM_CH_ENA2_S  2
/** SOC_ETM_CH_ENA3 : R/WTC/WTS; bitpos: [3]; default: 0;
 *  ch3 enable
 */
#define SOC_ETM_CH_ENA3    (BIT(3))
#define SOC_ETM_CH_ENA3_M  (SOC_ETM_CH_ENA3_V << SOC_ETM_CH_ENA3_S)
#define SOC_ETM_CH_ENA3_V  0x00000001U
#define SOC_ETM_CH_ENA3_S  3
/** SOC_ETM_CH_ENA4 : R/WTC/WTS; bitpos: [4]; default: 0;
 *  ch4 enable
 */
#define SOC_ETM_CH_ENA4    (BIT(4))
#define SOC_ETM_CH_ENA4_M  (SOC_ETM_CH_ENA4_V << SOC_ETM_CH_ENA4_S)
#define SOC_ETM_CH_ENA4_V  0x00000001U
#define SOC_ETM_CH_ENA4_S  4
/** SOC_ETM_CH_ENA5 : R/WTC/WTS; bitpos: [5]; default: 0;
 *  ch5 enable
 */
#define SOC_ETM_CH_ENA5    (BIT(5))
#define SOC_ETM_CH_ENA5_M  (SOC_ETM_CH_ENA5_V << SOC_ETM_CH_ENA5_S)
#define SOC_ETM_CH_ENA5_V  0x00000001U
#define SOC_ETM_CH_ENA5_S  5
/** SOC_ETM_CH_ENA6 : R/WTC/WTS; bitpos: [6]; default: 0;
 *  ch6 enable
 */
#define SOC_ETM_CH_ENA6    (BIT(6))
#define SOC_ETM_CH_ENA6_M  (SOC_ETM_CH_ENA6_V << SOC_ETM_CH_ENA6_S)
#define SOC_ETM_CH_ENA6_V  0x00000001U
#define SOC_ETM_CH_ENA6_S  6
/** SOC_ETM_CH_ENA7 : R/WTC/WTS; bitpos: [7]; default: 0;
 *  ch7 enable
 */
#define SOC_ETM_CH_ENA7    (BIT(7))
#define SOC_ETM_CH_ENA7_M  (SOC_ETM_CH_ENA7_V << SOC_ETM_CH_ENA7_S)
#define SOC_ETM_CH_ENA7_V  0x00000001U
#define SOC_ETM_CH_ENA7_S  7
/** SOC_ETM_CH_ENA8 : R/WTC/WTS; bitpos: [8]; default: 0;
 *  ch8 enable
 */
#define SOC_ETM_CH_ENA8    (BIT(8))
#define SOC_ETM_CH_ENA8_M  (SOC_ETM_CH_ENA8_V << SOC_ETM_CH_ENA8_S)
#define SOC_ETM_CH_ENA8_V  0x00000001U
#define SOC_ETM_CH_ENA8_S  8
/** SOC_ETM_CH_ENA9 : R/WTC/WTS; bitpos: [9]; default: 0;
 *  ch9 enable
 */
#define SOC_ETM_CH_ENA9    (BIT(9))
#define SOC_ETM_CH_ENA9_M  (SOC_ETM_CH_ENA9_V << SOC_ETM_CH_ENA9_S)
#define SOC_ETM_CH_ENA9_V  0x00000001U
#define SOC_ETM_CH_ENA9_S  9
/** SOC_ETM_CH_ENA10 : R/WTC/WTS; bitpos: [10]; default: 0;
 *  ch10 enable
 */
#define SOC_ETM_CH_ENA10    (BIT(10))
#define SOC_ETM_CH_ENA10_M  (SOC_ETM_CH_ENA10_V << SOC_ETM_CH_ENA10_S)
#define SOC_ETM_CH_ENA10_V  0x00000001U
#define SOC_ETM_CH_ENA10_S  10
/** SOC_ETM_CH_ENA11 : R/WTC/WTS; bitpos: [11]; default: 0;
 *  ch11 enable
 */
#define SOC_ETM_CH_ENA11    (BIT(11))
#define SOC_ETM_CH_ENA11_M  (SOC_ETM_CH_ENA11_V << SOC_ETM_CH_ENA11_S)
#define SOC_ETM_CH_ENA11_V  0x00000001U
#define SOC_ETM_CH_ENA11_S  11
/** SOC_ETM_CH_ENA12 : R/WTC/WTS; bitpos: [12]; default: 0;
 *  ch12 enable
 */
#define SOC_ETM_CH_ENA12    (BIT(12))
#define SOC_ETM_CH_ENA12_M  (SOC_ETM_CH_ENA12_V << SOC_ETM_CH_ENA12_S)
#define SOC_ETM_CH_ENA12_V  0x00000001U
#define SOC_ETM_CH_ENA12_S  12
/** SOC_ETM_CH_ENA13 : R/WTC/WTS; bitpos: [13]; default: 0;
 *  ch13 enable
 */
#define SOC_ETM_CH_ENA13    (BIT(13))
#define SOC_ETM_CH_ENA13_M  (SOC_ETM_CH_ENA13_V << SOC_ETM_CH_ENA13_S)
#define SOC_ETM_CH_ENA13_V  0x00000001U
#define SOC_ETM_CH_ENA13_S  13
/** SOC_ETM_CH_ENA14 : R/WTC/WTS; bitpos: [14]; default: 0;
 *  ch14 enable
 */
#define SOC_ETM_CH_ENA14    (BIT(14))
#define SOC_ETM_CH_ENA14_M  (SOC_ETM_CH_ENA14_V << SOC_ETM_CH_ENA14_S)
#define SOC_ETM_CH_ENA14_V  0x00000001U
#define SOC_ETM_CH_ENA14_S  14
/** SOC_ETM_CH_ENA15 : R/WTC/WTS; bitpos: [15]; default: 0;
 *  ch15 enable
 */
#define SOC_ETM_CH_ENA15    (BIT(15))
#define SOC_ETM_CH_ENA15_M  (SOC_ETM_CH_ENA15_V << SOC_ETM_CH_ENA15_S)
#define SOC_ETM_CH_ENA15_V  0x00000001U
#define SOC_ETM_CH_ENA15_S  15
/** SOC_ETM_CH_ENA16 : R/WTC/WTS; bitpos: [16]; default: 0;
 *  ch16 enable
 */
#define SOC_ETM_CH_ENA16    (BIT(16))
#define SOC_ETM_CH_ENA16_M  (SOC_ETM_CH_ENA16_V << SOC_ETM_CH_ENA16_S)
#define SOC_ETM_CH_ENA16_V  0x00000001U
#define SOC_ETM_CH_ENA16_S  16
/** SOC_ETM_CH_ENA17 : R/WTC/WTS; bitpos: [17]; default: 0;
 *  ch17 enable
 */
#define SOC_ETM_CH_ENA17    (BIT(17))
#define SOC_ETM_CH_ENA17_M  (SOC_ETM_CH_ENA17_V << SOC_ETM_CH_ENA17_S)
#define SOC_ETM_CH_ENA17_V  0x00000001U
#define SOC_ETM_CH_ENA17_S  17
/** SOC_ETM_CH_ENA18 : R/WTC/WTS; bitpos: [18]; default: 0;
 *  ch18 enable
 */
#define SOC_ETM_CH_ENA18    (BIT(18))
#define SOC_ETM_CH_ENA18_M  (SOC_ETM_CH_ENA18_V << SOC_ETM_CH_ENA18_S)
#define SOC_ETM_CH_ENA18_V  0x00000001U
#define SOC_ETM_CH_ENA18_S  18
/** SOC_ETM_CH_ENA19 : R/WTC/WTS; bitpos: [19]; default: 0;
 *  ch19 enable
 */
#define SOC_ETM_CH_ENA19    (BIT(19))
#define SOC_ETM_CH_ENA19_M  (SOC_ETM_CH_ENA19_V << SOC_ETM_CH_ENA19_S)
#define SOC_ETM_CH_ENA19_V  0x00000001U
#define SOC_ETM_CH_ENA19_S  19
/** SOC_ETM_CH_ENA20 : R/WTC/WTS; bitpos: [20]; default: 0;
 *  ch20 enable
 */
#define SOC_ETM_CH_ENA20    (BIT(20))
#define SOC_ETM_CH_ENA20_M  (SOC_ETM_CH_ENA20_V << SOC_ETM_CH_ENA20_S)
#define SOC_ETM_CH_ENA20_V  0x00000001U
#define SOC_ETM_CH_ENA20_S  20
/** SOC_ETM_CH_ENA21 : R/WTC/WTS; bitpos: [21]; default: 0;
 *  ch21 enable
 */
#define SOC_ETM_CH_ENA21    (BIT(21))
#define SOC_ETM_CH_ENA21_M  (SOC_ETM_CH_ENA21_V << SOC_ETM_CH_ENA21_S)
#define SOC_ETM_CH_ENA21_V  0x00000001U
#define SOC_ETM_CH_ENA21_S  21
/** SOC_ETM_CH_ENA22 : R/WTC/WTS; bitpos: [22]; default: 0;
 *  ch22 enable
 */
#define SOC_ETM_CH_ENA22    (BIT(22))
#define SOC_ETM_CH_ENA22_M  (SOC_ETM_CH_ENA22_V << SOC_ETM_CH_ENA22_S)
#define SOC_ETM_CH_ENA22_V  0x00000001U
#define SOC_ETM_CH_ENA22_S  22
/** SOC_ETM_CH_ENA23 : R/WTC/WTS; bitpos: [23]; default: 0;
 *  ch23 enable
 */
#define SOC_ETM_CH_ENA23    (BIT(23))
#define SOC_ETM_CH_ENA23_M  (SOC_ETM_CH_ENA23_V << SOC_ETM_CH_ENA23_S)
#define SOC_ETM_CH_ENA23_V  0x00000001U
#define SOC_ETM_CH_ENA23_S  23
/** SOC_ETM_CH_ENA24 : R/WTC/WTS; bitpos: [24]; default: 0;
 *  ch24 enable
 */
#define SOC_ETM_CH_ENA24    (BIT(24))
#define SOC_ETM_CH_ENA24_M  (SOC_ETM_CH_ENA24_V << SOC_ETM_CH_ENA24_S)
#define SOC_ETM_CH_ENA24_V  0x00000001U
#define SOC_ETM_CH_ENA24_S  24
/** SOC_ETM_CH_ENA25 : R/WTC/WTS; bitpos: [25]; default: 0;
 *  ch25 enable
 */
#define SOC_ETM_CH_ENA25    (BIT(25))
#define SOC_ETM_CH_ENA25_M  (SOC_ETM_CH_ENA25_V << SOC_ETM_CH_ENA25_S)
#define SOC_ETM_CH_ENA25_V  0x00000001U
#define SOC_ETM_CH_ENA25_S  25
/** SOC_ETM_CH_ENA26 : R/WTC/WTS; bitpos: [26]; default: 0;
 *  ch26 enable
 */
#define SOC_ETM_CH_ENA26    (BIT(26))
#define SOC_ETM_CH_ENA26_M  (SOC_ETM_CH_ENA26_V << SOC_ETM_CH_ENA26_S)
#define SOC_ETM_CH_ENA26_V  0x00000001U
#define SOC_ETM_CH_ENA26_S  26
/** SOC_ETM_CH_ENA27 : R/WTC/WTS; bitpos: [27]; default: 0;
 *  ch27 enable
 */
#define SOC_ETM_CH_ENA27    (BIT(27))
#define SOC_ETM_CH_ENA27_M  (SOC_ETM_CH_ENA27_V << SOC_ETM_CH_ENA27_S)
#define SOC_ETM_CH_ENA27_V  0x00000001U
#define SOC_ETM_CH_ENA27_S  27
/** SOC_ETM_CH_ENA28 : R/WTC/WTS; bitpos: [28]; default: 0;
 *  ch28 enable
 */
#define SOC_ETM_CH_ENA28    (BIT(28))
#define SOC_ETM_CH_ENA28_M  (SOC_ETM_CH_ENA28_V << SOC_ETM_CH_ENA28_S)
#define SOC_ETM_CH_ENA28_V  0x00000001U
#define SOC_ETM_CH_ENA28_S  28
/** SOC_ETM_CH_ENA29 : R/WTC/WTS; bitpos: [29]; default: 0;
 *  ch29 enable
 */
#define SOC_ETM_CH_ENA29    (BIT(29))
#define SOC_ETM_CH_ENA29_M  (SOC_ETM_CH_ENA29_V << SOC_ETM_CH_ENA29_S)
#define SOC_ETM_CH_ENA29_V  0x00000001U
#define SOC_ETM_CH_ENA29_S  29
/** SOC_ETM_CH_ENA30 : R/WTC/WTS; bitpos: [30]; default: 0;
 *  ch30 enable
 */
#define SOC_ETM_CH_ENA30    (BIT(30))
#define SOC_ETM_CH_ENA30_M  (SOC_ETM_CH_ENA30_V << SOC_ETM_CH_ENA30_S)
#define SOC_ETM_CH_ENA30_V  0x00000001U
#define SOC_ETM_CH_ENA30_S  30
/** SOC_ETM_CH_ENA31 : R/WTC/WTS; bitpos: [31]; default: 0;
 *  ch31 enable
 */
#define SOC_ETM_CH_ENA31    (BIT(31))
#define SOC_ETM_CH_ENA31_M  (SOC_ETM_CH_ENA31_V << SOC_ETM_CH_ENA31_S)
#define SOC_ETM_CH_ENA31_V  0x00000001U
#define SOC_ETM_CH_ENA31_S  31

/** SOC_ETM_CH_ENA_AD0_SET_REG register
 *  channel enable set register
 */
#define SOC_ETM_CH_ENA_AD0_SET_REG (DR_REG_SOC_ETM_BASE + 0x4)
/** SOC_ETM_CH_SET0 : WT; bitpos: [0]; default: 0;
 *  ch0 set
 */
#define SOC_ETM_CH_SET0    (BIT(0))
#define SOC_ETM_CH_SET0_M  (SOC_ETM_CH_SET0_V << SOC_ETM_CH_SET0_S)
#define SOC_ETM_CH_SET0_V  0x00000001U
#define SOC_ETM_CH_SET0_S  0
/** SOC_ETM_CH_SET1 : WT; bitpos: [1]; default: 0;
 *  ch1 set
 */
#define SOC_ETM_CH_SET1    (BIT(1))
#define SOC_ETM_CH_SET1_M  (SOC_ETM_CH_SET1_V << SOC_ETM_CH_SET1_S)
#define SOC_ETM_CH_SET1_V  0x00000001U
#define SOC_ETM_CH_SET1_S  1
/** SOC_ETM_CH_SET2 : WT; bitpos: [2]; default: 0;
 *  ch2 set
 */
#define SOC_ETM_CH_SET2    (BIT(2))
#define SOC_ETM_CH_SET2_M  (SOC_ETM_CH_SET2_V << SOC_ETM_CH_SET2_S)
#define SOC_ETM_CH_SET2_V  0x00000001U
#define SOC_ETM_CH_SET2_S  2
/** SOC_ETM_CH_SET3 : WT; bitpos: [3]; default: 0;
 *  ch3 set
 */
#define SOC_ETM_CH_SET3    (BIT(3))
#define SOC_ETM_CH_SET3_M  (SOC_ETM_CH_SET3_V << SOC_ETM_CH_SET3_S)
#define SOC_ETM_CH_SET3_V  0x00000001U
#define SOC_ETM_CH_SET3_S  3
/** SOC_ETM_CH_SET4 : WT; bitpos: [4]; default: 0;
 *  ch4 set
 */
#define SOC_ETM_CH_SET4    (BIT(4))
#define SOC_ETM_CH_SET4_M  (SOC_ETM_CH_SET4_V << SOC_ETM_CH_SET4_S)
#define SOC_ETM_CH_SET4_V  0x00000001U
#define SOC_ETM_CH_SET4_S  4
/** SOC_ETM_CH_SET5 : WT; bitpos: [5]; default: 0;
 *  ch5 set
 */
#define SOC_ETM_CH_SET5    (BIT(5))
#define SOC_ETM_CH_SET5_M  (SOC_ETM_CH_SET5_V << SOC_ETM_CH_SET5_S)
#define SOC_ETM_CH_SET5_V  0x00000001U
#define SOC_ETM_CH_SET5_S  5
/** SOC_ETM_CH_SET6 : WT; bitpos: [6]; default: 0;
 *  ch6 set
 */
#define SOC_ETM_CH_SET6    (BIT(6))
#define SOC_ETM_CH_SET6_M  (SOC_ETM_CH_SET6_V << SOC_ETM_CH_SET6_S)
#define SOC_ETM_CH_SET6_V  0x00000001U
#define SOC_ETM_CH_SET6_S  6
/** SOC_ETM_CH_SET7 : WT; bitpos: [7]; default: 0;
 *  ch7 set
 */
#define SOC_ETM_CH_SET7    (BIT(7))
#define SOC_ETM_CH_SET7_M  (SOC_ETM_CH_SET7_V << SOC_ETM_CH_SET7_S)
#define SOC_ETM_CH_SET7_V  0x00000001U
#define SOC_ETM_CH_SET7_S  7
/** SOC_ETM_CH_SET8 : WT; bitpos: [8]; default: 0;
 *  ch8 set
 */
#define SOC_ETM_CH_SET8    (BIT(8))
#define SOC_ETM_CH_SET8_M  (SOC_ETM_CH_SET8_V << SOC_ETM_CH_SET8_S)
#define SOC_ETM_CH_SET8_V  0x00000001U
#define SOC_ETM_CH_SET8_S  8
/** SOC_ETM_CH_SET9 : WT; bitpos: [9]; default: 0;
 *  ch9 set
 */
#define SOC_ETM_CH_SET9    (BIT(9))
#define SOC_ETM_CH_SET9_M  (SOC_ETM_CH_SET9_V << SOC_ETM_CH_SET9_S)
#define SOC_ETM_CH_SET9_V  0x00000001U
#define SOC_ETM_CH_SET9_S  9
/** SOC_ETM_CH_SET10 : WT; bitpos: [10]; default: 0;
 *  ch10 set
 */
#define SOC_ETM_CH_SET10    (BIT(10))
#define SOC_ETM_CH_SET10_M  (SOC_ETM_CH_SET10_V << SOC_ETM_CH_SET10_S)
#define SOC_ETM_CH_SET10_V  0x00000001U
#define SOC_ETM_CH_SET10_S  10
/** SOC_ETM_CH_SET11 : WT; bitpos: [11]; default: 0;
 *  ch11 set
 */
#define SOC_ETM_CH_SET11    (BIT(11))
#define SOC_ETM_CH_SET11_M  (SOC_ETM_CH_SET11_V << SOC_ETM_CH_SET11_S)
#define SOC_ETM_CH_SET11_V  0x00000001U
#define SOC_ETM_CH_SET11_S  11
/** SOC_ETM_CH_SET12 : WT; bitpos: [12]; default: 0;
 *  ch12 set
 */
#define SOC_ETM_CH_SET12    (BIT(12))
#define SOC_ETM_CH_SET12_M  (SOC_ETM_CH_SET12_V << SOC_ETM_CH_SET12_S)
#define SOC_ETM_CH_SET12_V  0x00000001U
#define SOC_ETM_CH_SET12_S  12
/** SOC_ETM_CH_SET13 : WT; bitpos: [13]; default: 0;
 *  ch13 set
 */
#define SOC_ETM_CH_SET13    (BIT(13))
#define SOC_ETM_CH_SET13_M  (SOC_ETM_CH_SET13_V << SOC_ETM_CH_SET13_S)
#define SOC_ETM_CH_SET13_V  0x00000001U
#define SOC_ETM_CH_SET13_S  13
/** SOC_ETM_CH_SET14 : WT; bitpos: [14]; default: 0;
 *  ch14 set
 */
#define SOC_ETM_CH_SET14    (BIT(14))
#define SOC_ETM_CH_SET14_M  (SOC_ETM_CH_SET14_V << SOC_ETM_CH_SET14_S)
#define SOC_ETM_CH_SET14_V  0x00000001U
#define SOC_ETM_CH_SET14_S  14
/** SOC_ETM_CH_SET15 : WT; bitpos: [15]; default: 0;
 *  ch15 set
 */
#define SOC_ETM_CH_SET15    (BIT(15))
#define SOC_ETM_CH_SET15_M  (SOC_ETM_CH_SET15_V << SOC_ETM_CH_SET15_S)
#define SOC_ETM_CH_SET15_V  0x00000001U
#define SOC_ETM_CH_SET15_S  15
/** SOC_ETM_CH_SET16 : WT; bitpos: [16]; default: 0;
 *  ch16 set
 */
#define SOC_ETM_CH_SET16    (BIT(16))
#define SOC_ETM_CH_SET16_M  (SOC_ETM_CH_SET16_V << SOC_ETM_CH_SET16_S)
#define SOC_ETM_CH_SET16_V  0x00000001U
#define SOC_ETM_CH_SET16_S  16
/** SOC_ETM_CH_SET17 : WT; bitpos: [17]; default: 0;
 *  ch17 set
 */
#define SOC_ETM_CH_SET17    (BIT(17))
#define SOC_ETM_CH_SET17_M  (SOC_ETM_CH_SET17_V << SOC_ETM_CH_SET17_S)
#define SOC_ETM_CH_SET17_V  0x00000001U
#define SOC_ETM_CH_SET17_S  17
/** SOC_ETM_CH_SET18 : WT; bitpos: [18]; default: 0;
 *  ch18 set
 */
#define SOC_ETM_CH_SET18    (BIT(18))
#define SOC_ETM_CH_SET18_M  (SOC_ETM_CH_SET18_V << SOC_ETM_CH_SET18_S)
#define SOC_ETM_CH_SET18_V  0x00000001U
#define SOC_ETM_CH_SET18_S  18
/** SOC_ETM_CH_SET19 : WT; bitpos: [19]; default: 0;
 *  ch19 set
 */
#define SOC_ETM_CH_SET19    (BIT(19))
#define SOC_ETM_CH_SET19_M  (SOC_ETM_CH_SET19_V << SOC_ETM_CH_SET19_S)
#define SOC_ETM_CH_SET19_V  0x00000001U
#define SOC_ETM_CH_SET19_S  19
/** SOC_ETM_CH_SET20 : WT; bitpos: [20]; default: 0;
 *  ch20 set
 */
#define SOC_ETM_CH_SET20    (BIT(20))
#define SOC_ETM_CH_SET20_M  (SOC_ETM_CH_SET20_V << SOC_ETM_CH_SET20_S)
#define SOC_ETM_CH_SET20_V  0x00000001U
#define SOC_ETM_CH_SET20_S  20
/** SOC_ETM_CH_SET21 : WT; bitpos: [21]; default: 0;
 *  ch21 set
 */
#define SOC_ETM_CH_SET21    (BIT(21))
#define SOC_ETM_CH_SET21_M  (SOC_ETM_CH_SET21_V << SOC_ETM_CH_SET21_S)
#define SOC_ETM_CH_SET21_V  0x00000001U
#define SOC_ETM_CH_SET21_S  21
/** SOC_ETM_CH_SET22 : WT; bitpos: [22]; default: 0;
 *  ch22 set
 */
#define SOC_ETM_CH_SET22    (BIT(22))
#define SOC_ETM_CH_SET22_M  (SOC_ETM_CH_SET22_V << SOC_ETM_CH_SET22_S)
#define SOC_ETM_CH_SET22_V  0x00000001U
#define SOC_ETM_CH_SET22_S  22
/** SOC_ETM_CH_SET23 : WT; bitpos: [23]; default: 0;
 *  ch23 set
 */
#define SOC_ETM_CH_SET23    (BIT(23))
#define SOC_ETM_CH_SET23_M  (SOC_ETM_CH_SET23_V << SOC_ETM_CH_SET23_S)
#define SOC_ETM_CH_SET23_V  0x00000001U
#define SOC_ETM_CH_SET23_S  23
/** SOC_ETM_CH_SET24 : WT; bitpos: [24]; default: 0;
 *  ch24 set
 */
#define SOC_ETM_CH_SET24    (BIT(24))
#define SOC_ETM_CH_SET24_M  (SOC_ETM_CH_SET24_V << SOC_ETM_CH_SET24_S)
#define SOC_ETM_CH_SET24_V  0x00000001U
#define SOC_ETM_CH_SET24_S  24
/** SOC_ETM_CH_SET25 : WT; bitpos: [25]; default: 0;
 *  ch25 set
 */
#define SOC_ETM_CH_SET25    (BIT(25))
#define SOC_ETM_CH_SET25_M  (SOC_ETM_CH_SET25_V << SOC_ETM_CH_SET25_S)
#define SOC_ETM_CH_SET25_V  0x00000001U
#define SOC_ETM_CH_SET25_S  25
/** SOC_ETM_CH_SET26 : WT; bitpos: [26]; default: 0;
 *  ch26 set
 */
#define SOC_ETM_CH_SET26    (BIT(26))
#define SOC_ETM_CH_SET26_M  (SOC_ETM_CH_SET26_V << SOC_ETM_CH_SET26_S)
#define SOC_ETM_CH_SET26_V  0x00000001U
#define SOC_ETM_CH_SET26_S  26
/** SOC_ETM_CH_SET27 : WT; bitpos: [27]; default: 0;
 *  ch27 set
 */
#define SOC_ETM_CH_SET27    (BIT(27))
#define SOC_ETM_CH_SET27_M  (SOC_ETM_CH_SET27_V << SOC_ETM_CH_SET27_S)
#define SOC_ETM_CH_SET27_V  0x00000001U
#define SOC_ETM_CH_SET27_S  27
/** SOC_ETM_CH_SET28 : WT; bitpos: [28]; default: 0;
 *  ch28 set
 */
#define SOC_ETM_CH_SET28    (BIT(28))
#define SOC_ETM_CH_SET28_M  (SOC_ETM_CH_SET28_V << SOC_ETM_CH_SET28_S)
#define SOC_ETM_CH_SET28_V  0x00000001U
#define SOC_ETM_CH_SET28_S  28
/** SOC_ETM_CH_SET29 : WT; bitpos: [29]; default: 0;
 *  ch29 set
 */
#define SOC_ETM_CH_SET29    (BIT(29))
#define SOC_ETM_CH_SET29_M  (SOC_ETM_CH_SET29_V << SOC_ETM_CH_SET29_S)
#define SOC_ETM_CH_SET29_V  0x00000001U
#define SOC_ETM_CH_SET29_S  29
/** SOC_ETM_CH_SET30 : WT; bitpos: [30]; default: 0;
 *  ch30 set
 */
#define SOC_ETM_CH_SET30    (BIT(30))
#define SOC_ETM_CH_SET30_M  (SOC_ETM_CH_SET30_V << SOC_ETM_CH_SET30_S)
#define SOC_ETM_CH_SET30_V  0x00000001U
#define SOC_ETM_CH_SET30_S  30
/** SOC_ETM_CH_SET31 : WT; bitpos: [31]; default: 0;
 *  ch31 set
 */
#define SOC_ETM_CH_SET31    (BIT(31))
#define SOC_ETM_CH_SET31_M  (SOC_ETM_CH_SET31_V << SOC_ETM_CH_SET31_S)
#define SOC_ETM_CH_SET31_V  0x00000001U
#define SOC_ETM_CH_SET31_S  31

/** SOC_ETM_CH_ENA_AD0_CLR_REG register
 *  channel enable clear register
 */
#define SOC_ETM_CH_ENA_AD0_CLR_REG (DR_REG_SOC_ETM_BASE + 0x8)
/** SOC_ETM_CH_CLR0 : WT; bitpos: [0]; default: 0;
 *  ch0 clear
 */
#define SOC_ETM_CH_CLR0    (BIT(0))
#define SOC_ETM_CH_CLR0_M  (SOC_ETM_CH_CLR0_V << SOC_ETM_CH_CLR0_S)
#define SOC_ETM_CH_CLR0_V  0x00000001U
#define SOC_ETM_CH_CLR0_S  0
/** SOC_ETM_CH_CLR1 : WT; bitpos: [1]; default: 0;
 *  ch1 clear
 */
#define SOC_ETM_CH_CLR1    (BIT(1))
#define SOC_ETM_CH_CLR1_M  (SOC_ETM_CH_CLR1_V << SOC_ETM_CH_CLR1_S)
#define SOC_ETM_CH_CLR1_V  0x00000001U
#define SOC_ETM_CH_CLR1_S  1
/** SOC_ETM_CH_CLR2 : WT; bitpos: [2]; default: 0;
 *  ch2 clear
 */
#define SOC_ETM_CH_CLR2    (BIT(2))
#define SOC_ETM_CH_CLR2_M  (SOC_ETM_CH_CLR2_V << SOC_ETM_CH_CLR2_S)
#define SOC_ETM_CH_CLR2_V  0x00000001U
#define SOC_ETM_CH_CLR2_S  2
/** SOC_ETM_CH_CLR3 : WT; bitpos: [3]; default: 0;
 *  ch3 clear
 */
#define SOC_ETM_CH_CLR3    (BIT(3))
#define SOC_ETM_CH_CLR3_M  (SOC_ETM_CH_CLR3_V << SOC_ETM_CH_CLR3_S)
#define SOC_ETM_CH_CLR3_V  0x00000001U
#define SOC_ETM_CH_CLR3_S  3
/** SOC_ETM_CH_CLR4 : WT; bitpos: [4]; default: 0;
 *  ch4 clear
 */
#define SOC_ETM_CH_CLR4    (BIT(4))
#define SOC_ETM_CH_CLR4_M  (SOC_ETM_CH_CLR4_V << SOC_ETM_CH_CLR4_S)
#define SOC_ETM_CH_CLR4_V  0x00000001U
#define SOC_ETM_CH_CLR4_S  4
/** SOC_ETM_CH_CLR5 : WT; bitpos: [5]; default: 0;
 *  ch5 clear
 */
#define SOC_ETM_CH_CLR5    (BIT(5))
#define SOC_ETM_CH_CLR5_M  (SOC_ETM_CH_CLR5_V << SOC_ETM_CH_CLR5_S)
#define SOC_ETM_CH_CLR5_V  0x00000001U
#define SOC_ETM_CH_CLR5_S  5
/** SOC_ETM_CH_CLR6 : WT; bitpos: [6]; default: 0;
 *  ch6 clear
 */
#define SOC_ETM_CH_CLR6    (BIT(6))
#define SOC_ETM_CH_CLR6_M  (SOC_ETM_CH_CLR6_V << SOC_ETM_CH_CLR6_S)
#define SOC_ETM_CH_CLR6_V  0x00000001U
#define SOC_ETM_CH_CLR6_S  6
/** SOC_ETM_CH_CLR7 : WT; bitpos: [7]; default: 0;
 *  ch7 clear
 */
#define SOC_ETM_CH_CLR7    (BIT(7))
#define SOC_ETM_CH_CLR7_M  (SOC_ETM_CH_CLR7_V << SOC_ETM_CH_CLR7_S)
#define SOC_ETM_CH_CLR7_V  0x00000001U
#define SOC_ETM_CH_CLR7_S  7
/** SOC_ETM_CH_CLR8 : WT; bitpos: [8]; default: 0;
 *  ch8 clear
 */
#define SOC_ETM_CH_CLR8    (BIT(8))
#define SOC_ETM_CH_CLR8_M  (SOC_ETM_CH_CLR8_V << SOC_ETM_CH_CLR8_S)
#define SOC_ETM_CH_CLR8_V  0x00000001U
#define SOC_ETM_CH_CLR8_S  8
/** SOC_ETM_CH_CLR9 : WT; bitpos: [9]; default: 0;
 *  ch9 clear
 */
#define SOC_ETM_CH_CLR9    (BIT(9))
#define SOC_ETM_CH_CLR9_M  (SOC_ETM_CH_CLR9_V << SOC_ETM_CH_CLR9_S)
#define SOC_ETM_CH_CLR9_V  0x00000001U
#define SOC_ETM_CH_CLR9_S  9
/** SOC_ETM_CH_CLR10 : WT; bitpos: [10]; default: 0;
 *  ch10 clear
 */
#define SOC_ETM_CH_CLR10    (BIT(10))
#define SOC_ETM_CH_CLR10_M  (SOC_ETM_CH_CLR10_V << SOC_ETM_CH_CLR10_S)
#define SOC_ETM_CH_CLR10_V  0x00000001U
#define SOC_ETM_CH_CLR10_S  10
/** SOC_ETM_CH_CLR11 : WT; bitpos: [11]; default: 0;
 *  ch11 clear
 */
#define SOC_ETM_CH_CLR11    (BIT(11))
#define SOC_ETM_CH_CLR11_M  (SOC_ETM_CH_CLR11_V << SOC_ETM_CH_CLR11_S)
#define SOC_ETM_CH_CLR11_V  0x00000001U
#define SOC_ETM_CH_CLR11_S  11
/** SOC_ETM_CH_CLR12 : WT; bitpos: [12]; default: 0;
 *  ch12 clear
 */
#define SOC_ETM_CH_CLR12    (BIT(12))
#define SOC_ETM_CH_CLR12_M  (SOC_ETM_CH_CLR12_V << SOC_ETM_CH_CLR12_S)
#define SOC_ETM_CH_CLR12_V  0x00000001U
#define SOC_ETM_CH_CLR12_S  12
/** SOC_ETM_CH_CLR13 : WT; bitpos: [13]; default: 0;
 *  ch13 clear
 */
#define SOC_ETM_CH_CLR13    (BIT(13))
#define SOC_ETM_CH_CLR13_M  (SOC_ETM_CH_CLR13_V << SOC_ETM_CH_CLR13_S)
#define SOC_ETM_CH_CLR13_V  0x00000001U
#define SOC_ETM_CH_CLR13_S  13
/** SOC_ETM_CH_CLR14 : WT; bitpos: [14]; default: 0;
 *  ch14 clear
 */
#define SOC_ETM_CH_CLR14    (BIT(14))
#define SOC_ETM_CH_CLR14_M  (SOC_ETM_CH_CLR14_V << SOC_ETM_CH_CLR14_S)
#define SOC_ETM_CH_CLR14_V  0x00000001U
#define SOC_ETM_CH_CLR14_S  14
/** SOC_ETM_CH_CLR15 : WT; bitpos: [15]; default: 0;
 *  ch15 clear
 */
#define SOC_ETM_CH_CLR15    (BIT(15))
#define SOC_ETM_CH_CLR15_M  (SOC_ETM_CH_CLR15_V << SOC_ETM_CH_CLR15_S)
#define SOC_ETM_CH_CLR15_V  0x00000001U
#define SOC_ETM_CH_CLR15_S  15
/** SOC_ETM_CH_CLR16 : WT; bitpos: [16]; default: 0;
 *  ch16 clear
 */
#define SOC_ETM_CH_CLR16    (BIT(16))
#define SOC_ETM_CH_CLR16_M  (SOC_ETM_CH_CLR16_V << SOC_ETM_CH_CLR16_S)
#define SOC_ETM_CH_CLR16_V  0x00000001U
#define SOC_ETM_CH_CLR16_S  16
/** SOC_ETM_CH_CLR17 : WT; bitpos: [17]; default: 0;
 *  ch17 clear
 */
#define SOC_ETM_CH_CLR17    (BIT(17))
#define SOC_ETM_CH_CLR17_M  (SOC_ETM_CH_CLR17_V << SOC_ETM_CH_CLR17_S)
#define SOC_ETM_CH_CLR17_V  0x00000001U
#define SOC_ETM_CH_CLR17_S  17
/** SOC_ETM_CH_CLR18 : WT; bitpos: [18]; default: 0;
 *  ch18 clear
 */
#define SOC_ETM_CH_CLR18    (BIT(18))
#define SOC_ETM_CH_CLR18_M  (SOC_ETM_CH_CLR18_V << SOC_ETM_CH_CLR18_S)
#define SOC_ETM_CH_CLR18_V  0x00000001U
#define SOC_ETM_CH_CLR18_S  18
/** SOC_ETM_CH_CLR19 : WT; bitpos: [19]; default: 0;
 *  ch19 clear
 */
#define SOC_ETM_CH_CLR19    (BIT(19))
#define SOC_ETM_CH_CLR19_M  (SOC_ETM_CH_CLR19_V << SOC_ETM_CH_CLR19_S)
#define SOC_ETM_CH_CLR19_V  0x00000001U
#define SOC_ETM_CH_CLR19_S  19
/** SOC_ETM_CH_CLR20 : WT; bitpos: [20]; default: 0;
 *  ch20 clear
 */
#define SOC_ETM_CH_CLR20    (BIT(20))
#define SOC_ETM_CH_CLR20_M  (SOC_ETM_CH_CLR20_V << SOC_ETM_CH_CLR20_S)
#define SOC_ETM_CH_CLR20_V  0x00000001U
#define SOC_ETM_CH_CLR20_S  20
/** SOC_ETM_CH_CLR21 : WT; bitpos: [21]; default: 0;
 *  ch21 clear
 */
#define SOC_ETM_CH_CLR21    (BIT(21))
#define SOC_ETM_CH_CLR21_M  (SOC_ETM_CH_CLR21_V << SOC_ETM_CH_CLR21_S)
#define SOC_ETM_CH_CLR21_V  0x00000001U
#define SOC_ETM_CH_CLR21_S  21
/** SOC_ETM_CH_CLR22 : WT; bitpos: [22]; default: 0;
 *  ch22 clear
 */
#define SOC_ETM_CH_CLR22    (BIT(22))
#define SOC_ETM_CH_CLR22_M  (SOC_ETM_CH_CLR22_V << SOC_ETM_CH_CLR22_S)
#define SOC_ETM_CH_CLR22_V  0x00000001U
#define SOC_ETM_CH_CLR22_S  22
/** SOC_ETM_CH_CLR23 : WT; bitpos: [23]; default: 0;
 *  ch23 clear
 */
#define SOC_ETM_CH_CLR23    (BIT(23))
#define SOC_ETM_CH_CLR23_M  (SOC_ETM_CH_CLR23_V << SOC_ETM_CH_CLR23_S)
#define SOC_ETM_CH_CLR23_V  0x00000001U
#define SOC_ETM_CH_CLR23_S  23
/** SOC_ETM_CH_CLR24 : WT; bitpos: [24]; default: 0;
 *  ch24 clear
 */
#define SOC_ETM_CH_CLR24    (BIT(24))
#define SOC_ETM_CH_CLR24_M  (SOC_ETM_CH_CLR24_V << SOC_ETM_CH_CLR24_S)
#define SOC_ETM_CH_CLR24_V  0x00000001U
#define SOC_ETM_CH_CLR24_S  24
/** SOC_ETM_CH_CLR25 : WT; bitpos: [25]; default: 0;
 *  ch25 clear
 */
#define SOC_ETM_CH_CLR25    (BIT(25))
#define SOC_ETM_CH_CLR25_M  (SOC_ETM_CH_CLR25_V << SOC_ETM_CH_CLR25_S)
#define SOC_ETM_CH_CLR25_V  0x00000001U
#define SOC_ETM_CH_CLR25_S  25
/** SOC_ETM_CH_CLR26 : WT; bitpos: [26]; default: 0;
 *  ch26 clear
 */
#define SOC_ETM_CH_CLR26    (BIT(26))
#define SOC_ETM_CH_CLR26_M  (SOC_ETM_CH_CLR26_V << SOC_ETM_CH_CLR26_S)
#define SOC_ETM_CH_CLR26_V  0x00000001U
#define SOC_ETM_CH_CLR26_S  26
/** SOC_ETM_CH_CLR27 : WT; bitpos: [27]; default: 0;
 *  ch27 clear
 */
#define SOC_ETM_CH_CLR27    (BIT(27))
#define SOC_ETM_CH_CLR27_M  (SOC_ETM_CH_CLR27_V << SOC_ETM_CH_CLR27_S)
#define SOC_ETM_CH_CLR27_V  0x00000001U
#define SOC_ETM_CH_CLR27_S  27
/** SOC_ETM_CH_CLR28 : WT; bitpos: [28]; default: 0;
 *  ch28 clear
 */
#define SOC_ETM_CH_CLR28    (BIT(28))
#define SOC_ETM_CH_CLR28_M  (SOC_ETM_CH_CLR28_V << SOC_ETM_CH_CLR28_S)
#define SOC_ETM_CH_CLR28_V  0x00000001U
#define SOC_ETM_CH_CLR28_S  28
/** SOC_ETM_CH_CLR29 : WT; bitpos: [29]; default: 0;
 *  ch29 clear
 */
#define SOC_ETM_CH_CLR29    (BIT(29))
#define SOC_ETM_CH_CLR29_M  (SOC_ETM_CH_CLR29_V << SOC_ETM_CH_CLR29_S)
#define SOC_ETM_CH_CLR29_V  0x00000001U
#define SOC_ETM_CH_CLR29_S  29
/** SOC_ETM_CH_CLR30 : WT; bitpos: [30]; default: 0;
 *  ch30 clear
 */
#define SOC_ETM_CH_CLR30    (BIT(30))
#define SOC_ETM_CH_CLR30_M  (SOC_ETM_CH_CLR30_V << SOC_ETM_CH_CLR30_S)
#define SOC_ETM_CH_CLR30_V  0x00000001U
#define SOC_ETM_CH_CLR30_S  30
/** SOC_ETM_CH_CLR31 : WT; bitpos: [31]; default: 0;
 *  ch31 clear
 */
#define SOC_ETM_CH_CLR31    (BIT(31))
#define SOC_ETM_CH_CLR31_M  (SOC_ETM_CH_CLR31_V << SOC_ETM_CH_CLR31_S)
#define SOC_ETM_CH_CLR31_V  0x00000001U
#define SOC_ETM_CH_CLR31_S  31

/** SOC_ETM_CH_ENA_AD1_REG register
 *  channel enable register
 */
#define SOC_ETM_CH_ENA_AD1_REG (DR_REG_SOC_ETM_BASE + 0xc)
/** SOC_ETM_CH_ENA32 : R/WTC/WTS; bitpos: [0]; default: 0;
 *  ch32 enable
 */
#define SOC_ETM_CH_ENA32    (BIT(0))
#define SOC_ETM_CH_ENA32_M  (SOC_ETM_CH_ENA32_V << SOC_ETM_CH_ENA32_S)
#define SOC_ETM_CH_ENA32_V  0x00000001U
#define SOC_ETM_CH_ENA32_S  0
/** SOC_ETM_CH_ENA33 : R/WTC/WTS; bitpos: [1]; default: 0;
 *  ch33 enable
 */
#define SOC_ETM_CH_ENA33    (BIT(1))
#define SOC_ETM_CH_ENA33_M  (SOC_ETM_CH_ENA33_V << SOC_ETM_CH_ENA33_S)
#define SOC_ETM_CH_ENA33_V  0x00000001U
#define SOC_ETM_CH_ENA33_S  1
/** SOC_ETM_CH_ENA34 : R/WTC/WTS; bitpos: [2]; default: 0;
 *  ch34 enable
 */
#define SOC_ETM_CH_ENA34    (BIT(2))
#define SOC_ETM_CH_ENA34_M  (SOC_ETM_CH_ENA34_V << SOC_ETM_CH_ENA34_S)
#define SOC_ETM_CH_ENA34_V  0x00000001U
#define SOC_ETM_CH_ENA34_S  2
/** SOC_ETM_CH_ENA35 : R/WTC/WTS; bitpos: [3]; default: 0;
 *  ch35 enable
 */
#define SOC_ETM_CH_ENA35    (BIT(3))
#define SOC_ETM_CH_ENA35_M  (SOC_ETM_CH_ENA35_V << SOC_ETM_CH_ENA35_S)
#define SOC_ETM_CH_ENA35_V  0x00000001U
#define SOC_ETM_CH_ENA35_S  3
/** SOC_ETM_CH_ENA36 : R/WTC/WTS; bitpos: [4]; default: 0;
 *  ch36 enable
 */
#define SOC_ETM_CH_ENA36    (BIT(4))
#define SOC_ETM_CH_ENA36_M  (SOC_ETM_CH_ENA36_V << SOC_ETM_CH_ENA36_S)
#define SOC_ETM_CH_ENA36_V  0x00000001U
#define SOC_ETM_CH_ENA36_S  4
/** SOC_ETM_CH_ENA37 : R/WTC/WTS; bitpos: [5]; default: 0;
 *  ch37 enable
 */
#define SOC_ETM_CH_ENA37    (BIT(5))
#define SOC_ETM_CH_ENA37_M  (SOC_ETM_CH_ENA37_V << SOC_ETM_CH_ENA37_S)
#define SOC_ETM_CH_ENA37_V  0x00000001U
#define SOC_ETM_CH_ENA37_S  5
/** SOC_ETM_CH_ENA38 : R/WTC/WTS; bitpos: [6]; default: 0;
 *  ch38 enable
 */
#define SOC_ETM_CH_ENA38    (BIT(6))
#define SOC_ETM_CH_ENA38_M  (SOC_ETM_CH_ENA38_V << SOC_ETM_CH_ENA38_S)
#define SOC_ETM_CH_ENA38_V  0x00000001U
#define SOC_ETM_CH_ENA38_S  6
/** SOC_ETM_CH_ENA39 : R/WTC/WTS; bitpos: [7]; default: 0;
 *  ch39 enable
 */
#define SOC_ETM_CH_ENA39    (BIT(7))
#define SOC_ETM_CH_ENA39_M  (SOC_ETM_CH_ENA39_V << SOC_ETM_CH_ENA39_S)
#define SOC_ETM_CH_ENA39_V  0x00000001U
#define SOC_ETM_CH_ENA39_S  7
/** SOC_ETM_CH_ENA40 : R/WTC/WTS; bitpos: [8]; default: 0;
 *  ch40 enable
 */
#define SOC_ETM_CH_ENA40    (BIT(8))
#define SOC_ETM_CH_ENA40_M  (SOC_ETM_CH_ENA40_V << SOC_ETM_CH_ENA40_S)
#define SOC_ETM_CH_ENA40_V  0x00000001U
#define SOC_ETM_CH_ENA40_S  8
/** SOC_ETM_CH_ENA41 : R/WTC/WTS; bitpos: [9]; default: 0;
 *  ch41 enable
 */
#define SOC_ETM_CH_ENA41    (BIT(9))
#define SOC_ETM_CH_ENA41_M  (SOC_ETM_CH_ENA41_V << SOC_ETM_CH_ENA41_S)
#define SOC_ETM_CH_ENA41_V  0x00000001U
#define SOC_ETM_CH_ENA41_S  9
/** SOC_ETM_CH_ENA42 : R/WTC/WTS; bitpos: [10]; default: 0;
 *  ch42 enable
 */
#define SOC_ETM_CH_ENA42    (BIT(10))
#define SOC_ETM_CH_ENA42_M  (SOC_ETM_CH_ENA42_V << SOC_ETM_CH_ENA42_S)
#define SOC_ETM_CH_ENA42_V  0x00000001U
#define SOC_ETM_CH_ENA42_S  10
/** SOC_ETM_CH_ENA43 : R/WTC/WTS; bitpos: [11]; default: 0;
 *  ch43 enable
 */
#define SOC_ETM_CH_ENA43    (BIT(11))
#define SOC_ETM_CH_ENA43_M  (SOC_ETM_CH_ENA43_V << SOC_ETM_CH_ENA43_S)
#define SOC_ETM_CH_ENA43_V  0x00000001U
#define SOC_ETM_CH_ENA43_S  11
/** SOC_ETM_CH_ENA44 : R/WTC/WTS; bitpos: [12]; default: 0;
 *  ch44 enable
 */
#define SOC_ETM_CH_ENA44    (BIT(12))
#define SOC_ETM_CH_ENA44_M  (SOC_ETM_CH_ENA44_V << SOC_ETM_CH_ENA44_S)
#define SOC_ETM_CH_ENA44_V  0x00000001U
#define SOC_ETM_CH_ENA44_S  12
/** SOC_ETM_CH_ENA45 : R/WTC/WTS; bitpos: [13]; default: 0;
 *  ch45 enable
 */
#define SOC_ETM_CH_ENA45    (BIT(13))
#define SOC_ETM_CH_ENA45_M  (SOC_ETM_CH_ENA45_V << SOC_ETM_CH_ENA45_S)
#define SOC_ETM_CH_ENA45_V  0x00000001U
#define SOC_ETM_CH_ENA45_S  13
/** SOC_ETM_CH_ENA46 : R/WTC/WTS; bitpos: [14]; default: 0;
 *  ch46 enable
 */
#define SOC_ETM_CH_ENA46    (BIT(14))
#define SOC_ETM_CH_ENA46_M  (SOC_ETM_CH_ENA46_V << SOC_ETM_CH_ENA46_S)
#define SOC_ETM_CH_ENA46_V  0x00000001U
#define SOC_ETM_CH_ENA46_S  14
/** SOC_ETM_CH_ENA47 : R/WTC/WTS; bitpos: [15]; default: 0;
 *  ch47 enable
 */
#define SOC_ETM_CH_ENA47    (BIT(15))
#define SOC_ETM_CH_ENA47_M  (SOC_ETM_CH_ENA47_V << SOC_ETM_CH_ENA47_S)
#define SOC_ETM_CH_ENA47_V  0x00000001U
#define SOC_ETM_CH_ENA47_S  15
/** SOC_ETM_CH_ENA48 : R/WTC/WTS; bitpos: [16]; default: 0;
 *  ch48 enable
 */
#define SOC_ETM_CH_ENA48    (BIT(16))
#define SOC_ETM_CH_ENA48_M  (SOC_ETM_CH_ENA48_V << SOC_ETM_CH_ENA48_S)
#define SOC_ETM_CH_ENA48_V  0x00000001U
#define SOC_ETM_CH_ENA48_S  16
/** SOC_ETM_CH_ENA49 : R/WTC/WTS; bitpos: [17]; default: 0;
 *  ch49 enable
 */
#define SOC_ETM_CH_ENA49    (BIT(17))
#define SOC_ETM_CH_ENA49_M  (SOC_ETM_CH_ENA49_V << SOC_ETM_CH_ENA49_S)
#define SOC_ETM_CH_ENA49_V  0x00000001U
#define SOC_ETM_CH_ENA49_S  17

/** SOC_ETM_CH_ENA_AD1_SET_REG register
 *  channel enable set register
 */
#define SOC_ETM_CH_ENA_AD1_SET_REG (DR_REG_SOC_ETM_BASE + 0x10)
/** SOC_ETM_CH_SET32 : WT; bitpos: [0]; default: 0;
 *  ch32 set
 */
#define SOC_ETM_CH_SET32    (BIT(0))
#define SOC_ETM_CH_SET32_M  (SOC_ETM_CH_SET32_V << SOC_ETM_CH_SET32_S)
#define SOC_ETM_CH_SET32_V  0x00000001U
#define SOC_ETM_CH_SET32_S  0
/** SOC_ETM_CH_SET33 : WT; bitpos: [1]; default: 0;
 *  ch33 set
 */
#define SOC_ETM_CH_SET33    (BIT(1))
#define SOC_ETM_CH_SET33_M  (SOC_ETM_CH_SET33_V << SOC_ETM_CH_SET33_S)
#define SOC_ETM_CH_SET33_V  0x00000001U
#define SOC_ETM_CH_SET33_S  1
/** SOC_ETM_CH_SET34 : WT; bitpos: [2]; default: 0;
 *  ch34 set
 */
#define SOC_ETM_CH_SET34    (BIT(2))
#define SOC_ETM_CH_SET34_M  (SOC_ETM_CH_SET34_V << SOC_ETM_CH_SET34_S)
#define SOC_ETM_CH_SET34_V  0x00000001U
#define SOC_ETM_CH_SET34_S  2
/** SOC_ETM_CH_SET35 : WT; bitpos: [3]; default: 0;
 *  ch35 set
 */
#define SOC_ETM_CH_SET35    (BIT(3))
#define SOC_ETM_CH_SET35_M  (SOC_ETM_CH_SET35_V << SOC_ETM_CH_SET35_S)
#define SOC_ETM_CH_SET35_V  0x00000001U
#define SOC_ETM_CH_SET35_S  3
/** SOC_ETM_CH_SET36 : WT; bitpos: [4]; default: 0;
 *  ch36 set
 */
#define SOC_ETM_CH_SET36    (BIT(4))
#define SOC_ETM_CH_SET36_M  (SOC_ETM_CH_SET36_V << SOC_ETM_CH_SET36_S)
#define SOC_ETM_CH_SET36_V  0x00000001U
#define SOC_ETM_CH_SET36_S  4
/** SOC_ETM_CH_SET37 : WT; bitpos: [5]; default: 0;
 *  ch37 set
 */
#define SOC_ETM_CH_SET37    (BIT(5))
#define SOC_ETM_CH_SET37_M  (SOC_ETM_CH_SET37_V << SOC_ETM_CH_SET37_S)
#define SOC_ETM_CH_SET37_V  0x00000001U
#define SOC_ETM_CH_SET37_S  5
/** SOC_ETM_CH_SET38 : WT; bitpos: [6]; default: 0;
 *  ch38 set
 */
#define SOC_ETM_CH_SET38    (BIT(6))
#define SOC_ETM_CH_SET38_M  (SOC_ETM_CH_SET38_V << SOC_ETM_CH_SET38_S)
#define SOC_ETM_CH_SET38_V  0x00000001U
#define SOC_ETM_CH_SET38_S  6
/** SOC_ETM_CH_SET39 : WT; bitpos: [7]; default: 0;
 *  ch39 set
 */
#define SOC_ETM_CH_SET39    (BIT(7))
#define SOC_ETM_CH_SET39_M  (SOC_ETM_CH_SET39_V << SOC_ETM_CH_SET39_S)
#define SOC_ETM_CH_SET39_V  0x00000001U
#define SOC_ETM_CH_SET39_S  7
/** SOC_ETM_CH_SET40 : WT; bitpos: [8]; default: 0;
 *  ch40 set
 */
#define SOC_ETM_CH_SET40    (BIT(8))
#define SOC_ETM_CH_SET40_M  (SOC_ETM_CH_SET40_V << SOC_ETM_CH_SET40_S)
#define SOC_ETM_CH_SET40_V  0x00000001U
#define SOC_ETM_CH_SET40_S  8
/** SOC_ETM_CH_SET41 : WT; bitpos: [9]; default: 0;
 *  ch41 set
 */
#define SOC_ETM_CH_SET41    (BIT(9))
#define SOC_ETM_CH_SET41_M  (SOC_ETM_CH_SET41_V << SOC_ETM_CH_SET41_S)
#define SOC_ETM_CH_SET41_V  0x00000001U
#define SOC_ETM_CH_SET41_S  9
/** SOC_ETM_CH_SET42 : WT; bitpos: [10]; default: 0;
 *  ch42 set
 */
#define SOC_ETM_CH_SET42    (BIT(10))
#define SOC_ETM_CH_SET42_M  (SOC_ETM_CH_SET42_V << SOC_ETM_CH_SET42_S)
#define SOC_ETM_CH_SET42_V  0x00000001U
#define SOC_ETM_CH_SET42_S  10
/** SOC_ETM_CH_SET43 : WT; bitpos: [11]; default: 0;
 *  ch43 set
 */
#define SOC_ETM_CH_SET43    (BIT(11))
#define SOC_ETM_CH_SET43_M  (SOC_ETM_CH_SET43_V << SOC_ETM_CH_SET43_S)
#define SOC_ETM_CH_SET43_V  0x00000001U
#define SOC_ETM_CH_SET43_S  11
/** SOC_ETM_CH_SET44 : WT; bitpos: [12]; default: 0;
 *  ch44 set
 */
#define SOC_ETM_CH_SET44    (BIT(12))
#define SOC_ETM_CH_SET44_M  (SOC_ETM_CH_SET44_V << SOC_ETM_CH_SET44_S)
#define SOC_ETM_CH_SET44_V  0x00000001U
#define SOC_ETM_CH_SET44_S  12
/** SOC_ETM_CH_SET45 : WT; bitpos: [13]; default: 0;
 *  ch45 set
 */
#define SOC_ETM_CH_SET45    (BIT(13))
#define SOC_ETM_CH_SET45_M  (SOC_ETM_CH_SET45_V << SOC_ETM_CH_SET45_S)
#define SOC_ETM_CH_SET45_V  0x00000001U
#define SOC_ETM_CH_SET45_S  13
/** SOC_ETM_CH_SET46 : WT; bitpos: [14]; default: 0;
 *  ch46 set
 */
#define SOC_ETM_CH_SET46    (BIT(14))
#define SOC_ETM_CH_SET46_M  (SOC_ETM_CH_SET46_V << SOC_ETM_CH_SET46_S)
#define SOC_ETM_CH_SET46_V  0x00000001U
#define SOC_ETM_CH_SET46_S  14
/** SOC_ETM_CH_SET47 : WT; bitpos: [15]; default: 0;
 *  ch47 set
 */
#define SOC_ETM_CH_SET47    (BIT(15))
#define SOC_ETM_CH_SET47_M  (SOC_ETM_CH_SET47_V << SOC_ETM_CH_SET47_S)
#define SOC_ETM_CH_SET47_V  0x00000001U
#define SOC_ETM_CH_SET47_S  15
/** SOC_ETM_CH_SET48 : WT; bitpos: [16]; default: 0;
 *  ch48 set
 */
#define SOC_ETM_CH_SET48    (BIT(16))
#define SOC_ETM_CH_SET48_M  (SOC_ETM_CH_SET48_V << SOC_ETM_CH_SET48_S)
#define SOC_ETM_CH_SET48_V  0x00000001U
#define SOC_ETM_CH_SET48_S  16
/** SOC_ETM_CH_SET49 : WT; bitpos: [17]; default: 0;
 *  ch49 set
 */
#define SOC_ETM_CH_SET49    (BIT(17))
#define SOC_ETM_CH_SET49_M  (SOC_ETM_CH_SET49_V << SOC_ETM_CH_SET49_S)
#define SOC_ETM_CH_SET49_V  0x00000001U
#define SOC_ETM_CH_SET49_S  17

/** SOC_ETM_CH_ENA_AD1_CLR_REG register
 *  channel enable clear register
 */
#define SOC_ETM_CH_ENA_AD1_CLR_REG (DR_REG_SOC_ETM_BASE + 0x14)
/** SOC_ETM_CH_CLR32 : WT; bitpos: [0]; default: 0;
 *  ch32 clear
 */
#define SOC_ETM_CH_CLR32    (BIT(0))
#define SOC_ETM_CH_CLR32_M  (SOC_ETM_CH_CLR32_V << SOC_ETM_CH_CLR32_S)
#define SOC_ETM_CH_CLR32_V  0x00000001U
#define SOC_ETM_CH_CLR32_S  0
/** SOC_ETM_CH_CLR33 : WT; bitpos: [1]; default: 0;
 *  ch33 clear
 */
#define SOC_ETM_CH_CLR33    (BIT(1))
#define SOC_ETM_CH_CLR33_M  (SOC_ETM_CH_CLR33_V << SOC_ETM_CH_CLR33_S)
#define SOC_ETM_CH_CLR33_V  0x00000001U
#define SOC_ETM_CH_CLR33_S  1
/** SOC_ETM_CH_CLR34 : WT; bitpos: [2]; default: 0;
 *  ch34 clear
 */
#define SOC_ETM_CH_CLR34    (BIT(2))
#define SOC_ETM_CH_CLR34_M  (SOC_ETM_CH_CLR34_V << SOC_ETM_CH_CLR34_S)
#define SOC_ETM_CH_CLR34_V  0x00000001U
#define SOC_ETM_CH_CLR34_S  2
/** SOC_ETM_CH_CLR35 : WT; bitpos: [3]; default: 0;
 *  ch35 clear
 */
#define SOC_ETM_CH_CLR35    (BIT(3))
#define SOC_ETM_CH_CLR35_M  (SOC_ETM_CH_CLR35_V << SOC_ETM_CH_CLR35_S)
#define SOC_ETM_CH_CLR35_V  0x00000001U
#define SOC_ETM_CH_CLR35_S  3
/** SOC_ETM_CH_CLR36 : WT; bitpos: [4]; default: 0;
 *  ch36 clear
 */
#define SOC_ETM_CH_CLR36    (BIT(4))
#define SOC_ETM_CH_CLR36_M  (SOC_ETM_CH_CLR36_V << SOC_ETM_CH_CLR36_S)
#define SOC_ETM_CH_CLR36_V  0x00000001U
#define SOC_ETM_CH_CLR36_S  4
/** SOC_ETM_CH_CLR37 : WT; bitpos: [5]; default: 0;
 *  ch37 clear
 */
#define SOC_ETM_CH_CLR37    (BIT(5))
#define SOC_ETM_CH_CLR37_M  (SOC_ETM_CH_CLR37_V << SOC_ETM_CH_CLR37_S)
#define SOC_ETM_CH_CLR37_V  0x00000001U
#define SOC_ETM_CH_CLR37_S  5
/** SOC_ETM_CH_CLR38 : WT; bitpos: [6]; default: 0;
 *  ch38 clear
 */
#define SOC_ETM_CH_CLR38    (BIT(6))
#define SOC_ETM_CH_CLR38_M  (SOC_ETM_CH_CLR38_V << SOC_ETM_CH_CLR38_S)
#define SOC_ETM_CH_CLR38_V  0x00000001U
#define SOC_ETM_CH_CLR38_S  6
/** SOC_ETM_CH_CLR39 : WT; bitpos: [7]; default: 0;
 *  ch39 clear
 */
#define SOC_ETM_CH_CLR39    (BIT(7))
#define SOC_ETM_CH_CLR39_M  (SOC_ETM_CH_CLR39_V << SOC_ETM_CH_CLR39_S)
#define SOC_ETM_CH_CLR39_V  0x00000001U
#define SOC_ETM_CH_CLR39_S  7
/** SOC_ETM_CH_CLR40 : WT; bitpos: [8]; default: 0;
 *  ch40 clear
 */
#define SOC_ETM_CH_CLR40    (BIT(8))
#define SOC_ETM_CH_CLR40_M  (SOC_ETM_CH_CLR40_V << SOC_ETM_CH_CLR40_S)
#define SOC_ETM_CH_CLR40_V  0x00000001U
#define SOC_ETM_CH_CLR40_S  8
/** SOC_ETM_CH_CLR41 : WT; bitpos: [9]; default: 0;
 *  ch41 clear
 */
#define SOC_ETM_CH_CLR41    (BIT(9))
#define SOC_ETM_CH_CLR41_M  (SOC_ETM_CH_CLR41_V << SOC_ETM_CH_CLR41_S)
#define SOC_ETM_CH_CLR41_V  0x00000001U
#define SOC_ETM_CH_CLR41_S  9
/** SOC_ETM_CH_CLR42 : WT; bitpos: [10]; default: 0;
 *  ch42 clear
 */
#define SOC_ETM_CH_CLR42    (BIT(10))
#define SOC_ETM_CH_CLR42_M  (SOC_ETM_CH_CLR42_V << SOC_ETM_CH_CLR42_S)
#define SOC_ETM_CH_CLR42_V  0x00000001U
#define SOC_ETM_CH_CLR42_S  10
/** SOC_ETM_CH_CLR43 : WT; bitpos: [11]; default: 0;
 *  ch43 clear
 */
#define SOC_ETM_CH_CLR43    (BIT(11))
#define SOC_ETM_CH_CLR43_M  (SOC_ETM_CH_CLR43_V << SOC_ETM_CH_CLR43_S)
#define SOC_ETM_CH_CLR43_V  0x00000001U
#define SOC_ETM_CH_CLR43_S  11
/** SOC_ETM_CH_CLR44 : WT; bitpos: [12]; default: 0;
 *  ch44 clear
 */
#define SOC_ETM_CH_CLR44    (BIT(12))
#define SOC_ETM_CH_CLR44_M  (SOC_ETM_CH_CLR44_V << SOC_ETM_CH_CLR44_S)
#define SOC_ETM_CH_CLR44_V  0x00000001U
#define SOC_ETM_CH_CLR44_S  12
/** SOC_ETM_CH_CLR45 : WT; bitpos: [13]; default: 0;
 *  ch45 clear
 */
#define SOC_ETM_CH_CLR45    (BIT(13))
#define SOC_ETM_CH_CLR45_M  (SOC_ETM_CH_CLR45_V << SOC_ETM_CH_CLR45_S)
#define SOC_ETM_CH_CLR45_V  0x00000001U
#define SOC_ETM_CH_CLR45_S  13
/** SOC_ETM_CH_CLR46 : WT; bitpos: [14]; default: 0;
 *  ch46 clear
 */
#define SOC_ETM_CH_CLR46    (BIT(14))
#define SOC_ETM_CH_CLR46_M  (SOC_ETM_CH_CLR46_V << SOC_ETM_CH_CLR46_S)
#define SOC_ETM_CH_CLR46_V  0x00000001U
#define SOC_ETM_CH_CLR46_S  14
/** SOC_ETM_CH_CLR47 : WT; bitpos: [15]; default: 0;
 *  ch47 clear
 */
#define SOC_ETM_CH_CLR47    (BIT(15))
#define SOC_ETM_CH_CLR47_M  (SOC_ETM_CH_CLR47_V << SOC_ETM_CH_CLR47_S)
#define SOC_ETM_CH_CLR47_V  0x00000001U
#define SOC_ETM_CH_CLR47_S  15
/** SOC_ETM_CH_CLR48 : WT; bitpos: [16]; default: 0;
 *  ch48 clear
 */
#define SOC_ETM_CH_CLR48    (BIT(16))
#define SOC_ETM_CH_CLR48_M  (SOC_ETM_CH_CLR48_V << SOC_ETM_CH_CLR48_S)
#define SOC_ETM_CH_CLR48_V  0x00000001U
#define SOC_ETM_CH_CLR48_S  16
/** SOC_ETM_CH_CLR49 : WT; bitpos: [17]; default: 0;
 *  ch49 clear
 */
#define SOC_ETM_CH_CLR49    (BIT(17))
#define SOC_ETM_CH_CLR49_M  (SOC_ETM_CH_CLR49_V << SOC_ETM_CH_CLR49_S)
#define SOC_ETM_CH_CLR49_V  0x00000001U
#define SOC_ETM_CH_CLR49_S  17

/** SOC_ETM_CH0_EVT_ID_REG register
 *  channel0 event id register
 */
#define SOC_ETM_CH0_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x18)
/** SOC_ETM_CH0_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch0_evt_id
 */
#define SOC_ETM_CH0_EVT_ID    0x000000FFU
#define SOC_ETM_CH0_EVT_ID_M  (SOC_ETM_CH0_EVT_ID_V << SOC_ETM_CH0_EVT_ID_S)
#define SOC_ETM_CH0_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH0_EVT_ID_S  0

/** SOC_ETM_CH0_TASK_ID_REG register
 *  channel0 task id register
 */
#define SOC_ETM_CH0_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x1c)
/** SOC_ETM_CH0_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch0_task_id
 */
#define SOC_ETM_CH0_TASK_ID    0x000000FFU
#define SOC_ETM_CH0_TASK_ID_M  (SOC_ETM_CH0_TASK_ID_V << SOC_ETM_CH0_TASK_ID_S)
#define SOC_ETM_CH0_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH0_TASK_ID_S  0

/** SOC_ETM_CH1_EVT_ID_REG register
 *  channel1 event id register
 */
#define SOC_ETM_CH1_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x20)
/** SOC_ETM_CH1_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch1_evt_id
 */
#define SOC_ETM_CH1_EVT_ID    0x000000FFU
#define SOC_ETM_CH1_EVT_ID_M  (SOC_ETM_CH1_EVT_ID_V << SOC_ETM_CH1_EVT_ID_S)
#define SOC_ETM_CH1_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH1_EVT_ID_S  0

/** SOC_ETM_CH1_TASK_ID_REG register
 *  channel1 task id register
 */
#define SOC_ETM_CH1_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x24)
/** SOC_ETM_CH1_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch1_task_id
 */
#define SOC_ETM_CH1_TASK_ID    0x000000FFU
#define SOC_ETM_CH1_TASK_ID_M  (SOC_ETM_CH1_TASK_ID_V << SOC_ETM_CH1_TASK_ID_S)
#define SOC_ETM_CH1_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH1_TASK_ID_S  0

/** SOC_ETM_CH2_EVT_ID_REG register
 *  channel2 event id register
 */
#define SOC_ETM_CH2_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x28)
/** SOC_ETM_CH2_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch2_evt_id
 */
#define SOC_ETM_CH2_EVT_ID    0x000000FFU
#define SOC_ETM_CH2_EVT_ID_M  (SOC_ETM_CH2_EVT_ID_V << SOC_ETM_CH2_EVT_ID_S)
#define SOC_ETM_CH2_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH2_EVT_ID_S  0

/** SOC_ETM_CH2_TASK_ID_REG register
 *  channel2 task id register
 */
#define SOC_ETM_CH2_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x2c)
/** SOC_ETM_CH2_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch2_task_id
 */
#define SOC_ETM_CH2_TASK_ID    0x000000FFU
#define SOC_ETM_CH2_TASK_ID_M  (SOC_ETM_CH2_TASK_ID_V << SOC_ETM_CH2_TASK_ID_S)
#define SOC_ETM_CH2_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH2_TASK_ID_S  0

/** SOC_ETM_CH3_EVT_ID_REG register
 *  channel3 event id register
 */
#define SOC_ETM_CH3_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x30)
/** SOC_ETM_CH3_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch3_evt_id
 */
#define SOC_ETM_CH3_EVT_ID    0x000000FFU
#define SOC_ETM_CH3_EVT_ID_M  (SOC_ETM_CH3_EVT_ID_V << SOC_ETM_CH3_EVT_ID_S)
#define SOC_ETM_CH3_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH3_EVT_ID_S  0

/** SOC_ETM_CH3_TASK_ID_REG register
 *  channel3 task id register
 */
#define SOC_ETM_CH3_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x34)
/** SOC_ETM_CH3_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch3_task_id
 */
#define SOC_ETM_CH3_TASK_ID    0x000000FFU
#define SOC_ETM_CH3_TASK_ID_M  (SOC_ETM_CH3_TASK_ID_V << SOC_ETM_CH3_TASK_ID_S)
#define SOC_ETM_CH3_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH3_TASK_ID_S  0

/** SOC_ETM_CH4_EVT_ID_REG register
 *  channel4 event id register
 */
#define SOC_ETM_CH4_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x38)
/** SOC_ETM_CH4_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch4_evt_id
 */
#define SOC_ETM_CH4_EVT_ID    0x000000FFU
#define SOC_ETM_CH4_EVT_ID_M  (SOC_ETM_CH4_EVT_ID_V << SOC_ETM_CH4_EVT_ID_S)
#define SOC_ETM_CH4_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH4_EVT_ID_S  0

/** SOC_ETM_CH4_TASK_ID_REG register
 *  channel4 task id register
 */
#define SOC_ETM_CH4_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x3c)
/** SOC_ETM_CH4_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch4_task_id
 */
#define SOC_ETM_CH4_TASK_ID    0x000000FFU
#define SOC_ETM_CH4_TASK_ID_M  (SOC_ETM_CH4_TASK_ID_V << SOC_ETM_CH4_TASK_ID_S)
#define SOC_ETM_CH4_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH4_TASK_ID_S  0

/** SOC_ETM_CH5_EVT_ID_REG register
 *  channel5 event id register
 */
#define SOC_ETM_CH5_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x40)
/** SOC_ETM_CH5_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch5_evt_id
 */
#define SOC_ETM_CH5_EVT_ID    0x000000FFU
#define SOC_ETM_CH5_EVT_ID_M  (SOC_ETM_CH5_EVT_ID_V << SOC_ETM_CH5_EVT_ID_S)
#define SOC_ETM_CH5_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH5_EVT_ID_S  0

/** SOC_ETM_CH5_TASK_ID_REG register
 *  channel5 task id register
 */
#define SOC_ETM_CH5_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x44)
/** SOC_ETM_CH5_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch5_task_id
 */
#define SOC_ETM_CH5_TASK_ID    0x000000FFU
#define SOC_ETM_CH5_TASK_ID_M  (SOC_ETM_CH5_TASK_ID_V << SOC_ETM_CH5_TASK_ID_S)
#define SOC_ETM_CH5_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH5_TASK_ID_S  0

/** SOC_ETM_CH6_EVT_ID_REG register
 *  channel6 event id register
 */
#define SOC_ETM_CH6_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x48)
/** SOC_ETM_CH6_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch6_evt_id
 */
#define SOC_ETM_CH6_EVT_ID    0x000000FFU
#define SOC_ETM_CH6_EVT_ID_M  (SOC_ETM_CH6_EVT_ID_V << SOC_ETM_CH6_EVT_ID_S)
#define SOC_ETM_CH6_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH6_EVT_ID_S  0

/** SOC_ETM_CH6_TASK_ID_REG register
 *  channel6 task id register
 */
#define SOC_ETM_CH6_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x4c)
/** SOC_ETM_CH6_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch6_task_id
 */
#define SOC_ETM_CH6_TASK_ID    0x000000FFU
#define SOC_ETM_CH6_TASK_ID_M  (SOC_ETM_CH6_TASK_ID_V << SOC_ETM_CH6_TASK_ID_S)
#define SOC_ETM_CH6_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH6_TASK_ID_S  0

/** SOC_ETM_CH7_EVT_ID_REG register
 *  channel7 event id register
 */
#define SOC_ETM_CH7_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x50)
/** SOC_ETM_CH7_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch7_evt_id
 */
#define SOC_ETM_CH7_EVT_ID    0x000000FFU
#define SOC_ETM_CH7_EVT_ID_M  (SOC_ETM_CH7_EVT_ID_V << SOC_ETM_CH7_EVT_ID_S)
#define SOC_ETM_CH7_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH7_EVT_ID_S  0

/** SOC_ETM_CH7_TASK_ID_REG register
 *  channel7 task id register
 */
#define SOC_ETM_CH7_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x54)
/** SOC_ETM_CH7_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch7_task_id
 */
#define SOC_ETM_CH7_TASK_ID    0x000000FFU
#define SOC_ETM_CH7_TASK_ID_M  (SOC_ETM_CH7_TASK_ID_V << SOC_ETM_CH7_TASK_ID_S)
#define SOC_ETM_CH7_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH7_TASK_ID_S  0

/** SOC_ETM_CH8_EVT_ID_REG register
 *  channel8 event id register
 */
#define SOC_ETM_CH8_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x58)
/** SOC_ETM_CH8_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch8_evt_id
 */
#define SOC_ETM_CH8_EVT_ID    0x000000FFU
#define SOC_ETM_CH8_EVT_ID_M  (SOC_ETM_CH8_EVT_ID_V << SOC_ETM_CH8_EVT_ID_S)
#define SOC_ETM_CH8_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH8_EVT_ID_S  0

/** SOC_ETM_CH8_TASK_ID_REG register
 *  channel8 task id register
 */
#define SOC_ETM_CH8_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x5c)
/** SOC_ETM_CH8_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch8_task_id
 */
#define SOC_ETM_CH8_TASK_ID    0x000000FFU
#define SOC_ETM_CH8_TASK_ID_M  (SOC_ETM_CH8_TASK_ID_V << SOC_ETM_CH8_TASK_ID_S)
#define SOC_ETM_CH8_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH8_TASK_ID_S  0

/** SOC_ETM_CH9_EVT_ID_REG register
 *  channel9 event id register
 */
#define SOC_ETM_CH9_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x60)
/** SOC_ETM_CH9_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch9_evt_id
 */
#define SOC_ETM_CH9_EVT_ID    0x000000FFU
#define SOC_ETM_CH9_EVT_ID_M  (SOC_ETM_CH9_EVT_ID_V << SOC_ETM_CH9_EVT_ID_S)
#define SOC_ETM_CH9_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH9_EVT_ID_S  0

/** SOC_ETM_CH9_TASK_ID_REG register
 *  channel9 task id register
 */
#define SOC_ETM_CH9_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x64)
/** SOC_ETM_CH9_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch9_task_id
 */
#define SOC_ETM_CH9_TASK_ID    0x000000FFU
#define SOC_ETM_CH9_TASK_ID_M  (SOC_ETM_CH9_TASK_ID_V << SOC_ETM_CH9_TASK_ID_S)
#define SOC_ETM_CH9_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH9_TASK_ID_S  0

/** SOC_ETM_CH10_EVT_ID_REG register
 *  channel10 event id register
 */
#define SOC_ETM_CH10_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x68)
/** SOC_ETM_CH10_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch10_evt_id
 */
#define SOC_ETM_CH10_EVT_ID    0x000000FFU
#define SOC_ETM_CH10_EVT_ID_M  (SOC_ETM_CH10_EVT_ID_V << SOC_ETM_CH10_EVT_ID_S)
#define SOC_ETM_CH10_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH10_EVT_ID_S  0

/** SOC_ETM_CH10_TASK_ID_REG register
 *  channel10 task id register
 */
#define SOC_ETM_CH10_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x6c)
/** SOC_ETM_CH10_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch10_task_id
 */
#define SOC_ETM_CH10_TASK_ID    0x000000FFU
#define SOC_ETM_CH10_TASK_ID_M  (SOC_ETM_CH10_TASK_ID_V << SOC_ETM_CH10_TASK_ID_S)
#define SOC_ETM_CH10_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH10_TASK_ID_S  0

/** SOC_ETM_CH11_EVT_ID_REG register
 *  channel11 event id register
 */
#define SOC_ETM_CH11_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x70)
/** SOC_ETM_CH11_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch11_evt_id
 */
#define SOC_ETM_CH11_EVT_ID    0x000000FFU
#define SOC_ETM_CH11_EVT_ID_M  (SOC_ETM_CH11_EVT_ID_V << SOC_ETM_CH11_EVT_ID_S)
#define SOC_ETM_CH11_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH11_EVT_ID_S  0

/** SOC_ETM_CH11_TASK_ID_REG register
 *  channel11 task id register
 */
#define SOC_ETM_CH11_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x74)
/** SOC_ETM_CH11_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch11_task_id
 */
#define SOC_ETM_CH11_TASK_ID    0x000000FFU
#define SOC_ETM_CH11_TASK_ID_M  (SOC_ETM_CH11_TASK_ID_V << SOC_ETM_CH11_TASK_ID_S)
#define SOC_ETM_CH11_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH11_TASK_ID_S  0

/** SOC_ETM_CH12_EVT_ID_REG register
 *  channel12 event id register
 */
#define SOC_ETM_CH12_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x78)
/** SOC_ETM_CH12_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch12_evt_id
 */
#define SOC_ETM_CH12_EVT_ID    0x000000FFU
#define SOC_ETM_CH12_EVT_ID_M  (SOC_ETM_CH12_EVT_ID_V << SOC_ETM_CH12_EVT_ID_S)
#define SOC_ETM_CH12_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH12_EVT_ID_S  0

/** SOC_ETM_CH12_TASK_ID_REG register
 *  channel12 task id register
 */
#define SOC_ETM_CH12_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x7c)
/** SOC_ETM_CH12_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch12_task_id
 */
#define SOC_ETM_CH12_TASK_ID    0x000000FFU
#define SOC_ETM_CH12_TASK_ID_M  (SOC_ETM_CH12_TASK_ID_V << SOC_ETM_CH12_TASK_ID_S)
#define SOC_ETM_CH12_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH12_TASK_ID_S  0

/** SOC_ETM_CH13_EVT_ID_REG register
 *  channel13 event id register
 */
#define SOC_ETM_CH13_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x80)
/** SOC_ETM_CH13_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch13_evt_id
 */
#define SOC_ETM_CH13_EVT_ID    0x000000FFU
#define SOC_ETM_CH13_EVT_ID_M  (SOC_ETM_CH13_EVT_ID_V << SOC_ETM_CH13_EVT_ID_S)
#define SOC_ETM_CH13_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH13_EVT_ID_S  0

/** SOC_ETM_CH13_TASK_ID_REG register
 *  channel13 task id register
 */
#define SOC_ETM_CH13_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x84)
/** SOC_ETM_CH13_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch13_task_id
 */
#define SOC_ETM_CH13_TASK_ID    0x000000FFU
#define SOC_ETM_CH13_TASK_ID_M  (SOC_ETM_CH13_TASK_ID_V << SOC_ETM_CH13_TASK_ID_S)
#define SOC_ETM_CH13_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH13_TASK_ID_S  0

/** SOC_ETM_CH14_EVT_ID_REG register
 *  channel14 event id register
 */
#define SOC_ETM_CH14_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x88)
/** SOC_ETM_CH14_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch14_evt_id
 */
#define SOC_ETM_CH14_EVT_ID    0x000000FFU
#define SOC_ETM_CH14_EVT_ID_M  (SOC_ETM_CH14_EVT_ID_V << SOC_ETM_CH14_EVT_ID_S)
#define SOC_ETM_CH14_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH14_EVT_ID_S  0

/** SOC_ETM_CH14_TASK_ID_REG register
 *  channel14 task id register
 */
#define SOC_ETM_CH14_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x8c)
/** SOC_ETM_CH14_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch14_task_id
 */
#define SOC_ETM_CH14_TASK_ID    0x000000FFU
#define SOC_ETM_CH14_TASK_ID_M  (SOC_ETM_CH14_TASK_ID_V << SOC_ETM_CH14_TASK_ID_S)
#define SOC_ETM_CH14_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH14_TASK_ID_S  0

/** SOC_ETM_CH15_EVT_ID_REG register
 *  channel15 event id register
 */
#define SOC_ETM_CH15_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x90)
/** SOC_ETM_CH15_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch15_evt_id
 */
#define SOC_ETM_CH15_EVT_ID    0x000000FFU
#define SOC_ETM_CH15_EVT_ID_M  (SOC_ETM_CH15_EVT_ID_V << SOC_ETM_CH15_EVT_ID_S)
#define SOC_ETM_CH15_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH15_EVT_ID_S  0

/** SOC_ETM_CH15_TASK_ID_REG register
 *  channel15 task id register
 */
#define SOC_ETM_CH15_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x94)
/** SOC_ETM_CH15_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch15_task_id
 */
#define SOC_ETM_CH15_TASK_ID    0x000000FFU
#define SOC_ETM_CH15_TASK_ID_M  (SOC_ETM_CH15_TASK_ID_V << SOC_ETM_CH15_TASK_ID_S)
#define SOC_ETM_CH15_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH15_TASK_ID_S  0

/** SOC_ETM_CH16_EVT_ID_REG register
 *  channel16 event id register
 */
#define SOC_ETM_CH16_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x98)
/** SOC_ETM_CH16_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch16_evt_id
 */
#define SOC_ETM_CH16_EVT_ID    0x000000FFU
#define SOC_ETM_CH16_EVT_ID_M  (SOC_ETM_CH16_EVT_ID_V << SOC_ETM_CH16_EVT_ID_S)
#define SOC_ETM_CH16_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH16_EVT_ID_S  0

/** SOC_ETM_CH16_TASK_ID_REG register
 *  channel16 task id register
 */
#define SOC_ETM_CH16_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x9c)
/** SOC_ETM_CH16_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch16_task_id
 */
#define SOC_ETM_CH16_TASK_ID    0x000000FFU
#define SOC_ETM_CH16_TASK_ID_M  (SOC_ETM_CH16_TASK_ID_V << SOC_ETM_CH16_TASK_ID_S)
#define SOC_ETM_CH16_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH16_TASK_ID_S  0

/** SOC_ETM_CH17_EVT_ID_REG register
 *  channel17 event id register
 */
#define SOC_ETM_CH17_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xa0)
/** SOC_ETM_CH17_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch17_evt_id
 */
#define SOC_ETM_CH17_EVT_ID    0x000000FFU
#define SOC_ETM_CH17_EVT_ID_M  (SOC_ETM_CH17_EVT_ID_V << SOC_ETM_CH17_EVT_ID_S)
#define SOC_ETM_CH17_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH17_EVT_ID_S  0

/** SOC_ETM_CH17_TASK_ID_REG register
 *  channel17 task id register
 */
#define SOC_ETM_CH17_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xa4)
/** SOC_ETM_CH17_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch17_task_id
 */
#define SOC_ETM_CH17_TASK_ID    0x000000FFU
#define SOC_ETM_CH17_TASK_ID_M  (SOC_ETM_CH17_TASK_ID_V << SOC_ETM_CH17_TASK_ID_S)
#define SOC_ETM_CH17_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH17_TASK_ID_S  0

/** SOC_ETM_CH18_EVT_ID_REG register
 *  channel18 event id register
 */
#define SOC_ETM_CH18_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xa8)
/** SOC_ETM_CH18_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch18_evt_id
 */
#define SOC_ETM_CH18_EVT_ID    0x000000FFU
#define SOC_ETM_CH18_EVT_ID_M  (SOC_ETM_CH18_EVT_ID_V << SOC_ETM_CH18_EVT_ID_S)
#define SOC_ETM_CH18_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH18_EVT_ID_S  0

/** SOC_ETM_CH18_TASK_ID_REG register
 *  channel18 task id register
 */
#define SOC_ETM_CH18_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xac)
/** SOC_ETM_CH18_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch18_task_id
 */
#define SOC_ETM_CH18_TASK_ID    0x000000FFU
#define SOC_ETM_CH18_TASK_ID_M  (SOC_ETM_CH18_TASK_ID_V << SOC_ETM_CH18_TASK_ID_S)
#define SOC_ETM_CH18_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH18_TASK_ID_S  0

/** SOC_ETM_CH19_EVT_ID_REG register
 *  channel19 event id register
 */
#define SOC_ETM_CH19_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xb0)
/** SOC_ETM_CH19_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch19_evt_id
 */
#define SOC_ETM_CH19_EVT_ID    0x000000FFU
#define SOC_ETM_CH19_EVT_ID_M  (SOC_ETM_CH19_EVT_ID_V << SOC_ETM_CH19_EVT_ID_S)
#define SOC_ETM_CH19_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH19_EVT_ID_S  0

/** SOC_ETM_CH19_TASK_ID_REG register
 *  channel19 task id register
 */
#define SOC_ETM_CH19_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xb4)
/** SOC_ETM_CH19_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch19_task_id
 */
#define SOC_ETM_CH19_TASK_ID    0x000000FFU
#define SOC_ETM_CH19_TASK_ID_M  (SOC_ETM_CH19_TASK_ID_V << SOC_ETM_CH19_TASK_ID_S)
#define SOC_ETM_CH19_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH19_TASK_ID_S  0

/** SOC_ETM_CH20_EVT_ID_REG register
 *  channel20 event id register
 */
#define SOC_ETM_CH20_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xb8)
/** SOC_ETM_CH20_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch20_evt_id
 */
#define SOC_ETM_CH20_EVT_ID    0x000000FFU
#define SOC_ETM_CH20_EVT_ID_M  (SOC_ETM_CH20_EVT_ID_V << SOC_ETM_CH20_EVT_ID_S)
#define SOC_ETM_CH20_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH20_EVT_ID_S  0

/** SOC_ETM_CH20_TASK_ID_REG register
 *  channel20 task id register
 */
#define SOC_ETM_CH20_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xbc)
/** SOC_ETM_CH20_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch20_task_id
 */
#define SOC_ETM_CH20_TASK_ID    0x000000FFU
#define SOC_ETM_CH20_TASK_ID_M  (SOC_ETM_CH20_TASK_ID_V << SOC_ETM_CH20_TASK_ID_S)
#define SOC_ETM_CH20_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH20_TASK_ID_S  0

/** SOC_ETM_CH21_EVT_ID_REG register
 *  channel21 event id register
 */
#define SOC_ETM_CH21_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xc0)
/** SOC_ETM_CH21_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch21_evt_id
 */
#define SOC_ETM_CH21_EVT_ID    0x000000FFU
#define SOC_ETM_CH21_EVT_ID_M  (SOC_ETM_CH21_EVT_ID_V << SOC_ETM_CH21_EVT_ID_S)
#define SOC_ETM_CH21_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH21_EVT_ID_S  0

/** SOC_ETM_CH21_TASK_ID_REG register
 *  channel21 task id register
 */
#define SOC_ETM_CH21_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xc4)
/** SOC_ETM_CH21_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch21_task_id
 */
#define SOC_ETM_CH21_TASK_ID    0x000000FFU
#define SOC_ETM_CH21_TASK_ID_M  (SOC_ETM_CH21_TASK_ID_V << SOC_ETM_CH21_TASK_ID_S)
#define SOC_ETM_CH21_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH21_TASK_ID_S  0

/** SOC_ETM_CH22_EVT_ID_REG register
 *  channel22 event id register
 */
#define SOC_ETM_CH22_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xc8)
/** SOC_ETM_CH22_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch22_evt_id
 */
#define SOC_ETM_CH22_EVT_ID    0x000000FFU
#define SOC_ETM_CH22_EVT_ID_M  (SOC_ETM_CH22_EVT_ID_V << SOC_ETM_CH22_EVT_ID_S)
#define SOC_ETM_CH22_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH22_EVT_ID_S  0

/** SOC_ETM_CH22_TASK_ID_REG register
 *  channel22 task id register
 */
#define SOC_ETM_CH22_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xcc)
/** SOC_ETM_CH22_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch22_task_id
 */
#define SOC_ETM_CH22_TASK_ID    0x000000FFU
#define SOC_ETM_CH22_TASK_ID_M  (SOC_ETM_CH22_TASK_ID_V << SOC_ETM_CH22_TASK_ID_S)
#define SOC_ETM_CH22_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH22_TASK_ID_S  0

/** SOC_ETM_CH23_EVT_ID_REG register
 *  channel23 event id register
 */
#define SOC_ETM_CH23_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xd0)
/** SOC_ETM_CH23_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch23_evt_id
 */
#define SOC_ETM_CH23_EVT_ID    0x000000FFU
#define SOC_ETM_CH23_EVT_ID_M  (SOC_ETM_CH23_EVT_ID_V << SOC_ETM_CH23_EVT_ID_S)
#define SOC_ETM_CH23_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH23_EVT_ID_S  0

/** SOC_ETM_CH23_TASK_ID_REG register
 *  channel23 task id register
 */
#define SOC_ETM_CH23_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xd4)
/** SOC_ETM_CH23_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch23_task_id
 */
#define SOC_ETM_CH23_TASK_ID    0x000000FFU
#define SOC_ETM_CH23_TASK_ID_M  (SOC_ETM_CH23_TASK_ID_V << SOC_ETM_CH23_TASK_ID_S)
#define SOC_ETM_CH23_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH23_TASK_ID_S  0

/** SOC_ETM_CH24_EVT_ID_REG register
 *  channel24 event id register
 */
#define SOC_ETM_CH24_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xd8)
/** SOC_ETM_CH24_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch24_evt_id
 */
#define SOC_ETM_CH24_EVT_ID    0x000000FFU
#define SOC_ETM_CH24_EVT_ID_M  (SOC_ETM_CH24_EVT_ID_V << SOC_ETM_CH24_EVT_ID_S)
#define SOC_ETM_CH24_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH24_EVT_ID_S  0

/** SOC_ETM_CH24_TASK_ID_REG register
 *  channel24 task id register
 */
#define SOC_ETM_CH24_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xdc)
/** SOC_ETM_CH24_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch24_task_id
 */
#define SOC_ETM_CH24_TASK_ID    0x000000FFU
#define SOC_ETM_CH24_TASK_ID_M  (SOC_ETM_CH24_TASK_ID_V << SOC_ETM_CH24_TASK_ID_S)
#define SOC_ETM_CH24_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH24_TASK_ID_S  0

/** SOC_ETM_CH25_EVT_ID_REG register
 *  channel25 event id register
 */
#define SOC_ETM_CH25_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xe0)
/** SOC_ETM_CH25_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch25_evt_id
 */
#define SOC_ETM_CH25_EVT_ID    0x000000FFU
#define SOC_ETM_CH25_EVT_ID_M  (SOC_ETM_CH25_EVT_ID_V << SOC_ETM_CH25_EVT_ID_S)
#define SOC_ETM_CH25_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH25_EVT_ID_S  0

/** SOC_ETM_CH25_TASK_ID_REG register
 *  channel25 task id register
 */
#define SOC_ETM_CH25_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xe4)
/** SOC_ETM_CH25_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch25_task_id
 */
#define SOC_ETM_CH25_TASK_ID    0x000000FFU
#define SOC_ETM_CH25_TASK_ID_M  (SOC_ETM_CH25_TASK_ID_V << SOC_ETM_CH25_TASK_ID_S)
#define SOC_ETM_CH25_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH25_TASK_ID_S  0

/** SOC_ETM_CH26_EVT_ID_REG register
 *  channel26 event id register
 */
#define SOC_ETM_CH26_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xe8)
/** SOC_ETM_CH26_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch26_evt_id
 */
#define SOC_ETM_CH26_EVT_ID    0x000000FFU
#define SOC_ETM_CH26_EVT_ID_M  (SOC_ETM_CH26_EVT_ID_V << SOC_ETM_CH26_EVT_ID_S)
#define SOC_ETM_CH26_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH26_EVT_ID_S  0

/** SOC_ETM_CH26_TASK_ID_REG register
 *  channel26 task id register
 */
#define SOC_ETM_CH26_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xec)
/** SOC_ETM_CH26_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch26_task_id
 */
#define SOC_ETM_CH26_TASK_ID    0x000000FFU
#define SOC_ETM_CH26_TASK_ID_M  (SOC_ETM_CH26_TASK_ID_V << SOC_ETM_CH26_TASK_ID_S)
#define SOC_ETM_CH26_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH26_TASK_ID_S  0

/** SOC_ETM_CH27_EVT_ID_REG register
 *  channel27 event id register
 */
#define SOC_ETM_CH27_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xf0)
/** SOC_ETM_CH27_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch27_evt_id
 */
#define SOC_ETM_CH27_EVT_ID    0x000000FFU
#define SOC_ETM_CH27_EVT_ID_M  (SOC_ETM_CH27_EVT_ID_V << SOC_ETM_CH27_EVT_ID_S)
#define SOC_ETM_CH27_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH27_EVT_ID_S  0

/** SOC_ETM_CH27_TASK_ID_REG register
 *  channel27 task id register
 */
#define SOC_ETM_CH27_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xf4)
/** SOC_ETM_CH27_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch27_task_id
 */
#define SOC_ETM_CH27_TASK_ID    0x000000FFU
#define SOC_ETM_CH27_TASK_ID_M  (SOC_ETM_CH27_TASK_ID_V << SOC_ETM_CH27_TASK_ID_S)
#define SOC_ETM_CH27_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH27_TASK_ID_S  0

/** SOC_ETM_CH28_EVT_ID_REG register
 *  channel28 event id register
 */
#define SOC_ETM_CH28_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0xf8)
/** SOC_ETM_CH28_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch28_evt_id
 */
#define SOC_ETM_CH28_EVT_ID    0x000000FFU
#define SOC_ETM_CH28_EVT_ID_M  (SOC_ETM_CH28_EVT_ID_V << SOC_ETM_CH28_EVT_ID_S)
#define SOC_ETM_CH28_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH28_EVT_ID_S  0

/** SOC_ETM_CH28_TASK_ID_REG register
 *  channel28 task id register
 */
#define SOC_ETM_CH28_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0xfc)
/** SOC_ETM_CH28_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch28_task_id
 */
#define SOC_ETM_CH28_TASK_ID    0x000000FFU
#define SOC_ETM_CH28_TASK_ID_M  (SOC_ETM_CH28_TASK_ID_V << SOC_ETM_CH28_TASK_ID_S)
#define SOC_ETM_CH28_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH28_TASK_ID_S  0

/** SOC_ETM_CH29_EVT_ID_REG register
 *  channel29 event id register
 */
#define SOC_ETM_CH29_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x100)
/** SOC_ETM_CH29_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch29_evt_id
 */
#define SOC_ETM_CH29_EVT_ID    0x000000FFU
#define SOC_ETM_CH29_EVT_ID_M  (SOC_ETM_CH29_EVT_ID_V << SOC_ETM_CH29_EVT_ID_S)
#define SOC_ETM_CH29_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH29_EVT_ID_S  0

/** SOC_ETM_CH29_TASK_ID_REG register
 *  channel29 task id register
 */
#define SOC_ETM_CH29_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x104)
/** SOC_ETM_CH29_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch29_task_id
 */
#define SOC_ETM_CH29_TASK_ID    0x000000FFU
#define SOC_ETM_CH29_TASK_ID_M  (SOC_ETM_CH29_TASK_ID_V << SOC_ETM_CH29_TASK_ID_S)
#define SOC_ETM_CH29_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH29_TASK_ID_S  0

/** SOC_ETM_CH30_EVT_ID_REG register
 *  channel30 event id register
 */
#define SOC_ETM_CH30_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x108)
/** SOC_ETM_CH30_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch30_evt_id
 */
#define SOC_ETM_CH30_EVT_ID    0x000000FFU
#define SOC_ETM_CH30_EVT_ID_M  (SOC_ETM_CH30_EVT_ID_V << SOC_ETM_CH30_EVT_ID_S)
#define SOC_ETM_CH30_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH30_EVT_ID_S  0

/** SOC_ETM_CH30_TASK_ID_REG register
 *  channel30 task id register
 */
#define SOC_ETM_CH30_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x10c)
/** SOC_ETM_CH30_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch30_task_id
 */
#define SOC_ETM_CH30_TASK_ID    0x000000FFU
#define SOC_ETM_CH30_TASK_ID_M  (SOC_ETM_CH30_TASK_ID_V << SOC_ETM_CH30_TASK_ID_S)
#define SOC_ETM_CH30_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH30_TASK_ID_S  0

/** SOC_ETM_CH31_EVT_ID_REG register
 *  channel31 event id register
 */
#define SOC_ETM_CH31_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x110)
/** SOC_ETM_CH31_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch31_evt_id
 */
#define SOC_ETM_CH31_EVT_ID    0x000000FFU
#define SOC_ETM_CH31_EVT_ID_M  (SOC_ETM_CH31_EVT_ID_V << SOC_ETM_CH31_EVT_ID_S)
#define SOC_ETM_CH31_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH31_EVT_ID_S  0

/** SOC_ETM_CH31_TASK_ID_REG register
 *  channel31 task id register
 */
#define SOC_ETM_CH31_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x114)
/** SOC_ETM_CH31_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch31_task_id
 */
#define SOC_ETM_CH31_TASK_ID    0x000000FFU
#define SOC_ETM_CH31_TASK_ID_M  (SOC_ETM_CH31_TASK_ID_V << SOC_ETM_CH31_TASK_ID_S)
#define SOC_ETM_CH31_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH31_TASK_ID_S  0

/** SOC_ETM_CH32_EVT_ID_REG register
 *  channel32 event id register
 */
#define SOC_ETM_CH32_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x118)
/** SOC_ETM_CH32_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch32_evt_id
 */
#define SOC_ETM_CH32_EVT_ID    0x000000FFU
#define SOC_ETM_CH32_EVT_ID_M  (SOC_ETM_CH32_EVT_ID_V << SOC_ETM_CH32_EVT_ID_S)
#define SOC_ETM_CH32_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH32_EVT_ID_S  0

/** SOC_ETM_CH32_TASK_ID_REG register
 *  channel32 task id register
 */
#define SOC_ETM_CH32_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x11c)
/** SOC_ETM_CH32_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch32_task_id
 */
#define SOC_ETM_CH32_TASK_ID    0x000000FFU
#define SOC_ETM_CH32_TASK_ID_M  (SOC_ETM_CH32_TASK_ID_V << SOC_ETM_CH32_TASK_ID_S)
#define SOC_ETM_CH32_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH32_TASK_ID_S  0

/** SOC_ETM_CH33_EVT_ID_REG register
 *  channel33 event id register
 */
#define SOC_ETM_CH33_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x120)
/** SOC_ETM_CH33_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch33_evt_id
 */
#define SOC_ETM_CH33_EVT_ID    0x000000FFU
#define SOC_ETM_CH33_EVT_ID_M  (SOC_ETM_CH33_EVT_ID_V << SOC_ETM_CH33_EVT_ID_S)
#define SOC_ETM_CH33_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH33_EVT_ID_S  0

/** SOC_ETM_CH33_TASK_ID_REG register
 *  channel33 task id register
 */
#define SOC_ETM_CH33_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x124)
/** SOC_ETM_CH33_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch33_task_id
 */
#define SOC_ETM_CH33_TASK_ID    0x000000FFU
#define SOC_ETM_CH33_TASK_ID_M  (SOC_ETM_CH33_TASK_ID_V << SOC_ETM_CH33_TASK_ID_S)
#define SOC_ETM_CH33_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH33_TASK_ID_S  0

/** SOC_ETM_CH34_EVT_ID_REG register
 *  channel34 event id register
 */
#define SOC_ETM_CH34_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x128)
/** SOC_ETM_CH34_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch34_evt_id
 */
#define SOC_ETM_CH34_EVT_ID    0x000000FFU
#define SOC_ETM_CH34_EVT_ID_M  (SOC_ETM_CH34_EVT_ID_V << SOC_ETM_CH34_EVT_ID_S)
#define SOC_ETM_CH34_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH34_EVT_ID_S  0

/** SOC_ETM_CH34_TASK_ID_REG register
 *  channel34 task id register
 */
#define SOC_ETM_CH34_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x12c)
/** SOC_ETM_CH34_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch34_task_id
 */
#define SOC_ETM_CH34_TASK_ID    0x000000FFU
#define SOC_ETM_CH34_TASK_ID_M  (SOC_ETM_CH34_TASK_ID_V << SOC_ETM_CH34_TASK_ID_S)
#define SOC_ETM_CH34_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH34_TASK_ID_S  0

/** SOC_ETM_CH35_EVT_ID_REG register
 *  channel35 event id register
 */
#define SOC_ETM_CH35_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x130)
/** SOC_ETM_CH35_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch35_evt_id
 */
#define SOC_ETM_CH35_EVT_ID    0x000000FFU
#define SOC_ETM_CH35_EVT_ID_M  (SOC_ETM_CH35_EVT_ID_V << SOC_ETM_CH35_EVT_ID_S)
#define SOC_ETM_CH35_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH35_EVT_ID_S  0

/** SOC_ETM_CH35_TASK_ID_REG register
 *  channel35 task id register
 */
#define SOC_ETM_CH35_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x134)
/** SOC_ETM_CH35_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch35_task_id
 */
#define SOC_ETM_CH35_TASK_ID    0x000000FFU
#define SOC_ETM_CH35_TASK_ID_M  (SOC_ETM_CH35_TASK_ID_V << SOC_ETM_CH35_TASK_ID_S)
#define SOC_ETM_CH35_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH35_TASK_ID_S  0

/** SOC_ETM_CH36_EVT_ID_REG register
 *  channel36 event id register
 */
#define SOC_ETM_CH36_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x138)
/** SOC_ETM_CH36_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch36_evt_id
 */
#define SOC_ETM_CH36_EVT_ID    0x000000FFU
#define SOC_ETM_CH36_EVT_ID_M  (SOC_ETM_CH36_EVT_ID_V << SOC_ETM_CH36_EVT_ID_S)
#define SOC_ETM_CH36_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH36_EVT_ID_S  0

/** SOC_ETM_CH36_TASK_ID_REG register
 *  channel36 task id register
 */
#define SOC_ETM_CH36_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x13c)
/** SOC_ETM_CH36_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch36_task_id
 */
#define SOC_ETM_CH36_TASK_ID    0x000000FFU
#define SOC_ETM_CH36_TASK_ID_M  (SOC_ETM_CH36_TASK_ID_V << SOC_ETM_CH36_TASK_ID_S)
#define SOC_ETM_CH36_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH36_TASK_ID_S  0

/** SOC_ETM_CH37_EVT_ID_REG register
 *  channel37 event id register
 */
#define SOC_ETM_CH37_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x140)
/** SOC_ETM_CH37_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch37_evt_id
 */
#define SOC_ETM_CH37_EVT_ID    0x000000FFU
#define SOC_ETM_CH37_EVT_ID_M  (SOC_ETM_CH37_EVT_ID_V << SOC_ETM_CH37_EVT_ID_S)
#define SOC_ETM_CH37_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH37_EVT_ID_S  0

/** SOC_ETM_CH37_TASK_ID_REG register
 *  channel37 task id register
 */
#define SOC_ETM_CH37_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x144)
/** SOC_ETM_CH37_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch37_task_id
 */
#define SOC_ETM_CH37_TASK_ID    0x000000FFU
#define SOC_ETM_CH37_TASK_ID_M  (SOC_ETM_CH37_TASK_ID_V << SOC_ETM_CH37_TASK_ID_S)
#define SOC_ETM_CH37_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH37_TASK_ID_S  0

/** SOC_ETM_CH38_EVT_ID_REG register
 *  channel38 event id register
 */
#define SOC_ETM_CH38_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x148)
/** SOC_ETM_CH38_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch38_evt_id
 */
#define SOC_ETM_CH38_EVT_ID    0x000000FFU
#define SOC_ETM_CH38_EVT_ID_M  (SOC_ETM_CH38_EVT_ID_V << SOC_ETM_CH38_EVT_ID_S)
#define SOC_ETM_CH38_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH38_EVT_ID_S  0

/** SOC_ETM_CH38_TASK_ID_REG register
 *  channel38 task id register
 */
#define SOC_ETM_CH38_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x14c)
/** SOC_ETM_CH38_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch38_task_id
 */
#define SOC_ETM_CH38_TASK_ID    0x000000FFU
#define SOC_ETM_CH38_TASK_ID_M  (SOC_ETM_CH38_TASK_ID_V << SOC_ETM_CH38_TASK_ID_S)
#define SOC_ETM_CH38_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH38_TASK_ID_S  0

/** SOC_ETM_CH39_EVT_ID_REG register
 *  channel39 event id register
 */
#define SOC_ETM_CH39_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x150)
/** SOC_ETM_CH39_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch39_evt_id
 */
#define SOC_ETM_CH39_EVT_ID    0x000000FFU
#define SOC_ETM_CH39_EVT_ID_M  (SOC_ETM_CH39_EVT_ID_V << SOC_ETM_CH39_EVT_ID_S)
#define SOC_ETM_CH39_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH39_EVT_ID_S  0

/** SOC_ETM_CH39_TASK_ID_REG register
 *  channel39 task id register
 */
#define SOC_ETM_CH39_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x154)
/** SOC_ETM_CH39_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch39_task_id
 */
#define SOC_ETM_CH39_TASK_ID    0x000000FFU
#define SOC_ETM_CH39_TASK_ID_M  (SOC_ETM_CH39_TASK_ID_V << SOC_ETM_CH39_TASK_ID_S)
#define SOC_ETM_CH39_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH39_TASK_ID_S  0

/** SOC_ETM_CH40_EVT_ID_REG register
 *  channel40 event id register
 */
#define SOC_ETM_CH40_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x158)
/** SOC_ETM_CH40_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch40_evt_id
 */
#define SOC_ETM_CH40_EVT_ID    0x000000FFU
#define SOC_ETM_CH40_EVT_ID_M  (SOC_ETM_CH40_EVT_ID_V << SOC_ETM_CH40_EVT_ID_S)
#define SOC_ETM_CH40_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH40_EVT_ID_S  0

/** SOC_ETM_CH40_TASK_ID_REG register
 *  channel40 task id register
 */
#define SOC_ETM_CH40_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x15c)
/** SOC_ETM_CH40_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch40_task_id
 */
#define SOC_ETM_CH40_TASK_ID    0x000000FFU
#define SOC_ETM_CH40_TASK_ID_M  (SOC_ETM_CH40_TASK_ID_V << SOC_ETM_CH40_TASK_ID_S)
#define SOC_ETM_CH40_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH40_TASK_ID_S  0

/** SOC_ETM_CH41_EVT_ID_REG register
 *  channel41 event id register
 */
#define SOC_ETM_CH41_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x160)
/** SOC_ETM_CH41_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch41_evt_id
 */
#define SOC_ETM_CH41_EVT_ID    0x000000FFU
#define SOC_ETM_CH41_EVT_ID_M  (SOC_ETM_CH41_EVT_ID_V << SOC_ETM_CH41_EVT_ID_S)
#define SOC_ETM_CH41_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH41_EVT_ID_S  0

/** SOC_ETM_CH41_TASK_ID_REG register
 *  channel41 task id register
 */
#define SOC_ETM_CH41_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x164)
/** SOC_ETM_CH41_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch41_task_id
 */
#define SOC_ETM_CH41_TASK_ID    0x000000FFU
#define SOC_ETM_CH41_TASK_ID_M  (SOC_ETM_CH41_TASK_ID_V << SOC_ETM_CH41_TASK_ID_S)
#define SOC_ETM_CH41_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH41_TASK_ID_S  0

/** SOC_ETM_CH42_EVT_ID_REG register
 *  channel42 event id register
 */
#define SOC_ETM_CH42_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x168)
/** SOC_ETM_CH42_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch42_evt_id
 */
#define SOC_ETM_CH42_EVT_ID    0x000000FFU
#define SOC_ETM_CH42_EVT_ID_M  (SOC_ETM_CH42_EVT_ID_V << SOC_ETM_CH42_EVT_ID_S)
#define SOC_ETM_CH42_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH42_EVT_ID_S  0

/** SOC_ETM_CH42_TASK_ID_REG register
 *  channel42 task id register
 */
#define SOC_ETM_CH42_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x16c)
/** SOC_ETM_CH42_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch42_task_id
 */
#define SOC_ETM_CH42_TASK_ID    0x000000FFU
#define SOC_ETM_CH42_TASK_ID_M  (SOC_ETM_CH42_TASK_ID_V << SOC_ETM_CH42_TASK_ID_S)
#define SOC_ETM_CH42_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH42_TASK_ID_S  0

/** SOC_ETM_CH43_EVT_ID_REG register
 *  channel43 event id register
 */
#define SOC_ETM_CH43_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x170)
/** SOC_ETM_CH43_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch43_evt_id
 */
#define SOC_ETM_CH43_EVT_ID    0x000000FFU
#define SOC_ETM_CH43_EVT_ID_M  (SOC_ETM_CH43_EVT_ID_V << SOC_ETM_CH43_EVT_ID_S)
#define SOC_ETM_CH43_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH43_EVT_ID_S  0

/** SOC_ETM_CH43_TASK_ID_REG register
 *  channel43 task id register
 */
#define SOC_ETM_CH43_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x174)
/** SOC_ETM_CH43_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch43_task_id
 */
#define SOC_ETM_CH43_TASK_ID    0x000000FFU
#define SOC_ETM_CH43_TASK_ID_M  (SOC_ETM_CH43_TASK_ID_V << SOC_ETM_CH43_TASK_ID_S)
#define SOC_ETM_CH43_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH43_TASK_ID_S  0

/** SOC_ETM_CH44_EVT_ID_REG register
 *  channel44 event id register
 */
#define SOC_ETM_CH44_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x178)
/** SOC_ETM_CH44_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch44_evt_id
 */
#define SOC_ETM_CH44_EVT_ID    0x000000FFU
#define SOC_ETM_CH44_EVT_ID_M  (SOC_ETM_CH44_EVT_ID_V << SOC_ETM_CH44_EVT_ID_S)
#define SOC_ETM_CH44_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH44_EVT_ID_S  0

/** SOC_ETM_CH44_TASK_ID_REG register
 *  channel44 task id register
 */
#define SOC_ETM_CH44_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x17c)
/** SOC_ETM_CH44_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch44_task_id
 */
#define SOC_ETM_CH44_TASK_ID    0x000000FFU
#define SOC_ETM_CH44_TASK_ID_M  (SOC_ETM_CH44_TASK_ID_V << SOC_ETM_CH44_TASK_ID_S)
#define SOC_ETM_CH44_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH44_TASK_ID_S  0

/** SOC_ETM_CH45_EVT_ID_REG register
 *  channel45 event id register
 */
#define SOC_ETM_CH45_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x180)
/** SOC_ETM_CH45_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch45_evt_id
 */
#define SOC_ETM_CH45_EVT_ID    0x000000FFU
#define SOC_ETM_CH45_EVT_ID_M  (SOC_ETM_CH45_EVT_ID_V << SOC_ETM_CH45_EVT_ID_S)
#define SOC_ETM_CH45_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH45_EVT_ID_S  0

/** SOC_ETM_CH45_TASK_ID_REG register
 *  channel45 task id register
 */
#define SOC_ETM_CH45_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x184)
/** SOC_ETM_CH45_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch45_task_id
 */
#define SOC_ETM_CH45_TASK_ID    0x000000FFU
#define SOC_ETM_CH45_TASK_ID_M  (SOC_ETM_CH45_TASK_ID_V << SOC_ETM_CH45_TASK_ID_S)
#define SOC_ETM_CH45_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH45_TASK_ID_S  0

/** SOC_ETM_CH46_EVT_ID_REG register
 *  channel46 event id register
 */
#define SOC_ETM_CH46_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x188)
/** SOC_ETM_CH46_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch46_evt_id
 */
#define SOC_ETM_CH46_EVT_ID    0x000000FFU
#define SOC_ETM_CH46_EVT_ID_M  (SOC_ETM_CH46_EVT_ID_V << SOC_ETM_CH46_EVT_ID_S)
#define SOC_ETM_CH46_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH46_EVT_ID_S  0

/** SOC_ETM_CH46_TASK_ID_REG register
 *  channel46 task id register
 */
#define SOC_ETM_CH46_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x18c)
/** SOC_ETM_CH46_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch46_task_id
 */
#define SOC_ETM_CH46_TASK_ID    0x000000FFU
#define SOC_ETM_CH46_TASK_ID_M  (SOC_ETM_CH46_TASK_ID_V << SOC_ETM_CH46_TASK_ID_S)
#define SOC_ETM_CH46_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH46_TASK_ID_S  0

/** SOC_ETM_CH47_EVT_ID_REG register
 *  channel47 event id register
 */
#define SOC_ETM_CH47_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x190)
/** SOC_ETM_CH47_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch47_evt_id
 */
#define SOC_ETM_CH47_EVT_ID    0x000000FFU
#define SOC_ETM_CH47_EVT_ID_M  (SOC_ETM_CH47_EVT_ID_V << SOC_ETM_CH47_EVT_ID_S)
#define SOC_ETM_CH47_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH47_EVT_ID_S  0

/** SOC_ETM_CH47_TASK_ID_REG register
 *  channel47 task id register
 */
#define SOC_ETM_CH47_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x194)
/** SOC_ETM_CH47_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch47_task_id
 */
#define SOC_ETM_CH47_TASK_ID    0x000000FFU
#define SOC_ETM_CH47_TASK_ID_M  (SOC_ETM_CH47_TASK_ID_V << SOC_ETM_CH47_TASK_ID_S)
#define SOC_ETM_CH47_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH47_TASK_ID_S  0

/** SOC_ETM_CH48_EVT_ID_REG register
 *  channel48 event id register
 */
#define SOC_ETM_CH48_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x198)
/** SOC_ETM_CH48_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch48_evt_id
 */
#define SOC_ETM_CH48_EVT_ID    0x000000FFU
#define SOC_ETM_CH48_EVT_ID_M  (SOC_ETM_CH48_EVT_ID_V << SOC_ETM_CH48_EVT_ID_S)
#define SOC_ETM_CH48_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH48_EVT_ID_S  0

/** SOC_ETM_CH48_TASK_ID_REG register
 *  channel48 task id register
 */
#define SOC_ETM_CH48_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x19c)
/** SOC_ETM_CH48_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch48_task_id
 */
#define SOC_ETM_CH48_TASK_ID    0x000000FFU
#define SOC_ETM_CH48_TASK_ID_M  (SOC_ETM_CH48_TASK_ID_V << SOC_ETM_CH48_TASK_ID_S)
#define SOC_ETM_CH48_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH48_TASK_ID_S  0

/** SOC_ETM_CH49_EVT_ID_REG register
 *  channel49 event id register
 */
#define SOC_ETM_CH49_EVT_ID_REG (DR_REG_SOC_ETM_BASE + 0x1a0)
/** SOC_ETM_CH49_EVT_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch49_evt_id
 */
#define SOC_ETM_CH49_EVT_ID    0x000000FFU
#define SOC_ETM_CH49_EVT_ID_M  (SOC_ETM_CH49_EVT_ID_V << SOC_ETM_CH49_EVT_ID_S)
#define SOC_ETM_CH49_EVT_ID_V  0x000000FFU
#define SOC_ETM_CH49_EVT_ID_S  0

/** SOC_ETM_CH49_TASK_ID_REG register
 *  channel49 task id register
 */
#define SOC_ETM_CH49_TASK_ID_REG (DR_REG_SOC_ETM_BASE + 0x1a4)
/** SOC_ETM_CH49_TASK_ID : R/W; bitpos: [7:0]; default: 0;
 *  ch49_task_id
 */
#define SOC_ETM_CH49_TASK_ID    0x000000FFU
#define SOC_ETM_CH49_TASK_ID_M  (SOC_ETM_CH49_TASK_ID_V << SOC_ETM_CH49_TASK_ID_S)
#define SOC_ETM_CH49_TASK_ID_V  0x000000FFU
#define SOC_ETM_CH49_TASK_ID_S  0

/** SOC_ETM_CLK_EN_REG register
 *  etm clock enable register
 */
#define SOC_ETM_CLK_EN_REG (DR_REG_SOC_ETM_BASE + 0x1a8)
/** SOC_ETM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  clock enable
 */
#define SOC_ETM_CLK_EN    (BIT(0))
#define SOC_ETM_CLK_EN_M  (SOC_ETM_CLK_EN_V << SOC_ETM_CLK_EN_S)
#define SOC_ETM_CLK_EN_V  0x00000001U
#define SOC_ETM_CLK_EN_S  0

/** SOC_ETM_DATE_REG register
 *  etm date register
 */
#define SOC_ETM_DATE_REG (DR_REG_SOC_ETM_BASE + 0x1ac)
/** SOC_ETM_DATE : R/W; bitpos: [27:0]; default: 35664018;
 *  date
 */
#define SOC_ETM_DATE    0x0FFFFFFFU
#define SOC_ETM_DATE_M  (SOC_ETM_DATE_V << SOC_ETM_DATE_S)
#define SOC_ETM_DATE_V  0x0FFFFFFFU
#define SOC_ETM_DATE_S  0

#ifdef __cplusplus
}
#endif
