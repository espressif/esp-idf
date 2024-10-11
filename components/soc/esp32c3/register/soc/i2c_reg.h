/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_I2C_REG_H_
#define _SOC_I2C_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc/soc.h"

#define I2C_SCL_LOW_PERIOD_REG(i)          (REG_I2C_BASE(i) + 0x0)
/* I2C_SCL_LOW_PERIOD : R/W ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: .*/
#define I2C_SCL_LOW_PERIOD    0x000001FF
#define I2C_SCL_LOW_PERIOD_M  ((I2C_SCL_LOW_PERIOD_V)<<(I2C_SCL_LOW_PERIOD_S))
#define I2C_SCL_LOW_PERIOD_V  0x1FF
#define I2C_SCL_LOW_PERIOD_S  0

#define I2C_CTR_REG(i)          (REG_I2C_BASE(i) + 0x4)
/* I2C_ADDR_BROADCASTING_EN : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ADDR_BROADCASTING_EN    (BIT(14))
#define I2C_ADDR_BROADCASTING_EN_M  (BIT(14))
#define I2C_ADDR_BROADCASTING_EN_V  0x1
#define I2C_ADDR_BROADCASTING_EN_S  14
/* I2C_ADDR_10BIT_RW_CHECK_EN : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ADDR_10BIT_RW_CHECK_EN    (BIT(13))
#define I2C_ADDR_10BIT_RW_CHECK_EN_M  (BIT(13))
#define I2C_ADDR_10BIT_RW_CHECK_EN_V  0x1
#define I2C_ADDR_10BIT_RW_CHECK_EN_S  13
/* I2C_SLV_TX_AUTO_START_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLV_TX_AUTO_START_EN    (BIT(12))
#define I2C_SLV_TX_AUTO_START_EN_M  (BIT(12))
#define I2C_SLV_TX_AUTO_START_EN_V  0x1
#define I2C_SLV_TX_AUTO_START_EN_S  12
/* I2C_CONF_UPGATE : WT ;bitpos:[11] ;default: 1'b0 ; */
/*description: .*/
#define I2C_CONF_UPGATE    (BIT(11))
#define I2C_CONF_UPGATE_M  (BIT(11))
#define I2C_CONF_UPGATE_V  0x1
#define I2C_CONF_UPGATE_S  11
/* I2C_FSM_RST : WT ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define I2C_FSM_RST    (BIT(10))
#define I2C_FSM_RST_M  (BIT(10))
#define I2C_FSM_RST_V  0x1
#define I2C_FSM_RST_S  10
/* I2C_ARBITRATION_EN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: .*/
#define I2C_ARBITRATION_EN    (BIT(9))
#define I2C_ARBITRATION_EN_M  (BIT(9))
#define I2C_ARBITRATION_EN_V  0x1
#define I2C_ARBITRATION_EN_S  9
/* I2C_CLK_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: .*/
#define I2C_CLK_EN    (BIT(8))
#define I2C_CLK_EN_M  (BIT(8))
#define I2C_CLK_EN_V  0x1
#define I2C_CLK_EN_S  8
/* I2C_RX_LSB_FIRST : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: .*/
#define I2C_RX_LSB_FIRST    (BIT(7))
#define I2C_RX_LSB_FIRST_M  (BIT(7))
#define I2C_RX_LSB_FIRST_V  0x1
#define I2C_RX_LSB_FIRST_S  7
/* I2C_TX_LSB_FIRST : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TX_LSB_FIRST    (BIT(6))
#define I2C_TX_LSB_FIRST_M  (BIT(6))
#define I2C_TX_LSB_FIRST_V  0x1
#define I2C_TX_LSB_FIRST_S  6
/* I2C_TRANS_START : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_START    (BIT(5))
#define I2C_TRANS_START_M  (BIT(5))
#define I2C_TRANS_START_V  0x1
#define I2C_TRANS_START_S  5
/* I2C_MS_MODE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define I2C_MS_MODE    (BIT(4))
#define I2C_MS_MODE_M  (BIT(4))
#define I2C_MS_MODE_V  0x1
#define I2C_MS_MODE_S  4
/* I2C_RX_FULL_ACK_LEVEL : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: .*/
#define I2C_RX_FULL_ACK_LEVEL    (BIT(3))
#define I2C_RX_FULL_ACK_LEVEL_M  (BIT(3))
#define I2C_RX_FULL_ACK_LEVEL_V  0x1
#define I2C_RX_FULL_ACK_LEVEL_S  3
/* I2C_SAMPLE_SCL_LEVEL : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SAMPLE_SCL_LEVEL    (BIT(2))
#define I2C_SAMPLE_SCL_LEVEL_M  (BIT(2))
#define I2C_SAMPLE_SCL_LEVEL_V  0x1
#define I2C_SAMPLE_SCL_LEVEL_S  2
/* I2C_SCL_FORCE_OUT : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define I2C_SCL_FORCE_OUT    (BIT(1))
#define I2C_SCL_FORCE_OUT_M  (BIT(1))
#define I2C_SCL_FORCE_OUT_V  0x1
#define I2C_SCL_FORCE_OUT_S  1
/* I2C_SDA_FORCE_OUT : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define I2C_SDA_FORCE_OUT    (BIT(0))
#define I2C_SDA_FORCE_OUT_M  (BIT(0))
#define I2C_SDA_FORCE_OUT_V  0x1
#define I2C_SDA_FORCE_OUT_S  0

#define I2C_SR_REG(i)          (REG_I2C_BASE(i) + 0x8)
/* I2C_SCL_STATE_LAST : RO ;bitpos:[30:28] ;default: 3'b0 ; */
/*description: .*/
#define I2C_SCL_STATE_LAST    0x00000007
#define I2C_SCL_STATE_LAST_M  ((I2C_SCL_STATE_LAST_V)<<(I2C_SCL_STATE_LAST_S))
#define I2C_SCL_STATE_LAST_V  0x7
#define I2C_SCL_STATE_LAST_S  28
/* I2C_SCL_MAIN_STATE_LAST : RO ;bitpos:[26:24] ;default: 3'b0 ; */
/*description: .*/
#define I2C_SCL_MAIN_STATE_LAST    0x00000007
#define I2C_SCL_MAIN_STATE_LAST_M  ((I2C_SCL_MAIN_STATE_LAST_V)<<(I2C_SCL_MAIN_STATE_LAST_S))
#define I2C_SCL_MAIN_STATE_LAST_V  0x7
#define I2C_SCL_MAIN_STATE_LAST_S  24
/* I2C_TXFIFO_CNT : RO ;bitpos:[23:18] ;default: 6'b0 ; */
/*description: .*/
#define I2C_TXFIFO_CNT    0x0000003F
#define I2C_TXFIFO_CNT_M  ((I2C_TXFIFO_CNT_V)<<(I2C_TXFIFO_CNT_S))
#define I2C_TXFIFO_CNT_V  0x3F
#define I2C_TXFIFO_CNT_S  18
/* I2C_STRETCH_CAUSE : RO ;bitpos:[15:14] ;default: 2'h3 ; */
/*description: .*/
#define I2C_STRETCH_CAUSE    0x00000003
#define I2C_STRETCH_CAUSE_M  ((I2C_STRETCH_CAUSE_V)<<(I2C_STRETCH_CAUSE_S))
#define I2C_STRETCH_CAUSE_V  0x3
#define I2C_STRETCH_CAUSE_S  14
/* I2C_RXFIFO_CNT : RO ;bitpos:[13:8] ;default: 6'b0 ; */
/*description: .*/
#define I2C_RXFIFO_CNT    0x0000003F
#define I2C_RXFIFO_CNT_M  ((I2C_RXFIFO_CNT_V)<<(I2C_RXFIFO_CNT_S))
#define I2C_RXFIFO_CNT_V  0x3F
#define I2C_RXFIFO_CNT_S  8
/* I2C_SLAVE_ADDRESSED : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_ADDRESSED    (BIT(5))
#define I2C_SLAVE_ADDRESSED_M  (BIT(5))
#define I2C_SLAVE_ADDRESSED_V  0x1
#define I2C_SLAVE_ADDRESSED_S  5
/* I2C_BUS_BUSY : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define I2C_BUS_BUSY    (BIT(4))
#define I2C_BUS_BUSY_M  (BIT(4))
#define I2C_BUS_BUSY_V  0x1
#define I2C_BUS_BUSY_S  4
/* I2C_ARB_LOST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ARB_LOST    (BIT(3))
#define I2C_ARB_LOST_M  (BIT(3))
#define I2C_ARB_LOST_V  0x1
#define I2C_ARB_LOST_S  3
/* I2C_SLAVE_RW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_RW    (BIT(1))
#define I2C_SLAVE_RW_M  (BIT(1))
#define I2C_SLAVE_RW_V  0x1
#define I2C_SLAVE_RW_S  1
/* I2C_RESP_REC : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RESP_REC    (BIT(0))
#define I2C_RESP_REC_M  (BIT(0))
#define I2C_RESP_REC_V  0x1
#define I2C_RESP_REC_S  0

#define I2C_TO_REG(i)          (REG_I2C_BASE(i) + 0xC)
/* I2C_TIME_OUT_EN : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TIME_OUT_EN    (BIT(5))
#define I2C_TIME_OUT_EN_M  (BIT(5))
#define I2C_TIME_OUT_EN_V  0x1
#define I2C_TIME_OUT_EN_S  5
/* I2C_TIME_OUT_REG : R/W ;bitpos:[4:0] ;default: 5'h10 ; */
/*description: .*/
#define I2C_TIME_OUT_REG    0x0000001F
#define I2C_TIME_OUT_REG_M  ((I2C_TIME_OUT_VALUE_V)<<(I2C_TIME_OUT_VALUE_S))
#define I2C_TIME_OUT_REG_V  0x1F
#define I2C_TIME_OUT_REG_S  0

#define I2C_SLAVE_ADDR_REG(i)          (REG_I2C_BASE(i) + 0x10)
/* I2C_ADDR_10BIT_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ADDR_10BIT_EN    (BIT(31))
#define I2C_ADDR_10BIT_EN_M  (BIT(31))
#define I2C_ADDR_10BIT_EN_V  0x1
#define I2C_ADDR_10BIT_EN_S  31
/* I2C_SLAVE_ADDR : R/W ;bitpos:[14:0] ;default: 15'b0 ; */
/*description: .*/
#define I2C_SLAVE_ADDR    0x00007FFF
#define I2C_SLAVE_ADDR_M  ((I2C_SLAVE_ADDR_V)<<(I2C_SLAVE_ADDR_S))
#define I2C_SLAVE_ADDR_V  0x7FFF
#define I2C_SLAVE_ADDR_S  0

#define I2C_FIFO_ST_REG(i)          (REG_I2C_BASE(i) + 0x14)
/* I2C_SLAVE_RW_POINT : RO ;bitpos:[29:22] ;default: 8'b0 ; */
/*description: .*/
#define I2C_SLAVE_RW_POINT    0x000000FF
#define I2C_SLAVE_RW_POINT_M  ((I2C_SLAVE_RW_POINT_V)<<(I2C_SLAVE_RW_POINT_S))
#define I2C_SLAVE_RW_POINT_V  0xFF
#define I2C_SLAVE_RW_POINT_S  22
/* I2C_TXFIFO_WADDR : RO ;bitpos:[19:15] ;default: 5'b0 ; */
/*description: .*/
#define I2C_TXFIFO_WADDR    0x0000001F
#define I2C_TXFIFO_WADDR_M  ((I2C_TXFIFO_WADDR_V)<<(I2C_TXFIFO_WADDR_S))
#define I2C_TXFIFO_WADDR_V  0x1F
#define I2C_TXFIFO_WADDR_S  15
/* I2C_TXFIFO_RADDR : RO ;bitpos:[14:10] ;default: 5'b0 ; */
/*description: .*/
#define I2C_TXFIFO_RADDR    0x0000001F
#define I2C_TXFIFO_RADDR_M  ((I2C_TXFIFO_RADDR_V)<<(I2C_TXFIFO_RADDR_S))
#define I2C_TXFIFO_RADDR_V  0x1F
#define I2C_TXFIFO_RADDR_S  10
/* I2C_RXFIFO_WADDR : RO ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: .*/
#define I2C_RXFIFO_WADDR    0x0000001F
#define I2C_RXFIFO_WADDR_M  ((I2C_RXFIFO_WADDR_V)<<(I2C_RXFIFO_WADDR_S))
#define I2C_RXFIFO_WADDR_V  0x1F
#define I2C_RXFIFO_WADDR_S  5
/* I2C_RXFIFO_RADDR : RO ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: .*/
#define I2C_RXFIFO_RADDR    0x0000001F
#define I2C_RXFIFO_RADDR_M  ((I2C_RXFIFO_RADDR_V)<<(I2C_RXFIFO_RADDR_S))
#define I2C_RXFIFO_RADDR_V  0x1F
#define I2C_RXFIFO_RADDR_S  0

#define I2C_FIFO_CONF_REG(i)          (REG_I2C_BASE(i) + 0x18)
/* I2C_FIFO_PRT_EN : R/W ;bitpos:[14] ;default: 1'b1 ; */
/*description: .*/
#define I2C_FIFO_PRT_EN    (BIT(14))
#define I2C_FIFO_PRT_EN_M  (BIT(14))
#define I2C_FIFO_PRT_EN_V  0x1
#define I2C_FIFO_PRT_EN_S  14
/* I2C_TX_FIFO_RST : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TX_FIFO_RST    (BIT(13))
#define I2C_TX_FIFO_RST_M  (BIT(13))
#define I2C_TX_FIFO_RST_V  0x1
#define I2C_TX_FIFO_RST_S  13
/* I2C_RX_FIFO_RST : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RX_FIFO_RST    (BIT(12))
#define I2C_RX_FIFO_RST_M  (BIT(12))
#define I2C_RX_FIFO_RST_V  0x1
#define I2C_RX_FIFO_RST_S  12
/* I2C_FIFO_ADDR_CFG_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: .*/
#define I2C_FIFO_ADDR_CFG_EN    (BIT(11))
#define I2C_FIFO_ADDR_CFG_EN_M  (BIT(11))
#define I2C_FIFO_ADDR_CFG_EN_V  0x1
#define I2C_FIFO_ADDR_CFG_EN_S  11
/* I2C_NONFIFO_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define I2C_NONFIFO_EN    (BIT(10))
#define I2C_NONFIFO_EN_M  (BIT(10))
#define I2C_NONFIFO_EN_V  0x1
#define I2C_NONFIFO_EN_S  10
/* I2C_TXFIFO_WM_THRHD : R/W ;bitpos:[9:5] ;default: 5'h4 ; */
/*description: .*/
#define I2C_TXFIFO_WM_THRHD    0x0000001F
#define I2C_TXFIFO_WM_THRHD_M  ((I2C_TXFIFO_WM_THRHD_V)<<(I2C_TXFIFO_WM_THRHD_S))
#define I2C_TXFIFO_WM_THRHD_V  0x1F
#define I2C_TXFIFO_WM_THRHD_S  5
/* I2C_RXFIFO_WM_THRHD : R/W ;bitpos:[4:0] ;default: 5'hb ; */
/*description: .*/
#define I2C_RXFIFO_WM_THRHD    0x0000001F
#define I2C_RXFIFO_WM_THRHD_M  ((I2C_RXFIFO_WM_THRHD_V)<<(I2C_RXFIFO_WM_THRHD_S))
#define I2C_RXFIFO_WM_THRHD_V  0x1F
#define I2C_RXFIFO_WM_THRHD_S  0

#define I2C_DATA_REG(i)          (REG_I2C_BASE(i) + 0x1C)
/* I2C_FIFO_RDATA : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: .*/
#define I2C_FIFO_RDATA    0x000000FF
#define I2C_FIFO_RDATA_M  ((I2C_FIFO_RDATA_V)<<(I2C_FIFO_RDATA_S))
#define I2C_FIFO_RDATA_V  0xFF
#define I2C_FIFO_RDATA_S  0

#define I2C_INT_RAW_REG(i)          (REG_I2C_BASE(i) + 0x20)
/* I2C_GENERAL_CALL_INT_RAW : R/SS/WTC ;bitpos:[17] ;default: 1'b0 ; */
/*description: .*/
#define I2C_GENERAL_CALL_INT_RAW    (BIT(17))
#define I2C_GENERAL_CALL_INT_RAW_M  (BIT(17))
#define I2C_GENERAL_CALL_INT_RAW_V  0x1
#define I2C_GENERAL_CALL_INT_RAW_S  17
/* I2C_SLAVE_STRETCH_INT_RAW : R/SS/WTC ;bitpos:[16] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_STRETCH_INT_RAW    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_RAW_M  (BIT(16))
#define I2C_SLAVE_STRETCH_INT_RAW_V  0x1
#define I2C_SLAVE_STRETCH_INT_RAW_S  16
/* I2C_DET_START_INT_RAW : R/SS/WTC ;bitpos:[15] ;default: 1'b0 ; */
/*description: .*/
#define I2C_DET_START_INT_RAW    (BIT(15))
#define I2C_DET_START_INT_RAW_M  (BIT(15))
#define I2C_DET_START_INT_RAW_V  0x1
#define I2C_DET_START_INT_RAW_S  15
/* I2C_SCL_MAIN_ST_TO_INT_RAW : R/SS/WTC ;bitpos:[14] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_MAIN_ST_TO_INT_RAW    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_RAW_M  (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_RAW_V  0x1
#define I2C_SCL_MAIN_ST_TO_INT_RAW_S  14
/* I2C_SCL_ST_TO_INT_RAW : R/SS/WTC ;bitpos:[13] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_ST_TO_INT_RAW    (BIT(13))
#define I2C_SCL_ST_TO_INT_RAW_M  (BIT(13))
#define I2C_SCL_ST_TO_INT_RAW_V  0x1
#define I2C_SCL_ST_TO_INT_RAW_S  13
/* I2C_RXFIFO_UDF_INT_RAW : R/SS/WTC ;bitpos:[12] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_UDF_INT_RAW    (BIT(12))
#define I2C_RXFIFO_UDF_INT_RAW_M  (BIT(12))
#define I2C_RXFIFO_UDF_INT_RAW_V  0x1
#define I2C_RXFIFO_UDF_INT_RAW_S  12
/* I2C_TXFIFO_OVF_INT_RAW : R/SS/WTC ;bitpos:[11] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TXFIFO_OVF_INT_RAW    (BIT(11))
#define I2C_TXFIFO_OVF_INT_RAW_M  (BIT(11))
#define I2C_TXFIFO_OVF_INT_RAW_V  0x1
#define I2C_TXFIFO_OVF_INT_RAW_S  11
/* I2C_NACK_INT_RAW : R/SS/WTC ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define I2C_NACK_INT_RAW    (BIT(10))
#define I2C_NACK_INT_RAW_M  (BIT(10))
#define I2C_NACK_INT_RAW_V  0x1
#define I2C_NACK_INT_RAW_S  10
/* I2C_TRANS_START_INT_RAW : R/SS/WTC ;bitpos:[9] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_START_INT_RAW    (BIT(9))
#define I2C_TRANS_START_INT_RAW_M  (BIT(9))
#define I2C_TRANS_START_INT_RAW_V  0x1
#define I2C_TRANS_START_INT_RAW_S  9
/* I2C_TIME_OUT_INT_RAW : R/SS/WTC ;bitpos:[8] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TIME_OUT_INT_RAW    (BIT(8))
#define I2C_TIME_OUT_INT_RAW_M  (BIT(8))
#define I2C_TIME_OUT_INT_RAW_V  0x1
#define I2C_TIME_OUT_INT_RAW_S  8
/* I2C_TRANS_COMPLETE_INT_RAW : R/SS/WTC ;bitpos:[7] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_COMPLETE_INT_RAW    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_RAW_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_RAW_V  0x1
#define I2C_TRANS_COMPLETE_INT_RAW_S  7
/* I2C_MST_TXFIFO_UDF_INT_RAW : R/SS/WTC ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define I2C_MST_TXFIFO_UDF_INT_RAW    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_RAW_M  (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_RAW_V  0x1
#define I2C_MST_TXFIFO_UDF_INT_RAW_S  6
/* I2C_ARBITRATION_LOST_INT_RAW : R/SS/WTC ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ARBITRATION_LOST_INT_RAW    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_RAW_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_RAW_V  0x1
#define I2C_ARBITRATION_LOST_INT_RAW_S  5
/* I2C_BYTE_TRANS_DONE_INT_RAW : R/SS/WTC ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define I2C_BYTE_TRANS_DONE_INT_RAW    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_RAW_M  (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_RAW_V  0x1
#define I2C_BYTE_TRANS_DONE_INT_RAW_S  4
/* I2C_END_DETECT_INT_RAW : R/SS/WTC ;bitpos:[3] ;default: 1'b0 ; */
/*description: .*/
#define I2C_END_DETECT_INT_RAW    (BIT(3))
#define I2C_END_DETECT_INT_RAW_M  (BIT(3))
#define I2C_END_DETECT_INT_RAW_V  0x1
#define I2C_END_DETECT_INT_RAW_S  3
/* I2C_RXFIFO_OVF_INT_RAW : R/SS/WTC ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_OVF_INT_RAW    (BIT(2))
#define I2C_RXFIFO_OVF_INT_RAW_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_RAW_V  0x1
#define I2C_RXFIFO_OVF_INT_RAW_S  2
/* I2C_TXFIFO_WM_INT_RAW : R/SS/WTC ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define I2C_TXFIFO_WM_INT_RAW    (BIT(1))
#define I2C_TXFIFO_WM_INT_RAW_M  (BIT(1))
#define I2C_TXFIFO_WM_INT_RAW_V  0x1
#define I2C_TXFIFO_WM_INT_RAW_S  1
/* I2C_RXFIFO_WM_INT_RAW : R/SS/WTC ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_WM_INT_RAW    (BIT(0))
#define I2C_RXFIFO_WM_INT_RAW_M  (BIT(0))
#define I2C_RXFIFO_WM_INT_RAW_V  0x1
#define I2C_RXFIFO_WM_INT_RAW_S  0

#define I2C_INT_CLR_REG(i)          (REG_I2C_BASE(i) + 0x24)
/* I2C_GENERAL_CALL_INT_CLR : WT ;bitpos:[17] ;default: 1'b0 ; */
/*description: .*/
#define I2C_GENERAL_CALL_INT_CLR    (BIT(17))
#define I2C_GENERAL_CALL_INT_CLR_M  (BIT(17))
#define I2C_GENERAL_CALL_INT_CLR_V  0x1
#define I2C_GENERAL_CALL_INT_CLR_S  17
/* I2C_SLAVE_STRETCH_INT_CLR : WT ;bitpos:[16] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_STRETCH_INT_CLR    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_CLR_M  (BIT(16))
#define I2C_SLAVE_STRETCH_INT_CLR_V  0x1
#define I2C_SLAVE_STRETCH_INT_CLR_S  16
/* I2C_DET_START_INT_CLR : WT ;bitpos:[15] ;default: 1'b0 ; */
/*description: .*/
#define I2C_DET_START_INT_CLR    (BIT(15))
#define I2C_DET_START_INT_CLR_M  (BIT(15))
#define I2C_DET_START_INT_CLR_V  0x1
#define I2C_DET_START_INT_CLR_S  15
/* I2C_SCL_MAIN_ST_TO_INT_CLR : WT ;bitpos:[14] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_MAIN_ST_TO_INT_CLR    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_CLR_M  (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_CLR_V  0x1
#define I2C_SCL_MAIN_ST_TO_INT_CLR_S  14
/* I2C_SCL_ST_TO_INT_CLR : WT ;bitpos:[13] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_ST_TO_INT_CLR    (BIT(13))
#define I2C_SCL_ST_TO_INT_CLR_M  (BIT(13))
#define I2C_SCL_ST_TO_INT_CLR_V  0x1
#define I2C_SCL_ST_TO_INT_CLR_S  13
/* I2C_RXFIFO_UDF_INT_CLR : WT ;bitpos:[12] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_UDF_INT_CLR    (BIT(12))
#define I2C_RXFIFO_UDF_INT_CLR_M  (BIT(12))
#define I2C_RXFIFO_UDF_INT_CLR_V  0x1
#define I2C_RXFIFO_UDF_INT_CLR_S  12
/* I2C_TXFIFO_OVF_INT_CLR : WT ;bitpos:[11] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TXFIFO_OVF_INT_CLR    (BIT(11))
#define I2C_TXFIFO_OVF_INT_CLR_M  (BIT(11))
#define I2C_TXFIFO_OVF_INT_CLR_V  0x1
#define I2C_TXFIFO_OVF_INT_CLR_S  11
/* I2C_NACK_INT_CLR : WT ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define I2C_NACK_INT_CLR    (BIT(10))
#define I2C_NACK_INT_CLR_M  (BIT(10))
#define I2C_NACK_INT_CLR_V  0x1
#define I2C_NACK_INT_CLR_S  10
/* I2C_TRANS_START_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_START_INT_CLR    (BIT(9))
#define I2C_TRANS_START_INT_CLR_M  (BIT(9))
#define I2C_TRANS_START_INT_CLR_V  0x1
#define I2C_TRANS_START_INT_CLR_S  9
/* I2C_TIME_OUT_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TIME_OUT_INT_CLR    (BIT(8))
#define I2C_TIME_OUT_INT_CLR_M  (BIT(8))
#define I2C_TIME_OUT_INT_CLR_V  0x1
#define I2C_TIME_OUT_INT_CLR_S  8
/* I2C_TRANS_COMPLETE_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_COMPLETE_INT_CLR    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_CLR_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_CLR_V  0x1
#define I2C_TRANS_COMPLETE_INT_CLR_S  7
/* I2C_MST_TXFIFO_UDF_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define I2C_MST_TXFIFO_UDF_INT_CLR    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_CLR_M  (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_CLR_V  0x1
#define I2C_MST_TXFIFO_UDF_INT_CLR_S  6
/* I2C_ARBITRATION_LOST_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ARBITRATION_LOST_INT_CLR    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_CLR_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_CLR_V  0x1
#define I2C_ARBITRATION_LOST_INT_CLR_S  5
/* I2C_BYTE_TRANS_DONE_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define I2C_BYTE_TRANS_DONE_INT_CLR    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_CLR_M  (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_CLR_V  0x1
#define I2C_BYTE_TRANS_DONE_INT_CLR_S  4
/* I2C_END_DETECT_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: .*/
#define I2C_END_DETECT_INT_CLR    (BIT(3))
#define I2C_END_DETECT_INT_CLR_M  (BIT(3))
#define I2C_END_DETECT_INT_CLR_V  0x1
#define I2C_END_DETECT_INT_CLR_S  3
/* I2C_RXFIFO_OVF_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_OVF_INT_CLR    (BIT(2))
#define I2C_RXFIFO_OVF_INT_CLR_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_CLR_V  0x1
#define I2C_RXFIFO_OVF_INT_CLR_S  2
/* I2C_TXFIFO_WM_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TXFIFO_WM_INT_CLR    (BIT(1))
#define I2C_TXFIFO_WM_INT_CLR_M  (BIT(1))
#define I2C_TXFIFO_WM_INT_CLR_V  0x1
#define I2C_TXFIFO_WM_INT_CLR_S  1
/* I2C_RXFIFO_WM_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_WM_INT_CLR    (BIT(0))
#define I2C_RXFIFO_WM_INT_CLR_M  (BIT(0))
#define I2C_RXFIFO_WM_INT_CLR_V  0x1
#define I2C_RXFIFO_WM_INT_CLR_S  0

#define I2C_INT_ENA_REG(i)          (REG_I2C_BASE(i) + 0x28)
/* I2C_GENERAL_CALL_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: .*/
#define I2C_GENERAL_CALL_INT_ENA    (BIT(17))
#define I2C_GENERAL_CALL_INT_ENA_M  (BIT(17))
#define I2C_GENERAL_CALL_INT_ENA_V  0x1
#define I2C_GENERAL_CALL_INT_ENA_S  17
/* I2C_SLAVE_STRETCH_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_STRETCH_INT_ENA    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_ENA_M  (BIT(16))
#define I2C_SLAVE_STRETCH_INT_ENA_V  0x1
#define I2C_SLAVE_STRETCH_INT_ENA_S  16
/* I2C_DET_START_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: .*/
#define I2C_DET_START_INT_ENA    (BIT(15))
#define I2C_DET_START_INT_ENA_M  (BIT(15))
#define I2C_DET_START_INT_ENA_V  0x1
#define I2C_DET_START_INT_ENA_S  15
/* I2C_SCL_MAIN_ST_TO_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_MAIN_ST_TO_INT_ENA    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_ENA_M  (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_ENA_V  0x1
#define I2C_SCL_MAIN_ST_TO_INT_ENA_S  14
/* I2C_SCL_ST_TO_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_ST_TO_INT_ENA    (BIT(13))
#define I2C_SCL_ST_TO_INT_ENA_M  (BIT(13))
#define I2C_SCL_ST_TO_INT_ENA_V  0x1
#define I2C_SCL_ST_TO_INT_ENA_S  13
/* I2C_RXFIFO_UDF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_UDF_INT_ENA    (BIT(12))
#define I2C_RXFIFO_UDF_INT_ENA_M  (BIT(12))
#define I2C_RXFIFO_UDF_INT_ENA_V  0x1
#define I2C_RXFIFO_UDF_INT_ENA_S  12
/* I2C_TXFIFO_OVF_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TXFIFO_OVF_INT_ENA    (BIT(11))
#define I2C_TXFIFO_OVF_INT_ENA_M  (BIT(11))
#define I2C_TXFIFO_OVF_INT_ENA_V  0x1
#define I2C_TXFIFO_OVF_INT_ENA_S  11
/* I2C_NACK_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define I2C_NACK_INT_ENA    (BIT(10))
#define I2C_NACK_INT_ENA_M  (BIT(10))
#define I2C_NACK_INT_ENA_V  0x1
#define I2C_NACK_INT_ENA_S  10
/* I2C_TRANS_START_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_START_INT_ENA    (BIT(9))
#define I2C_TRANS_START_INT_ENA_M  (BIT(9))
#define I2C_TRANS_START_INT_ENA_V  0x1
#define I2C_TRANS_START_INT_ENA_S  9
/* I2C_TIME_OUT_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TIME_OUT_INT_ENA    (BIT(8))
#define I2C_TIME_OUT_INT_ENA_M  (BIT(8))
#define I2C_TIME_OUT_INT_ENA_V  0x1
#define I2C_TIME_OUT_INT_ENA_S  8
/* I2C_TRANS_COMPLETE_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_COMPLETE_INT_ENA    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ENA_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ENA_V  0x1
#define I2C_TRANS_COMPLETE_INT_ENA_S  7
/* I2C_MST_TXFIFO_UDF_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define I2C_MST_TXFIFO_UDF_INT_ENA    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_ENA_M  (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_ENA_V  0x1
#define I2C_MST_TXFIFO_UDF_INT_ENA_S  6
/* I2C_ARBITRATION_LOST_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ARBITRATION_LOST_INT_ENA    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ENA_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ENA_V  0x1
#define I2C_ARBITRATION_LOST_INT_ENA_S  5
/* I2C_BYTE_TRANS_DONE_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define I2C_BYTE_TRANS_DONE_INT_ENA    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_ENA_M  (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_ENA_V  0x1
#define I2C_BYTE_TRANS_DONE_INT_ENA_S  4
/* I2C_END_DETECT_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: .*/
#define I2C_END_DETECT_INT_ENA    (BIT(3))
#define I2C_END_DETECT_INT_ENA_M  (BIT(3))
#define I2C_END_DETECT_INT_ENA_V  0x1
#define I2C_END_DETECT_INT_ENA_S  3
/* I2C_RXFIFO_OVF_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_OVF_INT_ENA    (BIT(2))
#define I2C_RXFIFO_OVF_INT_ENA_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_ENA_V  0x1
#define I2C_RXFIFO_OVF_INT_ENA_S  2
/* I2C_TXFIFO_WM_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TXFIFO_WM_INT_ENA    (BIT(1))
#define I2C_TXFIFO_WM_INT_ENA_M  (BIT(1))
#define I2C_TXFIFO_WM_INT_ENA_V  0x1
#define I2C_TXFIFO_WM_INT_ENA_S  1
/* I2C_RXFIFO_WM_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_WM_INT_ENA    (BIT(0))
#define I2C_RXFIFO_WM_INT_ENA_M  (BIT(0))
#define I2C_RXFIFO_WM_INT_ENA_V  0x1
#define I2C_RXFIFO_WM_INT_ENA_S  0

#define I2C_INT_STATUS_REG(i)          (REG_I2C_BASE(i) + 0x2C)
/* I2C_GENERAL_CALL_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: .*/
#define I2C_GENERAL_CALL_INT_ST    (BIT(17))
#define I2C_GENERAL_CALL_INT_ST_M  (BIT(17))
#define I2C_GENERAL_CALL_INT_ST_V  0x1
#define I2C_GENERAL_CALL_INT_ST_S  17
/* I2C_SLAVE_STRETCH_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_STRETCH_INT_ST    (BIT(16))
#define I2C_SLAVE_STRETCH_INT_ST_M  (BIT(16))
#define I2C_SLAVE_STRETCH_INT_ST_V  0x1
#define I2C_SLAVE_STRETCH_INT_ST_S  16
/* I2C_DET_START_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: .*/
#define I2C_DET_START_INT_ST    (BIT(15))
#define I2C_DET_START_INT_ST_M  (BIT(15))
#define I2C_DET_START_INT_ST_V  0x1
#define I2C_DET_START_INT_ST_S  15
/* I2C_SCL_MAIN_ST_TO_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_MAIN_ST_TO_INT_ST    (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_ST_M  (BIT(14))
#define I2C_SCL_MAIN_ST_TO_INT_ST_V  0x1
#define I2C_SCL_MAIN_ST_TO_INT_ST_S  14
/* I2C_SCL_ST_TO_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_ST_TO_INT_ST    (BIT(13))
#define I2C_SCL_ST_TO_INT_ST_M  (BIT(13))
#define I2C_SCL_ST_TO_INT_ST_V  0x1
#define I2C_SCL_ST_TO_INT_ST_S  13
/* I2C_RXFIFO_UDF_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_UDF_INT_ST    (BIT(12))
#define I2C_RXFIFO_UDF_INT_ST_M  (BIT(12))
#define I2C_RXFIFO_UDF_INT_ST_V  0x1
#define I2C_RXFIFO_UDF_INT_ST_S  12
/* I2C_TXFIFO_OVF_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TXFIFO_OVF_INT_ST    (BIT(11))
#define I2C_TXFIFO_OVF_INT_ST_M  (BIT(11))
#define I2C_TXFIFO_OVF_INT_ST_V  0x1
#define I2C_TXFIFO_OVF_INT_ST_S  11
/* I2C_NACK_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define I2C_NACK_INT_ST    (BIT(10))
#define I2C_NACK_INT_ST_M  (BIT(10))
#define I2C_NACK_INT_ST_V  0x1
#define I2C_NACK_INT_ST_S  10
/* I2C_TRANS_START_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_START_INT_ST    (BIT(9))
#define I2C_TRANS_START_INT_ST_M  (BIT(9))
#define I2C_TRANS_START_INT_ST_V  0x1
#define I2C_TRANS_START_INT_ST_S  9
/* I2C_TIME_OUT_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TIME_OUT_INT_ST    (BIT(8))
#define I2C_TIME_OUT_INT_ST_M  (BIT(8))
#define I2C_TIME_OUT_INT_ST_V  0x1
#define I2C_TIME_OUT_INT_ST_S  8
/* I2C_TRANS_COMPLETE_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TRANS_COMPLETE_INT_ST    (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ST_M  (BIT(7))
#define I2C_TRANS_COMPLETE_INT_ST_V  0x1
#define I2C_TRANS_COMPLETE_INT_ST_S  7
/* I2C_MST_TXFIFO_UDF_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define I2C_MST_TXFIFO_UDF_INT_ST    (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_ST_M  (BIT(6))
#define I2C_MST_TXFIFO_UDF_INT_ST_V  0x1
#define I2C_MST_TXFIFO_UDF_INT_ST_S  6
/* I2C_ARBITRATION_LOST_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define I2C_ARBITRATION_LOST_INT_ST    (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ST_M  (BIT(5))
#define I2C_ARBITRATION_LOST_INT_ST_V  0x1
#define I2C_ARBITRATION_LOST_INT_ST_S  5
/* I2C_BYTE_TRANS_DONE_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define I2C_BYTE_TRANS_DONE_INT_ST    (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_ST_M  (BIT(4))
#define I2C_BYTE_TRANS_DONE_INT_ST_V  0x1
#define I2C_BYTE_TRANS_DONE_INT_ST_S  4
/* I2C_END_DETECT_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: .*/
#define I2C_END_DETECT_INT_ST    (BIT(3))
#define I2C_END_DETECT_INT_ST_M  (BIT(3))
#define I2C_END_DETECT_INT_ST_V  0x1
#define I2C_END_DETECT_INT_ST_S  3
/* I2C_RXFIFO_OVF_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_OVF_INT_ST    (BIT(2))
#define I2C_RXFIFO_OVF_INT_ST_M  (BIT(2))
#define I2C_RXFIFO_OVF_INT_ST_V  0x1
#define I2C_RXFIFO_OVF_INT_ST_S  2
/* I2C_TXFIFO_WM_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: .*/
#define I2C_TXFIFO_WM_INT_ST    (BIT(1))
#define I2C_TXFIFO_WM_INT_ST_M  (BIT(1))
#define I2C_TXFIFO_WM_INT_ST_V  0x1
#define I2C_TXFIFO_WM_INT_ST_S  1
/* I2C_RXFIFO_WM_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define I2C_RXFIFO_WM_INT_ST    (BIT(0))
#define I2C_RXFIFO_WM_INT_ST_M  (BIT(0))
#define I2C_RXFIFO_WM_INT_ST_V  0x1
#define I2C_RXFIFO_WM_INT_ST_S  0

#define I2C_SDA_HOLD_REG(i)          (REG_I2C_BASE(i) + 0x30)
/* I2C_SDA_HOLD_TIME : R/W ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: .*/
#define I2C_SDA_HOLD_TIME    0x000001FF
#define I2C_SDA_HOLD_TIME_M  ((I2C_SDA_HOLD_TIME_V)<<(I2C_SDA_HOLD_TIME_S))
#define I2C_SDA_HOLD_TIME_V  0x1FF
#define I2C_SDA_HOLD_TIME_S  0

#define I2C_SDA_SAMPLE_REG(i)          (REG_I2C_BASE(i) + 0x34)
/* I2C_SDA_SAMPLE_TIME : R/W ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: .*/
#define I2C_SDA_SAMPLE_TIME    0x000001FF
#define I2C_SDA_SAMPLE_TIME_M  ((I2C_SDA_SAMPLE_TIME_V)<<(I2C_SDA_SAMPLE_TIME_S))
#define I2C_SDA_SAMPLE_TIME_V  0x1FF
#define I2C_SDA_SAMPLE_TIME_S  0

#define I2C_SCL_HIGH_PERIOD_REG(i)          (REG_I2C_BASE(i) + 0x38)
/* I2C_SCL_WAIT_HIGH_PERIOD : R/W ;bitpos:[15:9] ;default: 7'b0 ; */
/*description: .*/
#define I2C_SCL_WAIT_HIGH_PERIOD    0x0000007F
#define I2C_SCL_WAIT_HIGH_PERIOD_M  ((I2C_SCL_WAIT_HIGH_PERIOD_V)<<(I2C_SCL_WAIT_HIGH_PERIOD_S))
#define I2C_SCL_WAIT_HIGH_PERIOD_V  0x7F
#define I2C_SCL_WAIT_HIGH_PERIOD_S  9
/* I2C_SCL_HIGH_PERIOD : R/W ;bitpos:[8:0] ;default: 9'b0 ; */
/*description: .*/
#define I2C_SCL_HIGH_PERIOD    0x000001FF
#define I2C_SCL_HIGH_PERIOD_M  ((I2C_SCL_HIGH_PERIOD_V)<<(I2C_SCL_HIGH_PERIOD_S))
#define I2C_SCL_HIGH_PERIOD_V  0x1FF
#define I2C_SCL_HIGH_PERIOD_S  0

#define I2C_SCL_START_HOLD_REG(i)          (REG_I2C_BASE(i) + 0x40)
/* I2C_SCL_START_HOLD_TIME : R/W ;bitpos:[8:0] ;default: 9'b1000 ; */
/*description: .*/
#define I2C_SCL_START_HOLD_TIME    0x000001FF
#define I2C_SCL_START_HOLD_TIME_M  ((I2C_SCL_START_HOLD_TIME_V)<<(I2C_SCL_START_HOLD_TIME_S))
#define I2C_SCL_START_HOLD_TIME_V  0x1FF
#define I2C_SCL_START_HOLD_TIME_S  0

#define I2C_SCL_RSTART_SETUP_REG(i)          (REG_I2C_BASE(i) + 0x44)
/* I2C_SCL_RSTART_SETUP_TIME : R/W ;bitpos:[8:0] ;default: 9'b1000 ; */
/*description: .*/
#define I2C_SCL_RSTART_SETUP_TIME    0x000001FF
#define I2C_SCL_RSTART_SETUP_TIME_M  ((I2C_SCL_RSTART_SETUP_TIME_V)<<(I2C_SCL_RSTART_SETUP_TIME_S))
#define I2C_SCL_RSTART_SETUP_TIME_V  0x1FF
#define I2C_SCL_RSTART_SETUP_TIME_S  0

#define I2C_SCL_STOP_HOLD_REG(i)          (REG_I2C_BASE(i) + 0x48)
/* I2C_SCL_STOP_HOLD_TIME : R/W ;bitpos:[8:0] ;default: 9'b1000 ; */
/*description: .*/
#define I2C_SCL_STOP_HOLD_TIME    0x000001FF
#define I2C_SCL_STOP_HOLD_TIME_M  ((I2C_SCL_STOP_HOLD_TIME_V)<<(I2C_SCL_STOP_HOLD_TIME_S))
#define I2C_SCL_STOP_HOLD_TIME_V  0x1FF
#define I2C_SCL_STOP_HOLD_TIME_S  0

#define I2C_SCL_STOP_SETUP_REG(i)          (REG_I2C_BASE(i) + 0x4C)
/* I2C_SCL_STOP_SETUP_TIME : R/W ;bitpos:[8:0] ;default: 9'b1000 ; */
/*description: .*/
#define I2C_SCL_STOP_SETUP_TIME    0x000001FF
#define I2C_SCL_STOP_SETUP_TIME_M  ((I2C_SCL_STOP_SETUP_TIME_V)<<(I2C_SCL_STOP_SETUP_TIME_S))
#define I2C_SCL_STOP_SETUP_TIME_V  0x1FF
#define I2C_SCL_STOP_SETUP_TIME_S  0

#define I2C_FILTER_CFG_REG(i)          (REG_I2C_BASE(i) + 0x50)
/* I2C_SDA_FILTER_EN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: .*/
#define I2C_SDA_FILTER_EN    (BIT(9))
#define I2C_SDA_FILTER_EN_M  (BIT(9))
#define I2C_SDA_FILTER_EN_V  0x1
#define I2C_SDA_FILTER_EN_S  9
/* I2C_SCL_FILTER_EN : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: .*/
#define I2C_SCL_FILTER_EN    (BIT(8))
#define I2C_SCL_FILTER_EN_M  (BIT(8))
#define I2C_SCL_FILTER_EN_V  0x1
#define I2C_SCL_FILTER_EN_S  8
/* I2C_SDA_FILTER_THRES : R/W ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: .*/
#define I2C_SDA_FILTER_THRES    0x0000000F
#define I2C_SDA_FILTER_THRES_M  ((I2C_SDA_FILTER_THRES_V)<<(I2C_SDA_FILTER_THRES_S))
#define I2C_SDA_FILTER_THRES_V  0xF
#define I2C_SDA_FILTER_THRES_S  4
/* I2C_SCL_FILTER_THRES : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: .*/
#define I2C_SCL_FILTER_THRES    0x0000000F
#define I2C_SCL_FILTER_THRES_M  ((I2C_SCL_FILTER_THRES_V)<<(I2C_SCL_FILTER_THRES_S))
#define I2C_SCL_FILTER_THRES_V  0xF
#define I2C_SCL_FILTER_THRES_S  0

#define I2C_CLK_CONF_REG(i)          (REG_I2C_BASE(i) + 0x54)
/* I2C_SCLK_ACTIVE : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: .*/
#define I2C_SCLK_ACTIVE    (BIT(21))
#define I2C_SCLK_ACTIVE_M  (BIT(21))
#define I2C_SCLK_ACTIVE_V  0x1
#define I2C_SCLK_ACTIVE_S  21
/* I2C_SCLK_SEL : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCLK_SEL    (BIT(20))
#define I2C_SCLK_SEL_M  (BIT(20))
#define I2C_SCLK_SEL_V  0x1
#define I2C_SCLK_SEL_S  20
/* I2C_SCLK_DIV_B : R/W ;bitpos:[19:14] ;default: 6'b0 ; */
/*description: .*/
#define I2C_SCLK_DIV_B    0x0000003F
#define I2C_SCLK_DIV_B_M  ((I2C_SCLK_DIV_B_V)<<(I2C_SCLK_DIV_B_S))
#define I2C_SCLK_DIV_B_V  0x3F
#define I2C_SCLK_DIV_B_S  14
/* I2C_SCLK_DIV_A : R/W ;bitpos:[13:8] ;default: 6'b0 ; */
/*description: .*/
#define I2C_SCLK_DIV_A    0x0000003F
#define I2C_SCLK_DIV_A_M  ((I2C_SCLK_DIV_A_V)<<(I2C_SCLK_DIV_A_S))
#define I2C_SCLK_DIV_A_V  0x3F
#define I2C_SCLK_DIV_A_S  8
/* I2C_SCLK_DIV_NUM : R/W ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: .*/
#define I2C_SCLK_DIV_NUM    0x000000FF
#define I2C_SCLK_DIV_NUM_M  ((I2C_SCLK_DIV_NUM_V)<<(I2C_SCLK_DIV_NUM_S))
#define I2C_SCLK_DIV_NUM_V  0xFF
#define I2C_SCLK_DIV_NUM_S  0

#define I2C_COMD0_REG(i)          (REG_I2C_BASE(i) + 0x58)
/* I2C_COMMAND0_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND0_DONE    (BIT(31))
#define I2C_COMMAND0_DONE_M  (BIT(31))
#define I2C_COMMAND0_DONE_V  0x1
#define I2C_COMMAND0_DONE_S  31
/* I2C_COMMAND0 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND0    0x00003FFF
#define I2C_COMMAND0_M  ((I2C_COMMAND0_V)<<(I2C_COMMAND0_S))
#define I2C_COMMAND0_V  0x3FFF
#define I2C_COMMAND0_S  0

#define I2C_COMD1_REG(i)          (REG_I2C_BASE(i) + 0x5C)
/* I2C_COMMAND1_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND1_DONE    (BIT(31))
#define I2C_COMMAND1_DONE_M  (BIT(31))
#define I2C_COMMAND1_DONE_V  0x1
#define I2C_COMMAND1_DONE_S  31
/* I2C_COMMAND1 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND1    0x00003FFF
#define I2C_COMMAND1_M  ((I2C_COMMAND1_V)<<(I2C_COMMAND1_S))
#define I2C_COMMAND1_V  0x3FFF
#define I2C_COMMAND1_S  0

#define I2C_COMD2_REG(i)          (REG_I2C_BASE(i) + 0x60)
/* I2C_COMMAND2_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND2_DONE    (BIT(31))
#define I2C_COMMAND2_DONE_M  (BIT(31))
#define I2C_COMMAND2_DONE_V  0x1
#define I2C_COMMAND2_DONE_S  31
/* I2C_COMMAND2 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND2    0x00003FFF
#define I2C_COMMAND2_M  ((I2C_COMMAND2_V)<<(I2C_COMMAND2_S))
#define I2C_COMMAND2_V  0x3FFF
#define I2C_COMMAND2_S  0

#define I2C_COMD3_REG(i)          (REG_I2C_BASE(i) + 0x64)
/* I2C_COMMAND3_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND3_DONE    (BIT(31))
#define I2C_COMMAND3_DONE_M  (BIT(31))
#define I2C_COMMAND3_DONE_V  0x1
#define I2C_COMMAND3_DONE_S  31
/* I2C_COMMAND3 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND3    0x00003FFF
#define I2C_COMMAND3_M  ((I2C_COMMAND3_V)<<(I2C_COMMAND3_S))
#define I2C_COMMAND3_V  0x3FFF
#define I2C_COMMAND3_S  0

#define I2C_COMD4_REG(i)          (REG_I2C_BASE(i) + 0x68)
/* I2C_COMMAND4_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND4_DONE    (BIT(31))
#define I2C_COMMAND4_DONE_M  (BIT(31))
#define I2C_COMMAND4_DONE_V  0x1
#define I2C_COMMAND4_DONE_S  31
/* I2C_COMMAND4 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND4    0x00003FFF
#define I2C_COMMAND4_M  ((I2C_COMMAND4_V)<<(I2C_COMMAND4_S))
#define I2C_COMMAND4_V  0x3FFF
#define I2C_COMMAND4_S  0

#define I2C_COMD5_REG(i)          (REG_I2C_BASE(i) + 0x6C)
/* I2C_COMMAND5_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND5_DONE    (BIT(31))
#define I2C_COMMAND5_DONE_M  (BIT(31))
#define I2C_COMMAND5_DONE_V  0x1
#define I2C_COMMAND5_DONE_S  31
/* I2C_COMMAND5 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND5    0x00003FFF
#define I2C_COMMAND5_M  ((I2C_COMMAND5_V)<<(I2C_COMMAND5_S))
#define I2C_COMMAND5_V  0x3FFF
#define I2C_COMMAND5_S  0

#define I2C_COMD6_REG(i)          (REG_I2C_BASE(i) + 0x70)
/* I2C_COMMAND6_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND6_DONE    (BIT(31))
#define I2C_COMMAND6_DONE_M  (BIT(31))
#define I2C_COMMAND6_DONE_V  0x1
#define I2C_COMMAND6_DONE_S  31
/* I2C_COMMAND6 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND6    0x00003FFF
#define I2C_COMMAND6_M  ((I2C_COMMAND6_V)<<(I2C_COMMAND6_S))
#define I2C_COMMAND6_V  0x3FFF
#define I2C_COMMAND6_S  0

#define I2C_COMD7_REG(i)          (REG_I2C_BASE(i) + 0x74)
/* I2C_COMMAND7_DONE : R/W/SS ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define I2C_COMMAND7_DONE    (BIT(31))
#define I2C_COMMAND7_DONE_M  (BIT(31))
#define I2C_COMMAND7_DONE_V  0x1
#define I2C_COMMAND7_DONE_S  31
/* I2C_COMMAND7 : R/W ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: .*/
#define I2C_COMMAND7    0x00003FFF
#define I2C_COMMAND7_M  ((I2C_COMMAND7_V)<<(I2C_COMMAND7_S))
#define I2C_COMMAND7_V  0x3FFF
#define I2C_COMMAND7_S  0

#define I2C_SCL_ST_TIME_OUT_REG(i)          (REG_I2C_BASE(i) + 0x78)
/* I2C_SCL_ST_TO_REG : R/W ;bitpos:[4:0] ;default: 5'h10 ; */
/*description: no more than 23.*/
#define I2C_SCL_ST_TO_REG    0x0000001F
#define I2C_SCL_ST_TO_REG_M  ((I2C_SCL_ST_TO_REG_V)<<(I2C_SCL_ST_TO_REG_S))
#define I2C_SCL_ST_TO_REG_V  0x1F
#define I2C_SCL_ST_TO_REG_S  0

#define I2C_SCL_MAIN_ST_TIME_OUT_REG(i)          (REG_I2C_BASE(i) + 0x7C)
/* I2C_SCL_MAIN_ST_TO_REG : R/W ;bitpos:[4:0] ;default: 5'h10 ; */
/*description: no more than 23.*/
#define I2C_SCL_MAIN_ST_TO_REG    0x0000001F
#define I2C_SCL_MAIN_ST_TO_REG_M  ((I2C_SCL_MAIN_ST_TO_REG_V)<<(I2C_SCL_MAIN_ST_TO_REG_S))
#define I2C_SCL_MAIN_ST_TO_REG_V  0x1F
#define I2C_SCL_MAIN_ST_TO_REG_S  0

#define I2C_SCL_SP_CONF_REG(i)          (REG_I2C_BASE(i) + 0x80)
/* I2C_SDA_PD_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SDA_PD_EN    (BIT(7))
#define I2C_SDA_PD_EN_M  (BIT(7))
#define I2C_SDA_PD_EN_V  0x1
#define I2C_SDA_PD_EN_S  7
/* I2C_SCL_PD_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_PD_EN    (BIT(6))
#define I2C_SCL_PD_EN_M  (BIT(6))
#define I2C_SCL_PD_EN_V  0x1
#define I2C_SCL_PD_EN_S  6
/* I2C_SCL_RST_SLV_NUM : R/W ;bitpos:[5:1] ;default: 5'b0 ; */
/*description: .*/
#define I2C_SCL_RST_SLV_NUM    0x0000001F
#define I2C_SCL_RST_SLV_NUM_M  ((I2C_SCL_RST_SLV_NUM_V)<<(I2C_SCL_RST_SLV_NUM_S))
#define I2C_SCL_RST_SLV_NUM_V  0x1F
#define I2C_SCL_RST_SLV_NUM_S  1
/* I2C_SCL_RST_SLV_EN : R/W/SC ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SCL_RST_SLV_EN    (BIT(0))
#define I2C_SCL_RST_SLV_EN_M  (BIT(0))
#define I2C_SCL_RST_SLV_EN_V  0x1
#define I2C_SCL_RST_SLV_EN_S  0

#define I2C_SCL_STRETCH_CONF_REG(i)          (REG_I2C_BASE(i) + 0x84)
/* I2C_SLAVE_BYTE_ACK_LVL : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_BYTE_ACK_LVL    (BIT(13))
#define I2C_SLAVE_BYTE_ACK_LVL_M  (BIT(13))
#define I2C_SLAVE_BYTE_ACK_LVL_V  0x1
#define I2C_SLAVE_BYTE_ACK_LVL_S  13
/* I2C_SLAVE_BYTE_ACK_CTL_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_BYTE_ACK_CTL_EN    (BIT(12))
#define I2C_SLAVE_BYTE_ACK_CTL_EN_M  (BIT(12))
#define I2C_SLAVE_BYTE_ACK_CTL_EN_V  0x1
#define I2C_SLAVE_BYTE_ACK_CTL_EN_S  12
/* I2C_SLAVE_SCL_STRETCH_CLR : WT ;bitpos:[11] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_SCL_STRETCH_CLR    (BIT(11))
#define I2C_SLAVE_SCL_STRETCH_CLR_M  (BIT(11))
#define I2C_SLAVE_SCL_STRETCH_CLR_V  0x1
#define I2C_SLAVE_SCL_STRETCH_CLR_S  11
/* I2C_SLAVE_SCL_STRETCH_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define I2C_SLAVE_SCL_STRETCH_EN    (BIT(10))
#define I2C_SLAVE_SCL_STRETCH_EN_M  (BIT(10))
#define I2C_SLAVE_SCL_STRETCH_EN_V  0x1
#define I2C_SLAVE_SCL_STRETCH_EN_S  10
/* I2C_STRETCH_PROTECT_NUM : R/W ;bitpos:[9:0] ;default: 10'b0 ; */
/*description: .*/
#define I2C_STRETCH_PROTECT_NUM    0x000003FF
#define I2C_STRETCH_PROTECT_NUM_M  ((I2C_STRETCH_PROTECT_NUM_V)<<(I2C_STRETCH_PROTECT_NUM_S))
#define I2C_STRETCH_PROTECT_NUM_V  0x3FF
#define I2C_STRETCH_PROTECT_NUM_S  0

#define I2C_DATE_REG(i)          (REG_I2C_BASE(i) + 0xF8)
/* I2C_DATE : R/W ;bitpos:[31:0] ;default: 32'h20070201 ; */
/*description: .*/
#define I2C_DATE    0xFFFFFFFF
#define I2C_DATE_M  ((I2C_DATE_V)<<(I2C_DATE_S))
#define I2C_DATE_V  0xFFFFFFFF
#define I2C_DATE_S  0

#define I2C_TXFIFO_START_ADDR_REG(i)          (REG_I2C_BASE(i) + 0x100)

#define I2C_RXFIFO_START_ADDR_REG(i)          (REG_I2C_BASE(i) + 0x180)


#ifdef __cplusplus
}
#endif



#endif /*_SOC_I2C_REG_H_ */
