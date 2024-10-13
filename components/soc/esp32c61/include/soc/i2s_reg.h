/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** I2S_INT_RAW_REG register
 *  I2S interrupt raw register, valid in level.
 */
#define I2S_INT_RAW_REG (DR_REG_I2S_BASE + 0xc)
/** I2S_RX_DONE_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status bit  for the i2s_rx_done_int interrupt
 */
#define I2S_RX_DONE_INT_RAW    (BIT(0))
#define I2S_RX_DONE_INT_RAW_M  (I2S_RX_DONE_INT_RAW_V << I2S_RX_DONE_INT_RAW_S)
#define I2S_RX_DONE_INT_RAW_V  0x00000001U
#define I2S_RX_DONE_INT_RAW_S  0
/** I2S_TX_DONE_INT_RAW : RO/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status bit  for the i2s_tx_done_int interrupt
 */
#define I2S_TX_DONE_INT_RAW    (BIT(1))
#define I2S_TX_DONE_INT_RAW_M  (I2S_TX_DONE_INT_RAW_V << I2S_TX_DONE_INT_RAW_S)
#define I2S_TX_DONE_INT_RAW_V  0x00000001U
#define I2S_TX_DONE_INT_RAW_S  1
/** I2S_RX_HUNG_INT_RAW : RO/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status bit  for the i2s_rx_hung_int interrupt
 */
#define I2S_RX_HUNG_INT_RAW    (BIT(2))
#define I2S_RX_HUNG_INT_RAW_M  (I2S_RX_HUNG_INT_RAW_V << I2S_RX_HUNG_INT_RAW_S)
#define I2S_RX_HUNG_INT_RAW_V  0x00000001U
#define I2S_RX_HUNG_INT_RAW_S  2
/** I2S_TX_HUNG_INT_RAW : RO/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status bit  for the i2s_tx_hung_int interrupt
 */
#define I2S_TX_HUNG_INT_RAW    (BIT(3))
#define I2S_TX_HUNG_INT_RAW_M  (I2S_TX_HUNG_INT_RAW_V << I2S_TX_HUNG_INT_RAW_S)
#define I2S_TX_HUNG_INT_RAW_V  0x00000001U
#define I2S_TX_HUNG_INT_RAW_S  3

/** I2S_INT_ST_REG register
 *  I2S interrupt status register.
 */
#define I2S_INT_ST_REG (DR_REG_I2S_BASE + 0x10)
/** I2S_RX_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit  for the i2s_rx_done_int interrupt
 */
#define I2S_RX_DONE_INT_ST    (BIT(0))
#define I2S_RX_DONE_INT_ST_M  (I2S_RX_DONE_INT_ST_V << I2S_RX_DONE_INT_ST_S)
#define I2S_RX_DONE_INT_ST_V  0x00000001U
#define I2S_RX_DONE_INT_ST_S  0
/** I2S_TX_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit  for the i2s_tx_done_int interrupt
 */
#define I2S_TX_DONE_INT_ST    (BIT(1))
#define I2S_TX_DONE_INT_ST_M  (I2S_TX_DONE_INT_ST_V << I2S_TX_DONE_INT_ST_S)
#define I2S_TX_DONE_INT_ST_V  0x00000001U
#define I2S_TX_DONE_INT_ST_S  1
/** I2S_RX_HUNG_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit  for the i2s_rx_hung_int interrupt
 */
#define I2S_RX_HUNG_INT_ST    (BIT(2))
#define I2S_RX_HUNG_INT_ST_M  (I2S_RX_HUNG_INT_ST_V << I2S_RX_HUNG_INT_ST_S)
#define I2S_RX_HUNG_INT_ST_V  0x00000001U
#define I2S_RX_HUNG_INT_ST_S  2
/** I2S_TX_HUNG_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit  for the i2s_tx_hung_int interrupt
 */
#define I2S_TX_HUNG_INT_ST    (BIT(3))
#define I2S_TX_HUNG_INT_ST_M  (I2S_TX_HUNG_INT_ST_V << I2S_TX_HUNG_INT_ST_S)
#define I2S_TX_HUNG_INT_ST_V  0x00000001U
#define I2S_TX_HUNG_INT_ST_S  3

/** I2S_INT_ENA_REG register
 *  I2S interrupt enable register.
 */
#define I2S_INT_ENA_REG (DR_REG_I2S_BASE + 0x14)
/** I2S_RX_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit  for the i2s_rx_done_int interrupt
 */
#define I2S_RX_DONE_INT_ENA    (BIT(0))
#define I2S_RX_DONE_INT_ENA_M  (I2S_RX_DONE_INT_ENA_V << I2S_RX_DONE_INT_ENA_S)
#define I2S_RX_DONE_INT_ENA_V  0x00000001U
#define I2S_RX_DONE_INT_ENA_S  0
/** I2S_TX_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit  for the i2s_tx_done_int interrupt
 */
#define I2S_TX_DONE_INT_ENA    (BIT(1))
#define I2S_TX_DONE_INT_ENA_M  (I2S_TX_DONE_INT_ENA_V << I2S_TX_DONE_INT_ENA_S)
#define I2S_TX_DONE_INT_ENA_V  0x00000001U
#define I2S_TX_DONE_INT_ENA_S  1
/** I2S_RX_HUNG_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit  for the i2s_rx_hung_int interrupt
 */
#define I2S_RX_HUNG_INT_ENA    (BIT(2))
#define I2S_RX_HUNG_INT_ENA_M  (I2S_RX_HUNG_INT_ENA_V << I2S_RX_HUNG_INT_ENA_S)
#define I2S_RX_HUNG_INT_ENA_V  0x00000001U
#define I2S_RX_HUNG_INT_ENA_S  2
/** I2S_TX_HUNG_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit  for the i2s_tx_hung_int interrupt
 */
#define I2S_TX_HUNG_INT_ENA    (BIT(3))
#define I2S_TX_HUNG_INT_ENA_M  (I2S_TX_HUNG_INT_ENA_V << I2S_TX_HUNG_INT_ENA_S)
#define I2S_TX_HUNG_INT_ENA_V  0x00000001U
#define I2S_TX_HUNG_INT_ENA_S  3

/** I2S_INT_CLR_REG register
 *  I2S interrupt clear register.
 */
#define I2S_INT_CLR_REG (DR_REG_I2S_BASE + 0x18)
/** I2S_RX_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the i2s_rx_done_int interrupt
 */
#define I2S_RX_DONE_INT_CLR    (BIT(0))
#define I2S_RX_DONE_INT_CLR_M  (I2S_RX_DONE_INT_CLR_V << I2S_RX_DONE_INT_CLR_S)
#define I2S_RX_DONE_INT_CLR_V  0x00000001U
#define I2S_RX_DONE_INT_CLR_S  0
/** I2S_TX_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the i2s_tx_done_int interrupt
 */
#define I2S_TX_DONE_INT_CLR    (BIT(1))
#define I2S_TX_DONE_INT_CLR_M  (I2S_TX_DONE_INT_CLR_V << I2S_TX_DONE_INT_CLR_S)
#define I2S_TX_DONE_INT_CLR_V  0x00000001U
#define I2S_TX_DONE_INT_CLR_S  1
/** I2S_RX_HUNG_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the i2s_rx_hung_int interrupt
 */
#define I2S_RX_HUNG_INT_CLR    (BIT(2))
#define I2S_RX_HUNG_INT_CLR_M  (I2S_RX_HUNG_INT_CLR_V << I2S_RX_HUNG_INT_CLR_S)
#define I2S_RX_HUNG_INT_CLR_V  0x00000001U
#define I2S_RX_HUNG_INT_CLR_S  2
/** I2S_TX_HUNG_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the i2s_tx_hung_int interrupt
 */
#define I2S_TX_HUNG_INT_CLR    (BIT(3))
#define I2S_TX_HUNG_INT_CLR_M  (I2S_TX_HUNG_INT_CLR_V << I2S_TX_HUNG_INT_CLR_S)
#define I2S_TX_HUNG_INT_CLR_V  0x00000001U
#define I2S_TX_HUNG_INT_CLR_S  3

/** I2S_RX_CONF_REG register
 *  I2S RX configure register
 */
#define I2S_RX_CONF_REG (DR_REG_I2S_BASE + 0x20)
/** I2S_RX_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset receiver
 */
#define I2S_RX_RESET    (BIT(0))
#define I2S_RX_RESET_M  (I2S_RX_RESET_V << I2S_RX_RESET_S)
#define I2S_RX_RESET_V  0x00000001U
#define I2S_RX_RESET_S  0
/** I2S_RX_FIFO_RESET : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset Rx AFIFO
 */
#define I2S_RX_FIFO_RESET    (BIT(1))
#define I2S_RX_FIFO_RESET_M  (I2S_RX_FIFO_RESET_V << I2S_RX_FIFO_RESET_S)
#define I2S_RX_FIFO_RESET_V  0x00000001U
#define I2S_RX_FIFO_RESET_S  1
/** I2S_RX_START : R/W/SC; bitpos: [2]; default: 0;
 *  Set this bit to start receiving data
 */
#define I2S_RX_START    (BIT(2))
#define I2S_RX_START_M  (I2S_RX_START_V << I2S_RX_START_S)
#define I2S_RX_START_V  0x00000001U
#define I2S_RX_START_S  2
/** I2S_RX_SLAVE_MOD : R/W; bitpos: [3]; default: 0;
 *  Set this bit to enable slave receiver mode
 */
#define I2S_RX_SLAVE_MOD    (BIT(3))
#define I2S_RX_SLAVE_MOD_M  (I2S_RX_SLAVE_MOD_V << I2S_RX_SLAVE_MOD_S)
#define I2S_RX_SLAVE_MOD_V  0x00000001U
#define I2S_RX_SLAVE_MOD_S  3
/** I2S_RX_STOP_MODE : R/W; bitpos: [5:4]; default: 0;
 *  0  : I2S Rx only stop when reg_rx_start is cleared.   1: Stop when reg_rx_start is
 *  0 or in_suc_eof is 1.   2:  Stop I2S RX when reg_rx_start is 0 or RX FIFO is full.
 */
#define I2S_RX_STOP_MODE    0x00000003U
#define I2S_RX_STOP_MODE_M  (I2S_RX_STOP_MODE_V << I2S_RX_STOP_MODE_S)
#define I2S_RX_STOP_MODE_V  0x00000003U
#define I2S_RX_STOP_MODE_S  4
/** I2S_RX_MONO : R/W; bitpos: [6]; default: 0;
 *  Set this bit to enable receiver  in mono mode
 */
#define I2S_RX_MONO    (BIT(6))
#define I2S_RX_MONO_M  (I2S_RX_MONO_V << I2S_RX_MONO_S)
#define I2S_RX_MONO_V  0x00000001U
#define I2S_RX_MONO_S  6
/** I2S_RX_BIG_ENDIAN : R/W; bitpos: [7]; default: 0;
 *  I2S Rx byte endian, 1: low addr value to high addr. 0: low addr with low addr value.
 */
#define I2S_RX_BIG_ENDIAN    (BIT(7))
#define I2S_RX_BIG_ENDIAN_M  (I2S_RX_BIG_ENDIAN_V << I2S_RX_BIG_ENDIAN_S)
#define I2S_RX_BIG_ENDIAN_V  0x00000001U
#define I2S_RX_BIG_ENDIAN_S  7
/** I2S_RX_UPDATE : R/W/SC; bitpos: [8]; default: 0;
 *  Set 1 to update I2S RX registers from APB clock domain to I2S RX clock domain. This
 *  bit will be cleared by hardware after update register done.
 */
#define I2S_RX_UPDATE    (BIT(8))
#define I2S_RX_UPDATE_M  (I2S_RX_UPDATE_V << I2S_RX_UPDATE_S)
#define I2S_RX_UPDATE_V  0x00000001U
#define I2S_RX_UPDATE_S  8
/** I2S_RX_MONO_FST_VLD : R/W; bitpos: [9]; default: 1;
 *  1: The first channel data value is valid in I2S RX mono mode.   0: The second
 *  channel data value is valid in I2S RX mono mode.
 */
#define I2S_RX_MONO_FST_VLD    (BIT(9))
#define I2S_RX_MONO_FST_VLD_M  (I2S_RX_MONO_FST_VLD_V << I2S_RX_MONO_FST_VLD_S)
#define I2S_RX_MONO_FST_VLD_V  0x00000001U
#define I2S_RX_MONO_FST_VLD_S  9
/** I2S_RX_PCM_CONF : R/W; bitpos: [11:10]; default: 1;
 *  I2S RX compress/decompress configuration bit. & 0 (atol): A-Law decompress, 1
 *  (ltoa) : A-Law compress, 2 (utol) : u-Law decompress, 3 (ltou) : u-Law compress. &
 */
#define I2S_RX_PCM_CONF    0x00000003U
#define I2S_RX_PCM_CONF_M  (I2S_RX_PCM_CONF_V << I2S_RX_PCM_CONF_S)
#define I2S_RX_PCM_CONF_V  0x00000003U
#define I2S_RX_PCM_CONF_S  10
/** I2S_RX_PCM_BYPASS : R/W; bitpos: [12]; default: 1;
 *  Set this bit to bypass Compress/Decompress module for received data.
 */
#define I2S_RX_PCM_BYPASS    (BIT(12))
#define I2S_RX_PCM_BYPASS_M  (I2S_RX_PCM_BYPASS_V << I2S_RX_PCM_BYPASS_S)
#define I2S_RX_PCM_BYPASS_V  0x00000001U
#define I2S_RX_PCM_BYPASS_S  12
/** I2S_RX_MSB_SHIFT : R/W; bitpos: [13]; default: 1;
 *  Set this bit to enable receiver in Phillips standard mode
 */
#define I2S_RX_MSB_SHIFT    (BIT(13))
#define I2S_RX_MSB_SHIFT_M  (I2S_RX_MSB_SHIFT_V << I2S_RX_MSB_SHIFT_S)
#define I2S_RX_MSB_SHIFT_V  0x00000001U
#define I2S_RX_MSB_SHIFT_S  13
/** I2S_RX_LEFT_ALIGN : R/W; bitpos: [15]; default: 1;
 *  1: I2S RX left alignment mode. 0: I2S RX right alignment mode.
 */
#define I2S_RX_LEFT_ALIGN    (BIT(15))
#define I2S_RX_LEFT_ALIGN_M  (I2S_RX_LEFT_ALIGN_V << I2S_RX_LEFT_ALIGN_S)
#define I2S_RX_LEFT_ALIGN_V  0x00000001U
#define I2S_RX_LEFT_ALIGN_S  15
/** I2S_RX_24_FILL_EN : R/W; bitpos: [16]; default: 0;
 *  1: store 24 channel bits to 32 bits. 0:store 24 channel bits to 24 bits.
 */
#define I2S_RX_24_FILL_EN    (BIT(16))
#define I2S_RX_24_FILL_EN_M  (I2S_RX_24_FILL_EN_V << I2S_RX_24_FILL_EN_S)
#define I2S_RX_24_FILL_EN_V  0x00000001U
#define I2S_RX_24_FILL_EN_S  16
/** I2S_RX_WS_IDLE_POL : R/W; bitpos: [17]; default: 0;
 *  0: WS should be 0 when receiving left channel data, and WS is 1in right channel.
 *  1: WS should be 1 when receiving left channel data, and WS is 0in right channel.
 */
#define I2S_RX_WS_IDLE_POL    (BIT(17))
#define I2S_RX_WS_IDLE_POL_M  (I2S_RX_WS_IDLE_POL_V << I2S_RX_WS_IDLE_POL_S)
#define I2S_RX_WS_IDLE_POL_V  0x00000001U
#define I2S_RX_WS_IDLE_POL_S  17
/** I2S_RX_BIT_ORDER : R/W; bitpos: [18]; default: 0;
 *  I2S Rx bit endian. 1:small endian, the LSB is received first. 0:big endian, the MSB
 *  is received first.
 */
#define I2S_RX_BIT_ORDER    (BIT(18))
#define I2S_RX_BIT_ORDER_M  (I2S_RX_BIT_ORDER_V << I2S_RX_BIT_ORDER_S)
#define I2S_RX_BIT_ORDER_V  0x00000001U
#define I2S_RX_BIT_ORDER_S  18
/** I2S_RX_TDM_EN : R/W; bitpos: [19]; default: 0;
 *  1: Enable I2S TDM Rx mode . 0: Disable.
 */
#define I2S_RX_TDM_EN    (BIT(19))
#define I2S_RX_TDM_EN_M  (I2S_RX_TDM_EN_V << I2S_RX_TDM_EN_S)
#define I2S_RX_TDM_EN_V  0x00000001U
#define I2S_RX_TDM_EN_S  19
/** I2S_RX_PDM_EN : R/W; bitpos: [20]; default: 0;
 *  1: Enable I2S PDM Rx mode . 0: Disable.
 */
#define I2S_RX_PDM_EN    (BIT(20))
#define I2S_RX_PDM_EN_M  (I2S_RX_PDM_EN_V << I2S_RX_PDM_EN_S)
#define I2S_RX_PDM_EN_V  0x00000001U
#define I2S_RX_PDM_EN_S  20
/** I2S_RX_BCK_DIV_NUM : R/W; bitpos: [26:21]; default: 6;
 *  Bit clock configuration bits in receiver mode.
 */
#define I2S_RX_BCK_DIV_NUM    0x0000003FU
#define I2S_RX_BCK_DIV_NUM_M  (I2S_RX_BCK_DIV_NUM_V << I2S_RX_BCK_DIV_NUM_S)
#define I2S_RX_BCK_DIV_NUM_V  0x0000003FU
#define I2S_RX_BCK_DIV_NUM_S  21

/** I2S_TX_CONF_REG register
 *  I2S TX configure register
 */
#define I2S_TX_CONF_REG (DR_REG_I2S_BASE + 0x24)
/** I2S_TX_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset transmitter
 */
#define I2S_TX_RESET    (BIT(0))
#define I2S_TX_RESET_M  (I2S_TX_RESET_V << I2S_TX_RESET_S)
#define I2S_TX_RESET_V  0x00000001U
#define I2S_TX_RESET_S  0
/** I2S_TX_FIFO_RESET : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset Tx AFIFO
 */
#define I2S_TX_FIFO_RESET    (BIT(1))
#define I2S_TX_FIFO_RESET_M  (I2S_TX_FIFO_RESET_V << I2S_TX_FIFO_RESET_S)
#define I2S_TX_FIFO_RESET_V  0x00000001U
#define I2S_TX_FIFO_RESET_S  1
/** I2S_TX_START : R/W/SC; bitpos: [2]; default: 0;
 *  Set this bit to start transmitting data
 */
#define I2S_TX_START    (BIT(2))
#define I2S_TX_START_M  (I2S_TX_START_V << I2S_TX_START_S)
#define I2S_TX_START_V  0x00000001U
#define I2S_TX_START_S  2
/** I2S_TX_SLAVE_MOD : R/W; bitpos: [3]; default: 0;
 *  Set this bit to enable slave transmitter mode
 */
#define I2S_TX_SLAVE_MOD    (BIT(3))
#define I2S_TX_SLAVE_MOD_M  (I2S_TX_SLAVE_MOD_V << I2S_TX_SLAVE_MOD_S)
#define I2S_TX_SLAVE_MOD_V  0x00000001U
#define I2S_TX_SLAVE_MOD_S  3
/** I2S_TX_STOP_EN : R/W; bitpos: [4]; default: 1;
 *  Set this bit to stop disable output BCK signal and WS signal when tx FIFO is emtpy
 */
#define I2S_TX_STOP_EN    (BIT(4))
#define I2S_TX_STOP_EN_M  (I2S_TX_STOP_EN_V << I2S_TX_STOP_EN_S)
#define I2S_TX_STOP_EN_V  0x00000001U
#define I2S_TX_STOP_EN_S  4
/** I2S_TX_CHAN_EQUAL : R/W; bitpos: [5]; default: 0;
 *  1: The value of Left channel data is equal to the value of right channel data in
 *  I2S TX mono mode or TDM channel select mode. 0: The invalid channel data is
 *  reg_i2s_single_data in I2S TX mono mode or TDM channel select mode.
 */
#define I2S_TX_CHAN_EQUAL    (BIT(5))
#define I2S_TX_CHAN_EQUAL_M  (I2S_TX_CHAN_EQUAL_V << I2S_TX_CHAN_EQUAL_S)
#define I2S_TX_CHAN_EQUAL_V  0x00000001U
#define I2S_TX_CHAN_EQUAL_S  5
/** I2S_TX_MONO : R/W; bitpos: [6]; default: 0;
 *  Set this bit to enable transmitter in mono mode
 */
#define I2S_TX_MONO    (BIT(6))
#define I2S_TX_MONO_M  (I2S_TX_MONO_V << I2S_TX_MONO_S)
#define I2S_TX_MONO_V  0x00000001U
#define I2S_TX_MONO_S  6
/** I2S_TX_BIG_ENDIAN : R/W; bitpos: [7]; default: 0;
 *  I2S Tx byte endian, 1: low addr value to high addr.  0: low addr with low addr
 *  value.
 */
#define I2S_TX_BIG_ENDIAN    (BIT(7))
#define I2S_TX_BIG_ENDIAN_M  (I2S_TX_BIG_ENDIAN_V << I2S_TX_BIG_ENDIAN_S)
#define I2S_TX_BIG_ENDIAN_V  0x00000001U
#define I2S_TX_BIG_ENDIAN_S  7
/** I2S_TX_UPDATE : R/W/SC; bitpos: [8]; default: 0;
 *  Set 1 to update I2S TX registers from APB clock domain to I2S TX clock domain. This
 *  bit will be cleared by hardware after update register done.
 */
#define I2S_TX_UPDATE    (BIT(8))
#define I2S_TX_UPDATE_M  (I2S_TX_UPDATE_V << I2S_TX_UPDATE_S)
#define I2S_TX_UPDATE_V  0x00000001U
#define I2S_TX_UPDATE_S  8
/** I2S_TX_MONO_FST_VLD : R/W; bitpos: [9]; default: 1;
 *  1: The first channel data value is valid in I2S TX mono mode.   0: The second
 *  channel data value is valid in I2S TX mono mode.
 */
#define I2S_TX_MONO_FST_VLD    (BIT(9))
#define I2S_TX_MONO_FST_VLD_M  (I2S_TX_MONO_FST_VLD_V << I2S_TX_MONO_FST_VLD_S)
#define I2S_TX_MONO_FST_VLD_V  0x00000001U
#define I2S_TX_MONO_FST_VLD_S  9
/** I2S_TX_PCM_CONF : R/W; bitpos: [11:10]; default: 0;
 *  I2S TX compress/decompress configuration bit. & 0 (atol): A-Law decompress, 1
 *  (ltoa) : A-Law compress, 2 (utol) : u-Law decompress, 3 (ltou) : u-Law compress. &
 */
#define I2S_TX_PCM_CONF    0x00000003U
#define I2S_TX_PCM_CONF_M  (I2S_TX_PCM_CONF_V << I2S_TX_PCM_CONF_S)
#define I2S_TX_PCM_CONF_V  0x00000003U
#define I2S_TX_PCM_CONF_S  10
/** I2S_TX_PCM_BYPASS : R/W; bitpos: [12]; default: 1;
 *  Set this bit to bypass  Compress/Decompress module for transmitted data.
 */
#define I2S_TX_PCM_BYPASS    (BIT(12))
#define I2S_TX_PCM_BYPASS_M  (I2S_TX_PCM_BYPASS_V << I2S_TX_PCM_BYPASS_S)
#define I2S_TX_PCM_BYPASS_V  0x00000001U
#define I2S_TX_PCM_BYPASS_S  12
/** I2S_TX_MSB_SHIFT : R/W; bitpos: [13]; default: 1;
 *  Set this bit to enable transmitter in Phillips standard mode
 */
#define I2S_TX_MSB_SHIFT    (BIT(13))
#define I2S_TX_MSB_SHIFT_M  (I2S_TX_MSB_SHIFT_V << I2S_TX_MSB_SHIFT_S)
#define I2S_TX_MSB_SHIFT_V  0x00000001U
#define I2S_TX_MSB_SHIFT_S  13
/** I2S_TX_BCK_NO_DLY : R/W; bitpos: [14]; default: 1;
 *  1: BCK is not delayed to generate pos/neg edge in master mode. 0: BCK is delayed to
 *  generate pos/neg edge in master mode.
 */
#define I2S_TX_BCK_NO_DLY    (BIT(14))
#define I2S_TX_BCK_NO_DLY_M  (I2S_TX_BCK_NO_DLY_V << I2S_TX_BCK_NO_DLY_S)
#define I2S_TX_BCK_NO_DLY_V  0x00000001U
#define I2S_TX_BCK_NO_DLY_S  14
/** I2S_TX_LEFT_ALIGN : R/W; bitpos: [15]; default: 1;
 *  1: I2S TX left alignment mode. 0: I2S TX right alignment mode.
 */
#define I2S_TX_LEFT_ALIGN    (BIT(15))
#define I2S_TX_LEFT_ALIGN_M  (I2S_TX_LEFT_ALIGN_V << I2S_TX_LEFT_ALIGN_S)
#define I2S_TX_LEFT_ALIGN_V  0x00000001U
#define I2S_TX_LEFT_ALIGN_S  15
/** I2S_TX_24_FILL_EN : R/W; bitpos: [16]; default: 0;
 *  1: Sent 32 bits in 24 channel bits mode. 0: Sent 24 bits in 24 channel bits mode
 */
#define I2S_TX_24_FILL_EN    (BIT(16))
#define I2S_TX_24_FILL_EN_M  (I2S_TX_24_FILL_EN_V << I2S_TX_24_FILL_EN_S)
#define I2S_TX_24_FILL_EN_V  0x00000001U
#define I2S_TX_24_FILL_EN_S  16
/** I2S_TX_WS_IDLE_POL : R/W; bitpos: [17]; default: 0;
 *  0: WS should be 0 when sending left channel data, and WS is 1in right channel.  1:
 *  WS should be 1 when sending left channel data, and WS is 0in right channel.
 */
#define I2S_TX_WS_IDLE_POL    (BIT(17))
#define I2S_TX_WS_IDLE_POL_M  (I2S_TX_WS_IDLE_POL_V << I2S_TX_WS_IDLE_POL_S)
#define I2S_TX_WS_IDLE_POL_V  0x00000001U
#define I2S_TX_WS_IDLE_POL_S  17
/** I2S_TX_BIT_ORDER : R/W; bitpos: [18]; default: 0;
 *  I2S Tx bit endian. 1:small endian, the LSB is sent first. 0:big endian, the MSB is
 *  sent first.
 */
#define I2S_TX_BIT_ORDER    (BIT(18))
#define I2S_TX_BIT_ORDER_M  (I2S_TX_BIT_ORDER_V << I2S_TX_BIT_ORDER_S)
#define I2S_TX_BIT_ORDER_V  0x00000001U
#define I2S_TX_BIT_ORDER_S  18
/** I2S_TX_TDM_EN : R/W; bitpos: [19]; default: 0;
 *  1: Enable I2S TDM Tx mode . 0: Disable.
 */
#define I2S_TX_TDM_EN    (BIT(19))
#define I2S_TX_TDM_EN_M  (I2S_TX_TDM_EN_V << I2S_TX_TDM_EN_S)
#define I2S_TX_TDM_EN_V  0x00000001U
#define I2S_TX_TDM_EN_S  19
/** I2S_TX_PDM_EN : R/W; bitpos: [20]; default: 0;
 *  1: Enable I2S PDM Tx mode . 0: Disable.
 */
#define I2S_TX_PDM_EN    (BIT(20))
#define I2S_TX_PDM_EN_M  (I2S_TX_PDM_EN_V << I2S_TX_PDM_EN_S)
#define I2S_TX_PDM_EN_V  0x00000001U
#define I2S_TX_PDM_EN_S  20
/** I2S_TX_BCK_DIV_NUM : R/W; bitpos: [26:21]; default: 6;
 *  Bit clock configuration bits in transmitter mode.
 */
#define I2S_TX_BCK_DIV_NUM    0x0000003FU
#define I2S_TX_BCK_DIV_NUM_M  (I2S_TX_BCK_DIV_NUM_V << I2S_TX_BCK_DIV_NUM_S)
#define I2S_TX_BCK_DIV_NUM_V  0x0000003FU
#define I2S_TX_BCK_DIV_NUM_S  21
/** I2S_TX_CHAN_MOD : R/W; bitpos: [29:27]; default: 0;
 *  I2S transmitter channel mode configuration bits.
 */
#define I2S_TX_CHAN_MOD    0x00000007U
#define I2S_TX_CHAN_MOD_M  (I2S_TX_CHAN_MOD_V << I2S_TX_CHAN_MOD_S)
#define I2S_TX_CHAN_MOD_V  0x00000007U
#define I2S_TX_CHAN_MOD_S  27
/** I2S_SIG_LOOPBACK : R/W; bitpos: [30]; default: 0;
 *  Enable signal loop back mode with transmitter module and receiver module sharing
 *  the same WS and BCK signals.
 */
#define I2S_SIG_LOOPBACK    (BIT(30))
#define I2S_SIG_LOOPBACK_M  (I2S_SIG_LOOPBACK_V << I2S_SIG_LOOPBACK_S)
#define I2S_SIG_LOOPBACK_V  0x00000001U
#define I2S_SIG_LOOPBACK_S  30

/** I2S_RX_CONF1_REG register
 *  I2S RX configure register 1
 */
#define I2S_RX_CONF1_REG (DR_REG_I2S_BASE + 0x28)
/** I2S_RX_TDM_WS_WIDTH : R/W; bitpos: [8:0]; default: 0;
 *  The width of rx_ws_out at idle level in TDM mode is (I2S_RX_TDM_WS_WIDTH[8:0] +1) *
 *  T_bck
 */
#define I2S_RX_TDM_WS_WIDTH    0x000001FFU
#define I2S_RX_TDM_WS_WIDTH_M  (I2S_RX_TDM_WS_WIDTH_V << I2S_RX_TDM_WS_WIDTH_S)
#define I2S_RX_TDM_WS_WIDTH_V  0x000001FFU
#define I2S_RX_TDM_WS_WIDTH_S  0
/** I2S_RX_BITS_MOD : R/W; bitpos: [18:14]; default: 15;
 *  Set the bits to configure the valid data bit length of I2S receiver channel. 7: all
 *  the valid channel data is in 8-bit-mode. 15: all the valid channel data is in
 *  16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid
 *  channel data is in 32-bit-mode.
 */
#define I2S_RX_BITS_MOD    0x0000001FU
#define I2S_RX_BITS_MOD_M  (I2S_RX_BITS_MOD_V << I2S_RX_BITS_MOD_S)
#define I2S_RX_BITS_MOD_V  0x0000001FU
#define I2S_RX_BITS_MOD_S  14
/** I2S_RX_HALF_SAMPLE_BITS : R/W; bitpos: [26:19]; default: 15;
 *  I2S Rx half sample bits -1.
 */
#define I2S_RX_HALF_SAMPLE_BITS    0x000000FFU
#define I2S_RX_HALF_SAMPLE_BITS_M  (I2S_RX_HALF_SAMPLE_BITS_V << I2S_RX_HALF_SAMPLE_BITS_S)
#define I2S_RX_HALF_SAMPLE_BITS_V  0x000000FFU
#define I2S_RX_HALF_SAMPLE_BITS_S  19
/** I2S_RX_TDM_CHAN_BITS : R/W; bitpos: [31:27]; default: 15;
 *  The Rx bit number for each channel minus 1in TDM mode.
 */
#define I2S_RX_TDM_CHAN_BITS    0x0000001FU
#define I2S_RX_TDM_CHAN_BITS_M  (I2S_RX_TDM_CHAN_BITS_V << I2S_RX_TDM_CHAN_BITS_S)
#define I2S_RX_TDM_CHAN_BITS_V  0x0000001FU
#define I2S_RX_TDM_CHAN_BITS_S  27

/** I2S_TX_CONF1_REG register
 *  I2S TX configure register 1
 */
#define I2S_TX_CONF1_REG (DR_REG_I2S_BASE + 0x2c)
/** I2S_TX_TDM_WS_WIDTH : R/W; bitpos: [8:0]; default: 0;
 *  The width of tx_ws_out at idle level in TDM mode is (I2S_TX_TDM_WS_WIDTH[8:0] +1) *
 *  T_bck
 */
#define I2S_TX_TDM_WS_WIDTH    0x000001FFU
#define I2S_TX_TDM_WS_WIDTH_M  (I2S_TX_TDM_WS_WIDTH_V << I2S_TX_TDM_WS_WIDTH_S)
#define I2S_TX_TDM_WS_WIDTH_V  0x000001FFU
#define I2S_TX_TDM_WS_WIDTH_S  0
/** I2S_TX_BITS_MOD : R/W; bitpos: [18:14]; default: 15;
 *  Set the bits to configure the valid data bit length of I2S transmitter channel. 7:
 *  all the valid channel data is in 8-bit-mode. 15: all the valid channel data is in
 *  16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid
 *  channel data is in 32-bit-mode.
 */
#define I2S_TX_BITS_MOD    0x0000001FU
#define I2S_TX_BITS_MOD_M  (I2S_TX_BITS_MOD_V << I2S_TX_BITS_MOD_S)
#define I2S_TX_BITS_MOD_V  0x0000001FU
#define I2S_TX_BITS_MOD_S  14
/** I2S_TX_HALF_SAMPLE_BITS : R/W; bitpos: [26:19]; default: 15;
 *  I2S Tx half sample bits -1.
 */
#define I2S_TX_HALF_SAMPLE_BITS    0x000000FFU
#define I2S_TX_HALF_SAMPLE_BITS_M  (I2S_TX_HALF_SAMPLE_BITS_V << I2S_TX_HALF_SAMPLE_BITS_S)
#define I2S_TX_HALF_SAMPLE_BITS_V  0x000000FFU
#define I2S_TX_HALF_SAMPLE_BITS_S  19
/** I2S_TX_TDM_CHAN_BITS : R/W; bitpos: [31:27]; default: 15;
 *  The Tx bit number for each channel minus 1in TDM mode.
 */
#define I2S_TX_TDM_CHAN_BITS    0x0000001FU
#define I2S_TX_TDM_CHAN_BITS_M  (I2S_TX_TDM_CHAN_BITS_V << I2S_TX_TDM_CHAN_BITS_S)
#define I2S_TX_TDM_CHAN_BITS_V  0x0000001FU
#define I2S_TX_TDM_CHAN_BITS_S  27

/** I2S_TX_PCM2PDM_CONF_REG register
 *  I2S TX PCM2PDM configuration register
 */
#define I2S_TX_PCM2PDM_CONF_REG (DR_REG_I2S_BASE + 0x40)
/** I2S_TX_PDM_HP_BYPASS : R/W; bitpos: [0]; default: 0;
 *  I2S TX PDM bypass hp filter or not. The option has been removed.
 */
#define I2S_TX_PDM_HP_BYPASS    (BIT(0))
#define I2S_TX_PDM_HP_BYPASS_M  (I2S_TX_PDM_HP_BYPASS_V << I2S_TX_PDM_HP_BYPASS_S)
#define I2S_TX_PDM_HP_BYPASS_V  0x00000001U
#define I2S_TX_PDM_HP_BYPASS_S  0
/** I2S_TX_PDM_SINC_OSR2 : R/W; bitpos: [4:1]; default: 2;
 *  I2S TX PDM OSR2 value
 */
#define I2S_TX_PDM_SINC_OSR2    0x0000000FU
#define I2S_TX_PDM_SINC_OSR2_M  (I2S_TX_PDM_SINC_OSR2_V << I2S_TX_PDM_SINC_OSR2_S)
#define I2S_TX_PDM_SINC_OSR2_V  0x0000000FU
#define I2S_TX_PDM_SINC_OSR2_S  1
/** I2S_TX_PDM_PRESCALE : R/W; bitpos: [12:5]; default: 0;
 *  I2S TX PDM prescale for sigmadelta
 */
#define I2S_TX_PDM_PRESCALE    0x000000FFU
#define I2S_TX_PDM_PRESCALE_M  (I2S_TX_PDM_PRESCALE_V << I2S_TX_PDM_PRESCALE_S)
#define I2S_TX_PDM_PRESCALE_V  0x000000FFU
#define I2S_TX_PDM_PRESCALE_S  5
/** I2S_TX_PDM_HP_IN_SHIFT : R/W; bitpos: [14:13]; default: 1;
 *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
 */
#define I2S_TX_PDM_HP_IN_SHIFT    0x00000003U
#define I2S_TX_PDM_HP_IN_SHIFT_M  (I2S_TX_PDM_HP_IN_SHIFT_V << I2S_TX_PDM_HP_IN_SHIFT_S)
#define I2S_TX_PDM_HP_IN_SHIFT_V  0x00000003U
#define I2S_TX_PDM_HP_IN_SHIFT_S  13
/** I2S_TX_PDM_LP_IN_SHIFT : R/W; bitpos: [16:15]; default: 1;
 *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
 */
#define I2S_TX_PDM_LP_IN_SHIFT    0x00000003U
#define I2S_TX_PDM_LP_IN_SHIFT_M  (I2S_TX_PDM_LP_IN_SHIFT_V << I2S_TX_PDM_LP_IN_SHIFT_S)
#define I2S_TX_PDM_LP_IN_SHIFT_V  0x00000003U
#define I2S_TX_PDM_LP_IN_SHIFT_S  15
/** I2S_TX_PDM_SINC_IN_SHIFT : R/W; bitpos: [18:17]; default: 1;
 *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
 */
#define I2S_TX_PDM_SINC_IN_SHIFT    0x00000003U
#define I2S_TX_PDM_SINC_IN_SHIFT_M  (I2S_TX_PDM_SINC_IN_SHIFT_V << I2S_TX_PDM_SINC_IN_SHIFT_S)
#define I2S_TX_PDM_SINC_IN_SHIFT_V  0x00000003U
#define I2S_TX_PDM_SINC_IN_SHIFT_S  17
/** I2S_TX_PDM_SIGMADELTA_IN_SHIFT : R/W; bitpos: [20:19]; default: 1;
 *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
 */
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT    0x00000003U
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_M  (I2S_TX_PDM_SIGMADELTA_IN_SHIFT_V << I2S_TX_PDM_SIGMADELTA_IN_SHIFT_S)
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_V  0x00000003U
#define I2S_TX_PDM_SIGMADELTA_IN_SHIFT_S  19
/** I2S_TX_PDM_SIGMADELTA_DITHER2 : R/W; bitpos: [21]; default: 0;
 *  I2S TX PDM sigmadelta dither2 value
 */
#define I2S_TX_PDM_SIGMADELTA_DITHER2    (BIT(21))
#define I2S_TX_PDM_SIGMADELTA_DITHER2_M  (I2S_TX_PDM_SIGMADELTA_DITHER2_V << I2S_TX_PDM_SIGMADELTA_DITHER2_S)
#define I2S_TX_PDM_SIGMADELTA_DITHER2_V  0x00000001U
#define I2S_TX_PDM_SIGMADELTA_DITHER2_S  21
/** I2S_TX_PDM_SIGMADELTA_DITHER : R/W; bitpos: [22]; default: 1;
 *  I2S TX PDM sigmadelta dither value
 */
#define I2S_TX_PDM_SIGMADELTA_DITHER    (BIT(22))
#define I2S_TX_PDM_SIGMADELTA_DITHER_M  (I2S_TX_PDM_SIGMADELTA_DITHER_V << I2S_TX_PDM_SIGMADELTA_DITHER_S)
#define I2S_TX_PDM_SIGMADELTA_DITHER_V  0x00000001U
#define I2S_TX_PDM_SIGMADELTA_DITHER_S  22
/** I2S_TX_PDM_DAC_2OUT_EN : R/W; bitpos: [23]; default: 0;
 *  I2S TX PDM dac mode enable
 */
#define I2S_TX_PDM_DAC_2OUT_EN    (BIT(23))
#define I2S_TX_PDM_DAC_2OUT_EN_M  (I2S_TX_PDM_DAC_2OUT_EN_V << I2S_TX_PDM_DAC_2OUT_EN_S)
#define I2S_TX_PDM_DAC_2OUT_EN_V  0x00000001U
#define I2S_TX_PDM_DAC_2OUT_EN_S  23
/** I2S_TX_PDM_DAC_MODE_EN : R/W; bitpos: [24]; default: 0;
 *  I2S TX PDM dac 2channel enable
 */
#define I2S_TX_PDM_DAC_MODE_EN    (BIT(24))
#define I2S_TX_PDM_DAC_MODE_EN_M  (I2S_TX_PDM_DAC_MODE_EN_V << I2S_TX_PDM_DAC_MODE_EN_S)
#define I2S_TX_PDM_DAC_MODE_EN_V  0x00000001U
#define I2S_TX_PDM_DAC_MODE_EN_S  24
/** I2S_PCM2PDM_CONV_EN : R/W; bitpos: [25]; default: 0;
 *  I2S TX PDM Converter enable
 */
#define I2S_PCM2PDM_CONV_EN    (BIT(25))
#define I2S_PCM2PDM_CONV_EN_M  (I2S_PCM2PDM_CONV_EN_V << I2S_PCM2PDM_CONV_EN_S)
#define I2S_PCM2PDM_CONV_EN_V  0x00000001U
#define I2S_PCM2PDM_CONV_EN_S  25

/** I2S_TX_PCM2PDM_CONF1_REG register
 *  I2S TX PCM2PDM configuration register
 */
#define I2S_TX_PCM2PDM_CONF1_REG (DR_REG_I2S_BASE + 0x44)
/** I2S_TX_PDM_FP : R/W; bitpos: [9:0]; default: 960;
 *  I2S TX PDM Fp
 */
#define I2S_TX_PDM_FP    0x000003FFU
#define I2S_TX_PDM_FP_M  (I2S_TX_PDM_FP_V << I2S_TX_PDM_FP_S)
#define I2S_TX_PDM_FP_V  0x000003FFU
#define I2S_TX_PDM_FP_S  0
/** I2S_TX_PDM_FS : R/W; bitpos: [19:10]; default: 480;
 *  I2S TX PDM Fs
 */
#define I2S_TX_PDM_FS    0x000003FFU
#define I2S_TX_PDM_FS_M  (I2S_TX_PDM_FS_V << I2S_TX_PDM_FS_S)
#define I2S_TX_PDM_FS_V  0x000003FFU
#define I2S_TX_PDM_FS_S  10
/** I2S_TX_IIR_HP_MULT12_5 : R/W; bitpos: [22:20]; default: 7;
 *  The fourth parameter of PDM TX IIR_HP filter stage 2 is (504 +
 *  I2S_TX_IIR_HP_MULT12_5[2:0])
 */
#define I2S_TX_IIR_HP_MULT12_5    0x00000007U
#define I2S_TX_IIR_HP_MULT12_5_M  (I2S_TX_IIR_HP_MULT12_5_V << I2S_TX_IIR_HP_MULT12_5_S)
#define I2S_TX_IIR_HP_MULT12_5_V  0x00000007U
#define I2S_TX_IIR_HP_MULT12_5_S  20
/** I2S_TX_IIR_HP_MULT12_0 : R/W; bitpos: [25:23]; default: 7;
 *  The fourth parameter of PDM TX IIR_HP filter stage 1 is (504 +
 *  I2S_TX_IIR_HP_MULT12_0[2:0])
 */
#define I2S_TX_IIR_HP_MULT12_0    0x00000007U
#define I2S_TX_IIR_HP_MULT12_0_M  (I2S_TX_IIR_HP_MULT12_0_V << I2S_TX_IIR_HP_MULT12_0_S)
#define I2S_TX_IIR_HP_MULT12_0_V  0x00000007U
#define I2S_TX_IIR_HP_MULT12_0_S  23

/** I2S_RX_PDM2PCM_CONF_REG register
 *  I2S RX configure register
 */
#define I2S_RX_PDM2PCM_CONF_REG (DR_REG_I2S_BASE + 0x48)
/** I2S_RX_PDM2PCM_EN : R/W; bitpos: [19]; default: 0;
 *  1: Enable PDM2PCM RX mode. 0: DIsable.
 */
#define I2S_RX_PDM2PCM_EN    (BIT(19))
#define I2S_RX_PDM2PCM_EN_M  (I2S_RX_PDM2PCM_EN_V << I2S_RX_PDM2PCM_EN_S)
#define I2S_RX_PDM2PCM_EN_V  0x00000001U
#define I2S_RX_PDM2PCM_EN_S  19
/** I2S_RX_PDM_SINC_DSR_16_EN : R/W; bitpos: [20]; default: 0;
 *  Configure the down sampling rate of PDM RX filter group1 module. 1: The  down
 *  sampling rate is 128. 0: down sampling rate is 64.
 */
#define I2S_RX_PDM_SINC_DSR_16_EN    (BIT(20))
#define I2S_RX_PDM_SINC_DSR_16_EN_M  (I2S_RX_PDM_SINC_DSR_16_EN_V << I2S_RX_PDM_SINC_DSR_16_EN_S)
#define I2S_RX_PDM_SINC_DSR_16_EN_V  0x00000001U
#define I2S_RX_PDM_SINC_DSR_16_EN_S  20
/** I2S_RX_PDM2PCM_AMPLIFY_NUM : R/W; bitpos: [24:21]; default: 1;
 *  Configure PDM RX amplify number.
 */
#define I2S_RX_PDM2PCM_AMPLIFY_NUM    0x0000000FU
#define I2S_RX_PDM2PCM_AMPLIFY_NUM_M  (I2S_RX_PDM2PCM_AMPLIFY_NUM_V << I2S_RX_PDM2PCM_AMPLIFY_NUM_S)
#define I2S_RX_PDM2PCM_AMPLIFY_NUM_V  0x0000000FU
#define I2S_RX_PDM2PCM_AMPLIFY_NUM_S  21
/** I2S_RX_PDM_HP_BYPASS : R/W; bitpos: [25]; default: 0;
 *  I2S PDM RX bypass hp filter or not.
 */
#define I2S_RX_PDM_HP_BYPASS    (BIT(25))
#define I2S_RX_PDM_HP_BYPASS_M  (I2S_RX_PDM_HP_BYPASS_V << I2S_RX_PDM_HP_BYPASS_S)
#define I2S_RX_PDM_HP_BYPASS_V  0x00000001U
#define I2S_RX_PDM_HP_BYPASS_S  25
/** I2S_RX_IIR_HP_MULT12_5 : R/W; bitpos: [28:26]; default: 6;
 *  The fourth parameter of PDM RX IIR_HP filter stage 2 is (504 +
 *  LP_I2S_RX_IIR_HP_MULT12_5[2:0])
 */
#define I2S_RX_IIR_HP_MULT12_5    0x00000007U
#define I2S_RX_IIR_HP_MULT12_5_M  (I2S_RX_IIR_HP_MULT12_5_V << I2S_RX_IIR_HP_MULT12_5_S)
#define I2S_RX_IIR_HP_MULT12_5_V  0x00000007U
#define I2S_RX_IIR_HP_MULT12_5_S  26
/** I2S_RX_IIR_HP_MULT12_0 : R/W; bitpos: [31:29]; default: 7;
 *  The fourth parameter of PDM RX IIR_HP filter stage 1 is (504 +
 *  LP_I2S_RX_IIR_HP_MULT12_0[2:0])
 */
#define I2S_RX_IIR_HP_MULT12_0    0x00000007U
#define I2S_RX_IIR_HP_MULT12_0_M  (I2S_RX_IIR_HP_MULT12_0_V << I2S_RX_IIR_HP_MULT12_0_S)
#define I2S_RX_IIR_HP_MULT12_0_V  0x00000007U
#define I2S_RX_IIR_HP_MULT12_0_S  29

/** I2S_RX_TDM_CTRL_REG register
 *  I2S TX TDM mode control register
 */
#define I2S_RX_TDM_CTRL_REG (DR_REG_I2S_BASE + 0x50)
/** I2S_RX_TDM_PDM_CHAN0_EN : R/W; bitpos: [0]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN0_EN    (BIT(0))
#define I2S_RX_TDM_PDM_CHAN0_EN_M  (I2S_RX_TDM_PDM_CHAN0_EN_V << I2S_RX_TDM_PDM_CHAN0_EN_S)
#define I2S_RX_TDM_PDM_CHAN0_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN0_EN_S  0
/** I2S_RX_TDM_PDM_CHAN1_EN : R/W; bitpos: [1]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN1_EN    (BIT(1))
#define I2S_RX_TDM_PDM_CHAN1_EN_M  (I2S_RX_TDM_PDM_CHAN1_EN_V << I2S_RX_TDM_PDM_CHAN1_EN_S)
#define I2S_RX_TDM_PDM_CHAN1_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN1_EN_S  1
/** I2S_RX_TDM_PDM_CHAN2_EN : R/W; bitpos: [2]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN2_EN    (BIT(2))
#define I2S_RX_TDM_PDM_CHAN2_EN_M  (I2S_RX_TDM_PDM_CHAN2_EN_V << I2S_RX_TDM_PDM_CHAN2_EN_S)
#define I2S_RX_TDM_PDM_CHAN2_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN2_EN_S  2
/** I2S_RX_TDM_PDM_CHAN3_EN : R/W; bitpos: [3]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN3_EN    (BIT(3))
#define I2S_RX_TDM_PDM_CHAN3_EN_M  (I2S_RX_TDM_PDM_CHAN3_EN_V << I2S_RX_TDM_PDM_CHAN3_EN_S)
#define I2S_RX_TDM_PDM_CHAN3_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN3_EN_S  3
/** I2S_RX_TDM_PDM_CHAN4_EN : R/W; bitpos: [4]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN4_EN    (BIT(4))
#define I2S_RX_TDM_PDM_CHAN4_EN_M  (I2S_RX_TDM_PDM_CHAN4_EN_V << I2S_RX_TDM_PDM_CHAN4_EN_S)
#define I2S_RX_TDM_PDM_CHAN4_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN4_EN_S  4
/** I2S_RX_TDM_PDM_CHAN5_EN : R/W; bitpos: [5]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN5_EN    (BIT(5))
#define I2S_RX_TDM_PDM_CHAN5_EN_M  (I2S_RX_TDM_PDM_CHAN5_EN_V << I2S_RX_TDM_PDM_CHAN5_EN_S)
#define I2S_RX_TDM_PDM_CHAN5_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN5_EN_S  5
/** I2S_RX_TDM_PDM_CHAN6_EN : R/W; bitpos: [6]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN6_EN    (BIT(6))
#define I2S_RX_TDM_PDM_CHAN6_EN_M  (I2S_RX_TDM_PDM_CHAN6_EN_V << I2S_RX_TDM_PDM_CHAN6_EN_S)
#define I2S_RX_TDM_PDM_CHAN6_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN6_EN_S  6
/** I2S_RX_TDM_PDM_CHAN7_EN : R/W; bitpos: [7]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define I2S_RX_TDM_PDM_CHAN7_EN    (BIT(7))
#define I2S_RX_TDM_PDM_CHAN7_EN_M  (I2S_RX_TDM_PDM_CHAN7_EN_V << I2S_RX_TDM_PDM_CHAN7_EN_S)
#define I2S_RX_TDM_PDM_CHAN7_EN_V  0x00000001U
#define I2S_RX_TDM_PDM_CHAN7_EN_S  7
/** I2S_RX_TDM_CHAN8_EN : R/W; bitpos: [8]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN8_EN    (BIT(8))
#define I2S_RX_TDM_CHAN8_EN_M  (I2S_RX_TDM_CHAN8_EN_V << I2S_RX_TDM_CHAN8_EN_S)
#define I2S_RX_TDM_CHAN8_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN8_EN_S  8
/** I2S_RX_TDM_CHAN9_EN : R/W; bitpos: [9]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN9_EN    (BIT(9))
#define I2S_RX_TDM_CHAN9_EN_M  (I2S_RX_TDM_CHAN9_EN_V << I2S_RX_TDM_CHAN9_EN_S)
#define I2S_RX_TDM_CHAN9_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN9_EN_S  9
/** I2S_RX_TDM_CHAN10_EN : R/W; bitpos: [10]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN10_EN    (BIT(10))
#define I2S_RX_TDM_CHAN10_EN_M  (I2S_RX_TDM_CHAN10_EN_V << I2S_RX_TDM_CHAN10_EN_S)
#define I2S_RX_TDM_CHAN10_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN10_EN_S  10
/** I2S_RX_TDM_CHAN11_EN : R/W; bitpos: [11]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN11_EN    (BIT(11))
#define I2S_RX_TDM_CHAN11_EN_M  (I2S_RX_TDM_CHAN11_EN_V << I2S_RX_TDM_CHAN11_EN_S)
#define I2S_RX_TDM_CHAN11_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN11_EN_S  11
/** I2S_RX_TDM_CHAN12_EN : R/W; bitpos: [12]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN12_EN    (BIT(12))
#define I2S_RX_TDM_CHAN12_EN_M  (I2S_RX_TDM_CHAN12_EN_V << I2S_RX_TDM_CHAN12_EN_S)
#define I2S_RX_TDM_CHAN12_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN12_EN_S  12
/** I2S_RX_TDM_CHAN13_EN : R/W; bitpos: [13]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN13_EN    (BIT(13))
#define I2S_RX_TDM_CHAN13_EN_M  (I2S_RX_TDM_CHAN13_EN_V << I2S_RX_TDM_CHAN13_EN_S)
#define I2S_RX_TDM_CHAN13_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN13_EN_S  13
/** I2S_RX_TDM_CHAN14_EN : R/W; bitpos: [14]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN14_EN    (BIT(14))
#define I2S_RX_TDM_CHAN14_EN_M  (I2S_RX_TDM_CHAN14_EN_V << I2S_RX_TDM_CHAN14_EN_S)
#define I2S_RX_TDM_CHAN14_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN14_EN_S  14
/** I2S_RX_TDM_CHAN15_EN : R/W; bitpos: [15]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
 *  in this channel.
 */
#define I2S_RX_TDM_CHAN15_EN    (BIT(15))
#define I2S_RX_TDM_CHAN15_EN_M  (I2S_RX_TDM_CHAN15_EN_V << I2S_RX_TDM_CHAN15_EN_S)
#define I2S_RX_TDM_CHAN15_EN_V  0x00000001U
#define I2S_RX_TDM_CHAN15_EN_S  15
/** I2S_RX_TDM_TOT_CHAN_NUM : R/W; bitpos: [19:16]; default: 0;
 *  The total channel number of I2S TX TDM mode.
 */
#define I2S_RX_TDM_TOT_CHAN_NUM    0x0000000FU
#define I2S_RX_TDM_TOT_CHAN_NUM_M  (I2S_RX_TDM_TOT_CHAN_NUM_V << I2S_RX_TDM_TOT_CHAN_NUM_S)
#define I2S_RX_TDM_TOT_CHAN_NUM_V  0x0000000FU
#define I2S_RX_TDM_TOT_CHAN_NUM_S  16

/** I2S_TX_TDM_CTRL_REG register
 *  I2S TX TDM mode control register
 */
#define I2S_TX_TDM_CTRL_REG (DR_REG_I2S_BASE + 0x54)
/** I2S_TX_TDM_CHAN0_EN : R/W; bitpos: [0]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN0_EN    (BIT(0))
#define I2S_TX_TDM_CHAN0_EN_M  (I2S_TX_TDM_CHAN0_EN_V << I2S_TX_TDM_CHAN0_EN_S)
#define I2S_TX_TDM_CHAN0_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN0_EN_S  0
/** I2S_TX_TDM_CHAN1_EN : R/W; bitpos: [1]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN1_EN    (BIT(1))
#define I2S_TX_TDM_CHAN1_EN_M  (I2S_TX_TDM_CHAN1_EN_V << I2S_TX_TDM_CHAN1_EN_S)
#define I2S_TX_TDM_CHAN1_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN1_EN_S  1
/** I2S_TX_TDM_CHAN2_EN : R/W; bitpos: [2]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN2_EN    (BIT(2))
#define I2S_TX_TDM_CHAN2_EN_M  (I2S_TX_TDM_CHAN2_EN_V << I2S_TX_TDM_CHAN2_EN_S)
#define I2S_TX_TDM_CHAN2_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN2_EN_S  2
/** I2S_TX_TDM_CHAN3_EN : R/W; bitpos: [3]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN3_EN    (BIT(3))
#define I2S_TX_TDM_CHAN3_EN_M  (I2S_TX_TDM_CHAN3_EN_V << I2S_TX_TDM_CHAN3_EN_S)
#define I2S_TX_TDM_CHAN3_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN3_EN_S  3
/** I2S_TX_TDM_CHAN4_EN : R/W; bitpos: [4]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN4_EN    (BIT(4))
#define I2S_TX_TDM_CHAN4_EN_M  (I2S_TX_TDM_CHAN4_EN_V << I2S_TX_TDM_CHAN4_EN_S)
#define I2S_TX_TDM_CHAN4_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN4_EN_S  4
/** I2S_TX_TDM_CHAN5_EN : R/W; bitpos: [5]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN5_EN    (BIT(5))
#define I2S_TX_TDM_CHAN5_EN_M  (I2S_TX_TDM_CHAN5_EN_V << I2S_TX_TDM_CHAN5_EN_S)
#define I2S_TX_TDM_CHAN5_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN5_EN_S  5
/** I2S_TX_TDM_CHAN6_EN : R/W; bitpos: [6]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN6_EN    (BIT(6))
#define I2S_TX_TDM_CHAN6_EN_M  (I2S_TX_TDM_CHAN6_EN_V << I2S_TX_TDM_CHAN6_EN_S)
#define I2S_TX_TDM_CHAN6_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN6_EN_S  6
/** I2S_TX_TDM_CHAN7_EN : R/W; bitpos: [7]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN7_EN    (BIT(7))
#define I2S_TX_TDM_CHAN7_EN_M  (I2S_TX_TDM_CHAN7_EN_V << I2S_TX_TDM_CHAN7_EN_S)
#define I2S_TX_TDM_CHAN7_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN7_EN_S  7
/** I2S_TX_TDM_CHAN8_EN : R/W; bitpos: [8]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN8_EN    (BIT(8))
#define I2S_TX_TDM_CHAN8_EN_M  (I2S_TX_TDM_CHAN8_EN_V << I2S_TX_TDM_CHAN8_EN_S)
#define I2S_TX_TDM_CHAN8_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN8_EN_S  8
/** I2S_TX_TDM_CHAN9_EN : R/W; bitpos: [9]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN9_EN    (BIT(9))
#define I2S_TX_TDM_CHAN9_EN_M  (I2S_TX_TDM_CHAN9_EN_V << I2S_TX_TDM_CHAN9_EN_S)
#define I2S_TX_TDM_CHAN9_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN9_EN_S  9
/** I2S_TX_TDM_CHAN10_EN : R/W; bitpos: [10]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN10_EN    (BIT(10))
#define I2S_TX_TDM_CHAN10_EN_M  (I2S_TX_TDM_CHAN10_EN_V << I2S_TX_TDM_CHAN10_EN_S)
#define I2S_TX_TDM_CHAN10_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN10_EN_S  10
/** I2S_TX_TDM_CHAN11_EN : R/W; bitpos: [11]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN11_EN    (BIT(11))
#define I2S_TX_TDM_CHAN11_EN_M  (I2S_TX_TDM_CHAN11_EN_V << I2S_TX_TDM_CHAN11_EN_S)
#define I2S_TX_TDM_CHAN11_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN11_EN_S  11
/** I2S_TX_TDM_CHAN12_EN : R/W; bitpos: [12]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN12_EN    (BIT(12))
#define I2S_TX_TDM_CHAN12_EN_M  (I2S_TX_TDM_CHAN12_EN_V << I2S_TX_TDM_CHAN12_EN_S)
#define I2S_TX_TDM_CHAN12_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN12_EN_S  12
/** I2S_TX_TDM_CHAN13_EN : R/W; bitpos: [13]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN13_EN    (BIT(13))
#define I2S_TX_TDM_CHAN13_EN_M  (I2S_TX_TDM_CHAN13_EN_V << I2S_TX_TDM_CHAN13_EN_S)
#define I2S_TX_TDM_CHAN13_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN13_EN_S  13
/** I2S_TX_TDM_CHAN14_EN : R/W; bitpos: [14]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN14_EN    (BIT(14))
#define I2S_TX_TDM_CHAN14_EN_M  (I2S_TX_TDM_CHAN14_EN_V << I2S_TX_TDM_CHAN14_EN_S)
#define I2S_TX_TDM_CHAN14_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN14_EN_S  14
/** I2S_TX_TDM_CHAN15_EN : R/W; bitpos: [15]; default: 1;
 *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
 *  0 in this channel.
 */
#define I2S_TX_TDM_CHAN15_EN    (BIT(15))
#define I2S_TX_TDM_CHAN15_EN_M  (I2S_TX_TDM_CHAN15_EN_V << I2S_TX_TDM_CHAN15_EN_S)
#define I2S_TX_TDM_CHAN15_EN_V  0x00000001U
#define I2S_TX_TDM_CHAN15_EN_S  15
/** I2S_TX_TDM_TOT_CHAN_NUM : R/W; bitpos: [19:16]; default: 0;
 *  The total channel number of I2S TX TDM mode.
 */
#define I2S_TX_TDM_TOT_CHAN_NUM    0x0000000FU
#define I2S_TX_TDM_TOT_CHAN_NUM_M  (I2S_TX_TDM_TOT_CHAN_NUM_V << I2S_TX_TDM_TOT_CHAN_NUM_S)
#define I2S_TX_TDM_TOT_CHAN_NUM_V  0x0000000FU
#define I2S_TX_TDM_TOT_CHAN_NUM_S  16
/** I2S_TX_TDM_SKIP_MSK_EN : R/W; bitpos: [20]; default: 0;
 *  When DMA TX buffer stores the data of (REG_TX_TDM_TOT_CHAN_NUM + 1)  channels, and
 *  only the data of the enabled channels is sent, then this bit should be set. Clear
 *  it when all the data stored in DMA TX buffer is for enabled channels.
 */
#define I2S_TX_TDM_SKIP_MSK_EN    (BIT(20))
#define I2S_TX_TDM_SKIP_MSK_EN_M  (I2S_TX_TDM_SKIP_MSK_EN_V << I2S_TX_TDM_SKIP_MSK_EN_S)
#define I2S_TX_TDM_SKIP_MSK_EN_V  0x00000001U
#define I2S_TX_TDM_SKIP_MSK_EN_S  20

/** I2S_RX_TIMING_REG register
 *  I2S RX timing control register
 */
#define I2S_RX_TIMING_REG (DR_REG_I2S_BASE + 0x58)
/** I2S_RX_SD_IN_DM : R/W; bitpos: [1:0]; default: 0;
 *  The delay mode of I2S Rx SD input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_SD_IN_DM    0x00000003U
#define I2S_RX_SD_IN_DM_M  (I2S_RX_SD_IN_DM_V << I2S_RX_SD_IN_DM_S)
#define I2S_RX_SD_IN_DM_V  0x00000003U
#define I2S_RX_SD_IN_DM_S  0
/** I2S_RX_SD1_IN_DM : R/W; bitpos: [5:4]; default: 0;
 *  The delay mode of I2S Rx SD1 input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_SD1_IN_DM    0x00000003U
#define I2S_RX_SD1_IN_DM_M  (I2S_RX_SD1_IN_DM_V << I2S_RX_SD1_IN_DM_S)
#define I2S_RX_SD1_IN_DM_V  0x00000003U
#define I2S_RX_SD1_IN_DM_S  4
/** I2S_RX_SD2_IN_DM : R/W; bitpos: [9:8]; default: 0;
 *  The delay mode of I2S Rx SD2 input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_SD2_IN_DM    0x00000003U
#define I2S_RX_SD2_IN_DM_M  (I2S_RX_SD2_IN_DM_V << I2S_RX_SD2_IN_DM_S)
#define I2S_RX_SD2_IN_DM_V  0x00000003U
#define I2S_RX_SD2_IN_DM_S  8
/** I2S_RX_SD3_IN_DM : R/W; bitpos: [13:12]; default: 0;
 *  The delay mode of I2S Rx SD3 input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_SD3_IN_DM    0x00000003U
#define I2S_RX_SD3_IN_DM_M  (I2S_RX_SD3_IN_DM_V << I2S_RX_SD3_IN_DM_S)
#define I2S_RX_SD3_IN_DM_V  0x00000003U
#define I2S_RX_SD3_IN_DM_S  12
/** I2S_RX_WS_OUT_DM : R/W; bitpos: [17:16]; default: 0;
 *  The delay mode of I2S Rx WS output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_WS_OUT_DM    0x00000003U
#define I2S_RX_WS_OUT_DM_M  (I2S_RX_WS_OUT_DM_V << I2S_RX_WS_OUT_DM_S)
#define I2S_RX_WS_OUT_DM_V  0x00000003U
#define I2S_RX_WS_OUT_DM_S  16
/** I2S_RX_BCK_OUT_DM : R/W; bitpos: [21:20]; default: 0;
 *  The delay mode of I2S Rx BCK output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_BCK_OUT_DM    0x00000003U
#define I2S_RX_BCK_OUT_DM_M  (I2S_RX_BCK_OUT_DM_V << I2S_RX_BCK_OUT_DM_S)
#define I2S_RX_BCK_OUT_DM_V  0x00000003U
#define I2S_RX_BCK_OUT_DM_S  20
/** I2S_RX_WS_IN_DM : R/W; bitpos: [25:24]; default: 0;
 *  The delay mode of I2S Rx WS input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_WS_IN_DM    0x00000003U
#define I2S_RX_WS_IN_DM_M  (I2S_RX_WS_IN_DM_V << I2S_RX_WS_IN_DM_S)
#define I2S_RX_WS_IN_DM_V  0x00000003U
#define I2S_RX_WS_IN_DM_S  24
/** I2S_RX_BCK_IN_DM : R/W; bitpos: [29:28]; default: 0;
 *  The delay mode of I2S Rx BCK input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_RX_BCK_IN_DM    0x00000003U
#define I2S_RX_BCK_IN_DM_M  (I2S_RX_BCK_IN_DM_V << I2S_RX_BCK_IN_DM_S)
#define I2S_RX_BCK_IN_DM_V  0x00000003U
#define I2S_RX_BCK_IN_DM_S  28

/** I2S_TX_TIMING_REG register
 *  I2S TX timing control register
 */
#define I2S_TX_TIMING_REG (DR_REG_I2S_BASE + 0x5c)
/** I2S_TX_SD_OUT_DM : R/W; bitpos: [1:0]; default: 0;
 *  The delay mode of I2S TX SD output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_TX_SD_OUT_DM    0x00000003U
#define I2S_TX_SD_OUT_DM_M  (I2S_TX_SD_OUT_DM_V << I2S_TX_SD_OUT_DM_S)
#define I2S_TX_SD_OUT_DM_V  0x00000003U
#define I2S_TX_SD_OUT_DM_S  0
/** I2S_TX_SD1_OUT_DM : R/W; bitpos: [5:4]; default: 0;
 *  The delay mode of I2S TX SD1 output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_TX_SD1_OUT_DM    0x00000003U
#define I2S_TX_SD1_OUT_DM_M  (I2S_TX_SD1_OUT_DM_V << I2S_TX_SD1_OUT_DM_S)
#define I2S_TX_SD1_OUT_DM_V  0x00000003U
#define I2S_TX_SD1_OUT_DM_S  4
/** I2S_TX_WS_OUT_DM : R/W; bitpos: [17:16]; default: 0;
 *  The delay mode of I2S TX WS output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_TX_WS_OUT_DM    0x00000003U
#define I2S_TX_WS_OUT_DM_M  (I2S_TX_WS_OUT_DM_V << I2S_TX_WS_OUT_DM_S)
#define I2S_TX_WS_OUT_DM_V  0x00000003U
#define I2S_TX_WS_OUT_DM_S  16
/** I2S_TX_BCK_OUT_DM : R/W; bitpos: [21:20]; default: 0;
 *  The delay mode of I2S TX BCK output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_TX_BCK_OUT_DM    0x00000003U
#define I2S_TX_BCK_OUT_DM_M  (I2S_TX_BCK_OUT_DM_V << I2S_TX_BCK_OUT_DM_S)
#define I2S_TX_BCK_OUT_DM_V  0x00000003U
#define I2S_TX_BCK_OUT_DM_S  20
/** I2S_TX_WS_IN_DM : R/W; bitpos: [25:24]; default: 0;
 *  The delay mode of I2S TX WS input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_TX_WS_IN_DM    0x00000003U
#define I2S_TX_WS_IN_DM_M  (I2S_TX_WS_IN_DM_V << I2S_TX_WS_IN_DM_S)
#define I2S_TX_WS_IN_DM_V  0x00000003U
#define I2S_TX_WS_IN_DM_S  24
/** I2S_TX_BCK_IN_DM : R/W; bitpos: [29:28]; default: 0;
 *  The delay mode of I2S TX BCK input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define I2S_TX_BCK_IN_DM    0x00000003U
#define I2S_TX_BCK_IN_DM_M  (I2S_TX_BCK_IN_DM_V << I2S_TX_BCK_IN_DM_S)
#define I2S_TX_BCK_IN_DM_V  0x00000003U
#define I2S_TX_BCK_IN_DM_S  28

/** I2S_LC_HUNG_CONF_REG register
 *  I2S HUNG configure register.
 */
#define I2S_LC_HUNG_CONF_REG (DR_REG_I2S_BASE + 0x60)
/** I2S_LC_FIFO_TIMEOUT : R/W; bitpos: [7:0]; default: 16;
 *  the i2s_tx_hung_int interrupt or the i2s_rx_hung_int interrupt will be triggered
 *  when fifo hung counter is equal to this value
 */
#define I2S_LC_FIFO_TIMEOUT    0x000000FFU
#define I2S_LC_FIFO_TIMEOUT_M  (I2S_LC_FIFO_TIMEOUT_V << I2S_LC_FIFO_TIMEOUT_S)
#define I2S_LC_FIFO_TIMEOUT_V  0x000000FFU
#define I2S_LC_FIFO_TIMEOUT_S  0
/** I2S_LC_FIFO_TIMEOUT_SHIFT : R/W; bitpos: [10:8]; default: 0;
 *  The bits are used to scale tick counter threshold. The tick counter is reset when
 *  counter value >= 88000/2^i2s_lc_fifo_timeout_shift
 */
#define I2S_LC_FIFO_TIMEOUT_SHIFT    0x00000007U
#define I2S_LC_FIFO_TIMEOUT_SHIFT_M  (I2S_LC_FIFO_TIMEOUT_SHIFT_V << I2S_LC_FIFO_TIMEOUT_SHIFT_S)
#define I2S_LC_FIFO_TIMEOUT_SHIFT_V  0x00000007U
#define I2S_LC_FIFO_TIMEOUT_SHIFT_S  8
/** I2S_LC_FIFO_TIMEOUT_ENA : R/W; bitpos: [11]; default: 1;
 *  The enable bit for FIFO timeout
 */
#define I2S_LC_FIFO_TIMEOUT_ENA    (BIT(11))
#define I2S_LC_FIFO_TIMEOUT_ENA_M  (I2S_LC_FIFO_TIMEOUT_ENA_V << I2S_LC_FIFO_TIMEOUT_ENA_S)
#define I2S_LC_FIFO_TIMEOUT_ENA_V  0x00000001U
#define I2S_LC_FIFO_TIMEOUT_ENA_S  11

/** I2S_RXEOF_NUM_REG register
 *  I2S RX data number control register.
 */
#define I2S_RXEOF_NUM_REG (DR_REG_I2S_BASE + 0x64)
/** I2S_RX_EOF_NUM : R/W; bitpos: [11:0]; default: 64;
 *  The receive data bit length is (I2S_RX_BITS_MOD[4:0] + 1) * (REG_RX_EOF_NUM[11:0] +
 *  1) . It will trigger in_suc_eof interrupt in the configured DMA RX channel.
 */
#define I2S_RX_EOF_NUM    0x00000FFFU
#define I2S_RX_EOF_NUM_M  (I2S_RX_EOF_NUM_V << I2S_RX_EOF_NUM_S)
#define I2S_RX_EOF_NUM_V  0x00000FFFU
#define I2S_RX_EOF_NUM_S  0

/** I2S_CONF_SIGLE_DATA_REG register
 *  I2S signal data register
 */
#define I2S_CONF_SIGLE_DATA_REG (DR_REG_I2S_BASE + 0x68)
/** I2S_SINGLE_DATA : R/W; bitpos: [31:0]; default: 0;
 *  The configured constant channel data to be sent out.
 */
#define I2S_SINGLE_DATA    0xFFFFFFFFU
#define I2S_SINGLE_DATA_M  (I2S_SINGLE_DATA_V << I2S_SINGLE_DATA_S)
#define I2S_SINGLE_DATA_V  0xFFFFFFFFU
#define I2S_SINGLE_DATA_S  0

/** I2S_STATE_REG register
 *  I2S TX status register
 */
#define I2S_STATE_REG (DR_REG_I2S_BASE + 0x6c)
/** I2S_TX_IDLE : RO; bitpos: [0]; default: 1;
 *  1: i2s_tx is idle state. 0: i2s_tx is working.
 */
#define I2S_TX_IDLE    (BIT(0))
#define I2S_TX_IDLE_M  (I2S_TX_IDLE_V << I2S_TX_IDLE_S)
#define I2S_TX_IDLE_V  0x00000001U
#define I2S_TX_IDLE_S  0

/** I2S_ETM_CONF_REG register
 *  I2S ETM configure register
 */
#define I2S_ETM_CONF_REG (DR_REG_I2S_BASE + 0x70)
/** I2S_ETM_TX_SEND_WORD_NUM : R/W; bitpos: [9:0]; default: 64;
 *  I2S ETM send x words event. When sending word number of
 *  reg_etm_tx_send_word_num[9:0], i2s will trigger an etm event.
 */
#define I2S_ETM_TX_SEND_WORD_NUM    0x000003FFU
#define I2S_ETM_TX_SEND_WORD_NUM_M  (I2S_ETM_TX_SEND_WORD_NUM_V << I2S_ETM_TX_SEND_WORD_NUM_S)
#define I2S_ETM_TX_SEND_WORD_NUM_V  0x000003FFU
#define I2S_ETM_TX_SEND_WORD_NUM_S  0
/** I2S_ETM_RX_RECEIVE_WORD_NUM : R/W; bitpos: [19:10]; default: 64;
 *  I2S ETM receive x words event. When receiving word number of
 *  reg_etm_rx_receive_word_num[9:0], i2s will trigger an etm event.
 */
#define I2S_ETM_RX_RECEIVE_WORD_NUM    0x000003FFU
#define I2S_ETM_RX_RECEIVE_WORD_NUM_M  (I2S_ETM_RX_RECEIVE_WORD_NUM_V << I2S_ETM_RX_RECEIVE_WORD_NUM_S)
#define I2S_ETM_RX_RECEIVE_WORD_NUM_V  0x000003FFU
#define I2S_ETM_RX_RECEIVE_WORD_NUM_S  10

/** I2S_FIFO_CNT_REG register
 *  I2S sync counter register
 */
#define I2S_FIFO_CNT_REG (DR_REG_I2S_BASE + 0x74)
/** I2S_TX_FIFO_CNT : RO; bitpos: [30:0]; default: 0;
 *  tx fifo counter value.
 */
#define I2S_TX_FIFO_CNT    0x7FFFFFFFU
#define I2S_TX_FIFO_CNT_M  (I2S_TX_FIFO_CNT_V << I2S_TX_FIFO_CNT_S)
#define I2S_TX_FIFO_CNT_V  0x7FFFFFFFU
#define I2S_TX_FIFO_CNT_S  0
/** I2S_TX_FIFO_CNT_RST : WT; bitpos: [31]; default: 0;
 *  Set this bit to reset tx fifo counter.
 */
#define I2S_TX_FIFO_CNT_RST    (BIT(31))
#define I2S_TX_FIFO_CNT_RST_M  (I2S_TX_FIFO_CNT_RST_V << I2S_TX_FIFO_CNT_RST_S)
#define I2S_TX_FIFO_CNT_RST_V  0x00000001U
#define I2S_TX_FIFO_CNT_RST_S  31

/** I2S_BCK_CNT_REG register
 *  I2S sync counter register
 */
#define I2S_BCK_CNT_REG (DR_REG_I2S_BASE + 0x78)
/** I2S_TX_BCK_CNT : RO; bitpos: [30:0]; default: 0;
 *  tx bck counter value.
 */
#define I2S_TX_BCK_CNT    0x7FFFFFFFU
#define I2S_TX_BCK_CNT_M  (I2S_TX_BCK_CNT_V << I2S_TX_BCK_CNT_S)
#define I2S_TX_BCK_CNT_V  0x7FFFFFFFU
#define I2S_TX_BCK_CNT_S  0
/** I2S_TX_BCK_CNT_RST : WT; bitpos: [31]; default: 0;
 *  Set this bit to reset tx bck counter.
 */
#define I2S_TX_BCK_CNT_RST    (BIT(31))
#define I2S_TX_BCK_CNT_RST_M  (I2S_TX_BCK_CNT_RST_V << I2S_TX_BCK_CNT_RST_S)
#define I2S_TX_BCK_CNT_RST_V  0x00000001U
#define I2S_TX_BCK_CNT_RST_S  31

/** I2S_CLK_GATE_REG register
 *  Clock gate register
 */
#define I2S_CLK_GATE_REG (DR_REG_I2S_BASE + 0x7c)
/** I2S_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  set this bit to enable clock gate
 */
#define I2S_CLK_EN    (BIT(0))
#define I2S_CLK_EN_M  (I2S_CLK_EN_V << I2S_CLK_EN_S)
#define I2S_CLK_EN_V  0x00000001U
#define I2S_CLK_EN_S  0

/** I2S_DATE_REG register
 *  Version control register
 */
#define I2S_DATE_REG (DR_REG_I2S_BASE + 0x80)
/** I2S_DATE : R/W; bitpos: [27:0]; default: 36713024;
 *  I2S version control register
 */
#define I2S_DATE    0x0FFFFFFFU
#define I2S_DATE_M  (I2S_DATE_V << I2S_DATE_S)
#define I2S_DATE_V  0x0FFFFFFFU
#define I2S_DATE_S  0

#ifdef __cplusplus
}
#endif
