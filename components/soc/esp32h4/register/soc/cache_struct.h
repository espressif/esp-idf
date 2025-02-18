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

/** Group: Control and configuration registers */
/** Type of l1_icache_ctrl register
 *  L1 instruction Cache(L1-ICache) control register
 */
typedef union {
    struct {
        /** l1_icache_shut_ibus0 : R/W; bitpos: [0]; default: 0;
         *  The bit is used to disable core0 ibus access L1-ICache, 0: enable, 1: disable
         */
        uint32_t l1_icache_shut_ibus0:1;
        /** l1_icache_shut_ibus1 : R/W; bitpos: [1]; default: 0;
         *  The bit is used to disable core1 ibus access L1-ICache, 0: enable, 1: disable
         */
        uint32_t l1_icache_shut_ibus1:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cache_l1_icache_ctrl_reg_t;

/** Type of l1_dcache_ctrl register
 *  L1 data Cache(L1-DCache) control register
 */
typedef union {
    struct {
        /** l1_dcache_shut_dbus0 : R/W; bitpos: [0]; default: 0;
         *  The bit is used to disable core0 dbus access L1-DCache, 0: enable, 1: disable
         */
        uint32_t l1_dcache_shut_dbus0:1;
        /** l1_dcache_shut_dbus1 : R/W; bitpos: [1]; default: 0;
         *  The bit is used to disable core1 dbus access L1-DCache, 0: enable, 1: disable
         */
        uint32_t l1_dcache_shut_dbus1:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cache_l1_dcache_ctrl_reg_t;


/** Group: Bypass Cache Control and configuration registers */
/** Type of l1_bypass_cache_conf register
 *  Bypass Cache configure register
 */
typedef union {
    struct {
        /** bypass_l1_icache0_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable bypass L1-ICache0. 0: disable bypass, 1: enable bypass.
         */
        uint32_t bypass_l1_icache0_en:1;
        /** bypass_l1_icache1_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable bypass L1-ICache1. 0: disable bypass, 1: enable bypass.
         */
        uint32_t bypass_l1_icache1_en:1;
        uint32_t reserved_2:2;
        /** bypass_l1_dcache_en : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable bypass L1-DCache. 0: disable bypass, 1: enable bypass.
         */
        uint32_t bypass_l1_dcache_en:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_bypass_cache_conf_reg_t;


/** Group: Cache Atomic Control and configuration registers */
/** Type of l1_cache_atomic_conf register
 *  L1 Cache atomic feature configure register
 */
typedef union {
    struct {
        /** l1_dcache_atomic_en : R/W; bitpos: [0]; default: 1;
         *  The bit is used to enable atomic feature on L1-DCache when multiple cores access
         *  L1-DCache.  1: disable, 1: enable.
         */
        uint32_t l1_dcache_atomic_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cache_l1_cache_atomic_conf_reg_t;


/** Group: Cache Mode Control and configuration registers */
/** Type of l1_icache_cachesize_conf register
 *  L1 instruction Cache CacheSize mode configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** l1_icache_cachesize_32k : HRO; bitpos: [7]; default: 1;
         *  The field is used to configure cachesize of L1-ICache as 32k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_32k:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} cache_l1_icache_cachesize_conf_reg_t;

/** Type of l1_icache_blocksize_conf register
 *  L1 instruction Cache BlockSize mode configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** l1_icache_blocksize_32 : HRO; bitpos: [2]; default: 1;
         *  The field is used to configureblocksize of L1-ICache as 32 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_blocksize_32:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_l1_icache_blocksize_conf_reg_t;

/** Type of l1_dcache_cachesize_conf register
 *  L1 data Cache CacheSize mode configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** l1_dcache_cachesize_32k : HRO; bitpos: [7]; default: 1;
         *  The field is used to configure cachesize of L1-DCache as 32k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_32k:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} cache_l1_dcache_cachesize_conf_reg_t;

/** Type of l1_dcache_blocksize_conf register
 *  L1 data Cache BlockSize mode configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** l1_dcache_blocksize_32 : HRO; bitpos: [2]; default: 1;
         *  The field is used to configureblocksize of L1-DCache as 32 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_blocksize_32:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_l1_dcache_blocksize_conf_reg_t;


/** Group: Wrap Mode Control and configuration registers */
/** Type of l1_cache_wrap_around_ctrl register
 *  Cache wrap around control register
 */
typedef union {
    struct {
        /** l1_icache0_wrap : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to enable L1-ICache0 wrap around mode.
         */
        uint32_t l1_icache0_wrap:1;
        /** l1_icache1_wrap : R/W; bitpos: [1]; default: 0;
         *  Set this bit as 1 to enable L1-ICache1 wrap around mode.
         */
        uint32_t l1_icache1_wrap:1;
        uint32_t reserved_2:2;
        /** l1_dcache_wrap : R/W; bitpos: [4]; default: 0;
         *  Set this bit as 1 to enable L1-DCache wrap around mode.
         */
        uint32_t l1_dcache_wrap:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_wrap_around_ctrl_reg_t;


/** Group: Early Restart Control registers */
/** Type of l1_cache_miss_access_ctrl register
 *  Cache wrap around control register
 */
typedef union {
    struct {
        /** l1_icache0_miss_disable_access : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to disable early restart of  L1-ICache0
         */
        uint32_t l1_icache0_miss_disable_access:1;
        /** l1_icache1_miss_disable_access : R/W; bitpos: [1]; default: 0;
         *  Set this bit as 1 to disable early restart of  L1-ICache1
         */
        uint32_t l1_icache1_miss_disable_access:1;
        uint32_t reserved_2:2;
        /** l1_cache_miss_disable_access : R/W; bitpos: [4]; default: 0;
         *  Set this bit as 1 to disable early restart of L1-DCache
         */
        uint32_t l1_cache_miss_disable_access:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_miss_access_ctrl_reg_t;


/** Group: Cache Freeze Control registers */
/** Type of l1_cache_freeze_ctrl register
 *  Cache Freeze control register
 */
typedef union {
    struct {
        /** l1_icache0_freeze_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable freeze operation on L1-ICache0. It can be cleared by
         *  software.
         */
        uint32_t l1_icache0_freeze_en:1;
        /** l1_icache0_freeze_mode : R/W; bitpos: [1]; default: 0;
         *  The bit is used to configure mode of freeze operation L1-ICache0. 0: a miss-access
         *  will not stuck. 1: a miss-access will stuck.
         */
        uint32_t l1_icache0_freeze_mode:1;
        /** l1_icache0_freeze_done : RO; bitpos: [2]; default: 0;
         *  The bit is used to indicate whether freeze operation on L1-ICache0 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache0_freeze_done:1;
        uint32_t reserved_3:1;
        /** l1_icache1_freeze_en : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable freeze operation on L1-ICache1. It can be cleared by
         *  software.
         */
        uint32_t l1_icache1_freeze_en:1;
        /** l1_icache1_freeze_mode : R/W; bitpos: [5]; default: 0;
         *  The bit is used to configure mode of freeze operation L1-ICache1. 0: a miss-access
         *  will not stuck. 1: a miss-access will stuck.
         */
        uint32_t l1_icache1_freeze_mode:1;
        /** l1_icache1_freeze_done : RO; bitpos: [6]; default: 0;
         *  The bit is used to indicate whether freeze operation on L1-ICache1 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache1_freeze_done:1;
        uint32_t reserved_7:9;
        /** l1_dcache_freeze_en : R/W; bitpos: [16]; default: 0;
         *  The bit is used to enable freeze operation on L1-DCache. It can be cleared by
         *  software.
         */
        uint32_t l1_dcache_freeze_en:1;
        /** l1_dcache_freeze_mode : R/W; bitpos: [17]; default: 0;
         *  The bit is used to configure mode of freeze operation L1-DCache. 0: a miss-access
         *  will not stuck. 1: a miss-access will stuck.
         */
        uint32_t l1_dcache_freeze_mode:1;
        /** l1_dcache_freeze_done : RO; bitpos: [18]; default: 0;
         *  The bit is used to indicate whether freeze operation on L1-DCache is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_dcache_freeze_done:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} cache_l1_cache_freeze_ctrl_reg_t;


/** Group: Cache Data Memory Access Control and Configuration registers */
/** Type of l1_cache_data_mem_acs_conf register
 *  Cache data memory access configure register
 */
typedef union {
    struct {
        /** l1_icache0_data_mem_rd_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache0 data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache0_data_mem_rd_en:1;
        /** l1_icache0_data_mem_wr_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache0 data memoryory. 0: disable,
         *  1: enable.
         */
        uint32_t l1_icache0_data_mem_wr_en:1;
        uint32_t reserved_2:2;
        /** l1_icache1_data_mem_rd_en : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache1 data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache1_data_mem_rd_en:1;
        /** l1_icache1_data_mem_wr_en : R/W; bitpos: [5]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache1 data memoryory. 0: disable,
         *  1: enable.
         */
        uint32_t l1_icache1_data_mem_wr_en:1;
        uint32_t reserved_6:10;
        /** l1_dcache_data_mem_rd_en : R/W; bitpos: [16]; default: 0;
         *  The bit is used to enable config-bus read L1-DCache data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_dcache_data_mem_rd_en:1;
        /** l1_dcache_data_mem_wr_en : R/W; bitpos: [17]; default: 0;
         *  The bit is used to enable config-bus write L1-DCache data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_dcache_data_mem_wr_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} cache_l1_cache_data_mem_acs_conf_reg_t;


/** Group: Cache Tag Memory Access Control and Configuration registers */
/** Type of l1_cache_tag_mem_acs_conf register
 *  Cache tag memory access configure register
 */
typedef union {
    struct {
        /** l1_icache0_tag_mem_rd_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache0 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache0_tag_mem_rd_en:1;
        /** l1_icache0_tag_mem_wr_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache0 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache0_tag_mem_wr_en:1;
        uint32_t reserved_2:2;
        /** l1_icache1_tag_mem_rd_en : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache1 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache1_tag_mem_rd_en:1;
        /** l1_icache1_tag_mem_wr_en : R/W; bitpos: [5]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache1 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache1_tag_mem_wr_en:1;
        uint32_t reserved_6:10;
        /** l1_dcache_tag_mem_rd_en : R/W; bitpos: [16]; default: 0;
         *  The bit is used to enable config-bus read L1-DCache tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_dcache_tag_mem_rd_en:1;
        /** l1_dcache_tag_mem_wr_en : R/W; bitpos: [17]; default: 0;
         *  The bit is used to enable config-bus write L1-DCache tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_dcache_tag_mem_wr_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} cache_l1_cache_tag_mem_acs_conf_reg_t;


/** Group: Prelock Control and configuration registers */
/** Type of l1_icache0_prelock_conf register
 *  L1 instruction Cache 0 prelock configure register
 */
typedef union {
    struct {
        /** l1_icache0_prelock_sct0_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-ICache0.
         */
        uint32_t l1_icache0_prelock_sct0_en:1;
        /** l1_icache0_prelock_sct1_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-ICache0.
         */
        uint32_t l1_icache0_prelock_sct1_en:1;
        /** l1_icache0_prelock_rgid : R/W; bitpos: [5:2]; default: 0;
         *  The bit is used to set  the gid of l1 icache0 prelock.
         */
        uint32_t l1_icache0_prelock_rgid:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_icache0_prelock_conf_reg_t;

/** Type of l1_icache0_prelock_sct0_addr register
 *  L1 instruction Cache 0 prelock section0 address configure register
 */
typedef union {
    struct {
        /** l1_icache0_prelock_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section of prelock
         *  on L1-ICache0, which should be used together with L1_ICACHE0_PRELOCK_SCT0_SIZE_REG
         */
        uint32_t l1_icache0_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache0_prelock_sct0_addr_reg_t;

/** Type of l1_icache0_prelock_sct1_addr register
 *  L1 instruction Cache 0 prelock section1 address configure register
 */
typedef union {
    struct {
        /** l1_icache0_prelock_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section of prelock
         *  on L1-ICache0, which should be used together with L1_ICACHE0_PRELOCK_SCT1_SIZE_REG
         */
        uint32_t l1_icache0_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache0_prelock_sct1_addr_reg_t;

/** Type of l1_icache0_prelock_sct_size register
 *  L1 instruction Cache 0 prelock section size configure register
 */
typedef union {
    struct {
        /** l1_icache0_prelock_sct0_size : R/W; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_icache0_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_icache0_prelock_sct1_size : R/W; bitpos: [29:16]; default: 16383;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l1_icache0_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_icache0_prelock_sct_size_reg_t;

/** Type of l1_icache1_prelock_conf register
 *  L1 instruction Cache 1 prelock configure register
 */
typedef union {
    struct {
        /** l1_icache1_prelock_sct0_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-ICache1.
         */
        uint32_t l1_icache1_prelock_sct0_en:1;
        /** l1_icache1_prelock_sct1_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-ICache1.
         */
        uint32_t l1_icache1_prelock_sct1_en:1;
        /** l1_icache1_prelock_rgid : R/W; bitpos: [5:2]; default: 0;
         *  The bit is used to set  the gid of l1 icache1 prelock.
         */
        uint32_t l1_icache1_prelock_rgid:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_icache1_prelock_conf_reg_t;

/** Type of l1_icache1_prelock_sct0_addr register
 *  L1 instruction Cache 1 prelock section0 address configure register
 */
typedef union {
    struct {
        /** l1_icache1_prelock_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section of prelock
         *  on L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT0_SIZE_REG
         */
        uint32_t l1_icache1_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache1_prelock_sct0_addr_reg_t;

/** Type of l1_icache1_prelock_sct1_addr register
 *  L1 instruction Cache 1 prelock section1 address configure register
 */
typedef union {
    struct {
        /** l1_icache1_prelock_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section of prelock
         *  on L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT1_SIZE_REG
         */
        uint32_t l1_icache1_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache1_prelock_sct1_addr_reg_t;

/** Type of l1_icache1_prelock_sct_size register
 *  L1 instruction Cache 1 prelock section size configure register
 */
typedef union {
    struct {
        /** l1_icache1_prelock_sct0_size : R/W; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_icache1_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_icache1_prelock_sct1_size : R/W; bitpos: [29:16]; default: 16383;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l1_icache1_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_icache1_prelock_sct_size_reg_t;

/** Type of l1_dcache_prelock_conf register
 *  L1 data Cache prelock configure register
 */
typedef union {
    struct {
        /** l1_dcache_prelock_sct0_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-DCache.
         */
        uint32_t l1_dcache_prelock_sct0_en:1;
        /** l1_dcache_prelock_sct1_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-DCache.
         */
        uint32_t l1_dcache_prelock_sct1_en:1;
        /** l1_dcache_prelock_rgid : R/W; bitpos: [5:2]; default: 0;
         *  The bit is used to set  the gid of l1 dcache prelock.
         */
        uint32_t l1_dcache_prelock_rgid:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_dcache_prelock_conf_reg_t;

/** Type of l1_dcache_prelock_sct0_addr register
 *  L1 data Cache prelock section0 address configure register
 */
typedef union {
    struct {
        /** l1_dcache_prelock_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section of prelock
         *  on L1-DCache, which should be used together with L1_DCACHE_PRELOCK_SCT0_SIZE_REG
         */
        uint32_t l1_dcache_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_prelock_sct0_addr_reg_t;

/** Type of l1_dcache_prelock_sct1_addr register
 *  L1 data Cache prelock section1 address configure register
 */
typedef union {
    struct {
        /** l1_dcache_prelock_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section of prelock
         *  on L1-DCache, which should be used together with L1_DCACHE_PRELOCK_SCT1_SIZE_REG
         */
        uint32_t l1_dcache_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_prelock_sct1_addr_reg_t;

/** Type of l1_dcache_prelock_sct_size register
 *  L1 data Cache prelock section size configure register
 */
typedef union {
    struct {
        /** l1_dcache_prelock_sct0_size : R/W; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-DCache, which should be used together with L1_DCACHE_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_dcache_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_dcache_prelock_sct1_size : R/W; bitpos: [29:16]; default: 16383;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L1-DCache, which should be used together with L1_DCACHE_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l1_dcache_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_dcache_prelock_sct_size_reg_t;


/** Group: Lock Control and configuration registers */
/** Type of lock_ctrl register
 *  Lock-class (manual lock) operation control register
 */
typedef union {
    struct {
        /** lock_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable lock operation. It will be cleared by hardware after lock
         *  operation done. Note that (1) this bit and unlock_ena bit are mutually exclusive,
         *  that is, those bits can not be set to 1 at the same time. (2) lock operation can be
         *  applied on LL1-ICache, L1-DCache and L2-Cache.
         */
        uint32_t lock_ena:1;
        /** unlock_ena : R/W/SC; bitpos: [1]; default: 0;
         *  The bit is used to enable unlock operation. It will be cleared by hardware after
         *  unlock operation done. Note that (1) this bit and lock_ena bit are mutually
         *  exclusive, that is, those bits can not be set to 1 at the same time. (2) unlock
         *  operation can be applied on L1-ICache, L1-DCache and L2-Cache.
         */
        uint32_t unlock_ena:1;
        /** lock_done : RO; bitpos: [2]; default: 1;
         *  The bit is used to indicate whether unlock/lock operation is finished or not. 0:
         *  not finished. 1: finished.
         */
        uint32_t lock_done:1;
        /** lock_rgid : R/W; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of cache lock/unlock.
         */
        uint32_t lock_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_lock_ctrl_reg_t;

/** Type of lock_map register
 *  Lock (manual lock) map configure register
 */
typedef union {
    struct {
        /** lock_map : R/W; bitpos: [5:0]; default: 0;
         *  Those bits are used to indicate which caches in the two-level cache structure will
         *  apply this lock/unlock operation. [0]: L1-ICache0, [1]: L1-ICache1, [2]:
         *  L1-ICache2, [3]: L1-ICache3, [4]: L1-DCache, [5]: L2-Cache.
         */
        uint32_t lock_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_lock_map_reg_t;

/** Type of lock_addr register
 *  Lock (manual lock) address configure register
 */
typedef union {
    struct {
        /** lock_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the lock/unlock operation,
         *  which should be used together with CACHE_LOCK_SIZE_REG
         */
        uint32_t lock_addr:32;
    };
    uint32_t val;
} cache_lock_addr_reg_t;

/** Type of lock_size register
 *  Lock (manual lock) size configure register
 */
typedef union {
    struct {
        /** lock_size : R/W; bitpos: [15:0]; default: 0;
         *  Those bits are used to configure the size of the lock/unlock operation, which
         *  should be used together with CACHE_LOCK_ADDR_REG
         */
        uint32_t lock_size:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_lock_size_reg_t;


/** Group: Sync Control and configuration registers */
/** Type of sync_ctrl register
 *  Sync-class operation control register
 */
typedef union {
    struct {
        /** invalidate_ena : R/W/SC; bitpos: [0]; default: 1;
         *  The bit is used to enable invalidate operation. It will be cleared by hardware
         *  after invalidate operation done. Note that this bit and the other sync-bits
         *  (clean_ena, writeback_ena, writeback_invalidate_ena) are mutually exclusive, that
         *  is, those bits can not be set to 1 at the same time.
         */
        uint32_t invalidate_ena:1;
        /** clean_ena : R/W/SC; bitpos: [1]; default: 0;
         *  The bit is used to enable clean operation. It will be cleared by hardware after
         *  clean operation done. Note that this bit and the other sync-bits (invalidate_ena,
         *  writeback_ena, writeback_invalidate_ena) are mutually exclusive, that is, those
         *  bits can not be set to 1 at the same time.
         */
        uint32_t clean_ena:1;
        /** writeback_ena : R/W/SC; bitpos: [2]; default: 0;
         *  The bit is used to enable writeback operation. It will be cleared by hardware after
         *  writeback operation done. Note that this bit and the other sync-bits
         *  (invalidate_ena, clean_ena, writeback_invalidate_ena) are mutually exclusive, that
         *  is, those bits can not be set to 1 at the same time.
         */
        uint32_t writeback_ena:1;
        /** writeback_invalidate_ena : R/W/SC; bitpos: [3]; default: 0;
         *  The bit is used to enable writeback-invalidate operation. It will be cleared by
         *  hardware after writeback-invalidate operation done. Note that this bit and the
         *  other sync-bits (invalidate_ena, clean_ena, writeback_ena) are mutually exclusive,
         *  that is, those bits can not be set to 1 at the same time.
         */
        uint32_t writeback_invalidate_ena:1;
        /** sync_done : RO; bitpos: [4]; default: 0;
         *  The bit is used to indicate whether sync operation (invalidate, clean, writeback,
         *  writeback_invalidate) is finished or not. 0: not finished. 1: finished.
         */
        uint32_t sync_done:1;
        /** sync_rgid : R/W; bitpos: [8:5]; default: 0;
         *  The bit is used to set  the gid of cache sync operation (invalidate, clean,
         *  writeback, writeback_invalidate)
         */
        uint32_t sync_rgid:4;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} cache_sync_ctrl_reg_t;

/** Type of sync_map register
 *  Sync map configure register
 */
typedef union {
    struct {
        /** sync_map : R/W; bitpos: [5:0]; default: 31;
         *  Those bits are used to indicate which caches in the two-level cache structure will
         *  apply the sync operation. [0]: L1-ICache0, [1]: L1-ICache1, [2]: L1-ICache2, [3]:
         *  L1-ICache3, [4]: L1-DCache, [5]: L2-Cache.
         */
        uint32_t sync_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_sync_map_reg_t;

/** Type of sync_addr register
 *  Sync address configure register
 */
typedef union {
    struct {
        /** sync_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the sync operation, which
         *  should be used together with CACHE_SYNC_SIZE_REG
         */
        uint32_t sync_addr:32;
    };
    uint32_t val;
} cache_sync_addr_reg_t;

/** Type of sync_size register
 *  Sync size configure register
 */
typedef union {
    struct {
        /** sync_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the sync operation, which should be
         *  used together with CACHE_SYNC_ADDR_REG
         */
        uint32_t sync_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_sync_size_reg_t;


/** Group: Preload Control and configuration registers */
/** Type of l1_icache0_preload_ctrl register
 *  L1 instruction Cache 0 preload-operation control register
 */
typedef union {
    struct {
        /** l1_icache0_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable preload operation on L1-ICache0. It will be cleared by
         *  hardware automatically after preload operation is done.
         */
        uint32_t l1_icache0_preload_ena:1;
        /** l1_icache0_preload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether preload operation is finished or not. 0: not
         *  finished. 1: finished.
         */
        uint32_t l1_icache0_preload_done:1;
        /** l1_icache0_preload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_icache0_preload_order:1;
        /** l1_icache0_preload_rgid : R/W; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of l1 icache0 preload.
         */
        uint32_t l1_icache0_preload_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_l1_icache0_preload_ctrl_reg_t;

/** Type of l1_icache0_preload_addr register
 *  L1 instruction Cache 0 preload address configure register
 */
typedef union {
    struct {
        /** l1_icache0_preload_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of preload on L1-ICache0, which
         *  should be used together with L1_ICACHE0_PRELOAD_SIZE_REG
         */
        uint32_t l1_icache0_preload_addr:32;
    };
    uint32_t val;
} cache_l1_icache0_preload_addr_reg_t;

/** Type of l1_icache0_preload_size register
 *  L1 instruction Cache 0 preload size configure register
 */
typedef union {
    struct {
        /** l1_icache0_preload_size : R/W; bitpos: [13:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOAD_ADDR_REG
         */
        uint32_t l1_icache0_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache0_preload_size_reg_t;

/** Type of l1_icache1_preload_ctrl register
 *  L1 instruction Cache 1 preload-operation control register
 */
typedef union {
    struct {
        /** l1_icache1_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable preload operation on L1-ICache1. It will be cleared by
         *  hardware automatically after preload operation is done.
         */
        uint32_t l1_icache1_preload_ena:1;
        /** l1_icache1_preload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether preload operation is finished or not. 0: not
         *  finished. 1: finished.
         */
        uint32_t l1_icache1_preload_done:1;
        /** l1_icache1_preload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_icache1_preload_order:1;
        /** l1_icache1_preload_rgid : R/W; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of l1 icache1 preload.
         */
        uint32_t l1_icache1_preload_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_l1_icache1_preload_ctrl_reg_t;

/** Type of l1_icache1_preload_addr register
 *  L1 instruction Cache 1 preload address configure register
 */
typedef union {
    struct {
        /** l1_icache1_preload_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of preload on L1-ICache1, which
         *  should be used together with L1_ICACHE1_PRELOAD_SIZE_REG
         */
        uint32_t l1_icache1_preload_addr:32;
    };
    uint32_t val;
} cache_l1_icache1_preload_addr_reg_t;

/** Type of l1_icache1_preload_size register
 *  L1 instruction Cache 1 preload size configure register
 */
typedef union {
    struct {
        /** l1_icache1_preload_size : R/W; bitpos: [13:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOAD_ADDR_REG
         */
        uint32_t l1_icache1_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache1_preload_size_reg_t;

/** Type of l1_dcache_preload_ctrl register
 *  L1 data Cache  preload-operation control register
 */
typedef union {
    struct {
        /** l1_dcache_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable preload operation on L1-DCache. It will be cleared by
         *  hardware automatically after preload operation is done.
         */
        uint32_t l1_dcache_preload_ena:1;
        /** l1_dcache_preload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether preload operation is finished or not. 0: not
         *  finished. 1: finished.
         */
        uint32_t l1_dcache_preload_done:1;
        /** l1_dcache_preload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_dcache_preload_order:1;
        /** l1_dcache_preload_rgid : R/W; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of l1 dcache preload.
         */
        uint32_t l1_dcache_preload_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_l1_dcache_preload_ctrl_reg_t;

/** Type of l1_dcache_preload_addr register
 *  L1 data Cache  preload address configure register
 */
typedef union {
    struct {
        /** l1_dcache_preload_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of preload on L1-DCache, which
         *  should be used together with L1_DCACHE_PRELOAD_SIZE_REG
         */
        uint32_t l1_dcache_preload_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_preload_addr_reg_t;

/** Type of l1_dcache_preload_size register
 *  L1 data Cache  preload size configure register
 */
typedef union {
    struct {
        /** l1_dcache_preload_size : R/W; bitpos: [13:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-DCache, which should be used together with L1_DCACHE_PRELOAD_ADDR_REG
         */
        uint32_t l1_dcache_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_dcache_preload_size_reg_t;


/** Group: Autoload Control and configuration registers */
/** Type of l1_icache0_autoload_ctrl register
 *  L1 instruction Cache 0 autoload-operation control register
 */
typedef union {
    struct {
        /** l1_icache0_autoload_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-ICache0.  1: enable,
         *  0: disable.
         */
        uint32_t l1_icache0_autoload_ena:1;
        /** l1_icache0_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-ICache0 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache0_autoload_done:1;
        /** l1_icache0_autoload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-ICache0. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_icache0_autoload_order:1;
        /** l1_icache0_autoload_trigger_mode : R/W; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-ICache0.
         *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_icache0_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_icache0_autoload_sct0_ena : R/W; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-ICache0.
         */
        uint32_t l1_icache0_autoload_sct0_ena:1;
        /** l1_icache0_autoload_sct1_ena : R/W; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-ICache0.
         */
        uint32_t l1_icache0_autoload_sct1_ena:1;
        /** l1_icache0_autoload_rgid : R/W; bitpos: [13:10]; default: 0;
         *  The bit is used to set  the gid of l1 icache0 autoload.
         */
        uint32_t l1_icache0_autoload_rgid:4;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache0_autoload_ctrl_reg_t;

/** Type of l1_icache0_autoload_sct0_addr register
 *  L1 instruction Cache 0 autoload section 0 address configure register
 */
typedef union {
    struct {
        /** l1_icache0_autoload_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section for
         *  autoload operation on L1-ICache0. Note that it should be used together with
         *  L1_ICACHE0_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache0_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache0_autoload_sct0_addr_reg_t;

/** Type of l1_icache0_autoload_sct0_size register
 *  L1 instruction Cache 0 autoload section 0 size configure register
 */
typedef union {
    struct {
        /** l1_icache0_autoload_sct0_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the first section for autoload
         *  operation on L1-ICache0. Note that it should be used together with
         *  L1_ICACHE0_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache0_autoload_sct0_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache0_autoload_sct0_size_reg_t;

/** Type of l1_icache0_autoload_sct1_addr register
 *  L1 instruction Cache 0 autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_icache0_autoload_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section for
         *  autoload operation on L1-ICache0. Note that it should be used together with
         *  L1_ICACHE0_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache0_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache0_autoload_sct1_addr_reg_t;

/** Type of l1_icache0_autoload_sct1_size register
 *  L1 instruction Cache 0 autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_icache0_autoload_sct1_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the second section for autoload
         *  operation on L1-ICache0. Note that it should be used together with
         *  L1_ICACHE0_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache0_autoload_sct1_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache0_autoload_sct1_size_reg_t;

/** Type of l1_icache1_autoload_ctrl register
 *  L1 instruction Cache 1 autoload-operation control register
 */
typedef union {
    struct {
        /** l1_icache1_autoload_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-ICache1.  1: enable,
         *  0: disable.
         */
        uint32_t l1_icache1_autoload_ena:1;
        /** l1_icache1_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-ICache1 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache1_autoload_done:1;
        /** l1_icache1_autoload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-ICache1. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_icache1_autoload_order:1;
        /** l1_icache1_autoload_trigger_mode : R/W; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-ICache1.
         *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_icache1_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_icache1_autoload_sct0_ena : R/W; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-ICache1.
         */
        uint32_t l1_icache1_autoload_sct0_ena:1;
        /** l1_icache1_autoload_sct1_ena : R/W; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-ICache1.
         */
        uint32_t l1_icache1_autoload_sct1_ena:1;
        /** l1_icache1_autoload_rgid : R/W; bitpos: [13:10]; default: 0;
         *  The bit is used to set  the gid of l1 icache1 autoload.
         */
        uint32_t l1_icache1_autoload_rgid:4;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache1_autoload_ctrl_reg_t;

/** Type of l1_icache1_autoload_sct0_addr register
 *  L1 instruction Cache 1 autoload section 0 address configure register
 */
typedef union {
    struct {
        /** l1_icache1_autoload_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section for
         *  autoload operation on L1-ICache1. Note that it should be used together with
         *  L1_ICACHE1_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache1_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache1_autoload_sct0_addr_reg_t;

/** Type of l1_icache1_autoload_sct0_size register
 *  L1 instruction Cache 1 autoload section 0 size configure register
 */
typedef union {
    struct {
        /** l1_icache1_autoload_sct0_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the first section for autoload
         *  operation on L1-ICache1. Note that it should be used together with
         *  L1_ICACHE1_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache1_autoload_sct0_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache1_autoload_sct0_size_reg_t;

/** Type of l1_icache1_autoload_sct1_addr register
 *  L1 instruction Cache 1 autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_icache1_autoload_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section for
         *  autoload operation on L1-ICache1. Note that it should be used together with
         *  L1_ICACHE1_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache1_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache1_autoload_sct1_addr_reg_t;

/** Type of l1_icache1_autoload_sct1_size register
 *  L1 instruction Cache 1 autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_icache1_autoload_sct1_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the second section for autoload
         *  operation on L1-ICache1. Note that it should be used together with
         *  L1_ICACHE1_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache1_autoload_sct1_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache1_autoload_sct1_size_reg_t;

/** Type of l1_dcache_autoload_ctrl register
 *  L1 data Cache autoload-operation control register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-DCache.  1: enable,
         *  0: disable.
         */
        uint32_t l1_dcache_autoload_ena:1;
        /** l1_dcache_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-DCache is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_dcache_autoload_done:1;
        /** l1_dcache_autoload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-DCache. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_dcache_autoload_order:1;
        /** l1_dcache_autoload_trigger_mode : R/W; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-DCache.
         *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_dcache_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_dcache_autoload_sct0_ena : R/W; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-DCache.
         */
        uint32_t l1_dcache_autoload_sct0_ena:1;
        /** l1_dcache_autoload_sct1_ena : R/W; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-DCache.
         */
        uint32_t l1_dcache_autoload_sct1_ena:1;
        /** l1_dcache_autoload_sct2_ena : R/W; bitpos: [10]; default: 0;
         *  The bit is used to enable the third section for autoload operation on L1-DCache.
         */
        uint32_t l1_dcache_autoload_sct2_ena:1;
        /** l1_dcache_autoload_sct3_ena : R/W; bitpos: [11]; default: 0;
         *  The bit is used to enable the fourth section for autoload operation on L1-DCache.
         */
        uint32_t l1_dcache_autoload_sct3_ena:1;
        /** l1_dcache_autoload_rgid : R/W; bitpos: [15:12]; default: 0;
         *  The bit is used to set  the gid of l1 dcache autoload.
         */
        uint32_t l1_dcache_autoload_rgid:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l1_dcache_autoload_ctrl_reg_t;

/** Type of l1_dcache_autoload_sct0_addr register
 *  L1 data Cache autoload section 0 address configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section for
         *  autoload operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT0_SIZE and L1_DCACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_dcache_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct0_addr_reg_t;

/** Type of l1_dcache_autoload_sct0_size register
 *  L1 data Cache autoload section 0 size configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct0_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the first section for autoload
         *  operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT0_ADDR and L1_DCACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_dcache_autoload_sct0_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct0_size_reg_t;

/** Type of l1_dcache_autoload_sct1_addr register
 *  L1 data Cache autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section for
         *  autoload operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT1_SIZE and L1_DCACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_dcache_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct1_addr_reg_t;

/** Type of l1_dcache_autoload_sct1_size register
 *  L1 data Cache autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct1_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the second section for autoload
         *  operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT1_ADDR and L1_DCACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_dcache_autoload_sct1_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct1_size_reg_t;

/** Type of l1_dcache_autoload_sct2_addr register
 *  L1 data Cache autoload section 2 address configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct2_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the third section for
         *  autoload operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT2_SIZE and L1_DCACHE_AUTOLOAD_SCT2_ENA.
         */
        uint32_t l1_dcache_autoload_sct2_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct2_addr_reg_t;

/** Type of l1_dcache_autoload_sct2_size register
 *  L1 data Cache autoload section 2 size configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct2_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the third section for autoload
         *  operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT2_ADDR and L1_DCACHE_AUTOLOAD_SCT2_ENA.
         */
        uint32_t l1_dcache_autoload_sct2_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct2_size_reg_t;

/** Type of l1_dcache_autoload_sct3_addr register
 *  L1 data Cache autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct3_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the fourth section for
         *  autoload operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT3_SIZE and L1_DCACHE_AUTOLOAD_SCT3_ENA.
         */
        uint32_t l1_dcache_autoload_sct3_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct3_addr_reg_t;

/** Type of l1_dcache_autoload_sct3_size register
 *  L1 data Cache autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_dcache_autoload_sct3_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the fourth section for autoload
         *  operation on L1-DCache. Note that it should be used together with
         *  L1_DCACHE_AUTOLOAD_SCT3_ADDR and L1_DCACHE_AUTOLOAD_SCT3_ENA.
         */
        uint32_t l1_dcache_autoload_sct3_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_dcache_autoload_sct3_size_reg_t;


/** Group: Interrupt registers */
/** Type of l1_cache_acs_cnt_int_ena register
 *  Cache Access Counter Interrupt enable register
 */
typedef union {
    struct {
        /** l1_ibus0_ovf_int_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-ICache0 due to bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_ovf_int_ena:1;
        /** l1_ibus1_ovf_int_ena : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-ICache1 due to bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_ovf_int_ena:1;
        uint32_t reserved_2:2;
        /** l1_dbus0_ovf_int_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-DCache due to bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_ovf_int_ena:1;
        /** l1_dbus1_ovf_int_ena : R/W; bitpos: [5]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-DCache due to bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_ovf_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_ena_reg_t;

/** Type of l1_cache_acs_cnt_int_clr register
 *  Cache Access Counter Interrupt clear register
 */
typedef union {
    struct {
        /** l1_ibus0_ovf_int_clr : WT; bitpos: [0]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-ICache0 due
         *  to bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_ovf_int_clr:1;
        /** l1_ibus1_ovf_int_clr : WT; bitpos: [1]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-ICache1 due
         *  to bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_ovf_int_clr:1;
        uint32_t reserved_2:2;
        /** l1_dbus0_ovf_int_clr : WT; bitpos: [4]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
         *  to bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_ovf_int_clr:1;
        /** l1_dbus1_ovf_int_clr : WT; bitpos: [5]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
         *  to bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_ovf_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_clr_reg_t;

/** Type of l1_cache_acs_cnt_int_raw register
 *  Cache Access Counter Interrupt raw register
 */
typedef union {
    struct {
        /** l1_ibus0_ovf_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache0
         *  due to bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_ovf_int_raw:1;
        /** l1_ibus1_ovf_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache1
         *  due to bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_ovf_int_raw:1;
        uint32_t reserved_2:2;
        /** l1_dbus0_ovf_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
         *  due to bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_ovf_int_raw:1;
        /** l1_dbus1_ovf_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
         *  due to bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_ovf_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_raw_reg_t;

/** Type of l1_cache_acs_cnt_int_st register
 *  Cache Access Counter Interrupt status register
 */
typedef union {
    struct {
        /** l1_ibus0_ovf_int_st : RO; bitpos: [0]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-ICache0 due to bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_ovf_int_st:1;
        /** l1_ibus1_ovf_int_st : RO; bitpos: [1]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-ICache1 due to bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_ovf_int_st:1;
        uint32_t reserved_2:2;
        /** l1_dbus0_ovf_int_st : RO; bitpos: [4]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-DCache due to bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_ovf_int_st:1;
        /** l1_dbus1_ovf_int_st : RO; bitpos: [5]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-DCache due to bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_ovf_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_st_reg_t;

/** Type of l1_cache_acs_fail_int_ena register
 *  Cache Access Fail Interrupt enable register
 */
typedef union {
    struct {
        /** l1_icache0_fail_int_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable interrupt of access fail that occurs in L1-ICache0 due to
         *  cpu accesses L1-ICache0.
         */
        uint32_t l1_icache0_fail_int_ena:1;
        /** l1_icache1_fail_int_ena : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable interrupt of access fail that occurs in L1-ICache1 due to
         *  cpu accesses L1-ICache1.
         */
        uint32_t l1_icache1_fail_int_ena:1;
        uint32_t reserved_2:2;
        /** l1_dcache_fail_int_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable interrupt of access fail that occurs in L1-DCache due to
         *  cpu accesses L1-DCache.
         */
        uint32_t l1_dcache_fail_int_ena:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_ena_reg_t;

/** Type of l1_cache_acs_fail_int_clr register
 *  L1-Cache Access Fail Interrupt clear register
 */
typedef union {
    struct {
        /** l1_icache0_fail_int_clr : WT; bitpos: [0]; default: 0;
         *  The bit is used to clear interrupt of access fail that occurs in L1-ICache0 due to
         *  cpu accesses L1-ICache0.
         */
        uint32_t l1_icache0_fail_int_clr:1;
        /** l1_icache1_fail_int_clr : WT; bitpos: [1]; default: 0;
         *  The bit is used to clear interrupt of access fail that occurs in L1-ICache1 due to
         *  cpu accesses L1-ICache1.
         */
        uint32_t l1_icache1_fail_int_clr:1;
        uint32_t reserved_2:2;
        /** l1_dcache_fail_int_clr : WT; bitpos: [4]; default: 0;
         *  The bit is used to clear interrupt of access fail that occurs in L1-DCache due to
         *  cpu accesses L1-DCache.
         */
        uint32_t l1_dcache_fail_int_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_clr_reg_t;

/** Type of l1_cache_acs_fail_int_raw register
 *  Cache Access Fail Interrupt raw register
 */
typedef union {
    struct {
        /** l1_icache0_fail_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw bit of the interrupt of access fail that occurs in L1-ICache0.
         */
        uint32_t l1_icache0_fail_int_raw:1;
        /** l1_icache1_fail_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw bit of the interrupt of access fail that occurs in L1-ICache1.
         */
        uint32_t l1_icache1_fail_int_raw:1;
        uint32_t reserved_2:2;
        /** l1_dcache_fail_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit of the interrupt of access fail that occurs in L1-DCache.
         */
        uint32_t l1_dcache_fail_int_raw:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_raw_reg_t;

/** Type of l1_cache_acs_fail_int_st register
 *  Cache Access Fail Interrupt status register
 */
typedef union {
    struct {
        /** l1_icache0_fail_int_st : RO; bitpos: [0]; default: 0;
         *  The bit indicates the interrupt status of access fail that occurs in L1-ICache0 due
         *  to cpu accesses L1-ICache.
         */
        uint32_t l1_icache0_fail_int_st:1;
        /** l1_icache1_fail_int_st : RO; bitpos: [1]; default: 0;
         *  The bit indicates the interrupt status of access fail that occurs in L1-ICache1 due
         *  to cpu accesses L1-ICache.
         */
        uint32_t l1_icache1_fail_int_st:1;
        uint32_t reserved_2:2;
        /** l1_dcache_fail_int_st : RO; bitpos: [4]; default: 0;
         *  The bit indicates the interrupt status of access fail that occurs in L1-DCache due
         *  to cpu accesses L1-DCache.
         */
        uint32_t l1_dcache_fail_int_st:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_st_reg_t;

/** Type of l1_cache_sync_preload_int_ena register
 *  L1-Cache Access Fail Interrupt enable register
 */
typedef union {
    struct {
        /** l1_icache0_pld_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache0 preload-operation. If preload
         *  operation is done, interrupt occurs.
         */
        uint32_t l1_icache0_pld_done_int_ena:1;
        /** l1_icache1_pld_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache1 preload-operation. If preload
         *  operation is done, interrupt occurs.
         */
        uint32_t l1_icache1_pld_done_int_ena:1;
        uint32_t reserved_2:2;
        /** l1_dcache_pld_done_int_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable interrupt of L1-DCache preload-operation. If preload
         *  operation is done, interrupt occurs.
         */
        uint32_t l1_dcache_pld_done_int_ena:1;
        uint32_t reserved_5:1;
        /** sync_done_int_ena : R/W; bitpos: [6]; default: 0;
         *  The bit is used to enable interrupt of Cache sync-operation done.
         */
        uint32_t sync_done_int_ena:1;
        /** l1_icache0_pld_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache0 preload-operation error.
         */
        uint32_t l1_icache0_pld_err_int_ena:1;
        /** l1_icache1_pld_err_int_ena : R/W; bitpos: [8]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache1 preload-operation error.
         */
        uint32_t l1_icache1_pld_err_int_ena:1;
        uint32_t reserved_9:2;
        /** l1_dcache_pld_err_int_ena : R/W; bitpos: [11]; default: 0;
         *  The bit is used to enable interrupt of L1-DCache preload-operation error.
         */
        uint32_t l1_dcache_pld_err_int_ena:1;
        uint32_t reserved_12:1;
        /** sync_err_int_ena : R/W; bitpos: [13]; default: 0;
         *  The bit is used to enable interrupt of Cache sync-operation error.
         */
        uint32_t sync_err_int_ena:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_ena_reg_t;

/** Type of l1_cache_sync_preload_int_clr register
 *  Sync Preload operation Interrupt clear register
 */
typedef union {
    struct {
        /** l1_icache0_pld_done_int_clr : WT; bitpos: [0]; default: 0;
         *  The bit is used to clear interrupt that occurs only when L1-ICache0
         *  preload-operation is done.
         */
        uint32_t l1_icache0_pld_done_int_clr:1;
        /** l1_icache1_pld_done_int_clr : WT; bitpos: [1]; default: 0;
         *  The bit is used to clear interrupt that occurs only when L1-ICache1
         *  preload-operation is done.
         */
        uint32_t l1_icache1_pld_done_int_clr:1;
        uint32_t reserved_2:2;
        /** l1_dcache_pld_done_int_clr : WT; bitpos: [4]; default: 0;
         *  The bit is used to clear interrupt that occurs only when L1-DCache
         *  preload-operation is done.
         */
        uint32_t l1_dcache_pld_done_int_clr:1;
        uint32_t reserved_5:1;
        /** sync_done_int_clr : WT; bitpos: [6]; default: 0;
         *  The bit is used to clear interrupt that occurs only when Cache sync-operation is
         *  done.
         */
        uint32_t sync_done_int_clr:1;
        /** l1_icache0_pld_err_int_clr : WT; bitpos: [7]; default: 0;
         *  The bit is used to clear interrupt of L1-ICache0 preload-operation error.
         */
        uint32_t l1_icache0_pld_err_int_clr:1;
        /** l1_icache1_pld_err_int_clr : WT; bitpos: [8]; default: 0;
         *  The bit is used to clear interrupt of L1-ICache1 preload-operation error.
         */
        uint32_t l1_icache1_pld_err_int_clr:1;
        uint32_t reserved_9:2;
        /** l1_dcache_pld_err_int_clr : WT; bitpos: [11]; default: 0;
         *  The bit is used to clear interrupt of L1-DCache preload-operation error.
         */
        uint32_t l1_dcache_pld_err_int_clr:1;
        uint32_t reserved_12:1;
        /** sync_err_int_clr : WT; bitpos: [13]; default: 0;
         *  The bit is used to clear interrupt of Cache sync-operation error.
         */
        uint32_t sync_err_int_clr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_clr_reg_t;

/** Type of l1_cache_sync_preload_int_raw register
 *  Sync Preload operation Interrupt raw register
 */
typedef union {
    struct {
        /** l1_icache0_pld_done_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-ICache0 preload-operation is
         *  done.
         */
        uint32_t l1_icache0_pld_done_int_raw:1;
        /** l1_icache1_pld_done_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-ICache1 preload-operation is
         *  done.
         */
        uint32_t l1_icache1_pld_done_int_raw:1;
        uint32_t reserved_2:2;
        /** l1_dcache_pld_done_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-DCache preload-operation is
         *  done.
         */
        uint32_t l1_dcache_pld_done_int_raw:1;
        uint32_t reserved_5:1;
        /** sync_done_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw bit of the interrupt that occurs only when Cache sync-operation is done.
         */
        uint32_t sync_done_int_raw:1;
        /** l1_icache0_pld_err_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-ICache0 preload-operation
         *  error occurs.
         */
        uint32_t l1_icache0_pld_err_int_raw:1;
        /** l1_icache1_pld_err_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-ICache1 preload-operation
         *  error occurs.
         */
        uint32_t l1_icache1_pld_err_int_raw:1;
        uint32_t reserved_9:2;
        /** l1_dcache_pld_err_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-DCache preload-operation
         *  error occurs.
         */
        uint32_t l1_dcache_pld_err_int_raw:1;
        uint32_t reserved_12:1;
        /** sync_err_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw bit of the interrupt that occurs only when Cache sync-operation error
         *  occurs.
         */
        uint32_t sync_err_int_raw:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_raw_reg_t;

/** Type of l1_cache_sync_preload_int_st register
 *  L1-Cache Access Fail Interrupt status register
 */
typedef union {
    struct {
        /** l1_icache0_pld_done_int_st : RO; bitpos: [0]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when L1-ICache0
         *  preload-operation is done.
         */
        uint32_t l1_icache0_pld_done_int_st:1;
        /** l1_icache1_pld_done_int_st : RO; bitpos: [1]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when L1-ICache1
         *  preload-operation is done.
         */
        uint32_t l1_icache1_pld_done_int_st:1;
        uint32_t reserved_2:2;
        /** l1_dcache_pld_done_int_st : RO; bitpos: [4]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when L1-DCache
         *  preload-operation is done.
         */
        uint32_t l1_dcache_pld_done_int_st:1;
        uint32_t reserved_5:1;
        /** sync_done_int_st : RO; bitpos: [6]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when Cache
         *  sync-operation is done.
         */
        uint32_t sync_done_int_st:1;
        /** l1_icache0_pld_err_int_st : RO; bitpos: [7]; default: 0;
         *  The bit indicates the status of the interrupt of L1-ICache0 preload-operation error.
         */
        uint32_t l1_icache0_pld_err_int_st:1;
        /** l1_icache1_pld_err_int_st : RO; bitpos: [8]; default: 0;
         *  The bit indicates the status of the interrupt of L1-ICache1 preload-operation error.
         */
        uint32_t l1_icache1_pld_err_int_st:1;
        uint32_t reserved_9:2;
        /** l1_dcache_pld_err_int_st : RO; bitpos: [11]; default: 0;
         *  The bit indicates the status of the interrupt of L1-DCache preload-operation error.
         */
        uint32_t l1_dcache_pld_err_int_st:1;
        uint32_t reserved_12:1;
        /** sync_err_int_st : RO; bitpos: [13]; default: 0;
         *  The bit indicates the status of the interrupt of Cache sync-operation error.
         */
        uint32_t sync_err_int_st:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_st_reg_t;


/** Group: Cache Access Fail Configuration register */
/** Type of l1_cache_acs_fail_ctrl register
 *  Cache Access Fail Configuration register
 */
typedef union {
    struct {
        /** l1_icache0_acs_fail_check_mode : R/W; bitpos: [0]; default: 0;
         *  The bit is used to configure l1 icache0 access fail check mode. 0: the access fail
         *  is not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_icache0_acs_fail_check_mode:1;
        /** l1_icache1_acs_fail_check_mode : R/W; bitpos: [1]; default: 0;
         *  The bit is used to configure l1 icache1 access fail check mode. 0: the access fail
         *  is not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_icache1_acs_fail_check_mode:1;
        uint32_t reserved_2:2;
        /** l1_dcache_acs_fail_check_mode : R/W; bitpos: [4]; default: 0;
         *  The bit is used to configure l1 dcache access fail check mode. 0: the access fail
         *  is not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_dcache_acs_fail_check_mode:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_ctrl_reg_t;


/** Group: Access Statistics registers */
/** Type of l1_cache_acs_cnt_ctrl register
 *  Cache Access Counter enable and clear register
 */
typedef union {
    struct {
        /** l1_ibus0_cnt_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable ibus0 counter in L1-ICache0.
         */
        uint32_t l1_ibus0_cnt_ena:1;
        /** l1_ibus1_cnt_ena : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable ibus1 counter in L1-ICache1.
         */
        uint32_t l1_ibus1_cnt_ena:1;
        uint32_t reserved_2:2;
        /** l1_dbus0_cnt_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable dbus0 counter in L1-DCache.
         */
        uint32_t l1_dbus0_cnt_ena:1;
        /** l1_dbus1_cnt_ena : R/W; bitpos: [5]; default: 0;
         *  The bit is used to enable dbus1 counter in L1-DCache.
         */
        uint32_t l1_dbus1_cnt_ena:1;
        uint32_t reserved_6:10;
        /** l1_ibus0_cnt_clr : WT; bitpos: [16]; default: 0;
         *  The bit is used to clear ibus0 counter in L1-ICache0.
         */
        uint32_t l1_ibus0_cnt_clr:1;
        /** l1_ibus1_cnt_clr : WT; bitpos: [17]; default: 0;
         *  The bit is used to clear ibus1 counter in L1-ICache1.
         */
        uint32_t l1_ibus1_cnt_clr:1;
        uint32_t reserved_18:2;
        /** l1_dbus0_cnt_clr : WT; bitpos: [20]; default: 0;
         *  The bit is used to clear dbus0 counter in L1-DCache.
         */
        uint32_t l1_dbus0_cnt_clr:1;
        /** l1_dbus1_cnt_clr : WT; bitpos: [21]; default: 0;
         *  The bit is used to clear dbus1 counter in L1-DCache.
         */
        uint32_t l1_dbus1_cnt_clr:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_ctrl_reg_t;

/** Type of l1_ibus0_acs_hit_cnt register
 *  L1-ICache bus0 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus0_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus0_acs_hit_cnt_reg_t;

/** Type of l1_ibus0_acs_miss_cnt register
 *  L1-ICache bus0 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus0_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus0_acs_miss_cnt_reg_t;

/** Type of l1_ibus0_acs_conflict_cnt register
 *  L1-ICache bus0 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus0_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus0_acs_conflict_cnt_reg_t;

/** Type of l1_ibus0_acs_nxtlvl_rd_cnt register
 *  L1-ICache bus0 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus0_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-ICache accesses L2-Cache due to
         *  bus0 accessing L1-ICache0.
         */
        uint32_t l1_ibus0_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus0_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_ibus1_acs_hit_cnt register
 *  L1-ICache bus1 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus1_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus1_acs_hit_cnt_reg_t;

/** Type of l1_ibus1_acs_miss_cnt register
 *  L1-ICache bus1 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus1_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus1_acs_miss_cnt_reg_t;

/** Type of l1_ibus1_acs_conflict_cnt register
 *  L1-ICache bus1 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus1_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus1_acs_conflict_cnt_reg_t;

/** Type of l1_ibus1_acs_nxtlvl_rd_cnt register
 *  L1-ICache bus1 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus1_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-ICache accesses L2-Cache due to
         *  bus1 accessing L1-ICache1.
         */
        uint32_t l1_ibus1_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus1_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_dbus0_acs_hit_cnt register
 *  L1-DCache bus0 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus0_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus0_acs_hit_cnt_reg_t;

/** Type of l1_dbus0_acs_miss_cnt register
 *  L1-DCache bus0 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus0_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus0_acs_miss_cnt_reg_t;

/** Type of l1_dbus0_acs_conflict_cnt register
 *  L1-DCache bus0 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus0_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus0_acs_conflict_cnt_reg_t;

/** Type of l1_dbus0_acs_nxtlvl_rd_cnt register
 *  L1-DCache bus0 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus0_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-DCache accesses L2-Cache due to
         *  bus0 accessing L1-DCache.
         */
        uint32_t l1_dbus0_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus0_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_dbus0_acs_nxtlvl_wr_cnt register
 *  L1-DCache bus0 WB-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus0_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when bus0 accesses L1-DCache.
         */
        uint32_t l1_dbus0_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus0_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l1_dbus1_acs_hit_cnt register
 *  L1-DCache bus1 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus1_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_hit_cnt_reg_t;

/** Type of l1_dbus1_acs_miss_cnt register
 *  L1-DCache bus1 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus1_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_miss_cnt_reg_t;

/** Type of l1_dbus1_acs_conflict_cnt register
 *  L1-DCache bus1 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus1_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_conflict_cnt_reg_t;

/** Type of l1_dbus1_acs_nxtlvl_rd_cnt register
 *  L1-DCache bus1 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus1_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-DCache accesses L2-Cache due to
         *  bus1 accessing L1-DCache.
         */
        uint32_t l1_dbus1_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_dbus1_acs_nxtlvl_wr_cnt register
 *  L1-DCache bus1 WB-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus1_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when bus1 accesses L1-DCache.
         */
        uint32_t l1_dbus1_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_nxtlvl_wr_cnt_reg_t;


/** Group: Access Fail Debug registers */
/** Type of l1_icache0_acs_fail_id_attr register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
typedef union {
    struct {
        /** l1_icache0_fail_id : RO; bitpos: [15:0]; default: 0;
         *  The register records the ID of fail-access when cache0 accesses L1-ICache.
         */
        uint32_t l1_icache0_fail_id:16;
        /** l1_icache0_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  The register records the attribution of fail-access when cache0 accesses L1-ICache.
         */
        uint32_t l1_icache0_fail_attr:16;
    };
    uint32_t val;
} cache_l1_icache0_acs_fail_id_attr_reg_t;

/** Type of l1_icache0_acs_fail_addr register
 *  L1-ICache0 Access Fail Address information register
 */
typedef union {
    struct {
        /** l1_icache0_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  The register records the address of fail-access when cache0 accesses L1-ICache.
         */
        uint32_t l1_icache0_fail_addr:32;
    };
    uint32_t val;
} cache_l1_icache0_acs_fail_addr_reg_t;

/** Type of l1_icache1_acs_fail_id_attr register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
typedef union {
    struct {
        /** l1_icache1_fail_id : RO; bitpos: [15:0]; default: 0;
         *  The register records the ID of fail-access when cache1 accesses L1-ICache.
         */
        uint32_t l1_icache1_fail_id:16;
        /** l1_icache1_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  The register records the attribution of fail-access when cache1 accesses L1-ICache.
         */
        uint32_t l1_icache1_fail_attr:16;
    };
    uint32_t val;
} cache_l1_icache1_acs_fail_id_attr_reg_t;

/** Type of l1_icache1_acs_fail_addr register
 *  L1-ICache0 Access Fail Address information register
 */
typedef union {
    struct {
        /** l1_icache1_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  The register records the address of fail-access when cache1 accesses L1-ICache.
         */
        uint32_t l1_icache1_fail_addr:32;
    };
    uint32_t val;
} cache_l1_icache1_acs_fail_addr_reg_t;

/** Type of l1_dcache_acs_fail_id_attr register
 *  L1-DCache Access Fail ID/attribution information register
 */
typedef union {
    struct {
        /** l1_dcache_fail_id : RO; bitpos: [15:0]; default: 0;
         *  The register records the ID of fail-access when cache accesses L1-DCache.
         */
        uint32_t l1_dcache_fail_id:16;
        /** l1_dcache_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  The register records the attribution of fail-access when cache accesses L1-DCache.
         */
        uint32_t l1_dcache_fail_attr:16;
    };
    uint32_t val;
} cache_l1_dcache_acs_fail_id_attr_reg_t;

/** Type of l1_dcache_acs_fail_addr register
 *  L1-DCache Access Fail Address information register
 */
typedef union {
    struct {
        /** l1_dcache_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  The register records the address of fail-access when cache accesses L1-DCache.
         */
        uint32_t l1_dcache_fail_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_acs_fail_addr_reg_t;


/** Group: Operation Exception registers */
/** Type of l1_cache_sync_preload_exception register
 *  Cache Sync/Preload Operation exception register
 */
typedef union {
    struct {
        /** l1_icache0_pld_err_code : RO; bitpos: [1:0]; default: 0;
         *  The value 2 is Only available which means preload size is error in L1-ICache0.
         */
        uint32_t l1_icache0_pld_err_code:2;
        /** l1_icache1_pld_err_code : RO; bitpos: [3:2]; default: 0;
         *  The value 2 is Only available which means preload size is error in L1-ICache1.
         */
        uint32_t l1_icache1_pld_err_code:2;
        uint32_t reserved_4:4;
        /** l1_dcache_pld_err_code : RO; bitpos: [9:8]; default: 0;
         *  The value 2 is Only available which means preload size is error in L1-DCache.
         */
        uint32_t l1_dcache_pld_err_code:2;
        uint32_t reserved_10:2;
        /** sync_err_code : RO; bitpos: [13:12]; default: 0;
         *  The values 0-2 are available which means sync map, command conflict and size are
         *  error in Cache System.
         */
        uint32_t sync_err_code:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_exception_reg_t;


/** Group: Sync Reset control and configuration registers */
/** Type of l1_cache_sync_rst_ctrl register
 *  Cache Sync Reset control register
 */
typedef union {
    struct {
        /** l1_icache0_sync_rst : R/W; bitpos: [0]; default: 0;
         *  set this bit to reset sync-logic inside L1-ICache0. Recommend that this should only
         *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
         */
        uint32_t l1_icache0_sync_rst:1;
        /** l1_icache1_sync_rst : R/W; bitpos: [1]; default: 0;
         *  set this bit to reset sync-logic inside L1-ICache1. Recommend that this should only
         *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
         */
        uint32_t l1_icache1_sync_rst:1;
        uint32_t reserved_2:2;
        /** l1_dcache_sync_rst : R/W; bitpos: [4]; default: 0;
         *  set this bit to reset sync-logic inside L1-DCache. Recommend that this should only
         *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
         */
        uint32_t l1_dcache_sync_rst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_sync_rst_ctrl_reg_t;


/** Group: Preload Reset control and configuration registers */
/** Type of l1_cache_preload_rst_ctrl register
 *  Cache Preload Reset control register
 */
typedef union {
    struct {
        /** l1_icache0_pld_rst : R/W; bitpos: [0]; default: 0;
         *  set this bit to reset preload-logic inside L1-ICache0. Recommend that this should
         *  only be used to initialize preload-logic when some fatal error of preload-logic
         *  occurs.
         */
        uint32_t l1_icache0_pld_rst:1;
        /** l1_icache1_pld_rst : R/W; bitpos: [1]; default: 0;
         *  set this bit to reset preload-logic inside L1-ICache1. Recommend that this should
         *  only be used to initialize preload-logic when some fatal error of preload-logic
         *  occurs.
         */
        uint32_t l1_icache1_pld_rst:1;
        uint32_t reserved_2:2;
        /** l1_dcache_pld_rst : R/W; bitpos: [4]; default: 0;
         *  set this bit to reset preload-logic inside L1-DCache. Recommend that this should
         *  only be used to initialize preload-logic when some fatal error of preload-logic
         *  occurs.
         */
        uint32_t l1_dcache_pld_rst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_preload_rst_ctrl_reg_t;


/** Group: Autoload buffer clear control and configuration registers */
/** Type of l1_cache_autoload_buf_clr_ctrl register
 *  Cache Autoload buffer clear control register
 */
typedef union {
    struct {
        /** l1_icache0_ald_buf_clr : R/W; bitpos: [0]; default: 0;
         *  set this bit to clear autoload-buffer inside L1-ICache0. If this bit is active,
         *  autoload will not work in L1-ICache0. This bit should not be active when autoload
         *  works in L1-ICache0.
         */
        uint32_t l1_icache0_ald_buf_clr:1;
        /** l1_icache1_ald_buf_clr : R/W; bitpos: [1]; default: 0;
         *  set this bit to clear autoload-buffer inside L1-ICache1. If this bit is active,
         *  autoload will not work in L1-ICache1. This bit should not be active when autoload
         *  works in L1-ICache1.
         */
        uint32_t l1_icache1_ald_buf_clr:1;
        uint32_t reserved_2:2;
        /** l1_dcache_ald_buf_clr : R/W; bitpos: [4]; default: 0;
         *  set this bit to clear autoload-buffer inside L1-DCache. If this bit is active,
         *  autoload will not work in L1-DCache. This bit should not be active when autoload
         *  works in L1-DCache.
         */
        uint32_t l1_dcache_ald_buf_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_autoload_buf_clr_ctrl_reg_t;


/** Group: Unallocate request buffer clear registers */
/** Type of l1_unallocate_buffer_clear register
 *  Unallocate request buffer clear registers
 */
typedef union {
    struct {
        /** l1_icache0_unalloc_clr : R/W; bitpos: [0]; default: 0;
         *  The bit is used to clear the unallocate request buffer of l1 icache0 where the
         *  unallocate request is responded but not completed.
         */
        uint32_t l1_icache0_unalloc_clr:1;
        /** l1_icache1_unalloc_clr : R/W; bitpos: [1]; default: 0;
         *  The bit is used to clear the unallocate request buffer of l1 icache1 where the
         *  unallocate request is responded but not completed.
         */
        uint32_t l1_icache1_unalloc_clr:1;
        uint32_t reserved_2:2;
        /** l1_dcache_unalloc_clr : R/W; bitpos: [4]; default: 0;
         *  The bit is used to clear the unallocate request buffer of l1 dcache where the
         *  unallocate request is responded but not completed.
         */
        uint32_t l1_dcache_unalloc_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_unallocate_buffer_clear_reg_t;


/** Group: Tag and Data Memory Access Control and configuration register */
/** Type of l1_cache_object_ctrl register
 *  Cache Tag and Data memory Object control register
 */
typedef union {
    struct {
        /** l1_icache0_tag_object : R/W; bitpos: [0]; default: 0;
         *  Set this bit to set L1-ICache0 tag memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_icache0_tag_object:1;
        /** l1_icache1_tag_object : R/W; bitpos: [1]; default: 0;
         *  Set this bit to set L1-ICache1 tag memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_icache1_tag_object:1;
        uint32_t reserved_2:2;
        /** l1_dcache_tag_object : R/W; bitpos: [4]; default: 0;
         *  Set this bit to set L1-DCache tag memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_dcache_tag_object:1;
        uint32_t reserved_5:1;
        /** l1_icache0_mem_object : R/W; bitpos: [6]; default: 0;
         *  Set this bit to set L1-ICache0 data memory as object. This bit should be onehot
         *  with the others fields inside this register.
         */
        uint32_t l1_icache0_mem_object:1;
        /** l1_icache1_mem_object : R/W; bitpos: [7]; default: 0;
         *  Set this bit to set L1-ICache1 data memory as object. This bit should be onehot
         *  with the others fields inside this register.
         */
        uint32_t l1_icache1_mem_object:1;
        uint32_t reserved_8:2;
        /** l1_dcache_mem_object : R/W; bitpos: [10]; default: 0;
         *  Set this bit to set L1-DCache data memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_dcache_mem_object:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} cache_l1_cache_object_ctrl_reg_t;

/** Type of l1_cache_way_object register
 *  Cache Tag and Data memory way register
 */
typedef union {
    struct {
        /** l1_cache_way_object : R/W; bitpos: [2:0]; default: 0;
         *  Set this bits to select which way of the tag-object will be accessed. 0: way0, 1:
         *  way1, 2: way2, 3: way3, ?, 7: way7.
         */
        uint32_t l1_cache_way_object:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_l1_cache_way_object_reg_t;

/** Type of l1_cache_addr register
 *  Cache address register
 */
typedef union {
    struct {
        /** l1_cache_addr : R/W; bitpos: [31:0]; default: 1073741824;
         *  Those bits stores the address which will decide where inside the specified tag
         *  memory object will be accessed.
         */
        uint32_t l1_cache_addr:32;
    };
    uint32_t val;
} cache_l1_cache_addr_reg_t;

/** Type of l1_cache_debug_bus register
 *  Cache Tag/data memory content register
 */
typedef union {
    struct {
        /** l1_cache_debug_bus : R/W; bitpos: [31:0]; default: 620;
         *  This is a constant place where we can write data to or read data from the tag/data
         *  memory on the specified cache.
         */
        uint32_t l1_cache_debug_bus:32;
    };
    uint32_t val;
} cache_l1_cache_debug_bus_reg_t;


/** Group: Clock Gate Control and configuration register */
/** Type of clock_gate register
 *  Clock gate control register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable clock gate when access all registers in this module.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cache_clock_gate_reg_t;


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


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37765696;
         *  version control register. Note that this default value stored is the latest date
         *  when the hardware logic was updated.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_date_reg_t;


typedef struct {
    volatile cache_l1_icache_ctrl_reg_t l1_icache_ctrl;
    volatile cache_l1_dcache_ctrl_reg_t l1_dcache_ctrl;
    volatile cache_l1_bypass_cache_conf_reg_t l1_bypass_cache_conf;
    volatile cache_l1_cache_atomic_conf_reg_t l1_cache_atomic_conf;
    uint32_t reserved_010[2];
    volatile cache_l1_icache_cachesize_conf_reg_t l1_icache_cachesize_conf;
    volatile cache_l1_icache_blocksize_conf_reg_t l1_icache_blocksize_conf;
    volatile cache_l1_dcache_cachesize_conf_reg_t l1_dcache_cachesize_conf;
    volatile cache_l1_dcache_blocksize_conf_reg_t l1_dcache_blocksize_conf;
    volatile cache_l1_cache_wrap_around_ctrl_reg_t l1_cache_wrap_around_ctrl;
    volatile cache_l1_cache_miss_access_ctrl_reg_t l1_cache_miss_access_ctrl;
    volatile cache_l1_cache_freeze_ctrl_reg_t l1_cache_freeze_ctrl;
    volatile cache_l1_cache_data_mem_acs_conf_reg_t l1_cache_data_mem_acs_conf;
    volatile cache_l1_cache_tag_mem_acs_conf_reg_t l1_cache_tag_mem_acs_conf;
    volatile cache_l1_icache0_prelock_conf_reg_t l1_icache0_prelock_conf;
    volatile cache_l1_icache0_prelock_sct0_addr_reg_t l1_icache0_prelock_sct0_addr;
    volatile cache_l1_icache0_prelock_sct1_addr_reg_t l1_icache0_prelock_sct1_addr;
    volatile cache_l1_icache0_prelock_sct_size_reg_t l1_icache0_prelock_sct_size;
    volatile cache_l1_icache1_prelock_conf_reg_t l1_icache1_prelock_conf;
    volatile cache_l1_icache1_prelock_sct0_addr_reg_t l1_icache1_prelock_sct0_addr;
    volatile cache_l1_icache1_prelock_sct1_addr_reg_t l1_icache1_prelock_sct1_addr;
    volatile cache_l1_icache1_prelock_sct_size_reg_t l1_icache1_prelock_sct_size;
    uint32_t reserved_05c[8];
    volatile cache_l1_dcache_prelock_conf_reg_t l1_dcache_prelock_conf;
    volatile cache_l1_dcache_prelock_sct0_addr_reg_t l1_dcache_prelock_sct0_addr;
    volatile cache_l1_dcache_prelock_sct1_addr_reg_t l1_dcache_prelock_sct1_addr;
    volatile cache_l1_dcache_prelock_sct_size_reg_t l1_dcache_prelock_sct_size;
    volatile cache_lock_ctrl_reg_t lock_ctrl;
    volatile cache_lock_map_reg_t lock_map;
    volatile cache_lock_addr_reg_t lock_addr;
    volatile cache_lock_size_reg_t lock_size;
    volatile cache_sync_ctrl_reg_t sync_ctrl;
    volatile cache_sync_map_reg_t sync_map;
    volatile cache_sync_addr_reg_t sync_addr;
    volatile cache_sync_size_reg_t sync_size;
    volatile cache_l1_icache0_preload_ctrl_reg_t l1_icache0_preload_ctrl;
    volatile cache_l1_icache0_preload_addr_reg_t l1_icache0_preload_addr;
    volatile cache_l1_icache0_preload_size_reg_t l1_icache0_preload_size;
    volatile cache_l1_icache1_preload_ctrl_reg_t l1_icache1_preload_ctrl;
    volatile cache_l1_icache1_preload_addr_reg_t l1_icache1_preload_addr;
    volatile cache_l1_icache1_preload_size_reg_t l1_icache1_preload_size;
    uint32_t reserved_0c4[6];
    volatile cache_l1_dcache_preload_ctrl_reg_t l1_dcache_preload_ctrl;
    volatile cache_l1_dcache_preload_addr_reg_t l1_dcache_preload_addr;
    volatile cache_l1_dcache_preload_size_reg_t l1_dcache_preload_size;
    volatile cache_l1_icache0_autoload_ctrl_reg_t l1_icache0_autoload_ctrl;
    volatile cache_l1_icache0_autoload_sct0_addr_reg_t l1_icache0_autoload_sct0_addr;
    volatile cache_l1_icache0_autoload_sct0_size_reg_t l1_icache0_autoload_sct0_size;
    volatile cache_l1_icache0_autoload_sct1_addr_reg_t l1_icache0_autoload_sct1_addr;
    volatile cache_l1_icache0_autoload_sct1_size_reg_t l1_icache0_autoload_sct1_size;
    volatile cache_l1_icache1_autoload_ctrl_reg_t l1_icache1_autoload_ctrl;
    volatile cache_l1_icache1_autoload_sct0_addr_reg_t l1_icache1_autoload_sct0_addr;
    volatile cache_l1_icache1_autoload_sct0_size_reg_t l1_icache1_autoload_sct0_size;
    volatile cache_l1_icache1_autoload_sct1_addr_reg_t l1_icache1_autoload_sct1_addr;
    volatile cache_l1_icache1_autoload_sct1_size_reg_t l1_icache1_autoload_sct1_size;
    uint32_t reserved_110[10];
    volatile cache_l1_dcache_autoload_ctrl_reg_t l1_dcache_autoload_ctrl;
    volatile cache_l1_dcache_autoload_sct0_addr_reg_t l1_dcache_autoload_sct0_addr;
    volatile cache_l1_dcache_autoload_sct0_size_reg_t l1_dcache_autoload_sct0_size;
    volatile cache_l1_dcache_autoload_sct1_addr_reg_t l1_dcache_autoload_sct1_addr;
    volatile cache_l1_dcache_autoload_sct1_size_reg_t l1_dcache_autoload_sct1_size;
    volatile cache_l1_dcache_autoload_sct2_addr_reg_t l1_dcache_autoload_sct2_addr;
    volatile cache_l1_dcache_autoload_sct2_size_reg_t l1_dcache_autoload_sct2_size;
    volatile cache_l1_dcache_autoload_sct3_addr_reg_t l1_dcache_autoload_sct3_addr;
    volatile cache_l1_dcache_autoload_sct3_size_reg_t l1_dcache_autoload_sct3_size;
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
    volatile cache_l1_ibus0_acs_hit_cnt_reg_t l1_ibus0_acs_hit_cnt;
    volatile cache_l1_ibus0_acs_miss_cnt_reg_t l1_ibus0_acs_miss_cnt;
    volatile cache_l1_ibus0_acs_conflict_cnt_reg_t l1_ibus0_acs_conflict_cnt;
    volatile cache_l1_ibus0_acs_nxtlvl_rd_cnt_reg_t l1_ibus0_acs_nxtlvl_rd_cnt;
    volatile cache_l1_ibus1_acs_hit_cnt_reg_t l1_ibus1_acs_hit_cnt;
    volatile cache_l1_ibus1_acs_miss_cnt_reg_t l1_ibus1_acs_miss_cnt;
    volatile cache_l1_ibus1_acs_conflict_cnt_reg_t l1_ibus1_acs_conflict_cnt;
    volatile cache_l1_ibus1_acs_nxtlvl_rd_cnt_reg_t l1_ibus1_acs_nxtlvl_rd_cnt;
    uint32_t reserved_1a4[8];
    volatile cache_l1_dbus0_acs_hit_cnt_reg_t l1_dbus0_acs_hit_cnt;
    volatile cache_l1_dbus0_acs_miss_cnt_reg_t l1_dbus0_acs_miss_cnt;
    volatile cache_l1_dbus0_acs_conflict_cnt_reg_t l1_dbus0_acs_conflict_cnt;
    volatile cache_l1_dbus0_acs_nxtlvl_rd_cnt_reg_t l1_dbus0_acs_nxtlvl_rd_cnt;
    volatile cache_l1_dbus0_acs_nxtlvl_wr_cnt_reg_t l1_dbus0_acs_nxtlvl_wr_cnt;
    volatile cache_l1_dbus1_acs_hit_cnt_reg_t l1_dbus1_acs_hit_cnt;
    volatile cache_l1_dbus1_acs_miss_cnt_reg_t l1_dbus1_acs_miss_cnt;
    volatile cache_l1_dbus1_acs_conflict_cnt_reg_t l1_dbus1_acs_conflict_cnt;
    volatile cache_l1_dbus1_acs_nxtlvl_rd_cnt_reg_t l1_dbus1_acs_nxtlvl_rd_cnt;
    volatile cache_l1_dbus1_acs_nxtlvl_wr_cnt_reg_t l1_dbus1_acs_nxtlvl_wr_cnt;
    uint32_t reserved_1ec[10];
    volatile cache_l1_icache0_acs_fail_id_attr_reg_t l1_icache0_acs_fail_id_attr;
    volatile cache_l1_icache0_acs_fail_addr_reg_t l1_icache0_acs_fail_addr;
    volatile cache_l1_icache1_acs_fail_id_attr_reg_t l1_icache1_acs_fail_id_attr;
    volatile cache_l1_icache1_acs_fail_addr_reg_t l1_icache1_acs_fail_addr;
    uint32_t reserved_224[4];
    volatile cache_l1_dcache_acs_fail_id_attr_reg_t l1_dcache_acs_fail_id_attr;
    volatile cache_l1_dcache_acs_fail_addr_reg_t l1_dcache_acs_fail_addr;
    volatile cache_l1_cache_sync_preload_int_ena_reg_t l1_cache_sync_preload_int_ena;
    volatile cache_l1_cache_sync_preload_int_clr_reg_t l1_cache_sync_preload_int_clr;
    volatile cache_l1_cache_sync_preload_int_raw_reg_t l1_cache_sync_preload_int_raw;
    volatile cache_l1_cache_sync_preload_int_st_reg_t l1_cache_sync_preload_int_st;
    volatile cache_l1_cache_sync_preload_exception_reg_t l1_cache_sync_preload_exception;
    volatile cache_l1_cache_sync_rst_ctrl_reg_t l1_cache_sync_rst_ctrl;
    volatile cache_l1_cache_preload_rst_ctrl_reg_t l1_cache_preload_rst_ctrl;
    volatile cache_l1_cache_autoload_buf_clr_ctrl_reg_t l1_cache_autoload_buf_clr_ctrl;
    volatile cache_l1_unallocate_buffer_clear_reg_t l1_unallocate_buffer_clear;
    volatile cache_l1_cache_object_ctrl_reg_t l1_cache_object_ctrl;
    volatile cache_l1_cache_way_object_reg_t l1_cache_way_object;
    volatile cache_l1_cache_addr_reg_t l1_cache_addr;
    volatile cache_l1_cache_debug_bus_reg_t l1_cache_debug_bus;
    uint32_t reserved_270[89];
    volatile cache_clock_gate_reg_t clock_gate;
    volatile cache_trace_ena_reg_t trace_ena;
    uint32_t reserved_3dc[8];
    volatile cache_date_reg_t date;
} cache_dev_t;

extern cache_dev_t CACHE;

#ifndef __cplusplus
_Static_assert(sizeof(cache_dev_t) == 0x400, "Invalid size of cache_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
