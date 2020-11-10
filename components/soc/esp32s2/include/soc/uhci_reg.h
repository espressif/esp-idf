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
#ifndef _SOC_UHCI_REG_H_
#define _SOC_UHCI_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define UHCI_CONF0_REG(i)          (REG_UHCI_BASE(i) + 0x0)
/* UHCI_UART_RX_BRK_EOF_EN : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define UHCI_UART_RX_BRK_EOF_EN  (BIT(23))
#define UHCI_UART_RX_BRK_EOF_EN_M  (BIT(23))
#define UHCI_UART_RX_BRK_EOF_EN_V  0x1
#define UHCI_UART_RX_BRK_EOF_EN_S  23
/* UHCI_CLK_EN : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define UHCI_CLK_EN  (BIT(22))
#define UHCI_CLK_EN_M  (BIT(22))
#define UHCI_CLK_EN_V  0x1
#define UHCI_CLK_EN_S  22
/* UHCI_ENCODE_CRC_EN : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: */
#define UHCI_ENCODE_CRC_EN  (BIT(21))
#define UHCI_ENCODE_CRC_EN_M  (BIT(21))
#define UHCI_ENCODE_CRC_EN_V  0x1
#define UHCI_ENCODE_CRC_EN_S  21
/* UHCI_LEN_EOF_EN : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: */
#define UHCI_LEN_EOF_EN  (BIT(20))
#define UHCI_LEN_EOF_EN_M  (BIT(20))
#define UHCI_LEN_EOF_EN_V  0x1
#define UHCI_LEN_EOF_EN_S  20
/* UHCI_UART_IDLE_EOF_EN : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define UHCI_UART_IDLE_EOF_EN  (BIT(19))
#define UHCI_UART_IDLE_EOF_EN_M  (BIT(19))
#define UHCI_UART_IDLE_EOF_EN_V  0x1
#define UHCI_UART_IDLE_EOF_EN_S  19
/* UHCI_CRC_REC_EN : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: */
#define UHCI_CRC_REC_EN  (BIT(18))
#define UHCI_CRC_REC_EN_M  (BIT(18))
#define UHCI_CRC_REC_EN_V  0x1
#define UHCI_CRC_REC_EN_S  18
/* UHCI_HEAD_EN : R/W ;bitpos:[17] ;default: 1'b1 ; */
/*description: */
#define UHCI_HEAD_EN  (BIT(17))
#define UHCI_HEAD_EN_M  (BIT(17))
#define UHCI_HEAD_EN_V  0x1
#define UHCI_HEAD_EN_S  17
/* UHCI_SEPER_EN : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: */
#define UHCI_SEPER_EN  (BIT(16))
#define UHCI_SEPER_EN_M  (BIT(16))
#define UHCI_SEPER_EN_V  0x1
#define UHCI_SEPER_EN_S  16
/* UHCI_MEM_TRANS_EN : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UHCI_MEM_TRANS_EN  (BIT(15))
#define UHCI_MEM_TRANS_EN_M  (BIT(15))
#define UHCI_MEM_TRANS_EN_V  0x1
#define UHCI_MEM_TRANS_EN_S  15
/* UHCI_OUT_DATA_BURST_EN : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DATA_BURST_EN  (BIT(14))
#define UHCI_OUT_DATA_BURST_EN_M  (BIT(14))
#define UHCI_OUT_DATA_BURST_EN_V  0x1
#define UHCI_OUT_DATA_BURST_EN_S  14
/* UHCI_INDSCR_BURST_EN : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UHCI_INDSCR_BURST_EN  (BIT(13))
#define UHCI_INDSCR_BURST_EN_M  (BIT(13))
#define UHCI_INDSCR_BURST_EN_V  0x1
#define UHCI_INDSCR_BURST_EN_S  13
/* UHCI_OUTDSCR_BURST_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTDSCR_BURST_EN  (BIT(12))
#define UHCI_OUTDSCR_BURST_EN_M  (BIT(12))
#define UHCI_OUTDSCR_BURST_EN_V  0x1
#define UHCI_OUTDSCR_BURST_EN_S  12
/* UHCI_UART1_CE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UHCI_UART1_CE  (BIT(10))
#define UHCI_UART1_CE_M  (BIT(10))
#define UHCI_UART1_CE_V  0x1
#define UHCI_UART1_CE_S  10
/* UHCI_UART0_CE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UHCI_UART0_CE  (BIT(9))
#define UHCI_UART0_CE_M  (BIT(9))
#define UHCI_UART0_CE_V  0x1
#define UHCI_UART0_CE_S  9
/* UHCI_OUT_EOF_MODE : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define UHCI_OUT_EOF_MODE  (BIT(8))
#define UHCI_OUT_EOF_MODE_M  (BIT(8))
#define UHCI_OUT_EOF_MODE_V  0x1
#define UHCI_OUT_EOF_MODE_S  8
/* UHCI_OUT_NO_RESTART_CLR : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_NO_RESTART_CLR  (BIT(7))
#define UHCI_OUT_NO_RESTART_CLR_M  (BIT(7))
#define UHCI_OUT_NO_RESTART_CLR_V  0x1
#define UHCI_OUT_NO_RESTART_CLR_S  7
/* UHCI_OUT_AUTO_WRBACK : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_AUTO_WRBACK  (BIT(6))
#define UHCI_OUT_AUTO_WRBACK_M  (BIT(6))
#define UHCI_OUT_AUTO_WRBACK_V  0x1
#define UHCI_OUT_AUTO_WRBACK_S  6
/* UHCI_OUT_LOOP_TEST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_LOOP_TEST  (BIT(5))
#define UHCI_OUT_LOOP_TEST_M  (BIT(5))
#define UHCI_OUT_LOOP_TEST_V  0x1
#define UHCI_OUT_LOOP_TEST_S  5
/* UHCI_IN_LOOP_TEST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_LOOP_TEST  (BIT(4))
#define UHCI_IN_LOOP_TEST_M  (BIT(4))
#define UHCI_IN_LOOP_TEST_V  0x1
#define UHCI_IN_LOOP_TEST_S  4
/* UHCI_AHBM_RST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_AHBM_RST  (BIT(3))
#define UHCI_AHBM_RST_M  (BIT(3))
#define UHCI_AHBM_RST_V  0x1
#define UHCI_AHBM_RST_S  3
/* UHCI_AHBM_FIFO_RST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UHCI_AHBM_FIFO_RST  (BIT(2))
#define UHCI_AHBM_FIFO_RST_M  (BIT(2))
#define UHCI_AHBM_FIFO_RST_V  0x1
#define UHCI_AHBM_FIFO_RST_S  2
/* UHCI_OUT_RST : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_RST  (BIT(1))
#define UHCI_OUT_RST_M  (BIT(1))
#define UHCI_OUT_RST_V  0x1
#define UHCI_OUT_RST_S  1
/* UHCI_IN_RST : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define UHCI_IN_RST  (BIT(0))
#define UHCI_IN_RST_M  (BIT(0))
#define UHCI_IN_RST_V  0x1
#define UHCI_IN_RST_S  0

#define UHCI_INT_RAW_REG(i)          (REG_UHCI_BASE(i) + 0x4)
/* UHCI_DMA_INFIFO_FULL_WM_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UHCI_DMA_INFIFO_FULL_WM_INT_RAW  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_RAW_M  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_RAW_V  0x1
#define UHCI_DMA_INFIFO_FULL_WM_INT_RAW_S  16
/* UHCI_SEND_A_Q_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_A_Q_INT_RAW  (BIT(15))
#define UHCI_SEND_A_Q_INT_RAW_M  (BIT(15))
#define UHCI_SEND_A_Q_INT_RAW_V  0x1
#define UHCI_SEND_A_Q_INT_RAW_S  15
/* UHCI_SEND_S_Q_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_S_Q_INT_RAW  (BIT(14))
#define UHCI_SEND_S_Q_INT_RAW_M  (BIT(14))
#define UHCI_SEND_S_Q_INT_RAW_V  0x1
#define UHCI_SEND_S_Q_INT_RAW_S  14
/* UHCI_OUT_TOTAL_EOF_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_TOTAL_EOF_INT_RAW  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_RAW_M  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_RAW_V  0x1
#define UHCI_OUT_TOTAL_EOF_INT_RAW_S  13
/* UHCI_OUTLINK_EOF_ERR_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTLINK_EOF_ERR_INT_RAW  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_RAW_M  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_RAW_V  0x1
#define UHCI_OUTLINK_EOF_ERR_INT_RAW_S  12
/* UHCI_IN_DSCR_EMPTY_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_EMPTY_INT_RAW  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_RAW_M  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_RAW_V  0x1
#define UHCI_IN_DSCR_EMPTY_INT_RAW_S  11
/* UHCI_OUT_DSCR_ERR_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DSCR_ERR_INT_RAW  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_RAW_M  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_RAW_V  0x1
#define UHCI_OUT_DSCR_ERR_INT_RAW_S  10
/* UHCI_IN_DSCR_ERR_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_ERR_INT_RAW  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_RAW_M  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_RAW_V  0x1
#define UHCI_IN_DSCR_ERR_INT_RAW_S  9
/* UHCI_OUT_EOF_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_EOF_INT_RAW  (BIT(8))
#define UHCI_OUT_EOF_INT_RAW_M  (BIT(8))
#define UHCI_OUT_EOF_INT_RAW_V  0x1
#define UHCI_OUT_EOF_INT_RAW_S  8
/* UHCI_OUT_DONE_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DONE_INT_RAW  (BIT(7))
#define UHCI_OUT_DONE_INT_RAW_M  (BIT(7))
#define UHCI_OUT_DONE_INT_RAW_V  0x1
#define UHCI_OUT_DONE_INT_RAW_S  7
/* UHCI_IN_ERR_EOF_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_ERR_EOF_INT_RAW  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_RAW_M  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_RAW_V  0x1
#define UHCI_IN_ERR_EOF_INT_RAW_S  6
/* UHCI_IN_SUC_EOF_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_SUC_EOF_INT_RAW  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_RAW_M  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_RAW_V  0x1
#define UHCI_IN_SUC_EOF_INT_RAW_S  5
/* UHCI_IN_DONE_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DONE_INT_RAW  (BIT(4))
#define UHCI_IN_DONE_INT_RAW_M  (BIT(4))
#define UHCI_IN_DONE_INT_RAW_V  0x1
#define UHCI_IN_DONE_INT_RAW_S  4
/* UHCI_TX_HUNG_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_HUNG_INT_RAW  (BIT(3))
#define UHCI_TX_HUNG_INT_RAW_M  (BIT(3))
#define UHCI_TX_HUNG_INT_RAW_V  0x1
#define UHCI_TX_HUNG_INT_RAW_S  3
/* UHCI_RX_HUNG_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_HUNG_INT_RAW  (BIT(2))
#define UHCI_RX_HUNG_INT_RAW_M  (BIT(2))
#define UHCI_RX_HUNG_INT_RAW_V  0x1
#define UHCI_RX_HUNG_INT_RAW_S  2
/* UHCI_TX_START_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_START_INT_RAW  (BIT(1))
#define UHCI_TX_START_INT_RAW_M  (BIT(1))
#define UHCI_TX_START_INT_RAW_V  0x1
#define UHCI_TX_START_INT_RAW_S  1
/* UHCI_RX_START_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_START_INT_RAW  (BIT(0))
#define UHCI_RX_START_INT_RAW_M  (BIT(0))
#define UHCI_RX_START_INT_RAW_V  0x1
#define UHCI_RX_START_INT_RAW_S  0

#define UHCI_INT_ST_REG(i)          (REG_UHCI_BASE(i) + 0x8)
/* UHCI_DMA_INFIFO_FULL_WM_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UHCI_DMA_INFIFO_FULL_WM_INT_ST  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_ST_M  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_ST_V  0x1
#define UHCI_DMA_INFIFO_FULL_WM_INT_ST_S  16
/* UHCI_SEND_A_Q_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_A_Q_INT_ST  (BIT(15))
#define UHCI_SEND_A_Q_INT_ST_M  (BIT(15))
#define UHCI_SEND_A_Q_INT_ST_V  0x1
#define UHCI_SEND_A_Q_INT_ST_S  15
/* UHCI_SEND_S_Q_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_S_Q_INT_ST  (BIT(14))
#define UHCI_SEND_S_Q_INT_ST_M  (BIT(14))
#define UHCI_SEND_S_Q_INT_ST_V  0x1
#define UHCI_SEND_S_Q_INT_ST_S  14
/* UHCI_OUT_TOTAL_EOF_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_TOTAL_EOF_INT_ST  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_ST_M  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_ST_V  0x1
#define UHCI_OUT_TOTAL_EOF_INT_ST_S  13
/* UHCI_OUTLINK_EOF_ERR_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTLINK_EOF_ERR_INT_ST  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_ST_M  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_ST_V  0x1
#define UHCI_OUTLINK_EOF_ERR_INT_ST_S  12
/* UHCI_IN_DSCR_EMPTY_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_EMPTY_INT_ST  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_ST_M  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_ST_V  0x1
#define UHCI_IN_DSCR_EMPTY_INT_ST_S  11
/* UHCI_OUT_DSCR_ERR_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DSCR_ERR_INT_ST  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_ST_M  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_ST_V  0x1
#define UHCI_OUT_DSCR_ERR_INT_ST_S  10
/* UHCI_IN_DSCR_ERR_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_ERR_INT_ST  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_ST_M  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_ST_V  0x1
#define UHCI_IN_DSCR_ERR_INT_ST_S  9
/* UHCI_OUT_EOF_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_EOF_INT_ST  (BIT(8))
#define UHCI_OUT_EOF_INT_ST_M  (BIT(8))
#define UHCI_OUT_EOF_INT_ST_V  0x1
#define UHCI_OUT_EOF_INT_ST_S  8
/* UHCI_OUT_DONE_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DONE_INT_ST  (BIT(7))
#define UHCI_OUT_DONE_INT_ST_M  (BIT(7))
#define UHCI_OUT_DONE_INT_ST_V  0x1
#define UHCI_OUT_DONE_INT_ST_S  7
/* UHCI_IN_ERR_EOF_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_ERR_EOF_INT_ST  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_ST_M  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_ST_V  0x1
#define UHCI_IN_ERR_EOF_INT_ST_S  6
/* UHCI_IN_SUC_EOF_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_SUC_EOF_INT_ST  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_ST_M  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_ST_V  0x1
#define UHCI_IN_SUC_EOF_INT_ST_S  5
/* UHCI_IN_DONE_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DONE_INT_ST  (BIT(4))
#define UHCI_IN_DONE_INT_ST_M  (BIT(4))
#define UHCI_IN_DONE_INT_ST_V  0x1
#define UHCI_IN_DONE_INT_ST_S  4
/* UHCI_TX_HUNG_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_HUNG_INT_ST  (BIT(3))
#define UHCI_TX_HUNG_INT_ST_M  (BIT(3))
#define UHCI_TX_HUNG_INT_ST_V  0x1
#define UHCI_TX_HUNG_INT_ST_S  3
/* UHCI_RX_HUNG_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_HUNG_INT_ST  (BIT(2))
#define UHCI_RX_HUNG_INT_ST_M  (BIT(2))
#define UHCI_RX_HUNG_INT_ST_V  0x1
#define UHCI_RX_HUNG_INT_ST_S  2
/* UHCI_TX_START_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_START_INT_ST  (BIT(1))
#define UHCI_TX_START_INT_ST_M  (BIT(1))
#define UHCI_TX_START_INT_ST_V  0x1
#define UHCI_TX_START_INT_ST_S  1
/* UHCI_RX_START_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_START_INT_ST  (BIT(0))
#define UHCI_RX_START_INT_ST_M  (BIT(0))
#define UHCI_RX_START_INT_ST_V  0x1
#define UHCI_RX_START_INT_ST_S  0

#define UHCI_INT_ENA_REG(i)          (REG_UHCI_BASE(i) + 0xC)
/* UHCI_DMA_INFIFO_FULL_WM_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UHCI_DMA_INFIFO_FULL_WM_INT_ENA  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_ENA_M  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_ENA_V  0x1
#define UHCI_DMA_INFIFO_FULL_WM_INT_ENA_S  16
/* UHCI_SEND_A_Q_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_A_Q_INT_ENA  (BIT(15))
#define UHCI_SEND_A_Q_INT_ENA_M  (BIT(15))
#define UHCI_SEND_A_Q_INT_ENA_V  0x1
#define UHCI_SEND_A_Q_INT_ENA_S  15
/* UHCI_SEND_S_Q_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_S_Q_INT_ENA  (BIT(14))
#define UHCI_SEND_S_Q_INT_ENA_M  (BIT(14))
#define UHCI_SEND_S_Q_INT_ENA_V  0x1
#define UHCI_SEND_S_Q_INT_ENA_S  14
/* UHCI_OUT_TOTAL_EOF_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_TOTAL_EOF_INT_ENA  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_ENA_M  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_ENA_V  0x1
#define UHCI_OUT_TOTAL_EOF_INT_ENA_S  13
/* UHCI_OUTLINK_EOF_ERR_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTLINK_EOF_ERR_INT_ENA  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_M  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_V  0x1
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_S  12
/* UHCI_IN_DSCR_EMPTY_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_EMPTY_INT_ENA  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_ENA_M  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_ENA_V  0x1
#define UHCI_IN_DSCR_EMPTY_INT_ENA_S  11
/* UHCI_OUT_DSCR_ERR_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DSCR_ERR_INT_ENA  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_ENA_M  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_ENA_V  0x1
#define UHCI_OUT_DSCR_ERR_INT_ENA_S  10
/* UHCI_IN_DSCR_ERR_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_ERR_INT_ENA  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_ENA_M  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_ENA_V  0x1
#define UHCI_IN_DSCR_ERR_INT_ENA_S  9
/* UHCI_OUT_EOF_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_EOF_INT_ENA  (BIT(8))
#define UHCI_OUT_EOF_INT_ENA_M  (BIT(8))
#define UHCI_OUT_EOF_INT_ENA_V  0x1
#define UHCI_OUT_EOF_INT_ENA_S  8
/* UHCI_OUT_DONE_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DONE_INT_ENA  (BIT(7))
#define UHCI_OUT_DONE_INT_ENA_M  (BIT(7))
#define UHCI_OUT_DONE_INT_ENA_V  0x1
#define UHCI_OUT_DONE_INT_ENA_S  7
/* UHCI_IN_ERR_EOF_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_ERR_EOF_INT_ENA  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_ENA_M  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_ENA_V  0x1
#define UHCI_IN_ERR_EOF_INT_ENA_S  6
/* UHCI_IN_SUC_EOF_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_SUC_EOF_INT_ENA  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_ENA_M  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_ENA_V  0x1
#define UHCI_IN_SUC_EOF_INT_ENA_S  5
/* UHCI_IN_DONE_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DONE_INT_ENA  (BIT(4))
#define UHCI_IN_DONE_INT_ENA_M  (BIT(4))
#define UHCI_IN_DONE_INT_ENA_V  0x1
#define UHCI_IN_DONE_INT_ENA_S  4
/* UHCI_TX_HUNG_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_HUNG_INT_ENA  (BIT(3))
#define UHCI_TX_HUNG_INT_ENA_M  (BIT(3))
#define UHCI_TX_HUNG_INT_ENA_V  0x1
#define UHCI_TX_HUNG_INT_ENA_S  3
/* UHCI_RX_HUNG_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_HUNG_INT_ENA  (BIT(2))
#define UHCI_RX_HUNG_INT_ENA_M  (BIT(2))
#define UHCI_RX_HUNG_INT_ENA_V  0x1
#define UHCI_RX_HUNG_INT_ENA_S  2
/* UHCI_TX_START_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_START_INT_ENA  (BIT(1))
#define UHCI_TX_START_INT_ENA_M  (BIT(1))
#define UHCI_TX_START_INT_ENA_V  0x1
#define UHCI_TX_START_INT_ENA_S  1
/* UHCI_RX_START_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_START_INT_ENA  (BIT(0))
#define UHCI_RX_START_INT_ENA_M  (BIT(0))
#define UHCI_RX_START_INT_ENA_V  0x1
#define UHCI_RX_START_INT_ENA_S  0

#define UHCI_INT_CLR_REG(i)          (REG_UHCI_BASE(i) + 0x10)
/* UHCI_DMA_INFIFO_FULL_WM_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define UHCI_DMA_INFIFO_FULL_WM_INT_CLR  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_CLR_M  (BIT(16))
#define UHCI_DMA_INFIFO_FULL_WM_INT_CLR_V  0x1
#define UHCI_DMA_INFIFO_FULL_WM_INT_CLR_S  16
/* UHCI_SEND_A_Q_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_A_Q_INT_CLR  (BIT(15))
#define UHCI_SEND_A_Q_INT_CLR_M  (BIT(15))
#define UHCI_SEND_A_Q_INT_CLR_V  0x1
#define UHCI_SEND_A_Q_INT_CLR_S  15
/* UHCI_SEND_S_Q_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define UHCI_SEND_S_Q_INT_CLR  (BIT(14))
#define UHCI_SEND_S_Q_INT_CLR_M  (BIT(14))
#define UHCI_SEND_S_Q_INT_CLR_V  0x1
#define UHCI_SEND_S_Q_INT_CLR_S  14
/* UHCI_OUT_TOTAL_EOF_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_TOTAL_EOF_INT_CLR  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_CLR_M  (BIT(13))
#define UHCI_OUT_TOTAL_EOF_INT_CLR_V  0x1
#define UHCI_OUT_TOTAL_EOF_INT_CLR_S  13
/* UHCI_OUTLINK_EOF_ERR_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTLINK_EOF_ERR_INT_CLR  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_M  (BIT(12))
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_V  0x1
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_S  12
/* UHCI_IN_DSCR_EMPTY_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_EMPTY_INT_CLR  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_CLR_M  (BIT(11))
#define UHCI_IN_DSCR_EMPTY_INT_CLR_V  0x1
#define UHCI_IN_DSCR_EMPTY_INT_CLR_S  11
/* UHCI_OUT_DSCR_ERR_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DSCR_ERR_INT_CLR  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_CLR_M  (BIT(10))
#define UHCI_OUT_DSCR_ERR_INT_CLR_V  0x1
#define UHCI_OUT_DSCR_ERR_INT_CLR_S  10
/* UHCI_IN_DSCR_ERR_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DSCR_ERR_INT_CLR  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_CLR_M  (BIT(9))
#define UHCI_IN_DSCR_ERR_INT_CLR_V  0x1
#define UHCI_IN_DSCR_ERR_INT_CLR_S  9
/* UHCI_OUT_EOF_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_EOF_INT_CLR  (BIT(8))
#define UHCI_OUT_EOF_INT_CLR_M  (BIT(8))
#define UHCI_OUT_EOF_INT_CLR_V  0x1
#define UHCI_OUT_EOF_INT_CLR_S  8
/* UHCI_OUT_DONE_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_DONE_INT_CLR  (BIT(7))
#define UHCI_OUT_DONE_INT_CLR_M  (BIT(7))
#define UHCI_OUT_DONE_INT_CLR_V  0x1
#define UHCI_OUT_DONE_INT_CLR_S  7
/* UHCI_IN_ERR_EOF_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_ERR_EOF_INT_CLR  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_CLR_M  (BIT(6))
#define UHCI_IN_ERR_EOF_INT_CLR_V  0x1
#define UHCI_IN_ERR_EOF_INT_CLR_S  6
/* UHCI_IN_SUC_EOF_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_SUC_EOF_INT_CLR  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_CLR_M  (BIT(5))
#define UHCI_IN_SUC_EOF_INT_CLR_V  0x1
#define UHCI_IN_SUC_EOF_INT_CLR_S  5
/* UHCI_IN_DONE_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_DONE_INT_CLR  (BIT(4))
#define UHCI_IN_DONE_INT_CLR_M  (BIT(4))
#define UHCI_IN_DONE_INT_CLR_V  0x1
#define UHCI_IN_DONE_INT_CLR_S  4
/* UHCI_TX_HUNG_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_HUNG_INT_CLR  (BIT(3))
#define UHCI_TX_HUNG_INT_CLR_M  (BIT(3))
#define UHCI_TX_HUNG_INT_CLR_V  0x1
#define UHCI_TX_HUNG_INT_CLR_S  3
/* UHCI_RX_HUNG_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_HUNG_INT_CLR  (BIT(2))
#define UHCI_RX_HUNG_INT_CLR_M  (BIT(2))
#define UHCI_RX_HUNG_INT_CLR_V  0x1
#define UHCI_RX_HUNG_INT_CLR_S  2
/* UHCI_TX_START_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_START_INT_CLR  (BIT(1))
#define UHCI_TX_START_INT_CLR_M  (BIT(1))
#define UHCI_TX_START_INT_CLR_V  0x1
#define UHCI_TX_START_INT_CLR_S  1
/* UHCI_RX_START_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_START_INT_CLR  (BIT(0))
#define UHCI_RX_START_INT_CLR_M  (BIT(0))
#define UHCI_RX_START_INT_CLR_V  0x1
#define UHCI_RX_START_INT_CLR_S  0

#define UHCI_DMA_OUT_STATUS_REG(i)          (REG_UHCI_BASE(i) + 0x14)
/* UHCI_OUT_EMPTY : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define UHCI_OUT_EMPTY  (BIT(1))
#define UHCI_OUT_EMPTY_M  (BIT(1))
#define UHCI_OUT_EMPTY_V  0x1
#define UHCI_OUT_EMPTY_S  1
/* UHCI_OUT_FULL : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUT_FULL  (BIT(0))
#define UHCI_OUT_FULL_M  (BIT(0))
#define UHCI_OUT_FULL_V  0x1
#define UHCI_OUT_FULL_S  0

#define UHCI_DMA_OUT_PUSH_REG(i)          (REG_UHCI_BASE(i) + 0x18)
/* UHCI_OUTFIFO_PUSH : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define UHCI_OUTFIFO_PUSH  (BIT(16))
#define UHCI_OUTFIFO_PUSH_M  (BIT(16))
#define UHCI_OUTFIFO_PUSH_V  0x1
#define UHCI_OUTFIFO_PUSH_S  16
/* UHCI_OUTFIFO_WDATA : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: */
#define UHCI_OUTFIFO_WDATA  0x000001FF
#define UHCI_OUTFIFO_WDATA_M  ((UHCI_OUTFIFO_WDATA_V)<<(UHCI_OUTFIFO_WDATA_S))
#define UHCI_OUTFIFO_WDATA_V  0x1FF
#define UHCI_OUTFIFO_WDATA_S  0

#define UHCI_DMA_IN_STATUS_REG(i)          (REG_UHCI_BASE(i) + 0x1C)
/* UHCI_RX_ERR_CAUSE : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: */
#define UHCI_RX_ERR_CAUSE  0x00000007
#define UHCI_RX_ERR_CAUSE_M  ((UHCI_RX_ERR_CAUSE_V)<<(UHCI_RX_ERR_CAUSE_S))
#define UHCI_RX_ERR_CAUSE_V  0x7
#define UHCI_RX_ERR_CAUSE_S  4
/* UHCI_IN_EMPTY : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define UHCI_IN_EMPTY  (BIT(1))
#define UHCI_IN_EMPTY_M  (BIT(1))
#define UHCI_IN_EMPTY_V  0x1
#define UHCI_IN_EMPTY_S  1
/* UHCI_IN_FULL : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define UHCI_IN_FULL  (BIT(0))
#define UHCI_IN_FULL_M  (BIT(0))
#define UHCI_IN_FULL_V  0x1
#define UHCI_IN_FULL_S  0

#define UHCI_DMA_IN_POP_REG(i)          (REG_UHCI_BASE(i) + 0x20)
/* UHCI_INFIFO_POP : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define UHCI_INFIFO_POP  (BIT(16))
#define UHCI_INFIFO_POP_M  (BIT(16))
#define UHCI_INFIFO_POP_V  0x1
#define UHCI_INFIFO_POP_S  16
/* UHCI_INFIFO_RDATA : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define UHCI_INFIFO_RDATA  0x00000FFF
#define UHCI_INFIFO_RDATA_M  ((UHCI_INFIFO_RDATA_V)<<(UHCI_INFIFO_RDATA_S))
#define UHCI_INFIFO_RDATA_V  0xFFF
#define UHCI_INFIFO_RDATA_S  0

#define UHCI_DMA_OUT_LINK_REG(i)          (REG_UHCI_BASE(i) + 0x24)
/* UHCI_OUTLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define UHCI_OUTLINK_PARK  (BIT(31))
#define UHCI_OUTLINK_PARK_M  (BIT(31))
#define UHCI_OUTLINK_PARK_V  0x1
#define UHCI_OUTLINK_PARK_S  31
/* UHCI_OUTLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTLINK_RESTART  (BIT(30))
#define UHCI_OUTLINK_RESTART_M  (BIT(30))
#define UHCI_OUTLINK_RESTART_V  0x1
#define UHCI_OUTLINK_RESTART_S  30
/* UHCI_OUTLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTLINK_START  (BIT(29))
#define UHCI_OUTLINK_START_M  (BIT(29))
#define UHCI_OUTLINK_START_V  0x1
#define UHCI_OUTLINK_START_S  29
/* UHCI_OUTLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define UHCI_OUTLINK_STOP  (BIT(28))
#define UHCI_OUTLINK_STOP_M  (BIT(28))
#define UHCI_OUTLINK_STOP_V  0x1
#define UHCI_OUTLINK_STOP_S  28
/* UHCI_OUTLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define UHCI_OUTLINK_ADDR  0x000FFFFF
#define UHCI_OUTLINK_ADDR_M  ((UHCI_OUTLINK_ADDR_V)<<(UHCI_OUTLINK_ADDR_S))
#define UHCI_OUTLINK_ADDR_V  0xFFFFF
#define UHCI_OUTLINK_ADDR_S  0

#define UHCI_DMA_IN_LINK_REG(i)          (REG_UHCI_BASE(i) + 0x28)
/* UHCI_INLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define UHCI_INLINK_PARK  (BIT(31))
#define UHCI_INLINK_PARK_M  (BIT(31))
#define UHCI_INLINK_PARK_V  0x1
#define UHCI_INLINK_PARK_S  31
/* UHCI_INLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define UHCI_INLINK_RESTART  (BIT(30))
#define UHCI_INLINK_RESTART_M  (BIT(30))
#define UHCI_INLINK_RESTART_V  0x1
#define UHCI_INLINK_RESTART_S  30
/* UHCI_INLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define UHCI_INLINK_START  (BIT(29))
#define UHCI_INLINK_START_M  (BIT(29))
#define UHCI_INLINK_START_V  0x1
#define UHCI_INLINK_START_S  29
/* UHCI_INLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define UHCI_INLINK_STOP  (BIT(28))
#define UHCI_INLINK_STOP_M  (BIT(28))
#define UHCI_INLINK_STOP_V  0x1
#define UHCI_INLINK_STOP_S  28
/* UHCI_INLINK_AUTO_RET : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: */
#define UHCI_INLINK_AUTO_RET  (BIT(20))
#define UHCI_INLINK_AUTO_RET_M  (BIT(20))
#define UHCI_INLINK_AUTO_RET_V  0x1
#define UHCI_INLINK_AUTO_RET_S  20
/* UHCI_INLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define UHCI_INLINK_ADDR  0x000FFFFF
#define UHCI_INLINK_ADDR_M  ((UHCI_INLINK_ADDR_V)<<(UHCI_INLINK_ADDR_S))
#define UHCI_INLINK_ADDR_V  0xFFFFF
#define UHCI_INLINK_ADDR_S  0

#define UHCI_CONF1_REG(i)          (REG_UHCI_BASE(i) + 0x2C)
/* UHCI_DMA_INFIFO_FULL_THRS : R/W ;bitpos:[20:9] ;default: 12'b0 ; */
/*description: */
#define UHCI_DMA_INFIFO_FULL_THRS  0x00000FFF
#define UHCI_DMA_INFIFO_FULL_THRS_M  ((UHCI_DMA_INFIFO_FULL_THRS_V)<<(UHCI_DMA_INFIFO_FULL_THRS_S))
#define UHCI_DMA_INFIFO_FULL_THRS_V  0xFFF
#define UHCI_DMA_INFIFO_FULL_THRS_S  9
/* UHCI_SW_START : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define UHCI_SW_START  (BIT(8))
#define UHCI_SW_START_M  (BIT(8))
#define UHCI_SW_START_V  0x1
#define UHCI_SW_START_S  8
/* UHCI_WAIT_SW_START : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_WAIT_SW_START  (BIT(7))
#define UHCI_WAIT_SW_START_M  (BIT(7))
#define UHCI_WAIT_SW_START_V  0x1
#define UHCI_WAIT_SW_START_S  7
/* UHCI_CHECK_OWNER : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UHCI_CHECK_OWNER  (BIT(6))
#define UHCI_CHECK_OWNER_M  (BIT(6))
#define UHCI_CHECK_OWNER_V  0x1
#define UHCI_CHECK_OWNER_S  6
/* UHCI_TX_ACK_NUM_RE : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define UHCI_TX_ACK_NUM_RE  (BIT(5))
#define UHCI_TX_ACK_NUM_RE_M  (BIT(5))
#define UHCI_TX_ACK_NUM_RE_V  0x1
#define UHCI_TX_ACK_NUM_RE_S  5
/* UHCI_TX_CHECK_SUM_RE : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define UHCI_TX_CHECK_SUM_RE  (BIT(4))
#define UHCI_TX_CHECK_SUM_RE_M  (BIT(4))
#define UHCI_TX_CHECK_SUM_RE_V  0x1
#define UHCI_TX_CHECK_SUM_RE_S  4
/* UHCI_SAVE_HEAD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_SAVE_HEAD  (BIT(3))
#define UHCI_SAVE_HEAD_M  (BIT(3))
#define UHCI_SAVE_HEAD_V  0x1
#define UHCI_SAVE_HEAD_S  3
/* UHCI_CRC_DISABLE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UHCI_CRC_DISABLE  (BIT(2))
#define UHCI_CRC_DISABLE_M  (BIT(2))
#define UHCI_CRC_DISABLE_V  0x1
#define UHCI_CRC_DISABLE_S  2
/* UHCI_CHECK_SEQ_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define UHCI_CHECK_SEQ_EN  (BIT(1))
#define UHCI_CHECK_SEQ_EN_M  (BIT(1))
#define UHCI_CHECK_SEQ_EN_V  0x1
#define UHCI_CHECK_SEQ_EN_S  1
/* UHCI_CHECK_SUM_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define UHCI_CHECK_SUM_EN  (BIT(0))
#define UHCI_CHECK_SUM_EN_M  (BIT(0))
#define UHCI_CHECK_SUM_EN_V  0x1
#define UHCI_CHECK_SUM_EN_S  0

#define UHCI_STATE0_REG(i)          (REG_UHCI_BASE(i) + 0x30)
/* UHCI_DECODE_STATE : RO ;bitpos:[30:28] ;default: 3'b0 ; */
/*description: */
#define UHCI_DECODE_STATE  0x00000007
#define UHCI_DECODE_STATE_M  ((UHCI_DECODE_STATE_V)<<(UHCI_DECODE_STATE_S))
#define UHCI_DECODE_STATE_V  0x7
#define UHCI_DECODE_STATE_S  28
/* UHCI_INFIFO_CNT_DEBUG : RO ;bitpos:[27:23] ;default: 5'b0 ; */
/*description: */
#define UHCI_INFIFO_CNT_DEBUG  0x0000001F
#define UHCI_INFIFO_CNT_DEBUG_M  ((UHCI_INFIFO_CNT_DEBUG_V)<<(UHCI_INFIFO_CNT_DEBUG_S))
#define UHCI_INFIFO_CNT_DEBUG_V  0x1F
#define UHCI_INFIFO_CNT_DEBUG_S  23
/* UHCI_IN_STATE : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define UHCI_IN_STATE  0x00000007
#define UHCI_IN_STATE_M  ((UHCI_IN_STATE_V)<<(UHCI_IN_STATE_S))
#define UHCI_IN_STATE_V  0x7
#define UHCI_IN_STATE_S  20
/* UHCI_IN_DSCR_STATE : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: */
#define UHCI_IN_DSCR_STATE  0x00000003
#define UHCI_IN_DSCR_STATE_M  ((UHCI_IN_DSCR_STATE_V)<<(UHCI_IN_DSCR_STATE_S))
#define UHCI_IN_DSCR_STATE_V  0x3
#define UHCI_IN_DSCR_STATE_S  18
/* UHCI_INLINK_DSCR_ADDR : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: */
#define UHCI_INLINK_DSCR_ADDR  0x0003FFFF
#define UHCI_INLINK_DSCR_ADDR_M  ((UHCI_INLINK_DSCR_ADDR_V)<<(UHCI_INLINK_DSCR_ADDR_S))
#define UHCI_INLINK_DSCR_ADDR_V  0x3FFFF
#define UHCI_INLINK_DSCR_ADDR_S  0

#define UHCI_STATE1_REG(i)          (REG_UHCI_BASE(i) + 0x34)
/* UHCI_ENCODE_STATE : RO ;bitpos:[30:28] ;default: 3'b0 ; */
/*description: */
#define UHCI_ENCODE_STATE  0x00000007
#define UHCI_ENCODE_STATE_M  ((UHCI_ENCODE_STATE_V)<<(UHCI_ENCODE_STATE_S))
#define UHCI_ENCODE_STATE_V  0x7
#define UHCI_ENCODE_STATE_S  28
/* UHCI_OUTFIFO_CNT : RO ;bitpos:[27:23] ;default: 5'b0 ; */
/*description: */
#define UHCI_OUTFIFO_CNT  0x0000001F
#define UHCI_OUTFIFO_CNT_M  ((UHCI_OUTFIFO_CNT_V)<<(UHCI_OUTFIFO_CNT_S))
#define UHCI_OUTFIFO_CNT_V  0x1F
#define UHCI_OUTFIFO_CNT_S  23
/* UHCI_OUT_STATE : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define UHCI_OUT_STATE  0x00000007
#define UHCI_OUT_STATE_M  ((UHCI_OUT_STATE_V)<<(UHCI_OUT_STATE_S))
#define UHCI_OUT_STATE_V  0x7
#define UHCI_OUT_STATE_S  20
/* UHCI_OUT_DSCR_STATE : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: */
#define UHCI_OUT_DSCR_STATE  0x00000003
#define UHCI_OUT_DSCR_STATE_M  ((UHCI_OUT_DSCR_STATE_V)<<(UHCI_OUT_DSCR_STATE_S))
#define UHCI_OUT_DSCR_STATE_V  0x3
#define UHCI_OUT_DSCR_STATE_S  18
/* UHCI_OUTLINK_DSCR_ADDR : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: */
#define UHCI_OUTLINK_DSCR_ADDR  0x0003FFFF
#define UHCI_OUTLINK_DSCR_ADDR_M  ((UHCI_OUTLINK_DSCR_ADDR_V)<<(UHCI_OUTLINK_DSCR_ADDR_S))
#define UHCI_OUTLINK_DSCR_ADDR_V  0x3FFFF
#define UHCI_OUTLINK_DSCR_ADDR_S  0

#define UHCI_DMA_OUT_EOF_DES_ADDR_REG(i)          (REG_UHCI_BASE(i) + 0x38)
/* UHCI_OUT_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_OUT_EOF_DES_ADDR  0xFFFFFFFF
#define UHCI_OUT_EOF_DES_ADDR_M  ((UHCI_OUT_EOF_DES_ADDR_V)<<(UHCI_OUT_EOF_DES_ADDR_S))
#define UHCI_OUT_EOF_DES_ADDR_V  0xFFFFFFFF
#define UHCI_OUT_EOF_DES_ADDR_S  0

#define UHCI_DMA_IN_SUC_EOF_DES_ADDR_REG(i)          (REG_UHCI_BASE(i) + 0x3C)
/* UHCI_IN_SUC_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_IN_SUC_EOF_DES_ADDR  0xFFFFFFFF
#define UHCI_IN_SUC_EOF_DES_ADDR_M  ((UHCI_IN_SUC_EOF_DES_ADDR_V)<<(UHCI_IN_SUC_EOF_DES_ADDR_S))
#define UHCI_IN_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define UHCI_IN_SUC_EOF_DES_ADDR_S  0

#define UHCI_DMA_IN_ERR_EOF_DES_ADDR_REG(i)          (REG_UHCI_BASE(i) + 0x40)
/* UHCI_IN_ERR_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_IN_ERR_EOF_DES_ADDR  0xFFFFFFFF
#define UHCI_IN_ERR_EOF_DES_ADDR_M  ((UHCI_IN_ERR_EOF_DES_ADDR_V)<<(UHCI_IN_ERR_EOF_DES_ADDR_S))
#define UHCI_IN_ERR_EOF_DES_ADDR_V  0xFFFFFFFF
#define UHCI_IN_ERR_EOF_DES_ADDR_S  0

#define UHCI_DMA_OUT_EOF_BFR_DES_ADDR_REG(i)          (REG_UHCI_BASE(i) + 0x44)
/* UHCI_OUT_EOF_BFR_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_OUT_EOF_BFR_DES_ADDR  0xFFFFFFFF
#define UHCI_OUT_EOF_BFR_DES_ADDR_M  ((UHCI_OUT_EOF_BFR_DES_ADDR_V)<<(UHCI_OUT_EOF_BFR_DES_ADDR_S))
#define UHCI_OUT_EOF_BFR_DES_ADDR_V  0xFFFFFFFF
#define UHCI_OUT_EOF_BFR_DES_ADDR_S  0

#define UHCI_AHB_TEST_REG(i)          (REG_UHCI_BASE(i) + 0x48)
/* UHCI_AHB_TESTADDR : R/W ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: */
#define UHCI_AHB_TESTADDR  0x00000003
#define UHCI_AHB_TESTADDR_M  ((UHCI_AHB_TESTADDR_V)<<(UHCI_AHB_TESTADDR_S))
#define UHCI_AHB_TESTADDR_V  0x3
#define UHCI_AHB_TESTADDR_S  4
/* UHCI_AHB_TESTMODE : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define UHCI_AHB_TESTMODE  0x00000007
#define UHCI_AHB_TESTMODE_M  ((UHCI_AHB_TESTMODE_V)<<(UHCI_AHB_TESTMODE_S))
#define UHCI_AHB_TESTMODE_V  0x7
#define UHCI_AHB_TESTMODE_S  0

#define UHCI_DMA_IN_DSCR_REG(i)          (REG_UHCI_BASE(i) + 0x4C)
/* UHCI_INLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define UHCI_INLINK_DSCR  0xFFFFFFFF
#define UHCI_INLINK_DSCR_M  ((UHCI_INLINK_DSCR_V)<<(UHCI_INLINK_DSCR_S))
#define UHCI_INLINK_DSCR_V  0xFFFFFFFF
#define UHCI_INLINK_DSCR_S  0

#define UHCI_DMA_IN_DSCR_BF0_REG(i)          (REG_UHCI_BASE(i) + 0x50)
/* UHCI_INLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define UHCI_INLINK_DSCR_BF0  0xFFFFFFFF
#define UHCI_INLINK_DSCR_BF0_M  ((UHCI_INLINK_DSCR_BF0_V)<<(UHCI_INLINK_DSCR_BF0_S))
#define UHCI_INLINK_DSCR_BF0_V  0xFFFFFFFF
#define UHCI_INLINK_DSCR_BF0_S  0

#define UHCI_DMA_IN_DSCR_BF1_REG(i)          (REG_UHCI_BASE(i) + 0x54)
/* UHCI_INLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define UHCI_INLINK_DSCR_BF1  0xFFFFFFFF
#define UHCI_INLINK_DSCR_BF1_M  ((UHCI_INLINK_DSCR_BF1_V)<<(UHCI_INLINK_DSCR_BF1_S))
#define UHCI_INLINK_DSCR_BF1_V  0xFFFFFFFF
#define UHCI_INLINK_DSCR_BF1_S  0

#define UHCI_DMA_OUT_DSCR_REG(i)          (REG_UHCI_BASE(i) + 0x58)
/* UHCI_OUTLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define UHCI_OUTLINK_DSCR  0xFFFFFFFF
#define UHCI_OUTLINK_DSCR_M  ((UHCI_OUTLINK_DSCR_V)<<(UHCI_OUTLINK_DSCR_S))
#define UHCI_OUTLINK_DSCR_V  0xFFFFFFFF
#define UHCI_OUTLINK_DSCR_S  0

#define UHCI_DMA_OUT_DSCR_BF0_REG(i)          (REG_UHCI_BASE(i) + 0x5C)
/* UHCI_OUTLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define UHCI_OUTLINK_DSCR_BF0  0xFFFFFFFF
#define UHCI_OUTLINK_DSCR_BF0_M  ((UHCI_OUTLINK_DSCR_BF0_V)<<(UHCI_OUTLINK_DSCR_BF0_S))
#define UHCI_OUTLINK_DSCR_BF0_V  0xFFFFFFFF
#define UHCI_OUTLINK_DSCR_BF0_S  0

#define UHCI_DMA_OUT_DSCR_BF1_REG(i)          (REG_UHCI_BASE(i) + 0x60)
/* UHCI_OUTLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define UHCI_OUTLINK_DSCR_BF1  0xFFFFFFFF
#define UHCI_OUTLINK_DSCR_BF1_M  ((UHCI_OUTLINK_DSCR_BF1_V)<<(UHCI_OUTLINK_DSCR_BF1_S))
#define UHCI_OUTLINK_DSCR_BF1_V  0xFFFFFFFF
#define UHCI_OUTLINK_DSCR_BF1_S  0

#define UHCI_ESCAPE_CONF_REG(i)          (REG_UHCI_BASE(i) + 0x64)
/* UHCI_RX_13_ESC_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_13_ESC_EN  (BIT(7))
#define UHCI_RX_13_ESC_EN_M  (BIT(7))
#define UHCI_RX_13_ESC_EN_V  0x1
#define UHCI_RX_13_ESC_EN_S  7
/* UHCI_RX_11_ESC_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define UHCI_RX_11_ESC_EN  (BIT(6))
#define UHCI_RX_11_ESC_EN_M  (BIT(6))
#define UHCI_RX_11_ESC_EN_V  0x1
#define UHCI_RX_11_ESC_EN_S  6
/* UHCI_RX_DB_ESC_EN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define UHCI_RX_DB_ESC_EN  (BIT(5))
#define UHCI_RX_DB_ESC_EN_M  (BIT(5))
#define UHCI_RX_DB_ESC_EN_V  0x1
#define UHCI_RX_DB_ESC_EN_S  5
/* UHCI_RX_C0_ESC_EN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define UHCI_RX_C0_ESC_EN  (BIT(4))
#define UHCI_RX_C0_ESC_EN_M  (BIT(4))
#define UHCI_RX_C0_ESC_EN_V  0x1
#define UHCI_RX_C0_ESC_EN_S  4
/* UHCI_TX_13_ESC_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_13_ESC_EN  (BIT(3))
#define UHCI_TX_13_ESC_EN_M  (BIT(3))
#define UHCI_TX_13_ESC_EN_V  0x1
#define UHCI_TX_13_ESC_EN_S  3
/* UHCI_TX_11_ESC_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define UHCI_TX_11_ESC_EN  (BIT(2))
#define UHCI_TX_11_ESC_EN_M  (BIT(2))
#define UHCI_TX_11_ESC_EN_V  0x1
#define UHCI_TX_11_ESC_EN_S  2
/* UHCI_TX_DB_ESC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define UHCI_TX_DB_ESC_EN  (BIT(1))
#define UHCI_TX_DB_ESC_EN_M  (BIT(1))
#define UHCI_TX_DB_ESC_EN_V  0x1
#define UHCI_TX_DB_ESC_EN_S  1
/* UHCI_TX_C0_ESC_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define UHCI_TX_C0_ESC_EN  (BIT(0))
#define UHCI_TX_C0_ESC_EN_M  (BIT(0))
#define UHCI_TX_C0_ESC_EN_V  0x1
#define UHCI_TX_C0_ESC_EN_S  0

#define UHCI_HUNG_CONF_REG(i)          (REG_UHCI_BASE(i) + 0x68)
/* UHCI_RXFIFO_TIMEOUT_ENA : R/W ;bitpos:[23] ;default: 1'b1 ; */
/*description: */
#define UHCI_RXFIFO_TIMEOUT_ENA  (BIT(23))
#define UHCI_RXFIFO_TIMEOUT_ENA_M  (BIT(23))
#define UHCI_RXFIFO_TIMEOUT_ENA_V  0x1
#define UHCI_RXFIFO_TIMEOUT_ENA_S  23
/* UHCI_RXFIFO_TIMEOUT_SHIFT : R/W ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define UHCI_RXFIFO_TIMEOUT_SHIFT  0x00000007
#define UHCI_RXFIFO_TIMEOUT_SHIFT_M  ((UHCI_RXFIFO_TIMEOUT_SHIFT_V)<<(UHCI_RXFIFO_TIMEOUT_SHIFT_S))
#define UHCI_RXFIFO_TIMEOUT_SHIFT_V  0x7
#define UHCI_RXFIFO_TIMEOUT_SHIFT_S  20
/* UHCI_RXFIFO_TIMEOUT : R/W ;bitpos:[19:12] ;default: 8'h10 ; */
/*description: */
#define UHCI_RXFIFO_TIMEOUT  0x000000FF
#define UHCI_RXFIFO_TIMEOUT_M  ((UHCI_RXFIFO_TIMEOUT_V)<<(UHCI_RXFIFO_TIMEOUT_S))
#define UHCI_RXFIFO_TIMEOUT_V  0xFF
#define UHCI_RXFIFO_TIMEOUT_S  12
/* UHCI_TXFIFO_TIMEOUT_ENA : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define UHCI_TXFIFO_TIMEOUT_ENA  (BIT(11))
#define UHCI_TXFIFO_TIMEOUT_ENA_M  (BIT(11))
#define UHCI_TXFIFO_TIMEOUT_ENA_V  0x1
#define UHCI_TXFIFO_TIMEOUT_ENA_S  11
/* UHCI_TXFIFO_TIMEOUT_SHIFT : R/W ;bitpos:[10:8] ;default: 3'b0 ; */
/*description: */
#define UHCI_TXFIFO_TIMEOUT_SHIFT  0x00000007
#define UHCI_TXFIFO_TIMEOUT_SHIFT_M  ((UHCI_TXFIFO_TIMEOUT_SHIFT_V)<<(UHCI_TXFIFO_TIMEOUT_SHIFT_S))
#define UHCI_TXFIFO_TIMEOUT_SHIFT_V  0x7
#define UHCI_TXFIFO_TIMEOUT_SHIFT_S  8
/* UHCI_TXFIFO_TIMEOUT : R/W ;bitpos:[7:0] ;default: 8'h10 ; */
/*description: */
#define UHCI_TXFIFO_TIMEOUT  0x000000FF
#define UHCI_TXFIFO_TIMEOUT_M  ((UHCI_TXFIFO_TIMEOUT_V)<<(UHCI_TXFIFO_TIMEOUT_S))
#define UHCI_TXFIFO_TIMEOUT_V  0xFF
#define UHCI_TXFIFO_TIMEOUT_S  0

#define UHCI_ACK_NUM_REG(i)          (REG_UHCI_BASE(i) + 0x6C)

#define UHCI_RX_HEAD_REG(i)          (REG_UHCI_BASE(i) + 0x70)
/* UHCI_RX_HEAD : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_RX_HEAD  0xFFFFFFFF
#define UHCI_RX_HEAD_M  ((UHCI_RX_HEAD_V)<<(UHCI_RX_HEAD_S))
#define UHCI_RX_HEAD_V  0xFFFFFFFF
#define UHCI_RX_HEAD_S  0

#define UHCI_QUICK_SENT_REG(i)          (REG_UHCI_BASE(i) + 0x74)
/* UHCI_ALWAYS_SEND_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define UHCI_ALWAYS_SEND_EN  (BIT(7))
#define UHCI_ALWAYS_SEND_EN_M  (BIT(7))
#define UHCI_ALWAYS_SEND_EN_V  0x1
#define UHCI_ALWAYS_SEND_EN_S  7
/* UHCI_ALWAYS_SEND_NUM : R/W ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: */
#define UHCI_ALWAYS_SEND_NUM  0x00000007
#define UHCI_ALWAYS_SEND_NUM_M  ((UHCI_ALWAYS_SEND_NUM_V)<<(UHCI_ALWAYS_SEND_NUM_S))
#define UHCI_ALWAYS_SEND_NUM_V  0x7
#define UHCI_ALWAYS_SEND_NUM_S  4
/* UHCI_SINGLE_SEND_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define UHCI_SINGLE_SEND_EN  (BIT(3))
#define UHCI_SINGLE_SEND_EN_M  (BIT(3))
#define UHCI_SINGLE_SEND_EN_V  0x1
#define UHCI_SINGLE_SEND_EN_S  3
/* UHCI_SINGLE_SEND_NUM : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: */
#define UHCI_SINGLE_SEND_NUM  0x00000007
#define UHCI_SINGLE_SEND_NUM_M  ((UHCI_SINGLE_SEND_NUM_V)<<(UHCI_SINGLE_SEND_NUM_S))
#define UHCI_SINGLE_SEND_NUM_V  0x7
#define UHCI_SINGLE_SEND_NUM_S  0

#define UHCI_Q0_WORD0_REG(i)          (REG_UHCI_BASE(i) + 0x78)
/* UHCI_SEND_Q0_WORD0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q0_WORD0  0xFFFFFFFF
#define UHCI_SEND_Q0_WORD0_M  ((UHCI_SEND_Q0_WORD0_V)<<(UHCI_SEND_Q0_WORD0_S))
#define UHCI_SEND_Q0_WORD0_V  0xFFFFFFFF
#define UHCI_SEND_Q0_WORD0_S  0

#define UHCI_Q0_WORD1_REG(i)          (REG_UHCI_BASE(i) + 0x7C)
/* UHCI_SEND_Q0_WORD1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q0_WORD1  0xFFFFFFFF
#define UHCI_SEND_Q0_WORD1_M  ((UHCI_SEND_Q0_WORD1_V)<<(UHCI_SEND_Q0_WORD1_S))
#define UHCI_SEND_Q0_WORD1_V  0xFFFFFFFF
#define UHCI_SEND_Q0_WORD1_S  0

#define UHCI_Q1_WORD0_REG(i)          (REG_UHCI_BASE(i) + 0x80)
/* UHCI_SEND_Q1_WORD0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q1_WORD0  0xFFFFFFFF
#define UHCI_SEND_Q1_WORD0_M  ((UHCI_SEND_Q1_WORD0_V)<<(UHCI_SEND_Q1_WORD0_S))
#define UHCI_SEND_Q1_WORD0_V  0xFFFFFFFF
#define UHCI_SEND_Q1_WORD0_S  0

#define UHCI_Q1_WORD1_REG(i)          (REG_UHCI_BASE(i) + 0x84)
/* UHCI_SEND_Q1_WORD1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q1_WORD1  0xFFFFFFFF
#define UHCI_SEND_Q1_WORD1_M  ((UHCI_SEND_Q1_WORD1_V)<<(UHCI_SEND_Q1_WORD1_S))
#define UHCI_SEND_Q1_WORD1_V  0xFFFFFFFF
#define UHCI_SEND_Q1_WORD1_S  0

#define UHCI_Q2_WORD0_REG(i)          (REG_UHCI_BASE(i) + 0x88)
/* UHCI_SEND_Q2_WORD0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q2_WORD0  0xFFFFFFFF
#define UHCI_SEND_Q2_WORD0_M  ((UHCI_SEND_Q2_WORD0_V)<<(UHCI_SEND_Q2_WORD0_S))
#define UHCI_SEND_Q2_WORD0_V  0xFFFFFFFF
#define UHCI_SEND_Q2_WORD0_S  0

#define UHCI_Q2_WORD1_REG(i)          (REG_UHCI_BASE(i) + 0x8C)
/* UHCI_SEND_Q2_WORD1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q2_WORD1  0xFFFFFFFF
#define UHCI_SEND_Q2_WORD1_M  ((UHCI_SEND_Q2_WORD1_V)<<(UHCI_SEND_Q2_WORD1_S))
#define UHCI_SEND_Q2_WORD1_V  0xFFFFFFFF
#define UHCI_SEND_Q2_WORD1_S  0

#define UHCI_Q3_WORD0_REG(i)          (REG_UHCI_BASE(i) + 0x90)
/* UHCI_SEND_Q3_WORD0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q3_WORD0  0xFFFFFFFF
#define UHCI_SEND_Q3_WORD0_M  ((UHCI_SEND_Q3_WORD0_V)<<(UHCI_SEND_Q3_WORD0_S))
#define UHCI_SEND_Q3_WORD0_V  0xFFFFFFFF
#define UHCI_SEND_Q3_WORD0_S  0

#define UHCI_Q3_WORD1_REG(i)          (REG_UHCI_BASE(i) + 0x94)
/* UHCI_SEND_Q3_WORD1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q3_WORD1  0xFFFFFFFF
#define UHCI_SEND_Q3_WORD1_M  ((UHCI_SEND_Q3_WORD1_V)<<(UHCI_SEND_Q3_WORD1_S))
#define UHCI_SEND_Q3_WORD1_V  0xFFFFFFFF
#define UHCI_SEND_Q3_WORD1_S  0

#define UHCI_Q4_WORD0_REG(i)          (REG_UHCI_BASE(i) + 0x98)
/* UHCI_SEND_Q4_WORD0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q4_WORD0  0xFFFFFFFF
#define UHCI_SEND_Q4_WORD0_M  ((UHCI_SEND_Q4_WORD0_V)<<(UHCI_SEND_Q4_WORD0_S))
#define UHCI_SEND_Q4_WORD0_V  0xFFFFFFFF
#define UHCI_SEND_Q4_WORD0_S  0

#define UHCI_Q4_WORD1_REG(i)          (REG_UHCI_BASE(i) + 0x9C)
/* UHCI_SEND_Q4_WORD1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q4_WORD1  0xFFFFFFFF
#define UHCI_SEND_Q4_WORD1_M  ((UHCI_SEND_Q4_WORD1_V)<<(UHCI_SEND_Q4_WORD1_S))
#define UHCI_SEND_Q4_WORD1_V  0xFFFFFFFF
#define UHCI_SEND_Q4_WORD1_S  0

#define UHCI_Q5_WORD0_REG(i)          (REG_UHCI_BASE(i) + 0xA0)
/* UHCI_SEND_Q5_WORD0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q5_WORD0  0xFFFFFFFF
#define UHCI_SEND_Q5_WORD0_M  ((UHCI_SEND_Q5_WORD0_V)<<(UHCI_SEND_Q5_WORD0_S))
#define UHCI_SEND_Q5_WORD0_V  0xFFFFFFFF
#define UHCI_SEND_Q5_WORD0_S  0

#define UHCI_Q5_WORD1_REG(i)          (REG_UHCI_BASE(i) + 0xA4)
/* UHCI_SEND_Q5_WORD1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q5_WORD1  0xFFFFFFFF
#define UHCI_SEND_Q5_WORD1_M  ((UHCI_SEND_Q5_WORD1_V)<<(UHCI_SEND_Q5_WORD1_S))
#define UHCI_SEND_Q5_WORD1_V  0xFFFFFFFF
#define UHCI_SEND_Q5_WORD1_S  0

#define UHCI_Q6_WORD0_REG(i)          (REG_UHCI_BASE(i) + 0xA8)
/* UHCI_SEND_Q6_WORD0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q6_WORD0  0xFFFFFFFF
#define UHCI_SEND_Q6_WORD0_M  ((UHCI_SEND_Q6_WORD0_V)<<(UHCI_SEND_Q6_WORD0_S))
#define UHCI_SEND_Q6_WORD0_V  0xFFFFFFFF
#define UHCI_SEND_Q6_WORD0_S  0

#define UHCI_Q6_WORD1_REG(i)          (REG_UHCI_BASE(i) + 0xAC)
/* UHCI_SEND_Q6_WORD1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define UHCI_SEND_Q6_WORD1  0xFFFFFFFF
#define UHCI_SEND_Q6_WORD1_M  ((UHCI_SEND_Q6_WORD1_V)<<(UHCI_SEND_Q6_WORD1_S))
#define UHCI_SEND_Q6_WORD1_V  0xFFFFFFFF
#define UHCI_SEND_Q6_WORD1_S  0

#define UHCI_ESC_CONF0_REG(i)          (REG_UHCI_BASE(i) + 0xB0)
/* UHCI_SEPER_ESC_CHAR1 : R/W ;bitpos:[23:16] ;default: 8'hdc ; */
/*description: */
#define UHCI_SEPER_ESC_CHAR1  0x000000FF
#define UHCI_SEPER_ESC_CHAR1_M  ((UHCI_SEPER_ESC_CHAR1_V)<<(UHCI_SEPER_ESC_CHAR1_S))
#define UHCI_SEPER_ESC_CHAR1_V  0xFF
#define UHCI_SEPER_ESC_CHAR1_S  16
/* UHCI_SEPER_ESC_CHAR0 : R/W ;bitpos:[15:8] ;default: 8'hdb ; */
/*description: */
#define UHCI_SEPER_ESC_CHAR0  0x000000FF
#define UHCI_SEPER_ESC_CHAR0_M  ((UHCI_SEPER_ESC_CHAR0_V)<<(UHCI_SEPER_ESC_CHAR0_S))
#define UHCI_SEPER_ESC_CHAR0_V  0xFF
#define UHCI_SEPER_ESC_CHAR0_S  8
/* UHCI_SEPER_CHAR : R/W ;bitpos:[7:0] ;default: 8'hc0 ; */
/*description: */
#define UHCI_SEPER_CHAR  0x000000FF
#define UHCI_SEPER_CHAR_M  ((UHCI_SEPER_CHAR_V)<<(UHCI_SEPER_CHAR_S))
#define UHCI_SEPER_CHAR_V  0xFF
#define UHCI_SEPER_CHAR_S  0

#define UHCI_ESC_CONF1_REG(i)          (REG_UHCI_BASE(i) + 0xB4)
/* UHCI_ESC_SEQ0_CHAR1 : R/W ;bitpos:[23:16] ;default: 8'hdd ; */
/*description: */
#define UHCI_ESC_SEQ0_CHAR1  0x000000FF
#define UHCI_ESC_SEQ0_CHAR1_M  ((UHCI_ESC_SEQ0_CHAR1_V)<<(UHCI_ESC_SEQ0_CHAR1_S))
#define UHCI_ESC_SEQ0_CHAR1_V  0xFF
#define UHCI_ESC_SEQ0_CHAR1_S  16
/* UHCI_ESC_SEQ0_CHAR0 : R/W ;bitpos:[15:8] ;default: 8'hdb ; */
/*description: */
#define UHCI_ESC_SEQ0_CHAR0  0x000000FF
#define UHCI_ESC_SEQ0_CHAR0_M  ((UHCI_ESC_SEQ0_CHAR0_V)<<(UHCI_ESC_SEQ0_CHAR0_S))
#define UHCI_ESC_SEQ0_CHAR0_V  0xFF
#define UHCI_ESC_SEQ0_CHAR0_S  8
/* UHCI_ESC_SEQ0 : R/W ;bitpos:[7:0] ;default: 8'hdb ; */
/*description: */
#define UHCI_ESC_SEQ0  0x000000FF
#define UHCI_ESC_SEQ0_M  ((UHCI_ESC_SEQ0_V)<<(UHCI_ESC_SEQ0_S))
#define UHCI_ESC_SEQ0_V  0xFF
#define UHCI_ESC_SEQ0_S  0

#define UHCI_ESC_CONF2_REG(i)          (REG_UHCI_BASE(i) + 0xB8)
/* UHCI_ESC_SEQ1_CHAR1 : R/W ;bitpos:[23:16] ;default: 8'hde ; */
/*description: */
#define UHCI_ESC_SEQ1_CHAR1  0x000000FF
#define UHCI_ESC_SEQ1_CHAR1_M  ((UHCI_ESC_SEQ1_CHAR1_V)<<(UHCI_ESC_SEQ1_CHAR1_S))
#define UHCI_ESC_SEQ1_CHAR1_V  0xFF
#define UHCI_ESC_SEQ1_CHAR1_S  16
/* UHCI_ESC_SEQ1_CHAR0 : R/W ;bitpos:[15:8] ;default: 8'hdb ; */
/*description: */
#define UHCI_ESC_SEQ1_CHAR0  0x000000FF
#define UHCI_ESC_SEQ1_CHAR0_M  ((UHCI_ESC_SEQ1_CHAR0_V)<<(UHCI_ESC_SEQ1_CHAR0_S))
#define UHCI_ESC_SEQ1_CHAR0_V  0xFF
#define UHCI_ESC_SEQ1_CHAR0_S  8
/* UHCI_ESC_SEQ1 : R/W ;bitpos:[7:0] ;default: 8'h11 ; */
/*description: */
#define UHCI_ESC_SEQ1  0x000000FF
#define UHCI_ESC_SEQ1_M  ((UHCI_ESC_SEQ1_V)<<(UHCI_ESC_SEQ1_S))
#define UHCI_ESC_SEQ1_V  0xFF
#define UHCI_ESC_SEQ1_S  0

#define UHCI_ESC_CONF3_REG(i)          (REG_UHCI_BASE(i) + 0xBC)
/* UHCI_ESC_SEQ2_CHAR1 : R/W ;bitpos:[23:16] ;default: 8'hdf ; */
/*description: */
#define UHCI_ESC_SEQ2_CHAR1  0x000000FF
#define UHCI_ESC_SEQ2_CHAR1_M  ((UHCI_ESC_SEQ2_CHAR1_V)<<(UHCI_ESC_SEQ2_CHAR1_S))
#define UHCI_ESC_SEQ2_CHAR1_V  0xFF
#define UHCI_ESC_SEQ2_CHAR1_S  16
/* UHCI_ESC_SEQ2_CHAR0 : R/W ;bitpos:[15:8] ;default: 8'hdb ; */
/*description: */
#define UHCI_ESC_SEQ2_CHAR0  0x000000FF
#define UHCI_ESC_SEQ2_CHAR0_M  ((UHCI_ESC_SEQ2_CHAR0_V)<<(UHCI_ESC_SEQ2_CHAR0_S))
#define UHCI_ESC_SEQ2_CHAR0_V  0xFF
#define UHCI_ESC_SEQ2_CHAR0_S  8
/* UHCI_ESC_SEQ2 : R/W ;bitpos:[7:0] ;default: 8'h13 ; */
/*description: */
#define UHCI_ESC_SEQ2  0x000000FF
#define UHCI_ESC_SEQ2_M  ((UHCI_ESC_SEQ2_V)<<(UHCI_ESC_SEQ2_S))
#define UHCI_ESC_SEQ2_V  0xFF
#define UHCI_ESC_SEQ2_S  0

#define UHCI_PKT_THRES_REG(i)          (REG_UHCI_BASE(i) + 0xC0)
/* UHCI_PKT_THRS : R/W ;bitpos:[12:0] ;default: 13'h80 ; */
/*description: */
#define UHCI_PKT_THRS  0x00001FFF
#define UHCI_PKT_THRS_M  ((UHCI_PKT_THRS_V)<<(UHCI_PKT_THRS_S))
#define UHCI_PKT_THRS_V  0x1FFF
#define UHCI_PKT_THRS_S  0

#define UHCI_DATE_REG(i)          (REG_UHCI_BASE(i) + 0xFC)
/* UHCI_DATE : R/W ;bitpos:[31:0] ;default: 32'h18073001 ; */
/*description: */
#define UHCI_DATE  0xFFFFFFFF
#define UHCI_DATE_M  ((UHCI_DATE_V)<<(UHCI_DATE_S))
#define UHCI_DATE_V  0xFFFFFFFF
#define UHCI_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_UHCI_REG_H_ */
