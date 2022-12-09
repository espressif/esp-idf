// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_SLC_REG_H_
#define _SOC_SLC_REG_H_


#include "soc.h"
#define SLC_CONF0_REG          (DR_REG_SLC_BASE + 0x0)
/* SLC_SLC1_TOKEN_SEL : R/W ;bitpos:[31] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC1_TOKEN_SEL  (BIT(31))
#define SLC_SLC1_TOKEN_SEL_M  (BIT(31))
#define SLC_SLC1_TOKEN_SEL_V  0x1
#define SLC_SLC1_TOKEN_SEL_S  31
/* SLC_SLC1_TOKEN_AUTO_CLR : R/W ;bitpos:[30] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC1_TOKEN_AUTO_CLR  (BIT(30))
#define SLC_SLC1_TOKEN_AUTO_CLR_M  (BIT(30))
#define SLC_SLC1_TOKEN_AUTO_CLR_V  0x1
#define SLC_SLC1_TOKEN_AUTO_CLR_S  30
/* SLC_SLC1_TXDATA_BURST_EN : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_TXDATA_BURST_EN  (BIT(29))
#define SLC_SLC1_TXDATA_BURST_EN_M  (BIT(29))
#define SLC_SLC1_TXDATA_BURST_EN_V  0x1
#define SLC_SLC1_TXDATA_BURST_EN_S  29
/* SLC_SLC1_TXDSCR_BURST_EN : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_TXDSCR_BURST_EN  (BIT(28))
#define SLC_SLC1_TXDSCR_BURST_EN_M  (BIT(28))
#define SLC_SLC1_TXDSCR_BURST_EN_V  0x1
#define SLC_SLC1_TXDSCR_BURST_EN_S  28
/* SLC_SLC1_TXLINK_AUTO_RET : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_TXLINK_AUTO_RET  (BIT(27))
#define SLC_SLC1_TXLINK_AUTO_RET_M  (BIT(27))
#define SLC_SLC1_TXLINK_AUTO_RET_V  0x1
#define SLC_SLC1_TXLINK_AUTO_RET_S  27
/* SLC_SLC1_RXLINK_AUTO_RET : R/W ;bitpos:[26] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RXLINK_AUTO_RET  (BIT(26))
#define SLC_SLC1_RXLINK_AUTO_RET_M  (BIT(26))
#define SLC_SLC1_RXLINK_AUTO_RET_V  0x1
#define SLC_SLC1_RXLINK_AUTO_RET_S  26
/* SLC_SLC1_RXDATA_BURST_EN : R/W ;bitpos:[25] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RXDATA_BURST_EN  (BIT(25))
#define SLC_SLC1_RXDATA_BURST_EN_M  (BIT(25))
#define SLC_SLC1_RXDATA_BURST_EN_V  0x1
#define SLC_SLC1_RXDATA_BURST_EN_S  25
/* SLC_SLC1_RXDSCR_BURST_EN : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RXDSCR_BURST_EN  (BIT(24))
#define SLC_SLC1_RXDSCR_BURST_EN_M  (BIT(24))
#define SLC_SLC1_RXDSCR_BURST_EN_V  0x1
#define SLC_SLC1_RXDSCR_BURST_EN_S  24
/* SLC_SLC1_RX_NO_RESTART_CLR : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_NO_RESTART_CLR  (BIT(23))
#define SLC_SLC1_RX_NO_RESTART_CLR_M  (BIT(23))
#define SLC_SLC1_RX_NO_RESTART_CLR_V  0x1
#define SLC_SLC1_RX_NO_RESTART_CLR_S  23
/* SLC_SLC1_RX_AUTO_WRBACK : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_AUTO_WRBACK  (BIT(22))
#define SLC_SLC1_RX_AUTO_WRBACK_M  (BIT(22))
#define SLC_SLC1_RX_AUTO_WRBACK_V  0x1
#define SLC_SLC1_RX_AUTO_WRBACK_S  22
/* SLC_SLC1_RX_LOOP_TEST : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RX_LOOP_TEST  (BIT(21))
#define SLC_SLC1_RX_LOOP_TEST_M  (BIT(21))
#define SLC_SLC1_RX_LOOP_TEST_V  0x1
#define SLC_SLC1_RX_LOOP_TEST_S  21
/* SLC_SLC1_TX_LOOP_TEST : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_TX_LOOP_TEST  (BIT(20))
#define SLC_SLC1_TX_LOOP_TEST_M  (BIT(20))
#define SLC_SLC1_TX_LOOP_TEST_V  0x1
#define SLC_SLC1_TX_LOOP_TEST_S  20
/* SLC_SLC1_WR_RETRY_MASK_EN : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_WR_RETRY_MASK_EN  (BIT(19))
#define SLC_SLC1_WR_RETRY_MASK_EN_M  (BIT(19))
#define SLC_SLC1_WR_RETRY_MASK_EN_V  0x1
#define SLC_SLC1_WR_RETRY_MASK_EN_S  19
/* SLC_SLC0_WR_RETRY_MASK_EN : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_WR_RETRY_MASK_EN  (BIT(18))
#define SLC_SLC0_WR_RETRY_MASK_EN_M  (BIT(18))
#define SLC_SLC0_WR_RETRY_MASK_EN_V  0x1
#define SLC_SLC0_WR_RETRY_MASK_EN_S  18
/* SLC_SLC1_RX_RST : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_RST  (BIT(17))
#define SLC_SLC1_RX_RST_M  (BIT(17))
#define SLC_SLC1_RX_RST_V  0x1
#define SLC_SLC1_RX_RST_S  17
/* SLC_SLC1_TX_RST : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC1_TX_RST  (BIT(16))
#define SLC_SLC1_TX_RST_M  (BIT(16))
#define SLC_SLC1_TX_RST_V  0x1
#define SLC_SLC1_TX_RST_S  16
/* SLC_SLC0_TOKEN_SEL : R/W ;bitpos:[15] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC0_TOKEN_SEL  (BIT(15))
#define SLC_SLC0_TOKEN_SEL_M  (BIT(15))
#define SLC_SLC0_TOKEN_SEL_V  0x1
#define SLC_SLC0_TOKEN_SEL_S  15
/* SLC_SLC0_TOKEN_AUTO_CLR : R/W ;bitpos:[14] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC0_TOKEN_AUTO_CLR  (BIT(14))
#define SLC_SLC0_TOKEN_AUTO_CLR_M  (BIT(14))
#define SLC_SLC0_TOKEN_AUTO_CLR_V  0x1
#define SLC_SLC0_TOKEN_AUTO_CLR_S  14
/* SLC_SLC0_TXDATA_BURST_EN : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_TXDATA_BURST_EN  (BIT(13))
#define SLC_SLC0_TXDATA_BURST_EN_M  (BIT(13))
#define SLC_SLC0_TXDATA_BURST_EN_V  0x1
#define SLC_SLC0_TXDATA_BURST_EN_S  13
/* SLC_SLC0_TXDSCR_BURST_EN : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_TXDSCR_BURST_EN  (BIT(12))
#define SLC_SLC0_TXDSCR_BURST_EN_M  (BIT(12))
#define SLC_SLC0_TXDSCR_BURST_EN_V  0x1
#define SLC_SLC0_TXDSCR_BURST_EN_S  12
/* SLC_SLC0_TXLINK_AUTO_RET : R/W ;bitpos:[11] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC0_TXLINK_AUTO_RET  (BIT(11))
#define SLC_SLC0_TXLINK_AUTO_RET_M  (BIT(11))
#define SLC_SLC0_TXLINK_AUTO_RET_V  0x1
#define SLC_SLC0_TXLINK_AUTO_RET_S  11
/* SLC_SLC0_RXLINK_AUTO_RET : R/W ;bitpos:[10] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC0_RXLINK_AUTO_RET  (BIT(10))
#define SLC_SLC0_RXLINK_AUTO_RET_M  (BIT(10))
#define SLC_SLC0_RXLINK_AUTO_RET_V  0x1
#define SLC_SLC0_RXLINK_AUTO_RET_S  10
/* SLC_SLC0_RXDATA_BURST_EN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_RXDATA_BURST_EN  (BIT(9))
#define SLC_SLC0_RXDATA_BURST_EN_M  (BIT(9))
#define SLC_SLC0_RXDATA_BURST_EN_V  0x1
#define SLC_SLC0_RXDATA_BURST_EN_S  9
/* SLC_SLC0_RXDSCR_BURST_EN : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_RXDSCR_BURST_EN  (BIT(8))
#define SLC_SLC0_RXDSCR_BURST_EN_M  (BIT(8))
#define SLC_SLC0_RXDSCR_BURST_EN_V  0x1
#define SLC_SLC0_RXDSCR_BURST_EN_S  8
/* SLC_SLC0_RX_NO_RESTART_CLR : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_NO_RESTART_CLR  (BIT(7))
#define SLC_SLC0_RX_NO_RESTART_CLR_M  (BIT(7))
#define SLC_SLC0_RX_NO_RESTART_CLR_V  0x1
#define SLC_SLC0_RX_NO_RESTART_CLR_S  7
/* SLC_SLC0_RX_AUTO_WRBACK : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_AUTO_WRBACK  (BIT(6))
#define SLC_SLC0_RX_AUTO_WRBACK_M  (BIT(6))
#define SLC_SLC0_RX_AUTO_WRBACK_V  0x1
#define SLC_SLC0_RX_AUTO_WRBACK_S  6
/* SLC_SLC0_RX_LOOP_TEST : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_RX_LOOP_TEST  (BIT(5))
#define SLC_SLC0_RX_LOOP_TEST_M  (BIT(5))
#define SLC_SLC0_RX_LOOP_TEST_V  0x1
#define SLC_SLC0_RX_LOOP_TEST_S  5
/* SLC_SLC0_TX_LOOP_TEST : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_TX_LOOP_TEST  (BIT(4))
#define SLC_SLC0_TX_LOOP_TEST_M  (BIT(4))
#define SLC_SLC0_TX_LOOP_TEST_V  0x1
#define SLC_SLC0_TX_LOOP_TEST_S  4
/* SLC_AHBM_RST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_AHBM_RST  (BIT(3))
#define SLC_AHBM_RST_M  (BIT(3))
#define SLC_AHBM_RST_V  0x1
#define SLC_AHBM_RST_S  3
/* SLC_AHBM_FIFO_RST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_AHBM_FIFO_RST  (BIT(2))
#define SLC_AHBM_FIFO_RST_M  (BIT(2))
#define SLC_AHBM_FIFO_RST_V  0x1
#define SLC_AHBM_FIFO_RST_S  2
/* SLC_SLC0_RX_RST : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_RST  (BIT(1))
#define SLC_SLC0_RX_RST_M  (BIT(1))
#define SLC_SLC0_RX_RST_V  0x1
#define SLC_SLC0_RX_RST_S  1
/* SLC_SLC0_TX_RST : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC0_TX_RST  (BIT(0))
#define SLC_SLC0_TX_RST_M  (BIT(0))
#define SLC_SLC0_TX_RST_V  0x1
#define SLC_SLC0_TX_RST_S  0

#define SLC_0INT_RAW_REG          (DR_REG_SLC_BASE + 0x4)
/* SLC_SLC0_RX_QUICK_EOF_INT_RAW : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_QUICK_EOF_INT_RAW  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_RAW_M  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_RAW_V  0x1
#define SLC_SLC0_RX_QUICK_EOF_INT_RAW_S  26
/* SLC_CMD_DTC_INT_RAW : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define SLC_CMD_DTC_INT_RAW  (BIT(25))
#define SLC_CMD_DTC_INT_RAW_M  (BIT(25))
#define SLC_CMD_DTC_INT_RAW_V  0x1
#define SLC_CMD_DTC_INT_RAW_S  25
/* SLC_SLC0_TX_ERR_EOF_INT_RAW : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_ERR_EOF_INT_RAW  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_RAW_M  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_RAW_V  0x1
#define SLC_SLC0_TX_ERR_EOF_INT_RAW_S  24
/* SLC_SLC0_WR_RETRY_DONE_INT_RAW : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_WR_RETRY_DONE_INT_RAW  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_RAW_M  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_RAW_V  0x1
#define SLC_SLC0_WR_RETRY_DONE_INT_RAW_S  23
/* SLC_SLC0_HOST_RD_ACK_INT_RAW : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_HOST_RD_ACK_INT_RAW  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_RAW_M  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_RAW_V  0x1
#define SLC_SLC0_HOST_RD_ACK_INT_RAW_S  22
/* SLC_SLC0_TX_DSCR_EMPTY_INT_RAW : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_EMPTY_INT_RAW  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_RAW_M  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_RAW_V  0x1
#define SLC_SLC0_TX_DSCR_EMPTY_INT_RAW_S  21
/* SLC_SLC0_RX_DSCR_ERR_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DSCR_ERR_INT_RAW  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_RAW_M  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_RAW_V  0x1
#define SLC_SLC0_RX_DSCR_ERR_INT_RAW_S  20
/* SLC_SLC0_TX_DSCR_ERR_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_ERR_INT_RAW  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_RAW_M  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_RAW_V  0x1
#define SLC_SLC0_TX_DSCR_ERR_INT_RAW_S  19
/* SLC_SLC0_TOHOST_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOHOST_INT_RAW  (BIT(18))
#define SLC_SLC0_TOHOST_INT_RAW_M  (BIT(18))
#define SLC_SLC0_TOHOST_INT_RAW_V  0x1
#define SLC_SLC0_TOHOST_INT_RAW_S  18
/* SLC_SLC0_RX_EOF_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_EOF_INT_RAW  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_RAW_M  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_RAW_V  0x1
#define SLC_SLC0_RX_EOF_INT_RAW_S  17
/* SLC_SLC0_RX_DONE_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DONE_INT_RAW  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_RAW_M  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_RAW_V  0x1
#define SLC_SLC0_RX_DONE_INT_RAW_S  16
/* SLC_SLC0_TX_SUC_EOF_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_SUC_EOF_INT_RAW  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_RAW_M  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_RAW_V  0x1
#define SLC_SLC0_TX_SUC_EOF_INT_RAW_S  15
/* SLC_SLC0_TX_DONE_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DONE_INT_RAW  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_RAW_M  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_RAW_V  0x1
#define SLC_SLC0_TX_DONE_INT_RAW_S  14
/* SLC_SLC0_TOKEN1_1TO0_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_1TO0_INT_RAW  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_RAW_M  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_RAW_V  0x1
#define SLC_SLC0_TOKEN1_1TO0_INT_RAW_S  13
/* SLC_SLC0_TOKEN0_1TO0_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_1TO0_INT_RAW  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_RAW_M  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_RAW_V  0x1
#define SLC_SLC0_TOKEN0_1TO0_INT_RAW_S  12
/* SLC_SLC0_TX_OVF_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_OVF_INT_RAW  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_RAW_M  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_RAW_V  0x1
#define SLC_SLC0_TX_OVF_INT_RAW_S  11
/* SLC_SLC0_RX_UDF_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_UDF_INT_RAW  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_RAW_M  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_RAW_V  0x1
#define SLC_SLC0_RX_UDF_INT_RAW_S  10
/* SLC_SLC0_TX_START_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_START_INT_RAW  (BIT(9))
#define SLC_SLC0_TX_START_INT_RAW_M  (BIT(9))
#define SLC_SLC0_TX_START_INT_RAW_V  0x1
#define SLC_SLC0_TX_START_INT_RAW_S  9
/* SLC_SLC0_RX_START_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_START_INT_RAW  (BIT(8))
#define SLC_SLC0_RX_START_INT_RAW_M  (BIT(8))
#define SLC_SLC0_RX_START_INT_RAW_V  0x1
#define SLC_SLC0_RX_START_INT_RAW_S  8
/* SLC_FRHOST_BIT7_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT7_INT_RAW  (BIT(7))
#define SLC_FRHOST_BIT7_INT_RAW_M  (BIT(7))
#define SLC_FRHOST_BIT7_INT_RAW_V  0x1
#define SLC_FRHOST_BIT7_INT_RAW_S  7
/* SLC_FRHOST_BIT6_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT6_INT_RAW  (BIT(6))
#define SLC_FRHOST_BIT6_INT_RAW_M  (BIT(6))
#define SLC_FRHOST_BIT6_INT_RAW_V  0x1
#define SLC_FRHOST_BIT6_INT_RAW_S  6
/* SLC_FRHOST_BIT5_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT5_INT_RAW  (BIT(5))
#define SLC_FRHOST_BIT5_INT_RAW_M  (BIT(5))
#define SLC_FRHOST_BIT5_INT_RAW_V  0x1
#define SLC_FRHOST_BIT5_INT_RAW_S  5
/* SLC_FRHOST_BIT4_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT4_INT_RAW  (BIT(4))
#define SLC_FRHOST_BIT4_INT_RAW_M  (BIT(4))
#define SLC_FRHOST_BIT4_INT_RAW_V  0x1
#define SLC_FRHOST_BIT4_INT_RAW_S  4
/* SLC_FRHOST_BIT3_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT3_INT_RAW  (BIT(3))
#define SLC_FRHOST_BIT3_INT_RAW_M  (BIT(3))
#define SLC_FRHOST_BIT3_INT_RAW_V  0x1
#define SLC_FRHOST_BIT3_INT_RAW_S  3
/* SLC_FRHOST_BIT2_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT2_INT_RAW  (BIT(2))
#define SLC_FRHOST_BIT2_INT_RAW_M  (BIT(2))
#define SLC_FRHOST_BIT2_INT_RAW_V  0x1
#define SLC_FRHOST_BIT2_INT_RAW_S  2
/* SLC_FRHOST_BIT1_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT1_INT_RAW  (BIT(1))
#define SLC_FRHOST_BIT1_INT_RAW_M  (BIT(1))
#define SLC_FRHOST_BIT1_INT_RAW_V  0x1
#define SLC_FRHOST_BIT1_INT_RAW_S  1
/* SLC_FRHOST_BIT0_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT0_INT_RAW  (BIT(0))
#define SLC_FRHOST_BIT0_INT_RAW_M  (BIT(0))
#define SLC_FRHOST_BIT0_INT_RAW_V  0x1
#define SLC_FRHOST_BIT0_INT_RAW_S  0

#define SLC_0INT_ST_REG          (DR_REG_SLC_BASE + 0x8)
/* SLC_SLC0_RX_QUICK_EOF_INT_ST : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_QUICK_EOF_INT_ST  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ST_M  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ST_V  0x1
#define SLC_SLC0_RX_QUICK_EOF_INT_ST_S  26
/* SLC_CMD_DTC_INT_ST : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define SLC_CMD_DTC_INT_ST  (BIT(25))
#define SLC_CMD_DTC_INT_ST_M  (BIT(25))
#define SLC_CMD_DTC_INT_ST_V  0x1
#define SLC_CMD_DTC_INT_ST_S  25
/* SLC_SLC0_TX_ERR_EOF_INT_ST : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_ERR_EOF_INT_ST  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ST_M  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ST_V  0x1
#define SLC_SLC0_TX_ERR_EOF_INT_ST_S  24
/* SLC_SLC0_WR_RETRY_DONE_INT_ST : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_WR_RETRY_DONE_INT_ST  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ST_M  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ST_V  0x1
#define SLC_SLC0_WR_RETRY_DONE_INT_ST_S  23
/* SLC_SLC0_HOST_RD_ACK_INT_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_HOST_RD_ACK_INT_ST  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ST_M  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ST_V  0x1
#define SLC_SLC0_HOST_RD_ACK_INT_ST_S  22
/* SLC_SLC0_TX_DSCR_EMPTY_INT_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST_M  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST_V  0x1
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST_S  21
/* SLC_SLC0_RX_DSCR_ERR_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DSCR_ERR_INT_ST  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ST_M  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ST_V  0x1
#define SLC_SLC0_RX_DSCR_ERR_INT_ST_S  20
/* SLC_SLC0_TX_DSCR_ERR_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_ERR_INT_ST  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ST_M  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ST_V  0x1
#define SLC_SLC0_TX_DSCR_ERR_INT_ST_S  19
/* SLC_SLC0_TOHOST_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOHOST_INT_ST  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ST_M  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ST_V  0x1
#define SLC_SLC0_TOHOST_INT_ST_S  18
/* SLC_SLC0_RX_EOF_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_EOF_INT_ST  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ST_M  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ST_V  0x1
#define SLC_SLC0_RX_EOF_INT_ST_S  17
/* SLC_SLC0_RX_DONE_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DONE_INT_ST  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ST_M  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ST_V  0x1
#define SLC_SLC0_RX_DONE_INT_ST_S  16
/* SLC_SLC0_TX_SUC_EOF_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_SUC_EOF_INT_ST  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ST_M  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ST_V  0x1
#define SLC_SLC0_TX_SUC_EOF_INT_ST_S  15
/* SLC_SLC0_TX_DONE_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DONE_INT_ST  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ST_M  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ST_V  0x1
#define SLC_SLC0_TX_DONE_INT_ST_S  14
/* SLC_SLC0_TOKEN1_1TO0_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_1TO0_INT_ST  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ST_M  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ST_V  0x1
#define SLC_SLC0_TOKEN1_1TO0_INT_ST_S  13
/* SLC_SLC0_TOKEN0_1TO0_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_1TO0_INT_ST  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ST_M  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ST_V  0x1
#define SLC_SLC0_TOKEN0_1TO0_INT_ST_S  12
/* SLC_SLC0_TX_OVF_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_OVF_INT_ST  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ST_M  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ST_V  0x1
#define SLC_SLC0_TX_OVF_INT_ST_S  11
/* SLC_SLC0_RX_UDF_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_UDF_INT_ST  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ST_M  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ST_V  0x1
#define SLC_SLC0_RX_UDF_INT_ST_S  10
/* SLC_SLC0_TX_START_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_START_INT_ST  (BIT(9))
#define SLC_SLC0_TX_START_INT_ST_M  (BIT(9))
#define SLC_SLC0_TX_START_INT_ST_V  0x1
#define SLC_SLC0_TX_START_INT_ST_S  9
/* SLC_SLC0_RX_START_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_START_INT_ST  (BIT(8))
#define SLC_SLC0_RX_START_INT_ST_M  (BIT(8))
#define SLC_SLC0_RX_START_INT_ST_V  0x1
#define SLC_SLC0_RX_START_INT_ST_S  8
/* SLC_FRHOST_BIT7_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT7_INT_ST  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ST_M  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ST_V  0x1
#define SLC_FRHOST_BIT7_INT_ST_S  7
/* SLC_FRHOST_BIT6_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT6_INT_ST  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ST_M  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ST_V  0x1
#define SLC_FRHOST_BIT6_INT_ST_S  6
/* SLC_FRHOST_BIT5_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT5_INT_ST  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ST_M  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ST_V  0x1
#define SLC_FRHOST_BIT5_INT_ST_S  5
/* SLC_FRHOST_BIT4_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT4_INT_ST  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ST_M  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ST_V  0x1
#define SLC_FRHOST_BIT4_INT_ST_S  4
/* SLC_FRHOST_BIT3_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT3_INT_ST  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ST_M  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ST_V  0x1
#define SLC_FRHOST_BIT3_INT_ST_S  3
/* SLC_FRHOST_BIT2_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT2_INT_ST  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ST_M  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ST_V  0x1
#define SLC_FRHOST_BIT2_INT_ST_S  2
/* SLC_FRHOST_BIT1_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT1_INT_ST  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ST_M  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ST_V  0x1
#define SLC_FRHOST_BIT1_INT_ST_S  1
/* SLC_FRHOST_BIT0_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT0_INT_ST  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ST_M  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ST_V  0x1
#define SLC_FRHOST_BIT0_INT_ST_S  0

#define SLC_0INT_ENA_REG          (DR_REG_SLC_BASE + 0xC)
/* SLC_SLC0_RX_QUICK_EOF_INT_ENA : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA_M  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA_V  0x1
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA_S  26
/* SLC_CMD_DTC_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define SLC_CMD_DTC_INT_ENA  (BIT(25))
#define SLC_CMD_DTC_INT_ENA_M  (BIT(25))
#define SLC_CMD_DTC_INT_ENA_V  0x1
#define SLC_CMD_DTC_INT_ENA_S  25
/* SLC_SLC0_TX_ERR_EOF_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_ERR_EOF_INT_ENA  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ENA_M  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ENA_V  0x1
#define SLC_SLC0_TX_ERR_EOF_INT_ENA_S  24
/* SLC_SLC0_WR_RETRY_DONE_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA_M  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA_V  0x1
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA_S  23
/* SLC_SLC0_HOST_RD_ACK_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_HOST_RD_ACK_INT_ENA  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ENA_M  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ENA_V  0x1
#define SLC_SLC0_HOST_RD_ACK_INT_ENA_S  22
/* SLC_SLC0_TX_DSCR_EMPTY_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA_M  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA_V  0x1
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA_S  21
/* SLC_SLC0_RX_DSCR_ERR_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA_M  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA_V  0x1
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA_S  20
/* SLC_SLC0_TX_DSCR_ERR_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA_M  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA_V  0x1
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA_S  19
/* SLC_SLC0_TOHOST_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOHOST_INT_ENA  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ENA_M  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ENA_V  0x1
#define SLC_SLC0_TOHOST_INT_ENA_S  18
/* SLC_SLC0_RX_EOF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_EOF_INT_ENA  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ENA_M  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ENA_V  0x1
#define SLC_SLC0_RX_EOF_INT_ENA_S  17
/* SLC_SLC0_RX_DONE_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DONE_INT_ENA  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ENA_M  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ENA_V  0x1
#define SLC_SLC0_RX_DONE_INT_ENA_S  16
/* SLC_SLC0_TX_SUC_EOF_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_SUC_EOF_INT_ENA  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ENA_M  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ENA_V  0x1
#define SLC_SLC0_TX_SUC_EOF_INT_ENA_S  15
/* SLC_SLC0_TX_DONE_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DONE_INT_ENA  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ENA_M  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ENA_V  0x1
#define SLC_SLC0_TX_DONE_INT_ENA_S  14
/* SLC_SLC0_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA_M  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA_V  0x1
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA_S  13
/* SLC_SLC0_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA_M  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA_V  0x1
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA_S  12
/* SLC_SLC0_TX_OVF_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_OVF_INT_ENA  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ENA_M  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ENA_V  0x1
#define SLC_SLC0_TX_OVF_INT_ENA_S  11
/* SLC_SLC0_RX_UDF_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_UDF_INT_ENA  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ENA_M  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ENA_V  0x1
#define SLC_SLC0_RX_UDF_INT_ENA_S  10
/* SLC_SLC0_TX_START_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_START_INT_ENA  (BIT(9))
#define SLC_SLC0_TX_START_INT_ENA_M  (BIT(9))
#define SLC_SLC0_TX_START_INT_ENA_V  0x1
#define SLC_SLC0_TX_START_INT_ENA_S  9
/* SLC_SLC0_RX_START_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_START_INT_ENA  (BIT(8))
#define SLC_SLC0_RX_START_INT_ENA_M  (BIT(8))
#define SLC_SLC0_RX_START_INT_ENA_V  0x1
#define SLC_SLC0_RX_START_INT_ENA_S  8
/* SLC_FRHOST_BIT7_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT7_INT_ENA  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ENA_M  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ENA_V  0x1
#define SLC_FRHOST_BIT7_INT_ENA_S  7
/* SLC_FRHOST_BIT6_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT6_INT_ENA  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ENA_M  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ENA_V  0x1
#define SLC_FRHOST_BIT6_INT_ENA_S  6
/* SLC_FRHOST_BIT5_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT5_INT_ENA  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ENA_M  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ENA_V  0x1
#define SLC_FRHOST_BIT5_INT_ENA_S  5
/* SLC_FRHOST_BIT4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT4_INT_ENA  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ENA_M  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ENA_V  0x1
#define SLC_FRHOST_BIT4_INT_ENA_S  4
/* SLC_FRHOST_BIT3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT3_INT_ENA  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ENA_M  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ENA_V  0x1
#define SLC_FRHOST_BIT3_INT_ENA_S  3
/* SLC_FRHOST_BIT2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT2_INT_ENA  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ENA_M  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ENA_V  0x1
#define SLC_FRHOST_BIT2_INT_ENA_S  2
/* SLC_FRHOST_BIT1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT1_INT_ENA  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ENA_M  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ENA_V  0x1
#define SLC_FRHOST_BIT1_INT_ENA_S  1
/* SLC_FRHOST_BIT0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT0_INT_ENA  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ENA_M  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ENA_V  0x1
#define SLC_FRHOST_BIT0_INT_ENA_S  0

#define SLC_0INT_CLR_REG          (DR_REG_SLC_BASE + 0x10)
/* SLC_SLC0_RX_QUICK_EOF_INT_CLR : WO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_QUICK_EOF_INT_CLR  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_CLR_M  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_CLR_V  0x1
#define SLC_SLC0_RX_QUICK_EOF_INT_CLR_S  26
/* SLC_CMD_DTC_INT_CLR : WO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define SLC_CMD_DTC_INT_CLR  (BIT(25))
#define SLC_CMD_DTC_INT_CLR_M  (BIT(25))
#define SLC_CMD_DTC_INT_CLR_V  0x1
#define SLC_CMD_DTC_INT_CLR_S  25
/* SLC_SLC0_TX_ERR_EOF_INT_CLR : WO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_ERR_EOF_INT_CLR  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_CLR_M  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_CLR_V  0x1
#define SLC_SLC0_TX_ERR_EOF_INT_CLR_S  24
/* SLC_SLC0_WR_RETRY_DONE_INT_CLR : WO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_WR_RETRY_DONE_INT_CLR  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_CLR_M  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_CLR_V  0x1
#define SLC_SLC0_WR_RETRY_DONE_INT_CLR_S  23
/* SLC_SLC0_HOST_RD_ACK_INT_CLR : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_HOST_RD_ACK_INT_CLR  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_CLR_M  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_CLR_V  0x1
#define SLC_SLC0_HOST_RD_ACK_INT_CLR_S  22
/* SLC_SLC0_TX_DSCR_EMPTY_INT_CLR : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_EMPTY_INT_CLR  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_CLR_M  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_CLR_V  0x1
#define SLC_SLC0_TX_DSCR_EMPTY_INT_CLR_S  21
/* SLC_SLC0_RX_DSCR_ERR_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DSCR_ERR_INT_CLR  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_CLR_M  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_CLR_V  0x1
#define SLC_SLC0_RX_DSCR_ERR_INT_CLR_S  20
/* SLC_SLC0_TX_DSCR_ERR_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_ERR_INT_CLR  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_CLR_M  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_CLR_V  0x1
#define SLC_SLC0_TX_DSCR_ERR_INT_CLR_S  19
/* SLC_SLC0_TOHOST_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOHOST_INT_CLR  (BIT(18))
#define SLC_SLC0_TOHOST_INT_CLR_M  (BIT(18))
#define SLC_SLC0_TOHOST_INT_CLR_V  0x1
#define SLC_SLC0_TOHOST_INT_CLR_S  18
/* SLC_SLC0_RX_EOF_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_EOF_INT_CLR  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_CLR_M  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_CLR_V  0x1
#define SLC_SLC0_RX_EOF_INT_CLR_S  17
/* SLC_SLC0_RX_DONE_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DONE_INT_CLR  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_CLR_M  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_CLR_V  0x1
#define SLC_SLC0_RX_DONE_INT_CLR_S  16
/* SLC_SLC0_TX_SUC_EOF_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_SUC_EOF_INT_CLR  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_CLR_M  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_CLR_V  0x1
#define SLC_SLC0_TX_SUC_EOF_INT_CLR_S  15
/* SLC_SLC0_TX_DONE_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DONE_INT_CLR  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_CLR_M  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_CLR_V  0x1
#define SLC_SLC0_TX_DONE_INT_CLR_S  14
/* SLC_SLC0_TOKEN1_1TO0_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_1TO0_INT_CLR  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_CLR_M  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_CLR_V  0x1
#define SLC_SLC0_TOKEN1_1TO0_INT_CLR_S  13
/* SLC_SLC0_TOKEN0_1TO0_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_1TO0_INT_CLR  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_CLR_M  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_CLR_V  0x1
#define SLC_SLC0_TOKEN0_1TO0_INT_CLR_S  12
/* SLC_SLC0_TX_OVF_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_OVF_INT_CLR  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_CLR_M  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_CLR_V  0x1
#define SLC_SLC0_TX_OVF_INT_CLR_S  11
/* SLC_SLC0_RX_UDF_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_UDF_INT_CLR  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_CLR_M  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_CLR_V  0x1
#define SLC_SLC0_RX_UDF_INT_CLR_S  10
/* SLC_SLC0_TX_START_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_START_INT_CLR  (BIT(9))
#define SLC_SLC0_TX_START_INT_CLR_M  (BIT(9))
#define SLC_SLC0_TX_START_INT_CLR_V  0x1
#define SLC_SLC0_TX_START_INT_CLR_S  9
/* SLC_SLC0_RX_START_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_START_INT_CLR  (BIT(8))
#define SLC_SLC0_RX_START_INT_CLR_M  (BIT(8))
#define SLC_SLC0_RX_START_INT_CLR_V  0x1
#define SLC_SLC0_RX_START_INT_CLR_S  8
/* SLC_FRHOST_BIT7_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT7_INT_CLR  (BIT(7))
#define SLC_FRHOST_BIT7_INT_CLR_M  (BIT(7))
#define SLC_FRHOST_BIT7_INT_CLR_V  0x1
#define SLC_FRHOST_BIT7_INT_CLR_S  7
/* SLC_FRHOST_BIT6_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT6_INT_CLR  (BIT(6))
#define SLC_FRHOST_BIT6_INT_CLR_M  (BIT(6))
#define SLC_FRHOST_BIT6_INT_CLR_V  0x1
#define SLC_FRHOST_BIT6_INT_CLR_S  6
/* SLC_FRHOST_BIT5_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT5_INT_CLR  (BIT(5))
#define SLC_FRHOST_BIT5_INT_CLR_M  (BIT(5))
#define SLC_FRHOST_BIT5_INT_CLR_V  0x1
#define SLC_FRHOST_BIT5_INT_CLR_S  5
/* SLC_FRHOST_BIT4_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT4_INT_CLR  (BIT(4))
#define SLC_FRHOST_BIT4_INT_CLR_M  (BIT(4))
#define SLC_FRHOST_BIT4_INT_CLR_V  0x1
#define SLC_FRHOST_BIT4_INT_CLR_S  4
/* SLC_FRHOST_BIT3_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT3_INT_CLR  (BIT(3))
#define SLC_FRHOST_BIT3_INT_CLR_M  (BIT(3))
#define SLC_FRHOST_BIT3_INT_CLR_V  0x1
#define SLC_FRHOST_BIT3_INT_CLR_S  3
/* SLC_FRHOST_BIT2_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT2_INT_CLR  (BIT(2))
#define SLC_FRHOST_BIT2_INT_CLR_M  (BIT(2))
#define SLC_FRHOST_BIT2_INT_CLR_V  0x1
#define SLC_FRHOST_BIT2_INT_CLR_S  2
/* SLC_FRHOST_BIT1_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT1_INT_CLR  (BIT(1))
#define SLC_FRHOST_BIT1_INT_CLR_M  (BIT(1))
#define SLC_FRHOST_BIT1_INT_CLR_V  0x1
#define SLC_FRHOST_BIT1_INT_CLR_S  1
/* SLC_FRHOST_BIT0_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT0_INT_CLR  (BIT(0))
#define SLC_FRHOST_BIT0_INT_CLR_M  (BIT(0))
#define SLC_FRHOST_BIT0_INT_CLR_V  0x1
#define SLC_FRHOST_BIT0_INT_CLR_S  0

#define SLC_1INT_RAW_REG          (DR_REG_SLC_BASE + 0x14)
/* SLC_SLC1_TX_ERR_EOF_INT_RAW : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_ERR_EOF_INT_RAW  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_RAW_M  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_RAW_V  0x1
#define SLC_SLC1_TX_ERR_EOF_INT_RAW_S  24
/* SLC_SLC1_WR_RETRY_DONE_INT_RAW : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_WR_RETRY_DONE_INT_RAW  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_RAW_M  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_RAW_V  0x1
#define SLC_SLC1_WR_RETRY_DONE_INT_RAW_S  23
/* SLC_SLC1_HOST_RD_ACK_INT_RAW : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_HOST_RD_ACK_INT_RAW  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_RAW_M  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_RAW_V  0x1
#define SLC_SLC1_HOST_RD_ACK_INT_RAW_S  22
/* SLC_SLC1_TX_DSCR_EMPTY_INT_RAW : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_EMPTY_INT_RAW  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_RAW_M  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_RAW_V  0x1
#define SLC_SLC1_TX_DSCR_EMPTY_INT_RAW_S  21
/* SLC_SLC1_RX_DSCR_ERR_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DSCR_ERR_INT_RAW  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_RAW_M  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_RAW_V  0x1
#define SLC_SLC1_RX_DSCR_ERR_INT_RAW_S  20
/* SLC_SLC1_TX_DSCR_ERR_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_ERR_INT_RAW  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_RAW_M  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_RAW_V  0x1
#define SLC_SLC1_TX_DSCR_ERR_INT_RAW_S  19
/* SLC_SLC1_TOHOST_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOHOST_INT_RAW  (BIT(18))
#define SLC_SLC1_TOHOST_INT_RAW_M  (BIT(18))
#define SLC_SLC1_TOHOST_INT_RAW_V  0x1
#define SLC_SLC1_TOHOST_INT_RAW_S  18
/* SLC_SLC1_RX_EOF_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_EOF_INT_RAW  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_RAW_M  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_RAW_V  0x1
#define SLC_SLC1_RX_EOF_INT_RAW_S  17
/* SLC_SLC1_RX_DONE_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DONE_INT_RAW  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_RAW_M  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_RAW_V  0x1
#define SLC_SLC1_RX_DONE_INT_RAW_S  16
/* SLC_SLC1_TX_SUC_EOF_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_SUC_EOF_INT_RAW  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_RAW_M  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_RAW_V  0x1
#define SLC_SLC1_TX_SUC_EOF_INT_RAW_S  15
/* SLC_SLC1_TX_DONE_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DONE_INT_RAW  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_RAW_M  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_RAW_V  0x1
#define SLC_SLC1_TX_DONE_INT_RAW_S  14
/* SLC_SLC1_TOKEN1_1TO0_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_1TO0_INT_RAW  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_RAW_M  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_RAW_V  0x1
#define SLC_SLC1_TOKEN1_1TO0_INT_RAW_S  13
/* SLC_SLC1_TOKEN0_1TO0_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_1TO0_INT_RAW  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_RAW_M  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_RAW_V  0x1
#define SLC_SLC1_TOKEN0_1TO0_INT_RAW_S  12
/* SLC_SLC1_TX_OVF_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_OVF_INT_RAW  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_RAW_M  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_RAW_V  0x1
#define SLC_SLC1_TX_OVF_INT_RAW_S  11
/* SLC_SLC1_RX_UDF_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_UDF_INT_RAW  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_RAW_M  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_RAW_V  0x1
#define SLC_SLC1_RX_UDF_INT_RAW_S  10
/* SLC_SLC1_TX_START_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_START_INT_RAW  (BIT(9))
#define SLC_SLC1_TX_START_INT_RAW_M  (BIT(9))
#define SLC_SLC1_TX_START_INT_RAW_V  0x1
#define SLC_SLC1_TX_START_INT_RAW_S  9
/* SLC_SLC1_RX_START_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_START_INT_RAW  (BIT(8))
#define SLC_SLC1_RX_START_INT_RAW_M  (BIT(8))
#define SLC_SLC1_RX_START_INT_RAW_V  0x1
#define SLC_SLC1_RX_START_INT_RAW_S  8
/* SLC_FRHOST_BIT15_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT15_INT_RAW  (BIT(7))
#define SLC_FRHOST_BIT15_INT_RAW_M  (BIT(7))
#define SLC_FRHOST_BIT15_INT_RAW_V  0x1
#define SLC_FRHOST_BIT15_INT_RAW_S  7
/* SLC_FRHOST_BIT14_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT14_INT_RAW  (BIT(6))
#define SLC_FRHOST_BIT14_INT_RAW_M  (BIT(6))
#define SLC_FRHOST_BIT14_INT_RAW_V  0x1
#define SLC_FRHOST_BIT14_INT_RAW_S  6
/* SLC_FRHOST_BIT13_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT13_INT_RAW  (BIT(5))
#define SLC_FRHOST_BIT13_INT_RAW_M  (BIT(5))
#define SLC_FRHOST_BIT13_INT_RAW_V  0x1
#define SLC_FRHOST_BIT13_INT_RAW_S  5
/* SLC_FRHOST_BIT12_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT12_INT_RAW  (BIT(4))
#define SLC_FRHOST_BIT12_INT_RAW_M  (BIT(4))
#define SLC_FRHOST_BIT12_INT_RAW_V  0x1
#define SLC_FRHOST_BIT12_INT_RAW_S  4
/* SLC_FRHOST_BIT11_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT11_INT_RAW  (BIT(3))
#define SLC_FRHOST_BIT11_INT_RAW_M  (BIT(3))
#define SLC_FRHOST_BIT11_INT_RAW_V  0x1
#define SLC_FRHOST_BIT11_INT_RAW_S  3
/* SLC_FRHOST_BIT10_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT10_INT_RAW  (BIT(2))
#define SLC_FRHOST_BIT10_INT_RAW_M  (BIT(2))
#define SLC_FRHOST_BIT10_INT_RAW_V  0x1
#define SLC_FRHOST_BIT10_INT_RAW_S  2
/* SLC_FRHOST_BIT9_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT9_INT_RAW  (BIT(1))
#define SLC_FRHOST_BIT9_INT_RAW_M  (BIT(1))
#define SLC_FRHOST_BIT9_INT_RAW_V  0x1
#define SLC_FRHOST_BIT9_INT_RAW_S  1
/* SLC_FRHOST_BIT8_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT8_INT_RAW  (BIT(0))
#define SLC_FRHOST_BIT8_INT_RAW_M  (BIT(0))
#define SLC_FRHOST_BIT8_INT_RAW_V  0x1
#define SLC_FRHOST_BIT8_INT_RAW_S  0

#define SLC_1INT_ST_REG          (DR_REG_SLC_BASE + 0x18)
/* SLC_SLC1_TX_ERR_EOF_INT_ST : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_ERR_EOF_INT_ST  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ST_M  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ST_V  0x1
#define SLC_SLC1_TX_ERR_EOF_INT_ST_S  24
/* SLC_SLC1_WR_RETRY_DONE_INT_ST : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_WR_RETRY_DONE_INT_ST  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ST_M  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ST_V  0x1
#define SLC_SLC1_WR_RETRY_DONE_INT_ST_S  23
/* SLC_SLC1_HOST_RD_ACK_INT_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_HOST_RD_ACK_INT_ST  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ST_M  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ST_V  0x1
#define SLC_SLC1_HOST_RD_ACK_INT_ST_S  22
/* SLC_SLC1_TX_DSCR_EMPTY_INT_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST_M  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST_V  0x1
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST_S  21
/* SLC_SLC1_RX_DSCR_ERR_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DSCR_ERR_INT_ST  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ST_M  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ST_V  0x1
#define SLC_SLC1_RX_DSCR_ERR_INT_ST_S  20
/* SLC_SLC1_TX_DSCR_ERR_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_ERR_INT_ST  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ST_M  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ST_V  0x1
#define SLC_SLC1_TX_DSCR_ERR_INT_ST_S  19
/* SLC_SLC1_TOHOST_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOHOST_INT_ST  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ST_M  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ST_V  0x1
#define SLC_SLC1_TOHOST_INT_ST_S  18
/* SLC_SLC1_RX_EOF_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_EOF_INT_ST  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ST_M  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ST_V  0x1
#define SLC_SLC1_RX_EOF_INT_ST_S  17
/* SLC_SLC1_RX_DONE_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DONE_INT_ST  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ST_M  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ST_V  0x1
#define SLC_SLC1_RX_DONE_INT_ST_S  16
/* SLC_SLC1_TX_SUC_EOF_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_SUC_EOF_INT_ST  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ST_M  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ST_V  0x1
#define SLC_SLC1_TX_SUC_EOF_INT_ST_S  15
/* SLC_SLC1_TX_DONE_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DONE_INT_ST  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ST_M  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ST_V  0x1
#define SLC_SLC1_TX_DONE_INT_ST_S  14
/* SLC_SLC1_TOKEN1_1TO0_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_1TO0_INT_ST  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ST_M  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ST_V  0x1
#define SLC_SLC1_TOKEN1_1TO0_INT_ST_S  13
/* SLC_SLC1_TOKEN0_1TO0_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_1TO0_INT_ST  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ST_M  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ST_V  0x1
#define SLC_SLC1_TOKEN0_1TO0_INT_ST_S  12
/* SLC_SLC1_TX_OVF_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_OVF_INT_ST  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ST_M  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ST_V  0x1
#define SLC_SLC1_TX_OVF_INT_ST_S  11
/* SLC_SLC1_RX_UDF_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_UDF_INT_ST  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ST_M  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ST_V  0x1
#define SLC_SLC1_RX_UDF_INT_ST_S  10
/* SLC_SLC1_TX_START_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_START_INT_ST  (BIT(9))
#define SLC_SLC1_TX_START_INT_ST_M  (BIT(9))
#define SLC_SLC1_TX_START_INT_ST_V  0x1
#define SLC_SLC1_TX_START_INT_ST_S  9
/* SLC_SLC1_RX_START_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_START_INT_ST  (BIT(8))
#define SLC_SLC1_RX_START_INT_ST_M  (BIT(8))
#define SLC_SLC1_RX_START_INT_ST_V  0x1
#define SLC_SLC1_RX_START_INT_ST_S  8
/* SLC_FRHOST_BIT15_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT15_INT_ST  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ST_M  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ST_V  0x1
#define SLC_FRHOST_BIT15_INT_ST_S  7
/* SLC_FRHOST_BIT14_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT14_INT_ST  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ST_M  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ST_V  0x1
#define SLC_FRHOST_BIT14_INT_ST_S  6
/* SLC_FRHOST_BIT13_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT13_INT_ST  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ST_M  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ST_V  0x1
#define SLC_FRHOST_BIT13_INT_ST_S  5
/* SLC_FRHOST_BIT12_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT12_INT_ST  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ST_M  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ST_V  0x1
#define SLC_FRHOST_BIT12_INT_ST_S  4
/* SLC_FRHOST_BIT11_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT11_INT_ST  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ST_M  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ST_V  0x1
#define SLC_FRHOST_BIT11_INT_ST_S  3
/* SLC_FRHOST_BIT10_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT10_INT_ST  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ST_M  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ST_V  0x1
#define SLC_FRHOST_BIT10_INT_ST_S  2
/* SLC_FRHOST_BIT9_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT9_INT_ST  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ST_M  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ST_V  0x1
#define SLC_FRHOST_BIT9_INT_ST_S  1
/* SLC_FRHOST_BIT8_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT8_INT_ST  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ST_M  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ST_V  0x1
#define SLC_FRHOST_BIT8_INT_ST_S  0

#define SLC_1INT_ENA_REG          (DR_REG_SLC_BASE + 0x1C)
/* SLC_SLC1_TX_ERR_EOF_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_ERR_EOF_INT_ENA  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ENA_M  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ENA_V  0x1
#define SLC_SLC1_TX_ERR_EOF_INT_ENA_S  24
/* SLC_SLC1_WR_RETRY_DONE_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA_M  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA_V  0x1
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA_S  23
/* SLC_SLC1_HOST_RD_ACK_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_HOST_RD_ACK_INT_ENA  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ENA_M  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ENA_V  0x1
#define SLC_SLC1_HOST_RD_ACK_INT_ENA_S  22
/* SLC_SLC1_TX_DSCR_EMPTY_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA_M  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA_V  0x1
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA_S  21
/* SLC_SLC1_RX_DSCR_ERR_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA_M  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA_V  0x1
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA_S  20
/* SLC_SLC1_TX_DSCR_ERR_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA_M  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA_V  0x1
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA_S  19
/* SLC_SLC1_TOHOST_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOHOST_INT_ENA  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ENA_M  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ENA_V  0x1
#define SLC_SLC1_TOHOST_INT_ENA_S  18
/* SLC_SLC1_RX_EOF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_EOF_INT_ENA  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ENA_M  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ENA_V  0x1
#define SLC_SLC1_RX_EOF_INT_ENA_S  17
/* SLC_SLC1_RX_DONE_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DONE_INT_ENA  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ENA_M  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ENA_V  0x1
#define SLC_SLC1_RX_DONE_INT_ENA_S  16
/* SLC_SLC1_TX_SUC_EOF_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_SUC_EOF_INT_ENA  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ENA_M  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ENA_V  0x1
#define SLC_SLC1_TX_SUC_EOF_INT_ENA_S  15
/* SLC_SLC1_TX_DONE_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DONE_INT_ENA  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ENA_M  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ENA_V  0x1
#define SLC_SLC1_TX_DONE_INT_ENA_S  14
/* SLC_SLC1_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA_M  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA_V  0x1
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA_S  13
/* SLC_SLC1_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA_M  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA_V  0x1
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA_S  12
/* SLC_SLC1_TX_OVF_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_OVF_INT_ENA  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ENA_M  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ENA_V  0x1
#define SLC_SLC1_TX_OVF_INT_ENA_S  11
/* SLC_SLC1_RX_UDF_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_UDF_INT_ENA  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ENA_M  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ENA_V  0x1
#define SLC_SLC1_RX_UDF_INT_ENA_S  10
/* SLC_SLC1_TX_START_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_START_INT_ENA  (BIT(9))
#define SLC_SLC1_TX_START_INT_ENA_M  (BIT(9))
#define SLC_SLC1_TX_START_INT_ENA_V  0x1
#define SLC_SLC1_TX_START_INT_ENA_S  9
/* SLC_SLC1_RX_START_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_START_INT_ENA  (BIT(8))
#define SLC_SLC1_RX_START_INT_ENA_M  (BIT(8))
#define SLC_SLC1_RX_START_INT_ENA_V  0x1
#define SLC_SLC1_RX_START_INT_ENA_S  8
/* SLC_FRHOST_BIT15_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT15_INT_ENA  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ENA_M  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ENA_V  0x1
#define SLC_FRHOST_BIT15_INT_ENA_S  7
/* SLC_FRHOST_BIT14_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT14_INT_ENA  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ENA_M  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ENA_V  0x1
#define SLC_FRHOST_BIT14_INT_ENA_S  6
/* SLC_FRHOST_BIT13_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT13_INT_ENA  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ENA_M  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ENA_V  0x1
#define SLC_FRHOST_BIT13_INT_ENA_S  5
/* SLC_FRHOST_BIT12_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT12_INT_ENA  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ENA_M  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ENA_V  0x1
#define SLC_FRHOST_BIT12_INT_ENA_S  4
/* SLC_FRHOST_BIT11_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT11_INT_ENA  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ENA_M  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ENA_V  0x1
#define SLC_FRHOST_BIT11_INT_ENA_S  3
/* SLC_FRHOST_BIT10_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT10_INT_ENA  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ENA_M  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ENA_V  0x1
#define SLC_FRHOST_BIT10_INT_ENA_S  2
/* SLC_FRHOST_BIT9_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT9_INT_ENA  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ENA_M  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ENA_V  0x1
#define SLC_FRHOST_BIT9_INT_ENA_S  1
/* SLC_FRHOST_BIT8_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT8_INT_ENA  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ENA_M  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ENA_V  0x1
#define SLC_FRHOST_BIT8_INT_ENA_S  0

#define SLC_1INT_CLR_REG          (DR_REG_SLC_BASE + 0x20)
/* SLC_SLC1_TX_ERR_EOF_INT_CLR : WO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_ERR_EOF_INT_CLR  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_CLR_M  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_CLR_V  0x1
#define SLC_SLC1_TX_ERR_EOF_INT_CLR_S  24
/* SLC_SLC1_WR_RETRY_DONE_INT_CLR : WO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_WR_RETRY_DONE_INT_CLR  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_CLR_M  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_CLR_V  0x1
#define SLC_SLC1_WR_RETRY_DONE_INT_CLR_S  23
/* SLC_SLC1_HOST_RD_ACK_INT_CLR : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_HOST_RD_ACK_INT_CLR  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_CLR_M  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_CLR_V  0x1
#define SLC_SLC1_HOST_RD_ACK_INT_CLR_S  22
/* SLC_SLC1_TX_DSCR_EMPTY_INT_CLR : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_EMPTY_INT_CLR  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_CLR_M  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_CLR_V  0x1
#define SLC_SLC1_TX_DSCR_EMPTY_INT_CLR_S  21
/* SLC_SLC1_RX_DSCR_ERR_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DSCR_ERR_INT_CLR  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_CLR_M  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_CLR_V  0x1
#define SLC_SLC1_RX_DSCR_ERR_INT_CLR_S  20
/* SLC_SLC1_TX_DSCR_ERR_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_ERR_INT_CLR  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_CLR_M  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_CLR_V  0x1
#define SLC_SLC1_TX_DSCR_ERR_INT_CLR_S  19
/* SLC_SLC1_TOHOST_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOHOST_INT_CLR  (BIT(18))
#define SLC_SLC1_TOHOST_INT_CLR_M  (BIT(18))
#define SLC_SLC1_TOHOST_INT_CLR_V  0x1
#define SLC_SLC1_TOHOST_INT_CLR_S  18
/* SLC_SLC1_RX_EOF_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_EOF_INT_CLR  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_CLR_M  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_CLR_V  0x1
#define SLC_SLC1_RX_EOF_INT_CLR_S  17
/* SLC_SLC1_RX_DONE_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DONE_INT_CLR  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_CLR_M  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_CLR_V  0x1
#define SLC_SLC1_RX_DONE_INT_CLR_S  16
/* SLC_SLC1_TX_SUC_EOF_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_SUC_EOF_INT_CLR  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_CLR_M  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_CLR_V  0x1
#define SLC_SLC1_TX_SUC_EOF_INT_CLR_S  15
/* SLC_SLC1_TX_DONE_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DONE_INT_CLR  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_CLR_M  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_CLR_V  0x1
#define SLC_SLC1_TX_DONE_INT_CLR_S  14
/* SLC_SLC1_TOKEN1_1TO0_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_1TO0_INT_CLR  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_CLR_M  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_CLR_V  0x1
#define SLC_SLC1_TOKEN1_1TO0_INT_CLR_S  13
/* SLC_SLC1_TOKEN0_1TO0_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_1TO0_INT_CLR  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_CLR_M  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_CLR_V  0x1
#define SLC_SLC1_TOKEN0_1TO0_INT_CLR_S  12
/* SLC_SLC1_TX_OVF_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_OVF_INT_CLR  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_CLR_M  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_CLR_V  0x1
#define SLC_SLC1_TX_OVF_INT_CLR_S  11
/* SLC_SLC1_RX_UDF_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_UDF_INT_CLR  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_CLR_M  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_CLR_V  0x1
#define SLC_SLC1_RX_UDF_INT_CLR_S  10
/* SLC_SLC1_TX_START_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_START_INT_CLR  (BIT(9))
#define SLC_SLC1_TX_START_INT_CLR_M  (BIT(9))
#define SLC_SLC1_TX_START_INT_CLR_V  0x1
#define SLC_SLC1_TX_START_INT_CLR_S  9
/* SLC_SLC1_RX_START_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_START_INT_CLR  (BIT(8))
#define SLC_SLC1_RX_START_INT_CLR_M  (BIT(8))
#define SLC_SLC1_RX_START_INT_CLR_V  0x1
#define SLC_SLC1_RX_START_INT_CLR_S  8
/* SLC_FRHOST_BIT15_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT15_INT_CLR  (BIT(7))
#define SLC_FRHOST_BIT15_INT_CLR_M  (BIT(7))
#define SLC_FRHOST_BIT15_INT_CLR_V  0x1
#define SLC_FRHOST_BIT15_INT_CLR_S  7
/* SLC_FRHOST_BIT14_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT14_INT_CLR  (BIT(6))
#define SLC_FRHOST_BIT14_INT_CLR_M  (BIT(6))
#define SLC_FRHOST_BIT14_INT_CLR_V  0x1
#define SLC_FRHOST_BIT14_INT_CLR_S  6
/* SLC_FRHOST_BIT13_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT13_INT_CLR  (BIT(5))
#define SLC_FRHOST_BIT13_INT_CLR_M  (BIT(5))
#define SLC_FRHOST_BIT13_INT_CLR_V  0x1
#define SLC_FRHOST_BIT13_INT_CLR_S  5
/* SLC_FRHOST_BIT12_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT12_INT_CLR  (BIT(4))
#define SLC_FRHOST_BIT12_INT_CLR_M  (BIT(4))
#define SLC_FRHOST_BIT12_INT_CLR_V  0x1
#define SLC_FRHOST_BIT12_INT_CLR_S  4
/* SLC_FRHOST_BIT11_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT11_INT_CLR  (BIT(3))
#define SLC_FRHOST_BIT11_INT_CLR_M  (BIT(3))
#define SLC_FRHOST_BIT11_INT_CLR_V  0x1
#define SLC_FRHOST_BIT11_INT_CLR_S  3
/* SLC_FRHOST_BIT10_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT10_INT_CLR  (BIT(2))
#define SLC_FRHOST_BIT10_INT_CLR_M  (BIT(2))
#define SLC_FRHOST_BIT10_INT_CLR_V  0x1
#define SLC_FRHOST_BIT10_INT_CLR_S  2
/* SLC_FRHOST_BIT9_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT9_INT_CLR  (BIT(1))
#define SLC_FRHOST_BIT9_INT_CLR_M  (BIT(1))
#define SLC_FRHOST_BIT9_INT_CLR_V  0x1
#define SLC_FRHOST_BIT9_INT_CLR_S  1
/* SLC_FRHOST_BIT8_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT8_INT_CLR  (BIT(0))
#define SLC_FRHOST_BIT8_INT_CLR_M  (BIT(0))
#define SLC_FRHOST_BIT8_INT_CLR_V  0x1
#define SLC_FRHOST_BIT8_INT_CLR_S  0

#define SLC_RX_STATUS_REG          (DR_REG_SLC_BASE + 0x24)
/* SLC_SLC1_RX_EMPTY : RO ;bitpos:[17] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RX_EMPTY  (BIT(17))
#define SLC_SLC1_RX_EMPTY_M  (BIT(17))
#define SLC_SLC1_RX_EMPTY_V  0x1
#define SLC_SLC1_RX_EMPTY_S  17
/* SLC_SLC1_RX_FULL : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_FULL  (BIT(16))
#define SLC_SLC1_RX_FULL_M  (BIT(16))
#define SLC_SLC1_RX_FULL_V  0x1
#define SLC_SLC1_RX_FULL_S  16
/* SLC_SLC0_RX_EMPTY : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_RX_EMPTY  (BIT(1))
#define SLC_SLC0_RX_EMPTY_M  (BIT(1))
#define SLC_SLC0_RX_EMPTY_V  0x1
#define SLC_SLC0_RX_EMPTY_S  1
/* SLC_SLC0_RX_FULL : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_FULL  (BIT(0))
#define SLC_SLC0_RX_FULL_M  (BIT(0))
#define SLC_SLC0_RX_FULL_V  0x1
#define SLC_SLC0_RX_FULL_S  0

#define SLC_0RXFIFO_PUSH_REG          (DR_REG_SLC_BASE + 0x28)
/* SLC_SLC0_RXFIFO_PUSH : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC0_RXFIFO_PUSH  (BIT(16))
#define SLC_SLC0_RXFIFO_PUSH_M  (BIT(16))
#define SLC_SLC0_RXFIFO_PUSH_V  0x1
#define SLC_SLC0_RXFIFO_PUSH_S  16
/* SLC_SLC0_RXFIFO_WDATA : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: */
#define SLC_SLC0_RXFIFO_WDATA  0x000001FF
#define SLC_SLC0_RXFIFO_WDATA_M  ((SLC_SLC0_RXFIFO_WDATA_V)<<(SLC_SLC0_RXFIFO_WDATA_S))
#define SLC_SLC0_RXFIFO_WDATA_V  0x1FF
#define SLC_SLC0_RXFIFO_WDATA_S  0

#define SLC_1RXFIFO_PUSH_REG          (DR_REG_SLC_BASE + 0x2C)
/* SLC_SLC1_RXFIFO_PUSH : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC1_RXFIFO_PUSH  (BIT(16))
#define SLC_SLC1_RXFIFO_PUSH_M  (BIT(16))
#define SLC_SLC1_RXFIFO_PUSH_V  0x1
#define SLC_SLC1_RXFIFO_PUSH_S  16
/* SLC_SLC1_RXFIFO_WDATA : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: */
#define SLC_SLC1_RXFIFO_WDATA  0x000001FF
#define SLC_SLC1_RXFIFO_WDATA_M  ((SLC_SLC1_RXFIFO_WDATA_V)<<(SLC_SLC1_RXFIFO_WDATA_S))
#define SLC_SLC1_RXFIFO_WDATA_V  0x1FF
#define SLC_SLC1_RXFIFO_WDATA_S  0

#define SLC_TX_STATUS_REG          (DR_REG_SLC_BASE + 0x30)
/* SLC_SLC1_TX_EMPTY : RO ;bitpos:[17] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_TX_EMPTY  (BIT(17))
#define SLC_SLC1_TX_EMPTY_M  (BIT(17))
#define SLC_SLC1_TX_EMPTY_V  0x1
#define SLC_SLC1_TX_EMPTY_S  17
/* SLC_SLC1_TX_FULL : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_FULL  (BIT(16))
#define SLC_SLC1_TX_FULL_M  (BIT(16))
#define SLC_SLC1_TX_FULL_V  0x1
#define SLC_SLC1_TX_FULL_S  16
/* SLC_SLC0_TX_EMPTY : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_TX_EMPTY  (BIT(1))
#define SLC_SLC0_TX_EMPTY_M  (BIT(1))
#define SLC_SLC0_TX_EMPTY_V  0x1
#define SLC_SLC0_TX_EMPTY_S  1
/* SLC_SLC0_TX_FULL : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_FULL  (BIT(0))
#define SLC_SLC0_TX_FULL_M  (BIT(0))
#define SLC_SLC0_TX_FULL_V  0x1
#define SLC_SLC0_TX_FULL_S  0

#define SLC_0TXFIFO_POP_REG          (DR_REG_SLC_BASE + 0x34)
/* SLC_SLC0_TXFIFO_POP : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC0_TXFIFO_POP  (BIT(16))
#define SLC_SLC0_TXFIFO_POP_M  (BIT(16))
#define SLC_SLC0_TXFIFO_POP_V  0x1
#define SLC_SLC0_TXFIFO_POP_S  16
/* SLC_SLC0_TXFIFO_RDATA : RO ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SLC_SLC0_TXFIFO_RDATA  0x000007FF
#define SLC_SLC0_TXFIFO_RDATA_M  ((SLC_SLC0_TXFIFO_RDATA_V)<<(SLC_SLC0_TXFIFO_RDATA_S))
#define SLC_SLC0_TXFIFO_RDATA_V  0x7FF
#define SLC_SLC0_TXFIFO_RDATA_S  0

#define SLC_1TXFIFO_POP_REG          (DR_REG_SLC_BASE + 0x38)
/* SLC_SLC1_TXFIFO_POP : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC1_TXFIFO_POP  (BIT(16))
#define SLC_SLC1_TXFIFO_POP_M  (BIT(16))
#define SLC_SLC1_TXFIFO_POP_V  0x1
#define SLC_SLC1_TXFIFO_POP_S  16
/* SLC_SLC1_TXFIFO_RDATA : RO ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SLC_SLC1_TXFIFO_RDATA  0x000007FF
#define SLC_SLC1_TXFIFO_RDATA_M  ((SLC_SLC1_TXFIFO_RDATA_V)<<(SLC_SLC1_TXFIFO_RDATA_S))
#define SLC_SLC1_TXFIFO_RDATA_V  0x7FF
#define SLC_SLC1_TXFIFO_RDATA_S  0

#define SLC_0RX_LINK_REG          (DR_REG_SLC_BASE + 0x3C)
/* SLC_SLC0_RXLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC0_RXLINK_PARK  (BIT(31))
#define SLC_SLC0_RXLINK_PARK_M  (BIT(31))
#define SLC_SLC0_RXLINK_PARK_V  0x1
#define SLC_SLC0_RXLINK_PARK_S  31
/* SLC_SLC0_RXLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RXLINK_RESTART  (BIT(30))
#define SLC_SLC0_RXLINK_RESTART_M  (BIT(30))
#define SLC_SLC0_RXLINK_RESTART_V  0x1
#define SLC_SLC0_RXLINK_RESTART_S  30
/* SLC_SLC0_RXLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RXLINK_START  (BIT(29))
#define SLC_SLC0_RXLINK_START_M  (BIT(29))
#define SLC_SLC0_RXLINK_START_V  0x1
#define SLC_SLC0_RXLINK_START_S  29
/* SLC_SLC0_RXLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RXLINK_STOP  (BIT(28))
#define SLC_SLC0_RXLINK_STOP_M  (BIT(28))
#define SLC_SLC0_RXLINK_STOP_V  0x1
#define SLC_SLC0_RXLINK_STOP_S  28
/* SLC_SLC0_RXLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define SLC_SLC0_RXLINK_ADDR  0x000FFFFF
#define SLC_SLC0_RXLINK_ADDR_M  ((SLC_SLC0_RXLINK_ADDR_V)<<(SLC_SLC0_RXLINK_ADDR_S))
#define SLC_SLC0_RXLINK_ADDR_V  0xFFFFF
#define SLC_SLC0_RXLINK_ADDR_S  0

#define SLC_0TX_LINK_REG          (DR_REG_SLC_BASE + 0x40)
/* SLC_SLC0_TXLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC0_TXLINK_PARK  (BIT(31))
#define SLC_SLC0_TXLINK_PARK_M  (BIT(31))
#define SLC_SLC0_TXLINK_PARK_V  0x1
#define SLC_SLC0_TXLINK_PARK_S  31
/* SLC_SLC0_TXLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TXLINK_RESTART  (BIT(30))
#define SLC_SLC0_TXLINK_RESTART_M  (BIT(30))
#define SLC_SLC0_TXLINK_RESTART_V  0x1
#define SLC_SLC0_TXLINK_RESTART_S  30
/* SLC_SLC0_TXLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TXLINK_START  (BIT(29))
#define SLC_SLC0_TXLINK_START_M  (BIT(29))
#define SLC_SLC0_TXLINK_START_V  0x1
#define SLC_SLC0_TXLINK_START_S  29
/* SLC_SLC0_TXLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TXLINK_STOP  (BIT(28))
#define SLC_SLC0_TXLINK_STOP_M  (BIT(28))
#define SLC_SLC0_TXLINK_STOP_V  0x1
#define SLC_SLC0_TXLINK_STOP_S  28
/* SLC_SLC0_TXLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define SLC_SLC0_TXLINK_ADDR  0x000FFFFF
#define SLC_SLC0_TXLINK_ADDR_M  ((SLC_SLC0_TXLINK_ADDR_V)<<(SLC_SLC0_TXLINK_ADDR_S))
#define SLC_SLC0_TXLINK_ADDR_V  0xFFFFF
#define SLC_SLC0_TXLINK_ADDR_S  0

#define SLC_1RX_LINK_REG          (DR_REG_SLC_BASE + 0x44)
/* SLC_SLC1_RXLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC1_RXLINK_PARK  (BIT(31))
#define SLC_SLC1_RXLINK_PARK_M  (BIT(31))
#define SLC_SLC1_RXLINK_PARK_V  0x1
#define SLC_SLC1_RXLINK_PARK_S  31
/* SLC_SLC1_RXLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RXLINK_RESTART  (BIT(30))
#define SLC_SLC1_RXLINK_RESTART_M  (BIT(30))
#define SLC_SLC1_RXLINK_RESTART_V  0x1
#define SLC_SLC1_RXLINK_RESTART_S  30
/* SLC_SLC1_RXLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RXLINK_START  (BIT(29))
#define SLC_SLC1_RXLINK_START_M  (BIT(29))
#define SLC_SLC1_RXLINK_START_V  0x1
#define SLC_SLC1_RXLINK_START_S  29
/* SLC_SLC1_RXLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RXLINK_STOP  (BIT(28))
#define SLC_SLC1_RXLINK_STOP_M  (BIT(28))
#define SLC_SLC1_RXLINK_STOP_V  0x1
#define SLC_SLC1_RXLINK_STOP_S  28
/* SLC_SLC1_BT_PACKET : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_BT_PACKET  (BIT(20))
#define SLC_SLC1_BT_PACKET_M  (BIT(20))
#define SLC_SLC1_BT_PACKET_V  0x1
#define SLC_SLC1_BT_PACKET_S  20
/* SLC_SLC1_RXLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define SLC_SLC1_RXLINK_ADDR  0x000FFFFF
#define SLC_SLC1_RXLINK_ADDR_M  ((SLC_SLC1_RXLINK_ADDR_V)<<(SLC_SLC1_RXLINK_ADDR_S))
#define SLC_SLC1_RXLINK_ADDR_V  0xFFFFF
#define SLC_SLC1_RXLINK_ADDR_S  0

#define SLC_1TX_LINK_REG          (DR_REG_SLC_BASE + 0x48)
/* SLC_SLC1_TXLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC1_TXLINK_PARK  (BIT(31))
#define SLC_SLC1_TXLINK_PARK_M  (BIT(31))
#define SLC_SLC1_TXLINK_PARK_V  0x1
#define SLC_SLC1_TXLINK_PARK_S  31
/* SLC_SLC1_TXLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TXLINK_RESTART  (BIT(30))
#define SLC_SLC1_TXLINK_RESTART_M  (BIT(30))
#define SLC_SLC1_TXLINK_RESTART_V  0x1
#define SLC_SLC1_TXLINK_RESTART_S  30
/* SLC_SLC1_TXLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TXLINK_START  (BIT(29))
#define SLC_SLC1_TXLINK_START_M  (BIT(29))
#define SLC_SLC1_TXLINK_START_V  0x1
#define SLC_SLC1_TXLINK_START_S  29
/* SLC_SLC1_TXLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TXLINK_STOP  (BIT(28))
#define SLC_SLC1_TXLINK_STOP_M  (BIT(28))
#define SLC_SLC1_TXLINK_STOP_V  0x1
#define SLC_SLC1_TXLINK_STOP_S  28
/* SLC_SLC1_TXLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define SLC_SLC1_TXLINK_ADDR  0x000FFFFF
#define SLC_SLC1_TXLINK_ADDR_M  ((SLC_SLC1_TXLINK_ADDR_V)<<(SLC_SLC1_TXLINK_ADDR_S))
#define SLC_SLC1_TXLINK_ADDR_V  0xFFFFF
#define SLC_SLC1_TXLINK_ADDR_S  0

#define SLC_INTVEC_TOHOST_REG          (DR_REG_SLC_BASE + 0x4C)
/* SLC_SLC1_TOHOST_INTVEC : WO ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define SLC_SLC1_TOHOST_INTVEC  0x000000FF
#define SLC_SLC1_TOHOST_INTVEC_M  ((SLC_SLC1_TOHOST_INTVEC_V)<<(SLC_SLC1_TOHOST_INTVEC_S))
#define SLC_SLC1_TOHOST_INTVEC_V  0xFF
#define SLC_SLC1_TOHOST_INTVEC_S  16
/* SLC_SLC0_TOHOST_INTVEC : WO ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define SLC_SLC0_TOHOST_INTVEC  0x000000FF
#define SLC_SLC0_TOHOST_INTVEC_M  ((SLC_SLC0_TOHOST_INTVEC_V)<<(SLC_SLC0_TOHOST_INTVEC_S))
#define SLC_SLC0_TOHOST_INTVEC_V  0xFF
#define SLC_SLC0_TOHOST_INTVEC_S  0

#define SLC_0TOKEN0_REG          (DR_REG_SLC_BASE + 0x50)
/* SLC_SLC0_TOKEN0 : RO ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC0_TOKEN0  0x00000FFF
#define SLC_SLC0_TOKEN0_M  ((SLC_SLC0_TOKEN0_V)<<(SLC_SLC0_TOKEN0_S))
#define SLC_SLC0_TOKEN0_V  0xFFF
#define SLC_SLC0_TOKEN0_S  16
/* SLC_SLC0_TOKEN0_INC_MORE : WO ;bitpos:[14] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_INC_MORE  (BIT(14))
#define SLC_SLC0_TOKEN0_INC_MORE_M  (BIT(14))
#define SLC_SLC0_TOKEN0_INC_MORE_V  0x1
#define SLC_SLC0_TOKEN0_INC_MORE_S  14
/* SLC_SLC0_TOKEN0_INC : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_INC  (BIT(13))
#define SLC_SLC0_TOKEN0_INC_M  (BIT(13))
#define SLC_SLC0_TOKEN0_INC_V  0x1
#define SLC_SLC0_TOKEN0_INC_S  13
/* SLC_SLC0_TOKEN0_WR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_WR  (BIT(12))
#define SLC_SLC0_TOKEN0_WR_M  (BIT(12))
#define SLC_SLC0_TOKEN0_WR_V  0x1
#define SLC_SLC0_TOKEN0_WR_S  12
/* SLC_SLC0_TOKEN0_WDATA : WO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_WDATA  0x00000FFF
#define SLC_SLC0_TOKEN0_WDATA_M  ((SLC_SLC0_TOKEN0_WDATA_V)<<(SLC_SLC0_TOKEN0_WDATA_S))
#define SLC_SLC0_TOKEN0_WDATA_V  0xFFF
#define SLC_SLC0_TOKEN0_WDATA_S  0

#define SLC_0TOKEN1_REG          (DR_REG_SLC_BASE + 0x54)
/* SLC_SLC0_TOKEN1 : RO ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC0_TOKEN1  0x00000FFF
#define SLC_SLC0_TOKEN1_M  ((SLC_SLC0_TOKEN1_V)<<(SLC_SLC0_TOKEN1_S))
#define SLC_SLC0_TOKEN1_V  0xFFF
#define SLC_SLC0_TOKEN1_S  16
/* SLC_SLC0_TOKEN1_INC_MORE : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_INC_MORE  (BIT(14))
#define SLC_SLC0_TOKEN1_INC_MORE_M  (BIT(14))
#define SLC_SLC0_TOKEN1_INC_MORE_V  0x1
#define SLC_SLC0_TOKEN1_INC_MORE_S  14
/* SLC_SLC0_TOKEN1_INC : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_INC  (BIT(13))
#define SLC_SLC0_TOKEN1_INC_M  (BIT(13))
#define SLC_SLC0_TOKEN1_INC_V  0x1
#define SLC_SLC0_TOKEN1_INC_S  13
/* SLC_SLC0_TOKEN1_WR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_WR  (BIT(12))
#define SLC_SLC0_TOKEN1_WR_M  (BIT(12))
#define SLC_SLC0_TOKEN1_WR_V  0x1
#define SLC_SLC0_TOKEN1_WR_S  12
/* SLC_SLC0_TOKEN1_WDATA : WO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_WDATA  0x00000FFF
#define SLC_SLC0_TOKEN1_WDATA_M  ((SLC_SLC0_TOKEN1_WDATA_V)<<(SLC_SLC0_TOKEN1_WDATA_S))
#define SLC_SLC0_TOKEN1_WDATA_V  0xFFF
#define SLC_SLC0_TOKEN1_WDATA_S  0

#define SLC_1TOKEN0_REG          (DR_REG_SLC_BASE + 0x58)
/* SLC_SLC1_TOKEN0 : RO ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC1_TOKEN0  0x00000FFF
#define SLC_SLC1_TOKEN0_M  ((SLC_SLC1_TOKEN0_V)<<(SLC_SLC1_TOKEN0_S))
#define SLC_SLC1_TOKEN0_V  0xFFF
#define SLC_SLC1_TOKEN0_S  16
/* SLC_SLC1_TOKEN0_INC_MORE : WO ;bitpos:[14] ;default: 1'h0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_INC_MORE  (BIT(14))
#define SLC_SLC1_TOKEN0_INC_MORE_M  (BIT(14))
#define SLC_SLC1_TOKEN0_INC_MORE_V  0x1
#define SLC_SLC1_TOKEN0_INC_MORE_S  14
/* SLC_SLC1_TOKEN0_INC : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_INC  (BIT(13))
#define SLC_SLC1_TOKEN0_INC_M  (BIT(13))
#define SLC_SLC1_TOKEN0_INC_V  0x1
#define SLC_SLC1_TOKEN0_INC_S  13
/* SLC_SLC1_TOKEN0_WR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_WR  (BIT(12))
#define SLC_SLC1_TOKEN0_WR_M  (BIT(12))
#define SLC_SLC1_TOKEN0_WR_V  0x1
#define SLC_SLC1_TOKEN0_WR_S  12
/* SLC_SLC1_TOKEN0_WDATA : WO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_WDATA  0x00000FFF
#define SLC_SLC1_TOKEN0_WDATA_M  ((SLC_SLC1_TOKEN0_WDATA_V)<<(SLC_SLC1_TOKEN0_WDATA_S))
#define SLC_SLC1_TOKEN0_WDATA_V  0xFFF
#define SLC_SLC1_TOKEN0_WDATA_S  0

#define SLC_1TOKEN1_REG          (DR_REG_SLC_BASE + 0x5C)
/* SLC_SLC1_TOKEN1 : RO ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC1_TOKEN1  0x00000FFF
#define SLC_SLC1_TOKEN1_M  ((SLC_SLC1_TOKEN1_V)<<(SLC_SLC1_TOKEN1_S))
#define SLC_SLC1_TOKEN1_V  0xFFF
#define SLC_SLC1_TOKEN1_S  16
/* SLC_SLC1_TOKEN1_INC_MORE : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_INC_MORE  (BIT(14))
#define SLC_SLC1_TOKEN1_INC_MORE_M  (BIT(14))
#define SLC_SLC1_TOKEN1_INC_MORE_V  0x1
#define SLC_SLC1_TOKEN1_INC_MORE_S  14
/* SLC_SLC1_TOKEN1_INC : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_INC  (BIT(13))
#define SLC_SLC1_TOKEN1_INC_M  (BIT(13))
#define SLC_SLC1_TOKEN1_INC_V  0x1
#define SLC_SLC1_TOKEN1_INC_S  13
/* SLC_SLC1_TOKEN1_WR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_WR  (BIT(12))
#define SLC_SLC1_TOKEN1_WR_M  (BIT(12))
#define SLC_SLC1_TOKEN1_WR_V  0x1
#define SLC_SLC1_TOKEN1_WR_S  12
/* SLC_SLC1_TOKEN1_WDATA : WO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_WDATA  0x00000FFF
#define SLC_SLC1_TOKEN1_WDATA_M  ((SLC_SLC1_TOKEN1_WDATA_V)<<(SLC_SLC1_TOKEN1_WDATA_S))
#define SLC_SLC1_TOKEN1_WDATA_V  0xFFF
#define SLC_SLC1_TOKEN1_WDATA_S  0

#define SLC_CONF1_REG          (DR_REG_SLC_BASE + 0x60)
/* SLC_CLK_EN : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_CLK_EN  (BIT(22))
#define SLC_CLK_EN_M  (BIT(22))
#define SLC_CLK_EN_V  0x1
#define SLC_CLK_EN_S  22
/* SLC_SLC1_RX_STITCH_EN : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RX_STITCH_EN  (BIT(21))
#define SLC_SLC1_RX_STITCH_EN_M  (BIT(21))
#define SLC_SLC1_RX_STITCH_EN_V  0x1
#define SLC_SLC1_RX_STITCH_EN_S  21
/* SLC_SLC1_TX_STITCH_EN : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_TX_STITCH_EN  (BIT(20))
#define SLC_SLC1_TX_STITCH_EN_M  (BIT(20))
#define SLC_SLC1_TX_STITCH_EN_V  0x1
#define SLC_SLC1_TX_STITCH_EN_S  20
/* SLC_HOST_INT_LEVEL_SEL : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_HOST_INT_LEVEL_SEL  (BIT(19))
#define SLC_HOST_INT_LEVEL_SEL_M  (BIT(19))
#define SLC_HOST_INT_LEVEL_SEL_V  0x1
#define SLC_HOST_INT_LEVEL_SEL_S  19
/* SLC_SLC1_RX_CHECK_SUM_EN : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_CHECK_SUM_EN  (BIT(18))
#define SLC_SLC1_RX_CHECK_SUM_EN_M  (BIT(18))
#define SLC_SLC1_RX_CHECK_SUM_EN_V  0x1
#define SLC_SLC1_RX_CHECK_SUM_EN_S  18
/* SLC_SLC1_TX_CHECK_SUM_EN : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_CHECK_SUM_EN  (BIT(17))
#define SLC_SLC1_TX_CHECK_SUM_EN_M  (BIT(17))
#define SLC_SLC1_TX_CHECK_SUM_EN_V  0x1
#define SLC_SLC1_TX_CHECK_SUM_EN_S  17
/* SLC_SLC1_CHECK_OWNER : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_CHECK_OWNER  (BIT(16))
#define SLC_SLC1_CHECK_OWNER_M  (BIT(16))
#define SLC_SLC1_CHECK_OWNER_V  0x1
#define SLC_SLC1_CHECK_OWNER_S  16
/* SLC_SLC0_RX_STITCH_EN : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_RX_STITCH_EN  (BIT(6))
#define SLC_SLC0_RX_STITCH_EN_M  (BIT(6))
#define SLC_SLC0_RX_STITCH_EN_V  0x1
#define SLC_SLC0_RX_STITCH_EN_S  6
/* SLC_SLC0_TX_STITCH_EN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_TX_STITCH_EN  (BIT(5))
#define SLC_SLC0_TX_STITCH_EN_M  (BIT(5))
#define SLC_SLC0_TX_STITCH_EN_V  0x1
#define SLC_SLC0_TX_STITCH_EN_S  5
/* SLC_SLC0_LEN_AUTO_CLR : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_LEN_AUTO_CLR  (BIT(4))
#define SLC_SLC0_LEN_AUTO_CLR_M  (BIT(4))
#define SLC_SLC0_LEN_AUTO_CLR_V  0x1
#define SLC_SLC0_LEN_AUTO_CLR_S  4
/* SLC_CMD_HOLD_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define SLC_CMD_HOLD_EN  (BIT(3))
#define SLC_CMD_HOLD_EN_M  (BIT(3))
#define SLC_CMD_HOLD_EN_V  0x1
#define SLC_CMD_HOLD_EN_S  3
/* SLC_SLC0_RX_CHECK_SUM_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_CHECK_SUM_EN  (BIT(2))
#define SLC_SLC0_RX_CHECK_SUM_EN_M  (BIT(2))
#define SLC_SLC0_RX_CHECK_SUM_EN_V  0x1
#define SLC_SLC0_RX_CHECK_SUM_EN_S  2
/* SLC_SLC0_TX_CHECK_SUM_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_CHECK_SUM_EN  (BIT(1))
#define SLC_SLC0_TX_CHECK_SUM_EN_M  (BIT(1))
#define SLC_SLC0_TX_CHECK_SUM_EN_V  0x1
#define SLC_SLC0_TX_CHECK_SUM_EN_S  1
/* SLC_SLC0_CHECK_OWNER : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_CHECK_OWNER  (BIT(0))
#define SLC_SLC0_CHECK_OWNER_M  (BIT(0))
#define SLC_SLC0_CHECK_OWNER_V  0x1
#define SLC_SLC0_CHECK_OWNER_S  0

#define SLC_0_STATE0_REG          (DR_REG_SLC_BASE + 0x64)
/* SLC_SLC0_STATE0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_STATE0  0xFFFFFFFF
#define SLC_SLC0_STATE0_M  ((SLC_SLC0_STATE0_V)<<(SLC_SLC0_STATE0_S))
#define SLC_SLC0_STATE0_V  0xFFFFFFFF
#define SLC_SLC0_STATE0_S  0

#define SLC_0_STATE1_REG          (DR_REG_SLC_BASE + 0x68)
/* SLC_SLC0_STATE1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_STATE1  0xFFFFFFFF
#define SLC_SLC0_STATE1_M  ((SLC_SLC0_STATE1_V)<<(SLC_SLC0_STATE1_S))
#define SLC_SLC0_STATE1_V  0xFFFFFFFF
#define SLC_SLC0_STATE1_S  0

#define SLC_1_STATE0_REG          (DR_REG_SLC_BASE + 0x6C)
/* SLC_SLC1_STATE0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC1_STATE0  0xFFFFFFFF
#define SLC_SLC1_STATE0_M  ((SLC_SLC1_STATE0_V)<<(SLC_SLC1_STATE0_S))
#define SLC_SLC1_STATE0_V  0xFFFFFFFF
#define SLC_SLC1_STATE0_S  0

#define SLC_1_STATE1_REG          (DR_REG_SLC_BASE + 0x70)
/* SLC_SLC1_STATE1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC1_STATE1  0xFFFFFFFF
#define SLC_SLC1_STATE1_M  ((SLC_SLC1_STATE1_V)<<(SLC_SLC1_STATE1_S))
#define SLC_SLC1_STATE1_V  0xFFFFFFFF
#define SLC_SLC1_STATE1_S  0

#define SLC_BRIDGE_CONF_REG          (DR_REG_SLC_BASE + 0x74)
/* SLC_TX_PUSH_IDLE_NUM : R/W ;bitpos:[31:16] ;default: 16'ha ; */
/*description: */
#define SLC_TX_PUSH_IDLE_NUM  0x0000FFFF
#define SLC_TX_PUSH_IDLE_NUM_M  ((SLC_TX_PUSH_IDLE_NUM_V)<<(SLC_TX_PUSH_IDLE_NUM_S))
#define SLC_TX_PUSH_IDLE_NUM_V  0xFFFF
#define SLC_TX_PUSH_IDLE_NUM_S  16
/* SLC_SLC1_TX_DUMMY_MODE : R/W ;bitpos:[14] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC1_TX_DUMMY_MODE  (BIT(14))
#define SLC_SLC1_TX_DUMMY_MODE_M  (BIT(14))
#define SLC_SLC1_TX_DUMMY_MODE_V  0x1
#define SLC_SLC1_TX_DUMMY_MODE_S  14
/* SLC_HDA_MAP_128K : R/W ;bitpos:[13] ;default: 1'h1 ; */
/*description: */
#define SLC_HDA_MAP_128K  (BIT(13))
#define SLC_HDA_MAP_128K_M  (BIT(13))
#define SLC_HDA_MAP_128K_V  0x1
#define SLC_HDA_MAP_128K_S  13
/* SLC_SLC0_TX_DUMMY_MODE : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: */
#define SLC_SLC0_TX_DUMMY_MODE  (BIT(12))
#define SLC_SLC0_TX_DUMMY_MODE_M  (BIT(12))
#define SLC_SLC0_TX_DUMMY_MODE_V  0x1
#define SLC_SLC0_TX_DUMMY_MODE_S  12
/* SLC_FIFO_MAP_ENA : R/W ;bitpos:[11:8] ;default: 4'h7 ; */
/*description: */
#define SLC_FIFO_MAP_ENA  0x0000000F
#define SLC_FIFO_MAP_ENA_M  ((SLC_FIFO_MAP_ENA_V)<<(SLC_FIFO_MAP_ENA_S))
#define SLC_FIFO_MAP_ENA_V  0xF
#define SLC_FIFO_MAP_ENA_S  8
/* SLC_TXEOF_ENA : R/W ;bitpos:[5:0] ;default: 6'h20 ; */
/*description: */
#define SLC_TXEOF_ENA  0x0000003F
#define SLC_TXEOF_ENA_M  ((SLC_TXEOF_ENA_V)<<(SLC_TXEOF_ENA_S))
#define SLC_TXEOF_ENA_V  0x3F
#define SLC_TXEOF_ENA_S  0

#define SLC_0_TO_EOF_DES_ADDR_REG          (DR_REG_SLC_BASE + 0x78)
/* SLC_SLC0_TO_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TO_EOF_DES_ADDR  0xFFFFFFFF
#define SLC_SLC0_TO_EOF_DES_ADDR_M  ((SLC_SLC0_TO_EOF_DES_ADDR_V)<<(SLC_SLC0_TO_EOF_DES_ADDR_S))
#define SLC_SLC0_TO_EOF_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TO_EOF_DES_ADDR_S  0

#define SLC_0_TX_EOF_DES_ADDR_REG          (DR_REG_SLC_BASE + 0x7C)
/* SLC_SLC0_TX_SUC_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TX_SUC_EOF_DES_ADDR  0xFFFFFFFF
#define SLC_SLC0_TX_SUC_EOF_DES_ADDR_M  ((SLC_SLC0_TX_SUC_EOF_DES_ADDR_V)<<(SLC_SLC0_TX_SUC_EOF_DES_ADDR_S))
#define SLC_SLC0_TX_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TX_SUC_EOF_DES_ADDR_S  0

#define SLC_0_TO_EOF_BFR_DES_ADDR_REG          (DR_REG_SLC_BASE + 0x80)
/* SLC_SLC0_TO_EOF_BFR_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TO_EOF_BFR_DES_ADDR  0xFFFFFFFF
#define SLC_SLC0_TO_EOF_BFR_DES_ADDR_M  ((SLC_SLC0_TO_EOF_BFR_DES_ADDR_V)<<(SLC_SLC0_TO_EOF_BFR_DES_ADDR_S))
#define SLC_SLC0_TO_EOF_BFR_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TO_EOF_BFR_DES_ADDR_S  0

#define SLC_1_TO_EOF_DES_ADDR_REG          (DR_REG_SLC_BASE + 0x84)
/* SLC_SLC1_TO_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC1_TO_EOF_DES_ADDR  0xFFFFFFFF
#define SLC_SLC1_TO_EOF_DES_ADDR_M  ((SLC_SLC1_TO_EOF_DES_ADDR_V)<<(SLC_SLC1_TO_EOF_DES_ADDR_S))
#define SLC_SLC1_TO_EOF_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC1_TO_EOF_DES_ADDR_S  0

#define SLC_1_TX_EOF_DES_ADDR_REG          (DR_REG_SLC_BASE + 0x88)
/* SLC_SLC1_TX_SUC_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC1_TX_SUC_EOF_DES_ADDR  0xFFFFFFFF
#define SLC_SLC1_TX_SUC_EOF_DES_ADDR_M  ((SLC_SLC1_TX_SUC_EOF_DES_ADDR_V)<<(SLC_SLC1_TX_SUC_EOF_DES_ADDR_S))
#define SLC_SLC1_TX_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC1_TX_SUC_EOF_DES_ADDR_S  0

#define SLC_1_TO_EOF_BFR_DES_ADDR_REG          (DR_REG_SLC_BASE + 0x8C)
/* SLC_SLC1_TO_EOF_BFR_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC1_TO_EOF_BFR_DES_ADDR  0xFFFFFFFF
#define SLC_SLC1_TO_EOF_BFR_DES_ADDR_M  ((SLC_SLC1_TO_EOF_BFR_DES_ADDR_V)<<(SLC_SLC1_TO_EOF_BFR_DES_ADDR_S))
#define SLC_SLC1_TO_EOF_BFR_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC1_TO_EOF_BFR_DES_ADDR_S  0

#define SLC_AHB_TEST_REG          (DR_REG_SLC_BASE + 0x90)
/* SLC_AHB_TESTADDR : R/W ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: */
#define SLC_AHB_TESTADDR  0x00000003
#define SLC_AHB_TESTADDR_M  ((SLC_AHB_TESTADDR_V)<<(SLC_AHB_TESTADDR_S))
#define SLC_AHB_TESTADDR_V  0x3
#define SLC_AHB_TESTADDR_S  4
/* SLC_AHB_TESTMODE : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define SLC_AHB_TESTMODE  0x00000007
#define SLC_AHB_TESTMODE_M  ((SLC_AHB_TESTMODE_V)<<(SLC_AHB_TESTMODE_S))
#define SLC_AHB_TESTMODE_V  0x7
#define SLC_AHB_TESTMODE_S  0

#define SLC_SDIO_ST_REG          (DR_REG_SLC_BASE + 0x94)
/* SLC_FUNC2_ACC_STATE : RO ;bitpos:[28:24] ;default: 5'b0 ; */
/*description: */
#define SLC_FUNC2_ACC_STATE  0x0000001F
#define SLC_FUNC2_ACC_STATE_M  ((SLC_FUNC2_ACC_STATE_V)<<(SLC_FUNC2_ACC_STATE_S))
#define SLC_FUNC2_ACC_STATE_V  0x1F
#define SLC_FUNC2_ACC_STATE_S  24
/* SLC_FUNC1_ACC_STATE : RO ;bitpos:[20:16] ;default: 5'b0 ; */
/*description: */
#define SLC_FUNC1_ACC_STATE  0x0000001F
#define SLC_FUNC1_ACC_STATE_M  ((SLC_FUNC1_ACC_STATE_V)<<(SLC_FUNC1_ACC_STATE_S))
#define SLC_FUNC1_ACC_STATE_V  0x1F
#define SLC_FUNC1_ACC_STATE_S  16
/* SLC_BUS_ST : RO ;bitpos:[14:12] ;default: 3'b0 ; */
/*description: */
#define SLC_BUS_ST  0x00000007
#define SLC_BUS_ST_M  ((SLC_BUS_ST_V)<<(SLC_BUS_ST_S))
#define SLC_BUS_ST_V  0x7
#define SLC_BUS_ST_S  12
/* SLC_SDIO_WAKEUP : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SDIO_WAKEUP  (BIT(8))
#define SLC_SDIO_WAKEUP_M  (BIT(8))
#define SLC_SDIO_WAKEUP_V  0x1
#define SLC_SDIO_WAKEUP_S  8
/* SLC_FUNC_ST : RO ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: */
#define SLC_FUNC_ST  0x0000000F
#define SLC_FUNC_ST_M  ((SLC_FUNC_ST_V)<<(SLC_FUNC_ST_S))
#define SLC_FUNC_ST_V  0xF
#define SLC_FUNC_ST_S  4
/* SLC_CMD_ST : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define SLC_CMD_ST  0x00000007
#define SLC_CMD_ST_M  ((SLC_CMD_ST_V)<<(SLC_CMD_ST_S))
#define SLC_CMD_ST_V  0x7
#define SLC_CMD_ST_S  0

#define SLC_RX_DSCR_CONF_REG          (DR_REG_SLC_BASE + 0x98)
/* SLC_SLC1_RD_RETRY_THRESHOLD : R/W ;bitpos:[31:21] ;default: 11'h80 ; */
/*description: */
#define SLC_SLC1_RD_RETRY_THRESHOLD  0x000007FF
#define SLC_SLC1_RD_RETRY_THRESHOLD_M  ((SLC_SLC1_RD_RETRY_THRESHOLD_V)<<(SLC_SLC1_RD_RETRY_THRESHOLD_S))
#define SLC_SLC1_RD_RETRY_THRESHOLD_V  0x7FF
#define SLC_SLC1_RD_RETRY_THRESHOLD_S  21
/* SLC_SLC1_RX_FILL_EN : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RX_FILL_EN  (BIT(20))
#define SLC_SLC1_RX_FILL_EN_M  (BIT(20))
#define SLC_SLC1_RX_FILL_EN_V  0x1
#define SLC_SLC1_RX_FILL_EN_S  20
/* SLC_SLC1_RX_EOF_MODE : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_RX_EOF_MODE  (BIT(19))
#define SLC_SLC1_RX_EOF_MODE_M  (BIT(19))
#define SLC_SLC1_RX_EOF_MODE_V  0x1
#define SLC_SLC1_RX_EOF_MODE_S  19
/* SLC_SLC1_RX_FILL_MODE : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_FILL_MODE  (BIT(18))
#define SLC_SLC1_RX_FILL_MODE_M  (BIT(18))
#define SLC_SLC1_RX_FILL_MODE_V  0x1
#define SLC_SLC1_RX_FILL_MODE_S  18
/* SLC_SLC1_INFOR_NO_REPLACE : R/W ;bitpos:[17] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_INFOR_NO_REPLACE  (BIT(17))
#define SLC_SLC1_INFOR_NO_REPLACE_M  (BIT(17))
#define SLC_SLC1_INFOR_NO_REPLACE_V  0x1
#define SLC_SLC1_INFOR_NO_REPLACE_S  17
/* SLC_SLC1_TOKEN_NO_REPLACE : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC1_TOKEN_NO_REPLACE  (BIT(16))
#define SLC_SLC1_TOKEN_NO_REPLACE_M  (BIT(16))
#define SLC_SLC1_TOKEN_NO_REPLACE_V  0x1
#define SLC_SLC1_TOKEN_NO_REPLACE_S  16
/* SLC_SLC0_RD_RETRY_THRESHOLD : R/W ;bitpos:[15:5] ;default: 11'h80 ; */
/*description: */
#define SLC_SLC0_RD_RETRY_THRESHOLD  0x000007FF
#define SLC_SLC0_RD_RETRY_THRESHOLD_M  ((SLC_SLC0_RD_RETRY_THRESHOLD_V)<<(SLC_SLC0_RD_RETRY_THRESHOLD_S))
#define SLC_SLC0_RD_RETRY_THRESHOLD_V  0x7FF
#define SLC_SLC0_RD_RETRY_THRESHOLD_S  5
/* SLC_SLC0_RX_FILL_EN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_RX_FILL_EN  (BIT(4))
#define SLC_SLC0_RX_FILL_EN_M  (BIT(4))
#define SLC_SLC0_RX_FILL_EN_V  0x1
#define SLC_SLC0_RX_FILL_EN_S  4
/* SLC_SLC0_RX_EOF_MODE : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_RX_EOF_MODE  (BIT(3))
#define SLC_SLC0_RX_EOF_MODE_M  (BIT(3))
#define SLC_SLC0_RX_EOF_MODE_V  0x1
#define SLC_SLC0_RX_EOF_MODE_S  3
/* SLC_SLC0_RX_FILL_MODE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_FILL_MODE  (BIT(2))
#define SLC_SLC0_RX_FILL_MODE_M  (BIT(2))
#define SLC_SLC0_RX_FILL_MODE_V  0x1
#define SLC_SLC0_RX_FILL_MODE_S  2
/* SLC_SLC0_INFOR_NO_REPLACE : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define SLC_SLC0_INFOR_NO_REPLACE  (BIT(1))
#define SLC_SLC0_INFOR_NO_REPLACE_M  (BIT(1))
#define SLC_SLC0_INFOR_NO_REPLACE_V  0x1
#define SLC_SLC0_INFOR_NO_REPLACE_S  1
/* SLC_SLC0_TOKEN_NO_REPLACE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN_NO_REPLACE  (BIT(0))
#define SLC_SLC0_TOKEN_NO_REPLACE_M  (BIT(0))
#define SLC_SLC0_TOKEN_NO_REPLACE_V  0x1
#define SLC_SLC0_TOKEN_NO_REPLACE_S  0

#define SLC_0_TXLINK_DSCR_REG          (DR_REG_SLC_BASE + 0x9C)
/* SLC_SLC0_TXLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_TXLINK_DSCR  0xFFFFFFFF
#define SLC_SLC0_TXLINK_DSCR_M  ((SLC_SLC0_TXLINK_DSCR_V)<<(SLC_SLC0_TXLINK_DSCR_S))
#define SLC_SLC0_TXLINK_DSCR_V  0xFFFFFFFF
#define SLC_SLC0_TXLINK_DSCR_S  0

#define SLC_0_TXLINK_DSCR_BF0_REG          (DR_REG_SLC_BASE + 0xA0)
/* SLC_SLC0_TXLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_TXLINK_DSCR_BF0  0xFFFFFFFF
#define SLC_SLC0_TXLINK_DSCR_BF0_M  ((SLC_SLC0_TXLINK_DSCR_BF0_V)<<(SLC_SLC0_TXLINK_DSCR_BF0_S))
#define SLC_SLC0_TXLINK_DSCR_BF0_V  0xFFFFFFFF
#define SLC_SLC0_TXLINK_DSCR_BF0_S  0

#define SLC_0_TXLINK_DSCR_BF1_REG          (DR_REG_SLC_BASE + 0xA4)
/* SLC_SLC0_TXLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_TXLINK_DSCR_BF1  0xFFFFFFFF
#define SLC_SLC0_TXLINK_DSCR_BF1_M  ((SLC_SLC0_TXLINK_DSCR_BF1_V)<<(SLC_SLC0_TXLINK_DSCR_BF1_S))
#define SLC_SLC0_TXLINK_DSCR_BF1_V  0xFFFFFFFF
#define SLC_SLC0_TXLINK_DSCR_BF1_S  0

#define SLC_0_RXLINK_DSCR_REG          (DR_REG_SLC_BASE + 0xA8)
/* SLC_SLC0_RXLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_RXLINK_DSCR  0xFFFFFFFF
#define SLC_SLC0_RXLINK_DSCR_M  ((SLC_SLC0_RXLINK_DSCR_V)<<(SLC_SLC0_RXLINK_DSCR_S))
#define SLC_SLC0_RXLINK_DSCR_V  0xFFFFFFFF
#define SLC_SLC0_RXLINK_DSCR_S  0

#define SLC_0_RXLINK_DSCR_BF0_REG          (DR_REG_SLC_BASE + 0xAC)
/* SLC_SLC0_RXLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_RXLINK_DSCR_BF0  0xFFFFFFFF
#define SLC_SLC0_RXLINK_DSCR_BF0_M  ((SLC_SLC0_RXLINK_DSCR_BF0_V)<<(SLC_SLC0_RXLINK_DSCR_BF0_S))
#define SLC_SLC0_RXLINK_DSCR_BF0_V  0xFFFFFFFF
#define SLC_SLC0_RXLINK_DSCR_BF0_S  0

#define SLC_0_RXLINK_DSCR_BF1_REG          (DR_REG_SLC_BASE + 0xB0)
/* SLC_SLC0_RXLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_RXLINK_DSCR_BF1  0xFFFFFFFF
#define SLC_SLC0_RXLINK_DSCR_BF1_M  ((SLC_SLC0_RXLINK_DSCR_BF1_V)<<(SLC_SLC0_RXLINK_DSCR_BF1_S))
#define SLC_SLC0_RXLINK_DSCR_BF1_V  0xFFFFFFFF
#define SLC_SLC0_RXLINK_DSCR_BF1_S  0

#define SLC_1_TXLINK_DSCR_REG          (DR_REG_SLC_BASE + 0xB4)
/* SLC_SLC1_TXLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC1_TXLINK_DSCR  0xFFFFFFFF
#define SLC_SLC1_TXLINK_DSCR_M  ((SLC_SLC1_TXLINK_DSCR_V)<<(SLC_SLC1_TXLINK_DSCR_S))
#define SLC_SLC1_TXLINK_DSCR_V  0xFFFFFFFF
#define SLC_SLC1_TXLINK_DSCR_S  0

#define SLC_1_TXLINK_DSCR_BF0_REG          (DR_REG_SLC_BASE + 0xB8)
/* SLC_SLC1_TXLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC1_TXLINK_DSCR_BF0  0xFFFFFFFF
#define SLC_SLC1_TXLINK_DSCR_BF0_M  ((SLC_SLC1_TXLINK_DSCR_BF0_V)<<(SLC_SLC1_TXLINK_DSCR_BF0_S))
#define SLC_SLC1_TXLINK_DSCR_BF0_V  0xFFFFFFFF
#define SLC_SLC1_TXLINK_DSCR_BF0_S  0

#define SLC_1_TXLINK_DSCR_BF1_REG          (DR_REG_SLC_BASE + 0xBC)
/* SLC_SLC1_TXLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC1_TXLINK_DSCR_BF1  0xFFFFFFFF
#define SLC_SLC1_TXLINK_DSCR_BF1_M  ((SLC_SLC1_TXLINK_DSCR_BF1_V)<<(SLC_SLC1_TXLINK_DSCR_BF1_S))
#define SLC_SLC1_TXLINK_DSCR_BF1_V  0xFFFFFFFF
#define SLC_SLC1_TXLINK_DSCR_BF1_S  0

#define SLC_1_RXLINK_DSCR_REG          (DR_REG_SLC_BASE + 0xC0)
/* SLC_SLC1_RXLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC1_RXLINK_DSCR  0xFFFFFFFF
#define SLC_SLC1_RXLINK_DSCR_M  ((SLC_SLC1_RXLINK_DSCR_V)<<(SLC_SLC1_RXLINK_DSCR_S))
#define SLC_SLC1_RXLINK_DSCR_V  0xFFFFFFFF
#define SLC_SLC1_RXLINK_DSCR_S  0

#define SLC_1_RXLINK_DSCR_BF0_REG          (DR_REG_SLC_BASE + 0xC4)
/* SLC_SLC1_RXLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC1_RXLINK_DSCR_BF0  0xFFFFFFFF
#define SLC_SLC1_RXLINK_DSCR_BF0_M  ((SLC_SLC1_RXLINK_DSCR_BF0_V)<<(SLC_SLC1_RXLINK_DSCR_BF0_S))
#define SLC_SLC1_RXLINK_DSCR_BF0_V  0xFFFFFFFF
#define SLC_SLC1_RXLINK_DSCR_BF0_S  0

#define SLC_1_RXLINK_DSCR_BF1_REG          (DR_REG_SLC_BASE + 0xC8)
/* SLC_SLC1_RXLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC1_RXLINK_DSCR_BF1  0xFFFFFFFF
#define SLC_SLC1_RXLINK_DSCR_BF1_M  ((SLC_SLC1_RXLINK_DSCR_BF1_V)<<(SLC_SLC1_RXLINK_DSCR_BF1_S))
#define SLC_SLC1_RXLINK_DSCR_BF1_V  0xFFFFFFFF
#define SLC_SLC1_RXLINK_DSCR_BF1_S  0

#define SLC_0_TX_ERREOF_DES_ADDR_REG          (DR_REG_SLC_BASE + 0xCC)
/* SLC_SLC0_TX_ERR_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TX_ERR_EOF_DES_ADDR  0xFFFFFFFF
#define SLC_SLC0_TX_ERR_EOF_DES_ADDR_M  ((SLC_SLC0_TX_ERR_EOF_DES_ADDR_V)<<(SLC_SLC0_TX_ERR_EOF_DES_ADDR_S))
#define SLC_SLC0_TX_ERR_EOF_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TX_ERR_EOF_DES_ADDR_S  0

#define SLC_1_TX_ERREOF_DES_ADDR_REG          (DR_REG_SLC_BASE + 0xD0)
/* SLC_SLC1_TX_ERR_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC1_TX_ERR_EOF_DES_ADDR  0xFFFFFFFF
#define SLC_SLC1_TX_ERR_EOF_DES_ADDR_M  ((SLC_SLC1_TX_ERR_EOF_DES_ADDR_V)<<(SLC_SLC1_TX_ERR_EOF_DES_ADDR_S))
#define SLC_SLC1_TX_ERR_EOF_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC1_TX_ERR_EOF_DES_ADDR_S  0

#define SLC_TOKEN_LAT_REG          (DR_REG_SLC_BASE + 0xD4)
/* SLC_SLC1_TOKEN : RO ;bitpos:[27:16] ;default: 12'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN  0x00000FFF
#define SLC_SLC1_TOKEN_M  ((SLC_SLC1_TOKEN_V)<<(SLC_SLC1_TOKEN_S))
#define SLC_SLC1_TOKEN_V  0xFFF
#define SLC_SLC1_TOKEN_S  16
/* SLC_SLC0_TOKEN : RO ;bitpos:[11:0] ;default: 12'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN  0x00000FFF
#define SLC_SLC0_TOKEN_M  ((SLC_SLC0_TOKEN_V)<<(SLC_SLC0_TOKEN_S))
#define SLC_SLC0_TOKEN_V  0xFFF
#define SLC_SLC0_TOKEN_S  0

#define SLC_TX_DSCR_CONF_REG          (DR_REG_SLC_BASE + 0xD8)
/* SLC_WR_RETRY_THRESHOLD : R/W ;bitpos:[10:0] ;default: 11'h80 ; */
/*description: */
#define SLC_WR_RETRY_THRESHOLD  0x000007FF
#define SLC_WR_RETRY_THRESHOLD_M  ((SLC_WR_RETRY_THRESHOLD_V)<<(SLC_WR_RETRY_THRESHOLD_S))
#define SLC_WR_RETRY_THRESHOLD_V  0x7FF
#define SLC_WR_RETRY_THRESHOLD_S  0

#define SLC_CMD_INFOR0_REG          (DR_REG_SLC_BASE + 0xDC)
/* SLC_CMD_CONTENT0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_CMD_CONTENT0  0xFFFFFFFF
#define SLC_CMD_CONTENT0_M  ((SLC_CMD_CONTENT0_V)<<(SLC_CMD_CONTENT0_S))
#define SLC_CMD_CONTENT0_V  0xFFFFFFFF
#define SLC_CMD_CONTENT0_S  0

#define SLC_CMD_INFOR1_REG          (DR_REG_SLC_BASE + 0xE0)
/* SLC_CMD_CONTENT1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_CMD_CONTENT1  0xFFFFFFFF
#define SLC_CMD_CONTENT1_M  ((SLC_CMD_CONTENT1_V)<<(SLC_CMD_CONTENT1_S))
#define SLC_CMD_CONTENT1_V  0xFFFFFFFF
#define SLC_CMD_CONTENT1_S  0

#define SLC_0_LEN_CONF_REG          (DR_REG_SLC_BASE + 0xE4)
/* SLC_SLC0_TX_NEW_PKT_IND : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_NEW_PKT_IND  (BIT(28))
#define SLC_SLC0_TX_NEW_PKT_IND_M  (BIT(28))
#define SLC_SLC0_TX_NEW_PKT_IND_V  0x1
#define SLC_SLC0_TX_NEW_PKT_IND_S  28
/* SLC_SLC0_RX_NEW_PKT_IND : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_NEW_PKT_IND  (BIT(27))
#define SLC_SLC0_RX_NEW_PKT_IND_M  (BIT(27))
#define SLC_SLC0_RX_NEW_PKT_IND_V  0x1
#define SLC_SLC0_RX_NEW_PKT_IND_S  27
/* SLC_SLC0_TX_GET_USED_DSCR : WO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_GET_USED_DSCR  (BIT(26))
#define SLC_SLC0_TX_GET_USED_DSCR_M  (BIT(26))
#define SLC_SLC0_TX_GET_USED_DSCR_V  0x1
#define SLC_SLC0_TX_GET_USED_DSCR_S  26
/* SLC_SLC0_RX_GET_USED_DSCR : WO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_GET_USED_DSCR  (BIT(25))
#define SLC_SLC0_RX_GET_USED_DSCR_M  (BIT(25))
#define SLC_SLC0_RX_GET_USED_DSCR_V  0x1
#define SLC_SLC0_RX_GET_USED_DSCR_S  25
/* SLC_SLC0_TX_PACKET_LOAD_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_PACKET_LOAD_EN  (BIT(24))
#define SLC_SLC0_TX_PACKET_LOAD_EN_M  (BIT(24))
#define SLC_SLC0_TX_PACKET_LOAD_EN_V  0x1
#define SLC_SLC0_TX_PACKET_LOAD_EN_S  24
/* SLC_SLC0_RX_PACKET_LOAD_EN : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_PACKET_LOAD_EN  (BIT(23))
#define SLC_SLC0_RX_PACKET_LOAD_EN_M  (BIT(23))
#define SLC_SLC0_RX_PACKET_LOAD_EN_V  0x1
#define SLC_SLC0_RX_PACKET_LOAD_EN_S  23
/* SLC_SLC0_LEN_INC_MORE : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_LEN_INC_MORE  (BIT(22))
#define SLC_SLC0_LEN_INC_MORE_M  (BIT(22))
#define SLC_SLC0_LEN_INC_MORE_V  0x1
#define SLC_SLC0_LEN_INC_MORE_S  22
/* SLC_SLC0_LEN_INC : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_LEN_INC  (BIT(21))
#define SLC_SLC0_LEN_INC_M  (BIT(21))
#define SLC_SLC0_LEN_INC_V  0x1
#define SLC_SLC0_LEN_INC_S  21
/* SLC_SLC0_LEN_WR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_LEN_WR  (BIT(20))
#define SLC_SLC0_LEN_WR_M  (BIT(20))
#define SLC_SLC0_LEN_WR_V  0x1
#define SLC_SLC0_LEN_WR_S  20
/* SLC_SLC0_LEN_WDATA : WO ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define SLC_SLC0_LEN_WDATA  0x000FFFFF
#define SLC_SLC0_LEN_WDATA_M  ((SLC_SLC0_LEN_WDATA_V)<<(SLC_SLC0_LEN_WDATA_S))
#define SLC_SLC0_LEN_WDATA_V  0xFFFFF
#define SLC_SLC0_LEN_WDATA_S  0

#define SLC_0_LENGTH_REG          (DR_REG_SLC_BASE + 0xE8)
/* SLC_SLC0_LEN : RO ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define SLC_SLC0_LEN  0x000FFFFF
#define SLC_SLC0_LEN_M  ((SLC_SLC0_LEN_V)<<(SLC_SLC0_LEN_S))
#define SLC_SLC0_LEN_V  0xFFFFF
#define SLC_SLC0_LEN_S  0

#define SLC_0_TXPKT_H_DSCR_REG          (DR_REG_SLC_BASE + 0xEC)
/* SLC_SLC0_TX_PKT_H_DSCR_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TX_PKT_H_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_H_DSCR_ADDR_M  ((SLC_SLC0_TX_PKT_H_DSCR_ADDR_V)<<(SLC_SLC0_TX_PKT_H_DSCR_ADDR_S))
#define SLC_SLC0_TX_PKT_H_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_H_DSCR_ADDR_S  0

#define SLC_0_TXPKT_E_DSCR_REG          (DR_REG_SLC_BASE + 0xF0)
/* SLC_SLC0_TX_PKT_E_DSCR_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TX_PKT_E_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_E_DSCR_ADDR_M  ((SLC_SLC0_TX_PKT_E_DSCR_ADDR_V)<<(SLC_SLC0_TX_PKT_E_DSCR_ADDR_S))
#define SLC_SLC0_TX_PKT_E_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_E_DSCR_ADDR_S  0

#define SLC_0_RXPKT_H_DSCR_REG          (DR_REG_SLC_BASE + 0xF4)
/* SLC_SLC0_RX_PKT_H_DSCR_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_RX_PKT_H_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_H_DSCR_ADDR_M  ((SLC_SLC0_RX_PKT_H_DSCR_ADDR_V)<<(SLC_SLC0_RX_PKT_H_DSCR_ADDR_S))
#define SLC_SLC0_RX_PKT_H_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_H_DSCR_ADDR_S  0

#define SLC_0_RXPKT_E_DSCR_REG          (DR_REG_SLC_BASE + 0xF8)
/* SLC_SLC0_RX_PKT_E_DSCR_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_RX_PKT_E_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_E_DSCR_ADDR_M  ((SLC_SLC0_RX_PKT_E_DSCR_ADDR_V)<<(SLC_SLC0_RX_PKT_E_DSCR_ADDR_S))
#define SLC_SLC0_RX_PKT_E_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_E_DSCR_ADDR_S  0

#define SLC_0_TXPKTU_H_DSCR_REG          (DR_REG_SLC_BASE + 0xFC)
/* SLC_SLC0_TX_PKT_START_DSCR_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TX_PKT_START_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_START_DSCR_ADDR_M  ((SLC_SLC0_TX_PKT_START_DSCR_ADDR_V)<<(SLC_SLC0_TX_PKT_START_DSCR_ADDR_S))
#define SLC_SLC0_TX_PKT_START_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_START_DSCR_ADDR_S  0

#define SLC_0_TXPKTU_E_DSCR_REG          (DR_REG_SLC_BASE + 0x100)
/* SLC_SLC0_TX_PKT_END_DSCR_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_TX_PKT_END_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_END_DSCR_ADDR_M  ((SLC_SLC0_TX_PKT_END_DSCR_ADDR_V)<<(SLC_SLC0_TX_PKT_END_DSCR_ADDR_S))
#define SLC_SLC0_TX_PKT_END_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_TX_PKT_END_DSCR_ADDR_S  0

#define SLC_0_RXPKTU_H_DSCR_REG          (DR_REG_SLC_BASE + 0x104)
/* SLC_SLC0_RX_PKT_START_DSCR_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_RX_PKT_START_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_START_DSCR_ADDR_M  ((SLC_SLC0_RX_PKT_START_DSCR_ADDR_V)<<(SLC_SLC0_RX_PKT_START_DSCR_ADDR_S))
#define SLC_SLC0_RX_PKT_START_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_START_DSCR_ADDR_S  0

#define SLC_0_RXPKTU_E_DSCR_REG          (DR_REG_SLC_BASE + 0x108)
/* SLC_SLC0_RX_PKT_END_DSCR_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SLC_SLC0_RX_PKT_END_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_END_DSCR_ADDR_M  ((SLC_SLC0_RX_PKT_END_DSCR_ADDR_V)<<(SLC_SLC0_RX_PKT_END_DSCR_ADDR_S))
#define SLC_SLC0_RX_PKT_END_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_RX_PKT_END_DSCR_ADDR_S  0

#define SLC_SEQ_POSITION_REG          (DR_REG_SLC_BASE + 0x114)
/* SLC_SLC1_SEQ_POSITION : R/W ;bitpos:[15:8] ;default: 8'h5 ; */
/*description: */
#define SLC_SLC1_SEQ_POSITION  0x000000FF
#define SLC_SLC1_SEQ_POSITION_M  ((SLC_SLC1_SEQ_POSITION_V)<<(SLC_SLC1_SEQ_POSITION_S))
#define SLC_SLC1_SEQ_POSITION_V  0xFF
#define SLC_SLC1_SEQ_POSITION_S  8
/* SLC_SLC0_SEQ_POSITION : R/W ;bitpos:[7:0] ;default: 8'h9 ; */
/*description: */
#define SLC_SLC0_SEQ_POSITION  0x000000FF
#define SLC_SLC0_SEQ_POSITION_M  ((SLC_SLC0_SEQ_POSITION_V)<<(SLC_SLC0_SEQ_POSITION_S))
#define SLC_SLC0_SEQ_POSITION_V  0xFF
#define SLC_SLC0_SEQ_POSITION_S  0

#define SLC_0_DSCR_REC_CONF_REG          (DR_REG_SLC_BASE + 0x118)
/* SLC_SLC0_RX_DSCR_REC_LIM : R/W ;bitpos:[9:0] ;default: 10'h3ff ; */
/*description: */
#define SLC_SLC0_RX_DSCR_REC_LIM  0x000003FF
#define SLC_SLC0_RX_DSCR_REC_LIM_M  ((SLC_SLC0_RX_DSCR_REC_LIM_V)<<(SLC_SLC0_RX_DSCR_REC_LIM_S))
#define SLC_SLC0_RX_DSCR_REC_LIM_V  0x3FF
#define SLC_SLC0_RX_DSCR_REC_LIM_S  0

#define SLC_SDIO_CRC_ST0_REG          (DR_REG_SLC_BASE + 0x11C)
/* SLC_DAT3_CRC_ERR_CNT : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define SLC_DAT3_CRC_ERR_CNT  0x000000FF
#define SLC_DAT3_CRC_ERR_CNT_M  ((SLC_DAT3_CRC_ERR_CNT_V)<<(SLC_DAT3_CRC_ERR_CNT_S))
#define SLC_DAT3_CRC_ERR_CNT_V  0xFF
#define SLC_DAT3_CRC_ERR_CNT_S  24
/* SLC_DAT2_CRC_ERR_CNT : RO ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define SLC_DAT2_CRC_ERR_CNT  0x000000FF
#define SLC_DAT2_CRC_ERR_CNT_M  ((SLC_DAT2_CRC_ERR_CNT_V)<<(SLC_DAT2_CRC_ERR_CNT_S))
#define SLC_DAT2_CRC_ERR_CNT_V  0xFF
#define SLC_DAT2_CRC_ERR_CNT_S  16
/* SLC_DAT1_CRC_ERR_CNT : RO ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define SLC_DAT1_CRC_ERR_CNT  0x000000FF
#define SLC_DAT1_CRC_ERR_CNT_M  ((SLC_DAT1_CRC_ERR_CNT_V)<<(SLC_DAT1_CRC_ERR_CNT_S))
#define SLC_DAT1_CRC_ERR_CNT_V  0xFF
#define SLC_DAT1_CRC_ERR_CNT_S  8
/* SLC_DAT0_CRC_ERR_CNT : RO ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define SLC_DAT0_CRC_ERR_CNT  0x000000FF
#define SLC_DAT0_CRC_ERR_CNT_M  ((SLC_DAT0_CRC_ERR_CNT_V)<<(SLC_DAT0_CRC_ERR_CNT_S))
#define SLC_DAT0_CRC_ERR_CNT_V  0xFF
#define SLC_DAT0_CRC_ERR_CNT_S  0

#define SLC_SDIO_CRC_ST1_REG          (DR_REG_SLC_BASE + 0x120)
/* SLC_ERR_CNT_CLR : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define SLC_ERR_CNT_CLR  (BIT(31))
#define SLC_ERR_CNT_CLR_M  (BIT(31))
#define SLC_ERR_CNT_CLR_V  0x1
#define SLC_ERR_CNT_CLR_S  31
/* SLC_CMD_CRC_ERR_CNT : RO ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define SLC_CMD_CRC_ERR_CNT  0x000000FF
#define SLC_CMD_CRC_ERR_CNT_M  ((SLC_CMD_CRC_ERR_CNT_V)<<(SLC_CMD_CRC_ERR_CNT_S))
#define SLC_CMD_CRC_ERR_CNT_V  0xFF
#define SLC_CMD_CRC_ERR_CNT_S  0

#define SLC_0_EOF_START_DES_REG          (DR_REG_SLC_BASE + 0x124)
/* SLC_SLC0_EOF_START_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_EOF_START_DES_ADDR  0xFFFFFFFF
#define SLC_SLC0_EOF_START_DES_ADDR_M  ((SLC_SLC0_EOF_START_DES_ADDR_V)<<(SLC_SLC0_EOF_START_DES_ADDR_S))
#define SLC_SLC0_EOF_START_DES_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_EOF_START_DES_ADDR_S  0

#define SLC_0_PUSH_DSCR_ADDR_REG          (DR_REG_SLC_BASE + 0x128)
/* SLC_SLC0_RX_PUSH_DSCR_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_RX_PUSH_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_RX_PUSH_DSCR_ADDR_M  ((SLC_SLC0_RX_PUSH_DSCR_ADDR_V)<<(SLC_SLC0_RX_PUSH_DSCR_ADDR_S))
#define SLC_SLC0_RX_PUSH_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_RX_PUSH_DSCR_ADDR_S  0

#define SLC_0_DONE_DSCR_ADDR_REG          (DR_REG_SLC_BASE + 0x12C)
/* SLC_SLC0_RX_DONE_DSCR_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_RX_DONE_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_RX_DONE_DSCR_ADDR_M  ((SLC_SLC0_RX_DONE_DSCR_ADDR_V)<<(SLC_SLC0_RX_DONE_DSCR_ADDR_S))
#define SLC_SLC0_RX_DONE_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_RX_DONE_DSCR_ADDR_S  0

#define SLC_0_SUB_START_DES_REG          (DR_REG_SLC_BASE + 0x130)
/* SLC_SLC0_SUB_PAC_START_DSCR_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SLC_SLC0_SUB_PAC_START_DSCR_ADDR  0xFFFFFFFF
#define SLC_SLC0_SUB_PAC_START_DSCR_ADDR_M  ((SLC_SLC0_SUB_PAC_START_DSCR_ADDR_V)<<(SLC_SLC0_SUB_PAC_START_DSCR_ADDR_S))
#define SLC_SLC0_SUB_PAC_START_DSCR_ADDR_V  0xFFFFFFFF
#define SLC_SLC0_SUB_PAC_START_DSCR_ADDR_S  0

#define SLC_0_DSCR_CNT_REG          (DR_REG_SLC_BASE + 0x134)
/* SLC_SLC0_RX_GET_EOF_OCC : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_GET_EOF_OCC  (BIT(16))
#define SLC_SLC0_RX_GET_EOF_OCC_M  (BIT(16))
#define SLC_SLC0_RX_GET_EOF_OCC_V  0x1
#define SLC_SLC0_RX_GET_EOF_OCC_S  16
/* SLC_SLC0_RX_DSCR_CNT_LAT : RO ;bitpos:[9:0] ;default: 10'b0 ; */
/*description: */
#define SLC_SLC0_RX_DSCR_CNT_LAT  0x000003FF
#define SLC_SLC0_RX_DSCR_CNT_LAT_M  ((SLC_SLC0_RX_DSCR_CNT_LAT_V)<<(SLC_SLC0_RX_DSCR_CNT_LAT_S))
#define SLC_SLC0_RX_DSCR_CNT_LAT_V  0x3FF
#define SLC_SLC0_RX_DSCR_CNT_LAT_S  0

#define SLC_0_LEN_LIM_CONF_REG          (DR_REG_SLC_BASE + 0x138)
/* SLC_SLC0_LEN_LIM : R/W ;bitpos:[19:0] ;default: 20'h5400 ; */
/*description: */
#define SLC_SLC0_LEN_LIM  0x000FFFFF
#define SLC_SLC0_LEN_LIM_M  ((SLC_SLC0_LEN_LIM_V)<<(SLC_SLC0_LEN_LIM_S))
#define SLC_SLC0_LEN_LIM_V  0xFFFFF
#define SLC_SLC0_LEN_LIM_S  0

#define SLC_0INT_ST1_REG          (DR_REG_SLC_BASE + 0x13C)
/* SLC_SLC0_RX_QUICK_EOF_INT_ST1 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_QUICK_EOF_INT_ST1  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ST1_M  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ST1_V  0x1
#define SLC_SLC0_RX_QUICK_EOF_INT_ST1_S  26
/* SLC_CMD_DTC_INT_ST1 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define SLC_CMD_DTC_INT_ST1  (BIT(25))
#define SLC_CMD_DTC_INT_ST1_M  (BIT(25))
#define SLC_CMD_DTC_INT_ST1_V  0x1
#define SLC_CMD_DTC_INT_ST1_S  25
/* SLC_SLC0_TX_ERR_EOF_INT_ST1 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_ERR_EOF_INT_ST1  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ST1_M  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ST1_V  0x1
#define SLC_SLC0_TX_ERR_EOF_INT_ST1_S  24
/* SLC_SLC0_WR_RETRY_DONE_INT_ST1 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_WR_RETRY_DONE_INT_ST1  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ST1_M  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ST1_V  0x1
#define SLC_SLC0_WR_RETRY_DONE_INT_ST1_S  23
/* SLC_SLC0_HOST_RD_ACK_INT_ST1 : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_HOST_RD_ACK_INT_ST1  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ST1_M  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ST1_V  0x1
#define SLC_SLC0_HOST_RD_ACK_INT_ST1_S  22
/* SLC_SLC0_TX_DSCR_EMPTY_INT_ST1 : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST1  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST1_M  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST1_V  0x1
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ST1_S  21
/* SLC_SLC0_RX_DSCR_ERR_INT_ST1 : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DSCR_ERR_INT_ST1  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ST1_M  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ST1_V  0x1
#define SLC_SLC0_RX_DSCR_ERR_INT_ST1_S  20
/* SLC_SLC0_TX_DSCR_ERR_INT_ST1 : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_ERR_INT_ST1  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ST1_M  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ST1_V  0x1
#define SLC_SLC0_TX_DSCR_ERR_INT_ST1_S  19
/* SLC_SLC0_TOHOST_INT_ST1 : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOHOST_INT_ST1  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ST1_M  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ST1_V  0x1
#define SLC_SLC0_TOHOST_INT_ST1_S  18
/* SLC_SLC0_RX_EOF_INT_ST1 : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_EOF_INT_ST1  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ST1_M  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ST1_V  0x1
#define SLC_SLC0_RX_EOF_INT_ST1_S  17
/* SLC_SLC0_RX_DONE_INT_ST1 : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DONE_INT_ST1  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ST1_M  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ST1_V  0x1
#define SLC_SLC0_RX_DONE_INT_ST1_S  16
/* SLC_SLC0_TX_SUC_EOF_INT_ST1 : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_SUC_EOF_INT_ST1  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ST1_M  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ST1_V  0x1
#define SLC_SLC0_TX_SUC_EOF_INT_ST1_S  15
/* SLC_SLC0_TX_DONE_INT_ST1 : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DONE_INT_ST1  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ST1_M  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ST1_V  0x1
#define SLC_SLC0_TX_DONE_INT_ST1_S  14
/* SLC_SLC0_TOKEN1_1TO0_INT_ST1 : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_1TO0_INT_ST1  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ST1_M  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ST1_V  0x1
#define SLC_SLC0_TOKEN1_1TO0_INT_ST1_S  13
/* SLC_SLC0_TOKEN0_1TO0_INT_ST1 : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_1TO0_INT_ST1  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ST1_M  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ST1_V  0x1
#define SLC_SLC0_TOKEN0_1TO0_INT_ST1_S  12
/* SLC_SLC0_TX_OVF_INT_ST1 : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_OVF_INT_ST1  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ST1_M  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ST1_V  0x1
#define SLC_SLC0_TX_OVF_INT_ST1_S  11
/* SLC_SLC0_RX_UDF_INT_ST1 : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_UDF_INT_ST1  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ST1_M  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ST1_V  0x1
#define SLC_SLC0_RX_UDF_INT_ST1_S  10
/* SLC_SLC0_TX_START_INT_ST1 : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_START_INT_ST1  (BIT(9))
#define SLC_SLC0_TX_START_INT_ST1_M  (BIT(9))
#define SLC_SLC0_TX_START_INT_ST1_V  0x1
#define SLC_SLC0_TX_START_INT_ST1_S  9
/* SLC_SLC0_RX_START_INT_ST1 : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_START_INT_ST1  (BIT(8))
#define SLC_SLC0_RX_START_INT_ST1_M  (BIT(8))
#define SLC_SLC0_RX_START_INT_ST1_V  0x1
#define SLC_SLC0_RX_START_INT_ST1_S  8
/* SLC_FRHOST_BIT7_INT_ST1 : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT7_INT_ST1  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ST1_M  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ST1_V  0x1
#define SLC_FRHOST_BIT7_INT_ST1_S  7
/* SLC_FRHOST_BIT6_INT_ST1 : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT6_INT_ST1  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ST1_M  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ST1_V  0x1
#define SLC_FRHOST_BIT6_INT_ST1_S  6
/* SLC_FRHOST_BIT5_INT_ST1 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT5_INT_ST1  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ST1_M  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ST1_V  0x1
#define SLC_FRHOST_BIT5_INT_ST1_S  5
/* SLC_FRHOST_BIT4_INT_ST1 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT4_INT_ST1  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ST1_M  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ST1_V  0x1
#define SLC_FRHOST_BIT4_INT_ST1_S  4
/* SLC_FRHOST_BIT3_INT_ST1 : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT3_INT_ST1  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ST1_M  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ST1_V  0x1
#define SLC_FRHOST_BIT3_INT_ST1_S  3
/* SLC_FRHOST_BIT2_INT_ST1 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT2_INT_ST1  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ST1_M  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ST1_V  0x1
#define SLC_FRHOST_BIT2_INT_ST1_S  2
/* SLC_FRHOST_BIT1_INT_ST1 : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT1_INT_ST1  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ST1_M  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ST1_V  0x1
#define SLC_FRHOST_BIT1_INT_ST1_S  1
/* SLC_FRHOST_BIT0_INT_ST1 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT0_INT_ST1  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ST1_M  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ST1_V  0x1
#define SLC_FRHOST_BIT0_INT_ST1_S  0

#define SLC_0INT_ENA1_REG          (DR_REG_SLC_BASE + 0x140)
/* SLC_SLC0_RX_QUICK_EOF_INT_ENA1 : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA1  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA1_M  (BIT(26))
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA1_V  0x1
#define SLC_SLC0_RX_QUICK_EOF_INT_ENA1_S  26
/* SLC_CMD_DTC_INT_ENA1 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define SLC_CMD_DTC_INT_ENA1  (BIT(25))
#define SLC_CMD_DTC_INT_ENA1_M  (BIT(25))
#define SLC_CMD_DTC_INT_ENA1_V  0x1
#define SLC_CMD_DTC_INT_ENA1_S  25
/* SLC_SLC0_TX_ERR_EOF_INT_ENA1 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_ERR_EOF_INT_ENA1  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ENA1_M  (BIT(24))
#define SLC_SLC0_TX_ERR_EOF_INT_ENA1_V  0x1
#define SLC_SLC0_TX_ERR_EOF_INT_ENA1_S  24
/* SLC_SLC0_WR_RETRY_DONE_INT_ENA1 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA1  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA1_M  (BIT(23))
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA1_V  0x1
#define SLC_SLC0_WR_RETRY_DONE_INT_ENA1_S  23
/* SLC_SLC0_HOST_RD_ACK_INT_ENA1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_HOST_RD_ACK_INT_ENA1  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ENA1_M  (BIT(22))
#define SLC_SLC0_HOST_RD_ACK_INT_ENA1_V  0x1
#define SLC_SLC0_HOST_RD_ACK_INT_ENA1_S  22
/* SLC_SLC0_TX_DSCR_EMPTY_INT_ENA1 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA1  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA1_M  (BIT(21))
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA1_V  0x1
#define SLC_SLC0_TX_DSCR_EMPTY_INT_ENA1_S  21
/* SLC_SLC0_RX_DSCR_ERR_INT_ENA1 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA1  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA1_M  (BIT(20))
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA1_V  0x1
#define SLC_SLC0_RX_DSCR_ERR_INT_ENA1_S  20
/* SLC_SLC0_TX_DSCR_ERR_INT_ENA1 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA1  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA1_M  (BIT(19))
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA1_V  0x1
#define SLC_SLC0_TX_DSCR_ERR_INT_ENA1_S  19
/* SLC_SLC0_TOHOST_INT_ENA1 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOHOST_INT_ENA1  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ENA1_M  (BIT(18))
#define SLC_SLC0_TOHOST_INT_ENA1_V  0x1
#define SLC_SLC0_TOHOST_INT_ENA1_S  18
/* SLC_SLC0_RX_EOF_INT_ENA1 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_EOF_INT_ENA1  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ENA1_M  (BIT(17))
#define SLC_SLC0_RX_EOF_INT_ENA1_V  0x1
#define SLC_SLC0_RX_EOF_INT_ENA1_S  17
/* SLC_SLC0_RX_DONE_INT_ENA1 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_DONE_INT_ENA1  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ENA1_M  (BIT(16))
#define SLC_SLC0_RX_DONE_INT_ENA1_V  0x1
#define SLC_SLC0_RX_DONE_INT_ENA1_S  16
/* SLC_SLC0_TX_SUC_EOF_INT_ENA1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_SUC_EOF_INT_ENA1  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ENA1_M  (BIT(15))
#define SLC_SLC0_TX_SUC_EOF_INT_ENA1_V  0x1
#define SLC_SLC0_TX_SUC_EOF_INT_ENA1_S  15
/* SLC_SLC0_TX_DONE_INT_ENA1 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_DONE_INT_ENA1  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ENA1_M  (BIT(14))
#define SLC_SLC0_TX_DONE_INT_ENA1_V  0x1
#define SLC_SLC0_TX_DONE_INT_ENA1_S  14
/* SLC_SLC0_TOKEN1_1TO0_INT_ENA1 : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA1  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA1_M  (BIT(13))
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA1_V  0x1
#define SLC_SLC0_TOKEN1_1TO0_INT_ENA1_S  13
/* SLC_SLC0_TOKEN0_1TO0_INT_ENA1 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA1  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA1_M  (BIT(12))
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA1_V  0x1
#define SLC_SLC0_TOKEN0_1TO0_INT_ENA1_S  12
/* SLC_SLC0_TX_OVF_INT_ENA1 : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_OVF_INT_ENA1  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ENA1_M  (BIT(11))
#define SLC_SLC0_TX_OVF_INT_ENA1_V  0x1
#define SLC_SLC0_TX_OVF_INT_ENA1_S  11
/* SLC_SLC0_RX_UDF_INT_ENA1 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_UDF_INT_ENA1  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ENA1_M  (BIT(10))
#define SLC_SLC0_RX_UDF_INT_ENA1_V  0x1
#define SLC_SLC0_RX_UDF_INT_ENA1_S  10
/* SLC_SLC0_TX_START_INT_ENA1 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_TX_START_INT_ENA1  (BIT(9))
#define SLC_SLC0_TX_START_INT_ENA1_M  (BIT(9))
#define SLC_SLC0_TX_START_INT_ENA1_V  0x1
#define SLC_SLC0_TX_START_INT_ENA1_S  9
/* SLC_SLC0_RX_START_INT_ENA1 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC0_RX_START_INT_ENA1  (BIT(8))
#define SLC_SLC0_RX_START_INT_ENA1_M  (BIT(8))
#define SLC_SLC0_RX_START_INT_ENA1_V  0x1
#define SLC_SLC0_RX_START_INT_ENA1_S  8
/* SLC_FRHOST_BIT7_INT_ENA1 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT7_INT_ENA1  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ENA1_M  (BIT(7))
#define SLC_FRHOST_BIT7_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT7_INT_ENA1_S  7
/* SLC_FRHOST_BIT6_INT_ENA1 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT6_INT_ENA1  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ENA1_M  (BIT(6))
#define SLC_FRHOST_BIT6_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT6_INT_ENA1_S  6
/* SLC_FRHOST_BIT5_INT_ENA1 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT5_INT_ENA1  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ENA1_M  (BIT(5))
#define SLC_FRHOST_BIT5_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT5_INT_ENA1_S  5
/* SLC_FRHOST_BIT4_INT_ENA1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT4_INT_ENA1  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ENA1_M  (BIT(4))
#define SLC_FRHOST_BIT4_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT4_INT_ENA1_S  4
/* SLC_FRHOST_BIT3_INT_ENA1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT3_INT_ENA1  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ENA1_M  (BIT(3))
#define SLC_FRHOST_BIT3_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT3_INT_ENA1_S  3
/* SLC_FRHOST_BIT2_INT_ENA1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT2_INT_ENA1  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ENA1_M  (BIT(2))
#define SLC_FRHOST_BIT2_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT2_INT_ENA1_S  2
/* SLC_FRHOST_BIT1_INT_ENA1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT1_INT_ENA1  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ENA1_M  (BIT(1))
#define SLC_FRHOST_BIT1_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT1_INT_ENA1_S  1
/* SLC_FRHOST_BIT0_INT_ENA1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT0_INT_ENA1  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ENA1_M  (BIT(0))
#define SLC_FRHOST_BIT0_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT0_INT_ENA1_S  0

#define SLC_1INT_ST1_REG          (DR_REG_SLC_BASE + 0x144)
/* SLC_SLC1_TX_ERR_EOF_INT_ST1 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_ERR_EOF_INT_ST1  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ST1_M  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ST1_V  0x1
#define SLC_SLC1_TX_ERR_EOF_INT_ST1_S  24
/* SLC_SLC1_WR_RETRY_DONE_INT_ST1 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_WR_RETRY_DONE_INT_ST1  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ST1_M  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ST1_V  0x1
#define SLC_SLC1_WR_RETRY_DONE_INT_ST1_S  23
/* SLC_SLC1_HOST_RD_ACK_INT_ST1 : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_HOST_RD_ACK_INT_ST1  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ST1_M  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ST1_V  0x1
#define SLC_SLC1_HOST_RD_ACK_INT_ST1_S  22
/* SLC_SLC1_TX_DSCR_EMPTY_INT_ST1 : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST1  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST1_M  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST1_V  0x1
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ST1_S  21
/* SLC_SLC1_RX_DSCR_ERR_INT_ST1 : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DSCR_ERR_INT_ST1  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ST1_M  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ST1_V  0x1
#define SLC_SLC1_RX_DSCR_ERR_INT_ST1_S  20
/* SLC_SLC1_TX_DSCR_ERR_INT_ST1 : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_ERR_INT_ST1  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ST1_M  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ST1_V  0x1
#define SLC_SLC1_TX_DSCR_ERR_INT_ST1_S  19
/* SLC_SLC1_TOHOST_INT_ST1 : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOHOST_INT_ST1  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ST1_M  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ST1_V  0x1
#define SLC_SLC1_TOHOST_INT_ST1_S  18
/* SLC_SLC1_RX_EOF_INT_ST1 : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_EOF_INT_ST1  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ST1_M  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ST1_V  0x1
#define SLC_SLC1_RX_EOF_INT_ST1_S  17
/* SLC_SLC1_RX_DONE_INT_ST1 : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DONE_INT_ST1  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ST1_M  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ST1_V  0x1
#define SLC_SLC1_RX_DONE_INT_ST1_S  16
/* SLC_SLC1_TX_SUC_EOF_INT_ST1 : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_SUC_EOF_INT_ST1  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ST1_M  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ST1_V  0x1
#define SLC_SLC1_TX_SUC_EOF_INT_ST1_S  15
/* SLC_SLC1_TX_DONE_INT_ST1 : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DONE_INT_ST1  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ST1_M  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ST1_V  0x1
#define SLC_SLC1_TX_DONE_INT_ST1_S  14
/* SLC_SLC1_TOKEN1_1TO0_INT_ST1 : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_1TO0_INT_ST1  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ST1_M  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ST1_V  0x1
#define SLC_SLC1_TOKEN1_1TO0_INT_ST1_S  13
/* SLC_SLC1_TOKEN0_1TO0_INT_ST1 : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_1TO0_INT_ST1  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ST1_M  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ST1_V  0x1
#define SLC_SLC1_TOKEN0_1TO0_INT_ST1_S  12
/* SLC_SLC1_TX_OVF_INT_ST1 : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_OVF_INT_ST1  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ST1_M  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ST1_V  0x1
#define SLC_SLC1_TX_OVF_INT_ST1_S  11
/* SLC_SLC1_RX_UDF_INT_ST1 : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_UDF_INT_ST1  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ST1_M  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ST1_V  0x1
#define SLC_SLC1_RX_UDF_INT_ST1_S  10
/* SLC_SLC1_TX_START_INT_ST1 : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_START_INT_ST1  (BIT(9))
#define SLC_SLC1_TX_START_INT_ST1_M  (BIT(9))
#define SLC_SLC1_TX_START_INT_ST1_V  0x1
#define SLC_SLC1_TX_START_INT_ST1_S  9
/* SLC_SLC1_RX_START_INT_ST1 : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_START_INT_ST1  (BIT(8))
#define SLC_SLC1_RX_START_INT_ST1_M  (BIT(8))
#define SLC_SLC1_RX_START_INT_ST1_V  0x1
#define SLC_SLC1_RX_START_INT_ST1_S  8
/* SLC_FRHOST_BIT15_INT_ST1 : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT15_INT_ST1  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ST1_M  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ST1_V  0x1
#define SLC_FRHOST_BIT15_INT_ST1_S  7
/* SLC_FRHOST_BIT14_INT_ST1 : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT14_INT_ST1  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ST1_M  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ST1_V  0x1
#define SLC_FRHOST_BIT14_INT_ST1_S  6
/* SLC_FRHOST_BIT13_INT_ST1 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT13_INT_ST1  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ST1_M  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ST1_V  0x1
#define SLC_FRHOST_BIT13_INT_ST1_S  5
/* SLC_FRHOST_BIT12_INT_ST1 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT12_INT_ST1  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ST1_M  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ST1_V  0x1
#define SLC_FRHOST_BIT12_INT_ST1_S  4
/* SLC_FRHOST_BIT11_INT_ST1 : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT11_INT_ST1  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ST1_M  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ST1_V  0x1
#define SLC_FRHOST_BIT11_INT_ST1_S  3
/* SLC_FRHOST_BIT10_INT_ST1 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT10_INT_ST1  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ST1_M  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ST1_V  0x1
#define SLC_FRHOST_BIT10_INT_ST1_S  2
/* SLC_FRHOST_BIT9_INT_ST1 : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT9_INT_ST1  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ST1_M  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ST1_V  0x1
#define SLC_FRHOST_BIT9_INT_ST1_S  1
/* SLC_FRHOST_BIT8_INT_ST1 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT8_INT_ST1  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ST1_M  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ST1_V  0x1
#define SLC_FRHOST_BIT8_INT_ST1_S  0

#define SLC_1INT_ENA1_REG          (DR_REG_SLC_BASE + 0x148)
/* SLC_SLC1_TX_ERR_EOF_INT_ENA1 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_ERR_EOF_INT_ENA1  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ENA1_M  (BIT(24))
#define SLC_SLC1_TX_ERR_EOF_INT_ENA1_V  0x1
#define SLC_SLC1_TX_ERR_EOF_INT_ENA1_S  24
/* SLC_SLC1_WR_RETRY_DONE_INT_ENA1 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA1  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA1_M  (BIT(23))
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA1_V  0x1
#define SLC_SLC1_WR_RETRY_DONE_INT_ENA1_S  23
/* SLC_SLC1_HOST_RD_ACK_INT_ENA1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_HOST_RD_ACK_INT_ENA1  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ENA1_M  (BIT(22))
#define SLC_SLC1_HOST_RD_ACK_INT_ENA1_V  0x1
#define SLC_SLC1_HOST_RD_ACK_INT_ENA1_S  22
/* SLC_SLC1_TX_DSCR_EMPTY_INT_ENA1 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA1  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA1_M  (BIT(21))
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA1_V  0x1
#define SLC_SLC1_TX_DSCR_EMPTY_INT_ENA1_S  21
/* SLC_SLC1_RX_DSCR_ERR_INT_ENA1 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA1  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA1_M  (BIT(20))
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA1_V  0x1
#define SLC_SLC1_RX_DSCR_ERR_INT_ENA1_S  20
/* SLC_SLC1_TX_DSCR_ERR_INT_ENA1 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA1  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA1_M  (BIT(19))
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA1_V  0x1
#define SLC_SLC1_TX_DSCR_ERR_INT_ENA1_S  19
/* SLC_SLC1_TOHOST_INT_ENA1 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOHOST_INT_ENA1  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ENA1_M  (BIT(18))
#define SLC_SLC1_TOHOST_INT_ENA1_V  0x1
#define SLC_SLC1_TOHOST_INT_ENA1_S  18
/* SLC_SLC1_RX_EOF_INT_ENA1 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_EOF_INT_ENA1  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ENA1_M  (BIT(17))
#define SLC_SLC1_RX_EOF_INT_ENA1_V  0x1
#define SLC_SLC1_RX_EOF_INT_ENA1_S  17
/* SLC_SLC1_RX_DONE_INT_ENA1 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_DONE_INT_ENA1  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ENA1_M  (BIT(16))
#define SLC_SLC1_RX_DONE_INT_ENA1_V  0x1
#define SLC_SLC1_RX_DONE_INT_ENA1_S  16
/* SLC_SLC1_TX_SUC_EOF_INT_ENA1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_SUC_EOF_INT_ENA1  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ENA1_M  (BIT(15))
#define SLC_SLC1_TX_SUC_EOF_INT_ENA1_V  0x1
#define SLC_SLC1_TX_SUC_EOF_INT_ENA1_S  15
/* SLC_SLC1_TX_DONE_INT_ENA1 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_DONE_INT_ENA1  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ENA1_M  (BIT(14))
#define SLC_SLC1_TX_DONE_INT_ENA1_V  0x1
#define SLC_SLC1_TX_DONE_INT_ENA1_S  14
/* SLC_SLC1_TOKEN1_1TO0_INT_ENA1 : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA1  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA1_M  (BIT(13))
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA1_V  0x1
#define SLC_SLC1_TOKEN1_1TO0_INT_ENA1_S  13
/* SLC_SLC1_TOKEN0_1TO0_INT_ENA1 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA1  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA1_M  (BIT(12))
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA1_V  0x1
#define SLC_SLC1_TOKEN0_1TO0_INT_ENA1_S  12
/* SLC_SLC1_TX_OVF_INT_ENA1 : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_OVF_INT_ENA1  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ENA1_M  (BIT(11))
#define SLC_SLC1_TX_OVF_INT_ENA1_V  0x1
#define SLC_SLC1_TX_OVF_INT_ENA1_S  11
/* SLC_SLC1_RX_UDF_INT_ENA1 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_UDF_INT_ENA1  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ENA1_M  (BIT(10))
#define SLC_SLC1_RX_UDF_INT_ENA1_V  0x1
#define SLC_SLC1_RX_UDF_INT_ENA1_S  10
/* SLC_SLC1_TX_START_INT_ENA1 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_TX_START_INT_ENA1  (BIT(9))
#define SLC_SLC1_TX_START_INT_ENA1_M  (BIT(9))
#define SLC_SLC1_TX_START_INT_ENA1_V  0x1
#define SLC_SLC1_TX_START_INT_ENA1_S  9
/* SLC_SLC1_RX_START_INT_ENA1 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SLC_SLC1_RX_START_INT_ENA1  (BIT(8))
#define SLC_SLC1_RX_START_INT_ENA1_M  (BIT(8))
#define SLC_SLC1_RX_START_INT_ENA1_V  0x1
#define SLC_SLC1_RX_START_INT_ENA1_S  8
/* SLC_FRHOST_BIT15_INT_ENA1 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT15_INT_ENA1  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ENA1_M  (BIT(7))
#define SLC_FRHOST_BIT15_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT15_INT_ENA1_S  7
/* SLC_FRHOST_BIT14_INT_ENA1 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT14_INT_ENA1  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ENA1_M  (BIT(6))
#define SLC_FRHOST_BIT14_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT14_INT_ENA1_S  6
/* SLC_FRHOST_BIT13_INT_ENA1 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT13_INT_ENA1  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ENA1_M  (BIT(5))
#define SLC_FRHOST_BIT13_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT13_INT_ENA1_S  5
/* SLC_FRHOST_BIT12_INT_ENA1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT12_INT_ENA1  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ENA1_M  (BIT(4))
#define SLC_FRHOST_BIT12_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT12_INT_ENA1_S  4
/* SLC_FRHOST_BIT11_INT_ENA1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT11_INT_ENA1  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ENA1_M  (BIT(3))
#define SLC_FRHOST_BIT11_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT11_INT_ENA1_S  3
/* SLC_FRHOST_BIT10_INT_ENA1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT10_INT_ENA1  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ENA1_M  (BIT(2))
#define SLC_FRHOST_BIT10_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT10_INT_ENA1_S  2
/* SLC_FRHOST_BIT9_INT_ENA1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT9_INT_ENA1  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ENA1_M  (BIT(1))
#define SLC_FRHOST_BIT9_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT9_INT_ENA1_S  1
/* SLC_FRHOST_BIT8_INT_ENA1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SLC_FRHOST_BIT8_INT_ENA1  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ENA1_M  (BIT(0))
#define SLC_FRHOST_BIT8_INT_ENA1_V  0x1
#define SLC_FRHOST_BIT8_INT_ENA1_S  0

#define SLC_DATE_REG          (DR_REG_SLC_BASE + 0x1F8)
/* SLC_DATE : R/W ;bitpos:[31:0] ;default: 32'h16022500 ; */
/*description: */
#define SLC_DATE  0xFFFFFFFF
#define SLC_DATE_M  ((SLC_DATE_V)<<(SLC_DATE_S))
#define SLC_DATE_V  0xFFFFFFFF
#define SLC_DATE_S  0

#define SLC_ID_REG          (DR_REG_SLC_BASE + 0x1FC)
/* SLC_ID : R/W ;bitpos:[31:0] ;default: 32'h0100 ; */
/*description: */
#define SLC_ID  0xFFFFFFFF
#define SLC_ID_M  ((SLC_ID_V)<<(SLC_ID_S))
#define SLC_ID_V  0xFFFFFFFF
#define SLC_ID_S  0




#endif /*_SOC_SLC_REG_H_ */


