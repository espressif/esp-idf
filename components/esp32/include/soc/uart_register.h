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
#ifndef _SOC_UART_REG_H_
#define _SOC_UART_REG_H_

#include "soc.h"

//Slow bus
//#define REG_UART_BASE( i )  (0x60000000+(i)*0x10000)
//Fast bus
#define REG_UART_BASE( i )  (0x3ff40000+(i)*0x10000)

#define UART_FIFO_REG(i)          (REG_UART_BASE(i) + 0x0)
    /* UART_RXFIFO_RD_BYTE : RO ;bitpos:[7:0] ;default: 8'b0 ; */
    /*description: R/W share the same address*/
    #define UART_RXFIFO_RD_BYTE  0x000000FF
    #define UART_RXFIFO_RD_BYTE_M  ((UART_RXFIFO_RD_BYTE_V)<<(UART_RXFIFO_RD_BYTE_S))
    #define UART_RXFIFO_RD_BYTE_V  0xFF
    #define UART_RXFIFO_RD_BYTE_S  0
        #define UART_GET_RXFIFO_RD_BYTE(i)  GET_PERI_REG_BITS2(UART_FIFO_REG(i) , UART_RXFIFO_RD_BYTE_V, UART_RXFIFO_RD_BYTE_S)
		#define UART_SET_TXFIFO_WR_BYTE(i,val)  SET_PERI_REG_BITS(UART_FIFO_REG(i) , UART_RXFIFO_RD_BYTE_V, val, UART_RXFIFO_RD_BYTE_S)



#define UART_INT_RAW_REG(i)          (REG_UART_BASE(i) + 0x4)
    /* UART_AT_CMD_CHAR_DET_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for detecting AT_CMD char interrupt*/
    #define UART_AT_CMD_CHAR_DET_INT_RAW  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_RAW_M  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_RAW_V  0x1
    #define UART_AT_CMD_CHAR_DET_INT_RAW_S  18
    /* UART_RS485_CLASH_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for rs485 tx rx clash interrrupt*/
    #define UART_RS485_CLASH_INT_RAW  (BIT(17))
    #define UART_RS485_CLASH_INT_RAW_M  (BIT(17))
    #define UART_RS485_CLASH_INT_RAW_V  0x1
    #define UART_RS485_CLASH_INT_RAW_S  17
    /* UART_RS485_FRM_ERR_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for rs485 other rx error*/
    #define UART_RS485_FRM_ERR_INT_RAW  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_RAW_M  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_RAW_V  0x1
    #define UART_RS485_FRM_ERR_INT_RAW_S  16
    /* UART_RS485_PARITY_ERR_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for rs485 parity error*/
    #define UART_RS485_PARITY_ERR_INT_RAW  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_RAW_M  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_RAW_V  0x1
    #define UART_RS485_PARITY_ERR_INT_RAW_S  15
    /* UART_TX_DONE_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for tx done*/
    #define UART_TX_DONE_INT_RAW  (BIT(14))
    #define UART_TX_DONE_INT_RAW_M  (BIT(14))
    #define UART_TX_DONE_INT_RAW_V  0x1
    #define UART_TX_DONE_INT_RAW_S  14
    /* UART_TX_BRK_IDLE_DONE_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for tx send idle done(depends on the uart_tx_idle_num)*/
    #define UART_TX_BRK_IDLE_DONE_INT_RAW  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_RAW_M  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_RAW_V  0x1
    #define UART_TX_BRK_IDLE_DONE_INT_RAW_S  13
    /* UART_TX_BRK_DONE_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for send 0 done(depends on uart_tx_brk_num)*/
    #define UART_TX_BRK_DONE_INT_RAW  (BIT(12))
    #define UART_TX_BRK_DONE_INT_RAW_M  (BIT(12))
    #define UART_TX_BRK_DONE_INT_RAW_V  0x1
    #define UART_TX_BRK_DONE_INT_RAW_S  12
    /* UART_GLITCH_DET_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for detecting rx glitch pulse*/
    #define UART_GLITCH_DET_INT_RAW  (BIT(11))
    #define UART_GLITCH_DET_INT_RAW_M  (BIT(11))
    #define UART_GLITCH_DET_INT_RAW_V  0x1
    #define UART_GLITCH_DET_INT_RAW_S  11
    /* UART_SW_XOFF_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for rx receive xoff char(depends on the uart_xoff_char )*/
    #define UART_SW_XOFF_INT_RAW  (BIT(10))
    #define UART_SW_XOFF_INT_RAW_M  (BIT(10))
    #define UART_SW_XOFF_INT_RAW_V  0x1
    #define UART_SW_XOFF_INT_RAW_S  10
    /* UART_SW_XON_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for rx receive xon char(depends on the uart_xon_char )*/
    #define UART_SW_XON_INT_RAW  (BIT(9))
    #define UART_SW_XON_INT_RAW_M  (BIT(9))
    #define UART_SW_XON_INT_RAW_V  0x1
    #define UART_SW_XON_INT_RAW_S  9
    /* UART_RXFIFO_TOUT_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for Rx time-out interrupt(depands on the UART_RX_TOUT_THRHD)*/
    #define UART_RXFIFO_TOUT_INT_RAW  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_RAW_M  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_RAW_V  0x1
    #define UART_RXFIFO_TOUT_INT_RAW_S  8
    /* UART_BRK_DET_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for Rx byte start error*/
    #define UART_BRK_DET_INT_RAW  (BIT(7))
    #define UART_BRK_DET_INT_RAW_M  (BIT(7))
    #define UART_BRK_DET_INT_RAW_V  0x1
    #define UART_BRK_DET_INT_RAW_S  7
    /* UART_CTS_CHG_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for CTS changing level*/
    #define UART_CTS_CHG_INT_RAW  (BIT(6))
    #define UART_CTS_CHG_INT_RAW_M  (BIT(6))
    #define UART_CTS_CHG_INT_RAW_V  0x1
    #define UART_CTS_CHG_INT_RAW_S  6
    /* UART_DSR_CHG_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for DSR changing level*/
    #define UART_DSR_CHG_INT_RAW  (BIT(5))
    #define UART_DSR_CHG_INT_RAW_M  (BIT(5))
    #define UART_DSR_CHG_INT_RAW_V  0x1
    #define UART_DSR_CHG_INT_RAW_S  5
    /* UART_RXFIFO_OVF_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for rx fifo overflow*/
    #define UART_RXFIFO_OVF_INT_RAW  (BIT(4))
    #define UART_RXFIFO_OVF_INT_RAW_M  (BIT(4))
    #define UART_RXFIFO_OVF_INT_RAW_V  0x1
    #define UART_RXFIFO_OVF_INT_RAW_S  4
    /* UART_FRM_ERR_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for other rx error*/
    #define UART_FRM_ERR_INT_RAW  (BIT(3))
    #define UART_FRM_ERR_INT_RAW_M  (BIT(3))
    #define UART_FRM_ERR_INT_RAW_V  0x1
    #define UART_FRM_ERR_INT_RAW_S  3
    /* UART_PARITY_ERR_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for parity check error*/
    #define UART_PARITY_ERR_INT_RAW  (BIT(2))
    #define UART_PARITY_ERR_INT_RAW_M  (BIT(2))
    #define UART_PARITY_ERR_INT_RAW_V  0x1
    #define UART_PARITY_ERR_INT_RAW_S  2
    /* UART_TXFIFO_EMPTY_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for tx fifo empty interrupt(depands on UART_TXFIFO_EMPTY_THRHD bits)*/
    #define UART_TXFIFO_EMPTY_INT_RAW  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_RAW_M  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_RAW_V  0x1
    #define UART_TXFIFO_EMPTY_INT_RAW_S  1
    /* UART_RXFIFO_FULL_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
    /*description: The interrupt raw bit for rx fifo full interrupt(depands on UART_RXFIFO_FULL_THRHD bits)*/
    #define UART_RXFIFO_FULL_INT_RAW  (BIT(0))
    #define UART_RXFIFO_FULL_INT_RAW_M  (BIT(0))
    #define UART_RXFIFO_FULL_INT_RAW_V  0x1
    #define UART_RXFIFO_FULL_INT_RAW_S  0
        #define UART_GET_AT_CMD_CHAR_DET_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_AT_CMD_CHAR_DET_INT_RAW_V, UART_AT_CMD_CHAR_DET_INT_RAW_S)
        #define UART_GET_RS485_CLASH_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_RS485_CLASH_INT_RAW_V, UART_RS485_CLASH_INT_RAW_S)
        #define UART_GET_RS485_FRM_ERR_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_RS485_FRM_ERR_INT_RAW_V, UART_RS485_FRM_ERR_INT_RAW_S)
        #define UART_GET_RS485_PARITY_ERR_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_RS485_PARITY_ERR_INT_RAW_V, UART_RS485_PARITY_ERR_INT_RAW_S)
        #define UART_GET_TX_DONE_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_TX_DONE_INT_RAW_V, UART_TX_DONE_INT_RAW_S)
        #define UART_GET_TX_BRK_IDLE_DONE_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_TX_BRK_IDLE_DONE_INT_RAW_V, UART_TX_BRK_IDLE_DONE_INT_RAW_S)
        #define UART_GET_TX_BRK_DONE_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_TX_BRK_DONE_INT_RAW_V, UART_TX_BRK_DONE_INT_RAW_S)
        #define UART_GET_GLITCH_DET_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_GLITCH_DET_INT_RAW_V, UART_GLITCH_DET_INT_RAW_S)
        #define UART_GET_SW_XOFF_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_SW_XOFF_INT_RAW_V, UART_SW_XOFF_INT_RAW_S)
        #define UART_GET_SW_XON_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_SW_XON_INT_RAW_V, UART_SW_XON_INT_RAW_S)
        #define UART_GET_RXFIFO_TOUT_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_RXFIFO_TOUT_INT_RAW_V, UART_RXFIFO_TOUT_INT_RAW_S)
        #define UART_GET_BRK_DET_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_BRK_DET_INT_RAW_V, UART_BRK_DET_INT_RAW_S)
        #define UART_GET_CTS_CHG_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_CTS_CHG_INT_RAW_V, UART_CTS_CHG_INT_RAW_S)
        #define UART_GET_DSR_CHG_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_DSR_CHG_INT_RAW_V, UART_DSR_CHG_INT_RAW_S)
        #define UART_GET_RXFIFO_OVF_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_RXFIFO_OVF_INT_RAW_V, UART_RXFIFO_OVF_INT_RAW_S)
        #define UART_GET_FRM_ERR_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_FRM_ERR_INT_RAW_V, UART_FRM_ERR_INT_RAW_S)
        #define UART_GET_PARITY_ERR_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_PARITY_ERR_INT_RAW_V, UART_PARITY_ERR_INT_RAW_S)
        #define UART_GET_TXFIFO_EMPTY_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_TXFIFO_EMPTY_INT_RAW_V, UART_TXFIFO_EMPTY_INT_RAW_S)
        #define UART_GET_RXFIFO_FULL_INT_RAW(i)  GET_PERI_REG_BITS2(UART_INT_RAW_REG(i) , UART_RXFIFO_FULL_INT_RAW_V, UART_RXFIFO_FULL_INT_RAW_S)



#define UART_INT_ST_REG(i)          (REG_UART_BASE(i) + 0x8)
    /* UART_AT_CMD_CHAR_DET_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
    /*description: The interrupt state bit for detecting AT_CMD char interrupt*/
    #define UART_AT_CMD_CHAR_DET_INT_ST  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_ST_M  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_ST_V  0x1
    #define UART_AT_CMD_CHAR_DET_INT_ST_S  18
    /* UART_RS485_CLASH_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
    /*description: The interrupt state bit for rs485 tx rx clash interrrupt*/
    #define UART_RS485_CLASH_INT_ST  (BIT(17))
    #define UART_RS485_CLASH_INT_ST_M  (BIT(17))
    #define UART_RS485_CLASH_INT_ST_V  0x1
    #define UART_RS485_CLASH_INT_ST_S  17
    /* UART_RS485_FRM_ERR_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
    /*description: The interrupt state bit for rs485 other rx error*/
    #define UART_RS485_FRM_ERR_INT_ST  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_ST_M  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_ST_V  0x1
    #define UART_RS485_FRM_ERR_INT_ST_S  16
    /* UART_RS485_PARITY_ERR_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
    /*description: The interrupt state bit for rs485 parity error*/
    #define UART_RS485_PARITY_ERR_INT_ST  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_ST_M  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_ST_V  0x1
    #define UART_RS485_PARITY_ERR_INT_ST_S  15
    /* UART_TX_DONE_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
    /*description: The interrupt state bit for tx done event*/
    #define UART_TX_DONE_INT_ST  (BIT(14))
    #define UART_TX_DONE_INT_ST_M  (BIT(14))
    #define UART_TX_DONE_INT_ST_V  0x1
    #define UART_TX_DONE_INT_ST_S  14
    /* UART_TX_BRK_IDLE_DONE_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
    /*description: The interrupt state bit for tx send idle done event*/
    #define UART_TX_BRK_IDLE_DONE_INT_ST  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_ST_M  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_ST_V  0x1
    #define UART_TX_BRK_IDLE_DONE_INT_ST_S  13
    /* UART_TX_BRK_DONE_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
    /*description: The interrupt state bit for send 0 done event*/
    #define UART_TX_BRK_DONE_INT_ST  (BIT(12))
    #define UART_TX_BRK_DONE_INT_ST_M  (BIT(12))
    #define UART_TX_BRK_DONE_INT_ST_V  0x1
    #define UART_TX_BRK_DONE_INT_ST_S  12
    /* UART_GLITCH_DET_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
    /*description: The interrupt state bit for detecting rx glitch pulse event*/
    #define UART_GLITCH_DET_INT_ST  (BIT(11))
    #define UART_GLITCH_DET_INT_ST_M  (BIT(11))
    #define UART_GLITCH_DET_INT_ST_V  0x1
    #define UART_GLITCH_DET_INT_ST_S  11
    /* UART_SW_XOFF_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
    /*description: The interrupt state bit for rx receive xoff char event*/
    #define UART_SW_XOFF_INT_ST  (BIT(10))
    #define UART_SW_XOFF_INT_ST_M  (BIT(10))
    #define UART_SW_XOFF_INT_ST_V  0x1
    #define UART_SW_XOFF_INT_ST_S  10
    /* UART_SW_XON_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
    /*description: The interrupt state bit for rx receive xon char event*/
    #define UART_SW_XON_INT_ST  (BIT(9))
    #define UART_SW_XON_INT_ST_M  (BIT(9))
    #define UART_SW_XON_INT_ST_V  0x1
    #define UART_SW_XON_INT_ST_S  9
    /* UART_RXFIFO_TOUT_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
    /*description: The interrupt state bit for Rx time-out event*/
    #define UART_RXFIFO_TOUT_INT_ST  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_ST_M  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_ST_V  0x1
    #define UART_RXFIFO_TOUT_INT_ST_S  8
    /* UART_BRK_DET_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
    /*description: The interrupt state bit for rx byte start error*/
    #define UART_BRK_DET_INT_ST  (BIT(7))
    #define UART_BRK_DET_INT_ST_M  (BIT(7))
    #define UART_BRK_DET_INT_ST_V  0x1
    #define UART_BRK_DET_INT_ST_S  7
    /* UART_CTS_CHG_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
    /*description: The interrupt state bit for CTS changing level*/
    #define UART_CTS_CHG_INT_ST  (BIT(6))
    #define UART_CTS_CHG_INT_ST_M  (BIT(6))
    #define UART_CTS_CHG_INT_ST_V  0x1
    #define UART_CTS_CHG_INT_ST_S  6
    /* UART_DSR_CHG_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
    /*description: The interrupt state bit for DSR changing level*/
    #define UART_DSR_CHG_INT_ST  (BIT(5))
    #define UART_DSR_CHG_INT_ST_M  (BIT(5))
    #define UART_DSR_CHG_INT_ST_V  0x1
    #define UART_DSR_CHG_INT_ST_S  5
    /* UART_RXFIFO_OVF_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
    /*description: The interrupt state bit for RX fifo overflow*/
    #define UART_RXFIFO_OVF_INT_ST  (BIT(4))
    #define UART_RXFIFO_OVF_INT_ST_M  (BIT(4))
    #define UART_RXFIFO_OVF_INT_ST_V  0x1
    #define UART_RXFIFO_OVF_INT_ST_S  4
    /* UART_FRM_ERR_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
    /*description: The interrupt state for other rx error*/
    #define UART_FRM_ERR_INT_ST  (BIT(3))
    #define UART_FRM_ERR_INT_ST_M  (BIT(3))
    #define UART_FRM_ERR_INT_ST_V  0x1
    #define UART_FRM_ERR_INT_ST_S  3
    /* UART_PARITY_ERR_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
    /*description: The interrupt state bit for rx parity error*/
    #define UART_PARITY_ERR_INT_ST  (BIT(2))
    #define UART_PARITY_ERR_INT_ST_M  (BIT(2))
    #define UART_PARITY_ERR_INT_ST_V  0x1
    #define UART_PARITY_ERR_INT_ST_S  2
    /* UART_TXFIFO_EMPTY_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
    /*description: The interrupt state bit for TX fifo empty*/
    #define UART_TXFIFO_EMPTY_INT_ST  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_ST_M  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_ST_V  0x1
    #define UART_TXFIFO_EMPTY_INT_ST_S  1
    /* UART_RXFIFO_FULL_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
    /*description: The interrupt state bit for RX fifo full event*/
    #define UART_RXFIFO_FULL_INT_ST  (BIT(0))
    #define UART_RXFIFO_FULL_INT_ST_M  (BIT(0))
    #define UART_RXFIFO_FULL_INT_ST_V  0x1
    #define UART_RXFIFO_FULL_INT_ST_S  0
        #define UART_GET_AT_CMD_CHAR_DET_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_AT_CMD_CHAR_DET_INT_ST_V, UART_AT_CMD_CHAR_DET_INT_ST_S)
        #define UART_GET_RS485_CLASH_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_RS485_CLASH_INT_ST_V, UART_RS485_CLASH_INT_ST_S)
        #define UART_GET_RS485_FRM_ERR_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_RS485_FRM_ERR_INT_ST_V, UART_RS485_FRM_ERR_INT_ST_S)
        #define UART_GET_RS485_PARITY_ERR_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_RS485_PARITY_ERR_INT_ST_V, UART_RS485_PARITY_ERR_INT_ST_S)
        #define UART_GET_TX_DONE_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_TX_DONE_INT_ST_V, UART_TX_DONE_INT_ST_S)
        #define UART_GET_TX_BRK_IDLE_DONE_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_TX_BRK_IDLE_DONE_INT_ST_V, UART_TX_BRK_IDLE_DONE_INT_ST_S)
        #define UART_GET_TX_BRK_DONE_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_TX_BRK_DONE_INT_ST_V, UART_TX_BRK_DONE_INT_ST_S)
        #define UART_GET_GLITCH_DET_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_GLITCH_DET_INT_ST_V, UART_GLITCH_DET_INT_ST_S)
        #define UART_GET_SW_XOFF_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_SW_XOFF_INT_ST_V, UART_SW_XOFF_INT_ST_S)
        #define UART_GET_SW_XON_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_SW_XON_INT_ST_V, UART_SW_XON_INT_ST_S)
        #define UART_GET_RXFIFO_TOUT_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_RXFIFO_TOUT_INT_ST_V, UART_RXFIFO_TOUT_INT_ST_S)
        #define UART_GET_BRK_DET_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_BRK_DET_INT_ST_V, UART_BRK_DET_INT_ST_S)
        #define UART_GET_CTS_CHG_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_CTS_CHG_INT_ST_V, UART_CTS_CHG_INT_ST_S)
        #define UART_GET_DSR_CHG_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_DSR_CHG_INT_ST_V, UART_DSR_CHG_INT_ST_S)
        #define UART_GET_RXFIFO_OVF_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_RXFIFO_OVF_INT_ST_V, UART_RXFIFO_OVF_INT_ST_S)
        #define UART_GET_FRM_ERR_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_FRM_ERR_INT_ST_V, UART_FRM_ERR_INT_ST_S)
        #define UART_GET_PARITY_ERR_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_PARITY_ERR_INT_ST_V, UART_PARITY_ERR_INT_ST_S)
        #define UART_GET_TXFIFO_EMPTY_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_TXFIFO_EMPTY_INT_ST_V, UART_TXFIFO_EMPTY_INT_ST_S)
        #define UART_GET_RXFIFO_FULL_INT_ST(i)  GET_PERI_REG_BITS2(UART_INT_ST_REG(i) , UART_RXFIFO_FULL_INT_ST_V, UART_RXFIFO_FULL_INT_ST_S)



#define UART_INT_ENA_REG(i)          (REG_UART_BASE(i) + 0xC)
    /* UART_AT_CMD_CHAR_DET_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for detecting AT_CMD char interrupt*/
    #define UART_AT_CMD_CHAR_DET_INT_ENA  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_ENA_M  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_ENA_V  0x1
    #define UART_AT_CMD_CHAR_DET_INT_ENA_S  18
    /* UART_RS485_CLASH_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rs485 tx rx clash interrrupt*/
    #define UART_RS485_CLASH_INT_ENA  (BIT(17))
    #define UART_RS485_CLASH_INT_ENA_M  (BIT(17))
    #define UART_RS485_CLASH_INT_ENA_V  0x1
    #define UART_RS485_CLASH_INT_ENA_S  17
    /* UART_RS485_FRM_ERR_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rs485 other rx error*/
    #define UART_RS485_FRM_ERR_INT_ENA  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_ENA_M  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_ENA_V  0x1
    #define UART_RS485_FRM_ERR_INT_ENA_S  16
    /* UART_RS485_PARITY_ERR_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rs485 parity error*/
    #define UART_RS485_PARITY_ERR_INT_ENA  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_ENA_M  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_ENA_V  0x1
    #define UART_RS485_PARITY_ERR_INT_ENA_S  15
    /* UART_TX_DONE_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for tx done interrupt*/
    #define UART_TX_DONE_INT_ENA  (BIT(14))
    #define UART_TX_DONE_INT_ENA_M  (BIT(14))
    #define UART_TX_DONE_INT_ENA_V  0x1
    #define UART_TX_DONE_INT_ENA_S  14
    /* UART_TX_BRK_IDLE_DONE_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for tx send idle done interrupt*/
    #define UART_TX_BRK_IDLE_DONE_INT_ENA  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_ENA_M  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_ENA_V  0x1
    #define UART_TX_BRK_IDLE_DONE_INT_ENA_S  13
    /* UART_TX_BRK_DONE_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for send 0 done interrupt*/
    #define UART_TX_BRK_DONE_INT_ENA  (BIT(12))
    #define UART_TX_BRK_DONE_INT_ENA_M  (BIT(12))
    #define UART_TX_BRK_DONE_INT_ENA_V  0x1
    #define UART_TX_BRK_DONE_INT_ENA_S  12
    /* UART_GLITCH_DET_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for detecting rx glitch pulse interrupt*/
    #define UART_GLITCH_DET_INT_ENA  (BIT(11))
    #define UART_GLITCH_DET_INT_ENA_M  (BIT(11))
    #define UART_GLITCH_DET_INT_ENA_V  0x1
    #define UART_GLITCH_DET_INT_ENA_S  11
    /* UART_SW_XOFF_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rx receive xoff char interrupt*/
    #define UART_SW_XOFF_INT_ENA  (BIT(10))
    #define UART_SW_XOFF_INT_ENA_M  (BIT(10))
    #define UART_SW_XOFF_INT_ENA_V  0x1
    #define UART_SW_XOFF_INT_ENA_S  10
    /* UART_SW_XON_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rx receive xon char interrupt*/
    #define UART_SW_XON_INT_ENA  (BIT(9))
    #define UART_SW_XON_INT_ENA_M  (BIT(9))
    #define UART_SW_XON_INT_ENA_V  0x1
    #define UART_SW_XON_INT_ENA_S  9
    /* UART_RXFIFO_TOUT_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rx time-out interrupt*/
    #define UART_RXFIFO_TOUT_INT_ENA  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_ENA_M  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_ENA_V  0x1
    #define UART_RXFIFO_TOUT_INT_ENA_S  8
    /* UART_BRK_DET_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rx byte start error*/
    #define UART_BRK_DET_INT_ENA  (BIT(7))
    #define UART_BRK_DET_INT_ENA_M  (BIT(7))
    #define UART_BRK_DET_INT_ENA_V  0x1
    #define UART_BRK_DET_INT_ENA_S  7
    /* UART_CTS_CHG_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for CTS changing level*/
    #define UART_CTS_CHG_INT_ENA  (BIT(6))
    #define UART_CTS_CHG_INT_ENA_M  (BIT(6))
    #define UART_CTS_CHG_INT_ENA_V  0x1
    #define UART_CTS_CHG_INT_ENA_S  6
    /* UART_DSR_CHG_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for DSR changing level*/
    #define UART_DSR_CHG_INT_ENA  (BIT(5))
    #define UART_DSR_CHG_INT_ENA_M  (BIT(5))
    #define UART_DSR_CHG_INT_ENA_V  0x1
    #define UART_DSR_CHG_INT_ENA_S  5
    /* UART_RXFIFO_OVF_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rx fifo overflow*/
    #define UART_RXFIFO_OVF_INT_ENA  (BIT(4))
    #define UART_RXFIFO_OVF_INT_ENA_M  (BIT(4))
    #define UART_RXFIFO_OVF_INT_ENA_V  0x1
    #define UART_RXFIFO_OVF_INT_ENA_S  4
    /* UART_FRM_ERR_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for other rx error*/
    #define UART_FRM_ERR_INT_ENA  (BIT(3))
    #define UART_FRM_ERR_INT_ENA_M  (BIT(3))
    #define UART_FRM_ERR_INT_ENA_V  0x1
    #define UART_FRM_ERR_INT_ENA_S  3
    /* UART_PARITY_ERR_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for parity error*/
    #define UART_PARITY_ERR_INT_ENA  (BIT(2))
    #define UART_PARITY_ERR_INT_ENA_M  (BIT(2))
    #define UART_PARITY_ERR_INT_ENA_V  0x1
    #define UART_PARITY_ERR_INT_ENA_S  2
    /* UART_TXFIFO_EMPTY_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for tx fifo empty event*/
    #define UART_TXFIFO_EMPTY_INT_ENA  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_ENA_M  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_ENA_V  0x1
    #define UART_TXFIFO_EMPTY_INT_ENA_S  1
    /* UART_RXFIFO_FULL_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
    /*description: The interrupt enable bit for rx fifo full event*/
    #define UART_RXFIFO_FULL_INT_ENA  (BIT(0))
    #define UART_RXFIFO_FULL_INT_ENA_M  (BIT(0))
    #define UART_RXFIFO_FULL_INT_ENA_V  0x1
    #define UART_RXFIFO_FULL_INT_ENA_S  0
        #define UART_GET_AT_CMD_CHAR_DET_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_AT_CMD_CHAR_DET_INT_ENA_V, UART_AT_CMD_CHAR_DET_INT_ENA_S)
        #define UART_SET_AT_CMD_CHAR_DET_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_AT_CMD_CHAR_DET_INT_ENA_V,(val),UART_AT_CMD_CHAR_DET_INT_ENA_S)
        #define UART_GET_RS485_CLASH_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_RS485_CLASH_INT_ENA_V, UART_RS485_CLASH_INT_ENA_S)
        #define UART_SET_RS485_CLASH_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_RS485_CLASH_INT_ENA_V,(val),UART_RS485_CLASH_INT_ENA_S)
        #define UART_GET_RS485_FRM_ERR_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_RS485_FRM_ERR_INT_ENA_V, UART_RS485_FRM_ERR_INT_ENA_S)
        #define UART_SET_RS485_FRM_ERR_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_RS485_FRM_ERR_INT_ENA_V,(val),UART_RS485_FRM_ERR_INT_ENA_S)
        #define UART_GET_RS485_PARITY_ERR_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_RS485_PARITY_ERR_INT_ENA_V, UART_RS485_PARITY_ERR_INT_ENA_S)
        #define UART_SET_RS485_PARITY_ERR_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_RS485_PARITY_ERR_INT_ENA_V,(val),UART_RS485_PARITY_ERR_INT_ENA_S)
        #define UART_GET_TX_DONE_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_TX_DONE_INT_ENA_V, UART_TX_DONE_INT_ENA_S)
        #define UART_SET_TX_DONE_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_TX_DONE_INT_ENA_V,(val),UART_TX_DONE_INT_ENA_S)
        #define UART_GET_TX_BRK_IDLE_DONE_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_TX_BRK_IDLE_DONE_INT_ENA_V, UART_TX_BRK_IDLE_DONE_INT_ENA_S)
        #define UART_SET_TX_BRK_IDLE_DONE_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_TX_BRK_IDLE_DONE_INT_ENA_V,(val),UART_TX_BRK_IDLE_DONE_INT_ENA_S)
        #define UART_GET_TX_BRK_DONE_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_TX_BRK_DONE_INT_ENA_V, UART_TX_BRK_DONE_INT_ENA_S)
        #define UART_SET_TX_BRK_DONE_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_TX_BRK_DONE_INT_ENA_V,(val),UART_TX_BRK_DONE_INT_ENA_S)
        #define UART_GET_GLITCH_DET_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_GLITCH_DET_INT_ENA_V, UART_GLITCH_DET_INT_ENA_S)
        #define UART_SET_GLITCH_DET_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_GLITCH_DET_INT_ENA_V,(val),UART_GLITCH_DET_INT_ENA_S)
        #define UART_GET_SW_XOFF_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_SW_XOFF_INT_ENA_V, UART_SW_XOFF_INT_ENA_S)
        #define UART_SET_SW_XOFF_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_SW_XOFF_INT_ENA_V,(val),UART_SW_XOFF_INT_ENA_S)
        #define UART_GET_SW_XON_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_SW_XON_INT_ENA_V, UART_SW_XON_INT_ENA_S)
        #define UART_SET_SW_XON_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_SW_XON_INT_ENA_V,(val),UART_SW_XON_INT_ENA_S)
        #define UART_GET_RXFIFO_TOUT_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_RXFIFO_TOUT_INT_ENA_V, UART_RXFIFO_TOUT_INT_ENA_S)
        #define UART_SET_RXFIFO_TOUT_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_RXFIFO_TOUT_INT_ENA_V,(val),UART_RXFIFO_TOUT_INT_ENA_S)
        #define UART_GET_BRK_DET_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_BRK_DET_INT_ENA_V, UART_BRK_DET_INT_ENA_S)
        #define UART_SET_BRK_DET_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_BRK_DET_INT_ENA_V,(val),UART_BRK_DET_INT_ENA_S)
        #define UART_GET_CTS_CHG_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_CTS_CHG_INT_ENA_V, UART_CTS_CHG_INT_ENA_S)
        #define UART_SET_CTS_CHG_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_CTS_CHG_INT_ENA_V,(val),UART_CTS_CHG_INT_ENA_S)
        #define UART_GET_DSR_CHG_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_DSR_CHG_INT_ENA_V, UART_DSR_CHG_INT_ENA_S)
        #define UART_SET_DSR_CHG_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_DSR_CHG_INT_ENA_V,(val),UART_DSR_CHG_INT_ENA_S)
        #define UART_GET_RXFIFO_OVF_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_RXFIFO_OVF_INT_ENA_V, UART_RXFIFO_OVF_INT_ENA_S)
        #define UART_SET_RXFIFO_OVF_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_RXFIFO_OVF_INT_ENA_V,(val),UART_RXFIFO_OVF_INT_ENA_S)
        #define UART_GET_FRM_ERR_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_FRM_ERR_INT_ENA_V, UART_FRM_ERR_INT_ENA_S)
        #define UART_SET_FRM_ERR_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_FRM_ERR_INT_ENA_V,(val),UART_FRM_ERR_INT_ENA_S)
        #define UART_GET_PARITY_ERR_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_PARITY_ERR_INT_ENA_V, UART_PARITY_ERR_INT_ENA_S)
        #define UART_SET_PARITY_ERR_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_PARITY_ERR_INT_ENA_V,(val),UART_PARITY_ERR_INT_ENA_S)
        #define UART_GET_TXFIFO_EMPTY_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_TXFIFO_EMPTY_INT_ENA_V, UART_TXFIFO_EMPTY_INT_ENA_S)
        #define UART_SET_TXFIFO_EMPTY_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_TXFIFO_EMPTY_INT_ENA_V,(val),UART_TXFIFO_EMPTY_INT_ENA_S)
        #define UART_GET_RXFIFO_FULL_INT_ENA(i)  GET_PERI_REG_BITS2(UART_INT_ENA_REG(i) , UART_RXFIFO_FULL_INT_ENA_V, UART_RXFIFO_FULL_INT_ENA_S)
        #define UART_SET_RXFIFO_FULL_INT_ENA(i,val)  SET_PERI_REG_BITS(UART_INT_ENA_REG(i) ,UART_RXFIFO_FULL_INT_ENA_V,(val),UART_RXFIFO_FULL_INT_ENA_S)



#define UART_INT_CLR_REG(i)          (REG_UART_BASE(i) + 0x10)
    /* UART_AT_CMD_CHAR_DET_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
    /*description: Set this bit for detecting AT_CMD char interrupt*/
    #define UART_AT_CMD_CHAR_DET_INT_CLR  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_CLR_M  (BIT(18))
    #define UART_AT_CMD_CHAR_DET_INT_CLR_V  0x1
    #define UART_AT_CMD_CHAR_DET_INT_CLR_S  18
    /* UART_RS485_CLASH_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
    /*description: Set this bit for rs485 tx rx clash interrrupt*/
    #define UART_RS485_CLASH_INT_CLR  (BIT(17))
    #define UART_RS485_CLASH_INT_CLR_M  (BIT(17))
    #define UART_RS485_CLASH_INT_CLR_V  0x1
    #define UART_RS485_CLASH_INT_CLR_S  17
    /* UART_RS485_FRM_ERR_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
    /*description: Set this bit for rs485 other rx error*/
    #define UART_RS485_FRM_ERR_INT_CLR  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_CLR_M  (BIT(16))
    #define UART_RS485_FRM_ERR_INT_CLR_V  0x1
    #define UART_RS485_FRM_ERR_INT_CLR_S  16
    /* UART_RS485_PARITY_ERR_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
    /*description: Set this bit for rs485 parity error interrupt*/
    #define UART_RS485_PARITY_ERR_INT_CLR  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_CLR_M  (BIT(15))
    #define UART_RS485_PARITY_ERR_INT_CLR_V  0x1
    #define UART_RS485_PARITY_ERR_INT_CLR_S  15
    /* UART_TX_DONE_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
    /*description: Set this bit to clear the  tx done interrupt*/
    #define UART_TX_DONE_INT_CLR  (BIT(14))
    #define UART_TX_DONE_INT_CLR_M  (BIT(14))
    #define UART_TX_DONE_INT_CLR_V  0x1
    #define UART_TX_DONE_INT_CLR_S  14
    /* UART_TX_BRK_IDLE_DONE_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
    /*description: Set this bit to clear the  tx send idle done interrupt*/
    #define UART_TX_BRK_IDLE_DONE_INT_CLR  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_CLR_M  (BIT(13))
    #define UART_TX_BRK_IDLE_DONE_INT_CLR_V  0x1
    #define UART_TX_BRK_IDLE_DONE_INT_CLR_S  13
    /* UART_TX_BRK_DONE_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
    /*description: Set this bit to clear the  send 0 done interrupt*/
    #define UART_TX_BRK_DONE_INT_CLR  (BIT(12))
    #define UART_TX_BRK_DONE_INT_CLR_M  (BIT(12))
    #define UART_TX_BRK_DONE_INT_CLR_V  0x1
    #define UART_TX_BRK_DONE_INT_CLR_S  12
    /* UART_GLITCH_DET_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
    /*description: Set this bit to clear the  detecting rx glitch pulse interrupt*/
    #define UART_GLITCH_DET_INT_CLR  (BIT(11))
    #define UART_GLITCH_DET_INT_CLR_M  (BIT(11))
    #define UART_GLITCH_DET_INT_CLR_V  0x1
    #define UART_GLITCH_DET_INT_CLR_S  11
    /* UART_SW_XOFF_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
    /*description: Set this bit to clear the  rx receive xoff char interrupt*/
    #define UART_SW_XOFF_INT_CLR  (BIT(10))
    #define UART_SW_XOFF_INT_CLR_M  (BIT(10))
    #define UART_SW_XOFF_INT_CLR_V  0x1
    #define UART_SW_XOFF_INT_CLR_S  10
    /* UART_SW_XON_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
    /*description: Set this bit to clear the  rx receive xon char interrupt*/
    #define UART_SW_XON_INT_CLR  (BIT(9))
    #define UART_SW_XON_INT_CLR_M  (BIT(9))
    #define UART_SW_XON_INT_CLR_V  0x1
    #define UART_SW_XON_INT_CLR_S  9
    /* UART_RXFIFO_TOUT_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
    /*description: Set this bit to clear the rx time-out interrupt*/
    #define UART_RXFIFO_TOUT_INT_CLR  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_CLR_M  (BIT(8))
    #define UART_RXFIFO_TOUT_INT_CLR_V  0x1
    #define UART_RXFIFO_TOUT_INT_CLR_S  8
    /* UART_BRK_DET_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
    /*description: Set this bit to clear the rx byte start interrupt*/
    #define UART_BRK_DET_INT_CLR  (BIT(7))
    #define UART_BRK_DET_INT_CLR_M  (BIT(7))
    #define UART_BRK_DET_INT_CLR_V  0x1
    #define UART_BRK_DET_INT_CLR_S  7
    /* UART_CTS_CHG_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
    /*description: Set this bit to clear the CTS changing interrupt*/
    #define UART_CTS_CHG_INT_CLR  (BIT(6))
    #define UART_CTS_CHG_INT_CLR_M  (BIT(6))
    #define UART_CTS_CHG_INT_CLR_V  0x1
    #define UART_CTS_CHG_INT_CLR_S  6
    /* UART_DSR_CHG_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
    /*description: Set this bit to clear the DSR changing interrupt*/
    #define UART_DSR_CHG_INT_CLR  (BIT(5))
    #define UART_DSR_CHG_INT_CLR_M  (BIT(5))
    #define UART_DSR_CHG_INT_CLR_V  0x1
    #define UART_DSR_CHG_INT_CLR_S  5
    /* UART_RXFIFO_OVF_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
    /*description: Set this bit to clear the rx fifo over-flow interrupt*/
    #define UART_RXFIFO_OVF_INT_CLR  (BIT(4))
    #define UART_RXFIFO_OVF_INT_CLR_M  (BIT(4))
    #define UART_RXFIFO_OVF_INT_CLR_V  0x1
    #define UART_RXFIFO_OVF_INT_CLR_S  4
    /* UART_FRM_ERR_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
    /*description: Set this bit to clear other rx error interrupt*/
    #define UART_FRM_ERR_INT_CLR  (BIT(3))
    #define UART_FRM_ERR_INT_CLR_M  (BIT(3))
    #define UART_FRM_ERR_INT_CLR_V  0x1
    #define UART_FRM_ERR_INT_CLR_S  3
    /* UART_PARITY_ERR_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
    /*description: Set this bit to clear the parity error interrupt*/
    #define UART_PARITY_ERR_INT_CLR  (BIT(2))
    #define UART_PARITY_ERR_INT_CLR_M  (BIT(2))
    #define UART_PARITY_ERR_INT_CLR_V  0x1
    #define UART_PARITY_ERR_INT_CLR_S  2
    /* UART_TXFIFO_EMPTY_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
    /*description: Set this bit to clear the tx fifo empty interrupt*/
    #define UART_TXFIFO_EMPTY_INT_CLR  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_CLR_M  (BIT(1))
    #define UART_TXFIFO_EMPTY_INT_CLR_V  0x1
    #define UART_TXFIFO_EMPTY_INT_CLR_S  1
    /* UART_RXFIFO_FULL_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
    /*description: Set this bit to clear the rx fifo full interrupt*/
    #define UART_RXFIFO_FULL_INT_CLR  (BIT(0))
    #define UART_RXFIFO_FULL_INT_CLR_M  (BIT(0))
    #define UART_RXFIFO_FULL_INT_CLR_V  0x1
    #define UART_RXFIFO_FULL_INT_CLR_S  0
        #define UART_SET_AT_CMD_CHAR_DET_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_AT_CMD_CHAR_DET_INT_CLR_V,(val),UART_AT_CMD_CHAR_DET_INT_CLR_S)
        #define UART_SET_RS485_CLASH_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_RS485_CLASH_INT_CLR_V,(val),UART_RS485_CLASH_INT_CLR_S)
        #define UART_SET_RS485_FRM_ERR_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_RS485_FRM_ERR_INT_CLR_V,(val),UART_RS485_FRM_ERR_INT_CLR_S)
        #define UART_SET_RS485_PARITY_ERR_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_RS485_PARITY_ERR_INT_CLR_V,(val),UART_RS485_PARITY_ERR_INT_CLR_S)
        #define UART_SET_TX_DONE_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_TX_DONE_INT_CLR_V,(val),UART_TX_DONE_INT_CLR_S)
        #define UART_SET_TX_BRK_IDLE_DONE_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_TX_BRK_IDLE_DONE_INT_CLR_V,(val),UART_TX_BRK_IDLE_DONE_INT_CLR_S)
        #define UART_SET_TX_BRK_DONE_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_TX_BRK_DONE_INT_CLR_V,(val),UART_TX_BRK_DONE_INT_CLR_S)
        #define UART_SET_GLITCH_DET_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_GLITCH_DET_INT_CLR_V,(val),UART_GLITCH_DET_INT_CLR_S)
        #define UART_SET_SW_XOFF_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_SW_XOFF_INT_CLR_V,(val),UART_SW_XOFF_INT_CLR_S)
        #define UART_SET_SW_XON_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_SW_XON_INT_CLR_V,(val),UART_SW_XON_INT_CLR_S)
        #define UART_SET_RXFIFO_TOUT_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_RXFIFO_TOUT_INT_CLR_V,(val),UART_RXFIFO_TOUT_INT_CLR_S)
        #define UART_SET_BRK_DET_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_BRK_DET_INT_CLR_V,(val),UART_BRK_DET_INT_CLR_S)
        #define UART_SET_CTS_CHG_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_CTS_CHG_INT_CLR_V,(val),UART_CTS_CHG_INT_CLR_S)
        #define UART_SET_DSR_CHG_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_DSR_CHG_INT_CLR_V,(val),UART_DSR_CHG_INT_CLR_S)
        #define UART_SET_RXFIFO_OVF_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_RXFIFO_OVF_INT_CLR_V,(val),UART_RXFIFO_OVF_INT_CLR_S)
        #define UART_SET_FRM_ERR_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_FRM_ERR_INT_CLR_V,(val),UART_FRM_ERR_INT_CLR_S)
        #define UART_SET_PARITY_ERR_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_PARITY_ERR_INT_CLR_V,(val),UART_PARITY_ERR_INT_CLR_S)
        #define UART_SET_TXFIFO_EMPTY_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_TXFIFO_EMPTY_INT_CLR_V,(val),UART_TXFIFO_EMPTY_INT_CLR_S)
        #define UART_SET_RXFIFO_FULL_INT_CLR(i,val)  SET_PERI_REG_BITS(UART_INT_CLR_REG(i) ,UART_RXFIFO_FULL_INT_CLR_V,(val),UART_RXFIFO_FULL_INT_CLR_S)



#define UART_CLKDIV_REG(i)          (REG_UART_BASE(i) + 0x14)
    /* UART_CLKDIV_FRAG : R/W ;bitpos:[23:20] ;default: 4'b0 ; */
    /*description: The decimal part of divider*/
    #define UART_CLKDIV_FRAG  0x0000000F
    #define UART_CLKDIV_FRAG_M  ((UART_CLKDIV_FRAG_V)<<(UART_CLKDIV_FRAG_S))
    #define UART_CLKDIV_FRAG_V  0xF
    #define UART_CLKDIV_FRAG_S  20
    /* UART_CLKDIV : R/W ;bitpos:[19:0] ;default: 20'h2B6 ; */
    /*description: The interger part of divider  BAUDRATE = UART_CLK_FREQ / UART_CLKDIV*/
    #define UART_CLKDIV  0x000FFFFF
    #define UART_CLKDIV_M  ((UART_CLKDIV_V)<<(UART_CLKDIV_S))
    #define UART_CLKDIV_V  0xFFFFF
    #define UART_CLKDIV_S  0
        #define UART_GET_CLKDIV_FRAG(i)  GET_PERI_REG_BITS2(UART_CLKDIV_REG(i) , UART_CLKDIV_FRAG_V, UART_CLKDIV_FRAG_S)
        #define UART_SET_CLKDIV_FRAG(i,val)  SET_PERI_REG_BITS(UART_CLKDIV_REG(i) ,UART_CLKDIV_FRAG_V,(val),UART_CLKDIV_FRAG_S)
        #define UART_GET_CLKDIV(i)  GET_PERI_REG_BITS2(UART_CLKDIV_REG(i) , UART_CLKDIV_V, UART_CLKDIV_S)
        #define UART_SET_CLKDIV(i,val)  SET_PERI_REG_BITS(UART_CLKDIV_REG(i) ,UART_CLKDIV_V,(val),UART_CLKDIV_S)



#define UART_AUTOBAUD_REG(i)          (REG_UART_BASE(i) + 0x18)
    /* UART_GLITCH_FILT : R/W ;bitpos:[15:8] ;default: 8'h10 ; */
    /*description: ignore the glitch when pulse cnt is lower then this value*/
    #define UART_GLITCH_FILT  0x000000FF
    #define UART_GLITCH_FILT_M  ((UART_GLITCH_FILT_V)<<(UART_GLITCH_FILT_S))
    #define UART_GLITCH_FILT_V  0xFF
    #define UART_GLITCH_FILT_S  8
    /* UART_AUTOBAUD_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
    /*description: Set this bit to enable baudrate detect*/
    #define UART_AUTOBAUD_EN  (BIT(0))
    #define UART_AUTOBAUD_EN_M  (BIT(0))
    #define UART_AUTOBAUD_EN_V  0x1
    #define UART_AUTOBAUD_EN_S  0
        #define UART_GET_GLITCH_FILT(i)  GET_PERI_REG_BITS2(UART_AUTOBAUD_REG(i) , UART_GLITCH_FILT_V, UART_GLITCH_FILT_S)
        #define UART_SET_GLITCH_FILT(i,val)  SET_PERI_REG_BITS(UART_AUTOBAUD_REG(i) ,UART_GLITCH_FILT_V,(val),UART_GLITCH_FILT_S)
        #define UART_GET_AUTOBAUD_EN(i)  GET_PERI_REG_BITS2(UART_AUTOBAUD_REG(i) , UART_AUTOBAUD_EN_V, UART_AUTOBAUD_EN_S)
        #define UART_SET_AUTOBAUD_EN(i,val)  SET_PERI_REG_BITS(UART_AUTOBAUD_REG(i) ,UART_AUTOBAUD_EN_V,(val),UART_AUTOBAUD_EN_S)



#define UART_STATUS_REG(i)          (REG_UART_BASE(i) + 0x1C)
    /* UART_TXD : RO ;bitpos:[31] ;default: 8'h0 ; */
    /*description: The level of the internal uart txd value*/
    #define UART_TXD  (BIT(31))
    #define UART_TXD_M  (BIT(31))
    #define UART_TXD_V  0x1
    #define UART_TXD_S  31
    /* UART_RTSN : RO ;bitpos:[30] ;default: 1'b0 ; */
    /*description: The level of the internal uart rts pin*/
    #define UART_RTSN  (BIT(30))
    #define UART_RTSN_M  (BIT(30))
    #define UART_RTSN_V  0x1
    #define UART_RTSN_S  30
    /* UART_DTRN : RO ;bitpos:[29] ;default: 1'b0 ; */
    /*description: The level of the internaluart dtr pin*/
    #define UART_DTRN  (BIT(29))
    #define UART_DTRN_M  (BIT(29))
    #define UART_DTRN_V  0x1
    #define UART_DTRN_S  29
    /* UART_ST_UTX_OUT : RO ;bitpos:[27:24] ;default: 4'b0 ; */
    /*description: The state for transmitting data. 4'd0:TX_IDLE*/
    #define UART_ST_UTX_OUT  0x0000000F
    #define UART_ST_UTX_OUT_M  ((UART_ST_UTX_OUT_V)<<(UART_ST_UTX_OUT_S))
    #define UART_ST_UTX_OUT_V  0xF
    #define UART_ST_UTX_OUT_S  24
    /* UART_TXFIFO_CNT : RO ;bitpos:[23:16] ;default: 8'b0 ; */
    /*description: Number of data in UART TX fifo*/
    #define UART_TXFIFO_CNT  0x000000FF
    #define UART_TXFIFO_CNT_M  ((UART_TXFIFO_CNT_V)<<(UART_TXFIFO_CNT_S))
    #define UART_TXFIFO_CNT_V  0xFF
    #define UART_TXFIFO_CNT_S  16
    /* UART_RXD : RO ;bitpos:[15] ;default: 1'b0 ; */
    /*description: The level of the internal uart rxd value*/
    #define UART_RXD  (BIT(15))
    #define UART_RXD_M  (BIT(15))
    #define UART_RXD_V  0x1
    #define UART_RXD_S  15
    /* UART_CTSN : RO ;bitpos:[14] ;default: 1'b0 ; */
    /*description: The level of the internal uart cts value*/
    #define UART_CTSN  (BIT(14))
    #define UART_CTSN_M  (BIT(14))
    #define UART_CTSN_V  0x1
    #define UART_CTSN_S  14
    /* UART_DSRN : RO ;bitpos:[13] ;default: 1'b0 ; */
    /*description: The level of the internal uart dsr value*/
    #define UART_DSRN  (BIT(13))
    #define UART_DSRN_M  (BIT(13))
    #define UART_DSRN_V  0x1
    #define UART_DSRN_S  13
    /* UART_ST_URX_OUT : RO ;bitpos:[11:8] ;default: 4'b0 ; */
    /*description: The state for receiving data. 4‘d0: RX_IDLE*/
    #define UART_ST_URX_OUT  0x0000000F
    #define UART_ST_URX_OUT_M  ((UART_ST_URX_OUT_V)<<(UART_ST_URX_OUT_S))
    #define UART_ST_URX_OUT_V  0xF
    #define UART_ST_URX_OUT_S  8
    /* UART_RXFIFO_CNT : RO ;bitpos:[7:0] ;default: 8'b0 ; */
    /*description: Number of data in UART RX fifo*/
    #define UART_RXFIFO_CNT  0x000000FF
    #define UART_RXFIFO_CNT_M  ((UART_RXFIFO_CNT_V)<<(UART_RXFIFO_CNT_S))
    #define UART_RXFIFO_CNT_V  0xFF
    #define UART_RXFIFO_CNT_S  0
        #define UART_GET_TXD(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_TXD_V, UART_TXD_S)
        #define UART_GET_RTSN(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_RTSN_V, UART_RTSN_S)
        #define UART_GET_DTRN(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_DTRN_V, UART_DTRN_S)
        #define UART_GET_ST_UTX_OUT(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_ST_UTX_OUT_V, UART_ST_UTX_OUT_S)
        #define UART_GET_TXFIFO_CNT(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_TXFIFO_CNT_V, UART_TXFIFO_CNT_S)
        #define UART_GET_RXD(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_RXD_V, UART_RXD_S)
        #define UART_GET_CTSN(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_CTSN_V, UART_CTSN_S)
        #define UART_GET_DSRN(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_DSRN_V, UART_DSRN_S)
        #define UART_GET_ST_URX_OUT(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_ST_URX_OUT_V, UART_ST_URX_OUT_S)
        #define UART_GET_RXFIFO_CNT(i)  GET_PERI_REG_BITS2(UART_STATUS_REG(i) , UART_RXFIFO_CNT_V, UART_RXFIFO_CNT_S)



#define UART_CONF0_REG(i)          (REG_UART_BASE(i) + 0x20)
    /* UART_TICK_REF_ALWAYS_ON : R/W ;bitpos:[27] ;default: 1'b1 ; */
    /*description: 1：use apb clock：use referrence clock*/
    #define UART_TICK_REF_ALWAYS_ON  (BIT(27))
    #define UART_TICK_REF_ALWAYS_ON_M  (BIT(27))
    #define UART_TICK_REF_ALWAYS_ON_V  0x1
    #define UART_TICK_REF_ALWAYS_ON_S  27
    /* UART_ERR_WR_MASK : R/W ;bitpos:[26] ;default: 1'b0 ; */
    /*description: 1：rx stop storing data int rxfifo when data is error 0：if received data is wrong*/
    #define UART_ERR_WR_MASK  (BIT(26))
    #define UART_ERR_WR_MASK_M  (BIT(26))
    #define UART_ERR_WR_MASK_V  0x1
    #define UART_ERR_WR_MASK_S  26
    /* UART_CLK_EN : R/W ;bitpos:[25] ;default: 1'h0 ; */
    /*description: 1：force clock on for registers：support clock only when write registers*/
    #define UART_CLK_EN  (BIT(25))
    #define UART_CLK_EN_M  (BIT(25))
    #define UART_CLK_EN_V  0x1
    #define UART_CLK_EN_S  25
    /* UART_DTR_INV : R/W ;bitpos:[24] ;default: 1'h0 ; */
    /*description: Set this bit to inverse uart dtr level*/
    #define UART_DTR_INV  (BIT(24))
    #define UART_DTR_INV_M  (BIT(24))
    #define UART_DTR_INV_V  0x1
    #define UART_DTR_INV_S  24
    /* UART_RTS_INV : R/W ;bitpos:[23] ;default: 1'h0 ; */
    /*description: Set this bit to inverse uart rts level*/
    #define UART_RTS_INV  (BIT(23))
    #define UART_RTS_INV_M  (BIT(23))
    #define UART_RTS_INV_V  0x1
    #define UART_RTS_INV_S  23
    /* UART_TXD_INV : R/W ;bitpos:[22] ;default: 1'h0 ; */
    /*description: Set this bit to inverse uart txd level*/
    #define UART_TXD_INV  (BIT(22))
    #define UART_TXD_INV_M  (BIT(22))
    #define UART_TXD_INV_V  0x1
    #define UART_TXD_INV_S  22
    /* UART_DSR_INV : R/W ;bitpos:[21] ;default: 1'h0 ; */
    /*description: Set this bit to inverse uart dsr level*/
    #define UART_DSR_INV  (BIT(21))
    #define UART_DSR_INV_M  (BIT(21))
    #define UART_DSR_INV_V  0x1
    #define UART_DSR_INV_S  21
    /* UART_CTS_INV : R/W ;bitpos:[20] ;default: 1'h0 ; */
    /*description: Set this bit to inverse uart cts level*/
    #define UART_CTS_INV  (BIT(20))
    #define UART_CTS_INV_M  (BIT(20))
    #define UART_CTS_INV_V  0x1
    #define UART_CTS_INV_S  20
    /* UART_RXD_INV : R/W ;bitpos:[19] ;default: 1'h0 ; */
    /*description: Set this bit to inverse uart rxd level*/
    #define UART_RXD_INV  (BIT(19))
    #define UART_RXD_INV_M  (BIT(19))
    #define UART_RXD_INV_V  0x1
    #define UART_RXD_INV_S  19
    /* UART_TXFIFO_RST : R/W ;bitpos:[18] ;default: 1'h0 ; */
    /*description: Set this bit to reset uart tx fifo*/
    #define UART_TXFIFO_RST  (BIT(18))
    #define UART_TXFIFO_RST_M  (BIT(18))
    #define UART_TXFIFO_RST_V  0x1
    #define UART_TXFIFO_RST_S  18
    /* UART_RXFIFO_RST : R/W ;bitpos:[17] ;default: 1'h0 ; */
    /*description: Set this bit to reset uart rx fifo*/
    #define UART_RXFIFO_RST  (BIT(17))
    #define UART_RXFIFO_RST_M  (BIT(17))
    #define UART_RXFIFO_RST_V  0x1
    #define UART_RXFIFO_RST_S  17
    /* UART_IRDA_EN : R/W ;bitpos:[16] ;default: 1'h0 ; */
    /*description: Set this bit to enable irda protocol*/
    #define UART_IRDA_EN  (BIT(16))
    #define UART_IRDA_EN_M  (BIT(16))
    #define UART_IRDA_EN_V  0x1
    #define UART_IRDA_EN_S  16
    /* UART_TX_FLOW_EN : R/W ;bitpos:[15] ;default: 1'b0 ; */
    /*description: Set this bit to enable tx flow control*/
    #define UART_TX_FLOW_EN  (BIT(15))
    #define UART_TX_FLOW_EN_M  (BIT(15))
    #define UART_TX_FLOW_EN_V  0x1
    #define UART_TX_FLOW_EN_S  15
    /* UART_LOOPBACK : R/W ;bitpos:[14] ;default: 1'b0 ; */
    /*description: Set this bit to enable uart loopback test mode*/
    #define UART_LOOPBACK  (BIT(14))
    #define UART_LOOPBACK_M  (BIT(14))
    #define UART_LOOPBACK_V  0x1
    #define UART_LOOPBACK_S  14
    /* UART_IRDA_RX_INV : R/W ;bitpos:[13] ;default: 1'b0 ; */
    /*description: Set this bit to inverse irda rx level*/
    #define UART_IRDA_RX_INV  (BIT(13))
    #define UART_IRDA_RX_INV_M  (BIT(13))
    #define UART_IRDA_RX_INV_V  0x1
    #define UART_IRDA_RX_INV_S  13
    /* UART_IRDA_TX_INV : R/W ;bitpos:[12] ;default: 1'b0 ; */
    /*description: Set this bit to inverse irda tx level*/
    #define UART_IRDA_TX_INV  (BIT(12))
    #define UART_IRDA_TX_INV_M  (BIT(12))
    #define UART_IRDA_TX_INV_V  0x1
    #define UART_IRDA_TX_INV_S  12
    /* UART_IRDA_WCTL : R/W ;bitpos:[11] ;default: 1'b0 ; */
    /*description: 1：the tx 11th bit is the same to the 10th bit 0：set tx 11th bit to 0*/
    #define UART_IRDA_WCTL  (BIT(11))
    #define UART_IRDA_WCTL_M  (BIT(11))
    #define UART_IRDA_WCTL_V  0x1
    #define UART_IRDA_WCTL_S  11
    /* UART_IRDA_TX_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
    /*description: Set this bit to enable irda to send data*/
    #define UART_IRDA_TX_EN  (BIT(10))
    #define UART_IRDA_TX_EN_M  (BIT(10))
    #define UART_IRDA_TX_EN_V  0x1
    #define UART_IRDA_TX_EN_S  10
    /* UART_IRDA_DPLX : R/W ;bitpos:[9] ;default: 1'b0 ; */
    /*description: Set this bit to enable irda duplex mode*/
    #define UART_IRDA_DPLX  (BIT(9))
    #define UART_IRDA_DPLX_M  (BIT(9))
    #define UART_IRDA_DPLX_V  0x1
    #define UART_IRDA_DPLX_S  9
    /* UART_TXD_BRK : R/W ;bitpos:[8] ;default: 1'b0 ; */
    /*description: Set this bit to enbale tx send 0*/
    #define UART_TXD_BRK  (BIT(8))
    #define UART_TXD_BRK_M  (BIT(8))
    #define UART_TXD_BRK_V  0x1
    #define UART_TXD_BRK_S  8
    /* UART_SW_DTR : R/W ;bitpos:[7] ;default: 1'b0 ; */
    /*description: sw dtr*/
    #define UART_SW_DTR  (BIT(7))
    #define UART_SW_DTR_M  (BIT(7))
    #define UART_SW_DTR_V  0x1
    #define UART_SW_DTR_S  7
    /* UART_SW_RTS : R/W ;bitpos:[6] ;default: 1'b0 ; */
    /*description: sw rts*/
    #define UART_SW_RTS  (BIT(6))
    #define UART_SW_RTS_M  (BIT(6))
    #define UART_SW_RTS_V  0x1
    #define UART_SW_RTS_S  6
    /* UART_STOP_BIT_NUM : R/W ;bitpos:[5:4] ;default: 2'd1 ; */
    /*description: Set stop bit: 1:1bit  2:1.5bits  3:2bits*/
    #define UART_STOP_BIT_NUM  0x00000003
    #define UART_STOP_BIT_NUM_M  ((UART_STOP_BIT_NUM_V)<<(UART_STOP_BIT_NUM_S))
    #define UART_STOP_BIT_NUM_V  0x3
    #define UART_STOP_BIT_NUM_S  4
    /* UART_BIT_NUM : R/W ;bitpos:[3:2] ;default: 2'd3 ; */
    /*description: Set bit num:  0:5bits 1:6bits 2:7bits 3:8bits*/
    #define UART_BIT_NUM  0x00000003
    #define UART_BIT_NUM_M  ((UART_BIT_NUM_V)<<(UART_BIT_NUM_S))
    #define UART_BIT_NUM_V  0x3
    #define UART_BIT_NUM_S  2
    /* UART_PARITY_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
    /*description: Set this bit to enable uart parity check*/
    #define UART_PARITY_EN  (BIT(1))
    #define UART_PARITY_EN_M  (BIT(1))
    #define UART_PARITY_EN_V  0x1
    #define UART_PARITY_EN_S  1
    /* UART_PARITY : R/W ;bitpos:[0] ;default: 1'b0 ; */
    /*description: Set parity check:  0:even 1:odd*/
    #define UART_PARITY  (BIT(0))
    #define UART_PARITY_M  (BIT(0))
    #define UART_PARITY_V  0x1
    #define UART_PARITY_S  0
        #define UART_GET_TICK_REF_ALWAYS_ON(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_TICK_REF_ALWAYS_ON_V, UART_TICK_REF_ALWAYS_ON_S)
        #define UART_SET_TICK_REF_ALWAYS_ON(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_TICK_REF_ALWAYS_ON_V,(val),UART_TICK_REF_ALWAYS_ON_S)
        #define UART_GET_ERR_WR_MASK(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_ERR_WR_MASK_V, UART_ERR_WR_MASK_S)
        #define UART_SET_ERR_WR_MASK(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_ERR_WR_MASK_V,(val),UART_ERR_WR_MASK_S)
        #define UART_GET_REG_CLK_EN(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_CLK_EN_V, UART_CLK_EN_S)
        #define UART_SET_REG_CLK_EN(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_CLK_EN_V,(val),UART_CLK_EN_S)
        #define UART_GET_DTR_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_DTR_INV_V, UART_DTR_INV_S)
        #define UART_SET_DTR_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_DTR_INV_V,(val),UART_DTR_INV_S)
        #define UART_GET_RTS_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_RTS_INV_V, UART_RTS_INV_S)
        #define UART_SET_RTS_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_RTS_INV_V,(val),UART_RTS_INV_S)
        #define UART_GET_TXD_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_TXD_INV_V, UART_TXD_INV_S)
        #define UART_SET_TXD_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_TXD_INV_V,(val),UART_TXD_INV_S)
        #define UART_GET_DSR_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_DSR_INV_V, UART_DSR_INV_S)
        #define UART_SET_DSR_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_DSR_INV_V,(val),UART_DSR_INV_S)
        #define UART_GET_CTS_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_CTS_INV_V, UART_CTS_INV_S)
        #define UART_SET_CTS_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_CTS_INV_V,(val),UART_CTS_INV_S)
        #define UART_GET_RXD_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_RXD_INV_V, UART_RXD_INV_S)
        #define UART_SET_RXD_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_RXD_INV_V,(val),UART_RXD_INV_S)
        #define UART_GET_TXFIFO_RST(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_TXFIFO_RST_V, UART_TXFIFO_RST_S)
        #define UART_SET_TXFIFO_RST(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_TXFIFO_RST_V,(val),UART_TXFIFO_RST_S)
        #define UART_GET_RXFIFO_RST(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_RXFIFO_RST_V, UART_RXFIFO_RST_S)
        #define UART_SET_RXFIFO_RST(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_RXFIFO_RST_V,(val),UART_RXFIFO_RST_S)
        #define UART_GET_IRDA_EN(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_IRDA_EN_V, UART_IRDA_EN_S)
        #define UART_SET_IRDA_EN(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_IRDA_EN_V,(val),UART_IRDA_EN_S)
        #define UART_GET_TX_FLOW_EN(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_TX_FLOW_EN_V, UART_TX_FLOW_EN_S)
        #define UART_SET_TX_FLOW_EN(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_TX_FLOW_EN_V,(val),UART_TX_FLOW_EN_S)
        #define UART_GET_LOOPBACK(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_LOOPBACK_V, UART_LOOPBACK_S)
        #define UART_SET_LOOPBACK(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_LOOPBACK_V,(val),UART_LOOPBACK_S)
        #define UART_GET_IRDA_RX_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_IRDA_RX_INV_V, UART_IRDA_RX_INV_S)
        #define UART_SET_IRDA_RX_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_IRDA_RX_INV_V,(val),UART_IRDA_RX_INV_S)
        #define UART_GET_IRDA_TX_INV(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_IRDA_TX_INV_V, UART_IRDA_TX_INV_S)
        #define UART_SET_IRDA_TX_INV(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_IRDA_TX_INV_V,(val),UART_IRDA_TX_INV_S)
        #define UART_GET_IRDA_WCTL(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_IRDA_WCTL_V, UART_IRDA_WCTL_S)
        #define UART_SET_IRDA_WCTL(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_IRDA_WCTL_V,(val),UART_IRDA_WCTL_S)
        #define UART_GET_IRDA_TX_EN(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_IRDA_TX_EN_V, UART_IRDA_TX_EN_S)
        #define UART_SET_IRDA_TX_EN(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_IRDA_TX_EN_V,(val),UART_IRDA_TX_EN_S)
        #define UART_GET_IRDA_DPLX(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_IRDA_DPLX_V, UART_IRDA_DPLX_S)
        #define UART_SET_IRDA_DPLX(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_IRDA_DPLX_V,(val),UART_IRDA_DPLX_S)
        #define UART_GET_TXD_BRK(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_TXD_BRK_V, UART_TXD_BRK_S)
        #define UART_SET_TXD_BRK(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_TXD_BRK_V,(val),UART_TXD_BRK_S)
        #define UART_GET_SW_DTR(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_SW_DTR_V, UART_SW_DTR_S)
        #define UART_SET_SW_DTR(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_SW_DTR_V,(val),UART_SW_DTR_S)
        #define UART_GET_SW_RTS(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_SW_RTS_V, UART_SW_RTS_S)
        #define UART_SET_SW_RTS(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_SW_RTS_V,(val),UART_SW_RTS_S)
        #define UART_GET_STOP_BIT_NUM(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_STOP_BIT_NUM_V, UART_STOP_BIT_NUM_S)
        #define UART_SET_STOP_BIT_NUM(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_STOP_BIT_NUM_V,(val),UART_STOP_BIT_NUM_S)
        #define UART_GET_BIT_NUM(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_BIT_NUM_V, UART_BIT_NUM_S)
        #define UART_SET_BIT_NUM(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_BIT_NUM_V,(val),UART_BIT_NUM_S)
        #define UART_GET_PARITY_EN(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_PARITY_EN_V, UART_PARITY_EN_S)
        #define UART_SET_PARITY_EN(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_PARITY_EN_V,(val),UART_PARITY_EN_S)
        #define UART_GET_PARITY(i)  GET_PERI_REG_BITS2(UART_CONF0_REG(i) , UART_PARITY_V, UART_PARITY_S)
        #define UART_SET_PARITY(i,val)  SET_PERI_REG_BITS(UART_CONF0_REG(i) ,UART_PARITY_V,(val),UART_PARITY_S)



#define UART_CONF1_REG(i)          (REG_UART_BASE(i) + 0x24)
    /* UART_RX_TOUT_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
    /*description: Set this bit to enable rx time-out function*/
    #define UART_RX_TOUT_EN  (BIT(31))
    #define UART_RX_TOUT_EN_M  (BIT(31))
    #define UART_RX_TOUT_EN_V  0x1
    #define UART_RX_TOUT_EN_S  31
    /* UART_RX_TOUT_THRHD : R/W ;bitpos:[30:24] ;default: 7'b0 ; */
    /*description: Config bits for rx time-out threshold*/
    #define UART_RX_TOUT_THRHD  0x0000007F
    #define UART_RX_TOUT_THRHD_M  ((UART_RX_TOUT_THRHD_V)<<(UART_RX_TOUT_THRHD_S))
    #define UART_RX_TOUT_THRHD_V  0x7F
    #define UART_RX_TOUT_THRHD_S  24
    /* UART_RX_FLOW_EN : R/W ;bitpos:[23] ;default: 1'b0 ; */
    /*description: Set this bit to enable rx hardware flow control*/
    #define UART_RX_FLOW_EN  (BIT(23))
    #define UART_RX_FLOW_EN_M  (BIT(23))
    #define UART_RX_FLOW_EN_V  0x1
    #define UART_RX_FLOW_EN_S  23
    /* UART_RX_FLOW_THRHD : R/W ;bitpos:[22:16] ;default: 7'h0 ; */
    /*description: The config bits for rx flow control threshold*/
    #define UART_RX_FLOW_THRHD  0x0000007F
    #define UART_RX_FLOW_THRHD_M  ((UART_RX_FLOW_THRHD_V)<<(UART_RX_FLOW_THRHD_S))
    #define UART_RX_FLOW_THRHD_V  0x7F
    #define UART_RX_FLOW_THRHD_S  16
    /* UART_TXFIFO_EMPTY_THRHD : R/W ;bitpos:[14:8] ;default: 7'h60 ; */
    /*description: The config bits for tx fifo empty threshold*/
    #define UART_TXFIFO_EMPTY_THRHD  0x0000007F
    #define UART_TXFIFO_EMPTY_THRHD_M  ((UART_TXFIFO_EMPTY_THRHD_V)<<(UART_TXFIFO_EMPTY_THRHD_S))
    #define UART_TXFIFO_EMPTY_THRHD_V  0x7F
    #define UART_TXFIFO_EMPTY_THRHD_S  8
    /* UART_RXFIFO_FULL_THRHD : R/W ;bitpos:[6:0] ;default: 7'h60 ; */
    /*description: The config bits for rx fifo full threshold*/
    #define UART_RXFIFO_FULL_THRHD  0x0000007F
    #define UART_RXFIFO_FULL_THRHD_M  ((UART_RXFIFO_FULL_THRHD_V)<<(UART_RXFIFO_FULL_THRHD_S))
    #define UART_RXFIFO_FULL_THRHD_V  0x7F
    #define UART_RXFIFO_FULL_THRHD_S  0
        #define UART_GET_RX_TOUT_EN(i)  GET_PERI_REG_BITS2(UART_CONF1_REG(i) , UART_RX_TOUT_EN_V, UART_RX_TOUT_EN_S)
        #define UART_SET_RX_TOUT_EN(i,val)  SET_PERI_REG_BITS(UART_CONF1_REG(i) ,UART_RX_TOUT_EN_V,(val),UART_RX_TOUT_EN_S)
        #define UART_GET_RX_TOUT_THRHD(i)  GET_PERI_REG_BITS2(UART_CONF1_REG(i) , UART_RX_TOUT_THRHD_V, UART_RX_TOUT_THRHD_S)
        #define UART_SET_RX_TOUT_THRHD(i,val)  SET_PERI_REG_BITS(UART_CONF1_REG(i) ,UART_RX_TOUT_THRHD_V,(val),UART_RX_TOUT_THRHD_S)
        #define UART_GET_RX_FLOW_EN(i)  GET_PERI_REG_BITS2(UART_CONF1_REG(i) , UART_RX_FLOW_EN_V, UART_RX_FLOW_EN_S)
        #define UART_SET_RX_FLOW_EN(i,val)  SET_PERI_REG_BITS(UART_CONF1_REG(i) ,UART_RX_FLOW_EN_V,(val),UART_RX_FLOW_EN_S)
        #define UART_GET_RX_FLOW_THRHD(i)  GET_PERI_REG_BITS2(UART_CONF1_REG(i) , UART_RX_FLOW_THRHD_V, UART_RX_FLOW_THRHD_S)
        #define UART_SET_RX_FLOW_THRHD(i,val)  SET_PERI_REG_BITS(UART_CONF1_REG(i) ,UART_RX_FLOW_THRHD_V,(val),UART_RX_FLOW_THRHD_S)
        #define UART_GET_TXFIFO_EMPTY_THRHD(i)  GET_PERI_REG_BITS2(UART_CONF1_REG(i) , UART_TXFIFO_EMPTY_THRHD_V, UART_TXFIFO_EMPTY_THRHD_S)
        #define UART_SET_TXFIFO_EMPTY_THRHD(i,val)  SET_PERI_REG_BITS(UART_CONF1_REG(i) ,UART_TXFIFO_EMPTY_THRHD_V,(val),UART_TXFIFO_EMPTY_THRHD_S)
        #define UART_GET_RXFIFO_FULL_THRHD(i)  GET_PERI_REG_BITS2(UART_CONF1_REG(i) , UART_RXFIFO_FULL_THRHD_V, UART_RXFIFO_FULL_THRHD_S)
        #define UART_SET_RXFIFO_FULL_THRHD(i,val)  SET_PERI_REG_BITS(UART_CONF1_REG(i) ,UART_RXFIFO_FULL_THRHD_V,(val),UART_RXFIFO_FULL_THRHD_S)



#define UART_LOWPULSE_REG(i)          (REG_UART_BASE(i) + 0x28)
    /* UART_LOWPULSE_MIN_CNT : RO ;bitpos:[19:0] ;default: 20'hFFFFF ; */
    /*description: used in baudrate detect*/
    #define UART_LOWPULSE_MIN_CNT  0x000FFFFF
    #define UART_LOWPULSE_MIN_CNT_M  ((UART_LOWPULSE_MIN_CNT_V)<<(UART_LOWPULSE_MIN_CNT_S))
    #define UART_LOWPULSE_MIN_CNT_V  0xFFFFF
    #define UART_LOWPULSE_MIN_CNT_S  0
        #define UART_GET_LOWPULSE_MIN_CNT(i)  GET_PERI_REG_BITS2(UART_LOWPULSE_REG(i) , UART_LOWPULSE_MIN_CNT_V, UART_LOWPULSE_MIN_CNT_S)



#define UART_HIGHPULSE_REG(i)          (REG_UART_BASE(i) + 0x2C)
    /* UART_HIGHPULSE_MIN_CNT : RO ;bitpos:[19:0] ;default: 20'hFFFFF ; */
    /*description: used in baudrate detect*/
    #define UART_HIGHPULSE_MIN_CNT  0x000FFFFF
    #define UART_HIGHPULSE_MIN_CNT_M  ((UART_HIGHPULSE_MIN_CNT_V)<<(UART_HIGHPULSE_MIN_CNT_S))
    #define UART_HIGHPULSE_MIN_CNT_V  0xFFFFF
    #define UART_HIGHPULSE_MIN_CNT_S  0
        #define UART_GET_HIGHPULSE_MIN_CNT(i)  GET_PERI_REG_BITS2(UART_HIGHPULSE_REG(i) , UART_HIGHPULSE_MIN_CNT_V, UART_HIGHPULSE_MIN_CNT_S)



#define UART_RXD_CNT_REG(i)          (REG_UART_BASE(i) + 0x30)
    /* UART_RXD_EDGE_CNT : RO ;bitpos:[9:0] ;default: 10'h0 ; */
    /*description: used in baudrate detect*/
    #define UART_RXD_EDGE_CNT  0x000003FF
    #define UART_RXD_EDGE_CNT_M  ((UART_RXD_EDGE_CNT_V)<<(UART_RXD_EDGE_CNT_S))
    #define UART_RXD_EDGE_CNT_V  0x3FF
    #define UART_RXD_EDGE_CNT_S  0
        #define UART_GET_RXD_EDGE_CNT(i)  GET_PERI_REG_BITS2(UART_RXD_CNT_REG(i) , UART_RXD_EDGE_CNT_V, UART_RXD_EDGE_CNT_S)



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
    /*description: Set this bit to set ctsn*/
    #define UART_FORCE_XOFF  (BIT(3))
    #define UART_FORCE_XOFF_M  (BIT(3))
    #define UART_FORCE_XOFF_V  0x1
    #define UART_FORCE_XOFF_S  3
    /* UART_FORCE_XON : R/W ;bitpos:[2] ;default: 1'b0 ; */
    /*description: Set this bit to clear ctsn*/
    #define UART_FORCE_XON  (BIT(2))
    #define UART_FORCE_XON_M  (BIT(2))
    #define UART_FORCE_XON_V  0x1
    #define UART_FORCE_XON_S  2
    /* UART_XONOFF_DEL : R/W ;bitpos:[1] ;default: 1'b0 ; */
    /*description: Set this bit to remove flow control char from rx data*/
    #define UART_XONOFF_DEL  (BIT(1))
    #define UART_XONOFF_DEL_M  (BIT(1))
    #define UART_XONOFF_DEL_V  0x1
    #define UART_XONOFF_DEL_S  1
    /* UART_SW_FLOW_CON_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
    /*description: Set this bit to enable sw flow control*/
    #define UART_SW_FLOW_CON_EN  (BIT(0))
    #define UART_SW_FLOW_CON_EN_M  (BIT(0))
    #define UART_SW_FLOW_CON_EN_V  0x1
    #define UART_SW_FLOW_CON_EN_S  0
        #define UART_GET_SEND_XOFF(i)  GET_PERI_REG_BITS2(UART_FLOW_CONF_REG(i) , UART_SEND_XOFF_V, UART_SEND_XOFF_S)
        #define UART_SET_SEND_XOFF(i,val)  SET_PERI_REG_BITS(UART_FLOW_CONF_REG(i) ,UART_SEND_XOFF_V,(val),UART_SEND_XOFF_S)
        #define UART_GET_SEND_XON(i)  GET_PERI_REG_BITS2(UART_FLOW_CONF_REG(i) , UART_SEND_XON_V, UART_SEND_XON_S)
        #define UART_SET_SEND_XON(i,val)  SET_PERI_REG_BITS(UART_FLOW_CONF_REG(i) ,UART_SEND_XON_V,(val),UART_SEND_XON_S)
        #define UART_GET_FORCE_XOFF(i)  GET_PERI_REG_BITS2(UART_FLOW_CONF_REG(i) , UART_FORCE_XOFF_V, UART_FORCE_XOFF_S)
        #define UART_SET_FORCE_XOFF(i,val)  SET_PERI_REG_BITS(UART_FLOW_CONF_REG(i) ,UART_FORCE_XOFF_V,(val),UART_FORCE_XOFF_S)
        #define UART_GET_FORCE_XON(i)  GET_PERI_REG_BITS2(UART_FLOW_CONF_REG(i) , UART_FORCE_XON_V, UART_FORCE_XON_S)
        #define UART_SET_FORCE_XON(i,val)  SET_PERI_REG_BITS(UART_FLOW_CONF_REG(i) ,UART_FORCE_XON_V,(val),UART_FORCE_XON_S)
        #define UART_GET_XONOFF_DEL(i)  GET_PERI_REG_BITS2(UART_FLOW_CONF_REG(i) , UART_XONOFF_DEL_V, UART_XONOFF_DEL_S)
        #define UART_SET_XONOFF_DEL(i,val)  SET_PERI_REG_BITS(UART_FLOW_CONF_REG(i) ,UART_XONOFF_DEL_V,(val),UART_XONOFF_DEL_S)
        #define UART_GET_SW_FLOW_CON_EN(i)  GET_PERI_REG_BITS2(UART_FLOW_CONF_REG(i) , UART_SW_FLOW_CON_EN_V, UART_SW_FLOW_CON_EN_S)
        #define UART_SET_SW_FLOW_CON_EN(i,val)  SET_PERI_REG_BITS(UART_FLOW_CONF_REG(i) ,UART_SW_FLOW_CON_EN_V,(val),UART_SW_FLOW_CON_EN_S)



#define UART_SLEEP_CONF_REG(i)          (REG_UART_BASE(i) + 0x38)
    /* UART_ACTIVE_THRESHOLD : R/W ;bitpos:[9:0] ;default: 10'hf0 ; */
    /*description: when the num of rx level change is more than this value*/
    #define UART_ACTIVE_THRESHOLD  0x000003FF
    #define UART_ACTIVE_THRESHOLD_M  ((UART_ACTIVE_THRESHOLD_V)<<(UART_ACTIVE_THRESHOLD_S))
    #define UART_ACTIVE_THRESHOLD_V  0x3FF
    #define UART_ACTIVE_THRESHOLD_S  0
        #define UART_GET_ACTIVE_THRESHOLD(i)  GET_PERI_REG_BITS2(UART_SLEEP_CONF_REG(i) , UART_ACTIVE_THRESHOLD_V, UART_ACTIVE_THRESHOLD_S)
        #define UART_SET_ACTIVE_THRESHOLD(i,val)  SET_PERI_REG_BITS(UART_SLEEP_CONF_REG(i) ,UART_ACTIVE_THRESHOLD_V,(val),UART_ACTIVE_THRESHOLD_S)



#define UART_SWFC_CONF_REG(i)          (REG_UART_BASE(i) + 0x3C)
    /* UART_XOFF_CHAR : R/W ;bitpos:[31:24] ;default: 8'h13 ; */
    /*description: xoff char*/
    #define UART_XOFF_CHAR  0x000000FF
    #define UART_XOFF_CHAR_M  ((UART_XOFF_CHAR_V)<<(UART_XOFF_CHAR_S))
    #define UART_XOFF_CHAR_V  0xFF
    #define UART_XOFF_CHAR_S  24
    /* UART_XON_CHAR : R/W ;bitpos:[23:16] ;default: 8'h11 ; */
    /*description: xon char*/
    #define UART_XON_CHAR  0x000000FF
    #define UART_XON_CHAR_M  ((UART_XON_CHAR_V)<<(UART_XON_CHAR_S))
    #define UART_XON_CHAR_V  0xFF
    #define UART_XON_CHAR_S  16
    /* UART_XOFF_THRESHOLD : R/W ;bitpos:[15:8] ;default: 8'he0 ; */
    /*description: rx stop receiving data when rxfifo_cnt is more than this value*/
    #define UART_XOFF_THRESHOLD  0x000000FF
    #define UART_XOFF_THRESHOLD_M  ((UART_XOFF_THRESHOLD_V)<<(UART_XOFF_THRESHOLD_S))
    #define UART_XOFF_THRESHOLD_V  0xFF
    #define UART_XOFF_THRESHOLD_S  8
    /* UART_XON_THRESHOLD : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
    /*description: rx continue receiving data when rxfifo cnt is less then this value*/
    #define UART_XON_THRESHOLD  0x000000FF
    #define UART_XON_THRESHOLD_M  ((UART_XON_THRESHOLD_V)<<(UART_XON_THRESHOLD_S))
    #define UART_XON_THRESHOLD_V  0xFF
    #define UART_XON_THRESHOLD_S  0
        #define UART_GET_XOFF_CH(i)  GET_PERI_REG_BITS2(UART_SWFC_CONF_REG(i) , UART_XOFF_CHAR_V, UART_XOFF_CHAR_S)
        #define UART_SET_XOFF_CH(i,val)  SET_PERI_REG_BITS(UART_SWFC_CONF_REG(i) ,UART_XOFF_CHAR_V,(val),UART_XOFF_CHAR_S)
        #define UART_GET_XON_CH(i)  GET_PERI_REG_BITS2(UART_SWFC_CONF_REG(i) , UART_XON_CHAR_V, UART_XON_CHAR_S)
        #define UART_SET_XON_CH(i,val)  SET_PERI_REG_BITS(UART_SWFC_CONF_REG(i) ,UART_XON_CHAR_V,(val),UART_XON_CHAR_S)
        #define UART_GET_XOFF_THRESHOLD(i)  GET_PERI_REG_BITS2(UART_SWFC_CONF_REG(i) , UART_XOFF_THRESHOLD_V, UART_XOFF_THRESHOLD_S)
        #define UART_SET_XOFF_THRESHOLD(i,val)  SET_PERI_REG_BITS(UART_SWFC_CONF_REG(i) ,UART_XOFF_THRESHOLD_V,(val),UART_XOFF_THRESHOLD_S)
        #define UART_GET_XON_THRESHOLD(i)  GET_PERI_REG_BITS2(UART_SWFC_CONF_REG(i) , UART_XON_THRESHOLD_V, UART_XON_THRESHOLD_S)
        #define UART_SET_XON_THRESHOLD(i,val)  SET_PERI_REG_BITS(UART_SWFC_CONF_REG(i) ,UART_XON_THRESHOLD_V,(val),UART_XON_THRESHOLD_S)



#define UART_IDLE_CONF_REG(i)          (REG_UART_BASE(i) + 0x40)
    /* UART_TX_BRK_NUM : R/W ;bitpos:[27:20] ;default: 8'ha ; */
    /*description: brk number config*/
    #define UART_TX_BRK_NUM  0x000000FF
    #define UART_TX_BRK_NUM_M  ((UART_TX_BRK_NUM_V)<<(UART_TX_BRK_NUM_S))
    #define UART_TX_BRK_NUM_V  0xFF
    #define UART_TX_BRK_NUM_S  20
    /* UART_TX_IDLE_NUM : R/W ;bitpos:[19:10] ;default: 10'h100 ; */
    /*description: idle number config*/
    #define UART_TX_IDLE_NUM  0x000003FF
    #define UART_TX_IDLE_NUM_M  ((UART_TX_IDLE_NUM_V)<<(UART_TX_IDLE_NUM_S))
    #define UART_TX_IDLE_NUM_V  0x3FF
    #define UART_TX_IDLE_NUM_S  10
    /* UART_RX_IDLE_THRHD : R/W ;bitpos:[9:0] ;default: 10'h100 ; */
    /*description: rx idle number config*/
    #define UART_RX_IDLE_THRHD  0x000003FF
    #define UART_RX_IDLE_THRHD_M  ((UART_RX_IDLE_THRHD_V)<<(UART_RX_IDLE_THRHD_S))
    #define UART_RX_IDLE_THRHD_V  0x3FF
    #define UART_RX_IDLE_THRHD_S  0
        #define UART_GET_TX_BRK_NUM(i)  GET_PERI_REG_BITS2(UART_IDLE_CONF_REG(i) , UART_TX_BRK_NUM_V, UART_TX_BRK_NUM_S)
        #define UART_SET_TX_BRK_NUM(i,val)  SET_PERI_REG_BITS(UART_IDLE_CONF_REG(i) ,UART_TX_BRK_NUM_V,(val),UART_TX_BRK_NUM_S)
        #define UART_GET_TX_IDLE_NUM(i)  GET_PERI_REG_BITS2(UART_IDLE_CONF_REG(i) , UART_TX_IDLE_NUM_V, UART_TX_IDLE_NUM_S)
        #define UART_SET_TX_IDLE_NUM(i,val)  SET_PERI_REG_BITS(UART_IDLE_CONF_REG(i) ,UART_TX_IDLE_NUM_V,(val),UART_TX_IDLE_NUM_S)
        #define UART_GET_RX_IDLE_THRHD(i)  GET_PERI_REG_BITS2(UART_IDLE_CONF_REG(i) , UART_RX_IDLE_THRHD_V, UART_RX_IDLE_THRHD_S)
        #define UART_SET_RX_IDLE_THRHD(i,val)  SET_PERI_REG_BITS(UART_IDLE_CONF_REG(i) ,UART_RX_IDLE_THRHD_V,(val),UART_RX_IDLE_THRHD_S)



#define UART_RS485_CONF_REG(i)          (REG_UART_BASE(i) + 0x44)
    /* UART_RS485_TX_DLY_NUM : R/W ;bitpos:[9:6] ;default: 4'b0 ; */
    /*description: config this reg to delay tx signal*/
    #define UART_RS485_TX_DLY_NUM  0x0000000F
    #define UART_RS485_TX_DLY_NUM_M  ((UART_RS485_TX_DLY_NUM_V)<<(UART_RS485_TX_DLY_NUM_S))
    #define UART_RS485_TX_DLY_NUM_V  0xF
    #define UART_RS485_TX_DLY_NUM_S  6
    /* UART_RS485_RX_DLY_NUM : R/W ;bitpos:[5] ;default: 1'b0 ; */
    /*description: config this reg to delay rx signal*/
    #define UART_RS485_RX_DLY_NUM  (BIT(5))
    #define UART_RS485_RX_DLY_NUM_M  (BIT(5))
    #define UART_RS485_RX_DLY_NUM_V  0x1
    #define UART_RS485_RX_DLY_NUM_S  5
    /* UART_RS485RXBY_TX_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
    /*description: set this bit to enable tx when rx is busy*/
    #define UART_RS485RXBY_TX_EN  (BIT(4))
    #define UART_RS485RXBY_TX_EN_M  (BIT(4))
    #define UART_RS485RXBY_TX_EN_V  0x1
    #define UART_RS485RXBY_TX_EN_S  4
    /* UART_RS485TX_RX_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
    /*description: set this bit to enable rx receiving when tx is sending*/
    #define UART_RS485TX_RX_EN  (BIT(3))
    #define UART_RS485TX_RX_EN_M  (BIT(3))
    #define UART_RS485TX_RX_EN_V  0x1
    #define UART_RS485TX_RX_EN_S  3
    /* UART_DL1_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
    /*description: set this bit to add 1 bit stop bit(after data)*/
    #define UART_DL1_EN  (BIT(2))
    #define UART_DL1_EN_M  (BIT(2))
    #define UART_DL1_EN_V  0x1
    #define UART_DL1_EN_S  2
    /* UART_DL0_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
    /*description: set this bit to add 1 bit stop bit(before data)*/
    #define UART_DL0_EN  (BIT(1))
    #define UART_DL0_EN_M  (BIT(1))
    #define UART_DL0_EN_V  0x1
    #define UART_DL0_EN_S  1
    /* UART_RS485_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
    /*description: set this bit to enable rs485 protocol*/
    #define UART_RS485_EN  (BIT(0))
    #define UART_RS485_EN_M  (BIT(0))
    #define UART_RS485_EN_V  0x1
    #define UART_RS485_EN_S  0
        #define UART_GET_RS485_TX_DLY_NUM(i)  GET_PERI_REG_BITS2(UART_RS485_CONF_REG(i) , UART_RS485_TX_DLY_NUM_V, UART_RS485_TX_DLY_NUM_S)
        #define UART_SET_RS485_TX_DLY_NUM(i,val)  SET_PERI_REG_BITS(UART_RS485_CONF_REG(i) ,UART_RS485_TX_DLY_NUM_V,(val),UART_RS485_TX_DLY_NUM_S)
        #define UART_GET_RS485_RX_DLY_NUM(i)  GET_PERI_REG_BITS2(UART_RS485_CONF_REG(i) , UART_RS485_RX_DLY_NUM_V, UART_RS485_RX_DLY_NUM_S)
        #define UART_SET_RS485_RX_DLY_NUM(i,val)  SET_PERI_REG_BITS(UART_RS485_CONF_REG(i) ,UART_RS485_RX_DLY_NUM_V,(val),UART_RS485_RX_DLY_NUM_S)
        #define UART_GET_RS485RXBY_TX_EN(i)  GET_PERI_REG_BITS2(UART_RS485_CONF_REG(i) , UART_RS485RXBY_TX_EN_V, UART_RS485RXBY_TX_EN_S)
        #define UART_SET_RS485RXBY_TX_EN(i,val)  SET_PERI_REG_BITS(UART_RS485_CONF_REG(i) ,UART_RS485RXBY_TX_EN_V,(val),UART_RS485RXBY_TX_EN_S)
        #define UART_GET_RS485TX_RX_EN(i)  GET_PERI_REG_BITS2(UART_RS485_CONF_REG(i) , UART_RS485TX_RX_EN_V, UART_RS485TX_RX_EN_S)
        #define UART_SET_RS485TX_RX_EN(i,val)  SET_PERI_REG_BITS(UART_RS485_CONF_REG(i) ,UART_RS485TX_RX_EN_V,(val),UART_RS485TX_RX_EN_S)
        #define UART_GET_DL1_EN(i)  GET_PERI_REG_BITS2(UART_RS485_CONF_REG(i) , UART_DL1_EN_V, UART_DL1_EN_S)
        #define UART_SET_DL1_EN(i,val)  SET_PERI_REG_BITS(UART_RS485_CONF_REG(i) ,UART_DL1_EN_V,(val),UART_DL1_EN_S)
        #define UART_GET_DL0_EN(i)  GET_PERI_REG_BITS2(UART_RS485_CONF_REG(i) , UART_DL0_EN_V, UART_DL0_EN_S)
        #define UART_SET_DL0_EN(i,val)  SET_PERI_REG_BITS(UART_RS485_CONF_REG(i) ,UART_DL0_EN_V,(val),UART_DL0_EN_S)
        #define UART_GET_RS485_EN(i)  GET_PERI_REG_BITS2(UART_RS485_CONF_REG(i) , UART_RS485_EN_V, UART_RS485_EN_S)
        #define UART_SET_RS485_EN(i,val)  SET_PERI_REG_BITS(UART_RS485_CONF_REG(i) ,UART_RS485_EN_V,(val),UART_RS485_EN_S)



#define UART_AT_CMD_PRECNT_REG(i)          (REG_UART_BASE(i) + 0x48)
    /* UART_PRE_IDLE_NUM : R/W ;bitpos:[23:0] ;default: 24'h186a00 ; */
    /*description: config the idle time before receiving AT_CMD*/
    #define UART_PRE_IDLE_NUM  0x00FFFFFF
    #define UART_PRE_IDLE_NUM_M  ((UART_PRE_IDLE_NUM_V)<<(UART_PRE_IDLE_NUM_S))
    #define UART_PRE_IDLE_NUM_V  0xFFFFFF
    #define UART_PRE_IDLE_NUM_S  0
        #define UART_GET_PRE_IDLE_NUM(i)  GET_PERI_REG_BITS2(UART_AT_CMD_PRECNT_REG(i) , UART_PRE_IDLE_NUM_V, UART_PRE_IDLE_NUM_S)
        #define UART_SET_PRE_IDLE_NUM(i,val)  SET_PERI_REG_BITS(UART_AT_CMD_PRECNT_REG(i) ,UART_PRE_IDLE_NUM_V,(val),UART_PRE_IDLE_NUM_S)



#define UART_AT_CMD_POSTCNT_REG(i)          (REG_UART_BASE(i) + 0x4c)
    /* UART_POST_IDLE_NUM : R/W ;bitpos:[23:0] ;default: 24'h186a00 ; */
    /*description: config the idle time after receiving AT_CMD*/
    #define UART_POST_IDLE_NUM  0x00FFFFFF
    #define UART_POST_IDLE_NUM_M  ((UART_POST_IDLE_NUM_V)<<(UART_POST_IDLE_NUM_S))
    #define UART_POST_IDLE_NUM_V  0xFFFFFF
    #define UART_POST_IDLE_NUM_S  0
        #define UART_GET_POST_IDLE_NUM(i)  GET_PERI_REG_BITS2(UART_AT_CMD_POSTCNT_REG(i) , UART_POST_IDLE_NUM_V, UART_POST_IDLE_NUM_S)
        #define UART_SET_POST_IDLE_NUM(i,val)  SET_PERI_REG_BITS(UART_AT_CMD_POSTCNT_REG(i) ,UART_POST_IDLE_NUM_V,(val),UART_POST_IDLE_NUM_S)



#define UART_AT_CMD_GAPTOUT_REG(i)          (REG_UART_BASE(i) + 0x50)
    /* UART_RX_GAP_TOUT : R/W ;bitpos:[23:0] ;default: 24'h1e00 ; */
    /*description: config the time between receiving two AT_CMD*/
    #define UART_RX_GAP_TOUT  0x00FFFFFF
    #define UART_RX_GAP_TOUT_M  ((UART_RX_GAP_TOUT_V)<<(UART_RX_GAP_TOUT_S))
    #define UART_RX_GAP_TOUT_V  0xFFFFFF
    #define UART_RX_GAP_TOUT_S  0
        #define UART_GET_RX_GAP_TOUT(i)  GET_PERI_REG_BITS2(UART_AT_CMD_GAPTOUT_REG(i) , UART_RX_GAP_TOUT_V, UART_RX_GAP_TOUT_S)
        #define UART_SET_RX_GAP_TOUT(i,val)  SET_PERI_REG_BITS(UART_AT_CMD_GAPTOUT_REG(i) ,UART_RX_GAP_TOUT_V,(val),UART_RX_GAP_TOUT_S)



#define UART_AT_CMD_CHAR_REG(i)          (REG_UART_BASE(i) + 0x54)
    /* UART_CHAR_NUM : R/W ;bitpos:[15:8] ;default: 8'd3 ; */
    /*description: config the continous num of AT_CMD*/
    #define UART_CHAR_NUM  0x000000FF
    #define UART_CHAR_NUM_M  ((UART_CHAR_NUM_V)<<(UART_CHAR_NUM_S))
    #define UART_CHAR_NUM_V  0xFF
    #define UART_CHAR_NUM_S  8
    /* UART_AT_CMD_CHAR : R/W ;bitpos:[7:0] ;default: 8'h2b ; */
    /*description: config AT_CMD*/
    #define UART_AT_CMD_CHAR  0x000000FF
    #define UART_AT_CMD_CHAR_M  ((UART_AT_CMD_CHAR_V)<<(UART_AT_CMD_CHAR_S))
    #define UART_AT_CMD_CHAR_V  0xFF
    #define UART_AT_CMD_CHAR_S  0
        #define UART_GET_CHAR_NUM(i)  GET_PERI_REG_BITS2(UART_AT_CMD_CHAR_REG(i) , UART_CHAR_NUM_V, UART_CHAR_NUM_S)
        #define UART_SET_CHAR_NUM(i,val)  SET_PERI_REG_BITS(UART_AT_CMD_CHAR_REG(i) ,UART_CHAR_NUM_V,(val),UART_CHAR_NUM_S)
        #define UART_GET_AT_CMD_CHAR(i)  GET_PERI_REG_BITS2(UART_AT_CMD_CHAR_REG(i) , UART_AT_CMD_CHAR_V, UART_AT_CMD_CHAR_S)
        #define UART_SET_AT_CMD_CHAR(i,val)  SET_PERI_REG_BITS(UART_AT_CMD_CHAR_REG(i) ,UART_AT_CMD_CHAR_V,(val),UART_AT_CMD_CHAR_S)



#define UART_MEM_CONF_REG(i)          (REG_UART_BASE(i) + 0x58)
    /* UART_TX_MEM_EMPTY_THRHD : R/W ;bitpos:[30:28] ;default: 3'h0 ; */
    /*description: the most significant bits of tx fifo empty threashold*/
    #define UART_TX_MEM_EMPTY_THRHD  0x00000007
    #define UART_TX_MEM_EMPTY_THRHD_M  ((UART_TX_MEM_EMPTY_THRHD_V)<<(UART_TX_MEM_EMPTY_THRHD_S))
    #define UART_TX_MEM_EMPTY_THRHD_V  0x7
    #define UART_TX_MEM_EMPTY_THRHD_S  28
    /* UART_RX_MEM_FULL_THRHD : R/W ;bitpos:[27:25] ;default: 3'h0 ; */
    /*description: the most significant bits of rx fifo full threashold*/
    #define UART_RX_MEM_FULL_THRHD  0x00000007
    #define UART_RX_MEM_FULL_THRHD_M  ((UART_RX_MEM_FULL_THRHD_V)<<(UART_RX_MEM_FULL_THRHD_S))
    #define UART_RX_MEM_FULL_THRHD_V  0x7
    #define UART_RX_MEM_FULL_THRHD_S  25
    /* UART_XOFF_THRESHOLD_H2 : R/W ;bitpos:[24:23] ;default: 2'h0 ; */
    /*description: the most significant bits of uart xoff flow threashold*/
    #define UART_XOFF_THRESHOLD_H2  0x00000003
    #define UART_XOFF_THRESHOLD_H2_M  ((UART_XOFF_THRESHOLD_H2_V)<<(UART_XOFF_THRESHOLD_H2_S))
    #define UART_XOFF_THRESHOLD_H2_V  0x3
    #define UART_XOFF_THRESHOLD_H2_S  23
    /* UART_XON_THRESHOLD_H2 : R/W ;bitpos:[22:21] ;default: 2'h0 ; */
    /*description: the most significant bits of uart xon flow threshold*/
    #define UART_XON_THRESHOLD_H2  0x00000003
    #define UART_XON_THRESHOLD_H2_M  ((UART_XON_THRESHOLD_H2_V)<<(UART_XON_THRESHOLD_H2_S))
    #define UART_XON_THRESHOLD_H2_V  0x3
    #define UART_XON_THRESHOLD_H2_S  21
    /* UART_RX_TOUT_THRHD_H3 : R/W ;bitpos:[20:18] ;default: 3h0 ; */
    /*description: the most significant bits of rx timeout threshold*/
    #define UART_RX_TOUT_THRHD_H3  0x00000007
    #define UART_RX_TOUT_THRHD_H3_M  ((UART_RX_TOUT_THRHD_H3_V)<<(UART_RX_TOUT_THRHD_H3_S))
    #define UART_RX_TOUT_THRHD_H3_V  0x7
    #define UART_RX_TOUT_THRHD_H3_S  18
    /* UART_RX_FLOW_THRHD_H3 : R/W ;bitpos:[17:15] ;default: 3'h0 ; */
    /*description: the most significant bits of  rx flow control threshold*/
    #define UART_RX_FLOW_THRHD_H3  0x00000007
    #define UART_RX_FLOW_THRHD_H3_M  ((UART_RX_FLOW_THRHD_H3_V)<<(UART_RX_FLOW_THRHD_H3_S))
    #define UART_RX_FLOW_THRHD_H3_V  0x7
    #define UART_RX_FLOW_THRHD_H3_S  15
    /* UART_TX_SIZE : R/W ;bitpos:[10:7] ;default: 3'h0 ; */
    /*description: config tx_mem's size(128*reg_rx_size)*/
    #define UART_TX_SIZE  0x0000000F
    #define UART_TX_SIZE_M  ((UART_TX_SIZE_V)<<(UART_TX_SIZE_S))
    #define UART_TX_SIZE_V  0xF
    #define UART_TX_SIZE_S  7
    /* UART_RX_SIZE : R/W ;bitpos:[6:3] ;default: 3'h0 ; */
    /*description: config rx mem's size(128*reg_rx_size)*/
    #define UART_RX_SIZE  0x0000000F
    #define UART_RX_SIZE_M  ((UART_RX_SIZE_V)<<(UART_RX_SIZE_S))
    #define UART_RX_SIZE_V  0xF
    #define UART_RX_SIZE_S  3
    /* UART_MEM_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
    /*description: set this bit to pull_down mem's pd signal*/
    #define UART_MEM_PD  (BIT(0))
    #define UART_MEM_PD_M  (BIT(0))
    #define UART_MEM_PD_V  0x1
    #define UART_MEM_PD_S  0
        #define UART_GET_TX_MEM_EMPTY_THRHD(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_TX_MEM_EMPTY_THRHD_V, UART_TX_MEM_EMPTY_THRHD_S)
        #define UART_SET_TX_MEM_EMPTY_THRHD(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_TX_MEM_EMPTY_THRHD_V,(val),UART_TX_MEM_EMPTY_THRHD_S)
        #define UART_GET_RX_MEM_FULL_THRHD(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_RX_MEM_FULL_THRHD_V, UART_RX_MEM_FULL_THRHD_S)
        #define UART_SET_RX_MEM_FULL_THRHD(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_RX_MEM_FULL_THRHD_V,(val),UART_RX_MEM_FULL_THRHD_S)
        #define UART_GET_XOFF_THRESHOLD_H2(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_XOFF_THRESHOLD_H2_V, UART_XOFF_THRESHOLD_H2_S)
        #define UART_SET_XOFF_THRESHOLD_H2(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_XOFF_THRESHOLD_H2_V,(val),UART_XOFF_THRESHOLD_H2_S)
        #define UART_GET_XON_THRESHOLD_H2(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_XON_THRESHOLD_H2_V, UART_XON_THRESHOLD_H2_S)
        #define UART_SET_XON_THRESHOLD_H2(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_XON_THRESHOLD_H2_V,(val),UART_XON_THRESHOLD_H2_S)
        #define UART_GET_RX_TOUT_THRHD_H3(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_RX_TOUT_THRHD_H3_V, UART_RX_TOUT_THRHD_H3_S)
        #define UART_SET_RX_TOUT_THRHD_H3(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_RX_TOUT_THRHD_H3_V,(val),UART_RX_TOUT_THRHD_H3_S)
        #define UART_GET_RX_FLOW_THRHD_H3(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_RX_FLOW_THRHD_H3_V, UART_RX_FLOW_THRHD_H3_S)
        #define UART_SET_RX_FLOW_THRHD_H3(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_RX_FLOW_THRHD_H3_V,(val),UART_RX_FLOW_THRHD_H3_S)
        #define UART_GET_REG_TX_SIZE(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_TX_SIZE_V, UART_TX_SIZE_S)
        #define UART_SET_REG_TX_SIZE(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_TX_SIZE_V,(val),UART_TX_SIZE_S)
        #define UART_GET_REG_RX_SIZE(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_RX_SIZE_V, UART_RX_SIZE_S)
        #define UART_SET_REG_RX_SIZE(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_RX_SIZE_V,(val),UART_RX_SIZE_S)
        #define UART_GET_REG_MEM_PD(i)  GET_PERI_REG_BITS2(UART_MEM_CONF_REG(i) , UART_MEM_PD_V, UART_MEM_PD_S)
        #define UART_SET_REG_MEM_PD(i,val)  SET_PERI_REG_BITS(UART_MEM_CONF_REG(i) ,UART_MEM_PD_V,(val),UART_MEM_PD_S)



#define UART_MEM_TX_STATUS_REG(i)          (REG_UART_BASE(i) + 0x5c)
    /* UART_TX_RADDR_CHX : RO ;bitpos:[23:13] ;default: 11'b0 ; */
    /*description: uartx tx mem's read address*/
    #define UART_TX_RADDR_CHX  0x000007FF
    #define UART_TX_RADDR_CHX_M  ((UART_TX_RADDR_CHX_V)<<(UART_TX_RADDR_CHX_S))
    #define UART_TX_RADDR_CHX_V  0x7FF
    #define UART_TX_RADDR_CHX_S  13
    /* UART_APB_TX_WADDR_CHX : RO ;bitpos:[12:2] ;default: 11'b0 ; */
    /*description: uartx tx mem's write address*/
    #define UART_APB_TX_WADDR_CHX  0x000007FF
    #define UART_APB_TX_WADDR_CHX_M  ((UART_APB_TX_WADDR_CHX_V)<<(UART_APB_TX_WADDR_CHX_S))
    #define UART_APB_TX_WADDR_CHX_V  0x7FF
    #define UART_APB_TX_WADDR_CHX_S  2
        #define UART_GET_TX_RADDR_CHX(i)  GET_PERI_REG_BITS2(UART_MEM_TX_STATUS_REG(i) , UART_TX_RADDR_CHX_V, UART_TX_RADDR_CHX_S)
        #define UART_GET_APB_TX_WADDR_CHX(i)  GET_PERI_REG_BITS2(UART_MEM_TX_STATUS_REG(i) , UART_APB_TX_WADDR_CHX_V, UART_APB_TX_WADDR_CHX_S)



#define UART_MEM_RX_STATUS_REG(i)          (REG_UART_BASE(i) + 0x60)
    /* UART_RX_WADDR_CHX : RO ;bitpos:[23:13] ;default: 11'b0 ; */
    /*description: uartx rx mem's write address*/
    #define UART_RX_WADDR_CHX  0x000007FF
    #define UART_RX_WADDR_CHX_M  ((UART_RX_WADDR_CHX_V)<<(UART_RX_WADDR_CHX_S))
    #define UART_RX_WADDR_CHX_V  0x7FF
    #define UART_RX_WADDR_CHX_S  13
    /* UART_APB_RX_RADDR_CHX : RO ;bitpos:[12:2] ;default: 11'b0 ; */
    /*description: uartx rx mem's read address*/
    #define UART_APB_RX_RADDR_CHX  0x000007FF
    #define UART_APB_RX_RADDR_CHX_M  ((UART_APB_RX_RADDR_CHX_V)<<(UART_APB_RX_RADDR_CHX_S))
    #define UART_APB_RX_RADDR_CHX_V  0x7FF
    #define UART_APB_RX_RADDR_CHX_S  2
        #define UART_GET_RX_WADDR_CHX(i)  GET_PERI_REG_BITS2(UART_MEM_RX_STATUS_REG(i) , UART_RX_WADDR_CHX_V, UART_RX_WADDR_CHX_S)
        #define UART_GET_APB_RX_RADDR_CHX(i)  GET_PERI_REG_BITS2(UART_MEM_RX_STATUS_REG(i) , UART_APB_RX_RADDR_CHX_V, UART_APB_RX_RADDR_CHX_S)



#define UART_DATE_REG(i)          (REG_UART_BASE(i) + 0x78)
    /* UART_DATE : R/W ;bitpos:[31:0] ;default: 32'h15122500 ; */
    /*description: */
    #define UART_DATE  0xFFFFFFFF
    #define UART_DATE_M  ((UART_DATE_V)<<(UART_DATE_S))
    #define UART_DATE_V  0xFFFFFFFF
    #define UART_DATE_S  0
        #define UART_GET_DATE(i)  GET_PERI_REG_BITS2(UART_DATE_REG(i) , UART_DATE_V, UART_DATE_S)
        #define UART_SET_DATE(i,val)  SET_PERI_REG_BITS(UART_DATE_REG(i) ,UART_DATE_V,(val),UART_DATE_S)



#define UART_ID_REG(i)          (REG_UART_BASE(i) + 0x7C)
    /* UART_ID : R/W ;bitpos:[31:0] ;default: 32'h0500 ; */
    /*description: */
    #define UART_ID  0xFFFFFFFF
    #define UART_ID_M  ((UART_ID_V)<<(UART_ID_S))
    #define UART_ID_V  0xFFFFFFFF
    #define UART_ID_S  0
        #define UART_GET_ID(i)  GET_PERI_REG_BITS2(UART_ID_REG(i) , UART_ID_V, UART_ID_S)
        #define UART_SET_ID(i,val)  SET_PERI_REG_BITS(UART_ID_REG(i) ,UART_ID_V,(val),UART_ID_S)






#endif /* _SOC_UART_REG_H_ */


