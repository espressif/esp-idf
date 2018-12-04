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
#ifndef _SOC_UART_STRUCT_H_
#define _SOC_UART_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint8_t rw_byte;                 /*This register stores one byte data  read by rx fifo.*/
            uint8_t reserved[3];
        };
        uint32_t val;
    } fifo;
    union {
        struct {
            uint32_t rxfifo_full:      1;           /*This interrupt raw bit turns to high level when receiver receives more data than (rx_flow_thrhd_h3 rx_flow_thrhd).*/
            uint32_t txfifo_empty:     1;           /*This interrupt raw bit turns to high level when the amount of data in transmitter's fifo is less than ((tx_mem_cnttxfifo_cnt) .*/
            uint32_t parity_err:       1;           /*This interrupt raw bit turns to high level when receiver detects the parity error of data.*/
            uint32_t frm_err:          1;           /*This interrupt raw bit turns to high level when receiver detects data's frame error .*/
            uint32_t rxfifo_ovf:       1;           /*This interrupt raw bit turns to high level when receiver receives more data than the fifo can store.*/
            uint32_t dsr_chg:          1;           /*This interrupt raw bit turns to high level when receiver detects the edge change of dsrn signal.*/
            uint32_t cts_chg:          1;           /*This interrupt raw bit turns to high level when receiver detects the edge change of ctsn signal.*/
            uint32_t brk_det:          1;           /*This interrupt raw bit turns to high level when receiver detects the 0 after the stop bit.*/
            uint32_t rxfifo_tout:      1;           /*This interrupt raw bit turns to high level when receiver takes more time than rx_tout_thrhd to receive a byte.*/
            uint32_t sw_xon:           1;           /*This interrupt raw bit turns to high level when receiver receives xoff char with uart_sw_flow_con_en is set to 1.*/
            uint32_t sw_xoff:          1;           /*This interrupt raw bit turns to high level when receiver receives xon char with uart_sw_flow_con_en is set to 1.*/
            uint32_t glitch_det:       1;           /*This interrupt raw bit turns to high level when receiver detects the start bit.*/
            uint32_t tx_brk_done:      1;           /*This interrupt raw bit turns to high level when transmitter completes  sending  0 after all the data in transmitter's fifo are send.*/
            uint32_t tx_brk_idle_done: 1;           /*This interrupt raw bit turns to high level when transmitter has kept the shortest duration after the  last data has been send.*/
            uint32_t tx_done:          1;           /*This interrupt raw bit turns to high level when transmitter has send all the data in fifo.*/
            uint32_t rs485_parity_err: 1;           /*This interrupt raw bit turns to high level when rs485 detects the parity error.*/
            uint32_t rs485_frm_err:    1;           /*This interrupt raw bit turns to high level when rs485 detects the data frame error.*/
            uint32_t rs485_clash:      1;           /*This interrupt raw bit turns to high level when rs485 detects the clash between transmitter and receiver.*/
            uint32_t at_cmd_char_det:  1;           /*This interrupt raw bit turns to high level when receiver detects the configured at_cmd chars.*/
            uint32_t reserved19:      13;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rxfifo_full:      1;            /*This is the status bit for rxfifo_full_int_raw when rxfifo_full_int_ena is set to 1.*/
            uint32_t txfifo_empty:     1;            /*This is the status bit for  txfifo_empty_int_raw  when txfifo_empty_int_ena is set to 1.*/
            uint32_t parity_err:       1;            /*This is the status bit for parity_err_int_raw when parity_err_int_ena is set to 1.*/
            uint32_t frm_err:          1;            /*This is the status bit for frm_err_int_raw when fm_err_int_ena is set to 1.*/
            uint32_t rxfifo_ovf:       1;            /*This is the status bit for rxfifo_ovf_int_raw when rxfifo_ovf_int_ena is set to 1.*/
            uint32_t dsr_chg:          1;            /*This is the status bit for dsr_chg_int_raw when dsr_chg_int_ena is set to 1.*/
            uint32_t cts_chg:          1;            /*This is the status bit for cts_chg_int_raw when cts_chg_int_ena is set to 1.*/
            uint32_t brk_det:          1;            /*This is the status bit for brk_det_int_raw when brk_det_int_ena is set to 1.*/
            uint32_t rxfifo_tout:      1;            /*This is the status bit for rxfifo_tout_int_raw when rxfifo_tout_int_ena is set to 1.*/
            uint32_t sw_xon:           1;            /*This is the status bit for sw_xon_int_raw when sw_xon_int_ena is set to 1.*/
            uint32_t sw_xoff:          1;            /*This is the status bit for sw_xoff_int_raw when sw_xoff_int_ena is set to 1.*/
            uint32_t glitch_det:       1;            /*This is the status bit for glitch_det_int_raw when glitch_det_int_ena is set to 1.*/
            uint32_t tx_brk_done:      1;            /*This is the status bit for tx_brk_done_int_raw when tx_brk_done_int_ena is set to 1.*/
            uint32_t tx_brk_idle_done: 1;            /*This is the status bit for tx_brk_idle_done_int_raw when tx_brk_idle_done_int_ena is set to 1.*/
            uint32_t tx_done:          1;            /*This is the status bit for tx_done_int_raw when tx_done_int_ena is set to 1.*/
            uint32_t rs485_parity_err: 1;            /*This is the status bit for rs485_parity_err_int_raw when rs485_parity_int_ena is set to 1.*/
            uint32_t rs485_frm_err:    1;            /*This is the status bit for rs485_fm_err_int_raw when rs485_fm_err_int_ena is set to 1.*/
            uint32_t rs485_clash:      1;            /*This is the status bit for rs485_clash_int_raw when rs485_clash_int_ena is set to 1.*/
            uint32_t at_cmd_char_det:  1;            /*This is the status bit for at_cmd_det_int_raw when at_cmd_char_det_int_ena is set to 1.*/
            uint32_t reserved19:      13;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rxfifo_full:      1;           /*This is the enable bit for rxfifo_full_int_st register.*/
            uint32_t txfifo_empty:     1;           /*This is the enable bit for rxfifo_full_int_st register.*/
            uint32_t parity_err:       1;           /*This is the enable bit for parity_err_int_st register.*/
            uint32_t frm_err:          1;           /*This is the enable bit for frm_err_int_st register.*/
            uint32_t rxfifo_ovf:       1;           /*This is the enable bit for rxfifo_ovf_int_st register.*/
            uint32_t dsr_chg:          1;           /*This is the enable bit for dsr_chg_int_st register.*/
            uint32_t cts_chg:          1;           /*This is the enable bit for cts_chg_int_st register.*/
            uint32_t brk_det:          1;           /*This is the enable bit for brk_det_int_st register.*/
            uint32_t rxfifo_tout:      1;           /*This is the enable bit for rxfifo_tout_int_st register.*/
            uint32_t sw_xon:           1;           /*This is the enable bit for sw_xon_int_st register.*/
            uint32_t sw_xoff:          1;           /*This is the enable bit for sw_xoff_int_st register.*/
            uint32_t glitch_det:       1;           /*This is the enable bit for glitch_det_int_st register.*/
            uint32_t tx_brk_done:      1;           /*This is the enable bit for tx_brk_done_int_st register.*/
            uint32_t tx_brk_idle_done: 1;           /*This is the enable bit for tx_brk_idle_done_int_st register.*/
            uint32_t tx_done:          1;           /*This is the enable bit for tx_done_int_st register.*/
            uint32_t rs485_parity_err: 1;           /*This is the enable bit for rs485_parity_err_int_st register.*/
            uint32_t rs485_frm_err:    1;           /*This is the enable bit for rs485_parity_err_int_st register.*/
            uint32_t rs485_clash:      1;           /*This is the enable bit for rs485_clash_int_st register.*/
            uint32_t at_cmd_char_det:  1;           /*This is the enable bit for at_cmd_char_det_int_st register.*/
            uint32_t reserved19:      13;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rxfifo_full:      1;           /*Set this bit to clear the rxfifo_full_int_raw interrupt.*/
            uint32_t txfifo_empty:     1;           /*Set this bit to clear txfifo_empty_int_raw interrupt.*/
            uint32_t parity_err:       1;           /*Set this bit to clear parity_err_int_raw interrupt.*/
            uint32_t frm_err:          1;           /*Set this bit to clear frm_err_int_raw interrupt.*/
            uint32_t rxfifo_ovf:       1;           /*Set this bit to clear rxfifo_ovf_int_raw interrupt.*/
            uint32_t dsr_chg:          1;           /*Set this bit to clear the dsr_chg_int_raw interrupt.*/
            uint32_t cts_chg:          1;           /*Set this bit to clear the cts_chg_int_raw interrupt.*/
            uint32_t brk_det:          1;           /*Set this bit to clear the brk_det_int_raw interrupt.*/
            uint32_t rxfifo_tout:      1;           /*Set this bit to clear the rxfifo_tout_int_raw interrupt.*/
            uint32_t sw_xon:           1;           /*Set this bit to clear the sw_xon_int_raw interrupt.*/
            uint32_t sw_xoff:          1;           /*Set this bit to clear the sw_xon_int_raw interrupt.*/
            uint32_t glitch_det:       1;           /*Set this bit to clear the glitch_det_int_raw interrupt.*/
            uint32_t tx_brk_done:      1;           /*Set this bit to clear the tx_brk_done_int_raw interrupt..*/
            uint32_t tx_brk_idle_done: 1;           /*Set this bit to clear the tx_brk_idle_done_int_raw interrupt.*/
            uint32_t tx_done:          1;           /*Set this bit to clear the tx_done_int_raw interrupt.*/
            uint32_t rs485_parity_err: 1;           /*Set this bit to clear the rs485_parity_err_int_raw interrupt.*/
            uint32_t rs485_frm_err:    1;           /*Set this bit to clear the rs485_frm_err_int_raw interrupt.*/
            uint32_t rs485_clash:      1;           /*Set this bit to clear the rs485_clash_int_raw interrupt.*/
            uint32_t at_cmd_char_det:  1;           /*Set this bit to clear the at_cmd_char_det_int_raw interrupt.*/
            uint32_t reserved19:      13;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t div_int:    20;                /*The register value is  the  integer part of the frequency divider's factor.*/
            uint32_t div_frag:    4;                /*The register  value is the decimal part of the frequency divider's factor.*/
            uint32_t reserved24:  8;
        };
        uint32_t val;
    } clk_div;
    union {
        struct {
            uint32_t en: 1;                         /*This is the enable bit for detecting baudrate.*/
            uint32_t reserved1:   7;
            uint32_t glitch_filt: 8;                /*when input pulse width is lower then this value ignore this pulse.this register is used in auto-baud detect process.*/
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } auto_baud;
    union {
        struct {
            uint32_t rxfifo_cnt: 8;                 /*(rx_mem_cnt rxfifo_cnt) stores the byte number of valid data in receiver's fifo. rx_mem_cnt register stores the 3 most significant bits  rxfifo_cnt stores the 8 least significant bits.*/
            uint32_t st_urx_out: 4;                 /*This register stores the value of receiver's finite state machine. 0:RX_IDLE  1:RX_STRT  2:RX_DAT0  3:RX_DAT1  4:RX_DAT2  5:RX_DAT3  6:RX_DAT4  7:RX_DAT5  8:RX_DAT6  9:RX_DAT7   10:RX_PRTY   11:RX_STP1  12:RX_STP2 13:RX_DL1*/
            uint32_t reserved12: 1;
            uint32_t dsrn:       1;                 /*This register stores the level value of the internal uart dsr signal.*/
            uint32_t ctsn:       1;                 /*This register stores the level value of the internal uart cts signal.*/
            uint32_t rxd:        1;                 /*This register stores the level value of the internal uart rxd signal.*/
            uint32_t txfifo_cnt: 8;                 /*(tx_mem_cnt txfifo_cnt) stores the byte number of valid data in transmitter's fifo.tx_mem_cnt stores the 3 most significant bits  txfifo_cnt stores the 8 least significant bits.*/
            uint32_t st_utx_out: 4;                 /*This register stores the value of transmitter's finite state machine. 0:TX_IDLE  1:TX_STRT  2:TX_DAT0  3:TX_DAT1  4:TX_DAT2   5:TX_DAT3 6:TX_DAT4  7:TX_DAT5  8:TX_DAT6 9:TX_DAT7  10:TX_PRTY   11:TX_STP1  12:TX_STP2  13:TX_DL0   14:TX_DL1*/
            uint32_t reserved28: 1;
            uint32_t dtrn:       1;                 /*The register represent the level value of the internal uart dsr signal.*/
            uint32_t rtsn:       1;                 /*This register represent the level value of the internal uart cts signal.*/
            uint32_t txd:        1;                 /*This register represent the  level value of the internal uart rxd signal.*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t parity:             1;         /*This register is used to configure the parity check mode.  0:even 1:odd*/
            uint32_t parity_en:          1;         /*Set this bit to enable uart parity check.*/
            uint32_t bit_num:            2;         /*This register is used to set the length of data:  0:5bits 1:6bits 2:7bits 3:8bits*/
            uint32_t stop_bit_num:       2;         /*This register is used to set the length of  stop bit. 1:1bit  2:1.5bits  3:2bits*/
            uint32_t sw_rts:             1;         /*This register is used to configure the software rts signal which is used in software flow control.*/
            uint32_t sw_dtr:             1;         /*This register is used to configure the software dtr signal which is used in software flow control..*/
            uint32_t txd_brk:            1;         /*Set this bit to enable transmitter to  send 0 when the process of sending data is done.*/
            uint32_t irda_dplx:          1;         /*Set this bit to enable irda loop-back mode.*/
            uint32_t irda_tx_en:         1;         /*This is the start enable bit for irda transmitter.*/
            uint32_t irda_wctl:          1;         /*1：the irda transmitter's 11th bit is the same to the 10th bit. 0：set irda transmitter's 11th bit to 0.*/
            uint32_t irda_tx_inv:        1;         /*Set this bit to inverse the level value of  irda transmitter's level.*/
            uint32_t irda_rx_inv:        1;         /*Set this bit to inverse the level value of irda receiver's level.*/
            uint32_t loopback:           1;         /*Set this bit to enable uart loop-back test mode.*/
            uint32_t tx_flow_en:         1;         /*Set this bit to enable transmitter's flow control function.*/
            uint32_t irda_en:            1;         /*Set this bit to enable irda protocol.*/
            uint32_t rxfifo_rst:         1;         /*Set this bit to reset uart receiver's fifo.*/
            uint32_t txfifo_rst:         1;         /*Set this bit to reset uart transmitter's fifo.*/
            uint32_t rxd_inv:            1;         /*Set this bit to inverse the level value of uart rxd signal.*/
            uint32_t cts_inv:            1;         /*Set this bit to inverse the level value of uart cts signal.*/
            uint32_t dsr_inv:            1;         /*Set this bit to inverse the level value of uart dsr signal.*/
            uint32_t txd_inv:            1;         /*Set this bit to inverse the level value of uart txd signal.*/
            uint32_t rts_inv:            1;         /*Set this bit to inverse the level value of uart rts signal.*/
            uint32_t dtr_inv:            1;         /*Set this bit to inverse the level value of uart dtr signal.*/
            uint32_t clk_en:             1;         /*1：force clock on for registers：support clock only when write registers*/
            uint32_t err_wr_mask:        1;         /*1：receiver stops storing data int fifo when data is wrong. 0：receiver stores the data even if the  received data is wrong.*/
            uint32_t tick_ref_always_on: 1;         /*This register is used to select the clock.1：apb clock：ref_tick*/
            uint32_t reserved28:         4;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t rxfifo_full_thrhd:  7;         /*When receiver receives more data than its threshold value，receiver will produce rxfifo_full_int_raw interrupt.the threshold value is (rx_flow_thrhd_h3 rxfifo_full_thrhd).*/
            uint32_t reserved7:          1;
            uint32_t txfifo_empty_thrhd: 7;         /*when the data amount in transmitter fifo is less than its threshold value， it will produce txfifo_empty_int_raw interrupt. the threshold value is (tx_mem_empty_thrhd txfifo_empty_thrhd)*/
            uint32_t reserved15:         1;
            uint32_t rx_flow_thrhd:      7;         /*when receiver receives more data than its threshold value， receiver produce signal to tell the transmitter stop transferring data. the threshold value is (rx_flow_thrhd_h3 rx_flow_thrhd).*/
            uint32_t rx_flow_en:         1;         /*This is the flow enable bit for uart receiver. 1:choose software flow control with configuring sw_rts signal*/
            uint32_t rx_tout_thrhd:      7;         /*This register is used to configure the timeout value for uart receiver receiving a byte.*/
            uint32_t rx_tout_en:         1;         /*This is the enable bit for uart receiver's timeout function.*/
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t min_cnt:     20;               /*This register stores the value of the minimum duration time for the low level pulse， it is used in baudrate-detect process.*/
            uint32_t reserved20:  12;
        };
        uint32_t val;
    } lowpulse;
    union {
        struct {
            uint32_t min_cnt:     20;               /*This register stores  the value of the maximum duration time for the high level pulse， it is used in baudrate-detect process.*/
            uint32_t reserved20:  12;
        };
        uint32_t val;
    } highpulse;
    union {
        struct {
            uint32_t edge_cnt:    10;               /*This register stores the count of rxd edge change， it is used in baudrate-detect process.*/
            uint32_t reserved10:  22;
        };
        uint32_t val;
    } rxd_cnt;
    union {
        struct {
            uint32_t sw_flow_con_en: 1;             /*Set this bit to enable software  flow control. it is used with register sw_xon or sw_xoff .*/
            uint32_t xonoff_del:     1;             /*Set this bit to remove flow control char from the received data.*/
            uint32_t force_xon:      1;             /*Set this bit to clear ctsn to stop the  transmitter from sending data.*/
            uint32_t force_xoff:     1;             /*Set this bit to set ctsn to enable the transmitter to go on sending data.*/
            uint32_t send_xon:       1;             /*Set this bit to send xon char， it is cleared by hardware automatically.*/
            uint32_t send_xoff:      1;             /*Set this bit to send xoff char， it is cleared by hardware automatically.*/
            uint32_t reserved6:     26;
        };
        uint32_t val;
    } flow_conf;
    union {
        struct {
            uint32_t active_threshold:10;           /*When the input rxd edge changes more than this register value， the uart is active from light sleeping mode.*/
            uint32_t reserved10:      22;
        };
        uint32_t val;
    } sleep_conf;
    union {
        struct {
            uint32_t xon_threshold:  8;             /*when the data amount in receiver's fifo is more than this register value， it will send a xoff char with uart_sw_flow_con_en set to 1.*/
            uint32_t xoff_threshold: 8;             /*When the data amount in receiver's fifo is less than this register value， it will send a xon char with uart_sw_flow_con_en set to 1.*/
            uint32_t xon_char:       8;             /*This register stores the xon flow control char.*/
            uint32_t xoff_char:      8;             /*This register stores the xoff flow control char.*/
        };
        uint32_t val;
    } swfc_conf;
    union {
        struct {
            uint32_t rx_idle_thrhd:10;              /*when receiver takes more time than this register value to receive a byte data， it will produce frame end signal for uhci to stop receiving data.*/
            uint32_t tx_idle_num:  10;              /*This register is used to configure the duration time between transfers.*/
            uint32_t tx_brk_num:    8;              /*This register is used to configure the number of 0 send after the process of sending data is done. it is active when txd_brk is set to 1.*/
            uint32_t reserved28:    4;
        };
        uint32_t val;
    } idle_conf;
    union {
        struct {
            uint32_t en:               1;           /*Set this bit to choose rs485 mode.*/
            uint32_t dl0_en:           1;           /*Set this bit to delay the stop bit by 1 bit.*/
            uint32_t dl1_en:           1;           /*Set this bit to delay the stop bit by 1 bit.*/
            uint32_t tx_rx_en:         1;           /*Set this bit to enable loop-back transmitter's output data signal to receiver's input data signal.*/
            uint32_t rx_busy_tx_en:    1;           /*1: enable rs485's transmitter to send data when rs485's receiver is busy. 0:rs485's transmitter should not send data when its receiver is busy.*/
            uint32_t rx_dly_num:       1;           /*This register is used to delay the receiver's internal data signal.*/
            uint32_t tx_dly_num:       4;           /*This register is used to delay the transmitter's internal data signal.*/
            uint32_t reserved10:      22;
        };
        uint32_t val;
    } rs485_conf;
    union {
        struct {
            uint32_t pre_idle_num:24;               /*This register is used to configure the idle duration time before the first at_cmd is received by receiver， when the the duration is less than this register value it will not take the next data received as at_cmd char.*/
            uint32_t reserved24:   8;
        };
        uint32_t val;
    } at_cmd_precnt;
    union {
        struct {
            uint32_t post_idle_num:24;              /*This register is used to configure the duration time between the last at_cmd and the next data， when the duration is less than this register value  it will not take the previous data as at_cmd char.*/
            uint32_t reserved24:    8;
        };
        uint32_t val;
    } at_cmd_postcnt;
    union {
        struct {
            uint32_t rx_gap_tout:24;                /*This register is used to configure the duration time between the at_cmd chars， when the duration time is less than this register value it will not take the data as continous at_cmd chars.*/
            uint32_t reserved24:  8;
        };
        uint32_t val;
    } at_cmd_gaptout;
    union {
        struct {
            uint32_t data:        8;                /*This register is used to configure the content of at_cmd char.*/
            uint32_t char_num:    8;                /*This register is used to configure the number of continuous at_cmd chars received by receiver.*/
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } at_cmd_char;
    union {
        struct {
            uint32_t mem_pd:             1;         /*Set this bit to power down memory，when reg_mem_pd registers in the 3 uarts are all set to 1  memory will enter low power mode.*/
            uint32_t reserved1:          1;
            uint32_t reserved2:          1;
            uint32_t rx_size:            4;         /*This register is used to configure the amount of mem allocated to receiver's fifo. the default byte num is 128.*/
            uint32_t tx_size:            4;         /*This register is used to configure the amount of mem allocated to transmitter's fifo.the default byte num is 128.*/
            uint32_t reserved11:         4;
            uint32_t rx_flow_thrhd_h3:   3;         /*refer to the rx_flow_thrhd's description.*/
            uint32_t rx_tout_thrhd_h3:   3;         /*refer to the rx_tout_thrhd's description.*/
            uint32_t xon_threshold_h2:   2;         /*refer to the uart_xon_threshold's description.*/
            uint32_t xoff_threshold_h2:  2;         /*refer to the uart_xoff_threshold's description.*/
            uint32_t rx_mem_full_thrhd:  3;         /*refer to the rxfifo_full_thrhd's description.*/
            uint32_t tx_mem_empty_thrhd: 3;         /*refer to txfifo_empty_thrhd 's description.*/
            uint32_t reserved31:         1;
        };
        uint32_t val;
    } mem_conf;
    union {
        struct {
            uint32_t status:24;
            uint32_t reserved24:    8;
        };
        uint32_t val;
    } mem_tx_status;
    union {
        struct {
            uint32_t status:      24;
            uint32_t reserved24:   8;
        };
        struct {
            uint32_t reserved0:     2;
            uint32_t rd_addr:      11;              /*This register stores the rx mem read address.*/
            uint32_t wr_addr:      11;              /*This register stores the rx mem write address.*/
            uint32_t reserved:      8;
        };
        uint32_t val;
    } mem_rx_status;
    union {
        struct {
            uint32_t rx_cnt: 3;                      /*refer to the rxfifo_cnt's description.*/
            uint32_t tx_cnt: 3;                      /*refer to the txfifo_cnt's description.*/
            uint32_t reserved6: 26;
        };
        uint32_t val;
    } mem_cnt_status;
    union {
        struct {
            uint32_t min_cnt:     20;                 /*This register stores the count of rxd pos-edge edge， it is used in baudrate-detect process.*/
            uint32_t reserved20:  12;
        };
        uint32_t val;
    } pospulse;
    union {
        struct {
            uint32_t min_cnt:     20;                 /*This register stores the count of rxd neg-edge edge， it is used in baudrate-detect process.*/
            uint32_t reserved20:  12;
        };
        uint32_t val;
    } negpulse;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t date;                                    /**/
    uint32_t id;                                      /**/
} uart_dev_t;
extern uart_dev_t UART0;
extern uart_dev_t UART1;
extern uart_dev_t UART2;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_UART_STRUCT_H_ */
