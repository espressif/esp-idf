/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Memory configuration registers */
/** Type of mem_start_addr register
 *  Memory start address
 */
typedef union {
    struct {
        /** mem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the start address of the trace memory
         */
        uint32_t mem_start_addr:32;
    };
    uint32_t val;
} trace_mem_start_addr_reg_t;

/** Type of mem_end_addr register
 *  Memory end address
 */
typedef union {
    struct {
        /** mem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the end address of the trace memory.
         */
        uint32_t mem_end_addr:32;
    };
    uint32_t val;
} trace_mem_end_addr_reg_t;

/** Type of mem_current_addr register
 *  Memory current addr
 */
typedef union {
    struct {
        /** mem_current_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the current memory address for writing.
         */
        uint32_t mem_current_addr:32;
    };
    uint32_t val;
} trace_mem_current_addr_reg_t;

/** Type of mem_addr_update register
 *  Memory address update
 */
typedef union {
    struct {
        /** mem_current_addr_update : WT; bitpos: [0]; default: 0;
         *  Configures whether to update the value of
         *  \hyperref[fielddesc:TRACEMEMCURRENTADDR]{TRACE_MEM_CURRENT_ADDR} to
         *  \hyperref[fielddesc:TRACEMEMSTARTADDR]{TRACE_MEM_START_ADDR}.\\
         *  0: Not update\\
         *  1: Update\\
         */
        uint32_t mem_current_addr_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} trace_mem_addr_update_reg_t;


/** Group: Trace fifo status register */
/** Type of fifo_status register
 *  FIFO status register
 */
typedef union {
    struct {
        /** fifo_empty : RO; bitpos: [0]; default: 1;
         *  Represent whether the FIFO is empty. \\1: Empty \\0: Not empty
         */
        uint32_t fifo_empty:1;
        /** work_status : RO; bitpos: [2:1]; default: 0;
         *  Represent the state of the encoder: \\0: Idle state \\1: Working state\\ 2: Wait
         *  state becasue hart is halted or in reset \\3: Lost state\\
         */
        uint32_t work_status:2;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} trace_fifo_status_reg_t;


/** Group: Interrupt registers */
/** Type of intr_ena register
 *  Interrupt enable register
 */
typedef union {
    struct {
        /** fifo_overflow_intr_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable TRACE_FIFO_OVERFLOW_INTR
         */
        uint32_t fifo_overflow_intr_ena:1;
        /** mem_full_intr_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable TRACE_MEM_FULL_INTR
         */
        uint32_t mem_full_intr_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} trace_intr_ena_reg_t;

/** Type of intr_raw register
 *  Interrupt raw status register
 */
typedef union {
    struct {
        /** fifo_overflow_intr_raw : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status of TRACE_FIFO_OVERFLOW_INTR.
         */
        uint32_t fifo_overflow_intr_raw:1;
        /** mem_full_intr_raw : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status of TRACE_MEM_FULL_INTR
         */
        uint32_t mem_full_intr_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} trace_intr_raw_reg_t;

/** Type of intr_clr register
 *  Interrupt clear register
 */
typedef union {
    struct {
        /** fifo_overflow_intr_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear TRACE_FIFO_OVERFLOW_INTR
         */
        uint32_t fifo_overflow_intr_clr:1;
        /** mem_full_intr_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear TRACE_MEM_FULL_INTR
         */
        uint32_t mem_full_intr_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} trace_intr_clr_reg_t;


/** Group: Trace configuration register */
/** Type of trigger register
 *  Trace enable register
 */
typedef union {
    struct {
        /** trigger_on : WT; bitpos: [0]; default: 0;
         *  Configure whether to enable the encoder.\\0: Invalid \\1: Enable\\
         */
        uint32_t trigger_on:1;
        /** trigger_off : WT; bitpos: [1]; default: 0;
         *  Configure whether to disable the encoder.\\0: Invalid \\1: Disable\\
         */
        uint32_t trigger_off:1;
        /** mem_loop : R/W; bitpos: [2]; default: 1;
         *  Configure the memory writing mode. \\0: Non-loop mode. \\1: Loop mode\\
         */
        uint32_t mem_loop:1;
        /** restart_ena : R/W; bitpos: [3]; default: 1;
         *  Configure whether or not enable automatic restart function for the encoder.\\0:
         *  Disable\\1: Enable\\
         */
        uint32_t restart_ena:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} trace_trigger_reg_t;

/** Type of config register
 *  trace configuration register
 */
typedef union {
    struct {
        /** dm_trigger_ena : R/W; bitpos: [0]; default: 0;
         *  Configure whether to enable the trigger signal.\\0: Disable\\1:enable\\
         */
        uint32_t dm_trigger_ena:1;
        /** reset_ena : R/W; bitpos: [1]; default: 0;
         *  Configure whether to reset, when enabeld, if cpu have reset, the encoder will
         *  output a packet to report the address of the last instruction, and upon reset
         *  deassertion, the encoder start again.\\0: Disable\\0: Enable\\
         */
        uint32_t reset_ena:1;
        /** halt_ena : R/W; bitpos: [2]; default: 0;
         *  Configure whether to enable the halt signal. \\1: Disable\\1: Enable\\
         */
        uint32_t halt_ena:1;
        /** stall_ena : R/W; bitpos: [3]; default: 0;
         *  Configure whether to enable the stall signal. \\0: Disable.\\1: Enable\\
         */
        uint32_t stall_ena:1;
        /** full_address : R/W; bitpos: [4]; default: 0;
         *  Configure the address mode.\\0: Delta address mode.\\1: Full address mode.\\
         */
        uint32_t full_address:1;
        /** implicit_except : R/W; bitpos: [5]; default: 0;
         *  Configure whether or not enabel implicit exception mode. When enabled,, do not sent
         *  exception address, only exception cause in exception packets.\\1: enabled\\0:
         *  disabled\\
         */
        uint32_t implicit_except:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} trace_config_reg_t;

/** Type of filter_control register
 *  filter control register
 */
typedef union {
    struct {
        /** filter_en : R/W; bitpos: [0]; default: 0;
         *  Configure whether to enable filtering. \\0: Disable, always match.\\ 1: Enable
         */
        uint32_t filter_en:1;
        /** match_comp : R/W; bitpos: [1]; default: 0;
         *  Configure whether to enable the comparator match mode. \\0: Disable \\1: Enable,
         *  the comparator must be high in order for the filter to match
         */
        uint32_t match_comp:1;
        /** match_privilege : R/W; bitpos: [2]; default: 0;
         *  Configure whether to enable the privilege match mode. \\0: Disable \\1: Enable,
         *  match privilege levels specified by
         *  \hyperref[fielddesc:TRACEMATCHCHOICEPRIVILEGE]{TRACE_MATCH_CHOICE_PRIVILEGE}.
         */
        uint32_t match_privilege:1;
        /** match_ecause : R/W; bitpos: [3]; default: 0;
         *  Configure whether to enable ecause match mode. \\0: Disable \\1: Enable, start
         *  matching from exception cause codes specified by
         *  \hyperref[fielddesc:TRACEMATCHCHOICEECAUSE]{TRACE_MATCH_CHOICE_ECAUSE}, and stop
         *  matching upon return from the 1st matching exception.
         */
        uint32_t match_ecause:1;
        /** match_interrupt : R/W; bitpos: [4]; default: 0;
         *  Configure whether to enable the interrupt match mode. \\0: Disable \\1: Enable,
         *  start matching from a trap with the interrupt level codes specified by
         *  \hyperref[fielddesc:TRACEMATCHVALUEINTERRUPT]{TRACE_MATCH_VALUE_INTERRUPT}, and
         *  stop matching upon return from the 1st matching trap.
         */
        uint32_t match_interrupt:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} trace_filter_control_reg_t;

/** Type of filter_match_control register
 *  filter match control register
 */
typedef union {
    struct {
        /** match_choice_privilege : R/W; bitpos: [0]; default: 0;
         *  Configures the privilege level for matching. Valid only when
         *  \hyperref[fielddesc:TRACEMATCHPRIVILEGE]{TRACE_MATCH_PRIVILEGE} is set. \\0: User
         *  mode. \\1: Machine mode
         */
        uint32_t match_choice_privilege:1;
        /** match_value_interrupt : R/W; bitpos: [1]; default: 0;
         *  Configures the interrupt level for match. Valid only when when
         *  \hyperref[fielddesc:TRACEMATCHINTERRUPT]{TRACE_MATCH_INTERRUP} is set. \\0:
         *  itype=2. \\0: itype=2.
         */
        uint32_t match_value_interrupt:1;
        /** match_choice_ecause : R/W; bitpos: [7:2]; default: 0;
         *  Configures the ecause code for matching.
         */
        uint32_t match_choice_ecause:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} trace_filter_match_control_reg_t;

/** Type of filter_comparator_control register
 *  filter comparator match control register
 */
typedef union {
    struct {
        /** p_input : R/W; bitpos: [0]; default: 0;
         *  Configures the input of the primary comparator for matching: \\0: iaddr \\1: tval\\
         */
        uint32_t p_input:1;
        uint32_t reserved_1:1;
        /** p_function : R/W; bitpos: [4:2]; default: 0;
         *  Configures the function for the primary comparator. \\0: Equal, \\1: Not equal,
         *  \\2: Less than, \\3: Less than or equal, \\4: Greater than, \\5: Greater than or
         *  equal, \\Other: Always match
         */
        uint32_t p_function:3;
        /** p_notify : R/W; bitpos: [5]; default: 0;
         *  Configure whether to explicitly report an instruction address matched against the
         *  primary comparator. \\0:Not report \\1:Report
         */
        uint32_t p_notify:1;
        uint32_t reserved_6:2;
        /** s_input : R/W; bitpos: [8]; default: 0;
         *  Configures the input of the secondary comparator for matching: \\0: iaddr \\1:
         *  tval\\
         */
        uint32_t s_input:1;
        uint32_t reserved_9:1;
        /** s_function : R/W; bitpos: [12:10]; default: 0;
         *  Configures the function for the secondary comparator. \\0: Equal, \\1: Not equal,
         *  \\2: Less than, \\3: Less than or equal, \\4: Greater than, \\5: Greater than or
         *  equal, \\Other: Always match
         */
        uint32_t s_function:3;
        /** s_notify : R/W; bitpos: [13]; default: 0;
         *  Generate a trace packet explicitly reporting the address that cause the secondary
         *  match
         */
        uint32_t s_notify:1;
        uint32_t reserved_14:2;
        /** match_mode : R/W; bitpos: [17:16]; default: 0;
         *  Configures the comparator match mode: \\0: Only the primary comparator matches \\1:
         *  Both primary and secondary comparator matches(P\&\&S) \\ 2:Neither primary or
         *  secondary comparator matches !(P\&\&S) \\3: Start filtering when the primary
         *  comparator matches and stop filtering when the secondary comparator matches\\
         */
        uint32_t match_mode:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} trace_filter_comparator_control_reg_t;

/** Type of filter_p_comparator_match register
 *  primary comparator match value
 */
typedef union {
    struct {
        /** p_match : R/W; bitpos: [31:0]; default: 0;
         *  Configures the match value for the primary comparator
         */
        uint32_t p_match:32;
    };
    uint32_t val;
} trace_filter_p_comparator_match_reg_t;

/** Type of filter_s_comparator_match register
 *  secondary comparator match value
 */
typedef union {
    struct {
        /** s_match : R/W; bitpos: [31:0]; default: 0;
         *  Configures the match value for the secondary comparator
         */
        uint32_t s_match:32;
    };
    uint32_t val;
} trace_filter_s_comparator_match_reg_t;

/** Type of resync_prolonged register
 *  Resync configuration register
 */
typedef union {
    struct {
        /** resync_prolonged : R/W; bitpos: [23:0]; default: 128;
         *  Configures the threshold for synchronization counter
         */
        uint32_t resync_prolonged:24;
        /** resync_mode : R/W; bitpos: [25:24]; default: 0;
         *  Configures the synchronization mode: \\0: Disable the synchronization counter \\1:
         *  Invalid \\2: Synchronization counter counts by packet  \\3: Synchronization counter
         *  counts by cycle\\
         */
        uint32_t resync_mode:2;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} trace_resync_prolonged_reg_t;

/** Type of ahb_config register
 *  AHB config register
 */
typedef union {
    struct {
        /** hburst : R/W; bitpos: [2:0]; default: 0;
         *  Configures the AHB burst mode. \\0: SIGNLE \\1: INCR(length not defined) \\2:INCR4
         *  \\4:INCR8 \\Others:Invalid
         */
        uint32_t hburst:3;
        /** max_incr : R/W; bitpos: [5:3]; default: 0;
         *  Configures the maximum burst length for INCR mode
         */
        uint32_t max_incr:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} trace_ahb_config_reg_t;


/** Group: Clock Gate Control and configuration register */
/** Type of clock_gate register
 *  Clock gate control register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures register clock gating. \\0: Support clock only when the application
         *  writes registers to save power. \\1:Always force the clock on for registers \\ This
         *  bit does't affect register access.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} trace_clock_gate_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35721984;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} trace_date_reg_t;


typedef struct {
    volatile trace_mem_start_addr_reg_t mem_start_addr;
    volatile trace_mem_end_addr_reg_t mem_end_addr;
    volatile trace_mem_current_addr_reg_t mem_current_addr;
    volatile trace_mem_addr_update_reg_t mem_addr_update;
    volatile trace_fifo_status_reg_t fifo_status;
    volatile trace_intr_ena_reg_t intr_ena;
    volatile trace_intr_raw_reg_t intr_raw;
    volatile trace_intr_clr_reg_t intr_clr;
    volatile trace_trigger_reg_t trigger;
    volatile trace_config_reg_t config;
    volatile trace_filter_control_reg_t filter_control;
    volatile trace_filter_match_control_reg_t filter_match_control;
    volatile trace_filter_comparator_control_reg_t filter_comparator_control;
    volatile trace_filter_p_comparator_match_reg_t filter_p_comparator_match;
    volatile trace_filter_s_comparator_match_reg_t filter_s_comparator_match;
    volatile trace_resync_prolonged_reg_t resync_prolonged;
    volatile trace_ahb_config_reg_t ahb_config;
    volatile trace_clock_gate_reg_t clock_gate;
    uint32_t reserved_048[237];
    volatile trace_date_reg_t date;
} trace_dev_t;

extern trace_dev_t TRACE;

#ifndef __cplusplus
_Static_assert(sizeof(trace_dev_t) == 0x400, "Invalid size of trace_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
