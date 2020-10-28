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
#ifndef _SOC_UHCI_STRUCT_H_
#define _SOC_UHCI_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct uhci_dev_s {
    union {
        struct {
            uint32_t in_rst:             1;                /*Set this bit to reset in link operations.*/
            uint32_t out_rst:            1;                /*Set this bit to reset out link operations.*/
            uint32_t ahbm_fifo_rst:      1;                /*Set this bit to reset dma ahb fifo.*/
            uint32_t ahbm_rst:           1;                /*Set this bit to reset dma  ahb interface.*/
            uint32_t in_loop_test:       1;                /*Set this bit to enable loop test for in links.*/
            uint32_t out_loop_test:      1;                /*Set this bit to enable loop test for out links.*/
            uint32_t out_auto_wrback:    1;                /*when in link's length is 0  go on to use the next in link automatically.*/
            uint32_t out_no_restart_clr: 1;                /*don't use*/
            uint32_t out_eof_mode:       1;                /*Set this bit to produce eof after DMA pops all data  clear this bit to produce eof after DMA pushes all data*/
            uint32_t uart0_ce:           1;                /*Set this bit to use UART to transmit or receive data.*/
            uint32_t uart1_ce:           1;                /*Set this bit to use UART1 to transmit or receive data.*/
            uint32_t uart2_ce:           1;                /*Set this bit to use UART2 to transmit or receive data.*/
            uint32_t outdscr_burst_en:   1;                /*Set this bit to enable DMA in links to use burst mode.*/
            uint32_t indscr_burst_en:    1;                /*Set this bit to enable DMA out links to use burst mode.*/
            uint32_t out_data_burst_en:  1;                /*Set this bit to enable DMA burst MODE*/
            uint32_t mem_trans_en:       1;
            uint32_t seper_en:           1;                /*Set this bit to use special char to separate the data frame.*/
            uint32_t head_en:            1;                /*Set this bit to enable to use head packet  before the data frame.*/
            uint32_t crc_rec_en:         1;                /*Set this bit to enable receiver''s ability of crc calculation  when crc_en bit in head packet is 1 then there will be crc bytes after data_frame*/
            uint32_t uart_idle_eof_en:   1;                /*Set this bit to enable to use idle time  when the idle time after data frame is satisfied  this means the end of a data frame.*/
            uint32_t len_eof_en:         1;                /*Set this bit to enable to use packet_len in packet head  when the received data is equal to packet_len  this means the end of a data frame.*/
            uint32_t encode_crc_en:      1;                /*Set this bit to enable crc calculation for data frame when bit6 in the head packet is 1.*/
            uint32_t clk_en:             1;                /*Set this bit to enable clock-gating for read or write registers.*/
            uint32_t uart_rx_brk_eof_en: 1;                /*Set this bit to enable to use brk char as the end of a data frame.*/
            uint32_t reserved24:         8;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t rx_start:            1;               /*when a separator char has been send  it will produce uhci_rx_start_int interrupt.*/
            uint32_t tx_start:            1;               /*when DMA detects a separator char it will produce uhci_tx_start_int interrupt.*/
            uint32_t rx_hung:             1;               /*when DMA takes a lot of time to receive a data   it will produce uhci_rx_hung_int interrupt.*/
            uint32_t tx_hung:             1;               /*when DMA takes a lot of time to read a data from RAM  it will produce uhci_tx_hung_int interrupt.*/
            uint32_t in_done:             1;               /*when a in link descriptor has been completed it will produce uhci_in_done_int interrupt.*/
            uint32_t in_suc_eof:          1;               /*when a data packet has been received  it will produce uhci_in_suc_eof_int interrupt.*/
            uint32_t in_err_eof:          1;               /*when there are some errors about eof in in link descriptor  it will produce uhci_in_err_eof_int interrupt.*/
            uint32_t out_done:            1;               /*when a out link descriptor is completed  it will produce uhci_out_done_int interrupt.*/
            uint32_t out_eof:             1;               /*when the current descriptor's eof bit is 1  it will produce uhci_out_eof_int interrupt.*/
            uint32_t in_dscr_err:         1;               /*when there are some errors about the out link descriptor  it will produce uhci_in_dscr_err_int interrupt.*/
            uint32_t out_dscr_err:        1;               /*when there are some errors about the in link descriptor  it will produce uhci_out_dscr_err_int interrupt.*/
            uint32_t in_dscr_empty:       1;               /*when there are not enough in links for DMA it will produce uhci_in_dscr_err_int interrupt.*/
            uint32_t outlink_eof_err:     1;               /*when there are some errors about eof in outlink descriptor  it will produce uhci_outlink_eof_err_int interrupt.*/
            uint32_t out_total_eof:       1;               /*When all data have been send  it will produce uhci_out_total_eof_int interrupt.*/
            uint32_t send_s_q:            1;               /*When use single send registers to send a short packets it will produce this interrupt when dma has send the short packet.*/
            uint32_t send_a_q:            1;               /*When use always_send registers to send a series of short packets it will produce this interrupt when dma has send the short packet.*/
            uint32_t dma_in_fifo_full_wm: 1;
            uint32_t reserved17:         15;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_start:            1;
            uint32_t tx_start:            1;
            uint32_t rx_hung:             1;
            uint32_t tx_hung:             1;
            uint32_t in_done:             1;
            uint32_t in_suc_eof:          1;
            uint32_t in_err_eof:          1;
            uint32_t out_done:            1;
            uint32_t out_eof:             1;
            uint32_t in_dscr_err:         1;
            uint32_t out_dscr_err:        1;
            uint32_t in_dscr_empty:       1;
            uint32_t outlink_eof_err:     1;
            uint32_t out_total_eof:       1;
            uint32_t send_s_q:            1;
            uint32_t send_a_q:            1;
            uint32_t dma_in_fifo_full_wm: 1;
            uint32_t reserved17:         15;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rx_start:            1;
            uint32_t tx_start:            1;
            uint32_t rx_hung:             1;
            uint32_t tx_hung:             1;
            uint32_t in_done:             1;
            uint32_t in_suc_eof:          1;
            uint32_t in_err_eof:          1;
            uint32_t out_done:            1;
            uint32_t out_eof:             1;
            uint32_t in_dscr_err:         1;
            uint32_t out_dscr_err:        1;
            uint32_t in_dscr_empty:       1;
            uint32_t outlink_eof_err:     1;
            uint32_t out_total_eof:       1;
            uint32_t send_s_q:            1;
            uint32_t send_a_q:            1;
            uint32_t dma_in_fifo_full_wm: 1;
            uint32_t reserved17:         15;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_start:            1;
            uint32_t tx_start:            1;
            uint32_t rx_hung:             1;
            uint32_t tx_hung:             1;
            uint32_t in_done:             1;
            uint32_t in_suc_eof:          1;
            uint32_t in_err_eof:          1;
            uint32_t out_done:            1;
            uint32_t out_eof:             1;
            uint32_t in_dscr_err:         1;
            uint32_t out_dscr_err:        1;
            uint32_t in_dscr_empty:       1;
            uint32_t outlink_eof_err:     1;
            uint32_t out_total_eof:       1;
            uint32_t send_s_q:            1;
            uint32_t send_a_q:            1;
            uint32_t dma_in_fifo_full_wm: 1;
            uint32_t reserved17:         15;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t full:       1;                      /*1:DMA out link descriptor's fifo is full.*/
            uint32_t empty:      1;                      /*1:DMA in link descriptor's fifo is empty.*/
            uint32_t reserved2: 30;
        };
        uint32_t val;
    } dma_out_status;
    union {
        struct {
            uint32_t fifo_wdata: 9;                      /*This is the data need to be pushed into out link descriptor's fifo.*/
            uint32_t reserved9:  7;
            uint32_t fifo_push:  1;                      /*Set this bit to push data in out link descriptor's fifo.*/
            uint32_t reserved17:15;
        };
        uint32_t val;
    } dma_out_push;
    union {
        struct {
            uint32_t full:         1;
            uint32_t empty:        1;
            uint32_t reserved2:    2;
            uint32_t rx_err_cause: 3;                    /*This register stores the errors caused in out link descriptor's data packet.*/
            uint32_t reserved7:   25;
        };
        uint32_t val;
    } dma_in_status;
    union {
        struct {
            uint32_t fifo_rdata:  12;                    /*This register stores the data pop from in link descriptor's fifo.*/
            uint32_t reserved12:   4;
            uint32_t fifo_pop:     1;                    /*Set this bit to pop data in in link descriptor's fifo.*/
            uint32_t reserved17:  15;
        };
        uint32_t val;
    } dma_in_pop;
    union {
        struct {
            uint32_t addr:         20;                   /*This register stores the least 20 bits of the first out link descriptor's address.*/
            uint32_t reserved20:    8;
            uint32_t stop:          1;                   /*Set this bit to stop dealing with the out link descriptors.*/
            uint32_t start:         1;                   /*Set this bit to start dealing with the out link descriptors.*/
            uint32_t restart:       1;                   /*Set this bit to mount on new out link descriptors*/
            uint32_t park:          1;                   /*1： the out link descriptor's fsm is in idle state. 0:the out link descriptor's fsm is working.*/
        };
        uint32_t val;
    } dma_out_link;
    union {
        struct {
            uint32_t addr:       20;                     /*This register stores the least 20 bits of the first in link descriptor's address.*/
            uint32_t auto_ret:    1;                     /*1:when a packet is wrong in link descriptor returns to the descriptor which is lately used.*/
            uint32_t reserved21:  7;
            uint32_t stop:        1;                     /*Set this bit to stop dealing with the in link descriptors.*/
            uint32_t start:       1;                     /*Set this bit to start dealing with the in link descriptors.*/
            uint32_t restart:     1;                     /*Set this bit to mount on new in link descriptors*/
            uint32_t park:        1;                     /*1:the in link descriptor's fsm is in idle state.   0:the in link descriptor's fsm is working*/
        };
        uint32_t val;
    } dma_in_link;
    union {
        struct {
            uint32_t check_sum_en:          1;            /*Set this bit to enable decoder to check  check_sum in packet header.*/
            uint32_t check_seq_en:          1;            /*Set this bit to enable decoder to check seq num in packet header.*/
            uint32_t crc_disable:           1;            /*Set this bit to disable crc calculation.*/
            uint32_t save_head:             1;            /*Set this bit to save packet header .*/
            uint32_t tx_check_sum_re:       1;            /*Set this bit to enable hardware replace check_sum in packet header automatically.*/
            uint32_t tx_ack_num_re:         1;            /*Set this bit to enable hardware replace ack num in packet header automatically.*/
            uint32_t check_owner:           1;            /*Set this bit to check the owner bit in link descriptor.*/
            uint32_t wait_sw_start:         1;            /*Set this bit to enable software way to add packet header.*/
            uint32_t sw_start:              1;            /*Set this bit to start inserting the packet header.*/
            uint32_t dma_in_fifo_full_thrs:12;            /*when data amount in link descriptor's fifo is more than this register value  it will produce uhci_dma_in_fifo_full_wm_int interrupt.*/
            uint32_t reserved21:           11;
        };
        uint32_t val;
    } conf1;
    uint32_t state0;                                       /**/
    uint32_t state1;                                       /**/
    uint32_t dma_out_eof_des_addr;                         /*This register stores the address of out link description when eof bit in this descriptor is 1.*/
    uint32_t dma_in_suc_eof_des_addr;                      /*This register stores the address of in link descriptor when eof bit in this descriptor is 1.*/
    uint32_t dma_in_err_eof_des_addr;                      /*This register stores the address of in link descriptor when there are some errors in this descriptor.*/
    uint32_t dma_out_eof_bfr_des_addr;                     /*This register stores the address of out link descriptor when there are some errors in this descriptor.*/
    union {
        struct {
            uint32_t test_mode:   3;                       /*bit2 is ahb bus test enable ，bit1 is used to choose write(1) or read(0) mode. bit0 is used to choose test only once(1) or continue(0)*/
            uint32_t reserved3:   1;
            uint32_t test_addr:   2;                       /*The two bits represent ahb bus address bit[20:19]*/
            uint32_t reserved6:  26;
        };
        uint32_t val;
    } ahb_test;
    uint32_t dma_in_dscr;                                  /*The content of current in link descriptor's third dword*/
    uint32_t dma_in_dscr_bf0;                              /*The content of current in link descriptor's first dword*/
    uint32_t dma_in_dscr_bf1;                              /*The content of current in link descriptor's second dword*/
    uint32_t dma_out_dscr;                                 /*The content of current out link descriptor's third dword*/
    uint32_t dma_out_dscr_bf0;                             /*The content of current out link descriptor's first dword*/
    uint32_t dma_out_dscr_bf1;                             /*The content of current out link descriptor's second dword*/
    union {
        struct {
            uint32_t tx_c0_esc_en: 1;                      /*Set this bit to enable  0xc0 char decode when DMA receives data.*/
            uint32_t tx_db_esc_en: 1;                      /*Set this bit to enable  0xdb char decode when DMA receives data.*/
            uint32_t tx_11_esc_en: 1;                      /*Set this bit to enable  flow control char 0x11 decode when DMA receives data.*/
            uint32_t tx_13_esc_en: 1;                      /*Set this bit to enable flow control char 0x13 decode when DMA receives data.*/
            uint32_t rx_c0_esc_en: 1;                      /*Set this bit to enable  0xc0 char replace when DMA sends data.*/
            uint32_t rx_db_esc_en: 1;                      /*Set this bit to enable  0xdb char replace when DMA sends data.*/
            uint32_t rx_11_esc_en: 1;                      /*Set this bit to enable  flow control char 0x11 replace when DMA sends data.*/
            uint32_t rx_13_esc_en: 1;                      /*Set this bit to enable  flow control char 0x13 replace when DMA sends data.*/
            uint32_t reserved8:   24;
        };
        uint32_t val;
    } escape_conf;
    union {
        struct {
            uint32_t txfifo_timeout:       8;              /*This register stores the timeout value.when DMA takes more time than this register value to receive a data  it will produce  uhci_tx_hung_int interrupt.*/
            uint32_t txfifo_timeout_shift: 3;              /*The tick count is cleared when its value >=(17'd8000>>reg_txfifo_timeout_shift)*/
            uint32_t txfifo_timeout_ena:   1;              /*The enable bit for tx fifo receive data  timeout*/
            uint32_t rxfifo_timeout:       8;              /*This register stores the timeout value.when DMA takes more time than this register value to read a data from RAM  it will produce  uhci_rx_hung_int interrupt.*/
            uint32_t rxfifo_timeout_shift: 3;              /*The tick count is cleared when its value >=(17'd8000>>reg_rxfifo_timeout_shift)*/
            uint32_t rxfifo_timeout_ena:   1;              /*This is the enable bit for DMA  send data timeout*/
            uint32_t reserved24:           8;
        };
        uint32_t val;
    } hung_conf;
    uint32_t ack_num;                                      /**/
    uint32_t rx_head;                                      /*This register stores the packet header received by DMA*/
    union {
        struct {
            uint32_t single_send_num: 3;                   /*The bits are used to choose which short packet*/
            uint32_t single_send_en:  1;                   /*Set this bit to enable  send a short packet*/
            uint32_t always_send_num: 3;                   /*The bits are used to choose which short packet*/
            uint32_t always_send_en:  1;                   /*Set this bit to enable continuously send the same short packet*/
            uint32_t reserved8:      24;
        };
        uint32_t val;
    } quick_sent;
    struct{
        uint32_t w_data[2];                                /*This register stores the content of short packet's dword*/
    } q_data[7];
    union {
        struct {
            uint32_t seper_char:      8;                   /*This register stores the separator char  separator char is used to separate the data frame.*/
            uint32_t seper_esc_char0: 8;                   /*This register stores the first char used to replace separator char in data.*/
            uint32_t seper_esc_char1: 8;                   /*This register stores the second char used to replace separator char in data . 0xdc 0xdb replace 0xc0 by default.*/
            uint32_t reserved24:      8;
        };
        uint32_t val;
    } esc_conf0;
    union {
        struct {
            uint32_t seq0:       8;                        /*This register stores the first substitute char used to replace the separate char.*/
            uint32_t seq0_char0: 8;                        /*This register stores the first char used to replace reg_esc_seq0 in data.*/
            uint32_t seq0_char1: 8;                        /*This register stores the second char used to replace the reg_esc_seq0 in data*/
            uint32_t reserved24: 8;
        };
        uint32_t val;
    } esc_conf1;
    union {
        struct {
            uint32_t seq1:       8;                        /*This register stores the flow control char to turn on the flow_control*/
            uint32_t seq1_char0: 8;                        /*This register stores the first char used to replace the reg_esc_seq1 in data.*/
            uint32_t seq1_char1: 8;                        /*This register stores the second char used to replace the reg_esc_seq1 in data.*/
            uint32_t reserved24: 8;
        };
        uint32_t val;
    } esc_conf2;
    union {
        struct {
            uint32_t seq2:       8;                        /*This register stores the flow_control char to turn off the flow_control*/
            uint32_t seq2_char0: 8;                        /*This register stores the first char used to replace the reg_esc_seq2 in data.*/
            uint32_t seq2_char1: 8;                        /*This register stores  the second char used to replace the reg_esc_seq2 in data.*/
            uint32_t reserved24: 8;
        };
        uint32_t val;
    } esc_conf3;
    union {
        struct {
            uint32_t thrs:      13;                        /*when the amount of packet payload is larger than this value the process of receiving data is done.*/
            uint32_t reserved13:19;
        };
        uint32_t val;
    } pkt_thres;
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
    uint32_t date;                                         /*version information*/
} uhci_dev_t;
extern uhci_dev_t UHCI0;
extern uhci_dev_t UHCI1;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_UHCI_STRUCT_H_ */
