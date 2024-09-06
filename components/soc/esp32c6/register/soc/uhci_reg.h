/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** UHCI_CONF0_REG register
 *  a
 */
#define UHCI_CONF0_REG(i) (REG_UHCI_BASE(i) + 0x0)
/** UHCI_TX_RST : R/W; bitpos: [0]; default: 0;
 *  Write 1 then write 0 to this bit to reset decode state machine.
 */
#define UHCI_TX_RST    (BIT(0))
#define UHCI_TX_RST_M  (UHCI_TX_RST_V << UHCI_TX_RST_S)
#define UHCI_TX_RST_V  0x00000001U
#define UHCI_TX_RST_S  0
/** UHCI_RX_RST : R/W; bitpos: [1]; default: 0;
 *  Write 1 then write 0 to this bit to reset encode state machine.
 */
#define UHCI_RX_RST    (BIT(1))
#define UHCI_RX_RST_M  (UHCI_RX_RST_V << UHCI_RX_RST_S)
#define UHCI_RX_RST_V  0x00000001U
#define UHCI_RX_RST_S  1
/** UHCI_UART0_CE : R/W; bitpos: [2]; default: 0;
 *  Set this bit to link up HCI and UART0.
 */
#define UHCI_UART0_CE    (BIT(2))
#define UHCI_UART0_CE_M  (UHCI_UART0_CE_V << UHCI_UART0_CE_S)
#define UHCI_UART0_CE_V  0x00000001U
#define UHCI_UART0_CE_S  2
/** UHCI_UART1_CE : R/W; bitpos: [3]; default: 0;
 *  Set this bit to link up HCI and UART1.
 */
#define UHCI_UART1_CE    (BIT(3))
#define UHCI_UART1_CE_M  (UHCI_UART1_CE_V << UHCI_UART1_CE_S)
#define UHCI_UART1_CE_V  0x00000001U
#define UHCI_UART1_CE_S  3
/** UHCI_SEPER_EN : R/W; bitpos: [5]; default: 1;
 *  Set this bit to separate the data frame using a special char.
 */
#define UHCI_SEPER_EN    (BIT(5))
#define UHCI_SEPER_EN_M  (UHCI_SEPER_EN_V << UHCI_SEPER_EN_S)
#define UHCI_SEPER_EN_V  0x00000001U
#define UHCI_SEPER_EN_S  5
/** UHCI_HEAD_EN : R/W; bitpos: [6]; default: 1;
 *  Set this bit to encode the data packet with a formatting header.
 */
#define UHCI_HEAD_EN    (BIT(6))
#define UHCI_HEAD_EN_M  (UHCI_HEAD_EN_V << UHCI_HEAD_EN_S)
#define UHCI_HEAD_EN_V  0x00000001U
#define UHCI_HEAD_EN_S  6
/** UHCI_CRC_REC_EN : R/W; bitpos: [7]; default: 1;
 *  Set this bit to enable UHCI to receive the 16 bit CRC.
 */
#define UHCI_CRC_REC_EN    (BIT(7))
#define UHCI_CRC_REC_EN_M  (UHCI_CRC_REC_EN_V << UHCI_CRC_REC_EN_S)
#define UHCI_CRC_REC_EN_V  0x00000001U
#define UHCI_CRC_REC_EN_S  7
/** UHCI_UART_IDLE_EOF_EN : R/W; bitpos: [8]; default: 0;
 *  If this bit is set to 1 UHCI will end the payload receiving process when UART has
 *  been in idle state.
 */
#define UHCI_UART_IDLE_EOF_EN    (BIT(8))
#define UHCI_UART_IDLE_EOF_EN_M  (UHCI_UART_IDLE_EOF_EN_V << UHCI_UART_IDLE_EOF_EN_S)
#define UHCI_UART_IDLE_EOF_EN_V  0x00000001U
#define UHCI_UART_IDLE_EOF_EN_S  8
/** UHCI_LEN_EOF_EN : R/W; bitpos: [9]; default: 1;
 *  If this bit is set to 1 UHCI decoder receiving payload data is end when the
 *  receiving byte count has reached the specified value. The value is payload length
 *  indicated by UHCI packet header when UHCI_HEAD_EN is 1 or the value is
 *  configuration value when UHCI_HEAD_EN is 0. If this bit is set to 0 UHCI decoder
 *  receiving payload data is end when 0xc0 is received.
 */
#define UHCI_LEN_EOF_EN    (BIT(9))
#define UHCI_LEN_EOF_EN_M  (UHCI_LEN_EOF_EN_V << UHCI_LEN_EOF_EN_S)
#define UHCI_LEN_EOF_EN_V  0x00000001U
#define UHCI_LEN_EOF_EN_S  9
/** UHCI_ENCODE_CRC_EN : R/W; bitpos: [10]; default: 1;
 *  Set this bit to enable data integrity checking by appending a 16 bit CCITT-CRC to
 *  end of the payload.
 */
#define UHCI_ENCODE_CRC_EN    (BIT(10))
#define UHCI_ENCODE_CRC_EN_M  (UHCI_ENCODE_CRC_EN_V << UHCI_ENCODE_CRC_EN_S)
#define UHCI_ENCODE_CRC_EN_V  0x00000001U
#define UHCI_ENCODE_CRC_EN_S  10
/** UHCI_CLK_EN : R/W; bitpos: [11]; default: 0;
 *  1'b1: Force clock on for register. 1'b0: Support clock only when application writes
 *  registers.
 */
#define UHCI_CLK_EN    (BIT(11))
#define UHCI_CLK_EN_M  (UHCI_CLK_EN_V << UHCI_CLK_EN_S)
#define UHCI_CLK_EN_V  0x00000001U
#define UHCI_CLK_EN_S  11
/** UHCI_UART_RX_BRK_EOF_EN : R/W; bitpos: [12]; default: 0;
 *  If this bit is set to 1 UHCI will end payload receive process when NULL frame is
 *  received by UART.
 */
#define UHCI_UART_RX_BRK_EOF_EN    (BIT(12))
#define UHCI_UART_RX_BRK_EOF_EN_M  (UHCI_UART_RX_BRK_EOF_EN_V << UHCI_UART_RX_BRK_EOF_EN_S)
#define UHCI_UART_RX_BRK_EOF_EN_V  0x00000001U
#define UHCI_UART_RX_BRK_EOF_EN_S  12

/** UHCI_INT_RAW_REG register
 *  a
 */
#define UHCI_INT_RAW_REG(i) (REG_UHCI_BASE(i) + 0x4)
/** UHCI_RX_START_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  a
 */
#define UHCI_RX_START_INT_RAW    (BIT(0))
#define UHCI_RX_START_INT_RAW_M  (UHCI_RX_START_INT_RAW_V << UHCI_RX_START_INT_RAW_S)
#define UHCI_RX_START_INT_RAW_V  0x00000001U
#define UHCI_RX_START_INT_RAW_S  0
/** UHCI_TX_START_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  a
 */
#define UHCI_TX_START_INT_RAW    (BIT(1))
#define UHCI_TX_START_INT_RAW_M  (UHCI_TX_START_INT_RAW_V << UHCI_TX_START_INT_RAW_S)
#define UHCI_TX_START_INT_RAW_V  0x00000001U
#define UHCI_TX_START_INT_RAW_S  1
/** UHCI_RX_HUNG_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  a
 */
#define UHCI_RX_HUNG_INT_RAW    (BIT(2))
#define UHCI_RX_HUNG_INT_RAW_M  (UHCI_RX_HUNG_INT_RAW_V << UHCI_RX_HUNG_INT_RAW_S)
#define UHCI_RX_HUNG_INT_RAW_V  0x00000001U
#define UHCI_RX_HUNG_INT_RAW_S  2
/** UHCI_TX_HUNG_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_TX_HUNG_INT_RAW    (BIT(3))
#define UHCI_TX_HUNG_INT_RAW_M  (UHCI_TX_HUNG_INT_RAW_V << UHCI_TX_HUNG_INT_RAW_S)
#define UHCI_TX_HUNG_INT_RAW_V  0x00000001U
#define UHCI_TX_HUNG_INT_RAW_S  3
/** UHCI_SEND_S_REG_Q_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  a
 */
#define UHCI_SEND_S_REG_Q_INT_RAW    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_RAW_M  (UHCI_SEND_S_REG_Q_INT_RAW_V << UHCI_SEND_S_REG_Q_INT_RAW_S)
#define UHCI_SEND_S_REG_Q_INT_RAW_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_RAW_S  4
/** UHCI_SEND_A_REG_Q_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  a
 */
#define UHCI_SEND_A_REG_Q_INT_RAW    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_RAW_M  (UHCI_SEND_A_REG_Q_INT_RAW_V << UHCI_SEND_A_REG_Q_INT_RAW_S)
#define UHCI_SEND_A_REG_Q_INT_RAW_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_RAW_S  5
/** UHCI_OUT_EOF_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This is the interrupt raw bit. Triggered when there are some errors in EOF in the
 */
#define UHCI_OUT_EOF_INT_RAW    (BIT(6))
#define UHCI_OUT_EOF_INT_RAW_M  (UHCI_OUT_EOF_INT_RAW_V << UHCI_OUT_EOF_INT_RAW_S)
#define UHCI_OUT_EOF_INT_RAW_V  0x00000001U
#define UHCI_OUT_EOF_INT_RAW_S  6
/** UHCI_APP_CTRL0_INT_RAW : R/W; bitpos: [7]; default: 0;
 *  Soft control int raw bit.
 */
#define UHCI_APP_CTRL0_INT_RAW    (BIT(7))
#define UHCI_APP_CTRL0_INT_RAW_M  (UHCI_APP_CTRL0_INT_RAW_V << UHCI_APP_CTRL0_INT_RAW_S)
#define UHCI_APP_CTRL0_INT_RAW_V  0x00000001U
#define UHCI_APP_CTRL0_INT_RAW_S  7
/** UHCI_APP_CTRL1_INT_RAW : R/W; bitpos: [8]; default: 0;
 *  Soft control int raw bit.
 */
#define UHCI_APP_CTRL1_INT_RAW    (BIT(8))
#define UHCI_APP_CTRL1_INT_RAW_M  (UHCI_APP_CTRL1_INT_RAW_V << UHCI_APP_CTRL1_INT_RAW_S)
#define UHCI_APP_CTRL1_INT_RAW_V  0x00000001U
#define UHCI_APP_CTRL1_INT_RAW_S  8

/** UHCI_INT_ST_REG register
 *  a
 */
#define UHCI_INT_ST_REG(i) (REG_UHCI_BASE(i) + 0x8)
/** UHCI_RX_START_INT_ST : RO; bitpos: [0]; default: 0;
 *  a
 */
#define UHCI_RX_START_INT_ST    (BIT(0))
#define UHCI_RX_START_INT_ST_M  (UHCI_RX_START_INT_ST_V << UHCI_RX_START_INT_ST_S)
#define UHCI_RX_START_INT_ST_V  0x00000001U
#define UHCI_RX_START_INT_ST_S  0
/** UHCI_TX_START_INT_ST : RO; bitpos: [1]; default: 0;
 *  a
 */
#define UHCI_TX_START_INT_ST    (BIT(1))
#define UHCI_TX_START_INT_ST_M  (UHCI_TX_START_INT_ST_V << UHCI_TX_START_INT_ST_S)
#define UHCI_TX_START_INT_ST_V  0x00000001U
#define UHCI_TX_START_INT_ST_S  1
/** UHCI_RX_HUNG_INT_ST : RO; bitpos: [2]; default: 0;
 *  a
 */
#define UHCI_RX_HUNG_INT_ST    (BIT(2))
#define UHCI_RX_HUNG_INT_ST_M  (UHCI_RX_HUNG_INT_ST_V << UHCI_RX_HUNG_INT_ST_S)
#define UHCI_RX_HUNG_INT_ST_V  0x00000001U
#define UHCI_RX_HUNG_INT_ST_S  2
/** UHCI_TX_HUNG_INT_ST : RO; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_TX_HUNG_INT_ST    (BIT(3))
#define UHCI_TX_HUNG_INT_ST_M  (UHCI_TX_HUNG_INT_ST_V << UHCI_TX_HUNG_INT_ST_S)
#define UHCI_TX_HUNG_INT_ST_V  0x00000001U
#define UHCI_TX_HUNG_INT_ST_S  3
/** UHCI_SEND_S_REG_Q_INT_ST : RO; bitpos: [4]; default: 0;
 *  a
 */
#define UHCI_SEND_S_REG_Q_INT_ST    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_ST_M  (UHCI_SEND_S_REG_Q_INT_ST_V << UHCI_SEND_S_REG_Q_INT_ST_S)
#define UHCI_SEND_S_REG_Q_INT_ST_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_ST_S  4
/** UHCI_SEND_A_REG_Q_INT_ST : RO; bitpos: [5]; default: 0;
 *  a
 */
#define UHCI_SEND_A_REG_Q_INT_ST    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_ST_M  (UHCI_SEND_A_REG_Q_INT_ST_V << UHCI_SEND_A_REG_Q_INT_ST_S)
#define UHCI_SEND_A_REG_Q_INT_ST_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_ST_S  5
/** UHCI_OUTLINK_EOF_ERR_INT_ST : RO; bitpos: [6]; default: 0;
 *  a
 */
#define UHCI_OUTLINK_EOF_ERR_INT_ST    (BIT(6))
#define UHCI_OUTLINK_EOF_ERR_INT_ST_M  (UHCI_OUTLINK_EOF_ERR_INT_ST_V << UHCI_OUTLINK_EOF_ERR_INT_ST_S)
#define UHCI_OUTLINK_EOF_ERR_INT_ST_V  0x00000001U
#define UHCI_OUTLINK_EOF_ERR_INT_ST_S  6
/** UHCI_APP_CTRL0_INT_ST : RO; bitpos: [7]; default: 0;
 *  a
 */
#define UHCI_APP_CTRL0_INT_ST    (BIT(7))
#define UHCI_APP_CTRL0_INT_ST_M  (UHCI_APP_CTRL0_INT_ST_V << UHCI_APP_CTRL0_INT_ST_S)
#define UHCI_APP_CTRL0_INT_ST_V  0x00000001U
#define UHCI_APP_CTRL0_INT_ST_S  7
/** UHCI_APP_CTRL1_INT_ST : RO; bitpos: [8]; default: 0;
 *  a
 */
#define UHCI_APP_CTRL1_INT_ST    (BIT(8))
#define UHCI_APP_CTRL1_INT_ST_M  (UHCI_APP_CTRL1_INT_ST_V << UHCI_APP_CTRL1_INT_ST_S)
#define UHCI_APP_CTRL1_INT_ST_V  0x00000001U
#define UHCI_APP_CTRL1_INT_ST_S  8

/** UHCI_INT_ENA_REG register
 *  a
 */
#define UHCI_INT_ENA_REG(i) (REG_UHCI_BASE(i) + 0xc)
/** UHCI_RX_START_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  a
 */
#define UHCI_RX_START_INT_ENA    (BIT(0))
#define UHCI_RX_START_INT_ENA_M  (UHCI_RX_START_INT_ENA_V << UHCI_RX_START_INT_ENA_S)
#define UHCI_RX_START_INT_ENA_V  0x00000001U
#define UHCI_RX_START_INT_ENA_S  0
/** UHCI_TX_START_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  a
 */
#define UHCI_TX_START_INT_ENA    (BIT(1))
#define UHCI_TX_START_INT_ENA_M  (UHCI_TX_START_INT_ENA_V << UHCI_TX_START_INT_ENA_S)
#define UHCI_TX_START_INT_ENA_V  0x00000001U
#define UHCI_TX_START_INT_ENA_S  1
/** UHCI_RX_HUNG_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  a
 */
#define UHCI_RX_HUNG_INT_ENA    (BIT(2))
#define UHCI_RX_HUNG_INT_ENA_M  (UHCI_RX_HUNG_INT_ENA_V << UHCI_RX_HUNG_INT_ENA_S)
#define UHCI_RX_HUNG_INT_ENA_V  0x00000001U
#define UHCI_RX_HUNG_INT_ENA_S  2
/** UHCI_TX_HUNG_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_TX_HUNG_INT_ENA    (BIT(3))
#define UHCI_TX_HUNG_INT_ENA_M  (UHCI_TX_HUNG_INT_ENA_V << UHCI_TX_HUNG_INT_ENA_S)
#define UHCI_TX_HUNG_INT_ENA_V  0x00000001U
#define UHCI_TX_HUNG_INT_ENA_S  3
/** UHCI_SEND_S_REG_Q_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  a
 */
#define UHCI_SEND_S_REG_Q_INT_ENA    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_ENA_M  (UHCI_SEND_S_REG_Q_INT_ENA_V << UHCI_SEND_S_REG_Q_INT_ENA_S)
#define UHCI_SEND_S_REG_Q_INT_ENA_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_ENA_S  4
/** UHCI_SEND_A_REG_Q_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  a
 */
#define UHCI_SEND_A_REG_Q_INT_ENA    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_ENA_M  (UHCI_SEND_A_REG_Q_INT_ENA_V << UHCI_SEND_A_REG_Q_INT_ENA_S)
#define UHCI_SEND_A_REG_Q_INT_ENA_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_ENA_S  5
/** UHCI_OUTLINK_EOF_ERR_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  a
 */
#define UHCI_OUTLINK_EOF_ERR_INT_ENA    (BIT(6))
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_M  (UHCI_OUTLINK_EOF_ERR_INT_ENA_V << UHCI_OUTLINK_EOF_ERR_INT_ENA_S)
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_V  0x00000001U
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_S  6
/** UHCI_APP_CTRL0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  a
 */
#define UHCI_APP_CTRL0_INT_ENA    (BIT(7))
#define UHCI_APP_CTRL0_INT_ENA_M  (UHCI_APP_CTRL0_INT_ENA_V << UHCI_APP_CTRL0_INT_ENA_S)
#define UHCI_APP_CTRL0_INT_ENA_V  0x00000001U
#define UHCI_APP_CTRL0_INT_ENA_S  7
/** UHCI_APP_CTRL1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  a
 */
#define UHCI_APP_CTRL1_INT_ENA    (BIT(8))
#define UHCI_APP_CTRL1_INT_ENA_M  (UHCI_APP_CTRL1_INT_ENA_V << UHCI_APP_CTRL1_INT_ENA_S)
#define UHCI_APP_CTRL1_INT_ENA_V  0x00000001U
#define UHCI_APP_CTRL1_INT_ENA_S  8

/** UHCI_INT_CLR_REG register
 *  a
 */
#define UHCI_INT_CLR_REG(i) (REG_UHCI_BASE(i) + 0x10)
/** UHCI_RX_START_INT_CLR : WT; bitpos: [0]; default: 0;
 *  a
 */
#define UHCI_RX_START_INT_CLR    (BIT(0))
#define UHCI_RX_START_INT_CLR_M  (UHCI_RX_START_INT_CLR_V << UHCI_RX_START_INT_CLR_S)
#define UHCI_RX_START_INT_CLR_V  0x00000001U
#define UHCI_RX_START_INT_CLR_S  0
/** UHCI_TX_START_INT_CLR : WT; bitpos: [1]; default: 0;
 *  a
 */
#define UHCI_TX_START_INT_CLR    (BIT(1))
#define UHCI_TX_START_INT_CLR_M  (UHCI_TX_START_INT_CLR_V << UHCI_TX_START_INT_CLR_S)
#define UHCI_TX_START_INT_CLR_V  0x00000001U
#define UHCI_TX_START_INT_CLR_S  1
/** UHCI_RX_HUNG_INT_CLR : WT; bitpos: [2]; default: 0;
 *  a
 */
#define UHCI_RX_HUNG_INT_CLR    (BIT(2))
#define UHCI_RX_HUNG_INT_CLR_M  (UHCI_RX_HUNG_INT_CLR_V << UHCI_RX_HUNG_INT_CLR_S)
#define UHCI_RX_HUNG_INT_CLR_V  0x00000001U
#define UHCI_RX_HUNG_INT_CLR_S  2
/** UHCI_TX_HUNG_INT_CLR : WT; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_TX_HUNG_INT_CLR    (BIT(3))
#define UHCI_TX_HUNG_INT_CLR_M  (UHCI_TX_HUNG_INT_CLR_V << UHCI_TX_HUNG_INT_CLR_S)
#define UHCI_TX_HUNG_INT_CLR_V  0x00000001U
#define UHCI_TX_HUNG_INT_CLR_S  3
/** UHCI_SEND_S_REG_Q_INT_CLR : WT; bitpos: [4]; default: 0;
 *  a
 */
#define UHCI_SEND_S_REG_Q_INT_CLR    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_CLR_M  (UHCI_SEND_S_REG_Q_INT_CLR_V << UHCI_SEND_S_REG_Q_INT_CLR_S)
#define UHCI_SEND_S_REG_Q_INT_CLR_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_CLR_S  4
/** UHCI_SEND_A_REG_Q_INT_CLR : WT; bitpos: [5]; default: 0;
 *  a
 */
#define UHCI_SEND_A_REG_Q_INT_CLR    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_CLR_M  (UHCI_SEND_A_REG_Q_INT_CLR_V << UHCI_SEND_A_REG_Q_INT_CLR_S)
#define UHCI_SEND_A_REG_Q_INT_CLR_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_CLR_S  5
/** UHCI_OUTLINK_EOF_ERR_INT_CLR : WT; bitpos: [6]; default: 0;
 *  a
 */
#define UHCI_OUTLINK_EOF_ERR_INT_CLR    (BIT(6))
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_M  (UHCI_OUTLINK_EOF_ERR_INT_CLR_V << UHCI_OUTLINK_EOF_ERR_INT_CLR_S)
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_V  0x00000001U
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_S  6
/** UHCI_APP_CTRL0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  a
 */
#define UHCI_APP_CTRL0_INT_CLR    (BIT(7))
#define UHCI_APP_CTRL0_INT_CLR_M  (UHCI_APP_CTRL0_INT_CLR_V << UHCI_APP_CTRL0_INT_CLR_S)
#define UHCI_APP_CTRL0_INT_CLR_V  0x00000001U
#define UHCI_APP_CTRL0_INT_CLR_S  7
/** UHCI_APP_CTRL1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  a
 */
#define UHCI_APP_CTRL1_INT_CLR    (BIT(8))
#define UHCI_APP_CTRL1_INT_CLR_M  (UHCI_APP_CTRL1_INT_CLR_V << UHCI_APP_CTRL1_INT_CLR_S)
#define UHCI_APP_CTRL1_INT_CLR_V  0x00000001U
#define UHCI_APP_CTRL1_INT_CLR_S  8

/** UHCI_CONF1_REG register
 *  a
 */
#define UHCI_CONF1_REG(i) (REG_UHCI_BASE(i) + 0x14)
/** UHCI_CHECK_SUM_EN : R/W; bitpos: [0]; default: 1;
 *  a
 */
#define UHCI_CHECK_SUM_EN    (BIT(0))
#define UHCI_CHECK_SUM_EN_M  (UHCI_CHECK_SUM_EN_V << UHCI_CHECK_SUM_EN_S)
#define UHCI_CHECK_SUM_EN_V  0x00000001U
#define UHCI_CHECK_SUM_EN_S  0
/** UHCI_CHECK_SEQ_EN : R/W; bitpos: [1]; default: 1;
 *  a
 */
#define UHCI_CHECK_SEQ_EN    (BIT(1))
#define UHCI_CHECK_SEQ_EN_M  (UHCI_CHECK_SEQ_EN_V << UHCI_CHECK_SEQ_EN_S)
#define UHCI_CHECK_SEQ_EN_V  0x00000001U
#define UHCI_CHECK_SEQ_EN_S  1
/** UHCI_CRC_DISABLE : R/W; bitpos: [2]; default: 0;
 *  a
 */
#define UHCI_CRC_DISABLE    (BIT(2))
#define UHCI_CRC_DISABLE_M  (UHCI_CRC_DISABLE_V << UHCI_CRC_DISABLE_S)
#define UHCI_CRC_DISABLE_V  0x00000001U
#define UHCI_CRC_DISABLE_S  2
/** UHCI_SAVE_HEAD : R/W; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_SAVE_HEAD    (BIT(3))
#define UHCI_SAVE_HEAD_M  (UHCI_SAVE_HEAD_V << UHCI_SAVE_HEAD_S)
#define UHCI_SAVE_HEAD_V  0x00000001U
#define UHCI_SAVE_HEAD_S  3
/** UHCI_TX_CHECK_SUM_RE : R/W; bitpos: [4]; default: 1;
 *  a
 */
#define UHCI_TX_CHECK_SUM_RE    (BIT(4))
#define UHCI_TX_CHECK_SUM_RE_M  (UHCI_TX_CHECK_SUM_RE_V << UHCI_TX_CHECK_SUM_RE_S)
#define UHCI_TX_CHECK_SUM_RE_V  0x00000001U
#define UHCI_TX_CHECK_SUM_RE_S  4
/** UHCI_TX_ACK_NUM_RE : R/W; bitpos: [5]; default: 1;
 *  a
 */
#define UHCI_TX_ACK_NUM_RE    (BIT(5))
#define UHCI_TX_ACK_NUM_RE_M  (UHCI_TX_ACK_NUM_RE_V << UHCI_TX_ACK_NUM_RE_S)
#define UHCI_TX_ACK_NUM_RE_V  0x00000001U
#define UHCI_TX_ACK_NUM_RE_S  5
/** UHCI_WAIT_SW_START : R/W; bitpos: [7]; default: 0;
 *  a
 */
#define UHCI_WAIT_SW_START    (BIT(7))
#define UHCI_WAIT_SW_START_M  (UHCI_WAIT_SW_START_V << UHCI_WAIT_SW_START_S)
#define UHCI_WAIT_SW_START_V  0x00000001U
#define UHCI_WAIT_SW_START_S  7
/** UHCI_SW_START : WT; bitpos: [8]; default: 0;
 *  a
 */
#define UHCI_SW_START    (BIT(8))
#define UHCI_SW_START_M  (UHCI_SW_START_V << UHCI_SW_START_S)
#define UHCI_SW_START_V  0x00000001U
#define UHCI_SW_START_S  8

/** UHCI_STATE0_REG register
 *  a
 */
#define UHCI_STATE0_REG(i) (REG_UHCI_BASE(i) + 0x18)
/** UHCI_RX_ERR_CAUSE : RO; bitpos: [2:0]; default: 0;
 *  a
 */
#define UHCI_RX_ERR_CAUSE    0x00000007U
#define UHCI_RX_ERR_CAUSE_M  (UHCI_RX_ERR_CAUSE_V << UHCI_RX_ERR_CAUSE_S)
#define UHCI_RX_ERR_CAUSE_V  0x00000007U
#define UHCI_RX_ERR_CAUSE_S  0
/** UHCI_DECODE_STATE : RO; bitpos: [5:3]; default: 0;
 *  a
 */
#define UHCI_DECODE_STATE    0x00000007U
#define UHCI_DECODE_STATE_M  (UHCI_DECODE_STATE_V << UHCI_DECODE_STATE_S)
#define UHCI_DECODE_STATE_V  0x00000007U
#define UHCI_DECODE_STATE_S  3

/** UHCI_STATE1_REG register
 *  a
 */
#define UHCI_STATE1_REG(i) (REG_UHCI_BASE(i) + 0x1c)
/** UHCI_ENCODE_STATE : RO; bitpos: [2:0]; default: 0;
 *  a
 */
#define UHCI_ENCODE_STATE    0x00000007U
#define UHCI_ENCODE_STATE_M  (UHCI_ENCODE_STATE_V << UHCI_ENCODE_STATE_S)
#define UHCI_ENCODE_STATE_V  0x00000007U
#define UHCI_ENCODE_STATE_S  0

/** UHCI_ESCAPE_CONF_REG register
 *  a
 */
#define UHCI_ESCAPE_CONF_REG(i) (REG_UHCI_BASE(i) + 0x20)
/** UHCI_TX_C0_ESC_EN : R/W; bitpos: [0]; default: 1;
 *  a
 */
#define UHCI_TX_C0_ESC_EN    (BIT(0))
#define UHCI_TX_C0_ESC_EN_M  (UHCI_TX_C0_ESC_EN_V << UHCI_TX_C0_ESC_EN_S)
#define UHCI_TX_C0_ESC_EN_V  0x00000001U
#define UHCI_TX_C0_ESC_EN_S  0
/** UHCI_TX_DB_ESC_EN : R/W; bitpos: [1]; default: 1;
 *  a
 */
#define UHCI_TX_DB_ESC_EN    (BIT(1))
#define UHCI_TX_DB_ESC_EN_M  (UHCI_TX_DB_ESC_EN_V << UHCI_TX_DB_ESC_EN_S)
#define UHCI_TX_DB_ESC_EN_V  0x00000001U
#define UHCI_TX_DB_ESC_EN_S  1
/** UHCI_TX_11_ESC_EN : R/W; bitpos: [2]; default: 0;
 *  a
 */
#define UHCI_TX_11_ESC_EN    (BIT(2))
#define UHCI_TX_11_ESC_EN_M  (UHCI_TX_11_ESC_EN_V << UHCI_TX_11_ESC_EN_S)
#define UHCI_TX_11_ESC_EN_V  0x00000001U
#define UHCI_TX_11_ESC_EN_S  2
/** UHCI_TX_13_ESC_EN : R/W; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_TX_13_ESC_EN    (BIT(3))
#define UHCI_TX_13_ESC_EN_M  (UHCI_TX_13_ESC_EN_V << UHCI_TX_13_ESC_EN_S)
#define UHCI_TX_13_ESC_EN_V  0x00000001U
#define UHCI_TX_13_ESC_EN_S  3
/** UHCI_RX_C0_ESC_EN : R/W; bitpos: [4]; default: 1;
 *  a
 */
#define UHCI_RX_C0_ESC_EN    (BIT(4))
#define UHCI_RX_C0_ESC_EN_M  (UHCI_RX_C0_ESC_EN_V << UHCI_RX_C0_ESC_EN_S)
#define UHCI_RX_C0_ESC_EN_V  0x00000001U
#define UHCI_RX_C0_ESC_EN_S  4
/** UHCI_RX_DB_ESC_EN : R/W; bitpos: [5]; default: 1;
 *  a
 */
#define UHCI_RX_DB_ESC_EN    (BIT(5))
#define UHCI_RX_DB_ESC_EN_M  (UHCI_RX_DB_ESC_EN_V << UHCI_RX_DB_ESC_EN_S)
#define UHCI_RX_DB_ESC_EN_V  0x00000001U
#define UHCI_RX_DB_ESC_EN_S  5
/** UHCI_RX_11_ESC_EN : R/W; bitpos: [6]; default: 0;
 *  a
 */
#define UHCI_RX_11_ESC_EN    (BIT(6))
#define UHCI_RX_11_ESC_EN_M  (UHCI_RX_11_ESC_EN_V << UHCI_RX_11_ESC_EN_S)
#define UHCI_RX_11_ESC_EN_V  0x00000001U
#define UHCI_RX_11_ESC_EN_S  6
/** UHCI_RX_13_ESC_EN : R/W; bitpos: [7]; default: 0;
 *  a
 */
#define UHCI_RX_13_ESC_EN    (BIT(7))
#define UHCI_RX_13_ESC_EN_M  (UHCI_RX_13_ESC_EN_V << UHCI_RX_13_ESC_EN_S)
#define UHCI_RX_13_ESC_EN_V  0x00000001U
#define UHCI_RX_13_ESC_EN_S  7

/** UHCI_HUNG_CONF_REG register
 *  a
 */
#define UHCI_HUNG_CONF_REG(i) (REG_UHCI_BASE(i) + 0x24)
/** UHCI_TXFIFO_TIMEOUT : R/W; bitpos: [7:0]; default: 16;
 *  a
 */
#define UHCI_TXFIFO_TIMEOUT    0x000000FFU
#define UHCI_TXFIFO_TIMEOUT_M  (UHCI_TXFIFO_TIMEOUT_V << UHCI_TXFIFO_TIMEOUT_S)
#define UHCI_TXFIFO_TIMEOUT_V  0x000000FFU
#define UHCI_TXFIFO_TIMEOUT_S  0
/** UHCI_TXFIFO_TIMEOUT_SHIFT : R/W; bitpos: [10:8]; default: 0;
 *  a
 */
#define UHCI_TXFIFO_TIMEOUT_SHIFT    0x00000007U
#define UHCI_TXFIFO_TIMEOUT_SHIFT_M  (UHCI_TXFIFO_TIMEOUT_SHIFT_V << UHCI_TXFIFO_TIMEOUT_SHIFT_S)
#define UHCI_TXFIFO_TIMEOUT_SHIFT_V  0x00000007U
#define UHCI_TXFIFO_TIMEOUT_SHIFT_S  8
/** UHCI_TXFIFO_TIMEOUT_ENA : R/W; bitpos: [11]; default: 1;
 *  a
 */
#define UHCI_TXFIFO_TIMEOUT_ENA    (BIT(11))
#define UHCI_TXFIFO_TIMEOUT_ENA_M  (UHCI_TXFIFO_TIMEOUT_ENA_V << UHCI_TXFIFO_TIMEOUT_ENA_S)
#define UHCI_TXFIFO_TIMEOUT_ENA_V  0x00000001U
#define UHCI_TXFIFO_TIMEOUT_ENA_S  11
/** UHCI_RXFIFO_TIMEOUT : R/W; bitpos: [19:12]; default: 16;
 *  a
 */
#define UHCI_RXFIFO_TIMEOUT    0x000000FFU
#define UHCI_RXFIFO_TIMEOUT_M  (UHCI_RXFIFO_TIMEOUT_V << UHCI_RXFIFO_TIMEOUT_S)
#define UHCI_RXFIFO_TIMEOUT_V  0x000000FFU
#define UHCI_RXFIFO_TIMEOUT_S  12
/** UHCI_RXFIFO_TIMEOUT_SHIFT : R/W; bitpos: [22:20]; default: 0;
 *  a
 */
#define UHCI_RXFIFO_TIMEOUT_SHIFT    0x00000007U
#define UHCI_RXFIFO_TIMEOUT_SHIFT_M  (UHCI_RXFIFO_TIMEOUT_SHIFT_V << UHCI_RXFIFO_TIMEOUT_SHIFT_S)
#define UHCI_RXFIFO_TIMEOUT_SHIFT_V  0x00000007U
#define UHCI_RXFIFO_TIMEOUT_SHIFT_S  20
/** UHCI_RXFIFO_TIMEOUT_ENA : R/W; bitpos: [23]; default: 1;
 *  a
 */
#define UHCI_RXFIFO_TIMEOUT_ENA    (BIT(23))
#define UHCI_RXFIFO_TIMEOUT_ENA_M  (UHCI_RXFIFO_TIMEOUT_ENA_V << UHCI_RXFIFO_TIMEOUT_ENA_S)
#define UHCI_RXFIFO_TIMEOUT_ENA_V  0x00000001U
#define UHCI_RXFIFO_TIMEOUT_ENA_S  23

/** UHCI_ACK_NUM_REG register
 *  a
 */
#define UHCI_ACK_NUM_REG(i) (REG_UHCI_BASE(i) + 0x28)
/** UHCI_ACK_NUM : R/W; bitpos: [2:0]; default: 0;
 *  a
 */
#define UHCI_ACK_NUM    0x00000007U
#define UHCI_ACK_NUM_M  (UHCI_ACK_NUM_V << UHCI_ACK_NUM_S)
#define UHCI_ACK_NUM_V  0x00000007U
#define UHCI_ACK_NUM_S  0
/** UHCI_ACK_NUM_LOAD : WT; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_ACK_NUM_LOAD    (BIT(3))
#define UHCI_ACK_NUM_LOAD_M  (UHCI_ACK_NUM_LOAD_V << UHCI_ACK_NUM_LOAD_S)
#define UHCI_ACK_NUM_LOAD_V  0x00000001U
#define UHCI_ACK_NUM_LOAD_S  3

/** UHCI_RX_HEAD_REG register
 *  a
 */
#define UHCI_RX_HEAD_REG(i) (REG_UHCI_BASE(i) + 0x2c)
/** UHCI_RX_HEAD : RO; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_RX_HEAD    0xFFFFFFFFU
#define UHCI_RX_HEAD_M  (UHCI_RX_HEAD_V << UHCI_RX_HEAD_S)
#define UHCI_RX_HEAD_V  0xFFFFFFFFU
#define UHCI_RX_HEAD_S  0

/** UHCI_QUICK_SENT_REG register
 *  a
 */
#define UHCI_QUICK_SENT_REG(i) (REG_UHCI_BASE(i) + 0x30)
/** UHCI_SINGLE_SEND_NUM : R/W; bitpos: [2:0]; default: 0;
 *  a
 */
#define UHCI_SINGLE_SEND_NUM    0x00000007U
#define UHCI_SINGLE_SEND_NUM_M  (UHCI_SINGLE_SEND_NUM_V << UHCI_SINGLE_SEND_NUM_S)
#define UHCI_SINGLE_SEND_NUM_V  0x00000007U
#define UHCI_SINGLE_SEND_NUM_S  0
/** UHCI_SINGLE_SEND_EN : WT; bitpos: [3]; default: 0;
 *  a
 */
#define UHCI_SINGLE_SEND_EN    (BIT(3))
#define UHCI_SINGLE_SEND_EN_M  (UHCI_SINGLE_SEND_EN_V << UHCI_SINGLE_SEND_EN_S)
#define UHCI_SINGLE_SEND_EN_V  0x00000001U
#define UHCI_SINGLE_SEND_EN_S  3
/** UHCI_ALWAYS_SEND_NUM : R/W; bitpos: [6:4]; default: 0;
 *  a
 */
#define UHCI_ALWAYS_SEND_NUM    0x00000007U
#define UHCI_ALWAYS_SEND_NUM_M  (UHCI_ALWAYS_SEND_NUM_V << UHCI_ALWAYS_SEND_NUM_S)
#define UHCI_ALWAYS_SEND_NUM_V  0x00000007U
#define UHCI_ALWAYS_SEND_NUM_S  4
/** UHCI_ALWAYS_SEND_EN : R/W; bitpos: [7]; default: 0;
 *  a
 */
#define UHCI_ALWAYS_SEND_EN    (BIT(7))
#define UHCI_ALWAYS_SEND_EN_M  (UHCI_ALWAYS_SEND_EN_V << UHCI_ALWAYS_SEND_EN_S)
#define UHCI_ALWAYS_SEND_EN_V  0x00000001U
#define UHCI_ALWAYS_SEND_EN_S  7

/** UHCI_REG_Q0_WORD0_REG register
 *  a
 */
#define UHCI_REG_Q0_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x34)
/** UHCI_SEND_Q0_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q0_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD0_M  (UHCI_SEND_Q0_WORD0_V << UHCI_SEND_Q0_WORD0_S)
#define UHCI_SEND_Q0_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD0_S  0

/** UHCI_REG_Q0_WORD1_REG register
 *  a
 */
#define UHCI_REG_Q0_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x38)
/** UHCI_SEND_Q0_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q0_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD1_M  (UHCI_SEND_Q0_WORD1_V << UHCI_SEND_Q0_WORD1_S)
#define UHCI_SEND_Q0_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD1_S  0

/** UHCI_REG_Q1_WORD0_REG register
 *  a
 */
#define UHCI_REG_Q1_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x3c)
/** UHCI_SEND_Q1_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q1_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD0_M  (UHCI_SEND_Q1_WORD0_V << UHCI_SEND_Q1_WORD0_S)
#define UHCI_SEND_Q1_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD0_S  0

/** UHCI_REG_Q1_WORD1_REG register
 *  a
 */
#define UHCI_REG_Q1_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x40)
/** UHCI_SEND_Q1_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q1_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD1_M  (UHCI_SEND_Q1_WORD1_V << UHCI_SEND_Q1_WORD1_S)
#define UHCI_SEND_Q1_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD1_S  0

/** UHCI_REG_Q2_WORD0_REG register
 *  a
 */
#define UHCI_REG_Q2_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x44)
/** UHCI_SEND_Q2_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q2_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD0_M  (UHCI_SEND_Q2_WORD0_V << UHCI_SEND_Q2_WORD0_S)
#define UHCI_SEND_Q2_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD0_S  0

/** UHCI_REG_Q2_WORD1_REG register
 *  a
 */
#define UHCI_REG_Q2_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x48)
/** UHCI_SEND_Q2_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q2_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD1_M  (UHCI_SEND_Q2_WORD1_V << UHCI_SEND_Q2_WORD1_S)
#define UHCI_SEND_Q2_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD1_S  0

/** UHCI_REG_Q3_WORD0_REG register
 *  a
 */
#define UHCI_REG_Q3_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x4c)
/** UHCI_SEND_Q3_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q3_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD0_M  (UHCI_SEND_Q3_WORD0_V << UHCI_SEND_Q3_WORD0_S)
#define UHCI_SEND_Q3_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD0_S  0

/** UHCI_REG_Q3_WORD1_REG register
 *  a
 */
#define UHCI_REG_Q3_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x50)
/** UHCI_SEND_Q3_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q3_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD1_M  (UHCI_SEND_Q3_WORD1_V << UHCI_SEND_Q3_WORD1_S)
#define UHCI_SEND_Q3_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD1_S  0

/** UHCI_REG_Q4_WORD0_REG register
 *  a
 */
#define UHCI_REG_Q4_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x54)
/** UHCI_SEND_Q4_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q4_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD0_M  (UHCI_SEND_Q4_WORD0_V << UHCI_SEND_Q4_WORD0_S)
#define UHCI_SEND_Q4_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD0_S  0

/** UHCI_REG_Q4_WORD1_REG register
 *  a
 */
#define UHCI_REG_Q4_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x58)
/** UHCI_SEND_Q4_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q4_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD1_M  (UHCI_SEND_Q4_WORD1_V << UHCI_SEND_Q4_WORD1_S)
#define UHCI_SEND_Q4_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD1_S  0

/** UHCI_REG_Q5_WORD0_REG register
 *  a
 */
#define UHCI_REG_Q5_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x5c)
/** UHCI_SEND_Q5_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q5_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD0_M  (UHCI_SEND_Q5_WORD0_V << UHCI_SEND_Q5_WORD0_S)
#define UHCI_SEND_Q5_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD0_S  0

/** UHCI_REG_Q5_WORD1_REG register
 *  a
 */
#define UHCI_REG_Q5_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x60)
/** UHCI_SEND_Q5_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q5_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD1_M  (UHCI_SEND_Q5_WORD1_V << UHCI_SEND_Q5_WORD1_S)
#define UHCI_SEND_Q5_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD1_S  0

/** UHCI_REG_Q6_WORD0_REG register
 *  a
 */
#define UHCI_REG_Q6_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x64)
/** UHCI_SEND_Q6_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q6_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD0_M  (UHCI_SEND_Q6_WORD0_V << UHCI_SEND_Q6_WORD0_S)
#define UHCI_SEND_Q6_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD0_S  0

/** UHCI_REG_Q6_WORD1_REG register
 *  a
 */
#define UHCI_REG_Q6_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x68)
/** UHCI_SEND_Q6_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  a
 */
#define UHCI_SEND_Q6_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD1_M  (UHCI_SEND_Q6_WORD1_V << UHCI_SEND_Q6_WORD1_S)
#define UHCI_SEND_Q6_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD1_S  0

/** UHCI_ESC_CONF0_REG register
 *  a
 */
#define UHCI_ESC_CONF0_REG(i) (REG_UHCI_BASE(i) + 0x6c)
/** UHCI_SEPER_CHAR : R/W; bitpos: [7:0]; default: 192;
 *  a
 */
#define UHCI_SEPER_CHAR    0x000000FFU
#define UHCI_SEPER_CHAR_M  (UHCI_SEPER_CHAR_V << UHCI_SEPER_CHAR_S)
#define UHCI_SEPER_CHAR_V  0x000000FFU
#define UHCI_SEPER_CHAR_S  0
/** UHCI_SEPER_ESC_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  a
 */
#define UHCI_SEPER_ESC_CHAR0    0x000000FFU
#define UHCI_SEPER_ESC_CHAR0_M  (UHCI_SEPER_ESC_CHAR0_V << UHCI_SEPER_ESC_CHAR0_S)
#define UHCI_SEPER_ESC_CHAR0_V  0x000000FFU
#define UHCI_SEPER_ESC_CHAR0_S  8
/** UHCI_SEPER_ESC_CHAR1 : R/W; bitpos: [23:16]; default: 220;
 *  a
 */
#define UHCI_SEPER_ESC_CHAR1    0x000000FFU
#define UHCI_SEPER_ESC_CHAR1_M  (UHCI_SEPER_ESC_CHAR1_V << UHCI_SEPER_ESC_CHAR1_S)
#define UHCI_SEPER_ESC_CHAR1_V  0x000000FFU
#define UHCI_SEPER_ESC_CHAR1_S  16

/** UHCI_ESC_CONF1_REG register
 *  a
 */
#define UHCI_ESC_CONF1_REG(i) (REG_UHCI_BASE(i) + 0x70)
/** UHCI_ESC_SEQ0 : R/W; bitpos: [7:0]; default: 219;
 *  a
 */
#define UHCI_ESC_SEQ0    0x000000FFU
#define UHCI_ESC_SEQ0_M  (UHCI_ESC_SEQ0_V << UHCI_ESC_SEQ0_S)
#define UHCI_ESC_SEQ0_V  0x000000FFU
#define UHCI_ESC_SEQ0_S  0
/** UHCI_ESC_SEQ0_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  a
 */
#define UHCI_ESC_SEQ0_CHAR0    0x000000FFU
#define UHCI_ESC_SEQ0_CHAR0_M  (UHCI_ESC_SEQ0_CHAR0_V << UHCI_ESC_SEQ0_CHAR0_S)
#define UHCI_ESC_SEQ0_CHAR0_V  0x000000FFU
#define UHCI_ESC_SEQ0_CHAR0_S  8
/** UHCI_ESC_SEQ0_CHAR1 : R/W; bitpos: [23:16]; default: 221;
 *  a
 */
#define UHCI_ESC_SEQ0_CHAR1    0x000000FFU
#define UHCI_ESC_SEQ0_CHAR1_M  (UHCI_ESC_SEQ0_CHAR1_V << UHCI_ESC_SEQ0_CHAR1_S)
#define UHCI_ESC_SEQ0_CHAR1_V  0x000000FFU
#define UHCI_ESC_SEQ0_CHAR1_S  16

/** UHCI_ESC_CONF2_REG register
 *  a
 */
#define UHCI_ESC_CONF2_REG(i) (REG_UHCI_BASE(i) + 0x74)
/** UHCI_ESC_SEQ1 : R/W; bitpos: [7:0]; default: 17;
 *  a
 */
#define UHCI_ESC_SEQ1    0x000000FFU
#define UHCI_ESC_SEQ1_M  (UHCI_ESC_SEQ1_V << UHCI_ESC_SEQ1_S)
#define UHCI_ESC_SEQ1_V  0x000000FFU
#define UHCI_ESC_SEQ1_S  0
/** UHCI_ESC_SEQ1_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  a
 */
#define UHCI_ESC_SEQ1_CHAR0    0x000000FFU
#define UHCI_ESC_SEQ1_CHAR0_M  (UHCI_ESC_SEQ1_CHAR0_V << UHCI_ESC_SEQ1_CHAR0_S)
#define UHCI_ESC_SEQ1_CHAR0_V  0x000000FFU
#define UHCI_ESC_SEQ1_CHAR0_S  8
/** UHCI_ESC_SEQ1_CHAR1 : R/W; bitpos: [23:16]; default: 222;
 *  a
 */
#define UHCI_ESC_SEQ1_CHAR1    0x000000FFU
#define UHCI_ESC_SEQ1_CHAR1_M  (UHCI_ESC_SEQ1_CHAR1_V << UHCI_ESC_SEQ1_CHAR1_S)
#define UHCI_ESC_SEQ1_CHAR1_V  0x000000FFU
#define UHCI_ESC_SEQ1_CHAR1_S  16

/** UHCI_ESC_CONF3_REG register
 *  a
 */
#define UHCI_ESC_CONF3_REG(i) (REG_UHCI_BASE(i) + 0x78)
/** UHCI_ESC_SEQ2 : R/W; bitpos: [7:0]; default: 19;
 *  a
 */
#define UHCI_ESC_SEQ2    0x000000FFU
#define UHCI_ESC_SEQ2_M  (UHCI_ESC_SEQ2_V << UHCI_ESC_SEQ2_S)
#define UHCI_ESC_SEQ2_V  0x000000FFU
#define UHCI_ESC_SEQ2_S  0
/** UHCI_ESC_SEQ2_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  a
 */
#define UHCI_ESC_SEQ2_CHAR0    0x000000FFU
#define UHCI_ESC_SEQ2_CHAR0_M  (UHCI_ESC_SEQ2_CHAR0_V << UHCI_ESC_SEQ2_CHAR0_S)
#define UHCI_ESC_SEQ2_CHAR0_V  0x000000FFU
#define UHCI_ESC_SEQ2_CHAR0_S  8
/** UHCI_ESC_SEQ2_CHAR1 : R/W; bitpos: [23:16]; default: 223;
 *  a
 */
#define UHCI_ESC_SEQ2_CHAR1    0x000000FFU
#define UHCI_ESC_SEQ2_CHAR1_M  (UHCI_ESC_SEQ2_CHAR1_V << UHCI_ESC_SEQ2_CHAR1_S)
#define UHCI_ESC_SEQ2_CHAR1_V  0x000000FFU
#define UHCI_ESC_SEQ2_CHAR1_S  16

/** UHCI_PKT_THRES_REG register
 *  a
 */
#define UHCI_PKT_THRES_REG(i) (REG_UHCI_BASE(i) + 0x7c)
/** UHCI_PKT_THRS : R/W; bitpos: [12:0]; default: 128;
 *  a
 */
#define UHCI_PKT_THRS    0x00001FFFU
#define UHCI_PKT_THRS_M  (UHCI_PKT_THRS_V << UHCI_PKT_THRS_S)
#define UHCI_PKT_THRS_V  0x00001FFFU
#define UHCI_PKT_THRS_S  0

/** UHCI_DATE_REG register
 *  a
 */
#define UHCI_DATE_REG(i) (REG_UHCI_BASE(i) + 0x80)
/** UHCI_DATE : R/W; bitpos: [31:0]; default: 35655936;
 *  a
 */
#define UHCI_DATE    0xFFFFFFFFU
#define UHCI_DATE_M  (UHCI_DATE_V << UHCI_DATE_S)
#define UHCI_DATE_V  0xFFFFFFFFU
#define UHCI_DATE_S  0

#ifdef __cplusplus
}
#endif
