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

/** TRACE_MEM_START_ADDR_REG register
 *  Memory start address
 */
#define TRACE_MEM_START_ADDR_REG (DR_REG_TRACE_BASE + 0x0)
/** TRACE_MEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of the trace memory
 */
#define TRACE_MEM_START_ADDR    0xFFFFFFFFU
#define TRACE_MEM_START_ADDR_M  (TRACE_MEM_START_ADDR_V << TRACE_MEM_START_ADDR_S)
#define TRACE_MEM_START_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_START_ADDR_S  0

/** TRACE_MEM_END_ADDR_REG register
 *  Memory end address
 */
#define TRACE_MEM_END_ADDR_REG (DR_REG_TRACE_BASE + 0x4)
/** TRACE_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of the trace memory.
 */
#define TRACE_MEM_END_ADDR    0xFFFFFFFFU
#define TRACE_MEM_END_ADDR_M  (TRACE_MEM_END_ADDR_V << TRACE_MEM_END_ADDR_S)
#define TRACE_MEM_END_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_END_ADDR_S  0

/** TRACE_MEM_CURRENT_ADDR_REG register
 *  Memory current addr
 */
#define TRACE_MEM_CURRENT_ADDR_REG (DR_REG_TRACE_BASE + 0x8)
/** TRACE_MEM_CURRENT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the current memory address for writing.
 */
#define TRACE_MEM_CURRENT_ADDR    0xFFFFFFFFU
#define TRACE_MEM_CURRENT_ADDR_M  (TRACE_MEM_CURRENT_ADDR_V << TRACE_MEM_CURRENT_ADDR_S)
#define TRACE_MEM_CURRENT_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_CURRENT_ADDR_S  0

/** TRACE_MEM_ADDR_UPDATE_REG register
 *  Memory address update
 */
#define TRACE_MEM_ADDR_UPDATE_REG (DR_REG_TRACE_BASE + 0xc)
/** TRACE_MEM_CURRENT_ADDR_UPDATE : WT; bitpos: [0]; default: 0;
 *  Configures whether to update the value of
 *  \hyperref[fielddesc:TRACEMEMCURRENTADDR]{TRACE_MEM_CURRENT_ADDR} to
 *  \hyperref[fielddesc:TRACEMEMSTARTADDR]{TRACE_MEM_START_ADDR}.\\
 *  0: Not update\\
 *  1: Update\\
 */
#define TRACE_MEM_CURRENT_ADDR_UPDATE    (BIT(0))
#define TRACE_MEM_CURRENT_ADDR_UPDATE_M  (TRACE_MEM_CURRENT_ADDR_UPDATE_V << TRACE_MEM_CURRENT_ADDR_UPDATE_S)
#define TRACE_MEM_CURRENT_ADDR_UPDATE_V  0x00000001U
#define TRACE_MEM_CURRENT_ADDR_UPDATE_S  0

/** TRACE_FIFO_STATUS_REG register
 *  FIFO status register
 */
#define TRACE_FIFO_STATUS_REG (DR_REG_TRACE_BASE + 0x10)
/** TRACE_FIFO_EMPTY : RO; bitpos: [0]; default: 1;
 *  Represent whether the FIFO is empty. \\1: Empty \\0: Not empty
 */
#define TRACE_FIFO_EMPTY    (BIT(0))
#define TRACE_FIFO_EMPTY_M  (TRACE_FIFO_EMPTY_V << TRACE_FIFO_EMPTY_S)
#define TRACE_FIFO_EMPTY_V  0x00000001U
#define TRACE_FIFO_EMPTY_S  0
/** TRACE_WORK_STATUS : RO; bitpos: [2:1]; default: 0;
 *  Represent the state of the encoder: \\0: Idle state \\1: Working state\\ 2: Wait
 *  state because hart is halted or in reset \\3: Lost state\\
 */
#define TRACE_WORK_STATUS    0x00000003U
#define TRACE_WORK_STATUS_M  (TRACE_WORK_STATUS_V << TRACE_WORK_STATUS_S)
#define TRACE_WORK_STATUS_V  0x00000003U
#define TRACE_WORK_STATUS_S  1

/** TRACE_INTR_ENA_REG register
 *  Interrupt enable register
 */
#define TRACE_INTR_ENA_REG (DR_REG_TRACE_BASE + 0x14)
/** TRACE_FIFO_OVERFLOW_INTR_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable TRACE_FIFO_OVERFLOW_INTR
 */
#define TRACE_FIFO_OVERFLOW_INTR_ENA    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_ENA_M  (TRACE_FIFO_OVERFLOW_INTR_ENA_V << TRACE_FIFO_OVERFLOW_INTR_ENA_S)
#define TRACE_FIFO_OVERFLOW_INTR_ENA_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_ENA_S  0
/** TRACE_MEM_FULL_INTR_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable TRACE_MEM_FULL_INTR
 */
#define TRACE_MEM_FULL_INTR_ENA    (BIT(1))
#define TRACE_MEM_FULL_INTR_ENA_M  (TRACE_MEM_FULL_INTR_ENA_V << TRACE_MEM_FULL_INTR_ENA_S)
#define TRACE_MEM_FULL_INTR_ENA_V  0x00000001U
#define TRACE_MEM_FULL_INTR_ENA_S  1

/** TRACE_INTR_RAW_REG register
 *  Interrupt raw status register
 */
#define TRACE_INTR_RAW_REG (DR_REG_TRACE_BASE + 0x18)
/** TRACE_FIFO_OVERFLOW_INTR_RAW : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status of TRACE_FIFO_OVERFLOW_INTR.
 */
#define TRACE_FIFO_OVERFLOW_INTR_RAW    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_RAW_M  (TRACE_FIFO_OVERFLOW_INTR_RAW_V << TRACE_FIFO_OVERFLOW_INTR_RAW_S)
#define TRACE_FIFO_OVERFLOW_INTR_RAW_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_RAW_S  0
/** TRACE_MEM_FULL_INTR_RAW : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status of TRACE_MEM_FULL_INTR
 */
#define TRACE_MEM_FULL_INTR_RAW    (BIT(1))
#define TRACE_MEM_FULL_INTR_RAW_M  (TRACE_MEM_FULL_INTR_RAW_V << TRACE_MEM_FULL_INTR_RAW_S)
#define TRACE_MEM_FULL_INTR_RAW_V  0x00000001U
#define TRACE_MEM_FULL_INTR_RAW_S  1

/** TRACE_INTR_CLR_REG register
 *  Interrupt clear register
 */
#define TRACE_INTR_CLR_REG (DR_REG_TRACE_BASE + 0x1c)
/** TRACE_FIFO_OVERFLOW_INTR_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear TRACE_FIFO_OVERFLOW_INTR
 */
#define TRACE_FIFO_OVERFLOW_INTR_CLR    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_CLR_M  (TRACE_FIFO_OVERFLOW_INTR_CLR_V << TRACE_FIFO_OVERFLOW_INTR_CLR_S)
#define TRACE_FIFO_OVERFLOW_INTR_CLR_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_CLR_S  0
/** TRACE_MEM_FULL_INTR_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear TRACE_MEM_FULL_INTR
 */
#define TRACE_MEM_FULL_INTR_CLR    (BIT(1))
#define TRACE_MEM_FULL_INTR_CLR_M  (TRACE_MEM_FULL_INTR_CLR_V << TRACE_MEM_FULL_INTR_CLR_S)
#define TRACE_MEM_FULL_INTR_CLR_V  0x00000001U
#define TRACE_MEM_FULL_INTR_CLR_S  1

/** TRACE_TRIGGER_REG register
 *  Trace enable register
 */
#define TRACE_TRIGGER_REG (DR_REG_TRACE_BASE + 0x20)
/** TRACE_TRIGGER_ON : WT; bitpos: [0]; default: 0;
 *  Configure whether to enable the encoder.\\0: Invalid \\1: Enable\\
 */
#define TRACE_TRIGGER_ON    (BIT(0))
#define TRACE_TRIGGER_ON_M  (TRACE_TRIGGER_ON_V << TRACE_TRIGGER_ON_S)
#define TRACE_TRIGGER_ON_V  0x00000001U
#define TRACE_TRIGGER_ON_S  0
/** TRACE_TRIGGER_OFF : WT; bitpos: [1]; default: 0;
 *  Configure whether to disable the encoder.\\0: Invalid \\1: Disable\\
 */
#define TRACE_TRIGGER_OFF    (BIT(1))
#define TRACE_TRIGGER_OFF_M  (TRACE_TRIGGER_OFF_V << TRACE_TRIGGER_OFF_S)
#define TRACE_TRIGGER_OFF_V  0x00000001U
#define TRACE_TRIGGER_OFF_S  1
/** TRACE_MEM_LOOP : R/W; bitpos: [2]; default: 1;
 *  Configure the memory writing mode. \\0: Non-loop mode. \\1: Loop mode\\
 */
#define TRACE_MEM_LOOP    (BIT(2))
#define TRACE_MEM_LOOP_M  (TRACE_MEM_LOOP_V << TRACE_MEM_LOOP_S)
#define TRACE_MEM_LOOP_V  0x00000001U
#define TRACE_MEM_LOOP_S  2
/** TRACE_RESTART_ENA : R/W; bitpos: [3]; default: 1;
 *  Configure whether or not enable automatic restart function for the encoder.\\0:
 *  Disable\\1: Enable\\
 */
#define TRACE_RESTART_ENA    (BIT(3))
#define TRACE_RESTART_ENA_M  (TRACE_RESTART_ENA_V << TRACE_RESTART_ENA_S)
#define TRACE_RESTART_ENA_V  0x00000001U
#define TRACE_RESTART_ENA_S  3

/** TRACE_CONFIG_REG register
 *  trace configuration register
 */
#define TRACE_CONFIG_REG (DR_REG_TRACE_BASE + 0x24)
/** TRACE_DM_TRIGGER_ENA : R/W; bitpos: [0]; default: 0;
 *  Configure whether to enable the trigger signal.\\0: Disable\\1:enable\\
 */
#define TRACE_DM_TRIGGER_ENA    (BIT(0))
#define TRACE_DM_TRIGGER_ENA_M  (TRACE_DM_TRIGGER_ENA_V << TRACE_DM_TRIGGER_ENA_S)
#define TRACE_DM_TRIGGER_ENA_V  0x00000001U
#define TRACE_DM_TRIGGER_ENA_S  0
/** TRACE_RESET_ENA : R/W; bitpos: [1]; default: 0;
 *  Configure whether to reset, when enabled, if cpu have reset, the encoder will
 *  output a packet to report the address of the last instruction, and upon reset
 *  deassertion, the encoder start again.\\0: Disable\\0: Enable\\
 */
#define TRACE_RESET_ENA    (BIT(1))
#define TRACE_RESET_ENA_M  (TRACE_RESET_ENA_V << TRACE_RESET_ENA_S)
#define TRACE_RESET_ENA_V  0x00000001U
#define TRACE_RESET_ENA_S  1
/** TRACE_HALT_ENA : R/W; bitpos: [2]; default: 0;
 *  Configure whether to enable the halt signal. \\1: Disable\\1: Enable\\
 */
#define TRACE_HALT_ENA    (BIT(2))
#define TRACE_HALT_ENA_M  (TRACE_HALT_ENA_V << TRACE_HALT_ENA_S)
#define TRACE_HALT_ENA_V  0x00000001U
#define TRACE_HALT_ENA_S  2
/** TRACE_STALL_ENA : R/W; bitpos: [3]; default: 0;
 *  Configure whether to enable the stall signal. \\0: Disable.\\1: Enable\\
 */
#define TRACE_STALL_ENA    (BIT(3))
#define TRACE_STALL_ENA_M  (TRACE_STALL_ENA_V << TRACE_STALL_ENA_S)
#define TRACE_STALL_ENA_V  0x00000001U
#define TRACE_STALL_ENA_S  3
/** TRACE_FULL_ADDRESS : R/W; bitpos: [4]; default: 0;
 *  Configure the address mode.\\0: Delta address mode.\\1: Full address mode.\\
 */
#define TRACE_FULL_ADDRESS    (BIT(4))
#define TRACE_FULL_ADDRESS_M  (TRACE_FULL_ADDRESS_V << TRACE_FULL_ADDRESS_S)
#define TRACE_FULL_ADDRESS_V  0x00000001U
#define TRACE_FULL_ADDRESS_S  4
/** TRACE_IMPLICIT_EXCEPT : R/W; bitpos: [5]; default: 0;
 *  Configure whether or not enable implicit exception mode. When enabled,, do not sent
 *  exception address, only exception cause in exception packets.\\1: enabled\\0:
 *  disabled\\
 */
#define TRACE_IMPLICIT_EXCEPT    (BIT(5))
#define TRACE_IMPLICIT_EXCEPT_M  (TRACE_IMPLICIT_EXCEPT_V << TRACE_IMPLICIT_EXCEPT_S)
#define TRACE_IMPLICIT_EXCEPT_V  0x00000001U
#define TRACE_IMPLICIT_EXCEPT_S  5

/** TRACE_FILTER_CONTROL_REG register
 *  filter control register
 */
#define TRACE_FILTER_CONTROL_REG (DR_REG_TRACE_BASE + 0x28)
/** TRACE_FILTER_EN : R/W; bitpos: [0]; default: 0;
 *  Configure whether to enable filtering. \\0: Disable, always match.\\ 1: Enable
 */
#define TRACE_FILTER_EN    (BIT(0))
#define TRACE_FILTER_EN_M  (TRACE_FILTER_EN_V << TRACE_FILTER_EN_S)
#define TRACE_FILTER_EN_V  0x00000001U
#define TRACE_FILTER_EN_S  0
/** TRACE_MATCH_COMP : R/W; bitpos: [1]; default: 0;
 *  Configure whether to enable the comparator match mode. \\0: Disable \\1: Enable,
 *  the comparator must be high in order for the filter to match
 */
#define TRACE_MATCH_COMP    (BIT(1))
#define TRACE_MATCH_COMP_M  (TRACE_MATCH_COMP_V << TRACE_MATCH_COMP_S)
#define TRACE_MATCH_COMP_V  0x00000001U
#define TRACE_MATCH_COMP_S  1
/** TRACE_MATCH_PRIVILEGE : R/W; bitpos: [2]; default: 0;
 *  Configure whether to enable the privilege match mode. \\0: Disable \\1: Enable,
 *  match privilege levels specified by
 *  \hyperref[fielddesc:TRACEMATCHCHOICEPRIVILEGE]{TRACE_MATCH_CHOICE_PRIVILEGE}.
 */
#define TRACE_MATCH_PRIVILEGE    (BIT(2))
#define TRACE_MATCH_PRIVILEGE_M  (TRACE_MATCH_PRIVILEGE_V << TRACE_MATCH_PRIVILEGE_S)
#define TRACE_MATCH_PRIVILEGE_V  0x00000001U
#define TRACE_MATCH_PRIVILEGE_S  2
/** TRACE_MATCH_ECAUSE : R/W; bitpos: [3]; default: 0;
 *  Configure whether to enable ecause match mode. \\0: Disable \\1: Enable, start
 *  matching from exception cause codes specified by
 *  \hyperref[fielddesc:TRACEMATCHCHOICEECAUSE]{TRACE_MATCH_CHOICE_ECAUSE}, and stop
 *  matching upon return from the 1st matching exception.
 */
#define TRACE_MATCH_ECAUSE    (BIT(3))
#define TRACE_MATCH_ECAUSE_M  (TRACE_MATCH_ECAUSE_V << TRACE_MATCH_ECAUSE_S)
#define TRACE_MATCH_ECAUSE_V  0x00000001U
#define TRACE_MATCH_ECAUSE_S  3
/** TRACE_MATCH_INTERRUPT : R/W; bitpos: [4]; default: 0;
 *  Configure whether to enable the interrupt match mode. \\0: Disable \\1: Enable,
 *  start matching from a trap with the interrupt level codes specified by
 *  \hyperref[fielddesc:TRACEMATCHVALUEINTERRUPT]{TRACE_MATCH_VALUE_INTERRUPT}, and
 *  stop matching upon return from the 1st matching trap.
 */
#define TRACE_MATCH_INTERRUPT    (BIT(4))
#define TRACE_MATCH_INTERRUPT_M  (TRACE_MATCH_INTERRUPT_V << TRACE_MATCH_INTERRUPT_S)
#define TRACE_MATCH_INTERRUPT_V  0x00000001U
#define TRACE_MATCH_INTERRUPT_S  4

/** TRACE_FILTER_MATCH_CONTROL_REG register
 *  filter match control register
 */
#define TRACE_FILTER_MATCH_CONTROL_REG (DR_REG_TRACE_BASE + 0x2c)
/** TRACE_MATCH_CHOICE_PRIVILEGE : R/W; bitpos: [0]; default: 0;
 *  Configures the privilege level for matching. Valid only when
 *  \hyperref[fielddesc:TRACEMATCHPRIVILEGE]{TRACE_MATCH_PRIVILEGE} is set. \\0: User
 *  mode. \\1: Machine mode
 */
#define TRACE_MATCH_CHOICE_PRIVILEGE    (BIT(0))
#define TRACE_MATCH_CHOICE_PRIVILEGE_M  (TRACE_MATCH_CHOICE_PRIVILEGE_V << TRACE_MATCH_CHOICE_PRIVILEGE_S)
#define TRACE_MATCH_CHOICE_PRIVILEGE_V  0x00000001U
#define TRACE_MATCH_CHOICE_PRIVILEGE_S  0
/** TRACE_MATCH_VALUE_INTERRUPT : R/W; bitpos: [1]; default: 0;
 *  Configures the interrupt level for match. Valid only when when
 *  \hyperref[fielddesc:TRACEMATCHINTERRUPT]{TRACE_MATCH_INTERRUP} is set. \\0:
 *  itype=2. \\0: itype=2.
 */
#define TRACE_MATCH_VALUE_INTERRUPT    (BIT(1))
#define TRACE_MATCH_VALUE_INTERRUPT_M  (TRACE_MATCH_VALUE_INTERRUPT_V << TRACE_MATCH_VALUE_INTERRUPT_S)
#define TRACE_MATCH_VALUE_INTERRUPT_V  0x00000001U
#define TRACE_MATCH_VALUE_INTERRUPT_S  1
/** TRACE_MATCH_CHOICE_ECAUSE : R/W; bitpos: [7:2]; default: 0;
 *  Configures the ecause code for matching.
 */
#define TRACE_MATCH_CHOICE_ECAUSE    0x0000003FU
#define TRACE_MATCH_CHOICE_ECAUSE_M  (TRACE_MATCH_CHOICE_ECAUSE_V << TRACE_MATCH_CHOICE_ECAUSE_S)
#define TRACE_MATCH_CHOICE_ECAUSE_V  0x0000003FU
#define TRACE_MATCH_CHOICE_ECAUSE_S  2

/** TRACE_FILTER_COMPARATOR_CONTROL_REG register
 *  filter comparator match control register
 */
#define TRACE_FILTER_COMPARATOR_CONTROL_REG (DR_REG_TRACE_BASE + 0x30)
/** TRACE_P_INPUT : R/W; bitpos: [0]; default: 0;
 *  Configures the input of the primary comparator for matching: \\0: iaddr \\1: tval\\
 */
#define TRACE_P_INPUT    (BIT(0))
#define TRACE_P_INPUT_M  (TRACE_P_INPUT_V << TRACE_P_INPUT_S)
#define TRACE_P_INPUT_V  0x00000001U
#define TRACE_P_INPUT_S  0
/** TRACE_P_FUNCTION : R/W; bitpos: [4:2]; default: 0;
 *  Configures the function for the primary comparator. \\0: Equal, \\1: Not equal,
 *  \\2: Less than, \\3: Less than or equal, \\4: Greater than, \\5: Greater than or
 *  equal, \\Other: Always match
 */
#define TRACE_P_FUNCTION    0x00000007U
#define TRACE_P_FUNCTION_M  (TRACE_P_FUNCTION_V << TRACE_P_FUNCTION_S)
#define TRACE_P_FUNCTION_V  0x00000007U
#define TRACE_P_FUNCTION_S  2
/** TRACE_P_NOTIFY : R/W; bitpos: [5]; default: 0;
 *  Configure whether to explicitly report an instruction address matched against the
 *  primary comparator. \\0:Not report \\1:Report
 */
#define TRACE_P_NOTIFY    (BIT(5))
#define TRACE_P_NOTIFY_M  (TRACE_P_NOTIFY_V << TRACE_P_NOTIFY_S)
#define TRACE_P_NOTIFY_V  0x00000001U
#define TRACE_P_NOTIFY_S  5
/** TRACE_S_INPUT : R/W; bitpos: [8]; default: 0;
 *  Configures the input of the secondary comparator for matching: \\0: iaddr \\1:
 *  tval\\
 */
#define TRACE_S_INPUT    (BIT(8))
#define TRACE_S_INPUT_M  (TRACE_S_INPUT_V << TRACE_S_INPUT_S)
#define TRACE_S_INPUT_V  0x00000001U
#define TRACE_S_INPUT_S  8
/** TRACE_S_FUNCTION : R/W; bitpos: [12:10]; default: 0;
 *  Configures the function for the secondary comparator. \\0: Equal, \\1: Not equal,
 *  \\2: Less than, \\3: Less than or equal, \\4: Greater than, \\5: Greater than or
 *  equal, \\Other: Always match
 */
#define TRACE_S_FUNCTION    0x00000007U
#define TRACE_S_FUNCTION_M  (TRACE_S_FUNCTION_V << TRACE_S_FUNCTION_S)
#define TRACE_S_FUNCTION_V  0x00000007U
#define TRACE_S_FUNCTION_S  10
/** TRACE_S_NOTIFY : R/W; bitpos: [13]; default: 0;
 *  Generate a trace packet explicitly reporting the address that cause the secondary
 *  match
 */
#define TRACE_S_NOTIFY    (BIT(13))
#define TRACE_S_NOTIFY_M  (TRACE_S_NOTIFY_V << TRACE_S_NOTIFY_S)
#define TRACE_S_NOTIFY_V  0x00000001U
#define TRACE_S_NOTIFY_S  13
/** TRACE_MATCH_MODE : R/W; bitpos: [17:16]; default: 0;
 *  Configures the comparator match mode: \\0: Only the primary comparator matches \\1:
 *  Both primary and secondary comparator matches(P\&\&S) \\ 2:Neither primary or
 *  secondary comparator matches !(P\&\&S) \\3: Start filtering when the primary
 *  comparator matches and stop filtering when the secondary comparator matches\\
 */
#define TRACE_MATCH_MODE    0x00000003U
#define TRACE_MATCH_MODE_M  (TRACE_MATCH_MODE_V << TRACE_MATCH_MODE_S)
#define TRACE_MATCH_MODE_V  0x00000003U
#define TRACE_MATCH_MODE_S  16

/** TRACE_FILTER_P_COMPARATOR_MATCH_REG register
 *  primary comparator match value
 */
#define TRACE_FILTER_P_COMPARATOR_MATCH_REG (DR_REG_TRACE_BASE + 0x34)
/** TRACE_P_MATCH : R/W; bitpos: [31:0]; default: 0;
 *  Configures the match value for the primary comparator
 */
#define TRACE_P_MATCH    0xFFFFFFFFU
#define TRACE_P_MATCH_M  (TRACE_P_MATCH_V << TRACE_P_MATCH_S)
#define TRACE_P_MATCH_V  0xFFFFFFFFU
#define TRACE_P_MATCH_S  0

/** TRACE_FILTER_S_COMPARATOR_MATCH_REG register
 *  secondary comparator match value
 */
#define TRACE_FILTER_S_COMPARATOR_MATCH_REG (DR_REG_TRACE_BASE + 0x38)
/** TRACE_S_MATCH : R/W; bitpos: [31:0]; default: 0;
 *  Configures the match value for the secondary comparator
 */
#define TRACE_S_MATCH    0xFFFFFFFFU
#define TRACE_S_MATCH_M  (TRACE_S_MATCH_V << TRACE_S_MATCH_S)
#define TRACE_S_MATCH_V  0xFFFFFFFFU
#define TRACE_S_MATCH_S  0

/** TRACE_RESYNC_PROLONGED_REG register
 *  Resync configuration register
 */
#define TRACE_RESYNC_PROLONGED_REG (DR_REG_TRACE_BASE + 0x3c)
/** TRACE_RESYNC_PROLONGED : R/W; bitpos: [23:0]; default: 128;
 *  Configures the threshold for synchronization counter
 */
#define TRACE_RESYNC_PROLONGED    0x00FFFFFFU
#define TRACE_RESYNC_PROLONGED_M  (TRACE_RESYNC_PROLONGED_V << TRACE_RESYNC_PROLONGED_S)
#define TRACE_RESYNC_PROLONGED_V  0x00FFFFFFU
#define TRACE_RESYNC_PROLONGED_S  0
/** TRACE_RESYNC_MODE : R/W; bitpos: [25:24]; default: 0;
 *  Configures the synchronization mode: \\0: Disable the synchronization counter \\1:
 *  Invalid \\2: Synchronization counter counts by packet  \\3: Synchronization counter
 *  counts by cycle\\
 */
#define TRACE_RESYNC_MODE    0x00000003U
#define TRACE_RESYNC_MODE_M  (TRACE_RESYNC_MODE_V << TRACE_RESYNC_MODE_S)
#define TRACE_RESYNC_MODE_V  0x00000003U
#define TRACE_RESYNC_MODE_S  24

/** TRACE_AHB_CONFIG_REG register
 *  AHB config register
 */
#define TRACE_AHB_CONFIG_REG (DR_REG_TRACE_BASE + 0x40)
/** TRACE_HBURST : R/W; bitpos: [2:0]; default: 0;
 *  Configures the AHB burst mode. \\0: SINGLE \\1: INCR(length not defined) \\2:INCR4
 *  \\4:INCR8 \\Others:Invalid
 */
#define TRACE_HBURST    0x00000007U
#define TRACE_HBURST_M  (TRACE_HBURST_V << TRACE_HBURST_S)
#define TRACE_HBURST_V  0x00000007U
#define TRACE_HBURST_S  0
/** TRACE_MAX_INCR : R/W; bitpos: [5:3]; default: 0;
 *  Configures the maximum burst length for INCR mode
 */
#define TRACE_MAX_INCR    0x00000007U
#define TRACE_MAX_INCR_M  (TRACE_MAX_INCR_V << TRACE_MAX_INCR_S)
#define TRACE_MAX_INCR_V  0x00000007U
#define TRACE_MAX_INCR_S  3

/** TRACE_CLOCK_GATE_REG register
 *  Clock gate control register
 */
#define TRACE_CLOCK_GATE_REG (DR_REG_TRACE_BASE + 0x44)
/** TRACE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures register clock gating. \\0: Support clock only when the application
 *  writes registers to save power. \\1:Always force the clock on for registers \\ This
 *  bit doesn't affect register access.
 */
#define TRACE_CLK_EN    (BIT(0))
#define TRACE_CLK_EN_M  (TRACE_CLK_EN_V << TRACE_CLK_EN_S)
#define TRACE_CLK_EN_V  0x00000001U
#define TRACE_CLK_EN_S  0

/** TRACE_DATE_REG register
 *  Version control register
 */
#define TRACE_DATE_REG (DR_REG_TRACE_BASE + 0x3fc)
/** TRACE_DATE : R/W; bitpos: [27:0]; default: 35721984;
 *  Version control register.
 */
#define TRACE_DATE    0x0FFFFFFFU
#define TRACE_DATE_M  (TRACE_DATE_V << TRACE_DATE_S)
#define TRACE_DATE_V  0x0FFFFFFFU
#define TRACE_DATE_S  0

#ifdef __cplusplus
}
#endif
