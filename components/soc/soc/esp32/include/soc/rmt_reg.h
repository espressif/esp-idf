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
#ifndef _SOC_RMT_REG_H_
#define _SOC_RMT_REG_H_

#include "soc.h"
#define RMT_CH0DATA_REG          (DR_REG_RMT_BASE + 0x0000)

#define RMT_CH1DATA_REG          (DR_REG_RMT_BASE + 0x0004)

#define RMT_CH2DATA_REG          (DR_REG_RMT_BASE + 0x0008)

#define RMT_CH3DATA_REG          (DR_REG_RMT_BASE + 0x000c)

#define RMT_CH4DATA_REG          (DR_REG_RMT_BASE + 0x0010)

#define RMT_CH5DATA_REG          (DR_REG_RMT_BASE + 0x0014)

#define RMT_CH6DATA_REG          (DR_REG_RMT_BASE + 0x0018)

#define RMT_CH7DATA_REG          (DR_REG_RMT_BASE + 0x001c)

#define RMT_CH0CONF0_REG          (DR_REG_RMT_BASE + 0x0020)
/* RMT_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: This bit  is used  to control clock.when software config RMT
 internal registers  it controls the register clock.*/
#define RMT_CLK_EN  (BIT(31))
#define RMT_CLK_EN_M  (BIT(31))
#define RMT_CLK_EN_V  0x1
#define RMT_CLK_EN_S  31
/* RMT_MEM_PD : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: This bit is used to reduce power consumed by mem. 1:mem is in low power state.*/
#define RMT_MEM_PD  (BIT(30))
#define RMT_MEM_PD_M  (BIT(30))
#define RMT_MEM_PD_V  0x1
#define RMT_MEM_PD_S  30
/* RMT_CARRIER_OUT_LV_CH0 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure the way carrier wave is modulated
 for  channel0.1'b1:transmit on low output level  1'b0:transmit  on high output level.*/
#define RMT_CARRIER_OUT_LV_CH0  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_V  0x1
#define RMT_CARRIER_OUT_LV_CH0_S  29
/* RMT_CARRIER_EN_CH0 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel0.*/
#define RMT_CARRIER_EN_CH0  (BIT(28))
#define RMT_CARRIER_EN_CH0_M  (BIT(28))
#define RMT_CARRIER_EN_CH0_V  0x1
#define RMT_CARRIER_EN_CH0_S  28
/* RMT_MEM_SIZE_CH0 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel0.*/
#define RMT_MEM_SIZE_CH0  0x0000000F
#define RMT_MEM_SIZE_CH0_M  ((RMT_MEM_SIZE_CH0_V)<<(RMT_MEM_SIZE_CH0_S))
#define RMT_MEM_SIZE_CH0_V  0xF
#define RMT_MEM_SIZE_CH0_S  24
/* RMT_IDLE_THRES_CH0 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: In receive mode when no edge is detected on the input signal
 for longer than reg_idle_thres_ch0 then the receive process is done.*/
#define RMT_IDLE_THRES_CH0  0x0000FFFF
#define RMT_IDLE_THRES_CH0_M  ((RMT_IDLE_THRES_CH0_V)<<(RMT_IDLE_THRES_CH0_S))
#define RMT_IDLE_THRES_CH0_V  0xFFFF
#define RMT_IDLE_THRES_CH0_S  8
/* RMT_DIV_CNT_CH0 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel0.*/
#define RMT_DIV_CNT_CH0  0x000000FF
#define RMT_DIV_CNT_CH0_M  ((RMT_DIV_CNT_CH0_V)<<(RMT_DIV_CNT_CH0_S))
#define RMT_DIV_CNT_CH0_V  0xFF
#define RMT_DIV_CNT_CH0_S  0

#define RMT_CH0CONF1_REG          (DR_REG_RMT_BASE + 0x0024)
/* RMT_IDLE_OUT_EN_CH0 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel0 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH0  (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_V  0x1
#define RMT_IDLE_OUT_EN_CH0_S  19
/* RMT_IDLE_OUT_LV_CH0 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel0 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH0  (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_V  0x1
#define RMT_IDLE_OUT_LV_CH0_S  18
/* RMT_REF_ALWAYS_ON_CH0 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH0  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_V  0x1
#define RMT_REF_ALWAYS_ON_CH0_S  17
/* RMT_REF_CNT_RST_CH0 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel0.*/
#define RMT_REF_CNT_RST_CH0  (BIT(16))
#define RMT_REF_CNT_RST_CH0_M  (BIT(16))
#define RMT_REF_CNT_RST_CH0_V  0x1
#define RMT_REF_CNT_RST_CH0_S  16
/* RMT_RX_FILTER_THRES_CH0 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel0 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH0  0x000000FF
#define RMT_RX_FILTER_THRES_CH0_M  ((RMT_RX_FILTER_THRES_CH0_V)<<(RMT_RX_FILTER_THRES_CH0_S))
#define RMT_RX_FILTER_THRES_CH0_V  0xFF
#define RMT_RX_FILTER_THRES_CH0_S  8
/* RMT_RX_FILTER_EN_CH0 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel0.*/
#define RMT_RX_FILTER_EN_CH0  (BIT(7))
#define RMT_RX_FILTER_EN_CH0_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH0_V  0x1
#define RMT_RX_FILTER_EN_CH0_S  7
/* RMT_TX_CONTI_MODE_CH0 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel0 again and again.*/
#define RMT_TX_CONTI_MODE_CH0  (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_V  0x1
#define RMT_TX_CONTI_MODE_CH0_S  6
/* RMT_MEM_OWNER_CH0 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel0's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH0  (BIT(5))
#define RMT_MEM_OWNER_CH0_M  (BIT(5))
#define RMT_MEM_OWNER_CH0_V  0x1
#define RMT_MEM_OWNER_CH0_S  5
/* RMT_APB_MEM_RST_CH0 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel0 by apb fifo access*/
#define RMT_APB_MEM_RST_CH0  (BIT(4))
#define RMT_APB_MEM_RST_CH0_M  (BIT(4))
#define RMT_APB_MEM_RST_CH0_V  0x1
#define RMT_APB_MEM_RST_CH0_S  4
/* RMT_MEM_RD_RST_CH0 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel0 by transmitter access.*/
#define RMT_MEM_RD_RST_CH0  (BIT(3))
#define RMT_MEM_RD_RST_CH0_M  (BIT(3))
#define RMT_MEM_RD_RST_CH0_V  0x1
#define RMT_MEM_RD_RST_CH0_S  3
/* RMT_MEM_WR_RST_CH0 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel0 by receiver access.*/
#define RMT_MEM_WR_RST_CH0  (BIT(2))
#define RMT_MEM_WR_RST_CH0_M  (BIT(2))
#define RMT_MEM_WR_RST_CH0_V  0x1
#define RMT_MEM_WR_RST_CH0_S  2
/* RMT_RX_EN_CH0 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel0.*/
#define RMT_RX_EN_CH0  (BIT(1))
#define RMT_RX_EN_CH0_M  (BIT(1))
#define RMT_RX_EN_CH0_V  0x1
#define RMT_RX_EN_CH0_S  1
/* RMT_TX_START_CH0 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel0.*/
#define RMT_TX_START_CH0  (BIT(0))
#define RMT_TX_START_CH0_M  (BIT(0))
#define RMT_TX_START_CH0_V  0x1
#define RMT_TX_START_CH0_S  0

#define RMT_CH1CONF0_REG          (DR_REG_RMT_BASE + 0x0028)
/* RMT_CARRIER_OUT_LV_CH1 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure the way carrier wave is modulated
 for  channel1.1'b1:transmit on low output level  1'b0:transmit  on high output level.*/
#define RMT_CARRIER_OUT_LV_CH1  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH1_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH1_V  0x1
#define RMT_CARRIER_OUT_LV_CH1_S  29
/* RMT_CARRIER_EN_CH1 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel1.*/
#define RMT_CARRIER_EN_CH1  (BIT(28))
#define RMT_CARRIER_EN_CH1_M  (BIT(28))
#define RMT_CARRIER_EN_CH1_V  0x1
#define RMT_CARRIER_EN_CH1_S  28
/* RMT_MEM_SIZE_CH1 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel1.*/
#define RMT_MEM_SIZE_CH1  0x0000000F
#define RMT_MEM_SIZE_CH1_M  ((RMT_MEM_SIZE_CH1_V)<<(RMT_MEM_SIZE_CH1_S))
#define RMT_MEM_SIZE_CH1_V  0xF
#define RMT_MEM_SIZE_CH1_S  24
/* RMT_IDLE_THRES_CH1 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel1.*/
#define RMT_IDLE_THRES_CH1  0x0000FFFF
#define RMT_IDLE_THRES_CH1_M  ((RMT_IDLE_THRES_CH1_V)<<(RMT_IDLE_THRES_CH1_S))
#define RMT_IDLE_THRES_CH1_V  0xFFFF
#define RMT_IDLE_THRES_CH1_S  8
/* RMT_DIV_CNT_CH1 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel1.*/
#define RMT_DIV_CNT_CH1  0x000000FF
#define RMT_DIV_CNT_CH1_M  ((RMT_DIV_CNT_CH1_V)<<(RMT_DIV_CNT_CH1_S))
#define RMT_DIV_CNT_CH1_V  0xFF
#define RMT_DIV_CNT_CH1_S  0

#define RMT_CH1CONF1_REG          (DR_REG_RMT_BASE + 0x002c)
/* RMT_IDLE_OUT_EN_CH1 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel1 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH1  (BIT(19))
#define RMT_IDLE_OUT_EN_CH1_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH1_V  0x1
#define RMT_IDLE_OUT_EN_CH1_S  19
/* RMT_IDLE_OUT_LV_CH1 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel1 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH1  (BIT(18))
#define RMT_IDLE_OUT_LV_CH1_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH1_V  0x1
#define RMT_IDLE_OUT_LV_CH1_S  18
/* RMT_REF_ALWAYS_ON_CH1 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH1  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH1_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH1_V  0x1
#define RMT_REF_ALWAYS_ON_CH1_S  17
/* RMT_REF_CNT_RST_CH1 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel1.*/
#define RMT_REF_CNT_RST_CH1  (BIT(16))
#define RMT_REF_CNT_RST_CH1_M  (BIT(16))
#define RMT_REF_CNT_RST_CH1_V  0x1
#define RMT_REF_CNT_RST_CH1_S  16
/* RMT_RX_FILTER_THRES_CH1 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel1 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH1  0x000000FF
#define RMT_RX_FILTER_THRES_CH1_M  ((RMT_RX_FILTER_THRES_CH1_V)<<(RMT_RX_FILTER_THRES_CH1_S))
#define RMT_RX_FILTER_THRES_CH1_V  0xFF
#define RMT_RX_FILTER_THRES_CH1_S  8
/* RMT_RX_FILTER_EN_CH1 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel1.*/
#define RMT_RX_FILTER_EN_CH1  (BIT(7))
#define RMT_RX_FILTER_EN_CH1_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH1_V  0x1
#define RMT_RX_FILTER_EN_CH1_S  7
/* RMT_TX_CONTI_MODE_CH1 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel1 again and again.*/
#define RMT_TX_CONTI_MODE_CH1  (BIT(6))
#define RMT_TX_CONTI_MODE_CH1_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH1_V  0x1
#define RMT_TX_CONTI_MODE_CH1_S  6
/* RMT_MEM_OWNER_CH1 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel1's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH1  (BIT(5))
#define RMT_MEM_OWNER_CH1_M  (BIT(5))
#define RMT_MEM_OWNER_CH1_V  0x1
#define RMT_MEM_OWNER_CH1_S  5
/* RMT_APB_MEM_RST_CH1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel1 by apb fifo access*/
#define RMT_APB_MEM_RST_CH1  (BIT(4))
#define RMT_APB_MEM_RST_CH1_M  (BIT(4))
#define RMT_APB_MEM_RST_CH1_V  0x1
#define RMT_APB_MEM_RST_CH1_S  4
/* RMT_MEM_RD_RST_CH1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel1 by transmitter access.*/
#define RMT_MEM_RD_RST_CH1  (BIT(3))
#define RMT_MEM_RD_RST_CH1_M  (BIT(3))
#define RMT_MEM_RD_RST_CH1_V  0x1
#define RMT_MEM_RD_RST_CH1_S  3
/* RMT_MEM_WR_RST_CH1 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel1 by receiver access.*/
#define RMT_MEM_WR_RST_CH1  (BIT(2))
#define RMT_MEM_WR_RST_CH1_M  (BIT(2))
#define RMT_MEM_WR_RST_CH1_V  0x1
#define RMT_MEM_WR_RST_CH1_S  2
/* RMT_RX_EN_CH1 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel1.*/
#define RMT_RX_EN_CH1  (BIT(1))
#define RMT_RX_EN_CH1_M  (BIT(1))
#define RMT_RX_EN_CH1_V  0x1
#define RMT_RX_EN_CH1_S  1
/* RMT_TX_START_CH1 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel1.*/
#define RMT_TX_START_CH1  (BIT(0))
#define RMT_TX_START_CH1_M  (BIT(0))
#define RMT_TX_START_CH1_V  0x1
#define RMT_TX_START_CH1_S  0

#define RMT_CH2CONF0_REG          (DR_REG_RMT_BASE + 0x0030)
/* RMT_CARRIER_OUT_LV_CH2 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure carrier wave's position for channel2.1'b1:add
 on low level  1'b0:add  on high level.*/
#define RMT_CARRIER_OUT_LV_CH2  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH2_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH2_V  0x1
#define RMT_CARRIER_OUT_LV_CH2_S  29
/* RMT_CARRIER_EN_CH2 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel2.*/
#define RMT_CARRIER_EN_CH2  (BIT(28))
#define RMT_CARRIER_EN_CH2_M  (BIT(28))
#define RMT_CARRIER_EN_CH2_V  0x1
#define RMT_CARRIER_EN_CH2_S  28
/* RMT_MEM_SIZE_CH2 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel2.*/
#define RMT_MEM_SIZE_CH2  0x0000000F
#define RMT_MEM_SIZE_CH2_M  ((RMT_MEM_SIZE_CH2_V)<<(RMT_MEM_SIZE_CH2_S))
#define RMT_MEM_SIZE_CH2_V  0xF
#define RMT_MEM_SIZE_CH2_S  24
/* RMT_IDLE_THRES_CH2 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: In receive mode when the counter's value is bigger than reg_idle_thres_ch2
 then the receive process is done.*/
#define RMT_IDLE_THRES_CH2  0x0000FFFF
#define RMT_IDLE_THRES_CH2_M  ((RMT_IDLE_THRES_CH2_V)<<(RMT_IDLE_THRES_CH2_S))
#define RMT_IDLE_THRES_CH2_V  0xFFFF
#define RMT_IDLE_THRES_CH2_S  8
/* RMT_DIV_CNT_CH2 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel2.*/
#define RMT_DIV_CNT_CH2  0x000000FF
#define RMT_DIV_CNT_CH2_M  ((RMT_DIV_CNT_CH2_V)<<(RMT_DIV_CNT_CH2_S))
#define RMT_DIV_CNT_CH2_V  0xFF
#define RMT_DIV_CNT_CH2_S  0

#define RMT_CH2CONF1_REG          (DR_REG_RMT_BASE + 0x0034)
/* RMT_IDLE_OUT_EN_CH2 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel2 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH2  (BIT(19))
#define RMT_IDLE_OUT_EN_CH2_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH2_V  0x1
#define RMT_IDLE_OUT_EN_CH2_S  19
/* RMT_IDLE_OUT_LV_CH2 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel2 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH2  (BIT(18))
#define RMT_IDLE_OUT_LV_CH2_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH2_V  0x1
#define RMT_IDLE_OUT_LV_CH2_S  18
/* RMT_REF_ALWAYS_ON_CH2 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH2  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH2_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH2_V  0x1
#define RMT_REF_ALWAYS_ON_CH2_S  17
/* RMT_REF_CNT_RST_CH2 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel2.*/
#define RMT_REF_CNT_RST_CH2  (BIT(16))
#define RMT_REF_CNT_RST_CH2_M  (BIT(16))
#define RMT_REF_CNT_RST_CH2_V  0x1
#define RMT_REF_CNT_RST_CH2_S  16
/* RMT_RX_FILTER_THRES_CH2 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel2 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH2  0x000000FF
#define RMT_RX_FILTER_THRES_CH2_M  ((RMT_RX_FILTER_THRES_CH2_V)<<(RMT_RX_FILTER_THRES_CH2_S))
#define RMT_RX_FILTER_THRES_CH2_V  0xFF
#define RMT_RX_FILTER_THRES_CH2_S  8
/* RMT_RX_FILTER_EN_CH2 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel2.*/
#define RMT_RX_FILTER_EN_CH2  (BIT(7))
#define RMT_RX_FILTER_EN_CH2_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH2_V  0x1
#define RMT_RX_FILTER_EN_CH2_S  7
/* RMT_TX_CONTI_MODE_CH2 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel2.*/
#define RMT_TX_CONTI_MODE_CH2  (BIT(6))
#define RMT_TX_CONTI_MODE_CH2_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH2_V  0x1
#define RMT_TX_CONTI_MODE_CH2_S  6
/* RMT_MEM_OWNER_CH2 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel2's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH2  (BIT(5))
#define RMT_MEM_OWNER_CH2_M  (BIT(5))
#define RMT_MEM_OWNER_CH2_V  0x1
#define RMT_MEM_OWNER_CH2_S  5
/* RMT_APB_MEM_RST_CH2 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel2 by apb fifo access*/
#define RMT_APB_MEM_RST_CH2  (BIT(4))
#define RMT_APB_MEM_RST_CH2_M  (BIT(4))
#define RMT_APB_MEM_RST_CH2_V  0x1
#define RMT_APB_MEM_RST_CH2_S  4
/* RMT_MEM_RD_RST_CH2 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel2 by transmitter access.*/
#define RMT_MEM_RD_RST_CH2  (BIT(3))
#define RMT_MEM_RD_RST_CH2_M  (BIT(3))
#define RMT_MEM_RD_RST_CH2_V  0x1
#define RMT_MEM_RD_RST_CH2_S  3
/* RMT_MEM_WR_RST_CH2 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel2 by receiver access.*/
#define RMT_MEM_WR_RST_CH2  (BIT(2))
#define RMT_MEM_WR_RST_CH2_M  (BIT(2))
#define RMT_MEM_WR_RST_CH2_V  0x1
#define RMT_MEM_WR_RST_CH2_S  2
/* RMT_RX_EN_CH2 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel2.*/
#define RMT_RX_EN_CH2  (BIT(1))
#define RMT_RX_EN_CH2_M  (BIT(1))
#define RMT_RX_EN_CH2_V  0x1
#define RMT_RX_EN_CH2_S  1
/* RMT_TX_START_CH2 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel2.*/
#define RMT_TX_START_CH2  (BIT(0))
#define RMT_TX_START_CH2_M  (BIT(0))
#define RMT_TX_START_CH2_V  0x1
#define RMT_TX_START_CH2_S  0

#define RMT_CH3CONF0_REG          (DR_REG_RMT_BASE + 0x0038)
/* RMT_CARRIER_OUT_LV_CH3 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure carrier wave's position for channel3.1'b1:add
 on low level  1'b0:add  on high level.*/
#define RMT_CARRIER_OUT_LV_CH3  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH3_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH3_V  0x1
#define RMT_CARRIER_OUT_LV_CH3_S  29
/* RMT_CARRIER_EN_CH3 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel3.*/
#define RMT_CARRIER_EN_CH3  (BIT(28))
#define RMT_CARRIER_EN_CH3_M  (BIT(28))
#define RMT_CARRIER_EN_CH3_V  0x1
#define RMT_CARRIER_EN_CH3_S  28
/* RMT_MEM_SIZE_CH3 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel3.*/
#define RMT_MEM_SIZE_CH3  0x0000000F
#define RMT_MEM_SIZE_CH3_M  ((RMT_MEM_SIZE_CH3_V)<<(RMT_MEM_SIZE_CH3_S))
#define RMT_MEM_SIZE_CH3_V  0xF
#define RMT_MEM_SIZE_CH3_S  24
/* RMT_IDLE_THRES_CH3 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: In receive mode when the counter's value is bigger than reg_idle_thres_ch3
 then the receive process is done.*/
#define RMT_IDLE_THRES_CH3  0x0000FFFF
#define RMT_IDLE_THRES_CH3_M  ((RMT_IDLE_THRES_CH3_V)<<(RMT_IDLE_THRES_CH3_S))
#define RMT_IDLE_THRES_CH3_V  0xFFFF
#define RMT_IDLE_THRES_CH3_S  8
/* RMT_DIV_CNT_CH3 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel3.*/
#define RMT_DIV_CNT_CH3  0x000000FF
#define RMT_DIV_CNT_CH3_M  ((RMT_DIV_CNT_CH3_V)<<(RMT_DIV_CNT_CH3_S))
#define RMT_DIV_CNT_CH3_V  0xFF
#define RMT_DIV_CNT_CH3_S  0

#define RMT_CH3CONF1_REG          (DR_REG_RMT_BASE + 0x003c)
/* RMT_IDLE_OUT_EN_CH3 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel3 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH3  (BIT(19))
#define RMT_IDLE_OUT_EN_CH3_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH3_V  0x1
#define RMT_IDLE_OUT_EN_CH3_S  19
/* RMT_IDLE_OUT_LV_CH3 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel3 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH3  (BIT(18))
#define RMT_IDLE_OUT_LV_CH3_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH3_V  0x1
#define RMT_IDLE_OUT_LV_CH3_S  18
/* RMT_REF_ALWAYS_ON_CH3 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH3  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH3_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH3_V  0x1
#define RMT_REF_ALWAYS_ON_CH3_S  17
/* RMT_REF_CNT_RST_CH3 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel3.*/
#define RMT_REF_CNT_RST_CH3  (BIT(16))
#define RMT_REF_CNT_RST_CH3_M  (BIT(16))
#define RMT_REF_CNT_RST_CH3_V  0x1
#define RMT_REF_CNT_RST_CH3_S  16
/* RMT_RX_FILTER_THRES_CH3 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel3 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH3  0x000000FF
#define RMT_RX_FILTER_THRES_CH3_M  ((RMT_RX_FILTER_THRES_CH3_V)<<(RMT_RX_FILTER_THRES_CH3_S))
#define RMT_RX_FILTER_THRES_CH3_V  0xFF
#define RMT_RX_FILTER_THRES_CH3_S  8
/* RMT_RX_FILTER_EN_CH3 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel3.*/
#define RMT_RX_FILTER_EN_CH3  (BIT(7))
#define RMT_RX_FILTER_EN_CH3_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH3_V  0x1
#define RMT_RX_FILTER_EN_CH3_S  7
/* RMT_TX_CONTI_MODE_CH3 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel3.*/
#define RMT_TX_CONTI_MODE_CH3  (BIT(6))
#define RMT_TX_CONTI_MODE_CH3_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH3_V  0x1
#define RMT_TX_CONTI_MODE_CH3_S  6
/* RMT_MEM_OWNER_CH3 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel3's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH3  (BIT(5))
#define RMT_MEM_OWNER_CH3_M  (BIT(5))
#define RMT_MEM_OWNER_CH3_V  0x1
#define RMT_MEM_OWNER_CH3_S  5
/* RMT_APB_MEM_RST_CH3 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel3 by apb fifo access*/
#define RMT_APB_MEM_RST_CH3  (BIT(4))
#define RMT_APB_MEM_RST_CH3_M  (BIT(4))
#define RMT_APB_MEM_RST_CH3_V  0x1
#define RMT_APB_MEM_RST_CH3_S  4
/* RMT_MEM_RD_RST_CH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel3 by transmitter access.*/
#define RMT_MEM_RD_RST_CH3  (BIT(3))
#define RMT_MEM_RD_RST_CH3_M  (BIT(3))
#define RMT_MEM_RD_RST_CH3_V  0x1
#define RMT_MEM_RD_RST_CH3_S  3
/* RMT_MEM_WR_RST_CH3 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel3 by receiver access.*/
#define RMT_MEM_WR_RST_CH3  (BIT(2))
#define RMT_MEM_WR_RST_CH3_M  (BIT(2))
#define RMT_MEM_WR_RST_CH3_V  0x1
#define RMT_MEM_WR_RST_CH3_S  2
/* RMT_RX_EN_CH3 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel3.*/
#define RMT_RX_EN_CH3  (BIT(1))
#define RMT_RX_EN_CH3_M  (BIT(1))
#define RMT_RX_EN_CH3_V  0x1
#define RMT_RX_EN_CH3_S  1
/* RMT_TX_START_CH3 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel3.*/
#define RMT_TX_START_CH3  (BIT(0))
#define RMT_TX_START_CH3_M  (BIT(0))
#define RMT_TX_START_CH3_V  0x1
#define RMT_TX_START_CH3_S  0

#define RMT_CH4CONF0_REG          (DR_REG_RMT_BASE + 0x0040)
/* RMT_CARRIER_OUT_LV_CH4 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure carrier wave's position for channel4.1'b1:add
 on low level  1'b0:add  on high level.*/
#define RMT_CARRIER_OUT_LV_CH4  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH4_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH4_V  0x1
#define RMT_CARRIER_OUT_LV_CH4_S  29
/* RMT_CARRIER_EN_CH4 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel4.*/
#define RMT_CARRIER_EN_CH4  (BIT(28))
#define RMT_CARRIER_EN_CH4_M  (BIT(28))
#define RMT_CARRIER_EN_CH4_V  0x1
#define RMT_CARRIER_EN_CH4_S  28
/* RMT_MEM_SIZE_CH4 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel4.*/
#define RMT_MEM_SIZE_CH4  0x0000000F
#define RMT_MEM_SIZE_CH4_M  ((RMT_MEM_SIZE_CH4_V)<<(RMT_MEM_SIZE_CH4_S))
#define RMT_MEM_SIZE_CH4_V  0xF
#define RMT_MEM_SIZE_CH4_S  24
/* RMT_IDLE_THRES_CH4 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: In receive mode when the counter's value is bigger than reg_idle_thres_ch4
 then the receive process is done.*/
#define RMT_IDLE_THRES_CH4  0x0000FFFF
#define RMT_IDLE_THRES_CH4_M  ((RMT_IDLE_THRES_CH4_V)<<(RMT_IDLE_THRES_CH4_S))
#define RMT_IDLE_THRES_CH4_V  0xFFFF
#define RMT_IDLE_THRES_CH4_S  8
/* RMT_DIV_CNT_CH4 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel4.*/
#define RMT_DIV_CNT_CH4  0x000000FF
#define RMT_DIV_CNT_CH4_M  ((RMT_DIV_CNT_CH4_V)<<(RMT_DIV_CNT_CH4_S))
#define RMT_DIV_CNT_CH4_V  0xFF
#define RMT_DIV_CNT_CH4_S  0

#define RMT_CH4CONF1_REG          (DR_REG_RMT_BASE + 0x0044)
/* RMT_IDLE_OUT_EN_CH4 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel4 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH4  (BIT(19))
#define RMT_IDLE_OUT_EN_CH4_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH4_V  0x1
#define RMT_IDLE_OUT_EN_CH4_S  19
/* RMT_IDLE_OUT_LV_CH4 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel4 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH4  (BIT(18))
#define RMT_IDLE_OUT_LV_CH4_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH4_V  0x1
#define RMT_IDLE_OUT_LV_CH4_S  18
/* RMT_REF_ALWAYS_ON_CH4 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH4  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH4_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH4_V  0x1
#define RMT_REF_ALWAYS_ON_CH4_S  17
/* RMT_REF_CNT_RST_CH4 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel4.*/
#define RMT_REF_CNT_RST_CH4  (BIT(16))
#define RMT_REF_CNT_RST_CH4_M  (BIT(16))
#define RMT_REF_CNT_RST_CH4_V  0x1
#define RMT_REF_CNT_RST_CH4_S  16
/* RMT_RX_FILTER_THRES_CH4 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel4 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH4  0x000000FF
#define RMT_RX_FILTER_THRES_CH4_M  ((RMT_RX_FILTER_THRES_CH4_V)<<(RMT_RX_FILTER_THRES_CH4_S))
#define RMT_RX_FILTER_THRES_CH4_V  0xFF
#define RMT_RX_FILTER_THRES_CH4_S  8
/* RMT_RX_FILTER_EN_CH4 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel4.*/
#define RMT_RX_FILTER_EN_CH4  (BIT(7))
#define RMT_RX_FILTER_EN_CH4_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH4_V  0x1
#define RMT_RX_FILTER_EN_CH4_S  7
/* RMT_TX_CONTI_MODE_CH4 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel4.*/
#define RMT_TX_CONTI_MODE_CH4  (BIT(6))
#define RMT_TX_CONTI_MODE_CH4_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH4_V  0x1
#define RMT_TX_CONTI_MODE_CH4_S  6
/* RMT_MEM_OWNER_CH4 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel4's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH4  (BIT(5))
#define RMT_MEM_OWNER_CH4_M  (BIT(5))
#define RMT_MEM_OWNER_CH4_V  0x1
#define RMT_MEM_OWNER_CH4_S  5
/* RMT_APB_MEM_RST_CH4 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel4 by apb fifo access*/
#define RMT_APB_MEM_RST_CH4  (BIT(4))
#define RMT_APB_MEM_RST_CH4_M  (BIT(4))
#define RMT_APB_MEM_RST_CH4_V  0x1
#define RMT_APB_MEM_RST_CH4_S  4
/* RMT_MEM_RD_RST_CH4 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel4 by transmitter access.*/
#define RMT_MEM_RD_RST_CH4  (BIT(3))
#define RMT_MEM_RD_RST_CH4_M  (BIT(3))
#define RMT_MEM_RD_RST_CH4_V  0x1
#define RMT_MEM_RD_RST_CH4_S  3
/* RMT_MEM_WR_RST_CH4 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel4 by receiver access.*/
#define RMT_MEM_WR_RST_CH4  (BIT(2))
#define RMT_MEM_WR_RST_CH4_M  (BIT(2))
#define RMT_MEM_WR_RST_CH4_V  0x1
#define RMT_MEM_WR_RST_CH4_S  2
/* RMT_RX_EN_CH4 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel4.*/
#define RMT_RX_EN_CH4  (BIT(1))
#define RMT_RX_EN_CH4_M  (BIT(1))
#define RMT_RX_EN_CH4_V  0x1
#define RMT_RX_EN_CH4_S  1
/* RMT_TX_START_CH4 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel4.*/
#define RMT_TX_START_CH4  (BIT(0))
#define RMT_TX_START_CH4_M  (BIT(0))
#define RMT_TX_START_CH4_V  0x1
#define RMT_TX_START_CH4_S  0

#define RMT_CH5CONF0_REG          (DR_REG_RMT_BASE + 0x0048)
/* RMT_CARRIER_OUT_LV_CH5 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure carrier wave's position for channel5.1'b1:add
 on low level  1'b0:add  on high level.*/
#define RMT_CARRIER_OUT_LV_CH5  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH5_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH5_V  0x1
#define RMT_CARRIER_OUT_LV_CH5_S  29
/* RMT_CARRIER_EN_CH5 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel5.*/
#define RMT_CARRIER_EN_CH5  (BIT(28))
#define RMT_CARRIER_EN_CH5_M  (BIT(28))
#define RMT_CARRIER_EN_CH5_V  0x1
#define RMT_CARRIER_EN_CH5_S  28
/* RMT_MEM_SIZE_CH5 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel5.*/
#define RMT_MEM_SIZE_CH5  0x0000000F
#define RMT_MEM_SIZE_CH5_M  ((RMT_MEM_SIZE_CH5_V)<<(RMT_MEM_SIZE_CH5_S))
#define RMT_MEM_SIZE_CH5_V  0xF
#define RMT_MEM_SIZE_CH5_S  24
/* RMT_IDLE_THRES_CH5 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: In receive mode when the counter's value is bigger than reg_idle_thres_ch5
 then the receive process is done.*/
#define RMT_IDLE_THRES_CH5  0x0000FFFF
#define RMT_IDLE_THRES_CH5_M  ((RMT_IDLE_THRES_CH5_V)<<(RMT_IDLE_THRES_CH5_S))
#define RMT_IDLE_THRES_CH5_V  0xFFFF
#define RMT_IDLE_THRES_CH5_S  8
/* RMT_DIV_CNT_CH5 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel5.*/
#define RMT_DIV_CNT_CH5  0x000000FF
#define RMT_DIV_CNT_CH5_M  ((RMT_DIV_CNT_CH5_V)<<(RMT_DIV_CNT_CH5_S))
#define RMT_DIV_CNT_CH5_V  0xFF
#define RMT_DIV_CNT_CH5_S  0

#define RMT_CH5CONF1_REG          (DR_REG_RMT_BASE + 0x004c)
/* RMT_IDLE_OUT_EN_CH5 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel5 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH5  (BIT(19))
#define RMT_IDLE_OUT_EN_CH5_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH5_V  0x1
#define RMT_IDLE_OUT_EN_CH5_S  19
/* RMT_IDLE_OUT_LV_CH5 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel5 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH5  (BIT(18))
#define RMT_IDLE_OUT_LV_CH5_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH5_V  0x1
#define RMT_IDLE_OUT_LV_CH5_S  18
/* RMT_REF_ALWAYS_ON_CH5 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH5  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH5_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH5_V  0x1
#define RMT_REF_ALWAYS_ON_CH5_S  17
/* RMT_REF_CNT_RST_CH5 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel5.*/
#define RMT_REF_CNT_RST_CH5  (BIT(16))
#define RMT_REF_CNT_RST_CH5_M  (BIT(16))
#define RMT_REF_CNT_RST_CH5_V  0x1
#define RMT_REF_CNT_RST_CH5_S  16
/* RMT_RX_FILTER_THRES_CH5 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel5 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH5  0x000000FF
#define RMT_RX_FILTER_THRES_CH5_M  ((RMT_RX_FILTER_THRES_CH5_V)<<(RMT_RX_FILTER_THRES_CH5_S))
#define RMT_RX_FILTER_THRES_CH5_V  0xFF
#define RMT_RX_FILTER_THRES_CH5_S  8
/* RMT_RX_FILTER_EN_CH5 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel5.*/
#define RMT_RX_FILTER_EN_CH5  (BIT(7))
#define RMT_RX_FILTER_EN_CH5_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH5_V  0x1
#define RMT_RX_FILTER_EN_CH5_S  7
/* RMT_TX_CONTI_MODE_CH5 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel5.*/
#define RMT_TX_CONTI_MODE_CH5  (BIT(6))
#define RMT_TX_CONTI_MODE_CH5_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH5_V  0x1
#define RMT_TX_CONTI_MODE_CH5_S  6
/* RMT_MEM_OWNER_CH5 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel5's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH5  (BIT(5))
#define RMT_MEM_OWNER_CH5_M  (BIT(5))
#define RMT_MEM_OWNER_CH5_V  0x1
#define RMT_MEM_OWNER_CH5_S  5
/* RMT_APB_MEM_RST_CH5 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel5 by apb fifo access*/
#define RMT_APB_MEM_RST_CH5  (BIT(4))
#define RMT_APB_MEM_RST_CH5_M  (BIT(4))
#define RMT_APB_MEM_RST_CH5_V  0x1
#define RMT_APB_MEM_RST_CH5_S  4
/* RMT_MEM_RD_RST_CH5 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel5 by transmitter access.*/
#define RMT_MEM_RD_RST_CH5  (BIT(3))
#define RMT_MEM_RD_RST_CH5_M  (BIT(3))
#define RMT_MEM_RD_RST_CH5_V  0x1
#define RMT_MEM_RD_RST_CH5_S  3
/* RMT_MEM_WR_RST_CH5 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel5 by receiver access.*/
#define RMT_MEM_WR_RST_CH5  (BIT(2))
#define RMT_MEM_WR_RST_CH5_M  (BIT(2))
#define RMT_MEM_WR_RST_CH5_V  0x1
#define RMT_MEM_WR_RST_CH5_S  2
/* RMT_RX_EN_CH5 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel5.*/
#define RMT_RX_EN_CH5  (BIT(1))
#define RMT_RX_EN_CH5_M  (BIT(1))
#define RMT_RX_EN_CH5_V  0x1
#define RMT_RX_EN_CH5_S  1
/* RMT_TX_START_CH5 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel5.*/
#define RMT_TX_START_CH5  (BIT(0))
#define RMT_TX_START_CH5_M  (BIT(0))
#define RMT_TX_START_CH5_V  0x1
#define RMT_TX_START_CH5_S  0

#define RMT_CH6CONF0_REG          (DR_REG_RMT_BASE + 0x0050)
/* RMT_CARRIER_OUT_LV_CH6 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure carrier wave's position for channel6.1'b1:add
 on low level  1'b0:add  on high level.*/
#define RMT_CARRIER_OUT_LV_CH6  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH6_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH6_V  0x1
#define RMT_CARRIER_OUT_LV_CH6_S  29
/* RMT_CARRIER_EN_CH6 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel6.*/
#define RMT_CARRIER_EN_CH6  (BIT(28))
#define RMT_CARRIER_EN_CH6_M  (BIT(28))
#define RMT_CARRIER_EN_CH6_V  0x1
#define RMT_CARRIER_EN_CH6_S  28
/* RMT_MEM_SIZE_CH6 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel6.*/
#define RMT_MEM_SIZE_CH6  0x0000000F
#define RMT_MEM_SIZE_CH6_M  ((RMT_MEM_SIZE_CH6_V)<<(RMT_MEM_SIZE_CH6_S))
#define RMT_MEM_SIZE_CH6_V  0xF
#define RMT_MEM_SIZE_CH6_S  24
/* RMT_IDLE_THRES_CH6 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: In receive mode when the counter's value is bigger than reg_idle_thres_ch6
 then the receive process is done.*/
#define RMT_IDLE_THRES_CH6  0x0000FFFF
#define RMT_IDLE_THRES_CH6_M  ((RMT_IDLE_THRES_CH6_V)<<(RMT_IDLE_THRES_CH6_S))
#define RMT_IDLE_THRES_CH6_V  0xFFFF
#define RMT_IDLE_THRES_CH6_S  8
/* RMT_DIV_CNT_CH6 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel6.*/
#define RMT_DIV_CNT_CH6  0x000000FF
#define RMT_DIV_CNT_CH6_M  ((RMT_DIV_CNT_CH6_V)<<(RMT_DIV_CNT_CH6_S))
#define RMT_DIV_CNT_CH6_V  0xFF
#define RMT_DIV_CNT_CH6_S  0

#define RMT_CH6CONF1_REG          (DR_REG_RMT_BASE + 0x0054)
/* RMT_IDLE_OUT_EN_CH6 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel6 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH6  (BIT(19))
#define RMT_IDLE_OUT_EN_CH6_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH6_V  0x1
#define RMT_IDLE_OUT_EN_CH6_S  19
/* RMT_IDLE_OUT_LV_CH6 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel6 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH6  (BIT(18))
#define RMT_IDLE_OUT_LV_CH6_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH6_V  0x1
#define RMT_IDLE_OUT_LV_CH6_S  18
/* RMT_REF_ALWAYS_ON_CH6 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH6  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH6_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH6_V  0x1
#define RMT_REF_ALWAYS_ON_CH6_S  17
/* RMT_REF_CNT_RST_CH6 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel6.*/
#define RMT_REF_CNT_RST_CH6  (BIT(16))
#define RMT_REF_CNT_RST_CH6_M  (BIT(16))
#define RMT_REF_CNT_RST_CH6_V  0x1
#define RMT_REF_CNT_RST_CH6_S  16
/* RMT_RX_FILTER_THRES_CH6 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel6 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH6  0x000000FF
#define RMT_RX_FILTER_THRES_CH6_M  ((RMT_RX_FILTER_THRES_CH6_V)<<(RMT_RX_FILTER_THRES_CH6_S))
#define RMT_RX_FILTER_THRES_CH6_V  0xFF
#define RMT_RX_FILTER_THRES_CH6_S  8
/* RMT_RX_FILTER_EN_CH6 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel6.*/
#define RMT_RX_FILTER_EN_CH6  (BIT(7))
#define RMT_RX_FILTER_EN_CH6_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH6_V  0x1
#define RMT_RX_FILTER_EN_CH6_S  7
/* RMT_TX_CONTI_MODE_CH6 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel6.*/
#define RMT_TX_CONTI_MODE_CH6  (BIT(6))
#define RMT_TX_CONTI_MODE_CH6_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH6_V  0x1
#define RMT_TX_CONTI_MODE_CH6_S  6
/* RMT_MEM_OWNER_CH6 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel6's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH6  (BIT(5))
#define RMT_MEM_OWNER_CH6_M  (BIT(5))
#define RMT_MEM_OWNER_CH6_V  0x1
#define RMT_MEM_OWNER_CH6_S  5
/* RMT_APB_MEM_RST_CH6 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel6 by apb fifo access*/
#define RMT_APB_MEM_RST_CH6  (BIT(4))
#define RMT_APB_MEM_RST_CH6_M  (BIT(4))
#define RMT_APB_MEM_RST_CH6_V  0x1
#define RMT_APB_MEM_RST_CH6_S  4
/* RMT_MEM_RD_RST_CH6 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel6 by transmitter access.*/
#define RMT_MEM_RD_RST_CH6  (BIT(3))
#define RMT_MEM_RD_RST_CH6_M  (BIT(3))
#define RMT_MEM_RD_RST_CH6_V  0x1
#define RMT_MEM_RD_RST_CH6_S  3
/* RMT_MEM_WR_RST_CH6 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel6 by receiver access.*/
#define RMT_MEM_WR_RST_CH6  (BIT(2))
#define RMT_MEM_WR_RST_CH6_M  (BIT(2))
#define RMT_MEM_WR_RST_CH6_V  0x1
#define RMT_MEM_WR_RST_CH6_S  2
/* RMT_RX_EN_CH6 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel6.*/
#define RMT_RX_EN_CH6  (BIT(1))
#define RMT_RX_EN_CH6_M  (BIT(1))
#define RMT_RX_EN_CH6_V  0x1
#define RMT_RX_EN_CH6_S  1
/* RMT_TX_START_CH6 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel6.*/
#define RMT_TX_START_CH6  (BIT(0))
#define RMT_TX_START_CH6_M  (BIT(0))
#define RMT_TX_START_CH6_V  0x1
#define RMT_TX_START_CH6_S  0

#define RMT_CH7CONF0_REG          (DR_REG_RMT_BASE + 0x0058)
/* RMT_CARRIER_OUT_LV_CH7 : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: This bit is used to configure carrier wave's position for channel7.1'b1:add
 on low level  1'b0:add  on high level.*/
#define RMT_CARRIER_OUT_LV_CH7  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH7_M  (BIT(29))
#define RMT_CARRIER_OUT_LV_CH7_V  0x1
#define RMT_CARRIER_OUT_LV_CH7_S  29
/* RMT_CARRIER_EN_CH7 : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: This is the carrier modulation enable control bit for channel7.*/
#define RMT_CARRIER_EN_CH7  (BIT(28))
#define RMT_CARRIER_EN_CH7_M  (BIT(28))
#define RMT_CARRIER_EN_CH7_V  0x1
#define RMT_CARRIER_EN_CH7_S  28
/* RMT_MEM_SIZE_CH7 : R/W ;bitpos:[27:24] ;default: 4'h1 ; */
/*description: This register is used to configure the the amount of memory blocks
 allocated to channel7.*/
#define RMT_MEM_SIZE_CH7  0x0000000F
#define RMT_MEM_SIZE_CH7_M  ((RMT_MEM_SIZE_CH7_V)<<(RMT_MEM_SIZE_CH7_S))
#define RMT_MEM_SIZE_CH7_V  0xF
#define RMT_MEM_SIZE_CH7_S  24
/* RMT_IDLE_THRES_CH7 : R/W ;bitpos:[23:8] ;default: 16'h1000 ; */
/*description: In receive mode when the counter's value is bigger than reg_idle_thres_ch7
 then the receive process is done.*/
#define RMT_IDLE_THRES_CH7  0x0000FFFF
#define RMT_IDLE_THRES_CH7_M  ((RMT_IDLE_THRES_CH7_V)<<(RMT_IDLE_THRES_CH7_S))
#define RMT_IDLE_THRES_CH7_V  0xFFFF
#define RMT_IDLE_THRES_CH7_S  8
/* RMT_DIV_CNT_CH7 : R/W ;bitpos:[7:0] ;default: 8'h2 ; */
/*description: This register is used to configure the  frequency divider's factor in channel7.*/
#define RMT_DIV_CNT_CH7  0x000000FF
#define RMT_DIV_CNT_CH7_M  ((RMT_DIV_CNT_CH7_V)<<(RMT_DIV_CNT_CH7_S))
#define RMT_DIV_CNT_CH7_V  0xFF
#define RMT_DIV_CNT_CH7_S  0

#define RMT_CH7CONF1_REG          (DR_REG_RMT_BASE + 0x005c)
/* RMT_IDLE_OUT_EN_CH7 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: This is the output enable control bit for channel6 in IDLE state.*/
#define RMT_IDLE_OUT_EN_CH7  (BIT(19))
#define RMT_IDLE_OUT_EN_CH7_M  (BIT(19))
#define RMT_IDLE_OUT_EN_CH7_V  0x1
#define RMT_IDLE_OUT_EN_CH7_S  19
/* RMT_IDLE_OUT_LV_CH7 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: This bit configures the output signal's level for channel7 in IDLE state.*/
#define RMT_IDLE_OUT_LV_CH7  (BIT(18))
#define RMT_IDLE_OUT_LV_CH7_M  (BIT(18))
#define RMT_IDLE_OUT_LV_CH7_V  0x1
#define RMT_IDLE_OUT_LV_CH7_S  18
/* RMT_REF_ALWAYS_ON_CH7 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
#define RMT_REF_ALWAYS_ON_CH7  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH7_M  (BIT(17))
#define RMT_REF_ALWAYS_ON_CH7_V  0x1
#define RMT_REF_ALWAYS_ON_CH7_S  17
/* RMT_REF_CNT_RST_CH7 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: This bit is used to reset divider in channel7.*/
#define RMT_REF_CNT_RST_CH7  (BIT(16))
#define RMT_REF_CNT_RST_CH7_M  (BIT(16))
#define RMT_REF_CNT_RST_CH7_V  0x1
#define RMT_REF_CNT_RST_CH7_S  16
/* RMT_RX_FILTER_THRES_CH7 : R/W ;bitpos:[15:8] ;default: 8'hf ; */
/*description: in receive mode  channel7 ignore input pulse when the pulse width
 is smaller then this value.*/
#define RMT_RX_FILTER_THRES_CH7  0x000000FF
#define RMT_RX_FILTER_THRES_CH7_M  ((RMT_RX_FILTER_THRES_CH7_V)<<(RMT_RX_FILTER_THRES_CH7_S))
#define RMT_RX_FILTER_THRES_CH7_V  0xFF
#define RMT_RX_FILTER_THRES_CH7_S  8
/* RMT_RX_FILTER_EN_CH7 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: This is the receive filter enable bit for channel7.*/
#define RMT_RX_FILTER_EN_CH7  (BIT(7))
#define RMT_RX_FILTER_EN_CH7_M  (BIT(7))
#define RMT_RX_FILTER_EN_CH7_V  0x1
#define RMT_RX_FILTER_EN_CH7_S  7
/* RMT_TX_CONTI_MODE_CH7 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to continue sending  from the first data to the
 last data in channel7.*/
#define RMT_TX_CONTI_MODE_CH7  (BIT(6))
#define RMT_TX_CONTI_MODE_CH7_M  (BIT(6))
#define RMT_TX_CONTI_MODE_CH7_V  0x1
#define RMT_TX_CONTI_MODE_CH7_S  6
/* RMT_MEM_OWNER_CH7 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: This is the mark of channel7's ram usage right.1'b1：receiver
 uses the ram  0：transmitter uses the ram*/
#define RMT_MEM_OWNER_CH7  (BIT(5))
#define RMT_MEM_OWNER_CH7_M  (BIT(5))
#define RMT_MEM_OWNER_CH7_V  0x1
#define RMT_MEM_OWNER_CH7_S  5
/* RMT_APB_MEM_RST_CH7 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to reset W/R ram address for channel7 by apb fifo access*/
#define RMT_APB_MEM_RST_CH7  (BIT(4))
#define RMT_APB_MEM_RST_CH7_M  (BIT(4))
#define RMT_APB_MEM_RST_CH7_V  0x1
#define RMT_APB_MEM_RST_CH7_S  4
/* RMT_MEM_RD_RST_CH7 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset read ram address for channel7 by transmitter access.*/
#define RMT_MEM_RD_RST_CH7  (BIT(3))
#define RMT_MEM_RD_RST_CH7_M  (BIT(3))
#define RMT_MEM_RD_RST_CH7_V  0x1
#define RMT_MEM_RD_RST_CH7_S  3
/* RMT_MEM_WR_RST_CH7 : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to reset write ram address for channel7 by receiver access.*/
#define RMT_MEM_WR_RST_CH7  (BIT(2))
#define RMT_MEM_WR_RST_CH7_M  (BIT(2))
#define RMT_MEM_WR_RST_CH7_V  0x1
#define RMT_MEM_WR_RST_CH7_S  2
/* RMT_RX_EN_CH7 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to enbale receving data for channel7.*/
#define RMT_RX_EN_CH7  (BIT(1))
#define RMT_RX_EN_CH7_M  (BIT(1))
#define RMT_RX_EN_CH7_V  0x1
#define RMT_RX_EN_CH7_S  1
/* RMT_TX_START_CH7 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to start sending data for channel7.*/
#define RMT_TX_START_CH7  (BIT(0))
#define RMT_TX_START_CH7_M  (BIT(0))
#define RMT_TX_START_CH7_V  0x1
#define RMT_TX_START_CH7_S  0

#define RMT_CH0STATUS_REG          (DR_REG_RMT_BASE + 0x0060)
/* RMT_STATUS_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel0*/
#define RMT_STATUS_CH0  0xFFFFFFFF
#define RMT_STATUS_CH0_M  ((RMT_STATUS_CH0_V)<<(RMT_STATUS_CH0_S))
#define RMT_STATUS_CH0_V  0xFFFFFFFF
#define RMT_STATUS_CH0_S  0
/* RMT_APB_MEM_RD_ERR_CH0 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel0 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH0 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH0_M  ((RMT_APB_MEM_RD_ERR_CH0_V)<<(RMT_APB_MEM_RD_ERR_CH0_S))
#define RMT_APB_MEM_RD_ERR_CH0_V  0x1
#define RMT_APB_MEM_RD_ERR_CH0_S  31
/* RMT_APB_MEM_WR_ERR_CH0 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel0 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH0 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH0_M  ((RMT_APB_MEM_WR_ERR_CH0_V)<<(RMT_APB_MEM_WR_ERR_CH0_S))
#define RMT_APB_MEM_WR_ERR_CH0_V  0x1
#define RMT_APB_MEM_WR_ERR_CH0_S  30
/* RMT_MEM_EMPTY_CH0 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel0. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH0 (BIT(29))
#define RMT_MEM_EMPTY_CH0_M  ((RMT_MEM_EMPTY_CH0_V)<<(RMT_MEM_EMPTY_CH0_S))
#define RMT_MEM_EMPTY_CH0_V  0x1
#define RMT_MEM_EMPTY_CH0_S  29
/* RMT_MEM_FULL_CH0 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel0 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH0 (BIT(28))
#define RMT_MEM_FULL_CH0_M  ((RMT_MEM_FULL_CH0_V)<<(RMT_MEM_FULL_CH0_S))
#define RMT_MEM_FULL_CH0_V  0x1
#define RMT_MEM_FULL_CH0_S  28
/* RMT_MEM_OWNER_ERR_CH0 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel0 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH0 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH0_M  ((RMT_MEM_OWNER_ERR_CH0_V)<<(RMT_MEM_OWNER_ERR_CH0_S))
#define RMT_MEM_OWNER_ERR_CH0_V  0x1
#define RMT_MEM_OWNER_ERR_CH0_S  27
/* RMT_STATE_CH0 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel0 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH0 0x07000000
#define RMT_STATE_CH0_M  ((RMT_STATE_CH0_V)<<(RMT_STATE_CH0_S))
#define RMT_STATE_CH0_V  0x7
#define RMT_STATE_CH0_S  24
/* RMT_MEM_RADDR_EX_CH0 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel0.*/
#define RMT_MEM_RADDR_EX_CH0 0x003ff000
#define RMT_MEM_RADDR_EX_CH0_M  ((RMT_MEM_RADDR_EX_CH0_V)<<(RMT_MEM_RADDR_EX_CH0_S))
#define RMT_MEM_RADDR_EX_CH0_V  0x3ff
#define RMT_MEM_RADDR_EX_CH0_S  12
/* RMT_MEM_WADDR_EX_CH0 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel0.*/
#define RMT_MEM_WADDR_EX_CH0 0x000003ff
#define RMT_MEM_WADDR_EX_CH0_M  ((RMT_MEM_WADDR_EX_CH0_V)<<(RMT_MEM_WADDR_EX_CH0_S))
#define RMT_MEM_WADDR_EX_CH0_V  0x3ff
#define RMT_MEM_WADDR_EX_CH0_S  0

#define RMT_CH1STATUS_REG          (DR_REG_RMT_BASE + 0x0064)
/* RMT_STATUS_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel1*/
#define RMT_STATUS_CH1  0xFFFFFFFF
#define RMT_STATUS_CH1_M  ((RMT_STATUS_CH1_V)<<(RMT_STATUS_CH1_S))
#define RMT_STATUS_CH1_V  0xFFFFFFFF
#define RMT_STATUS_CH1_S  0
/* RMT_APB_MEM_RD_ERR_CH1 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel1 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH1 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH1_M  ((RMT_APB_MEM_RD_ERR_CH1_V)<<(RMT_APB_MEM_RD_ERR_CH1_S))
#define RMT_APB_MEM_RD_ERR_CH1_V  0x1
#define RMT_APB_MEM_RD_ERR_CH1_S  31
/* RMT_APB_MEM_WR_ERR_CH1 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel1 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH1 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH1_M  ((RMT_APB_MEM_WR_ERR_CH1_V)<<(RMT_APB_MEM_WR_ERR_CH1_S))
#define RMT_APB_MEM_WR_ERR_CH1_V  0x1
#define RMT_APB_MEM_WR_ERR_CH1_S  30
/* RMT_MEM_EMPTY_CH1 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel1. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH1 (BIT(29))
#define RMT_MEM_EMPTY_CH1_M  ((RMT_MEM_EMPTY_CH1_V)<<(RMT_MEM_EMPTY_CH1_S))
#define RMT_MEM_EMPTY_CH1_V  0x1
#define RMT_MEM_EMPTY_CH1_S  29
/* RMT_MEM_FULL_CH1 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel1 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH1 (BIT(28))
#define RMT_MEM_FULL_CH1_M  ((RMT_MEM_FULL_CH1_V)<<(RMT_MEM_FULL_CH1_S))
#define RMT_MEM_FULL_CH1_V  0x1
#define RMT_MEM_FULL_CH1_S  28
/* RMT_MEM_OWNER_ERR_CH1 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel1 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH1 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH1_M  ((RMT_MEM_OWNER_ERR_CH1_V)<<(RMT_MEM_OWNER_ERR_CH1_S))
#define RMT_MEM_OWNER_ERR_CH1_V  0x1
#define RMT_MEM_OWNER_ERR_CH1_S  27
/* RMT_STATE_CH1 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel1 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH1 0x07000000
#define RMT_STATE_CH1_M  ((RMT_STATE_CH1_V)<<(RMT_STATE_CH1_S))
#define RMT_STATE_CH1_V  0x7
#define RMT_STATE_CH1_S  24
/* RMT_MEM_RADDR_EX_CH1 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel1.*/
#define RMT_MEM_RADDR_EX_CH1 0x003ff000
#define RMT_MEM_RADDR_EX_CH1_M  ((RMT_MEM_RADDR_EX_CH1_V)<<(RMT_MEM_RADDR_EX_CH1_S))
#define RMT_MEM_RADDR_EX_CH1_V  0x3ff
#define RMT_MEM_RADDR_EX_CH1_S  12
/* RMT_MEM_WADDR_EX_CH1 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel1.*/
#define RMT_MEM_WADDR_EX_CH1 0x000003ff
#define RMT_MEM_WADDR_EX_CH1_M  ((RMT_MEM_WADDR_EX_CH1_V)<<(RMT_MEM_WADDR_EX_CH1_S))
#define RMT_MEM_WADDR_EX_CH1_V  0x3ff
#define RMT_MEM_WADDR_EX_CH1_S  0

#define RMT_CH2STATUS_REG          (DR_REG_RMT_BASE + 0x0068)
/* RMT_STATUS_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel2*/
#define RMT_STATUS_CH2  0xFFFFFFFF
#define RMT_STATUS_CH2_M  ((RMT_STATUS_CH2_V)<<(RMT_STATUS_CH2_S))
#define RMT_STATUS_CH2_V  0xFFFFFFFF
#define RMT_STATUS_CH2_S  0
/* RMT_APB_MEM_RD_ERR_CH2 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel2 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH2 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH2_M  ((RMT_APB_MEM_RD_ERR_CH2_V)<<(RMT_APB_MEM_RD_ERR_CH2_S))
#define RMT_APB_MEM_RD_ERR_CH2_V  0x1
#define RMT_APB_MEM_RD_ERR_CH2_S  31
/* RMT_APB_MEM_WR_ERR_CH2 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel2 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH2 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH2_M  ((RMT_APB_MEM_WR_ERR_CH2_V)<<(RMT_APB_MEM_WR_ERR_CH2_S))
#define RMT_APB_MEM_WR_ERR_CH2_V  0x1
#define RMT_APB_MEM_WR_ERR_CH2_S  30
/* RMT_MEM_EMPTY_CH2 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel2. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH2 (BIT(29))
#define RMT_MEM_EMPTY_CH2_M  ((RMT_MEM_EMPTY_CH2_V)<<(RMT_MEM_EMPTY_CH2_S))
#define RMT_MEM_EMPTY_CH2_V  0x1
#define RMT_MEM_EMPTY_CH2_S  29
/* RMT_MEM_FULL_CH2 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel2 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH2 (BIT(28))
#define RMT_MEM_FULL_CH2_M  ((RMT_MEM_FULL_CH2_V)<<(RMT_MEM_FULL_CH2_S))
#define RMT_MEM_FULL_CH2_V  0x1
#define RMT_MEM_FULL_CH2_S  28
/* RMT_MEM_OWNER_ERR_CH2 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel2 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH2 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH2_M  ((RMT_MEM_OWNER_ERR_CH2_V)<<(RMT_MEM_OWNER_ERR_CH2_S))
#define RMT_MEM_OWNER_ERR_CH2_V  0x1
#define RMT_MEM_OWNER_ERR_CH2_S  27
/* RMT_STATE_CH2 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel2 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH2 0x07000000
#define RMT_STATE_CH2_M  ((RMT_STATE_CH2_V)<<(RMT_STATE_CH2_S))
#define RMT_STATE_CH2_V  0x7
#define RMT_STATE_CH2_S  24
/* RMT_MEM_RADDR_EX_CH2 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel2.*/
#define RMT_MEM_RADDR_EX_CH2 0x003ff000
#define RMT_MEM_RADDR_EX_CH2_M  ((RMT_MEM_RADDR_EX_CH2_V)<<(RMT_MEM_RADDR_EX_CH2_S))
#define RMT_MEM_RADDR_EX_CH2_V  0x3ff
#define RMT_MEM_RADDR_EX_CH2_S  12
/* RMT_MEM_WADDR_EX_CH2 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel2.*/
#define RMT_MEM_WADDR_EX_CH2 0x000003ff
#define RMT_MEM_WADDR_EX_CH2_M  ((RMT_MEM_WADDR_EX_CH2_V)<<(RMT_MEM_WADDR_EX_CH2_S))
#define RMT_MEM_WADDR_EX_CH2_V  0x3ff
#define RMT_MEM_WADDR_EX_CH2_S  0

#define RMT_CH3STATUS_REG          (DR_REG_RMT_BASE + 0x006c)
/* RMT_STATUS_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel3*/
#define RMT_STATUS_CH3  0xFFFFFFFF
#define RMT_STATUS_CH3_M  ((RMT_STATUS_CH3_V)<<(RMT_STATUS_CH3_S))
#define RMT_STATUS_CH3_V  0xFFFFFFFF
#define RMT_STATUS_CH3_S  0
/* RMT_APB_MEM_RD_ERR_CH3 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel3 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH3 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH3_M  ((RMT_APB_MEM_RD_ERR_CH3_V)<<(RMT_APB_MEM_RD_ERR_CH3_S))
#define RMT_APB_MEM_RD_ERR_CH3_V  0x1
#define RMT_APB_MEM_RD_ERR_CH3_S  31
/* RMT_APB_MEM_WR_ERR_CH3 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel3 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH3 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH3_M  ((RMT_APB_MEM_WR_ERR_CH3_V)<<(RMT_APB_MEM_WR_ERR_CH3_S))
#define RMT_APB_MEM_WR_ERR_CH3_V  0x1
#define RMT_APB_MEM_WR_ERR_CH3_S  30
/* RMT_MEM_EMPTY_CH3 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel3. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH3 (BIT(29))
#define RMT_MEM_EMPTY_CH3_M  ((RMT_MEM_EMPTY_CH3_V)<<(RMT_MEM_EMPTY_CH3_S))
#define RMT_MEM_EMPTY_CH3_V  0x1
#define RMT_MEM_EMPTY_CH3_S  29
/* RMT_MEM_FULL_CH3 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel3 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH3 (BIT(28))
#define RMT_MEM_FULL_CH3_M  ((RMT_MEM_FULL_CH3_V)<<(RMT_MEM_FULL_CH3_S))
#define RMT_MEM_FULL_CH3_V  0x1
#define RMT_MEM_FULL_CH3_S  28
/* RMT_MEM_OWNER_ERR_CH3 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel3 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH3 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH3_M  ((RMT_MEM_OWNER_ERR_CH3_V)<<(RMT_MEM_OWNER_ERR_CH3_S))
#define RMT_MEM_OWNER_ERR_CH3_V  0x1
#define RMT_MEM_OWNER_ERR_CH3_S  27
/* RMT_STATE_CH3 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel3 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH3 0x07000000
#define RMT_STATE_CH3_M  ((RMT_STATE_CH3_V)<<(RMT_STATE_CH3_S))
#define RMT_STATE_CH3_V  0x7
#define RMT_STATE_CH3_S  24
/* RMT_MEM_RADDR_EX_CH3 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel3.*/
#define RMT_MEM_RADDR_EX_CH3 0x003ff000
#define RMT_MEM_RADDR_EX_CH3_M  ((RMT_MEM_RADDR_EX_CH3_V)<<(RMT_MEM_RADDR_EX_CH3_S))
#define RMT_MEM_RADDR_EX_CH3_V  0x3ff
#define RMT_MEM_RADDR_EX_CH3_S  12
/* RMT_MEM_WADDR_EX_CH3 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel3.*/
#define RMT_MEM_WADDR_EX_CH3 0x000003ff
#define RMT_MEM_WADDR_EX_CH3_M  ((RMT_MEM_WADDR_EX_CH3_V)<<(RMT_MEM_WADDR_EX_CH3_S))
#define RMT_MEM_WADDR_EX_CH3_V  0x3ff
#define RMT_MEM_WADDR_EX_CH3_S  0

#define RMT_CH4STATUS_REG          (DR_REG_RMT_BASE + 0x0070)
/* RMT_STATUS_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel4*/
#define RMT_STATUS_CH4  0xFFFFFFFF
#define RMT_STATUS_CH4_M  ((RMT_STATUS_CH4_V)<<(RMT_STATUS_CH4_S))
#define RMT_STATUS_CH4_V  0xFFFFFFFF
#define RMT_STATUS_CH4_S  0
/* RMT_APB_MEM_RD_ERR_CH4 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel4 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH4 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH4_M  ((RMT_APB_MEM_RD_ERR_CH4_V)<<(RMT_APB_MEM_RD_ERR_CH4_S))
#define RMT_APB_MEM_RD_ERR_CH4_V  0x1
#define RMT_APB_MEM_RD_ERR_CH4_S  31
/* RMT_APB_MEM_WR_ERR_CH4 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel4 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH4 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH4_M  ((RMT_APB_MEM_WR_ERR_CH4_V)<<(RMT_APB_MEM_WR_ERR_CH4_S))
#define RMT_APB_MEM_WR_ERR_CH4_V  0x1
#define RMT_APB_MEM_WR_ERR_CH4_S  30
/* RMT_MEM_EMPTY_CH4 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel4. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH4 (BIT(29))
#define RMT_MEM_EMPTY_CH4_M  ((RMT_MEM_EMPTY_CH4_V)<<(RMT_MEM_EMPTY_CH4_S))
#define RMT_MEM_EMPTY_CH4_V  0x1
#define RMT_MEM_EMPTY_CH4_S  29
/* RMT_MEM_FULL_CH4 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel4 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH4 (BIT(28))
#define RMT_MEM_FULL_CH4_M  ((RMT_MEM_FULL_CH4_V)<<(RMT_MEM_FULL_CH4_S))
#define RMT_MEM_FULL_CH4_V  0x1
#define RMT_MEM_FULL_CH4_S  28
/* RMT_MEM_OWNER_ERR_CH4 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel4 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH4 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH4_M  ((RMT_MEM_OWNER_ERR_CH4_V)<<(RMT_MEM_OWNER_ERR_CH4_S))
#define RMT_MEM_OWNER_ERR_CH4_V  0x1
#define RMT_MEM_OWNER_ERR_CH4_S  27
/* RMT_STATE_CH4 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel4 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH4 0x07000000
#define RMT_STATE_CH4_M  ((RMT_STATE_CH4_V)<<(RMT_STATE_CH4_S))
#define RMT_STATE_CH4_V  0x7
#define RMT_STATE_CH4_S  24
/* RMT_MEM_RADDR_EX_CH4 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel4.*/
#define RMT_MEM_RADDR_EX_CH4 0x003ff000
#define RMT_MEM_RADDR_EX_CH4_M  ((RMT_MEM_RADDR_EX_CH4_V)<<(RMT_MEM_RADDR_EX_CH4_S))
#define RMT_MEM_RADDR_EX_CH4_V  0x3ff
#define RMT_MEM_RADDR_EX_CH4_S  12
/* RMT_MEM_WADDR_EX_CH4 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel4.*/
#define RMT_MEM_WADDR_EX_CH4 0x000003ff
#define RMT_MEM_WADDR_EX_CH4_M  ((RMT_MEM_WADDR_EX_CH4_V)<<(RMT_MEM_WADDR_EX_CH4_S))
#define RMT_MEM_WADDR_EX_CH4_V  0x3ff
#define RMT_MEM_WADDR_EX_CH4_S  0

#define RMT_CH5STATUS_REG          (DR_REG_RMT_BASE + 0x0074)
/* RMT_STATUS_CH5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel5*/
#define RMT_STATUS_CH5  0xFFFFFFFF
#define RMT_STATUS_CH5_M  ((RMT_STATUS_CH5_V)<<(RMT_STATUS_CH5_S))
#define RMT_STATUS_CH5_V  0xFFFFFFFF
#define RMT_STATUS_CH5_S  0
/* RMT_APB_MEM_RD_ERR_CH5 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel5 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH5 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH5_M  ((RMT_APB_MEM_RD_ERR_CH5_V)<<(RMT_APB_MEM_RD_ERR_CH5_S))
#define RMT_APB_MEM_RD_ERR_CH5_V  0x1
#define RMT_APB_MEM_RD_ERR_CH5_S  31
/* RMT_APB_MEM_WR_ERR_CH5 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel5 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH5 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH5_M  ((RMT_APB_MEM_WR_ERR_CH5_V)<<(RMT_APB_MEM_WR_ERR_CH5_S))
#define RMT_APB_MEM_WR_ERR_CH5_V  0x1
#define RMT_APB_MEM_WR_ERR_CH5_S  30
/* RMT_MEM_EMPTY_CH5 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel5. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH5 (BIT(29))
#define RMT_MEM_EMPTY_CH5_M  ((RMT_MEM_EMPTY_CH5_V)<<(RMT_MEM_EMPTY_CH5_S))
#define RMT_MEM_EMPTY_CH5_V  0x1
#define RMT_MEM_EMPTY_CH5_S  29
/* RMT_MEM_FULL_CH5 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel5 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH5 (BIT(28))
#define RMT_MEM_FULL_CH5_M  ((RMT_MEM_FULL_CH5_V)<<(RMT_MEM_FULL_CH5_S))
#define RMT_MEM_FULL_CH5_V  0x1
#define RMT_MEM_FULL_CH5_S  28
/* RMT_MEM_OWNER_ERR_CH5 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel5 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH5 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH5_M  ((RMT_MEM_OWNER_ERR_CH5_V)<<(RMT_MEM_OWNER_ERR_CH5_S))
#define RMT_MEM_OWNER_ERR_CH5_V  0x1
#define RMT_MEM_OWNER_ERR_CH5_S  27
/* RMT_STATE_CH5 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel5 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH5 0x07000000
#define RMT_STATE_CH5_M  ((RMT_STATE_CH5_V)<<(RMT_STATE_CH5_S))
#define RMT_STATE_CH5_V  0x7
#define RMT_STATE_CH5_S  24
/* RMT_MEM_RADDR_EX_CH5 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel5.*/
#define RMT_MEM_RADDR_EX_CH5 0x003ff000
#define RMT_MEM_RADDR_EX_CH5_M  ((RMT_MEM_RADDR_EX_CH5_V)<<(RMT_MEM_RADDR_EX_CH5_S))
#define RMT_MEM_RADDR_EX_CH5_V  0x3ff
#define RMT_MEM_RADDR_EX_CH5_S  12
/* RMT_MEM_WADDR_EX_CH5 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel5.*/
#define RMT_MEM_WADDR_EX_CH5 0x000003ff
#define RMT_MEM_WADDR_EX_CH5_M  ((RMT_MEM_WADDR_EX_CH5_V)<<(RMT_MEM_WADDR_EX_CH5_S))
#define RMT_MEM_WADDR_EX_CH5_V  0x3ff
#define RMT_MEM_WADDR_EX_CH5_S  0

#define RMT_CH6STATUS_REG          (DR_REG_RMT_BASE + 0x0078)
/* RMT_STATUS_CH6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel6*/
#define RMT_STATUS_CH6  0xFFFFFFFF
#define RMT_STATUS_CH6_M  ((RMT_STATUS_CH6_V)<<(RMT_STATUS_CH6_S))
#define RMT_STATUS_CH6_V  0xFFFFFFFF
#define RMT_STATUS_CH6_S  0
/* RMT_APB_MEM_RD_ERR_CH6 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel6 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH6 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH6_M  ((RMT_APB_MEM_RD_ERR_CH6_V)<<(RMT_APB_MEM_RD_ERR_CH6_S))
#define RMT_APB_MEM_RD_ERR_CH6_V  0x1
#define RMT_APB_MEM_RD_ERR_CH6_S  31
/* RMT_APB_MEM_WR_ERR_CH6 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel6 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH6 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH6_M  ((RMT_APB_MEM_WR_ERR_CH6_V)<<(RMT_APB_MEM_WR_ERR_CH6_S))
#define RMT_APB_MEM_WR_ERR_CH6_V  0x1
#define RMT_APB_MEM_WR_ERR_CH6_S  30
/* RMT_MEM_EMPTY_CH6 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel6. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH6 (BIT(29))
#define RMT_MEM_EMPTY_CH6_M  ((RMT_MEM_EMPTY_CH6_V)<<(RMT_MEM_EMPTY_CH6_S))
#define RMT_MEM_EMPTY_CH6_V  0x1
#define RMT_MEM_EMPTY_CH6_S  29
/* RMT_MEM_FULL_CH6 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel6 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH6 (BIT(28))
#define RMT_MEM_FULL_CH6_M  ((RMT_MEM_FULL_CH6_V)<<(RMT_MEM_FULL_CH6_S))
#define RMT_MEM_FULL_CH6_V  0x1
#define RMT_MEM_FULL_CH6_S  28
/* RMT_MEM_OWNER_ERR_CH6 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel6 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH6 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH6_M  ((RMT_MEM_OWNER_ERR_CH6_V)<<(RMT_MEM_OWNER_ERR_CH6_S))
#define RMT_MEM_OWNER_ERR_CH6_V  0x1
#define RMT_MEM_OWNER_ERR_CH6_S  27
/* RMT_STATE_CH6 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel6 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH6 0x07000000
#define RMT_STATE_CH6_M  ((RMT_STATE_CH6_V)<<(RMT_STATE_CH6_S))
#define RMT_STATE_CH6_V  0x7
#define RMT_STATE_CH6_S  24
/* RMT_MEM_RADDR_EX_CH6 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel6.*/
#define RMT_MEM_RADDR_EX_CH6 0x003ff000
#define RMT_MEM_RADDR_EX_CH6_M  ((RMT_MEM_RADDR_EX_CH6_V)<<(RMT_MEM_RADDR_EX_CH6_S))
#define RMT_MEM_RADDR_EX_CH6_V  0x3ff
#define RMT_MEM_RADDR_EX_CH6_S  12
/* RMT_MEM_WADDR_EX_CH6 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel6.*/
#define RMT_MEM_WADDR_EX_CH6 0x000003ff
#define RMT_MEM_WADDR_EX_CH6_M  ((RMT_MEM_WADDR_EX_CH6_V)<<(RMT_MEM_WADDR_EX_CH6_S))
#define RMT_MEM_WADDR_EX_CH6_V  0x3ff
#define RMT_MEM_WADDR_EX_CH6_S  0

#define RMT_CH7STATUS_REG          (DR_REG_RMT_BASE + 0x007c)
/* RMT_STATUS_CH7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The status for channel7*/
#define RMT_STATUS_CH7  0xFFFFFFFF
#define RMT_STATUS_CH7_M  ((RMT_STATUS_CH7_V)<<(RMT_STATUS_CH7_S))
#define RMT_STATUS_CH7_V  0xFFFFFFFF
#define RMT_STATUS_CH7_S  0
/* RMT_APB_MEM_RD_ERR_CH7 : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The apb read memory status bit for channel7 turns to
 high level when the apb read address exceeds the configuration range.*/
#define RMT_APB_MEM_RD_ERR_CH7 (BIT(31))
#define RMT_APB_MEM_RD_ERR_CH7_M  ((RMT_APB_MEM_RD_ERR_CH7_V)<<(RMT_APB_MEM_RD_ERR_CH7_S))
#define RMT_APB_MEM_RD_ERR_CH7_V  0x1
#define RMT_APB_MEM_RD_ERR_CH7_S  31
/* RMT_APB_MEM_WR_ERR_CH7 : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The apb write memory status bit for channel7 turns to
 high level when the apb write address exceeds the configuration range.*/
#define RMT_APB_MEM_WR_ERR_CH7 (BIT(30))
#define RMT_APB_MEM_WR_ERR_CH7_M  ((RMT_APB_MEM_WR_ERR_CH7_V)<<(RMT_APB_MEM_WR_ERR_CH7_S))
#define RMT_APB_MEM_WR_ERR_CH7_V  0x1
#define RMT_APB_MEM_WR_ERR_CH7_S  30
/* RMT_MEM_EMPTY_CH7 : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The memory empty status bit for channel7. in acyclic mode,
 this bit turns to high level when mem_raddr_ex is greater than or equal to the configured range.*/
#define RMT_MEM_EMPTY_CH7 (BIT(29))
#define RMT_MEM_EMPTY_CH7_M  ((RMT_MEM_EMPTY_CH7_V)<<(RMT_MEM_EMPTY_CH7_S))
#define RMT_MEM_EMPTY_CH7_V  0x1
#define RMT_MEM_EMPTY_CH7_S  29
/* RMT_MEM_FULL_CH7 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The memory full status bit for channel7 turns to high level
 when mem_waddr_ex is greater than or equal to the configuration range.*/
#define RMT_MEM_FULL_CH7 (BIT(28))
#define RMT_MEM_FULL_CH7_M  ((RMT_MEM_FULL_CH7_V)<<(RMT_MEM_FULL_CH7_S))
#define RMT_MEM_FULL_CH7_V  0x1
#define RMT_MEM_FULL_CH7_S  28
/* RMT_MEM_OWNER_ERR_CH7 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: When channel7 is configured for receive mode, this bit will turn to high level
 if rmt_mem_owner register is not set to 1.*/
#define RMT_MEM_OWNER_ERR_CH7 (BIT(27))
#define RMT_MEM_OWNER_ERR_CH7_M  ((RMT_MEM_OWNER_ERR_CH7_V)<<(RMT_MEM_OWNER_ERR_CH7_S))
#define RMT_MEM_OWNER_ERR_CH7_V  0x1
#define RMT_MEM_OWNER_ERR_CH7_S  27
/* RMT_STATE_CH7 : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The channel7 state machine status register.
3'h0 : idle, 3'h1 : send, 3'h2 : read memory, 3'h3 : receive, 3'h4 : wait.*/
#define RMT_STATE_CH7 0x07000000
#define RMT_STATE_CH7_M  ((RMT_STATE_CH7_V)<<(RMT_STATE_CH7_S))
#define RMT_STATE_CH7_V  0x7
#define RMT_STATE_CH7_S  24
/* RMT_MEM_RADDR_EX_CH7 : RO ;bitpos:[21:12] ;default: 10'h0 ; */
/*description: The current memory write address of channel7.*/
#define RMT_MEM_RADDR_EX_CH7 0x003ff000
#define RMT_MEM_RADDR_EX_CH7_M  ((RMT_MEM_RADDR_EX_CH7_V)<<(RMT_MEM_RADDR_EX_CH7_S))
#define RMT_MEM_RADDR_EX_CH7_V  0x3ff
#define RMT_MEM_RADDR_EX_CH7_S  12
/* RMT_MEM_WADDR_EX_CH7 : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The current memory read address of channel7.*/
#define RMT_MEM_WADDR_EX_CH7 0x000003ff
#define RMT_MEM_WADDR_EX_CH7_M  ((RMT_MEM_WADDR_EX_CH7_V)<<(RMT_MEM_WADDR_EX_CH7_S))
#define RMT_MEM_WADDR_EX_CH7_V  0x3ff
#define RMT_MEM_WADDR_EX_CH7_S  0

#define RMT_CH0ADDR_REG          (DR_REG_RMT_BASE + 0x0080)
/* RMT_APB_MEM_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel0 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH0  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH0_M  ((RMT_APB_MEM_ADDR_CH0_V)<<(RMT_APB_MEM_ADDR_CH0_S))
#define RMT_APB_MEM_ADDR_CH0_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH0_S  0

#define RMT_CH1ADDR_REG          (DR_REG_RMT_BASE + 0x0084)
/* RMT_APB_MEM_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel1 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH1  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH1_M  ((RMT_APB_MEM_ADDR_CH1_V)<<(RMT_APB_MEM_ADDR_CH1_S))
#define RMT_APB_MEM_ADDR_CH1_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH1_S  0

#define RMT_CH2ADDR_REG          (DR_REG_RMT_BASE + 0x0088)
/* RMT_APB_MEM_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel2 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH2  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH2_M  ((RMT_APB_MEM_ADDR_CH2_V)<<(RMT_APB_MEM_ADDR_CH2_S))
#define RMT_APB_MEM_ADDR_CH2_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH2_S  0

#define RMT_CH3ADDR_REG          (DR_REG_RMT_BASE + 0x008c)
/* RMT_APB_MEM_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel3 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH3  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH3_M  ((RMT_APB_MEM_ADDR_CH3_V)<<(RMT_APB_MEM_ADDR_CH3_S))
#define RMT_APB_MEM_ADDR_CH3_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH3_S  0

#define RMT_CH4ADDR_REG          (DR_REG_RMT_BASE + 0x0090)
/* RMT_APB_MEM_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel4 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH4  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH4_M  ((RMT_APB_MEM_ADDR_CH4_V)<<(RMT_APB_MEM_ADDR_CH4_S))
#define RMT_APB_MEM_ADDR_CH4_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH4_S  0

#define RMT_CH5ADDR_REG          (DR_REG_RMT_BASE + 0x0094)
/* RMT_APB_MEM_ADDR_CH5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel5 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH5  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH5_M  ((RMT_APB_MEM_ADDR_CH5_V)<<(RMT_APB_MEM_ADDR_CH5_S))
#define RMT_APB_MEM_ADDR_CH5_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH5_S  0

#define RMT_CH6ADDR_REG          (DR_REG_RMT_BASE + 0x0098)
/* RMT_APB_MEM_ADDR_CH6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel6 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH6  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH6_M  ((RMT_APB_MEM_ADDR_CH6_V)<<(RMT_APB_MEM_ADDR_CH6_S))
#define RMT_APB_MEM_ADDR_CH6_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH6_S  0

#define RMT_CH7ADDR_REG          (DR_REG_RMT_BASE + 0x009c)
/* RMT_APB_MEM_ADDR_CH7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The ram relative address in channel7 by apb fifo access*/
#define RMT_APB_MEM_ADDR_CH7  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH7_M  ((RMT_APB_MEM_ADDR_CH7_V)<<(RMT_APB_MEM_ADDR_CH7_S))
#define RMT_APB_MEM_ADDR_CH7_V  0xFFFFFFFF
#define RMT_APB_MEM_ADDR_CH7_S  0

#define RMT_INT_RAW_REG          (DR_REG_RMT_BASE + 0x00a0)
/* RMT_CH7_TX_THR_EVENT_INT_RAW : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel7 turns to high level when
 transmitter in channle 7  have send datas more than  reg_rmt_tx_lim_ch7  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH7_TX_THR_EVENT_INT_RAW  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_RAW_M  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH7_TX_THR_EVENT_INT_RAW_S  31
/* RMT_CH6_TX_THR_EVENT_INT_RAW : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 6 turns to high level when
 transmitter in channle6  have send datas more than  reg_rmt_tx_lim_ch6  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH6_TX_THR_EVENT_INT_RAW  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_RAW_M  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH6_TX_THR_EVENT_INT_RAW_S  30
/* RMT_CH5_TX_THR_EVENT_INT_RAW : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 5 turns to high level when
 transmitter in channle5  have send datas more than  reg_rmt_tx_lim_ch5  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH5_TX_THR_EVENT_INT_RAW  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_RAW_M  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH5_TX_THR_EVENT_INT_RAW_S  29
/* RMT_CH4_TX_THR_EVENT_INT_RAW : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 4 turns to high level when
 transmitter in channle4  have send datas more than  reg_rmt_tx_lim_ch4  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH4_TX_THR_EVENT_INT_RAW  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_RAW_M  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH4_TX_THR_EVENT_INT_RAW_S  28
/* RMT_CH3_TX_THR_EVENT_INT_RAW : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 3 turns to high level when
 transmitter in channle3  have send datas more than  reg_rmt_tx_lim_ch3  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH3_TX_THR_EVENT_INT_RAW  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_RAW_M  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_RAW_S  27
/* RMT_CH2_TX_THR_EVENT_INT_RAW : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 2 turns to high level when
 transmitter in channle2  have send datas more than  reg_rmt_tx_lim_ch2  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH2_TX_THR_EVENT_INT_RAW  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_RAW_M  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_RAW_S  26
/* RMT_CH1_TX_THR_EVENT_INT_RAW : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 1 turns to high level when
 transmitter in channle1  have send datas more than  reg_rmt_tx_lim_ch1  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH1_TX_THR_EVENT_INT_RAW  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_RAW_M  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_RAW_S  25
/* RMT_CH0_TX_THR_EVENT_INT_RAW : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 0 turns to high level when
 transmitter in channle0  have send datas more than  reg_rmt_tx_lim_ch0  after detecting this interrupt  software can updata the old datas with new datas.*/
#define RMT_CH0_TX_THR_EVENT_INT_RAW  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_RAW_M  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_RAW_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_RAW_S  24
/* RMT_CH7_ERR_INT_RAW : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 7 turns to high level when
 channle 7 detects some errors.*/
#define RMT_CH7_ERR_INT_RAW  (BIT(23))
#define RMT_CH7_ERR_INT_RAW_M  (BIT(23))
#define RMT_CH7_ERR_INT_RAW_V  0x1
#define RMT_CH7_ERR_INT_RAW_S  23
/* RMT_CH7_RX_END_INT_RAW : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 7 turns to high level when
 the receive process is done.*/
#define RMT_CH7_RX_END_INT_RAW  (BIT(22))
#define RMT_CH7_RX_END_INT_RAW_M  (BIT(22))
#define RMT_CH7_RX_END_INT_RAW_V  0x1
#define RMT_CH7_RX_END_INT_RAW_S  22
/* RMT_CH7_TX_END_INT_RAW : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 7 turns to high level when
 the transmit process is done.*/
#define RMT_CH7_TX_END_INT_RAW  (BIT(21))
#define RMT_CH7_TX_END_INT_RAW_M  (BIT(21))
#define RMT_CH7_TX_END_INT_RAW_V  0x1
#define RMT_CH7_TX_END_INT_RAW_S  21
/* RMT_CH6_ERR_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 6 turns to high level when
 channle 6 detects some errors.*/
#define RMT_CH6_ERR_INT_RAW  (BIT(20))
#define RMT_CH6_ERR_INT_RAW_M  (BIT(20))
#define RMT_CH6_ERR_INT_RAW_V  0x1
#define RMT_CH6_ERR_INT_RAW_S  20
/* RMT_CH6_RX_END_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 6 turns to high level when
 the receive process is done.*/
#define RMT_CH6_RX_END_INT_RAW  (BIT(19))
#define RMT_CH6_RX_END_INT_RAW_M  (BIT(19))
#define RMT_CH6_RX_END_INT_RAW_V  0x1
#define RMT_CH6_RX_END_INT_RAW_S  19
/* RMT_CH6_TX_END_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 6 turns to high level when
 the transmit process is done.*/
#define RMT_CH6_TX_END_INT_RAW  (BIT(18))
#define RMT_CH6_TX_END_INT_RAW_M  (BIT(18))
#define RMT_CH6_TX_END_INT_RAW_V  0x1
#define RMT_CH6_TX_END_INT_RAW_S  18
/* RMT_CH5_ERR_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 5 turns to high level when
 channle 5 detects some errors.*/
#define RMT_CH5_ERR_INT_RAW  (BIT(17))
#define RMT_CH5_ERR_INT_RAW_M  (BIT(17))
#define RMT_CH5_ERR_INT_RAW_V  0x1
#define RMT_CH5_ERR_INT_RAW_S  17
/* RMT_CH5_RX_END_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 5 turns to high level when
 the receive process is done.*/
#define RMT_CH5_RX_END_INT_RAW  (BIT(16))
#define RMT_CH5_RX_END_INT_RAW_M  (BIT(16))
#define RMT_CH5_RX_END_INT_RAW_V  0x1
#define RMT_CH5_RX_END_INT_RAW_S  16
/* RMT_CH5_TX_END_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 5 turns to high level when
 the transmit process is done.*/
#define RMT_CH5_TX_END_INT_RAW  (BIT(15))
#define RMT_CH5_TX_END_INT_RAW_M  (BIT(15))
#define RMT_CH5_TX_END_INT_RAW_V  0x1
#define RMT_CH5_TX_END_INT_RAW_S  15
/* RMT_CH4_ERR_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 4 turns to high level when
 channle 4 detects some errors.*/
#define RMT_CH4_ERR_INT_RAW  (BIT(14))
#define RMT_CH4_ERR_INT_RAW_M  (BIT(14))
#define RMT_CH4_ERR_INT_RAW_V  0x1
#define RMT_CH4_ERR_INT_RAW_S  14
/* RMT_CH4_RX_END_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 4 turns to high level when
 the receive process is done.*/
#define RMT_CH4_RX_END_INT_RAW  (BIT(13))
#define RMT_CH4_RX_END_INT_RAW_M  (BIT(13))
#define RMT_CH4_RX_END_INT_RAW_V  0x1
#define RMT_CH4_RX_END_INT_RAW_S  13
/* RMT_CH4_TX_END_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 4 turns to high level when
 the transmit process is done.*/
#define RMT_CH4_TX_END_INT_RAW  (BIT(12))
#define RMT_CH4_TX_END_INT_RAW_M  (BIT(12))
#define RMT_CH4_TX_END_INT_RAW_V  0x1
#define RMT_CH4_TX_END_INT_RAW_S  12
/* RMT_CH3_ERR_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 3 turns to high level when
 channle 3 detects some errors.*/
#define RMT_CH3_ERR_INT_RAW  (BIT(11))
#define RMT_CH3_ERR_INT_RAW_M  (BIT(11))
#define RMT_CH3_ERR_INT_RAW_V  0x1
#define RMT_CH3_ERR_INT_RAW_S  11
/* RMT_CH3_RX_END_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 3 turns to high level when
 the receive process is done.*/
#define RMT_CH3_RX_END_INT_RAW  (BIT(10))
#define RMT_CH3_RX_END_INT_RAW_M  (BIT(10))
#define RMT_CH3_RX_END_INT_RAW_V  0x1
#define RMT_CH3_RX_END_INT_RAW_S  10
/* RMT_CH3_TX_END_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 3 turns to high level when
 the transmit process is done.*/
#define RMT_CH3_TX_END_INT_RAW  (BIT(9))
#define RMT_CH3_TX_END_INT_RAW_M  (BIT(9))
#define RMT_CH3_TX_END_INT_RAW_V  0x1
#define RMT_CH3_TX_END_INT_RAW_S  9
/* RMT_CH2_ERR_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 2 turns to high level when
 channle 2 detects some errors.*/
#define RMT_CH2_ERR_INT_RAW  (BIT(8))
#define RMT_CH2_ERR_INT_RAW_M  (BIT(8))
#define RMT_CH2_ERR_INT_RAW_V  0x1
#define RMT_CH2_ERR_INT_RAW_S  8
/* RMT_CH2_RX_END_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 2 turns to high level when
 the receive process is done.*/
#define RMT_CH2_RX_END_INT_RAW  (BIT(7))
#define RMT_CH2_RX_END_INT_RAW_M  (BIT(7))
#define RMT_CH2_RX_END_INT_RAW_V  0x1
#define RMT_CH2_RX_END_INT_RAW_S  7
/* RMT_CH2_TX_END_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 2 turns to high level when
 the transmit process is done.*/
#define RMT_CH2_TX_END_INT_RAW  (BIT(6))
#define RMT_CH2_TX_END_INT_RAW_M  (BIT(6))
#define RMT_CH2_TX_END_INT_RAW_V  0x1
#define RMT_CH2_TX_END_INT_RAW_S  6
/* RMT_CH1_ERR_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 1 turns to high level when
 channle 1 detects some errors.*/
#define RMT_CH1_ERR_INT_RAW  (BIT(5))
#define RMT_CH1_ERR_INT_RAW_M  (BIT(5))
#define RMT_CH1_ERR_INT_RAW_V  0x1
#define RMT_CH1_ERR_INT_RAW_S  5
/* RMT_CH1_RX_END_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 1 turns to high level when
 the receive process is done.*/
#define RMT_CH1_RX_END_INT_RAW  (BIT(4))
#define RMT_CH1_RX_END_INT_RAW_M  (BIT(4))
#define RMT_CH1_RX_END_INT_RAW_V  0x1
#define RMT_CH1_RX_END_INT_RAW_S  4
/* RMT_CH1_TX_END_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 1 turns to high level when
 the transmit process is done.*/
#define RMT_CH1_TX_END_INT_RAW  (BIT(3))
#define RMT_CH1_TX_END_INT_RAW_M  (BIT(3))
#define RMT_CH1_TX_END_INT_RAW_V  0x1
#define RMT_CH1_TX_END_INT_RAW_S  3
/* RMT_CH0_ERR_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 0 turns to high level when
 channle 0 detects some errors.*/
#define RMT_CH0_ERR_INT_RAW  (BIT(2))
#define RMT_CH0_ERR_INT_RAW_M  (BIT(2))
#define RMT_CH0_ERR_INT_RAW_V  0x1
#define RMT_CH0_ERR_INT_RAW_S  2
/* RMT_CH0_RX_END_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 0 turns to high level when
 the receive process is done.*/
#define RMT_CH0_RX_END_INT_RAW  (BIT(1))
#define RMT_CH0_RX_END_INT_RAW_M  (BIT(1))
#define RMT_CH0_RX_END_INT_RAW_V  0x1
#define RMT_CH0_RX_END_INT_RAW_S  1
/* RMT_CH0_TX_END_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt raw bit for channel 0 turns to high level when
 the transmit process is done.*/
#define RMT_CH0_TX_END_INT_RAW  (BIT(0))
#define RMT_CH0_TX_END_INT_RAW_M  (BIT(0))
#define RMT_CH0_TX_END_INT_RAW_V  0x1
#define RMT_CH0_TX_END_INT_RAW_S  0

#define RMT_INT_ST_REG          (DR_REG_RMT_BASE + 0x00a4)
/* RMT_CH7_TX_THR_EVENT_INT_ST : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 7's rmt_ch7_tx_thr_event_int_raw
 when mt_ch7_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH7_TX_THR_EVENT_INT_ST  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_ST_M  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH7_TX_THR_EVENT_INT_ST_S  31
/* RMT_CH6_TX_THR_EVENT_INT_ST : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 6's rmt_ch6_tx_thr_event_int_raw
 when mt_ch6_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH6_TX_THR_EVENT_INT_ST  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_ST_M  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH6_TX_THR_EVENT_INT_ST_S  30
/* RMT_CH5_TX_THR_EVENT_INT_ST : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 5's rmt_ch5_tx_thr_event_int_raw
 when mt_ch5_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH5_TX_THR_EVENT_INT_ST  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_ST_M  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH5_TX_THR_EVENT_INT_ST_S  29
/* RMT_CH4_TX_THR_EVENT_INT_ST : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 4's rmt_ch4_tx_thr_event_int_raw
 when mt_ch4_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH4_TX_THR_EVENT_INT_ST  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_ST_M  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH4_TX_THR_EVENT_INT_ST_S  28
/* RMT_CH3_TX_THR_EVENT_INT_ST : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 3's rmt_ch3_tx_thr_event_int_raw
 when mt_ch3_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH3_TX_THR_EVENT_INT_ST  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_ST_M  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_ST_S  27
/* RMT_CH2_TX_THR_EVENT_INT_ST : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 2's rmt_ch2_tx_thr_event_int_raw
 when mt_ch2_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH2_TX_THR_EVENT_INT_ST  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_ST_M  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_ST_S  26
/* RMT_CH1_TX_THR_EVENT_INT_ST : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 1's rmt_ch1_tx_thr_event_int_raw
 when mt_ch1_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH1_TX_THR_EVENT_INT_ST  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_ST_M  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_ST_S  25
/* RMT_CH0_TX_THR_EVENT_INT_ST : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: The interrupt state bit  for channel 0's rmt_ch0_tx_thr_event_int_raw
 when mt_ch0_tx_thr_event_int_ena is set to 1.*/
#define RMT_CH0_TX_THR_EVENT_INT_ST  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_ST_M  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_ST_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_ST_S  24
/* RMT_CH7_ERR_INT_ST : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 7's rmt_ch7_err_int_raw
 when  rmt_ch7_err_int_ena is set to 1.*/
#define RMT_CH7_ERR_INT_ST  (BIT(23))
#define RMT_CH7_ERR_INT_ST_M  (BIT(23))
#define RMT_CH7_ERR_INT_ST_V  0x1
#define RMT_CH7_ERR_INT_ST_S  23
/* RMT_CH7_RX_END_INT_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 7's rmt_ch7_rx_end_int_raw
 when  rmt_ch7_rx_end_int_ena is set to 1.*/
#define RMT_CH7_RX_END_INT_ST  (BIT(22))
#define RMT_CH7_RX_END_INT_ST_M  (BIT(22))
#define RMT_CH7_RX_END_INT_ST_V  0x1
#define RMT_CH7_RX_END_INT_ST_S  22
/* RMT_CH7_TX_END_INT_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 7's mt_ch7_tx_end_int_raw
 when mt_ch7_tx_end_int_ena is set to 1.*/
#define RMT_CH7_TX_END_INT_ST  (BIT(21))
#define RMT_CH7_TX_END_INT_ST_M  (BIT(21))
#define RMT_CH7_TX_END_INT_ST_V  0x1
#define RMT_CH7_TX_END_INT_ST_S  21
/* RMT_CH6_ERR_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 6's rmt_ch6_err_int_raw
 when  rmt_ch6_err_int_ena is set to 1.*/
#define RMT_CH6_ERR_INT_ST  (BIT(20))
#define RMT_CH6_ERR_INT_ST_M  (BIT(20))
#define RMT_CH6_ERR_INT_ST_V  0x1
#define RMT_CH6_ERR_INT_ST_S  20
/* RMT_CH6_RX_END_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 6's rmt_ch6_rx_end_int_raw
 when  rmt_ch6_rx_end_int_ena is set to 1.*/
#define RMT_CH6_RX_END_INT_ST  (BIT(19))
#define RMT_CH6_RX_END_INT_ST_M  (BIT(19))
#define RMT_CH6_RX_END_INT_ST_V  0x1
#define RMT_CH6_RX_END_INT_ST_S  19
/* RMT_CH6_TX_END_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 6's mt_ch6_tx_end_int_raw
 when mt_ch6_tx_end_int_ena is set to 1.*/
#define RMT_CH6_TX_END_INT_ST  (BIT(18))
#define RMT_CH6_TX_END_INT_ST_M  (BIT(18))
#define RMT_CH6_TX_END_INT_ST_V  0x1
#define RMT_CH6_TX_END_INT_ST_S  18
/* RMT_CH5_ERR_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 5's rmt_ch5_err_int_raw
 when  rmt_ch5_err_int_ena is set to 1.*/
#define RMT_CH5_ERR_INT_ST  (BIT(17))
#define RMT_CH5_ERR_INT_ST_M  (BIT(17))
#define RMT_CH5_ERR_INT_ST_V  0x1
#define RMT_CH5_ERR_INT_ST_S  17
/* RMT_CH5_RX_END_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 5's rmt_ch5_rx_end_int_raw
 when  rmt_ch5_rx_end_int_ena is set to 1.*/
#define RMT_CH5_RX_END_INT_ST  (BIT(16))
#define RMT_CH5_RX_END_INT_ST_M  (BIT(16))
#define RMT_CH5_RX_END_INT_ST_V  0x1
#define RMT_CH5_RX_END_INT_ST_S  16
/* RMT_CH5_TX_END_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 5's mt_ch5_tx_end_int_raw
 when mt_ch5_tx_end_int_ena is set to 1.*/
#define RMT_CH5_TX_END_INT_ST  (BIT(15))
#define RMT_CH5_TX_END_INT_ST_M  (BIT(15))
#define RMT_CH5_TX_END_INT_ST_V  0x1
#define RMT_CH5_TX_END_INT_ST_S  15
/* RMT_CH4_ERR_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 4's rmt_ch4_err_int_raw
 when  rmt_ch4_err_int_ena is set to 1.*/
#define RMT_CH4_ERR_INT_ST  (BIT(14))
#define RMT_CH4_ERR_INT_ST_M  (BIT(14))
#define RMT_CH4_ERR_INT_ST_V  0x1
#define RMT_CH4_ERR_INT_ST_S  14
/* RMT_CH4_RX_END_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 4's rmt_ch4_rx_end_int_raw
 when  rmt_ch4_rx_end_int_ena is set to 1.*/
#define RMT_CH4_RX_END_INT_ST  (BIT(13))
#define RMT_CH4_RX_END_INT_ST_M  (BIT(13))
#define RMT_CH4_RX_END_INT_ST_V  0x1
#define RMT_CH4_RX_END_INT_ST_S  13
/* RMT_CH4_TX_END_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 4's mt_ch4_tx_end_int_raw
 when mt_ch4_tx_end_int_ena is set to 1.*/
#define RMT_CH4_TX_END_INT_ST  (BIT(12))
#define RMT_CH4_TX_END_INT_ST_M  (BIT(12))
#define RMT_CH4_TX_END_INT_ST_V  0x1
#define RMT_CH4_TX_END_INT_ST_S  12
/* RMT_CH3_ERR_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 3's rmt_ch3_err_int_raw
 when  rmt_ch3_err_int_ena is set to 1.*/
#define RMT_CH3_ERR_INT_ST  (BIT(11))
#define RMT_CH3_ERR_INT_ST_M  (BIT(11))
#define RMT_CH3_ERR_INT_ST_V  0x1
#define RMT_CH3_ERR_INT_ST_S  11
/* RMT_CH3_RX_END_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 3's rmt_ch3_rx_end_int_raw
 when  rmt_ch3_rx_end_int_ena is set to 1.*/
#define RMT_CH3_RX_END_INT_ST  (BIT(10))
#define RMT_CH3_RX_END_INT_ST_M  (BIT(10))
#define RMT_CH3_RX_END_INT_ST_V  0x1
#define RMT_CH3_RX_END_INT_ST_S  10
/* RMT_CH3_TX_END_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 3's mt_ch3_tx_end_int_raw
 when mt_ch3_tx_end_int_ena is set to 1.*/
#define RMT_CH3_TX_END_INT_ST  (BIT(9))
#define RMT_CH3_TX_END_INT_ST_M  (BIT(9))
#define RMT_CH3_TX_END_INT_ST_V  0x1
#define RMT_CH3_TX_END_INT_ST_S  9
/* RMT_CH2_ERR_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 2's rmt_ch2_err_int_raw
 when  rmt_ch2_err_int_ena is set to 1.*/
#define RMT_CH2_ERR_INT_ST  (BIT(8))
#define RMT_CH2_ERR_INT_ST_M  (BIT(8))
#define RMT_CH2_ERR_INT_ST_V  0x1
#define RMT_CH2_ERR_INT_ST_S  8
/* RMT_CH2_RX_END_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 2's rmt_ch2_rx_end_int_raw
 when  rmt_ch2_rx_end_int_ena is set to 1.*/
#define RMT_CH2_RX_END_INT_ST  (BIT(7))
#define RMT_CH2_RX_END_INT_ST_M  (BIT(7))
#define RMT_CH2_RX_END_INT_ST_V  0x1
#define RMT_CH2_RX_END_INT_ST_S  7
/* RMT_CH2_TX_END_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 2's mt_ch2_tx_end_int_raw
 when mt_ch2_tx_end_int_ena is set to 1.*/
#define RMT_CH2_TX_END_INT_ST  (BIT(6))
#define RMT_CH2_TX_END_INT_ST_M  (BIT(6))
#define RMT_CH2_TX_END_INT_ST_V  0x1
#define RMT_CH2_TX_END_INT_ST_S  6
/* RMT_CH1_ERR_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 1's rmt_ch1_err_int_raw
 when  rmt_ch1_err_int_ena is set to 1.*/
#define RMT_CH1_ERR_INT_ST  (BIT(5))
#define RMT_CH1_ERR_INT_ST_M  (BIT(5))
#define RMT_CH1_ERR_INT_ST_V  0x1
#define RMT_CH1_ERR_INT_ST_S  5
/* RMT_CH1_RX_END_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 1's rmt_ch1_rx_end_int_raw
 when  rmt_ch1_rx_end_int_ena is set to 1.*/
#define RMT_CH1_RX_END_INT_ST  (BIT(4))
#define RMT_CH1_RX_END_INT_ST_M  (BIT(4))
#define RMT_CH1_RX_END_INT_ST_V  0x1
#define RMT_CH1_RX_END_INT_ST_S  4
/* RMT_CH1_TX_END_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 1's mt_ch1_tx_end_int_raw
 when mt_ch1_tx_end_int_ena is set to 1.*/
#define RMT_CH1_TX_END_INT_ST  (BIT(3))
#define RMT_CH1_TX_END_INT_ST_M  (BIT(3))
#define RMT_CH1_TX_END_INT_ST_V  0x1
#define RMT_CH1_TX_END_INT_ST_S  3
/* RMT_CH0_ERR_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 0's rmt_ch0_err_int_raw
 when  rmt_ch0_err_int_ena is set to 0.*/
#define RMT_CH0_ERR_INT_ST  (BIT(2))
#define RMT_CH0_ERR_INT_ST_M  (BIT(2))
#define RMT_CH0_ERR_INT_ST_V  0x1
#define RMT_CH0_ERR_INT_ST_S  2
/* RMT_CH0_RX_END_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 0's rmt_ch0_rx_end_int_raw
 when  rmt_ch0_rx_end_int_ena is set to 0.*/
#define RMT_CH0_RX_END_INT_ST  (BIT(1))
#define RMT_CH0_RX_END_INT_ST_M  (BIT(1))
#define RMT_CH0_RX_END_INT_ST_V  0x1
#define RMT_CH0_RX_END_INT_ST_S  1
/* RMT_CH0_TX_END_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt  state bit for channel 0's mt_ch0_tx_end_int_raw
 when mt_ch0_tx_end_int_ena is set to 0.*/
#define RMT_CH0_TX_END_INT_ST  (BIT(0))
#define RMT_CH0_TX_END_INT_ST_M  (BIT(0))
#define RMT_CH0_TX_END_INT_ST_V  0x1
#define RMT_CH0_TX_END_INT_ST_S  0

#define RMT_INT_ENA_REG          (DR_REG_RMT_BASE + 0x00a8)
/* RMT_CH7_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch7_tx_thr_event_int_st.*/
#define RMT_CH7_TX_THR_EVENT_INT_ENA  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_ENA_M  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH7_TX_THR_EVENT_INT_ENA_S  31
/* RMT_CH6_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch6_tx_thr_event_int_st.*/
#define RMT_CH6_TX_THR_EVENT_INT_ENA  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_ENA_M  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH6_TX_THR_EVENT_INT_ENA_S  30
/* RMT_CH5_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch5_tx_thr_event_int_st.*/
#define RMT_CH5_TX_THR_EVENT_INT_ENA  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_ENA_M  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH5_TX_THR_EVENT_INT_ENA_S  29
/* RMT_CH4_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch4_tx_thr_event_int_st.*/
#define RMT_CH4_TX_THR_EVENT_INT_ENA  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_ENA_M  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH4_TX_THR_EVENT_INT_ENA_S  28
/* RMT_CH3_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch3_tx_thr_event_int_st.*/
#define RMT_CH3_TX_THR_EVENT_INT_ENA  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_ENA_M  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_ENA_S  27
/* RMT_CH2_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch2_tx_thr_event_int_st.*/
#define RMT_CH2_TX_THR_EVENT_INT_ENA  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_ENA_M  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_ENA_S  26
/* RMT_CH1_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch1_tx_thr_event_int_st.*/
#define RMT_CH1_TX_THR_EVENT_INT_ENA  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_ENA_M  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_ENA_S  25
/* RMT_CH0_TX_THR_EVENT_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch0_tx_thr_event_int_st.*/
#define RMT_CH0_TX_THR_EVENT_INT_ENA  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_ENA_M  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_ENA_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_ENA_S  24
/* RMT_CH7_ERR_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch7_err_int_st.*/
#define RMT_CH7_ERR_INT_ENA  (BIT(23))
#define RMT_CH7_ERR_INT_ENA_M  (BIT(23))
#define RMT_CH7_ERR_INT_ENA_V  0x1
#define RMT_CH7_ERR_INT_ENA_S  23
/* RMT_CH7_RX_END_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch7_rx_end_int_st.*/
#define RMT_CH7_RX_END_INT_ENA  (BIT(22))
#define RMT_CH7_RX_END_INT_ENA_M  (BIT(22))
#define RMT_CH7_RX_END_INT_ENA_V  0x1
#define RMT_CH7_RX_END_INT_ENA_S  22
/* RMT_CH7_TX_END_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch7_tx_end_int_st.*/
#define RMT_CH7_TX_END_INT_ENA  (BIT(21))
#define RMT_CH7_TX_END_INT_ENA_M  (BIT(21))
#define RMT_CH7_TX_END_INT_ENA_V  0x1
#define RMT_CH7_TX_END_INT_ENA_S  21
/* RMT_CH6_ERR_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch6_err_int_st.*/
#define RMT_CH6_ERR_INT_ENA  (BIT(20))
#define RMT_CH6_ERR_INT_ENA_M  (BIT(20))
#define RMT_CH6_ERR_INT_ENA_V  0x1
#define RMT_CH6_ERR_INT_ENA_S  20
/* RMT_CH6_RX_END_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch6_rx_end_int_st.*/
#define RMT_CH6_RX_END_INT_ENA  (BIT(19))
#define RMT_CH6_RX_END_INT_ENA_M  (BIT(19))
#define RMT_CH6_RX_END_INT_ENA_V  0x1
#define RMT_CH6_RX_END_INT_ENA_S  19
/* RMT_CH6_TX_END_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch6_tx_end_int_st.*/
#define RMT_CH6_TX_END_INT_ENA  (BIT(18))
#define RMT_CH6_TX_END_INT_ENA_M  (BIT(18))
#define RMT_CH6_TX_END_INT_ENA_V  0x1
#define RMT_CH6_TX_END_INT_ENA_S  18
/* RMT_CH5_ERR_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch5_err_int_st.*/
#define RMT_CH5_ERR_INT_ENA  (BIT(17))
#define RMT_CH5_ERR_INT_ENA_M  (BIT(17))
#define RMT_CH5_ERR_INT_ENA_V  0x1
#define RMT_CH5_ERR_INT_ENA_S  17
/* RMT_CH5_RX_END_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch5_rx_end_int_st.*/
#define RMT_CH5_RX_END_INT_ENA  (BIT(16))
#define RMT_CH5_RX_END_INT_ENA_M  (BIT(16))
#define RMT_CH5_RX_END_INT_ENA_V  0x1
#define RMT_CH5_RX_END_INT_ENA_S  16
/* RMT_CH5_TX_END_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch5_tx_end_int_st.*/
#define RMT_CH5_TX_END_INT_ENA  (BIT(15))
#define RMT_CH5_TX_END_INT_ENA_M  (BIT(15))
#define RMT_CH5_TX_END_INT_ENA_V  0x1
#define RMT_CH5_TX_END_INT_ENA_S  15
/* RMT_CH4_ERR_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch4_err_int_st.*/
#define RMT_CH4_ERR_INT_ENA  (BIT(14))
#define RMT_CH4_ERR_INT_ENA_M  (BIT(14))
#define RMT_CH4_ERR_INT_ENA_V  0x1
#define RMT_CH4_ERR_INT_ENA_S  14
/* RMT_CH4_RX_END_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch4_rx_end_int_st.*/
#define RMT_CH4_RX_END_INT_ENA  (BIT(13))
#define RMT_CH4_RX_END_INT_ENA_M  (BIT(13))
#define RMT_CH4_RX_END_INT_ENA_V  0x1
#define RMT_CH4_RX_END_INT_ENA_S  13
/* RMT_CH4_TX_END_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch4_tx_end_int_st.*/
#define RMT_CH4_TX_END_INT_ENA  (BIT(12))
#define RMT_CH4_TX_END_INT_ENA_M  (BIT(12))
#define RMT_CH4_TX_END_INT_ENA_V  0x1
#define RMT_CH4_TX_END_INT_ENA_S  12
/* RMT_CH3_ERR_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch3_err_int_st.*/
#define RMT_CH3_ERR_INT_ENA  (BIT(11))
#define RMT_CH3_ERR_INT_ENA_M  (BIT(11))
#define RMT_CH3_ERR_INT_ENA_V  0x1
#define RMT_CH3_ERR_INT_ENA_S  11
/* RMT_CH3_RX_END_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch3_rx_end_int_st.*/
#define RMT_CH3_RX_END_INT_ENA  (BIT(10))
#define RMT_CH3_RX_END_INT_ENA_M  (BIT(10))
#define RMT_CH3_RX_END_INT_ENA_V  0x1
#define RMT_CH3_RX_END_INT_ENA_S  10
/* RMT_CH3_TX_END_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch3_tx_end_int_st.*/
#define RMT_CH3_TX_END_INT_ENA  (BIT(9))
#define RMT_CH3_TX_END_INT_ENA_M  (BIT(9))
#define RMT_CH3_TX_END_INT_ENA_V  0x1
#define RMT_CH3_TX_END_INT_ENA_S  9
/* RMT_CH2_ERR_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch2_err_int_st.*/
#define RMT_CH2_ERR_INT_ENA  (BIT(8))
#define RMT_CH2_ERR_INT_ENA_M  (BIT(8))
#define RMT_CH2_ERR_INT_ENA_V  0x1
#define RMT_CH2_ERR_INT_ENA_S  8
/* RMT_CH2_RX_END_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch2_rx_end_int_st.*/
#define RMT_CH2_RX_END_INT_ENA  (BIT(7))
#define RMT_CH2_RX_END_INT_ENA_M  (BIT(7))
#define RMT_CH2_RX_END_INT_ENA_V  0x1
#define RMT_CH2_RX_END_INT_ENA_S  7
/* RMT_CH2_TX_END_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch2_tx_end_int_st.*/
#define RMT_CH2_TX_END_INT_ENA  (BIT(6))
#define RMT_CH2_TX_END_INT_ENA_M  (BIT(6))
#define RMT_CH2_TX_END_INT_ENA_V  0x1
#define RMT_CH2_TX_END_INT_ENA_S  6
/* RMT_CH1_ERR_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch1_err_int_st.*/
#define RMT_CH1_ERR_INT_ENA  (BIT(5))
#define RMT_CH1_ERR_INT_ENA_M  (BIT(5))
#define RMT_CH1_ERR_INT_ENA_V  0x1
#define RMT_CH1_ERR_INT_ENA_S  5
/* RMT_CH1_RX_END_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch1_rx_end_int_st.*/
#define RMT_CH1_RX_END_INT_ENA  (BIT(4))
#define RMT_CH1_RX_END_INT_ENA_M  (BIT(4))
#define RMT_CH1_RX_END_INT_ENA_V  0x1
#define RMT_CH1_RX_END_INT_ENA_S  4
/* RMT_CH1_TX_END_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch1_tx_end_int_st.*/
#define RMT_CH1_TX_END_INT_ENA  (BIT(3))
#define RMT_CH1_TX_END_INT_ENA_M  (BIT(3))
#define RMT_CH1_TX_END_INT_ENA_V  0x1
#define RMT_CH1_TX_END_INT_ENA_S  3
/* RMT_CH0_ERR_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch0_err_int_st.*/
#define RMT_CH0_ERR_INT_ENA  (BIT(2))
#define RMT_CH0_ERR_INT_ENA_M  (BIT(2))
#define RMT_CH0_ERR_INT_ENA_V  0x1
#define RMT_CH0_ERR_INT_ENA_S  2
/* RMT_CH0_RX_END_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch0_rx_end_int_st.*/
#define RMT_CH0_RX_END_INT_ENA  (BIT(1))
#define RMT_CH0_RX_END_INT_ENA_M  (BIT(1))
#define RMT_CH0_RX_END_INT_ENA_V  0x1
#define RMT_CH0_RX_END_INT_ENA_S  1
/* RMT_CH0_TX_END_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to enable rmt_ch0_tx_end_int_st.*/
#define RMT_CH0_TX_END_INT_ENA  (BIT(0))
#define RMT_CH0_TX_END_INT_ENA_M  (BIT(0))
#define RMT_CH0_TX_END_INT_ENA_V  0x1
#define RMT_CH0_TX_END_INT_ENA_S  0

#define RMT_INT_CLR_REG          (DR_REG_RMT_BASE + 0x00ac)
/* RMT_CH7_TX_THR_EVENT_INT_CLR : WO ;bitpos:[31] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch7_tx_thr_event_int_raw interrupt.*/
#define RMT_CH7_TX_THR_EVENT_INT_CLR  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_CLR_M  (BIT(31))
#define RMT_CH7_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH7_TX_THR_EVENT_INT_CLR_S  31
/* RMT_CH6_TX_THR_EVENT_INT_CLR : WO ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch6_tx_thr_event_int_raw interrupt.*/
#define RMT_CH6_TX_THR_EVENT_INT_CLR  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_CLR_M  (BIT(30))
#define RMT_CH6_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH6_TX_THR_EVENT_INT_CLR_S  30
/* RMT_CH5_TX_THR_EVENT_INT_CLR : WO ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch5_tx_thr_event_int_raw interrupt.*/
#define RMT_CH5_TX_THR_EVENT_INT_CLR  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_CLR_M  (BIT(29))
#define RMT_CH5_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH5_TX_THR_EVENT_INT_CLR_S  29
/* RMT_CH4_TX_THR_EVENT_INT_CLR : WO ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch4_tx_thr_event_int_raw interrupt.*/
#define RMT_CH4_TX_THR_EVENT_INT_CLR  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_CLR_M  (BIT(28))
#define RMT_CH4_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH4_TX_THR_EVENT_INT_CLR_S  28
/* RMT_CH3_TX_THR_EVENT_INT_CLR : WO ;bitpos:[27] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch3_tx_thr_event_int_raw interrupt.*/
#define RMT_CH3_TX_THR_EVENT_INT_CLR  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_CLR_M  (BIT(27))
#define RMT_CH3_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH3_TX_THR_EVENT_INT_CLR_S  27
/* RMT_CH2_TX_THR_EVENT_INT_CLR : WO ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch2_tx_thr_event_int_raw interrupt.*/
#define RMT_CH2_TX_THR_EVENT_INT_CLR  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_CLR_M  (BIT(26))
#define RMT_CH2_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH2_TX_THR_EVENT_INT_CLR_S  26
/* RMT_CH1_TX_THR_EVENT_INT_CLR : WO ;bitpos:[25] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch1_tx_thr_event_int_raw interrupt.*/
#define RMT_CH1_TX_THR_EVENT_INT_CLR  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_CLR_M  (BIT(25))
#define RMT_CH1_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH1_TX_THR_EVENT_INT_CLR_S  25
/* RMT_CH0_TX_THR_EVENT_INT_CLR : WO ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch0_tx_thr_event_int_raw interrupt.*/
#define RMT_CH0_TX_THR_EVENT_INT_CLR  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_CLR_M  (BIT(24))
#define RMT_CH0_TX_THR_EVENT_INT_CLR_V  0x1
#define RMT_CH0_TX_THR_EVENT_INT_CLR_S  24
/* RMT_CH7_ERR_INT_CLR : WO ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch7_err_int_raw.*/
#define RMT_CH7_ERR_INT_CLR  (BIT(23))
#define RMT_CH7_ERR_INT_CLR_M  (BIT(23))
#define RMT_CH7_ERR_INT_CLR_V  0x1
#define RMT_CH7_ERR_INT_CLR_S  23
/* RMT_CH7_RX_END_INT_CLR : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch7_tx_end_int_raw.*/
#define RMT_CH7_RX_END_INT_CLR  (BIT(22))
#define RMT_CH7_RX_END_INT_CLR_M  (BIT(22))
#define RMT_CH7_RX_END_INT_CLR_V  0x1
#define RMT_CH7_RX_END_INT_CLR_S  22
/* RMT_CH7_TX_END_INT_CLR : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch7_rx_end_int_raw..*/
#define RMT_CH7_TX_END_INT_CLR  (BIT(21))
#define RMT_CH7_TX_END_INT_CLR_M  (BIT(21))
#define RMT_CH7_TX_END_INT_CLR_V  0x1
#define RMT_CH7_TX_END_INT_CLR_S  21
/* RMT_CH6_ERR_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch6_err_int_raw.*/
#define RMT_CH6_ERR_INT_CLR  (BIT(20))
#define RMT_CH6_ERR_INT_CLR_M  (BIT(20))
#define RMT_CH6_ERR_INT_CLR_V  0x1
#define RMT_CH6_ERR_INT_CLR_S  20
/* RMT_CH6_RX_END_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch6_tx_end_int_raw.*/
#define RMT_CH6_RX_END_INT_CLR  (BIT(19))
#define RMT_CH6_RX_END_INT_CLR_M  (BIT(19))
#define RMT_CH6_RX_END_INT_CLR_V  0x1
#define RMT_CH6_RX_END_INT_CLR_S  19
/* RMT_CH6_TX_END_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch6_rx_end_int_raw..*/
#define RMT_CH6_TX_END_INT_CLR  (BIT(18))
#define RMT_CH6_TX_END_INT_CLR_M  (BIT(18))
#define RMT_CH6_TX_END_INT_CLR_V  0x1
#define RMT_CH6_TX_END_INT_CLR_S  18
/* RMT_CH5_ERR_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch5_err_int_raw.*/
#define RMT_CH5_ERR_INT_CLR  (BIT(17))
#define RMT_CH5_ERR_INT_CLR_M  (BIT(17))
#define RMT_CH5_ERR_INT_CLR_V  0x1
#define RMT_CH5_ERR_INT_CLR_S  17
/* RMT_CH5_RX_END_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch5_tx_end_int_raw.*/
#define RMT_CH5_RX_END_INT_CLR  (BIT(16))
#define RMT_CH5_RX_END_INT_CLR_M  (BIT(16))
#define RMT_CH5_RX_END_INT_CLR_V  0x1
#define RMT_CH5_RX_END_INT_CLR_S  16
/* RMT_CH5_TX_END_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch5_rx_end_int_raw..*/
#define RMT_CH5_TX_END_INT_CLR  (BIT(15))
#define RMT_CH5_TX_END_INT_CLR_M  (BIT(15))
#define RMT_CH5_TX_END_INT_CLR_V  0x1
#define RMT_CH5_TX_END_INT_CLR_S  15
/* RMT_CH4_ERR_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch4_err_int_raw.*/
#define RMT_CH4_ERR_INT_CLR  (BIT(14))
#define RMT_CH4_ERR_INT_CLR_M  (BIT(14))
#define RMT_CH4_ERR_INT_CLR_V  0x1
#define RMT_CH4_ERR_INT_CLR_S  14
/* RMT_CH4_RX_END_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch4_tx_end_int_raw.*/
#define RMT_CH4_RX_END_INT_CLR  (BIT(13))
#define RMT_CH4_RX_END_INT_CLR_M  (BIT(13))
#define RMT_CH4_RX_END_INT_CLR_V  0x1
#define RMT_CH4_RX_END_INT_CLR_S  13
/* RMT_CH4_TX_END_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch4_rx_end_int_raw..*/
#define RMT_CH4_TX_END_INT_CLR  (BIT(12))
#define RMT_CH4_TX_END_INT_CLR_M  (BIT(12))
#define RMT_CH4_TX_END_INT_CLR_V  0x1
#define RMT_CH4_TX_END_INT_CLR_S  12
/* RMT_CH3_ERR_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch3_err_int_raw.*/
#define RMT_CH3_ERR_INT_CLR  (BIT(11))
#define RMT_CH3_ERR_INT_CLR_M  (BIT(11))
#define RMT_CH3_ERR_INT_CLR_V  0x1
#define RMT_CH3_ERR_INT_CLR_S  11
/* RMT_CH3_RX_END_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch3_tx_end_int_raw.*/
#define RMT_CH3_RX_END_INT_CLR  (BIT(10))
#define RMT_CH3_RX_END_INT_CLR_M  (BIT(10))
#define RMT_CH3_RX_END_INT_CLR_V  0x1
#define RMT_CH3_RX_END_INT_CLR_S  10
/* RMT_CH3_TX_END_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch3_rx_end_int_raw..*/
#define RMT_CH3_TX_END_INT_CLR  (BIT(9))
#define RMT_CH3_TX_END_INT_CLR_M  (BIT(9))
#define RMT_CH3_TX_END_INT_CLR_V  0x1
#define RMT_CH3_TX_END_INT_CLR_S  9
/* RMT_CH2_ERR_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch2_err_int_raw.*/
#define RMT_CH2_ERR_INT_CLR  (BIT(8))
#define RMT_CH2_ERR_INT_CLR_M  (BIT(8))
#define RMT_CH2_ERR_INT_CLR_V  0x1
#define RMT_CH2_ERR_INT_CLR_S  8
/* RMT_CH2_RX_END_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch2_tx_end_int_raw.*/
#define RMT_CH2_RX_END_INT_CLR  (BIT(7))
#define RMT_CH2_RX_END_INT_CLR_M  (BIT(7))
#define RMT_CH2_RX_END_INT_CLR_V  0x1
#define RMT_CH2_RX_END_INT_CLR_S  7
/* RMT_CH2_TX_END_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch2_rx_end_int_raw..*/
#define RMT_CH2_TX_END_INT_CLR  (BIT(6))
#define RMT_CH2_TX_END_INT_CLR_M  (BIT(6))
#define RMT_CH2_TX_END_INT_CLR_V  0x1
#define RMT_CH2_TX_END_INT_CLR_S  6
/* RMT_CH1_ERR_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch1_err_int_raw.*/
#define RMT_CH1_ERR_INT_CLR  (BIT(5))
#define RMT_CH1_ERR_INT_CLR_M  (BIT(5))
#define RMT_CH1_ERR_INT_CLR_V  0x1
#define RMT_CH1_ERR_INT_CLR_S  5
/* RMT_CH1_RX_END_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch1_tx_end_int_raw.*/
#define RMT_CH1_RX_END_INT_CLR  (BIT(4))
#define RMT_CH1_RX_END_INT_CLR_M  (BIT(4))
#define RMT_CH1_RX_END_INT_CLR_V  0x1
#define RMT_CH1_RX_END_INT_CLR_S  4
/* RMT_CH1_TX_END_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch1_rx_end_int_raw..*/
#define RMT_CH1_TX_END_INT_CLR  (BIT(3))
#define RMT_CH1_TX_END_INT_CLR_M  (BIT(3))
#define RMT_CH1_TX_END_INT_CLR_V  0x1
#define RMT_CH1_TX_END_INT_CLR_S  3
/* RMT_CH0_ERR_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the  rmt_ch0_err_int_raw.*/
#define RMT_CH0_ERR_INT_CLR  (BIT(2))
#define RMT_CH0_ERR_INT_CLR_M  (BIT(2))
#define RMT_CH0_ERR_INT_CLR_V  0x1
#define RMT_CH0_ERR_INT_CLR_S  2
/* RMT_CH0_RX_END_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch0_tx_end_int_raw.*/
#define RMT_CH0_RX_END_INT_CLR  (BIT(1))
#define RMT_CH0_RX_END_INT_CLR_M  (BIT(1))
#define RMT_CH0_RX_END_INT_CLR_V  0x1
#define RMT_CH0_RX_END_INT_CLR_S  1
/* RMT_CH0_TX_END_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the rmt_ch0_rx_end_int_raw..*/
#define RMT_CH0_TX_END_INT_CLR  (BIT(0))
#define RMT_CH0_TX_END_INT_CLR_M  (BIT(0))
#define RMT_CH0_TX_END_INT_CLR_V  0x1
#define RMT_CH0_TX_END_INT_CLR_S  0

#define RMT_CH0CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00b0)
/* RMT_CARRIER_HIGH_CH0 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel0.*/
#define RMT_CARRIER_HIGH_CH0  0x0000FFFF
#define RMT_CARRIER_HIGH_CH0_M  ((RMT_CARRIER_HIGH_CH0_V)<<(RMT_CARRIER_HIGH_CH0_S))
#define RMT_CARRIER_HIGH_CH0_V  0xFFFF
#define RMT_CARRIER_HIGH_CH0_S  16
/* RMT_CARRIER_LOW_CH0 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel0.*/
#define RMT_CARRIER_LOW_CH0  0x0000FFFF
#define RMT_CARRIER_LOW_CH0_M  ((RMT_CARRIER_LOW_CH0_V)<<(RMT_CARRIER_LOW_CH0_S))
#define RMT_CARRIER_LOW_CH0_V  0xFFFF
#define RMT_CARRIER_LOW_CH0_S  0

#define RMT_CH1CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00b4)
/* RMT_CARRIER_HIGH_CH1 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel1.*/
#define RMT_CARRIER_HIGH_CH1  0x0000FFFF
#define RMT_CARRIER_HIGH_CH1_M  ((RMT_CARRIER_HIGH_CH1_V)<<(RMT_CARRIER_HIGH_CH1_S))
#define RMT_CARRIER_HIGH_CH1_V  0xFFFF
#define RMT_CARRIER_HIGH_CH1_S  16
/* RMT_CARRIER_LOW_CH1 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel1.*/
#define RMT_CARRIER_LOW_CH1  0x0000FFFF
#define RMT_CARRIER_LOW_CH1_M  ((RMT_CARRIER_LOW_CH1_V)<<(RMT_CARRIER_LOW_CH1_S))
#define RMT_CARRIER_LOW_CH1_V  0xFFFF
#define RMT_CARRIER_LOW_CH1_S  0

#define RMT_CH2CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00b8)
/* RMT_CARRIER_HIGH_CH2 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel2.*/
#define RMT_CARRIER_HIGH_CH2  0x0000FFFF
#define RMT_CARRIER_HIGH_CH2_M  ((RMT_CARRIER_HIGH_CH2_V)<<(RMT_CARRIER_HIGH_CH2_S))
#define RMT_CARRIER_HIGH_CH2_V  0xFFFF
#define RMT_CARRIER_HIGH_CH2_S  16
/* RMT_CARRIER_LOW_CH2 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel2.*/
#define RMT_CARRIER_LOW_CH2  0x0000FFFF
#define RMT_CARRIER_LOW_CH2_M  ((RMT_CARRIER_LOW_CH2_V)<<(RMT_CARRIER_LOW_CH2_S))
#define RMT_CARRIER_LOW_CH2_V  0xFFFF
#define RMT_CARRIER_LOW_CH2_S  0

#define RMT_CH3CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00bc)
/* RMT_CARRIER_HIGH_CH3 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel3.*/
#define RMT_CARRIER_HIGH_CH3  0x0000FFFF
#define RMT_CARRIER_HIGH_CH3_M  ((RMT_CARRIER_HIGH_CH3_V)<<(RMT_CARRIER_HIGH_CH3_S))
#define RMT_CARRIER_HIGH_CH3_V  0xFFFF
#define RMT_CARRIER_HIGH_CH3_S  16
/* RMT_CARRIER_LOW_CH3 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel3.*/
#define RMT_CARRIER_LOW_CH3  0x0000FFFF
#define RMT_CARRIER_LOW_CH3_M  ((RMT_CARRIER_LOW_CH3_V)<<(RMT_CARRIER_LOW_CH3_S))
#define RMT_CARRIER_LOW_CH3_V  0xFFFF
#define RMT_CARRIER_LOW_CH3_S  0

#define RMT_CH4CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00c0)
/* RMT_CARRIER_HIGH_CH4 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel4.*/
#define RMT_CARRIER_HIGH_CH4  0x0000FFFF
#define RMT_CARRIER_HIGH_CH4_M  ((RMT_CARRIER_HIGH_CH4_V)<<(RMT_CARRIER_HIGH_CH4_S))
#define RMT_CARRIER_HIGH_CH4_V  0xFFFF
#define RMT_CARRIER_HIGH_CH4_S  16
/* RMT_CARRIER_LOW_CH4 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel4.*/
#define RMT_CARRIER_LOW_CH4  0x0000FFFF
#define RMT_CARRIER_LOW_CH4_M  ((RMT_CARRIER_LOW_CH4_V)<<(RMT_CARRIER_LOW_CH4_S))
#define RMT_CARRIER_LOW_CH4_V  0xFFFF
#define RMT_CARRIER_LOW_CH4_S  0

#define RMT_CH5CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00c4)
/* RMT_CARRIER_HIGH_CH5 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel5.*/
#define RMT_CARRIER_HIGH_CH5  0x0000FFFF
#define RMT_CARRIER_HIGH_CH5_M  ((RMT_CARRIER_HIGH_CH5_V)<<(RMT_CARRIER_HIGH_CH5_S))
#define RMT_CARRIER_HIGH_CH5_V  0xFFFF
#define RMT_CARRIER_HIGH_CH5_S  16
/* RMT_CARRIER_LOW_CH5 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel5.*/
#define RMT_CARRIER_LOW_CH5  0x0000FFFF
#define RMT_CARRIER_LOW_CH5_M  ((RMT_CARRIER_LOW_CH5_V)<<(RMT_CARRIER_LOW_CH5_S))
#define RMT_CARRIER_LOW_CH5_V  0xFFFF
#define RMT_CARRIER_LOW_CH5_S  0

#define RMT_CH6CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00c8)
/* RMT_CARRIER_HIGH_CH6 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel6.*/
#define RMT_CARRIER_HIGH_CH6  0x0000FFFF
#define RMT_CARRIER_HIGH_CH6_M  ((RMT_CARRIER_HIGH_CH6_V)<<(RMT_CARRIER_HIGH_CH6_S))
#define RMT_CARRIER_HIGH_CH6_V  0xFFFF
#define RMT_CARRIER_HIGH_CH6_S  16
/* RMT_CARRIER_LOW_CH6 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel6.*/
#define RMT_CARRIER_LOW_CH6  0x0000FFFF
#define RMT_CARRIER_LOW_CH6_M  ((RMT_CARRIER_LOW_CH6_V)<<(RMT_CARRIER_LOW_CH6_S))
#define RMT_CARRIER_LOW_CH6_V  0xFFFF
#define RMT_CARRIER_LOW_CH6_S  0

#define RMT_CH7CARRIER_DUTY_REG          (DR_REG_RMT_BASE + 0x00cc)
/* RMT_CARRIER_HIGH_CH7 : R/W ;bitpos:[31:16] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's high level value for channel7.*/
#define RMT_CARRIER_HIGH_CH7  0x0000FFFF
#define RMT_CARRIER_HIGH_CH7_M  ((RMT_CARRIER_HIGH_CH7_V)<<(RMT_CARRIER_HIGH_CH7_S))
#define RMT_CARRIER_HIGH_CH7_V  0xFFFF
#define RMT_CARRIER_HIGH_CH7_S  16
/* RMT_CARRIER_LOW_CH7 : R/W ;bitpos:[15:0] ;default: 16'h40 ; */
/*description: This register is used to configure carrier wave's low level value for channel7.*/
#define RMT_CARRIER_LOW_CH7  0x0000FFFF
#define RMT_CARRIER_LOW_CH7_M  ((RMT_CARRIER_LOW_CH7_V)<<(RMT_CARRIER_LOW_CH7_S))
#define RMT_CARRIER_LOW_CH7_V  0xFFFF
#define RMT_CARRIER_LOW_CH7_S  0

#define RMT_CH0_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00d0)
/* RMT_TX_LIM_CH0 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel0 sends more than reg_rmt_tx_lim_ch0 datas then channel0
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH0  0x000001FF
#define RMT_TX_LIM_CH0_M  ((RMT_TX_LIM_CH0_V)<<(RMT_TX_LIM_CH0_S))
#define RMT_TX_LIM_CH0_V  0x1FF
#define RMT_TX_LIM_CH0_S  0

#define RMT_CH1_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00d4)
/* RMT_TX_LIM_CH1 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel1 sends more than reg_rmt_tx_lim_ch1 datas then channel1
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH1  0x000001FF
#define RMT_TX_LIM_CH1_M  ((RMT_TX_LIM_CH1_V)<<(RMT_TX_LIM_CH1_S))
#define RMT_TX_LIM_CH1_V  0x1FF
#define RMT_TX_LIM_CH1_S  0

#define RMT_CH2_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00d8)
/* RMT_TX_LIM_CH2 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel2 sends more than reg_rmt_tx_lim_ch2 datas then channel2
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH2  0x000001FF
#define RMT_TX_LIM_CH2_M  ((RMT_TX_LIM_CH2_V)<<(RMT_TX_LIM_CH2_S))
#define RMT_TX_LIM_CH2_V  0x1FF
#define RMT_TX_LIM_CH2_S  0

#define RMT_CH3_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00dc)
/* RMT_TX_LIM_CH3 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel3 sends more than reg_rmt_tx_lim_ch3 datas then channel3
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH3  0x000001FF
#define RMT_TX_LIM_CH3_M  ((RMT_TX_LIM_CH3_V)<<(RMT_TX_LIM_CH3_S))
#define RMT_TX_LIM_CH3_V  0x1FF
#define RMT_TX_LIM_CH3_S  0

#define RMT_CH4_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00e0)
/* RMT_TX_LIM_CH4 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel4 sends more than reg_rmt_tx_lim_ch4 datas then channel4
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH4  0x000001FF
#define RMT_TX_LIM_CH4_M  ((RMT_TX_LIM_CH4_V)<<(RMT_TX_LIM_CH4_S))
#define RMT_TX_LIM_CH4_V  0x1FF
#define RMT_TX_LIM_CH4_S  0

#define RMT_CH5_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00e4)
/* RMT_TX_LIM_CH5 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel5 sends more than reg_rmt_tx_lim_ch5 datas then channel5
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH5  0x000001FF
#define RMT_TX_LIM_CH5_M  ((RMT_TX_LIM_CH5_V)<<(RMT_TX_LIM_CH5_S))
#define RMT_TX_LIM_CH5_V  0x1FF
#define RMT_TX_LIM_CH5_S  0

#define RMT_CH6_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00e8)
/* RMT_TX_LIM_CH6 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel6 sends more than reg_rmt_tx_lim_ch6 datas then channel6
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH6  0x000001FF
#define RMT_TX_LIM_CH6_M  ((RMT_TX_LIM_CH6_V)<<(RMT_TX_LIM_CH6_S))
#define RMT_TX_LIM_CH6_V  0x1FF
#define RMT_TX_LIM_CH6_S  0

#define RMT_CH7_TX_LIM_REG          (DR_REG_RMT_BASE + 0x00ec)
/* RMT_TX_LIM_CH7 : R/W ;bitpos:[8:0] ;default: 9'h80 ; */
/*description: When channel7 sends more than reg_rmt_tx_lim_ch7 datas then channel7
 produce the relative interrupt.*/
#define RMT_TX_LIM_CH7  0x000001FF
#define RMT_TX_LIM_CH7_M  ((RMT_TX_LIM_CH7_V)<<(RMT_TX_LIM_CH7_S))
#define RMT_TX_LIM_CH7_V  0x1FF
#define RMT_TX_LIM_CH7_S  0

#define RMT_APB_CONF_REG          (DR_REG_RMT_BASE + 0x00f0)
/* RMT_MEM_TX_WRAP_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: when datas need to be send is more than channel's mem can store
  then set this bit to enable reusage of mem this bit is used together with reg_rmt_tx_lim_chn.*/
#define RMT_MEM_TX_WRAP_EN  (BIT(1))
#define RMT_MEM_TX_WRAP_EN_M  (BIT(1))
#define RMT_MEM_TX_WRAP_EN_V  0x1
#define RMT_MEM_TX_WRAP_EN_S  1
/* RMT_APB_FIFO_MASK : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to disable apb fifo access*/
#define RMT_APB_FIFO_MASK  (BIT(0))
#define RMT_APB_FIFO_MASK_M  (BIT(0))
#define RMT_APB_FIFO_MASK_V  0x1
#define RMT_APB_FIFO_MASK_S  0

#define RMT_DATE_REG          (DR_REG_RMT_BASE + 0x00fc)
/* RMT_DATE : R/W ;bitpos:[31:0] ;default: 32'h16022600 ; */
/*description: This is the version register.*/
#define RMT_DATE  0xFFFFFFFF
#define RMT_DATE_M  ((RMT_DATE_V)<<(RMT_DATE_S))
#define RMT_DATE_V  0xFFFFFFFF
#define RMT_DATE_S  0

/* RMT memory block address */
#define RMT_CHANNEL_MEM(i)  (DR_REG_RMT_BASE + 0x800 + 64 * 4 * (i))


#endif /*_SOC_RMT_REG_H_ */


