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
#ifndef _SOC_UHCI_STRUCT_H_
#define _SOC_UHCI_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct uhci_dev_s {
    union {
        struct {
            uint32_t tx_rst                        :    1;
            uint32_t rx_rst                        :    1;
            uint32_t uart0_ce                      :    1;
            uint32_t uart1_ce                      :    1;
            uint32_t uart2_ce                      :    1;
            uint32_t seper_en                      :    1;
            uint32_t head_en                       :    1;
            uint32_t crc_rec_en                    :    1;
            uint32_t uart_idle_eof_en              :    1;
            uint32_t len_eof_en                    :    1;
            uint32_t encode_crc_en                 :    1;
            uint32_t clk_en                        :    1;
            uint32_t uart_rx_brk_eof_en            :    1;
            uint32_t reserved13                    :    19;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t rx_start                      :    1;
            uint32_t tx_start                      :    1;
            uint32_t rx_hung                       :    1;
            uint32_t tx_hung                       :    1;
            uint32_t send_s_q                      :    1;
            uint32_t send_a_q                      :    1;
            uint32_t outlink_eof_err               :    1;
            uint32_t app_ctrl0                     :    1;
            uint32_t app_ctrl1                     :    1;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_start                      :    1;
            uint32_t tx_start                      :    1;
            uint32_t rx_hung                       :    1;
            uint32_t tx_hung                       :    1;
            uint32_t send_s_q                      :    1;
            uint32_t send_a_q                      :    1;
            uint32_t outlink_eof_err               :    1;
            uint32_t app_ctrl0                     :    1;
            uint32_t app_ctrl1                     :    1;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rx_start                      :    1;
            uint32_t tx_start                      :    1;
            uint32_t rx_hung                       :    1;
            uint32_t tx_hung                       :    1;
            uint32_t send_s_q                      :    1;
            uint32_t send_a_q                      :    1;
            uint32_t outlink_eof_err               :    1;
            uint32_t app_ctrl0                     :    1;
            uint32_t app_ctrl1                     :    1;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_start                      :    1;
            uint32_t tx_start                      :    1;
            uint32_t rx_hung                       :    1;
            uint32_t tx_hung                       :    1;
            uint32_t send_s_q                      :    1;
            uint32_t send_a_q                      :    1;
            uint32_t outlink_eof_err               :    1;
            uint32_t app_ctrl0                     :    1;
            uint32_t app_ctrl1                     :    1;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t app_ctrl0_int_set             :    1;
            uint32_t app_ctrl1_int_set             :    1;
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } app_int_set;
    union {
        struct {
            uint32_t check_sum_en                  :    1;
            uint32_t check_seq_en                  :    1;
            uint32_t crc_disable                   :    1;
            uint32_t save_head                     :    1;
            uint32_t tx_check_sum_re               :    1;
            uint32_t tx_ack_num_re                 :    1;
            uint32_t reserved6                     :    1;
            uint32_t wait_sw_start                 :    1;
            uint32_t sw_start                      :    1;
            uint32_t reserved9                     :    12;
            uint32_t reserved21                    :    11;
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t rx_err_cause                  :    3;
            uint32_t decode_state                  :    3;
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } state0;
    union {
        struct {
            uint32_t encode_state                  :    3;
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } state1;
    union {
        struct {
            uint32_t tx_c0_esc_en                  :    1;
            uint32_t tx_db_esc_en                  :    1;
            uint32_t tx_11_esc_en                  :    1;
            uint32_t tx_13_esc_en                  :    1;
            uint32_t rx_c0_esc_en                  :    1;
            uint32_t rx_db_esc_en                  :    1;
            uint32_t rx_11_esc_en                  :    1;
            uint32_t rx_13_esc_en                  :    1;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } escape_conf;
    union {
        struct {
            uint32_t txfifo_timeout                :    8;
            uint32_t txfifo_timeout_shift          :    3;
            uint32_t txfifo_timeout_ena            :    1;
            uint32_t rxfifo_timeout                :    8;
            uint32_t rxfifo_timeout_shift          :    3;
            uint32_t rxfifo_timeout_ena            :    1;
            uint32_t reserved24                    :    8;
        };
        uint32_t val;
    } hung_conf;
    union {
        struct {
            uint32_t ack_num                       :    3;
            uint32_t ack_num_load                  :    1;
            uint32_t reserved4                     :    28;
        };
        uint32_t val;
    } ack_num;
    uint32_t rx_head;
    union {
        struct {
            uint32_t single_send_num               :    3;
            uint32_t single_send_en                :    1;
            uint32_t always_send_num               :    3;
            uint32_t always_send_en                :    1;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } quick_sent;
    struct {
        uint32_t word[2];
    } q_data[7];
    union {
        struct {
            uint32_t seper_char                    :    8;
            uint32_t seper_esc_char0               :    8;
            uint32_t seper_esc_char1               :    8;
            uint32_t reserved24                    :    8;
        };
        uint32_t val;
    } esc_conf0;
    union {
        struct {
            uint32_t seq0                          :    8;
            uint32_t seq0_char0                    :    8;
            uint32_t seq0_char1                    :    8;
            uint32_t reserved24                    :    8;
        };
        uint32_t val;
    } esc_conf1;
    union {
        struct {
            uint32_t seq1                          :    8;
            uint32_t seq1_char0                    :    8;
            uint32_t seq1_char1                    :    8;
            uint32_t reserved24                    :    8;
        };
        uint32_t val;
    } esc_conf2;
    union {
        struct {
            uint32_t seq2                          :    8;
            uint32_t seq2_char0                    :    8;
            uint32_t seq2_char1                    :    8;
            uint32_t reserved24                    :    8;
        };
        uint32_t val;
    } esc_conf3;
    union {
        struct {
            uint32_t thrs                          :    13;
            uint32_t reserved13                    :    19;
        };
        uint32_t val;
    } pkt_thres;
    uint32_t date;
} uhci_dev_t;
extern uhci_dev_t UHCI0;
extern uhci_dev_t UHCI1;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_UHCI_STRUCT_H_ */
