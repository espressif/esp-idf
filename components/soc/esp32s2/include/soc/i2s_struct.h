// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct i2s_dev_s {
    uint32_t reserved_0;
    uint32_t reserved_4;
    union {
        struct {
            uint32_t tx_reset:         1;               /*Set this bit to reset transmitter*/
            uint32_t rx_reset:         1;               /*Set this bit to reset receiver*/
            uint32_t tx_fifo_reset:    1;               /*Set this bit to reset txFIFO*/
            uint32_t rx_fifo_reset:    1;               /*Set this bit to reset rxFIFO*/
            uint32_t tx_start:         1;               /*Set this bit to start transmitting data*/
            uint32_t rx_start:         1;               /*Set this bit to start receiving data*/
            uint32_t tx_slave_mod:     1;               /*Set this bit to enable slave transmitter mode*/
            uint32_t rx_slave_mod:     1;               /*Set this bit to enable slave receiver mode*/
            uint32_t tx_right_first:   1;               /*Set this bit to transmit right channel data first*/
            uint32_t rx_right_first:   1;               /*Set this bit to receive right channel data first*/
            uint32_t tx_msb_shift:     1;               /*Set this bit to enable transmitter in Phillips standard mode*/
            uint32_t rx_msb_shift:     1;               /*Set this bit to enable receiver in Phillips standard mode*/
            uint32_t tx_short_sync:    1;               /*Set this bit to enable transmitter in PCM standard mode*/
            uint32_t rx_short_sync:    1;               /*Set this bit to enable receiver in PCM standard mode*/
            uint32_t tx_mono:          1;               /*Set this bit to enable transmitter in mono mode*/
            uint32_t rx_mono:          1;               /*Set this bit to enable receiver  in mono mode*/
            uint32_t tx_msb_right:     1;               /*Set this bit to place right channel data at the MSB in the transmit FIFO.*/
            uint32_t rx_msb_right:     1;               /*Set this bit to place right channel data at the MSB in the receive FIFO.*/
            uint32_t tx_lsb_first_dma: 1;               /*1:the data in DMA/APB transform from low bits*/
            uint32_t rx_lsb_first_dma: 1;               /*1:the data in DMA/APB transform from low bits*/
            uint32_t sig_loopback:     1;               /*Enable signal loopback mode with transmitter module and receiver module sharing the same WS and BCK signals.*/
            uint32_t tx_fifo_reset_st: 1;               /*1:i2s_tx_fifo reset is not ok   0:i2s_tx_fifo_reset is ok*/
            uint32_t rx_fifo_reset_st: 1;               /*1:i2s_rx_fifo_reset is not ok   0:i2s_rx_fifo reset is ok*/
            uint32_t tx_reset_st:      1;               /*1: i2s_tx_reset is not ok   0: i2s_tx_reset is ok*/
            uint32_t tx_dma_equal:     1;               /*1:data in left channel is equal to data in right channel*/
            uint32_t rx_dma_equal:     1;               /*1:data in left channel is equal to data in right channel*/
            uint32_t pre_req_en:       1;               /*set this bit to enable i2s to prepare data earlier*/
            uint32_t tx_big_endian:    1;
            uint32_t rx_big_endian:    1;
            uint32_t rx_reset_st:      1;
            uint32_t reserved30:       2;
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t rx_take_data:          1;          /*The raw interrupt status bit  for the i2s_rx_take_data_int interrupt*/
            uint32_t tx_put_data:           1;          /*The raw interrupt status bit  for the i2s_tx_put_data_int interrupt*/
            uint32_t rx_wfull:              1;          /*The raw interrupt status bit  for the i2s_rx_wfull_int interrupt*/
            uint32_t rx_rempty:             1;          /*The raw interrupt status bit  for the i2s_rx_rempty_int interrupt*/
            uint32_t tx_wfull:              1;          /*The raw interrupt status bit  for the i2s_tx_wfull_int interrupt*/
            uint32_t tx_rempty:             1;          /*The raw interrupt status bit  for the i2s_tx_rempty_int interrupt*/
            uint32_t rx_hung:               1;          /*The raw interrupt status bit  for the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung:               1;          /*The raw interrupt status bit  for the i2s_tx_hung_int interrupt*/
            uint32_t in_done:               1;          /*The raw interrupt status bit  for the i2s_in_done_int interrupt*/
            uint32_t in_suc_eof:            1;          /*The raw interrupt status bit  for the i2s_in_suc_eof_int interrupt*/
            uint32_t in_err_eof:            1;          /*don't use*/
            uint32_t out_done:              1;          /*The raw interrupt status bit  for the i2s_out_done_int interrupt*/
            uint32_t out_eof:               1;          /*The raw interrupt status bit  for the i2s_out_eof_int interrupt*/
            uint32_t in_dscr_err:           1;          /*The raw interrupt status bit  for the i2s_in_dscr_err_int interrupt*/
            uint32_t out_dscr_err:          1;          /*The raw interrupt status bit  for the i2s_out_dscr_err_int interrupt*/
            uint32_t in_dscr_empty:         1;          /*The raw interrupt status bit  for the i2s_in_dscr_empty_int interrupt*/
            uint32_t out_total_eof:         1;          /*The raw interrupt status bit  for the i2s_out_total_eof_int interrupt*/
            uint32_t v_sync:                1;          /*The raw interrupt status bit  for the i2s_v_sync_int interrupt*/
            uint32_t reserved18:           14;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_take_data:         1;           /*The masked interrupt status bit  for the i2s_rx_take_data_int interrupt*/
            uint32_t tx_put_data:          1;           /*The masked interrupt status bit  for the i2s_tx_put_data_int interrupt*/
            uint32_t rx_wfull:             1;           /*The masked interrupt status bit  for the i2s_rx_wfull_int interrupt*/
            uint32_t rx_rempty:            1;           /*The masked interrupt status bit  for the i2s_rx_rempty_int interrupt*/
            uint32_t tx_wfull:             1;           /*The masked interrupt status bit  for the i2s_tx_wfull_int interrupt*/
            uint32_t tx_rempty:            1;           /*The masked interrupt status bit  for the i2s_tx_rempty_int interrupt*/
            uint32_t rx_hung:              1;           /*The masked interrupt status bit  for the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung:              1;           /*The masked interrupt status bit  for the i2s_tx_hung_int interrupt*/
            uint32_t in_done:              1;           /*The masked interrupt status bit  for the i2s_in_done_int interrupt*/
            uint32_t in_suc_eof:           1;           /*The masked interrupt status bit  for the i2s_in_suc_eof_int interrupt*/
            uint32_t in_err_eof:           1;           /*don't use*/
            uint32_t out_done:             1;           /*The masked interrupt status bit  for the i2s_out_done_int interrupt*/
            uint32_t out_eof:              1;           /*The masked interrupt status bit  for the i2s_out_eof_int interrupt*/
            uint32_t in_dscr_err:          1;           /*The masked interrupt status bit  for the i2s_in_dscr_err_int interrupt*/
            uint32_t out_dscr_err:         1;           /*The masked interrupt status bit  for the i2s_out_dscr_err_int interrupt*/
            uint32_t in_dscr_empty:        1;           /*The masked interrupt status bit  for the i2s_in_dscr_empty_int interrupt*/
            uint32_t out_total_eof:        1;           /*The masked interrupt status bit  for the i2s_out_total_eof_int interrupt*/
            uint32_t v_sync:               1;           /*The masked interrupt status bit  for the i2s_v_sync_int  interrupt*/
            uint32_t reserved18:          14;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rx_take_data:          1;          /*The interrupt enable bit  for the i2s_rx_take_data_int interrupt*/
            uint32_t tx_put_data:           1;          /*The interrupt enable bit  for the i2s_tx_put_data_int interrupt*/
            uint32_t rx_wfull:              1;          /*The interrupt enable bit  for the i2s_rx_wfull_int interrupt*/
            uint32_t rx_rempty:             1;          /*The interrupt enable bit  for the i2s_rx_rempty_int interrupt*/
            uint32_t tx_wfull:              1;          /*The interrupt enable bit  for the i2s_tx_wfull_int interrupt*/
            uint32_t tx_rempty:             1;          /*The interrupt enable bit  for the i2s_tx_rempty_int interrupt*/
            uint32_t rx_hung:               1;          /*The interrupt enable bit  for the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung:               1;          /*The interrupt enable bit  for the i2s_tx_hung_int interrupt*/
            uint32_t in_done:               1;          /*The interrupt enable bit  for the i2s_in_done_int interrupt*/
            uint32_t in_suc_eof:            1;          /*The interrupt enable bit  for the i2s_in_suc_eof_int interrupt*/
            uint32_t in_err_eof:            1;          /*don't use*/
            uint32_t out_done:              1;          /*The interrupt enable bit  for the i2s_out_done_int interrupt*/
            uint32_t out_eof:               1;          /*The interrupt enable bit  for the i2s_out_eof_int interrupt*/
            uint32_t in_dscr_err:           1;          /*The interrupt enable bit  for the i2s_in_dscr_err_int interrupt*/
            uint32_t out_dscr_err:          1;          /*The interrupt enable bit  for the i2s_out_dscr_err_int interrupt*/
            uint32_t in_dscr_empty:         1;          /*The interrupt enable bit  for the i2s_in_dscr_empty_int interrupt*/
            uint32_t out_total_eof:         1;          /*The interrupt enable bit  for the i2s_out_total_eof_int interrupt*/
            uint32_t v_sync:                1;          /*The interrupt enable bit  for the i2s_v_sync_int interrupt*/
            uint32_t reserved18:           14;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t take_data:             1;          /*Set this bit to clear the i2s_rx_take_data_int interrupt*/
            uint32_t put_data:              1;          /*Set this bit to clear the i2s_tx_put_data_int interrupt*/
            uint32_t rx_wfull:              1;          /*Set this bit to clear the i2s_rx_wfull_int interrupt*/
            uint32_t rx_rempty:             1;          /*Set this bit to clear the i2s_rx_rempty_int interrupt*/
            uint32_t tx_wfull:              1;          /*Set this bit to clear the i2s_tx_wfull_int interrupt*/
            uint32_t tx_rempty:             1;          /*Set this bit to clear the i2s_tx_rempty_int interrupt*/
            uint32_t rx_hung:               1;          /*Set this bit to clear the i2s_rx_hung_int interrupt*/
            uint32_t tx_hung:               1;          /*Set this bit to clear the i2s_tx_hung_int interrupt*/
            uint32_t in_done:               1;          /*Set this bit to clear the i2s_in_done_int interrupt*/
            uint32_t in_suc_eof:            1;          /*Set this bit to clear the i2s_in_suc_eof_int interrupt*/
            uint32_t in_err_eof:            1;          /*don't use*/
            uint32_t out_done:              1;          /*Set this bit to clear the i2s_out_done_int interrupt*/
            uint32_t out_eof:               1;          /*Set this bit to clear the i2s_out_eof_int interrupt*/
            uint32_t in_dscr_err:           1;          /*Set this bit to clear the i2s_in_dscr_err_int interrupt*/
            uint32_t out_dscr_err:          1;          /*Set this bit to clear the i2s_out_dscr_err_int interrupt*/
            uint32_t in_dscr_empty:         1;          /*Set this bit to clear the i2s_in_dscr_empty_int interrupt*/
            uint32_t out_total_eof:         1;          /*Set this bit to clear the i2s_out_total_eof_int interrupt*/
            uint32_t v_sync:                1;          /*Set this bit to clear the  i2s_v_sync_int interrupt*/
            uint32_t reserved18:           14;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t tx_bck_in_delay:   2;              /*Number of delay cycles for BCK into the transmitter*/
            uint32_t tx_ws_in_delay:    2;              /*Number of delay cycles for WS into the transmitter*/
            uint32_t rx_bck_in_delay:   2;              /*Number of delay cycles for BCK into the receiver*/
            uint32_t rx_ws_in_delay:    2;              /*Number of delay cycles for WS into the receiver*/
            uint32_t rx_sd_in_delay:    2;              /*Number of delay cycles for SD into the receiver*/
            uint32_t tx_bck_out_delay:  2;              /*Number of delay cycles for BCK out of the transmitter*/
            uint32_t tx_ws_out_delay:   2;              /*Number of delay cycles for WS out of the transmitter*/
            uint32_t tx_sd_out_delay:   2;              /*Number of delay cycles for SD out of the transmitter*/
            uint32_t rx_ws_out_delay:   2;              /*Number of delay cycles for WS out of the receiver*/
            uint32_t rx_bck_out_delay:  2;              /*Number of delay cycles for BCK out of the receiver*/
            uint32_t tx_dsync_sw:       1;              /*Set this bit to synchronize signals with the double sync method into the transmitter*/
            uint32_t rx_dsync_sw:       1;              /*Set this bit to synchronize signals with the double sync method into the receiver*/
            uint32_t data_enable_delay: 2;              /*Number of delay cycles for data valid flag.*/
            uint32_t tx_bck_in_inv:     1;              /*Set this bit to invert BCK signal input to the slave transmitter*/
            uint32_t reserved25:        7;
        };
        uint32_t val;
    } timing;
    union {
        struct {
            uint32_t rx_data_num:          6;           /*Threshold of data length in receiver FIFO*/
            uint32_t tx_data_num:          6;           /*Threshold of data length in transmitter FIFO*/
            uint32_t dscr_en:              1;           /*Set this bit to enable I2S DMA mode*/
            uint32_t tx_fifo_mod:          3;           /*Transmitter FIFO mode configuration bits*/
            uint32_t rx_fifo_mod:          3;           /*Receiver FIFO mode configuration bits*/
            uint32_t tx_fifo_mod_force_en: 1;           /*The bit should always be set to 1*/
            uint32_t rx_fifo_mod_force_en: 1;           /*The bit should always be set to 1*/
            uint32_t rx_fifo_sync:         1;           /*force write back rx data to memory*/
            uint32_t rx_24msb_en:          1;           /*Only useful in rx 24bit mode. 1: the high 24 bits are effective in i2s fifo   0: the low 24 bits are effective in i2s fifo*/
            uint32_t tx_24msb_en:          1;           /*Only useful in tx 24bit mode. 1: the high 24 bits are effective in i2s fifo   0: the low 24 bits are effective in i2s fifo*/
            uint32_t reserved24:           8;
        };
        uint32_t val;
    } fifo_conf;
    uint32_t rx_eof_num;                              /*the length of data to be received. It will trigger i2s_in_suc_eof_int.*/
    uint32_t conf_single_data;                        /*the right channel or left channel put out constant value stored in this register according to tx_chan_mod and reg_tx_msb_right*/
    union {
        struct {
            uint32_t tx_chan_mod: 3;                    /*I2S transmitter channel mode configuration bits.*/
            uint32_t rx_chan_mod: 2;                    /*I2S receiver channel mode configuration bits.*/
            uint32_t reserved5:  27;
        };
        uint32_t val;
    } conf_chan;
    union {
        struct {
            uint32_t addr:           20;                /*The address of first outlink descriptor*/
            uint32_t reserved20:      8;
            uint32_t stop:            1;                /*Set this bit to stop outlink descriptor*/
            uint32_t start:           1;                /*Set this bit to start outlink descriptor*/
            uint32_t restart:         1;                /*Set this bit to restart outlink descriptor*/
            uint32_t park:            1;
        };
        uint32_t val;
    } out_link;
    union {
        struct {
            uint32_t addr:          20;                 /*The address of first inlink descriptor*/
            uint32_t reserved20:     8;
            uint32_t stop:           1;                 /*Set this bit to stop inlink descriptor*/
            uint32_t start:          1;                 /*Set this bit to start inlink descriptor*/
            uint32_t restart:        1;                 /*Set this bit to restart inlink descriptor*/
            uint32_t park:           1;
        };
        uint32_t val;
    } in_link;
    uint32_t out_eof_des_addr;                          /*The address of outlink descriptor that produces EOF*/
    uint32_t in_eof_des_addr;                           /*The address of inlink descriptor that produces EOF*/
    uint32_t out_eof_bfr_des_addr;                      /*The address of buffer relative to the outlink descriptor that produces EOF*/
    union {
        struct {
            uint32_t mode:         3;
            uint32_t reserved3:    1;
            uint32_t addr:         2;
            uint32_t reserved6:   26;
        };
        uint32_t val;
    } ahb_test;
    uint32_t in_link_dscr;                               /*The address of current inlink descriptor*/
    uint32_t in_link_dscr_bf0;                           /*The address of next inlink descriptor*/
    uint32_t in_link_dscr_bf1;                           /*The address of next inlink data buffer*/
    uint32_t out_link_dscr;                              /*The address of current outlink descriptor*/
    uint32_t out_link_dscr_bf0;                          /*The address of next outlink descriptor*/
    uint32_t out_link_dscr_bf1;                          /*The address of next outlink data buffer*/
    union {
        struct {
            uint32_t in_rst:             1;             /*Set this bit to reset in dma FSM*/
            uint32_t out_rst:            1;             /*Set this bit to reset out dma FSM*/
            uint32_t ahbm_fifo_rst:      1;             /*Set this bit to reset ahb interface cmdFIFO of DMA*/
            uint32_t ahbm_rst:           1;             /*Set this bit to reset ahb interface of DMA*/
            uint32_t out_loop_test:      1;             /*Set this bit to loop test inlink*/
            uint32_t in_loop_test:       1;             /*Set this bit to loop test outlink*/
            uint32_t out_auto_wrback:    1;             /*Set this bit to enable outlink-written-back automatically when out buffer is transmitted done.*/
            uint32_t out_no_restart_clr: 1;             /*don't use*/
            uint32_t out_eof_mode:       1;             /*DMA out EOF flag generation mode . 1: when dma has popped all data from the FIFO  0:when ahb has pushed all data to the FIFO*/
            uint32_t outdscr_burst_en:   1;             /*DMA outlink descriptor transfer mode configuration bit. 1:  to prepare outlink descriptor with burst mode    0: to prepare outlink descriptor with byte mode*/
            uint32_t indscr_burst_en:    1;             /*DMA inlink descriptor transfer mode configuration bit. 1:  to prepare inlink descriptor with burst mode    0: to prepare inlink descriptor with byte mode*/
            uint32_t out_data_burst_en:  1;             /*Transmitter data transfer mode configuration bit. 1:  to prepare out data with burst mode      0: to prepare out data with byte mode*/
            uint32_t check_owner:        1;             /*Set this bit to enable check owner bit by hardware*/
            uint32_t mem_trans_en:       1;             /*don't use*/
            uint32_t ext_mem_bk_size:    2;             /*DMA access external memory block size. 0: 16 bytes      1: 32 bytes    2:64 bytes      3:reserved*/
            uint32_t reserved16:        16;
        };
        uint32_t val;
    } lc_conf;
    union {
        struct {
            uint32_t wdata:         9;
            uint32_t reserved9:     7;
            uint32_t push:          1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } out_fifo_push;
    union {
        struct {
            uint32_t rdata:       12;
            uint32_t reserved12:   4;
            uint32_t pop:          1;
            uint32_t reserved17:  15;
        };
        uint32_t val;
    } in_fifo_pop;
    union {
        struct {
            uint32_t dscr_addr:        18;
            uint32_t out_dscr_state:    2;
            uint32_t out_state:         3;
            uint32_t cnt:               7;
            uint32_t out_full:          1;
            uint32_t out_empty:         1;              /*DMA transmitter status register*/
        };
        uint32_t val;
    } lc_state0;
    union {
        struct {
            uint32_t dscr_addr:       18;
            uint32_t in_dscr_state:    2;
            uint32_t in_state:         3;
            uint32_t cnt_debug:        7;
            uint32_t in_full:          1;
            uint32_t in_empty:         1;               /*DMA receiver status register*/
        };
        uint32_t val;
    } lc_state1;
    union {
        struct {
            uint32_t fifo_timeout:          8;          /*the i2s_tx_hung_int interrupt or the i2s_rx_hung_int interrupt will be triggered when fifo hung counter is equal to this value*/
            uint32_t fifo_timeout_shift:    3;          /*The bits are used to scale tick counter threshold. The tick counter is reset when counter value >= 88000/2^i2s_lc_fifo_timeout_shift*/
            uint32_t fifo_timeout_ena:      1;          /*The enable bit for FIFO timeout*/
            uint32_t reserved12:           20;
        };
        uint32_t val;
    } lc_hung_conf;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    union {
        struct {
            uint32_t tx_pcm_conf:    3;                 /*Compress/Decompress module configuration bits. 0: decompress transmitted data  1:compress transmitted data*/
            uint32_t tx_pcm_bypass:  1;                 /*Set this bit to bypass  Compress/Decompress module for transmitted data.*/
            uint32_t rx_pcm_conf:    3;                 /*Compress/Decompress module configuration bits. 0: decompress received data  1:compress received data*/
            uint32_t rx_pcm_bypass:  1;                 /*Set this bit to bypass Compress/Decompress module for received data.*/
            uint32_t tx_stop_en:     1;                 /*Set this bit to stop disable output BCK signal and WS signal when tx FIFO is emtpy*/
            uint32_t tx_zeros_rm_en: 1;                 /*don't use*/
            uint32_t reserved10:    22;
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t fifo_force_pd:    1;               /*Force FIFO power-down*/
            uint32_t fifo_force_pu:    1;               /*Force FIFO power-up*/
            uint32_t plc_mem_force_pd: 1;
            uint32_t plc_mem_force_pu: 1;
            uint32_t dma_ram_force_pd: 1;
            uint32_t dma_ram_force_pu: 1;
            uint32_t dma_ram_clk_fo:   1;
            uint32_t reserved7:       25;
        };
        uint32_t val;
    } pd_conf;
    union {
        struct {
            uint32_t camera_en:             1;          /*Set this bit to enable camera mode*/
            uint32_t lcd_tx_wrx2_en:        1;          /*LCD WR double for one datum.*/
            uint32_t lcd_tx_sdx2_en:        1;          /*Set this bit to duplicate data pairs (Frame Form 2) in LCD mode.*/
            uint32_t data_enable_test_en:   1;          /*for debug camera mode enable*/
            uint32_t data_enable:           1;          /*for debug camera mode enable*/
            uint32_t lcd_en:                1;          /*Set this bit to enable LCD mode*/
            uint32_t ext_adc_start_en:      1;          /*Set this bit to enable the function that ADC mode is triggered by external signal.*/
            uint32_t inter_valid_en:        1;          /*Set this bit to enable camera internal valid*/
            uint32_t cam_sync_fifo_reset:   1;          /*Set this bit to reset cam_sync_fifo*/
            uint32_t cam_clk_loopback:      1;          /*Set this bit to loopback cam_clk from i2s_rx*/
            uint32_t i_v_sync_filter_en:    1;
            uint32_t i_v_sync_filter_thres: 3;
            uint32_t reserved14:           18;
        };
        uint32_t val;
    } conf2;
    union {
        struct {
            uint32_t clkm_div_num: 8;                   /*Integral I2S clock divider value*/
            uint32_t clkm_div_b:   6;                   /*Fractional clock divider numerator value*/
            uint32_t clkm_div_a:   6;                   /*Fractional clock divider denominator value*/
            uint32_t clk_en:       1;                   /*Set this bit to enable clk gate*/
            uint32_t clk_sel:      2;                   /*Set this bit to enable clk_apll*/
            uint32_t reserved23:   9;
        };
        uint32_t val;
    } clkm_conf;
    union {
        struct {
            uint32_t tx_bck_div_num: 6;                 /*Bit clock configuration bits in transmitter mode.*/
            uint32_t rx_bck_div_num: 6;                 /*Bit clock configuration bits in receiver mode.*/
            uint32_t tx_bits_mod:    6;                 /*Set the bits to configure bit length of I2S transmitter channel.*/
            uint32_t rx_bits_mod:    6;                 /*Set the bits to configure bit length of I2S receiver channel.*/
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } sample_rate_conf;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    union {
        struct {
            uint32_t tx_idle:    1;
            uint32_t reserved1: 31;                     /*1: i2s_tx is idle state*/
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
    uint32_t date;                                      /**/
} i2s_dev_t;
extern i2s_dev_t I2S0;

_Static_assert(sizeof(i2s_dev_t)==0x100, "invalid i2s_dev_t size");

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_I2S_STRUCT_H_ */
