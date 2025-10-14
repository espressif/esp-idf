/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** CACHE_L1_ICACHE_CTRL_REG register
 *  L1 instruction Cache(L1-ICache) control register
 */
#define CACHE_L1_ICACHE_CTRL_REG (DR_REG_CACHE_BASE + 0x0)
/** CACHE_L1_ICACHE_SHUT_IBUS2 : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE_SHUT_IBUS2    (BIT(2))
#define CACHE_L1_ICACHE_SHUT_IBUS2_M  (CACHE_L1_ICACHE_SHUT_IBUS2_V << CACHE_L1_ICACHE_SHUT_IBUS2_S)
#define CACHE_L1_ICACHE_SHUT_IBUS2_V  0x00000001U
#define CACHE_L1_ICACHE_SHUT_IBUS2_S  2
/** CACHE_L1_ICACHE_SHUT_IBUS3 : R/W; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE_SHUT_IBUS3    (BIT(3))
#define CACHE_L1_ICACHE_SHUT_IBUS3_M  (CACHE_L1_ICACHE_SHUT_IBUS3_V << CACHE_L1_ICACHE_SHUT_IBUS3_S)
#define CACHE_L1_ICACHE_SHUT_IBUS3_V  0x00000001U
#define CACHE_L1_ICACHE_SHUT_IBUS3_S  3

/** CACHE_L1_CACHE_CTRL_REG register
 *  L1 data Cache(L1-Cache) control register
 */
#define CACHE_L1_CACHE_CTRL_REG (DR_REG_CACHE_BASE + 0x4)
/** CACHE_L1_CACHE_SHUT_BUS0 : R/W; bitpos: [0]; default: 0;
 *  The bit is used to disable core0 bus0 access L1-Cache, 0: enable, 1: disable
 */
#define CACHE_L1_CACHE_SHUT_BUS0    (BIT(0))
#define CACHE_L1_CACHE_SHUT_BUS0_M  (CACHE_L1_CACHE_SHUT_BUS0_V << CACHE_L1_CACHE_SHUT_BUS0_S)
#define CACHE_L1_CACHE_SHUT_BUS0_V  0x00000001U
#define CACHE_L1_CACHE_SHUT_BUS0_S  0
/** CACHE_L1_CACHE_SHUT_BUS1 : R/W; bitpos: [1]; default: 0;
 *  The bit is used to disable core0 bus1 access L1-Cache, 0: enable, 1: disable
 */
#define CACHE_L1_CACHE_SHUT_BUS1    (BIT(1))
#define CACHE_L1_CACHE_SHUT_BUS1_M  (CACHE_L1_CACHE_SHUT_BUS1_V << CACHE_L1_CACHE_SHUT_BUS1_S)
#define CACHE_L1_CACHE_SHUT_BUS1_V  0x00000001U
#define CACHE_L1_CACHE_SHUT_BUS1_S  1

/** CACHE_L1_ICACHE_CACHESIZE_CONF_REG register
 *  L1 instruction Cache CacheSize mode configure register
 */
#define CACHE_L1_ICACHE_CACHESIZE_CONF_REG (DR_REG_CACHE_BASE + 0x10)
/** CACHE_L1_ICACHE_CACHESIZE_4K : HRO; bitpos: [4]; default: 1;
 *  The field is used to configure cachesize of L1-ICache as 4k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_4K    (BIT(4))
#define CACHE_L1_ICACHE_CACHESIZE_4K_M  (CACHE_L1_ICACHE_CACHESIZE_4K_V << CACHE_L1_ICACHE_CACHESIZE_4K_S)
#define CACHE_L1_ICACHE_CACHESIZE_4K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_4K_S  4

/** CACHE_L1_ICACHE_BLOCKSIZE_CONF_REG register
 *  L1 instruction Cache BlockSize mode configure register
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_CONF_REG (DR_REG_CACHE_BASE + 0x14)
/** CACHE_L1_ICACHE_BLOCKSIZE_64 : HRO; bitpos: [3]; default: 1;
 *  The field is used to configureblocksize of L1-ICache as 64 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_64    (BIT(3))
#define CACHE_L1_ICACHE_BLOCKSIZE_64_M  (CACHE_L1_ICACHE_BLOCKSIZE_64_V << CACHE_L1_ICACHE_BLOCKSIZE_64_S)
#define CACHE_L1_ICACHE_BLOCKSIZE_64_V  0x00000001U
#define CACHE_L1_ICACHE_BLOCKSIZE_64_S  3

/** CACHE_L1_CACHE_CACHESIZE_CONF_REG register
 *  L1 data Cache CacheSize mode configure register
 */
#define CACHE_L1_CACHE_CACHESIZE_CONF_REG (DR_REG_CACHE_BASE + 0x18)
/** CACHE_L1_DCACHE_CACHESIZE_32K : HRO; bitpos: [7]; default: 1;
 *  The field is used to configure cachesize of L1-DCache as 32k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_32K    (BIT(7))
#define CACHE_L1_DCACHE_CACHESIZE_32K_M  (CACHE_L1_DCACHE_CACHESIZE_32K_V << CACHE_L1_DCACHE_CACHESIZE_32K_S)
#define CACHE_L1_DCACHE_CACHESIZE_32K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_32K_S  7

/** CACHE_L1_CACHE_BLOCKSIZE_CONF_REG register
 *  L1 data Cache BlockSize mode configure register
 */
#define CACHE_L1_CACHE_BLOCKSIZE_CONF_REG (DR_REG_CACHE_BASE + 0x1c)
/** CACHE_L1_CACHE_BLOCKSIZE_32 : HRO; bitpos: [2]; default: 1;
 *  The field is used to configureblocksize of L1-DCache as 32 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_CACHE_BLOCKSIZE_32    (BIT(2))
#define CACHE_L1_CACHE_BLOCKSIZE_32_M  (CACHE_L1_CACHE_BLOCKSIZE_32_V << CACHE_L1_CACHE_BLOCKSIZE_32_S)
#define CACHE_L1_CACHE_BLOCKSIZE_32_V  0x00000001U
#define CACHE_L1_CACHE_BLOCKSIZE_32_S  2

/** CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG register
 *  Cache wrap around control register
 */
#define CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG (DR_REG_CACHE_BASE + 0x20)
/** CACHE_L1_ICACHE2_WRAP : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_WRAP    (BIT(2))
#define CACHE_L1_ICACHE2_WRAP_M  (CACHE_L1_ICACHE2_WRAP_V << CACHE_L1_ICACHE2_WRAP_S)
#define CACHE_L1_ICACHE2_WRAP_V  0x00000001U
#define CACHE_L1_ICACHE2_WRAP_S  2
/** CACHE_L1_CACHE_WRAP : R/W; bitpos: [4]; default: 0;
 *  Set this bit as 1 to enable L1-DCache wrap around mode.
 */
#define CACHE_L1_CACHE_WRAP    (BIT(4))
#define CACHE_L1_CACHE_WRAP_M  (CACHE_L1_CACHE_WRAP_V << CACHE_L1_CACHE_WRAP_S)
#define CACHE_L1_CACHE_WRAP_V  0x00000001U
#define CACHE_L1_CACHE_WRAP_S  4

/** CACHE_L1_CACHE_MISS_ACCESS_CTRL_REG register
 *  Cache wrap around control register
 */
#define CACHE_L1_CACHE_MISS_ACCESS_CTRL_REG (DR_REG_CACHE_BASE + 0x24)
/** CACHE_L1_ICACHE2_MISS_DISABLE_ACCESS : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_MISS_DISABLE_ACCESS    (BIT(2))
#define CACHE_L1_ICACHE2_MISS_DISABLE_ACCESS_M  (CACHE_L1_ICACHE2_MISS_DISABLE_ACCESS_V << CACHE_L1_ICACHE2_MISS_DISABLE_ACCESS_S)
#define CACHE_L1_ICACHE2_MISS_DISABLE_ACCESS_V  0x00000001U
#define CACHE_L1_ICACHE2_MISS_DISABLE_ACCESS_S  2
/** CACHE_L1_CACHE_MISS_DISABLE_ACCESS : R/W; bitpos: [4]; default: 0;
 *  Set this bit as 1 to disable early restart of L1-DCache
 */
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS    (BIT(4))
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS_M  (CACHE_L1_CACHE_MISS_DISABLE_ACCESS_V << CACHE_L1_CACHE_MISS_DISABLE_ACCESS_S)
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS_V  0x00000001U
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS_S  4

/** CACHE_L1_CACHE_FREEZE_CTRL_REG register
 *  Cache Freeze control register
 */
#define CACHE_L1_CACHE_FREEZE_CTRL_REG (DR_REG_CACHE_BASE + 0x28)
/** CACHE_L1_ICACHE2_FREEZE_EN : R/W; bitpos: [8]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FREEZE_EN    (BIT(8))
#define CACHE_L1_ICACHE2_FREEZE_EN_M  (CACHE_L1_ICACHE2_FREEZE_EN_V << CACHE_L1_ICACHE2_FREEZE_EN_S)
#define CACHE_L1_ICACHE2_FREEZE_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_FREEZE_EN_S  8
/** CACHE_L1_ICACHE2_FREEZE_MODE : R/W; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FREEZE_MODE    (BIT(9))
#define CACHE_L1_ICACHE2_FREEZE_MODE_M  (CACHE_L1_ICACHE2_FREEZE_MODE_V << CACHE_L1_ICACHE2_FREEZE_MODE_S)
#define CACHE_L1_ICACHE2_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_ICACHE2_FREEZE_MODE_S  9
/** CACHE_L1_CACHE_FREEZE_EN : R/W; bitpos: [16]; default: 0;
 *  The bit is used to enable freeze operation on L1-Cache. It can be cleared by
 *  software.
 */
#define CACHE_L1_CACHE_FREEZE_EN    (BIT(16))
#define CACHE_L1_CACHE_FREEZE_EN_M  (CACHE_L1_CACHE_FREEZE_EN_V << CACHE_L1_CACHE_FREEZE_EN_S)
#define CACHE_L1_CACHE_FREEZE_EN_V  0x00000001U
#define CACHE_L1_CACHE_FREEZE_EN_S  16
/** CACHE_L1_CACHE_FREEZE_MODE : R/W; bitpos: [17]; default: 0;
 *  The bit is used to configure mode of freeze operation L1-Cache. 0: a miss-access
 *  will not stuck. 1: a miss-access will stuck.
 */
#define CACHE_L1_CACHE_FREEZE_MODE    (BIT(17))
#define CACHE_L1_CACHE_FREEZE_MODE_M  (CACHE_L1_CACHE_FREEZE_MODE_V << CACHE_L1_CACHE_FREEZE_MODE_S)
#define CACHE_L1_CACHE_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_CACHE_FREEZE_MODE_S  17
/** CACHE_L1_CACHE_FREEZE_DONE : RO; bitpos: [18]; default: 0;
 *  The bit is used to indicate whether freeze operation on L1-Cache is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_CACHE_FREEZE_DONE    (BIT(18))
#define CACHE_L1_CACHE_FREEZE_DONE_M  (CACHE_L1_CACHE_FREEZE_DONE_V << CACHE_L1_CACHE_FREEZE_DONE_S)
#define CACHE_L1_CACHE_FREEZE_DONE_V  0x00000001U
#define CACHE_L1_CACHE_FREEZE_DONE_S  18

/** CACHE_L1_CACHE_DATA_MEM_ACS_CONF_REG register
 *  Cache data memory access configure register
 */
#define CACHE_L1_CACHE_DATA_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x2c)
/** CACHE_L1_ICACHE2_DATA_MEM_RD_EN : R/W; bitpos: [8]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN    (BIT(8))
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN_M  (CACHE_L1_ICACHE2_DATA_MEM_RD_EN_V << CACHE_L1_ICACHE2_DATA_MEM_RD_EN_S)
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN_S  8
/** CACHE_L1_ICACHE2_DATA_MEM_WR_EN : R/W; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN    (BIT(9))
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN_M  (CACHE_L1_ICACHE2_DATA_MEM_WR_EN_V << CACHE_L1_ICACHE2_DATA_MEM_WR_EN_S)
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN_S  9
/** CACHE_L1_CACHE_DATA_MEM_RD_EN : R/W; bitpos: [16]; default: 0;
 *  The bit is used to enable config-bus read L1-Cache data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_CACHE_DATA_MEM_RD_EN    (BIT(16))
#define CACHE_L1_CACHE_DATA_MEM_RD_EN_M  (CACHE_L1_CACHE_DATA_MEM_RD_EN_V << CACHE_L1_CACHE_DATA_MEM_RD_EN_S)
#define CACHE_L1_CACHE_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_CACHE_DATA_MEM_RD_EN_S  16
/** CACHE_L1_CACHE_DATA_MEM_WR_EN : R/W; bitpos: [17]; default: 0;
 *  The bit is used to enable config-bus write L1-Cache data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_CACHE_DATA_MEM_WR_EN    (BIT(17))
#define CACHE_L1_CACHE_DATA_MEM_WR_EN_M  (CACHE_L1_CACHE_DATA_MEM_WR_EN_V << CACHE_L1_CACHE_DATA_MEM_WR_EN_S)
#define CACHE_L1_CACHE_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_CACHE_DATA_MEM_WR_EN_S  17

/** CACHE_L1_CACHE_TAG_MEM_ACS_CONF_REG register
 *  Cache tag memory access configure register
 */
#define CACHE_L1_CACHE_TAG_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x30)
/** CACHE_L1_ICACHE2_TAG_MEM_RD_EN : R/W; bitpos: [8]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN    (BIT(8))
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN_M  (CACHE_L1_ICACHE2_TAG_MEM_RD_EN_V << CACHE_L1_ICACHE2_TAG_MEM_RD_EN_S)
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN_S  8
/** CACHE_L1_ICACHE2_TAG_MEM_WR_EN : R/W; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN    (BIT(9))
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN_M  (CACHE_L1_ICACHE2_TAG_MEM_WR_EN_V << CACHE_L1_ICACHE2_TAG_MEM_WR_EN_S)
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN_S  9
/** CACHE_L1_CACHE_TAG_MEM_RD_EN : R/W; bitpos: [16]; default: 0;
 *  The bit is used to enable config-bus read L1-Cache tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_CACHE_TAG_MEM_RD_EN    (BIT(16))
#define CACHE_L1_CACHE_TAG_MEM_RD_EN_M  (CACHE_L1_CACHE_TAG_MEM_RD_EN_V << CACHE_L1_CACHE_TAG_MEM_RD_EN_S)
#define CACHE_L1_CACHE_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_CACHE_TAG_MEM_RD_EN_S  16
/** CACHE_L1_CACHE_TAG_MEM_WR_EN : R/W; bitpos: [17]; default: 0;
 *  The bit is used to enable config-bus write L1-Cache tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_CACHE_TAG_MEM_WR_EN    (BIT(17))
#define CACHE_L1_CACHE_TAG_MEM_WR_EN_M  (CACHE_L1_CACHE_TAG_MEM_WR_EN_V << CACHE_L1_CACHE_TAG_MEM_WR_EN_S)
#define CACHE_L1_CACHE_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_CACHE_TAG_MEM_WR_EN_S  17

/** CACHE_L1_ICACHE2_PRELOCK_CONF_REG register
 *  L1 instruction Cache 2 prelock configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x54)
/** CACHE_L1_ICACHE2_PRELOCK_SCT0_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_M  (CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_V << CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_ICACHE2_PRELOCK_SCT1_EN : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_M  (CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_V << CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_S  1
/** CACHE_L1_ICACHE2_PRELOCK_RGID : R/W; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l1 icache2 prelock.
 */
#define CACHE_L1_ICACHE2_PRELOCK_RGID    0x0000000FU
#define CACHE_L1_ICACHE2_PRELOCK_RGID_M  (CACHE_L1_ICACHE2_PRELOCK_RGID_V << CACHE_L1_ICACHE2_PRELOCK_RGID_S)
#define CACHE_L1_ICACHE2_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE2_PRELOCK_RGID_S  2

/** CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_REG register
 *  L1 instruction Cache 2 prelock section0 address configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x58)
/** CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the first section of prelock
 *  on L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_M  (CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_V << CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_REG register
 *  L1 instruction Cache 2 prelock section1 address configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x5c)
/** CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the second section of prelock
 *  on L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_M  (CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_V << CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE2_PRELOCK_SCT_SIZE_REG register
 *  L1 instruction Cache 2 prelock section size configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x60)
/** CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE : R/W; bitpos: [13:0]; default: 16383;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_M  (CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_V << CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE : R/W; bitpos: [29:16]; default: 16383;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_M  (CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_V << CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_S  16

/** CACHE_L1_CACHE_PRELOCK_CONF_REG register
 *  L1 Cache prelock configure register
 */
#define CACHE_L1_CACHE_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x74)
/** CACHE_L1_CACHE_PRELOCK_SCT0_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L1-Cache.
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN_M  (CACHE_L1_CACHE_PRELOCK_SCT0_EN_V << CACHE_L1_CACHE_PRELOCK_SCT0_EN_S)
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_CACHE_PRELOCK_SCT1_EN : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L1-Cache.
 */
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN_M  (CACHE_L1_CACHE_PRELOCK_SCT1_EN_V << CACHE_L1_CACHE_PRELOCK_SCT1_EN_S)
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN_S  1
/** CACHE_L1_CACHE_PRELOCK_RGID : HRO; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l1 cache prelock.
 */
#define CACHE_L1_CACHE_PRELOCK_RGID    0x0000000FU
#define CACHE_L1_CACHE_PRELOCK_RGID_M  (CACHE_L1_CACHE_PRELOCK_RGID_V << CACHE_L1_CACHE_PRELOCK_RGID_S)
#define CACHE_L1_CACHE_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L1_CACHE_PRELOCK_RGID_S  2

/** CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_REG register
 *  L1 Cache prelock section0 address configure register
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x78)
/** CACHE_L1_CACHE_PRELOCK_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the first section of prelock
 *  on L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_M  (CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_V << CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_REG register
 *  L1 Cache prelock section1 address configure register
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x7c)
/** CACHE_L1_CACHE_PRELOCK_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the second section of prelock
 *  on L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_M  (CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_V << CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_DCACHE_PRELOCK_SCT_SIZE_REG register
 *  L1  Cache prelock section size configure register
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x80)
/** CACHE_L1_CACHE_PRELOCK_SCT0_SIZE : R/W; bitpos: [13:0]; default: 16383;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_M  (CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_V << CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_CACHE_PRELOCK_SCT1_SIZE : R/W; bitpos: [29:16]; default: 16383;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L1-Cache, which should be used together with L1_CACHE_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_M  (CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_V << CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_S  16

/** CACHE_LOCK_CTRL_REG register
 *  Lock-class (manual lock) operation control register
 */
#define CACHE_LOCK_CTRL_REG (DR_REG_CACHE_BASE + 0x84)
/** CACHE_LOCK_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable lock operation. It will be cleared by hardware after lock
 *  operation done
 */
#define CACHE_LOCK_ENA    (BIT(0))
#define CACHE_LOCK_ENA_M  (CACHE_LOCK_ENA_V << CACHE_LOCK_ENA_S)
#define CACHE_LOCK_ENA_V  0x00000001U
#define CACHE_LOCK_ENA_S  0
/** CACHE_UNLOCK_ENA : R/W/SC; bitpos: [1]; default: 0;
 *  The bit is used to enable unlock operation. It will be cleared by hardware after
 *  unlock operation done
 */
#define CACHE_UNLOCK_ENA    (BIT(1))
#define CACHE_UNLOCK_ENA_M  (CACHE_UNLOCK_ENA_V << CACHE_UNLOCK_ENA_S)
#define CACHE_UNLOCK_ENA_V  0x00000001U
#define CACHE_UNLOCK_ENA_S  1
/** CACHE_LOCK_DONE : RO; bitpos: [2]; default: 1;
 *  The bit is used to indicate whether unlock/lock operation is finished or not. 0:
 *  not finished. 1: finished.
 */
#define CACHE_LOCK_DONE    (BIT(2))
#define CACHE_LOCK_DONE_M  (CACHE_LOCK_DONE_V << CACHE_LOCK_DONE_S)
#define CACHE_LOCK_DONE_V  0x00000001U
#define CACHE_LOCK_DONE_S  2
/** CACHE_LOCK_RGID : HRO; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of cache lock/unlock.
 */
#define CACHE_LOCK_RGID    0x0000000FU
#define CACHE_LOCK_RGID_M  (CACHE_LOCK_RGID_V << CACHE_LOCK_RGID_S)
#define CACHE_LOCK_RGID_V  0x0000000FU
#define CACHE_LOCK_RGID_S  3

/** CACHE_LOCK_MAP_REG register
 *  Lock (manual lock) map configure register
 */
#define CACHE_LOCK_MAP_REG (DR_REG_CACHE_BASE + 0x88)
/** CACHE_LOCK_MAP : R/W; bitpos: [5:0]; default: 0;
 *  Those bits are used to indicate which caches in the two-level cache structure will
 *  apply this lock/unlock operation. [4]: L1-Cache
 */
#define CACHE_LOCK_MAP    0x0000003FU
#define CACHE_LOCK_MAP_M  (CACHE_LOCK_MAP_V << CACHE_LOCK_MAP_S)
#define CACHE_LOCK_MAP_V  0x0000003FU
#define CACHE_LOCK_MAP_S  0

/** CACHE_LOCK_ADDR_REG register
 *  Lock (manual lock) address configure register
 */
#define CACHE_LOCK_ADDR_REG (DR_REG_CACHE_BASE + 0x8c)
/** CACHE_LOCK_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the lock/unlock operation,
 *  which should be used together with CACHE_LOCK_SIZE_REG
 */
#define CACHE_LOCK_ADDR    0xFFFFFFFFU
#define CACHE_LOCK_ADDR_M  (CACHE_LOCK_ADDR_V << CACHE_LOCK_ADDR_S)
#define CACHE_LOCK_ADDR_V  0xFFFFFFFFU
#define CACHE_LOCK_ADDR_S  0

/** CACHE_LOCK_SIZE_REG register
 *  Lock (manual lock) size configure register
 */
#define CACHE_LOCK_SIZE_REG (DR_REG_CACHE_BASE + 0x90)
/** CACHE_LOCK_SIZE : R/W; bitpos: [15:0]; default: 0;
 *  Those bits are used to configure the size of the lock/unlock operation, which
 *  should be used together with CACHE_LOCK_ADDR_REG
 */
#define CACHE_LOCK_SIZE    0x0000FFFFU
#define CACHE_LOCK_SIZE_M  (CACHE_LOCK_SIZE_V << CACHE_LOCK_SIZE_S)
#define CACHE_LOCK_SIZE_V  0x0000FFFFU
#define CACHE_LOCK_SIZE_S  0

/** CACHE_SYNC_CTRL_REG register
 *  Sync-class operation control register
 */
#define CACHE_SYNC_CTRL_REG (DR_REG_CACHE_BASE + 0x94)
/** CACHE_INVALIDATE_ENA : R/W/SC; bitpos: [0]; default: 1;
 *  The bit is used to enable invalidate operation. It will be cleared by hardware
 *  after invalidate operation done. Note that this bit and the other sync-bits
 *  (clean_ena, writeback_ena, writeback_invalidate_ena) are mutually exclusive, that
 *  is, those bits can not be set to 1 at the same time.
 */
#define CACHE_INVALIDATE_ENA    (BIT(0))
#define CACHE_INVALIDATE_ENA_M  (CACHE_INVALIDATE_ENA_V << CACHE_INVALIDATE_ENA_S)
#define CACHE_INVALIDATE_ENA_V  0x00000001U
#define CACHE_INVALIDATE_ENA_S  0
/** CACHE_CLEAN_ENA : R/W/SC; bitpos: [1]; default: 0;
 *  The bit is used to enable clean operation. It will be cleared by hardware after
 *  clean operation done. Note that this bit and the other sync-bits (invalidate_ena,
 *  writeback_ena, writeback_invalidate_ena) are mutually exclusive, that is, those
 *  bits can not be set to 1 at the same time.
 */
#define CACHE_CLEAN_ENA    (BIT(1))
#define CACHE_CLEAN_ENA_M  (CACHE_CLEAN_ENA_V << CACHE_CLEAN_ENA_S)
#define CACHE_CLEAN_ENA_V  0x00000001U
#define CACHE_CLEAN_ENA_S  1
/** CACHE_WRITEBACK_ENA : R/W/SC; bitpos: [2]; default: 0;
 *  The bit is used to enable writeback operation. It will be cleared by hardware after
 *  writeback operation done. Note that this bit and the other sync-bits
 *  (invalidate_ena, clean_ena, writeback_invalidate_ena) are mutually exclusive, that
 *  is, those bits can not be set to 1 at the same time.
 */
#define CACHE_WRITEBACK_ENA    (BIT(2))
#define CACHE_WRITEBACK_ENA_M  (CACHE_WRITEBACK_ENA_V << CACHE_WRITEBACK_ENA_S)
#define CACHE_WRITEBACK_ENA_V  0x00000001U
#define CACHE_WRITEBACK_ENA_S  2
/** CACHE_WRITEBACK_INVALIDATE_ENA : R/W/SC; bitpos: [3]; default: 0;
 *  The bit is used to enable writeback-invalidate operation. It will be cleared by
 *  hardware after writeback-invalidate operation done. Note that this bit and the
 *  other sync-bits (invalidate_ena, clean_ena, writeback_ena) are mutually exclusive,
 *  that is, those bits can not be set to 1 at the same time.
 */
#define CACHE_WRITEBACK_INVALIDATE_ENA    (BIT(3))
#define CACHE_WRITEBACK_INVALIDATE_ENA_M  (CACHE_WRITEBACK_INVALIDATE_ENA_V << CACHE_WRITEBACK_INVALIDATE_ENA_S)
#define CACHE_WRITEBACK_INVALIDATE_ENA_V  0x00000001U
#define CACHE_WRITEBACK_INVALIDATE_ENA_S  3
/** CACHE_SYNC_DONE : RO; bitpos: [4]; default: 0;
 *  The bit is used to indicate whether sync operation (invalidate, clean, writeback,
 *  writeback_invalidate) is finished or not. 0: not finished. 1: finished.
 */
#define CACHE_SYNC_DONE    (BIT(4))
#define CACHE_SYNC_DONE_M  (CACHE_SYNC_DONE_V << CACHE_SYNC_DONE_S)
#define CACHE_SYNC_DONE_V  0x00000001U
#define CACHE_SYNC_DONE_S  4
/** CACHE_SYNC_RGID : HRO; bitpos: [8:5]; default: 0;
 *  The bit is used to set  the gid of cache sync operation (invalidate, clean,
 *  writeback, writeback_invalidate)
 */
#define CACHE_SYNC_RGID    0x0000000FU
#define CACHE_SYNC_RGID_M  (CACHE_SYNC_RGID_V << CACHE_SYNC_RGID_S)
#define CACHE_SYNC_RGID_V  0x0000000FU
#define CACHE_SYNC_RGID_S  5

/** CACHE_SYNC_MAP_REG register
 *  Sync map configure register
 */
#define CACHE_SYNC_MAP_REG (DR_REG_CACHE_BASE + 0x98)
/** CACHE_SYNC_MAP : R/W; bitpos: [5:0]; default: 63;
 *  Those bits are used to indicate which caches in the two-level cache structure will
 *  apply the sync operation.  [4]: L1-Cache
 */
#define CACHE_SYNC_MAP    0x0000003FU
#define CACHE_SYNC_MAP_M  (CACHE_SYNC_MAP_V << CACHE_SYNC_MAP_S)
#define CACHE_SYNC_MAP_V  0x0000003FU
#define CACHE_SYNC_MAP_S  0

/** CACHE_SYNC_ADDR_REG register
 *  Sync address configure register
 */
#define CACHE_SYNC_ADDR_REG (DR_REG_CACHE_BASE + 0x9c)
/** CACHE_SYNC_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the sync operation, which
 *  should be used together with CACHE_SYNC_SIZE_REG
 */
#define CACHE_SYNC_ADDR    0xFFFFFFFFU
#define CACHE_SYNC_ADDR_M  (CACHE_SYNC_ADDR_V << CACHE_SYNC_ADDR_S)
#define CACHE_SYNC_ADDR_V  0xFFFFFFFFU
#define CACHE_SYNC_ADDR_S  0

/** CACHE_SYNC_SIZE_REG register
 *  Sync size configure register
 */
#define CACHE_SYNC_SIZE_REG (DR_REG_CACHE_BASE + 0xa0)
/** CACHE_SYNC_SIZE : R/W; bitpos: [24:0]; default: 0;
 *  Those bits are used to configure the size of the sync operation, which should be
 *  used together with CACHE_SYNC_ADDR_REG
 */
#define CACHE_SYNC_SIZE    0x01FFFFFFU
#define CACHE_SYNC_SIZE_M  (CACHE_SYNC_SIZE_V << CACHE_SYNC_SIZE_S)
#define CACHE_SYNC_SIZE_V  0x01FFFFFFU
#define CACHE_SYNC_SIZE_S  0

/** CACHE_L1_ICACHE2_PRELOAD_CTRL_REG register
 *  L1 instruction Cache 2 preload-operation control register
 */
#define CACHE_L1_ICACHE2_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xbc)
/** CACHE_L1_ICACHE2_PRELOAD_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable preload operation on L1-ICache2. It will be cleared by
 *  hardware automatically after preload operation is done.
 */
#define CACHE_L1_ICACHE2_PRELOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE2_PRELOAD_ENA_M  (CACHE_L1_ICACHE2_PRELOAD_ENA_V << CACHE_L1_ICACHE2_PRELOAD_ENA_S)
#define CACHE_L1_ICACHE2_PRELOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOAD_ENA_S  0
/** CACHE_L1_ICACHE2_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether preload operation is finished or not. 0: not
 *  finished. 1: finished.
 */
#define CACHE_L1_ICACHE2_PRELOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE2_PRELOAD_DONE_M  (CACHE_L1_ICACHE2_PRELOAD_DONE_V << CACHE_L1_ICACHE2_PRELOAD_DONE_S)
#define CACHE_L1_ICACHE2_PRELOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOAD_DONE_S  1
/** CACHE_L1_ICACHE2_PRELOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L1_ICACHE2_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE2_PRELOAD_ORDER_M  (CACHE_L1_ICACHE2_PRELOAD_ORDER_V << CACHE_L1_ICACHE2_PRELOAD_ORDER_S)
#define CACHE_L1_ICACHE2_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOAD_ORDER_S  2
/** CACHE_L1_ICACHE2_PRELOAD_RGID : R/W; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l1 icache2 preload.
 */
#define CACHE_L1_ICACHE2_PRELOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE2_PRELOAD_RGID_M  (CACHE_L1_ICACHE2_PRELOAD_RGID_V << CACHE_L1_ICACHE2_PRELOAD_RGID_S)
#define CACHE_L1_ICACHE2_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE2_PRELOAD_RGID_S  3

/** CACHE_L1_ICACHE2_PRELOAD_ADDR_REG register
 *  L1 instruction Cache 2 preload address configure register
 */
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xc0)
/** CACHE_L1_ICACHE2_PRELOAD_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of preload on L1-ICache2, which
 *  should be used together with L1_ICACHE2_PRELOAD_SIZE_REG
 */
#define CACHE_L1_ICACHE2_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_M  (CACHE_L1_ICACHE2_PRELOAD_ADDR_V << CACHE_L1_ICACHE2_PRELOAD_ADDR_S)
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_S  0

/** CACHE_L1_ICACHE2_PRELOAD_SIZE_REG register
 *  L1 instruction Cache 2 preload size configure register
 */
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xc4)
/** CACHE_L1_ICACHE2_PRELOAD_SIZE : R/W; bitpos: [13:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOAD_ADDR_REG
 */
#define CACHE_L1_ICACHE2_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_M  (CACHE_L1_ICACHE2_PRELOAD_SIZE_V << CACHE_L1_ICACHE2_PRELOAD_SIZE_S)
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_S  0

/** CACHE_L1_CACHE_PRELOAD_CTRL_REG register
 *  L1 Cache  preload-operation control register
 */
#define CACHE_L1_CACHE_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xd4)
/** CACHE_L1_CACHE_PRELOAD_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable preload operation on L1-Cache. It will be cleared by
 *  hardware automatically after preload operation is done.
 */
#define CACHE_L1_CACHE_PRELOAD_ENA    (BIT(0))
#define CACHE_L1_CACHE_PRELOAD_ENA_M  (CACHE_L1_CACHE_PRELOAD_ENA_V << CACHE_L1_CACHE_PRELOAD_ENA_S)
#define CACHE_L1_CACHE_PRELOAD_ENA_V  0x00000001U
#define CACHE_L1_CACHE_PRELOAD_ENA_S  0
/** CACHE_L1_CACHE_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether preload operation is finished or not. 0: not
 *  finished. 1: finished.
 */
#define CACHE_L1_CACHE_PRELOAD_DONE    (BIT(1))
#define CACHE_L1_CACHE_PRELOAD_DONE_M  (CACHE_L1_CACHE_PRELOAD_DONE_V << CACHE_L1_CACHE_PRELOAD_DONE_S)
#define CACHE_L1_CACHE_PRELOAD_DONE_V  0x00000001U
#define CACHE_L1_CACHE_PRELOAD_DONE_S  1
/** CACHE_L1_CACHE_PRELOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L1_CACHE_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_CACHE_PRELOAD_ORDER_M  (CACHE_L1_CACHE_PRELOAD_ORDER_V << CACHE_L1_CACHE_PRELOAD_ORDER_S)
#define CACHE_L1_CACHE_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_CACHE_PRELOAD_ORDER_S  2
/** CACHE_L1_CACHE_PRELOAD_RGID : HRO; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l1 cache preload.
 */
#define CACHE_L1_CACHE_PRELOAD_RGID    0x0000000FU
#define CACHE_L1_CACHE_PRELOAD_RGID_M  (CACHE_L1_CACHE_PRELOAD_RGID_V << CACHE_L1_CACHE_PRELOAD_RGID_S)
#define CACHE_L1_CACHE_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L1_CACHE_PRELOAD_RGID_S  3

/** CACHE_L1_DCACHE_PRELOAD_ADDR_REG register
 *  L1 Cache  preload address configure register
 */
#define CACHE_L1_DCACHE_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xd8)
/** CACHE_L1_CACHE_PRELOAD_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of preload on L1-Cache, which
 *  should be used together with L1_CACHE_PRELOAD_SIZE_REG
 */
#define CACHE_L1_CACHE_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOAD_ADDR_M  (CACHE_L1_CACHE_PRELOAD_ADDR_V << CACHE_L1_CACHE_PRELOAD_ADDR_S)
#define CACHE_L1_CACHE_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOAD_ADDR_S  0

/** CACHE_L1_DCACHE_PRELOAD_SIZE_REG register
 *  L1 Cache  preload size configure register
 */
#define CACHE_L1_DCACHE_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xdc)
/** CACHE_L1_CACHE_PRELOAD_SIZE : R/W; bitpos: [13:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-Cache, which should be used together with L1_CACHE_PRELOAD_ADDR_REG
 */
#define CACHE_L1_CACHE_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_CACHE_PRELOAD_SIZE_M  (CACHE_L1_CACHE_PRELOAD_SIZE_V << CACHE_L1_CACHE_PRELOAD_SIZE_S)
#define CACHE_L1_CACHE_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_CACHE_PRELOAD_SIZE_S  0

/** CACHE_L1_ICACHE2_AUTOLOAD_CTRL_REG register
 *  L1 instruction Cache 2 autoload-operation control register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x108)
/** CACHE_L1_ICACHE2_AUTOLOAD_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable and disable autoload operation on L1-ICache2.  1: enable,
 *  0: disable.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE2_AUTOLOAD_ENA_M  (CACHE_L1_ICACHE2_AUTOLOAD_ENA_V << CACHE_L1_ICACHE2_AUTOLOAD_ENA_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_ENA_S  0
/** CACHE_L1_ICACHE2_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether autoload operation on L1-ICache2 is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE2_AUTOLOAD_DONE_M  (CACHE_L1_ICACHE2_AUTOLOAD_DONE_V << CACHE_L1_ICACHE2_AUTOLOAD_DONE_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_DONE_S  1
/** CACHE_L1_ICACHE2_AUTOLOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L1-ICache2. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER_M  (CACHE_L1_ICACHE2_AUTOLOAD_ORDER_V << CACHE_L1_ICACHE2_AUTOLOAD_ORDER_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER_S  2
/** CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE : R/W; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L1-ICache2.
 *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_S  9
/** CACHE_L1_ICACHE2_AUTOLOAD_RGID : R/W; bitpos: [13:10]; default: 0;
 *  The bit is used to set  the gid of l1 icache2 autoload.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID_M  (CACHE_L1_ICACHE2_AUTOLOAD_RGID_V << CACHE_L1_ICACHE2_AUTOLOAD_RGID_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID_S  10

/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 instruction Cache 2 autoload section 0 address configure register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x10c)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the first section for
 *  autoload operation on L1-ICache2. Note that it should be used together with
 *  L1_ICACHE2_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_REG register
 *  L1 instruction Cache 2 autoload section 0 size configure register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x110)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the first section for autoload
 *  operation on L1-ICache2. Note that it should be used together with
 *  L1_ICACHE2_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_REG register
 *  L1 instruction Cache 2 autoload section 1 address configure register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x114)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the second section for
 *  autoload operation on L1-ICache2. Note that it should be used together with
 *  L1_ICACHE2_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 instruction Cache 2 autoload section 1 size configure register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x118)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the second section for autoload
 *  operation on L1-ICache2. Note that it should be used together with
 *  L1_ICACHE2_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_CACHE_AUTOLOAD_CTRL_REG register
 *  L1 Cache autoload-operation control register
 */
#define CACHE_L1_CACHE_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x130)
/** CACHE_L1_CACHE_AUTOLOAD_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable and disable autoload operation on L1-Cache.  1: enable,
 *  0: disable.
 */
#define CACHE_L1_CACHE_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L1_CACHE_AUTOLOAD_ENA_M  (CACHE_L1_CACHE_AUTOLOAD_ENA_V << CACHE_L1_CACHE_AUTOLOAD_ENA_S)
#define CACHE_L1_CACHE_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_ENA_S  0
/** CACHE_L1_CACHE_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether autoload operation on L1-Cache is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_CACHE_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L1_CACHE_AUTOLOAD_DONE_M  (CACHE_L1_CACHE_AUTOLOAD_DONE_V << CACHE_L1_CACHE_AUTOLOAD_DONE_S)
#define CACHE_L1_CACHE_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_DONE_S  1
/** CACHE_L1_CACHE_AUTOLOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L1-Cache. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L1_CACHE_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_CACHE_AUTOLOAD_ORDER_M  (CACHE_L1_CACHE_AUTOLOAD_ORDER_V << CACHE_L1_CACHE_AUTOLOAD_ORDER_S)
#define CACHE_L1_CACHE_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_ORDER_S  2
/** CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE : R/W; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L1-Cache. 0/3:
 *  miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L1-Cache.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_V << CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L1-Cache.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_V << CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_S  9

/** CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 Cache autoload section 0 address configure register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x134)
/** CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the first section for
 *  autoload operation on L1-Cache. Note that it should be used together with
 *  L1_CACHE_AUTOLOAD_SCT0_SIZE and L1_CACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_M  (CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_V << CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_REG register
 *  L1 Cache autoload section 0 size configure register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x138)
/** CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE : R/W; bitpos: [24:0]; default: 0;
 *  Those bits are used to configure the size of the first section for autoload
 *  operation on L1-Cache. Note that it should be used together with
 *  L1_CACHE_AUTOLOAD_SCT0_ADDR and L1_CACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE    0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_M  (CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_V << CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_V  0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_REG register
 *  L1 Cache autoload section 1 address configure register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x13c)
/** CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start address of the second section for
 *  autoload operation on L1-Cache. Note that it should be used together with
 *  L1_CACHE_AUTOLOAD_SCT1_SIZE and L1_CACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 Cache autoload section 1 size configure register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x140)
/** CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE : R/W; bitpos: [24:0]; default: 0;
 *  Those bits are used to configure the size of the second section for autoload
 *  operation on L1-Cache. Note that it should be used together with
 *  L1_CACHE_AUTOLOAD_SCT1_ADDR and L1_CACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE    0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_V  0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_CACHE_ACS_CNT_INT_ENA_REG register
 *  Cache Access Counter Interrupt enable register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_ENA_REG (DR_REG_CACHE_BASE + 0x154)
/** CACHE_L1_IBUS2_OVF_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_OVF_INT_ENA    (BIT(2))
#define CACHE_L1_IBUS2_OVF_INT_ENA_M  (CACHE_L1_IBUS2_OVF_INT_ENA_V << CACHE_L1_IBUS2_OVF_INT_ENA_S)
#define CACHE_L1_IBUS2_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_IBUS2_OVF_INT_ENA_S  2
/** CACHE_L1_IBUS3_OVF_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_OVF_INT_ENA    (BIT(3))
#define CACHE_L1_IBUS3_OVF_INT_ENA_M  (CACHE_L1_IBUS3_OVF_INT_ENA_V << CACHE_L1_IBUS3_OVF_INT_ENA_S)
#define CACHE_L1_IBUS3_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_IBUS3_OVF_INT_ENA_S  3
/** CACHE_L1_BUS0_OVF_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L1-DCache due to bus0 accesses L1-DCache.
 */
#define CACHE_L1_BUS0_OVF_INT_ENA    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_ENA_M  (CACHE_L1_BUS0_OVF_INT_ENA_V << CACHE_L1_BUS0_OVF_INT_ENA_S)
#define CACHE_L1_BUS0_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_ENA_S  4
/** CACHE_L1_BUS1_OVF_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L1-DCache due to bus1 accesses L1-DCache.
 */
#define CACHE_L1_BUS1_OVF_INT_ENA    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_ENA_M  (CACHE_L1_BUS1_OVF_INT_ENA_V << CACHE_L1_BUS1_OVF_INT_ENA_S)
#define CACHE_L1_BUS1_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_ENA_S  5

/** CACHE_L1_CACHE_ACS_CNT_INT_CLR_REG register
 *  Cache Access Counter Interrupt clear register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_CLR_REG (DR_REG_CACHE_BASE + 0x158)
/** CACHE_L1_IBUS2_OVF_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_OVF_INT_CLR    (BIT(2))
#define CACHE_L1_IBUS2_OVF_INT_CLR_M  (CACHE_L1_IBUS2_OVF_INT_CLR_V << CACHE_L1_IBUS2_OVF_INT_CLR_S)
#define CACHE_L1_IBUS2_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_IBUS2_OVF_INT_CLR_S  2
/** CACHE_L1_IBUS3_OVF_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_OVF_INT_CLR    (BIT(3))
#define CACHE_L1_IBUS3_OVF_INT_CLR_M  (CACHE_L1_IBUS3_OVF_INT_CLR_V << CACHE_L1_IBUS3_OVF_INT_CLR_S)
#define CACHE_L1_IBUS3_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_IBUS3_OVF_INT_CLR_S  3
/** CACHE_L1_BUS0_OVF_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
 *  to bus0 accesses L1-DCache.
 */
#define CACHE_L1_BUS0_OVF_INT_CLR    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_CLR_M  (CACHE_L1_BUS0_OVF_INT_CLR_V << CACHE_L1_BUS0_OVF_INT_CLR_S)
#define CACHE_L1_BUS0_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_CLR_S  4
/** CACHE_L1_BUS1_OVF_INT_CLR : WT; bitpos: [5]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
 *  to bus1 accesses L1-DCache.
 */
#define CACHE_L1_BUS1_OVF_INT_CLR    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_CLR_M  (CACHE_L1_BUS1_OVF_INT_CLR_V << CACHE_L1_BUS1_OVF_INT_CLR_S)
#define CACHE_L1_BUS1_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_CLR_S  5

/** CACHE_L1_CACHE_ACS_CNT_INT_RAW_REG register
 *  Cache Access Counter Interrupt raw register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_RAW_REG (DR_REG_CACHE_BASE + 0x15c)
/** CACHE_L1_IBUS2_OVF_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache2
 *  due to bus2 accesses L1-ICache2.
 */
#define CACHE_L1_IBUS2_OVF_INT_RAW    (BIT(2))
#define CACHE_L1_IBUS2_OVF_INT_RAW_M  (CACHE_L1_IBUS2_OVF_INT_RAW_V << CACHE_L1_IBUS2_OVF_INT_RAW_S)
#define CACHE_L1_IBUS2_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_IBUS2_OVF_INT_RAW_S  2
/** CACHE_L1_IBUS3_OVF_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache3
 *  due to bus3 accesses L1-ICache3.
 */
#define CACHE_L1_IBUS3_OVF_INT_RAW    (BIT(3))
#define CACHE_L1_IBUS3_OVF_INT_RAW_M  (CACHE_L1_IBUS3_OVF_INT_RAW_V << CACHE_L1_IBUS3_OVF_INT_RAW_S)
#define CACHE_L1_IBUS3_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_IBUS3_OVF_INT_RAW_S  3
/** CACHE_L1_BUS0_OVF_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
 *  due to bus0 accesses L1-DCache.
 */
#define CACHE_L1_BUS0_OVF_INT_RAW    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_RAW_M  (CACHE_L1_BUS0_OVF_INT_RAW_V << CACHE_L1_BUS0_OVF_INT_RAW_S)
#define CACHE_L1_BUS0_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_RAW_S  4
/** CACHE_L1_BUS1_OVF_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
 *  due to bus1 accesses L1-DCache.
 */
#define CACHE_L1_BUS1_OVF_INT_RAW    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_RAW_M  (CACHE_L1_BUS1_OVF_INT_RAW_V << CACHE_L1_BUS1_OVF_INT_RAW_S)
#define CACHE_L1_BUS1_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_RAW_S  5

/** CACHE_L1_CACHE_ACS_CNT_INT_ST_REG register
 *  Cache Access Counter Interrupt status register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_ST_REG (DR_REG_CACHE_BASE + 0x160)
/** CACHE_L1_IBUS2_OVF_INT_ST : RO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_OVF_INT_ST    (BIT(2))
#define CACHE_L1_IBUS2_OVF_INT_ST_M  (CACHE_L1_IBUS2_OVF_INT_ST_V << CACHE_L1_IBUS2_OVF_INT_ST_S)
#define CACHE_L1_IBUS2_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_IBUS2_OVF_INT_ST_S  2
/** CACHE_L1_IBUS3_OVF_INT_ST : RO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_OVF_INT_ST    (BIT(3))
#define CACHE_L1_IBUS3_OVF_INT_ST_M  (CACHE_L1_IBUS3_OVF_INT_ST_V << CACHE_L1_IBUS3_OVF_INT_ST_S)
#define CACHE_L1_IBUS3_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_IBUS3_OVF_INT_ST_S  3
/** CACHE_L1_BUS0_OVF_INT_ST : RO; bitpos: [4]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L1-DCache due to bus0 accesses L1-DCache.
 */
#define CACHE_L1_BUS0_OVF_INT_ST    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_ST_M  (CACHE_L1_BUS0_OVF_INT_ST_V << CACHE_L1_BUS0_OVF_INT_ST_S)
#define CACHE_L1_BUS0_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_ST_S  4
/** CACHE_L1_BUS1_OVF_INT_ST : RO; bitpos: [5]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L1-DCache due to bus1 accesses L1-DCache.
 */
#define CACHE_L1_BUS1_OVF_INT_ST    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_ST_M  (CACHE_L1_BUS1_OVF_INT_ST_V << CACHE_L1_BUS1_OVF_INT_ST_S)
#define CACHE_L1_BUS1_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_ST_S  5

/** CACHE_L1_CACHE_ACS_FAIL_CTRL_REG register
 *  Cache Access Fail Configuration register
 */
#define CACHE_L1_CACHE_ACS_FAIL_CTRL_REG (DR_REG_CACHE_BASE + 0x164)
/** CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure l1 icache2 access fail check mode. 0: the access fail
 *  is not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE    (BIT(2))
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_V << CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_S  2
/** CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE : R/W; bitpos: [4]; default: 0;
 *  The bit is used to configure l1 cache access fail check mode. 0: the access fail is
 *  not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE    (BIT(4))
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_V << CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_ENA_REG register
 *  Cache Access Fail Interrupt enable register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_ENA_REG (DR_REG_CACHE_BASE + 0x168)
/** CACHE_L1_ICACHE2_FAIL_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FAIL_INT_ENA    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_ENA_M  (CACHE_L1_ICACHE2_FAIL_INT_ENA_V << CACHE_L1_ICACHE2_FAIL_INT_ENA_S)
#define CACHE_L1_ICACHE2_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_ENA_S  2
/** CACHE_L1_CACHE_FAIL_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable interrupt of access fail that occurs in L1-DCache due to
 *  cpu accesses L1-DCache.
 */
#define CACHE_L1_CACHE_FAIL_INT_ENA    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_ENA_M  (CACHE_L1_CACHE_FAIL_INT_ENA_V << CACHE_L1_CACHE_FAIL_INT_ENA_S)
#define CACHE_L1_CACHE_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_ENA_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_CLR_REG register
 *  L1-Cache Access Fail Interrupt clear register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_CLR_REG (DR_REG_CACHE_BASE + 0x16c)
/** CACHE_L1_ICACHE2_FAIL_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FAIL_INT_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_CLR_M  (CACHE_L1_ICACHE2_FAIL_INT_CLR_V << CACHE_L1_ICACHE2_FAIL_INT_CLR_S)
#define CACHE_L1_ICACHE2_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_CLR_S  2
/** CACHE_L1_CACHE_FAIL_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The bit is used to clear interrupt of access fail that occurs in L1-DCache due to
 *  cpu accesses L1-DCache.
 */
#define CACHE_L1_CACHE_FAIL_INT_CLR    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_CLR_M  (CACHE_L1_CACHE_FAIL_INT_CLR_V << CACHE_L1_CACHE_FAIL_INT_CLR_S)
#define CACHE_L1_CACHE_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_CLR_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_RAW_REG register
 *  Cache Access Fail Interrupt raw register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_RAW_REG (DR_REG_CACHE_BASE + 0x170)
/** CACHE_L1_ICACHE2_FAIL_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L1-ICache2.
 */
#define CACHE_L1_ICACHE2_FAIL_INT_RAW    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_RAW_M  (CACHE_L1_ICACHE2_FAIL_INT_RAW_V << CACHE_L1_ICACHE2_FAIL_INT_RAW_S)
#define CACHE_L1_ICACHE2_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_RAW_S  2
/** CACHE_L1_CACHE_FAIL_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L1-DCache.
 */
#define CACHE_L1_CACHE_FAIL_INT_RAW    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_RAW_M  (CACHE_L1_CACHE_FAIL_INT_RAW_V << CACHE_L1_CACHE_FAIL_INT_RAW_S)
#define CACHE_L1_CACHE_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_RAW_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_ST_REG register
 *  Cache Access Fail Interrupt status register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_ST_REG (DR_REG_CACHE_BASE + 0x174)
/** CACHE_L1_ICACHE2_FAIL_INT_ST : RO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FAIL_INT_ST    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_ST_M  (CACHE_L1_ICACHE2_FAIL_INT_ST_V << CACHE_L1_ICACHE2_FAIL_INT_ST_S)
#define CACHE_L1_ICACHE2_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_ST_S  2
/** CACHE_L1_CACHE_FAIL_INT_ST : RO; bitpos: [4]; default: 0;
 *  The bit indicates the interrupt status of access fail that occurs in L1-DCache due
 *  to cpu accesses L1-DCache.
 */
#define CACHE_L1_CACHE_FAIL_INT_ST    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_ST_M  (CACHE_L1_CACHE_FAIL_INT_ST_V << CACHE_L1_CACHE_FAIL_INT_ST_S)
#define CACHE_L1_CACHE_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_ST_S  4

/** CACHE_L1_CACHE_ACS_CNT_CTRL_REG register
 *  Cache Access Counter enable and clear register
 */
#define CACHE_L1_CACHE_ACS_CNT_CTRL_REG (DR_REG_CACHE_BASE + 0x178)
/** CACHE_L1_IBUS2_CNT_ENA : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_CNT_ENA    (BIT(2))
#define CACHE_L1_IBUS2_CNT_ENA_M  (CACHE_L1_IBUS2_CNT_ENA_V << CACHE_L1_IBUS2_CNT_ENA_S)
#define CACHE_L1_IBUS2_CNT_ENA_V  0x00000001U
#define CACHE_L1_IBUS2_CNT_ENA_S  2
/** CACHE_L1_IBUS3_CNT_ENA : R/W; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_CNT_ENA    (BIT(3))
#define CACHE_L1_IBUS3_CNT_ENA_M  (CACHE_L1_IBUS3_CNT_ENA_V << CACHE_L1_IBUS3_CNT_ENA_S)
#define CACHE_L1_IBUS3_CNT_ENA_V  0x00000001U
#define CACHE_L1_IBUS3_CNT_ENA_S  3
/** CACHE_L1_BUS0_CNT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable dbus0 counter in L1-DCache.
 */
#define CACHE_L1_BUS0_CNT_ENA    (BIT(4))
#define CACHE_L1_BUS0_CNT_ENA_M  (CACHE_L1_BUS0_CNT_ENA_V << CACHE_L1_BUS0_CNT_ENA_S)
#define CACHE_L1_BUS0_CNT_ENA_V  0x00000001U
#define CACHE_L1_BUS0_CNT_ENA_S  4
/** CACHE_L1_BUS1_CNT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable dbus1 counter in L1-DCache.
 */
#define CACHE_L1_BUS1_CNT_ENA    (BIT(5))
#define CACHE_L1_BUS1_CNT_ENA_M  (CACHE_L1_BUS1_CNT_ENA_V << CACHE_L1_BUS1_CNT_ENA_S)
#define CACHE_L1_BUS1_CNT_ENA_V  0x00000001U
#define CACHE_L1_BUS1_CNT_ENA_S  5
/** CACHE_L1_IBUS2_CNT_CLR : WT; bitpos: [18]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_CNT_CLR    (BIT(18))
#define CACHE_L1_IBUS2_CNT_CLR_M  (CACHE_L1_IBUS2_CNT_CLR_V << CACHE_L1_IBUS2_CNT_CLR_S)
#define CACHE_L1_IBUS2_CNT_CLR_V  0x00000001U
#define CACHE_L1_IBUS2_CNT_CLR_S  18
/** CACHE_L1_IBUS3_CNT_CLR : WT; bitpos: [19]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_CNT_CLR    (BIT(19))
#define CACHE_L1_IBUS3_CNT_CLR_M  (CACHE_L1_IBUS3_CNT_CLR_V << CACHE_L1_IBUS3_CNT_CLR_S)
#define CACHE_L1_IBUS3_CNT_CLR_V  0x00000001U
#define CACHE_L1_IBUS3_CNT_CLR_S  19
/** CACHE_L1_BUS0_CNT_CLR : WT; bitpos: [20]; default: 0;
 *  The bit is used to clear dbus0 counter in L1-DCache.
 */
#define CACHE_L1_BUS0_CNT_CLR    (BIT(20))
#define CACHE_L1_BUS0_CNT_CLR_M  (CACHE_L1_BUS0_CNT_CLR_V << CACHE_L1_BUS0_CNT_CLR_S)
#define CACHE_L1_BUS0_CNT_CLR_V  0x00000001U
#define CACHE_L1_BUS0_CNT_CLR_S  20
/** CACHE_L1_BUS1_CNT_CLR : WT; bitpos: [21]; default: 0;
 *  The bit is used to clear dbus1 counter in L1-DCache.
 */
#define CACHE_L1_BUS1_CNT_CLR    (BIT(21))
#define CACHE_L1_BUS1_CNT_CLR_M  (CACHE_L1_BUS1_CNT_CLR_V << CACHE_L1_BUS1_CNT_CLR_S)
#define CACHE_L1_BUS1_CNT_CLR_V  0x00000001U
#define CACHE_L1_BUS1_CNT_CLR_S  21

/** CACHE_L1_IBUS2_ACS_HIT_CNT_REG register
 *  L1-ICache bus2 Hit-Access Counter register
 */
#define CACHE_L1_IBUS2_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x19c)
/** CACHE_L1_IBUS2_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus2 accesses L1-ICache2.
 */
#define CACHE_L1_IBUS2_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS2_HIT_CNT_M  (CACHE_L1_IBUS2_HIT_CNT_V << CACHE_L1_IBUS2_HIT_CNT_S)
#define CACHE_L1_IBUS2_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS2_HIT_CNT_S  0

/** CACHE_L1_IBUS2_ACS_MISS_CNT_REG register
 *  L1-ICache bus2 Miss-Access Counter register
 */
#define CACHE_L1_IBUS2_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1a0)
/** CACHE_L1_IBUS2_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus2 accesses L1-ICache2.
 */
#define CACHE_L1_IBUS2_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS2_MISS_CNT_M  (CACHE_L1_IBUS2_MISS_CNT_V << CACHE_L1_IBUS2_MISS_CNT_S)
#define CACHE_L1_IBUS2_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS2_MISS_CNT_S  0

/** CACHE_L1_IBUS2_ACS_CONFLICT_CNT_REG register
 *  L1-ICache bus2 Conflict-Access Counter register
 */
#define CACHE_L1_IBUS2_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1a4)
/** CACHE_L1_IBUS2_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus2 accesses L1-ICache2.
 */
#define CACHE_L1_IBUS2_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS2_CONFLICT_CNT_M  (CACHE_L1_IBUS2_CONFLICT_CNT_V << CACHE_L1_IBUS2_CONFLICT_CNT_S)
#define CACHE_L1_IBUS2_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS2_CONFLICT_CNT_S  0

/** CACHE_L1_IBUS2_ACS_NXTLVL_RD_CNT_REG register
 *  L1-ICache bus2 Next-Level-Access Counter register
 */
#define CACHE_L1_IBUS2_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1a8)
/** CACHE_L1_IBUS2_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-ICache accesses L2-Cache due to
 *  bus2 accessing L1-ICache2.
 */
#define CACHE_L1_IBUS2_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS2_NXTLVL_RD_CNT_M  (CACHE_L1_IBUS2_NXTLVL_RD_CNT_V << CACHE_L1_IBUS2_NXTLVL_RD_CNT_S)
#define CACHE_L1_IBUS2_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS2_NXTLVL_RD_CNT_S  0

/** CACHE_L1_IBUS3_ACS_HIT_CNT_REG register
 *  L1-ICache bus3 Hit-Access Counter register
 */
#define CACHE_L1_IBUS3_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1ac)
/** CACHE_L1_IBUS3_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus3 accesses L1-ICache3.
 */
#define CACHE_L1_IBUS3_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS3_HIT_CNT_M  (CACHE_L1_IBUS3_HIT_CNT_V << CACHE_L1_IBUS3_HIT_CNT_S)
#define CACHE_L1_IBUS3_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS3_HIT_CNT_S  0

/** CACHE_L1_IBUS3_ACS_MISS_CNT_REG register
 *  L1-ICache bus3 Miss-Access Counter register
 */
#define CACHE_L1_IBUS3_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1b0)
/** CACHE_L1_IBUS3_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus3 accesses L1-ICache3.
 */
#define CACHE_L1_IBUS3_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS3_MISS_CNT_M  (CACHE_L1_IBUS3_MISS_CNT_V << CACHE_L1_IBUS3_MISS_CNT_S)
#define CACHE_L1_IBUS3_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS3_MISS_CNT_S  0

/** CACHE_L1_IBUS3_ACS_CONFLICT_CNT_REG register
 *  L1-ICache bus3 Conflict-Access Counter register
 */
#define CACHE_L1_IBUS3_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1b4)
/** CACHE_L1_IBUS3_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus3 accesses L1-ICache3.
 */
#define CACHE_L1_IBUS3_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS3_CONFLICT_CNT_M  (CACHE_L1_IBUS3_CONFLICT_CNT_V << CACHE_L1_IBUS3_CONFLICT_CNT_S)
#define CACHE_L1_IBUS3_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS3_CONFLICT_CNT_S  0

/** CACHE_L1_IBUS3_ACS_NXTLVL_RD_CNT_REG register
 *  L1-ICache bus3 Next-Level-Access Counter register
 */
#define CACHE_L1_IBUS3_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1b8)
/** CACHE_L1_IBUS3_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-ICache accesses L2-Cache due to
 *  bus3 accessing L1-ICache3.
 */
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT_M  (CACHE_L1_IBUS3_NXTLVL_RD_CNT_V << CACHE_L1_IBUS3_NXTLVL_RD_CNT_S)
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT_S  0

/** CACHE_L1_BUS0_ACS_HIT_CNT_REG register
 *  L1-Cache bus0 Hit-Access Counter register
 */
#define CACHE_L1_BUS0_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1bc)
/** CACHE_L1_BUS0_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus0 accesses L1-Cache.
 */
#define CACHE_L1_BUS0_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_HIT_CNT_M  (CACHE_L1_BUS0_HIT_CNT_V << CACHE_L1_BUS0_HIT_CNT_S)
#define CACHE_L1_BUS0_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_HIT_CNT_S  0

/** CACHE_L1_BUS0_ACS_MISS_CNT_REG register
 *  L1-Cache bus0 Miss-Access Counter register
 */
#define CACHE_L1_BUS0_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1c0)
/** CACHE_L1_BUS0_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus0 accesses L1-Cache.
 */
#define CACHE_L1_BUS0_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_MISS_CNT_M  (CACHE_L1_BUS0_MISS_CNT_V << CACHE_L1_BUS0_MISS_CNT_S)
#define CACHE_L1_BUS0_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_MISS_CNT_S  0

/** CACHE_L1_BUS0_ACS_CONFLICT_CNT_REG register
 *  L1-Cache bus0 Conflict-Access Counter register
 */
#define CACHE_L1_BUS0_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1c4)
/** CACHE_L1_BUS0_CONFLICT_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus0 accesses L1-Cache.
 */
#define CACHE_L1_BUS0_CONFLICT_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_CONFLICT_RD_CNT_M  (CACHE_L1_BUS0_CONFLICT_RD_CNT_V << CACHE_L1_BUS0_CONFLICT_RD_CNT_S)
#define CACHE_L1_BUS0_CONFLICT_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_CONFLICT_RD_CNT_S  0

/** CACHE_L1_DBUS0_ACS_NXTLVL_RD_CNT_REG register
 *  L1-Cache bus0 Next-Level-Access Counter register
 */
#define CACHE_L1_DBUS0_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1c8)
/** CACHE_L1_BUS0_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-Cache accesses L2-Cache due to
 *  bus0 accessing L1-Cache.
 */
#define CACHE_L1_BUS0_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_RD_CNT_M  (CACHE_L1_BUS0_NXTLVL_RD_CNT_V << CACHE_L1_BUS0_NXTLVL_RD_CNT_S)
#define CACHE_L1_BUS0_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS0_ACS_NXTLVL_WR_CNT_REG register
 *  L1-DCache bus0 WB-Access Counter register
 */
#define CACHE_L1_DBUS0_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x1cc)
/** CACHE_L1_BUS0_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when bus0 accesses L1-Cache.
 */
#define CACHE_L1_BUS0_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_WR_CNT_M  (CACHE_L1_BUS0_NXTLVL_WR_CNT_V << CACHE_L1_BUS0_NXTLVL_WR_CNT_S)
#define CACHE_L1_BUS0_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_WR_CNT_S  0

/** CACHE_L1_BUS1_ACS_HIT_CNT_REG register
 *  L1-Cache bus1 Hit-Access Counter register
 */
#define CACHE_L1_BUS1_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1d0)
/** CACHE_L1_BUS1_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus1 accesses L1-Cache.
 */
#define CACHE_L1_BUS1_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_HIT_CNT_M  (CACHE_L1_BUS1_HIT_CNT_V << CACHE_L1_BUS1_HIT_CNT_S)
#define CACHE_L1_BUS1_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_HIT_CNT_S  0

/** CACHE_L1_BUS1_ACS_MISS_CNT_REG register
 *  L1-Cache bus1 Miss-Access Counter register
 */
#define CACHE_L1_BUS1_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1d4)
/** CACHE_L1_BUS1_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus1 accesses L1-Cache.
 */
#define CACHE_L1_BUS1_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_MISS_CNT_M  (CACHE_L1_BUS1_MISS_CNT_V << CACHE_L1_BUS1_MISS_CNT_S)
#define CACHE_L1_BUS1_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_MISS_CNT_S  0

/** CACHE_L1_BUS1_ACS_CONFLICT_CNT_REG register
 *  L1-Cache bus1 Conflict-Access Counter register
 */
#define CACHE_L1_BUS1_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1d8)
/** CACHE_L1_BUS1_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus1 accesses L1-Cache.
 */
#define CACHE_L1_BUS1_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_CONFLICT_CNT_M  (CACHE_L1_BUS1_CONFLICT_CNT_V << CACHE_L1_BUS1_CONFLICT_CNT_S)
#define CACHE_L1_BUS1_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_CONFLICT_CNT_S  0

/** CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG register
 *  L1-DCache bus1 Next-Level-Access Counter register
 */
#define CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1dc)
/** CACHE_L1_DBUS1_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-Cache accesses L2-Cache due to
 *  bus1 accessing L1-Cache.
 */
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT_M  (CACHE_L1_DBUS1_NXTLVL_RD_CNT_V << CACHE_L1_DBUS1_NXTLVL_RD_CNT_S)
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG register
 *  L1-DCache bus1 WB-Access Counter register
 */
#define CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x1e0)
/** CACHE_L1_DBUS1_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when bus1 accesses L1-Cache.
 */
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT_M  (CACHE_L1_DBUS1_NXTLVL_WR_CNT_V << CACHE_L1_DBUS1_NXTLVL_WR_CNT_S)
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT_S  0

/** CACHE_L1_ICACHE2_ACS_FAIL_ID_ATTR_REG register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
#define CACHE_L1_ICACHE2_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x21c)
/** CACHE_L1_ICACHE2_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  The register records the ID of fail-access when cache2 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE2_FAIL_ID    0x0000FFFFU
#define CACHE_L1_ICACHE2_FAIL_ID_M  (CACHE_L1_ICACHE2_FAIL_ID_V << CACHE_L1_ICACHE2_FAIL_ID_S)
#define CACHE_L1_ICACHE2_FAIL_ID_V  0x0000FFFFU
#define CACHE_L1_ICACHE2_FAIL_ID_S  0
/** CACHE_L1_ICACHE2_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  The register records the attribution of fail-access when cache2 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE2_FAIL_ATTR    0x0000FFFFU
#define CACHE_L1_ICACHE2_FAIL_ATTR_M  (CACHE_L1_ICACHE2_FAIL_ATTR_V << CACHE_L1_ICACHE2_FAIL_ATTR_S)
#define CACHE_L1_ICACHE2_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L1_ICACHE2_FAIL_ATTR_S  16

/** CACHE_L1_ICACHE2_ACS_FAIL_ADDR_REG register
 *  L1-ICache0 Access Fail Address information register
 */
#define CACHE_L1_ICACHE2_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x220)
/** CACHE_L1_ICACHE2_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when cache2 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE2_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_FAIL_ADDR_M  (CACHE_L1_ICACHE2_FAIL_ADDR_V << CACHE_L1_ICACHE2_FAIL_ADDR_S)
#define CACHE_L1_ICACHE2_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_FAIL_ADDR_S  0

/** CACHE_L1_DCACHE_ACS_FAIL_ID_ATTR_REG register
 *  L1-Cache Access Fail ID/attribution information register
 */
#define CACHE_L1_DCACHE_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x22c)
/** CACHE_L1_CACHE_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  The register records the ID of fail-access when cache accesses L1-Cache.
 */
#define CACHE_L1_CACHE_FAIL_ID    0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ID_M  (CACHE_L1_CACHE_FAIL_ID_V << CACHE_L1_CACHE_FAIL_ID_S)
#define CACHE_L1_CACHE_FAIL_ID_V  0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ID_S  0
/** CACHE_L1_CACHE_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  The register records the attribution of fail-access when cache accesses L1-Cache.
 */
#define CACHE_L1_CACHE_FAIL_ATTR    0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ATTR_M  (CACHE_L1_CACHE_FAIL_ATTR_V << CACHE_L1_CACHE_FAIL_ATTR_S)
#define CACHE_L1_CACHE_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ATTR_S  16

/** CACHE_L1_DCACHE_ACS_FAIL_ADDR_REG register
 *  L1-Cache Access Fail Address information register
 */
#define CACHE_L1_DCACHE_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x230)
/** CACHE_L1_CACHE_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when cache accesses L1-Cache.
 */
#define CACHE_L1_CACHE_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_FAIL_ADDR_M  (CACHE_L1_CACHE_FAIL_ADDR_V << CACHE_L1_CACHE_FAIL_ADDR_S)
#define CACHE_L1_CACHE_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_FAIL_ADDR_S  0

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_ENA_REG register
 *  L1-Cache Access Fail Interrupt enable register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_ENA_REG (DR_REG_CACHE_BASE + 0x234)
/** CACHE_L1_ICACHE2_PLD_DONE_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_V << CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_S  2
/** CACHE_L1_CACHE_PLD_DONE_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable interrupt of L1-Cache preload-operation. If preload
 *  operation is done, interrupt occurs.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA_M  (CACHE_L1_CACHE_PLD_DONE_INT_ENA_V << CACHE_L1_CACHE_PLD_DONE_INT_ENA_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA_S  4
/** CACHE_SYNC_DONE_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The bit is used to enable interrupt of Cache sync-operation done.
 */
#define CACHE_SYNC_DONE_INT_ENA    (BIT(6))
#define CACHE_SYNC_DONE_INT_ENA_M  (CACHE_SYNC_DONE_INT_ENA_V << CACHE_SYNC_DONE_INT_ENA_S)
#define CACHE_SYNC_DONE_INT_ENA_V  0x00000001U
#define CACHE_SYNC_DONE_INT_ENA_S  6
/** CACHE_L1_ICACHE2_PLD_ERR_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_V << CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_S  9
/** CACHE_L1_CACHE_PLD_ERR_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The bit is used to enable interrupt of L1-Cache preload-operation error.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA_M  (CACHE_L1_CACHE_PLD_ERR_INT_ENA_V << CACHE_L1_CACHE_PLD_ERR_INT_ENA_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA_S  11
/** CACHE_SYNC_ERR_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The bit is used to enable interrupt of Cache sync-operation error.
 */
#define CACHE_SYNC_ERR_INT_ENA    (BIT(13))
#define CACHE_SYNC_ERR_INT_ENA_M  (CACHE_SYNC_ERR_INT_ENA_V << CACHE_SYNC_ERR_INT_ENA_S)
#define CACHE_SYNC_ERR_INT_ENA_V  0x00000001U
#define CACHE_SYNC_ERR_INT_ENA_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_CLR_REG register
 *  Sync Preload operation Interrupt clear register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_CLR_REG (DR_REG_CACHE_BASE + 0x238)
/** CACHE_L1_ICACHE2_PLD_DONE_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_V << CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_S  2
/** CACHE_L1_CACHE_PLD_DONE_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The bit is used to clear interrupt that occurs only when L1-Cache preload-operation
 *  is done.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR_M  (CACHE_L1_CACHE_PLD_DONE_INT_CLR_V << CACHE_L1_CACHE_PLD_DONE_INT_CLR_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR_S  4
/** CACHE_SYNC_DONE_INT_CLR : WT; bitpos: [6]; default: 0;
 *  The bit is used to clear interrupt that occurs only when Cache sync-operation is
 *  done.
 */
#define CACHE_SYNC_DONE_INT_CLR    (BIT(6))
#define CACHE_SYNC_DONE_INT_CLR_M  (CACHE_SYNC_DONE_INT_CLR_V << CACHE_SYNC_DONE_INT_CLR_S)
#define CACHE_SYNC_DONE_INT_CLR_V  0x00000001U
#define CACHE_SYNC_DONE_INT_CLR_S  6
/** CACHE_L1_ICACHE2_PLD_ERR_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_V << CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_S  9
/** CACHE_L1_CACHE_PLD_ERR_INT_CLR : WT; bitpos: [11]; default: 0;
 *  The bit is used to clear interrupt of L1-Cache preload-operation error.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR_M  (CACHE_L1_CACHE_PLD_ERR_INT_CLR_V << CACHE_L1_CACHE_PLD_ERR_INT_CLR_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR_S  11
/** CACHE_SYNC_ERR_INT_CLR : WT; bitpos: [13]; default: 0;
 *  The bit is used to clear interrupt of Cache sync-operation error.
 */
#define CACHE_SYNC_ERR_INT_CLR    (BIT(13))
#define CACHE_SYNC_ERR_INT_CLR_M  (CACHE_SYNC_ERR_INT_CLR_V << CACHE_SYNC_ERR_INT_CLR_S)
#define CACHE_SYNC_ERR_INT_CLR_V  0x00000001U
#define CACHE_SYNC_ERR_INT_CLR_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_RAW_REG register
 *  Sync Preload operation Interrupt raw register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_RAW_REG (DR_REG_CACHE_BASE + 0x23c)
/** CACHE_L1_ICACHE2_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_V << CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_S  2
/** CACHE_L1_CACHE_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-Cache preload-operation is
 *  done.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW_M  (CACHE_L1_CACHE_PLD_DONE_INT_RAW_V << CACHE_L1_CACHE_PLD_DONE_INT_RAW_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW_S  4
/** CACHE_SYNC_DONE_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw bit of the interrupt that occurs only when Cache sync-operation is done.
 */
#define CACHE_SYNC_DONE_INT_RAW    (BIT(6))
#define CACHE_SYNC_DONE_INT_RAW_M  (CACHE_SYNC_DONE_INT_RAW_V << CACHE_SYNC_DONE_INT_RAW_S)
#define CACHE_SYNC_DONE_INT_RAW_V  0x00000001U
#define CACHE_SYNC_DONE_INT_RAW_S  6
/** CACHE_L1_ICACHE2_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_V << CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_S  9
/** CACHE_L1_CACHE_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-Cache preload-operation error
 *  occurs.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW_M  (CACHE_L1_CACHE_PLD_ERR_INT_RAW_V << CACHE_L1_CACHE_PLD_ERR_INT_RAW_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW_S  11
/** CACHE_SYNC_ERR_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw bit of the interrupt that occurs only when Cache sync-operation error
 *  occurs.
 */
#define CACHE_SYNC_ERR_INT_RAW    (BIT(13))
#define CACHE_SYNC_ERR_INT_RAW_M  (CACHE_SYNC_ERR_INT_RAW_V << CACHE_SYNC_ERR_INT_RAW_S)
#define CACHE_SYNC_ERR_INT_RAW_V  0x00000001U
#define CACHE_SYNC_ERR_INT_RAW_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_ST_REG register
 *  L1-Cache Access Fail Interrupt status register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_ST_REG (DR_REG_CACHE_BASE + 0x240)
/** CACHE_L1_ICACHE2_PLD_DONE_INT_ST : RO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_ST_V << CACHE_L1_ICACHE2_PLD_DONE_INT_ST_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST_S  2
/** CACHE_L1_CACHE_PLD_DONE_INT_ST : RO; bitpos: [4]; default: 0;
 *  The bit indicates the status of the interrupt that occurs only when L1-Cache
 *  preload-operation is done.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_ST    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_ST_M  (CACHE_L1_CACHE_PLD_DONE_INT_ST_V << CACHE_L1_CACHE_PLD_DONE_INT_ST_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_ST_S  4
/** CACHE_SYNC_DONE_INT_ST : RO; bitpos: [6]; default: 0;
 *  The bit indicates the status of the interrupt that occurs only when Cache
 *  sync-operation is done.
 */
#define CACHE_SYNC_DONE_INT_ST    (BIT(6))
#define CACHE_SYNC_DONE_INT_ST_M  (CACHE_SYNC_DONE_INT_ST_V << CACHE_SYNC_DONE_INT_ST_S)
#define CACHE_SYNC_DONE_INT_ST_V  0x00000001U
#define CACHE_SYNC_DONE_INT_ST_S  6
/** CACHE_L1_ICACHE2_PLD_ERR_INT_ST : RO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_ST_V << CACHE_L1_ICACHE2_PLD_ERR_INT_ST_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST_S  9
/** CACHE_L1_CACHE_PLD_ERR_INT_ST : RO; bitpos: [11]; default: 0;
 *  The bit indicates the status of the interrupt of L1-Cache preload-operation error.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_ST    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_ST_M  (CACHE_L1_CACHE_PLD_ERR_INT_ST_V << CACHE_L1_CACHE_PLD_ERR_INT_ST_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_ST_S  11
/** CACHE_SYNC_ERR_INT_ST : RO; bitpos: [13]; default: 0;
 *  The bit indicates the status of the interrupt of Cache sync-operation error.
 */
#define CACHE_SYNC_ERR_INT_ST    (BIT(13))
#define CACHE_SYNC_ERR_INT_ST_M  (CACHE_SYNC_ERR_INT_ST_V << CACHE_SYNC_ERR_INT_ST_S)
#define CACHE_SYNC_ERR_INT_ST_V  0x00000001U
#define CACHE_SYNC_ERR_INT_ST_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_EXCEPTION_REG register
 *  Cache Sync/Preload Operation exception register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_EXCEPTION_REG (DR_REG_CACHE_BASE + 0x244)
/** CACHE_L1_ICACHE2_PLD_ERR_CODE : RO; bitpos: [5:4]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_ICACHE2_PLD_ERR_CODE_M  (CACHE_L1_ICACHE2_PLD_ERR_CODE_V << CACHE_L1_ICACHE2_PLD_ERR_CODE_S)
#define CACHE_L1_ICACHE2_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_ICACHE2_PLD_ERR_CODE_S  4
/** CACHE_L1_CACHE_PLD_ERR_CODE : RO; bitpos: [9:8]; default: 0;
 *  The value 2 is Only available which means preload size is error in L1-Cache.
 */
#define CACHE_L1_CACHE_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_CACHE_PLD_ERR_CODE_M  (CACHE_L1_CACHE_PLD_ERR_CODE_V << CACHE_L1_CACHE_PLD_ERR_CODE_S)
#define CACHE_L1_CACHE_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_CACHE_PLD_ERR_CODE_S  8
/** CACHE_SYNC_ERR_CODE : RO; bitpos: [13:12]; default: 0;
 *  The values 0-2 are available which means sync map, command conflict and size are
 *  error in Cache System.
 */
#define CACHE_SYNC_ERR_CODE    0x00000003U
#define CACHE_SYNC_ERR_CODE_M  (CACHE_SYNC_ERR_CODE_V << CACHE_SYNC_ERR_CODE_S)
#define CACHE_SYNC_ERR_CODE_V  0x00000003U
#define CACHE_SYNC_ERR_CODE_S  12

/** CACHE_L1_CACHE_SYNC_RST_CTRL_REG register
 *  Cache Sync Reset control register
 */
#define CACHE_L1_CACHE_SYNC_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x248)
/** CACHE_L1_ICACHE2_SYNC_RST : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_SYNC_RST    (BIT(2))
#define CACHE_L1_ICACHE2_SYNC_RST_M  (CACHE_L1_ICACHE2_SYNC_RST_V << CACHE_L1_ICACHE2_SYNC_RST_S)
#define CACHE_L1_ICACHE2_SYNC_RST_V  0x00000001U
#define CACHE_L1_ICACHE2_SYNC_RST_S  2
/** CACHE_L1_CACHE_SYNC_RST : R/W; bitpos: [4]; default: 0;
 *  set this bit to reset sync-logic inside L1-Cache. Recommend that this should only
 *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
 */
#define CACHE_L1_CACHE_SYNC_RST    (BIT(4))
#define CACHE_L1_CACHE_SYNC_RST_M  (CACHE_L1_CACHE_SYNC_RST_V << CACHE_L1_CACHE_SYNC_RST_S)
#define CACHE_L1_CACHE_SYNC_RST_V  0x00000001U
#define CACHE_L1_CACHE_SYNC_RST_S  4

/** CACHE_L1_CACHE_PRELOAD_RST_CTRL_REG register
 *  Cache Preload Reset control register
 */
#define CACHE_L1_CACHE_PRELOAD_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x24c)
/** CACHE_L1_ICACHE2_PLD_RST : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_RST    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_RST_M  (CACHE_L1_ICACHE2_PLD_RST_V << CACHE_L1_ICACHE2_PLD_RST_S)
#define CACHE_L1_ICACHE2_PLD_RST_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_RST_S  2
/** CACHE_L1_CACHE_PLD_RST : R/W; bitpos: [4]; default: 0;
 *  set this bit to reset preload-logic inside L1-Cache. Recommend that this should
 *  only be used to initialize preload-logic when some fatal error of preload-logic
 *  occurs.
 */
#define CACHE_L1_CACHE_PLD_RST    (BIT(4))
#define CACHE_L1_CACHE_PLD_RST_M  (CACHE_L1_CACHE_PLD_RST_V << CACHE_L1_CACHE_PLD_RST_S)
#define CACHE_L1_CACHE_PLD_RST_V  0x00000001U
#define CACHE_L1_CACHE_PLD_RST_S  4

/** CACHE_L1_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG register
 *  Cache Autoload buffer clear control register
 */
#define CACHE_L1_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG (DR_REG_CACHE_BASE + 0x250)
/** CACHE_L1_ICACHE2_ALD_BUF_CLR : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_ALD_BUF_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_ALD_BUF_CLR_M  (CACHE_L1_ICACHE2_ALD_BUF_CLR_V << CACHE_L1_ICACHE2_ALD_BUF_CLR_S)
#define CACHE_L1_ICACHE2_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_ALD_BUF_CLR_S  2
/** CACHE_L1_CACHE_ALD_BUF_CLR : R/W; bitpos: [4]; default: 0;
 *  set this bit to clear autoload-buffer inside L1-Cache. If this bit is active,
 *  autoload will not work in L1-Cache. This bit should not be active when autoload
 *  works in L1-Cache.
 */
#define CACHE_L1_CACHE_ALD_BUF_CLR    (BIT(4))
#define CACHE_L1_CACHE_ALD_BUF_CLR_M  (CACHE_L1_CACHE_ALD_BUF_CLR_V << CACHE_L1_CACHE_ALD_BUF_CLR_S)
#define CACHE_L1_CACHE_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_CACHE_ALD_BUF_CLR_S  4

/** CACHE_L1_UNALLOCATE_BUFFER_CLEAR_REG register
 *  Unallocate request buffer clear registers
 */
#define CACHE_L1_UNALLOCATE_BUFFER_CLEAR_REG (DR_REG_CACHE_BASE + 0x254)
/** CACHE_L1_ICACHE2_UNALLOC_CLR : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_UNALLOC_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_UNALLOC_CLR_M  (CACHE_L1_ICACHE2_UNALLOC_CLR_V << CACHE_L1_ICACHE2_UNALLOC_CLR_S)
#define CACHE_L1_ICACHE2_UNALLOC_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_UNALLOC_CLR_S  2
/** CACHE_L1_CACHE_UNALLOC_CLR : R/W; bitpos: [4]; default: 0;
 *  The bit is used to clear the unallocate request buffer of l1 cache where the
 *  unallocate request is responded but not completed.
 */
#define CACHE_L1_CACHE_UNALLOC_CLR    (BIT(4))
#define CACHE_L1_CACHE_UNALLOC_CLR_M  (CACHE_L1_CACHE_UNALLOC_CLR_V << CACHE_L1_CACHE_UNALLOC_CLR_S)
#define CACHE_L1_CACHE_UNALLOC_CLR_V  0x00000001U
#define CACHE_L1_CACHE_UNALLOC_CLR_S  4

/** CACHE_L1_CACHE_OBJECT_CTRL_REG register
 *  Cache Tag and Data memory Object control register
 */
#define CACHE_L1_CACHE_OBJECT_CTRL_REG (DR_REG_CACHE_BASE + 0x258)
/** CACHE_L1_ICACHE2_TAG_OBJECT : R/W; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_OBJECT    (BIT(2))
#define CACHE_L1_ICACHE2_TAG_OBJECT_M  (CACHE_L1_ICACHE2_TAG_OBJECT_V << CACHE_L1_ICACHE2_TAG_OBJECT_S)
#define CACHE_L1_ICACHE2_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_OBJECT_S  2
/** CACHE_L1_CACHE_TAG_OBJECT : R/W; bitpos: [4]; default: 0;
 *  Set this bit to set L1-Cache tag memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L1_CACHE_TAG_OBJECT    (BIT(4))
#define CACHE_L1_CACHE_TAG_OBJECT_M  (CACHE_L1_CACHE_TAG_OBJECT_V << CACHE_L1_CACHE_TAG_OBJECT_S)
#define CACHE_L1_CACHE_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_CACHE_TAG_OBJECT_S  4
/** CACHE_L1_ICACHE2_MEM_OBJECT : R/W; bitpos: [8]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_MEM_OBJECT    (BIT(8))
#define CACHE_L1_ICACHE2_MEM_OBJECT_M  (CACHE_L1_ICACHE2_MEM_OBJECT_V << CACHE_L1_ICACHE2_MEM_OBJECT_S)
#define CACHE_L1_ICACHE2_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE2_MEM_OBJECT_S  8
/** CACHE_L1_CACHE_MEM_OBJECT : R/W; bitpos: [10]; default: 0;
 *  Set this bit to set L1-Cache data memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L1_CACHE_MEM_OBJECT    (BIT(10))
#define CACHE_L1_CACHE_MEM_OBJECT_M  (CACHE_L1_CACHE_MEM_OBJECT_V << CACHE_L1_CACHE_MEM_OBJECT_S)
#define CACHE_L1_CACHE_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_CACHE_MEM_OBJECT_S  10

/** CACHE_L1_CACHE_WAY_OBJECT_REG register
 *  Cache Tag and Data memory way register
 */
#define CACHE_L1_CACHE_WAY_OBJECT_REG (DR_REG_CACHE_BASE + 0x25c)
/** CACHE_L1_CACHE_WAY_OBJECT : R/W; bitpos: [2:0]; default: 0;
 *  Set this bits to select which way of the tag-object will be accessed. 0: way0, 1:
 *  way1, 2: way2, 3: way3, ?, 7: way7.
 */
#define CACHE_L1_CACHE_WAY_OBJECT    0x00000007U
#define CACHE_L1_CACHE_WAY_OBJECT_M  (CACHE_L1_CACHE_WAY_OBJECT_V << CACHE_L1_CACHE_WAY_OBJECT_S)
#define CACHE_L1_CACHE_WAY_OBJECT_V  0x00000007U
#define CACHE_L1_CACHE_WAY_OBJECT_S  0

/** CACHE_L1_CACHE_ADDR_REG register
 *  Cache address register
 */
#define CACHE_L1_CACHE_ADDR_REG (DR_REG_CACHE_BASE + 0x260)
/** CACHE_L1_CACHE_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits stores the address which will decide where inside the specified tag
 *  memory object will be accessed.
 */
#define CACHE_L1_CACHE_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_ADDR_M  (CACHE_L1_CACHE_ADDR_V << CACHE_L1_CACHE_ADDR_S)
#define CACHE_L1_CACHE_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_ADDR_S  0

/** CACHE_L1_CACHE_DEBUG_BUS_REG register
 *  Cache Tag/data memory content register
 */
#define CACHE_L1_CACHE_DEBUG_BUS_REG (DR_REG_CACHE_BASE + 0x264)
/** CACHE_L1_CACHE_DEBUG_BUS : R/W; bitpos: [31:0]; default: 612;
 *  This is a constant place where we can write data to or read data from the tag/data
 *  memory on the specified cache.
 */
#define CACHE_L1_CACHE_DEBUG_BUS    0xFFFFFFFFU
#define CACHE_L1_CACHE_DEBUG_BUS_M  (CACHE_L1_CACHE_DEBUG_BUS_V << CACHE_L1_CACHE_DEBUG_BUS_S)
#define CACHE_L1_CACHE_DEBUG_BUS_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_DEBUG_BUS_S  0

/** CACHE_TRACE_ENA_REG register
 *  Clock gate control register
 */
#define CACHE_TRACE_ENA_REG (DR_REG_CACHE_BASE + 0x3d0)
/** CACHE_L1_CACHE_TRACE_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable L1-Cache trace for the performance counter and fail tracer
 */
#define CACHE_L1_CACHE_TRACE_ENA    (BIT(0))
#define CACHE_L1_CACHE_TRACE_ENA_M  (CACHE_L1_CACHE_TRACE_ENA_V << CACHE_L1_CACHE_TRACE_ENA_S)
#define CACHE_L1_CACHE_TRACE_ENA_V  0x00000001U
#define CACHE_L1_CACHE_TRACE_ENA_S  0

/** CACHE_DATE_REG register
 *  Version control register
 */
#define CACHE_DATE_REG (DR_REG_CACHE_BASE + 0x3fc)
/** CACHE_DATE : R/W; bitpos: [27:0]; default: 36773968;
 *  version control register. Note that this default value stored is the latest date
 *  when the hardware logic was updated.
 */
#define CACHE_DATE    0x0FFFFFFFU
#define CACHE_DATE_M  (CACHE_DATE_V << CACHE_DATE_S)
#define CACHE_DATE_V  0x0FFFFFFFU
#define CACHE_DATE_S  0

#ifdef __cplusplus
}
#endif
