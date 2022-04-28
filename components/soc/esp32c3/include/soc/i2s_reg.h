/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_I2S_REG_H_
#define _SOC_I2S_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define I2S_INT_RAW_REG(i)          (REG_I2S_BASE(i) + 0x000c)
/* I2S_TX_HUNG_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_RAW  (BIT(3))
#define I2S_TX_HUNG_INT_RAW_M  (BIT(3))
#define I2S_TX_HUNG_INT_RAW_V  0x1
#define I2S_TX_HUNG_INT_RAW_S  3
/* I2S_RX_HUNG_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_RAW  (BIT(2))
#define I2S_RX_HUNG_INT_RAW_M  (BIT(2))
#define I2S_RX_HUNG_INT_RAW_V  0x1
#define I2S_RX_HUNG_INT_RAW_S  2
/* I2S_TX_DONE_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_tx_done_int interrupt*/
#define I2S_TX_DONE_INT_RAW  (BIT(1))
#define I2S_TX_DONE_INT_RAW_M  (BIT(1))
#define I2S_TX_DONE_INT_RAW_V  0x1
#define I2S_TX_DONE_INT_RAW_S  1
/* I2S_RX_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_rx_done_int interrupt*/
#define I2S_RX_DONE_INT_RAW  (BIT(0))
#define I2S_RX_DONE_INT_RAW_M  (BIT(0))
#define I2S_RX_DONE_INT_RAW_V  0x1
#define I2S_RX_DONE_INT_RAW_S  0

#define I2S_INT_ST_REG(i)          (REG_I2S_BASE(i) + 0x0010)
/* I2S_TX_HUNG_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_ST  (BIT(3))
#define I2S_TX_HUNG_INT_ST_M  (BIT(3))
#define I2S_TX_HUNG_INT_ST_V  0x1
#define I2S_TX_HUNG_INT_ST_S  3
/* I2S_RX_HUNG_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_ST  (BIT(2))
#define I2S_RX_HUNG_INT_ST_M  (BIT(2))
#define I2S_RX_HUNG_INT_ST_V  0x1
#define I2S_RX_HUNG_INT_ST_S  2
/* I2S_TX_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_tx_done_int interrupt*/
#define I2S_TX_DONE_INT_ST  (BIT(1))
#define I2S_TX_DONE_INT_ST_M  (BIT(1))
#define I2S_TX_DONE_INT_ST_V  0x1
#define I2S_TX_DONE_INT_ST_S  1
/* I2S_RX_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_rx_done_int interrupt*/
#define I2S_RX_DONE_INT_ST  (BIT(0))
#define I2S_RX_DONE_INT_ST_M  (BIT(0))
#define I2S_RX_DONE_INT_ST_V  0x1
#define I2S_RX_DONE_INT_ST_S  0

#define I2S_INT_ENA_REG(i)          (REG_I2S_BASE(i) + 0x0014)
/* I2S_TX_HUNG_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_ENA  (BIT(3))
#define I2S_TX_HUNG_INT_ENA_M  (BIT(3))
#define I2S_TX_HUNG_INT_ENA_V  0x1
#define I2S_TX_HUNG_INT_ENA_S  3
/* I2S_RX_HUNG_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_ENA  (BIT(2))
#define I2S_RX_HUNG_INT_ENA_M  (BIT(2))
#define I2S_RX_HUNG_INT_ENA_V  0x1
#define I2S_RX_HUNG_INT_ENA_S  2
/* I2S_TX_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_tx_done_int interrupt*/
#define I2S_TX_DONE_INT_ENA  (BIT(1))
#define I2S_TX_DONE_INT_ENA_M  (BIT(1))
#define I2S_TX_DONE_INT_ENA_V  0x1
#define I2S_TX_DONE_INT_ENA_S  1
/* I2S_RX_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_rx_done_int interrupt*/
#define I2S_RX_DONE_INT_ENA  (BIT(0))
#define I2S_RX_DONE_INT_ENA_M  (BIT(0))
#define I2S_RX_DONE_INT_ENA_V  0x1
#define I2S_RX_DONE_INT_ENA_S  0

#define I2S_INT_CLR_REG(i)          (REG_I2S_BASE(i) + 0x0018)
/* I2S_TX_HUNG_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_tx_hung_int interrupt*/
#define I2S_TX_HUNG_INT_CLR  (BIT(3))
#define I2S_TX_HUNG_INT_CLR_M  (BIT(3))
#define I2S_TX_HUNG_INT_CLR_V  0x1
#define I2S_TX_HUNG_INT_CLR_S  3
/* I2S_RX_HUNG_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_rx_hung_int interrupt*/
#define I2S_RX_HUNG_INT_CLR  (BIT(2))
#define I2S_RX_HUNG_INT_CLR_M  (BIT(2))
#define I2S_RX_HUNG_INT_CLR_V  0x1
#define I2S_RX_HUNG_INT_CLR_S  2
/* I2S_TX_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_tx_done_int interrupt*/
#define I2S_TX_DONE_INT_CLR  (BIT(1))
#define I2S_TX_DONE_INT_CLR_M  (BIT(1))
#define I2S_TX_DONE_INT_CLR_V  0x1
#define I2S_TX_DONE_INT_CLR_S  1
/* I2S_RX_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_rx_done_int interrupt*/
#define I2S_RX_DONE_INT_CLR  (BIT(0))
#define I2S_RX_DONE_INT_CLR_M  (BIT(0))
#define I2S_RX_DONE_INT_CLR_V  0x1
#define I2S_RX_DONE_INT_CLR_S  0

#define I2S_RX_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0020)
/* I2S_RX_PDM_EN : R/W ;bitpos:[20] ;default: 1'h0 ; */
/*description: 1: Enable I2S PDM Rx mode . 0: Disable.*/
#define I2S_RX_PDM_EN  (BIT(20))
#define I2S_RX_PDM_EN_M  (BIT(20))
#define I2S_RX_PDM_EN_V  0x1
#define I2S_RX_PDM_EN_S  20
/* I2S_RX_TDM_EN : R/W ;bitpos:[19] ;default: 1'h0 ; */
/*description: 1: Enable I2S TDM Rx mode . 0: Disable.*/
#define I2S_RX_TDM_EN  (BIT(19))
#define I2S_RX_TDM_EN_M  (BIT(19))
#define I2S_RX_TDM_EN_V  0x1
#define I2S_RX_TDM_EN_S  19
/* I2S_RX_BIT_ORDER : R/W ;bitpos:[18] ;default: 1'h0 ; */
/*description: I2S Rx bit endian. 1:small endian  the LSB is received first.
 0:big endian  the MSB is received first.*/
#define I2S_RX_BIT_ORDER  (BIT(18))
#define I2S_RX_BIT_ORDER_M  (BIT(18))
#define I2S_RX_BIT_ORDER_V  0x1
#define I2S_RX_BIT_ORDER_S  18
/* I2S_RX_WS_IDLE_POL : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: 0: WS should be 0 when receiving left channel data  and WS is
 1in right channel.  1: WS should be 1 when receiving left channel data  and WS is 0in right channel.*/
#define I2S_RX_WS_IDLE_POL  (BIT(17))
#define I2S_RX_WS_IDLE_POL_M  (BIT(17))
#define I2S_RX_WS_IDLE_POL_V  0x1
#define I2S_RX_WS_IDLE_POL_S  17
/* I2S_RX_24_FILL_EN : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: 1: store 24 channel bits to 32 bits. 0:store 24 channel bits to 24 bits.*/
#define I2S_RX_24_FILL_EN  (BIT(16))
#define I2S_RX_24_FILL_EN_M  (BIT(16))
#define I2S_RX_24_FILL_EN_V  0x1
#define I2S_RX_24_FILL_EN_S  16
/* I2S_RX_LEFT_ALIGN : R/W ;bitpos:[15] ;default: 1'b1 ; */
/*description: 1: I2S RX left alignment mode. 0: I2S RX right alignment mode.*/
#define I2S_RX_LEFT_ALIGN  (BIT(15))
#define I2S_RX_LEFT_ALIGN_M  (BIT(15))
#define I2S_RX_LEFT_ALIGN_V  0x1
#define I2S_RX_LEFT_ALIGN_S  15
/* I2S_RX_STOP_MODE : R/W ;bitpos:[14:13] ;default: 2'd0 ; */
/*description: 0  : I2S Rx only stop when reg_rx_start is cleared.   1: Stop
 when reg_rx_start is 0 or in_suc_eof is 1.   2:  Stop I2S RX when reg_rx_start is 0 or RX FIFO is full.*/
#define I2S_RX_STOP_MODE  0x00000003
#define I2S_RX_STOP_MODE_M  ((I2S_RX_STOP_MODE_V)<<(I2S_RX_STOP_MODE_S))
#define I2S_RX_STOP_MODE_V  0x3
#define I2S_RX_STOP_MODE_S  13
/* I2S_RX_PCM_BYPASS : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: Set this bit to bypass Compress/Decompress module for received data.*/
#define I2S_RX_PCM_BYPASS  (BIT(12))
#define I2S_RX_PCM_BYPASS_M  (BIT(12))
#define I2S_RX_PCM_BYPASS_V  0x1
#define I2S_RX_PCM_BYPASS_S  12
/* I2S_RX_PCM_CONF : R/W ;bitpos:[11:10] ;default: 2'h1 ; */
/*description: I2S RX compress/decompress configuration bit. & 0 (atol): A-Law
 decompress  1 (ltoa) : A-Law compress  2 (utol) : u-Law decompress  3 (ltou) : u-Law compress. &*/
#define I2S_RX_PCM_CONF  0x00000003
#define I2S_RX_PCM_CONF_M  ((I2S_RX_PCM_CONF_V)<<(I2S_RX_PCM_CONF_S))
#define I2S_RX_PCM_CONF_V  0x3
#define I2S_RX_PCM_CONF_S  10
/* I2S_RX_MONO_FST_VLD : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: 1: The first channel data value is valid in I2S RX mono mode.
   0: The second channel data value is valid in I2S RX mono mode.*/
#define I2S_RX_MONO_FST_VLD  (BIT(9))
#define I2S_RX_MONO_FST_VLD_M  (BIT(9))
#define I2S_RX_MONO_FST_VLD_V  0x1
#define I2S_RX_MONO_FST_VLD_S  9
/* I2S_RX_UPDATE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set 1 to update I2S RX registers from APB clock domain to I2S
 RX clock domain. This bit will be cleared by hardware after update register done.*/
#define I2S_RX_UPDATE  (BIT(8))
#define I2S_RX_UPDATE_M  (BIT(8))
#define I2S_RX_UPDATE_V  0x1
#define I2S_RX_UPDATE_S  8
/* I2S_RX_BIG_ENDIAN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: I2S Rx byte endian  1: low addr value to high addr. 0: low addr
 with low addr value.*/
#define I2S_RX_BIG_ENDIAN  (BIT(7))
#define I2S_RX_BIG_ENDIAN_M  (BIT(7))
#define I2S_RX_BIG_ENDIAN_V  0x1
#define I2S_RX_BIG_ENDIAN_S  7
/* I2S_RX_MONO : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable receiver  in mono mode*/
#define I2S_RX_MONO  (BIT(5))
#define I2S_RX_MONO_M  (BIT(5))
#define I2S_RX_MONO_V  0x1
#define I2S_RX_MONO_S  5
/* I2S_RX_SLAVE_MOD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to enable slave receiver mode*/
#define I2S_RX_SLAVE_MOD  (BIT(3))
#define I2S_RX_SLAVE_MOD_M  (BIT(3))
#define I2S_RX_SLAVE_MOD_V  0x1
#define I2S_RX_SLAVE_MOD_S  3
/* I2S_RX_START : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to start receiving data*/
#define I2S_RX_START  (BIT(2))
#define I2S_RX_START_M  (BIT(2))
#define I2S_RX_START_V  0x1
#define I2S_RX_START_S  2
/* I2S_RX_FIFO_RESET : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to reset Rx AFIFO*/
#define I2S_RX_FIFO_RESET  (BIT(1))
#define I2S_RX_FIFO_RESET_M  (BIT(1))
#define I2S_RX_FIFO_RESET_V  0x1
#define I2S_RX_FIFO_RESET_S  1
/* I2S_RX_RESET : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to reset receiver*/
#define I2S_RX_RESET  (BIT(0))
#define I2S_RX_RESET_M  (BIT(0))
#define I2S_RX_RESET_V  0x1
#define I2S_RX_RESET_S  0

#define I2S_TX_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0024)
/* I2S_SIG_LOOPBACK : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: Enable signal loop back mode with transmitter module and receiver
 module sharing the same WS and BCK signals.*/
#define I2S_SIG_LOOPBACK  (BIT(27))
#define I2S_SIG_LOOPBACK_M  (BIT(27))
#define I2S_SIG_LOOPBACK_V  0x1
#define I2S_SIG_LOOPBACK_S  27
/* I2S_TX_CHAN_MOD : R/W ;bitpos:[26:24] ;default: 3'b0 ; */
/*description: I2S transmitter channel mode configuration bits.*/
#define I2S_TX_CHAN_MOD  0x00000007
#define I2S_TX_CHAN_MOD_M  ((I2S_TX_CHAN_MOD_V)<<(I2S_TX_CHAN_MOD_S))
#define I2S_TX_CHAN_MOD_V  0x7
#define I2S_TX_CHAN_MOD_S  24
/* I2S_TX_PDM_EN : R/W ;bitpos:[20] ;default: 1'h0 ; */
/*description: 1: Enable I2S PDM Tx mode . 0: Disable.*/
#define I2S_TX_PDM_EN  (BIT(20))
#define I2S_TX_PDM_EN_M  (BIT(20))
#define I2S_TX_PDM_EN_V  0x1
#define I2S_TX_PDM_EN_S  20
/* I2S_TX_TDM_EN : R/W ;bitpos:[19] ;default: 1'h0 ; */
/*description: 1: Enable I2S TDM Tx mode . 0: Disable.*/
#define I2S_TX_TDM_EN  (BIT(19))
#define I2S_TX_TDM_EN_M  (BIT(19))
#define I2S_TX_TDM_EN_V  0x1
#define I2S_TX_TDM_EN_S  19
/* I2S_TX_BIT_ORDER : R/W ;bitpos:[18] ;default: 1'h0 ; */
/*description: I2S Tx bit endian. 1:small endian  the LSB is sent first. 0:big
 endian  the MSB is sent first.*/
#define I2S_TX_BIT_ORDER  (BIT(18))
#define I2S_TX_BIT_ORDER_M  (BIT(18))
#define I2S_TX_BIT_ORDER_V  0x1
#define I2S_TX_BIT_ORDER_S  18
/* I2S_TX_WS_IDLE_POL : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: 0: WS should be 0 when sending left channel data  and WS is 1in
 right channel.  1: WS should be 1 when sending left channel data  and WS is 0in right channel.*/
#define I2S_TX_WS_IDLE_POL  (BIT(17))
#define I2S_TX_WS_IDLE_POL_M  (BIT(17))
#define I2S_TX_WS_IDLE_POL_V  0x1
#define I2S_TX_WS_IDLE_POL_S  17
/* I2S_TX_24_FILL_EN : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: 1: Sent 32 bits in 24 channel bits mode. 0: Sent 24 bits in 24 channel bits mode*/
#define I2S_TX_24_FILL_EN  (BIT(16))
#define I2S_TX_24_FILL_EN_M  (BIT(16))
#define I2S_TX_24_FILL_EN_V  0x1
#define I2S_TX_24_FILL_EN_S  16
/* I2S_TX_LEFT_ALIGN : R/W ;bitpos:[15] ;default: 1'h1 ; */
/*description: 1: I2S TX left alignment mode. 0: I2S TX right alignment mode.*/
#define I2S_TX_LEFT_ALIGN  (BIT(15))
#define I2S_TX_LEFT_ALIGN_M  (BIT(15))
#define I2S_TX_LEFT_ALIGN_V  0x1
#define I2S_TX_LEFT_ALIGN_S  15
/* I2S_TX_STOP_EN : R/W ;bitpos:[13] ;default: 1'h1 ; */
/*description: Set this bit to stop disable output BCK signal and WS signal
 when tx FIFO is emtpy*/
#define I2S_TX_STOP_EN  (BIT(13))
#define I2S_TX_STOP_EN_M  (BIT(13))
#define I2S_TX_STOP_EN_V  0x1
#define I2S_TX_STOP_EN_S  13
/* I2S_TX_PCM_BYPASS : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: Set this bit to bypass  Compress/Decompress module for transmitted data.*/
#define I2S_TX_PCM_BYPASS  (BIT(12))
#define I2S_TX_PCM_BYPASS_M  (BIT(12))
#define I2S_TX_PCM_BYPASS_V  0x1
#define I2S_TX_PCM_BYPASS_S  12
/* I2S_TX_PCM_CONF : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: I2S TX compress/decompress configuration bit. & 0 (atol): A-Law
 decompress  1 (ltoa) : A-Law compress  2 (utol) : u-Law decompress  3 (ltou) : u-Law compress. &*/
#define I2S_TX_PCM_CONF  0x00000003
#define I2S_TX_PCM_CONF_M  ((I2S_TX_PCM_CONF_V)<<(I2S_TX_PCM_CONF_S))
#define I2S_TX_PCM_CONF_V  0x3
#define I2S_TX_PCM_CONF_S  10
/* I2S_TX_MONO_FST_VLD : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: 1: The first channel data value is valid in I2S TX mono mode.
   0: The second channel data value is valid in I2S TX mono mode.*/
#define I2S_TX_MONO_FST_VLD  (BIT(9))
#define I2S_TX_MONO_FST_VLD_M  (BIT(9))
#define I2S_TX_MONO_FST_VLD_V  0x1
#define I2S_TX_MONO_FST_VLD_S  9
/* I2S_TX_UPDATE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set 1 to update I2S TX registers from APB clock domain to I2S
 TX clock domain. This bit will be cleared by hardware after update register done.*/
#define I2S_TX_UPDATE  (BIT(8))
#define I2S_TX_UPDATE_M  (BIT(8))
#define I2S_TX_UPDATE_V  0x1
#define I2S_TX_UPDATE_S  8
/* I2S_TX_BIG_ENDIAN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: I2S Tx byte endian  1: low addr value to high addr.  0: low addr
 with low addr value.*/
#define I2S_TX_BIG_ENDIAN  (BIT(7))
#define I2S_TX_BIG_ENDIAN_M  (BIT(7))
#define I2S_TX_BIG_ENDIAN_V  0x1
#define I2S_TX_BIG_ENDIAN_S  7
/* I2S_TX_CHAN_EQUAL : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: 1: The value of Left channel data is equal to the value of right
 channel data in I2S TX mono mode or TDM channel select mode. 0: The invalid channel data is reg_i2s_single_data in I2S TX mono mode or TDM channel select mode.*/
#define I2S_TX_CHAN_EQUAL  (BIT(6))
#define I2S_TX_CHAN_EQUAL_M  (BIT(6))
#define I2S_TX_CHAN_EQUAL_V  0x1
#define I2S_TX_CHAN_EQUAL_S  6
/* I2S_TX_MONO : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable transmitter in mono mode*/
#define I2S_TX_MONO  (BIT(5))
#define I2S_TX_MONO_M  (BIT(5))
#define I2S_TX_MONO_V  0x1
#define I2S_TX_MONO_S  5
/* I2S_TX_SLAVE_MOD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to enable slave transmitter mode*/
#define I2S_TX_SLAVE_MOD  (BIT(3))
#define I2S_TX_SLAVE_MOD_M  (BIT(3))
#define I2S_TX_SLAVE_MOD_V  0x1
#define I2S_TX_SLAVE_MOD_S  3
/* I2S_TX_START : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to start transmitting data*/
#define I2S_TX_START  (BIT(2))
#define I2S_TX_START_M  (BIT(2))
#define I2S_TX_START_V  0x1
#define I2S_TX_START_S  2
/* I2S_TX_FIFO_RESET : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to reset Tx AFIFO*/
#define I2S_TX_FIFO_RESET  (BIT(1))
#define I2S_TX_FIFO_RESET_M  (BIT(1))
#define I2S_TX_FIFO_RESET_V  0x1
#define I2S_TX_FIFO_RESET_S  1
/* I2S_TX_RESET : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to reset transmitter*/
#define I2S_TX_RESET  (BIT(0))
#define I2S_TX_RESET_M  (BIT(0))
#define I2S_TX_RESET_V  0x1
#define I2S_TX_RESET_S  0

#define I2S_RX_CONF1_REG(i)          (REG_I2S_BASE(i) + 0x0028)
/* I2S_RX_MSB_SHIFT : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: Set this bit to enable receiver in Phillips standard mode*/
#define I2S_RX_MSB_SHIFT  (BIT(29))
#define I2S_RX_MSB_SHIFT_M  (BIT(29))
#define I2S_RX_MSB_SHIFT_V  0x1
#define I2S_RX_MSB_SHIFT_S  29
/* I2S_RX_TDM_CHAN_BITS : R/W ;bitpos:[28:24] ;default: 5'hF ; */
/*description: The Rx bit number for each channel minus 1in TDM mode.*/
#define I2S_RX_TDM_CHAN_BITS  0x0000001F
#define I2S_RX_TDM_CHAN_BITS_M  ((I2S_RX_TDM_CHAN_BITS_V)<<(I2S_RX_TDM_CHAN_BITS_S))
#define I2S_RX_TDM_CHAN_BITS_V  0x1F
#define I2S_RX_TDM_CHAN_BITS_S  24
/* I2S_RX_HALF_SAMPLE_BITS : R/W ;bitpos:[23:18] ;default: 6'hF ; */
/*description: I2S Rx half sample bits -1.*/
#define I2S_RX_HALF_SAMPLE_BITS  0x0000003F
#define I2S_RX_HALF_SAMPLE_BITS_M  ((I2S_RX_HALF_SAMPLE_BITS_V)<<(I2S_RX_HALF_SAMPLE_BITS_S))
#define I2S_RX_HALF_SAMPLE_BITS_V  0x3F
#define I2S_RX_HALF_SAMPLE_BITS_S  18
/* I2S_RX_BITS_MOD : R/W ;bitpos:[17:13] ;default: 5'hF ; */
/*description: Set the bits to configure bit length of I2S receiver channel.*/
#define I2S_RX_BITS_MOD  0x0000001F
#define I2S_RX_BITS_MOD_M  ((I2S_RX_BITS_MOD_V)<<(I2S_RX_BITS_MOD_S))
#define I2S_RX_BITS_MOD_V  0x1F
#define I2S_RX_BITS_MOD_S  13
/* I2S_RX_BCK_DIV_NUM : R/W ;bitpos:[12:7] ;default: 6'd6 ; */
/*description: Bit clock configuration bits in receiver mode.*/
#define I2S_RX_BCK_DIV_NUM  0x0000003F
#define I2S_RX_BCK_DIV_NUM_M  ((I2S_RX_BCK_DIV_NUM_V)<<(I2S_RX_BCK_DIV_NUM_S))
#define I2S_RX_BCK_DIV_NUM_V  0x3F
#define I2S_RX_BCK_DIV_NUM_S  7
/* I2S_RX_TDM_WS_WIDTH : R/W ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: The width of rx_ws_out in TDM mode is (reg_rx_tdm_ws_width[6:0] +1) * T_bck*/
#define I2S_RX_TDM_WS_WIDTH  0x0000007F
#define I2S_RX_TDM_WS_WIDTH_M  ((I2S_RX_TDM_WS_WIDTH_V)<<(I2S_RX_TDM_WS_WIDTH_S))
#define I2S_RX_TDM_WS_WIDTH_V  0x7F
#define I2S_RX_TDM_WS_WIDTH_S  0

#define I2S_TX_CONF1_REG(i)          (REG_I2S_BASE(i) + 0x002C)
/* I2S_TX_MSB_SHIFT : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: Set this bit to enable transmitter in Phillips standard mode*/
#define I2S_TX_MSB_SHIFT  (BIT(29))
#define I2S_TX_MSB_SHIFT_M  (BIT(29))
#define I2S_TX_MSB_SHIFT_V  0x1
#define I2S_TX_MSB_SHIFT_S  29
/* I2S_TX_TDM_CHAN_BITS : R/W ;bitpos:[28:24] ;default: 5'hF ; */
/*description: The Tx bit number for each channel minus 1in TDM mode.*/
#define I2S_TX_TDM_CHAN_BITS  0x0000001F
#define I2S_TX_TDM_CHAN_BITS_M  ((I2S_TX_TDM_CHAN_BITS_V)<<(I2S_TX_TDM_CHAN_BITS_S))
#define I2S_TX_TDM_CHAN_BITS_V  0x1F
#define I2S_TX_TDM_CHAN_BITS_S  24
/* I2S_TX_HALF_SAMPLE_BITS : R/W ;bitpos:[23:18] ;default: 6'hF ; */
/*description: I2S Tx half sample bits -1.*/
#define I2S_TX_HALF_SAMPLE_BITS  0x0000003F
#define I2S_TX_HALF_SAMPLE_BITS_M  ((I2S_TX_HALF_SAMPLE_BITS_V)<<(I2S_TX_HALF_SAMPLE_BITS_S))
#define I2S_TX_HALF_SAMPLE_BITS_V  0x3F
#define I2S_TX_HALF_SAMPLE_BITS_S  18
/* I2S_TX_BITS_MOD : R/W ;bitpos:[17:13] ;default: 5'hF ; */
/*description: Set the bits to configure bit length of I2S transmitter channel.*/
#define I2S_TX_BITS_MOD  0x0000001F
#define I2S_TX_BITS_MOD_M  ((I2S_TX_BITS_MOD_V)<<(I2S_TX_BITS_MOD_S))
#define I2S_TX_BITS_MOD_V  0x1F
#define I2S_TX_BITS_MOD_S  13
/* I2S_TX_BCK_DIV_NUM : R/W ;bitpos:[12:7] ;default: 6'd6 ; */
/*description: Bit clock configuration bits in transmitter mode.*/
#define I2S_TX_BCK_DIV_NUM  0x0000003F
#define I2S_TX_BCK_DIV_NUM_M  ((I2S_TX_BCK_DIV_NUM_V)<<(I2S_TX_BCK_DIV_NUM_S))
#define I2S_TX_BCK_DIV_NUM_V  0x3F
#define I2S_TX_BCK_DIV_NUM_S  7
/* I2S_TX_TDM_WS_WIDTH : R/W ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: The width of tx_ws_out in TDM mode is (reg_tx_tdm_ws_width[6:0] +1) * T_bck*/
#define I2S_TX_TDM_WS_WIDTH  0x0000007F
#define I2S_TX_TDM_WS_WIDTH_M  ((I2S_TX_TDM_WS_WIDTH_V)<<(I2S_TX_TDM_WS_WIDTH_S))
#define I2S_TX_TDM_WS_WIDTH_V  0x7F
#define I2S_TX_TDM_WS_WIDTH_S  0

#define I2S_RX_CLKM_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0030)
/* I2S_MCLK_SEL : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: 0: UseI2S Tx module clock as I2S_MCLK_OUT.  1: UseI2S Rx module
 clock as I2S_MCLK_OUT.*/
#define I2S_MCLK_SEL  (BIT(29))
#define I2S_MCLK_SEL_M  (BIT(29))
#define I2S_MCLK_SEL_V  0x1
#define I2S_MCLK_SEL_S  29
/* I2S_RX_CLK_SEL : R/W ;bitpos:[28:27] ;default: 2'b0 ; */
/*description: Select I2S Rx module source clock. 0: no clock. 1: APLL. 2: CLK160.
 3: I2S_MCLK_in.*/
#define I2S_RX_CLK_SEL  0x00000003
#define I2S_RX_CLK_SEL_M  ((I2S_RX_CLK_SEL_V)<<(I2S_RX_CLK_SEL_S))
#define I2S_RX_CLK_SEL_V  0x3
#define I2S_RX_CLK_SEL_S  27
/* I2S_RX_CLK_ACTIVE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: I2S Rx module clock enable signal.*/
#define I2S_RX_CLK_ACTIVE  (BIT(26))
#define I2S_RX_CLK_ACTIVE_M  (BIT(26))
#define I2S_RX_CLK_ACTIVE_V  0x1
#define I2S_RX_CLK_ACTIVE_S  26
/* I2S_RX_CLKM_DIV_NUM : R/W ;bitpos:[7:0] ;default: 8'd2 ; */
/*description: Integral I2S clock divider value*/
#define I2S_RX_CLKM_DIV_NUM  0x000000FF
#define I2S_RX_CLKM_DIV_NUM_M  ((I2S_RX_CLKM_DIV_NUM_V)<<(I2S_RX_CLKM_DIV_NUM_S))
#define I2S_RX_CLKM_DIV_NUM_V  0xFF
#define I2S_RX_CLKM_DIV_NUM_S  0

#define I2S_TX_CLKM_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0034)
/* I2S_CLK_EN : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to enable clk gate*/
#define I2S_CLK_EN  (BIT(29))
#define I2S_CLK_EN_M  (BIT(29))
#define I2S_CLK_EN_V  0x1
#define I2S_CLK_EN_S  29
/* I2S_TX_CLK_SEL : R/W ;bitpos:[28:27] ;default: 2'b0 ; */
/*description: Select I2S Tx module source clock. 0: XTAL clock. 1: APLL. 2:
 CLK160. 3: I2S_MCLK_in.*/
#define I2S_TX_CLK_SEL  0x00000003
#define I2S_TX_CLK_SEL_M  ((I2S_TX_CLK_SEL_V)<<(I2S_TX_CLK_SEL_S))
#define I2S_TX_CLK_SEL_V  0x3
#define I2S_TX_CLK_SEL_S  27
/* I2S_TX_CLK_ACTIVE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: I2S Tx module clock enable signal.*/
#define I2S_TX_CLK_ACTIVE  (BIT(26))
#define I2S_TX_CLK_ACTIVE_M  (BIT(26))
#define I2S_TX_CLK_ACTIVE_V  0x1
#define I2S_TX_CLK_ACTIVE_S  26
/* I2S_TX_CLKM_DIV_NUM : R/W ;bitpos:[7:0] ;default: 8'd2 ; */
/*description: Integral I2S TX clock divider value. f_I2S_CLK = f_I2S_CLK_S/(N+b/a).
 There will be (a-b) * n-div and b * (n+1)-div.  So the average combination will be:  for b <= a/2  z * [x * n-div + (n+1)-div] + y * n-div. For b > a/2  z * [n-div + x * (n+1)-div] + y * (n+1)-div.*/
#define I2S_TX_CLKM_DIV_NUM  0x000000FF
#define I2S_TX_CLKM_DIV_NUM_M  ((I2S_TX_CLKM_DIV_NUM_V)<<(I2S_TX_CLKM_DIV_NUM_S))
#define I2S_TX_CLKM_DIV_NUM_V  0xFF
#define I2S_TX_CLKM_DIV_NUM_S  0

#define I2S_RX_CLKM_DIV_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0038)
/* I2S_RX_CLKM_DIV_YN1 : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: For b <= a/2  the value of I2S_RX_CLKM_DIV_YN1 is 0 . For b >
 a/2  the value of I2S_RX_CLKM_DIV_YN1 is 1.*/
#define I2S_RX_CLKM_DIV_YN1  (BIT(27))
#define I2S_RX_CLKM_DIV_YN1_M  (BIT(27))
#define I2S_RX_CLKM_DIV_YN1_V  0x1
#define I2S_RX_CLKM_DIV_YN1_S  27
/* I2S_RX_CLKM_DIV_X : R/W ;bitpos:[26:18] ;default: 9'h0 ; */
/*description: For b <= a/2  the value of I2S_RX_CLKM_DIV_X is (a/b) - 1. For
 b > a/2  the value of I2S_RX_CLKM_DIV_X is (a/(a-b)) - 1.*/
#define I2S_RX_CLKM_DIV_X  0x000001FF
#define I2S_RX_CLKM_DIV_X_M  ((I2S_RX_CLKM_DIV_X_V)<<(I2S_RX_CLKM_DIV_X_S))
#define I2S_RX_CLKM_DIV_X_V  0x1FF
#define I2S_RX_CLKM_DIV_X_S  18
/* I2S_RX_CLKM_DIV_Y : R/W ;bitpos:[17:9] ;default: 9'h1 ; */
/*description: For b <= a/2  the value of I2S_RX_CLKM_DIV_Y is (a%b) . For b
 > a/2  the value of I2S_RX_CLKM_DIV_Y is (a%(a-b)).*/
#define I2S_RX_CLKM_DIV_Y  0x000001FF
#define I2S_RX_CLKM_DIV_Y_M  ((I2S_RX_CLKM_DIV_Y_V)<<(I2S_RX_CLKM_DIV_Y_S))
#define I2S_RX_CLKM_DIV_Y_V  0x1FF
#define I2S_RX_CLKM_DIV_Y_S  9
/* I2S_RX_CLKM_DIV_Z : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: For b <= a/2  the value of I2S_RX_CLKM_DIV_Z is b. For b > a/2
  the value of I2S_RX_CLKM_DIV_Z is (a-b).*/
#define I2S_RX_CLKM_DIV_Z  0x000001FF
#define I2S_RX_CLKM_DIV_Z_M  ((I2S_RX_CLKM_DIV_Z_V)<<(I2S_RX_CLKM_DIV_Z_S))
#define I2S_RX_CLKM_DIV_Z_V  0x1FF
#define I2S_RX_CLKM_DIV_Z_S  0

#define I2S_TX_CLKM_DIV_CONF_REG(i)          (REG_I2S_BASE(i) + 0x003C)
/* I2S_TX_CLKM_DIV_YN1 : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: For b <= a/2  the value of I2S_TX_CLKM_DIV_YN1 is 0 . For b >
 a/2  the value of I2S_TX_CLKM_DIV_YN1 is 1.*/
#define I2S_TX_CLKM_DIV_YN1  (BIT(27))
#define I2S_TX_CLKM_DIV_YN1_M  (BIT(27))
#define I2S_TX_CLKM_DIV_YN1_V  0x1
#define I2S_TX_CLKM_DIV_YN1_S  27
/* I2S_TX_CLKM_DIV_X : R/W ;bitpos:[26:18] ;default: 9'h0 ; */
/*description: For b <= a/2  the value of I2S_TX_CLKM_DIV_X is (a/b) - 1. For
 b > a/2  the value of I2S_TX_CLKM_DIV_X is (a/(a-b)) - 1.*/
#define I2S_TX_CLKM_DIV_X  0x000001FF
#define I2S_TX_CLKM_DIV_X_M  ((I2S_TX_CLKM_DIV_X_V)<<(I2S_TX_CLKM_DIV_X_S))
#define I2S_TX_CLKM_DIV_X_V  0x1FF
#define I2S_TX_CLKM_DIV_X_S  18
/* I2S_TX_CLKM_DIV_Y : R/W ;bitpos:[17:9] ;default: 9'h1 ; */
/*description: For b <= a/2  the value of I2S_TX_CLKM_DIV_Y is (a%b) . For b
 > a/2  the value of I2S_TX_CLKM_DIV_Y is (a%(a-b)).*/
#define I2S_TX_CLKM_DIV_Y  0x000001FF
#define I2S_TX_CLKM_DIV_Y_M  ((I2S_TX_CLKM_DIV_Y_V)<<(I2S_TX_CLKM_DIV_Y_S))
#define I2S_TX_CLKM_DIV_Y_V  0x1FF
#define I2S_TX_CLKM_DIV_Y_S  9
/* I2S_TX_CLKM_DIV_Z : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: For b <= a/2  the value of I2S_TX_CLKM_DIV_Z is b. For b > a/2
  the value of I2S_TX_CLKM_DIV_Z is (a-b).*/
#define I2S_TX_CLKM_DIV_Z  0x000001FF
#define I2S_TX_CLKM_DIV_Z_M  ((I2S_TX_CLKM_DIV_Z_V)<<(I2S_TX_CLKM_DIV_Z_S))
#define I2S_TX_CLKM_DIV_Z_V  0x1FF
#define I2S_TX_CLKM_DIV_Z_S  0

#define I2S_TX_PCM2PDM_CONF_REG(i)          (REG_I2S_BASE(i) + 0x040)
/* I2S_PCM2PDM_CONV_EN : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: I2S TX PDM Converter enable.*/
#define I2S_PCM2PDM_CONV_EN    (BIT(25))
#define I2S_PCM2PDM_CONV_EN_M  (BIT(25))
#define I2S_PCM2PDM_CONV_EN_V  0x1
#define I2S_PCM2PDM_CONV_EN_S  25
/* I2S_TX_PDM_DAC_MODE_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: I2S TX PDM dac 2channel enable.*/
#define I2S_TX_PDM_DAC_MODE_EN    (BIT(24))
#define I2S_TX_PDM_DAC_MODE_EN_M  (BIT(24))
#define I2S_TX_PDM_DAC_MODE_EN_V  0x1
#define I2S_TX_PDM_DAC_MODE_EN_S  24
/* I2S_TX_PDM_DAC_2OUT_EN : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: I2S TX PDM dac mode enable.*/
#define I2S_TX_PDM_DAC_2OUT_EN    (BIT(23))
#define I2S_TX_PDM_DAC_2OUT_EN_M  (BIT(23))
#define I2S_TX_PDM_DAC_2OUT_EN_V  0x1
#define I2S_TX_PDM_DAC_2OUT_EN_S  23
/* I2S_TX_PDM_SIGMADELTA_DITHER : R/W ;bitpos:[22] ;default: 1'b1 ; */
/*description: I2S TX PDM sigmadelta dither value.*/
#define I2S_TX_PDM_SIGMADELTA_DITHER    (BIT(22))
#define I2S_TX_PDM_SIGMADELTA_DITHER_M  (BIT(22))
#define I2S_TX_PDM_SIGMADELTA_DITHER_V  0x1
#define I2S_TX_PDM_SIGMADELTA_DITHER_S  22
/* I2S_TX_PDM_SIGMADELTA_DITHER2 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: I2S TX PDM sigmadelta dither2 value.*/
#define I2S_TX_PDM_SIGMADELTA_DITHER2    (BIT(21))
#define I2S_TX_PDM_SIGMADELTA_DITHER2_M  (BIT(21))
#define I2S_TX_PDM_SIGMADELTA_DITHER2_V  0x1
#define I2S_TX_PDM_SIGMADELTA_DITHER2_S  21
/* I2S_TX_PDM_SIGMADELTA_IN_SHIFT : R/W ;bitpos:[20:19] ;default: 2'h1 ; */
/*description: I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4.*/
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT    0x00000003
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_M  ((I2S_TX_PDM_SIGMADELTA_IN_SHIFT_V)<<(I2S_TX_PDM_SIGMADELTA_IN_SHIFT_S))
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_V  0x3
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_S  19
/* I2S_TX_PDM_SINC_IN_SHIFT : R/W ;bitpos:[18:17] ;default: 2'h1 ; */
/*description: I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4.*/
#define I2S_TX_PDM_SINC_IN_SHIFT    0x00000003
#define I2S_TX_PDM_SINC_IN_SHIFT_M  ((I2S_TX_PDM_SINC_IN_SHIFT_V)<<(I2S_TX_PDM_SINC_IN_SHIFT_S))
#define I2S_TX_PDM_SINC_IN_SHIFT_V  0x3
#define I2S_TX_PDM_SINC_IN_SHIFT_S  17
/* I2S_TX_PDM_LP_IN_SHIFT : R/W ;bitpos:[16:15] ;default: 2'h1 ; */
/*description: I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4.*/
#define I2S_TX_PDM_LP_IN_SHIFT    0x00000003
#define I2S_TX_PDM_LP_IN_SHIFT_M  ((I2S_TX_PDM_LP_IN_SHIFT_V)<<(I2S_TX_PDM_LP_IN_SHIFT_S))
#define I2S_TX_PDM_LP_IN_SHIFT_V  0x3
#define I2S_TX_PDM_LP_IN_SHIFT_S  15
/* I2S_TX_PDM_HP_IN_SHIFT : R/W ;bitpos:[14:13] ;default: 2'h1 ; */
/*description: I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4.*/
#define I2S_TX_PDM_HP_IN_SHIFT    0x00000003
#define I2S_TX_PDM_HP_IN_SHIFT_M  ((I2S_TX_PDM_HP_IN_SHIFT_V)<<(I2S_TX_PDM_HP_IN_SHIFT_S))
#define I2S_TX_PDM_HP_IN_SHIFT_V  0x3
#define I2S_TX_PDM_HP_IN_SHIFT_S  13
/* I2S_TX_PDM_PRESCALE : R/W ;bitpos:[12:5] ;default: 8'h0 ; */
/*description: I2S TX PDM prescale for sigmadelta.*/
#define I2S_TX_PDM_PRESCALE    0x000000FF
#define I2S_TX_PDM_PRESCALE_M  ((I2S_TX_PDM_PRESCALE_V)<<(I2S_TX_PDM_PRESCALE_S))
#define I2S_TX_PDM_PRESCALE_V  0xFF
#define I2S_TX_PDM_PRESCALE_S  5
/* I2S_TX_PDM_SINC_OSR2 : R/W ;bitpos:[4:1] ;default: 4'h2 ; */
/*description: I2S TX PDM OSR2 value.*/
#define I2S_TX_PDM_SINC_OSR2    0x0000000F
#define I2S_TX_PDM_SINC_OSR2_M  ((I2S_TX_PDM_SINC_OSR2_V)<<(I2S_TX_PDM_SINC_OSR2_S))
#define I2S_TX_PDM_SINC_OSR2_V  0xF
#define I2S_TX_PDM_SINC_OSR2_S  1
/* I2S_TX_PDM_HP_BYPASS : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: I2S TX PDM bypass hp filter or not. The option has been removed..*/
#define I2S_TX_PDM_HP_BYPASS    (BIT(0))
#define I2S_TX_PDM_HP_BYPASS_M  (BIT(0))
#define I2S_TX_PDM_HP_BYPASS_V  0x1
#define I2S_TX_PDM_HP_BYPASS_S  0

#define I2S_TX_PCM2PDM_CONF1_REG(i)          (REG_I2S_BASE(i) + 0x44)
/* I2S_TX_IIR_HP_MULT12_0 : R/W ;bitpos:[25:23] ;default: 3'd7 ; */
/*description: The fourth parameter of PDM TX IIR_HP filter stage 1 is (504 + I2S_TX_IIR_HP_MUL
T12_0[2:0]).*/
#define I2S_TX_IIR_HP_MULT12_0    0x00000007
#define I2S_TX_IIR_HP_MULT12_0_M  ((I2S_TX_IIR_HP_MULT12_0_V)<<(I2S_TX_IIR_HP_MULT12_0_S))
#define I2S_TX_IIR_HP_MULT12_0_V  0x7
#define I2S_TX_IIR_HP_MULT12_0_S  23
/* I2S_TX_IIR_HP_MULT12_5 : R/W ;bitpos:[22:20] ;default: 3'd7 ; */
/*description: The fourth parameter of PDM TX IIR_HP filter stage 2 is (504 + I2S_TX_IIR_HP_MUL
T12_5[2:0]).*/
#define I2S_TX_IIR_HP_MULT12_5    0x00000007
#define I2S_TX_IIR_HP_MULT12_5_M  ((I2S_TX_IIR_HP_MULT12_5_V)<<(I2S_TX_IIR_HP_MULT12_5_S))
#define I2S_TX_IIR_HP_MULT12_5_V  0x7
#define I2S_TX_IIR_HP_MULT12_5_S  20
/* I2S_TX_PDM_FS : R/W ;bitpos:[19:10] ;default: 10'd480 ; */
/*description: I2S TX PDM Fs.*/
#define I2S_TX_PDM_FS    0x000003FF
#define I2S_TX_PDM_FS_M  ((I2S_TX_PDM_FS_V)<<(I2S_TX_PDM_FS_S))
#define I2S_TX_PDM_FS_V  0x3FF
#define I2S_TX_PDM_FS_S  10
/* I2S_TX_PDM_FP : R/W ;bitpos:[9:0] ;default: 10'd960 ; */
/*description: I2S TX PDM Fp.*/
#define I2S_TX_PDM_FP    0x000003FF
#define I2S_TX_PDM_FP_M  ((I2S_TX_PDM_FP_V)<<(I2S_TX_PDM_FP_S))
#define I2S_TX_PDM_FP_V  0x3FF
#define I2S_TX_PDM_FP_S  0

#define I2S_RX_TDM_CTRL_REG(i)          (REG_I2S_BASE(i) + 0x050)
/* I2S_RX_TDM_TOT_CHAN_NUM : R/W ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: The total channel number of I2S TX TDM mode.*/
#define I2S_RX_TDM_TOT_CHAN_NUM  0x0000000F
#define I2S_RX_TDM_TOT_CHAN_NUM_M  ((I2S_RX_TDM_TOT_CHAN_NUM_V)<<(I2S_RX_TDM_TOT_CHAN_NUM_S))
#define I2S_RX_TDM_TOT_CHAN_NUM_V  0xF
#define I2S_RX_TDM_TOT_CHAN_NUM_S  16
/* I2S_RX_TDM_CHAN15_EN : R/W ;bitpos:[15] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN15_EN  (BIT(15))
#define I2S_RX_TDM_CHAN15_EN_M  (BIT(15))
#define I2S_RX_TDM_CHAN15_EN_V  0x1
#define I2S_RX_TDM_CHAN15_EN_S  15
/* I2S_RX_TDM_CHAN14_EN : R/W ;bitpos:[14] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN14_EN  (BIT(14))
#define I2S_RX_TDM_CHAN14_EN_M  (BIT(14))
#define I2S_RX_TDM_CHAN14_EN_V  0x1
#define I2S_RX_TDM_CHAN14_EN_S  14
/* I2S_RX_TDM_CHAN13_EN : R/W ;bitpos:[13] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN13_EN  (BIT(13))
#define I2S_RX_TDM_CHAN13_EN_M  (BIT(13))
#define I2S_RX_TDM_CHAN13_EN_V  0x1
#define I2S_RX_TDM_CHAN13_EN_S  13
/* I2S_RX_TDM_CHAN12_EN : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN12_EN  (BIT(12))
#define I2S_RX_TDM_CHAN12_EN_M  (BIT(12))
#define I2S_RX_TDM_CHAN12_EN_V  0x1
#define I2S_RX_TDM_CHAN12_EN_S  12
/* I2S_RX_TDM_CHAN11_EN : R/W ;bitpos:[11] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN11_EN  (BIT(11))
#define I2S_RX_TDM_CHAN11_EN_M  (BIT(11))
#define I2S_RX_TDM_CHAN11_EN_V  0x1
#define I2S_RX_TDM_CHAN11_EN_S  11
/* I2S_RX_TDM_CHAN10_EN : R/W ;bitpos:[10] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN10_EN  (BIT(10))
#define I2S_RX_TDM_CHAN10_EN_M  (BIT(10))
#define I2S_RX_TDM_CHAN10_EN_V  0x1
#define I2S_RX_TDM_CHAN10_EN_S  10
/* I2S_RX_TDM_CHAN9_EN : R/W ;bitpos:[9] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN9_EN  (BIT(9))
#define I2S_RX_TDM_CHAN9_EN_M  (BIT(9))
#define I2S_RX_TDM_CHAN9_EN_V  0x1
#define I2S_RX_TDM_CHAN9_EN_S  9
/* I2S_RX_TDM_CHAN8_EN : R/W ;bitpos:[8] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM channel $n. 0:
 Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_CHAN8_EN  (BIT(8))
#define I2S_RX_TDM_CHAN8_EN_M  (BIT(8))
#define I2S_RX_TDM_CHAN8_EN_V  0x1
#define I2S_RX_TDM_CHAN8_EN_S  8
/* I2S_RX_TDM_PDM_CHAN7_EN : R/W ;bitpos:[7] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN7_EN  (BIT(7))
#define I2S_RX_TDM_PDM_CHAN7_EN_M  (BIT(7))
#define I2S_RX_TDM_PDM_CHAN7_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN7_EN_S  7
/* I2S_RX_TDM_PDM_CHAN6_EN : R/W ;bitpos:[6] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN6_EN  (BIT(6))
#define I2S_RX_TDM_PDM_CHAN6_EN_M  (BIT(6))
#define I2S_RX_TDM_PDM_CHAN6_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN6_EN_S  6
/* I2S_RX_TDM_PDM_CHAN5_EN : R/W ;bitpos:[5] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN5_EN  (BIT(5))
#define I2S_RX_TDM_PDM_CHAN5_EN_M  (BIT(5))
#define I2S_RX_TDM_PDM_CHAN5_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN5_EN_S  5
/* I2S_RX_TDM_PDM_CHAN4_EN : R/W ;bitpos:[4] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN4_EN  (BIT(4))
#define I2S_RX_TDM_PDM_CHAN4_EN_M  (BIT(4))
#define I2S_RX_TDM_PDM_CHAN4_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN4_EN_S  4
/* I2S_RX_TDM_PDM_CHAN3_EN : R/W ;bitpos:[3] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN3_EN  (BIT(3))
#define I2S_RX_TDM_PDM_CHAN3_EN_M  (BIT(3))
#define I2S_RX_TDM_PDM_CHAN3_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN3_EN_S  3
/* I2S_RX_TDM_PDM_CHAN2_EN : R/W ;bitpos:[2] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN2_EN  (BIT(2))
#define I2S_RX_TDM_PDM_CHAN2_EN_M  (BIT(2))
#define I2S_RX_TDM_PDM_CHAN2_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN2_EN_S  2
/* I2S_RX_TDM_PDM_CHAN1_EN : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN1_EN  (BIT(1))
#define I2S_RX_TDM_PDM_CHAN1_EN_M  (BIT(1))
#define I2S_RX_TDM_PDM_CHAN1_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN1_EN_S  1
/* I2S_RX_TDM_PDM_CHAN0_EN : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: Enable the valid data input of I2S RX TDM or PDM channel $n.
 0:  Disable  just input 0 in this channel.*/
#define I2S_RX_TDM_PDM_CHAN0_EN  (BIT(0))
#define I2S_RX_TDM_PDM_CHAN0_EN_M  (BIT(0))
#define I2S_RX_TDM_PDM_CHAN0_EN_V  0x1
#define I2S_RX_TDM_PDM_CHAN0_EN_S  0

#define I2S_TX_TDM_CTRL_REG(i)          (REG_I2S_BASE(i) + 0x0054)
/* I2S_TX_TDM_SKIP_MSK_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: When DMA TX buffer stores the data of (REG_TX_TDM_TOT_CHAN_NUM
 + 1)  channels  and only the data of the enabled channels is sent  then this bit should be set. Clear it when all the data stored in DMA TX buffer is for enabled channels.*/
#define I2S_TX_TDM_SKIP_MSK_EN  (BIT(20))
#define I2S_TX_TDM_SKIP_MSK_EN_M  (BIT(20))
#define I2S_TX_TDM_SKIP_MSK_EN_V  0x1
#define I2S_TX_TDM_SKIP_MSK_EN_S  20
/* I2S_TX_TDM_TOT_CHAN_NUM : R/W ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: The total channel number minus 1 of I2S TX TDM mode.*/
#define I2S_TX_TDM_TOT_CHAN_NUM  0x0000000F
#define I2S_TX_TDM_TOT_CHAN_NUM_M  ((I2S_TX_TDM_TOT_CHAN_NUM_V)<<(I2S_TX_TDM_TOT_CHAN_NUM_S))
#define I2S_TX_TDM_TOT_CHAN_NUM_V  0xF
#define I2S_TX_TDM_TOT_CHAN_NUM_S  16
/* I2S_TX_TDM_CHAN15_EN : R/W ;bitpos:[15] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN15_EN  (BIT(15))
#define I2S_TX_TDM_CHAN15_EN_M  (BIT(15))
#define I2S_TX_TDM_CHAN15_EN_V  0x1
#define I2S_TX_TDM_CHAN15_EN_S  15
/* I2S_TX_TDM_CHAN14_EN : R/W ;bitpos:[14] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN14_EN  (BIT(14))
#define I2S_TX_TDM_CHAN14_EN_M  (BIT(14))
#define I2S_TX_TDM_CHAN14_EN_V  0x1
#define I2S_TX_TDM_CHAN14_EN_S  14
/* I2S_TX_TDM_CHAN13_EN : R/W ;bitpos:[13] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN13_EN  (BIT(13))
#define I2S_TX_TDM_CHAN13_EN_M  (BIT(13))
#define I2S_TX_TDM_CHAN13_EN_V  0x1
#define I2S_TX_TDM_CHAN13_EN_S  13
/* I2S_TX_TDM_CHAN12_EN : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN12_EN  (BIT(12))
#define I2S_TX_TDM_CHAN12_EN_M  (BIT(12))
#define I2S_TX_TDM_CHAN12_EN_V  0x1
#define I2S_TX_TDM_CHAN12_EN_S  12
/* I2S_TX_TDM_CHAN11_EN : R/W ;bitpos:[11] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN11_EN  (BIT(11))
#define I2S_TX_TDM_CHAN11_EN_M  (BIT(11))
#define I2S_TX_TDM_CHAN11_EN_V  0x1
#define I2S_TX_TDM_CHAN11_EN_S  11
/* I2S_TX_TDM_CHAN10_EN : R/W ;bitpos:[10] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN10_EN  (BIT(10))
#define I2S_TX_TDM_CHAN10_EN_M  (BIT(10))
#define I2S_TX_TDM_CHAN10_EN_V  0x1
#define I2S_TX_TDM_CHAN10_EN_S  10
/* I2S_TX_TDM_CHAN9_EN : R/W ;bitpos:[9] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN9_EN  (BIT(9))
#define I2S_TX_TDM_CHAN9_EN_M  (BIT(9))
#define I2S_TX_TDM_CHAN9_EN_V  0x1
#define I2S_TX_TDM_CHAN9_EN_S  9
/* I2S_TX_TDM_CHAN8_EN : R/W ;bitpos:[8] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN8_EN  (BIT(8))
#define I2S_TX_TDM_CHAN8_EN_M  (BIT(8))
#define I2S_TX_TDM_CHAN8_EN_V  0x1
#define I2S_TX_TDM_CHAN8_EN_S  8
/* I2S_TX_TDM_CHAN7_EN : R/W ;bitpos:[7] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN7_EN  (BIT(7))
#define I2S_TX_TDM_CHAN7_EN_M  (BIT(7))
#define I2S_TX_TDM_CHAN7_EN_V  0x1
#define I2S_TX_TDM_CHAN7_EN_S  7
/* I2S_TX_TDM_CHAN6_EN : R/W ;bitpos:[6] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN6_EN  (BIT(6))
#define I2S_TX_TDM_CHAN6_EN_M  (BIT(6))
#define I2S_TX_TDM_CHAN6_EN_V  0x1
#define I2S_TX_TDM_CHAN6_EN_S  6
/* I2S_TX_TDM_CHAN5_EN : R/W ;bitpos:[5] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN5_EN  (BIT(5))
#define I2S_TX_TDM_CHAN5_EN_M  (BIT(5))
#define I2S_TX_TDM_CHAN5_EN_V  0x1
#define I2S_TX_TDM_CHAN5_EN_S  5
/* I2S_TX_TDM_CHAN4_EN : R/W ;bitpos:[4] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN4_EN  (BIT(4))
#define I2S_TX_TDM_CHAN4_EN_M  (BIT(4))
#define I2S_TX_TDM_CHAN4_EN_V  0x1
#define I2S_TX_TDM_CHAN4_EN_S  4
/* I2S_TX_TDM_CHAN3_EN : R/W ;bitpos:[3] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN3_EN  (BIT(3))
#define I2S_TX_TDM_CHAN3_EN_M  (BIT(3))
#define I2S_TX_TDM_CHAN3_EN_V  0x1
#define I2S_TX_TDM_CHAN3_EN_S  3
/* I2S_TX_TDM_CHAN2_EN : R/W ;bitpos:[2] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN2_EN  (BIT(2))
#define I2S_TX_TDM_CHAN2_EN_M  (BIT(2))
#define I2S_TX_TDM_CHAN2_EN_V  0x1
#define I2S_TX_TDM_CHAN2_EN_S  2
/* I2S_TX_TDM_CHAN1_EN : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN1_EN  (BIT(1))
#define I2S_TX_TDM_CHAN1_EN_M  (BIT(1))
#define I2S_TX_TDM_CHAN1_EN_V  0x1
#define I2S_TX_TDM_CHAN1_EN_S  1
/* I2S_TX_TDM_CHAN0_EN : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: Enable the valid data output of I2S TX TDM channel $n. 0:
  Disable  just output 0 in this channel.*/
#define I2S_TX_TDM_CHAN0_EN  (BIT(0))
#define I2S_TX_TDM_CHAN0_EN_M  (BIT(0))
#define I2S_TX_TDM_CHAN0_EN_V  0x1
#define I2S_TX_TDM_CHAN0_EN_S  0

#define I2S_RX_TIMING_REG(i)          (REG_I2S_BASE(i) + 0x0058)
/* I2S_RX_BCK_IN_DM : R/W ;bitpos:[29:28] ;default: 2'h0 ; */
/*description: The delay mode of I2S Rx BCK input signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_RX_BCK_IN_DM  0x00000003
#define I2S_RX_BCK_IN_DM_M  ((I2S_RX_BCK_IN_DM_V)<<(I2S_RX_BCK_IN_DM_S))
#define I2S_RX_BCK_IN_DM_V  0x3
#define I2S_RX_BCK_IN_DM_S  28
/* I2S_RX_WS_IN_DM : R/W ;bitpos:[25:24] ;default: 2'h0 ; */
/*description: The delay mode of I2S Rx WS input signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_RX_WS_IN_DM  0x00000003
#define I2S_RX_WS_IN_DM_M  ((I2S_RX_WS_IN_DM_V)<<(I2S_RX_WS_IN_DM_S))
#define I2S_RX_WS_IN_DM_V  0x3
#define I2S_RX_WS_IN_DM_S  24
/* I2S_RX_BCK_OUT_DM : R/W ;bitpos:[21:20] ;default: 2'h0 ; */
/*description: The delay mode of I2S Rx BCK output signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_RX_BCK_OUT_DM  0x00000003
#define I2S_RX_BCK_OUT_DM_M  ((I2S_RX_BCK_OUT_DM_V)<<(I2S_RX_BCK_OUT_DM_S))
#define I2S_RX_BCK_OUT_DM_V  0x3
#define I2S_RX_BCK_OUT_DM_S  20
/* I2S_RX_WS_OUT_DM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: The delay mode of I2S Rx WS output signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_RX_WS_OUT_DM  0x00000003
#define I2S_RX_WS_OUT_DM_M  ((I2S_RX_WS_OUT_DM_V)<<(I2S_RX_WS_OUT_DM_S))
#define I2S_RX_WS_OUT_DM_V  0x3
#define I2S_RX_WS_OUT_DM_S  16
/* I2S_RX_SD_IN_DM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: The delay mode of I2S Rx SD input signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_RX_SD_IN_DM  0x00000003
#define I2S_RX_SD_IN_DM_M  ((I2S_RX_SD_IN_DM_V)<<(I2S_RX_SD_IN_DM_S))
#define I2S_RX_SD_IN_DM_V  0x3
#define I2S_RX_SD_IN_DM_S  0

#define I2S_TX_TIMING_REG(i)          (REG_I2S_BASE(i) + 0x005C)
/* I2S_TX_BCK_IN_DM : R/W ;bitpos:[29:28] ;default: 2'h0 ; */
/*description: The delay mode of I2S Tx BCK input signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_TX_BCK_IN_DM  0x00000003
#define I2S_TX_BCK_IN_DM_M  ((I2S_TX_BCK_IN_DM_V)<<(I2S_TX_BCK_IN_DM_S))
#define I2S_TX_BCK_IN_DM_V  0x3
#define I2S_TX_BCK_IN_DM_S  28
/* I2S_TX_WS_IN_DM : R/W ;bitpos:[25:24] ;default: 2'h0 ; */
/*description: The delay mode of I2S Tx WS input signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_TX_WS_IN_DM  0x00000003
#define I2S_TX_WS_IN_DM_M  ((I2S_TX_WS_IN_DM_V)<<(I2S_TX_WS_IN_DM_S))
#define I2S_TX_WS_IN_DM_V  0x3
#define I2S_TX_WS_IN_DM_S  24
/* I2S_TX_BCK_OUT_DM : R/W ;bitpos:[21:20] ;default: 2'h0 ; */
/*description: The delay mode of I2S Tx BCK output signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_TX_BCK_OUT_DM  0x00000003
#define I2S_TX_BCK_OUT_DM_M  ((I2S_TX_BCK_OUT_DM_V)<<(I2S_TX_BCK_OUT_DM_S))
#define I2S_TX_BCK_OUT_DM_V  0x3
#define I2S_TX_BCK_OUT_DM_S  20
/* I2S_TX_WS_OUT_DM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: The delay mode of I2S Tx WS output signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_TX_WS_OUT_DM  0x00000003
#define I2S_TX_WS_OUT_DM_M  ((I2S_TX_WS_OUT_DM_V)<<(I2S_TX_WS_OUT_DM_S))
#define I2S_TX_WS_OUT_DM_V  0x3
#define I2S_TX_WS_OUT_DM_S  16
/* I2S_TX_SD1_OUT_DM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: The delay mode of I2S TX SD1 output signal. 0: bypass. 1: delay by pos edge.  2:
 delay by neg edge. 3: not used..*/
#define I2S_TX_SD1_OUT_DM    0x00000003
#define I2S_TX_SD1_OUT_DM_M  ((I2S_TX_SD1_OUT_DM_V)<<(I2S_TX_SD1_OUT_DM_S))
#define I2S_TX_SD1_OUT_DM_V  0x3
#define I2S_TX_SD1_OUT_DM_S  4
/* I2S_TX_SD_OUT_DM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: The delay mode of I2S Tx SD output signal. 0: bypass. 1: delay
 by pos edge.  2: delay by neg edge. 3: not used.*/
#define I2S_TX_SD_OUT_DM  0x00000003
#define I2S_TX_SD_OUT_DM_M  ((I2S_TX_SD_OUT_DM_V)<<(I2S_TX_SD_OUT_DM_S))
#define I2S_TX_SD_OUT_DM_V  0x3
#define I2S_TX_SD_OUT_DM_S  0

#define I2S_LC_HUNG_CONF_REG(i)          (REG_I2S_BASE(i) + 0x0060)
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

#define I2S_RXEOF_NUM_REG(i)          (REG_I2S_BASE(i) + 0x0064)
/* I2S_RX_EOF_NUM : R/W ;bitpos:[11:0] ;default: 12'h40 ; */
/*description: the length of data to be received. It will trigger i2s_in_suc_eof_int.*/
#define I2S_RX_EOF_NUM  0x00000FFF
#define I2S_RX_EOF_NUM_M  ((I2S_RX_EOF_NUM_V)<<(I2S_RX_EOF_NUM_S))
#define I2S_RX_EOF_NUM_V  0xFFF
#define I2S_RX_EOF_NUM_S  0

#define I2S_CONF_SIGLE_DATA_REG(i)          (REG_I2S_BASE(i) + 0x0068)
/* I2S_SINGLE_DATA : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: the right channel or left channel put out constant value stored
 in this register according to tx_chan_mod and reg_tx_msb_right*/
#define I2S_SINGLE_DATA  0xFFFFFFFF
#define I2S_SINGLE_DATA_M  ((I2S_SINGLE_DATA_V)<<(I2S_SINGLE_DATA_S))
#define I2S_SINGLE_DATA_V  0xFFFFFFFF
#define I2S_SINGLE_DATA_S  0

#define I2S_STATE_REG(i)          (REG_I2S_BASE(i) + 0x006C)
/* I2S_TX_IDLE : RO ;bitpos:[0] ;default: 1'b1 ; */
/*description: 1: i2s_tx is idle state. 0: i2s_tx is working.*/
#define I2S_TX_IDLE  (BIT(0))
#define I2S_TX_IDLE_M  (BIT(0))
#define I2S_TX_IDLE_V  0x1
#define I2S_TX_IDLE_S  0

#define I2S_DATE_REG(i)          (REG_I2S_BASE(i) + 0x0080)
/* I2S_DATE : R/W ;bitpos:[27:0] ;default: 28'h2003230 ; */
/*description: Version control register*/
#define I2S_DATE  0x0FFFFFFF
#define I2S_DATE_M  ((I2S_DATE_V)<<(I2S_DATE_S))
#define I2S_DATE_V  0xFFFFFFF
#define I2S_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_I2S_REG_H_ */
