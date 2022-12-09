// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "soc.h"

/**
 * This file lists peripheral registers of an I2C controller which is part of the RTC.
 * ULP coprocessor uses this controller to implement I2C_RD and I2C_WR instructions.
 *
 * Part of the functionality of this controller (such as slave mode, and multi-byte
 * transfers) is not wired to the ULP, and is such, is not available to the
 * ULP programs.
 */

#define RTC_I2C_SCL_LOW_PERIOD_REG          (DR_REG_RTC_I2C_BASE + 0x000)
/* RTC_I2C_SCL_LOW_PERIOD : R/W ;bitpos:[18:0] ;default: 19'b0 ; */
/*description: number of cycles that scl == 0 */
#define RTC_I2C_SCL_LOW_PERIOD    0x1FFFFFF
#define RTC_I2C_SCL_LOW_PERIOD_M  ((RTC_I2C_SCL_LOW_PERIOD_V)<<(RTC_I2C_SCL_LOW_PERIOD_S))
#define RTC_I2C_SCL_LOW_PERIOD_V  0x1FFFFFF
#define RTC_I2C_SCL_LOW_PERIOD_S  0

#define RTC_I2C_CTRL_REG          (DR_REG_RTC_I2C_BASE + 0x004)
/* RTC_I2C_RX_LSB_FIRST : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Receive LSB first */
#define RTC_I2C_RX_LSB_FIRST	BIT(7)
#define RTC_I2C_RX_LSB_FIRST_M  BIT(7)
#define RTC_I2C_RX_LSB_FIRST_V  (1)
#define RTC_I2C_RX_LSB_FIRST_S  (7)
/* RTC_I2C_TX_LSB_FIRST : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Send LSB first */
#define RTC_I2C_TX_LSB_FIRST	BIT(6)
#define RTC_I2C_TX_LSB_FIRST_M  BIT(6)
#define RTC_I2C_TX_LSB_FIRST_V  (1)
#define RTC_I2C_TX_LSB_FIRST_S  (6)
/* RTC_I2C_TRANS_START : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Force to generate start condition */
#define RTC_I2C_TRANS_START	   BIT(5)
#define RTC_I2C_TRANS_START_M  BIT(5)
#define RTC_I2C_TRANS_START_V  (1)
#define RTC_I2C_TRANS_START_S  (5)
/* RTC_I2C_MS_MODE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Master (1) or slave (0) */
#define RTC_I2C_MS_MODE	   BIT(4)
#define RTC_I2C_MS_MODE_M  BIT(4)
#define RTC_I2C_MS_MODE_V  (1)
#define RTC_I2C_MS_MODE_S  (4)
/* RTC_I2C_SCL_FORCE_OUT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: SCL is push-pull (1) or open-drain (0) */
#define RTC_I2C_SCL_FORCE_OUT	 BIT(1)
#define RTC_I2C_SCL_FORCE_OUT_M  BIT(1)
#define RTC_I2C_SCL_FORCE_OUT_V  (1)
#define RTC_I2C_SCL_FORCE_OUT_S  (1)
/* RTC_I2C_SDA_FORCE_OUT : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: SDA is push-pull (1) or open-drain (0) */
#define RTC_I2C_SDA_FORCE_OUT	 BIT(0)
#define RTC_I2C_SDA_FORCE_OUT_M  BIT(0)
#define RTC_I2C_SDA_FORCE_OUT_V  (1)
#define RTC_I2C_SDA_FORCE_OUT_S  (0)

#define RTC_I2C_DEBUG_STATUS_REG          (DR_REG_RTC_I2C_BASE + 0x008)
/* RTC_I2C_SCL_STATE : R/W ;bitpos:[30:28] ;default: 3'b0 ; */
/*description: state of SCL state machine */
#define RTC_I2C_SCL_STATE	 0x7
#define RTC_I2C_SCL_STATE_M  ((RTC_I2C_SCL_STATE_V)<<(RTC_I2C_SCL_STATE_S))
#define RTC_I2C_SCL_STATE_V  0x7
#define RTC_I2C_SCL_STATE_S  28
/* RTC_I2C_MAIN_STATE : R/W ;bitpos:[27:25] ;default: 3'b0 ; */
/*description: state of the main state machine */
#define RTC_I2C_MAIN_STATE    0x7
#define RTC_I2C_MAIN_STATE_M  ((RTC_I2C_MAIN_STATE_V)<<(RTC_I2C_MAIN_STATE_S))
#define RTC_I2C_MAIN_STATE_V  0x7
#define RTC_I2C_MAIN_STATE_S  25
/* RTC_I2C_BYTE_TRANS : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: 8 bit transmit done */
#define RTC_I2C_BYTE_TRANS	   BIT(6)
#define RTC_I2C_BYTE_TRANS_M  BIT(6)
#define RTC_I2C_BYTE_TRANS_V  (1)
#define RTC_I2C_BYTE_TRANS_S  (6)
/* RTC_I2C_SLAVE_ADDR_MATCH : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: When working as a slave, whether address was matched */
#define RTC_I2C_SLAVE_ADDR_MATCH	   BIT(5)
#define RTC_I2C_SLAVE_ADDR_MATCH_M  BIT(5)
#define RTC_I2C_SLAVE_ADDR_MATCH_V  (1)
#define RTC_I2C_SLAVE_ADDR_MATCH_S  (5)
/* RTC_I2C_BUS_BUSY : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: operation is in progress */
#define RTC_I2C_BUS_BUSY	   BIT(4)
#define RTC_I2C_BUS_BUSY_M  BIT(4)
#define RTC_I2C_BUS_BUSY_V  (1)
#define RTC_I2C_BUS_BUSY_S  (4)
/* RTC_I2C_ARB_LOST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: When working as a master, lost control of I2C bus */
#define RTC_I2C_ARB_LOST	   BIT(3)
#define RTC_I2C_ARB_LOST_M  BIT(3)
#define RTC_I2C_ARB_LOST_V  (1)
#define RTC_I2C_ARB_LOST_S  (3)
/* RTC_I2C_TIMED_OUT : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Transfer has timed out */
#define RTC_I2C_TIMED_OUT	   BIT(2)
#define RTC_I2C_TIMED_OUT_M  BIT(2)
#define RTC_I2C_TIMED_OUT_V  (1)
#define RTC_I2C_TIMED_OUT_S  (2)
/* RTC_I2C_SLAVE_RW : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: When working as a slave, the value of R/W bit received */
#define RTC_I2C_SLAVE_RW	   BIT(1)
#define RTC_I2C_SLAVE_RW_M  BIT(1)
#define RTC_I2C_SLAVE_RW_V  (1)
#define RTC_I2C_SLAVE_RW_S  (1)
/* RTC_I2C_ACK_VAL : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The value of an acknowledge signal on the bus */
#define RTC_I2C_ACK_VAL	   BIT(0)
#define RTC_I2C_ACK_VAL_M  BIT(0)
#define RTC_I2C_ACK_VAL_V  (1)
#define RTC_I2C_ACK_VAL_S  (0)

#define RTC_I2C_TIMEOUT_REG          (DR_REG_RTC_I2C_BASE + 0x00c)
/* RTC_I2C_TIMEOUT : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: Maximum number of FAST_CLK cycles that the transmission can take */
#define RTC_I2C_TIMEOUT	   0xFFFFF
#define RTC_I2C_TIMEOUT_M  ((RTC_I2C_TIMEOUT_V)<<(RTC_I2C_TIMEOUT_S))
#define RTC_I2C_TIMEOUT_V  0xFFFFF
#define RTC_I2C_TIMEOUT_S  0

#define RTC_I2C_SLAVE_ADDR_REG          (DR_REG_RTC_I2C_BASE + 0x010)
/* RTC_I2C_SLAVE_ADDR_10BIT : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Set if local slave address is 10-bit */
#define RTC_I2C_SLAVE_ADDR_10BIT	BIT(31)
#define RTC_I2C_SLAVE_ADDR_10BIT_M  BIT(31)
#define RTC_I2C_SLAVE_ADDR_10BIT_V  (1)
#define RTC_I2C_SLAVE_ADDR_10BIT_S  (31)
/* RTC_I2C_SLAVE_ADDR : R/W ;bitpos:[14:0] ;default: 15'b0 ; */
/*description: local slave address */
#define RTC_I2C_SLAVE_ADDR	  0x7FFF
#define RTC_I2C_SLAVE_ADDR_M  ((RTC_I2C_SLAVE_ADDR_V)<<(RTC_I2C_SLAVE_ADDR_S))
#define RTC_I2C_SLAVE_ADDR_V  0x7FFF
#define RTC_I2C_SLAVE_ADDR_S  0

/* Result of last read operation. Not used directly as the data will be
 * returned to the ULP. Listed for debugging purposes.
 */
#define RTC_I2C_DATA_REG         (DR_REG_RTC_I2C_BASE + 0x01c)

/* Interrupt registers; since the interrupt from RTC_I2C is not connected,
 * these registers are only listed for debugging purposes.
 */

/* Interrupt raw status register */
#define RTC_I2C_INT_RAW_REG         (DR_REG_RTC_I2C_BASE + 0x020)
/* RTC_I2C_TIME_OUT_INT_RAW : R/O ;bitpos:[7] ;default: 1'b0 ; */
/*description: time out interrupt raw status */
#define RTC_I2C_TIME_OUT_INT_RAW	   BIT(7)
#define RTC_I2C_TIME_OUT_INT_RAW_M  BIT(7)
#define RTC_I2C_TIME_OUT_INT_RAW_V  (1)
#define RTC_I2C_TIME_OUT_INT_RAW_S  (7)
/* RTC_I2C_TRANS_COMPLETE_INT_RAW : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Stop condition has been detected interrupt raw status */
#define RTC_I2C_TRANS_COMPLETE_INT_RAW	   BIT(6)
#define RTC_I2C_TRANS_COMPLETE_INT_RAW_M  BIT(6)
#define RTC_I2C_TRANS_COMPLETE_INT_RAW_V  (1)
#define RTC_I2C_TRANS_COMPLETE_INT_RAW_S  (6)
/* RTC_I2C_MASTER_TRANS_COMPLETE_INT_RAW : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description:  */
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_RAW	   BIT(5)
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_RAW_M  BIT(5)
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_RAW_V  (1)
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_RAW_S  (5)
/* RTC_I2C_ARBITRATION_LOST_INT_RAW : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Master lost arbitration */
#define RTC_I2C_ARBITRATION_LOST_INT_RAW	   BIT(4)
#define RTC_I2C_ARBITRATION_LOST_INT_RAW_M  BIT(4)
#define RTC_I2C_ARBITRATION_LOST_INT_RAW_V  (1)
#define RTC_I2C_ARBITRATION_LOST_INT_RAW_S  (4)
/* RTC_I2C_SLAVE_TRANS_COMPLETE_INT_RAW : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Slave accepted 1 byte and address matched */
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_RAW	   BIT(3)
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_RAW_M  BIT(3)
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_RAW_V  (1)
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_RAW_S  (3)

/* Interrupt clear register */
#define RTC_I2C_INT_CLR_REG         (DR_REG_RTC_I2C_BASE + 0x024)
/* RTC_I2C_TIME_OUT_INT_CLR : W/O ;bitpos:[8] ;default: 1'b0 ; */
/*description:  */
#define RTC_I2C_TIME_OUT_INT_CLR	   BIT(8)
#define RTC_I2C_TIME_OUT_INT_CLR_M  BIT(8)
#define RTC_I2C_TIME_OUT_INT_CLR_V  (1)
#define RTC_I2C_TIME_OUT_INT_CLR_S  (8)
/* RTC_I2C_TRANS_COMPLETE_INT_CLR : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description:  */
#define RTC_I2C_TRANS_COMPLETE_INT_CLR	   BIT(7)
#define RTC_I2C_TRANS_COMPLETE_INT_CLR_M  BIT(7)
#define RTC_I2C_TRANS_COMPLETE_INT_CLR_V  (1)
#define RTC_I2C_TRANS_COMPLETE_INT_CLR_S  (7)
/* RTC_I2C_MASTER_TRANS_COMPLETE_INT_CLR : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description:  */
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_CLR	   BIT(6)
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_CLR_M  BIT(6)
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_CLR_V  (1)
#define RTC_I2C_MASTER_TRANS_COMPLETE_INT_CLR_S  (6)
/* RTC_I2C_ARBITRATION_LOST_INT_CLR : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description:  */
#define RTC_I2C_ARBITRATION_LOST_INT_CLR	   BIT(5)
#define RTC_I2C_ARBITRATION_LOST_INT_CLR_M  BIT(5)
#define RTC_I2C_ARBITRATION_LOST_INT_CLR_V  (1)
#define RTC_I2C_ARBITRATION_LOST_INT_CLR_S  (5)
/* RTC_I2C_SLAVE_TRANS_COMPLETE_INT_CLR : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description:  */
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_CLR	   BIT(4)
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_CLR_M  BIT(4)
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_CLR_V  (1)
#define RTC_I2C_SLAVE_TRANS_COMPLETE_INT_CLR_S  (4)

/* Interrupt enable register.
 * Bit definitions are not given here, because interrupt functionality
 * of RTC_I2C is not used.
 */
#define RTC_I2C_INT_EN_REG          (DR_REG_RTC_I2C_BASE + 0x028)

/* Masked interrupt status register.
 * Bit definitions are not given here, because interrupt functionality
 * of RTC_I2C is not used.
 */
#define RTC_I2C_INT_ST_REG          (DR_REG_RTC_I2C_BASE + 0x02c)

#define RTC_I2C_SDA_DUTY_REG          (DR_REG_RTC_I2C_BASE + 0x030)
/* RTC_I2C_SDA_DUTY : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: Number of FAST_CLK cycles SDA will switch after falling edge of SCL */
#define RTC_I2C_SDA_DUTY	0xFFFFF
#define RTC_I2C_SDA_DUTY_M  ((RTC_I2C_SDA_DUTY_V)<<(RTC_I2C_SDA_DUTY_S))
#define RTC_I2C_SDA_DUTY_V  0xFFFFF
#define RTC_I2C_SDA_DUTY_S  0

#define RTC_I2C_SCL_HIGH_PERIOD_REG          (DR_REG_RTC_I2C_BASE + 0x038)
/* RTC_I2C_SCL_HIGH_PERIOD : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: Number of FAST_CLK cycles for SCL to be high */
#define RTC_I2C_SCL_HIGH_PERIOD	   0xFFFFF
#define RTC_I2C_SCL_HIGH_PERIOD_M  ((RTC_I2C_SCL_HIGH_PERIOD_V)<<(RTC_I2C_SCL_HIGH_PERIOD_S))
#define RTC_I2C_SCL_HIGH_PERIOD_V  0xFFFFF
#define RTC_I2C_SCL_HIGH_PERIOD_S  0

#define RTC_I2C_SCL_START_PERIOD_REG          (DR_REG_RTC_I2C_BASE + 0x040)
/* RTC_I2C_SCL_START_PERIOD : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: Number of FAST_CLK cycles to wait before generating start condition */
#define RTC_I2C_SCL_START_PERIOD	0xFFFFF
#define RTC_I2C_SCL_START_PERIOD_M  ((RTC_I2C_SCL_START_PERIOD_V)<<(RTC_I2C_SCL_START_PERIOD_S))
#define RTC_I2C_SCL_START_PERIOD_V  0xFFFFF
#define RTC_I2C_SCL_START_PERIOD_S  0

#define RTC_I2C_SCL_STOP_PERIOD_REG          (DR_REG_RTC_I2C_BASE + 0x044)
/* RTC_I2C_SCL_STOP_PERIOD : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: Number of FAST_CLK cycles to wait before generating stop condition */
#define RTC_I2C_SCL_STOP_PERIOD	   0xFFFFF
#define RTC_I2C_SCL_STOP_PERIOD_M  ((RTC_I2C_SCL_STOP_PERIOD_V)<<(RTC_I2C_SCL_STOP_PERIOD_S))
#define RTC_I2C_SCL_STOP_PERIOD_V  0xFFFFF
#define RTC_I2C_SCL_STOP_PERIOD_S  0

/* A block of 16 RTC_I2C_CMD registers which describe I2C operation to be
 * performed. Unused when ULP is controlling RTC_I2C.
 */
#define RTC_I2C_CMD_REG_COUNT   16
#define RTC_I2C_CMD_REG(i)          (DR_REG_RTC_I2C_BASE + 0x048 + (i) * 4)
/* RTC_I2C_CMD_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Bit is set by HW when command is done */
#define RTC_I2C_CMD_DONE	   BIT(31)
#define RTC_I2C_CMD_DONE_M  BIT(31)
#define RTC_I2C_CMD_DONE_V  (1)
#define RTC_I2C_CMD_DONE_S  (31)
/* RTC_I2C_VAL : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: Command content */
#define RTC_I2C_VAL	   0
#define RTC_I2C_VAL_M  ((RTC_I2C_VAL_V)<<(RTC_I2C_VAL_S))
#define RTC_I2C_VAL_V  0x3FFF
#define RTC_I2C_VAL_S  0

