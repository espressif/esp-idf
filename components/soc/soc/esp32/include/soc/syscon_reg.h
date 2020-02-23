// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_SYSCON_REG_H_
#define _SOC_SYSCON_REG_H_

#include "soc.h"
#define SYSCON_SYSCLK_CONF_REG          (DR_REG_SYSCON_BASE + 0x0)
/* SYSCON_QUICK_CLK_CHNG : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define SYSCON_QUICK_CLK_CHNG  (BIT(13))
#define SYSCON_QUICK_CLK_CHNG_M  (BIT(13))
#define SYSCON_QUICK_CLK_CHNG_V  0x1
#define SYSCON_QUICK_CLK_CHNG_S  13
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

#define SYSCON_XTAL_TICK_CONF_REG          (DR_REG_SYSCON_BASE + 0x4)
/* SYSCON_XTAL_TICK_NUM : R/W ;bitpos:[7:0] ;default: 8'd39 ; */
/*description: */
#define SYSCON_XTAL_TICK_NUM  0x000000FF
#define SYSCON_XTAL_TICK_NUM_M  ((SYSCON_XTAL_TICK_NUM_V)<<(SYSCON_XTAL_TICK_NUM_S))
#define SYSCON_XTAL_TICK_NUM_V  0xFF
#define SYSCON_XTAL_TICK_NUM_S  0

#define SYSCON_PLL_TICK_CONF_REG          (DR_REG_SYSCON_BASE + 0x8)
/* SYSCON_PLL_TICK_NUM : R/W ;bitpos:[7:0] ;default: 8'd79 ; */
/*description: */
#define SYSCON_PLL_TICK_NUM  0x000000FF
#define SYSCON_PLL_TICK_NUM_M  ((SYSCON_PLL_TICK_NUM_V)<<(SYSCON_PLL_TICK_NUM_S))
#define SYSCON_PLL_TICK_NUM_V  0xFF
#define SYSCON_PLL_TICK_NUM_S  0

#define SYSCON_CK8M_TICK_CONF_REG          (DR_REG_SYSCON_BASE + 0xC)
/* SYSCON_CK8M_TICK_NUM : R/W ;bitpos:[7:0] ;default: 8'd11 ; */
/*description: */
#define SYSCON_CK8M_TICK_NUM  0x000000FF
#define SYSCON_CK8M_TICK_NUM_M  ((SYSCON_CK8M_TICK_NUM_V)<<(SYSCON_CK8M_TICK_NUM_S))
#define SYSCON_CK8M_TICK_NUM_V  0xFF
#define SYSCON_CK8M_TICK_NUM_S  0

#define SYSCON_SARADC_CTRL_REG          (DR_REG_SYSCON_BASE + 0x10)
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
/* SYSCON_SARADC_SAR2_MUX : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: 1: SAR ADC2 is controlled by DIG ADC2 CTRL  0: SAR ADC2 is controlled
 by PWDET CTRL*/
#define SYSCON_SARADC_SAR2_MUX  (BIT(2))
#define SYSCON_SARADC_SAR2_MUX_M  (BIT(2))
#define SYSCON_SARADC_SAR2_MUX_V  0x1
#define SYSCON_SARADC_SAR2_MUX_S  2
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

#define SYSCON_SARADC_CTRL2_REG          (DR_REG_SYSCON_BASE + 0x14)
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

#define SYSCON_SARADC_FSM_REG          (DR_REG_SYSCON_BASE + 0x18)
/* SYSCON_SARADC_SAMPLE_CYCLE : R/W ;bitpos:[31:24] ;default: 8'd2 ; */
/*description: sample cycles*/
#define SYSCON_SARADC_SAMPLE_CYCLE  0x000000FF
#define SYSCON_SARADC_SAMPLE_CYCLE_M  ((SYSCON_SARADC_SAMPLE_CYCLE_V)<<(SYSCON_SARADC_SAMPLE_CYCLE_S))
#define SYSCON_SARADC_SAMPLE_CYCLE_V  0xFF
#define SYSCON_SARADC_SAMPLE_CYCLE_S  24
/* SYSCON_SARADC_START_WAIT : R/W ;bitpos:[23:16] ;default: 8'd8 ; */
/*description: */
#define SYSCON_SARADC_START_WAIT  0x000000FF
#define SYSCON_SARADC_START_WAIT_M  ((SYSCON_SARADC_START_WAIT_V)<<(SYSCON_SARADC_START_WAIT_S))
#define SYSCON_SARADC_START_WAIT_V  0xFF
#define SYSCON_SARADC_START_WAIT_S  16
/* SYSCON_SARADC_STANDBY_WAIT : R/W ;bitpos:[15:8] ;default: 8'd255 ; */
/*description: */
#define SYSCON_SARADC_STANDBY_WAIT  0x000000FF
#define SYSCON_SARADC_STANDBY_WAIT_M  ((SYSCON_SARADC_STANDBY_WAIT_V)<<(SYSCON_SARADC_STANDBY_WAIT_S))
#define SYSCON_SARADC_STANDBY_WAIT_V  0xFF
#define SYSCON_SARADC_STANDBY_WAIT_S  8
/* SYSCON_SARADC_RSTB_WAIT : R/W ;bitpos:[7:0] ;default: 8'd8 ; */
/*description: */
#define SYSCON_SARADC_RSTB_WAIT  0x000000FF
#define SYSCON_SARADC_RSTB_WAIT_M  ((SYSCON_SARADC_RSTB_WAIT_V)<<(SYSCON_SARADC_RSTB_WAIT_S))
#define SYSCON_SARADC_RSTB_WAIT_V  0xFF
#define SYSCON_SARADC_RSTB_WAIT_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB1_REG          (DR_REG_SYSCON_BASE + 0x1C)
/* SYSCON_SARADC_SAR1_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB1  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB1_M  ((SYSCON_SARADC_SAR1_PATT_TAB1_V)<<(SYSCON_SARADC_SAR1_PATT_TAB1_S))
#define SYSCON_SARADC_SAR1_PATT_TAB1_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB1_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB2_REG          (DR_REG_SYSCON_BASE + 0x20)
/* SYSCON_SARADC_SAR1_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB2  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB2_M  ((SYSCON_SARADC_SAR1_PATT_TAB2_V)<<(SYSCON_SARADC_SAR1_PATT_TAB2_S))
#define SYSCON_SARADC_SAR1_PATT_TAB2_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB2_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB3_REG          (DR_REG_SYSCON_BASE + 0x24)
/* SYSCON_SARADC_SAR1_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB3  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB3_M  ((SYSCON_SARADC_SAR1_PATT_TAB3_V)<<(SYSCON_SARADC_SAR1_PATT_TAB3_S))
#define SYSCON_SARADC_SAR1_PATT_TAB3_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB3_S  0

#define SYSCON_SARADC_SAR1_PATT_TAB4_REG          (DR_REG_SYSCON_BASE + 0x28)
/* SYSCON_SARADC_SAR1_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 1 (each item one byte)*/
#define SYSCON_SARADC_SAR1_PATT_TAB4  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB4_M  ((SYSCON_SARADC_SAR1_PATT_TAB4_V)<<(SYSCON_SARADC_SAR1_PATT_TAB4_S))
#define SYSCON_SARADC_SAR1_PATT_TAB4_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR1_PATT_TAB4_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB1_REG          (DR_REG_SYSCON_BASE + 0x2C)
/* SYSCON_SARADC_SAR2_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB1  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB1_M  ((SYSCON_SARADC_SAR2_PATT_TAB1_V)<<(SYSCON_SARADC_SAR2_PATT_TAB1_S))
#define SYSCON_SARADC_SAR2_PATT_TAB1_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB1_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB2_REG          (DR_REG_SYSCON_BASE + 0x30)
/* SYSCON_SARADC_SAR2_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB2  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB2_M  ((SYSCON_SARADC_SAR2_PATT_TAB2_V)<<(SYSCON_SARADC_SAR2_PATT_TAB2_S))
#define SYSCON_SARADC_SAR2_PATT_TAB2_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB2_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB3_REG          (DR_REG_SYSCON_BASE + 0x34)
/* SYSCON_SARADC_SAR2_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB3  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB3_M  ((SYSCON_SARADC_SAR2_PATT_TAB3_V)<<(SYSCON_SARADC_SAR2_PATT_TAB3_S))
#define SYSCON_SARADC_SAR2_PATT_TAB3_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB3_S  0

#define SYSCON_SARADC_SAR2_PATT_TAB4_REG          (DR_REG_SYSCON_BASE + 0x38)
/* SYSCON_SARADC_SAR2_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 2 (each item one byte)*/
#define SYSCON_SARADC_SAR2_PATT_TAB4  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB4_M  ((SYSCON_SARADC_SAR2_PATT_TAB4_V)<<(SYSCON_SARADC_SAR2_PATT_TAB4_S))
#define SYSCON_SARADC_SAR2_PATT_TAB4_V  0xFFFFFFFF
#define SYSCON_SARADC_SAR2_PATT_TAB4_S  0

#define SYSCON_APLL_TICK_CONF_REG          (DR_REG_SYSCON_BASE + 0x3C)
/* SYSCON_APLL_TICK_NUM : R/W ;bitpos:[7:0] ;default: 8'd99 ; */
/*description: */
#define SYSCON_APLL_TICK_NUM  0x000000FF
#define SYSCON_APLL_TICK_NUM_M  ((SYSCON_APLL_TICK_NUM_V)<<(SYSCON_APLL_TICK_NUM_S))
#define SYSCON_APLL_TICK_NUM_V  0xFF
#define SYSCON_APLL_TICK_NUM_S  0

#define SYSCON_DATE_REG          (DR_REG_SYSCON_BASE + 0x7C)
/* SYSCON_DATE : R/W ;bitpos:[31:0] ;default: 32'h16042000 ; */
/*description: */
#define SYSCON_DATE  0xFFFFFFFF
#define SYSCON_DATE_M  ((SYSCON_DATE_V)<<(SYSCON_DATE_S))
#define SYSCON_DATE_V  0xFFFFFFFF
#define SYSCON_DATE_S  0




#endif /*_SOC_SYSCON_REG_H_ */


