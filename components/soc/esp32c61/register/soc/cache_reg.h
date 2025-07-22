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

/** CACHE_L1_CACHE_CTRL_REG register
 *  L1 data cache (L1 cache) control register
 */
#define CACHE_L1_CACHE_CTRL_REG (DR_REG_CACHE_BASE + 0x4)
/** CACHE_L1_CACHE_SHUT_BUS0 : R/W; bitpos: [0]; default: 0;
 *  Configures whether to disable BUS0 to access the L1 cache.
 *  0: Enable
 *  1: Disable
 */
#define CACHE_L1_CACHE_SHUT_BUS0    (BIT(0))
#define CACHE_L1_CACHE_SHUT_BUS0_M  (CACHE_L1_CACHE_SHUT_BUS0_V << CACHE_L1_CACHE_SHUT_BUS0_S)
#define CACHE_L1_CACHE_SHUT_BUS0_V  0x00000001U
#define CACHE_L1_CACHE_SHUT_BUS0_S  0
/** CACHE_L1_CACHE_SHUT_BUS1 : R/W; bitpos: [1]; default: 0;
 *  Configures whether to disable BUS1 to access the L1 cache.
 *  0: Enable
 *  1: Disable
 */
#define CACHE_L1_CACHE_SHUT_BUS1    (BIT(1))
#define CACHE_L1_CACHE_SHUT_BUS1_M  (CACHE_L1_CACHE_SHUT_BUS1_V << CACHE_L1_CACHE_SHUT_BUS1_S)
#define CACHE_L1_CACHE_SHUT_BUS1_V  0x00000001U
#define CACHE_L1_CACHE_SHUT_BUS1_S  1

/** CACHE_L1_CACHE_CACHESIZE_CONF_REG register
 *  Cache cache size register
 */
#define CACHE_L1_CACHE_CACHESIZE_CONF_REG (DR_REG_CACHE_BASE + 0x18)
/** CACHE_L1_CACHE_CACHESIZE_32K : RO; bitpos: [7]; default: 1;
 *  Indicates that cache size is 32 KB.
 */
#define CACHE_L1_CACHE_CACHESIZE_32K    (BIT(7))
#define CACHE_L1_CACHE_CACHESIZE_32K_M  (CACHE_L1_CACHE_CACHESIZE_32K_V << CACHE_L1_CACHE_CACHESIZE_32K_S)
#define CACHE_L1_CACHE_CACHESIZE_32K_V  0x00000001U
#define CACHE_L1_CACHE_CACHESIZE_32K_S  7

/** CACHE_L1_CACHE_BLOCKSIZE_CONF_REG register
 *  Cache block size register
 */
#define CACHE_L1_CACHE_BLOCKSIZE_CONF_REG (DR_REG_CACHE_BASE + 0x1c)
/** CACHE_L1_CACHE_BLOCKSIZE_32 : RO; bitpos: [2]; default: 1;
 *  Indicates that the cache block size is 32 bytes.
 */
#define CACHE_L1_CACHE_BLOCKSIZE_32    (BIT(2))
#define CACHE_L1_CACHE_BLOCKSIZE_32_M  (CACHE_L1_CACHE_BLOCKSIZE_32_V << CACHE_L1_CACHE_BLOCKSIZE_32_S)
#define CACHE_L1_CACHE_BLOCKSIZE_32_V  0x00000001U
#define CACHE_L1_CACHE_BLOCKSIZE_32_S  2

/** CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG register
 *  Cache critical word first control register
 */
#define CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG (DR_REG_CACHE_BASE + 0x20)
/** CACHE_L1_CACHE_WRAP : R/W; bitpos: [4]; default: 0;
 *  Configures whether to enable the critical word first mode for the L1 cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_WRAP    (BIT(4))
#define CACHE_L1_CACHE_WRAP_M  (CACHE_L1_CACHE_WRAP_V << CACHE_L1_CACHE_WRAP_S)
#define CACHE_L1_CACHE_WRAP_V  0x00000001U
#define CACHE_L1_CACHE_WRAP_S  4

/** CACHE_L1_CACHE_MISS_ACCESS_CTRL_REG register
 *  Cache wrap around control register
 */
#define CACHE_L1_CACHE_MISS_ACCESS_CTRL_REG (DR_REG_CACHE_BASE + 0x24)
/** CACHE_L1_CACHE_MISS_DISABLE_ACCESS : R/W; bitpos: [4]; default: 0;
 *  Configures whether to disable early restart function.
 *  0: Enable
 *  1: Disable
 */
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS    (BIT(4))
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS_M  (CACHE_L1_CACHE_MISS_DISABLE_ACCESS_V << CACHE_L1_CACHE_MISS_DISABLE_ACCESS_S)
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS_V  0x00000001U
#define CACHE_L1_CACHE_MISS_DISABLE_ACCESS_S  4

/** CACHE_L1_CACHE_FREEZE_CTRL_REG register
 *  Cache freeze control register
 */
#define CACHE_L1_CACHE_FREEZE_CTRL_REG (DR_REG_CACHE_BASE + 0x28)
/** CACHE_L1_CACHE_FREEZE_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether to enable freeze operation in the L1 cache. It can be cleared by
 *  software.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_FREEZE_EN    (BIT(16))
#define CACHE_L1_CACHE_FREEZE_EN_M  (CACHE_L1_CACHE_FREEZE_EN_V << CACHE_L1_CACHE_FREEZE_EN_S)
#define CACHE_L1_CACHE_FREEZE_EN_V  0x00000001U
#define CACHE_L1_CACHE_FREEZE_EN_S  16
/** CACHE_L1_CACHE_FREEZE_MODE : R/W; bitpos: [17]; default: 0;
 *  Configures the freeze mode in the L1 cache.
 *  0: Assert busy if a cache miss occurs
 *  1: Assert hit if a cache miss occurs
 */
#define CACHE_L1_CACHE_FREEZE_MODE    (BIT(17))
#define CACHE_L1_CACHE_FREEZE_MODE_M  (CACHE_L1_CACHE_FREEZE_MODE_V << CACHE_L1_CACHE_FREEZE_MODE_S)
#define CACHE_L1_CACHE_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_CACHE_FREEZE_MODE_S  17
/** CACHE_L1_CACHE_FREEZE_DONE : RO; bitpos: [18]; default: 0;
 *  Represents whether the freeze operation in L1 cache is finished.
 *  0: Not finished
 *  1: Finished
 */
#define CACHE_L1_CACHE_FREEZE_DONE    (BIT(18))
#define CACHE_L1_CACHE_FREEZE_DONE_M  (CACHE_L1_CACHE_FREEZE_DONE_V << CACHE_L1_CACHE_FREEZE_DONE_S)
#define CACHE_L1_CACHE_FREEZE_DONE_V  0x00000001U
#define CACHE_L1_CACHE_FREEZE_DONE_S  18

/** CACHE_L1_CACHE_DATA_MEM_ACS_CONF_REG register
 *  Cache data memory access configuration register
 */
#define CACHE_L1_CACHE_DATA_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x2c)
/** CACHE_L1_CACHE_DATA_MEM_RD_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether to enable the configuration bus to read the L1 cache data memory.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_DATA_MEM_RD_EN    (BIT(16))
#define CACHE_L1_CACHE_DATA_MEM_RD_EN_M  (CACHE_L1_CACHE_DATA_MEM_RD_EN_V << CACHE_L1_CACHE_DATA_MEM_RD_EN_S)
#define CACHE_L1_CACHE_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_CACHE_DATA_MEM_RD_EN_S  16
/** CACHE_L1_CACHE_DATA_MEM_WR_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether to enable the configuration bus to write the L1 cache data
 *  memory.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_DATA_MEM_WR_EN    (BIT(17))
#define CACHE_L1_CACHE_DATA_MEM_WR_EN_M  (CACHE_L1_CACHE_DATA_MEM_WR_EN_V << CACHE_L1_CACHE_DATA_MEM_WR_EN_S)
#define CACHE_L1_CACHE_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_CACHE_DATA_MEM_WR_EN_S  17

/** CACHE_L1_CACHE_TAG_MEM_ACS_CONF_REG register
 *  Cache tag memory access configuration register
 */
#define CACHE_L1_CACHE_TAG_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x30)
/** CACHE_L1_CACHE_TAG_MEM_RD_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether to enable the configuration bus to read the L1 cache tag memory.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_TAG_MEM_RD_EN    (BIT(16))
#define CACHE_L1_CACHE_TAG_MEM_RD_EN_M  (CACHE_L1_CACHE_TAG_MEM_RD_EN_V << CACHE_L1_CACHE_TAG_MEM_RD_EN_S)
#define CACHE_L1_CACHE_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_CACHE_TAG_MEM_RD_EN_S  16
/** CACHE_L1_CACHE_TAG_MEM_WR_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether to enable the configuration bus to write the L1 cache tag memory.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_TAG_MEM_WR_EN    (BIT(17))
#define CACHE_L1_CACHE_TAG_MEM_WR_EN_M  (CACHE_L1_CACHE_TAG_MEM_WR_EN_V << CACHE_L1_CACHE_TAG_MEM_WR_EN_S)
#define CACHE_L1_CACHE_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_CACHE_TAG_MEM_WR_EN_S  17

/** CACHE_L1_CACHE_PRELOCK_CONF_REG register
 *  L1 Cache prelock configure register
 */
#define CACHE_L1_CACHE_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x74)
/** CACHE_L1_CACHE_PRELOCK_SCT0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether to enable the prelocking function in Section 0 of the L1 cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN_M  (CACHE_L1_CACHE_PRELOCK_SCT0_EN_V << CACHE_L1_CACHE_PRELOCK_SCT0_EN_S)
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_CACHE_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_CACHE_PRELOCK_SCT1_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether to enable the prelocking function in Section 1 of the L1 cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN_M  (CACHE_L1_CACHE_PRELOCK_SCT1_EN_V << CACHE_L1_CACHE_PRELOCK_SCT1_EN_S)
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_CACHE_PRELOCK_SCT1_EN_S  1

/** CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_REG register
 *  L1 cache prelocking Section 0 address configuration register
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x78)
/** CACHE_L1_CACHE_PRELOCK_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting virtual address of Section 0 for prelocking in the L1
 *  cache. This field should be configured together with
 *  CACHE_L1_CACHE_PRELOCK_SCT0_SIZE.
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_M  (CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_V << CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_REG register
 *  L1 cache prelocking Section 1 address configuration register
 */
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x7c)
/** CACHE_L1_CACHE_PRELOCK_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting virtual address of Section 1 for prelocking in the L1
 *  cache. This field should be configured together with
 *  CACHE_L1_CACHE_PRELOCK_SCT1_SIZE.
 */
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_M  (CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_V << CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_CACHE_PRELOCK_SCT_SIZE_REG register
 *  L1 cache prelocking section size configuration register
 */
#define CACHE_L1_CACHE_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x80)
/** CACHE_L1_CACHE_PRELOCK_SCT0_SIZE : R/W; bitpos: [13:0]; default: 16383;
 *  Configures the size of Section 0 for prelocking in the L1 cache. This field should
 *  be configured together with CACHE_L1_CACHE_PRELOCK_SCT0_ADDR_REG.
 */
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_M  (CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_V << CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_CACHE_PRELOCK_SCT1_SIZE : R/W; bitpos: [29:16]; default: 16383;
 *  Configures the size of Section 1 for prelocking in the L1 cache. This field should
 *  be configured together with CACHE_L1_CACHE_PRELOCK_SCT1_ADDR_REG.
 */
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_M  (CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_V << CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_CACHE_PRELOCK_SCT1_SIZE_S  16

/** CACHE_LOCK_CTRL_REG register
 *  Locking operation control register
 */
#define CACHE_LOCK_CTRL_REG (DR_REG_CACHE_BASE + 0x84)
/** CACHE_LOCK_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  Configures whether to enable lock operation. It will be cleared by hardware after
 *  the lock operation is done.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_LOCK_ENA    (BIT(0))
#define CACHE_LOCK_ENA_M  (CACHE_LOCK_ENA_V << CACHE_LOCK_ENA_S)
#define CACHE_LOCK_ENA_V  0x00000001U
#define CACHE_LOCK_ENA_S  0
/** CACHE_UNLOCK_ENA : R/W/SC; bitpos: [1]; default: 0;
 *  Configures whether to enable unlock operation. It will be cleared by hardware after
 *  the unlock operation is done.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_UNLOCK_ENA    (BIT(1))
#define CACHE_UNLOCK_ENA_M  (CACHE_UNLOCK_ENA_V << CACHE_UNLOCK_ENA_S)
#define CACHE_UNLOCK_ENA_V  0x00000001U
#define CACHE_UNLOCK_ENA_S  1
/** CACHE_LOCK_DONE : RO; bitpos: [2]; default: 1;
 *  Represents whether the unlock or the lock operation is finished.
 *  0: Not finished
 *  1: Finished
 */
#define CACHE_LOCK_DONE    (BIT(2))
#define CACHE_LOCK_DONE_M  (CACHE_LOCK_DONE_V << CACHE_LOCK_DONE_S)
#define CACHE_LOCK_DONE_V  0x00000001U
#define CACHE_LOCK_DONE_S  2

/** CACHE_LOCK_MAP_REG register
 *  Locking map configuration register
 */
#define CACHE_LOCK_MAP_REG (DR_REG_CACHE_BASE + 0x88)
/** CACHE_LOCK_MAP : R/W; bitpos: [5:0]; default: 0;
 *  Configures which level of cache in the two-level cache structure to perform the
 *  locking or the unlocking operation.
 *  Bit 4: Write 1 to this bit to select the L1 cache
 *  Other bits: reserved
 */
#define CACHE_LOCK_MAP    0x0000003FU
#define CACHE_LOCK_MAP_M  (CACHE_LOCK_MAP_V << CACHE_LOCK_MAP_S)
#define CACHE_LOCK_MAP_V  0x0000003FU
#define CACHE_LOCK_MAP_S  0

/** CACHE_LOCK_ADDR_REG register
 *  Locking address configuration register
 */
#define CACHE_LOCK_ADDR_REG (DR_REG_CACHE_BASE + 0x8c)
/** CACHE_LOCK_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting virtual address for the locking or unlocking operation.
 *  This field should be configured together with CACHE_LOCK_SIZE_REG.
 */
#define CACHE_LOCK_ADDR    0xFFFFFFFFU
#define CACHE_LOCK_ADDR_M  (CACHE_LOCK_ADDR_V << CACHE_LOCK_ADDR_S)
#define CACHE_LOCK_ADDR_V  0xFFFFFFFFU
#define CACHE_LOCK_ADDR_S  0

/** CACHE_LOCK_SIZE_REG register
 *  Locking size configuration register
 */
#define CACHE_LOCK_SIZE_REG (DR_REG_CACHE_BASE + 0x90)
/** CACHE_LOCK_SIZE : R/W; bitpos: [15:0]; default: 0;
 *  Configures the size for the locking or unlocking operation. This field should be
 *  configured together with CACHE_LOCK_ADDR_REG.
 */
#define CACHE_LOCK_SIZE    0x0000FFFFU
#define CACHE_LOCK_SIZE_M  (CACHE_LOCK_SIZE_V << CACHE_LOCK_SIZE_S)
#define CACHE_LOCK_SIZE_V  0x0000FFFFU
#define CACHE_LOCK_SIZE_S  0

/** CACHE_SYNC_CTRL_REG register
 *  Sync operation control register
 */
#define CACHE_SYNC_CTRL_REG (DR_REG_CACHE_BASE + 0x94)
/** CACHE_INVALIDATE_ENA : RW; bitpos: [0]; default: 1;
 *  Configures whether to enable the invalidatation operation. Hardware will clear it
 *  once the operation has finished.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_INVALIDATE_ENA    (BIT(0))
#define CACHE_INVALIDATE_ENA_M  (CACHE_INVALIDATE_ENA_V << CACHE_INVALIDATE_ENA_S)
#define CACHE_INVALIDATE_ENA_V  0x00000001U
#define CACHE_INVALIDATE_ENA_S  0
/** CACHE_CLEAN_ENA : R/W/SC; bitpos: [1]; default: 0;
 *  Configures whether to enable the clean operation. Hardware will clear it once the
 *  operation has finished.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_CLEAN_ENA    (BIT(1))
#define CACHE_CLEAN_ENA_M  (CACHE_CLEAN_ENA_V << CACHE_CLEAN_ENA_S)
#define CACHE_CLEAN_ENA_V  0x00000001U
#define CACHE_CLEAN_ENA_S  1
/** CACHE_WRITEBACK_ENA : R/W/SC; bitpos: [2]; default: 0;
 *  Configures whether to enable the write-back operation. Hardware will clear it once
 *  the operation has finished.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_WRITEBACK_ENA    (BIT(2))
#define CACHE_WRITEBACK_ENA_M  (CACHE_WRITEBACK_ENA_V << CACHE_WRITEBACK_ENA_S)
#define CACHE_WRITEBACK_ENA_V  0x00000001U
#define CACHE_WRITEBACK_ENA_S  2
/** CACHE_WRITEBACK_INVALIDATE_ENA : R/W/SC; bitpos: [3]; default: 0;
 *  Configures whether to enable the write-back invalidation operation. Hardware will
 *  clear it once the operation has finished.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_WRITEBACK_INVALIDATE_ENA    (BIT(3))
#define CACHE_WRITEBACK_INVALIDATE_ENA_M  (CACHE_WRITEBACK_INVALIDATE_ENA_V << CACHE_WRITEBACK_INVALIDATE_ENA_S)
#define CACHE_WRITEBACK_INVALIDATE_ENA_V  0x00000001U
#define CACHE_WRITEBACK_INVALIDATE_ENA_S  3
/** CACHE_SYNC_DONE : RO; bitpos: [4]; default: 0;
 *  Represents whether the synchronization operation is finished.
 *  0: Not finished
 *  1: Finished
 */
#define CACHE_SYNC_DONE    (BIT(4))
#define CACHE_SYNC_DONE_M  (CACHE_SYNC_DONE_V << CACHE_SYNC_DONE_S)
#define CACHE_SYNC_DONE_V  0x00000001U
#define CACHE_SYNC_DONE_S  4

/** CACHE_SYNC_MAP_REG register
 *  Sync map configuration register
 */
#define CACHE_SYNC_MAP_REG (DR_REG_CACHE_BASE + 0x98)
/** CACHE_SYNC_MAP : R/W; bitpos: [5:0]; default: 63;
 *  Configures which caches in the two-level cache structure will apply the sync
 *  operation.
 *  Bit 4: Write 1 to this bit to select the L1 cache
 *  Other bits: Reserved
 */
#define CACHE_SYNC_MAP    0x0000003FU
#define CACHE_SYNC_MAP_M  (CACHE_SYNC_MAP_V << CACHE_SYNC_MAP_S)
#define CACHE_SYNC_MAP_V  0x0000003FU
#define CACHE_SYNC_MAP_S  0

/** CACHE_SYNC_ADDR_REG register
 *  Sync address configuration register
 */
#define CACHE_SYNC_ADDR_REG (DR_REG_CACHE_BASE + 0x9c)
/** CACHE_SYNC_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting virtual address for the synchronization operation. This
 *  field should be configured together with CACHE_SYNC_SIZE_REG.
 */
#define CACHE_SYNC_ADDR    0xFFFFFFFFU
#define CACHE_SYNC_ADDR_M  (CACHE_SYNC_ADDR_V << CACHE_SYNC_ADDR_S)
#define CACHE_SYNC_ADDR_V  0xFFFFFFFFU
#define CACHE_SYNC_ADDR_S  0

/** CACHE_SYNC_SIZE_REG register
 *  Sync size configuration register
 */
#define CACHE_SYNC_SIZE_REG (DR_REG_CACHE_BASE + 0xa0)
/** CACHE_SYNC_SIZE : R/W; bitpos: [24:0]; default: 0;
 *  Configures the size for the synchronization operation. This field should be
 *  configured together with CACHE_SYNC_ADDR_REG.
 */
#define CACHE_SYNC_SIZE    0x01FFFFFFU
#define CACHE_SYNC_SIZE_M  (CACHE_SYNC_SIZE_V << CACHE_SYNC_SIZE_S)
#define CACHE_SYNC_SIZE_V  0x01FFFFFFU
#define CACHE_SYNC_SIZE_S  0

/** CACHE_L1_CACHE_PRELOAD_CTRL_REG register
 *  L1 cache preloading operation control register
 */
#define CACHE_L1_CACHE_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xd4)
/** CACHE_L1_CACHE_PRELOAD_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  Configures whether to enable the preloading operation in the L1 cache. It will be
 *  cleared by hardware automatically after the preloading operation is done.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_PRELOAD_ENA    (BIT(0))
#define CACHE_L1_CACHE_PRELOAD_ENA_M  (CACHE_L1_CACHE_PRELOAD_ENA_V << CACHE_L1_CACHE_PRELOAD_ENA_S)
#define CACHE_L1_CACHE_PRELOAD_ENA_V  0x00000001U
#define CACHE_L1_CACHE_PRELOAD_ENA_S  0
/** CACHE_L1_CACHE_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  Represents whether the preloading operation is finished.
 *  0: Not finished
 *  1: Finished
 */
#define CACHE_L1_CACHE_PRELOAD_DONE    (BIT(1))
#define CACHE_L1_CACHE_PRELOAD_DONE_M  (CACHE_L1_CACHE_PRELOAD_DONE_V << CACHE_L1_CACHE_PRELOAD_DONE_S)
#define CACHE_L1_CACHE_PRELOAD_DONE_V  0x00000001U
#define CACHE_L1_CACHE_PRELOAD_DONE_S  1
/** CACHE_L1_CACHE_PRELOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  Configures the direction of the preloading operation.
 *  0: Ascending
 *  1: Descending
 */
#define CACHE_L1_CACHE_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_CACHE_PRELOAD_ORDER_M  (CACHE_L1_CACHE_PRELOAD_ORDER_V << CACHE_L1_CACHE_PRELOAD_ORDER_S)
#define CACHE_L1_CACHE_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_CACHE_PRELOAD_ORDER_S  2

/** CACHE_L1_CACHE_PRELOAD_ADDR_REG register
 *  L1 cache preloading address configuration register
 */
#define CACHE_L1_CACHE_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xd8)
/** CACHE_L1_CACHE_PRELOAD_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting virtual address for the preloading operation in the L1
 *  cache. This field should be configured together with
 *  CACHE_L1_CACHE_PRELOAD_SIZE_REG.
 */
#define CACHE_L1_CACHE_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOAD_ADDR_M  (CACHE_L1_CACHE_PRELOAD_ADDR_V << CACHE_L1_CACHE_PRELOAD_ADDR_S)
#define CACHE_L1_CACHE_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_PRELOAD_ADDR_S  0

/** CACHE_L1_CACHE_PRELOAD_SIZE_REG register
 *  L1 cache preloading size configuration register
 */
#define CACHE_L1_CACHE_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xdc)
/** CACHE_L1_CACHE_PRELOAD_SIZE : R/W; bitpos: [13:0]; default: 0;
 *  Configures the size of Section 0 for the preloading operation in the L1 cache. This
 *  field should be configured together with CACHE_L1_CACHE_PRELOAD_ADDR_REG.
 */
#define CACHE_L1_CACHE_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_CACHE_PRELOAD_SIZE_M  (CACHE_L1_CACHE_PRELOAD_SIZE_V << CACHE_L1_CACHE_PRELOAD_SIZE_S)
#define CACHE_L1_CACHE_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_CACHE_PRELOAD_SIZE_S  0

/** CACHE_L1_CACHE_AUTOLOAD_CTRL_REG register
 *  L1 cache autoloading operation control register
 */
#define CACHE_L1_CACHE_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x130)
/** CACHE_L1_CACHE_AUTOLOAD_ENA : R/W; bitpos: [0]; default: 0;
 *  Configures whether to enable the autoloading operation in the L1 cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L1_CACHE_AUTOLOAD_ENA_M  (CACHE_L1_CACHE_AUTOLOAD_ENA_V << CACHE_L1_CACHE_AUTOLOAD_ENA_S)
#define CACHE_L1_CACHE_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_ENA_S  0
/** CACHE_L1_CACHE_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  Represents whether the autoloading operation in the L1 cache is finished.
 *  0: Not finished
 *  1: Finished
 */
#define CACHE_L1_CACHE_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L1_CACHE_AUTOLOAD_DONE_M  (CACHE_L1_CACHE_AUTOLOAD_DONE_V << CACHE_L1_CACHE_AUTOLOAD_DONE_S)
#define CACHE_L1_CACHE_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_DONE_S  1
/** CACHE_L1_CACHE_AUTOLOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  Configures the direction of the autoloading operation in the L1 cache.
 *  0: Ascending
 *  1: Descending
 */
#define CACHE_L1_CACHE_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_CACHE_AUTOLOAD_ORDER_M  (CACHE_L1_CACHE_AUTOLOAD_ORDER_V << CACHE_L1_CACHE_AUTOLOAD_ORDER_S)
#define CACHE_L1_CACHE_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_ORDER_S  2
/** CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE : R/W; bitpos: [4:3]; default: 0;
 *  Configures the trigger mode of the autoloading operation in the L1 cache.
 *  0/3: Triggered by misses
 *  1: Triggered by hits
 *  2: Triggered by misses or hits
 */
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_CACHE_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA : R/W; bitpos: [8]; default: 0;
 *  Configures whether to enable Section 0 for the autoloading operation in the L1
 *  cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_V << CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA : R/W; bitpos: [9]; default: 0;
 *  Configures whether to enable Section 1 for the autoloading operation in the L1
 *  cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_V << CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA_S  9

/** CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 cache autoloading Section 0 address configuration register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x134)
/** CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting virtual address of Section 0 for the autoloading operation
 *  in the L1 cache. Note that it should be used together with
 *  CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE and CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_M  (CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_V << CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_REG register
 *  L1 cache autoloading Section 0 size configuration register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x138)
/** CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE : R/W; bitpos: [24:0]; default: 0;
 *  Configures the size of Section 0 for the autoloading operation in the L1 cache.
 *  Note that it should be used together with CACHE_L1_CACHE_AUTOLOAD_SCT0_ADDR and
 *  CACHE_L1_CACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE    0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_M  (CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_V << CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_V  0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_REG register
 *  L1 cache autoloading Section 1 address configuration register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x13c)
/** CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting virtual address of Section 1 for the autoloading operation
 *  in the L1 cache. Note that it should be used together with
 *  CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE and CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 cache autoloading Section 1 size configuration register
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x140)
/** CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE : R/W; bitpos: [24:0]; default: 0;
 *  Configures the size of Section 1 for the autoloading operation in the L1 cache.
 *  Note that it should be used together with CACHE_L1_CACHE_AUTOLOAD_SCT1_ADDR and
 *  CACHE_L1_CACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE    0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_V  0x01FFFFFFU
#define CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_CACHE_ACS_CNT_INT_ENA_REG register
 *  Cache access counter interrupt enable register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_ENA_REG (DR_REG_CACHE_BASE + 0x154)
/** CACHE_L1_BUS0_OVF_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable L1_BUS0_OVF_INT.
 */
#define CACHE_L1_BUS0_OVF_INT_ENA    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_ENA_M  (CACHE_L1_BUS0_OVF_INT_ENA_V << CACHE_L1_BUS0_OVF_INT_ENA_S)
#define CACHE_L1_BUS0_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_ENA_S  4
/** CACHE_L1_BUS1_OVF_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Write 1 to enable L1_BUS1_OVF_INT.
 */
#define CACHE_L1_BUS1_OVF_INT_ENA    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_ENA_M  (CACHE_L1_BUS1_OVF_INT_ENA_V << CACHE_L1_BUS1_OVF_INT_ENA_S)
#define CACHE_L1_BUS1_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_ENA_S  5

/** CACHE_L1_CACHE_ACS_CNT_INT_CLR_REG register
 *  Cache access counter interrupt clear register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_CLR_REG (DR_REG_CACHE_BASE + 0x158)
/** CACHE_L1_BUS0_OVF_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear L1_BUS0_OVF_INT.
 */
#define CACHE_L1_BUS0_OVF_INT_CLR    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_CLR_M  (CACHE_L1_BUS0_OVF_INT_CLR_V << CACHE_L1_BUS0_OVF_INT_CLR_S)
#define CACHE_L1_BUS0_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_CLR_S  4
/** CACHE_L1_BUS1_OVF_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Write 1 to clear L1_BUS1_OVF_INT.
 */
#define CACHE_L1_BUS1_OVF_INT_CLR    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_CLR_M  (CACHE_L1_BUS1_OVF_INT_CLR_V << CACHE_L1_BUS1_OVF_INT_CLR_S)
#define CACHE_L1_BUS1_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_CLR_S  5

/** CACHE_L1_CACHE_ACS_CNT_INT_RAW_REG register
 *  Cache access counter interrupt raw register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_RAW_REG (DR_REG_CACHE_BASE + 0x15c)
/** CACHE_L1_BUS0_OVF_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status of L1_BUS0_OVF_INT.
 */
#define CACHE_L1_BUS0_OVF_INT_RAW    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_RAW_M  (CACHE_L1_BUS0_OVF_INT_RAW_V << CACHE_L1_BUS0_OVF_INT_RAW_S)
#define CACHE_L1_BUS0_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_RAW_S  4
/** CACHE_L1_BUS1_OVF_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt status of L1_BUS1_OVF_INT.
 */
#define CACHE_L1_BUS1_OVF_INT_RAW    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_RAW_M  (CACHE_L1_BUS1_OVF_INT_RAW_V << CACHE_L1_BUS1_OVF_INT_RAW_S)
#define CACHE_L1_BUS1_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_RAW_S  5

/** CACHE_L1_CACHE_ACS_CNT_INT_ST_REG register
 *  Cache access counter interrupt status register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_ST_REG (DR_REG_CACHE_BASE + 0x160)
/** CACHE_L1_BUS0_OVF_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status of L1_BUS0_OVF_INT.
 */
#define CACHE_L1_BUS0_OVF_INT_ST    (BIT(4))
#define CACHE_L1_BUS0_OVF_INT_ST_M  (CACHE_L1_BUS0_OVF_INT_ST_V << CACHE_L1_BUS0_OVF_INT_ST_S)
#define CACHE_L1_BUS0_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_BUS0_OVF_INT_ST_S  4
/** CACHE_L1_BUS1_OVF_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status of L1_BUS1_OVF_INT.
 */
#define CACHE_L1_BUS1_OVF_INT_ST    (BIT(5))
#define CACHE_L1_BUS1_OVF_INT_ST_M  (CACHE_L1_BUS1_OVF_INT_ST_V << CACHE_L1_BUS1_OVF_INT_ST_S)
#define CACHE_L1_BUS1_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_BUS1_OVF_INT_ST_S  5

/** CACHE_L1_CACHE_ACS_FAIL_CTRL_REG register
 *  Cache access failed interrupt configuration register
 */
#define CACHE_L1_CACHE_ACS_FAIL_CTRL_REG (DR_REG_CACHE_BASE + 0x164)
/** CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE : R/W; bitpos: [4]; default: 0;
 *  Configures whether to pass the exception of cache accessing lower-level memory to
 *  the upper-level device.
 *  0: Do not pass to the upper-level device
 *  1: Pass to the upper-level device
 */
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE    (BIT(4))
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_V << CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_CACHE_ACS_FAIL_CHECK_MODE_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_ENA_REG register
 *  Cache access failed interrupt enable register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_ENA_REG (DR_REG_CACHE_BASE + 0x168)
/** CACHE_L1_CACHE_FAIL_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable L1_CACHE_FAIL_INT.
 */
#define CACHE_L1_CACHE_FAIL_INT_ENA    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_ENA_M  (CACHE_L1_CACHE_FAIL_INT_ENA_V << CACHE_L1_CACHE_FAIL_INT_ENA_S)
#define CACHE_L1_CACHE_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_ENA_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_CLR_REG register
 *  L1 cache access failed interrupt clear register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_CLR_REG (DR_REG_CACHE_BASE + 0x16c)
/** CACHE_L1_CACHE_FAIL_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear L1_CACHE_FAIL_INT.
 */
#define CACHE_L1_CACHE_FAIL_INT_CLR    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_CLR_M  (CACHE_L1_CACHE_FAIL_INT_CLR_V << CACHE_L1_CACHE_FAIL_INT_CLR_S)
#define CACHE_L1_CACHE_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_CLR_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_RAW_REG register
 *  Cache access failed interrupt raw register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_RAW_REG (DR_REG_CACHE_BASE + 0x170)
/** CACHE_L1_CACHE_FAIL_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status of L1_CACHE_FAIL_INT.
 */
#define CACHE_L1_CACHE_FAIL_INT_RAW    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_RAW_M  (CACHE_L1_CACHE_FAIL_INT_RAW_V << CACHE_L1_CACHE_FAIL_INT_RAW_S)
#define CACHE_L1_CACHE_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_RAW_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_ST_REG register
 *  Cache access failed interrupt status register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_ST_REG (DR_REG_CACHE_BASE + 0x174)
/** CACHE_L1_CACHE_FAIL_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status of L1_CACHE_FAIL_INT.
 */
#define CACHE_L1_CACHE_FAIL_INT_ST    (BIT(4))
#define CACHE_L1_CACHE_FAIL_INT_ST_M  (CACHE_L1_CACHE_FAIL_INT_ST_V << CACHE_L1_CACHE_FAIL_INT_ST_S)
#define CACHE_L1_CACHE_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_CACHE_FAIL_INT_ST_S  4

/** CACHE_L1_CACHE_ACS_CNT_CTRL_REG register
 *  Cache access counter enable and clear register
 */
#define CACHE_L1_CACHE_ACS_CNT_CTRL_REG (DR_REG_CACHE_BASE + 0x178)
/** CACHE_L1_BUS0_CNT_ENA : R/W; bitpos: [4]; default: 0;
 *  Configures whether to enable the BUS0 counters in the L1 cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_BUS0_CNT_ENA    (BIT(4))
#define CACHE_L1_BUS0_CNT_ENA_M  (CACHE_L1_BUS0_CNT_ENA_V << CACHE_L1_BUS0_CNT_ENA_S)
#define CACHE_L1_BUS0_CNT_ENA_V  0x00000001U
#define CACHE_L1_BUS0_CNT_ENA_S  4
/** CACHE_L1_BUS1_CNT_ENA : R/W; bitpos: [5]; default: 0;
 *  Configures whether to enable the BUS1 counters in the L1 cache.
 *  0: Disable
 *  1: Enable
 */
#define CACHE_L1_BUS1_CNT_ENA    (BIT(5))
#define CACHE_L1_BUS1_CNT_ENA_M  (CACHE_L1_BUS1_CNT_ENA_V << CACHE_L1_BUS1_CNT_ENA_S)
#define CACHE_L1_BUS1_CNT_ENA_V  0x00000001U
#define CACHE_L1_BUS1_CNT_ENA_S  5
/** CACHE_L1_BUS0_CNT_CLR : WT; bitpos: [20]; default: 0;
 *  Configures whether to clear the BUS0 counters in the L1 cache.
 *  0: Not clear
 *  1: Clear
 */
#define CACHE_L1_BUS0_CNT_CLR    (BIT(20))
#define CACHE_L1_BUS0_CNT_CLR_M  (CACHE_L1_BUS0_CNT_CLR_V << CACHE_L1_BUS0_CNT_CLR_S)
#define CACHE_L1_BUS0_CNT_CLR_V  0x00000001U
#define CACHE_L1_BUS0_CNT_CLR_S  20
/** CACHE_L1_BUS1_CNT_CLR : WT; bitpos: [21]; default: 0;
 *  Configures whether to clear the BUS1 counters in the L1 cache.
 *  0: Not clear
 *  1: Clear
 */
#define CACHE_L1_BUS1_CNT_CLR    (BIT(21))
#define CACHE_L1_BUS1_CNT_CLR_M  (CACHE_L1_BUS1_CNT_CLR_V << CACHE_L1_BUS1_CNT_CLR_S)
#define CACHE_L1_BUS1_CNT_CLR_V  0x00000001U
#define CACHE_L1_BUS1_CNT_CLR_S  21

/** CACHE_L1_BUS0_ACS_HIT_CNT_REG register
 *  L1 cache BUS0 hit-access counter register
 */
#define CACHE_L1_BUS0_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1bc)
/** CACHE_L1_BUS0_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of hits when BUS0 accesses the L1 cache.
 */
#define CACHE_L1_BUS0_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_HIT_CNT_M  (CACHE_L1_BUS0_HIT_CNT_V << CACHE_L1_BUS0_HIT_CNT_S)
#define CACHE_L1_BUS0_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_HIT_CNT_S  0

/** CACHE_L1_BUS0_ACS_MISS_CNT_REG register
 *  L1 cache BUS0 missed-access counter register
 */
#define CACHE_L1_BUS0_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1c0)
/** CACHE_L1_BUS0_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of misses when BUS0 accesses the L1 cache.
 */
#define CACHE_L1_BUS0_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_MISS_CNT_M  (CACHE_L1_BUS0_MISS_CNT_V << CACHE_L1_BUS0_MISS_CNT_S)
#define CACHE_L1_BUS0_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_MISS_CNT_S  0

/** CACHE_L1_BUS0_ACS_CONFLICT_CNT_REG register
 *  L1 cache BUS0 conflicting-access Counter register
 */
#define CACHE_L1_BUS0_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1c4)
/** CACHE_L1_BUS0_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of access conflicts when BUS0 accesses the L1 cache.
 */
#define CACHE_L1_BUS0_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_CONFLICT_CNT_M  (CACHE_L1_BUS0_CONFLICT_CNT_V << CACHE_L1_BUS0_CONFLICT_CNT_S)
#define CACHE_L1_BUS0_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_CONFLICT_CNT_S  0

/** CACHE_L1_BUS0_ACS_NXTLVL_RD_CNT_REG register
 *  L1 cache BUS0 reads the lower-level memory
 */
#define CACHE_L1_BUS0_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1c8)
/** CACHE_L1_BUS0_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of times that the cache accesses its lower-level module due
 *  to BUS0's read access.
 */
#define CACHE_L1_BUS0_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_RD_CNT_M  (CACHE_L1_BUS0_NXTLVL_RD_CNT_V << CACHE_L1_BUS0_NXTLVL_RD_CNT_S)
#define CACHE_L1_BUS0_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_RD_CNT_S  0

/** CACHE_L1_BUS0_ACS_NXTLVL_WR_CNT_REG register
 *  L1 cache BUS0 writes the lower-level memory
 */
#define CACHE_L1_BUS0_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x1cc)
/** CACHE_L1_BUS0_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of times that the cache accesses its lower-level module due
 *  to BUS0's write access.
 */
#define CACHE_L1_BUS0_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_WR_CNT_M  (CACHE_L1_BUS0_NXTLVL_WR_CNT_V << CACHE_L1_BUS0_NXTLVL_WR_CNT_S)
#define CACHE_L1_BUS0_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS0_NXTLVL_WR_CNT_S  0

/** CACHE_L1_BUS1_ACS_HIT_CNT_REG register
 *  L1 cache BUS1 hit-access counter register
 */
#define CACHE_L1_BUS1_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1d0)
/** CACHE_L1_BUS1_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of hits when BUS1 accesses the L1 cache.
 */
#define CACHE_L1_BUS1_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_HIT_CNT_M  (CACHE_L1_BUS1_HIT_CNT_V << CACHE_L1_BUS1_HIT_CNT_S)
#define CACHE_L1_BUS1_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_HIT_CNT_S  0

/** CACHE_L1_BUS1_ACS_MISS_CNT_REG register
 *  L1 cache BUS1 missed-access counter register
 */
#define CACHE_L1_BUS1_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1d4)
/** CACHE_L1_BUS1_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of misses when BUS1 accesses the L1 cache.
 */
#define CACHE_L1_BUS1_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_MISS_CNT_M  (CACHE_L1_BUS1_MISS_CNT_V << CACHE_L1_BUS1_MISS_CNT_S)
#define CACHE_L1_BUS1_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_MISS_CNT_S  0

/** CACHE_L1_BUS1_ACS_CONFLICT_CNT_REG register
 *  L1 cache BUS1 conflicting-access counter register
 */
#define CACHE_L1_BUS1_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1d8)
/** CACHE_L1_BUS1_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of access conflicts when BUS1 accesses the L1 cache.
 */
#define CACHE_L1_BUS1_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_CONFLICT_CNT_M  (CACHE_L1_BUS1_CONFLICT_CNT_V << CACHE_L1_BUS1_CONFLICT_CNT_S)
#define CACHE_L1_BUS1_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_CONFLICT_CNT_S  0

/** CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG register
 *  L1 cache BUS1 reads the lower-level memory
 */
#define CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1dc)
/** CACHE_L1_BUS1_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of times that the cache accesses its lower-level module due
 *  to BUS1's read access.
 */
#define CACHE_L1_BUS1_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_NXTLVL_RD_CNT_M  (CACHE_L1_BUS1_NXTLVL_RD_CNT_V << CACHE_L1_BUS1_NXTLVL_RD_CNT_S)
#define CACHE_L1_BUS1_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG register
 *  L1 cache BUS1 writes the lower-level memory
 */
#define CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x1e0)
/** CACHE_L1_BUS1_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  Represents the number of times that the cache accesses its lower-level module due
 *  to BUS1's write access.
 */
#define CACHE_L1_BUS1_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_BUS1_NXTLVL_WR_CNT_M  (CACHE_L1_BUS1_NXTLVL_WR_CNT_V << CACHE_L1_BUS1_NXTLVL_WR_CNT_S)
#define CACHE_L1_BUS1_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_BUS1_NXTLVL_WR_CNT_S  0

/** CACHE_L1_CACHE_ACS_FAIL_ID_ATTR_REG register
 *  L1 cache access failed ID and attribution recording register
 */
#define CACHE_L1_CACHE_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x22c)
/** CACHE_L1_CACHE_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  Represents the ID of the failed access to the L1 cache.
 *  Bit 4: BUS0
 *  Bit 5: BUS1
 *  Others: reserved
 */
#define CACHE_L1_CACHE_FAIL_ID    0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ID_M  (CACHE_L1_CACHE_FAIL_ID_V << CACHE_L1_CACHE_FAIL_ID_S)
#define CACHE_L1_CACHE_FAIL_ID_V  0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ID_S  0
/** CACHE_L1_CACHE_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  Represents the attribute of the failed access to the L1 cache.
 *  Bit 0: Fetching instructions
 *  Bit 1: Reading data
 *  Bit 7: non-cacheable
 *  Others: reserved
 */
#define CACHE_L1_CACHE_FAIL_ATTR    0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ATTR_M  (CACHE_L1_CACHE_FAIL_ATTR_V << CACHE_L1_CACHE_FAIL_ATTR_S)
#define CACHE_L1_CACHE_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L1_CACHE_FAIL_ATTR_S  16

/** CACHE_L1_CACHE_ACS_FAIL_ADDR_REG register
 *  L1 cache access failed address information register
 */
#define CACHE_L1_CACHE_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x230)
/** CACHE_L1_CACHE_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the failed access to the L1 cache.
 */
#define CACHE_L1_CACHE_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_FAIL_ADDR_M  (CACHE_L1_CACHE_FAIL_ADDR_V << CACHE_L1_CACHE_FAIL_ADDR_S)
#define CACHE_L1_CACHE_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_FAIL_ADDR_S  0

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_ENA_REG register
 *  Sync and preloading operation interrupt enable register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_ENA_REG (DR_REG_CACHE_BASE + 0x234)
/** CACHE_L1_CACHE_PLD_DONE_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable L1_CACHE_PLD_DONE_INT.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA_M  (CACHE_L1_CACHE_PLD_DONE_INT_ENA_V << CACHE_L1_CACHE_PLD_DONE_INT_ENA_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_ENA_S  4
/** CACHE_SYNC_DONE_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Write 1 to enable CACHE_SYNC_DONE_INT.
 */
#define CACHE_SYNC_DONE_INT_ENA    (BIT(6))
#define CACHE_SYNC_DONE_INT_ENA_M  (CACHE_SYNC_DONE_INT_ENA_V << CACHE_SYNC_DONE_INT_ENA_S)
#define CACHE_SYNC_DONE_INT_ENA_V  0x00000001U
#define CACHE_SYNC_DONE_INT_ENA_S  6
/** CACHE_L1_CACHE_PLD_ERR_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Write 1 to enable CACHE_PLD_ERR_INT.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA_M  (CACHE_L1_CACHE_PLD_ERR_INT_ENA_V << CACHE_L1_CACHE_PLD_ERR_INT_ENA_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_ENA_S  11
/** CACHE_SYNC_ERR_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  Write 1 to enable CACHE_SYNC_ERR_INT.
 */
#define CACHE_SYNC_ERR_INT_ENA    (BIT(13))
#define CACHE_SYNC_ERR_INT_ENA_M  (CACHE_SYNC_ERR_INT_ENA_V << CACHE_SYNC_ERR_INT_ENA_S)
#define CACHE_SYNC_ERR_INT_ENA_V  0x00000001U
#define CACHE_SYNC_ERR_INT_ENA_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_CLR_REG register
 *  Sync and preloading operation interrupt clear register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_CLR_REG (DR_REG_CACHE_BASE + 0x238)
/** CACHE_L1_CACHE_PLD_DONE_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear L1_CACHE_PLD_DONE_INT.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR_M  (CACHE_L1_CACHE_PLD_DONE_INT_CLR_V << CACHE_L1_CACHE_PLD_DONE_INT_CLR_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_CLR_S  4
/** CACHE_SYNC_DONE_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Write 1 to clear CACHE_SYNC_DONE_INT.
 */
#define CACHE_SYNC_DONE_INT_CLR    (BIT(6))
#define CACHE_SYNC_DONE_INT_CLR_M  (CACHE_SYNC_DONE_INT_CLR_V << CACHE_SYNC_DONE_INT_CLR_S)
#define CACHE_SYNC_DONE_INT_CLR_V  0x00000001U
#define CACHE_SYNC_DONE_INT_CLR_S  6
/** CACHE_L1_CACHE_PLD_ERR_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Write 1 to clear L1_CACHE_PLD_ERR_INT.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR_M  (CACHE_L1_CACHE_PLD_ERR_INT_CLR_V << CACHE_L1_CACHE_PLD_ERR_INT_CLR_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_CLR_S  11
/** CACHE_SYNC_ERR_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Write 1 to clear CACHE_SYNC_ERR_INT.
 */
#define CACHE_SYNC_ERR_INT_CLR    (BIT(13))
#define CACHE_SYNC_ERR_INT_CLR_M  (CACHE_SYNC_ERR_INT_CLR_V << CACHE_SYNC_ERR_INT_CLR_S)
#define CACHE_SYNC_ERR_INT_CLR_V  0x00000001U
#define CACHE_SYNC_ERR_INT_CLR_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_RAW_REG register
 *  Sync and preloading operation interrupt raw register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_RAW_REG (DR_REG_CACHE_BASE + 0x23c)
/** CACHE_L1_CACHE_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status of L1_CACHE_PLD_DONE_INT.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW_M  (CACHE_L1_CACHE_PLD_DONE_INT_RAW_V << CACHE_L1_CACHE_PLD_DONE_INT_RAW_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_RAW_S  4
/** CACHE_SYNC_DONE_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt status of CACHE_SYNC_DONE_INT.
 */
#define CACHE_SYNC_DONE_INT_RAW    (BIT(6))
#define CACHE_SYNC_DONE_INT_RAW_M  (CACHE_SYNC_DONE_INT_RAW_V << CACHE_SYNC_DONE_INT_RAW_S)
#define CACHE_SYNC_DONE_INT_RAW_V  0x00000001U
#define CACHE_SYNC_DONE_INT_RAW_S  6
/** CACHE_L1_CACHE_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt status of L1_CACHE_PLD_ERR_INT.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW_M  (CACHE_L1_CACHE_PLD_ERR_INT_RAW_V << CACHE_L1_CACHE_PLD_ERR_INT_RAW_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_RAW_S  11
/** CACHE_SYNC_ERR_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt status of CACHE_SYNC_ERR_INT.
 */
#define CACHE_SYNC_ERR_INT_RAW    (BIT(13))
#define CACHE_SYNC_ERR_INT_RAW_M  (CACHE_SYNC_ERR_INT_RAW_V << CACHE_SYNC_ERR_INT_RAW_S)
#define CACHE_SYNC_ERR_INT_RAW_V  0x00000001U
#define CACHE_SYNC_ERR_INT_RAW_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_INT_ST_REG register
 *  Sync and preloading operation interrupt status register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_INT_ST_REG (DR_REG_CACHE_BASE + 0x240)
/** CACHE_L1_CACHE_PLD_DONE_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status of L1_CACHE_PLD_DONE_INT.
 */
#define CACHE_L1_CACHE_PLD_DONE_INT_ST    (BIT(4))
#define CACHE_L1_CACHE_PLD_DONE_INT_ST_M  (CACHE_L1_CACHE_PLD_DONE_INT_ST_V << CACHE_L1_CACHE_PLD_DONE_INT_ST_S)
#define CACHE_L1_CACHE_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_CACHE_PLD_DONE_INT_ST_S  4
/** CACHE_SYNC_DONE_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status of CACHE_SYNC_DONE_INT.
 */
#define CACHE_SYNC_DONE_INT_ST    (BIT(6))
#define CACHE_SYNC_DONE_INT_ST_M  (CACHE_SYNC_DONE_INT_ST_V << CACHE_SYNC_DONE_INT_ST_S)
#define CACHE_SYNC_DONE_INT_ST_V  0x00000001U
#define CACHE_SYNC_DONE_INT_ST_S  6
/** CACHE_L1_CACHE_PLD_ERR_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status of L1_CACHE_PLD_ERR_INT.
 */
#define CACHE_L1_CACHE_PLD_ERR_INT_ST    (BIT(11))
#define CACHE_L1_CACHE_PLD_ERR_INT_ST_M  (CACHE_L1_CACHE_PLD_ERR_INT_ST_V << CACHE_L1_CACHE_PLD_ERR_INT_ST_S)
#define CACHE_L1_CACHE_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_CACHE_PLD_ERR_INT_ST_S  11
/** CACHE_SYNC_ERR_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status of CACHE_SYNC_ERR_INT.
 */
#define CACHE_SYNC_ERR_INT_ST    (BIT(13))
#define CACHE_SYNC_ERR_INT_ST_M  (CACHE_SYNC_ERR_INT_ST_V << CACHE_SYNC_ERR_INT_ST_S)
#define CACHE_SYNC_ERR_INT_ST_V  0x00000001U
#define CACHE_SYNC_ERR_INT_ST_S  13

/** CACHE_L1_CACHE_SYNC_PRELOAD_EXCEPTION_REG register
 *  Cache sync and preloading operation exception register
 */
#define CACHE_L1_CACHE_SYNC_PRELOAD_EXCEPTION_REG (DR_REG_CACHE_BASE + 0x244)
/** CACHE_L1_CACHE_PLD_ERR_CODE : RO; bitpos: [9:8]; default: 0;
 *  Represents the error code for the failed preloading operation.
 *  2: The preloading size in the L1 cache is wrong
 *  Other values: reserved
 */
#define CACHE_L1_CACHE_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_CACHE_PLD_ERR_CODE_M  (CACHE_L1_CACHE_PLD_ERR_CODE_V << CACHE_L1_CACHE_PLD_ERR_CODE_S)
#define CACHE_L1_CACHE_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_CACHE_PLD_ERR_CODE_S  8
/** CACHE_SYNC_ERR_CODE : RO; bitpos: [13:12]; default: 0;
 *  Represents the error code for the failed synchronization operation.
 *  0: Incorrect synchronization map
 *  1: Synchronization command conflict
 *  2: Incorrect synchronization size
 *  3: Reserved
 */
#define CACHE_SYNC_ERR_CODE    0x00000003U
#define CACHE_SYNC_ERR_CODE_M  (CACHE_SYNC_ERR_CODE_V << CACHE_SYNC_ERR_CODE_S)
#define CACHE_SYNC_ERR_CODE_V  0x00000003U
#define CACHE_SYNC_ERR_CODE_S  12

/** CACHE_L1_CACHE_SYNC_RST_CTRL_REG register
 *  Cache sync reset control register
 */
#define CACHE_L1_CACHE_SYNC_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x248)
/** CACHE_L1_CACHE_SYNC_RST : R/W; bitpos: [4]; default: 0;
 *  Configures whether to reset the synchronization logic inside the L1 cache.
 *  0: Not reset
 *  1: Reset
 *  It is recommended that this field should only be used when a fatal error of the
 *  synchronization logic occurs.
 */
#define CACHE_L1_CACHE_SYNC_RST    (BIT(4))
#define CACHE_L1_CACHE_SYNC_RST_M  (CACHE_L1_CACHE_SYNC_RST_V << CACHE_L1_CACHE_SYNC_RST_S)
#define CACHE_L1_CACHE_SYNC_RST_V  0x00000001U
#define CACHE_L1_CACHE_SYNC_RST_S  4

/** CACHE_L1_CACHE_PRELOAD_RST_CTRL_REG register
 *  Cache preloading reset control register
 */
#define CACHE_L1_CACHE_PRELOAD_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x24c)
/** CACHE_L1_CACHE_PLD_RST : R/W; bitpos: [4]; default: 0;
 *  Configures whether to reset the preloading logic inside the L1 cache.
 *  0: Not reset
 *  1: Reset
 *  It is recommended that this field should only be used when a fatal error of the
 *  preloading logic occurs.
 */
#define CACHE_L1_CACHE_PLD_RST    (BIT(4))
#define CACHE_L1_CACHE_PLD_RST_M  (CACHE_L1_CACHE_PLD_RST_V << CACHE_L1_CACHE_PLD_RST_S)
#define CACHE_L1_CACHE_PLD_RST_V  0x00000001U
#define CACHE_L1_CACHE_PLD_RST_S  4

/** CACHE_L1_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG register
 *  Cache autoloading buffer clear control register
 */
#define CACHE_L1_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG (DR_REG_CACHE_BASE + 0x250)
/** CACHE_L1_CACHE_ALD_BUF_CLR : R/W; bitpos: [4]; default: 0;
 *  Configures whether to clear the autoloading buffer inside the L1 cache.
 *  0: Not clear the buffer. Recommended when autoloading of L1 cache is used.
 *  1: Clear the buffer. Once set, autoloading of L1 cache will stop working.
 */
#define CACHE_L1_CACHE_ALD_BUF_CLR    (BIT(4))
#define CACHE_L1_CACHE_ALD_BUF_CLR_M  (CACHE_L1_CACHE_ALD_BUF_CLR_V << CACHE_L1_CACHE_ALD_BUF_CLR_S)
#define CACHE_L1_CACHE_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_CACHE_ALD_BUF_CLR_S  4

/** CACHE_L1_CACHE_OBJECT_CTRL_REG register
 *  Cache tag and data memory object control register
 */
#define CACHE_L1_CACHE_OBJECT_CTRL_REG (DR_REG_CACHE_BASE + 0x258)
/** CACHE_L1_CACHE_TAG_OBJECT : R/W; bitpos: [4]; default: 0;
 *  Configure whether to set the L1 cache tag memory as an object.
 *  0: Not set
 *  1: Set
 *  This register uses "one-hot encoding," which means that at any time, only one field
 *  can be valid in conjunction with other fields.
 */
#define CACHE_L1_CACHE_TAG_OBJECT    (BIT(4))
#define CACHE_L1_CACHE_TAG_OBJECT_M  (CACHE_L1_CACHE_TAG_OBJECT_V << CACHE_L1_CACHE_TAG_OBJECT_S)
#define CACHE_L1_CACHE_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_CACHE_TAG_OBJECT_S  4
/** CACHE_L1_CACHE_MEM_OBJECT : R/W; bitpos: [10]; default: 0;
 *  Configure whether to set the L1 cache data memory as an object.
 *  0: Not set
 *  1: Set
 *  This register uses "one-hot encoding," which means that at any time, only one field
 *  can be valid in conjunction with other fields.
 */
#define CACHE_L1_CACHE_MEM_OBJECT    (BIT(10))
#define CACHE_L1_CACHE_MEM_OBJECT_M  (CACHE_L1_CACHE_MEM_OBJECT_V << CACHE_L1_CACHE_MEM_OBJECT_S)
#define CACHE_L1_CACHE_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_CACHE_MEM_OBJECT_S  10

/** CACHE_L1_CACHE_WAY_OBJECT_REG register
 *  Cache tag and data memory way register
 */
#define CACHE_L1_CACHE_WAY_OBJECT_REG (DR_REG_CACHE_BASE + 0x25c)
/** CACHE_L1_CACHE_WAY_OBJECT : R/W; bitpos: [2:0]; default: 0;
 *  Configures which way in the tag memory or the data memory to access.
 *  0: Way 0
 *  1: Way 1
 *  2: Way 2
 *  3: Way 3
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
 *  Configures the virtual address inside the tag memory or data memory to access.
 */
#define CACHE_L1_CACHE_ADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_ADDR_M  (CACHE_L1_CACHE_ADDR_V << CACHE_L1_CACHE_ADDR_S)
#define CACHE_L1_CACHE_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_ADDR_S  0

/** CACHE_L1_CACHE_DEBUG_BUS_REG register
 *  Cache tag and data memory content register
 */
#define CACHE_L1_CACHE_DEBUG_BUS_REG (DR_REG_CACHE_BASE + 0x264)
/** CACHE_L1_CACHE_DEBUG_BUS : R/W; bitpos: [31:0]; default: 612;
 *  Configures the data to write to or the data to read from the tag memory or the data
 *  memory.
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
/** CACHE_DATE : R/W; bitpos: [27:0]; default: 36774432;
 *  Version control register. Note that the default value is the latest date when the
 *  hardware logic was updated.
 */
#define CACHE_DATE    0x0FFFFFFFU
#define CACHE_DATE_M  (CACHE_DATE_V << CACHE_DATE_S)
#define CACHE_DATE_V  0x0FFFFFFFU
#define CACHE_DATE_S  0

#ifdef __cplusplus
}
#endif
