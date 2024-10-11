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

/** TWAIFD_DEVICE_ID_VERSION_REG register
 *  TWAI FD device id status register
 */
#define TWAIFD_DEVICE_ID_VERSION_REG(i) (REG_TWAI_BASE(i) + 0x0)
/** TWAIFD_DEVICE_ID : RO; bitpos: [15:0]; default: 51965;
 *  Represents whether CAN IP function is mapped correctly on its base address.
 */
#define TWAIFD_DEVICE_ID    0x0000FFFFU
#define TWAIFD_DEVICE_ID_M  (TWAIFD_DEVICE_ID_V << TWAIFD_DEVICE_ID_S)
#define TWAIFD_DEVICE_ID_V  0x0000FFFFU
#define TWAIFD_DEVICE_ID_S  0
/** TWAIFD_VER_MINOR : RO; bitpos: [23:16]; default: 4;
 *  TWAI FD IP version
 */
#define TWAIFD_VER_MINOR    0x000000FFU
#define TWAIFD_VER_MINOR_M  (TWAIFD_VER_MINOR_V << TWAIFD_VER_MINOR_S)
#define TWAIFD_VER_MINOR_V  0x000000FFU
#define TWAIFD_VER_MINOR_S  16
/** TWAIFD_VER_MAJOR : RO; bitpos: [31:24]; default: 2;
 *  TWAI FD IP version
 */
#define TWAIFD_VER_MAJOR    0x000000FFU
#define TWAIFD_VER_MAJOR_M  (TWAIFD_VER_MAJOR_V << TWAIFD_VER_MAJOR_S)
#define TWAIFD_VER_MAJOR_V  0x000000FFU
#define TWAIFD_VER_MAJOR_S  24

/** TWAIFD_MODE_SETTINGS_REG register
 *  TWAI FD mode setting register
 */
#define TWAIFD_MODE_SETTINGS_REG(i) (REG_TWAI_BASE(i) + 0x4)
/** TWAIFD_RST : WO; bitpos: [0]; default: 0;
 *  Soft reset. Writing logic 1 resets CTU CAN FD. After writing logic 1, logic 0 does
 *  not need to be written, this bit
 *  is automatically cleared.
 *  0: invalid
 *  1: reset.
 */
#define TWAIFD_RST    (BIT(0))
#define TWAIFD_RST_M  (TWAIFD_RST_V << TWAIFD_RST_S)
#define TWAIFD_RST_V  0x00000001U
#define TWAIFD_RST_S  0
/** TWAIFD_BMM : R/W; bitpos: [1]; default: 0;
 *  Bus monitoring mode. In this mode CTU CAN FD only receives frames and sends only
 *  recessive bits on CAN
 *  bus. When a dominant bit is sent, it is re-routed internally so that bus value is
 *  not changed. When this mode is
 *  enabled, CTU CAN FD will not transmit any frame from TXT Buffers,
 *  0b0 - BMM_DISABLED - Bus monitoring mode disabled.
 *  0b1 - BMM_ENABLED - Bus monitoring mode enabled.
 */
#define TWAIFD_BMM    (BIT(1))
#define TWAIFD_BMM_M  (TWAIFD_BMM_V << TWAIFD_BMM_S)
#define TWAIFD_BMM_V  0x00000001U
#define TWAIFD_BMM_S  1
/** TWAIFD_STM : R/W; bitpos: [2]; default: 0;
 *  Self Test Mode. In this mode transmitted frame is considered valid even if dominant
 *  acknowledge was not received.
 *  0b0 - STM_DISABLED - Self test mode disabled.
 *  0b1 - STM_ENABLED - Self test mode enabled.
 */
#define TWAIFD_STM    (BIT(2))
#define TWAIFD_STM_M  (TWAIFD_STM_V << TWAIFD_STM_S)
#define TWAIFD_STM_V  0x00000001U
#define TWAIFD_STM_S  2
/** TWAIFD_AFM : R/W; bitpos: [3]; default: 0;
 *  Acceptance Filters Mode. If enabled, only RX frames which pass Frame filters are
 *  stored in RX buffer. If disabled,
 *  every received frame is stored to RX buffer. This bit has meaning only if there is
 *  at least one filter available.
 *  Otherwise, this bit is reserved.
 *  0b0 - AFM_DISABLED - Acceptance filter mode disabled
 *  0b1 - AFM_ENABLED - Acceptance filter mode enabled
 */
#define TWAIFD_AFM    (BIT(3))
#define TWAIFD_AFM_M  (TWAIFD_AFM_V << TWAIFD_AFM_S)
#define TWAIFD_AFM_V  0x00000001U
#define TWAIFD_AFM_S  3
/** TWAIFD_FDE : R/W; bitpos: [4]; default: 1;
 *  Flexible data rate enable. When flexible data rate is enabled CTU CAN FD recognizes
 *  CAN FD frames (FDF bit
 *  = 1).
 *  0b0 - FDE_DISABLE - Flexible data-rate support disabled.
 *  0b1 - FDE_ENABLE - Flexible data-rate support enabled.
 */
#define TWAIFD_FDE    (BIT(4))
#define TWAIFD_FDE_M  (TWAIFD_FDE_V << TWAIFD_FDE_S)
#define TWAIFD_FDE_V  0x00000001U
#define TWAIFD_FDE_S  4
/** TWAIFD_TTTM : R/W; bitpos: [5]; default: 0;
 *  Time triggered transmission mode.
 *  0b0 - TTTM_DISABLED -
 *  0b1 - TTTM_ENABLED -
 */
#define TWAIFD_TTTM    (BIT(5))
#define TWAIFD_TTTM_M  (TWAIFD_TTTM_V << TWAIFD_TTTM_S)
#define TWAIFD_TTTM_V  0x00000001U
#define TWAIFD_TTTM_S  5
/** TWAIFD_ROM : R/W; bitpos: [6]; default: 0;
 *  Restricted operation mode.
 *  0b0 - ROM_DISABLED - Restricted operation mode is disabled.
 *  0b1 - ROM_ENABLED - Restricted operation mode is enabled.
 */
#define TWAIFD_ROM    (BIT(6))
#define TWAIFD_ROM_M  (TWAIFD_ROM_V << TWAIFD_ROM_S)
#define TWAIFD_ROM_V  0x00000001U
#define TWAIFD_ROM_S  6
/** TWAIFD_ACF : R/W; bitpos: [7]; default: 0;
 *  Acknowledge Forbidden Mode. When enabled, acknowledge is not sent even if received
 *  CRC matches the calculated
 *  one.
 *  0b0 - ACF_DISABLED - Acknowledge forbidden mode disabled.
 *  0b1 - ACF_ENABLED - Acknowledge forbidden mode enabled.
 */
#define TWAIFD_ACF    (BIT(7))
#define TWAIFD_ACF_M  (TWAIFD_ACF_V << TWAIFD_ACF_S)
#define TWAIFD_ACF_V  0x00000001U
#define TWAIFD_ACF_S  7
/** TWAIFD_TSTM : R/W; bitpos: [8]; default: 0;
 *  Test Mode. In test mode several registers have special features. Refer to
 *  description of Test mode for further
 *  Details.
 */
#define TWAIFD_TSTM    (BIT(8))
#define TWAIFD_TSTM_M  (TWAIFD_TSTM_V << TWAIFD_TSTM_S)
#define TWAIFD_TSTM_V  0x00000001U
#define TWAIFD_TSTM_S  8
/** TWAIFD_RXBAM : R/W; bitpos: [9]; default: 1;
 *  RX Buffer automatic mode.
 *  0b0 - RXBAM_DISABLED -
 *  0b1 - RXBAM_ENABLED -
 */
#define TWAIFD_RXBAM    (BIT(9))
#define TWAIFD_RXBAM_M  (TWAIFD_RXBAM_V << TWAIFD_RXBAM_S)
#define TWAIFD_RXBAM_V  0x00000001U
#define TWAIFD_RXBAM_S  9
/** TWAIFD_TXBBM : R/W; bitpos: [10]; default: 0;
 *  TXT Buffer Backup mode\\
 *  0b0 - TXBBM_DISABLED - TXT Buffer Backup mode disabled\\
 *  0b1 - TXBBM_ENABLED - TXT Buffer Backup mode enabled\\
 */
#define TWAIFD_TXBBM    (BIT(10))
#define TWAIFD_TXBBM_M  (TWAIFD_TXBBM_V << TWAIFD_TXBBM_S)
#define TWAIFD_TXBBM_V  0x00000001U
#define TWAIFD_TXBBM_S  10
/** TWAIFD_SAM : R/W; bitpos: [11]; default: 0;
 *  Self-acknowledge mode.\\
 *  0b0 - SAM_DISABLE - Do not send dominant ACK bit when CTU CAN FD sends Acknowledge
 *  bit.\\
 *  0b1 - SAM_ENABLE - Send dominant ACK bit when CTU CAN FD transmits CAN frame.\\
 */
#define TWAIFD_SAM    (BIT(11))
#define TWAIFD_SAM_M  (TWAIFD_SAM_V << TWAIFD_SAM_S)
#define TWAIFD_SAM_V  0x00000001U
#define TWAIFD_SAM_S  11
/** TWAIFD_RTRLE : R/W; bitpos: [16]; default: 0;
 *  Retransmitt Limit Enable. If enabled, CTU CAN FD only attempts to retransmitt each
 *  frame up to RTR_TH
 *  times.
 *  0b0 - RTRLE_DISABLED - Retransmitt limit is disabled.
 *  0b1 - RTRLE_ENABLED - Retransmitt limit is enabled.
 */
#define TWAIFD_RTRLE    (BIT(16))
#define TWAIFD_RTRLE_M  (TWAIFD_RTRLE_V << TWAIFD_RTRLE_S)
#define TWAIFD_RTRLE_V  0x00000001U
#define TWAIFD_RTRLE_S  16
/** TWAIFD_RTRTH : R/W; bitpos: [20:17]; default: 0;
 *  Retransmitt Limit Threshold. Maximal amount of retransmission attempts when
 *  SETTINGS[RTRLE] is en-
 *  Abled.
 */
#define TWAIFD_RTRTH    0x0000000FU
#define TWAIFD_RTRTH_M  (TWAIFD_RTRTH_V << TWAIFD_RTRTH_S)
#define TWAIFD_RTRTH_V  0x0000000FU
#define TWAIFD_RTRTH_S  17
/** TWAIFD_ILBP : R/W; bitpos: [21]; default: 0;
 *  Internal Loop Back mode. When enabled, CTU CAN FD receives any frame it transmits.
 *  0b0 - INT_LOOP_DISABLED - Internal loop-back is disabled.
 *  0b1 - INT_LOOP_ENABLED - Internal loop-back is enabled.
 */
#define TWAIFD_ILBP    (BIT(21))
#define TWAIFD_ILBP_M  (TWAIFD_ILBP_V << TWAIFD_ILBP_S)
#define TWAIFD_ILBP_V  0x00000001U
#define TWAIFD_ILBP_S  21
/** TWAIFD_ENA : R/W; bitpos: [22]; default: 0;
 *  Main enable bit of CTU CAN FD. When enabled, CTU CAN FD communicates on CAN bus.
 *  When disabled, it
 *  is bus-off and does not take part of CAN bus communication.
 *  0b0 - CTU_CAN_DISABLED - The CAN Core is disabled.
 *  0b1 - CTU_CAN_ENABLED - The CAN Core is enabled.
 */
#define TWAIFD_ENA    (BIT(22))
#define TWAIFD_ENA_M  (TWAIFD_ENA_V << TWAIFD_ENA_S)
#define TWAIFD_ENA_V  0x00000001U
#define TWAIFD_ENA_S  22
/** TWAIFD_NISOFD : R/W; bitpos: [23]; default: 0;
 *  Non ISO FD. When this bit is set, CTU CAN FD is compliant to NON-ISO CAN FD
 *  specification (no stuff
 *  count field). This bit should be modified only when SETTINGS[ENA]=0.
 *  0b0 - ISO_FD - The CAN Controller conforms to ISO CAN FD specification.
 *  0b1 - NON_ISO_FD - The CAN Controller conforms to NON ISO CAN FD specification.
 *  CANFD 1.0
 */
#define TWAIFD_NISOFD    (BIT(23))
#define TWAIFD_NISOFD_M  (TWAIFD_NISOFD_V << TWAIFD_NISOFD_S)
#define TWAIFD_NISOFD_V  0x00000001U
#define TWAIFD_NISOFD_S  23
/** TWAIFD_PEX : R/W; bitpos: [24]; default: 0;
 *  Protocol exception handling. When this bit is set, CTU CAN FD will start
 *  integration upon detection of protocol
 *  exception. This should be modified only when SETTINGS[ENA] = ’0’.
 *  0b0 - PROTOCOL_EXCEPTION_DISABLED - Protocol exception handling is disabled.
 *  0b1 - PROTOCOL_EXCEPTION_ENABLED - Protocol exception handling is enabled.
 */
#define TWAIFD_PEX    (BIT(24))
#define TWAIFD_PEX_M  (TWAIFD_PEX_V << TWAIFD_PEX_S)
#define TWAIFD_PEX_V  0x00000001U
#define TWAIFD_PEX_S  24
/** TWAIFD_TBFBO : R/W; bitpos: [25]; default: 1;
 *  All TXT buffers shall go to "TX failed" state when CTU CAN FD becomes bus-off.
 *  0b0 - TXTBUF_FAILED_BUS_OFF_DISABLED - TXT Buffers dont go to "TX failed" state
 *  when CTU CAN
 *  FD becomes bus-off.
 *  0b1 - TXTBUF_FAILED_BUS_OFF_ENABLED - TXT Buffers go to "TX failed" state when CTU
 *  CAN FD
 *  becomes bus-off.
 */
#define TWAIFD_TBFBO    (BIT(25))
#define TWAIFD_TBFBO_M  (TWAIFD_TBFBO_V << TWAIFD_TBFBO_S)
#define TWAIFD_TBFBO_V  0x00000001U
#define TWAIFD_TBFBO_S  25
/** TWAIFD_FDRF : R/W; bitpos: [26]; default: 0;
 *  Frame filters drop Remote frames.
 *  0b0 - DROP_RF_DISABLED - Frame filters accept RTR frames.
 *  0b1 - DROP_RF_ENABLED - Frame filters drop RTR frames.
 */
#define TWAIFD_FDRF    (BIT(26))
#define TWAIFD_FDRF_M  (TWAIFD_FDRF_V << TWAIFD_FDRF_S)
#define TWAIFD_FDRF_V  0x00000001U
#define TWAIFD_FDRF_S  26
/** TWAIFD_PCHKE : R/W; bitpos: [27]; default: 0;
 *  Enable Parity checks in TXT Buffers and RX Buffer.
 */
#define TWAIFD_PCHKE    (BIT(27))
#define TWAIFD_PCHKE_M  (TWAIFD_PCHKE_V << TWAIFD_PCHKE_S)
#define TWAIFD_PCHKE_V  0x00000001U
#define TWAIFD_PCHKE_S  27

/** TWAIFD_STATUS_REG register
 *  TWAI FD status register
 */
#define TWAIFD_STATUS_REG(i) (REG_TWAI_BASE(i) + 0x8)
/** TWAIFD_RXNE : RO; bitpos: [0]; default: 0;
 *  RX buffer not empty. This bit is 1 when least one frame is stored in RX buffer.
 *  0: empty
 *  1: not empty
 */
#define TWAIFD_RXNE    (BIT(0))
#define TWAIFD_RXNE_M  (TWAIFD_RXNE_V << TWAIFD_RXNE_S)
#define TWAIFD_RXNE_V  0x00000001U
#define TWAIFD_RXNE_S  0
/** TWAIFD_DOR : RO; bitpos: [1]; default: 0;
 *  Data Overrun flag. This bit is set when frame was dropped due to lack of space in
 *  RX buffer. This bit can be
 *  cleared by COMMAND[RRB].
 *  0: not overrun
 *  1: overrun
 */
#define TWAIFD_DOR    (BIT(1))
#define TWAIFD_DOR_M  (TWAIFD_DOR_V << TWAIFD_DOR_S)
#define TWAIFD_DOR_V  0x00000001U
#define TWAIFD_DOR_S  1
/** TWAIFD_TXNF : RO; bitpos: [2]; default: 1;
 *  TXT buffers status. This bit is set if at least one TXT buffer is in "Empty" state.
 *  0: not full
 *  1: full
 */
#define TWAIFD_TXNF    (BIT(2))
#define TWAIFD_TXNF_M  (TWAIFD_TXNF_V << TWAIFD_TXNF_S)
#define TWAIFD_TXNF_V  0x00000001U
#define TWAIFD_TXNF_S  2
/** TWAIFD_EFT : RO; bitpos: [3]; default: 0;
 *  Error frame is being transmitted at the moment.
 *  0: not being transmitted
 *  1: being transmitted
 */
#define TWAIFD_EFT    (BIT(3))
#define TWAIFD_EFT_M  (TWAIFD_EFT_V << TWAIFD_EFT_S)
#define TWAIFD_EFT_V  0x00000001U
#define TWAIFD_EFT_S  3
/** TWAIFD_RXS : RO; bitpos: [4]; default: 0;
 *  CTU CAN FD is receiver of CAN Frame.
 *  0: not receiving
 *  1: receiving
 */
#define TWAIFD_RXS    (BIT(4))
#define TWAIFD_RXS_M  (TWAIFD_RXS_V << TWAIFD_RXS_S)
#define TWAIFD_RXS_V  0x00000001U
#define TWAIFD_RXS_S  4
/** TWAIFD_TXS : RO; bitpos: [5]; default: 0;
 *  CTU CAN FD is transmitter of CAN Frame.
 *  0: not transmitting
 *  1: transmitting
 */
#define TWAIFD_TXS    (BIT(5))
#define TWAIFD_TXS_M  (TWAIFD_TXS_V << TWAIFD_TXS_S)
#define TWAIFD_TXS_V  0x00000001U
#define TWAIFD_TXS_S  5
/** TWAIFD_EWL : RO; bitpos: [6]; default: 0;
 *  TX Error counter (TEC) or RX Error counter (REC) is equal to, or higher than Error
 *  warning limit (EWL).
 *  0: not reached
 *  1: reached
 */
#define TWAIFD_EWL    (BIT(6))
#define TWAIFD_EWL_M  (TWAIFD_EWL_V << TWAIFD_EWL_S)
#define TWAIFD_EWL_V  0x00000001U
#define TWAIFD_EWL_S  6
/** TWAIFD_IDLE : RO; bitpos: [7]; default: 1;
 *  Bus is idle (no frame is being transmitted/received) or CTU CAN FD is bus-off.
 *  0: active
 *  1: not active
 */
#define TWAIFD_IDLE    (BIT(7))
#define TWAIFD_IDLE_M  (TWAIFD_IDLE_V << TWAIFD_IDLE_S)
#define TWAIFD_IDLE_V  0x00000001U
#define TWAIFD_IDLE_S  7
/** TWAIFD_PEXS : RO; bitpos: [8]; default: 0;
 *  Protocol exception status (flag). Set when Protocol exception occurs. Cleared by
 *  writing COMMAND[CPEXS]=1.
 */
#define TWAIFD_PEXS    (BIT(8))
#define TWAIFD_PEXS_M  (TWAIFD_PEXS_V << TWAIFD_PEXS_S)
#define TWAIFD_PEXS_V  0x00000001U
#define TWAIFD_PEXS_S  8
/** TWAIFD_RXPE : RO; bitpos: [9]; default: 0;
 *  Set when parity error is detected during read of CAN frame from RX Buffer via
 *  RX_DATA register.
 */
#define TWAIFD_RXPE    (BIT(9))
#define TWAIFD_RXPE_M  (TWAIFD_RXPE_V << TWAIFD_RXPE_S)
#define TWAIFD_RXPE_V  0x00000001U
#define TWAIFD_RXPE_S  9
/** TWAIFD_TXPE : RO; bitpos: [10]; default: 0;
 *  TXT Buffers Parity Error flag. Set When Parity Error is detected in a TXT Buffer
 *  during transmission from this
 *  Buffer.
 */
#define TWAIFD_TXPE    (BIT(10))
#define TWAIFD_TXPE_M  (TWAIFD_TXPE_V << TWAIFD_TXPE_S)
#define TWAIFD_TXPE_V  0x00000001U
#define TWAIFD_TXPE_S  10
/** TWAIFD_TXDPE : RO; bitpos: [11]; default: 0;
 *  TXT Buffer double parity error. Set in TXT Buffer Backup mode when parity error is
 *  detected in "backup"
 *  TXT Buffer.
 */
#define TWAIFD_TXDPE    (BIT(11))
#define TWAIFD_TXDPE_M  (TWAIFD_TXDPE_V << TWAIFD_TXDPE_S)
#define TWAIFD_TXDPE_V  0x00000001U
#define TWAIFD_TXDPE_S  11
/** TWAIFD_STCNT : RO; bitpos: [16]; default: 1;
 *  Support of Traffic counters. When this bit is 1, Traffic counters are present.
 */
#define TWAIFD_STCNT    (BIT(16))
#define TWAIFD_STCNT_M  (TWAIFD_STCNT_V << TWAIFD_STCNT_S)
#define TWAIFD_STCNT_V  0x00000001U
#define TWAIFD_STCNT_S  16
/** TWAIFD_STRGS : RO; bitpos: [17]; default: 1;
 *  Support of Test Registers for memory testability. When this bit is 1, Test
 *  Registers are present.
 */
#define TWAIFD_STRGS    (BIT(17))
#define TWAIFD_STRGS_M  (TWAIFD_STRGS_V << TWAIFD_STRGS_S)
#define TWAIFD_STRGS_V  0x00000001U
#define TWAIFD_STRGS_S  17
/** TWAIFD_SPRT : RO; bitpos: [18]; default: 1;
 *  Support of Parity protection on each word of TXT Buffer RAM and RX Buffer RAM.
 */
#define TWAIFD_SPRT    (BIT(18))
#define TWAIFD_SPRT_M  (TWAIFD_SPRT_V << TWAIFD_SPRT_S)
#define TWAIFD_SPRT_V  0x00000001U
#define TWAIFD_SPRT_S  18

/** TWAIFD_COMMAND_REG register
 *  TWAI FD command register
 */
#define TWAIFD_COMMAND_REG(i) (REG_TWAI_BASE(i) + 0xc)
/** TWAIFD_RXRPMV : WO; bitpos: [1]; default: 0;
 *  RX Buffer read pointer move.
 */
#define TWAIFD_RXRPMV    (BIT(1))
#define TWAIFD_RXRPMV_M  (TWAIFD_RXRPMV_V << TWAIFD_RXRPMV_S)
#define TWAIFD_RXRPMV_V  0x00000001U
#define TWAIFD_RXRPMV_S  1
/** TWAIFD_RRB : WO; bitpos: [2]; default: 0;
 *  Release RX Buffer. This command flushes RX buffer and resets its memory pointers.
 *  0: invalid
 *  1: delete
 */
#define TWAIFD_RRB    (BIT(2))
#define TWAIFD_RRB_M  (TWAIFD_RRB_V << TWAIFD_RRB_S)
#define TWAIFD_RRB_V  0x00000001U
#define TWAIFD_RRB_S  2
/** TWAIFD_CDO : WO; bitpos: [3]; default: 0;
 *  Clear Data Overrun flag in RX buffer.
 *  0: invalid
 *  1: clear
 */
#define TWAIFD_CDO    (BIT(3))
#define TWAIFD_CDO_M  (TWAIFD_CDO_V << TWAIFD_CDO_S)
#define TWAIFD_CDO_V  0x00000001U
#define TWAIFD_CDO_S  3
/** TWAIFD_ERCRST : WO; bitpos: [4]; default: 0;
 *  Error Counters Reset. When unit is bus off, issuing this command will request
 *  erasing TEC, REC counters after
 *  128 consecutive occurrences of 11 recessive bits. Upon completion, TEC and REC are
 *  erased and fault confinement
 *  State is set to error-active. When unit is not bus-off, or when unit is bus-off due
 *  to being disabled (SETTINGS[ENA]
 *  = ’0’), this command has no effect.
 */
#define TWAIFD_ERCRST    (BIT(4))
#define TWAIFD_ERCRST_M  (TWAIFD_ERCRST_V << TWAIFD_ERCRST_S)
#define TWAIFD_ERCRST_V  0x00000001U
#define TWAIFD_ERCRST_S  4
/** TWAIFD_RXFCRST : WO; bitpos: [5]; default: 0;
 *  Clear RX bus traffic counter (RX_COUNTER register).
 */
#define TWAIFD_RXFCRST    (BIT(5))
#define TWAIFD_RXFCRST_M  (TWAIFD_RXFCRST_V << TWAIFD_RXFCRST_S)
#define TWAIFD_RXFCRST_V  0x00000001U
#define TWAIFD_RXFCRST_S  5
/** TWAIFD_TXFCRST : WO; bitpos: [6]; default: 0;
 *  Clear TX bus traffic counter (TX_COUNTER register).
 */
#define TWAIFD_TXFCRST    (BIT(6))
#define TWAIFD_TXFCRST_M  (TWAIFD_TXFCRST_V << TWAIFD_TXFCRST_S)
#define TWAIFD_TXFCRST_V  0x00000001U
#define TWAIFD_TXFCRST_S  6
/** TWAIFD_CPEXS : WO; bitpos: [7]; default: 0;
 *  Clear Protocol exception status (STATUS[PEXS]).
 */
#define TWAIFD_CPEXS    (BIT(7))
#define TWAIFD_CPEXS_M  (TWAIFD_CPEXS_V << TWAIFD_CPEXS_S)
#define TWAIFD_CPEXS_V  0x00000001U
#define TWAIFD_CPEXS_S  7
/** TWAIFD_CRXPE : WO; bitpos: [8]; default: 0;
 *  Clear STATUS[RXPE] flag.
 */
#define TWAIFD_CRXPE    (BIT(8))
#define TWAIFD_CRXPE_M  (TWAIFD_CRXPE_V << TWAIFD_CRXPE_S)
#define TWAIFD_CRXPE_V  0x00000001U
#define TWAIFD_CRXPE_S  8
/** TWAIFD_CTXPE : WO; bitpos: [9]; default: 0;
 *  Clear STATUS[TXPE] flag.
 */
#define TWAIFD_CTXPE    (BIT(9))
#define TWAIFD_CTXPE_M  (TWAIFD_CTXPE_V << TWAIFD_CTXPE_S)
#define TWAIFD_CTXPE_V  0x00000001U
#define TWAIFD_CTXPE_S  9
/** TWAIFD_CTXDPE : WO; bitpos: [10]; default: 0;
 *  Clear STATUS[TXDPE] flag.
 */
#define TWAIFD_CTXDPE    (BIT(10))
#define TWAIFD_CTXDPE_M  (TWAIFD_CTXDPE_V << TWAIFD_CTXDPE_S)
#define TWAIFD_CTXDPE_V  0x00000001U
#define TWAIFD_CTXDPE_S  10

/** TWAIFD_INT_STAT_REG register
 *  TWAI FD command register
 */
#define TWAIFD_INT_STAT_REG(i) (REG_TWAI_BASE(i) + 0x10)
/** TWAIFD_RXI_INT_ST : R/W1C; bitpos: [0]; default: 0;
 *  The masked interrupt status of TWAIFD_RXI_INT.
 *  Frame received interrupt.
 */
#define TWAIFD_RXI_INT_ST    (BIT(0))
#define TWAIFD_RXI_INT_ST_M  (TWAIFD_RXI_INT_ST_V << TWAIFD_RXI_INT_ST_S)
#define TWAIFD_RXI_INT_ST_V  0x00000001U
#define TWAIFD_RXI_INT_ST_S  0
/** TWAIFD_TXI_INT_ST : R/W1C; bitpos: [1]; default: 0;
 *  The masked interrupt status of TWAIFD_TXI_INT.
 *  Frame transmitted interrupt.
 */
#define TWAIFD_TXI_INT_ST    (BIT(1))
#define TWAIFD_TXI_INT_ST_M  (TWAIFD_TXI_INT_ST_V << TWAIFD_TXI_INT_ST_S)
#define TWAIFD_TXI_INT_ST_V  0x00000001U
#define TWAIFD_TXI_INT_ST_S  1
/** TWAIFD_EWLI_INT_ST : R/W1C; bitpos: [2]; default: 0;
 *  The masked interrupt status of TWAIFD_EWLI_INT.
 *  Error warning limit interrupt. When both TEC and REC are lower than EWL and one of
 *  the becomes equal to or
 *  higher than EWL, or when both TEC and REC become less than EWL, this interrupt is
 *  generated. When Interrupt
 *  is cleared and REC, or TEC is still equal to or higher than EWL, Interrupt is not
 *  generated again.
 */
#define TWAIFD_EWLI_INT_ST    (BIT(2))
#define TWAIFD_EWLI_INT_ST_M  (TWAIFD_EWLI_INT_ST_V << TWAIFD_EWLI_INT_ST_S)
#define TWAIFD_EWLI_INT_ST_V  0x00000001U
#define TWAIFD_EWLI_INT_ST_S  2
/** TWAIFD_DOI_INT_ST : R/W1C; bitpos: [3]; default: 0;
 *  The masked interrupt status of TWAIFD_DOI_INT.
 *  Data overrun interrupt. Before this  Interrupt is cleared , STATUS[DOR] must be
 *  cleared to avoid setting of this
 *  Interrupt again.
 */
#define TWAIFD_DOI_INT_ST    (BIT(3))
#define TWAIFD_DOI_INT_ST_M  (TWAIFD_DOI_INT_ST_V << TWAIFD_DOI_INT_ST_S)
#define TWAIFD_DOI_INT_ST_V  0x00000001U
#define TWAIFD_DOI_INT_ST_S  3
/** TWAIFD_FCSI_INT_ST : R/W1C; bitpos: [4]; default: 0;
 *  The masked interrupt status of TWAIFD_FCSI_INT.
 *  Fault confinement state changed interrupt. Interrupt is set when node turns
 *  error-passive (from error-active),
 *  bus-off (from error-passive) or error-active (from bus-off after reintegration or
 *  from error-passive).
 */
#define TWAIFD_FCSI_INT_ST    (BIT(4))
#define TWAIFD_FCSI_INT_ST_M  (TWAIFD_FCSI_INT_ST_V << TWAIFD_FCSI_INT_ST_S)
#define TWAIFD_FCSI_INT_ST_V  0x00000001U
#define TWAIFD_FCSI_INT_ST_S  4
/** TWAIFD_ALI_INT_ST : R/W1C; bitpos: [5]; default: 0;
 *  The masked interrupt status of TWAIFD_ALI_INT.
 *  Arbitration lost interrupt.
 */
#define TWAIFD_ALI_INT_ST    (BIT(5))
#define TWAIFD_ALI_INT_ST_M  (TWAIFD_ALI_INT_ST_V << TWAIFD_ALI_INT_ST_S)
#define TWAIFD_ALI_INT_ST_V  0x00000001U
#define TWAIFD_ALI_INT_ST_S  5
/** TWAIFD_BEI_INT_ST : R/W1C; bitpos: [6]; default: 0;
 *  The masked interrupt status of TWAIFD_BEI_INT.
 *  Bus error interrupt.
 */
#define TWAIFD_BEI_INT_ST    (BIT(6))
#define TWAIFD_BEI_INT_ST_M  (TWAIFD_BEI_INT_ST_V << TWAIFD_BEI_INT_ST_S)
#define TWAIFD_BEI_INT_ST_V  0x00000001U
#define TWAIFD_BEI_INT_ST_S  6
/** TWAIFD_OFI_INT_ST : R/W1C; bitpos: [7]; default: 0;
 *  The masked interrupt status of TWAIFD_OFI_INT.
 *  Overload frame interrupt.
 */
#define TWAIFD_OFI_INT_ST    (BIT(7))
#define TWAIFD_OFI_INT_ST_M  (TWAIFD_OFI_INT_ST_V << TWAIFD_OFI_INT_ST_S)
#define TWAIFD_OFI_INT_ST_V  0x00000001U
#define TWAIFD_OFI_INT_ST_S  7
/** TWAIFD_RXFI_INT_ST : R/W1C; bitpos: [8]; default: 0;
 *  The masked interrupt status of TWAIFD_RXFI_INT.
 *  RX buffer full interrupt.
 */
#define TWAIFD_RXFI_INT_ST    (BIT(8))
#define TWAIFD_RXFI_INT_ST_M  (TWAIFD_RXFI_INT_ST_V << TWAIFD_RXFI_INT_ST_S)
#define TWAIFD_RXFI_INT_ST_V  0x00000001U
#define TWAIFD_RXFI_INT_ST_S  8
/** TWAIFD_BSI_INT_ST : R/W1C; bitpos: [9]; default: 0;
 *  The masked interrupt status of TWAIFD_BSI_INT.
 *  Bit rate shifted interrupt.
 */
#define TWAIFD_BSI_INT_ST    (BIT(9))
#define TWAIFD_BSI_INT_ST_M  (TWAIFD_BSI_INT_ST_V << TWAIFD_BSI_INT_ST_S)
#define TWAIFD_BSI_INT_ST_V  0x00000001U
#define TWAIFD_BSI_INT_ST_S  9
/** TWAIFD_RBNEI_INT_ST : R/W1C; bitpos: [10]; default: 0;
 *  The masked interrupt status of TWAIFD_RBNEI_INT.
 *  RX buffer not empty interrupt. Clearing this interrupt and not reading out content
 *  of RX Buffer via RX_DATA
 *  will re-activate the interrupt.
 */
#define TWAIFD_RBNEI_INT_ST    (BIT(10))
#define TWAIFD_RBNEI_INT_ST_M  (TWAIFD_RBNEI_INT_ST_V << TWAIFD_RBNEI_INT_ST_S)
#define TWAIFD_RBNEI_INT_ST_V  0x00000001U
#define TWAIFD_RBNEI_INT_ST_S  10
/** TWAIFD_TXBHCI_INT_ST : R/W1C; bitpos: [11]; default: 0;
 *  The masked interrupt status of TWAIFD_TXBHCI_INT.
 *  TXT buffer HW command interrupt. Anytime TXT buffer receives HW command from CAN
 *  Core which
 *  changes TXT buffer state to "TX OK", "Error" or "Aborted", this interrupt will be
 *  generated.
 */
#define TWAIFD_TXBHCI_INT_ST    (BIT(11))
#define TWAIFD_TXBHCI_INT_ST_M  (TWAIFD_TXBHCI_INT_ST_V << TWAIFD_TXBHCI_INT_ST_S)
#define TWAIFD_TXBHCI_INT_ST_V  0x00000001U
#define TWAIFD_TXBHCI_INT_ST_S  11

/** TWAIFD_INT_ENA_SET_REG register
 *  TWAI FD interrupt enable register
 */
#define TWAIFD_INT_ENA_SET_REG(i) (REG_TWAI_BASE(i) + 0x14)
/** TWAIFD_RXI_INT_ENA_MASK : R/W1S; bitpos: [0]; default: 0;
 *  Write 1 to enable TWAIFD_RXI_INT.
 */
#define TWAIFD_RXI_INT_ENA_MASK    (BIT(0))
#define TWAIFD_RXI_INT_ENA_MASK_M  (TWAIFD_RXI_INT_ENA_MASK_V << TWAIFD_RXI_INT_ENA_MASK_S)
#define TWAIFD_RXI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_RXI_INT_ENA_MASK_S  0
/** TWAIFD_TXI_INT_ENA_MASK : R/W1S; bitpos: [1]; default: 0;
 *  Write 1 to enable TWAIFD_TXI_INT.
 */
#define TWAIFD_TXI_INT_ENA_MASK    (BIT(1))
#define TWAIFD_TXI_INT_ENA_MASK_M  (TWAIFD_TXI_INT_ENA_MASK_V << TWAIFD_TXI_INT_ENA_MASK_S)
#define TWAIFD_TXI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_TXI_INT_ENA_MASK_S  1
/** TWAIFD_EWLI_INT_ENA_MASK : R/W1S; bitpos: [2]; default: 0;
 *  Write 1 to enable TWAIFD_EWLI_INT.
 */
#define TWAIFD_EWLI_INT_ENA_MASK    (BIT(2))
#define TWAIFD_EWLI_INT_ENA_MASK_M  (TWAIFD_EWLI_INT_ENA_MASK_V << TWAIFD_EWLI_INT_ENA_MASK_S)
#define TWAIFD_EWLI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_EWLI_INT_ENA_MASK_S  2
/** TWAIFD_DOI_INT_ENA_MASK : R/W1S; bitpos: [3]; default: 0;
 *  Write 1 to enable TWAIFD_DOI_INT.
 */
#define TWAIFD_DOI_INT_ENA_MASK    (BIT(3))
#define TWAIFD_DOI_INT_ENA_MASK_M  (TWAIFD_DOI_INT_ENA_MASK_V << TWAIFD_DOI_INT_ENA_MASK_S)
#define TWAIFD_DOI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_DOI_INT_ENA_MASK_S  3
/** TWAIFD_FCSI_INT_ENA_MASK : R/W1S; bitpos: [4]; default: 0;
 *  Write 1 to enable TWAIFD_FCSI_INT.
 */
#define TWAIFD_FCSI_INT_ENA_MASK    (BIT(4))
#define TWAIFD_FCSI_INT_ENA_MASK_M  (TWAIFD_FCSI_INT_ENA_MASK_V << TWAIFD_FCSI_INT_ENA_MASK_S)
#define TWAIFD_FCSI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_FCSI_INT_ENA_MASK_S  4
/** TWAIFD_ALI_INT_ENA_MASK : R/W1S; bitpos: [5]; default: 0;
 *  Write 1 to enable TWAIFD_ALI_INT.
 */
#define TWAIFD_ALI_INT_ENA_MASK    (BIT(5))
#define TWAIFD_ALI_INT_ENA_MASK_M  (TWAIFD_ALI_INT_ENA_MASK_V << TWAIFD_ALI_INT_ENA_MASK_S)
#define TWAIFD_ALI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_ALI_INT_ENA_MASK_S  5
/** TWAIFD_BEI_INT_ENA_MASK : R/W1S; bitpos: [6]; default: 0;
 *  Write 1 to enable TWAIFD_BEI_INT.
 */
#define TWAIFD_BEI_INT_ENA_MASK    (BIT(6))
#define TWAIFD_BEI_INT_ENA_MASK_M  (TWAIFD_BEI_INT_ENA_MASK_V << TWAIFD_BEI_INT_ENA_MASK_S)
#define TWAIFD_BEI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_BEI_INT_ENA_MASK_S  6
/** TWAIFD_OFI_INT_ENA_MASK : R/W1S; bitpos: [7]; default: 0;
 *  Write 1 to enable TWAIFD_OFI_INT.
 */
#define TWAIFD_OFI_INT_ENA_MASK    (BIT(7))
#define TWAIFD_OFI_INT_ENA_MASK_M  (TWAIFD_OFI_INT_ENA_MASK_V << TWAIFD_OFI_INT_ENA_MASK_S)
#define TWAIFD_OFI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_OFI_INT_ENA_MASK_S  7
/** TWAIFD_RXFI_INT_ENA_MASK : R/W1S; bitpos: [8]; default: 0;
 *  Write 1 to enable TWAIFD_RXFI_INT.
 */
#define TWAIFD_RXFI_INT_ENA_MASK    (BIT(8))
#define TWAIFD_RXFI_INT_ENA_MASK_M  (TWAIFD_RXFI_INT_ENA_MASK_V << TWAIFD_RXFI_INT_ENA_MASK_S)
#define TWAIFD_RXFI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_RXFI_INT_ENA_MASK_S  8
/** TWAIFD_BSI_INT_ENA_MASK : R/W1S; bitpos: [9]; default: 0;
 *  Write 1 to enable TWAIFD_BSI_INT.
 */
#define TWAIFD_BSI_INT_ENA_MASK    (BIT(9))
#define TWAIFD_BSI_INT_ENA_MASK_M  (TWAIFD_BSI_INT_ENA_MASK_V << TWAIFD_BSI_INT_ENA_MASK_S)
#define TWAIFD_BSI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_BSI_INT_ENA_MASK_S  9
/** TWAIFD_RBNEI_INT_ENA_MASK : R/W1S; bitpos: [10]; default: 0;
 *  Write 1 to enable TWAIFD_RBNEI_INT.
 */
#define TWAIFD_RBNEI_INT_ENA_MASK    (BIT(10))
#define TWAIFD_RBNEI_INT_ENA_MASK_M  (TWAIFD_RBNEI_INT_ENA_MASK_V << TWAIFD_RBNEI_INT_ENA_MASK_S)
#define TWAIFD_RBNEI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_RBNEI_INT_ENA_MASK_S  10
/** TWAIFD_TXBHCI_INT_ENA_MASK : R/W1S; bitpos: [11]; default: 0;
 *  Write 1 to enable TWAIFD_TXBHCI_INT.
 */
#define TWAIFD_TXBHCI_INT_ENA_MASK    (BIT(11))
#define TWAIFD_TXBHCI_INT_ENA_MASK_M  (TWAIFD_TXBHCI_INT_ENA_MASK_V << TWAIFD_TXBHCI_INT_ENA_MASK_S)
#define TWAIFD_TXBHCI_INT_ENA_MASK_V  0x00000001U
#define TWAIFD_TXBHCI_INT_ENA_MASK_S  11

/** TWAIFD_INT_ENA_CLR_REG register
 *  TWAI FD interrupt enable clear register
 */
#define TWAIFD_INT_ENA_CLR_REG(i) (REG_TWAI_BASE(i) + 0x18)
/** TWAIFD_RXI_INT_ENA_CLR : WO; bitpos: [0]; default: 0;
 *  Write 1 to clear TWAIFD_RXI_INT_ENA .
 */
#define TWAIFD_RXI_INT_ENA_CLR    (BIT(0))
#define TWAIFD_RXI_INT_ENA_CLR_M  (TWAIFD_RXI_INT_ENA_CLR_V << TWAIFD_RXI_INT_ENA_CLR_S)
#define TWAIFD_RXI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_RXI_INT_ENA_CLR_S  0
/** TWAIFD_TXI_INT_ENA_CLR : WO; bitpos: [1]; default: 0;
 *  Write 1 to clear TWAIFD_TXI_INT_ENA .
 */
#define TWAIFD_TXI_INT_ENA_CLR    (BIT(1))
#define TWAIFD_TXI_INT_ENA_CLR_M  (TWAIFD_TXI_INT_ENA_CLR_V << TWAIFD_TXI_INT_ENA_CLR_S)
#define TWAIFD_TXI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_TXI_INT_ENA_CLR_S  1
/** TWAIFD_EWLI_INT_ENA_CLR : WO; bitpos: [2]; default: 0;
 *  Write 1 to clear TWAIFD_EWLI_INT_ENA .
 */
#define TWAIFD_EWLI_INT_ENA_CLR    (BIT(2))
#define TWAIFD_EWLI_INT_ENA_CLR_M  (TWAIFD_EWLI_INT_ENA_CLR_V << TWAIFD_EWLI_INT_ENA_CLR_S)
#define TWAIFD_EWLI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_EWLI_INT_ENA_CLR_S  2
/** TWAIFD_DOI_INT_ENA_CLR : WO; bitpos: [3]; default: 0;
 *  Write 1 to clear TWAIFD_DOI_INT_ENA .
 */
#define TWAIFD_DOI_INT_ENA_CLR    (BIT(3))
#define TWAIFD_DOI_INT_ENA_CLR_M  (TWAIFD_DOI_INT_ENA_CLR_V << TWAIFD_DOI_INT_ENA_CLR_S)
#define TWAIFD_DOI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_DOI_INT_ENA_CLR_S  3
/** TWAIFD_FCSI_INT_ENA_CLR : WO; bitpos: [4]; default: 0;
 *  Write 1 to clear TWAIFD_FCSI_INT_ENA .
 */
#define TWAIFD_FCSI_INT_ENA_CLR    (BIT(4))
#define TWAIFD_FCSI_INT_ENA_CLR_M  (TWAIFD_FCSI_INT_ENA_CLR_V << TWAIFD_FCSI_INT_ENA_CLR_S)
#define TWAIFD_FCSI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_FCSI_INT_ENA_CLR_S  4
/** TWAIFD_ALI_INT_ENA_CLR : WO; bitpos: [5]; default: 0;
 *  Write 1 to clear TWAIFD_ALI_INT_ENA .
 */
#define TWAIFD_ALI_INT_ENA_CLR    (BIT(5))
#define TWAIFD_ALI_INT_ENA_CLR_M  (TWAIFD_ALI_INT_ENA_CLR_V << TWAIFD_ALI_INT_ENA_CLR_S)
#define TWAIFD_ALI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_ALI_INT_ENA_CLR_S  5
/** TWAIFD_BEI_INT_ENA_CLR : WO; bitpos: [6]; default: 0;
 *  Write 1 to clear TWAIFD_BEI_INT_ENA .
 */
#define TWAIFD_BEI_INT_ENA_CLR    (BIT(6))
#define TWAIFD_BEI_INT_ENA_CLR_M  (TWAIFD_BEI_INT_ENA_CLR_V << TWAIFD_BEI_INT_ENA_CLR_S)
#define TWAIFD_BEI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_BEI_INT_ENA_CLR_S  6
/** TWAIFD_OFI_INT_ENA_CLR : WO; bitpos: [7]; default: 0;
 *  Write 1 to clear TWAIFD_OFI_INT_ENA .
 */
#define TWAIFD_OFI_INT_ENA_CLR    (BIT(7))
#define TWAIFD_OFI_INT_ENA_CLR_M  (TWAIFD_OFI_INT_ENA_CLR_V << TWAIFD_OFI_INT_ENA_CLR_S)
#define TWAIFD_OFI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_OFI_INT_ENA_CLR_S  7
/** TWAIFD_RXFI_INT_ENA_CLR : WO; bitpos: [8]; default: 0;
 *  Write 1 to clear TWAIFD_RXFI_INT_ENA .
 */
#define TWAIFD_RXFI_INT_ENA_CLR    (BIT(8))
#define TWAIFD_RXFI_INT_ENA_CLR_M  (TWAIFD_RXFI_INT_ENA_CLR_V << TWAIFD_RXFI_INT_ENA_CLR_S)
#define TWAIFD_RXFI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_RXFI_INT_ENA_CLR_S  8
/** TWAIFD_BSI_INT_ENA_CLR : WO; bitpos: [9]; default: 0;
 *  Write 1 to clear TWAIFD_BSI_INT_ENA .
 */
#define TWAIFD_BSI_INT_ENA_CLR    (BIT(9))
#define TWAIFD_BSI_INT_ENA_CLR_M  (TWAIFD_BSI_INT_ENA_CLR_V << TWAIFD_BSI_INT_ENA_CLR_S)
#define TWAIFD_BSI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_BSI_INT_ENA_CLR_S  9
/** TWAIFD_RBNEI_INT_ENA_CLR : WO; bitpos: [10]; default: 0;
 *  Write 1 to clear TWAIFD_RBNEI_INT_ENA .
 */
#define TWAIFD_RBNEI_INT_ENA_CLR    (BIT(10))
#define TWAIFD_RBNEI_INT_ENA_CLR_M  (TWAIFD_RBNEI_INT_ENA_CLR_V << TWAIFD_RBNEI_INT_ENA_CLR_S)
#define TWAIFD_RBNEI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_RBNEI_INT_ENA_CLR_S  10
/** TWAIFD_TXBHCI_INT_ENA_CLR : WO; bitpos: [11]; default: 0;
 *  Write 1 to clear TWAIFD_TXBHCI_INT_ENA .
 */
#define TWAIFD_TXBHCI_INT_ENA_CLR    (BIT(11))
#define TWAIFD_TXBHCI_INT_ENA_CLR_M  (TWAIFD_TXBHCI_INT_ENA_CLR_V << TWAIFD_TXBHCI_INT_ENA_CLR_S)
#define TWAIFD_TXBHCI_INT_ENA_CLR_V  0x00000001U
#define TWAIFD_TXBHCI_INT_ENA_CLR_S  11

/** TWAIFD_INT_MASK_SET_REG register
 *  TWAI FD interrupt mask register
 */
#define TWAIFD_INT_MASK_SET_REG(i) (REG_TWAI_BASE(i) + 0x1c)
/** TWAIFD_RXI_INT_MASK_SET : R/W1S; bitpos: [0]; default: 0;
 *  Write 1 to mask TWAIFD_RXI_INT.
 */
#define TWAIFD_RXI_INT_MASK_SET    (BIT(0))
#define TWAIFD_RXI_INT_MASK_SET_M  (TWAIFD_RXI_INT_MASK_SET_V << TWAIFD_RXI_INT_MASK_SET_S)
#define TWAIFD_RXI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_RXI_INT_MASK_SET_S  0
/** TWAIFD_TXI_INT_MASK_SET : R/W1S; bitpos: [1]; default: 0;
 *  Write 1 to mask TWAIFD_TXI_INT.
 */
#define TWAIFD_TXI_INT_MASK_SET    (BIT(1))
#define TWAIFD_TXI_INT_MASK_SET_M  (TWAIFD_TXI_INT_MASK_SET_V << TWAIFD_TXI_INT_MASK_SET_S)
#define TWAIFD_TXI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_TXI_INT_MASK_SET_S  1
/** TWAIFD_EWLI_INT_MASK_SET : R/W1S; bitpos: [2]; default: 0;
 *  Write 1 to mask TWAIFD_EWLI_INT.
 */
#define TWAIFD_EWLI_INT_MASK_SET    (BIT(2))
#define TWAIFD_EWLI_INT_MASK_SET_M  (TWAIFD_EWLI_INT_MASK_SET_V << TWAIFD_EWLI_INT_MASK_SET_S)
#define TWAIFD_EWLI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_EWLI_INT_MASK_SET_S  2
/** TWAIFD_DOI_INT_MASK_SET : R/W1S; bitpos: [3]; default: 0;
 *  Write 1 to mask TWAIFD_DOI_INT.
 */
#define TWAIFD_DOI_INT_MASK_SET    (BIT(3))
#define TWAIFD_DOI_INT_MASK_SET_M  (TWAIFD_DOI_INT_MASK_SET_V << TWAIFD_DOI_INT_MASK_SET_S)
#define TWAIFD_DOI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_DOI_INT_MASK_SET_S  3
/** TWAIFD_FCSI_INT_MASK_SET : R/W1S; bitpos: [4]; default: 0;
 *  Write 1 to mask TWAIFD_FCSI_INT.
 */
#define TWAIFD_FCSI_INT_MASK_SET    (BIT(4))
#define TWAIFD_FCSI_INT_MASK_SET_M  (TWAIFD_FCSI_INT_MASK_SET_V << TWAIFD_FCSI_INT_MASK_SET_S)
#define TWAIFD_FCSI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_FCSI_INT_MASK_SET_S  4
/** TWAIFD_ALI_INT_MASK_SET : R/W1S; bitpos: [5]; default: 0;
 *  Write 1 to mask TWAIFD_ALI_INT.
 */
#define TWAIFD_ALI_INT_MASK_SET    (BIT(5))
#define TWAIFD_ALI_INT_MASK_SET_M  (TWAIFD_ALI_INT_MASK_SET_V << TWAIFD_ALI_INT_MASK_SET_S)
#define TWAIFD_ALI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_ALI_INT_MASK_SET_S  5
/** TWAIFD_BEI_INT_MASK_SET : R/W1S; bitpos: [6]; default: 0;
 *  Write 1 to mask TWAIFD_BEI_INT.
 */
#define TWAIFD_BEI_INT_MASK_SET    (BIT(6))
#define TWAIFD_BEI_INT_MASK_SET_M  (TWAIFD_BEI_INT_MASK_SET_V << TWAIFD_BEI_INT_MASK_SET_S)
#define TWAIFD_BEI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_BEI_INT_MASK_SET_S  6
/** TWAIFD_OFI_INT_MASK_SET : R/W1S; bitpos: [7]; default: 0;
 *  Write 1 to mask TWAIFD_OFI_INT.
 */
#define TWAIFD_OFI_INT_MASK_SET    (BIT(7))
#define TWAIFD_OFI_INT_MASK_SET_M  (TWAIFD_OFI_INT_MASK_SET_V << TWAIFD_OFI_INT_MASK_SET_S)
#define TWAIFD_OFI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_OFI_INT_MASK_SET_S  7
/** TWAIFD_RXFI_INT_MASK_SET : R/W1S; bitpos: [8]; default: 0;
 *  Write 1 to mask TWAIFD_RXFI_INT.
 */
#define TWAIFD_RXFI_INT_MASK_SET    (BIT(8))
#define TWAIFD_RXFI_INT_MASK_SET_M  (TWAIFD_RXFI_INT_MASK_SET_V << TWAIFD_RXFI_INT_MASK_SET_S)
#define TWAIFD_RXFI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_RXFI_INT_MASK_SET_S  8
/** TWAIFD_BSI_INT_MASK_SET : R/W1S; bitpos: [9]; default: 0;
 *  Write 1 to mask TWAIFD_BSI_INT.
 */
#define TWAIFD_BSI_INT_MASK_SET    (BIT(9))
#define TWAIFD_BSI_INT_MASK_SET_M  (TWAIFD_BSI_INT_MASK_SET_V << TWAIFD_BSI_INT_MASK_SET_S)
#define TWAIFD_BSI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_BSI_INT_MASK_SET_S  9
/** TWAIFD_RBNEI_INT_MASK_SET : R/W1S; bitpos: [10]; default: 0;
 *  Write 1 to mask TWAIFD_RBNEI_INT.
 */
#define TWAIFD_RBNEI_INT_MASK_SET    (BIT(10))
#define TWAIFD_RBNEI_INT_MASK_SET_M  (TWAIFD_RBNEI_INT_MASK_SET_V << TWAIFD_RBNEI_INT_MASK_SET_S)
#define TWAIFD_RBNEI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_RBNEI_INT_MASK_SET_S  10
/** TWAIFD_TXBHCI_INT_MASK_SET : R/W1S; bitpos: [11]; default: 0;
 *  Write 1 to mask TWAIFD_TXBHCI_INT.
 */
#define TWAIFD_TXBHCI_INT_MASK_SET    (BIT(11))
#define TWAIFD_TXBHCI_INT_MASK_SET_M  (TWAIFD_TXBHCI_INT_MASK_SET_V << TWAIFD_TXBHCI_INT_MASK_SET_S)
#define TWAIFD_TXBHCI_INT_MASK_SET_V  0x00000001U
#define TWAIFD_TXBHCI_INT_MASK_SET_S  11

/** TWAIFD_INT_MASK_CLR_REG register
 *  TWAI FD interrupt mask clear register
 */
#define TWAIFD_INT_MASK_CLR_REG(i) (REG_TWAI_BASE(i) + 0x20)
/** TWAIFD_RXI_INT_MASK_CLR : WO; bitpos: [0]; default: 0;
 *  Write 1 to clear TWAIFD_RXI_INT_MASK_CLR .
 */
#define TWAIFD_RXI_INT_MASK_CLR    (BIT(0))
#define TWAIFD_RXI_INT_MASK_CLR_M  (TWAIFD_RXI_INT_MASK_CLR_V << TWAIFD_RXI_INT_MASK_CLR_S)
#define TWAIFD_RXI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_RXI_INT_MASK_CLR_S  0
/** TWAIFD_TXI_INT_MASK_CLR : WO; bitpos: [1]; default: 0;
 *  Write 1 to clear TWAIFD_TXI_INT_MASK_CLR .
 */
#define TWAIFD_TXI_INT_MASK_CLR    (BIT(1))
#define TWAIFD_TXI_INT_MASK_CLR_M  (TWAIFD_TXI_INT_MASK_CLR_V << TWAIFD_TXI_INT_MASK_CLR_S)
#define TWAIFD_TXI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_TXI_INT_MASK_CLR_S  1
/** TWAIFD_EWLI_INT_MASK_CLR : WO; bitpos: [2]; default: 0;
 *  Write 1 to clear TWAIFD_EWLI_INT_MASK_CLR .
 */
#define TWAIFD_EWLI_INT_MASK_CLR    (BIT(2))
#define TWAIFD_EWLI_INT_MASK_CLR_M  (TWAIFD_EWLI_INT_MASK_CLR_V << TWAIFD_EWLI_INT_MASK_CLR_S)
#define TWAIFD_EWLI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_EWLI_INT_MASK_CLR_S  2
/** TWAIFD_DOI_INT_MASK_CLR : WO; bitpos: [3]; default: 0;
 *  Write 1 to clear TWAIFD_DOI_INT_MASK_CLR .
 */
#define TWAIFD_DOI_INT_MASK_CLR    (BIT(3))
#define TWAIFD_DOI_INT_MASK_CLR_M  (TWAIFD_DOI_INT_MASK_CLR_V << TWAIFD_DOI_INT_MASK_CLR_S)
#define TWAIFD_DOI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_DOI_INT_MASK_CLR_S  3
/** TWAIFD_FCSI_INT_MASK_CLR : WO; bitpos: [4]; default: 0;
 *  Write 1 to clear TWAIFD_FCSI_INT_MASK_CLR .
 */
#define TWAIFD_FCSI_INT_MASK_CLR    (BIT(4))
#define TWAIFD_FCSI_INT_MASK_CLR_M  (TWAIFD_FCSI_INT_MASK_CLR_V << TWAIFD_FCSI_INT_MASK_CLR_S)
#define TWAIFD_FCSI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_FCSI_INT_MASK_CLR_S  4
/** TWAIFD_ALI_INT_MASK_CLR : WO; bitpos: [5]; default: 0;
 *  Write 1 to clear TWAIFD_ALI_INT_MASK_CLR .
 */
#define TWAIFD_ALI_INT_MASK_CLR    (BIT(5))
#define TWAIFD_ALI_INT_MASK_CLR_M  (TWAIFD_ALI_INT_MASK_CLR_V << TWAIFD_ALI_INT_MASK_CLR_S)
#define TWAIFD_ALI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_ALI_INT_MASK_CLR_S  5
/** TWAIFD_BEI_INT_MASK_CLR : WO; bitpos: [6]; default: 0;
 *  Write 1 to clear TWAIFD_BEI_INT_MASK_CLR .
 */
#define TWAIFD_BEI_INT_MASK_CLR    (BIT(6))
#define TWAIFD_BEI_INT_MASK_CLR_M  (TWAIFD_BEI_INT_MASK_CLR_V << TWAIFD_BEI_INT_MASK_CLR_S)
#define TWAIFD_BEI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_BEI_INT_MASK_CLR_S  6
/** TWAIFD_OFI_INT_MASK_CLR : WO; bitpos: [7]; default: 0;
 *  Write 1 to clear TWAIFD_OFI_INT_MASK_CLR .
 */
#define TWAIFD_OFI_INT_MASK_CLR    (BIT(7))
#define TWAIFD_OFI_INT_MASK_CLR_M  (TWAIFD_OFI_INT_MASK_CLR_V << TWAIFD_OFI_INT_MASK_CLR_S)
#define TWAIFD_OFI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_OFI_INT_MASK_CLR_S  7
/** TWAIFD_RXFI_INT_MASK_CLR : WO; bitpos: [8]; default: 0;
 *  Write 1 to clear TWAIFD_RXFI_INT_MASK_CLR .
 */
#define TWAIFD_RXFI_INT_MASK_CLR    (BIT(8))
#define TWAIFD_RXFI_INT_MASK_CLR_M  (TWAIFD_RXFI_INT_MASK_CLR_V << TWAIFD_RXFI_INT_MASK_CLR_S)
#define TWAIFD_RXFI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_RXFI_INT_MASK_CLR_S  8
/** TWAIFD_BSI_INT_MASK_CLR : WO; bitpos: [9]; default: 0;
 *  Write 1 to clear TWAIFD_BSI_INT_MASK_CLR .
 */
#define TWAIFD_BSI_INT_MASK_CLR    (BIT(9))
#define TWAIFD_BSI_INT_MASK_CLR_M  (TWAIFD_BSI_INT_MASK_CLR_V << TWAIFD_BSI_INT_MASK_CLR_S)
#define TWAIFD_BSI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_BSI_INT_MASK_CLR_S  9
/** TWAIFD_RBNEI_INT_MASK_CLR : WO; bitpos: [10]; default: 0;
 *  Write 1 to clear TWAIFD_RBNEI_INT_MASK_CLR .
 */
#define TWAIFD_RBNEI_INT_MASK_CLR    (BIT(10))
#define TWAIFD_RBNEI_INT_MASK_CLR_M  (TWAIFD_RBNEI_INT_MASK_CLR_V << TWAIFD_RBNEI_INT_MASK_CLR_S)
#define TWAIFD_RBNEI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_RBNEI_INT_MASK_CLR_S  10
/** TWAIFD_TXBHCI_INT_MASK_CLR : WO; bitpos: [11]; default: 0;
 *  Write 1 to clear TWAIFD_TXBHCI_INT_MASK_CLR .
 */
#define TWAIFD_TXBHCI_INT_MASK_CLR    (BIT(11))
#define TWAIFD_TXBHCI_INT_MASK_CLR_M  (TWAIFD_TXBHCI_INT_MASK_CLR_V << TWAIFD_TXBHCI_INT_MASK_CLR_S)
#define TWAIFD_TXBHCI_INT_MASK_CLR_V  0x00000001U
#define TWAIFD_TXBHCI_INT_MASK_CLR_S  11

/** TWAIFD_BTR_REG register
 *  TWAI FD bit-timing  register
 */
#define TWAIFD_BTR_REG(i) (REG_TWAI_BASE(i) + 0x24)
/** TWAIFD_PROP : R/W; bitpos: [6:0]; default: 5;
 *  Configures the propagation segment of nominal bit rate.
 *  Measurement unit: time quanta
 */
#define TWAIFD_PROP    0x0000007FU
#define TWAIFD_PROP_M  (TWAIFD_PROP_V << TWAIFD_PROP_S)
#define TWAIFD_PROP_V  0x0000007FU
#define TWAIFD_PROP_S  0
/** TWAIFD_PH1 : R/W; bitpos: [12:7]; default: 3;
 *  Configures the phase 1 segment of nominal bit rate.
 *  Measurement unit: time quanta
 */
#define TWAIFD_PH1    0x0000003FU
#define TWAIFD_PH1_M  (TWAIFD_PH1_V << TWAIFD_PH1_S)
#define TWAIFD_PH1_V  0x0000003FU
#define TWAIFD_PH1_S  7
/** TWAIFD_PH2 : R/W; bitpos: [18:13]; default: 5;
 *  Configures the phase 2 segment of nominal bit rate.
 *  Measurement unit: time quanta
 */
#define TWAIFD_PH2    0x0000003FU
#define TWAIFD_PH2_M  (TWAIFD_PH2_V << TWAIFD_PH2_S)
#define TWAIFD_PH2_V  0x0000003FU
#define TWAIFD_PH2_S  13
/** TWAIFD_BRP : R/W; bitpos: [26:19]; default: 10;
 *  Configures the baud-rate prescaler of nominal bit rate.
 *  Measurement unit: cycle of core clock.
 */
#define TWAIFD_BRP    0x000000FFU
#define TWAIFD_BRP_M  (TWAIFD_BRP_V << TWAIFD_BRP_S)
#define TWAIFD_BRP_V  0x000000FFU
#define TWAIFD_BRP_S  19
/** TWAIFD_SJW : R/W; bitpos: [31:27]; default: 2;
 *  Represents the synchronization jump width in nominal bit time.
 *  Measurement unit: time quanta
 */
#define TWAIFD_SJW    0x0000001FU
#define TWAIFD_SJW_M  (TWAIFD_SJW_V << TWAIFD_SJW_S)
#define TWAIFD_SJW_V  0x0000001FU
#define TWAIFD_SJW_S  27

/** TWAIFD_BTR_FD_REG register
 *  TWAI FD bit-timing of FD register
 */
#define TWAIFD_BTR_FD_REG(i) (REG_TWAI_BASE(i) + 0x28)
/** TWAIFD_PROP_FD : R/W; bitpos: [5:0]; default: 3;
 *  Configures the propagation segment of data bit rate.
 *  Measurement unit: time quanta
 */
#define TWAIFD_PROP_FD    0x0000003FU
#define TWAIFD_PROP_FD_M  (TWAIFD_PROP_FD_V << TWAIFD_PROP_FD_S)
#define TWAIFD_PROP_FD_V  0x0000003FU
#define TWAIFD_PROP_FD_S  0
/** TWAIFD_PH1_FD : R/W; bitpos: [11:7]; default: 3;
 *  Configures the phase 1 segment of data bit rate.
 *  Measurement unit: time quanta
 */
#define TWAIFD_PH1_FD    0x0000001FU
#define TWAIFD_PH1_FD_M  (TWAIFD_PH1_FD_V << TWAIFD_PH1_FD_S)
#define TWAIFD_PH1_FD_V  0x0000001FU
#define TWAIFD_PH1_FD_S  7
/** TWAIFD_PH2_FD : R/W; bitpos: [17:13]; default: 3;
 *  Configures the phase 2 segment of data bit rate.
 *  Measurement unit: time quanta
 */
#define TWAIFD_PH2_FD    0x0000001FU
#define TWAIFD_PH2_FD_M  (TWAIFD_PH2_FD_V << TWAIFD_PH2_FD_S)
#define TWAIFD_PH2_FD_V  0x0000001FU
#define TWAIFD_PH2_FD_S  13
/** TWAIFD_BRP_FD : R/W; bitpos: [26:19]; default: 4;
 *  Configures the baud-rate prescaler of data bit rate.
 *  Measurement unit: cycle of core clock.
 */
#define TWAIFD_BRP_FD    0x000000FFU
#define TWAIFD_BRP_FD_M  (TWAIFD_BRP_FD_V << TWAIFD_BRP_FD_S)
#define TWAIFD_BRP_FD_V  0x000000FFU
#define TWAIFD_BRP_FD_S  19
/** TWAIFD_SJW_FD : R/W; bitpos: [31:27]; default: 2;
 *  Represents the synchronization jump width in data bit time.
 *  Measurement unit: time quanta
 */
#define TWAIFD_SJW_FD    0x0000001FU
#define TWAIFD_SJW_FD_M  (TWAIFD_SJW_FD_V << TWAIFD_SJW_FD_S)
#define TWAIFD_SJW_FD_V  0x0000001FU
#define TWAIFD_SJW_FD_S  27

/** TWAIFD_EWL_ERP_FAULT_STATE_REG register
 *  TWAI FD error threshold and status register
 */
#define TWAIFD_EWL_ERP_FAULT_STATE_REG(i) (REG_TWAI_BASE(i) + 0x2c)
/** TWAIFD_EW_LIMIT : R/W; bitpos: [7:0]; default: 96;
 *  Error warning limit. If error warning limit is reached interrupt can be generated.
 *  Error warning limit
 *  indicates heavily disturbed bus.
 */
#define TWAIFD_EW_LIMIT    0x000000FFU
#define TWAIFD_EW_LIMIT_M  (TWAIFD_EW_LIMIT_V << TWAIFD_EW_LIMIT_S)
#define TWAIFD_EW_LIMIT_V  0x000000FFU
#define TWAIFD_EW_LIMIT_S  0
/** TWAIFD_ERP_LIMIT : R/W; bitpos: [15:8]; default: 128;
 *  Error Passive Limit. When one of error counters (REC/TEC) exceeds this value, Fault
 *  confinement state
 *  changes to error-passive.
 */
#define TWAIFD_ERP_LIMIT    0x000000FFU
#define TWAIFD_ERP_LIMIT_M  (TWAIFD_ERP_LIMIT_V << TWAIFD_ERP_LIMIT_S)
#define TWAIFD_ERP_LIMIT_V  0x000000FFU
#define TWAIFD_ERP_LIMIT_S  8
/** TWAIFD_ERA : RO; bitpos: [16]; default: 0;
 *  Represents the fault state of error active.
 */
#define TWAIFD_ERA    (BIT(16))
#define TWAIFD_ERA_M  (TWAIFD_ERA_V << TWAIFD_ERA_S)
#define TWAIFD_ERA_V  0x00000001U
#define TWAIFD_ERA_S  16
/** TWAIFD_ERP : RO; bitpos: [17]; default: 0;
 *  Represents the fault state of error passive.
 */
#define TWAIFD_ERP    (BIT(17))
#define TWAIFD_ERP_M  (TWAIFD_ERP_V << TWAIFD_ERP_S)
#define TWAIFD_ERP_V  0x00000001U
#define TWAIFD_ERP_S  17
/** TWAIFD_BOF : RO; bitpos: [18]; default: 1;
 *  Represents the fault state of bus off.
 */
#define TWAIFD_BOF    (BIT(18))
#define TWAIFD_BOF_M  (TWAIFD_BOF_V << TWAIFD_BOF_S)
#define TWAIFD_BOF_V  0x00000001U
#define TWAIFD_BOF_S  18

/** TWAIFD_REC_TEC_REG register
 *  TWAI FD error counters status register
 */
#define TWAIFD_REC_TEC_REG(i) (REG_TWAI_BASE(i) + 0x30)
/** TWAIFD_REC_VAL : RO; bitpos: [8:0]; default: 0;
 *  Represents the receiver error counter value.
 */
#define TWAIFD_REC_VAL    0x000001FFU
#define TWAIFD_REC_VAL_M  (TWAIFD_REC_VAL_V << TWAIFD_REC_VAL_S)
#define TWAIFD_REC_VAL_V  0x000001FFU
#define TWAIFD_REC_VAL_S  0
/** TWAIFD_TEC_VAL : RO; bitpos: [24:16]; default: 0;
 *  Represents the transmitter error counter value.
 */
#define TWAIFD_TEC_VAL    0x000001FFU
#define TWAIFD_TEC_VAL_M  (TWAIFD_TEC_VAL_V << TWAIFD_TEC_VAL_S)
#define TWAIFD_TEC_VAL_V  0x000001FFU
#define TWAIFD_TEC_VAL_S  16

/** TWAIFD_ERR_NORM_ERR_FD_REG register
 *  TWAI FD special error counters status register
 */
#define TWAIFD_ERR_NORM_ERR_FD_REG(i) (REG_TWAI_BASE(i) + 0x34)
/** TWAIFD_ERR_NORM_VAL : RO; bitpos: [15:0]; default: 0;
 *  Represents the number of error in the nominal bit time.
 */
#define TWAIFD_ERR_NORM_VAL    0x0000FFFFU
#define TWAIFD_ERR_NORM_VAL_M  (TWAIFD_ERR_NORM_VAL_V << TWAIFD_ERR_NORM_VAL_S)
#define TWAIFD_ERR_NORM_VAL_V  0x0000FFFFU
#define TWAIFD_ERR_NORM_VAL_S  0
/** TWAIFD_ERR_FD_VAL : RO; bitpos: [31:16]; default: 0;
 *  Represents the number of error in the data bit time.
 */
#define TWAIFD_ERR_FD_VAL    0x0000FFFFU
#define TWAIFD_ERR_FD_VAL_M  (TWAIFD_ERR_FD_VAL_V << TWAIFD_ERR_FD_VAL_S)
#define TWAIFD_ERR_FD_VAL_V  0x0000FFFFU
#define TWAIFD_ERR_FD_VAL_S  16

/** TWAIFD_CTR_PRES_REG register
 *  TWAI FD error counters pre-define configuration register
 */
#define TWAIFD_CTR_PRES_REG(i) (REG_TWAI_BASE(i) + 0x38)
/** TWAIFD_CTPV : WO; bitpos: [8:0]; default: 0;
 *  Configures the pre-defined value to set the error counter.
 */
#define TWAIFD_CTPV    0x000001FFU
#define TWAIFD_CTPV_M  (TWAIFD_CTPV_V << TWAIFD_CTPV_S)
#define TWAIFD_CTPV_V  0x000001FFU
#define TWAIFD_CTPV_S  0
/** TWAIFD_PTX : WT; bitpos: [9]; default: 0;
 *  Configures whether or not to set the receiver error counter into the value of
 *  pre-defined value.
 *  0: invalid
 *  1: set
 */
#define TWAIFD_PTX    (BIT(9))
#define TWAIFD_PTX_M  (TWAIFD_PTX_V << TWAIFD_PTX_S)
#define TWAIFD_PTX_V  0x00000001U
#define TWAIFD_PTX_S  9
/** TWAIFD_PRX : WT; bitpos: [10]; default: 0;
 *  Configures whether or not to set the transmitter error counter into the value of
 *  pre-defined value.
 *  0: invalid
 *  1: set
 */
#define TWAIFD_PRX    (BIT(10))
#define TWAIFD_PRX_M  (TWAIFD_PRX_V << TWAIFD_PRX_S)
#define TWAIFD_PRX_V  0x00000001U
#define TWAIFD_PRX_S  10
/** TWAIFD_ENORM : WO; bitpos: [11]; default: 0;
 *  Configures whether or not to erase the error counter of nominal bit time.
 *  0: invalid
 *  1: erase
 */
#define TWAIFD_ENORM    (BIT(11))
#define TWAIFD_ENORM_M  (TWAIFD_ENORM_V << TWAIFD_ENORM_S)
#define TWAIFD_ENORM_V  0x00000001U
#define TWAIFD_ENORM_S  11
/** TWAIFD_EFD : WO; bitpos: [12]; default: 0;
 *  Configures whether or not to erase the error counter of data bit time.
 *  0: invalid
 *  1: erase
 */
#define TWAIFD_EFD    (BIT(12))
#define TWAIFD_EFD_M  (TWAIFD_EFD_V << TWAIFD_EFD_S)
#define TWAIFD_EFD_V  0x00000001U
#define TWAIFD_EFD_S  12

/** TWAIFD_FILTER_A_MASK_REG register
 *  TWAI FD filter A mask value register
 */
#define TWAIFD_FILTER_A_MASK_REG(i) (REG_TWAI_BASE(i) + 0x3c)
/** TWAIFD_BIT_MASK_A_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Filter A mask. The identifier format is the same as in IDENTIFIER_W of TXT buffer
 *  or RX
 *  buffer. If filter A is not present, writes to this register have no effect and read
 *  will return all zeroes.
 */
#define TWAIFD_BIT_MASK_A_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_MASK_A_VAL_M  (TWAIFD_BIT_MASK_A_VAL_V << TWAIFD_BIT_MASK_A_VAL_S)
#define TWAIFD_BIT_MASK_A_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_MASK_A_VAL_S  0

/** TWAIFD_FILTER_A_VAL_REG register
 *  TWAI FD filter A bit value register
 */
#define TWAIFD_FILTER_A_VAL_REG(i) (REG_TWAI_BASE(i) + 0x40)
/** TWAIFD_BIT_VAL_A_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Filter A value. The identifier format is the same as in IDENTIFIER_W of TXT buffer
 *  or RX buffer.
 *  If filter A is not present, writes to this register have no effect and read will
 *  return all zeroes.
 */
#define TWAIFD_BIT_VAL_A_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_VAL_A_VAL_M  (TWAIFD_BIT_VAL_A_VAL_V << TWAIFD_BIT_VAL_A_VAL_S)
#define TWAIFD_BIT_VAL_A_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_VAL_A_VAL_S  0

/** TWAIFD_FILTER_B_MASK_REG register
 *  TWAI FD filter B mask value register
 */
#define TWAIFD_FILTER_B_MASK_REG(i) (REG_TWAI_BASE(i) + 0x44)
/** TWAIFD_BIT_MASK_B_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Filter B mask. The identifier format is the same as in IDENTIFIER_W of TXT buffer
 *  or RX
 *  buffer. If filter A is not present, writes to this register have no effect and read
 *  will return all zeroes.
 */
#define TWAIFD_BIT_MASK_B_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_MASK_B_VAL_M  (TWAIFD_BIT_MASK_B_VAL_V << TWAIFD_BIT_MASK_B_VAL_S)
#define TWAIFD_BIT_MASK_B_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_MASK_B_VAL_S  0

/** TWAIFD_FILTER_B_VAL_REG register
 *  TWAI FD filter B bit value register
 */
#define TWAIFD_FILTER_B_VAL_REG(i) (REG_TWAI_BASE(i) + 0x48)
/** TWAIFD_BIT_VAL_B_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Filter B value. The identifier format is the same as in IDENTIFIER_W of TXT buffer
 *  or RX buffer.
 *  If filter A is not present, writes to this register have no effect and read will
 *  return all zeroes.
 */
#define TWAIFD_BIT_VAL_B_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_VAL_B_VAL_M  (TWAIFD_BIT_VAL_B_VAL_V << TWAIFD_BIT_VAL_B_VAL_S)
#define TWAIFD_BIT_VAL_B_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_VAL_B_VAL_S  0

/** TWAIFD_FILTER_C_MASK_REG register
 *  TWAI FD filter C mask value register
 */
#define TWAIFD_FILTER_C_MASK_REG(i) (REG_TWAI_BASE(i) + 0x4c)
/** TWAIFD_BIT_MASK_C_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Filter C mask. The identifier format is the same as in IDENTIFIER_W of TXT buffer
 *  or RX
 *  buffer. If filter A is not present, writes to this register have no effect and read
 *  will return all zeroes.
 */
#define TWAIFD_BIT_MASK_C_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_MASK_C_VAL_M  (TWAIFD_BIT_MASK_C_VAL_V << TWAIFD_BIT_MASK_C_VAL_S)
#define TWAIFD_BIT_MASK_C_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_MASK_C_VAL_S  0

/** TWAIFD_FILTER_C_VAL_REG register
 *  TWAI FD filter C bit value register
 */
#define TWAIFD_FILTER_C_VAL_REG(i) (REG_TWAI_BASE(i) + 0x50)
/** TWAIFD_BIT_VAL_C_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Filter C value. The identifier format is the same as in IDENTIFIER_W of TXT buffer
 *  or RX buffer.
 *  If filter A is not present, writes to this register have no effect and read will
 *  return all zeroes.
 */
#define TWAIFD_BIT_VAL_C_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_VAL_C_VAL_M  (TWAIFD_BIT_VAL_C_VAL_V << TWAIFD_BIT_VAL_C_VAL_S)
#define TWAIFD_BIT_VAL_C_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_VAL_C_VAL_S  0

/** TWAIFD_FILTER_RAN_LOW_REG register
 *  TWAI FD filter range low value register
 */
#define TWAIFD_FILTER_RAN_LOW_REG(i) (REG_TWAI_BASE(i) + 0x54)
/** TWAIFD_BIT_RAN_LOW_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Filter Range Low threshold. The identifier format is the same as in IDENTIFIER_W of
 *  TXT
 *  buffer or RX buffer. If Range filter is not supported, writes to this register have
 *  no effect and read will return all
 *  Zeroes.
 */
#define TWAIFD_BIT_RAN_LOW_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_RAN_LOW_VAL_M  (TWAIFD_BIT_RAN_LOW_VAL_V << TWAIFD_BIT_RAN_LOW_VAL_S)
#define TWAIFD_BIT_RAN_LOW_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_RAN_LOW_VAL_S  0

/** TWAIFD_FILTER_RAN_HIGH_REG register
 *  TWAI FD filter range high value register
 */
#define TWAIFD_FILTER_RAN_HIGH_REG(i) (REG_TWAI_BASE(i) + 0x58)
/** TWAIFD_BIT_RAN_HIGH_VAL : R/W; bitpos: [28:0]; default: 0;
 *  Range filter High threshold. The identifier format is the same as in IDENTIFIER_W
 *  of TXT
 *  buffer or RX buffer. If Range filter is not supported, writes to this register have
 *  no effect and read will return all
 *  Zeroes.
 */
#define TWAIFD_BIT_RAN_HIGH_VAL    0x1FFFFFFFU
#define TWAIFD_BIT_RAN_HIGH_VAL_M  (TWAIFD_BIT_RAN_HIGH_VAL_V << TWAIFD_BIT_RAN_HIGH_VAL_S)
#define TWAIFD_BIT_RAN_HIGH_VAL_V  0x1FFFFFFFU
#define TWAIFD_BIT_RAN_HIGH_VAL_S  0

/** TWAIFD_FILTER_CONTROL_FILTER_STATUS_REG register
 *  TWAI FD filter control register
 */
#define TWAIFD_FILTER_CONTROL_FILTER_STATUS_REG(i) (REG_TWAI_BASE(i) + 0x5c)
/** TWAIFD_FANB : R/W; bitpos: [0]; default: 1;
 *  CAN Basic Frame is accepted by filter A.
 */
#define TWAIFD_FANB    (BIT(0))
#define TWAIFD_FANB_M  (TWAIFD_FANB_V << TWAIFD_FANB_S)
#define TWAIFD_FANB_V  0x00000001U
#define TWAIFD_FANB_S  0
/** TWAIFD_FANE : R/W; bitpos: [1]; default: 1;
 *  CAN Extended Frame is accepted by Filter A.
 */
#define TWAIFD_FANE    (BIT(1))
#define TWAIFD_FANE_M  (TWAIFD_FANE_V << TWAIFD_FANE_S)
#define TWAIFD_FANE_V  0x00000001U
#define TWAIFD_FANE_S  1
/** TWAIFD_FAFB : R/W; bitpos: [2]; default: 1;
 *  CAN FD Basic Frame is accepted by filter A.
 */
#define TWAIFD_FAFB    (BIT(2))
#define TWAIFD_FAFB_M  (TWAIFD_FAFB_V << TWAIFD_FAFB_S)
#define TWAIFD_FAFB_V  0x00000001U
#define TWAIFD_FAFB_S  2
/** TWAIFD_FAFE : R/W; bitpos: [3]; default: 1;
 *  CAN FD Extended Frame is accepted by filter A.
 */
#define TWAIFD_FAFE    (BIT(3))
#define TWAIFD_FAFE_M  (TWAIFD_FAFE_V << TWAIFD_FAFE_S)
#define TWAIFD_FAFE_V  0x00000001U
#define TWAIFD_FAFE_S  3
/** TWAIFD_FBNB : R/W; bitpos: [4]; default: 0;
 *  CAN Basic Frame is accepted by filter B.
 */
#define TWAIFD_FBNB    (BIT(4))
#define TWAIFD_FBNB_M  (TWAIFD_FBNB_V << TWAIFD_FBNB_S)
#define TWAIFD_FBNB_V  0x00000001U
#define TWAIFD_FBNB_S  4
/** TWAIFD_FBNE : R/W; bitpos: [5]; default: 0;
 *  CAN Extended Frame is accepted by Filter B.
 */
#define TWAIFD_FBNE    (BIT(5))
#define TWAIFD_FBNE_M  (TWAIFD_FBNE_V << TWAIFD_FBNE_S)
#define TWAIFD_FBNE_V  0x00000001U
#define TWAIFD_FBNE_S  5
/** TWAIFD_FBFB : R/W; bitpos: [6]; default: 0;
 *  CAN FD Basic Frame is accepted by filter B.
 */
#define TWAIFD_FBFB    (BIT(6))
#define TWAIFD_FBFB_M  (TWAIFD_FBFB_V << TWAIFD_FBFB_S)
#define TWAIFD_FBFB_V  0x00000001U
#define TWAIFD_FBFB_S  6
/** TWAIFD_FBFE : R/W; bitpos: [7]; default: 0;
 *  CAN FD Extended Frame is accepted by filter B.
 */
#define TWAIFD_FBFE    (BIT(7))
#define TWAIFD_FBFE_M  (TWAIFD_FBFE_V << TWAIFD_FBFE_S)
#define TWAIFD_FBFE_V  0x00000001U
#define TWAIFD_FBFE_S  7
/** TWAIFD_FCNB : R/W; bitpos: [8]; default: 0;
 *  CAN Basic Frame is accepted by filter C.
 */
#define TWAIFD_FCNB    (BIT(8))
#define TWAIFD_FCNB_M  (TWAIFD_FCNB_V << TWAIFD_FCNB_S)
#define TWAIFD_FCNB_V  0x00000001U
#define TWAIFD_FCNB_S  8
/** TWAIFD_FCNE : R/W; bitpos: [9]; default: 0;
 *  CAN Extended Frame is accepted by Filter C.
 */
#define TWAIFD_FCNE    (BIT(9))
#define TWAIFD_FCNE_M  (TWAIFD_FCNE_V << TWAIFD_FCNE_S)
#define TWAIFD_FCNE_V  0x00000001U
#define TWAIFD_FCNE_S  9
/** TWAIFD_FCFB : R/W; bitpos: [10]; default: 0;
 *  CAN FD Basic Frame is accepted by filter C.
 */
#define TWAIFD_FCFB    (BIT(10))
#define TWAIFD_FCFB_M  (TWAIFD_FCFB_V << TWAIFD_FCFB_S)
#define TWAIFD_FCFB_V  0x00000001U
#define TWAIFD_FCFB_S  10
/** TWAIFD_FCFE : R/W; bitpos: [11]; default: 0;
 *  CAN FD Extended Frame is accepted by filter C.
 */
#define TWAIFD_FCFE    (BIT(11))
#define TWAIFD_FCFE_M  (TWAIFD_FCFE_V << TWAIFD_FCFE_S)
#define TWAIFD_FCFE_V  0x00000001U
#define TWAIFD_FCFE_S  11
/** TWAIFD_FRNB : R/W; bitpos: [12]; default: 0;
 *  CAN Basic Frame is accepted by Range filter.
 */
#define TWAIFD_FRNB    (BIT(12))
#define TWAIFD_FRNB_M  (TWAIFD_FRNB_V << TWAIFD_FRNB_S)
#define TWAIFD_FRNB_V  0x00000001U
#define TWAIFD_FRNB_S  12
/** TWAIFD_FRNE : R/W; bitpos: [13]; default: 0;
 *  CAN Extended Frame is accepted by Range filter.
 */
#define TWAIFD_FRNE    (BIT(13))
#define TWAIFD_FRNE_M  (TWAIFD_FRNE_V << TWAIFD_FRNE_S)
#define TWAIFD_FRNE_V  0x00000001U
#define TWAIFD_FRNE_S  13
/** TWAIFD_FRFB : R/W; bitpos: [14]; default: 0;
 *  CAN FD Basic Frame is accepted by Range filter.
 */
#define TWAIFD_FRFB    (BIT(14))
#define TWAIFD_FRFB_M  (TWAIFD_FRFB_V << TWAIFD_FRFB_S)
#define TWAIFD_FRFB_V  0x00000001U
#define TWAIFD_FRFB_S  14
/** TWAIFD_FRFE : R/W; bitpos: [15]; default: 0;
 *  CAN FD Extended Frame is accepted by Range filter.
 */
#define TWAIFD_FRFE    (BIT(15))
#define TWAIFD_FRFE_M  (TWAIFD_FRFE_V << TWAIFD_FRFE_S)
#define TWAIFD_FRFE_V  0x00000001U
#define TWAIFD_FRFE_S  15
/** TWAIFD_SFA : RO; bitpos: [16]; default: 1;
 *  Logic 1 when Filter A is available. Otherwise logic 0.
 */
#define TWAIFD_SFA    (BIT(16))
#define TWAIFD_SFA_M  (TWAIFD_SFA_V << TWAIFD_SFA_S)
#define TWAIFD_SFA_V  0x00000001U
#define TWAIFD_SFA_S  16
/** TWAIFD_SFB : RO; bitpos: [17]; default: 1;
 *  Logic 1 when Filter B is available. Otherwise logic 0.
 */
#define TWAIFD_SFB    (BIT(17))
#define TWAIFD_SFB_M  (TWAIFD_SFB_V << TWAIFD_SFB_S)
#define TWAIFD_SFB_V  0x00000001U
#define TWAIFD_SFB_S  17
/** TWAIFD_SFC : RO; bitpos: [18]; default: 1;
 *  Logic 1 when Filter C is available. Otherwise logic 0.
 */
#define TWAIFD_SFC    (BIT(18))
#define TWAIFD_SFC_M  (TWAIFD_SFC_V << TWAIFD_SFC_S)
#define TWAIFD_SFC_V  0x00000001U
#define TWAIFD_SFC_S  18
/** TWAIFD_SFR : RO; bitpos: [19]; default: 1;
 *  Logic 1 when Range Filter is available. Otherwise logic 0.
 */
#define TWAIFD_SFR    (BIT(19))
#define TWAIFD_SFR_M  (TWAIFD_SFR_V << TWAIFD_SFR_S)
#define TWAIFD_SFR_V  0x00000001U
#define TWAIFD_SFR_S  19

/** TWAIFD_RX_MEM_INFO_REG register
 *  TWAI FD rx memory information register
 */
#define TWAIFD_RX_MEM_INFO_REG(i) (REG_TWAI_BASE(i) + 0x60)
/** TWAIFD_RX_BUFF_SIZE : RO; bitpos: [12:0]; default: 128;
 *  Size of RX buffer in 32-bit words.
 */
#define TWAIFD_RX_BUFF_SIZE    0x00001FFFU
#define TWAIFD_RX_BUFF_SIZE_M  (TWAIFD_RX_BUFF_SIZE_V << TWAIFD_RX_BUFF_SIZE_S)
#define TWAIFD_RX_BUFF_SIZE_V  0x00001FFFU
#define TWAIFD_RX_BUFF_SIZE_S  0
/** TWAIFD_RX_FREE : RO; bitpos: [28:16]; default: 128;
 *  Number of free 32 bit words in RX buffer.
 */
#define TWAIFD_RX_FREE    0x00001FFFU
#define TWAIFD_RX_FREE_M  (TWAIFD_RX_FREE_V << TWAIFD_RX_FREE_S)
#define TWAIFD_RX_FREE_V  0x00001FFFU
#define TWAIFD_RX_FREE_S  16

/** TWAIFD_RX_POINTERS_REG register
 *  TWAI FD rx memory pointer information register
 */
#define TWAIFD_RX_POINTERS_REG(i) (REG_TWAI_BASE(i) + 0x64)
/** TWAIFD_RX_WPP : RO; bitpos: [11:0]; default: 0;
 *  Write pointer position in RX buffer. Upon store of received frame write pointer is
 *  updated.
 */
#define TWAIFD_RX_WPP    0x00000FFFU
#define TWAIFD_RX_WPP_M  (TWAIFD_RX_WPP_V << TWAIFD_RX_WPP_S)
#define TWAIFD_RX_WPP_V  0x00000FFFU
#define TWAIFD_RX_WPP_S  0
/** TWAIFD_RX_RPP : RO; bitpos: [27:16]; default: 0;
 *  Read pointer position in RX buffer. Upon read of received frame read pointer is
 *  updated.
 */
#define TWAIFD_RX_RPP    0x00000FFFU
#define TWAIFD_RX_RPP_M  (TWAIFD_RX_RPP_V << TWAIFD_RX_RPP_S)
#define TWAIFD_RX_RPP_V  0x00000FFFU
#define TWAIFD_RX_RPP_S  16

/** TWAIFD_RX_STATUS_RX_SETTINGS_REG register
 *  TWAI FD rx status & setting register
 */
#define TWAIFD_RX_STATUS_RX_SETTINGS_REG(i) (REG_TWAI_BASE(i) + 0x68)
/** TWAIFD_RXE : RO; bitpos: [0]; default: 1;
 *  Represents whether or not the RX buffer is empty. RX buffer is empty. There is no
 *  CAN Frame stored in it.
 *  0: not empty
 *  1: empty
 */
#define TWAIFD_RXE    (BIT(0))
#define TWAIFD_RXE_M  (TWAIFD_RXE_V << TWAIFD_RXE_S)
#define TWAIFD_RXE_V  0x00000001U
#define TWAIFD_RXE_S  0
/** TWAIFD_RXF : RO; bitpos: [1]; default: 0;
 *  Represents whether or not the RX buffer is full. RX buffer is full, all memory
 *  words of RX buffer are occupied.
 *  0: not full
 *  1: full
 */
#define TWAIFD_RXF    (BIT(1))
#define TWAIFD_RXF_M  (TWAIFD_RXF_V << TWAIFD_RXF_S)
#define TWAIFD_RXF_V  0x00000001U
#define TWAIFD_RXF_S  1
/** TWAIFD_RXMOF : RO; bitpos: [2]; default: 0;
 *  Represents the number of received frame in RX buffer.
 *  RX Buffer middle of frame. When RXMOF = 1, next read from RX_DATA register will
 *  return other than first
 *  word (FRAME_FORMAT_W) of CAN frame.
 */
#define TWAIFD_RXMOF    (BIT(2))
#define TWAIFD_RXMOF_M  (TWAIFD_RXMOF_V << TWAIFD_RXMOF_S)
#define TWAIFD_RXMOF_V  0x00000001U
#define TWAIFD_RXMOF_S  2
/** TWAIFD_RXFRC : RO; bitpos: [14:4]; default: 0;
 *  RX buffer frame count. Number of CAN frames stored in RX buffer.
 */
#define TWAIFD_RXFRC    0x000007FFU
#define TWAIFD_RXFRC_M  (TWAIFD_RXFRC_V << TWAIFD_RXFRC_S)
#define TWAIFD_RXFRC_V  0x000007FFU
#define TWAIFD_RXFRC_S  4
/** TWAIFD_RTSOP : R/W; bitpos: [16]; default: 0;
 *  Receive buffer timestamp option. This register should be modified only when
 *  SETTINGS[ENA]=0.
 *  0b0 - RTS_END - Timestamp of received frame in RX FIFO is captured in last bit of
 *  EOF field.
 *  0b1 - RTS_BEG - Timestamp of received frame in RX FIFO is captured in SOF field.
 */
#define TWAIFD_RTSOP    (BIT(16))
#define TWAIFD_RTSOP_M  (TWAIFD_RTSOP_V << TWAIFD_RTSOP_S)
#define TWAIFD_RTSOP_V  0x00000001U
#define TWAIFD_RTSOP_S  16

/** TWAIFD_RX_DATA_REG register
 *  TWAI FD received data register
 */
#define TWAIFD_RX_DATA_REG(i) (REG_TWAI_BASE(i) + 0x6c)
/** TWAIFD_RX_DATA : RO; bitpos: [31:0]; default: 0;
 *  RX buffer data at read pointer position in FIFO. By reading from this register,
 *  read pointer is auto-
 *  matically increased, as long as there is next data word stored in RX buffer. First
 *  stored word in the buffer is
 *  FRAME_FORMAT_W, next IDENTIFIER_W etc. This register shall be read by 32 bit access.
 */
#define TWAIFD_RX_DATA    0xFFFFFFFFU
#define TWAIFD_RX_DATA_M  (TWAIFD_RX_DATA_V << TWAIFD_RX_DATA_S)
#define TWAIFD_RX_DATA_V  0xFFFFFFFFU
#define TWAIFD_RX_DATA_S  0

/** TWAIFD_TX_STATUS_REG register
 *  TWAI FD TX buffer status register
 */
#define TWAIFD_TX_STATUS_REG(i) (REG_TWAI_BASE(i) + 0x70)
/** TWAIFD_TXTB0_STATE : RO; bitpos: [3:0]; default: 8;
 *  Status of TXT buffer 1.
 *  0b0000 - TXT_NOT_EXIST - TXT buffer does not exist in the core (applies only to TXT
 *  buffers 3-8, when CTU
 *  CAN FD was synthesized with less than 8 TXT buffers).
 *  0b0001 - TXT_RDY - TXT buffer is in "Ready" state, it is waiting for CTU CAN FD to
 *  start transmission from it.
 *  0b0010 - TXT_TRAN - TXT buffer is in "TX in progress" state. CTU CAN FD is
 *  transmitting frame.
 *  0b0011 - TXT_ABTP - TXT buffer is in "Abort in progress" state.
 *  0b0100 - TXT_TOK - TXT buffer is in "TX OK" state.
 *  0b0110 - TXT_ERR - TXT buffer is in "Failed" state.
 *  0b0111 - TXT_ABT - TXT buffer is in "Aborted" state.
 *  0b1000 - TXT_ETY - TXT buffer is in "Empty" state.
 */
#define TWAIFD_TXTB0_STATE    0x0000000FU
#define TWAIFD_TXTB0_STATE_M  (TWAIFD_TXTB0_STATE_V << TWAIFD_TXTB0_STATE_S)
#define TWAIFD_TXTB0_STATE_V  0x0000000FU
#define TWAIFD_TXTB0_STATE_S  0
/** TWAIFD_TX2S : RO; bitpos: [7:4]; default: 8;
 *  Status of TXT buffer 2. Bit field meaning is analogous to TX1S.
 */
#define TWAIFD_TX2S    0x0000000FU
#define TWAIFD_TX2S_M  (TWAIFD_TX2S_V << TWAIFD_TX2S_S)
#define TWAIFD_TX2S_V  0x0000000FU
#define TWAIFD_TX2S_S  4
/** TWAIFD_TX3S : RO; bitpos: [11:8]; default: 8;
 *  Status of TXT buffer 3. Bit field meaning is analogous to TX1S.
 */
#define TWAIFD_TX3S    0x0000000FU
#define TWAIFD_TX3S_M  (TWAIFD_TX3S_V << TWAIFD_TX3S_S)
#define TWAIFD_TX3S_V  0x0000000FU
#define TWAIFD_TX3S_S  8
/** TWAIFD_TX4S : RO; bitpos: [15:12]; default: 8;
 *  Status of TXT buffer 4. Bit field meaning is analogous to TX1S.
 */
#define TWAIFD_TX4S    0x0000000FU
#define TWAIFD_TX4S_M  (TWAIFD_TX4S_V << TWAIFD_TX4S_S)
#define TWAIFD_TX4S_V  0x0000000FU
#define TWAIFD_TX4S_S  12
/** TWAIFD_TX5S : RO; bitpos: [19:16]; default: 0;
 *  Status of TXT buffer 5. Bit field meaning is analogous to TX1S.
 */
#define TWAIFD_TX5S    0x0000000FU
#define TWAIFD_TX5S_M  (TWAIFD_TX5S_V << TWAIFD_TX5S_S)
#define TWAIFD_TX5S_V  0x0000000FU
#define TWAIFD_TX5S_S  16
/** TWAIFD_TX6S : RO; bitpos: [23:20]; default: 0;
 *  Status of TXT buffer 6. Bit field meaning is analogous to TX1S.
 */
#define TWAIFD_TX6S    0x0000000FU
#define TWAIFD_TX6S_M  (TWAIFD_TX6S_V << TWAIFD_TX6S_S)
#define TWAIFD_TX6S_V  0x0000000FU
#define TWAIFD_TX6S_S  20
/** TWAIFD_TX7S : RO; bitpos: [27:24]; default: 0;
 *  Status of TXT buffer 7. Bit field meaning is analogous to TX1S.
 */
#define TWAIFD_TX7S    0x0000000FU
#define TWAIFD_TX7S_M  (TWAIFD_TX7S_V << TWAIFD_TX7S_S)
#define TWAIFD_TX7S_V  0x0000000FU
#define TWAIFD_TX7S_S  24
/** TWAIFD_TX8S : RO; bitpos: [31:28]; default: 0;
 *  Status of TXT buffer 8. Bit field meaning is analogous to TX1S.
 */
#define TWAIFD_TX8S    0x0000000FU
#define TWAIFD_TX8S_M  (TWAIFD_TX8S_V << TWAIFD_TX8S_S)
#define TWAIFD_TX8S_V  0x0000000FU
#define TWAIFD_TX8S_S  28

/** TWAIFD_TX_COMMAND_TXTB_INFO_REG register
 *  TWAI FD TXT buffer command & information register
 */
#define TWAIFD_TX_COMMAND_TXTB_INFO_REG(i) (REG_TWAI_BASE(i) + 0x74)
/** TWAIFD_TXCE : WO; bitpos: [0]; default: 0;
 *  Issues "set empty" command.
 */
#define TWAIFD_TXCE    (BIT(0))
#define TWAIFD_TXCE_M  (TWAIFD_TXCE_V << TWAIFD_TXCE_S)
#define TWAIFD_TXCE_V  0x00000001U
#define TWAIFD_TXCE_S  0
/** TWAIFD_TXCR : WO; bitpos: [1]; default: 0;
 *  Issues "set ready" command.
 */
#define TWAIFD_TXCR    (BIT(1))
#define TWAIFD_TXCR_M  (TWAIFD_TXCR_V << TWAIFD_TXCR_S)
#define TWAIFD_TXCR_V  0x00000001U
#define TWAIFD_TXCR_S  1
/** TWAIFD_TXCA : WO; bitpos: [2]; default: 0;
 *  Issues "set abort" command.
 */
#define TWAIFD_TXCA    (BIT(2))
#define TWAIFD_TXCA_M  (TWAIFD_TXCA_V << TWAIFD_TXCA_S)
#define TWAIFD_TXCA_V  0x00000001U
#define TWAIFD_TXCA_S  2
/** TWAIFD_TXB1 : WO; bitpos: [8]; default: 0;
 *  Command is issued to TXT Buffer 1.
 */
#define TWAIFD_TXB1    (BIT(8))
#define TWAIFD_TXB1_M  (TWAIFD_TXB1_V << TWAIFD_TXB1_S)
#define TWAIFD_TXB1_V  0x00000001U
#define TWAIFD_TXB1_S  8
/** TWAIFD_TXB2 : WO; bitpos: [9]; default: 0;
 *  Command is issued to TXT Buffer 2.
 */
#define TWAIFD_TXB2    (BIT(9))
#define TWAIFD_TXB2_M  (TWAIFD_TXB2_V << TWAIFD_TXB2_S)
#define TWAIFD_TXB2_V  0x00000001U
#define TWAIFD_TXB2_S  9
/** TWAIFD_TXB3 : WO; bitpos: [10]; default: 0;
 *  Command is issued to TXT Buffer 3. If number of TXT Buffers is less than 3, this
 *  field is reserved and has no
 *  Function.
 */
#define TWAIFD_TXB3    (BIT(10))
#define TWAIFD_TXB3_M  (TWAIFD_TXB3_V << TWAIFD_TXB3_S)
#define TWAIFD_TXB3_V  0x00000001U
#define TWAIFD_TXB3_S  10
/** TWAIFD_TXB4 : WO; bitpos: [11]; default: 0;
 *  Command is issued to TXT Buffer 4. If number of TXT Buffers is less than 4, this
 *  field is reserved and has no
 *  Function.
 */
#define TWAIFD_TXB4    (BIT(11))
#define TWAIFD_TXB4_M  (TWAIFD_TXB4_V << TWAIFD_TXB4_S)
#define TWAIFD_TXB4_V  0x00000001U
#define TWAIFD_TXB4_S  11
/** TWAIFD_TXB5 : WO; bitpos: [12]; default: 0;
 *  Command is issued to TXT Buffer 5. If number of TXT Buffers is less than 5, this
 *  field is reserved and has no
 *  Function.
 */
#define TWAIFD_TXB5    (BIT(12))
#define TWAIFD_TXB5_M  (TWAIFD_TXB5_V << TWAIFD_TXB5_S)
#define TWAIFD_TXB5_V  0x00000001U
#define TWAIFD_TXB5_S  12
/** TWAIFD_TXB6 : WO; bitpos: [13]; default: 0;
 *  Command is issued to TXT Buffer 6. If number of TXT Buffers is less than 6, this
 *  field is reserved and has no
 *  Function.
 */
#define TWAIFD_TXB6    (BIT(13))
#define TWAIFD_TXB6_M  (TWAIFD_TXB6_V << TWAIFD_TXB6_S)
#define TWAIFD_TXB6_V  0x00000001U
#define TWAIFD_TXB6_S  13
/** TWAIFD_TXB7 : WO; bitpos: [14]; default: 0;
 *  Command is issued to TXT Buffer 7. If number of TXT Buffers is less than 7, this
 *  field is reserved and has no
 *  Function.
 */
#define TWAIFD_TXB7    (BIT(14))
#define TWAIFD_TXB7_M  (TWAIFD_TXB7_V << TWAIFD_TXB7_S)
#define TWAIFD_TXB7_V  0x00000001U
#define TWAIFD_TXB7_S  14
/** TWAIFD_TXB8 : WO; bitpos: [15]; default: 0;
 *  Command is issued to TXT Buffer 8. If number of TXT Buffers is less than 8, this
 *  field is reserved and has no
 *  Function.
 */
#define TWAIFD_TXB8    (BIT(15))
#define TWAIFD_TXB8_M  (TWAIFD_TXB8_V << TWAIFD_TXB8_S)
#define TWAIFD_TXB8_V  0x00000001U
#define TWAIFD_TXB8_S  15
/** TWAIFD_TXT_BUFFER_COUNT : RO; bitpos: [19:16]; default: 4;
 *  Number of TXT buffers present in CTU CAN FD. Lowest buffer is always 1. Highest
 *  buffer
 *  is at index equal to number of present buffers.
 */
#define TWAIFD_TXT_BUFFER_COUNT    0x0000000FU
#define TWAIFD_TXT_BUFFER_COUNT_M  (TWAIFD_TXT_BUFFER_COUNT_V << TWAIFD_TXT_BUFFER_COUNT_S)
#define TWAIFD_TXT_BUFFER_COUNT_V  0x0000000FU
#define TWAIFD_TXT_BUFFER_COUNT_S  16

/** TWAIFD_TX_PRIORITY_REG register
 *  TWAI FD TXT buffer command & information register
 */
#define TWAIFD_TX_PRIORITY_REG(i) (REG_TWAI_BASE(i) + 0x78)
/** TWAIFD_TXT1P : R/W; bitpos: [2:0]; default: 1;
 *  Priority of TXT buffer 1.
 */
#define TWAIFD_TXT1P    0x00000007U
#define TWAIFD_TXT1P_M  (TWAIFD_TXT1P_V << TWAIFD_TXT1P_S)
#define TWAIFD_TXT1P_V  0x00000007U
#define TWAIFD_TXT1P_S  0
/** TWAIFD_TXT2P : R/W; bitpos: [6:4]; default: 0;
 *  Priority of TXT buffer 2.
 */
#define TWAIFD_TXT2P    0x00000007U
#define TWAIFD_TXT2P_M  (TWAIFD_TXT2P_V << TWAIFD_TXT2P_S)
#define TWAIFD_TXT2P_V  0x00000007U
#define TWAIFD_TXT2P_S  4
/** TWAIFD_TXT3P : R/W; bitpos: [10:8]; default: 0;
 *  Priority of TXT buffer 3. If number of TXT Buffers is less than 3, this field is
 *  reserved and has no function.
 */
#define TWAIFD_TXT3P    0x00000007U
#define TWAIFD_TXT3P_M  (TWAIFD_TXT3P_V << TWAIFD_TXT3P_S)
#define TWAIFD_TXT3P_V  0x00000007U
#define TWAIFD_TXT3P_S  8
/** TWAIFD_TXT4P : R/W; bitpos: [14:12]; default: 0;
 *  Priority of TXT buffer 4. If number of TXT Buffers is less than 4, this field is
 *  reserved and has no function.
 */
#define TWAIFD_TXT4P    0x00000007U
#define TWAIFD_TXT4P_M  (TWAIFD_TXT4P_V << TWAIFD_TXT4P_S)
#define TWAIFD_TXT4P_V  0x00000007U
#define TWAIFD_TXT4P_S  12
/** TWAIFD_TXT5P : R/W; bitpos: [18:16]; default: 0;
 *  Priority of TXT buffer 5. If number of TXT Buffers is less than 5, this field is
 *  reserved and has no function.
 */
#define TWAIFD_TXT5P    0x00000007U
#define TWAIFD_TXT5P_M  (TWAIFD_TXT5P_V << TWAIFD_TXT5P_S)
#define TWAIFD_TXT5P_V  0x00000007U
#define TWAIFD_TXT5P_S  16
/** TWAIFD_TXT6P : R/W; bitpos: [22:20]; default: 0;
 *  Priority of TXT buffer 6. If number of TXT Buffers is less than 6, this field is
 *  reserved and has no function.
 */
#define TWAIFD_TXT6P    0x00000007U
#define TWAIFD_TXT6P_M  (TWAIFD_TXT6P_V << TWAIFD_TXT6P_S)
#define TWAIFD_TXT6P_V  0x00000007U
#define TWAIFD_TXT6P_S  20
/** TWAIFD_TXT7P : R/W; bitpos: [26:24]; default: 0;
 *  Priority of TXT buffer 7. If number of TXT Buffers is less than 7, this field is
 *  reserved and has no function.
 */
#define TWAIFD_TXT7P    0x00000007U
#define TWAIFD_TXT7P_M  (TWAIFD_TXT7P_V << TWAIFD_TXT7P_S)
#define TWAIFD_TXT7P_V  0x00000007U
#define TWAIFD_TXT7P_S  24
/** TWAIFD_TXT8P : R/W; bitpos: [30:28]; default: 0;
 *  Priority of TXT buffer 8. If number of TXT Buffers is less than 8, this field is
 *  reserved and has no function.
 */
#define TWAIFD_TXT8P    0x00000007U
#define TWAIFD_TXT8P_M  (TWAIFD_TXT8P_V << TWAIFD_TXT8P_S)
#define TWAIFD_TXT8P_V  0x00000007U
#define TWAIFD_TXT8P_S  28

/** TWAIFD_ERR_CAPT_RETR_CTR_ALC_TS_INFO_REG register
 *  TWAI FD error capture & retransmit counter & arbitration lost & timestamp
 *  integration information register
 */
#define TWAIFD_ERR_CAPT_RETR_CTR_ALC_TS_INFO_REG(i) (REG_TWAI_BASE(i) + 0x7c)
/** TWAIFD_ERR_POS : RO; bitpos: [4:0]; default: 31;
 *  0b00000 - ERC_POS_SOF - Error in Start of Frame
 *  0b00001 - ERC_POS_ARB - Error in Arbitration Filed
 *  0b00010 - ERC_POS_CTRL - Error in Control field
 *  0b00011 - ERC_POS_DATA - Error in Data Field
 *  0b00100 - ERC_POS_CRC - Error in CRC Field
 *  0b00101 - ERC_POS_ACK - Error in CRC delimiter, ACK field or ACK delimiter
 *  0b00110 - ERC_POS_EOF - Error in End of frame field
 *  0b00111 - ERC_POS_ERR - Error during Error frame
 *  0b01000 - ERC_POS_OVRL - Error in Overload frame
 *  0b11111 - ERC_POS_OTHER - Other position of error
 */
#define TWAIFD_ERR_POS    0x0000001FU
#define TWAIFD_ERR_POS_M  (TWAIFD_ERR_POS_V << TWAIFD_ERR_POS_S)
#define TWAIFD_ERR_POS_V  0x0000001FU
#define TWAIFD_ERR_POS_S  0
/** TWAIFD_ERR_TYPE : RO; bitpos: [7:5]; default: 0;
 *  Type of last error.
 *  0b000 - ERC_BIT_ERR - Bit Error
 *  0b001 - ERC_CRC_ERR - CRC Error
 *  0b010 - ERC_FRM_ERR - Form Error
 *  0b011 - ERC_ACK_ERR - Acknowledge Error
 *  0b100 - ERC_STUF_ERR - Stuff Error
 */
#define TWAIFD_ERR_TYPE    0x00000007U
#define TWAIFD_ERR_TYPE_M  (TWAIFD_ERR_TYPE_V << TWAIFD_ERR_TYPE_S)
#define TWAIFD_ERR_TYPE_V  0x00000007U
#define TWAIFD_ERR_TYPE_S  5
/** TWAIFD_RETR_CTR_VAL : RO; bitpos: [11:8]; default: 0;
 *  Current value of retransmitt counter.
 */
#define TWAIFD_RETR_CTR_VAL    0x0000000FU
#define TWAIFD_RETR_CTR_VAL_M  (TWAIFD_RETR_CTR_VAL_V << TWAIFD_RETR_CTR_VAL_S)
#define TWAIFD_RETR_CTR_VAL_V  0x0000000FU
#define TWAIFD_RETR_CTR_VAL_S  8
/** TWAIFD_ALC_BIT : RO; bitpos: [20:16]; default: 0;
 *  Arbitration lost capture bit position. If ALC_ID_FIELD = ALC_BASE_ID then bit index
 *  of BASE identifier
 *  in which arbitration was lost is given as: 11 - ALC_VAL. If ALC_ID_FIELD =
 *  ALC_EXTENSION then bit index of
 *  EXTENDED identifier in which arbitration was lost is given as: 18 - ALC_VAL. For
 *  other values of ALC_ID_FIELD,
 *  this value is undefined.
 */
#define TWAIFD_ALC_BIT    0x0000001FU
#define TWAIFD_ALC_BIT_M  (TWAIFD_ALC_BIT_V << TWAIFD_ALC_BIT_S)
#define TWAIFD_ALC_BIT_V  0x0000001FU
#define TWAIFD_ALC_BIT_S  16
/** TWAIFD_ALC_ID_FIELD : RO; bitpos: [23:21]; default: 0;
 *  Part of CAN Identifier in which arbitration was lost.
 *  0b000 - ALC_RSVD - Unit did not loose arbitration since last reset.
 *  0b001 - ALC_BASE_ID - Arbitration was lost during base identifier.
 *  0b010 - ALC_SRR_RTR - Arbitration was lost during first bit after base identifier
 *  (SRR of Extended Frame, RTR
 *  bit of CAN 2.0 Base Frame)
 *  0b011 - ALC_IDE - Arbitration was lost during IDE bit.
 *  0b100 - ALC_EXTENSION - Arbitration was lost during Identifier extension.
 *  0b101 - ALC_RTR - Arbitration was lost during RTR bit after Identifier extension!
 */
#define TWAIFD_ALC_ID_FIELD    0x00000007U
#define TWAIFD_ALC_ID_FIELD_M  (TWAIFD_ALC_ID_FIELD_V << TWAIFD_ALC_ID_FIELD_S)
#define TWAIFD_ALC_ID_FIELD_V  0x00000007U
#define TWAIFD_ALC_ID_FIELD_S  21
/** TWAIFD_TS_BITS : RO; bitpos: [29:24]; default: 0;
 *  Number of active bits of CTU CAN FD time-base minus 1 (0x3F = 64 bit time-base).
 */
#define TWAIFD_TS_BITS    0x0000003FU
#define TWAIFD_TS_BITS_M  (TWAIFD_TS_BITS_V << TWAIFD_TS_BITS_S)
#define TWAIFD_TS_BITS_V  0x0000003FU
#define TWAIFD_TS_BITS_S  24

/** TWAIFD_TRV_DELAY_SSP_CFG_REG register
 *  TWAI FD transmit delay & secondary sample point configuration register
 */
#define TWAIFD_TRV_DELAY_SSP_CFG_REG(i) (REG_TWAI_BASE(i) + 0x80)
/** TWAIFD_TRV_DELAY_VALUE : RO; bitpos: [6:0]; default: 0;
 *  Measured Transmitter delay in multiple of minimal Time quanta.
 */
#define TWAIFD_TRV_DELAY_VALUE    0x0000007FU
#define TWAIFD_TRV_DELAY_VALUE_M  (TWAIFD_TRV_DELAY_VALUE_V << TWAIFD_TRV_DELAY_VALUE_S)
#define TWAIFD_TRV_DELAY_VALUE_V  0x0000007FU
#define TWAIFD_TRV_DELAY_VALUE_S  0
/** TWAIFD_SSP_OFFSET : R/W; bitpos: [23:16]; default: 10;
 *  Secondary sampling point offset. Value is given as multiple of minimal Time quanta.
 */
#define TWAIFD_SSP_OFFSET    0x000000FFU
#define TWAIFD_SSP_OFFSET_M  (TWAIFD_SSP_OFFSET_V << TWAIFD_SSP_OFFSET_S)
#define TWAIFD_SSP_OFFSET_V  0x000000FFU
#define TWAIFD_SSP_OFFSET_S  16
/** TWAIFD_SSP_SRC : R/W; bitpos: [25:24]; default: 0;
 *  Source of Secondary sampling point.
 *  0b00 - SSP_SRC_MEAS_N_OFFSET - SSP position = TRV_DELAY (Measured Transmitter
 *  delay) + SSP_OFFSET.
 *  0b01 - SSP_SRC_NO_SSP - SSP is not used. Transmitter uses regular Sampling Point
 *  during data bit rate.
 *  0b10 - SSP_SRC_OFFSET - SSP position = SSP_OFFSET. Measured Transmitter delay value
 *  is ignored.
 */
#define TWAIFD_SSP_SRC    0x00000003U
#define TWAIFD_SSP_SRC_M  (TWAIFD_SSP_SRC_V << TWAIFD_SSP_SRC_S)
#define TWAIFD_SSP_SRC_V  0x00000003U
#define TWAIFD_SSP_SRC_S  24

/** TWAIFD_RX_FR_CTR_REG register
 *  TWAI FD received frame counter register
 */
#define TWAIFD_RX_FR_CTR_REG(i) (REG_TWAI_BASE(i) + 0x84)
/** TWAIFD_RX_FR_CTR_VAL : RO; bitpos: [31:0]; default: 0;
 *  Number of received frames by CTU CAN FD.
 */
#define TWAIFD_RX_FR_CTR_VAL    0xFFFFFFFFU
#define TWAIFD_RX_FR_CTR_VAL_M  (TWAIFD_RX_FR_CTR_VAL_V << TWAIFD_RX_FR_CTR_VAL_S)
#define TWAIFD_RX_FR_CTR_VAL_V  0xFFFFFFFFU
#define TWAIFD_RX_FR_CTR_VAL_S  0

/** TWAIFD_TX_FR_CTR_REG register
 *  TWAI FD transmitted frame counter register
 */
#define TWAIFD_TX_FR_CTR_REG(i) (REG_TWAI_BASE(i) + 0x88)
/** TWAIFD_TX_CTR_VAL : RO; bitpos: [31:0]; default: 0;
 *  Number of transmitted frames by CTU CAN FD.
 */
#define TWAIFD_TX_CTR_VAL    0xFFFFFFFFU
#define TWAIFD_TX_CTR_VAL_M  (TWAIFD_TX_CTR_VAL_V << TWAIFD_TX_CTR_VAL_S)
#define TWAIFD_TX_CTR_VAL_V  0xFFFFFFFFU
#define TWAIFD_TX_CTR_VAL_S  0

/** TWAIFD_DEBUG_REG register
 *  TWAI FD debug register
 */
#define TWAIFD_DEBUG_REG(i) (REG_TWAI_BASE(i) + 0x8c)
/** TWAIFD_STUFF_COUNT : RO; bitpos: [2:0]; default: 0;
 *  Actual stuff count modulo 8 as defined in ISO FD protocol. Stuff count is erased
 *  in the beginning
 *  of CAN frame and increased by one with each stuff bit until Stuff count field in
 *  ISO FD frame. Then it stays fixed
 *  until the beginning of next frame. In non-ISO FD or normal CAN stuff bits are
 *  counted until the end of a frame.
 *  Note that this field is NOT gray encoded as defined in ISO FD standard. Stuff count
 *  is calculated only as long as
 *  controller is transceiving on the bus. During the reception this value remains
 *  fixed!
 */
#define TWAIFD_STUFF_COUNT    0x00000007U
#define TWAIFD_STUFF_COUNT_M  (TWAIFD_STUFF_COUNT_V << TWAIFD_STUFF_COUNT_S)
#define TWAIFD_STUFF_COUNT_V  0x00000007U
#define TWAIFD_STUFF_COUNT_S  0
/** TWAIFD_DESTUFF_COUNT : RO; bitpos: [5:3]; default: 0;
 *  Actual de-stuff count modulo 8 as defined in ISO FD protocol. De-Stuff count is
 *  erased in the
 *  beginning of the frame and increased by one with each de-stuffed bit until Stuff
 *  count field in ISO FD Frame. Then
 *  it stays fixed until beginning of next frame. In non-ISO FD or normal CAN de-stuff
 *  bits are counted until the end
 *  of the frame. Note that this field is NOT grey encoded as defined in ISO FD
 *  standard. De-stuff count is calculated
 *  in both. Transceiver as well as receiver.
 */
#define TWAIFD_DESTUFF_COUNT    0x00000007U
#define TWAIFD_DESTUFF_COUNT_M  (TWAIFD_DESTUFF_COUNT_V << TWAIFD_DESTUFF_COUNT_S)
#define TWAIFD_DESTUFF_COUNT_V  0x00000007U
#define TWAIFD_DESTUFF_COUNT_S  3
/** TWAIFD_PC_ARB : RO; bitpos: [6]; default: 0;
 *  Protocol control state machine is in Arbitration field.
 */
#define TWAIFD_PC_ARB    (BIT(6))
#define TWAIFD_PC_ARB_M  (TWAIFD_PC_ARB_V << TWAIFD_PC_ARB_S)
#define TWAIFD_PC_ARB_V  0x00000001U
#define TWAIFD_PC_ARB_S  6
/** TWAIFD_PC_CON : RO; bitpos: [7]; default: 0;
 *  Protocol control state machine is in Control field.
 */
#define TWAIFD_PC_CON    (BIT(7))
#define TWAIFD_PC_CON_M  (TWAIFD_PC_CON_V << TWAIFD_PC_CON_S)
#define TWAIFD_PC_CON_V  0x00000001U
#define TWAIFD_PC_CON_S  7
/** TWAIFD_PC_DAT : RO; bitpos: [8]; default: 0;
 *  Protocol control state machine is in Data field.
 */
#define TWAIFD_PC_DAT    (BIT(8))
#define TWAIFD_PC_DAT_M  (TWAIFD_PC_DAT_V << TWAIFD_PC_DAT_S)
#define TWAIFD_PC_DAT_V  0x00000001U
#define TWAIFD_PC_DAT_S  8
/** TWAIFD_PC_STC : RO; bitpos: [9]; default: 0;
 *  Protocol control state machine is in Stuff Count field.
 */
#define TWAIFD_PC_STC    (BIT(9))
#define TWAIFD_PC_STC_M  (TWAIFD_PC_STC_V << TWAIFD_PC_STC_S)
#define TWAIFD_PC_STC_V  0x00000001U
#define TWAIFD_PC_STC_S  9
/** TWAIFD_PC_CRC : RO; bitpos: [10]; default: 0;
 *  Protocol control state machine is in CRC field.
 */
#define TWAIFD_PC_CRC    (BIT(10))
#define TWAIFD_PC_CRC_M  (TWAIFD_PC_CRC_V << TWAIFD_PC_CRC_S)
#define TWAIFD_PC_CRC_V  0x00000001U
#define TWAIFD_PC_CRC_S  10
/** TWAIFD_PC_CRCD : RO; bitpos: [11]; default: 0;
 *  Protocol control state machine is in CRC Delimiter field.
 */
#define TWAIFD_PC_CRCD    (BIT(11))
#define TWAIFD_PC_CRCD_M  (TWAIFD_PC_CRCD_V << TWAIFD_PC_CRCD_S)
#define TWAIFD_PC_CRCD_V  0x00000001U
#define TWAIFD_PC_CRCD_S  11
/** TWAIFD_PC_ACK : RO; bitpos: [12]; default: 0;
 *  Protocol control state machine is in ACK field.
 */
#define TWAIFD_PC_ACK    (BIT(12))
#define TWAIFD_PC_ACK_M  (TWAIFD_PC_ACK_V << TWAIFD_PC_ACK_S)
#define TWAIFD_PC_ACK_V  0x00000001U
#define TWAIFD_PC_ACK_S  12
/** TWAIFD_PC_ACKD : RO; bitpos: [13]; default: 0;
 *  Protocol control state machine is in ACK Delimiter field.
 */
#define TWAIFD_PC_ACKD    (BIT(13))
#define TWAIFD_PC_ACKD_M  (TWAIFD_PC_ACKD_V << TWAIFD_PC_ACKD_S)
#define TWAIFD_PC_ACKD_V  0x00000001U
#define TWAIFD_PC_ACKD_S  13
/** TWAIFD_PC_EOF : RO; bitpos: [14]; default: 0;
 *  Protocol control state machine is in End of file field.
 */
#define TWAIFD_PC_EOF    (BIT(14))
#define TWAIFD_PC_EOF_M  (TWAIFD_PC_EOF_V << TWAIFD_PC_EOF_S)
#define TWAIFD_PC_EOF_V  0x00000001U
#define TWAIFD_PC_EOF_S  14
/** TWAIFD_PC_INT : RO; bitpos: [15]; default: 0;
 *  Protocol control state machine is in Intermission field.
 */
#define TWAIFD_PC_INT    (BIT(15))
#define TWAIFD_PC_INT_M  (TWAIFD_PC_INT_V << TWAIFD_PC_INT_S)
#define TWAIFD_PC_INT_V  0x00000001U
#define TWAIFD_PC_INT_S  15
/** TWAIFD_PC_SUSP : RO; bitpos: [16]; default: 0;
 *  Protocol control state machine is in Suspend transmission field.
 */
#define TWAIFD_PC_SUSP    (BIT(16))
#define TWAIFD_PC_SUSP_M  (TWAIFD_PC_SUSP_V << TWAIFD_PC_SUSP_S)
#define TWAIFD_PC_SUSP_V  0x00000001U
#define TWAIFD_PC_SUSP_S  16
/** TWAIFD_PC_OVR : RO; bitpos: [17]; default: 0;
 *  Protocol control state machine is in Overload field.
 */
#define TWAIFD_PC_OVR    (BIT(17))
#define TWAIFD_PC_OVR_M  (TWAIFD_PC_OVR_V << TWAIFD_PC_OVR_S)
#define TWAIFD_PC_OVR_V  0x00000001U
#define TWAIFD_PC_OVR_S  17
/** TWAIFD_PC_SOF : RO; bitpos: [18]; default: 0;
 *  Protocol control state machine is in Start of frame field.
 */
#define TWAIFD_PC_SOF    (BIT(18))
#define TWAIFD_PC_SOF_M  (TWAIFD_PC_SOF_V << TWAIFD_PC_SOF_S)
#define TWAIFD_PC_SOF_V  0x00000001U
#define TWAIFD_PC_SOF_S  18

/** TWAIFD_YOLO_REG register
 *  TWAI FD transmitted frame counter register
 */
#define TWAIFD_YOLO_REG(i) (REG_TWAI_BASE(i) + 0x90)
/** TWAIFD_YOLO_VAL : RO; bitpos: [31:0]; default: 3735928559;
 *  What else could be in this register??
 */
#define TWAIFD_YOLO_VAL    0xFFFFFFFFU
#define TWAIFD_YOLO_VAL_M  (TWAIFD_YOLO_VAL_V << TWAIFD_YOLO_VAL_S)
#define TWAIFD_YOLO_VAL_V  0xFFFFFFFFU
#define TWAIFD_YOLO_VAL_S  0

/** TWAIFD_TIMESTAMP_LOW_REG register
 *  TWAI FD transmitted frame counter register
 */
#define TWAIFD_TIMESTAMP_LOW_REG(i) (REG_TWAI_BASE(i) + 0x94)
/** TWAIFD_TIMESTAMP_LOW : RO; bitpos: [31:0]; default: 0;
 *  Bits 31:0 of time base.
 */
#define TWAIFD_TIMESTAMP_LOW    0xFFFFFFFFU
#define TWAIFD_TIMESTAMP_LOW_M  (TWAIFD_TIMESTAMP_LOW_V << TWAIFD_TIMESTAMP_LOW_S)
#define TWAIFD_TIMESTAMP_LOW_V  0xFFFFFFFFU
#define TWAIFD_TIMESTAMP_LOW_S  0

/** TWAIFD_TIMESTAMP_HIGH_REG register
 *  TWAI FD transmitted frame counter register
 */
#define TWAIFD_TIMESTAMP_HIGH_REG(i) (REG_TWAI_BASE(i) + 0x98)
/** TWAIFD_TIMESTAMP_HIGH : RO; bitpos: [31:0]; default: 0;
 *  Bits 63:32 of time base.
 */
#define TWAIFD_TIMESTAMP_HIGH    0xFFFFFFFFU
#define TWAIFD_TIMESTAMP_HIGH_M  (TWAIFD_TIMESTAMP_HIGH_V << TWAIFD_TIMESTAMP_HIGH_S)
#define TWAIFD_TIMESTAMP_HIGH_V  0xFFFFFFFFU
#define TWAIFD_TIMESTAMP_HIGH_S  0

/** TWAIFD_TIMER_CLK_EN_REG register
 *  TWAIFD timer clock force enable register.
 */
#define TWAIFD_TIMER_CLK_EN_REG(i) (REG_TWAI_BASE(i) + 0xfd4)
/** TWAIFD_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set this bit to force enable TWAIFD register configuration clock signal.
 */
#define TWAIFD_CLK_EN    (BIT(0))
#define TWAIFD_CLK_EN_M  (TWAIFD_CLK_EN_V << TWAIFD_CLK_EN_S)
#define TWAIFD_CLK_EN_V  0x00000001U
#define TWAIFD_CLK_EN_S  0
/** TWAIFD_FORCE_RXBUF_MEM_CLK_ON : R/W; bitpos: [1]; default: 0;
 *  Set this bit to force enable TWAIFD RX buffer ram clock signal.
 */
#define TWAIFD_FORCE_RXBUF_MEM_CLK_ON    (BIT(1))
#define TWAIFD_FORCE_RXBUF_MEM_CLK_ON_M  (TWAIFD_FORCE_RXBUF_MEM_CLK_ON_V << TWAIFD_FORCE_RXBUF_MEM_CLK_ON_S)
#define TWAIFD_FORCE_RXBUF_MEM_CLK_ON_V  0x00000001U
#define TWAIFD_FORCE_RXBUF_MEM_CLK_ON_S  1

/** TWAIFD_TIMER_INT_RAW_REG register
 *  TWAIFD raw interrupt register.
 */
#define TWAIFD_TIMER_INT_RAW_REG(i) (REG_TWAI_BASE(i) + 0xfd8)
/** TWAIFD_TIMER_OVERFLOW_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  The raw bit signal for read_done interrupt.
 */
#define TWAIFD_TIMER_OVERFLOW_INT_RAW    (BIT(0))
#define TWAIFD_TIMER_OVERFLOW_INT_RAW_M  (TWAIFD_TIMER_OVERFLOW_INT_RAW_V << TWAIFD_TIMER_OVERFLOW_INT_RAW_S)
#define TWAIFD_TIMER_OVERFLOW_INT_RAW_V  0x00000001U
#define TWAIFD_TIMER_OVERFLOW_INT_RAW_S  0

/** TWAIFD_TIMER_INT_ST_REG register
 *  TWAIFD interrupt status register.
 */
#define TWAIFD_TIMER_INT_ST_REG(i) (REG_TWAI_BASE(i) + 0xfdc)
/** TWAIFD_TIMER_OVERFLOW_INT_ST : RO; bitpos: [0]; default: 0;
 *  The status signal for read_done interrupt.
 */
#define TWAIFD_TIMER_OVERFLOW_INT_ST    (BIT(0))
#define TWAIFD_TIMER_OVERFLOW_INT_ST_M  (TWAIFD_TIMER_OVERFLOW_INT_ST_V << TWAIFD_TIMER_OVERFLOW_INT_ST_S)
#define TWAIFD_TIMER_OVERFLOW_INT_ST_V  0x00000001U
#define TWAIFD_TIMER_OVERFLOW_INT_ST_S  0

/** TWAIFD_TIMER_INT_ENA_REG register
 *  TWAIFD interrupt enable register.
 */
#define TWAIFD_TIMER_INT_ENA_REG(i) (REG_TWAI_BASE(i) + 0xfe0)
/** TWAIFD_TIMER_OVERFLOW_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The enable signal for read_done interrupt.
 */
#define TWAIFD_TIMER_OVERFLOW_INT_ENA    (BIT(0))
#define TWAIFD_TIMER_OVERFLOW_INT_ENA_M  (TWAIFD_TIMER_OVERFLOW_INT_ENA_V << TWAIFD_TIMER_OVERFLOW_INT_ENA_S)
#define TWAIFD_TIMER_OVERFLOW_INT_ENA_V  0x00000001U
#define TWAIFD_TIMER_OVERFLOW_INT_ENA_S  0

/** TWAIFD_TIMER_INT_CLR_REG register
 *  TWAIFD interrupt clear register.
 */
#define TWAIFD_TIMER_INT_CLR_REG(i) (REG_TWAI_BASE(i) + 0xfe4)
/** TWAIFD_TIMER_OVERFLOW_INT_CLR : WT; bitpos: [0]; default: 0;
 *  The clear signal for read_done interrupt.
 */
#define TWAIFD_TIMER_OVERFLOW_INT_CLR    (BIT(0))
#define TWAIFD_TIMER_OVERFLOW_INT_CLR_M  (TWAIFD_TIMER_OVERFLOW_INT_CLR_V << TWAIFD_TIMER_OVERFLOW_INT_CLR_S)
#define TWAIFD_TIMER_OVERFLOW_INT_CLR_V  0x00000001U
#define TWAIFD_TIMER_OVERFLOW_INT_CLR_S  0

/** TWAIFD_TIMER_CFG_REG register
 *  TWAI FD timer configure register.
 */
#define TWAIFD_TIMER_CFG_REG(i) (REG_TWAI_BASE(i) + 0xfe8)
/** TWAIFD_TIMER_CE : R/W; bitpos: [0]; default: 0;
 *  TWAI FD timer enable register.
 *  1b0: Not enable
 *  1b1: Enable timer
 */
#define TWAIFD_TIMER_CE    (BIT(0))
#define TWAIFD_TIMER_CE_M  (TWAIFD_TIMER_CE_V << TWAIFD_TIMER_CE_S)
#define TWAIFD_TIMER_CE_V  0x00000001U
#define TWAIFD_TIMER_CE_S  0
/** TWAIFD_TIMER_CLR : WT; bitpos: [1]; default: 0;
 *  TWAI FD timer clear register.
 *  1b0: Not enable
 *  1b1: Enable to clear value
 */
#define TWAIFD_TIMER_CLR    (BIT(1))
#define TWAIFD_TIMER_CLR_M  (TWAIFD_TIMER_CLR_V << TWAIFD_TIMER_CLR_S)
#define TWAIFD_TIMER_CLR_V  0x00000001U
#define TWAIFD_TIMER_CLR_S  1
/** TWAIFD_TIMER_SET : WT; bitpos: [2]; default: 0;
 *  TWAI FD timer set register.
 *  1b0: Not enable
 *  1b1: Enable to set value to ld_val.
 */
#define TWAIFD_TIMER_SET    (BIT(2))
#define TWAIFD_TIMER_SET_M  (TWAIFD_TIMER_SET_V << TWAIFD_TIMER_SET_S)
#define TWAIFD_TIMER_SET_V  0x00000001U
#define TWAIFD_TIMER_SET_S  2
/** TWAIFD_TIMER_UP_DN : R/W; bitpos: [8]; default: 1;
 *  TWAI FD timer up/down count register.
 *  1b0: Count-down
 *  1b1: Count-up
 */
#define TWAIFD_TIMER_UP_DN    (BIT(8))
#define TWAIFD_TIMER_UP_DN_M  (TWAIFD_TIMER_UP_DN_V << TWAIFD_TIMER_UP_DN_S)
#define TWAIFD_TIMER_UP_DN_V  0x00000001U
#define TWAIFD_TIMER_UP_DN_S  8
/** TWAIFD_TIMER_STEP : R/W; bitpos: [31:16]; default: 0;
 *  TWAI FD timer count step register, step=reg_timer_step+1
 */
#define TWAIFD_TIMER_STEP    0x0000FFFFU
#define TWAIFD_TIMER_STEP_M  (TWAIFD_TIMER_STEP_V << TWAIFD_TIMER_STEP_S)
#define TWAIFD_TIMER_STEP_V  0x0000FFFFU
#define TWAIFD_TIMER_STEP_S  16

/** TWAIFD_TIMER_LD_VAL_L_REG register
 *  TWAI FD timer pre-load value low register.
 */
#define TWAIFD_TIMER_LD_VAL_L_REG(i) (REG_TWAI_BASE(i) + 0xfec)
/** TWAIFD_TIMER_LD_VAL_L : R/W; bitpos: [31:0]; default: 0;
 *  TWAI FD timer count pre-load value register, low part.
 */
#define TWAIFD_TIMER_LD_VAL_L    0xFFFFFFFFU
#define TWAIFD_TIMER_LD_VAL_L_M  (TWAIFD_TIMER_LD_VAL_L_V << TWAIFD_TIMER_LD_VAL_L_S)
#define TWAIFD_TIMER_LD_VAL_L_V  0xFFFFFFFFU
#define TWAIFD_TIMER_LD_VAL_L_S  0

/** TWAIFD_TIMER_LD_VAL_H_REG register
 *  TWAI FD timer pre-load value high register.
 */
#define TWAIFD_TIMER_LD_VAL_H_REG(i) (REG_TWAI_BASE(i) + 0xff0)
/** TWAIFD_TIMER_LD_VAL_H : R/W; bitpos: [31:0]; default: 0;
 *  TWAI FD timer pre-load value register, high part.
 *  If timestamp valid bit-width less than 33, this field is ignored.
 */
#define TWAIFD_TIMER_LD_VAL_H    0xFFFFFFFFU
#define TWAIFD_TIMER_LD_VAL_H_M  (TWAIFD_TIMER_LD_VAL_H_V << TWAIFD_TIMER_LD_VAL_H_S)
#define TWAIFD_TIMER_LD_VAL_H_V  0xFFFFFFFFU
#define TWAIFD_TIMER_LD_VAL_H_S  0

/** TWAIFD_TIMER_CT_VAL_L_REG register
 *  TWAI FD timer count-to value low register.
 */
#define TWAIFD_TIMER_CT_VAL_L_REG(i) (REG_TWAI_BASE(i) + 0xff4)
/** TWAIFD_TIMER_CT_VAL_L : R/W; bitpos: [31:0]; default: 4294967295;
 *  TWAI FD timer count-to value register, low part.
 */
#define TWAIFD_TIMER_CT_VAL_L    0xFFFFFFFFU
#define TWAIFD_TIMER_CT_VAL_L_M  (TWAIFD_TIMER_CT_VAL_L_V << TWAIFD_TIMER_CT_VAL_L_S)
#define TWAIFD_TIMER_CT_VAL_L_V  0xFFFFFFFFU
#define TWAIFD_TIMER_CT_VAL_L_S  0

/** TWAIFD_TIMER_CT_VAL_H_REG register
 *  TWAI FD timer count-to value high register.
 */
#define TWAIFD_TIMER_CT_VAL_H_REG(i) (REG_TWAI_BASE(i) + 0xff8)
/** TWAIFD_TIMER_CT_VAL_H : R/W; bitpos: [31:0]; default: 4294967295;
 *  TWAI FD timer count-to value register, high part.
 *  If timestamp valid bit-width less than 33, this field is ignored.
 */
#define TWAIFD_TIMER_CT_VAL_H    0xFFFFFFFFU
#define TWAIFD_TIMER_CT_VAL_H_M  (TWAIFD_TIMER_CT_VAL_H_V << TWAIFD_TIMER_CT_VAL_H_S)
#define TWAIFD_TIMER_CT_VAL_H_V  0xFFFFFFFFU
#define TWAIFD_TIMER_CT_VAL_H_S  0

/** TWAIFD_DATE_VER_REG register
 *  TWAI FD date version
 */
#define TWAIFD_DATE_VER_REG(i) (REG_TWAI_BASE(i) + 0xffc)
/** TWAIFD_DATE_VER : R/W; bitpos: [31:0]; default: 36774224;
 *  TWAI FD version
 */
#define TWAIFD_DATE_VER    0xFFFFFFFFU
#define TWAIFD_DATE_VER_M  (TWAIFD_DATE_VER_V << TWAIFD_DATE_VER_S)
#define TWAIFD_DATE_VER_V  0xFFFFFFFFU
#define TWAIFD_DATE_VER_S  0

#ifdef __cplusplus
}
#endif
