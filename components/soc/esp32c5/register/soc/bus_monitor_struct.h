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

/** Group: monitor configuration registers */
/** Type of core_0_montr_ena register
 *  core0 monitor enable configuration register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_ena : R/W; bitpos: [0]; default: 0;
         *  Configures whether to monitor read operations in region 0 by the Data bus.
         *  0: Not monitor
         *  1: Monitor
         */
        uint32_t core_0_area_dram0_0_rd_ena:1;
        /** core_0_area_dram0_0_wr_ena : R/W; bitpos: [1]; default: 0;
         *  Configures whether to monitor write operations in region 0 by the Data bus.
         *  0: Not monitor
         *  1: Monitor
         */
        uint32_t core_0_area_dram0_0_wr_ena:1;
        /** core_0_area_dram0_1_rd_ena : R/W; bitpos: [2]; default: 0;
         *  Configures whether to monitor read operations in region 1 by the Data bus.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_area_dram0_1_rd_ena:1;
        /** core_0_area_dram0_1_wr_ena : R/W; bitpos: [3]; default: 0;
         *  Configures whether to monitor write operations in region 1 by the Data bus.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_area_dram0_1_wr_ena:1;
        /** core_0_area_pif_0_rd_ena : R/W; bitpos: [4]; default: 0;
         *  Configures whether to monitor read operations in region 0 by the Peripheral bus.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_area_pif_0_rd_ena:1;
        /** core_0_area_pif_0_wr_ena : R/W; bitpos: [5]; default: 0;
         *  Configures whether to monitor write operations in region 0 by the Peripheral bus.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_area_pif_0_wr_ena:1;
        /** core_0_area_pif_1_rd_ena : R/W; bitpos: [6]; default: 0;
         *  Configures whether to monitor read operations in region 1 by the Peripheral bus.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_area_pif_1_rd_ena:1;
        /** core_0_area_pif_1_wr_ena : R/W; bitpos: [7]; default: 0;
         *  Configures whether to monitor write operations in region 1 by the Peripheral bus.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_area_pif_1_wr_ena:1;
        /** core_0_sp_spill_min_ena : R/W; bitpos: [8]; default: 0;
         *  Configures whether to monitor SP exceeding the lower bound address of SP monitored
         *  region.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_sp_spill_min_ena:1;
        /** core_0_sp_spill_max_ena : R/W; bitpos: [9]; default: 0;
         *  Configures whether to monitor SP exceeding the upper bound address of SP monitored
         *  region.
         *  0: Not Monitor
         *  1: Monitor
         */
        uint32_t core_0_sp_spill_max_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} bus_monitor_core_0_montr_ena_reg_t;

/** Type of core_0_area_dram0_0_min register
 *  Configures lower boundary address of region 0 monitored on Data bus
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the lower bound address of Data bus region 0.
         */
        uint32_t core_0_area_dram0_0_min:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_dram0_0_min_reg_t;

/** Type of core_0_area_dram0_0_max register
 *  Configures upper boundary address of region 0 monitored on Data bus
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_max : R/W; bitpos: [31:0]; default: 0;
         *  Configures the upper bound address of Data bus region 0.
         */
        uint32_t core_0_area_dram0_0_max:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_dram0_0_max_reg_t;

/** Type of core_0_area_dram0_1_min register
 *  Configures lower boundary address of region 1 monitored on Data bus
 */
typedef union {
    struct {
        /** core_0_area_dram0_1_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the lower bound address of Data bus region 1.
         */
        uint32_t core_0_area_dram0_1_min:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_dram0_1_min_reg_t;

/** Type of core_0_area_dram0_1_max register
 *  Configures upper boundary address of region 1 monitored on Data bus
 */
typedef union {
    struct {
        /** core_0_area_dram0_1_max : R/W; bitpos: [31:0]; default: 0;
         *  Configures the upper bound address of Data bus region 1.
         */
        uint32_t core_0_area_dram0_1_max:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_dram0_1_max_reg_t;

/** Type of core_0_area_pif_0_min register
 *  Configures lower boundary address of region 0 monitored on Peripheral bus
 */
typedef union {
    struct {
        /** core_0_area_pif_0_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the lower bound address of Peripheral bus region 0.
         */
        uint32_t core_0_area_pif_0_min:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_pif_0_min_reg_t;

/** Type of core_0_area_pif_0_max register
 *  Configures upper boundary address of region 0 monitored on Peripheral bus
 */
typedef union {
    struct {
        /** core_0_area_pif_0_max : R/W; bitpos: [31:0]; default: 0;
         *  Configures the upper bound address of Peripheral bus region 0.
         */
        uint32_t core_0_area_pif_0_max:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_pif_0_max_reg_t;

/** Type of core_0_area_pif_1_min register
 *  Configures lower boundary address of region 1 monitored on Peripheral bus
 */
typedef union {
    struct {
        /** core_0_area_pif_1_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the lower bound address of Peripheral bus region 1.
         */
        uint32_t core_0_area_pif_1_min:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_pif_1_min_reg_t;

/** Type of core_0_area_pif_1_max register
 *  Configures upper boundary address of region 1 monitored on Peripheral bus
 */
typedef union {
    struct {
        /** core_0_area_pif_1_max : R/W; bitpos: [31:0]; default: 0;
         *  Configures the upper bound address of Peripheral bus region 1.
         */
        uint32_t core_0_area_pif_1_max:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_pif_1_max_reg_t;

/** Type of core_0_area_pc register
 *  Region monitoring HP CPU PC status register
 */
typedef union {
    struct {
        /** core_0_area_pc : RO; bitpos: [31:0]; default: 0;
         *  Represents the PC value when an interrupt is triggered during region monitoring.
         */
        uint32_t core_0_area_pc:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_pc_reg_t;

/** Type of core_0_area_sp register
 *  Region monitoring HP CPU SP status register
 */
typedef union {
    struct {
        /** core_0_area_sp : RO; bitpos: [31:0]; default: 0;
         *  Represents the SP value when an interrupt is triggered during region monitoring.
         */
        uint32_t core_0_area_sp:32;
    };
    uint32_t val;
} bus_monitor_core_0_area_sp_reg_t;

/** Type of core_0_sp_min register
 *  Configures stack monitoring lower boundary address
 */
typedef union {
    struct {
        /** core_0_sp_min : R/W; bitpos: [31:0]; default: 0;
         *  Configures the lower bound address of SP.
         */
        uint32_t core_0_sp_min:32;
    };
    uint32_t val;
} bus_monitor_core_0_sp_min_reg_t;

/** Type of core_0_sp_max register
 *  Configures stack monitoring upper boundary address
 */
typedef union {
    struct {
        /** core_0_sp_max : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the upper bound address of SP.
         */
        uint32_t core_0_sp_max:32;
    };
    uint32_t val;
} bus_monitor_core_0_sp_max_reg_t;

/** Type of core_0_sp_pc register
 *  Stack monitoring HP CPU PC status register
 */
typedef union {
    struct {
        /** core_0_sp_pc : RO; bitpos: [31:0]; default: 0;
         *  Represents the PC value during stack monitoring.
         */
        uint32_t core_0_sp_pc:32;
    };
    uint32_t val;
} bus_monitor_core_0_sp_pc_reg_t;


/** Group: interrupt configuration register */
/** Type of core_0_intr_raw register
 *  core0 monitor interrupt status register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_raw : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status of read operations in region 0 by Data bus.
         */
        uint32_t core_0_area_dram0_0_rd_raw:1;
        /** core_0_area_dram0_0_wr_raw : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status of write operations in region 0 by Data bus.
         */
        uint32_t core_0_area_dram0_0_wr_raw:1;
        /** core_0_area_dram0_1_rd_raw : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status of read operations in region 1 by Data bus.
         */
        uint32_t core_0_area_dram0_1_rd_raw:1;
        /** core_0_area_dram0_1_wr_raw : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status of write operations in region 1 by Data bus.
         */
        uint32_t core_0_area_dram0_1_wr_raw:1;
        /** core_0_area_pif_0_rd_raw : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status of read operations in region 0 by Peripheral bus.
         */
        uint32_t core_0_area_pif_0_rd_raw:1;
        /** core_0_area_pif_0_wr_raw : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status of write operations in region 0 by Peripheral bus.
         */
        uint32_t core_0_area_pif_0_wr_raw:1;
        /** core_0_area_pif_1_rd_raw : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status of read operations in region 1 by Peripheral bus.
         */
        uint32_t core_0_area_pif_1_rd_raw:1;
        /** core_0_area_pif_1_wr_raw : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status of write operations in region 1 by Peripheral bus.
         */
        uint32_t core_0_area_pif_1_wr_raw:1;
        /** core_0_sp_spill_min_raw : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status of SP exceeding the lower bound address of SP monitored
         *  region.
         */
        uint32_t core_0_sp_spill_min_raw:1;
        /** core_0_sp_spill_max_raw : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status of SP exceeding the upper bound address of SP monitored
         *  region.
         */
        uint32_t core_0_sp_spill_max_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} bus_monitor_core_0_intr_raw_reg_t;

/** Type of core_0_intr_ena register
 *  core0 monitor interrupt enable register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_intr_ena : R/W; bitpos: [0]; default: 0;
         *  Core0 dram0 area0 read monitor interrupt enable
         */
        uint32_t core_0_area_dram0_0_rd_intr_ena:1;
        /** core_0_area_dram0_0_wr_intr_ena : R/W; bitpos: [1]; default: 0;
         *  Core0 dram0 area0 write monitor interrupt enable
         */
        uint32_t core_0_area_dram0_0_wr_intr_ena:1;
        /** core_0_area_dram0_1_rd_intr_ena : R/W; bitpos: [2]; default: 0;
         *  Core0 dram0 area1 read monitor interrupt enable
         */
        uint32_t core_0_area_dram0_1_rd_intr_ena:1;
        /** core_0_area_dram0_1_wr_intr_ena : R/W; bitpos: [3]; default: 0;
         *  Core0 dram0 area1 write monitor interrupt enable
         */
        uint32_t core_0_area_dram0_1_wr_intr_ena:1;
        /** core_0_area_pif_0_rd_intr_ena : R/W; bitpos: [4]; default: 0;
         *  Core0 PIF area0 read monitor interrupt enable
         */
        uint32_t core_0_area_pif_0_rd_intr_ena:1;
        /** core_0_area_pif_0_wr_intr_ena : R/W; bitpos: [5]; default: 0;
         *  Core0 PIF area0 write monitor interrupt enable
         */
        uint32_t core_0_area_pif_0_wr_intr_ena:1;
        /** core_0_area_pif_1_rd_intr_ena : R/W; bitpos: [6]; default: 0;
         *  Core0 PIF area1 read monitor interrupt enable
         */
        uint32_t core_0_area_pif_1_rd_intr_ena:1;
        /** core_0_area_pif_1_wr_intr_ena : R/W; bitpos: [7]; default: 0;
         *  Core0 PIF area1 write monitor interrupt enable
         */
        uint32_t core_0_area_pif_1_wr_intr_ena:1;
        /** core_0_sp_spill_min_intr_ena : R/W; bitpos: [8]; default: 0;
         *  Core0 stackpoint underflow monitor interrupt enable
         */
        uint32_t core_0_sp_spill_min_intr_ena:1;
        /** core_0_sp_spill_max_intr_ena : R/W; bitpos: [9]; default: 0;
         *  Core0 stackpoint overflow monitor interrupt enable
         */
        uint32_t core_0_sp_spill_max_intr_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} bus_monitor_core_0_intr_ena_reg_t;

/** Type of core_0_intr_clr register
 *  core0 monitor interrupt clear register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear the interrupt for read operations in region 0 by Data bus.
         */
        uint32_t core_0_area_dram0_0_rd_clr:1;
        /** core_0_area_dram0_0_wr_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear the interrupt for write operations in region 0 by Data bus.
         */
        uint32_t core_0_area_dram0_0_wr_clr:1;
        /** core_0_area_dram0_1_rd_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear the interrupt for read operations in region 1 by Data bus.
         */
        uint32_t core_0_area_dram0_1_rd_clr:1;
        /** core_0_area_dram0_1_wr_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear the interrupt for write operations in region 1 by Data bus.
         */
        uint32_t core_0_area_dram0_1_wr_clr:1;
        /** core_0_area_pif_0_rd_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear the interrupt for read operations in region 0 by Peripheral bus.
         */
        uint32_t core_0_area_pif_0_rd_clr:1;
        /** core_0_area_pif_0_wr_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear the interrupt for write operations in region 0 by Peripheral bus.
         */
        uint32_t core_0_area_pif_0_wr_clr:1;
        /** core_0_area_pif_1_rd_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear the interrupt for read operations in region 1 by Peripheral bus.
         */
        uint32_t core_0_area_pif_1_rd_clr:1;
        /** core_0_area_pif_1_wr_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear the interrupt for write operations in region 1 by Peripheral bus.
         */
        uint32_t core_0_area_pif_1_wr_clr:1;
        /** core_0_sp_spill_min_clr : WT; bitpos: [8]; default: 0;
         *  Write 1 to clear the interrupt for SP exceeding the lower bound address of SP
         *  monitored region.
         */
        uint32_t core_0_sp_spill_min_clr:1;
        /** core_0_sp_spill_max_clr : WT; bitpos: [9]; default: 0;
         *  Write 1 to clear the interrupt for SP exceeding the upper bound address of SP
         *  monitored region.
         */
        uint32_t core_0_sp_spill_max_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} bus_monitor_core_0_intr_clr_reg_t;


/** Group: pc recording configuration register */
/** Type of core_0_rcd_en register
 *  HP CPU PC logging enable register
 */
typedef union {
    struct {
        /** core_0_rcd_recorden : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable PC logging.
         *  0: Disable
         *  1: ASSIST_DEBUG_CORE_0_RCD_PDEBUGPC_REG starts to record PC in real time
         */
        uint32_t core_0_rcd_recorden:1;
        /** core_0_rcd_pdebugen : R/W; bitpos: [1]; default: 0;
         *  Configures whether to enable HP CPU debugging.
         *  0: Disable
         *  1: HP CPU outputs PC
         */
        uint32_t core_0_rcd_pdebugen:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} bus_monitor_core_0_rcd_en_reg_t;


/** Group: pc recording status register */
/** Type of core_0_rcd_pdebugpc register
 *  PC logging register
 */
typedef union {
    struct {
        /** core_0_rcd_pdebugpc : RO; bitpos: [31:0]; default: 0;
         *  Represents the PC value at HP CPU reset.
         */
        uint32_t core_0_rcd_pdebugpc:32;
    };
    uint32_t val;
} bus_monitor_core_0_rcd_pdebugpc_reg_t;

/** Type of core_0_rcd_pdebugsp register
 *  PC logging register
 */
typedef union {
    struct {
        /** core_0_rcd_pdebugsp : RO; bitpos: [31:0]; default: 0;
         *  Represents SP.
         */
        uint32_t core_0_rcd_pdebugsp:32;
    };
    uint32_t val;
} bus_monitor_core_0_rcd_pdebugsp_reg_t;


/** Group: exception monitor register */
/** Type of core_x_iram0_dram0_exception_monitor_0 register
 *  exception monitor status register8
 */
typedef union {
    struct {
        /** core_x_iram0_dram0_limit_cycle_0 : R/W; bitpos: [19:0]; default: 0;
         *  reg_core_x_iram0_dram0_limit_cycle_0
         */
        uint32_t core_x_iram0_dram0_limit_cycle_0:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} bus_monitor_core_x_iram0_dram0_exception_monitor_0_reg_t;

/** Type of core_x_iram0_dram0_exception_monitor_1 register
 *  exception monitor status register9
 */
typedef union {
    struct {
        /** core_x_iram0_dram0_limit_cycle_1 : R/W; bitpos: [19:0]; default: 0;
         *  reg_core_x_iram0_dram0_limit_cycle_1
         */
        uint32_t core_x_iram0_dram0_limit_cycle_1:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} bus_monitor_core_x_iram0_dram0_exception_monitor_1_reg_t;


/** Group: cpu status registers */
/** Type of core_0_lastpc_before_exception register
 *  cpu status register
 */
typedef union {
    struct {
        /** core_0_lastpc_before_exc : RO; bitpos: [31:0]; default: 0;
         *  Represents the PC of the last command before the HP CPU enters exception.
         */
        uint32_t core_0_lastpc_before_exc:32;
    };
    uint32_t val;
} bus_monitor_core_0_lastpc_before_exception_reg_t;

/** Type of core_0_debug_mode register
 *  cpu status register
 */
typedef union {
    struct {
        /** core_0_debug_mode : RO; bitpos: [0]; default: 0;
         *  Represents whether RISC-V CPU (HP CPU) is in debugging mode.
         *  1: In debugging mode
         *  0: Not in debugging mode
         */
        uint32_t core_0_debug_mode:1;
        /** core_0_debug_module_active : RO; bitpos: [1]; default: 0;
         *  Represents the status of the RISC-V CPU (HP CPU) debug module.
         *  1: Active status
         *  Other: Inactive status
         */
        uint32_t core_0_debug_module_active:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} bus_monitor_core_0_debug_mode_reg_t;


/** Group: Configuration Registers */
/** Type of clock_gate register
 *  Register clock control
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to enable the register clock gating.
         *  0: Disable
         *  1: Enable
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} bus_monitor_clock_gate_reg_t;

/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 34640176;
         *  version register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} bus_monitor_date_reg_t;


typedef struct {
    volatile bus_monitor_core_0_montr_ena_reg_t core_0_montr_ena;
    volatile bus_monitor_core_0_intr_raw_reg_t core_0_intr_raw;
    volatile bus_monitor_core_0_intr_ena_reg_t core_0_intr_ena;
    volatile bus_monitor_core_0_intr_clr_reg_t core_0_intr_clr;
    volatile bus_monitor_core_0_area_dram0_0_min_reg_t core_0_area_dram0_0_min;
    volatile bus_monitor_core_0_area_dram0_0_max_reg_t core_0_area_dram0_0_max;
    volatile bus_monitor_core_0_area_dram0_1_min_reg_t core_0_area_dram0_1_min;
    volatile bus_monitor_core_0_area_dram0_1_max_reg_t core_0_area_dram0_1_max;
    volatile bus_monitor_core_0_area_pif_0_min_reg_t core_0_area_pif_0_min;
    volatile bus_monitor_core_0_area_pif_0_max_reg_t core_0_area_pif_0_max;
    volatile bus_monitor_core_0_area_pif_1_min_reg_t core_0_area_pif_1_min;
    volatile bus_monitor_core_0_area_pif_1_max_reg_t core_0_area_pif_1_max;
    volatile bus_monitor_core_0_area_pc_reg_t core_0_area_pc;
    volatile bus_monitor_core_0_area_sp_reg_t core_0_area_sp;
    volatile bus_monitor_core_0_sp_min_reg_t core_0_sp_min;
    volatile bus_monitor_core_0_sp_max_reg_t core_0_sp_max;
    volatile bus_monitor_core_0_sp_pc_reg_t core_0_sp_pc;
    volatile bus_monitor_core_0_rcd_en_reg_t core_0_rcd_en;
    volatile bus_monitor_core_0_rcd_pdebugpc_reg_t core_0_rcd_pdebugpc;
    volatile bus_monitor_core_0_rcd_pdebugsp_reg_t core_0_rcd_pdebugsp;
    uint32_t reserved_050[8];
    volatile bus_monitor_core_0_lastpc_before_exception_reg_t core_0_lastpc_before_exception;
    volatile bus_monitor_core_0_debug_mode_reg_t core_0_debug_mode;
    uint32_t reserved_078[34];
    volatile bus_monitor_core_x_iram0_dram0_exception_monitor_0_reg_t core_x_iram0_dram0_exception_monitor_0;
    volatile bus_monitor_core_x_iram0_dram0_exception_monitor_1_reg_t core_x_iram0_dram0_exception_monitor_1;
    volatile bus_monitor_clock_gate_reg_t clock_gate;
    uint32_t reserved_10c[188];
    volatile bus_monitor_date_reg_t date;
} bus_monitor_dev_t;

extern bus_monitor_dev_t BUS_MONITOR;

#ifndef __cplusplus
_Static_assert(sizeof(bus_monitor_dev_t) == 0x400, "Invalid size of bus_monitor_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
