/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 *  UHCI configuration register
 */
#define UHCI_CONF0_REG(i) (REG_UHCI_BASE(i) + 0x0)
/** UHCI_TX_RST : R/W; bitpos: [0]; default: 0;
 *  Write 1 and then write 0 to reset the decoder state machine.
 */
#define UHCI_TX_RST    (BIT(0))
#define UHCI_TX_RST_M  (UHCI_TX_RST_V << UHCI_TX_RST_S)
#define UHCI_TX_RST_V  0x00000001U
#define UHCI_TX_RST_S  0
/** UHCI_RX_RST : R/W; bitpos: [1]; default: 0;
 *  Write 1 and then write 0 to reset the encoder state machine.
 */
#define UHCI_RX_RST    (BIT(1))
#define UHCI_RX_RST_M  (UHCI_RX_RST_V << UHCI_RX_RST_S)
#define UHCI_RX_RST_V  0x00000001U
#define UHCI_RX_RST_S  1
/** UHCI_UART_SEL : R/W; bitpos: [4:2]; default: 7;
 *  Configures to select which uart to connect with UHCI.\\
 *  0: UART0\\
 *  1: UART1\\
 */
#define UHCI_UART_SEL    0x00000007U
#define UHCI_UART_SEL_M  (UHCI_UART_SEL_V << UHCI_UART_SEL_S)
#define UHCI_UART_SEL_V  0x00000007U
#define UHCI_UART_SEL_S  2
/** UHCI_SEPER_EN : R/W; bitpos: [5]; default: 1;
 *  Configures whether or not to separate the data frame with a special character.\\
 *  0: Not separate\\
 *  1: Separate\\
 */
#define UHCI_SEPER_EN    (BIT(5))
#define UHCI_SEPER_EN_M  (UHCI_SEPER_EN_V << UHCI_SEPER_EN_S)
#define UHCI_SEPER_EN_V  0x00000001U
#define UHCI_SEPER_EN_S  5
/** UHCI_HEAD_EN : R/W; bitpos: [6]; default: 1;
 *  Configures whether or not to encode the data packet with a formatting header.\\
 *  0: Not use formatting header\\
 *  1: Use formatting header\\
 */
#define UHCI_HEAD_EN    (BIT(6))
#define UHCI_HEAD_EN_M  (UHCI_HEAD_EN_V << UHCI_HEAD_EN_S)
#define UHCI_HEAD_EN_V  0x00000001U
#define UHCI_HEAD_EN_S  6
/** UHCI_CRC_REC_EN : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not to enable the reception of the 16-bit CRC.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UHCI_CRC_REC_EN    (BIT(7))
#define UHCI_CRC_REC_EN_M  (UHCI_CRC_REC_EN_V << UHCI_CRC_REC_EN_S)
#define UHCI_CRC_REC_EN_V  0x00000001U
#define UHCI_CRC_REC_EN_S  7
/** UHCI_UART_IDLE_EOF_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to stop receiving data when UART is idle.\\
 *  0: Not stop\\
 *  1: Stop\\
 */
#define UHCI_UART_IDLE_EOF_EN    (BIT(8))
#define UHCI_UART_IDLE_EOF_EN_M  (UHCI_UART_IDLE_EOF_EN_V << UHCI_UART_IDLE_EOF_EN_S)
#define UHCI_UART_IDLE_EOF_EN_V  0x00000001U
#define UHCI_UART_IDLE_EOF_EN_S  8
/** UHCI_LEN_EOF_EN : R/W; bitpos: [9]; default: 1;
 *  Configures when the UHCI decoder stops receiving data.\\
 *  0: Stops after receiving 0xC0\\
 *  1: Stops when the number of received data bytes reach the specified value. When
 *  UHCI_HEAD_EN is 1, the specified value is the data length indicated by the UHCI
 *  packet header. when UHCI_HEAD_EN is 0, the specified value is the configured
 *  value.\\
 */
#define UHCI_LEN_EOF_EN    (BIT(9))
#define UHCI_LEN_EOF_EN_M  (UHCI_LEN_EOF_EN_V << UHCI_LEN_EOF_EN_S)
#define UHCI_LEN_EOF_EN_V  0x00000001U
#define UHCI_LEN_EOF_EN_S  9
/** UHCI_ENCODE_CRC_EN : R/W; bitpos: [10]; default: 1;
 *  Configures whether or not to enable data integrity check by appending a 16 bit
 *  CCITT-CRC to the end of the data.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UHCI_ENCODE_CRC_EN    (BIT(10))
#define UHCI_ENCODE_CRC_EN_M  (UHCI_ENCODE_CRC_EN_V << UHCI_ENCODE_CRC_EN_S)
#define UHCI_ENCODE_CRC_EN_V  0x00000001U
#define UHCI_ENCODE_CRC_EN_S  10
/** UHCI_CLK_EN : R/W; bitpos: [11]; default: 0;
 *  Configures clock gating.\\
 *  0: Support clock only when the application writes registers.\\
 *  1: Always force the clock on for registers.\\
 */
#define UHCI_CLK_EN    (BIT(11))
#define UHCI_CLK_EN_M  (UHCI_CLK_EN_V << UHCI_CLK_EN_S)
#define UHCI_CLK_EN_V  0x00000001U
#define UHCI_CLK_EN_S  11
/** UHCI_UART_RX_BRK_EOF_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to stop UHCI from receiving data after UART has received
 *  a NULL frame.\\
 *  0: Not stop\\
 *  1: Stop\\
 */
#define UHCI_UART_RX_BRK_EOF_EN    (BIT(12))
#define UHCI_UART_RX_BRK_EOF_EN_M  (UHCI_UART_RX_BRK_EOF_EN_V << UHCI_UART_RX_BRK_EOF_EN_S)
#define UHCI_UART_RX_BRK_EOF_EN_V  0x00000001U
#define UHCI_UART_RX_BRK_EOF_EN_S  12

/** UHCI_INT_RAW_REG register
 *  Raw interrupt status
 */
#define UHCI_INT_RAW_REG(i) (REG_UHCI_BASE(i) + 0x4)
/** UHCI_RX_START_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status of UHCI_RX_START_INT.
 */
#define UHCI_RX_START_INT_RAW    (BIT(0))
#define UHCI_RX_START_INT_RAW_M  (UHCI_RX_START_INT_RAW_V << UHCI_RX_START_INT_RAW_S)
#define UHCI_RX_START_INT_RAW_V  0x00000001U
#define UHCI_RX_START_INT_RAW_S  0
/** UHCI_TX_START_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status of UHCI_TX_START_INT.
 */
#define UHCI_TX_START_INT_RAW    (BIT(1))
#define UHCI_TX_START_INT_RAW_M  (UHCI_TX_START_INT_RAW_V << UHCI_TX_START_INT_RAW_S)
#define UHCI_TX_START_INT_RAW_V  0x00000001U
#define UHCI_TX_START_INT_RAW_S  1
/** UHCI_RX_HUNG_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status of UHCI_RX_HUNG_INT.
 */
#define UHCI_RX_HUNG_INT_RAW    (BIT(2))
#define UHCI_RX_HUNG_INT_RAW_M  (UHCI_RX_HUNG_INT_RAW_V << UHCI_RX_HUNG_INT_RAW_S)
#define UHCI_RX_HUNG_INT_RAW_V  0x00000001U
#define UHCI_RX_HUNG_INT_RAW_S  2
/** UHCI_TX_HUNG_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status of UHCI_TX_HUNG_INT.
 */
#define UHCI_TX_HUNG_INT_RAW    (BIT(3))
#define UHCI_TX_HUNG_INT_RAW_M  (UHCI_TX_HUNG_INT_RAW_V << UHCI_TX_HUNG_INT_RAW_S)
#define UHCI_TX_HUNG_INT_RAW_V  0x00000001U
#define UHCI_TX_HUNG_INT_RAW_S  3
/** UHCI_SEND_S_REG_Q_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status of UHCI_SEND_S_REG_Q_INT.
 */
#define UHCI_SEND_S_REG_Q_INT_RAW    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_RAW_M  (UHCI_SEND_S_REG_Q_INT_RAW_V << UHCI_SEND_S_REG_Q_INT_RAW_S)
#define UHCI_SEND_S_REG_Q_INT_RAW_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_RAW_S  4
/** UHCI_SEND_A_REG_Q_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt status of UHCI_SEND_A_REG_Q_INT.
 */
#define UHCI_SEND_A_REG_Q_INT_RAW    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_RAW_M  (UHCI_SEND_A_REG_Q_INT_RAW_V << UHCI_SEND_A_REG_Q_INT_RAW_S)
#define UHCI_SEND_A_REG_Q_INT_RAW_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_RAW_S  5
/** UHCI_OUT_EOF_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt status of UHCI_OUT_EOF_INT.
 */
#define UHCI_OUT_EOF_INT_RAW    (BIT(6))
#define UHCI_OUT_EOF_INT_RAW_M  (UHCI_OUT_EOF_INT_RAW_V << UHCI_OUT_EOF_INT_RAW_S)
#define UHCI_OUT_EOF_INT_RAW_V  0x00000001U
#define UHCI_OUT_EOF_INT_RAW_S  6
/** UHCI_APP_CTRL0_INT_RAW : R/W; bitpos: [7]; default: 0;
 *  The raw interrupt status of UHCI_APP_CTRL0_INT.
 */
#define UHCI_APP_CTRL0_INT_RAW    (BIT(7))
#define UHCI_APP_CTRL0_INT_RAW_M  (UHCI_APP_CTRL0_INT_RAW_V << UHCI_APP_CTRL0_INT_RAW_S)
#define UHCI_APP_CTRL0_INT_RAW_V  0x00000001U
#define UHCI_APP_CTRL0_INT_RAW_S  7
/** UHCI_APP_CTRL1_INT_RAW : R/W; bitpos: [8]; default: 0;
 *  The raw interrupt status of UHCI_APP_CTRL1_INT.
 */
#define UHCI_APP_CTRL1_INT_RAW    (BIT(8))
#define UHCI_APP_CTRL1_INT_RAW_M  (UHCI_APP_CTRL1_INT_RAW_V << UHCI_APP_CTRL1_INT_RAW_S)
#define UHCI_APP_CTRL1_INT_RAW_V  0x00000001U
#define UHCI_APP_CTRL1_INT_RAW_S  8

/** UHCI_INT_ST_REG register
 *  Masked interrupt status
 */
#define UHCI_INT_ST_REG(i) (REG_UHCI_BASE(i) + 0x8)
/** UHCI_RX_START_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of UHCI_RX_START_INT.
 */
#define UHCI_RX_START_INT_ST    (BIT(0))
#define UHCI_RX_START_INT_ST_M  (UHCI_RX_START_INT_ST_V << UHCI_RX_START_INT_ST_S)
#define UHCI_RX_START_INT_ST_V  0x00000001U
#define UHCI_RX_START_INT_ST_S  0
/** UHCI_TX_START_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of UHCI_TX_START_INT.
 */
#define UHCI_TX_START_INT_ST    (BIT(1))
#define UHCI_TX_START_INT_ST_M  (UHCI_TX_START_INT_ST_V << UHCI_TX_START_INT_ST_S)
#define UHCI_TX_START_INT_ST_V  0x00000001U
#define UHCI_TX_START_INT_ST_S  1
/** UHCI_RX_HUNG_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of UHCI_RX_HUNG_INT.
 */
#define UHCI_RX_HUNG_INT_ST    (BIT(2))
#define UHCI_RX_HUNG_INT_ST_M  (UHCI_RX_HUNG_INT_ST_V << UHCI_RX_HUNG_INT_ST_S)
#define UHCI_RX_HUNG_INT_ST_V  0x00000001U
#define UHCI_RX_HUNG_INT_ST_S  2
/** UHCI_TX_HUNG_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status of UHCI_TX_HUNG_INT.
 */
#define UHCI_TX_HUNG_INT_ST    (BIT(3))
#define UHCI_TX_HUNG_INT_ST_M  (UHCI_TX_HUNG_INT_ST_V << UHCI_TX_HUNG_INT_ST_S)
#define UHCI_TX_HUNG_INT_ST_V  0x00000001U
#define UHCI_TX_HUNG_INT_ST_S  3
/** UHCI_SEND_S_REG_Q_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status of UHCI_SEND_S_REG_Q_INT.
 */
#define UHCI_SEND_S_REG_Q_INT_ST    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_ST_M  (UHCI_SEND_S_REG_Q_INT_ST_V << UHCI_SEND_S_REG_Q_INT_ST_S)
#define UHCI_SEND_S_REG_Q_INT_ST_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_ST_S  4
/** UHCI_SEND_A_REG_Q_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status of UHCI_SEND_A_REG_Q_INT.
 */
#define UHCI_SEND_A_REG_Q_INT_ST    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_ST_M  (UHCI_SEND_A_REG_Q_INT_ST_V << UHCI_SEND_A_REG_Q_INT_ST_S)
#define UHCI_SEND_A_REG_Q_INT_ST_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_ST_S  5
/** UHCI_OUTLINK_EOF_ERR_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status of UHCI_OUTLINK_EOF_ERR_INT.
 */
#define UHCI_OUTLINK_EOF_ERR_INT_ST    (BIT(6))
#define UHCI_OUTLINK_EOF_ERR_INT_ST_M  (UHCI_OUTLINK_EOF_ERR_INT_ST_V << UHCI_OUTLINK_EOF_ERR_INT_ST_S)
#define UHCI_OUTLINK_EOF_ERR_INT_ST_V  0x00000001U
#define UHCI_OUTLINK_EOF_ERR_INT_ST_S  6
/** UHCI_APP_CTRL0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status of UHCI_APP_CTRL0_INT.
 */
#define UHCI_APP_CTRL0_INT_ST    (BIT(7))
#define UHCI_APP_CTRL0_INT_ST_M  (UHCI_APP_CTRL0_INT_ST_V << UHCI_APP_CTRL0_INT_ST_S)
#define UHCI_APP_CTRL0_INT_ST_V  0x00000001U
#define UHCI_APP_CTRL0_INT_ST_S  7
/** UHCI_APP_CTRL1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status of UHCI_APP_CTRL1_INT.
 */
#define UHCI_APP_CTRL1_INT_ST    (BIT(8))
#define UHCI_APP_CTRL1_INT_ST_M  (UHCI_APP_CTRL1_INT_ST_V << UHCI_APP_CTRL1_INT_ST_S)
#define UHCI_APP_CTRL1_INT_ST_V  0x00000001U
#define UHCI_APP_CTRL1_INT_ST_S  8

/** UHCI_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define UHCI_INT_ENA_REG(i) (REG_UHCI_BASE(i) + 0xc)
/** UHCI_RX_START_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable UHCI_RX_START_INT.
 */
#define UHCI_RX_START_INT_ENA    (BIT(0))
#define UHCI_RX_START_INT_ENA_M  (UHCI_RX_START_INT_ENA_V << UHCI_RX_START_INT_ENA_S)
#define UHCI_RX_START_INT_ENA_V  0x00000001U
#define UHCI_RX_START_INT_ENA_S  0
/** UHCI_TX_START_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable UHCI_TX_START_INT.
 */
#define UHCI_TX_START_INT_ENA    (BIT(1))
#define UHCI_TX_START_INT_ENA_M  (UHCI_TX_START_INT_ENA_V << UHCI_TX_START_INT_ENA_S)
#define UHCI_TX_START_INT_ENA_V  0x00000001U
#define UHCI_TX_START_INT_ENA_S  1
/** UHCI_RX_HUNG_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable UHCI_RX_HUNG_INT.
 */
#define UHCI_RX_HUNG_INT_ENA    (BIT(2))
#define UHCI_RX_HUNG_INT_ENA_M  (UHCI_RX_HUNG_INT_ENA_V << UHCI_RX_HUNG_INT_ENA_S)
#define UHCI_RX_HUNG_INT_ENA_V  0x00000001U
#define UHCI_RX_HUNG_INT_ENA_S  2
/** UHCI_TX_HUNG_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Write 1 to enable UHCI_TX_HUNG_INT.
 */
#define UHCI_TX_HUNG_INT_ENA    (BIT(3))
#define UHCI_TX_HUNG_INT_ENA_M  (UHCI_TX_HUNG_INT_ENA_V << UHCI_TX_HUNG_INT_ENA_S)
#define UHCI_TX_HUNG_INT_ENA_V  0x00000001U
#define UHCI_TX_HUNG_INT_ENA_S  3
/** UHCI_SEND_S_REG_Q_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable UHCI_SEND_S_REG_Q_INT.
 */
#define UHCI_SEND_S_REG_Q_INT_ENA    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_ENA_M  (UHCI_SEND_S_REG_Q_INT_ENA_V << UHCI_SEND_S_REG_Q_INT_ENA_S)
#define UHCI_SEND_S_REG_Q_INT_ENA_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_ENA_S  4
/** UHCI_SEND_A_REG_Q_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Write 1 to enable UHCI_SEND_A_REG_Q_INT.
 */
#define UHCI_SEND_A_REG_Q_INT_ENA    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_ENA_M  (UHCI_SEND_A_REG_Q_INT_ENA_V << UHCI_SEND_A_REG_Q_INT_ENA_S)
#define UHCI_SEND_A_REG_Q_INT_ENA_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_ENA_S  5
/** UHCI_OUTLINK_EOF_ERR_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Write 1 to enable UHCI_OUTLINK_EOF_ERR_INT.
 */
#define UHCI_OUTLINK_EOF_ERR_INT_ENA    (BIT(6))
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_M  (UHCI_OUTLINK_EOF_ERR_INT_ENA_V << UHCI_OUTLINK_EOF_ERR_INT_ENA_S)
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_V  0x00000001U
#define UHCI_OUTLINK_EOF_ERR_INT_ENA_S  6
/** UHCI_APP_CTRL0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Write 1 to enable UHCI_APP_CTRL0_INT.
 */
#define UHCI_APP_CTRL0_INT_ENA    (BIT(7))
#define UHCI_APP_CTRL0_INT_ENA_M  (UHCI_APP_CTRL0_INT_ENA_V << UHCI_APP_CTRL0_INT_ENA_S)
#define UHCI_APP_CTRL0_INT_ENA_V  0x00000001U
#define UHCI_APP_CTRL0_INT_ENA_S  7
/** UHCI_APP_CTRL1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Write 1 to enable UHCI_APP_CTRL1_INT.
 */
#define UHCI_APP_CTRL1_INT_ENA    (BIT(8))
#define UHCI_APP_CTRL1_INT_ENA_M  (UHCI_APP_CTRL1_INT_ENA_V << UHCI_APP_CTRL1_INT_ENA_S)
#define UHCI_APP_CTRL1_INT_ENA_V  0x00000001U
#define UHCI_APP_CTRL1_INT_ENA_S  8

/** UHCI_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define UHCI_INT_CLR_REG(i) (REG_UHCI_BASE(i) + 0x10)
/** UHCI_RX_START_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear UHCI_RX_START_INT.
 */
#define UHCI_RX_START_INT_CLR    (BIT(0))
#define UHCI_RX_START_INT_CLR_M  (UHCI_RX_START_INT_CLR_V << UHCI_RX_START_INT_CLR_S)
#define UHCI_RX_START_INT_CLR_V  0x00000001U
#define UHCI_RX_START_INT_CLR_S  0
/** UHCI_TX_START_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear UHCI_TX_START_INT.
 */
#define UHCI_TX_START_INT_CLR    (BIT(1))
#define UHCI_TX_START_INT_CLR_M  (UHCI_TX_START_INT_CLR_V << UHCI_TX_START_INT_CLR_S)
#define UHCI_TX_START_INT_CLR_V  0x00000001U
#define UHCI_TX_START_INT_CLR_S  1
/** UHCI_RX_HUNG_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear UHCI_RX_HUNG_INT.
 */
#define UHCI_RX_HUNG_INT_CLR    (BIT(2))
#define UHCI_RX_HUNG_INT_CLR_M  (UHCI_RX_HUNG_INT_CLR_V << UHCI_RX_HUNG_INT_CLR_S)
#define UHCI_RX_HUNG_INT_CLR_V  0x00000001U
#define UHCI_RX_HUNG_INT_CLR_S  2
/** UHCI_TX_HUNG_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Write 1 to clear UHCI_TX_HUNG_INT.
 */
#define UHCI_TX_HUNG_INT_CLR    (BIT(3))
#define UHCI_TX_HUNG_INT_CLR_M  (UHCI_TX_HUNG_INT_CLR_V << UHCI_TX_HUNG_INT_CLR_S)
#define UHCI_TX_HUNG_INT_CLR_V  0x00000001U
#define UHCI_TX_HUNG_INT_CLR_S  3
/** UHCI_SEND_S_REG_Q_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear UHCI_SEND_S_REG_Q_INT.
 */
#define UHCI_SEND_S_REG_Q_INT_CLR    (BIT(4))
#define UHCI_SEND_S_REG_Q_INT_CLR_M  (UHCI_SEND_S_REG_Q_INT_CLR_V << UHCI_SEND_S_REG_Q_INT_CLR_S)
#define UHCI_SEND_S_REG_Q_INT_CLR_V  0x00000001U
#define UHCI_SEND_S_REG_Q_INT_CLR_S  4
/** UHCI_SEND_A_REG_Q_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Write 1 to clear UHCI_SEND_A_REG_Q_INT.
 */
#define UHCI_SEND_A_REG_Q_INT_CLR    (BIT(5))
#define UHCI_SEND_A_REG_Q_INT_CLR_M  (UHCI_SEND_A_REG_Q_INT_CLR_V << UHCI_SEND_A_REG_Q_INT_CLR_S)
#define UHCI_SEND_A_REG_Q_INT_CLR_V  0x00000001U
#define UHCI_SEND_A_REG_Q_INT_CLR_S  5
/** UHCI_OUTLINK_EOF_ERR_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Write 1 to clear UHCI_OUTLINK_EOF_ERR_INT.
 */
#define UHCI_OUTLINK_EOF_ERR_INT_CLR    (BIT(6))
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_M  (UHCI_OUTLINK_EOF_ERR_INT_CLR_V << UHCI_OUTLINK_EOF_ERR_INT_CLR_S)
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_V  0x00000001U
#define UHCI_OUTLINK_EOF_ERR_INT_CLR_S  6
/** UHCI_APP_CTRL0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Write 1 to clear UHCI_APP_CTRL0_INT.
 */
#define UHCI_APP_CTRL0_INT_CLR    (BIT(7))
#define UHCI_APP_CTRL0_INT_CLR_M  (UHCI_APP_CTRL0_INT_CLR_V << UHCI_APP_CTRL0_INT_CLR_S)
#define UHCI_APP_CTRL0_INT_CLR_V  0x00000001U
#define UHCI_APP_CTRL0_INT_CLR_S  7
/** UHCI_APP_CTRL1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Write 1 to clear UHCI_APP_CTRL1_INT.
 */
#define UHCI_APP_CTRL1_INT_CLR    (BIT(8))
#define UHCI_APP_CTRL1_INT_CLR_M  (UHCI_APP_CTRL1_INT_CLR_V << UHCI_APP_CTRL1_INT_CLR_S)
#define UHCI_APP_CTRL1_INT_CLR_V  0x00000001U
#define UHCI_APP_CTRL1_INT_CLR_S  8

/** UHCI_CONF1_REG register
 *  UHCI configuration register
 */
#define UHCI_CONF1_REG(i) (REG_UHCI_BASE(i) + 0x14)
/** UHCI_CHECK_SUM_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to enable header checksum validation when UHCI receives a
 *  data packet.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UHCI_CHECK_SUM_EN    (BIT(0))
#define UHCI_CHECK_SUM_EN_M  (UHCI_CHECK_SUM_EN_V << UHCI_CHECK_SUM_EN_S)
#define UHCI_CHECK_SUM_EN_V  0x00000001U
#define UHCI_CHECK_SUM_EN_S  0
/** UHCI_CHECK_SEQ_EN : R/W; bitpos: [1]; default: 1;
 *  Configures whether or not to enable the sequence number check when UHCI receives a
 *  data packet.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UHCI_CHECK_SEQ_EN    (BIT(1))
#define UHCI_CHECK_SEQ_EN_M  (UHCI_CHECK_SEQ_EN_V << UHCI_CHECK_SEQ_EN_S)
#define UHCI_CHECK_SEQ_EN_V  0x00000001U
#define UHCI_CHECK_SEQ_EN_S  1
/** UHCI_CRC_DISABLE : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable CRC calculation.\\
 *  0: Disable\\
 *  1: Enable\\
 *  Valid only when the Data Integrity Check Present bit in UHCI packet is 1.\\
 */
#define UHCI_CRC_DISABLE    (BIT(2))
#define UHCI_CRC_DISABLE_M  (UHCI_CRC_DISABLE_V << UHCI_CRC_DISABLE_S)
#define UHCI_CRC_DISABLE_V  0x00000001U
#define UHCI_CRC_DISABLE_S  2
/** UHCI_SAVE_HEAD : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to save the packet header when UHCI receives a data
 *  packet.\\
 *  0: Not save\\
 *  1: Save\\
 */
#define UHCI_SAVE_HEAD    (BIT(3))
#define UHCI_SAVE_HEAD_M  (UHCI_SAVE_HEAD_V << UHCI_SAVE_HEAD_S)
#define UHCI_SAVE_HEAD_V  0x00000001U
#define UHCI_SAVE_HEAD_S  3
/** UHCI_TX_CHECK_SUM_RE : R/W; bitpos: [4]; default: 1;
 *  Configures whether or not to encode the data packet with a checksum.\\
 *  0: Not use checksum\\
 *  1: Use checksum\\
 */
#define UHCI_TX_CHECK_SUM_RE    (BIT(4))
#define UHCI_TX_CHECK_SUM_RE_M  (UHCI_TX_CHECK_SUM_RE_V << UHCI_TX_CHECK_SUM_RE_S)
#define UHCI_TX_CHECK_SUM_RE_V  0x00000001U
#define UHCI_TX_CHECK_SUM_RE_S  4
/** UHCI_TX_ACK_NUM_RE : R/W; bitpos: [5]; default: 1;
 *  Configures whether or not to encode the data packet with an acknowledgment when a
 *  reliable packet is to be transmitted.\\
 *  0: Not use acknowledgement\\
 *  1: Use acknowledgement\\
 */
#define UHCI_TX_ACK_NUM_RE    (BIT(5))
#define UHCI_TX_ACK_NUM_RE_M  (UHCI_TX_ACK_NUM_RE_V << UHCI_TX_ACK_NUM_RE_S)
#define UHCI_TX_ACK_NUM_RE_V  0x00000001U
#define UHCI_TX_ACK_NUM_RE_S  5
/** UHCI_WAIT_SW_START : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to put the UHCI encoder state machine to ST_SW_WAIT
 *  state.\\
 *  0: No\\
 *  1: Yes\\
 */
#define UHCI_WAIT_SW_START    (BIT(7))
#define UHCI_WAIT_SW_START_M  (UHCI_WAIT_SW_START_V << UHCI_WAIT_SW_START_S)
#define UHCI_WAIT_SW_START_V  0x00000001U
#define UHCI_WAIT_SW_START_S  7
/** UHCI_SW_START : WT; bitpos: [8]; default: 0;
 *  Write 1 to send data packets when the encoder state machine is in ST_SW_WAIT state.
 */
#define UHCI_SW_START    (BIT(8))
#define UHCI_SW_START_M  (UHCI_SW_START_V << UHCI_SW_START_S)
#define UHCI_SW_START_V  0x00000001U
#define UHCI_SW_START_S  8

/** UHCI_STATE0_REG register
 *  UHCI receive status
 */
#define UHCI_STATE0_REG(i) (REG_UHCI_BASE(i) + 0x18)
/** UHCI_RX_ERR_CAUSE : RO; bitpos: [2:0]; default: 0;
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
#define UHCI_RX_ERR_CAUSE    0x00000007U
#define UHCI_RX_ERR_CAUSE_M  (UHCI_RX_ERR_CAUSE_V << UHCI_RX_ERR_CAUSE_S)
#define UHCI_RX_ERR_CAUSE_V  0x00000007U
#define UHCI_RX_ERR_CAUSE_S  0
/** UHCI_DECODE_STATE : RO; bitpos: [5:3]; default: 0;
 *  Represents the UHCI decoder status.
 */
#define UHCI_DECODE_STATE    0x00000007U
#define UHCI_DECODE_STATE_M  (UHCI_DECODE_STATE_V << UHCI_DECODE_STATE_S)
#define UHCI_DECODE_STATE_V  0x00000007U
#define UHCI_DECODE_STATE_S  3

/** UHCI_STATE1_REG register
 *  UHCI transmit status
 */
#define UHCI_STATE1_REG(i) (REG_UHCI_BASE(i) + 0x1c)
/** UHCI_ENCODE_STATE : RO; bitpos: [2:0]; default: 0;
 *  Represents the UHCI encoder status.
 */
#define UHCI_ENCODE_STATE    0x00000007U
#define UHCI_ENCODE_STATE_M  (UHCI_ENCODE_STATE_V << UHCI_ENCODE_STATE_S)
#define UHCI_ENCODE_STATE_V  0x00000007U
#define UHCI_ENCODE_STATE_S  0

/** UHCI_ESCAPE_CONF_REG register
 *  Escape character configuration
 */
#define UHCI_ESCAPE_CONF_REG(i) (REG_UHCI_BASE(i) + 0x20)
/** UHCI_TX_C0_ESC_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to decode character 0xC0 when DMA receives data.\\
 *  0: Not decode\\
 *  1: Decode\\
 */
#define UHCI_TX_C0_ESC_EN    (BIT(0))
#define UHCI_TX_C0_ESC_EN_M  (UHCI_TX_C0_ESC_EN_V << UHCI_TX_C0_ESC_EN_S)
#define UHCI_TX_C0_ESC_EN_V  0x00000001U
#define UHCI_TX_C0_ESC_EN_S  0
/** UHCI_TX_DB_ESC_EN : R/W; bitpos: [1]; default: 1;
 *  Configures whether or not to decode character 0xDB when DMA receives data.\\
 *  0: Not decode\\
 *  1: Decode\\
 */
#define UHCI_TX_DB_ESC_EN    (BIT(1))
#define UHCI_TX_DB_ESC_EN_M  (UHCI_TX_DB_ESC_EN_V << UHCI_TX_DB_ESC_EN_S)
#define UHCI_TX_DB_ESC_EN_V  0x00000001U
#define UHCI_TX_DB_ESC_EN_S  1
/** UHCI_TX_11_ESC_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to decode flow control character 0x11 when DMA receives
 *  data.\\
 *  0: Not decode\\
 *  1: Decode\\
 */
#define UHCI_TX_11_ESC_EN    (BIT(2))
#define UHCI_TX_11_ESC_EN_M  (UHCI_TX_11_ESC_EN_V << UHCI_TX_11_ESC_EN_S)
#define UHCI_TX_11_ESC_EN_V  0x00000001U
#define UHCI_TX_11_ESC_EN_S  2
/** UHCI_TX_13_ESC_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to decode flow control character 0x13 when DMA receives
 *  data.\\
 *  0: Not decode\\
 *  1: Decode\\
 */
#define UHCI_TX_13_ESC_EN    (BIT(3))
#define UHCI_TX_13_ESC_EN_M  (UHCI_TX_13_ESC_EN_V << UHCI_TX_13_ESC_EN_S)
#define UHCI_TX_13_ESC_EN_V  0x00000001U
#define UHCI_TX_13_ESC_EN_S  3
/** UHCI_RX_C0_ESC_EN : R/W; bitpos: [4]; default: 1;
 *  Configures whether or not to replace 0xC0 by special characters when DMA sends
 *  data.\\
 *  0: Not replace\\
 *  1: Replace\\
 */
#define UHCI_RX_C0_ESC_EN    (BIT(4))
#define UHCI_RX_C0_ESC_EN_M  (UHCI_RX_C0_ESC_EN_V << UHCI_RX_C0_ESC_EN_S)
#define UHCI_RX_C0_ESC_EN_V  0x00000001U
#define UHCI_RX_C0_ESC_EN_S  4
/** UHCI_RX_DB_ESC_EN : R/W; bitpos: [5]; default: 1;
 *  Configures whether or not to replace 0xDB by special characters when DMA sends
 *  data.\\
 *  0: Not replace\\
 *  1: Replace\\
 */
#define UHCI_RX_DB_ESC_EN    (BIT(5))
#define UHCI_RX_DB_ESC_EN_M  (UHCI_RX_DB_ESC_EN_V << UHCI_RX_DB_ESC_EN_S)
#define UHCI_RX_DB_ESC_EN_V  0x00000001U
#define UHCI_RX_DB_ESC_EN_S  5
/** UHCI_RX_11_ESC_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to replace flow control character 0x11 by special
 *  characters when DMA sends data.\\
 *  0: Not replace\\
 *  1: Replace\\
 */
#define UHCI_RX_11_ESC_EN    (BIT(6))
#define UHCI_RX_11_ESC_EN_M  (UHCI_RX_11_ESC_EN_V << UHCI_RX_11_ESC_EN_S)
#define UHCI_RX_11_ESC_EN_V  0x00000001U
#define UHCI_RX_11_ESC_EN_S  6
/** UHCI_RX_13_ESC_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to replace flow control character 0x13 by special
 *  characters when DMA sends data.\\
 *  0: Not replace\\
 *  1: Replace\\
 */
#define UHCI_RX_13_ESC_EN    (BIT(7))
#define UHCI_RX_13_ESC_EN_M  (UHCI_RX_13_ESC_EN_V << UHCI_RX_13_ESC_EN_S)
#define UHCI_RX_13_ESC_EN_V  0x00000001U
#define UHCI_RX_13_ESC_EN_S  7

/** UHCI_HUNG_CONF_REG register
 *  Timeout configuration
 */
#define UHCI_HUNG_CONF_REG(i) (REG_UHCI_BASE(i) + 0x24)
/** UHCI_TXFIFO_TIMEOUT : R/W; bitpos: [7:0]; default: 16;
 *  Configures the timeout value for DMA data reception.\\Measurement unit: ms.
 */
#define UHCI_TXFIFO_TIMEOUT    0x000000FFU
#define UHCI_TXFIFO_TIMEOUT_M  (UHCI_TXFIFO_TIMEOUT_V << UHCI_TXFIFO_TIMEOUT_S)
#define UHCI_TXFIFO_TIMEOUT_V  0x000000FFU
#define UHCI_TXFIFO_TIMEOUT_S  0
/** UHCI_TXFIFO_TIMEOUT_SHIFT : R/W; bitpos: [10:8]; default: 0;
 *  Configures the upper limit of the timeout counter for TX FIFO.
 */
#define UHCI_TXFIFO_TIMEOUT_SHIFT    0x00000007U
#define UHCI_TXFIFO_TIMEOUT_SHIFT_M  (UHCI_TXFIFO_TIMEOUT_SHIFT_V << UHCI_TXFIFO_TIMEOUT_SHIFT_S)
#define UHCI_TXFIFO_TIMEOUT_SHIFT_V  0x00000007U
#define UHCI_TXFIFO_TIMEOUT_SHIFT_S  8
/** UHCI_TXFIFO_TIMEOUT_ENA : R/W; bitpos: [11]; default: 1;
 *  Configures whether or not to enable the data reception timeout for TX FIFO.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UHCI_TXFIFO_TIMEOUT_ENA    (BIT(11))
#define UHCI_TXFIFO_TIMEOUT_ENA_M  (UHCI_TXFIFO_TIMEOUT_ENA_V << UHCI_TXFIFO_TIMEOUT_ENA_S)
#define UHCI_TXFIFO_TIMEOUT_ENA_V  0x00000001U
#define UHCI_TXFIFO_TIMEOUT_ENA_S  11
/** UHCI_RXFIFO_TIMEOUT : R/W; bitpos: [19:12]; default: 16;
 *  Configures the timeout value for DMA to read data from RAM.\\Measurement unit: ms.
 */
#define UHCI_RXFIFO_TIMEOUT    0x000000FFU
#define UHCI_RXFIFO_TIMEOUT_M  (UHCI_RXFIFO_TIMEOUT_V << UHCI_RXFIFO_TIMEOUT_S)
#define UHCI_RXFIFO_TIMEOUT_V  0x000000FFU
#define UHCI_RXFIFO_TIMEOUT_S  12
/** UHCI_RXFIFO_TIMEOUT_SHIFT : R/W; bitpos: [22:20]; default: 0;
 *  Configures the upper limit of the timeout counter for RX FIFO.
 */
#define UHCI_RXFIFO_TIMEOUT_SHIFT    0x00000007U
#define UHCI_RXFIFO_TIMEOUT_SHIFT_M  (UHCI_RXFIFO_TIMEOUT_SHIFT_V << UHCI_RXFIFO_TIMEOUT_SHIFT_S)
#define UHCI_RXFIFO_TIMEOUT_SHIFT_V  0x00000007U
#define UHCI_RXFIFO_TIMEOUT_SHIFT_S  20
/** UHCI_RXFIFO_TIMEOUT_ENA : R/W; bitpos: [23]; default: 1;
 *  Configures whether or not to enable the DMA data transmission timeout.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UHCI_RXFIFO_TIMEOUT_ENA    (BIT(23))
#define UHCI_RXFIFO_TIMEOUT_ENA_M  (UHCI_RXFIFO_TIMEOUT_ENA_V << UHCI_RXFIFO_TIMEOUT_ENA_S)
#define UHCI_RXFIFO_TIMEOUT_ENA_V  0x00000001U
#define UHCI_RXFIFO_TIMEOUT_ENA_S  23

/** UHCI_ACK_NUM_REG register
 *  UHCI ACK number configuration
 */
#define UHCI_ACK_NUM_REG(i) (REG_UHCI_BASE(i) + 0x28)
/** UHCI_ACK_NUM : R/W; bitpos: [2:0]; default: 0;
 *  Configures the number of acknowledgements used in software flow control.
 */
#define UHCI_ACK_NUM    0x00000007U
#define UHCI_ACK_NUM_M  (UHCI_ACK_NUM_V << UHCI_ACK_NUM_S)
#define UHCI_ACK_NUM_V  0x00000007U
#define UHCI_ACK_NUM_S  0
/** UHCI_ACK_NUM_LOAD : WT; bitpos: [3]; default: 0;
 *  Configures whether or not load acknowledgements.\\
 *  0: Not load\\
 *  1: Load\\
 */
#define UHCI_ACK_NUM_LOAD    (BIT(3))
#define UHCI_ACK_NUM_LOAD_M  (UHCI_ACK_NUM_LOAD_V << UHCI_ACK_NUM_LOAD_S)
#define UHCI_ACK_NUM_LOAD_V  0x00000001U
#define UHCI_ACK_NUM_LOAD_S  3

/** UHCI_RX_HEAD_REG register
 *  UHCI packet header register
 */
#define UHCI_RX_HEAD_REG(i) (REG_UHCI_BASE(i) + 0x2c)
/** UHCI_RX_HEAD : RO; bitpos: [31:0]; default: 0;
 *  Represents the header of the current received packet.
 */
#define UHCI_RX_HEAD    0xFFFFFFFFU
#define UHCI_RX_HEAD_M  (UHCI_RX_HEAD_V << UHCI_RX_HEAD_S)
#define UHCI_RX_HEAD_V  0xFFFFFFFFU
#define UHCI_RX_HEAD_S  0

/** UHCI_QUICK_SENT_REG register
 *  UHCI quick send configuration register
 */
#define UHCI_QUICK_SENT_REG(i) (REG_UHCI_BASE(i) + 0x30)
/** UHCI_SINGLE_SEND_NUM : R/W; bitpos: [2:0]; default: 0;
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
#define UHCI_SINGLE_SEND_NUM    0x00000007U
#define UHCI_SINGLE_SEND_NUM_M  (UHCI_SINGLE_SEND_NUM_V << UHCI_SINGLE_SEND_NUM_S)
#define UHCI_SINGLE_SEND_NUM_V  0x00000007U
#define UHCI_SINGLE_SEND_NUM_S  0
/** UHCI_SINGLE_SEND_EN : WT; bitpos: [3]; default: 0;
 *  Write 1 to enable single_send mode.
 */
#define UHCI_SINGLE_SEND_EN    (BIT(3))
#define UHCI_SINGLE_SEND_EN_M  (UHCI_SINGLE_SEND_EN_V << UHCI_SINGLE_SEND_EN_S)
#define UHCI_SINGLE_SEND_EN_V  0x00000001U
#define UHCI_SINGLE_SEND_EN_S  3
/** UHCI_ALWAYS_SEND_NUM : R/W; bitpos: [6:4]; default: 0;
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
#define UHCI_ALWAYS_SEND_NUM    0x00000007U
#define UHCI_ALWAYS_SEND_NUM_M  (UHCI_ALWAYS_SEND_NUM_V << UHCI_ALWAYS_SEND_NUM_S)
#define UHCI_ALWAYS_SEND_NUM_V  0x00000007U
#define UHCI_ALWAYS_SEND_NUM_S  4
/** UHCI_ALWAYS_SEND_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable always_send mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UHCI_ALWAYS_SEND_EN    (BIT(7))
#define UHCI_ALWAYS_SEND_EN_M  (UHCI_ALWAYS_SEND_EN_V << UHCI_ALWAYS_SEND_EN_S)
#define UHCI_ALWAYS_SEND_EN_V  0x00000001U
#define UHCI_ALWAYS_SEND_EN_S  7

/** UHCI_REG_Q0_WORD0_REG register
 *  Q0 WORD0 quick send register
 */
#define UHCI_REG_Q0_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x34)
/** UHCI_SEND_Q0_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q0 register.
 */
#define UHCI_SEND_Q0_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD0_M  (UHCI_SEND_Q0_WORD0_V << UHCI_SEND_Q0_WORD0_S)
#define UHCI_SEND_Q0_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD0_S  0

/** UHCI_REG_Q0_WORD1_REG register
 *  Q0 WORD1 quick send register
 */
#define UHCI_REG_Q0_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x38)
/** UHCI_SEND_Q0_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q0 register.
 */
#define UHCI_SEND_Q0_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD1_M  (UHCI_SEND_Q0_WORD1_V << UHCI_SEND_Q0_WORD1_S)
#define UHCI_SEND_Q0_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q0_WORD1_S  0

/** UHCI_REG_Q1_WORD0_REG register
 *  Q1 WORD0 quick send register
 */
#define UHCI_REG_Q1_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x3c)
/** UHCI_SEND_Q1_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q1 register.
 */
#define UHCI_SEND_Q1_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD0_M  (UHCI_SEND_Q1_WORD0_V << UHCI_SEND_Q1_WORD0_S)
#define UHCI_SEND_Q1_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD0_S  0

/** UHCI_REG_Q1_WORD1_REG register
 *  Q1 WORD1 quick send register
 */
#define UHCI_REG_Q1_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x40)
/** UHCI_SEND_Q1_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q1 register.
 */
#define UHCI_SEND_Q1_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD1_M  (UHCI_SEND_Q1_WORD1_V << UHCI_SEND_Q1_WORD1_S)
#define UHCI_SEND_Q1_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q1_WORD1_S  0

/** UHCI_REG_Q2_WORD0_REG register
 *  Q2 WORD0 quick send register
 */
#define UHCI_REG_Q2_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x44)
/** UHCI_SEND_Q2_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q2 register.
 */
#define UHCI_SEND_Q2_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD0_M  (UHCI_SEND_Q2_WORD0_V << UHCI_SEND_Q2_WORD0_S)
#define UHCI_SEND_Q2_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD0_S  0

/** UHCI_REG_Q2_WORD1_REG register
 *  Q2 WORD1 quick send register
 */
#define UHCI_REG_Q2_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x48)
/** UHCI_SEND_Q2_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q2 register.
 */
#define UHCI_SEND_Q2_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD1_M  (UHCI_SEND_Q2_WORD1_V << UHCI_SEND_Q2_WORD1_S)
#define UHCI_SEND_Q2_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q2_WORD1_S  0

/** UHCI_REG_Q3_WORD0_REG register
 *  Q3 WORD0 quick send register
 */
#define UHCI_REG_Q3_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x4c)
/** UHCI_SEND_Q3_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q3 register.
 */
#define UHCI_SEND_Q3_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD0_M  (UHCI_SEND_Q3_WORD0_V << UHCI_SEND_Q3_WORD0_S)
#define UHCI_SEND_Q3_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD0_S  0

/** UHCI_REG_Q3_WORD1_REG register
 *  Q3 WORD1 quick send register
 */
#define UHCI_REG_Q3_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x50)
/** UHCI_SEND_Q3_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q3 register.
 */
#define UHCI_SEND_Q3_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD1_M  (UHCI_SEND_Q3_WORD1_V << UHCI_SEND_Q3_WORD1_S)
#define UHCI_SEND_Q3_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q3_WORD1_S  0

/** UHCI_REG_Q4_WORD0_REG register
 *  Q4 WORD0 quick send register
 */
#define UHCI_REG_Q4_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x54)
/** UHCI_SEND_Q4_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q4 register.
 */
#define UHCI_SEND_Q4_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD0_M  (UHCI_SEND_Q4_WORD0_V << UHCI_SEND_Q4_WORD0_S)
#define UHCI_SEND_Q4_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD0_S  0

/** UHCI_REG_Q4_WORD1_REG register
 *  Q4 WORD1 quick send register
 */
#define UHCI_REG_Q4_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x58)
/** UHCI_SEND_Q4_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q4 register.
 */
#define UHCI_SEND_Q4_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD1_M  (UHCI_SEND_Q4_WORD1_V << UHCI_SEND_Q4_WORD1_S)
#define UHCI_SEND_Q4_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q4_WORD1_S  0

/** UHCI_REG_Q5_WORD0_REG register
 *  Q5 WORD0 quick send register
 */
#define UHCI_REG_Q5_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x5c)
/** UHCI_SEND_Q5_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q5 register.
 */
#define UHCI_SEND_Q5_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD0_M  (UHCI_SEND_Q5_WORD0_V << UHCI_SEND_Q5_WORD0_S)
#define UHCI_SEND_Q5_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD0_S  0

/** UHCI_REG_Q5_WORD1_REG register
 *  Q5 WORD1 quick send register
 */
#define UHCI_REG_Q5_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x60)
/** UHCI_SEND_Q5_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q5 register.
 */
#define UHCI_SEND_Q5_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD1_M  (UHCI_SEND_Q5_WORD1_V << UHCI_SEND_Q5_WORD1_S)
#define UHCI_SEND_Q5_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q5_WORD1_S  0

/** UHCI_REG_Q6_WORD0_REG register
 *  Q6 WORD0 quick send register
 */
#define UHCI_REG_Q6_WORD0_REG(i) (REG_UHCI_BASE(i) + 0x64)
/** UHCI_SEND_Q6_WORD0 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q6 register.
 */
#define UHCI_SEND_Q6_WORD0    0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD0_M  (UHCI_SEND_Q6_WORD0_V << UHCI_SEND_Q6_WORD0_S)
#define UHCI_SEND_Q6_WORD0_V  0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD0_S  0

/** UHCI_REG_Q6_WORD1_REG register
 *  Q6 WORD1 quick register
 */
#define UHCI_REG_Q6_WORD1_REG(i) (REG_UHCI_BASE(i) + 0x68)
/** UHCI_SEND_Q6_WORD1 : R/W; bitpos: [31:0]; default: 0;
 *  Data to be transmitted in Q6 register.
 */
#define UHCI_SEND_Q6_WORD1    0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD1_M  (UHCI_SEND_Q6_WORD1_V << UHCI_SEND_Q6_WORD1_S)
#define UHCI_SEND_Q6_WORD1_V  0xFFFFFFFFU
#define UHCI_SEND_Q6_WORD1_S  0

/** UHCI_ESC_CONF0_REG register
 *  Escape sequence configuration register 0
 */
#define UHCI_ESC_CONF0_REG(i) (REG_UHCI_BASE(i) + 0x6c)
/** UHCI_SEPER_CHAR : R/W; bitpos: [7:0]; default: 192;
 *  Configures separators to encode data packets. The default value is 0xC0.
 */
#define UHCI_SEPER_CHAR    0x000000FFU
#define UHCI_SEPER_CHAR_M  (UHCI_SEPER_CHAR_V << UHCI_SEPER_CHAR_S)
#define UHCI_SEPER_CHAR_V  0x000000FFU
#define UHCI_SEPER_CHAR_S  0
/** UHCI_SEPER_ESC_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
 */
#define UHCI_SEPER_ESC_CHAR0    0x000000FFU
#define UHCI_SEPER_ESC_CHAR0_M  (UHCI_SEPER_ESC_CHAR0_V << UHCI_SEPER_ESC_CHAR0_S)
#define UHCI_SEPER_ESC_CHAR0_V  0x000000FFU
#define UHCI_SEPER_ESC_CHAR0_S  8
/** UHCI_SEPER_ESC_CHAR1 : R/W; bitpos: [23:16]; default: 220;
 *  Configures the second character of SLIP escape sequence. The default value is 0xDC.
 */
#define UHCI_SEPER_ESC_CHAR1    0x000000FFU
#define UHCI_SEPER_ESC_CHAR1_M  (UHCI_SEPER_ESC_CHAR1_V << UHCI_SEPER_ESC_CHAR1_S)
#define UHCI_SEPER_ESC_CHAR1_V  0x000000FFU
#define UHCI_SEPER_ESC_CHAR1_S  16

/** UHCI_ESC_CONF1_REG register
 *  Escape sequence configuration register 1
 */
#define UHCI_ESC_CONF1_REG(i) (REG_UHCI_BASE(i) + 0x70)
/** UHCI_ESC_SEQ0 : R/W; bitpos: [7:0]; default: 219;
 *  Configures the character that needs to be encoded. The default value is 0xDB used
 *  as the first character of SLIP escape sequence.
 */
#define UHCI_ESC_SEQ0    0x000000FFU
#define UHCI_ESC_SEQ0_M  (UHCI_ESC_SEQ0_V << UHCI_ESC_SEQ0_S)
#define UHCI_ESC_SEQ0_V  0x000000FFU
#define UHCI_ESC_SEQ0_S  0
/** UHCI_ESC_SEQ0_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
 */
#define UHCI_ESC_SEQ0_CHAR0    0x000000FFU
#define UHCI_ESC_SEQ0_CHAR0_M  (UHCI_ESC_SEQ0_CHAR0_V << UHCI_ESC_SEQ0_CHAR0_S)
#define UHCI_ESC_SEQ0_CHAR0_V  0x000000FFU
#define UHCI_ESC_SEQ0_CHAR0_S  8
/** UHCI_ESC_SEQ0_CHAR1 : R/W; bitpos: [23:16]; default: 221;
 *  Configures the second character of SLIP escape sequence. The default value is 0xDD.
 */
#define UHCI_ESC_SEQ0_CHAR1    0x000000FFU
#define UHCI_ESC_SEQ0_CHAR1_M  (UHCI_ESC_SEQ0_CHAR1_V << UHCI_ESC_SEQ0_CHAR1_S)
#define UHCI_ESC_SEQ0_CHAR1_V  0x000000FFU
#define UHCI_ESC_SEQ0_CHAR1_S  16

/** UHCI_ESC_CONF2_REG register
 *  Escape sequence configuration register 2
 */
#define UHCI_ESC_CONF2_REG(i) (REG_UHCI_BASE(i) + 0x74)
/** UHCI_ESC_SEQ1 : R/W; bitpos: [7:0]; default: 17;
 *  Configures a character that need to be encoded. The default value is 0x11 used as a
 *  flow control character.
 */
#define UHCI_ESC_SEQ1    0x000000FFU
#define UHCI_ESC_SEQ1_M  (UHCI_ESC_SEQ1_V << UHCI_ESC_SEQ1_S)
#define UHCI_ESC_SEQ1_V  0x000000FFU
#define UHCI_ESC_SEQ1_S  0
/** UHCI_ESC_SEQ1_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
 */
#define UHCI_ESC_SEQ1_CHAR0    0x000000FFU
#define UHCI_ESC_SEQ1_CHAR0_M  (UHCI_ESC_SEQ1_CHAR0_V << UHCI_ESC_SEQ1_CHAR0_S)
#define UHCI_ESC_SEQ1_CHAR0_V  0x000000FFU
#define UHCI_ESC_SEQ1_CHAR0_S  8
/** UHCI_ESC_SEQ1_CHAR1 : R/W; bitpos: [23:16]; default: 222;
 *  Configures the second character of SLIP escape sequence. The default value is 0xDE.
 */
#define UHCI_ESC_SEQ1_CHAR1    0x000000FFU
#define UHCI_ESC_SEQ1_CHAR1_M  (UHCI_ESC_SEQ1_CHAR1_V << UHCI_ESC_SEQ1_CHAR1_S)
#define UHCI_ESC_SEQ1_CHAR1_V  0x000000FFU
#define UHCI_ESC_SEQ1_CHAR1_S  16

/** UHCI_ESC_CONF3_REG register
 *  Escape sequence configuration register 3
 */
#define UHCI_ESC_CONF3_REG(i) (REG_UHCI_BASE(i) + 0x78)
/** UHCI_ESC_SEQ2 : R/W; bitpos: [7:0]; default: 19;
 *  Configures the character that needs to be decoded. The default value is 0x13 used
 *  as a flow control character.
 */
#define UHCI_ESC_SEQ2    0x000000FFU
#define UHCI_ESC_SEQ2_M  (UHCI_ESC_SEQ2_V << UHCI_ESC_SEQ2_S)
#define UHCI_ESC_SEQ2_V  0x000000FFU
#define UHCI_ESC_SEQ2_S  0
/** UHCI_ESC_SEQ2_CHAR0 : R/W; bitpos: [15:8]; default: 219;
 *  Configures the first character of SLIP escape sequence. The default value is 0xDB.
 */
#define UHCI_ESC_SEQ2_CHAR0    0x000000FFU
#define UHCI_ESC_SEQ2_CHAR0_M  (UHCI_ESC_SEQ2_CHAR0_V << UHCI_ESC_SEQ2_CHAR0_S)
#define UHCI_ESC_SEQ2_CHAR0_V  0x000000FFU
#define UHCI_ESC_SEQ2_CHAR0_S  8
/** UHCI_ESC_SEQ2_CHAR1 : R/W; bitpos: [23:16]; default: 223;
 *  Configures the second character of SLIP escape sequence. The default value is 0xDF.
 */
#define UHCI_ESC_SEQ2_CHAR1    0x000000FFU
#define UHCI_ESC_SEQ2_CHAR1_M  (UHCI_ESC_SEQ2_CHAR1_V << UHCI_ESC_SEQ2_CHAR1_S)
#define UHCI_ESC_SEQ2_CHAR1_V  0x000000FFU
#define UHCI_ESC_SEQ2_CHAR1_S  16

/** UHCI_PKT_THRES_REG register
 *  Configuration register for packet length
 */
#define UHCI_PKT_THRES_REG(i) (REG_UHCI_BASE(i) + 0x7c)
/** UHCI_PKT_THRS : R/W; bitpos: [12:0]; default: 128;
 *  Configures the maximum value of the packet length.\\Measurement unit: byte.\\Valid
 *  only when UHCI_HEAD_EN is 0.
 */
#define UHCI_PKT_THRS    0x00001FFFU
#define UHCI_PKT_THRS_M  (UHCI_PKT_THRS_V << UHCI_PKT_THRS_S)
#define UHCI_PKT_THRS_V  0x00001FFFU
#define UHCI_PKT_THRS_S  0

/** UHCI_DATE_REG register
 *  UHCI version  control register
 */
#define UHCI_DATE_REG(i) (REG_UHCI_BASE(i) + 0x80)
/** UHCI_DATE : R/W; bitpos: [31:0]; default: 35655936;
 *  Version control register.
 */
#define UHCI_DATE    0xFFFFFFFFU
#define UHCI_DATE_M  (UHCI_DATE_V << UHCI_DATE_S)
#define UHCI_DATE_V  0xFFFFFFFFU
#define UHCI_DATE_S  0

#ifdef __cplusplus
}
#endif
