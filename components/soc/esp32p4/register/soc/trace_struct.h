/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Trace memory configuration registers */
/** Type of mem_start_addr register
 *  mem start addr
 */
typedef union {
    struct {
        /** mem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  The start address of trace memory
         */
        uint32_t mem_start_addr:32;
    };
    uint32_t val;
} trace_mem_start_addr_reg_t;

/** Type of mem_end_addr register
 *  mem end addr
 */
typedef union {
    struct {
        /** mem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  The end address of trace memory
         */
        uint32_t mem_end_addr:32;
    };
    uint32_t val;
} trace_mem_end_addr_reg_t;

/** Type of mem_current_addr register
 *  mem current addr
 */
typedef union {
    struct {
        /** mem_current_addr : RO; bitpos: [31:0]; default: 0;
         *  current_mem_addr,indicate that next writing addr
         */
        uint32_t mem_current_addr:32;
    };
    uint32_t val;
} trace_mem_current_addr_reg_t;

/** Type of mem_addr_update register
 *  mem addr update
 */
typedef union {
    struct {
        /** mem_current_addr_update : WT; bitpos: [0]; default: 0;
         *  when set, the  will
         *  \hyperref[fielddesc:TRACEMEMCURRENTADDR]{TRACE_MEM_CURRENT_ADDR} update to
         *  \hyperref[fielddesc:TRACEMEMSTARTADDR]{TRACE_MEM_START_ADDR}.
         */
        uint32_t mem_current_addr_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} trace_mem_addr_update_reg_t;


/** Group: Trace fifo status register */
/** Type of fifo_status register
 *  fifo status register
 */
typedef union {
    struct {
        /** fifo_empty : RO; bitpos: [0]; default: 1;
         *  Represent whether the fifo is empty. \\1: empty \\0: not empty
         */
        uint32_t fifo_empty:1;
        /** work_status : RO; bitpos: [2:1]; default: 0;
         *  Represent trace work status: \\0: idle state \\1: working state\\ 2: wait state due
         *  to hart halted or havereset \\3: lost state
         */
        uint32_t work_status:2;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} trace_fifo_status_reg_t;


/** Group: Trace interrupt configuration registers */
/** Type of intr_ena register
 *  interrupt enable register
 */
typedef union {
    struct {
        /** fifo_overflow_intr_ena : R/W; bitpos: [0]; default: 0;
         *  Set 1 enable fifo_overflow interrupt
         */
        uint32_t fifo_overflow_intr_ena:1;
        /** mem_full_intr_ena : R/W; bitpos: [1]; default: 0;
         *  Set 1 enable mem_full interrupt
         */
        uint32_t mem_full_intr_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} trace_intr_ena_reg_t;

/** Type of intr_raw register
 *  interrupt status register
 */
typedef union {
    struct {
        /** fifo_overflow_intr_raw : RO; bitpos: [0]; default: 0;
         *  fifo_overflow interrupt status
         */
        uint32_t fifo_overflow_intr_raw:1;
        /** mem_full_intr_raw : RO; bitpos: [1]; default: 0;
         *  mem_full interrupt status
         */
        uint32_t mem_full_intr_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} trace_intr_raw_reg_t;

/** Type of intr_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        /** fifo_overflow_intr_clr : WT; bitpos: [0]; default: 0;
         *  Set 1 clear fifo overflow interrupt
         */
        uint32_t fifo_overflow_intr_clr:1;
        /** mem_full_intr_clr : WT; bitpos: [1]; default: 0;
         *  Set 1 clear mem full interrupt
         */
        uint32_t mem_full_intr_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} trace_intr_clr_reg_t;


/** Group: Trace configuration register */
/** Type of trigger register
 *  trigger register
 */
typedef union {
    struct {
        /** trigger_on : WT; bitpos: [0]; default: 0;
         *  Configure whether or not start trace.\\1: start trace \\0: invalid\\
         */
        uint32_t trigger_on:1;
        /** trigger_off : WT; bitpos: [1]; default: 0;
         *  Configure whether or not stop trace.\\1: stop trace \\0: invalid\\
         */
        uint32_t trigger_off:1;
        /** mem_loop : R/W; bitpos: [2]; default: 1;
         *  Configure memory loop mode. \\1: trace will loop write trace_mem. \\0: when
         *  mem_current_addr at mem_end_addr, it will stop at the mem_end_addr\\
         */
        uint32_t mem_loop:1;
        /** restart_ena : R/W; bitpos: [3]; default: 1;
         *  Configure whether or not enable auto-restart.\\1: enable\\0: disable\\
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
         *  Configure whether or not enable cpu trigger action.\\1: enable\\0:disable\\
         */
        uint32_t dm_trigger_ena:1;
        /** reset_ena : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not enable trace cpu haverest, when enabled, if cpu have
         *  reset, the encoder will output a packet to report the address of the last
         *  instruction, and upon reset deassertion, the encoder start again.\\1: enabled\\0:
         *  disabled\\
         */
        uint32_t reset_ena:1;
        /** halt_ena : R/W; bitpos: [2]; default: 0;
         *  Configure whether or not enable trace cpu is halted, when enabled, if the cpu
         *  halted, the encoder will output a packet to report the address of the last
         *  instruction, and upon halted deassertion, the encoder start again.When disabled,
         *  encoder will not report the last address before halted and first address after
         *  halted, cpu halted information will not be tracked. \\1: enabled\\0: disabled\\
         */
        uint32_t halt_ena:1;
        /** stall_ena : R/W; bitpos: [3]; default: 0;
         *  Configure whether or not enable stall cpu. When enabled, when the fifo almost full,
         *  the cpu will be stalled until the packets is able to write to fifo.\\1:
         *  enabled.\\0: disabled\\
         */
        uint32_t stall_ena:1;
        /** full_address : R/W; bitpos: [4]; default: 0;
         *  Configure whether or not enable full-address mode.\\1: full address mode.\\0: delta
         *  address mode\\
         */
        uint32_t full_address:1;
        /** implicit_except : R/W; bitpos: [5]; default: 0;
         *  Configure whether or not enable implicit exception mode. When enabled,, do not sent
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
         *  Configure whether or not enable filter unit. \\1: enable filter.\\ 0: always match
         */
        uint32_t filter_en:1;
        /** match_comp : R/W; bitpos: [1]; default: 0;
         *  when set, the comparator must be high in order for the filter to match
         */
        uint32_t match_comp:1;
        /** match_privilege : R/W; bitpos: [2]; default: 0;
         *  when set, match privilege levels specified by
         *  \hyperref[fielddesc:TRACEMATCHCHOICEPRIVILEGE]{TRACE_MATCH_CHOICE_PRIVILEGE}.
         */
        uint32_t match_privilege:1;
        /** match_ecause : R/W; bitpos: [3]; default: 0;
         *  when set, start matching from exception cause codes specified by
         *  \hyperref[fielddesc:TRACEMATCHCHOICEECAUSE]{TRACE_MATCH_CHOICE_ECAUSE}, and stop
         *  matching upon return from the 1st matching exception.
         */
        uint32_t match_ecause:1;
        /** match_interrupt : R/W; bitpos: [4]; default: 0;
         *  when set, start matching from a trap with the interrupt level codes specified by
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
         *  Select match which privilege level when
         *  \hyperref[fielddesc:TRACEMATCHPRIVILEGE]{TRACE_MATCH_PRIVILEGE} is set. \\1:
         *  machine mode. \\0: user mode
         */
        uint32_t match_choice_privilege:1;
        /** match_value_interrupt : R/W; bitpos: [1]; default: 0;
         *  Select which match which itype when
         *  \hyperref[fielddesc:TRACEMATCHINTERRUPT]{TRACE_MATCH_INTERRUP} is set. \\1: match
         *  itype of 2. \\0: match itype or 1.
         */
        uint32_t match_value_interrupt:1;
        /** match_choice_ecause : R/W; bitpos: [7:2]; default: 0;
         *  specified which ecause matched.
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
         *  Determines which input to compare against the primary comparator, \\0: iaddr, \\1:
         *  tval.
         */
        uint32_t p_input:1;
        uint32_t reserved_1:1;
        /** p_function : R/W; bitpos: [4:2]; default: 0;
         *  Select the primary comparator function. \\0: equal, \\1: not equal, \\2: less than,
         *  \\3: less than or equal, \\4: greater than, \\5: greater than or equal, \\other:
         *  always match
         */
        uint32_t p_function:3;
        /** p_notify : R/W; bitpos: [5]; default: 0;
         *  Generate a trace packet explicitly reporting the address that cause the primary
         *  match
         */
        uint32_t p_notify:1;
        uint32_t reserved_6:2;
        /** s_input : R/W; bitpos: [8]; default: 0;
         *  Determines which input to compare against the secondary comparator, \\0: iaddr,
         *  \\1: tval.
         */
        uint32_t s_input:1;
        uint32_t reserved_9:1;
        /** s_function : R/W; bitpos: [12:10]; default: 0;
         *  Select the secondary comparator function. \\0: equal, \\1: not equal, \\2: less
         *  than, \\3: less than or equal, \\4: greater than, \\5: greater than or equal,
         *  \\other: always match
         */
        uint32_t s_function:3;
        /** s_notify : R/W; bitpos: [13]; default: 0;
         *  Generate a trace packet explicitly reporting the address that cause the secondary
         *  match
         */
        uint32_t s_notify:1;
        uint32_t reserved_14:2;
        /** match_mode : R/W; bitpos: [17:16]; default: 0;
         *  0: only primary matches, \\1: primary and secondary comparator both
         *  matches(P\&\&S),\\ 2:either primary or secondary comparator matches !(P\&\&S), \\3:
         *  set when primary matches and continue to match until after secondary comparator
         *  matches
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
         *  primary comparator match value
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
         *  secondary comparator match value
         */
        uint32_t s_match:32;
    };
    uint32_t val;
} trace_filter_s_comparator_match_reg_t;

/** Type of resync_prolonged register
 *  resync configuration register
 */
typedef union {
    struct {
        /** resync_prolonged : R/W; bitpos: [23:0]; default: 128;
         *  count number, when count to this value, send a sync package
         */
        uint32_t resync_prolonged:24;
        /** resync_mode : R/W; bitpos: [25:24]; default: 0;
         *  resyc mode sel: \\0: off, \\2: cycle count  \\3: package num count
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
         *  set hburst
         */
        uint32_t hburst:3;
        /** max_incr : R/W; bitpos: [5:3]; default: 0;
         *  set max continuous access for incr mode
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
         *  The bit is used to enable clock gate when access all registers in this module.
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
         *  version control register. Note that this default value stored is the latest date
         *  when the hardware logic was updated.
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

extern trace_dev_t TRACE0;
extern trace_dev_t TRACE1;

#ifndef __cplusplus
_Static_assert(sizeof(trace_dev_t) == 0x400, "Invalid size of trace_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
