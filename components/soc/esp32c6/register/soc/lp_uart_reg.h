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

/** LP_UART_FIFO_REG register
 *  FIFO data register
 */
#define LP_UART_FIFO_REG (DR_REG_LP_UART_BASE + 0x0)
/** LP_UART_RXFIFO_RD_BYTE : RO; bitpos: [7:0]; default: 0;
 *  UART $n accesses FIFO via this register.
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
 *  This interrupt raw bit turns to high level when receiver receives more data than
 *  what rxfifo_full_thrhd specifies.
 */
#define LP_UART_RXFIFO_FULL_INT_RAW    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_RAW_M  (LP_UART_RXFIFO_FULL_INT_RAW_V << LP_UART_RXFIFO_FULL_INT_RAW_S)
#define LP_UART_RXFIFO_FULL_INT_RAW_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_RAW_S  0
/** LP_UART_TXFIFO_EMPTY_INT_RAW : R/WTC/SS; bitpos: [1]; default: 1;
 *  This interrupt raw bit turns to high level when the amount of data in Tx-FIFO is
 *  less than what txfifo_empty_thrhd specifies .
 */
#define LP_UART_TXFIFO_EMPTY_INT_RAW    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_RAW_M  (LP_UART_TXFIFO_EMPTY_INT_RAW_V << LP_UART_TXFIFO_EMPTY_INT_RAW_S)
#define LP_UART_TXFIFO_EMPTY_INT_RAW_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_RAW_S  1
/** LP_UART_PARITY_ERR_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  This interrupt raw bit turns to high level when receiver detects a parity error in
 *  the data.
 */
#define LP_UART_PARITY_ERR_INT_RAW    (BIT(2))
#define LP_UART_PARITY_ERR_INT_RAW_M  (LP_UART_PARITY_ERR_INT_RAW_V << LP_UART_PARITY_ERR_INT_RAW_S)
#define LP_UART_PARITY_ERR_INT_RAW_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_RAW_S  2
/** LP_UART_FRM_ERR_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  This interrupt raw bit turns to high level when receiver detects a data frame error
 *  .
 */
#define LP_UART_FRM_ERR_INT_RAW    (BIT(3))
#define LP_UART_FRM_ERR_INT_RAW_M  (LP_UART_FRM_ERR_INT_RAW_V << LP_UART_FRM_ERR_INT_RAW_S)
#define LP_UART_FRM_ERR_INT_RAW_V  0x00000001U
#define LP_UART_FRM_ERR_INT_RAW_S  3
/** LP_UART_RXFIFO_OVF_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This interrupt raw bit turns to high level when receiver receives more data than
 *  the FIFO can store.
 */
#define LP_UART_RXFIFO_OVF_INT_RAW    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_RAW_M  (LP_UART_RXFIFO_OVF_INT_RAW_V << LP_UART_RXFIFO_OVF_INT_RAW_S)
#define LP_UART_RXFIFO_OVF_INT_RAW_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_RAW_S  4
/** LP_UART_DSR_CHG_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This interrupt raw bit turns to high level when receiver detects the edge change of
 *  DSRn signal.
 */
#define LP_UART_DSR_CHG_INT_RAW    (BIT(5))
#define LP_UART_DSR_CHG_INT_RAW_M  (LP_UART_DSR_CHG_INT_RAW_V << LP_UART_DSR_CHG_INT_RAW_S)
#define LP_UART_DSR_CHG_INT_RAW_V  0x00000001U
#define LP_UART_DSR_CHG_INT_RAW_S  5
/** LP_UART_CTS_CHG_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This interrupt raw bit turns to high level when receiver detects the edge change of
 *  CTSn signal.
 */
#define LP_UART_CTS_CHG_INT_RAW    (BIT(6))
#define LP_UART_CTS_CHG_INT_RAW_M  (LP_UART_CTS_CHG_INT_RAW_V << LP_UART_CTS_CHG_INT_RAW_S)
#define LP_UART_CTS_CHG_INT_RAW_V  0x00000001U
#define LP_UART_CTS_CHG_INT_RAW_S  6
/** LP_UART_BRK_DET_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This interrupt raw bit turns to high level when receiver detects a 0 after the stop
 *  bit.
 */
#define LP_UART_BRK_DET_INT_RAW    (BIT(7))
#define LP_UART_BRK_DET_INT_RAW_M  (LP_UART_BRK_DET_INT_RAW_V << LP_UART_BRK_DET_INT_RAW_S)
#define LP_UART_BRK_DET_INT_RAW_V  0x00000001U
#define LP_UART_BRK_DET_INT_RAW_S  7
/** LP_UART_RXFIFO_TOUT_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  This interrupt raw bit turns to high level when receiver takes more time than
 *  rx_tout_thrhd to receive a byte.
 */
#define LP_UART_RXFIFO_TOUT_INT_RAW    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_RAW_M  (LP_UART_RXFIFO_TOUT_INT_RAW_V << LP_UART_RXFIFO_TOUT_INT_RAW_S)
#define LP_UART_RXFIFO_TOUT_INT_RAW_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_RAW_S  8
/** LP_UART_SW_XON_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This interrupt raw bit turns to high level when receiver receives Xon char when
 *  uart_sw_flow_con_en is set to 1.
 */
#define LP_UART_SW_XON_INT_RAW    (BIT(9))
#define LP_UART_SW_XON_INT_RAW_M  (LP_UART_SW_XON_INT_RAW_V << LP_UART_SW_XON_INT_RAW_S)
#define LP_UART_SW_XON_INT_RAW_V  0x00000001U
#define LP_UART_SW_XON_INT_RAW_S  9
/** LP_UART_SW_XOFF_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  This interrupt raw bit turns to high level when receiver receives Xoff char when
 *  uart_sw_flow_con_en is set to 1.
 */
#define LP_UART_SW_XOFF_INT_RAW    (BIT(10))
#define LP_UART_SW_XOFF_INT_RAW_M  (LP_UART_SW_XOFF_INT_RAW_V << LP_UART_SW_XOFF_INT_RAW_S)
#define LP_UART_SW_XOFF_INT_RAW_V  0x00000001U
#define LP_UART_SW_XOFF_INT_RAW_S  10
/** LP_UART_GLITCH_DET_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  This interrupt raw bit turns to high level when receiver detects a glitch in the
 *  middle of a start bit.
 */
#define LP_UART_GLITCH_DET_INT_RAW    (BIT(11))
#define LP_UART_GLITCH_DET_INT_RAW_M  (LP_UART_GLITCH_DET_INT_RAW_V << LP_UART_GLITCH_DET_INT_RAW_S)
#define LP_UART_GLITCH_DET_INT_RAW_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_RAW_S  11
/** LP_UART_TX_BRK_DONE_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  This interrupt raw bit turns to high level when transmitter completes  sending
 *  NULL characters after all data in Tx-FIFO are sent.
 */
#define LP_UART_TX_BRK_DONE_INT_RAW    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_RAW_M  (LP_UART_TX_BRK_DONE_INT_RAW_V << LP_UART_TX_BRK_DONE_INT_RAW_S)
#define LP_UART_TX_BRK_DONE_INT_RAW_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_RAW_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  This interrupt raw bit turns to high level when transmitter has kept the shortest
 *  duration after sending the  last data.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW_M  (LP_UART_TX_BRK_IDLE_DONE_INT_RAW_V << LP_UART_TX_BRK_IDLE_DONE_INT_RAW_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_RAW_S  13
/** LP_UART_TX_DONE_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  This interrupt raw bit turns to high level when transmitter has send out all data
 *  in FIFO.
 */
#define LP_UART_TX_DONE_INT_RAW    (BIT(14))
#define LP_UART_TX_DONE_INT_RAW_M  (LP_UART_TX_DONE_INT_RAW_V << LP_UART_TX_DONE_INT_RAW_S)
#define LP_UART_TX_DONE_INT_RAW_V  0x00000001U
#define LP_UART_TX_DONE_INT_RAW_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  This interrupt raw bit turns to high level when receiver detects the configured
 *  at_cmd char.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW_M  (LP_UART_AT_CMD_CHAR_DET_INT_RAW_V << LP_UART_AT_CMD_CHAR_DET_INT_RAW_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_RAW_S  18
/** LP_UART_WAKEUP_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  This interrupt raw bit turns to high level when input rxd edge changes more times
 *  than what reg_active_threshold specifies in light sleeping mode.
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
 *  This is the status bit for rxfifo_full_int_raw when rxfifo_full_int_ena is set to 1.
 */
#define LP_UART_RXFIFO_FULL_INT_ST    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_ST_M  (LP_UART_RXFIFO_FULL_INT_ST_V << LP_UART_RXFIFO_FULL_INT_ST_S)
#define LP_UART_RXFIFO_FULL_INT_ST_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_ST_S  0
/** LP_UART_TXFIFO_EMPTY_INT_ST : RO; bitpos: [1]; default: 0;
 *  This is the status bit for  txfifo_empty_int_raw  when txfifo_empty_int_ena is set
 *  to 1.
 */
#define LP_UART_TXFIFO_EMPTY_INT_ST    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_ST_M  (LP_UART_TXFIFO_EMPTY_INT_ST_V << LP_UART_TXFIFO_EMPTY_INT_ST_S)
#define LP_UART_TXFIFO_EMPTY_INT_ST_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_ST_S  1
/** LP_UART_PARITY_ERR_INT_ST : RO; bitpos: [2]; default: 0;
 *  This is the status bit for parity_err_int_raw when parity_err_int_ena is set to 1.
 */
#define LP_UART_PARITY_ERR_INT_ST    (BIT(2))
#define LP_UART_PARITY_ERR_INT_ST_M  (LP_UART_PARITY_ERR_INT_ST_V << LP_UART_PARITY_ERR_INT_ST_S)
#define LP_UART_PARITY_ERR_INT_ST_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_ST_S  2
/** LP_UART_FRM_ERR_INT_ST : RO; bitpos: [3]; default: 0;
 *  This is the status bit for frm_err_int_raw when frm_err_int_ena is set to 1.
 */
#define LP_UART_FRM_ERR_INT_ST    (BIT(3))
#define LP_UART_FRM_ERR_INT_ST_M  (LP_UART_FRM_ERR_INT_ST_V << LP_UART_FRM_ERR_INT_ST_S)
#define LP_UART_FRM_ERR_INT_ST_V  0x00000001U
#define LP_UART_FRM_ERR_INT_ST_S  3
/** LP_UART_RXFIFO_OVF_INT_ST : RO; bitpos: [4]; default: 0;
 *  This is the status bit for rxfifo_ovf_int_raw when rxfifo_ovf_int_ena is set to 1.
 */
#define LP_UART_RXFIFO_OVF_INT_ST    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_ST_M  (LP_UART_RXFIFO_OVF_INT_ST_V << LP_UART_RXFIFO_OVF_INT_ST_S)
#define LP_UART_RXFIFO_OVF_INT_ST_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_ST_S  4
/** LP_UART_DSR_CHG_INT_ST : RO; bitpos: [5]; default: 0;
 *  This is the status bit for dsr_chg_int_raw when dsr_chg_int_ena is set to 1.
 */
#define LP_UART_DSR_CHG_INT_ST    (BIT(5))
#define LP_UART_DSR_CHG_INT_ST_M  (LP_UART_DSR_CHG_INT_ST_V << LP_UART_DSR_CHG_INT_ST_S)
#define LP_UART_DSR_CHG_INT_ST_V  0x00000001U
#define LP_UART_DSR_CHG_INT_ST_S  5
/** LP_UART_CTS_CHG_INT_ST : RO; bitpos: [6]; default: 0;
 *  This is the status bit for cts_chg_int_raw when cts_chg_int_ena is set to 1.
 */
#define LP_UART_CTS_CHG_INT_ST    (BIT(6))
#define LP_UART_CTS_CHG_INT_ST_M  (LP_UART_CTS_CHG_INT_ST_V << LP_UART_CTS_CHG_INT_ST_S)
#define LP_UART_CTS_CHG_INT_ST_V  0x00000001U
#define LP_UART_CTS_CHG_INT_ST_S  6
/** LP_UART_BRK_DET_INT_ST : RO; bitpos: [7]; default: 0;
 *  This is the status bit for brk_det_int_raw when brk_det_int_ena is set to 1.
 */
#define LP_UART_BRK_DET_INT_ST    (BIT(7))
#define LP_UART_BRK_DET_INT_ST_M  (LP_UART_BRK_DET_INT_ST_V << LP_UART_BRK_DET_INT_ST_S)
#define LP_UART_BRK_DET_INT_ST_V  0x00000001U
#define LP_UART_BRK_DET_INT_ST_S  7
/** LP_UART_RXFIFO_TOUT_INT_ST : RO; bitpos: [8]; default: 0;
 *  This is the status bit for rxfifo_tout_int_raw when rxfifo_tout_int_ena is set to 1.
 */
#define LP_UART_RXFIFO_TOUT_INT_ST    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_ST_M  (LP_UART_RXFIFO_TOUT_INT_ST_V << LP_UART_RXFIFO_TOUT_INT_ST_S)
#define LP_UART_RXFIFO_TOUT_INT_ST_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_ST_S  8
/** LP_UART_SW_XON_INT_ST : RO; bitpos: [9]; default: 0;
 *  This is the status bit for sw_xon_int_raw when sw_xon_int_ena is set to 1.
 */
#define LP_UART_SW_XON_INT_ST    (BIT(9))
#define LP_UART_SW_XON_INT_ST_M  (LP_UART_SW_XON_INT_ST_V << LP_UART_SW_XON_INT_ST_S)
#define LP_UART_SW_XON_INT_ST_V  0x00000001U
#define LP_UART_SW_XON_INT_ST_S  9
/** LP_UART_SW_XOFF_INT_ST : RO; bitpos: [10]; default: 0;
 *  This is the status bit for sw_xoff_int_raw when sw_xoff_int_ena is set to 1.
 */
#define LP_UART_SW_XOFF_INT_ST    (BIT(10))
#define LP_UART_SW_XOFF_INT_ST_M  (LP_UART_SW_XOFF_INT_ST_V << LP_UART_SW_XOFF_INT_ST_S)
#define LP_UART_SW_XOFF_INT_ST_V  0x00000001U
#define LP_UART_SW_XOFF_INT_ST_S  10
/** LP_UART_GLITCH_DET_INT_ST : RO; bitpos: [11]; default: 0;
 *  This is the status bit for glitch_det_int_raw when glitch_det_int_ena is set to 1.
 */
#define LP_UART_GLITCH_DET_INT_ST    (BIT(11))
#define LP_UART_GLITCH_DET_INT_ST_M  (LP_UART_GLITCH_DET_INT_ST_V << LP_UART_GLITCH_DET_INT_ST_S)
#define LP_UART_GLITCH_DET_INT_ST_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_ST_S  11
/** LP_UART_TX_BRK_DONE_INT_ST : RO; bitpos: [12]; default: 0;
 *  This is the status bit for tx_brk_done_int_raw when tx_brk_done_int_ena is set to 1.
 */
#define LP_UART_TX_BRK_DONE_INT_ST    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_ST_M  (LP_UART_TX_BRK_DONE_INT_ST_V << LP_UART_TX_BRK_DONE_INT_ST_S)
#define LP_UART_TX_BRK_DONE_INT_ST_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_ST_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_ST : RO; bitpos: [13]; default: 0;
 *  This is the status bit for tx_brk_idle_done_int_raw when tx_brk_idle_done_int_ena
 *  is set to 1.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST_M  (LP_UART_TX_BRK_IDLE_DONE_INT_ST_V << LP_UART_TX_BRK_IDLE_DONE_INT_ST_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_ST_S  13
/** LP_UART_TX_DONE_INT_ST : RO; bitpos: [14]; default: 0;
 *  This is the status bit for tx_done_int_raw when tx_done_int_ena is set to 1.
 */
#define LP_UART_TX_DONE_INT_ST    (BIT(14))
#define LP_UART_TX_DONE_INT_ST_M  (LP_UART_TX_DONE_INT_ST_V << LP_UART_TX_DONE_INT_ST_S)
#define LP_UART_TX_DONE_INT_ST_V  0x00000001U
#define LP_UART_TX_DONE_INT_ST_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_ST : RO; bitpos: [18]; default: 0;
 *  This is the status bit for at_cmd_det_int_raw when at_cmd_char_det_int_ena is set
 *  to 1.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_ST    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_ST_M  (LP_UART_AT_CMD_CHAR_DET_INT_ST_V << LP_UART_AT_CMD_CHAR_DET_INT_ST_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_ST_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_ST_S  18
/** LP_UART_WAKEUP_INT_ST : RO; bitpos: [19]; default: 0;
 *  This is the status bit for uart_wakeup_int_raw when uart_wakeup_int_ena is set to 1.
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
 *  This is the enable bit for rxfifo_full_int_st register.
 */
#define LP_UART_RXFIFO_FULL_INT_ENA    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_ENA_M  (LP_UART_RXFIFO_FULL_INT_ENA_V << LP_UART_RXFIFO_FULL_INT_ENA_S)
#define LP_UART_RXFIFO_FULL_INT_ENA_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_ENA_S  0
/** LP_UART_TXFIFO_EMPTY_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  This is the enable bit for txfifo_empty_int_st register.
 */
#define LP_UART_TXFIFO_EMPTY_INT_ENA    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_ENA_M  (LP_UART_TXFIFO_EMPTY_INT_ENA_V << LP_UART_TXFIFO_EMPTY_INT_ENA_S)
#define LP_UART_TXFIFO_EMPTY_INT_ENA_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_ENA_S  1
/** LP_UART_PARITY_ERR_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  This is the enable bit for parity_err_int_st register.
 */
#define LP_UART_PARITY_ERR_INT_ENA    (BIT(2))
#define LP_UART_PARITY_ERR_INT_ENA_M  (LP_UART_PARITY_ERR_INT_ENA_V << LP_UART_PARITY_ERR_INT_ENA_S)
#define LP_UART_PARITY_ERR_INT_ENA_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_ENA_S  2
/** LP_UART_FRM_ERR_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  This is the enable bit for frm_err_int_st register.
 */
#define LP_UART_FRM_ERR_INT_ENA    (BIT(3))
#define LP_UART_FRM_ERR_INT_ENA_M  (LP_UART_FRM_ERR_INT_ENA_V << LP_UART_FRM_ERR_INT_ENA_S)
#define LP_UART_FRM_ERR_INT_ENA_V  0x00000001U
#define LP_UART_FRM_ERR_INT_ENA_S  3
/** LP_UART_RXFIFO_OVF_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  This is the enable bit for rxfifo_ovf_int_st register.
 */
#define LP_UART_RXFIFO_OVF_INT_ENA    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_ENA_M  (LP_UART_RXFIFO_OVF_INT_ENA_V << LP_UART_RXFIFO_OVF_INT_ENA_S)
#define LP_UART_RXFIFO_OVF_INT_ENA_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_ENA_S  4
/** LP_UART_DSR_CHG_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  This is the enable bit for dsr_chg_int_st register.
 */
#define LP_UART_DSR_CHG_INT_ENA    (BIT(5))
#define LP_UART_DSR_CHG_INT_ENA_M  (LP_UART_DSR_CHG_INT_ENA_V << LP_UART_DSR_CHG_INT_ENA_S)
#define LP_UART_DSR_CHG_INT_ENA_V  0x00000001U
#define LP_UART_DSR_CHG_INT_ENA_S  5
/** LP_UART_CTS_CHG_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  This is the enable bit for cts_chg_int_st register.
 */
#define LP_UART_CTS_CHG_INT_ENA    (BIT(6))
#define LP_UART_CTS_CHG_INT_ENA_M  (LP_UART_CTS_CHG_INT_ENA_V << LP_UART_CTS_CHG_INT_ENA_S)
#define LP_UART_CTS_CHG_INT_ENA_V  0x00000001U
#define LP_UART_CTS_CHG_INT_ENA_S  6
/** LP_UART_BRK_DET_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  This is the enable bit for brk_det_int_st register.
 */
#define LP_UART_BRK_DET_INT_ENA    (BIT(7))
#define LP_UART_BRK_DET_INT_ENA_M  (LP_UART_BRK_DET_INT_ENA_V << LP_UART_BRK_DET_INT_ENA_S)
#define LP_UART_BRK_DET_INT_ENA_V  0x00000001U
#define LP_UART_BRK_DET_INT_ENA_S  7
/** LP_UART_RXFIFO_TOUT_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  This is the enable bit for rxfifo_tout_int_st register.
 */
#define LP_UART_RXFIFO_TOUT_INT_ENA    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_ENA_M  (LP_UART_RXFIFO_TOUT_INT_ENA_V << LP_UART_RXFIFO_TOUT_INT_ENA_S)
#define LP_UART_RXFIFO_TOUT_INT_ENA_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_ENA_S  8
/** LP_UART_SW_XON_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  This is the enable bit for sw_xon_int_st register.
 */
#define LP_UART_SW_XON_INT_ENA    (BIT(9))
#define LP_UART_SW_XON_INT_ENA_M  (LP_UART_SW_XON_INT_ENA_V << LP_UART_SW_XON_INT_ENA_S)
#define LP_UART_SW_XON_INT_ENA_V  0x00000001U
#define LP_UART_SW_XON_INT_ENA_S  9
/** LP_UART_SW_XOFF_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  This is the enable bit for sw_xoff_int_st register.
 */
#define LP_UART_SW_XOFF_INT_ENA    (BIT(10))
#define LP_UART_SW_XOFF_INT_ENA_M  (LP_UART_SW_XOFF_INT_ENA_V << LP_UART_SW_XOFF_INT_ENA_S)
#define LP_UART_SW_XOFF_INT_ENA_V  0x00000001U
#define LP_UART_SW_XOFF_INT_ENA_S  10
/** LP_UART_GLITCH_DET_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  This is the enable bit for glitch_det_int_st register.
 */
#define LP_UART_GLITCH_DET_INT_ENA    (BIT(11))
#define LP_UART_GLITCH_DET_INT_ENA_M  (LP_UART_GLITCH_DET_INT_ENA_V << LP_UART_GLITCH_DET_INT_ENA_S)
#define LP_UART_GLITCH_DET_INT_ENA_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_ENA_S  11
/** LP_UART_TX_BRK_DONE_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  This is the enable bit for tx_brk_done_int_st register.
 */
#define LP_UART_TX_BRK_DONE_INT_ENA    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_ENA_M  (LP_UART_TX_BRK_DONE_INT_ENA_V << LP_UART_TX_BRK_DONE_INT_ENA_S)
#define LP_UART_TX_BRK_DONE_INT_ENA_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_ENA_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  This is the enable bit for tx_brk_idle_done_int_st register.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA_M  (LP_UART_TX_BRK_IDLE_DONE_INT_ENA_V << LP_UART_TX_BRK_IDLE_DONE_INT_ENA_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_ENA_S  13
/** LP_UART_TX_DONE_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  This is the enable bit for tx_done_int_st register.
 */
#define LP_UART_TX_DONE_INT_ENA    (BIT(14))
#define LP_UART_TX_DONE_INT_ENA_M  (LP_UART_TX_DONE_INT_ENA_V << LP_UART_TX_DONE_INT_ENA_S)
#define LP_UART_TX_DONE_INT_ENA_V  0x00000001U
#define LP_UART_TX_DONE_INT_ENA_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  This is the enable bit for at_cmd_char_det_int_st register.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA_M  (LP_UART_AT_CMD_CHAR_DET_INT_ENA_V << LP_UART_AT_CMD_CHAR_DET_INT_ENA_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_ENA_S  18
/** LP_UART_WAKEUP_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  This is the enable bit for uart_wakeup_int_st register.
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
 *  Set this bit to clear the rxfifo_full_int_raw interrupt.
 */
#define LP_UART_RXFIFO_FULL_INT_CLR    (BIT(0))
#define LP_UART_RXFIFO_FULL_INT_CLR_M  (LP_UART_RXFIFO_FULL_INT_CLR_V << LP_UART_RXFIFO_FULL_INT_CLR_S)
#define LP_UART_RXFIFO_FULL_INT_CLR_V  0x00000001U
#define LP_UART_RXFIFO_FULL_INT_CLR_S  0
/** LP_UART_TXFIFO_EMPTY_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear txfifo_empty_int_raw interrupt.
 */
#define LP_UART_TXFIFO_EMPTY_INT_CLR    (BIT(1))
#define LP_UART_TXFIFO_EMPTY_INT_CLR_M  (LP_UART_TXFIFO_EMPTY_INT_CLR_V << LP_UART_TXFIFO_EMPTY_INT_CLR_S)
#define LP_UART_TXFIFO_EMPTY_INT_CLR_V  0x00000001U
#define LP_UART_TXFIFO_EMPTY_INT_CLR_S  1
/** LP_UART_PARITY_ERR_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear parity_err_int_raw interrupt.
 */
#define LP_UART_PARITY_ERR_INT_CLR    (BIT(2))
#define LP_UART_PARITY_ERR_INT_CLR_M  (LP_UART_PARITY_ERR_INT_CLR_V << LP_UART_PARITY_ERR_INT_CLR_S)
#define LP_UART_PARITY_ERR_INT_CLR_V  0x00000001U
#define LP_UART_PARITY_ERR_INT_CLR_S  2
/** LP_UART_FRM_ERR_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear frm_err_int_raw interrupt.
 */
#define LP_UART_FRM_ERR_INT_CLR    (BIT(3))
#define LP_UART_FRM_ERR_INT_CLR_M  (LP_UART_FRM_ERR_INT_CLR_V << LP_UART_FRM_ERR_INT_CLR_S)
#define LP_UART_FRM_ERR_INT_CLR_V  0x00000001U
#define LP_UART_FRM_ERR_INT_CLR_S  3
/** LP_UART_RXFIFO_OVF_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear rxfifo_ovf_int_raw interrupt.
 */
#define LP_UART_RXFIFO_OVF_INT_CLR    (BIT(4))
#define LP_UART_RXFIFO_OVF_INT_CLR_M  (LP_UART_RXFIFO_OVF_INT_CLR_V << LP_UART_RXFIFO_OVF_INT_CLR_S)
#define LP_UART_RXFIFO_OVF_INT_CLR_V  0x00000001U
#define LP_UART_RXFIFO_OVF_INT_CLR_S  4
/** LP_UART_DSR_CHG_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the dsr_chg_int_raw interrupt.
 */
#define LP_UART_DSR_CHG_INT_CLR    (BIT(5))
#define LP_UART_DSR_CHG_INT_CLR_M  (LP_UART_DSR_CHG_INT_CLR_V << LP_UART_DSR_CHG_INT_CLR_S)
#define LP_UART_DSR_CHG_INT_CLR_V  0x00000001U
#define LP_UART_DSR_CHG_INT_CLR_S  5
/** LP_UART_CTS_CHG_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the cts_chg_int_raw interrupt.
 */
#define LP_UART_CTS_CHG_INT_CLR    (BIT(6))
#define LP_UART_CTS_CHG_INT_CLR_M  (LP_UART_CTS_CHG_INT_CLR_V << LP_UART_CTS_CHG_INT_CLR_S)
#define LP_UART_CTS_CHG_INT_CLR_V  0x00000001U
#define LP_UART_CTS_CHG_INT_CLR_S  6
/** LP_UART_BRK_DET_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the brk_det_int_raw interrupt.
 */
#define LP_UART_BRK_DET_INT_CLR    (BIT(7))
#define LP_UART_BRK_DET_INT_CLR_M  (LP_UART_BRK_DET_INT_CLR_V << LP_UART_BRK_DET_INT_CLR_S)
#define LP_UART_BRK_DET_INT_CLR_V  0x00000001U
#define LP_UART_BRK_DET_INT_CLR_S  7
/** LP_UART_RXFIFO_TOUT_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the rxfifo_tout_int_raw interrupt.
 */
#define LP_UART_RXFIFO_TOUT_INT_CLR    (BIT(8))
#define LP_UART_RXFIFO_TOUT_INT_CLR_M  (LP_UART_RXFIFO_TOUT_INT_CLR_V << LP_UART_RXFIFO_TOUT_INT_CLR_S)
#define LP_UART_RXFIFO_TOUT_INT_CLR_V  0x00000001U
#define LP_UART_RXFIFO_TOUT_INT_CLR_S  8
/** LP_UART_SW_XON_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the sw_xon_int_raw interrupt.
 */
#define LP_UART_SW_XON_INT_CLR    (BIT(9))
#define LP_UART_SW_XON_INT_CLR_M  (LP_UART_SW_XON_INT_CLR_V << LP_UART_SW_XON_INT_CLR_S)
#define LP_UART_SW_XON_INT_CLR_V  0x00000001U
#define LP_UART_SW_XON_INT_CLR_S  9
/** LP_UART_SW_XOFF_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the sw_xoff_int_raw interrupt.
 */
#define LP_UART_SW_XOFF_INT_CLR    (BIT(10))
#define LP_UART_SW_XOFF_INT_CLR_M  (LP_UART_SW_XOFF_INT_CLR_V << LP_UART_SW_XOFF_INT_CLR_S)
#define LP_UART_SW_XOFF_INT_CLR_V  0x00000001U
#define LP_UART_SW_XOFF_INT_CLR_S  10
/** LP_UART_GLITCH_DET_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the glitch_det_int_raw interrupt.
 */
#define LP_UART_GLITCH_DET_INT_CLR    (BIT(11))
#define LP_UART_GLITCH_DET_INT_CLR_M  (LP_UART_GLITCH_DET_INT_CLR_V << LP_UART_GLITCH_DET_INT_CLR_S)
#define LP_UART_GLITCH_DET_INT_CLR_V  0x00000001U
#define LP_UART_GLITCH_DET_INT_CLR_S  11
/** LP_UART_TX_BRK_DONE_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the tx_brk_done_int_raw interrupt..
 */
#define LP_UART_TX_BRK_DONE_INT_CLR    (BIT(12))
#define LP_UART_TX_BRK_DONE_INT_CLR_M  (LP_UART_TX_BRK_DONE_INT_CLR_V << LP_UART_TX_BRK_DONE_INT_CLR_S)
#define LP_UART_TX_BRK_DONE_INT_CLR_V  0x00000001U
#define LP_UART_TX_BRK_DONE_INT_CLR_S  12
/** LP_UART_TX_BRK_IDLE_DONE_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Set this bit to clear the tx_brk_idle_done_int_raw interrupt.
 */
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR    (BIT(13))
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR_M  (LP_UART_TX_BRK_IDLE_DONE_INT_CLR_V << LP_UART_TX_BRK_IDLE_DONE_INT_CLR_S)
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR_V  0x00000001U
#define LP_UART_TX_BRK_IDLE_DONE_INT_CLR_S  13
/** LP_UART_TX_DONE_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Set this bit to clear the tx_done_int_raw interrupt.
 */
#define LP_UART_TX_DONE_INT_CLR    (BIT(14))
#define LP_UART_TX_DONE_INT_CLR_M  (LP_UART_TX_DONE_INT_CLR_V << LP_UART_TX_DONE_INT_CLR_S)
#define LP_UART_TX_DONE_INT_CLR_V  0x00000001U
#define LP_UART_TX_DONE_INT_CLR_S  14
/** LP_UART_AT_CMD_CHAR_DET_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Set this bit to clear the at_cmd_char_det_int_raw interrupt.
 */
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR    (BIT(18))
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR_M  (LP_UART_AT_CMD_CHAR_DET_INT_CLR_V << LP_UART_AT_CMD_CHAR_DET_INT_CLR_S)
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR_V  0x00000001U
#define LP_UART_AT_CMD_CHAR_DET_INT_CLR_S  18
/** LP_UART_WAKEUP_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Set this bit to clear the uart_wakeup_int_raw interrupt.
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
 *  The integral part of the frequency divider factor.
 */
#define LP_UART_CLKDIV    0x00000FFFU
#define LP_UART_CLKDIV_M  (LP_UART_CLKDIV_V << LP_UART_CLKDIV_S)
#define LP_UART_CLKDIV_V  0x00000FFFU
#define LP_UART_CLKDIV_S  0
/** LP_UART_CLKDIV_FRAG : R/W; bitpos: [23:20]; default: 0;
 *  The decimal part of the frequency divider factor.
 */
#define LP_UART_CLKDIV_FRAG    0x0000000FU
#define LP_UART_CLKDIV_FRAG_M  (LP_UART_CLKDIV_FRAG_V << LP_UART_CLKDIV_FRAG_S)
#define LP_UART_CLKDIV_FRAG_V  0x0000000FU
#define LP_UART_CLKDIV_FRAG_S  20

/** LP_UART_RX_FILT_REG register
 *  Rx Filter configuration
 */
#define LP_UART_RX_FILT_REG (DR_REG_LP_UART_BASE + 0x18)
/** LP_UART_GLITCH_FILT : R/W; bitpos: [7:0]; default: 8;
 *  when input pulse width is lower than this value the pulse is ignored.
 */
#define LP_UART_GLITCH_FILT    0x000000FFU
#define LP_UART_GLITCH_FILT_M  (LP_UART_GLITCH_FILT_V << LP_UART_GLITCH_FILT_S)
#define LP_UART_GLITCH_FILT_V  0x000000FFU
#define LP_UART_GLITCH_FILT_S  0
/** LP_UART_GLITCH_FILT_EN : R/W; bitpos: [8]; default: 0;
 *  Set this bit to enable Rx signal filter.
 */
#define LP_UART_GLITCH_FILT_EN    (BIT(8))
#define LP_UART_GLITCH_FILT_EN_M  (LP_UART_GLITCH_FILT_EN_V << LP_UART_GLITCH_FILT_EN_S)
#define LP_UART_GLITCH_FILT_EN_V  0x00000001U
#define LP_UART_GLITCH_FILT_EN_S  8

/** LP_UART_STATUS_REG register
 *  UART status register
 */
#define LP_UART_STATUS_REG (DR_REG_LP_UART_BASE + 0x1c)
/** LP_UART_RXFIFO_CNT : RO; bitpos: [7:3]; default: 0;
 *  Stores the byte number of valid data in Rx-FIFO.
 */
#define LP_UART_RXFIFO_CNT    0x0000001FU
#define LP_UART_RXFIFO_CNT_M  (LP_UART_RXFIFO_CNT_V << LP_UART_RXFIFO_CNT_S)
#define LP_UART_RXFIFO_CNT_V  0x0000001FU
#define LP_UART_RXFIFO_CNT_S  3
/** LP_UART_DSRN : RO; bitpos: [13]; default: 0;
 *  The register represent the level value of the internal uart dsr signal.
 */
#define LP_UART_DSRN    (BIT(13))
#define LP_UART_DSRN_M  (LP_UART_DSRN_V << LP_UART_DSRN_S)
#define LP_UART_DSRN_V  0x00000001U
#define LP_UART_DSRN_S  13
/** LP_UART_CTSN : RO; bitpos: [14]; default: 1;
 *  This register represent the level value of the internal uart cts signal.
 */
#define LP_UART_CTSN    (BIT(14))
#define LP_UART_CTSN_M  (LP_UART_CTSN_V << LP_UART_CTSN_S)
#define LP_UART_CTSN_V  0x00000001U
#define LP_UART_CTSN_S  14
/** LP_UART_RXD : RO; bitpos: [15]; default: 1;
 *  This register represent the  level value of the internal uart rxd signal.
 */
#define LP_UART_RXD    (BIT(15))
#define LP_UART_RXD_M  (LP_UART_RXD_V << LP_UART_RXD_S)
#define LP_UART_RXD_V  0x00000001U
#define LP_UART_RXD_S  15
/** LP_UART_TXFIFO_CNT : RO; bitpos: [23:19]; default: 0;
 *  Stores the byte number of data in Tx-FIFO.
 */
#define LP_UART_TXFIFO_CNT    0x0000001FU
#define LP_UART_TXFIFO_CNT_M  (LP_UART_TXFIFO_CNT_V << LP_UART_TXFIFO_CNT_S)
#define LP_UART_TXFIFO_CNT_V  0x0000001FU
#define LP_UART_TXFIFO_CNT_S  19
/** LP_UART_DTRN : RO; bitpos: [29]; default: 1;
 *  This bit represents the level of the internal uart dtr signal.
 */
#define LP_UART_DTRN    (BIT(29))
#define LP_UART_DTRN_M  (LP_UART_DTRN_V << LP_UART_DTRN_S)
#define LP_UART_DTRN_V  0x00000001U
#define LP_UART_DTRN_S  29
/** LP_UART_RTSN : RO; bitpos: [30]; default: 1;
 *  This bit represents the level of the internal uart rts signal.
 */
#define LP_UART_RTSN    (BIT(30))
#define LP_UART_RTSN_M  (LP_UART_RTSN_V << LP_UART_RTSN_S)
#define LP_UART_RTSN_V  0x00000001U
#define LP_UART_RTSN_S  30
/** LP_UART_TXD : RO; bitpos: [31]; default: 1;
 *  This bit represents the  level of the internal uart txd signal.
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
 *  This register is used to configure the parity check mode.
 */
#define LP_UART_PARITY    (BIT(0))
#define LP_UART_PARITY_M  (LP_UART_PARITY_V << LP_UART_PARITY_S)
#define LP_UART_PARITY_V  0x00000001U
#define LP_UART_PARITY_S  0
/** LP_UART_PARITY_EN : R/W; bitpos: [1]; default: 0;
 *  Set this bit to enable uart parity check.
 */
#define LP_UART_PARITY_EN    (BIT(1))
#define LP_UART_PARITY_EN_M  (LP_UART_PARITY_EN_V << LP_UART_PARITY_EN_S)
#define LP_UART_PARITY_EN_V  0x00000001U
#define LP_UART_PARITY_EN_S  1
/** LP_UART_BIT_NUM : R/W; bitpos: [3:2]; default: 3;
 *  This register is used to set the length of data.
 */
#define LP_UART_BIT_NUM    0x00000003U
#define LP_UART_BIT_NUM_M  (LP_UART_BIT_NUM_V << LP_UART_BIT_NUM_S)
#define LP_UART_BIT_NUM_V  0x00000003U
#define LP_UART_BIT_NUM_S  2
/** LP_UART_STOP_BIT_NUM : R/W; bitpos: [5:4]; default: 1;
 *  This register is used to set the length of  stop bit.
 */
#define LP_UART_STOP_BIT_NUM    0x00000003U
#define LP_UART_STOP_BIT_NUM_M  (LP_UART_STOP_BIT_NUM_V << LP_UART_STOP_BIT_NUM_S)
#define LP_UART_STOP_BIT_NUM_V  0x00000003U
#define LP_UART_STOP_BIT_NUM_S  4
/** LP_UART_TXD_BRK : R/W; bitpos: [6]; default: 0;
 *  Set this bit to enable transmitter to  send NULL when the process of sending data
 *  is done.
 */
#define LP_UART_TXD_BRK    (BIT(6))
#define LP_UART_TXD_BRK_M  (LP_UART_TXD_BRK_V << LP_UART_TXD_BRK_S)
#define LP_UART_TXD_BRK_V  0x00000001U
#define LP_UART_TXD_BRK_S  6
/** LP_UART_LOOPBACK : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable uart loopback test mode.
 */
#define LP_UART_LOOPBACK    (BIT(12))
#define LP_UART_LOOPBACK_M  (LP_UART_LOOPBACK_V << LP_UART_LOOPBACK_S)
#define LP_UART_LOOPBACK_V  0x00000001U
#define LP_UART_LOOPBACK_S  12
/** LP_UART_TX_FLOW_EN : R/W; bitpos: [13]; default: 0;
 *  Set this bit to enable flow control function for transmitter.
 */
#define LP_UART_TX_FLOW_EN    (BIT(13))
#define LP_UART_TX_FLOW_EN_M  (LP_UART_TX_FLOW_EN_V << LP_UART_TX_FLOW_EN_S)
#define LP_UART_TX_FLOW_EN_V  0x00000001U
#define LP_UART_TX_FLOW_EN_S  13
/** LP_UART_RXD_INV : R/W; bitpos: [15]; default: 0;
 *  Set this bit to inverse the level value of uart rxd signal.
 */
#define LP_UART_RXD_INV    (BIT(15))
#define LP_UART_RXD_INV_M  (LP_UART_RXD_INV_V << LP_UART_RXD_INV_S)
#define LP_UART_RXD_INV_V  0x00000001U
#define LP_UART_RXD_INV_S  15
/** LP_UART_TXD_INV : R/W; bitpos: [16]; default: 0;
 *  Set this bit to inverse the level value of uart txd signal.
 */
#define LP_UART_TXD_INV    (BIT(16))
#define LP_UART_TXD_INV_M  (LP_UART_TXD_INV_V << LP_UART_TXD_INV_S)
#define LP_UART_TXD_INV_V  0x00000001U
#define LP_UART_TXD_INV_S  16
/** LP_UART_DIS_RX_DAT_OVF : R/W; bitpos: [17]; default: 0;
 *  Disable UART Rx data overflow detect.
 */
#define LP_UART_DIS_RX_DAT_OVF    (BIT(17))
#define LP_UART_DIS_RX_DAT_OVF_M  (LP_UART_DIS_RX_DAT_OVF_V << LP_UART_DIS_RX_DAT_OVF_S)
#define LP_UART_DIS_RX_DAT_OVF_V  0x00000001U
#define LP_UART_DIS_RX_DAT_OVF_S  17
/** LP_UART_ERR_WR_MASK : R/W; bitpos: [18]; default: 0;
 *  1'h1: Receiver stops storing data into FIFO when data is wrong. 1'h0: Receiver
 *  stores the data even if the  received data is wrong.
 */
#define LP_UART_ERR_WR_MASK    (BIT(18))
#define LP_UART_ERR_WR_MASK_M  (LP_UART_ERR_WR_MASK_V << LP_UART_ERR_WR_MASK_S)
#define LP_UART_ERR_WR_MASK_V  0x00000001U
#define LP_UART_ERR_WR_MASK_S  18
/** LP_UART_MEM_CLK_EN : R/W; bitpos: [20]; default: 1;
 *  UART memory clock gate enable signal.
 */
#define LP_UART_MEM_CLK_EN    (BIT(20))
#define LP_UART_MEM_CLK_EN_M  (LP_UART_MEM_CLK_EN_V << LP_UART_MEM_CLK_EN_S)
#define LP_UART_MEM_CLK_EN_V  0x00000001U
#define LP_UART_MEM_CLK_EN_S  20
/** LP_UART_SW_RTS : R/W; bitpos: [21]; default: 0;
 *  This register is used to configure the software rts signal which is used in
 *  software flow control.
 */
#define LP_UART_SW_RTS    (BIT(21))
#define LP_UART_SW_RTS_M  (LP_UART_SW_RTS_V << LP_UART_SW_RTS_S)
#define LP_UART_SW_RTS_V  0x00000001U
#define LP_UART_SW_RTS_S  21
/** LP_UART_RXFIFO_RST : R/W; bitpos: [22]; default: 0;
 *  Set this bit to reset the uart receive-FIFO.
 */
#define LP_UART_RXFIFO_RST    (BIT(22))
#define LP_UART_RXFIFO_RST_M  (LP_UART_RXFIFO_RST_V << LP_UART_RXFIFO_RST_S)
#define LP_UART_RXFIFO_RST_V  0x00000001U
#define LP_UART_RXFIFO_RST_S  22
/** LP_UART_TXFIFO_RST : R/W; bitpos: [23]; default: 0;
 *  Set this bit to reset the uart transmit-FIFO.
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
 *  It will produce rxfifo_full_int interrupt when receiver receives more data than
 *  this register value.
 */
#define LP_UART_RXFIFO_FULL_THRHD    0x0000001FU
#define LP_UART_RXFIFO_FULL_THRHD_M  (LP_UART_RXFIFO_FULL_THRHD_V << LP_UART_RXFIFO_FULL_THRHD_S)
#define LP_UART_RXFIFO_FULL_THRHD_V  0x0000001FU
#define LP_UART_RXFIFO_FULL_THRHD_S  3
/** LP_UART_TXFIFO_EMPTY_THRHD : R/W; bitpos: [15:11]; default: 12;
 *  It will produce txfifo_empty_int interrupt when the data amount in Tx-FIFO is less
 *  than this register value.
 */
#define LP_UART_TXFIFO_EMPTY_THRHD    0x0000001FU
#define LP_UART_TXFIFO_EMPTY_THRHD_M  (LP_UART_TXFIFO_EMPTY_THRHD_V << LP_UART_TXFIFO_EMPTY_THRHD_S)
#define LP_UART_TXFIFO_EMPTY_THRHD_V  0x0000001FU
#define LP_UART_TXFIFO_EMPTY_THRHD_S  11
/** LP_UART_CTS_INV : R/W; bitpos: [16]; default: 0;
 *  Set this bit to inverse the level value of uart cts signal.
 */
#define LP_UART_CTS_INV    (BIT(16))
#define LP_UART_CTS_INV_M  (LP_UART_CTS_INV_V << LP_UART_CTS_INV_S)
#define LP_UART_CTS_INV_V  0x00000001U
#define LP_UART_CTS_INV_S  16
/** LP_UART_DSR_INV : R/W; bitpos: [17]; default: 0;
 *  Set this bit to inverse the level value of uart dsr signal.
 */
#define LP_UART_DSR_INV    (BIT(17))
#define LP_UART_DSR_INV_M  (LP_UART_DSR_INV_V << LP_UART_DSR_INV_S)
#define LP_UART_DSR_INV_V  0x00000001U
#define LP_UART_DSR_INV_S  17
/** LP_UART_RTS_INV : R/W; bitpos: [18]; default: 0;
 *  Set this bit to inverse the level value of uart rts signal.
 */
#define LP_UART_RTS_INV    (BIT(18))
#define LP_UART_RTS_INV_M  (LP_UART_RTS_INV_V << LP_UART_RTS_INV_S)
#define LP_UART_RTS_INV_V  0x00000001U
#define LP_UART_RTS_INV_S  18
/** LP_UART_DTR_INV : R/W; bitpos: [19]; default: 0;
 *  Set this bit to inverse the level value of uart dtr signal.
 */
#define LP_UART_DTR_INV    (BIT(19))
#define LP_UART_DTR_INV_M  (LP_UART_DTR_INV_V << LP_UART_DTR_INV_S)
#define LP_UART_DTR_INV_V  0x00000001U
#define LP_UART_DTR_INV_S  19
/** LP_UART_SW_DTR : R/W; bitpos: [20]; default: 0;
 *  This register is used to configure the software dtr signal which is used in
 *  software flow control.
 */
#define LP_UART_SW_DTR    (BIT(20))
#define LP_UART_SW_DTR_M  (LP_UART_SW_DTR_V << LP_UART_SW_DTR_S)
#define LP_UART_SW_DTR_V  0x00000001U
#define LP_UART_SW_DTR_S  20
/** LP_UART_CLK_EN : R/W; bitpos: [21]; default: 0;
 *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
 *  registers.
 */
#define LP_UART_CLK_EN    (BIT(21))
#define LP_UART_CLK_EN_M  (LP_UART_CLK_EN_V << LP_UART_CLK_EN_S)
#define LP_UART_CLK_EN_V  0x00000001U
#define LP_UART_CLK_EN_S  21

/** LP_UART_HWFC_CONF_SYNC_REG register
 *  Hardware flow-control configuration
 */
#define LP_UART_HWFC_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x2c)
/** LP_UART_RX_FLOW_THRHD : R/W; bitpos: [7:3]; default: 0;
 *  This register is used to configure the maximum amount of data that can be received
 *  when hardware flow control works.
 */
#define LP_UART_RX_FLOW_THRHD    0x0000001FU
#define LP_UART_RX_FLOW_THRHD_M  (LP_UART_RX_FLOW_THRHD_V << LP_UART_RX_FLOW_THRHD_S)
#define LP_UART_RX_FLOW_THRHD_V  0x0000001FU
#define LP_UART_RX_FLOW_THRHD_S  3
/** LP_UART_RX_FLOW_EN : R/W; bitpos: [8]; default: 0;
 *  This is the flow enable bit for UART receiver.
 */
#define LP_UART_RX_FLOW_EN    (BIT(8))
#define LP_UART_RX_FLOW_EN_M  (LP_UART_RX_FLOW_EN_V << LP_UART_RX_FLOW_EN_S)
#define LP_UART_RX_FLOW_EN_V  0x00000001U
#define LP_UART_RX_FLOW_EN_S  8

/** LP_UART_SLEEP_CONF0_REG register
 *  UART sleep configure register 0
 */
#define LP_UART_SLEEP_CONF0_REG (DR_REG_LP_UART_BASE + 0x30)
/** LP_UART_WK_CHAR1 : R/W; bitpos: [7:0]; default: 0;
 *  This register restores the specified wake up char1 to wake up
 */
#define LP_UART_WK_CHAR1    0x000000FFU
#define LP_UART_WK_CHAR1_M  (LP_UART_WK_CHAR1_V << LP_UART_WK_CHAR1_S)
#define LP_UART_WK_CHAR1_V  0x000000FFU
#define LP_UART_WK_CHAR1_S  0
/** LP_UART_WK_CHAR2 : R/W; bitpos: [15:8]; default: 0;
 *  This register restores the specified wake up char2 to wake up
 */
#define LP_UART_WK_CHAR2    0x000000FFU
#define LP_UART_WK_CHAR2_M  (LP_UART_WK_CHAR2_V << LP_UART_WK_CHAR2_S)
#define LP_UART_WK_CHAR2_V  0x000000FFU
#define LP_UART_WK_CHAR2_S  8
/** LP_UART_WK_CHAR3 : R/W; bitpos: [23:16]; default: 0;
 *  This register restores the specified wake up char3 to wake up
 */
#define LP_UART_WK_CHAR3    0x000000FFU
#define LP_UART_WK_CHAR3_M  (LP_UART_WK_CHAR3_V << LP_UART_WK_CHAR3_S)
#define LP_UART_WK_CHAR3_V  0x000000FFU
#define LP_UART_WK_CHAR3_S  16
/** LP_UART_WK_CHAR4 : R/W; bitpos: [31:24]; default: 0;
 *  This register restores the specified wake up char4 to wake up
 */
#define LP_UART_WK_CHAR4    0x000000FFU
#define LP_UART_WK_CHAR4_M  (LP_UART_WK_CHAR4_V << LP_UART_WK_CHAR4_S)
#define LP_UART_WK_CHAR4_V  0x000000FFU
#define LP_UART_WK_CHAR4_S  24

/** LP_UART_SLEEP_CONF1_REG register
 *  UART sleep configure register 1
 */
#define LP_UART_SLEEP_CONF1_REG (DR_REG_LP_UART_BASE + 0x34)
/** LP_UART_WK_CHAR0 : R/W; bitpos: [7:0]; default: 0;
 *  This register restores the specified char0 to wake up
 */
#define LP_UART_WK_CHAR0    0x000000FFU
#define LP_UART_WK_CHAR0_M  (LP_UART_WK_CHAR0_V << LP_UART_WK_CHAR0_S)
#define LP_UART_WK_CHAR0_V  0x000000FFU
#define LP_UART_WK_CHAR0_S  0

/** LP_UART_SLEEP_CONF2_REG register
 *  UART sleep configure register 2
 */
#define LP_UART_SLEEP_CONF2_REG (DR_REG_LP_UART_BASE + 0x38)
/** LP_UART_ACTIVE_THRESHOLD : R/W; bitpos: [9:0]; default: 240;
 *  The uart is activated from light sleeping mode when the input rxd edge changes more
 *  times than this register value.
 */
#define LP_UART_ACTIVE_THRESHOLD    0x000003FFU
#define LP_UART_ACTIVE_THRESHOLD_M  (LP_UART_ACTIVE_THRESHOLD_V << LP_UART_ACTIVE_THRESHOLD_S)
#define LP_UART_ACTIVE_THRESHOLD_V  0x000003FFU
#define LP_UART_ACTIVE_THRESHOLD_S  0
/** LP_UART_RX_WAKE_UP_THRHD : R/W; bitpos: [17:13]; default: 1;
 *  In wake up mode 1 this field is used to set the received data number threshold to
 *  wake up chip.
 */
#define LP_UART_RX_WAKE_UP_THRHD    0x0000001FU
#define LP_UART_RX_WAKE_UP_THRHD_M  (LP_UART_RX_WAKE_UP_THRHD_V << LP_UART_RX_WAKE_UP_THRHD_S)
#define LP_UART_RX_WAKE_UP_THRHD_V  0x0000001FU
#define LP_UART_RX_WAKE_UP_THRHD_S  13
/** LP_UART_WK_CHAR_NUM : R/W; bitpos: [20:18]; default: 5;
 *  This register is used to select number of wake up char.
 */
#define LP_UART_WK_CHAR_NUM    0x00000007U
#define LP_UART_WK_CHAR_NUM_M  (LP_UART_WK_CHAR_NUM_V << LP_UART_WK_CHAR_NUM_S)
#define LP_UART_WK_CHAR_NUM_V  0x00000007U
#define LP_UART_WK_CHAR_NUM_S  18
/** LP_UART_WK_CHAR_MASK : R/W; bitpos: [25:21]; default: 0;
 *  This register is used to mask  wake up char.
 */
#define LP_UART_WK_CHAR_MASK    0x0000001FU
#define LP_UART_WK_CHAR_MASK_M  (LP_UART_WK_CHAR_MASK_V << LP_UART_WK_CHAR_MASK_S)
#define LP_UART_WK_CHAR_MASK_V  0x0000001FU
#define LP_UART_WK_CHAR_MASK_S  21
/** LP_UART_WK_MODE_SEL : R/W; bitpos: [27:26]; default: 0;
 *  This register is used to select wake up mode. 0: RXD toggling to wake up. 1:
 *  received data number larger than
 */
#define LP_UART_WK_MODE_SEL    0x00000003U
#define LP_UART_WK_MODE_SEL_M  (LP_UART_WK_MODE_SEL_V << LP_UART_WK_MODE_SEL_S)
#define LP_UART_WK_MODE_SEL_V  0x00000003U
#define LP_UART_WK_MODE_SEL_S  26

/** LP_UART_SWFC_CONF0_SYNC_REG register
 *  Software flow-control character configuration
 */
#define LP_UART_SWFC_CONF0_SYNC_REG (DR_REG_LP_UART_BASE + 0x3c)
/** LP_UART_XON_CHAR : R/W; bitpos: [7:0]; default: 17;
 *  This register stores the Xon flow control char.
 */
#define LP_UART_XON_CHAR    0x000000FFU
#define LP_UART_XON_CHAR_M  (LP_UART_XON_CHAR_V << LP_UART_XON_CHAR_S)
#define LP_UART_XON_CHAR_V  0x000000FFU
#define LP_UART_XON_CHAR_S  0
/** LP_UART_XOFF_CHAR : R/W; bitpos: [15:8]; default: 19;
 *  This register stores the Xoff flow control char.
 */
#define LP_UART_XOFF_CHAR    0x000000FFU
#define LP_UART_XOFF_CHAR_M  (LP_UART_XOFF_CHAR_V << LP_UART_XOFF_CHAR_S)
#define LP_UART_XOFF_CHAR_V  0x000000FFU
#define LP_UART_XOFF_CHAR_S  8
/** LP_UART_XON_XOFF_STILL_SEND : R/W; bitpos: [16]; default: 0;
 *  In software flow control mode, UART Tx is disabled once UART Rx receives XOFF. In
 *  this status, UART Tx can not transmit XOFF even the received data number is larger
 *  than UART_XOFF_THRESHOLD. Set this bit to enable UART Tx can transmit XON/XOFF when
 *  UART Tx is disabled.
 */
#define LP_UART_XON_XOFF_STILL_SEND    (BIT(16))
#define LP_UART_XON_XOFF_STILL_SEND_M  (LP_UART_XON_XOFF_STILL_SEND_V << LP_UART_XON_XOFF_STILL_SEND_S)
#define LP_UART_XON_XOFF_STILL_SEND_V  0x00000001U
#define LP_UART_XON_XOFF_STILL_SEND_S  16
/** LP_UART_SW_FLOW_CON_EN : R/W; bitpos: [17]; default: 0;
 *  Set this bit to enable software flow control. It is used with register sw_xon or
 *  sw_xoff.
 */
#define LP_UART_SW_FLOW_CON_EN    (BIT(17))
#define LP_UART_SW_FLOW_CON_EN_M  (LP_UART_SW_FLOW_CON_EN_V << LP_UART_SW_FLOW_CON_EN_S)
#define LP_UART_SW_FLOW_CON_EN_V  0x00000001U
#define LP_UART_SW_FLOW_CON_EN_S  17
/** LP_UART_XONOFF_DEL : R/W; bitpos: [18]; default: 0;
 *  Set this bit to remove flow control char from the received data.
 */
#define LP_UART_XONOFF_DEL    (BIT(18))
#define LP_UART_XONOFF_DEL_M  (LP_UART_XONOFF_DEL_V << LP_UART_XONOFF_DEL_S)
#define LP_UART_XONOFF_DEL_V  0x00000001U
#define LP_UART_XONOFF_DEL_S  18
/** LP_UART_FORCE_XON : R/W; bitpos: [19]; default: 0;
 *  Set this bit to enable the transmitter to go on sending data.
 */
#define LP_UART_FORCE_XON    (BIT(19))
#define LP_UART_FORCE_XON_M  (LP_UART_FORCE_XON_V << LP_UART_FORCE_XON_S)
#define LP_UART_FORCE_XON_V  0x00000001U
#define LP_UART_FORCE_XON_S  19
/** LP_UART_FORCE_XOFF : R/W; bitpos: [20]; default: 0;
 *  Set this bit to stop the  transmitter from sending data.
 */
#define LP_UART_FORCE_XOFF    (BIT(20))
#define LP_UART_FORCE_XOFF_M  (LP_UART_FORCE_XOFF_V << LP_UART_FORCE_XOFF_S)
#define LP_UART_FORCE_XOFF_V  0x00000001U
#define LP_UART_FORCE_XOFF_S  20
/** LP_UART_SEND_XON : R/W/SS/SC; bitpos: [21]; default: 0;
 *  Set this bit to send Xon char. It is cleared by hardware automatically.
 */
#define LP_UART_SEND_XON    (BIT(21))
#define LP_UART_SEND_XON_M  (LP_UART_SEND_XON_V << LP_UART_SEND_XON_S)
#define LP_UART_SEND_XON_V  0x00000001U
#define LP_UART_SEND_XON_S  21
/** LP_UART_SEND_XOFF : R/W/SS/SC; bitpos: [22]; default: 0;
 *  Set this bit to send Xoff char. It is cleared by hardware automatically.
 */
#define LP_UART_SEND_XOFF    (BIT(22))
#define LP_UART_SEND_XOFF_M  (LP_UART_SEND_XOFF_V << LP_UART_SEND_XOFF_S)
#define LP_UART_SEND_XOFF_V  0x00000001U
#define LP_UART_SEND_XOFF_S  22

/** LP_UART_SWFC_CONF1_REG register
 *  Software flow-control character configuration
 */
#define LP_UART_SWFC_CONF1_REG (DR_REG_LP_UART_BASE + 0x40)
/** LP_UART_XON_THRESHOLD : R/W; bitpos: [7:3]; default: 0;
 *  When the data amount in Rx-FIFO is less than this register value with
 *  uart_sw_flow_con_en set to 1  it will send a Xon char.
 */
#define LP_UART_XON_THRESHOLD    0x0000001FU
#define LP_UART_XON_THRESHOLD_M  (LP_UART_XON_THRESHOLD_V << LP_UART_XON_THRESHOLD_S)
#define LP_UART_XON_THRESHOLD_V  0x0000001FU
#define LP_UART_XON_THRESHOLD_S  3
/** LP_UART_XOFF_THRESHOLD : R/W; bitpos: [15:11]; default: 12;
 *  When the data amount in Rx-FIFO is more than this register value with
 *  uart_sw_flow_con_en set to 1  it will send a Xoff char.
 */
#define LP_UART_XOFF_THRESHOLD    0x0000001FU
#define LP_UART_XOFF_THRESHOLD_M  (LP_UART_XOFF_THRESHOLD_V << LP_UART_XOFF_THRESHOLD_S)
#define LP_UART_XOFF_THRESHOLD_V  0x0000001FU
#define LP_UART_XOFF_THRESHOLD_S  11

/** LP_UART_TXBRK_CONF_SYNC_REG register
 *  Tx Break character configuration
 */
#define LP_UART_TXBRK_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x44)
/** LP_UART_TX_BRK_NUM : R/W; bitpos: [7:0]; default: 10;
 *  This register is used to configure the number of 0 to be sent after the process of
 *  sending data is done. It is active when txd_brk is set to 1.
 */
#define LP_UART_TX_BRK_NUM    0x000000FFU
#define LP_UART_TX_BRK_NUM_M  (LP_UART_TX_BRK_NUM_V << LP_UART_TX_BRK_NUM_S)
#define LP_UART_TX_BRK_NUM_V  0x000000FFU
#define LP_UART_TX_BRK_NUM_S  0

/** LP_UART_IDLE_CONF_SYNC_REG register
 *  Frame-end idle configuration
 */
#define LP_UART_IDLE_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x48)
/** LP_UART_RX_IDLE_THRHD : R/W; bitpos: [9:0]; default: 256;
 *  It will produce frame end signal when receiver takes more time to receive one byte
 *  data than this register value.
 */
#define LP_UART_RX_IDLE_THRHD    0x000003FFU
#define LP_UART_RX_IDLE_THRHD_M  (LP_UART_RX_IDLE_THRHD_V << LP_UART_RX_IDLE_THRHD_S)
#define LP_UART_RX_IDLE_THRHD_V  0x000003FFU
#define LP_UART_RX_IDLE_THRHD_S  0
/** LP_UART_TX_IDLE_NUM : R/W; bitpos: [19:10]; default: 256;
 *  This register is used to configure the duration time between transfers.
 */
#define LP_UART_TX_IDLE_NUM    0x000003FFU
#define LP_UART_TX_IDLE_NUM_M  (LP_UART_TX_IDLE_NUM_V << LP_UART_TX_IDLE_NUM_S)
#define LP_UART_TX_IDLE_NUM_V  0x000003FFU
#define LP_UART_TX_IDLE_NUM_S  10

/** LP_UART_RS485_CONF_SYNC_REG register
 *  RS485 mode configuration
 */
#define LP_UART_RS485_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x4c)
/** LP_UART_DL0_EN : R/W; bitpos: [1]; default: 0;
 *  Set this bit to delay the stop bit by 1 bit.
 */
#define LP_UART_DL0_EN    (BIT(1))
#define LP_UART_DL0_EN_M  (LP_UART_DL0_EN_V << LP_UART_DL0_EN_S)
#define LP_UART_DL0_EN_V  0x00000001U
#define LP_UART_DL0_EN_S  1
/** LP_UART_DL1_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to delay the stop bit by 1 bit.
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
 *  This register is used to configure the idle duration time before the first at_cmd
 *  is received by receiver.
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
 *  This register is used to configure the duration time between the last at_cmd and
 *  the next data.
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
 *  This register is used to configure the duration time between the at_cmd chars.
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
 *  This register is used to configure the content of at_cmd char.
 */
#define LP_UART_AT_CMD_CHAR    0x000000FFU
#define LP_UART_AT_CMD_CHAR_M  (LP_UART_AT_CMD_CHAR_V << LP_UART_AT_CMD_CHAR_S)
#define LP_UART_AT_CMD_CHAR_V  0x000000FFU
#define LP_UART_AT_CMD_CHAR_S  0
/** LP_UART_CHAR_NUM : R/W; bitpos: [15:8]; default: 3;
 *  This register is used to configure the num of continuous at_cmd chars received by
 *  receiver.
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
 *  UART threshold and allocation configuration
 */
#define LP_UART_TOUT_CONF_SYNC_REG (DR_REG_LP_UART_BASE + 0x64)
/** LP_UART_RX_TOUT_EN : R/W; bitpos: [0]; default: 0;
 *  This is the enable bit for uart receiver's timeout function.
 */
#define LP_UART_RX_TOUT_EN    (BIT(0))
#define LP_UART_RX_TOUT_EN_M  (LP_UART_RX_TOUT_EN_V << LP_UART_RX_TOUT_EN_S)
#define LP_UART_RX_TOUT_EN_V  0x00000001U
#define LP_UART_RX_TOUT_EN_S  0
/** LP_UART_RX_TOUT_FLOW_DIS : R/W; bitpos: [1]; default: 0;
 *  Set this bit to stop accumulating idle_cnt when hardware flow control works.
 */
#define LP_UART_RX_TOUT_FLOW_DIS    (BIT(1))
#define LP_UART_RX_TOUT_FLOW_DIS_M  (LP_UART_RX_TOUT_FLOW_DIS_V << LP_UART_RX_TOUT_FLOW_DIS_S)
#define LP_UART_RX_TOUT_FLOW_DIS_V  0x00000001U
#define LP_UART_RX_TOUT_FLOW_DIS_S  1
/** LP_UART_RX_TOUT_THRHD : R/W; bitpos: [11:2]; default: 10;
 *  This register is used to configure the threshold time that receiver takes to
 *  receive one byte. The rxfifo_tout_int interrupt will be trigger when the receiver
 *  takes more time to receive one byte with rx_tout_en set to 1.
 */
#define LP_UART_RX_TOUT_THRHD    0x000003FFU
#define LP_UART_RX_TOUT_THRHD_M  (LP_UART_RX_TOUT_THRHD_V << LP_UART_RX_TOUT_THRHD_S)
#define LP_UART_RX_TOUT_THRHD_V  0x000003FFU
#define LP_UART_RX_TOUT_THRHD_S  2

/** LP_UART_MEM_TX_STATUS_REG register
 *  Tx-SRAM write and read offset address.
 */
#define LP_UART_MEM_TX_STATUS_REG (DR_REG_LP_UART_BASE + 0x68)
/** LP_UART_TX_SRAM_WADDR : RO; bitpos: [7:3]; default: 0;
 *  This register stores the offset write address in Tx-SRAM.
 */
#define LP_UART_TX_SRAM_WADDR    0x0000001FU
#define LP_UART_TX_SRAM_WADDR_M  (LP_UART_TX_SRAM_WADDR_V << LP_UART_TX_SRAM_WADDR_S)
#define LP_UART_TX_SRAM_WADDR_V  0x0000001FU
#define LP_UART_TX_SRAM_WADDR_S  3
/** LP_UART_TX_SRAM_RADDR : RO; bitpos: [16:12]; default: 0;
 *  This register stores the offset read address in Tx-SRAM.
 */
#define LP_UART_TX_SRAM_RADDR    0x0000001FU
#define LP_UART_TX_SRAM_RADDR_M  (LP_UART_TX_SRAM_RADDR_V << LP_UART_TX_SRAM_RADDR_S)
#define LP_UART_TX_SRAM_RADDR_V  0x0000001FU
#define LP_UART_TX_SRAM_RADDR_S  12

/** LP_UART_MEM_RX_STATUS_REG register
 *  Rx-SRAM write and read offset address.
 */
#define LP_UART_MEM_RX_STATUS_REG (DR_REG_LP_UART_BASE + 0x6c)
/** LP_UART_RX_SRAM_RADDR : RO; bitpos: [7:3]; default: 16;
 *  This register stores the offset read address in RX-SRAM.
 */
#define LP_UART_RX_SRAM_RADDR    0x0000001FU
#define LP_UART_RX_SRAM_RADDR_M  (LP_UART_RX_SRAM_RADDR_V << LP_UART_RX_SRAM_RADDR_S)
#define LP_UART_RX_SRAM_RADDR_V  0x0000001FU
#define LP_UART_RX_SRAM_RADDR_S  3
/** LP_UART_RX_SRAM_WADDR : RO; bitpos: [16:12]; default: 16;
 *  This register stores the offset write address in Rx-SRAM.
 */
#define LP_UART_RX_SRAM_WADDR    0x0000001FU
#define LP_UART_RX_SRAM_WADDR_M  (LP_UART_RX_SRAM_WADDR_V << LP_UART_RX_SRAM_WADDR_S)
#define LP_UART_RX_SRAM_WADDR_V  0x0000001FU
#define LP_UART_RX_SRAM_WADDR_S  12

/** LP_UART_FSM_STATUS_REG register
 *  UART transmit and receive status.
 */
#define LP_UART_FSM_STATUS_REG (DR_REG_LP_UART_BASE + 0x70)
/** LP_UART_ST_URX_OUT : RO; bitpos: [3:0]; default: 0;
 *  This is the status register of receiver.
 */
#define LP_UART_ST_URX_OUT    0x0000000FU
#define LP_UART_ST_URX_OUT_M  (LP_UART_ST_URX_OUT_V << LP_UART_ST_URX_OUT_S)
#define LP_UART_ST_URX_OUT_V  0x0000000FU
#define LP_UART_ST_URX_OUT_S  0
/** LP_UART_ST_UTX_OUT : RO; bitpos: [7:4]; default: 0;
 *  This is the status register of transmitter.
 */
#define LP_UART_ST_UTX_OUT    0x0000000FU
#define LP_UART_ST_UTX_OUT_M  (LP_UART_ST_UTX_OUT_V << LP_UART_ST_UTX_OUT_S)
#define LP_UART_ST_UTX_OUT_V  0x0000000FU
#define LP_UART_ST_UTX_OUT_S  4

/** LP_UART_CLK_CONF_REG register
 *  UART core clock configuration
 */
#define LP_UART_CLK_CONF_REG (DR_REG_LP_UART_BASE + 0x88)
/** LP_UART_SCLK_DIV_B : R/W; bitpos: [5:0]; default: 0;
 *  The  denominator of the frequency divider factor.
 */
#define LP_UART_SCLK_DIV_B    0x0000003FU
#define LP_UART_SCLK_DIV_B_M  (LP_UART_SCLK_DIV_B_V << LP_UART_SCLK_DIV_B_S)
#define LP_UART_SCLK_DIV_B_V  0x0000003FU
#define LP_UART_SCLK_DIV_B_S  0
/** LP_UART_SCLK_DIV_A : R/W; bitpos: [11:6]; default: 0;
 *  The numerator of the frequency divider factor.
 */
#define LP_UART_SCLK_DIV_A    0x0000003FU
#define LP_UART_SCLK_DIV_A_M  (LP_UART_SCLK_DIV_A_V << LP_UART_SCLK_DIV_A_S)
#define LP_UART_SCLK_DIV_A_V  0x0000003FU
#define LP_UART_SCLK_DIV_A_S  6
/** LP_UART_SCLK_DIV_NUM : R/W; bitpos: [19:12]; default: 1;
 *  The integral part of the frequency divider factor.
 */
#define LP_UART_SCLK_DIV_NUM    0x000000FFU
#define LP_UART_SCLK_DIV_NUM_M  (LP_UART_SCLK_DIV_NUM_V << LP_UART_SCLK_DIV_NUM_S)
#define LP_UART_SCLK_DIV_NUM_V  0x000000FFU
#define LP_UART_SCLK_DIV_NUM_S  12
/** LP_UART_SCLK_SEL : R/W; bitpos: [21:20]; default: 3;
 *  UART clock source select. 1: 80Mhz.  2: 8Mhz.  3: XTAL.
 */
#define LP_UART_SCLK_SEL    0x00000003U
#define LP_UART_SCLK_SEL_M  (LP_UART_SCLK_SEL_V << LP_UART_SCLK_SEL_S)
#define LP_UART_SCLK_SEL_V  0x00000003U
#define LP_UART_SCLK_SEL_S  20
/** LP_UART_SCLK_EN : R/W; bitpos: [22]; default: 1;
 *  Set this bit to enable UART Tx/Rx clock.
 */
#define LP_UART_SCLK_EN    (BIT(22))
#define LP_UART_SCLK_EN_M  (LP_UART_SCLK_EN_V << LP_UART_SCLK_EN_S)
#define LP_UART_SCLK_EN_V  0x00000001U
#define LP_UART_SCLK_EN_S  22
/** LP_UART_RST_CORE : R/W; bitpos: [23]; default: 0;
 *  Write 1 then write 0 to this bit to reset UART Tx/Rx.
 */
#define LP_UART_RST_CORE    (BIT(23))
#define LP_UART_RST_CORE_M  (LP_UART_RST_CORE_V << LP_UART_RST_CORE_S)
#define LP_UART_RST_CORE_V  0x00000001U
#define LP_UART_RST_CORE_S  23
/** LP_UART_TX_SCLK_EN : R/W; bitpos: [24]; default: 1;
 *  Set this bit to enable UART Tx clock.
 */
#define LP_UART_TX_SCLK_EN    (BIT(24))
#define LP_UART_TX_SCLK_EN_M  (LP_UART_TX_SCLK_EN_V << LP_UART_TX_SCLK_EN_S)
#define LP_UART_TX_SCLK_EN_V  0x00000001U
#define LP_UART_TX_SCLK_EN_S  24
/** LP_UART_RX_SCLK_EN : R/W; bitpos: [25]; default: 1;
 *  Set this bit to enable UART Rx clock.
 */
#define LP_UART_RX_SCLK_EN    (BIT(25))
#define LP_UART_RX_SCLK_EN_M  (LP_UART_RX_SCLK_EN_V << LP_UART_RX_SCLK_EN_S)
#define LP_UART_RX_SCLK_EN_V  0x00000001U
#define LP_UART_RX_SCLK_EN_S  25
/** LP_UART_TX_RST_CORE : R/W; bitpos: [26]; default: 0;
 *  Write 1 then write 0 to this bit to reset UART Tx.
 */
#define LP_UART_TX_RST_CORE    (BIT(26))
#define LP_UART_TX_RST_CORE_M  (LP_UART_TX_RST_CORE_V << LP_UART_TX_RST_CORE_S)
#define LP_UART_TX_RST_CORE_V  0x00000001U
#define LP_UART_TX_RST_CORE_S  26
/** LP_UART_RX_RST_CORE : R/W; bitpos: [27]; default: 0;
 *  Write 1 then write 0 to this bit to reset UART Rx.
 */
#define LP_UART_RX_RST_CORE    (BIT(27))
#define LP_UART_RX_RST_CORE_M  (LP_UART_RX_RST_CORE_V << LP_UART_RX_RST_CORE_S)
#define LP_UART_RX_RST_CORE_V  0x00000001U
#define LP_UART_RX_RST_CORE_S  27

/** LP_UART_DATE_REG register
 *  UART Version register
 */
#define LP_UART_DATE_REG (DR_REG_LP_UART_BASE + 0x8c)
/** LP_UART_DATE : R/W; bitpos: [31:0]; default: 35656288;
 *  This is the version register.
 */
#define LP_UART_DATE    0xFFFFFFFFU
#define LP_UART_DATE_M  (LP_UART_DATE_V << LP_UART_DATE_S)
#define LP_UART_DATE_V  0xFFFFFFFFU
#define LP_UART_DATE_S  0

/** LP_UART_AFIFO_STATUS_REG register
 *  UART AFIFO Status
 */
#define LP_UART_AFIFO_STATUS_REG (DR_REG_LP_UART_BASE + 0x90)
/** LP_UART_TX_AFIFO_FULL : RO; bitpos: [0]; default: 0;
 *  Full signal of APB TX AFIFO.
 */
#define LP_UART_TX_AFIFO_FULL    (BIT(0))
#define LP_UART_TX_AFIFO_FULL_M  (LP_UART_TX_AFIFO_FULL_V << LP_UART_TX_AFIFO_FULL_S)
#define LP_UART_TX_AFIFO_FULL_V  0x00000001U
#define LP_UART_TX_AFIFO_FULL_S  0
/** LP_UART_TX_AFIFO_EMPTY : RO; bitpos: [1]; default: 1;
 *  Empty signal of APB TX AFIFO.
 */
#define LP_UART_TX_AFIFO_EMPTY    (BIT(1))
#define LP_UART_TX_AFIFO_EMPTY_M  (LP_UART_TX_AFIFO_EMPTY_V << LP_UART_TX_AFIFO_EMPTY_S)
#define LP_UART_TX_AFIFO_EMPTY_V  0x00000001U
#define LP_UART_TX_AFIFO_EMPTY_S  1
/** LP_UART_RX_AFIFO_FULL : RO; bitpos: [2]; default: 0;
 *  Full signal of APB RX AFIFO.
 */
#define LP_UART_RX_AFIFO_FULL    (BIT(2))
#define LP_UART_RX_AFIFO_FULL_M  (LP_UART_RX_AFIFO_FULL_V << LP_UART_RX_AFIFO_FULL_S)
#define LP_UART_RX_AFIFO_FULL_V  0x00000001U
#define LP_UART_RX_AFIFO_FULL_S  2
/** LP_UART_RX_AFIFO_EMPTY : RO; bitpos: [3]; default: 1;
 *  Empty signal of APB RX AFIFO.
 */
#define LP_UART_RX_AFIFO_EMPTY    (BIT(3))
#define LP_UART_RX_AFIFO_EMPTY_M  (LP_UART_RX_AFIFO_EMPTY_V << LP_UART_RX_AFIFO_EMPTY_S)
#define LP_UART_RX_AFIFO_EMPTY_V  0x00000001U
#define LP_UART_RX_AFIFO_EMPTY_S  3

/** LP_UART_REG_UPDATE_REG register
 *  UART Registers Configuration Update register
 */
#define LP_UART_REG_UPDATE_REG (DR_REG_LP_UART_BASE + 0x98)
/** LP_UART_REG_UPDATE : R/W/SC; bitpos: [0]; default: 0;
 *  Software write 1 would synchronize registers into UART Core clock domain and would
 *  be cleared by hardware after synchronization is done.
 */
#define LP_UART_REG_UPDATE    (BIT(0))
#define LP_UART_REG_UPDATE_M  (LP_UART_REG_UPDATE_V << LP_UART_REG_UPDATE_S)
#define LP_UART_REG_UPDATE_V  0x00000001U
#define LP_UART_REG_UPDATE_S  0

/** LP_UART_ID_REG register
 *  UART ID register
 */
#define LP_UART_ID_REG (DR_REG_LP_UART_BASE + 0x9c)
/** LP_UART_ID : R/W; bitpos: [31:0]; default: 1280;
 *  This register is used to configure the uart_id.
 */
#define LP_UART_ID    0xFFFFFFFFU
#define LP_UART_ID_M  (LP_UART_ID_V << LP_UART_ID_S)
#define LP_UART_ID_V  0xFFFFFFFFU
#define LP_UART_ID_S  0

#ifdef __cplusplus
}
#endif
