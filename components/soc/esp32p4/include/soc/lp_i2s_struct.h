/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: VAD registers */
/** Type of vad_conf register
 *  I2S VAD Configure register
 */
typedef union {
    struct {
        /** vad_en : R/W; bitpos: [0]; default: 0;
         *  VAD enable register
         */
        uint32_t vad_en:1;
        /** vad_reset : WT; bitpos: [1]; default: 0;
         *  VAD reset register
         */
        uint32_t vad_reset:1;
        /** vad_force_start : WT; bitpos: [2]; default: 0;
         *  VAD force start register.
         */
        uint32_t vad_force_start:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_i2s_vad_conf_reg_t;

/** Type of vad_result register
 *  I2S VAD Result register
 */
typedef union {
    struct {
        /** vad_flag : RO; bitpos: [0]; default: 0;
         *  Reg vad flag observe signal
         */
        uint32_t vad_flag:1;
        /** energy_enough : RO; bitpos: [1]; default: 0;
         *  Reg energy enough observe signal
         */
        uint32_t energy_enough:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_i2s_vad_result_reg_t;

/** Type of vad_param0 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_min_energy : R/W; bitpos: [15:0]; default: 5000;
         *  VAD parameter
         */
        uint32_t param_min_energy:16;
        /** param_init_frame_num : R/W; bitpos: [24:16]; default: 200;
         *  VAD parameter
         */
        uint32_t param_init_frame_num:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} lp_i2s_vad_param0_reg_t;

/** Type of vad_param1 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_min_speech_count : R/W; bitpos: [3:0]; default: 3;
         *  VAD parameter
         */
        uint32_t param_min_speech_count:4;
        /** param_max_speech_count : R/W; bitpos: [10:4]; default: 100;
         *  VAD parameter
         */
        uint32_t param_max_speech_count:7;
        /** param_hangover_speech : R/W; bitpos: [15:11]; default: 3;
         *  VAD parameter
         */
        uint32_t param_hangover_speech:5;
        /** param_hangover_silent : R/W; bitpos: [23:16]; default: 30;
         *  VAD parameter
         */
        uint32_t param_hangover_silent:8;
        /** param_max_offset : R/W; bitpos: [30:24]; default: 40;
         *  VAD parameter
         */
        uint32_t param_max_offset:7;
        /** param_skip_band_energy : R/W; bitpos: [31]; default: 0;
         *  Set 1 to skip band energy check.
         */
        uint32_t param_skip_band_energy:1;
    };
    uint32_t val;
} lp_i2s_vad_param1_reg_t;

/** Type of vad_param2 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_noise_amp_down : R/W; bitpos: [15:0]; default: 26214;
         *  VAD parameter
         */
        uint32_t param_noise_amp_down:16;
        /** param_noise_amp_up : R/W; bitpos: [31:16]; default: 32440;
         *  VAD parameter
         */
        uint32_t param_noise_amp_up:16;
    };
    uint32_t val;
} lp_i2s_vad_param2_reg_t;

/** Type of vad_param3 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_noise_spe_up0 : R/W; bitpos: [15:0]; default: 32735;
         *  VAD parameter
         */
        uint32_t param_noise_spe_up0:16;
        /** param_noise_spe_up1 : R/W; bitpos: [31:16]; default: 32113;
         *  VAD parameter
         */
        uint32_t param_noise_spe_up1:16;
    };
    uint32_t val;
} lp_i2s_vad_param3_reg_t;

/** Type of vad_param4 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_noise_spe_down : R/W; bitpos: [15:0]; default: 26214;
         *  VAD parameter
         */
        uint32_t param_noise_spe_down:16;
        /** param_noise_mean_down : R/W; bitpos: [31:16]; default: 31130;
         *  VAD parameter
         */
        uint32_t param_noise_mean_down:16;
    };
    uint32_t val;
} lp_i2s_vad_param4_reg_t;

/** Type of vad_param5 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_noise_mean_up0 : R/W; bitpos: [15:0]; default: 32113;
         *  VAD parameter
         */
        uint32_t param_noise_mean_up0:16;
        /** param_noise_mean_up1 : R/W; bitpos: [31:16]; default: 31784;
         *  VAD parameter
         */
        uint32_t param_noise_mean_up1:16;
    };
    uint32_t val;
} lp_i2s_vad_param5_reg_t;

/** Type of vad_param6 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_noise_std_fs_thsl : R/W; bitpos: [15:0]; default: 32000;
         *  Feature_sum threshold to determine noise_std max value when vad_tag=1, equal to
         *  ((noise_std_max)>>11)^2*5
         */
        uint32_t param_noise_std_fs_thsl:16;
        /** param_noise_std_fs_thsh : R/W; bitpos: [31:16]; default: 46080;
         *  Feature_sum threshold to determine noise_std max value when vad_tag=0, equal to
         *  ((noise_std_max)>>11)^2*5
         */
        uint32_t param_noise_std_fs_thsh:16;
    };
    uint32_t val;
} lp_i2s_vad_param6_reg_t;

/** Type of vad_param7 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_thres_upd_base : R/W; bitpos: [15:0]; default: 32440;
         *  VAD parameter
         */
        uint32_t param_thres_upd_base:16;
        /** param_thres_upd_vary : R/W; bitpos: [31:16]; default: 328;
         *  VAD parameter
         */
        uint32_t param_thres_upd_vary:16;
    };
    uint32_t val;
} lp_i2s_vad_param7_reg_t;

/** Type of vad_param8 register
 *  I2S VAD Parameter register
 */
typedef union {
    struct {
        /** param_thres_upd_bdl : R/W; bitpos: [7:0]; default: 64;
         *  Noise_std boundary low when updating threshold.
         */
        uint32_t param_thres_upd_bdl:8;
        /** param_thres_upd_bdh : R/W; bitpos: [15:8]; default: 80;
         *  Noise_std boundary high when updating threshold.
         */
        uint32_t param_thres_upd_bdh:8;
        /** param_feature_burst : R/W; bitpos: [31:16]; default: 8192;
         *  VAD parameter
         */
        uint32_t param_feature_burst:16;
    };
    uint32_t val;
} lp_i2s_vad_param8_reg_t;

/** Type of vad_ob0 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** speech_count_ob : RO; bitpos: [7:0]; default: 0;
         *  Reg silent count observe
         */
        uint32_t speech_count_ob:8;
        /** silent_count_ob : RO; bitpos: [15:8]; default: 0;
         *  Reg speech count observe
         */
        uint32_t silent_count_ob:8;
        /** max_signal0_ob : RO; bitpos: [31:16]; default: 0;
         *  Reg max signal0 observe
         */
        uint32_t max_signal0_ob:16;
    };
    uint32_t val;
} lp_i2s_vad_ob0_reg_t;

/** Type of vad_ob1 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** max_signal1_ob : RO; bitpos: [15:0]; default: 0;
         *  Reg max signal1 observe
         */
        uint32_t max_signal1_ob:16;
        /** max_signal2_ob : RO; bitpos: [31:16]; default: 0;
         *  Reg max signal2 observe
         */
        uint32_t max_signal2_ob:16;
    };
    uint32_t val;
} lp_i2s_vad_ob1_reg_t;

/** Type of vad_ob2 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** noise_amp_ob : RO; bitpos: [31:0]; default: 0;
         *  Reg noise_amp observe signal
         */
        uint32_t noise_amp_ob:32;
    };
    uint32_t val;
} lp_i2s_vad_ob2_reg_t;

/** Type of vad_ob3 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** noise_mean_ob : RO; bitpos: [31:0]; default: 0;
         *  Reg noise_mean observe signal
         */
        uint32_t noise_mean_ob:32;
    };
    uint32_t val;
} lp_i2s_vad_ob3_reg_t;

/** Type of vad_ob4 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** noise_std_ob : RO; bitpos: [31:0]; default: 0;
         *  Reg noise_std observe signal
         */
        uint32_t noise_std_ob:32;
    };
    uint32_t val;
} lp_i2s_vad_ob4_reg_t;

/** Type of vad_ob5 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** offset_ob : RO; bitpos: [31:0]; default: 0;
         *  Reg offset observe signal
         */
        uint32_t offset_ob:32;
    };
    uint32_t val;
} lp_i2s_vad_ob5_reg_t;

/** Type of vad_ob6 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** threshold_ob : RO; bitpos: [31:0]; default: 0;
         *  Reg threshold observe signal
         */
        uint32_t threshold_ob:32;
    };
    uint32_t val;
} lp_i2s_vad_ob6_reg_t;

/** Type of vad_ob7 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** energy_low_ob : RO; bitpos: [31:0]; default: 0;
         *  Reg energy bit 31~0 observe signal
         */
        uint32_t energy_low_ob:32;
    };
    uint32_t val;
} lp_i2s_vad_ob7_reg_t;

/** Type of vad_ob8 register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** energy_high_ob : RO; bitpos: [31:0]; default: 0;
         *  Reg energy bit 63~32 observe signal
         */
        uint32_t energy_high_ob:32;
    };
    uint32_t val;
} lp_i2s_vad_ob8_reg_t;


/** Group: RX Control and configuration registers */
/** Type of rx_mem_conf register
 *  I2S VAD Observe register
 */
typedef union {
    struct {
        /** rx_mem_fifo_cnt : RO; bitpos: [8:0]; default: 0;
         *  The number of data in the rx mem
         */
        uint32_t rx_mem_fifo_cnt:9;
        /** rx_mem_threshold : R/W; bitpos: [16:9]; default: 63;
         *  I2S rx mem will trigger an interrupt when the data in the mem is over(not including
         *  equal) reg_rx_mem_threshold
         */
        uint32_t rx_mem_threshold:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_i2s_rx_mem_conf_reg_t;

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
        /** rx_start : R/W; bitpos: [2]; default: 0;
         *  Set this bit to start receiving data
         */
        uint32_t rx_start:1;
        /** rx_slave_mod : R/W; bitpos: [3]; default: 0;
         *  Set this bit to enable slave receiver mode
         */
        uint32_t rx_slave_mod:1;
        /** rx_fifomem_reset : WT; bitpos: [4]; default: 0;
         *  Set this bit to reset Rx Syncfifomem
         */
        uint32_t rx_fifomem_reset:1;
        /** rx_mono : R/W; bitpos: [5]; default: 0;
         *  Set this bit to enable receiver  in mono mode
         */
        uint32_t rx_mono:1;
        uint32_t reserved_6:1;
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
        /** rx_stop_mode : R/W; bitpos: [14:13]; default: 0;
         *  0  : I2S Rx only stop when reg_rx_start is cleared.   1: Stop when reg_rx_start is
         *  0 or in_suc_eof is 1.   2:  Stop I2S RX when reg_rx_start is 0 or RX FIFO is full.
         */
        uint32_t rx_stop_mode:2;
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
        uint32_t reserved_21:11;
    };
    uint32_t val;
} lp_i2s_rx_conf_reg_t;

/** Type of rx_conf1 register
 *  I2S RX configure register 1
 */
typedef union {
    struct {
        /** rx_tdm_ws_width : R/W; bitpos: [6:0]; default: 0;
         *  The width of rx_ws_out in TDM mode is (I2S_RX_TDM_WS_WIDTH[6:0] +1) * T_bck
         */
        uint32_t rx_tdm_ws_width:7;
        /** rx_bck_div_num : R/W; bitpos: [12:7]; default: 6;
         *  Bit clock configuration bits in receiver mode.
         */
        uint32_t rx_bck_div_num:6;
        /** rx_bits_mod : R/W; bitpos: [17:13]; default: 15;
         *  Set the bits to configure the valid data bit length of I2S receiver channel. 7: all
         *  the valid channel data is in 8-bit-mode. 15: all the valid channel data is in
         *  16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid
         *  channel data is in 32-bit-mode.
         */
        uint32_t rx_bits_mod:5;
        /** rx_half_sample_bits : R/W; bitpos: [23:18]; default: 15;
         *  I2S Rx half sample bits -1.
         */
        uint32_t rx_half_sample_bits:6;
        /** rx_tdm_chan_bits : R/W; bitpos: [28:24]; default: 15;
         *  The Rx bit number for each channel minus 1in TDM mode.
         */
        uint32_t rx_tdm_chan_bits:5;
        /** rx_msb_shift : R/W; bitpos: [29]; default: 1;
         *  Set this bit to enable receiver in Phillips standard mode
         */
        uint32_t rx_msb_shift:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} lp_i2s_rx_conf1_reg_t;

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
        uint32_t reserved_2:14;
        /** rx_tdm_tot_chan_num : R/W; bitpos: [19:16]; default: 0;
         *  The total channel number of I2S TX TDM mode.
         */
        uint32_t rx_tdm_tot_chan_num:4;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_i2s_rx_tdm_ctrl_reg_t;

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
} lp_i2s_rxeof_num_reg_t;

/** Type of rx_pdm_conf register
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
        /** rx_iir_hp_mult12_5 : R/W; bitpos: [28:26]; default: 6;
         *  The fourth parameter of PDM RX IIR_HP filter stage 2 is (504 +
         *  LP_I2S_RX_IIR_HP_MULT12_5[2:0])
         */
        uint32_t rx_iir_hp_mult12_5:3;
        /** rx_iir_hp_mult12_0 : R/W; bitpos: [31:29]; default: 7;
         *  The fourth parameter of PDM RX IIR_HP filter stage 1 is (504 +
         *  LP_I2S_RX_IIR_HP_MULT12_0[2:0])
         */
        uint32_t rx_iir_hp_mult12_0:3;
    };
    uint32_t val;
} lp_i2s_rx_pdm_conf_reg_t;


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
        /** rx_hung_int_raw : RO/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status bit  for the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_raw:1;
        /** rx_fifomem_udf_int_raw : RO/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status bit  for the i2s_rx_fifomem_udf_int interrupt
         */
        uint32_t rx_fifomem_udf_int_raw:1;
        /** vad_done_int_raw : RO/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status bit  for the vad_done_int interrupt
         */
        uint32_t vad_done_int_raw:1;
        /** vad_reset_done_int_raw : RO/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status bit  for the vad_reset_done_int interrupt
         */
        uint32_t vad_reset_done_int_raw:1;
        /** rx_mem_threshold_int_raw : RO/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status bit  for the rx_mem_threshold_int interrupt
         */
        uint32_t rx_mem_threshold_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_i2s_int_raw_reg_t;

/** Type of int_st register
 *  I2S interrupt status register.
 */
typedef union {
    struct {
        /** rx_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit  for the i2s_rx_done_int interrupt
         */
        uint32_t rx_done_int_st:1;
        /** rx_hung_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit  for the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_st:1;
        /** rx_fifomem_udf_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit  for the i2s_rx_fifomem_udf_int interrupt
         */
        uint32_t rx_fifomem_udf_int_st:1;
        /** vad_done_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status bit  for the vad_done_int interrupt
         */
        uint32_t vad_done_int_st:1;
        /** vad_reset_done_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status bit  for the vad_reset_done_int interrupt
         */
        uint32_t vad_reset_done_int_st:1;
        /** rx_mem_threshold_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status bit  for the rx_mem_threshold_int interrupt
         */
        uint32_t rx_mem_threshold_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_i2s_int_st_reg_t;

/** Type of int_ena register
 *  I2S interrupt enable register.
 */
typedef union {
    struct {
        /** rx_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit  for the i2s_rx_done_int interrupt
         */
        uint32_t rx_done_int_ena:1;
        /** rx_hung_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit  for the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_ena:1;
        /** rx_fifomem_udf_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit  for the i2s_rx_fifomem_udf_int interrupt
         */
        uint32_t rx_fifomem_udf_int_ena:1;
        /** vad_done_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit  for the vad_done_int interrupt
         */
        uint32_t vad_done_int_ena:1;
        /** vad_reset_done_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit  for the vad_reset_done_int interrupt
         */
        uint32_t vad_reset_done_int_ena:1;
        /** rx_mem_threshold_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit  for the rx_mem_threshold_int interrupt
         */
        uint32_t rx_mem_threshold_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_i2s_int_ena_reg_t;

/** Type of int_clr register
 *  I2S interrupt clear register.
 */
typedef union {
    struct {
        /** rx_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the i2s_rx_done_int interrupt
         */
        uint32_t rx_done_int_clr:1;
        /** rx_hung_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the i2s_rx_hung_int interrupt
         */
        uint32_t rx_hung_int_clr:1;
        /** rx_fifomem_udf_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the i2s_rx_fifomem_udf_int interrupt
         */
        uint32_t rx_fifomem_udf_int_clr:1;
        /** vad_done_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the vad_done_int interrupt
         */
        uint32_t vad_done_int_clr:1;
        /** vad_reset_done_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the vad_reset_done_int interrupt
         */
        uint32_t vad_reset_done_int_clr:1;
        /** rx_mem_threshold_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the rx_mem_threshold_int interrupt
         */
        uint32_t rx_mem_threshold_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_i2s_int_clr_reg_t;


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
        uint32_t reserved_2:14;
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
} lp_i2s_rx_timing_reg_t;


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
} lp_i2s_lc_hung_conf_reg_t;

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
} lp_i2s_conf_sigle_data_reg_t;


/** Group: ECO registers */
/** Type of eco_low register
 *  I2S ECO register
 */
typedef union {
    struct {
        /** rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  logic low eco registers
         */
        uint32_t rdn_eco_low:32;
    };
    uint32_t val;
} lp_i2s_eco_low_reg_t;

/** Type of eco_high register
 *  I2S ECO register
 */
typedef union {
    struct {
        /** rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  logic high eco registers
         */
        uint32_t rdn_eco_high:32;
    };
    uint32_t val;
} lp_i2s_eco_high_reg_t;

/** Type of eco_conf register
 *  I2S ECO register
 */
typedef union {
    struct {
        /** rdn_ena : R/W; bitpos: [0]; default: 0;
         *  enable rdn counter bit
         */
        uint32_t rdn_ena:1;
        /** rdn_result : RO; bitpos: [1]; default: 0;
         *  rdn result
         */
        uint32_t rdn_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_i2s_eco_conf_reg_t;


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
        /** vad_cg_force_on : R/W; bitpos: [1]; default: 1;
         *  VAD clock gate force on register
         */
        uint32_t vad_cg_force_on:1;
        /** rx_mem_cg_force_on : R/W; bitpos: [2]; default: 0;
         *  I2S rx mem clock gate force on register
         */
        uint32_t rx_mem_cg_force_on:1;
        /** rx_reg_cg_force_on : R/W; bitpos: [3]; default: 1;
         *  I2S rx reg clock gate force on register
         */
        uint32_t rx_reg_cg_force_on:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_i2s_clk_gate_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36720704;
         *  I2S version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_i2s_date_reg_t;


typedef struct {
    volatile lp_i2s_vad_conf_reg_t vad_conf;
    volatile lp_i2s_vad_result_reg_t vad_result;
    volatile lp_i2s_rx_mem_conf_reg_t rx_mem_conf;
    volatile lp_i2s_int_raw_reg_t int_raw;
    volatile lp_i2s_int_st_reg_t int_st;
    volatile lp_i2s_int_ena_reg_t int_ena;
    volatile lp_i2s_int_clr_reg_t int_clr;
    uint32_t reserved_01c;
    volatile lp_i2s_rx_conf_reg_t rx_conf;
    uint32_t reserved_024;
    volatile lp_i2s_rx_conf1_reg_t rx_conf1;
    uint32_t reserved_02c[9];
    volatile lp_i2s_rx_tdm_ctrl_reg_t rx_tdm_ctrl;
    uint32_t reserved_054;
    volatile lp_i2s_rx_timing_reg_t rx_timing;
    uint32_t reserved_05c;
    volatile lp_i2s_lc_hung_conf_reg_t lc_hung_conf;
    volatile lp_i2s_rxeof_num_reg_t rxeof_num;
    volatile lp_i2s_conf_sigle_data_reg_t conf_sigle_data;
    uint32_t reserved_06c;
    volatile lp_i2s_rx_pdm_conf_reg_t rx_pdm_conf;
    volatile lp_i2s_eco_low_reg_t eco_low;
    volatile lp_i2s_eco_high_reg_t eco_high;
    volatile lp_i2s_eco_conf_reg_t eco_conf;
    volatile lp_i2s_vad_param0_reg_t vad_param0;
    volatile lp_i2s_vad_param1_reg_t vad_param1;
    volatile lp_i2s_vad_param2_reg_t vad_param2;
    volatile lp_i2s_vad_param3_reg_t vad_param3;
    volatile lp_i2s_vad_param4_reg_t vad_param4;
    volatile lp_i2s_vad_param5_reg_t vad_param5;
    volatile lp_i2s_vad_param6_reg_t vad_param6;
    volatile lp_i2s_vad_param7_reg_t vad_param7;
    volatile lp_i2s_vad_param8_reg_t vad_param8;
    uint32_t reserved_0a4[3];
    volatile lp_i2s_vad_ob0_reg_t vad_ob0;
    volatile lp_i2s_vad_ob1_reg_t vad_ob1;
    volatile lp_i2s_vad_ob2_reg_t vad_ob2;
    volatile lp_i2s_vad_ob3_reg_t vad_ob3;
    volatile lp_i2s_vad_ob4_reg_t vad_ob4;
    volatile lp_i2s_vad_ob5_reg_t vad_ob5;
    volatile lp_i2s_vad_ob6_reg_t vad_ob6;
    volatile lp_i2s_vad_ob7_reg_t vad_ob7;
    volatile lp_i2s_vad_ob8_reg_t vad_ob8;
    uint32_t reserved_0d4[9];
    volatile lp_i2s_clk_gate_reg_t clk_gate;
    volatile lp_i2s_date_reg_t date;
} lp_i2s_dev_t;

extern lp_i2s_dev_t LP_I2S;

#ifndef __cplusplus
_Static_assert(sizeof(lp_i2s_dev_t) == 0x100, "Invalid size of lp_i2s_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
