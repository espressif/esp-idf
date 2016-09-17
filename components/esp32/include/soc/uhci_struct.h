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
typedef struct {
    union {
        struct {
            volatile uint32_t in_rst:             1;                /*Set this bit to reset in link operations.*/
            volatile uint32_t out_rst:            1;                /*Set this bit to reset out link operations.*/
            volatile uint32_t ahbm_fifo_rst:      1;                /*Set this bit to reset dma ahb fifo.*/
            volatile uint32_t ahbm_rst:           1;                /*Set this bit to reset dma  ahb interface.*/
            volatile uint32_t in_loop_test:       1;                /*Set this bit to enable loop test for in links.*/
            volatile uint32_t out_loop_test:      1;                /*Set this bit to enable loop test for out links.*/
            volatile uint32_t out_auto_wrback:    1;                /*when in link's length is 0  go on to use the next in link automatically.*/
            volatile uint32_t out_no_restart_clr: 1;                /*don't use*/
            volatile uint32_t out_eof_mode:       1;                /*Set this bit to produce eof after DMA pops all data  clear this bit to produce eof after DMA pushes all data*/
            volatile uint32_t uart0_ce:           1;                /*Set this bit to use UART to transmit or receive data.*/
            volatile uint32_t uart1_ce:           1;                /*Set this bit to use UART1 to transmit or receive data.*/
            volatile uint32_t uart2_ce:           1;                /*Set this bit to use UART2 to transmit or receive data.*/
            volatile uint32_t outdscr_burst_en:   1;                /*Set this bit to enable DMA in links to use burst mode.*/
            volatile uint32_t indscr_burst_en:    1;                /*Set this bit to enable DMA out links to use burst mode.*/
            volatile uint32_t out_data_burst_en:  1;                /*Set this bit to enable DMA burst MODE*/
            volatile uint32_t mem_trans_en:       1;
            volatile uint32_t seper_en:           1;                /*Set this bit to use special char to separate the data frame.*/
            volatile uint32_t head_en:            1;                /*Set this bit to enable to use head packet  before the data frame.*/
            volatile uint32_t crc_rec_en:         1;                /*Set this bit to enable receiver''s ability of crc calculation  when crc_en bit in head packet is 1 then there will be crc bytes after data_frame*/
            volatile uint32_t uart_idle_eof_en:   1;                /*Set this bit to enable to use idle time  when the idle time after data frame is satisfied  this means the end of a data frame.*/
            volatile uint32_t len_eof_en:         1;                /*Set this bit to enable to use packet_len in packet head  when the received data is equal to packet_len  this means the end of a data frame.*/
            volatile uint32_t encode_crc_en:      1;                /*Set this bit to enable crc calculation for data frame when bit6 in the head packet is 1.*/
            volatile uint32_t clk_en:             1;                /*Set this bit to enable clock-gating for read or write registers.*/
            volatile uint32_t uart_rx_brk_eof_en: 1;                /*Set this bit to enable to use brk char as the end of a data frame.*/
            volatile uint32_t reserved24:         8;
        };
        volatile uint32_t val;
    }conf0;
    union {
        struct {
            volatile uint32_t rx_start_int_raw:           1;        /*when a separator char has been send  it will produce uhci_rx_start_int interrupt.*/
            volatile uint32_t tx_start_int_raw:           1;        /*when DMA detects a separator char it will produce uhci_tx_start_int interrupt.*/
            volatile uint32_t rx_hung_int_raw:            1;        /*when DMA takes a lot of time to receive a data   it will produce uhci_rx_hung_int interrupt.*/
            volatile uint32_t tx_hung_int_raw:            1;        /*when DMA takes a lot of time to read a data from RAM  it will produce uhci_tx_hung_int interrupt.*/
            volatile uint32_t in_done_int_raw:            1;        /*when a in link descriptor has been completed it will produce uhci_in_done_int interrupt.*/
            volatile uint32_t in_suc_eof_int_raw:         1;        /*when a data packet has been received  it will produce uhci_in_suc_eof_int interrupt.*/
            volatile uint32_t in_err_eof_int_raw:         1;        /*when there are some errors about eof in in link descriptor  it will produce uhci_in_err_eof_int interrupt.*/
            volatile uint32_t out_done_int_raw:           1;        /*when a out link descriptor is completed  it will produce uhci_out_done_int interrupt.*/
            volatile uint32_t out_eof_int_raw:            1;        /*when the current descriptor's eof bit is 1  it will produce uhci_out_eof_int interrupt.*/
            volatile uint32_t in_dscr_err_int_raw:        1;        /*when there are some errors about the out link descriptor  it will produce uhci_in_dscr_err_int interrupt.*/
            volatile uint32_t out_dscr_err_int_raw:       1;        /*when there are some errors about the in link descriptor  it will produce uhci_out_dscr_err_int interrupt.*/
            volatile uint32_t in_dscr_empty_int_raw:      1;        /*when there are not enough in links for DMA it will produce uhci_in_dscr_err_int interrupt.*/
            volatile uint32_t outlink_eof_err_int_raw:    1;        /*when there are some errors about eof in outlink descriptor  it will produce uhci_outlink_eof_err_int interrupt.*/
            volatile uint32_t out_total_eof_int_raw:      1;        /*When all data have been send  it will produce uhci_out_total_eof_int interrupt.*/
            volatile uint32_t send_s_q_int_raw:           1;        /*When use single send registers to send a short packets it will produce this interrupt when dma has send the short packet.*/
            volatile uint32_t send_a_q_int_raw:           1;        /*When use always_send registers to send a series of short packets it will produce this interrupt when dma has send the short packet.*/
            volatile uint32_t dma_infifo_full_wm_int_raw: 1;
            volatile uint32_t reserved17:                15;
        };
        volatile uint32_t val;
    }int_raw;
    union {
        struct {
            volatile uint32_t rx_start_int_st:           1;
            volatile uint32_t tx_start_int_st:           1;
            volatile uint32_t rx_hung_int_st:            1;
            volatile uint32_t tx_hung_int_st:            1;
            volatile uint32_t in_done_int_st:            1;
            volatile uint32_t in_suc_eof_int_st:         1;
            volatile uint32_t in_err_eof_int_st:         1;
            volatile uint32_t out_done_int_st:           1;
            volatile uint32_t out_eof_int_st:            1;
            volatile uint32_t in_dscr_err_int_st:        1;
            volatile uint32_t out_dscr_err_int_st:       1;
            volatile uint32_t in_dscr_empty_int_st:      1;
            volatile uint32_t outlink_eof_err_int_st:    1;
            volatile uint32_t out_total_eof_int_st:      1;
            volatile uint32_t send_s_q_int_st:           1;
            volatile uint32_t send_a_q_int_st:           1;
            volatile uint32_t dma_infifo_full_wm_int_st: 1;
            volatile uint32_t reserved17:               15;
        };
        volatile uint32_t val;
    }int_st;
    union {
        struct {
            volatile uint32_t rx_start_int_ena:           1;
            volatile uint32_t tx_start_int_ena:           1;
            volatile uint32_t rx_hung_int_ena:            1;
            volatile uint32_t tx_hung_int_ena:            1;
            volatile uint32_t in_done_int_ena:            1;
            volatile uint32_t in_suc_eof_int_ena:         1;
            volatile uint32_t in_err_eof_int_ena:         1;
            volatile uint32_t out_done_int_ena:           1;
            volatile uint32_t out_eof_int_ena:            1;
            volatile uint32_t in_dscr_err_int_ena:        1;
            volatile uint32_t out_dscr_err_int_ena:       1;
            volatile uint32_t in_dscr_empty_int_ena:      1;
            volatile uint32_t outlink_eof_err_int_ena:    1;
            volatile uint32_t out_total_eof_int_ena:      1;
            volatile uint32_t send_s_q_int_ena:           1;
            volatile uint32_t send_a_q_int_ena:           1;
            volatile uint32_t dma_infifo_full_wm_int_ena: 1;
            volatile uint32_t reserved17:                15;
        };
        volatile uint32_t val;
    }int_ena;
    union {
        struct {
            volatile uint32_t rx_start_int_clr:           1;
            volatile uint32_t tx_start_int_clr:           1;
            volatile uint32_t rx_hung_int_clr:            1;
            volatile uint32_t tx_hung_int_clr:            1;
            volatile uint32_t in_done_int_clr:            1;
            volatile uint32_t in_suc_eof_int_clr:         1;
            volatile uint32_t in_err_eof_int_clr:         1;
            volatile uint32_t out_done_int_clr:           1;
            volatile uint32_t out_eof_int_clr:            1;
            volatile uint32_t in_dscr_err_int_clr:        1;
            volatile uint32_t out_dscr_err_int_clr:       1;
            volatile uint32_t in_dscr_empty_int_clr:      1;
            volatile uint32_t outlink_eof_err_int_clr:    1;
            volatile uint32_t out_total_eof_int_clr:      1;
            volatile uint32_t send_s_q_int_clr:           1;
            volatile uint32_t send_a_q_int_clr:           1;
            volatile uint32_t dma_infifo_full_wm_int_clr: 1;
            volatile uint32_t reserved17:                15;
        };
        volatile uint32_t val;
    }int_clr;
    union {
        struct {
            volatile uint32_t out_full:   1;                        /*1:DMA out link descriptor's fifo is full.*/
            volatile uint32_t out_empty:  1;                        /*1:DMA in link descriptor's fifo is empty.*/
            volatile uint32_t reserved2: 30;
        };
        volatile uint32_t val;
    }dma_out_status;
    union {
        struct {
            volatile uint32_t outfifo_wdata: 9;                     /*This is the data need to be pushed into out link descriptor's fifo.*/
            volatile uint32_t reserved9:     7;
            volatile uint32_t outfifo_push:  1;                     /*Set this bit to push data in out link descriptor's fifo.*/
            volatile uint32_t reserved17:   15;
        };
        volatile uint32_t val;
    }dma_out_push;
    union {
        struct {
            volatile uint32_t in_full:      1;
            volatile uint32_t in_empty:     1;
            volatile uint32_t reserved2:    2;
            volatile uint32_t rx_err_cause: 3;                      /*This register stores the errors caused in out link descriptor's data packet.*/
            volatile uint32_t reserved7:   25;
        };
        volatile uint32_t val;
    }dma_in_status;
    union {
        struct {
            volatile uint32_t infifo_rdata:12;                      /*This register stores the data pop from in link descriptor's fifo.*/
            volatile uint32_t reserved12:   4;
            volatile uint32_t infifo_pop:   1;                      /*Set this bit to pop data in in link descriptor's fifo.*/
            volatile uint32_t reserved17:  15;
        };
        volatile uint32_t val;
    }dma_in_pop;
    union {
        struct {
            volatile uint32_t outlink_addr:   20;                   /*This register stores the least 20 bits of the first out link descriptor's address.*/
            volatile uint32_t reserved20:      8;
            volatile uint32_t outlink_stop:    1;                   /*Set this bit to stop dealing with the out link descriptors.*/
            volatile uint32_t outlink_start:   1;                   /*Set this bit to start dealing with the out link descriptors.*/
            volatile uint32_t outlink_restart: 1;                   /*Set this bit to mount on new out link descriptors*/
            volatile uint32_t outlink_park:    1;                   /*1： the out link descriptor's fsm is in idle state. 0:the out link descriptor's fsm is working.*/
        };
        volatile uint32_t val;
    }dma_out_link;
    union {
        struct {
            volatile uint32_t inlink_addr:    20;                   /*This register stores the least 20 bits of the first in link descriptor's address.*/
            volatile uint32_t inlink_auto_ret: 1;                   /*1:when a packet is wrong in link descriptor returns to the descriptor which is lately used.*/
            volatile uint32_t reserved21:      7;
            volatile uint32_t inlink_stop:     1;                   /*Set this bit to stop dealing with the in link descriptors.*/
            volatile uint32_t inlink_start:    1;                   /*Set this bit to start dealing with the in link descriptors.*/
            volatile uint32_t inlink_restart:  1;                   /*Set this bit to mount on new in link descriptors*/
            volatile uint32_t inlink_park:     1;                   /*1:the in link descriptor's fsm is in idle state.   0:the in link descriptor's fsm is working*/
        };
        volatile uint32_t val;
    }dma_in_link;
    union {
        struct {
            volatile uint32_t check_sum_en:         1;              /*Set this bit to enable decoder to check  check_sum in packet header.*/
            volatile uint32_t check_seq_en:         1;              /*Set this bit to enable decoder to check seq num in packet header.*/
            volatile uint32_t crc_disable:          1;              /*Set this bit to disable crc calculation.*/
            volatile uint32_t save_head:            1;              /*Set this bit to save packet header .*/
            volatile uint32_t tx_check_sum_re:      1;              /*Set this bit to enable hardware replace check_sum in packet header automatically.*/
            volatile uint32_t tx_ack_num_re:        1;              /*Set this bit to enable hardware replace ack num in packet header automatically.*/
            volatile uint32_t check_owner:          1;              /*Set this bit to check the owner bit in link descriptor.*/
            volatile uint32_t wait_sw_start:        1;              /*Set this bit to enable software way to add packet header.*/
            volatile uint32_t sw_start:             1;              /*Set this bit to start inserting the packet header.*/
            volatile uint32_t dma_infifo_full_thrs:12;              /*when data amount in link descriptor's fifo is more than this register value  it will produce uhci_dma_infifo_full_wm_int interrupt.*/
            volatile uint32_t reserved21:          11;
        };
        volatile uint32_t val;
    }conf1;
    volatile uint32_t state0;                                       /**/
    volatile uint32_t state1;                                       /**/
    volatile uint32_t dma_out_eof_des_addr;                         /*This register stores the address of out link description when eof bit in this descriptor is 1.*/
    volatile uint32_t dma_in_suc_eof_des_addr;                      /*This register stores the address of in link descriptor when eof bit in this descriptor is 1.*/
    volatile uint32_t dma_in_err_eof_des_addr;                      /*This register stores the address of in link descriptor when there are some errors in this descriptor.*/
    volatile uint32_t dma_out_eof_bfr_des_addr;                     /*This register stores the address of out link descriptor when there are some errors in this descriptor.*/
    union {
        struct {
            volatile uint32_t ahb_testmode: 3;                      /*bit2 is ahb bus test enable ，bit1 is used to choose write(1) or read(0) mode. bit0 is used to choose test only once(1) or continue(0)*/
            volatile uint32_t reserved3:    1;
            volatile uint32_t ahb_testaddr: 2;                      /*The two bits represent ahb bus address bit[20:19]*/
            volatile uint32_t reserved6:   26;
        };
        volatile uint32_t val;
    }ahb_test;
    volatile uint32_t dma_in_dscr;                                  /*The content of current in link descriptor's third dword*/
    volatile uint32_t dma_in_dscr_bf0;                              /*The content of current in link descriptor's first dword*/
    volatile uint32_t dma_in_dscr_bf1;                              /*The content of current in link descriptor's second dword*/
    volatile uint32_t dma_out_dscr;                                 /*The content of current out link descriptor's third dword*/
    volatile uint32_t dma_out_dscr_bf0;                             /*The content of current out link descriptor's first dword*/
    volatile uint32_t dma_out_dscr_bf1;                             /*The content of current out link descriptor's second dword*/
    union {
        struct {
            volatile uint32_t tx_c0_esc_en: 1;                      /*Set this bit to enable  0xc0 char decode when DMA receives data.*/
            volatile uint32_t tx_db_esc_en: 1;                      /*Set this bit to enable  0xdb char decode when DMA receives data.*/
            volatile uint32_t tx_11_esc_en: 1;                      /*Set this bit to enable  flow control char 0x11 decode when DMA receives data.*/
            volatile uint32_t tx_13_esc_en: 1;                      /*Set this bit to enable flow control char 0x13 decode when DMA receives data.*/
            volatile uint32_t rx_c0_esc_en: 1;                      /*Set this bit to enable  0xc0 char replace when DMA sends data.*/
            volatile uint32_t rx_db_esc_en: 1;                      /*Set this bit to enable  0xdb char replace when DMA sends data.*/
            volatile uint32_t rx_11_esc_en: 1;                      /*Set this bit to enable  flow control char 0x11 replace when DMA sends data.*/
            volatile uint32_t rx_13_esc_en: 1;                      /*Set this bit to enable  flow control char 0x13 replace when DMA sends data.*/
            volatile uint32_t reserved8:   24;
        };
        volatile uint32_t val;
    }escape_conf;
    union {
        struct {
            volatile uint32_t txfifo_timeout:       8;              /*This register stores the timeout value.when DMA takes more time than this register value to receive a data  it will produce  uhci_tx_hung_int interrupt.*/
            volatile uint32_t txfifo_timeout_shift: 3;              /*The tick count is cleared when its value >=(17'd8000>>reg_txfifo_timeout_shift)*/
            volatile uint32_t txfifo_timeout_ena:   1;              /*The enable bit for tx fifo receive data  timeout*/
            volatile uint32_t rxfifo_timeout:       8;              /*This register stores the timeout value.when DMA takes more time than this register value to read a data from RAM  it will produce  uhci_rx_hung_int interrupt.*/
            volatile uint32_t rxfifo_timeout_shift: 3;              /*The tick count is cleared when its value >=(17'd8000>>reg_rxfifo_timeout_shift)*/
            volatile uint32_t rxfifo_timeout_ena:   1;              /*This is the enable bit for DMA  send data timeout*/
            volatile uint32_t reserved24:           8;
        };
        volatile uint32_t val;
    }hung_conf;
    volatile uint32_t ack_num;                                      /**/
    volatile uint32_t rx_head;                                      /*This register stores the packet header received by DMA*/
    union {
        struct {
            volatile uint32_t single_send_num: 3;                   /*The bits are used to choose which short packet*/
            volatile uint32_t single_send_en:  1;                   /*Set this bit to enable  send a short packet*/
            volatile uint32_t always_send_num: 3;                   /*The bits are used to choose which short packet*/
            volatile uint32_t always_send_en:  1;                   /*Set this bit to enable continuously send the same short packet*/
            volatile uint32_t reserved8:      24;
        };
        volatile uint32_t val;
    }quick_sent;
    struct{
        volatile uint32_t w_data[2];                                 /*This register stores the content of short packet's dword*/
    }q_data[7];
    union {
        struct {
            volatile uint32_t seper_char:      8;                   /*This register stores the separator char  separator char is used to separate the data frame.*/
            volatile uint32_t seper_esc_char0: 8;                   /*This register stores the first char used to replace separator char in data.*/
            volatile uint32_t seper_esc_char1: 8;                   /*This register stores the second char used to replace separator char in data . 0xdc 0xdb replace 0xc0 by default.*/
            volatile uint32_t reserved24:      8;
        };
        volatile uint32_t val;
    }esc_conf0;
    union {
        struct {
            volatile uint32_t esc_seq0:       8;                    /*This register stores the first substitute char used to replace the separate char.*/
            volatile uint32_t esc_seq0_char0: 8;                    /*This register stores the first char used to replace reg_esc_seq0 in data.*/
            volatile uint32_t esc_seq0_char1: 8;                    /*This register stores the second char used to replace the reg_esc_seq0 in data*/
            volatile uint32_t reserved24:     8;
        };
        volatile uint32_t val;
    }esc_conf1;
    union {
        struct {
            volatile uint32_t esc_seq1:       8;                    /*This register stores the flow control char to turn on the flow_control*/
            volatile uint32_t esc_seq1_char0: 8;                    /*This register stores the first char used to replace the reg_esc_seq1 in data.*/
            volatile uint32_t esc_seq1_char1: 8;                    /*This register stores the second char used to replace the reg_esc_seq1 in data.*/
            volatile uint32_t reserved24:     8;
        };
        volatile uint32_t val;
    }esc_conf2;
    union {
        struct {
            volatile uint32_t esc_seq2:       8;                    /*This register stores the flow_control char to turn off the flow_control*/
            volatile uint32_t esc_seq2_char0: 8;                    /*This register stores the first char used to replace the reg_esc_seq2 in data.*/
            volatile uint32_t esc_seq2_char1: 8;                    /*This register stores  the second char used to replace the reg_esc_seq2 in data.*/
            volatile uint32_t reserved24:     8;
        };
        volatile uint32_t val;
    }esc_conf3;
    union {
        struct {
            volatile uint32_t pkt_thrs:  13;                        /*when the amount of packet payload is larger than this value the process of receiving data is done.*/
            volatile uint32_t reserved13:19;
        };
        volatile uint32_t val;
    }pkt_thres;
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
    volatile uint32_t date;                                         /*version information*/
} uhci_dev_t;
extern volatile uhci_dev_t UHCI0;
extern volatile uhci_dev_t UHCI1;
#endif  /* _SOC_UHCI_STRUCT_H_ */
