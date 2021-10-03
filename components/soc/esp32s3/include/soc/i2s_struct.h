// Copyright 2017-2021 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_I2S_STRUCT_H_
#define _SOC_I2S_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct i2s_dev_s {
    uint32_t reserved_0;
    uint32_t reserved_4;
    uint32_t reserved_8;
    union {
        struct {
            uint32_t rx_done                       :    1;  /*The raw interrupt status bit  for the i2s_rx_done_int interrupt*/
            uint32_t tx_done                       :    1;  /*The raw interrupt status bit  for the i2s_tx_done_int interrupt*/
            uint32_t rx_hung                       :    1;  /*The raw interrupt status bit  for the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung                       :    1;  /*The raw interrupt status bit  for the i2s_tx_hung_int interrupt*/
            uint32_t reserved4                     :    28;  /*Reserve*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_done                       :    1;  /*The masked interrupt status bit  for the i2s_rx_done_int interrupt*/
            uint32_t tx_done                       :    1;  /*The masked interrupt status bit  for the i2s_tx_done_int interrupt*/
            uint32_t rx_hung                       :    1;  /*The masked interrupt status bit  for the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung                       :    1;  /*The masked interrupt status bit  for the i2s_tx_hung_int interrupt*/
            uint32_t reserved4                     :    28;  /*Reserve*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rx_done                       :    1;  /*The interrupt enable bit  for the i2s_rx_done_int interrupt*/
            uint32_t tx_done                       :    1;  /*The interrupt enable bit  for the i2s_tx_done_int interrupt*/
            uint32_t rx_hung                       :    1;  /*The interrupt enable bit  for the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung                       :    1;  /*The interrupt enable bit  for the i2s_tx_hung_int interrupt*/
            uint32_t reserved4                     :    28;  /*Reserve*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_done                       :    1;  /*Set this bit to clear the i2s_rx_done_int interrupt*/
            uint32_t tx_done                       :    1;  /*Set this bit to clear the i2s_tx_done_int interrupt*/
            uint32_t rx_hung                       :    1;  /*Set this bit to clear the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung                       :    1;  /*Set this bit to clear the i2s_tx_hung_int interrupt*/
            uint32_t reserved4                     :    28;  /*Reserve*/
        };
        uint32_t val;
    } int_clr;
    uint32_t reserved_1c;
    union {
        struct {
            uint32_t rx_reset                      :    1;  /*Set this bit to reset receiver*/
            uint32_t rx_fifo_reset                 :    1;  /*Set this bit to reset Rx AFIFO*/
            uint32_t rx_start                      :    1;  /*Set this bit to start receiving data*/
            uint32_t rx_slave_mod                  :    1;  /*Set this bit to enable slave receiver mode*/
            uint32_t reserved4                     :    1;  /* Reserved*/
            uint32_t rx_mono                       :    1;  /*Set this bit to enable receiver  in mono mode*/
            uint32_t reserved6                     :    1;  /*Reserve*/
            uint32_t rx_big_endian                 :    1;  /*I2S Rx byte endian, 1: low addr value to high addr. 0: low addr with low addr value.*/
            uint32_t rx_update                     :    1;  /*Set 1 to update I2S RX registers from APB clock domain to I2S RX clock domain. This bit will be cleared by hardware after update register done.*/
            uint32_t rx_mono_fst_vld               :    1;  /*1: The first channel data value is valid in I2S RX mono mode.   0: The second channel data value is valid in I2S RX mono mode.*/
            uint32_t rx_pcm_conf                   :    2;  /*I2S RX compress/decompress configuration bit. & 0 (atol): A-Law decompress, 1 (ltoa) : A-Law compress, 2 (utol) : u-Law decompress, 3 (ltou) : u-Law compress. &*/
            uint32_t rx_pcm_bypass                 :    1;  /*Set this bit to bypass Compress/Decompress module for received data.*/
            uint32_t rx_stop_mode                  :    2;  /*0  : I2S Rx only stop when reg_rx_start is cleared.   1: Stop when reg_rx_start is 0 or in_suc_eof is 1.   2:  Stop I2S RX when reg_rx_start is 0 or RX FIFO is full.*/
            uint32_t rx_left_align                 :    1;  /*1: I2S RX left alignment mode. 0: I2S RX right alignment mode.*/
            uint32_t rx_24_fill_en                 :    1;  /*1: store 24 channel bits to 32 bits. 0:store 24 channel bits to 24 bits.*/
            uint32_t rx_ws_idle_pol                :    1;  /*0: WS should be 0 when receiving left channel data, and WS is 1in right channel.  1: WS should be 1 when receiving left channel data, and WS is 0in right channel. */
            uint32_t rx_bit_order                  :    1;  /*I2S Rx bit endian. 1:small endian, the LSB is received first. 0:big endian, the MSB is received first.*/
            uint32_t rx_tdm_en                     :    1;  /*1: Enable I2S TDM Rx mode . 0: Disable.*/
            uint32_t rx_pdm_en                     :    1;  /*1: Enable I2S PDM Rx mode . 0: Disable.*/
            uint32_t rx_pdm2pcm_en                 :    1;  /*1: Enable PDM2PCM RX mode. 0: DIsable.*/
            uint32_t rx_pdm_sinc_dsr_16_en         :    1;  /*Configure the down sampling rate of PDM RX filter group1 module. 1: The  down sampling rate is 128. 0: down sampling rate is 64.*/
            uint32_t reserved23                    :    9;  /*Reserve*/
        };
        uint32_t val;
    } rx_conf;
    union {
        struct {
            uint32_t tx_reset                      :    1;  /*Set this bit to reset transmitter*/
            uint32_t tx_fifo_reset                 :    1;  /*Set this bit to reset Tx AFIFO*/
            uint32_t tx_start                      :    1;  /*Set this bit to start transmitting data */
            uint32_t tx_slave_mod                  :    1;  /*Set this bit to enable slave transmitter mode  */
            uint32_t reserved4                     :    1;  /* Reserved*/
            uint32_t tx_mono                       :    1;  /*Set this bit to enable transmitter in mono mode  */
            uint32_t tx_chan_equal                 :    1;  /*1: The value of Left channel data is equal to the value of right channel data in I2S TX mono mode or TDM channel select mode. 0: The invalid channel data is reg_i2s_single_data in I2S TX mono mode or TDM channel select mode.*/
            uint32_t tx_big_endian                 :    1;  /*I2S Tx byte endian, 1: low addr value to high addr.  0: low addr with low addr value.*/
            uint32_t tx_update                     :    1;  /*Set 1 to update I2S TX registers from APB clock domain to I2S TX clock domain. This bit will be cleared by hardware after update register done.*/
            uint32_t tx_mono_fst_vld               :    1;  /*1: The first channel data value is valid in I2S TX mono mode.   0: The second channel data value is valid in I2S TX mono mode.*/
            uint32_t tx_pcm_conf                   :    2;  /*I2S TX compress/decompress configuration bit. & 0 (atol): A-Law decompress, 1 (ltoa) : A-Law compress, 2 (utol) : u-Law decompress, 3 (ltou) : u-Law compress. &*/
            uint32_t tx_pcm_bypass                 :    1;  /*Set this bit to bypass  Compress/Decompress module for transmitted data.*/
            uint32_t tx_stop_en                    :    1;  /*Set this bit to stop disable output BCK signal and WS signal when tx FIFO is emtpy*/
            uint32_t reserved14                    :    1;  /* Reserved*/
            uint32_t tx_left_align                 :    1;  /*1: I2S TX left alignment mode. 0: I2S TX right alignment mode.*/
            uint32_t tx_24_fill_en                 :    1;  /*1: Sent 32 bits in 24 channel bits mode. 0: Sent 24 bits in 24 channel bits mode*/
            uint32_t tx_ws_idle_pol                :    1;  /*0: WS should be 0 when sending left channel data, and WS is 1in right channel.  1: WS should be 1 when sending left channel data, and WS is 0in right channel. */
            uint32_t tx_bit_order                  :    1;  /*I2S Tx bit endian. 1:small endian, the LSB is sent first. 0:big endian, the MSB is sent first.*/
            uint32_t tx_tdm_en                     :    1;  /*1: Enable I2S TDM Tx mode . 0: Disable.*/
            uint32_t tx_pdm_en                     :    1;  /*1: Enable I2S PDM Tx mode . 0: Disable.*/
            uint32_t reserved21                    :    3;  /*Reserved*/
            uint32_t tx_chan_mod                   :    3;  /*I2S transmitter channel mode configuration bits.*/
            uint32_t sig_loopback                  :    1;  /*Enable signal loop back mode with transmitter module and receiver module sharing the same WS and BCK signals.*/
            uint32_t reserved28                    :    4;  /*Reserve*/
        };
        uint32_t val;
    } tx_conf;
    union {
        struct {
            uint32_t rx_tdm_ws_width               :    7;  /* The width of rx_ws_out in TDM mode is (I2S_RX_TDM_WS_WIDTH[6:0] +1) * T_bck*/
            uint32_t rx_bck_div_num                :    6;  /*Bit clock configuration bits in receiver mode. */
            uint32_t rx_bits_mod                   :    5;  /*Set the bits to configure the valid data bit length of I2S receiver channel. 7: all the valid channel data is in 8-bit-mode. 15: all the valid channel data is in 16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid channel data is in 32-bit-mode.*/
            uint32_t rx_half_sample_bits           :    6;  /*I2S Rx half sample bits -1.*/
            uint32_t rx_tdm_chan_bits              :    5;  /*The Rx bit number for each channel minus 1in TDM mode.*/
            uint32_t rx_msb_shift                  :    1;  /*Set this bit to enable receiver in Phillips standard mode*/
            uint32_t reserved30                    :    2;  /* Reserved*/
        };
        uint32_t val;
    } rx_conf1;
    union {
        struct {
            uint32_t tx_tdm_ws_width               :    7;  /* The width of tx_ws_out in TDM mode is (I2S_TX_TDM_WS_WIDTH[6:0] +1) * T_bck*/
            uint32_t tx_bck_div_num                :    6;  /*Bit clock configuration bits in transmitter mode. */
            uint32_t tx_bits_mod                   :    5;  /*Set the bits to configure the valid data bit length of I2S transmitter channel. 7: all the valid channel data is in 8-bit-mode. 15: all the valid channel data is in 16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid channel data is in 32-bit-mode.*/
            uint32_t tx_half_sample_bits           :    6;  /* I2S Tx half sample bits -1.*/
            uint32_t tx_tdm_chan_bits              :    5;  /*The Tx bit number for each channel minus 1in TDM mode.*/
            uint32_t tx_msb_shift                  :    1;  /*Set this bit to enable transmitter in Phillips standard mode*/
            uint32_t tx_bck_no_dly                 :    1;  /*1: BCK is not delayed to generate pos/neg edge in master mode. 0: BCK is delayed to generate pos/neg edge in master mode.*/
            uint32_t reserved31                    :    1;  /* Reserved*/
        };
        uint32_t val;
    } tx_conf1;
    union {
        struct {
            uint32_t rx_clkm_div_num               :    8;  /*Integral I2S clock divider value*/
            uint32_t reserved8                     :    18;  /* Reserved*/
            uint32_t rx_clk_active                 :    1;  /*I2S Rx module clock enable signal.*/
            uint32_t rx_clk_sel                    :    2;  /*Select I2S Rx module source clock. 0: no clock. 1: APLL. 2: CLK160. 3: I2S_MCLK_in.*/
            uint32_t mclk_sel                      :    1;  /* 0: UseI2S Tx module clock as I2S_MCLK_OUT.  1: UseI2S Rx module clock as I2S_MCLK_OUT. */
            uint32_t reserved30                    :    2;  /* Reserved*/
        };
        uint32_t val;
    } rx_clkm_conf;
    union {
        struct {
            uint32_t tx_clkm_div_num               :    8;  /*Integral I2S TX clock divider value. f_I2S_CLK = f_I2S_CLK_S/(N+b/a). There will be (a-b) * n-div and b * (n+1)-div.  So the average combination will be:  for b <= a/2, z * [x * n-div + (n+1)-div] + y * n-div. For b > a/2, z * [n-div + x * (n+1)-div] + y * (n+1)-div. */
            uint32_t reserved8                     :    18;  /* Reserved*/
            uint32_t tx_clk_active                 :    1;  /*I2S Tx module clock enable signal.*/
            uint32_t tx_clk_sel                    :    2;  /*Select I2S Tx module source clock. 0: XTAL clock. 1: APLL. 2: CLK160. 3: I2S_MCLK_in.*/
            uint32_t clk_en                        :    1;  /*Set this bit to enable clk gate*/
            uint32_t reserved30                    :    2;  /* Reserved*/
        };
        uint32_t val;
    } tx_clkm_conf;
    union {
        struct {
            uint32_t rx_clkm_div_z                 :    9;  /*For b <= a/2, the value of I2S_RX_CLKM_DIV_Z is b. For b > a/2, the value of I2S_RX_CLKM_DIV_Z is (a-b). */
            uint32_t rx_clkm_div_y                 :    9;  /*For b <= a/2, the value of I2S_RX_CLKM_DIV_Y is (a%b) . For b > a/2, the value of I2S_RX_CLKM_DIV_Y is (a%(a-b)). */
            uint32_t rx_clkm_div_x                 :    9;  /*For b <= a/2, the value of I2S_RX_CLKM_DIV_X is (a/b) - 1. For b > a/2, the value of I2S_RX_CLKM_DIV_X is (a/(a-b)) - 1. */
            uint32_t rx_clkm_div_yn1               :    1;  /*For b <= a/2, the value of I2S_RX_CLKM_DIV_YN1 is 0 . For b > a/2, the value of I2S_RX_CLKM_DIV_YN1 is 1. */
            uint32_t reserved28                    :    4;  /* Reserved*/
        };
        uint32_t val;
    } rx_clkm_div_conf;
    union {
        struct {
            uint32_t tx_clkm_div_z                 :    9;  /*For b <= a/2, the value of I2S_TX_CLKM_DIV_Z is b. For b > a/2, the value of I2S_TX_CLKM_DIV_Z is (a-b). */
            uint32_t tx_clkm_div_y                 :    9;  /*For b <= a/2, the value of I2S_TX_CLKM_DIV_Y is (a%b) . For b > a/2, the value of I2S_TX_CLKM_DIV_Y is (a%(a-b)). */
            uint32_t tx_clkm_div_x                 :    9;  /*For b <= a/2, the value of I2S_TX_CLKM_DIV_X is (a/b) - 1. For b > a/2, the value of I2S_TX_CLKM_DIV_X is (a/(a-b)) - 1. */
            uint32_t tx_clkm_div_yn1               :    1;  /*For b <= a/2, the value of I2S_TX_CLKM_DIV_YN1 is 0 . For b > a/2, the value of I2S_TX_CLKM_DIV_YN1 is 1. */
            uint32_t reserved28                    :    4;  /* Reserved*/
        };
        uint32_t val;
    } tx_clkm_div_conf;
    union {
        struct {
            uint32_t tx_hp_bypass                  :    1;  /*I2S TX PDM bypass hp filter or not. The option has been removed.*/
            uint32_t tx_sinc_osr2                  :    4;  /*I2S TX PDM OSR2 value*/
            uint32_t tx_prescale                   :    8;  /*I2S TX PDM prescale for sigmadelta*/
            uint32_t tx_hp_in_shift                :    2;  /*I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4*/
            uint32_t tx_lp_in_shift                :    2;  /*I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4*/
            uint32_t tx_sinc_in_shift              :    2;  /*I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4*/
            uint32_t tx_sigmadelta_in_shift        :    2;  /*I2S TX PDM sigmadelta scale shift number: 0:/2 , 1:x1 , 2:x2 , 3: x4*/
            uint32_t tx_sigmadelta_dither2         :    1;  /*I2S TX PDM sigmadelta dither2 value*/
            uint32_t tx_sigmadelta_dither          :    1;  /*I2S TX PDM sigmadelta dither value*/
            uint32_t tx_dac_2out_en                :    1;  /*I2S TX PDM dac mode enable*/
            uint32_t tx_dac_mode_en                :    1;  /*I2S TX PDM dac 2channel enable*/
            uint32_t pcm2pdm_conv_en               :    1;  /*I2S TX PDM Converter enable*/
            uint32_t reserved26                    :    6;  /*Reserved*/
        };
        uint32_t val;
    } tx_pcm2pdm_conf;
    union {
        struct {
            uint32_t tx_pdm_fp                     :    10;  /*I2S TX PDM Fp*/
            uint32_t tx_pdm_fs                     :    10;  /*I2S TX PDM Fs*/
            uint32_t tx_iir_hp_mult12_5            :    3;  /*The fourth parameter of PDM TX IIR_HP filter stage 2 is (504 + I2S_TX_IIR_HP_MULT12_5[2:0])*/
            uint32_t tx_iir_hp_mult12_0            :    3;  /*The fourth parameter of PDM TX IIR_HP filter stage 1 is (504 + I2S_TX_IIR_HP_MULT12_0[2:0])*/
            uint32_t reserved26                    :    6;  /*Reserved*/
        };
        uint32_t val;
    } tx_pcm2pdm_conf1;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    union {
        struct {
            uint32_t rx_tdm_chan0_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan1_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan2_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan3_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan4_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan5_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan6_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan7_en               :    1;  /*1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan8_en               :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan9_en               :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan10_en              :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan11_en              :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan12_en              :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan13_en              :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan14_en              :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_chan15_en              :    1;  /*1: Enable the valid data input of I2S RX TDM channel $n. 0:  Disable, just input 0 in this channel.*/
            uint32_t rx_tdm_tot_chan_num           :    4;  /*The total channel number of I2S TX TDM mode.*/
            uint32_t reserved20                    :    12;  /* Reserved*/
        };
        uint32_t val;
    } rx_tdm_ctrl;
    union {
        struct {
            uint32_t tx_tdm_chan0_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan1_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan2_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan3_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan4_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan5_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan6_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan7_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan8_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan9_en               :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan10_en              :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan11_en              :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan12_en              :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan13_en              :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan14_en              :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_chan15_en              :    1;  /*1: Enable the valid data output of I2S TX TDM channel $n. 0:  Disable, just output 0 in this channel.*/
            uint32_t tx_tdm_tot_chan_num           :    4;  /*The total channel number of I2S TX TDM mode.*/
            uint32_t tx_tdm_skip_msk_en            :    1;  /*When DMA TX buffer stores the data of (REG_TX_TDM_TOT_CHAN_NUM + 1)  channels, and only the data of the enabled channels is sent, then this bit should be set. Clear it when all the data stored in DMA TX buffer is for enabled channels.*/
            uint32_t reserved21                    :    11;  /* Reserved*/
        };
        uint32_t val;
    } tx_tdm_ctrl;
    union {
        struct {
            uint32_t rx_sd_in_dm                   :    2;  /*The delay mode of I2S Rx SD input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved2                     :    2;
            uint32_t rx_sd1_in_dm                  :    2;  /*The delay mode of I2S Rx SD1 input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved6                     :    2;
            uint32_t rx_sd2_in_dm                  :    2;  /*The delay mode of I2S Rx SD2 input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved10                    :    2;
            uint32_t rx_sd3_in_dm                  :    2;  /*The delay mode of I2S Rx SD3 input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved14                    :    2;
            uint32_t rx_ws_out_dm                  :    2;  /*The delay mode of I2S Rx WS output signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved18                    :    2;  /* Reserved*/
            uint32_t rx_bck_out_dm                 :    2;  /*The delay mode of I2S Rx BCK output signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved22                    :    2;  /* Reserved*/
            uint32_t rx_ws_in_dm                   :    2;  /*The delay mode of I2S Rx WS input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved26                    :    2;  /* Reserved*/
            uint32_t rx_bck_in_dm                  :    2;  /*The delay mode of I2S Rx BCK input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved30                    :    2;  /* Reserved*/
        };
        uint32_t val;
    } rx_timing;
    union {
        struct {
            uint32_t tx_sd_out_dm                  :    2;  /*The delay mode of I2S TX SD output signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved2                     :    2;  /* Reserved*/
            uint32_t tx_sd1_out_dm                 :    2;  /*The delay mode of I2S TX SD1 output signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved6                     :    10;  /* Reserved*/
            uint32_t tx_ws_out_dm                  :    2;  /*The delay mode of I2S TX WS output signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved18                    :    2;  /* Reserved*/
            uint32_t tx_bck_out_dm                 :    2;  /*The delay mode of I2S TX BCK output signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved22                    :    2;  /* Reserved*/
            uint32_t tx_ws_in_dm                   :    2;  /*The delay mode of I2S TX WS input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved26                    :    2;  /* Reserved*/
            uint32_t tx_bck_in_dm                  :    2;  /*The delay mode of I2S TX BCK input signal. 0: bypass. 1: delay by pos edge.  2: delay by neg edge. 3: not used.*/
            uint32_t reserved30                    :    2;  /* Reserved*/
        };
        uint32_t val;
    } tx_timing;
    union {
        struct {
            uint32_t fifo_timeout                  :    8;  /*the i2s_tx_hung_int interrupt or the i2s_rx_hung_int interrupt will be triggered when fifo hung counter is equal to this value  */
            uint32_t fifo_timeout_shift            :    3;  /*The bits are used to scale tick counter threshold. The tick counter is reset when counter value >= 88000/2^i2s_lc_fifo_timeout_shift*/
            uint32_t fifo_timeout_ena              :    1;  /*The enable bit for FIFO timeout*/
            uint32_t reserved12                    :    20;  /* Reserved*/
        };
        uint32_t val;
    } lc_hung_conf;
    union {
        struct {
            uint32_t rx_eof_num                    :    12;  /*The receive data bit length is (I2S_RX_BITS_MOD[4:0] + 1) * (REG_RX_EOF_NUM[11:0] + 1) . It will trigger in_suc_eof interrupt in the configured DMA RX channel.*/
            uint32_t reserved12                    :    20;  /* Reserved*/
        };
        uint32_t val;
    } rx_eof_num;
    uint32_t conf_single_data;                               /*the right channel or left channel put out constant value stored in this register according to tx_chan_mod and reg_tx_msb_right*/
    union {
        struct {
            uint32_t tx_idle                       :    1;  /*1: i2s_tx is idle state. 0: i2s_tx is working.*/
            uint32_t reserved1                     :    31;  /* Reserved*/
        };
        uint32_t val;
    } state;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    union {
        struct {
            uint32_t date                          :    28;  /*I2S version control register*/
            uint32_t reserved28                    :    4;  /* Reserved*/
        };
        uint32_t val;
    } date;
} i2s_dev_t;
extern i2s_dev_t I2S0;
extern i2s_dev_t I2S1;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_I2S_STRUCT_H_ */
