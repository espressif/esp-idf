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

/** Group: Control and configuration registers */
/** Type of l1_icache_ctrl register
 *  L1 instruction Cache(L1-ICache) control register
 */
typedef union {
    struct {
        /** l1_icache_shut_ibus0 : HRO; bitpos: [0]; default: 0;
         *  The bit is used to disable core0 ibus access L1-ICache, 0: enable, 1: disable
         */
        uint32_t l1_icache_shut_ibus0:1;
        /** l1_icache_shut_ibus1 : HRO; bitpos: [1]; default: 0;
         *  The bit is used to disable core1 ibus access L1-ICache, 0: enable, 1: disable
         */
        uint32_t l1_icache_shut_ibus1:1;
        /** l1_icache_shut_ibus2 : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache_shut_ibus2:1;
        /** l1_icache_shut_ibus3 : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache_shut_ibus3:1;
        uint32_t reserved_4:4;
        /** l1_icache_undef_op : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache_undef_op:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l1_icache_ctrl_reg_t;

/** Type of l1_cache_ctrl register
 *  L1 data Cache(L1-Cache) control register
 */
typedef union {
    struct {
        /** l1_cache_shut_bus0 : R/W; bitpos: [0]; default: 0;
         *  The bit is used to disable core0 bus0 access L1-Cache, 0: enable, 1: disable
         */
        uint32_t l1_cache_shut_bus0:1;
        /** l1_cache_shut_bus1 : R/W; bitpos: [1]; default: 0;
         *  The bit is used to disable core0 bus1 access L1-Cache, 0: enable, 1: disable
         */
        uint32_t l1_cache_shut_bus1:1;
        /** l1_cache_shut_dbus2 : HRO; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_cache_shut_dbus2:1;
        /** l1_cache_shut_dbus3 : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_cache_shut_dbus3:1;
        /** l1_cache_shut_dma : HRO; bitpos: [4]; default: 0;
         *  The bit is used to disable DMA access L1-Cache, 0: enable, 1: disable
         */
        uint32_t l1_cache_shut_dma:1;
        uint32_t reserved_5:3;
        /** l1_cache_undef_op : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t l1_cache_undef_op:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l1_cache_ctrl_reg_t;

/** Type of l2_cache_ctrl register
 *  L2 Cache(L2-Cache) control register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** l2_cache_shut_dma : HRO; bitpos: [4]; default: 0;
         *  The bit is used to disable DMA access L2-Cache, 0: enable, 1: disable
         */
        uint32_t l2_cache_shut_dma:1;
        uint32_t reserved_5:3;
        /** l2_cache_undef_op : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t l2_cache_undef_op:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l2_cache_ctrl_reg_t;


/** Group: Bypass Cache Control and configuration registers */
/** Type of l1_bypass_cache_conf register
 *  Bypass Cache configure register
 */
typedef union {
    struct {
        /** bypass_l1_icache0_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable bypass L1-ICache0. 0: disable bypass, 1: enable bypass.
         */
        uint32_t bypass_l1_icache0_en:1;
        /** bypass_l1_icache1_en : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable bypass L1-ICache1. 0: disable bypass, 1: enable bypass.
         */
        uint32_t bypass_l1_icache1_en:1;
        /** bypass_l1_icache2_en : HRO; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t bypass_l1_icache2_en:1;
        /** bypass_l1_icache3_en : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t bypass_l1_icache3_en:1;
        /** bypass_l1_dcache_en : HRO; bitpos: [4]; default: 0;
         *  The bit is used to enable bypass L1-DCache. 0: disable bypass, 1: enable bypass.
         */
        uint32_t bypass_l1_dcache_en:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_bypass_cache_conf_reg_t;

/** Type of l2_bypass_cache_conf register
 *  Bypass Cache configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** bypass_l2_cache_en : HRO; bitpos: [5]; default: 0;
         *  The bit is used to enable bypass L2-Cache. 0: disable bypass, 1: enable bypass.
         */
        uint32_t bypass_l2_cache_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_bypass_cache_conf_reg_t;


/** Group: Cache Atomic Control and configuration registers */
/** Type of l1_cache_atomic_conf register
 *  L1 Cache atomic feature configure register
 */
typedef union {
    struct {
        /** l1_cache_atomic_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable atomic feature on L1-Cache when multiple cores access
         *  L1-Cache.  1: disable, 1: enable.
         */
        uint32_t l1_cache_atomic_en:1;
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
        /** l1_icache_cachesize_256 : HRO; bitpos: [0]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 256 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_256:1;
        /** l1_icache_cachesize_512 : HRO; bitpos: [1]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 512 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_512:1;
        /** l1_icache_cachesize_1k : HRO; bitpos: [2]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 1k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_1k:1;
        /** l1_icache_cachesize_2k : HRO; bitpos: [3]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 2k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_2k:1;
        /** l1_icache_cachesize_4k : HRO; bitpos: [4]; default: 1;
         *  The field is used to configure cachesize of L1-ICache as 4k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_4k:1;
        /** l1_icache_cachesize_8k : HRO; bitpos: [5]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 8k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_8k:1;
        /** l1_icache_cachesize_16k : HRO; bitpos: [6]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 16k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_16k:1;
        /** l1_icache_cachesize_32k : HRO; bitpos: [7]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 32k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_32k:1;
        /** l1_icache_cachesize_64k : HRO; bitpos: [8]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 64k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_64k:1;
        /** l1_icache_cachesize_128k : HRO; bitpos: [9]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 128k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_128k:1;
        /** l1_icache_cachesize_256k : HRO; bitpos: [10]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 256k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_256k:1;
        /** l1_icache_cachesize_512k : HRO; bitpos: [11]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 512k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_512k:1;
        /** l1_icache_cachesize_1024k : HRO; bitpos: [12]; default: 0;
         *  The field is used to configure cachesize of L1-ICache as 1024k bytes. This field
         *  and all other fields within this register is onehot.
         */
        uint32_t l1_icache_cachesize_1024k:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cache_l1_icache_cachesize_conf_reg_t;

/** Type of l1_icache_blocksize_conf register
 *  L1 instruction Cache BlockSize mode configure register
 */
typedef union {
    struct {
        /** l1_icache_blocksize_8 : HRO; bitpos: [0]; default: 0;
         *  The field is used to configureblocksize of L1-ICache as 8 bytes. This field and all
         *  other fields within this register is onehot.
         */
        uint32_t l1_icache_blocksize_8:1;
        /** l1_icache_blocksize_16 : HRO; bitpos: [1]; default: 0;
         *  The field is used to configureblocksize of L1-ICache as 16 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_blocksize_16:1;
        /** l1_icache_blocksize_32 : HRO; bitpos: [2]; default: 0;
         *  The field is used to configureblocksize of L1-ICache as 32 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_blocksize_32:1;
        /** l1_icache_blocksize_64 : HRO; bitpos: [3]; default: 1;
         *  The field is used to configureblocksize of L1-ICache as 64 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_blocksize_64:1;
        /** l1_icache_blocksize_128 : HRO; bitpos: [4]; default: 0;
         *  The field is used to configureblocksize of L1-ICache as 128 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_blocksize_128:1;
        /** l1_icache_blocksize_256 : HRO; bitpos: [5]; default: 0;
         *  The field is used to configureblocksize of L1-ICache as 256 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_icache_blocksize_256:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_icache_blocksize_conf_reg_t;

/** Type of l1_cache_cachesize_conf register
 *  L1 data Cache CacheSize mode configure register
 */
typedef union {
    struct {
        /** l1_dcache_cachesize_256 : HRO; bitpos: [0]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 256 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_256:1;
        /** l1_dcache_cachesize_512 : HRO; bitpos: [1]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 512 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_512:1;
        /** l1_dcache_cachesize_1k : HRO; bitpos: [2]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 1k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_1k:1;
        /** l1_dcache_cachesize_2k : HRO; bitpos: [3]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 2k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_2k:1;
        /** l1_dcache_cachesize_4k : HRO; bitpos: [4]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 4k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_4k:1;
        /** l1_dcache_cachesize_8k : HRO; bitpos: [5]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 8k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_8k:1;
        /** l1_dcache_cachesize_16k : HRO; bitpos: [6]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 16k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_16k:1;
        /** l1_dcache_cachesize_32k : HRO; bitpos: [7]; default: 1;
         *  The field is used to configure cachesize of L1-DCache as 32k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_32k:1;
        /** l1_dcache_cachesize_64k : HRO; bitpos: [8]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 64k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_64k:1;
        /** l1_dcache_cachesize_128k : HRO; bitpos: [9]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 128k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_128k:1;
        /** l1_dcache_cachesize_256k : HRO; bitpos: [10]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 256k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_256k:1;
        /** l1_dcache_cachesize_512k : HRO; bitpos: [11]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 512k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_512k:1;
        /** l1_dcache_cachesize_1024k : HRO; bitpos: [12]; default: 0;
         *  The field is used to configure cachesize of L1-DCache as 1024k bytes. This field
         *  and all other fields within this register is onehot.
         */
        uint32_t l1_dcache_cachesize_1024k:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cache_l1_cache_cachesize_conf_reg_t;

/** Type of l1_cache_blocksize_conf register
 *  L1 data Cache BlockSize mode configure register
 */
typedef union {
    struct {
        /** l1_cache_blocksize_8 : HRO; bitpos: [0]; default: 0;
         *  The field is used to configureblocksize of L1-DCache as 8 bytes. This field and all
         *  other fields within this register is onehot.
         */
        uint32_t l1_cache_blocksize_8:1;
        /** l1_cache_blocksize_16 : HRO; bitpos: [1]; default: 0;
         *  The field is used to configureblocksize of L1-DCache as 16 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_cache_blocksize_16:1;
        /** l1_cache_blocksize_32 : HRO; bitpos: [2]; default: 1;
         *  The field is used to configureblocksize of L1-DCache as 32 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_cache_blocksize_32:1;
        /** l1_cache_blocksize_64 : HRO; bitpos: [3]; default: 0;
         *  The field is used to configureblocksize of L1-DCache as 64 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_cache_blocksize_64:1;
        /** l1_cache_blocksize_128 : HRO; bitpos: [4]; default: 0;
         *  The field is used to configureblocksize of L1-DCache as 128 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_cache_blocksize_128:1;
        /** l1_cache_blocksize_256 : HRO; bitpos: [5]; default: 0;
         *  The field is used to configureblocksize of L1-DCache as 256 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l1_cache_blocksize_256:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_blocksize_conf_reg_t;

/** Type of l2_cache_cachesize_conf register
 *  L2 Cache CacheSize mode configure register
 */
typedef union {
    struct {
        /** l2_cache_cachesize_256 : HRO; bitpos: [0]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 256 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_256:1;
        /** l2_cache_cachesize_512 : HRO; bitpos: [1]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 512 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_512:1;
        /** l2_cache_cachesize_1k : HRO; bitpos: [2]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 1k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_1k:1;
        /** l2_cache_cachesize_2k : HRO; bitpos: [3]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 2k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_2k:1;
        /** l2_cache_cachesize_4k : HRO; bitpos: [4]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 4k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_4k:1;
        /** l2_cache_cachesize_8k : HRO; bitpos: [5]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 8k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_8k:1;
        /** l2_cache_cachesize_16k : HRO; bitpos: [6]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 16k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_16k:1;
        /** l2_cache_cachesize_32k : HRO; bitpos: [7]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 32k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_32k:1;
        /** l2_cache_cachesize_64k : HRO; bitpos: [8]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 64k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_64k:1;
        /** l2_cache_cachesize_128k : HRO; bitpos: [9]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 128k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_128k:1;
        /** l2_cache_cachesize_256k : HRO; bitpos: [10]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 256k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_256k:1;
        /** l2_cache_cachesize_512k : HRO; bitpos: [11]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 512k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_512k:1;
        /** l2_cache_cachesize_1024k : HRO; bitpos: [12]; default: 0;
         *  The field is used to configure cachesize of L2-Cache as 1024k bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_cachesize_1024k:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cache_l2_cache_cachesize_conf_reg_t;

/** Type of l2_cache_blocksize_conf register
 *  L2 Cache BlockSize mode configure register
 */
typedef union {
    struct {
        /** l2_cache_blocksize_8 : HRO; bitpos: [0]; default: 0;
         *  The field is used to configureblocksize of L2-Cache as 8 bytes. This field and all
         *  other fields within this register is onehot.
         */
        uint32_t l2_cache_blocksize_8:1;
        /** l2_cache_blocksize_16 : HRO; bitpos: [1]; default: 0;
         *  The field is used to configureblocksize of L2-Cache as 16 bytes. This field and all
         *  other fields within this register is onehot.
         */
        uint32_t l2_cache_blocksize_16:1;
        /** l2_cache_blocksize_32 : HRO; bitpos: [2]; default: 0;
         *  The field is used to configureblocksize of L2-Cache as 32 bytes. This field and all
         *  other fields within this register is onehot.
         */
        uint32_t l2_cache_blocksize_32:1;
        /** l2_cache_blocksize_64 : HRO; bitpos: [3]; default: 0;
         *  The field is used to configureblocksize of L2-Cache as 64 bytes. This field and all
         *  other fields within this register is onehot.
         */
        uint32_t l2_cache_blocksize_64:1;
        /** l2_cache_blocksize_128 : HRO; bitpos: [4]; default: 0;
         *  The field is used to configureblocksize of L2-Cache as 128 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_blocksize_128:1;
        /** l2_cache_blocksize_256 : HRO; bitpos: [5]; default: 0;
         *  The field is used to configureblocksize of L2-Cache as 256 bytes. This field and
         *  all other fields within this register is onehot.
         */
        uint32_t l2_cache_blocksize_256:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_blocksize_conf_reg_t;


/** Group: Wrap Mode Control and configuration registers */
/** Type of l1_cache_wrap_around_ctrl register
 *  Cache wrap around control register
 */
typedef union {
    struct {
        /** l1_icache0_wrap : HRO; bitpos: [0]; default: 0;
         *  Set this bit as 1 to enable L1-ICache0 wrap around mode.
         */
        uint32_t l1_icache0_wrap:1;
        /** l1_icache1_wrap : HRO; bitpos: [1]; default: 0;
         *  Set this bit as 1 to enable L1-ICache1 wrap around mode.
         */
        uint32_t l1_icache1_wrap:1;
        /** l1_icache2_wrap : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_wrap:1;
        /** l1_icache3_wrap : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_wrap:1;
        /** l1_cache_wrap : R/W; bitpos: [4]; default: 0;
         *  Set this bit as 1 to enable L1-DCache wrap around mode.
         */
        uint32_t l1_cache_wrap:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_wrap_around_ctrl_reg_t;

/** Type of l2_cache_wrap_around_ctrl register
 *  Cache wrap around control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_wrap : HRO; bitpos: [5]; default: 0;
         *  Set this bit as 1 to enable L2-Cache wrap around mode.
         */
        uint32_t l2_cache_wrap:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_wrap_around_ctrl_reg_t;


/** Group: Early Restart Control registers */
/** Type of l1_cache_miss_access_ctrl register
 *  Cache wrap around control register
 */
typedef union {
    struct {
        /** l1_icache0_miss_disable_access : HRO; bitpos: [0]; default: 0;
         *  Set this bit as 1 to disable early restart of  L1-ICache0
         */
        uint32_t l1_icache0_miss_disable_access:1;
        /** l1_icache1_miss_disable_access : HRO; bitpos: [1]; default: 0;
         *  Set this bit as 1 to disable early restart of  L1-ICache1
         */
        uint32_t l1_icache1_miss_disable_access:1;
        /** l1_icache2_miss_disable_access : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_miss_disable_access:1;
        /** l1_icache3_miss_disable_access : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_miss_disable_access:1;
        /** l1_cache_miss_disable_access : R/W; bitpos: [4]; default: 0;
         *  Set this bit as 1 to disable early restart of L1-DCache
         */
        uint32_t l1_cache_miss_disable_access:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_miss_access_ctrl_reg_t;

/** Type of l2_cache_miss_access_ctrl register
 *  Cache wrap around control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_miss_disable_access : HRO; bitpos: [5]; default: 0;
         *  Set this bit as 1 to disable early restart of  L2-Cache
         */
        uint32_t l2_cache_miss_disable_access:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_miss_access_ctrl_reg_t;


/** Group: Cache Freeze Control registers */
/** Type of l1_cache_freeze_ctrl register
 *  Cache Freeze control register
 */
typedef union {
    struct {
        /** l1_icache0_freeze_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable freeze operation on L1-ICache0. It can be cleared by
         *  software.
         */
        uint32_t l1_icache0_freeze_en:1;
        /** l1_icache0_freeze_mode : HRO; bitpos: [1]; default: 0;
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
        /** l1_icache1_freeze_en : HRO; bitpos: [4]; default: 0;
         *  The bit is used to enable freeze operation on L1-ICache1. It can be cleared by
         *  software.
         */
        uint32_t l1_icache1_freeze_en:1;
        /** l1_icache1_freeze_mode : HRO; bitpos: [5]; default: 0;
         *  The bit is used to configure mode of freeze operation L1-ICache1. 0: a miss-access
         *  will not stuck. 1: a miss-access will stuck.
         */
        uint32_t l1_icache1_freeze_mode:1;
        /** l1_icache1_freeze_done : RO; bitpos: [6]; default: 0;
         *  The bit is used to indicate whether freeze operation on L1-ICache1 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache1_freeze_done:1;
        uint32_t reserved_7:1;
        /** l1_icache2_freeze_en : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_freeze_en:1;
        /** l1_icache2_freeze_mode : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_freeze_mode:1;
        /** l1_icache2_freeze_done : RO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_freeze_done:1;
        uint32_t reserved_11:1;
        /** l1_icache3_freeze_en : HRO; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_freeze_en:1;
        /** l1_icache3_freeze_mode : HRO; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_freeze_mode:1;
        /** l1_icache3_freeze_done : RO; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_freeze_done:1;
        uint32_t reserved_15:1;
        /** l1_cache_freeze_en : R/W; bitpos: [16]; default: 0;
         *  The bit is used to enable freeze operation on L1-Cache. It can be cleared by
         *  software.
         */
        uint32_t l1_cache_freeze_en:1;
        /** l1_cache_freeze_mode : R/W; bitpos: [17]; default: 0;
         *  The bit is used to configure mode of freeze operation L1-Cache. 0: a miss-access
         *  will not stuck. 1: a miss-access will stuck.
         */
        uint32_t l1_cache_freeze_mode:1;
        /** l1_cache_freeze_done : RO; bitpos: [18]; default: 0;
         *  The bit is used to indicate whether freeze operation on L1-Cache is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_cache_freeze_done:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} cache_l1_cache_freeze_ctrl_reg_t;

/** Type of l2_cache_freeze_ctrl register
 *  Cache Freeze control register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** l2_cache_freeze_en : HRO; bitpos: [20]; default: 0;
         *  The bit is used to enable freeze operation on L2-Cache. It can be cleared by
         *  software.
         */
        uint32_t l2_cache_freeze_en:1;
        /** l2_cache_freeze_mode : HRO; bitpos: [21]; default: 0;
         *  The bit is used to configure mode of freeze operation L2-Cache. 0: a miss-access
         *  will not stuck. 1: a miss-access will stuck.
         */
        uint32_t l2_cache_freeze_mode:1;
        /** l2_cache_freeze_done : RO; bitpos: [22]; default: 0;
         *  The bit is used to indicate whether freeze operation on L2-Cache is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l2_cache_freeze_done:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} cache_l2_cache_freeze_ctrl_reg_t;


/** Group: Cache Data Memory Access Control and Configuration registers */
/** Type of l1_cache_data_mem_acs_conf register
 *  Cache data memory access configure register
 */
typedef union {
    struct {
        /** l1_icache0_data_mem_rd_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache0 data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache0_data_mem_rd_en:1;
        /** l1_icache0_data_mem_wr_en : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache0 data memoryory. 0: disable,
         *  1: enable.
         */
        uint32_t l1_icache0_data_mem_wr_en:1;
        uint32_t reserved_2:2;
        /** l1_icache1_data_mem_rd_en : HRO; bitpos: [4]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache1 data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache1_data_mem_rd_en:1;
        /** l1_icache1_data_mem_wr_en : HRO; bitpos: [5]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache1 data memoryory. 0: disable,
         *  1: enable.
         */
        uint32_t l1_icache1_data_mem_wr_en:1;
        uint32_t reserved_6:2;
        /** l1_icache2_data_mem_rd_en : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_data_mem_rd_en:1;
        /** l1_icache2_data_mem_wr_en : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_data_mem_wr_en:1;
        uint32_t reserved_10:2;
        /** l1_icache3_data_mem_rd_en : HRO; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_data_mem_rd_en:1;
        /** l1_icache3_data_mem_wr_en : HRO; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_data_mem_wr_en:1;
        uint32_t reserved_14:2;
        /** l1_cache_data_mem_rd_en : R/W; bitpos: [16]; default: 0;
         *  The bit is used to enable config-bus read L1-Cache data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_cache_data_mem_rd_en:1;
        /** l1_cache_data_mem_wr_en : R/W; bitpos: [17]; default: 0;
         *  The bit is used to enable config-bus write L1-Cache data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_cache_data_mem_wr_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} cache_l1_cache_data_mem_acs_conf_reg_t;

/** Type of l2_cache_data_mem_acs_conf register
 *  Cache data memory access configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** l2_cache_data_mem_rd_en : HRO; bitpos: [20]; default: 0;
         *  The bit is used to enable config-bus read L2-Cache data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l2_cache_data_mem_rd_en:1;
        /** l2_cache_data_mem_wr_en : HRO; bitpos: [21]; default: 0;
         *  The bit is used to enable config-bus write L2-Cache data memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l2_cache_data_mem_wr_en:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} cache_l2_cache_data_mem_acs_conf_reg_t;


/** Group: Cache Tag Memory Access Control and Configuration registers */
/** Type of l1_cache_tag_mem_acs_conf register
 *  Cache tag memory access configure register
 */
typedef union {
    struct {
        /** l1_icache0_tag_mem_rd_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache0 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache0_tag_mem_rd_en:1;
        /** l1_icache0_tag_mem_wr_en : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache0 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache0_tag_mem_wr_en:1;
        uint32_t reserved_2:2;
        /** l1_icache1_tag_mem_rd_en : HRO; bitpos: [4]; default: 0;
         *  The bit is used to enable config-bus read L1-ICache1 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache1_tag_mem_rd_en:1;
        /** l1_icache1_tag_mem_wr_en : HRO; bitpos: [5]; default: 0;
         *  The bit is used to enable config-bus write L1-ICache1 tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_icache1_tag_mem_wr_en:1;
        uint32_t reserved_6:2;
        /** l1_icache2_tag_mem_rd_en : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_tag_mem_rd_en:1;
        /** l1_icache2_tag_mem_wr_en : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_tag_mem_wr_en:1;
        uint32_t reserved_10:2;
        /** l1_icache3_tag_mem_rd_en : HRO; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_tag_mem_rd_en:1;
        /** l1_icache3_tag_mem_wr_en : HRO; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_tag_mem_wr_en:1;
        uint32_t reserved_14:2;
        /** l1_cache_tag_mem_rd_en : R/W; bitpos: [16]; default: 0;
         *  The bit is used to enable config-bus read L1-Cache tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_cache_tag_mem_rd_en:1;
        /** l1_cache_tag_mem_wr_en : R/W; bitpos: [17]; default: 0;
         *  The bit is used to enable config-bus write L1-Cache tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l1_cache_tag_mem_wr_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} cache_l1_cache_tag_mem_acs_conf_reg_t;

/** Type of l2_cache_tag_mem_acs_conf register
 *  Cache tag memory access configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** l2_cache_tag_mem_rd_en : HRO; bitpos: [20]; default: 0;
         *  The bit is used to enable config-bus read L2-Cache tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l2_cache_tag_mem_rd_en:1;
        /** l2_cache_tag_mem_wr_en : HRO; bitpos: [21]; default: 0;
         *  The bit is used to enable config-bus write L2-Cache tag memoryory. 0: disable, 1:
         *  enable.
         */
        uint32_t l2_cache_tag_mem_wr_en:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} cache_l2_cache_tag_mem_acs_conf_reg_t;


/** Group: Prelock Control and configuration registers */
/** Type of l1_icache0_prelock_conf register
 *  L1 instruction Cache 0 prelock configure register
 */
typedef union {
    struct {
        /** l1_icache0_prelock_sct0_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-ICache0.
         */
        uint32_t l1_icache0_prelock_sct0_en:1;
        /** l1_icache0_prelock_sct1_en : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-ICache0.
         */
        uint32_t l1_icache0_prelock_sct1_en:1;
        /** l1_icache0_prelock_rgid : HRO; bitpos: [5:2]; default: 0;
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
        /** l1_icache0_prelock_sct0_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache0_prelock_sct1_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache0_prelock_sct0_size : HRO; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_icache0_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_icache0_prelock_sct1_size : HRO; bitpos: [29:16]; default: 16383;
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
        /** l1_icache1_prelock_sct0_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-ICache1.
         */
        uint32_t l1_icache1_prelock_sct0_en:1;
        /** l1_icache1_prelock_sct1_en : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-ICache1.
         */
        uint32_t l1_icache1_prelock_sct1_en:1;
        /** l1_icache1_prelock_rgid : HRO; bitpos: [5:2]; default: 0;
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
        /** l1_icache1_prelock_sct0_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache1_prelock_sct1_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache1_prelock_sct0_size : HRO; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_icache1_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_icache1_prelock_sct1_size : HRO; bitpos: [29:16]; default: 16383;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l1_icache1_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_icache1_prelock_sct_size_reg_t;

/** Type of l1_icache2_prelock_conf register
 *  L1 instruction Cache 2 prelock configure register
 */
typedef union {
    struct {
        /** l1_icache2_prelock_sct0_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-ICache2.
         */
        uint32_t l1_icache2_prelock_sct0_en:1;
        /** l1_icache2_prelock_sct1_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-ICache2.
         */
        uint32_t l1_icache2_prelock_sct1_en:1;
        /** l1_icache2_prelock_rgid : R/W; bitpos: [5:2]; default: 0;
         *  The bit is used to set  the gid of l1 icache2 prelock.
         */
        uint32_t l1_icache2_prelock_rgid:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_icache2_prelock_conf_reg_t;

/** Type of l1_icache2_prelock_sct0_addr register
 *  L1 instruction Cache 2 prelock section0 address configure register
 */
typedef union {
    struct {
        /** l1_icache2_prelock_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section of prelock
         *  on L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT0_SIZE_REG
         */
        uint32_t l1_icache2_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache2_prelock_sct0_addr_reg_t;

/** Type of l1_icache2_prelock_sct1_addr register
 *  L1 instruction Cache 2 prelock section1 address configure register
 */
typedef union {
    struct {
        /** l1_icache2_prelock_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section of prelock
         *  on L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT1_SIZE_REG
         */
        uint32_t l1_icache2_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache2_prelock_sct1_addr_reg_t;

/** Type of l1_icache2_prelock_sct_size register
 *  L1 instruction Cache 2 prelock section size configure register
 */
typedef union {
    struct {
        /** l1_icache2_prelock_sct0_size : R/W; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_icache2_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_icache2_prelock_sct1_size : R/W; bitpos: [29:16]; default: 16383;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l1_icache2_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_icache2_prelock_sct_size_reg_t;

/** Type of l1_icache3_prelock_conf register
 *  L1 instruction Cache 3 prelock configure register
 */
typedef union {
    struct {
        /** l1_icache3_prelock_sct0_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-ICache3.
         */
        uint32_t l1_icache3_prelock_sct0_en:1;
        /** l1_icache3_prelock_sct1_en : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-ICache3.
         */
        uint32_t l1_icache3_prelock_sct1_en:1;
        /** l1_icache3_prelock_rgid : HRO; bitpos: [5:2]; default: 0;
         *  The bit is used to set  the gid of l1 icache3 prelock.
         */
        uint32_t l1_icache3_prelock_rgid:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_icache3_prelock_conf_reg_t;

/** Type of l1_icache3_prelock_sct0_addr register
 *  L1 instruction Cache 3 prelock section0 address configure register
 */
typedef union {
    struct {
        /** l1_icache3_prelock_sct0_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section of prelock
         *  on L1-ICache3, which should be used together with L1_ICACHE3_PRELOCK_SCT0_SIZE_REG
         */
        uint32_t l1_icache3_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache3_prelock_sct0_addr_reg_t;

/** Type of l1_icache3_prelock_sct1_addr register
 *  L1 instruction Cache 3 prelock section1 address configure register
 */
typedef union {
    struct {
        /** l1_icache3_prelock_sct1_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section of prelock
         *  on L1-ICache3, which should be used together with L1_ICACHE3_PRELOCK_SCT1_SIZE_REG
         */
        uint32_t l1_icache3_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache3_prelock_sct1_addr_reg_t;

/** Type of l1_icache3_prelock_sct_size register
 *  L1 instruction Cache 3 prelock section size configure register
 */
typedef union {
    struct {
        /** l1_icache3_prelock_sct0_size : HRO; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache3, which should be used together with L1_ICACHE3_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_icache3_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_icache3_prelock_sct1_size : HRO; bitpos: [29:16]; default: 16383;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L1-ICache3, which should be used together with L1_ICACHE3_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l1_icache3_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_icache3_prelock_sct_size_reg_t;

/** Type of l1_cache_prelock_conf register
 *  L1 Cache prelock configure register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct0_en : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L1-Cache.
         */
        uint32_t l1_cache_prelock_sct0_en:1;
        /** l1_cache_prelock_sct1_en : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L1-Cache.
         */
        uint32_t l1_cache_prelock_sct1_en:1;
        /** l1_cache_prelock_rgid : HRO; bitpos: [5:2]; default: 0;
         *  The bit is used to set  the gid of l1 cache prelock.
         */
        uint32_t l1_cache_prelock_rgid:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l1_cache_prelock_conf_reg_t;

/** Type of l1_cache_prelock_sct0_addr register
 *  L1 Cache prelock section0 address configure register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section of prelock
         *  on L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT0_SIZE_REG
         */
        uint32_t l1_cache_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_cache_prelock_sct0_addr_reg_t;

/** Type of l1_dcache_prelock_sct1_addr register
 *  L1 Cache prelock section1 address configure register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section of prelock
         *  on L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT1_SIZE_REG
         */
        uint32_t l1_cache_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_prelock_sct1_addr_reg_t;

/** Type of l1_dcache_prelock_sct_size register
 *  L1  Cache prelock section size configure register
 */
typedef union {
    struct {
        /** l1_cache_prelock_sct0_size : R/W; bitpos: [13:0]; default: 16383;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l1_cache_prelock_sct0_size:14;
        uint32_t reserved_14:2;
        /** l1_cache_prelock_sct1_size : R/W; bitpos: [29:16]; default: 16383;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l1_cache_prelock_sct1_size:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} cache_l1_dcache_prelock_sct_size_reg_t;

/** Type of l2_cache_prelock_conf register
 *  L2 Cache prelock configure register
 */
typedef union {
    struct {
        /** l2_cache_prelock_sct0_en : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable the first section of prelock function on L2-Cache.
         */
        uint32_t l2_cache_prelock_sct0_en:1;
        /** l2_cache_prelock_sct1_en : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable the second section of prelock function on L2-Cache.
         */
        uint32_t l2_cache_prelock_sct1_en:1;
        /** l2_cache_prelock_rgid : HRO; bitpos: [5:2]; default: 0;
         *  The bit is used to set  the gid of l2 cache prelock.
         */
        uint32_t l2_cache_prelock_rgid:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_prelock_conf_reg_t;

/** Type of l2_cache_prelock_sct0_addr register
 *  L2 Cache prelock section0 address configure register
 */
typedef union {
    struct {
        /** l2_cache_prelock_sct0_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section of prelock
         *  on L2-Cache, which should be used together with L2_CACHE_PRELOCK_SCT0_SIZE_REG
         */
        uint32_t l2_cache_prelock_sct0_addr:32;
    };
    uint32_t val;
} cache_l2_cache_prelock_sct0_addr_reg_t;

/** Type of l2_cache_prelock_sct1_addr register
 *  L2 Cache prelock section1 address configure register
 */
typedef union {
    struct {
        /** l2_cache_prelock_sct1_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section of prelock
         *  on L2-Cache, which should be used together with L2_CACHE_PRELOCK_SCT1_SIZE_REG
         */
        uint32_t l2_cache_prelock_sct1_addr:32;
    };
    uint32_t val;
} cache_l2_cache_prelock_sct1_addr_reg_t;

/** Type of l2_cache_prelock_sct_size register
 *  L2 Cache prelock section size configure register
 */
typedef union {
    struct {
        /** l2_cache_prelock_sct0_size : HRO; bitpos: [15:0]; default: 65535;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L2-Cache, which should be used together with L2_CACHE_PRELOCK_SCT0_ADDR_REG
         */
        uint32_t l2_cache_prelock_sct0_size:16;
        /** l2_cache_prelock_sct1_size : HRO; bitpos: [31:16]; default: 65535;
         *  Those bits are used to configure the size of the second section of prelock on
         *  L2-Cache, which should be used together with L2_CACHE_PRELOCK_SCT1_ADDR_REG
         */
        uint32_t l2_cache_prelock_sct1_size:16;
    };
    uint32_t val;
} cache_l2_cache_prelock_sct_size_reg_t;


/** Group: Lock Control and configuration registers */
/** Type of lock_ctrl register
 *  Lock-class (manual lock) operation control register
 */
typedef union {
    struct {
        /** lock_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable lock operation. It will be cleared by hardware after lock
         *  operation done
         */
        uint32_t lock_ena:1;
        /** unlock_ena : R/W/SC; bitpos: [1]; default: 0;
         *  The bit is used to enable unlock operation. It will be cleared by hardware after
         *  unlock operation done
         */
        uint32_t unlock_ena:1;
        /** lock_done : RO; bitpos: [2]; default: 1;
         *  The bit is used to indicate whether unlock/lock operation is finished or not. 0:
         *  not finished. 1: finished.
         */
        uint32_t lock_done:1;
        /** lock_rgid : HRO; bitpos: [6:3]; default: 0;
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
         *  apply this lock/unlock operation. [4]: L1-Cache
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
        /** sync_rgid : HRO; bitpos: [8:5]; default: 0;
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
        /** sync_map : R/W; bitpos: [5:0]; default: 63;
         *  Those bits are used to indicate which caches in the two-level cache structure will
         *  apply the sync operation.  [4]: L1-Cache
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
        /** sync_size : R/W; bitpos: [24:0]; default: 0;
         *  Those bits are used to configure the size of the sync operation, which should be
         *  used together with CACHE_SYNC_ADDR_REG
         */
        uint32_t sync_size:25;
        uint32_t reserved_25:7;
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
        /** l1_icache0_preload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_icache0_preload_order:1;
        /** l1_icache0_preload_rgid : HRO; bitpos: [6:3]; default: 0;
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
        /** l1_icache0_preload_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache0_preload_size : HRO; bitpos: [13:0]; default: 0;
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
        /** l1_icache1_preload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_icache1_preload_order:1;
        /** l1_icache1_preload_rgid : HRO; bitpos: [6:3]; default: 0;
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
        /** l1_icache1_preload_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache1_preload_size : HRO; bitpos: [13:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOAD_ADDR_REG
         */
        uint32_t l1_icache1_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache1_preload_size_reg_t;

/** Type of l1_icache2_preload_ctrl register
 *  L1 instruction Cache 2 preload-operation control register
 */
typedef union {
    struct {
        /** l1_icache2_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable preload operation on L1-ICache2. It will be cleared by
         *  hardware automatically after preload operation is done.
         */
        uint32_t l1_icache2_preload_ena:1;
        /** l1_icache2_preload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether preload operation is finished or not. 0: not
         *  finished. 1: finished.
         */
        uint32_t l1_icache2_preload_done:1;
        /** l1_icache2_preload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_icache2_preload_order:1;
        /** l1_icache2_preload_rgid : R/W; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of l1 icache2 preload.
         */
        uint32_t l1_icache2_preload_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_l1_icache2_preload_ctrl_reg_t;

/** Type of l1_icache2_preload_addr register
 *  L1 instruction Cache 2 preload address configure register
 */
typedef union {
    struct {
        /** l1_icache2_preload_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of preload on L1-ICache2, which
         *  should be used together with L1_ICACHE2_PRELOAD_SIZE_REG
         */
        uint32_t l1_icache2_preload_addr:32;
    };
    uint32_t val;
} cache_l1_icache2_preload_addr_reg_t;

/** Type of l1_icache2_preload_size register
 *  L1 instruction Cache 2 preload size configure register
 */
typedef union {
    struct {
        /** l1_icache2_preload_size : R/W; bitpos: [13:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOAD_ADDR_REG
         */
        uint32_t l1_icache2_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache2_preload_size_reg_t;

/** Type of l1_icache3_preload_ctrl register
 *  L1 instruction Cache 3 preload-operation control register
 */
typedef union {
    struct {
        /** l1_icache3_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable preload operation on L1-ICache3. It will be cleared by
         *  hardware automatically after preload operation is done.
         */
        uint32_t l1_icache3_preload_ena:1;
        /** l1_icache3_preload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether preload operation is finished or not. 0: not
         *  finished. 1: finished.
         */
        uint32_t l1_icache3_preload_done:1;
        /** l1_icache3_preload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_icache3_preload_order:1;
        /** l1_icache3_preload_rgid : HRO; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of l1 icache3 preload.
         */
        uint32_t l1_icache3_preload_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_l1_icache3_preload_ctrl_reg_t;

/** Type of l1_icache3_preload_addr register
 *  L1 instruction Cache 3 preload address configure register
 */
typedef union {
    struct {
        /** l1_icache3_preload_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of preload on L1-ICache3, which
         *  should be used together with L1_ICACHE3_PRELOAD_SIZE_REG
         */
        uint32_t l1_icache3_preload_addr:32;
    };
    uint32_t val;
} cache_l1_icache3_preload_addr_reg_t;

/** Type of l1_icache3_preload_size register
 *  L1 instruction Cache 3 preload size configure register
 */
typedef union {
    struct {
        /** l1_icache3_preload_size : HRO; bitpos: [13:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-ICache3, which should be used together with L1_ICACHE3_PRELOAD_ADDR_REG
         */
        uint32_t l1_icache3_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache3_preload_size_reg_t;

/** Type of l1_cache_preload_ctrl register
 *  L1 Cache  preload-operation control register
 */
typedef union {
    struct {
        /** l1_cache_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable preload operation on L1-Cache. It will be cleared by
         *  hardware automatically after preload operation is done.
         */
        uint32_t l1_cache_preload_ena:1;
        /** l1_cache_preload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether preload operation is finished or not. 0: not
         *  finished. 1: finished.
         */
        uint32_t l1_cache_preload_done:1;
        /** l1_cache_preload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l1_cache_preload_order:1;
        /** l1_cache_preload_rgid : HRO; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of l1 cache preload.
         */
        uint32_t l1_cache_preload_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_l1_cache_preload_ctrl_reg_t;

/** Type of l1_dcache_preload_addr register
 *  L1 Cache  preload address configure register
 */
typedef union {
    struct {
        /** l1_cache_preload_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of preload on L1-Cache, which
         *  should be used together with L1_CACHE_PRELOAD_SIZE_REG
         */
        uint32_t l1_cache_preload_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_preload_addr_reg_t;

/** Type of l1_dcache_preload_size register
 *  L1 Cache  preload size configure register
 */
typedef union {
    struct {
        /** l1_cache_preload_size : R/W; bitpos: [13:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L1-Cache, which should be used together with L1_CACHE_PRELOAD_ADDR_REG
         */
        uint32_t l1_cache_preload_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_dcache_preload_size_reg_t;

/** Type of l2_cache_preload_ctrl register
 *  L2 Cache preload-operation control register
 */
typedef union {
    struct {
        /** l2_cache_preload_ena : R/W/SC; bitpos: [0]; default: 0;
         *  The bit is used to enable preload operation on L2-Cache. It will be cleared by
         *  hardware automatically after preload operation is done.
         */
        uint32_t l2_cache_preload_ena:1;
        /** l2_cache_preload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether preload operation is finished or not. 0: not
         *  finished. 1: finished.
         */
        uint32_t l2_cache_preload_done:1;
        /** l2_cache_preload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
         *  descending.
         */
        uint32_t l2_cache_preload_order:1;
        /** l2_cache_preload_rgid : HRO; bitpos: [6:3]; default: 0;
         *  The bit is used to set  the gid of l2 cache preload.
         */
        uint32_t l2_cache_preload_rgid:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} cache_l2_cache_preload_ctrl_reg_t;

/** Type of l2_cache_preload_addr register
 *  L2 Cache preload address configure register
 */
typedef union {
    struct {
        /** l2_cache_preload_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of preload on L2-Cache, which
         *  should be used together with L2_CACHE_PRELOAD_SIZE_REG
         */
        uint32_t l2_cache_preload_addr:32;
    };
    uint32_t val;
} cache_l2_cache_preload_addr_reg_t;

/** Type of l2_cache_preload_size register
 *  L2 Cache preload size configure register
 */
typedef union {
    struct {
        /** l2_cache_preload_size : HRO; bitpos: [15:0]; default: 0;
         *  Those bits are used to configure the size of the first section of prelock on
         *  L2-Cache, which should be used together with L2_CACHE_PRELOAD_ADDR_REG
         */
        uint32_t l2_cache_preload_size:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l2_cache_preload_size_reg_t;


/** Group: Autoload Control and configuration registers */
/** Type of l1_icache0_autoload_ctrl register
 *  L1 instruction Cache 0 autoload-operation control register
 */
typedef union {
    struct {
        /** l1_icache0_autoload_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-ICache0.  1: enable,
         *  0: disable.
         */
        uint32_t l1_icache0_autoload_ena:1;
        /** l1_icache0_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-ICache0 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache0_autoload_done:1;
        /** l1_icache0_autoload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-ICache0. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_icache0_autoload_order:1;
        /** l1_icache0_autoload_trigger_mode : HRO; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-ICache0.
         *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_icache0_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_icache0_autoload_sct0_ena : HRO; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-ICache0.
         */
        uint32_t l1_icache0_autoload_sct0_ena:1;
        /** l1_icache0_autoload_sct1_ena : HRO; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-ICache0.
         */
        uint32_t l1_icache0_autoload_sct1_ena:1;
        /** l1_icache0_autoload_rgid : HRO; bitpos: [13:10]; default: 0;
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
        /** l1_icache0_autoload_sct0_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache0_autoload_sct0_size : HRO; bitpos: [27:0]; default: 0;
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
        /** l1_icache0_autoload_sct1_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache0_autoload_sct1_size : HRO; bitpos: [27:0]; default: 0;
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
        /** l1_icache1_autoload_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-ICache1.  1: enable,
         *  0: disable.
         */
        uint32_t l1_icache1_autoload_ena:1;
        /** l1_icache1_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-ICache1 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache1_autoload_done:1;
        /** l1_icache1_autoload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-ICache1. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_icache1_autoload_order:1;
        /** l1_icache1_autoload_trigger_mode : HRO; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-ICache1.
         *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_icache1_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_icache1_autoload_sct0_ena : HRO; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-ICache1.
         */
        uint32_t l1_icache1_autoload_sct0_ena:1;
        /** l1_icache1_autoload_sct1_ena : HRO; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-ICache1.
         */
        uint32_t l1_icache1_autoload_sct1_ena:1;
        /** l1_icache1_autoload_rgid : HRO; bitpos: [13:10]; default: 0;
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
        /** l1_icache1_autoload_sct0_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache1_autoload_sct0_size : HRO; bitpos: [27:0]; default: 0;
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
        /** l1_icache1_autoload_sct1_addr : HRO; bitpos: [31:0]; default: 0;
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
        /** l1_icache1_autoload_sct1_size : HRO; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the second section for autoload
         *  operation on L1-ICache1. Note that it should be used together with
         *  L1_ICACHE1_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache1_autoload_sct1_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache1_autoload_sct1_size_reg_t;

/** Type of l1_icache2_autoload_ctrl register
 *  L1 instruction Cache 2 autoload-operation control register
 */
typedef union {
    struct {
        /** l1_icache2_autoload_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-ICache2.  1: enable,
         *  0: disable.
         */
        uint32_t l1_icache2_autoload_ena:1;
        /** l1_icache2_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-ICache2 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache2_autoload_done:1;
        /** l1_icache2_autoload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-ICache2. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_icache2_autoload_order:1;
        /** l1_icache2_autoload_trigger_mode : R/W; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-ICache2.
         *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_icache2_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_icache2_autoload_sct0_ena : R/W; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-ICache2.
         */
        uint32_t l1_icache2_autoload_sct0_ena:1;
        /** l1_icache2_autoload_sct1_ena : R/W; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-ICache2.
         */
        uint32_t l1_icache2_autoload_sct1_ena:1;
        /** l1_icache2_autoload_rgid : R/W; bitpos: [13:10]; default: 0;
         *  The bit is used to set  the gid of l1 icache2 autoload.
         */
        uint32_t l1_icache2_autoload_rgid:4;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache2_autoload_ctrl_reg_t;

/** Type of l1_icache2_autoload_sct0_addr register
 *  L1 instruction Cache 2 autoload section 0 address configure register
 */
typedef union {
    struct {
        /** l1_icache2_autoload_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section for
         *  autoload operation on L1-ICache2. Note that it should be used together with
         *  L1_ICACHE2_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache2_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache2_autoload_sct0_addr_reg_t;

/** Type of l1_icache2_autoload_sct0_size register
 *  L1 instruction Cache 2 autoload section 0 size configure register
 */
typedef union {
    struct {
        /** l1_icache2_autoload_sct0_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the first section for autoload
         *  operation on L1-ICache2. Note that it should be used together with
         *  L1_ICACHE2_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache2_autoload_sct0_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache2_autoload_sct0_size_reg_t;

/** Type of l1_icache2_autoload_sct1_addr register
 *  L1 instruction Cache 2 autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_icache2_autoload_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section for
         *  autoload operation on L1-ICache2. Note that it should be used together with
         *  L1_ICACHE2_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache2_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache2_autoload_sct1_addr_reg_t;

/** Type of l1_icache2_autoload_sct1_size register
 *  L1 instruction Cache 2 autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_icache2_autoload_sct1_size : R/W; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the second section for autoload
         *  operation on L1-ICache2. Note that it should be used together with
         *  L1_ICACHE2_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache2_autoload_sct1_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache2_autoload_sct1_size_reg_t;

/** Type of l1_icache3_autoload_ctrl register
 *  L1 instruction Cache 3 autoload-operation control register
 */
typedef union {
    struct {
        /** l1_icache3_autoload_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-ICache3.  1: enable,
         *  0: disable.
         */
        uint32_t l1_icache3_autoload_ena:1;
        /** l1_icache3_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-ICache3 is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_icache3_autoload_done:1;
        /** l1_icache3_autoload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-ICache3. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_icache3_autoload_order:1;
        /** l1_icache3_autoload_trigger_mode : HRO; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-ICache3.
         *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_icache3_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_icache3_autoload_sct0_ena : HRO; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-ICache3.
         */
        uint32_t l1_icache3_autoload_sct0_ena:1;
        /** l1_icache3_autoload_sct1_ena : HRO; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-ICache3.
         */
        uint32_t l1_icache3_autoload_sct1_ena:1;
        /** l1_icache3_autoload_rgid : HRO; bitpos: [13:10]; default: 0;
         *  The bit is used to set  the gid of l1 icache3 autoload.
         */
        uint32_t l1_icache3_autoload_rgid:4;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_icache3_autoload_ctrl_reg_t;

/** Type of l1_icache3_autoload_sct0_addr register
 *  L1 instruction Cache 3 autoload section 0 address configure register
 */
typedef union {
    struct {
        /** l1_icache3_autoload_sct0_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section for
         *  autoload operation on L1-ICache3. Note that it should be used together with
         *  L1_ICACHE3_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache3_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_icache3_autoload_sct0_addr_reg_t;

/** Type of l1_icache3_autoload_sct0_size register
 *  L1 instruction Cache 3 autoload section 0 size configure register
 */
typedef union {
    struct {
        /** l1_icache3_autoload_sct0_size : HRO; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the first section for autoload
         *  operation on L1-ICache3. Note that it should be used together with
         *  L1_ICACHE3_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_icache3_autoload_sct0_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache3_autoload_sct0_size_reg_t;

/** Type of l1_icache3_autoload_sct1_addr register
 *  L1 instruction Cache 3 autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_icache3_autoload_sct1_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section for
         *  autoload operation on L1-ICache3. Note that it should be used together with
         *  L1_ICACHE3_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_icache3_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_icache3_autoload_sct1_addr_reg_t;

/** Type of l1_icache3_autoload_sct1_size register
 *  L1 instruction Cache 3 autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_icache3_autoload_sct1_size : HRO; bitpos: [27:0]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_autoload_sct1_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l1_icache3_autoload_sct1_size_reg_t;

/** Type of l1_cache_autoload_ctrl register
 *  L1 Cache autoload-operation control register
 */
typedef union {
    struct {
        /** l1_cache_autoload_ena : R/W; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L1-Cache.  1: enable,
         *  0: disable.
         */
        uint32_t l1_cache_autoload_ena:1;
        /** l1_cache_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L1-Cache is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l1_cache_autoload_done:1;
        /** l1_cache_autoload_order : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L1-Cache. 0:
         *  ascending. 1: descending.
         */
        uint32_t l1_cache_autoload_order:1;
        /** l1_cache_autoload_trigger_mode : R/W; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L1-Cache. 0/3:
         *  miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l1_cache_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l1_cache_autoload_sct0_ena : R/W; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L1-Cache.
         */
        uint32_t l1_cache_autoload_sct0_ena:1;
        /** l1_cache_autoload_sct1_ena : R/W; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L1-Cache.
         */
        uint32_t l1_cache_autoload_sct1_ena:1;
        /** l1_cache_autoload_sct2_ena : HRO; bitpos: [10]; default: 0;
         *  The bit is used to enable the third section for autoload operation on L1-Cache.
         */
        uint32_t l1_cache_autoload_sct2_ena:1;
        /** l1_cache_autoload_sct3_ena : HRO; bitpos: [11]; default: 0;
         *  The bit is used to enable the fourth section for autoload operation on L1-Cache.
         */
        uint32_t l1_cache_autoload_sct3_ena:1;
        /** l1_cache_autoload_rgid : HRO; bitpos: [15:12]; default: 0;
         *  The bit is used to set  the gid of l1 cache autoload.
         */
        uint32_t l1_cache_autoload_rgid:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l1_cache_autoload_ctrl_reg_t;

/** Type of l1_cache_autoload_sct0_addr register
 *  L1 Cache autoload section 0 address configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct0_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section for
         *  autoload operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT0_SIZE and L1_CACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_cache_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct0_addr_reg_t;

/** Type of l1_cache_autoload_sct0_size register
 *  L1 Cache autoload section 0 size configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct0_size : R/W; bitpos: [24:0]; default: 0;
         *  Those bits are used to configure the size of the first section for autoload
         *  operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT0_ADDR and L1_CACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l1_cache_autoload_sct0_size:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct0_size_reg_t;

/** Type of l1_cache_autoload_sct1_addr register
 *  L1 Cache autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section for
         *  autoload operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT1_SIZE and L1_CACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_cache_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct1_addr_reg_t;

/** Type of l1_cache_autoload_sct1_size register
 *  L1 Cache autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct1_size : R/W; bitpos: [24:0]; default: 0;
         *  Those bits are used to configure the size of the second section for autoload
         *  operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT1_ADDR and L1_CACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l1_cache_autoload_sct1_size:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct1_size_reg_t;

/** Type of l1_cache_autoload_sct2_addr register
 *  L1 Cache autoload section 2 address configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct2_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the third section for
         *  autoload operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT2_SIZE and L1_CACHE_AUTOLOAD_SCT2_ENA.
         */
        uint32_t l1_cache_autoload_sct2_addr:32;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct2_addr_reg_t;

/** Type of l1_cache_autoload_sct2_size register
 *  L1 Cache autoload section 2 size configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct2_size : HRO; bitpos: [24:0]; default: 0;
         *  Those bits are used to configure the size of the third section for autoload
         *  operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT2_ADDR and L1_CACHE_AUTOLOAD_SCT2_ENA.
         */
        uint32_t l1_cache_autoload_sct2_size:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct2_size_reg_t;

/** Type of l1_cache_autoload_sct3_addr register
 *  L1 Cache autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct3_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the fourth section for
         *  autoload operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT3_SIZE and L1_CACHE_AUTOLOAD_SCT3_ENA.
         */
        uint32_t l1_cache_autoload_sct3_addr:32;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct3_addr_reg_t;

/** Type of l1_cache_autoload_sct3_size register
 *  L1 Cache autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l1_cache_autoload_sct3_size : HRO; bitpos: [24:0]; default: 0;
         *  Those bits are used to configure the size of the fourth section for autoload
         *  operation on L1-Cache. Note that it should be used together with
         *  L1_CACHE_AUTOLOAD_SCT3_ADDR and L1_CACHE_AUTOLOAD_SCT3_ENA.
         */
        uint32_t l1_cache_autoload_sct3_size:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} cache_l1_cache_autoload_sct3_size_reg_t;

/** Type of l2_cache_autoload_ctrl register
 *  L2 Cache autoload-operation control register
 */
typedef union {
    struct {
        /** l2_cache_autoload_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable and disable autoload operation on L2-Cache.  1: enable,
         *  0: disable.
         */
        uint32_t l2_cache_autoload_ena:1;
        /** l2_cache_autoload_done : RO; bitpos: [1]; default: 1;
         *  The bit is used to indicate whether autoload operation on L2-Cache is finished or
         *  not. 0: not finished. 1: finished.
         */
        uint32_t l2_cache_autoload_done:1;
        /** l2_cache_autoload_order : HRO; bitpos: [2]; default: 0;
         *  The bit is used to configure the direction of autoload operation on L2-Cache. 0:
         *  ascending. 1: descending.
         */
        uint32_t l2_cache_autoload_order:1;
        /** l2_cache_autoload_trigger_mode : HRO; bitpos: [4:3]; default: 0;
         *  The field is used to configure trigger mode of autoload operation on L2-Cache. 0/3:
         *  miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
         */
        uint32_t l2_cache_autoload_trigger_mode:2;
        uint32_t reserved_5:3;
        /** l2_cache_autoload_sct0_ena : HRO; bitpos: [8]; default: 0;
         *  The bit is used to enable the first section for autoload operation on L2-Cache.
         */
        uint32_t l2_cache_autoload_sct0_ena:1;
        /** l2_cache_autoload_sct1_ena : HRO; bitpos: [9]; default: 0;
         *  The bit is used to enable the second section for autoload operation on L2-Cache.
         */
        uint32_t l2_cache_autoload_sct1_ena:1;
        /** l2_cache_autoload_sct2_ena : HRO; bitpos: [10]; default: 0;
         *  The bit is used to enable the third section for autoload operation on L2-Cache.
         */
        uint32_t l2_cache_autoload_sct2_ena:1;
        /** l2_cache_autoload_sct3_ena : HRO; bitpos: [11]; default: 0;
         *  The bit is used to enable the fourth section for autoload operation on L2-Cache.
         */
        uint32_t l2_cache_autoload_sct3_ena:1;
        /** l2_cache_autoload_rgid : HRO; bitpos: [15:12]; default: 0;
         *  The bit is used to set  the gid of l2 cache autoload.
         */
        uint32_t l2_cache_autoload_rgid:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l2_cache_autoload_ctrl_reg_t;

/** Type of l2_cache_autoload_sct0_addr register
 *  L2 Cache autoload section 0 address configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct0_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the first section for
         *  autoload operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT0_SIZE and L2_CACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l2_cache_autoload_sct0_addr:32;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct0_addr_reg_t;

/** Type of l2_cache_autoload_sct0_size register
 *  L2 Cache autoload section 0 size configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct0_size : HRO; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the first section for autoload
         *  operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT0_ADDR and L2_CACHE_AUTOLOAD_SCT0_ENA.
         */
        uint32_t l2_cache_autoload_sct0_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct0_size_reg_t;

/** Type of l2_cache_autoload_sct1_addr register
 *  L2 Cache autoload section 1 address configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct1_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the second section for
         *  autoload operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT1_SIZE and L2_CACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l2_cache_autoload_sct1_addr:32;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct1_addr_reg_t;

/** Type of l2_cache_autoload_sct1_size register
 *  L2 Cache autoload section 1 size configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct1_size : HRO; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the second section for autoload
         *  operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT1_ADDR and L2_CACHE_AUTOLOAD_SCT1_ENA.
         */
        uint32_t l2_cache_autoload_sct1_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct1_size_reg_t;

/** Type of l2_cache_autoload_sct2_addr register
 *  L2 Cache autoload section 2 address configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct2_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the third section for
         *  autoload operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT2_SIZE and L2_CACHE_AUTOLOAD_SCT2_ENA.
         */
        uint32_t l2_cache_autoload_sct2_addr:32;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct2_addr_reg_t;

/** Type of l2_cache_autoload_sct2_size register
 *  L2 Cache autoload section 2 size configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct2_size : HRO; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the third section for autoload
         *  operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT2_ADDR and L2_CACHE_AUTOLOAD_SCT2_ENA.
         */
        uint32_t l2_cache_autoload_sct2_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct2_size_reg_t;

/** Type of l2_cache_autoload_sct3_addr register
 *  L2 Cache autoload section 3 address configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct3_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits are used to configure the start address of the fourth section for
         *  autoload operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT3_SIZE and L2_CACHE_AUTOLOAD_SCT3_ENA.
         */
        uint32_t l2_cache_autoload_sct3_addr:32;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct3_addr_reg_t;

/** Type of l2_cache_autoload_sct3_size register
 *  L2 Cache autoload section 3 size configure register
 */
typedef union {
    struct {
        /** l2_cache_autoload_sct3_size : HRO; bitpos: [27:0]; default: 0;
         *  Those bits are used to configure the size of the fourth section for autoload
         *  operation on L2-Cache. Note that it should be used together with
         *  L2_CACHE_AUTOLOAD_SCT3_ADDR and L2_CACHE_AUTOLOAD_SCT3_ENA.
         */
        uint32_t l2_cache_autoload_sct3_size:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cache_l2_cache_autoload_sct3_size_reg_t;


/** Group: Interrupt registers */
/** Type of l1_cache_acs_cnt_int_ena register
 *  Cache Access Counter Interrupt enable register
 */
typedef union {
    struct {
        /** l1_ibus0_ovf_int_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-ICache0 due to bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_ovf_int_ena:1;
        /** l1_ibus1_ovf_int_ena : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-ICache1 due to bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_ovf_int_ena:1;
        /** l1_ibus2_ovf_int_ena : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus2_ovf_int_ena:1;
        /** l1_ibus3_ovf_int_ena : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus3_ovf_int_ena:1;
        /** l1_bus0_ovf_int_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-DCache due to bus0 accesses L1-DCache.
         */
        uint32_t l1_bus0_ovf_int_ena:1;
        /** l1_bus1_ovf_int_ena : R/W; bitpos: [5]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L1-DCache due to bus1 accesses L1-DCache.
         */
        uint32_t l1_bus1_ovf_int_ena:1;
        /** l1_dbus2_ovf_int_ena : HRO; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus2_ovf_int_ena:1;
        /** l1_dbus3_ovf_int_ena : HRO; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus3_ovf_int_ena:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_ena_reg_t;

/** Type of l1_cache_acs_cnt_int_clr register
 *  Cache Access Counter Interrupt clear register
 */
typedef union {
    struct {
        /** l1_ibus0_ovf_int_clr : HRO; bitpos: [0]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-ICache0 due
         *  to bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_ovf_int_clr:1;
        /** l1_ibus1_ovf_int_clr : HRO; bitpos: [1]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-ICache1 due
         *  to bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_ovf_int_clr:1;
        /** l1_ibus2_ovf_int_clr : WT; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus2_ovf_int_clr:1;
        /** l1_ibus3_ovf_int_clr : WT; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus3_ovf_int_clr:1;
        /** l1_bus0_ovf_int_clr : WT; bitpos: [4]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
         *  to bus0 accesses L1-DCache.
         */
        uint32_t l1_bus0_ovf_int_clr:1;
        /** l1_bus1_ovf_int_clr : WT; bitpos: [5]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
         *  to bus1 accesses L1-DCache.
         */
        uint32_t l1_bus1_ovf_int_clr:1;
        /** l1_dbus2_ovf_int_clr : HRO; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus2_ovf_int_clr:1;
        /** l1_dbus3_ovf_int_clr : HRO; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus3_ovf_int_clr:1;
        uint32_t reserved_8:24;
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
        /** l1_ibus2_ovf_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache2
         *  due to bus2 accesses L1-ICache2.
         */
        uint32_t l1_ibus2_ovf_int_raw:1;
        /** l1_ibus3_ovf_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache3
         *  due to bus3 accesses L1-ICache3.
         */
        uint32_t l1_ibus3_ovf_int_raw:1;
        /** l1_bus0_ovf_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
         *  due to bus0 accesses L1-DCache.
         */
        uint32_t l1_bus0_ovf_int_raw:1;
        /** l1_bus1_ovf_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
         *  due to bus1 accesses L1-DCache.
         */
        uint32_t l1_bus1_ovf_int_raw:1;
        /** l1_dbus2_ovf_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
         *  due to bus2 accesses L1-DCache.
         */
        uint32_t l1_dbus2_ovf_int_raw:1;
        /** l1_dbus3_ovf_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
         *  due to bus3 accesses L1-DCache.
         */
        uint32_t l1_dbus3_ovf_int_raw:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_raw_reg_t;

/** Type of l1_cache_acs_cnt_int_st register
 *  Cache Access Counter Interrupt status register
 */
typedef union {
    struct {
        /** l1_ibus0_ovf_int_st : HRO; bitpos: [0]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-ICache0 due to bus0 accesses L1-ICache0.
         */
        uint32_t l1_ibus0_ovf_int_st:1;
        /** l1_ibus1_ovf_int_st : HRO; bitpos: [1]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-ICache1 due to bus1 accesses L1-ICache1.
         */
        uint32_t l1_ibus1_ovf_int_st:1;
        /** l1_ibus2_ovf_int_st : RO; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus2_ovf_int_st:1;
        /** l1_ibus3_ovf_int_st : RO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus3_ovf_int_st:1;
        /** l1_bus0_ovf_int_st : RO; bitpos: [4]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-DCache due to bus0 accesses L1-DCache.
         */
        uint32_t l1_bus0_ovf_int_st:1;
        /** l1_bus1_ovf_int_st : RO; bitpos: [5]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L1-DCache due to bus1 accesses L1-DCache.
         */
        uint32_t l1_bus1_ovf_int_st:1;
        /** l1_dbus2_ovf_int_st : HRO; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus2_ovf_int_st:1;
        /** l1_dbus3_ovf_int_st : HRO; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus3_ovf_int_st:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} cache_l1_cache_acs_cnt_int_st_reg_t;

/** Type of l1_cache_acs_fail_int_ena register
 *  Cache Access Fail Interrupt enable register
 */
typedef union {
    struct {
        /** l1_icache0_fail_int_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable interrupt of access fail that occurs in L1-ICache0 due to
         *  cpu accesses L1-ICache0.
         */
        uint32_t l1_icache0_fail_int_ena:1;
        /** l1_icache1_fail_int_ena : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable interrupt of access fail that occurs in L1-ICache1 due to
         *  cpu accesses L1-ICache1.
         */
        uint32_t l1_icache1_fail_int_ena:1;
        /** l1_icache2_fail_int_ena : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_fail_int_ena:1;
        /** l1_icache3_fail_int_ena : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_fail_int_ena:1;
        /** l1_cache_fail_int_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable interrupt of access fail that occurs in L1-DCache due to
         *  cpu accesses L1-DCache.
         */
        uint32_t l1_cache_fail_int_ena:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_ena_reg_t;

/** Type of l1_cache_acs_fail_int_clr register
 *  L1-Cache Access Fail Interrupt clear register
 */
typedef union {
    struct {
        /** l1_icache0_fail_int_clr : HRO; bitpos: [0]; default: 0;
         *  The bit is used to clear interrupt of access fail that occurs in L1-ICache0 due to
         *  cpu accesses L1-ICache0.
         */
        uint32_t l1_icache0_fail_int_clr:1;
        /** l1_icache1_fail_int_clr : HRO; bitpos: [1]; default: 0;
         *  The bit is used to clear interrupt of access fail that occurs in L1-ICache1 due to
         *  cpu accesses L1-ICache1.
         */
        uint32_t l1_icache1_fail_int_clr:1;
        /** l1_icache2_fail_int_clr : WT; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_fail_int_clr:1;
        /** l1_icache3_fail_int_clr : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_fail_int_clr:1;
        /** l1_cache_fail_int_clr : WT; bitpos: [4]; default: 0;
         *  The bit is used to clear interrupt of access fail that occurs in L1-DCache due to
         *  cpu accesses L1-DCache.
         */
        uint32_t l1_cache_fail_int_clr:1;
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
        /** l1_icache2_fail_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw bit of the interrupt of access fail that occurs in L1-ICache2.
         */
        uint32_t l1_icache2_fail_int_raw:1;
        /** l1_icache3_fail_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw bit of the interrupt of access fail that occurs in L1-ICache3.
         */
        uint32_t l1_icache3_fail_int_raw:1;
        /** l1_cache_fail_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit of the interrupt of access fail that occurs in L1-DCache.
         */
        uint32_t l1_cache_fail_int_raw:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_raw_reg_t;

/** Type of l1_cache_acs_fail_int_st register
 *  Cache Access Fail Interrupt status register
 */
typedef union {
    struct {
        /** l1_icache0_fail_int_st : HRO; bitpos: [0]; default: 0;
         *  The bit indicates the interrupt status of access fail that occurs in L1-ICache0 due
         *  to cpu accesses L1-ICache.
         */
        uint32_t l1_icache0_fail_int_st:1;
        /** l1_icache1_fail_int_st : HRO; bitpos: [1]; default: 0;
         *  The bit indicates the interrupt status of access fail that occurs in L1-ICache1 due
         *  to cpu accesses L1-ICache.
         */
        uint32_t l1_icache1_fail_int_st:1;
        /** l1_icache2_fail_int_st : RO; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_fail_int_st:1;
        /** l1_icache3_fail_int_st : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_fail_int_st:1;
        /** l1_cache_fail_int_st : RO; bitpos: [4]; default: 0;
         *  The bit indicates the interrupt status of access fail that occurs in L1-DCache due
         *  to cpu accesses L1-DCache.
         */
        uint32_t l1_cache_fail_int_st:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_int_st_reg_t;

/** Type of l1_cache_sync_preload_int_ena register
 *  L1-Cache Access Fail Interrupt enable register
 */
typedef union {
    struct {
        /** l1_icache0_pld_done_int_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache0 preload-operation. If preload
         *  operation is done, interrupt occurs.
         */
        uint32_t l1_icache0_pld_done_int_ena:1;
        /** l1_icache1_pld_done_int_ena : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache1 preload-operation. If preload
         *  operation is done, interrupt occurs.
         */
        uint32_t l1_icache1_pld_done_int_ena:1;
        /** l1_icache2_pld_done_int_ena : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_done_int_ena:1;
        /** l1_icache3_pld_done_int_ena : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_done_int_ena:1;
        /** l1_cache_pld_done_int_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable interrupt of L1-Cache preload-operation. If preload
         *  operation is done, interrupt occurs.
         */
        uint32_t l1_cache_pld_done_int_ena:1;
        uint32_t reserved_5:1;
        /** sync_done_int_ena : R/W; bitpos: [6]; default: 0;
         *  The bit is used to enable interrupt of Cache sync-operation done.
         */
        uint32_t sync_done_int_ena:1;
        /** l1_icache0_pld_err_int_ena : HRO; bitpos: [7]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache0 preload-operation error.
         */
        uint32_t l1_icache0_pld_err_int_ena:1;
        /** l1_icache1_pld_err_int_ena : HRO; bitpos: [8]; default: 0;
         *  The bit is used to enable interrupt of L1-ICache1 preload-operation error.
         */
        uint32_t l1_icache1_pld_err_int_ena:1;
        /** l1_icache2_pld_err_int_ena : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_err_int_ena:1;
        /** l1_icache3_pld_err_int_ena : HRO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_err_int_ena:1;
        /** l1_cache_pld_err_int_ena : R/W; bitpos: [11]; default: 0;
         *  The bit is used to enable interrupt of L1-Cache preload-operation error.
         */
        uint32_t l1_cache_pld_err_int_ena:1;
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
        /** l1_icache0_pld_done_int_clr : HRO; bitpos: [0]; default: 0;
         *  The bit is used to clear interrupt that occurs only when L1-ICache0
         *  preload-operation is done.
         */
        uint32_t l1_icache0_pld_done_int_clr:1;
        /** l1_icache1_pld_done_int_clr : HRO; bitpos: [1]; default: 0;
         *  The bit is used to clear interrupt that occurs only when L1-ICache1
         *  preload-operation is done.
         */
        uint32_t l1_icache1_pld_done_int_clr:1;
        /** l1_icache2_pld_done_int_clr : WT; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_done_int_clr:1;
        /** l1_icache3_pld_done_int_clr : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_done_int_clr:1;
        /** l1_cache_pld_done_int_clr : WT; bitpos: [4]; default: 0;
         *  The bit is used to clear interrupt that occurs only when L1-Cache preload-operation
         *  is done.
         */
        uint32_t l1_cache_pld_done_int_clr:1;
        uint32_t reserved_5:1;
        /** sync_done_int_clr : WT; bitpos: [6]; default: 0;
         *  The bit is used to clear interrupt that occurs only when Cache sync-operation is
         *  done.
         */
        uint32_t sync_done_int_clr:1;
        /** l1_icache0_pld_err_int_clr : HRO; bitpos: [7]; default: 0;
         *  The bit is used to clear interrupt of L1-ICache0 preload-operation error.
         */
        uint32_t l1_icache0_pld_err_int_clr:1;
        /** l1_icache1_pld_err_int_clr : HRO; bitpos: [8]; default: 0;
         *  The bit is used to clear interrupt of L1-ICache1 preload-operation error.
         */
        uint32_t l1_icache1_pld_err_int_clr:1;
        /** l1_icache2_pld_err_int_clr : WT; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_err_int_clr:1;
        /** l1_icache3_pld_err_int_clr : HRO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_err_int_clr:1;
        /** l1_cache_pld_err_int_clr : WT; bitpos: [11]; default: 0;
         *  The bit is used to clear interrupt of L1-Cache preload-operation error.
         */
        uint32_t l1_cache_pld_err_int_clr:1;
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
        /** l1_icache2_pld_done_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_done_int_raw:1;
        /** l1_icache3_pld_done_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_done_int_raw:1;
        /** l1_cache_pld_done_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-Cache preload-operation is
         *  done.
         */
        uint32_t l1_cache_pld_done_int_raw:1;
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
        /** l1_icache2_pld_err_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_err_int_raw:1;
        /** l1_icache3_pld_err_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_err_int_raw:1;
        /** l1_cache_pld_err_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw bit of the interrupt that occurs only when L1-Cache preload-operation error
         *  occurs.
         */
        uint32_t l1_cache_pld_err_int_raw:1;
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
        /** l1_icache0_pld_done_int_st : HRO; bitpos: [0]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when L1-ICache0
         *  preload-operation is done.
         */
        uint32_t l1_icache0_pld_done_int_st:1;
        /** l1_icache1_pld_done_int_st : HRO; bitpos: [1]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when L1-ICache1
         *  preload-operation is done.
         */
        uint32_t l1_icache1_pld_done_int_st:1;
        /** l1_icache2_pld_done_int_st : RO; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_done_int_st:1;
        /** l1_icache3_pld_done_int_st : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_done_int_st:1;
        /** l1_cache_pld_done_int_st : RO; bitpos: [4]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when L1-Cache
         *  preload-operation is done.
         */
        uint32_t l1_cache_pld_done_int_st:1;
        uint32_t reserved_5:1;
        /** sync_done_int_st : RO; bitpos: [6]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when Cache
         *  sync-operation is done.
         */
        uint32_t sync_done_int_st:1;
        /** l1_icache0_pld_err_int_st : HRO; bitpos: [7]; default: 0;
         *  The bit indicates the status of the interrupt of L1-ICache0 preload-operation error.
         */
        uint32_t l1_icache0_pld_err_int_st:1;
        /** l1_icache1_pld_err_int_st : HRO; bitpos: [8]; default: 0;
         *  The bit indicates the status of the interrupt of L1-ICache1 preload-operation error.
         */
        uint32_t l1_icache1_pld_err_int_st:1;
        /** l1_icache2_pld_err_int_st : RO; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_err_int_st:1;
        /** l1_icache3_pld_err_int_st : HRO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_err_int_st:1;
        /** l1_cache_pld_err_int_st : RO; bitpos: [11]; default: 0;
         *  The bit indicates the status of the interrupt of L1-Cache preload-operation error.
         */
        uint32_t l1_cache_pld_err_int_st:1;
        uint32_t reserved_12:1;
        /** sync_err_int_st : RO; bitpos: [13]; default: 0;
         *  The bit indicates the status of the interrupt of Cache sync-operation error.
         */
        uint32_t sync_err_int_st:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} cache_l1_cache_sync_preload_int_st_reg_t;

/** Type of l2_cache_acs_cnt_int_ena register
 *  Cache Access Counter Interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** l2_ibus0_ovf_int_ena : HRO; bitpos: [8]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L2-Cache due to bus0 accesses L2-Cache.
         */
        uint32_t l2_ibus0_ovf_int_ena:1;
        /** l2_ibus1_ovf_int_ena : HRO; bitpos: [9]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L2-Cache due to bus1 accesses L2-Cache.
         */
        uint32_t l2_ibus1_ovf_int_ena:1;
        /** l2_ibus2_ovf_int_ena : HRO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus2_ovf_int_ena:1;
        /** l2_ibus3_ovf_int_ena : HRO; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus3_ovf_int_ena:1;
        /** l2_dbus0_ovf_int_ena : HRO; bitpos: [12]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L2-Cache due to bus0 accesses L2-Cache.
         */
        uint32_t l2_dbus0_ovf_int_ena:1;
        /** l2_dbus1_ovf_int_ena : HRO; bitpos: [13]; default: 0;
         *  The bit is used to enable interrupt of one of counters overflow that occurs in
         *  L2-Cache due to bus1 accesses L2-Cache.
         */
        uint32_t l2_dbus1_ovf_int_ena:1;
        /** l2_dbus2_ovf_int_ena : HRO; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus2_ovf_int_ena:1;
        /** l2_dbus3_ovf_int_ena : HRO; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus3_ovf_int_ena:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l2_cache_acs_cnt_int_ena_reg_t;

/** Type of l2_cache_acs_cnt_int_clr register
 *  Cache Access Counter Interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** l2_ibus0_ovf_int_clr : HRO; bitpos: [8]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
         *  to bus0 accesses L2-Cache.
         */
        uint32_t l2_ibus0_ovf_int_clr:1;
        /** l2_ibus1_ovf_int_clr : HRO; bitpos: [9]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
         *  to bus1 accesses L2-Cache.
         */
        uint32_t l2_ibus1_ovf_int_clr:1;
        /** l2_ibus2_ovf_int_clr : HRO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus2_ovf_int_clr:1;
        /** l2_ibus3_ovf_int_clr : HRO; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus3_ovf_int_clr:1;
        /** l2_dbus0_ovf_int_clr : HRO; bitpos: [12]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
         *  to bus0 accesses L2-Cache.
         */
        uint32_t l2_dbus0_ovf_int_clr:1;
        /** l2_dbus1_ovf_int_clr : HRO; bitpos: [13]; default: 0;
         *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
         *  to bus1 accesses L2-Cache.
         */
        uint32_t l2_dbus1_ovf_int_clr:1;
        /** l2_dbus2_ovf_int_clr : HRO; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus2_ovf_int_clr:1;
        /** l2_dbus3_ovf_int_clr : HRO; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus3_ovf_int_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l2_cache_acs_cnt_int_clr_reg_t;

/** Type of l2_cache_acs_cnt_int_raw register
 *  Cache Access Counter Interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** l2_ibus0_ovf_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus0 accesses L2-ICache0.
         */
        uint32_t l2_ibus0_ovf_int_raw:1;
        /** l2_ibus1_ovf_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus1 accesses L2-ICache1.
         */
        uint32_t l2_ibus1_ovf_int_raw:1;
        /** l2_ibus2_ovf_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus2 accesses L2-ICache2.
         */
        uint32_t l2_ibus2_ovf_int_raw:1;
        /** l2_ibus3_ovf_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus3 accesses L2-ICache3.
         */
        uint32_t l2_ibus3_ovf_int_raw:1;
        /** l2_dbus0_ovf_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus0 accesses L2-DCache.
         */
        uint32_t l2_dbus0_ovf_int_raw:1;
        /** l2_dbus1_ovf_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus1 accesses L2-DCache.
         */
        uint32_t l2_dbus1_ovf_int_raw:1;
        /** l2_dbus2_ovf_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus2 accesses L2-DCache.
         */
        uint32_t l2_dbus2_ovf_int_raw:1;
        /** l2_dbus3_ovf_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
         *  due to bus3 accesses L2-DCache.
         */
        uint32_t l2_dbus3_ovf_int_raw:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l2_cache_acs_cnt_int_raw_reg_t;

/** Type of l2_cache_acs_cnt_int_st register
 *  Cache Access Counter Interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** l2_ibus0_ovf_int_st : HRO; bitpos: [8]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L2-Cache due to bus0 accesses L2-Cache.
         */
        uint32_t l2_ibus0_ovf_int_st:1;
        /** l2_ibus1_ovf_int_st : HRO; bitpos: [9]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L2-Cache due to bus1 accesses L2-Cache.
         */
        uint32_t l2_ibus1_ovf_int_st:1;
        /** l2_ibus2_ovf_int_st : HRO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus2_ovf_int_st:1;
        /** l2_ibus3_ovf_int_st : HRO; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus3_ovf_int_st:1;
        /** l2_dbus0_ovf_int_st : HRO; bitpos: [12]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L2-Cache due to bus0 accesses L2-Cache.
         */
        uint32_t l2_dbus0_ovf_int_st:1;
        /** l2_dbus1_ovf_int_st : HRO; bitpos: [13]; default: 0;
         *  The bit indicates the interrupt status of one of counters overflow that occurs in
         *  L2-Cache due to bus1 accesses L2-Cache.
         */
        uint32_t l2_dbus1_ovf_int_st:1;
        /** l2_dbus2_ovf_int_st : HRO; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus2_ovf_int_st:1;
        /** l2_dbus3_ovf_int_st : HRO; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus3_ovf_int_st:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} cache_l2_cache_acs_cnt_int_st_reg_t;

/** Type of l2_cache_acs_fail_int_ena register
 *  Cache Access Fail Interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_fail_int_ena : HRO; bitpos: [5]; default: 0;
         *  The bit is used to enable interrupt of access fail that occurs in L2-Cache due to
         *  l1 cache accesses L2-Cache.
         */
        uint32_t l2_cache_fail_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_acs_fail_int_ena_reg_t;

/** Type of l2_cache_acs_fail_int_clr register
 *  L1-Cache Access Fail Interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_fail_int_clr : HRO; bitpos: [5]; default: 0;
         *  The bit is used to clear interrupt of access fail that occurs in L2-Cache due to l1
         *  cache accesses L2-Cache.
         */
        uint32_t l2_cache_fail_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_acs_fail_int_clr_reg_t;

/** Type of l2_cache_acs_fail_int_raw register
 *  Cache Access Fail Interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_fail_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw bit of the interrupt of access fail that occurs in L2-Cache.
         */
        uint32_t l2_cache_fail_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_acs_fail_int_raw_reg_t;

/** Type of l2_cache_acs_fail_int_st register
 *  Cache Access Fail Interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_fail_int_st : HRO; bitpos: [5]; default: 0;
         *  The bit indicates the interrupt status of access fail that occurs in L2-Cache due
         *  to l1 cache accesses L2-Cache.
         */
        uint32_t l2_cache_fail_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_acs_fail_int_st_reg_t;

/** Type of l2_cache_sync_preload_int_ena register
 *  L1-Cache Access Fail Interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_pld_done_int_ena : HRO; bitpos: [5]; default: 0;
         *  The bit is used to enable interrupt of L2-Cache preload-operation done.
         */
        uint32_t l2_cache_pld_done_int_ena:1;
        uint32_t reserved_6:6;
        /** l2_cache_pld_err_int_ena : HRO; bitpos: [12]; default: 0;
         *  The bit is used to enable interrupt of L2-Cache preload-operation error.
         */
        uint32_t l2_cache_pld_err_int_ena:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cache_l2_cache_sync_preload_int_ena_reg_t;

/** Type of l2_cache_sync_preload_int_clr register
 *  Sync Preload operation Interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_pld_done_int_clr : HRO; bitpos: [5]; default: 0;
         *  The bit is used to clear interrupt that occurs only when L2-Cache preload-operation
         *  is done.
         */
        uint32_t l2_cache_pld_done_int_clr:1;
        uint32_t reserved_6:6;
        /** l2_cache_pld_err_int_clr : HRO; bitpos: [12]; default: 0;
         *  The bit is used to clear interrupt of L2-Cache preload-operation error.
         */
        uint32_t l2_cache_pld_err_int_clr:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cache_l2_cache_sync_preload_int_clr_reg_t;

/** Type of l2_cache_sync_preload_int_raw register
 *  Sync Preload operation Interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_pld_done_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw bit of the interrupt that occurs only when L2-Cache preload-operation is
         *  done.
         */
        uint32_t l2_cache_pld_done_int_raw:1;
        uint32_t reserved_6:6;
        /** l2_cache_pld_err_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw bit of the interrupt that occurs only when L2-Cache preload-operation error
         *  occurs.
         */
        uint32_t l2_cache_pld_err_int_raw:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cache_l2_cache_sync_preload_int_raw_reg_t;

/** Type of l2_cache_sync_preload_int_st register
 *  L1-Cache Access Fail Interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_pld_done_int_st : HRO; bitpos: [5]; default: 0;
         *  The bit indicates the status of the interrupt that occurs only when L2-Cache
         *  preload-operation is done.
         */
        uint32_t l2_cache_pld_done_int_st:1;
        uint32_t reserved_6:6;
        /** l2_cache_pld_err_int_st : HRO; bitpos: [12]; default: 0;
         *  The bit indicates the status of the interrupt of L2-Cache preload-operation error.
         */
        uint32_t l2_cache_pld_err_int_st:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cache_l2_cache_sync_preload_int_st_reg_t;


/** Group: Cache Access Fail Configuration register */
/** Type of l1_cache_acs_fail_ctrl register
 *  Cache Access Fail Configuration register
 */
typedef union {
    struct {
        /** l1_icache0_acs_fail_check_mode : HRO; bitpos: [0]; default: 0;
         *  The bit is used to configure l1 icache0 access fail check mode. 0: the access fail
         *  is not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_icache0_acs_fail_check_mode:1;
        /** l1_icache1_acs_fail_check_mode : HRO; bitpos: [1]; default: 0;
         *  The bit is used to configure l1 icache1 access fail check mode. 0: the access fail
         *  is not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_icache1_acs_fail_check_mode:1;
        /** l1_icache2_acs_fail_check_mode : R/W; bitpos: [2]; default: 0;
         *  The bit is used to configure l1 icache2 access fail check mode. 0: the access fail
         *  is not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_icache2_acs_fail_check_mode:1;
        /** l1_icache3_acs_fail_check_mode : HRO; bitpos: [3]; default: 0;
         *  The bit is used to configure l1 icache3 access fail check mode. 0: the access fail
         *  is not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_icache3_acs_fail_check_mode:1;
        /** l1_cache_acs_fail_check_mode : R/W; bitpos: [4]; default: 0;
         *  The bit is used to configure l1 cache access fail check mode. 0: the access fail is
         *  not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l1_cache_acs_fail_check_mode:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_acs_fail_ctrl_reg_t;

/** Type of l2_cache_acs_fail_ctrl register
 *  Cache Access Fail Configuration register
 */
typedef union {
    struct {
        /** l2_cache_acs_fail_check_mode : HRO; bitpos: [0]; default: 0;
         *  The bit is used to configure l2 cache access fail check mode. 0: the access fail is
         *  not propagated to the request, 1: the access fail is propagated to the request
         */
        uint32_t l2_cache_acs_fail_check_mode:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cache_l2_cache_acs_fail_ctrl_reg_t;


/** Group: Access Statistics registers */
/** Type of l1_cache_acs_cnt_ctrl register
 *  Cache Access Counter enable and clear register
 */
typedef union {
    struct {
        /** l1_ibus0_cnt_ena : HRO; bitpos: [0]; default: 0;
         *  The bit is used to enable ibus0 counter in L1-ICache0.
         */
        uint32_t l1_ibus0_cnt_ena:1;
        /** l1_ibus1_cnt_ena : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable ibus1 counter in L1-ICache1.
         */
        uint32_t l1_ibus1_cnt_ena:1;
        /** l1_ibus2_cnt_ena : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus2_cnt_ena:1;
        /** l1_ibus3_cnt_ena : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus3_cnt_ena:1;
        /** l1_bus0_cnt_ena : R/W; bitpos: [4]; default: 0;
         *  The bit is used to enable dbus0 counter in L1-DCache.
         */
        uint32_t l1_bus0_cnt_ena:1;
        /** l1_bus1_cnt_ena : R/W; bitpos: [5]; default: 0;
         *  The bit is used to enable dbus1 counter in L1-DCache.
         */
        uint32_t l1_bus1_cnt_ena:1;
        /** l1_dbus2_cnt_ena : HRO; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus2_cnt_ena:1;
        /** l1_dbus3_cnt_ena : HRO; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus3_cnt_ena:1;
        uint32_t reserved_8:8;
        /** l1_ibus0_cnt_clr : HRO; bitpos: [16]; default: 0;
         *  The bit is used to clear ibus0 counter in L1-ICache0.
         */
        uint32_t l1_ibus0_cnt_clr:1;
        /** l1_ibus1_cnt_clr : HRO; bitpos: [17]; default: 0;
         *  The bit is used to clear ibus1 counter in L1-ICache1.
         */
        uint32_t l1_ibus1_cnt_clr:1;
        /** l1_ibus2_cnt_clr : WT; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus2_cnt_clr:1;
        /** l1_ibus3_cnt_clr : WT; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t l1_ibus3_cnt_clr:1;
        /** l1_bus0_cnt_clr : WT; bitpos: [20]; default: 0;
         *  The bit is used to clear dbus0 counter in L1-DCache.
         */
        uint32_t l1_bus0_cnt_clr:1;
        /** l1_bus1_cnt_clr : WT; bitpos: [21]; default: 0;
         *  The bit is used to clear dbus1 counter in L1-DCache.
         */
        uint32_t l1_bus1_cnt_clr:1;
        /** l1_dbus2_cnt_clr : HRO; bitpos: [22]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus2_cnt_clr:1;
        /** l1_dbus3_cnt_clr : HRO; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t l1_dbus3_cnt_clr:1;
        uint32_t reserved_24:8;
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

/** Type of l1_ibus2_acs_hit_cnt register
 *  L1-ICache bus2 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus2_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus2 accesses L1-ICache2.
         */
        uint32_t l1_ibus2_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus2_acs_hit_cnt_reg_t;

/** Type of l1_ibus2_acs_miss_cnt register
 *  L1-ICache bus2 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus2_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus2 accesses L1-ICache2.
         */
        uint32_t l1_ibus2_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus2_acs_miss_cnt_reg_t;

/** Type of l1_ibus2_acs_conflict_cnt register
 *  L1-ICache bus2 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus2_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus2 accesses L1-ICache2.
         */
        uint32_t l1_ibus2_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus2_acs_conflict_cnt_reg_t;

/** Type of l1_ibus2_acs_nxtlvl_rd_cnt register
 *  L1-ICache bus2 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus2_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-ICache accesses L2-Cache due to
         *  bus2 accessing L1-ICache2.
         */
        uint32_t l1_ibus2_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus2_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_ibus3_acs_hit_cnt register
 *  L1-ICache bus3 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus3_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus3 accesses L1-ICache3.
         */
        uint32_t l1_ibus3_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus3_acs_hit_cnt_reg_t;

/** Type of l1_ibus3_acs_miss_cnt register
 *  L1-ICache bus3 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus3_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus3 accesses L1-ICache3.
         */
        uint32_t l1_ibus3_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus3_acs_miss_cnt_reg_t;

/** Type of l1_ibus3_acs_conflict_cnt register
 *  L1-ICache bus3 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus3_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus3 accesses L1-ICache3.
         */
        uint32_t l1_ibus3_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus3_acs_conflict_cnt_reg_t;

/** Type of l1_ibus3_acs_nxtlvl_rd_cnt register
 *  L1-ICache bus3 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_ibus3_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-ICache accesses L2-Cache due to
         *  bus3 accessing L1-ICache3.
         */
        uint32_t l1_ibus3_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_ibus3_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_bus0_acs_hit_cnt register
 *  L1-Cache bus0 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus0_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus0 accesses L1-Cache.
         */
        uint32_t l1_bus0_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_hit_cnt_reg_t;

/** Type of l1_bus0_acs_miss_cnt register
 *  L1-Cache bus0 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus0_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus0 accesses L1-Cache.
         */
        uint32_t l1_bus0_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_miss_cnt_reg_t;

/** Type of l1_bus0_acs_conflict_cnt register
 *  L1-Cache bus0 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus0_conflict_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus0 accesses L1-Cache.
         */
        uint32_t l1_bus0_conflict_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_bus0_acs_conflict_cnt_reg_t;

/** Type of l1_dbus0_acs_nxtlvl_rd_cnt register
 *  L1-Cache bus0 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus0_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-Cache accesses L2-Cache due to
         *  bus0 accessing L1-Cache.
         */
        uint32_t l1_bus0_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus0_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_dbus0_acs_nxtlvl_wr_cnt register
 *  L1-DCache bus0 WB-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus0_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when bus0 accesses L1-Cache.
         */
        uint32_t l1_bus0_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus0_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l1_bus1_acs_hit_cnt register
 *  L1-Cache bus1 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus1_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus1 accesses L1-Cache.
         */
        uint32_t l1_bus1_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_bus1_acs_hit_cnt_reg_t;

/** Type of l1_bus1_acs_miss_cnt register
 *  L1-Cache bus1 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus1_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus1 accesses L1-Cache.
         */
        uint32_t l1_bus1_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_bus1_acs_miss_cnt_reg_t;

/** Type of l1_bus1_acs_conflict_cnt register
 *  L1-Cache bus1 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_bus1_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus1 accesses L1-Cache.
         */
        uint32_t l1_bus1_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_bus1_acs_conflict_cnt_reg_t;

/** Type of l1_dbus1_acs_nxtlvl_rd_cnt register
 *  L1-DCache bus1 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus1_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-Cache accesses L2-Cache due to
         *  bus1 accessing L1-Cache.
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
         *  The register records the number of write back when bus1 accesses L1-Cache.
         */
        uint32_t l1_dbus1_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus1_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l1_dbus2_acs_hit_cnt register
 *  L1-DCache bus2 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus2_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus2 accesses L1-DCache.
         */
        uint32_t l1_dbus2_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus2_acs_hit_cnt_reg_t;

/** Type of l1_dbus2_acs_miss_cnt register
 *  L1-DCache bus2 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus2_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus2 accesses L1-DCache.
         */
        uint32_t l1_dbus2_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus2_acs_miss_cnt_reg_t;

/** Type of l1_dbus2_acs_conflict_cnt register
 *  L1-DCache bus2 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus2_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus2 accesses L1-DCache.
         */
        uint32_t l1_dbus2_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus2_acs_conflict_cnt_reg_t;

/** Type of l1_dbus2_acs_nxtlvl_rd_cnt register
 *  L1-DCache bus2 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus2_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-Cache accesses L2-Cache due to
         *  bus2 accessing L1-Cache.
         */
        uint32_t l1_dbus2_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus2_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_dbus2_acs_nxtlvl_wr_cnt register
 *  L1-DCache bus2 WB-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus2_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when bus2 accesses L1-Cache.
         */
        uint32_t l1_dbus2_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus2_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l1_dbus3_acs_hit_cnt register
 *  L1-DCache bus3 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus3_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when bus3 accesses L1-DCache.
         */
        uint32_t l1_dbus3_hit_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus3_acs_hit_cnt_reg_t;

/** Type of l1_dbus3_acs_miss_cnt register
 *  L1-DCache bus3 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus3_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when bus3 accesses L1-DCache.
         */
        uint32_t l1_dbus3_miss_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus3_acs_miss_cnt_reg_t;

/** Type of l1_dbus3_acs_conflict_cnt register
 *  L1-DCache bus3 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus3_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when bus3 accesses L1-DCache.
         */
        uint32_t l1_dbus3_conflict_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus3_acs_conflict_cnt_reg_t;

/** Type of l1_dbus3_acs_nxtlvl_rd_cnt register
 *  L1-DCache bus3 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus3_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L1-Cache accesses L2-Cache due to
         *  bus3 accessing L1-Cache.
         */
        uint32_t l1_dbus3_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus3_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l1_dbus3_acs_nxtlvl_wr_cnt register
 *  L1-DCache bus3 WB-Access Counter register
 */
typedef union {
    struct {
        /** l1_dbus3_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when bus0 accesses L1-Cache.
         */
        uint32_t l1_dbus3_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l1_dbus3_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l2_cache_acs_cnt_ctrl register
 *  Cache Access Counter enable and clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** l2_ibus0_cnt_ena : HRO; bitpos: [8]; default: 0;
         *  The bit is used to enable ibus0 counter in L2-Cache.
         */
        uint32_t l2_ibus0_cnt_ena:1;
        /** l2_ibus1_cnt_ena : HRO; bitpos: [9]; default: 0;
         *  The bit is used to enable ibus1 counter in L2-Cache.
         */
        uint32_t l2_ibus1_cnt_ena:1;
        /** l2_ibus2_cnt_ena : HRO; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus2_cnt_ena:1;
        /** l2_ibus3_cnt_ena : HRO; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus3_cnt_ena:1;
        /** l2_dbus0_cnt_ena : HRO; bitpos: [12]; default: 0;
         *  The bit is used to enable dbus0 counter in L2-Cache.
         */
        uint32_t l2_dbus0_cnt_ena:1;
        /** l2_dbus1_cnt_ena : HRO; bitpos: [13]; default: 0;
         *  The bit is used to enable dbus1 counter in L2-Cache.
         */
        uint32_t l2_dbus1_cnt_ena:1;
        /** l2_dbus2_cnt_ena : HRO; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus2_cnt_ena:1;
        /** l2_dbus3_cnt_ena : HRO; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus3_cnt_ena:1;
        uint32_t reserved_16:8;
        /** l2_ibus0_cnt_clr : HRO; bitpos: [24]; default: 0;
         *  The bit is used to clear ibus0 counter in L2-Cache.
         */
        uint32_t l2_ibus0_cnt_clr:1;
        /** l2_ibus1_cnt_clr : HRO; bitpos: [25]; default: 0;
         *  The bit is used to clear ibus1 counter in L2-Cache.
         */
        uint32_t l2_ibus1_cnt_clr:1;
        /** l2_ibus2_cnt_clr : HRO; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus2_cnt_clr:1;
        /** l2_ibus3_cnt_clr : HRO; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t l2_ibus3_cnt_clr:1;
        /** l2_dbus0_cnt_clr : HRO; bitpos: [28]; default: 0;
         *  The bit is used to clear dbus0 counter in L2-Cache.
         */
        uint32_t l2_dbus0_cnt_clr:1;
        /** l2_dbus1_cnt_clr : HRO; bitpos: [29]; default: 0;
         *  The bit is used to clear dbus1 counter in L2-Cache.
         */
        uint32_t l2_dbus1_cnt_clr:1;
        /** l2_dbus2_cnt_clr : HRO; bitpos: [30]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus2_cnt_clr:1;
        /** l2_dbus3_cnt_clr : HRO; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t l2_dbus3_cnt_clr:1;
    };
    uint32_t val;
} cache_l2_cache_acs_cnt_ctrl_reg_t;

/** Type of l2_ibus0_acs_hit_cnt register
 *  L2-Cache bus0 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus0_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-ICache0 accesses L2-Cache due to
         *  bus0 accessing L1-ICache0.
         */
        uint32_t l2_ibus0_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus0_acs_hit_cnt_reg_t;

/** Type of l2_ibus0_acs_miss_cnt register
 *  L2-Cache bus0 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus0_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-ICache0 accesses L2-Cache due to
         *  bus0 accessing L1-ICache0.
         */
        uint32_t l2_ibus0_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus0_acs_miss_cnt_reg_t;

/** Type of l2_ibus0_acs_conflict_cnt register
 *  L2-Cache bus0 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus0_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-ICache0 accesses
         *  L2-Cache due to bus0 accessing L1-ICache0.
         */
        uint32_t l2_ibus0_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus0_acs_conflict_cnt_reg_t;

/** Type of l2_ibus0_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus0 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus0_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-ICache0 accessing L2-Cache due to bus0 accessing L1-ICache0.
         */
        uint32_t l2_ibus0_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus0_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_ibus1_acs_hit_cnt register
 *  L2-Cache bus1 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus1_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-ICache1 accesses L2-Cache due to
         *  bus1 accessing L1-ICache1.
         */
        uint32_t l2_ibus1_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus1_acs_hit_cnt_reg_t;

/** Type of l2_ibus1_acs_miss_cnt register
 *  L2-Cache bus1 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus1_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-ICache1 accesses L2-Cache due to
         *  bus1 accessing L1-ICache1.
         */
        uint32_t l2_ibus1_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus1_acs_miss_cnt_reg_t;

/** Type of l2_ibus1_acs_conflict_cnt register
 *  L2-Cache bus1 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus1_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-ICache1 accesses
         *  L2-Cache due to bus1 accessing L1-ICache1.
         */
        uint32_t l2_ibus1_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus1_acs_conflict_cnt_reg_t;

/** Type of l2_ibus1_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus1 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus1_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-ICache1 accessing L2-Cache due to bus1 accessing L1-ICache1.
         */
        uint32_t l2_ibus1_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus1_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_ibus2_acs_hit_cnt register
 *  L2-Cache bus2 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus2_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-ICache2 accesses L2-Cache due to
         *  bus2 accessing L1-ICache2.
         */
        uint32_t l2_ibus2_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus2_acs_hit_cnt_reg_t;

/** Type of l2_ibus2_acs_miss_cnt register
 *  L2-Cache bus2 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus2_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-ICache2 accesses L2-Cache due to
         *  bus2 accessing L1-ICache2.
         */
        uint32_t l2_ibus2_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus2_acs_miss_cnt_reg_t;

/** Type of l2_ibus2_acs_conflict_cnt register
 *  L2-Cache bus2 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus2_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-ICache2 accesses
         *  L2-Cache due to bus2 accessing L1-ICache2.
         */
        uint32_t l2_ibus2_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus2_acs_conflict_cnt_reg_t;

/** Type of l2_ibus2_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus2 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus2_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-ICache2 accessing L2-Cache due to bus2 accessing L1-ICache2.
         */
        uint32_t l2_ibus2_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus2_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_ibus3_acs_hit_cnt register
 *  L2-Cache bus3 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus3_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-ICache3 accesses L2-Cache due to
         *  bus3 accessing L1-ICache3.
         */
        uint32_t l2_ibus3_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus3_acs_hit_cnt_reg_t;

/** Type of l2_ibus3_acs_miss_cnt register
 *  L2-Cache bus3 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus3_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-ICache3 accesses L2-Cache due to
         *  bus3 accessing L1-ICache3.
         */
        uint32_t l2_ibus3_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus3_acs_miss_cnt_reg_t;

/** Type of l2_ibus3_acs_conflict_cnt register
 *  L2-Cache bus3 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus3_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-ICache3 accesses
         *  L2-Cache due to bus3 accessing L1-ICache3.
         */
        uint32_t l2_ibus3_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus3_acs_conflict_cnt_reg_t;

/** Type of l2_ibus3_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus3 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_ibus3_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-ICache3 accessing L2-Cache due to bus3 accessing L1-ICache3.
         */
        uint32_t l2_ibus3_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_ibus3_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_dbus0_acs_hit_cnt register
 *  L2-Cache bus0 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus0_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-DCache accesses L2-Cache due to
         *  bus0 accessing L1-DCache.
         */
        uint32_t l2_dbus0_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus0_acs_hit_cnt_reg_t;

/** Type of l2_dbus0_acs_miss_cnt register
 *  L2-Cache bus0 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus0_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-DCache accesses L2-Cache due to
         *  bus0 accessing L1-DCache.
         */
        uint32_t l2_dbus0_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus0_acs_miss_cnt_reg_t;

/** Type of l2_dbus0_acs_conflict_cnt register
 *  L2-Cache bus0 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus0_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-DCache accesses
         *  L2-Cache due to bus0 accessing L1-DCache.
         */
        uint32_t l2_dbus0_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus0_acs_conflict_cnt_reg_t;

/** Type of l2_dbus0_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus0 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus0_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-DCache accessing L2-Cache due to bus0 accessing L1-DCache.
         */
        uint32_t l2_dbus0_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus0_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_dbus0_acs_nxtlvl_wr_cnt register
 *  L2-Cache bus0 WB-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus0_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when L1-DCache accesses L2-Cache due
         *  to bus0 accessing L1-DCache.
         */
        uint32_t l2_dbus0_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus0_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l2_dbus1_acs_hit_cnt register
 *  L2-Cache bus1 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus1_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-DCache accesses L2-Cache due to
         *  bus1 accessing L1-DCache.
         */
        uint32_t l2_dbus1_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus1_acs_hit_cnt_reg_t;

/** Type of l2_dbus1_acs_miss_cnt register
 *  L2-Cache bus1 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus1_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-DCache accesses L2-Cache due to
         *  bus1 accessing L1-DCache.
         */
        uint32_t l2_dbus1_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus1_acs_miss_cnt_reg_t;

/** Type of l2_dbus1_acs_conflict_cnt register
 *  L2-Cache bus1 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus1_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-DCache accesses
         *  L2-Cache due to bus1 accessing L1-DCache.
         */
        uint32_t l2_dbus1_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus1_acs_conflict_cnt_reg_t;

/** Type of l2_dbus1_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus1 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus1_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-DCache accessing L2-Cache due to bus1 accessing L1-DCache.
         */
        uint32_t l2_dbus1_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus1_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_dbus1_acs_nxtlvl_wr_cnt register
 *  L2-Cache bus1 WB-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus1_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when L1-DCache accesses L2-Cache due
         *  to bus1 accessing L1-DCache.
         */
        uint32_t l2_dbus1_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus1_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l2_dbus2_acs_hit_cnt register
 *  L2-Cache bus2 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus2_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-DCache accesses L2-Cache due to
         *  bus2 accessing L1-DCache.
         */
        uint32_t l2_dbus2_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus2_acs_hit_cnt_reg_t;

/** Type of l2_dbus2_acs_miss_cnt register
 *  L2-Cache bus2 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus2_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-DCache accesses L2-Cache due to
         *  bus2 accessing L1-DCache.
         */
        uint32_t l2_dbus2_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus2_acs_miss_cnt_reg_t;

/** Type of l2_dbus2_acs_conflict_cnt register
 *  L2-Cache bus2 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus2_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-DCache accesses
         *  L2-Cache due to bus2 accessing L1-DCache.
         */
        uint32_t l2_dbus2_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus2_acs_conflict_cnt_reg_t;

/** Type of l2_dbus2_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus2 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus2_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-DCache accessing L2-Cache due to bus2 accessing L1-DCache.
         */
        uint32_t l2_dbus2_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus2_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_dbus2_acs_nxtlvl_wr_cnt register
 *  L2-Cache bus2 WB-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus2_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when L1-DCache accesses L2-Cache due
         *  to bus2 accessing L1-DCache.
         */
        uint32_t l2_dbus2_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus2_acs_nxtlvl_wr_cnt_reg_t;

/** Type of l2_dbus3_acs_hit_cnt register
 *  L2-Cache bus3 Hit-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus3_hit_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of hits when L1-DCache accesses L2-Cache due to
         *  bus3 accessing L1-DCache.
         */
        uint32_t l2_dbus3_hit_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus3_acs_hit_cnt_reg_t;

/** Type of l2_dbus3_acs_miss_cnt register
 *  L2-Cache bus3 Miss-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus3_miss_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of missing when L1-DCache accesses L2-Cache due to
         *  bus3 accessing L1-DCache.
         */
        uint32_t l2_dbus3_miss_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus3_acs_miss_cnt_reg_t;

/** Type of l2_dbus3_acs_conflict_cnt register
 *  L2-Cache bus3 Conflict-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus3_conflict_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of access-conflicts when L1-DCache accesses
         *  L2-Cache due to bus3 accessing L1-DCache.
         */
        uint32_t l2_dbus3_conflict_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus3_acs_conflict_cnt_reg_t;

/** Type of l2_dbus3_acs_nxtlvl_rd_cnt register
 *  L2-Cache bus3 Next-Level-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus3_nxtlvl_rd_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of times that L2-Cache accesses external memory due
         *  to L1-DCache accessing L2-Cache due to bus3 accessing L1-DCache.
         */
        uint32_t l2_dbus3_nxtlvl_rd_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus3_acs_nxtlvl_rd_cnt_reg_t;

/** Type of l2_dbus3_acs_nxtlvl_wr_cnt register
 *  L2-Cache bus3 WB-Access Counter register
 */
typedef union {
    struct {
        /** l2_dbus3_nxtlvl_wr_cnt : RO; bitpos: [31:0]; default: 0;
         *  The register records the number of write back when L1-DCache accesses L2-Cache due
         *  to bus3 accessing L1-DCache.
         */
        uint32_t l2_dbus3_nxtlvl_wr_cnt:32;
    };
    uint32_t val;
} cache_l2_dbus3_acs_nxtlvl_wr_cnt_reg_t;


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

/** Type of l1_icache2_acs_fail_id_attr register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
typedef union {
    struct {
        /** l1_icache2_fail_id : RO; bitpos: [15:0]; default: 0;
         *  The register records the ID of fail-access when cache2 accesses L1-ICache.
         */
        uint32_t l1_icache2_fail_id:16;
        /** l1_icache2_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  The register records the attribution of fail-access when cache2 accesses L1-ICache.
         */
        uint32_t l1_icache2_fail_attr:16;
    };
    uint32_t val;
} cache_l1_icache2_acs_fail_id_attr_reg_t;

/** Type of l1_icache2_acs_fail_addr register
 *  L1-ICache0 Access Fail Address information register
 */
typedef union {
    struct {
        /** l1_icache2_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  The register records the address of fail-access when cache2 accesses L1-ICache.
         */
        uint32_t l1_icache2_fail_addr:32;
    };
    uint32_t val;
} cache_l1_icache2_acs_fail_addr_reg_t;

/** Type of l1_icache3_acs_fail_id_attr register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
typedef union {
    struct {
        /** l1_icache3_fail_id : RO; bitpos: [15:0]; default: 0;
         *  The register records the ID of fail-access when cache3 accesses L1-ICache.
         */
        uint32_t l1_icache3_fail_id:16;
        /** l1_icache3_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  The register records the attribution of fail-access when cache3 accesses L1-ICache.
         */
        uint32_t l1_icache3_fail_attr:16;
    };
    uint32_t val;
} cache_l1_icache3_acs_fail_id_attr_reg_t;

/** Type of l1_icache3_acs_fail_addr register
 *  L1-ICache0 Access Fail Address information register
 */
typedef union {
    struct {
        /** l1_icache3_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  The register records the address of fail-access when cache3 accesses L1-ICache.
         */
        uint32_t l1_icache3_fail_addr:32;
    };
    uint32_t val;
} cache_l1_icache3_acs_fail_addr_reg_t;

/** Type of l1_dcache_acs_fail_id_attr register
 *  L1-Cache Access Fail ID/attribution information register
 */
typedef union {
    struct {
        /** l1_cache_fail_id : RO; bitpos: [15:0]; default: 0;
         *  The register records the ID of fail-access when cache accesses L1-Cache.
         */
        uint32_t l1_cache_fail_id:16;
        /** l1_cache_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  The register records the attribution of fail-access when cache accesses L1-Cache.
         */
        uint32_t l1_cache_fail_attr:16;
    };
    uint32_t val;
} cache_l1_dcache_acs_fail_id_attr_reg_t;

/** Type of l1_dcache_acs_fail_addr register
 *  L1-Cache Access Fail Address information register
 */
typedef union {
    struct {
        /** l1_cache_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  The register records the address of fail-access when cache accesses L1-Cache.
         */
        uint32_t l1_cache_fail_addr:32;
    };
    uint32_t val;
} cache_l1_dcache_acs_fail_addr_reg_t;

/** Type of l2_cache_acs_fail_id_attr register
 *  L2-Cache Access Fail ID/attribution information register
 */
typedef union {
    struct {
        /** l2_cache_fail_id : RO; bitpos: [15:0]; default: 0;
         *  The register records the ID of fail-access when L1-Cache accesses L2-Cache.
         */
        uint32_t l2_cache_fail_id:16;
        /** l2_cache_fail_attr : RO; bitpos: [31:16]; default: 0;
         *  The register records the attribution of fail-access when L1-Cache accesses L2-Cache
         *  due to cache accessing L1-Cache.
         */
        uint32_t l2_cache_fail_attr:16;
    };
    uint32_t val;
} cache_l2_cache_acs_fail_id_attr_reg_t;

/** Type of l2_cache_acs_fail_addr register
 *  L2-Cache Access Fail Address information register
 */
typedef union {
    struct {
        /** l2_cache_fail_addr : RO; bitpos: [31:0]; default: 0;
         *  The register records the address of fail-access when L1-Cache accesses L2-Cache.
         */
        uint32_t l2_cache_fail_addr:32;
    };
    uint32_t val;
} cache_l2_cache_acs_fail_addr_reg_t;


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
        /** l1_icache2_pld_err_code : RO; bitpos: [5:4]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_err_code:2;
        /** l1_icache3_pld_err_code : RO; bitpos: [7:6]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_err_code:2;
        /** l1_cache_pld_err_code : RO; bitpos: [9:8]; default: 0;
         *  The value 2 is Only available which means preload size is error in L1-Cache.
         */
        uint32_t l1_cache_pld_err_code:2;
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

/** Type of l2_cache_sync_preload_exception register
 *  Cache Sync/Preload Operation exception register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** l2_cache_pld_err_code : RO; bitpos: [11:10]; default: 0;
         *  The value 2 is Only available which means preload size is error in L2-Cache.
         */
        uint32_t l2_cache_pld_err_code:2;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} cache_l2_cache_sync_preload_exception_reg_t;


/** Group: Sync Reset control and configuration registers */
/** Type of l1_cache_sync_rst_ctrl register
 *  Cache Sync Reset control register
 */
typedef union {
    struct {
        /** l1_icache0_sync_rst : HRO; bitpos: [0]; default: 0;
         *  set this bit to reset sync-logic inside L1-ICache0. Recommend that this should only
         *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
         */
        uint32_t l1_icache0_sync_rst:1;
        /** l1_icache1_sync_rst : HRO; bitpos: [1]; default: 0;
         *  set this bit to reset sync-logic inside L1-ICache1. Recommend that this should only
         *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
         */
        uint32_t l1_icache1_sync_rst:1;
        /** l1_icache2_sync_rst : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_sync_rst:1;
        /** l1_icache3_sync_rst : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_sync_rst:1;
        /** l1_cache_sync_rst : R/W; bitpos: [4]; default: 0;
         *  set this bit to reset sync-logic inside L1-Cache. Recommend that this should only
         *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
         */
        uint32_t l1_cache_sync_rst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_sync_rst_ctrl_reg_t;

/** Type of l2_cache_sync_rst_ctrl register
 *  Cache Sync Reset control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_sync_rst : HRO; bitpos: [5]; default: 0;
         *  set this bit to reset sync-logic inside L2-Cache. Recommend that this should only
         *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
         */
        uint32_t l2_cache_sync_rst:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_sync_rst_ctrl_reg_t;


/** Group: Preload Reset control and configuration registers */
/** Type of l1_cache_preload_rst_ctrl register
 *  Cache Preload Reset control register
 */
typedef union {
    struct {
        /** l1_icache0_pld_rst : HRO; bitpos: [0]; default: 0;
         *  set this bit to reset preload-logic inside L1-ICache0. Recommend that this should
         *  only be used to initialize preload-logic when some fatal error of preload-logic
         *  occurs.
         */
        uint32_t l1_icache0_pld_rst:1;
        /** l1_icache1_pld_rst : HRO; bitpos: [1]; default: 0;
         *  set this bit to reset preload-logic inside L1-ICache1. Recommend that this should
         *  only be used to initialize preload-logic when some fatal error of preload-logic
         *  occurs.
         */
        uint32_t l1_icache1_pld_rst:1;
        /** l1_icache2_pld_rst : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_pld_rst:1;
        /** l1_icache3_pld_rst : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_pld_rst:1;
        /** l1_cache_pld_rst : R/W; bitpos: [4]; default: 0;
         *  set this bit to reset preload-logic inside L1-Cache. Recommend that this should
         *  only be used to initialize preload-logic when some fatal error of preload-logic
         *  occurs.
         */
        uint32_t l1_cache_pld_rst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_preload_rst_ctrl_reg_t;

/** Type of l2_cache_preload_rst_ctrl register
 *  Cache Preload Reset control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_pld_rst : HRO; bitpos: [5]; default: 0;
         *  set this bit to reset preload-logic inside L2-Cache. Recommend that this should
         *  only be used to initialize preload-logic when some fatal error of preload-logic
         *  occurs.
         */
        uint32_t l2_cache_pld_rst:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_preload_rst_ctrl_reg_t;


/** Group: Autoload buffer clear control and configuration registers */
/** Type of l1_cache_autoload_buf_clr_ctrl register
 *  Cache Autoload buffer clear control register
 */
typedef union {
    struct {
        /** l1_icache0_ald_buf_clr : HRO; bitpos: [0]; default: 0;
         *  set this bit to clear autoload-buffer inside L1-ICache0. If this bit is active,
         *  autoload will not work in L1-ICache0. This bit should not be active when autoload
         *  works in L1-ICache0.
         */
        uint32_t l1_icache0_ald_buf_clr:1;
        /** l1_icache1_ald_buf_clr : HRO; bitpos: [1]; default: 0;
         *  set this bit to clear autoload-buffer inside L1-ICache1. If this bit is active,
         *  autoload will not work in L1-ICache1. This bit should not be active when autoload
         *  works in L1-ICache1.
         */
        uint32_t l1_icache1_ald_buf_clr:1;
        /** l1_icache2_ald_buf_clr : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_ald_buf_clr:1;
        /** l1_icache3_ald_buf_clr : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_ald_buf_clr:1;
        /** l1_cache_ald_buf_clr : R/W; bitpos: [4]; default: 0;
         *  set this bit to clear autoload-buffer inside L1-Cache. If this bit is active,
         *  autoload will not work in L1-Cache. This bit should not be active when autoload
         *  works in L1-Cache.
         */
        uint32_t l1_cache_ald_buf_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_cache_autoload_buf_clr_ctrl_reg_t;

/** Type of l2_cache_autoload_buf_clr_ctrl register
 *  Cache Autoload buffer clear control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_ald_buf_clr : HRO; bitpos: [5]; default: 0;
         *  set this bit to clear autoload-buffer inside L2-Cache. If this bit is active,
         *  autoload will not work in L2-Cache. This bit should not be active when autoload
         *  works in L2-Cache.
         */
        uint32_t l2_cache_ald_buf_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_cache_autoload_buf_clr_ctrl_reg_t;


/** Group: Unallocate request buffer clear registers */
/** Type of l1_unallocate_buffer_clear register
 *  Unallocate request buffer clear registers
 */
typedef union {
    struct {
        /** l1_icache0_unalloc_clr : HRO; bitpos: [0]; default: 0;
         *  The bit is used to clear the unallocate request buffer of l1 icache0 where the
         *  unallocate request is responsed but not completed.
         */
        uint32_t l1_icache0_unalloc_clr:1;
        /** l1_icache1_unalloc_clr : HRO; bitpos: [1]; default: 0;
         *  The bit is used to clear the unallocate request buffer of l1 icache1 where the
         *  unallocate request is responsed but not completed.
         */
        uint32_t l1_icache1_unalloc_clr:1;
        /** l1_icache2_unalloc_clr : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_unalloc_clr:1;
        /** l1_icache3_unalloc_clr : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_unalloc_clr:1;
        /** l1_cache_unalloc_clr : R/W; bitpos: [4]; default: 0;
         *  The bit is used to clear the unallocate request buffer of l1 cache where the
         *  unallocate request is responsed but not completed.
         */
        uint32_t l1_cache_unalloc_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cache_l1_unallocate_buffer_clear_reg_t;

/** Type of l2_unallocate_buffer_clear register
 *  Unallocate request buffer clear registers
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_unalloc_clr : HRO; bitpos: [5]; default: 0;
         *  The bit is used to clear the unallocate request buffer of l2 icache where the
         *  unallocate request is responsed but not completed.
         */
        uint32_t l2_cache_unalloc_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} cache_l2_unallocate_buffer_clear_reg_t;


/** Group: Tag and Data Memory Access Control and configuration register */
/** Type of l1_cache_object_ctrl register
 *  Cache Tag and Data memory Object control register
 */
typedef union {
    struct {
        /** l1_icache0_tag_object : HRO; bitpos: [0]; default: 0;
         *  Set this bit to set L1-ICache0 tag memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_icache0_tag_object:1;
        /** l1_icache1_tag_object : HRO; bitpos: [1]; default: 0;
         *  Set this bit to set L1-ICache1 tag memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_icache1_tag_object:1;
        /** l1_icache2_tag_object : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_tag_object:1;
        /** l1_icache3_tag_object : HRO; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_tag_object:1;
        /** l1_cache_tag_object : R/W; bitpos: [4]; default: 0;
         *  Set this bit to set L1-Cache tag memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_cache_tag_object:1;
        uint32_t reserved_5:1;
        /** l1_icache0_mem_object : HRO; bitpos: [6]; default: 0;
         *  Set this bit to set L1-ICache0 data memory as object. This bit should be onehot
         *  with the others fields inside this register.
         */
        uint32_t l1_icache0_mem_object:1;
        /** l1_icache1_mem_object : HRO; bitpos: [7]; default: 0;
         *  Set this bit to set L1-ICache1 data memory as object. This bit should be onehot
         *  with the others fields inside this register.
         */
        uint32_t l1_icache1_mem_object:1;
        /** l1_icache2_mem_object : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache2_mem_object:1;
        /** l1_icache3_mem_object : HRO; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t l1_icache3_mem_object:1;
        /** l1_cache_mem_object : R/W; bitpos: [10]; default: 0;
         *  Set this bit to set L1-Cache data memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l1_cache_mem_object:1;
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
        /** l1_cache_addr : R/W; bitpos: [31:0]; default: 0;
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
        /** l1_cache_debug_bus : R/W; bitpos: [31:0]; default: 612;
         *  This is a constant place where we can write data to or read data from the tag/data
         *  memory on the specified cache.
         */
        uint32_t l1_cache_debug_bus:32;
    };
    uint32_t val;
} cache_l1_cache_debug_bus_reg_t;

/** Type of l2_cache_object_ctrl register
 *  Cache Tag and Data memory Object control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** l2_cache_tag_object : HRO; bitpos: [5]; default: 0;
         *  Set this bit to set L2-Cache tag memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l2_cache_tag_object:1;
        uint32_t reserved_6:5;
        /** l2_cache_mem_object : HRO; bitpos: [11]; default: 0;
         *  Set this bit to set L2-Cache data memory as object. This bit should be onehot with
         *  the others fields inside this register.
         */
        uint32_t l2_cache_mem_object:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} cache_l2_cache_object_ctrl_reg_t;

/** Type of l2_cache_way_object register
 *  Cache Tag and Data memory way register
 */
typedef union {
    struct {
        /** l2_cache_way_object : HRO; bitpos: [2:0]; default: 0;
         *  Set this bits to select which way of the tag-object will be accessed. 0: way0, 1:
         *  way1, 2: way2, 3: way3, ?, 7: way7.
         */
        uint32_t l2_cache_way_object:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} cache_l2_cache_way_object_reg_t;

/** Type of l2_cache_addr register
 *  Cache address register
 */
typedef union {
    struct {
        /** l2_cache_addr : HRO; bitpos: [31:0]; default: 0;
         *  Those bits stores the address which will decide where inside the specified tag
         *  memory object will be accessed.
         */
        uint32_t l2_cache_addr:32;
    };
    uint32_t val;
} cache_l2_cache_addr_reg_t;

/** Type of l2_cache_debug_bus register
 *  Cache Tag/data memory content register
 */
typedef union {
    struct {
        /** l2_cache_debug_bus : R/W; bitpos: [31:0]; default: 964;
         *  This is a constant place where we can write data to or read data from the tag/data
         *  memory on the specified cache.
         */
        uint32_t l2_cache_debug_bus:32;
    };
    uint32_t val;
} cache_l2_cache_debug_bus_reg_t;


/** Group: Split L1 and L2 registers */
/** Type of level_split0 register
 *  USED TO SPLIT L1 CACHE AND L2 CACHE
 */
typedef union {
    struct {
        /** level_split0 : HRO; bitpos: [31:0]; default: 616;
         *  Reserved
         */
        uint32_t level_split0:32;
    };
    uint32_t val;
} cache_level_split0_reg_t;

/** Type of level_split1 register
 *  USED TO SPLIT L1 CACHE AND L2 CACHE
 */
typedef union {
    struct {
        /** level_split1 : HRO; bitpos: [31:0]; default: 968;
         *  Reserved
         */
        uint32_t level_split1:32;
    };
    uint32_t val;
} cache_level_split1_reg_t;


/** Group: L2 cache access attribute control register */
/** Type of l2_cache_access_attr_ctrl register
 *  L2 cache access attribute control register
 */
typedef union {
    struct {
        /** l2_cache_access_force_cc : HRO; bitpos: [0]; default: 1;
         *  Set this bit to force the request to l2 cache with cacheable attribute, otherwise,
         *  the attribute is propagated from L1 cache or CPU, it could be one of cacheable and
         *  non-cacheable.
         */
        uint32_t l2_cache_access_force_cc:1;
        /** l2_cache_access_force_wb : HRO; bitpos: [1]; default: 1;
         *  Set this bit to force the request to l2 cache with write-back attribute, otherwise,
         *  the attribute is propagated from L1 cache or CPU, it could be one of write-back and
         *  write-through.
         */
        uint32_t l2_cache_access_force_wb:1;
        /** l2_cache_access_force_wma : HRO; bitpos: [2]; default: 1;
         *  Set this bit to force the request to l2 cache with write-miss-allocate attribute,
         *  otherwise, the attribute is propagated from L1 cache or CPU, it could be one of
         *  write-miss-allocate and write-miss-no-allocate.
         */
        uint32_t l2_cache_access_force_wma:1;
        /** l2_cache_access_force_rma : HRO; bitpos: [3]; default: 1;
         *  Set this bit to force the request to l2 cache with read-miss-allocate attribute,
         *  otherwise, the attribute is propagated from L1 cache or CPU, it could be one of
         *  read-miss-allocate and read-miss-no-allocate.
         */
        uint32_t l2_cache_access_force_rma:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cache_l2_cache_access_attr_ctrl_reg_t;


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
        /** l2_cache_trace_ena : HRO; bitpos: [1]; default: 0;
         *  The bit is used to enable L2-Cache trace for the performance counter and fail tracer
         */
        uint32_t l2_cache_trace_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cache_trace_ena_reg_t;


/** Group: Redundancy  register (Prepare for ECO) */
/** Type of redundancy_sig0 register
 *  Cache redundancy signal 0 register
 */
typedef union {
    struct {
        /** redcy_sig0 : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are prepared for ECO.
         */
        uint32_t redcy_sig0:32;
    };
    uint32_t val;
} cache_redundancy_sig0_reg_t;

/** Type of redundancy_sig1 register
 *  Cache redundancy signal 1 register
 */
typedef union {
    struct {
        /** redcy_sig1 : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are prepared for ECO.
         */
        uint32_t redcy_sig1:32;
    };
    uint32_t val;
} cache_redundancy_sig1_reg_t;

/** Type of redundancy_sig2 register
 *  Cache redundancy signal 2 register
 */
typedef union {
    struct {
        /** redcy_sig2 : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are prepared for ECO.
         */
        uint32_t redcy_sig2:32;
    };
    uint32_t val;
} cache_redundancy_sig2_reg_t;

/** Type of redundancy_sig3 register
 *  Cache redundancy signal 3 register
 */
typedef union {
    struct {
        /** redcy_sig3 : R/W; bitpos: [31:0]; default: 0;
         *  Those bits are prepared for ECO.
         */
        uint32_t redcy_sig3:32;
    };
    uint32_t val;
} cache_redundancy_sig3_reg_t;

/** Type of redundancy_sig4 register
 *  Cache redundancy signal 0 register
 */
typedef union {
    struct {
        /** redcy_sig4 : RO; bitpos: [3:0]; default: 0;
         *  Those bits are prepared for ECO.
         */
        uint32_t redcy_sig4:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} cache_redundancy_sig4_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36773968;
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
    volatile cache_l1_cache_ctrl_reg_t l1_cache_ctrl;
    volatile cache_l1_bypass_cache_conf_reg_t l1_bypass_cache_conf;
    volatile cache_l1_cache_atomic_conf_reg_t l1_cache_atomic_conf;
    volatile cache_l1_icache_cachesize_conf_reg_t l1_icache_cachesize_conf;
    volatile cache_l1_icache_blocksize_conf_reg_t l1_icache_blocksize_conf;
    volatile cache_l1_cache_cachesize_conf_reg_t l1_cache_cachesize_conf;
    volatile cache_l1_cache_blocksize_conf_reg_t l1_cache_blocksize_conf;
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
    volatile cache_l1_icache2_prelock_conf_reg_t l1_icache2_prelock_conf;
    volatile cache_l1_icache2_prelock_sct0_addr_reg_t l1_icache2_prelock_sct0_addr;
    volatile cache_l1_icache2_prelock_sct1_addr_reg_t l1_icache2_prelock_sct1_addr;
    volatile cache_l1_icache2_prelock_sct_size_reg_t l1_icache2_prelock_sct_size;
    volatile cache_l1_icache3_prelock_conf_reg_t l1_icache3_prelock_conf;
    volatile cache_l1_icache3_prelock_sct0_addr_reg_t l1_icache3_prelock_sct0_addr;
    volatile cache_l1_icache3_prelock_sct1_addr_reg_t l1_icache3_prelock_sct1_addr;
    volatile cache_l1_icache3_prelock_sct_size_reg_t l1_icache3_prelock_sct_size;
    volatile cache_l1_cache_prelock_conf_reg_t l1_cache_prelock_conf;
    volatile cache_l1_cache_prelock_sct0_addr_reg_t l1_cache_prelock_sct0_addr;
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
    volatile cache_l1_icache2_preload_ctrl_reg_t l1_icache2_preload_ctrl;
    volatile cache_l1_icache2_preload_addr_reg_t l1_icache2_preload_addr;
    volatile cache_l1_icache2_preload_size_reg_t l1_icache2_preload_size;
    volatile cache_l1_icache3_preload_ctrl_reg_t l1_icache3_preload_ctrl;
    volatile cache_l1_icache3_preload_addr_reg_t l1_icache3_preload_addr;
    volatile cache_l1_icache3_preload_size_reg_t l1_icache3_preload_size;
    volatile cache_l1_cache_preload_ctrl_reg_t l1_cache_preload_ctrl;
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
    volatile cache_l1_icache2_autoload_ctrl_reg_t l1_icache2_autoload_ctrl;
    volatile cache_l1_icache2_autoload_sct0_addr_reg_t l1_icache2_autoload_sct0_addr;
    volatile cache_l1_icache2_autoload_sct0_size_reg_t l1_icache2_autoload_sct0_size;
    volatile cache_l1_icache2_autoload_sct1_addr_reg_t l1_icache2_autoload_sct1_addr;
    volatile cache_l1_icache2_autoload_sct1_size_reg_t l1_icache2_autoload_sct1_size;
    volatile cache_l1_icache3_autoload_ctrl_reg_t l1_icache3_autoload_ctrl;
    volatile cache_l1_icache3_autoload_sct0_addr_reg_t l1_icache3_autoload_sct0_addr;
    volatile cache_l1_icache3_autoload_sct0_size_reg_t l1_icache3_autoload_sct0_size;
    volatile cache_l1_icache3_autoload_sct1_addr_reg_t l1_icache3_autoload_sct1_addr;
    volatile cache_l1_icache3_autoload_sct1_size_reg_t l1_icache3_autoload_sct1_size;
    volatile cache_l1_cache_autoload_ctrl_reg_t l1_cache_autoload_ctrl;
    volatile cache_l1_cache_autoload_sct0_addr_reg_t l1_cache_autoload_sct0_addr;
    volatile cache_l1_cache_autoload_sct0_size_reg_t l1_cache_autoload_sct0_size;
    volatile cache_l1_cache_autoload_sct1_addr_reg_t l1_cache_autoload_sct1_addr;
    volatile cache_l1_cache_autoload_sct1_size_reg_t l1_cache_autoload_sct1_size;
    volatile cache_l1_cache_autoload_sct2_addr_reg_t l1_cache_autoload_sct2_addr;
    volatile cache_l1_cache_autoload_sct2_size_reg_t l1_cache_autoload_sct2_size;
    volatile cache_l1_cache_autoload_sct3_addr_reg_t l1_cache_autoload_sct3_addr;
    volatile cache_l1_cache_autoload_sct3_size_reg_t l1_cache_autoload_sct3_size;
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
    volatile cache_l1_ibus2_acs_hit_cnt_reg_t l1_ibus2_acs_hit_cnt;
    volatile cache_l1_ibus2_acs_miss_cnt_reg_t l1_ibus2_acs_miss_cnt;
    volatile cache_l1_ibus2_acs_conflict_cnt_reg_t l1_ibus2_acs_conflict_cnt;
    volatile cache_l1_ibus2_acs_nxtlvl_rd_cnt_reg_t l1_ibus2_acs_nxtlvl_rd_cnt;
    volatile cache_l1_ibus3_acs_hit_cnt_reg_t l1_ibus3_acs_hit_cnt;
    volatile cache_l1_ibus3_acs_miss_cnt_reg_t l1_ibus3_acs_miss_cnt;
    volatile cache_l1_ibus3_acs_conflict_cnt_reg_t l1_ibus3_acs_conflict_cnt;
    volatile cache_l1_ibus3_acs_nxtlvl_rd_cnt_reg_t l1_ibus3_acs_nxtlvl_rd_cnt;
    volatile cache_l1_bus0_acs_hit_cnt_reg_t l1_bus0_acs_hit_cnt;
    volatile cache_l1_bus0_acs_miss_cnt_reg_t l1_bus0_acs_miss_cnt;
    volatile cache_l1_bus0_acs_conflict_cnt_reg_t l1_bus0_acs_conflict_cnt;
    volatile cache_l1_dbus0_acs_nxtlvl_rd_cnt_reg_t l1_dbus0_acs_nxtlvl_rd_cnt;
    volatile cache_l1_dbus0_acs_nxtlvl_wr_cnt_reg_t l1_dbus0_acs_nxtlvl_wr_cnt;
    volatile cache_l1_bus1_acs_hit_cnt_reg_t l1_bus1_acs_hit_cnt;
    volatile cache_l1_bus1_acs_miss_cnt_reg_t l1_bus1_acs_miss_cnt;
    volatile cache_l1_bus1_acs_conflict_cnt_reg_t l1_bus1_acs_conflict_cnt;
    volatile cache_l1_dbus1_acs_nxtlvl_rd_cnt_reg_t l1_dbus1_acs_nxtlvl_rd_cnt;
    volatile cache_l1_dbus1_acs_nxtlvl_wr_cnt_reg_t l1_dbus1_acs_nxtlvl_wr_cnt;
    volatile cache_l1_dbus2_acs_hit_cnt_reg_t l1_dbus2_acs_hit_cnt;
    volatile cache_l1_dbus2_acs_miss_cnt_reg_t l1_dbus2_acs_miss_cnt;
    volatile cache_l1_dbus2_acs_conflict_cnt_reg_t l1_dbus2_acs_conflict_cnt;
    volatile cache_l1_dbus2_acs_nxtlvl_rd_cnt_reg_t l1_dbus2_acs_nxtlvl_rd_cnt;
    volatile cache_l1_dbus2_acs_nxtlvl_wr_cnt_reg_t l1_dbus2_acs_nxtlvl_wr_cnt;
    volatile cache_l1_dbus3_acs_hit_cnt_reg_t l1_dbus3_acs_hit_cnt;
    volatile cache_l1_dbus3_acs_miss_cnt_reg_t l1_dbus3_acs_miss_cnt;
    volatile cache_l1_dbus3_acs_conflict_cnt_reg_t l1_dbus3_acs_conflict_cnt;
    volatile cache_l1_dbus3_acs_nxtlvl_rd_cnt_reg_t l1_dbus3_acs_nxtlvl_rd_cnt;
    volatile cache_l1_dbus3_acs_nxtlvl_wr_cnt_reg_t l1_dbus3_acs_nxtlvl_wr_cnt;
    volatile cache_l1_icache0_acs_fail_id_attr_reg_t l1_icache0_acs_fail_id_attr;
    volatile cache_l1_icache0_acs_fail_addr_reg_t l1_icache0_acs_fail_addr;
    volatile cache_l1_icache1_acs_fail_id_attr_reg_t l1_icache1_acs_fail_id_attr;
    volatile cache_l1_icache1_acs_fail_addr_reg_t l1_icache1_acs_fail_addr;
    volatile cache_l1_icache2_acs_fail_id_attr_reg_t l1_icache2_acs_fail_id_attr;
    volatile cache_l1_icache2_acs_fail_addr_reg_t l1_icache2_acs_fail_addr;
    volatile cache_l1_icache3_acs_fail_id_attr_reg_t l1_icache3_acs_fail_id_attr;
    volatile cache_l1_icache3_acs_fail_addr_reg_t l1_icache3_acs_fail_addr;
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
    volatile cache_level_split0_reg_t level_split0;
    volatile cache_l2_cache_ctrl_reg_t l2_cache_ctrl;
    volatile cache_l2_bypass_cache_conf_reg_t l2_bypass_cache_conf;
    volatile cache_l2_cache_cachesize_conf_reg_t l2_cache_cachesize_conf;
    volatile cache_l2_cache_blocksize_conf_reg_t l2_cache_blocksize_conf;
    volatile cache_l2_cache_wrap_around_ctrl_reg_t l2_cache_wrap_around_ctrl;
    volatile cache_l2_cache_miss_access_ctrl_reg_t l2_cache_miss_access_ctrl;
    volatile cache_l2_cache_freeze_ctrl_reg_t l2_cache_freeze_ctrl;
    volatile cache_l2_cache_data_mem_acs_conf_reg_t l2_cache_data_mem_acs_conf;
    volatile cache_l2_cache_tag_mem_acs_conf_reg_t l2_cache_tag_mem_acs_conf;
    volatile cache_l2_cache_prelock_conf_reg_t l2_cache_prelock_conf;
    volatile cache_l2_cache_prelock_sct0_addr_reg_t l2_cache_prelock_sct0_addr;
    volatile cache_l2_cache_prelock_sct1_addr_reg_t l2_cache_prelock_sct1_addr;
    volatile cache_l2_cache_prelock_sct_size_reg_t l2_cache_prelock_sct_size;
    volatile cache_l2_cache_preload_ctrl_reg_t l2_cache_preload_ctrl;
    volatile cache_l2_cache_preload_addr_reg_t l2_cache_preload_addr;
    volatile cache_l2_cache_preload_size_reg_t l2_cache_preload_size;
    volatile cache_l2_cache_autoload_ctrl_reg_t l2_cache_autoload_ctrl;
    volatile cache_l2_cache_autoload_sct0_addr_reg_t l2_cache_autoload_sct0_addr;
    volatile cache_l2_cache_autoload_sct0_size_reg_t l2_cache_autoload_sct0_size;
    volatile cache_l2_cache_autoload_sct1_addr_reg_t l2_cache_autoload_sct1_addr;
    volatile cache_l2_cache_autoload_sct1_size_reg_t l2_cache_autoload_sct1_size;
    volatile cache_l2_cache_autoload_sct2_addr_reg_t l2_cache_autoload_sct2_addr;
    volatile cache_l2_cache_autoload_sct2_size_reg_t l2_cache_autoload_sct2_size;
    volatile cache_l2_cache_autoload_sct3_addr_reg_t l2_cache_autoload_sct3_addr;
    volatile cache_l2_cache_autoload_sct3_size_reg_t l2_cache_autoload_sct3_size;
    volatile cache_l2_cache_acs_cnt_int_ena_reg_t l2_cache_acs_cnt_int_ena;
    volatile cache_l2_cache_acs_cnt_int_clr_reg_t l2_cache_acs_cnt_int_clr;
    volatile cache_l2_cache_acs_cnt_int_raw_reg_t l2_cache_acs_cnt_int_raw;
    volatile cache_l2_cache_acs_cnt_int_st_reg_t l2_cache_acs_cnt_int_st;
    volatile cache_l2_cache_acs_fail_ctrl_reg_t l2_cache_acs_fail_ctrl;
    volatile cache_l2_cache_acs_fail_int_ena_reg_t l2_cache_acs_fail_int_ena;
    volatile cache_l2_cache_acs_fail_int_clr_reg_t l2_cache_acs_fail_int_clr;
    volatile cache_l2_cache_acs_fail_int_raw_reg_t l2_cache_acs_fail_int_raw;
    volatile cache_l2_cache_acs_fail_int_st_reg_t l2_cache_acs_fail_int_st;
    volatile cache_l2_cache_acs_cnt_ctrl_reg_t l2_cache_acs_cnt_ctrl;
    volatile cache_l2_ibus0_acs_hit_cnt_reg_t l2_ibus0_acs_hit_cnt;
    volatile cache_l2_ibus0_acs_miss_cnt_reg_t l2_ibus0_acs_miss_cnt;
    volatile cache_l2_ibus0_acs_conflict_cnt_reg_t l2_ibus0_acs_conflict_cnt;
    volatile cache_l2_ibus0_acs_nxtlvl_rd_cnt_reg_t l2_ibus0_acs_nxtlvl_rd_cnt;
    volatile cache_l2_ibus1_acs_hit_cnt_reg_t l2_ibus1_acs_hit_cnt;
    volatile cache_l2_ibus1_acs_miss_cnt_reg_t l2_ibus1_acs_miss_cnt;
    volatile cache_l2_ibus1_acs_conflict_cnt_reg_t l2_ibus1_acs_conflict_cnt;
    volatile cache_l2_ibus1_acs_nxtlvl_rd_cnt_reg_t l2_ibus1_acs_nxtlvl_rd_cnt;
    volatile cache_l2_ibus2_acs_hit_cnt_reg_t l2_ibus2_acs_hit_cnt;
    volatile cache_l2_ibus2_acs_miss_cnt_reg_t l2_ibus2_acs_miss_cnt;
    volatile cache_l2_ibus2_acs_conflict_cnt_reg_t l2_ibus2_acs_conflict_cnt;
    volatile cache_l2_ibus2_acs_nxtlvl_rd_cnt_reg_t l2_ibus2_acs_nxtlvl_rd_cnt;
    volatile cache_l2_ibus3_acs_hit_cnt_reg_t l2_ibus3_acs_hit_cnt;
    volatile cache_l2_ibus3_acs_miss_cnt_reg_t l2_ibus3_acs_miss_cnt;
    volatile cache_l2_ibus3_acs_conflict_cnt_reg_t l2_ibus3_acs_conflict_cnt;
    volatile cache_l2_ibus3_acs_nxtlvl_rd_cnt_reg_t l2_ibus3_acs_nxtlvl_rd_cnt;
    volatile cache_l2_dbus0_acs_hit_cnt_reg_t l2_dbus0_acs_hit_cnt;
    volatile cache_l2_dbus0_acs_miss_cnt_reg_t l2_dbus0_acs_miss_cnt;
    volatile cache_l2_dbus0_acs_conflict_cnt_reg_t l2_dbus0_acs_conflict_cnt;
    volatile cache_l2_dbus0_acs_nxtlvl_rd_cnt_reg_t l2_dbus0_acs_nxtlvl_rd_cnt;
    volatile cache_l2_dbus0_acs_nxtlvl_wr_cnt_reg_t l2_dbus0_acs_nxtlvl_wr_cnt;
    volatile cache_l2_dbus1_acs_hit_cnt_reg_t l2_dbus1_acs_hit_cnt;
    volatile cache_l2_dbus1_acs_miss_cnt_reg_t l2_dbus1_acs_miss_cnt;
    volatile cache_l2_dbus1_acs_conflict_cnt_reg_t l2_dbus1_acs_conflict_cnt;
    volatile cache_l2_dbus1_acs_nxtlvl_rd_cnt_reg_t l2_dbus1_acs_nxtlvl_rd_cnt;
    volatile cache_l2_dbus1_acs_nxtlvl_wr_cnt_reg_t l2_dbus1_acs_nxtlvl_wr_cnt;
    volatile cache_l2_dbus2_acs_hit_cnt_reg_t l2_dbus2_acs_hit_cnt;
    volatile cache_l2_dbus2_acs_miss_cnt_reg_t l2_dbus2_acs_miss_cnt;
    volatile cache_l2_dbus2_acs_conflict_cnt_reg_t l2_dbus2_acs_conflict_cnt;
    volatile cache_l2_dbus2_acs_nxtlvl_rd_cnt_reg_t l2_dbus2_acs_nxtlvl_rd_cnt;
    volatile cache_l2_dbus2_acs_nxtlvl_wr_cnt_reg_t l2_dbus2_acs_nxtlvl_wr_cnt;
    volatile cache_l2_dbus3_acs_hit_cnt_reg_t l2_dbus3_acs_hit_cnt;
    volatile cache_l2_dbus3_acs_miss_cnt_reg_t l2_dbus3_acs_miss_cnt;
    volatile cache_l2_dbus3_acs_conflict_cnt_reg_t l2_dbus3_acs_conflict_cnt;
    volatile cache_l2_dbus3_acs_nxtlvl_rd_cnt_reg_t l2_dbus3_acs_nxtlvl_rd_cnt;
    volatile cache_l2_dbus3_acs_nxtlvl_wr_cnt_reg_t l2_dbus3_acs_nxtlvl_wr_cnt;
    volatile cache_l2_cache_acs_fail_id_attr_reg_t l2_cache_acs_fail_id_attr;
    volatile cache_l2_cache_acs_fail_addr_reg_t l2_cache_acs_fail_addr;
    volatile cache_l2_cache_sync_preload_int_ena_reg_t l2_cache_sync_preload_int_ena;
    volatile cache_l2_cache_sync_preload_int_clr_reg_t l2_cache_sync_preload_int_clr;
    volatile cache_l2_cache_sync_preload_int_raw_reg_t l2_cache_sync_preload_int_raw;
    volatile cache_l2_cache_sync_preload_int_st_reg_t l2_cache_sync_preload_int_st;
    volatile cache_l2_cache_sync_preload_exception_reg_t l2_cache_sync_preload_exception;
    volatile cache_l2_cache_sync_rst_ctrl_reg_t l2_cache_sync_rst_ctrl;
    volatile cache_l2_cache_preload_rst_ctrl_reg_t l2_cache_preload_rst_ctrl;
    volatile cache_l2_cache_autoload_buf_clr_ctrl_reg_t l2_cache_autoload_buf_clr_ctrl;
    volatile cache_l2_unallocate_buffer_clear_reg_t l2_unallocate_buffer_clear;
    volatile cache_l2_cache_access_attr_ctrl_reg_t l2_cache_access_attr_ctrl;
    volatile cache_l2_cache_object_ctrl_reg_t l2_cache_object_ctrl;
    volatile cache_l2_cache_way_object_reg_t l2_cache_way_object;
    volatile cache_l2_cache_addr_reg_t l2_cache_addr;
    volatile cache_l2_cache_debug_bus_reg_t l2_cache_debug_bus;
    volatile cache_level_split1_reg_t level_split1;
    volatile cache_clock_gate_reg_t clock_gate;
    volatile cache_trace_ena_reg_t trace_ena;
    volatile cache_redundancy_sig0_reg_t redundancy_sig0;
    volatile cache_redundancy_sig1_reg_t redundancy_sig1;
    volatile cache_redundancy_sig2_reg_t redundancy_sig2;
    volatile cache_redundancy_sig3_reg_t redundancy_sig3;
    volatile cache_redundancy_sig4_reg_t redundancy_sig4;
    uint32_t reserved_3e8[5];
    volatile cache_date_reg_t date;
} cache_dev_t;

extern cache_dev_t CACHE;

#ifndef __cplusplus
_Static_assert(sizeof(cache_dev_t) == 0x400, "Invalid size of cache_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
