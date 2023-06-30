/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** TWAIFD_DEVICE_ID_REG register
 *  TWAI FD device id status register
 */
#define TWAIFD_DEVICE_ID_REG (DR_REG_TWAIFD_BASE + 0x0)
/** TWAIFD_DEVICE_ID : R/W; bitpos: [31:0]; default: 51965;
 *  Represents whether CAN IP function is mapped correctly on its base address.
 */
#define TWAIFD_DEVICE_ID    0xFFFFFFFFU
#define TWAIFD_DEVICE_ID_M  (TWAIFD_DEVICE_ID_V << TWAIFD_DEVICE_ID_S)
#define TWAIFD_DEVICE_ID_V  0xFFFFFFFFU
#define TWAIFD_DEVICE_ID_S  0

/** TWAIFD_MODE_SETTING_REG register
 *  TWAI FD mode setting register
 */
#define TWAIFD_MODE_SETTING_REG (DR_REG_TWAIFD_BASE + 0x4)
/** TWAIFD_SW_RESET : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to reset the TWAI FD controller.\\
 *  0: invalid\\
 *  1: reset.\\
 */
#define TWAIFD_SW_RESET    (BIT(0))
#define TWAIFD_SW_RESET_M  (TWAIFD_SW_RESET_V << TWAIFD_SW_RESET_S)
#define TWAIFD_SW_RESET_V  0x00000001U
#define TWAIFD_SW_RESET_S  0
/** TWAIFD_LISTEN_ONLY_MODE : R/W; bitpos: [1]; default: 0;
 *  bus monitor enable
 */
#define TWAIFD_LISTEN_ONLY_MODE    (BIT(1))
#define TWAIFD_LISTEN_ONLY_MODE_M  (TWAIFD_LISTEN_ONLY_MODE_V << TWAIFD_LISTEN_ONLY_MODE_S)
#define TWAIFD_LISTEN_ONLY_MODE_V  0x00000001U
#define TWAIFD_LISTEN_ONLY_MODE_S  1
/** TWAIFD_SELF_TEST_MODE : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable the self test mode.\\
 *  0: disable\\
 *  1: enable\\
 */
#define TWAIFD_SELF_TEST_MODE    (BIT(2))
#define TWAIFD_SELF_TEST_MODE_M  (TWAIFD_SELF_TEST_MODE_V << TWAIFD_SELF_TEST_MODE_S)
#define TWAIFD_SELF_TEST_MODE_V  0x00000001U
#define TWAIFD_SELF_TEST_MODE_S  2
/** TWAIFD_ACCEPT_FILTER_MODE : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable the usage of acceptance filters.\\
 *  0: disable\\
 *  1: enable\\
 */
#define TWAIFD_ACCEPT_FILTER_MODE    (BIT(3))
#define TWAIFD_ACCEPT_FILTER_MODE_M  (TWAIFD_ACCEPT_FILTER_MODE_V << TWAIFD_ACCEPT_FILTER_MODE_S)
#define TWAIFD_ACCEPT_FILTER_MODE_V  0x00000001U
#define TWAIFD_ACCEPT_FILTER_MODE_S  3
/** TWAIFD_FLEXIBLE_DATA_RATE : R/W; bitpos: [4]; default: 1;
 *  Configures whether or not to support flexible data rate.\\
 *  0: not support\\
 *  1: support\\
 */
#define TWAIFD_FLEXIBLE_DATA_RATE    (BIT(4))
#define TWAIFD_FLEXIBLE_DATA_RATE_M  (TWAIFD_FLEXIBLE_DATA_RATE_V << TWAIFD_FLEXIBLE_DATA_RATE_S)
#define TWAIFD_FLEXIBLE_DATA_RATE_V  0x00000001U
#define TWAIFD_FLEXIBLE_DATA_RATE_S  4
/** TWAIFD_RTR_FRM_BEHAVIOR : R/W; bitpos: [5]; default: 0;
 *  time_triggered transmission mode
 */
#define TWAIFD_RTR_FRM_BEHAVIOR    (BIT(5))
#define TWAIFD_RTR_FRM_BEHAVIOR_M  (TWAIFD_RTR_FRM_BEHAVIOR_V << TWAIFD_RTR_FRM_BEHAVIOR_S)
#define TWAIFD_RTR_FRM_BEHAVIOR_V  0x00000001U
#define TWAIFD_RTR_FRM_BEHAVIOR_S  5
/** TWAIFD_ROM : R/W; bitpos: [6]; default: 0;
 *  a\\
 */
#define TWAIFD_ROM    (BIT(6))
#define TWAIFD_ROM_M  (TWAIFD_ROM_V << TWAIFD_ROM_S)
#define TWAIFD_ROM_V  0x00000001U
#define TWAIFD_ROM_S  6
/** TWAIFD_ACK_BEHAVIOR : R/W; bitpos: [7]; default: 0;
 *  Configures the acknowledge behavior.\\
 *  0: normal behavior.\\
 *  1: acknowledge is not sent.\\
 */
#define TWAIFD_ACK_BEHAVIOR    (BIT(7))
#define TWAIFD_ACK_BEHAVIOR_M  (TWAIFD_ACK_BEHAVIOR_V << TWAIFD_ACK_BEHAVIOR_S)
#define TWAIFD_ACK_BEHAVIOR_V  0x00000001U
#define TWAIFD_ACK_BEHAVIOR_S  7
/** TWAIFD_TEST_MODE : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable the triple sampling mode.\\
 *  0: disable\\
 *  1: enable\\
 */
#define TWAIFD_TEST_MODE    (BIT(8))
#define TWAIFD_TEST_MODE_M  (TWAIFD_TEST_MODE_V << TWAIFD_TEST_MODE_S)
#define TWAIFD_TEST_MODE_V  0x00000001U
#define TWAIFD_TEST_MODE_S  8
/** TWAIFD_RXBAM : R/W; bitpos: [9]; default: 1;
 *  a\\
 */
#define TWAIFD_RXBAM    (BIT(9))
#define TWAIFD_RXBAM_M  (TWAIFD_RXBAM_V << TWAIFD_RXBAM_S)
#define TWAIFD_RXBAM_V  0x00000001U
#define TWAIFD_RXBAM_S  9
/** TWAIFD_LIMIT_RETX_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the limit of retransmission.\\
 *  0: disable\\
 *  1: enable\\
 */
#define TWAIFD_LIMIT_RETX_EN    (BIT(16))
#define TWAIFD_LIMIT_RETX_EN_M  (TWAIFD_LIMIT_RETX_EN_V << TWAIFD_LIMIT_RETX_EN_S)
#define TWAIFD_LIMIT_RETX_EN_V  0x00000001U
#define TWAIFD_LIMIT_RETX_EN_S  16
/** TWAIFD_RETX_THRES : R/W; bitpos: [20:17]; default: 0;
 *  Configures the threshold of retransmission attempts. \\
 */
#define TWAIFD_RETX_THRES    0x0000000FU
#define TWAIFD_RETX_THRES_M  (TWAIFD_RETX_THRES_V << TWAIFD_RETX_THRES_S)
#define TWAIFD_RETX_THRES_V  0x0000000FU
#define TWAIFD_RETX_THRES_S  17
/** TWAIFD_ILBP : R/W; bitpos: [21]; default: 0;
 *  acknowledge forbidden mode
 */
#define TWAIFD_ILBP    (BIT(21))
#define TWAIFD_ILBP_M  (TWAIFD_ILBP_V << TWAIFD_ILBP_S)
#define TWAIFD_ILBP_V  0x00000001U
#define TWAIFD_ILBP_S  21
/** TWAIFD_CTRL_EN : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to enable the twai FD controller.\\
 *  0: disable\\
 *  1: enable\\
 */
#define TWAIFD_CTRL_EN    (BIT(22))
#define TWAIFD_CTRL_EN_M  (TWAIFD_CTRL_EN_V << TWAIFD_CTRL_EN_S)
#define TWAIFD_CTRL_EN_V  0x00000001U
#define TWAIFD_CTRL_EN_S  22
/** TWAIFD_FD_TYPE : R/W; bitpos: [23]; default: 0;
 *  Configure the twai fd frame type.\\
 *  0: ISO CAN FD\\
 *  1: CAN FD 1.0\\
 */
#define TWAIFD_FD_TYPE    (BIT(23))
#define TWAIFD_FD_TYPE_M  (TWAIFD_FD_TYPE_V << TWAIFD_FD_TYPE_S)
#define TWAIFD_FD_TYPE_V  0x00000001U
#define TWAIFD_FD_TYPE_S  23
/** TWAIFD_PEX : R/W; bitpos: [24]; default: 0;
 *  protocol expection mode\\
 */
#define TWAIFD_PEX    (BIT(24))
#define TWAIFD_PEX_M  (TWAIFD_PEX_V << TWAIFD_PEX_S)
#define TWAIFD_PEX_V  0x00000001U
#define TWAIFD_PEX_S  24
/** TWAIFD_TBFBO : R/W; bitpos: [25]; default: 1;
 *  a\\
 */
#define TWAIFD_TBFBO    (BIT(25))
#define TWAIFD_TBFBO_M  (TWAIFD_TBFBO_V << TWAIFD_TBFBO_S)
#define TWAIFD_TBFBO_V  0x00000001U
#define TWAIFD_TBFBO_S  25
/** TWAIFD_FDRF : R/W; bitpos: [26]; default: 0;
 *  a\\
 */
#define TWAIFD_FDRF    (BIT(26))
#define TWAIFD_FDRF_M  (TWAIFD_FDRF_V << TWAIFD_FDRF_S)
#define TWAIFD_FDRF_V  0x00000001U
#define TWAIFD_FDRF_S  26

/** TWAIFD_COMMAND_REG register
 *  TWAI FD command register
 */
#define TWAIFD_COMMAND_REG (DR_REG_TWAIFD_BASE + 0x8)
/** TWAIFD_RXRPMV : WO; bitpos: [1]; default: 0;
 *  a\\
 */
#define TWAIFD_RXRPMV    (BIT(1))
#define TWAIFD_RXRPMV_M  (TWAIFD_RXRPMV_V << TWAIFD_RXRPMV_S)
#define TWAIFD_RXRPMV_V  0x00000001U
#define TWAIFD_RXRPMV_S  1
/** TWAIFD_RELEASE_RX_BUF : WO; bitpos: [2]; default: 0;
 *  Configures whether or not to delete all data from the receive buffer.\\
 *  0: invalid\\
 *  1: delete\\
 */
#define TWAIFD_RELEASE_RX_BUF    (BIT(2))
#define TWAIFD_RELEASE_RX_BUF_M  (TWAIFD_RELEASE_RX_BUF_V << TWAIFD_RELEASE_RX_BUF_S)
#define TWAIFD_RELEASE_RX_BUF_V  0x00000001U
#define TWAIFD_RELEASE_RX_BUF_S  2
/** TWAIFD_CLR_OVERRUN_FLG : WO; bitpos: [3]; default: 0;
 *  Configures whether or not to clear the data overrun flag.\\
 *  0: invalid\\
 *  1: clear\\
 */
#define TWAIFD_CLR_OVERRUN_FLG    (BIT(3))
#define TWAIFD_CLR_OVERRUN_FLG_M  (TWAIFD_CLR_OVERRUN_FLG_V << TWAIFD_CLR_OVERRUN_FLG_S)
#define TWAIFD_CLR_OVERRUN_FLG_V  0x00000001U
#define TWAIFD_CLR_OVERRUN_FLG_S  3
/** TWAIFD_ERCRST : WO; bitpos: [4]; default: 0;
 *  a\\
 */
#define TWAIFD_ERCRST    (BIT(4))
#define TWAIFD_ERCRST_M  (TWAIFD_ERCRST_V << TWAIFD_ERCRST_S)
#define TWAIFD_ERCRST_V  0x00000001U
#define TWAIFD_ERCRST_S  4
/** TWAIFD_RXFCRST : WO; bitpos: [5]; default: 0;
 *  a\\
 */
#define TWAIFD_RXFCRST    (BIT(5))
#define TWAIFD_RXFCRST_M  (TWAIFD_RXFCRST_V << TWAIFD_RXFCRST_S)
#define TWAIFD_RXFCRST_V  0x00000001U
#define TWAIFD_RXFCRST_S  5
/** TWAIFD_TXFCRST : WO; bitpos: [6]; default: 0;
 *  a\\
 */
#define TWAIFD_TXFCRST    (BIT(6))
#define TWAIFD_TXFCRST_M  (TWAIFD_TXFCRST_V << TWAIFD_TXFCRST_S)
#define TWAIFD_TXFCRST_V  0x00000001U
#define TWAIFD_TXFCRST_S  6
/** TWAIFD_CPEXS : WO; bitpos: [7]; default: 0;
 *  a\\
 */
#define TWAIFD_CPEXS    (BIT(7))
#define TWAIFD_CPEXS_M  (TWAIFD_CPEXS_V << TWAIFD_CPEXS_S)
#define TWAIFD_CPEXS_V  0x00000001U
#define TWAIFD_CPEXS_S  7

/** TWAIFD_STATUS_REG register
 *  TWAI FD status register
 */
#define TWAIFD_STATUS_REG (DR_REG_TWAIFD_BASE + 0xc)
/** TWAIFD_RX_BUF_STAT : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the receive buffer is empty.\\
 *  0: empty\\
 *  1: not empty\\
 */
#define TWAIFD_RX_BUF_STAT    (BIT(0))
#define TWAIFD_RX_BUF_STAT_M  (TWAIFD_RX_BUF_STAT_V << TWAIFD_RX_BUF_STAT_S)
#define TWAIFD_RX_BUF_STAT_V  0x00000001U
#define TWAIFD_RX_BUF_STAT_S  0
/** TWAIFD_DATA_OVERRUN_FLG : RO; bitpos: [1]; default: 0;
 *  Represents whether or not the receive buffer is full and the frame is
 *  overrun(lost).\\
 *  0: not overrun\\
 *  1: overrun\\
 */
#define TWAIFD_DATA_OVERRUN_FLG    (BIT(1))
#define TWAIFD_DATA_OVERRUN_FLG_M  (TWAIFD_DATA_OVERRUN_FLG_V << TWAIFD_DATA_OVERRUN_FLG_S)
#define TWAIFD_DATA_OVERRUN_FLG_V  0x00000001U
#define TWAIFD_DATA_OVERRUN_FLG_S  1
/** TWAIFD_TX_BUF_SAT : RO; bitpos: [2]; default: 0;
 *  Represents whether or not the transmit buffer is full.\\
 *  0: not full\\
 *  1: full\\
 */
#define TWAIFD_TX_BUF_SAT    (BIT(2))
#define TWAIFD_TX_BUF_SAT_M  (TWAIFD_TX_BUF_SAT_V << TWAIFD_TX_BUF_SAT_S)
#define TWAIFD_TX_BUF_SAT_V  0x00000001U
#define TWAIFD_TX_BUF_SAT_S  2
/** TWAIFD_ERR_FRM_TX : RO; bitpos: [3]; default: 0;
 *  Represents whether or not the error frame is being transmitted.\\
 *  0: not being transmitted\\
 *  1: being transmitted\\
 */
#define TWAIFD_ERR_FRM_TX    (BIT(3))
#define TWAIFD_ERR_FRM_TX_M  (TWAIFD_ERR_FRM_TX_V << TWAIFD_ERR_FRM_TX_S)
#define TWAIFD_ERR_FRM_TX_V  0x00000001U
#define TWAIFD_ERR_FRM_TX_S  3
/** TWAIFD_RX_FRM_STAT : RO; bitpos: [4]; default: 0;
 *  Represents whether or not the controller is receiving a frame.\\
 *  0: not receiving\\
 *  1: receiving\\
 */
#define TWAIFD_RX_FRM_STAT    (BIT(4))
#define TWAIFD_RX_FRM_STAT_M  (TWAIFD_RX_FRM_STAT_V << TWAIFD_RX_FRM_STAT_S)
#define TWAIFD_RX_FRM_STAT_V  0x00000001U
#define TWAIFD_RX_FRM_STAT_S  4
/** TWAIFD_TX_FRM_STAT : RO; bitpos: [5]; default: 0;
 *  Represents whether or not the controller is transmitting a frame.\\
 *  0: not transmitting\\
 *  1: transmitting\\
 */
#define TWAIFD_TX_FRM_STAT    (BIT(5))
#define TWAIFD_TX_FRM_STAT_M  (TWAIFD_TX_FRM_STAT_V << TWAIFD_TX_FRM_STAT_S)
#define TWAIFD_TX_FRM_STAT_V  0x00000001U
#define TWAIFD_TX_FRM_STAT_S  5
/** TWAIFD_ERR_STAT : RO; bitpos: [6]; default: 0;
 *  Represents whether or not the error warning limit is reached.\\
 *  0: not reached\\
 *  1: reached\\
 */
#define TWAIFD_ERR_STAT    (BIT(6))
#define TWAIFD_ERR_STAT_M  (TWAIFD_ERR_STAT_V << TWAIFD_ERR_STAT_S)
#define TWAIFD_ERR_STAT_V  0x00000001U
#define TWAIFD_ERR_STAT_S  6
/** TWAIFD_BUS_STAT : RO; bitpos: [7]; default: 1;
 *  Represents whether or not bus is active.\\
 *  0: active\\
 *  1: not active\\
 */
#define TWAIFD_BUS_STAT    (BIT(7))
#define TWAIFD_BUS_STAT_M  (TWAIFD_BUS_STAT_V << TWAIFD_BUS_STAT_S)
#define TWAIFD_BUS_STAT_V  0x00000001U
#define TWAIFD_BUS_STAT_S  7
/** TWAIFD_PEXS : RO; bitpos: [8]; default: 0;
 *  a\\
 */
#define TWAIFD_PEXS    (BIT(8))
#define TWAIFD_PEXS_M  (TWAIFD_PEXS_V << TWAIFD_PEXS_S)
#define TWAIFD_PEXS_V  0x00000001U
#define TWAIFD_PEXS_S  8
/** TWAIFD_REINTEGRATING_WAIT : RO; bitpos: [9]; default: 0;
 *  fsm is in reintegrating wait status
 */
#define TWAIFD_REINTEGRATING_WAIT    (BIT(9))
#define TWAIFD_REINTEGRATING_WAIT_M  (TWAIFD_REINTEGRATING_WAIT_V << TWAIFD_REINTEGRATING_WAIT_S)
#define TWAIFD_REINTEGRATING_WAIT_V  0x00000001U
#define TWAIFD_REINTEGRATING_WAIT_S  9
/** TWAIFD_STCNT : RO; bitpos: [16]; default: 0;
 *  a\\
 */
#define TWAIFD_STCNT    (BIT(16))
#define TWAIFD_STCNT_M  (TWAIFD_STCNT_V << TWAIFD_STCNT_S)
#define TWAIFD_STCNT_V  0x00000001U
#define TWAIFD_STCNT_S  16
/** TWAIFD_STRGS : RO; bitpos: [17]; default: 0;
 *  a\\
 */
#define TWAIFD_STRGS    (BIT(17))
#define TWAIFD_STRGS_M  (TWAIFD_STRGS_V << TWAIFD_STRGS_S)
#define TWAIFD_STRGS_V  0x00000001U
#define TWAIFD_STRGS_S  17

/** TWAIFD_INT_RAW_REG register
 *  TWAI FD interrupt raw register
 */
#define TWAIFD_INT_RAW_REG (DR_REG_TWAIFD_BASE + 0x10)
/** TWAIFD_RX_FRM_SUC_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status of TWAIFD_RX_FRM_SUC_INT.
 */
#define TWAIFD_RX_FRM_SUC_INT_RAW    (BIT(0))
#define TWAIFD_RX_FRM_SUC_INT_RAW_M  (TWAIFD_RX_FRM_SUC_INT_RAW_V << TWAIFD_RX_FRM_SUC_INT_RAW_S)
#define TWAIFD_RX_FRM_SUC_INT_RAW_V  0x00000001U
#define TWAIFD_RX_FRM_SUC_INT_RAW_S  0
/** TWAIFD_TX_FRM_SUC_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status of TWAIFD_TX_FRM_SUC_INT.
 */
#define TWAIFD_TX_FRM_SUC_INT_RAW    (BIT(1))
#define TWAIFD_TX_FRM_SUC_INT_RAW_M  (TWAIFD_TX_FRM_SUC_INT_RAW_V << TWAIFD_TX_FRM_SUC_INT_RAW_S)
#define TWAIFD_TX_FRM_SUC_INT_RAW_V  0x00000001U
#define TWAIFD_TX_FRM_SUC_INT_RAW_S  1
/** TWAIFD_ERR_WARNING_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status of TWAIFD_ERR_WARNING_INT.
 */
#define TWAIFD_ERR_WARNING_INT_RAW    (BIT(2))
#define TWAIFD_ERR_WARNING_INT_RAW_M  (TWAIFD_ERR_WARNING_INT_RAW_V << TWAIFD_ERR_WARNING_INT_RAW_S)
#define TWAIFD_ERR_WARNING_INT_RAW_V  0x00000001U
#define TWAIFD_ERR_WARNING_INT_RAW_S  2
/** TWAIFD_RX_DATA_OVERRUN_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status of TWAIFD_RX_DATA_OVERRUN_INT.
 */
#define TWAIFD_RX_DATA_OVERRUN_INT_RAW    (BIT(3))
#define TWAIFD_RX_DATA_OVERRUN_INT_RAW_M  (TWAIFD_RX_DATA_OVERRUN_INT_RAW_V << TWAIFD_RX_DATA_OVERRUN_INT_RAW_S)
#define TWAIFD_RX_DATA_OVERRUN_INT_RAW_V  0x00000001U
#define TWAIFD_RX_DATA_OVERRUN_INT_RAW_S  3
/** TWAIFD_FAULT_CONFINEMENT_CHG_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt status of TWAIFD_FAULT_CONFINEMENT_CHG_INT.
 */
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_RAW    (BIT(5))
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_RAW_M  (TWAIFD_FAULT_CONFINEMENT_CHG_INT_RAW_V << TWAIFD_FAULT_CONFINEMENT_CHG_INT_RAW_S)
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_RAW_V  0x00000001U
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_RAW_S  5
/** TWAIFD_ARB_LOST_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt status of TWAIFD_ARB_LOST_INT.
 */
#define TWAIFD_ARB_LOST_INT_RAW    (BIT(6))
#define TWAIFD_ARB_LOST_INT_RAW_M  (TWAIFD_ARB_LOST_INT_RAW_V << TWAIFD_ARB_LOST_INT_RAW_S)
#define TWAIFD_ARB_LOST_INT_RAW_V  0x00000001U
#define TWAIFD_ARB_LOST_INT_RAW_S  6
/** TWAIFD_ERR_DETECTED_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt status of TWAIFD_ERR_DETECTED_INT.
 */
#define TWAIFD_ERR_DETECTED_INT_RAW    (BIT(7))
#define TWAIFD_ERR_DETECTED_INT_RAW_M  (TWAIFD_ERR_DETECTED_INT_RAW_V << TWAIFD_ERR_DETECTED_INT_RAW_S)
#define TWAIFD_ERR_DETECTED_INT_RAW_V  0x00000001U
#define TWAIFD_ERR_DETECTED_INT_RAW_S  7
/** TWAIFD_IS_OVERLOAD_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt status of TWAIFD_IS_OVERLOAD_INT.
 */
#define TWAIFD_IS_OVERLOAD_INT_RAW    (BIT(8))
#define TWAIFD_IS_OVERLOAD_INT_RAW_M  (TWAIFD_IS_OVERLOAD_INT_RAW_V << TWAIFD_IS_OVERLOAD_INT_RAW_S)
#define TWAIFD_IS_OVERLOAD_INT_RAW_V  0x00000001U
#define TWAIFD_IS_OVERLOAD_INT_RAW_S  8
/** TWAIFD_RX_BUF_FULL_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt status of TWAIFD_RX_BUF_FULL_INT.
 */
#define TWAIFD_RX_BUF_FULL_INT_RAW    (BIT(9))
#define TWAIFD_RX_BUF_FULL_INT_RAW_M  (TWAIFD_RX_BUF_FULL_INT_RAW_V << TWAIFD_RX_BUF_FULL_INT_RAW_S)
#define TWAIFD_RX_BUF_FULL_INT_RAW_V  0x00000001U
#define TWAIFD_RX_BUF_FULL_INT_RAW_S  9
/** TWAIFD_BIT_RATE_SHIFT_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt status of TWAIFD_BIT_RATE_SHIFT_INT.
 */
#define TWAIFD_BIT_RATE_SHIFT_INT_RAW    (BIT(10))
#define TWAIFD_BIT_RATE_SHIFT_INT_RAW_M  (TWAIFD_BIT_RATE_SHIFT_INT_RAW_V << TWAIFD_BIT_RATE_SHIFT_INT_RAW_S)
#define TWAIFD_BIT_RATE_SHIFT_INT_RAW_V  0x00000001U
#define TWAIFD_BIT_RATE_SHIFT_INT_RAW_S  10
/** TWAIFD_RX_BUF_NOT_EMPTY_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt status of TWAIFD_RX_BUF_NOT_EMPTY_INT.
 */
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_RAW    (BIT(11))
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_RAW_M  (TWAIFD_RX_BUF_NOT_EMPTY_INT_RAW_V << TWAIFD_RX_BUF_NOT_EMPTY_INT_RAW_S)
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_RAW_V  0x00000001U
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_RAW_S  11
/** TWAIFD_TX_BUF_STATUS_CHG_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt status of TWAIFD_TX_BUF_STATUS_CHG_INT.
 */
#define TWAIFD_TX_BUF_STATUS_CHG_INT_RAW    (BIT(12))
#define TWAIFD_TX_BUF_STATUS_CHG_INT_RAW_M  (TWAIFD_TX_BUF_STATUS_CHG_INT_RAW_V << TWAIFD_TX_BUF_STATUS_CHG_INT_RAW_S)
#define TWAIFD_TX_BUF_STATUS_CHG_INT_RAW_V  0x00000001U
#define TWAIFD_TX_BUF_STATUS_CHG_INT_RAW_S  12

/** TWAIFD_INT_ENA_REG register
 *  TWAI FD interrupt enable register
 */
#define TWAIFD_INT_ENA_REG (DR_REG_TWAIFD_BASE + 0x14)
/** TWAIFD_RX_FRM_SUC_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable TWAIFD_RX_FRM_SUC_INT.
 */
#define TWAIFD_RX_FRM_SUC_INT_ENA    (BIT(0))
#define TWAIFD_RX_FRM_SUC_INT_ENA_M  (TWAIFD_RX_FRM_SUC_INT_ENA_V << TWAIFD_RX_FRM_SUC_INT_ENA_S)
#define TWAIFD_RX_FRM_SUC_INT_ENA_V  0x00000001U
#define TWAIFD_RX_FRM_SUC_INT_ENA_S  0
/** TWAIFD_TX_FRM_SUC_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable TWAIFD_TX_FRM_SUC_INT.
 */
#define TWAIFD_TX_FRM_SUC_INT_ENA    (BIT(1))
#define TWAIFD_TX_FRM_SUC_INT_ENA_M  (TWAIFD_TX_FRM_SUC_INT_ENA_V << TWAIFD_TX_FRM_SUC_INT_ENA_S)
#define TWAIFD_TX_FRM_SUC_INT_ENA_V  0x00000001U
#define TWAIFD_TX_FRM_SUC_INT_ENA_S  1
/** TWAIFD_ERR_WARNING_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable TWAIFD_ERR_WARNING_INT.
 */
#define TWAIFD_ERR_WARNING_INT_ENA    (BIT(2))
#define TWAIFD_ERR_WARNING_INT_ENA_M  (TWAIFD_ERR_WARNING_INT_ENA_V << TWAIFD_ERR_WARNING_INT_ENA_S)
#define TWAIFD_ERR_WARNING_INT_ENA_V  0x00000001U
#define TWAIFD_ERR_WARNING_INT_ENA_S  2
/** TWAIFD_RX_DATA_OVERRUN_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Write 1 to enable TWAIFD_RX_DATA_OVERRUN_INT.
 */
#define TWAIFD_RX_DATA_OVERRUN_INT_ENA    (BIT(3))
#define TWAIFD_RX_DATA_OVERRUN_INT_ENA_M  (TWAIFD_RX_DATA_OVERRUN_INT_ENA_V << TWAIFD_RX_DATA_OVERRUN_INT_ENA_S)
#define TWAIFD_RX_DATA_OVERRUN_INT_ENA_V  0x00000001U
#define TWAIFD_RX_DATA_OVERRUN_INT_ENA_S  3
/** TWAIFD_FAULT_CONFINEMENT_CHG_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Write 1 to enable TWAIFD_FAULT_CONFINEMENT_CHG_INT.
 */
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ENA    (BIT(5))
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ENA_M  (TWAIFD_FAULT_CONFINEMENT_CHG_INT_ENA_V << TWAIFD_FAULT_CONFINEMENT_CHG_INT_ENA_S)
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ENA_V  0x00000001U
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ENA_S  5
/** TWAIFD_ARB_LOST_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Write 1 to enable TWAIFD_ARB_LOST_INT.
 */
#define TWAIFD_ARB_LOST_INT_ENA    (BIT(6))
#define TWAIFD_ARB_LOST_INT_ENA_M  (TWAIFD_ARB_LOST_INT_ENA_V << TWAIFD_ARB_LOST_INT_ENA_S)
#define TWAIFD_ARB_LOST_INT_ENA_V  0x00000001U
#define TWAIFD_ARB_LOST_INT_ENA_S  6
/** TWAIFD_ERR_DETECTED_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Write 1 to enable TWAIFD_ERR_DETECTED_INT.
 */
#define TWAIFD_ERR_DETECTED_INT_ENA    (BIT(7))
#define TWAIFD_ERR_DETECTED_INT_ENA_M  (TWAIFD_ERR_DETECTED_INT_ENA_V << TWAIFD_ERR_DETECTED_INT_ENA_S)
#define TWAIFD_ERR_DETECTED_INT_ENA_V  0x00000001U
#define TWAIFD_ERR_DETECTED_INT_ENA_S  7
/** TWAIFD_IS_OVERLOAD_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Write 1 to enable TWAIFD_IS_OVERLOAD_INT.
 */
#define TWAIFD_IS_OVERLOAD_INT_ENA    (BIT(8))
#define TWAIFD_IS_OVERLOAD_INT_ENA_M  (TWAIFD_IS_OVERLOAD_INT_ENA_V << TWAIFD_IS_OVERLOAD_INT_ENA_S)
#define TWAIFD_IS_OVERLOAD_INT_ENA_V  0x00000001U
#define TWAIFD_IS_OVERLOAD_INT_ENA_S  8
/** TWAIFD_RX_BUF_FULL_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Write 1 to enable TWAIFD_RX_BUF_FULL_INT.
 */
#define TWAIFD_RX_BUF_FULL_INT_ENA    (BIT(9))
#define TWAIFD_RX_BUF_FULL_INT_ENA_M  (TWAIFD_RX_BUF_FULL_INT_ENA_V << TWAIFD_RX_BUF_FULL_INT_ENA_S)
#define TWAIFD_RX_BUF_FULL_INT_ENA_V  0x00000001U
#define TWAIFD_RX_BUF_FULL_INT_ENA_S  9
/** TWAIFD_BIT_RATE_SHIFT_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  Write 1 to enable TWAIFD_BIT_RATE_SHIFT_INT.
 */
#define TWAIFD_BIT_RATE_SHIFT_INT_ENA    (BIT(10))
#define TWAIFD_BIT_RATE_SHIFT_INT_ENA_M  (TWAIFD_BIT_RATE_SHIFT_INT_ENA_V << TWAIFD_BIT_RATE_SHIFT_INT_ENA_S)
#define TWAIFD_BIT_RATE_SHIFT_INT_ENA_V  0x00000001U
#define TWAIFD_BIT_RATE_SHIFT_INT_ENA_S  10
/** TWAIFD_RX_BUF_NOT_EMPTY_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Write 1 to enable TWAIFD_RX_BUF_NOT_EMPTY_INT.
 */
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ENA    (BIT(11))
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ENA_M  (TWAIFD_RX_BUF_NOT_EMPTY_INT_ENA_V << TWAIFD_RX_BUF_NOT_EMPTY_INT_ENA_S)
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ENA_V  0x00000001U
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ENA_S  11
/** TWAIFD_TX_BUF_STATUS_CHG_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  Write 1 to enable TWAIFD_TX_BUF_STATUS_CHG_INT.
 */
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ENA    (BIT(12))
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ENA_M  (TWAIFD_TX_BUF_STATUS_CHG_INT_ENA_V << TWAIFD_TX_BUF_STATUS_CHG_INT_ENA_S)
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ENA_V  0x00000001U
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ENA_S  12

/** TWAIFD_INT_ST_REG register
 *  TWAI FD interrupt status register
 */
#define TWAIFD_INT_ST_REG (DR_REG_TWAIFD_BASE + 0x18)
/** TWAIFD_RX_FRM_SUC_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of TWAIFD_RX_FRM_SUC_INT.
 */
#define TWAIFD_RX_FRM_SUC_INT_ST    (BIT(0))
#define TWAIFD_RX_FRM_SUC_INT_ST_M  (TWAIFD_RX_FRM_SUC_INT_ST_V << TWAIFD_RX_FRM_SUC_INT_ST_S)
#define TWAIFD_RX_FRM_SUC_INT_ST_V  0x00000001U
#define TWAIFD_RX_FRM_SUC_INT_ST_S  0
/** TWAIFD_TX_FRM_SUC_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of TWAIFD_TX_FRM_SUC_INT.
 */
#define TWAIFD_TX_FRM_SUC_INT_ST    (BIT(1))
#define TWAIFD_TX_FRM_SUC_INT_ST_M  (TWAIFD_TX_FRM_SUC_INT_ST_V << TWAIFD_TX_FRM_SUC_INT_ST_S)
#define TWAIFD_TX_FRM_SUC_INT_ST_V  0x00000001U
#define TWAIFD_TX_FRM_SUC_INT_ST_S  1
/** TWAIFD_ERR_WARNING_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of TWAIFD_ERR_WARNING_INT.
 */
#define TWAIFD_ERR_WARNING_INT_ST    (BIT(2))
#define TWAIFD_ERR_WARNING_INT_ST_M  (TWAIFD_ERR_WARNING_INT_ST_V << TWAIFD_ERR_WARNING_INT_ST_S)
#define TWAIFD_ERR_WARNING_INT_ST_V  0x00000001U
#define TWAIFD_ERR_WARNING_INT_ST_S  2
/** TWAIFD_RX_DATA_OVERRUN_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status of TWAIFD_RX_DATA_OVERRUN_INT.
 */
#define TWAIFD_RX_DATA_OVERRUN_INT_ST    (BIT(3))
#define TWAIFD_RX_DATA_OVERRUN_INT_ST_M  (TWAIFD_RX_DATA_OVERRUN_INT_ST_V << TWAIFD_RX_DATA_OVERRUN_INT_ST_S)
#define TWAIFD_RX_DATA_OVERRUN_INT_ST_V  0x00000001U
#define TWAIFD_RX_DATA_OVERRUN_INT_ST_S  3
/** TWAIFD_FAULT_CONFINEMENT_CHG_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status of TWAIFD_FAULT_CONFINEMENT_CHG_INT.
 */
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ST    (BIT(5))
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ST_M  (TWAIFD_FAULT_CONFINEMENT_CHG_INT_ST_V << TWAIFD_FAULT_CONFINEMENT_CHG_INT_ST_S)
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ST_V  0x00000001U
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_ST_S  5
/** TWAIFD_ARB_LOST_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status of TWAIFD_ARB_LOST_INT.
 */
#define TWAIFD_ARB_LOST_INT_ST    (BIT(6))
#define TWAIFD_ARB_LOST_INT_ST_M  (TWAIFD_ARB_LOST_INT_ST_V << TWAIFD_ARB_LOST_INT_ST_S)
#define TWAIFD_ARB_LOST_INT_ST_V  0x00000001U
#define TWAIFD_ARB_LOST_INT_ST_S  6
/** TWAIFD_ERR_DETECTED_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status of TWAIFD_ERR_DETECTED_INT.
 */
#define TWAIFD_ERR_DETECTED_INT_ST    (BIT(7))
#define TWAIFD_ERR_DETECTED_INT_ST_M  (TWAIFD_ERR_DETECTED_INT_ST_V << TWAIFD_ERR_DETECTED_INT_ST_S)
#define TWAIFD_ERR_DETECTED_INT_ST_V  0x00000001U
#define TWAIFD_ERR_DETECTED_INT_ST_S  7
/** TWAIFD_IS_OVERLOAD_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status of TWAIFD_IS_OVERLOAD_INT.
 */
#define TWAIFD_IS_OVERLOAD_INT_ST    (BIT(8))
#define TWAIFD_IS_OVERLOAD_INT_ST_M  (TWAIFD_IS_OVERLOAD_INT_ST_V << TWAIFD_IS_OVERLOAD_INT_ST_S)
#define TWAIFD_IS_OVERLOAD_INT_ST_V  0x00000001U
#define TWAIFD_IS_OVERLOAD_INT_ST_S  8
/** TWAIFD_RX_BUF_FULL_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status of TWAIFD_RX_BUF_FULL_INT.
 */
#define TWAIFD_RX_BUF_FULL_INT_ST    (BIT(9))
#define TWAIFD_RX_BUF_FULL_INT_ST_M  (TWAIFD_RX_BUF_FULL_INT_ST_V << TWAIFD_RX_BUF_FULL_INT_ST_S)
#define TWAIFD_RX_BUF_FULL_INT_ST_V  0x00000001U
#define TWAIFD_RX_BUF_FULL_INT_ST_S  9
/** TWAIFD_BIT_RATE_SHIFT_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status of TWAIFD_BIT_RATE_SHIFT_INT.
 */
#define TWAIFD_BIT_RATE_SHIFT_INT_ST    (BIT(10))
#define TWAIFD_BIT_RATE_SHIFT_INT_ST_M  (TWAIFD_BIT_RATE_SHIFT_INT_ST_V << TWAIFD_BIT_RATE_SHIFT_INT_ST_S)
#define TWAIFD_BIT_RATE_SHIFT_INT_ST_V  0x00000001U
#define TWAIFD_BIT_RATE_SHIFT_INT_ST_S  10
/** TWAIFD_RX_BUF_NOT_EMPTY_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status of TWAIFD_RX_BUF_NOT_EMPTY_INT.
 */
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ST    (BIT(11))
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ST_M  (TWAIFD_RX_BUF_NOT_EMPTY_INT_ST_V << TWAIFD_RX_BUF_NOT_EMPTY_INT_ST_S)
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ST_V  0x00000001U
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_ST_S  11
/** TWAIFD_TX_BUF_STATUS_CHG_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status of TWAIFD_TX_BUF_STATUS_CHG_INT.
 */
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ST    (BIT(12))
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ST_M  (TWAIFD_TX_BUF_STATUS_CHG_INT_ST_V << TWAIFD_TX_BUF_STATUS_CHG_INT_ST_S)
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ST_V  0x00000001U
#define TWAIFD_TX_BUF_STATUS_CHG_INT_ST_S  12

/** TWAIFD_INT_CLR_REG register
 *  TWAI FD interrupt clear register
 */
#define TWAIFD_INT_CLR_REG (DR_REG_TWAIFD_BASE + 0x1c)
/** TWAIFD_RX_FRM_SUC_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear TWAIFD_RX_FRM_SUC_INT.
 */
#define TWAIFD_RX_FRM_SUC_INT_CLR    (BIT(0))
#define TWAIFD_RX_FRM_SUC_INT_CLR_M  (TWAIFD_RX_FRM_SUC_INT_CLR_V << TWAIFD_RX_FRM_SUC_INT_CLR_S)
#define TWAIFD_RX_FRM_SUC_INT_CLR_V  0x00000001U
#define TWAIFD_RX_FRM_SUC_INT_CLR_S  0
/** TWAIFD_TX_FRM_SUC_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear TWAIFD_TX_FRM_SUC_INT.
 */
#define TWAIFD_TX_FRM_SUC_INT_CLR    (BIT(1))
#define TWAIFD_TX_FRM_SUC_INT_CLR_M  (TWAIFD_TX_FRM_SUC_INT_CLR_V << TWAIFD_TX_FRM_SUC_INT_CLR_S)
#define TWAIFD_TX_FRM_SUC_INT_CLR_V  0x00000001U
#define TWAIFD_TX_FRM_SUC_INT_CLR_S  1
/** TWAIFD_ERR_WARNING_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear TWAIFD_ERR_WARNING_INT.
 */
#define TWAIFD_ERR_WARNING_INT_CLR    (BIT(2))
#define TWAIFD_ERR_WARNING_INT_CLR_M  (TWAIFD_ERR_WARNING_INT_CLR_V << TWAIFD_ERR_WARNING_INT_CLR_S)
#define TWAIFD_ERR_WARNING_INT_CLR_V  0x00000001U
#define TWAIFD_ERR_WARNING_INT_CLR_S  2
/** TWAIFD_RX_DATA_OVERRUN_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Write 1 to clear TWAIFD_RX_DATA_OVERRUN_INT.
 */
#define TWAIFD_RX_DATA_OVERRUN_INT_CLR    (BIT(3))
#define TWAIFD_RX_DATA_OVERRUN_INT_CLR_M  (TWAIFD_RX_DATA_OVERRUN_INT_CLR_V << TWAIFD_RX_DATA_OVERRUN_INT_CLR_S)
#define TWAIFD_RX_DATA_OVERRUN_INT_CLR_V  0x00000001U
#define TWAIFD_RX_DATA_OVERRUN_INT_CLR_S  3
/** TWAIFD_FAULT_CONFINEMENT_CHG_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Write 1 to clear TWAIFD_FAULT_CONFINEMENT_CHG_INT.
 */
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_CLR    (BIT(5))
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_CLR_M  (TWAIFD_FAULT_CONFINEMENT_CHG_INT_CLR_V << TWAIFD_FAULT_CONFINEMENT_CHG_INT_CLR_S)
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_CLR_V  0x00000001U
#define TWAIFD_FAULT_CONFINEMENT_CHG_INT_CLR_S  5
/** TWAIFD_ARB_LOST_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Write 1 to clear TWAIFD_ARB_LOST_INT.
 */
#define TWAIFD_ARB_LOST_INT_CLR    (BIT(6))
#define TWAIFD_ARB_LOST_INT_CLR_M  (TWAIFD_ARB_LOST_INT_CLR_V << TWAIFD_ARB_LOST_INT_CLR_S)
#define TWAIFD_ARB_LOST_INT_CLR_V  0x00000001U
#define TWAIFD_ARB_LOST_INT_CLR_S  6
/** TWAIFD_ERR_DETECTED_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Write 1 to clear TWAIFD_ERR_DETECTED_INT.
 */
#define TWAIFD_ERR_DETECTED_INT_CLR    (BIT(7))
#define TWAIFD_ERR_DETECTED_INT_CLR_M  (TWAIFD_ERR_DETECTED_INT_CLR_V << TWAIFD_ERR_DETECTED_INT_CLR_S)
#define TWAIFD_ERR_DETECTED_INT_CLR_V  0x00000001U
#define TWAIFD_ERR_DETECTED_INT_CLR_S  7
/** TWAIFD_IS_OVERLOAD_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Write 1 to clear TWAIFD_IS_OVERLOAD_INT.
 */
#define TWAIFD_IS_OVERLOAD_INT_CLR    (BIT(8))
#define TWAIFD_IS_OVERLOAD_INT_CLR_M  (TWAIFD_IS_OVERLOAD_INT_CLR_V << TWAIFD_IS_OVERLOAD_INT_CLR_S)
#define TWAIFD_IS_OVERLOAD_INT_CLR_V  0x00000001U
#define TWAIFD_IS_OVERLOAD_INT_CLR_S  8
/** TWAIFD_RX_BUF_FULL_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Write 1 to clear TWAIFD_RX_BUF_FULL_INT.
 */
#define TWAIFD_RX_BUF_FULL_INT_CLR    (BIT(9))
#define TWAIFD_RX_BUF_FULL_INT_CLR_M  (TWAIFD_RX_BUF_FULL_INT_CLR_V << TWAIFD_RX_BUF_FULL_INT_CLR_S)
#define TWAIFD_RX_BUF_FULL_INT_CLR_V  0x00000001U
#define TWAIFD_RX_BUF_FULL_INT_CLR_S  9
/** TWAIFD_BIT_RATE_SHIFT_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Write 1 to clear TWAIFD_BIT_RATE_SHIFT_INT.
 */
#define TWAIFD_BIT_RATE_SHIFT_INT_CLR    (BIT(10))
#define TWAIFD_BIT_RATE_SHIFT_INT_CLR_M  (TWAIFD_BIT_RATE_SHIFT_INT_CLR_V << TWAIFD_BIT_RATE_SHIFT_INT_CLR_S)
#define TWAIFD_BIT_RATE_SHIFT_INT_CLR_V  0x00000001U
#define TWAIFD_BIT_RATE_SHIFT_INT_CLR_S  10
/** TWAIFD_RX_BUF_NOT_EMPTY_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Write 1 to clear TWAIFD_RX_BUF_NOT_EMPTY_INT.
 */
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_CLR    (BIT(11))
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_CLR_M  (TWAIFD_RX_BUF_NOT_EMPTY_INT_CLR_V << TWAIFD_RX_BUF_NOT_EMPTY_INT_CLR_S)
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_CLR_V  0x00000001U
#define TWAIFD_RX_BUF_NOT_EMPTY_INT_CLR_S  11
/** TWAIFD_TX_BUF_STATUS_CHG_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Write 1 to clear TWAIFD_TX_BUF_STATUS_CHG_INT.
 */
#define TWAIFD_TX_BUF_STATUS_CHG_INT_CLR    (BIT(12))
#define TWAIFD_TX_BUF_STATUS_CHG_INT_CLR_M  (TWAIFD_TX_BUF_STATUS_CHG_INT_CLR_V << TWAIFD_TX_BUF_STATUS_CHG_INT_CLR_S)
#define TWAIFD_TX_BUF_STATUS_CHG_INT_CLR_V  0x00000001U
#define TWAIFD_TX_BUF_STATUS_CHG_INT_CLR_S  12

/** TWAIFD_BIT_TIMING_REG register
 *  TWAI FD bit-timing  register
 */
#define TWAIFD_BIT_TIMING_REG (DR_REG_TWAIFD_BASE + 0x20)
/** TWAIFD_PROP : R/W; bitpos: [6:0]; default: 5;
 *  Configures the propagation segment of nominal bit rate.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_PROP    0x0000007FU
#define TWAIFD_PROP_M  (TWAIFD_PROP_V << TWAIFD_PROP_S)
#define TWAIFD_PROP_V  0x0000007FU
#define TWAIFD_PROP_S  0
/** TWAIFD_PH1 : R/W; bitpos: [12:7]; default: 3;
 *  Configures the phase 1 segment of nominal bit rate.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_PH1    0x0000003FU
#define TWAIFD_PH1_M  (TWAIFD_PH1_V << TWAIFD_PH1_S)
#define TWAIFD_PH1_V  0x0000003FU
#define TWAIFD_PH1_S  7
/** TWAIFD_PH2 : R/W; bitpos: [18:13]; default: 5;
 *  Configures the phase 2 segment of nominal bit rate.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_PH2    0x0000003FU
#define TWAIFD_PH2_M  (TWAIFD_PH2_V << TWAIFD_PH2_S)
#define TWAIFD_PH2_V  0x0000003FU
#define TWAIFD_PH2_S  13
/** TWAIFD_BRP : R/W; bitpos: [26:19]; default: 16;
 *  Configures the baud-rate prescaler of nominal bit rate.\\
 *  Measurement unit: cycle of core clock.
 */
#define TWAIFD_BRP    0x000000FFU
#define TWAIFD_BRP_M  (TWAIFD_BRP_V << TWAIFD_BRP_S)
#define TWAIFD_BRP_V  0x000000FFU
#define TWAIFD_BRP_S  19
/** TWAIFD_SJW : R/W; bitpos: [31:27]; default: 2;
 *  Represents the synchronization jump width in nominal bit time.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_SJW    0x0000001FU
#define TWAIFD_SJW_M  (TWAIFD_SJW_V << TWAIFD_SJW_S)
#define TWAIFD_SJW_V  0x0000001FU
#define TWAIFD_SJW_S  27

/** TWAIFD_BIT_TIMEING_FD_REG register
 *  TWAI FD bit-timing of FD register
 */
#define TWAIFD_BIT_TIMEING_FD_REG (DR_REG_TWAIFD_BASE + 0x24)
/** TWAIFD_PROP_FD : R/W; bitpos: [5:0]; default: 3;
 *  Configures the propagation segment of data bit rate.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_PROP_FD    0x0000003FU
#define TWAIFD_PROP_FD_M  (TWAIFD_PROP_FD_V << TWAIFD_PROP_FD_S)
#define TWAIFD_PROP_FD_V  0x0000003FU
#define TWAIFD_PROP_FD_S  0
/** TWAIFD_PH1_FD : R/W; bitpos: [11:7]; default: 3;
 *  Configures the phase 1 segment of data bit rate.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_PH1_FD    0x0000001FU
#define TWAIFD_PH1_FD_M  (TWAIFD_PH1_FD_V << TWAIFD_PH1_FD_S)
#define TWAIFD_PH1_FD_V  0x0000001FU
#define TWAIFD_PH1_FD_S  7
/** TWAIFD_PH2_FD : R/W; bitpos: [17:13]; default: 3;
 *  Configures the phase 2 segment of data bit rate.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_PH2_FD    0x0000001FU
#define TWAIFD_PH2_FD_M  (TWAIFD_PH2_FD_V << TWAIFD_PH2_FD_S)
#define TWAIFD_PH2_FD_V  0x0000001FU
#define TWAIFD_PH2_FD_S  13
/** TWAIFD_BRP_FD : R/W; bitpos: [26:19]; default: 4;
 *  Configures the baud-rate prescaler of data bit rate.\\
 *  Measurement unit: cycle of core clock.
 */
#define TWAIFD_BRP_FD    0x000000FFU
#define TWAIFD_BRP_FD_M  (TWAIFD_BRP_FD_V << TWAIFD_BRP_FD_S)
#define TWAIFD_BRP_FD_V  0x000000FFU
#define TWAIFD_BRP_FD_S  19
/** TWAIFD_SJW_FD : R/W; bitpos: [31:27]; default: 2;
 *  Represents the synchronization jump width in data bit time.\\
 *  Measurement unit: time quanta\\
 */
#define TWAIFD_SJW_FD    0x0000001FU
#define TWAIFD_SJW_FD_M  (TWAIFD_SJW_FD_V << TWAIFD_SJW_FD_S)
#define TWAIFD_SJW_FD_V  0x0000001FU
#define TWAIFD_SJW_FD_S  27

/** TWAIFD_ERR_TH_STAT_REG register
 *  TWAI FD error threshold and status register
 */
#define TWAIFD_ERR_TH_STAT_REG (DR_REG_TWAIFD_BASE + 0x28)
/** TWAIFD_ERR_WARNING_THRES : R/W; bitpos: [7:0]; default: 96;
 *  Configures the error warning threshold.\\
 */
#define TWAIFD_ERR_WARNING_THRES    0x000000FFU
#define TWAIFD_ERR_WARNING_THRES_M  (TWAIFD_ERR_WARNING_THRES_V << TWAIFD_ERR_WARNING_THRES_S)
#define TWAIFD_ERR_WARNING_THRES_V  0x000000FFU
#define TWAIFD_ERR_WARNING_THRES_S  0
/** TWAIFD_ERR_PASSIVE_THRES : R/W; bitpos: [15:8]; default: 128;
 *  Configures the error passive threshold.\\
 */
#define TWAIFD_ERR_PASSIVE_THRES    0x000000FFU
#define TWAIFD_ERR_PASSIVE_THRES_M  (TWAIFD_ERR_PASSIVE_THRES_V << TWAIFD_ERR_PASSIVE_THRES_S)
#define TWAIFD_ERR_PASSIVE_THRES_V  0x000000FFU
#define TWAIFD_ERR_PASSIVE_THRES_S  8
/** TWAIFD_ERR_ACTIVE : RO; bitpos: [16]; default: 1;
 *  Represents the fault state of error active.\\
 */
#define TWAIFD_ERR_ACTIVE    (BIT(16))
#define TWAIFD_ERR_ACTIVE_M  (TWAIFD_ERR_ACTIVE_V << TWAIFD_ERR_ACTIVE_S)
#define TWAIFD_ERR_ACTIVE_V  0x00000001U
#define TWAIFD_ERR_ACTIVE_S  16
/** TWAIFD_ERR_PASSIVE : RO; bitpos: [17]; default: 0;
 *  Represents the fault state of error passive.\\
 */
#define TWAIFD_ERR_PASSIVE    (BIT(17))
#define TWAIFD_ERR_PASSIVE_M  (TWAIFD_ERR_PASSIVE_V << TWAIFD_ERR_PASSIVE_S)
#define TWAIFD_ERR_PASSIVE_V  0x00000001U
#define TWAIFD_ERR_PASSIVE_S  17
/** TWAIFD_BUS_OFF : RO; bitpos: [18]; default: 0;
 *  Represents the fault state of bus off.\\
 */
#define TWAIFD_BUS_OFF    (BIT(18))
#define TWAIFD_BUS_OFF_M  (TWAIFD_BUS_OFF_V << TWAIFD_BUS_OFF_S)
#define TWAIFD_BUS_OFF_V  0x00000001U
#define TWAIFD_BUS_OFF_S  18

/** TWAIFD_ERROR_COUNTERS_REG register
 *  TWAI FD error counters status register
 */
#define TWAIFD_ERROR_COUNTERS_REG (DR_REG_TWAIFD_BASE + 0x2c)
/** TWAIFD_RXC_VAL : RO; bitpos: [15:0]; default: 0;
 *  Represents the receiver error counter value.\\
 */
#define TWAIFD_RXC_VAL    0x0000FFFFU
#define TWAIFD_RXC_VAL_M  (TWAIFD_RXC_VAL_V << TWAIFD_RXC_VAL_S)
#define TWAIFD_RXC_VAL_V  0x0000FFFFU
#define TWAIFD_RXC_VAL_S  0
/** TWAIFD_TXC_VAL : RO; bitpos: [31:16]; default: 0;
 *  Represents the transmitter error counter value.\\
 */
#define TWAIFD_TXC_VAL    0x0000FFFFU
#define TWAIFD_TXC_VAL_M  (TWAIFD_TXC_VAL_V << TWAIFD_TXC_VAL_S)
#define TWAIFD_TXC_VAL_V  0x0000FFFFU
#define TWAIFD_TXC_VAL_S  16

/** TWAIFD_ERROR_COUNTERS_SP_REG register
 *  TWAI FD special error counters status register
 */
#define TWAIFD_ERROR_COUNTERS_SP_REG (DR_REG_TWAIFD_BASE + 0x30)
/** TWAIFD_ERR_FD_VAL : RO; bitpos: [15:0]; default: 0;
 *  Represents the number of error in the data bit time.\\
 */
#define TWAIFD_ERR_FD_VAL    0x0000FFFFU
#define TWAIFD_ERR_FD_VAL_M  (TWAIFD_ERR_FD_VAL_V << TWAIFD_ERR_FD_VAL_S)
#define TWAIFD_ERR_FD_VAL_V  0x0000FFFFU
#define TWAIFD_ERR_FD_VAL_S  0
/** TWAIFD_ERR_NORM_VAL : RO; bitpos: [31:16]; default: 0;
 *  Represents the number of error in the nominal bit time.\\
 */
#define TWAIFD_ERR_NORM_VAL    0x0000FFFFU
#define TWAIFD_ERR_NORM_VAL_M  (TWAIFD_ERR_NORM_VAL_V << TWAIFD_ERR_NORM_VAL_S)
#define TWAIFD_ERR_NORM_VAL_V  0x0000FFFFU
#define TWAIFD_ERR_NORM_VAL_S  16

/** TWAIFD_CTR_PRES_REG register
 *  TWAI FD error counters pre-define configuration register
 */
#define TWAIFD_CTR_PRES_REG (DR_REG_TWAIFD_BASE + 0x34)
/** TWAIFD_CTR_PRES_VAL : WO; bitpos: [8:0]; default: 0;
 *  Configures the pre-defined value to set the error counter.\\
 */
#define TWAIFD_CTR_PRES_VAL    0x000001FFU
#define TWAIFD_CTR_PRES_VAL_M  (TWAIFD_CTR_PRES_VAL_V << TWAIFD_CTR_PRES_VAL_S)
#define TWAIFD_CTR_PRES_VAL_V  0x000001FFU
#define TWAIFD_CTR_PRES_VAL_S  0
/** TWAIFD_PTX : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to set the receiver error counter into the value of
 *  pre-defined value.\\
 *  0: invalid\\
 *  1: set\\
 */
#define TWAIFD_PTX    (BIT(9))
#define TWAIFD_PTX_M  (TWAIFD_PTX_V << TWAIFD_PTX_S)
#define TWAIFD_PTX_V  0x00000001U
#define TWAIFD_PTX_S  9
/** TWAIFD_PRX : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to set the transmitter error counter into the value of
 *  pre-defined value.\\
 *  0: invalid\\
 *  1: set\\
 */
#define TWAIFD_PRX    (BIT(10))
#define TWAIFD_PRX_M  (TWAIFD_PRX_V << TWAIFD_PRX_S)
#define TWAIFD_PRX_V  0x00000001U
#define TWAIFD_PRX_S  10
/** TWAIFD_ENORM : WO; bitpos: [11]; default: 0;
 *  Configures whether or not to erase the error counter of nominal bit time.\\
 *  0: invalid\\
 *  1: erase\\
 */
#define TWAIFD_ENORM    (BIT(11))
#define TWAIFD_ENORM_M  (TWAIFD_ENORM_V << TWAIFD_ENORM_S)
#define TWAIFD_ENORM_V  0x00000001U
#define TWAIFD_ENORM_S  11
/** TWAIFD_EFD : WO; bitpos: [12]; default: 0;
 *  Configures whether or not to erase the error counter of data bit time.\\
 *  0: invalid\\
 *  1: erase\\
 */
#define TWAIFD_EFD    (BIT(12))
#define TWAIFD_EFD_M  (TWAIFD_EFD_V << TWAIFD_EFD_S)
#define TWAIFD_EFD_V  0x00000001U
#define TWAIFD_EFD_S  12

/** TWAIFD_RX_MEM_INFO_REG register
 *  TWAI FD rx memory information register
 */
#define TWAIFD_RX_MEM_INFO_REG (DR_REG_TWAIFD_BASE + 0x38)
/** TWAIFD_RX_BUFF_SIZE_VAL : RO; bitpos: [12:0]; default: 0;
 *  Represents the size of RX buffer.\\
 */
#define TWAIFD_RX_BUFF_SIZE_VAL    0x00001FFFU
#define TWAIFD_RX_BUFF_SIZE_VAL_M  (TWAIFD_RX_BUFF_SIZE_VAL_V << TWAIFD_RX_BUFF_SIZE_VAL_S)
#define TWAIFD_RX_BUFF_SIZE_VAL_V  0x00001FFFU
#define TWAIFD_RX_BUFF_SIZE_VAL_S  0
/** TWAIFD_RX_FREE_CTR : RO; bitpos: [28:16]; default: 0;
 *  Represents the number of free words in RX buffer.\\
 */
#define TWAIFD_RX_FREE_CTR    0x00001FFFU
#define TWAIFD_RX_FREE_CTR_M  (TWAIFD_RX_FREE_CTR_V << TWAIFD_RX_FREE_CTR_S)
#define TWAIFD_RX_FREE_CTR_V  0x00001FFFU
#define TWAIFD_RX_FREE_CTR_S  16

/** TWAIFD_RX_POINTERS_REG register
 *  TWAI FD rx memory pointer information register
 */
#define TWAIFD_RX_POINTERS_REG (DR_REG_TWAIFD_BASE + 0x3c)
/** TWAIFD_RX_WPT_VAL : RO; bitpos: [11:0]; default: 0;
 *  Represents the write pointer position in RX buffer.\\
 */
#define TWAIFD_RX_WPT_VAL    0x00000FFFU
#define TWAIFD_RX_WPT_VAL_M  (TWAIFD_RX_WPT_VAL_V << TWAIFD_RX_WPT_VAL_S)
#define TWAIFD_RX_WPT_VAL_V  0x00000FFFU
#define TWAIFD_RX_WPT_VAL_S  0
/** TWAIFD_RX_RPT_VAL : RO; bitpos: [27:16]; default: 0;
 *  Represents the read pointer position in RX buffer.\\
 */
#define TWAIFD_RX_RPT_VAL    0x00000FFFU
#define TWAIFD_RX_RPT_VAL_M  (TWAIFD_RX_RPT_VAL_V << TWAIFD_RX_RPT_VAL_S)
#define TWAIFD_RX_RPT_VAL_V  0x00000FFFU
#define TWAIFD_RX_RPT_VAL_S  16

/** TWAIFD_RX_STATUS_SETTING_REG register
 *  TWAI FD tx status & setting register
 */
#define TWAIFD_RX_STATUS_SETTING_REG (DR_REG_TWAIFD_BASE + 0x40)
/** TWAIFD_RX_EMPTY : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the RX buffer is empty.\\
 *  0: not empty\\
 *  1: empty\\
 */
#define TWAIFD_RX_EMPTY    (BIT(0))
#define TWAIFD_RX_EMPTY_M  (TWAIFD_RX_EMPTY_V << TWAIFD_RX_EMPTY_S)
#define TWAIFD_RX_EMPTY_V  0x00000001U
#define TWAIFD_RX_EMPTY_S  0
/** TWAIFD_RX_FULL : RO; bitpos: [1]; default: 0;
 *  Represents whether or not the RX buffer is full.\\
 *  0: not full\\
 *  1: full\\
 */
#define TWAIFD_RX_FULL    (BIT(1))
#define TWAIFD_RX_FULL_M  (TWAIFD_RX_FULL_V << TWAIFD_RX_FULL_S)
#define TWAIFD_RX_FULL_V  0x00000001U
#define TWAIFD_RX_FULL_S  1
/** TWAIFD_RX_FRM_CTR : RO; bitpos: [14:4]; default: 0;
 *  Represents the number of received frame in RX buffer.\\
 */
#define TWAIFD_RX_FRM_CTR    0x000007FFU
#define TWAIFD_RX_FRM_CTR_M  (TWAIFD_RX_FRM_CTR_V << TWAIFD_RX_FRM_CTR_S)
#define TWAIFD_RX_FRM_CTR_V  0x000007FFU
#define TWAIFD_RX_FRM_CTR_S  4
/** TWAIFD_RTSOP : R/W; bitpos: [16]; default: 0;
 *  a\\
 */
#define TWAIFD_RTSOP    (BIT(16))
#define TWAIFD_RTSOP_M  (TWAIFD_RTSOP_V << TWAIFD_RTSOP_S)
#define TWAIFD_RTSOP_V  0x00000001U
#define TWAIFD_RTSOP_S  16

/** TWAIFD_RX_DATA_REG register
 *  TWAI FD received data register
 */
#define TWAIFD_RX_DATA_REG (DR_REG_TWAIFD_BASE + 0x44)
/** TWAIFD_RX_DATA : RO; bitpos: [31:0]; default: 0;
 *  Data received.
 */
#define TWAIFD_RX_DATA    0xFFFFFFFFU
#define TWAIFD_RX_DATA_M  (TWAIFD_RX_DATA_V << TWAIFD_RX_DATA_S)
#define TWAIFD_RX_DATA_V  0xFFFFFFFFU
#define TWAIFD_RX_DATA_S  0

/** TWAIFD_FILTER_A_MASK_REG register
 *  TWAI FD filter A mask value register
 */
#define TWAIFD_FILTER_A_MASK_REG (DR_REG_TWAIFD_BASE + 0x60)
/** TWAIFD_BIT_MASK_A : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit masked value.
 */
#define TWAIFD_BIT_MASK_A    0x1FFFFFFFU
#define TWAIFD_BIT_MASK_A_M  (TWAIFD_BIT_MASK_A_V << TWAIFD_BIT_MASK_A_S)
#define TWAIFD_BIT_MASK_A_V  0x1FFFFFFFU
#define TWAIFD_BIT_MASK_A_S  0

/** TWAIFD_FILTER_A_VAL_REG register
 *  TWAI FD filter A bit value register
 */
#define TWAIFD_FILTER_A_VAL_REG (DR_REG_TWAIFD_BASE + 0x64)
/** TWAIFD_BIT_VAL_A : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit value.
 */
#define TWAIFD_BIT_VAL_A    0x1FFFFFFFU
#define TWAIFD_BIT_VAL_A_M  (TWAIFD_BIT_VAL_A_V << TWAIFD_BIT_VAL_A_S)
#define TWAIFD_BIT_VAL_A_V  0x1FFFFFFFU
#define TWAIFD_BIT_VAL_A_S  0

/** TWAIFD_FILTER_B_MASK_REG register
 *  TWAI FD filter B mask value register
 */
#define TWAIFD_FILTER_B_MASK_REG (DR_REG_TWAIFD_BASE + 0x68)
/** TWAIFD_BIT_MASK_B : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit masked value.
 */
#define TWAIFD_BIT_MASK_B    0x1FFFFFFFU
#define TWAIFD_BIT_MASK_B_M  (TWAIFD_BIT_MASK_B_V << TWAIFD_BIT_MASK_B_S)
#define TWAIFD_BIT_MASK_B_V  0x1FFFFFFFU
#define TWAIFD_BIT_MASK_B_S  0

/** TWAIFD_FILTER_B_VAL_REG register
 *  TWAI FD filter B bit value register
 */
#define TWAIFD_FILTER_B_VAL_REG (DR_REG_TWAIFD_BASE + 0x6c)
/** TWAIFD_BIT_VAL_B : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit value.
 */
#define TWAIFD_BIT_VAL_B    0x1FFFFFFFU
#define TWAIFD_BIT_VAL_B_M  (TWAIFD_BIT_VAL_B_V << TWAIFD_BIT_VAL_B_S)
#define TWAIFD_BIT_VAL_B_V  0x1FFFFFFFU
#define TWAIFD_BIT_VAL_B_S  0

/** TWAIFD_FILTER_C_MASK_REG register
 *  TWAI FD filter C mask value register
 */
#define TWAIFD_FILTER_C_MASK_REG (DR_REG_TWAIFD_BASE + 0x70)
/** TWAIFD_BIT_MASK_C : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit masked value.
 */
#define TWAIFD_BIT_MASK_C    0x1FFFFFFFU
#define TWAIFD_BIT_MASK_C_M  (TWAIFD_BIT_MASK_C_V << TWAIFD_BIT_MASK_C_S)
#define TWAIFD_BIT_MASK_C_V  0x1FFFFFFFU
#define TWAIFD_BIT_MASK_C_S  0

/** TWAIFD_FILTER_C_VAL_REG register
 *  TWAI FD filter C bit value register
 */
#define TWAIFD_FILTER_C_VAL_REG (DR_REG_TWAIFD_BASE + 0x74)
/** TWAIFD_BIT_VAL_C : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit value.
 */
#define TWAIFD_BIT_VAL_C    0x1FFFFFFFU
#define TWAIFD_BIT_VAL_C_M  (TWAIFD_BIT_VAL_C_V << TWAIFD_BIT_VAL_C_S)
#define TWAIFD_BIT_VAL_C_V  0x1FFFFFFFU
#define TWAIFD_BIT_VAL_C_S  0

/** TWAIFD_FILTER_RAN_LOW_REG register
 *  TWAI FD filter range low value register
 */
#define TWAIFD_FILTER_RAN_LOW_REG (DR_REG_TWAIFD_BASE + 0x78)
/** TWAIFD_BIT_RAN_LOW : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit masked value.
 */
#define TWAIFD_BIT_RAN_LOW    0x1FFFFFFFU
#define TWAIFD_BIT_RAN_LOW_M  (TWAIFD_BIT_RAN_LOW_V << TWAIFD_BIT_RAN_LOW_S)
#define TWAIFD_BIT_RAN_LOW_V  0x1FFFFFFFU
#define TWAIFD_BIT_RAN_LOW_S  0

/** TWAIFD_FILTER_RAN_HIGH_REG register
 *  TWAI FD filter range high value register
 */
#define TWAIFD_FILTER_RAN_HIGH_REG (DR_REG_TWAIFD_BASE + 0x7c)
/** TWAIFD_BIT_RAN_HIGH : R/W; bitpos: [28:0]; default: 0;
 *  filter A bit value.
 */
#define TWAIFD_BIT_RAN_HIGH    0x1FFFFFFFU
#define TWAIFD_BIT_RAN_HIGH_M  (TWAIFD_BIT_RAN_HIGH_V << TWAIFD_BIT_RAN_HIGH_S)
#define TWAIFD_BIT_RAN_HIGH_V  0x1FFFFFFFU
#define TWAIFD_BIT_RAN_HIGH_S  0

/** TWAIFD_FILTER_CONTROL_REG register
 *  TWAI FD filter control register
 */
#define TWAIFD_FILTER_CONTROL_REG (DR_REG_TWAIFD_BASE + 0x80)
/** TWAIFD_FANB : R/W; bitpos: [0]; default: 1;
 *  filter A with nominal and base mode.
 */
#define TWAIFD_FANB    (BIT(0))
#define TWAIFD_FANB_M  (TWAIFD_FANB_V << TWAIFD_FANB_S)
#define TWAIFD_FANB_V  0x00000001U
#define TWAIFD_FANB_S  0
/** TWAIFD_FANE : R/W; bitpos: [1]; default: 1;
 *  filter A with nominal and extended mode.
 */
#define TWAIFD_FANE    (BIT(1))
#define TWAIFD_FANE_M  (TWAIFD_FANE_V << TWAIFD_FANE_S)
#define TWAIFD_FANE_V  0x00000001U
#define TWAIFD_FANE_S  1
/** TWAIFD_FAFB : R/W; bitpos: [2]; default: 1;
 *  filter A with FD and base mode.
 */
#define TWAIFD_FAFB    (BIT(2))
#define TWAIFD_FAFB_M  (TWAIFD_FAFB_V << TWAIFD_FAFB_S)
#define TWAIFD_FAFB_V  0x00000001U
#define TWAIFD_FAFB_S  2
/** TWAIFD_FAFE : R/W; bitpos: [3]; default: 1;
 *  filter A with FD and extended mode.
 */
#define TWAIFD_FAFE    (BIT(3))
#define TWAIFD_FAFE_M  (TWAIFD_FAFE_V << TWAIFD_FAFE_S)
#define TWAIFD_FAFE_V  0x00000001U
#define TWAIFD_FAFE_S  3
/** TWAIFD_FBNB : R/W; bitpos: [4]; default: 0;
 *  filter B with nominal and base mode.
 */
#define TWAIFD_FBNB    (BIT(4))
#define TWAIFD_FBNB_M  (TWAIFD_FBNB_V << TWAIFD_FBNB_S)
#define TWAIFD_FBNB_V  0x00000001U
#define TWAIFD_FBNB_S  4
/** TWAIFD_FBNE : R/W; bitpos: [5]; default: 0;
 *  filter B with nominal and extended mode.
 */
#define TWAIFD_FBNE    (BIT(5))
#define TWAIFD_FBNE_M  (TWAIFD_FBNE_V << TWAIFD_FBNE_S)
#define TWAIFD_FBNE_V  0x00000001U
#define TWAIFD_FBNE_S  5
/** TWAIFD_FBFB : R/W; bitpos: [6]; default: 0;
 *  filter B with FD and base mode.
 */
#define TWAIFD_FBFB    (BIT(6))
#define TWAIFD_FBFB_M  (TWAIFD_FBFB_V << TWAIFD_FBFB_S)
#define TWAIFD_FBFB_V  0x00000001U
#define TWAIFD_FBFB_S  6
/** TWAIFD_FBFE : R/W; bitpos: [7]; default: 0;
 *  filter B with FD and extended mode.
 */
#define TWAIFD_FBFE    (BIT(7))
#define TWAIFD_FBFE_M  (TWAIFD_FBFE_V << TWAIFD_FBFE_S)
#define TWAIFD_FBFE_V  0x00000001U
#define TWAIFD_FBFE_S  7
/** TWAIFD_FCNB : R/W; bitpos: [8]; default: 0;
 *  filter C with nominal and base mode.
 */
#define TWAIFD_FCNB    (BIT(8))
#define TWAIFD_FCNB_M  (TWAIFD_FCNB_V << TWAIFD_FCNB_S)
#define TWAIFD_FCNB_V  0x00000001U
#define TWAIFD_FCNB_S  8
/** TWAIFD_FCNE : R/W; bitpos: [9]; default: 0;
 *  filter C with nominal and extended mode.
 */
#define TWAIFD_FCNE    (BIT(9))
#define TWAIFD_FCNE_M  (TWAIFD_FCNE_V << TWAIFD_FCNE_S)
#define TWAIFD_FCNE_V  0x00000001U
#define TWAIFD_FCNE_S  9
/** TWAIFD_FCFB : R/W; bitpos: [10]; default: 0;
 *  filter C with FD and base mode.
 */
#define TWAIFD_FCFB    (BIT(10))
#define TWAIFD_FCFB_M  (TWAIFD_FCFB_V << TWAIFD_FCFB_S)
#define TWAIFD_FCFB_V  0x00000001U
#define TWAIFD_FCFB_S  10
/** TWAIFD_FCFE : R/W; bitpos: [11]; default: 0;
 *  filter C with FD and extended mode.
 */
#define TWAIFD_FCFE    (BIT(11))
#define TWAIFD_FCFE_M  (TWAIFD_FCFE_V << TWAIFD_FCFE_S)
#define TWAIFD_FCFE_V  0x00000001U
#define TWAIFD_FCFE_S  11
/** TWAIFD_FRNB : R/W; bitpos: [12]; default: 0;
 *  filter range with nominal and base mode.
 */
#define TWAIFD_FRNB    (BIT(12))
#define TWAIFD_FRNB_M  (TWAIFD_FRNB_V << TWAIFD_FRNB_S)
#define TWAIFD_FRNB_V  0x00000001U
#define TWAIFD_FRNB_S  12
/** TWAIFD_FRNE : R/W; bitpos: [13]; default: 0;
 *  filter range with nominal and extended mode.
 */
#define TWAIFD_FRNE    (BIT(13))
#define TWAIFD_FRNE_M  (TWAIFD_FRNE_V << TWAIFD_FRNE_S)
#define TWAIFD_FRNE_V  0x00000001U
#define TWAIFD_FRNE_S  13
/** TWAIFD_FRFB : R/W; bitpos: [14]; default: 0;
 *  filter range with FD and base mode.
 */
#define TWAIFD_FRFB    (BIT(14))
#define TWAIFD_FRFB_M  (TWAIFD_FRFB_V << TWAIFD_FRFB_S)
#define TWAIFD_FRFB_V  0x00000001U
#define TWAIFD_FRFB_S  14
/** TWAIFD_FRFE : R/W; bitpos: [15]; default: 0;
 *  filter range with FD and extended mode.
 */
#define TWAIFD_FRFE    (BIT(15))
#define TWAIFD_FRFE_M  (TWAIFD_FRFE_V << TWAIFD_FRFE_S)
#define TWAIFD_FRFE_V  0x00000001U
#define TWAIFD_FRFE_S  15
/** TWAIFD_SFA : RO; bitpos: [16]; default: 0;
 *  filter A status
 */
#define TWAIFD_SFA    (BIT(16))
#define TWAIFD_SFA_M  (TWAIFD_SFA_V << TWAIFD_SFA_S)
#define TWAIFD_SFA_V  0x00000001U
#define TWAIFD_SFA_S  16
/** TWAIFD_SFB : RO; bitpos: [17]; default: 0;
 *  filter B status
 */
#define TWAIFD_SFB    (BIT(17))
#define TWAIFD_SFB_M  (TWAIFD_SFB_V << TWAIFD_SFB_S)
#define TWAIFD_SFB_V  0x00000001U
#define TWAIFD_SFB_S  17
/** TWAIFD_SFC : RO; bitpos: [18]; default: 0;
 *  filter C status
 */
#define TWAIFD_SFC    (BIT(18))
#define TWAIFD_SFC_M  (TWAIFD_SFC_V << TWAIFD_SFC_S)
#define TWAIFD_SFC_V  0x00000001U
#define TWAIFD_SFC_S  18
/** TWAIFD_SFR : RO; bitpos: [19]; default: 0;
 *  filter range status
 */
#define TWAIFD_SFR    (BIT(19))
#define TWAIFD_SFR_M  (TWAIFD_SFR_V << TWAIFD_SFR_S)
#define TWAIFD_SFR_V  0x00000001U
#define TWAIFD_SFR_S  19

/** TWAIFD_TX_STAT_REG register
 *  TWAI FD TX buffer status register
 */
#define TWAIFD_TX_STAT_REG (DR_REG_TWAIFD_BASE + 0x94)
/** TWAIFD_TXT_1_EMPTY : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the TX buffer1 is empty.\\
 *  0: not empty\\
 *  1: empty\\
 */
#define TWAIFD_TXT_1_EMPTY    (BIT(0))
#define TWAIFD_TXT_1_EMPTY_M  (TWAIFD_TXT_1_EMPTY_V << TWAIFD_TXT_1_EMPTY_S)
#define TWAIFD_TXT_1_EMPTY_V  0x00000001U
#define TWAIFD_TXT_1_EMPTY_S  0
/** TWAIFD_TXT_2_EMPTY : RO; bitpos: [1]; default: 0;
 *  Represents whether or not the TX buffer2 is empty.\\
 *  0: not empty\\
 *  1: empty\\
 */
#define TWAIFD_TXT_2_EMPTY    (BIT(1))
#define TWAIFD_TXT_2_EMPTY_M  (TWAIFD_TXT_2_EMPTY_V << TWAIFD_TXT_2_EMPTY_S)
#define TWAIFD_TXT_2_EMPTY_V  0x00000001U
#define TWAIFD_TXT_2_EMPTY_S  1

/** TWAIFD_TX_CFG_REG register
 *  TWAI FD TX buffer configuration register
 */
#define TWAIFD_TX_CFG_REG (DR_REG_TWAIFD_BASE + 0x98)
/** TWAIFD_TXT_1_ALLOW : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not allow transmitting frames from TX buffer1.\\
 *  0: not allow\\
 *  1: allow\\
 */
#define TWAIFD_TXT_1_ALLOW    (BIT(0))
#define TWAIFD_TXT_1_ALLOW_M  (TWAIFD_TXT_1_ALLOW_V << TWAIFD_TXT_1_ALLOW_S)
#define TWAIFD_TXT_1_ALLOW_V  0x00000001U
#define TWAIFD_TXT_1_ALLOW_S  0
/** TWAIFD_TXT_2_ALLOW : R/W; bitpos: [1]; default: 1;
 *  Configures whether or not allow transmitting frames from TX buffer2.\\
 *  0: not allow\\
 *  1: allow\\
 */
#define TWAIFD_TXT_2_ALLOW    (BIT(1))
#define TWAIFD_TXT_2_ALLOW_M  (TWAIFD_TXT_2_ALLOW_V << TWAIFD_TXT_2_ALLOW_S)
#define TWAIFD_TXT_2_ALLOW_V  0x00000001U
#define TWAIFD_TXT_2_ALLOW_S  1
/** TWAIFD_TXT_1_COMMIT : WT; bitpos: [2]; default: 0;
 *  Configures whether or not the frames from TX register are inserted into TX
 *  buffer1.\\
 *  0: not inserted\\
 *  1: inserted\\
 */
#define TWAIFD_TXT_1_COMMIT    (BIT(2))
#define TWAIFD_TXT_1_COMMIT_M  (TWAIFD_TXT_1_COMMIT_V << TWAIFD_TXT_1_COMMIT_S)
#define TWAIFD_TXT_1_COMMIT_V  0x00000001U
#define TWAIFD_TXT_1_COMMIT_S  2
/** TWAIFD_TXT_2_COMMIT : WT; bitpos: [3]; default: 0;
 *  Configures whether or not the frames from TX register are inserted into TX
 *  buffer2.\\
 *  0: not inserted\\
 *  1: inserted\\
 */
#define TWAIFD_TXT_2_COMMIT    (BIT(3))
#define TWAIFD_TXT_2_COMMIT_M  (TWAIFD_TXT_2_COMMIT_V << TWAIFD_TXT_2_COMMIT_S)
#define TWAIFD_TXT_2_COMMIT_V  0x00000001U
#define TWAIFD_TXT_2_COMMIT_S  3

/** TWAIFD_TX_DATA_0_REG register
 *  TWAI FD transmit data register 0
 */
#define TWAIFD_TX_DATA_0_REG (DR_REG_TWAIFD_BASE + 0x9c)
/** TWAIFD_DLC_TX : R/W; bitpos: [3:0]; default: 0;
 *  Configures the brs to be transmitted.
 */
#define TWAIFD_DLC_TX    0x0000000FU
#define TWAIFD_DLC_TX_M  (TWAIFD_DLC_TX_V << TWAIFD_DLC_TX_S)
#define TWAIFD_DLC_TX_V  0x0000000FU
#define TWAIFD_DLC_TX_S  0
/** TWAIFD_RTR_TX : R/W; bitpos: [5]; default: 0;
 *  Configures the rtr bit to be transmitted.
 */
#define TWAIFD_RTR_TX    (BIT(5))
#define TWAIFD_RTR_TX_M  (TWAIFD_RTR_TX_V << TWAIFD_RTR_TX_S)
#define TWAIFD_RTR_TX_V  0x00000001U
#define TWAIFD_RTR_TX_S  5
/** TWAIFD_ID_TYPE_TX : R/W; bitpos: [6]; default: 0;
 *  Configures the frame type to be transmitted.
 */
#define TWAIFD_ID_TYPE_TX    (BIT(6))
#define TWAIFD_ID_TYPE_TX_M  (TWAIFD_ID_TYPE_TX_V << TWAIFD_ID_TYPE_TX_S)
#define TWAIFD_ID_TYPE_TX_V  0x00000001U
#define TWAIFD_ID_TYPE_TX_S  6
/** TWAIFD_FR_TYPE_TX : R/W; bitpos: [7]; default: 0;
 *  Configures the fd type to be transmitted.
 */
#define TWAIFD_FR_TYPE_TX    (BIT(7))
#define TWAIFD_FR_TYPE_TX_M  (TWAIFD_FR_TYPE_TX_V << TWAIFD_FR_TYPE_TX_S)
#define TWAIFD_FR_TYPE_TX_V  0x00000001U
#define TWAIFD_FR_TYPE_TX_S  7
/** TWAIFD_TBF_TX : R/W; bitpos: [8]; default: 0;
 *  Configures the tbf bit to be transmitted.
 */
#define TWAIFD_TBF_TX    (BIT(8))
#define TWAIFD_TBF_TX_M  (TWAIFD_TBF_TX_V << TWAIFD_TBF_TX_S)
#define TWAIFD_TBF_TX_V  0x00000001U
#define TWAIFD_TBF_TX_S  8
/** TWAIFD_BRS_TX : R/W; bitpos: [9]; default: 0;
 *  Configures the brs bit to be transmitted.
 */
#define TWAIFD_BRS_TX    (BIT(9))
#define TWAIFD_BRS_TX_M  (TWAIFD_BRS_TX_V << TWAIFD_BRS_TX_S)
#define TWAIFD_BRS_TX_V  0x00000001U
#define TWAIFD_BRS_TX_S  9

/** TWAIFD_TX_DATA_1_REG register
 *  TWAI FD transmit data register 1
 */
#define TWAIFD_TX_DATA_1_REG (DR_REG_TWAIFD_BASE + 0xa0)
/** TWAIFD_TS_VAL_U_TX : R/W; bitpos: [31:0]; default: 0;
 *  Configures the upper timestamp to be transmitted
 */
#define TWAIFD_TS_VAL_U_TX    0xFFFFFFFFU
#define TWAIFD_TS_VAL_U_TX_M  (TWAIFD_TS_VAL_U_TX_V << TWAIFD_TS_VAL_U_TX_S)
#define TWAIFD_TS_VAL_U_TX_V  0xFFFFFFFFU
#define TWAIFD_TS_VAL_U_TX_S  0

/** TWAIFD_TX_DATA_2_REG register
 *  TWAI FD transmit data register 2
 */
#define TWAIFD_TX_DATA_2_REG (DR_REG_TWAIFD_BASE + 0xa4)
/** TWAIFD_TS_VAL_L_TX : R/W; bitpos: [31:0]; default: 0;
 *  Configures the lower timestamp to be transmitted
 */
#define TWAIFD_TS_VAL_L_TX    0xFFFFFFFFU
#define TWAIFD_TS_VAL_L_TX_M  (TWAIFD_TS_VAL_L_TX_V << TWAIFD_TS_VAL_L_TX_S)
#define TWAIFD_TS_VAL_L_TX_V  0xFFFFFFFFU
#define TWAIFD_TS_VAL_L_TX_S  0

/** TWAIFD_TX_DATA_3_REG register
 *  TWAI FD transmit data register 3
 */
#define TWAIFD_TX_DATA_3_REG (DR_REG_TWAIFD_BASE + 0xa8)
/** TWAIFD_ID_EXT_TX : R/W; bitpos: [17:0]; default: 0;
 *  Configures the base ID to be transmitted
 */
#define TWAIFD_ID_EXT_TX    0x0003FFFFU
#define TWAIFD_ID_EXT_TX_M  (TWAIFD_ID_EXT_TX_V << TWAIFD_ID_EXT_TX_S)
#define TWAIFD_ID_EXT_TX_V  0x0003FFFFU
#define TWAIFD_ID_EXT_TX_S  0
/** TWAIFD_ID_BASE_TX : R/W; bitpos: [28:18]; default: 0;
 *  Configures the extended ID to be transmitted
 */
#define TWAIFD_ID_BASE_TX    0x000007FFU
#define TWAIFD_ID_BASE_TX_M  (TWAIFD_ID_BASE_TX_V << TWAIFD_ID_BASE_TX_S)
#define TWAIFD_ID_BASE_TX_V  0x000007FFU
#define TWAIFD_ID_BASE_TX_S  18

/** TWAIFD_TX_DATA_4_REG register
 *  TWAI FD transmit data register 4
 */
#define TWAIFD_TX_DATA_4_REG (DR_REG_TWAIFD_BASE + 0xac)
/** TWAIFD_TX_DATA0 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 0th word to be transmitted
 */
#define TWAIFD_TX_DATA0    0xFFFFFFFFU
#define TWAIFD_TX_DATA0_M  (TWAIFD_TX_DATA0_V << TWAIFD_TX_DATA0_S)
#define TWAIFD_TX_DATA0_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA0_S  0

/** TWAIFD_TX_DATA_5_REG register
 *  TWAI FD transmit data register 5
 */
#define TWAIFD_TX_DATA_5_REG (DR_REG_TWAIFD_BASE + 0xb0)
/** TWAIFD_TX_DATA1 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 1th word to be transmitted
 */
#define TWAIFD_TX_DATA1    0xFFFFFFFFU
#define TWAIFD_TX_DATA1_M  (TWAIFD_TX_DATA1_V << TWAIFD_TX_DATA1_S)
#define TWAIFD_TX_DATA1_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA1_S  0

/** TWAIFD_TX_DATA_6_REG register
 *  TWAI FD transmit data register 6
 */
#define TWAIFD_TX_DATA_6_REG (DR_REG_TWAIFD_BASE + 0xb4)
/** TWAIFD_TX_DATA2 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 2th word to be transmitted
 */
#define TWAIFD_TX_DATA2    0xFFFFFFFFU
#define TWAIFD_TX_DATA2_M  (TWAIFD_TX_DATA2_V << TWAIFD_TX_DATA2_S)
#define TWAIFD_TX_DATA2_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA2_S  0

/** TWAIFD_TX_DATA_7_REG register
 *  TWAI FD transmit data register 7
 */
#define TWAIFD_TX_DATA_7_REG (DR_REG_TWAIFD_BASE + 0xb8)
/** TWAIFD_TX_DATA3 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 3th word to be transmitted
 */
#define TWAIFD_TX_DATA3    0xFFFFFFFFU
#define TWAIFD_TX_DATA3_M  (TWAIFD_TX_DATA3_V << TWAIFD_TX_DATA3_S)
#define TWAIFD_TX_DATA3_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA3_S  0

/** TWAIFD_TX_DATA_8_REG register
 *  TWAI FD transmit data register 8
 */
#define TWAIFD_TX_DATA_8_REG (DR_REG_TWAIFD_BASE + 0xbc)
/** TWAIFD_TX_DATA4 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 4th word to be transmitted
 */
#define TWAIFD_TX_DATA4    0xFFFFFFFFU
#define TWAIFD_TX_DATA4_M  (TWAIFD_TX_DATA4_V << TWAIFD_TX_DATA4_S)
#define TWAIFD_TX_DATA4_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA4_S  0

/** TWAIFD_TX_DATA_9_REG register
 *  TWAI FD transmit data register 9
 */
#define TWAIFD_TX_DATA_9_REG (DR_REG_TWAIFD_BASE + 0xc0)
/** TWAIFD_TX_DATA5 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 5th word to be transmitted
 */
#define TWAIFD_TX_DATA5    0xFFFFFFFFU
#define TWAIFD_TX_DATA5_M  (TWAIFD_TX_DATA5_V << TWAIFD_TX_DATA5_S)
#define TWAIFD_TX_DATA5_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA5_S  0

/** TWAIFD_TX_DATA_10_REG register
 *  TWAI FD transmit data register 10
 */
#define TWAIFD_TX_DATA_10_REG (DR_REG_TWAIFD_BASE + 0xc4)
/** TWAIFD_TX_DATA6 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 6th word to be transmitted
 */
#define TWAIFD_TX_DATA6    0xFFFFFFFFU
#define TWAIFD_TX_DATA6_M  (TWAIFD_TX_DATA6_V << TWAIFD_TX_DATA6_S)
#define TWAIFD_TX_DATA6_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA6_S  0

/** TWAIFD_TX_DATA_11_REG register
 *  TWAI FD transmit data register 11
 */
#define TWAIFD_TX_DATA_11_REG (DR_REG_TWAIFD_BASE + 0xc8)
/** TWAIFD_TX_DATA7 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 7th word to be transmitted
 */
#define TWAIFD_TX_DATA7    0xFFFFFFFFU
#define TWAIFD_TX_DATA7_M  (TWAIFD_TX_DATA7_V << TWAIFD_TX_DATA7_S)
#define TWAIFD_TX_DATA7_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA7_S  0

/** TWAIFD_TX_DATA_12_REG register
 *  TWAI FD transmit data register 12
 */
#define TWAIFD_TX_DATA_12_REG (DR_REG_TWAIFD_BASE + 0xcc)
/** TWAIFD_TX_DATA8 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 8th word to be transmitted
 */
#define TWAIFD_TX_DATA8    0xFFFFFFFFU
#define TWAIFD_TX_DATA8_M  (TWAIFD_TX_DATA8_V << TWAIFD_TX_DATA8_S)
#define TWAIFD_TX_DATA8_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA8_S  0

/** TWAIFD_TX_DATA_13_REG register
 *  TWAI FD transmit data register 13
 */
#define TWAIFD_TX_DATA_13_REG (DR_REG_TWAIFD_BASE + 0xd0)
/** TWAIFD_TX_DATA9 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 9th word to be transmitted
 */
#define TWAIFD_TX_DATA9    0xFFFFFFFFU
#define TWAIFD_TX_DATA9_M  (TWAIFD_TX_DATA9_V << TWAIFD_TX_DATA9_S)
#define TWAIFD_TX_DATA9_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA9_S  0

/** TWAIFD_TX_DATA_14_REG register
 *  TWAI FD transmit data register 14
 */
#define TWAIFD_TX_DATA_14_REG (DR_REG_TWAIFD_BASE + 0xd4)
/** TWAIFD_TX_DATA10 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 10th word to be transmitted
 */
#define TWAIFD_TX_DATA10    0xFFFFFFFFU
#define TWAIFD_TX_DATA10_M  (TWAIFD_TX_DATA10_V << TWAIFD_TX_DATA10_S)
#define TWAIFD_TX_DATA10_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA10_S  0

/** TWAIFD_TX_DATA_15_REG register
 *  TWAI FD transmit data register 15
 */
#define TWAIFD_TX_DATA_15_REG (DR_REG_TWAIFD_BASE + 0xd8)
/** TWAIFD_TX_DATA11 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 11th word to be transmitted
 */
#define TWAIFD_TX_DATA11    0xFFFFFFFFU
#define TWAIFD_TX_DATA11_M  (TWAIFD_TX_DATA11_V << TWAIFD_TX_DATA11_S)
#define TWAIFD_TX_DATA11_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA11_S  0

/** TWAIFD_TX_DATA_16_REG register
 *  TWAI FD transmit data register 16
 */
#define TWAIFD_TX_DATA_16_REG (DR_REG_TWAIFD_BASE + 0xdc)
/** TWAIFD_TX_DATA12 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 12th word to be transmitted
 */
#define TWAIFD_TX_DATA12    0xFFFFFFFFU
#define TWAIFD_TX_DATA12_M  (TWAIFD_TX_DATA12_V << TWAIFD_TX_DATA12_S)
#define TWAIFD_TX_DATA12_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA12_S  0

/** TWAIFD_TX_DATA_17_REG register
 *  TWAI FD transmit data register 17
 */
#define TWAIFD_TX_DATA_17_REG (DR_REG_TWAIFD_BASE + 0xe0)
/** TWAIFD_TX_DATA13 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 13th word to be transmitted
 */
#define TWAIFD_TX_DATA13    0xFFFFFFFFU
#define TWAIFD_TX_DATA13_M  (TWAIFD_TX_DATA13_V << TWAIFD_TX_DATA13_S)
#define TWAIFD_TX_DATA13_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA13_S  0

/** TWAIFD_TX_DATA_18_REG register
 *  TWAI FD transmit data register 18
 */
#define TWAIFD_TX_DATA_18_REG (DR_REG_TWAIFD_BASE + 0xe4)
/** TWAIFD_TX_DATA14 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 14th word to be transmitted
 */
#define TWAIFD_TX_DATA14    0xFFFFFFFFU
#define TWAIFD_TX_DATA14_M  (TWAIFD_TX_DATA14_V << TWAIFD_TX_DATA14_S)
#define TWAIFD_TX_DATA14_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA14_S  0

/** TWAIFD_TX_DATA_19_REG register
 *  TWAI FD transmit data register 19
 */
#define TWAIFD_TX_DATA_19_REG (DR_REG_TWAIFD_BASE + 0xe8)
/** TWAIFD_TX_DATA15 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 15th word to be transmitted
 */
#define TWAIFD_TX_DATA15    0xFFFFFFFFU
#define TWAIFD_TX_DATA15_M  (TWAIFD_TX_DATA15_V << TWAIFD_TX_DATA15_S)
#define TWAIFD_TX_DATA15_V  0xFFFFFFFFU
#define TWAIFD_TX_DATA15_S  0

/** TWAIFD_TX_CAMMAND_INFO_REG register
 *  TWAI FD TXT buffer command & information register
 */
#define TWAIFD_TX_CAMMAND_INFO_REG (DR_REG_TWAIFD_BASE + 0x14c)
/** TWAIFD_TXTB_SW_SET_ETY : R/W; bitpos: [0]; default: 0;
 *  a\\
 */
#define TWAIFD_TXTB_SW_SET_ETY    (BIT(0))
#define TWAIFD_TXTB_SW_SET_ETY_M  (TWAIFD_TXTB_SW_SET_ETY_V << TWAIFD_TXTB_SW_SET_ETY_S)
#define TWAIFD_TXTB_SW_SET_ETY_V  0x00000001U
#define TWAIFD_TXTB_SW_SET_ETY_S  0
/** TWAIFD_TXTB_SW_SET_RDY : R/W; bitpos: [1]; default: 0;
 *  a\\
 */
#define TWAIFD_TXTB_SW_SET_RDY    (BIT(1))
#define TWAIFD_TXTB_SW_SET_RDY_M  (TWAIFD_TXTB_SW_SET_RDY_V << TWAIFD_TXTB_SW_SET_RDY_S)
#define TWAIFD_TXTB_SW_SET_RDY_V  0x00000001U
#define TWAIFD_TXTB_SW_SET_RDY_S  1
/** TWAIFD_TXTB_SW_SET_ABT : R/W; bitpos: [2]; default: 0;
 *  a\\
 */
#define TWAIFD_TXTB_SW_SET_ABT    (BIT(2))
#define TWAIFD_TXTB_SW_SET_ABT_M  (TWAIFD_TXTB_SW_SET_ABT_V << TWAIFD_TXTB_SW_SET_ABT_S)
#define TWAIFD_TXTB_SW_SET_ABT_V  0x00000001U
#define TWAIFD_TXTB_SW_SET_ABT_S  2
/** TWAIFD_TXB1 : R/W; bitpos: [8]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB1    (BIT(8))
#define TWAIFD_TXB1_M  (TWAIFD_TXB1_V << TWAIFD_TXB1_S)
#define TWAIFD_TXB1_V  0x00000001U
#define TWAIFD_TXB1_S  8
/** TWAIFD_TXB2 : R/W; bitpos: [9]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB2    (BIT(9))
#define TWAIFD_TXB2_M  (TWAIFD_TXB2_V << TWAIFD_TXB2_S)
#define TWAIFD_TXB2_V  0x00000001U
#define TWAIFD_TXB2_S  9
/** TWAIFD_TXB3 : R/W; bitpos: [10]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB3    (BIT(10))
#define TWAIFD_TXB3_M  (TWAIFD_TXB3_V << TWAIFD_TXB3_S)
#define TWAIFD_TXB3_V  0x00000001U
#define TWAIFD_TXB3_S  10
/** TWAIFD_TXB4 : R/W; bitpos: [11]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB4    (BIT(11))
#define TWAIFD_TXB4_M  (TWAIFD_TXB4_V << TWAIFD_TXB4_S)
#define TWAIFD_TXB4_V  0x00000001U
#define TWAIFD_TXB4_S  11
/** TWAIFD_TXB5 : R/W; bitpos: [12]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB5    (BIT(12))
#define TWAIFD_TXB5_M  (TWAIFD_TXB5_V << TWAIFD_TXB5_S)
#define TWAIFD_TXB5_V  0x00000001U
#define TWAIFD_TXB5_S  12
/** TWAIFD_TXB6 : R/W; bitpos: [13]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB6    (BIT(13))
#define TWAIFD_TXB6_M  (TWAIFD_TXB6_V << TWAIFD_TXB6_S)
#define TWAIFD_TXB6_V  0x00000001U
#define TWAIFD_TXB6_S  13
/** TWAIFD_TXB7 : R/W; bitpos: [14]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB7    (BIT(14))
#define TWAIFD_TXB7_M  (TWAIFD_TXB7_V << TWAIFD_TXB7_S)
#define TWAIFD_TXB7_V  0x00000001U
#define TWAIFD_TXB7_S  14
/** TWAIFD_TXB8 : R/W; bitpos: [15]; default: 0;
 *  a\\
 */
#define TWAIFD_TXB8    (BIT(15))
#define TWAIFD_TXB8_M  (TWAIFD_TXB8_V << TWAIFD_TXB8_S)
#define TWAIFD_TXB8_V  0x00000001U
#define TWAIFD_TXB8_S  15
/** TWAIFD_TXT_BUF_CTR : R/W; bitpos: [19:16]; default: 0;
 *  a\\
 */
#define TWAIFD_TXT_BUF_CTR    0x0000000FU
#define TWAIFD_TXT_BUF_CTR_M  (TWAIFD_TXT_BUF_CTR_V << TWAIFD_TXT_BUF_CTR_S)
#define TWAIFD_TXT_BUF_CTR_V  0x0000000FU
#define TWAIFD_TXT_BUF_CTR_S  16

/** TWAIFD_ERR_CAP_RETR_CTR_ALC_REG register
 *  TWAI FD error capture & retransmit counter & arbitration lost register
 */
#define TWAIFD_ERR_CAP_RETR_CTR_ALC_REG (DR_REG_TWAIFD_BASE + 0x160)
/** TWAIFD_ERR_TYPE : RO; bitpos: [4:0]; default: 0;
 *  a\\
 */
#define TWAIFD_ERR_TYPE    0x0000001FU
#define TWAIFD_ERR_TYPE_M  (TWAIFD_ERR_TYPE_V << TWAIFD_ERR_TYPE_S)
#define TWAIFD_ERR_TYPE_V  0x0000001FU
#define TWAIFD_ERR_TYPE_S  0
/** TWAIFD_ERR_POS : RO; bitpos: [7:5]; default: 0;
 *  a\\
 */
#define TWAIFD_ERR_POS    0x00000007U
#define TWAIFD_ERR_POS_M  (TWAIFD_ERR_POS_V << TWAIFD_ERR_POS_S)
#define TWAIFD_ERR_POS_V  0x00000007U
#define TWAIFD_ERR_POS_S  5
/** TWAIFD_RETR_CTR : RO; bitpos: [11:8]; default: 0;
 *  a\\
 */
#define TWAIFD_RETR_CTR    0x0000000FU
#define TWAIFD_RETR_CTR_M  (TWAIFD_RETR_CTR_V << TWAIFD_RETR_CTR_S)
#define TWAIFD_RETR_CTR_V  0x0000000FU
#define TWAIFD_RETR_CTR_S  8
/** TWAIFD_ALC_BIT : RO; bitpos: [20:16]; default: 0;
 *  a\\
 */
#define TWAIFD_ALC_BIT    0x0000001FU
#define TWAIFD_ALC_BIT_M  (TWAIFD_ALC_BIT_V << TWAIFD_ALC_BIT_S)
#define TWAIFD_ALC_BIT_V  0x0000001FU
#define TWAIFD_ALC_BIT_S  16
/** TWAIFD_ALC_ID_FIELD : RO; bitpos: [23:21]; default: 0;
 *  a\\
 */
#define TWAIFD_ALC_ID_FIELD    0x00000007U
#define TWAIFD_ALC_ID_FIELD_M  (TWAIFD_ALC_ID_FIELD_V << TWAIFD_ALC_ID_FIELD_S)
#define TWAIFD_ALC_ID_FIELD_V  0x00000007U
#define TWAIFD_ALC_ID_FIELD_S  21

/** TWAIFD_TRV_DELAY_SSP_CFG_REG register
 *  TWAI FD transmit delay & secondary sample point configuration register
 */
#define TWAIFD_TRV_DELAY_SSP_CFG_REG (DR_REG_TWAIFD_BASE + 0x164)
/** TWAIFD_TRV_DELAY_VALUE : RO; bitpos: [6:0]; default: 0;
 *  a\\
 */
#define TWAIFD_TRV_DELAY_VALUE    0x0000007FU
#define TWAIFD_TRV_DELAY_VALUE_M  (TWAIFD_TRV_DELAY_VALUE_V << TWAIFD_TRV_DELAY_VALUE_S)
#define TWAIFD_TRV_DELAY_VALUE_V  0x0000007FU
#define TWAIFD_TRV_DELAY_VALUE_S  0
/** TWAIFD_SSP_OFFSET : R/W; bitpos: [23:16]; default: 10;
 *  a\\
 */
#define TWAIFD_SSP_OFFSET    0x000000FFU
#define TWAIFD_SSP_OFFSET_M  (TWAIFD_SSP_OFFSET_V << TWAIFD_SSP_OFFSET_S)
#define TWAIFD_SSP_OFFSET_V  0x000000FFU
#define TWAIFD_SSP_OFFSET_S  16
/** TWAIFD_SSP_SRC : R/W; bitpos: [25:24]; default: 0;
 *  a\\
 */
#define TWAIFD_SSP_SRC    0x00000003U
#define TWAIFD_SSP_SRC_M  (TWAIFD_SSP_SRC_V << TWAIFD_SSP_SRC_S)
#define TWAIFD_SSP_SRC_V  0x00000003U
#define TWAIFD_SSP_SRC_S  24

/** TWAIFD_RX_FRM_COUNTER_REG register
 *  TWAI FD received frame counter register
 */
#define TWAIFD_RX_FRM_COUNTER_REG (DR_REG_TWAIFD_BASE + 0x180)
/** TWAIFD_RX_COUNTER_VAL : RO; bitpos: [31:0]; default: 0;
 *  Configures the received frame counters to enable bus traffic measurement.
 */
#define TWAIFD_RX_COUNTER_VAL    0xFFFFFFFFU
#define TWAIFD_RX_COUNTER_VAL_M  (TWAIFD_RX_COUNTER_VAL_V << TWAIFD_RX_COUNTER_VAL_S)
#define TWAIFD_RX_COUNTER_VAL_V  0xFFFFFFFFU
#define TWAIFD_RX_COUNTER_VAL_S  0

/** TWAIFD_TX_FRM_COUNTER_REG register
 *  TWAI FD transmitted frame counter register
 */
#define TWAIFD_TX_FRM_COUNTER_REG (DR_REG_TWAIFD_BASE + 0x184)
/** TWAIFD_TX_COUNTER_VAL : RO; bitpos: [31:0]; default: 0;
 *  Configures the transcieved frame counters to enable bus traffic measurement.
 */
#define TWAIFD_TX_COUNTER_VAL    0xFFFFFFFFU
#define TWAIFD_TX_COUNTER_VAL_M  (TWAIFD_TX_COUNTER_VAL_V << TWAIFD_TX_COUNTER_VAL_S)
#define TWAIFD_TX_COUNTER_VAL_V  0xFFFFFFFFU
#define TWAIFD_TX_COUNTER_VAL_S  0

/** TWAIFD_CLK_REG register
 *  TWAI FD clock configuration register
 */
#define TWAIFD_CLK_REG (DR_REG_TWAIFD_BASE + 0x18c)
/** TWAIFD_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  1'b1: Force clock on for register. 1'b0: Support clock only when application writes
 *  registers.
 */
#define TWAIFD_CLK_EN    (BIT(31))
#define TWAIFD_CLK_EN_M  (TWAIFD_CLK_EN_V << TWAIFD_CLK_EN_S)
#define TWAIFD_CLK_EN_V  0x00000001U
#define TWAIFD_CLK_EN_S  31

/** TWAIFD_DATE_REG register
 *  TWAI FD version register
 */
#define TWAIFD_DATE_REG (DR_REG_TWAIFD_BASE + 0x190)
/** TWAIFD_TWAIFD_DATE : R/W; bitpos: [31:0]; default: 35717712;
 *  This is the version register.
 */
#define TWAIFD_TWAIFD_DATE    0xFFFFFFFFU
#define TWAIFD_TWAIFD_DATE_M  (TWAIFD_TWAIFD_DATE_V << TWAIFD_TWAIFD_DATE_S)
#define TWAIFD_TWAIFD_DATE_V  0xFFFFFFFFU
#define TWAIFD_TWAIFD_DATE_S  0

#ifdef __cplusplus
}
#endif
