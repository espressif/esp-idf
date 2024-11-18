/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt registers */
/** Type of int_raw register
 *  I2S interrupt raw register, valid in level.
 */
typedef union {
    struct {
        /** rx_done_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status bit  for the i2s_rx_done_int interrupt
         */
        uint32_t rx_done_int_raw:1;
        /** tx_done_int_raw : RO/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status bit  for the i2s_tx_done_int interrupt
         */
        uint32_t tx_done_int_raw:1;
        /** rx_hung_int_raw : RO/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status bit  for the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_raw:1;
        /** tx_hung_int_raw : RO/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status bit  for the i2s_tx_hung_int interrupt
         */
        uint32_t tx_hung_int_raw:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} i2s_int_raw_reg_t;

/** Type of int_st register
 *  I2S interrupt status register.
 */
typedef union {
    struct {
        /** rx_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit  for the i2s_rx_done_int interrupt
         */
        uint32_t rx_done_int_st:1;
        /** tx_done_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit  for the i2s_tx_done_int interrupt
         */
        uint32_t tx_done_int_st:1;
        /** rx_hung_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit  for the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_st:1;
        /** tx_hung_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status bit  for the i2s_tx_hung_int interrupt
         */
        uint32_t tx_hung_int_st:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} i2s_int_st_reg_t;

/** Type of int_ena register
 *  I2S interrupt enable register.
 */
typedef union {
    struct {
        /** rx_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit  for the i2s_rx_done_int interrupt
         */
        uint32_t rx_done_int_ena:1;
        /** tx_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit  for the i2s_tx_done_int interrupt
         */
        uint32_t tx_done_int_ena:1;
        /** rx_hung_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit  for the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_ena:1;
        /** tx_hung_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit  for the i2s_tx_hung_int interrupt
         */
        uint32_t tx_hung_int_ena:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} i2s_int_ena_reg_t;

/** Type of int_clr register
 *  I2S interrupt clear register.
 */
typedef union {
    struct {
        /** rx_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the i2s_rx_done_int interrupt
         */
        uint32_t rx_done_int_clr:1;
        /** tx_done_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the i2s_tx_done_int interrupt
         */
        uint32_t tx_done_int_clr:1;
        /** rx_hung_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_clr:1;
        /** tx_hung_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the i2s_tx_hung_int interrupt
         */
        uint32_t tx_hung_int_clr:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} i2s_int_clr_reg_t;


/** Group: RX Control and configuration registers */
/** Type of rx_conf register
 *  I2S RX configure register
 */
typedef union {
    struct {
        /** rx_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset receiver
         */
        uint32_t rx_reset:1;
        /** rx_fifo_reset : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset Rx AFIFO
         */
        uint32_t rx_fifo_reset:1;
        /** rx_start : R/W/SC; bitpos: [2]; default: 0;
         *  Set this bit to start receiving data
         */
        uint32_t rx_start:1;
        /** rx_slave_mod : R/W; bitpos: [3]; default: 0;
         *  Set this bit to enable slave receiver mode
         */
        uint32_t rx_slave_mod:1;
        /** rx_stop_mode : R/W; bitpos: [5:4]; default: 0;
         *  0  : I2S Rx only stop when reg_rx_start is cleared.   1: Stop when reg_rx_start is
         *  0 or in_suc_eof is 1.   2:  Stop I2S RX when reg_rx_start is 0 or RX FIFO is full.
         */
        uint32_t rx_stop_mode:2;
        /** rx_mono : R/W; bitpos: [6]; default: 0;
         *  Set this bit to enable receiver  in mono mode
         */
        uint32_t rx_mono:1;
        /** rx_big_endian : R/W; bitpos: [7]; default: 0;
         *  I2S Rx byte endian, 1: low addr value to high addr. 0: low addr with low addr value.
         */
        uint32_t rx_big_endian:1;
        /** rx_update : R/W/SC; bitpos: [8]; default: 0;
         *  Set 1 to update I2S RX registers from APB clock domain to I2S RX clock domain. This
         *  bit will be cleared by hardware after update register done.
         */
        uint32_t rx_update:1;
        /** rx_mono_fst_vld : R/W; bitpos: [9]; default: 1;
         *  1: The first channel data value is valid in I2S RX mono mode.   0: The second
         *  channel data value is valid in I2S RX mono mode.
         */
        uint32_t rx_mono_fst_vld:1;
        /** rx_pcm_conf : R/W; bitpos: [11:10]; default: 1;
         *  I2S RX compress/decompress configuration bit. & 0 (atol): A-Law decompress, 1
         *  (ltoa) : A-Law compress, 2 (utol) : u-Law decompress, 3 (ltou) : u-Law compress. &
         */
        uint32_t rx_pcm_conf:2;
        /** rx_pcm_bypass : R/W; bitpos: [12]; default: 1;
         *  Set this bit to bypass Compress/Decompress module for received data.
         */
        uint32_t rx_pcm_bypass:1;
        /** rx_msb_shift : R/W; bitpos: [13]; default: 1;
         *  Set this bit to enable receiver in Phillips standard mode
         */
        uint32_t rx_msb_shift:1;
        uint32_t reserved_14:1;
        /** rx_left_align : R/W; bitpos: [15]; default: 1;
         *  1: I2S RX left alignment mode. 0: I2S RX right alignment mode.
         */
        uint32_t rx_left_align:1;
        /** rx_24_fill_en : R/W; bitpos: [16]; default: 0;
         *  1: store 24 channel bits to 32 bits. 0:store 24 channel bits to 24 bits.
         */
        uint32_t rx_24_fill_en:1;
        /** rx_ws_idle_pol : R/W; bitpos: [17]; default: 0;
         *  0: WS should be 0 when receiving left channel data, and WS is 1in right channel.
         *  1: WS should be 1 when receiving left channel data, and WS is 0in right channel.
         */
        uint32_t rx_ws_idle_pol:1;
        /** rx_bit_order : R/W; bitpos: [18]; default: 0;
         *  I2S Rx bit endian. 1:small endian, the LSB is received first. 0:big endian, the MSB
         *  is received first.
         */
        uint32_t rx_bit_order:1;
        /** rx_tdm_en : R/W; bitpos: [19]; default: 0;
         *  1: Enable I2S TDM Rx mode . 0: Disable.
         */
        uint32_t rx_tdm_en:1;
        /** rx_pdm_en : R/W; bitpos: [20]; default: 0;
         *  1: Enable I2S PDM Rx mode . 0: Disable.
         */
        uint32_t rx_pdm_en:1;
        /** rx_bck_div_num : R/W; bitpos: [26:21]; default: 6;
         *  Bit clock configuration bits in receiver mode.
         */
        uint32_t rx_bck_div_num:6;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} i2s_rx_conf_reg_t;

/** Type of rx_conf1 register
 *  I2S RX configure register 1
 */
typedef union {
    struct {
        /** rx_tdm_ws_width : R/W; bitpos: [8:0]; default: 0;
         *  The width of rx_ws_out at idle level in TDM mode is (I2S_RX_TDM_WS_WIDTH[8:0] +1) *
         *  T_bck
         */
        uint32_t rx_tdm_ws_width:9;
        uint32_t reserved_9:5;
        /** rx_bits_mod : R/W; bitpos: [18:14]; default: 15;
         *  Set the bits to configure the valid data bit length of I2S receiver channel. 7: all
         *  the valid channel data is in 8-bit-mode. 15: all the valid channel data is in
         *  16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid
         *  channel data is in 32-bit-mode.
         */
        uint32_t rx_bits_mod:5;
        /** rx_half_sample_bits : R/W; bitpos: [26:19]; default: 15;
         *  I2S Rx half sample bits -1.
         */
        uint32_t rx_half_sample_bits:8;
        /** rx_tdm_chan_bits : R/W; bitpos: [31:27]; default: 15;
         *  The Rx bit number for each channel minus 1in TDM mode.
         */
        uint32_t rx_tdm_chan_bits:5;
    };
    uint32_t val;
} i2s_rx_conf1_reg_t;

/** Type of rx_pdm2pcm_conf register
 *  I2S RX configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:19;
        /** rx_pdm2pcm_en : R/W; bitpos: [19]; default: 0;
         *  1: Enable PDM2PCM RX mode. 0: DIsable.
         */
        uint32_t rx_pdm2pcm_en:1;
        /** rx_pdm_sinc_dsr_16_en : R/W; bitpos: [20]; default: 0;
         *  Configure the down sampling rate of PDM RX filter group1 module. 1: The  down
         *  sampling rate is 128. 0: down sampling rate is 64.
         */
        uint32_t rx_pdm_sinc_dsr_16_en:1;
        /** rx_pdm2pcm_amplify_num : R/W; bitpos: [24:21]; default: 1;
         *  Configure PDM RX amplify number.
         */
        uint32_t rx_pdm2pcm_amplify_num:4;
        /** rx_pdm_hp_bypass : R/W; bitpos: [25]; default: 0;
         *  I2S PDM RX bypass hp filter or not.
         */
        uint32_t rx_pdm_hp_bypass:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} i2s_rx_pdm2pcm_conf_reg_t;

/** Type of rx_tdm_ctrl register
 *  I2S TX TDM mode control register
 */
typedef union {
    struct {
        /** rx_tdm_pdm_chan0_en : R/W; bitpos: [0]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan0_en:1;
        /** rx_tdm_pdm_chan1_en : R/W; bitpos: [1]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan1_en:1;
        /** rx_tdm_pdm_chan2_en : R/W; bitpos: [2]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan2_en:1;
        /** rx_tdm_pdm_chan3_en : R/W; bitpos: [3]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan3_en:1;
        /** rx_tdm_pdm_chan4_en : R/W; bitpos: [4]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan4_en:1;
        /** rx_tdm_pdm_chan5_en : R/W; bitpos: [5]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan5_en:1;
        /** rx_tdm_pdm_chan6_en : R/W; bitpos: [6]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan6_en:1;
        /** rx_tdm_pdm_chan7_en : R/W; bitpos: [7]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
         *  input 0 in this channel.
         */
        uint32_t rx_tdm_pdm_chan7_en:1;
        /** rx_tdm_chan8_en : R/W; bitpos: [8]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan8_en:1;
        /** rx_tdm_chan9_en : R/W; bitpos: [9]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan9_en:1;
        /** rx_tdm_chan10_en : R/W; bitpos: [10]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan10_en:1;
        /** rx_tdm_chan11_en : R/W; bitpos: [11]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan11_en:1;
        /** rx_tdm_chan12_en : R/W; bitpos: [12]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan12_en:1;
        /** rx_tdm_chan13_en : R/W; bitpos: [13]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan13_en:1;
        /** rx_tdm_chan14_en : R/W; bitpos: [14]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan14_en:1;
        /** rx_tdm_chan15_en : R/W; bitpos: [15]; default: 1;
         *  1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0
         *  in this channel.
         */
        uint32_t rx_tdm_chan15_en:1;
        /** rx_tdm_tot_chan_num : R/W; bitpos: [19:16]; default: 0;
         *  The total channel number of I2S TX TDM mode.
         */
        uint32_t rx_tdm_tot_chan_num:4;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} i2s_rx_tdm_ctrl_reg_t;

/** Type of rxeof_num register
 *  I2S RX data number control register.
 */
typedef union {
    struct {
        /** rx_eof_num : R/W; bitpos: [11:0]; default: 64;
         *  The receive data bit length is (I2S_RX_BITS_MOD[4:0] + 1) * (REG_RX_EOF_NUM[11:0] +
         *  1) . It will trigger in_suc_eof interrupt in the configured DMA RX channel.
         */
        uint32_t rx_eof_num:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} i2s_rxeof_num_reg_t;


/** Group: TX Control and configuration registers */
/** Type of tx_conf register
 *  I2S TX configure register
 */
typedef union {
    struct {
        /** tx_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset transmitter
         */
        uint32_t tx_reset:1;
        /** tx_fifo_reset : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset Tx AFIFO
         */
        uint32_t tx_fifo_reset:1;
        /** tx_start : R/W/SC; bitpos: [2]; default: 0;
         *  Set this bit to start transmitting data
         */
        uint32_t tx_start:1;
        /** tx_slave_mod : R/W; bitpos: [3]; default: 0;
         *  Set this bit to enable slave transmitter mode
         */
        uint32_t tx_slave_mod:1;
        /** tx_stop_en : R/W; bitpos: [4]; default: 1;
         *  Set this bit to stop disable output BCK signal and WS signal when tx FIFO is empty
         */
        uint32_t tx_stop_en:1;
        /** tx_chan_equal : R/W; bitpos: [5]; default: 0;
         *  1: The value of Left channel data is equal to the value of right channel data in
         *  I2S TX mono mode or TDM channel select mode. 0: The invalid channel data is
         *  reg_i2s_single_data in I2S TX mono mode or TDM channel select mode.
         */
        uint32_t tx_chan_equal:1;
        /** tx_mono : R/W; bitpos: [6]; default: 0;
         *  Set this bit to enable transmitter in mono mode
         */
        uint32_t tx_mono:1;
        /** tx_big_endian : R/W; bitpos: [7]; default: 0;
         *  I2S Tx byte endian, 1: low addr value to high addr.  0: low addr with low addr
         *  value.
         */
        uint32_t tx_big_endian:1;
        /** tx_update : R/W/SC; bitpos: [8]; default: 0;
         *  Set 1 to update I2S TX registers from APB clock domain to I2S TX clock domain. This
         *  bit will be cleared by hardware after update register done.
         */
        uint32_t tx_update:1;
        /** tx_mono_fst_vld : R/W; bitpos: [9]; default: 1;
         *  1: The first channel data value is valid in I2S TX mono mode.   0: The second
         *  channel data value is valid in I2S TX mono mode.
         */
        uint32_t tx_mono_fst_vld:1;
        /** tx_pcm_conf : R/W; bitpos: [11:10]; default: 0;
         *  I2S TX compress/decompress configuration bit. & 0 (atol): A-Law decompress, 1
         *  (ltoa) : A-Law compress, 2 (utol) : u-Law decompress, 3 (ltou) : u-Law compress. &
         */
        uint32_t tx_pcm_conf:2;
        /** tx_pcm_bypass : R/W; bitpos: [12]; default: 1;
         *  Set this bit to bypass  Compress/Decompress module for transmitted data.
         */
        uint32_t tx_pcm_bypass:1;
        /** tx_msb_shift : R/W; bitpos: [13]; default: 1;
         *  Set this bit to enable transmitter in Phillips standard mode
         */
        uint32_t tx_msb_shift:1;
        /** tx_bck_no_dly : R/W; bitpos: [14]; default: 1;
         *  1: BCK is not delayed to generate pos/neg edge in master mode. 0: BCK is delayed to
         *  generate pos/neg edge in master mode.
         */
        uint32_t tx_bck_no_dly:1;
        /** tx_left_align : R/W; bitpos: [15]; default: 1;
         *  1: I2S TX left alignment mode. 0: I2S TX right alignment mode.
         */
        uint32_t tx_left_align:1;
        /** tx_24_fill_en : R/W; bitpos: [16]; default: 0;
         *  1: Sent 32 bits in 24 channel bits mode. 0: Sent 24 bits in 24 channel bits mode
         */
        uint32_t tx_24_fill_en:1;
        /** tx_ws_idle_pol : R/W; bitpos: [17]; default: 0;
         *  0: WS should be 0 when sending left channel data, and WS is 1in right channel.  1:
         *  WS should be 1 when sending left channel data, and WS is 0in right channel.
         */
        uint32_t tx_ws_idle_pol:1;
        /** tx_bit_order : R/W; bitpos: [18]; default: 0;
         *  I2S Tx bit endian. 1:small endian, the LSB is sent first. 0:big endian, the MSB is
         *  sent first.
         */
        uint32_t tx_bit_order:1;
        /** tx_tdm_en : R/W; bitpos: [19]; default: 0;
         *  1: Enable I2S TDM Tx mode . 0: Disable.
         */
        uint32_t tx_tdm_en:1;
        /** tx_pdm_en : R/W; bitpos: [20]; default: 0;
         *  1: Enable I2S PDM Tx mode . 0: Disable.
         */
        uint32_t tx_pdm_en:1;
        /** tx_bck_div_num : R/W; bitpos: [26:21]; default: 6;
         *  Bit clock configuration bits in transmitter mode.
         */
        uint32_t tx_bck_div_num:6;
        /** tx_chan_mod : R/W; bitpos: [29:27]; default: 0;
         *  I2S transmitter channel mode configuration bits.
         */
        uint32_t tx_chan_mod:3;
        /** sig_loopback : R/W; bitpos: [30]; default: 0;
         *  Enable signal loop back mode with transmitter module and receiver module sharing
         *  the same WS and BCK signals.
         */
        uint32_t sig_loopback:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} i2s_tx_conf_reg_t;

/** Type of tx_conf1 register
 *  I2S TX configure register 1
 */
typedef union {
    struct {
        /** tx_tdm_ws_width : R/W; bitpos: [8:0]; default: 0;
         *  The width of tx_ws_out at idle level in TDM mode is (I2S_TX_TDM_WS_WIDTH[8:0] +1) *
         *  T_bck
         */
        uint32_t tx_tdm_ws_width:9;
        uint32_t reserved_9:5;
        /** tx_bits_mod : R/W; bitpos: [18:14]; default: 15;
         *  Set the bits to configure the valid data bit length of I2S transmitter channel. 7:
         *  all the valid channel data is in 8-bit-mode. 15: all the valid channel data is in
         *  16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid
         *  channel data is in 32-bit-mode.
         */
        uint32_t tx_bits_mod:5;
        /** tx_half_sample_bits : R/W; bitpos: [26:19]; default: 15;
         *  I2S Tx half sample bits -1.
         */
        uint32_t tx_half_sample_bits:8;
        /** tx_tdm_chan_bits : R/W; bitpos: [31:27]; default: 15;
         *  The Tx bit number for each channel minus 1in TDM mode.
         */
        uint32_t tx_tdm_chan_bits:5;
    };
    uint32_t val;
} i2s_tx_conf1_reg_t;

/** Type of tx_pcm2pdm_conf register
 *  I2S TX PCM2PDM configuration register
 */
typedef union {
    struct {
        /** tx_pdm_hp_bypass : R/W; bitpos: [0]; default: 0;
         *  I2S TX PDM bypass hp filter or not. The option has been removed.
         */
        uint32_t tx_pdm_hp_bypass:1;
        /** tx_pdm_sinc_osr2 : R/W; bitpos: [4:1]; default: 2;
         *  I2S TX PDM OSR2 value
         */
        uint32_t tx_pdm_sinc_osr2:4;
        /** tx_pdm_prescale : R/W; bitpos: [12:5]; default: 0;
         *  I2S TX PDM prescale for sigmadelta
         */
        uint32_t tx_pdm_prescale:8;
        /** tx_pdm_hp_in_shift : R/W; bitpos: [14:13]; default: 1;
         *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
         */
        uint32_t tx_pdm_hp_in_shift:2;
        /** tx_pdm_lp_in_shift : R/W; bitpos: [16:15]; default: 1;
         *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
         */
        uint32_t tx_pdm_lp_in_shift:2;
        /** tx_pdm_sinc_in_shift : R/W; bitpos: [18:17]; default: 1;
         *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
         */
        uint32_t tx_pdm_sinc_in_shift:2;
        /** tx_pdm_sigmadelta_in_shift : R/W; bitpos: [20:19]; default: 1;
         *  I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4
         */
        uint32_t tx_pdm_sigmadelta_in_shift:2;
        /** tx_pdm_sigmadelta_dither2 : R/W; bitpos: [21]; default: 0;
         *  I2S TX PDM sigmadelta dither2 value
         */
        uint32_t tx_pdm_sigmadelta_dither2:1;
        /** tx_pdm_sigmadelta_dither : R/W; bitpos: [22]; default: 1;
         *  I2S TX PDM sigmadelta dither value
         */
        uint32_t tx_pdm_sigmadelta_dither:1;
        /** tx_pdm_dac_2out_en : R/W; bitpos: [23]; default: 0;
         *  I2S TX PDM dac mode enable
         */
        uint32_t tx_pdm_dac_2out_en:1;
        /** tx_pdm_dac_mode_en : R/W; bitpos: [24]; default: 0;
         *  I2S TX PDM dac 2channel enable
         */
        uint32_t tx_pdm_dac_mode_en:1;
        /** pcm2pdm_conv_en : R/W; bitpos: [25]; default: 0;
         *  I2S TX PDM Converter enable
         */
        uint32_t pcm2pdm_conv_en:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} i2s_tx_pcm2pdm_conf_reg_t;

/** Type of tx_pcm2pdm_conf1 register
 *  I2S TX PCM2PDM configuration register
 */
typedef union {
    struct {
        /** tx_pdm_fp : R/W; bitpos: [9:0]; default: 960;
         *  I2S TX PDM Fp
         */
        uint32_t tx_pdm_fp:10;
        /** tx_pdm_fs : R/W; bitpos: [19:10]; default: 480;
         *  I2S TX PDM Fs
         */
        uint32_t tx_pdm_fs:10;
        /** tx_iir_hp_mult12_5 : R/W; bitpos: [22:20]; default: 7;
         *  The fourth parameter of PDM TX IIR_HP filter stage 2 is (504 +
         *  I2S_TX_IIR_HP_MULT12_5[2:0])
         */
        uint32_t tx_iir_hp_mult12_5:3;
        /** tx_iir_hp_mult12_0 : R/W; bitpos: [25:23]; default: 7;
         *  The fourth parameter of PDM TX IIR_HP filter stage 1 is (504 +
         *  I2S_TX_IIR_HP_MULT12_0[2:0])
         */
        uint32_t tx_iir_hp_mult12_0:3;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} i2s_tx_pcm2pdm_conf1_reg_t;

/** Type of tx_tdm_ctrl register
 *  I2S TX TDM mode control register
 */
typedef union {
    struct {
        /** tx_tdm_chan0_en : R/W; bitpos: [0]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan0_en:1;
        /** tx_tdm_chan1_en : R/W; bitpos: [1]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan1_en:1;
        /** tx_tdm_chan2_en : R/W; bitpos: [2]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan2_en:1;
        /** tx_tdm_chan3_en : R/W; bitpos: [3]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan3_en:1;
        /** tx_tdm_chan4_en : R/W; bitpos: [4]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan4_en:1;
        /** tx_tdm_chan5_en : R/W; bitpos: [5]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan5_en:1;
        /** tx_tdm_chan6_en : R/W; bitpos: [6]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan6_en:1;
        /** tx_tdm_chan7_en : R/W; bitpos: [7]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan7_en:1;
        /** tx_tdm_chan8_en : R/W; bitpos: [8]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan8_en:1;
        /** tx_tdm_chan9_en : R/W; bitpos: [9]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan9_en:1;
        /** tx_tdm_chan10_en : R/W; bitpos: [10]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan10_en:1;
        /** tx_tdm_chan11_en : R/W; bitpos: [11]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan11_en:1;
        /** tx_tdm_chan12_en : R/W; bitpos: [12]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan12_en:1;
        /** tx_tdm_chan13_en : R/W; bitpos: [13]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan13_en:1;
        /** tx_tdm_chan14_en : R/W; bitpos: [14]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan14_en:1;
        /** tx_tdm_chan15_en : R/W; bitpos: [15]; default: 1;
         *  1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output
         *  0 in this channel.
         */
        uint32_t tx_tdm_chan15_en:1;
        /** tx_tdm_tot_chan_num : R/W; bitpos: [19:16]; default: 0;
         *  The total channel number of I2S TX TDM mode.
         */
        uint32_t tx_tdm_tot_chan_num:4;
        /** tx_tdm_skip_msk_en : R/W; bitpos: [20]; default: 0;
         *  When DMA TX buffer stores the data of (REG_TX_TDM_TOT_CHAN_NUM + 1)  channels, and
         *  only the data of the enabled channels is sent, then this bit should be set. Clear
         *  it when all the data stored in DMA TX buffer is for enabled channels.
         */
        uint32_t tx_tdm_skip_msk_en:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} i2s_tx_tdm_ctrl_reg_t;


/** Group: RX clock and timing registers */
/** Type of rx_timing register
 *  I2S RX timing control register
 */
typedef union {
    struct {
        /** rx_sd_in_dm : R/W; bitpos: [1:0]; default: 0;
         *  The delay mode of I2S Rx SD input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_sd_in_dm:2;
        uint32_t reserved_2:2;
        /** rx_sd1_in_dm : R/W; bitpos: [5:4]; default: 0;
         *  The delay mode of I2S Rx SD1 input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_sd1_in_dm:2;
        uint32_t reserved_6:2;
        /** rx_sd2_in_dm : R/W; bitpos: [9:8]; default: 0;
         *  The delay mode of I2S Rx SD2 input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_sd2_in_dm:2;
        uint32_t reserved_10:2;
        /** rx_sd3_in_dm : R/W; bitpos: [13:12]; default: 0;
         *  The delay mode of I2S Rx SD3 input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_sd3_in_dm:2;
        uint32_t reserved_14:2;
        /** rx_ws_out_dm : R/W; bitpos: [17:16]; default: 0;
         *  The delay mode of I2S Rx WS output signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_ws_out_dm:2;
        uint32_t reserved_18:2;
        /** rx_bck_out_dm : R/W; bitpos: [21:20]; default: 0;
         *  The delay mode of I2S Rx BCK output signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_bck_out_dm:2;
        uint32_t reserved_22:2;
        /** rx_ws_in_dm : R/W; bitpos: [25:24]; default: 0;
         *  The delay mode of I2S Rx WS input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_ws_in_dm:2;
        uint32_t reserved_26:2;
        /** rx_bck_in_dm : R/W; bitpos: [29:28]; default: 0;
         *  The delay mode of I2S Rx BCK input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t rx_bck_in_dm:2;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} i2s_rx_timing_reg_t;


/** Group: TX clock and timing registers */
/** Type of tx_timing register
 *  I2S TX timing control register
 */
typedef union {
    struct {
        /** tx_sd_out_dm : R/W; bitpos: [1:0]; default: 0;
         *  The delay mode of I2S TX SD output signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t tx_sd_out_dm:2;
        uint32_t reserved_2:2;
        /** tx_sd1_out_dm : R/W; bitpos: [5:4]; default: 0;
         *  The delay mode of I2S TX SD1 output signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t tx_sd1_out_dm:2;
        uint32_t reserved_6:10;
        /** tx_ws_out_dm : R/W; bitpos: [17:16]; default: 0;
         *  The delay mode of I2S TX WS output signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t tx_ws_out_dm:2;
        uint32_t reserved_18:2;
        /** tx_bck_out_dm : R/W; bitpos: [21:20]; default: 0;
         *  The delay mode of I2S TX BCK output signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t tx_bck_out_dm:2;
        uint32_t reserved_22:2;
        /** tx_ws_in_dm : R/W; bitpos: [25:24]; default: 0;
         *  The delay mode of I2S TX WS input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t tx_ws_in_dm:2;
        uint32_t reserved_26:2;
        /** tx_bck_in_dm : R/W; bitpos: [29:28]; default: 0;
         *  The delay mode of I2S TX BCK input signal. 0: bypass. 1: delay by pos edge.  2:
         *  delay by neg edge. 3: not used.
         */
        uint32_t tx_bck_in_dm:2;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} i2s_tx_timing_reg_t;


/** Group: Control and configuration registers */
/** Type of lc_hung_conf register
 *  I2S HUNG configure register.
 */
typedef union {
    struct {
        /** lc_fifo_timeout : R/W; bitpos: [7:0]; default: 16;
         *  the i2s_tx_hung_int interrupt or the i2s_rx_hung_int interrupt will be triggered
         *  when fifo hung counter is equal to this value
         */
        uint32_t lc_fifo_timeout:8;
        /** lc_fifo_timeout_shift : R/W; bitpos: [10:8]; default: 0;
         *  The bits are used to scale tick counter threshold. The tick counter is reset when
         *  counter value >= 88000/2^i2s_lc_fifo_timeout_shift
         */
        uint32_t lc_fifo_timeout_shift:3;
        /** lc_fifo_timeout_ena : R/W; bitpos: [11]; default: 1;
         *  The enable bit for FIFO timeout
         */
        uint32_t lc_fifo_timeout_ena:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} i2s_lc_hung_conf_reg_t;

/** Type of conf_sigle_data register
 *  I2S signal data register
 */
typedef union {
    struct {
        /** single_data : R/W; bitpos: [31:0]; default: 0;
         *  The configured constant channel data to be sent out.
         */
        uint32_t single_data:32;
    };
    uint32_t val;
} i2s_conf_single_data_reg_t;


/** Group: TX status registers */
/** Type of state register
 *  I2S TX status register
 */
typedef union {
    struct {
        /** tx_idle : RO; bitpos: [0]; default: 1;
         *  1: i2s_tx is idle state. 0: i2s_tx is working.
         */
        uint32_t tx_idle:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} i2s_state_reg_t;


/** Group: ETM registers */
/** Type of etm_conf register
 *  I2S ETM configure register
 */
typedef union {
    struct {
        /** etm_tx_send_word_num : R/W; bitpos: [9:0]; default: 64;
         *  I2S ETM send x words event. When sending word number of
         *  reg_etm_tx_send_word_num[9:0], i2s will trigger an etm event.
         */
        uint32_t etm_tx_send_word_num:10;
        /** etm_rx_receive_word_num : R/W; bitpos: [19:10]; default: 64;
         *  I2S ETM receive x words event. When receiving word number of
         *  reg_etm_rx_receive_word_num[9:0], i2s will trigger an etm event.
         */
        uint32_t etm_rx_receive_word_num:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} i2s_etm_conf_reg_t;


/** Group: Sync counter registers */
/** Type of fifo_cnt register
 *  I2S sync counter register
 */
typedef union {
    struct {
        /** tx_fifo_cnt : RO; bitpos: [30:0]; default: 0;
         *  tx fifo counter value.
         */
        uint32_t tx_fifo_cnt:31;
        /** tx_fifo_cnt_rst : WT; bitpos: [31]; default: 0;
         *  Set this bit to reset tx fifo counter.
         */
        uint32_t tx_fifo_cnt_rst:1;
    };
    uint32_t val;
} i2s_fifo_cnt_reg_t;

/** Type of bck_cnt register
 *  I2S sync counter register
 */
typedef union {
    struct {
        /** tx_bck_cnt : RO; bitpos: [30:0]; default: 0;
         *  tx bck counter value.
         */
        uint32_t tx_bck_cnt:31;
        /** tx_bck_cnt_rst : WT; bitpos: [31]; default: 0;
         *  Set this bit to reset tx bck counter.
         */
        uint32_t tx_bck_cnt_rst:1;
    };
    uint32_t val;
} i2s_bck_cnt_reg_t;


/** Group: Clock registers */
/** Type of clk_gate register
 *  Clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  set this bit to enable clock gate
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} i2s_clk_gate_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36713024;
         *  I2S version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} i2s_date_reg_t;


typedef struct {
    uint32_t reserved_000[3];
    volatile i2s_int_raw_reg_t int_raw;
    volatile i2s_int_st_reg_t int_st;
    volatile i2s_int_ena_reg_t int_ena;
    volatile i2s_int_clr_reg_t int_clr;
    uint32_t reserved_01c;
    volatile i2s_rx_conf_reg_t rx_conf;
    volatile i2s_tx_conf_reg_t tx_conf;
    volatile i2s_rx_conf1_reg_t rx_conf1;
    volatile i2s_tx_conf1_reg_t tx_conf1;
    uint32_t reserved_030[4];
    volatile i2s_tx_pcm2pdm_conf_reg_t tx_pcm2pdm_conf;
    volatile i2s_tx_pcm2pdm_conf1_reg_t tx_pcm2pdm_conf1;
    volatile i2s_rx_pdm2pcm_conf_reg_t rx_pdm2pcm_conf;
    uint32_t reserved_04c;
    volatile i2s_rx_tdm_ctrl_reg_t rx_tdm_ctrl;
    volatile i2s_tx_tdm_ctrl_reg_t tx_tdm_ctrl;
    volatile i2s_rx_timing_reg_t rx_timing;
    volatile i2s_tx_timing_reg_t tx_timing;
    volatile i2s_lc_hung_conf_reg_t lc_hung_conf;
    volatile i2s_rxeof_num_reg_t rx_eof_num;
    volatile i2s_conf_single_data_reg_t conf_single_data;
    volatile i2s_state_reg_t state;
    volatile i2s_etm_conf_reg_t etm_conf;
    volatile i2s_fifo_cnt_reg_t fifo_cnt;
    volatile i2s_bck_cnt_reg_t bck_cnt;
    volatile i2s_clk_gate_reg_t clk_gate;
    volatile i2s_date_reg_t date;
} i2s_dev_t;

extern i2s_dev_t I2S0;

#ifndef __cplusplus
_Static_assert(sizeof(i2s_dev_t) == 0x84, "Invalid size of i2s_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
