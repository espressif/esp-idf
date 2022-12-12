/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Register */
/** Type of conf0 register
 *  a
 */
typedef union {
    struct {
        /** tx_rst : R/W; bitpos: [0]; default: 0;
         *  Write 1 then write 0 to this bit to reset decode state machine.
         */
        uint32_t tx_rst:1;
        /** rx_rst : R/W; bitpos: [1]; default: 0;
         *  Write 1 then write 0 to this bit to reset encode state machine.
         */
        uint32_t rx_rst:1;
        /** uart0_ce : R/W; bitpos: [2]; default: 0;
         *  Set this bit to link up HCI and UART0.
         */
        uint32_t uart0_ce:1;
        /** uart1_ce : R/W; bitpos: [3]; default: 0;
         *  Set this bit to link up HCI and UART1.
         */
        uint32_t uart1_ce:1;
        uint32_t reserved_4:1;
        /** seper_en : R/W; bitpos: [5]; default: 1;
         *  Set this bit to separate the data frame using a special char.
         */
        uint32_t seper_en:1;
        /** head_en : R/W; bitpos: [6]; default: 1;
         *  Set this bit to encode the data packet with a formatting header.
         */
        uint32_t head_en:1;
        /** crc_rec_en : R/W; bitpos: [7]; default: 1;
         *  Set this bit to enable UHCI to receive the 16 bit CRC.
         */
        uint32_t crc_rec_en:1;
        /** uart_idle_eof_en : R/W; bitpos: [8]; default: 0;
         *  If this bit is set to 1 UHCI will end the payload receiving process when UART has
         *  been in idle state.
         */
        uint32_t uart_idle_eof_en:1;
        /** len_eof_en : R/W; bitpos: [9]; default: 1;
         *  If this bit is set to 1 UHCI decoder receiving payload data is end when the
         *  receiving byte count has reached the specified value. The value is payload length
         *  indicated by UHCI packet header when UHCI_HEAD_EN is 1 or the value is
         *  configuration value when UHCI_HEAD_EN is 0. If this bit is set to 0 UHCI decoder
         *  receiving payload data is end when 0xc0 is received.
         */
        uint32_t len_eof_en:1;
        /** encode_crc_en : R/W; bitpos: [10]; default: 1;
         *  Set this bit to enable data integrity checking by appending a 16 bit CCITT-CRC to
         *  end of the payload.
         */
        uint32_t encode_crc_en:1;
        /** clk_en : R/W; bitpos: [11]; default: 0;
         *  1'b1: Force clock on for register. 1'b0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
        /** uart_rx_brk_eof_en : R/W; bitpos: [12]; default: 0;
         *  If this bit is set to 1 UHCI will end payload receive process when NULL frame is
         *  received by UART.
         */
        uint32_t uart_rx_brk_eof_en:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} uhci_conf0_reg_t;

/** Type of conf1 register
 *  a
 */
typedef union {
    struct {
        /** check_sum_en : R/W; bitpos: [0]; default: 1;
         *  a
         */
        uint32_t check_sum_en:1;
        /** check_seq_en : R/W; bitpos: [1]; default: 1;
         *  a
         */
        uint32_t check_seq_en:1;
        /** crc_disable : R/W; bitpos: [2]; default: 0;
         *  a
         */
        uint32_t crc_disable:1;
        /** save_head : R/W; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t save_head:1;
        /** tx_check_sum_re : R/W; bitpos: [4]; default: 1;
         *  a
         */
        uint32_t tx_check_sum_re:1;
        /** tx_ack_num_re : R/W; bitpos: [5]; default: 1;
         *  a
         */
        uint32_t tx_ack_num_re:1;
        uint32_t reserved_6:1;
        /** wait_sw_start : R/W; bitpos: [7]; default: 0;
         *  a
         */
        uint32_t wait_sw_start:1;
        /** sw_start : WT; bitpos: [8]; default: 0;
         *  a
         */
        uint32_t sw_start:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_conf1_reg_t;

/** Type of escape_conf register
 *  a
 */
typedef union {
    struct {
        /** tx_c0_esc_en : R/W; bitpos: [0]; default: 1;
         *  a
         */
        uint32_t tx_c0_esc_en:1;
        /** tx_db_esc_en : R/W; bitpos: [1]; default: 1;
         *  a
         */
        uint32_t tx_db_esc_en:1;
        /** tx_11_esc_en : R/W; bitpos: [2]; default: 0;
         *  a
         */
        uint32_t tx_11_esc_en:1;
        /** tx_13_esc_en : R/W; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t tx_13_esc_en:1;
        /** rx_c0_esc_en : R/W; bitpos: [4]; default: 1;
         *  a
         */
        uint32_t rx_c0_esc_en:1;
        /** rx_db_esc_en : R/W; bitpos: [5]; default: 1;
         *  a
         */
        uint32_t rx_db_esc_en:1;
        /** rx_11_esc_en : R/W; bitpos: [6]; default: 0;
         *  a
         */
        uint32_t rx_11_esc_en:1;
        /** rx_13_esc_en : R/W; bitpos: [7]; default: 0;
         *  a
         */
        uint32_t rx_13_esc_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} uhci_escape_conf_reg_t;

/** Type of hung_conf register
 *  a
 */
typedef union {
    struct {
        /** txfifo_timeout : R/W; bitpos: [7:0]; default: 16;
         *  a
         */
        uint32_t txfifo_timeout:8;
        /** txfifo_timeout_shift : R/W; bitpos: [10:8]; default: 0;
         *  a
         */
        uint32_t txfifo_timeout_shift:3;
        /** txfifo_timeout_ena : R/W; bitpos: [11]; default: 1;
         *  a
         */
        uint32_t txfifo_timeout_ena:1;
        /** rxfifo_timeout : R/W; bitpos: [19:12]; default: 16;
         *  a
         */
        uint32_t rxfifo_timeout:8;
        /** rxfifo_timeout_shift : R/W; bitpos: [22:20]; default: 0;
         *  a
         */
        uint32_t rxfifo_timeout_shift:3;
        /** rxfifo_timeout_ena : R/W; bitpos: [23]; default: 1;
         *  a
         */
        uint32_t rxfifo_timeout_ena:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_hung_conf_reg_t;

/** Type of ack_num register
 *  a
 */
typedef union {
    struct {
        /** ack_num : R/W; bitpos: [2:0]; default: 0;
         *  a
         */
        uint32_t ack_num:3;
        /** ack_num_load : WT; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t ack_num_load:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} uhci_ack_num_reg_t;

/** Type of quick_sent register
 *  a
 */
typedef union {
    struct {
        /** single_send_num : R/W; bitpos: [2:0]; default: 0;
         *  a
         */
        uint32_t single_send_num:3;
        /** single_send_en : WT; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t single_send_en:1;
        /** always_send_num : R/W; bitpos: [6:4]; default: 0;
         *  a
         */
        uint32_t always_send_num:3;
        /** always_send_en : R/W; bitpos: [7]; default: 0;
         *  a
         */
        uint32_t always_send_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} uhci_quick_sent_reg_t;

/** Type of reg_qn_word0 register
 *  a
 */
typedef union {
    struct {
        /** send_word0 : R/W; bitpos: [31:0]; default: 0;
         *  a
         */
        uint32_t send_word0:32;
    };
    uint32_t val;
} uhci_reg_qn_word0_reg_t;

/** Type of reg_qn_word1 register
 *  a
 */
typedef union {
    struct {
        /** send_word1 : R/W; bitpos: [31:0]; default: 0;
         *  a
         */
        uint32_t send_word1:32;
    };
    uint32_t val;
} uhci_reg_qn_word1_reg_t;

/** Type of esc_conf0 register
 *  a
 */
typedef union {
    struct {
        /** seper_char : R/W; bitpos: [7:0]; default: 192;
         *  a
         */
        uint32_t seper_char:8;
        /** seper_esc_char0 : R/W; bitpos: [15:8]; default: 219;
         *  a
         */
        uint32_t seper_esc_char0:8;
        /** seper_esc_char1 : R/W; bitpos: [23:16]; default: 220;
         *  a
         */
        uint32_t seper_esc_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf0_reg_t;

/** Type of esc_conf1 register
 *  a
 */
typedef union {
    struct {
        /** esc_seq0 : R/W; bitpos: [7:0]; default: 219;
         *  a
         */
        uint32_t esc_seq0:8;
        /** esc_seq0_char0 : R/W; bitpos: [15:8]; default: 219;
         *  a
         */
        uint32_t esc_seq0_char0:8;
        /** esc_seq0_char1 : R/W; bitpos: [23:16]; default: 221;
         *  a
         */
        uint32_t esc_seq0_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf1_reg_t;

/** Type of esc_conf2 register
 *  a
 */
typedef union {
    struct {
        /** esc_seq1 : R/W; bitpos: [7:0]; default: 17;
         *  a
         */
        uint32_t esc_seq1:8;
        /** esc_seq1_char0 : R/W; bitpos: [15:8]; default: 219;
         *  a
         */
        uint32_t esc_seq1_char0:8;
        /** esc_seq1_char1 : R/W; bitpos: [23:16]; default: 222;
         *  a
         */
        uint32_t esc_seq1_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf2_reg_t;

/** Type of esc_conf3 register
 *  a
 */
typedef union {
    struct {
        /** esc_seq2 : R/W; bitpos: [7:0]; default: 19;
         *  a
         */
        uint32_t esc_seq2:8;
        /** esc_seq2_char0 : R/W; bitpos: [15:8]; default: 219;
         *  a
         */
        uint32_t esc_seq2_char0:8;
        /** esc_seq2_char1 : R/W; bitpos: [23:16]; default: 223;
         *  a
         */
        uint32_t esc_seq2_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf3_reg_t;

/** Type of pkt_thres register
 *  a
 */
typedef union {
    struct {
        /** pkt_thrs : R/W; bitpos: [12:0]; default: 128;
         *  a
         */
        uint32_t pkt_thrs:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} uhci_pkt_thres_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  a
 */
typedef union {
    struct {
        /** rx_start_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  a
         */
        uint32_t rx_start_int_raw:1;
        /** tx_start_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  a
         */
        uint32_t tx_start_int_raw:1;
        /** rx_hung_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  a
         */
        uint32_t rx_hung_int_raw:1;
        /** tx_hung_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t tx_hung_int_raw:1;
        /** send_s_reg_q_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  a
         */
        uint32_t send_s_reg_q_int_raw:1;
        /** send_a_reg_q_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  a
         */
        uint32_t send_a_reg_q_int_raw:1;
        /** out_eof_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This is the interrupt raw bit. Triggered when there are some errors in EOF in the
         */
        uint32_t out_eof_int_raw:1;
        /** app_ctrl0_int_raw : R/W; bitpos: [7]; default: 0;
         *  Soft control int raw bit.
         */
        uint32_t app_ctrl0_int_raw:1;
        /** app_ctrl1_int_raw : R/W; bitpos: [8]; default: 0;
         *  Soft control int raw bit.
         */
        uint32_t app_ctrl1_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_raw_reg_t;

/** Type of int_st register
 *  a
 */
typedef union {
    struct {
        /** rx_start_int_st : RO; bitpos: [0]; default: 0;
         *  a
         */
        uint32_t rx_start_int_st:1;
        /** tx_start_int_st : RO; bitpos: [1]; default: 0;
         *  a
         */
        uint32_t tx_start_int_st:1;
        /** rx_hung_int_st : RO; bitpos: [2]; default: 0;
         *  a
         */
        uint32_t rx_hung_int_st:1;
        /** tx_hung_int_st : RO; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t tx_hung_int_st:1;
        /** send_s_reg_q_int_st : RO; bitpos: [4]; default: 0;
         *  a
         */
        uint32_t send_s_reg_q_int_st:1;
        /** send_a_reg_q_int_st : RO; bitpos: [5]; default: 0;
         *  a
         */
        uint32_t send_a_reg_q_int_st:1;
        /** outlink_eof_err_int_st : RO; bitpos: [6]; default: 0;
         *  a
         */
        uint32_t outlink_eof_err_int_st:1;
        /** app_ctrl0_int_st : RO; bitpos: [7]; default: 0;
         *  a
         */
        uint32_t app_ctrl0_int_st:1;
        /** app_ctrl1_int_st : RO; bitpos: [8]; default: 0;
         *  a
         */
        uint32_t app_ctrl1_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_st_reg_t;

/** Type of int_ena register
 *  a
 */
typedef union {
    struct {
        /** rx_start_int_ena : R/W; bitpos: [0]; default: 0;
         *  a
         */
        uint32_t rx_start_int_ena:1;
        /** tx_start_int_ena : R/W; bitpos: [1]; default: 0;
         *  a
         */
        uint32_t tx_start_int_ena:1;
        /** rx_hung_int_ena : R/W; bitpos: [2]; default: 0;
         *  a
         */
        uint32_t rx_hung_int_ena:1;
        /** tx_hung_int_ena : R/W; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t tx_hung_int_ena:1;
        /** send_s_reg_q_int_ena : R/W; bitpos: [4]; default: 0;
         *  a
         */
        uint32_t send_s_reg_q_int_ena:1;
        /** send_a_reg_q_int_ena : R/W; bitpos: [5]; default: 0;
         *  a
         */
        uint32_t send_a_reg_q_int_ena:1;
        /** outlink_eof_err_int_ena : R/W; bitpos: [6]; default: 0;
         *  a
         */
        uint32_t outlink_eof_err_int_ena:1;
        /** app_ctrl0_int_ena : R/W; bitpos: [7]; default: 0;
         *  a
         */
        uint32_t app_ctrl0_int_ena:1;
        /** app_ctrl1_int_ena : R/W; bitpos: [8]; default: 0;
         *  a
         */
        uint32_t app_ctrl1_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_ena_reg_t;

/** Type of int_clr register
 *  a
 */
typedef union {
    struct {
        /** rx_start_int_clr : WT; bitpos: [0]; default: 0;
         *  a
         */
        uint32_t rx_start_int_clr:1;
        /** tx_start_int_clr : WT; bitpos: [1]; default: 0;
         *  a
         */
        uint32_t tx_start_int_clr:1;
        /** rx_hung_int_clr : WT; bitpos: [2]; default: 0;
         *  a
         */
        uint32_t rx_hung_int_clr:1;
        /** tx_hung_int_clr : WT; bitpos: [3]; default: 0;
         *  a
         */
        uint32_t tx_hung_int_clr:1;
        /** send_s_reg_q_int_clr : WT; bitpos: [4]; default: 0;
         *  a
         */
        uint32_t send_s_reg_q_int_clr:1;
        /** send_a_reg_q_int_clr : WT; bitpos: [5]; default: 0;
         *  a
         */
        uint32_t send_a_reg_q_int_clr:1;
        /** outlink_eof_err_int_clr : WT; bitpos: [6]; default: 0;
         *  a
         */
        uint32_t outlink_eof_err_int_clr:1;
        /** app_ctrl0_int_clr : WT; bitpos: [7]; default: 0;
         *  a
         */
        uint32_t app_ctrl0_int_clr:1;
        /** app_ctrl1_int_clr : WT; bitpos: [8]; default: 0;
         *  a
         */
        uint32_t app_ctrl1_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_clr_reg_t;


/** Group: UHCI Status Register */
/** Type of state0 register
 *  a
 */
typedef union {
    struct {
        /** rx_err_cause : RO; bitpos: [2:0]; default: 0;
         *  a
         */
        uint32_t rx_err_cause:3;
        /** decode_state : RO; bitpos: [5:3]; default: 0;
         *  a
         */
        uint32_t decode_state:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} uhci_state0_reg_t;

/** Type of state1 register
 *  a
 */
typedef union {
    struct {
        /** encode_state : RO; bitpos: [2:0]; default: 0;
         *  a
         */
        uint32_t encode_state:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} uhci_state1_reg_t;

/** Type of rx_head register
 *  a
 */
typedef union {
    struct {
        /** rx_head : RO; bitpos: [31:0]; default: 0;
         *  a
         */
        uint32_t rx_head:32;
    };
    uint32_t val;
} uhci_rx_head_reg_t;


/** Group: Version Register */
/** Type of date register
 *  a
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 35655936;
         *  a
         */
        uint32_t date:32;
    };
    uint32_t val;
} uhci_date_reg_t;


typedef struct uhci_dev_s {
    volatile uhci_conf0_reg_t conf0;
    volatile uhci_int_raw_reg_t int_raw;
    volatile uhci_int_st_reg_t int_st;
    volatile uhci_int_ena_reg_t int_ena;
    volatile uhci_int_clr_reg_t int_clr;
    volatile uhci_conf1_reg_t conf1;
    volatile uhci_state0_reg_t state0;
    volatile uhci_state1_reg_t state1;
    volatile uhci_escape_conf_reg_t escape_conf;
    volatile uhci_hung_conf_reg_t hung_conf;
    volatile uhci_ack_num_reg_t ack_num;
    volatile uhci_rx_head_reg_t rx_head;
    volatile uhci_quick_sent_reg_t quick_sent;
    volatile struct {
        uhci_reg_qn_word0_reg_t word0;
        uhci_reg_qn_word1_reg_t word1;
    } q_data[7];
    volatile uhci_esc_conf0_reg_t esc_conf0;
    volatile uhci_esc_conf1_reg_t esc_conf1;
    volatile uhci_esc_conf2_reg_t esc_conf2;
    volatile uhci_esc_conf3_reg_t esc_conf3;
    volatile uhci_pkt_thres_reg_t pkt_thres;
    volatile uhci_date_reg_t date;
} uhci_dev_t;

extern uhci_dev_t UHCI0;

#ifndef __cplusplus
_Static_assert(sizeof(uhci_dev_t) == 0x84, "Invalid size of uhci_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
