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

/** LP_UART_FIFO_REG register
 *  FIFO data register
 */
#define LP_UART_FIFO_REG (DR_REG_LP_UART_BASE + 0x0)
/** LP_UART_RXFIFO_RD_BYTE : RO; bitpos: [7:0]; default: 0;
 *  Represents the data LP UART $n read from FIFO.\\
 *  Measurement unit: byte.
 */
#define LP_UART_RXFIFO_RD_BYTE    0x000000FFU
#define LP_UART_RXFIFO_RD_BYTE_M  (LP_UART_RXFIFO_RD_BYTE_V << LP_UART_RXFIFO_RD_BYTE_S)
#define LP_UART_RXFIFO_RD_BYTE_V  0x000000FFU
#define LP_UART_RXFIFO_RD_BYTE_S  0

/** LP_UART_INT_RAW_REG register
 *  Raw interrupt status
 */
#define LP_UART_INT_RAW_REG (DR_REG_LP_UART_BASE + 0x4)
/** LP_UART_RXFIFO_FULL_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status of LP_UART_RXFIFO_FULL_INT.
 */
#define LP_UART_RXFIFO_FULL_INT_RAW    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_RAW_M  (LP_UART_RXFIFO_FULL_INT_RAW_V << LP_UART_RXFIFO_FULL_INT_RAW_S)
#define LP_UART_RXFIFO_FULL_INT_RAW_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_RAW_S  0
/** LP_UART_TXFIFO_EMPTY_INT_RAW : R/WTC/SS; bitpos: [1]; default: 1;
 *  The raw interrupt status of LP_UART_TXFIFO_EMPTY_INT.
 */
#define LP_UART_TXFIFO_EMPTY_INT_RAW    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_RAW_M  (LP_UART_TXFIFO_EMPTY_INT_RAW_V << LP_UART_TXFIFO_EMPTY_INT_RAW_S)
#define LP_UART_TXFIFO_EMPTY_INT_RAW_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_RAW_S  1
/** LP_UART_PARITY_ERR_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status of LP_UART_PARITY_ERR_INT.
 */
#define LP_UART_PARITY_ERR_INT_RAW    (BIT(2))
#define LP_UART_PARITY_ERR_INT_RAW_M  (LP_UART_PARITY_ERR_INT_RAW_V << LP_UART_PARITY_ERR_INT_RAW_S)
#define LP_UART_PARITY_ERR_INT_RAW_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_RAW_S  2
/** LP_UART_FRM_ERR_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status of LP_UART_FRM_ERR_INT.
 */
#define LP_UART_FRM_ERR_INT_RAW    (BIT(3))
#define LP_UART_FRM_ERR_INT_RAW_M  (LP_UART_FRM_ERR_INT_RAW_V << LP_UART_FRM_ERR_INT_RAW_S)
#define LP_UART_FRM_ERR_INT_RAW_V  0x00000001U
#define LP_UART_FRM_ERR_INT_RAW_S  3
/** LP_UART_RXFIFO_OVF_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status of LP_UART_RXFIFO_OVF_INT.
 */
#define LP_UART_RXFIFO_OVF_INT_RAW    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_RAW_M  (LP_UART_RXFIFO_OVF_INT_RAW_V << LP_UART_RXFIFO_OVF_INT_RAW_S)
#define LP_UART_RXFIFO_OVF_INT_RAW_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_RAW_S  4
/** LP_UART_DSR_CHG_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt status of LP_UART_DSR_CHG_INT.
 */
#define LP_UART_DSR_CHG_INT_RAW    (BIT(5))
#define LP_UART_DSR_CHG_INT_RAW_M  (LP_UART_DSR_CHG_INT_RAW_V << LP_UART_DSR_CHG_INT_RAW_S)
#define LP_UART_DSR_CHG_INT_RAW_V  0x00000001U
#define LP_UART_DSR_CHG_INT_RAW_S  5
/** LP_UART_CTS_CHG_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt status of LP_UART_CTS_CHG_INT.
 */
#define LP_UART_CTS_CHG_INT_RAW    (BIT(6))
#define LP_UART_CTS_CHG_INT_RAW_M  (LP_UART_CTS_CHG_INT_RAW_V << LP_UART_CTS_CHG_INT_RAW_S)
#define LP_UART_CTS_CHG_INT_RAW_V  0x00000001U
#define LP_UART_CTS_CHG_INT_RAW_S  6
/** LP_UART_BRK_DET_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt status of LP_UART_BRK_DET_INT.
 */
#define LP_UART_BRK_DET_INT_RAW    (BIT(7))
#define LP_UART_BRK_DET_INT_RAW_M  (LP_UART_BRK_DET_INT_RAW_V << LP_UART_BRK_DET_INT_RAW_S)
#define LP_UART_BRK_DET_INT_RAW_V  0x00000001U
#define LP_UART_BRK_DET_INT_RAW_S  7
/** LP_UART_RXFIFO_TOUT_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt status of LP_UART_RXFIFO_TOUT_INT.
 */
#define LP_UART_RXFIFO_TOUT_INT_RAW    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_RAW_M  (LP_UART_RXFIFO_TOUT_INT_RAW_V << LP_UART_RXFIFO_TOUT_INT_RAW_S)
#define LP_UART_RXFIFO_TOUT_INT_RAW_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_RAW_S  8
/** LP_UART_SW_XON_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt status of LP_UART_SW_XON_INT.
 */
#define LP_UART_SW_XON_INT_RAW    (BIT(9))
#define LP_UART_SW_XON_INT_RAW_M  (LP_UART_SW_XON_INT_RAW_V << LP_UART_SW_XON_INT_RAW_S)
#define LP_UART_SW_XON_INT_RAW_V  0x00000001U
#define LP_UART_SW_XON_INT_RAW_S  9
/** LP_UART_SW_XOFF_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  LP_UART_SW_XOFF_INT.
 */
#define LP_UART_SW_XOFF_INT_RAW    (BIT(10))
#define LP_UART_SW_XOFF_INT_RAW_M  (LP_UART_SW_XOFF_INT_RAW_V << LP_UART_SW_XOFF_INT_RAW_S)
#define LP_UART_SW_XOFF_INT_RAW_V  0x00000001U
#define LP_UART_SW_XOFF_INT_RAW_S  10
/** LP_UART_GLITCH_DET_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt status of LP_UART_GLITCH_DET_INT.
 */
#define LP_UART_GLITCH_DET_INT_RAW    (BIT(11))
#define LP_UART_GLITCH_DET_INT_RAW_M  (LP_UART_GLITCH_DET_INT_RAW_V << LP_UART_GLITCH_DET_INT_RAW_S)
#define LP_UART_GLITCH_DET_INT_RAW_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_RAW_S  11
/** LP_UART_TX_BRK_DONE_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt status of LP_UART_TX_BRK_DONE_INT.
 */
#define LP_UART_TX_BRK_DONE_INT_RAW    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_RAW_M  (LP_UART_TX_BRK_DONE_INT_RAW_V << LP_UART_TX_BRK_DONE_INT_RAW_S)
#define LP_UART_TX_BRK_DONE_INT_RAW_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_RAW_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt status of LP_UART_TX_BRK_IDLE_DONE_INT.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW_M  (LP_UART_TX_BRK_IDLE_DONE_INT_RAW_V << LP_UART_TX_BRK_IDLE_DONE_INT_RAW_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW_S  13
/** LP_UART_TX_DONE_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw interrupt status of LP_UART_TX_DONE_INT.
 */
#define LP_UART_TX_DONE_INT_RAW    (BIT(14))
#define LP_UART_TX_DONE_INT_RAW_M  (LP_UART_TX_DONE_INT_RAW_V << LP_UART_TX_DONE_INT_RAW_S)
#define LP_UART_TX_DONE_INT_RAW_V  0x00000001U
#define LP_UART_TX_DONE_INT_RAW_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The raw interrupt status of LP_UART_AT_CMD_CHAR_DET_INT.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW_M  (LP_UART_AT_CMD_CHAR_DET_INT_RAW_V << LP_UART_AT_CMD_CHAR_DET_INT_RAW_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW_S  18
/** LP_UART_WAKEUP_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The raw interrupt status of LP_UART_WAKEUP_INT.
 */
#define LP_UART_WAKEUP_INT_RAW    (BIT(19))
#define LP_UART_WAKEUP_INT_RAW_M  (LP_UART_WAKEUP_INT_RAW_V << LP_UART_WAKEUP_INT_RAW_S)
#define LP_UART_WAKEUP_INT_RAW_V  0x00000001U
#define LP_UART_WAKEUP_INT_RAW_S  19

/** LP_UART_INT_ST_REG register
 *  Masked interrupt status
 */
#define LP_UART_INT_ST_REG (DR_REG_LP_UART_BASE + 0x8)
/** LP_UART_RXFIFO_FULL_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of LP_UART_RXFIFO_FULL_INT.
 */
#define LP_UART_RXFIFO_FULL_INT_ST    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_ST_M  (LP_UART_RXFIFO_FULL_INT_ST_V << LP_UART_RXFIFO_FULL_INT_ST_S)
#define LP_UART_RXFIFO_FULL_INT_ST_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_ST_S  0
/** LP_UART_TXFIFO_EMPTY_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of LP_UART_TXFIFO_EMPTY_INT.
 */
#define LP_UART_TXFIFO_EMPTY_INT_ST    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_ST_M  (LP_UART_TXFIFO_EMPTY_INT_ST_V << LP_UART_TXFIFO_EMPTY_INT_ST_S)
#define LP_UART_TXFIFO_EMPTY_INT_ST_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_ST_S  1
/** LP_UART_PARITY_ERR_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of LP_UART_PARITY_ERR_INT.
 */
#define LP_UART_PARITY_ERR_INT_ST    (BIT(2))
#define LP_UART_PARITY_ERR_INT_ST_M  (LP_UART_PARITY_ERR_INT_ST_V << LP_UART_PARITY_ERR_INT_ST_S)
#define LP_UART_PARITY_ERR_INT_ST_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_ST_S  2
/** LP_UART_FRM_ERR_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status of LP_UART_FRM_ERR_INT.
 */
#define LP_UART_FRM_ERR_INT_ST    (BIT(3))
#define LP_UART_FRM_ERR_INT_ST_M  (LP_UART_FRM_ERR_INT_ST_V << LP_UART_FRM_ERR_INT_ST_S)
#define LP_UART_FRM_ERR_INT_ST_V  0x00000001U
#define LP_UART_FRM_ERR_INT_ST_S  3
/** LP_UART_RXFIFO_OVF_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status of LP_UART_RXFIFO_OVF_INT.
 */
#define LP_UART_RXFIFO_OVF_INT_ST    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_ST_M  (LP_UART_RXFIFO_OVF_INT_ST_V << LP_UART_RXFIFO_OVF_INT_ST_S)
#define LP_UART_RXFIFO_OVF_INT_ST_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_ST_S  4
/** LP_UART_DSR_CHG_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status of LP_UART_DSR_CHG_INT.
 */
#define LP_UART_DSR_CHG_INT_ST    (BIT(5))
#define LP_UART_DSR_CHG_INT_ST_M  (LP_UART_DSR_CHG_INT_ST_V << LP_UART_DSR_CHG_INT_ST_S)
#define LP_UART_DSR_CHG_INT_ST_V  0x00000001U
#define LP_UART_DSR_CHG_INT_ST_S  5
/** LP_UART_CTS_CHG_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status of LP_UART_CTS_CHG_INT.
 */
#define LP_UART_CTS_CHG_INT_ST    (BIT(6))
#define LP_UART_CTS_CHG_INT_ST_M  (LP_UART_CTS_CHG_INT_ST_V << LP_UART_CTS_CHG_INT_ST_S)
#define LP_UART_CTS_CHG_INT_ST_V  0x00000001U
#define LP_UART_CTS_CHG_INT_ST_S  6
/** LP_UART_BRK_DET_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status of LP_UART_BRK_DET_INT.
 */
#define LP_UART_BRK_DET_INT_ST    (BIT(7))
#define LP_UART_BRK_DET_INT_ST_M  (LP_UART_BRK_DET_INT_ST_V << LP_UART_BRK_DET_INT_ST_S)
#define LP_UART_BRK_DET_INT_ST_V  0x00000001U
#define LP_UART_BRK_DET_INT_ST_S  7
/** LP_UART_RXFIFO_TOUT_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status of LP_UART_RXFIFO_TOUT_INT.
 */
#define LP_UART_RXFIFO_TOUT_INT_ST    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_ST_M  (LP_UART_RXFIFO_TOUT_INT_ST_V << LP_UART_RXFIFO_TOUT_INT_ST_S)
#define LP_UART_RXFIFO_TOUT_INT_ST_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_ST_S  8
/** LP_UART_SW_XON_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status of LP_UART_SW_XON_INT.
 */
#define LP_UART_SW_XON_INT_ST    (BIT(9))
#define LP_UART_SW_XON_INT_ST_M  (LP_UART_SW_XON_INT_ST_V << LP_UART_SW_XON_INT_ST_S)
#define LP_UART_SW_XON_INT_ST_V  0x00000001U
#define LP_UART_SW_XON_INT_ST_S  9
/** LP_UART_SW_XOFF_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status of LP_UART_SW_XOFF_INT.
 */
#define LP_UART_SW_XOFF_INT_ST    (BIT(10))
#define LP_UART_SW_XOFF_INT_ST_M  (LP_UART_SW_XOFF_INT_ST_V << LP_UART_SW_XOFF_INT_ST_S)
#define LP_UART_SW_XOFF_INT_ST_V  0x00000001U
#define LP_UART_SW_XOFF_INT_ST_S  10
/** LP_UART_GLITCH_DET_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status of LP_UART_GLITCH_DET_INT.
 */
#define LP_UART_GLITCH_DET_INT_ST    (BIT(11))
#define LP_UART_GLITCH_DET_INT_ST_M  (LP_UART_GLITCH_DET_INT_ST_V << LP_UART_GLITCH_DET_INT_ST_S)
#define LP_UART_GLITCH_DET_INT_ST_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_ST_S  11
/** LP_UART_TX_BRK_DONE_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status of LP_UART_TX_BRK_DONE_INT.
 */
#define LP_UART_TX_BRK_DONE_INT_ST    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_ST_M  (LP_UART_TX_BRK_DONE_INT_ST_V << LP_UART_TX_BRK_DONE_INT_ST_S)
#define LP_UART_TX_BRK_DONE_INT_ST_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_ST_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status of LP_UART_TX_BRK_IDLE_DONE_INT.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST_M  (LP_UART_TX_BRK_IDLE_DONE_INT_ST_V << LP_UART_TX_BRK_IDLE_DONE_INT_ST_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST_S  13
/** LP_UART_TX_DONE_INT_ST : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status of LP_UART_TX_DONE_INT.
 */
#define LP_UART_TX_DONE_INT_ST    (BIT(14))
#define LP_UART_TX_DONE_INT_ST_M  (LP_UART_TX_DONE_INT_ST_V << LP_UART_TX_DONE_INT_ST_S)
#define LP_UART_TX_DONE_INT_ST_V  0x00000001U
#define LP_UART_TX_DONE_INT_ST_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_ST : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status of LP_UART_AT_CMD_CHAR_DET_INT.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_ST    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_ST_M  (LP_UART_AT_CMD_CHAR_DET_INT_ST_V << LP_UART_AT_CMD_CHAR_DET_INT_ST_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_ST_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_ST_S  18
/** LP_UART_WAKEUP_INT_ST : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status of LP_UART_WAKEUP_INT.
 */
#define LP_UART_WAKEUP_INT_ST    (BIT(19))
#define LP_UART_WAKEUP_INT_ST_M  (LP_UART_WAKEUP_INT_ST_V << LP_UART_WAKEUP_INT_ST_S)
#define LP_UART_WAKEUP_INT_ST_V  0x00000001U
#define LP_UART_WAKEUP_INT_ST_S  19

/** LP_UART_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define LP_UART_INT_ENA_REG (DR_REG_LP_UART_BASE + 0xc)
/** LP_UART_RXFIFO_FULL_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable LP_UART_RXFIFO_FULL_INT.
 */
#define LP_UART_RXFIFO_FULL_INT_ENA    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_ENA_M  (LP_UART_RXFIFO_FULL_INT_ENA_V << LP_UART_RXFIFO_FULL_INT_ENA_S)
#define LP_UART_RXFIFO_FULL_INT_ENA_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_ENA_S  0
/** LP_UART_TXFIFO_EMPTY_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable LP_UART_TXFIFO_EMPTY_INT.
 */
#define LP_UART_TXFIFO_EMPTY_INT_ENA    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_ENA_M  (LP_UART_TXFIFO_EMPTY_INT_ENA_V << LP_UART_TXFIFO_EMPTY_INT_ENA_S)
#define LP_UART_TXFIFO_EMPTY_INT_ENA_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_ENA_S  1
/** LP_UART_PARITY_ERR_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable LP_UART_PARITY_ERR_INT.
 */
#define LP_UART_PARITY_ERR_INT_ENA    (BIT(2))
#define LP_UART_PARITY_ERR_INT_ENA_M  (LP_UART_PARITY_ERR_INT_ENA_V << LP_UART_PARITY_ERR_INT_ENA_S)
#define LP_UART_PARITY_ERR_INT_ENA_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_ENA_S  2
/** LP_UART_FRM_ERR_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Write 1 to enable LP_UART_FRM_ERR_INT.
 */
#define LP_UART_FRM_ERR_INT_ENA    (BIT(3))
#define LP_UART_FRM_ERR_INT_ENA_M  (LP_UART_FRM_ERR_INT_ENA_V << LP_UART_FRM_ERR_INT_ENA_S)
#define LP_UART_FRM_ERR_INT_ENA_V  0x00000001U
#define LP_UART_FRM_ERR_INT_ENA_S  3
/** LP_UART_RXFIFO_OVF_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable LP_UART_RXFIFO_OVF_INT.
 */
#define LP_UART_RXFIFO_OVF_INT_ENA    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_ENA_M  (LP_UART_RXFIFO_OVF_INT_ENA_V << LP_UART_RXFIFO_OVF_INT_ENA_S)
#define LP_UART_RXFIFO_OVF_INT_ENA_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_ENA_S  4
/** LP_UART_DSR_CHG_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Write 1 to enable LP_UART_DSR_CHG_INT.
 */
#define LP_UART_DSR_CHG_INT_ENA    (BIT(5))
#define LP_UART_DSR_CHG_INT_ENA_M  (LP_UART_DSR_CHG_INT_ENA_V << LP_UART_DSR_CHG_INT_ENA_S)
#define LP_UART_DSR_CHG_INT_ENA_V  0x00000001U
#define LP_UART_DSR_CHG_INT_ENA_S  5
/** LP_UART_CTS_CHG_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Write 1 to enable LP_UART_CTS_CHG_INT.
 */
#define LP_UART_CTS_CHG_INT_ENA    (BIT(6))
#define LP_UART_CTS_CHG_INT_ENA_M  (LP_UART_CTS_CHG_INT_ENA_V << LP_UART_CTS_CHG_INT_ENA_S)
#define LP_UART_CTS_CHG_INT_ENA_V  0x00000001U
#define LP_UART_CTS_CHG_INT_ENA_S  6
/** LP_UART_BRK_DET_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Write 1 to enable LP_UART_BRK_DET_INT.
 */
#define LP_UART_BRK_DET_INT_ENA    (BIT(7))
#define LP_UART_BRK_DET_INT_ENA_M  (LP_UART_BRK_DET_INT_ENA_V << LP_UART_BRK_DET_INT_ENA_S)
#define LP_UART_BRK_DET_INT_ENA_V  0x00000001U
#define LP_UART_BRK_DET_INT_ENA_S  7
/** LP_UART_RXFIFO_TOUT_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Write 1 to enable LP_UART_RXFIFO_TOUT_INT.
 */
#define LP_UART_RXFIFO_TOUT_INT_ENA    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_ENA_M  (LP_UART_RXFIFO_TOUT_INT_ENA_V << LP_UART_RXFIFO_TOUT_INT_ENA_S)
#define LP_UART_RXFIFO_TOUT_INT_ENA_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_ENA_S  8
/** LP_UART_SW_XON_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Write 1 to enable LP_UART_SW_XON_INT.
 */
#define LP_UART_SW_XON_INT_ENA    (BIT(9))
#define LP_UART_SW_XON_INT_ENA_M  (LP_UART_SW_XON_INT_ENA_V << LP_UART_SW_XON_INT_ENA_S)
#define LP_UART_SW_XON_INT_ENA_V  0x00000001U
#define LP_UART_SW_XON_INT_ENA_S  9
/** LP_UART_SW_XOFF_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  Write 1 to enable LP_UART_SW_XOFF_INT.
 */
#define LP_UART_SW_XOFF_INT_ENA    (BIT(10))
#define LP_UART_SW_XOFF_INT_ENA_M  (LP_UART_SW_XOFF_INT_ENA_V << LP_UART_SW_XOFF_INT_ENA_S)
#define LP_UART_SW_XOFF_INT_ENA_V  0x00000001U
#define LP_UART_SW_XOFF_INT_ENA_S  10
/** LP_UART_GLITCH_DET_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Write 1 to enable LP_UART_GLITCH_DET_INT.
 */
#define LP_UART_GLITCH_DET_INT_ENA    (BIT(11))
#define LP_UART_GLITCH_DET_INT_ENA_M  (LP_UART_GLITCH_DET_INT_ENA_V << LP_UART_GLITCH_DET_INT_ENA_S)
#define LP_UART_GLITCH_DET_INT_ENA_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_ENA_S  11
/** LP_UART_TX_BRK_DONE_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  Write 1 to enable LP_UART_TX_BRK_DONE_INT.
 */
#define LP_UART_TX_BRK_DONE_INT_ENA    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_ENA_M  (LP_UART_TX_BRK_DONE_INT_ENA_V << LP_UART_TX_BRK_DONE_INT_ENA_S)
#define LP_UART_TX_BRK_DONE_INT_ENA_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_ENA_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  Write 1 to enable LP_UART_TX_BRK_IDLE_DONE_INT.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA_M  (LP_UART_TX_BRK_IDLE_DONE_INT_ENA_V << LP_UART_TX_BRK_IDLE_DONE_INT_ENA_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA_S  13
/** LP_UART_TX_DONE_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  Write 1 to enable LP_UART_TX_DONE_INT.
 */
#define LP_UART_TX_DONE_INT_ENA    (BIT(14))
#define LP_UART_TX_DONE_INT_ENA_M  (LP_UART_TX_DONE_INT_ENA_V << LP_UART_TX_DONE_INT_ENA_S)
#define LP_UART_TX_DONE_INT_ENA_V  0x00000001U
#define LP_UART_TX_DONE_INT_ENA_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  Write 1 to enable LP_UART_AT_CMD_CHAR_DET_INT.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA_M  (LP_UART_AT_CMD_CHAR_DET_INT_ENA_V << LP_UART_AT_CMD_CHAR_DET_INT_ENA_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA_S  18
/** LP_UART_WAKEUP_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  Write 1 to enable LP_UART_WAKEUP_INT.
 */
#define LP_UART_WAKEUP_INT_ENA    (BIT(19))
#define LP_UART_WAKEUP_INT_ENA_M  (LP_UART_WAKEUP_INT_ENA_V << LP_UART_WAKEUP_INT_ENA_S)
#define LP_UART_WAKEUP_INT_ENA_V  0x00000001U
#define LP_UART_WAKEUP_INT_ENA_S  19

/** LP_UART_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define LP_UART_INT_CLR_REG (DR_REG_LP_UART_BASE + 0x10)
/** LP_UART_RXFIFO_FULL_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear LP_UART_RXFIFO_FULL_INT.
 */
#define LP_UART_RXFIFO_FULL_INT_CLR    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_CLR_M  (LP_UART_RXFIFO_FULL_INT_CLR_V << LP_UART_RXFIFO_FULL_INT_CLR_S)
#define LP_UART_RXFIFO_FULL_INT_CLR_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_CLR_S  0
/** LP_UART_TXFIFO_EMPTY_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear LP_UART_TXFIFO_EMPTY_INT.
 */
#define LP_UART_TXFIFO_EMPTY_INT_CLR    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_CLR_M  (LP_UART_TXFIFO_EMPTY_INT_CLR_V << LP_UART_TXFIFO_EMPTY_INT_CLR_S)
#define LP_UART_TXFIFO_EMPTY_INT_CLR_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_CLR_S  1
/** LP_UART_PARITY_ERR_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear LP_UART_PARITY_ERR_INT.
 */
#define LP_UART_PARITY_ERR_INT_CLR    (BIT(2))
#define LP_UART_PARITY_ERR_INT_CLR_M  (LP_UART_PARITY_ERR_INT_CLR_V << LP_UART_PARITY_ERR_INT_CLR_S)
#define LP_UART_PARITY_ERR_INT_CLR_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_CLR_S  2
/** LP_UART_FRM_ERR_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Write 1 to clear LP_UART_FRM_ERR_INT.
 */
#define LP_UART_FRM_ERR_INT_CLR    (BIT(3))
#define LP_UART_FRM_ERR_INT_CLR_M  (LP_UART_FRM_ERR_INT_CLR_V << LP_UART_FRM_ERR_INT_CLR_S)
#define LP_UART_FRM_ERR_INT_CLR_V  0x00000001U
#define LP_UART_FRM_ERR_INT_CLR_S  3
/** LP_UART_RXFIFO_OVF_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear LP_UART_RXFIFO_OVF_INT.
 */
#define LP_UART_RXFIFO_OVF_INT_CLR    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_CLR_M  (LP_UART_RXFIFO_OVF_INT_CLR_V << LP_UART_RXFIFO_OVF_INT_CLR_S)
#define LP_UART_RXFIFO_OVF_INT_CLR_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_CLR_S  4
/** LP_UART_DSR_CHG_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Write 1 to clear LP_UART_DSR_CHG_INT.
 */
#define LP_UART_DSR_CHG_INT_CLR    (BIT(5))
#define LP_UART_DSR_CHG_INT_CLR_M  (LP_UART_DSR_CHG_INT_CLR_V << LP_UART_DSR_CHG_INT_CLR_S)
#define LP_UART_DSR_CHG_INT_CLR_V  0x00000001U
#define LP_UART_DSR_CHG_INT_CLR_S  5
/** LP_UART_CTS_CHG_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Write 1 to clear LP_UART_CTS_CHG_INT.
 */
#define LP_UART_CTS_CHG_INT_CLR    (BIT(6))
#define LP_UART_CTS_CHG_INT_CLR_M  (LP_UART_CTS_CHG_INT_CLR_V << LP_UART_CTS_CHG_INT_CLR_S)
#define LP_UART_CTS_CHG_INT_CLR_V  0x00000001U
#define LP_UART_CTS_CHG_INT_CLR_S  6
/** LP_UART_BRK_DET_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Write 1 to clear LP_UART_BRK_DET_INT.
 */
#define LP_UART_BRK_DET_INT_CLR    (BIT(7))
#define LP_UART_BRK_DET_INT_CLR_M  (LP_UART_BRK_DET_INT_CLR_V << LP_UART_BRK_DET_INT_CLR_S)
#define LP_UART_BRK_DET_INT_CLR_V  0x00000001U
#define LP_UART_BRK_DET_INT_CLR_S  7
/** LP_UART_RXFIFO_TOUT_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Write 1 to clear LP_UART_RXFIFO_TOUT_INT.
 */
#define LP_UART_RXFIFO_TOUT_INT_CLR    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_CLR_M  (LP_UART_RXFIFO_TOUT_INT_CLR_V << LP_UART_RXFIFO_TOUT_INT_CLR_S)
#define LP_UART_RXFIFO_TOUT_INT_CLR_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_CLR_S  8
/** LP_UART_SW_XON_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Write 1 to clear LP_UART_SW_XON_INT.
 */
#define LP_UART_SW_XON_INT_CLR    (BIT(9))
#define LP_UART_SW_XON_INT_CLR_M  (LP_UART_SW_XON_INT_CLR_V << LP_UART_SW_XON_INT_CLR_S)
#define LP_UART_SW_XON_INT_CLR_V  0x00000001U
#define LP_UART_SW_XON_INT_CLR_S  9
/** LP_UART_SW_XOFF_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Write 1 to clear LP_UART_SW_XOFF_INT.
 */
#define LP_UART_SW_XOFF_INT_CLR    (BIT(10))
#define LP_UART_SW_XOFF_INT_CLR_M  (LP_UART_SW_XOFF_INT_CLR_V << LP_UART_SW_XOFF_INT_CLR_S)
#define LP_UART_SW_XOFF_INT_CLR_V  0x00000001U
#define LP_UART_SW_XOFF_INT_CLR_S  10
/** LP_UART_GLITCH_DET_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Write 1 to clear LP_UART_GLITCH_DET_INT.
 */
#define LP_UART_GLITCH_DET_INT_CLR    (BIT(11))
#define LP_UART_GLITCH_DET_INT_CLR_M  (LP_UART_GLITCH_DET_INT_CLR_V << LP_UART_GLITCH_DET_INT_CLR_S)
#define LP_UART_GLITCH_DET_INT_CLR_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_CLR_S  11
/** LP_UART_TX_BRK_DONE_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Write 1 to clear LP_UART_TX_BRK_DONE_INT.
 */
#define LP_UART_TX_BRK_DONE_INT_CLR    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_CLR_M  (LP_UART_TX_BRK_DONE_INT_CLR_V << LP_UART_TX_BRK_DONE_INT_CLR_S)
#define LP_UART_TX_BRK_DONE_INT_CLR_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_CLR_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Write 1 to clear LP_UART_TX_BRK_IDLE_DONE_INT.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR_M  (LP_UART_TX_BRK_IDLE_DONE_INT_CLR_V << LP_UART_TX_BRK_IDLE_DONE_INT_CLR_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR_S  13
/** LP_UART_TX_DONE_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Write 1 to clear LP_UART_TX_DONE_INT.
 */
#define LP_UART_TX_DONE_INT_CLR    (BIT(14))
#define LP_UART_TX_DONE_INT_CLR_M  (LP_UART_TX_DONE_INT_CLR_V << LP_UART_TX_DONE_INT_CLR_S)
#define LP_UART_TX_DONE_INT_CLR_V  0x00000001U
#define LP_UART_TX_DONE_INT_CLR_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Write 1 to clear LP_UART_AT_CMD_CHAR_DET_INT.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR_M  (LP_UART_AT_CMD_CHAR_DET_INT_CLR_V << LP_UART_AT_CMD_CHAR_DET_INT_CLR_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR_S  18
/** LP_UART_WAKEUP_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Write 1 to clear LP_UART_WAKEUP_INT.
 */
#define LP_UART_WAKEUP_INT_CLR    (BIT(19))
#define LP_UART_WAKEUP_INT_CLR_M  (LP_UART_WAKEUP_INT_CLR_V << LP_UART_WAKEUP_INT_CLR_S)
#define LP_UART_WAKEUP_INT_CLR_V  0x00000001U
#define LP_UART_WAKEUP_INT_CLR_S  19

/** LP_UART_CLKDIV_SYNC_REG register
 *  Clock divider configuration
 */
#define LP_UART_CLKDIV_SYNC_REG (DR_REG_LP_UART_BASE + 0x14)
/** LP_UART_CLKDIV : R/W; bitpos: [11:0]; default: 694;
 *  Configures the integral part of the divisor for baud rate generation.
 */
#define LP_UART_CLKDIV    0x00000FFFU
#define LP_UART_CLKDIV_M  (LP_UART_CLKDIV_V << LP_UART_CLKDIV_S)
#define LP_UART_CLKDIV_V  0x00000FFFU
#define LP_UART_CLKDIV_S  0
/** LP_UART_CLKDIV_FRAG : R/W; bitpos: [23:20]; default: 0;
 *  Configures the fractional part of the divisor for baud rate generation.
 */
#define LP_UART_CLKDIV_FRAG    0x0000000FU
#define LP_UART_CLKDIV_FRAG_M  (LP_UART_CLKDIV_FRAG_V << LP_UART_CLKDIV_FRAG_S)
#define LP_UART_CLKDIV_FRAG_V  0x0000000FU
#define LP_UART_CLKDIV_FRAG_S  20

/** LP_UART_RX_FILT_REG register
 *  RX filter configuration
 */
#define LP_UART_RX_FILT_REG (DR_REG_LP_UART_BASE + 0x18)
/** LP_UART_GLITCH_FILT : R/W; bitpos: [7:0]; default: 8;
 *  Configures the width of a pulse to be filtered.\\Measurement unit: UART Core's
 *  clock cycle.\\Pulses whose width is lower than this value will be ignored.
 */
#define LP_UART_GLITCH_FILT    0x000000FFU
#define LP_UART_GLITCH_FILT_M  (LP_UART_GLITCH_FILT_V << LP_UART_GLITCH_FILT_S)
#define LP_UART_GLITCH_FILT_V  0x000000FFU
#define LP_UART_GLITCH_FILT_S  0
/** LP_UART_GLITCH_FILT_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable RX signal filter.\\
 *  0: Disable\\
 *  1: Enable
 */
#define LP_UART_GLITCH_FILT_EN    (BIT(8))
#define LP_UART_GLITCH_FILT_EN_M  (LP_UART_GLITCH_FILT_EN_V << LP_UART_GLITCH_FILT_EN_S)
#define LP_UART_GLITCH_FILT_EN_V  0x00000001U
#define LP_UART_GLITCH_FILT_EN_S  8

/** LP_UART_STATUS_REG register
 *  LP UART status register
 */
#define LP_UART_STATUS_REG (DR_REG_LP_UART_BASE + 0x1c)
/** LP_UART_RXFIFO_CNT : RO; bitpos: [7:3]; default: 0;
 *  Represents the number of valid data bytes in RX FIFO.
 */
#define LP_UART_RXFIFO_CNT    0x0000001FU
#define LP_UART_RXFIFO_CNT_M  (LP_UART_RXFIFO_CNT_V << LP_UART_RXFIFO_CNT_S)
#define LP_UART_RXFIFO_CNT_V  0x0000001FU
#define LP_UART_RXFIFO_CNT_S  3
/** LP_UART_DSRN : RO; bitpos: [13]; default: 0;
 *  Represents the level of the internal LP UART DSR signal.
 */
#define LP_UART_DSRN    (BIT(13))
#define LP_UART_DSRN_M  (LP_UART_DSRN_V << LP_UART_DSRN_S)
#define LP_UART_DSRN_V  0x00000001U
#define LP_UART_DSRN_S  13
/** LP_UART_CTSN : RO; bitpos: [14]; default: 1;
 *  Represents the level of the internal LP UART CTS signal.
 */
#define LP_UART_CTSN    (BIT(14))
#define LP_UART_CTSN_M  (LP_UART_CTSN_V << LP_UART_CTSN_S)
#define LP_UART_CTSN_V  0x00000001U
#define LP_UART_CTSN_S  14
/** LP_UART_RXD : RO; bitpos: [15]; default: 1;
 *  Represents the  level of the internal LP UART RXD signal.
 */
#define LP_UART_RXD    (BIT(15))
#define LP_UART_RXD_M  (LP_UART_RXD_V << LP_UART_RXD_S)
#define LP_UART_RXD_V  0x00000001U
#define LP_UART_RXD_S  15
/** LP_UART_TXFIFO_CNT : RO; bitpos: [23:19]; default: 0;
 *  Represents the number of valid data bytes in RX FIFO.
 */
#define LP_UART_TXFIFO_CNT    0x0000001FU
#define LP_UART_TXFIFO_CNT_M  (LP_UART_TXFIFO_CNT_V << LP_UART_TXFIFO_CNT_S)
#define LP_UART_TXFIFO_CNT_V  0x0000001FU
#define LP_UART_TXFIFO_CNT_S  19
/** LP_UART_DTRN : RO; bitpos: [29]; default: 1;
 *  Represents the level of the internal LP UART DTR signal.
 */
#define LP_UART_DTRN    (BIT(29))
#define LP_UART_DTRN_M  (LP_UART_DTRN_V << LP_UART_DTRN_S)
#define LP_UART_DTRN_V  0x00000001U
#define LP_UART_DTRN_S  29
/** LP_UART_RTSN : RO; bitpos: [30]; default: 1;
 *  Represents the level of the internal LP UART RTS signal.
 */
#define LP_UART_RTSN    (BIT(30))
#define LP_UART_RTSN_M  (LP_UART_RTSN_V << LP_UART_RTSN_S)
#define LP_UART_RTSN_V  0x00000001U
#define LP_UART_RTSN_S  30
/** LP_UART_TXD : RO; bitpos: [31]; default: 1;
 *  Represents the  level of the internal LP UART TXD signal.
 */
#define LP_UART_TXD    (BIT(31))
#define LP_UART_TXD_M  (LP_UART_TXD_V << LP_UART_TXD_S)
#define LP_UART_TXD_V  0x00000001U
#define LP_UART_TXD_S  31

/** LP_UART_CONF0_SYNC_REG register
 *  Configuration register 0
 */
#define LP_UART_CONF0_SYNC_REG (DR_REG_LP_UART_BASE + 0x20)
/** LP_UART_PARITY : R/W; bitpos: [0]; default: 0;
 *  Configures the parity check mode.\\
 *  0: Even parity\\
 *  1: Odd parity\\
 */
#define LP_UART_PARITY    (BIT(0))
#define LP_UART_PARITY_M  (LP_UART_PARITY_V << LP_UART_PARITY_S)
#define LP_UART_PARITY_V  0x00000001U
#define LP_UART_PARITY_S  0
/** LP_UART_PARITY_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable LP UART parity check.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_PARITY_EN    (BIT(1))
#define LP_UART_PARITY_EN_M  (LP_UART_PARITY_EN_V << LP_UART_PARITY_EN_S)
#define LP_UART_PARITY_EN_V  0x00000001U
#define LP_UART_PARITY_EN_S  1
/** LP_UART_BIT_NUM : R/W; bitpos: [3:2]; default: 3;
 *  Configures the number of data bits.\\
 *  0: 5 bits\\
 *  1: 6 bits\\
 *  2: 7 bits\\
 *  3: 8 bits\\
 */
#define LP_UART_BIT_NUM    0x00000003U
#define LP_UART_BIT_NUM_M  (LP_UART_BIT_NUM_V << LP_UART_BIT_NUM_S)
#define LP_UART_BIT_NUM_V  0x00000003U
#define LP_UART_BIT_NUM_S  2
/** LP_UART_STOP_BIT_NUM : R/W; bitpos: [5:4]; default: 1;
 *  Configures the number of stop bits.\\
 *  0: Invalid. No effect\\
 *  1: 1 bits\\
 *  2: 1.5 bits\\
 *  3: 2 bits\\
 */
#define LP_UART_STOP_BIT_NUM    0x00000003U
#define LP_UART_STOP_BIT_NUM_M  (LP_UART_STOP_BIT_NUM_V << LP_UART_STOP_BIT_NUM_S)
#define LP_UART_STOP_BIT_NUM_V  0x00000003U
#define LP_UART_STOP_BIT_NUM_S  4
/** LP_UART_TXD_BRK : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to send NULL characters when finishing data
 *  transmission.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define LP_UART_TXD_BRK    (BIT(6))
#define LP_UART_TXD_BRK_M  (LP_UART_TXD_BRK_V << LP_UART_TXD_BRK_S)
#define LP_UART_TXD_BRK_V  0x00000001U
#define LP_UART_TXD_BRK_S  6
/** LP_UART_LOOPBACK : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable LP UART loopback test.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_LOOPBACK    (BIT(12))
#define LP_UART_LOOPBACK_M  (LP_UART_LOOPBACK_V << LP_UART_LOOPBACK_S)
#define LP_UART_LOOPBACK_V  0x00000001U
#define LP_UART_LOOPBACK_S  12
/** LP_UART_TX_FLOW_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable flow control for the transmitter.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_TX_FLOW_EN    (BIT(13))
#define LP_UART_TX_FLOW_EN_M  (LP_UART_TX_FLOW_EN_V << LP_UART_TX_FLOW_EN_S)
#define LP_UART_TX_FLOW_EN_V  0x00000001U
#define LP_UART_TX_FLOW_EN_S  13
/** LP_UART_RXD_INV : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to invert the level of LP UART RXD signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_UART_RXD_INV    (BIT(15))
#define LP_UART_RXD_INV_M  (LP_UART_RXD_INV_V << LP_UART_RXD_INV_S)
#define LP_UART_RXD_INV_V  0x00000001U
#define LP_UART_RXD_INV_S  15
/** LP_UART_TXD_INV : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to invert the level of LP UART TXD signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_UART_TXD_INV    (BIT(16))
#define LP_UART_TXD_INV_M  (LP_UART_TXD_INV_V << LP_UART_TXD_INV_S)
#define LP_UART_TXD_INV_V  0x00000001U
#define LP_UART_TXD_INV_S  16
/** LP_UART_DIS_RX_DAT_OVF : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to disable data overflow detection for the LP UART
 *  receiver.\\
 *  0: Enable\\
 *  1: Disable\\
 */
#define LP_UART_DIS_RX_DAT_OVF    (BIT(17))
#define LP_UART_DIS_RX_DAT_OVF_M  (LP_UART_DIS_RX_DAT_OVF_V << LP_UART_DIS_RX_DAT_OVF_S)
#define LP_UART_DIS_RX_DAT_OVF_V  0x00000001U
#define LP_UART_DIS_RX_DAT_OVF_S  17
/** LP_UART_ERR_WR_MASK : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to store the received data with errors into FIFO.\\
 *  0: Store\\
 *  1: Not store\\
 */
#define LP_UART_ERR_WR_MASK    (BIT(18))
#define LP_UART_ERR_WR_MASK_M  (LP_UART_ERR_WR_MASK_V << LP_UART_ERR_WR_MASK_S)
#define LP_UART_ERR_WR_MASK_V  0x00000001U
#define LP_UART_ERR_WR_MASK_S  18
/** LP_UART_MEM_CLK_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable clock gating for LP UART memory.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_MEM_CLK_EN    (BIT(20))
#define LP_UART_MEM_CLK_EN_M  (LP_UART_MEM_CLK_EN_V << LP_UART_MEM_CLK_EN_S)
#define LP_UART_MEM_CLK_EN_V  0x00000001U
#define LP_UART_MEM_CLK_EN_S  20
/** LP_UART_SW_RTS : R/W; bitpos: [21]; default: 0;
 *  Configures the RTS signal used in software flow control.\\
 *  0: The LP UART transmitter is allowed to send data.\\
 *  1: The LP UART transmitted is not allowed to send data.\\
 */
#define LP_UART_SW_RTS    (BIT(21))
#define LP_UART_SW_RTS_M  (LP_UART_SW_RTS_V << LP_UART_SW_RTS_S)
#define LP_UART_SW_RTS_V  0x00000001U
#define LP_UART_SW_RTS_S  21
/** LP_UART_RXFIFO_RST : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to reset the LP UART RX FIFO.\\
 *  0: Not reset\\
 *  1: Reset\\
 */
#define LP_UART_RXFIFO_RST    (BIT(22))
#define LP_UART_RXFIFO_RST_M  (LP_UART_RXFIFO_RST_V << LP_UART_RXFIFO_RST_S)
#define LP_UART_RXFIFO_RST_V  0x00000001U
#define LP_UART_RXFIFO_RST_S  22
/** LP_UART_TXFIFO_RST : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to reset the LP UART TX FIFO.\\
 *  0: Not reset\\
 *  1: Reset\\
 */
#define LP_UART_TXFIFO_RST    (BIT(23))
#define LP_UART_TXFIFO_RST_M  (LP_UART_TXFIFO_RST_V << LP_UART_TXFIFO_RST_S)
#define LP_UART_TXFIFO_RST_V  0x00000001U
#define LP_UART_TXFIFO_RST_S  23

/** LP_UART_CONF1_REG register
 *  Configuration register 1
 */
#define LP_UART_CONF1_REG (DR_REG_LP_UART_BASE + 0x24)
/** LP_UART_RXFIFO_FULL_THRHD : R/W; bitpos: [7:3]; default: 12;
 *  Configures the threshold for RX FIFO being full.\\Measurement unit: byte.
 */
#define LP_UART_RXFIFO_FULL_THRHD    0x0000001FU
#define LP_UART_RXFIFO_FULL_THRHD_M  (LP_UART_RXFIFO_FULL_THRHD_V << LP_UART_RXFIFO_FULL_THRHD_S)
#define LP_UART_RXFIFO_FULL_THRHD_V  0x0000001FU
#define LP_UART_RXFIFO_FULL_THRHD_S  3
/** LP_UART_TXFIFO_EMPTY_THRHD : R/W; bitpos: [15:11]; default: 12;
 *  Configures the threshold for TX FIFO being empty.\\Measurement unit: byte.
 */
#define LP_UART_TXFIFO_EMPTY_THRHD    0x0000001FU
#define LP_UART_TXFIFO_EMPTY_THRHD_M  (LP_UART_TXFIFO_EMPTY_THRHD_V << LP_UART_TXFIFO_EMPTY_THRHD_S)
#define LP_UART_TXFIFO_EMPTY_THRHD_V  0x0000001FU
#define LP_UART_TXFIFO_EMPTY_THRHD_S  11
/** LP_UART_CTS_INV : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to invert the level of LP UART CTS signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_UART_CTS_INV    (BIT(16))
#define LP_UART_CTS_INV_M  (LP_UART_CTS_INV_V << LP_UART_CTS_INV_S)
#define LP_UART_CTS_INV_V  0x00000001U
#define LP_UART_CTS_INV_S  16
/** LP_UART_DSR_INV : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to invert the level of LP UART DSR signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_UART_DSR_INV    (BIT(17))
#define LP_UART_DSR_INV_M  (LP_UART_DSR_INV_V << LP_UART_DSR_INV_S)
#define LP_UART_DSR_INV_V  0x00000001U
#define LP_UART_DSR_INV_S  17
/** LP_UART_RTS_INV : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to invert the level of LP UART RTS signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_UART_RTS_INV    (BIT(18))
#define LP_UART_RTS_INV_M  (LP_UART_RTS_INV_V << LP_UART_RTS_INV_S)
#define LP_UART_RTS_INV_V  0x00000001U
#define LP_UART_RTS_INV_S  18
/** LP_UART_DTR_INV : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to invert the level of LP UART DTR signal.\\
 *  0: Not invert\\
 *  1: Invert\\
 */
#define LP_UART_DTR_INV    (BIT(19))
#define LP_UART_DTR_INV_M  (LP_UART_DTR_INV_V << LP_UART_DTR_INV_S)
#define LP_UART_DTR_INV_V  0x00000001U
#define LP_UART_DTR_INV_S  19
/** LP_UART_SW_DTR : R/W; bitpos: [20]; default: 0;
 *  Configures the DTR signal used in software flow control.\\
 *  0: Data to be transmitted is not ready.\\
 *  1: Data to be transmitted is ready.\\
 */
#define LP_UART_SW_DTR    (BIT(20))
#define LP_UART_SW_DTR_M  (LP_UART_SW_DTR_V << LP_UART_SW_DTR_S)
#define LP_UART_SW_DTR_V  0x00000001U
#define LP_UART_SW_DTR_S  20
/** LP_UART_CLK_EN : R/W; bitpos: [21]; default: 0;
 *  Configures clock gating.\\
 *  0: Support clock only when the application writes registers.\\
 *  1: Always force the clock on for registers.\\
 */
#define LP_UART_CLK_EN    (BIT(21))
#define LP_UART_CLK_EN_M  (LP_UART_CLK_EN_V << LP_UART_CLK_EN_S)
#define LP_UART_CLK_EN_V  0x00000001U
#define LP_UART_CLK_EN_S  21

/** LP_UART_HWFC_CONF_SYNC_REG register
 *  Hardware flow control configuration
 */
#define LP_UART_HWFC_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x2c)
/** LP_UART_RX_FLOW_THRHD : R/W; bitpos: [7:3]; default: 0;
 *  Configures the maximum number of data bytes that can be received  during hardware
 *  flow control.\\Measurement unit: byte.
 */
#define LP_UART_RX_FLOW_THRHD    0x0000001FU
#define LP_UART_RX_FLOW_THRHD_M  (LP_UART_RX_FLOW_THRHD_V << LP_UART_RX_FLOW_THRHD_S)
#define LP_UART_RX_FLOW_THRHD_V  0x0000001FU
#define LP_UART_RX_FLOW_THRHD_S  3
/** LP_UART_RX_FLOW_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable the LP UART receiver.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_RX_FLOW_EN    (BIT(8))
#define LP_UART_RX_FLOW_EN_M  (LP_UART_RX_FLOW_EN_V << LP_UART_RX_FLOW_EN_S)
#define LP_UART_RX_FLOW_EN_V  0x00000001U
#define LP_UART_RX_FLOW_EN_S  8

/** LP_UART_SLEEP_CONF0_REG register
 *  LP UART sleep configuration register 0
 */
#define LP_UART_SLEEP_CONF0_REG (DR_REG_LP_UART_BASE + 0x30)
/** LP_UART_WK_CHAR1 : R/W; bitpos: [7:0]; default: 0;
 *  Configures wakeup character 1.
 */
#define LP_UART_WK_CHAR1    0x000000FFU
#define LP_UART_WK_CHAR1_M  (LP_UART_WK_CHAR1_V << LP_UART_WK_CHAR1_S)
#define LP_UART_WK_CHAR1_V  0x000000FFU
#define LP_UART_WK_CHAR1_S  0
/** LP_UART_WK_CHAR2 : R/W; bitpos: [15:8]; default: 0;
 *  Configures wakeup character 2.
 */
#define LP_UART_WK_CHAR2    0x000000FFU
#define LP_UART_WK_CHAR2_M  (LP_UART_WK_CHAR2_V << LP_UART_WK_CHAR2_S)
#define LP_UART_WK_CHAR2_V  0x000000FFU
#define LP_UART_WK_CHAR2_S  8
/** LP_UART_WK_CHAR3 : R/W; bitpos: [23:16]; default: 0;
 *  Configures wakeup character 3.
 */
#define LP_UART_WK_CHAR3    0x000000FFU
#define LP_UART_WK_CHAR3_M  (LP_UART_WK_CHAR3_V << LP_UART_WK_CHAR3_S)
#define LP_UART_WK_CHAR3_V  0x000000FFU
#define LP_UART_WK_CHAR3_S  16
/** LP_UART_WK_CHAR4 : R/W; bitpos: [31:24]; default: 0;
 *  Configures wakeup character 4.
 */
#define LP_UART_WK_CHAR4    0x000000FFU
#define LP_UART_WK_CHAR4_M  (LP_UART_WK_CHAR4_V << LP_UART_WK_CHAR4_S)
#define LP_UART_WK_CHAR4_V  0x000000FFU
#define LP_UART_WK_CHAR4_S  24

/** LP_UART_SLEEP_CONF1_REG register
 *  LP UART sleep configuration register 1
 */
#define LP_UART_SLEEP_CONF1_REG (DR_REG_LP_UART_BASE + 0x34)
/** LP_UART_WK_CHAR0 : R/W; bitpos: [7:0]; default: 0;
 *  Configures wakeup character 0.
 */
#define LP_UART_WK_CHAR0    0x000000FFU
#define LP_UART_WK_CHAR0_M  (LP_UART_WK_CHAR0_V << LP_UART_WK_CHAR0_S)
#define LP_UART_WK_CHAR0_V  0x000000FFU
#define LP_UART_WK_CHAR0_S  0

/** LP_UART_SLEEP_CONF2_REG register
 *  LP UART sleep configuration register 2
 */
#define LP_UART_SLEEP_CONF2_REG (DR_REG_LP_UART_BASE + 0x38)
/** LP_UART_ACTIVE_THRESHOLD : R/W; bitpos: [9:0]; default: 240;
 *  Configures the number of RXD edge changes to wake up the chip in wakeup mode 0.
 */
#define LP_UART_ACTIVE_THRESHOLD    0x000003FFU
#define LP_UART_ACTIVE_THRESHOLD_M  (LP_UART_ACTIVE_THRESHOLD_V << LP_UART_ACTIVE_THRESHOLD_S)
#define LP_UART_ACTIVE_THRESHOLD_V  0x000003FFU
#define LP_UART_ACTIVE_THRESHOLD_S  0
/** LP_UART_RX_WAKE_UP_THRHD : R/W; bitpos: [17:13]; default: 1;
 *  Configures the number of received data bytes to wake up the chip in wakeup mode 1.
 */
#define LP_UART_RX_WAKE_UP_THRHD    0x0000001FU
#define LP_UART_RX_WAKE_UP_THRHD_M  (LP_UART_RX_WAKE_UP_THRHD_V << LP_UART_RX_WAKE_UP_THRHD_S)
#define LP_UART_RX_WAKE_UP_THRHD_V  0x0000001FU
#define LP_UART_RX_WAKE_UP_THRHD_S  13
/** LP_UART_WK_CHAR_NUM : R/W; bitpos: [20:18]; default: 5;
 *  Configures the number of wakeup characters.
 */
#define LP_UART_WK_CHAR_NUM    0x00000007U
#define LP_UART_WK_CHAR_NUM_M  (LP_UART_WK_CHAR_NUM_V << LP_UART_WK_CHAR_NUM_S)
#define LP_UART_WK_CHAR_NUM_V  0x00000007U
#define LP_UART_WK_CHAR_NUM_S  18
/** LP_UART_WK_CHAR_MASK : R/W; bitpos: [25:21]; default: 0;
 *  Configures whether or not to mask wakeup characters.\\
 *  0: Not mask\\
 *  1: Mask\\
 */
#define LP_UART_WK_CHAR_MASK    0x0000001FU
#define LP_UART_WK_CHAR_MASK_M  (LP_UART_WK_CHAR_MASK_V << LP_UART_WK_CHAR_MASK_S)
#define LP_UART_WK_CHAR_MASK_V  0x0000001FU
#define LP_UART_WK_CHAR_MASK_S  21
/** LP_UART_WK_MODE_SEL : R/W; bitpos: [27:26]; default: 0;
 *  Configures which wakeup mode to select.\\
 *  0: Mode 0\\
 *  1: Mode 1\\
 *  2: Mode 2\\
 *  3: Mode 3\\
 */
#define LP_UART_WK_MODE_SEL    0x00000003U
#define LP_UART_WK_MODE_SEL_M  (LP_UART_WK_MODE_SEL_V << LP_UART_WK_MODE_SEL_S)
#define LP_UART_WK_MODE_SEL_V  0x00000003U
#define LP_UART_WK_MODE_SEL_S  26

/** LP_UART_SWFC_CONF0_SYNC_REG register
 *  Software flow control character configuration
 */
#define LP_UART_SWFC_CONF0_SYNC_REG (DR_REG_LP_UART_BASE + 0x3c)
/** LP_UART_XON_CHAR : R/W; bitpos: [7:0]; default: 17;
 *  Configures the XON character for flow control.
 */
#define LP_UART_XON_CHAR    0x000000FFU
#define LP_UART_XON_CHAR_M  (LP_UART_XON_CHAR_V << LP_UART_XON_CHAR_S)
#define LP_UART_XON_CHAR_V  0x000000FFU
#define LP_UART_XON_CHAR_S  0
/** LP_UART_XOFF_CHAR : R/W; bitpos: [15:8]; default: 19;
 *  Configures the XOFF character for flow control.
 */
#define LP_UART_XOFF_CHAR    0x000000FFU
#define LP_UART_XOFF_CHAR_M  (LP_UART_XOFF_CHAR_V << LP_UART_XOFF_CHAR_S)
#define LP_UART_XOFF_CHAR_V  0x000000FFU
#define LP_UART_XOFF_CHAR_S  8
/** LP_UART_XON_XOFF_STILL_SEND : R/W; bitpos: [16]; default: 0;
 *  Configures whether the LP UART transmitter can send XON or XOFF characters when it
 *  is disabled.\\
 *  0: Cannot send\\
 *  1: Can send\\
 */
#define LP_UART_XON_XOFF_STILL_SEND    (BIT(16))
#define LP_UART_XON_XOFF_STILL_SEND_M  (LP_UART_XON_XOFF_STILL_SEND_V << LP_UART_XON_XOFF_STILL_SEND_S)
#define LP_UART_XON_XOFF_STILL_SEND_V  0x00000001U
#define LP_UART_XON_XOFF_STILL_SEND_S  16
/** LP_UART_SW_FLOW_CON_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable software flow control.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_SW_FLOW_CON_EN    (BIT(17))
#define LP_UART_SW_FLOW_CON_EN_M  (LP_UART_SW_FLOW_CON_EN_V << LP_UART_SW_FLOW_CON_EN_S)
#define LP_UART_SW_FLOW_CON_EN_V  0x00000001U
#define LP_UART_SW_FLOW_CON_EN_S  17
/** LP_UART_XONOFF_DEL : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to remove flow control characters from the received
 *  data.\\
 *  0: Not move\\
 *  1: Move\\
 */
#define LP_UART_XONOFF_DEL    (BIT(18))
#define LP_UART_XONOFF_DEL_M  (LP_UART_XONOFF_DEL_V << LP_UART_XONOFF_DEL_S)
#define LP_UART_XONOFF_DEL_V  0x00000001U
#define LP_UART_XONOFF_DEL_S  18
/** LP_UART_FORCE_XON : R/W; bitpos: [19]; default: 0;
 *  Configures whether the transmitter continues to sending data.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define LP_UART_FORCE_XON    (BIT(19))
#define LP_UART_FORCE_XON_M  (LP_UART_FORCE_XON_V << LP_UART_FORCE_XON_S)
#define LP_UART_FORCE_XON_V  0x00000001U
#define LP_UART_FORCE_XON_S  19
/** LP_UART_FORCE_XOFF : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to stop the transmitter from sending data.\\
 *  0: Not stop\\
 *  1: Stop\\
 */
#define LP_UART_FORCE_XOFF    (BIT(20))
#define LP_UART_FORCE_XOFF_M  (LP_UART_FORCE_XOFF_V << LP_UART_FORCE_XOFF_S)
#define LP_UART_FORCE_XOFF_V  0x00000001U
#define LP_UART_FORCE_XOFF_S  20
/** LP_UART_SEND_XON : R/W/SS/SC; bitpos: [21]; default: 0;
 *  Configures whether or not to send XON characters.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define LP_UART_SEND_XON    (BIT(21))
#define LP_UART_SEND_XON_M  (LP_UART_SEND_XON_V << LP_UART_SEND_XON_S)
#define LP_UART_SEND_XON_V  0x00000001U
#define LP_UART_SEND_XON_S  21
/** LP_UART_SEND_XOFF : R/W/SS/SC; bitpos: [22]; default: 0;
 *  Configures whether or not to send XOFF characters.\\
 *  0: Not send\\
 *  1: Send\\
 */
#define LP_UART_SEND_XOFF    (BIT(22))
#define LP_UART_SEND_XOFF_M  (LP_UART_SEND_XOFF_V << LP_UART_SEND_XOFF_S)
#define LP_UART_SEND_XOFF_V  0x00000001U
#define LP_UART_SEND_XOFF_S  22

/** LP_UART_SWFC_CONF1_REG register
 *  Software flow control character configuration
 */
#define LP_UART_SWFC_CONF1_REG (DR_REG_LP_UART_BASE + 0x40)
/** LP_UART_XON_THRESHOLD : R/W; bitpos: [7:3]; default: 0;
 *  Configures the threshold for data in RX FIFO to send XON characters in software
 *  flow control.\\Measurement unit: byte.
 */
#define LP_UART_XON_THRESHOLD    0x0000001FU
#define LP_UART_XON_THRESHOLD_M  (LP_UART_XON_THRESHOLD_V << LP_UART_XON_THRESHOLD_S)
#define LP_UART_XON_THRESHOLD_V  0x0000001FU
#define LP_UART_XON_THRESHOLD_S  3
/** LP_UART_XOFF_THRESHOLD : R/W; bitpos: [15:11]; default: 12;
 *  Configures the threshold for data in RX FIFO to send XOFF characters in software
 *  flow control.\\Measurement unit: byte.
 */
#define LP_UART_XOFF_THRESHOLD    0x0000001FU
#define LP_UART_XOFF_THRESHOLD_M  (LP_UART_XOFF_THRESHOLD_V << LP_UART_XOFF_THRESHOLD_S)
#define LP_UART_XOFF_THRESHOLD_V  0x0000001FU
#define LP_UART_XOFF_THRESHOLD_S  11

/** LP_UART_TXBRK_CONF_SYNC_REG register
 *  TX break character configuration
 */
#define LP_UART_TXBRK_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x44)
/** LP_UART_TX_BRK_NUM : R/W; bitpos: [7:0]; default: 10;
 *  Configures the number of NULL characters to be sent after finishing data
 *  transmission.\\Valid only when LP_UART_TXD_BRK is 1.
 */
#define LP_UART_TX_BRK_NUM    0x000000FFU
#define LP_UART_TX_BRK_NUM_M  (LP_UART_TX_BRK_NUM_V << LP_UART_TX_BRK_NUM_S)
#define LP_UART_TX_BRK_NUM_V  0x000000FFU
#define LP_UART_TX_BRK_NUM_S  0

/** LP_UART_IDLE_CONF_SYNC_REG register
 *  Frame end idle time configuration
 */
#define LP_UART_IDLE_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x48)
/** LP_UART_RX_IDLE_THRHD : R/W; bitpos: [9:0]; default: 256;
 *  Configures the threshold to generate a frame end signal when the receiver takes
 *  more time to receive one data byte data.\\Measurement unit: bit time (the time to
 *  transmit 1 bit).
 */
#define LP_UART_RX_IDLE_THRHD    0x000003FFU
#define LP_UART_RX_IDLE_THRHD_M  (LP_UART_RX_IDLE_THRHD_V << LP_UART_RX_IDLE_THRHD_S)
#define LP_UART_RX_IDLE_THRHD_V  0x000003FFU
#define LP_UART_RX_IDLE_THRHD_S  0
/** LP_UART_TX_IDLE_NUM : R/W; bitpos: [19:10]; default: 256;
 *  Configures the interval between two data transfers.\\Measurement unit: bit time
 *  (the time to transmit 1 bit).
 */
#define LP_UART_TX_IDLE_NUM    0x000003FFU
#define LP_UART_TX_IDLE_NUM_M  (LP_UART_TX_IDLE_NUM_V << LP_UART_TX_IDLE_NUM_S)
#define LP_UART_TX_IDLE_NUM_V  0x000003FFU
#define LP_UART_TX_IDLE_NUM_S  10

/** LP_UART_DELAY_CONF_SYNC_REG register
 *  RS485 mode configuration
 */
#define LP_UART_DELAY_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x4c)
/** LP_UART_DL0_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to add a turnaround delay of 1 bit before the start bit.\\
 *  0: Not add\\
 *  1: Add\\
 */
#define LP_UART_DL0_EN    (BIT(1))
#define LP_UART_DL0_EN_M  (LP_UART_DL0_EN_V << LP_UART_DL0_EN_S)
#define LP_UART_DL0_EN_V  0x00000001U
#define LP_UART_DL0_EN_S  1
/** LP_UART_DL1_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to add a turnaround delay of 1 bit after the stop bit.\\
 *  0: Not add\\
 *  1: Add\\
 */
#define LP_UART_DL1_EN    (BIT(2))
#define LP_UART_DL1_EN_M  (LP_UART_DL1_EN_V << LP_UART_DL1_EN_S)
#define LP_UART_DL1_EN_V  0x00000001U
#define LP_UART_DL1_EN_S  2

/** LP_UART_AT_CMD_PRECNT_SYNC_REG register
 *  Pre-sequence timing configuration
 */
#define LP_UART_AT_CMD_PRECNT_SYNC_REG (DR_REG_LP_UART_BASE + 0x50)
/** LP_UART_PRE_IDLE_NUM : R/W; bitpos: [15:0]; default: 2305;
 *  Configures the idle time before the receiver receives the first
 *  AT_CMD.\\Measurement unit: bit time (the time to transmit 1 bit).
 */
#define LP_UART_PRE_IDLE_NUM    0x0000FFFFU
#define LP_UART_PRE_IDLE_NUM_M  (LP_UART_PRE_IDLE_NUM_V << LP_UART_PRE_IDLE_NUM_S)
#define LP_UART_PRE_IDLE_NUM_V  0x0000FFFFU
#define LP_UART_PRE_IDLE_NUM_S  0

/** LP_UART_AT_CMD_POSTCNT_SYNC_REG register
 *  Post-sequence timing configuration
 */
#define LP_UART_AT_CMD_POSTCNT_SYNC_REG (DR_REG_LP_UART_BASE + 0x54)
/** LP_UART_POST_IDLE_NUM : R/W; bitpos: [15:0]; default: 2305;
 *  Configures the interval between the last AT_CMD and subsequent data.\\Measurement
 *  unit: bit time (the time to transmit 1 bit).
 */
#define LP_UART_POST_IDLE_NUM    0x0000FFFFU
#define LP_UART_POST_IDLE_NUM_M  (LP_UART_POST_IDLE_NUM_V << LP_UART_POST_IDLE_NUM_S)
#define LP_UART_POST_IDLE_NUM_V  0x0000FFFFU
#define LP_UART_POST_IDLE_NUM_S  0

/** LP_UART_AT_CMD_GAPTOUT_SYNC_REG register
 *  Timeout configuration
 */
#define LP_UART_AT_CMD_GAPTOUT_SYNC_REG (DR_REG_LP_UART_BASE + 0x58)
/** LP_UART_RX_GAP_TOUT : R/W; bitpos: [15:0]; default: 11;
 *  Configures the interval between two AT_CMD characters.\\Measurement unit: bit time
 *  (the time to transmit 1 bit).
 */
#define LP_UART_RX_GAP_TOUT    0x0000FFFFU
#define LP_UART_RX_GAP_TOUT_M  (LP_UART_RX_GAP_TOUT_V << LP_UART_RX_GAP_TOUT_S)
#define LP_UART_RX_GAP_TOUT_V  0x0000FFFFU
#define LP_UART_RX_GAP_TOUT_S  0

/** LP_UART_AT_CMD_CHAR_SYNC_REG register
 *  AT escape sequence detection configuration
 */
#define LP_UART_AT_CMD_CHAR_SYNC_REG (DR_REG_LP_UART_BASE + 0x5c)
/** LP_UART_AT_CMD_CHAR : R/W; bitpos: [7:0]; default: 43;
 *  Configures the AT_CMD character.
 */
#define LP_UART_AT_CMD_CHAR    0x000000FFU
#define LP_UART_AT_CMD_CHAR_M  (LP_UART_AT_CMD_CHAR_V << LP_UART_AT_CMD_CHAR_S)
#define LP_UART_AT_CMD_CHAR_V  0x000000FFU
#define LP_UART_AT_CMD_CHAR_S  0
/** LP_UART_CHAR_NUM : R/W; bitpos: [15:8]; default: 3;
 *  Configures the number of continuous AT_CMD characters a receiver can receive.
 */
#define LP_UART_CHAR_NUM    0x000000FFU
#define LP_UART_CHAR_NUM_M  (LP_UART_CHAR_NUM_V << LP_UART_CHAR_NUM_S)
#define LP_UART_CHAR_NUM_V  0x000000FFU
#define LP_UART_CHAR_NUM_S  8

/** LP_UART_MEM_CONF_REG register
 *  UART memory power configuration
 */
#define LP_UART_MEM_CONF_REG (DR_REG_LP_UART_BASE + 0x60)
/** LP_UART_MEM_FORCE_PD : R/W; bitpos: [25]; default: 0;
 *  Set this bit to force power down UART memory.
 */
#define LP_UART_MEM_FORCE_PD    (BIT(25))
#define LP_UART_MEM_FORCE_PD_M  (LP_UART_MEM_FORCE_PD_V << LP_UART_MEM_FORCE_PD_S)
#define LP_UART_MEM_FORCE_PD_V  0x00000001U
#define LP_UART_MEM_FORCE_PD_S  25
/** LP_UART_MEM_FORCE_PU : R/W; bitpos: [26]; default: 0;
 *  Set this bit to force power up UART memory.
 */
#define LP_UART_MEM_FORCE_PU    (BIT(26))
#define LP_UART_MEM_FORCE_PU_M  (LP_UART_MEM_FORCE_PU_V << LP_UART_MEM_FORCE_PU_S)
#define LP_UART_MEM_FORCE_PU_V  0x00000001U
#define LP_UART_MEM_FORCE_PU_S  26

/** LP_UART_TOUT_CONF_SYNC_REG register
 *  LP UART threshold and allocation configuration
 */
#define LP_UART_TOUT_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x64)
/** LP_UART_RX_TOUT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable LP UART receiver's timeout function.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_RX_TOUT_EN    (BIT(0))
#define LP_UART_RX_TOUT_EN_M  (LP_UART_RX_TOUT_EN_V << LP_UART_RX_TOUT_EN_S)
#define LP_UART_RX_TOUT_EN_V  0x00000001U
#define LP_UART_RX_TOUT_EN_S  0
/** LP_UART_RX_TOUT_FLOW_DIS : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to stop the idle status counter when hardware flow
 *  control is enabled.\\
 *  0: Invalid. No effect\\
 *  1: Stop\\
 */
#define LP_UART_RX_TOUT_FLOW_DIS    (BIT(1))
#define LP_UART_RX_TOUT_FLOW_DIS_M  (LP_UART_RX_TOUT_FLOW_DIS_V << LP_UART_RX_TOUT_FLOW_DIS_S)
#define LP_UART_RX_TOUT_FLOW_DIS_V  0x00000001U
#define LP_UART_RX_TOUT_FLOW_DIS_S  1
/** LP_UART_RX_TOUT_THRHD : R/W; bitpos: [11:2]; default: 10;
 *  Configures the amount of time that the bus can remain idle before timeout.\\
 *  Measurement unit: bit time (the time to transmit 1 bit).
 */
#define LP_UART_RX_TOUT_THRHD    0x000003FFU
#define LP_UART_RX_TOUT_THRHD_M  (LP_UART_RX_TOUT_THRHD_V << LP_UART_RX_TOUT_THRHD_S)
#define LP_UART_RX_TOUT_THRHD_V  0x000003FFU
#define LP_UART_RX_TOUT_THRHD_S  2

/** LP_UART_MEM_TX_STATUS_REG register
 *  TX FIFO write and read offset address
 */
#define LP_UART_MEM_TX_STATUS_REG (DR_REG_LP_UART_BASE + 0x68)
/** LP_UART_TX_SRAM_WADDR : RO; bitpos: [7:3]; default: 0;
 *  Represents the offset address to write TX FIFO.
 */
#define LP_UART_TX_SRAM_WADDR    0x0000001FU
#define LP_UART_TX_SRAM_WADDR_M  (LP_UART_TX_SRAM_WADDR_V << LP_UART_TX_SRAM_WADDR_S)
#define LP_UART_TX_SRAM_WADDR_V  0x0000001FU
#define LP_UART_TX_SRAM_WADDR_S  3
/** LP_UART_TX_SRAM_RADDR : RO; bitpos: [16:12]; default: 0;
 *  Represents the offset address to read TX FIFO.
 */
#define LP_UART_TX_SRAM_RADDR    0x0000001FU
#define LP_UART_TX_SRAM_RADDR_M  (LP_UART_TX_SRAM_RADDR_V << LP_UART_TX_SRAM_RADDR_S)
#define LP_UART_TX_SRAM_RADDR_V  0x0000001FU
#define LP_UART_TX_SRAM_RADDR_S  12

/** LP_UART_MEM_RX_STATUS_REG register
 *  RX FIFO write and read offset address
 */
#define LP_UART_MEM_RX_STATUS_REG (DR_REG_LP_UART_BASE + 0x6c)
/** LP_UART_RX_SRAM_RADDR : RO; bitpos: [7:3]; default: 16;
 *  Represents the offset address to read RX FIFO.
 */
#define LP_UART_RX_SRAM_RADDR    0x0000001FU
#define LP_UART_RX_SRAM_RADDR_M  (LP_UART_RX_SRAM_RADDR_V << LP_UART_RX_SRAM_RADDR_S)
#define LP_UART_RX_SRAM_RADDR_V  0x0000001FU
#define LP_UART_RX_SRAM_RADDR_S  3
/** LP_UART_RX_SRAM_WADDR : RO; bitpos: [16:12]; default: 16;
 *  Represents the offset address to write RX FIFO.
 */
#define LP_UART_RX_SRAM_WADDR    0x0000001FU
#define LP_UART_RX_SRAM_WADDR_M  (LP_UART_RX_SRAM_WADDR_V << LP_UART_RX_SRAM_WADDR_S)
#define LP_UART_RX_SRAM_WADDR_V  0x0000001FU
#define LP_UART_RX_SRAM_WADDR_S  12

/** LP_UART_FSM_STATUS_REG register
 *  LP UART transmit and receive status
 */
#define LP_UART_FSM_STATUS_REG (DR_REG_LP_UART_BASE + 0x70)
/** LP_UART_ST_URX_OUT : RO; bitpos: [3:0]; default: 0;
 *  Represents the status of the receiver.
 */
#define LP_UART_ST_URX_OUT    0x0000000FU
#define LP_UART_ST_URX_OUT_M  (LP_UART_ST_URX_OUT_V << LP_UART_ST_URX_OUT_S)
#define LP_UART_ST_URX_OUT_V  0x0000000FU
#define LP_UART_ST_URX_OUT_S  0
/** LP_UART_ST_UTX_OUT : RO; bitpos: [7:4]; default: 0;
 *  Represents the status of the transmitter.
 */
#define LP_UART_ST_UTX_OUT    0x0000000FU
#define LP_UART_ST_UTX_OUT_M  (LP_UART_ST_UTX_OUT_V << LP_UART_ST_UTX_OUT_S)
#define LP_UART_ST_UTX_OUT_V  0x0000000FU
#define LP_UART_ST_UTX_OUT_S  4

/** LP_UART_CLK_CONF_REG register
 *  LP UART core clock configuration
 */
#define LP_UART_CLK_CONF_REG (DR_REG_LP_UART_BASE + 0x88)
/** LP_UART_TX_SCLK_EN : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to enable LP UART TX clock.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_TX_SCLK_EN    (BIT(24))
#define LP_UART_TX_SCLK_EN_M  (LP_UART_TX_SCLK_EN_V << LP_UART_TX_SCLK_EN_S)
#define LP_UART_TX_SCLK_EN_V  0x00000001U
#define LP_UART_TX_SCLK_EN_S  24
/** LP_UART_RX_SCLK_EN : R/W; bitpos: [25]; default: 1;
 *  Configures whether or not to enable LP UART RX clock.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_UART_RX_SCLK_EN    (BIT(25))
#define LP_UART_RX_SCLK_EN_M  (LP_UART_RX_SCLK_EN_V << LP_UART_RX_SCLK_EN_S)
#define LP_UART_RX_SCLK_EN_V  0x00000001U
#define LP_UART_RX_SCLK_EN_S  25
/** LP_UART_TX_RST_CORE : R/W; bitpos: [26]; default: 0;
 *  Write 1 and then write 0 to reset LP UART TX.
 */
#define LP_UART_TX_RST_CORE    (BIT(26))
#define LP_UART_TX_RST_CORE_M  (LP_UART_TX_RST_CORE_V << LP_UART_TX_RST_CORE_S)
#define LP_UART_TX_RST_CORE_V  0x00000001U
#define LP_UART_TX_RST_CORE_S  26
/** LP_UART_RX_RST_CORE : R/W; bitpos: [27]; default: 0;
 *  Write 1 and then write 0 to reset LP UART RX.
 */
#define LP_UART_RX_RST_CORE    (BIT(27))
#define LP_UART_RX_RST_CORE_M  (LP_UART_RX_RST_CORE_V << LP_UART_RX_RST_CORE_S)
#define LP_UART_RX_RST_CORE_V  0x00000001U
#define LP_UART_RX_RST_CORE_S  27

/** LP_UART_DATE_REG register
 *  LP UART version register
 */
#define LP_UART_DATE_REG (DR_REG_LP_UART_BASE + 0x8c)
/** LP_UART_DATE : R/W; bitpos: [31:0]; default: 36774432;
 *  Version control register.
 */
#define LP_UART_DATE    0xFFFFFFFFU
#define LP_UART_DATE_M  (LP_UART_DATE_V << LP_UART_DATE_S)
#define LP_UART_DATE_V  0xFFFFFFFFU
#define LP_UART_DATE_S  0

/** LP_UART_AFIFO_STATUS_REG register
 *  LP UART asynchronous FIFO Status
 */
#define LP_UART_AFIFO_STATUS_REG (DR_REG_LP_UART_BASE + 0x90)
/** LP_UART_TX_AFIFO_FULL : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the APB TX asynchronous FIFO is full.\\
 *  0: Not full\\
 *  1: Full\\
 */
#define LP_UART_TX_AFIFO_FULL    (BIT(0))
#define LP_UART_TX_AFIFO_FULL_M  (LP_UART_TX_AFIFO_FULL_V << LP_UART_TX_AFIFO_FULL_S)
#define LP_UART_TX_AFIFO_FULL_V  0x00000001U
#define LP_UART_TX_AFIFO_FULL_S  0
/** LP_UART_TX_AFIFO_EMPTY : RO; bitpos: [1]; default: 1;
 *  Represents whether or not the APB TX asynchronous FIFO is empty.\\
 *  0: Not empty\\
 *  1: Empty\\
 */
#define LP_UART_TX_AFIFO_EMPTY    (BIT(1))
#define LP_UART_TX_AFIFO_EMPTY_M  (LP_UART_TX_AFIFO_EMPTY_V << LP_UART_TX_AFIFO_EMPTY_S)
#define LP_UART_TX_AFIFO_EMPTY_V  0x00000001U
#define LP_UART_TX_AFIFO_EMPTY_S  1
/** LP_UART_RX_AFIFO_FULL : RO; bitpos: [2]; default: 0;
 *  Represents whether or not the APB RX asynchronous FIFO is full.\\
 *  0: Not full\\
 *  1: Full\\
 */
#define LP_UART_RX_AFIFO_FULL    (BIT(2))
#define LP_UART_RX_AFIFO_FULL_M  (LP_UART_RX_AFIFO_FULL_V << LP_UART_RX_AFIFO_FULL_S)
#define LP_UART_RX_AFIFO_FULL_V  0x00000001U
#define LP_UART_RX_AFIFO_FULL_S  2
/** LP_UART_RX_AFIFO_EMPTY : RO; bitpos: [3]; default: 1;
 *  Represents whether or not the APB RX asynchronous FIFO is empty.\\
 *  0: Not empty\\
 *  1: Empty\\
 */
#define LP_UART_RX_AFIFO_EMPTY    (BIT(3))
#define LP_UART_RX_AFIFO_EMPTY_M  (LP_UART_RX_AFIFO_EMPTY_V << LP_UART_RX_AFIFO_EMPTY_S)
#define LP_UART_RX_AFIFO_EMPTY_V  0x00000001U
#define LP_UART_RX_AFIFO_EMPTY_S  3

/** LP_UART_REG_UPDATE_REG register
 *  LP UART register configuration update register
 */
#define LP_UART_REG_UPDATE_REG (DR_REG_LP_UART_BASE + 0x98)
/** LP_UART_REG_UPDATE : R/W/SC; bitpos: [0]; default: 0;
 *  Configures whether or not to synchronize registers.\\
 *  0: Not synchronize\\
 *  1: Synchronize\\
 */
#define LP_UART_REG_UPDATE    (BIT(0))
#define LP_UART_REG_UPDATE_M  (LP_UART_REG_UPDATE_V << LP_UART_REG_UPDATE_S)
#define LP_UART_REG_UPDATE_V  0x00000001U
#define LP_UART_REG_UPDATE_S  0

/** LP_UART_ID_REG register
 *  LP UART ID register
 */
#define LP_UART_ID_REG (DR_REG_LP_UART_BASE + 0x9c)
/** LP_UART_ID : R/W; bitpos: [31:0]; default: 1280;
 *  Configures the LP UART ID.
 */
#define LP_UART_ID    0xFFFFFFFFU
#define LP_UART_ID_M  (LP_UART_ID_V << LP_UART_ID_S)
#define LP_UART_ID_V  0xFFFFFFFFU
#define LP_UART_ID_S  0

#ifdef __cplusplus
}
#endif
