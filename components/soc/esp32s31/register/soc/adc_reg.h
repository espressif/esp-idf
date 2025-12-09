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

/** ADC_CTRL0_REG register
 *  Register
 */
#define ADC_CTRL0_REG (DR_REG_ADC_BASE + 0x0)
/** ADC_SAR1_TRIGGER_STOP : WT; bitpos: [0]; default: 0;
 *  need_des
 */
#define ADC_SAR1_TRIGGER_STOP    (BIT(0))
#define ADC_SAR1_TRIGGER_STOP_M  (ADC_SAR1_TRIGGER_STOP_V << ADC_SAR1_TRIGGER_STOP_S)
#define ADC_SAR1_TRIGGER_STOP_V  0x00000001U
#define ADC_SAR1_TRIGGER_STOP_S  0
/** ADC_SAR1_TRIGGER_START : WT; bitpos: [1]; default: 0;
 *  need_des
 */
#define ADC_SAR1_TRIGGER_START    (BIT(1))
#define ADC_SAR1_TRIGGER_START_M  (ADC_SAR1_TRIGGER_START_V << ADC_SAR1_TRIGGER_START_S)
#define ADC_SAR1_TRIGGER_START_V  0x00000001U
#define ADC_SAR1_TRIGGER_START_S  1
/** ADC_SAR1_TRIGGER_MODE : R/W; bitpos: [3:2]; default: 0;
 *  SAR clock divider
 */
#define ADC_SAR1_TRIGGER_MODE    0x00000003U
#define ADC_SAR1_TRIGGER_MODE_M  (ADC_SAR1_TRIGGER_MODE_V << ADC_SAR1_TRIGGER_MODE_S)
#define ADC_SAR1_TRIGGER_MODE_V  0x00000003U
#define ADC_SAR1_TRIGGER_MODE_S  2
/** ADC_SAR1_PATT_LEN : R/W; bitpos: [17:14]; default: 15;
 *  0 ~ 15 means length 1 ~ 16
 */
#define ADC_SAR1_PATT_LEN    0x0000000FU
#define ADC_SAR1_PATT_LEN_M  (ADC_SAR1_PATT_LEN_V << ADC_SAR1_PATT_LEN_S)
#define ADC_SAR1_PATT_LEN_V  0x0000000FU
#define ADC_SAR1_PATT_LEN_S  14
/** ADC_SAR1_PATT_P_CLEAR : WT; bitpos: [22]; default: 0;
 *  clear the pointer of pattern table for DIG ADC1 CTRL
 */
#define ADC_SAR1_PATT_P_CLEAR    (BIT(22))
#define ADC_SAR1_PATT_P_CLEAR_M  (ADC_SAR1_PATT_P_CLEAR_V << ADC_SAR1_PATT_P_CLEAR_S)
#define ADC_SAR1_PATT_P_CLEAR_V  0x00000001U
#define ADC_SAR1_PATT_P_CLEAR_S  22
/** ADC_SAR1_PATT_TYPE : R/W; bitpos: [23]; default: 0;
 *  1: select meas start as table update 0:select mead done as table update
 */
#define ADC_SAR1_PATT_TYPE    (BIT(23))
#define ADC_SAR1_PATT_TYPE_M  (ADC_SAR1_PATT_TYPE_V << ADC_SAR1_PATT_TYPE_S)
#define ADC_SAR1_PATT_TYPE_V  0x00000001U
#define ADC_SAR1_PATT_TYPE_S  23
/** ADC_SAR1_CLK_POS_SEL : R/W; bitpos: [24]; default: 0;
 *  1: sar_sel will be coded by the MSB of the 16-bit output data, in this case the
 *  resolution should not be larger than 11 bits.
 */
#define ADC_SAR1_CLK_POS_SEL    (BIT(24))
#define ADC_SAR1_CLK_POS_SEL_M  (ADC_SAR1_CLK_POS_SEL_V << ADC_SAR1_CLK_POS_SEL_S)
#define ADC_SAR1_CLK_POS_SEL_V  0x00000001U
#define ADC_SAR1_CLK_POS_SEL_S  24
/** ADC_SAR1_CONTINUE_MODE_EN : R/W; bitpos: [25]; default: 0;
 *  1: I2S input data is from SAR ADC (for DMA), 0: I2S input data is from GPIO matrix
 */
#define ADC_SAR1_CONTINUE_MODE_EN    (BIT(25))
#define ADC_SAR1_CONTINUE_MODE_EN_M  (ADC_SAR1_CONTINUE_MODE_EN_V << ADC_SAR1_CONTINUE_MODE_EN_S)
#define ADC_SAR1_CONTINUE_MODE_EN_V  0x00000001U
#define ADC_SAR1_CONTINUE_MODE_EN_S  25
/** ADC_XPD_SAR1_FORCE : R/W; bitpos: [27:26]; default: 0;
 *  force option to xpd sar1 blocks
 */
#define ADC_XPD_SAR1_FORCE    0x00000003U
#define ADC_XPD_SAR1_FORCE_M  (ADC_XPD_SAR1_FORCE_V << ADC_XPD_SAR1_FORCE_S)
#define ADC_XPD_SAR1_FORCE_V  0x00000003U
#define ADC_XPD_SAR1_FORCE_S  26

/** ADC_CTRL1_REG register
 *  Register
 */
#define ADC_CTRL1_REG (DR_REG_ADC_BASE + 0x4)
/** ADC_SAR2_TRIGGER_STOP : WT; bitpos: [0]; default: 0;
 *  need_des
 */
#define ADC_SAR2_TRIGGER_STOP    (BIT(0))
#define ADC_SAR2_TRIGGER_STOP_M  (ADC_SAR2_TRIGGER_STOP_V << ADC_SAR2_TRIGGER_STOP_S)
#define ADC_SAR2_TRIGGER_STOP_V  0x00000001U
#define ADC_SAR2_TRIGGER_STOP_S  0
/** ADC_SAR2_TRIGGER_START : WT; bitpos: [1]; default: 0;
 *  need_des
 */
#define ADC_SAR2_TRIGGER_START    (BIT(1))
#define ADC_SAR2_TRIGGER_START_M  (ADC_SAR2_TRIGGER_START_V << ADC_SAR2_TRIGGER_START_S)
#define ADC_SAR2_TRIGGER_START_V  0x00000001U
#define ADC_SAR2_TRIGGER_START_S  1
/** ADC_SAR2_TRIGGER_MODE : R/W; bitpos: [3:2]; default: 0;
 *  SAR clock divider
 */
#define ADC_SAR2_TRIGGER_MODE    0x00000003U
#define ADC_SAR2_TRIGGER_MODE_M  (ADC_SAR2_TRIGGER_MODE_V << ADC_SAR2_TRIGGER_MODE_S)
#define ADC_SAR2_TRIGGER_MODE_V  0x00000003U
#define ADC_SAR2_TRIGGER_MODE_S  2
/** ADC_SAR2_PATT_LEN : R/W; bitpos: [17:14]; default: 15;
 *  0 ~ 15 means length 1 ~ 16
 */
#define ADC_SAR2_PATT_LEN    0x0000000FU
#define ADC_SAR2_PATT_LEN_M  (ADC_SAR2_PATT_LEN_V << ADC_SAR2_PATT_LEN_S)
#define ADC_SAR2_PATT_LEN_V  0x0000000FU
#define ADC_SAR2_PATT_LEN_S  14
/** ADC_SAR2_PATT_P_CLEAR : WT; bitpos: [22]; default: 0;
 *  clear the pointer of pattern table for DIG ADC1 CTRL
 */
#define ADC_SAR2_PATT_P_CLEAR    (BIT(22))
#define ADC_SAR2_PATT_P_CLEAR_M  (ADC_SAR2_PATT_P_CLEAR_V << ADC_SAR2_PATT_P_CLEAR_S)
#define ADC_SAR2_PATT_P_CLEAR_V  0x00000001U
#define ADC_SAR2_PATT_P_CLEAR_S  22
/** ADC_SAR2_PATT_TYPE : R/W; bitpos: [23]; default: 0;
 *  1: select meas start as table update 0:select mead done as table update
 */
#define ADC_SAR2_PATT_TYPE    (BIT(23))
#define ADC_SAR2_PATT_TYPE_M  (ADC_SAR2_PATT_TYPE_V << ADC_SAR2_PATT_TYPE_S)
#define ADC_SAR2_PATT_TYPE_V  0x00000001U
#define ADC_SAR2_PATT_TYPE_S  23
/** ADC_SAR2_CLK_POS_SEL : R/W; bitpos: [24]; default: 0;
 *  1: sar_sel will be coded by the MSB of the 16-bit output data, in this case the
 *  resolution should not be larger than 11 bits.
 */
#define ADC_SAR2_CLK_POS_SEL    (BIT(24))
#define ADC_SAR2_CLK_POS_SEL_M  (ADC_SAR2_CLK_POS_SEL_V << ADC_SAR2_CLK_POS_SEL_S)
#define ADC_SAR2_CLK_POS_SEL_V  0x00000001U
#define ADC_SAR2_CLK_POS_SEL_S  24
/** ADC_SAR2_CONTINUE_MODE_EN : R/W; bitpos: [25]; default: 0;
 *  1: I2S input data is from SAR ADC (for DMA), 0: I2S input data is from GPIO matrix
 */
#define ADC_SAR2_CONTINUE_MODE_EN    (BIT(25))
#define ADC_SAR2_CONTINUE_MODE_EN_M  (ADC_SAR2_CONTINUE_MODE_EN_V << ADC_SAR2_CONTINUE_MODE_EN_S)
#define ADC_SAR2_CONTINUE_MODE_EN_V  0x00000001U
#define ADC_SAR2_CONTINUE_MODE_EN_S  25
/** ADC_XPD_SAR2_FORCE : R/W; bitpos: [27:26]; default: 0;
 *  force option to xpd sar1 blocks
 */
#define ADC_XPD_SAR2_FORCE    0x00000003U
#define ADC_XPD_SAR2_FORCE_M  (ADC_XPD_SAR2_FORCE_V << ADC_XPD_SAR2_FORCE_S)
#define ADC_XPD_SAR2_FORCE_V  0x00000003U
#define ADC_XPD_SAR2_FORCE_S  26

/** ADC_CTRL2_REG register
 *  Register
 */
#define ADC_CTRL2_REG (DR_REG_ADC_BASE + 0x8)
/** ADC_TIMER_TARGET : R/W; bitpos: [23:12]; default: 10;
 *  to set saradc timer target
 */
#define ADC_TIMER_TARGET    0x00000FFFU
#define ADC_TIMER_TARGET_M  (ADC_TIMER_TARGET_V << ADC_TIMER_TARGET_S)
#define ADC_TIMER_TARGET_V  0x00000FFFU
#define ADC_TIMER_TARGET_S  12
/** ADC_TIMER_EN : R/W; bitpos: [24]; default: 0;
 *  to enable saradc timer trigger
 */
#define ADC_TIMER_EN    (BIT(24))
#define ADC_TIMER_EN_M  (ADC_TIMER_EN_V << ADC_TIMER_EN_S)
#define ADC_TIMER_EN_V  0x00000001U
#define ADC_TIMER_EN_S  24

/** ADC_FILTER_CTRL1_REG register
 *  Register
 */
#define ADC_FILTER_CTRL1_REG (DR_REG_ADC_BASE + 0xc)
/** ADC_FILTER_FACTOR1 : R/W; bitpos: [28:26]; default: 0;
 *  need_des
 */
#define ADC_FILTER_FACTOR1    0x00000007U
#define ADC_FILTER_FACTOR1_M  (ADC_FILTER_FACTOR1_V << ADC_FILTER_FACTOR1_S)
#define ADC_FILTER_FACTOR1_V  0x00000007U
#define ADC_FILTER_FACTOR1_S  26
/** ADC_FILTER_FACTOR0 : R/W; bitpos: [31:29]; default: 0;
 *  need_des
 */
#define ADC_FILTER_FACTOR0    0x00000007U
#define ADC_FILTER_FACTOR0_M  (ADC_FILTER_FACTOR0_V << ADC_FILTER_FACTOR0_S)
#define ADC_FILTER_FACTOR0_V  0x00000007U
#define ADC_FILTER_FACTOR0_S  29

/** ADC_SAR1_PATT_TAB1_REG register
 *  Register
 */
#define ADC_SAR1_PATT_TAB1_REG (DR_REG_ADC_BASE + 0x1c)
/** ADC_SAR1_PATT_TAB1 : R/W; bitpos: [23:0]; default: 15728640;
 *  item 0 ~ 3 for pattern table 1 (each item one byte)
 */
#define ADC_SAR1_PATT_TAB1    0x00FFFFFFU
#define ADC_SAR1_PATT_TAB1_M  (ADC_SAR1_PATT_TAB1_V << ADC_SAR1_PATT_TAB1_S)
#define ADC_SAR1_PATT_TAB1_V  0x00FFFFFFU
#define ADC_SAR1_PATT_TAB1_S  0

/** ADC_SAR1_PATT_TAB2_REG register
 *  Register
 */
#define ADC_SAR1_PATT_TAB2_REG (DR_REG_ADC_BASE + 0x20)
/** ADC_SAR1_PATT_TAB2 : R/W; bitpos: [23:0]; default: 0;
 *  Item 4 ~ 7 for pattern table 1 (each item one byte)
 */
#define ADC_SAR1_PATT_TAB2    0x00FFFFFFU
#define ADC_SAR1_PATT_TAB2_M  (ADC_SAR1_PATT_TAB2_V << ADC_SAR1_PATT_TAB2_S)
#define ADC_SAR1_PATT_TAB2_V  0x00FFFFFFU
#define ADC_SAR1_PATT_TAB2_S  0

/** ADC_SAR1_PATT_TAB3_REG register
 *  Register
 */
#define ADC_SAR1_PATT_TAB3_REG (DR_REG_ADC_BASE + 0x24)
/** ADC_SAR1_PATT_TAB3 : R/W; bitpos: [23:0]; default: 0;
 *  Item 8 ~ 11 for pattern table 1 (each item one byte)
 */
#define ADC_SAR1_PATT_TAB3    0x00FFFFFFU
#define ADC_SAR1_PATT_TAB3_M  (ADC_SAR1_PATT_TAB3_V << ADC_SAR1_PATT_TAB3_S)
#define ADC_SAR1_PATT_TAB3_V  0x00FFFFFFU
#define ADC_SAR1_PATT_TAB3_S  0

/** ADC_SAR1_PATT_TAB4_REG register
 *  Register
 */
#define ADC_SAR1_PATT_TAB4_REG (DR_REG_ADC_BASE + 0x28)
/** ADC_SAR1_PATT_TAB4 : R/W; bitpos: [23:0]; default: 0;
 *  Item 12 ~ 15 for pattern table 1 (each item one byte)
 */
#define ADC_SAR1_PATT_TAB4    0x00FFFFFFU
#define ADC_SAR1_PATT_TAB4_M  (ADC_SAR1_PATT_TAB4_V << ADC_SAR1_PATT_TAB4_S)
#define ADC_SAR1_PATT_TAB4_V  0x00FFFFFFU
#define ADC_SAR1_PATT_TAB4_S  0

/** ADC_SAR2_PATT_TAB1_REG register
 *  Register
 */
#define ADC_SAR2_PATT_TAB1_REG (DR_REG_ADC_BASE + 0x2c)
/** ADC_SAR2_PATT_TAB1 : R/W; bitpos: [23:0]; default: 15728640;
 *  item 0 ~ 3 for pattern table 2 (each item one byte)
 */
#define ADC_SAR2_PATT_TAB1    0x00FFFFFFU
#define ADC_SAR2_PATT_TAB1_M  (ADC_SAR2_PATT_TAB1_V << ADC_SAR2_PATT_TAB1_S)
#define ADC_SAR2_PATT_TAB1_V  0x00FFFFFFU
#define ADC_SAR2_PATT_TAB1_S  0

/** ADC_SAR2_PATT_TAB2_REG register
 *  Register
 */
#define ADC_SAR2_PATT_TAB2_REG (DR_REG_ADC_BASE + 0x30)
/** ADC_SAR2_PATT_TAB2 : R/W; bitpos: [23:0]; default: 0;
 *  Item 4 ~ 7 for pattern table 2 (each item one byte)
 */
#define ADC_SAR2_PATT_TAB2    0x00FFFFFFU
#define ADC_SAR2_PATT_TAB2_M  (ADC_SAR2_PATT_TAB2_V << ADC_SAR2_PATT_TAB2_S)
#define ADC_SAR2_PATT_TAB2_V  0x00FFFFFFU
#define ADC_SAR2_PATT_TAB2_S  0

/** ADC_SAR2_PATT_TAB3_REG register
 *  Register
 */
#define ADC_SAR2_PATT_TAB3_REG (DR_REG_ADC_BASE + 0x34)
/** ADC_SAR2_PATT_TAB3 : R/W; bitpos: [23:0]; default: 0;
 *  Item 8 ~ 11 for pattern table 2 (each item one byte)
 */
#define ADC_SAR2_PATT_TAB3    0x00FFFFFFU
#define ADC_SAR2_PATT_TAB3_M  (ADC_SAR2_PATT_TAB3_V << ADC_SAR2_PATT_TAB3_S)
#define ADC_SAR2_PATT_TAB3_V  0x00FFFFFFU
#define ADC_SAR2_PATT_TAB3_S  0

/** ADC_SAR2_PATT_TAB4_REG register
 *  Register
 */
#define ADC_SAR2_PATT_TAB4_REG (DR_REG_ADC_BASE + 0x38)
/** ADC_SAR2_PATT_TAB4 : R/W; bitpos: [23:0]; default: 0;
 *  Item 12 ~ 15 for pattern table 2 (each item one byte)
 */
#define ADC_SAR2_PATT_TAB4    0x00FFFFFFU
#define ADC_SAR2_PATT_TAB4_M  (ADC_SAR2_PATT_TAB4_V << ADC_SAR2_PATT_TAB4_S)
#define ADC_SAR2_PATT_TAB4_V  0x00FFFFFFU
#define ADC_SAR2_PATT_TAB4_S  0

/** ADC_FILTER_CTRL0_REG register
 *  Register
 */
#define ADC_FILTER_CTRL0_REG (DR_REG_ADC_BASE + 0x3c)
/** ADC_FILTER_CHANNEL1 : R/W; bitpos: [18:14]; default: 13;
 *  need_des
 */
#define ADC_FILTER_CHANNEL1    0x0000001FU
#define ADC_FILTER_CHANNEL1_M  (ADC_FILTER_CHANNEL1_V << ADC_FILTER_CHANNEL1_S)
#define ADC_FILTER_CHANNEL1_V  0x0000001FU
#define ADC_FILTER_CHANNEL1_S  14
/** ADC_FILTER_CHANNEL0 : R/W; bitpos: [23:19]; default: 13;
 *  apb_adc1_filter_factor
 */
#define ADC_FILTER_CHANNEL0    0x0000001FU
#define ADC_FILTER_CHANNEL0_M  (ADC_FILTER_CHANNEL0_V << ADC_FILTER_CHANNEL0_S)
#define ADC_FILTER_CHANNEL0_V  0x0000001FU
#define ADC_FILTER_CHANNEL0_S  19
/** ADC_FILTER_RESET : WT; bitpos: [31]; default: 0;
 *  enable apb_adc1_filter
 */
#define ADC_FILTER_RESET    (BIT(31))
#define ADC_FILTER_RESET_M  (ADC_FILTER_RESET_V << ADC_FILTER_RESET_S)
#define ADC_FILTER_RESET_V  0x00000001U
#define ADC_FILTER_RESET_S  31

/** ADC_SAR1_DATA_STATUS_REG register
 *  Register
 */
#define ADC_SAR1_DATA_STATUS_REG (DR_REG_ADC_BASE + 0x40)
/** ADC_APB_SARADC1_DATA : RO; bitpos: [21:0]; default: 0;
 *  need_des
 */
#define ADC_APB_SARADC1_DATA    0x003FFFFFU
#define ADC_APB_SARADC1_DATA_M  (ADC_APB_SARADC1_DATA_V << ADC_APB_SARADC1_DATA_S)
#define ADC_APB_SARADC1_DATA_V  0x003FFFFFU
#define ADC_APB_SARADC1_DATA_S  0

/** ADC_SAR2_DATA_STATUS_REG register
 *  Register
 */
#define ADC_SAR2_DATA_STATUS_REG (DR_REG_ADC_BASE + 0x44)
/** ADC_APB_SARADC2_DATA : RO; bitpos: [21:0]; default: 0;
 *  need_des
 */
#define ADC_APB_SARADC2_DATA    0x003FFFFFU
#define ADC_APB_SARADC2_DATA_M  (ADC_APB_SARADC2_DATA_V << ADC_APB_SARADC2_DATA_S)
#define ADC_APB_SARADC2_DATA_V  0x003FFFFFU
#define ADC_APB_SARADC2_DATA_S  0

/** ADC_THRES0_HIGH_THRES_REG register
 *  Register
 */
#define ADC_THRES0_HIGH_THRES_REG (DR_REG_ADC_BASE + 0x48)
/** ADC_THRES0_HIGH : R/W; bitpos: [16:0]; default: 255;
 *  need_des
 */
#define ADC_THRES0_HIGH    0x0001FFFFU
#define ADC_THRES0_HIGH_M  (ADC_THRES0_HIGH_V << ADC_THRES0_HIGH_S)
#define ADC_THRES0_HIGH_V  0x0001FFFFU
#define ADC_THRES0_HIGH_S  0

/** ADC_THRES0_LOW_THRES_REG register
 *  Register
 */
#define ADC_THRES0_LOW_THRES_REG (DR_REG_ADC_BASE + 0x4c)
/** ADC_THRES0_LOW : R/W; bitpos: [16:0]; default: 0;
 *  need_des
 */
#define ADC_THRES0_LOW    0x0001FFFFU
#define ADC_THRES0_LOW_M  (ADC_THRES0_LOW_V << ADC_THRES0_LOW_S)
#define ADC_THRES0_LOW_V  0x0001FFFFU
#define ADC_THRES0_LOW_S  0

/** ADC_THRES0_CTRL_REG register
 *  Register
 */
#define ADC_THRES0_CTRL_REG (DR_REG_ADC_BASE + 0x50)
/** ADC_THRES0_CHANNEL : R/W; bitpos: [4:0]; default: 13;
 *  need_des
 */
#define ADC_THRES0_CHANNEL    0x0000001FU
#define ADC_THRES0_CHANNEL_M  (ADC_THRES0_CHANNEL_V << ADC_THRES0_CHANNEL_S)
#define ADC_THRES0_CHANNEL_V  0x0000001FU
#define ADC_THRES0_CHANNEL_S  0
/** ADC_THRES0_EN : R/W; bitpos: [5]; default: 0;
 *  need_des
 */
#define ADC_THRES0_EN    (BIT(5))
#define ADC_THRES0_EN_M  (ADC_THRES0_EN_V << ADC_THRES0_EN_S)
#define ADC_THRES0_EN_V  0x00000001U
#define ADC_THRES0_EN_S  5

/** ADC_THRES1_HIGH_THRES_REG register
 *  Register
 */
#define ADC_THRES1_HIGH_THRES_REG (DR_REG_ADC_BASE + 0x54)
/** ADC_THRES1_HIGH : R/W; bitpos: [16:0]; default: 255;
 *  need_des
 */
#define ADC_THRES1_HIGH    0x0001FFFFU
#define ADC_THRES1_HIGH_M  (ADC_THRES1_HIGH_V << ADC_THRES1_HIGH_S)
#define ADC_THRES1_HIGH_V  0x0001FFFFU
#define ADC_THRES1_HIGH_S  0

/** ADC_THRES1_LOW_THRES_REG register
 *  Register
 */
#define ADC_THRES1_LOW_THRES_REG (DR_REG_ADC_BASE + 0x58)
/** ADC_THRES1_LOW : R/W; bitpos: [16:0]; default: 0;
 *  need_des
 */
#define ADC_THRES1_LOW    0x0001FFFFU
#define ADC_THRES1_LOW_M  (ADC_THRES1_LOW_V << ADC_THRES1_LOW_S)
#define ADC_THRES1_LOW_V  0x0001FFFFU
#define ADC_THRES1_LOW_S  0

/** ADC_THRES1_CTRL_REG register
 *  Register
 */
#define ADC_THRES1_CTRL_REG (DR_REG_ADC_BASE + 0x5c)
/** ADC_THRES1_CHANNEL : R/W; bitpos: [4:0]; default: 13;
 *  need_des
 */
#define ADC_THRES1_CHANNEL    0x0000001FU
#define ADC_THRES1_CHANNEL_M  (ADC_THRES1_CHANNEL_V << ADC_THRES1_CHANNEL_S)
#define ADC_THRES1_CHANNEL_V  0x0000001FU
#define ADC_THRES1_CHANNEL_S  0
/** ADC_THRES1_EN : R/W; bitpos: [5]; default: 0;
 *  need_des
 */
#define ADC_THRES1_EN    (BIT(5))
#define ADC_THRES1_EN_M  (ADC_THRES1_EN_V << ADC_THRES1_EN_S)
#define ADC_THRES1_EN_V  0x00000001U
#define ADC_THRES1_EN_S  5

/** ADC_THRES_CTRL_REG register
 *  Register
 */
#define ADC_THRES_CTRL_REG (DR_REG_ADC_BASE + 0x60)
/** ADC_THRES_ALL_EN : R/W; bitpos: [27]; default: 0;
 *  need_des
 */
#define ADC_THRES_ALL_EN    (BIT(27))
#define ADC_THRES_ALL_EN_M  (ADC_THRES_ALL_EN_V << ADC_THRES_ALL_EN_S)
#define ADC_THRES_ALL_EN_V  0x00000001U
#define ADC_THRES_ALL_EN_S  27

/** ADC_INT_ENA_REG register
 *  Register
 */
#define ADC_INT_ENA_REG (DR_REG_ADC_BASE + 0x64)
/** ADC_FIFO_OVERFLOW_INT_ENA : R/W; bitpos: [25]; default: 0;
 *  need_des
 */
#define ADC_FIFO_OVERFLOW_INT_ENA    (BIT(25))
#define ADC_FIFO_OVERFLOW_INT_ENA_M  (ADC_FIFO_OVERFLOW_INT_ENA_V << ADC_FIFO_OVERFLOW_INT_ENA_S)
#define ADC_FIFO_OVERFLOW_INT_ENA_V  0x00000001U
#define ADC_FIFO_OVERFLOW_INT_ENA_S  25
/** ADC_THRES1_LOW_INT_ENA : R/W; bitpos: [26]; default: 0;
 *  need_des
 */
#define ADC_THRES1_LOW_INT_ENA    (BIT(26))
#define ADC_THRES1_LOW_INT_ENA_M  (ADC_THRES1_LOW_INT_ENA_V << ADC_THRES1_LOW_INT_ENA_S)
#define ADC_THRES1_LOW_INT_ENA_V  0x00000001U
#define ADC_THRES1_LOW_INT_ENA_S  26
/** ADC_THRES0_LOW_INT_ENA : R/W; bitpos: [27]; default: 0;
 *  need_des
 */
#define ADC_THRES0_LOW_INT_ENA    (BIT(27))
#define ADC_THRES0_LOW_INT_ENA_M  (ADC_THRES0_LOW_INT_ENA_V << ADC_THRES0_LOW_INT_ENA_S)
#define ADC_THRES0_LOW_INT_ENA_V  0x00000001U
#define ADC_THRES0_LOW_INT_ENA_S  27
/** ADC_THRES1_HIGH_INT_ENA : R/W; bitpos: [28]; default: 0;
 *  need_des
 */
#define ADC_THRES1_HIGH_INT_ENA    (BIT(28))
#define ADC_THRES1_HIGH_INT_ENA_M  (ADC_THRES1_HIGH_INT_ENA_V << ADC_THRES1_HIGH_INT_ENA_S)
#define ADC_THRES1_HIGH_INT_ENA_V  0x00000001U
#define ADC_THRES1_HIGH_INT_ENA_S  28
/** ADC_THRES0_HIGH_INT_ENA : R/W; bitpos: [29]; default: 0;
 *  need_des
 */
#define ADC_THRES0_HIGH_INT_ENA    (BIT(29))
#define ADC_THRES0_HIGH_INT_ENA_M  (ADC_THRES0_HIGH_INT_ENA_V << ADC_THRES0_HIGH_INT_ENA_S)
#define ADC_THRES0_HIGH_INT_ENA_V  0x00000001U
#define ADC_THRES0_HIGH_INT_ENA_S  29
/** ADC_SAR2_DONE_INT_ENA : R/W; bitpos: [30]; default: 0;
 *  need_des
 */
#define ADC_SAR2_DONE_INT_ENA    (BIT(30))
#define ADC_SAR2_DONE_INT_ENA_M  (ADC_SAR2_DONE_INT_ENA_V << ADC_SAR2_DONE_INT_ENA_S)
#define ADC_SAR2_DONE_INT_ENA_V  0x00000001U
#define ADC_SAR2_DONE_INT_ENA_S  30
/** ADC_SAR1_DONE_INT_ENA : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define ADC_SAR1_DONE_INT_ENA    (BIT(31))
#define ADC_SAR1_DONE_INT_ENA_M  (ADC_SAR1_DONE_INT_ENA_V << ADC_SAR1_DONE_INT_ENA_S)
#define ADC_SAR1_DONE_INT_ENA_V  0x00000001U
#define ADC_SAR1_DONE_INT_ENA_S  31

/** ADC_INT_RAW_REG register
 *  Register
 */
#define ADC_INT_RAW_REG (DR_REG_ADC_BASE + 0x68)
/** ADC_FIFO_OVERFLOW_INT_RAW : R/WTC/SS; bitpos: [25]; default: 0;
 *  need_des
 */
#define ADC_FIFO_OVERFLOW_INT_RAW    (BIT(25))
#define ADC_FIFO_OVERFLOW_INT_RAW_M  (ADC_FIFO_OVERFLOW_INT_RAW_V << ADC_FIFO_OVERFLOW_INT_RAW_S)
#define ADC_FIFO_OVERFLOW_INT_RAW_V  0x00000001U
#define ADC_FIFO_OVERFLOW_INT_RAW_S  25
/** ADC_THRES1_LOW_INT_RAW : R/WTC/SS; bitpos: [26]; default: 0;
 *  need_des
 */
#define ADC_THRES1_LOW_INT_RAW    (BIT(26))
#define ADC_THRES1_LOW_INT_RAW_M  (ADC_THRES1_LOW_INT_RAW_V << ADC_THRES1_LOW_INT_RAW_S)
#define ADC_THRES1_LOW_INT_RAW_V  0x00000001U
#define ADC_THRES1_LOW_INT_RAW_S  26
/** ADC_THRES0_LOW_INT_RAW : R/WTC/SS; bitpos: [27]; default: 0;
 *  need_des
 */
#define ADC_THRES0_LOW_INT_RAW    (BIT(27))
#define ADC_THRES0_LOW_INT_RAW_M  (ADC_THRES0_LOW_INT_RAW_V << ADC_THRES0_LOW_INT_RAW_S)
#define ADC_THRES0_LOW_INT_RAW_V  0x00000001U
#define ADC_THRES0_LOW_INT_RAW_S  27
/** ADC_THRES1_HIGH_INT_RAW : R/WTC/SS; bitpos: [28]; default: 0;
 *  need_des
 */
#define ADC_THRES1_HIGH_INT_RAW    (BIT(28))
#define ADC_THRES1_HIGH_INT_RAW_M  (ADC_THRES1_HIGH_INT_RAW_V << ADC_THRES1_HIGH_INT_RAW_S)
#define ADC_THRES1_HIGH_INT_RAW_V  0x00000001U
#define ADC_THRES1_HIGH_INT_RAW_S  28
/** ADC_THRES0_HIGH_INT_RAW : R/WTC/SS; bitpos: [29]; default: 0;
 *  need_des
 */
#define ADC_THRES0_HIGH_INT_RAW    (BIT(29))
#define ADC_THRES0_HIGH_INT_RAW_M  (ADC_THRES0_HIGH_INT_RAW_V << ADC_THRES0_HIGH_INT_RAW_S)
#define ADC_THRES0_HIGH_INT_RAW_V  0x00000001U
#define ADC_THRES0_HIGH_INT_RAW_S  29
/** ADC_SAR2_DONE_INT_RAW : R/WTC/SS; bitpos: [30]; default: 0;
 *  need_des
 */
#define ADC_SAR2_DONE_INT_RAW    (BIT(30))
#define ADC_SAR2_DONE_INT_RAW_M  (ADC_SAR2_DONE_INT_RAW_V << ADC_SAR2_DONE_INT_RAW_S)
#define ADC_SAR2_DONE_INT_RAW_V  0x00000001U
#define ADC_SAR2_DONE_INT_RAW_S  30
/** ADC_SAR1_DONE_INT_RAW : R/WTC/SS; bitpos: [31]; default: 0;
 *  need_des
 */
#define ADC_SAR1_DONE_INT_RAW    (BIT(31))
#define ADC_SAR1_DONE_INT_RAW_M  (ADC_SAR1_DONE_INT_RAW_V << ADC_SAR1_DONE_INT_RAW_S)
#define ADC_SAR1_DONE_INT_RAW_V  0x00000001U
#define ADC_SAR1_DONE_INT_RAW_S  31

/** ADC_INT_ST_REG register
 *  Register
 */
#define ADC_INT_ST_REG (DR_REG_ADC_BASE + 0x6c)
/** ADC_FIFO_OVERFLOW_INT_ST : RO; bitpos: [25]; default: 0;
 *  need_des
 */
#define ADC_FIFO_OVERFLOW_INT_ST    (BIT(25))
#define ADC_FIFO_OVERFLOW_INT_ST_M  (ADC_FIFO_OVERFLOW_INT_ST_V << ADC_FIFO_OVERFLOW_INT_ST_S)
#define ADC_FIFO_OVERFLOW_INT_ST_V  0x00000001U
#define ADC_FIFO_OVERFLOW_INT_ST_S  25
/** ADC_THRES1_LOW_INT_ST : RO; bitpos: [26]; default: 0;
 *  need_des
 */
#define ADC_THRES1_LOW_INT_ST    (BIT(26))
#define ADC_THRES1_LOW_INT_ST_M  (ADC_THRES1_LOW_INT_ST_V << ADC_THRES1_LOW_INT_ST_S)
#define ADC_THRES1_LOW_INT_ST_V  0x00000001U
#define ADC_THRES1_LOW_INT_ST_S  26
/** ADC_THRES0_LOW_INT_ST : RO; bitpos: [27]; default: 0;
 *  need_des
 */
#define ADC_THRES0_LOW_INT_ST    (BIT(27))
#define ADC_THRES0_LOW_INT_ST_M  (ADC_THRES0_LOW_INT_ST_V << ADC_THRES0_LOW_INT_ST_S)
#define ADC_THRES0_LOW_INT_ST_V  0x00000001U
#define ADC_THRES0_LOW_INT_ST_S  27
/** ADC_THRES1_HIGH_INT_ST : RO; bitpos: [28]; default: 0;
 *  need_des
 */
#define ADC_THRES1_HIGH_INT_ST    (BIT(28))
#define ADC_THRES1_HIGH_INT_ST_M  (ADC_THRES1_HIGH_INT_ST_V << ADC_THRES1_HIGH_INT_ST_S)
#define ADC_THRES1_HIGH_INT_ST_V  0x00000001U
#define ADC_THRES1_HIGH_INT_ST_S  28
/** ADC_THRES0_HIGH_INT_ST : RO; bitpos: [29]; default: 0;
 *  need_des
 */
#define ADC_THRES0_HIGH_INT_ST    (BIT(29))
#define ADC_THRES0_HIGH_INT_ST_M  (ADC_THRES0_HIGH_INT_ST_V << ADC_THRES0_HIGH_INT_ST_S)
#define ADC_THRES0_HIGH_INT_ST_V  0x00000001U
#define ADC_THRES0_HIGH_INT_ST_S  29
/** ADC_SAR2_DONE_INT_ST : RO; bitpos: [30]; default: 0;
 *  need_des
 */
#define ADC_SAR2_DONE_INT_ST    (BIT(30))
#define ADC_SAR2_DONE_INT_ST_M  (ADC_SAR2_DONE_INT_ST_V << ADC_SAR2_DONE_INT_ST_S)
#define ADC_SAR2_DONE_INT_ST_V  0x00000001U
#define ADC_SAR2_DONE_INT_ST_S  30
/** ADC_SAR1_DONE_INT_ST : RO; bitpos: [31]; default: 0;
 *  need_des
 */
#define ADC_SAR1_DONE_INT_ST    (BIT(31))
#define ADC_SAR1_DONE_INT_ST_M  (ADC_SAR1_DONE_INT_ST_V << ADC_SAR1_DONE_INT_ST_S)
#define ADC_SAR1_DONE_INT_ST_V  0x00000001U
#define ADC_SAR1_DONE_INT_ST_S  31

/** ADC_INT_CLR_REG register
 *  Register
 */
#define ADC_INT_CLR_REG (DR_REG_ADC_BASE + 0x70)
/** ADC_FIFO_OVERFLOW_INT_CLR : WT; bitpos: [25]; default: 0;
 *  need_des
 */
#define ADC_FIFO_OVERFLOW_INT_CLR    (BIT(25))
#define ADC_FIFO_OVERFLOW_INT_CLR_M  (ADC_FIFO_OVERFLOW_INT_CLR_V << ADC_FIFO_OVERFLOW_INT_CLR_S)
#define ADC_FIFO_OVERFLOW_INT_CLR_V  0x00000001U
#define ADC_FIFO_OVERFLOW_INT_CLR_S  25
/** ADC_THRES1_LOW_INT_CLR : WT; bitpos: [26]; default: 0;
 *  need_des
 */
#define ADC_THRES1_LOW_INT_CLR    (BIT(26))
#define ADC_THRES1_LOW_INT_CLR_M  (ADC_THRES1_LOW_INT_CLR_V << ADC_THRES1_LOW_INT_CLR_S)
#define ADC_THRES1_LOW_INT_CLR_V  0x00000001U
#define ADC_THRES1_LOW_INT_CLR_S  26
/** ADC_THRES0_LOW_INT_CLR : WT; bitpos: [27]; default: 0;
 *  need_des
 */
#define ADC_THRES0_LOW_INT_CLR    (BIT(27))
#define ADC_THRES0_LOW_INT_CLR_M  (ADC_THRES0_LOW_INT_CLR_V << ADC_THRES0_LOW_INT_CLR_S)
#define ADC_THRES0_LOW_INT_CLR_V  0x00000001U
#define ADC_THRES0_LOW_INT_CLR_S  27
/** ADC_THRES1_HIGH_INT_CLR : WT; bitpos: [28]; default: 0;
 *  need_des
 */
#define ADC_THRES1_HIGH_INT_CLR    (BIT(28))
#define ADC_THRES1_HIGH_INT_CLR_M  (ADC_THRES1_HIGH_INT_CLR_V << ADC_THRES1_HIGH_INT_CLR_S)
#define ADC_THRES1_HIGH_INT_CLR_V  0x00000001U
#define ADC_THRES1_HIGH_INT_CLR_S  28
/** ADC_THRES0_HIGH_INT_CLR : WT; bitpos: [29]; default: 0;
 *  need_des
 */
#define ADC_THRES0_HIGH_INT_CLR    (BIT(29))
#define ADC_THRES0_HIGH_INT_CLR_M  (ADC_THRES0_HIGH_INT_CLR_V << ADC_THRES0_HIGH_INT_CLR_S)
#define ADC_THRES0_HIGH_INT_CLR_V  0x00000001U
#define ADC_THRES0_HIGH_INT_CLR_S  29
/** ADC_SAR2_DONE_INT_CLR : WT; bitpos: [30]; default: 0;
 *  need_des
 */
#define ADC_SAR2_DONE_INT_CLR    (BIT(30))
#define ADC_SAR2_DONE_INT_CLR_M  (ADC_SAR2_DONE_INT_CLR_V << ADC_SAR2_DONE_INT_CLR_S)
#define ADC_SAR2_DONE_INT_CLR_V  0x00000001U
#define ADC_SAR2_DONE_INT_CLR_S  30
/** ADC_SAR1_DONE_INT_CLR : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define ADC_SAR1_DONE_INT_CLR    (BIT(31))
#define ADC_SAR1_DONE_INT_CLR_M  (ADC_SAR1_DONE_INT_CLR_V << ADC_SAR1_DONE_INT_CLR_S)
#define ADC_SAR1_DONE_INT_CLR_V  0x00000001U
#define ADC_SAR1_DONE_INT_CLR_S  31

/** ADC_DMA_CONF_REG register
 *  Register
 */
#define ADC_DMA_CONF_REG (DR_REG_ADC_BASE + 0x74)
/** ADC_APB_ADC_EOF_NUM : R/W; bitpos: [15:0]; default: 255;
 *  the dma_in_suc_eof gen when sample cnt = spi_eof_num
 */
#define ADC_APB_ADC_EOF_NUM    0x0000FFFFU
#define ADC_APB_ADC_EOF_NUM_M  (ADC_APB_ADC_EOF_NUM_V << ADC_APB_ADC_EOF_NUM_S)
#define ADC_APB_ADC_EOF_NUM_V  0x0000FFFFU
#define ADC_APB_ADC_EOF_NUM_S  0
/** ADC_DMA_EOF_NUM_CLR : WT; bitpos: [16]; default: 0;
 *  clear eof cnt
 */
#define ADC_DMA_EOF_NUM_CLR    (BIT(16))
#define ADC_DMA_EOF_NUM_CLR_M  (ADC_DMA_EOF_NUM_CLR_V << ADC_DMA_EOF_NUM_CLR_S)
#define ADC_DMA_EOF_NUM_CLR_V  0x00000001U
#define ADC_DMA_EOF_NUM_CLR_S  16
/** ADC_APB_ADC_TRANS : R/W; bitpos: [31]; default: 0;
 *  enable apb_adc use spi_dma
 */
#define ADC_APB_ADC_TRANS    (BIT(31))
#define ADC_APB_ADC_TRANS_M  (ADC_APB_ADC_TRANS_V << ADC_APB_ADC_TRANS_S)
#define ADC_APB_ADC_TRANS_V  0x00000001U
#define ADC_APB_ADC_TRANS_S  31

/** ADC_REF_CONTROL_REG register
 *  Register
 */
#define ADC_REF_CONTROL_REG (DR_REG_ADC_BASE + 0x78)
/** ADC_RTC_REF_DELAY : R/W; bitpos: [29]; default: 0;
 *  control the refgen short vdd
 */
#define ADC_RTC_REF_DELAY    (BIT(29))
#define ADC_RTC_REF_DELAY_M  (ADC_RTC_REF_DELAY_V << ADC_RTC_REF_DELAY_S)
#define ADC_RTC_REF_DELAY_V  0x00000001U
#define ADC_RTC_REF_DELAY_S  29
/** ADC_RTC_PRE_CHARGE : R/W; bitpos: [30]; default: 0;
 *  control the refgen module pre charge
 */
#define ADC_RTC_PRE_CHARGE    (BIT(30))
#define ADC_RTC_PRE_CHARGE_M  (ADC_RTC_PRE_CHARGE_V << ADC_RTC_PRE_CHARGE_S)
#define ADC_RTC_PRE_CHARGE_V  0x00000001U
#define ADC_RTC_PRE_CHARGE_S  30
/** ADC_RTC_XPD_REFGEN : R/W; bitpos: [31]; default: 0;
 *  control the refgen module power up
 */
#define ADC_RTC_XPD_REFGEN    (BIT(31))
#define ADC_RTC_XPD_REFGEN_M  (ADC_RTC_XPD_REFGEN_V << ADC_RTC_XPD_REFGEN_S)
#define ADC_RTC_XPD_REFGEN_V  0x00000001U
#define ADC_RTC_XPD_REFGEN_S  31

/** ADC_RND_ECO_LOW_REG register
 *  Register
 */
#define ADC_RND_ECO_LOW_REG (DR_REG_ADC_BASE + 0x7c)
/** ADC_ADC_RND_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  rnd eco low
 */
#define ADC_ADC_RND_ECO_LOW    0xFFFFFFFFU
#define ADC_ADC_RND_ECO_LOW_M  (ADC_ADC_RND_ECO_LOW_V << ADC_ADC_RND_ECO_LOW_S)
#define ADC_ADC_RND_ECO_LOW_V  0xFFFFFFFFU
#define ADC_ADC_RND_ECO_LOW_S  0

/** ADC_RND_ECO_HIGH_REG register
 *  Register
 */
#define ADC_RND_ECO_HIGH_REG (DR_REG_ADC_BASE + 0x80)
/** ADC_ADC_RND_ECO_HIGH : R/W; bitpos: [31:0]; default: 4294967295;
 *  rnd eco high
 */
#define ADC_ADC_RND_ECO_HIGH    0xFFFFFFFFU
#define ADC_ADC_RND_ECO_HIGH_M  (ADC_ADC_RND_ECO_HIGH_V << ADC_ADC_RND_ECO_HIGH_S)
#define ADC_ADC_RND_ECO_HIGH_V  0xFFFFFFFFU
#define ADC_ADC_RND_ECO_HIGH_S  0

/** ADC_RND_ECO_CS_REG register
 *  Register
 */
#define ADC_RND_ECO_CS_REG (DR_REG_ADC_BASE + 0x84)
/** ADC_RND_ECO_EN : R/W; bitpos: [0]; default: 0;
 *  need_des
 */
#define ADC_RND_ECO_EN    (BIT(0))
#define ADC_RND_ECO_EN_M  (ADC_RND_ECO_EN_V << ADC_RND_ECO_EN_S)
#define ADC_RND_ECO_EN_V  0x00000001U
#define ADC_RND_ECO_EN_S  0
/** ADC_RND_ECO_RESULT : RO; bitpos: [1]; default: 0;
 *  need_des
 */
#define ADC_RND_ECO_RESULT    (BIT(1))
#define ADC_RND_ECO_RESULT_M  (ADC_RND_ECO_RESULT_V << ADC_RND_ECO_RESULT_S)
#define ADC_RND_ECO_RESULT_V  0x00000001U
#define ADC_RND_ECO_RESULT_S  1

/** ADC_CTRL_DATE_REG register
 *  Register
 */
#define ADC_CTRL_DATE_REG (DR_REG_ADC_BASE + 0x3fc)
/** ADC_CTRL_DATE : R/W; bitpos: [30:0]; default: 38830624;
 *  need_des
 */
#define ADC_CTRL_DATE    0x7FFFFFFFU
#define ADC_CTRL_DATE_M  (ADC_CTRL_DATE_V << ADC_CTRL_DATE_S)
#define ADC_CTRL_DATE_V  0x7FFFFFFFU
#define ADC_CTRL_DATE_S  0
/** ADC_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define ADC_CLK_EN    (BIT(31))
#define ADC_CLK_EN_M  (ADC_CLK_EN_V << ADC_CLK_EN_S)
#define ADC_CLK_EN_V  0x00000001U
#define ADC_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
