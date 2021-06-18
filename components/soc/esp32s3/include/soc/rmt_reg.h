/** Copyright 2021 Espressif Systems (Shanghai) PTE LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** RMT_CH0DATA_REG register
 *  The read and write  data register for CHANNEL0 by apb fifo access.
 */
#define RMT_CH0DATA_REG (DR_REG_RMT_BASE + 0x0)
/** RMT_CH0DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel 0 via APB FIFO.
 */
#define RMT_CH0DATA    0xFFFFFFFFU
#define RMT_CH0DATA_M  (RMT_CH0DATA_V << RMT_CH0DATA_S)
#define RMT_CH0DATA_V  0xFFFFFFFFU
#define RMT_CH0DATA_S  0

/** RMT_CH1DATA_REG register
 *  The read and write  data register for CHANNEL1 by apb fifo access.
 */
#define RMT_CH1DATA_REG (DR_REG_RMT_BASE + 0x4)
/** RMT_CH1DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel 1 via APB FIFO.
 */
#define RMT_CH1DATA    0xFFFFFFFFU
#define RMT_CH1DATA_M  (RMT_CH1DATA_V << RMT_CH1DATA_S)
#define RMT_CH1DATA_V  0xFFFFFFFFU
#define RMT_CH1DATA_S  0

/** RMT_CH2DATA_REG register
 *  The read and write  data register for CHANNEL2 by apb fifo access.
 */
#define RMT_CH2DATA_REG (DR_REG_RMT_BASE + 0x8)
/** RMT_CH2DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel 2 via APB FIFO.
 */
#define RMT_CH2DATA    0xFFFFFFFFU
#define RMT_CH2DATA_M  (RMT_CH2DATA_V << RMT_CH2DATA_S)
#define RMT_CH2DATA_V  0xFFFFFFFFU
#define RMT_CH2DATA_S  0

/** RMT_CH3DATA_REG register
 *  The read and write  data register for CHANNEL3 by apb fifo access.
 */
#define RMT_CH3DATA_REG (DR_REG_RMT_BASE + 0xc)
/** RMT_CH3DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel 3 via APB FIFO.
 */
#define RMT_CH3DATA    0xFFFFFFFFU
#define RMT_CH3DATA_M  (RMT_CH3DATA_V << RMT_CH3DATA_S)
#define RMT_CH3DATA_V  0xFFFFFFFFU
#define RMT_CH3DATA_S  0

/** RMT_CH4DATA_REG register
 *  The read and write  data register for CHANNEL$n by apb fifo access.
 */
#define RMT_CH4DATA_REG (DR_REG_RMT_BASE + 0x10)
/** RMT_CH4DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel $n via APB FIFO.
 */
#define RMT_CH4DATA    0xFFFFFFFFU
#define RMT_CH4DATA_M  (RMT_CH4DATA_V << RMT_CH4DATA_S)
#define RMT_CH4DATA_V  0xFFFFFFFFU
#define RMT_CH4DATA_S  0

/** RMT_CH5DATA_REG register
 *  The read and write  data register for CHANNEL$n by apb fifo access.
 */
#define RMT_CH5DATA_REG (DR_REG_RMT_BASE + 0x14)
/** RMT_CH5DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel $n via APB FIFO.
 */
#define RMT_CH5DATA    0xFFFFFFFFU
#define RMT_CH5DATA_M  (RMT_CH5DATA_V << RMT_CH5DATA_S)
#define RMT_CH5DATA_V  0xFFFFFFFFU
#define RMT_CH5DATA_S  0

/** RMT_CH6DATA_REG register
 *  The read and write  data register for CHANNEL$n by apb fifo access.
 */
#define RMT_CH6DATA_REG (DR_REG_RMT_BASE + 0x18)
/** RMT_CH6DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel $n via APB FIFO.
 */
#define RMT_CH6DATA    0xFFFFFFFFU
#define RMT_CH6DATA_M  (RMT_CH6DATA_V << RMT_CH6DATA_S)
#define RMT_CH6DATA_V  0xFFFFFFFFU
#define RMT_CH6DATA_S  0

/** RMT_CH7DATA_REG register
 *  The read and write  data register for CHANNEL$n by apb fifo access.
 */
#define RMT_CH7DATA_REG (DR_REG_RMT_BASE + 0x1c)
/** RMT_CH7DATA : RO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel $n via APB FIFO.
 */
#define RMT_CH7DATA    0xFFFFFFFFU
#define RMT_CH7DATA_M  (RMT_CH7DATA_V << RMT_CH7DATA_S)
#define RMT_CH7DATA_V  0xFFFFFFFFU
#define RMT_CH7DATA_S  0

/** RMT_CH0CONF0_REG register
 *  Channel 0 configure register 0
 */
#define RMT_CH0CONF0_REG (DR_REG_RMT_BASE + 0x20)
/** RMT_TX_START_0 : WT; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on CHANNEL0.
 */
#define RMT_TX_START_0    (BIT(0))
#define RMT_TX_START_0_M  (RMT_TX_START_0_V << RMT_TX_START_0_S)
#define RMT_TX_START_0_V  0x00000001U
#define RMT_TX_START_0_S  0
/** RMT_MEM_RD_RST_0 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset read ram address for CHANNEL0 by accessing transmitter.
 */
#define RMT_MEM_RD_RST_0    (BIT(1))
#define RMT_MEM_RD_RST_0_M  (RMT_MEM_RD_RST_0_V << RMT_MEM_RD_RST_0_S)
#define RMT_MEM_RD_RST_0_V  0x00000001U
#define RMT_MEM_RD_RST_0_S  1
/** RMT_APB_MEM_RST_0 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL0 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_0    (BIT(2))
#define RMT_APB_MEM_RST_0_M  (RMT_APB_MEM_RST_0_V << RMT_APB_MEM_RST_0_S)
#define RMT_APB_MEM_RST_0_V  0x00000001U
#define RMT_APB_MEM_RST_0_S  2
/** RMT_TX_CONTI_MODE_0 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to restart transmission  from the first data to the last data in
 *  CHANNEL0.
 */
#define RMT_TX_CONTI_MODE_0    (BIT(3))
#define RMT_TX_CONTI_MODE_0_M  (RMT_TX_CONTI_MODE_0_V << RMT_TX_CONTI_MODE_0_S)
#define RMT_TX_CONTI_MODE_0_V  0x00000001U
#define RMT_TX_CONTI_MODE_0_S  3
/** RMT_MEM_TX_WRAP_EN_0 : R/W; bitpos: [4]; default: 0;
 *  This is the channel 0 enable bit for wraparound mode: it will resume sending at the
 *  start when the data to be sent is more than its memory size.
 */
#define RMT_MEM_TX_WRAP_EN_0    (BIT(4))
#define RMT_MEM_TX_WRAP_EN_0_M  (RMT_MEM_TX_WRAP_EN_0_V << RMT_MEM_TX_WRAP_EN_0_S)
#define RMT_MEM_TX_WRAP_EN_0_V  0x00000001U
#define RMT_MEM_TX_WRAP_EN_0_S  4
/** RMT_IDLE_OUT_LV_0 : R/W; bitpos: [5]; default: 0;
 *  This bit configures the level of output signal in CHANNEL0 when the latter is in
 *  IDLE state.
 */
#define RMT_IDLE_OUT_LV_0    (BIT(5))
#define RMT_IDLE_OUT_LV_0_M  (RMT_IDLE_OUT_LV_0_V << RMT_IDLE_OUT_LV_0_S)
#define RMT_IDLE_OUT_LV_0_V  0x00000001U
#define RMT_IDLE_OUT_LV_0_S  5
/** RMT_IDLE_OUT_EN_0 : R/W; bitpos: [6]; default: 0;
 *  This is the output enable-control bit for CHANNEL0 in IDLE state.
 */
#define RMT_IDLE_OUT_EN_0    (BIT(6))
#define RMT_IDLE_OUT_EN_0_M  (RMT_IDLE_OUT_EN_0_V << RMT_IDLE_OUT_EN_0_S)
#define RMT_IDLE_OUT_EN_0_V  0x00000001U
#define RMT_IDLE_OUT_EN_0_S  6
/** RMT_TX_STOP_0 : R/W/SC; bitpos: [7]; default: 0;
 *  Set this bit to stop the transmitter of CHANNEL0 sending data out.
 */
#define RMT_TX_STOP_0    (BIT(7))
#define RMT_TX_STOP_0_M  (RMT_TX_STOP_0_V << RMT_TX_STOP_0_S)
#define RMT_TX_STOP_0_V  0x00000001U
#define RMT_TX_STOP_0_S  7
/** RMT_DIV_CNT_0 : R/W; bitpos: [15:8]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL0.
 */
#define RMT_DIV_CNT_0    0x000000FFU
#define RMT_DIV_CNT_0_M  (RMT_DIV_CNT_0_V << RMT_DIV_CNT_0_S)
#define RMT_DIV_CNT_0_V  0x000000FFU
#define RMT_DIV_CNT_0_S  8
/** RMT_MEM_SIZE_0 : R/W; bitpos: [19:16]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL0.
 */
#define RMT_MEM_SIZE_0    0x0000000FU
#define RMT_MEM_SIZE_0_M  (RMT_MEM_SIZE_0_V << RMT_MEM_SIZE_0_S)
#define RMT_MEM_SIZE_0_V  0x0000000FU
#define RMT_MEM_SIZE_0_S  16
/** RMT_CARRIER_EFF_EN_0 : R/W; bitpos: [20]; default: 1;
 *  1: Add carrier modulation on the output signal only at the send data state for
 *  CHANNEL0. 0: Add carrier modulation on the output signal at all state for CHANNEL0.
 *  Only valid when RMT_CARRIER_EN_CH0 is 1.
 */
#define RMT_CARRIER_EFF_EN_0    (BIT(20))
#define RMT_CARRIER_EFF_EN_0_M  (RMT_CARRIER_EFF_EN_0_V << RMT_CARRIER_EFF_EN_0_S)
#define RMT_CARRIER_EFF_EN_0_V  0x00000001U
#define RMT_CARRIER_EFF_EN_0_S  20
/** RMT_CARRIER_EN_0 : R/W; bitpos: [21]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL0. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_0    (BIT(21))
#define RMT_CARRIER_EN_0_M  (RMT_CARRIER_EN_0_V << RMT_CARRIER_EN_0_S)
#define RMT_CARRIER_EN_0_V  0x00000001U
#define RMT_CARRIER_EN_0_S  21
/** RMT_CARRIER_OUT_LV_0 : R/W; bitpos: [22]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL0.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_0    (BIT(22))
#define RMT_CARRIER_OUT_LV_0_M  (RMT_CARRIER_OUT_LV_0_V << RMT_CARRIER_OUT_LV_0_S)
#define RMT_CARRIER_OUT_LV_0_V  0x00000001U
#define RMT_CARRIER_OUT_LV_0_S  22
/** RMT_AFIFO_RST_0 : WT; bitpos: [23]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_0    (BIT(23))
#define RMT_AFIFO_RST_0_M  (RMT_AFIFO_RST_0_V << RMT_AFIFO_RST_0_S)
#define RMT_AFIFO_RST_0_V  0x00000001U
#define RMT_AFIFO_RST_0_S  23
/** RMT_CONF_UPDATE_0 : WT; bitpos: [24]; default: 0;
 *  synchronization bit for CHANNEL0
 */
#define RMT_CONF_UPDATE_0    (BIT(24))
#define RMT_CONF_UPDATE_0_M  (RMT_CONF_UPDATE_0_V << RMT_CONF_UPDATE_0_S)
#define RMT_CONF_UPDATE_0_V  0x00000001U
#define RMT_CONF_UPDATE_0_S  24

/** RMT_CH1CONF0_REG register
 *  Channel 1 configure register 0
 */
#define RMT_CH1CONF0_REG (DR_REG_RMT_BASE + 0x24)
/** RMT_TX_START_1 : WT; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on CHANNEL1.
 */
#define RMT_TX_START_1    (BIT(0))
#define RMT_TX_START_1_M  (RMT_TX_START_1_V << RMT_TX_START_1_S)
#define RMT_TX_START_1_V  0x00000001U
#define RMT_TX_START_1_S  0
/** RMT_MEM_RD_RST_1 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset read ram address for CHANNEL1 by accessing transmitter.
 */
#define RMT_MEM_RD_RST_1    (BIT(1))
#define RMT_MEM_RD_RST_1_M  (RMT_MEM_RD_RST_1_V << RMT_MEM_RD_RST_1_S)
#define RMT_MEM_RD_RST_1_V  0x00000001U
#define RMT_MEM_RD_RST_1_S  1
/** RMT_APB_MEM_RST_1 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL1 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_1    (BIT(2))
#define RMT_APB_MEM_RST_1_M  (RMT_APB_MEM_RST_1_V << RMT_APB_MEM_RST_1_S)
#define RMT_APB_MEM_RST_1_V  0x00000001U
#define RMT_APB_MEM_RST_1_S  2
/** RMT_TX_CONTI_MODE_1 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to restart transmission  from the first data to the last data in
 *  CHANNEL1.
 */
#define RMT_TX_CONTI_MODE_1    (BIT(3))
#define RMT_TX_CONTI_MODE_1_M  (RMT_TX_CONTI_MODE_1_V << RMT_TX_CONTI_MODE_1_S)
#define RMT_TX_CONTI_MODE_1_V  0x00000001U
#define RMT_TX_CONTI_MODE_1_S  3
/** RMT_MEM_TX_WRAP_EN_1 : R/W; bitpos: [4]; default: 0;
 *  This is the channel 1 enable bit for wraparound mode: it will resume sending at the
 *  start when the data to be sent is more than its memory size.
 */
#define RMT_MEM_TX_WRAP_EN_1    (BIT(4))
#define RMT_MEM_TX_WRAP_EN_1_M  (RMT_MEM_TX_WRAP_EN_1_V << RMT_MEM_TX_WRAP_EN_1_S)
#define RMT_MEM_TX_WRAP_EN_1_V  0x00000001U
#define RMT_MEM_TX_WRAP_EN_1_S  4
/** RMT_IDLE_OUT_LV_1 : R/W; bitpos: [5]; default: 0;
 *  This bit configures the level of output signal in CHANNEL1 when the latter is in
 *  IDLE state.
 */
#define RMT_IDLE_OUT_LV_1    (BIT(5))
#define RMT_IDLE_OUT_LV_1_M  (RMT_IDLE_OUT_LV_1_V << RMT_IDLE_OUT_LV_1_S)
#define RMT_IDLE_OUT_LV_1_V  0x00000001U
#define RMT_IDLE_OUT_LV_1_S  5
/** RMT_IDLE_OUT_EN_1 : R/W; bitpos: [6]; default: 0;
 *  This is the output enable-control bit for CHANNEL1 in IDLE state.
 */
#define RMT_IDLE_OUT_EN_1    (BIT(6))
#define RMT_IDLE_OUT_EN_1_M  (RMT_IDLE_OUT_EN_1_V << RMT_IDLE_OUT_EN_1_S)
#define RMT_IDLE_OUT_EN_1_V  0x00000001U
#define RMT_IDLE_OUT_EN_1_S  6
/** RMT_TX_STOP_1 : R/W/SC; bitpos: [7]; default: 0;
 *  Set this bit to stop the transmitter of CHANNEL1 sending data out.
 */
#define RMT_TX_STOP_1    (BIT(7))
#define RMT_TX_STOP_1_M  (RMT_TX_STOP_1_V << RMT_TX_STOP_1_S)
#define RMT_TX_STOP_1_V  0x00000001U
#define RMT_TX_STOP_1_S  7
/** RMT_DIV_CNT_1 : R/W; bitpos: [15:8]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL1.
 */
#define RMT_DIV_CNT_1    0x000000FFU
#define RMT_DIV_CNT_1_M  (RMT_DIV_CNT_1_V << RMT_DIV_CNT_1_S)
#define RMT_DIV_CNT_1_V  0x000000FFU
#define RMT_DIV_CNT_1_S  8
/** RMT_MEM_SIZE_1 : R/W; bitpos: [19:16]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL1.
 */
#define RMT_MEM_SIZE_1    0x0000000FU
#define RMT_MEM_SIZE_1_M  (RMT_MEM_SIZE_1_V << RMT_MEM_SIZE_1_S)
#define RMT_MEM_SIZE_1_V  0x0000000FU
#define RMT_MEM_SIZE_1_S  16
/** RMT_CARRIER_EFF_EN_1 : R/W; bitpos: [20]; default: 1;
 *  1: Add carrier modulation on the output signal only at the send data state for
 *  CHANNEL1. 0: Add carrier modulation on the output signal at all state for CHANNEL1.
 *  Only valid when RMT_CARRIER_EN_CH1 is 1.
 */
#define RMT_CARRIER_EFF_EN_1    (BIT(20))
#define RMT_CARRIER_EFF_EN_1_M  (RMT_CARRIER_EFF_EN_1_V << RMT_CARRIER_EFF_EN_1_S)
#define RMT_CARRIER_EFF_EN_1_V  0x00000001U
#define RMT_CARRIER_EFF_EN_1_S  20
/** RMT_CARRIER_EN_1 : R/W; bitpos: [21]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL1. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_1    (BIT(21))
#define RMT_CARRIER_EN_1_M  (RMT_CARRIER_EN_1_V << RMT_CARRIER_EN_1_S)
#define RMT_CARRIER_EN_1_V  0x00000001U
#define RMT_CARRIER_EN_1_S  21
/** RMT_CARRIER_OUT_LV_1 : R/W; bitpos: [22]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL1.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_1    (BIT(22))
#define RMT_CARRIER_OUT_LV_1_M  (RMT_CARRIER_OUT_LV_1_V << RMT_CARRIER_OUT_LV_1_S)
#define RMT_CARRIER_OUT_LV_1_V  0x00000001U
#define RMT_CARRIER_OUT_LV_1_S  22
/** RMT_AFIFO_RST_1 : WT; bitpos: [23]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_1    (BIT(23))
#define RMT_AFIFO_RST_1_M  (RMT_AFIFO_RST_1_V << RMT_AFIFO_RST_1_S)
#define RMT_AFIFO_RST_1_V  0x00000001U
#define RMT_AFIFO_RST_1_S  23
/** RMT_CONF_UPDATE_1 : WT; bitpos: [24]; default: 0;
 *  synchronization bit for CHANNEL1
 */
#define RMT_CONF_UPDATE_1    (BIT(24))
#define RMT_CONF_UPDATE_1_M  (RMT_CONF_UPDATE_1_V << RMT_CONF_UPDATE_1_S)
#define RMT_CONF_UPDATE_1_V  0x00000001U
#define RMT_CONF_UPDATE_1_S  24

/** RMT_CH2CONF0_REG register
 *  Channel 2 configure register 0
 */
#define RMT_CH2CONF0_REG (DR_REG_RMT_BASE + 0x28)
/** RMT_TX_START_2 : WT; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on CHANNEL2.
 */
#define RMT_TX_START_2    (BIT(0))
#define RMT_TX_START_2_M  (RMT_TX_START_2_V << RMT_TX_START_2_S)
#define RMT_TX_START_2_V  0x00000001U
#define RMT_TX_START_2_S  0
/** RMT_MEM_RD_RST_2 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset read ram address for CHANNEL2 by accessing transmitter.
 */
#define RMT_MEM_RD_RST_2    (BIT(1))
#define RMT_MEM_RD_RST_2_M  (RMT_MEM_RD_RST_2_V << RMT_MEM_RD_RST_2_S)
#define RMT_MEM_RD_RST_2_V  0x00000001U
#define RMT_MEM_RD_RST_2_S  1
/** RMT_APB_MEM_RST_2 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL2 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_2    (BIT(2))
#define RMT_APB_MEM_RST_2_M  (RMT_APB_MEM_RST_2_V << RMT_APB_MEM_RST_2_S)
#define RMT_APB_MEM_RST_2_V  0x00000001U
#define RMT_APB_MEM_RST_2_S  2
/** RMT_TX_CONTI_MODE_2 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to restart transmission  from the first data to the last data in
 *  CHANNEL2.
 */
#define RMT_TX_CONTI_MODE_2    (BIT(3))
#define RMT_TX_CONTI_MODE_2_M  (RMT_TX_CONTI_MODE_2_V << RMT_TX_CONTI_MODE_2_S)
#define RMT_TX_CONTI_MODE_2_V  0x00000001U
#define RMT_TX_CONTI_MODE_2_S  3
/** RMT_MEM_TX_WRAP_EN_2 : R/W; bitpos: [4]; default: 0;
 *  This is the channel 2 enable bit for wraparound mode: it will resume sending at the
 *  start when the data to be sent is more than its memory size.
 */
#define RMT_MEM_TX_WRAP_EN_2    (BIT(4))
#define RMT_MEM_TX_WRAP_EN_2_M  (RMT_MEM_TX_WRAP_EN_2_V << RMT_MEM_TX_WRAP_EN_2_S)
#define RMT_MEM_TX_WRAP_EN_2_V  0x00000001U
#define RMT_MEM_TX_WRAP_EN_2_S  4
/** RMT_IDLE_OUT_LV_2 : R/W; bitpos: [5]; default: 0;
 *  This bit configures the level of output signal in CHANNEL2 when the latter is in
 *  IDLE state.
 */
#define RMT_IDLE_OUT_LV_2    (BIT(5))
#define RMT_IDLE_OUT_LV_2_M  (RMT_IDLE_OUT_LV_2_V << RMT_IDLE_OUT_LV_2_S)
#define RMT_IDLE_OUT_LV_2_V  0x00000001U
#define RMT_IDLE_OUT_LV_2_S  5
/** RMT_IDLE_OUT_EN_2 : R/W; bitpos: [6]; default: 0;
 *  This is the output enable-control bit for CHANNEL2 in IDLE state.
 */
#define RMT_IDLE_OUT_EN_2    (BIT(6))
#define RMT_IDLE_OUT_EN_2_M  (RMT_IDLE_OUT_EN_2_V << RMT_IDLE_OUT_EN_2_S)
#define RMT_IDLE_OUT_EN_2_V  0x00000001U
#define RMT_IDLE_OUT_EN_2_S  6
/** RMT_TX_STOP_2 : R/W/SC; bitpos: [7]; default: 0;
 *  Set this bit to stop the transmitter of CHANNEL2 sending data out.
 */
#define RMT_TX_STOP_2    (BIT(7))
#define RMT_TX_STOP_2_M  (RMT_TX_STOP_2_V << RMT_TX_STOP_2_S)
#define RMT_TX_STOP_2_V  0x00000001U
#define RMT_TX_STOP_2_S  7
/** RMT_DIV_CNT_2 : R/W; bitpos: [15:8]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL2.
 */
#define RMT_DIV_CNT_2    0x000000FFU
#define RMT_DIV_CNT_2_M  (RMT_DIV_CNT_2_V << RMT_DIV_CNT_2_S)
#define RMT_DIV_CNT_2_V  0x000000FFU
#define RMT_DIV_CNT_2_S  8
/** RMT_MEM_SIZE_2 : R/W; bitpos: [19:16]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL2.
 */
#define RMT_MEM_SIZE_2    0x0000000FU
#define RMT_MEM_SIZE_2_M  (RMT_MEM_SIZE_2_V << RMT_MEM_SIZE_2_S)
#define RMT_MEM_SIZE_2_V  0x0000000FU
#define RMT_MEM_SIZE_2_S  16
/** RMT_CARRIER_EFF_EN_2 : R/W; bitpos: [20]; default: 1;
 *  1: Add carrier modulation on the output signal only at the send data state for
 *  CHANNEL2. 0: Add carrier modulation on the output signal at all state for CHANNEL2.
 *  Only valid when RMT_CARRIER_EN_CH2 is 1.
 */
#define RMT_CARRIER_EFF_EN_2    (BIT(20))
#define RMT_CARRIER_EFF_EN_2_M  (RMT_CARRIER_EFF_EN_2_V << RMT_CARRIER_EFF_EN_2_S)
#define RMT_CARRIER_EFF_EN_2_V  0x00000001U
#define RMT_CARRIER_EFF_EN_2_S  20
/** RMT_CARRIER_EN_2 : R/W; bitpos: [21]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL2. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_2    (BIT(21))
#define RMT_CARRIER_EN_2_M  (RMT_CARRIER_EN_2_V << RMT_CARRIER_EN_2_S)
#define RMT_CARRIER_EN_2_V  0x00000001U
#define RMT_CARRIER_EN_2_S  21
/** RMT_CARRIER_OUT_LV_2 : R/W; bitpos: [22]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL2.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_2    (BIT(22))
#define RMT_CARRIER_OUT_LV_2_M  (RMT_CARRIER_OUT_LV_2_V << RMT_CARRIER_OUT_LV_2_S)
#define RMT_CARRIER_OUT_LV_2_V  0x00000001U
#define RMT_CARRIER_OUT_LV_2_S  22
/** RMT_AFIFO_RST_2 : WT; bitpos: [23]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_2    (BIT(23))
#define RMT_AFIFO_RST_2_M  (RMT_AFIFO_RST_2_V << RMT_AFIFO_RST_2_S)
#define RMT_AFIFO_RST_2_V  0x00000001U
#define RMT_AFIFO_RST_2_S  23
/** RMT_CONF_UPDATE_2 : WT; bitpos: [24]; default: 0;
 *  synchronization bit for CHANNEL2
 */
#define RMT_CONF_UPDATE_2    (BIT(24))
#define RMT_CONF_UPDATE_2_M  (RMT_CONF_UPDATE_2_V << RMT_CONF_UPDATE_2_S)
#define RMT_CONF_UPDATE_2_V  0x00000001U
#define RMT_CONF_UPDATE_2_S  24

/** RMT_CH3CONF0_REG register
 *  Channel 3 configure register 0
 */
#define RMT_CH3CONF0_REG (DR_REG_RMT_BASE + 0x2c)
/** RMT_TX_START_3 : WT; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on CHANNEL3.
 */
#define RMT_TX_START_3    (BIT(0))
#define RMT_TX_START_3_M  (RMT_TX_START_3_V << RMT_TX_START_3_S)
#define RMT_TX_START_3_V  0x00000001U
#define RMT_TX_START_3_S  0
/** RMT_MEM_RD_RST_3 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset read ram address for CHANNEL3 by accessing transmitter.
 */
#define RMT_MEM_RD_RST_3    (BIT(1))
#define RMT_MEM_RD_RST_3_M  (RMT_MEM_RD_RST_3_V << RMT_MEM_RD_RST_3_S)
#define RMT_MEM_RD_RST_3_V  0x00000001U
#define RMT_MEM_RD_RST_3_S  1
/** RMT_APB_MEM_RST_3 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL3 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_3    (BIT(2))
#define RMT_APB_MEM_RST_3_M  (RMT_APB_MEM_RST_3_V << RMT_APB_MEM_RST_3_S)
#define RMT_APB_MEM_RST_3_V  0x00000001U
#define RMT_APB_MEM_RST_3_S  2
/** RMT_TX_CONTI_MODE_3 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to restart transmission  from the first data to the last data in
 *  CHANNEL3.
 */
#define RMT_TX_CONTI_MODE_3    (BIT(3))
#define RMT_TX_CONTI_MODE_3_M  (RMT_TX_CONTI_MODE_3_V << RMT_TX_CONTI_MODE_3_S)
#define RMT_TX_CONTI_MODE_3_V  0x00000001U
#define RMT_TX_CONTI_MODE_3_S  3
/** RMT_MEM_TX_WRAP_EN_3 : R/W; bitpos: [4]; default: 0;
 *  This is the channel 3 enable bit for wraparound mode: it will resume sending at the
 *  start when the data to be sent is more than its memory size.
 */
#define RMT_MEM_TX_WRAP_EN_3    (BIT(4))
#define RMT_MEM_TX_WRAP_EN_3_M  (RMT_MEM_TX_WRAP_EN_3_V << RMT_MEM_TX_WRAP_EN_3_S)
#define RMT_MEM_TX_WRAP_EN_3_V  0x00000001U
#define RMT_MEM_TX_WRAP_EN_3_S  4
/** RMT_IDLE_OUT_LV_3 : R/W; bitpos: [5]; default: 0;
 *  This bit configures the level of output signal in CHANNEL3 when the latter is in
 *  IDLE state.
 */
#define RMT_IDLE_OUT_LV_3    (BIT(5))
#define RMT_IDLE_OUT_LV_3_M  (RMT_IDLE_OUT_LV_3_V << RMT_IDLE_OUT_LV_3_S)
#define RMT_IDLE_OUT_LV_3_V  0x00000001U
#define RMT_IDLE_OUT_LV_3_S  5
/** RMT_IDLE_OUT_EN_3 : R/W; bitpos: [6]; default: 0;
 *  This is the output enable-control bit for CHANNEL3 in IDLE state.
 */
#define RMT_IDLE_OUT_EN_3    (BIT(6))
#define RMT_IDLE_OUT_EN_3_M  (RMT_IDLE_OUT_EN_3_V << RMT_IDLE_OUT_EN_3_S)
#define RMT_IDLE_OUT_EN_3_V  0x00000001U
#define RMT_IDLE_OUT_EN_3_S  6
/** RMT_TX_STOP_3 : R/W/SC; bitpos: [7]; default: 0;
 *  Set this bit to stop the transmitter of CHANNEL3 sending data out.
 */
#define RMT_TX_STOP_3    (BIT(7))
#define RMT_TX_STOP_3_M  (RMT_TX_STOP_3_V << RMT_TX_STOP_3_S)
#define RMT_TX_STOP_3_V  0x00000001U
#define RMT_TX_STOP_3_S  7
/** RMT_DIV_CNT_3 : R/W; bitpos: [15:8]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL3.
 */
#define RMT_DIV_CNT_3    0x000000FFU
#define RMT_DIV_CNT_3_M  (RMT_DIV_CNT_3_V << RMT_DIV_CNT_3_S)
#define RMT_DIV_CNT_3_V  0x000000FFU
#define RMT_DIV_CNT_3_S  8
/** RMT_MEM_SIZE_3 : R/W; bitpos: [19:16]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL3.
 */
#define RMT_MEM_SIZE_3    0x0000000FU
#define RMT_MEM_SIZE_3_M  (RMT_MEM_SIZE_3_V << RMT_MEM_SIZE_3_S)
#define RMT_MEM_SIZE_3_V  0x0000000FU
#define RMT_MEM_SIZE_3_S  16
/** RMT_CARRIER_EFF_EN_3 : R/W; bitpos: [20]; default: 1;
 *  1: Add carrier modulation on the output signal only at the send data state for
 *  CHANNEL3. 0: Add carrier modulation on the output signal at all state for CHANNEL3.
 *  Only valid when RMT_CARRIER_EN_CH3 is 1.
 */
#define RMT_CARRIER_EFF_EN_3    (BIT(20))
#define RMT_CARRIER_EFF_EN_3_M  (RMT_CARRIER_EFF_EN_3_V << RMT_CARRIER_EFF_EN_3_S)
#define RMT_CARRIER_EFF_EN_3_V  0x00000001U
#define RMT_CARRIER_EFF_EN_3_S  20
/** RMT_CARRIER_EN_3 : R/W; bitpos: [21]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL3. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_3    (BIT(21))
#define RMT_CARRIER_EN_3_M  (RMT_CARRIER_EN_3_V << RMT_CARRIER_EN_3_S)
#define RMT_CARRIER_EN_3_V  0x00000001U
#define RMT_CARRIER_EN_3_S  21
/** RMT_CARRIER_OUT_LV_3 : R/W; bitpos: [22]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL3.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_3    (BIT(22))
#define RMT_CARRIER_OUT_LV_3_M  (RMT_CARRIER_OUT_LV_3_V << RMT_CARRIER_OUT_LV_3_S)
#define RMT_CARRIER_OUT_LV_3_V  0x00000001U
#define RMT_CARRIER_OUT_LV_3_S  22
/** RMT_AFIFO_RST_3 : WT; bitpos: [23]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_3    (BIT(23))
#define RMT_AFIFO_RST_3_M  (RMT_AFIFO_RST_3_V << RMT_AFIFO_RST_3_S)
#define RMT_AFIFO_RST_3_V  0x00000001U
#define RMT_AFIFO_RST_3_S  23
/** RMT_CONF_UPDATE_3 : WT; bitpos: [24]; default: 0;
 *  synchronization bit for CHANNEL3
 */
#define RMT_CONF_UPDATE_3    (BIT(24))
#define RMT_CONF_UPDATE_3_M  (RMT_CONF_UPDATE_3_V << RMT_CONF_UPDATE_3_S)
#define RMT_CONF_UPDATE_3_V  0x00000001U
#define RMT_CONF_UPDATE_3_S  24

/** RMT_CH4CONF0_REG register
 *  Channel 4 configure register 0
 */
#define RMT_CH4CONF0_REG (DR_REG_RMT_BASE + 0x30)
/** RMT_DIV_CNT_4 : R/W; bitpos: [7:0]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL4.
 */
#define RMT_DIV_CNT_4    0x000000FFU
#define RMT_DIV_CNT_4_M  (RMT_DIV_CNT_4_V << RMT_DIV_CNT_4_S)
#define RMT_DIV_CNT_4_V  0x000000FFU
#define RMT_DIV_CNT_4_S  0
/** RMT_IDLE_THRES_4 : R/W; bitpos: [22:8]; default: 32767;
 *  When no edge is detected on the input signal and continuous clock cycles is longer
 *  than this register value, received process is finished.
 */
#define RMT_IDLE_THRES_4    0x00007FFFU
#define RMT_IDLE_THRES_4_M  (RMT_IDLE_THRES_4_V << RMT_IDLE_THRES_4_S)
#define RMT_IDLE_THRES_4_V  0x00007FFFU
#define RMT_IDLE_THRES_4_S  8
/** RMT_MEM_SIZE_4 : R/W; bitpos: [27:24]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL4.
 */
#define RMT_MEM_SIZE_4    0x0000000FU
#define RMT_MEM_SIZE_4_M  (RMT_MEM_SIZE_4_V << RMT_MEM_SIZE_4_S)
#define RMT_MEM_SIZE_4_V  0x0000000FU
#define RMT_MEM_SIZE_4_S  24
/** RMT_CARRIER_EN_4 : R/W; bitpos: [28]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL4. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_4    (BIT(28))
#define RMT_CARRIER_EN_4_M  (RMT_CARRIER_EN_4_V << RMT_CARRIER_EN_4_S)
#define RMT_CARRIER_EN_4_V  0x00000001U
#define RMT_CARRIER_EN_4_S  28
/** RMT_CARRIER_OUT_LV_4 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL4.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_4    (BIT(29))
#define RMT_CARRIER_OUT_LV_4_M  (RMT_CARRIER_OUT_LV_4_V << RMT_CARRIER_OUT_LV_4_S)
#define RMT_CARRIER_OUT_LV_4_V  0x00000001U
#define RMT_CARRIER_OUT_LV_4_S  29

/** RMT_CH4CONF1_REG register
 *  Channel 4 configure register 1
 */
#define RMT_CH4CONF1_REG (DR_REG_RMT_BASE + 0x34)
/** RMT_RX_EN_4 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable receiver to receive data on CHANNEL4.
 */
#define RMT_RX_EN_4    (BIT(0))
#define RMT_RX_EN_4_M  (RMT_RX_EN_4_V << RMT_RX_EN_4_S)
#define RMT_RX_EN_4_V  0x00000001U
#define RMT_RX_EN_4_S  0
/** RMT_MEM_WR_RST_4 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset write ram address for CHANNEL4 by accessing receiver.
 */
#define RMT_MEM_WR_RST_4    (BIT(1))
#define RMT_MEM_WR_RST_4_M  (RMT_MEM_WR_RST_4_V << RMT_MEM_WR_RST_4_S)
#define RMT_MEM_WR_RST_4_V  0x00000001U
#define RMT_MEM_WR_RST_4_S  1
/** RMT_APB_MEM_RST_4 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL4 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_4    (BIT(2))
#define RMT_APB_MEM_RST_4_M  (RMT_APB_MEM_RST_4_V << RMT_APB_MEM_RST_4_S)
#define RMT_APB_MEM_RST_4_V  0x00000001U
#define RMT_APB_MEM_RST_4_S  2
/** RMT_MEM_OWNER_4 : R/W/SC; bitpos: [3]; default: 1;
 *  This register marks the ownership of CHANNEL4's ram block.
 *
 *  1'h1: Receiver is using the ram.
 *
 *  1'h0: APB bus is using the ram.
 */
#define RMT_MEM_OWNER_4    (BIT(3))
#define RMT_MEM_OWNER_4_M  (RMT_MEM_OWNER_4_V << RMT_MEM_OWNER_4_S)
#define RMT_MEM_OWNER_4_V  0x00000001U
#define RMT_MEM_OWNER_4_S  3
/** RMT_RX_FILTER_EN_4 : R/W; bitpos: [4]; default: 0;
 *  This is the receive filter's enable bit for CHANNEL4.
 */
#define RMT_RX_FILTER_EN_4    (BIT(4))
#define RMT_RX_FILTER_EN_4_M  (RMT_RX_FILTER_EN_4_V << RMT_RX_FILTER_EN_4_S)
#define RMT_RX_FILTER_EN_4_V  0x00000001U
#define RMT_RX_FILTER_EN_4_S  4
/** RMT_RX_FILTER_THRES_4 : R/W; bitpos: [12:5]; default: 15;
 *  Ignores the input pulse when its width is smaller than this register value in APB
 *  clock periods (in receive mode).
 */
#define RMT_RX_FILTER_THRES_4    0x000000FFU
#define RMT_RX_FILTER_THRES_4_M  (RMT_RX_FILTER_THRES_4_V << RMT_RX_FILTER_THRES_4_S)
#define RMT_RX_FILTER_THRES_4_V  0x000000FFU
#define RMT_RX_FILTER_THRES_4_S  5
/** RMT_MEM_RX_WRAP_EN_4 : R/W; bitpos: [13]; default: 0;
 *  This is the channel 4 enable bit for wraparound mode: it will resume receiving at
 *  the start when the data to be received is more than its memory size.
 */
#define RMT_MEM_RX_WRAP_EN_4    (BIT(13))
#define RMT_MEM_RX_WRAP_EN_4_M  (RMT_MEM_RX_WRAP_EN_4_V << RMT_MEM_RX_WRAP_EN_4_S)
#define RMT_MEM_RX_WRAP_EN_4_V  0x00000001U
#define RMT_MEM_RX_WRAP_EN_4_S  13
/** RMT_AFIFO_RST_4 : WT; bitpos: [14]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_4    (BIT(14))
#define RMT_AFIFO_RST_4_M  (RMT_AFIFO_RST_4_V << RMT_AFIFO_RST_4_S)
#define RMT_AFIFO_RST_4_V  0x00000001U
#define RMT_AFIFO_RST_4_S  14
/** RMT_CONF_UPDATE_4 : WT; bitpos: [15]; default: 0;
 *  synchronization bit for CHANNEL4
 */
#define RMT_CONF_UPDATE_4    (BIT(15))
#define RMT_CONF_UPDATE_4_M  (RMT_CONF_UPDATE_4_V << RMT_CONF_UPDATE_4_S)
#define RMT_CONF_UPDATE_4_V  0x00000001U
#define RMT_CONF_UPDATE_4_S  15

/** RMT_CH5CONF0_REG register
 *  Channel 5 configure register 0
 */
#define RMT_CH5CONF0_REG (DR_REG_RMT_BASE + 0x38)
/** RMT_DIV_CNT_5 : R/W; bitpos: [7:0]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL5.
 */
#define RMT_DIV_CNT_5    0x000000FFU
#define RMT_DIV_CNT_5_M  (RMT_DIV_CNT_5_V << RMT_DIV_CNT_5_S)
#define RMT_DIV_CNT_5_V  0x000000FFU
#define RMT_DIV_CNT_5_S  0
/** RMT_IDLE_THRES_5 : R/W; bitpos: [22:8]; default: 32767;
 *  When no edge is detected on the input signal and continuous clock cycles is longer
 *  than this register value, received process is finished.
 */
#define RMT_IDLE_THRES_5    0x00007FFFU
#define RMT_IDLE_THRES_5_M  (RMT_IDLE_THRES_5_V << RMT_IDLE_THRES_5_S)
#define RMT_IDLE_THRES_5_V  0x00007FFFU
#define RMT_IDLE_THRES_5_S  8
/** RMT_MEM_SIZE_5 : R/W; bitpos: [27:24]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL5.
 */
#define RMT_MEM_SIZE_5    0x0000000FU
#define RMT_MEM_SIZE_5_M  (RMT_MEM_SIZE_5_V << RMT_MEM_SIZE_5_S)
#define RMT_MEM_SIZE_5_V  0x0000000FU
#define RMT_MEM_SIZE_5_S  24
/** RMT_CARRIER_EN_5 : R/W; bitpos: [28]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL5. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_5    (BIT(28))
#define RMT_CARRIER_EN_5_M  (RMT_CARRIER_EN_5_V << RMT_CARRIER_EN_5_S)
#define RMT_CARRIER_EN_5_V  0x00000001U
#define RMT_CARRIER_EN_5_S  28
/** RMT_CARRIER_OUT_LV_5 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL5.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_5    (BIT(29))
#define RMT_CARRIER_OUT_LV_5_M  (RMT_CARRIER_OUT_LV_5_V << RMT_CARRIER_OUT_LV_5_S)
#define RMT_CARRIER_OUT_LV_5_V  0x00000001U
#define RMT_CARRIER_OUT_LV_5_S  29

/** RMT_CH5CONF1_REG register
 *  Channel 5 configure register 1
 */
#define RMT_CH5CONF1_REG (DR_REG_RMT_BASE + 0x3c)
/** RMT_RX_EN_5 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable receiver to receive data on CHANNEL5.
 */
#define RMT_RX_EN_5    (BIT(0))
#define RMT_RX_EN_5_M  (RMT_RX_EN_5_V << RMT_RX_EN_5_S)
#define RMT_RX_EN_5_V  0x00000001U
#define RMT_RX_EN_5_S  0
/** RMT_MEM_WR_RST_5 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset write ram address for CHANNEL5 by accessing receiver.
 */
#define RMT_MEM_WR_RST_5    (BIT(1))
#define RMT_MEM_WR_RST_5_M  (RMT_MEM_WR_RST_5_V << RMT_MEM_WR_RST_5_S)
#define RMT_MEM_WR_RST_5_V  0x00000001U
#define RMT_MEM_WR_RST_5_S  1
/** RMT_APB_MEM_RST_5 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL5 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_5    (BIT(2))
#define RMT_APB_MEM_RST_5_M  (RMT_APB_MEM_RST_5_V << RMT_APB_MEM_RST_5_S)
#define RMT_APB_MEM_RST_5_V  0x00000001U
#define RMT_APB_MEM_RST_5_S  2
/** RMT_MEM_OWNER_5 : R/W/SC; bitpos: [3]; default: 1;
 *  This register marks the ownership of CHANNEL5's ram block.
 *
 *  1'h1: Receiver is using the ram.
 *
 *  1'h0: APB bus is using the ram.
 */
#define RMT_MEM_OWNER_5    (BIT(3))
#define RMT_MEM_OWNER_5_M  (RMT_MEM_OWNER_5_V << RMT_MEM_OWNER_5_S)
#define RMT_MEM_OWNER_5_V  0x00000001U
#define RMT_MEM_OWNER_5_S  3
/** RMT_RX_FILTER_EN_5 : R/W; bitpos: [4]; default: 0;
 *  This is the receive filter's enable bit for CHANNEL5.
 */
#define RMT_RX_FILTER_EN_5    (BIT(4))
#define RMT_RX_FILTER_EN_5_M  (RMT_RX_FILTER_EN_5_V << RMT_RX_FILTER_EN_5_S)
#define RMT_RX_FILTER_EN_5_V  0x00000001U
#define RMT_RX_FILTER_EN_5_S  4
/** RMT_RX_FILTER_THRES_5 : R/W; bitpos: [12:5]; default: 15;
 *  Ignores the input pulse when its width is smaller than this register value in APB
 *  clock periods (in receive mode).
 */
#define RMT_RX_FILTER_THRES_5    0x000000FFU
#define RMT_RX_FILTER_THRES_5_M  (RMT_RX_FILTER_THRES_5_V << RMT_RX_FILTER_THRES_5_S)
#define RMT_RX_FILTER_THRES_5_V  0x000000FFU
#define RMT_RX_FILTER_THRES_5_S  5
/** RMT_MEM_RX_WRAP_EN_5 : R/W; bitpos: [13]; default: 0;
 *  This is the channel 5 enable bit for wraparound mode: it will resume receiving at
 *  the start when the data to be received is more than its memory size.
 */
#define RMT_MEM_RX_WRAP_EN_5    (BIT(13))
#define RMT_MEM_RX_WRAP_EN_5_M  (RMT_MEM_RX_WRAP_EN_5_V << RMT_MEM_RX_WRAP_EN_5_S)
#define RMT_MEM_RX_WRAP_EN_5_V  0x00000001U
#define RMT_MEM_RX_WRAP_EN_5_S  13
/** RMT_AFIFO_RST_5 : WT; bitpos: [14]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_5    (BIT(14))
#define RMT_AFIFO_RST_5_M  (RMT_AFIFO_RST_5_V << RMT_AFIFO_RST_5_S)
#define RMT_AFIFO_RST_5_V  0x00000001U
#define RMT_AFIFO_RST_5_S  14
/** RMT_CONF_UPDATE_5 : WT; bitpos: [15]; default: 0;
 *  synchronization bit for CHANNEL5
 */
#define RMT_CONF_UPDATE_5    (BIT(15))
#define RMT_CONF_UPDATE_5_M  (RMT_CONF_UPDATE_5_V << RMT_CONF_UPDATE_5_S)
#define RMT_CONF_UPDATE_5_V  0x00000001U
#define RMT_CONF_UPDATE_5_S  15

/** RMT_CH6CONF0_REG register
 *  Channel 6 configure register 0
 */
#define RMT_CH6CONF0_REG (DR_REG_RMT_BASE + 0x40)
/** RMT_DIV_CNT_6 : R/W; bitpos: [7:0]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL6.
 */
#define RMT_DIV_CNT_6    0x000000FFU
#define RMT_DIV_CNT_6_M  (RMT_DIV_CNT_6_V << RMT_DIV_CNT_6_S)
#define RMT_DIV_CNT_6_V  0x000000FFU
#define RMT_DIV_CNT_6_S  0
/** RMT_IDLE_THRES_6 : R/W; bitpos: [22:8]; default: 32767;
 *  When no edge is detected on the input signal and continuous clock cycles is longer
 *  than this register value, received process is finished.
 */
#define RMT_IDLE_THRES_6    0x00007FFFU
#define RMT_IDLE_THRES_6_M  (RMT_IDLE_THRES_6_V << RMT_IDLE_THRES_6_S)
#define RMT_IDLE_THRES_6_V  0x00007FFFU
#define RMT_IDLE_THRES_6_S  8
/** RMT_MEM_SIZE_6 : R/W; bitpos: [27:24]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL6.
 */
#define RMT_MEM_SIZE_6    0x0000000FU
#define RMT_MEM_SIZE_6_M  (RMT_MEM_SIZE_6_V << RMT_MEM_SIZE_6_S)
#define RMT_MEM_SIZE_6_V  0x0000000FU
#define RMT_MEM_SIZE_6_S  24
/** RMT_CARRIER_EN_6 : R/W; bitpos: [28]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL6. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_6    (BIT(28))
#define RMT_CARRIER_EN_6_M  (RMT_CARRIER_EN_6_V << RMT_CARRIER_EN_6_S)
#define RMT_CARRIER_EN_6_V  0x00000001U
#define RMT_CARRIER_EN_6_S  28
/** RMT_CARRIER_OUT_LV_6 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL6.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_6    (BIT(29))
#define RMT_CARRIER_OUT_LV_6_M  (RMT_CARRIER_OUT_LV_6_V << RMT_CARRIER_OUT_LV_6_S)
#define RMT_CARRIER_OUT_LV_6_V  0x00000001U
#define RMT_CARRIER_OUT_LV_6_S  29

/** RMT_CH6CONF1_REG register
 *  Channel 6 configure register 1
 */
#define RMT_CH6CONF1_REG (DR_REG_RMT_BASE + 0x44)
/** RMT_RX_EN_6 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable receiver to receive data on CHANNEL6.
 */
#define RMT_RX_EN_6    (BIT(0))
#define RMT_RX_EN_6_M  (RMT_RX_EN_6_V << RMT_RX_EN_6_S)
#define RMT_RX_EN_6_V  0x00000001U
#define RMT_RX_EN_6_S  0
/** RMT_MEM_WR_RST_6 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset write ram address for CHANNEL6 by accessing receiver.
 */
#define RMT_MEM_WR_RST_6    (BIT(1))
#define RMT_MEM_WR_RST_6_M  (RMT_MEM_WR_RST_6_V << RMT_MEM_WR_RST_6_S)
#define RMT_MEM_WR_RST_6_V  0x00000001U
#define RMT_MEM_WR_RST_6_S  1
/** RMT_APB_MEM_RST_6 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL6 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_6    (BIT(2))
#define RMT_APB_MEM_RST_6_M  (RMT_APB_MEM_RST_6_V << RMT_APB_MEM_RST_6_S)
#define RMT_APB_MEM_RST_6_V  0x00000001U
#define RMT_APB_MEM_RST_6_S  2
/** RMT_MEM_OWNER_6 : R/W/SC; bitpos: [3]; default: 1;
 *  This register marks the ownership of CHANNEL6's ram block.
 *
 *  1'h1: Receiver is using the ram.
 *
 *  1'h0: APB bus is using the ram.
 */
#define RMT_MEM_OWNER_6    (BIT(3))
#define RMT_MEM_OWNER_6_M  (RMT_MEM_OWNER_6_V << RMT_MEM_OWNER_6_S)
#define RMT_MEM_OWNER_6_V  0x00000001U
#define RMT_MEM_OWNER_6_S  3
/** RMT_RX_FILTER_EN_6 : R/W; bitpos: [4]; default: 0;
 *  This is the receive filter's enable bit for CHANNEL6.
 */
#define RMT_RX_FILTER_EN_6    (BIT(4))
#define RMT_RX_FILTER_EN_6_M  (RMT_RX_FILTER_EN_6_V << RMT_RX_FILTER_EN_6_S)
#define RMT_RX_FILTER_EN_6_V  0x00000001U
#define RMT_RX_FILTER_EN_6_S  4
/** RMT_RX_FILTER_THRES_6 : R/W; bitpos: [12:5]; default: 15;
 *  Ignores the input pulse when its width is smaller than this register value in APB
 *  clock periods (in receive mode).
 */
#define RMT_RX_FILTER_THRES_6    0x000000FFU
#define RMT_RX_FILTER_THRES_6_M  (RMT_RX_FILTER_THRES_6_V << RMT_RX_FILTER_THRES_6_S)
#define RMT_RX_FILTER_THRES_6_V  0x000000FFU
#define RMT_RX_FILTER_THRES_6_S  5
/** RMT_MEM_RX_WRAP_EN_6 : R/W; bitpos: [13]; default: 0;
 *  This is the channel 6 enable bit for wraparound mode: it will resume receiving at
 *  the start when the data to be received is more than its memory size.
 */
#define RMT_MEM_RX_WRAP_EN_6    (BIT(13))
#define RMT_MEM_RX_WRAP_EN_6_M  (RMT_MEM_RX_WRAP_EN_6_V << RMT_MEM_RX_WRAP_EN_6_S)
#define RMT_MEM_RX_WRAP_EN_6_V  0x00000001U
#define RMT_MEM_RX_WRAP_EN_6_S  13
/** RMT_AFIFO_RST_6 : WT; bitpos: [14]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_6    (BIT(14))
#define RMT_AFIFO_RST_6_M  (RMT_AFIFO_RST_6_V << RMT_AFIFO_RST_6_S)
#define RMT_AFIFO_RST_6_V  0x00000001U
#define RMT_AFIFO_RST_6_S  14
/** RMT_CONF_UPDATE_6 : WT; bitpos: [15]; default: 0;
 *  synchronization bit for CHANNEL6
 */
#define RMT_CONF_UPDATE_6    (BIT(15))
#define RMT_CONF_UPDATE_6_M  (RMT_CONF_UPDATE_6_V << RMT_CONF_UPDATE_6_S)
#define RMT_CONF_UPDATE_6_V  0x00000001U
#define RMT_CONF_UPDATE_6_S  15

/** RMT_CH7CONF0_REG register
 *  Channel 7 configure register 0
 */
#define RMT_CH7CONF0_REG (DR_REG_RMT_BASE + 0x48)
/** RMT_DIV_CNT_7 : R/W; bitpos: [7:0]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL7.
 */
#define RMT_DIV_CNT_7    0x000000FFU
#define RMT_DIV_CNT_7_M  (RMT_DIV_CNT_7_V << RMT_DIV_CNT_7_S)
#define RMT_DIV_CNT_7_V  0x000000FFU
#define RMT_DIV_CNT_7_S  0
/** RMT_IDLE_THRES_7 : R/W; bitpos: [22:8]; default: 32767;
 *  When no edge is detected on the input signal and continuous clock cycles is longer
 *  than this register value, received process is finished.
 */
#define RMT_IDLE_THRES_7    0x00007FFFU
#define RMT_IDLE_THRES_7_M  (RMT_IDLE_THRES_7_V << RMT_IDLE_THRES_7_S)
#define RMT_IDLE_THRES_7_V  0x00007FFFU
#define RMT_IDLE_THRES_7_S  8
/** RMT_MEM_SIZE_7 : R/W; bitpos: [27:24]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL7.
 */
#define RMT_MEM_SIZE_7    0x0000000FU
#define RMT_MEM_SIZE_7_M  (RMT_MEM_SIZE_7_V << RMT_MEM_SIZE_7_S)
#define RMT_MEM_SIZE_7_V  0x0000000FU
#define RMT_MEM_SIZE_7_S  24
/** RMT_CARRIER_EN_7 : R/W; bitpos: [28]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL7. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_7    (BIT(28))
#define RMT_CARRIER_EN_7_M  (RMT_CARRIER_EN_7_V << RMT_CARRIER_EN_7_S)
#define RMT_CARRIER_EN_7_V  0x00000001U
#define RMT_CARRIER_EN_7_S  28
/** RMT_CARRIER_OUT_LV_7 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL7.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_7    (BIT(29))
#define RMT_CARRIER_OUT_LV_7_M  (RMT_CARRIER_OUT_LV_7_V << RMT_CARRIER_OUT_LV_7_S)
#define RMT_CARRIER_OUT_LV_7_V  0x00000001U
#define RMT_CARRIER_OUT_LV_7_S  29

/** RMT_CH7CONF1_REG register
 *  Channel 7 configure register 1
 */
#define RMT_CH7CONF1_REG (DR_REG_RMT_BASE + 0x4c)
/** RMT_RX_EN_7 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable receiver to receive data on CHANNEL7.
 */
#define RMT_RX_EN_7    (BIT(0))
#define RMT_RX_EN_7_M  (RMT_RX_EN_7_V << RMT_RX_EN_7_S)
#define RMT_RX_EN_7_V  0x00000001U
#define RMT_RX_EN_7_S  0
/** RMT_MEM_WR_RST_7 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset write ram address for CHANNEL7 by accessing receiver.
 */
#define RMT_MEM_WR_RST_7    (BIT(1))
#define RMT_MEM_WR_RST_7_M  (RMT_MEM_WR_RST_7_V << RMT_MEM_WR_RST_7_S)
#define RMT_MEM_WR_RST_7_V  0x00000001U
#define RMT_MEM_WR_RST_7_S  1
/** RMT_APB_MEM_RST_7 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL7 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_7    (BIT(2))
#define RMT_APB_MEM_RST_7_M  (RMT_APB_MEM_RST_7_V << RMT_APB_MEM_RST_7_S)
#define RMT_APB_MEM_RST_7_V  0x00000001U
#define RMT_APB_MEM_RST_7_S  2
/** RMT_MEM_OWNER_7 : R/W/SC; bitpos: [3]; default: 1;
 *  This register marks the ownership of CHANNEL7's ram block.
 *
 *  1'h1: Receiver is using the ram.
 *
 *  1'h0: APB bus is using the ram.
 */
#define RMT_MEM_OWNER_7    (BIT(3))
#define RMT_MEM_OWNER_7_M  (RMT_MEM_OWNER_7_V << RMT_MEM_OWNER_7_S)
#define RMT_MEM_OWNER_7_V  0x00000001U
#define RMT_MEM_OWNER_7_S  3
/** RMT_RX_FILTER_EN_7 : R/W; bitpos: [4]; default: 0;
 *  This is the receive filter's enable bit for CHANNEL7.
 */
#define RMT_RX_FILTER_EN_7    (BIT(4))
#define RMT_RX_FILTER_EN_7_M  (RMT_RX_FILTER_EN_7_V << RMT_RX_FILTER_EN_7_S)
#define RMT_RX_FILTER_EN_7_V  0x00000001U
#define RMT_RX_FILTER_EN_7_S  4
/** RMT_RX_FILTER_THRES_7 : R/W; bitpos: [12:5]; default: 15;
 *  Ignores the input pulse when its width is smaller than this register value in APB
 *  clock periods (in receive mode).
 */
#define RMT_RX_FILTER_THRES_7    0x000000FFU
#define RMT_RX_FILTER_THRES_7_M  (RMT_RX_FILTER_THRES_7_V << RMT_RX_FILTER_THRES_7_S)
#define RMT_RX_FILTER_THRES_7_V  0x000000FFU
#define RMT_RX_FILTER_THRES_7_S  5
/** RMT_MEM_RX_WRAP_EN_7 : R/W; bitpos: [13]; default: 0;
 *  This is the channel 7 enable bit for wraparound mode: it will resume receiving at
 *  the start when the data to be received is more than its memory size.
 */
#define RMT_MEM_RX_WRAP_EN_7    (BIT(13))
#define RMT_MEM_RX_WRAP_EN_7_M  (RMT_MEM_RX_WRAP_EN_7_V << RMT_MEM_RX_WRAP_EN_7_S)
#define RMT_MEM_RX_WRAP_EN_7_V  0x00000001U
#define RMT_MEM_RX_WRAP_EN_7_S  13
/** RMT_AFIFO_RST_7 : WT; bitpos: [14]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_7    (BIT(14))
#define RMT_AFIFO_RST_7_M  (RMT_AFIFO_RST_7_V << RMT_AFIFO_RST_7_S)
#define RMT_AFIFO_RST_7_V  0x00000001U
#define RMT_AFIFO_RST_7_S  14
/** RMT_CONF_UPDATE_7 : WT; bitpos: [15]; default: 0;
 *  synchronization bit for CHANNEL7
 */
#define RMT_CONF_UPDATE_7    (BIT(15))
#define RMT_CONF_UPDATE_7_M  (RMT_CONF_UPDATE_7_V << RMT_CONF_UPDATE_7_S)
#define RMT_CONF_UPDATE_7_V  0x00000001U
#define RMT_CONF_UPDATE_7_S  15

/** RMT_CH0STATUS_REG register
 *  Channel 0 status register
 */
#define RMT_CH0STATUS_REG (DR_REG_RMT_BASE + 0x50)
/** RMT_MEM_RADDR_EX_0 : RO; bitpos: [9:0]; default: 0;
 *  This register records the memory address offset when transmitter of CHANNEL0 is
 *  using the RAM.
 */
#define RMT_MEM_RADDR_EX_0    0x000003FFU
#define RMT_MEM_RADDR_EX_0_M  (RMT_MEM_RADDR_EX_0_V << RMT_MEM_RADDR_EX_0_S)
#define RMT_MEM_RADDR_EX_0_V  0x000003FFU
#define RMT_MEM_RADDR_EX_0_S  0
/** RMT_APB_MEM_WADDR_0 : RO; bitpos: [20:11]; default: 0;
 *  This register records the memory address offset when writes RAM over APB bus.
 */
#define RMT_APB_MEM_WADDR_0    0x000003FFU
#define RMT_APB_MEM_WADDR_0_M  (RMT_APB_MEM_WADDR_0_V << RMT_APB_MEM_WADDR_0_S)
#define RMT_APB_MEM_WADDR_0_V  0x000003FFU
#define RMT_APB_MEM_WADDR_0_S  11
/** RMT_STATE_0 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL0.
 */
#define RMT_STATE_0    0x00000007U
#define RMT_STATE_0_M  (RMT_STATE_0_V << RMT_STATE_0_S)
#define RMT_STATE_0_V  0x00000007U
#define RMT_STATE_0_S  22
/** RMT_MEM_EMPTY_0 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be set is more than memory size and
 *  the wraparound mode is disabled.
 */
#define RMT_MEM_EMPTY_0    (BIT(25))
#define RMT_MEM_EMPTY_0_M  (RMT_MEM_EMPTY_0_V << RMT_MEM_EMPTY_0_S)
#define RMT_MEM_EMPTY_0_V  0x00000001U
#define RMT_MEM_EMPTY_0_S  25
/** RMT_APB_MEM_WR_ERR_0 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_0    (BIT(26))
#define RMT_APB_MEM_WR_ERR_0_M  (RMT_APB_MEM_WR_ERR_0_V << RMT_APB_MEM_WR_ERR_0_S)
#define RMT_APB_MEM_WR_ERR_0_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_0_S  26

/** RMT_CH1STATUS_REG register
 *  Channel 1 status register
 */
#define RMT_CH1STATUS_REG (DR_REG_RMT_BASE + 0x54)
/** RMT_MEM_RADDR_EX_1 : RO; bitpos: [9:0]; default: 0;
 *  This register records the memory address offset when transmitter of CHANNEL1 is
 *  using the RAM.
 */
#define RMT_MEM_RADDR_EX_1    0x000003FFU
#define RMT_MEM_RADDR_EX_1_M  (RMT_MEM_RADDR_EX_1_V << RMT_MEM_RADDR_EX_1_S)
#define RMT_MEM_RADDR_EX_1_V  0x000003FFU
#define RMT_MEM_RADDR_EX_1_S  0
/** RMT_APB_MEM_WADDR_1 : RO; bitpos: [20:11]; default: 0;
 *  This register records the memory address offset when writes RAM over APB bus.
 */
#define RMT_APB_MEM_WADDR_1    0x000003FFU
#define RMT_APB_MEM_WADDR_1_M  (RMT_APB_MEM_WADDR_1_V << RMT_APB_MEM_WADDR_1_S)
#define RMT_APB_MEM_WADDR_1_V  0x000003FFU
#define RMT_APB_MEM_WADDR_1_S  11
/** RMT_STATE_1 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL1.
 */
#define RMT_STATE_1    0x00000007U
#define RMT_STATE_1_M  (RMT_STATE_1_V << RMT_STATE_1_S)
#define RMT_STATE_1_V  0x00000007U
#define RMT_STATE_1_S  22
/** RMT_MEM_EMPTY_1 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be set is more than memory size and
 *  the wraparound mode is disabled.
 */
#define RMT_MEM_EMPTY_1    (BIT(25))
#define RMT_MEM_EMPTY_1_M  (RMT_MEM_EMPTY_1_V << RMT_MEM_EMPTY_1_S)
#define RMT_MEM_EMPTY_1_V  0x00000001U
#define RMT_MEM_EMPTY_1_S  25
/** RMT_APB_MEM_WR_ERR_1 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_1    (BIT(26))
#define RMT_APB_MEM_WR_ERR_1_M  (RMT_APB_MEM_WR_ERR_1_V << RMT_APB_MEM_WR_ERR_1_S)
#define RMT_APB_MEM_WR_ERR_1_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_1_S  26

/** RMT_CH2STATUS_REG register
 *  Channel 2 status register
 */
#define RMT_CH2STATUS_REG (DR_REG_RMT_BASE + 0x58)
/** RMT_MEM_RADDR_EX_2 : RO; bitpos: [9:0]; default: 0;
 *  This register records the memory address offset when transmitter of CHANNEL2 is
 *  using the RAM.
 */
#define RMT_MEM_RADDR_EX_2    0x000003FFU
#define RMT_MEM_RADDR_EX_2_M  (RMT_MEM_RADDR_EX_2_V << RMT_MEM_RADDR_EX_2_S)
#define RMT_MEM_RADDR_EX_2_V  0x000003FFU
#define RMT_MEM_RADDR_EX_2_S  0
/** RMT_APB_MEM_WADDR_2 : RO; bitpos: [20:11]; default: 0;
 *  This register records the memory address offset when writes RAM over APB bus.
 */
#define RMT_APB_MEM_WADDR_2    0x000003FFU
#define RMT_APB_MEM_WADDR_2_M  (RMT_APB_MEM_WADDR_2_V << RMT_APB_MEM_WADDR_2_S)
#define RMT_APB_MEM_WADDR_2_V  0x000003FFU
#define RMT_APB_MEM_WADDR_2_S  11
/** RMT_STATE_2 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL2.
 */
#define RMT_STATE_2    0x00000007U
#define RMT_STATE_2_M  (RMT_STATE_2_V << RMT_STATE_2_S)
#define RMT_STATE_2_V  0x00000007U
#define RMT_STATE_2_S  22
/** RMT_MEM_EMPTY_2 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be set is more than memory size and
 *  the wraparound mode is disabled.
 */
#define RMT_MEM_EMPTY_2    (BIT(25))
#define RMT_MEM_EMPTY_2_M  (RMT_MEM_EMPTY_2_V << RMT_MEM_EMPTY_2_S)
#define RMT_MEM_EMPTY_2_V  0x00000001U
#define RMT_MEM_EMPTY_2_S  25
/** RMT_APB_MEM_WR_ERR_2 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_2    (BIT(26))
#define RMT_APB_MEM_WR_ERR_2_M  (RMT_APB_MEM_WR_ERR_2_V << RMT_APB_MEM_WR_ERR_2_S)
#define RMT_APB_MEM_WR_ERR_2_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_2_S  26

/** RMT_CH3STATUS_REG register
 *  Channel 3 status register
 */
#define RMT_CH3STATUS_REG (DR_REG_RMT_BASE + 0x5c)
/** RMT_MEM_RADDR_EX_3 : RO; bitpos: [9:0]; default: 0;
 *  This register records the memory address offset when transmitter of CHANNEL3 is
 *  using the RAM.
 */
#define RMT_MEM_RADDR_EX_3    0x000003FFU
#define RMT_MEM_RADDR_EX_3_M  (RMT_MEM_RADDR_EX_3_V << RMT_MEM_RADDR_EX_3_S)
#define RMT_MEM_RADDR_EX_3_V  0x000003FFU
#define RMT_MEM_RADDR_EX_3_S  0
/** RMT_APB_MEM_WADDR_3 : RO; bitpos: [20:11]; default: 0;
 *  This register records the memory address offset when writes RAM over APB bus.
 */
#define RMT_APB_MEM_WADDR_3    0x000003FFU
#define RMT_APB_MEM_WADDR_3_M  (RMT_APB_MEM_WADDR_3_V << RMT_APB_MEM_WADDR_3_S)
#define RMT_APB_MEM_WADDR_3_V  0x000003FFU
#define RMT_APB_MEM_WADDR_3_S  11
/** RMT_STATE_3 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL3.
 */
#define RMT_STATE_3    0x00000007U
#define RMT_STATE_3_M  (RMT_STATE_3_V << RMT_STATE_3_S)
#define RMT_STATE_3_V  0x00000007U
#define RMT_STATE_3_S  22
/** RMT_MEM_EMPTY_3 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be set is more than memory size and
 *  the wraparound mode is disabled.
 */
#define RMT_MEM_EMPTY_3    (BIT(25))
#define RMT_MEM_EMPTY_3_M  (RMT_MEM_EMPTY_3_V << RMT_MEM_EMPTY_3_S)
#define RMT_MEM_EMPTY_3_V  0x00000001U
#define RMT_MEM_EMPTY_3_S  25
/** RMT_APB_MEM_WR_ERR_3 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_3    (BIT(26))
#define RMT_APB_MEM_WR_ERR_3_M  (RMT_APB_MEM_WR_ERR_3_V << RMT_APB_MEM_WR_ERR_3_S)
#define RMT_APB_MEM_WR_ERR_3_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_3_S  26

/** RMT_CH4STATUS_REG register
 *  Channel 4 status register
 */
#define RMT_CH4STATUS_REG (DR_REG_RMT_BASE + 0x60)
/** RMT_MEM_WADDR_EX_4 : RO; bitpos: [9:0]; default: 192;
 *  This register records the memory address offset when receiver of CHANNEL4 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_4    0x000003FFU
#define RMT_MEM_WADDR_EX_4_M  (RMT_MEM_WADDR_EX_4_V << RMT_MEM_WADDR_EX_4_S)
#define RMT_MEM_WADDR_EX_4_V  0x000003FFU
#define RMT_MEM_WADDR_EX_4_S  0
/** RMT_APB_MEM_RADDR_4 : RO; bitpos: [20:11]; default: 192;
 *  This register records the memory address offset when reads RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_4    0x000003FFU
#define RMT_APB_MEM_RADDR_4_M  (RMT_APB_MEM_RADDR_4_V << RMT_APB_MEM_RADDR_4_S)
#define RMT_APB_MEM_RADDR_4_V  0x000003FFU
#define RMT_APB_MEM_RADDR_4_S  11
/** RMT_STATE_4 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL4.
 */
#define RMT_STATE_4    0x00000007U
#define RMT_STATE_4_M  (RMT_STATE_4_V << RMT_STATE_4_S)
#define RMT_STATE_4_V  0x00000007U
#define RMT_STATE_4_S  22
/** RMT_MEM_OWNER_ERR_4 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the ownership of memory block is wrong.
 */
#define RMT_MEM_OWNER_ERR_4    (BIT(25))
#define RMT_MEM_OWNER_ERR_4_M  (RMT_MEM_OWNER_ERR_4_V << RMT_MEM_OWNER_ERR_4_S)
#define RMT_MEM_OWNER_ERR_4_V  0x00000001U
#define RMT_MEM_OWNER_ERR_4_S  25
/** RMT_MEM_FULL_4 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory size.
 */
#define RMT_MEM_FULL_4    (BIT(26))
#define RMT_MEM_FULL_4_M  (RMT_MEM_FULL_4_V << RMT_MEM_FULL_4_S)
#define RMT_MEM_FULL_4_V  0x00000001U
#define RMT_MEM_FULL_4_S  26
/** RMT_APB_MEM_RD_ERR_4 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads via
 *  APB bus.
 */
#define RMT_APB_MEM_RD_ERR_4    (BIT(27))
#define RMT_APB_MEM_RD_ERR_4_M  (RMT_APB_MEM_RD_ERR_4_V << RMT_APB_MEM_RD_ERR_4_S)
#define RMT_APB_MEM_RD_ERR_4_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_4_S  27

/** RMT_CH5STATUS_REG register
 *  Channel 5 status register
 */
#define RMT_CH5STATUS_REG (DR_REG_RMT_BASE + 0x64)
/** RMT_MEM_WADDR_EX_5 : RO; bitpos: [9:0]; default: 192;
 *  This register records the memory address offset when receiver of CHANNEL5 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_5    0x000003FFU
#define RMT_MEM_WADDR_EX_5_M  (RMT_MEM_WADDR_EX_5_V << RMT_MEM_WADDR_EX_5_S)
#define RMT_MEM_WADDR_EX_5_V  0x000003FFU
#define RMT_MEM_WADDR_EX_5_S  0
/** RMT_APB_MEM_RADDR_5 : RO; bitpos: [20:11]; default: 192;
 *  This register records the memory address offset when reads RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_5    0x000003FFU
#define RMT_APB_MEM_RADDR_5_M  (RMT_APB_MEM_RADDR_5_V << RMT_APB_MEM_RADDR_5_S)
#define RMT_APB_MEM_RADDR_5_V  0x000003FFU
#define RMT_APB_MEM_RADDR_5_S  11
/** RMT_STATE_5 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL5.
 */
#define RMT_STATE_5    0x00000007U
#define RMT_STATE_5_M  (RMT_STATE_5_V << RMT_STATE_5_S)
#define RMT_STATE_5_V  0x00000007U
#define RMT_STATE_5_S  22
/** RMT_MEM_OWNER_ERR_5 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the ownership of memory block is wrong.
 */
#define RMT_MEM_OWNER_ERR_5    (BIT(25))
#define RMT_MEM_OWNER_ERR_5_M  (RMT_MEM_OWNER_ERR_5_V << RMT_MEM_OWNER_ERR_5_S)
#define RMT_MEM_OWNER_ERR_5_V  0x00000001U
#define RMT_MEM_OWNER_ERR_5_S  25
/** RMT_MEM_FULL_5 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory size.
 */
#define RMT_MEM_FULL_5    (BIT(26))
#define RMT_MEM_FULL_5_M  (RMT_MEM_FULL_5_V << RMT_MEM_FULL_5_S)
#define RMT_MEM_FULL_5_V  0x00000001U
#define RMT_MEM_FULL_5_S  26
/** RMT_APB_MEM_RD_ERR_5 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads via
 *  APB bus.
 */
#define RMT_APB_MEM_RD_ERR_5    (BIT(27))
#define RMT_APB_MEM_RD_ERR_5_M  (RMT_APB_MEM_RD_ERR_5_V << RMT_APB_MEM_RD_ERR_5_S)
#define RMT_APB_MEM_RD_ERR_5_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_5_S  27

/** RMT_CH6STATUS_REG register
 *  Channel 6 status register
 */
#define RMT_CH6STATUS_REG (DR_REG_RMT_BASE + 0x68)
/** RMT_MEM_WADDR_EX_6 : RO; bitpos: [9:0]; default: 192;
 *  This register records the memory address offset when receiver of CHANNEL6 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_6    0x000003FFU
#define RMT_MEM_WADDR_EX_6_M  (RMT_MEM_WADDR_EX_6_V << RMT_MEM_WADDR_EX_6_S)
#define RMT_MEM_WADDR_EX_6_V  0x000003FFU
#define RMT_MEM_WADDR_EX_6_S  0
/** RMT_APB_MEM_RADDR_6 : RO; bitpos: [20:11]; default: 192;
 *  This register records the memory address offset when reads RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_6    0x000003FFU
#define RMT_APB_MEM_RADDR_6_M  (RMT_APB_MEM_RADDR_6_V << RMT_APB_MEM_RADDR_6_S)
#define RMT_APB_MEM_RADDR_6_V  0x000003FFU
#define RMT_APB_MEM_RADDR_6_S  11
/** RMT_STATE_6 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL6.
 */
#define RMT_STATE_6    0x00000007U
#define RMT_STATE_6_M  (RMT_STATE_6_V << RMT_STATE_6_S)
#define RMT_STATE_6_V  0x00000007U
#define RMT_STATE_6_S  22
/** RMT_MEM_OWNER_ERR_6 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the ownership of memory block is wrong.
 */
#define RMT_MEM_OWNER_ERR_6    (BIT(25))
#define RMT_MEM_OWNER_ERR_6_M  (RMT_MEM_OWNER_ERR_6_V << RMT_MEM_OWNER_ERR_6_S)
#define RMT_MEM_OWNER_ERR_6_V  0x00000001U
#define RMT_MEM_OWNER_ERR_6_S  25
/** RMT_MEM_FULL_6 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory size.
 */
#define RMT_MEM_FULL_6    (BIT(26))
#define RMT_MEM_FULL_6_M  (RMT_MEM_FULL_6_V << RMT_MEM_FULL_6_S)
#define RMT_MEM_FULL_6_V  0x00000001U
#define RMT_MEM_FULL_6_S  26
/** RMT_APB_MEM_RD_ERR_6 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads via
 *  APB bus.
 */
#define RMT_APB_MEM_RD_ERR_6    (BIT(27))
#define RMT_APB_MEM_RD_ERR_6_M  (RMT_APB_MEM_RD_ERR_6_V << RMT_APB_MEM_RD_ERR_6_S)
#define RMT_APB_MEM_RD_ERR_6_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_6_S  27

/** RMT_CH7STATUS_REG register
 *  Channel 7 status register
 */
#define RMT_CH7STATUS_REG (DR_REG_RMT_BASE + 0x6c)
/** RMT_MEM_WADDR_EX_7 : RO; bitpos: [9:0]; default: 192;
 *  This register records the memory address offset when receiver of CHANNEL7 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_7    0x000003FFU
#define RMT_MEM_WADDR_EX_7_M  (RMT_MEM_WADDR_EX_7_V << RMT_MEM_WADDR_EX_7_S)
#define RMT_MEM_WADDR_EX_7_V  0x000003FFU
#define RMT_MEM_WADDR_EX_7_S  0
/** RMT_APB_MEM_RADDR_7 : RO; bitpos: [20:11]; default: 192;
 *  This register records the memory address offset when reads RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_7    0x000003FFU
#define RMT_APB_MEM_RADDR_7_M  (RMT_APB_MEM_RADDR_7_V << RMT_APB_MEM_RADDR_7_S)
#define RMT_APB_MEM_RADDR_7_V  0x000003FFU
#define RMT_APB_MEM_RADDR_7_S  11
/** RMT_STATE_7 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL7.
 */
#define RMT_STATE_7    0x00000007U
#define RMT_STATE_7_M  (RMT_STATE_7_V << RMT_STATE_7_S)
#define RMT_STATE_7_V  0x00000007U
#define RMT_STATE_7_S  22
/** RMT_MEM_OWNER_ERR_7 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the ownership of memory block is wrong.
 */
#define RMT_MEM_OWNER_ERR_7    (BIT(25))
#define RMT_MEM_OWNER_ERR_7_M  (RMT_MEM_OWNER_ERR_7_V << RMT_MEM_OWNER_ERR_7_S)
#define RMT_MEM_OWNER_ERR_7_V  0x00000001U
#define RMT_MEM_OWNER_ERR_7_S  25
/** RMT_MEM_FULL_7 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory size.
 */
#define RMT_MEM_FULL_7    (BIT(26))
#define RMT_MEM_FULL_7_M  (RMT_MEM_FULL_7_V << RMT_MEM_FULL_7_S)
#define RMT_MEM_FULL_7_V  0x00000001U
#define RMT_MEM_FULL_7_S  26
/** RMT_APB_MEM_RD_ERR_7 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads via
 *  APB bus.
 */
#define RMT_APB_MEM_RD_ERR_7    (BIT(27))
#define RMT_APB_MEM_RD_ERR_7_M  (RMT_APB_MEM_RD_ERR_7_V << RMT_APB_MEM_RD_ERR_7_S)
#define RMT_APB_MEM_RD_ERR_7_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_7_S  27

/** RMT_INT_RAW_REG register
 *  Raw interrupt status
 */
#define RMT_INT_RAW_REG (DR_REG_RMT_BASE + 0x70)
/** RMT_CH0_TX_END_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The interrupt raw bit for CHANNEL0. Triggered when transmission done.
 */
#define RMT_CH0_TX_END_INT_RAW    (BIT(0))
#define RMT_CH0_TX_END_INT_RAW_M  (RMT_CH0_TX_END_INT_RAW_V << RMT_CH0_TX_END_INT_RAW_S)
#define RMT_CH0_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH0_TX_END_INT_RAW_S  0
/** RMT_CH1_TX_END_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The interrupt raw bit for CHANNEL1. Triggered when transmission done.
 */
#define RMT_CH1_TX_END_INT_RAW    (BIT(1))
#define RMT_CH1_TX_END_INT_RAW_M  (RMT_CH1_TX_END_INT_RAW_V << RMT_CH1_TX_END_INT_RAW_S)
#define RMT_CH1_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH1_TX_END_INT_RAW_S  1
/** RMT_CH2_TX_END_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The interrupt raw bit for CHANNEL2. Triggered when transmission done.
 */
#define RMT_CH2_TX_END_INT_RAW    (BIT(2))
#define RMT_CH2_TX_END_INT_RAW_M  (RMT_CH2_TX_END_INT_RAW_V << RMT_CH2_TX_END_INT_RAW_S)
#define RMT_CH2_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH2_TX_END_INT_RAW_S  2
/** RMT_CH3_TX_END_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The interrupt raw bit for CHANNEL3. Triggered when transmission done.
 */
#define RMT_CH3_TX_END_INT_RAW    (BIT(3))
#define RMT_CH3_TX_END_INT_RAW_M  (RMT_CH3_TX_END_INT_RAW_V << RMT_CH3_TX_END_INT_RAW_S)
#define RMT_CH3_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH3_TX_END_INT_RAW_S  3
/** RMT_CH0_ERR_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The interrupt raw bit for CHANNEL0. Triggered when error occurs.
 */
#define RMT_CH0_ERR_INT_RAW    (BIT(4))
#define RMT_CH0_ERR_INT_RAW_M  (RMT_CH0_ERR_INT_RAW_V << RMT_CH0_ERR_INT_RAW_S)
#define RMT_CH0_ERR_INT_RAW_V  0x00000001U
#define RMT_CH0_ERR_INT_RAW_S  4
/** RMT_CH1_ERR_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The interrupt raw bit for CHANNEL1. Triggered when error occurs.
 */
#define RMT_CH1_ERR_INT_RAW    (BIT(5))
#define RMT_CH1_ERR_INT_RAW_M  (RMT_CH1_ERR_INT_RAW_V << RMT_CH1_ERR_INT_RAW_S)
#define RMT_CH1_ERR_INT_RAW_V  0x00000001U
#define RMT_CH1_ERR_INT_RAW_S  5
/** RMT_CH2_ERR_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The interrupt raw bit for CHANNEL2. Triggered when error occurs.
 */
#define RMT_CH2_ERR_INT_RAW    (BIT(6))
#define RMT_CH2_ERR_INT_RAW_M  (RMT_CH2_ERR_INT_RAW_V << RMT_CH2_ERR_INT_RAW_S)
#define RMT_CH2_ERR_INT_RAW_V  0x00000001U
#define RMT_CH2_ERR_INT_RAW_S  6
/** RMT_CH3_ERR_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The interrupt raw bit for CHANNEL3. Triggered when error occurs.
 */
#define RMT_CH3_ERR_INT_RAW    (BIT(7))
#define RMT_CH3_ERR_INT_RAW_M  (RMT_CH3_ERR_INT_RAW_V << RMT_CH3_ERR_INT_RAW_S)
#define RMT_CH3_ERR_INT_RAW_V  0x00000001U
#define RMT_CH3_ERR_INT_RAW_S  7
/** RMT_CH0_TX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The interrupt raw bit for CHANNEL0. Triggered when transmitter sent more data than
 *  configured value.
 */
#define RMT_CH0_TX_THR_EVENT_INT_RAW    (BIT(8))
#define RMT_CH0_TX_THR_EVENT_INT_RAW_M  (RMT_CH0_TX_THR_EVENT_INT_RAW_V << RMT_CH0_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH0_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_RAW_S  8
/** RMT_CH1_TX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The interrupt raw bit for CHANNEL1. Triggered when transmitter sent more data than
 *  configured value.
 */
#define RMT_CH1_TX_THR_EVENT_INT_RAW    (BIT(9))
#define RMT_CH1_TX_THR_EVENT_INT_RAW_M  (RMT_CH1_TX_THR_EVENT_INT_RAW_V << RMT_CH1_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH1_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_RAW_S  9
/** RMT_CH2_TX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The interrupt raw bit for CHANNEL2. Triggered when transmitter sent more data than
 *  configured value.
 */
#define RMT_CH2_TX_THR_EVENT_INT_RAW    (BIT(10))
#define RMT_CH2_TX_THR_EVENT_INT_RAW_M  (RMT_CH2_TX_THR_EVENT_INT_RAW_V << RMT_CH2_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH2_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_RAW_S  10
/** RMT_CH3_TX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The interrupt raw bit for CHANNEL3. Triggered when transmitter sent more data than
 *  configured value.
 */
#define RMT_CH3_TX_THR_EVENT_INT_RAW    (BIT(11))
#define RMT_CH3_TX_THR_EVENT_INT_RAW_M  (RMT_CH3_TX_THR_EVENT_INT_RAW_V << RMT_CH3_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH3_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_RAW_S  11
/** RMT_CH0_TX_LOOP_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The interrupt raw bit for CHANNEL0. Triggered when the loop count reaches the
 *  configured threshold value.
 */
#define RMT_CH0_TX_LOOP_INT_RAW    (BIT(12))
#define RMT_CH0_TX_LOOP_INT_RAW_M  (RMT_CH0_TX_LOOP_INT_RAW_V << RMT_CH0_TX_LOOP_INT_RAW_S)
#define RMT_CH0_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_RAW_S  12
/** RMT_CH1_TX_LOOP_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The interrupt raw bit for CHANNEL1. Triggered when the loop count reaches the
 *  configured threshold value.
 */
#define RMT_CH1_TX_LOOP_INT_RAW    (BIT(13))
#define RMT_CH1_TX_LOOP_INT_RAW_M  (RMT_CH1_TX_LOOP_INT_RAW_V << RMT_CH1_TX_LOOP_INT_RAW_S)
#define RMT_CH1_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_RAW_S  13
/** RMT_CH2_TX_LOOP_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The interrupt raw bit for CHANNEL2. Triggered when the loop count reaches the
 *  configured threshold value.
 */
#define RMT_CH2_TX_LOOP_INT_RAW    (BIT(14))
#define RMT_CH2_TX_LOOP_INT_RAW_M  (RMT_CH2_TX_LOOP_INT_RAW_V << RMT_CH2_TX_LOOP_INT_RAW_S)
#define RMT_CH2_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_RAW_S  14
/** RMT_CH3_TX_LOOP_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The interrupt raw bit for CHANNEL3. Triggered when the loop count reaches the
 *  configured threshold value.
 */
#define RMT_CH3_TX_LOOP_INT_RAW    (BIT(15))
#define RMT_CH3_TX_LOOP_INT_RAW_M  (RMT_CH3_TX_LOOP_INT_RAW_V << RMT_CH3_TX_LOOP_INT_RAW_S)
#define RMT_CH3_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_RAW_S  15
/** RMT_CH4_RX_END_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  The interrupt raw bit for CHANNEL4. Triggered when reception done.
 */
#define RMT_CH4_RX_END_INT_RAW    (BIT(16))
#define RMT_CH4_RX_END_INT_RAW_M  (RMT_CH4_RX_END_INT_RAW_V << RMT_CH4_RX_END_INT_RAW_S)
#define RMT_CH4_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH4_RX_END_INT_RAW_S  16
/** RMT_CH5_RX_END_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  The interrupt raw bit for CHANNEL5. Triggered when reception done.
 */
#define RMT_CH5_RX_END_INT_RAW    (BIT(17))
#define RMT_CH5_RX_END_INT_RAW_M  (RMT_CH5_RX_END_INT_RAW_V << RMT_CH5_RX_END_INT_RAW_S)
#define RMT_CH5_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH5_RX_END_INT_RAW_S  17
/** RMT_CH6_RX_END_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The interrupt raw bit for CHANNEL6. Triggered when reception done.
 */
#define RMT_CH6_RX_END_INT_RAW    (BIT(18))
#define RMT_CH6_RX_END_INT_RAW_M  (RMT_CH6_RX_END_INT_RAW_V << RMT_CH6_RX_END_INT_RAW_S)
#define RMT_CH6_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH6_RX_END_INT_RAW_S  18
/** RMT_CH7_RX_END_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The interrupt raw bit for CHANNEL7. Triggered when reception done.
 */
#define RMT_CH7_RX_END_INT_RAW    (BIT(19))
#define RMT_CH7_RX_END_INT_RAW_M  (RMT_CH7_RX_END_INT_RAW_V << RMT_CH7_RX_END_INT_RAW_S)
#define RMT_CH7_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH7_RX_END_INT_RAW_S  19
/** RMT_CH4_ERR_INT_RAW : R/WTC/SS; bitpos: [20]; default: 0;
 *  The interrupt raw bit for CHANNEL4. Triggered when error occurs.
 */
#define RMT_CH4_ERR_INT_RAW    (BIT(20))
#define RMT_CH4_ERR_INT_RAW_M  (RMT_CH4_ERR_INT_RAW_V << RMT_CH4_ERR_INT_RAW_S)
#define RMT_CH4_ERR_INT_RAW_V  0x00000001U
#define RMT_CH4_ERR_INT_RAW_S  20
/** RMT_CH5_ERR_INT_RAW : R/WTC/SS; bitpos: [21]; default: 0;
 *  The interrupt raw bit for CHANNEL5. Triggered when error occurs.
 */
#define RMT_CH5_ERR_INT_RAW    (BIT(21))
#define RMT_CH5_ERR_INT_RAW_M  (RMT_CH5_ERR_INT_RAW_V << RMT_CH5_ERR_INT_RAW_S)
#define RMT_CH5_ERR_INT_RAW_V  0x00000001U
#define RMT_CH5_ERR_INT_RAW_S  21
/** RMT_CH6_ERR_INT_RAW : R/WTC/SS; bitpos: [22]; default: 0;
 *  The interrupt raw bit for CHANNEL6. Triggered when error occurs.
 */
#define RMT_CH6_ERR_INT_RAW    (BIT(22))
#define RMT_CH6_ERR_INT_RAW_M  (RMT_CH6_ERR_INT_RAW_V << RMT_CH6_ERR_INT_RAW_S)
#define RMT_CH6_ERR_INT_RAW_V  0x00000001U
#define RMT_CH6_ERR_INT_RAW_S  22
/** RMT_CH7_ERR_INT_RAW : R/WTC/SS; bitpos: [23]; default: 0;
 *  The interrupt raw bit for CHANNEL7. Triggered when error occurs.
 */
#define RMT_CH7_ERR_INT_RAW    (BIT(23))
#define RMT_CH7_ERR_INT_RAW_M  (RMT_CH7_ERR_INT_RAW_V << RMT_CH7_ERR_INT_RAW_S)
#define RMT_CH7_ERR_INT_RAW_V  0x00000001U
#define RMT_CH7_ERR_INT_RAW_S  23
/** RMT_CH4_RX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [24]; default: 0;
 *  The interrupt raw bit for CHANNEL4. Triggered when receiver receive more data than
 *  configured value.
 */
#define RMT_CH4_RX_THR_EVENT_INT_RAW    (BIT(24))
#define RMT_CH4_RX_THR_EVENT_INT_RAW_M  (RMT_CH4_RX_THR_EVENT_INT_RAW_V << RMT_CH4_RX_THR_EVENT_INT_RAW_S)
#define RMT_CH4_RX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH4_RX_THR_EVENT_INT_RAW_S  24
/** RMT_CH5_RX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [25]; default: 0;
 *  The interrupt raw bit for CHANNEL5. Triggered when receiver receive more data than
 *  configured value.
 */
#define RMT_CH5_RX_THR_EVENT_INT_RAW    (BIT(25))
#define RMT_CH5_RX_THR_EVENT_INT_RAW_M  (RMT_CH5_RX_THR_EVENT_INT_RAW_V << RMT_CH5_RX_THR_EVENT_INT_RAW_S)
#define RMT_CH5_RX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH5_RX_THR_EVENT_INT_RAW_S  25
/** RMT_CH6_RX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [26]; default: 0;
 *  The interrupt raw bit for CHANNEL6. Triggered when receiver receive more data than
 *  configured value.
 */
#define RMT_CH6_RX_THR_EVENT_INT_RAW    (BIT(26))
#define RMT_CH6_RX_THR_EVENT_INT_RAW_M  (RMT_CH6_RX_THR_EVENT_INT_RAW_V << RMT_CH6_RX_THR_EVENT_INT_RAW_S)
#define RMT_CH6_RX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH6_RX_THR_EVENT_INT_RAW_S  26
/** RMT_CH7_RX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [27]; default: 0;
 *  The interrupt raw bit for CHANNEL7. Triggered when receiver receive more data than
 *  configured value.
 */
#define RMT_CH7_RX_THR_EVENT_INT_RAW    (BIT(27))
#define RMT_CH7_RX_THR_EVENT_INT_RAW_M  (RMT_CH7_RX_THR_EVENT_INT_RAW_V << RMT_CH7_RX_THR_EVENT_INT_RAW_S)
#define RMT_CH7_RX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH7_RX_THR_EVENT_INT_RAW_S  27
/** RMT_CH3_DMA_ACCESS_FAIL_INT_RAW : R/WTC/SS; bitpos: [28]; default: 0;
 *  The interrupt raw bit for CHANNEL3. Triggered when dma accessing CHANNEL3 fails.
 */
#define RMT_CH3_DMA_ACCESS_FAIL_INT_RAW    (BIT(28))
#define RMT_CH3_DMA_ACCESS_FAIL_INT_RAW_M  (RMT_CH3_DMA_ACCESS_FAIL_INT_RAW_V << RMT_CH3_DMA_ACCESS_FAIL_INT_RAW_S)
#define RMT_CH3_DMA_ACCESS_FAIL_INT_RAW_V  0x00000001U
#define RMT_CH3_DMA_ACCESS_FAIL_INT_RAW_S  28
/** RMT_CH7_DMA_ACCESS_FAIL_INT_RAW : R/WTC/SS; bitpos: [29]; default: 0;
 *  The interrupt raw bit for CHANNEL7. Triggered when dma accessing CHANNEL7 fails.
 */
#define RMT_CH7_DMA_ACCESS_FAIL_INT_RAW    (BIT(29))
#define RMT_CH7_DMA_ACCESS_FAIL_INT_RAW_M  (RMT_CH7_DMA_ACCESS_FAIL_INT_RAW_V << RMT_CH7_DMA_ACCESS_FAIL_INT_RAW_S)
#define RMT_CH7_DMA_ACCESS_FAIL_INT_RAW_V  0x00000001U
#define RMT_CH7_DMA_ACCESS_FAIL_INT_RAW_S  29

/** RMT_INT_ST_REG register
 *  Masked interrupt status
 */
#define RMT_INT_ST_REG (DR_REG_RMT_BASE + 0x74)
/** RMT_CH0_TX_END_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit for CH0_TX_END_INT.
 */
#define RMT_CH0_TX_END_INT_ST    (BIT(0))
#define RMT_CH0_TX_END_INT_ST_M  (RMT_CH0_TX_END_INT_ST_V << RMT_CH0_TX_END_INT_ST_S)
#define RMT_CH0_TX_END_INT_ST_V  0x00000001U
#define RMT_CH0_TX_END_INT_ST_S  0
/** RMT_CH1_TX_END_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit for CH1_TX_END_INT.
 */
#define RMT_CH1_TX_END_INT_ST    (BIT(1))
#define RMT_CH1_TX_END_INT_ST_M  (RMT_CH1_TX_END_INT_ST_V << RMT_CH1_TX_END_INT_ST_S)
#define RMT_CH1_TX_END_INT_ST_V  0x00000001U
#define RMT_CH1_TX_END_INT_ST_S  1
/** RMT_CH2_TX_END_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit for CH2_TX_END_INT.
 */
#define RMT_CH2_TX_END_INT_ST    (BIT(2))
#define RMT_CH2_TX_END_INT_ST_M  (RMT_CH2_TX_END_INT_ST_V << RMT_CH2_TX_END_INT_ST_S)
#define RMT_CH2_TX_END_INT_ST_V  0x00000001U
#define RMT_CH2_TX_END_INT_ST_S  2
/** RMT_CH3_TX_END_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit for CH3_TX_END_INT.
 */
#define RMT_CH3_TX_END_INT_ST    (BIT(3))
#define RMT_CH3_TX_END_INT_ST_M  (RMT_CH3_TX_END_INT_ST_V << RMT_CH3_TX_END_INT_ST_S)
#define RMT_CH3_TX_END_INT_ST_V  0x00000001U
#define RMT_CH3_TX_END_INT_ST_S  3
/** RMT_CH0_ERR_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit for CH0_ERR_INT.
 */
#define RMT_CH0_ERR_INT_ST    (BIT(4))
#define RMT_CH0_ERR_INT_ST_M  (RMT_CH0_ERR_INT_ST_V << RMT_CH0_ERR_INT_ST_S)
#define RMT_CH0_ERR_INT_ST_V  0x00000001U
#define RMT_CH0_ERR_INT_ST_S  4
/** RMT_CH1_ERR_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit for CH1_ERR_INT.
 */
#define RMT_CH1_ERR_INT_ST    (BIT(5))
#define RMT_CH1_ERR_INT_ST_M  (RMT_CH1_ERR_INT_ST_V << RMT_CH1_ERR_INT_ST_S)
#define RMT_CH1_ERR_INT_ST_V  0x00000001U
#define RMT_CH1_ERR_INT_ST_S  5
/** RMT_CH2_ERR_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status bit for CH2_ERR_INT.
 */
#define RMT_CH2_ERR_INT_ST    (BIT(6))
#define RMT_CH2_ERR_INT_ST_M  (RMT_CH2_ERR_INT_ST_V << RMT_CH2_ERR_INT_ST_S)
#define RMT_CH2_ERR_INT_ST_V  0x00000001U
#define RMT_CH2_ERR_INT_ST_S  6
/** RMT_CH3_ERR_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status bit for CH3_ERR_INT.
 */
#define RMT_CH3_ERR_INT_ST    (BIT(7))
#define RMT_CH3_ERR_INT_ST_M  (RMT_CH3_ERR_INT_ST_V << RMT_CH3_ERR_INT_ST_S)
#define RMT_CH3_ERR_INT_ST_V  0x00000001U
#define RMT_CH3_ERR_INT_ST_S  7
/** RMT_CH0_TX_THR_EVENT_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status bit for CH0_TX_THR_EVENT_INT.
 */
#define RMT_CH0_TX_THR_EVENT_INT_ST    (BIT(8))
#define RMT_CH0_TX_THR_EVENT_INT_ST_M  (RMT_CH0_TX_THR_EVENT_INT_ST_V << RMT_CH0_TX_THR_EVENT_INT_ST_S)
#define RMT_CH0_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_ST_S  8
/** RMT_CH1_TX_THR_EVENT_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status bit for CH1_TX_THR_EVENT_INT.
 */
#define RMT_CH1_TX_THR_EVENT_INT_ST    (BIT(9))
#define RMT_CH1_TX_THR_EVENT_INT_ST_M  (RMT_CH1_TX_THR_EVENT_INT_ST_V << RMT_CH1_TX_THR_EVENT_INT_ST_S)
#define RMT_CH1_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_ST_S  9
/** RMT_CH2_TX_THR_EVENT_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status bit for CH2_TX_THR_EVENT_INT.
 */
#define RMT_CH2_TX_THR_EVENT_INT_ST    (BIT(10))
#define RMT_CH2_TX_THR_EVENT_INT_ST_M  (RMT_CH2_TX_THR_EVENT_INT_ST_V << RMT_CH2_TX_THR_EVENT_INT_ST_S)
#define RMT_CH2_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_ST_S  10
/** RMT_CH3_TX_THR_EVENT_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status bit for CH3_TX_THR_EVENT_INT.
 */
#define RMT_CH3_TX_THR_EVENT_INT_ST    (BIT(11))
#define RMT_CH3_TX_THR_EVENT_INT_ST_M  (RMT_CH3_TX_THR_EVENT_INT_ST_V << RMT_CH3_TX_THR_EVENT_INT_ST_S)
#define RMT_CH3_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_ST_S  11
/** RMT_CH0_TX_LOOP_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status bit for CH0_TX_LOOP_INT.
 */
#define RMT_CH0_TX_LOOP_INT_ST    (BIT(12))
#define RMT_CH0_TX_LOOP_INT_ST_M  (RMT_CH0_TX_LOOP_INT_ST_V << RMT_CH0_TX_LOOP_INT_ST_S)
#define RMT_CH0_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_ST_S  12
/** RMT_CH1_TX_LOOP_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status bit for CH1_TX_LOOP_INT.
 */
#define RMT_CH1_TX_LOOP_INT_ST    (BIT(13))
#define RMT_CH1_TX_LOOP_INT_ST_M  (RMT_CH1_TX_LOOP_INT_ST_V << RMT_CH1_TX_LOOP_INT_ST_S)
#define RMT_CH1_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_ST_S  13
/** RMT_CH2_TX_LOOP_INT_ST : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status bit for CH2_TX_LOOP_INT.
 */
#define RMT_CH2_TX_LOOP_INT_ST    (BIT(14))
#define RMT_CH2_TX_LOOP_INT_ST_M  (RMT_CH2_TX_LOOP_INT_ST_V << RMT_CH2_TX_LOOP_INT_ST_S)
#define RMT_CH2_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_ST_S  14
/** RMT_CH3_TX_LOOP_INT_ST : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status bit for CH3_TX_LOOP_INT.
 */
#define RMT_CH3_TX_LOOP_INT_ST    (BIT(15))
#define RMT_CH3_TX_LOOP_INT_ST_M  (RMT_CH3_TX_LOOP_INT_ST_V << RMT_CH3_TX_LOOP_INT_ST_S)
#define RMT_CH3_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_ST_S  15
/** RMT_CH4_RX_END_INT_ST : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status bit for CH4_RX_END_INT.
 */
#define RMT_CH4_RX_END_INT_ST    (BIT(16))
#define RMT_CH4_RX_END_INT_ST_M  (RMT_CH4_RX_END_INT_ST_V << RMT_CH4_RX_END_INT_ST_S)
#define RMT_CH4_RX_END_INT_ST_V  0x00000001U
#define RMT_CH4_RX_END_INT_ST_S  16
/** RMT_CH5_RX_END_INT_ST : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status bit for CH5_RX_END_INT.
 */
#define RMT_CH5_RX_END_INT_ST    (BIT(17))
#define RMT_CH5_RX_END_INT_ST_M  (RMT_CH5_RX_END_INT_ST_V << RMT_CH5_RX_END_INT_ST_S)
#define RMT_CH5_RX_END_INT_ST_V  0x00000001U
#define RMT_CH5_RX_END_INT_ST_S  17
/** RMT_CH6_RX_END_INT_ST : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status bit for CH6_RX_END_INT.
 */
#define RMT_CH6_RX_END_INT_ST    (BIT(18))
#define RMT_CH6_RX_END_INT_ST_M  (RMT_CH6_RX_END_INT_ST_V << RMT_CH6_RX_END_INT_ST_S)
#define RMT_CH6_RX_END_INT_ST_V  0x00000001U
#define RMT_CH6_RX_END_INT_ST_S  18
/** RMT_CH7_RX_END_INT_ST : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status bit for CH7_RX_END_INT.
 */
#define RMT_CH7_RX_END_INT_ST    (BIT(19))
#define RMT_CH7_RX_END_INT_ST_M  (RMT_CH7_RX_END_INT_ST_V << RMT_CH7_RX_END_INT_ST_S)
#define RMT_CH7_RX_END_INT_ST_V  0x00000001U
#define RMT_CH7_RX_END_INT_ST_S  19
/** RMT_CH4_ERR_INT_ST : RO; bitpos: [20]; default: 0;
 *  The masked interrupt status bit for CH4_ERR_INT.
 */
#define RMT_CH4_ERR_INT_ST    (BIT(20))
#define RMT_CH4_ERR_INT_ST_M  (RMT_CH4_ERR_INT_ST_V << RMT_CH4_ERR_INT_ST_S)
#define RMT_CH4_ERR_INT_ST_V  0x00000001U
#define RMT_CH4_ERR_INT_ST_S  20
/** RMT_CH5_ERR_INT_ST : RO; bitpos: [21]; default: 0;
 *  The masked interrupt status bit for CH5_ERR_INT.
 */
#define RMT_CH5_ERR_INT_ST    (BIT(21))
#define RMT_CH5_ERR_INT_ST_M  (RMT_CH5_ERR_INT_ST_V << RMT_CH5_ERR_INT_ST_S)
#define RMT_CH5_ERR_INT_ST_V  0x00000001U
#define RMT_CH5_ERR_INT_ST_S  21
/** RMT_CH6_ERR_INT_ST : RO; bitpos: [22]; default: 0;
 *  The masked interrupt status bit for CH6_ERR_INT.
 */
#define RMT_CH6_ERR_INT_ST    (BIT(22))
#define RMT_CH6_ERR_INT_ST_M  (RMT_CH6_ERR_INT_ST_V << RMT_CH6_ERR_INT_ST_S)
#define RMT_CH6_ERR_INT_ST_V  0x00000001U
#define RMT_CH6_ERR_INT_ST_S  22
/** RMT_CH7_ERR_INT_ST : RO; bitpos: [23]; default: 0;
 *  The masked interrupt status bit for CH7_ERR_INT.
 */
#define RMT_CH7_ERR_INT_ST    (BIT(23))
#define RMT_CH7_ERR_INT_ST_M  (RMT_CH7_ERR_INT_ST_V << RMT_CH7_ERR_INT_ST_S)
#define RMT_CH7_ERR_INT_ST_V  0x00000001U
#define RMT_CH7_ERR_INT_ST_S  23
/** RMT_CH4_RX_THR_EVENT_INT_ST : RO; bitpos: [24]; default: 0;
 *  The masked interrupt status bit for CH4_RX_THR_EVENT_INT.
 */
#define RMT_CH4_RX_THR_EVENT_INT_ST    (BIT(24))
#define RMT_CH4_RX_THR_EVENT_INT_ST_M  (RMT_CH4_RX_THR_EVENT_INT_ST_V << RMT_CH4_RX_THR_EVENT_INT_ST_S)
#define RMT_CH4_RX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH4_RX_THR_EVENT_INT_ST_S  24
/** RMT_CH5_RX_THR_EVENT_INT_ST : RO; bitpos: [25]; default: 0;
 *  The masked interrupt status bit for CH5_RX_THR_EVENT_INT.
 */
#define RMT_CH5_RX_THR_EVENT_INT_ST    (BIT(25))
#define RMT_CH5_RX_THR_EVENT_INT_ST_M  (RMT_CH5_RX_THR_EVENT_INT_ST_V << RMT_CH5_RX_THR_EVENT_INT_ST_S)
#define RMT_CH5_RX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH5_RX_THR_EVENT_INT_ST_S  25
/** RMT_CH6_RX_THR_EVENT_INT_ST : RO; bitpos: [26]; default: 0;
 *  The masked interrupt status bit for CH6_RX_THR_EVENT_INT.
 */
#define RMT_CH6_RX_THR_EVENT_INT_ST    (BIT(26))
#define RMT_CH6_RX_THR_EVENT_INT_ST_M  (RMT_CH6_RX_THR_EVENT_INT_ST_V << RMT_CH6_RX_THR_EVENT_INT_ST_S)
#define RMT_CH6_RX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH6_RX_THR_EVENT_INT_ST_S  26
/** RMT_CH7_RX_THR_EVENT_INT_ST : RO; bitpos: [27]; default: 0;
 *  The masked interrupt status bit for CH7_RX_THR_EVENT_INT.
 */
#define RMT_CH7_RX_THR_EVENT_INT_ST    (BIT(27))
#define RMT_CH7_RX_THR_EVENT_INT_ST_M  (RMT_CH7_RX_THR_EVENT_INT_ST_V << RMT_CH7_RX_THR_EVENT_INT_ST_S)
#define RMT_CH7_RX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH7_RX_THR_EVENT_INT_ST_S  27
/** RMT_CH3_DMA_ACCESS_FAIL_INT_ST : RO; bitpos: [28]; default: 0;
 *  The masked interrupt status bit for  CH3_DMA_ACCESS_FAIL_INT.
 */
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ST    (BIT(28))
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ST_M  (RMT_CH3_DMA_ACCESS_FAIL_INT_ST_V << RMT_CH3_DMA_ACCESS_FAIL_INT_ST_S)
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ST_V  0x00000001U
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ST_S  28
/** RMT_CH7_DMA_ACCESS_FAIL_INT_ST : RO; bitpos: [29]; default: 0;
 *  The masked interrupt status bit for  CH7_DMA_ACCESS_FAIL_INT.
 */
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ST    (BIT(29))
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ST_M  (RMT_CH7_DMA_ACCESS_FAIL_INT_ST_V << RMT_CH7_DMA_ACCESS_FAIL_INT_ST_S)
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ST_V  0x00000001U
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ST_S  29

/** RMT_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define RMT_INT_ENA_REG (DR_REG_RMT_BASE + 0x78)
/** RMT_CH0_TX_END_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for CH0_TX_END_INT.
 */
#define RMT_CH0_TX_END_INT_ENA    (BIT(0))
#define RMT_CH0_TX_END_INT_ENA_M  (RMT_CH0_TX_END_INT_ENA_V << RMT_CH0_TX_END_INT_ENA_S)
#define RMT_CH0_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH0_TX_END_INT_ENA_S  0
/** RMT_CH1_TX_END_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for CH1_TX_END_INT.
 */
#define RMT_CH1_TX_END_INT_ENA    (BIT(1))
#define RMT_CH1_TX_END_INT_ENA_M  (RMT_CH1_TX_END_INT_ENA_V << RMT_CH1_TX_END_INT_ENA_S)
#define RMT_CH1_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH1_TX_END_INT_ENA_S  1
/** RMT_CH2_TX_END_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for CH2_TX_END_INT.
 */
#define RMT_CH2_TX_END_INT_ENA    (BIT(2))
#define RMT_CH2_TX_END_INT_ENA_M  (RMT_CH2_TX_END_INT_ENA_V << RMT_CH2_TX_END_INT_ENA_S)
#define RMT_CH2_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH2_TX_END_INT_ENA_S  2
/** RMT_CH3_TX_END_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for CH3_TX_END_INT.
 */
#define RMT_CH3_TX_END_INT_ENA    (BIT(3))
#define RMT_CH3_TX_END_INT_ENA_M  (RMT_CH3_TX_END_INT_ENA_V << RMT_CH3_TX_END_INT_ENA_S)
#define RMT_CH3_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH3_TX_END_INT_ENA_S  3
/** RMT_CH0_ERR_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for CH0_ERR_INT.
 */
#define RMT_CH0_ERR_INT_ENA    (BIT(4))
#define RMT_CH0_ERR_INT_ENA_M  (RMT_CH0_ERR_INT_ENA_V << RMT_CH0_ERR_INT_ENA_S)
#define RMT_CH0_ERR_INT_ENA_V  0x00000001U
#define RMT_CH0_ERR_INT_ENA_S  4
/** RMT_CH1_ERR_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for CH1_ERR_INT.
 */
#define RMT_CH1_ERR_INT_ENA    (BIT(5))
#define RMT_CH1_ERR_INT_ENA_M  (RMT_CH1_ERR_INT_ENA_V << RMT_CH1_ERR_INT_ENA_S)
#define RMT_CH1_ERR_INT_ENA_V  0x00000001U
#define RMT_CH1_ERR_INT_ENA_S  5
/** RMT_CH2_ERR_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for CH2_ERR_INT.
 */
#define RMT_CH2_ERR_INT_ENA    (BIT(6))
#define RMT_CH2_ERR_INT_ENA_M  (RMT_CH2_ERR_INT_ENA_V << RMT_CH2_ERR_INT_ENA_S)
#define RMT_CH2_ERR_INT_ENA_V  0x00000001U
#define RMT_CH2_ERR_INT_ENA_S  6
/** RMT_CH3_ERR_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for CH3_ERR_INT.
 */
#define RMT_CH3_ERR_INT_ENA    (BIT(7))
#define RMT_CH3_ERR_INT_ENA_M  (RMT_CH3_ERR_INT_ENA_V << RMT_CH3_ERR_INT_ENA_S)
#define RMT_CH3_ERR_INT_ENA_V  0x00000001U
#define RMT_CH3_ERR_INT_ENA_S  7
/** RMT_CH0_TX_THR_EVENT_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for CH0_TX_THR_EVENT_INT.
 */
#define RMT_CH0_TX_THR_EVENT_INT_ENA    (BIT(8))
#define RMT_CH0_TX_THR_EVENT_INT_ENA_M  (RMT_CH0_TX_THR_EVENT_INT_ENA_V << RMT_CH0_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH0_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_ENA_S  8
/** RMT_CH1_TX_THR_EVENT_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for CH1_TX_THR_EVENT_INT.
 */
#define RMT_CH1_TX_THR_EVENT_INT_ENA    (BIT(9))
#define RMT_CH1_TX_THR_EVENT_INT_ENA_M  (RMT_CH1_TX_THR_EVENT_INT_ENA_V << RMT_CH1_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH1_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_ENA_S  9
/** RMT_CH2_TX_THR_EVENT_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for CH2_TX_THR_EVENT_INT.
 */
#define RMT_CH2_TX_THR_EVENT_INT_ENA    (BIT(10))
#define RMT_CH2_TX_THR_EVENT_INT_ENA_M  (RMT_CH2_TX_THR_EVENT_INT_ENA_V << RMT_CH2_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH2_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_ENA_S  10
/** RMT_CH3_TX_THR_EVENT_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for CH3_TX_THR_EVENT_INT.
 */
#define RMT_CH3_TX_THR_EVENT_INT_ENA    (BIT(11))
#define RMT_CH3_TX_THR_EVENT_INT_ENA_M  (RMT_CH3_TX_THR_EVENT_INT_ENA_V << RMT_CH3_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH3_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_ENA_S  11
/** RMT_CH0_TX_LOOP_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for CH0_TX_LOOP_INT.
 */
#define RMT_CH0_TX_LOOP_INT_ENA    (BIT(12))
#define RMT_CH0_TX_LOOP_INT_ENA_M  (RMT_CH0_TX_LOOP_INT_ENA_V << RMT_CH0_TX_LOOP_INT_ENA_S)
#define RMT_CH0_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_ENA_S  12
/** RMT_CH1_TX_LOOP_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The interrupt enable bit for CH1_TX_LOOP_INT.
 */
#define RMT_CH1_TX_LOOP_INT_ENA    (BIT(13))
#define RMT_CH1_TX_LOOP_INT_ENA_M  (RMT_CH1_TX_LOOP_INT_ENA_V << RMT_CH1_TX_LOOP_INT_ENA_S)
#define RMT_CH1_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_ENA_S  13
/** RMT_CH2_TX_LOOP_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  The interrupt enable bit for CH2_TX_LOOP_INT.
 */
#define RMT_CH2_TX_LOOP_INT_ENA    (BIT(14))
#define RMT_CH2_TX_LOOP_INT_ENA_M  (RMT_CH2_TX_LOOP_INT_ENA_V << RMT_CH2_TX_LOOP_INT_ENA_S)
#define RMT_CH2_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_ENA_S  14
/** RMT_CH3_TX_LOOP_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  The interrupt enable bit for CH3_TX_LOOP_INT.
 */
#define RMT_CH3_TX_LOOP_INT_ENA    (BIT(15))
#define RMT_CH3_TX_LOOP_INT_ENA_M  (RMT_CH3_TX_LOOP_INT_ENA_V << RMT_CH3_TX_LOOP_INT_ENA_S)
#define RMT_CH3_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_ENA_S  15
/** RMT_CH4_RX_END_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  The interrupt enable bit for CH4_RX_END_INT.
 */
#define RMT_CH4_RX_END_INT_ENA    (BIT(16))
#define RMT_CH4_RX_END_INT_ENA_M  (RMT_CH4_RX_END_INT_ENA_V << RMT_CH4_RX_END_INT_ENA_S)
#define RMT_CH4_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH4_RX_END_INT_ENA_S  16
/** RMT_CH5_RX_END_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  The interrupt enable bit for CH5_RX_END_INT.
 */
#define RMT_CH5_RX_END_INT_ENA    (BIT(17))
#define RMT_CH5_RX_END_INT_ENA_M  (RMT_CH5_RX_END_INT_ENA_V << RMT_CH5_RX_END_INT_ENA_S)
#define RMT_CH5_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH5_RX_END_INT_ENA_S  17
/** RMT_CH6_RX_END_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  The interrupt enable bit for CH6_RX_END_INT.
 */
#define RMT_CH6_RX_END_INT_ENA    (BIT(18))
#define RMT_CH6_RX_END_INT_ENA_M  (RMT_CH6_RX_END_INT_ENA_V << RMT_CH6_RX_END_INT_ENA_S)
#define RMT_CH6_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH6_RX_END_INT_ENA_S  18
/** RMT_CH7_RX_END_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  The interrupt enable bit for CH7_RX_END_INT.
 */
#define RMT_CH7_RX_END_INT_ENA    (BIT(19))
#define RMT_CH7_RX_END_INT_ENA_M  (RMT_CH7_RX_END_INT_ENA_V << RMT_CH7_RX_END_INT_ENA_S)
#define RMT_CH7_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH7_RX_END_INT_ENA_S  19
/** RMT_CH4_ERR_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  The interrupt enable bit for CH4_ERR_INT.
 */
#define RMT_CH4_ERR_INT_ENA    (BIT(20))
#define RMT_CH4_ERR_INT_ENA_M  (RMT_CH4_ERR_INT_ENA_V << RMT_CH4_ERR_INT_ENA_S)
#define RMT_CH4_ERR_INT_ENA_V  0x00000001U
#define RMT_CH4_ERR_INT_ENA_S  20
/** RMT_CH5_ERR_INT_ENA : R/W; bitpos: [21]; default: 0;
 *  The interrupt enable bit for CH5_ERR_INT.
 */
#define RMT_CH5_ERR_INT_ENA    (BIT(21))
#define RMT_CH5_ERR_INT_ENA_M  (RMT_CH5_ERR_INT_ENA_V << RMT_CH5_ERR_INT_ENA_S)
#define RMT_CH5_ERR_INT_ENA_V  0x00000001U
#define RMT_CH5_ERR_INT_ENA_S  21
/** RMT_CH6_ERR_INT_ENA : R/W; bitpos: [22]; default: 0;
 *  The interrupt enable bit for CH6_ERR_INT.
 */
#define RMT_CH6_ERR_INT_ENA    (BIT(22))
#define RMT_CH6_ERR_INT_ENA_M  (RMT_CH6_ERR_INT_ENA_V << RMT_CH6_ERR_INT_ENA_S)
#define RMT_CH6_ERR_INT_ENA_V  0x00000001U
#define RMT_CH6_ERR_INT_ENA_S  22
/** RMT_CH7_ERR_INT_ENA : R/W; bitpos: [23]; default: 0;
 *  The interrupt enable bit for CH7_ERR_INT.
 */
#define RMT_CH7_ERR_INT_ENA    (BIT(23))
#define RMT_CH7_ERR_INT_ENA_M  (RMT_CH7_ERR_INT_ENA_V << RMT_CH7_ERR_INT_ENA_S)
#define RMT_CH7_ERR_INT_ENA_V  0x00000001U
#define RMT_CH7_ERR_INT_ENA_S  23
/** RMT_CH4_RX_THR_EVENT_INT_ENA : R/W; bitpos: [24]; default: 0;
 *  The interrupt enable bit for CH4_RX_THR_EVENT_INT.
 */
#define RMT_CH4_RX_THR_EVENT_INT_ENA    (BIT(24))
#define RMT_CH4_RX_THR_EVENT_INT_ENA_M  (RMT_CH4_RX_THR_EVENT_INT_ENA_V << RMT_CH4_RX_THR_EVENT_INT_ENA_S)
#define RMT_CH4_RX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH4_RX_THR_EVENT_INT_ENA_S  24
/** RMT_CH5_RX_THR_EVENT_INT_ENA : R/W; bitpos: [25]; default: 0;
 *  The interrupt enable bit for CH5_RX_THR_EVENT_INT.
 */
#define RMT_CH5_RX_THR_EVENT_INT_ENA    (BIT(25))
#define RMT_CH5_RX_THR_EVENT_INT_ENA_M  (RMT_CH5_RX_THR_EVENT_INT_ENA_V << RMT_CH5_RX_THR_EVENT_INT_ENA_S)
#define RMT_CH5_RX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH5_RX_THR_EVENT_INT_ENA_S  25
/** RMT_CH6_RX_THR_EVENT_INT_ENA : R/W; bitpos: [26]; default: 0;
 *  The interrupt enable bit for CH6_RX_THR_EVENT_INT.
 */
#define RMT_CH6_RX_THR_EVENT_INT_ENA    (BIT(26))
#define RMT_CH6_RX_THR_EVENT_INT_ENA_M  (RMT_CH6_RX_THR_EVENT_INT_ENA_V << RMT_CH6_RX_THR_EVENT_INT_ENA_S)
#define RMT_CH6_RX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH6_RX_THR_EVENT_INT_ENA_S  26
/** RMT_CH7_RX_THR_EVENT_INT_ENA : R/W; bitpos: [27]; default: 0;
 *  The interrupt enable bit for CH7_RX_THR_EVENT_INT.
 */
#define RMT_CH7_RX_THR_EVENT_INT_ENA    (BIT(27))
#define RMT_CH7_RX_THR_EVENT_INT_ENA_M  (RMT_CH7_RX_THR_EVENT_INT_ENA_V << RMT_CH7_RX_THR_EVENT_INT_ENA_S)
#define RMT_CH7_RX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH7_RX_THR_EVENT_INT_ENA_S  27
/** RMT_CH3_DMA_ACCESS_FAIL_INT_ENA : R/W; bitpos: [28]; default: 0;
 *  The interrupt enable bit for CH3_DMA_ACCESS_FAIL_INT.
 */
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ENA    (BIT(28))
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ENA_M  (RMT_CH3_DMA_ACCESS_FAIL_INT_ENA_V << RMT_CH3_DMA_ACCESS_FAIL_INT_ENA_S)
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ENA_V  0x00000001U
#define RMT_CH3_DMA_ACCESS_FAIL_INT_ENA_S  28
/** RMT_CH7_DMA_ACCESS_FAIL_INT_ENA : R/W; bitpos: [29]; default: 0;
 *  The interrupt enable bit for CH7_DMA_ACCESS_FAIL_INT.
 */
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ENA    (BIT(29))
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ENA_M  (RMT_CH7_DMA_ACCESS_FAIL_INT_ENA_V << RMT_CH7_DMA_ACCESS_FAIL_INT_ENA_S)
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ENA_V  0x00000001U
#define RMT_CH7_DMA_ACCESS_FAIL_INT_ENA_S  29

/** RMT_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define RMT_INT_CLR_REG (DR_REG_RMT_BASE + 0x7c)
/** RMT_CH0_TX_END_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear theCH0_TX_END_INT interrupt.
 */
#define RMT_CH0_TX_END_INT_CLR    (BIT(0))
#define RMT_CH0_TX_END_INT_CLR_M  (RMT_CH0_TX_END_INT_CLR_V << RMT_CH0_TX_END_INT_CLR_S)
#define RMT_CH0_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH0_TX_END_INT_CLR_S  0
/** RMT_CH1_TX_END_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear theCH1_TX_END_INT interrupt.
 */
#define RMT_CH1_TX_END_INT_CLR    (BIT(1))
#define RMT_CH1_TX_END_INT_CLR_M  (RMT_CH1_TX_END_INT_CLR_V << RMT_CH1_TX_END_INT_CLR_S)
#define RMT_CH1_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH1_TX_END_INT_CLR_S  1
/** RMT_CH2_TX_END_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear theCH2_TX_END_INT interrupt.
 */
#define RMT_CH2_TX_END_INT_CLR    (BIT(2))
#define RMT_CH2_TX_END_INT_CLR_M  (RMT_CH2_TX_END_INT_CLR_V << RMT_CH2_TX_END_INT_CLR_S)
#define RMT_CH2_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH2_TX_END_INT_CLR_S  2
/** RMT_CH3_TX_END_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear theCH3_TX_END_INT interrupt.
 */
#define RMT_CH3_TX_END_INT_CLR    (BIT(3))
#define RMT_CH3_TX_END_INT_CLR_M  (RMT_CH3_TX_END_INT_CLR_V << RMT_CH3_TX_END_INT_CLR_S)
#define RMT_CH3_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH3_TX_END_INT_CLR_S  3
/** RMT_CH0_ERR_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear theCH0_ERR_INT interrupt.
 */
#define RMT_CH0_ERR_INT_CLR    (BIT(4))
#define RMT_CH0_ERR_INT_CLR_M  (RMT_CH0_ERR_INT_CLR_V << RMT_CH0_ERR_INT_CLR_S)
#define RMT_CH0_ERR_INT_CLR_V  0x00000001U
#define RMT_CH0_ERR_INT_CLR_S  4
/** RMT_CH1_ERR_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear theCH1_ERR_INT interrupt.
 */
#define RMT_CH1_ERR_INT_CLR    (BIT(5))
#define RMT_CH1_ERR_INT_CLR_M  (RMT_CH1_ERR_INT_CLR_V << RMT_CH1_ERR_INT_CLR_S)
#define RMT_CH1_ERR_INT_CLR_V  0x00000001U
#define RMT_CH1_ERR_INT_CLR_S  5
/** RMT_CH2_ERR_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear theCH2_ERR_INT interrupt.
 */
#define RMT_CH2_ERR_INT_CLR    (BIT(6))
#define RMT_CH2_ERR_INT_CLR_M  (RMT_CH2_ERR_INT_CLR_V << RMT_CH2_ERR_INT_CLR_S)
#define RMT_CH2_ERR_INT_CLR_V  0x00000001U
#define RMT_CH2_ERR_INT_CLR_S  6
/** RMT_CH3_ERR_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear theCH3_ERR_INT interrupt.
 */
#define RMT_CH3_ERR_INT_CLR    (BIT(7))
#define RMT_CH3_ERR_INT_CLR_M  (RMT_CH3_ERR_INT_CLR_V << RMT_CH3_ERR_INT_CLR_S)
#define RMT_CH3_ERR_INT_CLR_V  0x00000001U
#define RMT_CH3_ERR_INT_CLR_S  7
/** RMT_CH0_TX_THR_EVENT_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear theCH0_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH0_TX_THR_EVENT_INT_CLR    (BIT(8))
#define RMT_CH0_TX_THR_EVENT_INT_CLR_M  (RMT_CH0_TX_THR_EVENT_INT_CLR_V << RMT_CH0_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH0_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_CLR_S  8
/** RMT_CH1_TX_THR_EVENT_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear theCH1_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH1_TX_THR_EVENT_INT_CLR    (BIT(9))
#define RMT_CH1_TX_THR_EVENT_INT_CLR_M  (RMT_CH1_TX_THR_EVENT_INT_CLR_V << RMT_CH1_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH1_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_CLR_S  9
/** RMT_CH2_TX_THR_EVENT_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear theCH2_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH2_TX_THR_EVENT_INT_CLR    (BIT(10))
#define RMT_CH2_TX_THR_EVENT_INT_CLR_M  (RMT_CH2_TX_THR_EVENT_INT_CLR_V << RMT_CH2_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH2_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_CLR_S  10
/** RMT_CH3_TX_THR_EVENT_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear theCH3_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH3_TX_THR_EVENT_INT_CLR    (BIT(11))
#define RMT_CH3_TX_THR_EVENT_INT_CLR_M  (RMT_CH3_TX_THR_EVENT_INT_CLR_V << RMT_CH3_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH3_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_CLR_S  11
/** RMT_CH0_TX_LOOP_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear theCH0_TX_LOOP_INT interrupt.
 */
#define RMT_CH0_TX_LOOP_INT_CLR    (BIT(12))
#define RMT_CH0_TX_LOOP_INT_CLR_M  (RMT_CH0_TX_LOOP_INT_CLR_V << RMT_CH0_TX_LOOP_INT_CLR_S)
#define RMT_CH0_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_CLR_S  12
/** RMT_CH1_TX_LOOP_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Set this bit to clear theCH1_TX_LOOP_INT interrupt.
 */
#define RMT_CH1_TX_LOOP_INT_CLR    (BIT(13))
#define RMT_CH1_TX_LOOP_INT_CLR_M  (RMT_CH1_TX_LOOP_INT_CLR_V << RMT_CH1_TX_LOOP_INT_CLR_S)
#define RMT_CH1_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_CLR_S  13
/** RMT_CH2_TX_LOOP_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Set this bit to clear theCH2_TX_LOOP_INT interrupt.
 */
#define RMT_CH2_TX_LOOP_INT_CLR    (BIT(14))
#define RMT_CH2_TX_LOOP_INT_CLR_M  (RMT_CH2_TX_LOOP_INT_CLR_V << RMT_CH2_TX_LOOP_INT_CLR_S)
#define RMT_CH2_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_CLR_S  14
/** RMT_CH3_TX_LOOP_INT_CLR : WT; bitpos: [15]; default: 0;
 *  Set this bit to clear theCH3_TX_LOOP_INT interrupt.
 */
#define RMT_CH3_TX_LOOP_INT_CLR    (BIT(15))
#define RMT_CH3_TX_LOOP_INT_CLR_M  (RMT_CH3_TX_LOOP_INT_CLR_V << RMT_CH3_TX_LOOP_INT_CLR_S)
#define RMT_CH3_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_CLR_S  15
/** RMT_CH4_RX_END_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Set this bit to clear theCH4_RX_END_INT interrupt.
 */
#define RMT_CH4_RX_END_INT_CLR    (BIT(16))
#define RMT_CH4_RX_END_INT_CLR_M  (RMT_CH4_RX_END_INT_CLR_V << RMT_CH4_RX_END_INT_CLR_S)
#define RMT_CH4_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH4_RX_END_INT_CLR_S  16
/** RMT_CH5_RX_END_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Set this bit to clear theCH5_RX_END_INT interrupt.
 */
#define RMT_CH5_RX_END_INT_CLR    (BIT(17))
#define RMT_CH5_RX_END_INT_CLR_M  (RMT_CH5_RX_END_INT_CLR_V << RMT_CH5_RX_END_INT_CLR_S)
#define RMT_CH5_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH5_RX_END_INT_CLR_S  17
/** RMT_CH6_RX_END_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Set this bit to clear theCH6_RX_END_INT interrupt.
 */
#define RMT_CH6_RX_END_INT_CLR    (BIT(18))
#define RMT_CH6_RX_END_INT_CLR_M  (RMT_CH6_RX_END_INT_CLR_V << RMT_CH6_RX_END_INT_CLR_S)
#define RMT_CH6_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH6_RX_END_INT_CLR_S  18
/** RMT_CH7_RX_END_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Set this bit to clear theCH7_RX_END_INT interrupt.
 */
#define RMT_CH7_RX_END_INT_CLR    (BIT(19))
#define RMT_CH7_RX_END_INT_CLR_M  (RMT_CH7_RX_END_INT_CLR_V << RMT_CH7_RX_END_INT_CLR_S)
#define RMT_CH7_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH7_RX_END_INT_CLR_S  19
/** RMT_CH4_ERR_INT_CLR : WT; bitpos: [20]; default: 0;
 *  Set this bit to clear theCH4_ERR_INT interrupt.
 */
#define RMT_CH4_ERR_INT_CLR    (BIT(20))
#define RMT_CH4_ERR_INT_CLR_M  (RMT_CH4_ERR_INT_CLR_V << RMT_CH4_ERR_INT_CLR_S)
#define RMT_CH4_ERR_INT_CLR_V  0x00000001U
#define RMT_CH4_ERR_INT_CLR_S  20
/** RMT_CH5_ERR_INT_CLR : WT; bitpos: [21]; default: 0;
 *  Set this bit to clear theCH5_ERR_INT interrupt.
 */
#define RMT_CH5_ERR_INT_CLR    (BIT(21))
#define RMT_CH5_ERR_INT_CLR_M  (RMT_CH5_ERR_INT_CLR_V << RMT_CH5_ERR_INT_CLR_S)
#define RMT_CH5_ERR_INT_CLR_V  0x00000001U
#define RMT_CH5_ERR_INT_CLR_S  21
/** RMT_CH6_ERR_INT_CLR : WT; bitpos: [22]; default: 0;
 *  Set this bit to clear theCH6_ERR_INT interrupt.
 */
#define RMT_CH6_ERR_INT_CLR    (BIT(22))
#define RMT_CH6_ERR_INT_CLR_M  (RMT_CH6_ERR_INT_CLR_V << RMT_CH6_ERR_INT_CLR_S)
#define RMT_CH6_ERR_INT_CLR_V  0x00000001U
#define RMT_CH6_ERR_INT_CLR_S  22
/** RMT_CH7_ERR_INT_CLR : WT; bitpos: [23]; default: 0;
 *  Set this bit to clear theCH7_ERR_INT interrupt.
 */
#define RMT_CH7_ERR_INT_CLR    (BIT(23))
#define RMT_CH7_ERR_INT_CLR_M  (RMT_CH7_ERR_INT_CLR_V << RMT_CH7_ERR_INT_CLR_S)
#define RMT_CH7_ERR_INT_CLR_V  0x00000001U
#define RMT_CH7_ERR_INT_CLR_S  23
/** RMT_CH4_RX_THR_EVENT_INT_CLR : WT; bitpos: [24]; default: 0;
 *  Set this bit to clear theCH4_RX_THR_EVENT_INT interrupt.
 */
#define RMT_CH4_RX_THR_EVENT_INT_CLR    (BIT(24))
#define RMT_CH4_RX_THR_EVENT_INT_CLR_M  (RMT_CH4_RX_THR_EVENT_INT_CLR_V << RMT_CH4_RX_THR_EVENT_INT_CLR_S)
#define RMT_CH4_RX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH4_RX_THR_EVENT_INT_CLR_S  24
/** RMT_CH5_RX_THR_EVENT_INT_CLR : WT; bitpos: [25]; default: 0;
 *  Set this bit to clear theCH5_RX_THR_EVENT_INT interrupt.
 */
#define RMT_CH5_RX_THR_EVENT_INT_CLR    (BIT(25))
#define RMT_CH5_RX_THR_EVENT_INT_CLR_M  (RMT_CH5_RX_THR_EVENT_INT_CLR_V << RMT_CH5_RX_THR_EVENT_INT_CLR_S)
#define RMT_CH5_RX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH5_RX_THR_EVENT_INT_CLR_S  25
/** RMT_CH6_RX_THR_EVENT_INT_CLR : WT; bitpos: [26]; default: 0;
 *  Set this bit to clear theCH6_RX_THR_EVENT_INT interrupt.
 */
#define RMT_CH6_RX_THR_EVENT_INT_CLR    (BIT(26))
#define RMT_CH6_RX_THR_EVENT_INT_CLR_M  (RMT_CH6_RX_THR_EVENT_INT_CLR_V << RMT_CH6_RX_THR_EVENT_INT_CLR_S)
#define RMT_CH6_RX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH6_RX_THR_EVENT_INT_CLR_S  26
/** RMT_CH7_RX_THR_EVENT_INT_CLR : WT; bitpos: [27]; default: 0;
 *  Set this bit to clear theCH7_RX_THR_EVENT_INT interrupt.
 */
#define RMT_CH7_RX_THR_EVENT_INT_CLR    (BIT(27))
#define RMT_CH7_RX_THR_EVENT_INT_CLR_M  (RMT_CH7_RX_THR_EVENT_INT_CLR_V << RMT_CH7_RX_THR_EVENT_INT_CLR_S)
#define RMT_CH7_RX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH7_RX_THR_EVENT_INT_CLR_S  27
/** RMT_CH3_DMA_ACCESS_FAIL_INT_CLR : WT; bitpos: [28]; default: 0;
 *  Set this bit to clear the CH3_DMA_ACCESS_FAIL_INT interrupt.
 */
#define RMT_CH3_DMA_ACCESS_FAIL_INT_CLR    (BIT(28))
#define RMT_CH3_DMA_ACCESS_FAIL_INT_CLR_M  (RMT_CH3_DMA_ACCESS_FAIL_INT_CLR_V << RMT_CH3_DMA_ACCESS_FAIL_INT_CLR_S)
#define RMT_CH3_DMA_ACCESS_FAIL_INT_CLR_V  0x00000001U
#define RMT_CH3_DMA_ACCESS_FAIL_INT_CLR_S  28
/** RMT_CH7_DMA_ACCESS_FAIL_INT_CLR : WT; bitpos: [29]; default: 0;
 *  Set this bit to clear the CH7_DMA_ACCESS_FAIL_INT interrupt.
 */
#define RMT_CH7_DMA_ACCESS_FAIL_INT_CLR    (BIT(29))
#define RMT_CH7_DMA_ACCESS_FAIL_INT_CLR_M  (RMT_CH7_DMA_ACCESS_FAIL_INT_CLR_V << RMT_CH7_DMA_ACCESS_FAIL_INT_CLR_S)
#define RMT_CH7_DMA_ACCESS_FAIL_INT_CLR_V  0x00000001U
#define RMT_CH7_DMA_ACCESS_FAIL_INT_CLR_S  29

/** RMT_CH0CARRIER_DUTY_REG register
 *  Channel 0 duty cycle configuration register
 */
#define RMT_CH0CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x80)
/** RMT_CARRIER_LOW_CH0 : R/W; bitpos: [15:0]; default: 64;
 *  This register is used to configure carrier wave 's low level clock period for
 *  CHANNEL0.
 */
#define RMT_CARRIER_LOW_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_M  (RMT_CARRIER_LOW_CH0_V << RMT_CARRIER_LOW_CH0_S)
#define RMT_CARRIER_LOW_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_S  0
/** RMT_CARRIER_HIGH_CH0 : R/W; bitpos: [31:16]; default: 64;
 *  This register is used to configure carrier wave 's high level clock period for
 *  CHANNEL0.
 */
#define RMT_CARRIER_HIGH_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_M  (RMT_CARRIER_HIGH_CH0_V << RMT_CARRIER_HIGH_CH0_S)
#define RMT_CARRIER_HIGH_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_S  16

/** RMT_CH1CARRIER_DUTY_REG register
 *  Channel 1 duty cycle configuration register
 */
#define RMT_CH1CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x84)
/** RMT_CARRIER_LOW_CH1 : R/W; bitpos: [15:0]; default: 64;
 *  This register is used to configure carrier wave 's low level clock period for
 *  CHANNEL1.
 */
#define RMT_CARRIER_LOW_CH1    0x0000FFFFU
#define RMT_CARRIER_LOW_CH1_M  (RMT_CARRIER_LOW_CH1_V << RMT_CARRIER_LOW_CH1_S)
#define RMT_CARRIER_LOW_CH1_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH1_S  0
/** RMT_CARRIER_HIGH_CH1 : R/W; bitpos: [31:16]; default: 64;
 *  This register is used to configure carrier wave 's high level clock period for
 *  CHANNEL1.
 */
#define RMT_CARRIER_HIGH_CH1    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH1_M  (RMT_CARRIER_HIGH_CH1_V << RMT_CARRIER_HIGH_CH1_S)
#define RMT_CARRIER_HIGH_CH1_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH1_S  16

/** RMT_CH2CARRIER_DUTY_REG register
 *  Channel 2 duty cycle configuration register
 */
#define RMT_CH2CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x88)
/** RMT_CARRIER_LOW_CH2 : R/W; bitpos: [15:0]; default: 64;
 *  This register is used to configure carrier wave 's low level clock period for
 *  CHANNEL2.
 */
#define RMT_CARRIER_LOW_CH2    0x0000FFFFU
#define RMT_CARRIER_LOW_CH2_M  (RMT_CARRIER_LOW_CH2_V << RMT_CARRIER_LOW_CH2_S)
#define RMT_CARRIER_LOW_CH2_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH2_S  0
/** RMT_CARRIER_HIGH_CH2 : R/W; bitpos: [31:16]; default: 64;
 *  This register is used to configure carrier wave 's high level clock period for
 *  CHANNEL2.
 */
#define RMT_CARRIER_HIGH_CH2    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH2_M  (RMT_CARRIER_HIGH_CH2_V << RMT_CARRIER_HIGH_CH2_S)
#define RMT_CARRIER_HIGH_CH2_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH2_S  16

/** RMT_CH3CARRIER_DUTY_REG register
 *  Channel 3 duty cycle configuration register
 */
#define RMT_CH3CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x8c)
/** RMT_CARRIER_LOW_CH3 : R/W; bitpos: [15:0]; default: 64;
 *  This register is used to configure carrier wave 's low level clock period for
 *  CHANNEL3.
 */
#define RMT_CARRIER_LOW_CH3    0x0000FFFFU
#define RMT_CARRIER_LOW_CH3_M  (RMT_CARRIER_LOW_CH3_V << RMT_CARRIER_LOW_CH3_S)
#define RMT_CARRIER_LOW_CH3_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH3_S  0
/** RMT_CARRIER_HIGH_CH3 : R/W; bitpos: [31:16]; default: 64;
 *  This register is used to configure carrier wave 's high level clock period for
 *  CHANNEL3.
 */
#define RMT_CARRIER_HIGH_CH3    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH3_M  (RMT_CARRIER_HIGH_CH3_V << RMT_CARRIER_HIGH_CH3_S)
#define RMT_CARRIER_HIGH_CH3_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH3_S  16

/** RMT_CH4_RX_CARRIER_RM_REG register
 *  Channel 4 carrier remove register
 */
#define RMT_CH4_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x90)
/** RMT_CARRIER_LOW_THRES_CH4 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_LOW_THRES_CH4 + 1) for channel 4.
 */
#define RMT_CARRIER_LOW_THRES_CH4    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH4_M  (RMT_CARRIER_LOW_THRES_CH4_V << RMT_CARRIER_LOW_THRES_CH4_S)
#define RMT_CARRIER_LOW_THRES_CH4_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH4_S  0
/** RMT_CARRIER_HIGH_THRES_CH4 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_HIGH_THRES_CH4 + 1) for channel 4.
 */
#define RMT_CARRIER_HIGH_THRES_CH4    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH4_M  (RMT_CARRIER_HIGH_THRES_CH4_V << RMT_CARRIER_HIGH_THRES_CH4_S)
#define RMT_CARRIER_HIGH_THRES_CH4_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH4_S  16

/** RMT_CH5_RX_CARRIER_RM_REG register
 *  Channel 5 carrier remove register
 */
#define RMT_CH5_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x94)
/** RMT_CARRIER_LOW_THRES_CH5 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_LOW_THRES_CH5 + 1) for channel 5.
 */
#define RMT_CARRIER_LOW_THRES_CH5    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH5_M  (RMT_CARRIER_LOW_THRES_CH5_V << RMT_CARRIER_LOW_THRES_CH5_S)
#define RMT_CARRIER_LOW_THRES_CH5_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH5_S  0
/** RMT_CARRIER_HIGH_THRES_CH5 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_HIGH_THRES_CH5 + 1) for channel 5.
 */
#define RMT_CARRIER_HIGH_THRES_CH5    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH5_M  (RMT_CARRIER_HIGH_THRES_CH5_V << RMT_CARRIER_HIGH_THRES_CH5_S)
#define RMT_CARRIER_HIGH_THRES_CH5_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH5_S  16

/** RMT_CH6_RX_CARRIER_RM_REG register
 *  Channel 6 carrier remove register
 */
#define RMT_CH6_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x98)
/** RMT_CARRIER_LOW_THRES_CH6 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_LOW_THRES_CH6 + 1) for channel 6.
 */
#define RMT_CARRIER_LOW_THRES_CH6    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH6_M  (RMT_CARRIER_LOW_THRES_CH6_V << RMT_CARRIER_LOW_THRES_CH6_S)
#define RMT_CARRIER_LOW_THRES_CH6_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH6_S  0
/** RMT_CARRIER_HIGH_THRES_CH6 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_HIGH_THRES_CH6 + 1) for channel 6.
 */
#define RMT_CARRIER_HIGH_THRES_CH6    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH6_M  (RMT_CARRIER_HIGH_THRES_CH6_V << RMT_CARRIER_HIGH_THRES_CH6_S)
#define RMT_CARRIER_HIGH_THRES_CH6_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH6_S  16

/** RMT_CH7_RX_CARRIER_RM_REG register
 *  Channel 7 carrier remove register
 */
#define RMT_CH7_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x9c)
/** RMT_CARRIER_LOW_THRES_CH7 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_LOW_THRES_CH7 + 1) for channel 7.
 */
#define RMT_CARRIER_LOW_THRES_CH7    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH7_M  (RMT_CARRIER_LOW_THRES_CH7_V << RMT_CARRIER_LOW_THRES_CH7_S)
#define RMT_CARRIER_LOW_THRES_CH7_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH7_S  0
/** RMT_CARRIER_HIGH_THRES_CH7 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_HIGH_THRES_CH7 + 1) for channel 7.
 */
#define RMT_CARRIER_HIGH_THRES_CH7    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH7_M  (RMT_CARRIER_HIGH_THRES_CH7_V << RMT_CARRIER_HIGH_THRES_CH7_S)
#define RMT_CARRIER_HIGH_THRES_CH7_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH7_S  16

/** RMT_CH0_TX_LIM_REG register
 *  Channel 0 Tx event configuration register
 */
#define RMT_CH0_TX_LIM_REG (DR_REG_RMT_BASE + 0xa0)
/** RMT_TX_LIM_CH0 : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL0 can send out.
 */
#define RMT_TX_LIM_CH0    0x000001FFU
#define RMT_TX_LIM_CH0_M  (RMT_TX_LIM_CH0_V << RMT_TX_LIM_CH0_S)
#define RMT_TX_LIM_CH0_V  0x000001FFU
#define RMT_TX_LIM_CH0_S  0
/** RMT_TX_LOOP_NUM_CH0 : R/W; bitpos: [18:9]; default: 0;
 *  This register is used to configure the maximum loop count when tx_conti_mode is
 *  valid.
 */
#define RMT_TX_LOOP_NUM_CH0    0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_M  (RMT_TX_LOOP_NUM_CH0_V << RMT_TX_LOOP_NUM_CH0_S)
#define RMT_TX_LOOP_NUM_CH0_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_S  9
/** RMT_TX_LOOP_CNT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This register is the enabled bit for loop count.
 */
#define RMT_TX_LOOP_CNT_EN_CH0    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH0_M  (RMT_TX_LOOP_CNT_EN_CH0_V << RMT_TX_LOOP_CNT_EN_CH0_S)
#define RMT_TX_LOOP_CNT_EN_CH0_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH0_S  19
/** RMT_LOOP_COUNT_RESET_CH0 : WT; bitpos: [20]; default: 0;
 *  This register is used to reset the loop count when tx_conti_mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH0    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH0_M  (RMT_LOOP_COUNT_RESET_CH0_V << RMT_LOOP_COUNT_RESET_CH0_S)
#define RMT_LOOP_COUNT_RESET_CH0_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH0_S  20
/** RMT_LOOP_STOP_EN_CH0 : R/W; bitpos: [21]; default: 0;
 *  This bit is used to enable the loop send stop function after the loop counter
 *  counts to  loop number for CHANNEL0.
 */
#define RMT_LOOP_STOP_EN_CH0    (BIT(21))
#define RMT_LOOP_STOP_EN_CH0_M  (RMT_LOOP_STOP_EN_CH0_V << RMT_LOOP_STOP_EN_CH0_S)
#define RMT_LOOP_STOP_EN_CH0_V  0x00000001U
#define RMT_LOOP_STOP_EN_CH0_S  21

/** RMT_CH1_TX_LIM_REG register
 *  Channel 1 Tx event configuration register
 */
#define RMT_CH1_TX_LIM_REG (DR_REG_RMT_BASE + 0xa4)
/** RMT_TX_LIM_CH1 : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL1 can send out.
 */
#define RMT_TX_LIM_CH1    0x000001FFU
#define RMT_TX_LIM_CH1_M  (RMT_TX_LIM_CH1_V << RMT_TX_LIM_CH1_S)
#define RMT_TX_LIM_CH1_V  0x000001FFU
#define RMT_TX_LIM_CH1_S  0
/** RMT_TX_LOOP_NUM_CH1 : R/W; bitpos: [18:9]; default: 0;
 *  This register is used to configure the maximum loop count when tx_conti_mode is
 *  valid.
 */
#define RMT_TX_LOOP_NUM_CH1    0x000003FFU
#define RMT_TX_LOOP_NUM_CH1_M  (RMT_TX_LOOP_NUM_CH1_V << RMT_TX_LOOP_NUM_CH1_S)
#define RMT_TX_LOOP_NUM_CH1_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH1_S  9
/** RMT_TX_LOOP_CNT_EN_CH1 : R/W; bitpos: [19]; default: 0;
 *  This register is the enabled bit for loop count.
 */
#define RMT_TX_LOOP_CNT_EN_CH1    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH1_M  (RMT_TX_LOOP_CNT_EN_CH1_V << RMT_TX_LOOP_CNT_EN_CH1_S)
#define RMT_TX_LOOP_CNT_EN_CH1_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH1_S  19
/** RMT_LOOP_COUNT_RESET_CH1 : WT; bitpos: [20]; default: 0;
 *  This register is used to reset the loop count when tx_conti_mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH1    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH1_M  (RMT_LOOP_COUNT_RESET_CH1_V << RMT_LOOP_COUNT_RESET_CH1_S)
#define RMT_LOOP_COUNT_RESET_CH1_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH1_S  20
/** RMT_LOOP_STOP_EN_CH1 : R/W; bitpos: [21]; default: 0;
 *  This bit is used to enable the loop send stop function after the loop counter
 *  counts to  loop number for CHANNEL1.
 */
#define RMT_LOOP_STOP_EN_CH1    (BIT(21))
#define RMT_LOOP_STOP_EN_CH1_M  (RMT_LOOP_STOP_EN_CH1_V << RMT_LOOP_STOP_EN_CH1_S)
#define RMT_LOOP_STOP_EN_CH1_V  0x00000001U
#define RMT_LOOP_STOP_EN_CH1_S  21

/** RMT_CH2_TX_LIM_REG register
 *  Channel 2 Tx event configuration register
 */
#define RMT_CH2_TX_LIM_REG (DR_REG_RMT_BASE + 0xa8)
/** RMT_TX_LIM_CH2 : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL2 can send out.
 */
#define RMT_TX_LIM_CH2    0x000001FFU
#define RMT_TX_LIM_CH2_M  (RMT_TX_LIM_CH2_V << RMT_TX_LIM_CH2_S)
#define RMT_TX_LIM_CH2_V  0x000001FFU
#define RMT_TX_LIM_CH2_S  0
/** RMT_TX_LOOP_NUM_CH2 : R/W; bitpos: [18:9]; default: 0;
 *  This register is used to configure the maximum loop count when tx_conti_mode is
 *  valid.
 */
#define RMT_TX_LOOP_NUM_CH2    0x000003FFU
#define RMT_TX_LOOP_NUM_CH2_M  (RMT_TX_LOOP_NUM_CH2_V << RMT_TX_LOOP_NUM_CH2_S)
#define RMT_TX_LOOP_NUM_CH2_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH2_S  9
/** RMT_TX_LOOP_CNT_EN_CH2 : R/W; bitpos: [19]; default: 0;
 *  This register is the enabled bit for loop count.
 */
#define RMT_TX_LOOP_CNT_EN_CH2    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH2_M  (RMT_TX_LOOP_CNT_EN_CH2_V << RMT_TX_LOOP_CNT_EN_CH2_S)
#define RMT_TX_LOOP_CNT_EN_CH2_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH2_S  19
/** RMT_LOOP_COUNT_RESET_CH2 : WT; bitpos: [20]; default: 0;
 *  This register is used to reset the loop count when tx_conti_mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH2    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH2_M  (RMT_LOOP_COUNT_RESET_CH2_V << RMT_LOOP_COUNT_RESET_CH2_S)
#define RMT_LOOP_COUNT_RESET_CH2_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH2_S  20
/** RMT_LOOP_STOP_EN_CH2 : R/W; bitpos: [21]; default: 0;
 *  This bit is used to enable the loop send stop function after the loop counter
 *  counts to  loop number for CHANNEL2.
 */
#define RMT_LOOP_STOP_EN_CH2    (BIT(21))
#define RMT_LOOP_STOP_EN_CH2_M  (RMT_LOOP_STOP_EN_CH2_V << RMT_LOOP_STOP_EN_CH2_S)
#define RMT_LOOP_STOP_EN_CH2_V  0x00000001U
#define RMT_LOOP_STOP_EN_CH2_S  21

/** RMT_CH3_TX_LIM_REG register
 *  Channel 3 Tx event configuration register
 */
#define RMT_CH3_TX_LIM_REG (DR_REG_RMT_BASE + 0xac)
/** RMT_TX_LIM_CH3 : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL3 can send out.
 */
#define RMT_TX_LIM_CH3    0x000001FFU
#define RMT_TX_LIM_CH3_M  (RMT_TX_LIM_CH3_V << RMT_TX_LIM_CH3_S)
#define RMT_TX_LIM_CH3_V  0x000001FFU
#define RMT_TX_LIM_CH3_S  0
/** RMT_TX_LOOP_NUM_CH3 : R/W; bitpos: [18:9]; default: 0;
 *  This register is used to configure the maximum loop count when tx_conti_mode is
 *  valid.
 */
#define RMT_TX_LOOP_NUM_CH3    0x000003FFU
#define RMT_TX_LOOP_NUM_CH3_M  (RMT_TX_LOOP_NUM_CH3_V << RMT_TX_LOOP_NUM_CH3_S)
#define RMT_TX_LOOP_NUM_CH3_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH3_S  9
/** RMT_TX_LOOP_CNT_EN_CH3 : R/W; bitpos: [19]; default: 0;
 *  This register is the enabled bit for loop count.
 */
#define RMT_TX_LOOP_CNT_EN_CH3    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH3_M  (RMT_TX_LOOP_CNT_EN_CH3_V << RMT_TX_LOOP_CNT_EN_CH3_S)
#define RMT_TX_LOOP_CNT_EN_CH3_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH3_S  19
/** RMT_LOOP_COUNT_RESET_CH3 : WT; bitpos: [20]; default: 0;
 *  This register is used to reset the loop count when tx_conti_mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH3    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH3_M  (RMT_LOOP_COUNT_RESET_CH3_V << RMT_LOOP_COUNT_RESET_CH3_S)
#define RMT_LOOP_COUNT_RESET_CH3_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH3_S  20
/** RMT_LOOP_STOP_EN_CH3 : R/W; bitpos: [21]; default: 0;
 *  This bit is used to enable the loop send stop function after the loop counter
 *  counts to  loop number for CHANNEL3.
 */
#define RMT_LOOP_STOP_EN_CH3    (BIT(21))
#define RMT_LOOP_STOP_EN_CH3_M  (RMT_LOOP_STOP_EN_CH3_V << RMT_LOOP_STOP_EN_CH3_S)
#define RMT_LOOP_STOP_EN_CH3_V  0x00000001U
#define RMT_LOOP_STOP_EN_CH3_S  21

/** RMT_CH4_RX_LIM_REG register
 *  Channel 4 Rx event configuration register
 */
#define RMT_CH4_RX_LIM_REG (DR_REG_RMT_BASE + 0xb0)
/** RMT_CH4_RX_LIM_REG : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL4 can receive.
 */
#define RMT_CH4_RX_LIM_REG    0x000001FFU
#define RMT_CH4_RX_LIM_REG_M  (RMT_CH4_RX_LIM_REG_V << RMT_CH4_RX_LIM_REG_S)
#define RMT_CH4_RX_LIM_REG_V  0x000001FFU
#define RMT_CH4_RX_LIM_REG_S  0

/** RMT_CH5_RX_LIM_REG register
 *  Channel 5 Rx event configuration register
 */
#define RMT_CH5_RX_LIM_REG (DR_REG_RMT_BASE + 0xb4)
/** RMT_CH5_RX_LIM_REG : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL5 can receive.
 */
#define RMT_CH5_RX_LIM_REG    0x000001FFU
#define RMT_CH5_RX_LIM_REG_M  (RMT_CH5_RX_LIM_REG_V << RMT_CH5_RX_LIM_REG_S)
#define RMT_CH5_RX_LIM_REG_V  0x000001FFU
#define RMT_CH5_RX_LIM_REG_S  0

/** RMT_CH6_RX_LIM_REG register
 *  Channel 6 Rx event configuration register
 */
#define RMT_CH6_RX_LIM_REG (DR_REG_RMT_BASE + 0xb8)
/** RMT_CH6_RX_LIM_REG : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL6 can receive.
 */
#define RMT_CH6_RX_LIM_REG    0x000001FFU
#define RMT_CH6_RX_LIM_REG_M  (RMT_CH6_RX_LIM_REG_V << RMT_CH6_RX_LIM_REG_S)
#define RMT_CH6_RX_LIM_REG_V  0x000001FFU
#define RMT_CH6_RX_LIM_REG_S  0

/** RMT_CH7_RX_LIM_REG register
 *  Channel 7 Rx event configuration register
 */
#define RMT_CH7_RX_LIM_REG (DR_REG_RMT_BASE + 0xbc)
/** RMT_CH7_RX_LIM_REG : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL7 can receive.
 */
#define RMT_CH7_RX_LIM_REG    0x000001FFU
#define RMT_CH7_RX_LIM_REG_M  (RMT_CH7_RX_LIM_REG_V << RMT_CH7_RX_LIM_REG_S)
#define RMT_CH7_RX_LIM_REG_V  0x000001FFU
#define RMT_CH7_RX_LIM_REG_S  0

/** RMT_SYS_CONF_REG register
 *  RMT apb configuration register
 */
#define RMT_SYS_CONF_REG (DR_REG_RMT_BASE + 0xc0)
/** RMT_APB_FIFO_MASK : R/W; bitpos: [0]; default: 0;
 *  1'h1: access memory directly.   1'h0: access memory by FIFO.
 */
#define RMT_APB_FIFO_MASK    (BIT(0))
#define RMT_APB_FIFO_MASK_M  (RMT_APB_FIFO_MASK_V << RMT_APB_FIFO_MASK_S)
#define RMT_APB_FIFO_MASK_V  0x00000001U
#define RMT_APB_FIFO_MASK_S  0
/** RMT_MEM_CLK_FORCE_ON : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable the clock for RMT memory.
 */
#define RMT_MEM_CLK_FORCE_ON    (BIT(1))
#define RMT_MEM_CLK_FORCE_ON_M  (RMT_MEM_CLK_FORCE_ON_V << RMT_MEM_CLK_FORCE_ON_S)
#define RMT_MEM_CLK_FORCE_ON_V  0x00000001U
#define RMT_MEM_CLK_FORCE_ON_S  1
/** RMT_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down RMT memory.
 */
#define RMT_MEM_FORCE_PD    (BIT(2))
#define RMT_MEM_FORCE_PD_M  (RMT_MEM_FORCE_PD_V << RMT_MEM_FORCE_PD_S)
#define RMT_MEM_FORCE_PD_V  0x00000001U
#define RMT_MEM_FORCE_PD_S  2
/** RMT_MEM_FORCE_PU : R/W; bitpos: [3]; default: 0;
 *  1: Disable RMT memory light sleep power down function. 0: Power down RMT memory
 *  when RMT is in light sleep mode.
 */
#define RMT_MEM_FORCE_PU    (BIT(3))
#define RMT_MEM_FORCE_PU_M  (RMT_MEM_FORCE_PU_V << RMT_MEM_FORCE_PU_S)
#define RMT_MEM_FORCE_PU_V  0x00000001U
#define RMT_MEM_FORCE_PU_S  3
/** RMT_SCLK_DIV_NUM : R/W; bitpos: [11:4]; default: 1;
 *  the integral part of the fractional divisor
 */
#define RMT_SCLK_DIV_NUM    0x000000FFU
#define RMT_SCLK_DIV_NUM_M  (RMT_SCLK_DIV_NUM_V << RMT_SCLK_DIV_NUM_S)
#define RMT_SCLK_DIV_NUM_V  0x000000FFU
#define RMT_SCLK_DIV_NUM_S  4
/** RMT_SCLK_DIV_A : R/W; bitpos: [17:12]; default: 0;
 *  the numerator of the fractional part of the fractional divisor
 */
#define RMT_SCLK_DIV_A    0x0000003FU
#define RMT_SCLK_DIV_A_M  (RMT_SCLK_DIV_A_V << RMT_SCLK_DIV_A_S)
#define RMT_SCLK_DIV_A_V  0x0000003FU
#define RMT_SCLK_DIV_A_S  12
/** RMT_SCLK_DIV_B : R/W; bitpos: [23:18]; default: 0;
 *  the denominator of the fractional part of the fractional divisor
 */
#define RMT_SCLK_DIV_B    0x0000003FU
#define RMT_SCLK_DIV_B_M  (RMT_SCLK_DIV_B_V << RMT_SCLK_DIV_B_S)
#define RMT_SCLK_DIV_B_V  0x0000003FU
#define RMT_SCLK_DIV_B_S  18
/** RMT_SCLK_SEL : R/W; bitpos: [25:24]; default: 1;
 *  choose the clock source of rmt_sclk. 1:CLK_80Mhz;2:CLK_8MHz; 2:XTAL
 */
#define RMT_SCLK_SEL    0x00000003U
#define RMT_SCLK_SEL_M  (RMT_SCLK_SEL_V << RMT_SCLK_SEL_S)
#define RMT_SCLK_SEL_V  0x00000003U
#define RMT_SCLK_SEL_S  24
/** RMT_SCLK_ACTIVE : R/W; bitpos: [26]; default: 1;
 *  rmt_sclk switch
 */
#define RMT_SCLK_ACTIVE    (BIT(26))
#define RMT_SCLK_ACTIVE_M  (RMT_SCLK_ACTIVE_V << RMT_SCLK_ACTIVE_S)
#define RMT_SCLK_ACTIVE_V  0x00000001U
#define RMT_SCLK_ACTIVE_S  26
/** RMT_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  RMT register clock gate enable signal. 1: Power up the drive clock of registers. 0:
 *  Power down the drive clock of registers
 */
#define RMT_CLK_EN    (BIT(31))
#define RMT_CLK_EN_M  (RMT_CLK_EN_V << RMT_CLK_EN_S)
#define RMT_CLK_EN_V  0x00000001U
#define RMT_CLK_EN_S  31

/** RMT_TX_SIM_REG register
 *  RMT TX synchronous register
 */
#define RMT_TX_SIM_REG (DR_REG_RMT_BASE + 0xc4)
/** RMT_TX_SIM_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable CHANNEL0 to start sending data synchronously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH0    (BIT(0))
#define RMT_TX_SIM_CH0_M  (RMT_TX_SIM_CH0_V << RMT_TX_SIM_CH0_S)
#define RMT_TX_SIM_CH0_V  0x00000001U
#define RMT_TX_SIM_CH0_S  0
/** RMT_TX_SIM_CH1 : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable CHANNEL1 to start sending data synchronously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH1    (BIT(1))
#define RMT_TX_SIM_CH1_M  (RMT_TX_SIM_CH1_V << RMT_TX_SIM_CH1_S)
#define RMT_TX_SIM_CH1_V  0x00000001U
#define RMT_TX_SIM_CH1_S  1
/** RMT_TX_SIM_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable CHANNEL2 to start sending data synchronously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH2    (BIT(2))
#define RMT_TX_SIM_CH2_M  (RMT_TX_SIM_CH2_V << RMT_TX_SIM_CH2_S)
#define RMT_TX_SIM_CH2_V  0x00000001U
#define RMT_TX_SIM_CH2_S  2
/** RMT_TX_SIM_CH3 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to enable CHANNEL3 to start sending data synchronously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH3    (BIT(3))
#define RMT_TX_SIM_CH3_M  (RMT_TX_SIM_CH3_V << RMT_TX_SIM_CH3_S)
#define RMT_TX_SIM_CH3_V  0x00000001U
#define RMT_TX_SIM_CH3_S  3
/** RMT_TX_SIM_EN : R/W; bitpos: [4]; default: 0;
 *  This register is used to enable multiple of channels to start sending data
 *  synchronously.
 */
#define RMT_TX_SIM_EN    (BIT(4))
#define RMT_TX_SIM_EN_M  (RMT_TX_SIM_EN_V << RMT_TX_SIM_EN_S)
#define RMT_TX_SIM_EN_V  0x00000001U
#define RMT_TX_SIM_EN_S  4

/** RMT_REF_CNT_RST_REG register
 *  RMT clock divider reset register
 */
#define RMT_REF_CNT_RST_REG (DR_REG_RMT_BASE + 0xc8)
/** RMT_REF_CNT_RST_CH0 : WT; bitpos: [0]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL0.
 */
#define RMT_REF_CNT_RST_CH0    (BIT(0))
#define RMT_REF_CNT_RST_CH0_M  (RMT_REF_CNT_RST_CH0_V << RMT_REF_CNT_RST_CH0_S)
#define RMT_REF_CNT_RST_CH0_V  0x00000001U
#define RMT_REF_CNT_RST_CH0_S  0
/** RMT_REF_CNT_RST_CH1 : WT; bitpos: [1]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL1.
 */
#define RMT_REF_CNT_RST_CH1    (BIT(1))
#define RMT_REF_CNT_RST_CH1_M  (RMT_REF_CNT_RST_CH1_V << RMT_REF_CNT_RST_CH1_S)
#define RMT_REF_CNT_RST_CH1_V  0x00000001U
#define RMT_REF_CNT_RST_CH1_S  1
/** RMT_REF_CNT_RST_CH2 : WT; bitpos: [2]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL2.
 */
#define RMT_REF_CNT_RST_CH2    (BIT(2))
#define RMT_REF_CNT_RST_CH2_M  (RMT_REF_CNT_RST_CH2_V << RMT_REF_CNT_RST_CH2_S)
#define RMT_REF_CNT_RST_CH2_V  0x00000001U
#define RMT_REF_CNT_RST_CH2_S  2
/** RMT_REF_CNT_RST_CH3 : WT; bitpos: [3]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL3.
 */
#define RMT_REF_CNT_RST_CH3    (BIT(3))
#define RMT_REF_CNT_RST_CH3_M  (RMT_REF_CNT_RST_CH3_V << RMT_REF_CNT_RST_CH3_S)
#define RMT_REF_CNT_RST_CH3_V  0x00000001U
#define RMT_REF_CNT_RST_CH3_S  3
/** RMT_REF_CNT_RST_CH4 : WT; bitpos: [4]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL4.
 */
#define RMT_REF_CNT_RST_CH4    (BIT(4))
#define RMT_REF_CNT_RST_CH4_M  (RMT_REF_CNT_RST_CH4_V << RMT_REF_CNT_RST_CH4_S)
#define RMT_REF_CNT_RST_CH4_V  0x00000001U
#define RMT_REF_CNT_RST_CH4_S  4
/** RMT_REF_CNT_RST_CH5 : WT; bitpos: [5]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL5.
 */
#define RMT_REF_CNT_RST_CH5    (BIT(5))
#define RMT_REF_CNT_RST_CH5_M  (RMT_REF_CNT_RST_CH5_V << RMT_REF_CNT_RST_CH5_S)
#define RMT_REF_CNT_RST_CH5_V  0x00000001U
#define RMT_REF_CNT_RST_CH5_S  5
/** RMT_REF_CNT_RST_CH6 : WT; bitpos: [6]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL6.
 */
#define RMT_REF_CNT_RST_CH6    (BIT(6))
#define RMT_REF_CNT_RST_CH6_M  (RMT_REF_CNT_RST_CH6_V << RMT_REF_CNT_RST_CH6_S)
#define RMT_REF_CNT_RST_CH6_V  0x00000001U
#define RMT_REF_CNT_RST_CH6_S  6
/** RMT_REF_CNT_RST_CH7 : WT; bitpos: [7]; default: 0;
 *  This register is used to reset the clock divider of CHANNEL7.
 */
#define RMT_REF_CNT_RST_CH7    (BIT(7))
#define RMT_REF_CNT_RST_CH7_M  (RMT_REF_CNT_RST_CH7_V << RMT_REF_CNT_RST_CH7_S)
#define RMT_REF_CNT_RST_CH7_V  0x00000001U
#define RMT_REF_CNT_RST_CH7_S  7

/** RMT_DATE_REG register
 *  RMT version register
 */
#define RMT_DATE_REG (DR_REG_RMT_BASE + 0xcc)
/** RMT_DATE : R/W; bitpos: [27:0]; default: 34607489;
 *  This is the version register.
 */
#define RMT_DATE    0x0FFFFFFFU
#define RMT_DATE_M  (RMT_DATE_V << RMT_DATE_S)
#define RMT_DATE_V  0x0FFFFFFFU
#define RMT_DATE_S  0

#ifdef __cplusplus
}
#endif
