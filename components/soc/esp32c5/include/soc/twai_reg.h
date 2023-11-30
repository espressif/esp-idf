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

/** TWAI_MODE_REG register
 *  TWAI mode register.
 */
#define TWAI_MODE_REG(i) (REG_TWAI_BASE(i) + 0x0)
/** TWAI_RESET_MODE : R/W; bitpos: [0]; default: 1;
 *  1: reset, detection of a set reset mode bit results in aborting the current
 *  transmission/reception of a message and entering the reset mode. 0: normal, on the
 *  '1-to-0' transition of the reset mode bit, the TWAI controller returns to the
 *  operating mode.
 */
#define TWAI_RESET_MODE    (BIT(0))
#define TWAI_RESET_MODE_M  (TWAI_RESET_MODE_V << TWAI_RESET_MODE_S)
#define TWAI_RESET_MODE_V  0x00000001U
#define TWAI_RESET_MODE_S  0
/** TWAI_LISTEN_ONLY_MODE : R/W; bitpos: [1]; default: 0;
 *  1: listen only, in this mode the TWAI controller would give no acknowledge to the
 *  TWAI-bus, even if a message is received successfully. The error counters are
 *  stopped at the current value. 0: normal.
 */
#define TWAI_LISTEN_ONLY_MODE    (BIT(1))
#define TWAI_LISTEN_ONLY_MODE_M  (TWAI_LISTEN_ONLY_MODE_V << TWAI_LISTEN_ONLY_MODE_S)
#define TWAI_LISTEN_ONLY_MODE_V  0x00000001U
#define TWAI_LISTEN_ONLY_MODE_S  1
/** TWAI_SELF_TEST_MODE : R/W; bitpos: [2]; default: 0;
 *  1: self test, in this mode a full node test is possible without any other active
 *  node on the bus using the self reception request command. The TWAI controller will
 *  perform a successful transmission, even if there is no acknowledge received. 0:
 *  normal, an acknowledge is required for successful transmission.
 */
#define TWAI_SELF_TEST_MODE    (BIT(2))
#define TWAI_SELF_TEST_MODE_M  (TWAI_SELF_TEST_MODE_V << TWAI_SELF_TEST_MODE_S)
#define TWAI_SELF_TEST_MODE_V  0x00000001U
#define TWAI_SELF_TEST_MODE_S  2
/** TWAI_ACCEPTANCE_FILTER_MODE : R/W; bitpos: [3]; default: 0;
 *  1:single, the single acceptance filter option is enabled (one filter with the
 *  length of 32 bit is active). 0:dual, the dual acceptance filter option is enabled
 *  (two filters, each with the length of 16 bit are active).
 */
#define TWAI_ACCEPTANCE_FILTER_MODE    (BIT(3))
#define TWAI_ACCEPTANCE_FILTER_MODE_M  (TWAI_ACCEPTANCE_FILTER_MODE_V << TWAI_ACCEPTANCE_FILTER_MODE_S)
#define TWAI_ACCEPTANCE_FILTER_MODE_V  0x00000001U
#define TWAI_ACCEPTANCE_FILTER_MODE_S  3

/** TWAI_CMD_REG register
 *  TWAI command register.
 */
#define TWAI_CMD_REG(i) (REG_TWAI_BASE(i) + 0x4)
/** TWAI_TX_REQUEST : WO; bitpos: [0]; default: 0;
 *  1: present, a message shall be transmitted. 0: absent
 */
#define TWAI_TX_REQUEST    (BIT(0))
#define TWAI_TX_REQUEST_M  (TWAI_TX_REQUEST_V << TWAI_TX_REQUEST_S)
#define TWAI_TX_REQUEST_V  0x00000001U
#define TWAI_TX_REQUEST_S  0
/** TWAI_ABORT_TX : WO; bitpos: [1]; default: 0;
 *  1: present, if not already in progress, a pending transmission request is
 *  cancelled. 0: absent
 */
#define TWAI_ABORT_TX    (BIT(1))
#define TWAI_ABORT_TX_M  (TWAI_ABORT_TX_V << TWAI_ABORT_TX_S)
#define TWAI_ABORT_TX_V  0x00000001U
#define TWAI_ABORT_TX_S  1
/** TWAI_RELEASE_BUFFER : WO; bitpos: [2]; default: 0;
 *  1: released, the receive buffer, representing the message memory space in the
 *  RXFIFO is released. 0: no action
 */
#define TWAI_RELEASE_BUFFER    (BIT(2))
#define TWAI_RELEASE_BUFFER_M  (TWAI_RELEASE_BUFFER_V << TWAI_RELEASE_BUFFER_S)
#define TWAI_RELEASE_BUFFER_V  0x00000001U
#define TWAI_RELEASE_BUFFER_S  2
/** TWAI_CLEAR_DATA_OVERRUN : WO; bitpos: [3]; default: 0;
 *  1: clear, the data overrun status bit is cleared. 0: no action.
 */
#define TWAI_CLEAR_DATA_OVERRUN    (BIT(3))
#define TWAI_CLEAR_DATA_OVERRUN_M  (TWAI_CLEAR_DATA_OVERRUN_V << TWAI_CLEAR_DATA_OVERRUN_S)
#define TWAI_CLEAR_DATA_OVERRUN_V  0x00000001U
#define TWAI_CLEAR_DATA_OVERRUN_S  3
/** TWAI_SELF_RX_REQUEST : WO; bitpos: [4]; default: 0;
 *  1: present, a message shall be transmitted and received simultaneously. 0: absent.
 */
#define TWAI_SELF_RX_REQUEST    (BIT(4))
#define TWAI_SELF_RX_REQUEST_M  (TWAI_SELF_RX_REQUEST_V << TWAI_SELF_RX_REQUEST_S)
#define TWAI_SELF_RX_REQUEST_V  0x00000001U
#define TWAI_SELF_RX_REQUEST_S  4

/** TWAI_STATUS_REG register
 *  TWAI status register.
 */
#define TWAI_STATUS_REG(i) (REG_TWAI_BASE(i) + 0x8)
/** TWAI_STATUS_RECEIVE_BUFFER : RO; bitpos: [0]; default: 0;
 *  1: full, one or more complete messages are available in the RXFIFO. 0: empty, no
 *  message is available
 */
#define TWAI_STATUS_RECEIVE_BUFFER    (BIT(0))
#define TWAI_STATUS_RECEIVE_BUFFER_M  (TWAI_STATUS_RECEIVE_BUFFER_V << TWAI_STATUS_RECEIVE_BUFFER_S)
#define TWAI_STATUS_RECEIVE_BUFFER_V  0x00000001U
#define TWAI_STATUS_RECEIVE_BUFFER_S  0
/** TWAI_STATUS_OVERRUN : RO; bitpos: [1]; default: 0;
 *  1: overrun, a message was lost because there was not enough space for that message
 *  in the RXFIFO. 0: absent, no data overrun has occurred since the last clear data
 *  overrun command was given
 */
#define TWAI_STATUS_OVERRUN    (BIT(1))
#define TWAI_STATUS_OVERRUN_M  (TWAI_STATUS_OVERRUN_V << TWAI_STATUS_OVERRUN_S)
#define TWAI_STATUS_OVERRUN_V  0x00000001U
#define TWAI_STATUS_OVERRUN_S  1
/** TWAI_STATUS_TRANSMIT_BUFFER : RO; bitpos: [2]; default: 0;
 *  1: released, the CPU may write a message into the transmit buffer. 0: locked, the
 *  CPU cannot access the transmit buffer, a message is either waiting for transmission
 *  or is in the process of being transmitted
 */
#define TWAI_STATUS_TRANSMIT_BUFFER    (BIT(2))
#define TWAI_STATUS_TRANSMIT_BUFFER_M  (TWAI_STATUS_TRANSMIT_BUFFER_V << TWAI_STATUS_TRANSMIT_BUFFER_S)
#define TWAI_STATUS_TRANSMIT_BUFFER_V  0x00000001U
#define TWAI_STATUS_TRANSMIT_BUFFER_S  2
/** TWAI_STATUS_TRANSMISSION_COMPLETE : RO; bitpos: [3]; default: 0;
 *  1: complete, last requested transmission has been successfully completed. 0:
 *  incomplete, previously requested transmission is not yet completed
 */
#define TWAI_STATUS_TRANSMISSION_COMPLETE    (BIT(3))
#define TWAI_STATUS_TRANSMISSION_COMPLETE_M  (TWAI_STATUS_TRANSMISSION_COMPLETE_V << TWAI_STATUS_TRANSMISSION_COMPLETE_S)
#define TWAI_STATUS_TRANSMISSION_COMPLETE_V  0x00000001U
#define TWAI_STATUS_TRANSMISSION_COMPLETE_S  3
/** TWAI_STATUS_RECEIVE : RO; bitpos: [4]; default: 0;
 *  1: receive, the TWAI controller is receiving a message. 0: idle
 */
#define TWAI_STATUS_RECEIVE    (BIT(4))
#define TWAI_STATUS_RECEIVE_M  (TWAI_STATUS_RECEIVE_V << TWAI_STATUS_RECEIVE_S)
#define TWAI_STATUS_RECEIVE_V  0x00000001U
#define TWAI_STATUS_RECEIVE_S  4
/** TWAI_STATUS_TRANSMIT : RO; bitpos: [5]; default: 0;
 *  1: transmit, the TWAI controller is transmitting a message. 0: idle
 */
#define TWAI_STATUS_TRANSMIT    (BIT(5))
#define TWAI_STATUS_TRANSMIT_M  (TWAI_STATUS_TRANSMIT_V << TWAI_STATUS_TRANSMIT_S)
#define TWAI_STATUS_TRANSMIT_V  0x00000001U
#define TWAI_STATUS_TRANSMIT_S  5
/** TWAI_STATUS_ERR : RO; bitpos: [6]; default: 0;
 *  1: error, at least one of the error counters has reached or exceeded the CPU
 *  warning limit defined by the Error Warning Limit Register (EWLR). 0: ok, both error
 *  counters are below the warning limit
 */
#define TWAI_STATUS_ERR    (BIT(6))
#define TWAI_STATUS_ERR_M  (TWAI_STATUS_ERR_V << TWAI_STATUS_ERR_S)
#define TWAI_STATUS_ERR_V  0x00000001U
#define TWAI_STATUS_ERR_S  6
/** TWAI_STATUS_NODE_BUS_OFF : RO; bitpos: [7]; default: 0;
 *  1: bus-off, the TWAI controller is not involved in bus activities. 0: bus-on, the
 *  TWAI controller is involved in bus activities
 */
#define TWAI_STATUS_NODE_BUS_OFF    (BIT(7))
#define TWAI_STATUS_NODE_BUS_OFF_M  (TWAI_STATUS_NODE_BUS_OFF_V << TWAI_STATUS_NODE_BUS_OFF_S)
#define TWAI_STATUS_NODE_BUS_OFF_V  0x00000001U
#define TWAI_STATUS_NODE_BUS_OFF_S  7
/** TWAI_STATUS_MISS : RO; bitpos: [8]; default: 0;
 *  1: current message is destroyed because of FIFO overflow.
 */
#define TWAI_STATUS_MISS    (BIT(8))
#define TWAI_STATUS_MISS_M  (TWAI_STATUS_MISS_V << TWAI_STATUS_MISS_S)
#define TWAI_STATUS_MISS_V  0x00000001U
#define TWAI_STATUS_MISS_S  8

/** TWAI_INTERRUPT_REG register
 *  Interrupt signals' register.
 */
#define TWAI_INTERRUPT_REG(i) (REG_TWAI_BASE(i) + 0xc)
/** TWAI_RECEIVE_INT_ST : RO; bitpos: [0]; default: 0;
 *  1: this bit is set while the receive FIFO is not empty and the RIE bit is set
 *  within the interrupt enable register. 0: reset
 */
#define TWAI_RECEIVE_INT_ST    (BIT(0))
#define TWAI_RECEIVE_INT_ST_M  (TWAI_RECEIVE_INT_ST_V << TWAI_RECEIVE_INT_ST_S)
#define TWAI_RECEIVE_INT_ST_V  0x00000001U
#define TWAI_RECEIVE_INT_ST_S  0
/** TWAI_TRANSMIT_INT_ST : RO; bitpos: [1]; default: 0;
 *  1: this bit is set whenever the transmit buffer status changes from '0-to-1'
 *  (released) and the TIE bit is set within the interrupt enable register. 0: reset
 */
#define TWAI_TRANSMIT_INT_ST    (BIT(1))
#define TWAI_TRANSMIT_INT_ST_M  (TWAI_TRANSMIT_INT_ST_V << TWAI_TRANSMIT_INT_ST_S)
#define TWAI_TRANSMIT_INT_ST_V  0x00000001U
#define TWAI_TRANSMIT_INT_ST_S  1
/** TWAI_ERR_WARNING_INT_ST : RO; bitpos: [2]; default: 0;
 *  1: this bit is set on every change (set and clear) of either the error status or
 *  bus status bits and the EIE bit is set within the interrupt enable register. 0:
 *  reset
 */
#define TWAI_ERR_WARNING_INT_ST    (BIT(2))
#define TWAI_ERR_WARNING_INT_ST_M  (TWAI_ERR_WARNING_INT_ST_V << TWAI_ERR_WARNING_INT_ST_S)
#define TWAI_ERR_WARNING_INT_ST_V  0x00000001U
#define TWAI_ERR_WARNING_INT_ST_S  2
/** TWAI_DATA_OVERRUN_INT_ST : RO; bitpos: [3]; default: 0;
 *  1: this bit is set on a '0-to-1' transition of the data overrun status bit and the
 *  DOIE bit is set within the interrupt enable register. 0: reset
 */
#define TWAI_DATA_OVERRUN_INT_ST    (BIT(3))
#define TWAI_DATA_OVERRUN_INT_ST_M  (TWAI_DATA_OVERRUN_INT_ST_V << TWAI_DATA_OVERRUN_INT_ST_S)
#define TWAI_DATA_OVERRUN_INT_ST_V  0x00000001U
#define TWAI_DATA_OVERRUN_INT_ST_S  3
/** TWAI_TS_COUNTER_OVFL_INT_ST : RO; bitpos: [4]; default: 0;
 *  1: this bit is set then the timestamp counter reaches the maximum value and
 *  overflow.
 */
#define TWAI_TS_COUNTER_OVFL_INT_ST    (BIT(4))
#define TWAI_TS_COUNTER_OVFL_INT_ST_M  (TWAI_TS_COUNTER_OVFL_INT_ST_V << TWAI_TS_COUNTER_OVFL_INT_ST_S)
#define TWAI_TS_COUNTER_OVFL_INT_ST_V  0x00000001U
#define TWAI_TS_COUNTER_OVFL_INT_ST_S  4
/** TWAI_ERR_PASSIVE_INT_ST : RO; bitpos: [5]; default: 0;
 *  1: this bit is set whenever the TWAI controller has reached the error passive
 *  status (at least one error counter exceeds the protocol-defined level of 127) or if
 *  the TWAI controller is in the error passive status and enters the error active
 *  status again and the EPIE bit is set within the interrupt enable register. 0: reset
 */
#define TWAI_ERR_PASSIVE_INT_ST    (BIT(5))
#define TWAI_ERR_PASSIVE_INT_ST_M  (TWAI_ERR_PASSIVE_INT_ST_V << TWAI_ERR_PASSIVE_INT_ST_S)
#define TWAI_ERR_PASSIVE_INT_ST_V  0x00000001U
#define TWAI_ERR_PASSIVE_INT_ST_S  5
/** TWAI_ARBITRATION_LOST_INT_ST : RO; bitpos: [6]; default: 0;
 *  1: this bit is set when the TWAI controller lost the arbitration and becomes a
 *  receiver and the ALIE bit is set within the interrupt enable register. 0: reset
 */
#define TWAI_ARBITRATION_LOST_INT_ST    (BIT(6))
#define TWAI_ARBITRATION_LOST_INT_ST_M  (TWAI_ARBITRATION_LOST_INT_ST_V << TWAI_ARBITRATION_LOST_INT_ST_S)
#define TWAI_ARBITRATION_LOST_INT_ST_V  0x00000001U
#define TWAI_ARBITRATION_LOST_INT_ST_S  6
/** TWAI_BUS_ERR_INT_ST : RO; bitpos: [7]; default: 0;
 *  1: this bit is set when the TWAI controller detects an error on the TWAI-bus and
 *  the BEIE bit is set within the interrupt enable register. 0: reset
 */
#define TWAI_BUS_ERR_INT_ST    (BIT(7))
#define TWAI_BUS_ERR_INT_ST_M  (TWAI_BUS_ERR_INT_ST_V << TWAI_BUS_ERR_INT_ST_S)
#define TWAI_BUS_ERR_INT_ST_V  0x00000001U
#define TWAI_BUS_ERR_INT_ST_S  7
/** TWAI_IDLE_INT_ST : RO; bitpos: [8]; default: 0;
 *  1: this bit is set when the TWAI controller detects state of TWAI become IDLE and
 *  this interrupt enable bit is set within the interrupt enable register. 0: reset
 */
#define TWAI_IDLE_INT_ST    (BIT(8))
#define TWAI_IDLE_INT_ST_M  (TWAI_IDLE_INT_ST_V << TWAI_IDLE_INT_ST_S)
#define TWAI_IDLE_INT_ST_V  0x00000001U
#define TWAI_IDLE_INT_ST_S  8

/** TWAI_INTERRUPT_ENABLE_REG register
 *  Interrupt enable register.
 */
#define TWAI_INTERRUPT_ENABLE_REG(i) (REG_TWAI_BASE(i) + 0x10)
/** TWAI_EXT_RECEIVE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  1: enabled, when the receive buffer status is 'full' the TWAI controller requests
 *  the respective interrupt. 0: disable
 */
#define TWAI_EXT_RECEIVE_INT_ENA    (BIT(0))
#define TWAI_EXT_RECEIVE_INT_ENA_M  (TWAI_EXT_RECEIVE_INT_ENA_V << TWAI_EXT_RECEIVE_INT_ENA_S)
#define TWAI_EXT_RECEIVE_INT_ENA_V  0x00000001U
#define TWAI_EXT_RECEIVE_INT_ENA_S  0
/** TWAI_EXT_TRANSMIT_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  1: enabled, when a message has been successfully transmitted or the transmit buffer
 *  is accessible again (e.g. after an abort transmission command), the TWAI controller
 *  requests the respective interrupt. 0: disable
 */
#define TWAI_EXT_TRANSMIT_INT_ENA    (BIT(1))
#define TWAI_EXT_TRANSMIT_INT_ENA_M  (TWAI_EXT_TRANSMIT_INT_ENA_V << TWAI_EXT_TRANSMIT_INT_ENA_S)
#define TWAI_EXT_TRANSMIT_INT_ENA_V  0x00000001U
#define TWAI_EXT_TRANSMIT_INT_ENA_S  1
/** TWAI_EXT_ERR_WARNING_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  1: enabled, if the error or bus status change (see status register. Table 14), the
 *  TWAI controllerrequests the respective interrupt. 0: disable
 */
#define TWAI_EXT_ERR_WARNING_INT_ENA    (BIT(2))
#define TWAI_EXT_ERR_WARNING_INT_ENA_M  (TWAI_EXT_ERR_WARNING_INT_ENA_V << TWAI_EXT_ERR_WARNING_INT_ENA_S)
#define TWAI_EXT_ERR_WARNING_INT_ENA_V  0x00000001U
#define TWAI_EXT_ERR_WARNING_INT_ENA_S  2
/** TWAI_EXT_DATA_OVERRUN_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  1: enabled, if the data overrun status bit is set (see status register. Table 14),
 *  the TWAI controllerrequests the respective interrupt. 0: disable
 */
#define TWAI_EXT_DATA_OVERRUN_INT_ENA    (BIT(3))
#define TWAI_EXT_DATA_OVERRUN_INT_ENA_M  (TWAI_EXT_DATA_OVERRUN_INT_ENA_V << TWAI_EXT_DATA_OVERRUN_INT_ENA_S)
#define TWAI_EXT_DATA_OVERRUN_INT_ENA_V  0x00000001U
#define TWAI_EXT_DATA_OVERRUN_INT_ENA_S  3
/** TWAI_TS_COUNTER_OVFL_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  enable the timestamp counter overflow interrupt request.
 */
#define TWAI_TS_COUNTER_OVFL_INT_ENA    (BIT(4))
#define TWAI_TS_COUNTER_OVFL_INT_ENA_M  (TWAI_TS_COUNTER_OVFL_INT_ENA_V << TWAI_TS_COUNTER_OVFL_INT_ENA_S)
#define TWAI_TS_COUNTER_OVFL_INT_ENA_V  0x00000001U
#define TWAI_TS_COUNTER_OVFL_INT_ENA_S  4
/** TWAI_ERR_PASSIVE_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  1: enabled, if the error status of the TWAI controller changes from error active to
 *  error passive or vice versa, the respective interrupt is requested. 0: disable
 */
#define TWAI_ERR_PASSIVE_INT_ENA    (BIT(5))
#define TWAI_ERR_PASSIVE_INT_ENA_M  (TWAI_ERR_PASSIVE_INT_ENA_V << TWAI_ERR_PASSIVE_INT_ENA_S)
#define TWAI_ERR_PASSIVE_INT_ENA_V  0x00000001U
#define TWAI_ERR_PASSIVE_INT_ENA_S  5
/** TWAI_ARBITRATION_LOST_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  1: enabled, if the TWAI controller has lost arbitration, the respective interrupt
 *  is requested. 0: disable
 */
#define TWAI_ARBITRATION_LOST_INT_ENA    (BIT(6))
#define TWAI_ARBITRATION_LOST_INT_ENA_M  (TWAI_ARBITRATION_LOST_INT_ENA_V << TWAI_ARBITRATION_LOST_INT_ENA_S)
#define TWAI_ARBITRATION_LOST_INT_ENA_V  0x00000001U
#define TWAI_ARBITRATION_LOST_INT_ENA_S  6
/** TWAI_BUS_ERR_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  1: enabled, if an bus error has been detected, the TWAI controller requests the
 *  respective interrupt. 0: disable
 */
#define TWAI_BUS_ERR_INT_ENA    (BIT(7))
#define TWAI_BUS_ERR_INT_ENA_M  (TWAI_BUS_ERR_INT_ENA_V << TWAI_BUS_ERR_INT_ENA_S)
#define TWAI_BUS_ERR_INT_ENA_V  0x00000001U
#define TWAI_BUS_ERR_INT_ENA_S  7
/** TWAI_IDLE_INT_ENA : RO; bitpos: [8]; default: 0;
 *  1: enabled, if state of TWAI become IDLE, the TWAI controller requests the
 *  respective interrupt. 0: disable
 */
#define TWAI_IDLE_INT_ENA    (BIT(8))
#define TWAI_IDLE_INT_ENA_M  (TWAI_IDLE_INT_ENA_V << TWAI_IDLE_INT_ENA_S)
#define TWAI_IDLE_INT_ENA_V  0x00000001U
#define TWAI_IDLE_INT_ENA_S  8

/** TWAI_BUS_TIMING_0_REG register
 *  Bit timing configuration register 0.
 */
#define TWAI_BUS_TIMING_0_REG(i) (REG_TWAI_BASE(i) + 0x18)
/** TWAI_BAUD_PRESC : R/W; bitpos: [13:0]; default: 0;
 *  The period of the TWAI system clock is programmable and determines the individual
 *  bit timing. Software has R/W permission in reset mode and RO permission in
 *  operation mode.
 */
#define TWAI_BAUD_PRESC    0x00003FFFU
#define TWAI_BAUD_PRESC_M  (TWAI_BAUD_PRESC_V << TWAI_BAUD_PRESC_S)
#define TWAI_BAUD_PRESC_V  0x00003FFFU
#define TWAI_BAUD_PRESC_S  0
/** TWAI_SYNC_JUMP_WIDTH : R/W; bitpos: [15:14]; default: 0;
 *  The synchronization jump width defines the maximum number of clock cycles a bit
 *  period may be shortened or lengthened. Software has R/W permission in reset mode
 *  and RO in operation mode.
 */
#define TWAI_SYNC_JUMP_WIDTH    0x00000003U
#define TWAI_SYNC_JUMP_WIDTH_M  (TWAI_SYNC_JUMP_WIDTH_V << TWAI_SYNC_JUMP_WIDTH_S)
#define TWAI_SYNC_JUMP_WIDTH_V  0x00000003U
#define TWAI_SYNC_JUMP_WIDTH_S  14

/** TWAI_BUS_TIMING_1_REG register
 *  Bit timing configuration register 1.
 */
#define TWAI_BUS_TIMING_1_REG(i) (REG_TWAI_BASE(i) + 0x1c)
/** TWAI_TIME_SEGMENT1 : R/W; bitpos: [3:0]; default: 0;
 *  The number of clock cycles in TSEG1 per bit timing. Software has R/W permission in
 *  reset mode and RO in operation mode.
 */
#define TWAI_TIME_SEGMENT1    0x0000000FU
#define TWAI_TIME_SEGMENT1_M  (TWAI_TIME_SEGMENT1_V << TWAI_TIME_SEGMENT1_S)
#define TWAI_TIME_SEGMENT1_V  0x0000000FU
#define TWAI_TIME_SEGMENT1_S  0
/** TWAI_TIME_SEGMENT2 : R/W; bitpos: [6:4]; default: 0;
 *  The number of clock cycles in TSEG2 per bit timing. Software has R/W permission in
 *  reset mode and RO in operation mode.
 */
#define TWAI_TIME_SEGMENT2    0x00000007U
#define TWAI_TIME_SEGMENT2_M  (TWAI_TIME_SEGMENT2_V << TWAI_TIME_SEGMENT2_S)
#define TWAI_TIME_SEGMENT2_V  0x00000007U
#define TWAI_TIME_SEGMENT2_S  4
/** TWAI_TIME_SAMPLING : R/W; bitpos: [7]; default: 0;
 *  1: triple, the bus is sampled three times. 0: single, the bus is sampled once.
 *  Software has R/W permission in reset mode and RO in operation mode.
 */
#define TWAI_TIME_SAMPLING    (BIT(7))
#define TWAI_TIME_SAMPLING_M  (TWAI_TIME_SAMPLING_V << TWAI_TIME_SAMPLING_S)
#define TWAI_TIME_SAMPLING_V  0x00000001U
#define TWAI_TIME_SAMPLING_S  7

/** TWAI_ARB_LOST_CAP_REG register
 *  TWAI arbiter lost capture register.
 */
#define TWAI_ARB_LOST_CAP_REG(i) (REG_TWAI_BASE(i) + 0x2c)
/** TWAI_ARBITRATION_LOST_CAPTURE : RO; bitpos: [4:0]; default: 0;
 *  This register contains information about the bit position of losing arbitration.
 */
#define TWAI_ARBITRATION_LOST_CAPTURE    0x0000001FU
#define TWAI_ARBITRATION_LOST_CAPTURE_M  (TWAI_ARBITRATION_LOST_CAPTURE_V << TWAI_ARBITRATION_LOST_CAPTURE_S)
#define TWAI_ARBITRATION_LOST_CAPTURE_V  0x0000001FU
#define TWAI_ARBITRATION_LOST_CAPTURE_S  0

/** TWAI_ERR_CODE_CAP_REG register
 *  TWAI error info capture register.
 */
#define TWAI_ERR_CODE_CAP_REG(i) (REG_TWAI_BASE(i) + 0x30)
/** TWAI_ERR_CAPTURE_CODE_SEGMENT : RO; bitpos: [4:0]; default: 0;
 *  This register contains information about the location of errors on the bus.
 */
#define TWAI_ERR_CAPTURE_CODE_SEGMENT    0x0000001FU
#define TWAI_ERR_CAPTURE_CODE_SEGMENT_M  (TWAI_ERR_CAPTURE_CODE_SEGMENT_V << TWAI_ERR_CAPTURE_CODE_SEGMENT_S)
#define TWAI_ERR_CAPTURE_CODE_SEGMENT_V  0x0000001FU
#define TWAI_ERR_CAPTURE_CODE_SEGMENT_S  0
/** TWAI_ERR_CAPTURE_CODE_DIRECTION : RO; bitpos: [5]; default: 0;
 *  1: RX, error occurred during reception. 0: TX, error occurred during transmission.
 */
#define TWAI_ERR_CAPTURE_CODE_DIRECTION    (BIT(5))
#define TWAI_ERR_CAPTURE_CODE_DIRECTION_M  (TWAI_ERR_CAPTURE_CODE_DIRECTION_V << TWAI_ERR_CAPTURE_CODE_DIRECTION_S)
#define TWAI_ERR_CAPTURE_CODE_DIRECTION_V  0x00000001U
#define TWAI_ERR_CAPTURE_CODE_DIRECTION_S  5
/** TWAI_ERR_CAPTURE_CODE_TYPE : RO; bitpos: [7:6]; default: 0;
 *  00: bit error. 01: form error. 10:stuff error. 11:other type of error.
 */
#define TWAI_ERR_CAPTURE_CODE_TYPE    0x00000003U
#define TWAI_ERR_CAPTURE_CODE_TYPE_M  (TWAI_ERR_CAPTURE_CODE_TYPE_V << TWAI_ERR_CAPTURE_CODE_TYPE_S)
#define TWAI_ERR_CAPTURE_CODE_TYPE_V  0x00000003U
#define TWAI_ERR_CAPTURE_CODE_TYPE_S  6

/** TWAI_ERR_WARNING_LIMIT_REG register
 *  TWAI error threshold configuration register.
 */
#define TWAI_ERR_WARNING_LIMIT_REG(i) (REG_TWAI_BASE(i) + 0x34)
/** TWAI_ERR_WARNING_LIMIT : R/W; bitpos: [7:0]; default: 96;
 *  The threshold that trigger error warning interrupt when this interrupt is enabled.
 *  Software has R/W permission in reset mode and RO in operation mode.
 */
#define TWAI_ERR_WARNING_LIMIT    0x000000FFU
#define TWAI_ERR_WARNING_LIMIT_M  (TWAI_ERR_WARNING_LIMIT_V << TWAI_ERR_WARNING_LIMIT_S)
#define TWAI_ERR_WARNING_LIMIT_V  0x000000FFU
#define TWAI_ERR_WARNING_LIMIT_S  0

/** TWAI_RX_ERR_CNT_REG register
 *  Rx error counter register.
 */
#define TWAI_RX_ERR_CNT_REG(i) (REG_TWAI_BASE(i) + 0x38)
/** TWAI_RX_ERR_CNT : R/W; bitpos: [7:0]; default: 0;
 *  The RX error counter register reflects the current value of the transmit error
 *  counter. Software has R/W permission in reset mode and RO in operation mode.
 */
#define TWAI_RX_ERR_CNT    0x000000FFU
#define TWAI_RX_ERR_CNT_M  (TWAI_RX_ERR_CNT_V << TWAI_RX_ERR_CNT_S)
#define TWAI_RX_ERR_CNT_V  0x000000FFU
#define TWAI_RX_ERR_CNT_S  0

/** TWAI_TX_ERR_CNT_REG register
 *  Tx error counter register.
 */
#define TWAI_TX_ERR_CNT_REG(i) (REG_TWAI_BASE(i) + 0x3c)
/** TWAI_TX_ERR_CNT : R/W; bitpos: [7:0]; default: 0;
 *  The TX error counter register reflects the current value of the transmit error
 *  counter. Software has R/W permission in reset mode and RO in operation mode.
 */
#define TWAI_TX_ERR_CNT    0x000000FFU
#define TWAI_TX_ERR_CNT_M  (TWAI_TX_ERR_CNT_V << TWAI_TX_ERR_CNT_S)
#define TWAI_TX_ERR_CNT_V  0x000000FFU
#define TWAI_TX_ERR_CNT_S  0

/** TWAI_DATA_0_REG register
 *  Data register 0.
 */
#define TWAI_DATA_0_REG(i) (REG_TWAI_BASE(i) + 0x40)
/** TWAI_DATA_0 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance code register 0 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 0 and when
 *  software initiate read operation, it is rx data register 0.
 */
#define TWAI_DATA_0    0x000000FFU
#define TWAI_DATA_0_M  (TWAI_DATA_0_V << TWAI_DATA_0_S)
#define TWAI_DATA_0_V  0x000000FFU
#define TWAI_DATA_0_S  0

/** TWAI_DATA_1_REG register
 *  Data register 1.
 */
#define TWAI_DATA_1_REG(i) (REG_TWAI_BASE(i) + 0x44)
/** TWAI_DATA_1 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance code register 1 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 1 and when
 *  software initiate read operation, it is rx data register 1.
 */
#define TWAI_DATA_1    0x000000FFU
#define TWAI_DATA_1_M  (TWAI_DATA_1_V << TWAI_DATA_1_S)
#define TWAI_DATA_1_V  0x000000FFU
#define TWAI_DATA_1_S  0

/** TWAI_DATA_2_REG register
 *  Data register 2.
 */
#define TWAI_DATA_2_REG(i) (REG_TWAI_BASE(i) + 0x48)
/** TWAI_DATA_2 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance code register 2 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 2 and when
 *  software initiate read operation, it is rx data register 2.
 */
#define TWAI_DATA_2    0x000000FFU
#define TWAI_DATA_2_M  (TWAI_DATA_2_V << TWAI_DATA_2_S)
#define TWAI_DATA_2_V  0x000000FFU
#define TWAI_DATA_2_S  0

/** TWAI_DATA_3_REG register
 *  Data register 3.
 */
#define TWAI_DATA_3_REG(i) (REG_TWAI_BASE(i) + 0x4c)
/** TWAI_DATA_3 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance code register 3 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 3 and when
 *  software initiate read operation, it is rx data register 3.
 */
#define TWAI_DATA_3    0x000000FFU
#define TWAI_DATA_3_M  (TWAI_DATA_3_V << TWAI_DATA_3_S)
#define TWAI_DATA_3_V  0x000000FFU
#define TWAI_DATA_3_S  0

/** TWAI_DATA_4_REG register
 *  Data register 4.
 */
#define TWAI_DATA_4_REG(i) (REG_TWAI_BASE(i) + 0x50)
/** TWAI_DATA_4 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance mask register 0 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 4 and when
 *  software initiate read operation, it is rx data register 4.
 */
#define TWAI_DATA_4    0x000000FFU
#define TWAI_DATA_4_M  (TWAI_DATA_4_V << TWAI_DATA_4_S)
#define TWAI_DATA_4_V  0x000000FFU
#define TWAI_DATA_4_S  0

/** TWAI_DATA_5_REG register
 *  Data register 5.
 */
#define TWAI_DATA_5_REG(i) (REG_TWAI_BASE(i) + 0x54)
/** TWAI_DATA_5 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance mask register 1 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 5 and when
 *  software initiate read operation, it is rx data register 5.
 */
#define TWAI_DATA_5    0x000000FFU
#define TWAI_DATA_5_M  (TWAI_DATA_5_V << TWAI_DATA_5_S)
#define TWAI_DATA_5_V  0x000000FFU
#define TWAI_DATA_5_S  0

/** TWAI_DATA_6_REG register
 *  Data register 6.
 */
#define TWAI_DATA_6_REG(i) (REG_TWAI_BASE(i) + 0x58)
/** TWAI_DATA_6 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance mask register 2 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 6 and when
 *  software initiate read operation, it is rx data register 6.
 */
#define TWAI_DATA_6    0x000000FFU
#define TWAI_DATA_6_M  (TWAI_DATA_6_V << TWAI_DATA_6_S)
#define TWAI_DATA_6_V  0x000000FFU
#define TWAI_DATA_6_S  0

/** TWAI_DATA_7_REG register
 *  Data register 7.
 */
#define TWAI_DATA_7_REG(i) (REG_TWAI_BASE(i) + 0x5c)
/** TWAI_DATA_7 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, it is acceptance mask register 3 with R/W Permission. In operation
 *  mode, when software initiate write operation, it is tx data register 7 and when
 *  software initiate read operation, it is rx data register 7.
 */
#define TWAI_DATA_7    0x000000FFU
#define TWAI_DATA_7_M  (TWAI_DATA_7_V << TWAI_DATA_7_S)
#define TWAI_DATA_7_V  0x000000FFU
#define TWAI_DATA_7_S  0

/** TWAI_DATA_8_REG register
 *  Data register 8.
 */
#define TWAI_DATA_8_REG(i) (REG_TWAI_BASE(i) + 0x60)
/** TWAI_DATA_8 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, reserved with RO. In operation mode, when software initiate write
 *  operation, it is tx data register 8 and when software initiate read operation, it
 *  is rx data register 8.
 */
#define TWAI_DATA_8    0x000000FFU
#define TWAI_DATA_8_M  (TWAI_DATA_8_V << TWAI_DATA_8_S)
#define TWAI_DATA_8_V  0x000000FFU
#define TWAI_DATA_8_S  0

/** TWAI_DATA_9_REG register
 *  Data register 9.
 */
#define TWAI_DATA_9_REG(i) (REG_TWAI_BASE(i) + 0x64)
/** TWAI_DATA_9 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, reserved with RO. In operation mode, when software initiate write
 *  operation, it is tx data register 9 and when software initiate read operation, it
 *  is rx data register 9.
 */
#define TWAI_DATA_9    0x000000FFU
#define TWAI_DATA_9_M  (TWAI_DATA_9_V << TWAI_DATA_9_S)
#define TWAI_DATA_9_V  0x000000FFU
#define TWAI_DATA_9_S  0

/** TWAI_DATA_10_REG register
 *  Data register 10.
 */
#define TWAI_DATA_10_REG(i) (REG_TWAI_BASE(i) + 0x68)
/** TWAI_DATA_10 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, reserved with RO. In operation mode, when software initiate write
 *  operation, it is tx data register 10 and when software initiate read operation, it
 *  is rx data register 10.
 */
#define TWAI_DATA_10    0x000000FFU
#define TWAI_DATA_10_M  (TWAI_DATA_10_V << TWAI_DATA_10_S)
#define TWAI_DATA_10_V  0x000000FFU
#define TWAI_DATA_10_S  0

/** TWAI_DATA_11_REG register
 *  Data register 11.
 */
#define TWAI_DATA_11_REG(i) (REG_TWAI_BASE(i) + 0x6c)
/** TWAI_DATA_11 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, reserved with RO. In operation mode, when software initiate write
 *  operation, it is tx data register 11 and when software initiate read operation, it
 *  is rx data register 11.
 */
#define TWAI_DATA_11    0x000000FFU
#define TWAI_DATA_11_M  (TWAI_DATA_11_V << TWAI_DATA_11_S)
#define TWAI_DATA_11_V  0x000000FFU
#define TWAI_DATA_11_S  0

/** TWAI_DATA_12_REG register
 *  Data register 12.
 */
#define TWAI_DATA_12_REG(i) (REG_TWAI_BASE(i) + 0x70)
/** TWAI_DATA_12 : R/W; bitpos: [7:0]; default: 0;
 *  In reset mode, reserved with RO. In operation mode, when software initiate write
 *  operation, it is tx data register 12 and when software initiate read operation, it
 *  is rx data register 12.
 */
#define TWAI_DATA_12    0x000000FFU
#define TWAI_DATA_12_M  (TWAI_DATA_12_V << TWAI_DATA_12_S)
#define TWAI_DATA_12_V  0x000000FFU
#define TWAI_DATA_12_S  0

/** TWAI_RX_MESSAGE_COUNTER_REG register
 *  Received message counter register.
 */
#define TWAI_RX_MESSAGE_COUNTER_REG(i) (REG_TWAI_BASE(i) + 0x74)
/** TWAI_RX_MESSAGE_COUNTER : RO; bitpos: [6:0]; default: 0;
 *  Reflects the number of messages available within the RXFIFO. The value is
 *  incremented with each receive event and decremented by the release receive buffer
 *  command.
 */
#define TWAI_RX_MESSAGE_COUNTER    0x0000007FU
#define TWAI_RX_MESSAGE_COUNTER_M  (TWAI_RX_MESSAGE_COUNTER_V << TWAI_RX_MESSAGE_COUNTER_S)
#define TWAI_RX_MESSAGE_COUNTER_V  0x0000007FU
#define TWAI_RX_MESSAGE_COUNTER_S  0

/** TWAI_CLOCK_DIVIDER_REG register
 *  Clock divider register.
 */
#define TWAI_CLOCK_DIVIDER_REG(i) (REG_TWAI_BASE(i) + 0x7c)
/** TWAI_CD : R/W; bitpos: [7:0]; default: 0;
 *  These bits are used to define the frequency at the external CLKOUT pin.
 */
#define TWAI_CD    0x000000FFU
#define TWAI_CD_M  (TWAI_CD_V << TWAI_CD_S)
#define TWAI_CD_V  0x000000FFU
#define TWAI_CD_S  0
/** TWAI_CLOCK_OFF : R/W; bitpos: [8]; default: 0;
 *  1: Disable the external CLKOUT pin. 0: Enable the external CLKOUT pin. Software has
 *  R/W permission in reset mode and RO in operation mode.
 */
#define TWAI_CLOCK_OFF    (BIT(8))
#define TWAI_CLOCK_OFF_M  (TWAI_CLOCK_OFF_V << TWAI_CLOCK_OFF_S)
#define TWAI_CLOCK_OFF_V  0x00000001U
#define TWAI_CLOCK_OFF_S  8

/** TWAI_SW_STANDBY_CFG_REG register
 *  Software configure standby pin directly.
 */
#define TWAI_SW_STANDBY_CFG_REG(i) (REG_TWAI_BASE(i) + 0x80)
/** TWAI_SW_STANDBY_EN : R/W; bitpos: [0]; default: 0;
 *  Enable standby pin.
 */
#define TWAI_SW_STANDBY_EN    (BIT(0))
#define TWAI_SW_STANDBY_EN_M  (TWAI_SW_STANDBY_EN_V << TWAI_SW_STANDBY_EN_S)
#define TWAI_SW_STANDBY_EN_V  0x00000001U
#define TWAI_SW_STANDBY_EN_S  0
/** TWAI_SW_STANDBY_CLR : R/W; bitpos: [1]; default: 1;
 *  Clear standby pin.
 */
#define TWAI_SW_STANDBY_CLR    (BIT(1))
#define TWAI_SW_STANDBY_CLR_M  (TWAI_SW_STANDBY_CLR_V << TWAI_SW_STANDBY_CLR_S)
#define TWAI_SW_STANDBY_CLR_V  0x00000001U
#define TWAI_SW_STANDBY_CLR_S  1

/** TWAI_HW_CFG_REG register
 *  Hardware configure standby pin.
 */
#define TWAI_HW_CFG_REG(i) (REG_TWAI_BASE(i) + 0x84)
/** TWAI_HW_STANDBY_EN : R/W; bitpos: [0]; default: 0;
 *  Enable function that hardware control standby pin.
 */
#define TWAI_HW_STANDBY_EN    (BIT(0))
#define TWAI_HW_STANDBY_EN_M  (TWAI_HW_STANDBY_EN_V << TWAI_HW_STANDBY_EN_S)
#define TWAI_HW_STANDBY_EN_V  0x00000001U
#define TWAI_HW_STANDBY_EN_S  0

/** TWAI_HW_STANDBY_CNT_REG register
 *  Configure standby counter.
 */
#define TWAI_HW_STANDBY_CNT_REG(i) (REG_TWAI_BASE(i) + 0x88)
/** TWAI_STANDBY_WAIT_CNT : R/W; bitpos: [31:0]; default: 1;
 *  Configure the number of cycles before standby becomes high when TWAI_HW_STANDBY_EN
 *  is enabled.
 */
#define TWAI_STANDBY_WAIT_CNT    0xFFFFFFFFU
#define TWAI_STANDBY_WAIT_CNT_M  (TWAI_STANDBY_WAIT_CNT_V << TWAI_STANDBY_WAIT_CNT_S)
#define TWAI_STANDBY_WAIT_CNT_V  0xFFFFFFFFU
#define TWAI_STANDBY_WAIT_CNT_S  0

/** TWAI_IDLE_INTR_CNT_REG register
 *  Configure idle interrupt counter.
 */
#define TWAI_IDLE_INTR_CNT_REG(i) (REG_TWAI_BASE(i) + 0x8c)
/** TWAI_IDLE_INTR_CNT : R/W; bitpos: [31:0]; default: 1;
 *  Configure the number of cycles before triggering idle interrupt.
 */
#define TWAI_IDLE_INTR_CNT    0xFFFFFFFFU
#define TWAI_IDLE_INTR_CNT_M  (TWAI_IDLE_INTR_CNT_V << TWAI_IDLE_INTR_CNT_S)
#define TWAI_IDLE_INTR_CNT_V  0xFFFFFFFFU
#define TWAI_IDLE_INTR_CNT_S  0

/** TWAI_ECO_CFG_REG register
 *  ECO configuration register.
 */
#define TWAI_ECO_CFG_REG(i) (REG_TWAI_BASE(i) + 0x90)
/** TWAI_RDN_ENA : R/W; bitpos: [0]; default: 0;
 *  Enable eco module.
 */
#define TWAI_RDN_ENA    (BIT(0))
#define TWAI_RDN_ENA_M  (TWAI_RDN_ENA_V << TWAI_RDN_ENA_S)
#define TWAI_RDN_ENA_V  0x00000001U
#define TWAI_RDN_ENA_S  0
/** TWAI_RDN_RESULT : RO; bitpos: [1]; default: 1;
 *  Output of eco module.
 */
#define TWAI_RDN_RESULT    (BIT(1))
#define TWAI_RDN_RESULT_M  (TWAI_RDN_RESULT_V << TWAI_RDN_RESULT_S)
#define TWAI_RDN_RESULT_V  0x00000001U
#define TWAI_RDN_RESULT_S  1

/** TWAI_TIMESTAMP_DATA_REG register
 *  Timestamp data register
 */
#define TWAI_TIMESTAMP_DATA_REG(i) (REG_TWAI_BASE(i) + 0x94)
/** TWAI_TIMESTAMP_DATA : RO; bitpos: [31:0]; default: 0;
 *  Data of timestamp of a CAN frame.
 */
#define TWAI_TIMESTAMP_DATA    0xFFFFFFFFU
#define TWAI_TIMESTAMP_DATA_M  (TWAI_TIMESTAMP_DATA_V << TWAI_TIMESTAMP_DATA_S)
#define TWAI_TIMESTAMP_DATA_V  0xFFFFFFFFU
#define TWAI_TIMESTAMP_DATA_S  0

/** TWAI_TIMESTAMP_PRESCALER_REG register
 *  Timestamp configuration register
 */
#define TWAI_TIMESTAMP_PRESCALER_REG(i) (REG_TWAI_BASE(i) + 0x98)
/** TWAI_TS_DIV_NUM : R/W; bitpos: [15:0]; default: 31;
 *  Configures the clock division number of timestamp counter.
 */
#define TWAI_TS_DIV_NUM    0x0000FFFFU
#define TWAI_TS_DIV_NUM_M  (TWAI_TS_DIV_NUM_V << TWAI_TS_DIV_NUM_S)
#define TWAI_TS_DIV_NUM_V  0x0000FFFFU
#define TWAI_TS_DIV_NUM_S  0

/** TWAI_TIMESTAMP_CFG_REG register
 *  Timestamp configuration register
 */
#define TWAI_TIMESTAMP_CFG_REG(i) (REG_TWAI_BASE(i) + 0x9c)
/** TWAI_TS_ENABLE : R/W; bitpos: [0]; default: 0;
 *  enable the timestamp collection function.
 */
#define TWAI_TS_ENABLE    (BIT(0))
#define TWAI_TS_ENABLE_M  (TWAI_TS_ENABLE_V << TWAI_TS_ENABLE_S)
#define TWAI_TS_ENABLE_V  0x00000001U
#define TWAI_TS_ENABLE_S  0

#ifdef __cplusplus
}
#endif
