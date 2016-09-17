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
typedef struct {
    volatile uint32_t reserved_0;
    volatile uint32_t reserved_4;
    union {
        struct {
            volatile uint32_t tx_reset:       1;
            volatile uint32_t rx_reset:       1;
            volatile uint32_t tx_fifo_reset:  1;
            volatile uint32_t rx_fifo_reset:  1;
            volatile uint32_t tx_start:       1;
            volatile uint32_t rx_start:       1;
            volatile uint32_t tx_slave_mod:   1;
            volatile uint32_t rx_slave_mod:   1;
            volatile uint32_t tx_right_first: 1;
            volatile uint32_t rx_right_first: 1;
            volatile uint32_t tx_msb_shift:   1;
            volatile uint32_t rx_msb_shift:   1;
            volatile uint32_t tx_short_sync:  1;
            volatile uint32_t rx_short_sync:  1;
            volatile uint32_t tx_mono:        1;
            volatile uint32_t rx_mono:        1;
            volatile uint32_t tx_msb_right:   1;
            volatile uint32_t rx_msb_right:   1;
            volatile uint32_t sig_loopback:   1;
            volatile uint32_t reserved19:    13;
        };
        volatile uint32_t val;
    }conf;
    union {
        struct {
            volatile uint32_t rx_take_data_int_raw:  1;
            volatile uint32_t tx_put_data_int_raw:   1;
            volatile uint32_t rx_wfull_int_raw:      1;
            volatile uint32_t rx_rempty_int_raw:     1;
            volatile uint32_t tx_wfull_int_raw:      1;
            volatile uint32_t tx_rempty_int_raw:     1;
            volatile uint32_t rx_hung_int_raw:       1;
            volatile uint32_t tx_hung_int_raw:       1;
            volatile uint32_t in_done_int_raw:       1;
            volatile uint32_t in_suc_eof_int_raw:    1;
            volatile uint32_t in_err_eof_int_raw:    1;
            volatile uint32_t out_done_int_raw:      1;
            volatile uint32_t out_eof_int_raw:       1;
            volatile uint32_t in_dscr_err_int_raw:   1;
            volatile uint32_t out_dscr_err_int_raw:  1;
            volatile uint32_t in_dscr_empty_int_raw: 1;
            volatile uint32_t out_total_eof_int_raw: 1;
            volatile uint32_t reserved17:           15;
        };
        volatile uint32_t val;
    }int_raw;
    union {
        struct {
            volatile uint32_t rx_take_data_int_st:  1;
            volatile uint32_t tx_put_data_int_st:   1;
            volatile uint32_t rx_wfull_int_st:      1;
            volatile uint32_t rx_rempty_int_st:     1;
            volatile uint32_t tx_wfull_int_st:      1;
            volatile uint32_t tx_rempty_int_st:     1;
            volatile uint32_t rx_hung_int_st:       1;
            volatile uint32_t tx_hung_int_st:       1;
            volatile uint32_t in_done_int_st:       1;
            volatile uint32_t in_suc_eof_int_st:    1;
            volatile uint32_t in_err_eof_int_st:    1;
            volatile uint32_t out_done_int_st:      1;
            volatile uint32_t out_eof_int_st:       1;
            volatile uint32_t in_dscr_err_int_st:   1;
            volatile uint32_t out_dscr_err_int_st:  1;
            volatile uint32_t in_dscr_empty_int_st: 1;
            volatile uint32_t out_total_eof_int_st: 1;
            volatile uint32_t reserved17:          15;
        };
        volatile uint32_t val;
    }int_st;
    union {
        struct {
            volatile uint32_t rx_take_data_int_ena:  1;
            volatile uint32_t tx_put_data_int_ena:   1;
            volatile uint32_t rx_wfull_int_ena:      1;
            volatile uint32_t rx_rempty_int_ena:     1;
            volatile uint32_t tx_wfull_int_ena:      1;
            volatile uint32_t tx_rempty_int_ena:     1;
            volatile uint32_t rx_hung_int_ena:       1;
            volatile uint32_t tx_hung_int_ena:       1;
            volatile uint32_t in_done_int_ena:       1;
            volatile uint32_t in_suc_eof_int_ena:    1;
            volatile uint32_t in_err_eof_int_ena:    1;
            volatile uint32_t out_done_int_ena:      1;
            volatile uint32_t out_eof_int_ena:       1;
            volatile uint32_t in_dscr_err_int_ena:   1;
            volatile uint32_t out_dscr_err_int_ena:  1;
            volatile uint32_t in_dscr_empty_int_ena: 1;
            volatile uint32_t out_total_eof_int_ena: 1;
            volatile uint32_t reserved17:           15;
        };
        volatile uint32_t val;
    }int_ena;
    union {
        struct {
            volatile uint32_t take_data_int_clr:     1;
            volatile uint32_t put_data_int_clr:      1;
            volatile uint32_t rx_wfull_int_clr:      1;
            volatile uint32_t rx_rempty_int_clr:     1;
            volatile uint32_t tx_wfull_int_clr:      1;
            volatile uint32_t tx_rempty_int_clr:     1;
            volatile uint32_t rx_hung_int_clr:       1;
            volatile uint32_t tx_hung_int_clr:       1;
            volatile uint32_t in_done_int_clr:       1;
            volatile uint32_t in_suc_eof_int_clr:    1;
            volatile uint32_t in_err_eof_int_clr:    1;
            volatile uint32_t out_done_int_clr:      1;
            volatile uint32_t out_eof_int_clr:       1;
            volatile uint32_t in_dscr_err_int_clr:   1;
            volatile uint32_t out_dscr_err_int_clr:  1;
            volatile uint32_t in_dscr_empty_int_clr: 1;
            volatile uint32_t out_total_eof_int_clr: 1;
            volatile uint32_t reserved17:           15;
        };
        volatile uint32_t val;
    }int_clr;
    union {
        struct {
            volatile uint32_t tx_bck_in_delay:   2;
            volatile uint32_t tx_ws_in_delay:    2;
            volatile uint32_t rx_bck_in_delay:   2;
            volatile uint32_t rx_ws_in_delay:    2;
            volatile uint32_t rx_sd_in_delay:    2;
            volatile uint32_t tx_bck_out_delay:  2;
            volatile uint32_t tx_ws_out_delay:   2;
            volatile uint32_t tx_sd_out_delay:   2;
            volatile uint32_t rx_ws_out_delay:   2;
            volatile uint32_t rx_bck_out_delay:  2;
            volatile uint32_t tx_dsync_sw:       1;
            volatile uint32_t rx_dsync_sw:       1;
            volatile uint32_t data_enable_delay: 2;
            volatile uint32_t tx_bck_in_inv:     1;
            volatile uint32_t reserved25:        7;
        };
        volatile uint32_t val;
    }timing;
    union {
        struct {
            volatile uint32_t rx_data_num:          6;
            volatile uint32_t tx_data_num:          6;
            volatile uint32_t dscr_en:              1;
            volatile uint32_t tx_fifo_mod:          3;
            volatile uint32_t rx_fifo_mod:          3;
            volatile uint32_t tx_fifo_mod_force_en: 1;
            volatile uint32_t rx_fifo_mod_force_en: 1;
            volatile uint32_t reserved21:          11;
        };
        volatile uint32_t val;
    }fifo_conf;
    volatile uint32_t rx_eof_num;
    volatile uint32_t conf_single_data;
    union {
        struct {
            volatile uint32_t tx_chan_mod: 3;
            volatile uint32_t rx_chan_mod: 2;
            volatile uint32_t reserved5:  27;
        };
        volatile uint32_t val;
    }conf_chan;
    union {
        struct {
            volatile uint32_t outlink_addr:   20;
            volatile uint32_t reserved20:      8;
            volatile uint32_t outlink_stop:    1;
            volatile uint32_t outlink_start:   1;
            volatile uint32_t outlink_restart: 1;
            volatile uint32_t outlink_park:    1;
        };
        volatile uint32_t val;
    }out_link;
    union {
        struct {
            volatile uint32_t inlink_addr:   20;
            volatile uint32_t reserved20:     8;
            volatile uint32_t inlink_stop:    1;
            volatile uint32_t inlink_start:   1;
            volatile uint32_t inlink_restart: 1;
            volatile uint32_t inlink_park:    1;
        };
        volatile uint32_t val;
    }in_link;
    volatile uint32_t out_eof_des_addr;
    volatile uint32_t in_eof_des_addr;
    volatile uint32_t out_eof_bfr_des_addr;
    union {
        struct {
            volatile uint32_t ahb_testmode: 3;
            volatile uint32_t reserved3:    1;
            volatile uint32_t ahb_testaddr: 2;
            volatile uint32_t reserved6:   26;
        };
        volatile uint32_t val;
    }ahb_test;
    volatile uint32_t in_link_dscr;
    volatile uint32_t in_link_dscr_bf0;
    volatile uint32_t in_link_dscr_bf1;
    volatile uint32_t out_link_dscr;
    volatile uint32_t out_link_dscr_bf0;
    volatile uint32_t out_link_dscr_bf1;
    union {
        struct {
            volatile uint32_t in_rst:             1;
            volatile uint32_t out_rst:            1;
            volatile uint32_t ahbm_fifo_rst:      1;
            volatile uint32_t ahbm_rst:           1;
            volatile uint32_t out_loop_test:      1;
            volatile uint32_t in_loop_test:       1;
            volatile uint32_t out_auto_wrback:    1;
            volatile uint32_t out_no_restart_clr: 1;
            volatile uint32_t out_eof_mode:       1;
            volatile uint32_t outdscr_burst_en:   1;
            volatile uint32_t indscr_burst_en:    1;
            volatile uint32_t out_data_burst_en:  1;
            volatile uint32_t check_owner:        1;
            volatile uint32_t mem_trans_en:       1;
            volatile uint32_t reserved14:        18;
        };
        volatile uint32_t val;
    }lc_conf;
    union {
        struct {
            volatile uint32_t out_fifo_wdata: 9;
            volatile uint32_t reserved9:      7;
            volatile uint32_t out_fifo_push:  1;
            volatile uint32_t reserved17:    15;
        };
        volatile uint32_t val;
    }out_fifo_push;
    union {
        struct {
            volatile uint32_t in_fifo_rdata:12;
            volatile uint32_t reserved12:    4;
            volatile uint32_t in_fifo_pop:   1;
            volatile uint32_t reserved17:   15;
        };
        volatile uint32_t val;
    }in_fifo_pop;
    volatile uint32_t lc_state0;
    volatile uint32_t lc_state1;
    union {
        struct {
            volatile uint32_t lc_fifo_timeout:       8;
            volatile uint32_t lc_fifo_timeout_shift: 3;
            volatile uint32_t lc_fifo_timeout_ena:   1;
            volatile uint32_t reserved12:           20;
        };
        volatile uint32_t val;
    }lc_hung_conf;
    volatile uint32_t reserved_78;
    volatile uint32_t reserved_7c;
    union {
        struct {
            volatile uint32_t cvsd_y_max:16;
            volatile uint32_t cvsd_y_min:16;
        };
        volatile uint32_t val;
    }cvsd_conf0;
    union {
        struct {
            volatile uint32_t cvsd_sigma_max:16;
            volatile uint32_t cvsd_sigma_min:16;
        };
        volatile uint32_t val;
    }cvsd_conf1;
    union {
        struct {
            volatile uint32_t cvsd_k:     3;
            volatile uint32_t cvsd_j:     3;
            volatile uint32_t cvsd_beta: 10;
            volatile uint32_t cvsd_h:     3;
            volatile uint32_t reserved19:13;
        };
        volatile uint32_t val;
    }cvsd_conf2;
    union {
        struct {
            volatile uint32_t good_pack_max:    6;
            volatile uint32_t n_err_seg:        3;
            volatile uint32_t shift_rate:       3;
            volatile uint32_t max_slide_sample: 8;
            volatile uint32_t pack_len_8k:      5;
            volatile uint32_t n_min_err:        3;
            volatile uint32_t reserved28:       4;
        };
        volatile uint32_t val;
    }plc_conf0;
    union {
        struct {
            volatile uint32_t bad_cef_atten_para:       8;
            volatile uint32_t bad_cef_atten_para_shift: 4;
            volatile uint32_t bad_ola_win2_para_shift:  4;
            volatile uint32_t bad_ola_win2_para:        8;
            volatile uint32_t slide_win_len:            8;
        };
        volatile uint32_t val;
    }plc_conf1;
    union {
        struct {
            volatile uint32_t cvsd_seg_mod: 2;
            volatile uint32_t min_period:   5;
            volatile uint32_t reserved7:   25;
        };
        volatile uint32_t val;
    }plc_conf2;
    union {
        struct {
            volatile uint32_t esco_en:                1;
            volatile uint32_t esco_chan_mod:          1;
            volatile uint32_t esco_cvsd_dec_pack_err: 1;
            volatile uint32_t esco_cvsd_pack_len_8k:  5;
            volatile uint32_t esco_cvsd_inf_en:       1;
            volatile uint32_t cvsd_dec_start:         1;
            volatile uint32_t cvsd_dec_reset:         1;
            volatile uint32_t plc_en:                 1;
            volatile uint32_t plc2dma_en:             1;
            volatile uint32_t reserved13:            19;
        };
        volatile uint32_t val;
    }esco_conf0;
    union {
        struct {
            volatile uint32_t sco_with_en:    1;
            volatile uint32_t sco_no_en:      1;
            volatile uint32_t cvsd_enc_start: 1;
            volatile uint32_t cvsd_enc_reset: 1;
            volatile uint32_t reserved4:     28;
        };
        volatile uint32_t val;
    }sco_conf0;
    union {
        struct {
            volatile uint32_t tx_pcm_conf:    3;
            volatile uint32_t tx_pcm_bypass:  1;
            volatile uint32_t rx_pcm_conf:    3;
            volatile uint32_t rx_pcm_bypass:  1;
            volatile uint32_t tx_stop_en:     1;
            volatile uint32_t tx_zeros_rm_en: 1;
            volatile uint32_t reserved10:    22;
        };
        volatile uint32_t val;
    }conf1;
    union {
        struct {
            volatile uint32_t fifo_force_pd:    1;
            volatile uint32_t fifo_force_pu:    1;
            volatile uint32_t plc_mem_force_pd: 1;
            volatile uint32_t plc_mem_force_pu: 1;
            volatile uint32_t reserved4:       28;
        };
        volatile uint32_t val;
    }pd_conf;
    union {
        struct {
            volatile uint32_t camera_en:           1;
            volatile uint32_t lcd_tx_wrx2_en:      1;
            volatile uint32_t lcd_tx_sdx2_en:      1;
            volatile uint32_t data_enable_test_en: 1;
            volatile uint32_t data_enable:         1;
            volatile uint32_t lcd_en:              1;
            volatile uint32_t ext_adc_start_en:    1;
            volatile uint32_t inter_valid_en:      1;
            volatile uint32_t reserved8:          24;
        };
        volatile uint32_t val;
    }conf2;
    union {
        struct {
            volatile uint32_t clkm_div_num: 8;
            volatile uint32_t clkm_div_b:   6;
            volatile uint32_t clkm_div_a:   6;
            volatile uint32_t clk_en:       1;
            volatile uint32_t clka_ena:     1;
            volatile uint32_t reserved22:  10;
        };
        volatile uint32_t val;
    }clkm_conf;
    union {
        struct {
            volatile uint32_t tx_bck_div_num: 6;
            volatile uint32_t rx_bck_div_num: 6;
            volatile uint32_t tx_bits_mod:    6;
            volatile uint32_t rx_bits_mod:    6;
            volatile uint32_t reserved24:     8;
        };
        volatile uint32_t val;
    }sample_rate_conf;
    union {
        struct {
            volatile uint32_t tx_pdm_en:                  1;
            volatile uint32_t rx_pdm_en:                  1;
            volatile uint32_t pcm2pdm_conv_en:            1;
            volatile uint32_t pdm2pcm_conv_en:            1;
            volatile uint32_t tx_pdm_sinc_osr2:           4;
            volatile uint32_t tx_pdm_prescale:            8;
            volatile uint32_t tx_pdm_hp_in_shift:         2;
            volatile uint32_t tx_pdm_lp_in_shift:         2;
            volatile uint32_t tx_pdm_sinc_in_shift:       2;
            volatile uint32_t tx_pdm_sigmadelta_in_shift: 2;
            volatile uint32_t rx_pdm_sinc_dsr_16_en:      1;
            volatile uint32_t tx_pdm_hp_bypass:           1;
            volatile uint32_t reserved26:                 6;
        };
        volatile uint32_t val;
    }pdm_conf;
    union {
        struct {
            volatile uint32_t tx_pdm_fs: 10;
            volatile uint32_t tx_pdm_fp: 10;
            volatile uint32_t reserved20:12;
        };
        volatile uint32_t val;
    }pdm_freq_conf;
    union {
        struct {
            volatile uint32_t tx_idle:            1;
            volatile uint32_t tx_fifo_reset_back: 1;
            volatile uint32_t rx_fifo_reset_back: 1;
            volatile uint32_t reserved3:         29;
        };
        volatile uint32_t val;
    }state;
    volatile uint32_t reserved_c0;
    volatile uint32_t reserved_c4;
    volatile uint32_t reserved_c8;
    volatile uint32_t reserved_cc;
    volatile uint32_t reserved_d0;
    volatile uint32_t reserved_d4;
    volatile uint32_t reserved_d8;
    volatile uint32_t reserved_dc;
    volatile uint32_t reserved_e0;
    volatile uint32_t reserved_e4;
    volatile uint32_t reserved_e8;
    volatile uint32_t reserved_ec;
    volatile uint32_t reserved_f0;
    volatile uint32_t reserved_f4;
    volatile uint32_t reserved_f8;
    volatile uint32_t date;                                        /**/
} i2s_dev_t;
extern volatile i2s_dev_t I2S0;
extern volatile i2s_dev_t I2S1;

#endif  /* _SOC_I2S_STRUCT_H_ */
