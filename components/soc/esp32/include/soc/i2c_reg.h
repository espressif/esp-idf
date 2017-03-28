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
#ifndef _SOC_I2C_REG_H_
#define _SOC_I2C_REG_H_


#include "soc.h"

#define REG_I2C_BASE(i)    (DR_REG_I2C_EXT_BASE + (i) * 0x14000 )

#define I2C_SCL_LOW_PERIOD_REG(i)          (REG_I2C_BASE(i) + 0x0000)
/* I2C_SCL_LOW_PERIOD : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This register is used to configure the  low level width of SCL clock.*/
#define I2C_SCL_LOW_PERIOD  0x00003FFF
#define I2C_SCL_LOW_PERIOD_M  ((I2C_SCL_LOW_PERIOD_V)<<(I2C_SCL_LOW_PERIOD_S))
#define I2C_SCL_LOW_PERIOD_V  0x3FFF
#define I2C_SCL_LOW_PERIOD_S  0

#define I2C_CTR_REG(i)          (REG_I2C_BASE(i) + 0x0004)
/* I2C_CLK_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: This is the clock gating control bit for reading or writing registers.*/
#define I2C_CLK_EN  (BIT(8))
#define I2C_CLK_EN_M  (BIT(8))
#define I2C_CLK_EN_V  0x1
#define I2C_CLK_EN_S  8
/* I2C_RX_LSB_FIRST : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: This bit is used to control the storage mode for received datas.
 1: receive data from most significant bit    0: receive data from least significant bit*/
#define I2C_RX_LSB_FIRST  (BIT(7))
#define I2C_RX_LSB_FIRST_M  (BIT(7))
#define I2C_RX_LSB_FIRST_V  0x1
#define I2C_RX_LSB_FIRST_S  7
/* I2C_TX_LSB_FIRST : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: This bit is used to control the sending mode for  data need to
 be send. 1: receive data from most significant bit    0: receive data from least significant bit*/
#define I2C_TX_LSB_FIRST  (BIT(6))
#define I2C_TX_LSB_FIRST_M  (BIT(6))
#define I2C_TX_LSB_FIRST_V  0x1
#define I2C_TX_LSB_FIRST_S  6
/* I2C_TRANS_START : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to start sending data in txfifo.*/
#define I2C_TRANS_START  (BIT(5))
#define I2C_TRANS_START_M  (BIT(5))
#define I2C_TRANS_START_V  0x1
#define I2C_TRANS_START_S  5
/* I2C_MS_MODE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to configure the module as i2c master  clear this
 bit to configure the module as i2c slave.*/
#define I2C_MS_MODE  (BIT(4))
#define I2C_MS_MODE_M  (BIT(4))
#define I2C_MS_MODE_V  0x1
#define I2C_MS_MODE_S  4
/* I2C_SAMPLE_SCL_LEVEL : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to sample data in SCL low level. clear this bit
 to sample data in SCL high level.*/
#define I2C_SAMPLE_SCL_LEVEL  (BIT(2))
#define I2C_SAMPLE_SCL_LEVEL_M  (BIT(2))
#define I2C_SAMPLE_SCL_LEVEL_V  0x1
#define I2C_SAMPLE_SCL_LEVEL_S  2
/* I2C_SCL_FORCE_OUT : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: 1: normally ouput scl clock  0: exchange the function of scl_o
 and scl_oe (scl_o is the original internal output scl signal  scl_oe is the enable bit for the internal output scl signal)*/
#define I2C_SCL_FORCE_OUT  (BIT(1))
#define I2C_SCL_FORCE_OUT_M  (BIT(1))
#define I2C_SCL_FORCE_OUT_V  0x1
#define I2C_SCL_FORCE_OUT_S  1
/* I2C_SDA_FORCE_OUT : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: 1: normally ouput sda data   0: exchange the function of sda_o
 and sda_oe (sda_o is the original internal output sda signal sda_oe is the enable bit for the internal output sda signal)*/
#define I2C_SDA_FORCE_OUT  (BIT(0))
#define I2C_SDA_FORCE_OUT_M  (BIT(0))
#define I2C_SDA_FORCE_OUT_V  0x1
#define I2C_SDA_FORCE_OUT_S  0

#define I2C_SR_REG(i)          (REG_I2C_BASE(i) + 0x0008)
/* I2C_SCL_STATE_LAST : RO ;bitpos:[30:28] ;default: 3'b0 ; */
/*description: This register stores the value of state machine to produce SCL.
 3'h0: SCL_IDLE  3'h1:SCL_START   3'h2:SCL_LOW_EDGE  3'h3: SCL_LOW   3'h4:SCL_HIGH_EDGE   3'h5:SCL_HIGH  3'h6:SCL_STOP*/
#define I2C_SCL_STATE_LAST  0x00000007
#define I2C_SCL_STATE_LAST_M  ((I2C_SCL_STATE_LAST_V)<<(I2C_SCL_STATE_LAST_S))
#define I2C_SCL_STATE_LAST_V  0x7
#define I2C_SCL_STATE_LAST_S  28
/* I2C_SCL_MAIN_STATE_LAST : RO ;bitpos:[26:24] ;default: 3'b0 ; */
/*description: This register stores the value of state machine for i2c module.
  3'h0: SCL_MAIN_IDLE  3'h1: SCL_ADDRESS_SHIFT 3'h2: SCL_ACK_ADDRESS  3'h3: SCL_RX_DATA  3'h4 SCL_TX_DATA  3'h5:SCL_SEND_ACK 3'h6:SCL_WAIT_ACK*/
#define I2C_SCL_MAIN_STATE_LAST  0x00000007
#define I2C_SCL_MAIN_STATE_LAST_M  ((I2C_SCL_MAIN_STATE_LAST_V)<<(I2C_SCL_MAIN_STATE_LAST_S))
#define I2C_SCL_MAIN_STATE_LAST_V  0x7
#define I2C_SCL_MAIN_STATE_LAST_S  24
/* I2C_TXFIFO_CNT : RO ;bitpos:[23:18] ;default: 6'b0 ; */
/*description: This register stores the amount of received data  in ram.*/
#define I2C_TXFIFO_CNT  0x0000003F
#define I2C_TXFIFO_CNT_M  ((I2C_TXFIFO_CNT_V)<<(I2C_TXFIFO_CNT_S))
#define I2C_TXFIFO_CNT_V  0x3F
#define I2C_TXFIFO_CNT_S  18
/* I2C_RXFIFO_CNT : RO ;bitpos:[13:8] ;default: 6'b0 ; */
/*description: This register represent the amount of data need to send.*/
#define I2C_RXFIFO_CNT  0x0000003F
#define I2C_RXFIFO_CNT_M  ((I2C_RXFIFO_CNT_V)<<(I2C_RXFIFO_CNT_S))
#define I2C_RXFIFO_CNT_V  0x3F
#define I2C_RXFIFO_CNT_S  8
/* I2C_BYTE_TRANS : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: This register changes to high level when one byte is transferred.*/
#define I2C_BYTE_TRANS  (BIT(6))
#define I2C_BYTE_TRANS_M  (BIT(6))
#define I2C_BYTE_TRANS_V  0x1
#define I2C_BYTE_TRANS_S  6
/* I2C_SLAVE_ADDRESSED : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: when configured as i2c slave  and the address send by master
 is equal to slave's address  then this bit will be high level.*/
#define I2C_SLAVE_ADDRESSED  (BIT(5))
#define I2C_SLAVE_ADDRESSED_M  (BIT(5))
#define I2C_SLAVE_ADDRESSED_V  0x1
#define I2C_SLAVE_ADDRESSED_S  5
/* I2C_BUS_BUSY : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: 1:I2C bus is busy transferring data. 0:I2C bus is in idle state.*/
#define I2C_BUS_BUSY  (BIT(4))
#define I2C_BUS_BUSY_M  (BIT(4))
#define I2C_BUS_BUSY_V  0x1
#define I2C_BUS_BUSY_S  4
/* I2C_ARB_LOST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: when I2C lost control of SDA line  this register changes to high level.*/
#define I2C_ARB_LOST  (BIT(3))
#define I2C_ARB_LOST_M  (BIT(3))
#define I2C_ARB_LOST_V  0x1
#define I2C_ARB_LOST_S  3
/* I2C_TIME_OUT : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: when I2C takes more than time_out_reg clocks to receive a data
 then this register changes to high level.*/
#define I2C_TIME_OUT  (BIT(2))
#define I2C_TIME_OUT_M  (BIT(2))
#define I2C_TIME_OUT_V  0x1
#define I2C_TIME_OUT_S  2
/* I2C_SLAVE_RW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: when in slave mode  1: master read slave  0: master write slave.*/
#define I2C_SLAVE_RW  (BIT(1))
#define I2C_SLAVE_RW_M  (BIT(1))
#define I2C_SLAVE_RW_V  0x1
#define I2C_SLAVE_RW_S  1
/* I2C_ACK_REC : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: This register stores the value of ACK bit.*/
#define I2C_ACK_REC  (BIT(0))
#define I2C_ACK_REC_M  (BIT(0))
#define I2C_ACK_REC_V  0x1
#define I2C_ACK_REC_S  0

#define I2C_TO_REG(i)          (REG_I2C_BASE(i) + 0x000c)
/* I2C_TIME_OUT_REG : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: This register is used to configure the max clock number of receiving  a data.*/
#define I2C_TIME_OUT_REG  0x000FFFFF
#define I2C_TIME_OUT_REG_M  ((I2C_TIME_OUT_REG_V)<<(I2C_TIME_OUT_REG_S))
#define I2C_TIME_OUT_REG_V  0xFFFFF
#define I2C_TIME_OUT_REG_S  0

#define I2C_SLAVE_ADDR_REG(i)          (REG_I2C_BASE(i) + 0x0010)
/* I2C_ADDR_10BIT_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: This register is used to enable slave 10bit address mode.*/
#define I2C_ADDR_10BIT_EN  (BIT(31))
#define I2C_ADDR_10BIT_EN_M  (BIT(31))
#define I2C_ADDR_10BIT_EN_V  0x1
#define I2C_ADDR_10BIT_EN_S  31
/* I2C_SLAVE_ADDR : R/W ;bitpos:[14:0] ;default: 15'b0 ; */
/*description: when configured as i2c slave  this register is used to configure
 slave's address.*/
#define I2C_SLAVE_ADDR  0x00007FFF
#define I2C_SLAVE_ADDR_M  ((I2C_SLAVE_ADDR_V)<<(I2C_SLAVE_ADDR_S))
#define I2C_SLAVE_ADDR_V  0x7FFF
#define I2C_SLAVE_ADDR_S  0

#define I2C_RXFIFO_ST_REG(i)          (REG_I2C_BASE(i) + 0x0014)
/* I2C_TXFIFO_END_ADDR : RO ;bitpos:[19:15] ;default: 5'b0 ; */
/*description: This is the offset address of the last  sending data as described
 in nonfifo_tx_thres register.*/
#define I2C_TXFIFO_END_ADDR  0x0000001F
#define I2C_TXFIFO_END_ADDR_M  ((I2C_TXFIFO_END_ADDR_V)<<(I2C_TXFIFO_END_ADDR_S))
#define I2C_TXFIFO_END_ADDR_V  0x1F
#define I2C_TXFIFO_END_ADDR_S  15
/* I2C_TXFIFO_START_ADDR : RO ;bitpos:[14:10] ;default: 5'b0 ; */
/*description: This is the offset address of the first  sending data as described
 in nonfifo_tx_thres register.*/
#define I2C_TXFIFO_START_ADDR  0x0000001F
#define I2C_TXFIFO_START_ADDR_M  ((I2C_TXFIFO_START_ADDR_V)<<(I2C_TXFIFO_START_ADDR_S))
#define I2C_TXFIFO_START_ADDR_V  0x1F
#define I2C_TXFIFO_START_ADDR_S  10
/* I2C_RXFIFO_END_ADDR : RO ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: This is the offset address of the first receiving data as described
 in nonfifo_rx_thres_register.*/
#define I2C_RXFIFO_END_ADDR  0x0000001F
#define I2C_RXFIFO_END_ADDR_M  ((I2C_RXFIFO_END_ADDR_V)<<(I2C_RXFIFO_END_ADDR_S))
#define I2C_RXFIFO_END_ADDR_V  0x1F
#define I2C_RXFIFO_END_ADDR_S  5
/* I2C_RXFIFO_START_ADDR : RO ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: This is the offset address of the last receiving data as described
 in nonfifo_rx_thres_register.*/
#define I2C_RXFIFO_START_ADDR  0x0000001F
#define I2C_RXFIFO_START_ADDR_M  ((I2C_RXFIFO_START_ADDR_V)<<(I2C_RXFIFO_START_ADDR_S))
#define I2C_RXFIFO_START_ADDR_V  0x1F
#define I2C_RXFIFO_START_ADDR_S  0

#define I2C_FIFO_CONF_REG(i)          (REG_I2C_BASE(i) + 0x0018)
/* I2C_NONFIFO_TX_THRES : R/W ;bitpos:[25:20] ;default: 6'h15 ; */
/*description: when I2C sends more than nonfifo_tx_thres data  it will produce
 tx_send_empty_int_raw interrupt and update the current offset address of the sending data.*/
#define I2C_NONFIFO_TX_THRES  0x0000003F
#define I2C_NONFIFO_TX_THRES_M  ((I2C_NONFIFO_TX_THRES_V)<<(I2C_NONFIFO_TX_THRES_S))
#define I2C_NONFIFO_TX_THRES_V  0x3F
#define I2C_NONFIFO_TX_THRES_S  20
/* I2C_NONFIFO_RX_THRES : R/W ;bitpos:[19:14] ;default: 6'h15 ; */
/*description: when I2C receives more than nonfifo_rx_thres data  it will produce
 rx_send_full_int_raw interrupt and update the current offset address of the receiving data.*/
#define I2C_NONFIFO_RX_THRES  0x0000003F
#define I2C_NONFIFO_RX_THRES_M  ((I2C_NONFIFO_RX_THRES_V)<<(I2C_NONFIFO_RX_THRES_S))
#define I2C_NONFIFO_RX_THRES_V  0x3F
#define I2C_NONFIFO_RX_THRES_S  14
/* I2C_TX_FIFO_RST : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to reset tx fifo when using apb fifo access.*/
#define I2C_TX_FIFO_RST  (BIT(13))
#define I2C_TX_FIFO_RST_M  (BIT(13))
#define I2C_TX_FIFO_RST_V  0x1
#define I2C_TX_FIFO_RST_S  13
/* I2C_RX_FIFO_RST : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to reset rx fifo when using apb fifo access.*/
#define I2C_RX_FIFO_RST  (BIT(12))
#define I2C_RX_FIFO_RST_M  (BIT(12))
#define I2C_RX_FIFO_RST_V  0x1
#define I2C_RX_FIFO_RST_S  12
/* I2C_FIFO_ADDR_CFG_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: When this bit is set to 1 then the byte after address represent
 the offset address of I2C Slave's ram.*/
#define I2C_FIFO_ADDR_CFG_EN  (BIT(11))
#define I2C_FIFO_ADDR_CFG_EN_M  (BIT(11))
#define I2C_FIFO_ADDR_CFG_EN_V  0x1
#define I2C_FIFO_ADDR_CFG_EN_S  11
/* I2C_NONFIFO_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to enble apb nonfifo access.*/
#define I2C_NONFIFO_EN  (BIT(10))
#define I2C_NONFIFO_EN_M  (BIT(10))
#define I2C_NONFIFO_EN_V  0x1
#define I2C_NONFIFO_EN_S  10
/* I2C_TXFIFO_EMPTY_THRHD : R/W ;bitpos:[9:5] ;default: 5'h4 ; */
/*description: Config txfifo empty threhd value when using apb fifo access*/
#define I2C_TXFIFO_EMPTY_THRHD  0x0000001F
#define I2C_TXFIFO_EMPTY_THRHD_M  ((I2C_TXFIFO_EMPTY_THRHD_V)<<(I2C_TXFIFO_EMPTY_THRHD_S))
#define I2C_TXFIFO_EMPTY_THRHD_V  0x1F
#define I2C_TXFIFO_EMPTY_THRHD_S  5
/* I2C_RXFIFO_FULL_THRHD : R/W ;bitpos:[4:0] ;default: 5'hb ; */
/*description: */
#define I2C_RXFIFO_FULL_THRHD  0x0000001F
#define I2C_RXFIFO_FULL_THRHD_M  ((I2C_RXFIFO_FULL_THRHD_V)<<(I2C_RXFIFO_FULL_THRHD_S))
#define I2C_RXFIFO_FULL_THRHD_V  0x1F
#define I2C_RXFIFO_FULL_THRHD_S  0

#define I2C_DATA_APB_REG(i)      (0x60013000 + (i) * 0x14000 + 0x001c)

#define I2C_DATA_REG(i)          (REG_I2C_BASE(i) + 0x001c)
/* I2C_FIFO_RDATA : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: The register represent the byte  data read from rxfifo when use apb fifo access*/
#define I2C_FIFO_RDATA  0x000000FF
#define I2C_FIFO_RDATA_M  ((I2C_FIFO_RDATA_V)<<(I2C_FIFO_RDATA_S))
#define I2C_FIFO_RDATA_V  0xFF
#define I2C_FIFO_RDATA_S  0

#define I2C_INT_RAW_REG(i)          (REG_I2C_BASE(i) + 0x0020)
/* I2C_TX_SEND_EMPTY_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for tx_send_empty_int interrupt.when
 I2C sends more data than nonfifo_tx_thres  it will produce tx_send_empty_int interrupt..*/
#define I2C_TX_SEND_EMPTY_INT_RAW  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_RAW_M  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_RAW_V  0x1
#define I2C_TX_SEND_EMPTY_INT_RAW_S  12
/* I2C_RX_REC_FULL_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for rx_rec_full_int interrupt. when
 I2C receives more data  than nonfifo_rx_thres  it will produce rx_rec_full_int interrupt.*/
#define I2C_RX_REC_FULL_INT_RAW  (BIT(11))
#define I2C_RX_REC_FULL_INT_RAW_M  (BIT(11))
#define I2C_RX_REC_FULL_INT_RAW_V  0x1
#define I2C_RX_REC_FULL_INT_RAW_S  11
/* I2C_ACK_ERR_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for ack_err_int interrupt. when
 I2C receives a wrong ACK bit  it will produce ack_err_int interrupt..*/
#define I2C_ACK_ERR_INT_RAW  (BIT(10))
#define I2C_ACK_ERR_INT_RAW_M  (BIT(10))
#define I2C_ACK_ERR_INT_RAW_V  0x1
#define I2C_ACK_ERR_INT_RAW_S  10
/* I2C_TRANS_START_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for trans_start_int interrupt. when
 I2C sends the START bit it will produce trans_start_int interrupt.*/
#define I2C_TRANS_START_INT_RAW  (BIT(9))
#define I2C_TRANS_START_INT_RAW_M  (BIT(9))
#define I2C_TRANS_START_INT_RAW_V  0x1
#define I2C_TRANS_START_INT_RAW_S  9
/* I2C_TIME_OUT_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for time_out_int interrupt. when
 I2C takes a lot of time to receive a data  it will produce  time_out_int interrupt.*/
#define I2C_TIME_OUT_INT_RAW  (BIT(8))
#define I2C_TIME_OUT_INT_RAW_M  (BIT(8))
#define I2C_TIME_OUT_INT_RAW_V  0x1
#define I2C_TIME_OUT_INT_RAW_S  8
/* I2C_TRANS_COMPLETE_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for trans_complete_int interrupt.
 when I2C Master finished STOP command  it will produce trans_complete_int interrupt.*/
#define I2C_TRANS_COMPLETE_INT_RAW  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_RAW_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_RAW_V  0x1
#define I2C_TRANS_COMPLETE_INT_RAW_S  7
/* I2C_MASTER_TRAN_COMP_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for master_tra_comp_int interrupt.
 when I2C Master sends or receives a byte it will produce master_tran_comp_int interrupt.*/
#define I2C_MASTER_TRAN_COMP_INT_RAW  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_RAW_M  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_RAW_V  0x1
#define I2C_MASTER_TRAN_COMP_INT_RAW_S  6
/* I2C_ARBITRATION_LOST_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for arbitration_lost_int interrupt.when
 I2C lost the usage right of I2C BUS it will produce arbitration_lost_int interrupt.*/
#define I2C_ARBITRATION_LOST_INT_RAW  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_RAW_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_RAW_V  0x1
#define I2C_ARBITRATION_LOST_INT_RAW_S  5
/* I2C_SLAVE_TRAN_COMP_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for slave_tran_comp_int interrupt.
 when I2C Slave detectsthe STOP bit  it will produce slave_tran_comp_int interrupt.*/
#define I2C_SLAVE_TRAN_COMP_INT_RAW  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_RAW_M  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_RAW_V  0x1
#define I2C_SLAVE_TRAN_COMP_INT_RAW_S  4
/* I2C_END_DETECT_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for end_detect_int interrupt. when
 I2C deals with  the END command  it will produce end_detect_int interrupt.*/
#define I2C_END_DETECT_INT_RAW  (BIT(3))
#define I2C_END_DETECT_INT_RAW_M  (BIT(3))
#define I2C_END_DETECT_INT_RAW_V  0x1
#define I2C_END_DETECT_INT_RAW_S  3
/* I2C_RXFIFO_OVF_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for receiving data overflow when
 use apb fifo access.*/
#define I2C_RXFIFO_OVF_INT_RAW  (BIT(2))
#define I2C_RXFIFO_OVF_INT_RAW_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_RAW_V  0x1
#define I2C_RXFIFO_OVF_INT_RAW_S  2
/* I2C_TXFIFO_EMPTY_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for txfifo empty when use apb fifo access.*/
#define I2C_TXFIFO_EMPTY_INT_RAW  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_RAW_M  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_RAW_V  0x1
#define I2C_TXFIFO_EMPTY_INT_RAW_S  1
/* I2C_RXFIFO_FULL_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for rxfifo full when use apb fifo access.*/
#define I2C_RXFIFO_FULL_INT_RAW  (BIT(0))
#define I2C_RXFIFO_FULL_INT_RAW_M  (BIT(0))
#define I2C_RXFIFO_FULL_INT_RAW_V  0x1
#define I2C_RXFIFO_FULL_INT_RAW_S  0

#define I2C_INT_CLR_REG(i)          (REG_I2C_BASE(i) + 0x0024)
/* I2C_TX_SEND_EMPTY_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the tx_send_empty_int interrupt.*/
#define I2C_TX_SEND_EMPTY_INT_CLR  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_CLR_M  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_CLR_V  0x1
#define I2C_TX_SEND_EMPTY_INT_CLR_S  12
/* I2C_RX_REC_FULL_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the rx_rec_full_int interrupt.*/
#define I2C_RX_REC_FULL_INT_CLR  (BIT(11))
#define I2C_RX_REC_FULL_INT_CLR_M  (BIT(11))
#define I2C_RX_REC_FULL_INT_CLR_V  0x1
#define I2C_RX_REC_FULL_INT_CLR_S  11
/* I2C_ACK_ERR_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the ack_err_int interrupt.*/
#define I2C_ACK_ERR_INT_CLR  (BIT(10))
#define I2C_ACK_ERR_INT_CLR_M  (BIT(10))
#define I2C_ACK_ERR_INT_CLR_V  0x1
#define I2C_ACK_ERR_INT_CLR_S  10
/* I2C_TRANS_START_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the trans_start_int interrupt.*/
#define I2C_TRANS_START_INT_CLR  (BIT(9))
#define I2C_TRANS_START_INT_CLR_M  (BIT(9))
#define I2C_TRANS_START_INT_CLR_V  0x1
#define I2C_TRANS_START_INT_CLR_S  9
/* I2C_TIME_OUT_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the time_out_int interrupt.*/
#define I2C_TIME_OUT_INT_CLR  (BIT(8))
#define I2C_TIME_OUT_INT_CLR_M  (BIT(8))
#define I2C_TIME_OUT_INT_CLR_V  0x1
#define I2C_TIME_OUT_INT_CLR_S  8
/* I2C_TRANS_COMPLETE_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the trans_complete_int interrupt.*/
#define I2C_TRANS_COMPLETE_INT_CLR  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_CLR_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_CLR_V  0x1
#define I2C_TRANS_COMPLETE_INT_CLR_S  7
/* I2C_MASTER_TRAN_COMP_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the master_tran_comp interrupt.*/
#define I2C_MASTER_TRAN_COMP_INT_CLR  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_CLR_M  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_CLR_V  0x1
#define I2C_MASTER_TRAN_COMP_INT_CLR_S  6
/* I2C_ARBITRATION_LOST_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the arbitration_lost_int interrupt.*/
#define I2C_ARBITRATION_LOST_INT_CLR  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_CLR_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_CLR_V  0x1
#define I2C_ARBITRATION_LOST_INT_CLR_S  5
/* I2C_SLAVE_TRAN_COMP_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the slave_tran_comp_int interrupt.*/
#define I2C_SLAVE_TRAN_COMP_INT_CLR  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_CLR_M  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_CLR_V  0x1
#define I2C_SLAVE_TRAN_COMP_INT_CLR_S  4
/* I2C_END_DETECT_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the end_detect_int interrupt.*/
#define I2C_END_DETECT_INT_CLR  (BIT(3))
#define I2C_END_DETECT_INT_CLR_M  (BIT(3))
#define I2C_END_DETECT_INT_CLR_V  0x1
#define I2C_END_DETECT_INT_CLR_S  3
/* I2C_RXFIFO_OVF_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the rxfifo_ovf_int interrupt.*/
#define I2C_RXFIFO_OVF_INT_CLR  (BIT(2))
#define I2C_RXFIFO_OVF_INT_CLR_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_CLR_V  0x1
#define I2C_RXFIFO_OVF_INT_CLR_S  2
/* I2C_TXFIFO_EMPTY_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the txfifo_empty_int interrupt.*/
#define I2C_TXFIFO_EMPTY_INT_CLR  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_CLR_M  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_CLR_V  0x1
#define I2C_TXFIFO_EMPTY_INT_CLR_S  1
/* I2C_RXFIFO_FULL_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the rxfifo_full_int interrupt.*/
#define I2C_RXFIFO_FULL_INT_CLR  (BIT(0))
#define I2C_RXFIFO_FULL_INT_CLR_M  (BIT(0))
#define I2C_RXFIFO_FULL_INT_CLR_V  0x1
#define I2C_RXFIFO_FULL_INT_CLR_S  0

#define I2C_INT_ENA_REG(i)          (REG_I2C_BASE(i) + 0x0028)
/* I2C_TX_SEND_EMPTY_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The enable bit for tx_send_empty_int interrupt.*/
#define I2C_TX_SEND_EMPTY_INT_ENA  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_ENA_M  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_ENA_V  0x1
#define I2C_TX_SEND_EMPTY_INT_ENA_S  12
/* I2C_RX_REC_FULL_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The enable bit for rx_rec_full_int interrupt.*/
#define I2C_RX_REC_FULL_INT_ENA  (BIT(11))
#define I2C_RX_REC_FULL_INT_ENA_M  (BIT(11))
#define I2C_RX_REC_FULL_INT_ENA_V  0x1
#define I2C_RX_REC_FULL_INT_ENA_S  11
/* I2C_ACK_ERR_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The enable bit for ack_err_int interrupt.*/
#define I2C_ACK_ERR_INT_ENA  (BIT(10))
#define I2C_ACK_ERR_INT_ENA_M  (BIT(10))
#define I2C_ACK_ERR_INT_ENA_V  0x1
#define I2C_ACK_ERR_INT_ENA_S  10
/* I2C_TRANS_START_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The enable bit for trans_start_int interrupt.*/
#define I2C_TRANS_START_INT_ENA  (BIT(9))
#define I2C_TRANS_START_INT_ENA_M  (BIT(9))
#define I2C_TRANS_START_INT_ENA_V  0x1
#define I2C_TRANS_START_INT_ENA_S  9
/* I2C_TIME_OUT_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The enable bit for time_out_int interrupt.*/
#define I2C_TIME_OUT_INT_ENA  (BIT(8))
#define I2C_TIME_OUT_INT_ENA_M  (BIT(8))
#define I2C_TIME_OUT_INT_ENA_V  0x1
#define I2C_TIME_OUT_INT_ENA_S  8
/* I2C_TRANS_COMPLETE_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The enable bit for trans_complete_int interrupt.*/
#define I2C_TRANS_COMPLETE_INT_ENA  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ENA_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ENA_V  0x1
#define I2C_TRANS_COMPLETE_INT_ENA_S  7
/* I2C_MASTER_TRAN_COMP_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The enable bit for master_tran_comp_int interrupt.*/
#define I2C_MASTER_TRAN_COMP_INT_ENA  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_ENA_M  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_ENA_V  0x1
#define I2C_MASTER_TRAN_COMP_INT_ENA_S  6
/* I2C_ARBITRATION_LOST_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The enable bit for arbitration_lost_int interrupt.*/
#define I2C_ARBITRATION_LOST_INT_ENA  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ENA_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ENA_V  0x1
#define I2C_ARBITRATION_LOST_INT_ENA_S  5
/* I2C_SLAVE_TRAN_COMP_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The enable bit for slave_tran_comp_int interrupt.*/
#define I2C_SLAVE_TRAN_COMP_INT_ENA  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_ENA_M  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_ENA_V  0x1
#define I2C_SLAVE_TRAN_COMP_INT_ENA_S  4
/* I2C_END_DETECT_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The enable bit for end_detect_int interrupt.*/
#define I2C_END_DETECT_INT_ENA  (BIT(3))
#define I2C_END_DETECT_INT_ENA_M  (BIT(3))
#define I2C_END_DETECT_INT_ENA_V  0x1
#define I2C_END_DETECT_INT_ENA_S  3
/* I2C_RXFIFO_OVF_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The enable bit for rxfifo_ovf_int interrupt.*/
#define I2C_RXFIFO_OVF_INT_ENA  (BIT(2))
#define I2C_RXFIFO_OVF_INT_ENA_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_ENA_V  0x1
#define I2C_RXFIFO_OVF_INT_ENA_S  2
/* I2C_TXFIFO_EMPTY_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable bit for txfifo_empty_int interrupt.*/
#define I2C_TXFIFO_EMPTY_INT_ENA  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_ENA_M  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_ENA_V  0x1
#define I2C_TXFIFO_EMPTY_INT_ENA_S  1
/* I2C_RXFIFO_FULL_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable bit for rxfifo_full_int interrupt.*/
#define I2C_RXFIFO_FULL_INT_ENA  (BIT(0))
#define I2C_RXFIFO_FULL_INT_ENA_M  (BIT(0))
#define I2C_RXFIFO_FULL_INT_ENA_V  0x1
#define I2C_RXFIFO_FULL_INT_ENA_S  0

#define I2C_INT_STATUS_REG(i)          (REG_I2C_BASE(i) + 0x002c)
/* I2C_TX_SEND_EMPTY_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The masked interrupt status for tx_send_empty_int interrupt.*/
#define I2C_TX_SEND_EMPTY_INT_ST  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_ST_M  (BIT(12))
#define I2C_TX_SEND_EMPTY_INT_ST_V  0x1
#define I2C_TX_SEND_EMPTY_INT_ST_S  12
/* I2C_RX_REC_FULL_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The masked interrupt status for rx_rec_full_int interrupt.*/
#define I2C_RX_REC_FULL_INT_ST  (BIT(11))
#define I2C_RX_REC_FULL_INT_ST_M  (BIT(11))
#define I2C_RX_REC_FULL_INT_ST_V  0x1
#define I2C_RX_REC_FULL_INT_ST_S  11
/* I2C_ACK_ERR_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The masked interrupt status for ack_err_int interrupt.*/
#define I2C_ACK_ERR_INT_ST  (BIT(10))
#define I2C_ACK_ERR_INT_ST_M  (BIT(10))
#define I2C_ACK_ERR_INT_ST_V  0x1
#define I2C_ACK_ERR_INT_ST_S  10
/* I2C_TRANS_START_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The masked interrupt status for trans_start_int interrupt.*/
#define I2C_TRANS_START_INT_ST  (BIT(9))
#define I2C_TRANS_START_INT_ST_M  (BIT(9))
#define I2C_TRANS_START_INT_ST_V  0x1
#define I2C_TRANS_START_INT_ST_S  9
/* I2C_TIME_OUT_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The masked interrupt status for time_out_int interrupt.*/
#define I2C_TIME_OUT_INT_ST  (BIT(8))
#define I2C_TIME_OUT_INT_ST_M  (BIT(8))
#define I2C_TIME_OUT_INT_ST_V  0x1
#define I2C_TIME_OUT_INT_ST_S  8
/* I2C_TRANS_COMPLETE_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The masked interrupt status for trans_complete_int interrupt.*/
#define I2C_TRANS_COMPLETE_INT_ST  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ST_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ST_V  0x1
#define I2C_TRANS_COMPLETE_INT_ST_S  7
/* I2C_MASTER_TRAN_COMP_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The masked interrupt status for master_tran_comp_int interrupt.*/
#define I2C_MASTER_TRAN_COMP_INT_ST  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_ST_M  (BIT(6))
#define I2C_MASTER_TRAN_COMP_INT_ST_V  0x1
#define I2C_MASTER_TRAN_COMP_INT_ST_S  6
/* I2C_ARBITRATION_LOST_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The masked interrupt status for arbitration_lost_int interrupt.*/
#define I2C_ARBITRATION_LOST_INT_ST  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ST_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ST_V  0x1
#define I2C_ARBITRATION_LOST_INT_ST_S  5
/* I2C_SLAVE_TRAN_COMP_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The masked interrupt status for slave_tran_comp_int interrupt.*/
#define I2C_SLAVE_TRAN_COMP_INT_ST  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_ST_M  (BIT(4))
#define I2C_SLAVE_TRAN_COMP_INT_ST_V  0x1
#define I2C_SLAVE_TRAN_COMP_INT_ST_S  4
/* I2C_END_DETECT_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The masked interrupt status for end_detect_int interrupt.*/
#define I2C_END_DETECT_INT_ST  (BIT(3))
#define I2C_END_DETECT_INT_ST_M  (BIT(3))
#define I2C_END_DETECT_INT_ST_V  0x1
#define I2C_END_DETECT_INT_ST_S  3
/* I2C_RXFIFO_OVF_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The masked interrupt status for rxfifo_ovf_int interrupt.*/
#define I2C_RXFIFO_OVF_INT_ST  (BIT(2))
#define I2C_RXFIFO_OVF_INT_ST_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_ST_V  0x1
#define I2C_RXFIFO_OVF_INT_ST_S  2
/* I2C_TXFIFO_EMPTY_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The masked interrupt status for txfifo_empty_int interrupt.*/
#define I2C_TXFIFO_EMPTY_INT_ST  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_ST_M  (BIT(1))
#define I2C_TXFIFO_EMPTY_INT_ST_V  0x1
#define I2C_TXFIFO_EMPTY_INT_ST_S  1
/* I2C_RXFIFO_FULL_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The masked interrupt status for rxfifo_full_int interrupt.*/
#define I2C_RXFIFO_FULL_INT_ST  (BIT(0))
#define I2C_RXFIFO_FULL_INT_ST_M  (BIT(0))
#define I2C_RXFIFO_FULL_INT_ST_V  0x1
#define I2C_RXFIFO_FULL_INT_ST_S  0

#define I2C_SDA_HOLD_REG(i)          (REG_I2C_BASE(i) + 0x0030)
/* I2C_SDA_HOLD_TIME : R/W ;bitpos:[9:0] ;default: 10'b0 ; */
/*description: This register is used to configure the clock num I2C used to
 hold the data after the negedge of SCL.*/
#define I2C_SDA_HOLD_TIME  0x000003FF
#define I2C_SDA_HOLD_TIME_M  ((I2C_SDA_HOLD_TIME_V)<<(I2C_SDA_HOLD_TIME_S))
#define I2C_SDA_HOLD_TIME_V  0x3FF
#define I2C_SDA_HOLD_TIME_S  0

#define I2C_SDA_SAMPLE_REG(i)          (REG_I2C_BASE(i) + 0x0034)
/* I2C_SDA_SAMPLE_TIME : R/W ;bitpos:[9:0] ;default: 10'b0 ; */
/*description: This register is used to configure the clock num I2C used to
 sample data on SDA after the posedge of SCL*/
#define I2C_SDA_SAMPLE_TIME  0x000003FF
#define I2C_SDA_SAMPLE_TIME_M  ((I2C_SDA_SAMPLE_TIME_V)<<(I2C_SDA_SAMPLE_TIME_S))
#define I2C_SDA_SAMPLE_TIME_V  0x3FF
#define I2C_SDA_SAMPLE_TIME_S  0

#define I2C_SCL_HIGH_PERIOD_REG(i)          (REG_I2C_BASE(i) + 0x0038)
/* I2C_SCL_HIGH_PERIOD : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This register is used to configure the clock num during SCL is low level.*/
#define I2C_SCL_HIGH_PERIOD  0x00003FFF
#define I2C_SCL_HIGH_PERIOD_M  ((I2C_SCL_HIGH_PERIOD_V)<<(I2C_SCL_HIGH_PERIOD_S))
#define I2C_SCL_HIGH_PERIOD_V  0x3FFF
#define I2C_SCL_HIGH_PERIOD_S  0

#define I2C_SCL_START_HOLD_REG(i)          (REG_I2C_BASE(i) + 0x0040)
/* I2C_SCL_START_HOLD_TIME : R/W ;bitpos:[9:0] ;default: 10'b1000 ; */
/*description: This register is used to configure the clock num between the
 negedge of SDA and negedge of SCL for start mark.*/
#define I2C_SCL_START_HOLD_TIME  0x000003FF
#define I2C_SCL_START_HOLD_TIME_M  ((I2C_SCL_START_HOLD_TIME_V)<<(I2C_SCL_START_HOLD_TIME_S))
#define I2C_SCL_START_HOLD_TIME_V  0x3FF
#define I2C_SCL_START_HOLD_TIME_S  0

#define I2C_SCL_RSTART_SETUP_REG(i)          (REG_I2C_BASE(i) + 0x0044)
/* I2C_SCL_RSTART_SETUP_TIME : R/W ;bitpos:[9:0] ;default: 10'b1000 ; */
/*description: This register is used to configure the clock num between the
 posedge of SCL and the negedge of SDA for restart mark.*/
#define I2C_SCL_RSTART_SETUP_TIME  0x000003FF
#define I2C_SCL_RSTART_SETUP_TIME_M  ((I2C_SCL_RSTART_SETUP_TIME_V)<<(I2C_SCL_RSTART_SETUP_TIME_S))
#define I2C_SCL_RSTART_SETUP_TIME_V  0x3FF
#define I2C_SCL_RSTART_SETUP_TIME_S  0

#define I2C_SCL_STOP_HOLD_REG(i)          (REG_I2C_BASE(i) + 0x0048)
/* I2C_SCL_STOP_HOLD_TIME : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This register is used to configure the clock num after the STOP bit's posedge.*/
#define I2C_SCL_STOP_HOLD_TIME  0x00003FFF
#define I2C_SCL_STOP_HOLD_TIME_M  ((I2C_SCL_STOP_HOLD_TIME_V)<<(I2C_SCL_STOP_HOLD_TIME_S))
#define I2C_SCL_STOP_HOLD_TIME_V  0x3FFF
#define I2C_SCL_STOP_HOLD_TIME_S  0

#define I2C_SCL_STOP_SETUP_REG(i)          (REG_I2C_BASE(i) + 0x004C)
/* I2C_SCL_STOP_SETUP_TIME : R/W ;bitpos:[9:0] ;default: 10'b0 ; */
/*description: This register is used to configure the clock num between the
 posedge of SCL and the posedge of SDA.*/
#define I2C_SCL_STOP_SETUP_TIME  0x000003FF
#define I2C_SCL_STOP_SETUP_TIME_M  ((I2C_SCL_STOP_SETUP_TIME_V)<<(I2C_SCL_STOP_SETUP_TIME_S))
#define I2C_SCL_STOP_SETUP_TIME_V  0x3FF
#define I2C_SCL_STOP_SETUP_TIME_S  0

#define I2C_SCL_FILTER_CFG_REG(i)          (REG_I2C_BASE(i) + 0x0050)
/* I2C_SCL_FILTER_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: This is the filter enable bit for SCL.*/
#define I2C_SCL_FILTER_EN  (BIT(3))
#define I2C_SCL_FILTER_EN_M  (BIT(3))
#define I2C_SCL_FILTER_EN_V  0x1
#define I2C_SCL_FILTER_EN_S  3
/* I2C_SCL_FILTER_THRES : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: When input SCL's pulse width is smaller than this register value
  I2C ignores this pulse.*/
#define I2C_SCL_FILTER_THRES  0x00000007
#define I2C_SCL_FILTER_THRES_M  ((I2C_SCL_FILTER_THRES_V)<<(I2C_SCL_FILTER_THRES_S))
#define I2C_SCL_FILTER_THRES_V  0x7
#define I2C_SCL_FILTER_THRES_S  0

#define I2C_SDA_FILTER_CFG_REG(i)          (REG_I2C_BASE(i) + 0x0054)
/* I2C_SDA_FILTER_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: This is the filter enable bit for SDA.*/
#define I2C_SDA_FILTER_EN  (BIT(3))
#define I2C_SDA_FILTER_EN_M  (BIT(3))
#define I2C_SDA_FILTER_EN_V  0x1
#define I2C_SDA_FILTER_EN_S  3
/* I2C_SDA_FILTER_THRES : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: When input SCL's pulse width is smaller than this register value
  I2C ignores this pulse.*/
#define I2C_SDA_FILTER_THRES  0x00000007
#define I2C_SDA_FILTER_THRES_M  ((I2C_SDA_FILTER_THRES_V)<<(I2C_SDA_FILTER_THRES_S))
#define I2C_SDA_FILTER_THRES_V  0x7
#define I2C_SDA_FILTER_THRES_S  0

#define I2C_COMD0_REG(i)          (REG_I2C_BASE(i) + 0x0058)
/* I2C_COMMAND0_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command0 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND0_DONE  (BIT(31))
#define I2C_COMMAND0_DONE_M  (BIT(31))
#define I2C_COMMAND0_DONE_V  0x1
#define I2C_COMMAND0_DONE_S  31
/* I2C_COMMAND0 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command0. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND0  0x00003FFF
#define I2C_COMMAND0_M  ((I2C_COMMAND0_V)<<(I2C_COMMAND0_S))
#define I2C_COMMAND0_V  0x3FFF
#define I2C_COMMAND0_S  0

#define I2C_COMD1_REG(i)          (REG_I2C_BASE(i) + 0x005C)
/* I2C_COMMAND1_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command1 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND1_DONE  (BIT(31))
#define I2C_COMMAND1_DONE_M  (BIT(31))
#define I2C_COMMAND1_DONE_V  0x1
#define I2C_COMMAND1_DONE_S  31
/* I2C_COMMAND1 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command1. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND1  0x00003FFF
#define I2C_COMMAND1_M  ((I2C_COMMAND1_V)<<(I2C_COMMAND1_S))
#define I2C_COMMAND1_V  0x3FFF
#define I2C_COMMAND1_S  0

#define I2C_COMD2_REG(i)          (REG_I2C_BASE(i) + 0x0060)
/* I2C_COMMAND2_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command2 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND2_DONE  (BIT(31))
#define I2C_COMMAND2_DONE_M  (BIT(31))
#define I2C_COMMAND2_DONE_V  0x1
#define I2C_COMMAND2_DONE_S  31
/* I2C_COMMAND2 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command2. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND2  0x00003FFF
#define I2C_COMMAND2_M  ((I2C_COMMAND2_V)<<(I2C_COMMAND2_S))
#define I2C_COMMAND2_V  0x3FFF
#define I2C_COMMAND2_S  0

#define I2C_COMD3_REG(i)          (REG_I2C_BASE(i) + 0x0064)
/* I2C_COMMAND3_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command3 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND3_DONE  (BIT(31))
#define I2C_COMMAND3_DONE_M  (BIT(31))
#define I2C_COMMAND3_DONE_V  0x1
#define I2C_COMMAND3_DONE_S  31
/* I2C_COMMAND3 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command3. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND3  0x00003FFF
#define I2C_COMMAND3_M  ((I2C_COMMAND3_V)<<(I2C_COMMAND3_S))
#define I2C_COMMAND3_V  0x3FFF
#define I2C_COMMAND3_S  0

#define I2C_COMD4_REG(i)          (REG_I2C_BASE(i) + 0x0068)
/* I2C_COMMAND4_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command4 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND4_DONE  (BIT(31))
#define I2C_COMMAND4_DONE_M  (BIT(31))
#define I2C_COMMAND4_DONE_V  0x1
#define I2C_COMMAND4_DONE_S  31
/* I2C_COMMAND4 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command4. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND4  0x00003FFF
#define I2C_COMMAND4_M  ((I2C_COMMAND4_V)<<(I2C_COMMAND4_S))
#define I2C_COMMAND4_V  0x3FFF
#define I2C_COMMAND4_S  0

#define I2C_COMD5_REG(i)          (REG_I2C_BASE(i) + 0x006C)
/* I2C_COMMAND5_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command5 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND5_DONE  (BIT(31))
#define I2C_COMMAND5_DONE_M  (BIT(31))
#define I2C_COMMAND5_DONE_V  0x1
#define I2C_COMMAND5_DONE_S  31
/* I2C_COMMAND5 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command5. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND5  0x00003FFF
#define I2C_COMMAND5_M  ((I2C_COMMAND5_V)<<(I2C_COMMAND5_S))
#define I2C_COMMAND5_V  0x3FFF
#define I2C_COMMAND5_S  0

#define I2C_COMD6_REG(i)          (REG_I2C_BASE(i) + 0x0070)
/* I2C_COMMAND6_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command6 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND6_DONE  (BIT(31))
#define I2C_COMMAND6_DONE_M  (BIT(31))
#define I2C_COMMAND6_DONE_V  0x1
#define I2C_COMMAND6_DONE_S  31
/* I2C_COMMAND6 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command6. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND6  0x00003FFF
#define I2C_COMMAND6_M  ((I2C_COMMAND6_V)<<(I2C_COMMAND6_S))
#define I2C_COMMAND6_V  0x3FFF
#define I2C_COMMAND6_S  0

#define I2C_COMD7_REG(i)          (REG_I2C_BASE(i) + 0x0074)
/* I2C_COMMAND7_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command7 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND7_DONE  (BIT(31))
#define I2C_COMMAND7_DONE_M  (BIT(31))
#define I2C_COMMAND7_DONE_V  0x1
#define I2C_COMMAND7_DONE_S  31
/* I2C_COMMAND7 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command7. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND7  0x00003FFF
#define I2C_COMMAND7_M  ((I2C_COMMAND7_V)<<(I2C_COMMAND7_S))
#define I2C_COMMAND7_V  0x3FFF
#define I2C_COMMAND7_S  0

#define I2C_COMD8_REG(i)          (REG_I2C_BASE(i) + 0x0078)
/* I2C_COMMAND8_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command8 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND8_DONE  (BIT(31))
#define I2C_COMMAND8_DONE_M  (BIT(31))
#define I2C_COMMAND8_DONE_V  0x1
#define I2C_COMMAND8_DONE_S  31
/* I2C_COMMAND8 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command8. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND8  0x00003FFF
#define I2C_COMMAND8_M  ((I2C_COMMAND8_V)<<(I2C_COMMAND8_S))
#define I2C_COMMAND8_V  0x3FFF
#define I2C_COMMAND8_S  0

#define I2C_COMD9_REG(i)          (REG_I2C_BASE(i) + 0x007C)
/* I2C_COMMAND9_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command9 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND9_DONE  (BIT(31))
#define I2C_COMMAND9_DONE_M  (BIT(31))
#define I2C_COMMAND9_DONE_V  0x1
#define I2C_COMMAND9_DONE_S  31
/* I2C_COMMAND9 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command9. It consists of three part. op_code
 is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND9  0x00003FFF
#define I2C_COMMAND9_M  ((I2C_COMMAND9_V)<<(I2C_COMMAND9_S))
#define I2C_COMMAND9_V  0x3FFF
#define I2C_COMMAND9_S  0

#define I2C_COMD10_REG(i)          (REG_I2C_BASE(i) + 0x0080)
/* I2C_COMMAND10_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command10 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND10_DONE  (BIT(31))
#define I2C_COMMAND10_DONE_M  (BIT(31))
#define I2C_COMMAND10_DONE_V  0x1
#define I2C_COMMAND10_DONE_S  31
/* I2C_COMMAND10 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command10. It consists of three part.
 op_code is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND10  0x00003FFF
#define I2C_COMMAND10_M  ((I2C_COMMAND10_V)<<(I2C_COMMAND10_S))
#define I2C_COMMAND10_V  0x3FFF
#define I2C_COMMAND10_S  0

#define I2C_COMD11_REG(i)          (REG_I2C_BASE(i) + 0x0084)
/* I2C_COMMAND11_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command11 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND11_DONE  (BIT(31))
#define I2C_COMMAND11_DONE_M  (BIT(31))
#define I2C_COMMAND11_DONE_V  0x1
#define I2C_COMMAND11_DONE_S  31
/* I2C_COMMAND11 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command11. It consists of three part.
 op_code is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND11  0x00003FFF
#define I2C_COMMAND11_M  ((I2C_COMMAND11_V)<<(I2C_COMMAND11_S))
#define I2C_COMMAND11_V  0x3FFF
#define I2C_COMMAND11_S  0

#define I2C_COMD12_REG(i)          (REG_I2C_BASE(i) + 0x0088)
/* I2C_COMMAND12_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command12 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND12_DONE  (BIT(31))
#define I2C_COMMAND12_DONE_M  (BIT(31))
#define I2C_COMMAND12_DONE_V  0x1
#define I2C_COMMAND12_DONE_S  31
/* I2C_COMMAND12 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command12. It consists of three part.
 op_code is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND12  0x00003FFF
#define I2C_COMMAND12_M  ((I2C_COMMAND12_V)<<(I2C_COMMAND12_S))
#define I2C_COMMAND12_V  0x3FFF
#define I2C_COMMAND12_S  0

#define I2C_COMD13_REG(i)          (REG_I2C_BASE(i) + 0x008C)
/* I2C_COMMAND13_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command13 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND13_DONE  (BIT(31))
#define I2C_COMMAND13_DONE_M  (BIT(31))
#define I2C_COMMAND13_DONE_V  0x1
#define I2C_COMMAND13_DONE_S  31
/* I2C_COMMAND13 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command13. It consists of three part.
 op_code is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND13  0x00003FFF
#define I2C_COMMAND13_M  ((I2C_COMMAND13_V)<<(I2C_COMMAND13_S))
#define I2C_COMMAND13_V  0x3FFF
#define I2C_COMMAND13_S  0

#define I2C_COMD14_REG(i)          (REG_I2C_BASE(i) + 0x0090)
/* I2C_COMMAND14_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command14 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND14_DONE  (BIT(31))
#define I2C_COMMAND14_DONE_M  (BIT(31))
#define I2C_COMMAND14_DONE_V  0x1
#define I2C_COMMAND14_DONE_S  31
/* I2C_COMMAND14 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command14. It consists of three part.
 op_code is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND14  0x00003FFF
#define I2C_COMMAND14_M  ((I2C_COMMAND14_V)<<(I2C_COMMAND14_S))
#define I2C_COMMAND14_V  0x3FFF
#define I2C_COMMAND14_S  0

#define I2C_COMD15_REG(i)          (REG_I2C_BASE(i) + 0x0094)
/* I2C_COMMAND15_DONE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When command15 is done in I2C Master mode  this bit changes to high level.*/
#define I2C_COMMAND15_DONE  (BIT(31))
#define I2C_COMMAND15_DONE_M  (BIT(31))
#define I2C_COMMAND15_DONE_V  0x1
#define I2C_COMMAND15_DONE_S  31
/* I2C_COMMAND15 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: This is the content of command15. It consists of three part.
 op_code is the command  0: RSTART   1: WRITE  2: READ  3: STOP . 4:END.  Byte_num represent the number of data need to be send or data need to be received. ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
#define I2C_COMMAND15  0x00003FFF
#define I2C_COMMAND15_M  ((I2C_COMMAND15_V)<<(I2C_COMMAND15_S))
#define I2C_COMMAND15_V  0x3FFF
#define I2C_COMMAND15_S  0

#define I2C_DATE_REG(i)          (REG_I2C_BASE(i) + 0x00F8)
/* I2C_DATE : R/W ;bitpos:[31:0] ;default: 32'h16042000 ; */
/*description: */
#define I2C_DATE  0xFFFFFFFF
#define I2C_DATE_M  ((I2C_DATE_V)<<(I2C_DATE_S))
#define I2C_DATE_V  0xFFFFFFFF
#define I2C_DATE_S  0

#define I2C_FIFO_START_ADDR_REG(i)          (REG_I2C_BASE(i) + 0x0100)




#endif /*_SOC_I2C_REG_H_ */


