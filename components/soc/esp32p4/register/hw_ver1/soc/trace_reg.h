/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
 *  mem start addr
 */
#define TRACE_MEM_START_ADDR_REG(i) (REG_TRACE_BASE(i) + 0x0)
/** TRACE_MEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The start address of trace memory
 */
#define TRACE_MEM_START_ADDR    0xFFFFFFFFU
#define TRACE_MEM_START_ADDR_M  (TRACE_MEM_START_ADDR_V << TRACE_MEM_START_ADDR_S)
#define TRACE_MEM_START_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_START_ADDR_S  0

/** TRACE_MEM_END_ADDR_REG register
 *  mem end addr
 */
#define TRACE_MEM_END_ADDR_REG(i) (REG_TRACE_BASE(i) + 0x4)
/** TRACE_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  The end address of trace memory
 */
#define TRACE_MEM_END_ADDR    0xFFFFFFFFU
#define TRACE_MEM_END_ADDR_M  (TRACE_MEM_END_ADDR_V << TRACE_MEM_END_ADDR_S)
#define TRACE_MEM_END_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_END_ADDR_S  0

/** TRACE_MEM_CURRENT_ADDR_REG register
 *  mem current addr
 */
#define TRACE_MEM_CURRENT_ADDR_REG(i) (REG_TRACE_BASE(i) + 0x8)
/** TRACE_MEM_CURRENT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  current_mem_addr,indicate that next writing addr
 */
#define TRACE_MEM_CURRENT_ADDR    0xFFFFFFFFU
#define TRACE_MEM_CURRENT_ADDR_M  (TRACE_MEM_CURRENT_ADDR_V << TRACE_MEM_CURRENT_ADDR_S)
#define TRACE_MEM_CURRENT_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_CURRENT_ADDR_S  0

/** TRACE_MEM_ADDR_UPDATE_REG register
 *  mem addr update
 */
#define TRACE_MEM_ADDR_UPDATE_REG(i) (REG_TRACE_BASE(i) + 0xc)
/** TRACE_MEM_CURRENT_ADDR_UPDATE : WT; bitpos: [0]; default: 0;
 *  when set, the  will
 *  \hyperref[fielddesc:TRACEMEMCURRENTADDR]{TRACE_MEM_CURRENT_ADDR} update to
 *  \hyperref[fielddesc:TRACEMEMSTARTADDR]{TRACE_MEM_START_ADDR}.
 */
#define TRACE_MEM_CURRENT_ADDR_UPDATE    (BIT(0))
#define TRACE_MEM_CURRENT_ADDR_UPDATE_M  (TRACE_MEM_CURRENT_ADDR_UPDATE_V << TRACE_MEM_CURRENT_ADDR_UPDATE_S)
#define TRACE_MEM_CURRENT_ADDR_UPDATE_V  0x00000001U
#define TRACE_MEM_CURRENT_ADDR_UPDATE_S  0

/** TRACE_FIFO_STATUS_REG register
 *  fifo status register
 */
#define TRACE_FIFO_STATUS_REG(i) (REG_TRACE_BASE(i) + 0x10)
/** TRACE_FIFO_EMPTY : RO; bitpos: [0]; default: 1;
 *  Represent whether the fifo is empty. \\1: empty \\0: not empty
 */
#define TRACE_FIFO_EMPTY    (BIT(0))
#define TRACE_FIFO_EMPTY_M  (TRACE_FIFO_EMPTY_V << TRACE_FIFO_EMPTY_S)
#define TRACE_FIFO_EMPTY_V  0x00000001U
#define TRACE_FIFO_EMPTY_S  0
/** TRACE_WORK_STATUS : RO; bitpos: [2:1]; default: 0;
 *  Represent trace work status: \\0: idle state \\1: working state\\ 2: wait state due
 *  to hart halted or havereset \\3: lost state
 */
#define TRACE_WORK_STATUS    0x00000003U
#define TRACE_WORK_STATUS_M  (TRACE_WORK_STATUS_V << TRACE_WORK_STATUS_S)
#define TRACE_WORK_STATUS_V  0x00000003U
#define TRACE_WORK_STATUS_S  1

/** TRACE_INTR_ENA_REG register
 *  interrupt enable register
 */
#define TRACE_INTR_ENA_REG(i) (REG_TRACE_BASE(i) + 0x14)
/** TRACE_FIFO_OVERFLOW_INTR_ENA : R/W; bitpos: [0]; default: 0;
 *  Set 1 enable fifo_overflow interrupt
 */
#define TRACE_FIFO_OVERFLOW_INTR_ENA    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_ENA_M  (TRACE_FIFO_OVERFLOW_INTR_ENA_V << TRACE_FIFO_OVERFLOW_INTR_ENA_S)
#define TRACE_FIFO_OVERFLOW_INTR_ENA_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_ENA_S  0
/** TRACE_MEM_FULL_INTR_ENA : R/W; bitpos: [1]; default: 0;
 *  Set 1 enable mem_full interrupt
 */
#define TRACE_MEM_FULL_INTR_ENA    (BIT(1))
#define TRACE_MEM_FULL_INTR_ENA_M  (TRACE_MEM_FULL_INTR_ENA_V << TRACE_MEM_FULL_INTR_ENA_S)
#define TRACE_MEM_FULL_INTR_ENA_V  0x00000001U
#define TRACE_MEM_FULL_INTR_ENA_S  1

/** TRACE_INTR_RAW_REG register
 *  interrupt status register
 */
#define TRACE_INTR_RAW_REG(i) (REG_TRACE_BASE(i) + 0x18)
/** TRACE_FIFO_OVERFLOW_INTR_RAW : RO; bitpos: [0]; default: 0;
 *  fifo_overflow interrupt status
 */
#define TRACE_FIFO_OVERFLOW_INTR_RAW    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_RAW_M  (TRACE_FIFO_OVERFLOW_INTR_RAW_V << TRACE_FIFO_OVERFLOW_INTR_RAW_S)
#define TRACE_FIFO_OVERFLOW_INTR_RAW_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_RAW_S  0
/** TRACE_MEM_FULL_INTR_RAW : RO; bitpos: [1]; default: 0;
 *  mem_full interrupt status
 */
#define TRACE_MEM_FULL_INTR_RAW    (BIT(1))
#define TRACE_MEM_FULL_INTR_RAW_M  (TRACE_MEM_FULL_INTR_RAW_V << TRACE_MEM_FULL_INTR_RAW_S)
#define TRACE_MEM_FULL_INTR_RAW_V  0x00000001U
#define TRACE_MEM_FULL_INTR_RAW_S  1

/** TRACE_INTR_CLR_REG register
 *  interrupt clear register
 */
#define TRACE_INTR_CLR_REG(i) (REG_TRACE_BASE(i) + 0x1c)
/** TRACE_FIFO_OVERFLOW_INTR_CLR : WT; bitpos: [0]; default: 0;
 *  Set 1 clear fifo overflow interrupt
 */
#define TRACE_FIFO_OVERFLOW_INTR_CLR    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_CLR_M  (TRACE_FIFO_OVERFLOW_INTR_CLR_V << TRACE_FIFO_OVERFLOW_INTR_CLR_S)
#define TRACE_FIFO_OVERFLOW_INTR_CLR_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_CLR_S  0
/** TRACE_MEM_FULL_INTR_CLR : WT; bitpos: [1]; default: 0;
 *  Set 1 clear mem full interrupt
 */
#define TRACE_MEM_FULL_INTR_CLR    (BIT(1))
#define TRACE_MEM_FULL_INTR_CLR_M  (TRACE_MEM_FULL_INTR_CLR_V << TRACE_MEM_FULL_INTR_CLR_S)
#define TRACE_MEM_FULL_INTR_CLR_V  0x00000001U
#define TRACE_MEM_FULL_INTR_CLR_S  1

/** TRACE_TRIGGER_REG register
 *  trigger register
 */
#define TRACE_TRIGGER_REG(i) (REG_TRACE_BASE(i) + 0x20)
/** TRACE_TRIGGER_ON : WT; bitpos: [0]; default: 0;
 *  Configure whether or not start trace.\\1: start trace \\0: invalid\\
 */
#define TRACE_TRIGGER_ON    (BIT(0))
#define TRACE_TRIGGER_ON_M  (TRACE_TRIGGER_ON_V << TRACE_TRIGGER_ON_S)
#define TRACE_TRIGGER_ON_V  0x00000001U
#define TRACE_TRIGGER_ON_S  0
/** TRACE_TRIGGER_OFF : WT; bitpos: [1]; default: 0;
 *  Configure whether or not stop trace.\\1: stop trace \\0: invalid\\
 */
#define TRACE_TRIGGER_OFF    (BIT(1))
#define TRACE_TRIGGER_OFF_M  (TRACE_TRIGGER_OFF_V << TRACE_TRIGGER_OFF_S)
#define TRACE_TRIGGER_OFF_V  0x00000001U
#define TRACE_TRIGGER_OFF_S  1
/** TRACE_MEM_LOOP : R/W; bitpos: [2]; default: 1;
 *  Configure memory loop mode. \\1: trace will loop write trace_mem. \\0: when
 *  mem_current_addr at mem_end_addr, it will stop at the mem_end_addr\\
 */
#define TRACE_MEM_LOOP    (BIT(2))
#define TRACE_MEM_LOOP_M  (TRACE_MEM_LOOP_V << TRACE_MEM_LOOP_S)
#define TRACE_MEM_LOOP_V  0x00000001U
#define TRACE_MEM_LOOP_S  2
/** TRACE_RESTART_ENA : R/W; bitpos: [3]; default: 1;
 *  Configure whether or not enable auto-restart.\\1: enable\\0: disable\\
 */
#define TRACE_RESTART_ENA    (BIT(3))
#define TRACE_RESTART_ENA_M  (TRACE_RESTART_ENA_V << TRACE_RESTART_ENA_S)
#define TRACE_RESTART_ENA_V  0x00000001U
#define TRACE_RESTART_ENA_S  3

/** TRACE_CONFIG_REG register
 *  trace configuration register
 */
#define TRACE_CONFIG_REG(i) (REG_TRACE_BASE(i) + 0x24)
/** TRACE_DM_TRIGGER_ENA : R/W; bitpos: [0]; default: 0;
 *  Configure whether or not enable cpu trigger action.\\1: enable\\0:disable\\
 */
#define TRACE_DM_TRIGGER_ENA    (BIT(0))
#define TRACE_DM_TRIGGER_ENA_M  (TRACE_DM_TRIGGER_ENA_V << TRACE_DM_TRIGGER_ENA_S)
#define TRACE_DM_TRIGGER_ENA_V  0x00000001U
#define TRACE_DM_TRIGGER_ENA_S  0
/** TRACE_RESET_ENA : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not enable trace cpu haverest, when enabled, if cpu have
 *  reset, the encoder will output a packet to report the address of the last
 *  instruction, and upon reset deassertion, the encoder start again.\\1: enabled\\0:
 *  disabled\\
 */
#define TRACE_RESET_ENA    (BIT(1))
#define TRACE_RESET_ENA_M  (TRACE_RESET_ENA_V << TRACE_RESET_ENA_S)
#define TRACE_RESET_ENA_V  0x00000001U
#define TRACE_RESET_ENA_S  1
/** TRACE_HALT_ENA : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not enable trace cpu is halted, when enabled, if the cpu
 *  halted, the encoder will output a packet to report the address of the last
 *  instruction, and upon halted deassertion, the encoder start again.When disabled,
 *  encoder will not report the last address before halted and first address after
 *  halted, cpu halted information will not be tracked. \\1: enabled\\0: disabled\\
 */
#define TRACE_HALT_ENA    (BIT(2))
#define TRACE_HALT_ENA_M  (TRACE_HALT_ENA_V << TRACE_HALT_ENA_S)
#define TRACE_HALT_ENA_V  0x00000001U
#define TRACE_HALT_ENA_S  2
/** TRACE_STALL_ENA : R/W; bitpos: [3]; default: 0;
 *  Configure whether or not enable stall cpu. When enabled, when the fifo almost full,
 *  the cpu will be stalled until the packets is able to write to fifo.\\1:
 *  enabled.\\0: disabled\\
 */
#define TRACE_STALL_ENA    (BIT(3))
#define TRACE_STALL_ENA_M  (TRACE_STALL_ENA_V << TRACE_STALL_ENA_S)
#define TRACE_STALL_ENA_V  0x00000001U
#define TRACE_STALL_ENA_S  3
/** TRACE_FULL_ADDRESS : R/W; bitpos: [4]; default: 0;
 *  Configure whether or not enable full-address mode.\\1: full address mode.\\0: delta
 *  address mode\\
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
#define TRACE_FILTER_CONTROL_REG(i) (REG_TRACE_BASE(i) + 0x28)
/** TRACE_FILTER_EN : R/W; bitpos: [0]; default: 0;
 *  Configure whether or not enable filter unit. \\1: enable filter.\\ 0: always match
 */
#define TRACE_FILTER_EN    (BIT(0))
#define TRACE_FILTER_EN_M  (TRACE_FILTER_EN_V << TRACE_FILTER_EN_S)
#define TRACE_FILTER_EN_V  0x00000001U
#define TRACE_FILTER_EN_S  0
/** TRACE_MATCH_COMP : R/W; bitpos: [1]; default: 0;
 *  when set, the comparator must be high in order for the filter to match
 */
#define TRACE_MATCH_COMP    (BIT(1))
#define TRACE_MATCH_COMP_M  (TRACE_MATCH_COMP_V << TRACE_MATCH_COMP_S)
#define TRACE_MATCH_COMP_V  0x00000001U
#define TRACE_MATCH_COMP_S  1
/** TRACE_MATCH_PRIVILEGE : R/W; bitpos: [2]; default: 0;
 *  when set, match privilege levels specified by
 *  \hyperref[fielddesc:TRACEMATCHCHOICEPRIVILEGE]{TRACE_MATCH_CHOICE_PRIVILEGE}.
 */
#define TRACE_MATCH_PRIVILEGE    (BIT(2))
#define TRACE_MATCH_PRIVILEGE_M  (TRACE_MATCH_PRIVILEGE_V << TRACE_MATCH_PRIVILEGE_S)
#define TRACE_MATCH_PRIVILEGE_V  0x00000001U
#define TRACE_MATCH_PRIVILEGE_S  2
/** TRACE_MATCH_ECAUSE : R/W; bitpos: [3]; default: 0;
 *  when set, start matching from exception cause codes specified by
 *  \hyperref[fielddesc:TRACEMATCHCHOICEECAUSE]{TRACE_MATCH_CHOICE_ECAUSE}, and stop
 *  matching upon return from the 1st matching exception.
 */
#define TRACE_MATCH_ECAUSE    (BIT(3))
#define TRACE_MATCH_ECAUSE_M  (TRACE_MATCH_ECAUSE_V << TRACE_MATCH_ECAUSE_S)
#define TRACE_MATCH_ECAUSE_V  0x00000001U
#define TRACE_MATCH_ECAUSE_S  3
/** TRACE_MATCH_INTERRUPT : R/W; bitpos: [4]; default: 0;
 *  when set, start matching from a trap with the interrupt level codes specified by
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
#define TRACE_FILTER_MATCH_CONTROL_REG(i) (REG_TRACE_BASE(i) + 0x2c)
/** TRACE_MATCH_CHOICE_PRIVILEGE : R/W; bitpos: [0]; default: 0;
 *  Select match which privilege level when
 *  \hyperref[fielddesc:TRACEMATCHPRIVILEGE]{TRACE_MATCH_PRIVILEGE} is set. \\1:
 *  machine mode. \\0: user mode
 */
#define TRACE_MATCH_CHOICE_PRIVILEGE    (BIT(0))
#define TRACE_MATCH_CHOICE_PRIVILEGE_M  (TRACE_MATCH_CHOICE_PRIVILEGE_V << TRACE_MATCH_CHOICE_PRIVILEGE_S)
#define TRACE_MATCH_CHOICE_PRIVILEGE_V  0x00000001U
#define TRACE_MATCH_CHOICE_PRIVILEGE_S  0
/** TRACE_MATCH_VALUE_INTERRUPT : R/W; bitpos: [1]; default: 0;
 *  Select which match which itype when
 *  \hyperref[fielddesc:TRACEMATCHINTERRUPT]{TRACE_MATCH_INTERRUP} is set. \\1: match
 *  itype of 2. \\0: match itype or 1.
 */
#define TRACE_MATCH_VALUE_INTERRUPT    (BIT(1))
#define TRACE_MATCH_VALUE_INTERRUPT_M  (TRACE_MATCH_VALUE_INTERRUPT_V << TRACE_MATCH_VALUE_INTERRUPT_S)
#define TRACE_MATCH_VALUE_INTERRUPT_V  0x00000001U
#define TRACE_MATCH_VALUE_INTERRUPT_S  1
/** TRACE_MATCH_CHOICE_ECAUSE : R/W; bitpos: [7:2]; default: 0;
 *  specified which ecause matched.
 */
#define TRACE_MATCH_CHOICE_ECAUSE    0x0000003FU
#define TRACE_MATCH_CHOICE_ECAUSE_M  (TRACE_MATCH_CHOICE_ECAUSE_V << TRACE_MATCH_CHOICE_ECAUSE_S)
#define TRACE_MATCH_CHOICE_ECAUSE_V  0x0000003FU
#define TRACE_MATCH_CHOICE_ECAUSE_S  2

/** TRACE_FILTER_COMPARATOR_CONTROL_REG register
 *  filter comparator match control register
 */
#define TRACE_FILTER_COMPARATOR_CONTROL_REG(i) (REG_TRACE_BASE(i) + 0x30)
/** TRACE_P_INPUT : R/W; bitpos: [0]; default: 0;
 *  Determines which input to compare against the primary comparator, \\0: iaddr, \\1:
 *  tval.
 */
#define TRACE_P_INPUT    (BIT(0))
#define TRACE_P_INPUT_M  (TRACE_P_INPUT_V << TRACE_P_INPUT_S)
#define TRACE_P_INPUT_V  0x00000001U
#define TRACE_P_INPUT_S  0
/** TRACE_P_FUNCTION : R/W; bitpos: [4:2]; default: 0;
 *  Select the primary comparator function. \\0: equal, \\1: not equal, \\2: less than,
 *  \\3: less than or equal, \\4: greater than, \\5: greater than or equal, \\other:
 *  always match
 */
#define TRACE_P_FUNCTION    0x00000007U
#define TRACE_P_FUNCTION_M  (TRACE_P_FUNCTION_V << TRACE_P_FUNCTION_S)
#define TRACE_P_FUNCTION_V  0x00000007U
#define TRACE_P_FUNCTION_S  2
/** TRACE_P_NOTIFY : R/W; bitpos: [5]; default: 0;
 *  Generate a trace packet explicitly reporting the address that cause the primary
 *  match
 */
#define TRACE_P_NOTIFY    (BIT(5))
#define TRACE_P_NOTIFY_M  (TRACE_P_NOTIFY_V << TRACE_P_NOTIFY_S)
#define TRACE_P_NOTIFY_V  0x00000001U
#define TRACE_P_NOTIFY_S  5
/** TRACE_S_INPUT : R/W; bitpos: [8]; default: 0;
 *  Determines which input to compare against the secondary comparator, \\0: iaddr,
 *  \\1: tval.
 */
#define TRACE_S_INPUT    (BIT(8))
#define TRACE_S_INPUT_M  (TRACE_S_INPUT_V << TRACE_S_INPUT_S)
#define TRACE_S_INPUT_V  0x00000001U
#define TRACE_S_INPUT_S  8
/** TRACE_S_FUNCTION : R/W; bitpos: [12:10]; default: 0;
 *  Select the secondary comparator function. \\0: equal, \\1: not equal, \\2: less
 *  than, \\3: less than or equal, \\4: greater than, \\5: greater than or equal,
 *  \\other: always match
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
 *  0: only primary matches, \\1: primary and secondary comparator both
 *  matches(P\&\&S),\\ 2:either primary or secondary comparator matches !(P\&\&S), \\3:
 *  set when primary matches and continue to match until after secondary comparator
 *  matches
 */
#define TRACE_MATCH_MODE    0x00000003U
#define TRACE_MATCH_MODE_M  (TRACE_MATCH_MODE_V << TRACE_MATCH_MODE_S)
#define TRACE_MATCH_MODE_V  0x00000003U
#define TRACE_MATCH_MODE_S  16

/** TRACE_FILTER_P_COMPARATOR_MATCH_REG register
 *  primary comparator match value
 */
#define TRACE_FILTER_P_COMPARATOR_MATCH_REG(i) (REG_TRACE_BASE(i) + 0x34)
/** TRACE_P_MATCH : R/W; bitpos: [31:0]; default: 0;
 *  primary comparator match value
 */
#define TRACE_P_MATCH    0xFFFFFFFFU
#define TRACE_P_MATCH_M  (TRACE_P_MATCH_V << TRACE_P_MATCH_S)
#define TRACE_P_MATCH_V  0xFFFFFFFFU
#define TRACE_P_MATCH_S  0

/** TRACE_FILTER_S_COMPARATOR_MATCH_REG register
 *  secondary comparator match value
 */
#define TRACE_FILTER_S_COMPARATOR_MATCH_REG(i) (REG_TRACE_BASE(i) + 0x38)
/** TRACE_S_MATCH : R/W; bitpos: [31:0]; default: 0;
 *  secondary comparator match value
 */
#define TRACE_S_MATCH    0xFFFFFFFFU
#define TRACE_S_MATCH_M  (TRACE_S_MATCH_V << TRACE_S_MATCH_S)
#define TRACE_S_MATCH_V  0xFFFFFFFFU
#define TRACE_S_MATCH_S  0

/** TRACE_RESYNC_PROLONGED_REG register
 *  resync configuration register
 */
#define TRACE_RESYNC_PROLONGED_REG(i) (REG_TRACE_BASE(i) + 0x3c)
/** TRACE_RESYNC_PROLONGED : R/W; bitpos: [23:0]; default: 128;
 *  count number, when count to this value, send a sync package
 */
#define TRACE_RESYNC_PROLONGED    0x00FFFFFFU
#define TRACE_RESYNC_PROLONGED_M  (TRACE_RESYNC_PROLONGED_V << TRACE_RESYNC_PROLONGED_S)
#define TRACE_RESYNC_PROLONGED_V  0x00FFFFFFU
#define TRACE_RESYNC_PROLONGED_S  0
/** TRACE_RESYNC_MODE : R/W; bitpos: [25:24]; default: 0;
 *  resyc mode sel: \\0: off, \\2: cycle count  \\3: package num count
 */
#define TRACE_RESYNC_MODE    0x00000003U
#define TRACE_RESYNC_MODE_M  (TRACE_RESYNC_MODE_V << TRACE_RESYNC_MODE_S)
#define TRACE_RESYNC_MODE_V  0x00000003U
#define TRACE_RESYNC_MODE_S  24

/** TRACE_AHB_CONFIG_REG register
 *  AHB config register
 */
#define TRACE_AHB_CONFIG_REG(i) (REG_TRACE_BASE(i) + 0x40)
/** TRACE_HBURST : R/W; bitpos: [2:0]; default: 0;
 *  set hburst
 */
#define TRACE_HBURST    0x00000007U
#define TRACE_HBURST_M  (TRACE_HBURST_V << TRACE_HBURST_S)
#define TRACE_HBURST_V  0x00000007U
#define TRACE_HBURST_S  0
/** TRACE_MAX_INCR : R/W; bitpos: [5:3]; default: 0;
 *  set max continuous access for incr mode
 */
#define TRACE_MAX_INCR    0x00000007U
#define TRACE_MAX_INCR_M  (TRACE_MAX_INCR_V << TRACE_MAX_INCR_S)
#define TRACE_MAX_INCR_V  0x00000007U
#define TRACE_MAX_INCR_S  3

/** TRACE_CLOCK_GATE_REG register
 *  Clock gate control register
 */
#define TRACE_CLOCK_GATE_REG(i) (REG_TRACE_BASE(i) + 0x44)
/** TRACE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  The bit is used to enable clock gate when access all registers in this module.
 */
#define TRACE_CLK_EN    (BIT(0))
#define TRACE_CLK_EN_M  (TRACE_CLK_EN_V << TRACE_CLK_EN_S)
#define TRACE_CLK_EN_V  0x00000001U
#define TRACE_CLK_EN_S  0

/** TRACE_DATE_REG register
 *  Version control register
 */
#define TRACE_DATE_REG(i) (REG_TRACE_BASE(i) + 0x3fc)
/** TRACE_DATE : R/W; bitpos: [27:0]; default: 35721984;
 *  version control register. Note that this default value stored is the latest date
 *  when the hardware logic was updated.
 */
#define TRACE_DATE    0x0FFFFFFFU
#define TRACE_DATE_M  (TRACE_DATE_V << TRACE_DATE_S)
#define TRACE_DATE_V  0x0FFFFFFFU
#define TRACE_DATE_S  0

#ifdef __cplusplus
}
#endif
