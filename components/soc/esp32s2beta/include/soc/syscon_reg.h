// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_SYSCON_REG_H_
#define _SOC_SYSCON_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define SYSCON_SYSCLK_CONF_REG          (DR_REG_SYSCON_BASE + 0x000)
/* SYSCON_SOC_CLK_SEL : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define SYSCON_SOC_CLK_SEL  0x00000003
#define SYSCON_SOC_CLK_SEL_M  ((SYSCON_SOC_CLK_SEL_V)<<(SYSCON_SOC_CLK_SEL_S))
#define SYSCON_SOC_CLK_SEL_V  0x3
#define SYSCON_SOC_CLK_SEL_S  14
#define SYSCON_SOC_CLK_SEL_XTL    0
#define SYSCON_SOC_CLK_SEL_PLL    1
#define SYSCON_SOC_CLK_SEL_8M     2
#define SYSCON_SOC_CLK_SEL_APLL   3
/* SYSCON_RST_TICK_CNT : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SYSCON_RST_TICK_CNT  (BIT(12))
#define SYSCON_RST_TICK_CNT_M  (BIT(12))
#define SYSCON_RST_TICK_CNT_V  0x1
#define SYSCON_RST_TICK_CNT_S  12
/* SYSCON_CLK_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SYSCON_CLK_EN  (BIT(11))
#define SYSCON_CLK_EN_M  (BIT(11))
#define SYSCON_CLK_EN_V  0x1
#define SYSCON_CLK_EN_S  11
/* SYSCON_CLK_320M_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SYSCON_CLK_320M_EN  (BIT(10))
#define SYSCON_CLK_320M_EN_M  (BIT(10))
#define SYSCON_CLK_320M_EN_V  0x1
#define SYSCON_CLK_320M_EN_S  10
/* SYSCON_PRE_DIV_CNT : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define SYSCON_PRE_DIV_CNT  0x000003FF
#define SYSCON_PRE_DIV_CNT_M  ((SYSCON_PRE_DIV_CNT_V)<<(SYSCON_PRE_DIV_CNT_S))
#define SYSCON_PRE_DIV_CNT_V  0x3FF
#define SYSCON_PRE_DIV_CNT_S  0

#define SYSCON_TICK_CONF_REG          (DR_REG_SYSCON_BASE + 0x004)
/* SYSCON_TICK_ENABLE : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: */
#define SYSCON_TICK_ENABLE  (BIT(16))
#define SYSCON_TICK_ENABLE_M  (BIT(16))
#define SYSCON_TICK_ENABLE_V  0x1
#define SYSCON_TICK_ENABLE_S  16
/* SYSCON_CK8M_TICK_NUM : R/W ;bitpos:[15:8] ;default: 8'd7 ; */
/*description: */
#define SYSCON_CK8M_TICK_NUM  0x000000FF
#define SYSCON_CK8M_TICK_NUM_M  ((SYSCON_CK8M_TICK_NUM_V)<<(SYSCON_CK8M_TICK_NUM_S))
#define SYSCON_CK8M_TICK_NUM_V  0xFF
#define SYSCON_CK8M_TICK_NUM_S  8
/* SYSCON_XTAL_TICK_NUM : R/W ;bitpos:[7:0] ;default: 8'd39 ; */
/*description: */
#define SYSCON_XTAL_TICK_NUM  0x000000FF
#define SYSCON_XTAL_TICK_NUM_M  ((SYSCON_XTAL_TICK_NUM_V)<<(SYSCON_XTAL_TICK_NUM_S))
#define SYSCON_XTAL_TICK_NUM_V  0xFF
#define SYSCON_XTAL_TICK_NUM_S  0

#define SYSCON_SARADC_CTRL_REG          (DR_REG_SYSCON_BASE + 0x008)
/* SYSCON_SARADC_XPD_SAR_FORCE : R/W ;bitpos:[28:27] ;default: 2'd0 ; */
/*description: force option to xpd sar blocks*/
#define SYSCON_SARADC_XPD_SAR_FORCE  0x00000003
#define SYSCON_SARADC_XPD_SAR_FORCE_M  ((SYSCON_SARADC_XPD_SAR_FORCE_V)<<(SYSCON_SARADC_XPD_SAR_FORCE_S))
#define SYSCON_SARADC_XPD_SAR_FORCE_V  0x3
#define SYSCON_SARADC_XPD_SAR_FORCE_S  27
/* SYSCON_SARADC_DATA_TO_I2S : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 1: I2S input data is from SAR ADC (for DMA)  0: I2S input data
 is from GPIO matrix*/
#define SYSCON_SARADC_DATA_TO_I2S  (BIT(26))
#define SYSCON_SARADC_DATA_TO_I2S_M  (BIT(26))
#define SYSCON_SARADC_DATA_TO_I2S_V  0x1
#define SYSCON_SARADC_DATA_TO_I2S_S  26
/* SYSCON_SARADC_DATA_SAR_SEL : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1: sar_sel will be coded by the MSB of the 16-bit output data
  in this case the resolution should not be larger than 11 bits.*/
#define SYSCON_SARADC_DATA_SAR_SEL  (BIT(25))
#define SYSCON_SARADC_DATA_SAR_SEL_M  (BIT(25))
#define SYSCON_SARADC_DATA_SAR_SEL_V  0x1
#define SYSCON_SARADC_DATA_SAR_SEL_S  25
/* SYSCON_SARADC_SAR2_PATT_P_CLEAR : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: clear the pointer of pattern table for DIG ADC2 CTRL*/
#define SYSCON_SARADC_SAR2_PATT_P_CLEAR  (BIT(24))
#define SYSCON_SARADC_SAR2_PATT_P_CLEAR_M  (BIT(24))
#define SYSCON_SARADC_SAR2_PATT_P_CLEAR_V  0x1
#define SYSCON_SARADC_SAR2_PATT_P_CLEAR_S  24
/* SYSCON_SARADC_SAR1_PATT_P_CLEAR : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: clear the pointer of pattern table for DIG ADC1 CTRL*/
#define SYSCON_SARADC_SAR1_PATT_P_CLEAR  (BIT(23))
#define SYSCON_SARADC_SAR1_PATT_P_CLEAR_M  (BIT(23))
#define SYSCON_SARADC_SAR1_PATT_P_CLEAR_V  0x1
#define SYSCON_SARADC_SAR1_PATT_P_CLEAR_S  23
/* SYSCON_SARADC_SAR2_PATT_LEN : R/W ;bitpos:[22:19] ;default: 4'd15 ; */
/*description: 0 ~ 15 means length 1 ~ 16*/
#define SYSCON_SARADC_SAR2_PATT_LEN  0x0000000F
#define SYSCON_SARADC_SAR2_PATT_LEN_M  ((SYSCON_SARADC_SAR2_PATT_LEN_V)<<(SYSCON_SARADC_SAR2_PATT_LEN_S))
#define SYSCON_SARADC_SAR2_PATT_LEN_V  0xF
#define SYSCON_SARADC_SAR2_PATT_LEN_S  19
/* SYSCON_SARADC_SAR1_PATT_LEN : R/W ;bitpos:[18:15] ;default: 4'd15 ; */
/*description: 0 ~ 15 means length 1 ~ 16*/
#define SYSCON_SARADC_SAR1_PATT_LEN  0x0000000F
#define SYSCON_SARADC_SAR1_PATT_LEN_M  ((SYSCON_SARADC_SAR1_PATT_LEN_V)<<(SYSCON_SARADC_SAR1_PATT_LEN_S))
#define SYSCON_SARADC_SAR1_PATT_LEN_V  0xF
#define SYSCON_SARADC_SAR1_PATT_LEN_S  15
/* SYSCON_SARADC_SAR_CLK_DIV : R/W ;bitpos:[14:7] ;default: 8'd4 ; */
/*description: SAR clock divider*/
#define SYSCON_SARADC_SAR_CLK_DIV  0x000000FF
#define SYSCON_SARADC_SAR_CLK_DIV_M  ((SYSCON_SARADC_SAR_CLK_DIV_V)<<(SYSCON_SARADC_SAR_CLK_DIV_S))
#define SYSCON_SARADC_SAR_CLK_DIV_V  0xFF
#define SYSCON_SARADC_SAR_CLK_DIV_S  7
/* SYSCON_SARADC_SAR_CLK_GATED : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define SYSCON_SARADC_SAR_CLK_GATED  (BIT(6))
#define SYSCON_SARADC_SAR_CLK_GATED_M  (BIT(6))
#define SYSCON_SARADC_SAR_CLK_GATED_V  0x1
#define SYSCON_SARADC_SAR_CLK_GATED_S  6
/* SYSCON_SARADC_SAR_SEL : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: 0: SAR1  1: SAR2  only work for single SAR mode*/
#define SYSCON_SARADC_SAR_SEL  (BIT(5))
#define SYSCON_SARADC_SAR_SEL_M  (BIT(5))
#define SYSCON_SARADC_SAR_SEL_V  0x1
#define SYSCON_SARADC_SAR_SEL_S  5
/* SYSCON_SARADC_WORK_MODE : R/W ;bitpos:[4:3] ;default: 2'd0 ; */
/*description: 0: single mode  1: double mode  2: alternate mode*/
#define SYSCON_SARADC_WORK_MODE  0x00000003
#define SYSCON_SARADC_WORK_MODE_M  ((SYSCON_SARADC_WORK_MODE_V)<<(SYSCON_SARADC_WORK_MODE_S))
#define SYSCON_SARADC_WORK_MODE_V  0x3
#define SYSCON_SARADC_WORK_MODE_S  3
/* SYSCON_SARADC_START : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define SYSCON_SARADC_START  (BIT(1))
#define SYSCON_SARADC_START_M  (BIT(1))
#define SYSCON_SARADC_START_V  0x1
#define SYSCON_SARADC_START_S  1
/* SYSCON_SARADC_START_FORCE : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define SYSCON_SARADC_START_FORCE  (BIT(0))
#define SYSCON_SARADC_START_FORCE_M  (BIT(0))
#define SYSCON_SARADC_START_FORCE_V  0x1
#define SYSCON_SARADC_START_FORCE_S  0

#define SYSCON_SARADC_CTRL2_REG          (DR_REG_SYSCON_BASE + 0x00C)
/* SYSCON_SARADC_TIMER_EN : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: to enable saradc timer trigger*/
#define SYSCON_SARADC_TIMER_EN  (BIT(20))
#define SYSCON_SARADC_TIMER_EN_M  (BIT(20))
#define SYSCON_SARADC_TIMER_EN_V  0x1
#define SYSCON_SARADC_TIMER_EN_S  20
/* SYSCON_SARADC_TIMER_TARGET : R/W ;bitpos:[19:12] ;default: 8'd10 ; */
/*description: to set saradc timer target*/
#define SYSCON_SARADC_TIMER_TARGET  0x000000FF
#define SYSCON_SARADC_TIMER_TARGET_M  ((SYSCON_SARADC_TIMER_TARGET_V)<<(SYSCON_SARADC_TIMER_TARGET_S))
#define SYSCON_SARADC_TIMER_TARGET_V  0xFF
#define SYSCON_SARADC_TIMER_TARGET_S  12
/* SYSCON_SARADC_TIMER_SEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: 1: select saradc timer 0: i2s_ws trigger*/
#define SYSCON_SARADC_TIMER_SEL  (BIT(11))
#define SYSCON_SARADC_TIMER_SEL_M  (BIT(11))
#define SYSCON_SARADC_TIMER_SEL_V  0x1
#define SYSCON_SARADC_TIMER_SEL_S  11
/* SYSCON_SARADC_SAR2_INV : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: 1: data to DIG ADC2 CTRL is inverted  otherwise not*/
#define SYSCON_SARADC_SAR2_INV  (BIT(10))
#define SYSCON_SARADC_SAR2_INV_M  (BIT(10))
#define SYSCON_SARADC_SAR2_INV_V  0x1
#define SYSCON_SARADC_SAR2_INV_S  10
/* SYSCON_SARADC_SAR1_INV : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: 1: data to DIG ADC1 CTRL is inverted  otherwise not*/
#define SYSCON_SARADC_SAR1_INV  (BIT(9))
#define SYSCON_SARADC_SAR1_INV_M  (BIT(9))
#define SYSCON_SARADC_SAR1_INV_V  0x1
#define SYSCON_SARADC_SAR1_INV_S  9
/* SYSCON_SARADC_MAX_MEAS_NUM : R/W ;bitpos:[8:1] ;default: 8'd255 ; */
/*description: max conversion number*/
#define SYSCON_SARADC_MAX_MEAS_NUM  0x000000FF
#define SYSCON_SARADC_MAX_MEAS_NUM_M  ((SYSCON_SARADC_MAX_MEAS_NUM_V)<<(SYSCON_SARADC_MAX_MEAS_NUM_S))
#define SYSCON_SARADC_MAX_MEAS_NUM_V  0xFF
#define SYSCON_SARADC_MAX_MEAS_NUM_S  1
/* SYSCON_SARADC_MEAS_NUM_LIMIT : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define SYSCON_SARADC_MEAS_NUM_LIMIT  (BIT(0))
#define SYSCON_SARADC_MEAS_NUM_LIMIT_M  (BIT(0))
#define SYSCON_SARADC_MEAS_NUM_LIMIT_V  0x1
#define SYSCON_SARADC_MEAS_NUM_LIMIT_S  0

#define SYSCON_SARADC_FSM_REG          (DR_REG_SYSCON_BASE + 0x010)
/* SYSCON_SARADC_SAMPLE_CYCLE : R/W ;bitpos:[31:24] ;default: 8'd2 ; */
/*description: sample cycles*/
#define SYSCON_SARADC_SAMPLE_CYCLE  0x000000FF
#define SYSCON_SARADC_SAMPLE_CYCLE_M  ((SYSCON_SARADC_SAMPLE_CYCLE_V)<<(SYSCON_SARADC_SAMPLE_CYCLE_S))
#define SYSCON_SARADC_SAMPLE_CYCLE_V  0xFF
#define SYSCON_SARADC_SAMPLE_CYCLE_S  24
/* SYSCON_SARADC_SAMPLE_NUM : R/W ;bitpos:[23:16] ;default: 8'd0 ; */
/*description: sample number*/
#define SYSCON_SARADC_SAMPLE_NUM  0x000000FF
#define SYSCON_SARADC_SAMPLE_NUM_M  ((SYSCON_SARADC_SAMPLE_NUM_V)<<(SYSCON_SARADC_SAMPLE_NUM_S))
#define SYSCON_SARADC_SAMPLE_NUM_V  0xFF
#define SYSCON_SARADC_SAMPLE_NUM_S  16

#define SYSCON_SARADC_FSM_WAIT_REG          (DR_REG_SYSCON_BASE + 0x014)
/* SYSCON_SARADC_STANDBY_WAIT : R/W ;bitpos:[23:16] ;default: 8'd255 ; */
/*description: */
#define SYSCON_SARADC_STANDBY_WAIT  0x000000FF
#define SYSCON_SARADC_STANDBY_WAIT_M  ((SYSCON_SARADC_STANDBY_WAIT_V)<<(SYSCON_SARADC_STANDBY_WAIT_S))
#define SYSCON_SARADC_STANDBY_WAIT_V  0xFF
#define SYSCON_SARADC_STANDBY_WAIT_S  16
/* SYSCON_SARADC_RSTB_WAIT : R/W ;bitpos:[15:8] ;default: 8'd8 ; */
/*description: */
#define SYSCON_SARADC_RSTB_WAIT  0x000000FF
#define SYSCON_SARADC_RSTB_WAIT_M  ((SYSCON_SARADC_RSTB_WAIT_V)<<(SYSCON_SARADC_RSTB_WAIT_S))
#define SYSCON_SARADC_RSTB_WAIT_V  0xFF
#define SYSCON_SARADC_RSTB_WAIT_S  8
/* SYSCON_SARADC_XPD_WAIT : R/W ;bitpos:[7:0] ;default: 8'd8 ; */
/*description: */
#define SYSCON_SARADC_XPD_WAIT  0x000000FF
#define SYSCON_SARADC_XPD_WAIT_M  ((SYSCON_SARADC_XPD_WAIT_V)<<(SYSCON_SARADC_XPD_WAIT_S))
#define SYSCON_SARADC_XPD_WAIT_V  0xFF
#define SYSCON_SARADC_XPD_WAIT_S  0

#define SYSCON_SARADC_SAR1_STATUS_REG          (DR_REG_SYSCON_BASE + 0x018)
/* SYSCON_SARADC_SAR1_STATUS : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define SYSCON_SARADC_SAR1_STATUS  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_STATUS_M  ((SYSCON_SARADC_SAR1_STATUS_V)<<(SYSCON_SARADC_SAR1_STATUS_S))
#define SYSCON_SARADC_SAR1_STATUS_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_STATUS_S  0

#define SYSCON_SARADC_SAR2_STATUS_REG          (DR_REG_SYSCON_BASE + 0x01C)
/* SYSCON_SARADC_SAR2_STATUS : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define SYSCON_SARADC_SAR2_STATUS  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_STATUS_M  ((SYSCON_SARADC_SAR2_STATUS_V)<<(SYSCON_SARADC_SAR2_STATUS_S))
#define SYSCON_SARADC_SAR2_STATUS_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_STATUS_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB1_REG          (DR_REG_SYSCON_BASE + 0x020)
/* SYSCON_SARADC_SAR1_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB1  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB1_M  ((SYSCON_SARADC_SAR1_PATT_TAB1_V)<<(SYSCON_SARADC_SAR1_PATT_TAB1_S))
#define SYSCON_SARADC_SAR1_PATT_TAB1_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB1_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB2_REG          (DR_REG_SYSCON_BASE + 0x024)
/* SYSCON_SARADC_SAR1_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB2  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB2_M  ((SYSCON_SARADC_SAR1_PATT_TAB2_V)<<(SYSCON_SARADC_SAR1_PATT_TAB2_S))
#define SYSCON_SARADC_SAR1_PATT_TAB2_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB2_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB3_REG          (DR_REG_SYSCON_BASE + 0x028)
/* SYSCON_SARADC_SAR1_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB3  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB3_M  ((SYSCON_SARADC_SAR1_PATT_TAB3_V)<<(SYSCON_SARADC_SAR1_PATT_TAB3_S))
#define SYSCON_SARADC_SAR1_PATT_TAB3_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB3_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB4_REG          (DR_REG_SYSCON_BASE + 0x02C)
/* SYSCON_SARADC_SAR1_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB4  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB4_M  ((SYSCON_SARADC_SAR1_PATT_TAB4_V)<<(SYSCON_SARADC_SAR1_PATT_TAB4_S))
#define SYSCON_SARADC_SAR1_PATT_TAB4_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB4_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB1_REG          (DR_REG_SYSCON_BASE + 0x030)
/* SYSCON_SARADC_SAR2_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB1  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB1_M  ((SYSCON_SARADC_SAR2_PATT_TAB1_V)<<(SYSCON_SARADC_SAR2_PATT_TAB1_S))
#define SYSCON_SARADC_SAR2_PATT_TAB1_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB1_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB2_REG          (DR_REG_SYSCON_BASE + 0x034)
/* SYSCON_SARADC_SAR2_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB2  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB2_M  ((SYSCON_SARADC_SAR2_PATT_TAB2_V)<<(SYSCON_SARADC_SAR2_PATT_TAB2_S))
#define SYSCON_SARADC_SAR2_PATT_TAB2_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB2_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB3_REG          (DR_REG_SYSCON_BASE + 0x038)
/* SYSCON_SARADC_SAR2_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB3  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB3_M  ((SYSCON_SARADC_SAR2_PATT_TAB3_V)<<(SYSCON_SARADC_SAR2_PATT_TAB3_S))
#define SYSCON_SARADC_SAR2_PATT_TAB3_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB3_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB4_REG          (DR_REG_SYSCON_BASE + 0x03C)
/* SYSCON_SARADC_SAR2_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB4  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB4_M  ((SYSCON_SARADC_SAR2_PATT_TAB4_V)<<(SYSCON_SARADC_SAR2_PATT_TAB4_S))
#define SYSCON_SARADC_SAR2_PATT_TAB4_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB4_S  0

#define SYSCON_ADC_ARB_CTRL_REG          (DR_REG_SYSCON_BASE + 0x040)
/* SYSCON_ADC_ARB_FIX_PRIORITY : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: adc2 arbiter uses fixed priority*/
#define SYSCON_ADC_ARB_FIX_PRIORITY  (BIT(12))
#define SYSCON_ADC_ARB_FIX_PRIORITY_M  (BIT(12))
#define SYSCON_ADC_ARB_FIX_PRIORITY_V  0x1
#define SYSCON_ADC_ARB_FIX_PRIORITY_S  12
/* SYSCON_ADC_ARB_WIFI_PRIORITY : R/W ;bitpos:[11:10] ;default: 2'd2 ; */
/*description: Set adc2 arbiter wifi priority*/
#define SYSCON_ADC_ARB_WIFI_PRIORITY  0x00000003
#define SYSCON_ADC_ARB_WIFI_PRIORITY_M  ((SYSCON_ADC_ARB_WIFI_PRIORITY_V)<<(SYSCON_ADC_ARB_WIFI_PRIORITY_S))
#define SYSCON_ADC_ARB_WIFI_PRIORITY_V  0x3
#define SYSCON_ADC_ARB_WIFI_PRIORITY_S  10
/* SYSCON_ADC_ARB_RTC_PRIORITY : R/W ;bitpos:[9:8] ;default: 2'd1 ; */
/*description: Set adc2 arbiter rtc priority*/
#define SYSCON_ADC_ARB_RTC_PRIORITY  0x00000003
#define SYSCON_ADC_ARB_RTC_PRIORITY_M  ((SYSCON_ADC_ARB_RTC_PRIORITY_V)<<(SYSCON_ADC_ARB_RTC_PRIORITY_S))
#define SYSCON_ADC_ARB_RTC_PRIORITY_V  0x3
#define SYSCON_ADC_ARB_RTC_PRIORITY_S  8
/* SYSCON_ADC_ARB_APB_PRIORITY : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Set adc2 arbiter apb priority*/
#define SYSCON_ADC_ARB_APB_PRIORITY  0x00000003
#define SYSCON_ADC_ARB_APB_PRIORITY_M  ((SYSCON_ADC_ARB_APB_PRIORITY_V)<<(SYSCON_ADC_ARB_APB_PRIORITY_S))
#define SYSCON_ADC_ARB_APB_PRIORITY_V  0x3
#define SYSCON_ADC_ARB_APB_PRIORITY_S  6
/* SYSCON_ADC_ARB_GRANT_FORCE : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: adc2 arbiter force grant*/
#define SYSCON_ADC_ARB_GRANT_FORCE  (BIT(5))
#define SYSCON_ADC_ARB_GRANT_FORCE_M  (BIT(5))
#define SYSCON_ADC_ARB_GRANT_FORCE_V  0x1
#define SYSCON_ADC_ARB_GRANT_FORCE_S  5
/* SYSCON_ADC_ARB_WIFI_FORCE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enable wifi controller*/
#define SYSCON_ADC_ARB_WIFI_FORCE  (BIT(4))
#define SYSCON_ADC_ARB_WIFI_FORCE_M  (BIT(4))
#define SYSCON_ADC_ARB_WIFI_FORCE_V  0x1
#define SYSCON_ADC_ARB_WIFI_FORCE_S  4
/* SYSCON_ADC_ARB_RTC_FORCE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enable rtc controller*/
#define SYSCON_ADC_ARB_RTC_FORCE  (BIT(3))
#define SYSCON_ADC_ARB_RTC_FORCE_M  (BIT(3))
#define SYSCON_ADC_ARB_RTC_FORCE_V  0x1
#define SYSCON_ADC_ARB_RTC_FORCE_S  3
/* SYSCON_ADC_ARB_APB_FORCE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enableapb controller*/
#define SYSCON_ADC_ARB_APB_FORCE  (BIT(2))
#define SYSCON_ADC_ARB_APB_FORCE_M  (BIT(2))
#define SYSCON_ADC_ARB_APB_FORCE_V  0x1
#define SYSCON_ADC_ARB_APB_FORCE_S  2

#define SYSCON_CLK_OUT_EN_REG          (DR_REG_SYSCON_BASE + 0x044)
/* SYSCON_CLK_XTAL_OEN : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_XTAL_OEN  (BIT(10))
#define SYSCON_CLK_XTAL_OEN_M  (BIT(10))
#define SYSCON_CLK_XTAL_OEN_V  0x1
#define SYSCON_CLK_XTAL_OEN_S  10
/* SYSCON_CLK40X_BB_OEN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK40X_BB_OEN  (BIT(9))
#define SYSCON_CLK40X_BB_OEN_M  (BIT(9))
#define SYSCON_CLK40X_BB_OEN_V  0x1
#define SYSCON_CLK40X_BB_OEN_S  9
/* SYSCON_CLK_DAC_CPU_OEN : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_DAC_CPU_OEN  (BIT(8))
#define SYSCON_CLK_DAC_CPU_OEN_M  (BIT(8))
#define SYSCON_CLK_DAC_CPU_OEN_V  0x1
#define SYSCON_CLK_DAC_CPU_OEN_S  8
/* SYSCON_CLK_ADC_INF_OEN : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_ADC_INF_OEN  (BIT(7))
#define SYSCON_CLK_ADC_INF_OEN_M  (BIT(7))
#define SYSCON_CLK_ADC_INF_OEN_V  0x1
#define SYSCON_CLK_ADC_INF_OEN_S  7
/* SYSCON_CLK_320M_OEN : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_320M_OEN  (BIT(6))
#define SYSCON_CLK_320M_OEN_M  (BIT(6))
#define SYSCON_CLK_320M_OEN_V  0x1
#define SYSCON_CLK_320M_OEN_S  6
/* SYSCON_CLK160_OEN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK160_OEN  (BIT(5))
#define SYSCON_CLK160_OEN_M  (BIT(5))
#define SYSCON_CLK160_OEN_V  0x1
#define SYSCON_CLK160_OEN_S  5
/* SYSCON_CLK80_OEN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK80_OEN  (BIT(4))
#define SYSCON_CLK80_OEN_M  (BIT(4))
#define SYSCON_CLK80_OEN_V  0x1
#define SYSCON_CLK80_OEN_S  4
/* SYSCON_CLK_BB_OEN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_BB_OEN  (BIT(3))
#define SYSCON_CLK_BB_OEN_M  (BIT(3))
#define SYSCON_CLK_BB_OEN_V  0x1
#define SYSCON_CLK_BB_OEN_S  3
/* SYSCON_CLK44_OEN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK44_OEN  (BIT(2))
#define SYSCON_CLK44_OEN_M  (BIT(2))
#define SYSCON_CLK44_OEN_V  0x1
#define SYSCON_CLK44_OEN_S  2
/* SYSCON_CLK22_OEN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK22_OEN  (BIT(1))
#define SYSCON_CLK22_OEN_M  (BIT(1))
#define SYSCON_CLK22_OEN_V  0x1
#define SYSCON_CLK22_OEN_S  1
/* SYSCON_CLK20_OEN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK20_OEN  (BIT(0))
#define SYSCON_CLK20_OEN_M  (BIT(0))
#define SYSCON_CLK20_OEN_V  0x1
#define SYSCON_CLK20_OEN_S  0

#define SYSCON_HOST_INF_SEL_REG          (DR_REG_SYSCON_BASE + 0x048)
/* SYSCON_SPI_PRIOR : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI_PRIOR  (BIT(13))
#define SYSCON_SPI_PRIOR_M  (BIT(13))
#define SYSCON_SPI_PRIOR_V  0x1
#define SYSCON_SPI_PRIOR_S  13
/* SYSCON_SPI1_HOLD : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI1_HOLD  (BIT(9))
#define SYSCON_SPI1_HOLD_M  (BIT(9))
#define SYSCON_SPI1_HOLD_V  0x1
#define SYSCON_SPI1_HOLD_S  9
/* SYSCON_SPI0_HOLD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI0_HOLD  (BIT(8))
#define SYSCON_SPI0_HOLD_M  (BIT(8))
#define SYSCON_SPI0_HOLD_V  0x1
#define SYSCON_SPI0_HOLD_S  8
/* SYSCON_PERI_IO_SWAP : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define SYSCON_PERI_IO_SWAP  0x000000FF
#define SYSCON_PERI_IO_SWAP_M  ((SYSCON_PERI_IO_SWAP_V)<<(SYSCON_PERI_IO_SWAP_S))
#define SYSCON_PERI_IO_SWAP_V  0xFF
#define SYSCON_PERI_IO_SWAP_S  0

#define SYSCON_EXT_MEM_PMS_LOCK_REG          (DR_REG_SYSCON_BASE + 0x04C)
/* SYSCON_EXT_MEM_PMS_LOCK : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSCON_EXT_MEM_PMS_LOCK  (BIT(0))
#define SYSCON_EXT_MEM_PMS_LOCK_M  (BIT(0))
#define SYSCON_EXT_MEM_PMS_LOCK_V  0x1
#define SYSCON_EXT_MEM_PMS_LOCK_S  0

#define SYSCON_FLASH_ACE0_ATTR_REG          (DR_REG_SYSCON_BASE + 0x050)
/* SYSCON_FLASH_ACE0_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE0_ATTR  0x00000007
#define SYSCON_FLASH_ACE0_ATTR_M  ((SYSCON_FLASH_ACE0_ATTR_V)<<(SYSCON_FLASH_ACE0_ATTR_S))
#define SYSCON_FLASH_ACE0_ATTR_V  0x7
#define SYSCON_FLASH_ACE0_ATTR_S  0

#define SYSCON_FLASH_ACE1_ATTR_REG          (DR_REG_SYSCON_BASE + 0x054)
/* SYSCON_FLASH_ACE1_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE1_ATTR  0x00000007
#define SYSCON_FLASH_ACE1_ATTR_M  ((SYSCON_FLASH_ACE1_ATTR_V)<<(SYSCON_FLASH_ACE1_ATTR_S))
#define SYSCON_FLASH_ACE1_ATTR_V  0x7
#define SYSCON_FLASH_ACE1_ATTR_S  0

#define SYSCON_FLASH_ACE2_ATTR_REG          (DR_REG_SYSCON_BASE + 0x058)
/* SYSCON_FLASH_ACE2_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE2_ATTR  0x00000007
#define SYSCON_FLASH_ACE2_ATTR_M  ((SYSCON_FLASH_ACE2_ATTR_V)<<(SYSCON_FLASH_ACE2_ATTR_S))
#define SYSCON_FLASH_ACE2_ATTR_V  0x7
#define SYSCON_FLASH_ACE2_ATTR_S  0

#define SYSCON_FLASH_ACE3_ATTR_REG          (DR_REG_SYSCON_BASE + 0x05C)
/* SYSCON_FLASH_ACE3_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE3_ATTR  0x00000007
#define SYSCON_FLASH_ACE3_ATTR_M  ((SYSCON_FLASH_ACE3_ATTR_V)<<(SYSCON_FLASH_ACE3_ATTR_S))
#define SYSCON_FLASH_ACE3_ATTR_V  0x7
#define SYSCON_FLASH_ACE3_ATTR_S  0

#define SYSCON_FLASH_ACE0_ADDR_REG          (DR_REG_SYSCON_BASE + 0x060)
/* SYSCON_FLASH_ACE0_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_FLASH_ACE0_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE0_ADDR_S_M  ((SYSCON_FLASH_ACE0_ADDR_S_V)<<(SYSCON_FLASH_ACE0_ADDR_S_S))
#define SYSCON_FLASH_ACE0_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE0_ADDR_S_S  0

#define SYSCON_FLASH_ACE1_ADDR_REG          (DR_REG_SYSCON_BASE + 0x064)
/* SYSCON_FLASH_ACE1_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h10000000 ; */
/*description: */
#define SYSCON_FLASH_ACE1_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE1_ADDR_S_M  ((SYSCON_FLASH_ACE1_ADDR_S_V)<<(SYSCON_FLASH_ACE1_ADDR_S_S))
#define SYSCON_FLASH_ACE1_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE1_ADDR_S_S  0

#define SYSCON_FLASH_ACE2_ADDR_REG          (DR_REG_SYSCON_BASE + 0x068)
/* SYSCON_FLASH_ACE2_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h20000000 ; */
/*description: */
#define SYSCON_FLASH_ACE2_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE2_ADDR_S_M  ((SYSCON_FLASH_ACE2_ADDR_S_V)<<(SYSCON_FLASH_ACE2_ADDR_S_S))
#define SYSCON_FLASH_ACE2_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE2_ADDR_S_S  0

#define SYSCON_FLASH_ACE3_ADDR_REG          (DR_REG_SYSCON_BASE + 0x06C)
/* SYSCON_FLASH_ACE3_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h30000000 ; */
/*description: */
#define SYSCON_FLASH_ACE3_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE3_ADDR_S_M  ((SYSCON_FLASH_ACE3_ADDR_S_V)<<(SYSCON_FLASH_ACE3_ADDR_S_S))
#define SYSCON_FLASH_ACE3_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE3_ADDR_S_S  0

#define SYSCON_FLASH_ACE0_SIZE_REG          (DR_REG_SYSCON_BASE + 0x070)
/* SYSCON_FLASH_ACE0_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE0_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE0_SIZE_M  ((SYSCON_FLASH_ACE0_SIZE_V)<<(SYSCON_FLASH_ACE0_SIZE_S))
#define SYSCON_FLASH_ACE0_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE0_SIZE_S  0

#define SYSCON_FLASH_ACE1_SIZE_REG          (DR_REG_SYSCON_BASE + 0x074)
/* SYSCON_FLASH_ACE1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE1_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE1_SIZE_M  ((SYSCON_FLASH_ACE1_SIZE_V)<<(SYSCON_FLASH_ACE1_SIZE_S))
#define SYSCON_FLASH_ACE1_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE1_SIZE_S  0

#define SYSCON_FLASH_ACE2_SIZE_REG          (DR_REG_SYSCON_BASE + 0x078)
/* SYSCON_FLASH_ACE2_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE2_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE2_SIZE_M  ((SYSCON_FLASH_ACE2_SIZE_V)<<(SYSCON_FLASH_ACE2_SIZE_S))
#define SYSCON_FLASH_ACE2_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE2_SIZE_S  0

#define SYSCON_FLASH_ACE3_SIZE_REG          (DR_REG_SYSCON_BASE + 0x07C)
/* SYSCON_FLASH_ACE3_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE3_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE3_SIZE_M  ((SYSCON_FLASH_ACE3_SIZE_V)<<(SYSCON_FLASH_ACE3_SIZE_S))
#define SYSCON_FLASH_ACE3_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE3_SIZE_S  0

#define SYSCON_SRAM_ACE0_ATTR_REG          (DR_REG_SYSCON_BASE + 0x080)
/* SYSCON_SRAM_ACE0_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE0_ATTR  0x00000007
#define SYSCON_SRAM_ACE0_ATTR_M  ((SYSCON_SRAM_ACE0_ATTR_V)<<(SYSCON_SRAM_ACE0_ATTR_S))
#define SYSCON_SRAM_ACE0_ATTR_V  0x7
#define SYSCON_SRAM_ACE0_ATTR_S  0

#define SYSCON_SRAM_ACE1_ATTR_REG          (DR_REG_SYSCON_BASE + 0x084)
/* SYSCON_SRAM_ACE1_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE1_ATTR  0x00000007
#define SYSCON_SRAM_ACE1_ATTR_M  ((SYSCON_SRAM_ACE1_ATTR_V)<<(SYSCON_SRAM_ACE1_ATTR_S))
#define SYSCON_SRAM_ACE1_ATTR_V  0x7
#define SYSCON_SRAM_ACE1_ATTR_S  0

#define SYSCON_SRAM_ACE2_ATTR_REG          (DR_REG_SYSCON_BASE + 0x088)
/* SYSCON_SRAM_ACE2_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE2_ATTR  0x00000007
#define SYSCON_SRAM_ACE2_ATTR_M  ((SYSCON_SRAM_ACE2_ATTR_V)<<(SYSCON_SRAM_ACE2_ATTR_S))
#define SYSCON_SRAM_ACE2_ATTR_V  0x7
#define SYSCON_SRAM_ACE2_ATTR_S  0

#define SYSCON_SRAM_ACE3_ATTR_REG          (DR_REG_SYSCON_BASE + 0x08C)
/* SYSCON_SRAM_ACE3_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE3_ATTR  0x00000007
#define SYSCON_SRAM_ACE3_ATTR_M  ((SYSCON_SRAM_ACE3_ATTR_V)<<(SYSCON_SRAM_ACE3_ATTR_S))
#define SYSCON_SRAM_ACE3_ATTR_V  0x7
#define SYSCON_SRAM_ACE3_ATTR_S  0

#define SYSCON_SRAM_ACE0_ADDR_REG          (DR_REG_SYSCON_BASE + 0x090)
/* SYSCON_SRAM_ACE0_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_SRAM_ACE0_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE0_ADDR_S_M  ((SYSCON_SRAM_ACE0_ADDR_S_V)<<(SYSCON_SRAM_ACE0_ADDR_S_S))
#define SYSCON_SRAM_ACE0_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE0_ADDR_S_S  0

#define SYSCON_SRAM_ACE1_ADDR_REG          (DR_REG_SYSCON_BASE + 0x094)
/* SYSCON_SRAM_ACE1_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h10000000 ; */
/*description: */
#define SYSCON_SRAM_ACE1_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE1_ADDR_S_M  ((SYSCON_SRAM_ACE1_ADDR_S_V)<<(SYSCON_SRAM_ACE1_ADDR_S_S))
#define SYSCON_SRAM_ACE1_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE1_ADDR_S_S  0

#define SYSCON_SRAM_ACE2_ADDR_REG          (DR_REG_SYSCON_BASE + 0x098)
/* SYSCON_SRAM_ACE2_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h20000000 ; */
/*description: */
#define SYSCON_SRAM_ACE2_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE2_ADDR_S_M  ((SYSCON_SRAM_ACE2_ADDR_S_V)<<(SYSCON_SRAM_ACE2_ADDR_S_S))
#define SYSCON_SRAM_ACE2_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE2_ADDR_S_S  0

#define SYSCON_SRAM_ACE3_ADDR_REG          (DR_REG_SYSCON_BASE + 0x09C)
/* SYSCON_SRAM_ACE3_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h30000000 ; */
/*description: */
#define SYSCON_SRAM_ACE3_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE3_ADDR_S_M  ((SYSCON_SRAM_ACE3_ADDR_S_V)<<(SYSCON_SRAM_ACE3_ADDR_S_S))
#define SYSCON_SRAM_ACE3_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE3_ADDR_S_S  0

#define SYSCON_SRAM_ACE0_SIZE_REG          (DR_REG_SYSCON_BASE + 0x0A0)
/* SYSCON_SRAM_ACE0_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE0_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE0_SIZE_M  ((SYSCON_SRAM_ACE0_SIZE_V)<<(SYSCON_SRAM_ACE0_SIZE_S))
#define SYSCON_SRAM_ACE0_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE0_SIZE_S  0

#define SYSCON_SRAM_ACE1_SIZE_REG          (DR_REG_SYSCON_BASE + 0x0A4)
/* SYSCON_SRAM_ACE1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE1_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE1_SIZE_M  ((SYSCON_SRAM_ACE1_SIZE_V)<<(SYSCON_SRAM_ACE1_SIZE_S))
#define SYSCON_SRAM_ACE1_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE1_SIZE_S  0

#define SYSCON_SRAM_ACE2_SIZE_REG          (DR_REG_SYSCON_BASE + 0x0A8)
/* SYSCON_SRAM_ACE2_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE2_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE2_SIZE_M  ((SYSCON_SRAM_ACE2_SIZE_V)<<(SYSCON_SRAM_ACE2_SIZE_S))
#define SYSCON_SRAM_ACE2_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE2_SIZE_S  0

#define SYSCON_SRAM_ACE3_SIZE_REG          (DR_REG_SYSCON_BASE + 0x0AC)
/* SYSCON_SRAM_ACE3_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE3_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE3_SIZE_M  ((SYSCON_SRAM_ACE3_SIZE_V)<<(SYSCON_SRAM_ACE3_SIZE_S))
#define SYSCON_SRAM_ACE3_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE3_SIZE_S  0

#define SYSCON_SPI0_PMS_CTRL_REG          (DR_REG_SYSCON_BASE + 0x0B0)
/* SYSCON_SPI0_REJECT_CDE : RO ;bitpos:[6:2] ;default: 5'h0 ; */
/*description: */
#define SYSCON_SPI0_REJECT_CDE  0x0000001F
#define SYSCON_SPI0_REJECT_CDE_M  ((SYSCON_SPI0_REJECT_CDE_V)<<(SYSCON_SPI0_REJECT_CDE_S))
#define SYSCON_SPI0_REJECT_CDE_V  0x1F
#define SYSCON_SPI0_REJECT_CDE_S  2
/* SYSCON_SPI0_REJECT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI0_REJECT_CLR  (BIT(1))
#define SYSCON_SPI0_REJECT_CLR_M  (BIT(1))
#define SYSCON_SPI0_REJECT_CLR_V  0x1
#define SYSCON_SPI0_REJECT_CLR_S  1
/* SYSCON_SPI0_REJECT_INT : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI0_REJECT_INT  (BIT(0))
#define SYSCON_SPI0_REJECT_INT_M  (BIT(0))
#define SYSCON_SPI0_REJECT_INT_V  0x1
#define SYSCON_SPI0_REJECT_INT_S  0

#define SYSCON_SPI0_REJECT_ADDR_REG          (DR_REG_SYSCON_BASE + 0x0B4)
/* SYSCON_SPI0_REJECT_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_SPI0_REJECT_ADDR  0xFFFFFFFF
#define SYSCON_SPI0_REJECT_ADDR_M  ((SYSCON_SPI0_REJECT_ADDR_V)<<(SYSCON_SPI0_REJECT_ADDR_S))
#define SYSCON_SPI0_REJECT_ADDR_V  0xFFFFFFFF
#define SYSCON_SPI0_REJECT_ADDR_S  0

#define SYSCON_SPI1_PMS_CTRL_REG          (DR_REG_SYSCON_BASE + 0x0B8)
/* SYSCON_SPI1_REJECT_CDE : RO ;bitpos:[6:2] ;default: 5'h0 ; */
/*description: */
#define SYSCON_SPI1_REJECT_CDE  0x0000001F
#define SYSCON_SPI1_REJECT_CDE_M  ((SYSCON_SPI1_REJECT_CDE_V)<<(SYSCON_SPI1_REJECT_CDE_S))
#define SYSCON_SPI1_REJECT_CDE_V  0x1F
#define SYSCON_SPI1_REJECT_CDE_S  2
/* SYSCON_SPI1_REJECT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI1_REJECT_CLR  (BIT(1))
#define SYSCON_SPI1_REJECT_CLR_M  (BIT(1))
#define SYSCON_SPI1_REJECT_CLR_V  0x1
#define SYSCON_SPI1_REJECT_CLR_S  1
/* SYSCON_SPI1_REJECT_INT : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI1_REJECT_INT  (BIT(0))
#define SYSCON_SPI1_REJECT_INT_M  (BIT(0))
#define SYSCON_SPI1_REJECT_INT_V  0x1
#define SYSCON_SPI1_REJECT_INT_S  0

#define SYSCON_SPI1_REJECT_ADDR_REG          (DR_REG_SYSCON_BASE + 0x0BC)
/* SYSCON_SPI1_REJECT_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_SPI1_REJECT_ADDR  0xFFFFFFFF
#define SYSCON_SPI1_REJECT_ADDR_M  ((SYSCON_SPI1_REJECT_ADDR_V)<<(SYSCON_SPI1_REJECT_ADDR_S))
#define SYSCON_SPI1_REJECT_ADDR_V  0xFFFFFFFF
#define SYSCON_SPI1_REJECT_ADDR_S  0

#define SYSCON_SDIO_CTRL_REG          (DR_REG_SYSCON_BASE + 0x0C0)
/* SYSCON_SDIO_WIN_ACCESS_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define SYSCON_SDIO_WIN_ACCESS_EN  (BIT(0))
#define SYSCON_SDIO_WIN_ACCESS_EN_M  (BIT(0))
#define SYSCON_SDIO_WIN_ACCESS_EN_V  0x1
#define SYSCON_SDIO_WIN_ACCESS_EN_S  0

#define SYSCON_REDCY_SIG0_REG          (DR_REG_SYSCON_BASE + 0x0C4)
/* SYSCON_REDCY_ANDOR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SYSCON_REDCY_ANDOR  (BIT(31))
#define SYSCON_REDCY_ANDOR_M  (BIT(31))
#define SYSCON_REDCY_ANDOR_V  0x1
#define SYSCON_REDCY_ANDOR_S  31
/* SYSCON_REDCY_SIG0 : R/W ;bitpos:[30:0] ;default: 31'h0 ; */
/*description: */
#define SYSCON_REDCY_SIG0  0x7FFFFFFF
#define SYSCON_REDCY_SIG0_M  ((SYSCON_REDCY_SIG0_V)<<(SYSCON_REDCY_SIG0_S))
#define SYSCON_REDCY_SIG0_V  0x7FFFFFFF
#define SYSCON_REDCY_SIG0_S  0

#define SYSCON_REDCY_SIG1_REG          (DR_REG_SYSCON_BASE + 0x0C8)
/* SYSCON_REDCY_NANDOR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SYSCON_REDCY_NANDOR  (BIT(31))
#define SYSCON_REDCY_NANDOR_M  (BIT(31))
#define SYSCON_REDCY_NANDOR_V  0x1
#define SYSCON_REDCY_NANDOR_S  31
/* SYSCON_REDCY_SIG1 : R/W ;bitpos:[30:0] ;default: 31'h0 ; */
/*description: */
#define SYSCON_REDCY_SIG1  0x7FFFFFFF
#define SYSCON_REDCY_SIG1_M  ((SYSCON_REDCY_SIG1_V)<<(SYSCON_REDCY_SIG1_S))
#define SYSCON_REDCY_SIG1_V  0x7FFFFFFF
#define SYSCON_REDCY_SIG1_S  0

#define SYSCON_WIFI_BB_CFG_REG          (DR_REG_SYSCON_BASE + 0x0CC)
/* SYSCON_WIFI_BB_CFG : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_WIFI_BB_CFG  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_M  ((SYSCON_WIFI_BB_CFG_V)<<(SYSCON_WIFI_BB_CFG_S))
#define SYSCON_WIFI_BB_CFG_V  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_S  0

#define SYSCON_WIFI_BB_CFG_2_REG          (DR_REG_SYSCON_BASE + 0x0D0)
/* SYSCON_WIFI_BB_CFG_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_WIFI_BB_CFG_2  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_2_M  ((SYSCON_WIFI_BB_CFG_2_V)<<(SYSCON_WIFI_BB_CFG_2_S))
#define SYSCON_WIFI_BB_CFG_2_V  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_2_S  0

#define SYSCON_WIFI_CLK_EN_REG          (DR_REG_SYSCON_BASE + 0x0D4)
/* SYSCON_WIFI_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'hfffce030 ; */
/*description: */
#define SYSCON_WIFI_CLK_EN  0xFFFFFFFF
#define SYSCON_WIFI_CLK_EN_M  ((SYSCON_WIFI_CLK_EN_V)<<(SYSCON_WIFI_CLK_EN_S))
#define SYSCON_WIFI_CLK_EN_V  0xFFFFFFFF
#define SYSCON_WIFI_CLK_EN_S  0

#define SYSCON_WIFI_RST_EN_REG          (DR_REG_SYSCON_BASE + 0x0D8)
/* SYSCON_WIFI_RST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_WIFI_RST  0xFFFFFFFF
#define SYSCON_WIFI_RST_M  ((SYSCON_WIFI_RST_V)<<(SYSCON_WIFI_RST_S))
#define SYSCON_WIFI_RST_V  0xFFFFFFFF
#define SYSCON_WIFI_RST_S  0

#define DPORT_WIFI_CLK_EN_REG          SYSCON_WIFI_CLK_EN_REG
/* DPORT_WIFI_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'hfffce030 ; */
/*description: */
#define DPORT_WIFI_CLK_EN  0xFFFFFFFF
#define DPORT_WIFI_CLK_EN_M  ((DPORT_WIFI_CLK_EN_V)<<(DPORT_WIFI_CLK_EN_S))
#define DPORT_WIFI_CLK_EN_V  0xFFFFFFFF
#define DPORT_WIFI_CLK_EN_S  0

/* Mask for all Wifi clock bits - 0, 1, 2, 3, 6, 7, 8, 9, 10, 15, 19, 20, 21
   Bit15 not included here because of the bit now can't be cleared */
#define DPORT_WIFI_CLK_WIFI_EN  0x00008040
#define DPORT_WIFI_CLK_WIFI_EN_M  ((DPORT_WIFI_CLK_WIFI_EN_V)<<(DPORT_WIFI_CLK_WIFI_EN_S))
#define DPORT_WIFI_CLK_WIFI_EN_V  0x0804
#define DPORT_WIFI_CLK_WIFI_EN_S  0
/* Mask for all Bluetooth clock bits - 11, 16, 17 */
#define DPORT_WIFI_CLK_BT_EN  0x61
#define DPORT_WIFI_CLK_BT_EN_M  ((DPORT_WIFI_CLK_BT_EN_V)<<(DPORT_WIFI_CLK_BT_EN_S))
#define DPORT_WIFI_CLK_BT_EN_V  0x61
#define DPORT_WIFI_CLK_BT_EN_S  11
/* Mask for clock bits used by both WIFI and Bluetooth, bit 0, 3, 6, 7, 8, 9 */
#define DPORT_WIFI_CLK_WIFI_BT_COMMON_M 0x0038078f

/* Digital team to check */
//bluetooth baseband bit11
#define DPORT_BT_BASEBAND_EN  BIT(11)
//bluetooth LC bit16 and bit17
#define DPORT_BT_LC_EN  (BIT(16)|BIT(17))

/* Remaining single bit clock masks */
#define DPORT_WIFI_CLK_SDIOSLAVE_EN  BIT(4)
#define DPORT_WIFI_CLK_UNUSED_BIT5  BIT(5)
#define DPORT_WIFI_CLK_UNUSED_BIT12  BIT(12)
#define DPORT_WIFI_CLK_SDIO_HOST_EN  BIT(13)
#define DPORT_WIFI_CLK_EMAC_EN  BIT(14)
#define DPORT_WIFI_CLK_RNG_EN  BIT(15)

#define DPORT_CORE_RST_EN_REG         DPORT_WIFI_RST_EN_REG
#define DPORT_WIFI_RST_EN_REG         SYSCON_WIFI_RST_EN_REG
/* DPORT_WIFI_RST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_WIFI_RST  0xFFFFFFFF
#define DPORT_WIFI_RST_M  ((DPORT_WIFI_RST_V)<<(DPORT_WIFI_RST_S))
#define DPORT_WIFI_RST_V  0xFFFFFFFF
#define DPORT_WIFI_RST_S  0
#define DPORT_RW_BTLP_RST (BIT(10))
#define DPORT_RW_BTMAC_RST (BIT(9))
#define DPORT_MACPWR_RST (BIT(8))
#define DPORT_EMAC_RST (BIT(7))
#define DPORT_SDIO_HOST_RST (BIT(6))
#define DPORT_SDIO_RST (BIT(5))
#define DPORT_BTMAC_RST (BIT(4))
#define DPORT_BT_RST (BIT(3))
#define DPORT_MAC_RST (BIT(2))
#define DPORT_FE_RST (BIT(1))
#define DPORT_BB_RST (BIT(0))  

#define SYSCON_FRONT_END_MEM_PD_REG          (DR_REG_SYSCON_BASE + 0x0DC)
/* SYSCON_DC_MEM_FORCE_PD : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SYSCON_DC_MEM_FORCE_PD  (BIT(5))
#define SYSCON_DC_MEM_FORCE_PD_M  (BIT(5))
#define SYSCON_DC_MEM_FORCE_PD_V  0x1
#define SYSCON_DC_MEM_FORCE_PD_S  5
/* SYSCON_DC_MEM_FORCE_PU : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define SYSCON_DC_MEM_FORCE_PU  (BIT(4))
#define SYSCON_DC_MEM_FORCE_PU_M  (BIT(4))
#define SYSCON_DC_MEM_FORCE_PU_V  0x1
#define SYSCON_DC_MEM_FORCE_PU_S  4
/* SYSCON_PBUS_MEM_FORCE_PD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SYSCON_PBUS_MEM_FORCE_PD  (BIT(3))
#define SYSCON_PBUS_MEM_FORCE_PD_M  (BIT(3))
#define SYSCON_PBUS_MEM_FORCE_PD_V  0x1
#define SYSCON_PBUS_MEM_FORCE_PD_S  3
/* SYSCON_PBUS_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define SYSCON_PBUS_MEM_FORCE_PU  (BIT(2))
#define SYSCON_PBUS_MEM_FORCE_PU_M  (BIT(2))
#define SYSCON_PBUS_MEM_FORCE_PU_V  0x1
#define SYSCON_PBUS_MEM_FORCE_PU_S  2
/* SYSCON_AGC_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSCON_AGC_MEM_FORCE_PD  (BIT(1))
#define SYSCON_AGC_MEM_FORCE_PD_M  (BIT(1))
#define SYSCON_AGC_MEM_FORCE_PD_V  0x1
#define SYSCON_AGC_MEM_FORCE_PD_S  1
/* SYSCON_AGC_MEM_FORCE_PU : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSCON_AGC_MEM_FORCE_PU  (BIT(0))
#define SYSCON_AGC_MEM_FORCE_PU_M  (BIT(0))
#define SYSCON_AGC_MEM_FORCE_PU_V  0x1
#define SYSCON_AGC_MEM_FORCE_PU_S  0

#define SYSCON_DATE_REG          (DR_REG_SYSCON_BASE + 0x3FC)
/* SYSCON_DATE : R/W ;bitpos:[31:0] ;default: 32'h18102500 ; */
/*description: */
#define SYSCON_DATE  0xFFFFFFFF
#define SYSCON_DATE_M  ((SYSCON_DATE_V)<<(SYSCON_DATE_S))
#define SYSCON_DATE_V  0xFFFFFFFF
#define SYSCON_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_SYSCON_REG_H_ */


