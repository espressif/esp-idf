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
#ifndef _SOC_APB_CTRL_REG_H_
#define _SOC_APB_CTRL_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define APB_CTRL_SYSCLK_CONF_REG          (DR_REG_APB_CTRL_BASE + 0x000)
/* APB_CTRL_SOC_CLK_SEL : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define APB_CTRL_SOC_CLK_SEL  0x00000003
#define APB_CTRL_SOC_CLK_SEL_M  ((APB_CTRL_SOC_CLK_SEL_V)<<(APB_CTRL_SOC_CLK_SEL_S))
#define APB_CTRL_SOC_CLK_SEL_V  0x3
#define APB_CTRL_SOC_CLK_SEL_S  14
#define APB_CTRL_SOC_CLK_SEL_XTL    0
#define APB_CTRL_SOC_CLK_SEL_PLL    1
#define APB_CTRL_SOC_CLK_SEL_8M     2
#define APB_CTRL_SOC_CLK_SEL_APLL   3

/* APB_CTRL_RST_TICK_CNT : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_RST_TICK_CNT  (BIT(12))
#define APB_CTRL_RST_TICK_CNT_M  (BIT(12))
#define APB_CTRL_RST_TICK_CNT_V  0x1
#define APB_CTRL_RST_TICK_CNT_S  12
/* APB_CTRL_CLK_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_CLK_EN  (BIT(11))
#define APB_CTRL_CLK_EN_M  (BIT(11))
#define APB_CTRL_CLK_EN_V  0x1
#define APB_CTRL_CLK_EN_S  11
/* APB_CTRL_CLK_320M_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_CLK_320M_EN  (BIT(10))
#define APB_CTRL_CLK_320M_EN_M  (BIT(10))
#define APB_CTRL_CLK_320M_EN_V  0x1
#define APB_CTRL_CLK_320M_EN_S  10
/* APB_CTRL_PRE_DIV_CNT : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define APB_CTRL_PRE_DIV_CNT  0x000003FF
#define APB_CTRL_PRE_DIV_CNT_M  ((APB_CTRL_PRE_DIV_CNT_V)<<(APB_CTRL_PRE_DIV_CNT_S))
#define APB_CTRL_PRE_DIV_CNT_V  0x3FF
#define APB_CTRL_PRE_DIV_CNT_S  0

#define APB_CTRL_TICK_CONF_REG          (DR_REG_APB_CTRL_BASE + 0x004)
/* APB_CTRL_TICK_ENABLE : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: */
#define APB_CTRL_TICK_ENABLE  (BIT(16))
#define APB_CTRL_TICK_ENABLE_M  (BIT(16))
#define APB_CTRL_TICK_ENABLE_V  0x1
#define APB_CTRL_TICK_ENABLE_S  16
/* APB_CTRL_CK8M_TICK_NUM : R/W ;bitpos:[15:8] ;default: 8'd7 ; */
/*description: */
#define APB_CTRL_CK8M_TICK_NUM  0x000000FF
#define APB_CTRL_CK8M_TICK_NUM_M  ((APB_CTRL_CK8M_TICK_NUM_V)<<(APB_CTRL_CK8M_TICK_NUM_S))
#define APB_CTRL_CK8M_TICK_NUM_V  0xFF
#define APB_CTRL_CK8M_TICK_NUM_S  8
/* APB_CTRL_XTAL_TICK_NUM : R/W ;bitpos:[7:0] ;default: 8'd39 ; */
/*description: */
#define APB_CTRL_XTAL_TICK_NUM  0x000000FF
#define APB_CTRL_XTAL_TICK_NUM_M  ((APB_CTRL_XTAL_TICK_NUM_V)<<(APB_CTRL_XTAL_TICK_NUM_S))
#define APB_CTRL_XTAL_TICK_NUM_V  0xFF
#define APB_CTRL_XTAL_TICK_NUM_S  0

#define APB_CTRL_SARADC_CTRL_REG          (DR_REG_APB_CTRL_BASE + 0x008)
/* APB_CTRL_SARADC_XPD_SAR_FORCE : R/W ;bitpos:[28:27] ;default: 2'd0 ; */
/*description: force option to xpd sar blocks*/
#define APB_CTRL_SARADC_XPD_SAR_FORCE  0x00000003
#define APB_CTRL_SARADC_XPD_SAR_FORCE_M  ((APB_CTRL_SARADC_XPD_SAR_FORCE_V)<<(APB_CTRL_SARADC_XPD_SAR_FORCE_S))
#define APB_CTRL_SARADC_XPD_SAR_FORCE_V  0x3
#define APB_CTRL_SARADC_XPD_SAR_FORCE_S  27
/* APB_CTRL_SARADC_DATA_TO_I2S : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 1: I2S input data is from SAR ADC (for DMA)  0: I2S input data
 is from GPIO matrix*/
#define APB_CTRL_SARADC_DATA_TO_I2S  (BIT(26))
#define APB_CTRL_SARADC_DATA_TO_I2S_M  (BIT(26))
#define APB_CTRL_SARADC_DATA_TO_I2S_V  0x1
#define APB_CTRL_SARADC_DATA_TO_I2S_S  26
/* APB_CTRL_SARADC_DATA_SAR_SEL : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1: sar_sel will be coded by the MSB of the 16-bit output data
  in this case the resolution should not be larger than 11 bits.*/
#define APB_CTRL_SARADC_DATA_SAR_SEL  (BIT(25))
#define APB_CTRL_SARADC_DATA_SAR_SEL_M  (BIT(25))
#define APB_CTRL_SARADC_DATA_SAR_SEL_V  0x1
#define APB_CTRL_SARADC_DATA_SAR_SEL_S  25
/* APB_CTRL_SARADC_SAR2_PATT_P_CLEAR : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: clear the pointer of pattern table for DIG ADC2 CTRL*/
#define APB_CTRL_SARADC_SAR2_PATT_P_CLEAR  (BIT(24))
#define APB_CTRL_SARADC_SAR2_PATT_P_CLEAR_M  (BIT(24))
#define APB_CTRL_SARADC_SAR2_PATT_P_CLEAR_V  0x1
#define APB_CTRL_SARADC_SAR2_PATT_P_CLEAR_S  24
/* APB_CTRL_SARADC_SAR1_PATT_P_CLEAR : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: clear the pointer of pattern table for DIG ADC1 CTRL*/
#define APB_CTRL_SARADC_SAR1_PATT_P_CLEAR  (BIT(23))
#define APB_CTRL_SARADC_SAR1_PATT_P_CLEAR_M  (BIT(23))
#define APB_CTRL_SARADC_SAR1_PATT_P_CLEAR_V  0x1
#define APB_CTRL_SARADC_SAR1_PATT_P_CLEAR_S  23
/* APB_CTRL_SARADC_SAR2_PATT_LEN : R/W ;bitpos:[22:19] ;default: 4'd15 ; */
/*description: 0 ~ 15 means length 1 ~ 16*/
#define APB_CTRL_SARADC_SAR2_PATT_LEN  0x0000000F
#define APB_CTRL_SARADC_SAR2_PATT_LEN_M  ((APB_CTRL_SARADC_SAR2_PATT_LEN_V)<<(APB_CTRL_SARADC_SAR2_PATT_LEN_S))
#define APB_CTRL_SARADC_SAR2_PATT_LEN_V  0xF
#define APB_CTRL_SARADC_SAR2_PATT_LEN_S  19
/* APB_CTRL_SARADC_SAR1_PATT_LEN : R/W ;bitpos:[18:15] ;default: 4'd15 ; */
/*description: 0 ~ 15 means length 1 ~ 16*/
#define APB_CTRL_SARADC_SAR1_PATT_LEN  0x0000000F
#define APB_CTRL_SARADC_SAR1_PATT_LEN_M  ((APB_CTRL_SARADC_SAR1_PATT_LEN_V)<<(APB_CTRL_SARADC_SAR1_PATT_LEN_S))
#define APB_CTRL_SARADC_SAR1_PATT_LEN_V  0xF
#define APB_CTRL_SARADC_SAR1_PATT_LEN_S  15
/* APB_CTRL_SARADC_SAR_CLK_DIV : R/W ;bitpos:[14:7] ;default: 8'd4 ; */
/*description: SAR clock divider*/
#define APB_CTRL_SARADC_SAR_CLK_DIV  0x000000FF
#define APB_CTRL_SARADC_SAR_CLK_DIV_M  ((APB_CTRL_SARADC_SAR_CLK_DIV_V)<<(APB_CTRL_SARADC_SAR_CLK_DIV_S))
#define APB_CTRL_SARADC_SAR_CLK_DIV_V  0xFF
#define APB_CTRL_SARADC_SAR_CLK_DIV_S  7
/* APB_CTRL_SARADC_SAR_CLK_GATED : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_SARADC_SAR_CLK_GATED  (BIT(6))
#define APB_CTRL_SARADC_SAR_CLK_GATED_M  (BIT(6))
#define APB_CTRL_SARADC_SAR_CLK_GATED_V  0x1
#define APB_CTRL_SARADC_SAR_CLK_GATED_S  6
/* APB_CTRL_SARADC_SAR_SEL : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: 0: SAR1  1: SAR2  only work for single SAR mode*/
#define APB_CTRL_SARADC_SAR_SEL  (BIT(5))
#define APB_CTRL_SARADC_SAR_SEL_M  (BIT(5))
#define APB_CTRL_SARADC_SAR_SEL_V  0x1
#define APB_CTRL_SARADC_SAR_SEL_S  5
/* APB_CTRL_SARADC_WORK_MODE : R/W ;bitpos:[4:3] ;default: 2'd0 ; */
/*description: 0: single mode  1: double mode  2: alternate mode*/
#define APB_CTRL_SARADC_WORK_MODE  0x00000003
#define APB_CTRL_SARADC_WORK_MODE_M  ((APB_CTRL_SARADC_WORK_MODE_V)<<(APB_CTRL_SARADC_WORK_MODE_S))
#define APB_CTRL_SARADC_WORK_MODE_V  0x3
#define APB_CTRL_SARADC_WORK_MODE_S  3
/* APB_CTRL_SARADC_START : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define APB_CTRL_SARADC_START  (BIT(1))
#define APB_CTRL_SARADC_START_M  (BIT(1))
#define APB_CTRL_SARADC_START_V  0x1
#define APB_CTRL_SARADC_START_S  1
/* APB_CTRL_SARADC_START_FORCE : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define APB_CTRL_SARADC_START_FORCE  (BIT(0))
#define APB_CTRL_SARADC_START_FORCE_M  (BIT(0))
#define APB_CTRL_SARADC_START_FORCE_V  0x1
#define APB_CTRL_SARADC_START_FORCE_S  0

#define APB_CTRL_SARADC_CTRL2_REG          (DR_REG_APB_CTRL_BASE + 0x00C)
/* APB_CTRL_SARADC_TIMER_EN : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: to enable saradc timer trigger*/
#define APB_CTRL_SARADC_TIMER_EN  (BIT(20))
#define APB_CTRL_SARADC_TIMER_EN_M  (BIT(20))
#define APB_CTRL_SARADC_TIMER_EN_V  0x1
#define APB_CTRL_SARADC_TIMER_EN_S  20
/* APB_CTRL_SARADC_TIMER_TARGET : R/W ;bitpos:[19:12] ;default: 8'd10 ; */
/*description: to set saradc timer target*/
#define APB_CTRL_SARADC_TIMER_TARGET  0x000000FF
#define APB_CTRL_SARADC_TIMER_TARGET_M  ((APB_CTRL_SARADC_TIMER_TARGET_V)<<(APB_CTRL_SARADC_TIMER_TARGET_S))
#define APB_CTRL_SARADC_TIMER_TARGET_V  0xFF
#define APB_CTRL_SARADC_TIMER_TARGET_S  12
/* APB_CTRL_SARADC_TIMER_SEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: 1: select saradc timer 0: i2s_ws trigger*/
#define APB_CTRL_SARADC_TIMER_SEL  (BIT(11))
#define APB_CTRL_SARADC_TIMER_SEL_M  (BIT(11))
#define APB_CTRL_SARADC_TIMER_SEL_V  0x1
#define APB_CTRL_SARADC_TIMER_SEL_S  11
/* APB_CTRL_SARADC_SAR2_INV : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: 1: data to DIG ADC2 CTRL is inverted  otherwise not*/
#define APB_CTRL_SARADC_SAR2_INV  (BIT(10))
#define APB_CTRL_SARADC_SAR2_INV_M  (BIT(10))
#define APB_CTRL_SARADC_SAR2_INV_V  0x1
#define APB_CTRL_SARADC_SAR2_INV_S  10
/* APB_CTRL_SARADC_SAR1_INV : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: 1: data to DIG ADC1 CTRL is inverted  otherwise not*/
#define APB_CTRL_SARADC_SAR1_INV  (BIT(9))
#define APB_CTRL_SARADC_SAR1_INV_M  (BIT(9))
#define APB_CTRL_SARADC_SAR1_INV_V  0x1
#define APB_CTRL_SARADC_SAR1_INV_S  9
/* APB_CTRL_SARADC_MAX_MEAS_NUM : R/W ;bitpos:[8:1] ;default: 8'd255 ; */
/*description: max conversion number*/
#define APB_CTRL_SARADC_MAX_MEAS_NUM  0x000000FF
#define APB_CTRL_SARADC_MAX_MEAS_NUM_M  ((APB_CTRL_SARADC_MAX_MEAS_NUM_V)<<(APB_CTRL_SARADC_MAX_MEAS_NUM_S))
#define APB_CTRL_SARADC_MAX_MEAS_NUM_V  0xFF
#define APB_CTRL_SARADC_MAX_MEAS_NUM_S  1
/* APB_CTRL_SARADC_MEAS_NUM_LIMIT : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define APB_CTRL_SARADC_MEAS_NUM_LIMIT  (BIT(0))
#define APB_CTRL_SARADC_MEAS_NUM_LIMIT_M  (BIT(0))
#define APB_CTRL_SARADC_MEAS_NUM_LIMIT_V  0x1
#define APB_CTRL_SARADC_MEAS_NUM_LIMIT_S  0

#define APB_CTRL_SARADC_FSM_REG          (DR_REG_APB_CTRL_BASE + 0x010)
/* APB_CTRL_SARADC_SAMPLE_CYCLE : R/W ;bitpos:[31:24] ;default: 8'd2 ; */
/*description: sample cycles*/
#define APB_CTRL_SARADC_SAMPLE_CYCLE  0x000000FF
#define APB_CTRL_SARADC_SAMPLE_CYCLE_M  ((APB_CTRL_SARADC_SAMPLE_CYCLE_V)<<(APB_CTRL_SARADC_SAMPLE_CYCLE_S))
#define APB_CTRL_SARADC_SAMPLE_CYCLE_V  0xFF
#define APB_CTRL_SARADC_SAMPLE_CYCLE_S  24
/* APB_CTRL_SARADC_SAMPLE_NUM : R/W ;bitpos:[23:16] ;default: 8'd0 ; */
/*description: sample number*/
#define APB_CTRL_SARADC_SAMPLE_NUM  0x000000FF
#define APB_CTRL_SARADC_SAMPLE_NUM_M  ((APB_CTRL_SARADC_SAMPLE_NUM_V)<<(APB_CTRL_SARADC_SAMPLE_NUM_S))
#define APB_CTRL_SARADC_SAMPLE_NUM_V  0xFF
#define APB_CTRL_SARADC_SAMPLE_NUM_S  16

#define APB_CTRL_SARADC_FSM_WAIT_REG          (DR_REG_APB_CTRL_BASE + 0x014)
/* APB_CTRL_SARADC_STANDBY_WAIT : R/W ;bitpos:[23:16] ;default: 8'd255 ; */
/*description: */
#define APB_CTRL_SARADC_STANDBY_WAIT  0x000000FF
#define APB_CTRL_SARADC_STANDBY_WAIT_M  ((APB_CTRL_SARADC_STANDBY_WAIT_V)<<(APB_CTRL_SARADC_STANDBY_WAIT_S))
#define APB_CTRL_SARADC_STANDBY_WAIT_V  0xFF
#define APB_CTRL_SARADC_STANDBY_WAIT_S  16
/* APB_CTRL_SARADC_RSTB_WAIT : R/W ;bitpos:[15:8] ;default: 8'd8 ; */
/*description: */
#define APB_CTRL_SARADC_RSTB_WAIT  0x000000FF
#define APB_CTRL_SARADC_RSTB_WAIT_M  ((APB_CTRL_SARADC_RSTB_WAIT_V)<<(APB_CTRL_SARADC_RSTB_WAIT_S))
#define APB_CTRL_SARADC_RSTB_WAIT_V  0xFF
#define APB_CTRL_SARADC_RSTB_WAIT_S  8
/* APB_CTRL_SARADC_XPD_WAIT : R/W ;bitpos:[7:0] ;default: 8'd8 ; */
/*description: */
#define APB_CTRL_SARADC_XPD_WAIT  0x000000FF
#define APB_CTRL_SARADC_XPD_WAIT_M  ((APB_CTRL_SARADC_XPD_WAIT_V)<<(APB_CTRL_SARADC_XPD_WAIT_S))
#define APB_CTRL_SARADC_XPD_WAIT_V  0xFF
#define APB_CTRL_SARADC_XPD_WAIT_S  0

#define APB_CTRL_SARADC_SAR1_STATUS_REG          (DR_REG_APB_CTRL_BASE + 0x018)
/* APB_CTRL_SARADC_SAR1_STATUS : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define APB_CTRL_SARADC_SAR1_STATUS  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_STATUS_M  ((APB_CTRL_SARADC_SAR1_STATUS_V)<<(APB_CTRL_SARADC_SAR1_STATUS_S))
#define APB_CTRL_SARADC_SAR1_STATUS_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_STATUS_S  0

#define APB_CTRL_SARADC_SAR2_STATUS_REG          (DR_REG_APB_CTRL_BASE + 0x01C)
/* APB_CTRL_SARADC_SAR2_STATUS : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define APB_CTRL_SARADC_SAR2_STATUS  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_STATUS_M  ((APB_CTRL_SARADC_SAR2_STATUS_V)<<(APB_CTRL_SARADC_SAR2_STATUS_S))
#define APB_CTRL_SARADC_SAR2_STATUS_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_STATUS_S  0

#define APB_CTRL_SARADC_SAR1_PATT_TAB1_REG          (DR_REG_APB_CTRL_BASE + 0x020)
/* APB_CTRL_SARADC_SAR1_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 1 (each item one byte)*/
#define APB_CTRL_SARADC_SAR1_PATT_TAB1  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB1_M  ((APB_CTRL_SARADC_SAR1_PATT_TAB1_V)<<(APB_CTRL_SARADC_SAR1_PATT_TAB1_S))
#define APB_CTRL_SARADC_SAR1_PATT_TAB1_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB1_S  0

#define APB_CTRL_SARADC_SAR1_PATT_TAB2_REG          (DR_REG_APB_CTRL_BASE + 0x024)
/* APB_CTRL_SARADC_SAR1_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 1 (each item one byte)*/
#define APB_CTRL_SARADC_SAR1_PATT_TAB2  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB2_M  ((APB_CTRL_SARADC_SAR1_PATT_TAB2_V)<<(APB_CTRL_SARADC_SAR1_PATT_TAB2_S))
#define APB_CTRL_SARADC_SAR1_PATT_TAB2_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB2_S  0

#define APB_CTRL_SARADC_SAR1_PATT_TAB3_REG          (DR_REG_APB_CTRL_BASE + 0x028)
/* APB_CTRL_SARADC_SAR1_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 1 (each item one byte)*/
#define APB_CTRL_SARADC_SAR1_PATT_TAB3  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB3_M  ((APB_CTRL_SARADC_SAR1_PATT_TAB3_V)<<(APB_CTRL_SARADC_SAR1_PATT_TAB3_S))
#define APB_CTRL_SARADC_SAR1_PATT_TAB3_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB3_S  0

#define APB_CTRL_SARADC_SAR1_PATT_TAB4_REG          (DR_REG_APB_CTRL_BASE + 0x02C)
/* APB_CTRL_SARADC_SAR1_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 1 (each item one byte)*/
#define APB_CTRL_SARADC_SAR1_PATT_TAB4  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB4_M  ((APB_CTRL_SARADC_SAR1_PATT_TAB4_V)<<(APB_CTRL_SARADC_SAR1_PATT_TAB4_S))
#define APB_CTRL_SARADC_SAR1_PATT_TAB4_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR1_PATT_TAB4_S  0

#define APB_CTRL_SARADC_SAR2_PATT_TAB1_REG          (DR_REG_APB_CTRL_BASE + 0x030)
/* APB_CTRL_SARADC_SAR2_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 2 (each item one byte)*/
#define APB_CTRL_SARADC_SAR2_PATT_TAB1  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB1_M  ((APB_CTRL_SARADC_SAR2_PATT_TAB1_V)<<(APB_CTRL_SARADC_SAR2_PATT_TAB1_S))
#define APB_CTRL_SARADC_SAR2_PATT_TAB1_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB1_S  0

#define APB_CTRL_SARADC_SAR2_PATT_TAB2_REG          (DR_REG_APB_CTRL_BASE + 0x034)
/* APB_CTRL_SARADC_SAR2_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 2 (each item one byte)*/
#define APB_CTRL_SARADC_SAR2_PATT_TAB2  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB2_M  ((APB_CTRL_SARADC_SAR2_PATT_TAB2_V)<<(APB_CTRL_SARADC_SAR2_PATT_TAB2_S))
#define APB_CTRL_SARADC_SAR2_PATT_TAB2_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB2_S  0

#define APB_CTRL_SARADC_SAR2_PATT_TAB3_REG          (DR_REG_APB_CTRL_BASE + 0x038)
/* APB_CTRL_SARADC_SAR2_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 2 (each item one byte)*/
#define APB_CTRL_SARADC_SAR2_PATT_TAB3  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB3_M  ((APB_CTRL_SARADC_SAR2_PATT_TAB3_V)<<(APB_CTRL_SARADC_SAR2_PATT_TAB3_S))
#define APB_CTRL_SARADC_SAR2_PATT_TAB3_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB3_S  0

#define APB_CTRL_SARADC_SAR2_PATT_TAB4_REG          (DR_REG_APB_CTRL_BASE + 0x03C)
/* APB_CTRL_SARADC_SAR2_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 2 (each item one byte)*/
#define APB_CTRL_SARADC_SAR2_PATT_TAB4  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB4_M  ((APB_CTRL_SARADC_SAR2_PATT_TAB4_V)<<(APB_CTRL_SARADC_SAR2_PATT_TAB4_S))
#define APB_CTRL_SARADC_SAR2_PATT_TAB4_V  0xFFFFFFFF
#define APB_CTRL_SARADC_SAR2_PATT_TAB4_S  0

#define APB_CTRL_ADC_ARB_CTRL_REG          (DR_REG_APB_CTRL_BASE + 0x040)
/* APB_CTRL_ADC_ARB_FIX_PRIORITY : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: adc2 arbiter uses fixed priority*/
#define APB_CTRL_ADC_ARB_FIX_PRIORITY  (BIT(12))
#define APB_CTRL_ADC_ARB_FIX_PRIORITY_M  (BIT(12))
#define APB_CTRL_ADC_ARB_FIX_PRIORITY_V  0x1
#define APB_CTRL_ADC_ARB_FIX_PRIORITY_S  12
/* APB_CTRL_ADC_ARB_WIFI_PRIORITY : R/W ;bitpos:[11:10] ;default: 2'd2 ; */
/*description: Set adc2 arbiter wifi priority*/
#define APB_CTRL_ADC_ARB_WIFI_PRIORITY  0x00000003
#define APB_CTRL_ADC_ARB_WIFI_PRIORITY_M  ((APB_CTRL_ADC_ARB_WIFI_PRIORITY_V)<<(APB_CTRL_ADC_ARB_WIFI_PRIORITY_S))
#define APB_CTRL_ADC_ARB_WIFI_PRIORITY_V  0x3
#define APB_CTRL_ADC_ARB_WIFI_PRIORITY_S  10
/* APB_CTRL_ADC_ARB_RTC_PRIORITY : R/W ;bitpos:[9:8] ;default: 2'd1 ; */
/*description: Set adc2 arbiter rtc priority*/
#define APB_CTRL_ADC_ARB_RTC_PRIORITY  0x00000003
#define APB_CTRL_ADC_ARB_RTC_PRIORITY_M  ((APB_CTRL_ADC_ARB_RTC_PRIORITY_V)<<(APB_CTRL_ADC_ARB_RTC_PRIORITY_S))
#define APB_CTRL_ADC_ARB_RTC_PRIORITY_V  0x3
#define APB_CTRL_ADC_ARB_RTC_PRIORITY_S  8
/* APB_CTRL_ADC_ARB_APB_PRIORITY : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Set adc2 arbiter apb priority*/
#define APB_CTRL_ADC_ARB_APB_PRIORITY  0x00000003
#define APB_CTRL_ADC_ARB_APB_PRIORITY_M  ((APB_CTRL_ADC_ARB_APB_PRIORITY_V)<<(APB_CTRL_ADC_ARB_APB_PRIORITY_S))
#define APB_CTRL_ADC_ARB_APB_PRIORITY_V  0x3
#define APB_CTRL_ADC_ARB_APB_PRIORITY_S  6
/* APB_CTRL_ADC_ARB_GRANT_FORCE : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: adc2 arbiter force grant*/
#define APB_CTRL_ADC_ARB_GRANT_FORCE  (BIT(5))
#define APB_CTRL_ADC_ARB_GRANT_FORCE_M  (BIT(5))
#define APB_CTRL_ADC_ARB_GRANT_FORCE_V  0x1
#define APB_CTRL_ADC_ARB_GRANT_FORCE_S  5
/* APB_CTRL_ADC_ARB_WIFI_FORCE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enable wifi controller*/
#define APB_CTRL_ADC_ARB_WIFI_FORCE  (BIT(4))
#define APB_CTRL_ADC_ARB_WIFI_FORCE_M  (BIT(4))
#define APB_CTRL_ADC_ARB_WIFI_FORCE_V  0x1
#define APB_CTRL_ADC_ARB_WIFI_FORCE_S  4
/* APB_CTRL_ADC_ARB_RTC_FORCE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enable rtc controller*/
#define APB_CTRL_ADC_ARB_RTC_FORCE  (BIT(3))
#define APB_CTRL_ADC_ARB_RTC_FORCE_M  (BIT(3))
#define APB_CTRL_ADC_ARB_RTC_FORCE_V  0x1
#define APB_CTRL_ADC_ARB_RTC_FORCE_S  3
/* APB_CTRL_ADC_ARB_APB_FORCE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enable apb controller*/
#define APB_CTRL_ADC_ARB_APB_FORCE  (BIT(2))
#define APB_CTRL_ADC_ARB_APB_FORCE_M  (BIT(2))
#define APB_CTRL_ADC_ARB_APB_FORCE_V  0x1
#define APB_CTRL_ADC_ARB_APB_FORCE_S  2

#define APB_CTRL_CLK_OUT_EN_REG          (DR_REG_APB_CTRL_BASE + 0x044)
/* APB_CTRL_CLK_XTAL_OEN : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK_XTAL_OEN  (BIT(10))
#define APB_CTRL_CLK_XTAL_OEN_M  (BIT(10))
#define APB_CTRL_CLK_XTAL_OEN_V  0x1
#define APB_CTRL_CLK_XTAL_OEN_S  10
/* APB_CTRL_CLK40X_BB_OEN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK40X_BB_OEN  (BIT(9))
#define APB_CTRL_CLK40X_BB_OEN_M  (BIT(9))
#define APB_CTRL_CLK40X_BB_OEN_V  0x1
#define APB_CTRL_CLK40X_BB_OEN_S  9
/* APB_CTRL_CLK_DAC_CPU_OEN : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK_DAC_CPU_OEN  (BIT(8))
#define APB_CTRL_CLK_DAC_CPU_OEN_M  (BIT(8))
#define APB_CTRL_CLK_DAC_CPU_OEN_V  0x1
#define APB_CTRL_CLK_DAC_CPU_OEN_S  8
/* APB_CTRL_CLK_ADC_INF_OEN : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK_ADC_INF_OEN  (BIT(7))
#define APB_CTRL_CLK_ADC_INF_OEN_M  (BIT(7))
#define APB_CTRL_CLK_ADC_INF_OEN_V  0x1
#define APB_CTRL_CLK_ADC_INF_OEN_S  7
/* APB_CTRL_CLK_320M_OEN : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK_320M_OEN  (BIT(6))
#define APB_CTRL_CLK_320M_OEN_M  (BIT(6))
#define APB_CTRL_CLK_320M_OEN_V  0x1
#define APB_CTRL_CLK_320M_OEN_S  6
/* APB_CTRL_CLK160_OEN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK160_OEN  (BIT(5))
#define APB_CTRL_CLK160_OEN_M  (BIT(5))
#define APB_CTRL_CLK160_OEN_V  0x1
#define APB_CTRL_CLK160_OEN_S  5
/* APB_CTRL_CLK80_OEN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK80_OEN  (BIT(4))
#define APB_CTRL_CLK80_OEN_M  (BIT(4))
#define APB_CTRL_CLK80_OEN_V  0x1
#define APB_CTRL_CLK80_OEN_S  4
/* APB_CTRL_CLK_BB_OEN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK_BB_OEN  (BIT(3))
#define APB_CTRL_CLK_BB_OEN_M  (BIT(3))
#define APB_CTRL_CLK_BB_OEN_V  0x1
#define APB_CTRL_CLK_BB_OEN_S  3
/* APB_CTRL_CLK44_OEN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK44_OEN  (BIT(2))
#define APB_CTRL_CLK44_OEN_M  (BIT(2))
#define APB_CTRL_CLK44_OEN_V  0x1
#define APB_CTRL_CLK44_OEN_S  2
/* APB_CTRL_CLK22_OEN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK22_OEN  (BIT(1))
#define APB_CTRL_CLK22_OEN_M  (BIT(1))
#define APB_CTRL_CLK22_OEN_V  0x1
#define APB_CTRL_CLK22_OEN_S  1
/* APB_CTRL_CLK20_OEN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_CLK20_OEN  (BIT(0))
#define APB_CTRL_CLK20_OEN_M  (BIT(0))
#define APB_CTRL_CLK20_OEN_V  0x1
#define APB_CTRL_CLK20_OEN_S  0

#define APB_CTRL_HOST_INF_SEL_REG          (DR_REG_APB_CTRL_BASE + 0x048)
/* APB_CTRL_SPI_PRIOR : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_SPI_PRIOR  (BIT(13))
#define APB_CTRL_SPI_PRIOR_M  (BIT(13))
#define APB_CTRL_SPI_PRIOR_V  0x1
#define APB_CTRL_SPI_PRIOR_S  13
/* APB_CTRL_SPI1_HOLD : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_SPI1_HOLD  (BIT(9))
#define APB_CTRL_SPI1_HOLD_M  (BIT(9))
#define APB_CTRL_SPI1_HOLD_V  0x1
#define APB_CTRL_SPI1_HOLD_S  9
/* APB_CTRL_SPI0_HOLD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_SPI0_HOLD  (BIT(8))
#define APB_CTRL_SPI0_HOLD_M  (BIT(8))
#define APB_CTRL_SPI0_HOLD_V  0x1
#define APB_CTRL_SPI0_HOLD_S  8
/* APB_CTRL_PERI_IO_SWAP : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define APB_CTRL_PERI_IO_SWAP  0x000000FF
#define APB_CTRL_PERI_IO_SWAP_M  ((APB_CTRL_PERI_IO_SWAP_V)<<(APB_CTRL_PERI_IO_SWAP_S))
#define APB_CTRL_PERI_IO_SWAP_V  0xFF
#define APB_CTRL_PERI_IO_SWAP_S  0

#define APB_CTRL_EXT_MEM_PMS_LOCK_REG          (DR_REG_APB_CTRL_BASE + 0x04C)
/* APB_CTRL_EXT_MEM_PMS_LOCK : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_EXT_MEM_PMS_LOCK  (BIT(0))
#define APB_CTRL_EXT_MEM_PMS_LOCK_M  (BIT(0))
#define APB_CTRL_EXT_MEM_PMS_LOCK_V  0x1
#define APB_CTRL_EXT_MEM_PMS_LOCK_S  0

#define APB_CTRL_FLASH_ACE0_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x050)
/* APB_CTRL_FLASH_ACE0_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_FLASH_ACE0_ATTR  0x00000007
#define APB_CTRL_FLASH_ACE0_ATTR_M  ((APB_CTRL_FLASH_ACE0_ATTR_V)<<(APB_CTRL_FLASH_ACE0_ATTR_S))
#define APB_CTRL_FLASH_ACE0_ATTR_V  0x7
#define APB_CTRL_FLASH_ACE0_ATTR_S  0

#define APB_CTRL_FLASH_ACE1_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x054)
/* APB_CTRL_FLASH_ACE1_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_FLASH_ACE1_ATTR  0x00000007
#define APB_CTRL_FLASH_ACE1_ATTR_M  ((APB_CTRL_FLASH_ACE1_ATTR_V)<<(APB_CTRL_FLASH_ACE1_ATTR_S))
#define APB_CTRL_FLASH_ACE1_ATTR_V  0x7
#define APB_CTRL_FLASH_ACE1_ATTR_S  0

#define APB_CTRL_FLASH_ACE2_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x058)
/* APB_CTRL_FLASH_ACE2_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_FLASH_ACE2_ATTR  0x00000007
#define APB_CTRL_FLASH_ACE2_ATTR_M  ((APB_CTRL_FLASH_ACE2_ATTR_V)<<(APB_CTRL_FLASH_ACE2_ATTR_S))
#define APB_CTRL_FLASH_ACE2_ATTR_V  0x7
#define APB_CTRL_FLASH_ACE2_ATTR_S  0

#define APB_CTRL_FLASH_ACE3_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x05C)
/* APB_CTRL_FLASH_ACE3_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_FLASH_ACE3_ATTR  0x00000007
#define APB_CTRL_FLASH_ACE3_ATTR_M  ((APB_CTRL_FLASH_ACE3_ATTR_V)<<(APB_CTRL_FLASH_ACE3_ATTR_S))
#define APB_CTRL_FLASH_ACE3_ATTR_V  0x7
#define APB_CTRL_FLASH_ACE3_ATTR_S  0

#define APB_CTRL_FLASH_ACE0_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x060)
/* APB_CTRL_FLASH_ACE0_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define APB_CTRL_FLASH_ACE0_ADDR_S  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE0_ADDR_S_M  ((APB_CTRL_FLASH_ACE0_ADDR_S_V)<<(APB_CTRL_FLASH_ACE0_ADDR_S_S))
#define APB_CTRL_FLASH_ACE0_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE0_ADDR_S_S  0

#define APB_CTRL_FLASH_ACE1_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x064)
/* APB_CTRL_FLASH_ACE1_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h10000000 ; */
/*description: */
#define APB_CTRL_FLASH_ACE1_ADDR_S  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE1_ADDR_S_M  ((APB_CTRL_FLASH_ACE1_ADDR_S_V)<<(APB_CTRL_FLASH_ACE1_ADDR_S_S))
#define APB_CTRL_FLASH_ACE1_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE1_ADDR_S_S  0

#define APB_CTRL_FLASH_ACE2_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x068)
/* APB_CTRL_FLASH_ACE2_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h20000000 ; */
/*description: */
#define APB_CTRL_FLASH_ACE2_ADDR_S  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE2_ADDR_S_M  ((APB_CTRL_FLASH_ACE2_ADDR_S_V)<<(APB_CTRL_FLASH_ACE2_ADDR_S_S))
#define APB_CTRL_FLASH_ACE2_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE2_ADDR_S_S  0

#define APB_CTRL_FLASH_ACE3_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x06C)
/* APB_CTRL_FLASH_ACE3_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h30000000 ; */
/*description: */
#define APB_CTRL_FLASH_ACE3_ADDR_S  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE3_ADDR_S_M  ((APB_CTRL_FLASH_ACE3_ADDR_S_V)<<(APB_CTRL_FLASH_ACE3_ADDR_S_S))
#define APB_CTRL_FLASH_ACE3_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_FLASH_ACE3_ADDR_S_S  0

#define APB_CTRL_FLASH_ACE0_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x070)
/* APB_CTRL_FLASH_ACE0_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_FLASH_ACE0_SIZE  0x0000FFFF
#define APB_CTRL_FLASH_ACE0_SIZE_M  ((APB_CTRL_FLASH_ACE0_SIZE_V)<<(APB_CTRL_FLASH_ACE0_SIZE_S))
#define APB_CTRL_FLASH_ACE0_SIZE_V  0xFFFF
#define APB_CTRL_FLASH_ACE0_SIZE_S  0

#define APB_CTRL_FLASH_ACE1_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x074)
/* APB_CTRL_FLASH_ACE1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_FLASH_ACE1_SIZE  0x0000FFFF
#define APB_CTRL_FLASH_ACE1_SIZE_M  ((APB_CTRL_FLASH_ACE1_SIZE_V)<<(APB_CTRL_FLASH_ACE1_SIZE_S))
#define APB_CTRL_FLASH_ACE1_SIZE_V  0xFFFF
#define APB_CTRL_FLASH_ACE1_SIZE_S  0

#define APB_CTRL_FLASH_ACE2_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x078)
/* APB_CTRL_FLASH_ACE2_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_FLASH_ACE2_SIZE  0x0000FFFF
#define APB_CTRL_FLASH_ACE2_SIZE_M  ((APB_CTRL_FLASH_ACE2_SIZE_V)<<(APB_CTRL_FLASH_ACE2_SIZE_S))
#define APB_CTRL_FLASH_ACE2_SIZE_V  0xFFFF
#define APB_CTRL_FLASH_ACE2_SIZE_S  0

#define APB_CTRL_FLASH_ACE3_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x07C)
/* APB_CTRL_FLASH_ACE3_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_FLASH_ACE3_SIZE  0x0000FFFF
#define APB_CTRL_FLASH_ACE3_SIZE_M  ((APB_CTRL_FLASH_ACE3_SIZE_V)<<(APB_CTRL_FLASH_ACE3_SIZE_S))
#define APB_CTRL_FLASH_ACE3_SIZE_V  0xFFFF
#define APB_CTRL_FLASH_ACE3_SIZE_S  0

#define APB_CTRL_SRAM_ACE0_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x080)
/* APB_CTRL_SRAM_ACE0_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_SRAM_ACE0_ATTR  0x00000007
#define APB_CTRL_SRAM_ACE0_ATTR_M  ((APB_CTRL_SRAM_ACE0_ATTR_V)<<(APB_CTRL_SRAM_ACE0_ATTR_S))
#define APB_CTRL_SRAM_ACE0_ATTR_V  0x7
#define APB_CTRL_SRAM_ACE0_ATTR_S  0

#define APB_CTRL_SRAM_ACE1_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x084)
/* APB_CTRL_SRAM_ACE1_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_SRAM_ACE1_ATTR  0x00000007
#define APB_CTRL_SRAM_ACE1_ATTR_M  ((APB_CTRL_SRAM_ACE1_ATTR_V)<<(APB_CTRL_SRAM_ACE1_ATTR_S))
#define APB_CTRL_SRAM_ACE1_ATTR_V  0x7
#define APB_CTRL_SRAM_ACE1_ATTR_S  0

#define APB_CTRL_SRAM_ACE2_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x088)
/* APB_CTRL_SRAM_ACE2_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_SRAM_ACE2_ATTR  0x00000007
#define APB_CTRL_SRAM_ACE2_ATTR_M  ((APB_CTRL_SRAM_ACE2_ATTR_V)<<(APB_CTRL_SRAM_ACE2_ATTR_S))
#define APB_CTRL_SRAM_ACE2_ATTR_V  0x7
#define APB_CTRL_SRAM_ACE2_ATTR_S  0

#define APB_CTRL_SRAM_ACE3_ATTR_REG          (DR_REG_APB_CTRL_BASE + 0x08C)
/* APB_CTRL_SRAM_ACE3_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define APB_CTRL_SRAM_ACE3_ATTR  0x00000007
#define APB_CTRL_SRAM_ACE3_ATTR_M  ((APB_CTRL_SRAM_ACE3_ATTR_V)<<(APB_CTRL_SRAM_ACE3_ATTR_S))
#define APB_CTRL_SRAM_ACE3_ATTR_V  0x7
#define APB_CTRL_SRAM_ACE3_ATTR_S  0

#define APB_CTRL_SRAM_ACE0_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x090)
/* APB_CTRL_SRAM_ACE0_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define APB_CTRL_SRAM_ACE0_ADDR_S  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE0_ADDR_S_M  ((APB_CTRL_SRAM_ACE0_ADDR_S_V)<<(APB_CTRL_SRAM_ACE0_ADDR_S_S))
#define APB_CTRL_SRAM_ACE0_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE0_ADDR_S_S  0

#define APB_CTRL_SRAM_ACE1_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x094)
/* APB_CTRL_SRAM_ACE1_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h10000000 ; */
/*description: */
#define APB_CTRL_SRAM_ACE1_ADDR_S  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE1_ADDR_S_M  ((APB_CTRL_SRAM_ACE1_ADDR_S_V)<<(APB_CTRL_SRAM_ACE1_ADDR_S_S))
#define APB_CTRL_SRAM_ACE1_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE1_ADDR_S_S  0

#define APB_CTRL_SRAM_ACE2_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x098)
/* APB_CTRL_SRAM_ACE2_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h20000000 ; */
/*description: */
#define APB_CTRL_SRAM_ACE2_ADDR_S  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE2_ADDR_S_M  ((APB_CTRL_SRAM_ACE2_ADDR_S_V)<<(APB_CTRL_SRAM_ACE2_ADDR_S_S))
#define APB_CTRL_SRAM_ACE2_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE2_ADDR_S_S  0

#define APB_CTRL_SRAM_ACE3_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x09C)
/* APB_CTRL_SRAM_ACE3_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h30000000 ; */
/*description: */
#define APB_CTRL_SRAM_ACE3_ADDR_S  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE3_ADDR_S_M  ((APB_CTRL_SRAM_ACE3_ADDR_S_V)<<(APB_CTRL_SRAM_ACE3_ADDR_S_S))
#define APB_CTRL_SRAM_ACE3_ADDR_S_V  0xFFFFFFFF
#define APB_CTRL_SRAM_ACE3_ADDR_S_S  0

#define APB_CTRL_SRAM_ACE0_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x0A0)
/* APB_CTRL_SRAM_ACE0_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_SRAM_ACE0_SIZE  0x0000FFFF
#define APB_CTRL_SRAM_ACE0_SIZE_M  ((APB_CTRL_SRAM_ACE0_SIZE_V)<<(APB_CTRL_SRAM_ACE0_SIZE_S))
#define APB_CTRL_SRAM_ACE0_SIZE_V  0xFFFF
#define APB_CTRL_SRAM_ACE0_SIZE_S  0

#define APB_CTRL_SRAM_ACE1_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x0A4)
/* APB_CTRL_SRAM_ACE1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_SRAM_ACE1_SIZE  0x0000FFFF
#define APB_CTRL_SRAM_ACE1_SIZE_M  ((APB_CTRL_SRAM_ACE1_SIZE_V)<<(APB_CTRL_SRAM_ACE1_SIZE_S))
#define APB_CTRL_SRAM_ACE1_SIZE_V  0xFFFF
#define APB_CTRL_SRAM_ACE1_SIZE_S  0

#define APB_CTRL_SRAM_ACE2_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x0A8)
/* APB_CTRL_SRAM_ACE2_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_SRAM_ACE2_SIZE  0x0000FFFF
#define APB_CTRL_SRAM_ACE2_SIZE_M  ((APB_CTRL_SRAM_ACE2_SIZE_V)<<(APB_CTRL_SRAM_ACE2_SIZE_S))
#define APB_CTRL_SRAM_ACE2_SIZE_V  0xFFFF
#define APB_CTRL_SRAM_ACE2_SIZE_S  0

#define APB_CTRL_SRAM_ACE3_SIZE_REG          (DR_REG_APB_CTRL_BASE + 0x0AC)
/* APB_CTRL_SRAM_ACE3_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define APB_CTRL_SRAM_ACE3_SIZE  0x0000FFFF
#define APB_CTRL_SRAM_ACE3_SIZE_M  ((APB_CTRL_SRAM_ACE3_SIZE_V)<<(APB_CTRL_SRAM_ACE3_SIZE_S))
#define APB_CTRL_SRAM_ACE3_SIZE_V  0xFFFF
#define APB_CTRL_SRAM_ACE3_SIZE_S  0

#define APB_CTRL_SPI0_PMS_CTRL_REG          (DR_REG_APB_CTRL_BASE + 0x0B0)
/* APB_CTRL_SPI0_REJECT_CDE : RO ;bitpos:[6:2] ;default: 5'h0 ; */
/*description: */
#define APB_CTRL_SPI0_REJECT_CDE  0x0000001F
#define APB_CTRL_SPI0_REJECT_CDE_M  ((APB_CTRL_SPI0_REJECT_CDE_V)<<(APB_CTRL_SPI0_REJECT_CDE_S))
#define APB_CTRL_SPI0_REJECT_CDE_V  0x1F
#define APB_CTRL_SPI0_REJECT_CDE_S  2
/* APB_CTRL_SPI0_REJECT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_SPI0_REJECT_CLR  (BIT(1))
#define APB_CTRL_SPI0_REJECT_CLR_M  (BIT(1))
#define APB_CTRL_SPI0_REJECT_CLR_V  0x1
#define APB_CTRL_SPI0_REJECT_CLR_S  1
/* APB_CTRL_SPI0_REJECT_INT : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_SPI0_REJECT_INT  (BIT(0))
#define APB_CTRL_SPI0_REJECT_INT_M  (BIT(0))
#define APB_CTRL_SPI0_REJECT_INT_V  0x1
#define APB_CTRL_SPI0_REJECT_INT_S  0

#define APB_CTRL_SPI0_REJECT_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x0B4)
/* APB_CTRL_SPI0_REJECT_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define APB_CTRL_SPI0_REJECT_ADDR  0xFFFFFFFF
#define APB_CTRL_SPI0_REJECT_ADDR_M  ((APB_CTRL_SPI0_REJECT_ADDR_V)<<(APB_CTRL_SPI0_REJECT_ADDR_S))
#define APB_CTRL_SPI0_REJECT_ADDR_V  0xFFFFFFFF
#define APB_CTRL_SPI0_REJECT_ADDR_S  0

#define APB_CTRL_SPI1_PMS_CTRL_REG          (DR_REG_APB_CTRL_BASE + 0x0B8)
/* APB_CTRL_SPI1_REJECT_CDE : RO ;bitpos:[6:2] ;default: 5'h0 ; */
/*description: */
#define APB_CTRL_SPI1_REJECT_CDE  0x0000001F
#define APB_CTRL_SPI1_REJECT_CDE_M  ((APB_CTRL_SPI1_REJECT_CDE_V)<<(APB_CTRL_SPI1_REJECT_CDE_S))
#define APB_CTRL_SPI1_REJECT_CDE_V  0x1F
#define APB_CTRL_SPI1_REJECT_CDE_S  2
/* APB_CTRL_SPI1_REJECT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_SPI1_REJECT_CLR  (BIT(1))
#define APB_CTRL_SPI1_REJECT_CLR_M  (BIT(1))
#define APB_CTRL_SPI1_REJECT_CLR_V  0x1
#define APB_CTRL_SPI1_REJECT_CLR_S  1
/* APB_CTRL_SPI1_REJECT_INT : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_SPI1_REJECT_INT  (BIT(0))
#define APB_CTRL_SPI1_REJECT_INT_M  (BIT(0))
#define APB_CTRL_SPI1_REJECT_INT_V  0x1
#define APB_CTRL_SPI1_REJECT_INT_S  0

#define APB_CTRL_SPI1_REJECT_ADDR_REG          (DR_REG_APB_CTRL_BASE + 0x0BC)
/* APB_CTRL_SPI1_REJECT_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define APB_CTRL_SPI1_REJECT_ADDR  0xFFFFFFFF
#define APB_CTRL_SPI1_REJECT_ADDR_M  ((APB_CTRL_SPI1_REJECT_ADDR_V)<<(APB_CTRL_SPI1_REJECT_ADDR_S))
#define APB_CTRL_SPI1_REJECT_ADDR_V  0xFFFFFFFF
#define APB_CTRL_SPI1_REJECT_ADDR_S  0

#define APB_CTRL_SDIO_CTRL_REG          (DR_REG_APB_CTRL_BASE + 0x0C0)
/* APB_CTRL_SDIO_WIN_ACCESS_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define APB_CTRL_SDIO_WIN_ACCESS_EN  (BIT(0))
#define APB_CTRL_SDIO_WIN_ACCESS_EN_M  (BIT(0))
#define APB_CTRL_SDIO_WIN_ACCESS_EN_V  0x1
#define APB_CTRL_SDIO_WIN_ACCESS_EN_S  0

#define APB_CTRL_REDCY_SIG0_REG          (DR_REG_APB_CTRL_BASE + 0x0C4)
/* APB_CTRL_REDCY_ANDOR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define APB_CTRL_REDCY_ANDOR  (BIT(31))
#define APB_CTRL_REDCY_ANDOR_M  (BIT(31))
#define APB_CTRL_REDCY_ANDOR_V  0x1
#define APB_CTRL_REDCY_ANDOR_S  31
/* APB_CTRL_REDCY_SIG0 : R/W ;bitpos:[30:0] ;default: 31'h0 ; */
/*description: */
#define APB_CTRL_REDCY_SIG0  0x7FFFFFFF
#define APB_CTRL_REDCY_SIG0_M  ((APB_CTRL_REDCY_SIG0_V)<<(APB_CTRL_REDCY_SIG0_S))
#define APB_CTRL_REDCY_SIG0_V  0x7FFFFFFF
#define APB_CTRL_REDCY_SIG0_S  0

#define APB_CTRL_REDCY_SIG1_REG          (DR_REG_APB_CTRL_BASE + 0x0C8)
/* APB_CTRL_REDCY_NANDOR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define APB_CTRL_REDCY_NANDOR  (BIT(31))
#define APB_CTRL_REDCY_NANDOR_M  (BIT(31))
#define APB_CTRL_REDCY_NANDOR_V  0x1
#define APB_CTRL_REDCY_NANDOR_S  31
/* APB_CTRL_REDCY_SIG1 : R/W ;bitpos:[30:0] ;default: 31'h0 ; */
/*description: */
#define APB_CTRL_REDCY_SIG1  0x7FFFFFFF
#define APB_CTRL_REDCY_SIG1_M  ((APB_CTRL_REDCY_SIG1_V)<<(APB_CTRL_REDCY_SIG1_S))
#define APB_CTRL_REDCY_SIG1_V  0x7FFFFFFF
#define APB_CTRL_REDCY_SIG1_S  0

#define APB_CTRL_WIFI_BB_CFG_REG          (DR_REG_APB_CTRL_BASE + 0x0CC)
/* APB_CTRL_WIFI_BB_CFG : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define APB_CTRL_WIFI_BB_CFG  0xFFFFFFFF
#define APB_CTRL_WIFI_BB_CFG_M  ((APB_CTRL_WIFI_BB_CFG_V)<<(APB_CTRL_WIFI_BB_CFG_S))
#define APB_CTRL_WIFI_BB_CFG_V  0xFFFFFFFF
#define APB_CTRL_WIFI_BB_CFG_S  0

#define APB_CTRL_WIFI_BB_CFG_2_REG          (DR_REG_APB_CTRL_BASE + 0x0D0)
/* APB_CTRL_WIFI_BB_CFG_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define APB_CTRL_WIFI_BB_CFG_2  0xFFFFFFFF
#define APB_CTRL_WIFI_BB_CFG_2_M  ((APB_CTRL_WIFI_BB_CFG_2_V)<<(APB_CTRL_WIFI_BB_CFG_2_S))
#define APB_CTRL_WIFI_BB_CFG_2_V  0xFFFFFFFF
#define APB_CTRL_WIFI_BB_CFG_2_S  0

#define APB_CTRL_WIFI_CLK_EN_REG          (DR_REG_APB_CTRL_BASE + 0x0D4)
/* APB_CTRL_WIFI_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'hfffce030 ; */
/*description: */
#define APB_CTRL_WIFI_CLK_EN  0xFFFFFFFF
#define APB_CTRL_WIFI_CLK_EN_M  ((APB_CTRL_WIFI_CLK_EN_V)<<(APB_CTRL_WIFI_CLK_EN_S))
#define APB_CTRL_WIFI_CLK_EN_V  0xFFFFFFFF
#define APB_CTRL_WIFI_CLK_EN_S  0

#define APB_CTRL_WIFI_RST_EN_REG          (DR_REG_APB_CTRL_BASE + 0x0D8)
/* APB_CTRL_WIFI_RST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define APB_CTRL_WIFI_RST  0xFFFFFFFF
#define APB_CTRL_WIFI_RST_M  ((APB_CTRL_WIFI_RST_V)<<(APB_CTRL_WIFI_RST_S))
#define APB_CTRL_WIFI_RST_V  0xFFFFFFFF
#define APB_CTRL_WIFI_RST_S  0

#define APB_CTRL_FRONT_END_MEM_PD_REG          (DR_REG_APB_CTRL_BASE + 0x0DC)
/* APB_CTRL_DC_MEM_FORCE_PD : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_DC_MEM_FORCE_PD  (BIT(5))
#define APB_CTRL_DC_MEM_FORCE_PD_M  (BIT(5))
#define APB_CTRL_DC_MEM_FORCE_PD_V  0x1
#define APB_CTRL_DC_MEM_FORCE_PD_S  5
/* APB_CTRL_DC_MEM_FORCE_PU : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_DC_MEM_FORCE_PU  (BIT(4))
#define APB_CTRL_DC_MEM_FORCE_PU_M  (BIT(4))
#define APB_CTRL_DC_MEM_FORCE_PU_V  0x1
#define APB_CTRL_DC_MEM_FORCE_PU_S  4
/* APB_CTRL_PBUS_MEM_FORCE_PD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_PBUS_MEM_FORCE_PD  (BIT(3))
#define APB_CTRL_PBUS_MEM_FORCE_PD_M  (BIT(3))
#define APB_CTRL_PBUS_MEM_FORCE_PD_V  0x1
#define APB_CTRL_PBUS_MEM_FORCE_PD_S  3
/* APB_CTRL_PBUS_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_PBUS_MEM_FORCE_PU  (BIT(2))
#define APB_CTRL_PBUS_MEM_FORCE_PU_M  (BIT(2))
#define APB_CTRL_PBUS_MEM_FORCE_PU_V  0x1
#define APB_CTRL_PBUS_MEM_FORCE_PU_S  2
/* APB_CTRL_AGC_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define APB_CTRL_AGC_MEM_FORCE_PD  (BIT(1))
#define APB_CTRL_AGC_MEM_FORCE_PD_M  (BIT(1))
#define APB_CTRL_AGC_MEM_FORCE_PD_V  0x1
#define APB_CTRL_AGC_MEM_FORCE_PD_S  1
/* APB_CTRL_AGC_MEM_FORCE_PU : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define APB_CTRL_AGC_MEM_FORCE_PU  (BIT(0))
#define APB_CTRL_AGC_MEM_FORCE_PU_M  (BIT(0))
#define APB_CTRL_AGC_MEM_FORCE_PU_V  0x1
#define APB_CTRL_AGC_MEM_FORCE_PU_S  0

#define APB_CTRL_DATE_REG          (DR_REG_APB_CTRL_BASE + 0x3FC)
/* APB_CTRL_DATE : R/W ;bitpos:[31:0] ;default: 32'h18102500 ; */
/*description: */
#define APB_CTRL_DATE  0xFFFFFFFF
#define APB_CTRL_DATE_M  ((APB_CTRL_DATE_V)<<(APB_CTRL_DATE_S))
#define APB_CTRL_DATE_V  0xFFFFFFFF
#define APB_CTRL_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_APB_CTRL_REG_H_ */


