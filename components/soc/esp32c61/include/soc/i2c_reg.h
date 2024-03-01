/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** I2C_SCL_LOW_PERIOD_REG register
 *  Configures the low level width of the SCL
 *  Clock
 */
#define I2C_SCL_LOW_PERIOD_REG (DR_REG_I2C_BASE + 0x0)
/** I2C_SCL_LOW_PERIOD : R/W; bitpos: [8:0]; default: 0;
 *  Configures the low level width of the SCL Clock in master mode. \\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_LOW_PERIOD    0x000001FFU
#define I2C_SCL_LOW_PERIOD_M  (I2C_SCL_LOW_PERIOD_V << I2C_SCL_LOW_PERIOD_S)
#define I2C_SCL_LOW_PERIOD_V  0x000001FFU
#define I2C_SCL_LOW_PERIOD_S  0

/** I2C_CTR_REG register
 *  Transmission setting
 */
#define I2C_CTR_REG (DR_REG_I2C_BASE + 0x4)
/** I2C_SDA_FORCE_OUT : R/W; bitpos: [0]; default: 0;
 *  Configures the SDA output mode.\\
 *  0: Open drain output \\
 *  1: Direct output \\
 */
#define I2C_SDA_FORCE_OUT    (BIT(0))
#define I2C_SDA_FORCE_OUT_M  (I2C_SDA_FORCE_OUT_V << I2C_SDA_FORCE_OUT_S)
#define I2C_SDA_FORCE_OUT_V  0x00000001U
#define I2C_SDA_FORCE_OUT_S  0
/** I2C_SCL_FORCE_OUT : R/W; bitpos: [1]; default: 0;
 *  Configures the SCL output mode.\\
 *  0: Open drain output \\
 *  1: Direct output \\
 */
#define I2C_SCL_FORCE_OUT    (BIT(1))
#define I2C_SCL_FORCE_OUT_M  (I2C_SCL_FORCE_OUT_V << I2C_SCL_FORCE_OUT_S)
#define I2C_SCL_FORCE_OUT_V  0x00000001U
#define I2C_SCL_FORCE_OUT_S  1
/** I2C_SAMPLE_SCL_LEVEL : R/W; bitpos: [2]; default: 0;
 *  Configures the sample mode for SDA.\\
 *  0: Sample SDA data on the SCL high level \\
 *  1: Sample SDA data on the SCL low level \\
 */
#define I2C_SAMPLE_SCL_LEVEL    (BIT(2))
#define I2C_SAMPLE_SCL_LEVEL_M  (I2C_SAMPLE_SCL_LEVEL_V << I2C_SAMPLE_SCL_LEVEL_S)
#define I2C_SAMPLE_SCL_LEVEL_V  0x00000001U
#define I2C_SAMPLE_SCL_LEVEL_S  2
/** I2C_RX_FULL_ACK_LEVEL : R/W; bitpos: [3]; default: 1;
 *  Configures the ACK value that needs to be sent by master when the rx_fifo_cnt has
 *  reached the threshold.\\
 */
#define I2C_RX_FULL_ACK_LEVEL    (BIT(3))
#define I2C_RX_FULL_ACK_LEVEL_M  (I2C_RX_FULL_ACK_LEVEL_V << I2C_RX_FULL_ACK_LEVEL_S)
#define I2C_RX_FULL_ACK_LEVEL_V  0x00000001U
#define I2C_RX_FULL_ACK_LEVEL_S  3
/** I2C_MS_MODE : R/W; bitpos: [4]; default: 0;
 *  Configures the module as an I2C Master or Slave.\\
 *  0: Slave \\
 *  1: Master \\
 */
#define I2C_MS_MODE    (BIT(4))
#define I2C_MS_MODE_M  (I2C_MS_MODE_V << I2C_MS_MODE_S)
#define I2C_MS_MODE_V  0x00000001U
#define I2C_MS_MODE_S  4
/** I2C_TRANS_START : WT; bitpos: [5]; default: 0;
 *  Configures whether the slave starts sending the data in txfifo.\\
 *  0: No effect \\
 *  1: Start
 */
#define I2C_TRANS_START    (BIT(5))
#define I2C_TRANS_START_M  (I2C_TRANS_START_V << I2C_TRANS_START_S)
#define I2C_TRANS_START_V  0x00000001U
#define I2C_TRANS_START_S  5
/** I2C_TX_LSB_FIRST : R/W; bitpos: [6]; default: 0;
 *  Configures to control the sending order for data needing to be sent.\\
 *  0: send data from the most significant bit \\
 *  1: send data from the least significant bit \\
 */
#define I2C_TX_LSB_FIRST    (BIT(6))
#define I2C_TX_LSB_FIRST_M  (I2C_TX_LSB_FIRST_V << I2C_TX_LSB_FIRST_S)
#define I2C_TX_LSB_FIRST_V  0x00000001U
#define I2C_TX_LSB_FIRST_S  6
/** I2C_RX_LSB_FIRST : R/W; bitpos: [7]; default: 0;
 *  Configures to control the storage order for received data.\\
 *  0: receive data from the most significant bit \\
 *  1: receive data from the least significant bit \\
 */
#define I2C_RX_LSB_FIRST    (BIT(7))
#define I2C_RX_LSB_FIRST_M  (I2C_RX_LSB_FIRST_V << I2C_RX_LSB_FIRST_S)
#define I2C_RX_LSB_FIRST_V  0x00000001U
#define I2C_RX_LSB_FIRST_S  7
/** I2C_CLK_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether to gate clock signal for registers.\\
 *  0: Support clock only when registers are read or written to by software \\
 *  1: Force clock on for registers \\
 */
#define I2C_CLK_EN    (BIT(8))
#define I2C_CLK_EN_M  (I2C_CLK_EN_V << I2C_CLK_EN_S)
#define I2C_CLK_EN_V  0x00000001U
#define I2C_CLK_EN_S  8
/** I2C_ARBITRATION_EN : R/W; bitpos: [9]; default: 1;
 *  Configures to enable I2C bus arbitration detection.\\
 *  0: No effect \\
 *  1: Enable \\
 */
#define I2C_ARBITRATION_EN    (BIT(9))
#define I2C_ARBITRATION_EN_M  (I2C_ARBITRATION_EN_V << I2C_ARBITRATION_EN_S)
#define I2C_ARBITRATION_EN_V  0x00000001U
#define I2C_ARBITRATION_EN_S  9
/** I2C_FSM_RST : WT; bitpos: [10]; default: 0;
 *  Configures to reset the SCL_FSM.\\
 *  0: No effect \\
 *  1: Reset
 */
#define I2C_FSM_RST    (BIT(10))
#define I2C_FSM_RST_M  (I2C_FSM_RST_V << I2C_FSM_RST_S)
#define I2C_FSM_RST_V  0x00000001U
#define I2C_FSM_RST_S  10
/** I2C_CONF_UPGATE : WT; bitpos: [11]; default: 0;
 *  Configures this bit for synchronization.\\
 *  0: No effect \\
 *  1: Synchronize
 */
#define I2C_CONF_UPGATE    (BIT(11))
#define I2C_CONF_UPGATE_M  (I2C_CONF_UPGATE_V << I2C_CONF_UPGATE_S)
#define I2C_CONF_UPGATE_V  0x00000001U
#define I2C_CONF_UPGATE_S  11
/** I2C_SLV_TX_AUTO_START_EN : R/W; bitpos: [12]; default: 0;
 *  Configures to enable slave to send data automatically\\
 *  0: Disable\\
 *  1: Enable \\
 */
#define I2C_SLV_TX_AUTO_START_EN    (BIT(12))
#define I2C_SLV_TX_AUTO_START_EN_M  (I2C_SLV_TX_AUTO_START_EN_V << I2C_SLV_TX_AUTO_START_EN_S)
#define I2C_SLV_TX_AUTO_START_EN_V  0x00000001U
#define I2C_SLV_TX_AUTO_START_EN_S  12
/** I2C_ADDR_10BIT_RW_CHECK_EN : R/W; bitpos: [13]; default: 0;
 *  Configures to check if the r/w bit of 10bit addressing consists with I2C protocol.\\
 *  0: Not check\\
 *  1: Check
 */
#define I2C_ADDR_10BIT_RW_CHECK_EN    (BIT(13))
#define I2C_ADDR_10BIT_RW_CHECK_EN_M  (I2C_ADDR_10BIT_RW_CHECK_EN_V << I2C_ADDR_10BIT_RW_CHECK_EN_S)
#define I2C_ADDR_10BIT_RW_CHECK_EN_V  0x00000001U
#define I2C_ADDR_10BIT_RW_CHECK_EN_S  13
/** I2C_ADDR_BROADCASTING_EN : R/W; bitpos: [14]; default: 0;
 *  Configures to support the 7 bit general call function.\\
 *  0: Not support \\
 *  1: Support \\
 */
#define I2C_ADDR_BROADCASTING_EN    (BIT(14))
#define I2C_ADDR_BROADCASTING_EN_M  (I2C_ADDR_BROADCASTING_EN_V << I2C_ADDR_BROADCASTING_EN_S)
#define I2C_ADDR_BROADCASTING_EN_V  0x00000001U
#define I2C_ADDR_BROADCASTING_EN_S  14

/** I2C_SR_REG register
 *  Describe I2C work status
 */
#define I2C_SR_REG (DR_REG_I2C_BASE + 0x8)
/** I2C_RESP_REC : RO; bitpos: [0]; default: 0;
 *  Represents the received ACK value in master mode or slave mode.\\
 *  0: ACK\\
 *  1: NACK.
 */
#define I2C_RESP_REC    (BIT(0))
#define I2C_RESP_REC_M  (I2C_RESP_REC_V << I2C_RESP_REC_S)
#define I2C_RESP_REC_V  0x00000001U
#define I2C_RESP_REC_S  0
/** I2C_SLAVE_RW : RO; bitpos: [1]; default: 0;
 *  Represents the transfer direction in slave mode.\\
 *  1: Master reads from slave\\
 *  0: Master writes to slave.
 */
#define I2C_SLAVE_RW    (BIT(1))
#define I2C_SLAVE_RW_M  (I2C_SLAVE_RW_V << I2C_SLAVE_RW_S)
#define I2C_SLAVE_RW_V  0x00000001U
#define I2C_SLAVE_RW_S  1
/** I2C_ARB_LOST : RO; bitpos: [3]; default: 0;
 *  Represents whether the I2C controller loses control of SCL line.\\
 *  0: No arbitration lost\\
 *  1: Arbitration lost\\
 */
#define I2C_ARB_LOST    (BIT(3))
#define I2C_ARB_LOST_M  (I2C_ARB_LOST_V << I2C_ARB_LOST_S)
#define I2C_ARB_LOST_V  0x00000001U
#define I2C_ARB_LOST_S  3
/** I2C_BUS_BUSY : RO; bitpos: [4]; default: 0;
 *  Represents the I2C bus state.\\
 *  1: The I2C bus is busy transferring data\\
 *  0: The I2C bus is in idle state.\\
 */
#define I2C_BUS_BUSY    (BIT(4))
#define I2C_BUS_BUSY_M  (I2C_BUS_BUSY_V << I2C_BUS_BUSY_S)
#define I2C_BUS_BUSY_V  0x00000001U
#define I2C_BUS_BUSY_S  4
/** I2C_SLAVE_ADDRESSED : RO; bitpos: [5]; default: 0;
 *  Represents whether the address sent by the master is equal to the address of the
 *  slave.\\
 *  Valid only when the module is configured as an I2C Slave.\\
 *  0: Not equal\\
 *  1: Equal\\
 */
#define I2C_SLAVE_ADDRESSED    (BIT(5))
#define I2C_SLAVE_ADDRESSED_M  (I2C_SLAVE_ADDRESSED_V << I2C_SLAVE_ADDRESSED_S)
#define I2C_SLAVE_ADDRESSED_V  0x00000001U
#define I2C_SLAVE_ADDRESSED_S  5
/** I2C_RXFIFO_CNT : RO; bitpos: [13:8]; default: 0;
 *  Represents the number of data bytes received in RAM.
 */
#define I2C_RXFIFO_CNT    0x0000003FU
#define I2C_RXFIFO_CNT_M  (I2C_RXFIFO_CNT_V << I2C_RXFIFO_CNT_S)
#define I2C_RXFIFO_CNT_V  0x0000003FU
#define I2C_RXFIFO_CNT_S  8
/** I2C_STRETCH_CAUSE : RO; bitpos: [15:14]; default: 3;
 *  Represents the cause of SCL clocking stretching in slave mode.\\
 *  0: Stretching SCL low when the master starts to read data.\\
 *  1: Stretching SCL low when I2C TX FIFO is empty in slave mode.\\
 *  2: Stretching SCL low when I2C RX FIFO is full in slave mode.
 */
#define I2C_STRETCH_CAUSE    0x00000003U
#define I2C_STRETCH_CAUSE_M  (I2C_STRETCH_CAUSE_V << I2C_STRETCH_CAUSE_S)
#define I2C_STRETCH_CAUSE_V  0x00000003U
#define I2C_STRETCH_CAUSE_S  14
/** I2C_TXFIFO_CNT : RO; bitpos: [23:18]; default: 0;
 *  Represents the number of data bytes to be sent.
 */
#define I2C_TXFIFO_CNT    0x0000003FU
#define I2C_TXFIFO_CNT_M  (I2C_TXFIFO_CNT_V << I2C_TXFIFO_CNT_S)
#define I2C_TXFIFO_CNT_V  0x0000003FU
#define I2C_TXFIFO_CNT_S  18
/** I2C_SCL_MAIN_STATE_LAST : RO; bitpos: [26:24]; default: 0;
 *  Represents the states of the I2C module state machine.\\
 *  0: Idle\\
 *  1: Address shift\\
 *  2: ACK address\\
 *  3: Rx data\\
 *  4: Tx data\\
 *  5: Send ACK\\
 *  6: Wait ACK
 */
#define I2C_SCL_MAIN_STATE_LAST    0x00000007U
#define I2C_SCL_MAIN_STATE_LAST_M  (I2C_SCL_MAIN_STATE_LAST_V << I2C_SCL_MAIN_STATE_LAST_S)
#define I2C_SCL_MAIN_STATE_LAST_V  0x00000007U
#define I2C_SCL_MAIN_STATE_LAST_S  24
/** I2C_SCL_STATE_LAST : RO; bitpos: [30:28]; default: 0;
 *  Represents the states of the state machine used to produce SCL.\\
 *  0: Idle\\
 *  1: Start\\
 *  2: Negative edge\\
 *  3: Low\\
 *  4: Positive edge\\
 *  5: High\\
 *  6: Stop
 */
#define I2C_SCL_STATE_LAST    0x00000007U
#define I2C_SCL_STATE_LAST_M  (I2C_SCL_STATE_LAST_V << I2C_SCL_STATE_LAST_S)
#define I2C_SCL_STATE_LAST_V  0x00000007U
#define I2C_SCL_STATE_LAST_S  28

/** I2C_TO_REG register
 *  Setting time out control for receiving data
 */
#define I2C_TO_REG (DR_REG_I2C_BASE + 0xc)
/** I2C_TIME_OUT_VALUE : R/W; bitpos: [4:0]; default: 16;
 *  Configures the timeout threshold period for SCL stucking at high or low level. The
 *  actual period is 2\^{}(reg_time_out_value).\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_TIME_OUT_VALUE    0x0000001FU
#define I2C_TIME_OUT_VALUE_M  (I2C_TIME_OUT_VALUE_V << I2C_TIME_OUT_VALUE_S)
#define I2C_TIME_OUT_VALUE_V  0x0000001FU
#define I2C_TIME_OUT_VALUE_S  0
/** I2C_TIME_OUT_EN : R/W; bitpos: [5]; default: 0;
 *  Configures to enable time out control.\\
 *  0: No effect \\
 *  1: Enable \\
 */
#define I2C_TIME_OUT_EN    (BIT(5))
#define I2C_TIME_OUT_EN_M  (I2C_TIME_OUT_EN_V << I2C_TIME_OUT_EN_S)
#define I2C_TIME_OUT_EN_V  0x00000001U
#define I2C_TIME_OUT_EN_S  5

/** I2C_SLAVE_ADDR_REG register
 *  Local slave address setting
 */
#define I2C_SLAVE_ADDR_REG (DR_REG_I2C_BASE + 0x10)
/** I2C_SLAVE_ADDR : R/W; bitpos: [14:0]; default: 0;
 *  Configure the slave address of I2C Slave.\\
 */
#define I2C_SLAVE_ADDR    0x00007FFFU
#define I2C_SLAVE_ADDR_M  (I2C_SLAVE_ADDR_V << I2C_SLAVE_ADDR_S)
#define I2C_SLAVE_ADDR_V  0x00007FFFU
#define I2C_SLAVE_ADDR_S  0
/** I2C_ADDR_10BIT_EN : R/W; bitpos: [31]; default: 0;
 *  Configures to enable the slave 10-bit addressing mode in master mode.\\
 *  0: No effect \\
 *  1: Enable \\
 */
#define I2C_ADDR_10BIT_EN    (BIT(31))
#define I2C_ADDR_10BIT_EN_M  (I2C_ADDR_10BIT_EN_V << I2C_ADDR_10BIT_EN_S)
#define I2C_ADDR_10BIT_EN_V  0x00000001U
#define I2C_ADDR_10BIT_EN_S  31

/** I2C_FIFO_ST_REG register
 *  FIFO status register
 */
#define I2C_FIFO_ST_REG (DR_REG_I2C_BASE + 0x14)
/** I2C_RXFIFO_RADDR : RO; bitpos: [4:0]; default: 0;
 *  Represents the offset address of the APB reading from RXFIFO.
 */
#define I2C_RXFIFO_RADDR    0x0000001FU
#define I2C_RXFIFO_RADDR_M  (I2C_RXFIFO_RADDR_V << I2C_RXFIFO_RADDR_S)
#define I2C_RXFIFO_RADDR_V  0x0000001FU
#define I2C_RXFIFO_RADDR_S  0
/** I2C_RXFIFO_WADDR : RO; bitpos: [9:5]; default: 0;
 *  Represents the offset address of i2c module receiving data and writing to RXFIFO.
 */
#define I2C_RXFIFO_WADDR    0x0000001FU
#define I2C_RXFIFO_WADDR_M  (I2C_RXFIFO_WADDR_V << I2C_RXFIFO_WADDR_S)
#define I2C_RXFIFO_WADDR_V  0x0000001FU
#define I2C_RXFIFO_WADDR_S  5
/** I2C_TXFIFO_RADDR : RO; bitpos: [14:10]; default: 0;
 *  Represents the offset address of i2c module reading from TXFIFO.
 */
#define I2C_TXFIFO_RADDR    0x0000001FU
#define I2C_TXFIFO_RADDR_M  (I2C_TXFIFO_RADDR_V << I2C_TXFIFO_RADDR_S)
#define I2C_TXFIFO_RADDR_V  0x0000001FU
#define I2C_TXFIFO_RADDR_S  10
/** I2C_TXFIFO_WADDR : RO; bitpos: [19:15]; default: 0;
 *  Represents the offset address of APB bus writing to TXFIFO.
 */
#define I2C_TXFIFO_WADDR    0x0000001FU
#define I2C_TXFIFO_WADDR_M  (I2C_TXFIFO_WADDR_V << I2C_TXFIFO_WADDR_S)
#define I2C_TXFIFO_WADDR_V  0x0000001FU
#define I2C_TXFIFO_WADDR_S  15
/** I2C_SLAVE_RW_POINT : RO; bitpos: [29:22]; default: 0;
 *  Represents the offset address in the I2C Slave RAM addressed by I2C Master when in
 *  I2C slave mode.
 */
#define I2C_SLAVE_RW_POINT    0x000000FFU
#define I2C_SLAVE_RW_POINT_M  (I2C_SLAVE_RW_POINT_V << I2C_SLAVE_RW_POINT_S)
#define I2C_SLAVE_RW_POINT_V  0x000000FFU
#define I2C_SLAVE_RW_POINT_S  22

/** I2C_FIFO_CONF_REG register
 *  FIFO configuration register
 */
#define I2C_FIFO_CONF_REG (DR_REG_I2C_BASE + 0x18)
/** I2C_RXFIFO_WM_THRHD : R/W; bitpos: [4:0]; default: 11;
 *  Configures the water mark threshold of RXFIFO in nonfifo access mode. When
 *  I2C_FIFO_PRT_EN is 1 and RX FIFO counter is bigger than I2C_RXFIFO_WM_THRHD[4:0],
 *  I2C_RXFIFO_WM_INT_RAW bit will be valid.
 *  \tododone{For CJ, please check this description. I habe doubt about
 *  reg_reg_fifo_prt_en.CJ: modified}
 */
#define I2C_RXFIFO_WM_THRHD    0x0000001FU
#define I2C_RXFIFO_WM_THRHD_M  (I2C_RXFIFO_WM_THRHD_V << I2C_RXFIFO_WM_THRHD_S)
#define I2C_RXFIFO_WM_THRHD_V  0x0000001FU
#define I2C_RXFIFO_WM_THRHD_S  0
/** I2C_TXFIFO_WM_THRHD : R/W; bitpos: [9:5]; default: 4;
 *  Configures the water mark threshold of TXFIFO in nonfifo access mode.  When
 *  I2C_FIFO_PRT_EN is 1 and TC FIFO counter is bigger than I2C_TXFIFO_WM_THRHD[4:0],
 *  I2C_TXFIFO_WM_INT_RAW bit will be valid.
 */
#define I2C_TXFIFO_WM_THRHD    0x0000001FU
#define I2C_TXFIFO_WM_THRHD_M  (I2C_TXFIFO_WM_THRHD_V << I2C_TXFIFO_WM_THRHD_S)
#define I2C_TXFIFO_WM_THRHD_V  0x0000001FU
#define I2C_TXFIFO_WM_THRHD_S  5
/** I2C_NONFIFO_EN : R/W; bitpos: [10]; default: 0;
 *  Configures to enable APB nonfifo access.
 */
#define I2C_NONFIFO_EN    (BIT(10))
#define I2C_NONFIFO_EN_M  (I2C_NONFIFO_EN_V << I2C_NONFIFO_EN_S)
#define I2C_NONFIFO_EN_V  0x00000001U
#define I2C_NONFIFO_EN_S  10
/** I2C_FIFO_ADDR_CFG_EN : R/W; bitpos: [11]; default: 0;
 *  Configures the slave to enable dual address mode. When this mode is enabled, the
 *  byte received after the I2C address byte represents the offset address in the I2C
 *  Slave RAM. \\
 *  0: Disable\\
 *  1: Enable \\
 */
#define I2C_FIFO_ADDR_CFG_EN    (BIT(11))
#define I2C_FIFO_ADDR_CFG_EN_M  (I2C_FIFO_ADDR_CFG_EN_V << I2C_FIFO_ADDR_CFG_EN_S)
#define I2C_FIFO_ADDR_CFG_EN_V  0x00000001U
#define I2C_FIFO_ADDR_CFG_EN_S  11
/** I2C_RX_FIFO_RST : R/W; bitpos: [12]; default: 0;
 *  Configures to reset RXFIFO.\\
 *  0: No effect \\
 *  1: Reset
 */
#define I2C_RX_FIFO_RST    (BIT(12))
#define I2C_RX_FIFO_RST_M  (I2C_RX_FIFO_RST_V << I2C_RX_FIFO_RST_S)
#define I2C_RX_FIFO_RST_V  0x00000001U
#define I2C_RX_FIFO_RST_S  12
/** I2C_TX_FIFO_RST : R/W; bitpos: [13]; default: 0;
 *  Configures to reset TXFIFO.\\
 *  0: No effect \\
 *  1: Reset
 */
#define I2C_TX_FIFO_RST    (BIT(13))
#define I2C_TX_FIFO_RST_M  (I2C_TX_FIFO_RST_V << I2C_TX_FIFO_RST_S)
#define I2C_TX_FIFO_RST_V  0x00000001U
#define I2C_TX_FIFO_RST_S  13
/** I2C_FIFO_PRT_EN : R/W; bitpos: [14]; default: 1;
 *  Configures to enable FIFO pointer in non-fifo access mode. This bit controls the
 *  valid bits and the TX/RX FIFO overflow, underflow, full and empty interrupts.\\
 *  0: No effect \\
 *  1: Enable \\
 */
#define I2C_FIFO_PRT_EN    (BIT(14))
#define I2C_FIFO_PRT_EN_M  (I2C_FIFO_PRT_EN_V << I2C_FIFO_PRT_EN_S)
#define I2C_FIFO_PRT_EN_V  0x00000001U
#define I2C_FIFO_PRT_EN_S  14

/** I2C_DATA_REG register
 *  Rx FIFO read data
 */
#define I2C_DATA_REG (DR_REG_I2C_BASE + 0x1c)
/** I2C_FIFO_RDATA : HRO; bitpos: [7:0]; default: 0;
 *  Represents the value of RXFIFO read data.
 */
#define I2C_FIFO_RDATA    0x000000FFU
#define I2C_FIFO_RDATA_M  (I2C_FIFO_RDATA_V << I2C_FIFO_RDATA_S)
#define I2C_FIFO_RDATA_V  0x000000FFU
#define I2C_FIFO_RDATA_S  0

/** I2C_INT_RAW_REG register
 *  Raw interrupt status
 */
#define I2C_INT_RAW_REG (DR_REG_I2C_BASE + 0x20)
/** I2C_RXFIFO_WM_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  The raw interrupt status of I2C_RXFIFO_WM_INT interrupt.
 */
#define I2C_RXFIFO_WM_INT_RAW    (BIT(0))
#define I2C_RXFIFO_WM_INT_RAW_M  (I2C_RXFIFO_WM_INT_RAW_V << I2C_RXFIFO_WM_INT_RAW_S)
#define I2C_RXFIFO_WM_INT_RAW_V  0x00000001U
#define I2C_RXFIFO_WM_INT_RAW_S  0
/** I2C_TXFIFO_WM_INT_RAW : R/SS/WTC; bitpos: [1]; default: 1;
 *  The raw interrupt status of I2C_TXFIFO_WM_INT interrupt.
 */
#define I2C_TXFIFO_WM_INT_RAW    (BIT(1))
#define I2C_TXFIFO_WM_INT_RAW_M  (I2C_TXFIFO_WM_INT_RAW_V << I2C_TXFIFO_WM_INT_RAW_S)
#define I2C_TXFIFO_WM_INT_RAW_V  0x00000001U
#define I2C_TXFIFO_WM_INT_RAW_S  1
/** I2C_RXFIFO_OVF_INT_RAW : R/SS/WTC; bitpos: [2]; default: 0;
 *  The raw interrupt status of I2C_RXFIFO_OVF_INT interrupt.
 */
#define I2C_RXFIFO_OVF_INT_RAW    (BIT(2))
#define I2C_RXFIFO_OVF_INT_RAW_M  (I2C_RXFIFO_OVF_INT_RAW_V << I2C_RXFIFO_OVF_INT_RAW_S)
#define I2C_RXFIFO_OVF_INT_RAW_V  0x00000001U
#define I2C_RXFIFO_OVF_INT_RAW_S  2
/** I2C_END_DETECT_INT_RAW : R/SS/WTC; bitpos: [3]; default: 0;
 *  The raw interrupt status of the I2C_END_DETECT_INT interrupt.
 */
#define I2C_END_DETECT_INT_RAW    (BIT(3))
#define I2C_END_DETECT_INT_RAW_M  (I2C_END_DETECT_INT_RAW_V << I2C_END_DETECT_INT_RAW_S)
#define I2C_END_DETECT_INT_RAW_V  0x00000001U
#define I2C_END_DETECT_INT_RAW_S  3
/** I2C_BYTE_TRANS_DONE_INT_RAW : R/SS/WTC; bitpos: [4]; default: 0;
 *  The raw interrupt status of the I2C_END_DETECT_INT interrupt.
 */
#define I2C_BYTE_TRANS_DONE_INT_RAW    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_RAW_M  (I2C_BYTE_TRANS_DONE_INT_RAW_V << I2C_BYTE_TRANS_DONE_INT_RAW_S)
#define I2C_BYTE_TRANS_DONE_INT_RAW_V  0x00000001U
#define I2C_BYTE_TRANS_DONE_INT_RAW_S  4
/** I2C_ARBITRATION_LOST_INT_RAW : R/SS/WTC; bitpos: [5]; default: 0;
 *  The raw interrupt status of the I2C_ARBITRATION_LOST_INT interrupt.
 */
#define I2C_ARBITRATION_LOST_INT_RAW    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_RAW_M  (I2C_ARBITRATION_LOST_INT_RAW_V << I2C_ARBITRATION_LOST_INT_RAW_S)
#define I2C_ARBITRATION_LOST_INT_RAW_V  0x00000001U
#define I2C_ARBITRATION_LOST_INT_RAW_S  5
/** I2C_MST_TXFIFO_UDF_INT_RAW : R/SS/WTC; bitpos: [6]; default: 0;
 *  The raw interrupt status of I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_MST_TXFIFO_UDF_INT_RAW    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_RAW_M  (I2C_MST_TXFIFO_UDF_INT_RAW_V << I2C_MST_TXFIFO_UDF_INT_RAW_S)
#define I2C_MST_TXFIFO_UDF_INT_RAW_V  0x00000001U
#define I2C_MST_TXFIFO_UDF_INT_RAW_S  6
/** I2C_TRANS_COMPLETE_INT_RAW : R/SS/WTC; bitpos: [7]; default: 0;
 *  The raw interrupt status of the I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_TRANS_COMPLETE_INT_RAW    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_RAW_M  (I2C_TRANS_COMPLETE_INT_RAW_V << I2C_TRANS_COMPLETE_INT_RAW_S)
#define I2C_TRANS_COMPLETE_INT_RAW_V  0x00000001U
#define I2C_TRANS_COMPLETE_INT_RAW_S  7
/** I2C_TIME_OUT_INT_RAW : R/SS/WTC; bitpos: [8]; default: 0;
 *  The raw interrupt status of the I2C_TIME_OUT_INT interrupt.
 */
#define I2C_TIME_OUT_INT_RAW    (BIT(8))
#define I2C_TIME_OUT_INT_RAW_M  (I2C_TIME_OUT_INT_RAW_V << I2C_TIME_OUT_INT_RAW_S)
#define I2C_TIME_OUT_INT_RAW_V  0x00000001U
#define I2C_TIME_OUT_INT_RAW_S  8
/** I2C_TRANS_START_INT_RAW : R/SS/WTC; bitpos: [9]; default: 0;
 *  The raw interrupt status of the I2C_TRANS_START_INT interrupt.
 */
#define I2C_TRANS_START_INT_RAW    (BIT(9))
#define I2C_TRANS_START_INT_RAW_M  (I2C_TRANS_START_INT_RAW_V << I2C_TRANS_START_INT_RAW_S)
#define I2C_TRANS_START_INT_RAW_V  0x00000001U
#define I2C_TRANS_START_INT_RAW_S  9
/** I2C_NACK_INT_RAW : R/SS/WTC; bitpos: [10]; default: 0;
 *  The raw interrupt status of I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_NACK_INT_RAW    (BIT(10))
#define I2C_NACK_INT_RAW_M  (I2C_NACK_INT_RAW_V << I2C_NACK_INT_RAW_S)
#define I2C_NACK_INT_RAW_V  0x00000001U
#define I2C_NACK_INT_RAW_S  10
/** I2C_TXFIFO_OVF_INT_RAW : R/SS/WTC; bitpos: [11]; default: 0;
 *  The raw interrupt status of I2C_TXFIFO_OVF_INT interrupt.
 */
#define I2C_TXFIFO_OVF_INT_RAW    (BIT(11))
#define I2C_TXFIFO_OVF_INT_RAW_M  (I2C_TXFIFO_OVF_INT_RAW_V << I2C_TXFIFO_OVF_INT_RAW_S)
#define I2C_TXFIFO_OVF_INT_RAW_V  0x00000001U
#define I2C_TXFIFO_OVF_INT_RAW_S  11
/** I2C_RXFIFO_UDF_INT_RAW : R/SS/WTC; bitpos: [12]; default: 0;
 *  The raw interrupt status of I2C_RXFIFO_UDF_INT  interrupt.
 */
#define I2C_RXFIFO_UDF_INT_RAW    (BIT(12))
#define I2C_RXFIFO_UDF_INT_RAW_M  (I2C_RXFIFO_UDF_INT_RAW_V << I2C_RXFIFO_UDF_INT_RAW_S)
#define I2C_RXFIFO_UDF_INT_RAW_V  0x00000001U
#define I2C_RXFIFO_UDF_INT_RAW_S  12
/** I2C_SCL_ST_TO_INT_RAW : R/SS/WTC; bitpos: [13]; default: 0;
 *  The raw interrupt status of I2C_SCL_ST_TO_INT interrupt.
 */
#define I2C_SCL_ST_TO_INT_RAW    (BIT(13))
#define I2C_SCL_ST_TO_INT_RAW_M  (I2C_SCL_ST_TO_INT_RAW_V << I2C_SCL_ST_TO_INT_RAW_S)
#define I2C_SCL_ST_TO_INT_RAW_V  0x00000001U
#define I2C_SCL_ST_TO_INT_RAW_S  13
/** I2C_SCL_MAIN_ST_TO_INT_RAW : R/SS/WTC; bitpos: [14]; default: 0;
 *  The raw interrupt status of I2C_SCL_MAIN_ST_TO_INT interrupt.
 */
#define I2C_SCL_MAIN_ST_TO_INT_RAW    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_RAW_M  (I2C_SCL_MAIN_ST_TO_INT_RAW_V << I2C_SCL_MAIN_ST_TO_INT_RAW_S)
#define I2C_SCL_MAIN_ST_TO_INT_RAW_V  0x00000001U
#define I2C_SCL_MAIN_ST_TO_INT_RAW_S  14
/** I2C_DET_START_INT_RAW : R/SS/WTC; bitpos: [15]; default: 0;
 *  The raw interrupt status of I2C_DET_START_INT interrupt.
 */
#define I2C_DET_START_INT_RAW    (BIT(15))
#define I2C_DET_START_INT_RAW_M  (I2C_DET_START_INT_RAW_V << I2C_DET_START_INT_RAW_S)
#define I2C_DET_START_INT_RAW_V  0x00000001U
#define I2C_DET_START_INT_RAW_S  15
/** I2C_SLAVE_STRETCH_INT_RAW : R/SS/WTC; bitpos: [16]; default: 0;
 *  The raw interrupt status of I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_SLAVE_STRETCH_INT_RAW    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_RAW_M  (I2C_SLAVE_STRETCH_INT_RAW_V << I2C_SLAVE_STRETCH_INT_RAW_S)
#define I2C_SLAVE_STRETCH_INT_RAW_V  0x00000001U
#define I2C_SLAVE_STRETCH_INT_RAW_S  16
/** I2C_GENERAL_CALL_INT_RAW : R/SS/WTC; bitpos: [17]; default: 0;
 *  The raw interrupt status of I2C_GENARAL_CALL_INT interrupt.
 */
#define I2C_GENERAL_CALL_INT_RAW    (BIT(17))
#define I2C_GENERAL_CALL_INT_RAW_M  (I2C_GENERAL_CALL_INT_RAW_V << I2C_GENERAL_CALL_INT_RAW_S)
#define I2C_GENERAL_CALL_INT_RAW_V  0x00000001U
#define I2C_GENERAL_CALL_INT_RAW_S  17
/** I2C_SLAVE_ADDR_UNMATCH_INT_RAW : R/SS/WTC; bitpos: [18]; default: 0;
 *  The raw interrupt status of I2C_SLAVE_ADDR_UNMATCH_INT_RAW interrupt.
 */
#define I2C_SLAVE_ADDR_UNMATCH_INT_RAW    (BIT(18))
#define I2C_SLAVE_ADDR_UNMATCH_INT_RAW_M  (I2C_SLAVE_ADDR_UNMATCH_INT_RAW_V << I2C_SLAVE_ADDR_UNMATCH_INT_RAW_S)
#define I2C_SLAVE_ADDR_UNMATCH_INT_RAW_V  0x00000001U
#define I2C_SLAVE_ADDR_UNMATCH_INT_RAW_S  18

/** I2C_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define I2C_INT_CLR_REG (DR_REG_I2C_BASE + 0x24)
/** I2C_RXFIFO_WM_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear I2C_RXFIFO_WM_INT interrupt.
 */
#define I2C_RXFIFO_WM_INT_CLR    (BIT(0))
#define I2C_RXFIFO_WM_INT_CLR_M  (I2C_RXFIFO_WM_INT_CLR_V << I2C_RXFIFO_WM_INT_CLR_S)
#define I2C_RXFIFO_WM_INT_CLR_V  0x00000001U
#define I2C_RXFIFO_WM_INT_CLR_S  0
/** I2C_TXFIFO_WM_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear I2C_TXFIFO_WM_INT interrupt.
 */
#define I2C_TXFIFO_WM_INT_CLR    (BIT(1))
#define I2C_TXFIFO_WM_INT_CLR_M  (I2C_TXFIFO_WM_INT_CLR_V << I2C_TXFIFO_WM_INT_CLR_S)
#define I2C_TXFIFO_WM_INT_CLR_V  0x00000001U
#define I2C_TXFIFO_WM_INT_CLR_S  1
/** I2C_RXFIFO_OVF_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear I2C_RXFIFO_OVF_INT interrupt.
 */
#define I2C_RXFIFO_OVF_INT_CLR    (BIT(2))
#define I2C_RXFIFO_OVF_INT_CLR_M  (I2C_RXFIFO_OVF_INT_CLR_V << I2C_RXFIFO_OVF_INT_CLR_S)
#define I2C_RXFIFO_OVF_INT_CLR_V  0x00000001U
#define I2C_RXFIFO_OVF_INT_CLR_S  2
/** I2C_END_DETECT_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Write 1 to clear the I2C_END_DETECT_INT interrupt.
 */
#define I2C_END_DETECT_INT_CLR    (BIT(3))
#define I2C_END_DETECT_INT_CLR_M  (I2C_END_DETECT_INT_CLR_V << I2C_END_DETECT_INT_CLR_S)
#define I2C_END_DETECT_INT_CLR_V  0x00000001U
#define I2C_END_DETECT_INT_CLR_S  3
/** I2C_BYTE_TRANS_DONE_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear the I2C_END_DETECT_INT interrupt.
 */
#define I2C_BYTE_TRANS_DONE_INT_CLR    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_CLR_M  (I2C_BYTE_TRANS_DONE_INT_CLR_V << I2C_BYTE_TRANS_DONE_INT_CLR_S)
#define I2C_BYTE_TRANS_DONE_INT_CLR_V  0x00000001U
#define I2C_BYTE_TRANS_DONE_INT_CLR_S  4
/** I2C_ARBITRATION_LOST_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Write 1 to clear the I2C_ARBITRATION_LOST_INT interrupt.
 */
#define I2C_ARBITRATION_LOST_INT_CLR    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_CLR_M  (I2C_ARBITRATION_LOST_INT_CLR_V << I2C_ARBITRATION_LOST_INT_CLR_S)
#define I2C_ARBITRATION_LOST_INT_CLR_V  0x00000001U
#define I2C_ARBITRATION_LOST_INT_CLR_S  5
/** I2C_MST_TXFIFO_UDF_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Write 1 to clear I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_MST_TXFIFO_UDF_INT_CLR    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_CLR_M  (I2C_MST_TXFIFO_UDF_INT_CLR_V << I2C_MST_TXFIFO_UDF_INT_CLR_S)
#define I2C_MST_TXFIFO_UDF_INT_CLR_V  0x00000001U
#define I2C_MST_TXFIFO_UDF_INT_CLR_S  6
/** I2C_TRANS_COMPLETE_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Write 1 to clear the I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_TRANS_COMPLETE_INT_CLR    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_CLR_M  (I2C_TRANS_COMPLETE_INT_CLR_V << I2C_TRANS_COMPLETE_INT_CLR_S)
#define I2C_TRANS_COMPLETE_INT_CLR_V  0x00000001U
#define I2C_TRANS_COMPLETE_INT_CLR_S  7
/** I2C_TIME_OUT_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Write 1 to clear the I2C_TIME_OUT_INT interrupt.
 */
#define I2C_TIME_OUT_INT_CLR    (BIT(8))
#define I2C_TIME_OUT_INT_CLR_M  (I2C_TIME_OUT_INT_CLR_V << I2C_TIME_OUT_INT_CLR_S)
#define I2C_TIME_OUT_INT_CLR_V  0x00000001U
#define I2C_TIME_OUT_INT_CLR_S  8
/** I2C_TRANS_START_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Write 1 to clear the I2C_TRANS_START_INT interrupt.
 */
#define I2C_TRANS_START_INT_CLR    (BIT(9))
#define I2C_TRANS_START_INT_CLR_M  (I2C_TRANS_START_INT_CLR_V << I2C_TRANS_START_INT_CLR_S)
#define I2C_TRANS_START_INT_CLR_V  0x00000001U
#define I2C_TRANS_START_INT_CLR_S  9
/** I2C_NACK_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Write 1 to clear I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_NACK_INT_CLR    (BIT(10))
#define I2C_NACK_INT_CLR_M  (I2C_NACK_INT_CLR_V << I2C_NACK_INT_CLR_S)
#define I2C_NACK_INT_CLR_V  0x00000001U
#define I2C_NACK_INT_CLR_S  10
/** I2C_TXFIFO_OVF_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Write 1 to clear I2C_TXFIFO_OVF_INT interrupt.
 */
#define I2C_TXFIFO_OVF_INT_CLR    (BIT(11))
#define I2C_TXFIFO_OVF_INT_CLR_M  (I2C_TXFIFO_OVF_INT_CLR_V << I2C_TXFIFO_OVF_INT_CLR_S)
#define I2C_TXFIFO_OVF_INT_CLR_V  0x00000001U
#define I2C_TXFIFO_OVF_INT_CLR_S  11
/** I2C_RXFIFO_UDF_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Write 1 to clear I2C_RXFIFO_UDF_INT  interrupt.
 */
#define I2C_RXFIFO_UDF_INT_CLR    (BIT(12))
#define I2C_RXFIFO_UDF_INT_CLR_M  (I2C_RXFIFO_UDF_INT_CLR_V << I2C_RXFIFO_UDF_INT_CLR_S)
#define I2C_RXFIFO_UDF_INT_CLR_V  0x00000001U
#define I2C_RXFIFO_UDF_INT_CLR_S  12
/** I2C_SCL_ST_TO_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Write 1 to clear I2C_SCL_ST_TO_INT interrupt.
 */
#define I2C_SCL_ST_TO_INT_CLR    (BIT(13))
#define I2C_SCL_ST_TO_INT_CLR_M  (I2C_SCL_ST_TO_INT_CLR_V << I2C_SCL_ST_TO_INT_CLR_S)
#define I2C_SCL_ST_TO_INT_CLR_V  0x00000001U
#define I2C_SCL_ST_TO_INT_CLR_S  13
/** I2C_SCL_MAIN_ST_TO_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Write 1 to clear I2C_SCL_MAIN_ST_TO_INT interrupt.
 */
#define I2C_SCL_MAIN_ST_TO_INT_CLR    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_CLR_M  (I2C_SCL_MAIN_ST_TO_INT_CLR_V << I2C_SCL_MAIN_ST_TO_INT_CLR_S)
#define I2C_SCL_MAIN_ST_TO_INT_CLR_V  0x00000001U
#define I2C_SCL_MAIN_ST_TO_INT_CLR_S  14
/** I2C_DET_START_INT_CLR : WT; bitpos: [15]; default: 0;
 *  Write 1 to clear I2C_DET_START_INT interrupt.
 */
#define I2C_DET_START_INT_CLR    (BIT(15))
#define I2C_DET_START_INT_CLR_M  (I2C_DET_START_INT_CLR_V << I2C_DET_START_INT_CLR_S)
#define I2C_DET_START_INT_CLR_V  0x00000001U
#define I2C_DET_START_INT_CLR_S  15
/** I2C_SLAVE_STRETCH_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Write 1 to clear I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_SLAVE_STRETCH_INT_CLR    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_CLR_M  (I2C_SLAVE_STRETCH_INT_CLR_V << I2C_SLAVE_STRETCH_INT_CLR_S)
#define I2C_SLAVE_STRETCH_INT_CLR_V  0x00000001U
#define I2C_SLAVE_STRETCH_INT_CLR_S  16
/** I2C_GENERAL_CALL_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Write 1 to clear I2C_GENARAL_CALL_INT interrupt.
 */
#define I2C_GENERAL_CALL_INT_CLR    (BIT(17))
#define I2C_GENERAL_CALL_INT_CLR_M  (I2C_GENERAL_CALL_INT_CLR_V << I2C_GENERAL_CALL_INT_CLR_S)
#define I2C_GENERAL_CALL_INT_CLR_V  0x00000001U
#define I2C_GENERAL_CALL_INT_CLR_S  17
/** I2C_SLAVE_ADDR_UNMATCH_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Write 1 to clear I2C_SLAVE_ADDR_UNMATCH_INT_RAW interrupt.
 */
#define I2C_SLAVE_ADDR_UNMATCH_INT_CLR    (BIT(18))
#define I2C_SLAVE_ADDR_UNMATCH_INT_CLR_M  (I2C_SLAVE_ADDR_UNMATCH_INT_CLR_V << I2C_SLAVE_ADDR_UNMATCH_INT_CLR_S)
#define I2C_SLAVE_ADDR_UNMATCH_INT_CLR_V  0x00000001U
#define I2C_SLAVE_ADDR_UNMATCH_INT_CLR_S  18

/** I2C_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define I2C_INT_ENA_REG (DR_REG_I2C_BASE + 0x28)
/** I2C_RXFIFO_WM_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable I2C_RXFIFO_WM_INT interrupt.
 */
#define I2C_RXFIFO_WM_INT_ENA    (BIT(0))
#define I2C_RXFIFO_WM_INT_ENA_M  (I2C_RXFIFO_WM_INT_ENA_V << I2C_RXFIFO_WM_INT_ENA_S)
#define I2C_RXFIFO_WM_INT_ENA_V  0x00000001U
#define I2C_RXFIFO_WM_INT_ENA_S  0
/** I2C_TXFIFO_WM_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable I2C_TXFIFO_WM_INT interrupt.
 */
#define I2C_TXFIFO_WM_INT_ENA    (BIT(1))
#define I2C_TXFIFO_WM_INT_ENA_M  (I2C_TXFIFO_WM_INT_ENA_V << I2C_TXFIFO_WM_INT_ENA_S)
#define I2C_TXFIFO_WM_INT_ENA_V  0x00000001U
#define I2C_TXFIFO_WM_INT_ENA_S  1
/** I2C_RXFIFO_OVF_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable I2C_RXFIFO_OVF_INT interrupt.
 */
#define I2C_RXFIFO_OVF_INT_ENA    (BIT(2))
#define I2C_RXFIFO_OVF_INT_ENA_M  (I2C_RXFIFO_OVF_INT_ENA_V << I2C_RXFIFO_OVF_INT_ENA_S)
#define I2C_RXFIFO_OVF_INT_ENA_V  0x00000001U
#define I2C_RXFIFO_OVF_INT_ENA_S  2
/** I2C_END_DETECT_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Write 1 to enable the I2C_END_DETECT_INT interrupt.
 */
#define I2C_END_DETECT_INT_ENA    (BIT(3))
#define I2C_END_DETECT_INT_ENA_M  (I2C_END_DETECT_INT_ENA_V << I2C_END_DETECT_INT_ENA_S)
#define I2C_END_DETECT_INT_ENA_V  0x00000001U
#define I2C_END_DETECT_INT_ENA_S  3
/** I2C_BYTE_TRANS_DONE_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable the I2C_END_DETECT_INT interrupt.
 */
#define I2C_BYTE_TRANS_DONE_INT_ENA    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_ENA_M  (I2C_BYTE_TRANS_DONE_INT_ENA_V << I2C_BYTE_TRANS_DONE_INT_ENA_S)
#define I2C_BYTE_TRANS_DONE_INT_ENA_V  0x00000001U
#define I2C_BYTE_TRANS_DONE_INT_ENA_S  4
/** I2C_ARBITRATION_LOST_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Write 1 to enable the I2C_ARBITRATION_LOST_INT interrupt.
 */
#define I2C_ARBITRATION_LOST_INT_ENA    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ENA_M  (I2C_ARBITRATION_LOST_INT_ENA_V << I2C_ARBITRATION_LOST_INT_ENA_S)
#define I2C_ARBITRATION_LOST_INT_ENA_V  0x00000001U
#define I2C_ARBITRATION_LOST_INT_ENA_S  5
/** I2C_MST_TXFIFO_UDF_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Write 1 to enable I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_MST_TXFIFO_UDF_INT_ENA    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_ENA_M  (I2C_MST_TXFIFO_UDF_INT_ENA_V << I2C_MST_TXFIFO_UDF_INT_ENA_S)
#define I2C_MST_TXFIFO_UDF_INT_ENA_V  0x00000001U
#define I2C_MST_TXFIFO_UDF_INT_ENA_S  6
/** I2C_TRANS_COMPLETE_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Write 1 to enable the I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_TRANS_COMPLETE_INT_ENA    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ENA_M  (I2C_TRANS_COMPLETE_INT_ENA_V << I2C_TRANS_COMPLETE_INT_ENA_S)
#define I2C_TRANS_COMPLETE_INT_ENA_V  0x00000001U
#define I2C_TRANS_COMPLETE_INT_ENA_S  7
/** I2C_TIME_OUT_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Write 1 to enable the I2C_TIME_OUT_INT interrupt.
 */
#define I2C_TIME_OUT_INT_ENA    (BIT(8))
#define I2C_TIME_OUT_INT_ENA_M  (I2C_TIME_OUT_INT_ENA_V << I2C_TIME_OUT_INT_ENA_S)
#define I2C_TIME_OUT_INT_ENA_V  0x00000001U
#define I2C_TIME_OUT_INT_ENA_S  8
/** I2C_TRANS_START_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Write 1 to enable the I2C_TRANS_START_INT interrupt.
 */
#define I2C_TRANS_START_INT_ENA    (BIT(9))
#define I2C_TRANS_START_INT_ENA_M  (I2C_TRANS_START_INT_ENA_V << I2C_TRANS_START_INT_ENA_S)
#define I2C_TRANS_START_INT_ENA_V  0x00000001U
#define I2C_TRANS_START_INT_ENA_S  9
/** I2C_NACK_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  Write 1 to enable I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_NACK_INT_ENA    (BIT(10))
#define I2C_NACK_INT_ENA_M  (I2C_NACK_INT_ENA_V << I2C_NACK_INT_ENA_S)
#define I2C_NACK_INT_ENA_V  0x00000001U
#define I2C_NACK_INT_ENA_S  10
/** I2C_TXFIFO_OVF_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Write 1 to enable I2C_TXFIFO_OVF_INT interrupt.
 */
#define I2C_TXFIFO_OVF_INT_ENA    (BIT(11))
#define I2C_TXFIFO_OVF_INT_ENA_M  (I2C_TXFIFO_OVF_INT_ENA_V << I2C_TXFIFO_OVF_INT_ENA_S)
#define I2C_TXFIFO_OVF_INT_ENA_V  0x00000001U
#define I2C_TXFIFO_OVF_INT_ENA_S  11
/** I2C_RXFIFO_UDF_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  Write 1 to enable I2C_RXFIFO_UDF_INT  interrupt.
 */
#define I2C_RXFIFO_UDF_INT_ENA    (BIT(12))
#define I2C_RXFIFO_UDF_INT_ENA_M  (I2C_RXFIFO_UDF_INT_ENA_V << I2C_RXFIFO_UDF_INT_ENA_S)
#define I2C_RXFIFO_UDF_INT_ENA_V  0x00000001U
#define I2C_RXFIFO_UDF_INT_ENA_S  12
/** I2C_SCL_ST_TO_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  Write 1 to enable I2C_SCL_ST_TO_INT interrupt.
 */
#define I2C_SCL_ST_TO_INT_ENA    (BIT(13))
#define I2C_SCL_ST_TO_INT_ENA_M  (I2C_SCL_ST_TO_INT_ENA_V << I2C_SCL_ST_TO_INT_ENA_S)
#define I2C_SCL_ST_TO_INT_ENA_V  0x00000001U
#define I2C_SCL_ST_TO_INT_ENA_S  13
/** I2C_SCL_MAIN_ST_TO_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  Write 1 to enable I2C_SCL_MAIN_ST_TO_INT interrupt.
 */
#define I2C_SCL_MAIN_ST_TO_INT_ENA    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_ENA_M  (I2C_SCL_MAIN_ST_TO_INT_ENA_V << I2C_SCL_MAIN_ST_TO_INT_ENA_S)
#define I2C_SCL_MAIN_ST_TO_INT_ENA_V  0x00000001U
#define I2C_SCL_MAIN_ST_TO_INT_ENA_S  14
/** I2C_DET_START_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  Write 1 to enable I2C_DET_START_INT interrupt.
 */
#define I2C_DET_START_INT_ENA    (BIT(15))
#define I2C_DET_START_INT_ENA_M  (I2C_DET_START_INT_ENA_V << I2C_DET_START_INT_ENA_S)
#define I2C_DET_START_INT_ENA_V  0x00000001U
#define I2C_DET_START_INT_ENA_S  15
/** I2C_SLAVE_STRETCH_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  Write 1 to enable I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_SLAVE_STRETCH_INT_ENA    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_ENA_M  (I2C_SLAVE_STRETCH_INT_ENA_V << I2C_SLAVE_STRETCH_INT_ENA_S)
#define I2C_SLAVE_STRETCH_INT_ENA_V  0x00000001U
#define I2C_SLAVE_STRETCH_INT_ENA_S  16
/** I2C_GENERAL_CALL_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  Write 1 to enable I2C_GENARAL_CALL_INT interrupt.
 */
#define I2C_GENERAL_CALL_INT_ENA    (BIT(17))
#define I2C_GENERAL_CALL_INT_ENA_M  (I2C_GENERAL_CALL_INT_ENA_V << I2C_GENERAL_CALL_INT_ENA_S)
#define I2C_GENERAL_CALL_INT_ENA_V  0x00000001U
#define I2C_GENERAL_CALL_INT_ENA_S  17
/** I2C_SLAVE_ADDR_UNMATCH_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  Write 1 to enable I2C_SLAVE_ADDR_UNMATCH_INT interrupt.
 */
#define I2C_SLAVE_ADDR_UNMATCH_INT_ENA    (BIT(18))
#define I2C_SLAVE_ADDR_UNMATCH_INT_ENA_M  (I2C_SLAVE_ADDR_UNMATCH_INT_ENA_V << I2C_SLAVE_ADDR_UNMATCH_INT_ENA_S)
#define I2C_SLAVE_ADDR_UNMATCH_INT_ENA_V  0x00000001U
#define I2C_SLAVE_ADDR_UNMATCH_INT_ENA_S  18

/** I2C_INT_STATUS_REG register
 *  Status of captured I2C communication events
 */
#define I2C_INT_STATUS_REG (DR_REG_I2C_BASE + 0x2c)
/** I2C_RXFIFO_WM_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status status of I2C_RXFIFO_WM_INT interrupt.
 */
#define I2C_RXFIFO_WM_INT_ST    (BIT(0))
#define I2C_RXFIFO_WM_INT_ST_M  (I2C_RXFIFO_WM_INT_ST_V << I2C_RXFIFO_WM_INT_ST_S)
#define I2C_RXFIFO_WM_INT_ST_V  0x00000001U
#define I2C_RXFIFO_WM_INT_ST_S  0
/** I2C_TXFIFO_WM_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status status of I2C_TXFIFO_WM_INT interrupt.
 */
#define I2C_TXFIFO_WM_INT_ST    (BIT(1))
#define I2C_TXFIFO_WM_INT_ST_M  (I2C_TXFIFO_WM_INT_ST_V << I2C_TXFIFO_WM_INT_ST_S)
#define I2C_TXFIFO_WM_INT_ST_V  0x00000001U
#define I2C_TXFIFO_WM_INT_ST_S  1
/** I2C_RXFIFO_OVF_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status status of I2C_RXFIFO_OVF_INT interrupt.
 */
#define I2C_RXFIFO_OVF_INT_ST    (BIT(2))
#define I2C_RXFIFO_OVF_INT_ST_M  (I2C_RXFIFO_OVF_INT_ST_V << I2C_RXFIFO_OVF_INT_ST_S)
#define I2C_RXFIFO_OVF_INT_ST_V  0x00000001U
#define I2C_RXFIFO_OVF_INT_ST_S  2
/** I2C_END_DETECT_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status status of the I2C_END_DETECT_INT interrupt.
 */
#define I2C_END_DETECT_INT_ST    (BIT(3))
#define I2C_END_DETECT_INT_ST_M  (I2C_END_DETECT_INT_ST_V << I2C_END_DETECT_INT_ST_S)
#define I2C_END_DETECT_INT_ST_V  0x00000001U
#define I2C_END_DETECT_INT_ST_S  3
/** I2C_BYTE_TRANS_DONE_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status status of the I2C_END_DETECT_INT interrupt.
 */
#define I2C_BYTE_TRANS_DONE_INT_ST    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_ST_M  (I2C_BYTE_TRANS_DONE_INT_ST_V << I2C_BYTE_TRANS_DONE_INT_ST_S)
#define I2C_BYTE_TRANS_DONE_INT_ST_V  0x00000001U
#define I2C_BYTE_TRANS_DONE_INT_ST_S  4
/** I2C_ARBITRATION_LOST_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status status of the I2C_ARBITRATION_LOST_INT interrupt.
 */
#define I2C_ARBITRATION_LOST_INT_ST    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ST_M  (I2C_ARBITRATION_LOST_INT_ST_V << I2C_ARBITRATION_LOST_INT_ST_S)
#define I2C_ARBITRATION_LOST_INT_ST_V  0x00000001U
#define I2C_ARBITRATION_LOST_INT_ST_S  5
/** I2C_MST_TXFIFO_UDF_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status status of I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_MST_TXFIFO_UDF_INT_ST    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_ST_M  (I2C_MST_TXFIFO_UDF_INT_ST_V << I2C_MST_TXFIFO_UDF_INT_ST_S)
#define I2C_MST_TXFIFO_UDF_INT_ST_V  0x00000001U
#define I2C_MST_TXFIFO_UDF_INT_ST_S  6
/** I2C_TRANS_COMPLETE_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status status of the I2C_TRANS_COMPLETE_INT interrupt.
 */
#define I2C_TRANS_COMPLETE_INT_ST    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ST_M  (I2C_TRANS_COMPLETE_INT_ST_V << I2C_TRANS_COMPLETE_INT_ST_S)
#define I2C_TRANS_COMPLETE_INT_ST_V  0x00000001U
#define I2C_TRANS_COMPLETE_INT_ST_S  7
/** I2C_TIME_OUT_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status status of the I2C_TIME_OUT_INT interrupt.
 */
#define I2C_TIME_OUT_INT_ST    (BIT(8))
#define I2C_TIME_OUT_INT_ST_M  (I2C_TIME_OUT_INT_ST_V << I2C_TIME_OUT_INT_ST_S)
#define I2C_TIME_OUT_INT_ST_V  0x00000001U
#define I2C_TIME_OUT_INT_ST_S  8
/** I2C_TRANS_START_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status status of the I2C_TRANS_START_INT interrupt.
 */
#define I2C_TRANS_START_INT_ST    (BIT(9))
#define I2C_TRANS_START_INT_ST_M  (I2C_TRANS_START_INT_ST_V << I2C_TRANS_START_INT_ST_S)
#define I2C_TRANS_START_INT_ST_V  0x00000001U
#define I2C_TRANS_START_INT_ST_S  9
/** I2C_NACK_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status status of I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_NACK_INT_ST    (BIT(10))
#define I2C_NACK_INT_ST_M  (I2C_NACK_INT_ST_V << I2C_NACK_INT_ST_S)
#define I2C_NACK_INT_ST_V  0x00000001U
#define I2C_NACK_INT_ST_S  10
/** I2C_TXFIFO_OVF_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status status of I2C_TXFIFO_OVF_INT interrupt.
 */
#define I2C_TXFIFO_OVF_INT_ST    (BIT(11))
#define I2C_TXFIFO_OVF_INT_ST_M  (I2C_TXFIFO_OVF_INT_ST_V << I2C_TXFIFO_OVF_INT_ST_S)
#define I2C_TXFIFO_OVF_INT_ST_V  0x00000001U
#define I2C_TXFIFO_OVF_INT_ST_S  11
/** I2C_RXFIFO_UDF_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status status of I2C_RXFIFO_UDF_INT  interrupt.
 */
#define I2C_RXFIFO_UDF_INT_ST    (BIT(12))
#define I2C_RXFIFO_UDF_INT_ST_M  (I2C_RXFIFO_UDF_INT_ST_V << I2C_RXFIFO_UDF_INT_ST_S)
#define I2C_RXFIFO_UDF_INT_ST_V  0x00000001U
#define I2C_RXFIFO_UDF_INT_ST_S  12
/** I2C_SCL_ST_TO_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status status of I2C_SCL_ST_TO_INT interrupt.
 */
#define I2C_SCL_ST_TO_INT_ST    (BIT(13))
#define I2C_SCL_ST_TO_INT_ST_M  (I2C_SCL_ST_TO_INT_ST_V << I2C_SCL_ST_TO_INT_ST_S)
#define I2C_SCL_ST_TO_INT_ST_V  0x00000001U
#define I2C_SCL_ST_TO_INT_ST_S  13
/** I2C_SCL_MAIN_ST_TO_INT_ST : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status status of I2C_SCL_MAIN_ST_TO_INT interrupt.
 */
#define I2C_SCL_MAIN_ST_TO_INT_ST    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_ST_M  (I2C_SCL_MAIN_ST_TO_INT_ST_V << I2C_SCL_MAIN_ST_TO_INT_ST_S)
#define I2C_SCL_MAIN_ST_TO_INT_ST_V  0x00000001U
#define I2C_SCL_MAIN_ST_TO_INT_ST_S  14
/** I2C_DET_START_INT_ST : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status status of I2C_DET_START_INT interrupt.
 */
#define I2C_DET_START_INT_ST    (BIT(15))
#define I2C_DET_START_INT_ST_M  (I2C_DET_START_INT_ST_V << I2C_DET_START_INT_ST_S)
#define I2C_DET_START_INT_ST_V  0x00000001U
#define I2C_DET_START_INT_ST_S  15
/** I2C_SLAVE_STRETCH_INT_ST : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status status of I2C_SLAVE_STRETCH_INT interrupt.
 */
#define I2C_SLAVE_STRETCH_INT_ST    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_ST_M  (I2C_SLAVE_STRETCH_INT_ST_V << I2C_SLAVE_STRETCH_INT_ST_S)
#define I2C_SLAVE_STRETCH_INT_ST_V  0x00000001U
#define I2C_SLAVE_STRETCH_INT_ST_S  16
/** I2C_GENERAL_CALL_INT_ST : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status status of I2C_GENARAL_CALL_INT interrupt.
 */
#define I2C_GENERAL_CALL_INT_ST    (BIT(17))
#define I2C_GENERAL_CALL_INT_ST_M  (I2C_GENERAL_CALL_INT_ST_V << I2C_GENERAL_CALL_INT_ST_S)
#define I2C_GENERAL_CALL_INT_ST_V  0x00000001U
#define I2C_GENERAL_CALL_INT_ST_S  17
/** I2C_SLAVE_ADDR_UNMATCH_INT_ST : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status status of I2C_SLAVE_ADDR_UNMATCH_INT interrupt.
 */
#define I2C_SLAVE_ADDR_UNMATCH_INT_ST    (BIT(18))
#define I2C_SLAVE_ADDR_UNMATCH_INT_ST_M  (I2C_SLAVE_ADDR_UNMATCH_INT_ST_V << I2C_SLAVE_ADDR_UNMATCH_INT_ST_S)
#define I2C_SLAVE_ADDR_UNMATCH_INT_ST_V  0x00000001U
#define I2C_SLAVE_ADDR_UNMATCH_INT_ST_S  18

/** I2C_SDA_HOLD_REG register
 *  Configures the hold time after a negative SCL edge
 */
#define I2C_SDA_HOLD_REG (DR_REG_I2C_BASE + 0x30)
/** I2C_SDA_HOLD_TIME : R/W; bitpos: [8:0]; default: 0;
 *  Configures the time to hold the data after the falling edge of SCL.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SDA_HOLD_TIME    0x000001FFU
#define I2C_SDA_HOLD_TIME_M  (I2C_SDA_HOLD_TIME_V << I2C_SDA_HOLD_TIME_S)
#define I2C_SDA_HOLD_TIME_V  0x000001FFU
#define I2C_SDA_HOLD_TIME_S  0

/** I2C_SDA_SAMPLE_REG register
 *  Configures the sample time after a positive SCL edge
 */
#define I2C_SDA_SAMPLE_REG (DR_REG_I2C_BASE + 0x34)
/** I2C_SDA_SAMPLE_TIME : R/W; bitpos: [8:0]; default: 0;
 *  Configures the time for sampling SDA.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SDA_SAMPLE_TIME    0x000001FFU
#define I2C_SDA_SAMPLE_TIME_M  (I2C_SDA_SAMPLE_TIME_V << I2C_SDA_SAMPLE_TIME_S)
#define I2C_SDA_SAMPLE_TIME_V  0x000001FFU
#define I2C_SDA_SAMPLE_TIME_S  0

/** I2C_SCL_HIGH_PERIOD_REG register
 *  Configures the high level width of SCL
 */
#define I2C_SCL_HIGH_PERIOD_REG (DR_REG_I2C_BASE + 0x38)
/** I2C_SCL_HIGH_PERIOD : R/W; bitpos: [8:0]; default: 0;
 *  Configures for how long SCL remains high in master mode.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_HIGH_PERIOD    0x000001FFU
#define I2C_SCL_HIGH_PERIOD_M  (I2C_SCL_HIGH_PERIOD_V << I2C_SCL_HIGH_PERIOD_S)
#define I2C_SCL_HIGH_PERIOD_V  0x000001FFU
#define I2C_SCL_HIGH_PERIOD_S  0
/** I2C_SCL_WAIT_HIGH_PERIOD : R/W; bitpos: [15:9]; default: 0;
 *  Configures the SCL_FSM's waiting period for SCL high level in master mode.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_WAIT_HIGH_PERIOD    0x0000007FU
#define I2C_SCL_WAIT_HIGH_PERIOD_M  (I2C_SCL_WAIT_HIGH_PERIOD_V << I2C_SCL_WAIT_HIGH_PERIOD_S)
#define I2C_SCL_WAIT_HIGH_PERIOD_V  0x0000007FU
#define I2C_SCL_WAIT_HIGH_PERIOD_S  9

/** I2C_SCL_START_HOLD_REG register
 *  Configures the delay between the SDA and SCL negative edge for a start condition
 */
#define I2C_SCL_START_HOLD_REG (DR_REG_I2C_BASE + 0x40)
/** I2C_SCL_START_HOLD_TIME : R/W; bitpos: [8:0]; default: 8;
 *  Configures the time between the falling edge of SDA and the falling edge of SCL for
 *  a START condition.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_START_HOLD_TIME    0x000001FFU
#define I2C_SCL_START_HOLD_TIME_M  (I2C_SCL_START_HOLD_TIME_V << I2C_SCL_START_HOLD_TIME_S)
#define I2C_SCL_START_HOLD_TIME_V  0x000001FFU
#define I2C_SCL_START_HOLD_TIME_S  0

/** I2C_SCL_RSTART_SETUP_REG register
 *  Configures the delay between the positive
 *  edge of SCL and the negative edge of SDA
 */
#define I2C_SCL_RSTART_SETUP_REG (DR_REG_I2C_BASE + 0x44)
/** I2C_SCL_RSTART_SETUP_TIME : R/W; bitpos: [8:0]; default: 8;
 *  Configures the time between the positive edge of SCL and the negative edge of SDA
 *  for a RESTART condition.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_RSTART_SETUP_TIME    0x000001FFU
#define I2C_SCL_RSTART_SETUP_TIME_M  (I2C_SCL_RSTART_SETUP_TIME_V << I2C_SCL_RSTART_SETUP_TIME_S)
#define I2C_SCL_RSTART_SETUP_TIME_V  0x000001FFU
#define I2C_SCL_RSTART_SETUP_TIME_S  0

/** I2C_SCL_STOP_HOLD_REG register
 *  Configures the delay after the SCL clock
 *  edge for a stop condition
 */
#define I2C_SCL_STOP_HOLD_REG (DR_REG_I2C_BASE + 0x48)
/** I2C_SCL_STOP_HOLD_TIME : R/W; bitpos: [8:0]; default: 8;
 *  Configures the delay after the STOP condition.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_STOP_HOLD_TIME    0x000001FFU
#define I2C_SCL_STOP_HOLD_TIME_M  (I2C_SCL_STOP_HOLD_TIME_V << I2C_SCL_STOP_HOLD_TIME_S)
#define I2C_SCL_STOP_HOLD_TIME_V  0x000001FFU
#define I2C_SCL_STOP_HOLD_TIME_S  0

/** I2C_SCL_STOP_SETUP_REG register
 *  Configures the delay between the SDA and
 *  SCL positive edge for a stop condition
 */
#define I2C_SCL_STOP_SETUP_REG (DR_REG_I2C_BASE + 0x4c)
/** I2C_SCL_STOP_SETUP_TIME : R/W; bitpos: [8:0]; default: 8;
 *  This register is used to configure the time between the positive edgeof SCL and the
 *  positive edge of SDA, in I2C module clock cycles.
 */
#define I2C_SCL_STOP_SETUP_TIME    0x000001FFU
#define I2C_SCL_STOP_SETUP_TIME_M  (I2C_SCL_STOP_SETUP_TIME_V << I2C_SCL_STOP_SETUP_TIME_S)
#define I2C_SCL_STOP_SETUP_TIME_V  0x000001FFU
#define I2C_SCL_STOP_SETUP_TIME_S  0

/** I2C_FILTER_CFG_REG register
 *  SCL and SDA filter configuration register
 */
#define I2C_FILTER_CFG_REG (DR_REG_I2C_BASE + 0x50)
/** I2C_SCL_FILTER_THRES : R/W; bitpos: [3:0]; default: 0;
 *  Configures the threshold pulse width to be filtered on SCL. When a pulse on the SCL
 *  input has smaller width than this register value, the I2C controller will ignore
 *  that pulse.
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_FILTER_THRES    0x0000000FU
#define I2C_SCL_FILTER_THRES_M  (I2C_SCL_FILTER_THRES_V << I2C_SCL_FILTER_THRES_S)
#define I2C_SCL_FILTER_THRES_V  0x0000000FU
#define I2C_SCL_FILTER_THRES_S  0
/** I2C_SDA_FILTER_THRES : R/W; bitpos: [7:4]; default: 0;
 *  Configures the threshold pulse width to be filtered on SDA. When a pulse on the SDA
 *  input has smaller width than this register value, the I2C controller will ignore
 *  that pulse.
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SDA_FILTER_THRES    0x0000000FU
#define I2C_SDA_FILTER_THRES_M  (I2C_SDA_FILTER_THRES_V << I2C_SDA_FILTER_THRES_S)
#define I2C_SDA_FILTER_THRES_V  0x0000000FU
#define I2C_SDA_FILTER_THRES_S  4
/** I2C_SCL_FILTER_EN : R/W; bitpos: [8]; default: 1;
 *  Configures to enable the filter function for SCL. \\
 *  0: No effect \\
 *  1: Enable \\
 */
#define I2C_SCL_FILTER_EN    (BIT(8))
#define I2C_SCL_FILTER_EN_M  (I2C_SCL_FILTER_EN_V << I2C_SCL_FILTER_EN_S)
#define I2C_SCL_FILTER_EN_V  0x00000001U
#define I2C_SCL_FILTER_EN_S  8
/** I2C_SDA_FILTER_EN : R/W; bitpos: [9]; default: 1;
 *  Configures to enable the filter function for SDA. \\
 *  0: No effect \\
 *  1: Enable \\
 */
#define I2C_SDA_FILTER_EN    (BIT(9))
#define I2C_SDA_FILTER_EN_M  (I2C_SDA_FILTER_EN_V << I2C_SDA_FILTER_EN_S)
#define I2C_SDA_FILTER_EN_V  0x00000001U
#define I2C_SDA_FILTER_EN_S  9

/** I2C_CLK_CONF_REG register
 *  I2C CLK configuration register
 */
#define I2C_CLK_CONF_REG (DR_REG_I2C_BASE + 0x54)
/** I2C_SCLK_DIV_NUM : R/W; bitpos: [7:0]; default: 0;
 *  the integral part of the fractional divisor for i2c module
 */
#define I2C_SCLK_DIV_NUM    0x000000FFU
#define I2C_SCLK_DIV_NUM_M  (I2C_SCLK_DIV_NUM_V << I2C_SCLK_DIV_NUM_S)
#define I2C_SCLK_DIV_NUM_V  0x000000FFU
#define I2C_SCLK_DIV_NUM_S  0
/** I2C_SCLK_DIV_A : R/W; bitpos: [13:8]; default: 0;
 *  the numerator of the fractional part of the fractional divisor for i2c module
 */
#define I2C_SCLK_DIV_A    0x0000003FU
#define I2C_SCLK_DIV_A_M  (I2C_SCLK_DIV_A_V << I2C_SCLK_DIV_A_S)
#define I2C_SCLK_DIV_A_V  0x0000003FU
#define I2C_SCLK_DIV_A_S  8
/** I2C_SCLK_DIV_B : R/W; bitpos: [19:14]; default: 0;
 *  the denominator of the fractional part of the fractional divisor for i2c module
 */
#define I2C_SCLK_DIV_B    0x0000003FU
#define I2C_SCLK_DIV_B_M  (I2C_SCLK_DIV_B_V << I2C_SCLK_DIV_B_S)
#define I2C_SCLK_DIV_B_V  0x0000003FU
#define I2C_SCLK_DIV_B_S  14
/** I2C_SCLK_SEL : R/W; bitpos: [20]; default: 0;
 *  The clock selection for i2c module:0-XTAL,1-CLK_8MHz.
 */
#define I2C_SCLK_SEL    (BIT(20))
#define I2C_SCLK_SEL_M  (I2C_SCLK_SEL_V << I2C_SCLK_SEL_S)
#define I2C_SCLK_SEL_V  0x00000001U
#define I2C_SCLK_SEL_S  20
/** I2C_SCLK_ACTIVE : R/W; bitpos: [21]; default: 1;
 *  The clock switch for i2c module
 */
#define I2C_SCLK_ACTIVE    (BIT(21))
#define I2C_SCLK_ACTIVE_M  (I2C_SCLK_ACTIVE_V << I2C_SCLK_ACTIVE_S)
#define I2C_SCLK_ACTIVE_V  0x00000001U
#define I2C_SCLK_ACTIVE_S  21

/** I2C_COMD0_REG register
 *  I2C command register 0
 */
#define I2C_COMD0_REG (DR_REG_I2C_BASE + 0x58)
/** I2C_COMMAND0 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 0. \\
 *  It consists of three parts:\\
 *  op_code is the command\\
 *  1: WRITE\\
 *  2: STOP\\
 *  3: READ\\
 *  4: END\\
 *  6: RSTART\\
 *  Byte_num represents the number of bytes that need to be sent or received.\\
 *  ack_check_en, ack_exp and ack are used to control the ACK bit. See I2C cmd
 *  structure <a href=fig:i2c-cmd-structure">link</a> for more information.
 *  \\\tododone{for CJ, please add a hyperlink for I2C CMD structure.CJ: done.}"
 */
#define I2C_COMMAND0    0x00003FFFU
#define I2C_COMMAND0_M  (I2C_COMMAND0_V << I2C_COMMAND0_S)
#define I2C_COMMAND0_V  0x00003FFFU
#define I2C_COMMAND0_S  0
/** I2C_COMMAND0_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 0 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND0_DONE    (BIT(31))
#define I2C_COMMAND0_DONE_M  (I2C_COMMAND0_DONE_V << I2C_COMMAND0_DONE_S)
#define I2C_COMMAND0_DONE_V  0x00000001U
#define I2C_COMMAND0_DONE_S  31

/** I2C_COMD1_REG register
 *  I2C command register 1
 */
#define I2C_COMD1_REG (DR_REG_I2C_BASE + 0x5c)
/** I2C_COMMAND1 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 1.\\
 *  See details in I2C_CMD0_REG[13:0].
 */
#define I2C_COMMAND1    0x00003FFFU
#define I2C_COMMAND1_M  (I2C_COMMAND1_V << I2C_COMMAND1_S)
#define I2C_COMMAND1_V  0x00003FFFU
#define I2C_COMMAND1_S  0
/** I2C_COMMAND1_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 1 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND1_DONE    (BIT(31))
#define I2C_COMMAND1_DONE_M  (I2C_COMMAND1_DONE_V << I2C_COMMAND1_DONE_S)
#define I2C_COMMAND1_DONE_V  0x00000001U
#define I2C_COMMAND1_DONE_S  31

/** I2C_COMD2_REG register
 *  I2C command register 2
 */
#define I2C_COMD2_REG (DR_REG_I2C_BASE + 0x60)
/** I2C_COMMAND2 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 2. See details in I2C_CMD0_REG[13:0].
 */
#define I2C_COMMAND2    0x00003FFFU
#define I2C_COMMAND2_M  (I2C_COMMAND2_V << I2C_COMMAND2_S)
#define I2C_COMMAND2_V  0x00003FFFU
#define I2C_COMMAND2_S  0
/** I2C_COMMAND2_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 2 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND2_DONE    (BIT(31))
#define I2C_COMMAND2_DONE_M  (I2C_COMMAND2_DONE_V << I2C_COMMAND2_DONE_S)
#define I2C_COMMAND2_DONE_V  0x00000001U
#define I2C_COMMAND2_DONE_S  31

/** I2C_COMD3_REG register
 *  I2C command register 3
 */
#define I2C_COMD3_REG (DR_REG_I2C_BASE + 0x64)
/** I2C_COMMAND3 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 3. See details in I2C_CMD0_REG[13:0].
 */
#define I2C_COMMAND3    0x00003FFFU
#define I2C_COMMAND3_M  (I2C_COMMAND3_V << I2C_COMMAND3_S)
#define I2C_COMMAND3_V  0x00003FFFU
#define I2C_COMMAND3_S  0
/** I2C_COMMAND3_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 3 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND3_DONE    (BIT(31))
#define I2C_COMMAND3_DONE_M  (I2C_COMMAND3_DONE_V << I2C_COMMAND3_DONE_S)
#define I2C_COMMAND3_DONE_V  0x00000001U
#define I2C_COMMAND3_DONE_S  31

/** I2C_COMD4_REG register
 *  I2C command register 4
 */
#define I2C_COMD4_REG (DR_REG_I2C_BASE + 0x68)
/** I2C_COMMAND4 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 4. See details in I2C_CMD0_REG[13:0].
 */
#define I2C_COMMAND4    0x00003FFFU
#define I2C_COMMAND4_M  (I2C_COMMAND4_V << I2C_COMMAND4_S)
#define I2C_COMMAND4_V  0x00003FFFU
#define I2C_COMMAND4_S  0
/** I2C_COMMAND4_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 4 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND4_DONE    (BIT(31))
#define I2C_COMMAND4_DONE_M  (I2C_COMMAND4_DONE_V << I2C_COMMAND4_DONE_S)
#define I2C_COMMAND4_DONE_V  0x00000001U
#define I2C_COMMAND4_DONE_S  31

/** I2C_COMD5_REG register
 *  I2C command register 5
 */
#define I2C_COMD5_REG (DR_REG_I2C_BASE + 0x6c)
/** I2C_COMMAND5 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 5. See details in I2C_CMD0_REG[13:0].
 */
#define I2C_COMMAND5    0x00003FFFU
#define I2C_COMMAND5_M  (I2C_COMMAND5_V << I2C_COMMAND5_S)
#define I2C_COMMAND5_V  0x00003FFFU
#define I2C_COMMAND5_S  0
/** I2C_COMMAND5_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 5 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND5_DONE    (BIT(31))
#define I2C_COMMAND5_DONE_M  (I2C_COMMAND5_DONE_V << I2C_COMMAND5_DONE_S)
#define I2C_COMMAND5_DONE_V  0x00000001U
#define I2C_COMMAND5_DONE_S  31

/** I2C_COMD6_REG register
 *  I2C command register 6
 */
#define I2C_COMD6_REG (DR_REG_I2C_BASE + 0x70)
/** I2C_COMMAND6 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 6. See details in I2C_CMD0_REG[13:0].
 */
#define I2C_COMMAND6    0x00003FFFU
#define I2C_COMMAND6_M  (I2C_COMMAND6_V << I2C_COMMAND6_S)
#define I2C_COMMAND6_V  0x00003FFFU
#define I2C_COMMAND6_S  0
/** I2C_COMMAND6_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 6 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND6_DONE    (BIT(31))
#define I2C_COMMAND6_DONE_M  (I2C_COMMAND6_DONE_V << I2C_COMMAND6_DONE_S)
#define I2C_COMMAND6_DONE_V  0x00000001U
#define I2C_COMMAND6_DONE_S  31

/** I2C_COMD7_REG register
 *  I2C command register 7
 */
#define I2C_COMD7_REG (DR_REG_I2C_BASE + 0x74)
/** I2C_COMMAND7 : R/W; bitpos: [13:0]; default: 0;
 *  Configures command 7. See details in I2C_CMD0_REG[13:0].
 */
#define I2C_COMMAND7    0x00003FFFU
#define I2C_COMMAND7_M  (I2C_COMMAND7_V << I2C_COMMAND7_S)
#define I2C_COMMAND7_V  0x00003FFFU
#define I2C_COMMAND7_S  0
/** I2C_COMMAND7_DONE : R/W/SS; bitpos: [31]; default: 0;
 *  Represents whether command 7 is done in I2C Master mode.\\
 *  0: Not done \\
 *  1: Done \\
 */
#define I2C_COMMAND7_DONE    (BIT(31))
#define I2C_COMMAND7_DONE_M  (I2C_COMMAND7_DONE_V << I2C_COMMAND7_DONE_S)
#define I2C_COMMAND7_DONE_V  0x00000001U
#define I2C_COMMAND7_DONE_S  31

/** I2C_SCL_ST_TIME_OUT_REG register
 *  SCL status time out register
 */
#define I2C_SCL_ST_TIME_OUT_REG (DR_REG_I2C_BASE + 0x78)
/** I2C_SCL_ST_TO_I2C : R/W; bitpos: [4:0]; default: 16;
 *  Configures the threshold value of SCL_FSM state unchanged period. It should be no
 *  more than 23.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_ST_TO_I2C    0x0000001FU
#define I2C_SCL_ST_TO_I2C_M  (I2C_SCL_ST_TO_I2C_V << I2C_SCL_ST_TO_I2C_S)
#define I2C_SCL_ST_TO_I2C_V  0x0000001FU
#define I2C_SCL_ST_TO_I2C_S  0

/** I2C_SCL_MAIN_ST_TIME_OUT_REG register
 *  SCL main status time out register
 */
#define I2C_SCL_MAIN_ST_TIME_OUT_REG (DR_REG_I2C_BASE + 0x7c)
/** I2C_SCL_MAIN_ST_TO_I2C : R/W; bitpos: [4:0]; default: 16;
 *  Configures the threshold value of SCL_MAIN_FSM state unchanged period. It should be
 *  no more than 23.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_MAIN_ST_TO_I2C    0x0000001FU
#define I2C_SCL_MAIN_ST_TO_I2C_M  (I2C_SCL_MAIN_ST_TO_I2C_V << I2C_SCL_MAIN_ST_TO_I2C_S)
#define I2C_SCL_MAIN_ST_TO_I2C_V  0x0000001FU
#define I2C_SCL_MAIN_ST_TO_I2C_S  0

/** I2C_SCL_SP_CONF_REG register
 *  Power configuration register
 */
#define I2C_SCL_SP_CONF_REG (DR_REG_I2C_BASE + 0x80)
/** I2C_SCL_RST_SLV_EN : R/W/SC; bitpos: [0]; default: 0;
 *  Configures to send out SCL pulses when I2C master is IDLE. The number of pulses
 *  equals to I2C_SCL_RST_SLV_NUM[4:0].
 */
#define I2C_SCL_RST_SLV_EN    (BIT(0))
#define I2C_SCL_RST_SLV_EN_M  (I2C_SCL_RST_SLV_EN_V << I2C_SCL_RST_SLV_EN_S)
#define I2C_SCL_RST_SLV_EN_V  0x00000001U
#define I2C_SCL_RST_SLV_EN_S  0
/** I2C_SCL_RST_SLV_NUM : R/W; bitpos: [5:1]; default: 0;
 *  Configure the pulses of SCL generated in I2C master mode. \\
 *  Valid when I2C_SCL_RST_SLV_EN is 1.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_SCL_RST_SLV_NUM    0x0000001FU
#define I2C_SCL_RST_SLV_NUM_M  (I2C_SCL_RST_SLV_NUM_V << I2C_SCL_RST_SLV_NUM_S)
#define I2C_SCL_RST_SLV_NUM_V  0x0000001FU
#define I2C_SCL_RST_SLV_NUM_S  1
/** I2C_SCL_PD_EN : R/W; bitpos: [6]; default: 0;
 *  Configures to power down the I2C output SCL line. \\
 *  0: Not power down.\\
 *  1: Not work and power down.\\
 *  Valid only when I2C_SCL_FORCE_OUT is 1.
 */
#define I2C_SCL_PD_EN    (BIT(6))
#define I2C_SCL_PD_EN_M  (I2C_SCL_PD_EN_V << I2C_SCL_PD_EN_S)
#define I2C_SCL_PD_EN_V  0x00000001U
#define I2C_SCL_PD_EN_S  6
/** I2C_SDA_PD_EN : R/W; bitpos: [7]; default: 0;
 *  Configures to power down the I2C output SDA line. \\
 *  0: Not power down.\\
 *  1: Not work and power down.\\
 *  Valid only when I2C_SDA_FORCE_OUT is 1.
 */
#define I2C_SDA_PD_EN    (BIT(7))
#define I2C_SDA_PD_EN_M  (I2C_SDA_PD_EN_V << I2C_SDA_PD_EN_S)
#define I2C_SDA_PD_EN_V  0x00000001U
#define I2C_SDA_PD_EN_S  7

/** I2C_SCL_STRETCH_CONF_REG register
 *  Set SCL stretch of I2C slave
 */
#define I2C_SCL_STRETCH_CONF_REG (DR_REG_I2C_BASE + 0x84)
/** I2C_STRETCH_PROTECT_NUM : R/W; bitpos: [9:0]; default: 0;
 *  Configures the time period to release the SCL line from stretching to avoid timing
 *  violation. Usually it should be larger than the SDA setup time.\\
 *  Measurement unit: i2c_sclk \\
 */
#define I2C_STRETCH_PROTECT_NUM    0x000003FFU
#define I2C_STRETCH_PROTECT_NUM_M  (I2C_STRETCH_PROTECT_NUM_V << I2C_STRETCH_PROTECT_NUM_S)
#define I2C_STRETCH_PROTECT_NUM_V  0x000003FFU
#define I2C_STRETCH_PROTECT_NUM_S  0
/** I2C_SLAVE_SCL_STRETCH_EN : R/W; bitpos: [10]; default: 0;
 *  Configures to enable slave SCL stretch function. The SCL output line will be
 *  stretched low when I2C_SLAVE_SCL_STRETCH_EN is 1 and stretch event happens. The
 *  stretch cause can be seen in I2C_STRETCH_CAUSE.\\
 *  0: Disable \\
 *  1: Enable \\
 */
#define I2C_SLAVE_SCL_STRETCH_EN    (BIT(10))
#define I2C_SLAVE_SCL_STRETCH_EN_M  (I2C_SLAVE_SCL_STRETCH_EN_V << I2C_SLAVE_SCL_STRETCH_EN_S)
#define I2C_SLAVE_SCL_STRETCH_EN_V  0x00000001U
#define I2C_SLAVE_SCL_STRETCH_EN_S  10
/** I2C_SLAVE_SCL_STRETCH_CLR : WT; bitpos: [11]; default: 0;
 *  Configures to clear the I2C slave SCL stretch function.\\
 *  0: No effect \\
 *  1: Clear\\
 */
#define I2C_SLAVE_SCL_STRETCH_CLR    (BIT(11))
#define I2C_SLAVE_SCL_STRETCH_CLR_M  (I2C_SLAVE_SCL_STRETCH_CLR_V << I2C_SLAVE_SCL_STRETCH_CLR_S)
#define I2C_SLAVE_SCL_STRETCH_CLR_V  0x00000001U
#define I2C_SLAVE_SCL_STRETCH_CLR_S  11
/** I2C_SLAVE_BYTE_ACK_CTL_EN : R/W; bitpos: [12]; default: 0;
 *  Configures to enable the function for slave to control ACK level.\\
 *  0: Disable \\
 *  1: Enable \\
 */
#define I2C_SLAVE_BYTE_ACK_CTL_EN    (BIT(12))
#define I2C_SLAVE_BYTE_ACK_CTL_EN_M  (I2C_SLAVE_BYTE_ACK_CTL_EN_V << I2C_SLAVE_BYTE_ACK_CTL_EN_S)
#define I2C_SLAVE_BYTE_ACK_CTL_EN_V  0x00000001U
#define I2C_SLAVE_BYTE_ACK_CTL_EN_S  12
/** I2C_SLAVE_BYTE_ACK_LVL : R/W; bitpos: [13]; default: 0;
 *  Set the ACK level when slave controlling ACK level function enables.\\
 *  0: Low level\\
 *  1: High level \\
 */
#define I2C_SLAVE_BYTE_ACK_LVL    (BIT(13))
#define I2C_SLAVE_BYTE_ACK_LVL_M  (I2C_SLAVE_BYTE_ACK_LVL_V << I2C_SLAVE_BYTE_ACK_LVL_S)
#define I2C_SLAVE_BYTE_ACK_LVL_V  0x00000001U
#define I2C_SLAVE_BYTE_ACK_LVL_S  13

/** I2C_DATE_REG register
 *  Version register
 */
#define I2C_DATE_REG (DR_REG_I2C_BASE + 0xf8)
/** I2C_DATE : R/W; bitpos: [31:0]; default: 37752896;
 *  Version control register.
 */
#define I2C_DATE    0xFFFFFFFFU
#define I2C_DATE_M  (I2C_DATE_V << I2C_DATE_S)
#define I2C_DATE_V  0xFFFFFFFFU
#define I2C_DATE_S  0

/** I2C_TXFIFO_START_ADDR_REG register
 *  I2C TXFIFO base address register
 */
#define I2C_TXFIFO_START_ADDR_REG (DR_REG_I2C_BASE + 0x100)
/** I2C_TXFIFO_START_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Represents the I2C txfifo first address.
 */
#define I2C_TXFIFO_START_ADDR    0xFFFFFFFFU
#define I2C_TXFIFO_START_ADDR_M  (I2C_TXFIFO_START_ADDR_V << I2C_TXFIFO_START_ADDR_S)
#define I2C_TXFIFO_START_ADDR_V  0xFFFFFFFFU
#define I2C_TXFIFO_START_ADDR_S  0

/** I2C_RXFIFO_START_ADDR_REG register
 *  I2C RXFIFO base address register
 */
#define I2C_RXFIFO_START_ADDR_REG (DR_REG_I2C_BASE + 0x180)
/** I2C_RXFIFO_START_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Represents the I2C rxfifo first address.
 */
#define I2C_RXFIFO_START_ADDR    0xFFFFFFFFU
#define I2C_RXFIFO_START_ADDR_M  (I2C_RXFIFO_START_ADDR_V << I2C_RXFIFO_START_ADDR_S)
#define I2C_RXFIFO_START_ADDR_V  0xFFFFFFFFU
#define I2C_RXFIFO_START_ADDR_S  0

#ifdef __cplusplus
}
#endif
