// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_PCNT_REG_H_
#define _SOC_PCNT_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define PCNT_U0_CONF0_REG          (DR_REG_PCNT_BASE + 0x0000)
/* PCNT_CH1_LCTRL_MODE_U0 : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_LCTRL_MODE_U0  0x00000003
#define PCNT_CH1_LCTRL_MODE_U0_M  ((PCNT_CH1_LCTRL_MODE_U0_V)<<(PCNT_CH1_LCTRL_MODE_U0_S))
#define PCNT_CH1_LCTRL_MODE_U0_V  0x3
#define PCNT_CH1_LCTRL_MODE_U0_S  30
/* PCNT_CH1_HCTRL_MODE_U0 : R/W ;bitpos:[29:28] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_HCTRL_MODE_U0  0x00000003
#define PCNT_CH1_HCTRL_MODE_U0_M  ((PCNT_CH1_HCTRL_MODE_U0_V)<<(PCNT_CH1_HCTRL_MODE_U0_S))
#define PCNT_CH1_HCTRL_MODE_U0_V  0x3
#define PCNT_CH1_HCTRL_MODE_U0_S  28
/* PCNT_CH1_POS_MODE_U0 : R/W ;bitpos:[27:26] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_POS_MODE_U0  0x00000003
#define PCNT_CH1_POS_MODE_U0_M  ((PCNT_CH1_POS_MODE_U0_V)<<(PCNT_CH1_POS_MODE_U0_S))
#define PCNT_CH1_POS_MODE_U0_V  0x3
#define PCNT_CH1_POS_MODE_U0_S  26
/* PCNT_CH1_NEG_MODE_U0 : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_NEG_MODE_U0  0x00000003
#define PCNT_CH1_NEG_MODE_U0_M  ((PCNT_CH1_NEG_MODE_U0_V)<<(PCNT_CH1_NEG_MODE_U0_S))
#define PCNT_CH1_NEG_MODE_U0_V  0x3
#define PCNT_CH1_NEG_MODE_U0_S  24
/* PCNT_CH0_LCTRL_MODE_U0 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_LCTRL_MODE_U0  0x00000003
#define PCNT_CH0_LCTRL_MODE_U0_M  ((PCNT_CH0_LCTRL_MODE_U0_V)<<(PCNT_CH0_LCTRL_MODE_U0_S))
#define PCNT_CH0_LCTRL_MODE_U0_V  0x3
#define PCNT_CH0_LCTRL_MODE_U0_S  22
/* PCNT_CH0_HCTRL_MODE_U0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_HCTRL_MODE_U0  0x00000003
#define PCNT_CH0_HCTRL_MODE_U0_M  ((PCNT_CH0_HCTRL_MODE_U0_V)<<(PCNT_CH0_HCTRL_MODE_U0_S))
#define PCNT_CH0_HCTRL_MODE_U0_V  0x3
#define PCNT_CH0_HCTRL_MODE_U0_S  20
/* PCNT_CH0_POS_MODE_U0 : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_POS_MODE_U0  0x00000003
#define PCNT_CH0_POS_MODE_U0_M  ((PCNT_CH0_POS_MODE_U0_V)<<(PCNT_CH0_POS_MODE_U0_S))
#define PCNT_CH0_POS_MODE_U0_V  0x3
#define PCNT_CH0_POS_MODE_U0_S  18
/* PCNT_CH0_NEG_MODE_U0 : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_NEG_MODE_U0  0x00000003
#define PCNT_CH0_NEG_MODE_U0_M  ((PCNT_CH0_NEG_MODE_U0_V)<<(PCNT_CH0_NEG_MODE_U0_S))
#define PCNT_CH0_NEG_MODE_U0_V  0x3
#define PCNT_CH0_NEG_MODE_U0_S  16
/* PCNT_THR_THRES1_EN_U0 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES1_EN_U0  (BIT(15))
#define PCNT_THR_THRES1_EN_U0_M  (BIT(15))
#define PCNT_THR_THRES1_EN_U0_V  0x1
#define PCNT_THR_THRES1_EN_U0_S  15
/* PCNT_THR_THRES0_EN_U0 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES0_EN_U0  (BIT(14))
#define PCNT_THR_THRES0_EN_U0_M  (BIT(14))
#define PCNT_THR_THRES0_EN_U0_V  0x1
#define PCNT_THR_THRES0_EN_U0_S  14
/* PCNT_THR_L_LIM_EN_U0 : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_L_LIM_EN_U0  (BIT(13))
#define PCNT_THR_L_LIM_EN_U0_M  (BIT(13))
#define PCNT_THR_L_LIM_EN_U0_V  0x1
#define PCNT_THR_L_LIM_EN_U0_S  13
/* PCNT_THR_H_LIM_EN_U0 : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_H_LIM_EN_U0  (BIT(12))
#define PCNT_THR_H_LIM_EN_U0_M  (BIT(12))
#define PCNT_THR_H_LIM_EN_U0_V  0x1
#define PCNT_THR_H_LIM_EN_U0_S  12
/* PCNT_THR_ZERO_EN_U0 : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_ZERO_EN_U0  (BIT(11))
#define PCNT_THR_ZERO_EN_U0_M  (BIT(11))
#define PCNT_THR_ZERO_EN_U0_V  0x1
#define PCNT_THR_ZERO_EN_U0_S  11
/* PCNT_FILTER_EN_U0 : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: */
#define PCNT_FILTER_EN_U0  (BIT(10))
#define PCNT_FILTER_EN_U0_M  (BIT(10))
#define PCNT_FILTER_EN_U0_V  0x1
#define PCNT_FILTER_EN_U0_S  10
/* PCNT_FILTER_THRES_U0 : R/W ;bitpos:[9:0] ;default: 10'h10 ; */
/*description: */
#define PCNT_FILTER_THRES_U0  0x000003FF
#define PCNT_FILTER_THRES_U0_M  ((PCNT_FILTER_THRES_U0_V)<<(PCNT_FILTER_THRES_U0_S))
#define PCNT_FILTER_THRES_U0_V  0x3FF
#define PCNT_FILTER_THRES_U0_S  0

#define PCNT_U0_CONF1_REG          (DR_REG_PCNT_BASE + 0x0004)
/* PCNT_CNT_THRES1_U0 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES1_U0  0x0000FFFF
#define PCNT_CNT_THRES1_U0_M  ((PCNT_CNT_THRES1_U0_V)<<(PCNT_CNT_THRES1_U0_S))
#define PCNT_CNT_THRES1_U0_V  0xFFFF
#define PCNT_CNT_THRES1_U0_S  16
/* PCNT_CNT_THRES0_U0 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES0_U0  0x0000FFFF
#define PCNT_CNT_THRES0_U0_M  ((PCNT_CNT_THRES0_U0_V)<<(PCNT_CNT_THRES0_U0_S))
#define PCNT_CNT_THRES0_U0_V  0xFFFF
#define PCNT_CNT_THRES0_U0_S  0

#define PCNT_U0_CONF2_REG          (DR_REG_PCNT_BASE + 0x0008)
/* PCNT_CNT_L_LIM_U0 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_L_LIM_U0  0x0000FFFF
#define PCNT_CNT_L_LIM_U0_M  ((PCNT_CNT_L_LIM_U0_V)<<(PCNT_CNT_L_LIM_U0_S))
#define PCNT_CNT_L_LIM_U0_V  0xFFFF
#define PCNT_CNT_L_LIM_U0_S  16
/* PCNT_CNT_H_LIM_U0 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_H_LIM_U0  0x0000FFFF
#define PCNT_CNT_H_LIM_U0_M  ((PCNT_CNT_H_LIM_U0_V)<<(PCNT_CNT_H_LIM_U0_S))
#define PCNT_CNT_H_LIM_U0_V  0xFFFF
#define PCNT_CNT_H_LIM_U0_S  0

#define PCNT_U1_CONF0_REG          (DR_REG_PCNT_BASE + 0x000c)
/* PCNT_CH1_LCTRL_MODE_U1 : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_LCTRL_MODE_U1  0x00000003
#define PCNT_CH1_LCTRL_MODE_U1_M  ((PCNT_CH1_LCTRL_MODE_U1_V)<<(PCNT_CH1_LCTRL_MODE_U1_S))
#define PCNT_CH1_LCTRL_MODE_U1_V  0x3
#define PCNT_CH1_LCTRL_MODE_U1_S  30
/* PCNT_CH1_HCTRL_MODE_U1 : R/W ;bitpos:[29:28] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_HCTRL_MODE_U1  0x00000003
#define PCNT_CH1_HCTRL_MODE_U1_M  ((PCNT_CH1_HCTRL_MODE_U1_V)<<(PCNT_CH1_HCTRL_MODE_U1_S))
#define PCNT_CH1_HCTRL_MODE_U1_V  0x3
#define PCNT_CH1_HCTRL_MODE_U1_S  28
/* PCNT_CH1_POS_MODE_U1 : R/W ;bitpos:[27:26] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_POS_MODE_U1  0x00000003
#define PCNT_CH1_POS_MODE_U1_M  ((PCNT_CH1_POS_MODE_U1_V)<<(PCNT_CH1_POS_MODE_U1_S))
#define PCNT_CH1_POS_MODE_U1_V  0x3
#define PCNT_CH1_POS_MODE_U1_S  26
/* PCNT_CH1_NEG_MODE_U1 : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_NEG_MODE_U1  0x00000003
#define PCNT_CH1_NEG_MODE_U1_M  ((PCNT_CH1_NEG_MODE_U1_V)<<(PCNT_CH1_NEG_MODE_U1_S))
#define PCNT_CH1_NEG_MODE_U1_V  0x3
#define PCNT_CH1_NEG_MODE_U1_S  24
/* PCNT_CH0_LCTRL_MODE_U1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_LCTRL_MODE_U1  0x00000003
#define PCNT_CH0_LCTRL_MODE_U1_M  ((PCNT_CH0_LCTRL_MODE_U1_V)<<(PCNT_CH0_LCTRL_MODE_U1_S))
#define PCNT_CH0_LCTRL_MODE_U1_V  0x3
#define PCNT_CH0_LCTRL_MODE_U1_S  22
/* PCNT_CH0_HCTRL_MODE_U1 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_HCTRL_MODE_U1  0x00000003
#define PCNT_CH0_HCTRL_MODE_U1_M  ((PCNT_CH0_HCTRL_MODE_U1_V)<<(PCNT_CH0_HCTRL_MODE_U1_S))
#define PCNT_CH0_HCTRL_MODE_U1_V  0x3
#define PCNT_CH0_HCTRL_MODE_U1_S  20
/* PCNT_CH0_POS_MODE_U1 : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_POS_MODE_U1  0x00000003
#define PCNT_CH0_POS_MODE_U1_M  ((PCNT_CH0_POS_MODE_U1_V)<<(PCNT_CH0_POS_MODE_U1_S))
#define PCNT_CH0_POS_MODE_U1_V  0x3
#define PCNT_CH0_POS_MODE_U1_S  18
/* PCNT_CH0_NEG_MODE_U1 : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_NEG_MODE_U1  0x00000003
#define PCNT_CH0_NEG_MODE_U1_M  ((PCNT_CH0_NEG_MODE_U1_V)<<(PCNT_CH0_NEG_MODE_U1_S))
#define PCNT_CH0_NEG_MODE_U1_V  0x3
#define PCNT_CH0_NEG_MODE_U1_S  16
/* PCNT_THR_THRES1_EN_U1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES1_EN_U1  (BIT(15))
#define PCNT_THR_THRES1_EN_U1_M  (BIT(15))
#define PCNT_THR_THRES1_EN_U1_V  0x1
#define PCNT_THR_THRES1_EN_U1_S  15
/* PCNT_THR_THRES0_EN_U1 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES0_EN_U1  (BIT(14))
#define PCNT_THR_THRES0_EN_U1_M  (BIT(14))
#define PCNT_THR_THRES0_EN_U1_V  0x1
#define PCNT_THR_THRES0_EN_U1_S  14
/* PCNT_THR_L_LIM_EN_U1 : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_L_LIM_EN_U1  (BIT(13))
#define PCNT_THR_L_LIM_EN_U1_M  (BIT(13))
#define PCNT_THR_L_LIM_EN_U1_V  0x1
#define PCNT_THR_L_LIM_EN_U1_S  13
/* PCNT_THR_H_LIM_EN_U1 : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_H_LIM_EN_U1  (BIT(12))
#define PCNT_THR_H_LIM_EN_U1_M  (BIT(12))
#define PCNT_THR_H_LIM_EN_U1_V  0x1
#define PCNT_THR_H_LIM_EN_U1_S  12
/* PCNT_THR_ZERO_EN_U1 : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_ZERO_EN_U1  (BIT(11))
#define PCNT_THR_ZERO_EN_U1_M  (BIT(11))
#define PCNT_THR_ZERO_EN_U1_V  0x1
#define PCNT_THR_ZERO_EN_U1_S  11
/* PCNT_FILTER_EN_U1 : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: */
#define PCNT_FILTER_EN_U1  (BIT(10))
#define PCNT_FILTER_EN_U1_M  (BIT(10))
#define PCNT_FILTER_EN_U1_V  0x1
#define PCNT_FILTER_EN_U1_S  10
/* PCNT_FILTER_THRES_U1 : R/W ;bitpos:[9:0] ;default: 10'h10 ; */
/*description: */
#define PCNT_FILTER_THRES_U1  0x000003FF
#define PCNT_FILTER_THRES_U1_M  ((PCNT_FILTER_THRES_U1_V)<<(PCNT_FILTER_THRES_U1_S))
#define PCNT_FILTER_THRES_U1_V  0x3FF
#define PCNT_FILTER_THRES_U1_S  0

#define PCNT_U1_CONF1_REG          (DR_REG_PCNT_BASE + 0x0010)
/* PCNT_CNT_THRES1_U1 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES1_U1  0x0000FFFF
#define PCNT_CNT_THRES1_U1_M  ((PCNT_CNT_THRES1_U1_V)<<(PCNT_CNT_THRES1_U1_S))
#define PCNT_CNT_THRES1_U1_V  0xFFFF
#define PCNT_CNT_THRES1_U1_S  16
/* PCNT_CNT_THRES0_U1 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES0_U1  0x0000FFFF
#define PCNT_CNT_THRES0_U1_M  ((PCNT_CNT_THRES0_U1_V)<<(PCNT_CNT_THRES0_U1_S))
#define PCNT_CNT_THRES0_U1_V  0xFFFF
#define PCNT_CNT_THRES0_U1_S  0

#define PCNT_U1_CONF2_REG          (DR_REG_PCNT_BASE + 0x0014)
/* PCNT_CNT_L_LIM_U1 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_L_LIM_U1  0x0000FFFF
#define PCNT_CNT_L_LIM_U1_M  ((PCNT_CNT_L_LIM_U1_V)<<(PCNT_CNT_L_LIM_U1_S))
#define PCNT_CNT_L_LIM_U1_V  0xFFFF
#define PCNT_CNT_L_LIM_U1_S  16
/* PCNT_CNT_H_LIM_U1 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_H_LIM_U1  0x0000FFFF
#define PCNT_CNT_H_LIM_U1_M  ((PCNT_CNT_H_LIM_U1_V)<<(PCNT_CNT_H_LIM_U1_S))
#define PCNT_CNT_H_LIM_U1_V  0xFFFF
#define PCNT_CNT_H_LIM_U1_S  0

#define PCNT_U2_CONF0_REG          (DR_REG_PCNT_BASE + 0x0018)
/* PCNT_CH1_LCTRL_MODE_U2 : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_LCTRL_MODE_U2  0x00000003
#define PCNT_CH1_LCTRL_MODE_U2_M  ((PCNT_CH1_LCTRL_MODE_U2_V)<<(PCNT_CH1_LCTRL_MODE_U2_S))
#define PCNT_CH1_LCTRL_MODE_U2_V  0x3
#define PCNT_CH1_LCTRL_MODE_U2_S  30
/* PCNT_CH1_HCTRL_MODE_U2 : R/W ;bitpos:[29:28] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_HCTRL_MODE_U2  0x00000003
#define PCNT_CH1_HCTRL_MODE_U2_M  ((PCNT_CH1_HCTRL_MODE_U2_V)<<(PCNT_CH1_HCTRL_MODE_U2_S))
#define PCNT_CH1_HCTRL_MODE_U2_V  0x3
#define PCNT_CH1_HCTRL_MODE_U2_S  28
/* PCNT_CH1_POS_MODE_U2 : R/W ;bitpos:[27:26] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_POS_MODE_U2  0x00000003
#define PCNT_CH1_POS_MODE_U2_M  ((PCNT_CH1_POS_MODE_U2_V)<<(PCNT_CH1_POS_MODE_U2_S))
#define PCNT_CH1_POS_MODE_U2_V  0x3
#define PCNT_CH1_POS_MODE_U2_S  26
/* PCNT_CH1_NEG_MODE_U2 : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_NEG_MODE_U2  0x00000003
#define PCNT_CH1_NEG_MODE_U2_M  ((PCNT_CH1_NEG_MODE_U2_V)<<(PCNT_CH1_NEG_MODE_U2_S))
#define PCNT_CH1_NEG_MODE_U2_V  0x3
#define PCNT_CH1_NEG_MODE_U2_S  24
/* PCNT_CH0_LCTRL_MODE_U2 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_LCTRL_MODE_U2  0x00000003
#define PCNT_CH0_LCTRL_MODE_U2_M  ((PCNT_CH0_LCTRL_MODE_U2_V)<<(PCNT_CH0_LCTRL_MODE_U2_S))
#define PCNT_CH0_LCTRL_MODE_U2_V  0x3
#define PCNT_CH0_LCTRL_MODE_U2_S  22
/* PCNT_CH0_HCTRL_MODE_U2 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_HCTRL_MODE_U2  0x00000003
#define PCNT_CH0_HCTRL_MODE_U2_M  ((PCNT_CH0_HCTRL_MODE_U2_V)<<(PCNT_CH0_HCTRL_MODE_U2_S))
#define PCNT_CH0_HCTRL_MODE_U2_V  0x3
#define PCNT_CH0_HCTRL_MODE_U2_S  20
/* PCNT_CH0_POS_MODE_U2 : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_POS_MODE_U2  0x00000003
#define PCNT_CH0_POS_MODE_U2_M  ((PCNT_CH0_POS_MODE_U2_V)<<(PCNT_CH0_POS_MODE_U2_S))
#define PCNT_CH0_POS_MODE_U2_V  0x3
#define PCNT_CH0_POS_MODE_U2_S  18
/* PCNT_CH0_NEG_MODE_U2 : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_NEG_MODE_U2  0x00000003
#define PCNT_CH0_NEG_MODE_U2_M  ((PCNT_CH0_NEG_MODE_U2_V)<<(PCNT_CH0_NEG_MODE_U2_S))
#define PCNT_CH0_NEG_MODE_U2_V  0x3
#define PCNT_CH0_NEG_MODE_U2_S  16
/* PCNT_THR_THRES1_EN_U2 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES1_EN_U2  (BIT(15))
#define PCNT_THR_THRES1_EN_U2_M  (BIT(15))
#define PCNT_THR_THRES1_EN_U2_V  0x1
#define PCNT_THR_THRES1_EN_U2_S  15
/* PCNT_THR_THRES0_EN_U2 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES0_EN_U2  (BIT(14))
#define PCNT_THR_THRES0_EN_U2_M  (BIT(14))
#define PCNT_THR_THRES0_EN_U2_V  0x1
#define PCNT_THR_THRES0_EN_U2_S  14
/* PCNT_THR_L_LIM_EN_U2 : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_L_LIM_EN_U2  (BIT(13))
#define PCNT_THR_L_LIM_EN_U2_M  (BIT(13))
#define PCNT_THR_L_LIM_EN_U2_V  0x1
#define PCNT_THR_L_LIM_EN_U2_S  13
/* PCNT_THR_H_LIM_EN_U2 : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_H_LIM_EN_U2  (BIT(12))
#define PCNT_THR_H_LIM_EN_U2_M  (BIT(12))
#define PCNT_THR_H_LIM_EN_U2_V  0x1
#define PCNT_THR_H_LIM_EN_U2_S  12
/* PCNT_THR_ZERO_EN_U2 : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_ZERO_EN_U2  (BIT(11))
#define PCNT_THR_ZERO_EN_U2_M  (BIT(11))
#define PCNT_THR_ZERO_EN_U2_V  0x1
#define PCNT_THR_ZERO_EN_U2_S  11
/* PCNT_FILTER_EN_U2 : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: */
#define PCNT_FILTER_EN_U2  (BIT(10))
#define PCNT_FILTER_EN_U2_M  (BIT(10))
#define PCNT_FILTER_EN_U2_V  0x1
#define PCNT_FILTER_EN_U2_S  10
/* PCNT_FILTER_THRES_U2 : R/W ;bitpos:[9:0] ;default: 10'h10 ; */
/*description: */
#define PCNT_FILTER_THRES_U2  0x000003FF
#define PCNT_FILTER_THRES_U2_M  ((PCNT_FILTER_THRES_U2_V)<<(PCNT_FILTER_THRES_U2_S))
#define PCNT_FILTER_THRES_U2_V  0x3FF
#define PCNT_FILTER_THRES_U2_S  0

#define PCNT_U2_CONF1_REG          (DR_REG_PCNT_BASE + 0x001c)
/* PCNT_CNT_THRES1_U2 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES1_U2  0x0000FFFF
#define PCNT_CNT_THRES1_U2_M  ((PCNT_CNT_THRES1_U2_V)<<(PCNT_CNT_THRES1_U2_S))
#define PCNT_CNT_THRES1_U2_V  0xFFFF
#define PCNT_CNT_THRES1_U2_S  16
/* PCNT_CNT_THRES0_U2 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES0_U2  0x0000FFFF
#define PCNT_CNT_THRES0_U2_M  ((PCNT_CNT_THRES0_U2_V)<<(PCNT_CNT_THRES0_U2_S))
#define PCNT_CNT_THRES0_U2_V  0xFFFF
#define PCNT_CNT_THRES0_U2_S  0

#define PCNT_U2_CONF2_REG          (DR_REG_PCNT_BASE + 0x0020)
/* PCNT_CNT_L_LIM_U2 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_L_LIM_U2  0x0000FFFF
#define PCNT_CNT_L_LIM_U2_M  ((PCNT_CNT_L_LIM_U2_V)<<(PCNT_CNT_L_LIM_U2_S))
#define PCNT_CNT_L_LIM_U2_V  0xFFFF
#define PCNT_CNT_L_LIM_U2_S  16
/* PCNT_CNT_H_LIM_U2 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_H_LIM_U2  0x0000FFFF
#define PCNT_CNT_H_LIM_U2_M  ((PCNT_CNT_H_LIM_U2_V)<<(PCNT_CNT_H_LIM_U2_S))
#define PCNT_CNT_H_LIM_U2_V  0xFFFF
#define PCNT_CNT_H_LIM_U2_S  0

#define PCNT_U3_CONF0_REG          (DR_REG_PCNT_BASE + 0x0024)
/* PCNT_CH1_LCTRL_MODE_U3 : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_LCTRL_MODE_U3  0x00000003
#define PCNT_CH1_LCTRL_MODE_U3_M  ((PCNT_CH1_LCTRL_MODE_U3_V)<<(PCNT_CH1_LCTRL_MODE_U3_S))
#define PCNT_CH1_LCTRL_MODE_U3_V  0x3
#define PCNT_CH1_LCTRL_MODE_U3_S  30
/* PCNT_CH1_HCTRL_MODE_U3 : R/W ;bitpos:[29:28] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_HCTRL_MODE_U3  0x00000003
#define PCNT_CH1_HCTRL_MODE_U3_M  ((PCNT_CH1_HCTRL_MODE_U3_V)<<(PCNT_CH1_HCTRL_MODE_U3_S))
#define PCNT_CH1_HCTRL_MODE_U3_V  0x3
#define PCNT_CH1_HCTRL_MODE_U3_S  28
/* PCNT_CH1_POS_MODE_U3 : R/W ;bitpos:[27:26] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_POS_MODE_U3  0x00000003
#define PCNT_CH1_POS_MODE_U3_M  ((PCNT_CH1_POS_MODE_U3_V)<<(PCNT_CH1_POS_MODE_U3_S))
#define PCNT_CH1_POS_MODE_U3_V  0x3
#define PCNT_CH1_POS_MODE_U3_S  26
/* PCNT_CH1_NEG_MODE_U3 : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH1_NEG_MODE_U3  0x00000003
#define PCNT_CH1_NEG_MODE_U3_M  ((PCNT_CH1_NEG_MODE_U3_V)<<(PCNT_CH1_NEG_MODE_U3_S))
#define PCNT_CH1_NEG_MODE_U3_V  0x3
#define PCNT_CH1_NEG_MODE_U3_S  24
/* PCNT_CH0_LCTRL_MODE_U3 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_LCTRL_MODE_U3  0x00000003
#define PCNT_CH0_LCTRL_MODE_U3_M  ((PCNT_CH0_LCTRL_MODE_U3_V)<<(PCNT_CH0_LCTRL_MODE_U3_S))
#define PCNT_CH0_LCTRL_MODE_U3_V  0x3
#define PCNT_CH0_LCTRL_MODE_U3_S  22
/* PCNT_CH0_HCTRL_MODE_U3 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_HCTRL_MODE_U3  0x00000003
#define PCNT_CH0_HCTRL_MODE_U3_M  ((PCNT_CH0_HCTRL_MODE_U3_V)<<(PCNT_CH0_HCTRL_MODE_U3_S))
#define PCNT_CH0_HCTRL_MODE_U3_V  0x3
#define PCNT_CH0_HCTRL_MODE_U3_S  20
/* PCNT_CH0_POS_MODE_U3 : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_POS_MODE_U3  0x00000003
#define PCNT_CH0_POS_MODE_U3_M  ((PCNT_CH0_POS_MODE_U3_V)<<(PCNT_CH0_POS_MODE_U3_S))
#define PCNT_CH0_POS_MODE_U3_V  0x3
#define PCNT_CH0_POS_MODE_U3_S  18
/* PCNT_CH0_NEG_MODE_U3 : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define PCNT_CH0_NEG_MODE_U3  0x00000003
#define PCNT_CH0_NEG_MODE_U3_M  ((PCNT_CH0_NEG_MODE_U3_V)<<(PCNT_CH0_NEG_MODE_U3_S))
#define PCNT_CH0_NEG_MODE_U3_V  0x3
#define PCNT_CH0_NEG_MODE_U3_S  16
/* PCNT_THR_THRES1_EN_U3 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES1_EN_U3  (BIT(15))
#define PCNT_THR_THRES1_EN_U3_M  (BIT(15))
#define PCNT_THR_THRES1_EN_U3_V  0x1
#define PCNT_THR_THRES1_EN_U3_S  15
/* PCNT_THR_THRES0_EN_U3 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define PCNT_THR_THRES0_EN_U3  (BIT(14))
#define PCNT_THR_THRES0_EN_U3_M  (BIT(14))
#define PCNT_THR_THRES0_EN_U3_V  0x1
#define PCNT_THR_THRES0_EN_U3_S  14
/* PCNT_THR_L_LIM_EN_U3 : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_L_LIM_EN_U3  (BIT(13))
#define PCNT_THR_L_LIM_EN_U3_M  (BIT(13))
#define PCNT_THR_L_LIM_EN_U3_V  0x1
#define PCNT_THR_L_LIM_EN_U3_S  13
/* PCNT_THR_H_LIM_EN_U3 : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_H_LIM_EN_U3  (BIT(12))
#define PCNT_THR_H_LIM_EN_U3_M  (BIT(12))
#define PCNT_THR_H_LIM_EN_U3_V  0x1
#define PCNT_THR_H_LIM_EN_U3_S  12
/* PCNT_THR_ZERO_EN_U3 : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define PCNT_THR_ZERO_EN_U3  (BIT(11))
#define PCNT_THR_ZERO_EN_U3_M  (BIT(11))
#define PCNT_THR_ZERO_EN_U3_V  0x1
#define PCNT_THR_ZERO_EN_U3_S  11
/* PCNT_FILTER_EN_U3 : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: */
#define PCNT_FILTER_EN_U3  (BIT(10))
#define PCNT_FILTER_EN_U3_M  (BIT(10))
#define PCNT_FILTER_EN_U3_V  0x1
#define PCNT_FILTER_EN_U3_S  10
/* PCNT_FILTER_THRES_U3 : R/W ;bitpos:[9:0] ;default: 10'h10 ; */
/*description: */
#define PCNT_FILTER_THRES_U3  0x000003FF
#define PCNT_FILTER_THRES_U3_M  ((PCNT_FILTER_THRES_U3_V)<<(PCNT_FILTER_THRES_U3_S))
#define PCNT_FILTER_THRES_U3_V  0x3FF
#define PCNT_FILTER_THRES_U3_S  0

#define PCNT_U3_CONF1_REG          (DR_REG_PCNT_BASE + 0x0028)
/* PCNT_CNT_THRES1_U3 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES1_U3  0x0000FFFF
#define PCNT_CNT_THRES1_U3_M  ((PCNT_CNT_THRES1_U3_V)<<(PCNT_CNT_THRES1_U3_S))
#define PCNT_CNT_THRES1_U3_V  0xFFFF
#define PCNT_CNT_THRES1_U3_S  16
/* PCNT_CNT_THRES0_U3 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_THRES0_U3  0x0000FFFF
#define PCNT_CNT_THRES0_U3_M  ((PCNT_CNT_THRES0_U3_V)<<(PCNT_CNT_THRES0_U3_S))
#define PCNT_CNT_THRES0_U3_V  0xFFFF
#define PCNT_CNT_THRES0_U3_S  0

#define PCNT_U3_CONF2_REG          (DR_REG_PCNT_BASE + 0x002c)
/* PCNT_CNT_L_LIM_U3 : R/W ;bitpos:[31:16] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_L_LIM_U3  0x0000FFFF
#define PCNT_CNT_L_LIM_U3_M  ((PCNT_CNT_L_LIM_U3_V)<<(PCNT_CNT_L_LIM_U3_S))
#define PCNT_CNT_L_LIM_U3_V  0xFFFF
#define PCNT_CNT_L_LIM_U3_S  16
/* PCNT_CNT_H_LIM_U3 : R/W ;bitpos:[15:0] ;default: 10'h0 ; */
/*description: */
#define PCNT_CNT_H_LIM_U3  0x0000FFFF
#define PCNT_CNT_H_LIM_U3_M  ((PCNT_CNT_H_LIM_U3_V)<<(PCNT_CNT_H_LIM_U3_S))
#define PCNT_CNT_H_LIM_U3_V  0xFFFF
#define PCNT_CNT_H_LIM_U3_S  0

#define PCNT_U0_CNT_REG          (DR_REG_PCNT_BASE + 0x0030)
/* PCNT_PULSE_CNT_U0 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define PCNT_PULSE_CNT_U0  0x0000FFFF
#define PCNT_PULSE_CNT_U0_M  ((PCNT_PULSE_CNT_U0_V)<<(PCNT_PULSE_CNT_U0_S))
#define PCNT_PULSE_CNT_U0_V  0xFFFF
#define PCNT_PULSE_CNT_U0_S  0

#define PCNT_U1_CNT_REG          (DR_REG_PCNT_BASE + 0x0034)
/* PCNT_PULSE_CNT_U1 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define PCNT_PULSE_CNT_U1  0x0000FFFF
#define PCNT_PULSE_CNT_U1_M  ((PCNT_PULSE_CNT_U1_V)<<(PCNT_PULSE_CNT_U1_S))
#define PCNT_PULSE_CNT_U1_V  0xFFFF
#define PCNT_PULSE_CNT_U1_S  0

#define PCNT_U2_CNT_REG          (DR_REG_PCNT_BASE + 0x0038)
/* PCNT_PULSE_CNT_U2 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define PCNT_PULSE_CNT_U2  0x0000FFFF
#define PCNT_PULSE_CNT_U2_M  ((PCNT_PULSE_CNT_U2_V)<<(PCNT_PULSE_CNT_U2_S))
#define PCNT_PULSE_CNT_U2_V  0xFFFF
#define PCNT_PULSE_CNT_U2_S  0

#define PCNT_U3_CNT_REG          (DR_REG_PCNT_BASE + 0x003c)
/* PCNT_PULSE_CNT_U3 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define PCNT_PULSE_CNT_U3  0x0000FFFF
#define PCNT_PULSE_CNT_U3_M  ((PCNT_PULSE_CNT_U3_V)<<(PCNT_PULSE_CNT_U3_S))
#define PCNT_PULSE_CNT_U3_V  0xFFFF
#define PCNT_PULSE_CNT_U3_S  0

#define PCNT_INT_RAW_REG          (DR_REG_PCNT_BASE + 0x0040)
/* PCNT_CNT_THR_EVENT_U3_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U3_INT_RAW  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_RAW_M  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_RAW_V  0x1
#define PCNT_CNT_THR_EVENT_U3_INT_RAW_S  3
/* PCNT_CNT_THR_EVENT_U2_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U2_INT_RAW  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_RAW_M  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_RAW_V  0x1
#define PCNT_CNT_THR_EVENT_U2_INT_RAW_S  2
/* PCNT_CNT_THR_EVENT_U1_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U1_INT_RAW  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_RAW_M  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_RAW_V  0x1
#define PCNT_CNT_THR_EVENT_U1_INT_RAW_S  1
/* PCNT_CNT_THR_EVENT_U0_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U0_INT_RAW  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_RAW_M  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_RAW_V  0x1
#define PCNT_CNT_THR_EVENT_U0_INT_RAW_S  0

#define PCNT_INT_ST_REG          (DR_REG_PCNT_BASE + 0x0044)
/* PCNT_CNT_THR_EVENT_U3_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U3_INT_ST  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_ST_M  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_ST_V  0x1
#define PCNT_CNT_THR_EVENT_U3_INT_ST_S  3
/* PCNT_CNT_THR_EVENT_U2_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U2_INT_ST  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_ST_M  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_ST_V  0x1
#define PCNT_CNT_THR_EVENT_U2_INT_ST_S  2
/* PCNT_CNT_THR_EVENT_U1_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U1_INT_ST  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_ST_M  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_ST_V  0x1
#define PCNT_CNT_THR_EVENT_U1_INT_ST_S  1
/* PCNT_CNT_THR_EVENT_U0_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U0_INT_ST  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_ST_M  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_ST_V  0x1
#define PCNT_CNT_THR_EVENT_U0_INT_ST_S  0

#define PCNT_INT_ENA_REG          (DR_REG_PCNT_BASE + 0x0048)
/* PCNT_CNT_THR_EVENT_U3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U3_INT_ENA  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_ENA_M  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_ENA_V  0x1
#define PCNT_CNT_THR_EVENT_U3_INT_ENA_S  3
/* PCNT_CNT_THR_EVENT_U2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U2_INT_ENA  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_ENA_M  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_ENA_V  0x1
#define PCNT_CNT_THR_EVENT_U2_INT_ENA_S  2
/* PCNT_CNT_THR_EVENT_U1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U1_INT_ENA  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_ENA_M  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_ENA_V  0x1
#define PCNT_CNT_THR_EVENT_U1_INT_ENA_S  1
/* PCNT_CNT_THR_EVENT_U0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U0_INT_ENA  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_ENA_M  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_ENA_V  0x1
#define PCNT_CNT_THR_EVENT_U0_INT_ENA_S  0

#define PCNT_INT_CLR_REG          (DR_REG_PCNT_BASE + 0x004c)
/* PCNT_CNT_THR_EVENT_U3_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U3_INT_CLR  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_CLR_M  (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_CLR_V  0x1
#define PCNT_CNT_THR_EVENT_U3_INT_CLR_S  3
/* PCNT_CNT_THR_EVENT_U2_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U2_INT_CLR  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_CLR_M  (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_CLR_V  0x1
#define PCNT_CNT_THR_EVENT_U2_INT_CLR_S  2
/* PCNT_CNT_THR_EVENT_U1_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U1_INT_CLR  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_CLR_M  (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_CLR_V  0x1
#define PCNT_CNT_THR_EVENT_U1_INT_CLR_S  1
/* PCNT_CNT_THR_EVENT_U0_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_EVENT_U0_INT_CLR  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_CLR_M  (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_CLR_V  0x1
#define PCNT_CNT_THR_EVENT_U0_INT_CLR_S  0

#define PCNT_U0_STATUS_REG          (DR_REG_PCNT_BASE + 0x0050)
/* PCNT_CNT_THR_ZERO_LAT_U0 : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_LAT_U0  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U0_M  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U0_V  0x1
#define PCNT_CNT_THR_ZERO_LAT_U0_S  6
/* PCNT_CNT_THR_H_LIM_LAT_U0 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_H_LIM_LAT_U0  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U0_M  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U0_V  0x1
#define PCNT_CNT_THR_H_LIM_LAT_U0_S  5
/* PCNT_CNT_THR_L_LIM_LAT_U0 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_L_LIM_LAT_U0  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U0_M  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U0_V  0x1
#define PCNT_CNT_THR_L_LIM_LAT_U0_S  4
/* PCNT_CNT_THR_THRES0_LAT_U0 : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES0_LAT_U0  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U0_M  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U0_V  0x1
#define PCNT_CNT_THR_THRES0_LAT_U0_S  3
/* PCNT_CNT_THR_THRES1_LAT_U0 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES1_LAT_U0  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U0_M  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U0_V  0x1
#define PCNT_CNT_THR_THRES1_LAT_U0_S  2
/* PCNT_CNT_THR_ZERO_MODE_U0 : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_MODE_U0  0x00000003
#define PCNT_CNT_THR_ZERO_MODE_U0_M  ((PCNT_CNT_THR_ZERO_MODE_U0_V)<<(PCNT_CNT_THR_ZERO_MODE_U0_S))
#define PCNT_CNT_THR_ZERO_MODE_U0_V  0x3
#define PCNT_CNT_THR_ZERO_MODE_U0_S  0

#define PCNT_U1_STATUS_REG          (DR_REG_PCNT_BASE + 0x0054)
/* PCNT_CNT_THR_ZERO_LAT_U1 : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_LAT_U1  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U1_M  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U1_V  0x1
#define PCNT_CNT_THR_ZERO_LAT_U1_S  6
/* PCNT_CNT_THR_H_LIM_LAT_U1 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_H_LIM_LAT_U1  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U1_M  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U1_V  0x1
#define PCNT_CNT_THR_H_LIM_LAT_U1_S  5
/* PCNT_CNT_THR_L_LIM_LAT_U1 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_L_LIM_LAT_U1  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U1_M  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U1_V  0x1
#define PCNT_CNT_THR_L_LIM_LAT_U1_S  4
/* PCNT_CNT_THR_THRES0_LAT_U1 : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES0_LAT_U1  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U1_M  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U1_V  0x1
#define PCNT_CNT_THR_THRES0_LAT_U1_S  3
/* PCNT_CNT_THR_THRES1_LAT_U1 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES1_LAT_U1  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U1_M  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U1_V  0x1
#define PCNT_CNT_THR_THRES1_LAT_U1_S  2
/* PCNT_CNT_THR_ZERO_MODE_U1 : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_MODE_U1  0x00000003
#define PCNT_CNT_THR_ZERO_MODE_U1_M  ((PCNT_CNT_THR_ZERO_MODE_U1_V)<<(PCNT_CNT_THR_ZERO_MODE_U1_S))
#define PCNT_CNT_THR_ZERO_MODE_U1_V  0x3
#define PCNT_CNT_THR_ZERO_MODE_U1_S  0

#define PCNT_U2_STATUS_REG          (DR_REG_PCNT_BASE + 0x0058)
/* PCNT_CNT_THR_ZERO_LAT_U2 : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_LAT_U2  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U2_M  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U2_V  0x1
#define PCNT_CNT_THR_ZERO_LAT_U2_S  6
/* PCNT_CNT_THR_H_LIM_LAT_U2 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_H_LIM_LAT_U2  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U2_M  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U2_V  0x1
#define PCNT_CNT_THR_H_LIM_LAT_U2_S  5
/* PCNT_CNT_THR_L_LIM_LAT_U2 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_L_LIM_LAT_U2  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U2_M  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U2_V  0x1
#define PCNT_CNT_THR_L_LIM_LAT_U2_S  4
/* PCNT_CNT_THR_THRES0_LAT_U2 : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES0_LAT_U2  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U2_M  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U2_V  0x1
#define PCNT_CNT_THR_THRES0_LAT_U2_S  3
/* PCNT_CNT_THR_THRES1_LAT_U2 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES1_LAT_U2  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U2_M  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U2_V  0x1
#define PCNT_CNT_THR_THRES1_LAT_U2_S  2
/* PCNT_CNT_THR_ZERO_MODE_U2 : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_MODE_U2  0x00000003
#define PCNT_CNT_THR_ZERO_MODE_U2_M  ((PCNT_CNT_THR_ZERO_MODE_U2_V)<<(PCNT_CNT_THR_ZERO_MODE_U2_S))
#define PCNT_CNT_THR_ZERO_MODE_U2_V  0x3
#define PCNT_CNT_THR_ZERO_MODE_U2_S  0

#define PCNT_U3_STATUS_REG          (DR_REG_PCNT_BASE + 0x005c)
/* PCNT_CNT_THR_ZERO_LAT_U3 : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_LAT_U3  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U3_M  (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U3_V  0x1
#define PCNT_CNT_THR_ZERO_LAT_U3_S  6
/* PCNT_CNT_THR_H_LIM_LAT_U3 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_H_LIM_LAT_U3  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U3_M  (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U3_V  0x1
#define PCNT_CNT_THR_H_LIM_LAT_U3_S  5
/* PCNT_CNT_THR_L_LIM_LAT_U3 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_L_LIM_LAT_U3  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U3_M  (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U3_V  0x1
#define PCNT_CNT_THR_L_LIM_LAT_U3_S  4
/* PCNT_CNT_THR_THRES0_LAT_U3 : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES0_LAT_U3  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U3_M  (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U3_V  0x1
#define PCNT_CNT_THR_THRES0_LAT_U3_S  3
/* PCNT_CNT_THR_THRES1_LAT_U3 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_THR_THRES1_LAT_U3  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U3_M  (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U3_V  0x1
#define PCNT_CNT_THR_THRES1_LAT_U3_S  2
/* PCNT_CNT_THR_ZERO_MODE_U3 : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define PCNT_CNT_THR_ZERO_MODE_U3  0x00000003
#define PCNT_CNT_THR_ZERO_MODE_U3_M  ((PCNT_CNT_THR_ZERO_MODE_U3_V)<<(PCNT_CNT_THR_ZERO_MODE_U3_S))
#define PCNT_CNT_THR_ZERO_MODE_U3_V  0x3
#define PCNT_CNT_THR_ZERO_MODE_U3_S  0

#define PCNT_CTRL_REG          (DR_REG_PCNT_BASE + 0x0060)
/* PCNT_CLK_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define PCNT_CLK_EN  (BIT(16))
#define PCNT_CLK_EN_M  (BIT(16))
#define PCNT_CLK_EN_V  0x1
#define PCNT_CLK_EN_S  16
/* PCNT_CNT_PAUSE_U3 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_PAUSE_U3  (BIT(7))
#define PCNT_CNT_PAUSE_U3_M  (BIT(7))
#define PCNT_CNT_PAUSE_U3_V  0x1
#define PCNT_CNT_PAUSE_U3_S  7
/* PCNT_PULSE_CNT_RST_U3 : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define PCNT_PULSE_CNT_RST_U3  (BIT(6))
#define PCNT_PULSE_CNT_RST_U3_M  (BIT(6))
#define PCNT_PULSE_CNT_RST_U3_V  0x1
#define PCNT_PULSE_CNT_RST_U3_S  6
/* PCNT_CNT_PAUSE_U2 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_PAUSE_U2  (BIT(5))
#define PCNT_CNT_PAUSE_U2_M  (BIT(5))
#define PCNT_CNT_PAUSE_U2_V  0x1
#define PCNT_CNT_PAUSE_U2_S  5
/* PCNT_PULSE_CNT_RST_U2 : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define PCNT_PULSE_CNT_RST_U2  (BIT(4))
#define PCNT_PULSE_CNT_RST_U2_M  (BIT(4))
#define PCNT_PULSE_CNT_RST_U2_V  0x1
#define PCNT_PULSE_CNT_RST_U2_S  4
/* PCNT_CNT_PAUSE_U1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_PAUSE_U1  (BIT(3))
#define PCNT_CNT_PAUSE_U1_M  (BIT(3))
#define PCNT_CNT_PAUSE_U1_V  0x1
#define PCNT_CNT_PAUSE_U1_S  3
/* PCNT_PULSE_CNT_RST_U1 : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define PCNT_PULSE_CNT_RST_U1  (BIT(2))
#define PCNT_PULSE_CNT_RST_U1_M  (BIT(2))
#define PCNT_PULSE_CNT_RST_U1_V  0x1
#define PCNT_PULSE_CNT_RST_U1_S  2
/* PCNT_CNT_PAUSE_U0 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define PCNT_CNT_PAUSE_U0  (BIT(1))
#define PCNT_CNT_PAUSE_U0_M  (BIT(1))
#define PCNT_CNT_PAUSE_U0_V  0x1
#define PCNT_CNT_PAUSE_U0_S  1
/* PCNT_PULSE_CNT_RST_U0 : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define PCNT_PULSE_CNT_RST_U0  (BIT(0))
#define PCNT_PULSE_CNT_RST_U0_M  (BIT(0))
#define PCNT_PULSE_CNT_RST_U0_V  0x1
#define PCNT_PULSE_CNT_RST_U0_S  0

#define PCNT_DATE_REG          (DR_REG_PCNT_BASE + 0x00fc)
/* PCNT_DATE : R/W ;bitpos:[31:0] ;default: 32'h18072600 ; */
/*description: */
#define PCNT_DATE  0xFFFFFFFF
#define PCNT_DATE_M  ((PCNT_DATE_V)<<(PCNT_DATE_S))
#define PCNT_DATE_V  0xFFFFFFFF
#define PCNT_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_PCNT_REG_H_ */
