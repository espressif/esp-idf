/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
/** RMT_CH0DATA : HRO; bitpos: [31:0]; default: 0;
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
/** RMT_CH1DATA : HRO; bitpos: [31:0]; default: 0;
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
/** RMT_CH2DATA : HRO; bitpos: [31:0]; default: 0;
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
/** RMT_CH3DATA : HRO; bitpos: [31:0]; default: 0;
 *  Read and write data for channel 3 via APB FIFO.
 */
#define RMT_CH3DATA    0xFFFFFFFFU
#define RMT_CH3DATA_M  (RMT_CH3DATA_V << RMT_CH3DATA_S)
#define RMT_CH3DATA_V  0xFFFFFFFFU
#define RMT_CH3DATA_S  0

/** RMT_CH0CONF0_REG register
 *  Channel 0 configure register 0
 */
#define RMT_CH0CONF0_REG (DR_REG_RMT_BASE + 0x10)
/** RMT_TX_START_CH0 : WT; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on CHANNEL0.
 */
#define RMT_TX_START_CH0    (BIT(0))
#define RMT_TX_START_CH0_M  (RMT_TX_START_CH0_V << RMT_TX_START_CH0_S)
#define RMT_TX_START_CH0_V  0x00000001U
#define RMT_TX_START_CH0_S  0
/** RMT_MEM_RD_RST_CH0 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset read ram address for CHANNEL0 by accessing transmitter.
 */
#define RMT_MEM_RD_RST_CH0    (BIT(1))
#define RMT_MEM_RD_RST_CH0_M  (RMT_MEM_RD_RST_CH0_V << RMT_MEM_RD_RST_CH0_S)
#define RMT_MEM_RD_RST_CH0_V  0x00000001U
#define RMT_MEM_RD_RST_CH0_S  1
/** RMT_APB_MEM_RST_CH0 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL0 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH0    (BIT(2))
#define RMT_APB_MEM_RST_CH0_M  (RMT_APB_MEM_RST_CH0_V << RMT_APB_MEM_RST_CH0_S)
#define RMT_APB_MEM_RST_CH0_V  0x00000001U
#define RMT_APB_MEM_RST_CH0_S  2
/** RMT_TX_CONTI_MODE_CH0 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to restart transmission  from the first data to the last data in
 *  CHANNEL0.
 */
#define RMT_TX_CONTI_MODE_CH0    (BIT(3))
#define RMT_TX_CONTI_MODE_CH0_M  (RMT_TX_CONTI_MODE_CH0_V << RMT_TX_CONTI_MODE_CH0_S)
#define RMT_TX_CONTI_MODE_CH0_V  0x00000001U
#define RMT_TX_CONTI_MODE_CH0_S  3
/** RMT_MEM_TX_WRAP_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  This is the channel 0 enable bit for wraparound mode: it will resume sending at the
 *  start when the data to be sent is more than its memory size.
 */
#define RMT_MEM_TX_WRAP_EN_CH0    (BIT(4))
#define RMT_MEM_TX_WRAP_EN_CH0_M  (RMT_MEM_TX_WRAP_EN_CH0_V << RMT_MEM_TX_WRAP_EN_CH0_S)
#define RMT_MEM_TX_WRAP_EN_CH0_V  0x00000001U
#define RMT_MEM_TX_WRAP_EN_CH0_S  4
/** RMT_IDLE_OUT_LV_CH0 : R/W; bitpos: [5]; default: 0;
 *  This bit configures the level of output signal in CHANNEL0 when the latter is in
 *  IDLE state.
 */
#define RMT_IDLE_OUT_LV_CH0    (BIT(5))
#define RMT_IDLE_OUT_LV_CH0_M  (RMT_IDLE_OUT_LV_CH0_V << RMT_IDLE_OUT_LV_CH0_S)
#define RMT_IDLE_OUT_LV_CH0_V  0x00000001U
#define RMT_IDLE_OUT_LV_CH0_S  5
/** RMT_IDLE_OUT_EN_CH0 : R/W; bitpos: [6]; default: 0;
 *  This is the output enable-control bit for CHANNEL0 in IDLE state.
 */
#define RMT_IDLE_OUT_EN_CH0    (BIT(6))
#define RMT_IDLE_OUT_EN_CH0_M  (RMT_IDLE_OUT_EN_CH0_V << RMT_IDLE_OUT_EN_CH0_S)
#define RMT_IDLE_OUT_EN_CH0_V  0x00000001U
#define RMT_IDLE_OUT_EN_CH0_S  6
/** RMT_TX_STOP_CH0 : R/W/SC; bitpos: [7]; default: 0;
 *  Set this bit to stop the transmitter of CHANNEL0 sending data out.
 */
#define RMT_TX_STOP_CH0    (BIT(7))
#define RMT_TX_STOP_CH0_M  (RMT_TX_STOP_CH0_V << RMT_TX_STOP_CH0_S)
#define RMT_TX_STOP_CH0_V  0x00000001U
#define RMT_TX_STOP_CH0_S  7
/** RMT_DIV_CNT_CH0 : R/W; bitpos: [15:8]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL0.
 */
#define RMT_DIV_CNT_CH0    0x000000FFU
#define RMT_DIV_CNT_CH0_M  (RMT_DIV_CNT_CH0_V << RMT_DIV_CNT_CH0_S)
#define RMT_DIV_CNT_CH0_V  0x000000FFU
#define RMT_DIV_CNT_CH0_S  8
/** RMT_MEM_SIZE_CH0 : R/W; bitpos: [18:16]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL0.
 */
#define RMT_MEM_SIZE_CH0    0x00000007U
#define RMT_MEM_SIZE_CH0_M  (RMT_MEM_SIZE_CH0_V << RMT_MEM_SIZE_CH0_S)
#define RMT_MEM_SIZE_CH0_V  0x00000007U
#define RMT_MEM_SIZE_CH0_S  16
/** RMT_CARRIER_EFF_EN_CH0 : R/W; bitpos: [20]; default: 1;
 *  1: Add carrier modulation on the output signal only at the send data state for
 *  CHANNEL0. 0: Add carrier modulation on the output signal at all state for CHANNEL0.
 *  Only valid when RMT_CARRIER_EN_CH0 is 1.
 */
#define RMT_CARRIER_EFF_EN_CH0    (BIT(20))
#define RMT_CARRIER_EFF_EN_CH0_M  (RMT_CARRIER_EFF_EN_CH0_V << RMT_CARRIER_EFF_EN_CH0_S)
#define RMT_CARRIER_EFF_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EFF_EN_CH0_S  20
/** RMT_CARRIER_EN_CH0 : R/W; bitpos: [21]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL0. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_CH0    (BIT(21))
#define RMT_CARRIER_EN_CH0_M  (RMT_CARRIER_EN_CH0_V << RMT_CARRIER_EN_CH0_S)
#define RMT_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EN_CH0_S  21
/** RMT_CARRIER_OUT_LV_CH0 : R/W; bitpos: [22]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL0.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_CH0    (BIT(22))
#define RMT_CARRIER_OUT_LV_CH0_M  (RMT_CARRIER_OUT_LV_CH0_V << RMT_CARRIER_OUT_LV_CH0_S)
#define RMT_CARRIER_OUT_LV_CH0_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH0_S  22
/** RMT_AFIFO_RST_CH0 : WT; bitpos: [23]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_CH0    (BIT(23))
#define RMT_AFIFO_RST_CH0_M  (RMT_AFIFO_RST_CH0_V << RMT_AFIFO_RST_CH0_S)
#define RMT_AFIFO_RST_CH0_V  0x00000001U
#define RMT_AFIFO_RST_CH0_S  23
/** RMT_CONF_UPDATE_CH0 : WT; bitpos: [24]; default: 0;
 *  synchronization bit for CHANNEL0
 */
#define RMT_CONF_UPDATE_CH0    (BIT(24))
#define RMT_CONF_UPDATE_CH0_M  (RMT_CONF_UPDATE_CH0_V << RMT_CONF_UPDATE_CH0_S)
#define RMT_CONF_UPDATE_CH0_V  0x00000001U
#define RMT_CONF_UPDATE_CH0_S  24

/** RMT_CH1CONF0_REG register
 *  Channel 1 configure register 0
 */
#define RMT_CH1CONF0_REG (DR_REG_RMT_BASE + 0x14)
/** RMT_TX_START_CH1 : WT; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on CHANNEL1.
 */
#define RMT_TX_START_CH1    (BIT(0))
#define RMT_TX_START_CH1_M  (RMT_TX_START_CH1_V << RMT_TX_START_CH1_S)
#define RMT_TX_START_CH1_V  0x00000001U
#define RMT_TX_START_CH1_S  0
/** RMT_MEM_RD_RST_CH1 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset read ram address for CHANNEL1 by accessing transmitter.
 */
#define RMT_MEM_RD_RST_CH1    (BIT(1))
#define RMT_MEM_RD_RST_CH1_M  (RMT_MEM_RD_RST_CH1_V << RMT_MEM_RD_RST_CH1_S)
#define RMT_MEM_RD_RST_CH1_V  0x00000001U
#define RMT_MEM_RD_RST_CH1_S  1
/** RMT_APB_MEM_RST_CH1 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL1 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH1    (BIT(2))
#define RMT_APB_MEM_RST_CH1_M  (RMT_APB_MEM_RST_CH1_V << RMT_APB_MEM_RST_CH1_S)
#define RMT_APB_MEM_RST_CH1_V  0x00000001U
#define RMT_APB_MEM_RST_CH1_S  2
/** RMT_TX_CONTI_MODE_CH1 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to restart transmission  from the first data to the last data in
 *  CHANNEL1.
 */
#define RMT_TX_CONTI_MODE_CH1    (BIT(3))
#define RMT_TX_CONTI_MODE_CH1_M  (RMT_TX_CONTI_MODE_CH1_V << RMT_TX_CONTI_MODE_CH1_S)
#define RMT_TX_CONTI_MODE_CH1_V  0x00000001U
#define RMT_TX_CONTI_MODE_CH1_S  3
/** RMT_MEM_TX_WRAP_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  This is the channel 1 enable bit for wraparound mode: it will resume sending at the
 *  start when the data to be sent is more than its memory size.
 */
#define RMT_MEM_TX_WRAP_EN_CH1    (BIT(4))
#define RMT_MEM_TX_WRAP_EN_CH1_M  (RMT_MEM_TX_WRAP_EN_CH1_V << RMT_MEM_TX_WRAP_EN_CH1_S)
#define RMT_MEM_TX_WRAP_EN_CH1_V  0x00000001U
#define RMT_MEM_TX_WRAP_EN_CH1_S  4
/** RMT_IDLE_OUT_LV_CH1 : R/W; bitpos: [5]; default: 0;
 *  This bit configures the level of output signal in CHANNEL1 when the latter is in
 *  IDLE state.
 */
#define RMT_IDLE_OUT_LV_CH1    (BIT(5))
#define RMT_IDLE_OUT_LV_CH1_M  (RMT_IDLE_OUT_LV_CH1_V << RMT_IDLE_OUT_LV_CH1_S)
#define RMT_IDLE_OUT_LV_CH1_V  0x00000001U
#define RMT_IDLE_OUT_LV_CH1_S  5
/** RMT_IDLE_OUT_EN_CH1 : R/W; bitpos: [6]; default: 0;
 *  This is the output enable-control bit for CHANNEL1 in IDLE state.
 */
#define RMT_IDLE_OUT_EN_CH1    (BIT(6))
#define RMT_IDLE_OUT_EN_CH1_M  (RMT_IDLE_OUT_EN_CH1_V << RMT_IDLE_OUT_EN_CH1_S)
#define RMT_IDLE_OUT_EN_CH1_V  0x00000001U
#define RMT_IDLE_OUT_EN_CH1_S  6
/** RMT_TX_STOP_CH1 : R/W/SC; bitpos: [7]; default: 0;
 *  Set this bit to stop the transmitter of CHANNEL1 sending data out.
 */
#define RMT_TX_STOP_CH1    (BIT(7))
#define RMT_TX_STOP_CH1_M  (RMT_TX_STOP_CH1_V << RMT_TX_STOP_CH1_S)
#define RMT_TX_STOP_CH1_V  0x00000001U
#define RMT_TX_STOP_CH1_S  7
/** RMT_DIV_CNT_CH1 : R/W; bitpos: [15:8]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL1.
 */
#define RMT_DIV_CNT_CH1    0x000000FFU
#define RMT_DIV_CNT_CH1_M  (RMT_DIV_CNT_CH1_V << RMT_DIV_CNT_CH1_S)
#define RMT_DIV_CNT_CH1_V  0x000000FFU
#define RMT_DIV_CNT_CH1_S  8
/** RMT_MEM_SIZE_CH1 : R/W; bitpos: [18:16]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL1.
 */
#define RMT_MEM_SIZE_CH1    0x00000007U
#define RMT_MEM_SIZE_CH1_M  (RMT_MEM_SIZE_CH1_V << RMT_MEM_SIZE_CH1_S)
#define RMT_MEM_SIZE_CH1_V  0x00000007U
#define RMT_MEM_SIZE_CH1_S  16
/** RMT_CARRIER_EFF_EN_CH1 : R/W; bitpos: [20]; default: 1;
 *  1: Add carrier modulation on the output signal only at the send data state for
 *  CHANNEL1. 0: Add carrier modulation on the output signal at all state for CHANNEL1.
 *  Only valid when RMT_CARRIER_EN_CH1 is 1.
 */
#define RMT_CARRIER_EFF_EN_CH1    (BIT(20))
#define RMT_CARRIER_EFF_EN_CH1_M  (RMT_CARRIER_EFF_EN_CH1_V << RMT_CARRIER_EFF_EN_CH1_S)
#define RMT_CARRIER_EFF_EN_CH1_V  0x00000001U
#define RMT_CARRIER_EFF_EN_CH1_S  20
/** RMT_CARRIER_EN_CH1 : R/W; bitpos: [21]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL1. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_CH1    (BIT(21))
#define RMT_CARRIER_EN_CH1_M  (RMT_CARRIER_EN_CH1_V << RMT_CARRIER_EN_CH1_S)
#define RMT_CARRIER_EN_CH1_V  0x00000001U
#define RMT_CARRIER_EN_CH1_S  21
/** RMT_CARRIER_OUT_LV_CH1 : R/W; bitpos: [22]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL1.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_CH1    (BIT(22))
#define RMT_CARRIER_OUT_LV_CH1_M  (RMT_CARRIER_OUT_LV_CH1_V << RMT_CARRIER_OUT_LV_CH1_S)
#define RMT_CARRIER_OUT_LV_CH1_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH1_S  22
/** RMT_AFIFO_RST_CH1 : WT; bitpos: [23]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_CH1    (BIT(23))
#define RMT_AFIFO_RST_CH1_M  (RMT_AFIFO_RST_CH1_V << RMT_AFIFO_RST_CH1_S)
#define RMT_AFIFO_RST_CH1_V  0x00000001U
#define RMT_AFIFO_RST_CH1_S  23
/** RMT_CONF_UPDATE_CH1 : WT; bitpos: [24]; default: 0;
 *  synchronization bit for CHANNEL1
 */
#define RMT_CONF_UPDATE_CH1    (BIT(24))
#define RMT_CONF_UPDATE_CH1_M  (RMT_CONF_UPDATE_CH1_V << RMT_CONF_UPDATE_CH1_S)
#define RMT_CONF_UPDATE_CH1_V  0x00000001U
#define RMT_CONF_UPDATE_CH1_S  24

/** RMT_CH2CONF0_REG register
 *  Channel 2 configure register 0
 */
#define RMT_CH2CONF0_REG (DR_REG_RMT_BASE + 0x18)
/** RMT_DIV_CNT_CH2 : R/W; bitpos: [7:0]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL2.
 */
#define RMT_DIV_CNT_CH2    0x000000FFU
#define RMT_DIV_CNT_CH2_M  (RMT_DIV_CNT_CH2_V << RMT_DIV_CNT_CH2_S)
#define RMT_DIV_CNT_CH2_V  0x000000FFU
#define RMT_DIV_CNT_CH2_S  0
/** RMT_IDLE_THRES_CH2 : R/W; bitpos: [22:8]; default: 32767;
 *  When no edge is detected on the input signal and continuous clock cycles is longer
 *  than this register value, received process is finished.
 */
#define RMT_IDLE_THRES_CH2    0x00007FFFU
#define RMT_IDLE_THRES_CH2_M  (RMT_IDLE_THRES_CH2_V << RMT_IDLE_THRES_CH2_S)
#define RMT_IDLE_THRES_CH2_V  0x00007FFFU
#define RMT_IDLE_THRES_CH2_S  8
/** RMT_MEM_SIZE_CH2 : R/W; bitpos: [25:23]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL2.
 */
#define RMT_MEM_SIZE_CH2    0x00000007U
#define RMT_MEM_SIZE_CH2_M  (RMT_MEM_SIZE_CH2_V << RMT_MEM_SIZE_CH2_S)
#define RMT_MEM_SIZE_CH2_V  0x00000007U
#define RMT_MEM_SIZE_CH2_S  23
/** RMT_CARRIER_EN_CH2 : R/W; bitpos: [28]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL2. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_CH2    (BIT(28))
#define RMT_CARRIER_EN_CH2_M  (RMT_CARRIER_EN_CH2_V << RMT_CARRIER_EN_CH2_S)
#define RMT_CARRIER_EN_CH2_V  0x00000001U
#define RMT_CARRIER_EN_CH2_S  28
/** RMT_CARRIER_OUT_LV_CH2 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL2.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_CH2    (BIT(29))
#define RMT_CARRIER_OUT_LV_CH2_M  (RMT_CARRIER_OUT_LV_CH2_V << RMT_CARRIER_OUT_LV_CH2_S)
#define RMT_CARRIER_OUT_LV_CH2_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH2_S  29

/** RMT_CH2CONF1_REG register
 *  Channel 2 configure register 1
 */
#define RMT_CH2CONF1_REG (DR_REG_RMT_BASE + 0x1c)
/** RMT_RX_EN_CH2 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable receiver to receive data on CHANNEL2.
 */
#define RMT_RX_EN_CH2    (BIT(0))
#define RMT_RX_EN_CH2_M  (RMT_RX_EN_CH2_V << RMT_RX_EN_CH2_S)
#define RMT_RX_EN_CH2_V  0x00000001U
#define RMT_RX_EN_CH2_S  0
/** RMT_MEM_WR_RST_CH2 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset write ram address for CHANNEL2 by accessing receiver.
 */
#define RMT_MEM_WR_RST_CH2    (BIT(1))
#define RMT_MEM_WR_RST_CH2_M  (RMT_MEM_WR_RST_CH2_V << RMT_MEM_WR_RST_CH2_S)
#define RMT_MEM_WR_RST_CH2_V  0x00000001U
#define RMT_MEM_WR_RST_CH2_S  1
/** RMT_APB_MEM_RST_CH2 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL2 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH2    (BIT(2))
#define RMT_APB_MEM_RST_CH2_M  (RMT_APB_MEM_RST_CH2_V << RMT_APB_MEM_RST_CH2_S)
#define RMT_APB_MEM_RST_CH2_V  0x00000001U
#define RMT_APB_MEM_RST_CH2_S  2
/** RMT_MEM_OWNER_CH2 : R/W/SC; bitpos: [3]; default: 1;
 *  This register marks the ownership of CHANNEL2's ram block.
 *
 *  1'h1: Receiver is using the ram.
 *
 *  1'h0: APB bus is using the ram.
 */
#define RMT_MEM_OWNER_CH2    (BIT(3))
#define RMT_MEM_OWNER_CH2_M  (RMT_MEM_OWNER_CH2_V << RMT_MEM_OWNER_CH2_S)
#define RMT_MEM_OWNER_CH2_V  0x00000001U
#define RMT_MEM_OWNER_CH2_S  3
/** RMT_RX_FILTER_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  This is the receive filter's enable bit for CHANNEL2.
 */
#define RMT_RX_FILTER_EN_CH2    (BIT(4))
#define RMT_RX_FILTER_EN_CH2_M  (RMT_RX_FILTER_EN_CH2_V << RMT_RX_FILTER_EN_CH2_S)
#define RMT_RX_FILTER_EN_CH2_V  0x00000001U
#define RMT_RX_FILTER_EN_CH2_S  4
/** RMT_RX_FILTER_THRES_CH2 : R/W; bitpos: [12:5]; default: 15;
 *  Ignores the input pulse when its width is smaller than this register value in APB
 *  clock periods (in receive mode).
 */
#define RMT_RX_FILTER_THRES_CH2    0x000000FFU
#define RMT_RX_FILTER_THRES_CH2_M  (RMT_RX_FILTER_THRES_CH2_V << RMT_RX_FILTER_THRES_CH2_S)
#define RMT_RX_FILTER_THRES_CH2_V  0x000000FFU
#define RMT_RX_FILTER_THRES_CH2_S  5
/** RMT_MEM_RX_WRAP_EN_CH2 : R/W; bitpos: [13]; default: 0;
 *  This is the channel 2 enable bit for wraparound mode: it will resume receiving at
 *  the start when the data to be received is more than its memory size.
 */
#define RMT_MEM_RX_WRAP_EN_CH2    (BIT(13))
#define RMT_MEM_RX_WRAP_EN_CH2_M  (RMT_MEM_RX_WRAP_EN_CH2_V << RMT_MEM_RX_WRAP_EN_CH2_S)
#define RMT_MEM_RX_WRAP_EN_CH2_V  0x00000001U
#define RMT_MEM_RX_WRAP_EN_CH2_S  13
/** RMT_AFIFO_RST_CH2 : WT; bitpos: [14]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_CH2    (BIT(14))
#define RMT_AFIFO_RST_CH2_M  (RMT_AFIFO_RST_CH2_V << RMT_AFIFO_RST_CH2_S)
#define RMT_AFIFO_RST_CH2_V  0x00000001U
#define RMT_AFIFO_RST_CH2_S  14
/** RMT_CONF_UPDATE_CH2 : WT; bitpos: [15]; default: 0;
 *  synchronization bit for CHANNEL2
 */
#define RMT_CONF_UPDATE_CH2    (BIT(15))
#define RMT_CONF_UPDATE_CH2_M  (RMT_CONF_UPDATE_CH2_V << RMT_CONF_UPDATE_CH2_S)
#define RMT_CONF_UPDATE_CH2_V  0x00000001U
#define RMT_CONF_UPDATE_CH2_S  15

/** RMT_CH3CONF0_REG register
 *  Channel 3 configure register 0
 */
#define RMT_CH3CONF0_REG (DR_REG_RMT_BASE + 0x20)
/** RMT_DIV_CNT_CH3 : R/W; bitpos: [7:0]; default: 2;
 *  This register is used to configure the divider for clock of CHANNEL3.
 */
#define RMT_DIV_CNT_CH3    0x000000FFU
#define RMT_DIV_CNT_CH3_M  (RMT_DIV_CNT_CH3_V << RMT_DIV_CNT_CH3_S)
#define RMT_DIV_CNT_CH3_V  0x000000FFU
#define RMT_DIV_CNT_CH3_S  0
/** RMT_IDLE_THRES_CH3 : R/W; bitpos: [22:8]; default: 32767;
 *  When no edge is detected on the input signal and continuous clock cycles is longer
 *  than this register value, received process is finished.
 */
#define RMT_IDLE_THRES_CH3    0x00007FFFU
#define RMT_IDLE_THRES_CH3_M  (RMT_IDLE_THRES_CH3_V << RMT_IDLE_THRES_CH3_S)
#define RMT_IDLE_THRES_CH3_V  0x00007FFFU
#define RMT_IDLE_THRES_CH3_S  8
/** RMT_MEM_SIZE_CH3 : R/W; bitpos: [25:23]; default: 1;
 *  This register is used to configure the maximum size of memory allocated to CHANNEL3.
 */
#define RMT_MEM_SIZE_CH3    0x00000007U
#define RMT_MEM_SIZE_CH3_M  (RMT_MEM_SIZE_CH3_V << RMT_MEM_SIZE_CH3_S)
#define RMT_MEM_SIZE_CH3_V  0x00000007U
#define RMT_MEM_SIZE_CH3_S  23
/** RMT_CARRIER_EN_CH3 : R/W; bitpos: [28]; default: 1;
 *  This is the carrier modulation enable-control bit for CHANNEL3. 1: Add carrier
 *  modulation in the output signal. 0: No carrier modulation in sig_out.
 */
#define RMT_CARRIER_EN_CH3    (BIT(28))
#define RMT_CARRIER_EN_CH3_M  (RMT_CARRIER_EN_CH3_V << RMT_CARRIER_EN_CH3_S)
#define RMT_CARRIER_EN_CH3_V  0x00000001U
#define RMT_CARRIER_EN_CH3_S  28
/** RMT_CARRIER_OUT_LV_CH3 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for CHANNEL3.
 *
 *  1'h0: add carrier wave on low level.
 *
 *  1'h1: add carrier wave on high level.
 */
#define RMT_CARRIER_OUT_LV_CH3    (BIT(29))
#define RMT_CARRIER_OUT_LV_CH3_M  (RMT_CARRIER_OUT_LV_CH3_V << RMT_CARRIER_OUT_LV_CH3_S)
#define RMT_CARRIER_OUT_LV_CH3_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH3_S  29

/** RMT_CH3CONF1_REG register
 *  Channel 3 configure register 1
 */
#define RMT_CH3CONF1_REG (DR_REG_RMT_BASE + 0x24)
/** RMT_RX_EN_CH3 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable receiver to receive data on CHANNEL3.
 */
#define RMT_RX_EN_CH3    (BIT(0))
#define RMT_RX_EN_CH3_M  (RMT_RX_EN_CH3_V << RMT_RX_EN_CH3_S)
#define RMT_RX_EN_CH3_V  0x00000001U
#define RMT_RX_EN_CH3_S  0
/** RMT_MEM_WR_RST_CH3 : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset write ram address for CHANNEL3 by accessing receiver.
 */
#define RMT_MEM_WR_RST_CH3    (BIT(1))
#define RMT_MEM_WR_RST_CH3_M  (RMT_MEM_WR_RST_CH3_V << RMT_MEM_WR_RST_CH3_S)
#define RMT_MEM_WR_RST_CH3_V  0x00000001U
#define RMT_MEM_WR_RST_CH3_S  1
/** RMT_APB_MEM_RST_CH3 : WT; bitpos: [2]; default: 0;
 *  Set this bit to reset W/R ram address for CHANNEL3 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH3    (BIT(2))
#define RMT_APB_MEM_RST_CH3_M  (RMT_APB_MEM_RST_CH3_V << RMT_APB_MEM_RST_CH3_S)
#define RMT_APB_MEM_RST_CH3_V  0x00000001U
#define RMT_APB_MEM_RST_CH3_S  2
/** RMT_MEM_OWNER_CH3 : R/W/SC; bitpos: [3]; default: 1;
 *  This register marks the ownership of CHANNEL3's ram block.
 *
 *  1'h1: Receiver is using the ram.
 *
 *  1'h0: APB bus is using the ram.
 */
#define RMT_MEM_OWNER_CH3    (BIT(3))
#define RMT_MEM_OWNER_CH3_M  (RMT_MEM_OWNER_CH3_V << RMT_MEM_OWNER_CH3_S)
#define RMT_MEM_OWNER_CH3_V  0x00000001U
#define RMT_MEM_OWNER_CH3_S  3
/** RMT_RX_FILTER_EN_CH3 : R/W; bitpos: [4]; default: 0;
 *  This is the receive filter's enable bit for CHANNEL3.
 */
#define RMT_RX_FILTER_EN_CH3    (BIT(4))
#define RMT_RX_FILTER_EN_CH3_M  (RMT_RX_FILTER_EN_CH3_V << RMT_RX_FILTER_EN_CH3_S)
#define RMT_RX_FILTER_EN_CH3_V  0x00000001U
#define RMT_RX_FILTER_EN_CH3_S  4
/** RMT_RX_FILTER_THRES_CH3 : R/W; bitpos: [12:5]; default: 15;
 *  Ignores the input pulse when its width is smaller than this register value in APB
 *  clock periods (in receive mode).
 */
#define RMT_RX_FILTER_THRES_CH3    0x000000FFU
#define RMT_RX_FILTER_THRES_CH3_M  (RMT_RX_FILTER_THRES_CH3_V << RMT_RX_FILTER_THRES_CH3_S)
#define RMT_RX_FILTER_THRES_CH3_V  0x000000FFU
#define RMT_RX_FILTER_THRES_CH3_S  5
/** RMT_MEM_RX_WRAP_EN_CH3 : R/W; bitpos: [13]; default: 0;
 *  This is the channel 3 enable bit for wraparound mode: it will resume receiving at
 *  the start when the data to be received is more than its memory size.
 */
#define RMT_MEM_RX_WRAP_EN_CH3    (BIT(13))
#define RMT_MEM_RX_WRAP_EN_CH3_M  (RMT_MEM_RX_WRAP_EN_CH3_V << RMT_MEM_RX_WRAP_EN_CH3_S)
#define RMT_MEM_RX_WRAP_EN_CH3_V  0x00000001U
#define RMT_MEM_RX_WRAP_EN_CH3_S  13
/** RMT_AFIFO_RST_CH3 : WT; bitpos: [14]; default: 0;
 *  Reserved
 */
#define RMT_AFIFO_RST_CH3    (BIT(14))
#define RMT_AFIFO_RST_CH3_M  (RMT_AFIFO_RST_CH3_V << RMT_AFIFO_RST_CH3_S)
#define RMT_AFIFO_RST_CH3_V  0x00000001U
#define RMT_AFIFO_RST_CH3_S  14
/** RMT_CONF_UPDATE_CH3 : WT; bitpos: [15]; default: 0;
 *  synchronization bit for CHANNEL3
 */
#define RMT_CONF_UPDATE_CH3    (BIT(15))
#define RMT_CONF_UPDATE_CH3_M  (RMT_CONF_UPDATE_CH3_V << RMT_CONF_UPDATE_CH3_S)
#define RMT_CONF_UPDATE_CH3_V  0x00000001U
#define RMT_CONF_UPDATE_CH3_S  15

/** RMT_CH0STATUS_REG register
 *  Channel 0 status register
 */
#define RMT_CH0STATUS_REG (DR_REG_RMT_BASE + 0x28)
/** RMT_MEM_RADDR_EX_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This register records the memory address offset when transmitter of CHANNEL0 is
 *  using the RAM.
 */
#define RMT_MEM_RADDR_EX_CH0    0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_M  (RMT_MEM_RADDR_EX_CH0_V << RMT_MEM_RADDR_EX_CH0_S)
#define RMT_MEM_RADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_S  0
/** RMT_STATE_CH0 : RO; bitpos: [11:9]; default: 0;
 *  This register records the FSM status of CHANNEL0.
 */
#define RMT_STATE_CH0    0x00000007U
#define RMT_STATE_CH0_M  (RMT_STATE_CH0_V << RMT_STATE_CH0_S)
#define RMT_STATE_CH0_V  0x00000007U
#define RMT_STATE_CH0_S  9
/** RMT_APB_MEM_WADDR_CH0 : RO; bitpos: [20:12]; default: 0;
 *  This register records the memory address offset when writes RAM over APB bus.
 */
#define RMT_APB_MEM_WADDR_CH0    0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_M  (RMT_APB_MEM_WADDR_CH0_V << RMT_APB_MEM_WADDR_CH0_S)
#define RMT_APB_MEM_WADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_S  12
/** RMT_APB_MEM_RD_ERR_CH0 : RO; bitpos: [21]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reading
 *  via APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH0    (BIT(21))
#define RMT_APB_MEM_RD_ERR_CH0_M  (RMT_APB_MEM_RD_ERR_CH0_V << RMT_APB_MEM_RD_ERR_CH0_S)
#define RMT_APB_MEM_RD_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH0_S  21
/** RMT_MEM_EMPTY_CH0 : RO; bitpos: [22]; default: 0;
 *  This status bit will be set when the data to be set is more than memory size and
 *  the wraparound mode is disabled.
 */
#define RMT_MEM_EMPTY_CH0    (BIT(22))
#define RMT_MEM_EMPTY_CH0_M  (RMT_MEM_EMPTY_CH0_V << RMT_MEM_EMPTY_CH0_S)
#define RMT_MEM_EMPTY_CH0_V  0x00000001U
#define RMT_MEM_EMPTY_CH0_S  22
/** RMT_APB_MEM_WR_ERR_CH0 : RO; bitpos: [23]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_CH0    (BIT(23))
#define RMT_APB_MEM_WR_ERR_CH0_M  (RMT_APB_MEM_WR_ERR_CH0_V << RMT_APB_MEM_WR_ERR_CH0_S)
#define RMT_APB_MEM_WR_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_CH0_S  23
/** RMT_APB_MEM_RADDR_CH0 : RO; bitpos: [31:24]; default: 0;
 *  This register records the memory address offset when reading RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_CH0    0x000000FFU
#define RMT_APB_MEM_RADDR_CH0_M  (RMT_APB_MEM_RADDR_CH0_V << RMT_APB_MEM_RADDR_CH0_S)
#define RMT_APB_MEM_RADDR_CH0_V  0x000000FFU
#define RMT_APB_MEM_RADDR_CH0_S  24

/** RMT_CH1STATUS_REG register
 *  Channel 1 status register
 */
#define RMT_CH1STATUS_REG (DR_REG_RMT_BASE + 0x2c)
/** RMT_MEM_RADDR_EX_CH1 : RO; bitpos: [8:0]; default: 0;
 *  This register records the memory address offset when transmitter of CHANNEL1 is
 *  using the RAM.
 */
#define RMT_MEM_RADDR_EX_CH1    0x000001FFU
#define RMT_MEM_RADDR_EX_CH1_M  (RMT_MEM_RADDR_EX_CH1_V << RMT_MEM_RADDR_EX_CH1_S)
#define RMT_MEM_RADDR_EX_CH1_V  0x000001FFU
#define RMT_MEM_RADDR_EX_CH1_S  0
/** RMT_STATE_CH1 : RO; bitpos: [11:9]; default: 0;
 *  This register records the FSM status of CHANNEL1.
 */
#define RMT_STATE_CH1    0x00000007U
#define RMT_STATE_CH1_M  (RMT_STATE_CH1_V << RMT_STATE_CH1_S)
#define RMT_STATE_CH1_V  0x00000007U
#define RMT_STATE_CH1_S  9
/** RMT_APB_MEM_WADDR_CH1 : RO; bitpos: [20:12]; default: 0;
 *  This register records the memory address offset when writes RAM over APB bus.
 */
#define RMT_APB_MEM_WADDR_CH1    0x000001FFU
#define RMT_APB_MEM_WADDR_CH1_M  (RMT_APB_MEM_WADDR_CH1_V << RMT_APB_MEM_WADDR_CH1_S)
#define RMT_APB_MEM_WADDR_CH1_V  0x000001FFU
#define RMT_APB_MEM_WADDR_CH1_S  12
/** RMT_APB_MEM_RD_ERR_CH1 : RO; bitpos: [21]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reading
 *  via APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH1    (BIT(21))
#define RMT_APB_MEM_RD_ERR_CH1_M  (RMT_APB_MEM_RD_ERR_CH1_V << RMT_APB_MEM_RD_ERR_CH1_S)
#define RMT_APB_MEM_RD_ERR_CH1_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH1_S  21
/** RMT_MEM_EMPTY_CH1 : RO; bitpos: [22]; default: 0;
 *  This status bit will be set when the data to be set is more than memory size and
 *  the wraparound mode is disabled.
 */
#define RMT_MEM_EMPTY_CH1    (BIT(22))
#define RMT_MEM_EMPTY_CH1_M  (RMT_MEM_EMPTY_CH1_V << RMT_MEM_EMPTY_CH1_S)
#define RMT_MEM_EMPTY_CH1_V  0x00000001U
#define RMT_MEM_EMPTY_CH1_S  22
/** RMT_APB_MEM_WR_ERR_CH1 : RO; bitpos: [23]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_CH1    (BIT(23))
#define RMT_APB_MEM_WR_ERR_CH1_M  (RMT_APB_MEM_WR_ERR_CH1_V << RMT_APB_MEM_WR_ERR_CH1_S)
#define RMT_APB_MEM_WR_ERR_CH1_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_CH1_S  23
/** RMT_APB_MEM_RADDR_CH1 : RO; bitpos: [31:24]; default: 0;
 *  This register records the memory address offset when reading RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_CH1    0x000000FFU
#define RMT_APB_MEM_RADDR_CH1_M  (RMT_APB_MEM_RADDR_CH1_V << RMT_APB_MEM_RADDR_CH1_S)
#define RMT_APB_MEM_RADDR_CH1_V  0x000000FFU
#define RMT_APB_MEM_RADDR_CH1_S  24

/** RMT_CH2STATUS_REG register
 *  Channel 2 status register
 */
#define RMT_CH2STATUS_REG (DR_REG_RMT_BASE + 0x30)
/** RMT_MEM_WADDR_EX_CH2 : RO; bitpos: [8:0]; default: 0;
 *  This register records the memory address offset when receiver of CHANNEL2 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_CH2    0x000001FFU
#define RMT_MEM_WADDR_EX_CH2_M  (RMT_MEM_WADDR_EX_CH2_V << RMT_MEM_WADDR_EX_CH2_S)
#define RMT_MEM_WADDR_EX_CH2_V  0x000001FFU
#define RMT_MEM_WADDR_EX_CH2_S  0
/** RMT_APB_MEM_RADDR_CH2 : RO; bitpos: [20:12]; default: 0;
 *  This register records the memory address offset when reads RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_CH2    0x000001FFU
#define RMT_APB_MEM_RADDR_CH2_M  (RMT_APB_MEM_RADDR_CH2_V << RMT_APB_MEM_RADDR_CH2_S)
#define RMT_APB_MEM_RADDR_CH2_V  0x000001FFU
#define RMT_APB_MEM_RADDR_CH2_S  12
/** RMT_STATE_CH2 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL2.
 */
#define RMT_STATE_CH2    0x00000007U
#define RMT_STATE_CH2_M  (RMT_STATE_CH2_V << RMT_STATE_CH2_S)
#define RMT_STATE_CH2_V  0x00000007U
#define RMT_STATE_CH2_S  22
/** RMT_MEM_OWNER_ERR_CH2 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the ownership of memory block is wrong.
 */
#define RMT_MEM_OWNER_ERR_CH2    (BIT(25))
#define RMT_MEM_OWNER_ERR_CH2_M  (RMT_MEM_OWNER_ERR_CH2_V << RMT_MEM_OWNER_ERR_CH2_S)
#define RMT_MEM_OWNER_ERR_CH2_V  0x00000001U
#define RMT_MEM_OWNER_ERR_CH2_S  25
/** RMT_MEM_FULL_CH2 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory size.
 */
#define RMT_MEM_FULL_CH2    (BIT(26))
#define RMT_MEM_FULL_CH2_M  (RMT_MEM_FULL_CH2_V << RMT_MEM_FULL_CH2_S)
#define RMT_MEM_FULL_CH2_V  0x00000001U
#define RMT_MEM_FULL_CH2_S  26
/** RMT_APB_MEM_RD_ERR_CH2 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads via
 *  APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH2    (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH2_M  (RMT_APB_MEM_RD_ERR_CH2_V << RMT_APB_MEM_RD_ERR_CH2_S)
#define RMT_APB_MEM_RD_ERR_CH2_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH2_S  27

/** RMT_CH3STATUS_REG register
 *  Channel 3 status register
 */
#define RMT_CH3STATUS_REG (DR_REG_RMT_BASE + 0x34)
/** RMT_MEM_WADDR_EX_CH3 : RO; bitpos: [8:0]; default: 0;
 *  This register records the memory address offset when receiver of CHANNEL3 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_CH3    0x000001FFU
#define RMT_MEM_WADDR_EX_CH3_M  (RMT_MEM_WADDR_EX_CH3_V << RMT_MEM_WADDR_EX_CH3_S)
#define RMT_MEM_WADDR_EX_CH3_V  0x000001FFU
#define RMT_MEM_WADDR_EX_CH3_S  0
/** RMT_APB_MEM_RADDR_CH3 : RO; bitpos: [20:12]; default: 0;
 *  This register records the memory address offset when reads RAM over APB bus.
 */
#define RMT_APB_MEM_RADDR_CH3    0x000001FFU
#define RMT_APB_MEM_RADDR_CH3_M  (RMT_APB_MEM_RADDR_CH3_V << RMT_APB_MEM_RADDR_CH3_S)
#define RMT_APB_MEM_RADDR_CH3_V  0x000001FFU
#define RMT_APB_MEM_RADDR_CH3_S  12
/** RMT_STATE_CH3 : RO; bitpos: [24:22]; default: 0;
 *  This register records the FSM status of CHANNEL3.
 */
#define RMT_STATE_CH3    0x00000007U
#define RMT_STATE_CH3_M  (RMT_STATE_CH3_V << RMT_STATE_CH3_S)
#define RMT_STATE_CH3_V  0x00000007U
#define RMT_STATE_CH3_S  22
/** RMT_MEM_OWNER_ERR_CH3 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the ownership of memory block is wrong.
 */
#define RMT_MEM_OWNER_ERR_CH3    (BIT(25))
#define RMT_MEM_OWNER_ERR_CH3_M  (RMT_MEM_OWNER_ERR_CH3_V << RMT_MEM_OWNER_ERR_CH3_S)
#define RMT_MEM_OWNER_ERR_CH3_V  0x00000001U
#define RMT_MEM_OWNER_ERR_CH3_S  25
/** RMT_MEM_FULL_CH3 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory size.
 */
#define RMT_MEM_FULL_CH3    (BIT(26))
#define RMT_MEM_FULL_CH3_M  (RMT_MEM_FULL_CH3_V << RMT_MEM_FULL_CH3_S)
#define RMT_MEM_FULL_CH3_V  0x00000001U
#define RMT_MEM_FULL_CH3_S  26
/** RMT_APB_MEM_RD_ERR_CH3 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads via
 *  APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH3    (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH3_M  (RMT_APB_MEM_RD_ERR_CH3_V << RMT_APB_MEM_RD_ERR_CH3_S)
#define RMT_APB_MEM_RD_ERR_CH3_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH3_S  27

/** RMT_INT_RAW_REG register
 *  Raw interrupt status
 */
#define RMT_INT_RAW_REG (DR_REG_RMT_BASE + 0x38)
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
/** RMT_CH2_RX_END_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The interrupt raw bit for CHANNEL2. Triggered when reception done.
 */
#define RMT_CH2_RX_END_INT_RAW    (BIT(2))
#define RMT_CH2_RX_END_INT_RAW_M  (RMT_CH2_RX_END_INT_RAW_V << RMT_CH2_RX_END_INT_RAW_S)
#define RMT_CH2_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH2_RX_END_INT_RAW_S  2
/** RMT_CH3_RX_END_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The interrupt raw bit for CHANNEL3. Triggered when reception done.
 */
#define RMT_CH3_RX_END_INT_RAW    (BIT(3))
#define RMT_CH3_RX_END_INT_RAW_M  (RMT_CH3_RX_END_INT_RAW_V << RMT_CH3_RX_END_INT_RAW_S)
#define RMT_CH3_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH3_RX_END_INT_RAW_S  3
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
/** RMT_CH2_RX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The interrupt raw bit for CHANNEL2. Triggered when receiver receive more data than
 *  configured value.
 */
#define RMT_CH2_RX_THR_EVENT_INT_RAW    (BIT(10))
#define RMT_CH2_RX_THR_EVENT_INT_RAW_M  (RMT_CH2_RX_THR_EVENT_INT_RAW_V << RMT_CH2_RX_THR_EVENT_INT_RAW_S)
#define RMT_CH2_RX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH2_RX_THR_EVENT_INT_RAW_S  10
/** RMT_CH3_RX_THR_EVENT_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The interrupt raw bit for CHANNEL3. Triggered when receiver receive more data than
 *  configured value.
 */
#define RMT_CH3_RX_THR_EVENT_INT_RAW    (BIT(11))
#define RMT_CH3_RX_THR_EVENT_INT_RAW_M  (RMT_CH3_RX_THR_EVENT_INT_RAW_V << RMT_CH3_RX_THR_EVENT_INT_RAW_S)
#define RMT_CH3_RX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH3_RX_THR_EVENT_INT_RAW_S  11
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

/** RMT_INT_ST_REG register
 *  Masked interrupt status
 */
#define RMT_INT_ST_REG (DR_REG_RMT_BASE + 0x3c)
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
/** RMT_CH2_RX_END_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit for CH2_RX_END_INT.
 */
#define RMT_CH2_RX_END_INT_ST    (BIT(2))
#define RMT_CH2_RX_END_INT_ST_M  (RMT_CH2_RX_END_INT_ST_V << RMT_CH2_RX_END_INT_ST_S)
#define RMT_CH2_RX_END_INT_ST_V  0x00000001U
#define RMT_CH2_RX_END_INT_ST_S  2
/** RMT_CH3_RX_END_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit for CH3_RX_END_INT.
 */
#define RMT_CH3_RX_END_INT_ST    (BIT(3))
#define RMT_CH3_RX_END_INT_ST_M  (RMT_CH3_RX_END_INT_ST_V << RMT_CH3_RX_END_INT_ST_S)
#define RMT_CH3_RX_END_INT_ST_V  0x00000001U
#define RMT_CH3_RX_END_INT_ST_S  3
/** RMT_CH0_ERR_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit for CH$n_ERR_INT.
 */
#define RMT_CH0_ERR_INT_ST    (BIT(4))
#define RMT_CH0_ERR_INT_ST_M  (RMT_CH0_ERR_INT_ST_V << RMT_CH0_ERR_INT_ST_S)
#define RMT_CH0_ERR_INT_ST_V  0x00000001U
#define RMT_CH0_ERR_INT_ST_S  4
/** RMT_CH1_ERR_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit for CH$n_ERR_INT.
 */
#define RMT_CH1_ERR_INT_ST    (BIT(5))
#define RMT_CH1_ERR_INT_ST_M  (RMT_CH1_ERR_INT_ST_V << RMT_CH1_ERR_INT_ST_S)
#define RMT_CH1_ERR_INT_ST_V  0x00000001U
#define RMT_CH1_ERR_INT_ST_S  5
/** RMT_CH2_ERR_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status bit for CH$n_ERR_INT.
 */
#define RMT_CH2_ERR_INT_ST    (BIT(6))
#define RMT_CH2_ERR_INT_ST_M  (RMT_CH2_ERR_INT_ST_V << RMT_CH2_ERR_INT_ST_S)
#define RMT_CH2_ERR_INT_ST_V  0x00000001U
#define RMT_CH2_ERR_INT_ST_S  6
/** RMT_CH3_ERR_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status bit for CH$n_ERR_INT.
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
/** RMT_CH2_RX_THR_EVENT_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status bit for CH2_RX_THR_EVENT_INT.
 */
#define RMT_CH2_RX_THR_EVENT_INT_ST    (BIT(10))
#define RMT_CH2_RX_THR_EVENT_INT_ST_M  (RMT_CH2_RX_THR_EVENT_INT_ST_V << RMT_CH2_RX_THR_EVENT_INT_ST_S)
#define RMT_CH2_RX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH2_RX_THR_EVENT_INT_ST_S  10
/** RMT_CH3_RX_THR_EVENT_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status bit for CH3_RX_THR_EVENT_INT.
 */
#define RMT_CH3_RX_THR_EVENT_INT_ST    (BIT(11))
#define RMT_CH3_RX_THR_EVENT_INT_ST_M  (RMT_CH3_RX_THR_EVENT_INT_ST_V << RMT_CH3_RX_THR_EVENT_INT_ST_S)
#define RMT_CH3_RX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH3_RX_THR_EVENT_INT_ST_S  11
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

/** RMT_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define RMT_INT_ENA_REG (DR_REG_RMT_BASE + 0x40)
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
/** RMT_CH2_RX_END_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for CH2_RX_END_INT.
 */
#define RMT_CH2_RX_END_INT_ENA    (BIT(2))
#define RMT_CH2_RX_END_INT_ENA_M  (RMT_CH2_RX_END_INT_ENA_V << RMT_CH2_RX_END_INT_ENA_S)
#define RMT_CH2_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH2_RX_END_INT_ENA_S  2
/** RMT_CH3_RX_END_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for CH3_RX_END_INT.
 */
#define RMT_CH3_RX_END_INT_ENA    (BIT(3))
#define RMT_CH3_RX_END_INT_ENA_M  (RMT_CH3_RX_END_INT_ENA_V << RMT_CH3_RX_END_INT_ENA_S)
#define RMT_CH3_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH3_RX_END_INT_ENA_S  3
/** RMT_CH0_ERR_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for CH$n_ERR_INT.
 */
#define RMT_CH0_ERR_INT_ENA    (BIT(4))
#define RMT_CH0_ERR_INT_ENA_M  (RMT_CH0_ERR_INT_ENA_V << RMT_CH0_ERR_INT_ENA_S)
#define RMT_CH0_ERR_INT_ENA_V  0x00000001U
#define RMT_CH0_ERR_INT_ENA_S  4
/** RMT_CH1_ERR_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for CH$n_ERR_INT.
 */
#define RMT_CH1_ERR_INT_ENA    (BIT(5))
#define RMT_CH1_ERR_INT_ENA_M  (RMT_CH1_ERR_INT_ENA_V << RMT_CH1_ERR_INT_ENA_S)
#define RMT_CH1_ERR_INT_ENA_V  0x00000001U
#define RMT_CH1_ERR_INT_ENA_S  5
/** RMT_CH2_ERR_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for CH$n_ERR_INT.
 */
#define RMT_CH2_ERR_INT_ENA    (BIT(6))
#define RMT_CH2_ERR_INT_ENA_M  (RMT_CH2_ERR_INT_ENA_V << RMT_CH2_ERR_INT_ENA_S)
#define RMT_CH2_ERR_INT_ENA_V  0x00000001U
#define RMT_CH2_ERR_INT_ENA_S  6
/** RMT_CH3_ERR_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for CH$n_ERR_INT.
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
/** RMT_CH2_RX_THR_EVENT_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for CH2_RX_THR_EVENT_INT.
 */
#define RMT_CH2_RX_THR_EVENT_INT_ENA    (BIT(10))
#define RMT_CH2_RX_THR_EVENT_INT_ENA_M  (RMT_CH2_RX_THR_EVENT_INT_ENA_V << RMT_CH2_RX_THR_EVENT_INT_ENA_S)
#define RMT_CH2_RX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH2_RX_THR_EVENT_INT_ENA_S  10
/** RMT_CH3_RX_THR_EVENT_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for CH3_RX_THR_EVENT_INT.
 */
#define RMT_CH3_RX_THR_EVENT_INT_ENA    (BIT(11))
#define RMT_CH3_RX_THR_EVENT_INT_ENA_M  (RMT_CH3_RX_THR_EVENT_INT_ENA_V << RMT_CH3_RX_THR_EVENT_INT_ENA_S)
#define RMT_CH3_RX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH3_RX_THR_EVENT_INT_ENA_S  11
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

/** RMT_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define RMT_INT_CLR_REG (DR_REG_RMT_BASE + 0x44)
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
/** RMT_CH2_RX_END_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear theCH2_RX_END_INT interrupt.
 */
#define RMT_CH2_RX_END_INT_CLR    (BIT(2))
#define RMT_CH2_RX_END_INT_CLR_M  (RMT_CH2_RX_END_INT_CLR_V << RMT_CH2_RX_END_INT_CLR_S)
#define RMT_CH2_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH2_RX_END_INT_CLR_S  2
/** RMT_CH3_RX_END_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear theCH3_RX_END_INT interrupt.
 */
#define RMT_CH3_RX_END_INT_CLR    (BIT(3))
#define RMT_CH3_RX_END_INT_CLR_M  (RMT_CH3_RX_END_INT_CLR_V << RMT_CH3_RX_END_INT_CLR_S)
#define RMT_CH3_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH3_RX_END_INT_CLR_S  3
/** RMT_CH0_ERR_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear theCH$n_ERR_INT interrupt.
 */
#define RMT_CH0_ERR_INT_CLR    (BIT(4))
#define RMT_CH0_ERR_INT_CLR_M  (RMT_CH0_ERR_INT_CLR_V << RMT_CH0_ERR_INT_CLR_S)
#define RMT_CH0_ERR_INT_CLR_V  0x00000001U
#define RMT_CH0_ERR_INT_CLR_S  4
/** RMT_CH1_ERR_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear theCH$n_ERR_INT interrupt.
 */
#define RMT_CH1_ERR_INT_CLR    (BIT(5))
#define RMT_CH1_ERR_INT_CLR_M  (RMT_CH1_ERR_INT_CLR_V << RMT_CH1_ERR_INT_CLR_S)
#define RMT_CH1_ERR_INT_CLR_V  0x00000001U
#define RMT_CH1_ERR_INT_CLR_S  5
/** RMT_CH2_ERR_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear theCH$n_ERR_INT interrupt.
 */
#define RMT_CH2_ERR_INT_CLR    (BIT(6))
#define RMT_CH2_ERR_INT_CLR_M  (RMT_CH2_ERR_INT_CLR_V << RMT_CH2_ERR_INT_CLR_S)
#define RMT_CH2_ERR_INT_CLR_V  0x00000001U
#define RMT_CH2_ERR_INT_CLR_S  6
/** RMT_CH3_ERR_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear theCH$n_ERR_INT interrupt.
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
/** RMT_CH2_RX_THR_EVENT_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear theCH2_RX_THR_EVENT_INT interrupt.
 */
#define RMT_CH2_RX_THR_EVENT_INT_CLR    (BIT(10))
#define RMT_CH2_RX_THR_EVENT_INT_CLR_M  (RMT_CH2_RX_THR_EVENT_INT_CLR_V << RMT_CH2_RX_THR_EVENT_INT_CLR_S)
#define RMT_CH2_RX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH2_RX_THR_EVENT_INT_CLR_S  10
/** RMT_CH3_RX_THR_EVENT_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear theCH3_RX_THR_EVENT_INT interrupt.
 */
#define RMT_CH3_RX_THR_EVENT_INT_CLR    (BIT(11))
#define RMT_CH3_RX_THR_EVENT_INT_CLR_M  (RMT_CH3_RX_THR_EVENT_INT_CLR_V << RMT_CH3_RX_THR_EVENT_INT_CLR_S)
#define RMT_CH3_RX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH3_RX_THR_EVENT_INT_CLR_S  11
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

/** RMT_CH0CARRIER_DUTY_REG register
 *  Channel 0 duty cycle configuration register
 */
#define RMT_CH0CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x48)
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
#define RMT_CH1CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x4c)
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

/** RMT_CH2_RX_CARRIER_RM_REG register
 *  Channel 2 carrier remove register
 */
#define RMT_CH2_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x50)
/** RMT_CARRIER_LOW_THRES_CH2 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_LOW_THRES_CH2 + 1) for channel 2.
 */
#define RMT_CARRIER_LOW_THRES_CH2    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH2_M  (RMT_CARRIER_LOW_THRES_CH2_V << RMT_CARRIER_LOW_THRES_CH2_S)
#define RMT_CARRIER_LOW_THRES_CH2_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH2_S  0
/** RMT_CARRIER_HIGH_THRES_CH2 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_HIGH_THRES_CH2 + 1) for channel 2.
 */
#define RMT_CARRIER_HIGH_THRES_CH2    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH2_M  (RMT_CARRIER_HIGH_THRES_CH2_V << RMT_CARRIER_HIGH_THRES_CH2_S)
#define RMT_CARRIER_HIGH_THRES_CH2_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH2_S  16

/** RMT_CH3_RX_CARRIER_RM_REG register
 *  Channel 3 carrier remove register
 */
#define RMT_CH3_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x54)
/** RMT_CARRIER_LOW_THRES_CH3 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_LOW_THRES_CH3 + 1) for channel 3.
 */
#define RMT_CARRIER_LOW_THRES_CH3    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH3_M  (RMT_CARRIER_LOW_THRES_CH3_V << RMT_CARRIER_LOW_THRES_CH3_S)
#define RMT_CARRIER_LOW_THRES_CH3_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH3_S  0
/** RMT_CARRIER_HIGH_THRES_CH3 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in a carrier modulation mode is
 *  (REG_RMT_REG_CARRIER_HIGH_THRES_CH3 + 1) for channel 3.
 */
#define RMT_CARRIER_HIGH_THRES_CH3    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH3_M  (RMT_CARRIER_HIGH_THRES_CH3_V << RMT_CARRIER_HIGH_THRES_CH3_S)
#define RMT_CARRIER_HIGH_THRES_CH3_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH3_S  16

/** RMT_CH0_TX_LIM_REG register
 *  Channel 0 Tx event configuration register
 */
#define RMT_CH0_TX_LIM_REG (DR_REG_RMT_BASE + 0x58)
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
#define RMT_CH1_TX_LIM_REG (DR_REG_RMT_BASE + 0x5c)
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

/** RMT_CH2_RX_LIM_REG register
 *  Channel 2 Rx event configuration register
 */
#define RMT_CH2_RX_LIM_REG (DR_REG_RMT_BASE + 0x60)
/** RMT_RMT_RX_LIM_CH2 : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL2 can receive.
 */
#define RMT_RMT_RX_LIM_CH2    0x000001FFU
#define RMT_RMT_RX_LIM_CH2_M  (RMT_RMT_RX_LIM_CH2_V << RMT_RMT_RX_LIM_CH2_S)
#define RMT_RMT_RX_LIM_CH2_V  0x000001FFU
#define RMT_RMT_RX_LIM_CH2_S  0

/** RMT_CH3_RX_LIM_REG register
 *  Channel 3 Rx event configuration register
 */
#define RMT_CH3_RX_LIM_REG (DR_REG_RMT_BASE + 0x64)
/** RMT_RMT_RX_LIM_CH2 : R/W; bitpos: [8:0]; default: 128;
 *  This register is used to configure the maximum entries that CHANNEL3 can receive.
 */
#define RMT_RMT_RX_LIM_CH2    0x000001FFU
#define RMT_RMT_RX_LIM_CH2_M  (RMT_RMT_RX_LIM_CH2_V << RMT_RMT_RX_LIM_CH2_S)
#define RMT_RMT_RX_LIM_CH2_V  0x000001FFU
#define RMT_RMT_RX_LIM_CH2_S  0

/** RMT_SYS_CONF_REG register
 *  RMT apb configuration register
 */
#define RMT_SYS_CONF_REG (DR_REG_RMT_BASE + 0x68)
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
/** RMT_RMT_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down RMT memory.
 */
#define RMT_RMT_MEM_FORCE_PD    (BIT(2))
#define RMT_RMT_MEM_FORCE_PD_M  (RMT_RMT_MEM_FORCE_PD_V << RMT_RMT_MEM_FORCE_PD_S)
#define RMT_RMT_MEM_FORCE_PD_V  0x00000001U
#define RMT_RMT_MEM_FORCE_PD_S  2
/** RMT_RMT_MEM_FORCE_PU : R/W; bitpos: [3]; default: 0;
 *  1: Disable RMT memory light sleep power down function. 0: Power down RMT memory
 *  when RMT is in light sleep mode.
 */
#define RMT_RMT_MEM_FORCE_PU    (BIT(3))
#define RMT_RMT_MEM_FORCE_PU_M  (RMT_RMT_MEM_FORCE_PU_V << RMT_RMT_MEM_FORCE_PU_S)
#define RMT_RMT_MEM_FORCE_PU_V  0x00000001U
#define RMT_RMT_MEM_FORCE_PU_S  3
/** RMT_RMT_SCLK_DIV_NUM : R/W; bitpos: [11:4]; default: 1;
 *  the integral part of the fractional divisor
 */
#define RMT_RMT_SCLK_DIV_NUM    0x000000FFU
#define RMT_RMT_SCLK_DIV_NUM_M  (RMT_RMT_SCLK_DIV_NUM_V << RMT_RMT_SCLK_DIV_NUM_S)
#define RMT_RMT_SCLK_DIV_NUM_V  0x000000FFU
#define RMT_RMT_SCLK_DIV_NUM_S  4
/** RMT_RMT_SCLK_DIV_A : R/W; bitpos: [17:12]; default: 0;
 *  the numerator of the fractional part of the fractional divisor
 */
#define RMT_RMT_SCLK_DIV_A    0x0000003FU
#define RMT_RMT_SCLK_DIV_A_M  (RMT_RMT_SCLK_DIV_A_V << RMT_RMT_SCLK_DIV_A_S)
#define RMT_RMT_SCLK_DIV_A_V  0x0000003FU
#define RMT_RMT_SCLK_DIV_A_S  12
/** RMT_RMT_SCLK_DIV_B : R/W; bitpos: [23:18]; default: 0;
 *  the denominator of the fractional part of the fractional divisor
 */
#define RMT_RMT_SCLK_DIV_B    0x0000003FU
#define RMT_RMT_SCLK_DIV_B_M  (RMT_RMT_SCLK_DIV_B_V << RMT_RMT_SCLK_DIV_B_S)
#define RMT_RMT_SCLK_DIV_B_V  0x0000003FU
#define RMT_RMT_SCLK_DIV_B_S  18
/** RMT_RMT_SCLK_SEL : R/W; bitpos: [25:24]; default: 1;
 *  choose the clock source of rmt_sclk. 1:CLK_80Mhz,2:CLK_FOSC, 3:XTAL
 */
#define RMT_RMT_SCLK_SEL    0x00000003U
#define RMT_RMT_SCLK_SEL_M  (RMT_RMT_SCLK_SEL_V << RMT_RMT_SCLK_SEL_S)
#define RMT_RMT_SCLK_SEL_V  0x00000003U
#define RMT_RMT_SCLK_SEL_S  24
/** RMT_RMT_SCLK_ACTIVE : R/W; bitpos: [26]; default: 1;
 *  rmt_sclk switch
 */
#define RMT_RMT_SCLK_ACTIVE    (BIT(26))
#define RMT_RMT_SCLK_ACTIVE_M  (RMT_RMT_SCLK_ACTIVE_V << RMT_RMT_SCLK_ACTIVE_S)
#define RMT_RMT_SCLK_ACTIVE_V  0x00000001U
#define RMT_RMT_SCLK_ACTIVE_S  26
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
#define RMT_TX_SIM_REG (DR_REG_RMT_BASE + 0x6c)
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
/** RMT_TX_SIM_EN : R/W; bitpos: [2]; default: 0;
 *  This register is used to enable multiple of channels to start sending data
 *  synchronously.
 */
#define RMT_TX_SIM_EN    (BIT(2))
#define RMT_TX_SIM_EN_M  (RMT_TX_SIM_EN_V << RMT_TX_SIM_EN_S)
#define RMT_TX_SIM_EN_V  0x00000001U
#define RMT_TX_SIM_EN_S  2

/** RMT_REF_CNT_RST_REG register
 *  RMT clock divider reset register
 */
#define RMT_REF_CNT_RST_REG (DR_REG_RMT_BASE + 0x70)
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

/** RMT_DATE_REG register
 *  RMT version register
 */
#define RMT_DATE_REG (DR_REG_RMT_BASE + 0xcc)
/** RMT_RMT_DATE : R/W; bitpos: [27:0]; default: 34636307;
 *  This is the version register.
 */
#define RMT_RMT_DATE    0x0FFFFFFFU
#define RMT_RMT_DATE_M  (RMT_RMT_DATE_V << RMT_RMT_DATE_S)
#define RMT_RMT_DATE_V  0x0FFFFFFFU
#define RMT_RMT_DATE_S  0

#ifdef __cplusplus
}
#endif
