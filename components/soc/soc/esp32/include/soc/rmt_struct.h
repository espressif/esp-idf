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
#ifndef _SOC_RMT_STRUCT_H_
#define _SOC_RMT_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct rmt_dev_s {
    uint32_t data_ch[8];                                /*The R/W ram address for channel0-7 by apb fifo access.
                                                        Note that in some circumstances, data read from the FIFO may get lost. As RMT memory area accesses using the RMTMEM method do not have this issue
                                                        and provide all the functionality that the FIFO register has, it is encouraged to use that instead.*/
    struct {
        union {
            struct {
                uint32_t div_cnt:        8;             /*This register is used to configure the  frequency divider's factor in channel0-7.*/
                uint32_t idle_thres:    16;             /*In receive mode when no edge is detected on the input signal for longer than reg_idle_thres_ch0 then the receive process is done.*/
                uint32_t mem_size:       4;             /*This register is used to configure the the amount of memory blocks allocated to channel0-7.*/
                uint32_t carrier_en:     1;             /*This is the carrier modulation enable control bit for channel0-7.*/
                uint32_t carrier_out_lv: 1;             /*This bit is used to configure the way carrier wave is modulated for  channel0-7.1'b1:transmit on low output level  1'b0:transmit  on high output level.*/
                uint32_t mem_pd:         1;             /*This bit is used to reduce power consumed by memory. 1:memory is in low power state.*/
                uint32_t clk_en:         1;             /*This bit  is used  to control clock.when software configure RMT internal registers  it controls the register clock.*/
            };
            uint32_t val;
        } conf0;
        union {
            struct {
                uint32_t tx_start:        1;            /*Set this bit to start sending data for channel0-7.*/
                uint32_t rx_en:           1;            /*Set this bit to enable receiving data for channel0-7.*/
                uint32_t mem_wr_rst:      1;            /*Set this bit to reset write ram address for channel0-7 by receiver access.*/
                uint32_t mem_rd_rst:      1;            /*Set this bit to reset read ram address for channel0-7 by transmitter access.*/
                uint32_t apb_mem_rst:     1;            /*Set this bit to reset W/R ram address for channel0-7 by apb fifo access (using fifo is discouraged, please see the note above at data_ch[] item)*/
                uint32_t mem_owner:       1;            /*This is the mark of channel0-7's ram usage right.1'b1：receiver uses the ram  0：transmitter uses the ram*/
                uint32_t tx_conti_mode:   1;            /*Set this bit to continue sending  from the first data to the last data in channel0-7 again and again.*/
                uint32_t rx_filter_en:    1;            /*This is the receive filter enable bit for channel0-7.*/
                uint32_t rx_filter_thres: 8;            /*in receive mode  channel0-7 ignore input pulse when the pulse width is smaller then this value.*/
                uint32_t ref_cnt_rst:     1;            /*This bit is used to reset divider in channel0-7.*/
                uint32_t ref_always_on:   1;            /*This bit is used to select base clock. 1'b1:clk_apb  1'b0:clk_ref*/
                uint32_t idle_out_lv:     1;            /*This bit configures the output signal's level for channel0-7 in IDLE state.*/
                uint32_t idle_out_en:     1;            /*This is the output enable control bit for channel0-7 in IDLE state.*/
                uint32_t reserved20:     12;
            };
            uint32_t val;
        } conf1;
    } conf_ch[8];
    uint32_t status_ch[8];                              /*The status for channel0-7*/
    uint32_t apb_mem_addr_ch[8];                        /*The ram relative address in channel0-7 by apb fifo access (using fifo is discouraged, please see the note above at data_ch[] item)*/
    union {
        struct {
            uint32_t ch0_tx_end:       1;               /*The interrupt raw bit for channel 0 turns to high level when the transmit process is done.*/
            uint32_t ch0_rx_end:       1;               /*The interrupt raw bit for channel 0 turns to high level when the receive process is done.*/
            uint32_t ch0_err:          1;               /*The interrupt raw bit for channel 0 turns to high level when channel 0 detects some errors.*/
            uint32_t ch1_tx_end:       1;               /*The interrupt raw bit for channel 1 turns to high level when the transmit process is done.*/
            uint32_t ch1_rx_end:       1;               /*The interrupt raw bit for channel 1 turns to high level when the receive process is done.*/
            uint32_t ch1_err:          1;               /*The interrupt raw bit for channel 1 turns to high level when channel 1 detects some errors.*/
            uint32_t ch2_tx_end:       1;               /*The interrupt raw bit for channel 2 turns to high level when the transmit process is done.*/
            uint32_t ch2_rx_end:       1;               /*The interrupt raw bit for channel 2 turns to high level when the receive process is done.*/
            uint32_t ch2_err:          1;               /*The interrupt raw bit for channel 2 turns to high level when channel 2 detects some errors.*/
            uint32_t ch3_tx_end:       1;               /*The interrupt raw bit for channel 3 turns to high level when the transmit process is done.*/
            uint32_t ch3_rx_end:       1;               /*The interrupt raw bit for channel 3 turns to high level when the receive process is done.*/
            uint32_t ch3_err:          1;               /*The interrupt raw bit for channel 3 turns to high level when channel 3 detects some errors.*/
            uint32_t ch4_tx_end:       1;               /*The interrupt raw bit for channel 4 turns to high level when the transmit process is done.*/
            uint32_t ch4_rx_end:       1;               /*The interrupt raw bit for channel 4 turns to high level when the receive process is done.*/
            uint32_t ch4_err:          1;               /*The interrupt raw bit for channel 4 turns to high level when channel 4 detects some errors.*/
            uint32_t ch5_tx_end:       1;               /*The interrupt raw bit for channel 5 turns to high level when the transmit process is done.*/
            uint32_t ch5_rx_end:       1;               /*The interrupt raw bit for channel 5 turns to high level when the receive process is done.*/
            uint32_t ch5_err:          1;               /*The interrupt raw bit for channel 5 turns to high level when channel 5 detects some errors.*/
            uint32_t ch6_tx_end:       1;               /*The interrupt raw bit for channel 6 turns to high level when the transmit process is done.*/
            uint32_t ch6_rx_end:       1;               /*The interrupt raw bit for channel 6 turns to high level when the receive process is done.*/
            uint32_t ch6_err:          1;               /*The interrupt raw bit for channel 6 turns to high level when channel 6 detects some errors.*/
            uint32_t ch7_tx_end:       1;               /*The interrupt raw bit for channel 7 turns to high level when the transmit process is done.*/
            uint32_t ch7_rx_end:       1;               /*The interrupt raw bit for channel 7 turns to high level when the receive process is done.*/
            uint32_t ch7_err:          1;               /*The interrupt raw bit for channel 7 turns to high level when channel 7 detects some errors.*/
            uint32_t ch0_tx_thr_event: 1;               /*The interrupt raw bit for channel 0 turns to high level when transmitter in channel0  have send data more than  reg_rmt_tx_lim_ch0  after detecting this interrupt  software can updata the old data with new data.*/
            uint32_t ch1_tx_thr_event: 1;               /*The interrupt raw bit for channel 1 turns to high level when transmitter in channel1  have send data more than  reg_rmt_tx_lim_ch1  after detecting this interrupt  software can updata the old data with new data.*/
            uint32_t ch2_tx_thr_event: 1;               /*The interrupt raw bit for channel 2 turns to high level when transmitter in channel2  have send data more than  reg_rmt_tx_lim_ch2  after detecting this interrupt  software can updata the old data with new data.*/
            uint32_t ch3_tx_thr_event: 1;               /*The interrupt raw bit for channel 3 turns to high level when transmitter in channel3  have send data more than  reg_rmt_tx_lim_ch3  after detecting this interrupt  software can updata the old data with new data.*/
            uint32_t ch4_tx_thr_event: 1;               /*The interrupt raw bit for channel 4 turns to high level when transmitter in channel4  have send data more than  reg_rmt_tx_lim_ch4  after detecting this interrupt  software can updata the old data with new data.*/
            uint32_t ch5_tx_thr_event: 1;               /*The interrupt raw bit for channel 5 turns to high level when transmitter in channel5  have send data more than  reg_rmt_tx_lim_ch5  after detecting this interrupt  software can updata the old data with new data.*/
            uint32_t ch6_tx_thr_event: 1;               /*The interrupt raw bit for channel 6 turns to high level when transmitter in channel6  have send data more than  reg_rmt_tx_lim_ch6  after detecting this interrupt  software can updata the old data with new data.*/
            uint32_t ch7_tx_thr_event: 1;               /*The interrupt raw bit for channel 7 turns to high level when transmitter in channel7  have send data more than  reg_rmt_tx_lim_ch7  after detecting this interrupt  software can updata the old data with new data.*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t ch0_tx_end:       1;                /*The interrupt  state bit for channel 0's mt_ch0_tx_end_int_raw when mt_ch0_tx_end_int_ena is set to 0.*/
            uint32_t ch0_rx_end:       1;                /*The interrupt  state bit for channel 0's rmt_ch0_rx_end_int_raw when  rmt_ch0_rx_end_int_ena is set to 0.*/
            uint32_t ch0_err:          1;                /*The interrupt  state bit for channel 0's rmt_ch0_err_int_raw when  rmt_ch0_err_int_ena is set to 0.*/
            uint32_t ch1_tx_end:       1;                /*The interrupt  state bit for channel 1's mt_ch1_tx_end_int_raw when mt_ch1_tx_end_int_ena is set to 1.*/
            uint32_t ch1_rx_end:       1;                /*The interrupt  state bit for channel 1's rmt_ch1_rx_end_int_raw when  rmt_ch1_rx_end_int_ena is set to 1.*/
            uint32_t ch1_err:          1;                /*The interrupt  state bit for channel 1's rmt_ch1_err_int_raw when  rmt_ch1_err_int_ena is set to 1.*/
            uint32_t ch2_tx_end:       1;                /*The interrupt  state bit for channel 2's mt_ch2_tx_end_int_raw when mt_ch2_tx_end_int_ena is set to 1.*/
            uint32_t ch2_rx_end:       1;                /*The interrupt  state bit for channel 2's rmt_ch2_rx_end_int_raw when  rmt_ch2_rx_end_int_ena is set to 1.*/
            uint32_t ch2_err:          1;                /*The interrupt  state bit for channel 2's rmt_ch2_err_int_raw when  rmt_ch2_err_int_ena is set to 1.*/
            uint32_t ch3_tx_end:       1;                /*The interrupt  state bit for channel 3's mt_ch3_tx_end_int_raw when mt_ch3_tx_end_int_ena is set to 1.*/
            uint32_t ch3_rx_end:       1;                /*The interrupt  state bit for channel 3's rmt_ch3_rx_end_int_raw when  rmt_ch3_rx_end_int_ena is set to 1.*/
            uint32_t ch3_err:          1;                /*The interrupt  state bit for channel 3's rmt_ch3_err_int_raw when  rmt_ch3_err_int_ena is set to 1.*/
            uint32_t ch4_tx_end:       1;                /*The interrupt  state bit for channel 4's mt_ch4_tx_end_int_raw when mt_ch4_tx_end_int_ena is set to 1.*/
            uint32_t ch4_rx_end:       1;                /*The interrupt  state bit for channel 4's rmt_ch4_rx_end_int_raw when  rmt_ch4_rx_end_int_ena is set to 1.*/
            uint32_t ch4_err:          1;                /*The interrupt  state bit for channel 4's rmt_ch4_err_int_raw when  rmt_ch4_err_int_ena is set to 1.*/
            uint32_t ch5_tx_end:       1;                /*The interrupt  state bit for channel 5's mt_ch5_tx_end_int_raw when mt_ch5_tx_end_int_ena is set to 1.*/
            uint32_t ch5_rx_end:       1;                /*The interrupt  state bit for channel 5's rmt_ch5_rx_end_int_raw when  rmt_ch5_rx_end_int_ena is set to 1.*/
            uint32_t ch5_err:          1;                /*The interrupt  state bit for channel 5's rmt_ch5_err_int_raw when  rmt_ch5_err_int_ena is set to 1.*/
            uint32_t ch6_tx_end:       1;                /*The interrupt  state bit for channel 6's mt_ch6_tx_end_int_raw when mt_ch6_tx_end_int_ena is set to 1.*/
            uint32_t ch6_rx_end:       1;                /*The interrupt  state bit for channel 6's rmt_ch6_rx_end_int_raw when  rmt_ch6_rx_end_int_ena is set to 1.*/
            uint32_t ch6_err:          1;                /*The interrupt  state bit for channel 6's rmt_ch6_err_int_raw when  rmt_ch6_err_int_ena is set to 1.*/
            uint32_t ch7_tx_end:       1;                /*The interrupt  state bit for channel 7's mt_ch7_tx_end_int_raw when mt_ch7_tx_end_int_ena is set to 1.*/
            uint32_t ch7_rx_end:       1;                /*The interrupt  state bit for channel 7's rmt_ch7_rx_end_int_raw when  rmt_ch7_rx_end_int_ena is set to 1.*/
            uint32_t ch7_err:          1;                /*The interrupt  state bit for channel 7's rmt_ch7_err_int_raw when  rmt_ch7_err_int_ena is set to 1.*/
            uint32_t ch0_tx_thr_event: 1;                /*The interrupt state bit  for channel 0's rmt_ch0_tx_thr_event_int_raw when mt_ch0_tx_thr_event_int_ena is set to 1.*/
            uint32_t ch1_tx_thr_event: 1;                /*The interrupt state bit  for channel 1's rmt_ch1_tx_thr_event_int_raw when mt_ch1_tx_thr_event_int_ena is set to 1.*/
            uint32_t ch2_tx_thr_event: 1;                /*The interrupt state bit  for channel 2's rmt_ch2_tx_thr_event_int_raw when mt_ch2_tx_thr_event_int_ena is set to 1.*/
            uint32_t ch3_tx_thr_event: 1;                /*The interrupt state bit  for channel 3's rmt_ch3_tx_thr_event_int_raw when mt_ch3_tx_thr_event_int_ena is set to 1.*/
            uint32_t ch4_tx_thr_event: 1;                /*The interrupt state bit  for channel 4's rmt_ch4_tx_thr_event_int_raw when mt_ch4_tx_thr_event_int_ena is set to 1.*/
            uint32_t ch5_tx_thr_event: 1;                /*The interrupt state bit  for channel 5's rmt_ch5_tx_thr_event_int_raw when mt_ch5_tx_thr_event_int_ena is set to 1.*/
            uint32_t ch6_tx_thr_event: 1;                /*The interrupt state bit  for channel 6's rmt_ch6_tx_thr_event_int_raw when mt_ch6_tx_thr_event_int_ena is set to 1.*/
            uint32_t ch7_tx_thr_event: 1;                /*The interrupt state bit  for channel 7's rmt_ch7_tx_thr_event_int_raw when mt_ch7_tx_thr_event_int_ena is set to 1.*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t ch0_tx_end:       1;               /*Set this bit to enable rmt_ch0_tx_end_int_st.*/
            uint32_t ch0_rx_end:       1;               /*Set this bit to enable rmt_ch0_rx_end_int_st.*/
            uint32_t ch0_err:          1;               /*Set this bit to enable rmt_ch0_err_int_st.*/
            uint32_t ch1_tx_end:       1;               /*Set this bit to enable rmt_ch1_tx_end_int_st.*/
            uint32_t ch1_rx_end:       1;               /*Set this bit to enable rmt_ch1_rx_end_int_st.*/
            uint32_t ch1_err:          1;               /*Set this bit to enable rmt_ch1_err_int_st.*/
            uint32_t ch2_tx_end:       1;               /*Set this bit to enable rmt_ch2_tx_end_int_st.*/
            uint32_t ch2_rx_end:       1;               /*Set this bit to enable rmt_ch2_rx_end_int_st.*/
            uint32_t ch2_err:          1;               /*Set this bit to enable rmt_ch2_err_int_st.*/
            uint32_t ch3_tx_end:       1;               /*Set this bit to enable rmt_ch3_tx_end_int_st.*/
            uint32_t ch3_rx_end:       1;               /*Set this bit to enable rmt_ch3_rx_end_int_st.*/
            uint32_t ch3_err:          1;               /*Set this bit to enable rmt_ch3_err_int_st.*/
            uint32_t ch4_tx_end:       1;               /*Set this bit to enable rmt_ch4_tx_end_int_st.*/
            uint32_t ch4_rx_end:       1;               /*Set this bit to enable rmt_ch4_rx_end_int_st.*/
            uint32_t ch4_err:          1;               /*Set this bit to enable rmt_ch4_err_int_st.*/
            uint32_t ch5_tx_end:       1;               /*Set this bit to enable rmt_ch5_tx_end_int_st.*/
            uint32_t ch5_rx_end:       1;               /*Set this bit to enable rmt_ch5_rx_end_int_st.*/
            uint32_t ch5_err:          1;               /*Set this bit to enable rmt_ch5_err_int_st.*/
            uint32_t ch6_tx_end:       1;               /*Set this bit to enable rmt_ch6_tx_end_int_st.*/
            uint32_t ch6_rx_end:       1;               /*Set this bit to enable rmt_ch6_rx_end_int_st.*/
            uint32_t ch6_err:          1;               /*Set this bit to enable rmt_ch6_err_int_st.*/
            uint32_t ch7_tx_end:       1;               /*Set this bit to enable rmt_ch7_tx_end_int_st.*/
            uint32_t ch7_rx_end:       1;               /*Set this bit to enable rmt_ch7_rx_end_int_st.*/
            uint32_t ch7_err:          1;               /*Set this bit to enable rmt_ch7_err_int_st.*/
            uint32_t ch0_tx_thr_event: 1;               /*Set this bit to enable rmt_ch0_tx_thr_event_int_st.*/
            uint32_t ch1_tx_thr_event: 1;               /*Set this bit to enable rmt_ch1_tx_thr_event_int_st.*/
            uint32_t ch2_tx_thr_event: 1;               /*Set this bit to enable rmt_ch2_tx_thr_event_int_st.*/
            uint32_t ch3_tx_thr_event: 1;               /*Set this bit to enable rmt_ch3_tx_thr_event_int_st.*/
            uint32_t ch4_tx_thr_event: 1;               /*Set this bit to enable rmt_ch4_tx_thr_event_int_st.*/
            uint32_t ch5_tx_thr_event: 1;               /*Set this bit to enable rmt_ch5_tx_thr_event_int_st.*/
            uint32_t ch6_tx_thr_event: 1;               /*Set this bit to enable rmt_ch6_tx_thr_event_int_st.*/
            uint32_t ch7_tx_thr_event: 1;               /*Set this bit to enable rmt_ch7_tx_thr_event_int_st.*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t ch0_tx_end:       1;               /*Set this bit to clear the rmt_ch0_rx_end_int_raw..*/
            uint32_t ch0_rx_end:       1;               /*Set this bit to clear the rmt_ch0_tx_end_int_raw.*/
            uint32_t ch0_err:          1;               /*Set this bit to clear the  rmt_ch0_err_int_raw.*/
            uint32_t ch1_tx_end:       1;               /*Set this bit to clear the rmt_ch1_rx_end_int_raw..*/
            uint32_t ch1_rx_end:       1;               /*Set this bit to clear the rmt_ch1_tx_end_int_raw.*/
            uint32_t ch1_err:          1;               /*Set this bit to clear the  rmt_ch1_err_int_raw.*/
            uint32_t ch2_tx_end:       1;               /*Set this bit to clear the rmt_ch2_rx_end_int_raw..*/
            uint32_t ch2_rx_end:       1;               /*Set this bit to clear the rmt_ch2_tx_end_int_raw.*/
            uint32_t ch2_err:          1;               /*Set this bit to clear the  rmt_ch2_err_int_raw.*/
            uint32_t ch3_tx_end:       1;               /*Set this bit to clear the rmt_ch3_rx_end_int_raw..*/
            uint32_t ch3_rx_end:       1;               /*Set this bit to clear the rmt_ch3_tx_end_int_raw.*/
            uint32_t ch3_err:          1;               /*Set this bit to clear the  rmt_ch3_err_int_raw.*/
            uint32_t ch4_tx_end:       1;               /*Set this bit to clear the rmt_ch4_rx_end_int_raw..*/
            uint32_t ch4_rx_end:       1;               /*Set this bit to clear the rmt_ch4_tx_end_int_raw.*/
            uint32_t ch4_err:          1;               /*Set this bit to clear the  rmt_ch4_err_int_raw.*/
            uint32_t ch5_tx_end:       1;               /*Set this bit to clear the rmt_ch5_rx_end_int_raw..*/
            uint32_t ch5_rx_end:       1;               /*Set this bit to clear the rmt_ch5_tx_end_int_raw.*/
            uint32_t ch5_err:          1;               /*Set this bit to clear the  rmt_ch5_err_int_raw.*/
            uint32_t ch6_tx_end:       1;               /*Set this bit to clear the rmt_ch6_rx_end_int_raw..*/
            uint32_t ch6_rx_end:       1;               /*Set this bit to clear the rmt_ch6_tx_end_int_raw.*/
            uint32_t ch6_err:          1;               /*Set this bit to clear the  rmt_ch6_err_int_raw.*/
            uint32_t ch7_tx_end:       1;               /*Set this bit to clear the rmt_ch7_rx_end_int_raw..*/
            uint32_t ch7_rx_end:       1;               /*Set this bit to clear the rmt_ch7_tx_end_int_raw.*/
            uint32_t ch7_err:          1;               /*Set this bit to clear the  rmt_ch7_err_int_raw.*/
            uint32_t ch0_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch0_tx_thr_event_int_raw interrupt.*/
            uint32_t ch1_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch1_tx_thr_event_int_raw interrupt.*/
            uint32_t ch2_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch2_tx_thr_event_int_raw interrupt.*/
            uint32_t ch3_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch3_tx_thr_event_int_raw interrupt.*/
            uint32_t ch4_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch4_tx_thr_event_int_raw interrupt.*/
            uint32_t ch5_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch5_tx_thr_event_int_raw interrupt.*/
            uint32_t ch6_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch6_tx_thr_event_int_raw interrupt.*/
            uint32_t ch7_tx_thr_event: 1;               /*Set this bit to clear the  rmt_ch7_tx_thr_event_int_raw interrupt.*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t low: 16;                           /*This register is used to configure carrier wave's low level value for channel0-7.*/
            uint32_t high:16;                           /*This register is used to configure carrier wave's high level value for channel0-7.*/
        };
        uint32_t val;
    } carrier_duty_ch[8];
    union {
        struct {
            uint32_t limit: 9;                          /*When channel0-7 sends more than reg_rmt_tx_lim_ch0 data then channel0-7 produce the relative interrupt.*/
            uint32_t reserved9: 23;
        };
        uint32_t val;
    } tx_lim_ch[8];
    union {
        struct {
            uint32_t fifo_mask:  1;                     /*Set this bit to enable RMTMEM and disable apb fifo access (using fifo is discouraged, please see the note above at data_ch[] item)*/
            uint32_t mem_tx_wrap_en: 1;                 /*when data need to be send is more than channel's mem can store  then set this bit to enable reuse of mem this bit is used together with reg_rmt_tx_lim_chn.*/
            uint32_t reserved2:     30;
        };
        uint32_t val;
    } apb_conf;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t date;                                      /*This is the version register.*/
} rmt_dev_t;
extern rmt_dev_t RMT;

typedef struct rmt_item32_s {
    union {
        struct {
            uint32_t duration0 :15;
            uint32_t level0 :1;
            uint32_t duration1 :15;
            uint32_t level1 :1;
        };
        uint32_t val;
    };
} rmt_item32_t;

//Allow access to RMT memory using RMTMEM.chan[0].data32[8]
typedef volatile struct rmt_mem_s {
    struct {
        union {
            rmt_item32_t data32[64];
        };
    } chan[8];
} rmt_mem_t;
extern rmt_mem_t RMTMEM;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_RMT_STRUCT_H_ */
