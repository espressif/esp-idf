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

/** RTC_I2C_SCL_LOW_REG register
 *  configure low scl period
 */
#define RTC_I2C_SCL_LOW_REG (DR_REG_RTC_BASE + 0x0)
/** RTC_I2C_SCL_LOW_PERIOD_REG : R/W; bitpos: [19:0]; default: 256;
 *  time period that scl =0
 */
#define RTC_I2C_SCL_LOW_PERIOD_REG    0x000FFFFFU
#define RTC_I2C_SCL_LOW_PERIOD_REG_M  (RTC_I2C_SCL_LOW_PERIOD_REG_V << RTC_I2C_SCL_LOW_PERIOD_REG_S)
#define RTC_I2C_SCL_LOW_PERIOD_REG_V  0x000FFFFFU
#define RTC_I2C_SCL_LOW_PERIOD_REG_S  0

/** RTC_I2C_CTRL_REG register
 *  configure i2c ctrl
 */
#define RTC_I2C_CTRL_REG (DR_REG_RTC_BASE + 0x4)
/** RTC_I2C_SDA_FORCE_OUT : R/W; bitpos: [0]; default: 0;
 *  1=push pull,0=open drain
 */
#define RTC_I2C_SDA_FORCE_OUT    (BIT(0))
#define RTC_I2C_SDA_FORCE_OUT_M  (RTC_I2C_SDA_FORCE_OUT_V << RTC_I2C_SDA_FORCE_OUT_S)
#define RTC_I2C_SDA_FORCE_OUT_V  0x00000001U
#define RTC_I2C_SDA_FORCE_OUT_S  0
/** RTC_I2C_SCL_FORCE_OUT : R/W; bitpos: [1]; default: 0;
 *  1=push pull,0=open drain
 */
#define RTC_I2C_SCL_FORCE_OUT    (BIT(1))
#define RTC_I2C_SCL_FORCE_OUT_M  (RTC_I2C_SCL_FORCE_OUT_V << RTC_I2C_SCL_FORCE_OUT_S)
#define RTC_I2C_SCL_FORCE_OUT_V  0x00000001U
#define RTC_I2C_SCL_FORCE_OUT_S  1
/** RTC_I2C_MS_MODE : R/W; bitpos: [2]; default: 0;
 *  1=master,0=slave
 */
#define RTC_I2C_MS_MODE    (BIT(2))
#define RTC_I2C_MS_MODE_M  (RTC_I2C_MS_MODE_V << RTC_I2C_MS_MODE_S)
#define RTC_I2C_MS_MODE_V  0x00000001U
#define RTC_I2C_MS_MODE_S  2
/** RTC_I2C_TRANS_START : R/W; bitpos: [3]; default: 0;
 *  force start
 */
#define RTC_I2C_TRANS_START    (BIT(3))
#define RTC_I2C_TRANS_START_M  (RTC_I2C_TRANS_START_V << RTC_I2C_TRANS_START_S)
#define RTC_I2C_TRANS_START_V  0x00000001U
#define RTC_I2C_TRANS_START_S  3
/** RTC_I2C_TX_LSB_FIRST : R/W; bitpos: [4]; default: 0;
 *  transit lsb first
 */
#define RTC_I2C_TX_LSB_FIRST    (BIT(4))
#define RTC_I2C_TX_LSB_FIRST_M  (RTC_I2C_TX_LSB_FIRST_V << RTC_I2C_TX_LSB_FIRST_S)
#define RTC_I2C_TX_LSB_FIRST_V  0x00000001U
#define RTC_I2C_TX_LSB_FIRST_S  4
/** RTC_I2C_RX_LSB_FIRST : R/W; bitpos: [5]; default: 0;
 *  receive lsb first
 */
#define RTC_I2C_RX_LSB_FIRST    (BIT(5))
#define RTC_I2C_RX_LSB_FIRST_M  (RTC_I2C_RX_LSB_FIRST_V << RTC_I2C_RX_LSB_FIRST_S)
#define RTC_I2C_RX_LSB_FIRST_V  0x00000001U
#define RTC_I2C_RX_LSB_FIRST_S  5
/** RTC_I2C_I2C_CTRL_CLK_GATE_EN : R/W; bitpos: [29]; default: 0;
 *  configure i2c ctrl clk enable
 */
#define RTC_I2C_I2C_CTRL_CLK_GATE_EN    (BIT(29))
#define RTC_I2C_I2C_CTRL_CLK_GATE_EN_M  (RTC_I2C_I2C_CTRL_CLK_GATE_EN_V << RTC_I2C_I2C_CTRL_CLK_GATE_EN_S)
#define RTC_I2C_I2C_CTRL_CLK_GATE_EN_V  0x00000001U
#define RTC_I2C_I2C_CTRL_CLK_GATE_EN_S  29
/** RTC_I2C_I2C_RESET : R/W; bitpos: [30]; default: 0;
 *  rtc i2c sw reset
 */
#define RTC_I2C_I2C_RESET    (BIT(30))
#define RTC_I2C_I2C_RESET_M  (RTC_I2C_I2C_RESET_V << RTC_I2C_I2C_RESET_S)
#define RTC_I2C_I2C_RESET_V  0x00000001U
#define RTC_I2C_I2C_RESET_S  30
/** RTC_I2C_I2CCLK_EN : R/W; bitpos: [31]; default: 0;
 *  rtc i2c reg clk gating
 */
#define RTC_I2C_I2CCLK_EN    (BIT(31))
#define RTC_I2C_I2CCLK_EN_M  (RTC_I2C_I2CCLK_EN_V << RTC_I2C_I2CCLK_EN_S)
#define RTC_I2C_I2CCLK_EN_V  0x00000001U
#define RTC_I2C_I2CCLK_EN_S  31

/** RTC_I2C_STATUS_REG register
 *  get i2c status
 */
#define RTC_I2C_STATUS_REG (DR_REG_RTC_BASE + 0x8)
/** RTC_I2C_ACK_REC : RO; bitpos: [0]; default: 0;
 *  ack response
 */
#define RTC_I2C_ACK_REC    (BIT(0))
#define RTC_I2C_ACK_REC_M  (RTC_I2C_ACK_REC_V << RTC_I2C_ACK_REC_S)
#define RTC_I2C_ACK_REC_V  0x00000001U
#define RTC_I2C_ACK_REC_S  0
/** RTC_I2C_SLAVE_RW : RO; bitpos: [1]; default: 0;
 *  slave read or write
 */
#define RTC_I2C_SLAVE_RW    (BIT(1))
#define RTC_I2C_SLAVE_RW_M  (RTC_I2C_SLAVE_RW_V << RTC_I2C_SLAVE_RW_S)
#define RTC_I2C_SLAVE_RW_V  0x00000001U
#define RTC_I2C_SLAVE_RW_S  1
/** RTC_I2C_ARB_LOST : RO; bitpos: [2]; default: 0;
 *  arbitration is lost
 */
#define RTC_I2C_ARB_LOST    (BIT(2))
#define RTC_I2C_ARB_LOST_M  (RTC_I2C_ARB_LOST_V << RTC_I2C_ARB_LOST_S)
#define RTC_I2C_ARB_LOST_V  0x00000001U
#define RTC_I2C_ARB_LOST_S  2
/** RTC_I2C_BUS_BUSY : RO; bitpos: [3]; default: 0;
 *  bus is busy
 */
#define RTC_I2C_BUS_BUSY    (BIT(3))
#define RTC_I2C_BUS_BUSY_M  (RTC_I2C_BUS_BUSY_V << RTC_I2C_BUS_BUSY_S)
#define RTC_I2C_BUS_BUSY_V  0x00000001U
#define RTC_I2C_BUS_BUSY_S  3
/** RTC_I2C_SLAVE_ADDRESSED : RO; bitpos: [4]; default: 0;
 *  slave reg sub address
 */
#define RTC_I2C_SLAVE_ADDRESSED    (BIT(4))
#define RTC_I2C_SLAVE_ADDRESSED_M  (RTC_I2C_SLAVE_ADDRESSED_V << RTC_I2C_SLAVE_ADDRESSED_S)
#define RTC_I2C_SLAVE_ADDRESSED_V  0x00000001U
#define RTC_I2C_SLAVE_ADDRESSED_S  4
/** RTC_I2C_BYTE_TRANS : RO; bitpos: [5]; default: 0;
 *  One byte transit done
 */
#define RTC_I2C_BYTE_TRANS    (BIT(5))
#define RTC_I2C_BYTE_TRANS_M  (RTC_I2C_BYTE_TRANS_V << RTC_I2C_BYTE_TRANS_S)
#define RTC_I2C_BYTE_TRANS_V  0x00000001U
#define RTC_I2C_BYTE_TRANS_S  5
/** RTC_I2C_OP_CNT : RO; bitpos: [7:6]; default: 0;
 *  which operation is working
 */
#define RTC_I2C_OP_CNT    0x00000003U
#define RTC_I2C_OP_CNT_M  (RTC_I2C_OP_CNT_V << RTC_I2C_OP_CNT_S)
#define RTC_I2C_OP_CNT_V  0x00000003U
#define RTC_I2C_OP_CNT_S  6
/** RTC_I2C_SHIFT_REG : RO; bitpos: [23:16]; default: 0;
 *  shifter content
 */
#define RTC_I2C_SHIFT_REG    0x000000FFU
#define RTC_I2C_SHIFT_REG_M  (RTC_I2C_SHIFT_REG_V << RTC_I2C_SHIFT_REG_S)
#define RTC_I2C_SHIFT_REG_V  0x000000FFU
#define RTC_I2C_SHIFT_REG_S  16
/** RTC_I2C_SCL_MAIN_STATE_LAST : RO; bitpos: [26:24]; default: 0;
 *  i2c last main status
 */
#define RTC_I2C_SCL_MAIN_STATE_LAST    0x00000007U
#define RTC_I2C_SCL_MAIN_STATE_LAST_M  (RTC_I2C_SCL_MAIN_STATE_LAST_V << RTC_I2C_SCL_MAIN_STATE_LAST_S)
#define RTC_I2C_SCL_MAIN_STATE_LAST_V  0x00000007U
#define RTC_I2C_SCL_MAIN_STATE_LAST_S  24
/** RTC_I2C_SCL_STATE_LAST : RO; bitpos: [30:28]; default: 0;
 *  scl last status
 */
#define RTC_I2C_SCL_STATE_LAST    0x00000007U
#define RTC_I2C_SCL_STATE_LAST_M  (RTC_I2C_SCL_STATE_LAST_V << RTC_I2C_SCL_STATE_LAST_S)
#define RTC_I2C_SCL_STATE_LAST_V  0x00000007U
#define RTC_I2C_SCL_STATE_LAST_S  28

/** RTC_I2C_TO_REG register
 *  configure time out
 */
#define RTC_I2C_TO_REG (DR_REG_RTC_BASE + 0xc)
/** RTC_I2C_TIME_OUT_REG : R/W; bitpos: [19:0]; default: 65536;
 *  time out threshold
 */
#define RTC_I2C_TIME_OUT_REG    0x000FFFFFU
#define RTC_I2C_TIME_OUT_REG_M  (RTC_I2C_TIME_OUT_REG_V << RTC_I2C_TIME_OUT_REG_S)
#define RTC_I2C_TIME_OUT_REG_V  0x000FFFFFU
#define RTC_I2C_TIME_OUT_REG_S  0

/** RTC_I2C_SLAVE_ADDR_REG register
 *  configure slave id
 */
#define RTC_I2C_SLAVE_ADDR_REG (DR_REG_RTC_BASE + 0x10)
/** RTC_I2C_SLAVE_ADDR : R/W; bitpos: [14:0]; default: 0;
 *  slave address
 */
#define RTC_I2C_SLAVE_ADDR    0x00007FFFU
#define RTC_I2C_SLAVE_ADDR_M  (RTC_I2C_SLAVE_ADDR_V << RTC_I2C_SLAVE_ADDR_S)
#define RTC_I2C_SLAVE_ADDR_V  0x00007FFFU
#define RTC_I2C_SLAVE_ADDR_S  0
/** RTC_I2C_ADDR_10BIT_EN : R/W; bitpos: [31]; default: 0;
 *  i2c 10bit mode enable
 */
#define RTC_I2C_ADDR_10BIT_EN    (BIT(31))
#define RTC_I2C_ADDR_10BIT_EN_M  (RTC_I2C_ADDR_10BIT_EN_V << RTC_I2C_ADDR_10BIT_EN_S)
#define RTC_I2C_ADDR_10BIT_EN_V  0x00000001U
#define RTC_I2C_ADDR_10BIT_EN_S  31

/** RTC_I2C_SCL_HIGH_REG register
 *  configure high scl period
 */
#define RTC_I2C_SCL_HIGH_REG (DR_REG_RTC_BASE + 0x14)
/** RTC_I2C_SCL_HIGH_PERIOD_REG : R/W; bitpos: [19:0]; default: 256;
 *  time period that scl = 1
 */
#define RTC_I2C_SCL_HIGH_PERIOD_REG    0x000FFFFFU
#define RTC_I2C_SCL_HIGH_PERIOD_REG_M  (RTC_I2C_SCL_HIGH_PERIOD_REG_V << RTC_I2C_SCL_HIGH_PERIOD_REG_S)
#define RTC_I2C_SCL_HIGH_PERIOD_REG_V  0x000FFFFFU
#define RTC_I2C_SCL_HIGH_PERIOD_REG_S  0

/** RTC_I2C_SDA_DUTY_REG register
 *  configure sda duty
 */
#define RTC_I2C_SDA_DUTY_REG (DR_REG_RTC_BASE + 0x18)
/** RTC_I2C_SDA_DUTY_NUM : R/W; bitpos: [19:0]; default: 16;
 *  time period for SDA to toggle after SCL goes low
 */
#define RTC_I2C_SDA_DUTY_NUM    0x000FFFFFU
#define RTC_I2C_SDA_DUTY_NUM_M  (RTC_I2C_SDA_DUTY_NUM_V << RTC_I2C_SDA_DUTY_NUM_S)
#define RTC_I2C_SDA_DUTY_NUM_V  0x000FFFFFU
#define RTC_I2C_SDA_DUTY_NUM_S  0

/** RTC_I2C_SCL_START_PERIOD_REG register
 *  configure scl start period
 */
#define RTC_I2C_SCL_START_PERIOD_REG (DR_REG_RTC_BASE + 0x1c)
/** RTC_I2C_SCL_START_PERIOD : R/W; bitpos: [19:0]; default: 8;
 *  time period for SCL to toggle after I2C start is triggered
 */
#define RTC_I2C_SCL_START_PERIOD    0x000FFFFFU
#define RTC_I2C_SCL_START_PERIOD_M  (RTC_I2C_SCL_START_PERIOD_V << RTC_I2C_SCL_START_PERIOD_S)
#define RTC_I2C_SCL_START_PERIOD_V  0x000FFFFFU
#define RTC_I2C_SCL_START_PERIOD_S  0

/** RTC_I2C_SCL_STOP_PERIOD_REG register
 *  configure scl stop period
 */
#define RTC_I2C_SCL_STOP_PERIOD_REG (DR_REG_RTC_BASE + 0x20)
/** RTC_I2C_SCL_STOP_PERIOD : R/W; bitpos: [19:0]; default: 8;
 *  time period for SCL to stop after I2C end is triggered
 */
#define RTC_I2C_SCL_STOP_PERIOD    0x000FFFFFU
#define RTC_I2C_SCL_STOP_PERIOD_M  (RTC_I2C_SCL_STOP_PERIOD_V << RTC_I2C_SCL_STOP_PERIOD_S)
#define RTC_I2C_SCL_STOP_PERIOD_V  0x000FFFFFU
#define RTC_I2C_SCL_STOP_PERIOD_S  0

/** RTC_I2C_INT_CLR_REG register
 *  interrupt clear register
 */
#define RTC_I2C_INT_CLR_REG (DR_REG_RTC_BASE + 0x24)
/** RTC_I2C_SLAVE_TRAN_COMP_INT_CLR : WO; bitpos: [0]; default: 0;
 *  clear slave transit complete interrupt
 */
#define RTC_I2C_SLAVE_TRAN_COMP_INT_CLR    (BIT(0))
#define RTC_I2C_SLAVE_TRAN_COMP_INT_CLR_M  (RTC_I2C_SLAVE_TRAN_COMP_INT_CLR_V << RTC_I2C_SLAVE_TRAN_COMP_INT_CLR_S)
#define RTC_I2C_SLAVE_TRAN_COMP_INT_CLR_V  0x00000001U
#define RTC_I2C_SLAVE_TRAN_COMP_INT_CLR_S  0
/** RTC_I2C_ARBITRATION_LOST_INT_CLR : WO; bitpos: [1]; default: 0;
 *  clear arbitration lost interrupt
 */
#define RTC_I2C_ARBITRATION_LOST_INT_CLR    (BIT(1))
#define RTC_I2C_ARBITRATION_LOST_INT_CLR_M  (RTC_I2C_ARBITRATION_LOST_INT_CLR_V << RTC_I2C_ARBITRATION_LOST_INT_CLR_S)
#define RTC_I2C_ARBITRATION_LOST_INT_CLR_V  0x00000001U
#define RTC_I2C_ARBITRATION_LOST_INT_CLR_S  1
/** RTC_I2C_MASTER_TRAN_COMP_INT_CLR : WO; bitpos: [2]; default: 0;
 *  clear master transit complete interrupt
 */
#define RTC_I2C_MASTER_TRAN_COMP_INT_CLR    (BIT(2))
#define RTC_I2C_MASTER_TRAN_COMP_INT_CLR_M  (RTC_I2C_MASTER_TRAN_COMP_INT_CLR_V << RTC_I2C_MASTER_TRAN_COMP_INT_CLR_S)
#define RTC_I2C_MASTER_TRAN_COMP_INT_CLR_V  0x00000001U
#define RTC_I2C_MASTER_TRAN_COMP_INT_CLR_S  2
/** RTC_I2C_TRANS_COMPLETE_INT_CLR : WO; bitpos: [3]; default: 0;
 *  clear transit complete interrupt
 */
#define RTC_I2C_TRANS_COMPLETE_INT_CLR    (BIT(3))
#define RTC_I2C_TRANS_COMPLETE_INT_CLR_M  (RTC_I2C_TRANS_COMPLETE_INT_CLR_V << RTC_I2C_TRANS_COMPLETE_INT_CLR_S)
#define RTC_I2C_TRANS_COMPLETE_INT_CLR_V  0x00000001U
#define RTC_I2C_TRANS_COMPLETE_INT_CLR_S  3
/** RTC_I2C_TIME_OUT_INT_CLR : WO; bitpos: [4]; default: 0;
 *  clear time out interrupt
 */
#define RTC_I2C_TIME_OUT_INT_CLR    (BIT(4))
#define RTC_I2C_TIME_OUT_INT_CLR_M  (RTC_I2C_TIME_OUT_INT_CLR_V << RTC_I2C_TIME_OUT_INT_CLR_S)
#define RTC_I2C_TIME_OUT_INT_CLR_V  0x00000001U
#define RTC_I2C_TIME_OUT_INT_CLR_S  4
/** RTC_I2C_ACK_ERR_INT_CLR : WO; bitpos: [5]; default: 0;
 *  clear ack error interrupt
 */
#define RTC_I2C_ACK_ERR_INT_CLR    (BIT(5))
#define RTC_I2C_ACK_ERR_INT_CLR_M  (RTC_I2C_ACK_ERR_INT_CLR_V << RTC_I2C_ACK_ERR_INT_CLR_S)
#define RTC_I2C_ACK_ERR_INT_CLR_V  0x00000001U
#define RTC_I2C_ACK_ERR_INT_CLR_S  5
/** RTC_I2C_RX_DATA_INT_CLR : WO; bitpos: [6]; default: 0;
 *  clear receive data interrupt
 */
#define RTC_I2C_RX_DATA_INT_CLR    (BIT(6))
#define RTC_I2C_RX_DATA_INT_CLR_M  (RTC_I2C_RX_DATA_INT_CLR_V << RTC_I2C_RX_DATA_INT_CLR_S)
#define RTC_I2C_RX_DATA_INT_CLR_V  0x00000001U
#define RTC_I2C_RX_DATA_INT_CLR_S  6
/** RTC_I2C_TX_DATA_INT_CLR : WO; bitpos: [7]; default: 0;
 *  clear transit load data complete interrupt
 */
#define RTC_I2C_TX_DATA_INT_CLR    (BIT(7))
#define RTC_I2C_TX_DATA_INT_CLR_M  (RTC_I2C_TX_DATA_INT_CLR_V << RTC_I2C_TX_DATA_INT_CLR_S)
#define RTC_I2C_TX_DATA_INT_CLR_V  0x00000001U
#define RTC_I2C_TX_DATA_INT_CLR_S  7
/** RTC_I2C_DETECT_START_INT_CLR : WO; bitpos: [8]; default: 0;
 *  clear detect start interrupt
 */
#define RTC_I2C_DETECT_START_INT_CLR    (BIT(8))
#define RTC_I2C_DETECT_START_INT_CLR_M  (RTC_I2C_DETECT_START_INT_CLR_V << RTC_I2C_DETECT_START_INT_CLR_S)
#define RTC_I2C_DETECT_START_INT_CLR_V  0x00000001U
#define RTC_I2C_DETECT_START_INT_CLR_S  8

/** RTC_I2C_INT_RAW_REG register
 *  interrupt raw register
 */
#define RTC_I2C_INT_RAW_REG (DR_REG_RTC_BASE + 0x28)
/** RTC_I2C_SLAVE_TRAN_COMP_INT_RAW : RO; bitpos: [0]; default: 0;
 *  slave transit complete interrupt raw
 */
#define RTC_I2C_SLAVE_TRAN_COMP_INT_RAW    (BIT(0))
#define RTC_I2C_SLAVE_TRAN_COMP_INT_RAW_M  (RTC_I2C_SLAVE_TRAN_COMP_INT_RAW_V << RTC_I2C_SLAVE_TRAN_COMP_INT_RAW_S)
#define RTC_I2C_SLAVE_TRAN_COMP_INT_RAW_V  0x00000001U
#define RTC_I2C_SLAVE_TRAN_COMP_INT_RAW_S  0
/** RTC_I2C_ARBITRATION_LOST_INT_RAW : RO; bitpos: [1]; default: 0;
 *  arbitration lost interrupt raw
 */
#define RTC_I2C_ARBITRATION_LOST_INT_RAW    (BIT(1))
#define RTC_I2C_ARBITRATION_LOST_INT_RAW_M  (RTC_I2C_ARBITRATION_LOST_INT_RAW_V << RTC_I2C_ARBITRATION_LOST_INT_RAW_S)
#define RTC_I2C_ARBITRATION_LOST_INT_RAW_V  0x00000001U
#define RTC_I2C_ARBITRATION_LOST_INT_RAW_S  1
/** RTC_I2C_MASTER_TRAN_COMP_INT_RAW : RO; bitpos: [2]; default: 0;
 *  master transit complete interrupt raw
 */
#define RTC_I2C_MASTER_TRAN_COMP_INT_RAW    (BIT(2))
#define RTC_I2C_MASTER_TRAN_COMP_INT_RAW_M  (RTC_I2C_MASTER_TRAN_COMP_INT_RAW_V << RTC_I2C_MASTER_TRAN_COMP_INT_RAW_S)
#define RTC_I2C_MASTER_TRAN_COMP_INT_RAW_V  0x00000001U
#define RTC_I2C_MASTER_TRAN_COMP_INT_RAW_S  2
/** RTC_I2C_TRANS_COMPLETE_INT_RAW : RO; bitpos: [3]; default: 0;
 *  transit complete interrupt raw
 */
#define RTC_I2C_TRANS_COMPLETE_INT_RAW    (BIT(3))
#define RTC_I2C_TRANS_COMPLETE_INT_RAW_M  (RTC_I2C_TRANS_COMPLETE_INT_RAW_V << RTC_I2C_TRANS_COMPLETE_INT_RAW_S)
#define RTC_I2C_TRANS_COMPLETE_INT_RAW_V  0x00000001U
#define RTC_I2C_TRANS_COMPLETE_INT_RAW_S  3
/** RTC_I2C_TIME_OUT_INT_RAW : RO; bitpos: [4]; default: 0;
 *  time out interrupt raw
 */
#define RTC_I2C_TIME_OUT_INT_RAW    (BIT(4))
#define RTC_I2C_TIME_OUT_INT_RAW_M  (RTC_I2C_TIME_OUT_INT_RAW_V << RTC_I2C_TIME_OUT_INT_RAW_S)
#define RTC_I2C_TIME_OUT_INT_RAW_V  0x00000001U
#define RTC_I2C_TIME_OUT_INT_RAW_S  4
/** RTC_I2C_ACK_ERR_INT_RAW : RO; bitpos: [5]; default: 0;
 *  ack error interrupt raw
 */
#define RTC_I2C_ACK_ERR_INT_RAW    (BIT(5))
#define RTC_I2C_ACK_ERR_INT_RAW_M  (RTC_I2C_ACK_ERR_INT_RAW_V << RTC_I2C_ACK_ERR_INT_RAW_S)
#define RTC_I2C_ACK_ERR_INT_RAW_V  0x00000001U
#define RTC_I2C_ACK_ERR_INT_RAW_S  5
/** RTC_I2C_RX_DATA_INT_RAW : RO; bitpos: [6]; default: 0;
 *  receive data interrupt raw
 */
#define RTC_I2C_RX_DATA_INT_RAW    (BIT(6))
#define RTC_I2C_RX_DATA_INT_RAW_M  (RTC_I2C_RX_DATA_INT_RAW_V << RTC_I2C_RX_DATA_INT_RAW_S)
#define RTC_I2C_RX_DATA_INT_RAW_V  0x00000001U
#define RTC_I2C_RX_DATA_INT_RAW_S  6
/** RTC_I2C_TX_DATA_INT_RAW : RO; bitpos: [7]; default: 0;
 *  transit data interrupt raw
 */
#define RTC_I2C_TX_DATA_INT_RAW    (BIT(7))
#define RTC_I2C_TX_DATA_INT_RAW_M  (RTC_I2C_TX_DATA_INT_RAW_V << RTC_I2C_TX_DATA_INT_RAW_S)
#define RTC_I2C_TX_DATA_INT_RAW_V  0x00000001U
#define RTC_I2C_TX_DATA_INT_RAW_S  7
/** RTC_I2C_DETECT_START_INT_RAW : RO; bitpos: [8]; default: 0;
 *  detect start interrupt raw
 */
#define RTC_I2C_DETECT_START_INT_RAW    (BIT(8))
#define RTC_I2C_DETECT_START_INT_RAW_M  (RTC_I2C_DETECT_START_INT_RAW_V << RTC_I2C_DETECT_START_INT_RAW_S)
#define RTC_I2C_DETECT_START_INT_RAW_V  0x00000001U
#define RTC_I2C_DETECT_START_INT_RAW_S  8

/** RTC_I2C_INT_ST_REG register
 *  interrupt state register
 */
#define RTC_I2C_INT_ST_REG (DR_REG_RTC_BASE + 0x2c)
/** RTC_I2C_SLAVE_TRAN_COMP_INT_ST : RO; bitpos: [0]; default: 0;
 *  slave transit complete interrupt state
 */
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ST    (BIT(0))
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ST_M  (RTC_I2C_SLAVE_TRAN_COMP_INT_ST_V << RTC_I2C_SLAVE_TRAN_COMP_INT_ST_S)
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ST_V  0x00000001U
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ST_S  0
/** RTC_I2C_ARBITRATION_LOST_INT_ST : RO; bitpos: [1]; default: 0;
 *  arbitration lost interrupt state
 */
#define RTC_I2C_ARBITRATION_LOST_INT_ST    (BIT(1))
#define RTC_I2C_ARBITRATION_LOST_INT_ST_M  (RTC_I2C_ARBITRATION_LOST_INT_ST_V << RTC_I2C_ARBITRATION_LOST_INT_ST_S)
#define RTC_I2C_ARBITRATION_LOST_INT_ST_V  0x00000001U
#define RTC_I2C_ARBITRATION_LOST_INT_ST_S  1
/** RTC_I2C_MASTER_TRAN_COMP_INT_ST : RO; bitpos: [2]; default: 0;
 *  master transit complete interrupt state
 */
#define RTC_I2C_MASTER_TRAN_COMP_INT_ST    (BIT(2))
#define RTC_I2C_MASTER_TRAN_COMP_INT_ST_M  (RTC_I2C_MASTER_TRAN_COMP_INT_ST_V << RTC_I2C_MASTER_TRAN_COMP_INT_ST_S)
#define RTC_I2C_MASTER_TRAN_COMP_INT_ST_V  0x00000001U
#define RTC_I2C_MASTER_TRAN_COMP_INT_ST_S  2
/** RTC_I2C_TRANS_COMPLETE_INT_ST : RO; bitpos: [3]; default: 0;
 *  transit complete interrupt state
 */
#define RTC_I2C_TRANS_COMPLETE_INT_ST    (BIT(3))
#define RTC_I2C_TRANS_COMPLETE_INT_ST_M  (RTC_I2C_TRANS_COMPLETE_INT_ST_V << RTC_I2C_TRANS_COMPLETE_INT_ST_S)
#define RTC_I2C_TRANS_COMPLETE_INT_ST_V  0x00000001U
#define RTC_I2C_TRANS_COMPLETE_INT_ST_S  3
/** RTC_I2C_TIME_OUT_INT_ST : RO; bitpos: [4]; default: 0;
 *  time out interrupt state
 */
#define RTC_I2C_TIME_OUT_INT_ST    (BIT(4))
#define RTC_I2C_TIME_OUT_INT_ST_M  (RTC_I2C_TIME_OUT_INT_ST_V << RTC_I2C_TIME_OUT_INT_ST_S)
#define RTC_I2C_TIME_OUT_INT_ST_V  0x00000001U
#define RTC_I2C_TIME_OUT_INT_ST_S  4
/** RTC_I2C_ACK_ERR_INT_ST : RO; bitpos: [5]; default: 0;
 *  ack error interrupt state
 */
#define RTC_I2C_ACK_ERR_INT_ST    (BIT(5))
#define RTC_I2C_ACK_ERR_INT_ST_M  (RTC_I2C_ACK_ERR_INT_ST_V << RTC_I2C_ACK_ERR_INT_ST_S)
#define RTC_I2C_ACK_ERR_INT_ST_V  0x00000001U
#define RTC_I2C_ACK_ERR_INT_ST_S  5
/** RTC_I2C_RX_DATA_INT_ST : RO; bitpos: [6]; default: 0;
 *  receive data interrupt state
 */
#define RTC_I2C_RX_DATA_INT_ST    (BIT(6))
#define RTC_I2C_RX_DATA_INT_ST_M  (RTC_I2C_RX_DATA_INT_ST_V << RTC_I2C_RX_DATA_INT_ST_S)
#define RTC_I2C_RX_DATA_INT_ST_V  0x00000001U
#define RTC_I2C_RX_DATA_INT_ST_S  6
/** RTC_I2C_TX_DATA_INT_ST : RO; bitpos: [7]; default: 0;
 *  transit data interrupt state
 */
#define RTC_I2C_TX_DATA_INT_ST    (BIT(7))
#define RTC_I2C_TX_DATA_INT_ST_M  (RTC_I2C_TX_DATA_INT_ST_V << RTC_I2C_TX_DATA_INT_ST_S)
#define RTC_I2C_TX_DATA_INT_ST_V  0x00000001U
#define RTC_I2C_TX_DATA_INT_ST_S  7
/** RTC_I2C_DETECT_START_INT_ST : RO; bitpos: [8]; default: 0;
 *  detect start interrupt state
 */
#define RTC_I2C_DETECT_START_INT_ST    (BIT(8))
#define RTC_I2C_DETECT_START_INT_ST_M  (RTC_I2C_DETECT_START_INT_ST_V << RTC_I2C_DETECT_START_INT_ST_S)
#define RTC_I2C_DETECT_START_INT_ST_V  0x00000001U
#define RTC_I2C_DETECT_START_INT_ST_S  8

/** RTC_I2C_INT_ENA_REG register
 *  interrupt enable register
 */
#define RTC_I2C_INT_ENA_REG (DR_REG_RTC_BASE + 0x30)
/** RTC_I2C_SLAVE_TRAN_COMP_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  enable slave transit complete interrupt
 */
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ENA    (BIT(0))
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ENA_M  (RTC_I2C_SLAVE_TRAN_COMP_INT_ENA_V << RTC_I2C_SLAVE_TRAN_COMP_INT_ENA_S)
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ENA_V  0x00000001U
#define RTC_I2C_SLAVE_TRAN_COMP_INT_ENA_S  0
/** RTC_I2C_ARBITRATION_LOST_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  enable arbitration lost interrupt
 */
#define RTC_I2C_ARBITRATION_LOST_INT_ENA    (BIT(1))
#define RTC_I2C_ARBITRATION_LOST_INT_ENA_M  (RTC_I2C_ARBITRATION_LOST_INT_ENA_V << RTC_I2C_ARBITRATION_LOST_INT_ENA_S)
#define RTC_I2C_ARBITRATION_LOST_INT_ENA_V  0x00000001U
#define RTC_I2C_ARBITRATION_LOST_INT_ENA_S  1
/** RTC_I2C_MASTER_TRAN_COMP_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  enable master transit complete interrupt
 */
#define RTC_I2C_MASTER_TRAN_COMP_INT_ENA    (BIT(2))
#define RTC_I2C_MASTER_TRAN_COMP_INT_ENA_M  (RTC_I2C_MASTER_TRAN_COMP_INT_ENA_V << RTC_I2C_MASTER_TRAN_COMP_INT_ENA_S)
#define RTC_I2C_MASTER_TRAN_COMP_INT_ENA_V  0x00000001U
#define RTC_I2C_MASTER_TRAN_COMP_INT_ENA_S  2
/** RTC_I2C_TRANS_COMPLETE_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  enable transit complete interrupt
 */
#define RTC_I2C_TRANS_COMPLETE_INT_ENA    (BIT(3))
#define RTC_I2C_TRANS_COMPLETE_INT_ENA_M  (RTC_I2C_TRANS_COMPLETE_INT_ENA_V << RTC_I2C_TRANS_COMPLETE_INT_ENA_S)
#define RTC_I2C_TRANS_COMPLETE_INT_ENA_V  0x00000001U
#define RTC_I2C_TRANS_COMPLETE_INT_ENA_S  3
/** RTC_I2C_TIME_OUT_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  enable time out interrupt
 */
#define RTC_I2C_TIME_OUT_INT_ENA    (BIT(4))
#define RTC_I2C_TIME_OUT_INT_ENA_M  (RTC_I2C_TIME_OUT_INT_ENA_V << RTC_I2C_TIME_OUT_INT_ENA_S)
#define RTC_I2C_TIME_OUT_INT_ENA_V  0x00000001U
#define RTC_I2C_TIME_OUT_INT_ENA_S  4
/** RTC_I2C_ACK_ERR_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  enable eack error interrupt
 */
#define RTC_I2C_ACK_ERR_INT_ENA    (BIT(5))
#define RTC_I2C_ACK_ERR_INT_ENA_M  (RTC_I2C_ACK_ERR_INT_ENA_V << RTC_I2C_ACK_ERR_INT_ENA_S)
#define RTC_I2C_ACK_ERR_INT_ENA_V  0x00000001U
#define RTC_I2C_ACK_ERR_INT_ENA_S  5
/** RTC_I2C_RX_DATA_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  enable receive data interrupt
 */
#define RTC_I2C_RX_DATA_INT_ENA    (BIT(6))
#define RTC_I2C_RX_DATA_INT_ENA_M  (RTC_I2C_RX_DATA_INT_ENA_V << RTC_I2C_RX_DATA_INT_ENA_S)
#define RTC_I2C_RX_DATA_INT_ENA_V  0x00000001U
#define RTC_I2C_RX_DATA_INT_ENA_S  6
/** RTC_I2C_TX_DATA_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  enable transit data interrupt
 */
#define RTC_I2C_TX_DATA_INT_ENA    (BIT(7))
#define RTC_I2C_TX_DATA_INT_ENA_M  (RTC_I2C_TX_DATA_INT_ENA_V << RTC_I2C_TX_DATA_INT_ENA_S)
#define RTC_I2C_TX_DATA_INT_ENA_V  0x00000001U
#define RTC_I2C_TX_DATA_INT_ENA_S  7
/** RTC_I2C_DETECT_START_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  enable detect start interrupt
 */
#define RTC_I2C_DETECT_START_INT_ENA    (BIT(8))
#define RTC_I2C_DETECT_START_INT_ENA_M  (RTC_I2C_DETECT_START_INT_ENA_V << RTC_I2C_DETECT_START_INT_ENA_S)
#define RTC_I2C_DETECT_START_INT_ENA_V  0x00000001U
#define RTC_I2C_DETECT_START_INT_ENA_S  8

/** RTC_I2C_DATA_REG register
 *  get i2c data status
 */
#define RTC_I2C_DATA_REG (DR_REG_RTC_BASE + 0x34)
/** RTC_I2C_I2C_RDATA : RO; bitpos: [7:0]; default: 0;
 *  data received
 */
#define RTC_I2C_I2C_RDATA    0x000000FFU
#define RTC_I2C_I2C_RDATA_M  (RTC_I2C_I2C_RDATA_V << RTC_I2C_I2C_RDATA_S)
#define RTC_I2C_I2C_RDATA_V  0x000000FFU
#define RTC_I2C_I2C_RDATA_S  0
/** RTC_I2C_SLAVE_TX_DATA : R/W; bitpos: [15:8]; default: 0;
 *  data sent by slave
 */
#define RTC_I2C_SLAVE_TX_DATA    0x000000FFU
#define RTC_I2C_SLAVE_TX_DATA_M  (RTC_I2C_SLAVE_TX_DATA_V << RTC_I2C_SLAVE_TX_DATA_S)
#define RTC_I2C_SLAVE_TX_DATA_V  0x000000FFU
#define RTC_I2C_SLAVE_TX_DATA_S  8
/** RTC_I2C_I2C_DONE : RO; bitpos: [31]; default: 0;
 *  i2c done
 */
#define RTC_I2C_I2C_DONE    (BIT(31))
#define RTC_I2C_I2C_DONE_M  (RTC_I2C_I2C_DONE_V << RTC_I2C_I2C_DONE_S)
#define RTC_I2C_I2C_DONE_V  0x00000001U
#define RTC_I2C_I2C_DONE_S  31

/** RTC_I2C_CMD0_REG register
 *  i2c commond0 register
 */
#define RTC_I2C_CMD0_REG (DR_REG_RTC_BASE + 0x38)
/** RTC_I2C_COMMAND0 : R/W; bitpos: [13:0]; default: 2307;
 *  command0
 */
#define RTC_I2C_COMMAND0    0x00003FFFU
#define RTC_I2C_COMMAND0_M  (RTC_I2C_COMMAND0_V << RTC_I2C_COMMAND0_S)
#define RTC_I2C_COMMAND0_V  0x00003FFFU
#define RTC_I2C_COMMAND0_S  0
/** RTC_I2C_COMMAND0_DONE : RO; bitpos: [31]; default: 0;
 *  command0_done
 */
#define RTC_I2C_COMMAND0_DONE    (BIT(31))
#define RTC_I2C_COMMAND0_DONE_M  (RTC_I2C_COMMAND0_DONE_V << RTC_I2C_COMMAND0_DONE_S)
#define RTC_I2C_COMMAND0_DONE_V  0x00000001U
#define RTC_I2C_COMMAND0_DONE_S  31

/** RTC_I2C_CMD1_REG register
 *  i2c commond1 register
 */
#define RTC_I2C_CMD1_REG (DR_REG_RTC_BASE + 0x3c)
/** RTC_I2C_COMMAND1 : R/W; bitpos: [13:0]; default: 6401;
 *  command1
 */
#define RTC_I2C_COMMAND1    0x00003FFFU
#define RTC_I2C_COMMAND1_M  (RTC_I2C_COMMAND1_V << RTC_I2C_COMMAND1_S)
#define RTC_I2C_COMMAND1_V  0x00003FFFU
#define RTC_I2C_COMMAND1_S  0
/** RTC_I2C_COMMAND1_DONE : RO; bitpos: [31]; default: 0;
 *  command1_done
 */
#define RTC_I2C_COMMAND1_DONE    (BIT(31))
#define RTC_I2C_COMMAND1_DONE_M  (RTC_I2C_COMMAND1_DONE_V << RTC_I2C_COMMAND1_DONE_S)
#define RTC_I2C_COMMAND1_DONE_V  0x00000001U
#define RTC_I2C_COMMAND1_DONE_S  31

/** RTC_I2C_CMD2_REG register
 *  i2c commond2 register
 */
#define RTC_I2C_CMD2_REG (DR_REG_RTC_BASE + 0x40)
/** RTC_I2C_COMMAND2 : R/W; bitpos: [13:0]; default: 2306;
 *  command2
 */
#define RTC_I2C_COMMAND2    0x00003FFFU
#define RTC_I2C_COMMAND2_M  (RTC_I2C_COMMAND2_V << RTC_I2C_COMMAND2_S)
#define RTC_I2C_COMMAND2_V  0x00003FFFU
#define RTC_I2C_COMMAND2_S  0
/** RTC_I2C_COMMAND2_DONE : RO; bitpos: [31]; default: 0;
 *  command2_done
 */
#define RTC_I2C_COMMAND2_DONE    (BIT(31))
#define RTC_I2C_COMMAND2_DONE_M  (RTC_I2C_COMMAND2_DONE_V << RTC_I2C_COMMAND2_DONE_S)
#define RTC_I2C_COMMAND2_DONE_V  0x00000001U
#define RTC_I2C_COMMAND2_DONE_S  31

/** RTC_I2C_CMD3_REG register
 *  i2c commond3 register
 */
#define RTC_I2C_CMD3_REG (DR_REG_RTC_BASE + 0x44)
/** RTC_I2C_COMMAND3 : R/W; bitpos: [13:0]; default: 257;
 *  command3
 */
#define RTC_I2C_COMMAND3    0x00003FFFU
#define RTC_I2C_COMMAND3_M  (RTC_I2C_COMMAND3_V << RTC_I2C_COMMAND3_S)
#define RTC_I2C_COMMAND3_V  0x00003FFFU
#define RTC_I2C_COMMAND3_S  0
/** RTC_I2C_COMMAND3_DONE : RO; bitpos: [31]; default: 0;
 *  command3_done
 */
#define RTC_I2C_COMMAND3_DONE    (BIT(31))
#define RTC_I2C_COMMAND3_DONE_M  (RTC_I2C_COMMAND3_DONE_V << RTC_I2C_COMMAND3_DONE_S)
#define RTC_I2C_COMMAND3_DONE_V  0x00000001U
#define RTC_I2C_COMMAND3_DONE_S  31

/** RTC_I2C_CMD4_REG register
 *  i2c commond4 register
 */
#define RTC_I2C_CMD4_REG (DR_REG_RTC_BASE + 0x48)
/** RTC_I2C_COMMAND4 : R/W; bitpos: [13:0]; default: 2305;
 *  command4
 */
#define RTC_I2C_COMMAND4    0x00003FFFU
#define RTC_I2C_COMMAND4_M  (RTC_I2C_COMMAND4_V << RTC_I2C_COMMAND4_S)
#define RTC_I2C_COMMAND4_V  0x00003FFFU
#define RTC_I2C_COMMAND4_S  0
/** RTC_I2C_COMMAND4_DONE : RO; bitpos: [31]; default: 0;
 *  command4_done
 */
#define RTC_I2C_COMMAND4_DONE    (BIT(31))
#define RTC_I2C_COMMAND4_DONE_M  (RTC_I2C_COMMAND4_DONE_V << RTC_I2C_COMMAND4_DONE_S)
#define RTC_I2C_COMMAND4_DONE_V  0x00000001U
#define RTC_I2C_COMMAND4_DONE_S  31

/** RTC_I2C_CMD5_REG register
 *  i2c commond5_register
 */
#define RTC_I2C_CMD5_REG (DR_REG_RTC_BASE + 0x4c)
/** RTC_I2C_COMMAND5 : R/W; bitpos: [13:0]; default: 5889;
 *  command5
 */
#define RTC_I2C_COMMAND5    0x00003FFFU
#define RTC_I2C_COMMAND5_M  (RTC_I2C_COMMAND5_V << RTC_I2C_COMMAND5_S)
#define RTC_I2C_COMMAND5_V  0x00003FFFU
#define RTC_I2C_COMMAND5_S  0
/** RTC_I2C_COMMAND5_DONE : RO; bitpos: [31]; default: 0;
 *  command5_done
 */
#define RTC_I2C_COMMAND5_DONE    (BIT(31))
#define RTC_I2C_COMMAND5_DONE_M  (RTC_I2C_COMMAND5_DONE_V << RTC_I2C_COMMAND5_DONE_S)
#define RTC_I2C_COMMAND5_DONE_V  0x00000001U
#define RTC_I2C_COMMAND5_DONE_S  31

/** RTC_I2C_CMD6_REG register
 *  i2c commond6 register
 */
#define RTC_I2C_CMD6_REG (DR_REG_RTC_BASE + 0x50)
/** RTC_I2C_COMMAND6 : R/W; bitpos: [13:0]; default: 6401;
 *  command6
 */
#define RTC_I2C_COMMAND6    0x00003FFFU
#define RTC_I2C_COMMAND6_M  (RTC_I2C_COMMAND6_V << RTC_I2C_COMMAND6_S)
#define RTC_I2C_COMMAND6_V  0x00003FFFU
#define RTC_I2C_COMMAND6_S  0
/** RTC_I2C_COMMAND6_DONE : RO; bitpos: [31]; default: 0;
 *  command6_done
 */
#define RTC_I2C_COMMAND6_DONE    (BIT(31))
#define RTC_I2C_COMMAND6_DONE_M  (RTC_I2C_COMMAND6_DONE_V << RTC_I2C_COMMAND6_DONE_S)
#define RTC_I2C_COMMAND6_DONE_V  0x00000001U
#define RTC_I2C_COMMAND6_DONE_S  31

/** RTC_I2C_CMD7_REG register
 *  i2c commond7 register
 */
#define RTC_I2C_CMD7_REG (DR_REG_RTC_BASE + 0x54)
/** RTC_I2C_COMMAND7 : R/W; bitpos: [13:0]; default: 2308;
 *  command7
 */
#define RTC_I2C_COMMAND7    0x00003FFFU
#define RTC_I2C_COMMAND7_M  (RTC_I2C_COMMAND7_V << RTC_I2C_COMMAND7_S)
#define RTC_I2C_COMMAND7_V  0x00003FFFU
#define RTC_I2C_COMMAND7_S  0
/** RTC_I2C_COMMAND7_DONE : RO; bitpos: [31]; default: 0;
 *  command7_done
 */
#define RTC_I2C_COMMAND7_DONE    (BIT(31))
#define RTC_I2C_COMMAND7_DONE_M  (RTC_I2C_COMMAND7_DONE_V << RTC_I2C_COMMAND7_DONE_S)
#define RTC_I2C_COMMAND7_DONE_V  0x00000001U
#define RTC_I2C_COMMAND7_DONE_S  31

/** RTC_I2C_CMD8_REG register
 *  i2c commond8 register
 */
#define RTC_I2C_CMD8_REG (DR_REG_RTC_BASE + 0x58)
/** RTC_I2C_COMMAND8 : R/W; bitpos: [13:0]; default: 6401;
 *  command8
 */
#define RTC_I2C_COMMAND8    0x00003FFFU
#define RTC_I2C_COMMAND8_M  (RTC_I2C_COMMAND8_V << RTC_I2C_COMMAND8_S)
#define RTC_I2C_COMMAND8_V  0x00003FFFU
#define RTC_I2C_COMMAND8_S  0
/** RTC_I2C_COMMAND8_DONE : RO; bitpos: [31]; default: 0;
 *  command8_done
 */
#define RTC_I2C_COMMAND8_DONE    (BIT(31))
#define RTC_I2C_COMMAND8_DONE_M  (RTC_I2C_COMMAND8_DONE_V << RTC_I2C_COMMAND8_DONE_S)
#define RTC_I2C_COMMAND8_DONE_V  0x00000001U
#define RTC_I2C_COMMAND8_DONE_S  31

/** RTC_I2C_CMD9_REG register
 *  i2c commond9 register
 */
#define RTC_I2C_CMD9_REG (DR_REG_RTC_BASE + 0x5c)
/** RTC_I2C_COMMAND9 : R/W; bitpos: [13:0]; default: 2307;
 *  command9
 */
#define RTC_I2C_COMMAND9    0x00003FFFU
#define RTC_I2C_COMMAND9_M  (RTC_I2C_COMMAND9_V << RTC_I2C_COMMAND9_S)
#define RTC_I2C_COMMAND9_V  0x00003FFFU
#define RTC_I2C_COMMAND9_S  0
/** RTC_I2C_COMMAND9_DONE : RO; bitpos: [31]; default: 0;
 *  command9_done
 */
#define RTC_I2C_COMMAND9_DONE    (BIT(31))
#define RTC_I2C_COMMAND9_DONE_M  (RTC_I2C_COMMAND9_DONE_V << RTC_I2C_COMMAND9_DONE_S)
#define RTC_I2C_COMMAND9_DONE_V  0x00000001U
#define RTC_I2C_COMMAND9_DONE_S  31

/** RTC_I2C_CMD10_REG register
 *  i2c commond10 register
 */
#define RTC_I2C_CMD10_REG (DR_REG_RTC_BASE + 0x60)
/** RTC_I2C_COMMAND10 : R/W; bitpos: [13:0]; default: 257;
 *  command10
 */
#define RTC_I2C_COMMAND10    0x00003FFFU
#define RTC_I2C_COMMAND10_M  (RTC_I2C_COMMAND10_V << RTC_I2C_COMMAND10_S)
#define RTC_I2C_COMMAND10_V  0x00003FFFU
#define RTC_I2C_COMMAND10_S  0
/** RTC_I2C_COMMAND10_DONE : RO; bitpos: [31]; default: 0;
 *  command10_done
 */
#define RTC_I2C_COMMAND10_DONE    (BIT(31))
#define RTC_I2C_COMMAND10_DONE_M  (RTC_I2C_COMMAND10_DONE_V << RTC_I2C_COMMAND10_DONE_S)
#define RTC_I2C_COMMAND10_DONE_V  0x00000001U
#define RTC_I2C_COMMAND10_DONE_S  31

/** RTC_I2C_CMD11_REG register
 *  i2c commond11 register
 */
#define RTC_I2C_CMD11_REG (DR_REG_RTC_BASE + 0x64)
/** RTC_I2C_COMMAND11 : R/W; bitpos: [13:0]; default: 2305;
 *  command11
 */
#define RTC_I2C_COMMAND11    0x00003FFFU
#define RTC_I2C_COMMAND11_M  (RTC_I2C_COMMAND11_V << RTC_I2C_COMMAND11_S)
#define RTC_I2C_COMMAND11_V  0x00003FFFU
#define RTC_I2C_COMMAND11_S  0
/** RTC_I2C_COMMAND11_DONE : RO; bitpos: [31]; default: 0;
 *  command11_done
 */
#define RTC_I2C_COMMAND11_DONE    (BIT(31))
#define RTC_I2C_COMMAND11_DONE_M  (RTC_I2C_COMMAND11_DONE_V << RTC_I2C_COMMAND11_DONE_S)
#define RTC_I2C_COMMAND11_DONE_V  0x00000001U
#define RTC_I2C_COMMAND11_DONE_S  31

/** RTC_I2C_CMD12_REG register
 *  i2c commond12 register
 */
#define RTC_I2C_CMD12_REG (DR_REG_RTC_BASE + 0x68)
/** RTC_I2C_COMMAND12 : R/W; bitpos: [13:0]; default: 5889;
 *  command12
 */
#define RTC_I2C_COMMAND12    0x00003FFFU
#define RTC_I2C_COMMAND12_M  (RTC_I2C_COMMAND12_V << RTC_I2C_COMMAND12_S)
#define RTC_I2C_COMMAND12_V  0x00003FFFU
#define RTC_I2C_COMMAND12_S  0
/** RTC_I2C_COMMAND12_DONE : RO; bitpos: [31]; default: 0;
 *  command12_done
 */
#define RTC_I2C_COMMAND12_DONE    (BIT(31))
#define RTC_I2C_COMMAND12_DONE_M  (RTC_I2C_COMMAND12_DONE_V << RTC_I2C_COMMAND12_DONE_S)
#define RTC_I2C_COMMAND12_DONE_V  0x00000001U
#define RTC_I2C_COMMAND12_DONE_S  31

/** RTC_I2C_CMD13_REG register
 *  i2c commond13 register
 */
#define RTC_I2C_CMD13_REG (DR_REG_RTC_BASE + 0x6c)
/** RTC_I2C_COMMAND13 : R/W; bitpos: [13:0]; default: 6401;
 *  command13
 */
#define RTC_I2C_COMMAND13    0x00003FFFU
#define RTC_I2C_COMMAND13_M  (RTC_I2C_COMMAND13_V << RTC_I2C_COMMAND13_S)
#define RTC_I2C_COMMAND13_V  0x00003FFFU
#define RTC_I2C_COMMAND13_S  0
/** RTC_I2C_COMMAND13_DONE : RO; bitpos: [31]; default: 0;
 *  command13_done
 */
#define RTC_I2C_COMMAND13_DONE    (BIT(31))
#define RTC_I2C_COMMAND13_DONE_M  (RTC_I2C_COMMAND13_DONE_V << RTC_I2C_COMMAND13_DONE_S)
#define RTC_I2C_COMMAND13_DONE_V  0x00000001U
#define RTC_I2C_COMMAND13_DONE_S  31

/** RTC_I2C_CMD14_REG register
 *  i2c commond14 register
 */
#define RTC_I2C_CMD14_REG (DR_REG_RTC_BASE + 0x70)
/** RTC_I2C_COMMAND14 : R/W; bitpos: [13:0]; default: 0;
 *  command14
 */
#define RTC_I2C_COMMAND14    0x00003FFFU
#define RTC_I2C_COMMAND14_M  (RTC_I2C_COMMAND14_V << RTC_I2C_COMMAND14_S)
#define RTC_I2C_COMMAND14_V  0x00003FFFU
#define RTC_I2C_COMMAND14_S  0
/** RTC_I2C_COMMAND14_DONE : RO; bitpos: [31]; default: 0;
 *  command14_done
 */
#define RTC_I2C_COMMAND14_DONE    (BIT(31))
#define RTC_I2C_COMMAND14_DONE_M  (RTC_I2C_COMMAND14_DONE_V << RTC_I2C_COMMAND14_DONE_S)
#define RTC_I2C_COMMAND14_DONE_V  0x00000001U
#define RTC_I2C_COMMAND14_DONE_S  31

/** RTC_I2C_CMD15_REG register
 *  i2c commond15 register
 */
#define RTC_I2C_CMD15_REG (DR_REG_RTC_BASE + 0x74)
/** RTC_I2C_COMMAND15 : R/W; bitpos: [13:0]; default: 0;
 *  command15
 */
#define RTC_I2C_COMMAND15    0x00003FFFU
#define RTC_I2C_COMMAND15_M  (RTC_I2C_COMMAND15_V << RTC_I2C_COMMAND15_S)
#define RTC_I2C_COMMAND15_V  0x00003FFFU
#define RTC_I2C_COMMAND15_S  0
/** RTC_I2C_COMMAND15_DONE : RO; bitpos: [31]; default: 0;
 *  command15_done
 */
#define RTC_I2C_COMMAND15_DONE    (BIT(31))
#define RTC_I2C_COMMAND15_DONE_M  (RTC_I2C_COMMAND15_DONE_V << RTC_I2C_COMMAND15_DONE_S)
#define RTC_I2C_COMMAND15_DONE_V  0x00000001U
#define RTC_I2C_COMMAND15_DONE_S  31

/** RTC_I2C_DATE_REG register
 *  version register
 */
#define RTC_I2C_DATE_REG (DR_REG_RTC_BASE + 0xfc)
/** RTC_I2C_I2C_DATE : R/W; bitpos: [27:0]; default: 26235664;
 *  version
 */
#define RTC_I2C_I2C_DATE    0x0FFFFFFFU
#define RTC_I2C_I2C_DATE_M  (RTC_I2C_I2C_DATE_V << RTC_I2C_I2C_DATE_S)
#define RTC_I2C_I2C_DATE_V  0x0FFFFFFFU
#define RTC_I2C_I2C_DATE_S  0

#ifdef __cplusplus
}
#endif
