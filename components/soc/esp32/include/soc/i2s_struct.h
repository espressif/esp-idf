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
#ifndef _SOC_I2S_STRUCT_H_
#define _SOC_I2S_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct i2s_dev_s {
    uint32_t fifo_wr;
    uint32_t fifo_rd;
    union {
        struct {
            uint32_t tx_reset:       1;
            uint32_t rx_reset:       1;
            uint32_t tx_fifo_reset:  1;
            uint32_t rx_fifo_reset:  1;
            uint32_t tx_start:       1;
            uint32_t rx_start:       1;
            uint32_t tx_slave_mod:   1;
            uint32_t rx_slave_mod:   1;
            uint32_t tx_right_first: 1;
            uint32_t rx_right_first: 1;
            uint32_t tx_msb_shift:   1;
            uint32_t rx_msb_shift:   1;
            uint32_t tx_short_sync:  1;
            uint32_t rx_short_sync:  1;
            uint32_t tx_mono:        1;
            uint32_t rx_mono:        1;
            uint32_t tx_msb_right:   1;
            uint32_t rx_msb_right:   1;
            uint32_t sig_loopback:   1;
            uint32_t reserved19:    13;
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t rx_take_data:  1;
            uint32_t tx_put_data:   1;
            uint32_t rx_wfull:      1;
            uint32_t rx_rempty:     1;
            uint32_t tx_wfull:      1;
            uint32_t tx_rempty:     1;
            uint32_t rx_hung:       1;
            uint32_t tx_hung:       1;
            uint32_t in_done:       1;
            uint32_t in_suc_eof:    1;
            uint32_t in_err_eof:    1;
            uint32_t out_done:      1;
            uint32_t out_eof:       1;
            uint32_t in_dscr_err:   1;
            uint32_t out_dscr_err:  1;
            uint32_t in_dscr_empty: 1;
            uint32_t out_total_eof: 1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_take_data:  1;
            uint32_t tx_put_data:   1;
            uint32_t rx_wfull:      1;
            uint32_t rx_rempty:     1;
            uint32_t tx_wfull:      1;
            uint32_t tx_rempty:     1;
            uint32_t rx_hung:       1;
            uint32_t tx_hung:       1;
            uint32_t in_done:       1;
            uint32_t in_suc_eof:    1;
            uint32_t in_err_eof:    1;
            uint32_t out_done:      1;
            uint32_t out_eof:       1;
            uint32_t in_dscr_err:   1;
            uint32_t out_dscr_err:  1;
            uint32_t in_dscr_empty: 1;
            uint32_t out_total_eof: 1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rx_take_data:  1;
            uint32_t tx_put_data:   1;
            uint32_t rx_wfull:      1;
            uint32_t rx_rempty:     1;
            uint32_t tx_wfull:      1;
            uint32_t tx_rempty:     1;
            uint32_t rx_hung:       1;
            uint32_t tx_hung:       1;
            uint32_t in_done:       1;
            uint32_t in_suc_eof:    1;
            uint32_t in_err_eof:    1;
            uint32_t out_done:      1;
            uint32_t out_eof:       1;
            uint32_t in_dscr_err:   1;
            uint32_t out_dscr_err:  1;
            uint32_t in_dscr_empty: 1;
            uint32_t out_total_eof: 1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t take_data:     1;
            uint32_t put_data:      1;
            uint32_t rx_wfull:      1;
            uint32_t rx_rempty:     1;
            uint32_t tx_wfull:      1;
            uint32_t tx_rempty:     1;
            uint32_t rx_hung:       1;
            uint32_t tx_hung:       1;
            uint32_t in_done:       1;
            uint32_t in_suc_eof:    1;
            uint32_t in_err_eof:    1;
            uint32_t out_done:      1;
            uint32_t out_eof:       1;
            uint32_t in_dscr_err:   1;
            uint32_t out_dscr_err:  1;
            uint32_t in_dscr_empty: 1;
            uint32_t out_total_eof: 1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t tx_bck_in_delay:   2;
            uint32_t tx_ws_in_delay:    2;
            uint32_t rx_bck_in_delay:   2;
            uint32_t rx_ws_in_delay:    2;
            uint32_t rx_sd_in_delay:    2;
            uint32_t tx_bck_out_delay:  2;
            uint32_t tx_ws_out_delay:   2;
            uint32_t tx_sd_out_delay:   2;
            uint32_t rx_ws_out_delay:   2;
            uint32_t rx_bck_out_delay:  2;
            uint32_t tx_dsync_sw:       1;
            uint32_t rx_dsync_sw:       1;
            uint32_t data_enable_delay: 2;
            uint32_t tx_bck_in_inv:     1;
            uint32_t reserved25:        7;
        };
        uint32_t val;
    } timing;
    union {
        struct {
            uint32_t rx_data_num:          6;
            uint32_t tx_data_num:          6;
            uint32_t dscr_en:              1;
            uint32_t tx_fifo_mod:          3;
            uint32_t rx_fifo_mod:          3;
            uint32_t tx_fifo_mod_force_en: 1;
            uint32_t rx_fifo_mod_force_en: 1;
            uint32_t reserved21:          11;
        };
        uint32_t val;
    } fifo_conf;
    uint32_t rx_eof_num;
    uint32_t conf_single_data;
    union {
        struct {
            uint32_t tx_chan_mod: 3;
            uint32_t rx_chan_mod: 2;
            uint32_t reserved5:  27;
        };
        uint32_t val;
    } conf_chan;
    union {
        struct {
            uint32_t addr:       20;
            uint32_t reserved20:  8;
            uint32_t stop:        1;
            uint32_t start:       1;
            uint32_t restart:     1;
            uint32_t park:        1;
        };
        uint32_t val;
    } out_link;
    union {
        struct {
            uint32_t addr:       20;
            uint32_t reserved20:  8;
            uint32_t stop:        1;
            uint32_t start:       1;
            uint32_t restart:     1;
            uint32_t park:        1;
        };
        uint32_t val;
    } in_link;
    uint32_t out_eof_des_addr;
    uint32_t in_eof_des_addr;
    uint32_t out_eof_bfr_des_addr;
    union {
        struct {
            uint32_t mode:       3;
            uint32_t reserved3:  1;
            uint32_t addr:       2;
            uint32_t reserved6: 26;
        };
        uint32_t val;
    } ahb_test;
    uint32_t in_link_dscr;
    uint32_t in_link_dscr_bf0;
    uint32_t in_link_dscr_bf1;
    uint32_t out_link_dscr;
    uint32_t out_link_dscr_bf0;
    uint32_t out_link_dscr_bf1;
    union {
        struct {
            uint32_t in_rst:             1;
            uint32_t out_rst:            1;
            uint32_t ahbm_fifo_rst:      1;
            uint32_t ahbm_rst:           1;
            uint32_t out_loop_test:      1;
            uint32_t in_loop_test:       1;
            uint32_t out_auto_wrback:    1;
            uint32_t out_no_restart_clr: 1;
            uint32_t out_eof_mode:       1;
            uint32_t outdscr_burst_en:   1;
            uint32_t indscr_burst_en:    1;
            uint32_t out_data_burst_en:  1;
            uint32_t check_owner:        1;
            uint32_t mem_trans_en:       1;
            uint32_t reserved14:        18;
        };
        uint32_t val;
    } lc_conf;
    union {
        struct {
            uint32_t wdata:      9;
            uint32_t reserved9:  7;
            uint32_t push:       1;
            uint32_t reserved17: 15;
        };
        uint32_t val;
    } out_fifo_push;
    union {
        struct {
            uint32_t rdata:      12;
            uint32_t reserved12:  4;
            uint32_t pop:         1;
            uint32_t reserved17: 15;
        };
        uint32_t val;
    } in_fifo_pop;
    uint32_t lc_state0;
    uint32_t lc_state1;
    union {
        struct {
            uint32_t fifo_timeout:       8;
            uint32_t fifo_timeout_shift: 3;
            uint32_t fifo_timeout_ena:   1;
            uint32_t reserved12:        20;
        };
        uint32_t val;
    } lc_hung_conf;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    union {
        struct {
            uint32_t y_max:16;
            uint32_t y_min:16;
        };
        uint32_t val;
    } cvsd_conf0;
    union {
        struct {
            uint32_t sigma_max:16;
            uint32_t sigma_min:16;
        };
        uint32_t val;
    } cvsd_conf1;
    union {
        struct {
            uint32_t cvsd_k:     3;
            uint32_t cvsd_j:     3;
            uint32_t cvsd_beta: 10;
            uint32_t cvsd_h:     3;
            uint32_t reserved19:13;
        };
        uint32_t val;
    } cvsd_conf2;
    union {
        struct {
            uint32_t good_pack_max:    6;
            uint32_t n_err_seg:        3;
            uint32_t shift_rate:       3;
            uint32_t max_slide_sample: 8;
            uint32_t pack_len_8k:      5;
            uint32_t n_min_err:        3;
            uint32_t reserved28:       4;
        };
        uint32_t val;
    } plc_conf0;
    union {
        struct {
            uint32_t bad_cef_atten_para:       8;
            uint32_t bad_cef_atten_para_shift: 4;
            uint32_t bad_ola_win2_para_shift:  4;
            uint32_t bad_ola_win2_para:        8;
            uint32_t slide_win_len:            8;
        };
        uint32_t val;
    } plc_conf1;
    union {
        struct {
            uint32_t cvsd_seg_mod: 2;
            uint32_t min_period:   5;
            uint32_t reserved7:   25;
        };
        uint32_t val;
    } plc_conf2;
    union {
        struct {
            uint32_t en:                1;
            uint32_t chan_mod:          1;
            uint32_t cvsd_dec_pack_err: 1;
            uint32_t cvsd_pack_len_8k:  5;
            uint32_t cvsd_inf_en:       1;
            uint32_t cvsd_dec_start:    1;
            uint32_t cvsd_dec_reset:    1;
            uint32_t plc_en:            1;
            uint32_t plc2dma_en:        1;
            uint32_t reserved13:       19;
        };
        uint32_t val;
    } esco_conf0;
    union {
        struct {
            uint32_t with_en:        1;
            uint32_t no_en:          1;
            uint32_t cvsd_enc_start: 1;
            uint32_t cvsd_enc_reset: 1;
            uint32_t reserved4:     28;
        };
        uint32_t val;
    } sco_conf0;
    union {
        struct {
            uint32_t tx_pcm_conf:    3;
            uint32_t tx_pcm_bypass:  1;
            uint32_t rx_pcm_conf:    3;
            uint32_t rx_pcm_bypass:  1;
            uint32_t tx_stop_en:     1;
            uint32_t tx_zeros_rm_en: 1;
            uint32_t reserved10:    22;
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t fifo_force_pd:    1;
            uint32_t fifo_force_pu:    1;
            uint32_t plc_mem_force_pd: 1;
            uint32_t plc_mem_force_pu: 1;
            uint32_t reserved4:       28;
        };
        uint32_t val;
    } pd_conf;
    union {
        struct {
            uint32_t camera_en:           1;
            uint32_t lcd_tx_wrx2_en:      1;
            uint32_t lcd_tx_sdx2_en:      1;
            uint32_t data_enable_test_en: 1;
            uint32_t data_enable:         1;
            uint32_t lcd_en:              1;
            uint32_t ext_adc_start_en:    1;
            uint32_t inter_valid_en:      1;
            uint32_t reserved8:          24;
        };
        uint32_t val;
    } conf2;
    union {
        struct {
            uint32_t clkm_div_num: 8;
            uint32_t clkm_div_b:   6;
            uint32_t clkm_div_a:   6;
            uint32_t clk_en:       1;
            uint32_t clka_en:      1;
            uint32_t reserved22:  10;
        };
        uint32_t val;
    } clkm_conf;
    union {
        struct {
            uint32_t tx_bck_div_num: 6;
            uint32_t rx_bck_div_num: 6;
            uint32_t tx_bits_mod:    6;
            uint32_t rx_bits_mod:    6;
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } sample_rate_conf;
    union {
        struct {
            uint32_t tx_pdm_en:              1;
            uint32_t rx_pdm_en:              1;
            uint32_t pcm2pdm_conv_en:        1;
            uint32_t pdm2pcm_conv_en:        1;
            uint32_t tx_sinc_osr2:           4;
            uint32_t tx_prescale:            8;
            uint32_t tx_hp_in_shift:         2;
            uint32_t tx_lp_in_shift:         2;
            uint32_t tx_sinc_in_shift:       2;
            uint32_t tx_sigmadelta_in_shift: 2;
            uint32_t rx_sinc_dsr_16_en:      1;
            uint32_t txhp_bypass:            1;
            uint32_t reserved26:             6;
        };
        uint32_t val;
    } pdm_conf;
    union {
        struct {
            uint32_t tx_pdm_fs: 10;
            uint32_t tx_pdm_fp: 10;
            uint32_t reserved20:12;
        };
        uint32_t val;
    } pdm_freq_conf;
    union {
        struct {
            uint32_t tx_idle:            1;
            uint32_t tx_fifo_reset_back: 1;
            uint32_t rx_fifo_reset_back: 1;
            uint32_t reserved3:         29;
        };
        uint32_t val;
    } state;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t date;                                        /**/
} i2s_dev_t;
extern i2s_dev_t I2S0;
extern i2s_dev_t I2S1;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_I2S_STRUCT_H_ */
