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
#ifndef _SOC_RMT_REG_H_
#define _SOC_RMT_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define RMT_CH0DATA_REG          (DR_REG_RMT_BASE + 0x0000)

#define RMT_CH1DATA_REG          (DR_REG_RMT_BASE + 0x0004)

#define RMT_CH2DATA_REG          (DR_REG_RMT_BASE + 0x0008)

#define RMT_CH3DATA_REG          (DR_REG_RMT_BASE + 0x000c)

#define RMT_CH0CONF0_REG          (DR_REG_RMT_BASE + 0x0010)
/* RMT_CARRIER_OUT_LV_CH0 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_OUT_LV_CH0  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_V  0x1
#define RMT_CARRIER_OUT_LV_CH0_S  29
/* RMT_CARRIER_EN_CH0 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EN_CH0  (BIT(28))
#define RMT_CARRIER_EN_CH0_M  (BIT(28))
#define RMT_CARRIER_EN_CH0_V  0x1
#define RMT_CARRIER_EN_CH0_S  28
/* RMT_CARRIER_EFF_EN_CH0 : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EFF_EN_CH0  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH0_M  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH0_V  0x1
#define RMT_CARRIER_EFF_EN_CH0_S  27
/* RMT_MEM_SIZE_CH0 : R/W ;bitpos:[26:24] ;default: 3'h1 ; */
/*description: */
#define RMT_MEM_SIZE_CH0  0x00000007
#define RMT_MEM_SIZE_CH0_M  ((RMT_MEM_SIZE_CH0_V)<<(RMT_MEM_SIZE_CH0_S))
#define RMT_MEM_SIZE_CH0_V  0x7
#define RMT_MEM_SIZE_CH0_S  24
/* RMT_IDLE_THRES_CH0 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: */
#define RMT_IDLE_THRES_CH0  0x0000FFFF
#define RMT_IDLE_THRES_CH0_M  ((RMT_IDLE_THRES_CH0_V)<<(RMT_IDLE_THRES_CH0_S))
#define RMT_IDLE_THRES_CH0_V  0xFFFF
#define RMT_IDLE_THRES_CH0_S  8
/* RMT_DIV_CNT_CH0 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: */
#define RMT_DIV_CNT_CH0  0x000000FF
#define RMT_DIV_CNT_CH0_M  ((RMT_DIV_CNT_CH0_V)<<(RMT_DIV_CNT_CH0_S))
#define RMT_DIV_CNT_CH0_V  0xFF
#define RMT_DIV_CNT_CH0_S  0

#define RMT_CH0CONF1_REG          (DR_REG_RMT_BASE + 0x0014)
/* RMT_TX_STOP_CH0 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_STOP_CH0  (BIT(20))
#define RMT_TX_STOP_CH0_M  (BIT(20))
#define RMT_TX_STOP_CH0_V  0x1
#define RMT_TX_STOP_CH0_S  20
/* RMT_IDLE_OUT_EN_CH0 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_EN_CH0  (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_V  0x1
#define RMT_IDLE_OUT_EN_CH0_S  19
/* RMT_IDLE_OUT_LV_CH0 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_LV_CH0  (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_V  0x1
#define RMT_IDLE_OUT_LV_CH0_S  18
/* RMT_REF_ALWAYS_ON_CH0 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_ALWAYS_ON_CH0  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_V  0x1
#define RMT_REF_ALWAYS_ON_CH0_S  17
/* RMT_CHK_RX_CARRIER_EN_CH0 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CHK_RX_CARRIER_EN_CH0  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH0_M  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH0_V  0x1
#define RMT_CHK_RX_CARRIER_EN_CH0_S  16
/* RMT_RX_FILTER_THRES_CH0 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: */
#define RMT_RX_FILTER_THRES_CH0  0x000000FF
#define RMT_RX_FILTER_THRES_CH0_M  ((RMT_RX_FILTER_THRES_CH0_V)<<(RMT_RX_FILTER_THRES_CH0_S))
#define RMT_RX_FILTER_THRES_CH0_V  0xFF
#define RMT_RX_FILTER_THRES_CH0_S  8
/* RMT_RX_FILTER_EN_CH0 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_RX_FILTER_EN_CH0  (BIT(7))
#define RMT_RX_FILTER_EN_CH0_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH0_V  0x1
#define RMT_RX_FILTER_EN_CH0_S  7
/* RMT_TX_CONTI_MODE_CH0 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_CONTI_MODE_CH0  (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_V  0x1
#define RMT_TX_CONTI_MODE_CH0_S  6
/* RMT_MEM_OWNER_CH0 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define RMT_MEM_OWNER_CH0  (BIT(5))
#define RMT_MEM_OWNER_CH0_M  (BIT(5))
#define RMT_MEM_OWNER_CH0_V  0x1
#define RMT_MEM_OWNER_CH0_S  5
/* RMT_APB_MEM_RST_CH0 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RST_CH0  (BIT(4))
#define RMT_APB_MEM_RST_CH0_M  (BIT(4))
#define RMT_APB_MEM_RST_CH0_V  0x1
#define RMT_APB_MEM_RST_CH0_S  4
/* RMT_MEM_RD_RST_CH0 : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_RD_RST_CH0  (BIT(3))
#define RMT_MEM_RD_RST_CH0_M  (BIT(3))
#define RMT_MEM_RD_RST_CH0_V  0x1
#define RMT_MEM_RD_RST_CH0_S  3
/* RMT_MEM_WR_RST_CH0 : WO ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define RMT_MEM_WR_RST_CH0  (BIT(2))
#define RMT_MEM_WR_RST_CH0_M  (BIT(2))
#define RMT_MEM_WR_RST_CH0_V  0x1
#define RMT_MEM_WR_RST_CH0_S  2
/* RMT_RX_EN_CH0 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define RMT_RX_EN_CH0  (BIT(1))
#define RMT_RX_EN_CH0_M  (BIT(1))
#define RMT_RX_EN_CH0_V  0x1
#define RMT_RX_EN_CH0_S  1
/* RMT_TX_START_CH0 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define RMT_TX_START_CH0  (BIT(0))
#define RMT_TX_START_CH0_M  (BIT(0))
#define RMT_TX_START_CH0_V  0x1
#define RMT_TX_START_CH0_S  0

#define RMT_CH1CONF0_REG          (DR_REG_RMT_BASE + 0x0018)
/* RMT_CARRIER_OUT_LV_CH1 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_OUT_LV_CH1  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH1_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH1_V  0x1
#define RMT_CARRIER_OUT_LV_CH1_S  29
/* RMT_CARRIER_EN_CH1 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EN_CH1  (BIT(28))
#define RMT_CARRIER_EN_CH1_M  (BIT(28))
#define RMT_CARRIER_EN_CH1_V  0x1
#define RMT_CARRIER_EN_CH1_S  28
/* RMT_CARRIER_EFF_EN_CH1 : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EFF_EN_CH1  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH1_M  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH1_V  0x1
#define RMT_CARRIER_EFF_EN_CH1_S  27
/* RMT_MEM_SIZE_CH1 : R/W ;bitpos:[26:24] ;default: 3'h1 ; */
/*description: */
#define RMT_MEM_SIZE_CH1  0x00000007
#define RMT_MEM_SIZE_CH1_M  ((RMT_MEM_SIZE_CH1_V)<<(RMT_MEM_SIZE_CH1_S))
#define RMT_MEM_SIZE_CH1_V  0x7
#define RMT_MEM_SIZE_CH1_S  24
/* RMT_IDLE_THRES_CH1 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: */
#define RMT_IDLE_THRES_CH1  0x0000FFFF
#define RMT_IDLE_THRES_CH1_M  ((RMT_IDLE_THRES_CH1_V)<<(RMT_IDLE_THRES_CH1_S))
#define RMT_IDLE_THRES_CH1_V  0xFFFF
#define RMT_IDLE_THRES_CH1_S  8
/* RMT_DIV_CNT_CH1 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: */
#define RMT_DIV_CNT_CH1  0x000000FF
#define RMT_DIV_CNT_CH1_M  ((RMT_DIV_CNT_CH1_V)<<(RMT_DIV_CNT_CH1_S))
#define RMT_DIV_CNT_CH1_V  0xFF
#define RMT_DIV_CNT_CH1_S  0

#define RMT_CH1CONF1_REG          (DR_REG_RMT_BASE + 0x001c)
/* RMT_TX_STOP_CH1 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_STOP_CH1  (BIT(20))
#define RMT_TX_STOP_CH1_M  (BIT(20))
#define RMT_TX_STOP_CH1_V  0x1
#define RMT_TX_STOP_CH1_S  20
/* RMT_IDLE_OUT_EN_CH1 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_EN_CH1  (BIT(19))
#define RMT_IDLE_OUT_EN_CH1_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH1_V  0x1
#define RMT_IDLE_OUT_EN_CH1_S  19
/* RMT_IDLE_OUT_LV_CH1 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_LV_CH1  (BIT(18))
#define RMT_IDLE_OUT_LV_CH1_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH1_V  0x1
#define RMT_IDLE_OUT_LV_CH1_S  18
/* RMT_REF_ALWAYS_ON_CH1 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_ALWAYS_ON_CH1  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH1_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH1_V  0x1
#define RMT_REF_ALWAYS_ON_CH1_S  17
/* RMT_CHK_RX_CARRIER_EN_CH1 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CHK_RX_CARRIER_EN_CH1  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH1_M  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH1_V  0x1
#define RMT_CHK_RX_CARRIER_EN_CH1_S  16
/* RMT_RX_FILTER_THRES_CH1 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: */
#define RMT_RX_FILTER_THRES_CH1  0x000000FF
#define RMT_RX_FILTER_THRES_CH1_M  ((RMT_RX_FILTER_THRES_CH1_V)<<(RMT_RX_FILTER_THRES_CH1_S))
#define RMT_RX_FILTER_THRES_CH1_V  0xFF
#define RMT_RX_FILTER_THRES_CH1_S  8
/* RMT_RX_FILTER_EN_CH1 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_RX_FILTER_EN_CH1  (BIT(7))
#define RMT_RX_FILTER_EN_CH1_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH1_V  0x1
#define RMT_RX_FILTER_EN_CH1_S  7
/* RMT_TX_CONTI_MODE_CH1 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_CONTI_MODE_CH1  (BIT(6))
#define RMT_TX_CONTI_MODE_CH1_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH1_V  0x1
#define RMT_TX_CONTI_MODE_CH1_S  6
/* RMT_MEM_OWNER_CH1 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define RMT_MEM_OWNER_CH1  (BIT(5))
#define RMT_MEM_OWNER_CH1_M  (BIT(5))
#define RMT_MEM_OWNER_CH1_V  0x1
#define RMT_MEM_OWNER_CH1_S  5
/* RMT_APB_MEM_RST_CH1 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RST_CH1  (BIT(4))
#define RMT_APB_MEM_RST_CH1_M  (BIT(4))
#define RMT_APB_MEM_RST_CH1_V  0x1
#define RMT_APB_MEM_RST_CH1_S  4
/* RMT_MEM_RD_RST_CH1 : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_RD_RST_CH1  (BIT(3))
#define RMT_MEM_RD_RST_CH1_M  (BIT(3))
#define RMT_MEM_RD_RST_CH1_V  0x1
#define RMT_MEM_RD_RST_CH1_S  3
/* RMT_MEM_WR_RST_CH1 : WO ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define RMT_MEM_WR_RST_CH1  (BIT(2))
#define RMT_MEM_WR_RST_CH1_M  (BIT(2))
#define RMT_MEM_WR_RST_CH1_V  0x1
#define RMT_MEM_WR_RST_CH1_S  2
/* RMT_RX_EN_CH1 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define RMT_RX_EN_CH1  (BIT(1))
#define RMT_RX_EN_CH1_M  (BIT(1))
#define RMT_RX_EN_CH1_V  0x1
#define RMT_RX_EN_CH1_S  1
/* RMT_TX_START_CH1 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define RMT_TX_START_CH1  (BIT(0))
#define RMT_TX_START_CH1_M  (BIT(0))
#define RMT_TX_START_CH1_V  0x1
#define RMT_TX_START_CH1_S  0

#define RMT_CH2CONF0_REG          (DR_REG_RMT_BASE + 0x0020)
/* RMT_CARRIER_OUT_LV_CH2 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_OUT_LV_CH2  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH2_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH2_V  0x1
#define RMT_CARRIER_OUT_LV_CH2_S  29
/* RMT_CARRIER_EN_CH2 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EN_CH2  (BIT(28))
#define RMT_CARRIER_EN_CH2_M  (BIT(28))
#define RMT_CARRIER_EN_CH2_V  0x1
#define RMT_CARRIER_EN_CH2_S  28
/* RMT_CARRIER_EFF_EN_CH2 : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EFF_EN_CH2  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH2_M  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH2_V  0x1
#define RMT_CARRIER_EFF_EN_CH2_S  27
/* RMT_MEM_SIZE_CH2 : R/W ;bitpos:[26:24] ;default: 3'h1 ; */
/*description: */
#define RMT_MEM_SIZE_CH2  0x00000007
#define RMT_MEM_SIZE_CH2_M  ((RMT_MEM_SIZE_CH2_V)<<(RMT_MEM_SIZE_CH2_S))
#define RMT_MEM_SIZE_CH2_V  0x7
#define RMT_MEM_SIZE_CH2_S  24
/* RMT_IDLE_THRES_CH2 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: */
#define RMT_IDLE_THRES_CH2  0x0000FFFF
#define RMT_IDLE_THRES_CH2_M  ((RMT_IDLE_THRES_CH2_V)<<(RMT_IDLE_THRES_CH2_S))
#define RMT_IDLE_THRES_CH2_V  0xFFFF
#define RMT_IDLE_THRES_CH2_S  8
/* RMT_DIV_CNT_CH2 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: */
#define RMT_DIV_CNT_CH2  0x000000FF
#define RMT_DIV_CNT_CH2_M  ((RMT_DIV_CNT_CH2_V)<<(RMT_DIV_CNT_CH2_S))
#define RMT_DIV_CNT_CH2_V  0xFF
#define RMT_DIV_CNT_CH2_S  0

#define RMT_CH2CONF1_REG          (DR_REG_RMT_BASE + 0x0024)
/* RMT_TX_STOP_CH2 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_STOP_CH2  (BIT(20))
#define RMT_TX_STOP_CH2_M  (BIT(20))
#define RMT_TX_STOP_CH2_V  0x1
#define RMT_TX_STOP_CH2_S  20
/* RMT_IDLE_OUT_EN_CH2 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_EN_CH2  (BIT(19))
#define RMT_IDLE_OUT_EN_CH2_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH2_V  0x1
#define RMT_IDLE_OUT_EN_CH2_S  19
/* RMT_IDLE_OUT_LV_CH2 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_LV_CH2  (BIT(18))
#define RMT_IDLE_OUT_LV_CH2_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH2_V  0x1
#define RMT_IDLE_OUT_LV_CH2_S  18
/* RMT_REF_ALWAYS_ON_CH2 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_ALWAYS_ON_CH2  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH2_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH2_V  0x1
#define RMT_REF_ALWAYS_ON_CH2_S  17
/* RMT_CHK_RX_CARRIER_EN_CH2 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CHK_RX_CARRIER_EN_CH2  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH2_M  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH2_V  0x1
#define RMT_CHK_RX_CARRIER_EN_CH2_S  16
/* RMT_RX_FILTER_THRES_CH2 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: */
#define RMT_RX_FILTER_THRES_CH2  0x000000FF
#define RMT_RX_FILTER_THRES_CH2_M  ((RMT_RX_FILTER_THRES_CH2_V)<<(RMT_RX_FILTER_THRES_CH2_S))
#define RMT_RX_FILTER_THRES_CH2_V  0xFF
#define RMT_RX_FILTER_THRES_CH2_S  8
/* RMT_RX_FILTER_EN_CH2 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_RX_FILTER_EN_CH2  (BIT(7))
#define RMT_RX_FILTER_EN_CH2_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH2_V  0x1
#define RMT_RX_FILTER_EN_CH2_S  7
/* RMT_TX_CONTI_MODE_CH2 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_CONTI_MODE_CH2  (BIT(6))
#define RMT_TX_CONTI_MODE_CH2_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH2_V  0x1
#define RMT_TX_CONTI_MODE_CH2_S  6
/* RMT_MEM_OWNER_CH2 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define RMT_MEM_OWNER_CH2  (BIT(5))
#define RMT_MEM_OWNER_CH2_M  (BIT(5))
#define RMT_MEM_OWNER_CH2_V  0x1
#define RMT_MEM_OWNER_CH2_S  5
/* RMT_APB_MEM_RST_CH2 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RST_CH2  (BIT(4))
#define RMT_APB_MEM_RST_CH2_M  (BIT(4))
#define RMT_APB_MEM_RST_CH2_V  0x1
#define RMT_APB_MEM_RST_CH2_S  4
/* RMT_MEM_RD_RST_CH2 : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_RD_RST_CH2  (BIT(3))
#define RMT_MEM_RD_RST_CH2_M  (BIT(3))
#define RMT_MEM_RD_RST_CH2_V  0x1
#define RMT_MEM_RD_RST_CH2_S  3
/* RMT_MEM_WR_RST_CH2 : WO ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define RMT_MEM_WR_RST_CH2  (BIT(2))
#define RMT_MEM_WR_RST_CH2_M  (BIT(2))
#define RMT_MEM_WR_RST_CH2_V  0x1
#define RMT_MEM_WR_RST_CH2_S  2
/* RMT_RX_EN_CH2 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define RMT_RX_EN_CH2  (BIT(1))
#define RMT_RX_EN_CH2_M  (BIT(1))
#define RMT_RX_EN_CH2_V  0x1
#define RMT_RX_EN_CH2_S  1
/* RMT_TX_START_CH2 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define RMT_TX_START_CH2  (BIT(0))
#define RMT_TX_START_CH2_M  (BIT(0))
#define RMT_TX_START_CH2_V  0x1
#define RMT_TX_START_CH2_S  0

#define RMT_CH3CONF0_REG          (DR_REG_RMT_BASE + 0x0028)
/* RMT_CARRIER_OUT_LV_CH3 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_OUT_LV_CH3  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH3_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH3_V  0x1
#define RMT_CARRIER_OUT_LV_CH3_S  29
/* RMT_CARRIER_EN_CH3 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EN_CH3  (BIT(28))
#define RMT_CARRIER_EN_CH3_M  (BIT(28))
#define RMT_CARRIER_EN_CH3_V  0x1
#define RMT_CARRIER_EN_CH3_S  28
/* RMT_CARRIER_EFF_EN_CH3 : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: */
#define RMT_CARRIER_EFF_EN_CH3  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH3_M  (BIT(27))
#define RMT_CARRIER_EFF_EN_CH3_V  0x1
#define RMT_CARRIER_EFF_EN_CH3_S  27
/* RMT_MEM_SIZE_CH3 : R/W ;bitpos:[26:24] ;default: 3'h1 ; */
/*description: */
#define RMT_MEM_SIZE_CH3  0x00000007
#define RMT_MEM_SIZE_CH3_M  ((RMT_MEM_SIZE_CH3_V)<<(RMT_MEM_SIZE_CH3_S))
#define RMT_MEM_SIZE_CH3_V  0x7
#define RMT_MEM_SIZE_CH3_S  24
/* RMT_IDLE_THRES_CH3 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: */
#define RMT_IDLE_THRES_CH3  0x0000FFFF
#define RMT_IDLE_THRES_CH3_M  ((RMT_IDLE_THRES_CH3_V)<<(RMT_IDLE_THRES_CH3_S))
#define RMT_IDLE_THRES_CH3_V  0xFFFF
#define RMT_IDLE_THRES_CH3_S  8
/* RMT_DIV_CNT_CH3 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: */
#define RMT_DIV_CNT_CH3  0x000000FF
#define RMT_DIV_CNT_CH3_M  ((RMT_DIV_CNT_CH3_V)<<(RMT_DIV_CNT_CH3_S))
#define RMT_DIV_CNT_CH3_V  0xFF
#define RMT_DIV_CNT_CH3_S  0

#define RMT_CH3CONF1_REG          (DR_REG_RMT_BASE + 0x002c)
/* RMT_TX_STOP_CH3 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_STOP_CH3  (BIT(20))
#define RMT_TX_STOP_CH3_M  (BIT(20))
#define RMT_TX_STOP_CH3_V  0x1
#define RMT_TX_STOP_CH3_S  20
/* RMT_IDLE_OUT_EN_CH3 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_EN_CH3  (BIT(19))
#define RMT_IDLE_OUT_EN_CH3_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH3_V  0x1
#define RMT_IDLE_OUT_EN_CH3_S  19
/* RMT_IDLE_OUT_LV_CH3 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_IDLE_OUT_LV_CH3  (BIT(18))
#define RMT_IDLE_OUT_LV_CH3_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH3_V  0x1
#define RMT_IDLE_OUT_LV_CH3_S  18
/* RMT_REF_ALWAYS_ON_CH3 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_ALWAYS_ON_CH3  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH3_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH3_V  0x1
#define RMT_REF_ALWAYS_ON_CH3_S  17
/* RMT_CHK_RX_CARRIER_EN_CH3 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CHK_RX_CARRIER_EN_CH3  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH3_M  (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH3_V  0x1
#define RMT_CHK_RX_CARRIER_EN_CH3_S  16
/* RMT_RX_FILTER_THRES_CH3 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: */
#define RMT_RX_FILTER_THRES_CH3  0x000000FF
#define RMT_RX_FILTER_THRES_CH3_M  ((RMT_RX_FILTER_THRES_CH3_V)<<(RMT_RX_FILTER_THRES_CH3_S))
#define RMT_RX_FILTER_THRES_CH3_V  0xFF
#define RMT_RX_FILTER_THRES_CH3_S  8
/* RMT_RX_FILTER_EN_CH3 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_RX_FILTER_EN_CH3  (BIT(7))
#define RMT_RX_FILTER_EN_CH3_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH3_V  0x1
#define RMT_RX_FILTER_EN_CH3_S  7
/* RMT_TX_CONTI_MODE_CH3 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_CONTI_MODE_CH3  (BIT(6))
#define RMT_TX_CONTI_MODE_CH3_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH3_V  0x1
#define RMT_TX_CONTI_MODE_CH3_S  6
/* RMT_MEM_OWNER_CH3 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define RMT_MEM_OWNER_CH3  (BIT(5))
#define RMT_MEM_OWNER_CH3_M  (BIT(5))
#define RMT_MEM_OWNER_CH3_V  0x1
#define RMT_MEM_OWNER_CH3_S  5
/* RMT_APB_MEM_RST_CH3 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RST_CH3  (BIT(4))
#define RMT_APB_MEM_RST_CH3_M  (BIT(4))
#define RMT_APB_MEM_RST_CH3_V  0x1
#define RMT_APB_MEM_RST_CH3_S  4
/* RMT_MEM_RD_RST_CH3 : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_RD_RST_CH3  (BIT(3))
#define RMT_MEM_RD_RST_CH3_M  (BIT(3))
#define RMT_MEM_RD_RST_CH3_V  0x1
#define RMT_MEM_RD_RST_CH3_S  3
/* RMT_MEM_WR_RST_CH3 : WO ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define RMT_MEM_WR_RST_CH3  (BIT(2))
#define RMT_MEM_WR_RST_CH3_M  (BIT(2))
#define RMT_MEM_WR_RST_CH3_V  0x1
#define RMT_MEM_WR_RST_CH3_S  2
/* RMT_RX_EN_CH3 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define RMT_RX_EN_CH3  (BIT(1))
#define RMT_RX_EN_CH3_M  (BIT(1))
#define RMT_RX_EN_CH3_V  0x1
#define RMT_RX_EN_CH3_S  1
/* RMT_TX_START_CH3 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define RMT_TX_START_CH3  (BIT(0))
#define RMT_TX_START_CH3_M  (BIT(0))
#define RMT_TX_START_CH3_V  0x1
#define RMT_TX_START_CH3_S  0

#define RMT_CH0STATUS_REG          (DR_REG_RMT_BASE + 0x0030)
/* RMT_APB_MEM_RD_ERR_CH0 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RD_ERR_CH0  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH0_M  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH0_V  0x1
#define RMT_APB_MEM_RD_ERR_CH0_S  27
/* RMT_APB_MEM_WR_ERR_CH0 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_WR_ERR_CH0  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH0_M  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH0_V  0x1
#define RMT_APB_MEM_WR_ERR_CH0_S  26
/* RMT_MEM_EMPTY_CH0 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_EMPTY_CH0  (BIT(25))
#define RMT_MEM_EMPTY_CH0_M  (BIT(25))
#define RMT_MEM_EMPTY_CH0_V  0x1
#define RMT_MEM_EMPTY_CH0_S  25
/* RMT_MEM_FULL_CH0 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_FULL_CH0  (BIT(24))
#define RMT_MEM_FULL_CH0_M  (BIT(24))
#define RMT_MEM_FULL_CH0_V  0x1
#define RMT_MEM_FULL_CH0_S  24
/* RMT_MEM_OWNER_ERR_CH0 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_OWNER_ERR_CH0  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH0_M  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH0_V  0x1
#define RMT_MEM_OWNER_ERR_CH0_S  23
/* RMT_STATE_CH0 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define RMT_STATE_CH0  0x00000007
#define RMT_STATE_CH0_M  ((RMT_STATE_CH0_V)<<(RMT_STATE_CH0_S))
#define RMT_STATE_CH0_V  0x7
#define RMT_STATE_CH0_S  20
/* RMT_MEM_RADDR_EX_CH0 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_RADDR_EX_CH0  0x000001FF
#define RMT_MEM_RADDR_EX_CH0_M  ((RMT_MEM_RADDR_EX_CH0_V)<<(RMT_MEM_RADDR_EX_CH0_S))
#define RMT_MEM_RADDR_EX_CH0_V  0x1FF
#define RMT_MEM_RADDR_EX_CH0_S  10
/* RMT_MEM_WADDR_EX_CH0 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_WADDR_EX_CH0  0x000001FF
#define RMT_MEM_WADDR_EX_CH0_M  ((RMT_MEM_WADDR_EX_CH0_V)<<(RMT_MEM_WADDR_EX_CH0_S))
#define RMT_MEM_WADDR_EX_CH0_V  0x1FF
#define RMT_MEM_WADDR_EX_CH0_S  0

#define RMT_CH1STATUS_REG          (DR_REG_RMT_BASE + 0x0034)
/* RMT_APB_MEM_RD_ERR_CH1 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RD_ERR_CH1  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH1_M  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH1_V  0x1
#define RMT_APB_MEM_RD_ERR_CH1_S  27
/* RMT_APB_MEM_WR_ERR_CH1 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_WR_ERR_CH1  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH1_M  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH1_V  0x1
#define RMT_APB_MEM_WR_ERR_CH1_S  26
/* RMT_MEM_EMPTY_CH1 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_EMPTY_CH1  (BIT(25))
#define RMT_MEM_EMPTY_CH1_M  (BIT(25))
#define RMT_MEM_EMPTY_CH1_V  0x1
#define RMT_MEM_EMPTY_CH1_S  25
/* RMT_MEM_FULL_CH1 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_FULL_CH1  (BIT(24))
#define RMT_MEM_FULL_CH1_M  (BIT(24))
#define RMT_MEM_FULL_CH1_V  0x1
#define RMT_MEM_FULL_CH1_S  24
/* RMT_MEM_OWNER_ERR_CH1 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_OWNER_ERR_CH1  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH1_M  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH1_V  0x1
#define RMT_MEM_OWNER_ERR_CH1_S  23
/* RMT_STATE_CH1 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define RMT_STATE_CH1  0x00000007
#define RMT_STATE_CH1_M  ((RMT_STATE_CH1_V)<<(RMT_STATE_CH1_S))
#define RMT_STATE_CH1_V  0x7
#define RMT_STATE_CH1_S  20
/* RMT_MEM_RADDR_EX_CH1 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_RADDR_EX_CH1  0x000001FF
#define RMT_MEM_RADDR_EX_CH1_M  ((RMT_MEM_RADDR_EX_CH1_V)<<(RMT_MEM_RADDR_EX_CH1_S))
#define RMT_MEM_RADDR_EX_CH1_V  0x1FF
#define RMT_MEM_RADDR_EX_CH1_S  10
/* RMT_MEM_WADDR_EX_CH1 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_WADDR_EX_CH1  0x000001FF
#define RMT_MEM_WADDR_EX_CH1_M  ((RMT_MEM_WADDR_EX_CH1_V)<<(RMT_MEM_WADDR_EX_CH1_S))
#define RMT_MEM_WADDR_EX_CH1_V  0x1FF
#define RMT_MEM_WADDR_EX_CH1_S  0

#define RMT_CH2STATUS_REG          (DR_REG_RMT_BASE + 0x0038)
/* RMT_APB_MEM_RD_ERR_CH2 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RD_ERR_CH2  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH2_M  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH2_V  0x1
#define RMT_APB_MEM_RD_ERR_CH2_S  27
/* RMT_APB_MEM_WR_ERR_CH2 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_WR_ERR_CH2  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH2_M  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH2_V  0x1
#define RMT_APB_MEM_WR_ERR_CH2_S  26
/* RMT_MEM_EMPTY_CH2 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_EMPTY_CH2  (BIT(25))
#define RMT_MEM_EMPTY_CH2_M  (BIT(25))
#define RMT_MEM_EMPTY_CH2_V  0x1
#define RMT_MEM_EMPTY_CH2_S  25
/* RMT_MEM_FULL_CH2 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_FULL_CH2  (BIT(24))
#define RMT_MEM_FULL_CH2_M  (BIT(24))
#define RMT_MEM_FULL_CH2_V  0x1
#define RMT_MEM_FULL_CH2_S  24
/* RMT_MEM_OWNER_ERR_CH2 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_OWNER_ERR_CH2  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH2_M  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH2_V  0x1
#define RMT_MEM_OWNER_ERR_CH2_S  23
/* RMT_STATE_CH2 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define RMT_STATE_CH2  0x00000007
#define RMT_STATE_CH2_M  ((RMT_STATE_CH2_V)<<(RMT_STATE_CH2_S))
#define RMT_STATE_CH2_V  0x7
#define RMT_STATE_CH2_S  20
/* RMT_MEM_RADDR_EX_CH2 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_RADDR_EX_CH2  0x000001FF
#define RMT_MEM_RADDR_EX_CH2_M  ((RMT_MEM_RADDR_EX_CH2_V)<<(RMT_MEM_RADDR_EX_CH2_S))
#define RMT_MEM_RADDR_EX_CH2_V  0x1FF
#define RMT_MEM_RADDR_EX_CH2_S  10
/* RMT_MEM_WADDR_EX_CH2 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_WADDR_EX_CH2  0x000001FF
#define RMT_MEM_WADDR_EX_CH2_M  ((RMT_MEM_WADDR_EX_CH2_V)<<(RMT_MEM_WADDR_EX_CH2_S))
#define RMT_MEM_WADDR_EX_CH2_V  0x1FF
#define RMT_MEM_WADDR_EX_CH2_S  0

#define RMT_CH3STATUS_REG          (DR_REG_RMT_BASE + 0x003c)
/* RMT_APB_MEM_RD_ERR_CH3 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_RD_ERR_CH3  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH3_M  (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH3_V  0x1
#define RMT_APB_MEM_RD_ERR_CH3_S  27
/* RMT_APB_MEM_WR_ERR_CH3 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define RMT_APB_MEM_WR_ERR_CH3  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH3_M  (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH3_V  0x1
#define RMT_APB_MEM_WR_ERR_CH3_S  26
/* RMT_MEM_EMPTY_CH3 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_EMPTY_CH3  (BIT(25))
#define RMT_MEM_EMPTY_CH3_M  (BIT(25))
#define RMT_MEM_EMPTY_CH3_V  0x1
#define RMT_MEM_EMPTY_CH3_S  25
/* RMT_MEM_FULL_CH3 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_FULL_CH3  (BIT(24))
#define RMT_MEM_FULL_CH3_M  (BIT(24))
#define RMT_MEM_FULL_CH3_V  0x1
#define RMT_MEM_FULL_CH3_S  24
/* RMT_MEM_OWNER_ERR_CH3 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_OWNER_ERR_CH3  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH3_M  (BIT(23))
#define RMT_MEM_OWNER_ERR_CH3_V  0x1
#define RMT_MEM_OWNER_ERR_CH3_S  23
/* RMT_STATE_CH3 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define RMT_STATE_CH3  0x00000007
#define RMT_STATE_CH3_M  ((RMT_STATE_CH3_V)<<(RMT_STATE_CH3_S))
#define RMT_STATE_CH3_V  0x7
#define RMT_STATE_CH3_S  20
/* RMT_MEM_RADDR_EX_CH3 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_RADDR_EX_CH3  0x000001FF
#define RMT_MEM_RADDR_EX_CH3_M  ((RMT_MEM_RADDR_EX_CH3_V)<<(RMT_MEM_RADDR_EX_CH3_S))
#define RMT_MEM_RADDR_EX_CH3_V  0x1FF
#define RMT_MEM_RADDR_EX_CH3_S  10
/* RMT_MEM_WADDR_EX_CH3 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_MEM_WADDR_EX_CH3  0x000001FF
#define RMT_MEM_WADDR_EX_CH3_M  ((RMT_MEM_WADDR_EX_CH3_V)<<(RMT_MEM_WADDR_EX_CH3_S))
#define RMT_MEM_WADDR_EX_CH3_V  0x1FF
#define RMT_MEM_WADDR_EX_CH3_S  0

#define RMT_CH0ADDR_REG          (DR_REG_RMT_BASE + 0x0040)
/* RMT_APB_MEM_RADDR_CH0 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_RADDR_CH0  0x000001FF
#define RMT_APB_MEM_RADDR_CH0_M  ((RMT_APB_MEM_RADDR_CH0_V)<<(RMT_APB_MEM_RADDR_CH0_S))
#define RMT_APB_MEM_RADDR_CH0_V  0x1FF
#define RMT_APB_MEM_RADDR_CH0_S  10
/* RMT_APB_MEM_WADDR_CH0 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_WADDR_CH0  0x000001FF
#define RMT_APB_MEM_WADDR_CH0_M  ((RMT_APB_MEM_WADDR_CH0_V)<<(RMT_APB_MEM_WADDR_CH0_S))
#define RMT_APB_MEM_WADDR_CH0_V  0x1FF
#define RMT_APB_MEM_WADDR_CH0_S  0

#define RMT_CH1ADDR_REG          (DR_REG_RMT_BASE + 0x0044)
/* RMT_APB_MEM_RADDR_CH1 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_RADDR_CH1  0x000001FF
#define RMT_APB_MEM_RADDR_CH1_M  ((RMT_APB_MEM_RADDR_CH1_V)<<(RMT_APB_MEM_RADDR_CH1_S))
#define RMT_APB_MEM_RADDR_CH1_V  0x1FF
#define RMT_APB_MEM_RADDR_CH1_S  10
/* RMT_APB_MEM_WADDR_CH1 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_WADDR_CH1  0x000001FF
#define RMT_APB_MEM_WADDR_CH1_M  ((RMT_APB_MEM_WADDR_CH1_V)<<(RMT_APB_MEM_WADDR_CH1_S))
#define RMT_APB_MEM_WADDR_CH1_V  0x1FF
#define RMT_APB_MEM_WADDR_CH1_S  0

#define RMT_CH2ADDR_REG          (DR_REG_RMT_BASE + 0x0048)
/* RMT_APB_MEM_RADDR_CH2 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_RADDR_CH2  0x000001FF
#define RMT_APB_MEM_RADDR_CH2_M  ((RMT_APB_MEM_RADDR_CH2_V)<<(RMT_APB_MEM_RADDR_CH2_S))
#define RMT_APB_MEM_RADDR_CH2_V  0x1FF
#define RMT_APB_MEM_RADDR_CH2_S  10
/* RMT_APB_MEM_WADDR_CH2 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_WADDR_CH2  0x000001FF
#define RMT_APB_MEM_WADDR_CH2_M  ((RMT_APB_MEM_WADDR_CH2_V)<<(RMT_APB_MEM_WADDR_CH2_S))
#define RMT_APB_MEM_WADDR_CH2_V  0x1FF
#define RMT_APB_MEM_WADDR_CH2_S  0

#define RMT_CH3ADDR_REG          (DR_REG_RMT_BASE + 0x004c)
/* RMT_APB_MEM_RADDR_CH3 : RO ;bitpos:[18:10] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_RADDR_CH3  0x000001FF
#define RMT_APB_MEM_RADDR_CH3_M  ((RMT_APB_MEM_RADDR_CH3_V)<<(RMT_APB_MEM_RADDR_CH3_S))
#define RMT_APB_MEM_RADDR_CH3_V  0x1FF
#define RMT_APB_MEM_RADDR_CH3_S  10
/* RMT_APB_MEM_WADDR_CH3 : RO ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: */
#define RMT_APB_MEM_WADDR_CH3  0x000001FF
#define RMT_APB_MEM_WADDR_CH3_M  ((RMT_APB_MEM_WADDR_CH3_V)<<(RMT_APB_MEM_WADDR_CH3_S))
#define RMT_APB_MEM_WADDR_CH3_V  0x1FF
#define RMT_APB_MEM_WADDR_CH3_S  0

#define RMT_INT_RAW_REG          (DR_REG_RMT_BASE + 0x0050)
/* RMT_CH3_RX_THR_EVENT_INT_RAW : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_THR_EVENT_INT_RAW  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_RAW_M  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH3_RX_THR_EVENT_INT_RAW_S  23
/* RMT_CH2_RX_THR_EVENT_INT_RAW : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_THR_EVENT_INT_RAW  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_RAW_M  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH2_RX_THR_EVENT_INT_RAW_S  22
/* RMT_CH1_RX_THR_EVENT_INT_RAW : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_THR_EVENT_INT_RAW  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_RAW_M  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH1_RX_THR_EVENT_INT_RAW_S  21
/* RMT_CH0_RX_THR_EVENT_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_THR_EVENT_INT_RAW  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_RAW_M  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH0_RX_THR_EVENT_INT_RAW_S  20
/* RMT_CH3_TX_LOOP_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_LOOP_INT_RAW  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_RAW_M  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_RAW_V  0x1
#define RMT_CH3_TX_LOOP_INT_RAW_S  19
/* RMT_CH2_TX_LOOP_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_LOOP_INT_RAW  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_RAW_M  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_RAW_V  0x1
#define RMT_CH2_TX_LOOP_INT_RAW_S  18
/* RMT_CH1_TX_LOOP_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_LOOP_INT_RAW  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_RAW_M  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_RAW_V  0x1
#define RMT_CH1_TX_LOOP_INT_RAW_S  17
/* RMT_CH0_TX_LOOP_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_LOOP_INT_RAW  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_RAW_M  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_RAW_V  0x1
#define RMT_CH0_TX_LOOP_INT_RAW_S  16
/* RMT_CH3_TX_THR_EVENT_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_THR_EVENT_INT_RAW  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_RAW_M  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_RAW_S  15
/* RMT_CH2_TX_THR_EVENT_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_THR_EVENT_INT_RAW  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_RAW_M  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_RAW_S  14
/* RMT_CH1_TX_THR_EVENT_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_THR_EVENT_INT_RAW  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_RAW_M  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_RAW_S  13
/* RMT_CH0_TX_THR_EVENT_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_THR_EVENT_INT_RAW  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_RAW_M  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_RAW_S  12
/* RMT_CH3_ERR_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_ERR_INT_RAW  (BIT(11))
#define RMT_CH3_ERR_INT_RAW_M  (BIT(11))
#define RMT_CH3_ERR_INT_RAW_V  0x1
#define RMT_CH3_ERR_INT_RAW_S  11
/* RMT_CH3_RX_END_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_END_INT_RAW  (BIT(10))
#define RMT_CH3_RX_END_INT_RAW_M  (BIT(10))
#define RMT_CH3_RX_END_INT_RAW_V  0x1
#define RMT_CH3_RX_END_INT_RAW_S  10
/* RMT_CH3_TX_END_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_END_INT_RAW  (BIT(9))
#define RMT_CH3_TX_END_INT_RAW_M  (BIT(9))
#define RMT_CH3_TX_END_INT_RAW_V  0x1
#define RMT_CH3_TX_END_INT_RAW_S  9
/* RMT_CH2_ERR_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_ERR_INT_RAW  (BIT(8))
#define RMT_CH2_ERR_INT_RAW_M  (BIT(8))
#define RMT_CH2_ERR_INT_RAW_V  0x1
#define RMT_CH2_ERR_INT_RAW_S  8
/* RMT_CH2_RX_END_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_END_INT_RAW  (BIT(7))
#define RMT_CH2_RX_END_INT_RAW_M  (BIT(7))
#define RMT_CH2_RX_END_INT_RAW_V  0x1
#define RMT_CH2_RX_END_INT_RAW_S  7
/* RMT_CH2_TX_END_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_END_INT_RAW  (BIT(6))
#define RMT_CH2_TX_END_INT_RAW_M  (BIT(6))
#define RMT_CH2_TX_END_INT_RAW_V  0x1
#define RMT_CH2_TX_END_INT_RAW_S  6
/* RMT_CH1_ERR_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_ERR_INT_RAW  (BIT(5))
#define RMT_CH1_ERR_INT_RAW_M  (BIT(5))
#define RMT_CH1_ERR_INT_RAW_V  0x1
#define RMT_CH1_ERR_INT_RAW_S  5
/* RMT_CH1_RX_END_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_END_INT_RAW  (BIT(4))
#define RMT_CH1_RX_END_INT_RAW_M  (BIT(4))
#define RMT_CH1_RX_END_INT_RAW_V  0x1
#define RMT_CH1_RX_END_INT_RAW_S  4
/* RMT_CH1_TX_END_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_END_INT_RAW  (BIT(3))
#define RMT_CH1_TX_END_INT_RAW_M  (BIT(3))
#define RMT_CH1_TX_END_INT_RAW_V  0x1
#define RMT_CH1_TX_END_INT_RAW_S  3
/* RMT_CH0_ERR_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_ERR_INT_RAW  (BIT(2))
#define RMT_CH0_ERR_INT_RAW_M  (BIT(2))
#define RMT_CH0_ERR_INT_RAW_V  0x1
#define RMT_CH0_ERR_INT_RAW_S  2
/* RMT_CH0_RX_END_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_END_INT_RAW  (BIT(1))
#define RMT_CH0_RX_END_INT_RAW_M  (BIT(1))
#define RMT_CH0_RX_END_INT_RAW_V  0x1
#define RMT_CH0_RX_END_INT_RAW_S  1
/* RMT_CH0_TX_END_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_END_INT_RAW  (BIT(0))
#define RMT_CH0_TX_END_INT_RAW_M  (BIT(0))
#define RMT_CH0_TX_END_INT_RAW_V  0x1
#define RMT_CH0_TX_END_INT_RAW_S  0

#define RMT_INT_ST_REG          (DR_REG_RMT_BASE + 0x0054)
/* RMT_CH3_RX_THR_EVENT_INT_ST : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_THR_EVENT_INT_ST  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_ST_M  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH3_RX_THR_EVENT_INT_ST_S  23
/* RMT_CH2_RX_THR_EVENT_INT_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_THR_EVENT_INT_ST  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_ST_M  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH2_RX_THR_EVENT_INT_ST_S  22
/* RMT_CH1_RX_THR_EVENT_INT_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_THR_EVENT_INT_ST  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_ST_M  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH1_RX_THR_EVENT_INT_ST_S  21
/* RMT_CH0_RX_THR_EVENT_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_THR_EVENT_INT_ST  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_ST_M  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH0_RX_THR_EVENT_INT_ST_S  20
/* RMT_CH3_TX_LOOP_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_LOOP_INT_ST  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_ST_M  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_ST_V  0x1
#define RMT_CH3_TX_LOOP_INT_ST_S  19
/* RMT_CH2_TX_LOOP_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_LOOP_INT_ST  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_ST_M  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_ST_V  0x1
#define RMT_CH2_TX_LOOP_INT_ST_S  18
/* RMT_CH1_TX_LOOP_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_LOOP_INT_ST  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_ST_M  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_ST_V  0x1
#define RMT_CH1_TX_LOOP_INT_ST_S  17
/* RMT_CH0_TX_LOOP_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_LOOP_INT_ST  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_ST_M  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_ST_V  0x1
#define RMT_CH0_TX_LOOP_INT_ST_S  16
/* RMT_CH3_TX_THR_EVENT_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_THR_EVENT_INT_ST  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_ST_M  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_ST_S  15
/* RMT_CH2_TX_THR_EVENT_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_THR_EVENT_INT_ST  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_ST_M  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_ST_S  14
/* RMT_CH1_TX_THR_EVENT_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_THR_EVENT_INT_ST  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_ST_M  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_ST_S  13
/* RMT_CH0_TX_THR_EVENT_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_THR_EVENT_INT_ST  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_ST_M  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_ST_S  12
/* RMT_CH3_ERR_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_ERR_INT_ST  (BIT(11))
#define RMT_CH3_ERR_INT_ST_M  (BIT(11))
#define RMT_CH3_ERR_INT_ST_V  0x1
#define RMT_CH3_ERR_INT_ST_S  11
/* RMT_CH3_RX_END_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_END_INT_ST  (BIT(10))
#define RMT_CH3_RX_END_INT_ST_M  (BIT(10))
#define RMT_CH3_RX_END_INT_ST_V  0x1
#define RMT_CH3_RX_END_INT_ST_S  10
/* RMT_CH3_TX_END_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_END_INT_ST  (BIT(9))
#define RMT_CH3_TX_END_INT_ST_M  (BIT(9))
#define RMT_CH3_TX_END_INT_ST_V  0x1
#define RMT_CH3_TX_END_INT_ST_S  9
/* RMT_CH2_ERR_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_ERR_INT_ST  (BIT(8))
#define RMT_CH2_ERR_INT_ST_M  (BIT(8))
#define RMT_CH2_ERR_INT_ST_V  0x1
#define RMT_CH2_ERR_INT_ST_S  8
/* RMT_CH2_RX_END_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_END_INT_ST  (BIT(7))
#define RMT_CH2_RX_END_INT_ST_M  (BIT(7))
#define RMT_CH2_RX_END_INT_ST_V  0x1
#define RMT_CH2_RX_END_INT_ST_S  7
/* RMT_CH2_TX_END_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_END_INT_ST  (BIT(6))
#define RMT_CH2_TX_END_INT_ST_M  (BIT(6))
#define RMT_CH2_TX_END_INT_ST_V  0x1
#define RMT_CH2_TX_END_INT_ST_S  6
/* RMT_CH1_ERR_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_ERR_INT_ST  (BIT(5))
#define RMT_CH1_ERR_INT_ST_M  (BIT(5))
#define RMT_CH1_ERR_INT_ST_V  0x1
#define RMT_CH1_ERR_INT_ST_S  5
/* RMT_CH1_RX_END_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_END_INT_ST  (BIT(4))
#define RMT_CH1_RX_END_INT_ST_M  (BIT(4))
#define RMT_CH1_RX_END_INT_ST_V  0x1
#define RMT_CH1_RX_END_INT_ST_S  4
/* RMT_CH1_TX_END_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_END_INT_ST  (BIT(3))
#define RMT_CH1_TX_END_INT_ST_M  (BIT(3))
#define RMT_CH1_TX_END_INT_ST_V  0x1
#define RMT_CH1_TX_END_INT_ST_S  3
/* RMT_CH0_ERR_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_ERR_INT_ST  (BIT(2))
#define RMT_CH0_ERR_INT_ST_M  (BIT(2))
#define RMT_CH0_ERR_INT_ST_V  0x1
#define RMT_CH0_ERR_INT_ST_S  2
/* RMT_CH0_RX_END_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_END_INT_ST  (BIT(1))
#define RMT_CH0_RX_END_INT_ST_M  (BIT(1))
#define RMT_CH0_RX_END_INT_ST_V  0x1
#define RMT_CH0_RX_END_INT_ST_S  1
/* RMT_CH0_TX_END_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_END_INT_ST  (BIT(0))
#define RMT_CH0_TX_END_INT_ST_M  (BIT(0))
#define RMT_CH0_TX_END_INT_ST_V  0x1
#define RMT_CH0_TX_END_INT_ST_S  0

#define RMT_INT_ENA_REG          (DR_REG_RMT_BASE + 0x0058)
/* RMT_CH3_RX_THR_EVENT_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_THR_EVENT_INT_ENA  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_ENA_M  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH3_RX_THR_EVENT_INT_ENA_S  23
/* RMT_CH2_RX_THR_EVENT_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_THR_EVENT_INT_ENA  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_ENA_M  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH2_RX_THR_EVENT_INT_ENA_S  22
/* RMT_CH1_RX_THR_EVENT_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_THR_EVENT_INT_ENA  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_ENA_M  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH1_RX_THR_EVENT_INT_ENA_S  21
/* RMT_CH0_RX_THR_EVENT_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_THR_EVENT_INT_ENA  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_ENA_M  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH0_RX_THR_EVENT_INT_ENA_S  20
/* RMT_CH3_TX_LOOP_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_LOOP_INT_ENA  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_ENA_M  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_ENA_V  0x1
#define RMT_CH3_TX_LOOP_INT_ENA_S  19
/* RMT_CH2_TX_LOOP_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_LOOP_INT_ENA  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_ENA_M  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_ENA_V  0x1
#define RMT_CH2_TX_LOOP_INT_ENA_S  18
/* RMT_CH1_TX_LOOP_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_LOOP_INT_ENA  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_ENA_M  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_ENA_V  0x1
#define RMT_CH1_TX_LOOP_INT_ENA_S  17
/* RMT_CH0_TX_LOOP_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_LOOP_INT_ENA  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_ENA_M  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_ENA_V  0x1
#define RMT_CH0_TX_LOOP_INT_ENA_S  16
/* RMT_CH3_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_THR_EVENT_INT_ENA  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_ENA_M  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_ENA_S  15
/* RMT_CH2_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_THR_EVENT_INT_ENA  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_ENA_M  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_ENA_S  14
/* RMT_CH1_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_THR_EVENT_INT_ENA  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_ENA_M  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_ENA_S  13
/* RMT_CH0_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_THR_EVENT_INT_ENA  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_ENA_M  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_ENA_S  12
/* RMT_CH3_ERR_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_ERR_INT_ENA  (BIT(11))
#define RMT_CH3_ERR_INT_ENA_M  (BIT(11))
#define RMT_CH3_ERR_INT_ENA_V  0x1
#define RMT_CH3_ERR_INT_ENA_S  11
/* RMT_CH3_RX_END_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_END_INT_ENA  (BIT(10))
#define RMT_CH3_RX_END_INT_ENA_M  (BIT(10))
#define RMT_CH3_RX_END_INT_ENA_V  0x1
#define RMT_CH3_RX_END_INT_ENA_S  10
/* RMT_CH3_TX_END_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_END_INT_ENA  (BIT(9))
#define RMT_CH3_TX_END_INT_ENA_M  (BIT(9))
#define RMT_CH3_TX_END_INT_ENA_V  0x1
#define RMT_CH3_TX_END_INT_ENA_S  9
/* RMT_CH2_ERR_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_ERR_INT_ENA  (BIT(8))
#define RMT_CH2_ERR_INT_ENA_M  (BIT(8))
#define RMT_CH2_ERR_INT_ENA_V  0x1
#define RMT_CH2_ERR_INT_ENA_S  8
/* RMT_CH2_RX_END_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_END_INT_ENA  (BIT(7))
#define RMT_CH2_RX_END_INT_ENA_M  (BIT(7))
#define RMT_CH2_RX_END_INT_ENA_V  0x1
#define RMT_CH2_RX_END_INT_ENA_S  7
/* RMT_CH2_TX_END_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_END_INT_ENA  (BIT(6))
#define RMT_CH2_TX_END_INT_ENA_M  (BIT(6))
#define RMT_CH2_TX_END_INT_ENA_V  0x1
#define RMT_CH2_TX_END_INT_ENA_S  6
/* RMT_CH1_ERR_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_ERR_INT_ENA  (BIT(5))
#define RMT_CH1_ERR_INT_ENA_M  (BIT(5))
#define RMT_CH1_ERR_INT_ENA_V  0x1
#define RMT_CH1_ERR_INT_ENA_S  5
/* RMT_CH1_RX_END_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_END_INT_ENA  (BIT(4))
#define RMT_CH1_RX_END_INT_ENA_M  (BIT(4))
#define RMT_CH1_RX_END_INT_ENA_V  0x1
#define RMT_CH1_RX_END_INT_ENA_S  4
/* RMT_CH1_TX_END_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_END_INT_ENA  (BIT(3))
#define RMT_CH1_TX_END_INT_ENA_M  (BIT(3))
#define RMT_CH1_TX_END_INT_ENA_V  0x1
#define RMT_CH1_TX_END_INT_ENA_S  3
/* RMT_CH0_ERR_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_ERR_INT_ENA  (BIT(2))
#define RMT_CH0_ERR_INT_ENA_M  (BIT(2))
#define RMT_CH0_ERR_INT_ENA_V  0x1
#define RMT_CH0_ERR_INT_ENA_S  2
/* RMT_CH0_RX_END_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_END_INT_ENA  (BIT(1))
#define RMT_CH0_RX_END_INT_ENA_M  (BIT(1))
#define RMT_CH0_RX_END_INT_ENA_V  0x1
#define RMT_CH0_RX_END_INT_ENA_S  1
/* RMT_CH0_TX_END_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_END_INT_ENA  (BIT(0))
#define RMT_CH0_TX_END_INT_ENA_M  (BIT(0))
#define RMT_CH0_TX_END_INT_ENA_V  0x1
#define RMT_CH0_TX_END_INT_ENA_S  0

#define RMT_INT_CLR_REG          (DR_REG_RMT_BASE + 0x005c)
/* RMT_CH3_RX_THR_EVENT_INT_CLR : WO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_THR_EVENT_INT_CLR  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_CLR_M  (BIT(23))
#define RMT_CH3_RX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH3_RX_THR_EVENT_INT_CLR_S  23
/* RMT_CH2_RX_THR_EVENT_INT_CLR : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_THR_EVENT_INT_CLR  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_CLR_M  (BIT(22))
#define RMT_CH2_RX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH2_RX_THR_EVENT_INT_CLR_S  22
/* RMT_CH1_RX_THR_EVENT_INT_CLR : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_THR_EVENT_INT_CLR  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_CLR_M  (BIT(21))
#define RMT_CH1_RX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH1_RX_THR_EVENT_INT_CLR_S  21
/* RMT_CH0_RX_THR_EVENT_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_THR_EVENT_INT_CLR  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_CLR_M  (BIT(20))
#define RMT_CH0_RX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH0_RX_THR_EVENT_INT_CLR_S  20
/* RMT_CH3_TX_LOOP_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_LOOP_INT_CLR  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_CLR_M  (BIT(19))
#define RMT_CH3_TX_LOOP_INT_CLR_V  0x1
#define RMT_CH3_TX_LOOP_INT_CLR_S  19
/* RMT_CH2_TX_LOOP_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_LOOP_INT_CLR  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_CLR_M  (BIT(18))
#define RMT_CH2_TX_LOOP_INT_CLR_V  0x1
#define RMT_CH2_TX_LOOP_INT_CLR_S  18
/* RMT_CH1_TX_LOOP_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_LOOP_INT_CLR  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_CLR_M  (BIT(17))
#define RMT_CH1_TX_LOOP_INT_CLR_V  0x1
#define RMT_CH1_TX_LOOP_INT_CLR_S  17
/* RMT_CH0_TX_LOOP_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_LOOP_INT_CLR  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_CLR_M  (BIT(16))
#define RMT_CH0_TX_LOOP_INT_CLR_V  0x1
#define RMT_CH0_TX_LOOP_INT_CLR_S  16
/* RMT_CH3_TX_THR_EVENT_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_THR_EVENT_INT_CLR  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_CLR_M  (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_CLR_S  15
/* RMT_CH2_TX_THR_EVENT_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_THR_EVENT_INT_CLR  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_CLR_M  (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_CLR_S  14
/* RMT_CH1_TX_THR_EVENT_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_THR_EVENT_INT_CLR  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_CLR_M  (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_CLR_S  13
/* RMT_CH0_TX_THR_EVENT_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_THR_EVENT_INT_CLR  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_CLR_M  (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_CLR_S  12
/* RMT_CH3_ERR_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_ERR_INT_CLR  (BIT(11))
#define RMT_CH3_ERR_INT_CLR_M  (BIT(11))
#define RMT_CH3_ERR_INT_CLR_V  0x1
#define RMT_CH3_ERR_INT_CLR_S  11
/* RMT_CH3_RX_END_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_RX_END_INT_CLR  (BIT(10))
#define RMT_CH3_RX_END_INT_CLR_M  (BIT(10))
#define RMT_CH3_RX_END_INT_CLR_V  0x1
#define RMT_CH3_RX_END_INT_CLR_S  10
/* RMT_CH3_TX_END_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define RMT_CH3_TX_END_INT_CLR  (BIT(9))
#define RMT_CH3_TX_END_INT_CLR_M  (BIT(9))
#define RMT_CH3_TX_END_INT_CLR_V  0x1
#define RMT_CH3_TX_END_INT_CLR_S  9
/* RMT_CH2_ERR_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_ERR_INT_CLR  (BIT(8))
#define RMT_CH2_ERR_INT_CLR_M  (BIT(8))
#define RMT_CH2_ERR_INT_CLR_V  0x1
#define RMT_CH2_ERR_INT_CLR_S  8
/* RMT_CH2_RX_END_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_RX_END_INT_CLR  (BIT(7))
#define RMT_CH2_RX_END_INT_CLR_M  (BIT(7))
#define RMT_CH2_RX_END_INT_CLR_V  0x1
#define RMT_CH2_RX_END_INT_CLR_S  7
/* RMT_CH2_TX_END_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RMT_CH2_TX_END_INT_CLR  (BIT(6))
#define RMT_CH2_TX_END_INT_CLR_M  (BIT(6))
#define RMT_CH2_TX_END_INT_CLR_V  0x1
#define RMT_CH2_TX_END_INT_CLR_S  6
/* RMT_CH1_ERR_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_ERR_INT_CLR  (BIT(5))
#define RMT_CH1_ERR_INT_CLR_M  (BIT(5))
#define RMT_CH1_ERR_INT_CLR_V  0x1
#define RMT_CH1_ERR_INT_CLR_S  5
/* RMT_CH1_RX_END_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_RX_END_INT_CLR  (BIT(4))
#define RMT_CH1_RX_END_INT_CLR_M  (BIT(4))
#define RMT_CH1_RX_END_INT_CLR_V  0x1
#define RMT_CH1_RX_END_INT_CLR_S  4
/* RMT_CH1_TX_END_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_CH1_TX_END_INT_CLR  (BIT(3))
#define RMT_CH1_TX_END_INT_CLR_M  (BIT(3))
#define RMT_CH1_TX_END_INT_CLR_V  0x1
#define RMT_CH1_TX_END_INT_CLR_S  3
/* RMT_CH0_ERR_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_ERR_INT_CLR  (BIT(2))
#define RMT_CH0_ERR_INT_CLR_M  (BIT(2))
#define RMT_CH0_ERR_INT_CLR_V  0x1
#define RMT_CH0_ERR_INT_CLR_S  2
/* RMT_CH0_RX_END_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_RX_END_INT_CLR  (BIT(1))
#define RMT_CH0_RX_END_INT_CLR_M  (BIT(1))
#define RMT_CH0_RX_END_INT_CLR_V  0x1
#define RMT_CH0_RX_END_INT_CLR_S  1
/* RMT_CH0_TX_END_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RMT_CH0_TX_END_INT_CLR  (BIT(0))
#define RMT_CH0_TX_END_INT_CLR_M  (BIT(0))
#define RMT_CH0_TX_END_INT_CLR_V  0x1
#define RMT_CH0_TX_END_INT_CLR_S  0

#define RMT_CH0CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x0060)
/* RMT_CARRIER_HIGH_CH0 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_HIGH_CH0  0x0000FFFF
#define RMT_CARRIER_HIGH_CH0_M  ((RMT_CARRIER_HIGH_CH0_V)<<(RMT_CARRIER_HIGH_CH0_S))
#define RMT_CARRIER_HIGH_CH0_V  0xFFFF
#define RMT_CARRIER_HIGH_CH0_S  16
/* RMT_CARRIER_LOW_CH0 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_LOW_CH0  0x0000FFFF
#define RMT_CARRIER_LOW_CH0_M  ((RMT_CARRIER_LOW_CH0_V)<<(RMT_CARRIER_LOW_CH0_S))
#define RMT_CARRIER_LOW_CH0_V  0xFFFF
#define RMT_CARRIER_LOW_CH0_S  0

#define RMT_CH1CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x0064)
/* RMT_CARRIER_HIGH_CH1 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_HIGH_CH1  0x0000FFFF
#define RMT_CARRIER_HIGH_CH1_M  ((RMT_CARRIER_HIGH_CH1_V)<<(RMT_CARRIER_HIGH_CH1_S))
#define RMT_CARRIER_HIGH_CH1_V  0xFFFF
#define RMT_CARRIER_HIGH_CH1_S  16
/* RMT_CARRIER_LOW_CH1 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_LOW_CH1  0x0000FFFF
#define RMT_CARRIER_LOW_CH1_M  ((RMT_CARRIER_LOW_CH1_V)<<(RMT_CARRIER_LOW_CH1_S))
#define RMT_CARRIER_LOW_CH1_V  0xFFFF
#define RMT_CARRIER_LOW_CH1_S  0

#define RMT_CH2CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x0068)
/* RMT_CARRIER_HIGH_CH2 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_HIGH_CH2  0x0000FFFF
#define RMT_CARRIER_HIGH_CH2_M  ((RMT_CARRIER_HIGH_CH2_V)<<(RMT_CARRIER_HIGH_CH2_S))
#define RMT_CARRIER_HIGH_CH2_V  0xFFFF
#define RMT_CARRIER_HIGH_CH2_S  16
/* RMT_CARRIER_LOW_CH2 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_LOW_CH2  0x0000FFFF
#define RMT_CARRIER_LOW_CH2_M  ((RMT_CARRIER_LOW_CH2_V)<<(RMT_CARRIER_LOW_CH2_S))
#define RMT_CARRIER_LOW_CH2_V  0xFFFF
#define RMT_CARRIER_LOW_CH2_S  0

#define RMT_CH3CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x006c)
/* RMT_CARRIER_HIGH_CH3 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_HIGH_CH3  0x0000FFFF
#define RMT_CARRIER_HIGH_CH3_M  ((RMT_CARRIER_HIGH_CH3_V)<<(RMT_CARRIER_HIGH_CH3_S))
#define RMT_CARRIER_HIGH_CH3_V  0xFFFF
#define RMT_CARRIER_HIGH_CH3_S  16
/* RMT_CARRIER_LOW_CH3 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: */
#define RMT_CARRIER_LOW_CH3  0x0000FFFF
#define RMT_CARRIER_LOW_CH3_M  ((RMT_CARRIER_LOW_CH3_V)<<(RMT_CARRIER_LOW_CH3_S))
#define RMT_CARRIER_LOW_CH3_V  0xFFFF
#define RMT_CARRIER_LOW_CH3_S  0

#define RMT_CH0_TX_LIM_REG          (DR_REG_RMT_BASE + 0x0070)
/* RMT_RX_LIM_CH0 : R/W ;bitpos:[29:21] ;default: 9'h80 ; */
/*description: */
#define RMT_RX_LIM_CH0  0x000001FF
#define RMT_RX_LIM_CH0_M  ((RMT_RX_LIM_CH0_V)<<(RMT_RX_LIM_CH0_S))
#define RMT_RX_LIM_CH0_V  0x1FF
#define RMT_RX_LIM_CH0_S  21
/* RMT_LOOP_COUNT_RESET_CH0 : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_LOOP_COUNT_RESET_CH0  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH0_M  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH0_V  0x1
#define RMT_LOOP_COUNT_RESET_CH0_S  20
/* RMT_TX_LOOP_CNT_EN_CH0 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_LOOP_CNT_EN_CH0  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH0_M  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH0_V  0x1
#define RMT_TX_LOOP_CNT_EN_CH0_S  19
/* RMT_TX_LOOP_NUM_CH0 : R/W ;bitpos:[18:9] ;default: 10'b0 ; */
/*description: */
#define RMT_TX_LOOP_NUM_CH0  0x000003FF
#define RMT_TX_LOOP_NUM_CH0_M  ((RMT_TX_LOOP_NUM_CH0_V)<<(RMT_TX_LOOP_NUM_CH0_S))
#define RMT_TX_LOOP_NUM_CH0_V  0x3FF
#define RMT_TX_LOOP_NUM_CH0_S  9
/* RMT_TX_LIM_CH0 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: */
#define RMT_TX_LIM_CH0  0x000001FF
#define RMT_TX_LIM_CH0_M  ((RMT_TX_LIM_CH0_V)<<(RMT_TX_LIM_CH0_S))
#define RMT_TX_LIM_CH0_V  0x1FF
#define RMT_TX_LIM_CH0_S  0

#define RMT_CH1_TX_LIM_REG          (DR_REG_RMT_BASE + 0x0074)
/* RMT_RX_LIM_CH1 : R/W ;bitpos:[29:21] ;default: 9'h80 ; */
/*description: */
#define RMT_RX_LIM_CH1  0x000001FF
#define RMT_RX_LIM_CH1_M  ((RMT_RX_LIM_CH1_V)<<(RMT_RX_LIM_CH1_S))
#define RMT_RX_LIM_CH1_V  0x1FF
#define RMT_RX_LIM_CH1_S  21
/* RMT_LOOP_COUNT_RESET_CH1 : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_LOOP_COUNT_RESET_CH1  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH1_M  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH1_V  0x1
#define RMT_LOOP_COUNT_RESET_CH1_S  20
/* RMT_TX_LOOP_CNT_EN_CH1 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_LOOP_CNT_EN_CH1  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH1_M  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH1_V  0x1
#define RMT_TX_LOOP_CNT_EN_CH1_S  19
/* RMT_TX_LOOP_NUM_CH1 : R/W ;bitpos:[18:9] ;default: 10'b0 ; */
/*description: */
#define RMT_TX_LOOP_NUM_CH1  0x000003FF
#define RMT_TX_LOOP_NUM_CH1_M  ((RMT_TX_LOOP_NUM_CH1_V)<<(RMT_TX_LOOP_NUM_CH1_S))
#define RMT_TX_LOOP_NUM_CH1_V  0x3FF
#define RMT_TX_LOOP_NUM_CH1_S  9
/* RMT_TX_LIM_CH1 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: */
#define RMT_TX_LIM_CH1  0x000001FF
#define RMT_TX_LIM_CH1_M  ((RMT_TX_LIM_CH1_V)<<(RMT_TX_LIM_CH1_S))
#define RMT_TX_LIM_CH1_V  0x1FF
#define RMT_TX_LIM_CH1_S  0

#define RMT_CH2_TX_LIM_REG          (DR_REG_RMT_BASE + 0x0078)
/* RMT_RX_LIM_CH2 : R/W ;bitpos:[29:21] ;default: 9'h80 ; */
/*description: */
#define RMT_RX_LIM_CH2  0x000001FF
#define RMT_RX_LIM_CH2_M  ((RMT_RX_LIM_CH2_V)<<(RMT_RX_LIM_CH2_S))
#define RMT_RX_LIM_CH2_V  0x1FF
#define RMT_RX_LIM_CH2_S  21
/* RMT_LOOP_COUNT_RESET_CH2 : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_LOOP_COUNT_RESET_CH2  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH2_M  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH2_V  0x1
#define RMT_LOOP_COUNT_RESET_CH2_S  20
/* RMT_TX_LOOP_CNT_EN_CH2 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_LOOP_CNT_EN_CH2  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH2_M  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH2_V  0x1
#define RMT_TX_LOOP_CNT_EN_CH2_S  19
/* RMT_TX_LOOP_NUM_CH2 : R/W ;bitpos:[18:9] ;default: 10'b0 ; */
/*description: */
#define RMT_TX_LOOP_NUM_CH2  0x000003FF
#define RMT_TX_LOOP_NUM_CH2_M  ((RMT_TX_LOOP_NUM_CH2_V)<<(RMT_TX_LOOP_NUM_CH2_S))
#define RMT_TX_LOOP_NUM_CH2_V  0x3FF
#define RMT_TX_LOOP_NUM_CH2_S  9
/* RMT_TX_LIM_CH2 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: */
#define RMT_TX_LIM_CH2  0x000001FF
#define RMT_TX_LIM_CH2_M  ((RMT_TX_LIM_CH2_V)<<(RMT_TX_LIM_CH2_S))
#define RMT_TX_LIM_CH2_V  0x1FF
#define RMT_TX_LIM_CH2_S  0

#define RMT_CH3_TX_LIM_REG          (DR_REG_RMT_BASE + 0x007c)
/* RMT_RX_LIM_CH3 : R/W ;bitpos:[29:21] ;default: 9'h80 ; */
/*description: */
#define RMT_RX_LIM_CH3  0x000001FF
#define RMT_RX_LIM_CH3_M  ((RMT_RX_LIM_CH3_V)<<(RMT_RX_LIM_CH3_S))
#define RMT_RX_LIM_CH3_V  0x1FF
#define RMT_RX_LIM_CH3_S  21
/* RMT_LOOP_COUNT_RESET_CH3 : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RMT_LOOP_COUNT_RESET_CH3  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH3_M  (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH3_V  0x1
#define RMT_LOOP_COUNT_RESET_CH3_S  20
/* RMT_TX_LOOP_CNT_EN_CH3 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_LOOP_CNT_EN_CH3  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH3_M  (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH3_V  0x1
#define RMT_TX_LOOP_CNT_EN_CH3_S  19
/* RMT_TX_LOOP_NUM_CH3 : R/W ;bitpos:[18:9] ;default: 10'b0 ; */
/*description: */
#define RMT_TX_LOOP_NUM_CH3  0x000003FF
#define RMT_TX_LOOP_NUM_CH3_M  ((RMT_TX_LOOP_NUM_CH3_V)<<(RMT_TX_LOOP_NUM_CH3_S))
#define RMT_TX_LOOP_NUM_CH3_V  0x3FF
#define RMT_TX_LOOP_NUM_CH3_S  9
/* RMT_TX_LIM_CH3 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: */
#define RMT_TX_LIM_CH3  0x000001FF
#define RMT_TX_LIM_CH3_M  ((RMT_TX_LIM_CH3_V)<<(RMT_TX_LIM_CH3_S))
#define RMT_TX_LIM_CH3_V  0x1FF
#define RMT_TX_LIM_CH3_S  0

#define RMT_APB_CONF_REG          (DR_REG_RMT_BASE + 0x0080)
/* RMT_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define RMT_CLK_EN  (BIT(31))
#define RMT_CLK_EN_M  (BIT(31))
#define RMT_CLK_EN_V  0x1
#define RMT_CLK_EN_S  31
/* RMT_MEM_FORCE_PU : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_FORCE_PU  (BIT(4))
#define RMT_MEM_FORCE_PU_M  (BIT(4))
#define RMT_MEM_FORCE_PU_V  0x1
#define RMT_MEM_FORCE_PU_S  4
/* RMT_MEM_FORCE_PD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_FORCE_PD  (BIT(3))
#define RMT_MEM_FORCE_PD_M  (BIT(3))
#define RMT_MEM_FORCE_PD_V  0x1
#define RMT_MEM_FORCE_PD_S  3
/* RMT_MEM_CLK_FORCE_ON : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define RMT_MEM_CLK_FORCE_ON  (BIT(2))
#define RMT_MEM_CLK_FORCE_ON_M  (BIT(2))
#define RMT_MEM_CLK_FORCE_ON_V  0x1
#define RMT_MEM_CLK_FORCE_ON_S  2
/* RMT_MEM_TX_WRAP_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RMT_MEM_TX_WRAP_EN  (BIT(1))
#define RMT_MEM_TX_WRAP_EN_M  (BIT(1))
#define RMT_MEM_TX_WRAP_EN_V  0x1
#define RMT_MEM_TX_WRAP_EN_S  1
/* RMT_APB_FIFO_MASK : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define RMT_APB_FIFO_MASK  (BIT(0))
#define RMT_APB_FIFO_MASK_M  (BIT(0))
#define RMT_APB_FIFO_MASK_V  0x1
#define RMT_APB_FIFO_MASK_S  0

#define RMT_TX_SIM_REG          (DR_REG_RMT_BASE + 0x0084)
/* RMT_TX_SIM_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_SIM_EN  (BIT(4))
#define RMT_TX_SIM_EN_M  (BIT(4))
#define RMT_TX_SIM_EN_V  0x1
#define RMT_TX_SIM_EN_S  4
/* RMT_TX_SIM_CH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_SIM_CH3  (BIT(3))
#define RMT_TX_SIM_CH3_M  (BIT(3))
#define RMT_TX_SIM_CH3_V  0x1
#define RMT_TX_SIM_CH3_S  3
/* RMT_TX_SIM_CH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_SIM_CH2  (BIT(2))
#define RMT_TX_SIM_CH2_M  (BIT(2))
#define RMT_TX_SIM_CH2_V  0x1
#define RMT_TX_SIM_CH2_S  2
/* RMT_TX_SIM_CH1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_SIM_CH1  (BIT(1))
#define RMT_TX_SIM_CH1_M  (BIT(1))
#define RMT_TX_SIM_CH1_V  0x1
#define RMT_TX_SIM_CH1_S  1
/* RMT_TX_SIM_CH0 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RMT_TX_SIM_CH0  (BIT(0))
#define RMT_TX_SIM_CH0_M  (BIT(0))
#define RMT_TX_SIM_CH0_V  0x1
#define RMT_TX_SIM_CH0_S  0

#define RMT_REF_CNT_RST_REG          (DR_REG_RMT_BASE + 0x0088)
/* RMT_REF_CNT_RST_CH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_CNT_RST_CH3  (BIT(3))
#define RMT_REF_CNT_RST_CH3_M  (BIT(3))
#define RMT_REF_CNT_RST_CH3_V  0x1
#define RMT_REF_CNT_RST_CH3_S  3
/* RMT_REF_CNT_RST_CH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_CNT_RST_CH2  (BIT(2))
#define RMT_REF_CNT_RST_CH2_M  (BIT(2))
#define RMT_REF_CNT_RST_CH2_V  0x1
#define RMT_REF_CNT_RST_CH2_S  2
/* RMT_REF_CNT_RST_CH1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_CNT_RST_CH1  (BIT(1))
#define RMT_REF_CNT_RST_CH1_M  (BIT(1))
#define RMT_REF_CNT_RST_CH1_V  0x1
#define RMT_REF_CNT_RST_CH1_S  1
/* RMT_REF_CNT_RST_CH0 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RMT_REF_CNT_RST_CH0  (BIT(0))
#define RMT_REF_CNT_RST_CH0_M  (BIT(0))
#define RMT_REF_CNT_RST_CH0_V  0x1
#define RMT_REF_CNT_RST_CH0_S  0

#define RMT_CH0_RX_CARRIER_RM_REG          (DR_REG_RMT_BASE + 0x008c)
/* RMT_CARRIER_HIGH_THRES_CH0 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_HIGH_THRES_CH0  0x0000FFFF
#define RMT_CARRIER_HIGH_THRES_CH0_M  ((RMT_CARRIER_HIGH_THRES_CH0_V)<<(RMT_CARRIER_HIGH_THRES_CH0_S))
#define RMT_CARRIER_HIGH_THRES_CH0_V  0xFFFF
#define RMT_CARRIER_HIGH_THRES_CH0_S  16
/* RMT_CARRIER_LOW_THRES_CH0 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_LOW_THRES_CH0  0x0000FFFF
#define RMT_CARRIER_LOW_THRES_CH0_M  ((RMT_CARRIER_LOW_THRES_CH0_V)<<(RMT_CARRIER_LOW_THRES_CH0_S))
#define RMT_CARRIER_LOW_THRES_CH0_V  0xFFFF
#define RMT_CARRIER_LOW_THRES_CH0_S  0

#define RMT_CH1_RX_CARRIER_RM_REG          (DR_REG_RMT_BASE + 0x0090)
/* RMT_CARRIER_HIGH_THRES_CH1 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_HIGH_THRES_CH1  0x0000FFFF
#define RMT_CARRIER_HIGH_THRES_CH1_M  ((RMT_CARRIER_HIGH_THRES_CH1_V)<<(RMT_CARRIER_HIGH_THRES_CH1_S))
#define RMT_CARRIER_HIGH_THRES_CH1_V  0xFFFF
#define RMT_CARRIER_HIGH_THRES_CH1_S  16
/* RMT_CARRIER_LOW_THRES_CH1 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_LOW_THRES_CH1  0x0000FFFF
#define RMT_CARRIER_LOW_THRES_CH1_M  ((RMT_CARRIER_LOW_THRES_CH1_V)<<(RMT_CARRIER_LOW_THRES_CH1_S))
#define RMT_CARRIER_LOW_THRES_CH1_V  0xFFFF
#define RMT_CARRIER_LOW_THRES_CH1_S  0

#define RMT_CH2_RX_CARRIER_RM_REG          (DR_REG_RMT_BASE + 0x0094)
/* RMT_CARRIER_HIGH_THRES_CH2 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_HIGH_THRES_CH2  0x0000FFFF
#define RMT_CARRIER_HIGH_THRES_CH2_M  ((RMT_CARRIER_HIGH_THRES_CH2_V)<<(RMT_CARRIER_HIGH_THRES_CH2_S))
#define RMT_CARRIER_HIGH_THRES_CH2_V  0xFFFF
#define RMT_CARRIER_HIGH_THRES_CH2_S  16
/* RMT_CARRIER_LOW_THRES_CH2 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_LOW_THRES_CH2  0x0000FFFF
#define RMT_CARRIER_LOW_THRES_CH2_M  ((RMT_CARRIER_LOW_THRES_CH2_V)<<(RMT_CARRIER_LOW_THRES_CH2_S))
#define RMT_CARRIER_LOW_THRES_CH2_V  0xFFFF
#define RMT_CARRIER_LOW_THRES_CH2_S  0

#define RMT_CH3_RX_CARRIER_RM_REG          (DR_REG_RMT_BASE + 0x0098)
/* RMT_CARRIER_HIGH_THRES_CH3 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_HIGH_THRES_CH3  0x0000FFFF
#define RMT_CARRIER_HIGH_THRES_CH3_M  ((RMT_CARRIER_HIGH_THRES_CH3_V)<<(RMT_CARRIER_HIGH_THRES_CH3_S))
#define RMT_CARRIER_HIGH_THRES_CH3_V  0xFFFF
#define RMT_CARRIER_HIGH_THRES_CH3_S  16
/* RMT_CARRIER_LOW_THRES_CH3 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define RMT_CARRIER_LOW_THRES_CH3  0x0000FFFF
#define RMT_CARRIER_LOW_THRES_CH3_M  ((RMT_CARRIER_LOW_THRES_CH3_V)<<(RMT_CARRIER_LOW_THRES_CH3_S))
#define RMT_CARRIER_LOW_THRES_CH3_V  0xFFFF
#define RMT_CARRIER_LOW_THRES_CH3_S  0

#define RMT_DATE_REG          (DR_REG_RMT_BASE + 0x0fc)
/* RMT_DATE : R/W ;bitpos:[31:0] ;default: 32'h19080700 ; */
/*description: */
#define RMT_DATE  0xFFFFFFFF
#define RMT_DATE_M  ((RMT_DATE_V)<<(RMT_DATE_S))
#define RMT_DATE_V  0xFFFFFFFF
#define RMT_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_RMT_REG_H_ */
