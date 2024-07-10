/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 *  UHCI configuration register
 */
typedef union {
    struct {
        /** tx_rst : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then write 0 to reset the decoder state machine.
         */
        uint32_t tx_rst:1;
        /** rx_rst : R/W; bitpos: [1]; default: 0;
         *  Write 1 and then write 0 to reset the encoder state machine.
         */
        uint32_t rx_rst:1;
        /** uart_sel : R/W; bitpos: [4:2]; default: 7;
         *  Configures to select which uart to connect with UHCI.\\
         *  0: UART0\\
         *  1: UART1\\
         */
        uint32_t uart_sel:3;
        /** seper_en : R/W; bitpos: [5]; default: 1;
         *  Configures whether or not to separate the data frame with a special character.\\
         *  0: Not separate\\
         *  1: Separate\\
         */
        uint32_t seper_en:1;
        /** head_en : R/W; bitpos: [6]; default: 1;
         *  Configures whether or not to encode the data packet with a formatting header.\\
         *  0: Not use formatting header\\
         *  1: Use formatting header\\
         */
        uint32_t head_en:1;
        /** crc_rec_en : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not to enable the reception of the 16-bit CRC.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t crc_rec_en:1;
        /** uart_idle_eof_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to stop receiving data when UART is idle.\\
         *  0: Not stop\\
         *  1: Stop\\
         */
        uint32_t uart_idle_eof_en:1;
        /** len_eof_en : R/W; bitpos: [9]; default: 1;
         *  Configures when the UHCI decoder stops receiving data.\\
         *  0: Stops after receiving 0xC0\\
         *  1: Stops when the number of received data bytes reach the specified value. When
         *  UHCI_HEAD_EN is 1, the specified value is the data length indicated by the UHCI
         *  packet header. when UHCI_HEAD_EN is 0, the specified value is the configured
         *  value.\\
         */
        uint32_t len_eof_en:1;
        /** encode_crc_en : R/W; bitpos: [10]; default: 1;
         *  Configures whether or not to enable data integrity check by appending a 16 bit
         *  CCITT-CRC to the end of the data.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t encode_crc_en:1;
        /** clk_en : R/W; bitpos: [11]; default: 0;
         *  Configures clock gating.\\
         *  0: Support clock only when the application writes registers.\\
         *  1: Always force the clock on for registers.\\
         */
        uint32_t clk_en:1;
        /** uart_rx_brk_eof_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to stop UHCI from receiving data after UART has received
         *  a NULL frame.\\
         *  0: Not stop\\
         *  1: Stop\\
         */
        uint32_t uart_rx_brk_eof_en:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} uhci_conf0_reg_t;

/** Type of conf1 register
 *  UHCI configuration register
 */
typedef union {
    struct {
        /** check_sum_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to enable header checksum validation when UHCI receives a
         *  data packet.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t check_sum_en:1;
        /** check_seq_en : R/W; bitpos: [1]; default: 1;
         *  Configures whether or not to enable the sequence number check when UHCI receives a
         *  data packet.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t check_seq_en:1;
        /** crc_disable : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable CRC calculation.\\
         *  0: Disable\\
         *  1: Enable\\
         *  Valid only when the Data Integrity Check Present bit in UHCI packet is 1.\\
         */
        uint32_t crc_disable:1;
        /** save_head : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to save the packet header when UHCI receives a data
         *  packet.\\
         *  0: Not save\\
         *  1: Save\\
         */
        uint32_t save_head:1;
        /** tx_check_sum_re : R/W; bitpos: [4]; default: 1;
         *  Configures whether or not to encode the data packet with a checksum.\\
         *  0: Not use checksum\\
         *  1: Use checksum\\
         */
        uint32_t tx_check_sum_re:1;
        /** tx_ack_num_re : R/W; bitpos: [5]; default: 1;
         *  Configures whether or not to encode the data packet with an acknowledgment when a
         *  reliable packet is to be transmitted.\\
         *  0: Not use acknowledgement\\
         *  1: Use acknowledgement\\
         */
        uint32_t tx_ack_num_re:1;
        uint32_t reserved_6:1;
        /** wait_sw_start : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to put the UHCI encoder state machine to ST_SW_WAIT
         *  state.\\
         *  0: No\\
         *  1: Yes\\
         */
        uint32_t wait_sw_start:1;
        /** sw_start : WT; bitpos: [8]; default: 0;
         *  Write 1 to send data packets when the encoder state machine is in ST_SW_WAIT state.
         */
        uint32_t sw_start:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_conf1_reg_t;

/** Type of escape_conf register
 *  Escape character configuration
 */
typedef union {
    struct {
        /** tx_c0_esc_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to decode character 0xC0 when DMA receives data.\\
         *  0: Not decode\\
         *  1: Decode\\
         */
        uint32_t tx_c0_esc_en:1;
        /** tx_db_esc_en : R/W; bitpos: [1]; default: 1;
         *  Configures whether or not to decode character 0xDB when DMA receives data.\\
         *  0: Not decode\\
         *  1: Decode\\
         */
        uint32_t tx_db_esc_en:1;
        /** tx_11_esc_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to decode flow control character 0x11 when DMA receives
         *  data.\\
         *  0: Not decode\\
         *  1: Decode\\
         */
        uint32_t tx_11_esc_en:1;
        /** tx_13_esc_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to decode flow control character 0x13 when DMA receives
         *  data.\\
         *  0: Not decode\\
         *  1: Decode\\
         */
        uint32_t tx_13_esc_en:1;
        /** rx_c0_esc_en : R/W; bitpos: [4]; default: 1;
         *  Configures whether or not to replace 0xC0 by special characters when DMA sends
         *  data.\\
         *  0: Not replace\\
         *  1: Replace\\
         */
        uint32_t rx_c0_esc_en:1;
        /** rx_db_esc_en : R/W; bitpos: [5]; default: 1;
         *  Configures whether or not to replace 0xDB by special characters when DMA sends
         *  data.\\
         *  0: Not replace\\
         *  1: Replace\\
         */
        uint32_t rx_db_esc_en:1;
        /** rx_11_esc_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to replace flow control character 0x11 by special
         *  characters when DMA sends data.\\
         *  0: Not replace\\
         *  1: Replace\\
         */
        uint32_t rx_11_esc_en:1;
        /** rx_13_esc_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to replace flow control character 0x13 by special
         *  characters when DMA sends data.\\
         *  0: Not replace\\
         *  1: Replace\\
         */
        uint32_t rx_13_esc_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} uhci_escape_conf_reg_t;

/** Type of hung_conf register
 *  Timeout configuration
 */
typedef union {
    struct {
        /** txfifo_timeout : R/W; bitpos: [7:0]; default: 16;
         *  Configures the timeout value for DMA data reception.\\Measurement unit: ms.
         */
        uint32_t txfifo_timeout:8;
        /** txfifo_timeout_shift : R/W; bitpos: [10:8]; default: 0;
         *  Configures the upper limit of the timeout counter for TX FIFO.
         */
        uint32_t txfifo_timeout_shift:3;
        /** txfifo_timeout_ena : R/W; bitpos: [11]; default: 1;
         *  Configures whether or not to enable the data reception timeout for TX FIFO.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t txfifo_timeout_ena:1;
        /** rxfifo_timeout : R/W; bitpos: [19:12]; default: 16;
         *  Configures the timeout value for DMA to read data from RAM.\\Measurement unit: ms.
         */
        uint32_t rxfifo_timeout:8;
        /** rxfifo_timeout_shift : R/W; bitpos: [22:20]; default: 0;
         *  Configures the upper limit of the timeout counter for RX FIFO.
         */
        uint32_t rxfifo_timeout_shift:3;
        /** rxfifo_timeout_ena : R/W; bitpos: [23]; default: 1;
         *  Configures whether or not to enable the DMA data transmission timeout.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t rxfifo_timeout_ena:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_hung_conf_reg_t;

/** Type of ack_num register
 *  UHCI ACK number configuration
 */
typedef union {
    struct {
        /** ack_num : R/W; bitpos: [2:0]; default: 0;
         *  Configures the number of acknowledgements used in software flow control.
         */
        uint32_t ack_num:3;
        /** ack_num_load : WT; bitpos: [3]; default: 0;
         *  Configures whether or not load acknowledgements.\\
         *  0: Not load\\
         *  1: Load\\
         */
        uint32_t ack_num_load:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} uhci_ack_num_reg_t;

/** Type of quick_sent register
 *  UHCI quick send configuration register
 */
typedef union {
    struct {
        /** single_send_num : R/W; bitpos: [2:0]; default: 0;
         *  Configures the source of data to be transmitted in single_send mode.\\
         *  0: Q0 register\\
         *  1: Q1 register\\
         *  2: Q2 register\\
         *  3: Q3 register\\
         *  4: Q4 register\\
         *  5: Q5 register\\
         *  6: Q6 register\\
         *  7: Invalid. No effect\\
         */
        uint32_t single_send_num:3;
        /** single_send_en : WT; bitpos: [3]; default: 0;
         *  Write 1 to enable single_send mode.
         */
        uint32_t single_send_en:1;
        /** always_send_num : R/W; bitpos: [6:4]; default: 0;
         *  Configures the source of data to be transmitted in always_send mode.\\
         *  0: Q0 register\\
         *  1: Q1 register\\
         *  2: Q2 register\\
         *  3: Q3 register\\
         *  4: Q4 register\\
         *  5: Q5 register\\
         *  6: Q6 register\\
         *  7: Invalid. No effect\\
         */
        uint32_t always_send_num:3;
        /** always_send_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable always_send mode.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t always_send_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} uhci_quick_sent_reg_t;

/** Type of reg_q0_word0 register
 *  Q0 WORD0 quick send register
 */
typedef union {
    struct {
        /** send_q0_word0 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q0 register.
         */
        uint32_t send_q0_word0:32;
    };
    uint32_t val;
} uhci_reg_q0_word0_reg_t;

/** Type of reg_q0_word1 register
 *  Q0 WORD1 quick send register
 */
typedef union {
    struct {
        /** send_q0_word1 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q0 register.
         */
        uint32_t send_q0_word1:32;
    };
    uint32_t val;
} uhci_reg_q0_word1_reg_t;

/** Type of reg_q1_word0 register
 *  Q1 WORD0 quick send register
 */
typedef union {
    struct {
        /** send_q1_word0 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q1 register.
         */
        uint32_t send_q1_word0:32;
    };
    uint32_t val;
} uhci_reg_q1_word0_reg_t;

/** Type of reg_q1_word1 register
 *  Q1 WORD1 quick send register
 */
typedef union {
    struct {
        /** send_q1_word1 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q1 register.
         */
        uint32_t send_q1_word1:32;
    };
    uint32_t val;
} uhci_reg_q1_word1_reg_t;

/** Type of reg_q2_word0 register
 *  Q2 WORD0 quick send register
 */
typedef union {
    struct {
        /** send_q2_word0 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q2 register.
         */
        uint32_t send_q2_word0:32;
    };
    uint32_t val;
} uhci_reg_q2_word0_reg_t;

/** Type of reg_q2_word1 register
 *  Q2 WORD1 quick send register
 */
typedef union {
    struct {
        /** send_q2_word1 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q2 register.
         */
        uint32_t send_q2_word1:32;
    };
    uint32_t val;
} uhci_reg_q2_word1_reg_t;

/** Type of reg_q3_word0 register
 *  Q3 WORD0 quick send register
 */
typedef union {
    struct {
        /** send_q3_word0 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q3 register.
         */
        uint32_t send_q3_word0:32;
    };
    uint32_t val;
} uhci_reg_q3_word0_reg_t;

/** Type of reg_q3_word1 register
 *  Q3 WORD1 quick send register
 */
typedef union {
    struct {
        /** send_q3_word1 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q3 register.
         */
        uint32_t send_q3_word1:32;
    };
    uint32_t val;
} uhci_reg_q3_word1_reg_t;

/** Type of reg_q4_word0 register
 *  Q4 WORD0 quick send register
 */
typedef union {
    struct {
        /** send_q4_word0 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q4 register.
         */
        uint32_t send_q4_word0:32;
    };
    uint32_t val;
} uhci_reg_q4_word0_reg_t;

/** Type of reg_q4_word1 register
 *  Q4 WORD1 quick send register
 */
typedef union {
    struct {
        /** send_q4_word1 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q4 register.
         */
        uint32_t send_q4_word1:32;
    };
    uint32_t val;
} uhci_reg_q4_word1_reg_t;

/** Type of reg_q5_word0 register
 *  Q5 WORD0 quick send register
 */
typedef union {
    struct {
        /** send_q5_word0 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q5 register.
         */
        uint32_t send_q5_word0:32;
    };
    uint32_t val;
} uhci_reg_q5_word0_reg_t;

/** Type of reg_q5_word1 register
 *  Q5 WORD1 quick send register
 */
typedef union {
    struct {
        /** send_q5_word1 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q5 register.
         */
        uint32_t send_q5_word1:32;
    };
    uint32_t val;
} uhci_reg_q5_word1_reg_t;

/** Type of reg_q6_word0 register
 *  Q6 WORD0 quick send register
 */
typedef union {
    struct {
        /** send_q6_word0 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q6 register.
         */
        uint32_t send_q6_word0:32;
    };
    uint32_t val;
} uhci_reg_q6_word0_reg_t;

/** Type of reg_q6_word1 register
 *  Q6 WORD1 quick register
 */
typedef union {
    struct {
        /** send_q6_word1 : R/W; bitpos: [31:0]; default: 0;
         *  Data to be transmitted in Q6 register.
         */
        uint32_t send_q6_word1:32;
    };
    uint32_t val;
} uhci_reg_q6_word1_reg_t;

/** Type of esc_conf0 register
 *  Escape sequence configuration register 0
 */
typedef union {
    struct {
        /** seper_char : R/W; bitpos: [7:0]; default: 192;
         *  Configures separators to encode data packets. The default value is 0xC0.
         */
        uint32_t seper_char:8;
        /** seper_esc_char0 : R/W; bitpos: [15:8]; default: 219;
         *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
         */
        uint32_t seper_esc_char0:8;
        /** seper_esc_char1 : R/W; bitpos: [23:16]; default: 220;
         *  Configures the second character of SLIP escape sequence. The default value is 0xDC.
         */
        uint32_t seper_esc_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf0_reg_t;

/** Type of esc_conf1 register
 *  Escape sequence configuration register 1
 */
typedef union {
    struct {
        /** esc_seq0 : R/W; bitpos: [7:0]; default: 219;
         *  Configures the character that needs to be encoded. The default value is 0xDB used
         *  as the first character of SLIP escape sequence.
         */
        uint32_t esc_seq0:8;
        /** esc_seq0_char0 : R/W; bitpos: [15:8]; default: 219;
         *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
         */
        uint32_t esc_seq0_char0:8;
        /** esc_seq0_char1 : R/W; bitpos: [23:16]; default: 221;
         *  Configures the second character of SLIP escape sequence. The default value is 0xDD.
         */
        uint32_t esc_seq0_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf1_reg_t;

/** Type of esc_conf2 register
 *  Escape sequence configuration register 2
 */
typedef union {
    struct {
        /** esc_seq1 : R/W; bitpos: [7:0]; default: 17;
         *  Configures a character that need to be encoded. The default value is 0x11 used as a
         *  flow control character.
         */
        uint32_t esc_seq1:8;
        /** esc_seq1_char0 : R/W; bitpos: [15:8]; default: 219;
         *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
         */
        uint32_t esc_seq1_char0:8;
        /** esc_seq1_char1 : R/W; bitpos: [23:16]; default: 222;
         *  Configures the second character of SLIP escape sequence. The default value is 0xDE.
         */
        uint32_t esc_seq1_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf2_reg_t;

/** Type of esc_conf3 register
 *  Escape sequence configuration register 3
 */
typedef union {
    struct {
        /** esc_seq2 : R/W; bitpos: [7:0]; default: 19;
         *  Configures the character that needs to be decoded. The default value is 0x13 used
         *  as a flow control character.
         */
        uint32_t esc_seq2:8;
        /** esc_seq2_char0 : R/W; bitpos: [15:8]; default: 219;
         *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
         */
        uint32_t esc_seq2_char0:8;
        /** esc_seq2_char1 : R/W; bitpos: [23:16]; default: 223;
         *  Configures the second character of SLIP escape sequence. The default value is 0xDF.
         */
        uint32_t esc_seq2_char1:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uhci_esc_conf3_reg_t;

/** Type of pkt_thres register
 *  Configuration register for packet length
 */
typedef union {
    struct {
        /** pkt_thrs : R/W; bitpos: [12:0]; default: 128;
         *  Configures the maximum value of the packet length.\\Measurement unit: byte.\\Valid
         *  only when UHCI_HEAD_EN is 0.
         */
        uint32_t pkt_thrs:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} uhci_pkt_thres_reg_t;


/** Group: UHCI Interrupt Register */
/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** rx_start_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of UHCI_RX_START_INT.
         */
        uint32_t rx_start_int_raw:1;
        /** tx_start_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of UHCI_TX_START_INT.
         */
        uint32_t tx_start_int_raw:1;
        /** rx_hung_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of UHCI_RX_HUNG_INT.
         */
        uint32_t rx_hung_int_raw:1;
        /** tx_hung_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of UHCI_TX_HUNG_INT.
         */
        uint32_t tx_hung_int_raw:1;
        /** send_s_reg_q_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of UHCI_SEND_S_REG_Q_INT.
         */
        uint32_t send_s_reg_q_int_raw:1;
        /** send_a_reg_q_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of UHCI_SEND_A_REG_Q_INT.
         */
        uint32_t send_a_reg_q_int_raw:1;
        /** out_eof_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of UHCI_OUT_EOF_INT.
         */
        uint32_t out_eof_int_raw:1;
        /** app_ctrl0_int_raw : R/W; bitpos: [7]; default: 0;
         *  The raw interrupt status of UHCI_APP_CTRL0_INT.
         */
        uint32_t app_ctrl0_int_raw:1;
        /** app_ctrl1_int_raw : R/W; bitpos: [8]; default: 0;
         *  The raw interrupt status of UHCI_APP_CTRL1_INT.
         */
        uint32_t app_ctrl1_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** rx_start_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of UHCI_RX_START_INT.
         */
        uint32_t rx_start_int_st:1;
        /** tx_start_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of UHCI_TX_START_INT.
         */
        uint32_t tx_start_int_st:1;
        /** rx_hung_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of UHCI_RX_HUNG_INT.
         */
        uint32_t rx_hung_int_st:1;
        /** tx_hung_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of UHCI_TX_HUNG_INT.
         */
        uint32_t tx_hung_int_st:1;
        /** send_s_reg_q_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of UHCI_SEND_S_REG_Q_INT.
         */
        uint32_t send_s_reg_q_int_st:1;
        /** send_a_reg_q_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of UHCI_SEND_A_REG_Q_INT.
         */
        uint32_t send_a_reg_q_int_st:1;
        /** outlink_eof_err_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of UHCI_OUTLINK_EOF_ERR_INT.
         */
        uint32_t outlink_eof_err_int_st:1;
        /** app_ctrl0_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status of UHCI_APP_CTRL0_INT.
         */
        uint32_t app_ctrl0_int_st:1;
        /** app_ctrl1_int_st : RO; bitpos: [8]; default: 0;
         *  The masked interrupt status of UHCI_APP_CTRL1_INT.
         */
        uint32_t app_ctrl1_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** rx_start_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable UHCI_RX_START_INT.
         */
        uint32_t rx_start_int_ena:1;
        /** tx_start_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable UHCI_TX_START_INT.
         */
        uint32_t tx_start_int_ena:1;
        /** rx_hung_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable UHCI_RX_HUNG_INT.
         */
        uint32_t rx_hung_int_ena:1;
        /** tx_hung_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable UHCI_TX_HUNG_INT.
         */
        uint32_t tx_hung_int_ena:1;
        /** send_s_reg_q_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable UHCI_SEND_S_REG_Q_INT.
         */
        uint32_t send_s_reg_q_int_ena:1;
        /** send_a_reg_q_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable UHCI_SEND_A_REG_Q_INT.
         */
        uint32_t send_a_reg_q_int_ena:1;
        /** outlink_eof_err_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable UHCI_OUTLINK_EOF_ERR_INT.
         */
        uint32_t outlink_eof_err_int_ena:1;
        /** app_ctrl0_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable UHCI_APP_CTRL0_INT.
         */
        uint32_t app_ctrl0_int_ena:1;
        /** app_ctrl1_int_ena : R/W; bitpos: [8]; default: 0;
         *  Write 1 to enable UHCI_APP_CTRL1_INT.
         */
        uint32_t app_ctrl1_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** rx_start_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear UHCI_RX_START_INT.
         */
        uint32_t rx_start_int_clr:1;
        /** tx_start_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear UHCI_TX_START_INT.
         */
        uint32_t tx_start_int_clr:1;
        /** rx_hung_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear UHCI_RX_HUNG_INT.
         */
        uint32_t rx_hung_int_clr:1;
        /** tx_hung_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear UHCI_TX_HUNG_INT.
         */
        uint32_t tx_hung_int_clr:1;
        /** send_s_reg_q_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear UHCI_SEND_S_REG_Q_INT.
         */
        uint32_t send_s_reg_q_int_clr:1;
        /** send_a_reg_q_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear UHCI_SEND_A_REG_Q_INT.
         */
        uint32_t send_a_reg_q_int_clr:1;
        /** outlink_eof_err_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear UHCI_OUTLINK_EOF_ERR_INT.
         */
        uint32_t outlink_eof_err_int_clr:1;
        /** app_ctrl0_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear UHCI_APP_CTRL0_INT.
         */
        uint32_t app_ctrl0_int_clr:1;
        /** app_ctrl1_int_clr : WT; bitpos: [8]; default: 0;
         *  Write 1 to clear UHCI_APP_CTRL1_INT.
         */
        uint32_t app_ctrl1_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uhci_int_clr_reg_t;


/** Group: UHCI Status Register */
/** Type of state0 register
 *  UHCI receive status
 */
typedef union {
    struct {
        /** rx_err_cause : RO; bitpos: [2:0]; default: 0;
         *  Represents the error type when DMA has received a packet with error.\\
         *  0: Invalid. No effect\\
         *  1: Checksum error in the HCI packet\\
         *  2: Sequence number error in the HCI packet\\
         *  3: CRC bit error in the HCI packet\\
         *  4: 0xC0 is found but the received HCI packet is not complete\
         *  5: 0xC0 is not found when the HCI packet has been received\\
         *  6: CRC check error\\
         *  7: Invalid. No effect\\
         */
        uint32_t rx_err_cause:3;
        /** decode_state : RO; bitpos: [5:3]; default: 0;
         *  Represents the UHCI decoder status.
         */
        uint32_t decode_state:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} uhci_state0_reg_t;

/** Type of state1 register
 *  UHCI transmit status
 */
typedef union {
    struct {
        /** encode_state : RO; bitpos: [2:0]; default: 0;
         *  Represents the UHCI encoder status.
         */
        uint32_t encode_state:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} uhci_state1_reg_t;

/** Type of rx_head register
 *  UHCI packet header register
 */
typedef union {
    struct {
        /** rx_head : RO; bitpos: [31:0]; default: 0;
         *  Represents the header of the current received packet.
         */
        uint32_t rx_head:32;
    };
    uint32_t val;
} uhci_rx_head_reg_t;


/** Group: Version Register */
/** Type of date register
 *  UHCI version  control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 35655936;
         *  Version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} uhci_date_reg_t;


typedef struct {
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
    volatile uhci_reg_q0_word0_reg_t reg_q0_word0;
    volatile uhci_reg_q0_word1_reg_t reg_q0_word1;
    volatile uhci_reg_q1_word0_reg_t reg_q1_word0;
    volatile uhci_reg_q1_word1_reg_t reg_q1_word1;
    volatile uhci_reg_q2_word0_reg_t reg_q2_word0;
    volatile uhci_reg_q2_word1_reg_t reg_q2_word1;
    volatile uhci_reg_q3_word0_reg_t reg_q3_word0;
    volatile uhci_reg_q3_word1_reg_t reg_q3_word1;
    volatile uhci_reg_q4_word0_reg_t reg_q4_word0;
    volatile uhci_reg_q4_word1_reg_t reg_q4_word1;
    volatile uhci_reg_q5_word0_reg_t reg_q5_word0;
    volatile uhci_reg_q5_word1_reg_t reg_q5_word1;
    volatile uhci_reg_q6_word0_reg_t reg_q6_word0;
    volatile uhci_reg_q6_word1_reg_t reg_q6_word1;
    volatile uhci_esc_conf0_reg_t esc_conf0;
    volatile uhci_esc_conf1_reg_t esc_conf1;
    volatile uhci_esc_conf2_reg_t esc_conf2;
    volatile uhci_esc_conf3_reg_t esc_conf3;
    volatile uhci_pkt_thres_reg_t pkt_thres;
    volatile uhci_date_reg_t date;
} uhci_dev_t;

extern uhci_dev_t UHCI;

#ifndef __cplusplus
_Static_assert(sizeof(uhci_dev_t) == 0x84, "Invalid size of uhci_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
