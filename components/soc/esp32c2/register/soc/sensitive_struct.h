/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of rom_table_lock register
 *  register description
 */
typedef union {
    struct {
        /** rom_table_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t rom_table_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_rom_table_lock_reg_t;

/** Type of rom_table register
 *  register description
 */
typedef union {
    struct {
        /** rom_table : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t rom_table:32;
    };
    uint32_t val;
} sensitive_rom_table_reg_t;

/** Type of apb_peripheral_access_0 register
 *  register description
 */
typedef union {
    struct {
        /** apb_peripheral_access_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t apb_peripheral_access_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_apb_peripheral_access_0_reg_t;

/** Type of apb_peripheral_access_1 register
 *  register description
 */
typedef union {
    struct {
        /** apb_peripheral_access_split_burst : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t apb_peripheral_access_split_burst:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_apb_peripheral_access_1_reg_t;

/** Type of internal_sram_usage_0 register
 *  register description
 */
typedef union {
    struct {
        /** internal_sram_usage_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t internal_sram_usage_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_internal_sram_usage_0_reg_t;

/** Type of internal_sram_usage_1 register
 *  register description
 */
typedef union {
    struct {
        /** internal_sram_usage_cpu_cache : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t internal_sram_usage_cpu_cache:1;
        /** internal_sram_usage_cpu_sram : R/W; bitpos: [3:1]; default: 7;
         *  Need add description
         */
        uint32_t internal_sram_usage_cpu_sram:3;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sensitive_internal_sram_usage_1_reg_t;

/** Type of internal_sram_usage_3 register
 *  register description
 */
typedef union {
    struct {
        /** internal_sram_usage_mac_dump_sram : R/W; bitpos: [2:0]; default: 0;
         *  Need add description
         */
        uint32_t internal_sram_usage_mac_dump_sram:3;
        /** internal_sram_alloc_mac_dump : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t internal_sram_alloc_mac_dump:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sensitive_internal_sram_usage_3_reg_t;

/** Type of cache_tag_access_0 register
 *  register description
 */
typedef union {
    struct {
        /** cache_tag_access_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cache_tag_access_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_cache_tag_access_0_reg_t;

/** Type of cache_tag_access_1 register
 *  register description
 */
typedef union {
    struct {
        /** pro_i_tag_rd_acs : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t pro_i_tag_rd_acs:1;
        /** pro_i_tag_wr_acs : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t pro_i_tag_wr_acs:1;
        /** pro_d_tag_rd_acs : R/W; bitpos: [2]; default: 1;
         *  Need add description
         */
        uint32_t pro_d_tag_rd_acs:1;
        /** pro_d_tag_wr_acs : R/W; bitpos: [3]; default: 1;
         *  Need add description
         */
        uint32_t pro_d_tag_wr_acs:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sensitive_cache_tag_access_1_reg_t;

/** Type of cache_mmu_access_0 register
 *  register description
 */
typedef union {
    struct {
        /** cache_mmu_access_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cache_mmu_access_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_cache_mmu_access_0_reg_t;

/** Type of cache_mmu_access_1 register
 *  register description
 */
typedef union {
    struct {
        /** pro_mmu_rd_acs : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t pro_mmu_rd_acs:1;
        /** pro_mmu_wr_acs : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t pro_mmu_wr_acs:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_cache_mmu_access_1_reg_t;

/** Type of pif_access_monitor_0 register
 *  register description
 */
typedef union {
    struct {
        /** pif_access_monitor_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t pif_access_monitor_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_pif_access_monitor_0_reg_t;

/** Type of pif_access_monitor_1 register
 *  register description
 */
typedef union {
    struct {
        /** pif_access_monitor_nonword_violate_clr : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t pif_access_monitor_nonword_violate_clr:1;
        /** pif_access_monitor_nonword_violate_en : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t pif_access_monitor_nonword_violate_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_pif_access_monitor_1_reg_t;

/** Type of pif_access_monitor_2 register
 *  register description
 */
typedef union {
    struct {
        /** pif_access_monitor_nonword_violate_intr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t pif_access_monitor_nonword_violate_intr:1;
        /** pif_access_monitor_nonword_violate_status_hsize : RO; bitpos: [2:1]; default: 0;
         *  Need add description
         */
        uint32_t pif_access_monitor_nonword_violate_status_hsize:2;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} sensitive_pif_access_monitor_2_reg_t;

/** Type of pif_access_monitor_3 register
 *  register description
 */
typedef union {
    struct {
        /** pif_access_monitor_nonword_violate_status_haddr : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t pif_access_monitor_nonword_violate_status_haddr:32;
    };
    uint32_t val;
} sensitive_pif_access_monitor_3_reg_t;

/** Type of xts_aes_key_update register
 *  register description
 */
typedef union {
    struct {
        /** xts_aes_key_update : R/W; bitpos: [0]; default: 0;
         *  Set this bit to update xts_aes key
         */
        uint32_t xts_aes_key_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_xts_aes_key_update_reg_t;

/** Type of clock_gate register
 *  register description
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_clock_gate_reg_t;

/** Type of sensitive_reg_date register
 *  register description
 */
typedef union {
    struct {
        /** sensitive_reg_date : R/W; bitpos: [27:0]; default: 34628353;
         *  Need add description
         */
        uint32_t sensitive_reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sensitive_sensitive_reg_date_reg_t;


typedef struct {
    volatile sensitive_rom_table_lock_reg_t rom_table_lock;
    volatile sensitive_rom_table_reg_t rom_table;
    volatile sensitive_apb_peripheral_access_0_reg_t apb_peripheral_access_0;
    volatile sensitive_apb_peripheral_access_1_reg_t apb_peripheral_access_1;
    volatile sensitive_internal_sram_usage_0_reg_t internal_sram_usage_0;
    volatile sensitive_internal_sram_usage_1_reg_t internal_sram_usage_1;
    volatile sensitive_internal_sram_usage_3_reg_t internal_sram_usage_3;
    volatile sensitive_cache_tag_access_0_reg_t cache_tag_access_0;
    volatile sensitive_cache_tag_access_1_reg_t cache_tag_access_1;
    volatile sensitive_cache_mmu_access_0_reg_t cache_mmu_access_0;
    volatile sensitive_cache_mmu_access_1_reg_t cache_mmu_access_1;
    volatile sensitive_pif_access_monitor_0_reg_t pif_access_monitor_0;
    volatile sensitive_pif_access_monitor_1_reg_t pif_access_monitor_1;
    volatile sensitive_pif_access_monitor_2_reg_t pif_access_monitor_2;
    volatile sensitive_pif_access_monitor_3_reg_t pif_access_monitor_3;
    volatile sensitive_xts_aes_key_update_reg_t xts_aes_key_update;
    volatile sensitive_clock_gate_reg_t clock_gate;
    uint32_t reserved_044[1006];
    volatile sensitive_sensitive_reg_date_reg_t sensitive_reg_date;
} sensitive_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(sensitive_dev_t) == 0x1000, "Invalid size of sensitive_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
