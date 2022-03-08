/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** RMT_CH0DATA_REG register
 *  Read and write data for channel 0 via APB FIFO
 */
#define RMT_CH0DATA_REG (DR_REG_RMT_BASE + 0x0)
/** RMT_CH0_DATA : RO; bitpos: [31:0]; default: 0;
 *  This register is used to read and write data for channel 0 via APB FIFO.
 */
#define RMT_CH0_DATA    0xFFFFFFFFU
#define RMT_CH0_DATA_M  (RMT_CH0_DATA_V << RMT_CH0_DATA_S)
#define RMT_CH0_DATA_V  0xFFFFFFFFU
#define RMT_CH0_DATA_S  0

/** RMT_CH1DATA_REG register
 *  Read and write data for channel 1 via APB FIFO
 */
#define RMT_CH1DATA_REG (DR_REG_RMT_BASE + 0x4)
/** RMT_CH0_DATA : RO; bitpos: [31:0]; default: 0;
 *  This register is used to read and write data for channel 1 via APB FIFO.
 */
#define RMT_CH0_DATA    0xFFFFFFFFU
#define RMT_CH0_DATA_M  (RMT_CH0_DATA_V << RMT_CH0_DATA_S)
#define RMT_CH0_DATA_V  0xFFFFFFFFU
#define RMT_CH0_DATA_S  0

/** RMT_CH2DATA_REG register
 *  Read and write data for channel 2 via APB FIFO
 */
#define RMT_CH2DATA_REG (DR_REG_RMT_BASE + 0x8)
/** RMT_CH0_DATA : RO; bitpos: [31:0]; default: 0;
 *  This register is used to read and write data for channel 2 via APB FIFO.
 */
#define RMT_CH0_DATA    0xFFFFFFFFU
#define RMT_CH0_DATA_M  (RMT_CH0_DATA_V << RMT_CH0_DATA_S)
#define RMT_CH0_DATA_V  0xFFFFFFFFU
#define RMT_CH0_DATA_S  0

/** RMT_CH3DATA_REG register
 *  Read and write data for channel 3 via APB FIFO
 */
#define RMT_CH3DATA_REG (DR_REG_RMT_BASE + 0xc)
/** RMT_CH0_DATA : RO; bitpos: [31:0]; default: 0;
 *  This register is used to read and write data for channel 3 via APB FIFO.
 */
#define RMT_CH0_DATA    0xFFFFFFFFU
#define RMT_CH0_DATA_M  (RMT_CH0_DATA_V << RMT_CH0_DATA_S)
#define RMT_CH0_DATA_V  0xFFFFFFFFU
#define RMT_CH0_DATA_S  0

/** RMT_CH0CONF0_REG register
 *  Channel 0 configuration register 0
 */
#define RMT_CH0CONF0_REG (DR_REG_RMT_BASE + 0x10)
/** RMT_DIV_CNT_CH0 : R/W; bitpos: [7:0]; default: 2;
 *  This field is used to configure clock divider for channel 0.
 */
#define RMT_DIV_CNT_CH0    0x000000FFU
#define RMT_DIV_CNT_CH0_M  (RMT_DIV_CNT_CH0_V << RMT_DIV_CNT_CH0_S)
#define RMT_DIV_CNT_CH0_V  0x000000FFU
#define RMT_DIV_CNT_CH0_S  0
/** RMT_IDLE_THRES_CH0 : R/W; bitpos: [23:8]; default: 4096;
 *  Receiving ends when no edge is detected on input signals for continuous clock
 *  cycles longer than this field value.
 */
#define RMT_IDLE_THRES_CH0    0x0000FFFFU
#define RMT_IDLE_THRES_CH0_M  (RMT_IDLE_THRES_CH0_V << RMT_IDLE_THRES_CH0_S)
#define RMT_IDLE_THRES_CH0_V  0x0000FFFFU
#define RMT_IDLE_THRES_CH0_S  8
/** RMT_MEM_SIZE_CH0 : R/W; bitpos: [26:24]; default: 1;
 *  This field is used to configure the maximum blocks allocated to channel 0. The
 *  valid range is from 1 ~ 4-0.
 */
#define RMT_MEM_SIZE_CH0    0x00000007U
#define RMT_MEM_SIZE_CH0_M  (RMT_MEM_SIZE_CH0_V << RMT_MEM_SIZE_CH0_S)
#define RMT_MEM_SIZE_CH0_V  0x00000007U
#define RMT_MEM_SIZE_CH0_S  24
/** RMT_CARRIER_EFF_EN_CH0 : R/W; bitpos: [27]; default: 1;
 *  1: Add carrier modulation on output signals only at data sending state for channel
 *  0. 0: Add carrier modulation on signals at all states for channel 0. States here
 *  include idle state (ST_IDLE), reading data from RAM (ST_RD_MEM), and sending data
 *  stored in RAM (ST_SEND). Only valid when RMT_CARRIER_EN_CH0 is set to 1.
 */
#define RMT_CARRIER_EFF_EN_CH0    (BIT(27))
#define RMT_CARRIER_EFF_EN_CH0_M  (RMT_CARRIER_EFF_EN_CH0_V << RMT_CARRIER_EFF_EN_CH0_S)
#define RMT_CARRIER_EFF_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EFF_EN_CH0_S  27
/** RMT_CARRIER_EN_CH0 : R/W; bitpos: [28]; default: 1;
 *  This bit is used to enable carrier modulation for channel 0. 1: Add carrier
 *  modulation on output signals. 0: No carrier modulation is added on output signals.
 */
#define RMT_CARRIER_EN_CH0    (BIT(28))
#define RMT_CARRIER_EN_CH0_M  (RMT_CARRIER_EN_CH0_V << RMT_CARRIER_EN_CH0_S)
#define RMT_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EN_CH0_S  28
/** RMT_CARRIER_OUT_LV_CH0 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for channel 0.
 *
 *  1'h0: Add carrier wave on low-level output signals.
 *
 *  1'h1: Add carrier wave on high-level output signals.
 */
#define RMT_CARRIER_OUT_LV_CH0    (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_M  (RMT_CARRIER_OUT_LV_CH0_V << RMT_CARRIER_OUT_LV_CH0_S)
#define RMT_CARRIER_OUT_LV_CH0_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH0_S  29

/** RMT_CH0CONF1_REG register
 *  Channel 0 configuration register 1
 */
#define RMT_CH0CONF1_REG (DR_REG_RMT_BASE + 0x14)
/** RMT_TX_START_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on channel 0.
 */
#define RMT_TX_START_CH0    (BIT(0))
#define RMT_TX_START_CH0_M  (RMT_TX_START_CH0_V << RMT_TX_START_CH0_S)
#define RMT_TX_START_CH0_V  0x00000001U
#define RMT_TX_START_CH0_S  0
/** RMT_RX_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable receiver to receive data on channel 0.
 */
#define RMT_RX_EN_CH0    (BIT(1))
#define RMT_RX_EN_CH0_M  (RMT_RX_EN_CH0_V << RMT_RX_EN_CH0_S)
#define RMT_RX_EN_CH0_V  0x00000001U
#define RMT_RX_EN_CH0_S  1
/** RMT_MEM_WR_RST_CH0 : WO; bitpos: [2]; default: 0;
 *  Set this bit to reset RAM write address accessed by the receiver for channel 0.
 */
#define RMT_MEM_WR_RST_CH0    (BIT(2))
#define RMT_MEM_WR_RST_CH0_M  (RMT_MEM_WR_RST_CH0_V << RMT_MEM_WR_RST_CH0_S)
#define RMT_MEM_WR_RST_CH0_V  0x00000001U
#define RMT_MEM_WR_RST_CH0_S  2
/** RMT_MEM_RD_RST_CH0 : WO; bitpos: [3]; default: 0;
 *  Set this bit to reset RAM read address accessed by the transmitter for channel 0.
 */
#define RMT_MEM_RD_RST_CH0    (BIT(3))
#define RMT_MEM_RD_RST_CH0_M  (RMT_MEM_RD_RST_CH0_V << RMT_MEM_RD_RST_CH0_S)
#define RMT_MEM_RD_RST_CH0_V  0x00000001U
#define RMT_MEM_RD_RST_CH0_S  3
/** RMT_APB_MEM_RST_CH0 : WO; bitpos: [4]; default: 0;
 *  Set this bit to reset W/R ram address for channel 0 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH0    (BIT(4))
#define RMT_APB_MEM_RST_CH0_M  (RMT_APB_MEM_RST_CH0_V << RMT_APB_MEM_RST_CH0_S)
#define RMT_APB_MEM_RST_CH0_V  0x00000001U
#define RMT_APB_MEM_RST_CH0_S  4
/** RMT_MEM_OWNER_CH0 : R/W; bitpos: [5]; default: 1;
 *  This bit marks the ownership of channel 0's RAM block.
 *
 *  1'h1: Receiver is using the RAM.
 *
 *  1'h0: Transmitter is using the RAM.
 */
#define RMT_MEM_OWNER_CH0    (BIT(5))
#define RMT_MEM_OWNER_CH0_M  (RMT_MEM_OWNER_CH0_V << RMT_MEM_OWNER_CH0_S)
#define RMT_MEM_OWNER_CH0_V  0x00000001U
#define RMT_MEM_OWNER_CH0_S  5
/** RMT_TX_CONTI_MODE_CH0 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to restart transmission in continuous node from the first data in
 *  channel 0.
 */
#define RMT_TX_CONTI_MODE_CH0    (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_M  (RMT_TX_CONTI_MODE_CH0_V << RMT_TX_CONTI_MODE_CH0_S)
#define RMT_TX_CONTI_MODE_CH0_V  0x00000001U
#define RMT_TX_CONTI_MODE_CH0_S  6
/** RMT_RX_FILTER_EN_CH0 : R/W; bitpos: [7]; default: 0;
 *  Set this bit to enable the receiver's filter for channel 0.
 */
#define RMT_RX_FILTER_EN_CH0    (BIT(7))
#define RMT_RX_FILTER_EN_CH0_M  (RMT_RX_FILTER_EN_CH0_V << RMT_RX_FILTER_EN_CH0_S)
#define RMT_RX_FILTER_EN_CH0_V  0x00000001U
#define RMT_RX_FILTER_EN_CH0_S  7
/** RMT_RX_FILTER_THRES_CH0 : R/W; bitpos: [15:8]; default: 15;
 *  Set this field to ignore the input pulse when its width is less than
 *  RMT_RX_FILTER_THRES_CH0 APB clock cycles in receive mode.
 */
#define RMT_RX_FILTER_THRES_CH0    0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_M  (RMT_RX_FILTER_THRES_CH0_V << RMT_RX_FILTER_THRES_CH0_S)
#define RMT_RX_FILTER_THRES_CH0_V  0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_S  8
/** RMT_CHK_RX_CARRIER_EN_CH0 : R/W; bitpos: [16]; default: 0;
 *  Set this bit to enable memory loop read mode when carrier modulation is enabled for
 *  channel 0.
 */
#define RMT_CHK_RX_CARRIER_EN_CH0    (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH0_M  (RMT_CHK_RX_CARRIER_EN_CH0_V << RMT_CHK_RX_CARRIER_EN_CH0_S)
#define RMT_CHK_RX_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CHK_RX_CARRIER_EN_CH0_S  16
/** RMT_REF_ALWAYS_ON_CH0 : R/W; bitpos: [17]; default: 0;
 *  Set this bit to select a base clock for channel 0.
 *
 *  1'h1: APB_CLK    1'h0: REF_TICK
 */
#define RMT_REF_ALWAYS_ON_CH0    (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_M  (RMT_REF_ALWAYS_ON_CH0_V << RMT_REF_ALWAYS_ON_CH0_S)
#define RMT_REF_ALWAYS_ON_CH0_V  0x00000001U
#define RMT_REF_ALWAYS_ON_CH0_S  17
/** RMT_IDLE_OUT_LV_CH0 : R/W; bitpos: [18]; default: 0;
 *  This bit configures the level of output signals in channel 0 when the transmitter
 *  is in idle state.
 */
#define RMT_IDLE_OUT_LV_CH0    (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_M  (RMT_IDLE_OUT_LV_CH0_V << RMT_IDLE_OUT_LV_CH0_S)
#define RMT_IDLE_OUT_LV_CH0_V  0x00000001U
#define RMT_IDLE_OUT_LV_CH0_S  18
/** RMT_IDLE_OUT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This is the output enable bit for channel 0 in idle state.
 */
#define RMT_IDLE_OUT_EN_CH0    (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_M  (RMT_IDLE_OUT_EN_CH0_V << RMT_IDLE_OUT_EN_CH0_S)
#define RMT_IDLE_OUT_EN_CH0_V  0x00000001U
#define RMT_IDLE_OUT_EN_CH0_S  19
/** RMT_TX_STOP_CH0 : R/W; bitpos: [20]; default: 0;
 *  Set this bit to stop the transmitter of channel 0 sending data out.
 */
#define RMT_TX_STOP_CH0    (BIT(20))
#define RMT_TX_STOP_CH0_M  (RMT_TX_STOP_CH0_V << RMT_TX_STOP_CH0_S)
#define RMT_TX_STOP_CH0_V  0x00000001U
#define RMT_TX_STOP_CH0_S  20

/** RMT_CH1CONF0_REG register
 *  Channel 1 configuration register 0
 */
#define RMT_CH1CONF0_REG (DR_REG_RMT_BASE + 0x18)
/** RMT_DIV_CNT_CH0 : R/W; bitpos: [7:0]; default: 2;
 *  This field is used to configure clock divider for channel 1.
 */
#define RMT_DIV_CNT_CH0    0x000000FFU
#define RMT_DIV_CNT_CH0_M  (RMT_DIV_CNT_CH0_V << RMT_DIV_CNT_CH0_S)
#define RMT_DIV_CNT_CH0_V  0x000000FFU
#define RMT_DIV_CNT_CH0_S  0
/** RMT_IDLE_THRES_CH0 : R/W; bitpos: [23:8]; default: 4096;
 *  Receiving ends when no edge is detected on input signals for continuous clock
 *  cycles longer than this field value.
 */
#define RMT_IDLE_THRES_CH0    0x0000FFFFU
#define RMT_IDLE_THRES_CH0_M  (RMT_IDLE_THRES_CH0_V << RMT_IDLE_THRES_CH0_S)
#define RMT_IDLE_THRES_CH0_V  0x0000FFFFU
#define RMT_IDLE_THRES_CH0_S  8
/** RMT_MEM_SIZE_CH0 : R/W; bitpos: [26:24]; default: 1;
 *  This field is used to configure the maximum blocks allocated to channel 1. The
 *  valid range is from 1 ~ 4-1.
 */
#define RMT_MEM_SIZE_CH0    0x00000007U
#define RMT_MEM_SIZE_CH0_M  (RMT_MEM_SIZE_CH0_V << RMT_MEM_SIZE_CH0_S)
#define RMT_MEM_SIZE_CH0_V  0x00000007U
#define RMT_MEM_SIZE_CH0_S  24
/** RMT_CARRIER_EFF_EN_CH0 : R/W; bitpos: [27]; default: 1;
 *  1: Add carrier modulation on output signals only at data sending state for channel
 *  1. 0: Add carrier modulation on signals at all states for channel 1. States here
 *  include idle state (ST_IDLE), reading data from RAM (ST_RD_MEM), and sending data
 *  stored in RAM (ST_SEND). Only valid when RMT_CARRIER_EN_CH1 is set to 1.
 */
#define RMT_CARRIER_EFF_EN_CH0    (BIT(27))
#define RMT_CARRIER_EFF_EN_CH0_M  (RMT_CARRIER_EFF_EN_CH0_V << RMT_CARRIER_EFF_EN_CH0_S)
#define RMT_CARRIER_EFF_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EFF_EN_CH0_S  27
/** RMT_CARRIER_EN_CH0 : R/W; bitpos: [28]; default: 1;
 *  This bit is used to enable carrier modulation for channel 1. 1: Add carrier
 *  modulation on output signals. 0: No carrier modulation is added on output signals.
 */
#define RMT_CARRIER_EN_CH0    (BIT(28))
#define RMT_CARRIER_EN_CH0_M  (RMT_CARRIER_EN_CH0_V << RMT_CARRIER_EN_CH0_S)
#define RMT_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EN_CH0_S  28
/** RMT_CARRIER_OUT_LV_CH0 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for channel 1.
 *
 *  1'h0: Add carrier wave on low-level output signals.
 *
 *  1'h1: Add carrier wave on high-level output signals.
 */
#define RMT_CARRIER_OUT_LV_CH0    (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_M  (RMT_CARRIER_OUT_LV_CH0_V << RMT_CARRIER_OUT_LV_CH0_S)
#define RMT_CARRIER_OUT_LV_CH0_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH0_S  29

/** RMT_CH1CONF1_REG register
 *  Channel 1 configuration register 1
 */
#define RMT_CH1CONF1_REG (DR_REG_RMT_BASE + 0x1c)
/** RMT_TX_START_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on channel 1.
 */
#define RMT_TX_START_CH0    (BIT(0))
#define RMT_TX_START_CH0_M  (RMT_TX_START_CH0_V << RMT_TX_START_CH0_S)
#define RMT_TX_START_CH0_V  0x00000001U
#define RMT_TX_START_CH0_S  0
/** RMT_RX_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable receiver to receive data on channel 1.
 */
#define RMT_RX_EN_CH0    (BIT(1))
#define RMT_RX_EN_CH0_M  (RMT_RX_EN_CH0_V << RMT_RX_EN_CH0_S)
#define RMT_RX_EN_CH0_V  0x00000001U
#define RMT_RX_EN_CH0_S  1
/** RMT_MEM_WR_RST_CH0 : WO; bitpos: [2]; default: 0;
 *  Set this bit to reset RAM write address accessed by the receiver for channel 1.
 */
#define RMT_MEM_WR_RST_CH0    (BIT(2))
#define RMT_MEM_WR_RST_CH0_M  (RMT_MEM_WR_RST_CH0_V << RMT_MEM_WR_RST_CH0_S)
#define RMT_MEM_WR_RST_CH0_V  0x00000001U
#define RMT_MEM_WR_RST_CH0_S  2
/** RMT_MEM_RD_RST_CH0 : WO; bitpos: [3]; default: 0;
 *  Set this bit to reset RAM read address accessed by the transmitter for channel 1.
 */
#define RMT_MEM_RD_RST_CH0    (BIT(3))
#define RMT_MEM_RD_RST_CH0_M  (RMT_MEM_RD_RST_CH0_V << RMT_MEM_RD_RST_CH0_S)
#define RMT_MEM_RD_RST_CH0_V  0x00000001U
#define RMT_MEM_RD_RST_CH0_S  3
/** RMT_APB_MEM_RST_CH0 : WO; bitpos: [4]; default: 0;
 *  Set this bit to reset W/R ram address for channel 1 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH0    (BIT(4))
#define RMT_APB_MEM_RST_CH0_M  (RMT_APB_MEM_RST_CH0_V << RMT_APB_MEM_RST_CH0_S)
#define RMT_APB_MEM_RST_CH0_V  0x00000001U
#define RMT_APB_MEM_RST_CH0_S  4
/** RMT_MEM_OWNER_CH0 : R/W; bitpos: [5]; default: 1;
 *  This bit marks the ownership of channel 1's RAM block.
 *
 *  1'h1: Receiver is using the RAM.
 *
 *  1'h0: Transmitter is using the RAM.
 */
#define RMT_MEM_OWNER_CH0    (BIT(5))
#define RMT_MEM_OWNER_CH0_M  (RMT_MEM_OWNER_CH0_V << RMT_MEM_OWNER_CH0_S)
#define RMT_MEM_OWNER_CH0_V  0x00000001U
#define RMT_MEM_OWNER_CH0_S  5
/** RMT_TX_CONTI_MODE_CH0 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to restart transmission in continuous node from the first data in
 *  channel 1.
 */
#define RMT_TX_CONTI_MODE_CH0    (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_M  (RMT_TX_CONTI_MODE_CH0_V << RMT_TX_CONTI_MODE_CH0_S)
#define RMT_TX_CONTI_MODE_CH0_V  0x00000001U
#define RMT_TX_CONTI_MODE_CH0_S  6
/** RMT_RX_FILTER_EN_CH0 : R/W; bitpos: [7]; default: 0;
 *  Set this bit to enable the receiver's filter for channel 1.
 */
#define RMT_RX_FILTER_EN_CH0    (BIT(7))
#define RMT_RX_FILTER_EN_CH0_M  (RMT_RX_FILTER_EN_CH0_V << RMT_RX_FILTER_EN_CH0_S)
#define RMT_RX_FILTER_EN_CH0_V  0x00000001U
#define RMT_RX_FILTER_EN_CH0_S  7
/** RMT_RX_FILTER_THRES_CH0 : R/W; bitpos: [15:8]; default: 15;
 *  Set this field to ignore the input pulse when its width is less than
 *  RMT_RX_FILTER_THRES_CH1 APB clock cycles in receive mode.
 */
#define RMT_RX_FILTER_THRES_CH0    0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_M  (RMT_RX_FILTER_THRES_CH0_V << RMT_RX_FILTER_THRES_CH0_S)
#define RMT_RX_FILTER_THRES_CH0_V  0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_S  8
/** RMT_CHK_RX_CARRIER_EN_CH0 : R/W; bitpos: [16]; default: 0;
 *  Set this bit to enable memory loop read mode when carrier modulation is enabled for
 *  channel 1.
 */
#define RMT_CHK_RX_CARRIER_EN_CH0    (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH0_M  (RMT_CHK_RX_CARRIER_EN_CH0_V << RMT_CHK_RX_CARRIER_EN_CH0_S)
#define RMT_CHK_RX_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CHK_RX_CARRIER_EN_CH0_S  16
/** RMT_REF_ALWAYS_ON_CH0 : R/W; bitpos: [17]; default: 0;
 *  Set this bit to select a base clock for channel 1.
 *
 *  1'h1: APB_CLK    1'h0: REF_TICK
 */
#define RMT_REF_ALWAYS_ON_CH0    (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_M  (RMT_REF_ALWAYS_ON_CH0_V << RMT_REF_ALWAYS_ON_CH0_S)
#define RMT_REF_ALWAYS_ON_CH0_V  0x00000001U
#define RMT_REF_ALWAYS_ON_CH0_S  17
/** RMT_IDLE_OUT_LV_CH0 : R/W; bitpos: [18]; default: 0;
 *  This bit configures the level of output signals in channel 1 when the transmitter
 *  is in idle state.
 */
#define RMT_IDLE_OUT_LV_CH0    (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_M  (RMT_IDLE_OUT_LV_CH0_V << RMT_IDLE_OUT_LV_CH0_S)
#define RMT_IDLE_OUT_LV_CH0_V  0x00000001U
#define RMT_IDLE_OUT_LV_CH0_S  18
/** RMT_IDLE_OUT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This is the output enable bit for channel 1 in idle state.
 */
#define RMT_IDLE_OUT_EN_CH0    (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_M  (RMT_IDLE_OUT_EN_CH0_V << RMT_IDLE_OUT_EN_CH0_S)
#define RMT_IDLE_OUT_EN_CH0_V  0x00000001U
#define RMT_IDLE_OUT_EN_CH0_S  19
/** RMT_TX_STOP_CH0 : R/W; bitpos: [20]; default: 0;
 *  Set this bit to stop the transmitter of channel 1 sending data out.
 */
#define RMT_TX_STOP_CH0    (BIT(20))
#define RMT_TX_STOP_CH0_M  (RMT_TX_STOP_CH0_V << RMT_TX_STOP_CH0_S)
#define RMT_TX_STOP_CH0_V  0x00000001U
#define RMT_TX_STOP_CH0_S  20

/** RMT_CH2CONF0_REG register
 *  Channel 2 configuration register 0
 */
#define RMT_CH2CONF0_REG (DR_REG_RMT_BASE + 0x20)
/** RMT_DIV_CNT_CH0 : R/W; bitpos: [7:0]; default: 2;
 *  This field is used to configure clock divider for channel 2.
 */
#define RMT_DIV_CNT_CH0    0x000000FFU
#define RMT_DIV_CNT_CH0_M  (RMT_DIV_CNT_CH0_V << RMT_DIV_CNT_CH0_S)
#define RMT_DIV_CNT_CH0_V  0x000000FFU
#define RMT_DIV_CNT_CH0_S  0
/** RMT_IDLE_THRES_CH0 : R/W; bitpos: [23:8]; default: 4096;
 *  Receiving ends when no edge is detected on input signals for continuous clock
 *  cycles longer than this field value.
 */
#define RMT_IDLE_THRES_CH0    0x0000FFFFU
#define RMT_IDLE_THRES_CH0_M  (RMT_IDLE_THRES_CH0_V << RMT_IDLE_THRES_CH0_S)
#define RMT_IDLE_THRES_CH0_V  0x0000FFFFU
#define RMT_IDLE_THRES_CH0_S  8
/** RMT_MEM_SIZE_CH0 : R/W; bitpos: [26:24]; default: 1;
 *  This field is used to configure the maximum blocks allocated to channel 2. The
 *  valid range is from 1 ~ 4-2.
 */
#define RMT_MEM_SIZE_CH0    0x00000007U
#define RMT_MEM_SIZE_CH0_M  (RMT_MEM_SIZE_CH0_V << RMT_MEM_SIZE_CH0_S)
#define RMT_MEM_SIZE_CH0_V  0x00000007U
#define RMT_MEM_SIZE_CH0_S  24
/** RMT_CARRIER_EFF_EN_CH0 : R/W; bitpos: [27]; default: 1;
 *  1: Add carrier modulation on output signals only at data sending state for channel
 *  2. 0: Add carrier modulation on signals at all states for channel 2. States here
 *  include idle state (ST_IDLE), reading data from RAM (ST_RD_MEM), and sending data
 *  stored in RAM (ST_SEND). Only valid when RMT_CARRIER_EN_CH2 is set to 1.
 */
#define RMT_CARRIER_EFF_EN_CH0    (BIT(27))
#define RMT_CARRIER_EFF_EN_CH0_M  (RMT_CARRIER_EFF_EN_CH0_V << RMT_CARRIER_EFF_EN_CH0_S)
#define RMT_CARRIER_EFF_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EFF_EN_CH0_S  27
/** RMT_CARRIER_EN_CH0 : R/W; bitpos: [28]; default: 1;
 *  This bit is used to enable carrier modulation for channel 2. 1: Add carrier
 *  modulation on output signals. 0: No carrier modulation is added on output signals.
 */
#define RMT_CARRIER_EN_CH0    (BIT(28))
#define RMT_CARRIER_EN_CH0_M  (RMT_CARRIER_EN_CH0_V << RMT_CARRIER_EN_CH0_S)
#define RMT_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EN_CH0_S  28
/** RMT_CARRIER_OUT_LV_CH0 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for channel 2.
 *
 *  1'h0: Add carrier wave on low-level output signals.
 *
 *  1'h1: Add carrier wave on high-level output signals.
 */
#define RMT_CARRIER_OUT_LV_CH0    (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_M  (RMT_CARRIER_OUT_LV_CH0_V << RMT_CARRIER_OUT_LV_CH0_S)
#define RMT_CARRIER_OUT_LV_CH0_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH0_S  29

/** RMT_CH2CONF1_REG register
 *  Channel 2 configuration register 1
 */
#define RMT_CH2CONF1_REG (DR_REG_RMT_BASE + 0x24)
/** RMT_TX_START_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on channel 2.
 */
#define RMT_TX_START_CH0    (BIT(0))
#define RMT_TX_START_CH0_M  (RMT_TX_START_CH0_V << RMT_TX_START_CH0_S)
#define RMT_TX_START_CH0_V  0x00000001U
#define RMT_TX_START_CH0_S  0
/** RMT_RX_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable receiver to receive data on channel 2.
 */
#define RMT_RX_EN_CH0    (BIT(1))
#define RMT_RX_EN_CH0_M  (RMT_RX_EN_CH0_V << RMT_RX_EN_CH0_S)
#define RMT_RX_EN_CH0_V  0x00000001U
#define RMT_RX_EN_CH0_S  1
/** RMT_MEM_WR_RST_CH0 : WO; bitpos: [2]; default: 0;
 *  Set this bit to reset RAM write address accessed by the receiver for channel 2.
 */
#define RMT_MEM_WR_RST_CH0    (BIT(2))
#define RMT_MEM_WR_RST_CH0_M  (RMT_MEM_WR_RST_CH0_V << RMT_MEM_WR_RST_CH0_S)
#define RMT_MEM_WR_RST_CH0_V  0x00000001U
#define RMT_MEM_WR_RST_CH0_S  2
/** RMT_MEM_RD_RST_CH0 : WO; bitpos: [3]; default: 0;
 *  Set this bit to reset RAM read address accessed by the transmitter for channel 2.
 */
#define RMT_MEM_RD_RST_CH0    (BIT(3))
#define RMT_MEM_RD_RST_CH0_M  (RMT_MEM_RD_RST_CH0_V << RMT_MEM_RD_RST_CH0_S)
#define RMT_MEM_RD_RST_CH0_V  0x00000001U
#define RMT_MEM_RD_RST_CH0_S  3
/** RMT_APB_MEM_RST_CH0 : WO; bitpos: [4]; default: 0;
 *  Set this bit to reset W/R ram address for channel 2 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH0    (BIT(4))
#define RMT_APB_MEM_RST_CH0_M  (RMT_APB_MEM_RST_CH0_V << RMT_APB_MEM_RST_CH0_S)
#define RMT_APB_MEM_RST_CH0_V  0x00000001U
#define RMT_APB_MEM_RST_CH0_S  4
/** RMT_MEM_OWNER_CH0 : R/W; bitpos: [5]; default: 1;
 *  This bit marks the ownership of channel 2's RAM block.
 *
 *  1'h1: Receiver is using the RAM.
 *
 *  1'h0: Transmitter is using the RAM.
 */
#define RMT_MEM_OWNER_CH0    (BIT(5))
#define RMT_MEM_OWNER_CH0_M  (RMT_MEM_OWNER_CH0_V << RMT_MEM_OWNER_CH0_S)
#define RMT_MEM_OWNER_CH0_V  0x00000001U
#define RMT_MEM_OWNER_CH0_S  5
/** RMT_TX_CONTI_MODE_CH0 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to restart transmission in continuous node from the first data in
 *  channel 2.
 */
#define RMT_TX_CONTI_MODE_CH0    (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_M  (RMT_TX_CONTI_MODE_CH0_V << RMT_TX_CONTI_MODE_CH0_S)
#define RMT_TX_CONTI_MODE_CH0_V  0x00000001U
#define RMT_TX_CONTI_MODE_CH0_S  6
/** RMT_RX_FILTER_EN_CH0 : R/W; bitpos: [7]; default: 0;
 *  Set this bit to enable the receiver's filter for channel 2.
 */
#define RMT_RX_FILTER_EN_CH0    (BIT(7))
#define RMT_RX_FILTER_EN_CH0_M  (RMT_RX_FILTER_EN_CH0_V << RMT_RX_FILTER_EN_CH0_S)
#define RMT_RX_FILTER_EN_CH0_V  0x00000001U
#define RMT_RX_FILTER_EN_CH0_S  7
/** RMT_RX_FILTER_THRES_CH0 : R/W; bitpos: [15:8]; default: 15;
 *  Set this field to ignore the input pulse when its width is less than
 *  RMT_RX_FILTER_THRES_CH2 APB clock cycles in receive mode.
 */
#define RMT_RX_FILTER_THRES_CH0    0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_M  (RMT_RX_FILTER_THRES_CH0_V << RMT_RX_FILTER_THRES_CH0_S)
#define RMT_RX_FILTER_THRES_CH0_V  0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_S  8
/** RMT_CHK_RX_CARRIER_EN_CH0 : R/W; bitpos: [16]; default: 0;
 *  Set this bit to enable memory loop read mode when carrier modulation is enabled for
 *  channel 2.
 */
#define RMT_CHK_RX_CARRIER_EN_CH0    (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH0_M  (RMT_CHK_RX_CARRIER_EN_CH0_V << RMT_CHK_RX_CARRIER_EN_CH0_S)
#define RMT_CHK_RX_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CHK_RX_CARRIER_EN_CH0_S  16
/** RMT_REF_ALWAYS_ON_CH0 : R/W; bitpos: [17]; default: 0;
 *  Set this bit to select a base clock for channel 2.
 *
 *  1'h1: APB_CLK    1'h0: REF_TICK
 */
#define RMT_REF_ALWAYS_ON_CH0    (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_M  (RMT_REF_ALWAYS_ON_CH0_V << RMT_REF_ALWAYS_ON_CH0_S)
#define RMT_REF_ALWAYS_ON_CH0_V  0x00000001U
#define RMT_REF_ALWAYS_ON_CH0_S  17
/** RMT_IDLE_OUT_LV_CH0 : R/W; bitpos: [18]; default: 0;
 *  This bit configures the level of output signals in channel 2 when the transmitter
 *  is in idle state.
 */
#define RMT_IDLE_OUT_LV_CH0    (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_M  (RMT_IDLE_OUT_LV_CH0_V << RMT_IDLE_OUT_LV_CH0_S)
#define RMT_IDLE_OUT_LV_CH0_V  0x00000001U
#define RMT_IDLE_OUT_LV_CH0_S  18
/** RMT_IDLE_OUT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This is the output enable bit for channel 2 in idle state.
 */
#define RMT_IDLE_OUT_EN_CH0    (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_M  (RMT_IDLE_OUT_EN_CH0_V << RMT_IDLE_OUT_EN_CH0_S)
#define RMT_IDLE_OUT_EN_CH0_V  0x00000001U
#define RMT_IDLE_OUT_EN_CH0_S  19
/** RMT_TX_STOP_CH0 : R/W; bitpos: [20]; default: 0;
 *  Set this bit to stop the transmitter of channel 2 sending data out.
 */
#define RMT_TX_STOP_CH0    (BIT(20))
#define RMT_TX_STOP_CH0_M  (RMT_TX_STOP_CH0_V << RMT_TX_STOP_CH0_S)
#define RMT_TX_STOP_CH0_V  0x00000001U
#define RMT_TX_STOP_CH0_S  20

/** RMT_CH3CONF0_REG register
 *  Channel 3 configuration register 0
 */
#define RMT_CH3CONF0_REG (DR_REG_RMT_BASE + 0x28)
/** RMT_DIV_CNT_CH0 : R/W; bitpos: [7:0]; default: 2;
 *  This field is used to configure clock divider for channel 3.
 */
#define RMT_DIV_CNT_CH0    0x000000FFU
#define RMT_DIV_CNT_CH0_M  (RMT_DIV_CNT_CH0_V << RMT_DIV_CNT_CH0_S)
#define RMT_DIV_CNT_CH0_V  0x000000FFU
#define RMT_DIV_CNT_CH0_S  0
/** RMT_IDLE_THRES_CH0 : R/W; bitpos: [23:8]; default: 4096;
 *  Receiving ends when no edge is detected on input signals for continuous clock
 *  cycles longer than this field value.
 */
#define RMT_IDLE_THRES_CH0    0x0000FFFFU
#define RMT_IDLE_THRES_CH0_M  (RMT_IDLE_THRES_CH0_V << RMT_IDLE_THRES_CH0_S)
#define RMT_IDLE_THRES_CH0_V  0x0000FFFFU
#define RMT_IDLE_THRES_CH0_S  8
/** RMT_MEM_SIZE_CH0 : R/W; bitpos: [26:24]; default: 1;
 *  This field is used to configure the maximum blocks allocated to channel 3. The
 *  valid range is from 1 ~ 4-3.
 */
#define RMT_MEM_SIZE_CH0    0x00000007U
#define RMT_MEM_SIZE_CH0_M  (RMT_MEM_SIZE_CH0_V << RMT_MEM_SIZE_CH0_S)
#define RMT_MEM_SIZE_CH0_V  0x00000007U
#define RMT_MEM_SIZE_CH0_S  24
/** RMT_CARRIER_EFF_EN_CH0 : R/W; bitpos: [27]; default: 1;
 *  1: Add carrier modulation on output signals only at data sending state for channel
 *  3. 0: Add carrier modulation on signals at all states for channel 3. States here
 *  include idle state (ST_IDLE), reading data from RAM (ST_RD_MEM), and sending data
 *  stored in RAM (ST_SEND). Only valid when RMT_CARRIER_EN_CH3 is set to 1.
 */
#define RMT_CARRIER_EFF_EN_CH0    (BIT(27))
#define RMT_CARRIER_EFF_EN_CH0_M  (RMT_CARRIER_EFF_EN_CH0_V << RMT_CARRIER_EFF_EN_CH0_S)
#define RMT_CARRIER_EFF_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EFF_EN_CH0_S  27
/** RMT_CARRIER_EN_CH0 : R/W; bitpos: [28]; default: 1;
 *  This bit is used to enable carrier modulation for channel 3. 1: Add carrier
 *  modulation on output signals. 0: No carrier modulation is added on output signals.
 */
#define RMT_CARRIER_EN_CH0    (BIT(28))
#define RMT_CARRIER_EN_CH0_M  (RMT_CARRIER_EN_CH0_V << RMT_CARRIER_EN_CH0_S)
#define RMT_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CARRIER_EN_CH0_S  28
/** RMT_CARRIER_OUT_LV_CH0 : R/W; bitpos: [29]; default: 1;
 *  This bit is used to configure the position of carrier wave for channel 3.
 *
 *  1'h0: Add carrier wave on low-level output signals.
 *
 *  1'h1: Add carrier wave on high-level output signals.
 */
#define RMT_CARRIER_OUT_LV_CH0    (BIT(29))
#define RMT_CARRIER_OUT_LV_CH0_M  (RMT_CARRIER_OUT_LV_CH0_V << RMT_CARRIER_OUT_LV_CH0_S)
#define RMT_CARRIER_OUT_LV_CH0_V  0x00000001U
#define RMT_CARRIER_OUT_LV_CH0_S  29

/** RMT_CH3CONF1_REG register
 *  Channel 3 configuration register 1
 */
#define RMT_CH3CONF1_REG (DR_REG_RMT_BASE + 0x2c)
/** RMT_TX_START_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to start sending data on channel 3.
 */
#define RMT_TX_START_CH0    (BIT(0))
#define RMT_TX_START_CH0_M  (RMT_TX_START_CH0_V << RMT_TX_START_CH0_S)
#define RMT_TX_START_CH0_V  0x00000001U
#define RMT_TX_START_CH0_S  0
/** RMT_RX_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable receiver to receive data on channel 3.
 */
#define RMT_RX_EN_CH0    (BIT(1))
#define RMT_RX_EN_CH0_M  (RMT_RX_EN_CH0_V << RMT_RX_EN_CH0_S)
#define RMT_RX_EN_CH0_V  0x00000001U
#define RMT_RX_EN_CH0_S  1
/** RMT_MEM_WR_RST_CH0 : WO; bitpos: [2]; default: 0;
 *  Set this bit to reset RAM write address accessed by the receiver for channel 3.
 */
#define RMT_MEM_WR_RST_CH0    (BIT(2))
#define RMT_MEM_WR_RST_CH0_M  (RMT_MEM_WR_RST_CH0_V << RMT_MEM_WR_RST_CH0_S)
#define RMT_MEM_WR_RST_CH0_V  0x00000001U
#define RMT_MEM_WR_RST_CH0_S  2
/** RMT_MEM_RD_RST_CH0 : WO; bitpos: [3]; default: 0;
 *  Set this bit to reset RAM read address accessed by the transmitter for channel 3.
 */
#define RMT_MEM_RD_RST_CH0    (BIT(3))
#define RMT_MEM_RD_RST_CH0_M  (RMT_MEM_RD_RST_CH0_V << RMT_MEM_RD_RST_CH0_S)
#define RMT_MEM_RD_RST_CH0_V  0x00000001U
#define RMT_MEM_RD_RST_CH0_S  3
/** RMT_APB_MEM_RST_CH0 : WO; bitpos: [4]; default: 0;
 *  Set this bit to reset W/R ram address for channel 3 by accessing apb fifo.
 */
#define RMT_APB_MEM_RST_CH0    (BIT(4))
#define RMT_APB_MEM_RST_CH0_M  (RMT_APB_MEM_RST_CH0_V << RMT_APB_MEM_RST_CH0_S)
#define RMT_APB_MEM_RST_CH0_V  0x00000001U
#define RMT_APB_MEM_RST_CH0_S  4
/** RMT_MEM_OWNER_CH0 : R/W; bitpos: [5]; default: 1;
 *  This bit marks the ownership of channel 3's RAM block.
 *
 *  1'h1: Receiver is using the RAM.
 *
 *  1'h0: Transmitter is using the RAM.
 */
#define RMT_MEM_OWNER_CH0    (BIT(5))
#define RMT_MEM_OWNER_CH0_M  (RMT_MEM_OWNER_CH0_V << RMT_MEM_OWNER_CH0_S)
#define RMT_MEM_OWNER_CH0_V  0x00000001U
#define RMT_MEM_OWNER_CH0_S  5
/** RMT_TX_CONTI_MODE_CH0 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to restart transmission in continuous node from the first data in
 *  channel 3.
 */
#define RMT_TX_CONTI_MODE_CH0    (BIT(6))
#define RMT_TX_CONTI_MODE_CH0_M  (RMT_TX_CONTI_MODE_CH0_V << RMT_TX_CONTI_MODE_CH0_S)
#define RMT_TX_CONTI_MODE_CH0_V  0x00000001U
#define RMT_TX_CONTI_MODE_CH0_S  6
/** RMT_RX_FILTER_EN_CH0 : R/W; bitpos: [7]; default: 0;
 *  Set this bit to enable the receiver's filter for channel 3.
 */
#define RMT_RX_FILTER_EN_CH0    (BIT(7))
#define RMT_RX_FILTER_EN_CH0_M  (RMT_RX_FILTER_EN_CH0_V << RMT_RX_FILTER_EN_CH0_S)
#define RMT_RX_FILTER_EN_CH0_V  0x00000001U
#define RMT_RX_FILTER_EN_CH0_S  7
/** RMT_RX_FILTER_THRES_CH0 : R/W; bitpos: [15:8]; default: 15;
 *  Set this field to ignore the input pulse when its width is less than
 *  RMT_RX_FILTER_THRES_CH3 APB clock cycles in receive mode.
 */
#define RMT_RX_FILTER_THRES_CH0    0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_M  (RMT_RX_FILTER_THRES_CH0_V << RMT_RX_FILTER_THRES_CH0_S)
#define RMT_RX_FILTER_THRES_CH0_V  0x000000FFU
#define RMT_RX_FILTER_THRES_CH0_S  8
/** RMT_CHK_RX_CARRIER_EN_CH0 : R/W; bitpos: [16]; default: 0;
 *  Set this bit to enable memory loop read mode when carrier modulation is enabled for
 *  channel 3.
 */
#define RMT_CHK_RX_CARRIER_EN_CH0    (BIT(16))
#define RMT_CHK_RX_CARRIER_EN_CH0_M  (RMT_CHK_RX_CARRIER_EN_CH0_V << RMT_CHK_RX_CARRIER_EN_CH0_S)
#define RMT_CHK_RX_CARRIER_EN_CH0_V  0x00000001U
#define RMT_CHK_RX_CARRIER_EN_CH0_S  16
/** RMT_REF_ALWAYS_ON_CH0 : R/W; bitpos: [17]; default: 0;
 *  Set this bit to select a base clock for channel 3.
 *
 *  1'h1: APB_CLK    1'h0: REF_TICK
 */
#define RMT_REF_ALWAYS_ON_CH0    (BIT(17))
#define RMT_REF_ALWAYS_ON_CH0_M  (RMT_REF_ALWAYS_ON_CH0_V << RMT_REF_ALWAYS_ON_CH0_S)
#define RMT_REF_ALWAYS_ON_CH0_V  0x00000001U
#define RMT_REF_ALWAYS_ON_CH0_S  17
/** RMT_IDLE_OUT_LV_CH0 : R/W; bitpos: [18]; default: 0;
 *  This bit configures the level of output signals in channel 3 when the transmitter
 *  is in idle state.
 */
#define RMT_IDLE_OUT_LV_CH0    (BIT(18))
#define RMT_IDLE_OUT_LV_CH0_M  (RMT_IDLE_OUT_LV_CH0_V << RMT_IDLE_OUT_LV_CH0_S)
#define RMT_IDLE_OUT_LV_CH0_V  0x00000001U
#define RMT_IDLE_OUT_LV_CH0_S  18
/** RMT_IDLE_OUT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This is the output enable bit for channel 3 in idle state.
 */
#define RMT_IDLE_OUT_EN_CH0    (BIT(19))
#define RMT_IDLE_OUT_EN_CH0_M  (RMT_IDLE_OUT_EN_CH0_V << RMT_IDLE_OUT_EN_CH0_S)
#define RMT_IDLE_OUT_EN_CH0_V  0x00000001U
#define RMT_IDLE_OUT_EN_CH0_S  19
/** RMT_TX_STOP_CH0 : R/W; bitpos: [20]; default: 0;
 *  Set this bit to stop the transmitter of channel 3 sending data out.
 */
#define RMT_TX_STOP_CH0    (BIT(20))
#define RMT_TX_STOP_CH0_M  (RMT_TX_STOP_CH0_V << RMT_TX_STOP_CH0_S)
#define RMT_TX_STOP_CH0_V  0x00000001U
#define RMT_TX_STOP_CH0_S  20

/** RMT_CH0STATUS_REG register
 *  Channel 0 status register
 */
#define RMT_CH0STATUS_REG (DR_REG_RMT_BASE + 0x30)
/** RMT_MEM_WADDR_EX_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when receiver of channel 0 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_CH0    0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_M  (RMT_MEM_WADDR_EX_CH0_V << RMT_MEM_WADDR_EX_CH0_S)
#define RMT_MEM_WADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_S  0
/** RMT_MEM_RADDR_EX_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when transmitter of channel 0 is using
 *  the RAM.
 */
#define RMT_MEM_RADDR_EX_CH0    0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_M  (RMT_MEM_RADDR_EX_CH0_V << RMT_MEM_RADDR_EX_CH0_S)
#define RMT_MEM_RADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_S  10
/** RMT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  This field records the FSM status of channel 0.
 */
#define RMT_STATE_CH0    0x00000007U
#define RMT_STATE_CH0_M  (RMT_STATE_CH0_V << RMT_STATE_CH0_S)
#define RMT_STATE_CH0_V  0x00000007U
#define RMT_STATE_CH0_S  20
/** RMT_MEM_OWNER_ERR_CH0 : RO; bitpos: [23]; default: 0;
 *  This status bit will be set when the ownership of memory block is violated.
 */
#define RMT_MEM_OWNER_ERR_CH0    (BIT(23))
#define RMT_MEM_OWNER_ERR_CH0_M  (RMT_MEM_OWNER_ERR_CH0_V << RMT_MEM_OWNER_ERR_CH0_S)
#define RMT_MEM_OWNER_ERR_CH0_V  0x00000001U
#define RMT_MEM_OWNER_ERR_CH0_S  23
/** RMT_MEM_FULL_CH0 : RO; bitpos: [24]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory
 *  allows.
 */
#define RMT_MEM_FULL_CH0    (BIT(24))
#define RMT_MEM_FULL_CH0_M  (RMT_MEM_FULL_CH0_V << RMT_MEM_FULL_CH0_S)
#define RMT_MEM_FULL_CH0_V  0x00000001U
#define RMT_MEM_FULL_CH0_S  24
/** RMT_MEM_EMPTY_CH0 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be sent is more than memory allows and
 *  the wrap mode is disabled.
 */
#define RMT_MEM_EMPTY_CH0    (BIT(25))
#define RMT_MEM_EMPTY_CH0_M  (RMT_MEM_EMPTY_CH0_V << RMT_MEM_EMPTY_CH0_S)
#define RMT_MEM_EMPTY_CH0_V  0x00000001U
#define RMT_MEM_EMPTY_CH0_S  25
/** RMT_APB_MEM_WR_ERR_CH0 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  RAM via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_CH0    (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH0_M  (RMT_APB_MEM_WR_ERR_CH0_V << RMT_APB_MEM_WR_ERR_CH0_S)
#define RMT_APB_MEM_WR_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_CH0_S  26
/** RMT_APB_MEM_RD_ERR_CH0 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads RAM
 *  via APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH0    (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH0_M  (RMT_APB_MEM_RD_ERR_CH0_V << RMT_APB_MEM_RD_ERR_CH0_S)
#define RMT_APB_MEM_RD_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH0_S  27

/** RMT_CH1STATUS_REG register
 *  Channel 1 status register
 */
#define RMT_CH1STATUS_REG (DR_REG_RMT_BASE + 0x34)
/** RMT_MEM_WADDR_EX_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when receiver of channel 1 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_CH0    0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_M  (RMT_MEM_WADDR_EX_CH0_V << RMT_MEM_WADDR_EX_CH0_S)
#define RMT_MEM_WADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_S  0
/** RMT_MEM_RADDR_EX_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when transmitter of channel 1 is using
 *  the RAM.
 */
#define RMT_MEM_RADDR_EX_CH0    0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_M  (RMT_MEM_RADDR_EX_CH0_V << RMT_MEM_RADDR_EX_CH0_S)
#define RMT_MEM_RADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_S  10
/** RMT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  This field records the FSM status of channel 1.
 */
#define RMT_STATE_CH0    0x00000007U
#define RMT_STATE_CH0_M  (RMT_STATE_CH0_V << RMT_STATE_CH0_S)
#define RMT_STATE_CH0_V  0x00000007U
#define RMT_STATE_CH0_S  20
/** RMT_MEM_OWNER_ERR_CH0 : RO; bitpos: [23]; default: 0;
 *  This status bit will be set when the ownership of memory block is violated.
 */
#define RMT_MEM_OWNER_ERR_CH0    (BIT(23))
#define RMT_MEM_OWNER_ERR_CH0_M  (RMT_MEM_OWNER_ERR_CH0_V << RMT_MEM_OWNER_ERR_CH0_S)
#define RMT_MEM_OWNER_ERR_CH0_V  0x00000001U
#define RMT_MEM_OWNER_ERR_CH0_S  23
/** RMT_MEM_FULL_CH0 : RO; bitpos: [24]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory
 *  allows.
 */
#define RMT_MEM_FULL_CH0    (BIT(24))
#define RMT_MEM_FULL_CH0_M  (RMT_MEM_FULL_CH0_V << RMT_MEM_FULL_CH0_S)
#define RMT_MEM_FULL_CH0_V  0x00000001U
#define RMT_MEM_FULL_CH0_S  24
/** RMT_MEM_EMPTY_CH0 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be sent is more than memory allows and
 *  the wrap mode is disabled.
 */
#define RMT_MEM_EMPTY_CH0    (BIT(25))
#define RMT_MEM_EMPTY_CH0_M  (RMT_MEM_EMPTY_CH0_V << RMT_MEM_EMPTY_CH0_S)
#define RMT_MEM_EMPTY_CH0_V  0x00000001U
#define RMT_MEM_EMPTY_CH0_S  25
/** RMT_APB_MEM_WR_ERR_CH0 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  RAM via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_CH0    (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH0_M  (RMT_APB_MEM_WR_ERR_CH0_V << RMT_APB_MEM_WR_ERR_CH0_S)
#define RMT_APB_MEM_WR_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_CH0_S  26
/** RMT_APB_MEM_RD_ERR_CH0 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads RAM
 *  via APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH0    (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH0_M  (RMT_APB_MEM_RD_ERR_CH0_V << RMT_APB_MEM_RD_ERR_CH0_S)
#define RMT_APB_MEM_RD_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH0_S  27

/** RMT_CH2STATUS_REG register
 *  Channel 2 status register
 */
#define RMT_CH2STATUS_REG (DR_REG_RMT_BASE + 0x38)
/** RMT_MEM_WADDR_EX_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when receiver of channel 2 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_CH0    0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_M  (RMT_MEM_WADDR_EX_CH0_V << RMT_MEM_WADDR_EX_CH0_S)
#define RMT_MEM_WADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_S  0
/** RMT_MEM_RADDR_EX_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when transmitter of channel 2 is using
 *  the RAM.
 */
#define RMT_MEM_RADDR_EX_CH0    0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_M  (RMT_MEM_RADDR_EX_CH0_V << RMT_MEM_RADDR_EX_CH0_S)
#define RMT_MEM_RADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_S  10
/** RMT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  This field records the FSM status of channel 2.
 */
#define RMT_STATE_CH0    0x00000007U
#define RMT_STATE_CH0_M  (RMT_STATE_CH0_V << RMT_STATE_CH0_S)
#define RMT_STATE_CH0_V  0x00000007U
#define RMT_STATE_CH0_S  20
/** RMT_MEM_OWNER_ERR_CH0 : RO; bitpos: [23]; default: 0;
 *  This status bit will be set when the ownership of memory block is violated.
 */
#define RMT_MEM_OWNER_ERR_CH0    (BIT(23))
#define RMT_MEM_OWNER_ERR_CH0_M  (RMT_MEM_OWNER_ERR_CH0_V << RMT_MEM_OWNER_ERR_CH0_S)
#define RMT_MEM_OWNER_ERR_CH0_V  0x00000001U
#define RMT_MEM_OWNER_ERR_CH0_S  23
/** RMT_MEM_FULL_CH0 : RO; bitpos: [24]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory
 *  allows.
 */
#define RMT_MEM_FULL_CH0    (BIT(24))
#define RMT_MEM_FULL_CH0_M  (RMT_MEM_FULL_CH0_V << RMT_MEM_FULL_CH0_S)
#define RMT_MEM_FULL_CH0_V  0x00000001U
#define RMT_MEM_FULL_CH0_S  24
/** RMT_MEM_EMPTY_CH0 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be sent is more than memory allows and
 *  the wrap mode is disabled.
 */
#define RMT_MEM_EMPTY_CH0    (BIT(25))
#define RMT_MEM_EMPTY_CH0_M  (RMT_MEM_EMPTY_CH0_V << RMT_MEM_EMPTY_CH0_S)
#define RMT_MEM_EMPTY_CH0_V  0x00000001U
#define RMT_MEM_EMPTY_CH0_S  25
/** RMT_APB_MEM_WR_ERR_CH0 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  RAM via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_CH0    (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH0_M  (RMT_APB_MEM_WR_ERR_CH0_V << RMT_APB_MEM_WR_ERR_CH0_S)
#define RMT_APB_MEM_WR_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_CH0_S  26
/** RMT_APB_MEM_RD_ERR_CH0 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads RAM
 *  via APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH0    (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH0_M  (RMT_APB_MEM_RD_ERR_CH0_V << RMT_APB_MEM_RD_ERR_CH0_S)
#define RMT_APB_MEM_RD_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH0_S  27

/** RMT_CH3STATUS_REG register
 *  Channel 3 status register
 */
#define RMT_CH3STATUS_REG (DR_REG_RMT_BASE + 0x3c)
/** RMT_MEM_WADDR_EX_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when receiver of channel 3 is using
 *  the RAM.
 */
#define RMT_MEM_WADDR_EX_CH0    0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_M  (RMT_MEM_WADDR_EX_CH0_V << RMT_MEM_WADDR_EX_CH0_S)
#define RMT_MEM_WADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_WADDR_EX_CH0_S  0
/** RMT_MEM_RADDR_EX_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when transmitter of channel 3 is using
 *  the RAM.
 */
#define RMT_MEM_RADDR_EX_CH0    0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_M  (RMT_MEM_RADDR_EX_CH0_V << RMT_MEM_RADDR_EX_CH0_S)
#define RMT_MEM_RADDR_EX_CH0_V  0x000001FFU
#define RMT_MEM_RADDR_EX_CH0_S  10
/** RMT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  This field records the FSM status of channel 3.
 */
#define RMT_STATE_CH0    0x00000007U
#define RMT_STATE_CH0_M  (RMT_STATE_CH0_V << RMT_STATE_CH0_S)
#define RMT_STATE_CH0_V  0x00000007U
#define RMT_STATE_CH0_S  20
/** RMT_MEM_OWNER_ERR_CH0 : RO; bitpos: [23]; default: 0;
 *  This status bit will be set when the ownership of memory block is violated.
 */
#define RMT_MEM_OWNER_ERR_CH0    (BIT(23))
#define RMT_MEM_OWNER_ERR_CH0_M  (RMT_MEM_OWNER_ERR_CH0_V << RMT_MEM_OWNER_ERR_CH0_S)
#define RMT_MEM_OWNER_ERR_CH0_V  0x00000001U
#define RMT_MEM_OWNER_ERR_CH0_S  23
/** RMT_MEM_FULL_CH0 : RO; bitpos: [24]; default: 0;
 *  This status bit will be set if the receiver receives more data than the memory
 *  allows.
 */
#define RMT_MEM_FULL_CH0    (BIT(24))
#define RMT_MEM_FULL_CH0_M  (RMT_MEM_FULL_CH0_V << RMT_MEM_FULL_CH0_S)
#define RMT_MEM_FULL_CH0_V  0x00000001U
#define RMT_MEM_FULL_CH0_S  24
/** RMT_MEM_EMPTY_CH0 : RO; bitpos: [25]; default: 0;
 *  This status bit will be set when the data to be sent is more than memory allows and
 *  the wrap mode is disabled.
 */
#define RMT_MEM_EMPTY_CH0    (BIT(25))
#define RMT_MEM_EMPTY_CH0_M  (RMT_MEM_EMPTY_CH0_V << RMT_MEM_EMPTY_CH0_S)
#define RMT_MEM_EMPTY_CH0_V  0x00000001U
#define RMT_MEM_EMPTY_CH0_S  25
/** RMT_APB_MEM_WR_ERR_CH0 : RO; bitpos: [26]; default: 0;
 *  This status bit will be set if the offset address out of memory size when writes
 *  RAM via APB bus.
 */
#define RMT_APB_MEM_WR_ERR_CH0    (BIT(26))
#define RMT_APB_MEM_WR_ERR_CH0_M  (RMT_APB_MEM_WR_ERR_CH0_V << RMT_APB_MEM_WR_ERR_CH0_S)
#define RMT_APB_MEM_WR_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_WR_ERR_CH0_S  26
/** RMT_APB_MEM_RD_ERR_CH0 : RO; bitpos: [27]; default: 0;
 *  This status bit will be set if the offset address out of memory size when reads RAM
 *  via APB bus.
 */
#define RMT_APB_MEM_RD_ERR_CH0    (BIT(27))
#define RMT_APB_MEM_RD_ERR_CH0_M  (RMT_APB_MEM_RD_ERR_CH0_V << RMT_APB_MEM_RD_ERR_CH0_S)
#define RMT_APB_MEM_RD_ERR_CH0_V  0x00000001U
#define RMT_APB_MEM_RD_ERR_CH0_S  27

/** RMT_CH0ADDR_REG register
 *  Channel 0 address register
 */
#define RMT_CH0ADDR_REG (DR_REG_RMT_BASE + 0x40)
/** RMT_APB_MEM_WADDR_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when channel 0 writes RAM via APB bus.
 */
#define RMT_APB_MEM_WADDR_CH0    0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_M  (RMT_APB_MEM_WADDR_CH0_V << RMT_APB_MEM_WADDR_CH0_S)
#define RMT_APB_MEM_WADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_S  0
/** RMT_APB_MEM_RADDR_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when channel 0 reads RAM via APB bus.
 */
#define RMT_APB_MEM_RADDR_CH0    0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_M  (RMT_APB_MEM_RADDR_CH0_V << RMT_APB_MEM_RADDR_CH0_S)
#define RMT_APB_MEM_RADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_S  10

/** RMT_CH1ADDR_REG register
 *  Channel 1 address register
 */
#define RMT_CH1ADDR_REG (DR_REG_RMT_BASE + 0x44)
/** RMT_APB_MEM_WADDR_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when channel 1 writes RAM via APB bus.
 */
#define RMT_APB_MEM_WADDR_CH0    0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_M  (RMT_APB_MEM_WADDR_CH0_V << RMT_APB_MEM_WADDR_CH0_S)
#define RMT_APB_MEM_WADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_S  0
/** RMT_APB_MEM_RADDR_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when channel 1 reads RAM via APB bus.
 */
#define RMT_APB_MEM_RADDR_CH0    0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_M  (RMT_APB_MEM_RADDR_CH0_V << RMT_APB_MEM_RADDR_CH0_S)
#define RMT_APB_MEM_RADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_S  10

/** RMT_CH2ADDR_REG register
 *  Channel 2 address register
 */
#define RMT_CH2ADDR_REG (DR_REG_RMT_BASE + 0x48)
/** RMT_APB_MEM_WADDR_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when channel 2 writes RAM via APB bus.
 */
#define RMT_APB_MEM_WADDR_CH0    0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_M  (RMT_APB_MEM_WADDR_CH0_V << RMT_APB_MEM_WADDR_CH0_S)
#define RMT_APB_MEM_WADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_S  0
/** RMT_APB_MEM_RADDR_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when channel 2 reads RAM via APB bus.
 */
#define RMT_APB_MEM_RADDR_CH0    0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_M  (RMT_APB_MEM_RADDR_CH0_V << RMT_APB_MEM_RADDR_CH0_S)
#define RMT_APB_MEM_RADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_S  10

/** RMT_CH3ADDR_REG register
 *  Channel 3 address register
 */
#define RMT_CH3ADDR_REG (DR_REG_RMT_BASE + 0x4c)
/** RMT_APB_MEM_WADDR_CH0 : RO; bitpos: [8:0]; default: 0;
 *  This field records the memory address offset when channel 3 writes RAM via APB bus.
 */
#define RMT_APB_MEM_WADDR_CH0    0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_M  (RMT_APB_MEM_WADDR_CH0_V << RMT_APB_MEM_WADDR_CH0_S)
#define RMT_APB_MEM_WADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_WADDR_CH0_S  0
/** RMT_APB_MEM_RADDR_CH0 : RO; bitpos: [18:10]; default: 0;
 *  This field records the memory address offset when channel 3 reads RAM via APB bus.
 */
#define RMT_APB_MEM_RADDR_CH0    0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_M  (RMT_APB_MEM_RADDR_CH0_V << RMT_APB_MEM_RADDR_CH0_S)
#define RMT_APB_MEM_RADDR_CH0_V  0x000001FFU
#define RMT_APB_MEM_RADDR_CH0_S  10

/** RMT_INT_RAW_REG register
 *  Raw interrupt status register
 */
#define RMT_INT_RAW_REG (DR_REG_RMT_BASE + 0x50)
/** RMT_CH0_TX_END_INT_RAW : RO; bitpos: [0]; default: 0;
 *  The interrupt raw bit for channel 0. Triggered when transmitting ends.
 */
#define RMT_CH0_TX_END_INT_RAW    (BIT(0))
#define RMT_CH0_TX_END_INT_RAW_M  (RMT_CH0_TX_END_INT_RAW_V << RMT_CH0_TX_END_INT_RAW_S)
#define RMT_CH0_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH0_TX_END_INT_RAW_S  0
/** RMT_CH0_RX_END_INT_RAW : RO; bitpos: [1]; default: 0;
 *  The interrupt raw bit for channel 0. Triggered when receiving ends.
 */
#define RMT_CH0_RX_END_INT_RAW    (BIT(1))
#define RMT_CH0_RX_END_INT_RAW_M  (RMT_CH0_RX_END_INT_RAW_V << RMT_CH0_RX_END_INT_RAW_S)
#define RMT_CH0_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH0_RX_END_INT_RAW_S  1
/** RMT_CH0_ERR_INT_RAW : RO; bitpos: [2]; default: 0;
 *  The interrupt raw bit for channel 0. Triggered when error occurs.
 */
#define RMT_CH0_ERR_INT_RAW    (BIT(2))
#define RMT_CH0_ERR_INT_RAW_M  (RMT_CH0_ERR_INT_RAW_V << RMT_CH0_ERR_INT_RAW_S)
#define RMT_CH0_ERR_INT_RAW_V  0x00000001U
#define RMT_CH0_ERR_INT_RAW_S  2
/** RMT_CH1_TX_END_INT_RAW : RO; bitpos: [3]; default: 0;
 *  The interrupt raw bit for channel 1. Triggered when transmitting ends.
 */
#define RMT_CH1_TX_END_INT_RAW    (BIT(3))
#define RMT_CH1_TX_END_INT_RAW_M  (RMT_CH1_TX_END_INT_RAW_V << RMT_CH1_TX_END_INT_RAW_S)
#define RMT_CH1_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH1_TX_END_INT_RAW_S  3
/** RMT_CH1_RX_END_INT_RAW : RO; bitpos: [4]; default: 0;
 *  The interrupt raw bit for channel 1. Triggered when receiving ends.
 */
#define RMT_CH1_RX_END_INT_RAW    (BIT(4))
#define RMT_CH1_RX_END_INT_RAW_M  (RMT_CH1_RX_END_INT_RAW_V << RMT_CH1_RX_END_INT_RAW_S)
#define RMT_CH1_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH1_RX_END_INT_RAW_S  4
/** RMT_CH1_ERR_INT_RAW : RO; bitpos: [5]; default: 0;
 *  The interrupt raw bit for channel 1. Triggered when error occurs.
 */
#define RMT_CH1_ERR_INT_RAW    (BIT(5))
#define RMT_CH1_ERR_INT_RAW_M  (RMT_CH1_ERR_INT_RAW_V << RMT_CH1_ERR_INT_RAW_S)
#define RMT_CH1_ERR_INT_RAW_V  0x00000001U
#define RMT_CH1_ERR_INT_RAW_S  5
/** RMT_CH2_TX_END_INT_RAW : RO; bitpos: [6]; default: 0;
 *  The interrupt raw bit for channel 2. Triggered when transmitting ends.
 */
#define RMT_CH2_TX_END_INT_RAW    (BIT(6))
#define RMT_CH2_TX_END_INT_RAW_M  (RMT_CH2_TX_END_INT_RAW_V << RMT_CH2_TX_END_INT_RAW_S)
#define RMT_CH2_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH2_TX_END_INT_RAW_S  6
/** RMT_CH2_RX_END_INT_RAW : RO; bitpos: [7]; default: 0;
 *  The interrupt raw bit for channel 2. Triggered when receiving ends.
 */
#define RMT_CH2_RX_END_INT_RAW    (BIT(7))
#define RMT_CH2_RX_END_INT_RAW_M  (RMT_CH2_RX_END_INT_RAW_V << RMT_CH2_RX_END_INT_RAW_S)
#define RMT_CH2_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH2_RX_END_INT_RAW_S  7
/** RMT_CH2_ERR_INT_RAW : RO; bitpos: [8]; default: 0;
 *  The interrupt raw bit for channel 2. Triggered when error occurs.
 */
#define RMT_CH2_ERR_INT_RAW    (BIT(8))
#define RMT_CH2_ERR_INT_RAW_M  (RMT_CH2_ERR_INT_RAW_V << RMT_CH2_ERR_INT_RAW_S)
#define RMT_CH2_ERR_INT_RAW_V  0x00000001U
#define RMT_CH2_ERR_INT_RAW_S  8
/** RMT_CH3_TX_END_INT_RAW : RO; bitpos: [9]; default: 0;
 *  The interrupt raw bit for channel 3. Triggered when transmitting ends.
 */
#define RMT_CH3_TX_END_INT_RAW    (BIT(9))
#define RMT_CH3_TX_END_INT_RAW_M  (RMT_CH3_TX_END_INT_RAW_V << RMT_CH3_TX_END_INT_RAW_S)
#define RMT_CH3_TX_END_INT_RAW_V  0x00000001U
#define RMT_CH3_TX_END_INT_RAW_S  9
/** RMT_CH3_RX_END_INT_RAW : RO; bitpos: [10]; default: 0;
 *  The interrupt raw bit for channel 3. Triggered when receiving ends.
 */
#define RMT_CH3_RX_END_INT_RAW    (BIT(10))
#define RMT_CH3_RX_END_INT_RAW_M  (RMT_CH3_RX_END_INT_RAW_V << RMT_CH3_RX_END_INT_RAW_S)
#define RMT_CH3_RX_END_INT_RAW_V  0x00000001U
#define RMT_CH3_RX_END_INT_RAW_S  10
/** RMT_CH3_ERR_INT_RAW : RO; bitpos: [11]; default: 0;
 *  The interrupt raw bit for channel 3. Triggered when error occurs.
 */
#define RMT_CH3_ERR_INT_RAW    (BIT(11))
#define RMT_CH3_ERR_INT_RAW_M  (RMT_CH3_ERR_INT_RAW_V << RMT_CH3_ERR_INT_RAW_S)
#define RMT_CH3_ERR_INT_RAW_V  0x00000001U
#define RMT_CH3_ERR_INT_RAW_S  11
/** RMT_CH0_TX_THR_EVENT_INT_RAW : RO; bitpos: [12]; default: 0;
 *  The interrupt raw bit for channel 0. Triggered when transmitter sends more data
 *  than configured value.
 */
#define RMT_CH0_TX_THR_EVENT_INT_RAW    (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_RAW_M  (RMT_CH0_TX_THR_EVENT_INT_RAW_V << RMT_CH0_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH0_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_RAW_S  12
/** RMT_CH1_TX_THR_EVENT_INT_RAW : RO; bitpos: [13]; default: 0;
 *  The interrupt raw bit for channel 1. Triggered when transmitter sends more data
 *  than configured value.
 */
#define RMT_CH1_TX_THR_EVENT_INT_RAW    (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_RAW_M  (RMT_CH1_TX_THR_EVENT_INT_RAW_V << RMT_CH1_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH1_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_RAW_S  13
/** RMT_CH2_TX_THR_EVENT_INT_RAW : RO; bitpos: [14]; default: 0;
 *  The interrupt raw bit for channel 2. Triggered when transmitter sends more data
 *  than configured value.
 */
#define RMT_CH2_TX_THR_EVENT_INT_RAW    (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_RAW_M  (RMT_CH2_TX_THR_EVENT_INT_RAW_V << RMT_CH2_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH2_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_RAW_S  14
/** RMT_CH3_TX_THR_EVENT_INT_RAW : RO; bitpos: [15]; default: 0;
 *  The interrupt raw bit for channel 3. Triggered when transmitter sends more data
 *  than configured value.
 */
#define RMT_CH3_TX_THR_EVENT_INT_RAW    (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_RAW_M  (RMT_CH3_TX_THR_EVENT_INT_RAW_V << RMT_CH3_TX_THR_EVENT_INT_RAW_S)
#define RMT_CH3_TX_THR_EVENT_INT_RAW_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_RAW_S  15
/** RMT_CH0_TX_LOOP_INT_RAW : RO; bitpos: [16]; default: 0;
 *  The interrupt raw bit for channel 0. Triggered when loop counting reaches the
 *  configured threshold value.
 */
#define RMT_CH0_TX_LOOP_INT_RAW    (BIT(16))
#define RMT_CH0_TX_LOOP_INT_RAW_M  (RMT_CH0_TX_LOOP_INT_RAW_V << RMT_CH0_TX_LOOP_INT_RAW_S)
#define RMT_CH0_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_RAW_S  16
/** RMT_CH1_TX_LOOP_INT_RAW : RO; bitpos: [17]; default: 0;
 *  The interrupt raw bit for channel 1. Triggered when loop counting reaches the
 *  configured threshold value.
 */
#define RMT_CH1_TX_LOOP_INT_RAW    (BIT(17))
#define RMT_CH1_TX_LOOP_INT_RAW_M  (RMT_CH1_TX_LOOP_INT_RAW_V << RMT_CH1_TX_LOOP_INT_RAW_S)
#define RMT_CH1_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_RAW_S  17
/** RMT_CH2_TX_LOOP_INT_RAW : RO; bitpos: [18]; default: 0;
 *  The interrupt raw bit for channel 2. Triggered when loop counting reaches the
 *  configured threshold value.
 */
#define RMT_CH2_TX_LOOP_INT_RAW    (BIT(18))
#define RMT_CH2_TX_LOOP_INT_RAW_M  (RMT_CH2_TX_LOOP_INT_RAW_V << RMT_CH2_TX_LOOP_INT_RAW_S)
#define RMT_CH2_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_RAW_S  18
/** RMT_CH3_TX_LOOP_INT_RAW : RO; bitpos: [19]; default: 0;
 *  The interrupt raw bit for channel 3. Triggered when loop counting reaches the
 *  configured threshold value.
 */
#define RMT_CH3_TX_LOOP_INT_RAW    (BIT(19))
#define RMT_CH3_TX_LOOP_INT_RAW_M  (RMT_CH3_TX_LOOP_INT_RAW_V << RMT_CH3_TX_LOOP_INT_RAW_S)
#define RMT_CH3_TX_LOOP_INT_RAW_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_RAW_S  19

/** RMT_INT_ST_REG register
 *  Masked interrupt status register
 */
#define RMT_INT_ST_REG (DR_REG_RMT_BASE + 0x54)
/** RMT_CH0_TX_END_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit for RMT_CH0_TX_END_INT.
 */
#define RMT_CH0_TX_END_INT_ST    (BIT(0))
#define RMT_CH0_TX_END_INT_ST_M  (RMT_CH0_TX_END_INT_ST_V << RMT_CH0_TX_END_INT_ST_S)
#define RMT_CH0_TX_END_INT_ST_V  0x00000001U
#define RMT_CH0_TX_END_INT_ST_S  0
/** RMT_CH0_RX_END_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit for RMT_CH0_RX_END_INT.
 */
#define RMT_CH0_RX_END_INT_ST    (BIT(1))
#define RMT_CH0_RX_END_INT_ST_M  (RMT_CH0_RX_END_INT_ST_V << RMT_CH0_RX_END_INT_ST_S)
#define RMT_CH0_RX_END_INT_ST_V  0x00000001U
#define RMT_CH0_RX_END_INT_ST_S  1
/** RMT_CH0_ERR_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit for RMT_CH0_ERR_INT.
 */
#define RMT_CH0_ERR_INT_ST    (BIT(2))
#define RMT_CH0_ERR_INT_ST_M  (RMT_CH0_ERR_INT_ST_V << RMT_CH0_ERR_INT_ST_S)
#define RMT_CH0_ERR_INT_ST_V  0x00000001U
#define RMT_CH0_ERR_INT_ST_S  2
/** RMT_CH1_TX_END_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit for RMT_CH1_TX_END_INT.
 */
#define RMT_CH1_TX_END_INT_ST    (BIT(3))
#define RMT_CH1_TX_END_INT_ST_M  (RMT_CH1_TX_END_INT_ST_V << RMT_CH1_TX_END_INT_ST_S)
#define RMT_CH1_TX_END_INT_ST_V  0x00000001U
#define RMT_CH1_TX_END_INT_ST_S  3
/** RMT_CH1_RX_END_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit for RMT_CH1_RX_END_INT.
 */
#define RMT_CH1_RX_END_INT_ST    (BIT(4))
#define RMT_CH1_RX_END_INT_ST_M  (RMT_CH1_RX_END_INT_ST_V << RMT_CH1_RX_END_INT_ST_S)
#define RMT_CH1_RX_END_INT_ST_V  0x00000001U
#define RMT_CH1_RX_END_INT_ST_S  4
/** RMT_CH1_ERR_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit for RMT_CH1_ERR_INT.
 */
#define RMT_CH1_ERR_INT_ST    (BIT(5))
#define RMT_CH1_ERR_INT_ST_M  (RMT_CH1_ERR_INT_ST_V << RMT_CH1_ERR_INT_ST_S)
#define RMT_CH1_ERR_INT_ST_V  0x00000001U
#define RMT_CH1_ERR_INT_ST_S  5
/** RMT_CH2_TX_END_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status bit for RMT_CH2_TX_END_INT.
 */
#define RMT_CH2_TX_END_INT_ST    (BIT(6))
#define RMT_CH2_TX_END_INT_ST_M  (RMT_CH2_TX_END_INT_ST_V << RMT_CH2_TX_END_INT_ST_S)
#define RMT_CH2_TX_END_INT_ST_V  0x00000001U
#define RMT_CH2_TX_END_INT_ST_S  6
/** RMT_CH2_RX_END_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status bit for RMT_CH2_RX_END_INT.
 */
#define RMT_CH2_RX_END_INT_ST    (BIT(7))
#define RMT_CH2_RX_END_INT_ST_M  (RMT_CH2_RX_END_INT_ST_V << RMT_CH2_RX_END_INT_ST_S)
#define RMT_CH2_RX_END_INT_ST_V  0x00000001U
#define RMT_CH2_RX_END_INT_ST_S  7
/** RMT_CH2_ERR_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status bit for RMT_CH2_ERR_INT.
 */
#define RMT_CH2_ERR_INT_ST    (BIT(8))
#define RMT_CH2_ERR_INT_ST_M  (RMT_CH2_ERR_INT_ST_V << RMT_CH2_ERR_INT_ST_S)
#define RMT_CH2_ERR_INT_ST_V  0x00000001U
#define RMT_CH2_ERR_INT_ST_S  8
/** RMT_CH3_TX_END_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status bit for RMT_CH3_TX_END_INT.
 */
#define RMT_CH3_TX_END_INT_ST    (BIT(9))
#define RMT_CH3_TX_END_INT_ST_M  (RMT_CH3_TX_END_INT_ST_V << RMT_CH3_TX_END_INT_ST_S)
#define RMT_CH3_TX_END_INT_ST_V  0x00000001U
#define RMT_CH3_TX_END_INT_ST_S  9
/** RMT_CH3_RX_END_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status bit for RMT_CH3_RX_END_INT.
 */
#define RMT_CH3_RX_END_INT_ST    (BIT(10))
#define RMT_CH3_RX_END_INT_ST_M  (RMT_CH3_RX_END_INT_ST_V << RMT_CH3_RX_END_INT_ST_S)
#define RMT_CH3_RX_END_INT_ST_V  0x00000001U
#define RMT_CH3_RX_END_INT_ST_S  10
/** RMT_CH3_ERR_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status bit for RMT_CH3_ERR_INT.
 */
#define RMT_CH3_ERR_INT_ST    (BIT(11))
#define RMT_CH3_ERR_INT_ST_M  (RMT_CH3_ERR_INT_ST_V << RMT_CH3_ERR_INT_ST_S)
#define RMT_CH3_ERR_INT_ST_V  0x00000001U
#define RMT_CH3_ERR_INT_ST_S  11
/** RMT_CH0_TX_THR_EVENT_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status bit for RMT_CH0_TX_THR_EVENT_INT.
 */
#define RMT_CH0_TX_THR_EVENT_INT_ST    (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_ST_M  (RMT_CH0_TX_THR_EVENT_INT_ST_V << RMT_CH0_TX_THR_EVENT_INT_ST_S)
#define RMT_CH0_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_ST_S  12
/** RMT_CH1_TX_THR_EVENT_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status bit for RMT_CH1_TX_THR_EVENT_INT.
 */
#define RMT_CH1_TX_THR_EVENT_INT_ST    (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_ST_M  (RMT_CH1_TX_THR_EVENT_INT_ST_V << RMT_CH1_TX_THR_EVENT_INT_ST_S)
#define RMT_CH1_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_ST_S  13
/** RMT_CH2_TX_THR_EVENT_INT_ST : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status bit for RMT_CH2_TX_THR_EVENT_INT.
 */
#define RMT_CH2_TX_THR_EVENT_INT_ST    (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_ST_M  (RMT_CH2_TX_THR_EVENT_INT_ST_V << RMT_CH2_TX_THR_EVENT_INT_ST_S)
#define RMT_CH2_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_ST_S  14
/** RMT_CH3_TX_THR_EVENT_INT_ST : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status bit for RMT_CH3_TX_THR_EVENT_INT.
 */
#define RMT_CH3_TX_THR_EVENT_INT_ST    (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_ST_M  (RMT_CH3_TX_THR_EVENT_INT_ST_V << RMT_CH3_TX_THR_EVENT_INT_ST_S)
#define RMT_CH3_TX_THR_EVENT_INT_ST_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_ST_S  15
/** RMT_CH0_TX_LOOP_INT_ST : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status bit for RMT_CH0_TX_LOOP_INT.
 */
#define RMT_CH0_TX_LOOP_INT_ST    (BIT(16))
#define RMT_CH0_TX_LOOP_INT_ST_M  (RMT_CH0_TX_LOOP_INT_ST_V << RMT_CH0_TX_LOOP_INT_ST_S)
#define RMT_CH0_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_ST_S  16
/** RMT_CH1_TX_LOOP_INT_ST : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status bit for RMT_CH1_TX_LOOP_INT.
 */
#define RMT_CH1_TX_LOOP_INT_ST    (BIT(17))
#define RMT_CH1_TX_LOOP_INT_ST_M  (RMT_CH1_TX_LOOP_INT_ST_V << RMT_CH1_TX_LOOP_INT_ST_S)
#define RMT_CH1_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_ST_S  17
/** RMT_CH2_TX_LOOP_INT_ST : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status bit for RMT_CH2_TX_LOOP_INT.
 */
#define RMT_CH2_TX_LOOP_INT_ST    (BIT(18))
#define RMT_CH2_TX_LOOP_INT_ST_M  (RMT_CH2_TX_LOOP_INT_ST_V << RMT_CH2_TX_LOOP_INT_ST_S)
#define RMT_CH2_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_ST_S  18
/** RMT_CH3_TX_LOOP_INT_ST : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status bit for RMT_CH3_TX_LOOP_INT.
 */
#define RMT_CH3_TX_LOOP_INT_ST    (BIT(19))
#define RMT_CH3_TX_LOOP_INT_ST_M  (RMT_CH3_TX_LOOP_INT_ST_V << RMT_CH3_TX_LOOP_INT_ST_S)
#define RMT_CH3_TX_LOOP_INT_ST_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_ST_S  19

/** RMT_INT_ENA_REG register
 *  Interrupt enable register
 */
#define RMT_INT_ENA_REG (DR_REG_RMT_BASE + 0x58)
/** RMT_CH0_TX_END_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enabled bit for RMT_CH0_TX_END_INT.
 */
#define RMT_CH0_TX_END_INT_ENA    (BIT(0))
#define RMT_CH0_TX_END_INT_ENA_M  (RMT_CH0_TX_END_INT_ENA_V << RMT_CH0_TX_END_INT_ENA_S)
#define RMT_CH0_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH0_TX_END_INT_ENA_S  0
/** RMT_CH0_RX_END_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enabled bit for RMT_CH0_RX_END_INT.
 */
#define RMT_CH0_RX_END_INT_ENA    (BIT(1))
#define RMT_CH0_RX_END_INT_ENA_M  (RMT_CH0_RX_END_INT_ENA_V << RMT_CH0_RX_END_INT_ENA_S)
#define RMT_CH0_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH0_RX_END_INT_ENA_S  1
/** RMT_CH0_ERR_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enabled bit for RMT_CH0_ERR_INT.
 */
#define RMT_CH0_ERR_INT_ENA    (BIT(2))
#define RMT_CH0_ERR_INT_ENA_M  (RMT_CH0_ERR_INT_ENA_V << RMT_CH0_ERR_INT_ENA_S)
#define RMT_CH0_ERR_INT_ENA_V  0x00000001U
#define RMT_CH0_ERR_INT_ENA_S  2
/** RMT_CH1_TX_END_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enabled bit for RMT_CH1_TX_END_INT.
 */
#define RMT_CH1_TX_END_INT_ENA    (BIT(3))
#define RMT_CH1_TX_END_INT_ENA_M  (RMT_CH1_TX_END_INT_ENA_V << RMT_CH1_TX_END_INT_ENA_S)
#define RMT_CH1_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH1_TX_END_INT_ENA_S  3
/** RMT_CH1_RX_END_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enabled bit for RMT_CH1_RX_END_INT.
 */
#define RMT_CH1_RX_END_INT_ENA    (BIT(4))
#define RMT_CH1_RX_END_INT_ENA_M  (RMT_CH1_RX_END_INT_ENA_V << RMT_CH1_RX_END_INT_ENA_S)
#define RMT_CH1_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH1_RX_END_INT_ENA_S  4
/** RMT_CH1_ERR_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enabled bit for RMT_CH1_ERR_INT.
 */
#define RMT_CH1_ERR_INT_ENA    (BIT(5))
#define RMT_CH1_ERR_INT_ENA_M  (RMT_CH1_ERR_INT_ENA_V << RMT_CH1_ERR_INT_ENA_S)
#define RMT_CH1_ERR_INT_ENA_V  0x00000001U
#define RMT_CH1_ERR_INT_ENA_S  5
/** RMT_CH2_TX_END_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enabled bit for RMT_CH2_TX_END_INT.
 */
#define RMT_CH2_TX_END_INT_ENA    (BIT(6))
#define RMT_CH2_TX_END_INT_ENA_M  (RMT_CH2_TX_END_INT_ENA_V << RMT_CH2_TX_END_INT_ENA_S)
#define RMT_CH2_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH2_TX_END_INT_ENA_S  6
/** RMT_CH2_RX_END_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enabled bit for RMT_CH2_RX_END_INT.
 */
#define RMT_CH2_RX_END_INT_ENA    (BIT(7))
#define RMT_CH2_RX_END_INT_ENA_M  (RMT_CH2_RX_END_INT_ENA_V << RMT_CH2_RX_END_INT_ENA_S)
#define RMT_CH2_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH2_RX_END_INT_ENA_S  7
/** RMT_CH2_ERR_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enabled bit for RMT_CH2_ERR_INT.
 */
#define RMT_CH2_ERR_INT_ENA    (BIT(8))
#define RMT_CH2_ERR_INT_ENA_M  (RMT_CH2_ERR_INT_ENA_V << RMT_CH2_ERR_INT_ENA_S)
#define RMT_CH2_ERR_INT_ENA_V  0x00000001U
#define RMT_CH2_ERR_INT_ENA_S  8
/** RMT_CH3_TX_END_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enabled bit for RMT_CH3_TX_END_INT.
 */
#define RMT_CH3_TX_END_INT_ENA    (BIT(9))
#define RMT_CH3_TX_END_INT_ENA_M  (RMT_CH3_TX_END_INT_ENA_V << RMT_CH3_TX_END_INT_ENA_S)
#define RMT_CH3_TX_END_INT_ENA_V  0x00000001U
#define RMT_CH3_TX_END_INT_ENA_S  9
/** RMT_CH3_RX_END_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enabled bit for RMT_CH3_RX_END_INT.
 */
#define RMT_CH3_RX_END_INT_ENA    (BIT(10))
#define RMT_CH3_RX_END_INT_ENA_M  (RMT_CH3_RX_END_INT_ENA_V << RMT_CH3_RX_END_INT_ENA_S)
#define RMT_CH3_RX_END_INT_ENA_V  0x00000001U
#define RMT_CH3_RX_END_INT_ENA_S  10
/** RMT_CH3_ERR_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enabled bit for RMT_CH3_ERR_INT.
 */
#define RMT_CH3_ERR_INT_ENA    (BIT(11))
#define RMT_CH3_ERR_INT_ENA_M  (RMT_CH3_ERR_INT_ENA_V << RMT_CH3_ERR_INT_ENA_S)
#define RMT_CH3_ERR_INT_ENA_V  0x00000001U
#define RMT_CH3_ERR_INT_ENA_S  11
/** RMT_CH0_TX_THR_EVENT_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enabled bit for RMT_CH0_TX_THR_EVENT_INT.
 */
#define RMT_CH0_TX_THR_EVENT_INT_ENA    (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_ENA_M  (RMT_CH0_TX_THR_EVENT_INT_ENA_V << RMT_CH0_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH0_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_ENA_S  12
/** RMT_CH1_TX_THR_EVENT_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The interrupt enabled bit for RMT_CH1_TX_THR_EVENT_INT.
 */
#define RMT_CH1_TX_THR_EVENT_INT_ENA    (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_ENA_M  (RMT_CH1_TX_THR_EVENT_INT_ENA_V << RMT_CH1_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH1_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_ENA_S  13
/** RMT_CH2_TX_THR_EVENT_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  The interrupt enabled bit for RMT_CH2_TX_THR_EVENT_INT.
 */
#define RMT_CH2_TX_THR_EVENT_INT_ENA    (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_ENA_M  (RMT_CH2_TX_THR_EVENT_INT_ENA_V << RMT_CH2_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH2_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_ENA_S  14
/** RMT_CH3_TX_THR_EVENT_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  The interrupt enabled bit for RMT_CH3_TX_THR_EVENT_INT.
 */
#define RMT_CH3_TX_THR_EVENT_INT_ENA    (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_ENA_M  (RMT_CH3_TX_THR_EVENT_INT_ENA_V << RMT_CH3_TX_THR_EVENT_INT_ENA_S)
#define RMT_CH3_TX_THR_EVENT_INT_ENA_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_ENA_S  15
/** RMT_CH0_TX_LOOP_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  The interrupt enabled bit for RMT_CH0_TX_LOOP_INT.
 */
#define RMT_CH0_TX_LOOP_INT_ENA    (BIT(16))
#define RMT_CH0_TX_LOOP_INT_ENA_M  (RMT_CH0_TX_LOOP_INT_ENA_V << RMT_CH0_TX_LOOP_INT_ENA_S)
#define RMT_CH0_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_ENA_S  16
/** RMT_CH1_TX_LOOP_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  The interrupt enabled bit for RMT_CH1_TX_LOOP_INT.
 */
#define RMT_CH1_TX_LOOP_INT_ENA    (BIT(17))
#define RMT_CH1_TX_LOOP_INT_ENA_M  (RMT_CH1_TX_LOOP_INT_ENA_V << RMT_CH1_TX_LOOP_INT_ENA_S)
#define RMT_CH1_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_ENA_S  17
/** RMT_CH2_TX_LOOP_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  The interrupt enabled bit for RMT_CH2_TX_LOOP_INT.
 */
#define RMT_CH2_TX_LOOP_INT_ENA    (BIT(18))
#define RMT_CH2_TX_LOOP_INT_ENA_M  (RMT_CH2_TX_LOOP_INT_ENA_V << RMT_CH2_TX_LOOP_INT_ENA_S)
#define RMT_CH2_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_ENA_S  18
/** RMT_CH3_TX_LOOP_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  The interrupt enabled bit for RMT_CH3_TX_LOOP_INT.
 */
#define RMT_CH3_TX_LOOP_INT_ENA    (BIT(19))
#define RMT_CH3_TX_LOOP_INT_ENA_M  (RMT_CH3_TX_LOOP_INT_ENA_V << RMT_CH3_TX_LOOP_INT_ENA_S)
#define RMT_CH3_TX_LOOP_INT_ENA_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_ENA_S  19

/** RMT_INT_CLR_REG register
 *  Interrupt clear register
 */
#define RMT_INT_CLR_REG (DR_REG_RMT_BASE + 0x5c)
/** RMT_CH0_TX_END_INT_CLR : WO; bitpos: [0]; default: 0;
 *  Set this bit to clear RMT_CH0_TX_END_INT interrupt.
 */
#define RMT_CH0_TX_END_INT_CLR    (BIT(0))
#define RMT_CH0_TX_END_INT_CLR_M  (RMT_CH0_TX_END_INT_CLR_V << RMT_CH0_TX_END_INT_CLR_S)
#define RMT_CH0_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH0_TX_END_INT_CLR_S  0
/** RMT_CH0_RX_END_INT_CLR : WO; bitpos: [1]; default: 0;
 *  Set this bit to clear RMT_CH0_RX_END_INT interrupt.
 */
#define RMT_CH0_RX_END_INT_CLR    (BIT(1))
#define RMT_CH0_RX_END_INT_CLR_M  (RMT_CH0_RX_END_INT_CLR_V << RMT_CH0_RX_END_INT_CLR_S)
#define RMT_CH0_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH0_RX_END_INT_CLR_S  1
/** RMT_CH0_ERR_INT_CLR : WO; bitpos: [2]; default: 0;
 *  Set this bit to clear RMT_CH0_ERR_INT interrupt.
 */
#define RMT_CH0_ERR_INT_CLR    (BIT(2))
#define RMT_CH0_ERR_INT_CLR_M  (RMT_CH0_ERR_INT_CLR_V << RMT_CH0_ERR_INT_CLR_S)
#define RMT_CH0_ERR_INT_CLR_V  0x00000001U
#define RMT_CH0_ERR_INT_CLR_S  2
/** RMT_CH1_TX_END_INT_CLR : WO; bitpos: [3]; default: 0;
 *  Set this bit to clear RMT_CH1_TX_END_INT interrupt.
 */
#define RMT_CH1_TX_END_INT_CLR    (BIT(3))
#define RMT_CH1_TX_END_INT_CLR_M  (RMT_CH1_TX_END_INT_CLR_V << RMT_CH1_TX_END_INT_CLR_S)
#define RMT_CH1_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH1_TX_END_INT_CLR_S  3
/** RMT_CH1_RX_END_INT_CLR : WO; bitpos: [4]; default: 0;
 *  Set this bit to clear RMT_CH1_RX_END_INT interrupt.
 */
#define RMT_CH1_RX_END_INT_CLR    (BIT(4))
#define RMT_CH1_RX_END_INT_CLR_M  (RMT_CH1_RX_END_INT_CLR_V << RMT_CH1_RX_END_INT_CLR_S)
#define RMT_CH1_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH1_RX_END_INT_CLR_S  4
/** RMT_CH1_ERR_INT_CLR : WO; bitpos: [5]; default: 0;
 *  Set this bit to clear RMT_CH1_ERR_INT interrupt.
 */
#define RMT_CH1_ERR_INT_CLR    (BIT(5))
#define RMT_CH1_ERR_INT_CLR_M  (RMT_CH1_ERR_INT_CLR_V << RMT_CH1_ERR_INT_CLR_S)
#define RMT_CH1_ERR_INT_CLR_V  0x00000001U
#define RMT_CH1_ERR_INT_CLR_S  5
/** RMT_CH2_TX_END_INT_CLR : WO; bitpos: [6]; default: 0;
 *  Set this bit to clear RMT_CH2_TX_END_INT interrupt.
 */
#define RMT_CH2_TX_END_INT_CLR    (BIT(6))
#define RMT_CH2_TX_END_INT_CLR_M  (RMT_CH2_TX_END_INT_CLR_V << RMT_CH2_TX_END_INT_CLR_S)
#define RMT_CH2_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH2_TX_END_INT_CLR_S  6
/** RMT_CH2_RX_END_INT_CLR : WO; bitpos: [7]; default: 0;
 *  Set this bit to clear RMT_CH2_RX_END_INT interrupt.
 */
#define RMT_CH2_RX_END_INT_CLR    (BIT(7))
#define RMT_CH2_RX_END_INT_CLR_M  (RMT_CH2_RX_END_INT_CLR_V << RMT_CH2_RX_END_INT_CLR_S)
#define RMT_CH2_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH2_RX_END_INT_CLR_S  7
/** RMT_CH2_ERR_INT_CLR : WO; bitpos: [8]; default: 0;
 *  Set this bit to clear RMT_CH2_ERR_INT interrupt.
 */
#define RMT_CH2_ERR_INT_CLR    (BIT(8))
#define RMT_CH2_ERR_INT_CLR_M  (RMT_CH2_ERR_INT_CLR_V << RMT_CH2_ERR_INT_CLR_S)
#define RMT_CH2_ERR_INT_CLR_V  0x00000001U
#define RMT_CH2_ERR_INT_CLR_S  8
/** RMT_CH3_TX_END_INT_CLR : WO; bitpos: [9]; default: 0;
 *  Set this bit to clear RMT_CH3_TX_END_INT interrupt.
 */
#define RMT_CH3_TX_END_INT_CLR    (BIT(9))
#define RMT_CH3_TX_END_INT_CLR_M  (RMT_CH3_TX_END_INT_CLR_V << RMT_CH3_TX_END_INT_CLR_S)
#define RMT_CH3_TX_END_INT_CLR_V  0x00000001U
#define RMT_CH3_TX_END_INT_CLR_S  9
/** RMT_CH3_RX_END_INT_CLR : WO; bitpos: [10]; default: 0;
 *  Set this bit to clear RMT_CH3_RX_END_INT interrupt.
 */
#define RMT_CH3_RX_END_INT_CLR    (BIT(10))
#define RMT_CH3_RX_END_INT_CLR_M  (RMT_CH3_RX_END_INT_CLR_V << RMT_CH3_RX_END_INT_CLR_S)
#define RMT_CH3_RX_END_INT_CLR_V  0x00000001U
#define RMT_CH3_RX_END_INT_CLR_S  10
/** RMT_CH3_ERR_INT_CLR : WO; bitpos: [11]; default: 0;
 *  Set this bit to clear RMT_CH3_ERR_INT interrupt.
 */
#define RMT_CH3_ERR_INT_CLR    (BIT(11))
#define RMT_CH3_ERR_INT_CLR_M  (RMT_CH3_ERR_INT_CLR_V << RMT_CH3_ERR_INT_CLR_S)
#define RMT_CH3_ERR_INT_CLR_V  0x00000001U
#define RMT_CH3_ERR_INT_CLR_S  11
/** RMT_CH0_TX_THR_EVENT_INT_CLR : WO; bitpos: [12]; default: 0;
 *  Set this bit to clear RMT_CH0_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH0_TX_THR_EVENT_INT_CLR    (BIT(12))
#define RMT_CH0_TX_THR_EVENT_INT_CLR_M  (RMT_CH0_TX_THR_EVENT_INT_CLR_V << RMT_CH0_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH0_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH0_TX_THR_EVENT_INT_CLR_S  12
/** RMT_CH1_TX_THR_EVENT_INT_CLR : WO; bitpos: [13]; default: 0;
 *  Set this bit to clear RMT_CH1_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH1_TX_THR_EVENT_INT_CLR    (BIT(13))
#define RMT_CH1_TX_THR_EVENT_INT_CLR_M  (RMT_CH1_TX_THR_EVENT_INT_CLR_V << RMT_CH1_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH1_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH1_TX_THR_EVENT_INT_CLR_S  13
/** RMT_CH2_TX_THR_EVENT_INT_CLR : WO; bitpos: [14]; default: 0;
 *  Set this bit to clear RMT_CH2_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH2_TX_THR_EVENT_INT_CLR    (BIT(14))
#define RMT_CH2_TX_THR_EVENT_INT_CLR_M  (RMT_CH2_TX_THR_EVENT_INT_CLR_V << RMT_CH2_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH2_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH2_TX_THR_EVENT_INT_CLR_S  14
/** RMT_CH3_TX_THR_EVENT_INT_CLR : WO; bitpos: [15]; default: 0;
 *  Set this bit to clear RMT_CH3_TX_THR_EVENT_INT interrupt.
 */
#define RMT_CH3_TX_THR_EVENT_INT_CLR    (BIT(15))
#define RMT_CH3_TX_THR_EVENT_INT_CLR_M  (RMT_CH3_TX_THR_EVENT_INT_CLR_V << RMT_CH3_TX_THR_EVENT_INT_CLR_S)
#define RMT_CH3_TX_THR_EVENT_INT_CLR_V  0x00000001U
#define RMT_CH3_TX_THR_EVENT_INT_CLR_S  15
/** RMT_CH0_TX_LOOP_INT_CLR : WO; bitpos: [16]; default: 0;
 *  Set this bit to clear RMT_CH0_TX_LOOP_INT interrupt.
 */
#define RMT_CH0_TX_LOOP_INT_CLR    (BIT(16))
#define RMT_CH0_TX_LOOP_INT_CLR_M  (RMT_CH0_TX_LOOP_INT_CLR_V << RMT_CH0_TX_LOOP_INT_CLR_S)
#define RMT_CH0_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH0_TX_LOOP_INT_CLR_S  16
/** RMT_CH1_TX_LOOP_INT_CLR : WO; bitpos: [17]; default: 0;
 *  Set this bit to clear RMT_CH1_TX_LOOP_INT interrupt.
 */
#define RMT_CH1_TX_LOOP_INT_CLR    (BIT(17))
#define RMT_CH1_TX_LOOP_INT_CLR_M  (RMT_CH1_TX_LOOP_INT_CLR_V << RMT_CH1_TX_LOOP_INT_CLR_S)
#define RMT_CH1_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH1_TX_LOOP_INT_CLR_S  17
/** RMT_CH2_TX_LOOP_INT_CLR : WO; bitpos: [18]; default: 0;
 *  Set this bit to clear RMT_CH2_TX_LOOP_INT interrupt.
 */
#define RMT_CH2_TX_LOOP_INT_CLR    (BIT(18))
#define RMT_CH2_TX_LOOP_INT_CLR_M  (RMT_CH2_TX_LOOP_INT_CLR_V << RMT_CH2_TX_LOOP_INT_CLR_S)
#define RMT_CH2_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH2_TX_LOOP_INT_CLR_S  18
/** RMT_CH3_TX_LOOP_INT_CLR : WO; bitpos: [19]; default: 0;
 *  Set this bit to clear RMT_CH3_TX_LOOP_INT interrupt.
 */
#define RMT_CH3_TX_LOOP_INT_CLR    (BIT(19))
#define RMT_CH3_TX_LOOP_INT_CLR_M  (RMT_CH3_TX_LOOP_INT_CLR_V << RMT_CH3_TX_LOOP_INT_CLR_S)
#define RMT_CH3_TX_LOOP_INT_CLR_V  0x00000001U
#define RMT_CH3_TX_LOOP_INT_CLR_S  19

/** RMT_CH0CARRIER_DUTY_REG register
 *  Channel 0 duty cycle configuration register
 */
#define RMT_CH0CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x60)
/** RMT_CARRIER_LOW_CH0 : R/W; bitpos: [15:0]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at low level for
 *  channel 0.
 */
#define RMT_CARRIER_LOW_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_M  (RMT_CARRIER_LOW_CH0_V << RMT_CARRIER_LOW_CH0_S)
#define RMT_CARRIER_LOW_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_S  0
/** RMT_CARRIER_HIGH_CH0 : R/W; bitpos: [31:16]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at high level for
 *  channel 0.
 */
#define RMT_CARRIER_HIGH_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_M  (RMT_CARRIER_HIGH_CH0_V << RMT_CARRIER_HIGH_CH0_S)
#define RMT_CARRIER_HIGH_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_S  16

/** RMT_CH1CARRIER_DUTY_REG register
 *  Channel 1 duty cycle configuration register
 */
#define RMT_CH1CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x64)
/** RMT_CARRIER_LOW_CH0 : R/W; bitpos: [15:0]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at low level for
 *  channel 1.
 */
#define RMT_CARRIER_LOW_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_M  (RMT_CARRIER_LOW_CH0_V << RMT_CARRIER_LOW_CH0_S)
#define RMT_CARRIER_LOW_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_S  0
/** RMT_CARRIER_HIGH_CH0 : R/W; bitpos: [31:16]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at high level for
 *  channel 1.
 */
#define RMT_CARRIER_HIGH_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_M  (RMT_CARRIER_HIGH_CH0_V << RMT_CARRIER_HIGH_CH0_S)
#define RMT_CARRIER_HIGH_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_S  16

/** RMT_CH2CARRIER_DUTY_REG register
 *  Channel 2 duty cycle configuration register
 */
#define RMT_CH2CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x68)
/** RMT_CARRIER_LOW_CH0 : R/W; bitpos: [15:0]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at low level for
 *  channel 2.
 */
#define RMT_CARRIER_LOW_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_M  (RMT_CARRIER_LOW_CH0_V << RMT_CARRIER_LOW_CH0_S)
#define RMT_CARRIER_LOW_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_S  0
/** RMT_CARRIER_HIGH_CH0 : R/W; bitpos: [31:16]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at high level for
 *  channel 2.
 */
#define RMT_CARRIER_HIGH_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_M  (RMT_CARRIER_HIGH_CH0_V << RMT_CARRIER_HIGH_CH0_S)
#define RMT_CARRIER_HIGH_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_S  16

/** RMT_CH3CARRIER_DUTY_REG register
 *  Channel 3 duty cycle configuration register
 */
#define RMT_CH3CARRIER_DUTY_REG (DR_REG_RMT_BASE + 0x6c)
/** RMT_CARRIER_LOW_CH0 : R/W; bitpos: [15:0]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at low level for
 *  channel 3.
 */
#define RMT_CARRIER_LOW_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_M  (RMT_CARRIER_LOW_CH0_V << RMT_CARRIER_LOW_CH0_S)
#define RMT_CARRIER_LOW_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_CH0_S  0
/** RMT_CARRIER_HIGH_CH0 : R/W; bitpos: [31:16]; default: 64;
 *  This field is used to configure the clock cycles of carrier wave at high level for
 *  channel 3.
 */
#define RMT_CARRIER_HIGH_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_M  (RMT_CARRIER_HIGH_CH0_V << RMT_CARRIER_HIGH_CH0_S)
#define RMT_CARRIER_HIGH_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_CH0_S  16

/** RMT_CH0_TX_LIM_REG register
 *  Channel 0 Tx event configuration register
 */
#define RMT_CH0_TX_LIM_REG (DR_REG_RMT_BASE + 0x70)
/** RMT_TX_LIM_CH0 : R/W; bitpos: [8:0]; default: 128;
 *  This field is used to configure the maximum entries that channel 0 can send out.
 *  When RMT_MEM_SIZE_CH0 = 1, this field can be set to any value among 0 ~ 128
 *  (64*32/16 = 128). When RMT_MEM_SIZE_CH0 > 1, this field can be set to any value
 *  among (0 ~ 128)*RMT_MEM_SIZE_CH0.
 */
#define RMT_TX_LIM_CH0    0x000001FFU
#define RMT_TX_LIM_CH0_M  (RMT_TX_LIM_CH0_V << RMT_TX_LIM_CH0_S)
#define RMT_TX_LIM_CH0_V  0x000001FFU
#define RMT_TX_LIM_CH0_S  0
/** RMT_TX_LOOP_NUM_CH0 : R/W; bitpos: [18:9]; default: 0;
 *  This field is used to configure the maximum loop times when continuous transmission
 *  mode is enabled.
 */
#define RMT_TX_LOOP_NUM_CH0    0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_M  (RMT_TX_LOOP_NUM_CH0_V << RMT_TX_LOOP_NUM_CH0_S)
#define RMT_TX_LOOP_NUM_CH0_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_S  9
/** RMT_TX_LOOP_CNT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This bit is used to enable loop counting.
 */
#define RMT_TX_LOOP_CNT_EN_CH0    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH0_M  (RMT_TX_LOOP_CNT_EN_CH0_V << RMT_TX_LOOP_CNT_EN_CH0_S)
#define RMT_TX_LOOP_CNT_EN_CH0_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH0_S  19
/** RMT_LOOP_COUNT_RESET_CH0 : WO; bitpos: [20]; default: 0;
 *  This bit is used to reset loop counting when continuous transmission mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH0    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH0_M  (RMT_LOOP_COUNT_RESET_CH0_V << RMT_LOOP_COUNT_RESET_CH0_S)
#define RMT_LOOP_COUNT_RESET_CH0_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH0_S  20

/** RMT_CH1_TX_LIM_REG register
 *  Channel 1 Tx event configuration register
 */
#define RMT_CH1_TX_LIM_REG (DR_REG_RMT_BASE + 0x74)
/** RMT_TX_LIM_CH0 : R/W; bitpos: [8:0]; default: 128;
 *  This field is used to configure the maximum entries that channel 1 can send out.
 *  When RMT_MEM_SIZE_CH1 = 1, this field can be set to any value among 0 ~ 128
 *  (64*32/16 = 128). When RMT_MEM_SIZE_CH1 > 1, this field can be set to any value
 *  among (0 ~ 128)*RMT_MEM_SIZE_CH1.
 */
#define RMT_TX_LIM_CH0    0x000001FFU
#define RMT_TX_LIM_CH0_M  (RMT_TX_LIM_CH0_V << RMT_TX_LIM_CH0_S)
#define RMT_TX_LIM_CH0_V  0x000001FFU
#define RMT_TX_LIM_CH0_S  0
/** RMT_TX_LOOP_NUM_CH0 : R/W; bitpos: [18:9]; default: 0;
 *  This field is used to configure the maximum loop times when continuous transmission
 *  mode is enabled.
 */
#define RMT_TX_LOOP_NUM_CH0    0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_M  (RMT_TX_LOOP_NUM_CH0_V << RMT_TX_LOOP_NUM_CH0_S)
#define RMT_TX_LOOP_NUM_CH0_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_S  9
/** RMT_TX_LOOP_CNT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This bit is used to enable loop counting.
 */
#define RMT_TX_LOOP_CNT_EN_CH0    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH0_M  (RMT_TX_LOOP_CNT_EN_CH0_V << RMT_TX_LOOP_CNT_EN_CH0_S)
#define RMT_TX_LOOP_CNT_EN_CH0_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH0_S  19
/** RMT_LOOP_COUNT_RESET_CH0 : WO; bitpos: [20]; default: 0;
 *  This bit is used to reset loop counting when continuous transmission mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH0    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH0_M  (RMT_LOOP_COUNT_RESET_CH0_V << RMT_LOOP_COUNT_RESET_CH0_S)
#define RMT_LOOP_COUNT_RESET_CH0_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH0_S  20

/** RMT_CH2_TX_LIM_REG register
 *  Channel 2 Tx event configuration register
 */
#define RMT_CH2_TX_LIM_REG (DR_REG_RMT_BASE + 0x78)
/** RMT_TX_LIM_CH0 : R/W; bitpos: [8:0]; default: 128;
 *  This field is used to configure the maximum entries that channel 2 can send out.
 *  When RMT_MEM_SIZE_CH2 = 1, this field can be set to any value among 0 ~ 128
 *  (64*32/16 = 128). When RMT_MEM_SIZE_CH2 > 1, this field can be set to any value
 *  among (0 ~ 128)*RMT_MEM_SIZE_CH2.
 */
#define RMT_TX_LIM_CH0    0x000001FFU
#define RMT_TX_LIM_CH0_M  (RMT_TX_LIM_CH0_V << RMT_TX_LIM_CH0_S)
#define RMT_TX_LIM_CH0_V  0x000001FFU
#define RMT_TX_LIM_CH0_S  0
/** RMT_TX_LOOP_NUM_CH0 : R/W; bitpos: [18:9]; default: 0;
 *  This field is used to configure the maximum loop times when continuous transmission
 *  mode is enabled.
 */
#define RMT_TX_LOOP_NUM_CH0    0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_M  (RMT_TX_LOOP_NUM_CH0_V << RMT_TX_LOOP_NUM_CH0_S)
#define RMT_TX_LOOP_NUM_CH0_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_S  9
/** RMT_TX_LOOP_CNT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This bit is used to enable loop counting.
 */
#define RMT_TX_LOOP_CNT_EN_CH0    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH0_M  (RMT_TX_LOOP_CNT_EN_CH0_V << RMT_TX_LOOP_CNT_EN_CH0_S)
#define RMT_TX_LOOP_CNT_EN_CH0_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH0_S  19
/** RMT_LOOP_COUNT_RESET_CH0 : WO; bitpos: [20]; default: 0;
 *  This bit is used to reset loop counting when continuous transmission mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH0    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH0_M  (RMT_LOOP_COUNT_RESET_CH0_V << RMT_LOOP_COUNT_RESET_CH0_S)
#define RMT_LOOP_COUNT_RESET_CH0_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH0_S  20

/** RMT_CH3_TX_LIM_REG register
 *  Channel 3 Tx event configuration register
 */
#define RMT_CH3_TX_LIM_REG (DR_REG_RMT_BASE + 0x7c)
/** RMT_TX_LIM_CH0 : R/W; bitpos: [8:0]; default: 128;
 *  This field is used to configure the maximum entries that channel 3 can send out.
 *  When RMT_MEM_SIZE_CH3 = 1, this field can be set to any value among 0 ~ 128
 *  (64*32/16 = 128). When RMT_MEM_SIZE_CH3 > 1, this field can be set to any value
 *  among (0 ~ 128)*RMT_MEM_SIZE_CH3.
 */
#define RMT_TX_LIM_CH0    0x000001FFU
#define RMT_TX_LIM_CH0_M  (RMT_TX_LIM_CH0_V << RMT_TX_LIM_CH0_S)
#define RMT_TX_LIM_CH0_V  0x000001FFU
#define RMT_TX_LIM_CH0_S  0
/** RMT_TX_LOOP_NUM_CH0 : R/W; bitpos: [18:9]; default: 0;
 *  This field is used to configure the maximum loop times when continuous transmission
 *  mode is enabled.
 */
#define RMT_TX_LOOP_NUM_CH0    0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_M  (RMT_TX_LOOP_NUM_CH0_V << RMT_TX_LOOP_NUM_CH0_S)
#define RMT_TX_LOOP_NUM_CH0_V  0x000003FFU
#define RMT_TX_LOOP_NUM_CH0_S  9
/** RMT_TX_LOOP_CNT_EN_CH0 : R/W; bitpos: [19]; default: 0;
 *  This bit is used to enable loop counting.
 */
#define RMT_TX_LOOP_CNT_EN_CH0    (BIT(19))
#define RMT_TX_LOOP_CNT_EN_CH0_M  (RMT_TX_LOOP_CNT_EN_CH0_V << RMT_TX_LOOP_CNT_EN_CH0_S)
#define RMT_TX_LOOP_CNT_EN_CH0_V  0x00000001U
#define RMT_TX_LOOP_CNT_EN_CH0_S  19
/** RMT_LOOP_COUNT_RESET_CH0 : WO; bitpos: [20]; default: 0;
 *  This bit is used to reset loop counting when continuous transmission mode is valid.
 */
#define RMT_LOOP_COUNT_RESET_CH0    (BIT(20))
#define RMT_LOOP_COUNT_RESET_CH0_M  (RMT_LOOP_COUNT_RESET_CH0_V << RMT_LOOP_COUNT_RESET_CH0_S)
#define RMT_LOOP_COUNT_RESET_CH0_V  0x00000001U
#define RMT_LOOP_COUNT_RESET_CH0_S  20

/** RMT_APB_CONF_REG register
 *  RMT APB configuration register
 */
#define RMT_APB_CONF_REG (DR_REG_RMT_BASE + 0x80)
/** RMT_APB_FIFO_MASK : R/W; bitpos: [0]; default: 0;
 *  1'h1: Access memory directly.  1'h0: access memory via APB FIFO.
 */
#define RMT_APB_FIFO_MASK    (BIT(0))
#define RMT_APB_FIFO_MASK_M  (RMT_APB_FIFO_MASK_V << RMT_APB_FIFO_MASK_S)
#define RMT_APB_FIFO_MASK_V  0x00000001U
#define RMT_APB_FIFO_MASK_S  0
/** RMT_MEM_TX_WRAP_EN : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable wrap mode.
 */
#define RMT_MEM_TX_WRAP_EN    (BIT(1))
#define RMT_MEM_TX_WRAP_EN_M  (RMT_MEM_TX_WRAP_EN_V << RMT_MEM_TX_WRAP_EN_S)
#define RMT_MEM_TX_WRAP_EN_V  0x00000001U
#define RMT_MEM_TX_WRAP_EN_S  1
/** RMT_MEM_CLK_FORCE_ON : R/W; bitpos: [2]; default: 1;
 *  Set this bit to enable the clock for RAM when RMT module starts working, disable
 *  this clock when RMT stops working, to achieve low-power scheme.
 */
#define RMT_MEM_CLK_FORCE_ON    (BIT(2))
#define RMT_MEM_CLK_FORCE_ON_M  (RMT_MEM_CLK_FORCE_ON_V << RMT_MEM_CLK_FORCE_ON_S)
#define RMT_MEM_CLK_FORCE_ON_V  0x00000001U
#define RMT_MEM_CLK_FORCE_ON_S  2
/** RMT_MEM_FORCE_PD : R/W; bitpos: [3]; default: 0;
 *  Set this bit to power down RMT memory.
 */
#define RMT_MEM_FORCE_PD    (BIT(3))
#define RMT_MEM_FORCE_PD_M  (RMT_MEM_FORCE_PD_V << RMT_MEM_FORCE_PD_S)
#define RMT_MEM_FORCE_PD_V  0x00000001U
#define RMT_MEM_FORCE_PD_S  3
/** RMT_MEM_FORCE_PU : R/W; bitpos: [4]; default: 0;
 *  1: Disable RAM's Light-sleep power down function. 0: power down RMT RAM when RMT is
 *  in Light-sleep mode.
 */
#define RMT_MEM_FORCE_PU    (BIT(4))
#define RMT_MEM_FORCE_PU_M  (RMT_MEM_FORCE_PU_V << RMT_MEM_FORCE_PU_S)
#define RMT_MEM_FORCE_PU_V  0x00000001U
#define RMT_MEM_FORCE_PU_S  4
/** RMT_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Clock gating enable bit for RMT registers to achieve low-power scheme. 1: Power up
 *  drive clock for RMT registers. 0: Power down drive clock for RMT registers.
 */
#define RMT_CLK_EN    (BIT(31))
#define RMT_CLK_EN_M  (RMT_CLK_EN_V << RMT_CLK_EN_S)
#define RMT_CLK_EN_V  0x00000001U
#define RMT_CLK_EN_S  31

/** RMT_TX_SIM_REG register
 *  Enable RMT simultaneous transmission
 */
#define RMT_TX_SIM_REG (DR_REG_RMT_BASE + 0x84)
/** RMT_TX_SIM_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable channel 0 to start sending data simultaneously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH0    (BIT(0))
#define RMT_TX_SIM_CH0_M  (RMT_TX_SIM_CH0_V << RMT_TX_SIM_CH0_S)
#define RMT_TX_SIM_CH0_V  0x00000001U
#define RMT_TX_SIM_CH0_S  0
/** RMT_TX_SIM_CH1 : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable channel 1 to start sending data simultaneously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH1    (BIT(1))
#define RMT_TX_SIM_CH1_M  (RMT_TX_SIM_CH1_V << RMT_TX_SIM_CH1_S)
#define RMT_TX_SIM_CH1_V  0x00000001U
#define RMT_TX_SIM_CH1_S  1
/** RMT_TX_SIM_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable channel 2 to start sending data simultaneously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH2    (BIT(2))
#define RMT_TX_SIM_CH2_M  (RMT_TX_SIM_CH2_V << RMT_TX_SIM_CH2_S)
#define RMT_TX_SIM_CH2_V  0x00000001U
#define RMT_TX_SIM_CH2_S  2
/** RMT_TX_SIM_CH3 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to enable channel 3 to start sending data simultaneously with other
 *  enabled channels.
 */
#define RMT_TX_SIM_CH3    (BIT(3))
#define RMT_TX_SIM_CH3_M  (RMT_TX_SIM_CH3_V << RMT_TX_SIM_CH3_S)
#define RMT_TX_SIM_CH3_V  0x00000001U
#define RMT_TX_SIM_CH3_S  3
/** RMT_TX_SIM_EN : R/W; bitpos: [4]; default: 0;
 *  This bit is used to enable multiple channels to start sending data simultaneously.
 */
#define RMT_TX_SIM_EN    (BIT(4))
#define RMT_TX_SIM_EN_M  (RMT_TX_SIM_EN_V << RMT_TX_SIM_EN_S)
#define RMT_TX_SIM_EN_V  0x00000001U
#define RMT_TX_SIM_EN_S  4

/** RMT_REF_CNT_RST_REG register
 *  RMT clock divider reset register
 */
#define RMT_REF_CNT_RST_REG (DR_REG_RMT_BASE + 0x88)
/** RMT_REF_CNT_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset the clock divider of channel 0.
 */
#define RMT_REF_CNT_RST_CH0    (BIT(0))
#define RMT_REF_CNT_RST_CH0_M  (RMT_REF_CNT_RST_CH0_V << RMT_REF_CNT_RST_CH0_S)
#define RMT_REF_CNT_RST_CH0_V  0x00000001U
#define RMT_REF_CNT_RST_CH0_S  0
/** RMT_REF_CNT_RST_CH1 : R/W; bitpos: [1]; default: 0;
 *  This bit is used to reset the clock divider of channel 1.
 */
#define RMT_REF_CNT_RST_CH1    (BIT(1))
#define RMT_REF_CNT_RST_CH1_M  (RMT_REF_CNT_RST_CH1_V << RMT_REF_CNT_RST_CH1_S)
#define RMT_REF_CNT_RST_CH1_V  0x00000001U
#define RMT_REF_CNT_RST_CH1_S  1
/** RMT_REF_CNT_RST_CH2 : R/W; bitpos: [2]; default: 0;
 *  This bit is used to reset the clock divider of channel 2.
 */
#define RMT_REF_CNT_RST_CH2    (BIT(2))
#define RMT_REF_CNT_RST_CH2_M  (RMT_REF_CNT_RST_CH2_V << RMT_REF_CNT_RST_CH2_S)
#define RMT_REF_CNT_RST_CH2_V  0x00000001U
#define RMT_REF_CNT_RST_CH2_S  2
/** RMT_REF_CNT_RST_CH3 : R/W; bitpos: [3]; default: 0;
 *  This bit is used to reset the clock divider of channel 3.
 */
#define RMT_REF_CNT_RST_CH3    (BIT(3))
#define RMT_REF_CNT_RST_CH3_M  (RMT_REF_CNT_RST_CH3_V << RMT_REF_CNT_RST_CH3_S)
#define RMT_REF_CNT_RST_CH3_V  0x00000001U
#define RMT_REF_CNT_RST_CH3_S  3

/** RMT_CH0_RX_CARRIER_RM_REG register
 *  Channel 0 carrier remove register
 */
#define RMT_CH0_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x8c)
/** RMT_CARRIER_LOW_THRES_CH0 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in carrier modulation mode is (RMT_CARRIER_LOW_THRES_CH0 + 1)
 *  clock cycles for channel 0.
 */
#define RMT_CARRIER_LOW_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_M  (RMT_CARRIER_LOW_THRES_CH0_V << RMT_CARRIER_LOW_THRES_CH0_S)
#define RMT_CARRIER_LOW_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_S  0
/** RMT_CARRIER_HIGH_THRES_CH0 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in carrier modulation mode is (RMT_CARRIER_HIGH_THRES_CH0 +
 *  1) clock cycles for channel 0.
 */
#define RMT_CARRIER_HIGH_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_M  (RMT_CARRIER_HIGH_THRES_CH0_V << RMT_CARRIER_HIGH_THRES_CH0_S)
#define RMT_CARRIER_HIGH_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_S  16

/** RMT_CH1_RX_CARRIER_RM_REG register
 *  Channel 1 carrier remove register
 */
#define RMT_CH1_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x90)
/** RMT_CARRIER_LOW_THRES_CH0 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in carrier modulation mode is (RMT_CARRIER_LOW_THRES_CH1 + 1)
 *  clock cycles for channel 1.
 */
#define RMT_CARRIER_LOW_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_M  (RMT_CARRIER_LOW_THRES_CH0_V << RMT_CARRIER_LOW_THRES_CH0_S)
#define RMT_CARRIER_LOW_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_S  0
/** RMT_CARRIER_HIGH_THRES_CH0 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in carrier modulation mode is (RMT_CARRIER_HIGH_THRES_CH1 +
 *  1) clock cycles for channel 1.
 */
#define RMT_CARRIER_HIGH_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_M  (RMT_CARRIER_HIGH_THRES_CH0_V << RMT_CARRIER_HIGH_THRES_CH0_S)
#define RMT_CARRIER_HIGH_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_S  16

/** RMT_CH2_RX_CARRIER_RM_REG register
 *  Channel 2 carrier remove register
 */
#define RMT_CH2_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x94)
/** RMT_CARRIER_LOW_THRES_CH0 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in carrier modulation mode is (RMT_CARRIER_LOW_THRES_CH2 + 1)
 *  clock cycles for channel 2.
 */
#define RMT_CARRIER_LOW_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_M  (RMT_CARRIER_LOW_THRES_CH0_V << RMT_CARRIER_LOW_THRES_CH0_S)
#define RMT_CARRIER_LOW_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_S  0
/** RMT_CARRIER_HIGH_THRES_CH0 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in carrier modulation mode is (RMT_CARRIER_HIGH_THRES_CH2 +
 *  1) clock cycles for channel 2.
 */
#define RMT_CARRIER_HIGH_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_M  (RMT_CARRIER_HIGH_THRES_CH0_V << RMT_CARRIER_HIGH_THRES_CH0_S)
#define RMT_CARRIER_HIGH_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_S  16

/** RMT_CH3_RX_CARRIER_RM_REG register
 *  Channel 3 carrier remove register
 */
#define RMT_CH3_RX_CARRIER_RM_REG (DR_REG_RMT_BASE + 0x98)
/** RMT_CARRIER_LOW_THRES_CH0 : R/W; bitpos: [15:0]; default: 0;
 *  The low level period in carrier modulation mode is (RMT_CARRIER_LOW_THRES_CH3 + 1)
 *  clock cycles for channel 3.
 */
#define RMT_CARRIER_LOW_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_M  (RMT_CARRIER_LOW_THRES_CH0_V << RMT_CARRIER_LOW_THRES_CH0_S)
#define RMT_CARRIER_LOW_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_LOW_THRES_CH0_S  0
/** RMT_CARRIER_HIGH_THRES_CH0 : R/W; bitpos: [31:16]; default: 0;
 *  The high level period in carrier modulation mode is (RMT_CARRIER_HIGH_THRES_CH3 +
 *  1) clock cycles for channel 3.
 */
#define RMT_CARRIER_HIGH_THRES_CH0    0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_M  (RMT_CARRIER_HIGH_THRES_CH0_V << RMT_CARRIER_HIGH_THRES_CH0_S)
#define RMT_CARRIER_HIGH_THRES_CH0_V  0x0000FFFFU
#define RMT_CARRIER_HIGH_THRES_CH0_S  16

/** RMT_DATE_REG register
 *  Version control register
 */
#define RMT_DATE_REG (DR_REG_RMT_BASE + 0xfc)
/** RMT_DATE : R/W; bitpos: [31:0]; default: 419898881;
 *  Version control register
 */
#define RMT_DATE    0xFFFFFFFFU
#define RMT_DATE_M  (RMT_DATE_V << RMT_DATE_S)
#define RMT_DATE_V  0xFFFFFFFFU
#define RMT_DATE_S  0

#ifdef __cplusplus
}
#endif
