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

/** Group: configuration registers */
/** Type of log_setting register
 *  log config regsiter
 */
typedef union {
    struct {
        /** log_ena : R/W; bitpos: [2:0]; default: 0;
         *  enable bus log. BIT0: hp-cpu, BIT1: lp-cpu, BIT2: DMA.823 don't support lp-cpu
         */
        uint32_t log_ena:3;
        /** log_mode : R/W; bitpos: [6:3]; default: 0;
         *  This field must be onehot. 4'b0001 : WR monitor, 4'b0010: WORD monitor, 4'b0100:
         *  HALFWORD monitor, 4'b1000: BYTE monitor.
         */
        uint32_t log_mode:4;
        /** log_mem_loop_enable : R/W; bitpos: [7]; default: 1;
         *  Set 1 enable mem_loop, it will loop write at the range of MEM_START and MEM_END
         */
        uint32_t log_mem_loop_enable:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} mem_monitor_log_setting_reg_t;

/** Type of log_check_data register
 *  check data regsiter
 */
typedef union {
    struct {
        /** log_check_data : R/W; bitpos: [31:0]; default: 0;
         *  The special check data, when write this special data, it will trigger logging.
         */
        uint32_t log_check_data:32;
    };
    uint32_t val;
} mem_monitor_log_check_data_reg_t;

/** Type of log_data_mask register
 *  check data mask register
 */
typedef union {
    struct {
        /** log_data_mask : R/W; bitpos: [3:0]; default: 0;
         *  byte mask enable, BIT0 mask the first byte of MEM_MONITOR_LOG_CHECK_DATA, and BIT1
         *  mask second byte, and so on.
         */
        uint32_t log_data_mask:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} mem_monitor_log_data_mask_reg_t;

/** Type of log_min register
 *  log boundary regsiter
 */
typedef union {
    struct {
        /** log_min : R/W; bitpos: [31:0]; default: 0;
         *  the min address of log range
         */
        uint32_t log_min:32;
    };
    uint32_t val;
} mem_monitor_log_min_reg_t;

/** Type of log_max register
 *  log boundary regsiter
 */
typedef union {
    struct {
        /** log_max : R/W; bitpos: [31:0]; default: 0;
         *  the max address of log range
         */
        uint32_t log_max:32;
    };
    uint32_t val;
} mem_monitor_log_max_reg_t;

/** Type of log_mem_start register
 *  log message store range register
 */
typedef union {
    struct {
        /** log_mem_start : R/W; bitpos: [31:0]; default: 0;
         *  the start address of writing logging message
         */
        uint32_t log_mem_start:32;
    };
    uint32_t val;
} mem_monitor_log_mem_start_reg_t;

/** Type of log_mem_end register
 *  log message store range register
 */
typedef union {
    struct {
        /** log_mem_end : R/W; bitpos: [31:0]; default: 0;
         *  the end address of writing logging message
         */
        uint32_t log_mem_end:32;
    };
    uint32_t val;
} mem_monitor_log_mem_end_reg_t;

/** Type of log_mem_current_addr register
 *  current writing address.
 */
typedef union {
    struct {
        /** log_mem_current_addr : RO; bitpos: [31:0]; default: 0;
         *  means next writing address
         */
        uint32_t log_mem_current_addr:32;
    };
    uint32_t val;
} mem_monitor_log_mem_current_addr_reg_t;

/** Type of log_mem_addr_update register
 *  writing address update
 */
typedef union {
    struct {
        /** log_mem_addr_update : WT; bitpos: [0]; default: 0;
         *  Set 1 to updata MEM_MONITOR_LOG_MEM_CURRENT_ADDR, when set 1,
         *  MEM_MONITOR_LOG_MEM_CURRENT_ADDR will update to MEM_MONITOR_LOG_MEM_START
         */
        uint32_t log_mem_addr_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mem_monitor_log_mem_addr_update_reg_t;

/** Type of log_mem_full_flag register
 *  full flag status register
 */
typedef union {
    struct {
        /** log_mem_full_flag : RO; bitpos: [0]; default: 0;
         *  1 means memory write loop at least one time at the range of MEM_START and MEM_END
         */
        uint32_t log_mem_full_flag:1;
        /** clr_log_mem_full_flag : WT; bitpos: [1]; default: 0;
         *  Set 1 to clr MEM_MONITOR_LOG_MEM_FULL_FLAG
         */
        uint32_t clr_log_mem_full_flag:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} mem_monitor_log_mem_full_flag_reg_t;


/** Group: clk register */
/** Type of clock_gate register
 *  clock gate force on register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Set 1 to force on the clk of mem_monitor register
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mem_monitor_clock_gate_reg_t;


/** Group: version register */
/** Type of date register
 *  version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35660096;
         *  version register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} mem_monitor_date_reg_t;


typedef struct {
    volatile mem_monitor_log_setting_reg_t log_setting;
    volatile mem_monitor_log_check_data_reg_t log_check_data;
    volatile mem_monitor_log_data_mask_reg_t log_data_mask;
    volatile mem_monitor_log_min_reg_t log_min;
    volatile mem_monitor_log_max_reg_t log_max;
    volatile mem_monitor_log_mem_start_reg_t log_mem_start;
    volatile mem_monitor_log_mem_end_reg_t log_mem_end;
    volatile mem_monitor_log_mem_current_addr_reg_t log_mem_current_addr;
    volatile mem_monitor_log_mem_addr_update_reg_t log_mem_addr_update;
    volatile mem_monitor_log_mem_full_flag_reg_t log_mem_full_flag;
    volatile mem_monitor_clock_gate_reg_t clock_gate;
    uint32_t reserved_02c[244];
    volatile mem_monitor_date_reg_t date;
} mem_monitor_dev_t;

extern mem_monitor_dev_t MEM_MONITOR;

#ifndef __cplusplus
_Static_assert(sizeof(mem_monitor_dev_t) == 0x400, "Invalid size of mem_monitor_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
