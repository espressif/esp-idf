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

/** UART_FIFO_REG register
 *  FIFO data register
 */
#define UART_FIFO_REG(i) (REG_UART_BASE(i) + 0x0)
/** UART_RXFIFO_RD_BYTE : RO; bitpos: [7:0]; default: 0;
 *  Represents the data UART $n read from FIFO.\\
 *  Measurement unit: byte.
 */
#define UART_RXFIFO_RD_BYTE    0x000000FFU
#define UART_RXFIFO_RD_BYTE_M  (UART_RXFIFO_RD_BYTE_V << UART_RXFIFO_RD_BYTE_S)
#define UART_RXFIFO_RD_BYTE_V  0x000000FFU
#define UART_RXFIFO_RD_BYTE_S  0

/** UART_INT_RAW_REG register
 *  Raw interrupt status
 */
#define UART_INT_RAW_REG(i) (REG_UART_BASE(i) + 0x4)
/** UART_RXFIFO_FULL_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status of UART_RXFIFO_FULL_INT.
 */
#define UART_RXFIFO_FULL_INT_RAW    (BIT(0))
#define UART_RXFIFO_FULL_INT_RAW_M  (UART_RXFIFO_FULL_INT_RAW_V << UART_RXFIFO_FULL_INT_RAW_S)
#define UART_RXFIFO_FULL_INT_RAW_V  0x00000001U
#define UART_RXFIFO_FULL_INT_RAW_S  0
/** UART_TXFIFO_EMPTY_INT_RAW : R/WTC/SS; bitpos: [1]; default: 1;
 *  The raw interrupt status of UART_TXFIFO_EMPTY_INT.
 */
#define UART_TXFIFO_EMPTY_INT_RAW    (BIT(1))
#define UART_TXFIFO_EMPTY_INT_RAW_M  (UART_TXFIFO_EMPTY_INT_RAW_V << UART_TXFIFO_EMPTY_INT_RAW_S)
#define UART_TXFIFO_EMPTY_INT_RAW_V  0x00000001U
#define UART_TXFIFO_EMPTY_INT_RAW_S  1
/** UART_PARITY_ERR_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status of UART_PARITY_ERR_INT.
 */
#define UART_PARITY_ERR_INT_RAW    (BIT(2))
#define UART_PARITY_ERR_INT_RAW_M  (UART_PARITY_ERR_INT_RAW_V << UART_PARITY_ERR_INT_RAW_S)
#define UART_PARITY_ERR_INT_RAW_V  0x00000001U
#define UART_PARITY_ERR_INT_RAW_S  2
/** UART_FRM_ERR_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status of UART_FRM_ERR_INT.
 */
#define UART_FRM_ERR_INT_RAW    (BIT(3))
#define UART_FRM_ERR_INT_RAW_M  (UART_FRM_ERR_INT_RAW_V << UART_FRM_ERR_INT_RAW_S)
#define UART_FRM_ERR_INT_RAW_V  0x00000001U
#define UART_FRM_ERR_INT_RAW_S  3
/** UART_RXFIFO_OVF_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status of UART_RXFIFO_OVF_INT.
 */
#define UART_RXFIFO_OVF_INT_RAW    (BIT(4))
#define UART_RXFIFO_OVF_INT_RAW_M  (UART_RXFIFO_OVF_INT_RAW_V << UART_RXFIFO_OVF_INT_RAW_S)
#define UART_RXFIFO_OVF_INT_RAW_V  0x00000001U
#define UART_RXFIFO_OVF_INT_RAW_S  4
/** UART_DSR_CHG_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt status of UART_DSR_CHG_INT.
 */
#define UART_DSR_CHG_INT_RAW    (BIT(5))
#define UART_DSR_CHG_INT_RAW_M  (UART_DSR_CHG_INT_RAW_V << UART_DSR_CHG_INT_RAW_S)
#define UART_DSR_CHG_INT_RAW_V  0x00000001U
#define UART_DSR_CHG_INT_RAW_S  5
/** UART_CTS_CHG_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt status of UART_CTS_CHG_INT.
 */
#define UART_CTS_CHG_INT_RAW    (BIT(6))
#define UART_CTS_CHG_INT_RAW_M  (UART_CTS_CHG_INT_RAW_V << UART_CTS_CHG_INT_RAW_S)
#define UART_CTS_CHG_INT_RAW_V  0x00000001U
#define UART_CTS_CHG_INT_RAW_S  6
/** UART_BRK_DET_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt status of UART_BRK_DET_INT.
 */
#define UART_BRK_DET_INT_RAW    (BIT(7))
#define UART_BRK_DET_INT_RAW_M  (UART_BRK_DET_INT_RAW_V << UART_BRK_DET_INT_RAW_S)
#define UART_BRK_DET_INT_RAW_V  0x00000001U
#define UART_BRK_DET_INT_RAW_S  7
/** UART_RXFIFO_TOUT_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt status of UART_RXFIFO_TOUT_INT.
 */
#define UART_RXFIFO_TOUT_INT_RAW    (BIT(8))
#define UART_RXFIFO_TOUT_INT_RAW_M  (UART_RXFIFO_TOUT_INT_RAW_V << UART_RXFIFO_TOUT_INT_RAW_S)
#define UART_RXFIFO_TOUT_INT_RAW_V  0x00000001U
#define UART_RXFIFO_TOUT_INT_RAW_S  8
/** UART_SW_XON_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt status of UART_SW_XON_INT.
 */
#define UART_SW_XON_INT_RAW    (BIT(9))
#define UART_SW_XON_INT_RAW_M  (UART_SW_XON_INT_RAW_V << UART_SW_XON_INT_RAW_S)
#define UART_SW_XON_INT_RAW_V  0x00000001U
#define UART_SW_XON_INT_RAW_S  9
/** UART_SW_XOFF_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  UART_SW_XOFF_INT.
 */
#define UART_SW_XOFF_INT_RAW    (BIT(10))
#define UART_SW_XOFF_INT_RAW_M  (UART_SW_XOFF_INT_RAW_V << UART_SW_XOFF_INT_RAW_S)
#define UART_SW_XOFF_INT_RAW_V  0x00000001U
#define UART_SW_XOFF_INT_RAW_S  10
/** UART_GLITCH_DET_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt status of UART_GLITCH_DET_INT.
 */
#define UART_GLITCH_DET_INT_RAW    (BIT(11))
#define UART_GLITCH_DET_INT_RAW_M  (UART_GLITCH_DET_INT_RAW_V << UART_GLITCH_DET_INT_RAW_S)
#define UART_GLITCH_DET_INT_RAW_V  0x00000001U
#define UART_GLITCH_DET_INT_RAW_S  11
/** UART_TX_BRK_DONE_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt status of UART_TX_BRK_DONE_INT.
 */
#define UART_TX_BRK_DONE_INT_RAW    (BIT(12))
#define UART_TX_BRK_DONE_INT_RAW_M  (UART_TX_BRK_DONE_INT_RAW_V << UART_TX_BRK_DONE_INT_RAW_S)
#define UART_TX_BRK_DONE_INT_RAW_V  0x00000001U
#define UART_TX_BRK_DONE_INT_RAW_S  12
/** UART_TX_BRK_IDLE_DONE_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt status of UART_TX_BRK_IDLE_DONE_INT.
 */
#define UART_TX_BRK_IDLE_DONE_INT_RAW    (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_RAW_M  (UART_TX_BRK_IDLE_DONE_INT_RAW_V << UART_TX_BRK_IDLE_DONE_INT_RAW_S)
#define UART_TX_BRK_IDLE_DONE_INT_RAW_V  0x00000001U
#define UART_TX_BRK_IDLE_DONE_INT_RAW_S  13
/** UART_TX_DONE_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw interrupt status of UART_TX_DONE_INT.
 */
#define UART_TX_DONE_INT_RAW    (BIT(14))
#define UART_TX_DONE_INT_RAW_M  (UART_TX_DONE_INT_RAW_V << UART_TX_DONE_INT_RAW_S)
#define UART_TX_DONE_INT_RAW_V  0x00000001U
#define UART_TX_DONE_INT_RAW_S  14
/** UART_RS485_PARITY_ERR_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The raw interrupt status of UART_RS485_PARITY_ERR_INT.
 */
#define UART_RS485_PARITY_ERR_INT_RAW    (BIT(15))
#define UART_RS485_PARITY_ERR_INT_RAW_M  (UART_RS485_PARITY_ERR_INT_RAW_V << UART_RS485_PARITY_ERR_INT_RAW_S)
#define UART_RS485_PARITY_ERR_INT_RAW_V  0x00000001U
#define UART_RS485_PARITY_ERR_INT_RAW_S  15
/** UART_RS485_FRM_ERR_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  The raw interrupt status of UART_RS485_FRM_ERR_INT.
 */
#define UART_RS485_FRM_ERR_INT_RAW    (BIT(16))
#define UART_RS485_FRM_ERR_INT_RAW_M  (UART_RS485_FRM_ERR_INT_RAW_V << UART_RS485_FRM_ERR_INT_RAW_S)
#define UART_RS485_FRM_ERR_INT_RAW_V  0x00000001U
#define UART_RS485_FRM_ERR_INT_RAW_S  16
/** UART_RS485_CLASH_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  The raw interrupt status of UART_RS485_CLASH_INT.
 */
#define UART_RS485_CLASH_INT_RAW    (BIT(17))
#define UART_RS485_CLASH_INT_RAW_M  (UART_RS485_CLASH_INT_RAW_V << UART_RS485_CLASH_INT_RAW_S)
#define UART_RS485_CLASH_INT_RAW_V  0x00000001U
#define UART_RS485_CLASH_INT_RAW_S  17
/** UART_AT_CMD_CHAR_DET_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The raw interrupt status of UART_AT_CMD_CHAR_DET_INT.
 */
#define UART_AT_CMD_CHAR_DET_INT_RAW    (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_RAW_M  (UART_AT_CMD_CHAR_DET_INT_RAW_V << UART_AT_CMD_CHAR_DET_INT_RAW_S)
#define UART_AT_CMD_CHAR_DET_INT_RAW_V  0x00000001U
#define UART_AT_CMD_CHAR_DET_INT_RAW_S  18
/** UART_WAKEUP_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The raw interrupt status of UART_WAKEUP_INT.
 */
#define UART_WAKEUP_INT_RAW    (BIT(19))
#define UART_WAKEUP_INT_RAW_M  (UART_WAKEUP_INT_RAW_V << UART_WAKEUP_INT_RAW_S)
#define UART_WAKEUP_INT_RAW_V  0x00000001U
#define UART_WAKEUP_INT_RAW_S  19

/** UART_INT_ST_REG register
 *  Masked interrupt status
 */
#define UART_INT_ST_REG(i) (REG_UART_BASE(i) + 0x8)
/** UART_RXFIFO_FULL_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of UART_RXFIFO_FULL_INT.
 */
#define UART_RXFIFO_FULL_INT_ST    (BIT(0))
#define UART_RXFIFO_FULL_INT_ST_M  (UART_RXFIFO_FULL_INT_ST_V << UART_RXFIFO_FULL_INT_ST_S)
#define UART_RXFIFO_FULL_INT_ST_V  0x00000001U
#define UART_RXFIFO_FULL_INT_ST_S  0
/** UART_TXFIFO_EMPTY_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of UART_TXFIFO_EMPTY_INT.
 */
#define UART_TXFIFO_EMPTY_INT_ST    (BIT(1))
#define UART_TXFIFO_EMPTY_INT_ST_M  (UART_TXFIFO_EMPTY_INT_ST_V << UART_TXFIFO_EMPTY_INT_ST_S)
#define UART_TXFIFO_EMPTY_INT_ST_V  0x00000001U
#define UART_TXFIFO_EMPTY_INT_ST_S  1
/** UART_PARITY_ERR_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of UART_PARITY_ERR_INT.
 */
#define UART_PARITY_ERR_INT_ST    (BIT(2))
#define UART_PARITY_ERR_INT_ST_M  (UART_PARITY_ERR_INT_ST_V << UART_PARITY_ERR_INT_ST_S)
#define UART_PARITY_ERR_INT_ST_V  0x00000001U
#define UART_PARITY_ERR_INT_ST_S  2
/** UART_FRM_ERR_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status of UART_FRM_ERR_INT.
 */
#define UART_FRM_ERR_INT_ST    (BIT(3))
#define UART_FRM_ERR_INT_ST_M  (UART_FRM_ERR_INT_ST_V << UART_FRM_ERR_INT_ST_S)
#define UART_FRM_ERR_INT_ST_V  0x00000001U
#define UART_FRM_ERR_INT_ST_S  3
/** UART_RXFIFO_OVF_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status of UART_RXFIFO_OVF_INT.
 */
#define UART_RXFIFO_OVF_INT_ST    (BIT(4))
#define UART_RXFIFO_OVF_INT_ST_M  (UART_RXFIFO_OVF_INT_ST_V << UART_RXFIFO_OVF_INT_ST_S)
#define UART_RXFIFO_OVF_INT_ST_V  0x00000001U
#define UART_RXFIFO_OVF_INT_ST_S  4
/** UART_DSR_CHG_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status of UART_DSR_CHG_INT.
 */
#define UART_DSR_CHG_INT_ST    (BIT(5))
#define UART_DSR_CHG_INT_ST_M  (UART_DSR_CHG_INT_ST_V << UART_DSR_CHG_INT_ST_S)
#define UART_DSR_CHG_INT_ST_V  0x00000001U
#define UART_DSR_CHG_INT_ST_S  5
/** UART_CTS_CHG_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status of UART_CTS_CHG_INT.
 */
#define UART_CTS_CHG_INT_ST    (BIT(6))
#define UART_CTS_CHG_INT_ST_M  (UART_CTS_CHG_INT_ST_V << UART_CTS_CHG_INT_ST_S)
#define UART_CTS_CHG_INT_ST_V  0x00000001U
#define UART_CTS_CHG_INT_ST_S  6
/** UART_BRK_DET_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status of UART_BRK_DET_INT.
 */
#define UART_BRK_DET_INT_ST    (BIT(7))
#define UART_BRK_DET_INT_ST_M  (UART_BRK_DET_INT_ST_V << UART_BRK_DET_INT_ST_S)
#define UART_BRK_DET_INT_ST_V  0x00000001U
#define UART_BRK_DET_INT_ST_S  7
/** UART_RXFIFO_TOUT_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status of UART_RXFIFO_TOUT_INT.
 */
#define UART_RXFIFO_TOUT_INT_ST    (BIT(8))
#define UART_RXFIFO_TOUT_INT_ST_M  (UART_RXFIFO_TOUT_INT_ST_V << UART_RXFIFO_TOUT_INT_ST_S)
#define UART_RXFIFO_TOUT_INT_ST_V  0x00000001U
#define UART_RXFIFO_TOUT_INT_ST_S  8
/** UART_SW_XON_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status of UART_SW_XON_INT.
 */
#define UART_SW_XON_INT_ST    (BIT(9))
#define UART_SW_XON_INT_ST_M  (UART_SW_XON_INT_ST_V << UART_SW_XON_INT_ST_S)
#define UART_SW_XON_INT_ST_V  0x00000001U
#define UART_SW_XON_INT_ST_S  9
/** UART_SW_XOFF_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status of UART_SW_XOFF_INT.
 */
#define UART_SW_XOFF_INT_ST    (BIT(10))
#define UART_SW_XOFF_INT_ST_M  (UART_SW_XOFF_INT_ST_V << UART_SW_XOFF_INT_ST_S)
#define UART_SW_XOFF_INT_ST_V  0x00000001U
#define UART_SW_XOFF_INT_ST_S  10
/** UART_GLITCH_DET_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status of UART_GLITCH_DET_INT.
 */
#define UART_GLITCH_DET_INT_ST    (BIT(11))
#define UART_GLITCH_DET_INT_ST_M  (UART_GLITCH_DET_INT_ST_V << UART_GLITCH_DET_INT_ST_S)
#define UART_GLITCH_DET_INT_ST_V  0x00000001U
#define UART_GLITCH_DET_INT_ST_S  11
/** UART_TX_BRK_DONE_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status of UART_TX_BRK_DONE_INT.
 */
#define UART_TX_BRK_DONE_INT_ST    (BIT(12))
#define UART_TX_BRK_DONE_INT_ST_M  (UART_TX_BRK_DONE_INT_ST_V << UART_TX_BRK_DONE_INT_ST_S)
#define UART_TX_BRK_DONE_INT_ST_V  0x00000001U
#define UART_TX_BRK_DONE_INT_ST_S  12
/** UART_TX_BRK_IDLE_DONE_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status of UART_TX_BRK_IDLE_DONE_INT.
 */
#define UART_TX_BRK_IDLE_DONE_INT_ST    (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_ST_M  (UART_TX_BRK_IDLE_DONE_INT_ST_V << UART_TX_BRK_IDLE_DONE_INT_ST_S)
#define UART_TX_BRK_IDLE_DONE_INT_ST_V  0x00000001U
#define UART_TX_BRK_IDLE_DONE_INT_ST_S  13
/** UART_TX_DONE_INT_ST : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status of UART_TX_DONE_INT.
 */
#define UART_TX_DONE_INT_ST    (BIT(14))
#define UART_TX_DONE_INT_ST_M  (UART_TX_DONE_INT_ST_V << UART_TX_DONE_INT_ST_S)
#define UART_TX_DONE_INT_ST_V  0x00000001U
#define UART_TX_DONE_INT_ST_S  14
/** UART_RS485_PARITY_ERR_INT_ST : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status of UART_RS485_PARITY_ERR_INT.
 */
#define UART_RS485_PARITY_ERR_INT_ST    (BIT(15))
#define UART_RS485_PARITY_ERR_INT_ST_M  (UART_RS485_PARITY_ERR_INT_ST_V << UART_RS485_PARITY_ERR_INT_ST_S)
#define UART_RS485_PARITY_ERR_INT_ST_V  0x00000001U
#define UART_RS485_PARITY_ERR_INT_ST_S  15
/** UART_RS485_FRM_ERR_INT_ST : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status of UART_RS485_FRM_ERR_INT.
 */
#define UART_RS485_FRM_ERR_INT_ST    (BIT(16))
#define UART_RS485_FRM_ERR_INT_ST_M  (UART_RS485_FRM_ERR_INT_ST_V << UART_RS485_FRM_ERR_INT_ST_S)
#define UART_RS485_FRM_ERR_INT_ST_V  0x00000001U
#define UART_RS485_FRM_ERR_INT_ST_S  16
/** UART_RS485_CLASH_INT_ST : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status of UART_RS485_CLASH_INT.
 */
#define UART_RS485_CLASH_INT_ST    (BIT(17))
#define UART_RS485_CLASH_INT_ST_M  (UART_RS485_CLASH_INT_ST_V << UART_RS485_CLASH_INT_ST_S)
#define UART_RS485_CLASH_INT_ST_V  0x00000001U
#define UART_RS485_CLASH_INT_ST_S  17
/** UART_AT_CMD_CHAR_DET_INT_ST : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status of UART_AT_CMD_CHAR_DET_INT.
 */
#define UART_AT_CMD_CHAR_DET_INT_ST    (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_ST_M  (UART_AT_CMD_CHAR_DET_INT_ST_V << UART_AT_CMD_CHAR_DET_INT_ST_S)
#define UART_AT_CMD_CHAR_DET_INT_ST_V  0x00000001U
#define UART_AT_CMD_CHAR_DET_INT_ST_S  18
/** UART_WAKEUP_INT_ST : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status of UART_WAKEUP_INT.
 */
#define UART_WAKEUP_INT_ST    (BIT(19))
#define UART_WAKEUP_INT_ST_M  (UART_WAKEUP_INT_ST_V << UART_WAKEUP_INT_ST_S)
#define UART_WAKEUP_INT_ST_V  0x00000001U
#define UART_WAKEUP_INT_ST_S  19

/** UART_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define UART_INT_ENA_REG(i) (REG_UART_BASE(i) + 0xc)
/** UART_RXFIFO_FULL_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable UART_RXFIFO_FULL_INT.
 */
#define UART_RXFIFO_FULL_INT_ENA    (BIT(0))
#define UART_RXFIFO_FULL_INT_ENA_M  (UART_RXFIFO_FULL_INT_ENA_V << UART_RXFIFO_FULL_INT_ENA_S)
#define UART_RXFIFO_FULL_INT_ENA_V  0x00000001U
#define UART_RXFIFO_FULL_INT_ENA_S  0
/** UART_TXFIFO_EMPTY_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable UART_TXFIFO_EMPTY_INT.
 */
#define UART_TXFIFO_EMPTY_INT_ENA    (BIT(1))
#define UART_TXFIFO_EMPTY_INT_ENA_M  (UART_TXFIFO_EMPTY_INT_ENA_V << UART_TXFIFO_EMPTY_INT_ENA_S)
#define UART_TXFIFO_EMPTY_INT_ENA_V  0x00000001U
#define UART_TXFIFO_EMPTY_INT_ENA_S  1
/** UART_PARITY_ERR_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable UART_PARITY_ERR_INT.
 */
#define UART_PARITY_ERR_INT_ENA    (BIT(2))
#define UART_PARITY_ERR_INT_ENA_M  (UART_PARITY_ERR_INT_ENA_V << UART_PARITY_ERR_INT_ENA_S)
#define UART_PARITY_ERR_INT_ENA_V  0x00000001U
#define UART_PARITY_ERR_INT_ENA_S  2
/** UART_FRM_ERR_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Write 1 to enable UART_FRM_ERR_INT.
 */
#define UART_FRM_ERR_INT_ENA    (BIT(3))
#define UART_FRM_ERR_INT_ENA_M  (UART_FRM_ERR_INT_ENA_V << UART_FRM_ERR_INT_ENA_S)
#define UART_FRM_ERR_INT_ENA_V  0x00000001U
#define UART_FRM_ERR_INT_ENA_S  3
/** UART_RXFIFO_OVF_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable UART_RXFIFO_OVF_INT.
 */
#define UART_RXFIFO_OVF_INT_ENA    (BIT(4))
#define UART_RXFIFO_OVF_INT_ENA_M  (UART_RXFIFO_OVF_INT_ENA_V << UART_RXFIFO_OVF_INT_ENA_S)
#define UART_RXFIFO_OVF_INT_ENA_V  0x00000001U
#define UART_RXFIFO_OVF_INT_ENA_S  4
/** UART_DSR_CHG_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Write 1 to enable UART_DSR_CHG_INT.
 */
#define UART_DSR_CHG_INT_ENA    (BIT(5))
#define UART_DSR_CHG_INT_ENA_M  (UART_DSR_CHG_INT_ENA_V << UART_DSR_CHG_INT_ENA_S)
#define UART_DSR_CHG_INT_ENA_V  0x00000001U
#define UART_DSR_CHG_INT_ENA_S  5
/** UART_CTS_CHG_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Write 1 to enable UART_CTS_CHG_INT.
 */
#define UART_CTS_CHG_INT_ENA    (BIT(6))
#define UART_CTS_CHG_INT_ENA_M  (UART_CTS_CHG_INT_ENA_V << UART_CTS_CHG_INT_ENA_S)
#define UART_CTS_CHG_INT_ENA_V  0x00000001U
#define UART_CTS_CHG_INT_ENA_S  6
/** UART_BRK_DET_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Write 1 to enable UART_BRK_DET_INT.
 */
#define UART_BRK_DET_INT_ENA    (BIT(7))
#define UART_BRK_DET_INT_ENA_M  (UART_BRK_DET_INT_ENA_V << UART_BRK_DET_INT_ENA_S)
#define UART_BRK_DET_INT_ENA_V  0x00000001U
#define UART_BRK_DET_INT_ENA_S  7
/** UART_RXFIFO_TOUT_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Write 1 to enable UART_RXFIFO_TOUT_INT.
 */
#define UART_RXFIFO_TOUT_INT_ENA    (BIT(8))
#define UART_RXFIFO_TOUT_INT_ENA_M  (UART_RXFIFO_TOUT_INT_ENA_V << UART_RXFIFO_TOUT_INT_ENA_S)
#define UART_RXFIFO_TOUT_INT_ENA_V  0x00000001U
#define UART_RXFIFO_TOUT_INT_ENA_S  8
/** UART_SW_XON_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Write 1 to enable UART_SW_XON_INT.
 */
#define UART_SW_XON_INT_ENA    (BIT(9))
#define UART_SW_XON_INT_ENA_M  (UART_SW_XON_INT_ENA_V << UART_SW_XON_INT_ENA_S)
#define UART_SW_XON_INT_ENA_V  0x00000001U
#define UART_SW_XON_INT_ENA_S  9
/** UART_SW_XOFF_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  Write 1 to enable UART_SW_XOFF_INT.
 */
#define UART_SW_XOFF_INT_ENA    (BIT(10))
#define UART_SW_XOFF_INT_ENA_M  (UART_SW_XOFF_INT_ENA_V << UART_SW_XOFF_INT_ENA_S)
#define UART_SW_XOFF_INT_ENA_V  0x00000001U
#define UART_SW_XOFF_INT_ENA_S  10
/** UART_GLITCH_DET_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Write 1 to enable UART_GLITCH_DET_INT.
 */
#define UART_GLITCH_DET_INT_ENA    (BIT(11))
#define UART_GLITCH_DET_INT_ENA_M  (UART_GLITCH_DET_INT_ENA_V << UART_GLITCH_DET_INT_ENA_S)
#define UART_GLITCH_DET_INT_ENA_V  0x00000001U
#define UART_GLITCH_DET_INT_ENA_S  11
/** UART_TX_BRK_DONE_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  Write 1 to enable UART_TX_BRK_DONE_INT.
 */
#define UART_TX_BRK_DONE_INT_ENA    (BIT(12))
#define UART_TX_BRK_DONE_INT_ENA_M  (UART_TX_BRK_DONE_INT_ENA_V << UART_TX_BRK_DONE_INT_ENA_S)
#define UART_TX_BRK_DONE_INT_ENA_V  0x00000001U
#define UART_TX_BRK_DONE_INT_ENA_S  12
/** UART_TX_BRK_IDLE_DONE_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  Write 1 to enable UART_TX_BRK_IDLE_DONE_INT.
 */
#define UART_TX_BRK_IDLE_DONE_INT_ENA    (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_ENA_M  (UART_TX_BRK_IDLE_DONE_INT_ENA_V << UART_TX_BRK_IDLE_DONE_INT_ENA_S)
#define UART_TX_BRK_IDLE_DONE_INT_ENA_V  0x00000001U
#define UART_TX_BRK_IDLE_DONE_INT_ENA_S  13
/** UART_TX_DONE_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  Write 1 to enable UART_TX_DONE_INT.
 */
#define UART_TX_DONE_INT_ENA    (BIT(14))
#define UART_TX_DONE_INT_ENA_M  (UART_TX_DONE_INT_ENA_V << UART_TX_DONE_INT_ENA_S)
#define UART_TX_DONE_INT_ENA_V  0x00000001U
#define UART_TX_DONE_INT_ENA_S  14
/** UART_RS485_PARITY_ERR_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  Write 1 to enable UART_RS485_PARITY_ERR_INT.
 */
#define UART_RS485_PARITY_ERR_INT_ENA    (BIT(15))
#define UART_RS485_PARITY_ERR_INT_ENA_M  (UART_RS485_PARITY_ERR_INT_ENA_V << UART_RS485_PARITY_ERR_INT_ENA_S)
#define UART_RS485_PARITY_ERR_INT_ENA_V  0x00000001U
#define UART_RS485_PARITY_ERR_INT_ENA_S  15
/** UART_RS485_FRM_ERR_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  Write 1 to enable UART_RS485_FRM_ERR_INT.
 */
#define UART_RS485_FRM_ERR_INT_ENA    (BIT(16))
#define UART_RS485_FRM_ERR_INT_ENA_M  (UART_RS485_FRM_ERR_INT_ENA_V << UART_RS485_FRM_ERR_INT_ENA_S)
#define UART_RS485_FRM_ERR_INT_ENA_V  0x00000001U
#define UART_RS485_FRM_ERR_INT_ENA_S  16
/** UART_RS485_CLASH_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  Write 1 to enable UART_RS485_CLASH_INT.
 */
#define UART_RS485_CLASH_INT_ENA    (BIT(17))
#define UART_RS485_CLASH_INT_ENA_M  (UART_RS485_CLASH_INT_ENA_V << UART_RS485_CLASH_INT_ENA_S)
#define UART_RS485_CLASH_INT_ENA_V  0x00000001U
#define UART_RS485_CLASH_INT_ENA_S  17
/** UART_AT_CMD_CHAR_DET_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  Write 1 to enable UART_AT_CMD_CHAR_DET_INT.
 */
#define UART_AT_CMD_CHAR_DET_INT_ENA    (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_ENA_M  (UART_AT_CMD_CHAR_DET_INT_ENA_V << UART_AT_CMD_CHAR_DET_INT_ENA_S)
#define UART_AT_CMD_CHAR_DET_INT_ENA_V  0x00000001U
#define UART_AT_CMD_CHAR_DET_INT_ENA_S  18
/** UART_WAKEUP_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  Write 1 to enable UART_WAKEUP_INT.
 */
#define UART_WAKEUP_INT_ENA    (BIT(19))
#define UART_WAKEUP_INT_ENA_M  (UART_WAKEUP_INT_ENA_V << UART_WAKEUP_INT_ENA_S)
#define UART_WAKEUP_INT_ENA_V  0x00000001U
#define UART_WAKEUP_INT_ENA_S  19

/** UART_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define UART_INT_CLR_REG(i) (REG_UART_BASE(i) + 0x10)
/** UART_RXFIFO_FULL_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear UART_RXFIFO_FULL_INT.
 */
#define UART_RXFIFO_FULL_INT_CLR    (BIT(0))
#define UART_RXFIFO_FULL_INT_CLR_M  (UART_RXFIFO_FULL_INT_CLR_V << UART_RXFIFO_FULL_INT_CLR_S)
#define UART_RXFIFO_FULL_INT_CLR_V  0x00000001U
#define UART_RXFIFO_FULL_INT_CLR_S  0
/** UART_TXFIFO_EMPTY_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear UART_TXFIFO_EMPTY_INT.
 */
#define UART_TXFIFO_EMPTY_INT_CLR    (BIT(1))
#define UART_TXFIFO_EMPTY_INT_CLR_M  (UART_TXFIFO_EMPTY_INT_CLR_V << UART_TXFIFO_EMPTY_INT_CLR_S)
#define UART_TXFIFO_EMPTY_INT_CLR_V  0x00000001U
#define UART_TXFIFO_EMPTY_INT_CLR_S  1
/** UART_PARITY_ERR_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear UART_PARITY_ERR_INT.
 */
#define UART_PARITY_ERR_INT_CLR    (BIT(2))
#define UART_PARITY_ERR_INT_CLR_M  (UART_PARITY_ERR_INT_CLR_V << UART_PARITY_ERR_INT_CLR_S)
#define UART_PARITY_ERR_INT_CLR_V  0x00000001U
#define UART_PARITY_ERR_INT_CLR_S  2
/** UART_FRM_ERR_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Write 1 to clear UART_FRM_ERR_INT.
 */
#define UART_FRM_ERR_INT_CLR    (BIT(3))
#define UART_FRM_ERR_INT_CLR_M  (UART_FRM_ERR_INT_CLR_V << UART_FRM_ERR_INT_CLR_S)
#define UART_FRM_ERR_INT_CLR_V  0x00000001U
#define UART_FRM_ERR_INT_CLR_S  3
/** UART_RXFIFO_OVF_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear UART_RXFIFO_OVF_INT.
 */
#define UART_RXFIFO_OVF_INT_CLR    (BIT(4))
#define UART_RXFIFO_OVF_INT_CLR_M  (UART_RXFIFO_OVF_INT_CLR_V << UART_RXFIFO_OVF_INT_CLR_S)
#define UART_RXFIFO_OVF_INT_CLR_V  0x00000001U
#define UART_RXFIFO_OVF_INT_CLR_S  4
/** UART_DSR_CHG_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Write 1 to clear UART_DSR_CHG_INT.
 */
#define UART_DSR_CHG_INT_CLR    (BIT(5))
#define UART_DSR_CHG_INT_CLR_M  (UART_DSR_CHG_INT_CLR_V << UART_DSR_CHG_INT_CLR_S)
#define UART_DSR_CHG_INT_CLR_V  0x00000001U
#define UART_DSR_CHG_INT_CLR_S  5
/** UART_CTS_CHG_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Write 1 to clear UART_CTS_CHG_INT.
 */
#define UART_CTS_CHG_INT_CLR    (BIT(6))
#define UART_CTS_CHG_INT_CLR_M  (UART_CTS_CHG_INT_CLR_V << UART_CTS_CHG_INT_CLR_S)
#define UART_CTS_CHG_INT_CLR_V  0x00000001U
#define UART_CTS_CHG_INT_CLR_S  6
/** UART_BRK_DET_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Write 1 to clear UART_BRK_DET_INT.
 */
#define UART_BRK_DET_INT_CLR    (BIT(7))
#define UART_BRK_DET_INT_CLR_M  (UART_BRK_DET_INT_CLR_V << UART_BRK_DET_INT_CLR_S)
#define UART_BRK_DET_INT_CLR_V  0x00000001U
#define UART_BRK_DET_INT_CLR_S  7
/** UART_RXFIFO_TOUT_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Write 1 to clear UART_RXFIFO_TOUT_INT.
 */
#define UART_RXFIFO_TOUT_INT_CLR    (BIT(8))
#define UART_RXFIFO_TOUT_INT_CLR_M  (UART_RXFIFO_TOUT_INT_CLR_V << UART_RXFIFO_TOUT_INT_CLR_S)
#define UART_RXFIFO_TOUT_INT_CLR_V  0x00000001U
#define UART_RXFIFO_TOUT_INT_CLR_S  8
/** UART_SW_XON_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Write 1 to clear UART_SW_XON_INT.
 */
#define UART_SW_XON_INT_CLR    (BIT(9))
#define UART_SW_XON_INT_CLR_M  (UART_SW_XON_INT_CLR_V << UART_SW_XON_INT_CLR_S)
#define UART_SW_XON_INT_CLR_V  0x00000001U
#define UART_SW_XON_INT_CLR_S  9
/** UART_SW_XOFF_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Write 1 to clear UART_SW_XOFF_INT.
 */
#define UART_SW_XOFF_INT_CLR    (BIT(10))
#define UART_SW_XOFF_INT_CLR_M  (UART_SW_XOFF_INT_CLR_V << UART_SW_XOFF_INT_CLR_S)
#define UART_SW_XOFF_INT_CLR_V  0x00000001U
#define UART_SW_XOFF_INT_CLR_S  10
/** UART_GLITCH_DET_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Write 1 to clear UART_GLITCH_DET_INT.
 */
#define UART_GLITCH_DET_INT_CLR    (BIT(11))
#define UART_GLITCH_DET_INT_CLR_M  (UART_GLITCH_DET_INT_CLR_V << UART_GLITCH_DET_INT_CLR_S)
#define UART_GLITCH_DET_INT_CLR_V  0x00000001U
#define UART_GLITCH_DET_INT_CLR_S  11
/** UART_TX_BRK_DONE_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Write 1 to clear UART_TX_BRK_DONE_INT.
 */
#define UART_TX_BRK_DONE_INT_CLR    (BIT(12))
#define UART_TX_BRK_DONE_INT_CLR_M  (UART_TX_BRK_DONE_INT_CLR_V << UART_TX_BRK_DONE_INT_CLR_S)
#define UART_TX_BRK_DONE_INT_CLR_V  0x00000001U
#define UART_TX_BRK_DONE_INT_CLR_S  12
/** UART_TX_BRK_IDLE_DONE_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Write 1 to clear UART_TX_BRK_IDLE_DONE_INT.
 */
#define UART_TX_BRK_IDLE_DONE_INT_CLR    (BIT(13))
#define UART_TX_BRK_IDLE_DONE_INT_CLR_M  (UART_TX_BRK_IDLE_DONE_INT_CLR_V << UART_TX_BRK_IDLE_DONE_INT_CLR_S)
#define UART_TX_BRK_IDLE_DONE_INT_CLR_V  0x00000001U
#define UART_TX_BRK_IDLE_DONE_INT_CLR_S  13
/** UART_TX_DONE_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Write 1 to clear UART_TX_DONE_INT.
 */
#define UART_TX_DONE_INT_CLR    (BIT(14))
#define UART_TX_DONE_INT_CLR_M  (UART_TX_DONE_INT_CLR_V << UART_TX_DONE_INT_CLR_S)
#define UART_TX_DONE_INT_CLR_V  0x00000001U
#define UART_TX_DONE_INT_CLR_S  14
/** UART_RS485_PARITY_ERR_INT_CLR : WT; bitpos: [15]; default: 0;
 *  Write 1 to clear UART_RS485_PARITY_ERR_INT.
 */
#define UART_RS485_PARITY_ERR_INT_CLR    (BIT(15))
#define UART_RS485_PARITY_ERR_INT_CLR_M  (UART_RS485_PARITY_ERR_INT_CLR_V << UART_RS485_PARITY_ERR_INT_CLR_S)
#define UART_RS485_PARITY_ERR_INT_CLR_V  0x00000001U
#define UART_RS485_PARITY_ERR_INT_CLR_S  15
/** UART_RS485_FRM_ERR_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Write 1 to clear UART_RS485_FRM_ERR_INT.
 */
#define UART_RS485_FRM_ERR_INT_CLR    (BIT(16))
#define UART_RS485_FRM_ERR_INT_CLR_M  (UART_RS485_FRM_ERR_INT_CLR_V << UART_RS485_FRM_ERR_INT_CLR_S)
#define UART_RS485_FRM_ERR_INT_CLR_V  0x00000001U
#define UART_RS485_FRM_ERR_INT_CLR_S  16
/** UART_RS485_CLASH_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Write 1 to clear UART_RS485_CLASH_INT.
 */
#define UART_RS485_CLASH_INT_CLR    (BIT(17))
#define UART_RS485_CLASH_INT_CLR_M  (UART_RS485_CLASH_INT_CLR_V << UART_RS485_CLASH_INT_CLR_S)
#define UART_RS485_CLASH_INT_CLR_V  0x00000001U
#define UART_RS485_CLASH_INT_CLR_S  17
/** UART_AT_CMD_CHAR_DET_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Write 1 to clear UART_AT_CMD_CHAR_DET_INT.
 */
#define UART_AT_CMD_CHAR_DET_INT_CLR    (BIT(18))
#define UART_AT_CMD_CHAR_DET_INT_CLR_M  (UART_AT_CMD_CHAR_DET_INT_CLR_V << UART_AT_CMD_CHAR_DET_INT_CLR_S)
#define UART_AT_CMD_CHAR_DET_INT_CLR_V  0x00000001U
#define UART_AT_CMD_CHAR_DET_INT_CLR_S  18
/** UART_WAKEUP_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Write 1 to clear UART_WAKEUP_INT.
 */
#define UART_WAKEUP_INT_CLR    (BIT(19))
#define UART_WAKEUP_INT_CLR_M  (UART_WAKEUP_INT_CLR_V << UART_WAKEUP_INT_CLR_S)
#define UART_WAKEUP_INT_CLR_V  0x00000001U
#define UART_WAKEUP_INT_CLR_S  19

/** UART_CLKDIV_SYNC_REG register
 *  Clock divider configuration
 */
#define UART_CLKDIV_SYNC_REG(i) (REG_UART_BASE(i) + 0x14)
/** UART_CLKDIV : R/W; bitpos: [11:0]; default: 694;
 *  Configures the integral part of the divisor for baud rate generation.
 */
#define UART_CLKDIV    0x00000FFFU
#define UART_CLKDIV_M  (UART_CLKDIV_V << UART_CLKDIV_S)
#define UART_CLKDIV_V  0x00000FFFU
#define UART_CLKDIV_S  0
/** UART_CLKDIV_FRAG : R/W; bitpos: [23:20]; default: 0;
 *  Configures the fractional part of the divisor for baud rate generation.
 */
#define UART_CLKDIV_FRAG    0x0000000FU
#define UART_CLKDIV_FRAG_M  (UART_CLKDIV_FRAG_V << UART_CLKDIV_FRAG_S)
#define UART_CLKDIV_FRAG_V  0x0000000FU
#define UART_CLKDIV_FRAG_S  20

/** UART_RX_FILT_REG register
 *  RX filter configuration
 */
#define UART_RX_FILT_REG(i) (REG_UART_BASE(i) + 0x18)
/** UART_GLITCH_FILT : R/W; bitpos: [7:0]; default: 8;
 *  Configures the width of a pulse to be filtered.\\Measurement unit: UART Core's
 *  clock cycle.\\Pulses whose width is lower than this value will be ignored.
 */
#define UART_GLITCH_FILT    0x000000FFU
#define UART_GLITCH_FILT_M  (UART_GLITCH_FILT_V << UART_GLITCH_FILT_S)
#define UART_GLITCH_FILT_V  0x000000FFU
#define UART_GLITCH_FILT_S  0
/** UART_GLITCH_FILT_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable RX signal filter.\\
 *  0: Disable\\
 *  1: Enable
 */
#define UART_GLITCH_FILT_EN    (BIT(8))
#define UART_GLITCH_FILT_EN_M  (UART_GLITCH_FILT_EN_V << UART_GLITCH_FILT_EN_S)
#define UART_GLITCH_FILT_EN_V  0x00000001U
#define UART_GLITCH_FILT_EN_S  8

/** UART_STATUS_REG register
 *  UART status register
 */
#define UART_STATUS_REG(i) (REG_UART_BASE(i) + 0x1c)
/** UART_RXFIFO_CNT : RO; bitpos: [7:0]; default: 0;
 *  Represents the number of valid data bytes in RX FIFO.
 */
#define UART_RXFIFO_CNT    0x000000FFU
#define UART_RXFIFO_CNT_M  (UART_RXFIFO_CNT_V << UART_RXFIFO_CNT_S)
#define UART_RXFIFO_CNT_V  0x000000FFU
#define UART_RXFIFO_CNT_S  0
/** UART_DSRN : RO; bitpos: [13]; default: 0;
 *  Represents the level of the internal UART DSR signal.
 */
#define UART_DSRN    (BIT(13))
#define UART_DSRN_M  (UART_DSRN_V << UART_DSRN_S)
#define UART_DSRN_V  0x00000001U
#define UART_DSRN_S  13
/** UART_CTSN : RO; bitpos: [14]; default: 1;
 *  Represents the level of the internal UART CTS signal.
 */
#define UART_CTSN    (BIT(14))
#define UART_CTSN_M  (UART_CTSN_V << UART_CTSN_S)
#define UART_CTSN_V  0x00000001U
#define UART_CTSN_S  14
/** UART_RXD : RO; bitpos: [15]; default: 1;
 *  Represents the  level of the internal UART RXD signal.
 */
#define UART_RXD    (BIT(15))
#define UART_RXD_M  (UART_RXD_V << UART_RXD_S)
#define UART_RXD_V  0x00000001U
#define UART_RXD_S  15
/** UART_TXFIFO_CNT : RO; bitpos: [23:16]; default: 0;
 *  Represents the number of valid data bytes in RX FIFO.
 */
#define UART_TXFIFO_CNT    0x000000FFU
#define UART_TXFIFO_CNT_M  (UART_TXFIFO_CNT_V << UART_TXFIFO_CNT_S)
#define UART_TXFIFO_CNT_V  0x000000FFU
#define UART_TXFIFO_CNT_S  16
/** UART_DTRN : RO; bitpos: [29]; default: 1;
 *  Represents the level of the internal UART DTR signal.
 */
#define UART_DTRN    (BIT(29))
#define UART_DTRN_M  (UART_DTRN_V << UART_DTRN_S)
#define UART_DTRN_V  0x00000001U
#define UART_DTRN_S  29
/** UART_RTSN : RO; bitpos: [30]; default: 1;
 *  Represents the level of the internal UART RTS signal.
 */
#define UART_RTSN    (BIT(30))
#define UART_RTSN_M  (UART_RTSN_V << UART_RTSN_S)
#define UART_RTSN_V  0x00000001U
#define UART_RTSN_S  30
/** UART_TXD : RO; bitpos: [31]; default: 1;
 *  Represents the  level of the internal UART TXD signal.
 */
#define UART_TXD    (BIT(31))
#define UART_TXD_M  (UART_TXD_V << UART_TXD_S)
#define UART_TXD_V  0x00000001U
#define UART_TXD_S  31

/** UART_CONF0_SYNC_REG register
 *  Configuration register 0
 */
#define UART_CONF0_SYNC_REG(i) (REG_UART_BASE(i) + 0x20)
/** UART_PARITY : R/W; bitpos: [0]; default: 0;
 *  Configures the parity check mode.\\
 *  0: Even parity\\
 *  1: Odd parity\\
 */
#define UART_PARITY    (BIT(0))
#define UART_PARITY_M  (UART_PARITY_V << UART_PARITY_S)
#define UART_PARITY_V  0x00000001U
#define UART_PARITY_S  0
/** UART_PARITY_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable UART parity check.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_PARITY_EN    (BIT(1))
#define UART_PARITY_EN_M  (UART_PARITY_EN_V << UART_PARITY_EN_S)
#define UART_PARITY_EN_V  0x00000001U
#define UART_PARITY_EN_S  1
/** UART_BIT_NUM : R/W; bitpos: [3:2]; default: 3;
 *  Configures the number of data bits.\\
 *  0: 5 bits\\
 *  1: 6 bits\\
 *  2: 7 bits\\
 *  3: 8 bits\\
 */
#define UART_BIT_NUM    0x00000003U
#define UART_BIT_NUM_M  (UART_BIT_NUM_V << UART_BIT_NUM_S)
#define UART_BIT_NUM_V  0x00000003U
#define UART_BIT_NUM_S  2
/** UART_STOP_BIT_NUM : R/W; bitpos: [5:4]; default: 1;
 *  Configures the number of stop bits.\\
 *  0: Invalid. No effect\\
 *  1: 1 bits\\
 *  2: 1.5 bits\\
 *  3: 2 bits\\
 */
#define UART_STOP_BIT_NUM    0x00000003U
#define UART_STOP_BIT_NUM_M  (UART_STOP_BIT_NUM_V << UART_STOP_BIT_NUM_S)
#define UART_STOP_BIT_NUM_V  0x00000003U
#define UART_STOP_BIT_NUM_S  4
/** UART_TXD_BRK : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to send NULL characters when finishing data
 *  transmission.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define UART_TXD_BRK    (BIT(6))
#define UART_TXD_BRK_M  (UART_TXD_BRK_V << UART_TXD_BRK_S)
#define UART_TXD_BRK_V  0x00000001U
#define UART_TXD_BRK_S  6
/** UART_IRDA_DPLX : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable IrDA loopback test.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_IRDA_DPLX    (BIT(7))
#define UART_IRDA_DPLX_M  (UART_IRDA_DPLX_V << UART_IRDA_DPLX_S)
#define UART_IRDA_DPLX_V  0x00000001U
#define UART_IRDA_DPLX_S  7
/** UART_IRDA_TX_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable the IrDA transmitter.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_IRDA_TX_EN    (BIT(8))
#define UART_IRDA_TX_EN_M  (UART_IRDA_TX_EN_V << UART_IRDA_TX_EN_S)
#define UART_IRDA_TX_EN_V  0x00000001U
#define UART_IRDA_TX_EN_S  8
/** UART_IRDA_WCTL : R/W; bitpos: [9]; default: 0;
 *  Configures the 11th bit of the IrDA transmitter.\\
 *  0: This bit is 0.\\
 *  1: This bit is the same as the 10th bit.\\
 */
#define UART_IRDA_WCTL    (BIT(9))
#define UART_IRDA_WCTL_M  (UART_IRDA_WCTL_V << UART_IRDA_WCTL_S)
#define UART_IRDA_WCTL_V  0x00000001U
#define UART_IRDA_WCTL_S  9
/** UART_IRDA_TX_INV : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to invert the level of the IrDA transmitter.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_IRDA_TX_INV    (BIT(10))
#define UART_IRDA_TX_INV_M  (UART_IRDA_TX_INV_V << UART_IRDA_TX_INV_S)
#define UART_IRDA_TX_INV_V  0x00000001U
#define UART_IRDA_TX_INV_S  10
/** UART_IRDA_RX_INV : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert the level of the IrDA receiver.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_IRDA_RX_INV    (BIT(11))
#define UART_IRDA_RX_INV_M  (UART_IRDA_RX_INV_V << UART_IRDA_RX_INV_S)
#define UART_IRDA_RX_INV_V  0x00000001U
#define UART_IRDA_RX_INV_S  11
/** UART_LOOPBACK : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable UART loopback test.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_LOOPBACK    (BIT(12))
#define UART_LOOPBACK_M  (UART_LOOPBACK_V << UART_LOOPBACK_S)
#define UART_LOOPBACK_V  0x00000001U
#define UART_LOOPBACK_S  12
/** UART_TX_FLOW_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable flow control for the transmitter.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_TX_FLOW_EN    (BIT(13))
#define UART_TX_FLOW_EN_M  (UART_TX_FLOW_EN_V << UART_TX_FLOW_EN_S)
#define UART_TX_FLOW_EN_V  0x00000001U
#define UART_TX_FLOW_EN_S  13
/** UART_IRDA_EN : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to enable IrDA protocol.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_IRDA_EN    (BIT(14))
#define UART_IRDA_EN_M  (UART_IRDA_EN_V << UART_IRDA_EN_S)
#define UART_IRDA_EN_V  0x00000001U
#define UART_IRDA_EN_S  14
/** UART_RXD_INV : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to invert the level of UART RXD signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_RXD_INV    (BIT(15))
#define UART_RXD_INV_M  (UART_RXD_INV_V << UART_RXD_INV_S)
#define UART_RXD_INV_V  0x00000001U
#define UART_RXD_INV_S  15
/** UART_TXD_INV : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to invert the level of UART TXD signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_TXD_INV    (BIT(16))
#define UART_TXD_INV_M  (UART_TXD_INV_V << UART_TXD_INV_S)
#define UART_TXD_INV_V  0x00000001U
#define UART_TXD_INV_S  16
/** UART_DIS_RX_DAT_OVF : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to disable data overflow detection for the UART
 *  receiver.\\
 *  0: Enable\\
 *  1: Disable\\
 */
#define UART_DIS_RX_DAT_OVF    (BIT(17))
#define UART_DIS_RX_DAT_OVF_M  (UART_DIS_RX_DAT_OVF_V << UART_DIS_RX_DAT_OVF_S)
#define UART_DIS_RX_DAT_OVF_V  0x00000001U
#define UART_DIS_RX_DAT_OVF_S  17
/** UART_ERR_WR_MASK : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to store the received data with errors into FIFO.\\
 *  0: Store\\
 *  1: Not store\\
 */
#define UART_ERR_WR_MASK    (BIT(18))
#define UART_ERR_WR_MASK_M  (UART_ERR_WR_MASK_V << UART_ERR_WR_MASK_S)
#define UART_ERR_WR_MASK_V  0x00000001U
#define UART_ERR_WR_MASK_S  18
/** UART_AUTOBAUD_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable baud rate detection.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_AUTOBAUD_EN    (BIT(19))
#define UART_AUTOBAUD_EN_M  (UART_AUTOBAUD_EN_V << UART_AUTOBAUD_EN_S)
#define UART_AUTOBAUD_EN_V  0x00000001U
#define UART_AUTOBAUD_EN_S  19
/** UART_MEM_CLK_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable clock gating for UART memory.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_MEM_CLK_EN    (BIT(20))
#define UART_MEM_CLK_EN_M  (UART_MEM_CLK_EN_V << UART_MEM_CLK_EN_S)
#define UART_MEM_CLK_EN_V  0x00000001U
#define UART_MEM_CLK_EN_S  20
/** UART_SW_RTS : R/W; bitpos: [21]; default: 0;
 *  Configures the RTS signal used in software flow control.\\
 *  0: The UART transmitter is allowed to send data.\\
 *  1: The UART transmitted is not allowed to send data.\\
 */
#define UART_SW_RTS    (BIT(21))
#define UART_SW_RTS_M  (UART_SW_RTS_V << UART_SW_RTS_S)
#define UART_SW_RTS_V  0x00000001U
#define UART_SW_RTS_S  21
/** UART_RXFIFO_RST : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to reset the UART RX FIFO.\\
 *  0: Not reset\\
 *  1: Reset\\
 */
#define UART_RXFIFO_RST    (BIT(22))
#define UART_RXFIFO_RST_M  (UART_RXFIFO_RST_V << UART_RXFIFO_RST_S)
#define UART_RXFIFO_RST_V  0x00000001U
#define UART_RXFIFO_RST_S  22
/** UART_TXFIFO_RST : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to reset the UART TX FIFO.\\
 *  0: Not reset\\
 *  1: Reset\\
 */
#define UART_TXFIFO_RST    (BIT(23))
#define UART_TXFIFO_RST_M  (UART_TXFIFO_RST_V << UART_TXFIFO_RST_S)
#define UART_TXFIFO_RST_V  0x00000001U
#define UART_TXFIFO_RST_S  23

/** UART_CONF1_REG register
 *  Configuration register 1
 */
#define UART_CONF1_REG(i) (REG_UART_BASE(i) + 0x24)
/** UART_RXFIFO_FULL_THRHD : R/W; bitpos: [7:0]; default: 96;
 *  Configures the threshold for RX FIFO being full.\\Measurement unit: byte.
 */
#define UART_RXFIFO_FULL_THRHD    0x000000FFU
#define UART_RXFIFO_FULL_THRHD_M  (UART_RXFIFO_FULL_THRHD_V << UART_RXFIFO_FULL_THRHD_S)
#define UART_RXFIFO_FULL_THRHD_V  0x000000FFU
#define UART_RXFIFO_FULL_THRHD_S  0
/** UART_TXFIFO_EMPTY_THRHD : R/W; bitpos: [15:8]; default: 96;
 *  Configures the threshold for TX FIFO being empty.\\Measurement unit: byte.
 */
#define UART_TXFIFO_EMPTY_THRHD    0x000000FFU
#define UART_TXFIFO_EMPTY_THRHD_M  (UART_TXFIFO_EMPTY_THRHD_V << UART_TXFIFO_EMPTY_THRHD_S)
#define UART_TXFIFO_EMPTY_THRHD_V  0x000000FFU
#define UART_TXFIFO_EMPTY_THRHD_S  8
/** UART_CTS_INV : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to invert the level of UART CTS signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_CTS_INV    (BIT(16))
#define UART_CTS_INV_M  (UART_CTS_INV_V << UART_CTS_INV_S)
#define UART_CTS_INV_V  0x00000001U
#define UART_CTS_INV_S  16
/** UART_DSR_INV : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to invert the level of UART DSR signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_DSR_INV    (BIT(17))
#define UART_DSR_INV_M  (UART_DSR_INV_V << UART_DSR_INV_S)
#define UART_DSR_INV_V  0x00000001U
#define UART_DSR_INV_S  17
/** UART_RTS_INV : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to invert the level of UART RTS signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_RTS_INV    (BIT(18))
#define UART_RTS_INV_M  (UART_RTS_INV_V << UART_RTS_INV_S)
#define UART_RTS_INV_V  0x00000001U
#define UART_RTS_INV_S  18
/** UART_DTR_INV : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to invert the level of UART DTR signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define UART_DTR_INV    (BIT(19))
#define UART_DTR_INV_M  (UART_DTR_INV_V << UART_DTR_INV_S)
#define UART_DTR_INV_V  0x00000001U
#define UART_DTR_INV_S  19
/** UART_SW_DTR : R/W; bitpos: [20]; default: 0;
 *  Configures the DTR signal used in software flow control.\\
 *  0: Data to be transmitted is not ready.\\
 *  1: Data to be transmitted is ready.\\
 */
#define UART_SW_DTR    (BIT(20))
#define UART_SW_DTR_M  (UART_SW_DTR_V << UART_SW_DTR_S)
#define UART_SW_DTR_V  0x00000001U
#define UART_SW_DTR_S  20
/** UART_CLK_EN : R/W; bitpos: [21]; default: 0;
 *  Configures clock gating.\\
 *  0: Support clock only when the application writes registers.\\
 *  1: Always force the clock on for registers.\\
 */
#define UART_CLK_EN    (BIT(21))
#define UART_CLK_EN_M  (UART_CLK_EN_V << UART_CLK_EN_S)
#define UART_CLK_EN_V  0x00000001U
#define UART_CLK_EN_S  21

/** UART_HWFC_CONF_SYNC_REG register
 *  Hardware flow control configuration
 */
#define UART_HWFC_CONF_SYNC_REG(i) (REG_UART_BASE(i) + 0x2c)
/** UART_RX_FLOW_THRHD : R/W; bitpos: [7:0]; default: 0;
 *  Configures the maximum number of data bytes that can be received  during hardware
 *  flow control.\\Measurement unit: byte.
 */
#define UART_RX_FLOW_THRHD    0x000000FFU
#define UART_RX_FLOW_THRHD_M  (UART_RX_FLOW_THRHD_V << UART_RX_FLOW_THRHD_S)
#define UART_RX_FLOW_THRHD_V  0x000000FFU
#define UART_RX_FLOW_THRHD_S  0
/** UART_RX_FLOW_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable the UART receiver.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_RX_FLOW_EN    (BIT(8))
#define UART_RX_FLOW_EN_M  (UART_RX_FLOW_EN_V << UART_RX_FLOW_EN_S)
#define UART_RX_FLOW_EN_V  0x00000001U
#define UART_RX_FLOW_EN_S  8

/** UART_SLEEP_CONF0_REG register
 *  UART sleep configuration register 0
 */
#define UART_SLEEP_CONF0_REG(i) (REG_UART_BASE(i) + 0x30)
/** UART_WK_CHAR1 : R/W; bitpos: [7:0]; default: 0;
 *  Configures wakeup character 1.
 */
#define UART_WK_CHAR1    0x000000FFU
#define UART_WK_CHAR1_M  (UART_WK_CHAR1_V << UART_WK_CHAR1_S)
#define UART_WK_CHAR1_V  0x000000FFU
#define UART_WK_CHAR1_S  0
/** UART_WK_CHAR2 : R/W; bitpos: [15:8]; default: 0;
 *  Configures wakeup character 2.
 */
#define UART_WK_CHAR2    0x000000FFU
#define UART_WK_CHAR2_M  (UART_WK_CHAR2_V << UART_WK_CHAR2_S)
#define UART_WK_CHAR2_V  0x000000FFU
#define UART_WK_CHAR2_S  8
/** UART_WK_CHAR3 : R/W; bitpos: [23:16]; default: 0;
 *  Configures wakeup character 3.
 */
#define UART_WK_CHAR3    0x000000FFU
#define UART_WK_CHAR3_M  (UART_WK_CHAR3_V << UART_WK_CHAR3_S)
#define UART_WK_CHAR3_V  0x000000FFU
#define UART_WK_CHAR3_S  16
/** UART_WK_CHAR4 : R/W; bitpos: [31:24]; default: 0;
 *  Configures wakeup character 4.
 */
#define UART_WK_CHAR4    0x000000FFU
#define UART_WK_CHAR4_M  (UART_WK_CHAR4_V << UART_WK_CHAR4_S)
#define UART_WK_CHAR4_V  0x000000FFU
#define UART_WK_CHAR4_S  24

/** UART_SLEEP_CONF1_REG register
 *  UART sleep configuration register 1
 */
#define UART_SLEEP_CONF1_REG(i) (REG_UART_BASE(i) + 0x34)
/** UART_WK_CHAR0 : R/W; bitpos: [7:0]; default: 0;
 *  Configures wakeup character 0.
 */
#define UART_WK_CHAR0    0x000000FFU
#define UART_WK_CHAR0_M  (UART_WK_CHAR0_V << UART_WK_CHAR0_S)
#define UART_WK_CHAR0_V  0x000000FFU
#define UART_WK_CHAR0_S  0

/** UART_SLEEP_CONF2_REG register
 *  UART sleep configuration register 2
 */
#define UART_SLEEP_CONF2_REG(i) (REG_UART_BASE(i) + 0x38)
/** UART_ACTIVE_THRESHOLD : R/W; bitpos: [9:0]; default: 240;
 *  Configures the number of RXD edge changes to wake up the chip in wakeup mode 0.
 */
#define UART_ACTIVE_THRESHOLD    0x000003FFU
#define UART_ACTIVE_THRESHOLD_M  (UART_ACTIVE_THRESHOLD_V << UART_ACTIVE_THRESHOLD_S)
#define UART_ACTIVE_THRESHOLD_V  0x000003FFU
#define UART_ACTIVE_THRESHOLD_S  0
/** UART_RX_WAKE_UP_THRHD : R/W; bitpos: [17:10]; default: 1;
 *  Configures the number of received data bytes to wake up the chip in wakeup mode 1.
 */
#define UART_RX_WAKE_UP_THRHD    0x000000FFU
#define UART_RX_WAKE_UP_THRHD_M  (UART_RX_WAKE_UP_THRHD_V << UART_RX_WAKE_UP_THRHD_S)
#define UART_RX_WAKE_UP_THRHD_V  0x000000FFU
#define UART_RX_WAKE_UP_THRHD_S  10
/** UART_WK_CHAR_NUM : R/W; bitpos: [20:18]; default: 5;
 *  Configures the number of wakeup characters.
 */
#define UART_WK_CHAR_NUM    0x00000007U
#define UART_WK_CHAR_NUM_M  (UART_WK_CHAR_NUM_V << UART_WK_CHAR_NUM_S)
#define UART_WK_CHAR_NUM_V  0x00000007U
#define UART_WK_CHAR_NUM_S  18
/** UART_WK_CHAR_MASK : R/W; bitpos: [25:21]; default: 0;
 *  Configures whether or not to mask wakeup characters.\\
 *  0: Not mask\\
 *  1: Mask\\
 */
#define UART_WK_CHAR_MASK    0x0000001FU
#define UART_WK_CHAR_MASK_M  (UART_WK_CHAR_MASK_V << UART_WK_CHAR_MASK_S)
#define UART_WK_CHAR_MASK_V  0x0000001FU
#define UART_WK_CHAR_MASK_S  21
/** UART_WK_MODE_SEL : R/W; bitpos: [27:26]; default: 0;
 *  Configures which wakeup mode to select.\\
 *  0: Mode 0\\
 *  1: Mode 1\\
 *  2: Mode 2\\
 *  3: Mode 3\\
 */
#define UART_WK_MODE_SEL    0x00000003U
#define UART_WK_MODE_SEL_M  (UART_WK_MODE_SEL_V << UART_WK_MODE_SEL_S)
#define UART_WK_MODE_SEL_V  0x00000003U
#define UART_WK_MODE_SEL_S  26

/** UART_SWFC_CONF0_SYNC_REG register
 *  Software flow control character configuration
 */
#define UART_SWFC_CONF0_SYNC_REG(i) (REG_UART_BASE(i) + 0x3c)
/** UART_XON_CHAR : R/W; bitpos: [7:0]; default: 17;
 *  Configures the XON character for flow control.
 */
#define UART_XON_CHAR    0x000000FFU
#define UART_XON_CHAR_M  (UART_XON_CHAR_V << UART_XON_CHAR_S)
#define UART_XON_CHAR_V  0x000000FFU
#define UART_XON_CHAR_S  0
/** UART_XOFF_CHAR : R/W; bitpos: [15:8]; default: 19;
 *  Configures the XOFF character for flow control.
 */
#define UART_XOFF_CHAR    0x000000FFU
#define UART_XOFF_CHAR_M  (UART_XOFF_CHAR_V << UART_XOFF_CHAR_S)
#define UART_XOFF_CHAR_V  0x000000FFU
#define UART_XOFF_CHAR_S  8
/** UART_XON_XOFF_STILL_SEND : R/W; bitpos: [16]; default: 0;
 *  Configures whether the UART transmitter can send XON or XOFF characters when it is
 *  disabled.\\
 *  0: Cannot send\\
 *  1: Can send\\
 */
#define UART_XON_XOFF_STILL_SEND    (BIT(16))
#define UART_XON_XOFF_STILL_SEND_M  (UART_XON_XOFF_STILL_SEND_V << UART_XON_XOFF_STILL_SEND_S)
#define UART_XON_XOFF_STILL_SEND_V  0x00000001U
#define UART_XON_XOFF_STILL_SEND_S  16
/** UART_SW_FLOW_CON_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable software flow control.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_SW_FLOW_CON_EN    (BIT(17))
#define UART_SW_FLOW_CON_EN_M  (UART_SW_FLOW_CON_EN_V << UART_SW_FLOW_CON_EN_S)
#define UART_SW_FLOW_CON_EN_V  0x00000001U
#define UART_SW_FLOW_CON_EN_S  17
/** UART_XONOFF_DEL : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to remove flow control characters from the received
 *  data.\\
 *  0: Not move\\
 *  1: Move\\
 */
#define UART_XONOFF_DEL    (BIT(18))
#define UART_XONOFF_DEL_M  (UART_XONOFF_DEL_V << UART_XONOFF_DEL_S)
#define UART_XONOFF_DEL_V  0x00000001U
#define UART_XONOFF_DEL_S  18
/** UART_FORCE_XON : R/W; bitpos: [19]; default: 0;
 *  Configures whether the transmitter continues to sending data.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define UART_FORCE_XON    (BIT(19))
#define UART_FORCE_XON_M  (UART_FORCE_XON_V << UART_FORCE_XON_S)
#define UART_FORCE_XON_V  0x00000001U
#define UART_FORCE_XON_S  19
/** UART_FORCE_XOFF : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to stop the transmitter from sending data.\\
 *  0: Not stop\\
 *  1: Stop\\
 */
#define UART_FORCE_XOFF    (BIT(20))
#define UART_FORCE_XOFF_M  (UART_FORCE_XOFF_V << UART_FORCE_XOFF_S)
#define UART_FORCE_XOFF_V  0x00000001U
#define UART_FORCE_XOFF_S  20
/** UART_SEND_XON : R/W/SS/SC; bitpos: [21]; default: 0;
 *  Configures whether or not to send XON characters.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define UART_SEND_XON    (BIT(21))
#define UART_SEND_XON_M  (UART_SEND_XON_V << UART_SEND_XON_S)
#define UART_SEND_XON_V  0x00000001U
#define UART_SEND_XON_S  21
/** UART_SEND_XOFF : R/W/SS/SC; bitpos: [22]; default: 0;
 *  Configures whether or not to send XOFF characters.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define UART_SEND_XOFF    (BIT(22))
#define UART_SEND_XOFF_M  (UART_SEND_XOFF_V << UART_SEND_XOFF_S)
#define UART_SEND_XOFF_V  0x00000001U
#define UART_SEND_XOFF_S  22

/** UART_SWFC_CONF1_REG register
 *  Software flow control character configuration
 */
#define UART_SWFC_CONF1_REG(i) (REG_UART_BASE(i) + 0x40)
/** UART_XON_THRESHOLD : R/W; bitpos: [7:0]; default: 0;
 *  Configures the threshold for data in RX FIFO to send XON characters in software
 *  flow control.\\Measurement unit: byte.
 */
#define UART_XON_THRESHOLD    0x000000FFU
#define UART_XON_THRESHOLD_M  (UART_XON_THRESHOLD_V << UART_XON_THRESHOLD_S)
#define UART_XON_THRESHOLD_V  0x000000FFU
#define UART_XON_THRESHOLD_S  0
/** UART_XOFF_THRESHOLD : R/W; bitpos: [15:8]; default: 224;
 *  Configures the threshold for data in RX FIFO to send XOFF characters in software
 *  flow control.\\Measurement unit: byte.
 */
#define UART_XOFF_THRESHOLD    0x000000FFU
#define UART_XOFF_THRESHOLD_M  (UART_XOFF_THRESHOLD_V << UART_XOFF_THRESHOLD_S)
#define UART_XOFF_THRESHOLD_V  0x000000FFU
#define UART_XOFF_THRESHOLD_S  8

/** UART_TXBRK_CONF_SYNC_REG register
 *  TX break character configuration
 */
#define UART_TXBRK_CONF_SYNC_REG(i) (REG_UART_BASE(i) + 0x44)
/** UART_TX_BRK_NUM : R/W; bitpos: [7:0]; default: 10;
 *  Configures the number of NULL characters to be sent after finishing data
 *  transmission.\\Valid only when UART_TXD_BRK is 1.
 */
#define UART_TX_BRK_NUM    0x000000FFU
#define UART_TX_BRK_NUM_M  (UART_TX_BRK_NUM_V << UART_TX_BRK_NUM_S)
#define UART_TX_BRK_NUM_V  0x000000FFU
#define UART_TX_BRK_NUM_S  0

/** UART_IDLE_CONF_SYNC_REG register
 *  Frame end idle time configuration
 */
#define UART_IDLE_CONF_SYNC_REG(i) (REG_UART_BASE(i) + 0x48)
/** UART_RX_IDLE_THRHD : R/W; bitpos: [9:0]; default: 256;
 *  Configures the threshold to generate a frame end signal when the receiver takes
 *  more time to receive one data byte data.\\Measurement unit: bit time (the time to
 *  transmit 1 bit).
 */
#define UART_RX_IDLE_THRHD    0x000003FFU
#define UART_RX_IDLE_THRHD_M  (UART_RX_IDLE_THRHD_V << UART_RX_IDLE_THRHD_S)
#define UART_RX_IDLE_THRHD_V  0x000003FFU
#define UART_RX_IDLE_THRHD_S  0
/** UART_TX_IDLE_NUM : R/W; bitpos: [19:10]; default: 256;
 *  Configures the interval between two data transfers.\\Measurement unit: bit time
 *  (the time to transmit 1 bit).
 */
#define UART_TX_IDLE_NUM    0x000003FFU
#define UART_TX_IDLE_NUM_M  (UART_TX_IDLE_NUM_V << UART_TX_IDLE_NUM_S)
#define UART_TX_IDLE_NUM_V  0x000003FFU
#define UART_TX_IDLE_NUM_S  10

/** UART_RS485_CONF_SYNC_REG register
 *  RS485 mode configuration
 */
#define UART_RS485_CONF_SYNC_REG(i) (REG_UART_BASE(i) + 0x4c)
/** UART_RS485_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable RS485 mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_RS485_EN    (BIT(0))
#define UART_RS485_EN_M  (UART_RS485_EN_V << UART_RS485_EN_S)
#define UART_RS485_EN_V  0x00000001U
#define UART_RS485_EN_S  0
/** UART_DL0_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to add a turnaround delay of 1 bit before the start bit.\\
 *  0: Not add\\
 *  1: Add\\
 */
#define UART_DL0_EN    (BIT(1))
#define UART_DL0_EN_M  (UART_DL0_EN_V << UART_DL0_EN_S)
#define UART_DL0_EN_V  0x00000001U
#define UART_DL0_EN_S  1
/** UART_DL1_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to add a turnaround delay of 1 bit after the stop bit.\\
 *  0: Not add\\
 *  1: Add\\
 */
#define UART_DL1_EN    (BIT(2))
#define UART_DL1_EN_M  (UART_DL1_EN_V << UART_DL1_EN_S)
#define UART_DL1_EN_V  0x00000001U
#define UART_DL1_EN_S  2
/** UART_RS485TX_RX_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable the receiver for data reception when the
 *  transmitter is transmitting data in RS485 mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_RS485TX_RX_EN    (BIT(3))
#define UART_RS485TX_RX_EN_M  (UART_RS485TX_RX_EN_V << UART_RS485TX_RX_EN_S)
#define UART_RS485TX_RX_EN_V  0x00000001U
#define UART_RS485TX_RX_EN_S  3
/** UART_RS485RXBY_TX_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether to enable the RS485 transmitter for data transmission when the
 *  RS485 receiver is busy.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_RS485RXBY_TX_EN    (BIT(4))
#define UART_RS485RXBY_TX_EN_M  (UART_RS485RXBY_TX_EN_V << UART_RS485RXBY_TX_EN_S)
#define UART_RS485RXBY_TX_EN_V  0x00000001U
#define UART_RS485RXBY_TX_EN_S  4
/** UART_RS485_RX_DLY_NUM : R/W; bitpos: [5]; default: 0;
 *  Configures the delay of internal data signals in the receiver.\\Measurement unit:
 *  bit time (the time to transmit 1 bit)..
 */
#define UART_RS485_RX_DLY_NUM    (BIT(5))
#define UART_RS485_RX_DLY_NUM_M  (UART_RS485_RX_DLY_NUM_V << UART_RS485_RX_DLY_NUM_S)
#define UART_RS485_RX_DLY_NUM_V  0x00000001U
#define UART_RS485_RX_DLY_NUM_S  5
/** UART_RS485_TX_DLY_NUM : R/W; bitpos: [9:6]; default: 0;
 *  Configures the delay of internal data signals in the transmitter.\\Measurement
 *  unit: bit time (the time to transmit 1 bit).
 */
#define UART_RS485_TX_DLY_NUM    0x0000000FU
#define UART_RS485_TX_DLY_NUM_M  (UART_RS485_TX_DLY_NUM_V << UART_RS485_TX_DLY_NUM_S)
#define UART_RS485_TX_DLY_NUM_V  0x0000000FU
#define UART_RS485_TX_DLY_NUM_S  6

/** UART_AT_CMD_PRECNT_SYNC_REG register
 *  Pre-sequence timing configuration
 */
#define UART_AT_CMD_PRECNT_SYNC_REG(i) (REG_UART_BASE(i) + 0x50)
/** UART_PRE_IDLE_NUM : R/W; bitpos: [15:0]; default: 2305;
 *  Configures the idle time before the receiver receives the first
 *  AT_CMD.\\Measurement unit: bit time (the time to transmit 1 bit).
 */
#define UART_PRE_IDLE_NUM    0x0000FFFFU
#define UART_PRE_IDLE_NUM_M  (UART_PRE_IDLE_NUM_V << UART_PRE_IDLE_NUM_S)
#define UART_PRE_IDLE_NUM_V  0x0000FFFFU
#define UART_PRE_IDLE_NUM_S  0

/** UART_AT_CMD_POSTCNT_SYNC_REG register
 *  Post-sequence timing configuration
 */
#define UART_AT_CMD_POSTCNT_SYNC_REG(i) (REG_UART_BASE(i) + 0x54)
/** UART_POST_IDLE_NUM : R/W; bitpos: [15:0]; default: 2305;
 *  Configures the interval between the last AT_CMD and subsequent data.\\Measurement
 *  unit: bit time (the time to transmit 1 bit).
 */
#define UART_POST_IDLE_NUM    0x0000FFFFU
#define UART_POST_IDLE_NUM_M  (UART_POST_IDLE_NUM_V << UART_POST_IDLE_NUM_S)
#define UART_POST_IDLE_NUM_V  0x0000FFFFU
#define UART_POST_IDLE_NUM_S  0

/** UART_AT_CMD_GAPTOUT_SYNC_REG register
 *  Timeout configuration
 */
#define UART_AT_CMD_GAPTOUT_SYNC_REG(i) (REG_UART_BASE(i) + 0x58)
/** UART_RX_GAP_TOUT : R/W; bitpos: [15:0]; default: 11;
 *  Configures the interval between two AT_CMD characters.\\Measurement unit: bit time
 *  (the time to transmit 1 bit).
 */
#define UART_RX_GAP_TOUT    0x0000FFFFU
#define UART_RX_GAP_TOUT_M  (UART_RX_GAP_TOUT_V << UART_RX_GAP_TOUT_S)
#define UART_RX_GAP_TOUT_V  0x0000FFFFU
#define UART_RX_GAP_TOUT_S  0

/** UART_AT_CMD_CHAR_SYNC_REG register
 *  AT escape sequence detection configuration
 */
#define UART_AT_CMD_CHAR_SYNC_REG(i) (REG_UART_BASE(i) + 0x5c)
/** UART_AT_CMD_CHAR : R/W; bitpos: [7:0]; default: 43;
 *  Configures the AT_CMD character.
 */
#define UART_AT_CMD_CHAR    0x000000FFU
#define UART_AT_CMD_CHAR_M  (UART_AT_CMD_CHAR_V << UART_AT_CMD_CHAR_S)
#define UART_AT_CMD_CHAR_V  0x000000FFU
#define UART_AT_CMD_CHAR_S  0
/** UART_CHAR_NUM : R/W; bitpos: [15:8]; default: 3;
 *  Configures the number of continuous AT_CMD characters a receiver can receive.
 */
#define UART_CHAR_NUM    0x000000FFU
#define UART_CHAR_NUM_M  (UART_CHAR_NUM_V << UART_CHAR_NUM_S)
#define UART_CHAR_NUM_V  0x000000FFU
#define UART_CHAR_NUM_S  8

/** UART_MEM_CONF_REG register
 *  UART memory power configuration
 */
#define UART_MEM_CONF_REG(i) (REG_UART_BASE(i) + 0x60)
/** UART_MEM_FORCE_PD : R/W; bitpos: [25]; default: 0;
 *  Set this bit to force power down UART memory.
 */
#define UART_MEM_FORCE_PD    (BIT(25))
#define UART_MEM_FORCE_PD_M  (UART_MEM_FORCE_PD_V << UART_MEM_FORCE_PD_S)
#define UART_MEM_FORCE_PD_V  0x00000001U
#define UART_MEM_FORCE_PD_S  25
/** UART_MEM_FORCE_PU : R/W; bitpos: [26]; default: 0;
 *  Set this bit to force power up UART memory.
 */
#define UART_MEM_FORCE_PU    (BIT(26))
#define UART_MEM_FORCE_PU_M  (UART_MEM_FORCE_PU_V << UART_MEM_FORCE_PU_S)
#define UART_MEM_FORCE_PU_V  0x00000001U
#define UART_MEM_FORCE_PU_S  26

/** UART_TOUT_CONF_SYNC_REG register
 *  UART threshold and allocation configuration
 */
#define UART_TOUT_CONF_SYNC_REG(i) (REG_UART_BASE(i) + 0x64)
/** UART_RX_TOUT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable UART receiver's timeout function.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_RX_TOUT_EN    (BIT(0))
#define UART_RX_TOUT_EN_M  (UART_RX_TOUT_EN_V << UART_RX_TOUT_EN_S)
#define UART_RX_TOUT_EN_V  0x00000001U
#define UART_RX_TOUT_EN_S  0
/** UART_RX_TOUT_FLOW_DIS : R/W; bitpos: [1]; default: 0;
 *  Set this bit to stop accumulating idle_cnt when hardware flow control works.
 */
#define UART_RX_TOUT_FLOW_DIS    (BIT(1))
#define UART_RX_TOUT_FLOW_DIS_M  (UART_RX_TOUT_FLOW_DIS_V << UART_RX_TOUT_FLOW_DIS_S)
#define UART_RX_TOUT_FLOW_DIS_V  0x00000001U
#define UART_RX_TOUT_FLOW_DIS_S  1
/** UART_RX_TOUT_THRHD : R/W; bitpos: [11:2]; default: 10;
 *  Configures the amount of time that the bus can remain idle before timeout.\\
 *  Measurement unit: bit time (the time to transmit 1 bit).
 */
#define UART_RX_TOUT_THRHD    0x000003FFU
#define UART_RX_TOUT_THRHD_M  (UART_RX_TOUT_THRHD_V << UART_RX_TOUT_THRHD_S)
#define UART_RX_TOUT_THRHD_V  0x000003FFU
#define UART_RX_TOUT_THRHD_S  2

/** UART_MEM_TX_STATUS_REG register
 *  TX FIFO write and read offset address
 */
#define UART_MEM_TX_STATUS_REG(i) (REG_UART_BASE(i) + 0x68)
/** UART_TX_SRAM_WADDR : RO; bitpos: [7:0]; default: 0;
 *  Represents the offset address to write TX FIFO.
 */
#define UART_TX_SRAM_WADDR    0x000000FFU
#define UART_TX_SRAM_WADDR_M  (UART_TX_SRAM_WADDR_V << UART_TX_SRAM_WADDR_S)
#define UART_TX_SRAM_WADDR_V  0x000000FFU
#define UART_TX_SRAM_WADDR_S  0
/** UART_TX_SRAM_RADDR : RO; bitpos: [16:9]; default: 0;
 *  Represents the offset address to read TX FIFO.
 */
#define UART_TX_SRAM_RADDR    0x000000FFU
#define UART_TX_SRAM_RADDR_M  (UART_TX_SRAM_RADDR_V << UART_TX_SRAM_RADDR_S)
#define UART_TX_SRAM_RADDR_V  0x000000FFU
#define UART_TX_SRAM_RADDR_S  9

/** UART_MEM_RX_STATUS_REG register
 *  Rx FIFO write and read offset address
 */
#define UART_MEM_RX_STATUS_REG(i) (REG_UART_BASE(i) + 0x6c)
/** UART_RX_SRAM_RADDR : RO; bitpos: [7:0]; default: 128;
 *  Represents the offset address to read RX FIFO.
 */
#define UART_RX_SRAM_RADDR    0x000000FFU
#define UART_RX_SRAM_RADDR_M  (UART_RX_SRAM_RADDR_V << UART_RX_SRAM_RADDR_S)
#define UART_RX_SRAM_RADDR_V  0x000000FFU
#define UART_RX_SRAM_RADDR_S  0
/** UART_RX_SRAM_WADDR : RO; bitpos: [16:9]; default: 128;
 *  Represents the offset address to write RX FIFO.
 */
#define UART_RX_SRAM_WADDR    0x000000FFU
#define UART_RX_SRAM_WADDR_M  (UART_RX_SRAM_WADDR_V << UART_RX_SRAM_WADDR_S)
#define UART_RX_SRAM_WADDR_V  0x000000FFU
#define UART_RX_SRAM_WADDR_S  9

/** UART_FSM_STATUS_REG register
 *  UART transmit and receive status
 */
#define UART_FSM_STATUS_REG(i) (REG_UART_BASE(i) + 0x70)
/** UART_ST_URX_OUT : RO; bitpos: [3:0]; default: 0;
 *  Represents the status of the receiver.
 */
#define UART_ST_URX_OUT    0x0000000FU
#define UART_ST_URX_OUT_M  (UART_ST_URX_OUT_V << UART_ST_URX_OUT_S)
#define UART_ST_URX_OUT_V  0x0000000FU
#define UART_ST_URX_OUT_S  0
/** UART_ST_UTX_OUT : RO; bitpos: [7:4]; default: 0;
 *  Represents the status of the transmitter.
 */
#define UART_ST_UTX_OUT    0x0000000FU
#define UART_ST_UTX_OUT_M  (UART_ST_UTX_OUT_V << UART_ST_UTX_OUT_S)
#define UART_ST_UTX_OUT_V  0x0000000FU
#define UART_ST_UTX_OUT_S  4

/** UART_POSPULSE_REG register
 *  Autobaud high pulse register
 */
#define UART_POSPULSE_REG(i) (REG_UART_BASE(i) + 0x74)
/** UART_POSEDGE_MIN_CNT : RO; bitpos: [11:0]; default: 4095;
 *  Represents the minimal input clock counter value between two positive edges. It is
 *  used for baud rate detection.
 */
#define UART_POSEDGE_MIN_CNT    0x00000FFFU
#define UART_POSEDGE_MIN_CNT_M  (UART_POSEDGE_MIN_CNT_V << UART_POSEDGE_MIN_CNT_S)
#define UART_POSEDGE_MIN_CNT_V  0x00000FFFU
#define UART_POSEDGE_MIN_CNT_S  0

/** UART_NEGPULSE_REG register
 *  Autobaud low pulse register
 */
#define UART_NEGPULSE_REG(i) (REG_UART_BASE(i) + 0x78)
/** UART_NEGEDGE_MIN_CNT : RO; bitpos: [11:0]; default: 4095;
 *  Represents the minimal input clock counter value between two negative edges. It is
 *  used for baud rate detection.
 */
#define UART_NEGEDGE_MIN_CNT    0x00000FFFU
#define UART_NEGEDGE_MIN_CNT_M  (UART_NEGEDGE_MIN_CNT_V << UART_NEGEDGE_MIN_CNT_S)
#define UART_NEGEDGE_MIN_CNT_V  0x00000FFFU
#define UART_NEGEDGE_MIN_CNT_S  0

/** UART_LOWPULSE_REG register
 *  Autobaud minimum low pulse duration register
 */
#define UART_LOWPULSE_REG(i) (REG_UART_BASE(i) + 0x7c)
/** UART_LOWPULSE_MIN_CNT : RO; bitpos: [11:0]; default: 4095;
 *  Represents the minimum duration time of a low-level pulse. It is used for baud rate
 *  detection.\\Measurement unit: APB_CLK clock cycle.
 */
#define UART_LOWPULSE_MIN_CNT    0x00000FFFU
#define UART_LOWPULSE_MIN_CNT_M  (UART_LOWPULSE_MIN_CNT_V << UART_LOWPULSE_MIN_CNT_S)
#define UART_LOWPULSE_MIN_CNT_V  0x00000FFFU
#define UART_LOWPULSE_MIN_CNT_S  0

/** UART_HIGHPULSE_REG register
 *  Autobaud minimum high pulse duration register
 */
#define UART_HIGHPULSE_REG(i) (REG_UART_BASE(i) + 0x80)
/** UART_HIGHPULSE_MIN_CNT : RO; bitpos: [11:0]; default: 4095;
 *  Represents  the maximum duration time for a high-level pulse. It is used for baud
 *  rate detection.\\Measurement unit: APB_CLK clock cycle.
 */
#define UART_HIGHPULSE_MIN_CNT    0x00000FFFU
#define UART_HIGHPULSE_MIN_CNT_M  (UART_HIGHPULSE_MIN_CNT_V << UART_HIGHPULSE_MIN_CNT_S)
#define UART_HIGHPULSE_MIN_CNT_V  0x00000FFFU
#define UART_HIGHPULSE_MIN_CNT_S  0

/** UART_RXD_CNT_REG register
 *  Autobaud edge change count register
 */
#define UART_RXD_CNT_REG(i) (REG_UART_BASE(i) + 0x84)
/** UART_RXD_EDGE_CNT : RO; bitpos: [9:0]; default: 0;
 *  Represents the number of RXD edge changes. It is used for baud rate detection.
 */
#define UART_RXD_EDGE_CNT    0x000003FFU
#define UART_RXD_EDGE_CNT_M  (UART_RXD_EDGE_CNT_V << UART_RXD_EDGE_CNT_S)
#define UART_RXD_EDGE_CNT_V  0x000003FFU
#define UART_RXD_EDGE_CNT_S  0

/** UART_CLK_CONF_REG register
 *  UART core clock configuration
 */
#define UART_CLK_CONF_REG(i) (REG_UART_BASE(i) + 0x88)
/** UART_TX_SCLK_EN : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to enable UART TX clock.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_TX_SCLK_EN    (BIT(24))
#define UART_TX_SCLK_EN_M  (UART_TX_SCLK_EN_V << UART_TX_SCLK_EN_S)
#define UART_TX_SCLK_EN_V  0x00000001U
#define UART_TX_SCLK_EN_S  24
/** UART_RX_SCLK_EN : R/W; bitpos: [25]; default: 1;
 *  Configures whether or not to enable UART RX clock.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define UART_RX_SCLK_EN    (BIT(25))
#define UART_RX_SCLK_EN_M  (UART_RX_SCLK_EN_V << UART_RX_SCLK_EN_S)
#define UART_RX_SCLK_EN_V  0x00000001U
#define UART_RX_SCLK_EN_S  25
/** UART_TX_RST_CORE : R/W; bitpos: [26]; default: 0;
 *  Write 1 and then write 0 to reset UART TX.
 */
#define UART_TX_RST_CORE    (BIT(26))
#define UART_TX_RST_CORE_M  (UART_TX_RST_CORE_V << UART_TX_RST_CORE_S)
#define UART_TX_RST_CORE_V  0x00000001U
#define UART_TX_RST_CORE_S  26
/** UART_RX_RST_CORE : R/W; bitpos: [27]; default: 0;
 *  Write 1 and then write 0 to reset UART RX.
 */
#define UART_RX_RST_CORE    (BIT(27))
#define UART_RX_RST_CORE_M  (UART_RX_RST_CORE_V << UART_RX_RST_CORE_S)
#define UART_RX_RST_CORE_V  0x00000001U
#define UART_RX_RST_CORE_S  27

/** UART_DATE_REG register
 *  UART version control register
 */
#define UART_DATE_REG(i) (REG_UART_BASE(i) + 0x8c)
/** UART_DATE : R/W; bitpos: [31:0]; default: 36774432;
 *  Version control register.
 */
#define UART_DATE    0xFFFFFFFFU
#define UART_DATE_M  (UART_DATE_V << UART_DATE_S)
#define UART_DATE_V  0xFFFFFFFFU
#define UART_DATE_S  0

/** UART_AFIFO_STATUS_REG register
 *  UART asynchronous FIFO status
 */
#define UART_AFIFO_STATUS_REG(i) (REG_UART_BASE(i) + 0x90)
/** UART_TX_AFIFO_FULL : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the APB TX asynchronous FIFO is full.\\
 *  0: Not full\\
 *  1: Full\\
 */
#define UART_TX_AFIFO_FULL    (BIT(0))
#define UART_TX_AFIFO_FULL_M  (UART_TX_AFIFO_FULL_V << UART_TX_AFIFO_FULL_S)
#define UART_TX_AFIFO_FULL_V  0x00000001U
#define UART_TX_AFIFO_FULL_S  0
/** UART_TX_AFIFO_EMPTY : RO; bitpos: [1]; default: 1;
 *  Represents whether or not the APB TX asynchronous FIFO is empty.\\
 *  0: Not empty\\
 *  1: Empty\\
 */
#define UART_TX_AFIFO_EMPTY    (BIT(1))
#define UART_TX_AFIFO_EMPTY_M  (UART_TX_AFIFO_EMPTY_V << UART_TX_AFIFO_EMPTY_S)
#define UART_TX_AFIFO_EMPTY_V  0x00000001U
#define UART_TX_AFIFO_EMPTY_S  1
/** UART_RX_AFIFO_FULL : RO; bitpos: [2]; default: 0;
 *  Represents whether or not the APB RX asynchronous FIFO is full.\\
 *  0: Not full\\
 *  1: Full\\
 */
#define UART_RX_AFIFO_FULL    (BIT(2))
#define UART_RX_AFIFO_FULL_M  (UART_RX_AFIFO_FULL_V << UART_RX_AFIFO_FULL_S)
#define UART_RX_AFIFO_FULL_V  0x00000001U
#define UART_RX_AFIFO_FULL_S  2
/** UART_RX_AFIFO_EMPTY : RO; bitpos: [3]; default: 1;
 *  Represents whether or not the APB RX asynchronous FIFO is empty.\\
 *  0: Not empty\\
 *  1: Empty\\
 */
#define UART_RX_AFIFO_EMPTY    (BIT(3))
#define UART_RX_AFIFO_EMPTY_M  (UART_RX_AFIFO_EMPTY_V << UART_RX_AFIFO_EMPTY_S)
#define UART_RX_AFIFO_EMPTY_V  0x00000001U
#define UART_RX_AFIFO_EMPTY_S  3

/** UART_REG_UPDATE_REG register
 *  UART register configuration update
 */
#define UART_REG_UPDATE_REG(i) (REG_UART_BASE(i) + 0x98)
/** UART_REG_UPDATE : R/W/SC; bitpos: [0]; default: 0;
 *  Configures whether or not to synchronize registers.\\
 *  0: Not synchronize\\
 *  1: Synchronize\\
 */
#define UART_REG_UPDATE    (BIT(0))
#define UART_REG_UPDATE_M  (UART_REG_UPDATE_V << UART_REG_UPDATE_S)
#define UART_REG_UPDATE_V  0x00000001U
#define UART_REG_UPDATE_S  0

/** UART_ID_REG register
 *  UART ID register
 */
#define UART_ID_REG(i) (REG_UART_BASE(i) + 0x9c)
/** UART_ID : R/W; bitpos: [31:0]; default: 1280;
 *  Configures the UART ID.
 */
#define UART_ID    0xFFFFFFFFU
#define UART_ID_M  (UART_ID_V << UART_ID_S)
#define UART_ID_V  0xFFFFFFFFU
#define UART_ID_S  0

#ifdef __cplusplus
}
#endif
