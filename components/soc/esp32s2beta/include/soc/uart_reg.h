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
#ifndef _SOC_UART_REG_H_
#define _SOC_UART_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define UART_FIFO_REG(i)          (REG_UART_BASE(i) + 0x0)
/* UART_RXFIFO_RD_BYTE : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: */
#define UART_RXFIFO_RD_BYTE  0x000000FF
#define UART_RXFIFO_RD_BYTE_M  ((UART_RXFIFO_RD_BYTE_V)<<(UART_RXFIFO_RD_BYTE_S))
#define UART_RXFIFO_RD_BYTE_V  0xFF
#define UART_RXFIFO_RD_BYTE_S  0

#define UART_INT_RAW_REG(i)          (REG_UART_BASE(i) + 0x4)
/* UART_WAKEUP_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define UART_WAKEUP_INT_RAW  (BIT(19))
#define UART_WAKEUP_INT_RAW_M  (BIT(19))
#define UART_WAKEUP_INT_RAW_V  0x1
#define UART_WAKEUP_INT_RAW_S  19
/* UART_AT_CMD_CHAR_DET_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define UART_AT_CMD_CHAR_DET_INT_RAW  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_RAW_M  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_RAW_V  0x1
#define UART_AT_CMD_CHAR_DET_INT_RAW_S  18
/* UART_RS485_CLASH_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_CLASH_INT_RAW  (BIT(17))
#define UART_RS485_CLASH_INT_RAW_M  (BIT(17))
#define UART_RS485_CLASH_INT_RAW_V  0x1
#define UART_RS485_CLASH_INT_RAW_S  17
/* UART_RS485_FRM_ERR_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_FRM_ERR_INT_RAW  (BIT(16))
#define UART_RS485_FRM_ERR_INT_RAW_M  (BIT(16))
#define UART_RS485_FRM_ERR_INT_RAW_V  0x1
#define UART_RS485_FRM_ERR_INT_RAW_S  16
/* UART_RS485_PARITY_ERR_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_PARITY_ERR_INT_RAW  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_RAW_M  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_RAW_V  0x1
#define UART_RS485_PARITY_ERR_INT_RAW_S  15
/* UART_TX_DONE_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UART_TX_DONE_INT_RAW  (BIT(14))
#define UART_TX_DONE_INT_RAW_M  (BIT(14))
#define UART_TX_DONE_INT_RAW_V  0x1
#define UART_TX_DONE_INT_RAW_S  14
/* UART_TX_BRK_IDLE_DONE_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_IDLE_DONE_INT_RAW  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_RAW_M  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_RAW_V  0x1
#define UART_TX_BRK_IDLE_DONE_INT_RAW_S  13
/* UART_TX_BRK_DONE_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_DONE_INT_RAW  (BIT(12))
#define UART_TX_BRK_DONE_INT_RAW_M  (BIT(12))
#define UART_TX_BRK_DONE_INT_RAW_V  0x1
#define UART_TX_BRK_DONE_INT_RAW_S  12
/* UART_GLITCH_DET_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UART_GLITCH_DET_INT_RAW  (BIT(11))
#define UART_GLITCH_DET_INT_RAW_M  (BIT(11))
#define UART_GLITCH_DET_INT_RAW_V  0x1
#define UART_GLITCH_DET_INT_RAW_S  11
/* UART_SW_XOFF_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XOFF_INT_RAW  (BIT(10))
#define UART_SW_XOFF_INT_RAW_M  (BIT(10))
#define UART_SW_XOFF_INT_RAW_V  0x1
#define UART_SW_XOFF_INT_RAW_S  10
/* UART_SW_XON_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XON_INT_RAW  (BIT(9))
#define UART_SW_XON_INT_RAW_M  (BIT(9))
#define UART_SW_XON_INT_RAW_V  0x1
#define UART_SW_XON_INT_RAW_S  9
/* UART_RXFIFO_TOUT_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_TOUT_INT_RAW  (BIT(8))
#define UART_RXFIFO_TOUT_INT_RAW_M  (BIT(8))
#define UART_RXFIFO_TOUT_INT_RAW_V  0x1
#define UART_RXFIFO_TOUT_INT_RAW_S  8
/* UART_BRK_DET_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UART_BRK_DET_INT_RAW  (BIT(7))
#define UART_BRK_DET_INT_RAW_M  (BIT(7))
#define UART_BRK_DET_INT_RAW_V  0x1
#define UART_BRK_DET_INT_RAW_S  7
/* UART_CTS_CHG_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UART_CTS_CHG_INT_RAW  (BIT(6))
#define UART_CTS_CHG_INT_RAW_M  (BIT(6))
#define UART_CTS_CHG_INT_RAW_V  0x1
#define UART_CTS_CHG_INT_RAW_S  6
/* UART_DSR_CHG_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UART_DSR_CHG_INT_RAW  (BIT(5))
#define UART_DSR_CHG_INT_RAW_M  (BIT(5))
#define UART_DSR_CHG_INT_RAW_V  0x1
#define UART_DSR_CHG_INT_RAW_S  5
/* UART_RXFIFO_OVF_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_OVF_INT_RAW  (BIT(4))
#define UART_RXFIFO_OVF_INT_RAW_M  (BIT(4))
#define UART_RXFIFO_OVF_INT_RAW_V  0x1
#define UART_RXFIFO_OVF_INT_RAW_S  4
/* UART_FRM_ERR_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UART_FRM_ERR_INT_RAW  (BIT(3))
#define UART_FRM_ERR_INT_RAW_M  (BIT(3))
#define UART_FRM_ERR_INT_RAW_V  0x1
#define UART_FRM_ERR_INT_RAW_S  3
/* UART_PARITY_ERR_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UART_PARITY_ERR_INT_RAW  (BIT(2))
#define UART_PARITY_ERR_INT_RAW_M  (BIT(2))
#define UART_PARITY_ERR_INT_RAW_V  0x1
#define UART_PARITY_ERR_INT_RAW_S  2
/* UART_TXFIFO_EMPTY_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UART_TXFIFO_EMPTY_INT_RAW  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_RAW_M  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_RAW_V  0x1
#define UART_TXFIFO_EMPTY_INT_RAW_S  1
/* UART_RXFIFO_FULL_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_FULL_INT_RAW  (BIT(0))
#define UART_RXFIFO_FULL_INT_RAW_M  (BIT(0))
#define UART_RXFIFO_FULL_INT_RAW_V  0x1
#define UART_RXFIFO_FULL_INT_RAW_S  0

#define UART_INT_ST_REG(i)          (REG_UART_BASE(i) + 0x8)
/* UART_WAKEUP_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define UART_WAKEUP_INT_ST  (BIT(19))
#define UART_WAKEUP_INT_ST_M  (BIT(19))
#define UART_WAKEUP_INT_ST_V  0x1
#define UART_WAKEUP_INT_ST_S  19
/* UART_AT_CMD_CHAR_DET_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define UART_AT_CMD_CHAR_DET_INT_ST  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_ST_M  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_ST_V  0x1
#define UART_AT_CMD_CHAR_DET_INT_ST_S  18
/* UART_RS485_CLASH_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_CLASH_INT_ST  (BIT(17))
#define UART_RS485_CLASH_INT_ST_M  (BIT(17))
#define UART_RS485_CLASH_INT_ST_V  0x1
#define UART_RS485_CLASH_INT_ST_S  17
/* UART_RS485_FRM_ERR_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_FRM_ERR_INT_ST  (BIT(16))
#define UART_RS485_FRM_ERR_INT_ST_M  (BIT(16))
#define UART_RS485_FRM_ERR_INT_ST_V  0x1
#define UART_RS485_FRM_ERR_INT_ST_S  16
/* UART_RS485_PARITY_ERR_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_PARITY_ERR_INT_ST  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_ST_M  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_ST_V  0x1
#define UART_RS485_PARITY_ERR_INT_ST_S  15
/* UART_TX_DONE_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UART_TX_DONE_INT_ST  (BIT(14))
#define UART_TX_DONE_INT_ST_M  (BIT(14))
#define UART_TX_DONE_INT_ST_V  0x1
#define UART_TX_DONE_INT_ST_S  14
/* UART_TX_BRK_IDLE_DONE_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_IDLE_DONE_INT_ST  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_ST_M  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_ST_V  0x1
#define UART_TX_BRK_IDLE_DONE_INT_ST_S  13
/* UART_TX_BRK_DONE_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_DONE_INT_ST  (BIT(12))
#define UART_TX_BRK_DONE_INT_ST_M  (BIT(12))
#define UART_TX_BRK_DONE_INT_ST_V  0x1
#define UART_TX_BRK_DONE_INT_ST_S  12
/* UART_GLITCH_DET_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UART_GLITCH_DET_INT_ST  (BIT(11))
#define UART_GLITCH_DET_INT_ST_M  (BIT(11))
#define UART_GLITCH_DET_INT_ST_V  0x1
#define UART_GLITCH_DET_INT_ST_S  11
/* UART_SW_XOFF_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XOFF_INT_ST  (BIT(10))
#define UART_SW_XOFF_INT_ST_M  (BIT(10))
#define UART_SW_XOFF_INT_ST_V  0x1
#define UART_SW_XOFF_INT_ST_S  10
/* UART_SW_XON_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XON_INT_ST  (BIT(9))
#define UART_SW_XON_INT_ST_M  (BIT(9))
#define UART_SW_XON_INT_ST_V  0x1
#define UART_SW_XON_INT_ST_S  9
/* UART_RXFIFO_TOUT_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_TOUT_INT_ST  (BIT(8))
#define UART_RXFIFO_TOUT_INT_ST_M  (BIT(8))
#define UART_RXFIFO_TOUT_INT_ST_V  0x1
#define UART_RXFIFO_TOUT_INT_ST_S  8
/* UART_BRK_DET_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UART_BRK_DET_INT_ST  (BIT(7))
#define UART_BRK_DET_INT_ST_M  (BIT(7))
#define UART_BRK_DET_INT_ST_V  0x1
#define UART_BRK_DET_INT_ST_S  7
/* UART_CTS_CHG_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UART_CTS_CHG_INT_ST  (BIT(6))
#define UART_CTS_CHG_INT_ST_M  (BIT(6))
#define UART_CTS_CHG_INT_ST_V  0x1
#define UART_CTS_CHG_INT_ST_S  6
/* UART_DSR_CHG_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UART_DSR_CHG_INT_ST  (BIT(5))
#define UART_DSR_CHG_INT_ST_M  (BIT(5))
#define UART_DSR_CHG_INT_ST_V  0x1
#define UART_DSR_CHG_INT_ST_S  5
/* UART_RXFIFO_OVF_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_OVF_INT_ST  (BIT(4))
#define UART_RXFIFO_OVF_INT_ST_M  (BIT(4))
#define UART_RXFIFO_OVF_INT_ST_V  0x1
#define UART_RXFIFO_OVF_INT_ST_S  4
/* UART_FRM_ERR_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UART_FRM_ERR_INT_ST  (BIT(3))
#define UART_FRM_ERR_INT_ST_M  (BIT(3))
#define UART_FRM_ERR_INT_ST_V  0x1
#define UART_FRM_ERR_INT_ST_S  3
/* UART_PARITY_ERR_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UART_PARITY_ERR_INT_ST  (BIT(2))
#define UART_PARITY_ERR_INT_ST_M  (BIT(2))
#define UART_PARITY_ERR_INT_ST_V  0x1
#define UART_PARITY_ERR_INT_ST_S  2
/* UART_TXFIFO_EMPTY_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UART_TXFIFO_EMPTY_INT_ST  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_ST_M  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_ST_V  0x1
#define UART_TXFIFO_EMPTY_INT_ST_S  1
/* UART_RXFIFO_FULL_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_FULL_INT_ST  (BIT(0))
#define UART_RXFIFO_FULL_INT_ST_M  (BIT(0))
#define UART_RXFIFO_FULL_INT_ST_V  0x1
#define UART_RXFIFO_FULL_INT_ST_S  0

#define UART_INT_ENA_REG(i)          (REG_UART_BASE(i) + 0xC)
/* UART_WAKEUP_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define UART_WAKEUP_INT_ENA  (BIT(19))
#define UART_WAKEUP_INT_ENA_M  (BIT(19))
#define UART_WAKEUP_INT_ENA_V  0x1
#define UART_WAKEUP_INT_ENA_S  19
/* UART_AT_CMD_CHAR_DET_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define UART_AT_CMD_CHAR_DET_INT_ENA  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_ENA_M  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_ENA_V  0x1
#define UART_AT_CMD_CHAR_DET_INT_ENA_S  18
/* UART_RS485_CLASH_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_CLASH_INT_ENA  (BIT(17))
#define UART_RS485_CLASH_INT_ENA_M  (BIT(17))
#define UART_RS485_CLASH_INT_ENA_V  0x1
#define UART_RS485_CLASH_INT_ENA_S  17
/* UART_RS485_FRM_ERR_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_FRM_ERR_INT_ENA  (BIT(16))
#define UART_RS485_FRM_ERR_INT_ENA_M  (BIT(16))
#define UART_RS485_FRM_ERR_INT_ENA_V  0x1
#define UART_RS485_FRM_ERR_INT_ENA_S  16
/* UART_RS485_PARITY_ERR_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_PARITY_ERR_INT_ENA  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_ENA_M  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_ENA_V  0x1
#define UART_RS485_PARITY_ERR_INT_ENA_S  15
/* UART_TX_DONE_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UART_TX_DONE_INT_ENA  (BIT(14))
#define UART_TX_DONE_INT_ENA_M  (BIT(14))
#define UART_TX_DONE_INT_ENA_V  0x1
#define UART_TX_DONE_INT_ENA_S  14
/* UART_TX_BRK_IDLE_DONE_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_IDLE_DONE_INT_ENA  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_ENA_M  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_ENA_V  0x1
#define UART_TX_BRK_IDLE_DONE_INT_ENA_S  13
/* UART_TX_BRK_DONE_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_DONE_INT_ENA  (BIT(12))
#define UART_TX_BRK_DONE_INT_ENA_M  (BIT(12))
#define UART_TX_BRK_DONE_INT_ENA_V  0x1
#define UART_TX_BRK_DONE_INT_ENA_S  12
/* UART_GLITCH_DET_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UART_GLITCH_DET_INT_ENA  (BIT(11))
#define UART_GLITCH_DET_INT_ENA_M  (BIT(11))
#define UART_GLITCH_DET_INT_ENA_V  0x1
#define UART_GLITCH_DET_INT_ENA_S  11
/* UART_SW_XOFF_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XOFF_INT_ENA  (BIT(10))
#define UART_SW_XOFF_INT_ENA_M  (BIT(10))
#define UART_SW_XOFF_INT_ENA_V  0x1
#define UART_SW_XOFF_INT_ENA_S  10
/* UART_SW_XON_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XON_INT_ENA  (BIT(9))
#define UART_SW_XON_INT_ENA_M  (BIT(9))
#define UART_SW_XON_INT_ENA_V  0x1
#define UART_SW_XON_INT_ENA_S  9
/* UART_RXFIFO_TOUT_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_TOUT_INT_ENA  (BIT(8))
#define UART_RXFIFO_TOUT_INT_ENA_M  (BIT(8))
#define UART_RXFIFO_TOUT_INT_ENA_V  0x1
#define UART_RXFIFO_TOUT_INT_ENA_S  8
/* UART_BRK_DET_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UART_BRK_DET_INT_ENA  (BIT(7))
#define UART_BRK_DET_INT_ENA_M  (BIT(7))
#define UART_BRK_DET_INT_ENA_V  0x1
#define UART_BRK_DET_INT_ENA_S  7
/* UART_CTS_CHG_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UART_CTS_CHG_INT_ENA  (BIT(6))
#define UART_CTS_CHG_INT_ENA_M  (BIT(6))
#define UART_CTS_CHG_INT_ENA_V  0x1
#define UART_CTS_CHG_INT_ENA_S  6
/* UART_DSR_CHG_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UART_DSR_CHG_INT_ENA  (BIT(5))
#define UART_DSR_CHG_INT_ENA_M  (BIT(5))
#define UART_DSR_CHG_INT_ENA_V  0x1
#define UART_DSR_CHG_INT_ENA_S  5
/* UART_RXFIFO_OVF_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_OVF_INT_ENA  (BIT(4))
#define UART_RXFIFO_OVF_INT_ENA_M  (BIT(4))
#define UART_RXFIFO_OVF_INT_ENA_V  0x1
#define UART_RXFIFO_OVF_INT_ENA_S  4
/* UART_FRM_ERR_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UART_FRM_ERR_INT_ENA  (BIT(3))
#define UART_FRM_ERR_INT_ENA_M  (BIT(3))
#define UART_FRM_ERR_INT_ENA_V  0x1
#define UART_FRM_ERR_INT_ENA_S  3
/* UART_PARITY_ERR_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UART_PARITY_ERR_INT_ENA  (BIT(2))
#define UART_PARITY_ERR_INT_ENA_M  (BIT(2))
#define UART_PARITY_ERR_INT_ENA_V  0x1
#define UART_PARITY_ERR_INT_ENA_S  2
/* UART_TXFIFO_EMPTY_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UART_TXFIFO_EMPTY_INT_ENA  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_ENA_M  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_ENA_V  0x1
#define UART_TXFIFO_EMPTY_INT_ENA_S  1
/* UART_RXFIFO_FULL_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_FULL_INT_ENA  (BIT(0))
#define UART_RXFIFO_FULL_INT_ENA_M  (BIT(0))
#define UART_RXFIFO_FULL_INT_ENA_V  0x1
#define UART_RXFIFO_FULL_INT_ENA_S  0

#define UART_INT_CLR_REG(i)          (REG_UART_BASE(i) + 0x10)
/* UART_WAKEUP_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define UART_WAKEUP_INT_CLR  (BIT(19))
#define UART_WAKEUP_INT_CLR_M  (BIT(19))
#define UART_WAKEUP_INT_CLR_V  0x1
#define UART_WAKEUP_INT_CLR_S  19
/* UART_AT_CMD_CHAR_DET_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define UART_AT_CMD_CHAR_DET_INT_CLR  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_CLR_M  (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_CLR_V  0x1
#define UART_AT_CMD_CHAR_DET_INT_CLR_S  18
/* UART_RS485_CLASH_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_CLASH_INT_CLR  (BIT(17))
#define UART_RS485_CLASH_INT_CLR_M  (BIT(17))
#define UART_RS485_CLASH_INT_CLR_V  0x1
#define UART_RS485_CLASH_INT_CLR_S  17
/* UART_RS485_FRM_ERR_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_FRM_ERR_INT_CLR  (BIT(16))
#define UART_RS485_FRM_ERR_INT_CLR_M  (BIT(16))
#define UART_RS485_FRM_ERR_INT_CLR_V  0x1
#define UART_RS485_FRM_ERR_INT_CLR_S  16
/* UART_RS485_PARITY_ERR_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_PARITY_ERR_INT_CLR  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_CLR_M  (BIT(15))
#define UART_RS485_PARITY_ERR_INT_CLR_V  0x1
#define UART_RS485_PARITY_ERR_INT_CLR_S  15
/* UART_TX_DONE_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UART_TX_DONE_INT_CLR  (BIT(14))
#define UART_TX_DONE_INT_CLR_M  (BIT(14))
#define UART_TX_DONE_INT_CLR_V  0x1
#define UART_TX_DONE_INT_CLR_S  14
/* UART_TX_BRK_IDLE_DONE_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_IDLE_DONE_INT_CLR  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_CLR_M  (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_CLR_V  0x1
#define UART_TX_BRK_IDLE_DONE_INT_CLR_S  13
/* UART_TX_BRK_DONE_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UART_TX_BRK_DONE_INT_CLR  (BIT(12))
#define UART_TX_BRK_DONE_INT_CLR_M  (BIT(12))
#define UART_TX_BRK_DONE_INT_CLR_V  0x1
#define UART_TX_BRK_DONE_INT_CLR_S  12
/* UART_GLITCH_DET_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UART_GLITCH_DET_INT_CLR  (BIT(11))
#define UART_GLITCH_DET_INT_CLR_M  (BIT(11))
#define UART_GLITCH_DET_INT_CLR_V  0x1
#define UART_GLITCH_DET_INT_CLR_S  11
/* UART_SW_XOFF_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XOFF_INT_CLR  (BIT(10))
#define UART_SW_XOFF_INT_CLR_M  (BIT(10))
#define UART_SW_XOFF_INT_CLR_V  0x1
#define UART_SW_XOFF_INT_CLR_S  10
/* UART_SW_XON_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UART_SW_XON_INT_CLR  (BIT(9))
#define UART_SW_XON_INT_CLR_M  (BIT(9))
#define UART_SW_XON_INT_CLR_V  0x1
#define UART_SW_XON_INT_CLR_S  9
/* UART_RXFIFO_TOUT_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_TOUT_INT_CLR  (BIT(8))
#define UART_RXFIFO_TOUT_INT_CLR_M  (BIT(8))
#define UART_RXFIFO_TOUT_INT_CLR_V  0x1
#define UART_RXFIFO_TOUT_INT_CLR_S  8
/* UART_BRK_DET_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UART_BRK_DET_INT_CLR  (BIT(7))
#define UART_BRK_DET_INT_CLR_M  (BIT(7))
#define UART_BRK_DET_INT_CLR_V  0x1
#define UART_BRK_DET_INT_CLR_S  7
/* UART_CTS_CHG_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UART_CTS_CHG_INT_CLR  (BIT(6))
#define UART_CTS_CHG_INT_CLR_M  (BIT(6))
#define UART_CTS_CHG_INT_CLR_V  0x1
#define UART_CTS_CHG_INT_CLR_S  6
/* UART_DSR_CHG_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UART_DSR_CHG_INT_CLR  (BIT(5))
#define UART_DSR_CHG_INT_CLR_M  (BIT(5))
#define UART_DSR_CHG_INT_CLR_V  0x1
#define UART_DSR_CHG_INT_CLR_S  5
/* UART_RXFIFO_OVF_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_OVF_INT_CLR  (BIT(4))
#define UART_RXFIFO_OVF_INT_CLR_M  (BIT(4))
#define UART_RXFIFO_OVF_INT_CLR_V  0x1
#define UART_RXFIFO_OVF_INT_CLR_S  4
/* UART_FRM_ERR_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UART_FRM_ERR_INT_CLR  (BIT(3))
#define UART_FRM_ERR_INT_CLR_M  (BIT(3))
#define UART_FRM_ERR_INT_CLR_V  0x1
#define UART_FRM_ERR_INT_CLR_S  3
/* UART_PARITY_ERR_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UART_PARITY_ERR_INT_CLR  (BIT(2))
#define UART_PARITY_ERR_INT_CLR_M  (BIT(2))
#define UART_PARITY_ERR_INT_CLR_V  0x1
#define UART_PARITY_ERR_INT_CLR_S  2
/* UART_TXFIFO_EMPTY_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UART_TXFIFO_EMPTY_INT_CLR  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_CLR_M  (BIT(1))
#define UART_TXFIFO_EMPTY_INT_CLR_V  0x1
#define UART_TXFIFO_EMPTY_INT_CLR_S  1
/* UART_RXFIFO_FULL_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_RXFIFO_FULL_INT_CLR  (BIT(0))
#define UART_RXFIFO_FULL_INT_CLR_M  (BIT(0))
#define UART_RXFIFO_FULL_INT_CLR_V  0x1
#define UART_RXFIFO_FULL_INT_CLR_S  0

#define UART_CLKDIV_REG(i)          (REG_UART_BASE(i) + 0x14)
/* UART_CLKDIV_FRAG : R/W ;bitpos:[23:20] ;default: 4'h0 ; */
/*description: */
#define UART_CLKDIV_FRAG  0x0000000F
#define UART_CLKDIV_FRAG_M  ((UART_CLKDIV_FRAG_V)<<(UART_CLKDIV_FRAG_S))
#define UART_CLKDIV_FRAG_V  0xF
#define UART_CLKDIV_FRAG_S  20
/* UART_CLKDIV : R/W ;bitpos:[19:0] ;default: 20'h2B6 ; */
/*description: */
#define UART_CLKDIV  0x000FFFFF
#define UART_CLKDIV_M  ((UART_CLKDIV_V)<<(UART_CLKDIV_S))
#define UART_CLKDIV_V  0xFFFFF
#define UART_CLKDIV_S  0

#define UART_AUTOBAUD_REG(i)          (REG_UART_BASE(i) + 0x18)
/* UART_GLITCH_FILT : R/W ;bitpos:[15:8] ;default: 8'h10 ; */
/*description: */
#define UART_GLITCH_FILT  0x000000FF
#define UART_GLITCH_FILT_M  ((UART_GLITCH_FILT_V)<<(UART_GLITCH_FILT_S))
#define UART_GLITCH_FILT_V  0xFF
#define UART_GLITCH_FILT_S  8
/* UART_AUTOBAUD_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_AUTOBAUD_EN  (BIT(0))
#define UART_AUTOBAUD_EN_M  (BIT(0))
#define UART_AUTOBAUD_EN_V  0x1
#define UART_AUTOBAUD_EN_S  0

#define UART_STATUS_REG(i)          (REG_UART_BASE(i) + 0x1C)
/* UART_TXD : RO ;bitpos:[31] ;default: 8'h0 ; */
/*description: */
#define UART_TXD  (BIT(31))
#define UART_TXD_M  (BIT(31))
#define UART_TXD_V  0x1
#define UART_TXD_S  31
/* UART_RTSN : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define UART_RTSN  (BIT(30))
#define UART_RTSN_M  (BIT(30))
#define UART_RTSN_V  0x1
#define UART_RTSN_S  30
/* UART_DTRN : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define UART_DTRN  (BIT(29))
#define UART_DTRN_M  (BIT(29))
#define UART_DTRN_V  0x1
#define UART_DTRN_S  29
/* UART_TXFIFO_CNT : RO ;bitpos:[25:16] ;default: 10'b0 ; */
/*description: */
#define UART_TXFIFO_CNT  0x000003FF
#define UART_TXFIFO_CNT_M  ((UART_TXFIFO_CNT_V)<<(UART_TXFIFO_CNT_S))
#define UART_TXFIFO_CNT_V  0x3FF
#define UART_TXFIFO_CNT_S  16
/* UART_RXD : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UART_RXD  (BIT(15))
#define UART_RXD_M  (BIT(15))
#define UART_RXD_V  0x1
#define UART_RXD_S  15
/* UART_CTSN : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UART_CTSN  (BIT(14))
#define UART_CTSN_M  (BIT(14))
#define UART_CTSN_V  0x1
#define UART_CTSN_S  14
/* UART_DSRN : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UART_DSRN  (BIT(13))
#define UART_DSRN_M  (BIT(13))
#define UART_DSRN_V  0x1
#define UART_DSRN_S  13
/* UART_RXFIFO_CNT : RO ;bitpos:[9:0] ;default: 10'b0 ; */
/*description: */
#define UART_RXFIFO_CNT  0x000003FF
#define UART_RXFIFO_CNT_M  ((UART_RXFIFO_CNT_V)<<(UART_RXFIFO_CNT_S))
#define UART_RXFIFO_CNT_V  0x3FF
#define UART_RXFIFO_CNT_S  0

#define UART_CONF0_REG(i)          (REG_UART_BASE(i) + 0x20)
/* UART_MEM_CLK_EN : R/W ;bitpos:[28] ;default: 1'h1 ; */
/*description: */
#define UART_MEM_CLK_EN  (BIT(28))
#define UART_MEM_CLK_EN_M  (BIT(28))
#define UART_MEM_CLK_EN_V  0x1
#define UART_MEM_CLK_EN_S  28
/* UART_TICK_REF_ALWAYS_ON : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: */
#define UART_TICK_REF_ALWAYS_ON  (BIT(27))
#define UART_TICK_REF_ALWAYS_ON_M  (BIT(27))
#define UART_TICK_REF_ALWAYS_ON_V  0x1
#define UART_TICK_REF_ALWAYS_ON_S  27
/* UART_ERR_WR_MASK : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define UART_ERR_WR_MASK  (BIT(26))
#define UART_ERR_WR_MASK_M  (BIT(26))
#define UART_ERR_WR_MASK_V  0x1
#define UART_ERR_WR_MASK_S  26
/* UART_CLK_EN : R/W ;bitpos:[25] ;default: 1'h0 ; */
/*description: */
#define UART_CLK_EN  (BIT(25))
#define UART_CLK_EN_M  (BIT(25))
#define UART_CLK_EN_V  0x1
#define UART_CLK_EN_S  25
/* UART_DTR_INV : R/W ;bitpos:[24] ;default: 1'h0 ; */
/*description: */
#define UART_DTR_INV  (BIT(24))
#define UART_DTR_INV_M  (BIT(24))
#define UART_DTR_INV_V  0x1
#define UART_DTR_INV_S  24
/* UART_RTS_INV : R/W ;bitpos:[23] ;default: 1'h0 ; */
/*description: */
#define UART_RTS_INV  (BIT(23))
#define UART_RTS_INV_M  (BIT(23))
#define UART_RTS_INV_V  0x1
#define UART_RTS_INV_S  23
/* UART_TXD_INV : R/W ;bitpos:[22] ;default: 1'h0 ; */
/*description: */
#define UART_TXD_INV  (BIT(22))
#define UART_TXD_INV_M  (BIT(22))
#define UART_TXD_INV_V  0x1
#define UART_TXD_INV_S  22
/* UART_DSR_INV : R/W ;bitpos:[21] ;default: 1'h0 ; */
/*description: */
#define UART_DSR_INV  (BIT(21))
#define UART_DSR_INV_M  (BIT(21))
#define UART_DSR_INV_V  0x1
#define UART_DSR_INV_S  21
/* UART_CTS_INV : R/W ;bitpos:[20] ;default: 1'h0 ; */
/*description: */
#define UART_CTS_INV  (BIT(20))
#define UART_CTS_INV_M  (BIT(20))
#define UART_CTS_INV_V  0x1
#define UART_CTS_INV_S  20
/* UART_RXD_INV : R/W ;bitpos:[19] ;default: 1'h0 ; */
/*description: */
#define UART_RXD_INV  (BIT(19))
#define UART_RXD_INV_M  (BIT(19))
#define UART_RXD_INV_V  0x1
#define UART_RXD_INV_S  19
/* UART_TXFIFO_RST : R/W ;bitpos:[18] ;default: 1'h0 ; */
/*description: */
#define UART_TXFIFO_RST  (BIT(18))
#define UART_TXFIFO_RST_M  (BIT(18))
#define UART_TXFIFO_RST_V  0x1
#define UART_TXFIFO_RST_S  18
/* UART_RXFIFO_RST : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: */
#define UART_RXFIFO_RST  (BIT(17))
#define UART_RXFIFO_RST_M  (BIT(17))
#define UART_RXFIFO_RST_V  0x1
#define UART_RXFIFO_RST_S  17
/* UART_IRDA_EN : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define UART_IRDA_EN  (BIT(16))
#define UART_IRDA_EN_M  (BIT(16))
#define UART_IRDA_EN_V  0x1
#define UART_IRDA_EN_S  16
/* UART_TX_FLOW_EN : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UART_TX_FLOW_EN  (BIT(15))
#define UART_TX_FLOW_EN_M  (BIT(15))
#define UART_TX_FLOW_EN_V  0x1
#define UART_TX_FLOW_EN_S  15
/* UART_LOOPBACK : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UART_LOOPBACK  (BIT(14))
#define UART_LOOPBACK_M  (BIT(14))
#define UART_LOOPBACK_V  0x1
#define UART_LOOPBACK_S  14
/* UART_IRDA_RX_INV : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UART_IRDA_RX_INV  (BIT(13))
#define UART_IRDA_RX_INV_M  (BIT(13))
#define UART_IRDA_RX_INV_V  0x1
#define UART_IRDA_RX_INV_S  13
/* UART_IRDA_TX_INV : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UART_IRDA_TX_INV  (BIT(12))
#define UART_IRDA_TX_INV_M  (BIT(12))
#define UART_IRDA_TX_INV_V  0x1
#define UART_IRDA_TX_INV_S  12
/* UART_IRDA_WCTL : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UART_IRDA_WCTL  (BIT(11))
#define UART_IRDA_WCTL_M  (BIT(11))
#define UART_IRDA_WCTL_V  0x1
#define UART_IRDA_WCTL_S  11
/* UART_IRDA_TX_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UART_IRDA_TX_EN  (BIT(10))
#define UART_IRDA_TX_EN_M  (BIT(10))
#define UART_IRDA_TX_EN_V  0x1
#define UART_IRDA_TX_EN_S  10
/* UART_IRDA_DPLX : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UART_IRDA_DPLX  (BIT(9))
#define UART_IRDA_DPLX_M  (BIT(9))
#define UART_IRDA_DPLX_V  0x1
#define UART_IRDA_DPLX_S  9
/* UART_TXD_BRK : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UART_TXD_BRK  (BIT(8))
#define UART_TXD_BRK_M  (BIT(8))
#define UART_TXD_BRK_V  0x1
#define UART_TXD_BRK_S  8
/* UART_SW_DTR : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UART_SW_DTR  (BIT(7))
#define UART_SW_DTR_M  (BIT(7))
#define UART_SW_DTR_V  0x1
#define UART_SW_DTR_S  7
/* UART_SW_RTS : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UART_SW_RTS  (BIT(6))
#define UART_SW_RTS_M  (BIT(6))
#define UART_SW_RTS_V  0x1
#define UART_SW_RTS_S  6
/* UART_STOP_BIT_NUM : R/W ;bitpos:[5:4] ;default: 2'd1 ; */
/*description: */
#define UART_STOP_BIT_NUM  0x00000003
#define UART_STOP_BIT_NUM_M  ((UART_STOP_BIT_NUM_V)<<(UART_STOP_BIT_NUM_S))
#define UART_STOP_BIT_NUM_V  0x3
#define UART_STOP_BIT_NUM_S  4
/* UART_BIT_NUM : R/W ;bitpos:[3:2] ;default: 2'd3 ; */
/*description: */
#define UART_BIT_NUM  0x00000003
#define UART_BIT_NUM_M  ((UART_BIT_NUM_V)<<(UART_BIT_NUM_S))
#define UART_BIT_NUM_V  0x3
#define UART_BIT_NUM_S  2
/* UART_PARITY_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UART_PARITY_EN  (BIT(1))
#define UART_PARITY_EN_M  (BIT(1))
#define UART_PARITY_EN_V  0x1
#define UART_PARITY_EN_S  1
/* UART_PARITY : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_PARITY  (BIT(0))
#define UART_PARITY_M  (BIT(0))
#define UART_PARITY_V  0x1
#define UART_PARITY_S  0

#define UART_CONF1_REG(i)          (REG_UART_BASE(i) + 0x24)
/* UART_RX_TOUT_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define UART_RX_TOUT_EN  (BIT(31))
#define UART_RX_TOUT_EN_M  (BIT(31))
#define UART_RX_TOUT_EN_V  0x1
#define UART_RX_TOUT_EN_S  31
/* UART_RX_FLOW_EN : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define UART_RX_FLOW_EN  (BIT(30))
#define UART_RX_FLOW_EN_M  (BIT(30))
#define UART_RX_FLOW_EN_V  0x1
#define UART_RX_FLOW_EN_S  30
/* UART_RX_TOUT_FLOW_DIS : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define UART_RX_TOUT_FLOW_DIS  (BIT(29))
#define UART_RX_TOUT_FLOW_DIS_M  (BIT(29))
#define UART_RX_TOUT_FLOW_DIS_V  0x1
#define UART_RX_TOUT_FLOW_DIS_S  29
/* UART_TXFIFO_EMPTY_THRHD : R/W ;bitpos:[17:9] ;default: 9'h60 ; */
/*description: */
#define UART_TXFIFO_EMPTY_THRHD  0x000001FF
#define UART_TXFIFO_EMPTY_THRHD_M  ((UART_TXFIFO_EMPTY_THRHD_V)<<(UART_TXFIFO_EMPTY_THRHD_S))
#define UART_TXFIFO_EMPTY_THRHD_V  0x1FF
#define UART_TXFIFO_EMPTY_THRHD_S  9
/* UART_RXFIFO_FULL_THRHD : R/W ;bitpos:[8:0] ;default: 9'h60 ; */
/*description: */
#define UART_RXFIFO_FULL_THRHD  0x000001FF
#define UART_RXFIFO_FULL_THRHD_M  ((UART_RXFIFO_FULL_THRHD_V)<<(UART_RXFIFO_FULL_THRHD_S))
#define UART_RXFIFO_FULL_THRHD_V  0x1FF
#define UART_RXFIFO_FULL_THRHD_S  0

#define UART_LOWPULSE_REG(i)          (REG_UART_BASE(i) + 0x28)
/* UART_LOWPULSE_MIN_CNT : RO ;bitpos:[19:0] ;default: 20'hFFFFF ; */
/*description: */
#define UART_LOWPULSE_MIN_CNT  0x000FFFFF
#define UART_LOWPULSE_MIN_CNT_M  ((UART_LOWPULSE_MIN_CNT_V)<<(UART_LOWPULSE_MIN_CNT_S))
#define UART_LOWPULSE_MIN_CNT_V  0xFFFFF
#define UART_LOWPULSE_MIN_CNT_S  0

#define UART_HIGHPULSE_REG(i)          (REG_UART_BASE(i) + 0x2C)
/* UART_HIGHPULSE_MIN_CNT : RO ;bitpos:[19:0] ;default: 20'hFFFFF ; */
/*description: */
#define UART_HIGHPULSE_MIN_CNT  0x000FFFFF
#define UART_HIGHPULSE_MIN_CNT_M  ((UART_HIGHPULSE_MIN_CNT_V)<<(UART_HIGHPULSE_MIN_CNT_S))
#define UART_HIGHPULSE_MIN_CNT_V  0xFFFFF
#define UART_HIGHPULSE_MIN_CNT_S  0

#define UART_RXD_CNT_REG(i)          (REG_UART_BASE(i) + 0x30)
/* UART_RXD_EDGE_CNT : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define UART_RXD_EDGE_CNT  0x000003FF
#define UART_RXD_EDGE_CNT_M  ((UART_RXD_EDGE_CNT_V)<<(UART_RXD_EDGE_CNT_S))
#define UART_RXD_EDGE_CNT_V  0x3FF
#define UART_RXD_EDGE_CNT_S  0

#define UART_FLOW_CONF_REG(i)          (REG_UART_BASE(i) + 0x34)
/* UART_SEND_XOFF : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UART_SEND_XOFF  (BIT(5))
#define UART_SEND_XOFF_M  (BIT(5))
#define UART_SEND_XOFF_V  0x1
#define UART_SEND_XOFF_S  5
/* UART_SEND_XON : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UART_SEND_XON  (BIT(4))
#define UART_SEND_XON_M  (BIT(4))
#define UART_SEND_XON_V  0x1
#define UART_SEND_XON_S  4
/* UART_FORCE_XOFF : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UART_FORCE_XOFF  (BIT(3))
#define UART_FORCE_XOFF_M  (BIT(3))
#define UART_FORCE_XOFF_V  0x1
#define UART_FORCE_XOFF_S  3
/* UART_FORCE_XON : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UART_FORCE_XON  (BIT(2))
#define UART_FORCE_XON_M  (BIT(2))
#define UART_FORCE_XON_V  0x1
#define UART_FORCE_XON_S  2
/* UART_XONOFF_DEL : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UART_XONOFF_DEL  (BIT(1))
#define UART_XONOFF_DEL_M  (BIT(1))
#define UART_XONOFF_DEL_V  0x1
#define UART_XONOFF_DEL_S  1
/* UART_SW_FLOW_CON_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_SW_FLOW_CON_EN  (BIT(0))
#define UART_SW_FLOW_CON_EN_M  (BIT(0))
#define UART_SW_FLOW_CON_EN_V  0x1
#define UART_SW_FLOW_CON_EN_S  0

#define UART_SLEEP_CONF_REG(i)          (REG_UART_BASE(i) + 0x38)
/* UART_ACTIVE_THRESHOLD : R/W ;bitpos:[9:0] ;default: 10'hf0 ; */
/*description: */
#define UART_ACTIVE_THRESHOLD  0x000003FF
#define UART_ACTIVE_THRESHOLD_M  ((UART_ACTIVE_THRESHOLD_V)<<(UART_ACTIVE_THRESHOLD_S))
#define UART_ACTIVE_THRESHOLD_V  0x3FF
#define UART_ACTIVE_THRESHOLD_S  0

#define UART_SWFC_CONF0_REG(i)          (REG_UART_BASE(i) + 0x3C)
/* UART_XOFF_CHAR : R/W ;bitpos:[16:9] ;default: 8'h13 ; */
/*description: */
#define UART_XOFF_CHAR  0x000000FF
#define UART_XOFF_CHAR_M  ((UART_XOFF_CHAR_V)<<(UART_XOFF_CHAR_S))
#define UART_XOFF_CHAR_V  0xFF
#define UART_XOFF_CHAR_S  9
/* UART_XOFF_THRESHOLD : R/W ;bitpos:[8:0] ;default: 9'he0 ; */
/*description: */
#define UART_XOFF_THRESHOLD  0x000001FF
#define UART_XOFF_THRESHOLD_M  ((UART_XOFF_THRESHOLD_V)<<(UART_XOFF_THRESHOLD_S))
#define UART_XOFF_THRESHOLD_V  0x1FF
#define UART_XOFF_THRESHOLD_S  0

#define UART_SWFC_CONF1_REG(i)          (REG_UART_BASE(i) + 0x40)
/* UART_XON_CHAR : R/W ;bitpos:[16:9] ;default: 8'h11 ; */
/*description: */
#define UART_XON_CHAR  0x000000FF
#define UART_XON_CHAR_M  ((UART_XON_CHAR_V)<<(UART_XON_CHAR_S))
#define UART_XON_CHAR_V  0xFF
#define UART_XON_CHAR_S  9
/* UART_XON_THRESHOLD : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: */
#define UART_XON_THRESHOLD  0x000001FF
#define UART_XON_THRESHOLD_M  ((UART_XON_THRESHOLD_V)<<(UART_XON_THRESHOLD_S))
#define UART_XON_THRESHOLD_V  0x1FF
#define UART_XON_THRESHOLD_S  0

#define UART_IDLE_CONF_REG(i)          (REG_UART_BASE(i) + 0x44)
/* UART_TX_BRK_NUM : R/W ;bitpos:[27:20] ;default: 8'ha ; */
/*description: */
#define UART_TX_BRK_NUM  0x000000FF
#define UART_TX_BRK_NUM_M  ((UART_TX_BRK_NUM_V)<<(UART_TX_BRK_NUM_S))
#define UART_TX_BRK_NUM_V  0xFF
#define UART_TX_BRK_NUM_S  20
/* UART_TX_IDLE_NUM : R/W ;bitpos:[19:10] ;default: 10'h100 ; */
/*description: */
#define UART_TX_IDLE_NUM  0x000003FF
#define UART_TX_IDLE_NUM_M  ((UART_TX_IDLE_NUM_V)<<(UART_TX_IDLE_NUM_S))
#define UART_TX_IDLE_NUM_V  0x3FF
#define UART_TX_IDLE_NUM_S  10
/* UART_RX_IDLE_THRHD : R/W ;bitpos:[9:0] ;default: 10'h100 ; */
/*description: */
#define UART_RX_IDLE_THRHD  0x000003FF
#define UART_RX_IDLE_THRHD_M  ((UART_RX_IDLE_THRHD_V)<<(UART_RX_IDLE_THRHD_S))
#define UART_RX_IDLE_THRHD_V  0x3FF
#define UART_RX_IDLE_THRHD_S  0

#define UART_RS485_CONF_REG(i)          (REG_UART_BASE(i) + 0x48)
/* UART_RS485_TX_DLY_NUM : R/W ;bitpos:[9:6] ;default: 4'b0 ; */
/*description: */
#define UART_RS485_TX_DLY_NUM  0x0000000F
#define UART_RS485_TX_DLY_NUM_M  ((UART_RS485_TX_DLY_NUM_V)<<(UART_RS485_TX_DLY_NUM_S))
#define UART_RS485_TX_DLY_NUM_V  0xF
#define UART_RS485_TX_DLY_NUM_S  6
/* UART_RS485_RX_DLY_NUM : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_RX_DLY_NUM  (BIT(5))
#define UART_RS485_RX_DLY_NUM_M  (BIT(5))
#define UART_RS485_RX_DLY_NUM_V  0x1
#define UART_RS485_RX_DLY_NUM_S  5
/* UART_RS485RXBY_TX_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UART_RS485RXBY_TX_EN  (BIT(4))
#define UART_RS485RXBY_TX_EN_M  (BIT(4))
#define UART_RS485RXBY_TX_EN_V  0x1
#define UART_RS485RXBY_TX_EN_S  4
/* UART_RS485TX_RX_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UART_RS485TX_RX_EN  (BIT(3))
#define UART_RS485TX_RX_EN_M  (BIT(3))
#define UART_RS485TX_RX_EN_V  0x1
#define UART_RS485TX_RX_EN_S  3
/* UART_DL1_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UART_DL1_EN  (BIT(2))
#define UART_DL1_EN_M  (BIT(2))
#define UART_DL1_EN_V  0x1
#define UART_DL1_EN_S  2
/* UART_DL0_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UART_DL0_EN  (BIT(1))
#define UART_DL0_EN_M  (BIT(1))
#define UART_DL0_EN_V  0x1
#define UART_DL0_EN_S  1
/* UART_RS485_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UART_RS485_EN  (BIT(0))
#define UART_RS485_EN_M  (BIT(0))
#define UART_RS485_EN_V  0x1
#define UART_RS485_EN_S  0

#define UART_AT_CMD_PRECNT_REG(i)          (REG_UART_BASE(i) + 0x4c)
/* UART_PRE_IDLE_NUM : R/W ;bitpos:[15:0] ;default: 16'h901 ; */
/*description: */
#define UART_PRE_IDLE_NUM  0x0000FFFF
#define UART_PRE_IDLE_NUM_M  ((UART_PRE_IDLE_NUM_V)<<(UART_PRE_IDLE_NUM_S))
#define UART_PRE_IDLE_NUM_V  0xFFFF
#define UART_PRE_IDLE_NUM_S  0

#define UART_AT_CMD_POSTCNT_REG(i)          (REG_UART_BASE(i) + 0x50)
/* UART_POST_IDLE_NUM : R/W ;bitpos:[15:0] ;default: 16'h901 ; */
/*description: */
#define UART_POST_IDLE_NUM  0x0000FFFF
#define UART_POST_IDLE_NUM_M  ((UART_POST_IDLE_NUM_V)<<(UART_POST_IDLE_NUM_S))
#define UART_POST_IDLE_NUM_V  0xFFFF
#define UART_POST_IDLE_NUM_S  0

#define UART_AT_CMD_GAPTOUT_REG(i)          (REG_UART_BASE(i) + 0x54)
/* UART_RX_GAP_TOUT : R/W ;bitpos:[15:0] ;default: 16'd11 ; */
/*description: */
#define UART_RX_GAP_TOUT  0x0000FFFF
#define UART_RX_GAP_TOUT_M  ((UART_RX_GAP_TOUT_V)<<(UART_RX_GAP_TOUT_S))
#define UART_RX_GAP_TOUT_V  0xFFFF
#define UART_RX_GAP_TOUT_S  0

#define UART_AT_CMD_CHAR_REG(i)          (REG_UART_BASE(i) + 0x58)
/* UART_CHAR_NUM : R/W ;bitpos:[15:8] ;default: 8'h3 ; */
/*description: */
#define UART_CHAR_NUM  0x000000FF
#define UART_CHAR_NUM_M  ((UART_CHAR_NUM_V)<<(UART_CHAR_NUM_S))
#define UART_CHAR_NUM_V  0xFF
#define UART_CHAR_NUM_S  8
/* UART_AT_CMD_CHAR : R/W ;bitpos:[7:0] ;default: 8'h2b ; */
/*description: */
#define UART_AT_CMD_CHAR  0x000000FF
#define UART_AT_CMD_CHAR_M  ((UART_AT_CMD_CHAR_V)<<(UART_AT_CMD_CHAR_S))
#define UART_AT_CMD_CHAR_V  0xFF
#define UART_AT_CMD_CHAR_S  0

#define UART_MEM_CONF_REG(i)          (REG_UART_BASE(i) + 0x5c)
/* UART_MEM_FORCE_PU : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define UART_MEM_FORCE_PU  (BIT(27))
#define UART_MEM_FORCE_PU_M  (BIT(27))
#define UART_MEM_FORCE_PU_V  0x1
#define UART_MEM_FORCE_PU_S  27
/* UART_MEM_FORCE_PD : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define UART_MEM_FORCE_PD  (BIT(26))
#define UART_MEM_FORCE_PD_M  (BIT(26))
#define UART_MEM_FORCE_PD_V  0x1
#define UART_MEM_FORCE_PD_S  26
/* UART_RX_TOUT_THRHD : R/W ;bitpos:[25:16] ;default: 10'ha ; */
/*description: */
#define UART_RX_TOUT_THRHD  0x000003FF
#define UART_RX_TOUT_THRHD_M  ((UART_RX_TOUT_THRHD_V)<<(UART_RX_TOUT_THRHD_S))
#define UART_RX_TOUT_THRHD_V  0x3FF
#define UART_RX_TOUT_THRHD_S  16
/* UART_RX_FLOW_THRHD : R/W ;bitpos:[15:7] ;default: 9'h0 ; */
/*description: */
#define UART_RX_FLOW_THRHD  0x000001FF
#define UART_RX_FLOW_THRHD_M  ((UART_RX_FLOW_THRHD_V)<<(UART_RX_FLOW_THRHD_S))
#define UART_RX_FLOW_THRHD_V  0x1FF
#define UART_RX_FLOW_THRHD_S  7
/* UART_TX_SIZE : R/W ;bitpos:[6:4] ;default: 3'h1 ; */
/*description: */
#define UART_TX_SIZE  0x00000007
#define UART_TX_SIZE_M  ((UART_TX_SIZE_V)<<(UART_TX_SIZE_S))
#define UART_TX_SIZE_V  0x7
#define UART_TX_SIZE_S  4
/* UART_RX_SIZE : R/W ;bitpos:[3:1] ;default: 3'b1 ; */
/*description: */
#define UART_RX_SIZE  0x00000007
#define UART_RX_SIZE_M  ((UART_RX_SIZE_V)<<(UART_RX_SIZE_S))
#define UART_RX_SIZE_V  0x7
#define UART_RX_SIZE_S  1

#define UART_MEM_TX_STATUS_REG(i)          (REG_UART_BASE(i) + 0x60)
/* UART_TX_RADDR : RO ;bitpos:[20:11] ;default: 10'h0 ; */
/*description: */
#define UART_TX_RADDR  0x000003FF
#define UART_TX_RADDR_M  ((UART_TX_RADDR_V)<<(UART_TX_RADDR_S))
#define UART_TX_RADDR_V  0x3FF
#define UART_TX_RADDR_S  11
/* UART_APB_TX_WADDR : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define UART_APB_TX_WADDR  0x000003FF
#define UART_APB_TX_WADDR_M  ((UART_APB_TX_WADDR_V)<<(UART_APB_TX_WADDR_S))
#define UART_APB_TX_WADDR_V  0x3FF
#define UART_APB_TX_WADDR_S  0

#define UART_MEM_RX_STATUS_REG(i)          (REG_UART_BASE(i) + 0x64)
/* UART_RX_WADDR : RO ;bitpos:[20:11] ;default: 10'h0 ; */
/*description: */
#define UART_RX_WADDR  0x000003FF
#define UART_RX_WADDR_M  ((UART_RX_WADDR_V)<<(UART_RX_WADDR_S))
#define UART_RX_WADDR_V  0x3FF
#define UART_RX_WADDR_S  11
/* UART_APB_RX_RADDR : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define UART_APB_RX_RADDR  0x000003FF
#define UART_APB_RX_RADDR_M  ((UART_APB_RX_RADDR_V)<<(UART_APB_RX_RADDR_S))
#define UART_APB_RX_RADDR_V  0x3FF
#define UART_APB_RX_RADDR_S  0

#define UART_FSM_STATUS_REG(i)          (REG_UART_BASE(i) + 0x68)
/* UART_ST_UTX_OUT : RO ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: */
#define UART_ST_UTX_OUT  0x0000000F
#define UART_ST_UTX_OUT_M  ((UART_ST_UTX_OUT_V)<<(UART_ST_UTX_OUT_S))
#define UART_ST_UTX_OUT_V  0xF
#define UART_ST_UTX_OUT_S  4
/* UART_ST_URX_OUT : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: */
#define UART_ST_URX_OUT  0x0000000F
#define UART_ST_URX_OUT_M  ((UART_ST_URX_OUT_V)<<(UART_ST_URX_OUT_S))
#define UART_ST_URX_OUT_V  0xF
#define UART_ST_URX_OUT_S  0

#define UART_POSPULSE_REG(i)          (REG_UART_BASE(i) + 0x6c)
/* UART_POSEDGE_MIN_CNT : RO ;bitpos:[19:0] ;default: 20'hFFFFF ; */
/*description: */
#define UART_POSEDGE_MIN_CNT  0x000FFFFF
#define UART_POSEDGE_MIN_CNT_M  ((UART_POSEDGE_MIN_CNT_V)<<(UART_POSEDGE_MIN_CNT_S))
#define UART_POSEDGE_MIN_CNT_V  0xFFFFF
#define UART_POSEDGE_MIN_CNT_S  0

#define UART_NEGPULSE_REG(i)          (REG_UART_BASE(i) + 0x70)
/* UART_NEGEDGE_MIN_CNT : RO ;bitpos:[19:0] ;default: 20'hFFFFF ; */
/*description: */
#define UART_NEGEDGE_MIN_CNT  0x000FFFFF
#define UART_NEGEDGE_MIN_CNT_M  ((UART_NEGEDGE_MIN_CNT_V)<<(UART_NEGEDGE_MIN_CNT_S))
#define UART_NEGEDGE_MIN_CNT_V  0xFFFFF
#define UART_NEGEDGE_MIN_CNT_S  0

#define UART_DATE_REG(i)          (REG_UART_BASE(i) + 0x74)
/* UART_DATE : R/W ;bitpos:[31:0] ;default: 32'h19031400 ; */
/*description: */
#define UART_DATE  0xFFFFFFFF
#define UART_DATE_M  ((UART_DATE_V)<<(UART_DATE_S))
#define UART_DATE_V  0xFFFFFFFF
#define UART_DATE_S  0

#define UART_ID_REG(i)          (REG_UART_BASE(i) + 0x78)
/* UART_ID : R/W ;bitpos:[31:0] ;default: 32'h0500 ; */
/*description: */
#define UART_ID  0xFFFFFFFF
#define UART_ID_M  ((UART_ID_V)<<(UART_ID_S))
#define UART_ID_V  0xFFFFFFFF
#define UART_ID_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_UART_REG_H_ */


