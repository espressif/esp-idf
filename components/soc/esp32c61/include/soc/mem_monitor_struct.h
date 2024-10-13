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

/** Group: configuration registers */
/** Type of log_setting register
 *  Bus access logging configuration register
 */
typedef union {
    struct {
        /** log_mode : R/W; bitpos: [3:0]; default: 0;
         *  Configures monitoring modes.bit[0]: Configures write monitoring. \\
         *  0: Disable \\
         *  1: Enable\\
         *  bit[1]: Configures word monitoring. \\
         *  0: Disable \\
         *  1: Enable\\
         *  bit[2]: Configures halfword monitoring. \\
         *  0: Disable \\
         *  1: Enable\\
         *  bit[3]: Configures byte monitoring. \\
         *  0: Disable \\
         *  1: Enable\\
         */
        uint32_t log_mode:4;
        /** log_mem_loop_enable : R/W; bitpos: [4]; default: 1;
         *  Configures the writing mode for recorded data.1: Loop mode\\
         *  0: Non-loop mode\\
         */
        uint32_t log_mem_loop_enable:1;
        uint32_t reserved_5:3;
        /** log_core_ena : R/W; bitpos: [15:8]; default: 0;
         *  Configures whether to enable  CPU bus access logging.bit[0]: Configures whether to
         *  enable HP CPU bus access logging. \\
         *  0: Disable \\
         *  1: Enable\\
         *  Bit[7:1]: Reserved
         */
        uint32_t log_core_ena:8;
        /** log_dma_0_ena : R/W; bitpos: [23:16]; default: 0;
         *  Configures whether to enable  DMA_0 bus access logging.bit[0]: Configures whether
         *  to enable DMA_0 bus access logging. \\
         *  0: Disable \\
         *  1: Enable\\
         *  Bit[7:1]: Reserved
         */
        uint32_t log_dma_0_ena:8;
        /** log_dma_1_ena : R/W; bitpos: [31:24]; default: 0;
         *  Configures whether to enable  DMA_1 bus access logging.bit[0]: Configures whether
         *  to enable DMA_1 bus access logging. \\
         *  0: Disable \\
         *  1: Enable\\
         *  Bit[7:1]: Reserved
         */
        uint32_t log_dma_1_ena:8;
    };
    uint32_t val;
} mem_monitor_log_setting_reg_t;

/** Type of log_setting1 register
 *  Bus access logging configuration register
 */
typedef union {
    struct {
        /** log_dma_2_ena : R/W; bitpos: [7:0]; default: 0;
         *  Configures whether to enable  DMA_2 bus access logging.bit[0]: Configures whether
         *  to enable DMA_2 bus access logging. \\
         *  0: Disable \\
         *  1: Enable\\
         *  Bit[7:1]: Reserved
         */
        uint32_t log_dma_2_ena:8;
        /** log_dma_3_ena : R/W; bitpos: [15:8]; default: 0;
         *  Configures whether to enable  DMA_3 bus access logging.bit[0]: Configures whether
         *  to enable DMA_3 bus access logging. \\
         *  0: Disable \\
         *  1: Enable\\
         *  Bit[7:1]: Reserved
         */
        uint32_t log_dma_3_ena:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mem_monitor_log_setting1_reg_t;

/** Type of log_check_data register
 *  Configures monitored data in Bus access logging
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
 *  Configures masked data in Bus access logging
 */
typedef union {
    struct {
        /** log_data_mask : R/W; bitpos: [3:0]; default: 0;
         *  Configures which byte(s) in MEM_MONITOR_LOG_CHECK_DATA_REG to mask.bit[0]:
         *  Configures whether to mask the least significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG.\\
         *  0: Not mask \\
         *  1: Mask\\
         *  bit[1]: Configures whether to mask the second least significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG. \\
         *  0: Not mask \\
         *  1: Mask\\
         *  bit[2]: Configures whether to mask the second most significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG. \\
         *  0: Not mask \\
         *  1: Mask\\
         *  bit[3]: Configures whether to mask the most significant byte of
         *  MEM_MONITOR_LOG_CHECK_DATA_REG. \\
         *  0: Not mask \\
         *  1: Mask\\
         */
        uint32_t log_data_mask:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} mem_monitor_log_data_mask_reg_t;

/** Type of log_min register
 *  Configures monitored address space in Bus access logging
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
 *  Configures monitored address space in Bus access logging
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
 *  Configures the address space of from MEM_MONITOR_LOG_MIN_REG to
 *  MEM_MONITOR_LOG_MAX_REG as the monitored address space of the certain master.
 */
typedef union {
    struct {
        /** log_mon_addr_core_update : WT; bitpos: [7:0]; default: 0;
         *  Configures the monitored address space of the certain master. Bit[0]: Configures
         *  the address space of from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG as the
         *  monitored address space of the HP CPU bus.1: Update\\
         *  0: Not update\\
         *  Bit[7:1]: Reserved\\
         */
        uint32_t log_mon_addr_core_update:8;
        uint32_t reserved_8:23;
        /** log_mon_addr_all_update : WT; bitpos: [31]; default: 0;
         *  Configures the address space of from MEM_MONITOR_LOG_MIN_REG to
         *  MEM_MONITOR_LOG_MAX_REG as the monitored address space of all masters.1: Update\\
         *  0: Not update\\
         */
        uint32_t log_mon_addr_all_update:1;
    };
    uint32_t val;
} mem_monitor_log_mon_addr_update_0_reg_t;

/** Type of log_mon_addr_update_1 register
 *  Configures the address space of from MEM_MONITOR_LOG_MIN_REG to
 *  MEM_MONITOR_LOG_MAX_REG as the monitored address space of the certain master.
 */
typedef union {
    struct {
        /** log_mon_addr_dma_0_update : WT; bitpos: [7:0]; default: 0;
         *  Configures the monitored address space of the certain master. Bit[0]: Configures
         *  the address space of from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG as the
         *  monitored address space of the DMA_0 bus.1: Update\\
         *  0: Not update\\
         *  Bit[7:1]: Reserved\\
         */
        uint32_t log_mon_addr_dma_0_update:8;
        /** log_mon_addr_dma_1_update : WT; bitpos: [15:8]; default: 0;
         *  Configures the monitored address space of the certain master. Bit[0]: Configures
         *  the address space of from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG as the
         *  monitored address space of the DMA_1 bus.1: Update\\
         *  0: Not update\\
         *  Bit[7:1]: Reserved\\
         */
        uint32_t log_mon_addr_dma_1_update:8;
        /** log_mon_addr_dma_2_update : WT; bitpos: [23:16]; default: 0;
         *  Configures the monitored address space of the certain master. Bit[0]: Configures
         *  the address space of from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG as the
         *  monitored address space of the DMA_2 bus.1: Update\\
         *  0: Not update\\
         *  Bit[7:1]: Reserved\\
         */
        uint32_t log_mon_addr_dma_2_update:8;
        /** log_mon_addr_dma_3_update : WT; bitpos: [31:24]; default: 0;
         *  Configures the monitored address space of the certain master. Bit[0]: Configures
         *  the address space of from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG as the
         *  monitored address space of the DMA_3 bus.1: Update\\
         *  0: Not update\\
         *  Bit[7:1]: Reserved\\
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
         *  Configures whether to update the value in MEM_MONITOR_LOG_MEM_START_REG to
         *  MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG.\raggedright1: Update \\
         *  0: Not update (default) \\
         */
        uint32_t log_mem_addr_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mem_monitor_log_mem_addr_update_reg_t;

/** Type of log_mem_full_flag register
 *  Logging overflow status register
 */
typedef union {
    struct {
        /** log_mem_full_flag : RO; bitpos: [0]; default: 0;
         *  Represents whether data overflows the storage space.0: Not Overflow\\
         *  1: Overflow\\
         */
        uint32_t log_mem_full_flag:1;
        /** clr_log_mem_full_flag : WT; bitpos: [1]; default: 0;
         *  Configures whether to clear the  MEM_MONITOR_LOG_MEM_FULL_FLAG flag bit.0: Not
         *  clear\\
         *  1: Clear\\
         */
        uint32_t clr_log_mem_full_flag:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} mem_monitor_log_mem_full_flag_reg_t;


/** Group: clk register */
/** Type of clock_gate register
 *  Register clock control
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable the register clock gating.0: Disable\\
         *  1: Enable\\
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mem_monitor_clock_gate_reg_t;


/** Group: version register */
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
