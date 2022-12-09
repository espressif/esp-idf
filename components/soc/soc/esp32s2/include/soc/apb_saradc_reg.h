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
#ifndef _SOC_APB_SARADC_REG_H_
#define _SOC_APB_SARADC_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define APB_SARADC_CTRL_REG          (DR_REG_APB_SARADC_BASE + 0x000)
/* APB_SARADC_WAIT_ARB_CYCLE : R/W ;bitpos:[31:30] ;default: 2'd1 ; */
/*description: wait arbit signal stable after sar_done*/
#define APB_SARADC_WAIT_ARB_CYCLE  0x00000003
#define APB_SARADC_WAIT_ARB_CYCLE_M  ((APB_SARADC_WAIT_ARB_CYCLE_V)<<(APB_SARADC_WAIT_ARB_CYCLE_S))
#define APB_SARADC_WAIT_ARB_CYCLE_V  0x3
#define APB_SARADC_WAIT_ARB_CYCLE_S  30
/* APB_SARADC_XPD_SAR_FORCE : R/W ;bitpos:[28:27] ;default: 2'd0 ; */
/*description: force option to xpd sar blocks*/
#define APB_SARADC_XPD_SAR_FORCE  0x00000003
#define APB_SARADC_XPD_SAR_FORCE_M  ((APB_SARADC_XPD_SAR_FORCE_V)<<(APB_SARADC_XPD_SAR_FORCE_S))
#define APB_SARADC_XPD_SAR_FORCE_V  0x3
#define APB_SARADC_XPD_SAR_FORCE_S  27
/* APB_SARADC_DATA_TO_I2S : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 1: I2S input data is from SAR ADC (for DMA)  0: I2S input data
 is from GPIO matrix*/
#define APB_SARADC_DATA_TO_I2S  (BIT(26))
#define APB_SARADC_DATA_TO_I2S_M  (BIT(26))
#define APB_SARADC_DATA_TO_I2S_V  0x1
#define APB_SARADC_DATA_TO_I2S_S  26
/* APB_SARADC_DATA_SAR_SEL : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1: sar_sel will be coded by the MSB of the 16-bit output data
  in this case the resolution should not be larger than 11 bits.*/
#define APB_SARADC_DATA_SAR_SEL  (BIT(25))
#define APB_SARADC_DATA_SAR_SEL_M  (BIT(25))
#define APB_SARADC_DATA_SAR_SEL_V  0x1
#define APB_SARADC_DATA_SAR_SEL_S  25
/* APB_SARADC_SAR2_PATT_P_CLEAR : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: clear the pointer of pattern table for DIG ADC2 CTRL*/
#define APB_SARADC_SAR2_PATT_P_CLEAR  (BIT(24))
#define APB_SARADC_SAR2_PATT_P_CLEAR_M  (BIT(24))
#define APB_SARADC_SAR2_PATT_P_CLEAR_V  0x1
#define APB_SARADC_SAR2_PATT_P_CLEAR_S  24
/* APB_SARADC_SAR1_PATT_P_CLEAR : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: clear the pointer of pattern table for DIG ADC1 CTRL*/
#define APB_SARADC_SAR1_PATT_P_CLEAR  (BIT(23))
#define APB_SARADC_SAR1_PATT_P_CLEAR_M  (BIT(23))
#define APB_SARADC_SAR1_PATT_P_CLEAR_V  0x1
#define APB_SARADC_SAR1_PATT_P_CLEAR_S  23
/* APB_SARADC_SAR2_PATT_LEN : R/W ;bitpos:[22:19] ;default: 4'd15 ; */
/*description: 0 ~ 15 means length 1 ~ 16*/
#define APB_SARADC_SAR2_PATT_LEN  0x0000000F
#define APB_SARADC_SAR2_PATT_LEN_M  ((APB_SARADC_SAR2_PATT_LEN_V)<<(APB_SARADC_SAR2_PATT_LEN_S))
#define APB_SARADC_SAR2_PATT_LEN_V  0xF
#define APB_SARADC_SAR2_PATT_LEN_S  19
/* APB_SARADC_SAR1_PATT_LEN : R/W ;bitpos:[18:15] ;default: 4'd15 ; */
/*description: 0 ~ 15 means length 1 ~ 16*/
#define APB_SARADC_SAR1_PATT_LEN  0x0000000F
#define APB_SARADC_SAR1_PATT_LEN_M  ((APB_SARADC_SAR1_PATT_LEN_V)<<(APB_SARADC_SAR1_PATT_LEN_S))
#define APB_SARADC_SAR1_PATT_LEN_V  0xF
#define APB_SARADC_SAR1_PATT_LEN_S  15
/* APB_SARADC_SAR_CLK_DIV : R/W ;bitpos:[14:7] ;default: 8'd4 ; */
/*description: SAR clock divider*/
#define APB_SARADC_SAR_CLK_DIV  0x000000FF
#define APB_SARADC_SAR_CLK_DIV_M  ((APB_SARADC_SAR_CLK_DIV_V)<<(APB_SARADC_SAR_CLK_DIV_S))
#define APB_SARADC_SAR_CLK_DIV_V  0xFF
#define APB_SARADC_SAR_CLK_DIV_S  7
/* APB_SARADC_SAR_CLK_GATED : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define APB_SARADC_SAR_CLK_GATED  (BIT(6))
#define APB_SARADC_SAR_CLK_GATED_M  (BIT(6))
#define APB_SARADC_SAR_CLK_GATED_V  0x1
#define APB_SARADC_SAR_CLK_GATED_S  6
/* APB_SARADC_SAR_SEL : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: 0: SAR1  1: SAR2  only work for single SAR mode*/
#define APB_SARADC_SAR_SEL  (BIT(5))
#define APB_SARADC_SAR_SEL_M  (BIT(5))
#define APB_SARADC_SAR_SEL_V  0x1
#define APB_SARADC_SAR_SEL_S  5
/* APB_SARADC_WORK_MODE : R/W ;bitpos:[4:3] ;default: 2'd0 ; */
/*description: 0: single mode  1: double mode  2: alternate mode*/
#define APB_SARADC_WORK_MODE  0x00000003
#define APB_SARADC_WORK_MODE_M  ((APB_SARADC_WORK_MODE_V)<<(APB_SARADC_WORK_MODE_S))
#define APB_SARADC_WORK_MODE_V  0x3
#define APB_SARADC_WORK_MODE_S  3
/* APB_SARADC_START : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define APB_SARADC_START  (BIT(1))
#define APB_SARADC_START_M  (BIT(1))
#define APB_SARADC_START_V  0x1
#define APB_SARADC_START_S  1
/* APB_SARADC_START_FORCE : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define APB_SARADC_START_FORCE  (BIT(0))
#define APB_SARADC_START_FORCE_M  (BIT(0))
#define APB_SARADC_START_FORCE_V  0x1
#define APB_SARADC_START_FORCE_S  0

#define APB_SARADC_CTRL2_REG          (DR_REG_APB_SARADC_BASE + 0x004)
/* APB_SARADC_TIMER_EN : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: to enable saradc timer trigger*/
#define APB_SARADC_TIMER_EN  (BIT(24))
#define APB_SARADC_TIMER_EN_M  (BIT(24))
#define APB_SARADC_TIMER_EN_V  0x1
#define APB_SARADC_TIMER_EN_S  24
/* APB_SARADC_TIMER_TARGET : R/W ;bitpos:[23:12] ;default: 12'd10 ; */
/*description: to set saradc timer target*/
#define APB_SARADC_TIMER_TARGET  0x00000FFF
#define APB_SARADC_TIMER_TARGET_M  ((APB_SARADC_TIMER_TARGET_V)<<(APB_SARADC_TIMER_TARGET_S))
#define APB_SARADC_TIMER_TARGET_V  0xFFF
#define APB_SARADC_TIMER_TARGET_S  12
/* APB_SARADC_TIMER_SEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: 1: select saradc timer 0: i2s_ws trigger*/
#define APB_SARADC_TIMER_SEL  (BIT(11))
#define APB_SARADC_TIMER_SEL_M  (BIT(11))
#define APB_SARADC_TIMER_SEL_V  0x1
#define APB_SARADC_TIMER_SEL_S  11
/* APB_SARADC_SAR2_INV : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: 1: data to DIG ADC2 CTRL is inverted  otherwise not*/
#define APB_SARADC_SAR2_INV  (BIT(10))
#define APB_SARADC_SAR2_INV_M  (BIT(10))
#define APB_SARADC_SAR2_INV_V  0x1
#define APB_SARADC_SAR2_INV_S  10
/* APB_SARADC_SAR1_INV : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: 1: data to DIG ADC1 CTRL is inverted  otherwise not*/
#define APB_SARADC_SAR1_INV  (BIT(9))
#define APB_SARADC_SAR1_INV_M  (BIT(9))
#define APB_SARADC_SAR1_INV_V  0x1
#define APB_SARADC_SAR1_INV_S  9
/* APB_SARADC_MAX_MEAS_NUM : R/W ;bitpos:[8:1] ;default: 8'd255 ; */
/*description: max conversion number*/
#define APB_SARADC_MAX_MEAS_NUM  0x000000FF
#define APB_SARADC_MAX_MEAS_NUM_M  ((APB_SARADC_MAX_MEAS_NUM_V)<<(APB_SARADC_MAX_MEAS_NUM_S))
#define APB_SARADC_MAX_MEAS_NUM_V  0xFF
#define APB_SARADC_MAX_MEAS_NUM_S  1
/* APB_SARADC_MEAS_NUM_LIMIT : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define APB_SARADC_MEAS_NUM_LIMIT  (BIT(0))
#define APB_SARADC_MEAS_NUM_LIMIT_M  (BIT(0))
#define APB_SARADC_MEAS_NUM_LIMIT_V  0x1
#define APB_SARADC_MEAS_NUM_LIMIT_S  0

#define APB_SARADC_FSM_REG          (DR_REG_APB_SARADC_BASE + 0x008)
/* APB_SARADC_SAMPLE_CYCLE : R/W ;bitpos:[31:24] ;default: 8'd2 ; */
/*description: sample cycles*/
#define APB_SARADC_SAMPLE_CYCLE  0x000000FF
#define APB_SARADC_SAMPLE_CYCLE_M  ((APB_SARADC_SAMPLE_CYCLE_V)<<(APB_SARADC_SAMPLE_CYCLE_S))
#define APB_SARADC_SAMPLE_CYCLE_V  0xFF
#define APB_SARADC_SAMPLE_CYCLE_S  24
/* APB_SARADC_SAMPLE_NUM : R/W ;bitpos:[23:16] ;default: 8'd0 ; */
/*description: sample number*/
#define APB_SARADC_SAMPLE_NUM  0x000000FF
#define APB_SARADC_SAMPLE_NUM_M  ((APB_SARADC_SAMPLE_NUM_V)<<(APB_SARADC_SAMPLE_NUM_S))
#define APB_SARADC_SAMPLE_NUM_V  0xFF
#define APB_SARADC_SAMPLE_NUM_S  16

#define APB_SARADC_FSM_WAIT_REG          (DR_REG_APB_SARADC_BASE + 0x00C)
/* APB_SARADC_STANDBY_WAIT : R/W ;bitpos:[23:16] ;default: 8'd255 ; */
/*description: */
#define APB_SARADC_STANDBY_WAIT  0x000000FF
#define APB_SARADC_STANDBY_WAIT_M  ((APB_SARADC_STANDBY_WAIT_V)<<(APB_SARADC_STANDBY_WAIT_S))
#define APB_SARADC_STANDBY_WAIT_V  0xFF
#define APB_SARADC_STANDBY_WAIT_S  16
/* APB_SARADC_RSTB_WAIT : R/W ;bitpos:[15:8] ;default: 8'd8 ; */
/*description: */
#define APB_SARADC_RSTB_WAIT  0x000000FF
#define APB_SARADC_RSTB_WAIT_M  ((APB_SARADC_RSTB_WAIT_V)<<(APB_SARADC_RSTB_WAIT_S))
#define APB_SARADC_RSTB_WAIT_V  0xFF
#define APB_SARADC_RSTB_WAIT_S  8
/* APB_SARADC_XPD_WAIT : R/W ;bitpos:[7:0] ;default: 8'd8 ; */
/*description: */
#define APB_SARADC_XPD_WAIT  0x000000FF
#define APB_SARADC_XPD_WAIT_M  ((APB_SARADC_XPD_WAIT_V)<<(APB_SARADC_XPD_WAIT_S))
#define APB_SARADC_XPD_WAIT_V  0xFF
#define APB_SARADC_XPD_WAIT_S  0

#define APB_SARADC_SAR1_STATUS_REG          (DR_REG_APB_SARADC_BASE + 0x010)
/* APB_SARADC_SAR1_STATUS : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define APB_SARADC_SAR1_STATUS  0xFFFFFFFF
#define APB_SARADC_SAR1_STATUS_M  ((APB_SARADC_SAR1_STATUS_V)<<(APB_SARADC_SAR1_STATUS_S))
#define APB_SARADC_SAR1_STATUS_V  0xFFFFFFFF
#define APB_SARADC_SAR1_STATUS_S  0

#define APB_SARADC_SAR2_STATUS_REG          (DR_REG_APB_SARADC_BASE + 0x014)
/* APB_SARADC_SAR2_STATUS : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define APB_SARADC_SAR2_STATUS  0xFFFFFFFF
#define APB_SARADC_SAR2_STATUS_M  ((APB_SARADC_SAR2_STATUS_V)<<(APB_SARADC_SAR2_STATUS_S))
#define APB_SARADC_SAR2_STATUS_V  0xFFFFFFFF
#define APB_SARADC_SAR2_STATUS_S  0

#define APB_SARADC_SAR1_PATT_TAB1_REG          (DR_REG_APB_SARADC_BASE + 0x018)
/* APB_SARADC_SAR1_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 1 (each item one byte)*/
#define APB_SARADC_SAR1_PATT_TAB1  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB1_M  ((APB_SARADC_SAR1_PATT_TAB1_V)<<(APB_SARADC_SAR1_PATT_TAB1_S))
#define APB_SARADC_SAR1_PATT_TAB1_V  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB1_S  0

#define APB_SARADC_SAR1_PATT_TAB2_REG          (DR_REG_APB_SARADC_BASE + 0x01C)
/* APB_SARADC_SAR1_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 1 (each item one byte)*/
#define APB_SARADC_SAR1_PATT_TAB2  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB2_M  ((APB_SARADC_SAR1_PATT_TAB2_V)<<(APB_SARADC_SAR1_PATT_TAB2_S))
#define APB_SARADC_SAR1_PATT_TAB2_V  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB2_S  0

#define APB_SARADC_SAR1_PATT_TAB3_REG          (DR_REG_APB_SARADC_BASE + 0x020)
/* APB_SARADC_SAR1_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 1 (each item one byte)*/
#define APB_SARADC_SAR1_PATT_TAB3  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB3_M  ((APB_SARADC_SAR1_PATT_TAB3_V)<<(APB_SARADC_SAR1_PATT_TAB3_S))
#define APB_SARADC_SAR1_PATT_TAB3_V  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB3_S  0

#define APB_SARADC_SAR1_PATT_TAB4_REG          (DR_REG_APB_SARADC_BASE + 0x024)
/* APB_SARADC_SAR1_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 1 (each item one byte)*/
#define APB_SARADC_SAR1_PATT_TAB4  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB4_M  ((APB_SARADC_SAR1_PATT_TAB4_V)<<(APB_SARADC_SAR1_PATT_TAB4_S))
#define APB_SARADC_SAR1_PATT_TAB4_V  0xFFFFFFFF
#define APB_SARADC_SAR1_PATT_TAB4_S  0

#define APB_SARADC_SAR2_PATT_TAB1_REG          (DR_REG_APB_SARADC_BASE + 0x028)
/* APB_SARADC_SAR2_PATT_TAB1 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: item 0 ~ 3 for pattern table 2 (each item one byte)*/
#define APB_SARADC_SAR2_PATT_TAB1  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB1_M  ((APB_SARADC_SAR2_PATT_TAB1_V)<<(APB_SARADC_SAR2_PATT_TAB1_S))
#define APB_SARADC_SAR2_PATT_TAB1_V  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB1_S  0

#define APB_SARADC_SAR2_PATT_TAB2_REG          (DR_REG_APB_SARADC_BASE + 0x02C)
/* APB_SARADC_SAR2_PATT_TAB2 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 4 ~ 7 for pattern table 2 (each item one byte)*/
#define APB_SARADC_SAR2_PATT_TAB2  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB2_M  ((APB_SARADC_SAR2_PATT_TAB2_V)<<(APB_SARADC_SAR2_PATT_TAB2_S))
#define APB_SARADC_SAR2_PATT_TAB2_V  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB2_S  0

#define APB_SARADC_SAR2_PATT_TAB3_REG          (DR_REG_APB_SARADC_BASE + 0x030)
/* APB_SARADC_SAR2_PATT_TAB3 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 8 ~ 11 for pattern table 2 (each item one byte)*/
#define APB_SARADC_SAR2_PATT_TAB3  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB3_M  ((APB_SARADC_SAR2_PATT_TAB3_V)<<(APB_SARADC_SAR2_PATT_TAB3_S))
#define APB_SARADC_SAR2_PATT_TAB3_V  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB3_S  0

#define APB_SARADC_SAR2_PATT_TAB4_REG          (DR_REG_APB_SARADC_BASE + 0x034)
/* APB_SARADC_SAR2_PATT_TAB4 : R/W ;bitpos:[31:0] ;default: 32'hf0f0f0f ; */
/*description: Item 12 ~ 15 for pattern table 2 (each item one byte)*/
#define APB_SARADC_SAR2_PATT_TAB4  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB4_M  ((APB_SARADC_SAR2_PATT_TAB4_V)<<(APB_SARADC_SAR2_PATT_TAB4_S))
#define APB_SARADC_SAR2_PATT_TAB4_V  0xFFFFFFFF
#define APB_SARADC_SAR2_PATT_TAB4_S  0

#define APB_SARADC_APB_ADC_ARB_CTRL_REG          (DR_REG_APB_SARADC_BASE + 0x038)
/* APB_SARADC_ADC_ARB_FIX_PRIORITY : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: adc2 arbiter uses fixed priority*/
#define APB_SARADC_ADC_ARB_FIX_PRIORITY  (BIT(12))
#define APB_SARADC_ADC_ARB_FIX_PRIORITY_M  (BIT(12))
#define APB_SARADC_ADC_ARB_FIX_PRIORITY_V  0x1
#define APB_SARADC_ADC_ARB_FIX_PRIORITY_S  12
/* APB_SARADC_ADC_ARB_WIFI_PRIORITY : R/W ;bitpos:[11:10] ;default: 2'd2 ; */
/*description: Set adc2 arbiter wifi priority*/
#define APB_SARADC_ADC_ARB_WIFI_PRIORITY  0x00000003
#define APB_SARADC_ADC_ARB_WIFI_PRIORITY_M  ((APB_SARADC_ADC_ARB_WIFI_PRIORITY_V)<<(APB_SARADC_ADC_ARB_WIFI_PRIORITY_S))
#define APB_SARADC_ADC_ARB_WIFI_PRIORITY_V  0x3
#define APB_SARADC_ADC_ARB_WIFI_PRIORITY_S  10
/* APB_SARADC_ADC_ARB_RTC_PRIORITY : R/W ;bitpos:[9:8] ;default: 2'd1 ; */
/*description: Set adc2 arbiter rtc priority*/
#define APB_SARADC_ADC_ARB_RTC_PRIORITY  0x00000003
#define APB_SARADC_ADC_ARB_RTC_PRIORITY_M  ((APB_SARADC_ADC_ARB_RTC_PRIORITY_V)<<(APB_SARADC_ADC_ARB_RTC_PRIORITY_S))
#define APB_SARADC_ADC_ARB_RTC_PRIORITY_V  0x3
#define APB_SARADC_ADC_ARB_RTC_PRIORITY_S  8
/* APB_SARADC_ADC_ARB_APB_PRIORITY : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Set adc2 arbiterapb priority*/
#define APB_SARADC_ADC_ARB_APB_PRIORITY  0x00000003
#define APB_SARADC_ADC_ARB_APB_PRIORITY_M  ((APB_SARADC_ADC_ARB_APB_PRIORITY_V)<<(APB_SARADC_ADC_ARB_APB_PRIORITY_S))
#define APB_SARADC_ADC_ARB_APB_PRIORITY_V  0x3
#define APB_SARADC_ADC_ARB_APB_PRIORITY_S  6
/* APB_SARADC_ADC_ARB_GRANT_FORCE : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: adc2 arbiter force grant*/
#define APB_SARADC_ADC_ARB_GRANT_FORCE  (BIT(5))
#define APB_SARADC_ADC_ARB_GRANT_FORCE_M  (BIT(5))
#define APB_SARADC_ADC_ARB_GRANT_FORCE_V  0x1
#define APB_SARADC_ADC_ARB_GRANT_FORCE_S  5
/* APB_SARADC_ADC_ARB_WIFI_FORCE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enable wifi controller*/
#define APB_SARADC_ADC_ARB_WIFI_FORCE  (BIT(4))
#define APB_SARADC_ADC_ARB_WIFI_FORCE_M  (BIT(4))
#define APB_SARADC_ADC_ARB_WIFI_FORCE_V  0x1
#define APB_SARADC_ADC_ARB_WIFI_FORCE_S  4
/* APB_SARADC_ADC_ARB_RTC_FORCE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enable rtc controller*/
#define APB_SARADC_ADC_ARB_RTC_FORCE  (BIT(3))
#define APB_SARADC_ADC_ARB_RTC_FORCE_M  (BIT(3))
#define APB_SARADC_ADC_ARB_RTC_FORCE_V  0x1
#define APB_SARADC_ADC_ARB_RTC_FORCE_S  3
/* APB_SARADC_ADC_ARB_APB_FORCE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: adc2 arbiter force to enableapb controller*/
#define APB_SARADC_ADC_ARB_APB_FORCE  (BIT(2))
#define APB_SARADC_ADC_ARB_APB_FORCE_M  (BIT(2))
#define APB_SARADC_ADC_ARB_APB_FORCE_V  0x1
#define APB_SARADC_ADC_ARB_APB_FORCE_S  2

#define APB_SARADC_FILTER_CTRL_REG          (DR_REG_APB_SARADC_BASE + 0x03C)
/* APB_SARADC_ADC1_FILTER_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: enable apb_adc1_filter*/
#define APB_SARADC_ADC1_FILTER_EN  (BIT(31))
#define APB_SARADC_ADC1_FILTER_EN_M  (BIT(31))
#define APB_SARADC_ADC1_FILTER_EN_V  0x1
#define APB_SARADC_ADC1_FILTER_EN_S  31
/* APB_SARADC_ADC2_FILTER_EN : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: enable apb_adc2_filter*/
#define APB_SARADC_ADC2_FILTER_EN  (BIT(30))
#define APB_SARADC_ADC2_FILTER_EN_M  (BIT(30))
#define APB_SARADC_ADC2_FILTER_EN_V  0x1
#define APB_SARADC_ADC2_FILTER_EN_S  30
/* APB_SARADC_ADC1_FILTER_FACTOR : R/W ;bitpos:[29:23] ;default: 7'd64 ; */
/*description: apb_adc1_filter_factor*/
#define APB_SARADC_ADC1_FILTER_FACTOR  0x0000007F
#define APB_SARADC_ADC1_FILTER_FACTOR_M  ((APB_SARADC_ADC1_FILTER_FACTOR_V)<<(APB_SARADC_ADC1_FILTER_FACTOR_S))
#define APB_SARADC_ADC1_FILTER_FACTOR_V  0x7F
#define APB_SARADC_ADC1_FILTER_FACTOR_S  23
/* APB_SARADC_ADC2_FILTER_FACTOR : R/W ;bitpos:[22:16] ;default: 7'd64 ; */
/*description: apb_adc2_filter_factor*/
#define APB_SARADC_ADC2_FILTER_FACTOR  0x0000007F
#define APB_SARADC_ADC2_FILTER_FACTOR_M  ((APB_SARADC_ADC2_FILTER_FACTOR_V)<<(APB_SARADC_ADC2_FILTER_FACTOR_S))
#define APB_SARADC_ADC2_FILTER_FACTOR_V  0x7F
#define APB_SARADC_ADC2_FILTER_FACTOR_S  16
/* APB_SARADC_ADC1_FILTER_RESET : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reset_adc1_filter*/
#define APB_SARADC_ADC1_FILTER_RESET  (BIT(1))
#define APB_SARADC_ADC1_FILTER_RESET_M  (BIT(1))
#define APB_SARADC_ADC1_FILTER_RESET_V  0x1
#define APB_SARADC_ADC1_FILTER_RESET_S  1
/* APB_SARADC_ADC2_FILTER_RESET : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: reset_adc2_filter*/
#define APB_SARADC_ADC2_FILTER_RESET  (BIT(0))
#define APB_SARADC_ADC2_FILTER_RESET_M  (BIT(0))
#define APB_SARADC_ADC2_FILTER_RESET_V  0x1
#define APB_SARADC_ADC2_FILTER_RESET_S  0

#define APB_SARADC_FILTER_STATUS_REG          (DR_REG_APB_SARADC_BASE + 0x040)
/* APB_SARADC_ADC1_FILTER_DATA : RO ;bitpos:[31:16] ;default: 16'd0 ; */
/*description: */
#define APB_SARADC_ADC1_FILTER_DATA  0x0000FFFF
#define APB_SARADC_ADC1_FILTER_DATA_M  ((APB_SARADC_ADC1_FILTER_DATA_V)<<(APB_SARADC_ADC1_FILTER_DATA_S))
#define APB_SARADC_ADC1_FILTER_DATA_V  0xFFFF
#define APB_SARADC_ADC1_FILTER_DATA_S  16
/* APB_SARADC_ADC2_FILTER_DATA : RO ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define APB_SARADC_ADC2_FILTER_DATA  0x0000FFFF
#define APB_SARADC_ADC2_FILTER_DATA_M  ((APB_SARADC_ADC2_FILTER_DATA_V)<<(APB_SARADC_ADC2_FILTER_DATA_S))
#define APB_SARADC_ADC2_FILTER_DATA_V  0xFFFF
#define APB_SARADC_ADC2_FILTER_DATA_S  0

#define APB_SARADC_THRES_CTRL_REG          (DR_REG_APB_SARADC_BASE + 0x044)
/* APB_SARADC_ADC1_THRES_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_THRES_EN  (BIT(31))
#define APB_SARADC_ADC1_THRES_EN_M  (BIT(31))
#define APB_SARADC_ADC1_THRES_EN_V  0x1
#define APB_SARADC_ADC1_THRES_EN_S  31
/* APB_SARADC_ADC2_THRES_EN : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_THRES_EN  (BIT(30))
#define APB_SARADC_ADC2_THRES_EN_M  (BIT(30))
#define APB_SARADC_ADC2_THRES_EN_V  0x1
#define APB_SARADC_ADC2_THRES_EN_S  30
/* APB_SARADC_ADC1_THRES : R/W ;bitpos:[29:17] ;default: 13'd0 ; */
/*description: */
#define APB_SARADC_ADC1_THRES  0x00001FFF
#define APB_SARADC_ADC1_THRES_M  ((APB_SARADC_ADC1_THRES_V)<<(APB_SARADC_ADC1_THRES_S))
#define APB_SARADC_ADC1_THRES_V  0x1FFF
#define APB_SARADC_ADC1_THRES_S  17
/* APB_SARADC_ADC2_THRES : R/W ;bitpos:[16:4] ;default: 13'd0 ; */
/*description: */
#define APB_SARADC_ADC2_THRES  0x00001FFF
#define APB_SARADC_ADC2_THRES_M  ((APB_SARADC_ADC2_THRES_V)<<(APB_SARADC_ADC2_THRES_S))
#define APB_SARADC_ADC2_THRES_V  0x1FFF
#define APB_SARADC_ADC2_THRES_S  4
/* APB_SARADC_ADC1_THRES_MODE : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: */
#define APB_SARADC_ADC1_THRES_MODE  (BIT(3))
#define APB_SARADC_ADC1_THRES_MODE_M  (BIT(3))
#define APB_SARADC_ADC1_THRES_MODE_V  0x1
#define APB_SARADC_ADC1_THRES_MODE_S  3
/* APB_SARADC_ADC2_THRES_MODE : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define APB_SARADC_ADC2_THRES_MODE  (BIT(2))
#define APB_SARADC_ADC2_THRES_MODE_M  (BIT(2))
#define APB_SARADC_ADC2_THRES_MODE_V  0x1
#define APB_SARADC_ADC2_THRES_MODE_S  2
/* APB_SARADC_CLK_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_CLK_EN  (BIT(0))
#define APB_SARADC_CLK_EN_M  (BIT(0))
#define APB_SARADC_CLK_EN_V  0x1
#define APB_SARADC_CLK_EN_S  0

#define APB_SARADC_INT_ENA_REG          (DR_REG_APB_SARADC_BASE + 0x048)
/* APB_SARADC_ADC1_DONE_INT_ENA : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_DONE_INT_ENA  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_ENA_M  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_ENA_V  0x1
#define APB_SARADC_ADC1_DONE_INT_ENA_S  31
/* APB_SARADC_ADC2_DONE_INT_ENA : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_DONE_INT_ENA  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_ENA_M  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_ENA_V  0x1
#define APB_SARADC_ADC2_DONE_INT_ENA_S  30
/* APB_SARADC_ADC1_THRES_INT_ENA : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_THRES_INT_ENA  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_ENA_M  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_ENA_V  0x1
#define APB_SARADC_ADC1_THRES_INT_ENA_S  29
/* APB_SARADC_ADC2_THRES_INT_ENA : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_THRES_INT_ENA  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_ENA_M  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_ENA_V  0x1
#define APB_SARADC_ADC2_THRES_INT_ENA_S  28

#define APB_SARADC_INT_RAW_REG          (DR_REG_APB_SARADC_BASE + 0x04C)
/* APB_SARADC_ADC1_DONE_INT_RAW : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_DONE_INT_RAW  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_RAW_M  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_RAW_V  0x1
#define APB_SARADC_ADC1_DONE_INT_RAW_S  31
/* APB_SARADC_ADC2_DONE_INT_RAW : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_DONE_INT_RAW  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_RAW_M  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_RAW_V  0x1
#define APB_SARADC_ADC2_DONE_INT_RAW_S  30
/* APB_SARADC_ADC1_THRES_INT_RAW : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_THRES_INT_RAW  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_RAW_M  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_RAW_V  0x1
#define APB_SARADC_ADC1_THRES_INT_RAW_S  29
/* APB_SARADC_ADC2_THRES_INT_RAW : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_THRES_INT_RAW  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_RAW_M  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_RAW_V  0x1
#define APB_SARADC_ADC2_THRES_INT_RAW_S  28

#define APB_SARADC_INT_ST_REG          (DR_REG_APB_SARADC_BASE + 0x050)
/* APB_SARADC_ADC1_DONE_INT_ST : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_DONE_INT_ST  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_ST_M  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_ST_V  0x1
#define APB_SARADC_ADC1_DONE_INT_ST_S  31
/* APB_SARADC_ADC2_DONE_INT_ST : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_DONE_INT_ST  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_ST_M  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_ST_V  0x1
#define APB_SARADC_ADC2_DONE_INT_ST_S  30
/* APB_SARADC_ADC1_THRES_INT_ST : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_THRES_INT_ST  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_ST_M  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_ST_V  0x1
#define APB_SARADC_ADC1_THRES_INT_ST_S  29
/* APB_SARADC_ADC2_THRES_INT_ST : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_THRES_INT_ST  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_ST_M  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_ST_V  0x1
#define APB_SARADC_ADC2_THRES_INT_ST_S  28

#define APB_SARADC_INT_CLR_REG          (DR_REG_APB_SARADC_BASE + 0x054)
/* APB_SARADC_ADC1_DONE_INT_CLR : WO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_DONE_INT_CLR  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_CLR_M  (BIT(31))
#define APB_SARADC_ADC1_DONE_INT_CLR_V  0x1
#define APB_SARADC_ADC1_DONE_INT_CLR_S  31
/* APB_SARADC_ADC2_DONE_INT_CLR : WO ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_DONE_INT_CLR  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_CLR_M  (BIT(30))
#define APB_SARADC_ADC2_DONE_INT_CLR_V  0x1
#define APB_SARADC_ADC2_DONE_INT_CLR_S  30
/* APB_SARADC_ADC1_THRES_INT_CLR : WO ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC1_THRES_INT_CLR  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_CLR_M  (BIT(29))
#define APB_SARADC_ADC1_THRES_INT_CLR_V  0x1
#define APB_SARADC_ADC1_THRES_INT_CLR_S  29
/* APB_SARADC_ADC2_THRES_INT_CLR : WO ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_ADC2_THRES_INT_CLR  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_CLR_M  (BIT(28))
#define APB_SARADC_ADC2_THRES_INT_CLR_V  0x1
#define APB_SARADC_ADC2_THRES_INT_CLR_S  28

#define APB_SARADC_DMA_CONF_REG          (DR_REG_APB_SARADC_BASE + 0x058)
/* APB_SARADC_APB_ADC_TRANS : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: enable apb_adc use spi_dma*/
#define APB_SARADC_APB_ADC_TRANS  (BIT(31))
#define APB_SARADC_APB_ADC_TRANS_M  (BIT(31))
#define APB_SARADC_APB_ADC_TRANS_V  0x1
#define APB_SARADC_APB_ADC_TRANS_S  31
/* APB_SARADC_APB_ADC_RESET_FSM : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: reset_apb_adc_state*/
#define APB_SARADC_APB_ADC_RESET_FSM  (BIT(30))
#define APB_SARADC_APB_ADC_RESET_FSM_M  (BIT(30))
#define APB_SARADC_APB_ADC_RESET_FSM_V  0x1
#define APB_SARADC_APB_ADC_RESET_FSM_S  30
/* APB_SARADC_APB_ADC_EOF_NUM : R/W ;bitpos:[15:0] ;default: 16'd255 ; */
/*description: the dma_in_suc_eof gen when sample cnt = spi_eof_num*/
#define APB_SARADC_APB_ADC_EOF_NUM  0x0000FFFF
#define APB_SARADC_APB_ADC_EOF_NUM_M  ((APB_SARADC_APB_ADC_EOF_NUM_V)<<(APB_SARADC_APB_ADC_EOF_NUM_S))
#define APB_SARADC_APB_ADC_EOF_NUM_V  0xFFFF
#define APB_SARADC_APB_ADC_EOF_NUM_S  0

#define APB_SARADC_APB_ADC_CLKM_CONF_REG          (DR_REG_APB_SARADC_BASE + 0x05c)
/* APB_SARADC_CLK_SEL : R/W ;bitpos:[22:21] ;default: 2'b0 ; */
/*description: Set this bit to enable clk_apll*/
#define APB_SARADC_CLK_SEL  0x00000003
#define APB_SARADC_CLK_SEL_M  ((APB_SARADC_CLK_SEL_V)<<(APB_SARADC_CLK_SEL_S))
#define APB_SARADC_CLK_SEL_V  0x3
#define APB_SARADC_CLK_SEL_S  21
/* APB_SARADC_CLKM_DIV_A : R/W ;bitpos:[19:14] ;default: 6'h0 ; */
/*description: Fractional clock divider denominator value*/
#define APB_SARADC_CLKM_DIV_A  0x0000003F
#define APB_SARADC_CLKM_DIV_A_M  ((APB_SARADC_CLKM_DIV_A_V)<<(APB_SARADC_CLKM_DIV_A_S))
#define APB_SARADC_CLKM_DIV_A_V  0x3F
#define APB_SARADC_CLKM_DIV_A_S  14
/* APB_SARADC_CLKM_DIV_B : R/W ;bitpos:[13:8] ;default: 6'h0 ; */
/*description: Fractional clock divider numerator value*/
#define APB_SARADC_CLKM_DIV_B  0x0000003F
#define APB_SARADC_CLKM_DIV_B_M  ((APB_SARADC_CLKM_DIV_B_V)<<(APB_SARADC_CLKM_DIV_B_S))
#define APB_SARADC_CLKM_DIV_B_V  0x3F
#define APB_SARADC_CLKM_DIV_B_S  8
/* APB_SARADC_CLKM_DIV_NUM : R/W ;bitpos:[7:0] ;default: 8'd4 ; */
/*description: Integral I2S clock divider value*/
#define APB_SARADC_CLKM_DIV_NUM  0x000000FF
#define APB_SARADC_CLKM_DIV_NUM_M  ((APB_SARADC_CLKM_DIV_NUM_V)<<(APB_SARADC_CLKM_DIV_NUM_S))
#define APB_SARADC_CLKM_DIV_NUM_V  0xFF
#define APB_SARADC_CLKM_DIV_NUM_S  0

#define APB_SARADC_APB_DAC_CTRL_REG          (DR_REG_APB_SARADC_BASE + 0x060)
/* APB_SARADC_APB_DAC_RST : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_APB_DAC_RST  (BIT(16))
#define APB_SARADC_APB_DAC_RST_M  (BIT(16))
#define APB_SARADC_APB_DAC_RST_V  0x1
#define APB_SARADC_APB_DAC_RST_S  16
/* APB_SARADC_DAC_RESET_FIFO : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define APB_SARADC_DAC_RESET_FIFO  (BIT(15))
#define APB_SARADC_DAC_RESET_FIFO_M  (BIT(15))
#define APB_SARADC_DAC_RESET_FIFO_V  0x1
#define APB_SARADC_DAC_RESET_FIFO_S  15
/* APB_SARADC_APB_DAC_TRANS : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: enable dma_dac*/
#define APB_SARADC_APB_DAC_TRANS  (BIT(14))
#define APB_SARADC_APB_DAC_TRANS_M  (BIT(14))
#define APB_SARADC_APB_DAC_TRANS_V  0x1
#define APB_SARADC_APB_DAC_TRANS_S  14
/* APB_SARADC_APB_DAC_ALTER_MODE : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: enable dac alter mode*/
#define APB_SARADC_APB_DAC_ALTER_MODE  (BIT(13))
#define APB_SARADC_APB_DAC_ALTER_MODE_M  (BIT(13))
#define APB_SARADC_APB_DAC_ALTER_MODE_V  0x1
#define APB_SARADC_APB_DAC_ALTER_MODE_S  13
/* APB_SARADC_DAC_TIMER_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: enable read dac data*/
#define APB_SARADC_DAC_TIMER_EN  (BIT(12))
#define APB_SARADC_DAC_TIMER_EN_M  (BIT(12))
#define APB_SARADC_DAC_TIMER_EN_V  0x1
#define APB_SARADC_DAC_TIMER_EN_S  12
/* APB_SARADC_DAC_TIMER_TARGET : R/W ;bitpos:[11:0] ;default: 12'd100 ; */
/*description: dac_timer target*/
#define APB_SARADC_DAC_TIMER_TARGET  0x00000FFF
#define APB_SARADC_DAC_TIMER_TARGET_M  ((APB_SARADC_DAC_TIMER_TARGET_V)<<(APB_SARADC_DAC_TIMER_TARGET_S))
#define APB_SARADC_DAC_TIMER_TARGET_V  0xFFF
#define APB_SARADC_DAC_TIMER_TARGET_S  0

#define APB_SARADC_APB_CTRL_DATE_REG          (DR_REG_APB_SARADC_BASE + 0x3FC)
/* APB_SARADC_APB_CTRL_DATE : R/W ;bitpos:[31:0] ;default: 32'h01907162 ; */
/*description: */
#define APB_SARADC_APB_CTRL_DATE  0xFFFFFFFF
#define APB_SARADC_APB_CTRL_DATE_M  ((APB_SARADC_APB_CTRL_DATE_V)<<(APB_SARADC_APB_CTRL_DATE_S))
#define APB_SARADC_APB_CTRL_DATE_V  0xFFFFFFFF
#define APB_SARADC_APB_CTRL_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_APB_SARADC_REG_H_ */


