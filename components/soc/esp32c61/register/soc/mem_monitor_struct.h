/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Bus logging configuration registers */
/** Type of log_setting register
 *  Configures bus access logging
 */
typedef union {
    struct {
        /** log_mode : R/W; bitpos: [3:0]; default: 0;
         *  Configures monitoring modes.
         *  1: Enable write monitoring
         *  2: Enable word monitoring
         *  4: Enable halfword monitoring
         *  8: Enable byte monitoring
         *  Other values: Invalid
         */
        uint32_t log_mode:4;
        /** log_mem_loop_enable : R/W; bitpos: [4]; default: 1;
         *  Configures the writing mode for recorded data.
         *  0: Non-loop mode
         *  1: Loop mode
         */
        uint32_t log_mem_loop_enable:1;
        uint32_t reserved_5:3;
        /** log_core_ena : R/W; bitpos: [15:8]; default: 0;
         *  Configures whether to enable HP CPU bus access logging.
         *  0: Disable
         *  1: Enable
         *  Other values: Invalid
         */
        uint32_t log_core_ena:8;
        /** log_dma_0_ena : R/W; bitpos: [23:16]; default: 0;
         *  Configures whether to enable DMA_0 bus access logging.
         *  0: Disable
         *  1: Enable
         *  Other values: Invalid
         */
        uint32_t log_dma_0_ena:8;
        /** log_dma_1_ena : R/W; bitpos: [31:24]; default: 0;
         *  Configures whether to enable DMA_1 bus access logging.
         *  0: Disable
         *  1: Enable
         *  Other values: Invalid
         */
        uint32_t log_dma_1_ena:8;
    };
    uint32_t val;
} mem_monitor_log_setting_reg_t;

/** Type of log_setting1 register
 *  Configures bus access logging
 */
typedef union {
    struct {
        /** log_dma_2_ena : R/W; bitpos: [7:0]; default: 0;
         *  Configures whether to enable DMA_2 bus access logging.
         *  0: Disable
         *  1: Enable
         *  Other values: Invalid
         */
        uint32_t log_dma_2_ena:8;
        /** log_dma_3_ena : R/W; bitpos: [15:8]; default: 0;
         *  Configures whether to enable DMA_3 bus access logging.
         *  0: Disable
         *  1: Enable
         *  Other values: Invalid
         */
        uint32_t log_dma_3_ena:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mem_monitor_log_setting1_reg_t;

/** Type of log_check_data register
 *  Configures data to be monitored for bus access logging
 */
typedef union {
    struct {
        /** log_check_data : R/W; bitpos: [31:0]; default: 0;
         *  Configures the data to be monitored during bus accessing.
         */
        uint32_t log_check_data:32;
    };
    uint32_t val;
} mem_monitor_log_check_data_reg_t;

/** Type of log_data_mask register
 *  Configures data mask for bus access logging
 */
typedef union {
    struct {
        /** log_data_mask : R/W; bitpos: [3:0]; default: 0;
         *  Configures which byte(s) in MEM_MONITOR_LOG_CHECK_DATA_REG to mask. Multiple bytes
         *  can be masked at the same time.
         *  bit[0]: Configures whether to mask the least significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG.
         *  0: Not mask
         *  1: Mask
         *  bit[1]: Configures whether to mask the second least significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG.
         *  0: Not mask
         *  1: Mask
         *  bit[2]: Configures whether to mask the second most significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG.
         *  0: Not mask
         *  1: Mask
         *  bit[3]: Configures whether to mask the most significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG.
         *  0: Not mask
         *  1: Mask
         */
        uint32_t log_data_mask:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} mem_monitor_log_data_mask_reg_t;

/** Type of log_min register
 *  Configures the monitored lower address for bus access logging
 */
typedef union {
    struct {
        /** log_min : R/W; bitpos: [31:0]; default: 0;
         *  Configures the lower bound address of the monitored address space.
         */
        uint32_t log_min:32;
    };
    uint32_t val;
} mem_monitor_log_min_reg_t;

/** Type of log_max register
 *  Configures the monitored upper address for bus access logging
 */
typedef union {
    struct {
        /** log_max : R/W; bitpos: [31:0]; default: 0;
         *  Configures the upper bound address of the monitored address space.
         */
        uint32_t log_max:32;
    };
    uint32_t val;
} mem_monitor_log_max_reg_t;

/** Type of log_mon_addr_update_0 register
 *  Configures whether to update the monitored address space for HP CPU bus access
 *  logging
 */
typedef union {
    struct {
        /** log_mon_addr_core_update : WT; bitpos: [7:0]; default: 0;
         *  Configures whether to update the monitored address space of the HP CPU bus as the
         *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
         *  0: Not update
         *  1: Update
         *  Other values: Invalid
         */
        uint32_t log_mon_addr_core_update:8;
        uint32_t reserved_8:23;
        /** log_mon_addr_all_update : WT; bitpos: [31]; default: 0;
         *  Configures whether to update the monitored address space of all masters as the
         *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
         *  0: Not update
         *  1: Update
         */
        uint32_t log_mon_addr_all_update:1;
    };
    uint32_t val;
} mem_monitor_log_mon_addr_update_0_reg_t;

/** Type of log_mon_addr_update_1 register
 *  Configures whether to update the monitored address space for DMA_0 bus access
 *  logging
 */
typedef union {
    struct {
        /** log_mon_addr_dma_0_update : WT; bitpos: [7:0]; default: 0;
         *  Configures whether to update the monitored address space of the DMA_0 bus as the
         *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
         *  0: Not update
         *  1: Update
         *  Other values: Invalid
         */
        uint32_t log_mon_addr_dma_0_update:8;
        /** log_mon_addr_dma_1_update : WT; bitpos: [15:8]; default: 0;
         *  Configures whether to update the monitored address space of the DMA_1 bus as the
         *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
         *  0: Not update
         *  1: Update
         *  Other values: Invalid
         */
        uint32_t log_mon_addr_dma_1_update:8;
        /** log_mon_addr_dma_2_update : WT; bitpos: [23:16]; default: 0;
         *  Configures whether to update the monitored address space of the DMA_2 bus as the
         *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
         *  0: Not update
         *  1: Update
         *  Other values: Invalid
         */
        uint32_t log_mon_addr_dma_2_update:8;
        /** log_mon_addr_dma_3_update : WT; bitpos: [31:24]; default: 0;
         *  Configures whether to update the monitored address space of the DMA_3 bus as the
         *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
         *  0: Not update
         *  1: Update
         *  Other values: Invalid
         */
        uint32_t log_mon_addr_dma_3_update:8;
    };
    uint32_t val;
} mem_monitor_log_mon_addr_update_1_reg_t;

/** Type of log_mem_start register
 *  Configures the starting address of the storage memory for recorded data
 */
typedef union {
    struct {
        /** log_mem_start : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting address of the storage space for recorded data.
         */
        uint32_t log_mem_start:32;
    };
    uint32_t val;
} mem_monitor_log_mem_start_reg_t;

/** Type of log_mem_end register
 *  Configures the end address of the storage memory for recorded data
 */
typedef union {
    struct {
        /** log_mem_end : R/W; bitpos: [31:0]; default: 0;
         *  Configures the ending address of the storage space for recorded data.
         */
        uint32_t log_mem_end:32;
    };
    uint32_t val;
} mem_monitor_log_mem_end_reg_t;

/** Type of log_mem_current_addr register
 *  Represents the address for the next write
 */
typedef union {
    struct {
        /** log_mem_current_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next write.
         */
        uint32_t log_mem_current_addr:32;
    };
    uint32_t val;
} mem_monitor_log_mem_current_addr_reg_t;

/** Type of log_mem_addr_update register
 *  Updates the address for the next write with the starting address for the recorded
 *  data
 */
typedef union {
    struct {
        /** log_mem_addr_update : WT; bitpos: [0]; default: 0;
         *  Configures whether to update the value in MEM_MONITOR_LOG_MEM_START_REG to the
         *  value of MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG.
         *  0: Not update
         *  1: Update
         */
        uint32_t log_mem_addr_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mem_monitor_log_mem_addr_update_reg_t;

/** Type of log_mem_full_flag register
 *  Represents logging buffer overflow status register
 */
typedef union {
    struct {
        /** log_mem_full_flag : RO; bitpos: [0]; default: 0;
         *  Represents whether data overflows the storage space.
         *  0: Not Overflow
         *  1: Overflow
         */
        uint32_t log_mem_full_flag:1;
        /** clr_log_mem_full_flag : WT; bitpos: [1]; default: 0;
         *  Configures whether to clear theMEM_MONITOR_LOG_MEM_FULL_FLAG flag bit.
         *  0: Not clear (default)
         *  1: Clear
         */
        uint32_t clr_log_mem_full_flag:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} mem_monitor_log_mem_full_flag_reg_t;


/** Group: Clock gating control register */
/** Type of clock_gate register
 *  Clock gating control register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable the register clock gating.
         *  0: Disable
         *  1: Enable
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mem_monitor_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36733248;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} mem_monitor_date_reg_t;


typedef struct {
    volatile mem_monitor_log_setting_reg_t log_setting;
    volatile mem_monitor_log_setting1_reg_t log_setting1;
    volatile mem_monitor_log_check_data_reg_t log_check_data;
    volatile mem_monitor_log_data_mask_reg_t log_data_mask;
    volatile mem_monitor_log_min_reg_t log_min;
    volatile mem_monitor_log_max_reg_t log_max;
    volatile mem_monitor_log_mon_addr_update_0_reg_t log_mon_addr_update_0;
    volatile mem_monitor_log_mon_addr_update_1_reg_t log_mon_addr_update_1;
    volatile mem_monitor_log_mem_start_reg_t log_mem_start;
    volatile mem_monitor_log_mem_end_reg_t log_mem_end;
    volatile mem_monitor_log_mem_current_addr_reg_t log_mem_current_addr;
    volatile mem_monitor_log_mem_addr_update_reg_t log_mem_addr_update;
    volatile mem_monitor_log_mem_full_flag_reg_t log_mem_full_flag;
    volatile mem_monitor_clock_gate_reg_t clock_gate;
    uint32_t reserved_038[241];
    volatile mem_monitor_date_reg_t date;
} mem_monitor_dev_t;

extern mem_monitor_dev_t TCM_MEM_MONITOR;

#ifndef __cplusplus
_Static_assert(sizeof(mem_monitor_dev_t) == 0x400, "Invalid size of mem_monitor_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
