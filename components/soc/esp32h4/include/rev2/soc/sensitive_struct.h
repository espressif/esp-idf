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

/** Type of privilege_mode_sel_lock register
 *  register description
 */
typedef union {
    struct {
        /** privilege_mode_sel_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t privilege_mode_sel_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_privilege_mode_sel_lock_reg_t;

/** Type of privilege_mode_sel register
 *  register description
 */
typedef union {
    struct {
        /** privilege_mode_sel : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t privilege_mode_sel:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_privilege_mode_sel_reg_t;

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

/** Type of internal_sram_usage_4 register
 *  register description
 */
typedef union {
    struct {
        /** internal_sram_usage_log_sram : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t internal_sram_usage_log_sram:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_internal_sram_usage_4_reg_t;

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

/** Type of dma_apbperi_spi2_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_spi2_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_spi2_pms_constrain_0_reg_t;

/** Type of dma_apbperi_spi2_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_spi2_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_spi2_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_spi2_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_spi2_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_spi2_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_spi2_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_spi2_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_spi2_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_spi2_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_spi2_pms_constrain_1_reg_t;

/** Type of dma_apbperi_uchi0_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_uchi0_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_uchi0_pms_constrain_0_reg_t;

/** Type of dma_apbperi_uchi0_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_uchi0_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_uchi0_pms_constrain_1_reg_t;

/** Type of dma_apbperi_i2s0_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_i2s0_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_i2s0_pms_constrain_0_reg_t;

/** Type of dma_apbperi_i2s0_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_i2s0_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_i2s0_pms_constrain_1_reg_t;

/** Type of dma_apbperi_mac_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_mac_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_mac_pms_constrain_0_reg_t;

/** Type of dma_apbperi_mac_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_mac_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_mac_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_mac_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_mac_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_mac_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_mac_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_mac_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_mac_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_mac_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_mac_pms_constrain_1_reg_t;

/** Type of dma_apbperi_backup_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_backup_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_backup_pms_constrain_0_reg_t;

/** Type of dma_apbperi_backup_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_backup_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_backup_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_backup_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_backup_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_backup_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_backup_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_backup_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_backup_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_backup_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_backup_pms_constrain_1_reg_t;

/** Type of dma_apbperi_lc_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_lc_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_lc_pms_constrain_0_reg_t;

/** Type of dma_apbperi_lc_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_lc_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_lc_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_lc_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_lc_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_lc_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_lc_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_lc_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_lc_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_lc_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_lc_pms_constrain_1_reg_t;

/** Type of dma_apbperi_aes_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_aes_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_aes_pms_constrain_0_reg_t;

/** Type of dma_apbperi_aes_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_aes_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_aes_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_aes_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_aes_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_aes_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_aes_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_aes_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_aes_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_aes_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_aes_pms_constrain_1_reg_t;

/** Type of dma_apbperi_sha_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_sha_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_sha_pms_constrain_0_reg_t;

/** Type of dma_apbperi_sha_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_sha_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_sha_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_sha_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_sha_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_sha_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_sha_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_sha_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_sha_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sha_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_sha_pms_constrain_1_reg_t;

/** Type of dma_apbperi_adc_dac_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_adc_dac_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_adc_dac_pms_constrain_0_reg_t;

/** Type of dma_apbperi_adc_dac_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_adc_dac_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_adc_dac_pms_constrain_1_reg_t;

/** Type of dma_apbperi_ble_sec_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_ble_sec_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_ble_sec_pms_constrain_0_reg_t;

/** Type of dma_apbperi_ble_sec_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default:
         *  3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_ble_sec_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_ble_sec_pms_constrain_1_reg_t;

/** Type of dma_apbperi_white_list_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_white_list_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_white_list_pms_constrain_0_reg_t;

/** Type of dma_apbperi_white_list_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_white_list_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_white_list_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_white_list_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_white_list_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_white_list_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_white_list_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_white_list_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_white_list_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_white_list_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_white_list_pms_constrain_1_reg_t;

/** Type of dma_apbperi_sdio_host_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_sdio_host_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_sdio_host_pms_constrain_0_reg_t;

/** Type of dma_apbperi_sdio_host_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_0:2;
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_1:2;
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_2:2;
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_0:2;
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_1:2;
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_2:2;
        /** dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18];
         *  default: 3;
         *  Need add description
         */
        uint32_t dma_apbperi_sdio_host_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_dma_apbperi_sdio_host_pms_constrain_1_reg_t;

/** Type of dma_apbperi_pms_monitor_0 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_pms_monitor_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_dma_apbperi_pms_monitor_0_reg_t;

/** Type of dma_apbperi_pms_monitor_1 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_pms_monitor_violate_clr : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_violate_clr:1;
        /** dma_apbperi_pms_monitor_violate_en : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_violate_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_dma_apbperi_pms_monitor_1_reg_t;

/** Type of dma_apbperi_pms_monitor_2 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_pms_monitor_violate_intr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_violate_intr:1;
        /** dma_apbperi_pms_monitor_violate_status_world : RO; bitpos: [2:1]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_violate_status_world:2;
        /** dma_apbperi_pms_monitor_violate_status_addr : RO; bitpos: [26:3]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_violate_status_addr:24;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} sensitive_dma_apbperi_pms_monitor_2_reg_t;

/** Type of dma_apbperi_pms_monitor_3 register
 *  register description
 */
typedef union {
    struct {
        /** dma_apbperi_pms_monitor_violate_status_wr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_violate_status_wr:1;
        /** dma_apbperi_pms_monitor_violate_status_byteen : RO; bitpos: [4:1]; default: 0;
         *  Need add description
         */
        uint32_t dma_apbperi_pms_monitor_violate_status_byteen:4;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} sensitive_dma_apbperi_pms_monitor_3_reg_t;

/** Type of core_x_iram0_dram0_dma_split_line_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_iram0_dram0_dma_split_line_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_dram0_dma_split_line_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_core_x_iram0_dram0_dma_split_line_constrain_0_reg_t;

/** Type of core_x_iram0_dram0_dma_split_line_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_iram0_dram0_dma_sram_category_0 : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_dram0_dma_sram_category_0:2;
        /** core_x_iram0_dram0_dma_sram_category_1 : R/W; bitpos: [3:2]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_dram0_dma_sram_category_1:2;
        /** core_x_iram0_dram0_dma_sram_category_2 : R/W; bitpos: [5:4]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_dram0_dma_sram_category_2:2;
        uint32_t reserved_6:8;
        /** core_x_iram0_dram0_dma_sram_splitaddr : R/W; bitpos: [21:14]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_dram0_dma_sram_splitaddr:8;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} sensitive_core_x_iram0_dram0_dma_split_line_constrain_1_reg_t;

/** Type of core_x_iram0_dram0_dma_split_line_constrain_2 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_iram0_sram_line_0_category_0 : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_0_category_0:2;
        /** core_x_iram0_sram_line_0_category_1 : R/W; bitpos: [3:2]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_0_category_1:2;
        /** core_x_iram0_sram_line_0_category_2 : R/W; bitpos: [5:4]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_0_category_2:2;
        uint32_t reserved_6:8;
        /** core_x_iram0_sram_line_0_splitaddr : R/W; bitpos: [21:14]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_0_splitaddr:8;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} sensitive_core_x_iram0_dram0_dma_split_line_constrain_2_reg_t;

/** Type of core_x_iram0_dram0_dma_split_line_constrain_3 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_iram0_sram_line_1_category_0 : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_1_category_0:2;
        /** core_x_iram0_sram_line_1_category_1 : R/W; bitpos: [3:2]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_1_category_1:2;
        /** core_x_iram0_sram_line_1_category_2 : R/W; bitpos: [5:4]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_1_category_2:2;
        uint32_t reserved_6:8;
        /** core_x_iram0_sram_line_1_splitaddr : R/W; bitpos: [21:14]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_sram_line_1_splitaddr:8;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} sensitive_core_x_iram0_dram0_dma_split_line_constrain_3_reg_t;

/** Type of core_x_iram0_dram0_dma_split_line_constrain_4 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_dram0_dma_sram_line_0_category_0 : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_0_category_0:2;
        /** core_x_dram0_dma_sram_line_0_category_1 : R/W; bitpos: [3:2]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_0_category_1:2;
        /** core_x_dram0_dma_sram_line_0_category_2 : R/W; bitpos: [5:4]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_0_category_2:2;
        uint32_t reserved_6:8;
        /** core_x_dram0_dma_sram_line_0_splitaddr : R/W; bitpos: [21:14]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_0_splitaddr:8;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} sensitive_core_x_iram0_dram0_dma_split_line_constrain_4_reg_t;

/** Type of core_x_iram0_dram0_dma_split_line_constrain_5 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_dram0_dma_sram_line_1_category_0 : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_1_category_0:2;
        /** core_x_dram0_dma_sram_line_1_category_1 : R/W; bitpos: [3:2]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_1_category_1:2;
        /** core_x_dram0_dma_sram_line_1_category_2 : R/W; bitpos: [5:4]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_1_category_2:2;
        uint32_t reserved_6:8;
        /** core_x_dram0_dma_sram_line_1_splitaddr : R/W; bitpos: [21:14]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_dma_sram_line_1_splitaddr:8;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} sensitive_core_x_iram0_dram0_dma_split_line_constrain_5_reg_t;

/** Type of core_x_iram0_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_iram0_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_core_x_iram0_pms_constrain_0_reg_t;

/** Type of core_x_iram0_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_iram0_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [2:0]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_1_pms_0:3;
        /** core_x_iram0_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [5:3]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_1_pms_1:3;
        /** core_x_iram0_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [8:6]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_1_pms_2:3;
        /** core_x_iram0_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [11:9]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_1_pms_3:3;
        /** core_x_iram0_pms_constrain_sram_world_1_cachedataarray_pms_0 : R/W; bitpos:
         *  [14:12]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_1_cachedataarray_pms_0:3;
        uint32_t reserved_15:3;
        /** core_x_iram0_pms_constrain_rom_world_1_pms : R/W; bitpos: [20:18]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_rom_world_1_pms:3;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} sensitive_core_x_iram0_pms_constrain_1_reg_t;

/** Type of core_x_iram0_pms_constrain_2 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_iram0_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [2:0]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_0_pms_0:3;
        /** core_x_iram0_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [5:3]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_0_pms_1:3;
        /** core_x_iram0_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [8:6]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_0_pms_2:3;
        /** core_x_iram0_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [11:9]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_0_pms_3:3;
        /** core_x_iram0_pms_constrain_sram_world_0_cachedataarray_pms_0 : R/W; bitpos:
         *  [14:12]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_sram_world_0_cachedataarray_pms_0:3;
        uint32_t reserved_15:3;
        /** core_x_iram0_pms_constrain_rom_world_0_pms : R/W; bitpos: [20:18]; default: 7;
         *  Need add description
         */
        uint32_t core_x_iram0_pms_constrain_rom_world_0_pms:3;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} sensitive_core_x_iram0_pms_constrain_2_reg_t;

/** Type of core_0_iram0_pms_monitor_0 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_iram0_pms_monitor_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_core_0_iram0_pms_monitor_0_reg_t;

/** Type of core_0_iram0_pms_monitor_1 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_iram0_pms_monitor_violate_clr : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_violate_clr:1;
        /** core_0_iram0_pms_monitor_violate_en : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_violate_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_core_0_iram0_pms_monitor_1_reg_t;

/** Type of core_0_iram0_pms_monitor_2 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_iram0_pms_monitor_violate_intr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_violate_intr:1;
        /** core_0_iram0_pms_monitor_violate_status_wr : RO; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_violate_status_wr:1;
        /** core_0_iram0_pms_monitor_violate_status_loadstore : RO; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_violate_status_loadstore:1;
        /** core_0_iram0_pms_monitor_violate_status_world : RO; bitpos: [4:3]; default: 0;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_violate_status_world:2;
        /** core_0_iram0_pms_monitor_violate_status_addr : RO; bitpos: [28:5]; default: 0;
         *  Need add description
         */
        uint32_t core_0_iram0_pms_monitor_violate_status_addr:24;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sensitive_core_0_iram0_pms_monitor_2_reg_t;

/** Type of core_x_dram0_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_dram0_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_core_x_dram0_pms_constrain_0_reg_t;

/** Type of core_x_dram0_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** core_x_dram0_pms_constrain_sram_world_0_pms_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_0_pms_0:2;
        /** core_x_dram0_pms_constrain_sram_world_0_pms_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_0_pms_1:2;
        /** core_x_dram0_pms_constrain_sram_world_0_pms_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_0_pms_2:2;
        /** core_x_dram0_pms_constrain_sram_world_0_pms_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_0_pms_3:2;
        uint32_t reserved_8:4;
        /** core_x_dram0_pms_constrain_sram_world_1_pms_0 : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_1_pms_0:2;
        /** core_x_dram0_pms_constrain_sram_world_1_pms_1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_1_pms_1:2;
        /** core_x_dram0_pms_constrain_sram_world_1_pms_2 : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_1_pms_2:2;
        /** core_x_dram0_pms_constrain_sram_world_1_pms_3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_sram_world_1_pms_3:2;
        uint32_t reserved_20:4;
        /** core_x_dram0_pms_constrain_rom_world_0_pms : R/W; bitpos: [25:24]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_rom_world_0_pms:2;
        /** core_x_dram0_pms_constrain_rom_world_1_pms : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t core_x_dram0_pms_constrain_rom_world_1_pms:2;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sensitive_core_x_dram0_pms_constrain_1_reg_t;

/** Type of core_0_dram0_pms_monitor_0 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_dram0_pms_monitor_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_core_0_dram0_pms_monitor_0_reg_t;

/** Type of core_0_dram0_pms_monitor_1 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_dram0_pms_monitor_violate_clr : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_clr:1;
        /** core_0_dram0_pms_monitor_violate_en : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_core_0_dram0_pms_monitor_1_reg_t;

/** Type of core_0_dram0_pms_monitor_2 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_dram0_pms_monitor_violate_intr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_intr:1;
        /** core_0_dram0_pms_monitor_violate_status_lock : RO; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_status_lock:1;
        /** core_0_dram0_pms_monitor_violate_status_world : RO; bitpos: [3:2]; default: 0;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_status_world:2;
        /** core_0_dram0_pms_monitor_violate_status_addr : RO; bitpos: [27:4]; default: 0;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_status_addr:24;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sensitive_core_0_dram0_pms_monitor_2_reg_t;

/** Type of core_0_dram0_pms_monitor_3 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_dram0_pms_monitor_violate_status_wr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_status_wr:1;
        /** core_0_dram0_pms_monitor_violate_status_byteen : RO; bitpos: [4:1]; default: 0;
         *  Need add description
         */
        uint32_t core_0_dram0_pms_monitor_violate_status_byteen:4;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} sensitive_core_0_dram0_pms_monitor_3_reg_t;

/** Type of core_0_pif_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_0_reg_t;

/** Type of core_0_pif_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_0_uart : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_uart:2;
        /** core_0_pif_pms_constrain_world_0_g0spi_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_g0spi_1:2;
        /** core_0_pif_pms_constrain_world_0_g0spi_0 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_g0spi_0:2;
        /** core_0_pif_pms_constrain_world_0_gpio : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_gpio:2;
        uint32_t reserved_8:2;
        /** core_0_pif_pms_constrain_world_0_fe : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_fe:2;
        uint32_t reserved_12:2;
        /** core_0_pif_pms_constrain_world_0_rtc : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_rtc:2;
        /** core_0_pif_pms_constrain_world_0_io_mux : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_io_mux:2;
        /** core_0_pif_pms_constrain_world_0_wdg : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_wdg:2;
        uint32_t reserved_20:4;
        /** core_0_pif_pms_constrain_world_0_misc : R/W; bitpos: [25:24]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_misc:2;
        /** core_0_pif_pms_constrain_world_0_i2c : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_i2c:2;
        uint32_t reserved_28:2;
        /** core_0_pif_pms_constrain_world_0_uart1 : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_uart1:2;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_1_reg_t;

/** Type of core_0_pif_pms_constrain_2 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_0_bt : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_bt:2;
        uint32_t reserved_2:2;
        /** core_0_pif_pms_constrain_world_0_i2c_ext0 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_i2c_ext0:2;
        /** core_0_pif_pms_constrain_world_0_uhci0 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_uhci0:2;
        uint32_t reserved_8:2;
        /** core_0_pif_pms_constrain_world_0_rmt : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_rmt:2;
        uint32_t reserved_12:4;
        /** core_0_pif_pms_constrain_world_0_ledc : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_ledc:2;
        /** core_0_pif_pms_constrain_world_0_efuse : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_efuse:2;
        uint32_t reserved_20:6;
        /** core_0_pif_pms_constrain_world_0_timergroup : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_timergroup:2;
        /** core_0_pif_pms_constrain_world_0_timergroup1 : R/W; bitpos: [29:28]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_timergroup1:2;
        /** core_0_pif_pms_constrain_world_0_systimer : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_systimer:2;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_2_reg_t;

/** Type of core_0_pif_pms_constrain_3 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_0_spi_2 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_spi_2:2;
        uint32_t reserved_2:2;
        /** core_0_pif_pms_constrain_world_0_apb_ctrl : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_apb_ctrl:2;
        uint32_t reserved_6:4;
        /** core_0_pif_pms_constrain_world_0_twai : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_twai:2;
        uint32_t reserved_12:2;
        /** core_0_pif_pms_constrain_world_0_i2s1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_i2s1:2;
        uint32_t reserved_16:6;
        /** core_0_pif_pms_constrain_world_0_rwbt : R/W; bitpos: [23:22]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_rwbt:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_3_reg_t;

/** Type of core_0_pif_pms_constrain_4 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** core_0_pif_pms_constrain_world_0_crypto_peri : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_crypto_peri:2;
        /** core_0_pif_pms_constrain_world_0_crypto_dma : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_crypto_dma:2;
        /** core_0_pif_pms_constrain_world_0_apb_adc : R/W; bitpos: [9:8]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_apb_adc:2;
        uint32_t reserved_10:4;
        /** core_0_pif_pms_constrain_world_0_usb_device : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_usb_device:2;
        /** core_0_pif_pms_constrain_world_0_etm : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_etm:2;
        /** core_0_pif_pms_constrain_world_0_timergroup3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_timergroup3:2;
        /** core_0_pif_pms_constrain_world_0_ble_sec : R/W; bitpos: [21:20]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_ble_sec:2;
        /** core_0_pif_pms_constrain_world_0_ieee802154mac : R/W; bitpos: [23:22]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_ieee802154mac:2;
        uint32_t reserved_24:2;
        /** core_0_pif_pms_constrain_world_0_coex : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_coex:2;
        /** core_0_pif_pms_constrain_world_0_rtc_ble_tmr : R/W; bitpos: [29:28]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_rtc_ble_tmr:2;
        /** core_0_pif_pms_constrain_world_0_clkrst : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_clkrst:2;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_4_reg_t;

/** Type of core_0_pif_pms_constrain_5 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_0_pvt : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_pvt:2;
        /** core_0_pif_pms_constrain_world_0_modem_widgets : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_modem_widgets:2;
        /** core_0_pif_pms_constrain_world_0_system : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_system:2;
        /** core_0_pif_pms_constrain_world_0_sensitive : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_sensitive:2;
        /** core_0_pif_pms_constrain_world_0_interrupt : R/W; bitpos: [9:8]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_interrupt:2;
        /** core_0_pif_pms_constrain_world_0_dma_copy : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_dma_copy:2;
        /** core_0_pif_pms_constrain_world_0_cache_config : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_cache_config:2;
        /** core_0_pif_pms_constrain_world_0_ad : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_ad:2;
        /** core_0_pif_pms_constrain_world_0_dio : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_dio:2;
        /** core_0_pif_pms_constrain_world_0_world_controller : R/W; bitpos: [19:18]; default:
         *  3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_0_world_controller:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_5_reg_t;

/** Type of core_0_pif_pms_constrain_6 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_1_uart : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_uart:2;
        /** core_0_pif_pms_constrain_world_1_g0spi_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_g0spi_1:2;
        /** core_0_pif_pms_constrain_world_1_g0spi_0 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_g0spi_0:2;
        /** core_0_pif_pms_constrain_world_1_gpio : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_gpio:2;
        uint32_t reserved_8:2;
        /** core_0_pif_pms_constrain_world_1_fe : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_fe:2;
        uint32_t reserved_12:2;
        /** core_0_pif_pms_constrain_world_1_rtc : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_rtc:2;
        /** core_0_pif_pms_constrain_world_1_io_mux : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_io_mux:2;
        /** core_0_pif_pms_constrain_world_1_wdg : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_wdg:2;
        uint32_t reserved_20:4;
        /** core_0_pif_pms_constrain_world_1_misc : R/W; bitpos: [25:24]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_misc:2;
        /** core_0_pif_pms_constrain_world_1_i2c : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_i2c:2;
        uint32_t reserved_28:2;
        /** core_0_pif_pms_constrain_world_1_uart1 : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_uart1:2;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_6_reg_t;

/** Type of core_0_pif_pms_constrain_7 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_1_bt : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_bt:2;
        uint32_t reserved_2:2;
        /** core_0_pif_pms_constrain_world_1_i2c_ext0 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_i2c_ext0:2;
        /** core_0_pif_pms_constrain_world_1_uhci0 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_uhci0:2;
        uint32_t reserved_8:2;
        /** core_0_pif_pms_constrain_world_1_rmt : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_rmt:2;
        uint32_t reserved_12:4;
        /** core_0_pif_pms_constrain_world_1_ledc : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_ledc:2;
        /** core_0_pif_pms_constrain_world_1_efuse : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_efuse:2;
        uint32_t reserved_20:6;
        /** core_0_pif_pms_constrain_world_1_timergroup : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_timergroup:2;
        /** core_0_pif_pms_constrain_world_1_timergroup1 : R/W; bitpos: [29:28]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_timergroup1:2;
        /** core_0_pif_pms_constrain_world_1_systimer : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_systimer:2;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_7_reg_t;

/** Type of core_0_pif_pms_constrain_8 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_1_spi_2 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_spi_2:2;
        uint32_t reserved_2:2;
        /** core_0_pif_pms_constrain_world_1_apb_ctrl : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_apb_ctrl:2;
        uint32_t reserved_6:4;
        /** core_0_pif_pms_constrain_world_1_twai : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_twai:2;
        uint32_t reserved_12:2;
        /** core_0_pif_pms_constrain_world_1_i2s1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_i2s1:2;
        uint32_t reserved_16:6;
        /** core_0_pif_pms_constrain_world_1_rwbt : R/W; bitpos: [23:22]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_rwbt:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_8_reg_t;

/** Type of core_0_pif_pms_constrain_9 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** core_0_pif_pms_constrain_world_1_crypto_peri : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_crypto_peri:2;
        /** core_0_pif_pms_constrain_world_1_crypto_dma : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_crypto_dma:2;
        /** core_0_pif_pms_constrain_world_1_apb_adc : R/W; bitpos: [9:8]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_apb_adc:2;
        uint32_t reserved_10:4;
        /** core_0_pif_pms_constrain_world_1_usb_device : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_usb_device:2;
        /** core_0_pif_pms_constrain_world_1_etm : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_etm:2;
        /** core_0_pif_pms_constrain_world_1_timergroup3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_timergroup3:2;
        /** core_0_pif_pms_constrain_world_1_ble_sec : R/W; bitpos: [21:20]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_ble_sec:2;
        /** core_0_pif_pms_constrain_world_1_ieee802154mac : R/W; bitpos: [23:22]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_ieee802154mac:2;
        uint32_t reserved_24:2;
        /** core_0_pif_pms_constrain_world_1_coex : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_coex:2;
        /** core_0_pif_pms_constrain_world_1_rtc_ble_tmr : R/W; bitpos: [29:28]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_rtc_ble_tmr:2;
        /** core_0_pif_pms_constrain_world_1_clkrst : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_clkrst:2;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_9_reg_t;

/** Type of core_0_pif_pms_constrain_10 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_world_1_pvt : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_pvt:2;
        /** core_0_pif_pms_constrain_world_1_modem_widgets : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_modem_widgets:2;
        /** core_0_pif_pms_constrain_world_1_system : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_system:2;
        /** core_0_pif_pms_constrain_world_1_sensitive : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_sensitive:2;
        /** core_0_pif_pms_constrain_world_1_interrupt : R/W; bitpos: [9:8]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_interrupt:2;
        /** core_0_pif_pms_constrain_world_1_dma_copy : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_dma_copy:2;
        /** core_0_pif_pms_constrain_world_1_cache_config : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_cache_config:2;
        /** core_0_pif_pms_constrain_world_1_ad : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_ad:2;
        /** core_0_pif_pms_constrain_world_1_dio : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_dio:2;
        /** core_0_pif_pms_constrain_world_1_world_controller : R/W; bitpos: [19:18]; default:
         *  3;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_world_1_world_controller:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_10_reg_t;

/** Type of core_0_pif_pms_constrain_11 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_rtcfast_spltaddr_world_0 : R/W; bitpos: [10:0]; default:
         *  2047;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_rtcfast_spltaddr_world_0:11;
        /** core_0_pif_pms_constrain_rtcfast_spltaddr_world_1 : R/W; bitpos: [21:11]; default:
         *  2047;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_rtcfast_spltaddr_world_1:11;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_11_reg_t;

/** Type of core_0_pif_pms_constrain_12 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_constrain_rtcfast_world_0_l : R/W; bitpos: [2:0]; default: 7;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_rtcfast_world_0_l:3;
        /** core_0_pif_pms_constrain_rtcfast_world_0_h : R/W; bitpos: [5:3]; default: 7;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_rtcfast_world_0_h:3;
        /** core_0_pif_pms_constrain_rtcfast_world_1_l : R/W; bitpos: [8:6]; default: 7;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_rtcfast_world_1_l:3;
        /** core_0_pif_pms_constrain_rtcfast_world_1_h : R/W; bitpos: [11:9]; default: 7;
         *  Need add description
         */
        uint32_t core_0_pif_pms_constrain_rtcfast_world_1_h:3;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_constrain_12_reg_t;

/** Type of region_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_region_pms_constrain_0_reg_t;

/** Type of region_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_world_0_area_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_0_area_0:2;
        /** region_pms_constrain_world_0_area_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_0_area_1:2;
        /** region_pms_constrain_world_0_area_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_0_area_2:2;
        /** region_pms_constrain_world_0_area_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_0_area_3:2;
        /** region_pms_constrain_world_0_area_4 : R/W; bitpos: [9:8]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_0_area_4:2;
        /** region_pms_constrain_world_0_area_5 : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_0_area_5:2;
        /** region_pms_constrain_world_0_area_6 : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_0_area_6:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} sensitive_region_pms_constrain_1_reg_t;

/** Type of region_pms_constrain_2 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_world_1_area_0 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_1_area_0:2;
        /** region_pms_constrain_world_1_area_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_1_area_1:2;
        /** region_pms_constrain_world_1_area_2 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_1_area_2:2;
        /** region_pms_constrain_world_1_area_3 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_1_area_3:2;
        /** region_pms_constrain_world_1_area_4 : R/W; bitpos: [9:8]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_1_area_4:2;
        /** region_pms_constrain_world_1_area_5 : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_1_area_5:2;
        /** region_pms_constrain_world_1_area_6 : R/W; bitpos: [13:12]; default: 3;
         *  Need add description
         */
        uint32_t region_pms_constrain_world_1_area_6:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} sensitive_region_pms_constrain_2_reg_t;

/** Type of region_pms_constrain_3 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_0 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_0:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_3_reg_t;

/** Type of region_pms_constrain_4 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_1 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_1:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_4_reg_t;

/** Type of region_pms_constrain_5 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_2 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_2:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_5_reg_t;

/** Type of region_pms_constrain_6 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_3 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_3:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_6_reg_t;

/** Type of region_pms_constrain_7 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_4 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_4:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_7_reg_t;

/** Type of region_pms_constrain_8 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_5 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_5:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_8_reg_t;

/** Type of region_pms_constrain_9 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_6 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_6:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_9_reg_t;

/** Type of region_pms_constrain_10 register
 *  register description
 */
typedef union {
    struct {
        /** region_pms_constrain_addr_7 : R/W; bitpos: [29:0]; default: 0;
         *  Need add description
         */
        uint32_t region_pms_constrain_addr_7:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sensitive_region_pms_constrain_10_reg_t;

/** Type of core_0_pif_pms_monitor_0 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_monitor_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_monitor_0_reg_t;

/** Type of core_0_pif_pms_monitor_1 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_monitor_violate_clr : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_clr:1;
        /** core_0_pif_pms_monitor_violate_en : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_monitor_1_reg_t;

/** Type of core_0_pif_pms_monitor_2 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_monitor_violate_intr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_intr:1;
        /** core_0_pif_pms_monitor_violate_status_hport_0 : RO; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_status_hport_0:1;
        /** core_0_pif_pms_monitor_violate_status_hsize : RO; bitpos: [4:2]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_status_hsize:3;
        /** core_0_pif_pms_monitor_violate_status_hwrite : RO; bitpos: [5]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_status_hwrite:1;
        /** core_0_pif_pms_monitor_violate_status_hworld : RO; bitpos: [7:6]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_status_hworld:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_monitor_2_reg_t;

/** Type of core_0_pif_pms_monitor_3 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_monitor_violate_status_haddr : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_violate_status_haddr:32;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_monitor_3_reg_t;

/** Type of core_0_pif_pms_monitor_4 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_monitor_nonword_violate_clr : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_nonword_violate_clr:1;
        /** core_0_pif_pms_monitor_nonword_violate_en : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_nonword_violate_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_monitor_4_reg_t;

/** Type of core_0_pif_pms_monitor_5 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_monitor_nonword_violate_intr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_nonword_violate_intr:1;
        /** core_0_pif_pms_monitor_nonword_violate_status_hsize : RO; bitpos: [2:1]; default: 0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_nonword_violate_status_hsize:2;
        /** core_0_pif_pms_monitor_nonword_violate_status_hworld : RO; bitpos: [4:3]; default:
         *  0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_nonword_violate_status_hworld:2;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_monitor_5_reg_t;

/** Type of core_0_pif_pms_monitor_6 register
 *  register description
 */
typedef union {
    struct {
        /** core_0_pif_pms_monitor_nonword_violate_status_haddr : RO; bitpos: [31:0]; default:
         *  0;
         *  Need add description
         */
        uint32_t core_0_pif_pms_monitor_nonword_violate_status_haddr:32;
    };
    uint32_t val;
} sensitive_core_0_pif_pms_monitor_6_reg_t;

/** Type of backup_bus_pms_constrain_0 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_constrain_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_backup_bus_pms_constrain_0_reg_t;

/** Type of backup_bus_pms_constrain_1 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_constrain_uart : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_uart:2;
        /** backup_bus_pms_constrain_g0spi_1 : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_g0spi_1:2;
        /** backup_bus_pms_constrain_g0spi_0 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_g0spi_0:2;
        /** backup_bus_pms_constrain_gpio : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_gpio:2;
        uint32_t reserved_8:2;
        /** backup_bus_pms_constrain_fe : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_fe:2;
        uint32_t reserved_12:2;
        /** backup_bus_pms_constrain_rtc : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_rtc:2;
        /** backup_bus_pms_constrain_io_mux : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_io_mux:2;
        /** backup_bus_pms_constrain_wdg : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_wdg:2;
        uint32_t reserved_20:4;
        /** backup_bus_pms_constrain_misc : R/W; bitpos: [25:24]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_misc:2;
        /** backup_bus_pms_constrain_i2c : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_i2c:2;
        uint32_t reserved_28:2;
        /** backup_bus_pms_constrain_uart1 : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_uart1:2;
    };
    uint32_t val;
} sensitive_backup_bus_pms_constrain_1_reg_t;

/** Type of backup_bus_pms_constrain_2 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_constrain_bt : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_bt:2;
        uint32_t reserved_2:2;
        /** backup_bus_pms_constrain_i2c_ext0 : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_i2c_ext0:2;
        /** backup_bus_pms_constrain_uhci0 : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_uhci0:2;
        uint32_t reserved_8:2;
        /** backup_bus_pms_constrain_rmt : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_rmt:2;
        uint32_t reserved_12:4;
        /** backup_bus_pms_constrain_ledc : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_ledc:2;
        /** backup_bus_pms_constrain_efuse : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_efuse:2;
        uint32_t reserved_20:6;
        /** backup_bus_pms_constrain_timergroup : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_timergroup:2;
        /** backup_bus_pms_constrain_timergroup1 : R/W; bitpos: [29:28]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_timergroup1:2;
        /** backup_bus_pms_constrain_systimer : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_systimer:2;
    };
    uint32_t val;
} sensitive_backup_bus_pms_constrain_2_reg_t;

/** Type of backup_bus_pms_constrain_3 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_constrain_spi_2 : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_spi_2:2;
        uint32_t reserved_2:2;
        /** backup_bus_pms_constrain_apb_ctrl : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_apb_ctrl:2;
        uint32_t reserved_6:4;
        /** backup_bus_pms_constrain_twai : R/W; bitpos: [11:10]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_twai:2;
        uint32_t reserved_12:2;
        /** backup_bus_pms_constrain_i2s1 : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_i2s1:2;
        uint32_t reserved_16:6;
        /** backup_bus_pms_constrain_rwbt : R/W; bitpos: [23:22]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_rwbt:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} sensitive_backup_bus_pms_constrain_3_reg_t;

/** Type of backup_bus_pms_constrain_4 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** backup_bus_pms_constrain_crypto_peri : R/W; bitpos: [5:4]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_crypto_peri:2;
        /** backup_bus_pms_constrain_crypto_dma : R/W; bitpos: [7:6]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_crypto_dma:2;
        /** backup_bus_pms_constrain_apb_adc : R/W; bitpos: [9:8]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_apb_adc:2;
        uint32_t reserved_10:4;
        /** backup_bus_pms_constrain_usb_device : R/W; bitpos: [15:14]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_usb_device:2;
        /** backup_bus_pms_constrain_etm : R/W; bitpos: [17:16]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_etm:2;
        /** backup_bus_pms_constrain_timergroup3 : R/W; bitpos: [19:18]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_timergroup3:2;
        /** backup_bus_pms_constrain_ble_sec : R/W; bitpos: [21:20]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_ble_sec:2;
        /** backup_bus_pms_constrain_ieee802154mac : R/W; bitpos: [23:22]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_ieee802154mac:2;
        uint32_t reserved_24:2;
        /** backup_bus_pms_constrain_coex : R/W; bitpos: [27:26]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_coex:2;
        /** backup_bus_pms_constrain_rtc_ble_tmr : R/W; bitpos: [29:28]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_rtc_ble_tmr:2;
        /** backup_bus_pms_constrain_clkrst : R/W; bitpos: [31:30]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_clkrst:2;
    };
    uint32_t val;
} sensitive_backup_bus_pms_constrain_4_reg_t;

/** Type of backup_bus_pms_constrain_5 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_constrain_pvt : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_pvt:2;
        /** backup_bus_pms_constrain_modem_widgets : R/W; bitpos: [3:2]; default: 3;
         *  Need add description
         */
        uint32_t backup_bus_pms_constrain_modem_widgets:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sensitive_backup_bus_pms_constrain_5_reg_t;

/** Type of backup_bus_pms_monitor_0 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_monitor_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} sensitive_backup_bus_pms_monitor_0_reg_t;

/** Type of backup_bus_pms_monitor_1 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_monitor_violate_clr : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_violate_clr:1;
        /** backup_bus_pms_monitor_violate_en : R/W; bitpos: [1]; default: 1;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_violate_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sensitive_backup_bus_pms_monitor_1_reg_t;

/** Type of backup_bus_pms_monitor_2 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_monitor_violate_intr : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_violate_intr:1;
        /** backup_bus_pms_monitor_violate_status_htrans : RO; bitpos: [2:1]; default: 0;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_violate_status_htrans:2;
        /** backup_bus_pms_monitor_violate_status_hsize : RO; bitpos: [5:3]; default: 0;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_violate_status_hsize:3;
        /** backup_bus_pms_monitor_violate_status_hwrite : RO; bitpos: [6]; default: 0;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_violate_status_hwrite:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} sensitive_backup_bus_pms_monitor_2_reg_t;

/** Type of backup_bus_pms_monitor_3 register
 *  register description
 */
typedef union {
    struct {
        /** backup_bus_pms_monitor_violate_haddr : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t backup_bus_pms_monitor_violate_haddr:32;
    };
    uint32_t val;
} sensitive_backup_bus_pms_monitor_3_reg_t;

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
        /** sensitive_reg_date : R/W; bitpos: [27:0]; default: 34636368;
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
    volatile sensitive_privilege_mode_sel_lock_reg_t privilege_mode_sel_lock;
    volatile sensitive_privilege_mode_sel_reg_t privilege_mode_sel;
    volatile sensitive_apb_peripheral_access_0_reg_t apb_peripheral_access_0;
    volatile sensitive_apb_peripheral_access_1_reg_t apb_peripheral_access_1;
    volatile sensitive_internal_sram_usage_0_reg_t internal_sram_usage_0;
    volatile sensitive_internal_sram_usage_1_reg_t internal_sram_usage_1;
    volatile sensitive_internal_sram_usage_3_reg_t internal_sram_usage_3;
    volatile sensitive_internal_sram_usage_4_reg_t internal_sram_usage_4;
    volatile sensitive_cache_tag_access_0_reg_t cache_tag_access_0;
    volatile sensitive_cache_tag_access_1_reg_t cache_tag_access_1;
    volatile sensitive_cache_mmu_access_0_reg_t cache_mmu_access_0;
    volatile sensitive_cache_mmu_access_1_reg_t cache_mmu_access_1;
    volatile sensitive_dma_apbperi_spi2_pms_constrain_0_reg_t dma_apbperi_spi2_pms_constrain_0;
    volatile sensitive_dma_apbperi_spi2_pms_constrain_1_reg_t dma_apbperi_spi2_pms_constrain_1;
    volatile sensitive_dma_apbperi_uchi0_pms_constrain_0_reg_t dma_apbperi_uchi0_pms_constrain_0;
    volatile sensitive_dma_apbperi_uchi0_pms_constrain_1_reg_t dma_apbperi_uchi0_pms_constrain_1;
    volatile sensitive_dma_apbperi_i2s0_pms_constrain_0_reg_t dma_apbperi_i2s0_pms_constrain_0;
    volatile sensitive_dma_apbperi_i2s0_pms_constrain_1_reg_t dma_apbperi_i2s0_pms_constrain_1;
    volatile sensitive_dma_apbperi_mac_pms_constrain_0_reg_t dma_apbperi_mac_pms_constrain_0;
    volatile sensitive_dma_apbperi_mac_pms_constrain_1_reg_t dma_apbperi_mac_pms_constrain_1;
    volatile sensitive_dma_apbperi_backup_pms_constrain_0_reg_t dma_apbperi_backup_pms_constrain_0;
    volatile sensitive_dma_apbperi_backup_pms_constrain_1_reg_t dma_apbperi_backup_pms_constrain_1;
    volatile sensitive_dma_apbperi_lc_pms_constrain_0_reg_t dma_apbperi_lc_pms_constrain_0;
    volatile sensitive_dma_apbperi_lc_pms_constrain_1_reg_t dma_apbperi_lc_pms_constrain_1;
    volatile sensitive_dma_apbperi_aes_pms_constrain_0_reg_t dma_apbperi_aes_pms_constrain_0;
    volatile sensitive_dma_apbperi_aes_pms_constrain_1_reg_t dma_apbperi_aes_pms_constrain_1;
    volatile sensitive_dma_apbperi_sha_pms_constrain_0_reg_t dma_apbperi_sha_pms_constrain_0;
    volatile sensitive_dma_apbperi_sha_pms_constrain_1_reg_t dma_apbperi_sha_pms_constrain_1;
    volatile sensitive_dma_apbperi_adc_dac_pms_constrain_0_reg_t dma_apbperi_adc_dac_pms_constrain_0;
    volatile sensitive_dma_apbperi_adc_dac_pms_constrain_1_reg_t dma_apbperi_adc_dac_pms_constrain_1;
    volatile sensitive_dma_apbperi_ble_sec_pms_constrain_0_reg_t dma_apbperi_ble_sec_pms_constrain_0;
    volatile sensitive_dma_apbperi_ble_sec_pms_constrain_1_reg_t dma_apbperi_ble_sec_pms_constrain_1;
    volatile sensitive_dma_apbperi_white_list_pms_constrain_0_reg_t dma_apbperi_white_list_pms_constrain_0;
    volatile sensitive_dma_apbperi_white_list_pms_constrain_1_reg_t dma_apbperi_white_list_pms_constrain_1;
    volatile sensitive_dma_apbperi_sdio_host_pms_constrain_0_reg_t dma_apbperi_sdio_host_pms_constrain_0;
    volatile sensitive_dma_apbperi_sdio_host_pms_constrain_1_reg_t dma_apbperi_sdio_host_pms_constrain_1;
    volatile sensitive_dma_apbperi_pms_monitor_0_reg_t dma_apbperi_pms_monitor_0;
    volatile sensitive_dma_apbperi_pms_monitor_1_reg_t dma_apbperi_pms_monitor_1;
    volatile sensitive_dma_apbperi_pms_monitor_2_reg_t dma_apbperi_pms_monitor_2;
    volatile sensitive_dma_apbperi_pms_monitor_3_reg_t dma_apbperi_pms_monitor_3;
    volatile sensitive_core_x_iram0_dram0_dma_split_line_constrain_0_reg_t core_x_iram0_dram0_dma_split_line_constrain_0;
    volatile sensitive_core_x_iram0_dram0_dma_split_line_constrain_1_reg_t core_x_iram0_dram0_dma_split_line_constrain_1;
    volatile sensitive_core_x_iram0_dram0_dma_split_line_constrain_2_reg_t core_x_iram0_dram0_dma_split_line_constrain_2;
    volatile sensitive_core_x_iram0_dram0_dma_split_line_constrain_3_reg_t core_x_iram0_dram0_dma_split_line_constrain_3;
    volatile sensitive_core_x_iram0_dram0_dma_split_line_constrain_4_reg_t core_x_iram0_dram0_dma_split_line_constrain_4;
    volatile sensitive_core_x_iram0_dram0_dma_split_line_constrain_5_reg_t core_x_iram0_dram0_dma_split_line_constrain_5;
    volatile sensitive_core_x_iram0_pms_constrain_0_reg_t core_x_iram0_pms_constrain_0;
    volatile sensitive_core_x_iram0_pms_constrain_1_reg_t core_x_iram0_pms_constrain_1;
    volatile sensitive_core_x_iram0_pms_constrain_2_reg_t core_x_iram0_pms_constrain_2;
    volatile sensitive_core_0_iram0_pms_monitor_0_reg_t core_0_iram0_pms_monitor_0;
    volatile sensitive_core_0_iram0_pms_monitor_1_reg_t core_0_iram0_pms_monitor_1;
    volatile sensitive_core_0_iram0_pms_monitor_2_reg_t core_0_iram0_pms_monitor_2;
    volatile sensitive_core_x_dram0_pms_constrain_0_reg_t core_x_dram0_pms_constrain_0;
    volatile sensitive_core_x_dram0_pms_constrain_1_reg_t core_x_dram0_pms_constrain_1;
    volatile sensitive_core_0_dram0_pms_monitor_0_reg_t core_0_dram0_pms_monitor_0;
    volatile sensitive_core_0_dram0_pms_monitor_1_reg_t core_0_dram0_pms_monitor_1;
    volatile sensitive_core_0_dram0_pms_monitor_2_reg_t core_0_dram0_pms_monitor_2;
    volatile sensitive_core_0_dram0_pms_monitor_3_reg_t core_0_dram0_pms_monitor_3;
    volatile sensitive_core_0_pif_pms_constrain_0_reg_t core_0_pif_pms_constrain_0;
    volatile sensitive_core_0_pif_pms_constrain_1_reg_t core_0_pif_pms_constrain_1;
    volatile sensitive_core_0_pif_pms_constrain_2_reg_t core_0_pif_pms_constrain_2;
    volatile sensitive_core_0_pif_pms_constrain_3_reg_t core_0_pif_pms_constrain_3;
    volatile sensitive_core_0_pif_pms_constrain_4_reg_t core_0_pif_pms_constrain_4;
    volatile sensitive_core_0_pif_pms_constrain_5_reg_t core_0_pif_pms_constrain_5;
    volatile sensitive_core_0_pif_pms_constrain_6_reg_t core_0_pif_pms_constrain_6;
    volatile sensitive_core_0_pif_pms_constrain_7_reg_t core_0_pif_pms_constrain_7;
    volatile sensitive_core_0_pif_pms_constrain_8_reg_t core_0_pif_pms_constrain_8;
    volatile sensitive_core_0_pif_pms_constrain_9_reg_t core_0_pif_pms_constrain_9;
    volatile sensitive_core_0_pif_pms_constrain_10_reg_t core_0_pif_pms_constrain_10;
    volatile sensitive_core_0_pif_pms_constrain_11_reg_t core_0_pif_pms_constrain_11;
    volatile sensitive_core_0_pif_pms_constrain_12_reg_t core_0_pif_pms_constrain_12;
    volatile sensitive_region_pms_constrain_0_reg_t region_pms_constrain_0;
    volatile sensitive_region_pms_constrain_1_reg_t region_pms_constrain_1;
    volatile sensitive_region_pms_constrain_2_reg_t region_pms_constrain_2;
    volatile sensitive_region_pms_constrain_3_reg_t region_pms_constrain_3;
    volatile sensitive_region_pms_constrain_4_reg_t region_pms_constrain_4;
    volatile sensitive_region_pms_constrain_5_reg_t region_pms_constrain_5;
    volatile sensitive_region_pms_constrain_6_reg_t region_pms_constrain_6;
    volatile sensitive_region_pms_constrain_7_reg_t region_pms_constrain_7;
    volatile sensitive_region_pms_constrain_8_reg_t region_pms_constrain_8;
    volatile sensitive_region_pms_constrain_9_reg_t region_pms_constrain_9;
    volatile sensitive_region_pms_constrain_10_reg_t region_pms_constrain_10;
    volatile sensitive_core_0_pif_pms_monitor_0_reg_t core_0_pif_pms_monitor_0;
    volatile sensitive_core_0_pif_pms_monitor_1_reg_t core_0_pif_pms_monitor_1;
    volatile sensitive_core_0_pif_pms_monitor_2_reg_t core_0_pif_pms_monitor_2;
    volatile sensitive_core_0_pif_pms_monitor_3_reg_t core_0_pif_pms_monitor_3;
    volatile sensitive_core_0_pif_pms_monitor_4_reg_t core_0_pif_pms_monitor_4;
    volatile sensitive_core_0_pif_pms_monitor_5_reg_t core_0_pif_pms_monitor_5;
    volatile sensitive_core_0_pif_pms_monitor_6_reg_t core_0_pif_pms_monitor_6;
    volatile sensitive_backup_bus_pms_constrain_0_reg_t backup_bus_pms_constrain_0;
    volatile sensitive_backup_bus_pms_constrain_1_reg_t backup_bus_pms_constrain_1;
    volatile sensitive_backup_bus_pms_constrain_2_reg_t backup_bus_pms_constrain_2;
    volatile sensitive_backup_bus_pms_constrain_3_reg_t backup_bus_pms_constrain_3;
    volatile sensitive_backup_bus_pms_constrain_4_reg_t backup_bus_pms_constrain_4;
    volatile sensitive_backup_bus_pms_constrain_5_reg_t backup_bus_pms_constrain_5;
    volatile sensitive_backup_bus_pms_monitor_0_reg_t backup_bus_pms_monitor_0;
    volatile sensitive_backup_bus_pms_monitor_1_reg_t backup_bus_pms_monitor_1;
    volatile sensitive_backup_bus_pms_monitor_2_reg_t backup_bus_pms_monitor_2;
    volatile sensitive_backup_bus_pms_monitor_3_reg_t backup_bus_pms_monitor_3;
    volatile sensitive_clock_gate_reg_t clock_gate;
    uint32_t reserved_198[921];
    volatile sensitive_sensitive_reg_date_reg_t sensitive_reg_date;
} sensitive_dev_t;

extern sensitive_dev_t SENSITIVE;

#ifndef __cplusplus
_Static_assert(sizeof(sensitive_dev_t) == 0x1000, "Invalid size of sensitive_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
