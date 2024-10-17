/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
        /** mem_staet_addr : R/W; bitpos: [31:0]; default: 0;
         *  The start address of trace memory
         */
        uint32_t mem_staet_addr:32;
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
         *  when set this reg, the current_mem_addr will update to start_addr
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
         *  1 indicate that fifo is empty
         */
        uint32_t fifo_empty:1;
        /** work_status : RO; bitpos: [1]; default: 0;
         *  mem_full interrupt status
         */
        uint32_t work_status:1;
        uint32_t reserved_2:30;
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
         *  Set 1 clr fifo overflow interrupt
         */
        uint32_t fifo_overflow_intr_clr:1;
        /** mem_full_intr_clr : WT; bitpos: [1]; default: 0;
         *  Set 1 clr mem full interrupt
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
         *  [0] set 1 start trace.
         */
        uint32_t trigger_on:1;
        /** trigger_off : WT; bitpos: [1]; default: 0;
         *  set 1 stop trace.
         */
        uint32_t trigger_off:1;
        /** mem_loop : R/W; bitpos: [2]; default: 1;
         *  if this reg is 1, trace will loop write trace_mem.  If is 0, when mem_current_addr
         *  at mem_end_addr, it will stop at the mem_end_addr
         */
        uint32_t mem_loop:1;
        /** restart_ena : R/W; bitpos: [3]; default: 1;
         *  enable encoder auto-restart, when lost package, the encoder will end, if enable
         *  auto-restart, when fifo empty, encoder will restart and send a sync package.
         */
        uint32_t restart_ena:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} trace_trigger_reg_t;

/** Type of resync_prolonged register
 *  resync configuration register
 */
typedef union {
    struct {
        /** resync_prolonged : R/W; bitpos: [23:0]; default: 128;
         *  count number, when count to this value, send a sync package
         */
        uint32_t resync_prolonged:24;
        /** resync_mode : R/W; bitpos: [24]; default: 0;
         *  resyc mode sel: 0: default, cycle count   1: package num count
         */
        uint32_t resync_mode:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} trace_resync_prolonged_reg_t;


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
        /** date : R/W; bitpos: [27:0]; default: 35663920;
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
    volatile trace_resync_prolonged_reg_t resync_prolonged;
    volatile trace_clock_gate_reg_t clock_gate;
    uint32_t reserved_02c[244];
    volatile trace_date_reg_t date;
} trace_dev_t;

extern trace_dev_t TRACE;

#ifndef __cplusplus
_Static_assert(sizeof(trace_dev_t) == 0x400, "Invalid size of trace_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
