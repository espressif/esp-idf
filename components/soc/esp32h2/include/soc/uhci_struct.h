// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_UHCI_STRUCT_H_
#define _SOC_UHCI_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct uhci_dev_s {
    union {
        struct {
            uint32_t tx_rst:             1;            /*Write 1  then write 0 to this bit to reset decode state machine.*/
            uint32_t rx_rst:             1;            /*Write 1  then write 0 to this bit to reset encode state machine.*/
            uint32_t uart0_ce:           1;            /*Set this bit to link up HCI and UART0.*/
            uint32_t uart1_ce:           1;            /*Set this bit to link up HCI and UART1.*/
            uint32_t reserved4:          1;
            uint32_t seper_en:           1;            /*Set this bit to separate the data frame using a special char.*/
            uint32_t head_en:            1;            /*Set this bit to encode the data packet with a formatting header.*/
            uint32_t crc_rec_en:         1;            /*Set this bit to enable UHCI to receive the 16 bit CRC.*/
            uint32_t uart_idle_eof_en:   1;            /*If this bit is set to 1  UHCI will end the payload receiving process when UART has been in idle state.*/
            uint32_t len_eof_en:         1;            /*If this bit is set to 1  UHCI decoder receiving payload data is end when the receiving byte count has reached the specified value. The value is payload length indicated by UHCI packet header when UHCI_HEAD_EN is 1 or the value is configuration value when UHCI_HEAD_EN is 0. If this bit is set to 0  UHCI decoder receiving payload data is end when 0xc0 is received.*/
            uint32_t encode_crc_en:      1;            /*Set this bit to enable data integrity checking by appending a 16 bit CCITT-CRC to end of the payload.*/
            uint32_t clk_en:             1;            /*1'b1: Force clock on for register. 1'b0: Support clock only when application writes registers.*/
            uint32_t uart_rx_brk_eof_en: 1;            /*If this bit is set to 1  UHCI will end payload receive process when NULL frame is received by UART.*/
            uint32_t reserved13:        19;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t rx_start:                1;       /*a*/
            uint32_t tx_start:                1;       /*a*/
            uint32_t rx_hung:                 1;       /*a*/
            uint32_t tx_hung:                 1;       /*a*/
            uint32_t send_s_q:                1;       /*a*/
            uint32_t send_a_q:                1;       /*a*/
            uint32_t outlink_eof_err:         1;       /*This is the interrupt raw bit. Triggered when there are some errors in EOF in the*/
            uint32_t app_ctrl0:               1;       /*Soft control int raw bit.*/
            uint32_t app_ctrl1:               1;       /*Soft control int raw bit.*/
            uint32_t reserved9:              23;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_start:               1;        /*a*/
            uint32_t tx_start:               1;        /*a*/
            uint32_t rx_hung:                1;        /*a*/
            uint32_t tx_hung:                1;        /*a*/
            uint32_t send_s_q:               1;        /*a*/
            uint32_t send_a_q:               1;        /*a*/
            uint32_t outlink_eof_err:        1;        /*a*/
            uint32_t app_ctrl0:              1;        /*a*/
            uint32_t app_ctrl1:              1;        /*a*/
            uint32_t reserved9:             23;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rx_start:                1;       /*a*/
            uint32_t tx_start:                1;       /*a*/
            uint32_t rx_hung:                 1;       /*a*/
            uint32_t tx_hung:                 1;       /*a*/
            uint32_t send_s_q:                1;       /*a*/
            uint32_t send_a_q:                1;       /*a*/
            uint32_t outlink_eof_err:         1;       /*a*/
            uint32_t app_ctrl0:               1;       /*a*/
            uint32_t app_ctrl1:               1;       /*a*/
            uint32_t reserved9:              23;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_start:                1;       /*a*/
            uint32_t tx_start:                1;       /*a*/
            uint32_t rx_hung:                 1;       /*a*/
            uint32_t tx_hung:                 1;       /*a*/
            uint32_t send_s_q:                1;       /*a*/
            uint32_t send_a_q:                1;       /*a*/
            uint32_t outlink_eof_err:         1;       /*a*/
            uint32_t app_ctrl0:               1;       /*a*/
            uint32_t app_ctrl1:               1;       /*a*/
            uint32_t reserved9:              23;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t check_sum_en:    1;               /*a*/
            uint32_t check_seq_en:    1;               /*a*/
            uint32_t crc_disable:     1;               /*a*/
            uint32_t save_head:       1;               /*a*/
            uint32_t tx_check_sum_re: 1;               /*a*/
            uint32_t tx_ack_num_re:   1;               /*a*/
            uint32_t reserved6:       1;
            uint32_t wait_sw_start:   1;               /*a*/
            uint32_t sw_start:        1;               /*a*/
            uint32_t reserved9:      12;
            uint32_t reserved21:     11;
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t rx_err_cause: 3;                  /*a*/
            uint32_t decode_state: 3;                  /*a*/
            uint32_t reserved6:   26;
        };
        uint32_t val;
    } state0;
    union {
        struct {
            uint32_t encode_state: 3;                  /*a*/
            uint32_t reserved3:   29;
        };
        uint32_t val;
    } state1;
    union {
        struct {
            uint32_t tx_c0_esc_en: 1;                  /*a*/
            uint32_t tx_db_esc_en: 1;                  /*a*/
            uint32_t tx_11_esc_en: 1;                  /*a*/
            uint32_t tx_13_esc_en: 1;                  /*a*/
            uint32_t rx_c0_esc_en: 1;                  /*a*/
            uint32_t rx_db_esc_en: 1;                  /*a*/
            uint32_t rx_11_esc_en: 1;                  /*a*/
            uint32_t rx_13_esc_en: 1;                  /*a*/
            uint32_t reserved8:   24;
        };
        uint32_t val;
    } escape_conf;
    union {
        struct {
            uint32_t txfifo_timeout:       8;          /*a*/
            uint32_t txfifo_timeout_shift: 3;          /*a*/
            uint32_t txfifo_timeout_ena:   1;          /*a*/
            uint32_t rxfifo_timeout:       8;          /*a*/
            uint32_t rxfifo_timeout_shift: 3;          /*a*/
            uint32_t rxfifo_timeout_ena:   1;          /*a*/
            uint32_t reserved24:           8;
        };
        uint32_t val;
    } hung_conf;
    union {
        struct {
            uint32_t ack_num:      3;
            uint32_t ack_num_load: 1;                  /*a*/
            uint32_t reserved4:   28;
        };
        uint32_t val;
    } ack_num;
    uint32_t rx_head;                                  /*a*/
    union {
        struct {
            uint32_t single_send_num: 3;               /*a*/
            uint32_t single_send_en:  1;               /*a*/
            uint32_t always_send_num: 3;               /*a*/
            uint32_t always_send_en:  1;               /*a*/
            uint32_t reserved8:      24;
        };
        uint32_t val;
    } quick_sent;
    struct {
        uint32_t w_data[2];                                 /*a*/
    } q_data[7];
    union {
        struct {
            uint32_t seper_char:      8;               /*a*/
            uint32_t seper_esc_char0: 8;               /*a*/
            uint32_t seper_esc_char1: 8;               /*a*/
            uint32_t reserved24:      8;               /*a*/
        };
        uint32_t val;
    } esc_conf0;
    union {
        struct {
            uint32_t seq0:           8;                /*a*/
            uint32_t seq0_char0:     8;                /*a*/
            uint32_t seq0_char1:     8;                /*a*/
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } esc_conf1;
    union {
        struct {
            uint32_t seq1:           8;                /*a*/
            uint32_t seq1_char0:     8;                /*a*/
            uint32_t seq1_char1:     8;                /*a*/
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } esc_conf2;
    union {
        struct {
            uint32_t seq2:           8;                /*a*/
            uint32_t seq2_char0:     8;                /*a*/
            uint32_t seq2_char1:     8;                /*a*/
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } esc_conf3;
    union {
        struct {
            uint32_t thrs:      13;                    /*a*/
            uint32_t reserved13:19;
        };
        uint32_t val;
    } pkt_thres;
    uint32_t date;                                     /*a*/
} uhci_dev_t;
extern uhci_dev_t UHCI0;
extern uhci_dev_t UHCI1;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_UHCI_STRUCT_H_ */
