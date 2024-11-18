/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_ASSIST_DEBUG_STRUCT_H_
#define _SOC_ASSIST_DEBUG_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct assist_debug_dev_s {
    union {
        struct {
            uint32_t core_0_area_dram0_0_rd        :    1;
            uint32_t core_0_area_dram0_0_wr        :    1;
            uint32_t core_0_area_dram0_1_rd        :    1;
            uint32_t core_0_area_dram0_1_wr        :    1;
            uint32_t core_0_area_pif_0_rd          :    1;
            uint32_t core_0_area_pif_0_wr          :    1;
            uint32_t core_0_area_pif_1_rd          :    1;
            uint32_t core_0_area_pif_1_wr          :    1;
            uint32_t core_0_sp_spill_min           :    1;
            uint32_t core_0_sp_spill_max           :    1;
            uint32_t core_0_iram0_exception_monitor:    1;
            uint32_t core_0_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_0_interrupt_ena;
    union {
        struct {
            uint32_t core_0_area_dram0_0_rd        :    1;
            uint32_t core_0_area_dram0_0_wr        :    1;
            uint32_t core_0_area_dram0_1_rd        :    1;
            uint32_t core_0_area_dram0_1_wr        :    1;
            uint32_t core_0_area_pif_0_rd          :    1;
            uint32_t core_0_area_pif_0_wr          :    1;
            uint32_t core_0_area_pif_1_rd          :    1;
            uint32_t core_0_area_pif_1_wr          :    1;
            uint32_t core_0_sp_spill_min           :    1;
            uint32_t core_0_sp_spill_max           :    1;
            uint32_t core_0_iram0_exception_monitor:    1;
            uint32_t core_0_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_0_interrupt_raw;
    union {
        struct {
            uint32_t core_0_area_dram0_0_rd        :    1;
            uint32_t core_0_area_dram0_0_wr        :    1;
            uint32_t core_0_area_dram0_1_rd        :    1;
            uint32_t core_0_area_dram0_1_wr        :    1;
            uint32_t core_0_area_pif_0_rd          :    1;
            uint32_t core_0_area_pif_0_wr          :    1;
            uint32_t core_0_area_pif_1_rd          :    1;
            uint32_t core_0_area_pif_1_wr          :    1;
            uint32_t core_0_sp_spill_min           :    1;
            uint32_t core_0_sp_spill_max           :    1;
            uint32_t core_0_iram0_exception_monitor:    1;
            uint32_t core_0_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_0_interrupt_rls;
    union {
        struct {
            uint32_t core_0_area_dram0_0_rd        :    1;
            uint32_t core_0_area_dram0_0_wr        :    1;
            uint32_t core_0_area_dram0_1_rd        :    1;
            uint32_t core_0_area_dram0_1_wr        :    1;
            uint32_t core_0_area_pif_0_rd          :    1;
            uint32_t core_0_area_pif_0_wr          :    1;
            uint32_t core_0_area_pif_1_rd          :    1;
            uint32_t core_0_area_pif_1_wr          :    1;
            uint32_t core_0_sp_spill_min           :    1;
            uint32_t core_0_sp_spill_max           :    1;
            uint32_t core_0_iram0_exception_monitor:    1;
            uint32_t core_0_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_0_interrupt_clr;
    uint32_t core_0_area_dram0_0_min;
    uint32_t core_0_area_dram0_0_max;
    uint32_t core_0_area_dram0_1_min;
    uint32_t core_0_area_dram0_1_max;
    uint32_t core_0_area_pif_0_min;
    uint32_t core_0_area_pif_0_max;
    uint32_t core_0_area_pif_1_min;
    uint32_t core_0_area_pif_1_max;
    uint32_t core_0_area_sp;
    uint32_t core_0_area_pc;
    union {
        struct {
            uint32_t core_0_sp_unstable            :    8;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } core_0_sp_unstable;
    uint32_t core_0_sp_min;
    uint32_t core_0_sp_max;
    uint32_t core_0_sp_pc;
    union {
        struct {
            uint32_t core_0_rcd_pdebugenable       :    1;
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } core_0_rcd_pdebugenable;
    union {
        struct {
            uint32_t core_0_rcd_recording          :    1;
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } core_0_rcd_recording;
    uint32_t core_0_rcd_pdebuginst;
    union {
        struct {
            uint32_t core_0_rcd_pdebugstatus       :    8;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } core_0_rcd_pdebugstatus;
    uint32_t core_0_rcd_pdebugdata;
    uint32_t core_0_rcd_pdebugpc;
    uint32_t core_0_rcd_pdebugls0stat;
    uint32_t core_0_rcd_pdebugls0addr;
    uint32_t core_0_rcd_pdebugls0data;
    uint32_t core_0_rcd_sp;
    union {
        struct {
            uint32_t core_0_iram0_recording_addr_0 :    24;
            uint32_t core_0_iram0_recording_wr_0   :    1;
            uint32_t core_0_iram0_recording_loadstore_0:    1;
            uint32_t reserved26                    :    6;
        };
        uint32_t val;
    } core_0_iram0_exception_monitor_0;
    union {
        struct {
            uint32_t core_0_iram0_recording_addr_1 :    24;
            uint32_t core_0_iram0_recording_wr_1   :    1;
            uint32_t core_0_iram0_recording_loadstore_1:    1;
            uint32_t reserved26                    :    6;
        };
        uint32_t val;
    } core_0_iram0_exception_monitor_1;
    union {
        struct {
            uint32_t core_0_dram0_recording_addr_0 :    22;
            uint32_t core_0_dram0_recording_wr_0   :    1;
            uint32_t reserved23                    :    9;
        };
        uint32_t val;
    } core_0_dram0_exception_monitor_0;
    union {
        struct {
            uint32_t core_0_dram0_recording_byteen_0:    16;
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } core_0_dram0_exception_monitor_1;
    uint32_t core_0_dram0_exception_monitor_2;
    union {
        struct {
            uint32_t core_0_dram0_recording_addr_1 :    22;
            uint32_t core_0_dram0_recording_wr_1   :    1;
            uint32_t reserved23                    :    9;
        };
        uint32_t val;
    } core_0_dram0_exception_monitor_3;
    union {
        struct {
            uint32_t core_0_dram0_recording_byteen_1:    16;
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } core_0_dram0_exception_monitor_4;
    uint32_t core_0_dram0_exception_monitor_5;
    union {
        struct {
            uint32_t core_1_area_dram0_0_rd        :    1;
            uint32_t core_1_area_dram0_0_wr        :    1;
            uint32_t core_1_area_dram0_1_rd        :    1;
            uint32_t core_1_area_dram0_1_wr        :    1;
            uint32_t core_1_area_pif_0_rd          :    1;
            uint32_t core_1_area_pif_0_wr          :    1;
            uint32_t core_1_area_pif_1_rd          :    1;
            uint32_t core_1_area_pif_1_wr          :    1;
            uint32_t core_1_sp_spill_min           :    1;
            uint32_t core_1_sp_spill_max           :    1;
            uint32_t core_1_iram0_exception_monitor:    1;
            uint32_t core_1_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_1_interrupt_ena;
    union {
        struct {
            uint32_t core_1_area_dram0_0_rd        :    1;
            uint32_t core_1_area_dram0_0_wr        :    1;
            uint32_t core_1_area_dram0_1_rd        :    1;
            uint32_t core_1_area_dram0_1_wr        :    1;
            uint32_t core_1_area_pif_0_rd          :    1;
            uint32_t core_1_area_pif_0_wr          :    1;
            uint32_t core_1_area_pif_1_rd          :    1;
            uint32_t core_1_area_pif_1_wr          :    1;
            uint32_t core_1_sp_spill_min           :    1;
            uint32_t core_1_sp_spill_max           :    1;
            uint32_t core_1_iram0_exception_monitor:    1;
            uint32_t core_1_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_1_interrupt_raw;
    union {
        struct {
            uint32_t core_1_area_dram0_0_rd        :    1;
            uint32_t core_1_area_dram0_0_wr        :    1;
            uint32_t core_1_area_dram0_1_rd        :    1;
            uint32_t core_1_area_dram0_1_wr        :    1;
            uint32_t core_1_area_pif_0_rd          :    1;
            uint32_t core_1_area_pif_0_wr          :    1;
            uint32_t core_1_area_pif_1_rd          :    1;
            uint32_t core_1_area_pif_1_wr          :    1;
            uint32_t core_1_sp_spill_min           :    1;
            uint32_t core_1_sp_spill_max           :    1;
            uint32_t core_1_iram0_exception_monitor:    1;
            uint32_t core_1_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_1_interrupt_rls;
    union {
        struct {
            uint32_t core_1_area_dram0_0_rd        :    1;
            uint32_t core_1_area_dram0_0_wr        :    1;
            uint32_t core_1_area_dram0_1_rd        :    1;
            uint32_t core_1_area_dram0_1_wr        :    1;
            uint32_t core_1_area_pif_0_rd          :    1;
            uint32_t core_1_area_pif_0_wr          :    1;
            uint32_t core_1_area_pif_1_rd          :    1;
            uint32_t core_1_area_pif_1_wr          :    1;
            uint32_t core_1_sp_spill_min           :    1;
            uint32_t core_1_sp_spill_max           :    1;
            uint32_t core_1_iram0_exception_monitor:    1;
            uint32_t core_1_dram0_exception_monitor:    1;
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } core_1_interrupt_clr;
    uint32_t core_1_area_dram0_0_min;
    uint32_t core_1_area_dram0_0_max;
    uint32_t core_1_area_dram0_1_min;
    uint32_t core_1_area_dram0_1_max;
    uint32_t core_1_area_pif_0_min;
    uint32_t core_1_area_pif_0_max;
    uint32_t core_1_area_pif_1_min;
    uint32_t core_1_area_pif_1_max;
    uint32_t core_1_area_pc;
    uint32_t core_1_area_sp;
    union {
        struct {
            uint32_t core_1_sp_unstable            :    8;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } core_1_sp_unstable;
    uint32_t core_1_sp_min;
    uint32_t core_1_sp_max;
    uint32_t core_1_sp_pc;
    union {
        struct {
            uint32_t core_1_rcd_pdebugenable       :    1;
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } core_1_rcd_pdebugenable;
    union {
        struct {
            uint32_t core_1_rcd_recording          :    1;
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } core_1_rcd_recording;
    uint32_t core_1_rcd_pdebuginst;
    union {
        struct {
            uint32_t core_1_rcd_pdebugstatus       :    8;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } core_1_rcd_pdebugstatus;
    uint32_t core_1_rcd_pdebugdata;
    uint32_t core_1_rcd_pdebugpc;
    uint32_t core_1_rcd_pdebugls0stat;
    uint32_t core_1_rcd_pdebugls0addr;
    uint32_t core_1_rcd_pdebugls0data;
    uint32_t core_1_rcd_sp;
    union {
        struct {
            uint32_t core_1_iram0_recording_addr_0 :    24;
            uint32_t core_1_iram0_recording_wr_0   :    1;
            uint32_t core_1_iram0_recording_loadstore_0:    1;
            uint32_t reserved26                    :    6;
        };
        uint32_t val;
    } core_1_iram0_exception_monitor_0;
    union {
        struct {
            uint32_t core_1_iram0_recording_addr_1 :    24;
            uint32_t core_1_iram0_recording_wr_1   :    1;
            uint32_t core_1_iram0_recording_loadstore_1:    1;
            uint32_t reserved26                    :    6;
        };
        uint32_t val;
    } core_1_iram0_exception_monitor_1;
    union {
        struct {
            uint32_t core_1_dram0_recording_addr_0 :    22;
            uint32_t core_1_dram0_recording_wr_0   :    1;
            uint32_t reserved23                    :    9;
        };
        uint32_t val;
    } core_1_dram0_exception_monitor_0;
    union {
        struct {
            uint32_t core_1_dram0_recording_byteen_0:    16;
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } core_1_dram0_exception_monitor_1;
    uint32_t core_1_dram0_exception_monitor_2;
    union {
        struct {
            uint32_t core_1_dram0_recording_addr_1 :    22;
            uint32_t core_1_dram0_recording_wr_1   :    1;
            uint32_t reserved23                    :    9;
        };
        uint32_t val;
    } core_1_dram0_exception_monitor_3;
    union {
        struct {
            uint32_t core_1_dram0_recording_byteen_1:    16;
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } core_1_dram0_exception_monitor_4;
    uint32_t core_1_dram0_exception_monitor_5;
    union {
        struct {
            uint32_t core_x_iram0_dram0_limit_cycle_0:    20;
            uint32_t reserved20                    :    12;
        };
        uint32_t val;
    } core_x_iram0_dram0_exception_monitor_0;
    union {
        struct {
            uint32_t core_x_iram0_dram0_limit_cycle_1:    20;
            uint32_t reserved20                    :    12;
        };
        uint32_t val;
    } core_x_iram0_dram0_exception_monitor_1;
    union {
        struct {
            uint32_t log                           :    3;
            uint32_t log_mode                      :    3;
            uint32_t log_mem_loopble               :    1;
            uint32_t reserved7                     :    25;
        };
        uint32_t val;
    } log_setting;
    uint32_t log_data_0;
    uint32_t log_data_1;
    uint32_t log_data_2;
    uint32_t log_data_3;
    union {
        struct {
            uint32_t log_data_size                 :    16;
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } log_data_mask;
    uint32_t log_min;
    uint32_t log_max;
    uint32_t log_mem_start;
    uint32_t log_mem_end;
    uint32_t log_mem_writing_addr;
    union {
        struct {
            uint32_t log_mem_full_flag             :    1;
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } log_mem_full_flag;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    union {
        struct {
            uint32_t assist_debug_reg_date         :    28;
            uint32_t reserved28                    :    4;
        };
        uint32_t val;
    } reg_date;
} assist_debug_dev_t;
extern assist_debug_dev_t ASSIST_DEBUG;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_ASSIST_DEBUG_STRUCT_H_ */
