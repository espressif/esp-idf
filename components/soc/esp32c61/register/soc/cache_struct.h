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

/** Group: Control and Configuration Registers */
/** Type of l1_cache_ctrl register
 *  L1 data cache (L1 cache) control register
 */
typedef union {
    struct {
        /** l1_cache_shut_bus0 : R/W; bitpos: [0]; default: 0;
         *  Configures whether to disable BUS0 to access the L1 cache.
         *  0: Enable
         *  1: Disable
         */
        uint32_t l1_cache_shut_bus0:1;
        /** l1_cache_shut_bus1 : R/W; bitpos: [1]; default: 0;
         *  Configures whether to disable BUS1 to access the L1 cache.
         *  0: Enable
         *  1: Disable
         */
        uint32_t l1_cache_shut_bus1:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cache_l1_cache_ctrl_reg_t;

/** Type of l1_cache_cachesize_conf register
 *  Cache cache size register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** l1_cache_cachesize_32k : RO; bitpos: [7]; default: 1;
         *  Indicates that cache size is 32 KB.
         */
        uint32_t l1_cache_cachesize_32k:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} cache_l1_cache_cachesize_conf_reg_t;

/** Type of l1_cache_blocksize_conf register
 *  Cache block size register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** l1_cache_blocksize_32 : RO; bitpos: [2]; default: 1;
         *  Indicates that the cache block size is 32 bytes.
         */
        uint32_t l1_cache_blocksize_32:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_l1_cache_blocksize_conf_reg_t;

/** Type of l1_cache_wrap_around_ctrl register
 *  Cache critical word first control register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_wrap : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable the critical word first mode for the L1 cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_wrap:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_wrap_around_ctrl_reg_t;

/** Type of l1_cache_miss_access_ctrl register
 *  Cache wrap around control register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_miss_disable_access : R/W; bitpos: [4]; default: 0;
         *  Configures whether to disable early restart function.
         *  0: Enable
         *  1: Disable
         */
        uint32_t l1_cache_miss_disable_access:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_miss_access_ctrl_reg_t;


/** Group: Cache Freeze Control Registers */
/** Type of l1_cache_freeze_ctrl register
 *  Cache freeze control register
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** l1_cache_freeze_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether to enable freeze operation in the L1 cache. It can be cleared by
         *  software.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_freeze_en:1;
        /** l1_cache_freeze_mode : R/W; bitpos: [17]; default: 0;
         *  Configures the freeze mode in the L1 cache.
         *  0: Assert busy if a cache miss occurs
         *  1: Assert hit if a cache miss occurs
         */
        uint32_t l1_cache_freeze_mode:1;
        /** l1_cache_freeze_done : RO; bitpos: [18]; default: 0;
         *  Represents whether the freeze operation in L1 cache is finished.
         *  0: Not finished
         *  1: Finished
         */
        uint32_t l1_cache_freeze_done:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} cache_l1_cache_freeze_ctrl_reg_t;


/** Group: Cache Data Memory Access Control and Configuration Registers */
/** Type of l1_cache_data_mem_acs_conf register
 *  Cache data memory access configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** l1_cache_data_mem_rd_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether to enable the configuration bus to read the L1 cache data memory.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_data_mem_rd_en:1;
        /** l1_cache_data_mem_wr_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether to enable the configuration bus to write the L1 cache data
         *  memory.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_data_mem_wr_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} cache_l1_cache_data_mem_acs_conf_reg_t;

/** Type of l1_cache_tag_mem_acs_conf register
 *  Cache tag memory access configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** l1_cache_tag_mem_rd_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether to enable the configuration bus to read the L1 cache tag memory.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_tag_mem_rd_en:1;
        /** l1_cache_tag_mem_wr_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether to enable the configuration bus to write the L1 cache tag memory.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_tag_mem_wr_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} cache_l1_cache_tag_mem_acs_conf_reg_t;


/** Group: Prelock Control and Configuration Registers */
/** Type of l1_cache_prelock_conf register
 *  L1 Cache prelock configure register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable the prelocking function in Section 0 of the L1 cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_prelock_sct0_en:1;
        /** l1_cache_prelock_sct1_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether to enable the prelocking function in Section 1 of the L1 cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_prelock_sct1_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cache_l1_cache_prelock_conf_reg_t;

/** Type of l1_cache_prelock_sct0_addr register
 *  L1 cache prelocking Section 0 address configuration register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting virtual address of Section 0 for prelocking in the L1
         *  cache. This field should be configured together with
         *  CACHE_L1_CACHE_PRELOCK_SCT0_SIZE.
         */
        uint32_t l1_cache_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_cache_prelock_sct0_addr_reg_t;

/** Type of l1_cache_prelock_sct1_addr register
 *  L1 cache prelocking Section 1 address configuration register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting virtual address of Section 1 for prelocking in the L1
         *  cache. This field should be configured together with
         *  CACHE_L1_CACHE_PRELOCK_SCT1_SIZE.
         */
        uint32_t l1_cache_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_cache_prelock_sct1_addr_reg_t;

/** Type of l1_cache_prelock_sct_size register
 *  L1 cache prelocking section size configuration register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct0_size : R/W; bitpos: [13:0]; default: 16383;
         *  Configures the size of Section 0 for prelocking in the L1 cache. This field should
         *  be configured together with CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_REG.
         */
        uint32_t l1_cache_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_cache_prelock_sct1_size : R/W; bitpos: [29:16]; default: 16383;
         *  Configures the size of Section 1 for prelocking in the L1 cache. This field should
         *  be configured together with CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_REG.
         */
        uint32_t l1_cache_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_cache_prelock_sct_size_reg_t;


/** Group: Lock Control and Configuration Registers */
/** Type of lock_ctrl register
 *  Locking operation control register
 */
typedef union {
    struct {
        /** lock_ena : R/W/SC; bitpos: [0]; default: 0;
         *  Configures whether to enable lock operation. It will be cleared by hardware after
         *  the lock operation is done.
         *  0: Disable
         *  1: Enable
         */
        uint32_t lock_ena:1;
        /** unlock_ena : R/W/SC; bitpos: [1]; default: 0;
         *  Configures whether to enable unlock operation. It will be cleared by hardware after
         *  the unlock operation is done.
         *  0: Disable
         *  1: Enable
         */
        uint32_t unlock_ena:1;
        /** lock_done : RO; bitpos: [2]; default: 1;
         *  Represents whether the unlock or the lock operation is finished.
         *  0: Not finished
         *  1: Finished
         */
        uint32_t lock_done:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_lock_ctrl_reg_t;

/** Type of lock_map register
 *  Locking map configuration register
 */
typedef union {
    struct {
        /** lock_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures which level of cache in the two-level cache structure to perform the
         *  locking or the unlocking operation.
         *  Bit 4: Write 1 to this bit to select the L1 cache
         *  Other bits: reserved
         */
        uint32_t lock_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_lock_map_reg_t;

/** Type of lock_addr register
 *  Locking address configuration register
 */
typedef union {
    struct {
        /** lock_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting virtual address for the locking or unlocking operation.
         *  This field should be configured together with CACHE_LOCK_SIZE_REG.
         */
        uint32_t lock_addr:32;
    };
    uint32_t val;
} cache_lock_addr_reg_t;

/** Type of lock_size register
 *  Locking size configuration register
 */
typedef union {
    struct {
        /** lock_size : R/W; bitpos: [15:0]; default: 0;
         *  Configures the size for the locking or unlocking operation. This field should be
         *  configured together with CACHE_LOCK_ADDR_REG.
         */
        uint32_t lock_size:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_lock_size_reg_t;


/** Group: Sync Control and Configuration Registers */
/** Type of sync_ctrl register
 *  Sync operation control register
 */
typedef union {
    struct {
        /** invalidate_ena : RW; bitpos: [0]; default: 1;
         *  Configures whether to enable the invalidatation operation. Hardware will clear it
         *  once the operation has finished.
         *  0: Disable
         *  1: Enable
         */
        uint32_t invalidate_ena:1;
        /** clean_ena : R/W/SC; bitpos: [1]; default: 0;
         *  Configures whether to enable the clean operation. Hardware will clear it once the
         *  operation has finished.
         *  0: Disable
         *  1: Enable
         */
        uint32_t clean_ena:1;
        /** writeback_ena : R/W/SC; bitpos: [2]; default: 0;
         *  Configures whether to enable the write-back operation. Hardware will clear it once
         *  the operation has finished.
         *  0: Disable
         *  1: Enable
         */
        uint32_t writeback_ena:1;
        /** writeback_invalidate_ena : R/W/SC; bitpos: [3]; default: 0;
         *  Configures whether to enable the write-back invalidation operation. Hardware will
         *  clear it once the operation has finished.
         *  0: Disable
         *  1: Enable
         */
        uint32_t writeback_invalidate_ena:1;
        /** sync_done : RO; bitpos: [4]; default: 0;
         *  Represents whether the synchronization operation is finished.
         *  0: Not finished
         *  1: Finished
         */
        uint32_t sync_done:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_sync_ctrl_reg_t;

/** Type of sync_map register
 *  Sync map configuration register
 */
typedef union {
    struct {
        /** sync_map : R/W; bitpos: [5:0]; default: 63;
         *  Configures which caches in the two-level cache structure will apply the sync
         *  operation.
         *  Bit 4: Write 1 to this bit to select the L1 cache
         *  Other bits: Reserved
         */
        uint32_t sync_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_sync_map_reg_t;

/** Type of sync_addr register
 *  Sync address configuration register
 */
typedef union {
    struct {
        /** sync_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting virtual address for the synchronization operation. This
         *  field should be configured together with CACHE_SYNC_SIZE_REG.
         */
        uint32_t sync_addr:32;
    };
    uint32_t val;
} cache_sync_addr_reg_t;

/** Type of sync_size register
 *  Sync size configuration register
 */
typedef union {
    struct {
        /** sync_size : R/W; bitpos: [24:0]; default: 0;
         *  Configures the size for the synchronization operation. This field should be
         *  configured together with CACHE_SYNC_ADDR_REG.
         */
        uint32_t sync_size:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} cache_sync_size_reg_t;


/** Group: Preloading Control and Configuration Registers */
/** Type of l1_cache_preload_ctrl register
 *  L1 cache preloading operation control register
 */
typedef union {
    struct {
        /** l1_cache_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  Configures whether to enable the preloading operation in the L1 cache. It will be
         *  cleared by hardware automatically after the preloading operation is done.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_preload_ena:1;
        /** l1_cache_preload_done : RO; bitpos: [1]; default: 1;
         *  Represents whether the preloading operation is finished.
         *  0: Not finished
         *  1: Finished
         */
        uint32_t l1_cache_preload_done:1;
        /** l1_cache_preload_order : R/W; bitpos: [2]; default: 0;
         *  Configures the direction of the preloading operation.
         *  0: Ascending
         *  1: Descending
         */
        uint32_t l1_cache_preload_order:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_l1_cache_preload_ctrl_reg_t;

/** Type of l1_cache_preload_addr register
 *  L1 cache preloading address configuration register
 */
typedef union {
    struct {
        /** l1_cache_preload_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting virtual address for the preloading operation in the L1
         *  cache. This field should be configured together with
         *  CACHE_L1_CACHE_PRELOAD_SIZE_REG.
         */
        uint32_t l1_cache_preload_addr:32;
    };
    uint32_t val;
} cache_l1_cache_preload_addr_reg_t;

/** Type of l1_cache_preload_size register
 *  L1 cache preloading size configuration register
 */
typedef union {
    struct {
        /** l1_cache_preload_size : R/W; bitpos: [13:0]; default: 0;
         *  Configures the size of Section 0 for the preloading operation in the L1 cache. This
         *  field should be configured together with CACHE_L1_CACHE_PRELOAD_ADDR_REG.
         */
        uint32_t l1_cache_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_preload_size_reg_t;


/** Group: Autoloading Control and Configuration Registers */
/** Type of l1_cache_autoload_ctrl register
 *  L1 cache autoloading operation control register
 */
typedef union {
    struct {
        /** l1_cache_autoload_ena : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable the autoloading operation in the L1 cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_autoload_ena:1;
        /** l1_cache_autoload_done : RO; bitpos: [1]; default: 1;
         *  Represents whether the autoloading operation in the L1 cache is finished.
         *  0: Not finished
         *  1: Finished
         */
        uint32_t l1_cache_autoload_done:1;
        /** l1_cache_autoload_order : R/W; bitpos: [2]; default: 0;
         *  Configures the direction of the autoloading operation in the L1 cache.
         *  0: Ascending
         *  1: Descending
         */
        uint32_t l1_cache_autoload_order:1;
        /** l1_cache_autoload_trigger_mode : R/W; bitpos: [4:3]; default: 0;
         *  Configures the trigger mode of the autoloading operation in the L1 cache.
         *  0/3: Triggered by misses
         *  1: Triggered by hits
         *  2: Triggered by misses or hits
         */
        uint32_t l1_cache_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_cache_autoload_sct0_ena : R/W; bitpos: [8]; default: 0;
         *  Configures whether to enable Section 0 for the autoloading operation in the L1
         *  cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_autoload_sct0_ena:1;
        /** l1_cache_autoload_sct1_ena : R/W; bitpos: [9]; default: 0;
         *  Configures whether to enable Section 1 for the autoloading operation in the L1
         *  cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_cache_autoload_sct1_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} cache_l1_cache_autoload_ctrl_reg_t;

/** Type of l1_cache_autoload_sct0_addr register
 *  L1 cache autoloading Section 0 address configuration register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting virtual address of Section 0 for the autoloading operation
         *  in the L1 cache. Note that it should be used together with
         *  CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE and CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_cache_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct0_addr_reg_t;

/** Type of l1_cache_autoload_sct0_size register
 *  L1 cache autoloading Section 0 size configuration register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct0_size : R/W; bitpos: [24:0]; default: 0;
         *  Configures the size of Section 0 for the autoloading operation in the L1 cache.
         *  Note that it should be used together with CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR and
         *  CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_cache_autoload_sct0_size:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct0_size_reg_t;

/** Type of l1_cache_autoload_sct1_addr register
 *  L1 cache autoloading Section 1 address configuration register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the starting virtual address of Section 1 for the autoloading operation
         *  in the L1 cache. Note that it should be used together with
         *  CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE and CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_cache_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct1_addr_reg_t;

/** Type of l1_cache_autoload_sct1_size register
 *  L1 cache autoloading Section 1 size configuration register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct1_size : R/W; bitpos: [24:0]; default: 0;
         *  Configures the size of Section 1 for the autoloading operation in the L1 cache.
         *  Note that it should be used together with CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR and
         *  CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_cache_autoload_sct1_size:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct1_size_reg_t;


/** Group: Interrupt Registers */
/** Type of l1_cache_acs_cnt_int_ena register
 *  Cache access counter interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_bus0_ovf_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable L1_BUS0_OVF_INT.
         */
        uint32_t l1_bus0_ovf_int_ena:1;
        /** l1_bus1_ovf_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable L1_BUS1_OVF_INT.
         */
        uint32_t l1_bus1_ovf_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_ena_reg_t;

/** Type of l1_cache_acs_cnt_int_clr register
 *  Cache access counter interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_bus0_ovf_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear L1_BUS0_OVF_INT.
         */
        uint32_t l1_bus0_ovf_int_clr:1;
        /** l1_bus1_ovf_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear L1_BUS1_OVF_INT.
         */
        uint32_t l1_bus1_ovf_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_clr_reg_t;

/** Type of l1_cache_acs_cnt_int_raw register
 *  Cache access counter interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_bus0_ovf_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of L1_BUS0_OVF_INT.
         */
        uint32_t l1_bus0_ovf_int_raw:1;
        /** l1_bus1_ovf_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of L1_BUS1_OVF_INT.
         */
        uint32_t l1_bus1_ovf_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_raw_reg_t;

/** Type of l1_cache_acs_cnt_int_st register
 *  Cache access counter interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_bus0_ovf_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of L1_BUS0_OVF_INT.
         */
        uint32_t l1_bus0_ovf_int_st:1;
        /** l1_bus1_ovf_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of L1_BUS1_OVF_INT.
         */
        uint32_t l1_bus1_ovf_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_st_reg_t;

/** Type of l1_cache_acs_fail_ctrl register
 *  Cache access failed interrupt configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_acs_fail_check_mode : R/W; bitpos: [4]; default: 0;
         *  Configures whether to pass the exception of cache accessing lower-level memory to
         *  the upper-level device.
         *  0: Do not pass to the upper-level device
         *  1: Pass to the upper-level device
         */
        uint32_t l1_cache_acs_fail_check_mode:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_ctrl_reg_t;

/** Type of l1_cache_acs_fail_int_ena register
 *  Cache access failed interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_fail_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable L1_CACHE_FAIL_INT.
         */
        uint32_t l1_cache_fail_int_ena:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_ena_reg_t;

/** Type of l1_cache_acs_fail_int_clr register
 *  L1 cache access failed interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_fail_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear L1_CACHE_FAIL_INT.
         */
        uint32_t l1_cache_fail_int_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_clr_reg_t;

/** Type of l1_cache_acs_fail_int_raw register
 *  Cache access failed interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_fail_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of L1_CACHE_FAIL_INT.
         */
        uint32_t l1_cache_fail_int_raw:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_raw_reg_t;

/** Type of l1_cache_acs_fail_int_st register
 *  Cache access failed interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_fail_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of L1_CACHE_FAIL_INT.
         */
        uint32_t l1_cache_fail_int_st:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_st_reg_t;

/** Type of l1_cache_sync_preload_int_ena register
 *  Sync and preloading operation interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_pld_done_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable L1_CACHE_PLD_DONE_INT.
         */
        uint32_t l1_cache_pld_done_int_ena:1;
        uint32_t reserved_5:1;
        /** sync_done_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable CACHE_SYNC_DONE_INT.
         */
        uint32_t sync_done_int_ena:1;
        uint32_t reserved_7:4;
        /** l1_cache_pld_err_int_ena : R/W; bitpos: [11]; default: 0;
         *  Write 1 to enable CACHE_PLD_ERR_INT.
         */
        uint32_t l1_cache_pld_err_int_ena:1;
        uint32_t reserved_12:1;
        /** sync_err_int_ena : R/W; bitpos: [13]; default: 0;
         *  Write 1 to enable CACHE_SYNC_ERR_INT.
         */
        uint32_t sync_err_int_ena:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_ena_reg_t;

/** Type of l1_cache_sync_preload_int_clr register
 *  Sync and preloading operation interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_pld_done_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear L1_CACHE_PLD_DONE_INT.
         */
        uint32_t l1_cache_pld_done_int_clr:1;
        uint32_t reserved_5:1;
        /** sync_done_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear CACHE_SYNC_DONE_INT.
         */
        uint32_t sync_done_int_clr:1;
        uint32_t reserved_7:4;
        /** l1_cache_pld_err_int_clr : WT; bitpos: [11]; default: 0;
         *  Write 1 to clear L1_CACHE_PLD_ERR_INT.
         */
        uint32_t l1_cache_pld_err_int_clr:1;
        uint32_t reserved_12:1;
        /** sync_err_int_clr : WT; bitpos: [13]; default: 0;
         *  Write 1 to clear CACHE_SYNC_ERR_INT.
         */
        uint32_t sync_err_int_clr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_clr_reg_t;

/** Type of l1_cache_sync_preload_int_raw register
 *  Sync and preloading operation interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_pld_done_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of L1_CACHE_PLD_DONE_INT.
         */
        uint32_t l1_cache_pld_done_int_raw:1;
        uint32_t reserved_5:1;
        /** sync_done_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of CACHE_SYNC_DONE_INT.
         */
        uint32_t sync_done_int_raw:1;
        uint32_t reserved_7:4;
        /** l1_cache_pld_err_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt status of L1_CACHE_PLD_ERR_INT.
         */
        uint32_t l1_cache_pld_err_int_raw:1;
        uint32_t reserved_12:1;
        /** sync_err_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw interrupt status of CACHE_SYNC_ERR_INT.
         */
        uint32_t sync_err_int_raw:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_raw_reg_t;

/** Type of l1_cache_sync_preload_int_st register
 *  Sync and preloading operation interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_pld_done_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of L1_CACHE_PLD_DONE_INT.
         */
        uint32_t l1_cache_pld_done_int_st:1;
        uint32_t reserved_5:1;
        /** sync_done_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of CACHE_SYNC_DONE_INT.
         */
        uint32_t sync_done_int_st:1;
        uint32_t reserved_7:4;
        /** l1_cache_pld_err_int_st : RO; bitpos: [11]; default: 0;
         *  The masked interrupt status of L1_CACHE_PLD_ERR_INT.
         */
        uint32_t l1_cache_pld_err_int_st:1;
        uint32_t reserved_12:1;
        /** sync_err_int_st : RO; bitpos: [13]; default: 0;
         *  The masked interrupt status of CACHE_SYNC_ERR_INT.
         */
        uint32_t sync_err_int_st:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_st_reg_t;


/** Group: Access Statistics Registers */
/** Type of l1_cache_acs_cnt_ctrl register
 *  Cache access counter enable and clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_bus0_cnt_ena : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable the BUS0 counters in the L1 cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_bus0_cnt_ena:1;
        /** l1_bus1_cnt_ena : R/W; bitpos: [5]; default: 0;
         *  Configures whether to enable the BUS1 counters in the L1 cache.
         *  0: Disable
         *  1: Enable
         */
        uint32_t l1_bus1_cnt_ena:1;
        uint32_t reserved_6:14;
        /** l1_bus0_cnt_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether to clear the BUS0 counters in the L1 cache.
         *  0: Not clear
         *  1: Clear
         */
        uint32_t l1_bus0_cnt_clr:1;
        /** l1_bus1_cnt_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether to clear the BUS1 counters in the L1 cache.
         *  0: Not clear
         *  1: Clear
         */
        uint32_t l1_bus1_cnt_clr:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_ctrl_reg_t;


/** Group: Access Statistics registers */
/** Type of l1_bus0_acs_hit_cnt register
 *  L1 cache BUS0 hit-access counter register
 */
typedef union {
    struct {
        /** l1_bus0_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of hits when BUS0 accesses the L1 cache.
         */
        uint32_t l1_bus0_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_hit_cnt_reg_t;

/** Type of l1_bus0_acs_miss_cnt register
 *  L1 cache BUS0 missed-access counter register
 */
typedef union {
    struct {
        /** l1_bus0_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of misses when BUS0 accesses the L1 cache.
         */
        uint32_t l1_bus0_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_miss_cnt_reg_t;

/** Type of l1_bus0_acs_conflict_cnt register
 *  L1 cache BUS0 conflicting-access Counter register
 */
typedef union {
    struct {
        /** l1_bus0_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of access conflicts when BUS0 accesses the L1 cache.
         */
        uint32_t l1_bus0_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_conflict_cnt_reg_t;

/** Type of l1_bus0_acs_nxtlvl_rd_cnt register
 *  L1 cache BUS0 reads the lower-level memory
 */
typedef union {
    struct {
        /** l1_bus0_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of times that the cache accesses its lower-level module due
         *  to BUS0's read access.
         */
        uint32_t l1_bus0_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_bus0_acs_nxtlvl_wr_cnt register
 *  L1 cache BUS0 writes the lower-level memory
 */
typedef union {
    struct {
        /** l1_bus0_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of times that the cache accesses its lower-level module due
         *  to BUS0's write access.
         */
        uint32_t l1_bus0_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l1_bus1_acs_hit_cnt register
 *  L1 cache BUS1 hit-access counter register
 */
typedef union {
    struct {
        /** l1_bus1_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of hits when BUS1 accesses the L1 cache.
         */
        uint32_t l1_bus1_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_bus1_acs_hit_cnt_reg_t;

/** Type of l1_bus1_acs_miss_cnt register
 *  L1 cache BUS1 missed-access counter register
 */
typedef union {
    struct {
        /** l1_bus1_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of misses when BUS1 accesses the L1 cache.
         */
        uint32_t l1_bus1_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_bus1_acs_miss_cnt_reg_t;

/** Type of l1_bus1_acs_conflict_cnt register
 *  L1 cache BUS1 conflicting-access counter register
 */
typedef union {
    struct {
        /** l1_bus1_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of access conflicts when BUS1 accesses the L1 cache.
         */
        uint32_t l1_bus1_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_bus1_acs_conflict_cnt_reg_t;

/** Type of l1_dbus1_acs_nxtlvl_rd_cnt register
 *  L1 cache BUS1 reads the lower-level memory
 */
typedef union {
    struct {
        /** l1_bus1_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of times that the cache accesses its lower-level module due
         *  to BUS1's read access.
         */
        uint32_t l1_bus1_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_dbus1_acs_nxtlvl_wr_cnt register
 *  L1 cache BUS1 writes the lower-level memory
 */
typedef union {
    struct {
        /** l1_bus1_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  Represents the number of times that the cache accesses its lower-level module due
         *  to BUS1's write access.
         */
        uint32_t l1_bus1_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_nxtlvl_wr_cnt_reg_t;


/** Group: Access Fail Debug registers */
/** Type of l1_cache_acs_fail_id_attr register
 *  L1 cache access failed ID and attribution recording register
 */
typedef union {
    struct {
        /** l1_cache_fail_id : RO; bitpos: [15:0]; default: 0;
         *  Represents the ID of the failed access to the L1 cache.
         *  Bit 4: BUS0
         *  Bit 5: BUS1
         *  Others: reserved
         */
        uint32_t l1_cache_fail_id:16;
        /** l1_cache_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  Represents the attribute of the failed access to the L1 cache.
         *  Bit 0: Fetching instructions
         *  Bit 1: Reading data
         *  Bit 7: non-cacheable
         *  Others: reserved
         */
        uint32_t l1_cache_fail_attr:16;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_id_attr_reg_t;

/** Type of l1_cache_acs_fail_addr register
 *  L1 cache access failed address information register
 */
typedef union {
    struct {
        /** l1_cache_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the failed access to the L1 cache.
         */
        uint32_t l1_cache_fail_addr:32;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_addr_reg_t;


/** Group: Access Fail Debug Registers */
/** Type of l1_cache_sync_preload_exception register
 *  Cache sync and preloading operation exception register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** l1_cache_pld_err_code : RO; bitpos: [9:8]; default: 0;
         *  Represents the error code for the failed preloading operation.
         *  2: The preloading size in the L1 cache is wrong
         *  Other values: reserved
         */
        uint32_t l1_cache_pld_err_code:2;
        uint32_t reserved_10:2;
        /** sync_err_code : RO; bitpos: [13:12]; default: 0;
         *  Represents the error code for the failed synchronization operation.
         *  0: Incorrect synchronization map
         *  1: Synchronization command conflict
         *  2: Incorrect synchronization size
         *  3: Reserved
         */
        uint32_t sync_err_code:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_exception_reg_t;


/** Group: Sync Reset Control and Configuration Registers */
/** Type of l1_cache_sync_rst_ctrl register
 *  Cache sync reset control register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_sync_rst : R/W; bitpos: [4]; default: 0;
         *  Configures whether to reset the synchronization logic inside the L1 cache.
         *  0: Not reset
         *  1: Reset
         *  It is recommended that this field should only be used when a fatal error of the
         *  synchronization logic occurs.
         */
        uint32_t l1_cache_sync_rst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_sync_rst_ctrl_reg_t;

/** Type of l1_cache_preload_rst_ctrl register
 *  Cache preloading reset control register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_pld_rst : R/W; bitpos: [4]; default: 0;
         *  Configures whether to reset the preloading logic inside the L1 cache.
         *  0: Not reset
         *  1: Reset
         *  It is recommended that this field should only be used when a fatal error of the
         *  preloading logic occurs.
         */
        uint32_t l1_cache_pld_rst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_preload_rst_ctrl_reg_t;


/** Group: Autoloading Buffer Clear Control and Configuration Registers */
/** Type of l1_cache_autoload_buf_clr_ctrl register
 *  Cache autoloading buffer clear control register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_ald_buf_clr : R/W; bitpos: [4]; default: 0;
         *  Configures whether to clear the autoloading buffer inside the L1 cache.
         *  0: Not clear the buffer. Recommended when autoloading of L1 cache is used.
         *  1: Clear the buffer. Once set, autoloading of L1 cache will stop working.
         */
        uint32_t l1_cache_ald_buf_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_autoload_buf_clr_ctrl_reg_t;


/** Group: Tag and Data Memory Access Control and Configuration Registers */
/** Type of l1_cache_object_ctrl register
 *  Cache tag and data memory object control register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l1_cache_tag_object : R/W; bitpos: [4]; default: 0;
         *  Configure whether to set the L1 cache tag memory as an object.
         *  0: Not set
         *  1: Set
         *  This register uses "one-hot encoding," which means that at any time, only one field
         *  can be valid in conjunction with other fields.
         */
        uint32_t l1_cache_tag_object:1;
        uint32_t reserved_5:5;
        /** l1_cache_mem_object : R/W; bitpos: [10]; default: 0;
         *  Configure whether to set the L1 cache data memory as an object.
         *  0: Not set
         *  1: Set
         *  This register uses "one-hot encoding," which means that at any time, only one field
         *  can be valid in conjunction with other fields.
         */
        uint32_t l1_cache_mem_object:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} cache_l1_cache_object_ctrl_reg_t;

/** Type of l1_cache_way_object register
 *  Cache tag and data memory way register
 */
typedef union {
    struct {
        /** l1_cache_way_object : R/W; bitpos: [2:0]; default: 0;
         *  Configures which way in the tag memory or the data memory to access.
         *  0: Way 0
         *  1: Way 1
         *  2: Way 2
         *  3: Way 3
         */
        uint32_t l1_cache_way_object:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_l1_cache_way_object_reg_t;

/** Type of l1_cache_debug_bus register
 *  Cache tag and data memory content register
 */
typedef union {
    struct {
        /** l1_cache_debug_bus : R/W; bitpos: [31:0]; default: 612;
         *  Configures the data to write to or the data to read from the tag memory or the data
         *  memory.
         */
        uint32_t l1_cache_debug_bus:32;
    };
    uint32_t val;
} cache_l1_cache_debug_bus_reg_t;


/** Group: Tag and Data Memory Access Control and configuration register */
/** Type of l1_cache_addr register
 *  Cache address register
 */
typedef union {
    struct {
        /** l1_cache_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the virtual address inside the tag memory or data memory to access.
         */
        uint32_t l1_cache_addr:32;
    };
    uint32_t val;
} cache_l1_cache_addr_reg_t;


/** Group: Cache Trace Control register */
/** Type of trace_ena register
 *  Clock gate control register
 */
typedef union {
    struct {
        /** l1_cache_trace_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable L1-Cache trace for the performance counter and fail tracer
         */
        uint32_t l1_cache_trace_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cache_trace_ena_reg_t;


/** Group: Version Control Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36774432;
         *  Version control register. Note that the default value is the latest date when the
         *  hardware logic was updated.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_date_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile cache_l1_cache_ctrl_reg_t l1_cache_ctrl;
    uint32_t reserved_008[4];
    volatile cache_l1_cache_cachesize_conf_reg_t l1_cache_cachesize_conf;
    volatile cache_l1_cache_blocksize_conf_reg_t l1_cache_blocksize_conf;
    volatile cache_l1_cache_wrap_around_ctrl_reg_t l1_cache_wrap_around_ctrl;
    volatile cache_l1_cache_miss_access_ctrl_reg_t l1_cache_miss_access_ctrl;
    volatile cache_l1_cache_freeze_ctrl_reg_t l1_cache_freeze_ctrl;
    volatile cache_l1_cache_data_mem_acs_conf_reg_t l1_cache_data_mem_acs_conf;
    volatile cache_l1_cache_tag_mem_acs_conf_reg_t l1_cache_tag_mem_acs_conf;
    uint32_t reserved_034[16];
    volatile cache_l1_cache_prelock_conf_reg_t l1_cache_prelock_conf;
    volatile cache_l1_cache_prelock_sct0_addr_reg_t l1_cache_prelock_sct0_addr;
    volatile cache_l1_cache_prelock_sct1_addr_reg_t l1_cache_prelock_sct1_addr;
    volatile cache_l1_cache_prelock_sct_size_reg_t l1_cache_prelock_sct_size;
    volatile cache_lock_ctrl_reg_t lock_ctrl;
    volatile cache_lock_map_reg_t lock_map;
    volatile cache_lock_addr_reg_t lock_addr;
    volatile cache_lock_size_reg_t lock_size;
    volatile cache_sync_ctrl_reg_t sync_ctrl;
    volatile cache_sync_map_reg_t sync_map;
    volatile cache_sync_addr_reg_t sync_addr;
    volatile cache_sync_size_reg_t sync_size;
    uint32_t reserved_0a4[12];
    volatile cache_l1_cache_preload_ctrl_reg_t l1_cache_preload_ctrl;
    volatile cache_l1_cache_preload_addr_reg_t l1_cache_preload_addr;
    volatile cache_l1_cache_preload_size_reg_t l1_cache_preload_size;
    uint32_t reserved_0e0[20];
    volatile cache_l1_cache_autoload_ctrl_reg_t l1_cache_autoload_ctrl;
    volatile cache_l1_cache_autoload_sct0_addr_reg_t l1_cache_autoload_sct0_addr;
    volatile cache_l1_cache_autoload_sct0_size_reg_t l1_cache_autoload_sct0_size;
    volatile cache_l1_cache_autoload_sct1_addr_reg_t l1_cache_autoload_sct1_addr;
    volatile cache_l1_cache_autoload_sct1_size_reg_t l1_cache_autoload_sct1_size;
    uint32_t reserved_144[4];
    volatile cache_l1_cache_acs_cnt_int_ena_reg_t l1_cache_acs_cnt_int_ena;
    volatile cache_l1_cache_acs_cnt_int_clr_reg_t l1_cache_acs_cnt_int_clr;
    volatile cache_l1_cache_acs_cnt_int_raw_reg_t l1_cache_acs_cnt_int_raw;
    volatile cache_l1_cache_acs_cnt_int_st_reg_t l1_cache_acs_cnt_int_st;
    volatile cache_l1_cache_acs_fail_ctrl_reg_t l1_cache_acs_fail_ctrl;
    volatile cache_l1_cache_acs_fail_int_ena_reg_t l1_cache_acs_fail_int_ena;
    volatile cache_l1_cache_acs_fail_int_clr_reg_t l1_cache_acs_fail_int_clr;
    volatile cache_l1_cache_acs_fail_int_raw_reg_t l1_cache_acs_fail_int_raw;
    volatile cache_l1_cache_acs_fail_int_st_reg_t l1_cache_acs_fail_int_st;
    volatile cache_l1_cache_acs_cnt_ctrl_reg_t l1_cache_acs_cnt_ctrl;
    uint32_t reserved_17c[16];
    volatile cache_l1_bus0_acs_hit_cnt_reg_t l1_bus0_acs_hit_cnt;
    volatile cache_l1_bus0_acs_miss_cnt_reg_t l1_bus0_acs_miss_cnt;
    volatile cache_l1_bus0_acs_conflict_cnt_reg_t l1_bus0_acs_conflict_cnt;
    volatile cache_l1_bus0_acs_nxtlvl_rd_cnt_reg_t l1_bus0_acs_nxtlvl_rd_cnt;
    volatile cache_l1_bus0_acs_nxtlvl_wr_cnt_reg_t l1_bus0_acs_nxtlvl_wr_cnt;
    volatile cache_l1_bus1_acs_hit_cnt_reg_t l1_bus1_acs_hit_cnt;
    volatile cache_l1_bus1_acs_miss_cnt_reg_t l1_bus1_acs_miss_cnt;
    volatile cache_l1_bus1_acs_conflict_cnt_reg_t l1_bus1_acs_conflict_cnt;
    volatile cache_l1_dbus1_acs_nxtlvl_rd_cnt_reg_t l1_dbus1_acs_nxtlvl_rd_cnt;
    volatile cache_l1_dbus1_acs_nxtlvl_wr_cnt_reg_t l1_dbus1_acs_nxtlvl_wr_cnt;
    uint32_t reserved_1e4[18];
    volatile cache_l1_cache_acs_fail_id_attr_reg_t l1_cache_acs_fail_id_attr;
    volatile cache_l1_cache_acs_fail_addr_reg_t l1_cache_acs_fail_addr;
    volatile cache_l1_cache_sync_preload_int_ena_reg_t l1_cache_sync_preload_int_ena;
    volatile cache_l1_cache_sync_preload_int_clr_reg_t l1_cache_sync_preload_int_clr;
    volatile cache_l1_cache_sync_preload_int_raw_reg_t l1_cache_sync_preload_int_raw;
    volatile cache_l1_cache_sync_preload_int_st_reg_t l1_cache_sync_preload_int_st;
    volatile cache_l1_cache_sync_preload_exception_reg_t l1_cache_sync_preload_exception;
    volatile cache_l1_cache_sync_rst_ctrl_reg_t l1_cache_sync_rst_ctrl;
    volatile cache_l1_cache_preload_rst_ctrl_reg_t l1_cache_preload_rst_ctrl;
    volatile cache_l1_cache_autoload_buf_clr_ctrl_reg_t l1_cache_autoload_buf_clr_ctrl;
    uint32_t reserved_254;
    volatile cache_l1_cache_object_ctrl_reg_t l1_cache_object_ctrl;
    volatile cache_l1_cache_way_object_reg_t l1_cache_way_object;
    volatile cache_l1_cache_addr_reg_t l1_cache_addr;
    volatile cache_l1_cache_debug_bus_reg_t l1_cache_debug_bus;
    uint32_t reserved_268[90];
    volatile cache_trace_ena_reg_t trace_ena;
    uint32_t reserved_3d4[10];
    volatile cache_date_reg_t date;
} cache_dev_t;

extern cache_dev_t CACHE;

#ifndef __cplusplus
_Static_assert(sizeof(cache_dev_t) == 0x400, "Invalid size of cache_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
