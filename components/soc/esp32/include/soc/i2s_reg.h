/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_I2S_REG_H_
#define _SOC_I2S_REG_H_

#include "soc.h"

#define REG_I2S_BASE( i ) ( DR_REG_I2S_BASE + ((i)*0x1E000))


#define I2S_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0008)
/* I2S_SIG_LOOPBACK : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define I2S_SIG_LOOPBACK  (BIT(18))
#define I2S_SIG_LOOPBACK_M  (BIT(18))
#define I2S_SIG_LOOPBACK_V  0x1
#define I2S_SIG_LOOPBACK_S  18
/* I2S_RX_MSB_RIGHT : R/W ;bitpos:[17] ;default: 1'b1 ; */
/*description: */
#define I2S_RX_MSB_RIGHT  (BIT(17))
#define I2S_RX_MSB_RIGHT_M  (BIT(17))
#define I2S_RX_MSB_RIGHT_V  0x1
#define I2S_RX_MSB_RIGHT_S  17
/* I2S_TX_MSB_RIGHT : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: */
#define I2S_TX_MSB_RIGHT  (BIT(16))
#define I2S_TX_MSB_RIGHT_M  (BIT(16))
#define I2S_TX_MSB_RIGHT_V  0x1
#define I2S_TX_MSB_RIGHT_S  16
/* I2S_RX_MONO : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_MONO  (BIT(15))
#define I2S_RX_MONO_M  (BIT(15))
#define I2S_RX_MONO_V  0x1
#define I2S_RX_MONO_S  15
/* I2S_TX_MONO : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_MONO  (BIT(14))
#define I2S_TX_MONO_M  (BIT(14))
#define I2S_TX_MONO_V  0x1
#define I2S_TX_MONO_S  14
/* I2S_RX_SHORT_SYNC : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_SHORT_SYNC  (BIT(13))
#define I2S_RX_SHORT_SYNC_M  (BIT(13))
#define I2S_RX_SHORT_SYNC_V  0x1
#define I2S_RX_SHORT_SYNC_S  13
/* I2S_TX_SHORT_SYNC : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_SHORT_SYNC  (BIT(12))
#define I2S_TX_SHORT_SYNC_M  (BIT(12))
#define I2S_TX_SHORT_SYNC_V  0x1
#define I2S_TX_SHORT_SYNC_S  12
/* I2S_RX_MSB_SHIFT : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_MSB_SHIFT  (BIT(11))
#define I2S_RX_MSB_SHIFT_M  (BIT(11))
#define I2S_RX_MSB_SHIFT_V  0x1
#define I2S_RX_MSB_SHIFT_S  11
/* I2S_TX_MSB_SHIFT : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_MSB_SHIFT  (BIT(10))
#define I2S_TX_MSB_SHIFT_M  (BIT(10))
#define I2S_TX_MSB_SHIFT_V  0x1
#define I2S_TX_MSB_SHIFT_S  10
/* I2S_RX_RIGHT_FIRST : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: */
#define I2S_RX_RIGHT_FIRST  (BIT(9))
#define I2S_RX_RIGHT_FIRST_M  (BIT(9))
#define I2S_RX_RIGHT_FIRST_V  0x1
#define I2S_RX_RIGHT_FIRST_S  9
/* I2S_TX_RIGHT_FIRST : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define I2S_TX_RIGHT_FIRST  (BIT(8))
#define I2S_TX_RIGHT_FIRST_M  (BIT(8))
#define I2S_TX_RIGHT_FIRST_V  0x1
#define I2S_TX_RIGHT_FIRST_S  8
/* I2S_RX_SLAVE_MOD : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_SLAVE_MOD  (BIT(7))
#define I2S_RX_SLAVE_MOD_M  (BIT(7))
#define I2S_RX_SLAVE_MOD_V  0x1
#define I2S_RX_SLAVE_MOD_S  7
/* I2S_TX_SLAVE_MOD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_SLAVE_MOD  (BIT(6))
#define I2S_TX_SLAVE_MOD_M  (BIT(6))
#define I2S_TX_SLAVE_MOD_V  0x1
#define I2S_TX_SLAVE_MOD_S  6
/* I2S_RX_START : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_START  (BIT(5))
#define I2S_RX_START_M  (BIT(5))
#define I2S_RX_START_V  0x1
#define I2S_RX_START_S  5
/* I2S_TX_START : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_START  (BIT(4))
#define I2S_TX_START_M  (BIT(4))
#define I2S_TX_START_V  0x1
#define I2S_TX_START_S  4
/* I2S_RX_FIFO_RESET : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_FIFO_RESET  (BIT(3))
#define I2S_RX_FIFO_RESET_M  (BIT(3))
#define I2S_RX_FIFO_RESET_V  0x1
#define I2S_RX_FIFO_RESET_S  3
/* I2S_TX_FIFO_RESET : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_FIFO_RESET  (BIT(2))
#define I2S_TX_FIFO_RESET_M  (BIT(2))
#define I2S_TX_FIFO_RESET_V  0x1
#define I2S_TX_FIFO_RESET_S  2
/* I2S_RX_RESET : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_RESET  (BIT(1))
#define I2S_RX_RESET_M  (BIT(1))
#define I2S_RX_RESET_V  0x1
#define I2S_RX_RESET_S  1
/* I2S_TX_RESET : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_RESET  (BIT(0))
#define I2S_TX_RESET_M  (BIT(0))
#define I2S_TX_RESET_V  0x1
#define I2S_TX_RESET_S  0

#define I2S_INT_RAW_REG(i)          (REG_I2S_BASE(i) + 0x000c)
/* I2S_OUT_TOTAL_EOF_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_TOTAL_EOF_INT_RAW  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_RAW_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_RAW_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_RAW_S  16
/* I2S_IN_DSCR_EMPTY_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_EMPTY_INT_RAW  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_RAW_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_RAW_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_RAW_S  15
/* I2S_OUT_DSCR_ERR_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DSCR_ERR_INT_RAW  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_RAW_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_RAW_V  0x1
#define I2S_OUT_DSCR_ERR_INT_RAW_S  14
/* I2S_IN_DSCR_ERR_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_ERR_INT_RAW  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_RAW_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_RAW_V  0x1
#define I2S_IN_DSCR_ERR_INT_RAW_S  13
/* I2S_OUT_EOF_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_EOF_INT_RAW  (BIT(12))
#define I2S_OUT_EOF_INT_RAW_M  (BIT(12))
#define I2S_OUT_EOF_INT_RAW_V  0x1
#define I2S_OUT_EOF_INT_RAW_S  12
/* I2S_OUT_DONE_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DONE_INT_RAW  (BIT(11))
#define I2S_OUT_DONE_INT_RAW_M  (BIT(11))
#define I2S_OUT_DONE_INT_RAW_V  0x1
#define I2S_OUT_DONE_INT_RAW_S  11
/* I2S_IN_ERR_EOF_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_ERR_EOF_INT_RAW  (BIT(10))
#define I2S_IN_ERR_EOF_INT_RAW_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_RAW_V  0x1
#define I2S_IN_ERR_EOF_INT_RAW_S  10
/* I2S_IN_SUC_EOF_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_SUC_EOF_INT_RAW  (BIT(9))
#define I2S_IN_SUC_EOF_INT_RAW_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_RAW_V  0x1
#define I2S_IN_SUC_EOF_INT_RAW_S  9
/* I2S_IN_DONE_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DONE_INT_RAW  (BIT(8))
#define I2S_IN_DONE_INT_RAW_M  (BIT(8))
#define I2S_IN_DONE_INT_RAW_V  0x1
#define I2S_IN_DONE_INT_RAW_S  8
/* I2S_TX_HUNG_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_HUNG_INT_RAW  (BIT(7))
#define I2S_TX_HUNG_INT_RAW_M  (BIT(7))
#define I2S_TX_HUNG_INT_RAW_V  0x1
#define I2S_TX_HUNG_INT_RAW_S  7
/* I2S_RX_HUNG_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_HUNG_INT_RAW  (BIT(6))
#define I2S_RX_HUNG_INT_RAW_M  (BIT(6))
#define I2S_RX_HUNG_INT_RAW_V  0x1
#define I2S_RX_HUNG_INT_RAW_S  6
/* I2S_TX_REMPTY_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_REMPTY_INT_RAW  (BIT(5))
#define I2S_TX_REMPTY_INT_RAW_M  (BIT(5))
#define I2S_TX_REMPTY_INT_RAW_V  0x1
#define I2S_TX_REMPTY_INT_RAW_S  5
/* I2S_TX_WFULL_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_WFULL_INT_RAW  (BIT(4))
#define I2S_TX_WFULL_INT_RAW_M  (BIT(4))
#define I2S_TX_WFULL_INT_RAW_V  0x1
#define I2S_TX_WFULL_INT_RAW_S  4
/* I2S_RX_REMPTY_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_REMPTY_INT_RAW  (BIT(3))
#define I2S_RX_REMPTY_INT_RAW_M  (BIT(3))
#define I2S_RX_REMPTY_INT_RAW_V  0x1
#define I2S_RX_REMPTY_INT_RAW_S  3
/* I2S_RX_WFULL_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_WFULL_INT_RAW  (BIT(2))
#define I2S_RX_WFULL_INT_RAW_M  (BIT(2))
#define I2S_RX_WFULL_INT_RAW_V  0x1
#define I2S_RX_WFULL_INT_RAW_S  2
/* I2S_TX_PUT_DATA_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_PUT_DATA_INT_RAW  (BIT(1))
#define I2S_TX_PUT_DATA_INT_RAW_M  (BIT(1))
#define I2S_TX_PUT_DATA_INT_RAW_V  0x1
#define I2S_TX_PUT_DATA_INT_RAW_S  1
/* I2S_RX_TAKE_DATA_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_TAKE_DATA_INT_RAW  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_RAW_M  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_RAW_V  0x1
#define I2S_RX_TAKE_DATA_INT_RAW_S  0

#define I2S_INT_ST_REG(i)          (REG_I2S_BASE(i) + 0x0010)
/* I2S_OUT_TOTAL_EOF_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_TOTAL_EOF_INT_ST  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ST_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ST_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_ST_S  16
/* I2S_IN_DSCR_EMPTY_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_EMPTY_INT_ST  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ST_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ST_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_ST_S  15
/* I2S_OUT_DSCR_ERR_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DSCR_ERR_INT_ST  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ST_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ST_V  0x1
#define I2S_OUT_DSCR_ERR_INT_ST_S  14
/* I2S_IN_DSCR_ERR_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_ERR_INT_ST  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ST_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ST_V  0x1
#define I2S_IN_DSCR_ERR_INT_ST_S  13
/* I2S_OUT_EOF_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_EOF_INT_ST  (BIT(12))
#define I2S_OUT_EOF_INT_ST_M  (BIT(12))
#define I2S_OUT_EOF_INT_ST_V  0x1
#define I2S_OUT_EOF_INT_ST_S  12
/* I2S_OUT_DONE_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DONE_INT_ST  (BIT(11))
#define I2S_OUT_DONE_INT_ST_M  (BIT(11))
#define I2S_OUT_DONE_INT_ST_V  0x1
#define I2S_OUT_DONE_INT_ST_S  11
/* I2S_IN_ERR_EOF_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_ERR_EOF_INT_ST  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ST_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ST_V  0x1
#define I2S_IN_ERR_EOF_INT_ST_S  10
/* I2S_IN_SUC_EOF_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_SUC_EOF_INT_ST  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ST_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ST_V  0x1
#define I2S_IN_SUC_EOF_INT_ST_S  9
/* I2S_IN_DONE_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DONE_INT_ST  (BIT(8))
#define I2S_IN_DONE_INT_ST_M  (BIT(8))
#define I2S_IN_DONE_INT_ST_V  0x1
#define I2S_IN_DONE_INT_ST_S  8
/* I2S_TX_HUNG_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_HUNG_INT_ST  (BIT(7))
#define I2S_TX_HUNG_INT_ST_M  (BIT(7))
#define I2S_TX_HUNG_INT_ST_V  0x1
#define I2S_TX_HUNG_INT_ST_S  7
/* I2S_RX_HUNG_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_HUNG_INT_ST  (BIT(6))
#define I2S_RX_HUNG_INT_ST_M  (BIT(6))
#define I2S_RX_HUNG_INT_ST_V  0x1
#define I2S_RX_HUNG_INT_ST_S  6
/* I2S_TX_REMPTY_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_REMPTY_INT_ST  (BIT(5))
#define I2S_TX_REMPTY_INT_ST_M  (BIT(5))
#define I2S_TX_REMPTY_INT_ST_V  0x1
#define I2S_TX_REMPTY_INT_ST_S  5
/* I2S_TX_WFULL_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_WFULL_INT_ST  (BIT(4))
#define I2S_TX_WFULL_INT_ST_M  (BIT(4))
#define I2S_TX_WFULL_INT_ST_V  0x1
#define I2S_TX_WFULL_INT_ST_S  4
/* I2S_RX_REMPTY_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_REMPTY_INT_ST  (BIT(3))
#define I2S_RX_REMPTY_INT_ST_M  (BIT(3))
#define I2S_RX_REMPTY_INT_ST_V  0x1
#define I2S_RX_REMPTY_INT_ST_S  3
/* I2S_RX_WFULL_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_WFULL_INT_ST  (BIT(2))
#define I2S_RX_WFULL_INT_ST_M  (BIT(2))
#define I2S_RX_WFULL_INT_ST_V  0x1
#define I2S_RX_WFULL_INT_ST_S  2
/* I2S_TX_PUT_DATA_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_PUT_DATA_INT_ST  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ST_M  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ST_V  0x1
#define I2S_TX_PUT_DATA_INT_ST_S  1
/* I2S_RX_TAKE_DATA_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_TAKE_DATA_INT_ST  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ST_M  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ST_V  0x1
#define I2S_RX_TAKE_DATA_INT_ST_S  0

#define I2S_INT_ENA_REG(i)          (REG_I2S_BASE(i) + 0x0014)
/* I2S_OUT_TOTAL_EOF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_TOTAL_EOF_INT_ENA  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ENA_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ENA_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_ENA_S  16
/* I2S_IN_DSCR_EMPTY_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_EMPTY_INT_ENA  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ENA_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ENA_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_ENA_S  15
/* I2S_OUT_DSCR_ERR_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DSCR_ERR_INT_ENA  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ENA_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ENA_V  0x1
#define I2S_OUT_DSCR_ERR_INT_ENA_S  14
/* I2S_IN_DSCR_ERR_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_ERR_INT_ENA  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ENA_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ENA_V  0x1
#define I2S_IN_DSCR_ERR_INT_ENA_S  13
/* I2S_OUT_EOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_EOF_INT_ENA  (BIT(12))
#define I2S_OUT_EOF_INT_ENA_M  (BIT(12))
#define I2S_OUT_EOF_INT_ENA_V  0x1
#define I2S_OUT_EOF_INT_ENA_S  12
/* I2S_OUT_DONE_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DONE_INT_ENA  (BIT(11))
#define I2S_OUT_DONE_INT_ENA_M  (BIT(11))
#define I2S_OUT_DONE_INT_ENA_V  0x1
#define I2S_OUT_DONE_INT_ENA_S  11
/* I2S_IN_ERR_EOF_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_ERR_EOF_INT_ENA  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ENA_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ENA_V  0x1
#define I2S_IN_ERR_EOF_INT_ENA_S  10
/* I2S_IN_SUC_EOF_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_SUC_EOF_INT_ENA  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ENA_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ENA_V  0x1
#define I2S_IN_SUC_EOF_INT_ENA_S  9
/* I2S_IN_DONE_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DONE_INT_ENA  (BIT(8))
#define I2S_IN_DONE_INT_ENA_M  (BIT(8))
#define I2S_IN_DONE_INT_ENA_V  0x1
#define I2S_IN_DONE_INT_ENA_S  8
/* I2S_TX_HUNG_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_HUNG_INT_ENA  (BIT(7))
#define I2S_TX_HUNG_INT_ENA_M  (BIT(7))
#define I2S_TX_HUNG_INT_ENA_V  0x1
#define I2S_TX_HUNG_INT_ENA_S  7
/* I2S_RX_HUNG_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_HUNG_INT_ENA  (BIT(6))
#define I2S_RX_HUNG_INT_ENA_M  (BIT(6))
#define I2S_RX_HUNG_INT_ENA_V  0x1
#define I2S_RX_HUNG_INT_ENA_S  6
/* I2S_TX_REMPTY_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_REMPTY_INT_ENA  (BIT(5))
#define I2S_TX_REMPTY_INT_ENA_M  (BIT(5))
#define I2S_TX_REMPTY_INT_ENA_V  0x1
#define I2S_TX_REMPTY_INT_ENA_S  5
/* I2S_TX_WFULL_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_WFULL_INT_ENA  (BIT(4))
#define I2S_TX_WFULL_INT_ENA_M  (BIT(4))
#define I2S_TX_WFULL_INT_ENA_V  0x1
#define I2S_TX_WFULL_INT_ENA_S  4
/* I2S_RX_REMPTY_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_REMPTY_INT_ENA  (BIT(3))
#define I2S_RX_REMPTY_INT_ENA_M  (BIT(3))
#define I2S_RX_REMPTY_INT_ENA_V  0x1
#define I2S_RX_REMPTY_INT_ENA_S  3
/* I2S_RX_WFULL_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_WFULL_INT_ENA  (BIT(2))
#define I2S_RX_WFULL_INT_ENA_M  (BIT(2))
#define I2S_RX_WFULL_INT_ENA_V  0x1
#define I2S_RX_WFULL_INT_ENA_S  2
/* I2S_TX_PUT_DATA_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_PUT_DATA_INT_ENA  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ENA_M  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ENA_V  0x1
#define I2S_TX_PUT_DATA_INT_ENA_S  1
/* I2S_RX_TAKE_DATA_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_TAKE_DATA_INT_ENA  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ENA_M  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ENA_V  0x1
#define I2S_RX_TAKE_DATA_INT_ENA_S  0

#define I2S_INT_CLR_REG(i)          (REG_I2S_BASE(i) + 0x0018)
/* I2S_OUT_TOTAL_EOF_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_TOTAL_EOF_INT_CLR  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_CLR_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_CLR_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_CLR_S  16
/* I2S_IN_DSCR_EMPTY_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_EMPTY_INT_CLR  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_CLR_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_CLR_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_CLR_S  15
/* I2S_OUT_DSCR_ERR_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DSCR_ERR_INT_CLR  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_CLR_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_CLR_V  0x1
#define I2S_OUT_DSCR_ERR_INT_CLR_S  14
/* I2S_IN_DSCR_ERR_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DSCR_ERR_INT_CLR  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_CLR_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_CLR_V  0x1
#define I2S_IN_DSCR_ERR_INT_CLR_S  13
/* I2S_OUT_EOF_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_EOF_INT_CLR  (BIT(12))
#define I2S_OUT_EOF_INT_CLR_M  (BIT(12))
#define I2S_OUT_EOF_INT_CLR_V  0x1
#define I2S_OUT_EOF_INT_CLR_S  12
/* I2S_OUT_DONE_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DONE_INT_CLR  (BIT(11))
#define I2S_OUT_DONE_INT_CLR_M  (BIT(11))
#define I2S_OUT_DONE_INT_CLR_V  0x1
#define I2S_OUT_DONE_INT_CLR_S  11
/* I2S_IN_ERR_EOF_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_ERR_EOF_INT_CLR  (BIT(10))
#define I2S_IN_ERR_EOF_INT_CLR_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_CLR_V  0x1
#define I2S_IN_ERR_EOF_INT_CLR_S  10
/* I2S_IN_SUC_EOF_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_SUC_EOF_INT_CLR  (BIT(9))
#define I2S_IN_SUC_EOF_INT_CLR_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_CLR_V  0x1
#define I2S_IN_SUC_EOF_INT_CLR_S  9
/* I2S_IN_DONE_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_DONE_INT_CLR  (BIT(8))
#define I2S_IN_DONE_INT_CLR_M  (BIT(8))
#define I2S_IN_DONE_INT_CLR_V  0x1
#define I2S_IN_DONE_INT_CLR_S  8
/* I2S_TX_HUNG_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_HUNG_INT_CLR  (BIT(7))
#define I2S_TX_HUNG_INT_CLR_M  (BIT(7))
#define I2S_TX_HUNG_INT_CLR_V  0x1
#define I2S_TX_HUNG_INT_CLR_S  7
/* I2S_RX_HUNG_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_HUNG_INT_CLR  (BIT(6))
#define I2S_RX_HUNG_INT_CLR_M  (BIT(6))
#define I2S_RX_HUNG_INT_CLR_V  0x1
#define I2S_RX_HUNG_INT_CLR_S  6
/* I2S_TX_REMPTY_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_REMPTY_INT_CLR  (BIT(5))
#define I2S_TX_REMPTY_INT_CLR_M  (BIT(5))
#define I2S_TX_REMPTY_INT_CLR_V  0x1
#define I2S_TX_REMPTY_INT_CLR_S  5
/* I2S_TX_WFULL_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_WFULL_INT_CLR  (BIT(4))
#define I2S_TX_WFULL_INT_CLR_M  (BIT(4))
#define I2S_TX_WFULL_INT_CLR_V  0x1
#define I2S_TX_WFULL_INT_CLR_S  4
/* I2S_RX_REMPTY_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_REMPTY_INT_CLR  (BIT(3))
#define I2S_RX_REMPTY_INT_CLR_M  (BIT(3))
#define I2S_RX_REMPTY_INT_CLR_V  0x1
#define I2S_RX_REMPTY_INT_CLR_S  3
/* I2S_RX_WFULL_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_WFULL_INT_CLR  (BIT(2))
#define I2S_RX_WFULL_INT_CLR_M  (BIT(2))
#define I2S_RX_WFULL_INT_CLR_V  0x1
#define I2S_RX_WFULL_INT_CLR_S  2
/* I2S_PUT_DATA_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define I2S_PUT_DATA_INT_CLR  (BIT(1))
#define I2S_PUT_DATA_INT_CLR_M  (BIT(1))
#define I2S_PUT_DATA_INT_CLR_V  0x1
#define I2S_PUT_DATA_INT_CLR_S  1
/* I2S_TAKE_DATA_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define I2S_TAKE_DATA_INT_CLR  (BIT(0))
#define I2S_TAKE_DATA_INT_CLR_M  (BIT(0))
#define I2S_TAKE_DATA_INT_CLR_V  0x1
#define I2S_TAKE_DATA_INT_CLR_S  0

#define I2S_TIMING_REG(i)          (REG_I2S_BASE(i) + 0x001c)
/* I2S_TX_BCK_IN_INV : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_BCK_IN_INV  (BIT(24))
#define I2S_TX_BCK_IN_INV_M  (BIT(24))
#define I2S_TX_BCK_IN_INV_V  0x1
#define I2S_TX_BCK_IN_INV_S  24
/* I2S_DATA_ENABLE_DELAY : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: */
#define I2S_DATA_ENABLE_DELAY  0x00000003
#define I2S_DATA_ENABLE_DELAY_M  ((I2S_DATA_ENABLE_DELAY_V)<<(I2S_DATA_ENABLE_DELAY_S))
#define I2S_DATA_ENABLE_DELAY_V  0x3
#define I2S_DATA_ENABLE_DELAY_S  22
/* I2S_RX_DSYNC_SW : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_DSYNC_SW  (BIT(21))
#define I2S_RX_DSYNC_SW_M  (BIT(21))
#define I2S_RX_DSYNC_SW_V  0x1
#define I2S_RX_DSYNC_SW_S  21
/* I2S_TX_DSYNC_SW : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_DSYNC_SW  (BIT(20))
#define I2S_TX_DSYNC_SW_M  (BIT(20))
#define I2S_TX_DSYNC_SW_V  0x1
#define I2S_TX_DSYNC_SW_S  20
/* I2S_RX_BCK_OUT_DELAY : R/W ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: */
#define I2S_RX_BCK_OUT_DELAY  0x00000003
#define I2S_RX_BCK_OUT_DELAY_M  ((I2S_RX_BCK_OUT_DELAY_V)<<(I2S_RX_BCK_OUT_DELAY_S))
#define I2S_RX_BCK_OUT_DELAY_V  0x3
#define I2S_RX_BCK_OUT_DELAY_S  18
/* I2S_RX_WS_OUT_DELAY : R/W ;bitpos:[17:16] ;default: 2'b0 ; */
/*description: */
#define I2S_RX_WS_OUT_DELAY  0x00000003
#define I2S_RX_WS_OUT_DELAY_M  ((I2S_RX_WS_OUT_DELAY_V)<<(I2S_RX_WS_OUT_DELAY_S))
#define I2S_RX_WS_OUT_DELAY_V  0x3
#define I2S_RX_WS_OUT_DELAY_S  16
/* I2S_TX_SD_OUT_DELAY : R/W ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: */
#define I2S_TX_SD_OUT_DELAY  0x00000003
#define I2S_TX_SD_OUT_DELAY_M  ((I2S_TX_SD_OUT_DELAY_V)<<(I2S_TX_SD_OUT_DELAY_S))
#define I2S_TX_SD_OUT_DELAY_V  0x3
#define I2S_TX_SD_OUT_DELAY_S  14
/* I2S_TX_WS_OUT_DELAY : R/W ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: */
#define I2S_TX_WS_OUT_DELAY  0x00000003
#define I2S_TX_WS_OUT_DELAY_M  ((I2S_TX_WS_OUT_DELAY_V)<<(I2S_TX_WS_OUT_DELAY_S))
#define I2S_TX_WS_OUT_DELAY_V  0x3
#define I2S_TX_WS_OUT_DELAY_S  12
/* I2S_TX_BCK_OUT_DELAY : R/W ;bitpos:[11:10] ;default: 2'b0 ; */
/*description: */
#define I2S_TX_BCK_OUT_DELAY  0x00000003
#define I2S_TX_BCK_OUT_DELAY_M  ((I2S_TX_BCK_OUT_DELAY_V)<<(I2S_TX_BCK_OUT_DELAY_S))
#define I2S_TX_BCK_OUT_DELAY_V  0x3
#define I2S_TX_BCK_OUT_DELAY_S  10
/* I2S_RX_SD_IN_DELAY : R/W ;bitpos:[9:8] ;default: 2'b0 ; */
/*description: */
#define I2S_RX_SD_IN_DELAY  0x00000003
#define I2S_RX_SD_IN_DELAY_M  ((I2S_RX_SD_IN_DELAY_V)<<(I2S_RX_SD_IN_DELAY_S))
#define I2S_RX_SD_IN_DELAY_V  0x3
#define I2S_RX_SD_IN_DELAY_S  8
/* I2S_RX_WS_IN_DELAY : R/W ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: */
#define I2S_RX_WS_IN_DELAY  0x00000003
#define I2S_RX_WS_IN_DELAY_M  ((I2S_RX_WS_IN_DELAY_V)<<(I2S_RX_WS_IN_DELAY_S))
#define I2S_RX_WS_IN_DELAY_V  0x3
#define I2S_RX_WS_IN_DELAY_S  6
/* I2S_RX_BCK_IN_DELAY : R/W ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: */
#define I2S_RX_BCK_IN_DELAY  0x00000003
#define I2S_RX_BCK_IN_DELAY_M  ((I2S_RX_BCK_IN_DELAY_V)<<(I2S_RX_BCK_IN_DELAY_S))
#define I2S_RX_BCK_IN_DELAY_V  0x3
#define I2S_RX_BCK_IN_DELAY_S  4
/* I2S_TX_WS_IN_DELAY : R/W ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: */
#define I2S_TX_WS_IN_DELAY  0x00000003
#define I2S_TX_WS_IN_DELAY_M  ((I2S_TX_WS_IN_DELAY_V)<<(I2S_TX_WS_IN_DELAY_S))
#define I2S_TX_WS_IN_DELAY_V  0x3
#define I2S_TX_WS_IN_DELAY_S  2
/* I2S_TX_BCK_IN_DELAY : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define I2S_TX_BCK_IN_DELAY  0x00000003
#define I2S_TX_BCK_IN_DELAY_M  ((I2S_TX_BCK_IN_DELAY_V)<<(I2S_TX_BCK_IN_DELAY_S))
#define I2S_TX_BCK_IN_DELAY_V  0x3
#define I2S_TX_BCK_IN_DELAY_S  0

#define I2S_FIFO_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0020)
/* I2S_RX_FIFO_MOD_FORCE_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_FIFO_MOD_FORCE_EN  (BIT(20))
#define I2S_RX_FIFO_MOD_FORCE_EN_M  (BIT(20))
#define I2S_RX_FIFO_MOD_FORCE_EN_V  0x1
#define I2S_RX_FIFO_MOD_FORCE_EN_S  20
/* I2S_TX_FIFO_MOD_FORCE_EN : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_FIFO_MOD_FORCE_EN  (BIT(19))
#define I2S_TX_FIFO_MOD_FORCE_EN_M  (BIT(19))
#define I2S_TX_FIFO_MOD_FORCE_EN_V  0x1
#define I2S_TX_FIFO_MOD_FORCE_EN_S  19
/* I2S_RX_FIFO_MOD : R/W ;bitpos:[18:16] ;default: 3'b0 ; */
/*description: */
#define I2S_RX_FIFO_MOD  0x00000007
#define I2S_RX_FIFO_MOD_M  ((I2S_RX_FIFO_MOD_V)<<(I2S_RX_FIFO_MOD_S))
#define I2S_RX_FIFO_MOD_V  0x7
#define I2S_RX_FIFO_MOD_S  16
/* I2S_TX_FIFO_MOD : R/W ;bitpos:[15:13] ;default: 3'b0 ; */
/*description: */
#define I2S_TX_FIFO_MOD  0x00000007
#define I2S_TX_FIFO_MOD_M  ((I2S_TX_FIFO_MOD_V)<<(I2S_TX_FIFO_MOD_S))
#define I2S_TX_FIFO_MOD_V  0x7
#define I2S_TX_FIFO_MOD_S  13
/* I2S_DSCR_EN : R/W ;bitpos:[12] ;default: 1'd1 ; */
/*description: */
#define I2S_DSCR_EN  (BIT(12))
#define I2S_DSCR_EN_M  (BIT(12))
#define I2S_DSCR_EN_V  0x1
#define I2S_DSCR_EN_S  12
/* I2S_TX_DATA_NUM : R/W ;bitpos:[11:6] ;default: 6'd32 ; */
/*description: */
#define I2S_TX_DATA_NUM  0x0000003F
#define I2S_TX_DATA_NUM_M  ((I2S_TX_DATA_NUM_V)<<(I2S_TX_DATA_NUM_S))
#define I2S_TX_DATA_NUM_V  0x3F
#define I2S_TX_DATA_NUM_S  6
/* I2S_RX_DATA_NUM : R/W ;bitpos:[5:0] ;default: 6'd32 ; */
/*description: */
#define I2S_RX_DATA_NUM  0x0000003F
#define I2S_RX_DATA_NUM_M  ((I2S_RX_DATA_NUM_V)<<(I2S_RX_DATA_NUM_S))
#define I2S_RX_DATA_NUM_V  0x3F
#define I2S_RX_DATA_NUM_S  0

#define I2S_RXEOF_NUM_REG(i)          (REG_I2S_BASE(i) + 0x0024)
/* I2S_RX_EOF_NUM : R/W ;bitpos:[31:0] ;default: 32'd64 ; */
/*description: */
#define I2S_RX_EOF_NUM  0xFFFFFFFF
#define I2S_RX_EOF_NUM_M  ((I2S_RX_EOF_NUM_V)<<(I2S_RX_EOF_NUM_S))
#define I2S_RX_EOF_NUM_V  0xFFFFFFFF
#define I2S_RX_EOF_NUM_S  0

#define I2S_CONF_SIGLE_DATA_REG(i)          (REG_I2S_BASE(i) + 0x0028)
/* I2S_SIGLE_DATA : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define I2S_SIGLE_DATA  0xFFFFFFFF
#define I2S_SIGLE_DATA_M  ((I2S_SIGLE_DATA_V)<<(I2S_SIGLE_DATA_S))
#define I2S_SIGLE_DATA_V  0xFFFFFFFF
#define I2S_SIGLE_DATA_S  0

#define I2S_CONF_CHAN_REG(i)          (REG_I2S_BASE(i) + 0x002c)
/* I2S_RX_CHAN_MOD : R/W ;bitpos:[4:3] ;default: 2'b0 ; */
/*description: */
#define I2S_RX_CHAN_MOD  0x00000003
#define I2S_RX_CHAN_MOD_M  ((I2S_RX_CHAN_MOD_V)<<(I2S_RX_CHAN_MOD_S))
#define I2S_RX_CHAN_MOD_V  0x3
#define I2S_RX_CHAN_MOD_S  3
/* I2S_TX_CHAN_MOD : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define I2S_TX_CHAN_MOD  0x00000007
#define I2S_TX_CHAN_MOD_M  ((I2S_TX_CHAN_MOD_V)<<(I2S_TX_CHAN_MOD_S))
#define I2S_TX_CHAN_MOD_V  0x7
#define I2S_TX_CHAN_MOD_S  0

#define I2S_OUT_LINK_REG(i)          (REG_I2S_BASE(i) + 0x0030)
/* I2S_OUTLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define I2S_OUTLINK_PARK  (BIT(31))
#define I2S_OUTLINK_PARK_M  (BIT(31))
#define I2S_OUTLINK_PARK_V  0x1
#define I2S_OUTLINK_PARK_S  31
/* I2S_OUTLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define I2S_OUTLINK_RESTART  (BIT(30))
#define I2S_OUTLINK_RESTART_M  (BIT(30))
#define I2S_OUTLINK_RESTART_V  0x1
#define I2S_OUTLINK_RESTART_S  30
/* I2S_OUTLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define I2S_OUTLINK_START  (BIT(29))
#define I2S_OUTLINK_START_M  (BIT(29))
#define I2S_OUTLINK_START_V  0x1
#define I2S_OUTLINK_START_S  29
/* I2S_OUTLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define I2S_OUTLINK_STOP  (BIT(28))
#define I2S_OUTLINK_STOP_M  (BIT(28))
#define I2S_OUTLINK_STOP_V  0x1
#define I2S_OUTLINK_STOP_S  28
/* I2S_OUTLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define I2S_OUTLINK_ADDR  0x000FFFFF
#define I2S_OUTLINK_ADDR_M  ((I2S_OUTLINK_ADDR_V)<<(I2S_OUTLINK_ADDR_S))
#define I2S_OUTLINK_ADDR_V  0xFFFFF
#define I2S_OUTLINK_ADDR_S  0

#define I2S_IN_LINK_REG(i)          (REG_I2S_BASE(i) + 0x0034)
/* I2S_INLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define I2S_INLINK_PARK  (BIT(31))
#define I2S_INLINK_PARK_M  (BIT(31))
#define I2S_INLINK_PARK_V  0x1
#define I2S_INLINK_PARK_S  31
/* I2S_INLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define I2S_INLINK_RESTART  (BIT(30))
#define I2S_INLINK_RESTART_M  (BIT(30))
#define I2S_INLINK_RESTART_V  0x1
#define I2S_INLINK_RESTART_S  30
/* I2S_INLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define I2S_INLINK_START  (BIT(29))
#define I2S_INLINK_START_M  (BIT(29))
#define I2S_INLINK_START_V  0x1
#define I2S_INLINK_START_S  29
/* I2S_INLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define I2S_INLINK_STOP  (BIT(28))
#define I2S_INLINK_STOP_M  (BIT(28))
#define I2S_INLINK_STOP_V  0x1
#define I2S_INLINK_STOP_S  28
/* I2S_INLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define I2S_INLINK_ADDR  0x000FFFFF
#define I2S_INLINK_ADDR_M  ((I2S_INLINK_ADDR_V)<<(I2S_INLINK_ADDR_S))
#define I2S_INLINK_ADDR_V  0xFFFFF
#define I2S_INLINK_ADDR_S  0

#define I2S_OUT_EOF_DES_ADDR_REG(i)          (REG_I2S_BASE(i) + 0x0038)
/* I2S_OUT_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define I2S_OUT_EOF_DES_ADDR  0xFFFFFFFF
#define I2S_OUT_EOF_DES_ADDR_M  ((I2S_OUT_EOF_DES_ADDR_V)<<(I2S_OUT_EOF_DES_ADDR_S))
#define I2S_OUT_EOF_DES_ADDR_V  0xFFFFFFFF
#define I2S_OUT_EOF_DES_ADDR_S  0

#define I2S_IN_EOF_DES_ADDR_REG(i)          (REG_I2S_BASE(i) + 0x003c)
/* I2S_IN_SUC_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define I2S_IN_SUC_EOF_DES_ADDR  0xFFFFFFFF
#define I2S_IN_SUC_EOF_DES_ADDR_M  ((I2S_IN_SUC_EOF_DES_ADDR_V)<<(I2S_IN_SUC_EOF_DES_ADDR_S))
#define I2S_IN_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define I2S_IN_SUC_EOF_DES_ADDR_S  0

#define I2S_OUT_EOF_BFR_DES_ADDR_REG(i)          (REG_I2S_BASE(i) + 0x0040)
/* I2S_OUT_EOF_BFR_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define I2S_OUT_EOF_BFR_DES_ADDR  0xFFFFFFFF
#define I2S_OUT_EOF_BFR_DES_ADDR_M  ((I2S_OUT_EOF_BFR_DES_ADDR_V)<<(I2S_OUT_EOF_BFR_DES_ADDR_S))
#define I2S_OUT_EOF_BFR_DES_ADDR_V  0xFFFFFFFF
#define I2S_OUT_EOF_BFR_DES_ADDR_S  0

#define I2S_AHB_TEST_REG(i)          (REG_I2S_BASE(i) + 0x0044)
/* I2S_AHB_TESTADDR : R/W ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: */
#define I2S_AHB_TESTADDR  0x00000003
#define I2S_AHB_TESTADDR_M  ((I2S_AHB_TESTADDR_V)<<(I2S_AHB_TESTADDR_S))
#define I2S_AHB_TESTADDR_V  0x3
#define I2S_AHB_TESTADDR_S  4
/* I2S_AHB_TESTMODE : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define I2S_AHB_TESTMODE  0x00000007
#define I2S_AHB_TESTMODE_M  ((I2S_AHB_TESTMODE_V)<<(I2S_AHB_TESTMODE_S))
#define I2S_AHB_TESTMODE_V  0x7
#define I2S_AHB_TESTMODE_S  0

#define I2S_INLINK_DSCR_REG(i)          (REG_I2S_BASE(i) + 0x0048)
/* I2S_INLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define I2S_INLINK_DSCR  0xFFFFFFFF
#define I2S_INLINK_DSCR_M  ((I2S_INLINK_DSCR_V)<<(I2S_INLINK_DSCR_S))
#define I2S_INLINK_DSCR_V  0xFFFFFFFF
#define I2S_INLINK_DSCR_S  0

#define I2S_INLINK_DSCR_BF0_REG(i)          (REG_I2S_BASE(i) + 0x004C)
/* I2S_INLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define I2S_INLINK_DSCR_BF0  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF0_M  ((I2S_INLINK_DSCR_BF0_V)<<(I2S_INLINK_DSCR_BF0_S))
#define I2S_INLINK_DSCR_BF0_V  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF0_S  0

#define I2S_INLINK_DSCR_BF1_REG(i)          (REG_I2S_BASE(i) + 0x0050)
/* I2S_INLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define I2S_INLINK_DSCR_BF1  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF1_M  ((I2S_INLINK_DSCR_BF1_V)<<(I2S_INLINK_DSCR_BF1_S))
#define I2S_INLINK_DSCR_BF1_V  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF1_S  0

#define I2S_OUTLINK_DSCR_REG(i)          (REG_I2S_BASE(i) + 0x0054)
/* I2S_OUTLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define I2S_OUTLINK_DSCR  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_M  ((I2S_OUTLINK_DSCR_V)<<(I2S_OUTLINK_DSCR_S))
#define I2S_OUTLINK_DSCR_V  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_S  0

#define I2S_OUTLINK_DSCR_BF0_REG(i)          (REG_I2S_BASE(i) + 0x0058)
/* I2S_OUTLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define I2S_OUTLINK_DSCR_BF0  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF0_M  ((I2S_OUTLINK_DSCR_BF0_V)<<(I2S_OUTLINK_DSCR_BF0_S))
#define I2S_OUTLINK_DSCR_BF0_V  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF0_S  0

#define I2S_OUTLINK_DSCR_BF1_REG(i)          (REG_I2S_BASE(i) + 0x005C)
/* I2S_OUTLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define I2S_OUTLINK_DSCR_BF1  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF1_M  ((I2S_OUTLINK_DSCR_BF1_V)<<(I2S_OUTLINK_DSCR_BF1_S))
#define I2S_OUTLINK_DSCR_BF1_V  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF1_S  0

#define I2S_LC_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0060)
/* I2S_MEM_TRANS_EN : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define I2S_MEM_TRANS_EN  (BIT(13))
#define I2S_MEM_TRANS_EN_M  (BIT(13))
#define I2S_MEM_TRANS_EN_V  0x1
#define I2S_MEM_TRANS_EN_S  13
/* I2S_CHECK_OWNER : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define I2S_CHECK_OWNER  (BIT(12))
#define I2S_CHECK_OWNER_M  (BIT(12))
#define I2S_CHECK_OWNER_V  0x1
#define I2S_CHECK_OWNER_S  12
/* I2S_OUT_DATA_BURST_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_DATA_BURST_EN  (BIT(11))
#define I2S_OUT_DATA_BURST_EN_M  (BIT(11))
#define I2S_OUT_DATA_BURST_EN_V  0x1
#define I2S_OUT_DATA_BURST_EN_S  11
/* I2S_INDSCR_BURST_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_INDSCR_BURST_EN  (BIT(10))
#define I2S_INDSCR_BURST_EN_M  (BIT(10))
#define I2S_INDSCR_BURST_EN_V  0x1
#define I2S_INDSCR_BURST_EN_S  10
/* I2S_OUTDSCR_BURST_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define I2S_OUTDSCR_BURST_EN  (BIT(9))
#define I2S_OUTDSCR_BURST_EN_M  (BIT(9))
#define I2S_OUTDSCR_BURST_EN_V  0x1
#define I2S_OUTDSCR_BURST_EN_S  9
/* I2S_OUT_EOF_MODE : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define I2S_OUT_EOF_MODE  (BIT(8))
#define I2S_OUT_EOF_MODE_M  (BIT(8))
#define I2S_OUT_EOF_MODE_V  0x1
#define I2S_OUT_EOF_MODE_S  8
/* I2S_OUT_NO_RESTART_CLR : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_NO_RESTART_CLR  (BIT(7))
#define I2S_OUT_NO_RESTART_CLR_M  (BIT(7))
#define I2S_OUT_NO_RESTART_CLR_V  0x1
#define I2S_OUT_NO_RESTART_CLR_S  7
/* I2S_OUT_AUTO_WRBACK : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_AUTO_WRBACK  (BIT(6))
#define I2S_OUT_AUTO_WRBACK_M  (BIT(6))
#define I2S_OUT_AUTO_WRBACK_V  0x1
#define I2S_OUT_AUTO_WRBACK_S  6
/* I2S_IN_LOOP_TEST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_LOOP_TEST  (BIT(5))
#define I2S_IN_LOOP_TEST_M  (BIT(5))
#define I2S_IN_LOOP_TEST_V  0x1
#define I2S_IN_LOOP_TEST_S  5
/* I2S_OUT_LOOP_TEST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_LOOP_TEST  (BIT(4))
#define I2S_OUT_LOOP_TEST_M  (BIT(4))
#define I2S_OUT_LOOP_TEST_V  0x1
#define I2S_OUT_LOOP_TEST_S  4
/* I2S_AHBM_RST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define I2S_AHBM_RST  (BIT(3))
#define I2S_AHBM_RST_M  (BIT(3))
#define I2S_AHBM_RST_V  0x1
#define I2S_AHBM_RST_S  3
/* I2S_AHBM_FIFO_RST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define I2S_AHBM_FIFO_RST  (BIT(2))
#define I2S_AHBM_FIFO_RST_M  (BIT(2))
#define I2S_AHBM_FIFO_RST_V  0x1
#define I2S_AHBM_FIFO_RST_S  2
/* I2S_OUT_RST : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_RST  (BIT(1))
#define I2S_OUT_RST_M  (BIT(1))
#define I2S_OUT_RST_V  0x1
#define I2S_OUT_RST_S  1
/* I2S_IN_RST : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define I2S_IN_RST  (BIT(0))
#define I2S_IN_RST_M  (BIT(0))
#define I2S_IN_RST_V  0x1
#define I2S_IN_RST_S  0

#define I2S_OUTFIFO_PUSH_REG(i)          (REG_I2S_BASE(i) + 0x0064)
/* I2S_OUTFIFO_PUSH : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define I2S_OUTFIFO_PUSH  (BIT(16))
#define I2S_OUTFIFO_PUSH_M  (BIT(16))
#define I2S_OUTFIFO_PUSH_V  0x1
#define I2S_OUTFIFO_PUSH_S  16
/* I2S_OUTFIFO_WDATA : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: */
#define I2S_OUTFIFO_WDATA  0x000001FF
#define I2S_OUTFIFO_WDATA_M  ((I2S_OUTFIFO_WDATA_V)<<(I2S_OUTFIFO_WDATA_S))
#define I2S_OUTFIFO_WDATA_V  0x1FF
#define I2S_OUTFIFO_WDATA_S  0

#define I2S_INFIFO_POP_REG(i)          (REG_I2S_BASE(i) + 0x0068)
/* I2S_INFIFO_POP : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: */
#define I2S_INFIFO_POP  (BIT(16))
#define I2S_INFIFO_POP_M  (BIT(16))
#define I2S_INFIFO_POP_V  0x1
#define I2S_INFIFO_POP_S  16
/* I2S_INFIFO_RDATA : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define I2S_INFIFO_RDATA  0x00000FFF
#define I2S_INFIFO_RDATA_M  ((I2S_INFIFO_RDATA_V)<<(I2S_INFIFO_RDATA_S))
#define I2S_INFIFO_RDATA_V  0xFFF
#define I2S_INFIFO_RDATA_S  0

#define I2S_LC_STATE0_REG(i)          (REG_I2S_BASE(i) + 0x006C)
/* I2S_LC_STATE0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define I2S_LC_STATE0  0xFFFFFFFF
#define I2S_LC_STATE0_M  ((I2S_LC_STATE0_V)<<(I2S_LC_STATE0_S))
#define I2S_LC_STATE0_V  0xFFFFFFFF
#define I2S_LC_STATE0_S  0

#define I2S_LC_STATE1_REG(i)          (REG_I2S_BASE(i) + 0x0070)
/* I2S_LC_STATE1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define I2S_LC_STATE1  0xFFFFFFFF
#define I2S_LC_STATE1_M  ((I2S_LC_STATE1_V)<<(I2S_LC_STATE1_S))
#define I2S_LC_STATE1_V  0xFFFFFFFF
#define I2S_LC_STATE1_S  0

#define I2S_LC_HUNG_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0074)
/* I2S_LC_FIFO_TIMEOUT_ENA : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define I2S_LC_FIFO_TIMEOUT_ENA  (BIT(11))
#define I2S_LC_FIFO_TIMEOUT_ENA_M  (BIT(11))
#define I2S_LC_FIFO_TIMEOUT_ENA_V  0x1
#define I2S_LC_FIFO_TIMEOUT_ENA_S  11
/* I2S_LC_FIFO_TIMEOUT_SHIFT : R/W ;bitpos:[10:8] ;default: 3'b0 ; */
/*description: */
#define I2S_LC_FIFO_TIMEOUT_SHIFT  0x00000007
#define I2S_LC_FIFO_TIMEOUT_SHIFT_M  ((I2S_LC_FIFO_TIMEOUT_SHIFT_V)<<(I2S_LC_FIFO_TIMEOUT_SHIFT_S))
#define I2S_LC_FIFO_TIMEOUT_SHIFT_V  0x7
#define I2S_LC_FIFO_TIMEOUT_SHIFT_S  8
/* I2S_LC_FIFO_TIMEOUT : R/W ;bitpos:[7:0] ;default: 8'h10 ; */
/*description: */
#define I2S_LC_FIFO_TIMEOUT  0x000000FF
#define I2S_LC_FIFO_TIMEOUT_M  ((I2S_LC_FIFO_TIMEOUT_V)<<(I2S_LC_FIFO_TIMEOUT_S))
#define I2S_LC_FIFO_TIMEOUT_V  0xFF
#define I2S_LC_FIFO_TIMEOUT_S  0

#define I2S_CVSD_CONF0_REG(i)          (REG_I2S_BASE(i) + 0x0080)
/* I2S_CVSD_Y_MIN : R/W ;bitpos:[31:16] ;default: 16'h8000 ; */
/*description: */
#define I2S_CVSD_Y_MIN  0x0000FFFF
#define I2S_CVSD_Y_MIN_M  ((I2S_CVSD_Y_MIN_V)<<(I2S_CVSD_Y_MIN_S))
#define I2S_CVSD_Y_MIN_V  0xFFFF
#define I2S_CVSD_Y_MIN_S  16
/* I2S_CVSD_Y_MAX : R/W ;bitpos:[15:0] ;default: 16'h7fff ; */
/*description: */
#define I2S_CVSD_Y_MAX  0x0000FFFF
#define I2S_CVSD_Y_MAX_M  ((I2S_CVSD_Y_MAX_V)<<(I2S_CVSD_Y_MAX_S))
#define I2S_CVSD_Y_MAX_V  0xFFFF
#define I2S_CVSD_Y_MAX_S  0

#define I2S_CVSD_CONF1_REG(i)          (REG_I2S_BASE(i) + 0x0084)
/* I2S_CVSD_SIGMA_MIN : R/W ;bitpos:[31:16] ;default: 16'd10 ; */
/*description: */
#define I2S_CVSD_SIGMA_MIN  0x0000FFFF
#define I2S_CVSD_SIGMA_MIN_M  ((I2S_CVSD_SIGMA_MIN_V)<<(I2S_CVSD_SIGMA_MIN_S))
#define I2S_CVSD_SIGMA_MIN_V  0xFFFF
#define I2S_CVSD_SIGMA_MIN_S  16
/* I2S_CVSD_SIGMA_MAX : R/W ;bitpos:[15:0] ;default: 16'd1280 ; */
/*description: */
#define I2S_CVSD_SIGMA_MAX  0x0000FFFF
#define I2S_CVSD_SIGMA_MAX_M  ((I2S_CVSD_SIGMA_MAX_V)<<(I2S_CVSD_SIGMA_MAX_S))
#define I2S_CVSD_SIGMA_MAX_V  0xFFFF
#define I2S_CVSD_SIGMA_MAX_S  0

#define I2S_CVSD_CONF2_REG(i)          (REG_I2S_BASE(i) + 0x0088)
/* I2S_CVSD_H : R/W ;bitpos:[18:16] ;default: 3'd5 ; */
/*description: */
#define I2S_CVSD_H  0x00000007
#define I2S_CVSD_H_M  ((I2S_CVSD_H_V)<<(I2S_CVSD_H_S))
#define I2S_CVSD_H_V  0x7
#define I2S_CVSD_H_S  16
/* I2S_CVSD_BETA : R/W ;bitpos:[15:6] ;default: 10'd10 ; */
/*description: */
#define I2S_CVSD_BETA  0x000003FF
#define I2S_CVSD_BETA_M  ((I2S_CVSD_BETA_V)<<(I2S_CVSD_BETA_S))
#define I2S_CVSD_BETA_V  0x3FF
#define I2S_CVSD_BETA_S  6
/* I2S_CVSD_J : R/W ;bitpos:[5:3] ;default: 3'h4 ; */
/*description: */
#define I2S_CVSD_J  0x00000007
#define I2S_CVSD_J_M  ((I2S_CVSD_J_V)<<(I2S_CVSD_J_S))
#define I2S_CVSD_J_V  0x7
#define I2S_CVSD_J_S  3
/* I2S_CVSD_K : R/W ;bitpos:[2:0] ;default: 3'h4 ; */
/*description: */
#define I2S_CVSD_K  0x00000007
#define I2S_CVSD_K_M  ((I2S_CVSD_K_V)<<(I2S_CVSD_K_S))
#define I2S_CVSD_K_V  0x7
#define I2S_CVSD_K_S  0

#define I2S_PLC_CONF0_REG(i)          (REG_I2S_BASE(i) + 0x008C)
/* I2S_N_MIN_ERR : R/W ;bitpos:[27:25] ;default: 3'd4 ; */
/*description: */
#define I2S_N_MIN_ERR  0x00000007
#define I2S_N_MIN_ERR_M  ((I2S_N_MIN_ERR_V)<<(I2S_N_MIN_ERR_S))
#define I2S_N_MIN_ERR_V  0x7
#define I2S_N_MIN_ERR_S  25
/* I2S_PACK_LEN_8K : R/W ;bitpos:[24:20] ;default: 5'd10 ; */
/*description: */
#define I2S_PACK_LEN_8K  0x0000001F
#define I2S_PACK_LEN_8K_M  ((I2S_PACK_LEN_8K_V)<<(I2S_PACK_LEN_8K_S))
#define I2S_PACK_LEN_8K_V  0x1F
#define I2S_PACK_LEN_8K_S  20
/* I2S_MAX_SLIDE_SAMPLE : R/W ;bitpos:[19:12] ;default: 8'd128 ; */
/*description: */
#define I2S_MAX_SLIDE_SAMPLE  0x000000FF
#define I2S_MAX_SLIDE_SAMPLE_M  ((I2S_MAX_SLIDE_SAMPLE_V)<<(I2S_MAX_SLIDE_SAMPLE_S))
#define I2S_MAX_SLIDE_SAMPLE_V  0xFF
#define I2S_MAX_SLIDE_SAMPLE_S  12
/* I2S_SHIFT_RATE : R/W ;bitpos:[11:9] ;default: 3'h1 ; */
/*description: */
#define I2S_SHIFT_RATE  0x00000007
#define I2S_SHIFT_RATE_M  ((I2S_SHIFT_RATE_V)<<(I2S_SHIFT_RATE_S))
#define I2S_SHIFT_RATE_V  0x7
#define I2S_SHIFT_RATE_S  9
/* I2S_N_ERR_SEG : R/W ;bitpos:[8:6] ;default: 3'h4 ; */
/*description: */
#define I2S_N_ERR_SEG  0x00000007
#define I2S_N_ERR_SEG_M  ((I2S_N_ERR_SEG_V)<<(I2S_N_ERR_SEG_S))
#define I2S_N_ERR_SEG_V  0x7
#define I2S_N_ERR_SEG_S  6
/* I2S_GOOD_PACK_MAX : R/W ;bitpos:[5:0] ;default: 6'h39 ; */
/*description: */
#define I2S_GOOD_PACK_MAX  0x0000003F
#define I2S_GOOD_PACK_MAX_M  ((I2S_GOOD_PACK_MAX_V)<<(I2S_GOOD_PACK_MAX_S))
#define I2S_GOOD_PACK_MAX_V  0x3F
#define I2S_GOOD_PACK_MAX_S  0

#define I2S_PLC_CONF1_REG(i)          (REG_I2S_BASE(i) + 0x0090)
/* I2S_SLIDE_WIN_LEN : R/W ;bitpos:[31:24] ;default: 8'd160 ; */
/*description: */
#define I2S_SLIDE_WIN_LEN  0x000000FF
#define I2S_SLIDE_WIN_LEN_M  ((I2S_SLIDE_WIN_LEN_V)<<(I2S_SLIDE_WIN_LEN_S))
#define I2S_SLIDE_WIN_LEN_V  0xFF
#define I2S_SLIDE_WIN_LEN_S  24
/* I2S_BAD_OLA_WIN2_PARA : R/W ;bitpos:[23:16] ;default: 8'd23 ; */
/*description: */
#define I2S_BAD_OLA_WIN2_PARA  0x000000FF
#define I2S_BAD_OLA_WIN2_PARA_M  ((I2S_BAD_OLA_WIN2_PARA_V)<<(I2S_BAD_OLA_WIN2_PARA_S))
#define I2S_BAD_OLA_WIN2_PARA_V  0xFF
#define I2S_BAD_OLA_WIN2_PARA_S  16
/* I2S_BAD_OLA_WIN2_PARA_SHIFT : R/W ;bitpos:[15:12] ;default: 4'd8 ; */
/*description: */
#define I2S_BAD_OLA_WIN2_PARA_SHIFT  0x0000000F
#define I2S_BAD_OLA_WIN2_PARA_SHIFT_M  ((I2S_BAD_OLA_WIN2_PARA_SHIFT_V)<<(I2S_BAD_OLA_WIN2_PARA_SHIFT_S))
#define I2S_BAD_OLA_WIN2_PARA_SHIFT_V  0xF
#define I2S_BAD_OLA_WIN2_PARA_SHIFT_S  12
/* I2S_BAD_CEF_ATTEN_PARA_SHIFT : R/W ;bitpos:[11:8] ;default: 4'd10 ; */
/*description: */
#define I2S_BAD_CEF_ATTEN_PARA_SHIFT  0x0000000F
#define I2S_BAD_CEF_ATTEN_PARA_SHIFT_M  ((I2S_BAD_CEF_ATTEN_PARA_SHIFT_V)<<(I2S_BAD_CEF_ATTEN_PARA_SHIFT_S))
#define I2S_BAD_CEF_ATTEN_PARA_SHIFT_V  0xF
#define I2S_BAD_CEF_ATTEN_PARA_SHIFT_S  8
/* I2S_BAD_CEF_ATTEN_PARA : R/W ;bitpos:[7:0] ;default: 8'd5 ; */
/*description: */
#define I2S_BAD_CEF_ATTEN_PARA  0x000000FF
#define I2S_BAD_CEF_ATTEN_PARA_M  ((I2S_BAD_CEF_ATTEN_PARA_V)<<(I2S_BAD_CEF_ATTEN_PARA_S))
#define I2S_BAD_CEF_ATTEN_PARA_V  0xFF
#define I2S_BAD_CEF_ATTEN_PARA_S  0

#define I2S_PLC_CONF2_REG(i)          (REG_I2S_BASE(i) + 0x0094)
/* I2S_MIN_PERIOD : R/W ;bitpos:[6:2] ;default: 5'd10 ; */
/*description: */
#define I2S_MIN_PERIOD  0x0000001F
#define I2S_MIN_PERIOD_M  ((I2S_MIN_PERIOD_V)<<(I2S_MIN_PERIOD_S))
#define I2S_MIN_PERIOD_V  0x1F
#define I2S_MIN_PERIOD_S  2
/* I2S_CVSD_SEG_MOD : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define I2S_CVSD_SEG_MOD  0x00000003
#define I2S_CVSD_SEG_MOD_M  ((I2S_CVSD_SEG_MOD_V)<<(I2S_CVSD_SEG_MOD_S))
#define I2S_CVSD_SEG_MOD_V  0x3
#define I2S_CVSD_SEG_MOD_S  0

#define I2S_ESCO_CONF0_REG(i)          (REG_I2S_BASE(i) + 0x0098)
/* I2S_PLC2DMA_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define I2S_PLC2DMA_EN  (BIT(12))
#define I2S_PLC2DMA_EN_M  (BIT(12))
#define I2S_PLC2DMA_EN_V  0x1
#define I2S_PLC2DMA_EN_S  12
/* I2S_PLC_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define I2S_PLC_EN  (BIT(11))
#define I2S_PLC_EN_M  (BIT(11))
#define I2S_PLC_EN_V  0x1
#define I2S_PLC_EN_S  11
/* I2S_CVSD_DEC_RESET : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_CVSD_DEC_RESET  (BIT(10))
#define I2S_CVSD_DEC_RESET_M  (BIT(10))
#define I2S_CVSD_DEC_RESET_V  0x1
#define I2S_CVSD_DEC_RESET_S  10
/* I2S_CVSD_DEC_START : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define I2S_CVSD_DEC_START  (BIT(9))
#define I2S_CVSD_DEC_START_M  (BIT(9))
#define I2S_CVSD_DEC_START_V  0x1
#define I2S_CVSD_DEC_START_S  9
/* I2S_ESCO_CVSD_INF_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define I2S_ESCO_CVSD_INF_EN  (BIT(8))
#define I2S_ESCO_CVSD_INF_EN_M  (BIT(8))
#define I2S_ESCO_CVSD_INF_EN_V  0x1
#define I2S_ESCO_CVSD_INF_EN_S  8
/* I2S_ESCO_CVSD_PACK_LEN_8K : R/W ;bitpos:[7:3] ;default: 5'b0 ; */
/*description: */
#define I2S_ESCO_CVSD_PACK_LEN_8K  0x0000001F
#define I2S_ESCO_CVSD_PACK_LEN_8K_M  ((I2S_ESCO_CVSD_PACK_LEN_8K_V)<<(I2S_ESCO_CVSD_PACK_LEN_8K_S))
#define I2S_ESCO_CVSD_PACK_LEN_8K_V  0x1F
#define I2S_ESCO_CVSD_PACK_LEN_8K_S  3
/* I2S_ESCO_CVSD_DEC_PACK_ERR : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define I2S_ESCO_CVSD_DEC_PACK_ERR  (BIT(2))
#define I2S_ESCO_CVSD_DEC_PACK_ERR_M  (BIT(2))
#define I2S_ESCO_CVSD_DEC_PACK_ERR_V  0x1
#define I2S_ESCO_CVSD_DEC_PACK_ERR_S  2
/* I2S_ESCO_CHAN_MOD : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define I2S_ESCO_CHAN_MOD  (BIT(1))
#define I2S_ESCO_CHAN_MOD_M  (BIT(1))
#define I2S_ESCO_CHAN_MOD_V  0x1
#define I2S_ESCO_CHAN_MOD_S  1
/* I2S_ESCO_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define I2S_ESCO_EN  (BIT(0))
#define I2S_ESCO_EN_M  (BIT(0))
#define I2S_ESCO_EN_V  0x1
#define I2S_ESCO_EN_S  0

#define I2S_SCO_CONF0_REG(i)          (REG_I2S_BASE(i) + 0x009c)
/* I2S_CVSD_ENC_RESET : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: */
#define I2S_CVSD_ENC_RESET  (BIT(3))
#define I2S_CVSD_ENC_RESET_M  (BIT(3))
#define I2S_CVSD_ENC_RESET_V  0x1
#define I2S_CVSD_ENC_RESET_S  3
/* I2S_CVSD_ENC_START : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: */
#define I2S_CVSD_ENC_START  (BIT(2))
#define I2S_CVSD_ENC_START_M  (BIT(2))
#define I2S_CVSD_ENC_START_V  0x1
#define I2S_CVSD_ENC_START_S  2
/* I2S_SCO_NO_I2S_EN : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define I2S_SCO_NO_I2S_EN  (BIT(1))
#define I2S_SCO_NO_I2S_EN_M  (BIT(1))
#define I2S_SCO_NO_I2S_EN_V  0x1
#define I2S_SCO_NO_I2S_EN_S  1
/* I2S_SCO_WITH_I2S_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define I2S_SCO_WITH_I2S_EN  (BIT(0))
#define I2S_SCO_WITH_I2S_EN_M  (BIT(0))
#define I2S_SCO_WITH_I2S_EN_V  0x1
#define I2S_SCO_WITH_I2S_EN_S  0

#define I2S_CONF1_REG(i)          (REG_I2S_BASE(i) + 0x00a0)
/* I2S_TX_ZEROS_RM_EN : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define I2S_TX_ZEROS_RM_EN  (BIT(9))
#define I2S_TX_ZEROS_RM_EN_M  (BIT(9))
#define I2S_TX_ZEROS_RM_EN_V  0x1
#define I2S_TX_ZEROS_RM_EN_S  9
/* I2S_TX_STOP_EN : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define I2S_TX_STOP_EN  (BIT(8))
#define I2S_TX_STOP_EN_M  (BIT(8))
#define I2S_TX_STOP_EN_V  0x1
#define I2S_TX_STOP_EN_S  8
/* I2S_RX_PCM_BYPASS : R/W ;bitpos:[7] ;default: 1'h1 ; */
/*description: */
#define I2S_RX_PCM_BYPASS  (BIT(7))
#define I2S_RX_PCM_BYPASS_M  (BIT(7))
#define I2S_RX_PCM_BYPASS_V  0x1
#define I2S_RX_PCM_BYPASS_S  7
/* I2S_RX_PCM_CONF : R/W ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: */
#define I2S_RX_PCM_CONF  0x00000007
#define I2S_RX_PCM_CONF_M  ((I2S_RX_PCM_CONF_V)<<(I2S_RX_PCM_CONF_S))
#define I2S_RX_PCM_CONF_V  0x7
#define I2S_RX_PCM_CONF_S  4
/* I2S_TX_PCM_BYPASS : R/W ;bitpos:[3] ;default: 1'h1 ; */
/*description: */
#define I2S_TX_PCM_BYPASS  (BIT(3))
#define I2S_TX_PCM_BYPASS_M  (BIT(3))
#define I2S_TX_PCM_BYPASS_V  0x1
#define I2S_TX_PCM_BYPASS_S  3
/* I2S_TX_PCM_CONF : R/W ;bitpos:[2:0] ;default: 3'h1 ; */
/*description: */
#define I2S_TX_PCM_CONF  0x00000007
#define I2S_TX_PCM_CONF_M  ((I2S_TX_PCM_CONF_V)<<(I2S_TX_PCM_CONF_S))
#define I2S_TX_PCM_CONF_V  0x7
#define I2S_TX_PCM_CONF_S  0

#define I2S_PD_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00a4)
/* I2S_PLC_MEM_FORCE_PU : R/W ;bitpos:[3] ;default: 1'h1 ; */
/*description: */
#define I2S_PLC_MEM_FORCE_PU  (BIT(3))
#define I2S_PLC_MEM_FORCE_PU_M  (BIT(3))
#define I2S_PLC_MEM_FORCE_PU_V  0x1
#define I2S_PLC_MEM_FORCE_PU_S  3
/* I2S_PLC_MEM_FORCE_PD : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define I2S_PLC_MEM_FORCE_PD  (BIT(2))
#define I2S_PLC_MEM_FORCE_PD_M  (BIT(2))
#define I2S_PLC_MEM_FORCE_PD_V  0x1
#define I2S_PLC_MEM_FORCE_PD_S  2
/* I2S_FIFO_FORCE_PU : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: */
#define I2S_FIFO_FORCE_PU  (BIT(1))
#define I2S_FIFO_FORCE_PU_M  (BIT(1))
#define I2S_FIFO_FORCE_PU_V  0x1
#define I2S_FIFO_FORCE_PU_S  1
/* I2S_FIFO_FORCE_PD : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define I2S_FIFO_FORCE_PD  (BIT(0))
#define I2S_FIFO_FORCE_PD_M  (BIT(0))
#define I2S_FIFO_FORCE_PD_V  0x1
#define I2S_FIFO_FORCE_PD_S  0

#define I2S_CONF2_REG(i)          (REG_I2S_BASE(i) + 0x00a8)
/* I2S_INTER_VALID_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define I2S_INTER_VALID_EN  (BIT(7))
#define I2S_INTER_VALID_EN_M  (BIT(7))
#define I2S_INTER_VALID_EN_V  0x1
#define I2S_INTER_VALID_EN_S  7
/* I2S_EXT_ADC_START_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define I2S_EXT_ADC_START_EN  (BIT(6))
#define I2S_EXT_ADC_START_EN_M  (BIT(6))
#define I2S_EXT_ADC_START_EN_V  0x1
#define I2S_EXT_ADC_START_EN_S  6
/* I2S_LCD_EN : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define I2S_LCD_EN  (BIT(5))
#define I2S_LCD_EN_M  (BIT(5))
#define I2S_LCD_EN_V  0x1
#define I2S_LCD_EN_S  5
/* I2S_DATA_ENABLE : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: */
#define I2S_DATA_ENABLE  (BIT(4))
#define I2S_DATA_ENABLE_M  (BIT(4))
#define I2S_DATA_ENABLE_V  0x1
#define I2S_DATA_ENABLE_S  4
/* I2S_DATA_ENABLE_TEST_EN : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: */
#define I2S_DATA_ENABLE_TEST_EN  (BIT(3))
#define I2S_DATA_ENABLE_TEST_EN_M  (BIT(3))
#define I2S_DATA_ENABLE_TEST_EN_V  0x1
#define I2S_DATA_ENABLE_TEST_EN_S  3
/* I2S_LCD_TX_SDX2_EN : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define I2S_LCD_TX_SDX2_EN  (BIT(2))
#define I2S_LCD_TX_SDX2_EN_M  (BIT(2))
#define I2S_LCD_TX_SDX2_EN_V  0x1
#define I2S_LCD_TX_SDX2_EN_S  2
/* I2S_LCD_TX_WRX2_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define I2S_LCD_TX_WRX2_EN  (BIT(1))
#define I2S_LCD_TX_WRX2_EN_M  (BIT(1))
#define I2S_LCD_TX_WRX2_EN_V  0x1
#define I2S_LCD_TX_WRX2_EN_S  1
/* I2S_CAMERA_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define I2S_CAMERA_EN  (BIT(0))
#define I2S_CAMERA_EN_M  (BIT(0))
#define I2S_CAMERA_EN_V  0x1
#define I2S_CAMERA_EN_S  0

#define I2S_CLKM_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00ac)
/* I2S_CLKA_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define I2S_CLKA_ENA  (BIT(21))
#define I2S_CLKA_ENA_M  (BIT(21))
#define I2S_CLKA_ENA_V  0x1
#define I2S_CLKA_ENA_S  21
/* I2S_CLK_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define I2S_CLK_EN  (BIT(20))
#define I2S_CLK_EN_M  (BIT(20))
#define I2S_CLK_EN_V  0x1
#define I2S_CLK_EN_S  20
/* I2S_CLKM_DIV_A : R/W ;bitpos:[19:14] ;default: 6'h0 ; */
/*description: */
#define I2S_CLKM_DIV_A  0x0000003F
#define I2S_CLKM_DIV_A_M  ((I2S_CLKM_DIV_A_V)<<(I2S_CLKM_DIV_A_S))
#define I2S_CLKM_DIV_A_V  0x3F
#define I2S_CLKM_DIV_A_S  14
/* I2S_CLKM_DIV_B : R/W ;bitpos:[13:8] ;default: 6'h0 ; */
/*description: */
#define I2S_CLKM_DIV_B  0x0000003F
#define I2S_CLKM_DIV_B_M  ((I2S_CLKM_DIV_B_V)<<(I2S_CLKM_DIV_B_S))
#define I2S_CLKM_DIV_B_V  0x3F
#define I2S_CLKM_DIV_B_S  8
/* I2S_CLKM_DIV_NUM : R/W ;bitpos:[7:0] ;default: 8'd4 ; */
/*description: */
#define I2S_CLKM_DIV_NUM  0x000000FF
#define I2S_CLKM_DIV_NUM_M  ((I2S_CLKM_DIV_NUM_V)<<(I2S_CLKM_DIV_NUM_S))
#define I2S_CLKM_DIV_NUM_V  0xFF
#define I2S_CLKM_DIV_NUM_S  0

#define I2S_SAMPLE_RATE_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00b0)
/* I2S_RX_BITS_MOD : R/W ;bitpos:[23:18] ;default: 6'd16 ; */
/*description: */
#define I2S_RX_BITS_MOD  0x0000003F
#define I2S_RX_BITS_MOD_M  ((I2S_RX_BITS_MOD_V)<<(I2S_RX_BITS_MOD_S))
#define I2S_RX_BITS_MOD_V  0x3F
#define I2S_RX_BITS_MOD_S  18
/* I2S_TX_BITS_MOD : R/W ;bitpos:[17:12] ;default: 6'd16 ; */
/*description: */
#define I2S_TX_BITS_MOD  0x0000003F
#define I2S_TX_BITS_MOD_M  ((I2S_TX_BITS_MOD_V)<<(I2S_TX_BITS_MOD_S))
#define I2S_TX_BITS_MOD_V  0x3F
#define I2S_TX_BITS_MOD_S  12
/* I2S_RX_BCK_DIV_NUM : R/W ;bitpos:[11:6] ;default: 6'd6 ; */
/*description: */
#define I2S_RX_BCK_DIV_NUM  0x0000003F
#define I2S_RX_BCK_DIV_NUM_M  ((I2S_RX_BCK_DIV_NUM_V)<<(I2S_RX_BCK_DIV_NUM_S))
#define I2S_RX_BCK_DIV_NUM_V  0x3F
#define I2S_RX_BCK_DIV_NUM_S  6
/* I2S_TX_BCK_DIV_NUM : R/W ;bitpos:[5:0] ;default: 6'd6 ; */
/*description: */
#define I2S_TX_BCK_DIV_NUM  0x0000003F
#define I2S_TX_BCK_DIV_NUM_M  ((I2S_TX_BCK_DIV_NUM_V)<<(I2S_TX_BCK_DIV_NUM_S))
#define I2S_TX_BCK_DIV_NUM_V  0x3F
#define I2S_TX_BCK_DIV_NUM_S  0

#define I2S_PDM_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00b4)
/* I2S_TX_PDM_HP_BYPASS : R/W ;bitpos:[25] ;default: 1'h0 ; */
/*description: */
#define I2S_TX_PDM_HP_BYPASS  (BIT(25))
#define I2S_TX_PDM_HP_BYPASS_M  (BIT(25))
#define I2S_TX_PDM_HP_BYPASS_V  0x1
#define I2S_TX_PDM_HP_BYPASS_S  25
/* I2S_RX_PDM_SINC_DSR_16_EN : R/W ;bitpos:[24] ;default: 1'h1 ; */
/*description: */
#define I2S_RX_PDM_SINC_DSR_16_EN  (BIT(24))
#define I2S_RX_PDM_SINC_DSR_16_EN_M  (BIT(24))
#define I2S_RX_PDM_SINC_DSR_16_EN_V  0x1
#define I2S_RX_PDM_SINC_DSR_16_EN_S  24
/* I2S_TX_PDM_SIGMADELTA_IN_SHIFT : R/W ;bitpos:[23:22] ;default: 2'h1 ; */
/*description: */
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT  0x00000003
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_M  ((I2S_TX_PDM_SIGMADELTA_IN_SHIFT_V)<<(I2S_TX_PDM_SIGMADELTA_IN_SHIFT_S))
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_V  0x3
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_S  22
/* I2S_TX_PDM_SINC_IN_SHIFT : R/W ;bitpos:[21:20] ;default: 2'h1 ; */
/*description: */
#define I2S_TX_PDM_SINC_IN_SHIFT  0x00000003
#define I2S_TX_PDM_SINC_IN_SHIFT_M  ((I2S_TX_PDM_SINC_IN_SHIFT_V)<<(I2S_TX_PDM_SINC_IN_SHIFT_S))
#define I2S_TX_PDM_SINC_IN_SHIFT_V  0x3
#define I2S_TX_PDM_SINC_IN_SHIFT_S  20
/* I2S_TX_PDM_LP_IN_SHIFT : R/W ;bitpos:[19:18] ;default: 2'h1 ; */
/*description: */
#define I2S_TX_PDM_LP_IN_SHIFT  0x00000003
#define I2S_TX_PDM_LP_IN_SHIFT_M  ((I2S_TX_PDM_LP_IN_SHIFT_V)<<(I2S_TX_PDM_LP_IN_SHIFT_S))
#define I2S_TX_PDM_LP_IN_SHIFT_V  0x3
#define I2S_TX_PDM_LP_IN_SHIFT_S  18
/* I2S_TX_PDM_HP_IN_SHIFT : R/W ;bitpos:[17:16] ;default: 2'h1 ; */
/*description: */
#define I2S_TX_PDM_HP_IN_SHIFT  0x00000003
#define I2S_TX_PDM_HP_IN_SHIFT_M  ((I2S_TX_PDM_HP_IN_SHIFT_V)<<(I2S_TX_PDM_HP_IN_SHIFT_S))
#define I2S_TX_PDM_HP_IN_SHIFT_V  0x3
#define I2S_TX_PDM_HP_IN_SHIFT_S  16
/* I2S_TX_PDM_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define I2S_TX_PDM_PRESCALE  0x000000FF
#define I2S_TX_PDM_PRESCALE_M  ((I2S_TX_PDM_PRESCALE_V)<<(I2S_TX_PDM_PRESCALE_S))
#define I2S_TX_PDM_PRESCALE_V  0xFF
#define I2S_TX_PDM_PRESCALE_S  8
/* I2S_TX_PDM_SINC_OSR2 : R/W ;bitpos:[7:4] ;default: 4'h2 ; */
/*description: */
#define I2S_TX_PDM_SINC_OSR2  0x0000000F
#define I2S_TX_PDM_SINC_OSR2_M  ((I2S_TX_PDM_SINC_OSR2_V)<<(I2S_TX_PDM_SINC_OSR2_S))
#define I2S_TX_PDM_SINC_OSR2_V  0xF
#define I2S_TX_PDM_SINC_OSR2_S  4
/* I2S_PDM2PCM_CONV_EN : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: */
#define I2S_PDM2PCM_CONV_EN  (BIT(3))
#define I2S_PDM2PCM_CONV_EN_M  (BIT(3))
#define I2S_PDM2PCM_CONV_EN_V  0x1
#define I2S_PDM2PCM_CONV_EN_S  3
/* I2S_PCM2PDM_CONV_EN : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define I2S_PCM2PDM_CONV_EN  (BIT(2))
#define I2S_PCM2PDM_CONV_EN_M  (BIT(2))
#define I2S_PCM2PDM_CONV_EN_V  0x1
#define I2S_PCM2PDM_CONV_EN_S  2
/* I2S_RX_PDM_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define I2S_RX_PDM_EN  (BIT(1))
#define I2S_RX_PDM_EN_M  (BIT(1))
#define I2S_RX_PDM_EN_V  0x1
#define I2S_RX_PDM_EN_S  1
/* I2S_TX_PDM_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define I2S_TX_PDM_EN  (BIT(0))
#define I2S_TX_PDM_EN_M  (BIT(0))
#define I2S_TX_PDM_EN_V  0x1
#define I2S_TX_PDM_EN_S  0

#define I2S_PDM_FREQ_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00b8)
/* I2S_TX_PDM_FP : R/W ;bitpos:[19:10] ;default: 10'd960 ; */
/*description: */
#define I2S_TX_PDM_FP  0x000003FF
#define I2S_TX_PDM_FP_M  ((I2S_TX_PDM_FP_V)<<(I2S_TX_PDM_FP_S))
#define I2S_TX_PDM_FP_V  0x3FF
#define I2S_TX_PDM_FP_S  10
/* I2S_TX_PDM_FS : R/W ;bitpos:[9:0] ;default: 10'd480 ; */
/*description: */
#define I2S_TX_PDM_FS  0x000003FF
#define I2S_TX_PDM_FS_M  ((I2S_TX_PDM_FS_V)<<(I2S_TX_PDM_FS_S))
#define I2S_TX_PDM_FS_V  0x3FF
#define I2S_TX_PDM_FS_S  0

#define I2S_STATE_REG(i)          (REG_I2S_BASE(i) + 0x00bc)
/* I2S_RX_FIFO_RESET_BACK : RO ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define I2S_RX_FIFO_RESET_BACK  (BIT(2))
#define I2S_RX_FIFO_RESET_BACK_M  (BIT(2))
#define I2S_RX_FIFO_RESET_BACK_V  0x1
#define I2S_RX_FIFO_RESET_BACK_S  2
/* I2S_TX_FIFO_RESET_BACK : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define I2S_TX_FIFO_RESET_BACK  (BIT(1))
#define I2S_TX_FIFO_RESET_BACK_M  (BIT(1))
#define I2S_TX_FIFO_RESET_BACK_V  0x1
#define I2S_TX_FIFO_RESET_BACK_S  1
/* I2S_TX_IDLE : RO ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define I2S_TX_IDLE  (BIT(0))
#define I2S_TX_IDLE_M  (BIT(0))
#define I2S_TX_IDLE_V  0x1
#define I2S_TX_IDLE_S  0

#define I2S_DATE_REG(i)          (REG_I2S_BASE(i) + 0x00fc)
/* I2S_I2SDATE : R/W ;bitpos:[31:0] ;default: 32'h1604201 ; */
/*description: */
#define I2S_I2SDATE  0xFFFFFFFF
#define I2S_I2SDATE_M  ((I2S_I2SDATE_V)<<(I2S_I2SDATE_S))
#define I2S_I2SDATE_V  0xFFFFFFFF
#define I2S_I2SDATE_S  0




#endif /*_SOC_I2S_REG_H_ */
