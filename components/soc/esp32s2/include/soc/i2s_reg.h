/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_I2S_REG_H_
#define _SOC_I2S_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define I2S_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0008)
/* I2S_RX_RESET_ST : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_RESET_ST  (BIT(29))
#define I2S_RX_RESET_ST_M  (BIT(29))
#define I2S_RX_RESET_ST_V  0x1
#define I2S_RX_RESET_ST_S  29
/* I2S_RX_BIG_ENDIAN : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define I2S_RX_BIG_ENDIAN  (BIT(28))
#define I2S_RX_BIG_ENDIAN_M  (BIT(28))
#define I2S_RX_BIG_ENDIAN_V  0x1
#define I2S_RX_BIG_ENDIAN_S  28
/* I2S_TX_BIG_ENDIAN : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define I2S_TX_BIG_ENDIAN  (BIT(27))
#define I2S_TX_BIG_ENDIAN_M  (BIT(27))
#define I2S_TX_BIG_ENDIAN_V  0x1
#define I2S_TX_BIG_ENDIAN_S  27
/* I2S_PRE_REQ_EN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: set this bit to enable i2s to prepare data earlier*/
#define I2S_PRE_REQ_EN  (BIT(26))
#define I2S_PRE_REQ_EN_M  (BIT(26))
#define I2S_PRE_REQ_EN_V  0x1
#define I2S_PRE_REQ_EN_S  26
/* I2S_RX_DMA_EQUAL : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1:data in left channel is equal to data in right channel*/
#define I2S_RX_DMA_EQUAL  (BIT(25))
#define I2S_RX_DMA_EQUAL_M  (BIT(25))
#define I2S_RX_DMA_EQUAL_V  0x1
#define I2S_RX_DMA_EQUAL_S  25
/* I2S_TX_DMA_EQUAL : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: 1:data in left channel is equal to data in right channel*/
#define I2S_TX_DMA_EQUAL  (BIT(24))
#define I2S_TX_DMA_EQUAL_M  (BIT(24))
#define I2S_TX_DMA_EQUAL_V  0x1
#define I2S_TX_DMA_EQUAL_S  24
/* I2S_TX_RESET_ST : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: 1: i2s_tx_reset is not ok   0: i2s_tx_reset is ok*/
#define I2S_TX_RESET_ST  (BIT(23))
#define I2S_TX_RESET_ST_M  (BIT(23))
#define I2S_TX_RESET_ST_V  0x1
#define I2S_TX_RESET_ST_S  23
/* I2S_RX_FIFO_RESET_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: 1:i2s_rx_fifo_reset is not ok   0:i2s_rx_fifo reset is ok*/
#define I2S_RX_FIFO_RESET_ST  (BIT(22))
#define I2S_RX_FIFO_RESET_ST_M  (BIT(22))
#define I2S_RX_FIFO_RESET_ST_V  0x1
#define I2S_RX_FIFO_RESET_ST_S  22
/* I2S_TX_FIFO_RESET_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: 1:i2s_tx_fifo reset is not ok   0:i2s_tx_fifo_reset is ok*/
#define I2S_TX_FIFO_RESET_ST  (BIT(21))
#define I2S_TX_FIFO_RESET_ST_M  (BIT(21))
#define I2S_TX_FIFO_RESET_ST_V  0x1
#define I2S_TX_FIFO_RESET_ST_S  21
/* I2S_SIG_LOOPBACK : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Enable signal loopback mode with transmitter module and receiver
 module sharing the same WS and BCK signals.*/
#define I2S_SIG_LOOPBACK  (BIT(20))
#define I2S_SIG_LOOPBACK_M  (BIT(20))
#define I2S_SIG_LOOPBACK_V  0x1
#define I2S_SIG_LOOPBACK_S  20
/* I2S_RX_LSB_FIRST_DMA : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: 1:the data in DMA/APB transform from low bits*/
#define I2S_RX_LSB_FIRST_DMA  (BIT(19))
#define I2S_RX_LSB_FIRST_DMA_M  (BIT(19))
#define I2S_RX_LSB_FIRST_DMA_V  0x1
#define I2S_RX_LSB_FIRST_DMA_S  19
/* I2S_TX_LSB_FIRST_DMA : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: 1:the data in DMA/APB transform from low bits*/
#define I2S_TX_LSB_FIRST_DMA  (BIT(18))
#define I2S_TX_LSB_FIRST_DMA_M  (BIT(18))
#define I2S_TX_LSB_FIRST_DMA_V  0x1
#define I2S_TX_LSB_FIRST_DMA_S  18
/* I2S_RX_MSB_RIGHT : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to place right channel data at the MSB in the receive FIFO.*/
#define I2S_RX_MSB_RIGHT  (BIT(17))
#define I2S_RX_MSB_RIGHT_M  (BIT(17))
#define I2S_RX_MSB_RIGHT_V  0x1
#define I2S_RX_MSB_RIGHT_S  17
/* I2S_TX_MSB_RIGHT : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to place right channel data at the MSB in the transmit FIFO.*/
#define I2S_TX_MSB_RIGHT  (BIT(16))
#define I2S_TX_MSB_RIGHT_M  (BIT(16))
#define I2S_TX_MSB_RIGHT_V  0x1
#define I2S_TX_MSB_RIGHT_S  16
/* I2S_RX_MONO : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to enable receiver  in mono mode*/
#define I2S_RX_MONO  (BIT(15))
#define I2S_RX_MONO_M  (BIT(15))
#define I2S_RX_MONO_V  0x1
#define I2S_RX_MONO_S  15
/* I2S_TX_MONO : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to enable transmitter in mono mode*/
#define I2S_TX_MONO  (BIT(14))
#define I2S_TX_MONO_M  (BIT(14))
#define I2S_TX_MONO_V  0x1
#define I2S_TX_MONO_S  14
/* I2S_RX_SHORT_SYNC : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to enable receiver in PCM standard mode*/
#define I2S_RX_SHORT_SYNC  (BIT(13))
#define I2S_RX_SHORT_SYNC_M  (BIT(13))
#define I2S_RX_SHORT_SYNC_V  0x1
#define I2S_RX_SHORT_SYNC_S  13
/* I2S_TX_SHORT_SYNC : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable transmitter in PCM standard mode*/
#define I2S_TX_SHORT_SYNC  (BIT(12))
#define I2S_TX_SHORT_SYNC_M  (BIT(12))
#define I2S_TX_SHORT_SYNC_V  0x1
#define I2S_TX_SHORT_SYNC_S  12
/* I2S_RX_MSB_SHIFT : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to enable receiver in Phillips standard mode*/
#define I2S_RX_MSB_SHIFT  (BIT(11))
#define I2S_RX_MSB_SHIFT_M  (BIT(11))
#define I2S_RX_MSB_SHIFT_V  0x1
#define I2S_RX_MSB_SHIFT_S  11
/* I2S_TX_MSB_SHIFT : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to enable transmitter in Phillips standard mode*/
#define I2S_TX_MSB_SHIFT  (BIT(10))
#define I2S_TX_MSB_SHIFT_M  (BIT(10))
#define I2S_TX_MSB_SHIFT_V  0x1
#define I2S_TX_MSB_SHIFT_S  10
/* I2S_RX_RIGHT_FIRST : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: Set this bit to receive right channel data first*/
#define I2S_RX_RIGHT_FIRST  (BIT(9))
#define I2S_RX_RIGHT_FIRST_M  (BIT(9))
#define I2S_RX_RIGHT_FIRST_V  0x1
#define I2S_RX_RIGHT_FIRST_S  9
/* I2S_TX_RIGHT_FIRST : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: Set this bit to transmit right channel data first*/
#define I2S_TX_RIGHT_FIRST  (BIT(8))
#define I2S_TX_RIGHT_FIRST_M  (BIT(8))
#define I2S_TX_RIGHT_FIRST_V  0x1
#define I2S_TX_RIGHT_FIRST_S  8
/* I2S_RX_SLAVE_MOD : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to enable slave receiver mode*/
#define I2S_RX_SLAVE_MOD  (BIT(7))
#define I2S_RX_SLAVE_MOD_M  (BIT(7))
#define I2S_RX_SLAVE_MOD_V  0x1
#define I2S_RX_SLAVE_MOD_S  7
/* I2S_TX_SLAVE_MOD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to enable slave transmitter mode*/
#define I2S_TX_SLAVE_MOD  (BIT(6))
#define I2S_TX_SLAVE_MOD_M  (BIT(6))
#define I2S_TX_SLAVE_MOD_V  0x1
#define I2S_TX_SLAVE_MOD_S  6
/* I2S_RX_START : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to start receiving data*/
#define I2S_RX_START  (BIT(5))
#define I2S_RX_START_M  (BIT(5))
#define I2S_RX_START_V  0x1
#define I2S_RX_START_S  5
/* I2S_TX_START : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to start transmitting data*/
#define I2S_TX_START  (BIT(4))
#define I2S_TX_START_M  (BIT(4))
#define I2S_TX_START_V  0x1
#define I2S_TX_START_S  4
/* I2S_RX_FIFO_RESET : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset rxFIFO*/
#define I2S_RX_FIFO_RESET  (BIT(3))
#define I2S_RX_FIFO_RESET_M  (BIT(3))
#define I2S_RX_FIFO_RESET_V  0x1
#define I2S_RX_FIFO_RESET_S  3
/* I2S_TX_FIFO_RESET : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to reset txFIFO*/
#define I2S_TX_FIFO_RESET  (BIT(2))
#define I2S_TX_FIFO_RESET_M  (BIT(2))
#define I2S_TX_FIFO_RESET_V  0x1
#define I2S_TX_FIFO_RESET_S  2
/* I2S_RX_RESET : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to reset receiver*/
#define I2S_RX_RESET  (BIT(1))
#define I2S_RX_RESET_M  (BIT(1))
#define I2S_RX_RESET_V  0x1
#define I2S_RX_RESET_S  1
/* I2S_TX_RESET : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to reset transmitter*/
#define I2S_TX_RESET  (BIT(0))
#define I2S_TX_RESET_M  (BIT(0))
#define I2S_TX_RESET_V  0x1
#define I2S_TX_RESET_S  0

#define I2S_INT_RAW_REG(i)          (REG_I2S_BASE(i) + 0x000c)
/* I2S_V_SYNC_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_v_sync_int interrupt*/
#define I2S_V_SYNC_INT_RAW  (BIT(17))
#define I2S_V_SYNC_INT_RAW_M  (BIT(17))
#define I2S_V_SYNC_INT_RAW_V  0x1
#define I2S_V_SYNC_INT_RAW_S  17
/* I2S_OUT_TOTAL_EOF_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_out_total_eof_int interrupt*/
#define I2S_OUT_TOTAL_EOF_INT_RAW  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_RAW_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_RAW_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_RAW_S  16
/* I2S_IN_DSCR_EMPTY_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_in_dscr_empty_int interrupt*/
#define I2S_IN_DSCR_EMPTY_INT_RAW  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_RAW_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_RAW_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_RAW_S  15
/* I2S_OUT_DSCR_ERR_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_out_dscr_err_int interrupt*/
#define I2S_OUT_DSCR_ERR_INT_RAW  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_RAW_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_RAW_V  0x1
#define I2S_OUT_DSCR_ERR_INT_RAW_S  14
/* I2S_IN_DSCR_ERR_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_in_dscr_err_int interrupt*/
#define I2S_IN_DSCR_ERR_INT_RAW  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_RAW_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_RAW_V  0x1
#define I2S_IN_DSCR_ERR_INT_RAW_S  13
/* I2S_OUT_EOF_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_out_eof_int interrupt*/
#define I2S_OUT_EOF_INT_RAW  (BIT(12))
#define I2S_OUT_EOF_INT_RAW_M  (BIT(12))
#define I2S_OUT_EOF_INT_RAW_V  0x1
#define I2S_OUT_EOF_INT_RAW_S  12
/* I2S_OUT_DONE_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_out_done_int interrupt*/
#define I2S_OUT_DONE_INT_RAW  (BIT(11))
#define I2S_OUT_DONE_INT_RAW_M  (BIT(11))
#define I2S_OUT_DONE_INT_RAW_V  0x1
#define I2S_OUT_DONE_INT_RAW_S  11
/* I2S_IN_ERR_EOF_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: don't use*/
#define I2S_IN_ERR_EOF_INT_RAW  (BIT(10))
#define I2S_IN_ERR_EOF_INT_RAW_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_RAW_V  0x1
#define I2S_IN_ERR_EOF_INT_RAW_S  10
/* I2S_IN_SUC_EOF_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_in_suc_eof_int interrupt*/
#define I2S_IN_SUC_EOF_INT_RAW  (BIT(9))
#define I2S_IN_SUC_EOF_INT_RAW_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_RAW_V  0x1
#define I2S_IN_SUC_EOF_INT_RAW_S  9
/* I2S_IN_DONE_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_in_done_int interrupt*/
#define I2S_IN_DONE_INT_RAW  (BIT(8))
#define I2S_IN_DONE_INT_RAW_M  (BIT(8))
#define I2S_IN_DONE_INT_RAW_V  0x1
#define I2S_IN_DONE_INT_RAW_S  8
/* I2S_TX_HUNG_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_RAW  (BIT(7))
#define I2S_TX_HUNG_INT_RAW_M  (BIT(7))
#define I2S_TX_HUNG_INT_RAW_V  0x1
#define I2S_TX_HUNG_INT_RAW_S  7
/* I2S_RX_HUNG_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_RAW  (BIT(6))
#define I2S_RX_HUNG_INT_RAW_M  (BIT(6))
#define I2S_RX_HUNG_INT_RAW_V  0x1
#define I2S_RX_HUNG_INT_RAW_S  6
/* I2S_TX_REMPTY_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_tx_rempty_int interrupt*/
#define I2S_TX_REMPTY_INT_RAW  (BIT(5))
#define I2S_TX_REMPTY_INT_RAW_M  (BIT(5))
#define I2S_TX_REMPTY_INT_RAW_V  0x1
#define I2S_TX_REMPTY_INT_RAW_S  5
/* I2S_TX_WFULL_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_tx_wfull_int interrupt*/
#define I2S_TX_WFULL_INT_RAW  (BIT(4))
#define I2S_TX_WFULL_INT_RAW_M  (BIT(4))
#define I2S_TX_WFULL_INT_RAW_V  0x1
#define I2S_TX_WFULL_INT_RAW_S  4
/* I2S_RX_REMPTY_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_rx_rempty_int interrupt*/
#define I2S_RX_REMPTY_INT_RAW  (BIT(3))
#define I2S_RX_REMPTY_INT_RAW_M  (BIT(3))
#define I2S_RX_REMPTY_INT_RAW_V  0x1
#define I2S_RX_REMPTY_INT_RAW_S  3
/* I2S_RX_WFULL_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_rx_wfull_int interrupt*/
#define I2S_RX_WFULL_INT_RAW  (BIT(2))
#define I2S_RX_WFULL_INT_RAW_M  (BIT(2))
#define I2S_RX_WFULL_INT_RAW_V  0x1
#define I2S_RX_WFULL_INT_RAW_S  2
/* I2S_TX_PUT_DATA_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_tx_put_data_int interrupt*/
#define I2S_TX_PUT_DATA_INT_RAW  (BIT(1))
#define I2S_TX_PUT_DATA_INT_RAW_M  (BIT(1))
#define I2S_TX_PUT_DATA_INT_RAW_V  0x1
#define I2S_TX_PUT_DATA_INT_RAW_S  1
/* I2S_RX_TAKE_DATA_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_rx_take_data_int interrupt*/
#define I2S_RX_TAKE_DATA_INT_RAW  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_RAW_M  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_RAW_V  0x1
#define I2S_RX_TAKE_DATA_INT_RAW_S  0

#define I2S_INT_ST_REG(i)          (REG_I2S_BASE(i) + 0x0010)
/* I2S_V_SYNC_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_v_sync_int  interrupt*/
#define I2S_V_SYNC_INT_ST  (BIT(17))
#define I2S_V_SYNC_INT_ST_M  (BIT(17))
#define I2S_V_SYNC_INT_ST_V  0x1
#define I2S_V_SYNC_INT_ST_S  17
/* I2S_OUT_TOTAL_EOF_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_out_total_eof_int interrupt*/
#define I2S_OUT_TOTAL_EOF_INT_ST  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ST_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ST_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_ST_S  16
/* I2S_IN_DSCR_EMPTY_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_in_dscr_empty_int interrupt*/
#define I2S_IN_DSCR_EMPTY_INT_ST  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ST_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ST_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_ST_S  15
/* I2S_OUT_DSCR_ERR_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_out_dscr_err_int interrupt*/
#define I2S_OUT_DSCR_ERR_INT_ST  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ST_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ST_V  0x1
#define I2S_OUT_DSCR_ERR_INT_ST_S  14
/* I2S_IN_DSCR_ERR_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_in_dscr_err_int interrupt*/
#define I2S_IN_DSCR_ERR_INT_ST  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ST_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ST_V  0x1
#define I2S_IN_DSCR_ERR_INT_ST_S  13
/* I2S_OUT_EOF_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_out_eof_int interrupt*/
#define I2S_OUT_EOF_INT_ST  (BIT(12))
#define I2S_OUT_EOF_INT_ST_M  (BIT(12))
#define I2S_OUT_EOF_INT_ST_V  0x1
#define I2S_OUT_EOF_INT_ST_S  12
/* I2S_OUT_DONE_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_out_done_int interrupt*/
#define I2S_OUT_DONE_INT_ST  (BIT(11))
#define I2S_OUT_DONE_INT_ST_M  (BIT(11))
#define I2S_OUT_DONE_INT_ST_V  0x1
#define I2S_OUT_DONE_INT_ST_S  11
/* I2S_IN_ERR_EOF_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: don't use*/
#define I2S_IN_ERR_EOF_INT_ST  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ST_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ST_V  0x1
#define I2S_IN_ERR_EOF_INT_ST_S  10
/* I2S_IN_SUC_EOF_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_in_suc_eof_int interrupt*/
#define I2S_IN_SUC_EOF_INT_ST  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ST_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ST_V  0x1
#define I2S_IN_SUC_EOF_INT_ST_S  9
/* I2S_IN_DONE_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_in_done_int interrupt*/
#define I2S_IN_DONE_INT_ST  (BIT(8))
#define I2S_IN_DONE_INT_ST_M  (BIT(8))
#define I2S_IN_DONE_INT_ST_V  0x1
#define I2S_IN_DONE_INT_ST_S  8
/* I2S_TX_HUNG_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_ST  (BIT(7))
#define I2S_TX_HUNG_INT_ST_M  (BIT(7))
#define I2S_TX_HUNG_INT_ST_V  0x1
#define I2S_TX_HUNG_INT_ST_S  7
/* I2S_RX_HUNG_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_ST  (BIT(6))
#define I2S_RX_HUNG_INT_ST_M  (BIT(6))
#define I2S_RX_HUNG_INT_ST_V  0x1
#define I2S_RX_HUNG_INT_ST_S  6
/* I2S_TX_REMPTY_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_tx_rempty_int interrupt*/
#define I2S_TX_REMPTY_INT_ST  (BIT(5))
#define I2S_TX_REMPTY_INT_ST_M  (BIT(5))
#define I2S_TX_REMPTY_INT_ST_V  0x1
#define I2S_TX_REMPTY_INT_ST_S  5
/* I2S_TX_WFULL_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_tx_wfull_int interrupt*/
#define I2S_TX_WFULL_INT_ST  (BIT(4))
#define I2S_TX_WFULL_INT_ST_M  (BIT(4))
#define I2S_TX_WFULL_INT_ST_V  0x1
#define I2S_TX_WFULL_INT_ST_S  4
/* I2S_RX_REMPTY_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_rx_rempty_int interrupt*/
#define I2S_RX_REMPTY_INT_ST  (BIT(3))
#define I2S_RX_REMPTY_INT_ST_M  (BIT(3))
#define I2S_RX_REMPTY_INT_ST_V  0x1
#define I2S_RX_REMPTY_INT_ST_S  3
/* I2S_RX_WFULL_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_rx_wfull_int interrupt*/
#define I2S_RX_WFULL_INT_ST  (BIT(2))
#define I2S_RX_WFULL_INT_ST_M  (BIT(2))
#define I2S_RX_WFULL_INT_ST_V  0x1
#define I2S_RX_WFULL_INT_ST_S  2
/* I2S_TX_PUT_DATA_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_tx_put_data_int interrupt*/
#define I2S_TX_PUT_DATA_INT_ST  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ST_M  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ST_V  0x1
#define I2S_TX_PUT_DATA_INT_ST_S  1
/* I2S_RX_TAKE_DATA_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_rx_take_data_int interrupt*/
#define I2S_RX_TAKE_DATA_INT_ST  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ST_M  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ST_V  0x1
#define I2S_RX_TAKE_DATA_INT_ST_S  0

#define I2S_INT_ENA_REG(i)          (REG_I2S_BASE(i) + 0x0014)
/* I2S_V_SYNC_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_v_sync_int interrupt*/
#define I2S_V_SYNC_INT_ENA  (BIT(17))
#define I2S_V_SYNC_INT_ENA_M  (BIT(17))
#define I2S_V_SYNC_INT_ENA_V  0x1
#define I2S_V_SYNC_INT_ENA_S  17
/* I2S_OUT_TOTAL_EOF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_out_total_eof_int interrupt*/
#define I2S_OUT_TOTAL_EOF_INT_ENA  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ENA_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_ENA_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_ENA_S  16
/* I2S_IN_DSCR_EMPTY_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_in_dscr_empty_int interrupt*/
#define I2S_IN_DSCR_EMPTY_INT_ENA  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ENA_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_ENA_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_ENA_S  15
/* I2S_OUT_DSCR_ERR_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_out_dscr_err_int interrupt*/
#define I2S_OUT_DSCR_ERR_INT_ENA  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ENA_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_ENA_V  0x1
#define I2S_OUT_DSCR_ERR_INT_ENA_S  14
/* I2S_IN_DSCR_ERR_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_in_dscr_err_int interrupt*/
#define I2S_IN_DSCR_ERR_INT_ENA  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ENA_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_ENA_V  0x1
#define I2S_IN_DSCR_ERR_INT_ENA_S  13
/* I2S_OUT_EOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_out_eof_int interrupt*/
#define I2S_OUT_EOF_INT_ENA  (BIT(12))
#define I2S_OUT_EOF_INT_ENA_M  (BIT(12))
#define I2S_OUT_EOF_INT_ENA_V  0x1
#define I2S_OUT_EOF_INT_ENA_S  12
/* I2S_OUT_DONE_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_out_done_int interrupt*/
#define I2S_OUT_DONE_INT_ENA  (BIT(11))
#define I2S_OUT_DONE_INT_ENA_M  (BIT(11))
#define I2S_OUT_DONE_INT_ENA_V  0x1
#define I2S_OUT_DONE_INT_ENA_S  11
/* I2S_IN_ERR_EOF_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: don't use*/
#define I2S_IN_ERR_EOF_INT_ENA  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ENA_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_ENA_V  0x1
#define I2S_IN_ERR_EOF_INT_ENA_S  10
/* I2S_IN_SUC_EOF_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_in_suc_eof_int interrupt*/
#define I2S_IN_SUC_EOF_INT_ENA  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ENA_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_ENA_V  0x1
#define I2S_IN_SUC_EOF_INT_ENA_S  9
/* I2S_IN_DONE_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_in_done_int interrupt*/
#define I2S_IN_DONE_INT_ENA  (BIT(8))
#define I2S_IN_DONE_INT_ENA_M  (BIT(8))
#define I2S_IN_DONE_INT_ENA_V  0x1
#define I2S_IN_DONE_INT_ENA_S  8
/* I2S_TX_HUNG_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_ENA  (BIT(7))
#define I2S_TX_HUNG_INT_ENA_M  (BIT(7))
#define I2S_TX_HUNG_INT_ENA_V  0x1
#define I2S_TX_HUNG_INT_ENA_S  7
/* I2S_RX_HUNG_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_ENA  (BIT(6))
#define I2S_RX_HUNG_INT_ENA_M  (BIT(6))
#define I2S_RX_HUNG_INT_ENA_V  0x1
#define I2S_RX_HUNG_INT_ENA_S  6
/* I2S_TX_REMPTY_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_tx_rempty_int interrupt*/
#define I2S_TX_REMPTY_INT_ENA  (BIT(5))
#define I2S_TX_REMPTY_INT_ENA_M  (BIT(5))
#define I2S_TX_REMPTY_INT_ENA_V  0x1
#define I2S_TX_REMPTY_INT_ENA_S  5
/* I2S_TX_WFULL_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_tx_wfull_int interrupt*/
#define I2S_TX_WFULL_INT_ENA  (BIT(4))
#define I2S_TX_WFULL_INT_ENA_M  (BIT(4))
#define I2S_TX_WFULL_INT_ENA_V  0x1
#define I2S_TX_WFULL_INT_ENA_S  4
/* I2S_RX_REMPTY_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_rx_rempty_int interrupt*/
#define I2S_RX_REMPTY_INT_ENA  (BIT(3))
#define I2S_RX_REMPTY_INT_ENA_M  (BIT(3))
#define I2S_RX_REMPTY_INT_ENA_V  0x1
#define I2S_RX_REMPTY_INT_ENA_S  3
/* I2S_RX_WFULL_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_rx_wfull_int interrupt*/
#define I2S_RX_WFULL_INT_ENA  (BIT(2))
#define I2S_RX_WFULL_INT_ENA_M  (BIT(2))
#define I2S_RX_WFULL_INT_ENA_V  0x1
#define I2S_RX_WFULL_INT_ENA_S  2
/* I2S_TX_PUT_DATA_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_tx_put_data_int interrupt*/
#define I2S_TX_PUT_DATA_INT_ENA  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ENA_M  (BIT(1))
#define I2S_TX_PUT_DATA_INT_ENA_V  0x1
#define I2S_TX_PUT_DATA_INT_ENA_S  1
/* I2S_RX_TAKE_DATA_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_rx_take_data_int interrupt*/
#define I2S_RX_TAKE_DATA_INT_ENA  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ENA_M  (BIT(0))
#define I2S_RX_TAKE_DATA_INT_ENA_V  0x1
#define I2S_RX_TAKE_DATA_INT_ENA_S  0

#define I2S_INT_CLR_REG(i)          (REG_I2S_BASE(i) + 0x0018)
/* I2S_V_SYNC_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to clear the  i2s_v_sync_int interrupt*/
#define I2S_V_SYNC_INT_CLR  (BIT(17))
#define I2S_V_SYNC_INT_CLR_M  (BIT(17))
#define I2S_V_SYNC_INT_CLR_V  0x1
#define I2S_V_SYNC_INT_CLR_S  17
/* I2S_OUT_TOTAL_EOF_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_out_total_eof_int interrupt*/
#define I2S_OUT_TOTAL_EOF_INT_CLR  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_CLR_M  (BIT(16))
#define I2S_OUT_TOTAL_EOF_INT_CLR_V  0x1
#define I2S_OUT_TOTAL_EOF_INT_CLR_S  16
/* I2S_IN_DSCR_EMPTY_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_in_dscr_empty_int interrupt*/
#define I2S_IN_DSCR_EMPTY_INT_CLR  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_CLR_M  (BIT(15))
#define I2S_IN_DSCR_EMPTY_INT_CLR_V  0x1
#define I2S_IN_DSCR_EMPTY_INT_CLR_S  15
/* I2S_OUT_DSCR_ERR_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_out_dscr_err_int interrupt*/
#define I2S_OUT_DSCR_ERR_INT_CLR  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_CLR_M  (BIT(14))
#define I2S_OUT_DSCR_ERR_INT_CLR_V  0x1
#define I2S_OUT_DSCR_ERR_INT_CLR_S  14
/* I2S_IN_DSCR_ERR_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_in_dscr_err_int interrupt*/
#define I2S_IN_DSCR_ERR_INT_CLR  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_CLR_M  (BIT(13))
#define I2S_IN_DSCR_ERR_INT_CLR_V  0x1
#define I2S_IN_DSCR_ERR_INT_CLR_S  13
/* I2S_OUT_EOF_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_out_eof_int interrupt*/
#define I2S_OUT_EOF_INT_CLR  (BIT(12))
#define I2S_OUT_EOF_INT_CLR_M  (BIT(12))
#define I2S_OUT_EOF_INT_CLR_V  0x1
#define I2S_OUT_EOF_INT_CLR_S  12
/* I2S_OUT_DONE_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_out_done_int interrupt*/
#define I2S_OUT_DONE_INT_CLR  (BIT(11))
#define I2S_OUT_DONE_INT_CLR_M  (BIT(11))
#define I2S_OUT_DONE_INT_CLR_V  0x1
#define I2S_OUT_DONE_INT_CLR_S  11
/* I2S_IN_ERR_EOF_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: don't use*/
#define I2S_IN_ERR_EOF_INT_CLR  (BIT(10))
#define I2S_IN_ERR_EOF_INT_CLR_M  (BIT(10))
#define I2S_IN_ERR_EOF_INT_CLR_V  0x1
#define I2S_IN_ERR_EOF_INT_CLR_S  10
/* I2S_IN_SUC_EOF_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_in_suc_eof_int interrupt*/
#define I2S_IN_SUC_EOF_INT_CLR  (BIT(9))
#define I2S_IN_SUC_EOF_INT_CLR_M  (BIT(9))
#define I2S_IN_SUC_EOF_INT_CLR_V  0x1
#define I2S_IN_SUC_EOF_INT_CLR_S  9
/* I2S_IN_DONE_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_in_done_int interrupt*/
#define I2S_IN_DONE_INT_CLR  (BIT(8))
#define I2S_IN_DONE_INT_CLR_M  (BIT(8))
#define I2S_IN_DONE_INT_CLR_V  0x1
#define I2S_IN_DONE_INT_CLR_S  8
/* I2S_TX_HUNG_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_CLR  (BIT(7))
#define I2S_TX_HUNG_INT_CLR_M  (BIT(7))
#define I2S_TX_HUNG_INT_CLR_V  0x1
#define I2S_TX_HUNG_INT_CLR_S  7
/* I2S_RX_HUNG_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_CLR  (BIT(6))
#define I2S_RX_HUNG_INT_CLR_M  (BIT(6))
#define I2S_RX_HUNG_INT_CLR_V  0x1
#define I2S_RX_HUNG_INT_CLR_S  6
/* I2S_TX_REMPTY_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_tx_rempty_int interrupt*/
#define I2S_TX_REMPTY_INT_CLR  (BIT(5))
#define I2S_TX_REMPTY_INT_CLR_M  (BIT(5))
#define I2S_TX_REMPTY_INT_CLR_V  0x1
#define I2S_TX_REMPTY_INT_CLR_S  5
/* I2S_TX_WFULL_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_tx_wfull_int interrupt*/
#define I2S_TX_WFULL_INT_CLR  (BIT(4))
#define I2S_TX_WFULL_INT_CLR_M  (BIT(4))
#define I2S_TX_WFULL_INT_CLR_V  0x1
#define I2S_TX_WFULL_INT_CLR_S  4
/* I2S_RX_REMPTY_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_rx_rempty_int interrupt*/
#define I2S_RX_REMPTY_INT_CLR  (BIT(3))
#define I2S_RX_REMPTY_INT_CLR_M  (BIT(3))
#define I2S_RX_REMPTY_INT_CLR_V  0x1
#define I2S_RX_REMPTY_INT_CLR_S  3
/* I2S_RX_WFULL_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_rx_wfull_int interrupt*/
#define I2S_RX_WFULL_INT_CLR  (BIT(2))
#define I2S_RX_WFULL_INT_CLR_M  (BIT(2))
#define I2S_RX_WFULL_INT_CLR_V  0x1
#define I2S_RX_WFULL_INT_CLR_S  2
/* I2S_PUT_DATA_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_tx_put_data_int interrupt*/
#define I2S_PUT_DATA_INT_CLR  (BIT(1))
#define I2S_PUT_DATA_INT_CLR_M  (BIT(1))
#define I2S_PUT_DATA_INT_CLR_V  0x1
#define I2S_PUT_DATA_INT_CLR_S  1
/* I2S_TAKE_DATA_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_rx_take_data_int interrupt*/
#define I2S_TAKE_DATA_INT_CLR  (BIT(0))
#define I2S_TAKE_DATA_INT_CLR_M  (BIT(0))
#define I2S_TAKE_DATA_INT_CLR_V  0x1
#define I2S_TAKE_DATA_INT_CLR_S  0

#define I2S_TIMING_REG(i)          (REG_I2S_BASE(i) + 0x001c)
/* I2S_TX_BCK_IN_INV : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to invert BCK signal input to the slave transmitter*/
#define I2S_TX_BCK_IN_INV  (BIT(24))
#define I2S_TX_BCK_IN_INV_M  (BIT(24))
#define I2S_TX_BCK_IN_INV_V  0x1
#define I2S_TX_BCK_IN_INV_S  24
/* I2S_DATA_ENABLE_DELAY : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: Number of delay cycles for data valid flag.*/
#define I2S_DATA_ENABLE_DELAY  0x00000003
#define I2S_DATA_ENABLE_DELAY_M  ((I2S_DATA_ENABLE_DELAY_V)<<(I2S_DATA_ENABLE_DELAY_S))
#define I2S_DATA_ENABLE_DELAY_V  0x3
#define I2S_DATA_ENABLE_DELAY_S  22
/* I2S_RX_DSYNC_SW : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to synchronize signals with the double sync method
 into the receiver*/
#define I2S_RX_DSYNC_SW  (BIT(21))
#define I2S_RX_DSYNC_SW_M  (BIT(21))
#define I2S_RX_DSYNC_SW_V  0x1
#define I2S_RX_DSYNC_SW_S  21
/* I2S_TX_DSYNC_SW : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to synchronize signals with the double sync method
 into the transmitter*/
#define I2S_TX_DSYNC_SW  (BIT(20))
#define I2S_TX_DSYNC_SW_M  (BIT(20))
#define I2S_TX_DSYNC_SW_V  0x1
#define I2S_TX_DSYNC_SW_S  20
/* I2S_RX_BCK_OUT_DELAY : R/W ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: Number of delay cycles for BCK out of the receiver*/
#define I2S_RX_BCK_OUT_DELAY  0x00000003
#define I2S_RX_BCK_OUT_DELAY_M  ((I2S_RX_BCK_OUT_DELAY_V)<<(I2S_RX_BCK_OUT_DELAY_S))
#define I2S_RX_BCK_OUT_DELAY_V  0x3
#define I2S_RX_BCK_OUT_DELAY_S  18
/* I2S_RX_WS_OUT_DELAY : R/W ;bitpos:[17:16] ;default: 2'b0 ; */
/*description: Number of delay cycles for WS out of the receiver*/
#define I2S_RX_WS_OUT_DELAY  0x00000003
#define I2S_RX_WS_OUT_DELAY_M  ((I2S_RX_WS_OUT_DELAY_V)<<(I2S_RX_WS_OUT_DELAY_S))
#define I2S_RX_WS_OUT_DELAY_V  0x3
#define I2S_RX_WS_OUT_DELAY_S  16
/* I2S_TX_SD_OUT_DELAY : R/W ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: Number of delay cycles for SD out of the transmitter*/
#define I2S_TX_SD_OUT_DELAY  0x00000003
#define I2S_TX_SD_OUT_DELAY_M  ((I2S_TX_SD_OUT_DELAY_V)<<(I2S_TX_SD_OUT_DELAY_S))
#define I2S_TX_SD_OUT_DELAY_V  0x3
#define I2S_TX_SD_OUT_DELAY_S  14
/* I2S_TX_WS_OUT_DELAY : R/W ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: Number of delay cycles for WS out of the transmitter*/
#define I2S_TX_WS_OUT_DELAY  0x00000003
#define I2S_TX_WS_OUT_DELAY_M  ((I2S_TX_WS_OUT_DELAY_V)<<(I2S_TX_WS_OUT_DELAY_S))
#define I2S_TX_WS_OUT_DELAY_V  0x3
#define I2S_TX_WS_OUT_DELAY_S  12
/* I2S_TX_BCK_OUT_DELAY : R/W ;bitpos:[11:10] ;default: 2'b0 ; */
/*description: Number of delay cycles for BCK out of the transmitter*/
#define I2S_TX_BCK_OUT_DELAY  0x00000003
#define I2S_TX_BCK_OUT_DELAY_M  ((I2S_TX_BCK_OUT_DELAY_V)<<(I2S_TX_BCK_OUT_DELAY_S))
#define I2S_TX_BCK_OUT_DELAY_V  0x3
#define I2S_TX_BCK_OUT_DELAY_S  10
/* I2S_RX_SD_IN_DELAY : R/W ;bitpos:[9:8] ;default: 2'b0 ; */
/*description: Number of delay cycles for SD into the receiver*/
#define I2S_RX_SD_IN_DELAY  0x00000003
#define I2S_RX_SD_IN_DELAY_M  ((I2S_RX_SD_IN_DELAY_V)<<(I2S_RX_SD_IN_DELAY_S))
#define I2S_RX_SD_IN_DELAY_V  0x3
#define I2S_RX_SD_IN_DELAY_S  8
/* I2S_RX_WS_IN_DELAY : R/W ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: Number of delay cycles for WS into the receiver*/
#define I2S_RX_WS_IN_DELAY  0x00000003
#define I2S_RX_WS_IN_DELAY_M  ((I2S_RX_WS_IN_DELAY_V)<<(I2S_RX_WS_IN_DELAY_S))
#define I2S_RX_WS_IN_DELAY_V  0x3
#define I2S_RX_WS_IN_DELAY_S  6
/* I2S_RX_BCK_IN_DELAY : R/W ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: Number of delay cycles for BCK into the receiver*/
#define I2S_RX_BCK_IN_DELAY  0x00000003
#define I2S_RX_BCK_IN_DELAY_M  ((I2S_RX_BCK_IN_DELAY_V)<<(I2S_RX_BCK_IN_DELAY_S))
#define I2S_RX_BCK_IN_DELAY_V  0x3
#define I2S_RX_BCK_IN_DELAY_S  4
/* I2S_TX_WS_IN_DELAY : R/W ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: Number of delay cycles for WS into the transmitter*/
#define I2S_TX_WS_IN_DELAY  0x00000003
#define I2S_TX_WS_IN_DELAY_M  ((I2S_TX_WS_IN_DELAY_V)<<(I2S_TX_WS_IN_DELAY_S))
#define I2S_TX_WS_IN_DELAY_V  0x3
#define I2S_TX_WS_IN_DELAY_S  2
/* I2S_TX_BCK_IN_DELAY : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: Number of delay cycles for BCK into the transmitter*/
#define I2S_TX_BCK_IN_DELAY  0x00000003
#define I2S_TX_BCK_IN_DELAY_M  ((I2S_TX_BCK_IN_DELAY_V)<<(I2S_TX_BCK_IN_DELAY_S))
#define I2S_TX_BCK_IN_DELAY_V  0x3
#define I2S_TX_BCK_IN_DELAY_S  0

#define I2S_FIFO_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0020)
/* I2S_TX_24MSB_EN : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Only useful in tx 24bit mode. 1: the high 24 bits are effective
 in i2s fifo   0: the low 24 bits are effective in i2s fifo*/
#define I2S_TX_24MSB_EN  (BIT(23))
#define I2S_TX_24MSB_EN_M  (BIT(23))
#define I2S_TX_24MSB_EN_V  0x1
#define I2S_TX_24MSB_EN_S  23
/* I2S_RX_24MSB_EN : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Only useful in rx 24bit mode. 1: the high 24 bits are effective
 in i2s fifo   0: the low 24 bits are effective in i2s fifo*/
#define I2S_RX_24MSB_EN  (BIT(22))
#define I2S_RX_24MSB_EN_M  (BIT(22))
#define I2S_RX_24MSB_EN_V  0x1
#define I2S_RX_24MSB_EN_S  22
/* I2S_RX_FIFO_SYNC : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: force write back rx data to memory*/
#define I2S_RX_FIFO_SYNC  (BIT(21))
#define I2S_RX_FIFO_SYNC_M  (BIT(21))
#define I2S_RX_FIFO_SYNC_V  0x1
#define I2S_RX_FIFO_SYNC_S  21
/* I2S_RX_FIFO_MOD_FORCE_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: The bit should always be set to 1*/
#define I2S_RX_FIFO_MOD_FORCE_EN  (BIT(20))
#define I2S_RX_FIFO_MOD_FORCE_EN_M  (BIT(20))
#define I2S_RX_FIFO_MOD_FORCE_EN_V  0x1
#define I2S_RX_FIFO_MOD_FORCE_EN_S  20
/* I2S_TX_FIFO_MOD_FORCE_EN : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: The bit should always be set to 1*/
#define I2S_TX_FIFO_MOD_FORCE_EN  (BIT(19))
#define I2S_TX_FIFO_MOD_FORCE_EN_M  (BIT(19))
#define I2S_TX_FIFO_MOD_FORCE_EN_V  0x1
#define I2S_TX_FIFO_MOD_FORCE_EN_S  19
/* I2S_RX_FIFO_MOD : R/W ;bitpos:[18:16] ;default: 3'b0 ; */
/*description: Receiver FIFO mode configuration bits*/
#define I2S_RX_FIFO_MOD  0x00000007
#define I2S_RX_FIFO_MOD_M  ((I2S_RX_FIFO_MOD_V)<<(I2S_RX_FIFO_MOD_S))
#define I2S_RX_FIFO_MOD_V  0x7
#define I2S_RX_FIFO_MOD_S  16
/* I2S_TX_FIFO_MOD : R/W ;bitpos:[15:13] ;default: 3'b0 ; */
/*description: Transmitter FIFO mode configuration bits*/
#define I2S_TX_FIFO_MOD  0x00000007
#define I2S_TX_FIFO_MOD_M  ((I2S_TX_FIFO_MOD_V)<<(I2S_TX_FIFO_MOD_S))
#define I2S_TX_FIFO_MOD_V  0x7
#define I2S_TX_FIFO_MOD_S  13
/* I2S_DSCR_EN : R/W ;bitpos:[12] ;default: 1'd1 ; */
/*description: Set this bit to enable I2S DMA mode*/
#define I2S_DSCR_EN  (BIT(12))
#define I2S_DSCR_EN_M  (BIT(12))
#define I2S_DSCR_EN_V  0x1
#define I2S_DSCR_EN_S  12
/* I2S_TX_DATA_NUM : R/W ;bitpos:[11:6] ;default: 6'd32 ; */
/*description: Threshold of data length in transmitter FIFO*/
#define I2S_TX_DATA_NUM  0x0000003F
#define I2S_TX_DATA_NUM_M  ((I2S_TX_DATA_NUM_V)<<(I2S_TX_DATA_NUM_S))
#define I2S_TX_DATA_NUM_V  0x3F
#define I2S_TX_DATA_NUM_S  6
/* I2S_RX_DATA_NUM : R/W ;bitpos:[5:0] ;default: 6'd32 ; */
/*description: Threshold of data length in receiver FIFO*/
#define I2S_RX_DATA_NUM  0x0000003F
#define I2S_RX_DATA_NUM_M  ((I2S_RX_DATA_NUM_V)<<(I2S_RX_DATA_NUM_S))
#define I2S_RX_DATA_NUM_V  0x3F
#define I2S_RX_DATA_NUM_S  0

#define I2S_RXEOF_NUM_REG(i)          (REG_I2S_BASE(i) + 0x0024)
/* I2S_RX_EOF_NUM : R/W ;bitpos:[31:0] ;default: 32'd64 ; */
/*description: the length of data to be received. It will trigger i2s_in_suc_eof_int.*/
#define I2S_RX_EOF_NUM  0xFFFFFFFF
#define I2S_RX_EOF_NUM_M  ((I2S_RX_EOF_NUM_V)<<(I2S_RX_EOF_NUM_S))
#define I2S_RX_EOF_NUM_V  0xFFFFFFFF
#define I2S_RX_EOF_NUM_S  0

#define I2S_CONF_SIGLE_DATA_REG(i)          (REG_I2S_BASE(i) + 0x0028)
/* I2S_SIGLE_DATA : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: the right channel or left channel put out constant value stored
 in this register according to tx_chan_mod and reg_tx_msb_right*/
#define I2S_SIGLE_DATA  0xFFFFFFFF
#define I2S_SIGLE_DATA_M  ((I2S_SIGLE_DATA_V)<<(I2S_SIGLE_DATA_S))
#define I2S_SIGLE_DATA_V  0xFFFFFFFF
#define I2S_SIGLE_DATA_S  0

#define I2S_CONF_CHAN_REG(i)          (REG_I2S_BASE(i) + 0x002c)
/* I2S_RX_CHAN_MOD : R/W ;bitpos:[4:3] ;default: 2'b0 ; */
/*description: I2S receiver channel mode configuration bits.*/
#define I2S_RX_CHAN_MOD  0x00000003
#define I2S_RX_CHAN_MOD_M  ((I2S_RX_CHAN_MOD_V)<<(I2S_RX_CHAN_MOD_S))
#define I2S_RX_CHAN_MOD_V  0x3
#define I2S_RX_CHAN_MOD_S  3
/* I2S_TX_CHAN_MOD : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: I2S transmitter channel mode configuration bits.*/
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
/*description: Set this bit to restart outlink descriptor*/
#define I2S_OUTLINK_RESTART  (BIT(30))
#define I2S_OUTLINK_RESTART_M  (BIT(30))
#define I2S_OUTLINK_RESTART_V  0x1
#define I2S_OUTLINK_RESTART_S  30
/* I2S_OUTLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to start outlink descriptor*/
#define I2S_OUTLINK_START  (BIT(29))
#define I2S_OUTLINK_START_M  (BIT(29))
#define I2S_OUTLINK_START_V  0x1
#define I2S_OUTLINK_START_S  29
/* I2S_OUTLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to stop outlink descriptor*/
#define I2S_OUTLINK_STOP  (BIT(28))
#define I2S_OUTLINK_STOP_M  (BIT(28))
#define I2S_OUTLINK_STOP_V  0x1
#define I2S_OUTLINK_STOP_S  28
/* I2S_OUTLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The address of first outlink descriptor*/
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
/*description: Set this bit to restart inlink descriptor*/
#define I2S_INLINK_RESTART  (BIT(30))
#define I2S_INLINK_RESTART_M  (BIT(30))
#define I2S_INLINK_RESTART_V  0x1
#define I2S_INLINK_RESTART_S  30
/* I2S_INLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to start inlink descriptor*/
#define I2S_INLINK_START  (BIT(29))
#define I2S_INLINK_START_M  (BIT(29))
#define I2S_INLINK_START_V  0x1
#define I2S_INLINK_START_S  29
/* I2S_INLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to stop inlink descriptor*/
#define I2S_INLINK_STOP  (BIT(28))
#define I2S_INLINK_STOP_M  (BIT(28))
#define I2S_INLINK_STOP_V  0x1
#define I2S_INLINK_STOP_S  28
/* I2S_INLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The address of first inlink descriptor*/
#define I2S_INLINK_ADDR  0x000FFFFF
#define I2S_INLINK_ADDR_M  ((I2S_INLINK_ADDR_V)<<(I2S_INLINK_ADDR_S))
#define I2S_INLINK_ADDR_V  0xFFFFF
#define I2S_INLINK_ADDR_S  0

#define I2S_OUT_EOF_DES_ADDR_REG(i)          (REG_I2S_BASE(i) + 0x0038)
/* I2S_OUT_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The address of outlink descriptor that produces EOF*/
#define I2S_OUT_EOF_DES_ADDR  0xFFFFFFFF
#define I2S_OUT_EOF_DES_ADDR_M  ((I2S_OUT_EOF_DES_ADDR_V)<<(I2S_OUT_EOF_DES_ADDR_S))
#define I2S_OUT_EOF_DES_ADDR_V  0xFFFFFFFF
#define I2S_OUT_EOF_DES_ADDR_S  0

#define I2S_IN_EOF_DES_ADDR_REG(i)          (REG_I2S_BASE(i) + 0x003c)
/* I2S_IN_SUC_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The address of inlink descriptor that produces EOF*/
#define I2S_IN_SUC_EOF_DES_ADDR  0xFFFFFFFF
#define I2S_IN_SUC_EOF_DES_ADDR_M  ((I2S_IN_SUC_EOF_DES_ADDR_V)<<(I2S_IN_SUC_EOF_DES_ADDR_S))
#define I2S_IN_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define I2S_IN_SUC_EOF_DES_ADDR_S  0

#define I2S_OUT_EOF_BFR_DES_ADDR_REG(i)          (REG_I2S_BASE(i) + 0x0040)
/* I2S_OUT_EOF_BFR_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The address of buffer relative to the outlink descriptor that produces EOF*/
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
/*description: The address of current inlink descriptor*/
#define I2S_INLINK_DSCR  0xFFFFFFFF
#define I2S_INLINK_DSCR_M  ((I2S_INLINK_DSCR_V)<<(I2S_INLINK_DSCR_S))
#define I2S_INLINK_DSCR_V  0xFFFFFFFF
#define I2S_INLINK_DSCR_S  0

#define I2S_INLINK_DSCR_BF0_REG(i)          (REG_I2S_BASE(i) + 0x004C)
/* I2S_INLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of next inlink descriptor*/
#define I2S_INLINK_DSCR_BF0  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF0_M  ((I2S_INLINK_DSCR_BF0_V)<<(I2S_INLINK_DSCR_BF0_S))
#define I2S_INLINK_DSCR_BF0_V  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF0_S  0

#define I2S_INLINK_DSCR_BF1_REG(i)          (REG_I2S_BASE(i) + 0x0050)
/* I2S_INLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of next inlink data buffer*/
#define I2S_INLINK_DSCR_BF1  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF1_M  ((I2S_INLINK_DSCR_BF1_V)<<(I2S_INLINK_DSCR_BF1_S))
#define I2S_INLINK_DSCR_BF1_V  0xFFFFFFFF
#define I2S_INLINK_DSCR_BF1_S  0

#define I2S_OUTLINK_DSCR_REG(i)          (REG_I2S_BASE(i) + 0x0054)
/* I2S_OUTLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of current outlink descriptor*/
#define I2S_OUTLINK_DSCR  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_M  ((I2S_OUTLINK_DSCR_V)<<(I2S_OUTLINK_DSCR_S))
#define I2S_OUTLINK_DSCR_V  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_S  0

#define I2S_OUTLINK_DSCR_BF0_REG(i)          (REG_I2S_BASE(i) + 0x0058)
/* I2S_OUTLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of next outlink descriptor*/
#define I2S_OUTLINK_DSCR_BF0  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF0_M  ((I2S_OUTLINK_DSCR_BF0_V)<<(I2S_OUTLINK_DSCR_BF0_S))
#define I2S_OUTLINK_DSCR_BF0_V  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF0_S  0

#define I2S_OUTLINK_DSCR_BF1_REG(i)          (REG_I2S_BASE(i) + 0x005C)
/* I2S_OUTLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of next outlink data buffer*/
#define I2S_OUTLINK_DSCR_BF1  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF1_M  ((I2S_OUTLINK_DSCR_BF1_V)<<(I2S_OUTLINK_DSCR_BF1_S))
#define I2S_OUTLINK_DSCR_BF1_V  0xFFFFFFFF
#define I2S_OUTLINK_DSCR_BF1_S  0

#define I2S_LC_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0060)
/* I2S_EXT_MEM_BK_SIZE : R/W ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: DMA access external memory block size. 0: 16 bytes      1: 32
 bytes    2:64 bytes      3:reserved*/
#define I2S_EXT_MEM_BK_SIZE  0x00000003
#define I2S_EXT_MEM_BK_SIZE_M  ((I2S_EXT_MEM_BK_SIZE_V)<<(I2S_EXT_MEM_BK_SIZE_S))
#define I2S_EXT_MEM_BK_SIZE_V  0x3
#define I2S_EXT_MEM_BK_SIZE_S  14
/* I2S_MEM_TRANS_EN : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: don't use*/
#define I2S_MEM_TRANS_EN  (BIT(13))
#define I2S_MEM_TRANS_EN_M  (BIT(13))
#define I2S_MEM_TRANS_EN_V  0x1
#define I2S_MEM_TRANS_EN_S  13
/* I2S_CHECK_OWNER : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable check owner bit by hardware*/
#define I2S_CHECK_OWNER  (BIT(12))
#define I2S_CHECK_OWNER_M  (BIT(12))
#define I2S_CHECK_OWNER_V  0x1
#define I2S_CHECK_OWNER_S  12
/* I2S_OUT_DATA_BURST_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: Transmitter data transfer mode configuration bit. 1:  to prepare
 out data with burst mode      0: to prepare out data with byte mode*/
#define I2S_OUT_DATA_BURST_EN  (BIT(11))
#define I2S_OUT_DATA_BURST_EN_M  (BIT(11))
#define I2S_OUT_DATA_BURST_EN_V  0x1
#define I2S_OUT_DATA_BURST_EN_S  11
/* I2S_INDSCR_BURST_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: DMA inlink descriptor transfer mode configuration bit. 1:  to
 prepare inlink descriptor with burst mode    0: to prepare inlink descriptor with byte mode*/
#define I2S_INDSCR_BURST_EN  (BIT(10))
#define I2S_INDSCR_BURST_EN_M  (BIT(10))
#define I2S_INDSCR_BURST_EN_V  0x1
#define I2S_INDSCR_BURST_EN_S  10
/* I2S_OUTDSCR_BURST_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: DMA outlink descriptor transfer mode configuration bit. 1:  to
 prepare outlink descriptor with burst mode    0: to prepare outlink descriptor with byte mode*/
#define I2S_OUTDSCR_BURST_EN  (BIT(9))
#define I2S_OUTDSCR_BURST_EN_M  (BIT(9))
#define I2S_OUTDSCR_BURST_EN_V  0x1
#define I2S_OUTDSCR_BURST_EN_S  9
/* I2S_OUT_EOF_MODE : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: DMA out EOF flag generation mode . 1: when dma has popped all
 data from the FIFO  0:when ahb has pushed all data to the FIFO*/
#define I2S_OUT_EOF_MODE  (BIT(8))
#define I2S_OUT_EOF_MODE_M  (BIT(8))
#define I2S_OUT_EOF_MODE_V  0x1
#define I2S_OUT_EOF_MODE_S  8
/* I2S_OUT_NO_RESTART_CLR : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: don't use*/
#define I2S_OUT_NO_RESTART_CLR  (BIT(7))
#define I2S_OUT_NO_RESTART_CLR_M  (BIT(7))
#define I2S_OUT_NO_RESTART_CLR_V  0x1
#define I2S_OUT_NO_RESTART_CLR_S  7
/* I2S_OUT_AUTO_WRBACK : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to enable outlink-written-back automatically when
 out buffer is transmitted done.*/
#define I2S_OUT_AUTO_WRBACK  (BIT(6))
#define I2S_OUT_AUTO_WRBACK_M  (BIT(6))
#define I2S_OUT_AUTO_WRBACK_V  0x1
#define I2S_OUT_AUTO_WRBACK_S  6
/* I2S_IN_LOOP_TEST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to loop test outlink*/
#define I2S_IN_LOOP_TEST  (BIT(5))
#define I2S_IN_LOOP_TEST_M  (BIT(5))
#define I2S_IN_LOOP_TEST_V  0x1
#define I2S_IN_LOOP_TEST_S  5
/* I2S_OUT_LOOP_TEST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to loop test inlink*/
#define I2S_OUT_LOOP_TEST  (BIT(4))
#define I2S_OUT_LOOP_TEST_M  (BIT(4))
#define I2S_OUT_LOOP_TEST_V  0x1
#define I2S_OUT_LOOP_TEST_S  4
/* I2S_AHBM_RST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to reset ahb interface of DMA*/
#define I2S_AHBM_RST  (BIT(3))
#define I2S_AHBM_RST_M  (BIT(3))
#define I2S_AHBM_RST_V  0x1
#define I2S_AHBM_RST_S  3
/* I2S_AHBM_FIFO_RST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to reset ahb interface cmdFIFO of DMA*/
#define I2S_AHBM_FIFO_RST  (BIT(2))
#define I2S_AHBM_FIFO_RST_M  (BIT(2))
#define I2S_AHBM_FIFO_RST_V  0x1
#define I2S_AHBM_FIFO_RST_S  2
/* I2S_OUT_RST : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to reset out dma FSM*/
#define I2S_OUT_RST  (BIT(1))
#define I2S_OUT_RST_M  (BIT(1))
#define I2S_OUT_RST_V  0x1
#define I2S_OUT_RST_S  1
/* I2S_IN_RST : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to reset in dma FSM*/
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
/* I2S_OUT_EMPTY : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: DMA transmitter status register*/
#define I2S_OUT_EMPTY  (BIT(31))
#define I2S_OUT_EMPTY_M  (BIT(31))
#define I2S_OUT_EMPTY_V  0x1
#define I2S_OUT_EMPTY_S  31
/* I2S_OUT_FULL : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define I2S_OUT_FULL  (BIT(30))
#define I2S_OUT_FULL_M  (BIT(30))
#define I2S_OUT_FULL_V  0x1
#define I2S_OUT_FULL_S  30
/* I2S_OUTFIFO_CNT : RO ;bitpos:[29:23] ;default: 7'b0 ; */
/*description: */
#define I2S_OUTFIFO_CNT  0x0000007F
#define I2S_OUTFIFO_CNT_M  ((I2S_OUTFIFO_CNT_V)<<(I2S_OUTFIFO_CNT_S))
#define I2S_OUTFIFO_CNT_V  0x7F
#define I2S_OUTFIFO_CNT_S  23
/* I2S_OUT_STATE : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define I2S_OUT_STATE  0x00000007
#define I2S_OUT_STATE_M  ((I2S_OUT_STATE_V)<<(I2S_OUT_STATE_S))
#define I2S_OUT_STATE_V  0x7
#define I2S_OUT_STATE_S  20
/* I2S_OUT_DSCR_STATE : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: */
#define I2S_OUT_DSCR_STATE  0x00000003
#define I2S_OUT_DSCR_STATE_M  ((I2S_OUT_DSCR_STATE_V)<<(I2S_OUT_DSCR_STATE_S))
#define I2S_OUT_DSCR_STATE_V  0x3
#define I2S_OUT_DSCR_STATE_S  18
/* I2S_OUTLINK_DSCR_ADDR : RO ;bitpos:[17:0] ;default: 18'h0 ; */
/*description: */
#define I2S_OUTLINK_DSCR_ADDR  0x0003FFFF
#define I2S_OUTLINK_DSCR_ADDR_M  ((I2S_OUTLINK_DSCR_ADDR_V)<<(I2S_OUTLINK_DSCR_ADDR_S))
#define I2S_OUTLINK_DSCR_ADDR_V  0x3FFFF
#define I2S_OUTLINK_DSCR_ADDR_S  0

#define I2S_LC_STATE1_REG(i)          (REG_I2S_BASE(i) + 0x0070)
/* I2S_IN_EMPTY : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: DMA receiver status register*/
#define I2S_IN_EMPTY  (BIT(31))
#define I2S_IN_EMPTY_M  (BIT(31))
#define I2S_IN_EMPTY_V  0x1
#define I2S_IN_EMPTY_S  31
/* I2S_IN_FULL : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define I2S_IN_FULL  (BIT(30))
#define I2S_IN_FULL_M  (BIT(30))
#define I2S_IN_FULL_V  0x1
#define I2S_IN_FULL_S  30
/* I2S_INFIFO_CNT_DEBUG : RO ;bitpos:[29:23] ;default: 7'b0 ; */
/*description: */
#define I2S_INFIFO_CNT_DEBUG  0x0000007F
#define I2S_INFIFO_CNT_DEBUG_M  ((I2S_INFIFO_CNT_DEBUG_V)<<(I2S_INFIFO_CNT_DEBUG_S))
#define I2S_INFIFO_CNT_DEBUG_V  0x7F
#define I2S_INFIFO_CNT_DEBUG_S  23
/* I2S_IN_STATE : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define I2S_IN_STATE  0x00000007
#define I2S_IN_STATE_M  ((I2S_IN_STATE_V)<<(I2S_IN_STATE_S))
#define I2S_IN_STATE_V  0x7
#define I2S_IN_STATE_S  20
/* I2S_IN_DSCR_STATE : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: */
#define I2S_IN_DSCR_STATE  0x00000003
#define I2S_IN_DSCR_STATE_M  ((I2S_IN_DSCR_STATE_V)<<(I2S_IN_DSCR_STATE_S))
#define I2S_IN_DSCR_STATE_V  0x3
#define I2S_IN_DSCR_STATE_S  18
/* I2S_INLINK_DSCR_ADDR : RO ;bitpos:[17:0] ;default: 18'h0 ; */
/*description: */
#define I2S_INLINK_DSCR_ADDR  0x0003FFFF
#define I2S_INLINK_DSCR_ADDR_M  ((I2S_INLINK_DSCR_ADDR_V)<<(I2S_INLINK_DSCR_ADDR_S))
#define I2S_INLINK_DSCR_ADDR_V  0x3FFFF
#define I2S_INLINK_DSCR_ADDR_S  0

#define I2S_LC_HUNG_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0074)
/* I2S_LC_FIFO_TIMEOUT_ENA : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: The enable bit for FIFO timeout*/
#define I2S_LC_FIFO_TIMEOUT_ENA  (BIT(11))
#define I2S_LC_FIFO_TIMEOUT_ENA_M  (BIT(11))
#define I2S_LC_FIFO_TIMEOUT_ENA_V  0x1
#define I2S_LC_FIFO_TIMEOUT_ENA_S  11
/* I2S_LC_FIFO_TIMEOUT_SHIFT : R/W ;bitpos:[10:8] ;default: 3'b0 ; */
/*description: The bits are used to scale tick counter threshold. The tick counter
 is reset when counter value >= 88000/2^i2s_lc_fifo_timeout_shift*/
#define I2S_LC_FIFO_TIMEOUT_SHIFT  0x00000007
#define I2S_LC_FIFO_TIMEOUT_SHIFT_M  ((I2S_LC_FIFO_TIMEOUT_SHIFT_V)<<(I2S_LC_FIFO_TIMEOUT_SHIFT_S))
#define I2S_LC_FIFO_TIMEOUT_SHIFT_V  0x7
#define I2S_LC_FIFO_TIMEOUT_SHIFT_S  8
/* I2S_LC_FIFO_TIMEOUT : R/W ;bitpos:[7:0] ;default: 8'h10 ; */
/*description: the i2s_tx_hung_int interrupt or the i2s_rx_hung_int interrupt
 will be triggered when fifo hung counter is equal to this value*/
#define I2S_LC_FIFO_TIMEOUT  0x000000FF
#define I2S_LC_FIFO_TIMEOUT_M  ((I2S_LC_FIFO_TIMEOUT_V)<<(I2S_LC_FIFO_TIMEOUT_S))
#define I2S_LC_FIFO_TIMEOUT_V  0xFF
#define I2S_LC_FIFO_TIMEOUT_S  0

#define I2S_CONF1_REG(i)          (REG_I2S_BASE(i) + 0x00a0)
/* I2S_TX_ZEROS_RM_EN : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: don't use*/
#define I2S_TX_ZEROS_RM_EN  (BIT(9))
#define I2S_TX_ZEROS_RM_EN_M  (BIT(9))
#define I2S_TX_ZEROS_RM_EN_V  0x1
#define I2S_TX_ZEROS_RM_EN_S  9
/* I2S_TX_STOP_EN : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: Set this bit to stop disable output BCK signal and WS signal
 when tx FIFO is emtpy*/
#define I2S_TX_STOP_EN  (BIT(8))
#define I2S_TX_STOP_EN_M  (BIT(8))
#define I2S_TX_STOP_EN_V  0x1
#define I2S_TX_STOP_EN_S  8
/* I2S_RX_PCM_BYPASS : R/W ;bitpos:[7] ;default: 1'h1 ; */
/*description: Set this bit to bypass Compress/Decompress module for received data.*/
#define I2S_RX_PCM_BYPASS  (BIT(7))
#define I2S_RX_PCM_BYPASS_M  (BIT(7))
#define I2S_RX_PCM_BYPASS_V  0x1
#define I2S_RX_PCM_BYPASS_S  7
/* I2S_RX_PCM_CONF : R/W ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: Compress/Decompress module configuration bits. 0: decompress
 received data  1:compress received data*/
#define I2S_RX_PCM_CONF  0x00000007
#define I2S_RX_PCM_CONF_M  ((I2S_RX_PCM_CONF_V)<<(I2S_RX_PCM_CONF_S))
#define I2S_RX_PCM_CONF_V  0x7
#define I2S_RX_PCM_CONF_S  4
/* I2S_TX_PCM_BYPASS : R/W ;bitpos:[3] ;default: 1'h1 ; */
/*description: Set this bit to bypass  Compress/Decompress module for transmitted data.*/
#define I2S_TX_PCM_BYPASS  (BIT(3))
#define I2S_TX_PCM_BYPASS_M  (BIT(3))
#define I2S_TX_PCM_BYPASS_V  0x1
#define I2S_TX_PCM_BYPASS_S  3
/* I2S_TX_PCM_CONF : R/W ;bitpos:[2:0] ;default: 3'h1 ; */
/*description: Compress/Decompress module configuration bits. 0: decompress
 transmitted data  1:compress transmitted data*/
#define I2S_TX_PCM_CONF  0x00000007
#define I2S_TX_PCM_CONF_M  ((I2S_TX_PCM_CONF_V)<<(I2S_TX_PCM_CONF_S))
#define I2S_TX_PCM_CONF_V  0x7
#define I2S_TX_PCM_CONF_S  0

#define I2S_PD_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00a4)
/* I2S_DMA_RAM_CLK_FO : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define I2S_DMA_RAM_CLK_FO  (BIT(6))
#define I2S_DMA_RAM_CLK_FO_M  (BIT(6))
#define I2S_DMA_RAM_CLK_FO_V  0x1
#define I2S_DMA_RAM_CLK_FO_S  6
/* I2S_DMA_RAM_FORCE_PU : R/W ;bitpos:[5] ;default: 1'h1 ; */
/*description: */
#define I2S_DMA_RAM_FORCE_PU  (BIT(5))
#define I2S_DMA_RAM_FORCE_PU_M  (BIT(5))
#define I2S_DMA_RAM_FORCE_PU_V  0x1
#define I2S_DMA_RAM_FORCE_PU_S  5
/* I2S_DMA_RAM_FORCE_PD : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: */
#define I2S_DMA_RAM_FORCE_PD  (BIT(4))
#define I2S_DMA_RAM_FORCE_PD_M  (BIT(4))
#define I2S_DMA_RAM_FORCE_PD_V  0x1
#define I2S_DMA_RAM_FORCE_PD_S  4
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
/*description: Force FIFO power-up*/
#define I2S_FIFO_FORCE_PU  (BIT(1))
#define I2S_FIFO_FORCE_PU_M  (BIT(1))
#define I2S_FIFO_FORCE_PU_V  0x1
#define I2S_FIFO_FORCE_PU_S  1
/* I2S_FIFO_FORCE_PD : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Force FIFO power-down*/
#define I2S_FIFO_FORCE_PD  (BIT(0))
#define I2S_FIFO_FORCE_PD_M  (BIT(0))
#define I2S_FIFO_FORCE_PD_V  0x1
#define I2S_FIFO_FORCE_PD_S  0

#define I2S_CONF2_REG(i)          (REG_I2S_BASE(i) + 0x00a8)
/* I2S_I2SI_V_SYNC_FILTER_THRES : R/W ;bitpos:[13:11] ;default: 3'b0 ; */
/*description: */
#define I2S_I2SI_V_SYNC_FILTER_THRES  0x00000007
#define I2S_I2SI_V_SYNC_FILTER_THRES_M  ((I2S_I2SI_V_SYNC_FILTER_THRES_V)<<(I2S_I2SI_V_SYNC_FILTER_THRES_S))
#define I2S_I2SI_V_SYNC_FILTER_THRES_V  0x7
#define I2S_I2SI_V_SYNC_FILTER_THRES_S  11
/* I2S_I2SI_V_SYNC_FILTER_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define I2S_I2SI_V_SYNC_FILTER_EN  (BIT(10))
#define I2S_I2SI_V_SYNC_FILTER_EN_M  (BIT(10))
#define I2S_I2SI_V_SYNC_FILTER_EN_V  0x1
#define I2S_I2SI_V_SYNC_FILTER_EN_S  10
/* I2S_CAM_CLK_LOOPBACK : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to loopback cam_clk from i2s_rx*/
#define I2S_CAM_CLK_LOOPBACK  (BIT(9))
#define I2S_CAM_CLK_LOOPBACK_M  (BIT(9))
#define I2S_CAM_CLK_LOOPBACK_V  0x1
#define I2S_CAM_CLK_LOOPBACK_S  9
/* I2S_CAM_SYNC_FIFO_RESET : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to reset cam_sync_fifo*/
#define I2S_CAM_SYNC_FIFO_RESET  (BIT(8))
#define I2S_CAM_SYNC_FIFO_RESET_M  (BIT(8))
#define I2S_CAM_SYNC_FIFO_RESET_V  0x1
#define I2S_CAM_SYNC_FIFO_RESET_S  8
/* I2S_INTER_VALID_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to enable camera internal valid*/
#define I2S_INTER_VALID_EN  (BIT(7))
#define I2S_INTER_VALID_EN_M  (BIT(7))
#define I2S_INTER_VALID_EN_V  0x1
#define I2S_INTER_VALID_EN_S  7
/* I2S_EXT_ADC_START_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to enable the function that ADC mode is triggered
 by external signal.*/
#define I2S_EXT_ADC_START_EN  (BIT(6))
#define I2S_EXT_ADC_START_EN_M  (BIT(6))
#define I2S_EXT_ADC_START_EN_V  0x1
#define I2S_EXT_ADC_START_EN_S  6
/* I2S_LCD_EN : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable LCD mode*/
#define I2S_LCD_EN  (BIT(5))
#define I2S_LCD_EN_M  (BIT(5))
#define I2S_LCD_EN_V  0x1
#define I2S_LCD_EN_S  5
/* I2S_DATA_ENABLE : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: for debug camera mode enable*/
#define I2S_DATA_ENABLE  (BIT(4))
#define I2S_DATA_ENABLE_M  (BIT(4))
#define I2S_DATA_ENABLE_V  0x1
#define I2S_DATA_ENABLE_S  4
/* I2S_DATA_ENABLE_TEST_EN : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: for debug camera mode enable*/
#define I2S_DATA_ENABLE_TEST_EN  (BIT(3))
#define I2S_DATA_ENABLE_TEST_EN_M  (BIT(3))
#define I2S_DATA_ENABLE_TEST_EN_V  0x1
#define I2S_DATA_ENABLE_TEST_EN_S  3
/* I2S_LCD_TX_SDX2_EN : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to duplicate data pairs (Frame Form 2) in LCD mode.*/
#define I2S_LCD_TX_SDX2_EN  (BIT(2))
#define I2S_LCD_TX_SDX2_EN_M  (BIT(2))
#define I2S_LCD_TX_SDX2_EN_V  0x1
#define I2S_LCD_TX_SDX2_EN_S  2
/* I2S_LCD_TX_WRX2_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: LCD WR double for one datum.*/
#define I2S_LCD_TX_WRX2_EN  (BIT(1))
#define I2S_LCD_TX_WRX2_EN_M  (BIT(1))
#define I2S_LCD_TX_WRX2_EN_V  0x1
#define I2S_LCD_TX_WRX2_EN_S  1
/* I2S_CAMERA_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to enable camera mode*/
#define I2S_CAMERA_EN  (BIT(0))
#define I2S_CAMERA_EN_M  (BIT(0))
#define I2S_CAMERA_EN_V  0x1
#define I2S_CAMERA_EN_S  0

#define I2S_CLKM_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00ac)
/* I2S_CLK_SEL : R/W ;bitpos:[22:21] ;default: 2'b0 ; */
/*description: Set this bit to enable clk_apll*/
#define I2S_CLK_SEL  0x00000003
#define I2S_CLK_SEL_M  ((I2S_CLK_SEL_V)<<(I2S_CLK_SEL_S))
#define I2S_CLK_SEL_V  0x3
#define I2S_CLK_SEL_S  21
#define I2S_CLK_AUDIO_PLL 1
#define I2S_CLK_160M_PLL 2
/* I2S_CLK_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to enable clk gate*/
#define I2S_CLK_EN  (BIT(20))
#define I2S_CLK_EN_M  (BIT(20))
#define I2S_CLK_EN_V  0x1
#define I2S_CLK_EN_S  20
/* I2S_CLKM_DIV_A : R/W ;bitpos:[19:14] ;default: 6'h0 ; */
/*description: Fractional clock divider denominator value*/
#define I2S_CLKM_DIV_A  0x0000003F
#define I2S_CLKM_DIV_A_M  ((I2S_CLKM_DIV_A_V)<<(I2S_CLKM_DIV_A_S))
#define I2S_CLKM_DIV_A_V  0x3F
#define I2S_CLKM_DIV_A_S  14
/* I2S_CLKM_DIV_B : R/W ;bitpos:[13:8] ;default: 6'h0 ; */
/*description: Fractional clock divider numerator value*/
#define I2S_CLKM_DIV_B  0x0000003F
#define I2S_CLKM_DIV_B_M  ((I2S_CLKM_DIV_B_V)<<(I2S_CLKM_DIV_B_S))
#define I2S_CLKM_DIV_B_V  0x3F
#define I2S_CLKM_DIV_B_S  8
/* I2S_CLKM_DIV_NUM : R/W ;bitpos:[7:0] ;default: 8'd4 ; */
/*description: Integral I2S clock divider value*/
#define I2S_CLKM_DIV_NUM  0x000000FF
#define I2S_CLKM_DIV_NUM_M  ((I2S_CLKM_DIV_NUM_V)<<(I2S_CLKM_DIV_NUM_S))
#define I2S_CLKM_DIV_NUM_V  0xFF
#define I2S_CLKM_DIV_NUM_S  0

#define I2S_SAMPLE_RATE_CONF_REG(i)          (REG_I2S_BASE(i) + 0x00b0)
/* I2S_RX_BITS_MOD : R/W ;bitpos:[23:18] ;default: 6'd16 ; */
/*description: Set the bits to configure bit length of I2S receiver channel.*/
#define I2S_RX_BITS_MOD  0x0000003F
#define I2S_RX_BITS_MOD_M  ((I2S_RX_BITS_MOD_V)<<(I2S_RX_BITS_MOD_S))
#define I2S_RX_BITS_MOD_V  0x3F
#define I2S_RX_BITS_MOD_S  18
/* I2S_TX_BITS_MOD : R/W ;bitpos:[17:12] ;default: 6'd16 ; */
/*description: Set the bits to configure bit length of I2S transmitter channel.*/
#define I2S_TX_BITS_MOD  0x0000003F
#define I2S_TX_BITS_MOD_M  ((I2S_TX_BITS_MOD_V)<<(I2S_TX_BITS_MOD_S))
#define I2S_TX_BITS_MOD_V  0x3F
#define I2S_TX_BITS_MOD_S  12
/* I2S_RX_BCK_DIV_NUM : R/W ;bitpos:[11:6] ;default: 6'd6 ; */
/*description: Bit clock configuration bits in receiver mode.*/
#define I2S_RX_BCK_DIV_NUM  0x0000003F
#define I2S_RX_BCK_DIV_NUM_M  ((I2S_RX_BCK_DIV_NUM_V)<<(I2S_RX_BCK_DIV_NUM_S))
#define I2S_RX_BCK_DIV_NUM_V  0x3F
#define I2S_RX_BCK_DIV_NUM_S  6
/* I2S_TX_BCK_DIV_NUM : R/W ;bitpos:[5:0] ;default: 6'd6 ; */
/*description: Bit clock configuration bits in transmitter mode.*/
#define I2S_TX_BCK_DIV_NUM  0x0000003F
#define I2S_TX_BCK_DIV_NUM_M  ((I2S_TX_BCK_DIV_NUM_V)<<(I2S_TX_BCK_DIV_NUM_S))
#define I2S_TX_BCK_DIV_NUM_V  0x3F
#define I2S_TX_BCK_DIV_NUM_S  0

#define I2S_STATE_REG(i)          (REG_I2S_BASE(i) + 0x00bc)
/* I2S_TX_IDLE : RO ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define I2S_TX_IDLE  (BIT(0))
#define I2S_TX_IDLE_M  (BIT(0))
#define I2S_TX_IDLE_V  0x1
#define I2S_TX_IDLE_S  0

#define I2S_DATE_REG(i)          (REG_I2S_BASE(i) + 0x00fc)
/* I2S_I2SDATE : R/W ;bitpos:[31:0] ;default: 32'h19052500 ; */
/*description: */
#define I2S_I2SDATE  0xFFFFFFFF
#define I2S_I2SDATE_M  ((I2S_I2SDATE_V)<<(I2S_I2SDATE_S))
#define I2S_I2SDATE_V  0xFFFFFFFF
#define I2S_I2SDATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_I2S_REG_H_ */
