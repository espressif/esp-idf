/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** CACHE_L1_ICACHE_CTRL_REG register
 *  L1 instruction Cache(L1-ICache) control register
 */
#define CACHE_L1_ICACHE_CTRL_REG (DR_REG_CACHE_BASE + 0x0)
/** CACHE_L1_ICACHE_SHUT_IBUS0 : R/W; bitpos: [0]; default: 0;
 *  The bit is used to disable core0 ibus access L1-ICache, 0: enable, 1: disable
 */
#define CACHE_L1_ICACHE_SHUT_IBUS0    (BIT(0))
#define CACHE_L1_ICACHE_SHUT_IBUS0_M  (CACHE_L1_ICACHE_SHUT_IBUS0_V << CACHE_L1_ICACHE_SHUT_IBUS0_S)
#define CACHE_L1_ICACHE_SHUT_IBUS0_V  0x00000001U
#define CACHE_L1_ICACHE_SHUT_IBUS0_S  0
/** CACHE_L1_ICACHE_SHUT_IBUS1 : R/W; bitpos: [1]; default: 0;
 *  The bit is used to disable core1 ibus access L1-ICache, 0: enable, 1: disable
 */
#define CACHE_L1_ICACHE_SHUT_IBUS1    (BIT(1))
#define CACHE_L1_ICACHE_SHUT_IBUS1_M  (CACHE_L1_ICACHE_SHUT_IBUS1_V << CACHE_L1_ICACHE_SHUT_IBUS1_S)
#define CACHE_L1_ICACHE_SHUT_IBUS1_V  0x00000001U
#define CACHE_L1_ICACHE_SHUT_IBUS1_S  1
/** CACHE_L1_ICACHE_SHUT_IBUS2 : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE_SHUT_IBUS2    (BIT(2))
#define CACHE_L1_ICACHE_SHUT_IBUS2_M  (CACHE_L1_ICACHE_SHUT_IBUS2_V << CACHE_L1_ICACHE_SHUT_IBUS2_S)
#define CACHE_L1_ICACHE_SHUT_IBUS2_V  0x00000001U
#define CACHE_L1_ICACHE_SHUT_IBUS2_S  2
/** CACHE_L1_ICACHE_SHUT_IBUS3 : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE_SHUT_IBUS3    (BIT(3))
#define CACHE_L1_ICACHE_SHUT_IBUS3_M  (CACHE_L1_ICACHE_SHUT_IBUS3_V << CACHE_L1_ICACHE_SHUT_IBUS3_S)
#define CACHE_L1_ICACHE_SHUT_IBUS3_V  0x00000001U
#define CACHE_L1_ICACHE_SHUT_IBUS3_S  3

/** CACHE_L1_DCACHE_CTRL_REG register
 *  L1 data Cache(L1-DCache) control register
 */
#define CACHE_L1_DCACHE_CTRL_REG (DR_REG_CACHE_BASE + 0x4)
/** CACHE_L1_DCACHE_SHUT_DBUS0 : R/W; bitpos: [0]; default: 0;
 *  The bit is used to disable core0 dbus access L1-DCache, 0: enable, 1: disable
 */
#define CACHE_L1_DCACHE_SHUT_DBUS0    (BIT(0))
#define CACHE_L1_DCACHE_SHUT_DBUS0_M  (CACHE_L1_DCACHE_SHUT_DBUS0_V << CACHE_L1_DCACHE_SHUT_DBUS0_S)
#define CACHE_L1_DCACHE_SHUT_DBUS0_V  0x00000001U
#define CACHE_L1_DCACHE_SHUT_DBUS0_S  0
/** CACHE_L1_DCACHE_SHUT_DBUS1 : R/W; bitpos: [1]; default: 0;
 *  The bit is used to disable core1 dbus access L1-DCache, 0: enable, 1: disable
 */
#define CACHE_L1_DCACHE_SHUT_DBUS1    (BIT(1))
#define CACHE_L1_DCACHE_SHUT_DBUS1_M  (CACHE_L1_DCACHE_SHUT_DBUS1_V << CACHE_L1_DCACHE_SHUT_DBUS1_S)
#define CACHE_L1_DCACHE_SHUT_DBUS1_V  0x00000001U
#define CACHE_L1_DCACHE_SHUT_DBUS1_S  1
/** CACHE_L1_DCACHE_SHUT_DBUS2 : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DCACHE_SHUT_DBUS2    (BIT(2))
#define CACHE_L1_DCACHE_SHUT_DBUS2_M  (CACHE_L1_DCACHE_SHUT_DBUS2_V << CACHE_L1_DCACHE_SHUT_DBUS2_S)
#define CACHE_L1_DCACHE_SHUT_DBUS2_V  0x00000001U
#define CACHE_L1_DCACHE_SHUT_DBUS2_S  2
/** CACHE_L1_DCACHE_SHUT_DBUS3 : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DCACHE_SHUT_DBUS3    (BIT(3))
#define CACHE_L1_DCACHE_SHUT_DBUS3_M  (CACHE_L1_DCACHE_SHUT_DBUS3_V << CACHE_L1_DCACHE_SHUT_DBUS3_S)
#define CACHE_L1_DCACHE_SHUT_DBUS3_V  0x00000001U
#define CACHE_L1_DCACHE_SHUT_DBUS3_S  3
/** CACHE_L1_DCACHE_SHUT_DMA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to disable DMA access L1-DCache, 0: enable, 1: disable
 */
#define CACHE_L1_DCACHE_SHUT_DMA    (BIT(4))
#define CACHE_L1_DCACHE_SHUT_DMA_M  (CACHE_L1_DCACHE_SHUT_DMA_V << CACHE_L1_DCACHE_SHUT_DMA_S)
#define CACHE_L1_DCACHE_SHUT_DMA_V  0x00000001U
#define CACHE_L1_DCACHE_SHUT_DMA_S  4

/** CACHE_L1_BYPASS_CACHE_CONF_REG register
 *  Bypass Cache configure register
 */
#define CACHE_L1_BYPASS_CACHE_CONF_REG (DR_REG_CACHE_BASE + 0x8)
/** CACHE_BYPASS_L1_ICACHE0_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable bypass L1-ICache0. 0: disable bypass, 1: enable bypass.
 */
#define CACHE_BYPASS_L1_ICACHE0_EN    (BIT(0))
#define CACHE_BYPASS_L1_ICACHE0_EN_M  (CACHE_BYPASS_L1_ICACHE0_EN_V << CACHE_BYPASS_L1_ICACHE0_EN_S)
#define CACHE_BYPASS_L1_ICACHE0_EN_V  0x00000001U
#define CACHE_BYPASS_L1_ICACHE0_EN_S  0
/** CACHE_BYPASS_L1_ICACHE1_EN : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable bypass L1-ICache1. 0: disable bypass, 1: enable bypass.
 */
#define CACHE_BYPASS_L1_ICACHE1_EN    (BIT(1))
#define CACHE_BYPASS_L1_ICACHE1_EN_M  (CACHE_BYPASS_L1_ICACHE1_EN_V << CACHE_BYPASS_L1_ICACHE1_EN_S)
#define CACHE_BYPASS_L1_ICACHE1_EN_V  0x00000001U
#define CACHE_BYPASS_L1_ICACHE1_EN_S  1
/** CACHE_BYPASS_L1_ICACHE2_EN : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_BYPASS_L1_ICACHE2_EN    (BIT(2))
#define CACHE_BYPASS_L1_ICACHE2_EN_M  (CACHE_BYPASS_L1_ICACHE2_EN_V << CACHE_BYPASS_L1_ICACHE2_EN_S)
#define CACHE_BYPASS_L1_ICACHE2_EN_V  0x00000001U
#define CACHE_BYPASS_L1_ICACHE2_EN_S  2
/** CACHE_BYPASS_L1_ICACHE3_EN : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_BYPASS_L1_ICACHE3_EN    (BIT(3))
#define CACHE_BYPASS_L1_ICACHE3_EN_M  (CACHE_BYPASS_L1_ICACHE3_EN_V << CACHE_BYPASS_L1_ICACHE3_EN_S)
#define CACHE_BYPASS_L1_ICACHE3_EN_V  0x00000001U
#define CACHE_BYPASS_L1_ICACHE3_EN_S  3
/** CACHE_BYPASS_L1_DCACHE_EN : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable bypass L1-DCache. 0: disable bypass, 1: enable bypass.
 */
#define CACHE_BYPASS_L1_DCACHE_EN    (BIT(4))
#define CACHE_BYPASS_L1_DCACHE_EN_M  (CACHE_BYPASS_L1_DCACHE_EN_V << CACHE_BYPASS_L1_DCACHE_EN_S)
#define CACHE_BYPASS_L1_DCACHE_EN_V  0x00000001U
#define CACHE_BYPASS_L1_DCACHE_EN_S  4

/** CACHE_L1_CACHE_ATOMIC_CONF_REG register
 *  L1 Cache atomic feature configure register
 */
#define CACHE_L1_CACHE_ATOMIC_CONF_REG (DR_REG_CACHE_BASE + 0xc)
/** CACHE_L1_DCACHE_ATOMIC_EN : R/W; bitpos: [0]; default: 1;
 *  The bit is used to enable atomic feature on L1-DCache when multiple cores access
 *  L1-DCache.  1: disable, 1: enable.
 */
#define CACHE_L1_DCACHE_ATOMIC_EN    (BIT(0))
#define CACHE_L1_DCACHE_ATOMIC_EN_M  (CACHE_L1_DCACHE_ATOMIC_EN_V << CACHE_L1_DCACHE_ATOMIC_EN_S)
#define CACHE_L1_DCACHE_ATOMIC_EN_V  0x00000001U
#define CACHE_L1_DCACHE_ATOMIC_EN_S  0

/** CACHE_L1_ICACHE_CACHESIZE_CONF_REG register
 *  L1 instruction Cache CacheSize mode configure register
 */
#define CACHE_L1_ICACHE_CACHESIZE_CONF_REG (DR_REG_CACHE_BASE + 0x10)
/** CACHE_L1_ICACHE_CACHESIZE_256 : HRO; bitpos: [0]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 256 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_256    (BIT(0))
#define CACHE_L1_ICACHE_CACHESIZE_256_M  (CACHE_L1_ICACHE_CACHESIZE_256_V << CACHE_L1_ICACHE_CACHESIZE_256_S)
#define CACHE_L1_ICACHE_CACHESIZE_256_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_256_S  0
/** CACHE_L1_ICACHE_CACHESIZE_512 : HRO; bitpos: [1]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 512 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_512    (BIT(1))
#define CACHE_L1_ICACHE_CACHESIZE_512_M  (CACHE_L1_ICACHE_CACHESIZE_512_V << CACHE_L1_ICACHE_CACHESIZE_512_S)
#define CACHE_L1_ICACHE_CACHESIZE_512_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_512_S  1
/** CACHE_L1_ICACHE_CACHESIZE_1K : HRO; bitpos: [2]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 1k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_1K    (BIT(2))
#define CACHE_L1_ICACHE_CACHESIZE_1K_M  (CACHE_L1_ICACHE_CACHESIZE_1K_V << CACHE_L1_ICACHE_CACHESIZE_1K_S)
#define CACHE_L1_ICACHE_CACHESIZE_1K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_1K_S  2
/** CACHE_L1_ICACHE_CACHESIZE_2K : HRO; bitpos: [3]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 2k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_2K    (BIT(3))
#define CACHE_L1_ICACHE_CACHESIZE_2K_M  (CACHE_L1_ICACHE_CACHESIZE_2K_V << CACHE_L1_ICACHE_CACHESIZE_2K_S)
#define CACHE_L1_ICACHE_CACHESIZE_2K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_2K_S  3
/** CACHE_L1_ICACHE_CACHESIZE_4K : HRO; bitpos: [4]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 4k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_4K    (BIT(4))
#define CACHE_L1_ICACHE_CACHESIZE_4K_M  (CACHE_L1_ICACHE_CACHESIZE_4K_V << CACHE_L1_ICACHE_CACHESIZE_4K_S)
#define CACHE_L1_ICACHE_CACHESIZE_4K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_4K_S  4
/** CACHE_L1_ICACHE_CACHESIZE_8K : HRO; bitpos: [5]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 8k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_8K    (BIT(5))
#define CACHE_L1_ICACHE_CACHESIZE_8K_M  (CACHE_L1_ICACHE_CACHESIZE_8K_V << CACHE_L1_ICACHE_CACHESIZE_8K_S)
#define CACHE_L1_ICACHE_CACHESIZE_8K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_8K_S  5
/** CACHE_L1_ICACHE_CACHESIZE_16K : HRO; bitpos: [6]; default: 1;
 *  The field is used to configure cachesize of L1-ICache as 16k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_16K    (BIT(6))
#define CACHE_L1_ICACHE_CACHESIZE_16K_M  (CACHE_L1_ICACHE_CACHESIZE_16K_V << CACHE_L1_ICACHE_CACHESIZE_16K_S)
#define CACHE_L1_ICACHE_CACHESIZE_16K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_16K_S  6
/** CACHE_L1_ICACHE_CACHESIZE_32K : HRO; bitpos: [7]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 32k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_32K    (BIT(7))
#define CACHE_L1_ICACHE_CACHESIZE_32K_M  (CACHE_L1_ICACHE_CACHESIZE_32K_V << CACHE_L1_ICACHE_CACHESIZE_32K_S)
#define CACHE_L1_ICACHE_CACHESIZE_32K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_32K_S  7
/** CACHE_L1_ICACHE_CACHESIZE_64K : HRO; bitpos: [8]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 64k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_64K    (BIT(8))
#define CACHE_L1_ICACHE_CACHESIZE_64K_M  (CACHE_L1_ICACHE_CACHESIZE_64K_V << CACHE_L1_ICACHE_CACHESIZE_64K_S)
#define CACHE_L1_ICACHE_CACHESIZE_64K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_64K_S  8
/** CACHE_L1_ICACHE_CACHESIZE_128K : HRO; bitpos: [9]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 128k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_128K    (BIT(9))
#define CACHE_L1_ICACHE_CACHESIZE_128K_M  (CACHE_L1_ICACHE_CACHESIZE_128K_V << CACHE_L1_ICACHE_CACHESIZE_128K_S)
#define CACHE_L1_ICACHE_CACHESIZE_128K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_128K_S  9
/** CACHE_L1_ICACHE_CACHESIZE_256K : HRO; bitpos: [10]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 256k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_256K    (BIT(10))
#define CACHE_L1_ICACHE_CACHESIZE_256K_M  (CACHE_L1_ICACHE_CACHESIZE_256K_V << CACHE_L1_ICACHE_CACHESIZE_256K_S)
#define CACHE_L1_ICACHE_CACHESIZE_256K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_256K_S  10
/** CACHE_L1_ICACHE_CACHESIZE_512K : HRO; bitpos: [11]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 512k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_512K    (BIT(11))
#define CACHE_L1_ICACHE_CACHESIZE_512K_M  (CACHE_L1_ICACHE_CACHESIZE_512K_V << CACHE_L1_ICACHE_CACHESIZE_512K_S)
#define CACHE_L1_ICACHE_CACHESIZE_512K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_512K_S  11
/** CACHE_L1_ICACHE_CACHESIZE_1024K : HRO; bitpos: [12]; default: 0;
 *  The field is used to configure cachesize of L1-ICache as 1024k bytes. This field
 *  and all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_CACHESIZE_1024K    (BIT(12))
#define CACHE_L1_ICACHE_CACHESIZE_1024K_M  (CACHE_L1_ICACHE_CACHESIZE_1024K_V << CACHE_L1_ICACHE_CACHESIZE_1024K_S)
#define CACHE_L1_ICACHE_CACHESIZE_1024K_V  0x00000001U
#define CACHE_L1_ICACHE_CACHESIZE_1024K_S  12

/** CACHE_L1_ICACHE_BLOCKSIZE_CONF_REG register
 *  L1 instruction Cache BlockSize mode configure register
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_CONF_REG (DR_REG_CACHE_BASE + 0x14)
/** CACHE_L1_ICACHE_BLOCKSIZE_8 : HRO; bitpos: [0]; default: 0;
 *  The field is used to configureblocksize of L1-ICache as 8 bytes. This field and all
 *  other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_8    (BIT(0))
#define CACHE_L1_ICACHE_BLOCKSIZE_8_M  (CACHE_L1_ICACHE_BLOCKSIZE_8_V << CACHE_L1_ICACHE_BLOCKSIZE_8_S)
#define CACHE_L1_ICACHE_BLOCKSIZE_8_V  0x00000001U
#define CACHE_L1_ICACHE_BLOCKSIZE_8_S  0
/** CACHE_L1_ICACHE_BLOCKSIZE_16 : HRO; bitpos: [1]; default: 0;
 *  The field is used to configureblocksize of L1-ICache as 16 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_16    (BIT(1))
#define CACHE_L1_ICACHE_BLOCKSIZE_16_M  (CACHE_L1_ICACHE_BLOCKSIZE_16_V << CACHE_L1_ICACHE_BLOCKSIZE_16_S)
#define CACHE_L1_ICACHE_BLOCKSIZE_16_V  0x00000001U
#define CACHE_L1_ICACHE_BLOCKSIZE_16_S  1
/** CACHE_L1_ICACHE_BLOCKSIZE_32 : HRO; bitpos: [2]; default: 0;
 *  The field is used to configureblocksize of L1-ICache as 32 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_32    (BIT(2))
#define CACHE_L1_ICACHE_BLOCKSIZE_32_M  (CACHE_L1_ICACHE_BLOCKSIZE_32_V << CACHE_L1_ICACHE_BLOCKSIZE_32_S)
#define CACHE_L1_ICACHE_BLOCKSIZE_32_V  0x00000001U
#define CACHE_L1_ICACHE_BLOCKSIZE_32_S  2
/** CACHE_L1_ICACHE_BLOCKSIZE_64 : HRO; bitpos: [3]; default: 1;
 *  The field is used to configureblocksize of L1-ICache as 64 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_64    (BIT(3))
#define CACHE_L1_ICACHE_BLOCKSIZE_64_M  (CACHE_L1_ICACHE_BLOCKSIZE_64_V << CACHE_L1_ICACHE_BLOCKSIZE_64_S)
#define CACHE_L1_ICACHE_BLOCKSIZE_64_V  0x00000001U
#define CACHE_L1_ICACHE_BLOCKSIZE_64_S  3
/** CACHE_L1_ICACHE_BLOCKSIZE_128 : HRO; bitpos: [4]; default: 0;
 *  The field is used to configureblocksize of L1-ICache as 128 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_128    (BIT(4))
#define CACHE_L1_ICACHE_BLOCKSIZE_128_M  (CACHE_L1_ICACHE_BLOCKSIZE_128_V << CACHE_L1_ICACHE_BLOCKSIZE_128_S)
#define CACHE_L1_ICACHE_BLOCKSIZE_128_V  0x00000001U
#define CACHE_L1_ICACHE_BLOCKSIZE_128_S  4
/** CACHE_L1_ICACHE_BLOCKSIZE_256 : HRO; bitpos: [5]; default: 0;
 *  The field is used to configureblocksize of L1-ICache as 256 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_ICACHE_BLOCKSIZE_256    (BIT(5))
#define CACHE_L1_ICACHE_BLOCKSIZE_256_M  (CACHE_L1_ICACHE_BLOCKSIZE_256_V << CACHE_L1_ICACHE_BLOCKSIZE_256_S)
#define CACHE_L1_ICACHE_BLOCKSIZE_256_V  0x00000001U
#define CACHE_L1_ICACHE_BLOCKSIZE_256_S  5

/** CACHE_L1_DCACHE_CACHESIZE_CONF_REG register
 *  L1 data Cache CacheSize mode configure register
 */
#define CACHE_L1_DCACHE_CACHESIZE_CONF_REG (DR_REG_CACHE_BASE + 0x18)
/** CACHE_L1_DCACHE_CACHESIZE_256 : HRO; bitpos: [0]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 256 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_256    (BIT(0))
#define CACHE_L1_DCACHE_CACHESIZE_256_M  (CACHE_L1_DCACHE_CACHESIZE_256_V << CACHE_L1_DCACHE_CACHESIZE_256_S)
#define CACHE_L1_DCACHE_CACHESIZE_256_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_256_S  0
/** CACHE_L1_DCACHE_CACHESIZE_512 : HRO; bitpos: [1]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 512 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_512    (BIT(1))
#define CACHE_L1_DCACHE_CACHESIZE_512_M  (CACHE_L1_DCACHE_CACHESIZE_512_V << CACHE_L1_DCACHE_CACHESIZE_512_S)
#define CACHE_L1_DCACHE_CACHESIZE_512_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_512_S  1
/** CACHE_L1_DCACHE_CACHESIZE_1K : HRO; bitpos: [2]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 1k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_1K    (BIT(2))
#define CACHE_L1_DCACHE_CACHESIZE_1K_M  (CACHE_L1_DCACHE_CACHESIZE_1K_V << CACHE_L1_DCACHE_CACHESIZE_1K_S)
#define CACHE_L1_DCACHE_CACHESIZE_1K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_1K_S  2
/** CACHE_L1_DCACHE_CACHESIZE_2K : HRO; bitpos: [3]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 2k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_2K    (BIT(3))
#define CACHE_L1_DCACHE_CACHESIZE_2K_M  (CACHE_L1_DCACHE_CACHESIZE_2K_V << CACHE_L1_DCACHE_CACHESIZE_2K_S)
#define CACHE_L1_DCACHE_CACHESIZE_2K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_2K_S  3
/** CACHE_L1_DCACHE_CACHESIZE_4K : HRO; bitpos: [4]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 4k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_4K    (BIT(4))
#define CACHE_L1_DCACHE_CACHESIZE_4K_M  (CACHE_L1_DCACHE_CACHESIZE_4K_V << CACHE_L1_DCACHE_CACHESIZE_4K_S)
#define CACHE_L1_DCACHE_CACHESIZE_4K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_4K_S  4
/** CACHE_L1_DCACHE_CACHESIZE_8K : HRO; bitpos: [5]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 8k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_8K    (BIT(5))
#define CACHE_L1_DCACHE_CACHESIZE_8K_M  (CACHE_L1_DCACHE_CACHESIZE_8K_V << CACHE_L1_DCACHE_CACHESIZE_8K_S)
#define CACHE_L1_DCACHE_CACHESIZE_8K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_8K_S  5
/** CACHE_L1_DCACHE_CACHESIZE_16K : HRO; bitpos: [6]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 16k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_16K    (BIT(6))
#define CACHE_L1_DCACHE_CACHESIZE_16K_M  (CACHE_L1_DCACHE_CACHESIZE_16K_V << CACHE_L1_DCACHE_CACHESIZE_16K_S)
#define CACHE_L1_DCACHE_CACHESIZE_16K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_16K_S  6
/** CACHE_L1_DCACHE_CACHESIZE_32K : HRO; bitpos: [7]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 32k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_32K    (BIT(7))
#define CACHE_L1_DCACHE_CACHESIZE_32K_M  (CACHE_L1_DCACHE_CACHESIZE_32K_V << CACHE_L1_DCACHE_CACHESIZE_32K_S)
#define CACHE_L1_DCACHE_CACHESIZE_32K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_32K_S  7
/** CACHE_L1_DCACHE_CACHESIZE_64K : HRO; bitpos: [8]; default: 1;
 *  The field is used to configure cachesize of L1-DCache as 64k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_64K    (BIT(8))
#define CACHE_L1_DCACHE_CACHESIZE_64K_M  (CACHE_L1_DCACHE_CACHESIZE_64K_V << CACHE_L1_DCACHE_CACHESIZE_64K_S)
#define CACHE_L1_DCACHE_CACHESIZE_64K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_64K_S  8
/** CACHE_L1_DCACHE_CACHESIZE_128K : HRO; bitpos: [9]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 128k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_128K    (BIT(9))
#define CACHE_L1_DCACHE_CACHESIZE_128K_M  (CACHE_L1_DCACHE_CACHESIZE_128K_V << CACHE_L1_DCACHE_CACHESIZE_128K_S)
#define CACHE_L1_DCACHE_CACHESIZE_128K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_128K_S  9
/** CACHE_L1_DCACHE_CACHESIZE_256K : HRO; bitpos: [10]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 256k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_256K    (BIT(10))
#define CACHE_L1_DCACHE_CACHESIZE_256K_M  (CACHE_L1_DCACHE_CACHESIZE_256K_V << CACHE_L1_DCACHE_CACHESIZE_256K_S)
#define CACHE_L1_DCACHE_CACHESIZE_256K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_256K_S  10
/** CACHE_L1_DCACHE_CACHESIZE_512K : HRO; bitpos: [11]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 512k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_512K    (BIT(11))
#define CACHE_L1_DCACHE_CACHESIZE_512K_M  (CACHE_L1_DCACHE_CACHESIZE_512K_V << CACHE_L1_DCACHE_CACHESIZE_512K_S)
#define CACHE_L1_DCACHE_CACHESIZE_512K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_512K_S  11
/** CACHE_L1_DCACHE_CACHESIZE_1024K : HRO; bitpos: [12]; default: 0;
 *  The field is used to configure cachesize of L1-DCache as 1024k bytes. This field
 *  and all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_CACHESIZE_1024K    (BIT(12))
#define CACHE_L1_DCACHE_CACHESIZE_1024K_M  (CACHE_L1_DCACHE_CACHESIZE_1024K_V << CACHE_L1_DCACHE_CACHESIZE_1024K_S)
#define CACHE_L1_DCACHE_CACHESIZE_1024K_V  0x00000001U
#define CACHE_L1_DCACHE_CACHESIZE_1024K_S  12

/** CACHE_L1_DCACHE_BLOCKSIZE_CONF_REG register
 *  L1 data Cache BlockSize mode configure register
 */
#define CACHE_L1_DCACHE_BLOCKSIZE_CONF_REG (DR_REG_CACHE_BASE + 0x1c)
/** CACHE_L1_DCACHE_BLOCKSIZE_8 : HRO; bitpos: [0]; default: 0;
 *  The field is used to configureblocksize of L1-DCache as 8 bytes. This field and all
 *  other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_BLOCKSIZE_8    (BIT(0))
#define CACHE_L1_DCACHE_BLOCKSIZE_8_M  (CACHE_L1_DCACHE_BLOCKSIZE_8_V << CACHE_L1_DCACHE_BLOCKSIZE_8_S)
#define CACHE_L1_DCACHE_BLOCKSIZE_8_V  0x00000001U
#define CACHE_L1_DCACHE_BLOCKSIZE_8_S  0
/** CACHE_L1_DCACHE_BLOCKSIZE_16 : HRO; bitpos: [1]; default: 0;
 *  The field is used to configureblocksize of L1-DCache as 16 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_BLOCKSIZE_16    (BIT(1))
#define CACHE_L1_DCACHE_BLOCKSIZE_16_M  (CACHE_L1_DCACHE_BLOCKSIZE_16_V << CACHE_L1_DCACHE_BLOCKSIZE_16_S)
#define CACHE_L1_DCACHE_BLOCKSIZE_16_V  0x00000001U
#define CACHE_L1_DCACHE_BLOCKSIZE_16_S  1
/** CACHE_L1_DCACHE_BLOCKSIZE_32 : HRO; bitpos: [2]; default: 0;
 *  The field is used to configureblocksize of L1-DCache as 32 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_BLOCKSIZE_32    (BIT(2))
#define CACHE_L1_DCACHE_BLOCKSIZE_32_M  (CACHE_L1_DCACHE_BLOCKSIZE_32_V << CACHE_L1_DCACHE_BLOCKSIZE_32_S)
#define CACHE_L1_DCACHE_BLOCKSIZE_32_V  0x00000001U
#define CACHE_L1_DCACHE_BLOCKSIZE_32_S  2
/** CACHE_L1_DCACHE_BLOCKSIZE_64 : HRO; bitpos: [3]; default: 1;
 *  The field is used to configureblocksize of L1-DCache as 64 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_BLOCKSIZE_64    (BIT(3))
#define CACHE_L1_DCACHE_BLOCKSIZE_64_M  (CACHE_L1_DCACHE_BLOCKSIZE_64_V << CACHE_L1_DCACHE_BLOCKSIZE_64_S)
#define CACHE_L1_DCACHE_BLOCKSIZE_64_V  0x00000001U
#define CACHE_L1_DCACHE_BLOCKSIZE_64_S  3
/** CACHE_L1_DCACHE_BLOCKSIZE_128 : HRO; bitpos: [4]; default: 0;
 *  The field is used to configureblocksize of L1-DCache as 128 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_BLOCKSIZE_128    (BIT(4))
#define CACHE_L1_DCACHE_BLOCKSIZE_128_M  (CACHE_L1_DCACHE_BLOCKSIZE_128_V << CACHE_L1_DCACHE_BLOCKSIZE_128_S)
#define CACHE_L1_DCACHE_BLOCKSIZE_128_V  0x00000001U
#define CACHE_L1_DCACHE_BLOCKSIZE_128_S  4
/** CACHE_L1_DCACHE_BLOCKSIZE_256 : HRO; bitpos: [5]; default: 0;
 *  The field is used to configureblocksize of L1-DCache as 256 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L1_DCACHE_BLOCKSIZE_256    (BIT(5))
#define CACHE_L1_DCACHE_BLOCKSIZE_256_M  (CACHE_L1_DCACHE_BLOCKSIZE_256_V << CACHE_L1_DCACHE_BLOCKSIZE_256_S)
#define CACHE_L1_DCACHE_BLOCKSIZE_256_V  0x00000001U
#define CACHE_L1_DCACHE_BLOCKSIZE_256_S  5

/** CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG register
 *  Cache wrap around control register
 */
#define CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG (DR_REG_CACHE_BASE + 0x20)
/** CACHE_L1_ICACHE0_WRAP : R/W; bitpos: [0]; default: 0;
 *  Set this bit as 1 to enable L1-ICache0 wrap around mode.
 */
#define CACHE_L1_ICACHE0_WRAP    (BIT(0))
#define CACHE_L1_ICACHE0_WRAP_M  (CACHE_L1_ICACHE0_WRAP_V << CACHE_L1_ICACHE0_WRAP_S)
#define CACHE_L1_ICACHE0_WRAP_V  0x00000001U
#define CACHE_L1_ICACHE0_WRAP_S  0
/** CACHE_L1_ICACHE1_WRAP : R/W; bitpos: [1]; default: 0;
 *  Set this bit as 1 to enable L1-ICache1 wrap around mode.
 */
#define CACHE_L1_ICACHE1_WRAP    (BIT(1))
#define CACHE_L1_ICACHE1_WRAP_M  (CACHE_L1_ICACHE1_WRAP_V << CACHE_L1_ICACHE1_WRAP_S)
#define CACHE_L1_ICACHE1_WRAP_V  0x00000001U
#define CACHE_L1_ICACHE1_WRAP_S  1
/** CACHE_L1_ICACHE2_WRAP : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_WRAP    (BIT(2))
#define CACHE_L1_ICACHE2_WRAP_M  (CACHE_L1_ICACHE2_WRAP_V << CACHE_L1_ICACHE2_WRAP_S)
#define CACHE_L1_ICACHE2_WRAP_V  0x00000001U
#define CACHE_L1_ICACHE2_WRAP_S  2
/** CACHE_L1_ICACHE3_WRAP : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_WRAP    (BIT(3))
#define CACHE_L1_ICACHE3_WRAP_M  (CACHE_L1_ICACHE3_WRAP_V << CACHE_L1_ICACHE3_WRAP_S)
#define CACHE_L1_ICACHE3_WRAP_V  0x00000001U
#define CACHE_L1_ICACHE3_WRAP_S  3
/** CACHE_L1_DCACHE_WRAP : R/W; bitpos: [4]; default: 0;
 *  Set this bit as 1 to enable L1-DCache wrap around mode.
 */
#define CACHE_L1_DCACHE_WRAP    (BIT(4))
#define CACHE_L1_DCACHE_WRAP_M  (CACHE_L1_DCACHE_WRAP_V << CACHE_L1_DCACHE_WRAP_S)
#define CACHE_L1_DCACHE_WRAP_V  0x00000001U
#define CACHE_L1_DCACHE_WRAP_S  4

/** CACHE_L1_CACHE_TAG_MEM_POWER_CTRL_REG register
 *  Cache tag memory power control register
 */
#define CACHE_L1_CACHE_TAG_MEM_POWER_CTRL_REG (DR_REG_CACHE_BASE + 0x24)
/** CACHE_L1_ICACHE0_TAG_MEM_FORCE_ON : R/W; bitpos: [0]; default: 1;
 *  The bit is used to close clock gating of  L1-ICache0 tag memory. 1: close gating,
 *  0: open clock gating.
 */
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_ON    (BIT(0))
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_ON_M  (CACHE_L1_ICACHE0_TAG_MEM_FORCE_ON_V << CACHE_L1_ICACHE0_TAG_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_ON_S  0
/** CACHE_L1_ICACHE0_TAG_MEM_FORCE_PD : R/W; bitpos: [1]; default: 0;
 *  The bit is used to power L1-ICache0 tag memory down. 0: follow rtc_lslp, 1: power
 *  down
 */
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PD    (BIT(1))
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PD_M  (CACHE_L1_ICACHE0_TAG_MEM_FORCE_PD_V << CACHE_L1_ICACHE0_TAG_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PD_S  1
/** CACHE_L1_ICACHE0_TAG_MEM_FORCE_PU : R/W; bitpos: [2]; default: 1;
 *  The bit is used to power L1-ICache0 tag memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PU    (BIT(2))
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PU_M  (CACHE_L1_ICACHE0_TAG_MEM_FORCE_PU_V << CACHE_L1_ICACHE0_TAG_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE0_TAG_MEM_FORCE_PU_S  2
/** CACHE_L1_ICACHE1_TAG_MEM_FORCE_ON : R/W; bitpos: [4]; default: 1;
 *  The bit is used to close clock gating of  L1-ICache1 tag memory. 1: close gating,
 *  0: open clock gating.
 */
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_ON    (BIT(4))
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_ON_M  (CACHE_L1_ICACHE1_TAG_MEM_FORCE_ON_V << CACHE_L1_ICACHE1_TAG_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_ON_S  4
/** CACHE_L1_ICACHE1_TAG_MEM_FORCE_PD : R/W; bitpos: [5]; default: 0;
 *  The bit is used to power L1-ICache1 tag memory down. 0: follow rtc_lslp, 1: power
 *  down
 */
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PD    (BIT(5))
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PD_M  (CACHE_L1_ICACHE1_TAG_MEM_FORCE_PD_V << CACHE_L1_ICACHE1_TAG_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PD_S  5
/** CACHE_L1_ICACHE1_TAG_MEM_FORCE_PU : R/W; bitpos: [6]; default: 1;
 *  The bit is used to power L1-ICache1 tag memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PU    (BIT(6))
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PU_M  (CACHE_L1_ICACHE1_TAG_MEM_FORCE_PU_V << CACHE_L1_ICACHE1_TAG_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE1_TAG_MEM_FORCE_PU_S  6
/** CACHE_L1_ICACHE2_TAG_MEM_FORCE_ON : HRO; bitpos: [8]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_ON    (BIT(8))
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_ON_M  (CACHE_L1_ICACHE2_TAG_MEM_FORCE_ON_V << CACHE_L1_ICACHE2_TAG_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_ON_S  8
/** CACHE_L1_ICACHE2_TAG_MEM_FORCE_PD : HRO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PD    (BIT(9))
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PD_M  (CACHE_L1_ICACHE2_TAG_MEM_FORCE_PD_V << CACHE_L1_ICACHE2_TAG_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PD_S  9
/** CACHE_L1_ICACHE2_TAG_MEM_FORCE_PU : HRO; bitpos: [10]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PU    (BIT(10))
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PU_M  (CACHE_L1_ICACHE2_TAG_MEM_FORCE_PU_V << CACHE_L1_ICACHE2_TAG_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_MEM_FORCE_PU_S  10
/** CACHE_L1_ICACHE3_TAG_MEM_FORCE_ON : HRO; bitpos: [12]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_ON    (BIT(12))
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_ON_M  (CACHE_L1_ICACHE3_TAG_MEM_FORCE_ON_V << CACHE_L1_ICACHE3_TAG_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_ON_S  12
/** CACHE_L1_ICACHE3_TAG_MEM_FORCE_PD : HRO; bitpos: [13]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PD    (BIT(13))
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PD_M  (CACHE_L1_ICACHE3_TAG_MEM_FORCE_PD_V << CACHE_L1_ICACHE3_TAG_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PD_S  13
/** CACHE_L1_ICACHE3_TAG_MEM_FORCE_PU : HRO; bitpos: [14]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PU    (BIT(14))
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PU_M  (CACHE_L1_ICACHE3_TAG_MEM_FORCE_PU_V << CACHE_L1_ICACHE3_TAG_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE3_TAG_MEM_FORCE_PU_S  14
/** CACHE_L1_DCACHE_TAG_MEM_FORCE_ON : R/W; bitpos: [16]; default: 1;
 *  The bit is used to close clock gating of  L1-DCache tag memory. 1: close gating, 0:
 *  open clock gating.
 */
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_ON    (BIT(16))
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_ON_M  (CACHE_L1_DCACHE_TAG_MEM_FORCE_ON_V << CACHE_L1_DCACHE_TAG_MEM_FORCE_ON_S)
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_ON_S  16
/** CACHE_L1_DCACHE_TAG_MEM_FORCE_PD : R/W; bitpos: [17]; default: 0;
 *  The bit is used to power L1-DCache tag memory down. 0: follow rtc_lslp, 1: power
 *  down
 */
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PD    (BIT(17))
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PD_M  (CACHE_L1_DCACHE_TAG_MEM_FORCE_PD_V << CACHE_L1_DCACHE_TAG_MEM_FORCE_PD_S)
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PD_S  17
/** CACHE_L1_DCACHE_TAG_MEM_FORCE_PU : R/W; bitpos: [18]; default: 1;
 *  The bit is used to power L1-DCache tag memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PU    (BIT(18))
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PU_M  (CACHE_L1_DCACHE_TAG_MEM_FORCE_PU_V << CACHE_L1_DCACHE_TAG_MEM_FORCE_PU_S)
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_DCACHE_TAG_MEM_FORCE_PU_S  18

/** CACHE_L1_CACHE_DATA_MEM_POWER_CTRL_REG register
 *  Cache data memory power control register
 */
#define CACHE_L1_CACHE_DATA_MEM_POWER_CTRL_REG (DR_REG_CACHE_BASE + 0x28)
/** CACHE_L1_ICACHE0_DATA_MEM_FORCE_ON : R/W; bitpos: [0]; default: 1;
 *  The bit is used to close clock gating of  L1-ICache0 data memory. 1: close gating,
 *  0: open clock gating.
 */
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_ON    (BIT(0))
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_ON_M  (CACHE_L1_ICACHE0_DATA_MEM_FORCE_ON_V << CACHE_L1_ICACHE0_DATA_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_ON_S  0
/** CACHE_L1_ICACHE0_DATA_MEM_FORCE_PD : R/W; bitpos: [1]; default: 0;
 *  The bit is used to power L1-ICache0 data memory down. 0: follow rtc_lslp, 1: power
 *  down
 */
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PD    (BIT(1))
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PD_M  (CACHE_L1_ICACHE0_DATA_MEM_FORCE_PD_V << CACHE_L1_ICACHE0_DATA_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PD_S  1
/** CACHE_L1_ICACHE0_DATA_MEM_FORCE_PU : R/W; bitpos: [2]; default: 1;
 *  The bit is used to power L1-ICache0 data memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PU    (BIT(2))
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PU_M  (CACHE_L1_ICACHE0_DATA_MEM_FORCE_PU_V << CACHE_L1_ICACHE0_DATA_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE0_DATA_MEM_FORCE_PU_S  2
/** CACHE_L1_ICACHE1_DATA_MEM_FORCE_ON : R/W; bitpos: [4]; default: 1;
 *  The bit is used to close clock gating of  L1-ICache1 data memory. 1: close gating,
 *  0: open clock gating.
 */
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_ON    (BIT(4))
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_ON_M  (CACHE_L1_ICACHE1_DATA_MEM_FORCE_ON_V << CACHE_L1_ICACHE1_DATA_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_ON_S  4
/** CACHE_L1_ICACHE1_DATA_MEM_FORCE_PD : R/W; bitpos: [5]; default: 0;
 *  The bit is used to power L1-ICache1 data memory down. 0: follow rtc_lslp, 1: power
 *  down
 */
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PD    (BIT(5))
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PD_M  (CACHE_L1_ICACHE1_DATA_MEM_FORCE_PD_V << CACHE_L1_ICACHE1_DATA_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PD_S  5
/** CACHE_L1_ICACHE1_DATA_MEM_FORCE_PU : R/W; bitpos: [6]; default: 1;
 *  The bit is used to power L1-ICache1 data memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PU    (BIT(6))
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PU_M  (CACHE_L1_ICACHE1_DATA_MEM_FORCE_PU_V << CACHE_L1_ICACHE1_DATA_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE1_DATA_MEM_FORCE_PU_S  6
/** CACHE_L1_ICACHE2_DATA_MEM_FORCE_ON : HRO; bitpos: [8]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_ON    (BIT(8))
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_ON_M  (CACHE_L1_ICACHE2_DATA_MEM_FORCE_ON_V << CACHE_L1_ICACHE2_DATA_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_ON_S  8
/** CACHE_L1_ICACHE2_DATA_MEM_FORCE_PD : HRO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PD    (BIT(9))
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PD_M  (CACHE_L1_ICACHE2_DATA_MEM_FORCE_PD_V << CACHE_L1_ICACHE2_DATA_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PD_S  9
/** CACHE_L1_ICACHE2_DATA_MEM_FORCE_PU : HRO; bitpos: [10]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PU    (BIT(10))
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PU_M  (CACHE_L1_ICACHE2_DATA_MEM_FORCE_PU_V << CACHE_L1_ICACHE2_DATA_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE2_DATA_MEM_FORCE_PU_S  10
/** CACHE_L1_ICACHE3_DATA_MEM_FORCE_ON : HRO; bitpos: [12]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_ON    (BIT(12))
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_ON_M  (CACHE_L1_ICACHE3_DATA_MEM_FORCE_ON_V << CACHE_L1_ICACHE3_DATA_MEM_FORCE_ON_S)
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_ON_S  12
/** CACHE_L1_ICACHE3_DATA_MEM_FORCE_PD : HRO; bitpos: [13]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PD    (BIT(13))
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PD_M  (CACHE_L1_ICACHE3_DATA_MEM_FORCE_PD_V << CACHE_L1_ICACHE3_DATA_MEM_FORCE_PD_S)
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PD_S  13
/** CACHE_L1_ICACHE3_DATA_MEM_FORCE_PU : HRO; bitpos: [14]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PU    (BIT(14))
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PU_M  (CACHE_L1_ICACHE3_DATA_MEM_FORCE_PU_V << CACHE_L1_ICACHE3_DATA_MEM_FORCE_PU_S)
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_ICACHE3_DATA_MEM_FORCE_PU_S  14
/** CACHE_L1_DCACHE_DATA_MEM_FORCE_ON : R/W; bitpos: [16]; default: 1;
 *  The bit is used to close clock gating of  L1-DCache data memory. 1: close gating,
 *  0: open clock gating.
 */
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_ON    (BIT(16))
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_ON_M  (CACHE_L1_DCACHE_DATA_MEM_FORCE_ON_V << CACHE_L1_DCACHE_DATA_MEM_FORCE_ON_S)
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_ON_S  16
/** CACHE_L1_DCACHE_DATA_MEM_FORCE_PD : R/W; bitpos: [17]; default: 0;
 *  The bit is used to power L1-DCache data memory down. 0: follow rtc_lslp, 1: power
 *  down
 */
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PD    (BIT(17))
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PD_M  (CACHE_L1_DCACHE_DATA_MEM_FORCE_PD_V << CACHE_L1_DCACHE_DATA_MEM_FORCE_PD_S)
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PD_S  17
/** CACHE_L1_DCACHE_DATA_MEM_FORCE_PU : R/W; bitpos: [18]; default: 1;
 *  The bit is used to power L1-DCache data memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PU    (BIT(18))
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PU_M  (CACHE_L1_DCACHE_DATA_MEM_FORCE_PU_V << CACHE_L1_DCACHE_DATA_MEM_FORCE_PU_S)
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L1_DCACHE_DATA_MEM_FORCE_PU_S  18

/** CACHE_L1_CACHE_FREEZE_CTRL_REG register
 *  Cache Freeze control register
 */
#define CACHE_L1_CACHE_FREEZE_CTRL_REG (DR_REG_CACHE_BASE + 0x2c)
/** CACHE_L1_ICACHE0_FREEZE_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable freeze operation on L1-ICache0. It can be cleared by
 *  software.
 */
#define CACHE_L1_ICACHE0_FREEZE_EN    (BIT(0))
#define CACHE_L1_ICACHE0_FREEZE_EN_M  (CACHE_L1_ICACHE0_FREEZE_EN_V << CACHE_L1_ICACHE0_FREEZE_EN_S)
#define CACHE_L1_ICACHE0_FREEZE_EN_V  0x00000001U
#define CACHE_L1_ICACHE0_FREEZE_EN_S  0
/** CACHE_L1_ICACHE0_FREEZE_MODE : R/W; bitpos: [1]; default: 0;
 *  The bit is used to configure mode of freeze operation L1-ICache0. 0: a miss-access
 *  will not stuck. 1: a miss-access will stuck.
 */
#define CACHE_L1_ICACHE0_FREEZE_MODE    (BIT(1))
#define CACHE_L1_ICACHE0_FREEZE_MODE_M  (CACHE_L1_ICACHE0_FREEZE_MODE_V << CACHE_L1_ICACHE0_FREEZE_MODE_S)
#define CACHE_L1_ICACHE0_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_ICACHE0_FREEZE_MODE_S  1
/** CACHE_L1_ICACHE0_FREEZE_DONE : RO; bitpos: [2]; default: 0;
 *  The bit is used to indicate whether freeze operation on L1-ICache0 is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_ICACHE0_FREEZE_DONE    (BIT(2))
#define CACHE_L1_ICACHE0_FREEZE_DONE_M  (CACHE_L1_ICACHE0_FREEZE_DONE_V << CACHE_L1_ICACHE0_FREEZE_DONE_S)
#define CACHE_L1_ICACHE0_FREEZE_DONE_V  0x00000001U
#define CACHE_L1_ICACHE0_FREEZE_DONE_S  2
/** CACHE_L1_ICACHE1_FREEZE_EN : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable freeze operation on L1-ICache1. It can be cleared by
 *  software.
 */
#define CACHE_L1_ICACHE1_FREEZE_EN    (BIT(4))
#define CACHE_L1_ICACHE1_FREEZE_EN_M  (CACHE_L1_ICACHE1_FREEZE_EN_V << CACHE_L1_ICACHE1_FREEZE_EN_S)
#define CACHE_L1_ICACHE1_FREEZE_EN_V  0x00000001U
#define CACHE_L1_ICACHE1_FREEZE_EN_S  4
/** CACHE_L1_ICACHE1_FREEZE_MODE : R/W; bitpos: [5]; default: 0;
 *  The bit is used to configure mode of freeze operation L1-ICache1. 0: a miss-access
 *  will not stuck. 1: a miss-access will stuck.
 */
#define CACHE_L1_ICACHE1_FREEZE_MODE    (BIT(5))
#define CACHE_L1_ICACHE1_FREEZE_MODE_M  (CACHE_L1_ICACHE1_FREEZE_MODE_V << CACHE_L1_ICACHE1_FREEZE_MODE_S)
#define CACHE_L1_ICACHE1_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_ICACHE1_FREEZE_MODE_S  5
/** CACHE_L1_ICACHE1_FREEZE_DONE : RO; bitpos: [6]; default: 0;
 *  The bit is used to indicate whether freeze operation on L1-ICache1 is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_ICACHE1_FREEZE_DONE    (BIT(6))
#define CACHE_L1_ICACHE1_FREEZE_DONE_M  (CACHE_L1_ICACHE1_FREEZE_DONE_V << CACHE_L1_ICACHE1_FREEZE_DONE_S)
#define CACHE_L1_ICACHE1_FREEZE_DONE_V  0x00000001U
#define CACHE_L1_ICACHE1_FREEZE_DONE_S  6
/** CACHE_L1_ICACHE2_FREEZE_EN : HRO; bitpos: [8]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FREEZE_EN    (BIT(8))
#define CACHE_L1_ICACHE2_FREEZE_EN_M  (CACHE_L1_ICACHE2_FREEZE_EN_V << CACHE_L1_ICACHE2_FREEZE_EN_S)
#define CACHE_L1_ICACHE2_FREEZE_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_FREEZE_EN_S  8
/** CACHE_L1_ICACHE2_FREEZE_MODE : HRO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FREEZE_MODE    (BIT(9))
#define CACHE_L1_ICACHE2_FREEZE_MODE_M  (CACHE_L1_ICACHE2_FREEZE_MODE_V << CACHE_L1_ICACHE2_FREEZE_MODE_S)
#define CACHE_L1_ICACHE2_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_ICACHE2_FREEZE_MODE_S  9
/** CACHE_L1_ICACHE2_FREEZE_DONE : RO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FREEZE_DONE    (BIT(10))
#define CACHE_L1_ICACHE2_FREEZE_DONE_M  (CACHE_L1_ICACHE2_FREEZE_DONE_V << CACHE_L1_ICACHE2_FREEZE_DONE_S)
#define CACHE_L1_ICACHE2_FREEZE_DONE_V  0x00000001U
#define CACHE_L1_ICACHE2_FREEZE_DONE_S  10
/** CACHE_L1_ICACHE3_FREEZE_EN : HRO; bitpos: [12]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_FREEZE_EN    (BIT(12))
#define CACHE_L1_ICACHE3_FREEZE_EN_M  (CACHE_L1_ICACHE3_FREEZE_EN_V << CACHE_L1_ICACHE3_FREEZE_EN_S)
#define CACHE_L1_ICACHE3_FREEZE_EN_V  0x00000001U
#define CACHE_L1_ICACHE3_FREEZE_EN_S  12
/** CACHE_L1_ICACHE3_FREEZE_MODE : HRO; bitpos: [13]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_FREEZE_MODE    (BIT(13))
#define CACHE_L1_ICACHE3_FREEZE_MODE_M  (CACHE_L1_ICACHE3_FREEZE_MODE_V << CACHE_L1_ICACHE3_FREEZE_MODE_S)
#define CACHE_L1_ICACHE3_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_ICACHE3_FREEZE_MODE_S  13
/** CACHE_L1_ICACHE3_FREEZE_DONE : RO; bitpos: [14]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_FREEZE_DONE    (BIT(14))
#define CACHE_L1_ICACHE3_FREEZE_DONE_M  (CACHE_L1_ICACHE3_FREEZE_DONE_V << CACHE_L1_ICACHE3_FREEZE_DONE_S)
#define CACHE_L1_ICACHE3_FREEZE_DONE_V  0x00000001U
#define CACHE_L1_ICACHE3_FREEZE_DONE_S  14
/** CACHE_L1_DCACHE_FREEZE_EN : R/W; bitpos: [16]; default: 0;
 *  The bit is used to enable freeze operation on L1-DCache. It can be cleared by
 *  software.
 */
#define CACHE_L1_DCACHE_FREEZE_EN    (BIT(16))
#define CACHE_L1_DCACHE_FREEZE_EN_M  (CACHE_L1_DCACHE_FREEZE_EN_V << CACHE_L1_DCACHE_FREEZE_EN_S)
#define CACHE_L1_DCACHE_FREEZE_EN_V  0x00000001U
#define CACHE_L1_DCACHE_FREEZE_EN_S  16
/** CACHE_L1_DCACHE_FREEZE_MODE : R/W; bitpos: [17]; default: 0;
 *  The bit is used to configure mode of freeze operation L1-DCache. 0: a miss-access
 *  will not stuck. 1: a miss-access will stuck.
 */
#define CACHE_L1_DCACHE_FREEZE_MODE    (BIT(17))
#define CACHE_L1_DCACHE_FREEZE_MODE_M  (CACHE_L1_DCACHE_FREEZE_MODE_V << CACHE_L1_DCACHE_FREEZE_MODE_S)
#define CACHE_L1_DCACHE_FREEZE_MODE_V  0x00000001U
#define CACHE_L1_DCACHE_FREEZE_MODE_S  17
/** CACHE_L1_DCACHE_FREEZE_DONE : RO; bitpos: [18]; default: 0;
 *  The bit is used to indicate whether freeze operation on L1-DCache is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_DCACHE_FREEZE_DONE    (BIT(18))
#define CACHE_L1_DCACHE_FREEZE_DONE_M  (CACHE_L1_DCACHE_FREEZE_DONE_V << CACHE_L1_DCACHE_FREEZE_DONE_S)
#define CACHE_L1_DCACHE_FREEZE_DONE_V  0x00000001U
#define CACHE_L1_DCACHE_FREEZE_DONE_S  18

/** CACHE_L1_CACHE_DATA_MEM_ACS_CONF_REG register
 *  Cache data memory access configure register
 */
#define CACHE_L1_CACHE_DATA_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x30)
/** CACHE_L1_ICACHE0_DATA_MEM_RD_EN : R/W; bitpos: [0]; default: 1;
 *  The bit is used to enable config-bus read L1-ICache0 data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_ICACHE0_DATA_MEM_RD_EN    (BIT(0))
#define CACHE_L1_ICACHE0_DATA_MEM_RD_EN_M  (CACHE_L1_ICACHE0_DATA_MEM_RD_EN_V << CACHE_L1_ICACHE0_DATA_MEM_RD_EN_S)
#define CACHE_L1_ICACHE0_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE0_DATA_MEM_RD_EN_S  0
/** CACHE_L1_ICACHE0_DATA_MEM_WR_EN : R/W; bitpos: [1]; default: 1;
 *  The bit is used to enable config-bus write L1-ICache0 data memoryory. 0: disable,
 *  1: enable.
 */
#define CACHE_L1_ICACHE0_DATA_MEM_WR_EN    (BIT(1))
#define CACHE_L1_ICACHE0_DATA_MEM_WR_EN_M  (CACHE_L1_ICACHE0_DATA_MEM_WR_EN_V << CACHE_L1_ICACHE0_DATA_MEM_WR_EN_S)
#define CACHE_L1_ICACHE0_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE0_DATA_MEM_WR_EN_S  1
/** CACHE_L1_ICACHE1_DATA_MEM_RD_EN : R/W; bitpos: [4]; default: 1;
 *  The bit is used to enable config-bus read L1-ICache1 data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_ICACHE1_DATA_MEM_RD_EN    (BIT(4))
#define CACHE_L1_ICACHE1_DATA_MEM_RD_EN_M  (CACHE_L1_ICACHE1_DATA_MEM_RD_EN_V << CACHE_L1_ICACHE1_DATA_MEM_RD_EN_S)
#define CACHE_L1_ICACHE1_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE1_DATA_MEM_RD_EN_S  4
/** CACHE_L1_ICACHE1_DATA_MEM_WR_EN : R/W; bitpos: [5]; default: 1;
 *  The bit is used to enable config-bus write L1-ICache1 data memoryory. 0: disable,
 *  1: enable.
 */
#define CACHE_L1_ICACHE1_DATA_MEM_WR_EN    (BIT(5))
#define CACHE_L1_ICACHE1_DATA_MEM_WR_EN_M  (CACHE_L1_ICACHE1_DATA_MEM_WR_EN_V << CACHE_L1_ICACHE1_DATA_MEM_WR_EN_S)
#define CACHE_L1_ICACHE1_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE1_DATA_MEM_WR_EN_S  5
/** CACHE_L1_ICACHE2_DATA_MEM_RD_EN : HRO; bitpos: [8]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN    (BIT(8))
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN_M  (CACHE_L1_ICACHE2_DATA_MEM_RD_EN_V << CACHE_L1_ICACHE2_DATA_MEM_RD_EN_S)
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_DATA_MEM_RD_EN_S  8
/** CACHE_L1_ICACHE2_DATA_MEM_WR_EN : HRO; bitpos: [9]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN    (BIT(9))
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN_M  (CACHE_L1_ICACHE2_DATA_MEM_WR_EN_V << CACHE_L1_ICACHE2_DATA_MEM_WR_EN_S)
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_DATA_MEM_WR_EN_S  9
/** CACHE_L1_ICACHE3_DATA_MEM_RD_EN : HRO; bitpos: [12]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_DATA_MEM_RD_EN    (BIT(12))
#define CACHE_L1_ICACHE3_DATA_MEM_RD_EN_M  (CACHE_L1_ICACHE3_DATA_MEM_RD_EN_V << CACHE_L1_ICACHE3_DATA_MEM_RD_EN_S)
#define CACHE_L1_ICACHE3_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE3_DATA_MEM_RD_EN_S  12
/** CACHE_L1_ICACHE3_DATA_MEM_WR_EN : HRO; bitpos: [13]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_DATA_MEM_WR_EN    (BIT(13))
#define CACHE_L1_ICACHE3_DATA_MEM_WR_EN_M  (CACHE_L1_ICACHE3_DATA_MEM_WR_EN_V << CACHE_L1_ICACHE3_DATA_MEM_WR_EN_S)
#define CACHE_L1_ICACHE3_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE3_DATA_MEM_WR_EN_S  13
/** CACHE_L1_DCACHE_DATA_MEM_RD_EN : R/W; bitpos: [16]; default: 1;
 *  The bit is used to enable config-bus read L1-DCache data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_DCACHE_DATA_MEM_RD_EN    (BIT(16))
#define CACHE_L1_DCACHE_DATA_MEM_RD_EN_M  (CACHE_L1_DCACHE_DATA_MEM_RD_EN_V << CACHE_L1_DCACHE_DATA_MEM_RD_EN_S)
#define CACHE_L1_DCACHE_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_DCACHE_DATA_MEM_RD_EN_S  16
/** CACHE_L1_DCACHE_DATA_MEM_WR_EN : R/W; bitpos: [17]; default: 1;
 *  The bit is used to enable config-bus write L1-DCache data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_DCACHE_DATA_MEM_WR_EN    (BIT(17))
#define CACHE_L1_DCACHE_DATA_MEM_WR_EN_M  (CACHE_L1_DCACHE_DATA_MEM_WR_EN_V << CACHE_L1_DCACHE_DATA_MEM_WR_EN_S)
#define CACHE_L1_DCACHE_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_DCACHE_DATA_MEM_WR_EN_S  17

/** CACHE_L1_CACHE_TAG_MEM_ACS_CONF_REG register
 *  Cache tag memory access configure register
 */
#define CACHE_L1_CACHE_TAG_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x34)
/** CACHE_L1_ICACHE0_TAG_MEM_RD_EN : R/W; bitpos: [0]; default: 1;
 *  The bit is used to enable config-bus read L1-ICache0 tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_ICACHE0_TAG_MEM_RD_EN    (BIT(0))
#define CACHE_L1_ICACHE0_TAG_MEM_RD_EN_M  (CACHE_L1_ICACHE0_TAG_MEM_RD_EN_V << CACHE_L1_ICACHE0_TAG_MEM_RD_EN_S)
#define CACHE_L1_ICACHE0_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE0_TAG_MEM_RD_EN_S  0
/** CACHE_L1_ICACHE0_TAG_MEM_WR_EN : R/W; bitpos: [1]; default: 1;
 *  The bit is used to enable config-bus write L1-ICache0 tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_ICACHE0_TAG_MEM_WR_EN    (BIT(1))
#define CACHE_L1_ICACHE0_TAG_MEM_WR_EN_M  (CACHE_L1_ICACHE0_TAG_MEM_WR_EN_V << CACHE_L1_ICACHE0_TAG_MEM_WR_EN_S)
#define CACHE_L1_ICACHE0_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE0_TAG_MEM_WR_EN_S  1
/** CACHE_L1_ICACHE1_TAG_MEM_RD_EN : R/W; bitpos: [4]; default: 1;
 *  The bit is used to enable config-bus read L1-ICache1 tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_ICACHE1_TAG_MEM_RD_EN    (BIT(4))
#define CACHE_L1_ICACHE1_TAG_MEM_RD_EN_M  (CACHE_L1_ICACHE1_TAG_MEM_RD_EN_V << CACHE_L1_ICACHE1_TAG_MEM_RD_EN_S)
#define CACHE_L1_ICACHE1_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE1_TAG_MEM_RD_EN_S  4
/** CACHE_L1_ICACHE1_TAG_MEM_WR_EN : R/W; bitpos: [5]; default: 1;
 *  The bit is used to enable config-bus write L1-ICache1 tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_ICACHE1_TAG_MEM_WR_EN    (BIT(5))
#define CACHE_L1_ICACHE1_TAG_MEM_WR_EN_M  (CACHE_L1_ICACHE1_TAG_MEM_WR_EN_V << CACHE_L1_ICACHE1_TAG_MEM_WR_EN_S)
#define CACHE_L1_ICACHE1_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE1_TAG_MEM_WR_EN_S  5
/** CACHE_L1_ICACHE2_TAG_MEM_RD_EN : HRO; bitpos: [8]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN    (BIT(8))
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN_M  (CACHE_L1_ICACHE2_TAG_MEM_RD_EN_V << CACHE_L1_ICACHE2_TAG_MEM_RD_EN_S)
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_MEM_RD_EN_S  8
/** CACHE_L1_ICACHE2_TAG_MEM_WR_EN : HRO; bitpos: [9]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN    (BIT(9))
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN_M  (CACHE_L1_ICACHE2_TAG_MEM_WR_EN_V << CACHE_L1_ICACHE2_TAG_MEM_WR_EN_S)
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_MEM_WR_EN_S  9
/** CACHE_L1_ICACHE3_TAG_MEM_RD_EN : HRO; bitpos: [12]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_TAG_MEM_RD_EN    (BIT(12))
#define CACHE_L1_ICACHE3_TAG_MEM_RD_EN_M  (CACHE_L1_ICACHE3_TAG_MEM_RD_EN_V << CACHE_L1_ICACHE3_TAG_MEM_RD_EN_S)
#define CACHE_L1_ICACHE3_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_ICACHE3_TAG_MEM_RD_EN_S  12
/** CACHE_L1_ICACHE3_TAG_MEM_WR_EN : HRO; bitpos: [13]; default: 1;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_TAG_MEM_WR_EN    (BIT(13))
#define CACHE_L1_ICACHE3_TAG_MEM_WR_EN_M  (CACHE_L1_ICACHE3_TAG_MEM_WR_EN_V << CACHE_L1_ICACHE3_TAG_MEM_WR_EN_S)
#define CACHE_L1_ICACHE3_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_ICACHE3_TAG_MEM_WR_EN_S  13
/** CACHE_L1_DCACHE_TAG_MEM_RD_EN : R/W; bitpos: [16]; default: 1;
 *  The bit is used to enable config-bus read L1-DCache tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_DCACHE_TAG_MEM_RD_EN    (BIT(16))
#define CACHE_L1_DCACHE_TAG_MEM_RD_EN_M  (CACHE_L1_DCACHE_TAG_MEM_RD_EN_V << CACHE_L1_DCACHE_TAG_MEM_RD_EN_S)
#define CACHE_L1_DCACHE_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L1_DCACHE_TAG_MEM_RD_EN_S  16
/** CACHE_L1_DCACHE_TAG_MEM_WR_EN : R/W; bitpos: [17]; default: 1;
 *  The bit is used to enable config-bus write L1-DCache tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L1_DCACHE_TAG_MEM_WR_EN    (BIT(17))
#define CACHE_L1_DCACHE_TAG_MEM_WR_EN_M  (CACHE_L1_DCACHE_TAG_MEM_WR_EN_V << CACHE_L1_DCACHE_TAG_MEM_WR_EN_S)
#define CACHE_L1_DCACHE_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L1_DCACHE_TAG_MEM_WR_EN_S  17

/** CACHE_L1_ICACHE0_PRELOCK_CONF_REG register
 *  L1 instruction Cache 0 prelock configure register
 */
#define CACHE_L1_ICACHE0_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x38)
/** CACHE_L1_ICACHE0_PRELOCK_SCT0_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L1-ICache0.
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_EN_M  (CACHE_L1_ICACHE0_PRELOCK_SCT0_EN_V << CACHE_L1_ICACHE0_PRELOCK_SCT0_EN_S)
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_ICACHE0_PRELOCK_SCT1_EN : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L1-ICache0.
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_EN_M  (CACHE_L1_ICACHE0_PRELOCK_SCT1_EN_V << CACHE_L1_ICACHE0_PRELOCK_SCT1_EN_S)
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_EN_S  1
/** CACHE_L1_ICACHE0_PRELOCK_RGID : R/W; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l1 icache0 prelock.
 */
#define CACHE_L1_ICACHE0_PRELOCK_RGID    0x0000000FU
#define CACHE_L1_ICACHE0_PRELOCK_RGID_M  (CACHE_L1_ICACHE0_PRELOCK_RGID_V << CACHE_L1_ICACHE0_PRELOCK_RGID_S)
#define CACHE_L1_ICACHE0_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE0_PRELOCK_RGID_S  2

/** CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR_REG register
 *  L1 instruction Cache 0 prelock section0 address configure register
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x3c)
/** CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section of
 *  prelock on L1-ICache0, which should be used together with
 *  L1_ICACHE0_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR_M  (CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR_V << CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR_REG register
 *  L1 instruction Cache 0 prelock section1 address configure register
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x40)
/** CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section of
 *  prelock on L1-ICache0, which should be used together with
 *  L1_ICACHE0_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR_M  (CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR_V << CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE0_PRELOCK_SCT_SIZE_REG register
 *  L1 instruction Cache 0 prelock section size configure register
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x44)
/** CACHE_L1_ICACHE0_PRELOCK_SCT0_SIZE : R/W; bitpos: [13:0]; default: 16383;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_SIZE_M  (CACHE_L1_ICACHE0_PRELOCK_SCT0_SIZE_V << CACHE_L1_ICACHE0_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_ICACHE0_PRELOCK_SCT1_SIZE : R/W; bitpos: [29:16]; default: 16383;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_SIZE_M  (CACHE_L1_ICACHE0_PRELOCK_SCT1_SIZE_V << CACHE_L1_ICACHE0_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE0_PRELOCK_SCT1_SIZE_S  16

/** CACHE_L1_ICACHE1_PRELOCK_CONF_REG register
 *  L1 instruction Cache 1 prelock configure register
 */
#define CACHE_L1_ICACHE1_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x48)
/** CACHE_L1_ICACHE1_PRELOCK_SCT0_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L1-ICache1.
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_EN_M  (CACHE_L1_ICACHE1_PRELOCK_SCT0_EN_V << CACHE_L1_ICACHE1_PRELOCK_SCT0_EN_S)
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_ICACHE1_PRELOCK_SCT1_EN : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L1-ICache1.
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_EN_M  (CACHE_L1_ICACHE1_PRELOCK_SCT1_EN_V << CACHE_L1_ICACHE1_PRELOCK_SCT1_EN_S)
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_EN_S  1
/** CACHE_L1_ICACHE1_PRELOCK_RGID : R/W; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l1 icache1 prelock.
 */
#define CACHE_L1_ICACHE1_PRELOCK_RGID    0x0000000FU
#define CACHE_L1_ICACHE1_PRELOCK_RGID_M  (CACHE_L1_ICACHE1_PRELOCK_RGID_V << CACHE_L1_ICACHE1_PRELOCK_RGID_S)
#define CACHE_L1_ICACHE1_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE1_PRELOCK_RGID_S  2

/** CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR_REG register
 *  L1 instruction Cache 1 prelock section0 address configure register
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x4c)
/** CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section of
 *  prelock on L1-ICache1, which should be used together with
 *  L1_ICACHE1_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR_M  (CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR_V << CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR_REG register
 *  L1 instruction Cache 1 prelock section1 address configure register
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x50)
/** CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section of
 *  prelock on L1-ICache1, which should be used together with
 *  L1_ICACHE1_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR_M  (CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR_V << CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE1_PRELOCK_SCT_SIZE_REG register
 *  L1 instruction Cache 1 prelock section size configure register
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x54)
/** CACHE_L1_ICACHE1_PRELOCK_SCT0_SIZE : R/W; bitpos: [13:0]; default: 16383;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_SIZE_M  (CACHE_L1_ICACHE1_PRELOCK_SCT0_SIZE_V << CACHE_L1_ICACHE1_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_ICACHE1_PRELOCK_SCT1_SIZE : R/W; bitpos: [29:16]; default: 16383;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_SIZE_M  (CACHE_L1_ICACHE1_PRELOCK_SCT1_SIZE_V << CACHE_L1_ICACHE1_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE1_PRELOCK_SCT1_SIZE_S  16

/** CACHE_L1_ICACHE2_PRELOCK_CONF_REG register
 *  L1 instruction Cache 2 prelock configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x58)
/** CACHE_L1_ICACHE2_PRELOCK_SCT0_EN : HRO; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_M  (CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_V << CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_ICACHE2_PRELOCK_SCT1_EN : HRO; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_M  (CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_V << CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_EN_S  1
/** CACHE_L1_ICACHE2_PRELOCK_RGID : HRO; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l1 icache2 prelock.
 */
#define CACHE_L1_ICACHE2_PRELOCK_RGID    0x0000000FU
#define CACHE_L1_ICACHE2_PRELOCK_RGID_M  (CACHE_L1_ICACHE2_PRELOCK_RGID_V << CACHE_L1_ICACHE2_PRELOCK_RGID_S)
#define CACHE_L1_ICACHE2_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE2_PRELOCK_RGID_S  2

/** CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_REG register
 *  L1 instruction Cache 2 prelock section0 address configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x5c)
/** CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section of
 *  prelock on L1-ICache2, which should be used together with
 *  L1_ICACHE2_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_M  (CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_V << CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_REG register
 *  L1 instruction Cache 2 prelock section1 address configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x60)
/** CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section of
 *  prelock on L1-ICache2, which should be used together with
 *  L1_ICACHE2_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_M  (CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_V << CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE2_PRELOCK_SCT_SIZE_REG register
 *  L1 instruction Cache 2 prelock section size configure register
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x64)
/** CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE : HRO; bitpos: [13:0]; default: 16383;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_M  (CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_V << CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE : HRO; bitpos: [29:16]; default: 16383;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_M  (CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_V << CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOCK_SCT1_SIZE_S  16

/** CACHE_L1_ICACHE3_PRELOCK_CONF_REG register
 *  L1 instruction Cache 3 prelock configure register
 */
#define CACHE_L1_ICACHE3_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x68)
/** CACHE_L1_ICACHE3_PRELOCK_SCT0_EN : HRO; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L1-ICache3.
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_EN_M  (CACHE_L1_ICACHE3_PRELOCK_SCT0_EN_V << CACHE_L1_ICACHE3_PRELOCK_SCT0_EN_S)
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_ICACHE3_PRELOCK_SCT1_EN : HRO; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L1-ICache3.
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_EN_M  (CACHE_L1_ICACHE3_PRELOCK_SCT1_EN_V << CACHE_L1_ICACHE3_PRELOCK_SCT1_EN_S)
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_EN_S  1
/** CACHE_L1_ICACHE3_PRELOCK_RGID : HRO; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l1 icache3 prelock.
 */
#define CACHE_L1_ICACHE3_PRELOCK_RGID    0x0000000FU
#define CACHE_L1_ICACHE3_PRELOCK_RGID_M  (CACHE_L1_ICACHE3_PRELOCK_RGID_V << CACHE_L1_ICACHE3_PRELOCK_RGID_S)
#define CACHE_L1_ICACHE3_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE3_PRELOCK_RGID_S  2

/** CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR_REG register
 *  L1 instruction Cache 3 prelock section0 address configure register
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x6c)
/** CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section of
 *  prelock on L1-ICache3, which should be used together with
 *  L1_ICACHE3_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR_M  (CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR_V << CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR_REG register
 *  L1 instruction Cache 3 prelock section1 address configure register
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x70)
/** CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section of
 *  prelock on L1-ICache3, which should be used together with
 *  L1_ICACHE3_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR_M  (CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR_V << CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE3_PRELOCK_SCT_SIZE_REG register
 *  L1 instruction Cache 3 prelock section size configure register
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x74)
/** CACHE_L1_ICACHE3_PRELOCK_SCT0_SIZE : HRO; bitpos: [13:0]; default: 16383;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache3, which should be used together with L1_ICACHE3_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_SIZE_M  (CACHE_L1_ICACHE3_PRELOCK_SCT0_SIZE_V << CACHE_L1_ICACHE3_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_ICACHE3_PRELOCK_SCT1_SIZE : HRO; bitpos: [29:16]; default: 16383;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L1-ICache3, which should be used together with L1_ICACHE3_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_SIZE_M  (CACHE_L1_ICACHE3_PRELOCK_SCT1_SIZE_V << CACHE_L1_ICACHE3_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE3_PRELOCK_SCT1_SIZE_S  16

/** CACHE_L1_DCACHE_PRELOCK_CONF_REG register
 *  L1 data Cache prelock configure register
 */
#define CACHE_L1_DCACHE_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x78)
/** CACHE_L1_DCACHE_PRELOCK_SCT0_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L1-DCache.
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L1_DCACHE_PRELOCK_SCT0_EN_M  (CACHE_L1_DCACHE_PRELOCK_SCT0_EN_V << CACHE_L1_DCACHE_PRELOCK_SCT0_EN_S)
#define CACHE_L1_DCACHE_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L1_DCACHE_PRELOCK_SCT0_EN_S  0
/** CACHE_L1_DCACHE_PRELOCK_SCT1_EN : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L1-DCache.
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L1_DCACHE_PRELOCK_SCT1_EN_M  (CACHE_L1_DCACHE_PRELOCK_SCT1_EN_V << CACHE_L1_DCACHE_PRELOCK_SCT1_EN_S)
#define CACHE_L1_DCACHE_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L1_DCACHE_PRELOCK_SCT1_EN_S  1
/** CACHE_L1_DCACHE_PRELOCK_RGID : R/W; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l1 dcache prelock.
 */
#define CACHE_L1_DCACHE_PRELOCK_RGID    0x0000000FU
#define CACHE_L1_DCACHE_PRELOCK_RGID_M  (CACHE_L1_DCACHE_PRELOCK_RGID_V << CACHE_L1_DCACHE_PRELOCK_RGID_S)
#define CACHE_L1_DCACHE_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L1_DCACHE_PRELOCK_RGID_S  2

/** CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR_REG register
 *  L1 data Cache prelock section0 address configure register
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x7c)
/** CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section of
 *  prelock on L1-DCache, which should be used together with
 *  L1_DCACHE_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR_M  (CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR_V << CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR_S)
#define CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_REG register
 *  L1 data Cache prelock section1 address configure register
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x80)
/** CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section of
 *  prelock on L1-DCache, which should be used together with
 *  L1_DCACHE_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_M  (CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_V << CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_S)
#define CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L1_DCACHE_PRELOCK_SCT_SIZE_REG register
 *  L1 data Cache prelock section size configure register
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x84)
/** CACHE_L1_DCACHE_PRELOCK_SCT0_SIZE : R/W; bitpos: [13:0]; default: 16383;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-DCache, which should be used together with L1_DCACHE_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT0_SIZE    0x00003FFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT0_SIZE_M  (CACHE_L1_DCACHE_PRELOCK_SCT0_SIZE_V << CACHE_L1_DCACHE_PRELOCK_SCT0_SIZE_S)
#define CACHE_L1_DCACHE_PRELOCK_SCT0_SIZE_V  0x00003FFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L1_DCACHE_PRELOCK_SCT1_SIZE : R/W; bitpos: [29:16]; default: 16383;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L1-DCache, which should be used together with L1_DCACHE_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L1_DCACHE_PRELOCK_SCT1_SIZE    0x00003FFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT1_SIZE_M  (CACHE_L1_DCACHE_PRELOCK_SCT1_SIZE_V << CACHE_L1_DCACHE_PRELOCK_SCT1_SIZE_S)
#define CACHE_L1_DCACHE_PRELOCK_SCT1_SIZE_V  0x00003FFFU
#define CACHE_L1_DCACHE_PRELOCK_SCT1_SIZE_S  16

/** CACHE_LOCK_CTRL_REG register
 *  Lock-class (manual lock) operation control register
 */
#define CACHE_LOCK_CTRL_REG (DR_REG_CACHE_BASE + 0x88)
/** CACHE_LOCK_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable lock operation. It will be cleared by hardware after lock
 *  operation done. Note that (1) this bit and unlock_ena bit are mutually exclusive,
 *  that is, those bits can not be set to 1 at the same time. (2) lock operation can be
 *  applied on LL1-ICache, L1-DCache and L2-Cache.
 */
#define CACHE_LOCK_ENA    (BIT(0))
#define CACHE_LOCK_ENA_M  (CACHE_LOCK_ENA_V << CACHE_LOCK_ENA_S)
#define CACHE_LOCK_ENA_V  0x00000001U
#define CACHE_LOCK_ENA_S  0
/** CACHE_UNLOCK_ENA : R/W/SC; bitpos: [1]; default: 0;
 *  The bit is used to enable unlock operation. It will be cleared by hardware after
 *  unlock operation done. Note that (1) this bit and lock_ena bit are mutually
 *  exclusive, that is, those bits can not be set to 1 at the same time. (2) unlock
 *  operation can be applied on L1-ICache, L1-DCache and L2-Cache.
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
/** CACHE_LOCK_RGID : R/W; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of cache lock/unlock.
 */
#define CACHE_LOCK_RGID    0x0000000FU
#define CACHE_LOCK_RGID_M  (CACHE_LOCK_RGID_V << CACHE_LOCK_RGID_S)
#define CACHE_LOCK_RGID_V  0x0000000FU
#define CACHE_LOCK_RGID_S  3

/** CACHE_LOCK_MAP_REG register
 *  Lock (manual lock) map configure register
 */
#define CACHE_LOCK_MAP_REG (DR_REG_CACHE_BASE + 0x8c)
/** CACHE_LOCK_MAP : R/W; bitpos: [5:0]; default: 0;
 *  Those bits are used to indicate which caches in the two-level cache structure will
 *  apply this lock/unlock operation. [0]: L1-ICache0, [1]: L1-ICache1, [2]:
 *  L1-ICache2, [3]: L1-ICache3, [4]: L1-DCache, [5]: L2-Cache.
 */
#define CACHE_LOCK_MAP    0x0000003FU
#define CACHE_LOCK_MAP_M  (CACHE_LOCK_MAP_V << CACHE_LOCK_MAP_S)
#define CACHE_LOCK_MAP_V  0x0000003FU
#define CACHE_LOCK_MAP_S  0

/** CACHE_LOCK_ADDR_REG register
 *  Lock (manual lock) address configure register
 */
#define CACHE_LOCK_ADDR_REG (DR_REG_CACHE_BASE + 0x90)
/** CACHE_LOCK_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the lock/unlock
 *  operation, which should be used together with CACHE_LOCK_SIZE_REG
 */
#define CACHE_LOCK_ADDR    0xFFFFFFFFU
#define CACHE_LOCK_ADDR_M  (CACHE_LOCK_ADDR_V << CACHE_LOCK_ADDR_S)
#define CACHE_LOCK_ADDR_V  0xFFFFFFFFU
#define CACHE_LOCK_ADDR_S  0

/** CACHE_LOCK_SIZE_REG register
 *  Lock (manual lock) size configure register
 */
#define CACHE_LOCK_SIZE_REG (DR_REG_CACHE_BASE + 0x94)
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
#define CACHE_SYNC_CTRL_REG (DR_REG_CACHE_BASE + 0x98)
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
/** CACHE_SYNC_RGID : R/W; bitpos: [8:5]; default: 0;
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
#define CACHE_SYNC_MAP_REG (DR_REG_CACHE_BASE + 0x9c)
/** CACHE_SYNC_MAP : R/W; bitpos: [5:0]; default: 31;
 *  Those bits are used to indicate which caches in the two-level cache structure will
 *  apply the sync operation. [0]: L1-ICache0, [1]: L1-ICache1, [2]: L1-ICache2, [3]:
 *  L1-ICache3, [4]: L1-DCache, [5]: L2-Cache.
 */
#define CACHE_SYNC_MAP    0x0000003FU
#define CACHE_SYNC_MAP_M  (CACHE_SYNC_MAP_V << CACHE_SYNC_MAP_S)
#define CACHE_SYNC_MAP_V  0x0000003FU
#define CACHE_SYNC_MAP_S  0

/** CACHE_SYNC_ADDR_REG register
 *  Sync address configure register
 */
#define CACHE_SYNC_ADDR_REG (DR_REG_CACHE_BASE + 0xa0)
/** CACHE_SYNC_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the sync operation,
 *  which should be used together with CACHE_SYNC_SIZE_REG
 */
#define CACHE_SYNC_ADDR    0xFFFFFFFFU
#define CACHE_SYNC_ADDR_M  (CACHE_SYNC_ADDR_V << CACHE_SYNC_ADDR_S)
#define CACHE_SYNC_ADDR_V  0xFFFFFFFFU
#define CACHE_SYNC_ADDR_S  0

/** CACHE_SYNC_SIZE_REG register
 *  Sync size configure register
 */
#define CACHE_SYNC_SIZE_REG (DR_REG_CACHE_BASE + 0xa4)
/** CACHE_SYNC_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the sync operation, which should be
 *  used together with CACHE_SYNC_ADDR_REG
 */
#define CACHE_SYNC_SIZE    0x0FFFFFFFU
#define CACHE_SYNC_SIZE_M  (CACHE_SYNC_SIZE_V << CACHE_SYNC_SIZE_S)
#define CACHE_SYNC_SIZE_V  0x0FFFFFFFU
#define CACHE_SYNC_SIZE_S  0

/** CACHE_L1_ICACHE0_PRELOAD_CTRL_REG register
 *  L1 instruction Cache 0 preload-operation control register
 */
#define CACHE_L1_ICACHE0_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xa8)
/** CACHE_L1_ICACHE0_PRELOAD_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable preload operation on L1-ICache0. It will be cleared by
 *  hardware automatically after preload operation is done.
 */
#define CACHE_L1_ICACHE0_PRELOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE0_PRELOAD_ENA_M  (CACHE_L1_ICACHE0_PRELOAD_ENA_V << CACHE_L1_ICACHE0_PRELOAD_ENA_S)
#define CACHE_L1_ICACHE0_PRELOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE0_PRELOAD_ENA_S  0
/** CACHE_L1_ICACHE0_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether preload operation is finished or not. 0: not
 *  finished. 1: finished.
 */
#define CACHE_L1_ICACHE0_PRELOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE0_PRELOAD_DONE_M  (CACHE_L1_ICACHE0_PRELOAD_DONE_V << CACHE_L1_ICACHE0_PRELOAD_DONE_S)
#define CACHE_L1_ICACHE0_PRELOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE0_PRELOAD_DONE_S  1
/** CACHE_L1_ICACHE0_PRELOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L1_ICACHE0_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE0_PRELOAD_ORDER_M  (CACHE_L1_ICACHE0_PRELOAD_ORDER_V << CACHE_L1_ICACHE0_PRELOAD_ORDER_S)
#define CACHE_L1_ICACHE0_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE0_PRELOAD_ORDER_S  2
/** CACHE_L1_ICACHE0_PRELOAD_RGID : R/W; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l1 icache0 preload.
 */
#define CACHE_L1_ICACHE0_PRELOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE0_PRELOAD_RGID_M  (CACHE_L1_ICACHE0_PRELOAD_RGID_V << CACHE_L1_ICACHE0_PRELOAD_RGID_S)
#define CACHE_L1_ICACHE0_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE0_PRELOAD_RGID_S  3

/** CACHE_L1_ICACHE0_PRELOAD_ADDR_REG register
 *  L1 instruction Cache 0 preload address configure register
 */
#define CACHE_L1_ICACHE0_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xac)
/** CACHE_L1_ICACHE0_PRELOAD_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of preload on
 *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOAD_SIZE_REG
 */
#define CACHE_L1_ICACHE0_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE0_PRELOAD_ADDR_M  (CACHE_L1_ICACHE0_PRELOAD_ADDR_V << CACHE_L1_ICACHE0_PRELOAD_ADDR_S)
#define CACHE_L1_ICACHE0_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE0_PRELOAD_ADDR_S  0

/** CACHE_L1_ICACHE0_PRELOAD_SIZE_REG register
 *  L1 instruction Cache 0 preload size configure register
 */
#define CACHE_L1_ICACHE0_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xb0)
/** CACHE_L1_ICACHE0_PRELOAD_SIZE : R/W; bitpos: [13:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache0, which should be used together with L1_ICACHE0_PRELOAD_ADDR_REG
 */
#define CACHE_L1_ICACHE0_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE0_PRELOAD_SIZE_M  (CACHE_L1_ICACHE0_PRELOAD_SIZE_V << CACHE_L1_ICACHE0_PRELOAD_SIZE_S)
#define CACHE_L1_ICACHE0_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE0_PRELOAD_SIZE_S  0

/** CACHE_L1_ICACHE1_PRELOAD_CTRL_REG register
 *  L1 instruction Cache 1 preload-operation control register
 */
#define CACHE_L1_ICACHE1_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xb4)
/** CACHE_L1_ICACHE1_PRELOAD_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable preload operation on L1-ICache1. It will be cleared by
 *  hardware automatically after preload operation is done.
 */
#define CACHE_L1_ICACHE1_PRELOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE1_PRELOAD_ENA_M  (CACHE_L1_ICACHE1_PRELOAD_ENA_V << CACHE_L1_ICACHE1_PRELOAD_ENA_S)
#define CACHE_L1_ICACHE1_PRELOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE1_PRELOAD_ENA_S  0
/** CACHE_L1_ICACHE1_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether preload operation is finished or not. 0: not
 *  finished. 1: finished.
 */
#define CACHE_L1_ICACHE1_PRELOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE1_PRELOAD_DONE_M  (CACHE_L1_ICACHE1_PRELOAD_DONE_V << CACHE_L1_ICACHE1_PRELOAD_DONE_S)
#define CACHE_L1_ICACHE1_PRELOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE1_PRELOAD_DONE_S  1
/** CACHE_L1_ICACHE1_PRELOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L1_ICACHE1_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE1_PRELOAD_ORDER_M  (CACHE_L1_ICACHE1_PRELOAD_ORDER_V << CACHE_L1_ICACHE1_PRELOAD_ORDER_S)
#define CACHE_L1_ICACHE1_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE1_PRELOAD_ORDER_S  2
/** CACHE_L1_ICACHE1_PRELOAD_RGID : R/W; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l1 icache1 preload.
 */
#define CACHE_L1_ICACHE1_PRELOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE1_PRELOAD_RGID_M  (CACHE_L1_ICACHE1_PRELOAD_RGID_V << CACHE_L1_ICACHE1_PRELOAD_RGID_S)
#define CACHE_L1_ICACHE1_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE1_PRELOAD_RGID_S  3

/** CACHE_L1_ICACHE1_PRELOAD_ADDR_REG register
 *  L1 instruction Cache 1 preload address configure register
 */
#define CACHE_L1_ICACHE1_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xb8)
/** CACHE_L1_ICACHE1_PRELOAD_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of preload on
 *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOAD_SIZE_REG
 */
#define CACHE_L1_ICACHE1_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE1_PRELOAD_ADDR_M  (CACHE_L1_ICACHE1_PRELOAD_ADDR_V << CACHE_L1_ICACHE1_PRELOAD_ADDR_S)
#define CACHE_L1_ICACHE1_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE1_PRELOAD_ADDR_S  0

/** CACHE_L1_ICACHE1_PRELOAD_SIZE_REG register
 *  L1 instruction Cache 1 preload size configure register
 */
#define CACHE_L1_ICACHE1_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xbc)
/** CACHE_L1_ICACHE1_PRELOAD_SIZE : R/W; bitpos: [13:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache1, which should be used together with L1_ICACHE1_PRELOAD_ADDR_REG
 */
#define CACHE_L1_ICACHE1_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE1_PRELOAD_SIZE_M  (CACHE_L1_ICACHE1_PRELOAD_SIZE_V << CACHE_L1_ICACHE1_PRELOAD_SIZE_S)
#define CACHE_L1_ICACHE1_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE1_PRELOAD_SIZE_S  0

/** CACHE_L1_ICACHE2_PRELOAD_CTRL_REG register
 *  L1 instruction Cache 2 preload-operation control register
 */
#define CACHE_L1_ICACHE2_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xc0)
/** CACHE_L1_ICACHE2_PRELOAD_ENA : HRO; bitpos: [0]; default: 0;
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
/** CACHE_L1_ICACHE2_PRELOAD_ORDER : HRO; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L1_ICACHE2_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE2_PRELOAD_ORDER_M  (CACHE_L1_ICACHE2_PRELOAD_ORDER_V << CACHE_L1_ICACHE2_PRELOAD_ORDER_S)
#define CACHE_L1_ICACHE2_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE2_PRELOAD_ORDER_S  2
/** CACHE_L1_ICACHE2_PRELOAD_RGID : HRO; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l1 icache2 preload.
 */
#define CACHE_L1_ICACHE2_PRELOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE2_PRELOAD_RGID_M  (CACHE_L1_ICACHE2_PRELOAD_RGID_V << CACHE_L1_ICACHE2_PRELOAD_RGID_S)
#define CACHE_L1_ICACHE2_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE2_PRELOAD_RGID_S  3

/** CACHE_L1_ICACHE2_PRELOAD_ADDR_REG register
 *  L1 instruction Cache 2 preload address configure register
 */
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xc4)
/** CACHE_L1_ICACHE2_PRELOAD_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of preload on
 *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOAD_SIZE_REG
 */
#define CACHE_L1_ICACHE2_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_M  (CACHE_L1_ICACHE2_PRELOAD_ADDR_V << CACHE_L1_ICACHE2_PRELOAD_ADDR_S)
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_PRELOAD_ADDR_S  0

/** CACHE_L1_ICACHE2_PRELOAD_SIZE_REG register
 *  L1 instruction Cache 2 preload size configure register
 */
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xc8)
/** CACHE_L1_ICACHE2_PRELOAD_SIZE : HRO; bitpos: [13:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache2, which should be used together with L1_ICACHE2_PRELOAD_ADDR_REG
 */
#define CACHE_L1_ICACHE2_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_M  (CACHE_L1_ICACHE2_PRELOAD_SIZE_V << CACHE_L1_ICACHE2_PRELOAD_SIZE_S)
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE2_PRELOAD_SIZE_S  0

/** CACHE_L1_ICACHE3_PRELOAD_CTRL_REG register
 *  L1 instruction Cache 3 preload-operation control register
 */
#define CACHE_L1_ICACHE3_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xcc)
/** CACHE_L1_ICACHE3_PRELOAD_ENA : HRO; bitpos: [0]; default: 0;
 *  The bit is used to enable preload operation on L1-ICache3. It will be cleared by
 *  hardware automatically after preload operation is done.
 */
#define CACHE_L1_ICACHE3_PRELOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE3_PRELOAD_ENA_M  (CACHE_L1_ICACHE3_PRELOAD_ENA_V << CACHE_L1_ICACHE3_PRELOAD_ENA_S)
#define CACHE_L1_ICACHE3_PRELOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE3_PRELOAD_ENA_S  0
/** CACHE_L1_ICACHE3_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether preload operation is finished or not. 0: not
 *  finished. 1: finished.
 */
#define CACHE_L1_ICACHE3_PRELOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE3_PRELOAD_DONE_M  (CACHE_L1_ICACHE3_PRELOAD_DONE_V << CACHE_L1_ICACHE3_PRELOAD_DONE_S)
#define CACHE_L1_ICACHE3_PRELOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE3_PRELOAD_DONE_S  1
/** CACHE_L1_ICACHE3_PRELOAD_ORDER : HRO; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L1_ICACHE3_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE3_PRELOAD_ORDER_M  (CACHE_L1_ICACHE3_PRELOAD_ORDER_V << CACHE_L1_ICACHE3_PRELOAD_ORDER_S)
#define CACHE_L1_ICACHE3_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE3_PRELOAD_ORDER_S  2
/** CACHE_L1_ICACHE3_PRELOAD_RGID : HRO; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l1 icache3 preload.
 */
#define CACHE_L1_ICACHE3_PRELOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE3_PRELOAD_RGID_M  (CACHE_L1_ICACHE3_PRELOAD_RGID_V << CACHE_L1_ICACHE3_PRELOAD_RGID_S)
#define CACHE_L1_ICACHE3_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE3_PRELOAD_RGID_S  3

/** CACHE_L1_ICACHE3_PRELOAD_ADDR_REG register
 *  L1 instruction Cache 3 preload address configure register
 */
#define CACHE_L1_ICACHE3_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xd0)
/** CACHE_L1_ICACHE3_PRELOAD_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of preload on
 *  L1-ICache3, which should be used together with L1_ICACHE3_PRELOAD_SIZE_REG
 */
#define CACHE_L1_ICACHE3_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE3_PRELOAD_ADDR_M  (CACHE_L1_ICACHE3_PRELOAD_ADDR_V << CACHE_L1_ICACHE3_PRELOAD_ADDR_S)
#define CACHE_L1_ICACHE3_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE3_PRELOAD_ADDR_S  0

/** CACHE_L1_ICACHE3_PRELOAD_SIZE_REG register
 *  L1 instruction Cache 3 preload size configure register
 */
#define CACHE_L1_ICACHE3_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xd4)
/** CACHE_L1_ICACHE3_PRELOAD_SIZE : HRO; bitpos: [13:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-ICache3, which should be used together with L1_ICACHE3_PRELOAD_ADDR_REG
 */
#define CACHE_L1_ICACHE3_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_ICACHE3_PRELOAD_SIZE_M  (CACHE_L1_ICACHE3_PRELOAD_SIZE_V << CACHE_L1_ICACHE3_PRELOAD_SIZE_S)
#define CACHE_L1_ICACHE3_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_ICACHE3_PRELOAD_SIZE_S  0

/** CACHE_L1_DCACHE_PRELOAD_CTRL_REG register
 *  L1 data Cache  preload-operation control register
 */
#define CACHE_L1_DCACHE_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xd8)
/** CACHE_L1_DCACHE_PRELOAD_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable preload operation on L1-DCache. It will be cleared by
 *  hardware automatically after preload operation is done.
 */
#define CACHE_L1_DCACHE_PRELOAD_ENA    (BIT(0))
#define CACHE_L1_DCACHE_PRELOAD_ENA_M  (CACHE_L1_DCACHE_PRELOAD_ENA_V << CACHE_L1_DCACHE_PRELOAD_ENA_S)
#define CACHE_L1_DCACHE_PRELOAD_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_PRELOAD_ENA_S  0
/** CACHE_L1_DCACHE_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether preload operation is finished or not. 0: not
 *  finished. 1: finished.
 */
#define CACHE_L1_DCACHE_PRELOAD_DONE    (BIT(1))
#define CACHE_L1_DCACHE_PRELOAD_DONE_M  (CACHE_L1_DCACHE_PRELOAD_DONE_V << CACHE_L1_DCACHE_PRELOAD_DONE_S)
#define CACHE_L1_DCACHE_PRELOAD_DONE_V  0x00000001U
#define CACHE_L1_DCACHE_PRELOAD_DONE_S  1
/** CACHE_L1_DCACHE_PRELOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L1_DCACHE_PRELOAD_ORDER    (BIT(2))
#define CACHE_L1_DCACHE_PRELOAD_ORDER_M  (CACHE_L1_DCACHE_PRELOAD_ORDER_V << CACHE_L1_DCACHE_PRELOAD_ORDER_S)
#define CACHE_L1_DCACHE_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L1_DCACHE_PRELOAD_ORDER_S  2
/** CACHE_L1_DCACHE_PRELOAD_RGID : R/W; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l1 dcache preload.
 */
#define CACHE_L1_DCACHE_PRELOAD_RGID    0x0000000FU
#define CACHE_L1_DCACHE_PRELOAD_RGID_M  (CACHE_L1_DCACHE_PRELOAD_RGID_V << CACHE_L1_DCACHE_PRELOAD_RGID_S)
#define CACHE_L1_DCACHE_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L1_DCACHE_PRELOAD_RGID_S  3

/** CACHE_L1_DCACHE_PRELOAD_ADDR_REG register
 *  L1 data Cache  preload address configure register
 */
#define CACHE_L1_DCACHE_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0xdc)
/** CACHE_L1_DCACHE_PRELOAD_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of preload on L1-DCache,
 *  which should be used together with L1_DCACHE_PRELOAD_SIZE_REG
 */
#define CACHE_L1_DCACHE_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_PRELOAD_ADDR_M  (CACHE_L1_DCACHE_PRELOAD_ADDR_V << CACHE_L1_DCACHE_PRELOAD_ADDR_S)
#define CACHE_L1_DCACHE_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_PRELOAD_ADDR_S  0

/** CACHE_L1_DCACHE_PRELOAD_SIZE_REG register
 *  L1 data Cache  preload size configure register
 */
#define CACHE_L1_DCACHE_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0xe0)
/** CACHE_L1_DCACHE_PRELOAD_SIZE : R/W; bitpos: [13:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L1-DCache, which should be used together with L1_DCACHE_PRELOAD_ADDR_REG
 */
#define CACHE_L1_DCACHE_PRELOAD_SIZE    0x00003FFFU
#define CACHE_L1_DCACHE_PRELOAD_SIZE_M  (CACHE_L1_DCACHE_PRELOAD_SIZE_V << CACHE_L1_DCACHE_PRELOAD_SIZE_S)
#define CACHE_L1_DCACHE_PRELOAD_SIZE_V  0x00003FFFU
#define CACHE_L1_DCACHE_PRELOAD_SIZE_S  0

/** CACHE_L1_ICACHE0_AUTOLOAD_CTRL_REG register
 *  L1 instruction Cache 0 autoload-operation control register
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xe4)
/** CACHE_L1_ICACHE0_AUTOLOAD_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable and disable autoload operation on L1-ICache0.  1: enable,
 *  0: disable.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE0_AUTOLOAD_ENA_M  (CACHE_L1_ICACHE0_AUTOLOAD_ENA_V << CACHE_L1_ICACHE0_AUTOLOAD_ENA_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE0_AUTOLOAD_ENA_S  0
/** CACHE_L1_ICACHE0_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether autoload operation on L1-ICache0 is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE0_AUTOLOAD_DONE_M  (CACHE_L1_ICACHE0_AUTOLOAD_DONE_V << CACHE_L1_ICACHE0_AUTOLOAD_DONE_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE0_AUTOLOAD_DONE_S  1
/** CACHE_L1_ICACHE0_AUTOLOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L1-ICache0. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE0_AUTOLOAD_ORDER_M  (CACHE_L1_ICACHE0_AUTOLOAD_ORDER_V << CACHE_L1_ICACHE0_AUTOLOAD_ORDER_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE0_AUTOLOAD_ORDER_S  2
/** CACHE_L1_ICACHE0_AUTOLOAD_TRIGGER_MODE : R/W; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L1-ICache0.
 *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_ICACHE0_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_ICACHE0_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_ICACHE0_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_ICACHE0_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L1-ICache0.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ENA_V << CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L1-ICache0.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ENA_V << CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ENA_S  9
/** CACHE_L1_ICACHE0_AUTOLOAD_RGID : R/W; bitpos: [13:10]; default: 0;
 *  The bit is used to set  the gid of l1 icache0 autoload.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE0_AUTOLOAD_RGID_M  (CACHE_L1_ICACHE0_AUTOLOAD_RGID_V << CACHE_L1_ICACHE0_AUTOLOAD_RGID_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE0_AUTOLOAD_RGID_S  10

/** CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 instruction Cache 0 autoload section 0 address configure register
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0xe8)
/** CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section for
 *  autoload operation on L1-ICache0. Note that it should be used together with
 *  L1_ICACHE0_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR_M  (CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR_V << CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE_REG register
 *  L1 instruction Cache 0 autoload section 0 size configure register
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0xec)
/** CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the first section for autoload
 *  operation on L1-ICache0. Note that it should be used together with
 *  L1_ICACHE0_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE_M  (CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE_V << CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR_REG register
 *  L1 instruction Cache 0 autoload section 1 address configure register
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0xf0)
/** CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section
 *  for autoload operation on L1-ICache0. Note that it should be used together with
 *  L1_ICACHE0_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 instruction Cache 0 autoload section 1 size configure register
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0xf4)
/** CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the second section for autoload
 *  operation on L1-ICache0. Note that it should be used together with
 *  L1_ICACHE0_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE0_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_ICACHE1_AUTOLOAD_CTRL_REG register
 *  L1 instruction Cache 1 autoload-operation control register
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0xf8)
/** CACHE_L1_ICACHE1_AUTOLOAD_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable and disable autoload operation on L1-ICache1.  1: enable,
 *  0: disable.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE1_AUTOLOAD_ENA_M  (CACHE_L1_ICACHE1_AUTOLOAD_ENA_V << CACHE_L1_ICACHE1_AUTOLOAD_ENA_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE1_AUTOLOAD_ENA_S  0
/** CACHE_L1_ICACHE1_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether autoload operation on L1-ICache1 is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE1_AUTOLOAD_DONE_M  (CACHE_L1_ICACHE1_AUTOLOAD_DONE_V << CACHE_L1_ICACHE1_AUTOLOAD_DONE_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE1_AUTOLOAD_DONE_S  1
/** CACHE_L1_ICACHE1_AUTOLOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L1-ICache1. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE1_AUTOLOAD_ORDER_M  (CACHE_L1_ICACHE1_AUTOLOAD_ORDER_V << CACHE_L1_ICACHE1_AUTOLOAD_ORDER_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE1_AUTOLOAD_ORDER_S  2
/** CACHE_L1_ICACHE1_AUTOLOAD_TRIGGER_MODE : R/W; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L1-ICache1.
 *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_ICACHE1_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_ICACHE1_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_ICACHE1_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_ICACHE1_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L1-ICache1.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ENA_V << CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L1-ICache1.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ENA_V << CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ENA_S  9
/** CACHE_L1_ICACHE1_AUTOLOAD_RGID : R/W; bitpos: [13:10]; default: 0;
 *  The bit is used to set  the gid of l1 icache1 autoload.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE1_AUTOLOAD_RGID_M  (CACHE_L1_ICACHE1_AUTOLOAD_RGID_V << CACHE_L1_ICACHE1_AUTOLOAD_RGID_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE1_AUTOLOAD_RGID_S  10

/** CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 instruction Cache 1 autoload section 0 address configure register
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0xfc)
/** CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section for
 *  autoload operation on L1-ICache1. Note that it should be used together with
 *  L1_ICACHE1_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR_M  (CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR_V << CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE_REG register
 *  L1 instruction Cache 1 autoload section 0 size configure register
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x100)
/** CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the first section for autoload
 *  operation on L1-ICache1. Note that it should be used together with
 *  L1_ICACHE1_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE_M  (CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE_V << CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR_REG register
 *  L1 instruction Cache 1 autoload section 1 address configure register
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x104)
/** CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section
 *  for autoload operation on L1-ICache1. Note that it should be used together with
 *  L1_ICACHE1_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 instruction Cache 1 autoload section 1 size configure register
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x108)
/** CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the second section for autoload
 *  operation on L1-ICache1. Note that it should be used together with
 *  L1_ICACHE1_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE1_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_ICACHE2_AUTOLOAD_CTRL_REG register
 *  L1 instruction Cache 2 autoload-operation control register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x10c)
/** CACHE_L1_ICACHE2_AUTOLOAD_ENA : HRO; bitpos: [0]; default: 0;
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
/** CACHE_L1_ICACHE2_AUTOLOAD_ORDER : HRO; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L1-ICache2. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER_M  (CACHE_L1_ICACHE2_AUTOLOAD_ORDER_V << CACHE_L1_ICACHE2_AUTOLOAD_ORDER_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_ORDER_S  2
/** CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE : HRO; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L1-ICache2.
 *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_ICACHE2_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA : HRO; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA : HRO; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L1-ICache2.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ENA_S  9
/** CACHE_L1_ICACHE2_AUTOLOAD_RGID : HRO; bitpos: [13:10]; default: 0;
 *  The bit is used to set  the gid of l1 icache2 autoload.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID_M  (CACHE_L1_ICACHE2_AUTOLOAD_RGID_V << CACHE_L1_ICACHE2_AUTOLOAD_RGID_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE2_AUTOLOAD_RGID_S  10

/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 instruction Cache 2 autoload section 0 address configure register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x110)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section for
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
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x114)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT0_SIZE : HRO; bitpos: [27:0]; default: 0;
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
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x118)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section
 *  for autoload operation on L1-ICache2. Note that it should be used together with
 *  L1_ICACHE2_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 instruction Cache 2 autoload section 1 size configure register
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x11c)
/** CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE : HRO; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the second section for autoload
 *  operation on L1-ICache2. Note that it should be used together with
 *  L1_ICACHE2_AUTOLOAD_SCT1_ADDR and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE2_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_ICACHE3_AUTOLOAD_CTRL_REG register
 *  L1 instruction Cache 3 autoload-operation control register
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x120)
/** CACHE_L1_ICACHE3_AUTOLOAD_ENA : HRO; bitpos: [0]; default: 0;
 *  The bit is used to enable and disable autoload operation on L1-ICache3.  1: enable,
 *  0: disable.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L1_ICACHE3_AUTOLOAD_ENA_M  (CACHE_L1_ICACHE3_AUTOLOAD_ENA_V << CACHE_L1_ICACHE3_AUTOLOAD_ENA_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L1_ICACHE3_AUTOLOAD_ENA_S  0
/** CACHE_L1_ICACHE3_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether autoload operation on L1-ICache3 is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L1_ICACHE3_AUTOLOAD_DONE_M  (CACHE_L1_ICACHE3_AUTOLOAD_DONE_V << CACHE_L1_ICACHE3_AUTOLOAD_DONE_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L1_ICACHE3_AUTOLOAD_DONE_S  1
/** CACHE_L1_ICACHE3_AUTOLOAD_ORDER : HRO; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L1-ICache3. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_ICACHE3_AUTOLOAD_ORDER_M  (CACHE_L1_ICACHE3_AUTOLOAD_ORDER_V << CACHE_L1_ICACHE3_AUTOLOAD_ORDER_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_ICACHE3_AUTOLOAD_ORDER_S  2
/** CACHE_L1_ICACHE3_AUTOLOAD_TRIGGER_MODE : HRO; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L1-ICache3.
 *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_ICACHE3_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_ICACHE3_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_ICACHE3_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_ICACHE3_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ENA : HRO; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L1-ICache3.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ENA_V << CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ENA : HRO; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L1-ICache3.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ENA_V << CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ENA_S  9
/** CACHE_L1_ICACHE3_AUTOLOAD_RGID : HRO; bitpos: [13:10]; default: 0;
 *  The bit is used to set  the gid of l1 icache3 autoload.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_RGID    0x0000000FU
#define CACHE_L1_ICACHE3_AUTOLOAD_RGID_M  (CACHE_L1_ICACHE3_AUTOLOAD_RGID_V << CACHE_L1_ICACHE3_AUTOLOAD_RGID_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_RGID_V  0x0000000FU
#define CACHE_L1_ICACHE3_AUTOLOAD_RGID_S  10

/** CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 instruction Cache 3 autoload section 0 address configure register
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x124)
/** CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section for
 *  autoload operation on L1-ICache3. Note that it should be used together with
 *  L1_ICACHE3_AUTOLOAD_SCT0_SIZE and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR_M  (CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR_V << CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE_REG register
 *  L1 instruction Cache 3 autoload section 0 size configure register
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x128)
/** CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE : HRO; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the first section for autoload
 *  operation on L1-ICache3. Note that it should be used together with
 *  L1_ICACHE3_AUTOLOAD_SCT0_ADDR and L1_ICACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE_M  (CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE_V << CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR_REG register
 *  L1 instruction Cache 3 autoload section 1 address configure register
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x12c)
/** CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR : HRO; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section
 *  for autoload operation on L1-ICache3. Note that it should be used together with
 *  L1_ICACHE3_AUTOLOAD_SCT1_SIZE and L1_ICACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 instruction Cache 3 autoload section 1 size configure register
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x130)
/** CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE : HRO; bitpos: [27:0]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE    0x0FFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_ICACHE3_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_CTRL_REG register
 *  L1 data Cache autoload-operation control register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x134)
/** CACHE_L1_DCACHE_AUTOLOAD_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable and disable autoload operation on L1-DCache.  1: enable,
 *  0: disable.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L1_DCACHE_AUTOLOAD_ENA_M  (CACHE_L1_DCACHE_AUTOLOAD_ENA_V << CACHE_L1_DCACHE_AUTOLOAD_ENA_S)
#define CACHE_L1_DCACHE_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_AUTOLOAD_ENA_S  0
/** CACHE_L1_DCACHE_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether autoload operation on L1-DCache is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L1_DCACHE_AUTOLOAD_DONE_M  (CACHE_L1_DCACHE_AUTOLOAD_DONE_V << CACHE_L1_DCACHE_AUTOLOAD_DONE_S)
#define CACHE_L1_DCACHE_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L1_DCACHE_AUTOLOAD_DONE_S  1
/** CACHE_L1_DCACHE_AUTOLOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L1-DCache. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L1_DCACHE_AUTOLOAD_ORDER_M  (CACHE_L1_DCACHE_AUTOLOAD_ORDER_V << CACHE_L1_DCACHE_AUTOLOAD_ORDER_S)
#define CACHE_L1_DCACHE_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L1_DCACHE_AUTOLOAD_ORDER_S  2
/** CACHE_L1_DCACHE_AUTOLOAD_TRIGGER_MODE : R/W; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L1-DCache.
 *  0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L1_DCACHE_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L1_DCACHE_AUTOLOAD_TRIGGER_MODE_V << CACHE_L1_DCACHE_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L1_DCACHE_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L1_DCACHE_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L1_DCACHE_AUTOLOAD_SCT0_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L1-DCache.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ENA_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT0_ENA_V << CACHE_L1_DCACHE_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L1_DCACHE_AUTOLOAD_SCT1_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L1-DCache.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ENA_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT1_ENA_V << CACHE_L1_DCACHE_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ENA_S  9
/** CACHE_L1_DCACHE_AUTOLOAD_SCT2_ENA : R/W; bitpos: [10]; default: 0;
 *  The bit is used to enable the third section for autoload operation on L1-DCache.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ENA    (BIT(10))
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ENA_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT2_ENA_V << CACHE_L1_DCACHE_AUTOLOAD_SCT2_ENA_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ENA_S  10
/** CACHE_L1_DCACHE_AUTOLOAD_SCT3_ENA : R/W; bitpos: [11]; default: 0;
 *  The bit is used to enable the fourth section for autoload operation on L1-DCache.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ENA    (BIT(11))
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ENA_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT3_ENA_V << CACHE_L1_DCACHE_AUTOLOAD_SCT3_ENA_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ENA_S  11
/** CACHE_L1_DCACHE_AUTOLOAD_RGID : R/W; bitpos: [15:12]; default: 0;
 *  The bit is used to set  the gid of l1 dcache autoload.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_RGID    0x0000000FU
#define CACHE_L1_DCACHE_AUTOLOAD_RGID_M  (CACHE_L1_DCACHE_AUTOLOAD_RGID_V << CACHE_L1_DCACHE_AUTOLOAD_RGID_S)
#define CACHE_L1_DCACHE_AUTOLOAD_RGID_V  0x0000000FU
#define CACHE_L1_DCACHE_AUTOLOAD_RGID_S  12

/** CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR_REG register
 *  L1 data Cache autoload section 0 address configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x138)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section for
 *  autoload operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT0_SIZE and L1_DCACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR_V << CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE_REG register
 *  L1 data Cache autoload section 0 size configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x13c)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the first section for autoload
 *  operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT0_ADDR and L1_DCACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE    0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE_V << CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR_REG register
 *  L1 data Cache autoload section 1 address configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x140)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section
 *  for autoload operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT1_SIZE and L1_DCACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR_V << CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE_REG register
 *  L1 data Cache autoload section 1 size configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x144)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the second section for autoload
 *  operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT1_ADDR and L1_DCACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE    0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE_V << CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR_REG register
 *  L1 data Cache autoload section 2 address configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR_REG (DR_REG_CACHE_BASE + 0x148)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the third section for
 *  autoload operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT2_SIZE and L1_DCACHE_AUTOLOAD_SCT2_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR_V << CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_ADDR_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE_REG register
 *  L1 data Cache autoload section 2 size configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE_REG (DR_REG_CACHE_BASE + 0x14c)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the third section for autoload
 *  operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT2_ADDR and L1_DCACHE_AUTOLOAD_SCT2_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE    0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE_V << CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT2_SIZE_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR_REG register
 *  L1 data Cache autoload section 1 address configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR_REG (DR_REG_CACHE_BASE + 0x150)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the fourth section
 *  for autoload operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT3_SIZE and L1_DCACHE_AUTOLOAD_SCT3_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR_V << CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_ADDR_S  0

/** CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE_REG register
 *  L1 data Cache autoload section 1 size configure register
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE_REG (DR_REG_CACHE_BASE + 0x154)
/** CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the fourth section for autoload
 *  operation on L1-DCache. Note that it should be used together with
 *  L1_DCACHE_AUTOLOAD_SCT3_ADDR and L1_DCACHE_AUTOLOAD_SCT3_ENA.
 */
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE    0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE_M  (CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE_V << CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE_S)
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE_V  0x0FFFFFFFU
#define CACHE_L1_DCACHE_AUTOLOAD_SCT3_SIZE_S  0

/** CACHE_L1_CACHE_ACS_CNT_INT_ENA_REG register
 *  Cache Access Counter Interrupt enable register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_ENA_REG (DR_REG_CACHE_BASE + 0x158)
/** CACHE_L1_IBUS0_OVF_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L1-ICache0 due to bus0 accesses L1-ICache0.
 */
#define CACHE_L1_IBUS0_OVF_INT_ENA    (BIT(0))
#define CACHE_L1_IBUS0_OVF_INT_ENA_M  (CACHE_L1_IBUS0_OVF_INT_ENA_V << CACHE_L1_IBUS0_OVF_INT_ENA_S)
#define CACHE_L1_IBUS0_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_IBUS0_OVF_INT_ENA_S  0
/** CACHE_L1_IBUS1_OVF_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L1-ICache1 due to bus1 accesses L1-ICache1.
 */
#define CACHE_L1_IBUS1_OVF_INT_ENA    (BIT(1))
#define CACHE_L1_IBUS1_OVF_INT_ENA_M  (CACHE_L1_IBUS1_OVF_INT_ENA_V << CACHE_L1_IBUS1_OVF_INT_ENA_S)
#define CACHE_L1_IBUS1_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_IBUS1_OVF_INT_ENA_S  1
/** CACHE_L1_IBUS2_OVF_INT_ENA : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_OVF_INT_ENA    (BIT(2))
#define CACHE_L1_IBUS2_OVF_INT_ENA_M  (CACHE_L1_IBUS2_OVF_INT_ENA_V << CACHE_L1_IBUS2_OVF_INT_ENA_S)
#define CACHE_L1_IBUS2_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_IBUS2_OVF_INT_ENA_S  2
/** CACHE_L1_IBUS3_OVF_INT_ENA : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_OVF_INT_ENA    (BIT(3))
#define CACHE_L1_IBUS3_OVF_INT_ENA_M  (CACHE_L1_IBUS3_OVF_INT_ENA_V << CACHE_L1_IBUS3_OVF_INT_ENA_S)
#define CACHE_L1_IBUS3_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_IBUS3_OVF_INT_ENA_S  3
/** CACHE_L1_DBUS0_OVF_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L1-DCache due to bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_OVF_INT_ENA    (BIT(4))
#define CACHE_L1_DBUS0_OVF_INT_ENA_M  (CACHE_L1_DBUS0_OVF_INT_ENA_V << CACHE_L1_DBUS0_OVF_INT_ENA_S)
#define CACHE_L1_DBUS0_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_DBUS0_OVF_INT_ENA_S  4
/** CACHE_L1_DBUS1_OVF_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L1-DCache due to bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_OVF_INT_ENA    (BIT(5))
#define CACHE_L1_DBUS1_OVF_INT_ENA_M  (CACHE_L1_DBUS1_OVF_INT_ENA_V << CACHE_L1_DBUS1_OVF_INT_ENA_S)
#define CACHE_L1_DBUS1_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_DBUS1_OVF_INT_ENA_S  5
/** CACHE_L1_DBUS2_OVF_INT_ENA : HRO; bitpos: [6]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS2_OVF_INT_ENA    (BIT(6))
#define CACHE_L1_DBUS2_OVF_INT_ENA_M  (CACHE_L1_DBUS2_OVF_INT_ENA_V << CACHE_L1_DBUS2_OVF_INT_ENA_S)
#define CACHE_L1_DBUS2_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_DBUS2_OVF_INT_ENA_S  6
/** CACHE_L1_DBUS3_OVF_INT_ENA : HRO; bitpos: [7]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS3_OVF_INT_ENA    (BIT(7))
#define CACHE_L1_DBUS3_OVF_INT_ENA_M  (CACHE_L1_DBUS3_OVF_INT_ENA_V << CACHE_L1_DBUS3_OVF_INT_ENA_S)
#define CACHE_L1_DBUS3_OVF_INT_ENA_V  0x00000001U
#define CACHE_L1_DBUS3_OVF_INT_ENA_S  7

/** CACHE_L1_CACHE_ACS_CNT_INT_CLR_REG register
 *  Cache Access Counter Interrupt clear register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_CLR_REG (DR_REG_CACHE_BASE + 0x15c)
/** CACHE_L1_IBUS0_OVF_INT_CLR : WT; bitpos: [0]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L1-ICache0 due
 *  to bus0 accesses L1-ICache0.
 */
#define CACHE_L1_IBUS0_OVF_INT_CLR    (BIT(0))
#define CACHE_L1_IBUS0_OVF_INT_CLR_M  (CACHE_L1_IBUS0_OVF_INT_CLR_V << CACHE_L1_IBUS0_OVF_INT_CLR_S)
#define CACHE_L1_IBUS0_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_IBUS0_OVF_INT_CLR_S  0
/** CACHE_L1_IBUS1_OVF_INT_CLR : WT; bitpos: [1]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L1-ICache1 due
 *  to bus1 accesses L1-ICache1.
 */
#define CACHE_L1_IBUS1_OVF_INT_CLR    (BIT(1))
#define CACHE_L1_IBUS1_OVF_INT_CLR_M  (CACHE_L1_IBUS1_OVF_INT_CLR_V << CACHE_L1_IBUS1_OVF_INT_CLR_S)
#define CACHE_L1_IBUS1_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_IBUS1_OVF_INT_CLR_S  1
/** CACHE_L1_IBUS2_OVF_INT_CLR : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_OVF_INT_CLR    (BIT(2))
#define CACHE_L1_IBUS2_OVF_INT_CLR_M  (CACHE_L1_IBUS2_OVF_INT_CLR_V << CACHE_L1_IBUS2_OVF_INT_CLR_S)
#define CACHE_L1_IBUS2_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_IBUS2_OVF_INT_CLR_S  2
/** CACHE_L1_IBUS3_OVF_INT_CLR : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_OVF_INT_CLR    (BIT(3))
#define CACHE_L1_IBUS3_OVF_INT_CLR_M  (CACHE_L1_IBUS3_OVF_INT_CLR_V << CACHE_L1_IBUS3_OVF_INT_CLR_S)
#define CACHE_L1_IBUS3_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_IBUS3_OVF_INT_CLR_S  3
/** CACHE_L1_DBUS0_OVF_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
 *  to bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_OVF_INT_CLR    (BIT(4))
#define CACHE_L1_DBUS0_OVF_INT_CLR_M  (CACHE_L1_DBUS0_OVF_INT_CLR_V << CACHE_L1_DBUS0_OVF_INT_CLR_S)
#define CACHE_L1_DBUS0_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_DBUS0_OVF_INT_CLR_S  4
/** CACHE_L1_DBUS1_OVF_INT_CLR : WT; bitpos: [5]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L1-DCache due
 *  to bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_OVF_INT_CLR    (BIT(5))
#define CACHE_L1_DBUS1_OVF_INT_CLR_M  (CACHE_L1_DBUS1_OVF_INT_CLR_V << CACHE_L1_DBUS1_OVF_INT_CLR_S)
#define CACHE_L1_DBUS1_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_DBUS1_OVF_INT_CLR_S  5
/** CACHE_L1_DBUS2_OVF_INT_CLR : HRO; bitpos: [6]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS2_OVF_INT_CLR    (BIT(6))
#define CACHE_L1_DBUS2_OVF_INT_CLR_M  (CACHE_L1_DBUS2_OVF_INT_CLR_V << CACHE_L1_DBUS2_OVF_INT_CLR_S)
#define CACHE_L1_DBUS2_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_DBUS2_OVF_INT_CLR_S  6
/** CACHE_L1_DBUS3_OVF_INT_CLR : HRO; bitpos: [7]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS3_OVF_INT_CLR    (BIT(7))
#define CACHE_L1_DBUS3_OVF_INT_CLR_M  (CACHE_L1_DBUS3_OVF_INT_CLR_V << CACHE_L1_DBUS3_OVF_INT_CLR_S)
#define CACHE_L1_DBUS3_OVF_INT_CLR_V  0x00000001U
#define CACHE_L1_DBUS3_OVF_INT_CLR_S  7

/** CACHE_L1_CACHE_ACS_CNT_INT_RAW_REG register
 *  Cache Access Counter Interrupt raw register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_RAW_REG (DR_REG_CACHE_BASE + 0x160)
/** CACHE_L1_IBUS0_OVF_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache0
 *  due to bus0 accesses L1-ICache0.
 */
#define CACHE_L1_IBUS0_OVF_INT_RAW    (BIT(0))
#define CACHE_L1_IBUS0_OVF_INT_RAW_M  (CACHE_L1_IBUS0_OVF_INT_RAW_V << CACHE_L1_IBUS0_OVF_INT_RAW_S)
#define CACHE_L1_IBUS0_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_IBUS0_OVF_INT_RAW_S  0
/** CACHE_L1_IBUS1_OVF_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-ICache1
 *  due to bus1 accesses L1-ICache1.
 */
#define CACHE_L1_IBUS1_OVF_INT_RAW    (BIT(1))
#define CACHE_L1_IBUS1_OVF_INT_RAW_M  (CACHE_L1_IBUS1_OVF_INT_RAW_V << CACHE_L1_IBUS1_OVF_INT_RAW_S)
#define CACHE_L1_IBUS1_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_IBUS1_OVF_INT_RAW_S  1
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
/** CACHE_L1_DBUS0_OVF_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
 *  due to bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_OVF_INT_RAW    (BIT(4))
#define CACHE_L1_DBUS0_OVF_INT_RAW_M  (CACHE_L1_DBUS0_OVF_INT_RAW_V << CACHE_L1_DBUS0_OVF_INT_RAW_S)
#define CACHE_L1_DBUS0_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_DBUS0_OVF_INT_RAW_S  4
/** CACHE_L1_DBUS1_OVF_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
 *  due to bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_OVF_INT_RAW    (BIT(5))
#define CACHE_L1_DBUS1_OVF_INT_RAW_M  (CACHE_L1_DBUS1_OVF_INT_RAW_V << CACHE_L1_DBUS1_OVF_INT_RAW_S)
#define CACHE_L1_DBUS1_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_DBUS1_OVF_INT_RAW_S  5
/** CACHE_L1_DBUS2_OVF_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
 *  due to bus2 accesses L1-DCache.
 */
#define CACHE_L1_DBUS2_OVF_INT_RAW    (BIT(6))
#define CACHE_L1_DBUS2_OVF_INT_RAW_M  (CACHE_L1_DBUS2_OVF_INT_RAW_V << CACHE_L1_DBUS2_OVF_INT_RAW_S)
#define CACHE_L1_DBUS2_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_DBUS2_OVF_INT_RAW_S  6
/** CACHE_L1_DBUS3_OVF_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L1-DCache
 *  due to bus3 accesses L1-DCache.
 */
#define CACHE_L1_DBUS3_OVF_INT_RAW    (BIT(7))
#define CACHE_L1_DBUS3_OVF_INT_RAW_M  (CACHE_L1_DBUS3_OVF_INT_RAW_V << CACHE_L1_DBUS3_OVF_INT_RAW_S)
#define CACHE_L1_DBUS3_OVF_INT_RAW_V  0x00000001U
#define CACHE_L1_DBUS3_OVF_INT_RAW_S  7

/** CACHE_L1_CACHE_ACS_CNT_INT_ST_REG register
 *  Cache Access Counter Interrupt status register
 */
#define CACHE_L1_CACHE_ACS_CNT_INT_ST_REG (DR_REG_CACHE_BASE + 0x164)
/** CACHE_L1_IBUS0_OVF_INT_ST : RO; bitpos: [0]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L1-ICache0 due to bus0 accesses L1-ICache0.
 */
#define CACHE_L1_IBUS0_OVF_INT_ST    (BIT(0))
#define CACHE_L1_IBUS0_OVF_INT_ST_M  (CACHE_L1_IBUS0_OVF_INT_ST_V << CACHE_L1_IBUS0_OVF_INT_ST_S)
#define CACHE_L1_IBUS0_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_IBUS0_OVF_INT_ST_S  0
/** CACHE_L1_IBUS1_OVF_INT_ST : RO; bitpos: [1]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L1-ICache1 due to bus1 accesses L1-ICache1.
 */
#define CACHE_L1_IBUS1_OVF_INT_ST    (BIT(1))
#define CACHE_L1_IBUS1_OVF_INT_ST_M  (CACHE_L1_IBUS1_OVF_INT_ST_V << CACHE_L1_IBUS1_OVF_INT_ST_S)
#define CACHE_L1_IBUS1_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_IBUS1_OVF_INT_ST_S  1
/** CACHE_L1_IBUS2_OVF_INT_ST : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_OVF_INT_ST    (BIT(2))
#define CACHE_L1_IBUS2_OVF_INT_ST_M  (CACHE_L1_IBUS2_OVF_INT_ST_V << CACHE_L1_IBUS2_OVF_INT_ST_S)
#define CACHE_L1_IBUS2_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_IBUS2_OVF_INT_ST_S  2
/** CACHE_L1_IBUS3_OVF_INT_ST : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_OVF_INT_ST    (BIT(3))
#define CACHE_L1_IBUS3_OVF_INT_ST_M  (CACHE_L1_IBUS3_OVF_INT_ST_V << CACHE_L1_IBUS3_OVF_INT_ST_S)
#define CACHE_L1_IBUS3_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_IBUS3_OVF_INT_ST_S  3
/** CACHE_L1_DBUS0_OVF_INT_ST : RO; bitpos: [4]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L1-DCache due to bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_OVF_INT_ST    (BIT(4))
#define CACHE_L1_DBUS0_OVF_INT_ST_M  (CACHE_L1_DBUS0_OVF_INT_ST_V << CACHE_L1_DBUS0_OVF_INT_ST_S)
#define CACHE_L1_DBUS0_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_DBUS0_OVF_INT_ST_S  4
/** CACHE_L1_DBUS1_OVF_INT_ST : RO; bitpos: [5]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L1-DCache due to bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_OVF_INT_ST    (BIT(5))
#define CACHE_L1_DBUS1_OVF_INT_ST_M  (CACHE_L1_DBUS1_OVF_INT_ST_V << CACHE_L1_DBUS1_OVF_INT_ST_S)
#define CACHE_L1_DBUS1_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_DBUS1_OVF_INT_ST_S  5
/** CACHE_L1_DBUS2_OVF_INT_ST : HRO; bitpos: [6]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS2_OVF_INT_ST    (BIT(6))
#define CACHE_L1_DBUS2_OVF_INT_ST_M  (CACHE_L1_DBUS2_OVF_INT_ST_V << CACHE_L1_DBUS2_OVF_INT_ST_S)
#define CACHE_L1_DBUS2_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_DBUS2_OVF_INT_ST_S  6
/** CACHE_L1_DBUS3_OVF_INT_ST : HRO; bitpos: [7]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS3_OVF_INT_ST    (BIT(7))
#define CACHE_L1_DBUS3_OVF_INT_ST_M  (CACHE_L1_DBUS3_OVF_INT_ST_V << CACHE_L1_DBUS3_OVF_INT_ST_S)
#define CACHE_L1_DBUS3_OVF_INT_ST_V  0x00000001U
#define CACHE_L1_DBUS3_OVF_INT_ST_S  7

/** CACHE_L1_CACHE_ACS_FAIL_CTRL_REG register
 *  Cache Access Fail Configuration register
 */
#define CACHE_L1_CACHE_ACS_FAIL_CTRL_REG (DR_REG_CACHE_BASE + 0x168)
/** CACHE_L1_ICACHE0_ACS_FAIL_CHECK_MODE : R/W; bitpos: [0]; default: 0;
 *  The bit is used to configure l1 icache0 access fail check mode. 0: the access fail
 *  is not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L1_ICACHE0_ACS_FAIL_CHECK_MODE    (BIT(0))
#define CACHE_L1_ICACHE0_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_ICACHE0_ACS_FAIL_CHECK_MODE_V << CACHE_L1_ICACHE0_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_ICACHE0_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_ICACHE0_ACS_FAIL_CHECK_MODE_S  0
/** CACHE_L1_ICACHE1_ACS_FAIL_CHECK_MODE : R/W; bitpos: [1]; default: 0;
 *  The bit is used to configure l1 icache1 access fail check mode. 0: the access fail
 *  is not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L1_ICACHE1_ACS_FAIL_CHECK_MODE    (BIT(1))
#define CACHE_L1_ICACHE1_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_ICACHE1_ACS_FAIL_CHECK_MODE_V << CACHE_L1_ICACHE1_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_ICACHE1_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_ICACHE1_ACS_FAIL_CHECK_MODE_S  1
/** CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure l1 icache2 access fail check mode. 0: the access fail
 *  is not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE    (BIT(2))
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_V << CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_ICACHE2_ACS_FAIL_CHECK_MODE_S  2
/** CACHE_L1_ICACHE3_ACS_FAIL_CHECK_MODE : R/W; bitpos: [3]; default: 0;
 *  The bit is used to configure l1 icache3 access fail check mode. 0: the access fail
 *  is not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L1_ICACHE3_ACS_FAIL_CHECK_MODE    (BIT(3))
#define CACHE_L1_ICACHE3_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_ICACHE3_ACS_FAIL_CHECK_MODE_V << CACHE_L1_ICACHE3_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_ICACHE3_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_ICACHE3_ACS_FAIL_CHECK_MODE_S  3
/** CACHE_L1_DCACHE_ACS_FAIL_CHECK_MODE : R/W; bitpos: [4]; default: 0;
 *  The bit is used to configure l1 dcache access fail check mode. 0: the access fail
 *  is not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L1_DCACHE_ACS_FAIL_CHECK_MODE    (BIT(4))
#define CACHE_L1_DCACHE_ACS_FAIL_CHECK_MODE_M  (CACHE_L1_DCACHE_ACS_FAIL_CHECK_MODE_V << CACHE_L1_DCACHE_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L1_DCACHE_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L1_DCACHE_ACS_FAIL_CHECK_MODE_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_ENA_REG register
 *  Cache Access Fail Interrupt enable register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_ENA_REG (DR_REG_CACHE_BASE + 0x16c)
/** CACHE_L1_ICACHE0_FAIL_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable interrupt of access fail that occurs in L1-ICache0 due to
 *  cpu accesses L1-ICache0.
 */
#define CACHE_L1_ICACHE0_FAIL_INT_ENA    (BIT(0))
#define CACHE_L1_ICACHE0_FAIL_INT_ENA_M  (CACHE_L1_ICACHE0_FAIL_INT_ENA_V << CACHE_L1_ICACHE0_FAIL_INT_ENA_S)
#define CACHE_L1_ICACHE0_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE0_FAIL_INT_ENA_S  0
/** CACHE_L1_ICACHE1_FAIL_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable interrupt of access fail that occurs in L1-ICache1 due to
 *  cpu accesses L1-ICache1.
 */
#define CACHE_L1_ICACHE1_FAIL_INT_ENA    (BIT(1))
#define CACHE_L1_ICACHE1_FAIL_INT_ENA_M  (CACHE_L1_ICACHE1_FAIL_INT_ENA_V << CACHE_L1_ICACHE1_FAIL_INT_ENA_S)
#define CACHE_L1_ICACHE1_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE1_FAIL_INT_ENA_S  1
/** CACHE_L1_ICACHE2_FAIL_INT_ENA : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FAIL_INT_ENA    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_ENA_M  (CACHE_L1_ICACHE2_FAIL_INT_ENA_V << CACHE_L1_ICACHE2_FAIL_INT_ENA_S)
#define CACHE_L1_ICACHE2_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_ENA_S  2
/** CACHE_L1_ICACHE3_FAIL_INT_ENA : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_FAIL_INT_ENA    (BIT(3))
#define CACHE_L1_ICACHE3_FAIL_INT_ENA_M  (CACHE_L1_ICACHE3_FAIL_INT_ENA_V << CACHE_L1_ICACHE3_FAIL_INT_ENA_S)
#define CACHE_L1_ICACHE3_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE3_FAIL_INT_ENA_S  3
/** CACHE_L1_DCACHE_FAIL_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable interrupt of access fail that occurs in L1-DCache due to
 *  cpu accesses L1-DCache.
 */
#define CACHE_L1_DCACHE_FAIL_INT_ENA    (BIT(4))
#define CACHE_L1_DCACHE_FAIL_INT_ENA_M  (CACHE_L1_DCACHE_FAIL_INT_ENA_V << CACHE_L1_DCACHE_FAIL_INT_ENA_S)
#define CACHE_L1_DCACHE_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_FAIL_INT_ENA_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_CLR_REG register
 *  L1-Cache Access Fail Interrupt clear register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_CLR_REG (DR_REG_CACHE_BASE + 0x170)
/** CACHE_L1_ICACHE0_FAIL_INT_CLR : WT; bitpos: [0]; default: 0;
 *  The bit is used to clear interrupt of access fail that occurs in L1-ICache0 due to
 *  cpu accesses L1-ICache0.
 */
#define CACHE_L1_ICACHE0_FAIL_INT_CLR    (BIT(0))
#define CACHE_L1_ICACHE0_FAIL_INT_CLR_M  (CACHE_L1_ICACHE0_FAIL_INT_CLR_V << CACHE_L1_ICACHE0_FAIL_INT_CLR_S)
#define CACHE_L1_ICACHE0_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE0_FAIL_INT_CLR_S  0
/** CACHE_L1_ICACHE1_FAIL_INT_CLR : WT; bitpos: [1]; default: 0;
 *  The bit is used to clear interrupt of access fail that occurs in L1-ICache1 due to
 *  cpu accesses L1-ICache1.
 */
#define CACHE_L1_ICACHE1_FAIL_INT_CLR    (BIT(1))
#define CACHE_L1_ICACHE1_FAIL_INT_CLR_M  (CACHE_L1_ICACHE1_FAIL_INT_CLR_V << CACHE_L1_ICACHE1_FAIL_INT_CLR_S)
#define CACHE_L1_ICACHE1_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE1_FAIL_INT_CLR_S  1
/** CACHE_L1_ICACHE2_FAIL_INT_CLR : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FAIL_INT_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_CLR_M  (CACHE_L1_ICACHE2_FAIL_INT_CLR_V << CACHE_L1_ICACHE2_FAIL_INT_CLR_S)
#define CACHE_L1_ICACHE2_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_CLR_S  2
/** CACHE_L1_ICACHE3_FAIL_INT_CLR : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_FAIL_INT_CLR    (BIT(3))
#define CACHE_L1_ICACHE3_FAIL_INT_CLR_M  (CACHE_L1_ICACHE3_FAIL_INT_CLR_V << CACHE_L1_ICACHE3_FAIL_INT_CLR_S)
#define CACHE_L1_ICACHE3_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE3_FAIL_INT_CLR_S  3
/** CACHE_L1_DCACHE_FAIL_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The bit is used to clear interrupt of access fail that occurs in L1-DCache due to
 *  cpu accesses L1-DCache.
 */
#define CACHE_L1_DCACHE_FAIL_INT_CLR    (BIT(4))
#define CACHE_L1_DCACHE_FAIL_INT_CLR_M  (CACHE_L1_DCACHE_FAIL_INT_CLR_V << CACHE_L1_DCACHE_FAIL_INT_CLR_S)
#define CACHE_L1_DCACHE_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L1_DCACHE_FAIL_INT_CLR_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_RAW_REG register
 *  Cache Access Fail Interrupt raw register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_RAW_REG (DR_REG_CACHE_BASE + 0x174)
/** CACHE_L1_ICACHE0_FAIL_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L1-ICache0.
 */
#define CACHE_L1_ICACHE0_FAIL_INT_RAW    (BIT(0))
#define CACHE_L1_ICACHE0_FAIL_INT_RAW_M  (CACHE_L1_ICACHE0_FAIL_INT_RAW_V << CACHE_L1_ICACHE0_FAIL_INT_RAW_S)
#define CACHE_L1_ICACHE0_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE0_FAIL_INT_RAW_S  0
/** CACHE_L1_ICACHE1_FAIL_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L1-ICache1.
 */
#define CACHE_L1_ICACHE1_FAIL_INT_RAW    (BIT(1))
#define CACHE_L1_ICACHE1_FAIL_INT_RAW_M  (CACHE_L1_ICACHE1_FAIL_INT_RAW_V << CACHE_L1_ICACHE1_FAIL_INT_RAW_S)
#define CACHE_L1_ICACHE1_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE1_FAIL_INT_RAW_S  1
/** CACHE_L1_ICACHE2_FAIL_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L1-ICache2.
 */
#define CACHE_L1_ICACHE2_FAIL_INT_RAW    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_RAW_M  (CACHE_L1_ICACHE2_FAIL_INT_RAW_V << CACHE_L1_ICACHE2_FAIL_INT_RAW_S)
#define CACHE_L1_ICACHE2_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_RAW_S  2
/** CACHE_L1_ICACHE3_FAIL_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L1-ICache3.
 */
#define CACHE_L1_ICACHE3_FAIL_INT_RAW    (BIT(3))
#define CACHE_L1_ICACHE3_FAIL_INT_RAW_M  (CACHE_L1_ICACHE3_FAIL_INT_RAW_V << CACHE_L1_ICACHE3_FAIL_INT_RAW_S)
#define CACHE_L1_ICACHE3_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE3_FAIL_INT_RAW_S  3
/** CACHE_L1_DCACHE_FAIL_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L1-DCache.
 */
#define CACHE_L1_DCACHE_FAIL_INT_RAW    (BIT(4))
#define CACHE_L1_DCACHE_FAIL_INT_RAW_M  (CACHE_L1_DCACHE_FAIL_INT_RAW_V << CACHE_L1_DCACHE_FAIL_INT_RAW_S)
#define CACHE_L1_DCACHE_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L1_DCACHE_FAIL_INT_RAW_S  4

/** CACHE_L1_CACHE_ACS_FAIL_INT_ST_REG register
 *  Cache Access Fail Interrupt status register
 */
#define CACHE_L1_CACHE_ACS_FAIL_INT_ST_REG (DR_REG_CACHE_BASE + 0x178)
/** CACHE_L1_ICACHE0_FAIL_INT_ST : RO; bitpos: [0]; default: 0;
 *  The bit indicates the interrupt status of access fail that occurs in L1-ICache0 due
 *  to cpu accesses L1-ICache.
 */
#define CACHE_L1_ICACHE0_FAIL_INT_ST    (BIT(0))
#define CACHE_L1_ICACHE0_FAIL_INT_ST_M  (CACHE_L1_ICACHE0_FAIL_INT_ST_V << CACHE_L1_ICACHE0_FAIL_INT_ST_S)
#define CACHE_L1_ICACHE0_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE0_FAIL_INT_ST_S  0
/** CACHE_L1_ICACHE1_FAIL_INT_ST : RO; bitpos: [1]; default: 0;
 *  The bit indicates the interrupt status of access fail that occurs in L1-ICache1 due
 *  to cpu accesses L1-ICache.
 */
#define CACHE_L1_ICACHE1_FAIL_INT_ST    (BIT(1))
#define CACHE_L1_ICACHE1_FAIL_INT_ST_M  (CACHE_L1_ICACHE1_FAIL_INT_ST_V << CACHE_L1_ICACHE1_FAIL_INT_ST_S)
#define CACHE_L1_ICACHE1_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE1_FAIL_INT_ST_S  1
/** CACHE_L1_ICACHE2_FAIL_INT_ST : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_FAIL_INT_ST    (BIT(2))
#define CACHE_L1_ICACHE2_FAIL_INT_ST_M  (CACHE_L1_ICACHE2_FAIL_INT_ST_V << CACHE_L1_ICACHE2_FAIL_INT_ST_S)
#define CACHE_L1_ICACHE2_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE2_FAIL_INT_ST_S  2
/** CACHE_L1_ICACHE3_FAIL_INT_ST : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_FAIL_INT_ST    (BIT(3))
#define CACHE_L1_ICACHE3_FAIL_INT_ST_M  (CACHE_L1_ICACHE3_FAIL_INT_ST_V << CACHE_L1_ICACHE3_FAIL_INT_ST_S)
#define CACHE_L1_ICACHE3_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE3_FAIL_INT_ST_S  3
/** CACHE_L1_DCACHE_FAIL_INT_ST : RO; bitpos: [4]; default: 0;
 *  The bit indicates the interrupt status of access fail that occurs in L1-DCache due
 *  to cpu accesses L1-DCache.
 */
#define CACHE_L1_DCACHE_FAIL_INT_ST    (BIT(4))
#define CACHE_L1_DCACHE_FAIL_INT_ST_M  (CACHE_L1_DCACHE_FAIL_INT_ST_V << CACHE_L1_DCACHE_FAIL_INT_ST_S)
#define CACHE_L1_DCACHE_FAIL_INT_ST_V  0x00000001U
#define CACHE_L1_DCACHE_FAIL_INT_ST_S  4

/** CACHE_L1_CACHE_ACS_CNT_CTRL_REG register
 *  Cache Access Counter enable and clear register
 */
#define CACHE_L1_CACHE_ACS_CNT_CTRL_REG (DR_REG_CACHE_BASE + 0x17c)
/** CACHE_L1_IBUS0_CNT_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable ibus0 counter in L1-ICache0.
 */
#define CACHE_L1_IBUS0_CNT_ENA    (BIT(0))
#define CACHE_L1_IBUS0_CNT_ENA_M  (CACHE_L1_IBUS0_CNT_ENA_V << CACHE_L1_IBUS0_CNT_ENA_S)
#define CACHE_L1_IBUS0_CNT_ENA_V  0x00000001U
#define CACHE_L1_IBUS0_CNT_ENA_S  0
/** CACHE_L1_IBUS1_CNT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable ibus1 counter in L1-ICache1.
 */
#define CACHE_L1_IBUS1_CNT_ENA    (BIT(1))
#define CACHE_L1_IBUS1_CNT_ENA_M  (CACHE_L1_IBUS1_CNT_ENA_V << CACHE_L1_IBUS1_CNT_ENA_S)
#define CACHE_L1_IBUS1_CNT_ENA_V  0x00000001U
#define CACHE_L1_IBUS1_CNT_ENA_S  1
/** CACHE_L1_IBUS2_CNT_ENA : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_CNT_ENA    (BIT(2))
#define CACHE_L1_IBUS2_CNT_ENA_M  (CACHE_L1_IBUS2_CNT_ENA_V << CACHE_L1_IBUS2_CNT_ENA_S)
#define CACHE_L1_IBUS2_CNT_ENA_V  0x00000001U
#define CACHE_L1_IBUS2_CNT_ENA_S  2
/** CACHE_L1_IBUS3_CNT_ENA : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_CNT_ENA    (BIT(3))
#define CACHE_L1_IBUS3_CNT_ENA_M  (CACHE_L1_IBUS3_CNT_ENA_V << CACHE_L1_IBUS3_CNT_ENA_S)
#define CACHE_L1_IBUS3_CNT_ENA_V  0x00000001U
#define CACHE_L1_IBUS3_CNT_ENA_S  3
/** CACHE_L1_DBUS0_CNT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable dbus0 counter in L1-DCache.
 */
#define CACHE_L1_DBUS0_CNT_ENA    (BIT(4))
#define CACHE_L1_DBUS0_CNT_ENA_M  (CACHE_L1_DBUS0_CNT_ENA_V << CACHE_L1_DBUS0_CNT_ENA_S)
#define CACHE_L1_DBUS0_CNT_ENA_V  0x00000001U
#define CACHE_L1_DBUS0_CNT_ENA_S  4
/** CACHE_L1_DBUS1_CNT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable dbus1 counter in L1-DCache.
 */
#define CACHE_L1_DBUS1_CNT_ENA    (BIT(5))
#define CACHE_L1_DBUS1_CNT_ENA_M  (CACHE_L1_DBUS1_CNT_ENA_V << CACHE_L1_DBUS1_CNT_ENA_S)
#define CACHE_L1_DBUS1_CNT_ENA_V  0x00000001U
#define CACHE_L1_DBUS1_CNT_ENA_S  5
/** CACHE_L1_DBUS2_CNT_ENA : HRO; bitpos: [6]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS2_CNT_ENA    (BIT(6))
#define CACHE_L1_DBUS2_CNT_ENA_M  (CACHE_L1_DBUS2_CNT_ENA_V << CACHE_L1_DBUS2_CNT_ENA_S)
#define CACHE_L1_DBUS2_CNT_ENA_V  0x00000001U
#define CACHE_L1_DBUS2_CNT_ENA_S  6
/** CACHE_L1_DBUS3_CNT_ENA : HRO; bitpos: [7]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS3_CNT_ENA    (BIT(7))
#define CACHE_L1_DBUS3_CNT_ENA_M  (CACHE_L1_DBUS3_CNT_ENA_V << CACHE_L1_DBUS3_CNT_ENA_S)
#define CACHE_L1_DBUS3_CNT_ENA_V  0x00000001U
#define CACHE_L1_DBUS3_CNT_ENA_S  7
/** CACHE_L1_IBUS0_CNT_CLR : WT; bitpos: [16]; default: 0;
 *  The bit is used to clear ibus0 counter in L1-ICache0.
 */
#define CACHE_L1_IBUS0_CNT_CLR    (BIT(16))
#define CACHE_L1_IBUS0_CNT_CLR_M  (CACHE_L1_IBUS0_CNT_CLR_V << CACHE_L1_IBUS0_CNT_CLR_S)
#define CACHE_L1_IBUS0_CNT_CLR_V  0x00000001U
#define CACHE_L1_IBUS0_CNT_CLR_S  16
/** CACHE_L1_IBUS1_CNT_CLR : WT; bitpos: [17]; default: 0;
 *  The bit is used to clear ibus1 counter in L1-ICache1.
 */
#define CACHE_L1_IBUS1_CNT_CLR    (BIT(17))
#define CACHE_L1_IBUS1_CNT_CLR_M  (CACHE_L1_IBUS1_CNT_CLR_V << CACHE_L1_IBUS1_CNT_CLR_S)
#define CACHE_L1_IBUS1_CNT_CLR_V  0x00000001U
#define CACHE_L1_IBUS1_CNT_CLR_S  17
/** CACHE_L1_IBUS2_CNT_CLR : HRO; bitpos: [18]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS2_CNT_CLR    (BIT(18))
#define CACHE_L1_IBUS2_CNT_CLR_M  (CACHE_L1_IBUS2_CNT_CLR_V << CACHE_L1_IBUS2_CNT_CLR_S)
#define CACHE_L1_IBUS2_CNT_CLR_V  0x00000001U
#define CACHE_L1_IBUS2_CNT_CLR_S  18
/** CACHE_L1_IBUS3_CNT_CLR : HRO; bitpos: [19]; default: 0;
 *  Reserved
 */
#define CACHE_L1_IBUS3_CNT_CLR    (BIT(19))
#define CACHE_L1_IBUS3_CNT_CLR_M  (CACHE_L1_IBUS3_CNT_CLR_V << CACHE_L1_IBUS3_CNT_CLR_S)
#define CACHE_L1_IBUS3_CNT_CLR_V  0x00000001U
#define CACHE_L1_IBUS3_CNT_CLR_S  19
/** CACHE_L1_DBUS0_CNT_CLR : WT; bitpos: [20]; default: 0;
 *  The bit is used to clear dbus0 counter in L1-DCache.
 */
#define CACHE_L1_DBUS0_CNT_CLR    (BIT(20))
#define CACHE_L1_DBUS0_CNT_CLR_M  (CACHE_L1_DBUS0_CNT_CLR_V << CACHE_L1_DBUS0_CNT_CLR_S)
#define CACHE_L1_DBUS0_CNT_CLR_V  0x00000001U
#define CACHE_L1_DBUS0_CNT_CLR_S  20
/** CACHE_L1_DBUS1_CNT_CLR : WT; bitpos: [21]; default: 0;
 *  The bit is used to clear dbus1 counter in L1-DCache.
 */
#define CACHE_L1_DBUS1_CNT_CLR    (BIT(21))
#define CACHE_L1_DBUS1_CNT_CLR_M  (CACHE_L1_DBUS1_CNT_CLR_V << CACHE_L1_DBUS1_CNT_CLR_S)
#define CACHE_L1_DBUS1_CNT_CLR_V  0x00000001U
#define CACHE_L1_DBUS1_CNT_CLR_S  21
/** CACHE_L1_DBUS2_CNT_CLR : HRO; bitpos: [22]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS2_CNT_CLR    (BIT(22))
#define CACHE_L1_DBUS2_CNT_CLR_M  (CACHE_L1_DBUS2_CNT_CLR_V << CACHE_L1_DBUS2_CNT_CLR_S)
#define CACHE_L1_DBUS2_CNT_CLR_V  0x00000001U
#define CACHE_L1_DBUS2_CNT_CLR_S  22
/** CACHE_L1_DBUS3_CNT_CLR : HRO; bitpos: [23]; default: 0;
 *  Reserved
 */
#define CACHE_L1_DBUS3_CNT_CLR    (BIT(23))
#define CACHE_L1_DBUS3_CNT_CLR_M  (CACHE_L1_DBUS3_CNT_CLR_V << CACHE_L1_DBUS3_CNT_CLR_S)
#define CACHE_L1_DBUS3_CNT_CLR_V  0x00000001U
#define CACHE_L1_DBUS3_CNT_CLR_S  23

/** CACHE_L1_IBUS0_ACS_HIT_CNT_REG register
 *  L1-ICache bus0 Hit-Access Counter register
 */
#define CACHE_L1_IBUS0_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x180)
/** CACHE_L1_IBUS0_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus0 accesses L1-ICache0.
 */
#define CACHE_L1_IBUS0_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS0_HIT_CNT_M  (CACHE_L1_IBUS0_HIT_CNT_V << CACHE_L1_IBUS0_HIT_CNT_S)
#define CACHE_L1_IBUS0_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS0_HIT_CNT_S  0

/** CACHE_L1_IBUS0_ACS_MISS_CNT_REG register
 *  L1-ICache bus0 Miss-Access Counter register
 */
#define CACHE_L1_IBUS0_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x184)
/** CACHE_L1_IBUS0_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus0 accesses L1-ICache0.
 */
#define CACHE_L1_IBUS0_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS0_MISS_CNT_M  (CACHE_L1_IBUS0_MISS_CNT_V << CACHE_L1_IBUS0_MISS_CNT_S)
#define CACHE_L1_IBUS0_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS0_MISS_CNT_S  0

/** CACHE_L1_IBUS0_ACS_CONFLICT_CNT_REG register
 *  L1-ICache bus0 Conflict-Access Counter register
 */
#define CACHE_L1_IBUS0_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x188)
/** CACHE_L1_IBUS0_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus0 accesses L1-ICache0.
 */
#define CACHE_L1_IBUS0_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS0_CONFLICT_CNT_M  (CACHE_L1_IBUS0_CONFLICT_CNT_V << CACHE_L1_IBUS0_CONFLICT_CNT_S)
#define CACHE_L1_IBUS0_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS0_CONFLICT_CNT_S  0

/** CACHE_L1_IBUS0_ACS_NXTLVL_RD_CNT_REG register
 *  L1-ICache bus0 Next-Level-Access Counter register
 */
#define CACHE_L1_IBUS0_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x18c)
/** CACHE_L1_IBUS0_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-ICache accesses L2-Cache due to
 *  bus0 accessing L1-ICache0.
 */
#define CACHE_L1_IBUS0_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS0_NXTLVL_RD_CNT_M  (CACHE_L1_IBUS0_NXTLVL_RD_CNT_V << CACHE_L1_IBUS0_NXTLVL_RD_CNT_S)
#define CACHE_L1_IBUS0_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS0_NXTLVL_RD_CNT_S  0

/** CACHE_L1_IBUS1_ACS_HIT_CNT_REG register
 *  L1-ICache bus1 Hit-Access Counter register
 */
#define CACHE_L1_IBUS1_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x190)
/** CACHE_L1_IBUS1_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus1 accesses L1-ICache1.
 */
#define CACHE_L1_IBUS1_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS1_HIT_CNT_M  (CACHE_L1_IBUS1_HIT_CNT_V << CACHE_L1_IBUS1_HIT_CNT_S)
#define CACHE_L1_IBUS1_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS1_HIT_CNT_S  0

/** CACHE_L1_IBUS1_ACS_MISS_CNT_REG register
 *  L1-ICache bus1 Miss-Access Counter register
 */
#define CACHE_L1_IBUS1_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x194)
/** CACHE_L1_IBUS1_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus1 accesses L1-ICache1.
 */
#define CACHE_L1_IBUS1_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS1_MISS_CNT_M  (CACHE_L1_IBUS1_MISS_CNT_V << CACHE_L1_IBUS1_MISS_CNT_S)
#define CACHE_L1_IBUS1_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS1_MISS_CNT_S  0

/** CACHE_L1_IBUS1_ACS_CONFLICT_CNT_REG register
 *  L1-ICache bus1 Conflict-Access Counter register
 */
#define CACHE_L1_IBUS1_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x198)
/** CACHE_L1_IBUS1_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus1 accesses L1-ICache1.
 */
#define CACHE_L1_IBUS1_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS1_CONFLICT_CNT_M  (CACHE_L1_IBUS1_CONFLICT_CNT_V << CACHE_L1_IBUS1_CONFLICT_CNT_S)
#define CACHE_L1_IBUS1_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS1_CONFLICT_CNT_S  0

/** CACHE_L1_IBUS1_ACS_NXTLVL_RD_CNT_REG register
 *  L1-ICache bus1 Next-Level-Access Counter register
 */
#define CACHE_L1_IBUS1_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x19c)
/** CACHE_L1_IBUS1_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-ICache accesses L2-Cache due to
 *  bus1 accessing L1-ICache1.
 */
#define CACHE_L1_IBUS1_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS1_NXTLVL_RD_CNT_M  (CACHE_L1_IBUS1_NXTLVL_RD_CNT_V << CACHE_L1_IBUS1_NXTLVL_RD_CNT_S)
#define CACHE_L1_IBUS1_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS1_NXTLVL_RD_CNT_S  0

/** CACHE_L1_IBUS2_ACS_HIT_CNT_REG register
 *  L1-ICache bus2 Hit-Access Counter register
 */
#define CACHE_L1_IBUS2_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1a0)
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
#define CACHE_L1_IBUS2_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1a4)
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
#define CACHE_L1_IBUS2_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1a8)
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
#define CACHE_L1_IBUS2_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1ac)
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
#define CACHE_L1_IBUS3_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1b0)
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
#define CACHE_L1_IBUS3_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1b4)
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
#define CACHE_L1_IBUS3_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1b8)
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
#define CACHE_L1_IBUS3_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1bc)
/** CACHE_L1_IBUS3_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-ICache accesses L2-Cache due to
 *  bus3 accessing L1-ICache3.
 */
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT_M  (CACHE_L1_IBUS3_NXTLVL_RD_CNT_V << CACHE_L1_IBUS3_NXTLVL_RD_CNT_S)
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_IBUS3_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS0_ACS_HIT_CNT_REG register
 *  L1-DCache bus0 Hit-Access Counter register
 */
#define CACHE_L1_DBUS0_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1c0)
/** CACHE_L1_DBUS0_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS0_HIT_CNT_M  (CACHE_L1_DBUS0_HIT_CNT_V << CACHE_L1_DBUS0_HIT_CNT_S)
#define CACHE_L1_DBUS0_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS0_HIT_CNT_S  0

/** CACHE_L1_DBUS0_ACS_MISS_CNT_REG register
 *  L1-DCache bus0 Miss-Access Counter register
 */
#define CACHE_L1_DBUS0_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1c4)
/** CACHE_L1_DBUS0_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS0_MISS_CNT_M  (CACHE_L1_DBUS0_MISS_CNT_V << CACHE_L1_DBUS0_MISS_CNT_S)
#define CACHE_L1_DBUS0_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS0_MISS_CNT_S  0

/** CACHE_L1_DBUS0_ACS_CONFLICT_CNT_REG register
 *  L1-DCache bus0 Conflict-Access Counter register
 */
#define CACHE_L1_DBUS0_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1c8)
/** CACHE_L1_DBUS0_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS0_CONFLICT_CNT_M  (CACHE_L1_DBUS0_CONFLICT_CNT_V << CACHE_L1_DBUS0_CONFLICT_CNT_S)
#define CACHE_L1_DBUS0_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS0_CONFLICT_CNT_S  0

/** CACHE_L1_DBUS0_ACS_NXTLVL_RD_CNT_REG register
 *  L1-DCache bus0 Next-Level-Access Counter register
 */
#define CACHE_L1_DBUS0_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1cc)
/** CACHE_L1_DBUS0_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-DCache accesses L2-Cache due to
 *  bus0 accessing L1-DCache.
 */
#define CACHE_L1_DBUS0_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS0_NXTLVL_RD_CNT_M  (CACHE_L1_DBUS0_NXTLVL_RD_CNT_V << CACHE_L1_DBUS0_NXTLVL_RD_CNT_S)
#define CACHE_L1_DBUS0_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS0_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS0_ACS_NXTLVL_WR_CNT_REG register
 *  L1-DCache bus0 WB-Access Counter register
 */
#define CACHE_L1_DBUS0_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x1d0)
/** CACHE_L1_DBUS0_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS0_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS0_NXTLVL_WR_CNT_M  (CACHE_L1_DBUS0_NXTLVL_WR_CNT_V << CACHE_L1_DBUS0_NXTLVL_WR_CNT_S)
#define CACHE_L1_DBUS0_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS0_NXTLVL_WR_CNT_S  0

/** CACHE_L1_DBUS1_ACS_HIT_CNT_REG register
 *  L1-DCache bus1 Hit-Access Counter register
 */
#define CACHE_L1_DBUS1_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1d4)
/** CACHE_L1_DBUS1_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS1_HIT_CNT_M  (CACHE_L1_DBUS1_HIT_CNT_V << CACHE_L1_DBUS1_HIT_CNT_S)
#define CACHE_L1_DBUS1_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS1_HIT_CNT_S  0

/** CACHE_L1_DBUS1_ACS_MISS_CNT_REG register
 *  L1-DCache bus1 Miss-Access Counter register
 */
#define CACHE_L1_DBUS1_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1d8)
/** CACHE_L1_DBUS1_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS1_MISS_CNT_M  (CACHE_L1_DBUS1_MISS_CNT_V << CACHE_L1_DBUS1_MISS_CNT_S)
#define CACHE_L1_DBUS1_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS1_MISS_CNT_S  0

/** CACHE_L1_DBUS1_ACS_CONFLICT_CNT_REG register
 *  L1-DCache bus1 Conflict-Access Counter register
 */
#define CACHE_L1_DBUS1_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1dc)
/** CACHE_L1_DBUS1_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS1_CONFLICT_CNT_M  (CACHE_L1_DBUS1_CONFLICT_CNT_V << CACHE_L1_DBUS1_CONFLICT_CNT_S)
#define CACHE_L1_DBUS1_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS1_CONFLICT_CNT_S  0

/** CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG register
 *  L1-DCache bus1 Next-Level-Access Counter register
 */
#define CACHE_L1_DBUS1_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1e0)
/** CACHE_L1_DBUS1_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-DCache accesses L2-Cache due to
 *  bus1 accessing L1-DCache.
 */
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT_M  (CACHE_L1_DBUS1_NXTLVL_RD_CNT_V << CACHE_L1_DBUS1_NXTLVL_RD_CNT_S)
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG register
 *  L1-DCache bus1 WB-Access Counter register
 */
#define CACHE_L1_DBUS1_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x1e4)
/** CACHE_L1_DBUS1_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when bus1 accesses L1-DCache.
 */
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT_M  (CACHE_L1_DBUS1_NXTLVL_WR_CNT_V << CACHE_L1_DBUS1_NXTLVL_WR_CNT_S)
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS1_NXTLVL_WR_CNT_S  0

/** CACHE_L1_DBUS2_ACS_HIT_CNT_REG register
 *  L1-DCache bus2 Hit-Access Counter register
 */
#define CACHE_L1_DBUS2_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1e8)
/** CACHE_L1_DBUS2_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus2 accesses L1-DCache.
 */
#define CACHE_L1_DBUS2_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS2_HIT_CNT_M  (CACHE_L1_DBUS2_HIT_CNT_V << CACHE_L1_DBUS2_HIT_CNT_S)
#define CACHE_L1_DBUS2_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS2_HIT_CNT_S  0

/** CACHE_L1_DBUS2_ACS_MISS_CNT_REG register
 *  L1-DCache bus2 Miss-Access Counter register
 */
#define CACHE_L1_DBUS2_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x1ec)
/** CACHE_L1_DBUS2_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus2 accesses L1-DCache.
 */
#define CACHE_L1_DBUS2_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS2_MISS_CNT_M  (CACHE_L1_DBUS2_MISS_CNT_V << CACHE_L1_DBUS2_MISS_CNT_S)
#define CACHE_L1_DBUS2_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS2_MISS_CNT_S  0

/** CACHE_L1_DBUS2_ACS_CONFLICT_CNT_REG register
 *  L1-DCache bus2 Conflict-Access Counter register
 */
#define CACHE_L1_DBUS2_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x1f0)
/** CACHE_L1_DBUS2_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus2 accesses L1-DCache.
 */
#define CACHE_L1_DBUS2_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS2_CONFLICT_CNT_M  (CACHE_L1_DBUS2_CONFLICT_CNT_V << CACHE_L1_DBUS2_CONFLICT_CNT_S)
#define CACHE_L1_DBUS2_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS2_CONFLICT_CNT_S  0

/** CACHE_L1_DBUS2_ACS_NXTLVL_RD_CNT_REG register
 *  L1-DCache bus2 Next-Level-Access Counter register
 */
#define CACHE_L1_DBUS2_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x1f4)
/** CACHE_L1_DBUS2_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-DCache accesses L2-Cache due to
 *  bus2 accessing L1-DCache.
 */
#define CACHE_L1_DBUS2_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS2_NXTLVL_RD_CNT_M  (CACHE_L1_DBUS2_NXTLVL_RD_CNT_V << CACHE_L1_DBUS2_NXTLVL_RD_CNT_S)
#define CACHE_L1_DBUS2_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS2_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS2_ACS_NXTLVL_WR_CNT_REG register
 *  L1-DCache bus2 WB-Access Counter register
 */
#define CACHE_L1_DBUS2_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x1f8)
/** CACHE_L1_DBUS2_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when bus2 accesses L1-DCache.
 */
#define CACHE_L1_DBUS2_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS2_NXTLVL_WR_CNT_M  (CACHE_L1_DBUS2_NXTLVL_WR_CNT_V << CACHE_L1_DBUS2_NXTLVL_WR_CNT_S)
#define CACHE_L1_DBUS2_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS2_NXTLVL_WR_CNT_S  0

/** CACHE_L1_DBUS3_ACS_HIT_CNT_REG register
 *  L1-DCache bus3 Hit-Access Counter register
 */
#define CACHE_L1_DBUS3_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x1fc)
/** CACHE_L1_DBUS3_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when bus3 accesses L1-DCache.
 */
#define CACHE_L1_DBUS3_HIT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS3_HIT_CNT_M  (CACHE_L1_DBUS3_HIT_CNT_V << CACHE_L1_DBUS3_HIT_CNT_S)
#define CACHE_L1_DBUS3_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS3_HIT_CNT_S  0

/** CACHE_L1_DBUS3_ACS_MISS_CNT_REG register
 *  L1-DCache bus3 Miss-Access Counter register
 */
#define CACHE_L1_DBUS3_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x200)
/** CACHE_L1_DBUS3_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when bus3 accesses L1-DCache.
 */
#define CACHE_L1_DBUS3_MISS_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS3_MISS_CNT_M  (CACHE_L1_DBUS3_MISS_CNT_V << CACHE_L1_DBUS3_MISS_CNT_S)
#define CACHE_L1_DBUS3_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS3_MISS_CNT_S  0

/** CACHE_L1_DBUS3_ACS_CONFLICT_CNT_REG register
 *  L1-DCache bus3 Conflict-Access Counter register
 */
#define CACHE_L1_DBUS3_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x204)
/** CACHE_L1_DBUS3_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when bus3 accesses L1-DCache.
 */
#define CACHE_L1_DBUS3_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS3_CONFLICT_CNT_M  (CACHE_L1_DBUS3_CONFLICT_CNT_V << CACHE_L1_DBUS3_CONFLICT_CNT_S)
#define CACHE_L1_DBUS3_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS3_CONFLICT_CNT_S  0

/** CACHE_L1_DBUS3_ACS_NXTLVL_RD_CNT_REG register
 *  L1-DCache bus3 Next-Level-Access Counter register
 */
#define CACHE_L1_DBUS3_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x208)
/** CACHE_L1_DBUS3_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L1-DCache accesses L2-Cache due to
 *  bus3 accessing L1-DCache.
 */
#define CACHE_L1_DBUS3_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS3_NXTLVL_RD_CNT_M  (CACHE_L1_DBUS3_NXTLVL_RD_CNT_V << CACHE_L1_DBUS3_NXTLVL_RD_CNT_S)
#define CACHE_L1_DBUS3_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS3_NXTLVL_RD_CNT_S  0

/** CACHE_L1_DBUS3_ACS_NXTLVL_WR_CNT_REG register
 *  L1-DCache bus3 WB-Access Counter register
 */
#define CACHE_L1_DBUS3_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x20c)
/** CACHE_L1_DBUS3_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when bus0 accesses L1-DCache.
 */
#define CACHE_L1_DBUS3_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L1_DBUS3_NXTLVL_WR_CNT_M  (CACHE_L1_DBUS3_NXTLVL_WR_CNT_V << CACHE_L1_DBUS3_NXTLVL_WR_CNT_S)
#define CACHE_L1_DBUS3_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L1_DBUS3_NXTLVL_WR_CNT_S  0

/** CACHE_L1_ICACHE0_ACS_FAIL_ID_ATTR_REG register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
#define CACHE_L1_ICACHE0_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x210)
/** CACHE_L1_ICACHE0_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  The register records the ID of fail-access when cache0 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE0_FAIL_ID    0x0000FFFFU
#define CACHE_L1_ICACHE0_FAIL_ID_M  (CACHE_L1_ICACHE0_FAIL_ID_V << CACHE_L1_ICACHE0_FAIL_ID_S)
#define CACHE_L1_ICACHE0_FAIL_ID_V  0x0000FFFFU
#define CACHE_L1_ICACHE0_FAIL_ID_S  0
/** CACHE_L1_ICACHE0_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  The register records the attribution of fail-access when cache0 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE0_FAIL_ATTR    0x0000FFFFU
#define CACHE_L1_ICACHE0_FAIL_ATTR_M  (CACHE_L1_ICACHE0_FAIL_ATTR_V << CACHE_L1_ICACHE0_FAIL_ATTR_S)
#define CACHE_L1_ICACHE0_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L1_ICACHE0_FAIL_ATTR_S  16

/** CACHE_L1_ICACHE0_ACS_FAIL_ADDR_REG register
 *  L1-ICache0 Access Fail Address information register
 */
#define CACHE_L1_ICACHE0_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x214)
/** CACHE_L1_ICACHE0_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when cache0 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE0_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE0_FAIL_ADDR_M  (CACHE_L1_ICACHE0_FAIL_ADDR_V << CACHE_L1_ICACHE0_FAIL_ADDR_S)
#define CACHE_L1_ICACHE0_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE0_FAIL_ADDR_S  0

/** CACHE_L1_ICACHE1_ACS_FAIL_ID_ATTR_REG register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
#define CACHE_L1_ICACHE1_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x218)
/** CACHE_L1_ICACHE1_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  The register records the ID of fail-access when cache1 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE1_FAIL_ID    0x0000FFFFU
#define CACHE_L1_ICACHE1_FAIL_ID_M  (CACHE_L1_ICACHE1_FAIL_ID_V << CACHE_L1_ICACHE1_FAIL_ID_S)
#define CACHE_L1_ICACHE1_FAIL_ID_V  0x0000FFFFU
#define CACHE_L1_ICACHE1_FAIL_ID_S  0
/** CACHE_L1_ICACHE1_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  The register records the attribution of fail-access when cache1 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE1_FAIL_ATTR    0x0000FFFFU
#define CACHE_L1_ICACHE1_FAIL_ATTR_M  (CACHE_L1_ICACHE1_FAIL_ATTR_V << CACHE_L1_ICACHE1_FAIL_ATTR_S)
#define CACHE_L1_ICACHE1_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L1_ICACHE1_FAIL_ATTR_S  16

/** CACHE_L1_ICACHE1_ACS_FAIL_ADDR_REG register
 *  L1-ICache0 Access Fail Address information register
 */
#define CACHE_L1_ICACHE1_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x21c)
/** CACHE_L1_ICACHE1_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when cache1 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE1_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE1_FAIL_ADDR_M  (CACHE_L1_ICACHE1_FAIL_ADDR_V << CACHE_L1_ICACHE1_FAIL_ADDR_S)
#define CACHE_L1_ICACHE1_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE1_FAIL_ADDR_S  0

/** CACHE_L1_ICACHE2_ACS_FAIL_ID_ATTR_REG register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
#define CACHE_L1_ICACHE2_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x220)
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
#define CACHE_L1_ICACHE2_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x224)
/** CACHE_L1_ICACHE2_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when cache2 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE2_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE2_FAIL_ADDR_M  (CACHE_L1_ICACHE2_FAIL_ADDR_V << CACHE_L1_ICACHE2_FAIL_ADDR_S)
#define CACHE_L1_ICACHE2_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE2_FAIL_ADDR_S  0

/** CACHE_L1_ICACHE3_ACS_FAIL_ID_ATTR_REG register
 *  L1-ICache0 Access Fail ID/attribution information register
 */
#define CACHE_L1_ICACHE3_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x228)
/** CACHE_L1_ICACHE3_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  The register records the ID of fail-access when cache3 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE3_FAIL_ID    0x0000FFFFU
#define CACHE_L1_ICACHE3_FAIL_ID_M  (CACHE_L1_ICACHE3_FAIL_ID_V << CACHE_L1_ICACHE3_FAIL_ID_S)
#define CACHE_L1_ICACHE3_FAIL_ID_V  0x0000FFFFU
#define CACHE_L1_ICACHE3_FAIL_ID_S  0
/** CACHE_L1_ICACHE3_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  The register records the attribution of fail-access when cache3 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE3_FAIL_ATTR    0x0000FFFFU
#define CACHE_L1_ICACHE3_FAIL_ATTR_M  (CACHE_L1_ICACHE3_FAIL_ATTR_V << CACHE_L1_ICACHE3_FAIL_ATTR_S)
#define CACHE_L1_ICACHE3_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L1_ICACHE3_FAIL_ATTR_S  16

/** CACHE_L1_ICACHE3_ACS_FAIL_ADDR_REG register
 *  L1-ICache0 Access Fail Address information register
 */
#define CACHE_L1_ICACHE3_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x22c)
/** CACHE_L1_ICACHE3_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when cache3 accesses L1-ICache.
 */
#define CACHE_L1_ICACHE3_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_ICACHE3_FAIL_ADDR_M  (CACHE_L1_ICACHE3_FAIL_ADDR_V << CACHE_L1_ICACHE3_FAIL_ADDR_S)
#define CACHE_L1_ICACHE3_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_ICACHE3_FAIL_ADDR_S  0

/** CACHE_L1_DCACHE_ACS_FAIL_ID_ATTR_REG register
 *  L1-DCache Access Fail ID/attribution information register
 */
#define CACHE_L1_DCACHE_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x230)
/** CACHE_L1_DCACHE_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  The register records the ID of fail-access when cache accesses L1-DCache.
 */
#define CACHE_L1_DCACHE_FAIL_ID    0x0000FFFFU
#define CACHE_L1_DCACHE_FAIL_ID_M  (CACHE_L1_DCACHE_FAIL_ID_V << CACHE_L1_DCACHE_FAIL_ID_S)
#define CACHE_L1_DCACHE_FAIL_ID_V  0x0000FFFFU
#define CACHE_L1_DCACHE_FAIL_ID_S  0
/** CACHE_L1_DCACHE_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  The register records the attribution of fail-access when cache accesses L1-DCache.
 */
#define CACHE_L1_DCACHE_FAIL_ATTR    0x0000FFFFU
#define CACHE_L1_DCACHE_FAIL_ATTR_M  (CACHE_L1_DCACHE_FAIL_ATTR_V << CACHE_L1_DCACHE_FAIL_ATTR_S)
#define CACHE_L1_DCACHE_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L1_DCACHE_FAIL_ATTR_S  16

/** CACHE_L1_DCACHE_ACS_FAIL_ADDR_REG register
 *  L1-DCache Access Fail Address information register
 */
#define CACHE_L1_DCACHE_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x234)
/** CACHE_L1_DCACHE_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when cache accesses L1-DCache.
 */
#define CACHE_L1_DCACHE_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L1_DCACHE_FAIL_ADDR_M  (CACHE_L1_DCACHE_FAIL_ADDR_V << CACHE_L1_DCACHE_FAIL_ADDR_S)
#define CACHE_L1_DCACHE_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L1_DCACHE_FAIL_ADDR_S  0

/** CACHE_SYNC_L1_CACHE_PRELOAD_INT_ENA_REG register
 *  L1-Cache Access Fail Interrupt enable register
 */
#define CACHE_SYNC_L1_CACHE_PRELOAD_INT_ENA_REG (DR_REG_CACHE_BASE + 0x238)
/** CACHE_L1_ICACHE0_PLD_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable interrupt of L1-ICache0 preload-operation. If preload
 *  operation is done, interrupt occurs.
 */
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ENA    (BIT(0))
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ENA_M  (CACHE_L1_ICACHE0_PLD_DONE_INT_ENA_V << CACHE_L1_ICACHE0_PLD_DONE_INT_ENA_S)
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ENA_S  0
/** CACHE_L1_ICACHE1_PLD_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable interrupt of L1-ICache1 preload-operation. If preload
 *  operation is done, interrupt occurs.
 */
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ENA    (BIT(1))
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ENA_M  (CACHE_L1_ICACHE1_PLD_DONE_INT_ENA_V << CACHE_L1_ICACHE1_PLD_DONE_INT_ENA_S)
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ENA_S  1
/** CACHE_L1_ICACHE2_PLD_DONE_INT_ENA : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_V << CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ENA_S  2
/** CACHE_L1_ICACHE3_PLD_DONE_INT_ENA : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ENA    (BIT(3))
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ENA_M  (CACHE_L1_ICACHE3_PLD_DONE_INT_ENA_V << CACHE_L1_ICACHE3_PLD_DONE_INT_ENA_S)
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ENA_S  3
/** CACHE_L1_DCACHE_PLD_DONE_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable interrupt of L1-DCache preload-operation. If preload
 *  operation is done, interrupt occurs.
 */
#define CACHE_L1_DCACHE_PLD_DONE_INT_ENA    (BIT(4))
#define CACHE_L1_DCACHE_PLD_DONE_INT_ENA_M  (CACHE_L1_DCACHE_PLD_DONE_INT_ENA_V << CACHE_L1_DCACHE_PLD_DONE_INT_ENA_S)
#define CACHE_L1_DCACHE_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_DONE_INT_ENA_S  4
/** CACHE_SYNC_DONE_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The bit is used to enable interrupt of Cache sync-operation done.
 */
#define CACHE_SYNC_DONE_INT_ENA    (BIT(6))
#define CACHE_SYNC_DONE_INT_ENA_M  (CACHE_SYNC_DONE_INT_ENA_V << CACHE_SYNC_DONE_INT_ENA_S)
#define CACHE_SYNC_DONE_INT_ENA_V  0x00000001U
#define CACHE_SYNC_DONE_INT_ENA_S  6
/** CACHE_L1_ICACHE0_PLD_ERR_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The bit is used to enable interrupt of L1-ICache0 preload-operation error.
 */
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ENA    (BIT(7))
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ENA_M  (CACHE_L1_ICACHE0_PLD_ERR_INT_ENA_V << CACHE_L1_ICACHE0_PLD_ERR_INT_ENA_S)
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ENA_S  7
/** CACHE_L1_ICACHE1_PLD_ERR_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable interrupt of L1-ICache1 preload-operation error.
 */
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ENA    (BIT(8))
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ENA_M  (CACHE_L1_ICACHE1_PLD_ERR_INT_ENA_V << CACHE_L1_ICACHE1_PLD_ERR_INT_ENA_S)
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ENA_S  8
/** CACHE_L1_ICACHE2_PLD_ERR_INT_ENA : HRO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_V << CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ENA_S  9
/** CACHE_L1_ICACHE3_PLD_ERR_INT_ENA : HRO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ENA    (BIT(10))
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ENA_M  (CACHE_L1_ICACHE3_PLD_ERR_INT_ENA_V << CACHE_L1_ICACHE3_PLD_ERR_INT_ENA_S)
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ENA_S  10
/** CACHE_L1_DCACHE_PLD_ERR_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The bit is used to enable interrupt of L1-DCache preload-operation error.
 */
#define CACHE_L1_DCACHE_PLD_ERR_INT_ENA    (BIT(11))
#define CACHE_L1_DCACHE_PLD_ERR_INT_ENA_M  (CACHE_L1_DCACHE_PLD_ERR_INT_ENA_V << CACHE_L1_DCACHE_PLD_ERR_INT_ENA_S)
#define CACHE_L1_DCACHE_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_ERR_INT_ENA_S  11
/** CACHE_SYNC_ERR_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The bit is used to enable interrupt of Cache sync-operation error.
 */
#define CACHE_SYNC_ERR_INT_ENA    (BIT(13))
#define CACHE_SYNC_ERR_INT_ENA_M  (CACHE_SYNC_ERR_INT_ENA_V << CACHE_SYNC_ERR_INT_ENA_S)
#define CACHE_SYNC_ERR_INT_ENA_V  0x00000001U
#define CACHE_SYNC_ERR_INT_ENA_S  13

/** CACHE_SYNC_L1_CACHE_PRELOAD_INT_CLR_REG register
 *  Sync Preload operation Interrupt clear register
 */
#define CACHE_SYNC_L1_CACHE_PRELOAD_INT_CLR_REG (DR_REG_CACHE_BASE + 0x23c)
/** CACHE_L1_ICACHE0_PLD_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  The bit is used to clear interrupt that occurs only when L1-ICache0
 *  preload-operation is done.
 */
#define CACHE_L1_ICACHE0_PLD_DONE_INT_CLR    (BIT(0))
#define CACHE_L1_ICACHE0_PLD_DONE_INT_CLR_M  (CACHE_L1_ICACHE0_PLD_DONE_INT_CLR_V << CACHE_L1_ICACHE0_PLD_DONE_INT_CLR_S)
#define CACHE_L1_ICACHE0_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_DONE_INT_CLR_S  0
/** CACHE_L1_ICACHE1_PLD_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  The bit is used to clear interrupt that occurs only when L1-ICache1
 *  preload-operation is done.
 */
#define CACHE_L1_ICACHE1_PLD_DONE_INT_CLR    (BIT(1))
#define CACHE_L1_ICACHE1_PLD_DONE_INT_CLR_M  (CACHE_L1_ICACHE1_PLD_DONE_INT_CLR_V << CACHE_L1_ICACHE1_PLD_DONE_INT_CLR_S)
#define CACHE_L1_ICACHE1_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_DONE_INT_CLR_S  1
/** CACHE_L1_ICACHE2_PLD_DONE_INT_CLR : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_V << CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_CLR_S  2
/** CACHE_L1_ICACHE3_PLD_DONE_INT_CLR : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_DONE_INT_CLR    (BIT(3))
#define CACHE_L1_ICACHE3_PLD_DONE_INT_CLR_M  (CACHE_L1_ICACHE3_PLD_DONE_INT_CLR_V << CACHE_L1_ICACHE3_PLD_DONE_INT_CLR_S)
#define CACHE_L1_ICACHE3_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_DONE_INT_CLR_S  3
/** CACHE_L1_DCACHE_PLD_DONE_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The bit is used to clear interrupt that occurs only when L1-DCache
 *  preload-operation is done.
 */
#define CACHE_L1_DCACHE_PLD_DONE_INT_CLR    (BIT(4))
#define CACHE_L1_DCACHE_PLD_DONE_INT_CLR_M  (CACHE_L1_DCACHE_PLD_DONE_INT_CLR_V << CACHE_L1_DCACHE_PLD_DONE_INT_CLR_S)
#define CACHE_L1_DCACHE_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_DONE_INT_CLR_S  4
/** CACHE_SYNC_DONE_INT_CLR : WT; bitpos: [6]; default: 0;
 *  The bit is used to clear interrupt that occurs only when Cache sync-operation is
 *  done.
 */
#define CACHE_SYNC_DONE_INT_CLR    (BIT(6))
#define CACHE_SYNC_DONE_INT_CLR_M  (CACHE_SYNC_DONE_INT_CLR_V << CACHE_SYNC_DONE_INT_CLR_S)
#define CACHE_SYNC_DONE_INT_CLR_V  0x00000001U
#define CACHE_SYNC_DONE_INT_CLR_S  6
/** CACHE_L1_ICACHE0_PLD_ERR_INT_CLR : WT; bitpos: [7]; default: 0;
 *  The bit is used to clear interrupt of L1-ICache0 preload-operation error.
 */
#define CACHE_L1_ICACHE0_PLD_ERR_INT_CLR    (BIT(7))
#define CACHE_L1_ICACHE0_PLD_ERR_INT_CLR_M  (CACHE_L1_ICACHE0_PLD_ERR_INT_CLR_V << CACHE_L1_ICACHE0_PLD_ERR_INT_CLR_S)
#define CACHE_L1_ICACHE0_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_ERR_INT_CLR_S  7
/** CACHE_L1_ICACHE1_PLD_ERR_INT_CLR : WT; bitpos: [8]; default: 0;
 *  The bit is used to clear interrupt of L1-ICache1 preload-operation error.
 */
#define CACHE_L1_ICACHE1_PLD_ERR_INT_CLR    (BIT(8))
#define CACHE_L1_ICACHE1_PLD_ERR_INT_CLR_M  (CACHE_L1_ICACHE1_PLD_ERR_INT_CLR_V << CACHE_L1_ICACHE1_PLD_ERR_INT_CLR_S)
#define CACHE_L1_ICACHE1_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_ERR_INT_CLR_S  8
/** CACHE_L1_ICACHE2_PLD_ERR_INT_CLR : HRO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_V << CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_CLR_S  9
/** CACHE_L1_ICACHE3_PLD_ERR_INT_CLR : HRO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_ERR_INT_CLR    (BIT(10))
#define CACHE_L1_ICACHE3_PLD_ERR_INT_CLR_M  (CACHE_L1_ICACHE3_PLD_ERR_INT_CLR_V << CACHE_L1_ICACHE3_PLD_ERR_INT_CLR_S)
#define CACHE_L1_ICACHE3_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_ERR_INT_CLR_S  10
/** CACHE_L1_DCACHE_PLD_ERR_INT_CLR : WT; bitpos: [11]; default: 0;
 *  The bit is used to clear interrupt of L1-DCache preload-operation error.
 */
#define CACHE_L1_DCACHE_PLD_ERR_INT_CLR    (BIT(11))
#define CACHE_L1_DCACHE_PLD_ERR_INT_CLR_M  (CACHE_L1_DCACHE_PLD_ERR_INT_CLR_V << CACHE_L1_DCACHE_PLD_ERR_INT_CLR_S)
#define CACHE_L1_DCACHE_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_ERR_INT_CLR_S  11
/** CACHE_SYNC_ERR_INT_CLR : WT; bitpos: [13]; default: 0;
 *  The bit is used to clear interrupt of Cache sync-operation error.
 */
#define CACHE_SYNC_ERR_INT_CLR    (BIT(13))
#define CACHE_SYNC_ERR_INT_CLR_M  (CACHE_SYNC_ERR_INT_CLR_V << CACHE_SYNC_ERR_INT_CLR_S)
#define CACHE_SYNC_ERR_INT_CLR_V  0x00000001U
#define CACHE_SYNC_ERR_INT_CLR_S  13

/** CACHE_SYNC_L1_CACHE_PRELOAD_INT_RAW_REG register
 *  Sync Preload operation Interrupt raw register
 */
#define CACHE_SYNC_L1_CACHE_PRELOAD_INT_RAW_REG (DR_REG_CACHE_BASE + 0x240)
/** CACHE_L1_ICACHE0_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-ICache0 preload-operation is
 *  done.
 */
#define CACHE_L1_ICACHE0_PLD_DONE_INT_RAW    (BIT(0))
#define CACHE_L1_ICACHE0_PLD_DONE_INT_RAW_M  (CACHE_L1_ICACHE0_PLD_DONE_INT_RAW_V << CACHE_L1_ICACHE0_PLD_DONE_INT_RAW_S)
#define CACHE_L1_ICACHE0_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_DONE_INT_RAW_S  0
/** CACHE_L1_ICACHE1_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-ICache1 preload-operation is
 *  done.
 */
#define CACHE_L1_ICACHE1_PLD_DONE_INT_RAW    (BIT(1))
#define CACHE_L1_ICACHE1_PLD_DONE_INT_RAW_M  (CACHE_L1_ICACHE1_PLD_DONE_INT_RAW_V << CACHE_L1_ICACHE1_PLD_DONE_INT_RAW_S)
#define CACHE_L1_ICACHE1_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_DONE_INT_RAW_S  1
/** CACHE_L1_ICACHE2_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_V << CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_RAW_S  2
/** CACHE_L1_ICACHE3_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_DONE_INT_RAW    (BIT(3))
#define CACHE_L1_ICACHE3_PLD_DONE_INT_RAW_M  (CACHE_L1_ICACHE3_PLD_DONE_INT_RAW_V << CACHE_L1_ICACHE3_PLD_DONE_INT_RAW_S)
#define CACHE_L1_ICACHE3_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_DONE_INT_RAW_S  3
/** CACHE_L1_DCACHE_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-DCache preload-operation is
 *  done.
 */
#define CACHE_L1_DCACHE_PLD_DONE_INT_RAW    (BIT(4))
#define CACHE_L1_DCACHE_PLD_DONE_INT_RAW_M  (CACHE_L1_DCACHE_PLD_DONE_INT_RAW_V << CACHE_L1_DCACHE_PLD_DONE_INT_RAW_S)
#define CACHE_L1_DCACHE_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_DONE_INT_RAW_S  4
/** CACHE_SYNC_DONE_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw bit of the interrupt that occurs only when Cache sync-operation is done.
 */
#define CACHE_SYNC_DONE_INT_RAW    (BIT(6))
#define CACHE_SYNC_DONE_INT_RAW_M  (CACHE_SYNC_DONE_INT_RAW_V << CACHE_SYNC_DONE_INT_RAW_S)
#define CACHE_SYNC_DONE_INT_RAW_V  0x00000001U
#define CACHE_SYNC_DONE_INT_RAW_S  6
/** CACHE_L1_ICACHE0_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-ICache0 preload-operation
 *  error occurs.
 */
#define CACHE_L1_ICACHE0_PLD_ERR_INT_RAW    (BIT(7))
#define CACHE_L1_ICACHE0_PLD_ERR_INT_RAW_M  (CACHE_L1_ICACHE0_PLD_ERR_INT_RAW_V << CACHE_L1_ICACHE0_PLD_ERR_INT_RAW_S)
#define CACHE_L1_ICACHE0_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_ERR_INT_RAW_S  7
/** CACHE_L1_ICACHE1_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-ICache1 preload-operation
 *  error occurs.
 */
#define CACHE_L1_ICACHE1_PLD_ERR_INT_RAW    (BIT(8))
#define CACHE_L1_ICACHE1_PLD_ERR_INT_RAW_M  (CACHE_L1_ICACHE1_PLD_ERR_INT_RAW_V << CACHE_L1_ICACHE1_PLD_ERR_INT_RAW_S)
#define CACHE_L1_ICACHE1_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_ERR_INT_RAW_S  8
/** CACHE_L1_ICACHE2_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_V << CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_RAW_S  9
/** CACHE_L1_ICACHE3_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_ERR_INT_RAW    (BIT(10))
#define CACHE_L1_ICACHE3_PLD_ERR_INT_RAW_M  (CACHE_L1_ICACHE3_PLD_ERR_INT_RAW_V << CACHE_L1_ICACHE3_PLD_ERR_INT_RAW_S)
#define CACHE_L1_ICACHE3_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_ERR_INT_RAW_S  10
/** CACHE_L1_DCACHE_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw bit of the interrupt that occurs only when L1-DCache preload-operation
 *  error occurs.
 */
#define CACHE_L1_DCACHE_PLD_ERR_INT_RAW    (BIT(11))
#define CACHE_L1_DCACHE_PLD_ERR_INT_RAW_M  (CACHE_L1_DCACHE_PLD_ERR_INT_RAW_V << CACHE_L1_DCACHE_PLD_ERR_INT_RAW_S)
#define CACHE_L1_DCACHE_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_ERR_INT_RAW_S  11
/** CACHE_SYNC_ERR_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw bit of the interrupt that occurs only when Cache sync-operation error
 *  occurs.
 */
#define CACHE_SYNC_ERR_INT_RAW    (BIT(13))
#define CACHE_SYNC_ERR_INT_RAW_M  (CACHE_SYNC_ERR_INT_RAW_V << CACHE_SYNC_ERR_INT_RAW_S)
#define CACHE_SYNC_ERR_INT_RAW_V  0x00000001U
#define CACHE_SYNC_ERR_INT_RAW_S  13

/** CACHE_SYNC_L1_CACHE_PRELOAD_INT_ST_REG register
 *  L1-Cache Access Fail Interrupt status register
 */
#define CACHE_SYNC_L1_CACHE_PRELOAD_INT_ST_REG (DR_REG_CACHE_BASE + 0x244)
/** CACHE_L1_ICACHE0_PLD_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The bit indicates the status of the interrupt that occurs only when L1-ICache0
 *  preload-operation is done.
 */
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ST    (BIT(0))
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ST_M  (CACHE_L1_ICACHE0_PLD_DONE_INT_ST_V << CACHE_L1_ICACHE0_PLD_DONE_INT_ST_S)
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_DONE_INT_ST_S  0
/** CACHE_L1_ICACHE1_PLD_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The bit indicates the status of the interrupt that occurs only when L1-ICache1
 *  preload-operation is done.
 */
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ST    (BIT(1))
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ST_M  (CACHE_L1_ICACHE1_PLD_DONE_INT_ST_V << CACHE_L1_ICACHE1_PLD_DONE_INT_ST_S)
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_DONE_INT_ST_S  1
/** CACHE_L1_ICACHE2_PLD_DONE_INT_ST : RO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST_M  (CACHE_L1_ICACHE2_PLD_DONE_INT_ST_V << CACHE_L1_ICACHE2_PLD_DONE_INT_ST_S)
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_DONE_INT_ST_S  2
/** CACHE_L1_ICACHE3_PLD_DONE_INT_ST : RO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ST    (BIT(3))
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ST_M  (CACHE_L1_ICACHE3_PLD_DONE_INT_ST_V << CACHE_L1_ICACHE3_PLD_DONE_INT_ST_S)
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_DONE_INT_ST_S  3
/** CACHE_L1_DCACHE_PLD_DONE_INT_ST : RO; bitpos: [4]; default: 0;
 *  The bit indicates the status of the interrupt that occurs only when L1-DCache
 *  preload-operation is done.
 */
#define CACHE_L1_DCACHE_PLD_DONE_INT_ST    (BIT(4))
#define CACHE_L1_DCACHE_PLD_DONE_INT_ST_M  (CACHE_L1_DCACHE_PLD_DONE_INT_ST_V << CACHE_L1_DCACHE_PLD_DONE_INT_ST_S)
#define CACHE_L1_DCACHE_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_DONE_INT_ST_S  4
/** CACHE_SYNC_DONE_INT_ST : RO; bitpos: [6]; default: 0;
 *  The bit indicates the status of the interrupt that occurs only when Cache
 *  sync-operation is done.
 */
#define CACHE_SYNC_DONE_INT_ST    (BIT(6))
#define CACHE_SYNC_DONE_INT_ST_M  (CACHE_SYNC_DONE_INT_ST_V << CACHE_SYNC_DONE_INT_ST_S)
#define CACHE_SYNC_DONE_INT_ST_V  0x00000001U
#define CACHE_SYNC_DONE_INT_ST_S  6
/** CACHE_L1_ICACHE0_PLD_ERR_INT_ST : RO; bitpos: [7]; default: 0;
 *  The bit indicates the status of the interrupt of L1-ICache0 preload-operation error.
 */
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ST    (BIT(7))
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ST_M  (CACHE_L1_ICACHE0_PLD_ERR_INT_ST_V << CACHE_L1_ICACHE0_PLD_ERR_INT_ST_S)
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_ERR_INT_ST_S  7
/** CACHE_L1_ICACHE1_PLD_ERR_INT_ST : RO; bitpos: [8]; default: 0;
 *  The bit indicates the status of the interrupt of L1-ICache1 preload-operation error.
 */
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ST    (BIT(8))
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ST_M  (CACHE_L1_ICACHE1_PLD_ERR_INT_ST_V << CACHE_L1_ICACHE1_PLD_ERR_INT_ST_S)
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_ERR_INT_ST_S  8
/** CACHE_L1_ICACHE2_PLD_ERR_INT_ST : RO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST    (BIT(9))
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST_M  (CACHE_L1_ICACHE2_PLD_ERR_INT_ST_V << CACHE_L1_ICACHE2_PLD_ERR_INT_ST_S)
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_ERR_INT_ST_S  9
/** CACHE_L1_ICACHE3_PLD_ERR_INT_ST : RO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ST    (BIT(10))
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ST_M  (CACHE_L1_ICACHE3_PLD_ERR_INT_ST_V << CACHE_L1_ICACHE3_PLD_ERR_INT_ST_S)
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_ERR_INT_ST_S  10
/** CACHE_L1_DCACHE_PLD_ERR_INT_ST : RO; bitpos: [11]; default: 0;
 *  The bit indicates the status of the interrupt of L1-DCache preload-operation error.
 */
#define CACHE_L1_DCACHE_PLD_ERR_INT_ST    (BIT(11))
#define CACHE_L1_DCACHE_PLD_ERR_INT_ST_M  (CACHE_L1_DCACHE_PLD_ERR_INT_ST_V << CACHE_L1_DCACHE_PLD_ERR_INT_ST_S)
#define CACHE_L1_DCACHE_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_ERR_INT_ST_S  11
/** CACHE_SYNC_ERR_INT_ST : RO; bitpos: [13]; default: 0;
 *  The bit indicates the status of the interrupt of Cache sync-operation error.
 */
#define CACHE_SYNC_ERR_INT_ST    (BIT(13))
#define CACHE_SYNC_ERR_INT_ST_M  (CACHE_SYNC_ERR_INT_ST_V << CACHE_SYNC_ERR_INT_ST_S)
#define CACHE_SYNC_ERR_INT_ST_V  0x00000001U
#define CACHE_SYNC_ERR_INT_ST_S  13

/** CACHE_SYNC_L1_CACHE_PRELOAD_EXCEPTION_REG register
 *  Cache Sync/Preload Operation exception register
 */
#define CACHE_SYNC_L1_CACHE_PRELOAD_EXCEPTION_REG (DR_REG_CACHE_BASE + 0x248)
/** CACHE_L1_ICACHE0_PLD_ERR_CODE : RO; bitpos: [1:0]; default: 0;
 *  The value 2 is Only available which means preload size is error in L1-ICache0.
 */
#define CACHE_L1_ICACHE0_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_ICACHE0_PLD_ERR_CODE_M  (CACHE_L1_ICACHE0_PLD_ERR_CODE_V << CACHE_L1_ICACHE0_PLD_ERR_CODE_S)
#define CACHE_L1_ICACHE0_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_ICACHE0_PLD_ERR_CODE_S  0
/** CACHE_L1_ICACHE1_PLD_ERR_CODE : RO; bitpos: [3:2]; default: 0;
 *  The value 2 is Only available which means preload size is error in L1-ICache1.
 */
#define CACHE_L1_ICACHE1_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_ICACHE1_PLD_ERR_CODE_M  (CACHE_L1_ICACHE1_PLD_ERR_CODE_V << CACHE_L1_ICACHE1_PLD_ERR_CODE_S)
#define CACHE_L1_ICACHE1_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_ICACHE1_PLD_ERR_CODE_S  2
/** CACHE_L1_ICACHE2_PLD_ERR_CODE : RO; bitpos: [5:4]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_ICACHE2_PLD_ERR_CODE_M  (CACHE_L1_ICACHE2_PLD_ERR_CODE_V << CACHE_L1_ICACHE2_PLD_ERR_CODE_S)
#define CACHE_L1_ICACHE2_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_ICACHE2_PLD_ERR_CODE_S  4
/** CACHE_L1_ICACHE3_PLD_ERR_CODE : RO; bitpos: [7:6]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_ICACHE3_PLD_ERR_CODE_M  (CACHE_L1_ICACHE3_PLD_ERR_CODE_V << CACHE_L1_ICACHE3_PLD_ERR_CODE_S)
#define CACHE_L1_ICACHE3_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_ICACHE3_PLD_ERR_CODE_S  6
/** CACHE_L1_DCACHE_PLD_ERR_CODE : RO; bitpos: [9:8]; default: 0;
 *  The value 2 is Only available which means preload size is error in L1-DCache.
 */
#define CACHE_L1_DCACHE_PLD_ERR_CODE    0x00000003U
#define CACHE_L1_DCACHE_PLD_ERR_CODE_M  (CACHE_L1_DCACHE_PLD_ERR_CODE_V << CACHE_L1_DCACHE_PLD_ERR_CODE_S)
#define CACHE_L1_DCACHE_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L1_DCACHE_PLD_ERR_CODE_S  8
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
#define CACHE_L1_CACHE_SYNC_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x24c)
/** CACHE_L1_ICACHE0_SYNC_RST : R/W; bitpos: [0]; default: 0;
 *  set this bit to reset sync-logic inside L1-ICache0. Recommend that this should only
 *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
 */
#define CACHE_L1_ICACHE0_SYNC_RST    (BIT(0))
#define CACHE_L1_ICACHE0_SYNC_RST_M  (CACHE_L1_ICACHE0_SYNC_RST_V << CACHE_L1_ICACHE0_SYNC_RST_S)
#define CACHE_L1_ICACHE0_SYNC_RST_V  0x00000001U
#define CACHE_L1_ICACHE0_SYNC_RST_S  0
/** CACHE_L1_ICACHE1_SYNC_RST : R/W; bitpos: [1]; default: 0;
 *  set this bit to reset sync-logic inside L1-ICache1. Recommend that this should only
 *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
 */
#define CACHE_L1_ICACHE1_SYNC_RST    (BIT(1))
#define CACHE_L1_ICACHE1_SYNC_RST_M  (CACHE_L1_ICACHE1_SYNC_RST_V << CACHE_L1_ICACHE1_SYNC_RST_S)
#define CACHE_L1_ICACHE1_SYNC_RST_V  0x00000001U
#define CACHE_L1_ICACHE1_SYNC_RST_S  1
/** CACHE_L1_ICACHE2_SYNC_RST : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_SYNC_RST    (BIT(2))
#define CACHE_L1_ICACHE2_SYNC_RST_M  (CACHE_L1_ICACHE2_SYNC_RST_V << CACHE_L1_ICACHE2_SYNC_RST_S)
#define CACHE_L1_ICACHE2_SYNC_RST_V  0x00000001U
#define CACHE_L1_ICACHE2_SYNC_RST_S  2
/** CACHE_L1_ICACHE3_SYNC_RST : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_SYNC_RST    (BIT(3))
#define CACHE_L1_ICACHE3_SYNC_RST_M  (CACHE_L1_ICACHE3_SYNC_RST_V << CACHE_L1_ICACHE3_SYNC_RST_S)
#define CACHE_L1_ICACHE3_SYNC_RST_V  0x00000001U
#define CACHE_L1_ICACHE3_SYNC_RST_S  3
/** CACHE_L1_DCACHE_SYNC_RST : R/W; bitpos: [4]; default: 0;
 *  set this bit to reset sync-logic inside L1-DCache. Recommend that this should only
 *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
 */
#define CACHE_L1_DCACHE_SYNC_RST    (BIT(4))
#define CACHE_L1_DCACHE_SYNC_RST_M  (CACHE_L1_DCACHE_SYNC_RST_V << CACHE_L1_DCACHE_SYNC_RST_S)
#define CACHE_L1_DCACHE_SYNC_RST_V  0x00000001U
#define CACHE_L1_DCACHE_SYNC_RST_S  4

/** CACHE_L1_CACHE_PRELOAD_RST_CTRL_REG register
 *  Cache Preload Reset control register
 */
#define CACHE_L1_CACHE_PRELOAD_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x250)
/** CACHE_L1_ICACHE0_PLD_RST : R/W; bitpos: [0]; default: 0;
 *  set this bit to reset preload-logic inside L1-ICache0. Recommend that this should
 *  only be used to initialize preload-logic when some fatal error of preload-logic
 *  occurs.
 */
#define CACHE_L1_ICACHE0_PLD_RST    (BIT(0))
#define CACHE_L1_ICACHE0_PLD_RST_M  (CACHE_L1_ICACHE0_PLD_RST_V << CACHE_L1_ICACHE0_PLD_RST_S)
#define CACHE_L1_ICACHE0_PLD_RST_V  0x00000001U
#define CACHE_L1_ICACHE0_PLD_RST_S  0
/** CACHE_L1_ICACHE1_PLD_RST : R/W; bitpos: [1]; default: 0;
 *  set this bit to reset preload-logic inside L1-ICache1. Recommend that this should
 *  only be used to initialize preload-logic when some fatal error of preload-logic
 *  occurs.
 */
#define CACHE_L1_ICACHE1_PLD_RST    (BIT(1))
#define CACHE_L1_ICACHE1_PLD_RST_M  (CACHE_L1_ICACHE1_PLD_RST_V << CACHE_L1_ICACHE1_PLD_RST_S)
#define CACHE_L1_ICACHE1_PLD_RST_V  0x00000001U
#define CACHE_L1_ICACHE1_PLD_RST_S  1
/** CACHE_L1_ICACHE2_PLD_RST : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_PLD_RST    (BIT(2))
#define CACHE_L1_ICACHE2_PLD_RST_M  (CACHE_L1_ICACHE2_PLD_RST_V << CACHE_L1_ICACHE2_PLD_RST_S)
#define CACHE_L1_ICACHE2_PLD_RST_V  0x00000001U
#define CACHE_L1_ICACHE2_PLD_RST_S  2
/** CACHE_L1_ICACHE3_PLD_RST : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_PLD_RST    (BIT(3))
#define CACHE_L1_ICACHE3_PLD_RST_M  (CACHE_L1_ICACHE3_PLD_RST_V << CACHE_L1_ICACHE3_PLD_RST_S)
#define CACHE_L1_ICACHE3_PLD_RST_V  0x00000001U
#define CACHE_L1_ICACHE3_PLD_RST_S  3
/** CACHE_L1_DCACHE_PLD_RST : R/W; bitpos: [4]; default: 0;
 *  set this bit to reset preload-logic inside L1-DCache. Recommend that this should
 *  only be used to initialize preload-logic when some fatal error of preload-logic
 *  occurs.
 */
#define CACHE_L1_DCACHE_PLD_RST    (BIT(4))
#define CACHE_L1_DCACHE_PLD_RST_M  (CACHE_L1_DCACHE_PLD_RST_V << CACHE_L1_DCACHE_PLD_RST_S)
#define CACHE_L1_DCACHE_PLD_RST_V  0x00000001U
#define CACHE_L1_DCACHE_PLD_RST_S  4

/** CACHE_L1_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG register
 *  Cache Autoload buffer clear control register
 */
#define CACHE_L1_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG (DR_REG_CACHE_BASE + 0x254)
/** CACHE_L1_ICACHE0_ALD_BUF_CLR : R/W; bitpos: [0]; default: 0;
 *  set this bit to clear autoload-buffer inside L1-ICache0. If this bit is active,
 *  autoload will not work in L1-ICache0. This bit should not be active when autoload
 *  works in L1-ICache0.
 */
#define CACHE_L1_ICACHE0_ALD_BUF_CLR    (BIT(0))
#define CACHE_L1_ICACHE0_ALD_BUF_CLR_M  (CACHE_L1_ICACHE0_ALD_BUF_CLR_V << CACHE_L1_ICACHE0_ALD_BUF_CLR_S)
#define CACHE_L1_ICACHE0_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_ICACHE0_ALD_BUF_CLR_S  0
/** CACHE_L1_ICACHE1_ALD_BUF_CLR : R/W; bitpos: [1]; default: 0;
 *  set this bit to clear autoload-buffer inside L1-ICache1. If this bit is active,
 *  autoload will not work in L1-ICache1. This bit should not be active when autoload
 *  works in L1-ICache1.
 */
#define CACHE_L1_ICACHE1_ALD_BUF_CLR    (BIT(1))
#define CACHE_L1_ICACHE1_ALD_BUF_CLR_M  (CACHE_L1_ICACHE1_ALD_BUF_CLR_V << CACHE_L1_ICACHE1_ALD_BUF_CLR_S)
#define CACHE_L1_ICACHE1_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_ICACHE1_ALD_BUF_CLR_S  1
/** CACHE_L1_ICACHE2_ALD_BUF_CLR : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_ALD_BUF_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_ALD_BUF_CLR_M  (CACHE_L1_ICACHE2_ALD_BUF_CLR_V << CACHE_L1_ICACHE2_ALD_BUF_CLR_S)
#define CACHE_L1_ICACHE2_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_ALD_BUF_CLR_S  2
/** CACHE_L1_ICACHE3_ALD_BUF_CLR : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_ALD_BUF_CLR    (BIT(3))
#define CACHE_L1_ICACHE3_ALD_BUF_CLR_M  (CACHE_L1_ICACHE3_ALD_BUF_CLR_V << CACHE_L1_ICACHE3_ALD_BUF_CLR_S)
#define CACHE_L1_ICACHE3_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_ICACHE3_ALD_BUF_CLR_S  3
/** CACHE_L1_DCACHE_ALD_BUF_CLR : R/W; bitpos: [4]; default: 0;
 *  set this bit to clear autoload-buffer inside L1-DCache. If this bit is active,
 *  autoload will not work in L1-DCache. This bit should not be active when autoload
 *  works in L1-DCache.
 */
#define CACHE_L1_DCACHE_ALD_BUF_CLR    (BIT(4))
#define CACHE_L1_DCACHE_ALD_BUF_CLR_M  (CACHE_L1_DCACHE_ALD_BUF_CLR_V << CACHE_L1_DCACHE_ALD_BUF_CLR_S)
#define CACHE_L1_DCACHE_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L1_DCACHE_ALD_BUF_CLR_S  4

/** CACHE_L1_UNALLOCATE_BUFFER_CLEAR_REG register
 *  Unallocate request buffer clear registers
 */
#define CACHE_L1_UNALLOCATE_BUFFER_CLEAR_REG (DR_REG_CACHE_BASE + 0x258)
/** CACHE_L1_ICACHE0_UNALLOC_CLR : R/W; bitpos: [0]; default: 0;
 *  The bit is used to clear the unallocate request buffer of l1 icache0 where the
 *  unallocate request is responded but not completed.
 */
#define CACHE_L1_ICACHE0_UNALLOC_CLR    (BIT(0))
#define CACHE_L1_ICACHE0_UNALLOC_CLR_M  (CACHE_L1_ICACHE0_UNALLOC_CLR_V << CACHE_L1_ICACHE0_UNALLOC_CLR_S)
#define CACHE_L1_ICACHE0_UNALLOC_CLR_V  0x00000001U
#define CACHE_L1_ICACHE0_UNALLOC_CLR_S  0
/** CACHE_L1_ICACHE1_UNALLOC_CLR : R/W; bitpos: [1]; default: 0;
 *  The bit is used to clear the unallocate request buffer of l1 icache1 where the
 *  unallocate request is responded but not completed.
 */
#define CACHE_L1_ICACHE1_UNALLOC_CLR    (BIT(1))
#define CACHE_L1_ICACHE1_UNALLOC_CLR_M  (CACHE_L1_ICACHE1_UNALLOC_CLR_V << CACHE_L1_ICACHE1_UNALLOC_CLR_S)
#define CACHE_L1_ICACHE1_UNALLOC_CLR_V  0x00000001U
#define CACHE_L1_ICACHE1_UNALLOC_CLR_S  1
/** CACHE_L1_ICACHE2_UNALLOC_CLR : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_UNALLOC_CLR    (BIT(2))
#define CACHE_L1_ICACHE2_UNALLOC_CLR_M  (CACHE_L1_ICACHE2_UNALLOC_CLR_V << CACHE_L1_ICACHE2_UNALLOC_CLR_S)
#define CACHE_L1_ICACHE2_UNALLOC_CLR_V  0x00000001U
#define CACHE_L1_ICACHE2_UNALLOC_CLR_S  2
/** CACHE_L1_ICACHE3_UNALLOC_CLR : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_UNALLOC_CLR    (BIT(3))
#define CACHE_L1_ICACHE3_UNALLOC_CLR_M  (CACHE_L1_ICACHE3_UNALLOC_CLR_V << CACHE_L1_ICACHE3_UNALLOC_CLR_S)
#define CACHE_L1_ICACHE3_UNALLOC_CLR_V  0x00000001U
#define CACHE_L1_ICACHE3_UNALLOC_CLR_S  3
/** CACHE_L1_DCACHE_UNALLOC_CLR : R/W; bitpos: [4]; default: 0;
 *  The bit is used to clear the unallocate request buffer of l1 dcache where the
 *  unallocate request is responded but not completed.
 */
#define CACHE_L1_DCACHE_UNALLOC_CLR    (BIT(4))
#define CACHE_L1_DCACHE_UNALLOC_CLR_M  (CACHE_L1_DCACHE_UNALLOC_CLR_V << CACHE_L1_DCACHE_UNALLOC_CLR_S)
#define CACHE_L1_DCACHE_UNALLOC_CLR_V  0x00000001U
#define CACHE_L1_DCACHE_UNALLOC_CLR_S  4

/** CACHE_L1_CACHE_OBJECT_CTRL_REG register
 *  Cache Tag and Data memory Object control register
 */
#define CACHE_L1_CACHE_OBJECT_CTRL_REG (DR_REG_CACHE_BASE + 0x25c)
/** CACHE_L1_ICACHE0_TAG_OBJECT : R/W; bitpos: [0]; default: 0;
 *  Set this bit to set L1-ICache0 tag memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L1_ICACHE0_TAG_OBJECT    (BIT(0))
#define CACHE_L1_ICACHE0_TAG_OBJECT_M  (CACHE_L1_ICACHE0_TAG_OBJECT_V << CACHE_L1_ICACHE0_TAG_OBJECT_S)
#define CACHE_L1_ICACHE0_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE0_TAG_OBJECT_S  0
/** CACHE_L1_ICACHE1_TAG_OBJECT : R/W; bitpos: [1]; default: 0;
 *  Set this bit to set L1-ICache1 tag memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L1_ICACHE1_TAG_OBJECT    (BIT(1))
#define CACHE_L1_ICACHE1_TAG_OBJECT_M  (CACHE_L1_ICACHE1_TAG_OBJECT_V << CACHE_L1_ICACHE1_TAG_OBJECT_S)
#define CACHE_L1_ICACHE1_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE1_TAG_OBJECT_S  1
/** CACHE_L1_ICACHE2_TAG_OBJECT : HRO; bitpos: [2]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_TAG_OBJECT    (BIT(2))
#define CACHE_L1_ICACHE2_TAG_OBJECT_M  (CACHE_L1_ICACHE2_TAG_OBJECT_V << CACHE_L1_ICACHE2_TAG_OBJECT_S)
#define CACHE_L1_ICACHE2_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE2_TAG_OBJECT_S  2
/** CACHE_L1_ICACHE3_TAG_OBJECT : HRO; bitpos: [3]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_TAG_OBJECT    (BIT(3))
#define CACHE_L1_ICACHE3_TAG_OBJECT_M  (CACHE_L1_ICACHE3_TAG_OBJECT_V << CACHE_L1_ICACHE3_TAG_OBJECT_S)
#define CACHE_L1_ICACHE3_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE3_TAG_OBJECT_S  3
/** CACHE_L1_DCACHE_TAG_OBJECT : R/W; bitpos: [4]; default: 0;
 *  Set this bit to set L1-DCache tag memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L1_DCACHE_TAG_OBJECT    (BIT(4))
#define CACHE_L1_DCACHE_TAG_OBJECT_M  (CACHE_L1_DCACHE_TAG_OBJECT_V << CACHE_L1_DCACHE_TAG_OBJECT_S)
#define CACHE_L1_DCACHE_TAG_OBJECT_V  0x00000001U
#define CACHE_L1_DCACHE_TAG_OBJECT_S  4
/** CACHE_L1_ICACHE0_MEM_OBJECT : R/W; bitpos: [6]; default: 0;
 *  Set this bit to set L1-ICache0 data memory as object. This bit should be onehot
 *  with the others fields inside this register.
 */
#define CACHE_L1_ICACHE0_MEM_OBJECT    (BIT(6))
#define CACHE_L1_ICACHE0_MEM_OBJECT_M  (CACHE_L1_ICACHE0_MEM_OBJECT_V << CACHE_L1_ICACHE0_MEM_OBJECT_S)
#define CACHE_L1_ICACHE0_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE0_MEM_OBJECT_S  6
/** CACHE_L1_ICACHE1_MEM_OBJECT : R/W; bitpos: [7]; default: 0;
 *  Set this bit to set L1-ICache1 data memory as object. This bit should be onehot
 *  with the others fields inside this register.
 */
#define CACHE_L1_ICACHE1_MEM_OBJECT    (BIT(7))
#define CACHE_L1_ICACHE1_MEM_OBJECT_M  (CACHE_L1_ICACHE1_MEM_OBJECT_V << CACHE_L1_ICACHE1_MEM_OBJECT_S)
#define CACHE_L1_ICACHE1_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE1_MEM_OBJECT_S  7
/** CACHE_L1_ICACHE2_MEM_OBJECT : HRO; bitpos: [8]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE2_MEM_OBJECT    (BIT(8))
#define CACHE_L1_ICACHE2_MEM_OBJECT_M  (CACHE_L1_ICACHE2_MEM_OBJECT_V << CACHE_L1_ICACHE2_MEM_OBJECT_S)
#define CACHE_L1_ICACHE2_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE2_MEM_OBJECT_S  8
/** CACHE_L1_ICACHE3_MEM_OBJECT : HRO; bitpos: [9]; default: 0;
 *  Reserved
 */
#define CACHE_L1_ICACHE3_MEM_OBJECT    (BIT(9))
#define CACHE_L1_ICACHE3_MEM_OBJECT_M  (CACHE_L1_ICACHE3_MEM_OBJECT_V << CACHE_L1_ICACHE3_MEM_OBJECT_S)
#define CACHE_L1_ICACHE3_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_ICACHE3_MEM_OBJECT_S  9
/** CACHE_L1_DCACHE_MEM_OBJECT : R/W; bitpos: [10]; default: 0;
 *  Set this bit to set L1-DCache data memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L1_DCACHE_MEM_OBJECT    (BIT(10))
#define CACHE_L1_DCACHE_MEM_OBJECT_M  (CACHE_L1_DCACHE_MEM_OBJECT_V << CACHE_L1_DCACHE_MEM_OBJECT_S)
#define CACHE_L1_DCACHE_MEM_OBJECT_V  0x00000001U
#define CACHE_L1_DCACHE_MEM_OBJECT_S  10

/** CACHE_L1_CACHE_WAY_OBJECT_REG register
 *  Cache Tag and Data memory way register
 */
#define CACHE_L1_CACHE_WAY_OBJECT_REG (DR_REG_CACHE_BASE + 0x260)
/** CACHE_L1_CACHE_WAY_OBJECT : R/W; bitpos: [2:0]; default: 0;
 *  Set this bits to select which way of the tag-object will be accessed. 0: way0, 1:
 *  way1, 2: way2, 3: way3, ?, 7: way7.
 */
#define CACHE_L1_CACHE_WAY_OBJECT    0x00000007U
#define CACHE_L1_CACHE_WAY_OBJECT_M  (CACHE_L1_CACHE_WAY_OBJECT_V << CACHE_L1_CACHE_WAY_OBJECT_S)
#define CACHE_L1_CACHE_WAY_OBJECT_V  0x00000007U
#define CACHE_L1_CACHE_WAY_OBJECT_S  0

/** CACHE_L1_CACHE_VADDR_REG register
 *  Cache Vaddr register
 */
#define CACHE_L1_CACHE_VADDR_REG (DR_REG_CACHE_BASE + 0x264)
/** CACHE_L1_CACHE_VADDR : R/W; bitpos: [31:0]; default: 1073741824;
 *  Those bits stores the virtual address which will decide where inside the specified
 *  tag memory object will be accessed.
 */
#define CACHE_L1_CACHE_VADDR    0xFFFFFFFFU
#define CACHE_L1_CACHE_VADDR_M  (CACHE_L1_CACHE_VADDR_V << CACHE_L1_CACHE_VADDR_S)
#define CACHE_L1_CACHE_VADDR_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_VADDR_S  0

/** CACHE_L1_CACHE_DEBUG_BUS_REG register
 *  Cache Tag/data memory content register
 */
#define CACHE_L1_CACHE_DEBUG_BUS_REG (DR_REG_CACHE_BASE + 0x268)
/** CACHE_L1_CACHE_DEBUG_BUS : R/W; bitpos: [31:0]; default: 616;
 *  This is a constant place where we can write data to or read data from the tag/data
 *  memory on the specified cache.
 */
#define CACHE_L1_CACHE_DEBUG_BUS    0xFFFFFFFFU
#define CACHE_L1_CACHE_DEBUG_BUS_M  (CACHE_L1_CACHE_DEBUG_BUS_V << CACHE_L1_CACHE_DEBUG_BUS_S)
#define CACHE_L1_CACHE_DEBUG_BUS_V  0xFFFFFFFFU
#define CACHE_L1_CACHE_DEBUG_BUS_S  0

/** CACHE_L2_CACHE_CTRL_REG register
 *  L2 Cache(L2-Cache) control register
 */
#define CACHE_L2_CACHE_CTRL_REG (DR_REG_CACHE_BASE + 0x270)
/** CACHE_L2_CACHE_SHUT_DMA : R/W; bitpos: [4]; default: 1;
 *  The bit is used to disable DMA access L2-Cache, 0: enable, 1: disable
 */
#define CACHE_L2_CACHE_SHUT_DMA    (BIT(4))
#define CACHE_L2_CACHE_SHUT_DMA_M  (CACHE_L2_CACHE_SHUT_DMA_V << CACHE_L2_CACHE_SHUT_DMA_S)
#define CACHE_L2_CACHE_SHUT_DMA_V  0x00000001U
#define CACHE_L2_CACHE_SHUT_DMA_S  4

/** CACHE_L2_BYPASS_CACHE_CONF_REG register
 *  Bypass Cache configure register
 */
#define CACHE_L2_BYPASS_CACHE_CONF_REG (DR_REG_CACHE_BASE + 0x274)
/** CACHE_BYPASS_L2_CACHE_EN : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable bypass L2-Cache. 0: disable bypass, 1: enable bypass.
 */
#define CACHE_BYPASS_L2_CACHE_EN    (BIT(5))
#define CACHE_BYPASS_L2_CACHE_EN_M  (CACHE_BYPASS_L2_CACHE_EN_V << CACHE_BYPASS_L2_CACHE_EN_S)
#define CACHE_BYPASS_L2_CACHE_EN_V  0x00000001U
#define CACHE_BYPASS_L2_CACHE_EN_S  5

/** CACHE_L2_CACHE_CACHESIZE_CONF_REG register
 *  L2 Cache CacheSize mode configure register
 */
#define CACHE_L2_CACHE_CACHESIZE_CONF_REG (DR_REG_CACHE_BASE + 0x278)
/** CACHE_L2_CACHE_CACHESIZE_256 : HRO; bitpos: [0]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 256 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_256    (BIT(0))
#define CACHE_L2_CACHE_CACHESIZE_256_M  (CACHE_L2_CACHE_CACHESIZE_256_V << CACHE_L2_CACHE_CACHESIZE_256_S)
#define CACHE_L2_CACHE_CACHESIZE_256_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_256_S  0
/** CACHE_L2_CACHE_CACHESIZE_512 : HRO; bitpos: [1]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 512 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_512    (BIT(1))
#define CACHE_L2_CACHE_CACHESIZE_512_M  (CACHE_L2_CACHE_CACHESIZE_512_V << CACHE_L2_CACHE_CACHESIZE_512_S)
#define CACHE_L2_CACHE_CACHESIZE_512_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_512_S  1
/** CACHE_L2_CACHE_CACHESIZE_1K : HRO; bitpos: [2]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 1k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_1K    (BIT(2))
#define CACHE_L2_CACHE_CACHESIZE_1K_M  (CACHE_L2_CACHE_CACHESIZE_1K_V << CACHE_L2_CACHE_CACHESIZE_1K_S)
#define CACHE_L2_CACHE_CACHESIZE_1K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_1K_S  2
/** CACHE_L2_CACHE_CACHESIZE_2K : HRO; bitpos: [3]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 2k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_2K    (BIT(3))
#define CACHE_L2_CACHE_CACHESIZE_2K_M  (CACHE_L2_CACHE_CACHESIZE_2K_V << CACHE_L2_CACHE_CACHESIZE_2K_S)
#define CACHE_L2_CACHE_CACHESIZE_2K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_2K_S  3
/** CACHE_L2_CACHE_CACHESIZE_4K : HRO; bitpos: [4]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 4k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_4K    (BIT(4))
#define CACHE_L2_CACHE_CACHESIZE_4K_M  (CACHE_L2_CACHE_CACHESIZE_4K_V << CACHE_L2_CACHE_CACHESIZE_4K_S)
#define CACHE_L2_CACHE_CACHESIZE_4K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_4K_S  4
/** CACHE_L2_CACHE_CACHESIZE_8K : HRO; bitpos: [5]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 8k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_8K    (BIT(5))
#define CACHE_L2_CACHE_CACHESIZE_8K_M  (CACHE_L2_CACHE_CACHESIZE_8K_V << CACHE_L2_CACHE_CACHESIZE_8K_S)
#define CACHE_L2_CACHE_CACHESIZE_8K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_8K_S  5
/** CACHE_L2_CACHE_CACHESIZE_16K : HRO; bitpos: [6]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 16k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_16K    (BIT(6))
#define CACHE_L2_CACHE_CACHESIZE_16K_M  (CACHE_L2_CACHE_CACHESIZE_16K_V << CACHE_L2_CACHE_CACHESIZE_16K_S)
#define CACHE_L2_CACHE_CACHESIZE_16K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_16K_S  6
/** CACHE_L2_CACHE_CACHESIZE_32K : HRO; bitpos: [7]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 32k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_32K    (BIT(7))
#define CACHE_L2_CACHE_CACHESIZE_32K_M  (CACHE_L2_CACHE_CACHESIZE_32K_V << CACHE_L2_CACHE_CACHESIZE_32K_S)
#define CACHE_L2_CACHE_CACHESIZE_32K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_32K_S  7
/** CACHE_L2_CACHE_CACHESIZE_64K : HRO; bitpos: [8]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 64k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_64K    (BIT(8))
#define CACHE_L2_CACHE_CACHESIZE_64K_M  (CACHE_L2_CACHE_CACHESIZE_64K_V << CACHE_L2_CACHE_CACHESIZE_64K_S)
#define CACHE_L2_CACHE_CACHESIZE_64K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_64K_S  8
/** CACHE_L2_CACHE_CACHESIZE_128K : R/W; bitpos: [9]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 128k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_128K    (BIT(9))
#define CACHE_L2_CACHE_CACHESIZE_128K_M  (CACHE_L2_CACHE_CACHESIZE_128K_V << CACHE_L2_CACHE_CACHESIZE_128K_S)
#define CACHE_L2_CACHE_CACHESIZE_128K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_128K_S  9
/** CACHE_L2_CACHE_CACHESIZE_256K : R/W; bitpos: [10]; default: 1;
 *  The field is used to configure cachesize of L2-Cache as 256k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_256K    (BIT(10))
#define CACHE_L2_CACHE_CACHESIZE_256K_M  (CACHE_L2_CACHE_CACHESIZE_256K_V << CACHE_L2_CACHE_CACHESIZE_256K_S)
#define CACHE_L2_CACHE_CACHESIZE_256K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_256K_S  10
/** CACHE_L2_CACHE_CACHESIZE_512K : R/W; bitpos: [11]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 512k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_512K    (BIT(11))
#define CACHE_L2_CACHE_CACHESIZE_512K_M  (CACHE_L2_CACHE_CACHESIZE_512K_V << CACHE_L2_CACHE_CACHESIZE_512K_S)
#define CACHE_L2_CACHE_CACHESIZE_512K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_512K_S  11
/** CACHE_L2_CACHE_CACHESIZE_1024K : HRO; bitpos: [12]; default: 0;
 *  The field is used to configure cachesize of L2-Cache as 1024k bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_CACHESIZE_1024K    (BIT(12))
#define CACHE_L2_CACHE_CACHESIZE_1024K_M  (CACHE_L2_CACHE_CACHESIZE_1024K_V << CACHE_L2_CACHE_CACHESIZE_1024K_S)
#define CACHE_L2_CACHE_CACHESIZE_1024K_V  0x00000001U
#define CACHE_L2_CACHE_CACHESIZE_1024K_S  12

/** CACHE_L2_CACHE_BLOCKSIZE_CONF_REG register
 *  L2 Cache BlockSize mode configure register
 */
#define CACHE_L2_CACHE_BLOCKSIZE_CONF_REG (DR_REG_CACHE_BASE + 0x27c)
/** CACHE_L2_CACHE_BLOCKSIZE_8 : HRO; bitpos: [0]; default: 0;
 *  The field is used to configureblocksize of L2-Cache as 8 bytes. This field and all
 *  other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_BLOCKSIZE_8    (BIT(0))
#define CACHE_L2_CACHE_BLOCKSIZE_8_M  (CACHE_L2_CACHE_BLOCKSIZE_8_V << CACHE_L2_CACHE_BLOCKSIZE_8_S)
#define CACHE_L2_CACHE_BLOCKSIZE_8_V  0x00000001U
#define CACHE_L2_CACHE_BLOCKSIZE_8_S  0
/** CACHE_L2_CACHE_BLOCKSIZE_16 : HRO; bitpos: [1]; default: 0;
 *  The field is used to configureblocksize of L2-Cache as 16 bytes. This field and all
 *  other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_BLOCKSIZE_16    (BIT(1))
#define CACHE_L2_CACHE_BLOCKSIZE_16_M  (CACHE_L2_CACHE_BLOCKSIZE_16_V << CACHE_L2_CACHE_BLOCKSIZE_16_S)
#define CACHE_L2_CACHE_BLOCKSIZE_16_V  0x00000001U
#define CACHE_L2_CACHE_BLOCKSIZE_16_S  1
/** CACHE_L2_CACHE_BLOCKSIZE_32 : HRO; bitpos: [2]; default: 0;
 *  The field is used to configureblocksize of L2-Cache as 32 bytes. This field and all
 *  other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_BLOCKSIZE_32    (BIT(2))
#define CACHE_L2_CACHE_BLOCKSIZE_32_M  (CACHE_L2_CACHE_BLOCKSIZE_32_V << CACHE_L2_CACHE_BLOCKSIZE_32_S)
#define CACHE_L2_CACHE_BLOCKSIZE_32_V  0x00000001U
#define CACHE_L2_CACHE_BLOCKSIZE_32_S  2
/** CACHE_L2_CACHE_BLOCKSIZE_64 : R/W; bitpos: [3]; default: 1;
 *  The field is used to configureblocksize of L2-Cache as 64 bytes. This field and all
 *  other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_BLOCKSIZE_64    (BIT(3))
#define CACHE_L2_CACHE_BLOCKSIZE_64_M  (CACHE_L2_CACHE_BLOCKSIZE_64_V << CACHE_L2_CACHE_BLOCKSIZE_64_S)
#define CACHE_L2_CACHE_BLOCKSIZE_64_V  0x00000001U
#define CACHE_L2_CACHE_BLOCKSIZE_64_S  3
/** CACHE_L2_CACHE_BLOCKSIZE_128 : R/W; bitpos: [4]; default: 0;
 *  The field is used to configureblocksize of L2-Cache as 128 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_BLOCKSIZE_128    (BIT(4))
#define CACHE_L2_CACHE_BLOCKSIZE_128_M  (CACHE_L2_CACHE_BLOCKSIZE_128_V << CACHE_L2_CACHE_BLOCKSIZE_128_S)
#define CACHE_L2_CACHE_BLOCKSIZE_128_V  0x00000001U
#define CACHE_L2_CACHE_BLOCKSIZE_128_S  4
/** CACHE_L2_CACHE_BLOCKSIZE_256 : HRO; bitpos: [5]; default: 0;
 *  The field is used to configureblocksize of L2-Cache as 256 bytes. This field and
 *  all other fields within this register is onehot.
 */
#define CACHE_L2_CACHE_BLOCKSIZE_256    (BIT(5))
#define CACHE_L2_CACHE_BLOCKSIZE_256_M  (CACHE_L2_CACHE_BLOCKSIZE_256_V << CACHE_L2_CACHE_BLOCKSIZE_256_S)
#define CACHE_L2_CACHE_BLOCKSIZE_256_V  0x00000001U
#define CACHE_L2_CACHE_BLOCKSIZE_256_S  5

/** CACHE_L2_CACHE_WRAP_AROUND_CTRL_REG register
 *  Cache wrap around control register
 */
#define CACHE_L2_CACHE_WRAP_AROUND_CTRL_REG (DR_REG_CACHE_BASE + 0x280)
/** CACHE_L2_CACHE_WRAP : R/W; bitpos: [5]; default: 0;
 *  Set this bit as 1 to enable L2-Cache wrap around mode.
 */
#define CACHE_L2_CACHE_WRAP    (BIT(5))
#define CACHE_L2_CACHE_WRAP_M  (CACHE_L2_CACHE_WRAP_V << CACHE_L2_CACHE_WRAP_S)
#define CACHE_L2_CACHE_WRAP_V  0x00000001U
#define CACHE_L2_CACHE_WRAP_S  5

/** CACHE_L2_CACHE_TAG_MEM_POWER_CTRL_REG register
 *  Cache tag memory power control register
 */
#define CACHE_L2_CACHE_TAG_MEM_POWER_CTRL_REG (DR_REG_CACHE_BASE + 0x284)
/** CACHE_L2_CACHE_TAG_MEM_FORCE_ON : R/W; bitpos: [20]; default: 1;
 *  The bit is used to close clock gating of  L2-Cache tag memory. 1: close gating, 0:
 *  open clock gating.
 */
#define CACHE_L2_CACHE_TAG_MEM_FORCE_ON    (BIT(20))
#define CACHE_L2_CACHE_TAG_MEM_FORCE_ON_M  (CACHE_L2_CACHE_TAG_MEM_FORCE_ON_V << CACHE_L2_CACHE_TAG_MEM_FORCE_ON_S)
#define CACHE_L2_CACHE_TAG_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L2_CACHE_TAG_MEM_FORCE_ON_S  20
/** CACHE_L2_CACHE_TAG_MEM_FORCE_PD : R/W; bitpos: [21]; default: 0;
 *  The bit is used to power L2-Cache tag memory down. 0: follow rtc_lslp, 1: power down
 */
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PD    (BIT(21))
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PD_M  (CACHE_L2_CACHE_TAG_MEM_FORCE_PD_V << CACHE_L2_CACHE_TAG_MEM_FORCE_PD_S)
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PD_S  21
/** CACHE_L2_CACHE_TAG_MEM_FORCE_PU : R/W; bitpos: [22]; default: 1;
 *  The bit is used to power L2-Cache tag memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PU    (BIT(22))
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PU_M  (CACHE_L2_CACHE_TAG_MEM_FORCE_PU_V << CACHE_L2_CACHE_TAG_MEM_FORCE_PU_S)
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L2_CACHE_TAG_MEM_FORCE_PU_S  22

/** CACHE_L2_CACHE_DATA_MEM_POWER_CTRL_REG register
 *  Cache data memory power control register
 */
#define CACHE_L2_CACHE_DATA_MEM_POWER_CTRL_REG (DR_REG_CACHE_BASE + 0x288)
/** CACHE_L2_CACHE_DATA_MEM_FORCE_ON : R/W; bitpos: [20]; default: 1;
 *  The bit is used to close clock gating of  L2-Cache data memory. 1: close gating, 0:
 *  open clock gating.
 */
#define CACHE_L2_CACHE_DATA_MEM_FORCE_ON    (BIT(20))
#define CACHE_L2_CACHE_DATA_MEM_FORCE_ON_M  (CACHE_L2_CACHE_DATA_MEM_FORCE_ON_V << CACHE_L2_CACHE_DATA_MEM_FORCE_ON_S)
#define CACHE_L2_CACHE_DATA_MEM_FORCE_ON_V  0x00000001U
#define CACHE_L2_CACHE_DATA_MEM_FORCE_ON_S  20
/** CACHE_L2_CACHE_DATA_MEM_FORCE_PD : R/W; bitpos: [21]; default: 0;
 *  The bit is used to power L2-Cache data memory down. 0: follow rtc_lslp, 1: power
 *  down
 */
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PD    (BIT(21))
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PD_M  (CACHE_L2_CACHE_DATA_MEM_FORCE_PD_V << CACHE_L2_CACHE_DATA_MEM_FORCE_PD_S)
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PD_V  0x00000001U
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PD_S  21
/** CACHE_L2_CACHE_DATA_MEM_FORCE_PU : R/W; bitpos: [22]; default: 1;
 *  The bit is used to power L2-Cache data memory up. 0: follow rtc_lslp, 1: power up
 */
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PU    (BIT(22))
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PU_M  (CACHE_L2_CACHE_DATA_MEM_FORCE_PU_V << CACHE_L2_CACHE_DATA_MEM_FORCE_PU_S)
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PU_V  0x00000001U
#define CACHE_L2_CACHE_DATA_MEM_FORCE_PU_S  22

/** CACHE_L2_CACHE_FREEZE_CTRL_REG register
 *  Cache Freeze control register
 */
#define CACHE_L2_CACHE_FREEZE_CTRL_REG (DR_REG_CACHE_BASE + 0x28c)
/** CACHE_L2_CACHE_FREEZE_EN : R/W; bitpos: [20]; default: 0;
 *  The bit is used to enable freeze operation on L2-Cache. It can be cleared by
 *  software.
 */
#define CACHE_L2_CACHE_FREEZE_EN    (BIT(20))
#define CACHE_L2_CACHE_FREEZE_EN_M  (CACHE_L2_CACHE_FREEZE_EN_V << CACHE_L2_CACHE_FREEZE_EN_S)
#define CACHE_L2_CACHE_FREEZE_EN_V  0x00000001U
#define CACHE_L2_CACHE_FREEZE_EN_S  20
/** CACHE_L2_CACHE_FREEZE_MODE : R/W; bitpos: [21]; default: 0;
 *  The bit is used to configure mode of freeze operation L2-Cache. 0: a miss-access
 *  will not stuck. 1: a miss-access will stuck.
 */
#define CACHE_L2_CACHE_FREEZE_MODE    (BIT(21))
#define CACHE_L2_CACHE_FREEZE_MODE_M  (CACHE_L2_CACHE_FREEZE_MODE_V << CACHE_L2_CACHE_FREEZE_MODE_S)
#define CACHE_L2_CACHE_FREEZE_MODE_V  0x00000001U
#define CACHE_L2_CACHE_FREEZE_MODE_S  21
/** CACHE_L2_CACHE_FREEZE_DONE : RO; bitpos: [22]; default: 0;
 *  The bit is used to indicate whether freeze operation on L2-Cache is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L2_CACHE_FREEZE_DONE    (BIT(22))
#define CACHE_L2_CACHE_FREEZE_DONE_M  (CACHE_L2_CACHE_FREEZE_DONE_V << CACHE_L2_CACHE_FREEZE_DONE_S)
#define CACHE_L2_CACHE_FREEZE_DONE_V  0x00000001U
#define CACHE_L2_CACHE_FREEZE_DONE_S  22

/** CACHE_L2_CACHE_DATA_MEM_ACS_CONF_REG register
 *  Cache data memory access configure register
 */
#define CACHE_L2_CACHE_DATA_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x290)
/** CACHE_L2_CACHE_DATA_MEM_RD_EN : R/W; bitpos: [20]; default: 1;
 *  The bit is used to enable config-bus read L2-Cache data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L2_CACHE_DATA_MEM_RD_EN    (BIT(20))
#define CACHE_L2_CACHE_DATA_MEM_RD_EN_M  (CACHE_L2_CACHE_DATA_MEM_RD_EN_V << CACHE_L2_CACHE_DATA_MEM_RD_EN_S)
#define CACHE_L2_CACHE_DATA_MEM_RD_EN_V  0x00000001U
#define CACHE_L2_CACHE_DATA_MEM_RD_EN_S  20
/** CACHE_L2_CACHE_DATA_MEM_WR_EN : R/W; bitpos: [21]; default: 1;
 *  The bit is used to enable config-bus write L2-Cache data memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L2_CACHE_DATA_MEM_WR_EN    (BIT(21))
#define CACHE_L2_CACHE_DATA_MEM_WR_EN_M  (CACHE_L2_CACHE_DATA_MEM_WR_EN_V << CACHE_L2_CACHE_DATA_MEM_WR_EN_S)
#define CACHE_L2_CACHE_DATA_MEM_WR_EN_V  0x00000001U
#define CACHE_L2_CACHE_DATA_MEM_WR_EN_S  21

/** CACHE_L2_CACHE_TAG_MEM_ACS_CONF_REG register
 *  Cache tag memory access configure register
 */
#define CACHE_L2_CACHE_TAG_MEM_ACS_CONF_REG (DR_REG_CACHE_BASE + 0x294)
/** CACHE_L2_CACHE_TAG_MEM_RD_EN : R/W; bitpos: [20]; default: 1;
 *  The bit is used to enable config-bus read L2-Cache tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L2_CACHE_TAG_MEM_RD_EN    (BIT(20))
#define CACHE_L2_CACHE_TAG_MEM_RD_EN_M  (CACHE_L2_CACHE_TAG_MEM_RD_EN_V << CACHE_L2_CACHE_TAG_MEM_RD_EN_S)
#define CACHE_L2_CACHE_TAG_MEM_RD_EN_V  0x00000001U
#define CACHE_L2_CACHE_TAG_MEM_RD_EN_S  20
/** CACHE_L2_CACHE_TAG_MEM_WR_EN : R/W; bitpos: [21]; default: 1;
 *  The bit is used to enable config-bus write L2-Cache tag memoryory. 0: disable, 1:
 *  enable.
 */
#define CACHE_L2_CACHE_TAG_MEM_WR_EN    (BIT(21))
#define CACHE_L2_CACHE_TAG_MEM_WR_EN_M  (CACHE_L2_CACHE_TAG_MEM_WR_EN_V << CACHE_L2_CACHE_TAG_MEM_WR_EN_S)
#define CACHE_L2_CACHE_TAG_MEM_WR_EN_V  0x00000001U
#define CACHE_L2_CACHE_TAG_MEM_WR_EN_S  21

/** CACHE_L2_CACHE_PRELOCK_CONF_REG register
 *  L2 Cache prelock configure register
 */
#define CACHE_L2_CACHE_PRELOCK_CONF_REG (DR_REG_CACHE_BASE + 0x298)
/** CACHE_L2_CACHE_PRELOCK_SCT0_EN : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable the first section of prelock function on L2-Cache.
 */
#define CACHE_L2_CACHE_PRELOCK_SCT0_EN    (BIT(0))
#define CACHE_L2_CACHE_PRELOCK_SCT0_EN_M  (CACHE_L2_CACHE_PRELOCK_SCT0_EN_V << CACHE_L2_CACHE_PRELOCK_SCT0_EN_S)
#define CACHE_L2_CACHE_PRELOCK_SCT0_EN_V  0x00000001U
#define CACHE_L2_CACHE_PRELOCK_SCT0_EN_S  0
/** CACHE_L2_CACHE_PRELOCK_SCT1_EN : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the second section of prelock function on L2-Cache.
 */
#define CACHE_L2_CACHE_PRELOCK_SCT1_EN    (BIT(1))
#define CACHE_L2_CACHE_PRELOCK_SCT1_EN_M  (CACHE_L2_CACHE_PRELOCK_SCT1_EN_V << CACHE_L2_CACHE_PRELOCK_SCT1_EN_S)
#define CACHE_L2_CACHE_PRELOCK_SCT1_EN_V  0x00000001U
#define CACHE_L2_CACHE_PRELOCK_SCT1_EN_S  1
/** CACHE_L2_CACHE_PRELOCK_RGID : R/W; bitpos: [5:2]; default: 0;
 *  The bit is used to set  the gid of l2 cache prelock.
 */
#define CACHE_L2_CACHE_PRELOCK_RGID    0x0000000FU
#define CACHE_L2_CACHE_PRELOCK_RGID_M  (CACHE_L2_CACHE_PRELOCK_RGID_V << CACHE_L2_CACHE_PRELOCK_RGID_S)
#define CACHE_L2_CACHE_PRELOCK_RGID_V  0x0000000FU
#define CACHE_L2_CACHE_PRELOCK_RGID_S  2

/** CACHE_L2_CACHE_PRELOCK_SCT0_ADDR_REG register
 *  L2 Cache prelock section0 address configure register
 */
#define CACHE_L2_CACHE_PRELOCK_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x29c)
/** CACHE_L2_CACHE_PRELOCK_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section of
 *  prelock on L2-Cache, which should be used together with
 *  L2_CACHE_PRELOCK_SCT0_SIZE_REG
 */
#define CACHE_L2_CACHE_PRELOCK_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT0_ADDR_M  (CACHE_L2_CACHE_PRELOCK_SCT0_ADDR_V << CACHE_L2_CACHE_PRELOCK_SCT0_ADDR_S)
#define CACHE_L2_CACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT0_ADDR_S  0

/** CACHE_L2_CACHE_PRELOCK_SCT1_ADDR_REG register
 *  L2 Cache prelock section1 address configure register
 */
#define CACHE_L2_CACHE_PRELOCK_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x2a0)
/** CACHE_L2_CACHE_PRELOCK_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section of
 *  prelock on L2-Cache, which should be used together with
 *  L2_CACHE_PRELOCK_SCT1_SIZE_REG
 */
#define CACHE_L2_CACHE_PRELOCK_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT1_ADDR_M  (CACHE_L2_CACHE_PRELOCK_SCT1_ADDR_V << CACHE_L2_CACHE_PRELOCK_SCT1_ADDR_S)
#define CACHE_L2_CACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT1_ADDR_S  0

/** CACHE_L2_CACHE_PRELOCK_SCT_SIZE_REG register
 *  L2 Cache prelock section size configure register
 */
#define CACHE_L2_CACHE_PRELOCK_SCT_SIZE_REG (DR_REG_CACHE_BASE + 0x2a4)
/** CACHE_L2_CACHE_PRELOCK_SCT0_SIZE : R/W; bitpos: [15:0]; default: 65535;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L2-Cache, which should be used together with L2_CACHE_PRELOCK_SCT0_ADDR_REG
 */
#define CACHE_L2_CACHE_PRELOCK_SCT0_SIZE    0x0000FFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT0_SIZE_M  (CACHE_L2_CACHE_PRELOCK_SCT0_SIZE_V << CACHE_L2_CACHE_PRELOCK_SCT0_SIZE_S)
#define CACHE_L2_CACHE_PRELOCK_SCT0_SIZE_V  0x0000FFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT0_SIZE_S  0
/** CACHE_L2_CACHE_PRELOCK_SCT1_SIZE : R/W; bitpos: [31:16]; default: 65535;
 *  Those bits are used to configure the size of the second section of prelock on
 *  L2-Cache, which should be used together with L2_CACHE_PRELOCK_SCT1_ADDR_REG
 */
#define CACHE_L2_CACHE_PRELOCK_SCT1_SIZE    0x0000FFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT1_SIZE_M  (CACHE_L2_CACHE_PRELOCK_SCT1_SIZE_V << CACHE_L2_CACHE_PRELOCK_SCT1_SIZE_S)
#define CACHE_L2_CACHE_PRELOCK_SCT1_SIZE_V  0x0000FFFFU
#define CACHE_L2_CACHE_PRELOCK_SCT1_SIZE_S  16

/** CACHE_L2_CACHE_PRELOAD_CTRL_REG register
 *  L2 Cache preload-operation control register
 */
#define CACHE_L2_CACHE_PRELOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x2a8)
/** CACHE_L2_CACHE_PRELOAD_ENA : R/W/SC; bitpos: [0]; default: 0;
 *  The bit is used to enable preload operation on L2-Cache. It will be cleared by
 *  hardware automatically after preload operation is done.
 */
#define CACHE_L2_CACHE_PRELOAD_ENA    (BIT(0))
#define CACHE_L2_CACHE_PRELOAD_ENA_M  (CACHE_L2_CACHE_PRELOAD_ENA_V << CACHE_L2_CACHE_PRELOAD_ENA_S)
#define CACHE_L2_CACHE_PRELOAD_ENA_V  0x00000001U
#define CACHE_L2_CACHE_PRELOAD_ENA_S  0
/** CACHE_L2_CACHE_PRELOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether preload operation is finished or not. 0: not
 *  finished. 1: finished.
 */
#define CACHE_L2_CACHE_PRELOAD_DONE    (BIT(1))
#define CACHE_L2_CACHE_PRELOAD_DONE_M  (CACHE_L2_CACHE_PRELOAD_DONE_V << CACHE_L2_CACHE_PRELOAD_DONE_S)
#define CACHE_L2_CACHE_PRELOAD_DONE_V  0x00000001U
#define CACHE_L2_CACHE_PRELOAD_DONE_S  1
/** CACHE_L2_CACHE_PRELOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of preload operation. 0: ascending, 1:
 *  descending.
 */
#define CACHE_L2_CACHE_PRELOAD_ORDER    (BIT(2))
#define CACHE_L2_CACHE_PRELOAD_ORDER_M  (CACHE_L2_CACHE_PRELOAD_ORDER_V << CACHE_L2_CACHE_PRELOAD_ORDER_S)
#define CACHE_L2_CACHE_PRELOAD_ORDER_V  0x00000001U
#define CACHE_L2_CACHE_PRELOAD_ORDER_S  2
/** CACHE_L2_CACHE_PRELOAD_RGID : R/W; bitpos: [6:3]; default: 0;
 *  The bit is used to set  the gid of l2 cache preload.
 */
#define CACHE_L2_CACHE_PRELOAD_RGID    0x0000000FU
#define CACHE_L2_CACHE_PRELOAD_RGID_M  (CACHE_L2_CACHE_PRELOAD_RGID_V << CACHE_L2_CACHE_PRELOAD_RGID_S)
#define CACHE_L2_CACHE_PRELOAD_RGID_V  0x0000000FU
#define CACHE_L2_CACHE_PRELOAD_RGID_S  3

/** CACHE_L2_CACHE_PRELOAD_ADDR_REG register
 *  L2 Cache preload address configure register
 */
#define CACHE_L2_CACHE_PRELOAD_ADDR_REG (DR_REG_CACHE_BASE + 0x2ac)
/** CACHE_L2_CACHE_PRELOAD_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of preload on L2-Cache,
 *  which should be used together with L2_CACHE_PRELOAD_SIZE_REG
 */
#define CACHE_L2_CACHE_PRELOAD_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_PRELOAD_ADDR_M  (CACHE_L2_CACHE_PRELOAD_ADDR_V << CACHE_L2_CACHE_PRELOAD_ADDR_S)
#define CACHE_L2_CACHE_PRELOAD_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_PRELOAD_ADDR_S  0

/** CACHE_L2_CACHE_PRELOAD_SIZE_REG register
 *  L2 Cache preload size configure register
 */
#define CACHE_L2_CACHE_PRELOAD_SIZE_REG (DR_REG_CACHE_BASE + 0x2b0)
/** CACHE_L2_CACHE_PRELOAD_SIZE : R/W; bitpos: [15:0]; default: 0;
 *  Those bits are used to configure the size of the first section of prelock on
 *  L2-Cache, which should be used together with L2_CACHE_PRELOAD_ADDR_REG
 */
#define CACHE_L2_CACHE_PRELOAD_SIZE    0x0000FFFFU
#define CACHE_L2_CACHE_PRELOAD_SIZE_M  (CACHE_L2_CACHE_PRELOAD_SIZE_V << CACHE_L2_CACHE_PRELOAD_SIZE_S)
#define CACHE_L2_CACHE_PRELOAD_SIZE_V  0x0000FFFFU
#define CACHE_L2_CACHE_PRELOAD_SIZE_S  0

/** CACHE_L2_CACHE_AUTOLOAD_CTRL_REG register
 *  L2 Cache autoload-operation control register
 */
#define CACHE_L2_CACHE_AUTOLOAD_CTRL_REG (DR_REG_CACHE_BASE + 0x2b4)
/** CACHE_L2_CACHE_AUTOLOAD_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable and disable autoload operation on L2-Cache.  1: enable,
 *  0: disable.
 */
#define CACHE_L2_CACHE_AUTOLOAD_ENA    (BIT(0))
#define CACHE_L2_CACHE_AUTOLOAD_ENA_M  (CACHE_L2_CACHE_AUTOLOAD_ENA_V << CACHE_L2_CACHE_AUTOLOAD_ENA_S)
#define CACHE_L2_CACHE_AUTOLOAD_ENA_V  0x00000001U
#define CACHE_L2_CACHE_AUTOLOAD_ENA_S  0
/** CACHE_L2_CACHE_AUTOLOAD_DONE : RO; bitpos: [1]; default: 1;
 *  The bit is used to indicate whether autoload operation on L2-Cache is finished or
 *  not. 0: not finished. 1: finished.
 */
#define CACHE_L2_CACHE_AUTOLOAD_DONE    (BIT(1))
#define CACHE_L2_CACHE_AUTOLOAD_DONE_M  (CACHE_L2_CACHE_AUTOLOAD_DONE_V << CACHE_L2_CACHE_AUTOLOAD_DONE_S)
#define CACHE_L2_CACHE_AUTOLOAD_DONE_V  0x00000001U
#define CACHE_L2_CACHE_AUTOLOAD_DONE_S  1
/** CACHE_L2_CACHE_AUTOLOAD_ORDER : R/W; bitpos: [2]; default: 0;
 *  The bit is used to configure the direction of autoload operation on L2-Cache. 0:
 *  ascending. 1: descending.
 */
#define CACHE_L2_CACHE_AUTOLOAD_ORDER    (BIT(2))
#define CACHE_L2_CACHE_AUTOLOAD_ORDER_M  (CACHE_L2_CACHE_AUTOLOAD_ORDER_V << CACHE_L2_CACHE_AUTOLOAD_ORDER_S)
#define CACHE_L2_CACHE_AUTOLOAD_ORDER_V  0x00000001U
#define CACHE_L2_CACHE_AUTOLOAD_ORDER_S  2
/** CACHE_L2_CACHE_AUTOLOAD_TRIGGER_MODE : R/W; bitpos: [4:3]; default: 0;
 *  The field is used to configure trigger mode of autoload operation on L2-Cache. 0/3:
 *  miss-trigger, 1: hit-trigger, 2: miss-hit-trigger.
 */
#define CACHE_L2_CACHE_AUTOLOAD_TRIGGER_MODE    0x00000003U
#define CACHE_L2_CACHE_AUTOLOAD_TRIGGER_MODE_M  (CACHE_L2_CACHE_AUTOLOAD_TRIGGER_MODE_V << CACHE_L2_CACHE_AUTOLOAD_TRIGGER_MODE_S)
#define CACHE_L2_CACHE_AUTOLOAD_TRIGGER_MODE_V  0x00000003U
#define CACHE_L2_CACHE_AUTOLOAD_TRIGGER_MODE_S  3
/** CACHE_L2_CACHE_AUTOLOAD_SCT0_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable the first section for autoload operation on L2-Cache.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ENA    (BIT(8))
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ENA_M  (CACHE_L2_CACHE_AUTOLOAD_SCT0_ENA_V << CACHE_L2_CACHE_AUTOLOAD_SCT0_ENA_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ENA_V  0x00000001U
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ENA_S  8
/** CACHE_L2_CACHE_AUTOLOAD_SCT1_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable the second section for autoload operation on L2-Cache.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ENA    (BIT(9))
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ENA_M  (CACHE_L2_CACHE_AUTOLOAD_SCT1_ENA_V << CACHE_L2_CACHE_AUTOLOAD_SCT1_ENA_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ENA_V  0x00000001U
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ENA_S  9
/** CACHE_L2_CACHE_AUTOLOAD_SCT2_ENA : R/W; bitpos: [10]; default: 0;
 *  The bit is used to enable the third section for autoload operation on L2-Cache.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ENA    (BIT(10))
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ENA_M  (CACHE_L2_CACHE_AUTOLOAD_SCT2_ENA_V << CACHE_L2_CACHE_AUTOLOAD_SCT2_ENA_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ENA_V  0x00000001U
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ENA_S  10
/** CACHE_L2_CACHE_AUTOLOAD_SCT3_ENA : R/W; bitpos: [11]; default: 0;
 *  The bit is used to enable the fourth section for autoload operation on L2-Cache.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ENA    (BIT(11))
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ENA_M  (CACHE_L2_CACHE_AUTOLOAD_SCT3_ENA_V << CACHE_L2_CACHE_AUTOLOAD_SCT3_ENA_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ENA_V  0x00000001U
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ENA_S  11
/** CACHE_L2_CACHE_AUTOLOAD_RGID : R/W; bitpos: [15:12]; default: 0;
 *  The bit is used to set  the gid of l2 cache autoload.
 */
#define CACHE_L2_CACHE_AUTOLOAD_RGID    0x0000000FU
#define CACHE_L2_CACHE_AUTOLOAD_RGID_M  (CACHE_L2_CACHE_AUTOLOAD_RGID_V << CACHE_L2_CACHE_AUTOLOAD_RGID_S)
#define CACHE_L2_CACHE_AUTOLOAD_RGID_V  0x0000000FU
#define CACHE_L2_CACHE_AUTOLOAD_RGID_S  12

/** CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR_REG register
 *  L2 Cache autoload section 0 address configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR_REG (DR_REG_CACHE_BASE + 0x2b8)
/** CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the first section for
 *  autoload operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT0_SIZE and L2_CACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR_M  (CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR_V << CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_ADDR_S  0

/** CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE_REG register
 *  L2 Cache autoload section 0 size configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE_REG (DR_REG_CACHE_BASE + 0x2bc)
/** CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the first section for autoload
 *  operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT0_ADDR and L2_CACHE_AUTOLOAD_SCT0_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE    0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE_M  (CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE_V << CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE_V  0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT0_SIZE_S  0

/** CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR_REG register
 *  L2 Cache autoload section 1 address configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR_REG (DR_REG_CACHE_BASE + 0x2c0)
/** CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the second section
 *  for autoload operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT1_SIZE and L2_CACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR_M  (CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR_V << CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_ADDR_S  0

/** CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE_REG register
 *  L2 Cache autoload section 1 size configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE_REG (DR_REG_CACHE_BASE + 0x2c4)
/** CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the second section for autoload
 *  operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT1_ADDR and L2_CACHE_AUTOLOAD_SCT1_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE    0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE_M  (CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE_V << CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE_V  0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT1_SIZE_S  0

/** CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR_REG register
 *  L2 Cache autoload section 2 address configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR_REG (DR_REG_CACHE_BASE + 0x2c8)
/** CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the third section for
 *  autoload operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT2_SIZE and L2_CACHE_AUTOLOAD_SCT2_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR_M  (CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR_V << CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_ADDR_S  0

/** CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE_REG register
 *  L2 Cache autoload section 2 size configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE_REG (DR_REG_CACHE_BASE + 0x2cc)
/** CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the third section for autoload
 *  operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT2_ADDR and L2_CACHE_AUTOLOAD_SCT2_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE    0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE_M  (CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE_V << CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE_V  0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT2_SIZE_S  0

/** CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR_REG register
 *  L2 Cache autoload section 3 address configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR_REG (DR_REG_CACHE_BASE + 0x2d0)
/** CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Those bits are used to configure the start virtual address of the fourth section
 *  for autoload operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT3_SIZE and L2_CACHE_AUTOLOAD_SCT3_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR_M  (CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR_V << CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_ADDR_S  0

/** CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE_REG register
 *  L2 Cache autoload section 3 size configure register
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE_REG (DR_REG_CACHE_BASE + 0x2d4)
/** CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE : R/W; bitpos: [27:0]; default: 0;
 *  Those bits are used to configure the size of the fourth section for autoload
 *  operation on L2-Cache. Note that it should be used together with
 *  L2_CACHE_AUTOLOAD_SCT3_ADDR and L2_CACHE_AUTOLOAD_SCT3_ENA.
 */
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE    0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE_M  (CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE_V << CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE_S)
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE_V  0x0FFFFFFFU
#define CACHE_L2_CACHE_AUTOLOAD_SCT3_SIZE_S  0

/** CACHE_L2_CACHE_ACS_CNT_INT_ENA_REG register
 *  Cache Access Counter Interrupt enable register
 */
#define CACHE_L2_CACHE_ACS_CNT_INT_ENA_REG (DR_REG_CACHE_BASE + 0x2d8)
/** CACHE_L2_IBUS0_OVF_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L2-Cache due to bus0 accesses L2-Cache.
 */
#define CACHE_L2_IBUS0_OVF_INT_ENA    (BIT(8))
#define CACHE_L2_IBUS0_OVF_INT_ENA_M  (CACHE_L2_IBUS0_OVF_INT_ENA_V << CACHE_L2_IBUS0_OVF_INT_ENA_S)
#define CACHE_L2_IBUS0_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_IBUS0_OVF_INT_ENA_S  8
/** CACHE_L2_IBUS1_OVF_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L2-Cache due to bus1 accesses L2-Cache.
 */
#define CACHE_L2_IBUS1_OVF_INT_ENA    (BIT(9))
#define CACHE_L2_IBUS1_OVF_INT_ENA_M  (CACHE_L2_IBUS1_OVF_INT_ENA_V << CACHE_L2_IBUS1_OVF_INT_ENA_S)
#define CACHE_L2_IBUS1_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_IBUS1_OVF_INT_ENA_S  9
/** CACHE_L2_IBUS2_OVF_INT_ENA : HRO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS2_OVF_INT_ENA    (BIT(10))
#define CACHE_L2_IBUS2_OVF_INT_ENA_M  (CACHE_L2_IBUS2_OVF_INT_ENA_V << CACHE_L2_IBUS2_OVF_INT_ENA_S)
#define CACHE_L2_IBUS2_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_IBUS2_OVF_INT_ENA_S  10
/** CACHE_L2_IBUS3_OVF_INT_ENA : HRO; bitpos: [11]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS3_OVF_INT_ENA    (BIT(11))
#define CACHE_L2_IBUS3_OVF_INT_ENA_M  (CACHE_L2_IBUS3_OVF_INT_ENA_V << CACHE_L2_IBUS3_OVF_INT_ENA_S)
#define CACHE_L2_IBUS3_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_IBUS3_OVF_INT_ENA_S  11
/** CACHE_L2_DBUS0_OVF_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L2-Cache due to bus0 accesses L2-Cache.
 */
#define CACHE_L2_DBUS0_OVF_INT_ENA    (BIT(12))
#define CACHE_L2_DBUS0_OVF_INT_ENA_M  (CACHE_L2_DBUS0_OVF_INT_ENA_V << CACHE_L2_DBUS0_OVF_INT_ENA_S)
#define CACHE_L2_DBUS0_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_DBUS0_OVF_INT_ENA_S  12
/** CACHE_L2_DBUS1_OVF_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The bit is used to enable interrupt of one of counters overflow that occurs in
 *  L2-Cache due to bus1 accesses L2-Cache.
 */
#define CACHE_L2_DBUS1_OVF_INT_ENA    (BIT(13))
#define CACHE_L2_DBUS1_OVF_INT_ENA_M  (CACHE_L2_DBUS1_OVF_INT_ENA_V << CACHE_L2_DBUS1_OVF_INT_ENA_S)
#define CACHE_L2_DBUS1_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_DBUS1_OVF_INT_ENA_S  13
/** CACHE_L2_DBUS2_OVF_INT_ENA : HRO; bitpos: [14]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS2_OVF_INT_ENA    (BIT(14))
#define CACHE_L2_DBUS2_OVF_INT_ENA_M  (CACHE_L2_DBUS2_OVF_INT_ENA_V << CACHE_L2_DBUS2_OVF_INT_ENA_S)
#define CACHE_L2_DBUS2_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_DBUS2_OVF_INT_ENA_S  14
/** CACHE_L2_DBUS3_OVF_INT_ENA : HRO; bitpos: [15]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS3_OVF_INT_ENA    (BIT(15))
#define CACHE_L2_DBUS3_OVF_INT_ENA_M  (CACHE_L2_DBUS3_OVF_INT_ENA_V << CACHE_L2_DBUS3_OVF_INT_ENA_S)
#define CACHE_L2_DBUS3_OVF_INT_ENA_V  0x00000001U
#define CACHE_L2_DBUS3_OVF_INT_ENA_S  15

/** CACHE_L2_CACHE_ACS_CNT_INT_CLR_REG register
 *  Cache Access Counter Interrupt clear register
 */
#define CACHE_L2_CACHE_ACS_CNT_INT_CLR_REG (DR_REG_CACHE_BASE + 0x2dc)
/** CACHE_L2_IBUS0_OVF_INT_CLR : WT; bitpos: [8]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
 *  to bus0 accesses L2-Cache.
 */
#define CACHE_L2_IBUS0_OVF_INT_CLR    (BIT(8))
#define CACHE_L2_IBUS0_OVF_INT_CLR_M  (CACHE_L2_IBUS0_OVF_INT_CLR_V << CACHE_L2_IBUS0_OVF_INT_CLR_S)
#define CACHE_L2_IBUS0_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_IBUS0_OVF_INT_CLR_S  8
/** CACHE_L2_IBUS1_OVF_INT_CLR : WT; bitpos: [9]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
 *  to bus1 accesses L2-Cache.
 */
#define CACHE_L2_IBUS1_OVF_INT_CLR    (BIT(9))
#define CACHE_L2_IBUS1_OVF_INT_CLR_M  (CACHE_L2_IBUS1_OVF_INT_CLR_V << CACHE_L2_IBUS1_OVF_INT_CLR_S)
#define CACHE_L2_IBUS1_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_IBUS1_OVF_INT_CLR_S  9
/** CACHE_L2_IBUS2_OVF_INT_CLR : HRO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS2_OVF_INT_CLR    (BIT(10))
#define CACHE_L2_IBUS2_OVF_INT_CLR_M  (CACHE_L2_IBUS2_OVF_INT_CLR_V << CACHE_L2_IBUS2_OVF_INT_CLR_S)
#define CACHE_L2_IBUS2_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_IBUS2_OVF_INT_CLR_S  10
/** CACHE_L2_IBUS3_OVF_INT_CLR : HRO; bitpos: [11]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS3_OVF_INT_CLR    (BIT(11))
#define CACHE_L2_IBUS3_OVF_INT_CLR_M  (CACHE_L2_IBUS3_OVF_INT_CLR_V << CACHE_L2_IBUS3_OVF_INT_CLR_S)
#define CACHE_L2_IBUS3_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_IBUS3_OVF_INT_CLR_S  11
/** CACHE_L2_DBUS0_OVF_INT_CLR : WT; bitpos: [12]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
 *  to bus0 accesses L2-Cache.
 */
#define CACHE_L2_DBUS0_OVF_INT_CLR    (BIT(12))
#define CACHE_L2_DBUS0_OVF_INT_CLR_M  (CACHE_L2_DBUS0_OVF_INT_CLR_V << CACHE_L2_DBUS0_OVF_INT_CLR_S)
#define CACHE_L2_DBUS0_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_DBUS0_OVF_INT_CLR_S  12
/** CACHE_L2_DBUS1_OVF_INT_CLR : WT; bitpos: [13]; default: 0;
 *  The bit is used to clear counters overflow interrupt and counters in L2-Cache due
 *  to bus1 accesses L2-Cache.
 */
#define CACHE_L2_DBUS1_OVF_INT_CLR    (BIT(13))
#define CACHE_L2_DBUS1_OVF_INT_CLR_M  (CACHE_L2_DBUS1_OVF_INT_CLR_V << CACHE_L2_DBUS1_OVF_INT_CLR_S)
#define CACHE_L2_DBUS1_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_DBUS1_OVF_INT_CLR_S  13
/** CACHE_L2_DBUS2_OVF_INT_CLR : HRO; bitpos: [14]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS2_OVF_INT_CLR    (BIT(14))
#define CACHE_L2_DBUS2_OVF_INT_CLR_M  (CACHE_L2_DBUS2_OVF_INT_CLR_V << CACHE_L2_DBUS2_OVF_INT_CLR_S)
#define CACHE_L2_DBUS2_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_DBUS2_OVF_INT_CLR_S  14
/** CACHE_L2_DBUS3_OVF_INT_CLR : HRO; bitpos: [15]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS3_OVF_INT_CLR    (BIT(15))
#define CACHE_L2_DBUS3_OVF_INT_CLR_M  (CACHE_L2_DBUS3_OVF_INT_CLR_V << CACHE_L2_DBUS3_OVF_INT_CLR_S)
#define CACHE_L2_DBUS3_OVF_INT_CLR_V  0x00000001U
#define CACHE_L2_DBUS3_OVF_INT_CLR_S  15

/** CACHE_L2_CACHE_ACS_CNT_INT_RAW_REG register
 *  Cache Access Counter Interrupt raw register
 */
#define CACHE_L2_CACHE_ACS_CNT_INT_RAW_REG (DR_REG_CACHE_BASE + 0x2e0)
/** CACHE_L2_IBUS0_OVF_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus0 accesses L2-ICache0.
 */
#define CACHE_L2_IBUS0_OVF_INT_RAW    (BIT(8))
#define CACHE_L2_IBUS0_OVF_INT_RAW_M  (CACHE_L2_IBUS0_OVF_INT_RAW_V << CACHE_L2_IBUS0_OVF_INT_RAW_S)
#define CACHE_L2_IBUS0_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_IBUS0_OVF_INT_RAW_S  8
/** CACHE_L2_IBUS1_OVF_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus1 accesses L2-ICache1.
 */
#define CACHE_L2_IBUS1_OVF_INT_RAW    (BIT(9))
#define CACHE_L2_IBUS1_OVF_INT_RAW_M  (CACHE_L2_IBUS1_OVF_INT_RAW_V << CACHE_L2_IBUS1_OVF_INT_RAW_S)
#define CACHE_L2_IBUS1_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_IBUS1_OVF_INT_RAW_S  9
/** CACHE_L2_IBUS2_OVF_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus2 accesses L2-ICache2.
 */
#define CACHE_L2_IBUS2_OVF_INT_RAW    (BIT(10))
#define CACHE_L2_IBUS2_OVF_INT_RAW_M  (CACHE_L2_IBUS2_OVF_INT_RAW_V << CACHE_L2_IBUS2_OVF_INT_RAW_S)
#define CACHE_L2_IBUS2_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_IBUS2_OVF_INT_RAW_S  10
/** CACHE_L2_IBUS3_OVF_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus3 accesses L2-ICache3.
 */
#define CACHE_L2_IBUS3_OVF_INT_RAW    (BIT(11))
#define CACHE_L2_IBUS3_OVF_INT_RAW_M  (CACHE_L2_IBUS3_OVF_INT_RAW_V << CACHE_L2_IBUS3_OVF_INT_RAW_S)
#define CACHE_L2_IBUS3_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_IBUS3_OVF_INT_RAW_S  11
/** CACHE_L2_DBUS0_OVF_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus0 accesses L2-DCache.
 */
#define CACHE_L2_DBUS0_OVF_INT_RAW    (BIT(12))
#define CACHE_L2_DBUS0_OVF_INT_RAW_M  (CACHE_L2_DBUS0_OVF_INT_RAW_V << CACHE_L2_DBUS0_OVF_INT_RAW_S)
#define CACHE_L2_DBUS0_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_DBUS0_OVF_INT_RAW_S  12
/** CACHE_L2_DBUS1_OVF_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus1 accesses L2-DCache.
 */
#define CACHE_L2_DBUS1_OVF_INT_RAW    (BIT(13))
#define CACHE_L2_DBUS1_OVF_INT_RAW_M  (CACHE_L2_DBUS1_OVF_INT_RAW_V << CACHE_L2_DBUS1_OVF_INT_RAW_S)
#define CACHE_L2_DBUS1_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_DBUS1_OVF_INT_RAW_S  13
/** CACHE_L2_DBUS2_OVF_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus2 accesses L2-DCache.
 */
#define CACHE_L2_DBUS2_OVF_INT_RAW    (BIT(14))
#define CACHE_L2_DBUS2_OVF_INT_RAW_M  (CACHE_L2_DBUS2_OVF_INT_RAW_V << CACHE_L2_DBUS2_OVF_INT_RAW_S)
#define CACHE_L2_DBUS2_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_DBUS2_OVF_INT_RAW_S  14
/** CACHE_L2_DBUS3_OVF_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The raw bit of the interrupt of one of counters overflow that occurs in L2-Cache
 *  due to bus3 accesses L2-DCache.
 */
#define CACHE_L2_DBUS3_OVF_INT_RAW    (BIT(15))
#define CACHE_L2_DBUS3_OVF_INT_RAW_M  (CACHE_L2_DBUS3_OVF_INT_RAW_V << CACHE_L2_DBUS3_OVF_INT_RAW_S)
#define CACHE_L2_DBUS3_OVF_INT_RAW_V  0x00000001U
#define CACHE_L2_DBUS3_OVF_INT_RAW_S  15

/** CACHE_L2_CACHE_ACS_CNT_INT_ST_REG register
 *  Cache Access Counter Interrupt status register
 */
#define CACHE_L2_CACHE_ACS_CNT_INT_ST_REG (DR_REG_CACHE_BASE + 0x2e4)
/** CACHE_L2_IBUS0_OVF_INT_ST : RO; bitpos: [8]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L2-Cache due to bus0 accesses L2-Cache.
 */
#define CACHE_L2_IBUS0_OVF_INT_ST    (BIT(8))
#define CACHE_L2_IBUS0_OVF_INT_ST_M  (CACHE_L2_IBUS0_OVF_INT_ST_V << CACHE_L2_IBUS0_OVF_INT_ST_S)
#define CACHE_L2_IBUS0_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_IBUS0_OVF_INT_ST_S  8
/** CACHE_L2_IBUS1_OVF_INT_ST : RO; bitpos: [9]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L2-Cache due to bus1 accesses L2-Cache.
 */
#define CACHE_L2_IBUS1_OVF_INT_ST    (BIT(9))
#define CACHE_L2_IBUS1_OVF_INT_ST_M  (CACHE_L2_IBUS1_OVF_INT_ST_V << CACHE_L2_IBUS1_OVF_INT_ST_S)
#define CACHE_L2_IBUS1_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_IBUS1_OVF_INT_ST_S  9
/** CACHE_L2_IBUS2_OVF_INT_ST : HRO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS2_OVF_INT_ST    (BIT(10))
#define CACHE_L2_IBUS2_OVF_INT_ST_M  (CACHE_L2_IBUS2_OVF_INT_ST_V << CACHE_L2_IBUS2_OVF_INT_ST_S)
#define CACHE_L2_IBUS2_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_IBUS2_OVF_INT_ST_S  10
/** CACHE_L2_IBUS3_OVF_INT_ST : HRO; bitpos: [11]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS3_OVF_INT_ST    (BIT(11))
#define CACHE_L2_IBUS3_OVF_INT_ST_M  (CACHE_L2_IBUS3_OVF_INT_ST_V << CACHE_L2_IBUS3_OVF_INT_ST_S)
#define CACHE_L2_IBUS3_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_IBUS3_OVF_INT_ST_S  11
/** CACHE_L2_DBUS0_OVF_INT_ST : RO; bitpos: [12]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L2-Cache due to bus0 accesses L2-Cache.
 */
#define CACHE_L2_DBUS0_OVF_INT_ST    (BIT(12))
#define CACHE_L2_DBUS0_OVF_INT_ST_M  (CACHE_L2_DBUS0_OVF_INT_ST_V << CACHE_L2_DBUS0_OVF_INT_ST_S)
#define CACHE_L2_DBUS0_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_DBUS0_OVF_INT_ST_S  12
/** CACHE_L2_DBUS1_OVF_INT_ST : RO; bitpos: [13]; default: 0;
 *  The bit indicates the interrupt status of one of counters overflow that occurs in
 *  L2-Cache due to bus1 accesses L2-Cache.
 */
#define CACHE_L2_DBUS1_OVF_INT_ST    (BIT(13))
#define CACHE_L2_DBUS1_OVF_INT_ST_M  (CACHE_L2_DBUS1_OVF_INT_ST_V << CACHE_L2_DBUS1_OVF_INT_ST_S)
#define CACHE_L2_DBUS1_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_DBUS1_OVF_INT_ST_S  13
/** CACHE_L2_DBUS2_OVF_INT_ST : HRO; bitpos: [14]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS2_OVF_INT_ST    (BIT(14))
#define CACHE_L2_DBUS2_OVF_INT_ST_M  (CACHE_L2_DBUS2_OVF_INT_ST_V << CACHE_L2_DBUS2_OVF_INT_ST_S)
#define CACHE_L2_DBUS2_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_DBUS2_OVF_INT_ST_S  14
/** CACHE_L2_DBUS3_OVF_INT_ST : HRO; bitpos: [15]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS3_OVF_INT_ST    (BIT(15))
#define CACHE_L2_DBUS3_OVF_INT_ST_M  (CACHE_L2_DBUS3_OVF_INT_ST_V << CACHE_L2_DBUS3_OVF_INT_ST_S)
#define CACHE_L2_DBUS3_OVF_INT_ST_V  0x00000001U
#define CACHE_L2_DBUS3_OVF_INT_ST_S  15

/** CACHE_L2_CACHE_ACS_FAIL_CTRL_REG register
 *  Cache Access Fail Configuration register
 */
#define CACHE_L2_CACHE_ACS_FAIL_CTRL_REG (DR_REG_CACHE_BASE + 0x2e8)
/** CACHE_L2_CACHE_ACS_FAIL_CHECK_MODE : R/W; bitpos: [0]; default: 0;
 *  The bit is used to configure l2 cache access fail check mode. 0: the access fail is
 *  not propagated to the request, 1: the access fail is propagated to the request
 */
#define CACHE_L2_CACHE_ACS_FAIL_CHECK_MODE    (BIT(0))
#define CACHE_L2_CACHE_ACS_FAIL_CHECK_MODE_M  (CACHE_L2_CACHE_ACS_FAIL_CHECK_MODE_V << CACHE_L2_CACHE_ACS_FAIL_CHECK_MODE_S)
#define CACHE_L2_CACHE_ACS_FAIL_CHECK_MODE_V  0x00000001U
#define CACHE_L2_CACHE_ACS_FAIL_CHECK_MODE_S  0

/** CACHE_L2_CACHE_ACS_FAIL_INT_ENA_REG register
 *  Cache Access Fail Interrupt enable register
 */
#define CACHE_L2_CACHE_ACS_FAIL_INT_ENA_REG (DR_REG_CACHE_BASE + 0x2ec)
/** CACHE_L2_CACHE_FAIL_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable interrupt of access fail that occurs in L2-Cache due to
 *  l1 cache accesses L2-Cache.
 */
#define CACHE_L2_CACHE_FAIL_INT_ENA    (BIT(5))
#define CACHE_L2_CACHE_FAIL_INT_ENA_M  (CACHE_L2_CACHE_FAIL_INT_ENA_V << CACHE_L2_CACHE_FAIL_INT_ENA_S)
#define CACHE_L2_CACHE_FAIL_INT_ENA_V  0x00000001U
#define CACHE_L2_CACHE_FAIL_INT_ENA_S  5

/** CACHE_L2_CACHE_ACS_FAIL_INT_CLR_REG register
 *  L1-Cache Access Fail Interrupt clear register
 */
#define CACHE_L2_CACHE_ACS_FAIL_INT_CLR_REG (DR_REG_CACHE_BASE + 0x2f0)
/** CACHE_L2_CACHE_FAIL_INT_CLR : WT; bitpos: [5]; default: 0;
 *  The bit is used to clear interrupt of access fail that occurs in L2-Cache due to l1
 *  cache accesses L2-Cache.
 */
#define CACHE_L2_CACHE_FAIL_INT_CLR    (BIT(5))
#define CACHE_L2_CACHE_FAIL_INT_CLR_M  (CACHE_L2_CACHE_FAIL_INT_CLR_V << CACHE_L2_CACHE_FAIL_INT_CLR_S)
#define CACHE_L2_CACHE_FAIL_INT_CLR_V  0x00000001U
#define CACHE_L2_CACHE_FAIL_INT_CLR_S  5

/** CACHE_L2_CACHE_ACS_FAIL_INT_RAW_REG register
 *  Cache Access Fail Interrupt raw register
 */
#define CACHE_L2_CACHE_ACS_FAIL_INT_RAW_REG (DR_REG_CACHE_BASE + 0x2f4)
/** CACHE_L2_CACHE_FAIL_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw bit of the interrupt of access fail that occurs in L2-Cache.
 */
#define CACHE_L2_CACHE_FAIL_INT_RAW    (BIT(5))
#define CACHE_L2_CACHE_FAIL_INT_RAW_M  (CACHE_L2_CACHE_FAIL_INT_RAW_V << CACHE_L2_CACHE_FAIL_INT_RAW_S)
#define CACHE_L2_CACHE_FAIL_INT_RAW_V  0x00000001U
#define CACHE_L2_CACHE_FAIL_INT_RAW_S  5

/** CACHE_L2_CACHE_ACS_FAIL_INT_ST_REG register
 *  Cache Access Fail Interrupt status register
 */
#define CACHE_L2_CACHE_ACS_FAIL_INT_ST_REG (DR_REG_CACHE_BASE + 0x2f8)
/** CACHE_L2_CACHE_FAIL_INT_ST : RO; bitpos: [5]; default: 0;
 *  The bit indicates the interrupt status of access fail that occurs in L2-Cache due
 *  to l1 cache accesses L2-Cache.
 */
#define CACHE_L2_CACHE_FAIL_INT_ST    (BIT(5))
#define CACHE_L2_CACHE_FAIL_INT_ST_M  (CACHE_L2_CACHE_FAIL_INT_ST_V << CACHE_L2_CACHE_FAIL_INT_ST_S)
#define CACHE_L2_CACHE_FAIL_INT_ST_V  0x00000001U
#define CACHE_L2_CACHE_FAIL_INT_ST_S  5

/** CACHE_L2_CACHE_ACS_CNT_CTRL_REG register
 *  Cache Access Counter enable and clear register
 */
#define CACHE_L2_CACHE_ACS_CNT_CTRL_REG (DR_REG_CACHE_BASE + 0x2fc)
/** CACHE_L2_IBUS0_CNT_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable ibus0 counter in L2-Cache.
 */
#define CACHE_L2_IBUS0_CNT_ENA    (BIT(8))
#define CACHE_L2_IBUS0_CNT_ENA_M  (CACHE_L2_IBUS0_CNT_ENA_V << CACHE_L2_IBUS0_CNT_ENA_S)
#define CACHE_L2_IBUS0_CNT_ENA_V  0x00000001U
#define CACHE_L2_IBUS0_CNT_ENA_S  8
/** CACHE_L2_IBUS1_CNT_ENA : R/W; bitpos: [9]; default: 0;
 *  The bit is used to enable ibus1 counter in L2-Cache.
 */
#define CACHE_L2_IBUS1_CNT_ENA    (BIT(9))
#define CACHE_L2_IBUS1_CNT_ENA_M  (CACHE_L2_IBUS1_CNT_ENA_V << CACHE_L2_IBUS1_CNT_ENA_S)
#define CACHE_L2_IBUS1_CNT_ENA_V  0x00000001U
#define CACHE_L2_IBUS1_CNT_ENA_S  9
/** CACHE_L2_IBUS2_CNT_ENA : HRO; bitpos: [10]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS2_CNT_ENA    (BIT(10))
#define CACHE_L2_IBUS2_CNT_ENA_M  (CACHE_L2_IBUS2_CNT_ENA_V << CACHE_L2_IBUS2_CNT_ENA_S)
#define CACHE_L2_IBUS2_CNT_ENA_V  0x00000001U
#define CACHE_L2_IBUS2_CNT_ENA_S  10
/** CACHE_L2_IBUS3_CNT_ENA : HRO; bitpos: [11]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS3_CNT_ENA    (BIT(11))
#define CACHE_L2_IBUS3_CNT_ENA_M  (CACHE_L2_IBUS3_CNT_ENA_V << CACHE_L2_IBUS3_CNT_ENA_S)
#define CACHE_L2_IBUS3_CNT_ENA_V  0x00000001U
#define CACHE_L2_IBUS3_CNT_ENA_S  11
/** CACHE_L2_DBUS0_CNT_ENA : R/W; bitpos: [12]; default: 0;
 *  The bit is used to enable dbus0 counter in L2-Cache.
 */
#define CACHE_L2_DBUS0_CNT_ENA    (BIT(12))
#define CACHE_L2_DBUS0_CNT_ENA_M  (CACHE_L2_DBUS0_CNT_ENA_V << CACHE_L2_DBUS0_CNT_ENA_S)
#define CACHE_L2_DBUS0_CNT_ENA_V  0x00000001U
#define CACHE_L2_DBUS0_CNT_ENA_S  12
/** CACHE_L2_DBUS1_CNT_ENA : R/W; bitpos: [13]; default: 0;
 *  The bit is used to enable dbus1 counter in L2-Cache.
 */
#define CACHE_L2_DBUS1_CNT_ENA    (BIT(13))
#define CACHE_L2_DBUS1_CNT_ENA_M  (CACHE_L2_DBUS1_CNT_ENA_V << CACHE_L2_DBUS1_CNT_ENA_S)
#define CACHE_L2_DBUS1_CNT_ENA_V  0x00000001U
#define CACHE_L2_DBUS1_CNT_ENA_S  13
/** CACHE_L2_DBUS2_CNT_ENA : HRO; bitpos: [14]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS2_CNT_ENA    (BIT(14))
#define CACHE_L2_DBUS2_CNT_ENA_M  (CACHE_L2_DBUS2_CNT_ENA_V << CACHE_L2_DBUS2_CNT_ENA_S)
#define CACHE_L2_DBUS2_CNT_ENA_V  0x00000001U
#define CACHE_L2_DBUS2_CNT_ENA_S  14
/** CACHE_L2_DBUS3_CNT_ENA : HRO; bitpos: [15]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS3_CNT_ENA    (BIT(15))
#define CACHE_L2_DBUS3_CNT_ENA_M  (CACHE_L2_DBUS3_CNT_ENA_V << CACHE_L2_DBUS3_CNT_ENA_S)
#define CACHE_L2_DBUS3_CNT_ENA_V  0x00000001U
#define CACHE_L2_DBUS3_CNT_ENA_S  15
/** CACHE_L2_IBUS0_CNT_CLR : WT; bitpos: [24]; default: 0;
 *  The bit is used to clear ibus0 counter in L2-Cache.
 */
#define CACHE_L2_IBUS0_CNT_CLR    (BIT(24))
#define CACHE_L2_IBUS0_CNT_CLR_M  (CACHE_L2_IBUS0_CNT_CLR_V << CACHE_L2_IBUS0_CNT_CLR_S)
#define CACHE_L2_IBUS0_CNT_CLR_V  0x00000001U
#define CACHE_L2_IBUS0_CNT_CLR_S  24
/** CACHE_L2_IBUS1_CNT_CLR : WT; bitpos: [25]; default: 0;
 *  The bit is used to clear ibus1 counter in L2-Cache.
 */
#define CACHE_L2_IBUS1_CNT_CLR    (BIT(25))
#define CACHE_L2_IBUS1_CNT_CLR_M  (CACHE_L2_IBUS1_CNT_CLR_V << CACHE_L2_IBUS1_CNT_CLR_S)
#define CACHE_L2_IBUS1_CNT_CLR_V  0x00000001U
#define CACHE_L2_IBUS1_CNT_CLR_S  25
/** CACHE_L2_IBUS2_CNT_CLR : HRO; bitpos: [26]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS2_CNT_CLR    (BIT(26))
#define CACHE_L2_IBUS2_CNT_CLR_M  (CACHE_L2_IBUS2_CNT_CLR_V << CACHE_L2_IBUS2_CNT_CLR_S)
#define CACHE_L2_IBUS2_CNT_CLR_V  0x00000001U
#define CACHE_L2_IBUS2_CNT_CLR_S  26
/** CACHE_L2_IBUS3_CNT_CLR : HRO; bitpos: [27]; default: 0;
 *  Reserved
 */
#define CACHE_L2_IBUS3_CNT_CLR    (BIT(27))
#define CACHE_L2_IBUS3_CNT_CLR_M  (CACHE_L2_IBUS3_CNT_CLR_V << CACHE_L2_IBUS3_CNT_CLR_S)
#define CACHE_L2_IBUS3_CNT_CLR_V  0x00000001U
#define CACHE_L2_IBUS3_CNT_CLR_S  27
/** CACHE_L2_DBUS0_CNT_CLR : WT; bitpos: [28]; default: 0;
 *  The bit is used to clear dbus0 counter in L2-Cache.
 */
#define CACHE_L2_DBUS0_CNT_CLR    (BIT(28))
#define CACHE_L2_DBUS0_CNT_CLR_M  (CACHE_L2_DBUS0_CNT_CLR_V << CACHE_L2_DBUS0_CNT_CLR_S)
#define CACHE_L2_DBUS0_CNT_CLR_V  0x00000001U
#define CACHE_L2_DBUS0_CNT_CLR_S  28
/** CACHE_L2_DBUS1_CNT_CLR : WT; bitpos: [29]; default: 0;
 *  The bit is used to clear dbus1 counter in L2-Cache.
 */
#define CACHE_L2_DBUS1_CNT_CLR    (BIT(29))
#define CACHE_L2_DBUS1_CNT_CLR_M  (CACHE_L2_DBUS1_CNT_CLR_V << CACHE_L2_DBUS1_CNT_CLR_S)
#define CACHE_L2_DBUS1_CNT_CLR_V  0x00000001U
#define CACHE_L2_DBUS1_CNT_CLR_S  29
/** CACHE_L2_DBUS2_CNT_CLR : HRO; bitpos: [30]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS2_CNT_CLR    (BIT(30))
#define CACHE_L2_DBUS2_CNT_CLR_M  (CACHE_L2_DBUS2_CNT_CLR_V << CACHE_L2_DBUS2_CNT_CLR_S)
#define CACHE_L2_DBUS2_CNT_CLR_V  0x00000001U
#define CACHE_L2_DBUS2_CNT_CLR_S  30
/** CACHE_L2_DBUS3_CNT_CLR : HRO; bitpos: [31]; default: 0;
 *  Reserved
 */
#define CACHE_L2_DBUS3_CNT_CLR    (BIT(31))
#define CACHE_L2_DBUS3_CNT_CLR_M  (CACHE_L2_DBUS3_CNT_CLR_V << CACHE_L2_DBUS3_CNT_CLR_S)
#define CACHE_L2_DBUS3_CNT_CLR_V  0x00000001U
#define CACHE_L2_DBUS3_CNT_CLR_S  31

/** CACHE_L2_IBUS0_ACS_HIT_CNT_REG register
 *  L2-Cache bus0 Hit-Access Counter register
 */
#define CACHE_L2_IBUS0_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x300)
/** CACHE_L2_IBUS0_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-ICache0 accesses L2-Cache due to
 *  bus0 accessing L1-ICache0.
 */
#define CACHE_L2_IBUS0_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS0_HIT_CNT_M  (CACHE_L2_IBUS0_HIT_CNT_V << CACHE_L2_IBUS0_HIT_CNT_S)
#define CACHE_L2_IBUS0_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS0_HIT_CNT_S  0

/** CACHE_L2_IBUS0_ACS_MISS_CNT_REG register
 *  L2-Cache bus0 Miss-Access Counter register
 */
#define CACHE_L2_IBUS0_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x304)
/** CACHE_L2_IBUS0_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-ICache0 accesses L2-Cache due to
 *  bus0 accessing L1-ICache0.
 */
#define CACHE_L2_IBUS0_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS0_MISS_CNT_M  (CACHE_L2_IBUS0_MISS_CNT_V << CACHE_L2_IBUS0_MISS_CNT_S)
#define CACHE_L2_IBUS0_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS0_MISS_CNT_S  0

/** CACHE_L2_IBUS0_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus0 Conflict-Access Counter register
 */
#define CACHE_L2_IBUS0_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x308)
/** CACHE_L2_IBUS0_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-ICache0 accesses
 *  L2-Cache due to bus0 accessing L1-ICache0.
 */
#define CACHE_L2_IBUS0_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS0_CONFLICT_CNT_M  (CACHE_L2_IBUS0_CONFLICT_CNT_V << CACHE_L2_IBUS0_CONFLICT_CNT_S)
#define CACHE_L2_IBUS0_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS0_CONFLICT_CNT_S  0

/** CACHE_L2_IBUS0_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus0 Next-Level-Access Counter register
 */
#define CACHE_L2_IBUS0_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x30c)
/** CACHE_L2_IBUS0_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-ICache0 accessing L2-Cache due to bus0 accessing L1-ICache0.
 */
#define CACHE_L2_IBUS0_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS0_NXTLVL_RD_CNT_M  (CACHE_L2_IBUS0_NXTLVL_RD_CNT_V << CACHE_L2_IBUS0_NXTLVL_RD_CNT_S)
#define CACHE_L2_IBUS0_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS0_NXTLVL_RD_CNT_S  0

/** CACHE_L2_IBUS1_ACS_HIT_CNT_REG register
 *  L2-Cache bus1 Hit-Access Counter register
 */
#define CACHE_L2_IBUS1_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x310)
/** CACHE_L2_IBUS1_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-ICache1 accesses L2-Cache due to
 *  bus1 accessing L1-ICache1.
 */
#define CACHE_L2_IBUS1_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS1_HIT_CNT_M  (CACHE_L2_IBUS1_HIT_CNT_V << CACHE_L2_IBUS1_HIT_CNT_S)
#define CACHE_L2_IBUS1_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS1_HIT_CNT_S  0

/** CACHE_L2_IBUS1_ACS_MISS_CNT_REG register
 *  L2-Cache bus1 Miss-Access Counter register
 */
#define CACHE_L2_IBUS1_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x314)
/** CACHE_L2_IBUS1_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-ICache1 accesses L2-Cache due to
 *  bus1 accessing L1-ICache1.
 */
#define CACHE_L2_IBUS1_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS1_MISS_CNT_M  (CACHE_L2_IBUS1_MISS_CNT_V << CACHE_L2_IBUS1_MISS_CNT_S)
#define CACHE_L2_IBUS1_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS1_MISS_CNT_S  0

/** CACHE_L2_IBUS1_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus1 Conflict-Access Counter register
 */
#define CACHE_L2_IBUS1_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x318)
/** CACHE_L2_IBUS1_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-ICache1 accesses
 *  L2-Cache due to bus1 accessing L1-ICache1.
 */
#define CACHE_L2_IBUS1_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS1_CONFLICT_CNT_M  (CACHE_L2_IBUS1_CONFLICT_CNT_V << CACHE_L2_IBUS1_CONFLICT_CNT_S)
#define CACHE_L2_IBUS1_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS1_CONFLICT_CNT_S  0

/** CACHE_L2_IBUS1_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus1 Next-Level-Access Counter register
 */
#define CACHE_L2_IBUS1_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x31c)
/** CACHE_L2_IBUS1_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-ICache1 accessing L2-Cache due to bus1 accessing L1-ICache1.
 */
#define CACHE_L2_IBUS1_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS1_NXTLVL_RD_CNT_M  (CACHE_L2_IBUS1_NXTLVL_RD_CNT_V << CACHE_L2_IBUS1_NXTLVL_RD_CNT_S)
#define CACHE_L2_IBUS1_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS1_NXTLVL_RD_CNT_S  0

/** CACHE_L2_IBUS2_ACS_HIT_CNT_REG register
 *  L2-Cache bus2 Hit-Access Counter register
 */
#define CACHE_L2_IBUS2_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x320)
/** CACHE_L2_IBUS2_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-ICache2 accesses L2-Cache due to
 *  bus2 accessing L1-ICache2.
 */
#define CACHE_L2_IBUS2_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS2_HIT_CNT_M  (CACHE_L2_IBUS2_HIT_CNT_V << CACHE_L2_IBUS2_HIT_CNT_S)
#define CACHE_L2_IBUS2_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS2_HIT_CNT_S  0

/** CACHE_L2_IBUS2_ACS_MISS_CNT_REG register
 *  L2-Cache bus2 Miss-Access Counter register
 */
#define CACHE_L2_IBUS2_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x324)
/** CACHE_L2_IBUS2_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-ICache2 accesses L2-Cache due to
 *  bus2 accessing L1-ICache2.
 */
#define CACHE_L2_IBUS2_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS2_MISS_CNT_M  (CACHE_L2_IBUS2_MISS_CNT_V << CACHE_L2_IBUS2_MISS_CNT_S)
#define CACHE_L2_IBUS2_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS2_MISS_CNT_S  0

/** CACHE_L2_IBUS2_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus2 Conflict-Access Counter register
 */
#define CACHE_L2_IBUS2_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x328)
/** CACHE_L2_IBUS2_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-ICache2 accesses
 *  L2-Cache due to bus2 accessing L1-ICache2.
 */
#define CACHE_L2_IBUS2_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS2_CONFLICT_CNT_M  (CACHE_L2_IBUS2_CONFLICT_CNT_V << CACHE_L2_IBUS2_CONFLICT_CNT_S)
#define CACHE_L2_IBUS2_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS2_CONFLICT_CNT_S  0

/** CACHE_L2_IBUS2_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus2 Next-Level-Access Counter register
 */
#define CACHE_L2_IBUS2_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x32c)
/** CACHE_L2_IBUS2_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-ICache2 accessing L2-Cache due to bus2 accessing L1-ICache2.
 */
#define CACHE_L2_IBUS2_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS2_NXTLVL_RD_CNT_M  (CACHE_L2_IBUS2_NXTLVL_RD_CNT_V << CACHE_L2_IBUS2_NXTLVL_RD_CNT_S)
#define CACHE_L2_IBUS2_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS2_NXTLVL_RD_CNT_S  0

/** CACHE_L2_IBUS3_ACS_HIT_CNT_REG register
 *  L2-Cache bus3 Hit-Access Counter register
 */
#define CACHE_L2_IBUS3_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x330)
/** CACHE_L2_IBUS3_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-ICache3 accesses L2-Cache due to
 *  bus3 accessing L1-ICache3.
 */
#define CACHE_L2_IBUS3_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS3_HIT_CNT_M  (CACHE_L2_IBUS3_HIT_CNT_V << CACHE_L2_IBUS3_HIT_CNT_S)
#define CACHE_L2_IBUS3_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS3_HIT_CNT_S  0

/** CACHE_L2_IBUS3_ACS_MISS_CNT_REG register
 *  L2-Cache bus3 Miss-Access Counter register
 */
#define CACHE_L2_IBUS3_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x334)
/** CACHE_L2_IBUS3_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-ICache3 accesses L2-Cache due to
 *  bus3 accessing L1-ICache3.
 */
#define CACHE_L2_IBUS3_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS3_MISS_CNT_M  (CACHE_L2_IBUS3_MISS_CNT_V << CACHE_L2_IBUS3_MISS_CNT_S)
#define CACHE_L2_IBUS3_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS3_MISS_CNT_S  0

/** CACHE_L2_IBUS3_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus3 Conflict-Access Counter register
 */
#define CACHE_L2_IBUS3_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x338)
/** CACHE_L2_IBUS3_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-ICache3 accesses
 *  L2-Cache due to bus3 accessing L1-ICache3.
 */
#define CACHE_L2_IBUS3_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS3_CONFLICT_CNT_M  (CACHE_L2_IBUS3_CONFLICT_CNT_V << CACHE_L2_IBUS3_CONFLICT_CNT_S)
#define CACHE_L2_IBUS3_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS3_CONFLICT_CNT_S  0

/** CACHE_L2_IBUS3_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus3 Next-Level-Access Counter register
 */
#define CACHE_L2_IBUS3_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x33c)
/** CACHE_L2_IBUS3_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-ICache3 accessing L2-Cache due to bus3 accessing L1-ICache3.
 */
#define CACHE_L2_IBUS3_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_IBUS3_NXTLVL_RD_CNT_M  (CACHE_L2_IBUS3_NXTLVL_RD_CNT_V << CACHE_L2_IBUS3_NXTLVL_RD_CNT_S)
#define CACHE_L2_IBUS3_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_IBUS3_NXTLVL_RD_CNT_S  0

/** CACHE_L2_DBUS0_ACS_HIT_CNT_REG register
 *  L2-Cache bus0 Hit-Access Counter register
 */
#define CACHE_L2_DBUS0_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x340)
/** CACHE_L2_DBUS0_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-DCache accesses L2-Cache due to
 *  bus0 accessing L1-DCache.
 */
#define CACHE_L2_DBUS0_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS0_HIT_CNT_M  (CACHE_L2_DBUS0_HIT_CNT_V << CACHE_L2_DBUS0_HIT_CNT_S)
#define CACHE_L2_DBUS0_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS0_HIT_CNT_S  0

/** CACHE_L2_DBUS0_ACS_MISS_CNT_REG register
 *  L2-Cache bus0 Miss-Access Counter register
 */
#define CACHE_L2_DBUS0_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x344)
/** CACHE_L2_DBUS0_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-DCache accesses L2-Cache due to
 *  bus0 accessing L1-DCache.
 */
#define CACHE_L2_DBUS0_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS0_MISS_CNT_M  (CACHE_L2_DBUS0_MISS_CNT_V << CACHE_L2_DBUS0_MISS_CNT_S)
#define CACHE_L2_DBUS0_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS0_MISS_CNT_S  0

/** CACHE_L2_DBUS0_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus0 Conflict-Access Counter register
 */
#define CACHE_L2_DBUS0_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x348)
/** CACHE_L2_DBUS0_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-DCache accesses
 *  L2-Cache due to bus0 accessing L1-DCache.
 */
#define CACHE_L2_DBUS0_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS0_CONFLICT_CNT_M  (CACHE_L2_DBUS0_CONFLICT_CNT_V << CACHE_L2_DBUS0_CONFLICT_CNT_S)
#define CACHE_L2_DBUS0_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS0_CONFLICT_CNT_S  0

/** CACHE_L2_DBUS0_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus0 Next-Level-Access Counter register
 */
#define CACHE_L2_DBUS0_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x34c)
/** CACHE_L2_DBUS0_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-DCache accessing L2-Cache due to bus0 accessing L1-DCache.
 */
#define CACHE_L2_DBUS0_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS0_NXTLVL_RD_CNT_M  (CACHE_L2_DBUS0_NXTLVL_RD_CNT_V << CACHE_L2_DBUS0_NXTLVL_RD_CNT_S)
#define CACHE_L2_DBUS0_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS0_NXTLVL_RD_CNT_S  0

/** CACHE_L2_DBUS0_ACS_NXTLVL_WR_CNT_REG register
 *  L2-Cache bus0 WB-Access Counter register
 */
#define CACHE_L2_DBUS0_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x350)
/** CACHE_L2_DBUS0_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when L1-DCache accesses L2-Cache due
 *  to bus0 accessing L1-DCache.
 */
#define CACHE_L2_DBUS0_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS0_NXTLVL_WR_CNT_M  (CACHE_L2_DBUS0_NXTLVL_WR_CNT_V << CACHE_L2_DBUS0_NXTLVL_WR_CNT_S)
#define CACHE_L2_DBUS0_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS0_NXTLVL_WR_CNT_S  0

/** CACHE_L2_DBUS1_ACS_HIT_CNT_REG register
 *  L2-Cache bus1 Hit-Access Counter register
 */
#define CACHE_L2_DBUS1_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x354)
/** CACHE_L2_DBUS1_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-DCache accesses L2-Cache due to
 *  bus1 accessing L1-DCache.
 */
#define CACHE_L2_DBUS1_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS1_HIT_CNT_M  (CACHE_L2_DBUS1_HIT_CNT_V << CACHE_L2_DBUS1_HIT_CNT_S)
#define CACHE_L2_DBUS1_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS1_HIT_CNT_S  0

/** CACHE_L2_DBUS1_ACS_MISS_CNT_REG register
 *  L2-Cache bus1 Miss-Access Counter register
 */
#define CACHE_L2_DBUS1_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x358)
/** CACHE_L2_DBUS1_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-DCache accesses L2-Cache due to
 *  bus1 accessing L1-DCache.
 */
#define CACHE_L2_DBUS1_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS1_MISS_CNT_M  (CACHE_L2_DBUS1_MISS_CNT_V << CACHE_L2_DBUS1_MISS_CNT_S)
#define CACHE_L2_DBUS1_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS1_MISS_CNT_S  0

/** CACHE_L2_DBUS1_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus1 Conflict-Access Counter register
 */
#define CACHE_L2_DBUS1_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x35c)
/** CACHE_L2_DBUS1_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-DCache accesses
 *  L2-Cache due to bus1 accessing L1-DCache.
 */
#define CACHE_L2_DBUS1_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS1_CONFLICT_CNT_M  (CACHE_L2_DBUS1_CONFLICT_CNT_V << CACHE_L2_DBUS1_CONFLICT_CNT_S)
#define CACHE_L2_DBUS1_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS1_CONFLICT_CNT_S  0

/** CACHE_L2_DBUS1_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus1 Next-Level-Access Counter register
 */
#define CACHE_L2_DBUS1_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x360)
/** CACHE_L2_DBUS1_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-DCache accessing L2-Cache due to bus1 accessing L1-DCache.
 */
#define CACHE_L2_DBUS1_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS1_NXTLVL_RD_CNT_M  (CACHE_L2_DBUS1_NXTLVL_RD_CNT_V << CACHE_L2_DBUS1_NXTLVL_RD_CNT_S)
#define CACHE_L2_DBUS1_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS1_NXTLVL_RD_CNT_S  0

/** CACHE_L2_DBUS1_ACS_NXTLVL_WR_CNT_REG register
 *  L2-Cache bus1 WB-Access Counter register
 */
#define CACHE_L2_DBUS1_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x364)
/** CACHE_L2_DBUS1_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when L1-DCache accesses L2-Cache due
 *  to bus1 accessing L1-DCache.
 */
#define CACHE_L2_DBUS1_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS1_NXTLVL_WR_CNT_M  (CACHE_L2_DBUS1_NXTLVL_WR_CNT_V << CACHE_L2_DBUS1_NXTLVL_WR_CNT_S)
#define CACHE_L2_DBUS1_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS1_NXTLVL_WR_CNT_S  0

/** CACHE_L2_DBUS2_ACS_HIT_CNT_REG register
 *  L2-Cache bus2 Hit-Access Counter register
 */
#define CACHE_L2_DBUS2_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x368)
/** CACHE_L2_DBUS2_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-DCache accesses L2-Cache due to
 *  bus2 accessing L1-DCache.
 */
#define CACHE_L2_DBUS2_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS2_HIT_CNT_M  (CACHE_L2_DBUS2_HIT_CNT_V << CACHE_L2_DBUS2_HIT_CNT_S)
#define CACHE_L2_DBUS2_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS2_HIT_CNT_S  0

/** CACHE_L2_DBUS2_ACS_MISS_CNT_REG register
 *  L2-Cache bus2 Miss-Access Counter register
 */
#define CACHE_L2_DBUS2_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x36c)
/** CACHE_L2_DBUS2_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-DCache accesses L2-Cache due to
 *  bus2 accessing L1-DCache.
 */
#define CACHE_L2_DBUS2_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS2_MISS_CNT_M  (CACHE_L2_DBUS2_MISS_CNT_V << CACHE_L2_DBUS2_MISS_CNT_S)
#define CACHE_L2_DBUS2_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS2_MISS_CNT_S  0

/** CACHE_L2_DBUS2_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus2 Conflict-Access Counter register
 */
#define CACHE_L2_DBUS2_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x370)
/** CACHE_L2_DBUS2_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-DCache accesses
 *  L2-Cache due to bus2 accessing L1-DCache.
 */
#define CACHE_L2_DBUS2_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS2_CONFLICT_CNT_M  (CACHE_L2_DBUS2_CONFLICT_CNT_V << CACHE_L2_DBUS2_CONFLICT_CNT_S)
#define CACHE_L2_DBUS2_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS2_CONFLICT_CNT_S  0

/** CACHE_L2_DBUS2_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus2 Next-Level-Access Counter register
 */
#define CACHE_L2_DBUS2_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x374)
/** CACHE_L2_DBUS2_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-DCache accessing L2-Cache due to bus2 accessing L1-DCache.
 */
#define CACHE_L2_DBUS2_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS2_NXTLVL_RD_CNT_M  (CACHE_L2_DBUS2_NXTLVL_RD_CNT_V << CACHE_L2_DBUS2_NXTLVL_RD_CNT_S)
#define CACHE_L2_DBUS2_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS2_NXTLVL_RD_CNT_S  0

/** CACHE_L2_DBUS2_ACS_NXTLVL_WR_CNT_REG register
 *  L2-Cache bus2 WB-Access Counter register
 */
#define CACHE_L2_DBUS2_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x378)
/** CACHE_L2_DBUS2_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when L1-DCache accesses L2-Cache due
 *  to bus2 accessing L1-DCache.
 */
#define CACHE_L2_DBUS2_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS2_NXTLVL_WR_CNT_M  (CACHE_L2_DBUS2_NXTLVL_WR_CNT_V << CACHE_L2_DBUS2_NXTLVL_WR_CNT_S)
#define CACHE_L2_DBUS2_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS2_NXTLVL_WR_CNT_S  0

/** CACHE_L2_DBUS3_ACS_HIT_CNT_REG register
 *  L2-Cache bus3 Hit-Access Counter register
 */
#define CACHE_L2_DBUS3_ACS_HIT_CNT_REG (DR_REG_CACHE_BASE + 0x37c)
/** CACHE_L2_DBUS3_HIT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of hits when L1-DCache accesses L2-Cache due to
 *  bus3 accessing L1-DCache.
 */
#define CACHE_L2_DBUS3_HIT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS3_HIT_CNT_M  (CACHE_L2_DBUS3_HIT_CNT_V << CACHE_L2_DBUS3_HIT_CNT_S)
#define CACHE_L2_DBUS3_HIT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS3_HIT_CNT_S  0

/** CACHE_L2_DBUS3_ACS_MISS_CNT_REG register
 *  L2-Cache bus3 Miss-Access Counter register
 */
#define CACHE_L2_DBUS3_ACS_MISS_CNT_REG (DR_REG_CACHE_BASE + 0x380)
/** CACHE_L2_DBUS3_MISS_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of missing when L1-DCache accesses L2-Cache due to
 *  bus3 accessing L1-DCache.
 */
#define CACHE_L2_DBUS3_MISS_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS3_MISS_CNT_M  (CACHE_L2_DBUS3_MISS_CNT_V << CACHE_L2_DBUS3_MISS_CNT_S)
#define CACHE_L2_DBUS3_MISS_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS3_MISS_CNT_S  0

/** CACHE_L2_DBUS3_ACS_CONFLICT_CNT_REG register
 *  L2-Cache bus3 Conflict-Access Counter register
 */
#define CACHE_L2_DBUS3_ACS_CONFLICT_CNT_REG (DR_REG_CACHE_BASE + 0x384)
/** CACHE_L2_DBUS3_CONFLICT_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of access-conflicts when L1-DCache accesses
 *  L2-Cache due to bus3 accessing L1-DCache.
 */
#define CACHE_L2_DBUS3_CONFLICT_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS3_CONFLICT_CNT_M  (CACHE_L2_DBUS3_CONFLICT_CNT_V << CACHE_L2_DBUS3_CONFLICT_CNT_S)
#define CACHE_L2_DBUS3_CONFLICT_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS3_CONFLICT_CNT_S  0

/** CACHE_L2_DBUS3_ACS_NXTLVL_RD_CNT_REG register
 *  L2-Cache bus3 Next-Level-Access Counter register
 */
#define CACHE_L2_DBUS3_ACS_NXTLVL_RD_CNT_REG (DR_REG_CACHE_BASE + 0x388)
/** CACHE_L2_DBUS3_NXTLVL_RD_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of times that L2-Cache accesses external memory due
 *  to L1-DCache accessing L2-Cache due to bus3 accessing L1-DCache.
 */
#define CACHE_L2_DBUS3_NXTLVL_RD_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS3_NXTLVL_RD_CNT_M  (CACHE_L2_DBUS3_NXTLVL_RD_CNT_V << CACHE_L2_DBUS3_NXTLVL_RD_CNT_S)
#define CACHE_L2_DBUS3_NXTLVL_RD_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS3_NXTLVL_RD_CNT_S  0

/** CACHE_L2_DBUS3_ACS_NXTLVL_WR_CNT_REG register
 *  L2-Cache bus3 WB-Access Counter register
 */
#define CACHE_L2_DBUS3_ACS_NXTLVL_WR_CNT_REG (DR_REG_CACHE_BASE + 0x38c)
/** CACHE_L2_DBUS3_NXTLVL_WR_CNT : RO; bitpos: [31:0]; default: 0;
 *  The register records the number of write back when L1-DCache accesses L2-Cache due
 *  to bus3 accessing L1-DCache.
 */
#define CACHE_L2_DBUS3_NXTLVL_WR_CNT    0xFFFFFFFFU
#define CACHE_L2_DBUS3_NXTLVL_WR_CNT_M  (CACHE_L2_DBUS3_NXTLVL_WR_CNT_V << CACHE_L2_DBUS3_NXTLVL_WR_CNT_S)
#define CACHE_L2_DBUS3_NXTLVL_WR_CNT_V  0xFFFFFFFFU
#define CACHE_L2_DBUS3_NXTLVL_WR_CNT_S  0

/** CACHE_L2_CACHE_ACS_FAIL_ID_ATTR_REG register
 *  L2-Cache Access Fail ID/attribution information register
 */
#define CACHE_L2_CACHE_ACS_FAIL_ID_ATTR_REG (DR_REG_CACHE_BASE + 0x390)
/** CACHE_L2_CACHE_FAIL_ID : RO; bitpos: [15:0]; default: 0;
 *  The register records the ID of fail-access when L1-Cache accesses L2-Cache.
 */
#define CACHE_L2_CACHE_FAIL_ID    0x0000FFFFU
#define CACHE_L2_CACHE_FAIL_ID_M  (CACHE_L2_CACHE_FAIL_ID_V << CACHE_L2_CACHE_FAIL_ID_S)
#define CACHE_L2_CACHE_FAIL_ID_V  0x0000FFFFU
#define CACHE_L2_CACHE_FAIL_ID_S  0
/** CACHE_L2_CACHE_FAIL_ATTR : RO; bitpos: [31:16]; default: 0;
 *  The register records the attribution of fail-access when L1-Cache accesses L2-Cache
 *  due to cache accessing L1-Cache.
 */
#define CACHE_L2_CACHE_FAIL_ATTR    0x0000FFFFU
#define CACHE_L2_CACHE_FAIL_ATTR_M  (CACHE_L2_CACHE_FAIL_ATTR_V << CACHE_L2_CACHE_FAIL_ATTR_S)
#define CACHE_L2_CACHE_FAIL_ATTR_V  0x0000FFFFU
#define CACHE_L2_CACHE_FAIL_ATTR_S  16

/** CACHE_L2_CACHE_ACS_FAIL_ADDR_REG register
 *  L2-Cache Access Fail Address information register
 */
#define CACHE_L2_CACHE_ACS_FAIL_ADDR_REG (DR_REG_CACHE_BASE + 0x394)
/** CACHE_L2_CACHE_FAIL_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The register records the address of fail-access when L1-Cache accesses L2-Cache.
 */
#define CACHE_L2_CACHE_FAIL_ADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_FAIL_ADDR_M  (CACHE_L2_CACHE_FAIL_ADDR_V << CACHE_L2_CACHE_FAIL_ADDR_S)
#define CACHE_L2_CACHE_FAIL_ADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_FAIL_ADDR_S  0

/** CACHE_L2_CACHE_SYNC_PRELOAD_INT_ENA_REG register
 *  L1-Cache Access Fail Interrupt enable register
 */
#define CACHE_L2_CACHE_SYNC_PRELOAD_INT_ENA_REG (DR_REG_CACHE_BASE + 0x398)
/** CACHE_L2_CACHE_PLD_DONE_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable interrupt of L2-Cache preload-operation done.
 */
#define CACHE_L2_CACHE_PLD_DONE_INT_ENA    (BIT(5))
#define CACHE_L2_CACHE_PLD_DONE_INT_ENA_M  (CACHE_L2_CACHE_PLD_DONE_INT_ENA_V << CACHE_L2_CACHE_PLD_DONE_INT_ENA_S)
#define CACHE_L2_CACHE_PLD_DONE_INT_ENA_V  0x00000001U
#define CACHE_L2_CACHE_PLD_DONE_INT_ENA_S  5
/** CACHE_L2_CACHE_PLD_ERR_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The bit is used to enable interrupt of L2-Cache preload-operation error.
 */
#define CACHE_L2_CACHE_PLD_ERR_INT_ENA    (BIT(12))
#define CACHE_L2_CACHE_PLD_ERR_INT_ENA_M  (CACHE_L2_CACHE_PLD_ERR_INT_ENA_V << CACHE_L2_CACHE_PLD_ERR_INT_ENA_S)
#define CACHE_L2_CACHE_PLD_ERR_INT_ENA_V  0x00000001U
#define CACHE_L2_CACHE_PLD_ERR_INT_ENA_S  12

/** CACHE_L2_CACHE_SYNC_PRELOAD_INT_CLR_REG register
 *  Sync Preload operation Interrupt clear register
 */
#define CACHE_L2_CACHE_SYNC_PRELOAD_INT_CLR_REG (DR_REG_CACHE_BASE + 0x39c)
/** CACHE_L2_CACHE_PLD_DONE_INT_CLR : WT; bitpos: [5]; default: 0;
 *  The bit is used to clear interrupt that occurs only when L2-Cache preload-operation
 *  is done.
 */
#define CACHE_L2_CACHE_PLD_DONE_INT_CLR    (BIT(5))
#define CACHE_L2_CACHE_PLD_DONE_INT_CLR_M  (CACHE_L2_CACHE_PLD_DONE_INT_CLR_V << CACHE_L2_CACHE_PLD_DONE_INT_CLR_S)
#define CACHE_L2_CACHE_PLD_DONE_INT_CLR_V  0x00000001U
#define CACHE_L2_CACHE_PLD_DONE_INT_CLR_S  5
/** CACHE_L2_CACHE_PLD_ERR_INT_CLR : WT; bitpos: [12]; default: 0;
 *  The bit is used to clear interrupt of L2-Cache preload-operation error.
 */
#define CACHE_L2_CACHE_PLD_ERR_INT_CLR    (BIT(12))
#define CACHE_L2_CACHE_PLD_ERR_INT_CLR_M  (CACHE_L2_CACHE_PLD_ERR_INT_CLR_V << CACHE_L2_CACHE_PLD_ERR_INT_CLR_S)
#define CACHE_L2_CACHE_PLD_ERR_INT_CLR_V  0x00000001U
#define CACHE_L2_CACHE_PLD_ERR_INT_CLR_S  12

/** CACHE_L2_CACHE_SYNC_PRELOAD_INT_RAW_REG register
 *  Sync Preload operation Interrupt raw register
 */
#define CACHE_L2_CACHE_SYNC_PRELOAD_INT_RAW_REG (DR_REG_CACHE_BASE + 0x3a0)
/** CACHE_L2_CACHE_PLD_DONE_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw bit of the interrupt that occurs only when L2-Cache preload-operation is
 *  done.
 */
#define CACHE_L2_CACHE_PLD_DONE_INT_RAW    (BIT(5))
#define CACHE_L2_CACHE_PLD_DONE_INT_RAW_M  (CACHE_L2_CACHE_PLD_DONE_INT_RAW_V << CACHE_L2_CACHE_PLD_DONE_INT_RAW_S)
#define CACHE_L2_CACHE_PLD_DONE_INT_RAW_V  0x00000001U
#define CACHE_L2_CACHE_PLD_DONE_INT_RAW_S  5
/** CACHE_L2_CACHE_PLD_ERR_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw bit of the interrupt that occurs only when L2-Cache preload-operation error
 *  occurs.
 */
#define CACHE_L2_CACHE_PLD_ERR_INT_RAW    (BIT(12))
#define CACHE_L2_CACHE_PLD_ERR_INT_RAW_M  (CACHE_L2_CACHE_PLD_ERR_INT_RAW_V << CACHE_L2_CACHE_PLD_ERR_INT_RAW_S)
#define CACHE_L2_CACHE_PLD_ERR_INT_RAW_V  0x00000001U
#define CACHE_L2_CACHE_PLD_ERR_INT_RAW_S  12

/** CACHE_L2_CACHE_SYNC_PRELOAD_INT_ST_REG register
 *  L1-Cache Access Fail Interrupt status register
 */
#define CACHE_L2_CACHE_SYNC_PRELOAD_INT_ST_REG (DR_REG_CACHE_BASE + 0x3a4)
/** CACHE_L2_CACHE_PLD_DONE_INT_ST : RO; bitpos: [5]; default: 0;
 *  The bit indicates the status of the interrupt that occurs only when L2-Cache
 *  preload-operation is done.
 */
#define CACHE_L2_CACHE_PLD_DONE_INT_ST    (BIT(5))
#define CACHE_L2_CACHE_PLD_DONE_INT_ST_M  (CACHE_L2_CACHE_PLD_DONE_INT_ST_V << CACHE_L2_CACHE_PLD_DONE_INT_ST_S)
#define CACHE_L2_CACHE_PLD_DONE_INT_ST_V  0x00000001U
#define CACHE_L2_CACHE_PLD_DONE_INT_ST_S  5
/** CACHE_L2_CACHE_PLD_ERR_INT_ST : RO; bitpos: [12]; default: 0;
 *  The bit indicates the status of the interrupt of L2-Cache preload-operation error.
 */
#define CACHE_L2_CACHE_PLD_ERR_INT_ST    (BIT(12))
#define CACHE_L2_CACHE_PLD_ERR_INT_ST_M  (CACHE_L2_CACHE_PLD_ERR_INT_ST_V << CACHE_L2_CACHE_PLD_ERR_INT_ST_S)
#define CACHE_L2_CACHE_PLD_ERR_INT_ST_V  0x00000001U
#define CACHE_L2_CACHE_PLD_ERR_INT_ST_S  12

/** CACHE_L2_CACHE_SYNC_PRELOAD_EXCEPTION_REG register
 *  Cache Sync/Preload Operation exception register
 */
#define CACHE_L2_CACHE_SYNC_PRELOAD_EXCEPTION_REG (DR_REG_CACHE_BASE + 0x3a8)
/** CACHE_L2_CACHE_PLD_ERR_CODE : RO; bitpos: [11:10]; default: 0;
 *  The value 2 is Only available which means preload size is error in L2-Cache.
 */
#define CACHE_L2_CACHE_PLD_ERR_CODE    0x00000003U
#define CACHE_L2_CACHE_PLD_ERR_CODE_M  (CACHE_L2_CACHE_PLD_ERR_CODE_V << CACHE_L2_CACHE_PLD_ERR_CODE_S)
#define CACHE_L2_CACHE_PLD_ERR_CODE_V  0x00000003U
#define CACHE_L2_CACHE_PLD_ERR_CODE_S  10

/** CACHE_L2_CACHE_SYNC_RST_CTRL_REG register
 *  Cache Sync Reset control register
 */
#define CACHE_L2_CACHE_SYNC_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x3ac)
/** CACHE_L2_CACHE_SYNC_RST : R/W; bitpos: [5]; default: 0;
 *  set this bit to reset sync-logic inside L2-Cache. Recommend that this should only
 *  be used to initialize sync-logic when some fatal error of sync-logic occurs.
 */
#define CACHE_L2_CACHE_SYNC_RST    (BIT(5))
#define CACHE_L2_CACHE_SYNC_RST_M  (CACHE_L2_CACHE_SYNC_RST_V << CACHE_L2_CACHE_SYNC_RST_S)
#define CACHE_L2_CACHE_SYNC_RST_V  0x00000001U
#define CACHE_L2_CACHE_SYNC_RST_S  5

/** CACHE_L2_CACHE_PRELOAD_RST_CTRL_REG register
 *  Cache Preload Reset control register
 */
#define CACHE_L2_CACHE_PRELOAD_RST_CTRL_REG (DR_REG_CACHE_BASE + 0x3b0)
/** CACHE_L2_CACHE_PLD_RST : R/W; bitpos: [5]; default: 0;
 *  set this bit to reset preload-logic inside L2-Cache. Recommend that this should
 *  only be used to initialize preload-logic when some fatal error of preload-logic
 *  occurs.
 */
#define CACHE_L2_CACHE_PLD_RST    (BIT(5))
#define CACHE_L2_CACHE_PLD_RST_M  (CACHE_L2_CACHE_PLD_RST_V << CACHE_L2_CACHE_PLD_RST_S)
#define CACHE_L2_CACHE_PLD_RST_V  0x00000001U
#define CACHE_L2_CACHE_PLD_RST_S  5

/** CACHE_L2_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG register
 *  Cache Autoload buffer clear control register
 */
#define CACHE_L2_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG (DR_REG_CACHE_BASE + 0x3b4)
/** CACHE_L2_CACHE_ALD_BUF_CLR : R/W; bitpos: [5]; default: 0;
 *  set this bit to clear autoload-buffer inside L2-Cache. If this bit is active,
 *  autoload will not work in L2-Cache. This bit should not be active when autoload
 *  works in L2-Cache.
 */
#define CACHE_L2_CACHE_ALD_BUF_CLR    (BIT(5))
#define CACHE_L2_CACHE_ALD_BUF_CLR_M  (CACHE_L2_CACHE_ALD_BUF_CLR_V << CACHE_L2_CACHE_ALD_BUF_CLR_S)
#define CACHE_L2_CACHE_ALD_BUF_CLR_V  0x00000001U
#define CACHE_L2_CACHE_ALD_BUF_CLR_S  5

/** CACHE_L2_UNALLOCATE_BUFFER_CLEAR_REG register
 *  Unallocate request buffer clear registers
 */
#define CACHE_L2_UNALLOCATE_BUFFER_CLEAR_REG (DR_REG_CACHE_BASE + 0x3b8)
/** CACHE_L2_CACHE_UNALLOC_CLR : R/W; bitpos: [5]; default: 0;
 *  The bit is used to clear the unallocate request buffer of l2 icache where the
 *  unallocate request is responded but not completed.
 */
#define CACHE_L2_CACHE_UNALLOC_CLR    (BIT(5))
#define CACHE_L2_CACHE_UNALLOC_CLR_M  (CACHE_L2_CACHE_UNALLOC_CLR_V << CACHE_L2_CACHE_UNALLOC_CLR_S)
#define CACHE_L2_CACHE_UNALLOC_CLR_V  0x00000001U
#define CACHE_L2_CACHE_UNALLOC_CLR_S  5

/** CACHE_L2_CACHE_ACCESS_ATTR_CTRL_REG register
 *  L2 cache access attribute control register
 */
#define CACHE_L2_CACHE_ACCESS_ATTR_CTRL_REG (DR_REG_CACHE_BASE + 0x3bc)
/** CACHE_L2_CACHE_ACCESS_FORCE_CC : R/W; bitpos: [0]; default: 1;
 *  Set this bit to force the request to l2 cache with cacheable attribute, otherwise,
 *  the attribute is propagated from L1 cache or CPU, it could be one of cacheable and
 *  non-cacheable.
 */
#define CACHE_L2_CACHE_ACCESS_FORCE_CC    (BIT(0))
#define CACHE_L2_CACHE_ACCESS_FORCE_CC_M  (CACHE_L2_CACHE_ACCESS_FORCE_CC_V << CACHE_L2_CACHE_ACCESS_FORCE_CC_S)
#define CACHE_L2_CACHE_ACCESS_FORCE_CC_V  0x00000001U
#define CACHE_L2_CACHE_ACCESS_FORCE_CC_S  0
/** CACHE_L2_CACHE_ACCESS_FORCE_WB : R/W; bitpos: [1]; default: 1;
 *  Set this bit to force the request to l2 cache with write-back attribute, otherwise,
 *  the attribute is propagated from L1 cache or CPU, it could be one of write-back and
 *  write-through.
 */
#define CACHE_L2_CACHE_ACCESS_FORCE_WB    (BIT(1))
#define CACHE_L2_CACHE_ACCESS_FORCE_WB_M  (CACHE_L2_CACHE_ACCESS_FORCE_WB_V << CACHE_L2_CACHE_ACCESS_FORCE_WB_S)
#define CACHE_L2_CACHE_ACCESS_FORCE_WB_V  0x00000001U
#define CACHE_L2_CACHE_ACCESS_FORCE_WB_S  1
/** CACHE_L2_CACHE_ACCESS_FORCE_WMA : R/W; bitpos: [2]; default: 1;
 *  Set this bit to force the request to l2 cache with write-miss-allocate attribute,
 *  otherwise, the attribute is propagated from L1 cache or CPU, it could be one of
 *  write-miss-allocate and write-miss-no-allocate.
 */
#define CACHE_L2_CACHE_ACCESS_FORCE_WMA    (BIT(2))
#define CACHE_L2_CACHE_ACCESS_FORCE_WMA_M  (CACHE_L2_CACHE_ACCESS_FORCE_WMA_V << CACHE_L2_CACHE_ACCESS_FORCE_WMA_S)
#define CACHE_L2_CACHE_ACCESS_FORCE_WMA_V  0x00000001U
#define CACHE_L2_CACHE_ACCESS_FORCE_WMA_S  2
/** CACHE_L2_CACHE_ACCESS_FORCE_RMA : R/W; bitpos: [3]; default: 1;
 *  Set this bit to force the request to l2 cache with read-miss-allocate attribute,
 *  otherwise, the attribute is propagated from L1 cache or CPU, it could be one of
 *  read-miss-allocate and read-miss-no-allocate.
 */
#define CACHE_L2_CACHE_ACCESS_FORCE_RMA    (BIT(3))
#define CACHE_L2_CACHE_ACCESS_FORCE_RMA_M  (CACHE_L2_CACHE_ACCESS_FORCE_RMA_V << CACHE_L2_CACHE_ACCESS_FORCE_RMA_S)
#define CACHE_L2_CACHE_ACCESS_FORCE_RMA_V  0x00000001U
#define CACHE_L2_CACHE_ACCESS_FORCE_RMA_S  3

/** CACHE_L2_CACHE_OBJECT_CTRL_REG register
 *  Cache Tag and Data memory Object control register
 */
#define CACHE_L2_CACHE_OBJECT_CTRL_REG (DR_REG_CACHE_BASE + 0x3c0)
/** CACHE_L2_CACHE_TAG_OBJECT : R/W; bitpos: [5]; default: 0;
 *  Set this bit to set L2-Cache tag memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L2_CACHE_TAG_OBJECT    (BIT(5))
#define CACHE_L2_CACHE_TAG_OBJECT_M  (CACHE_L2_CACHE_TAG_OBJECT_V << CACHE_L2_CACHE_TAG_OBJECT_S)
#define CACHE_L2_CACHE_TAG_OBJECT_V  0x00000001U
#define CACHE_L2_CACHE_TAG_OBJECT_S  5
/** CACHE_L2_CACHE_MEM_OBJECT : R/W; bitpos: [11]; default: 0;
 *  Set this bit to set L2-Cache data memory as object. This bit should be onehot with
 *  the others fields inside this register.
 */
#define CACHE_L2_CACHE_MEM_OBJECT    (BIT(11))
#define CACHE_L2_CACHE_MEM_OBJECT_M  (CACHE_L2_CACHE_MEM_OBJECT_V << CACHE_L2_CACHE_MEM_OBJECT_S)
#define CACHE_L2_CACHE_MEM_OBJECT_V  0x00000001U
#define CACHE_L2_CACHE_MEM_OBJECT_S  11

/** CACHE_L2_CACHE_WAY_OBJECT_REG register
 *  Cache Tag and Data memory way register
 */
#define CACHE_L2_CACHE_WAY_OBJECT_REG (DR_REG_CACHE_BASE + 0x3c4)
/** CACHE_L2_CACHE_WAY_OBJECT : R/W; bitpos: [2:0]; default: 0;
 *  Set this bits to select which way of the tag-object will be accessed. 0: way0, 1:
 *  way1, 2: way2, 3: way3, ?, 7: way7.
 */
#define CACHE_L2_CACHE_WAY_OBJECT    0x00000007U
#define CACHE_L2_CACHE_WAY_OBJECT_M  (CACHE_L2_CACHE_WAY_OBJECT_V << CACHE_L2_CACHE_WAY_OBJECT_S)
#define CACHE_L2_CACHE_WAY_OBJECT_V  0x00000007U
#define CACHE_L2_CACHE_WAY_OBJECT_S  0

/** CACHE_L2_CACHE_VADDR_REG register
 *  Cache Vaddr register
 */
#define CACHE_L2_CACHE_VADDR_REG (DR_REG_CACHE_BASE + 0x3c8)
/** CACHE_L2_CACHE_VADDR : R/W; bitpos: [31:0]; default: 1073741824;
 *  Those bits stores the virtual address which will decide where inside the specified
 *  tag memory object will be accessed.
 */
#define CACHE_L2_CACHE_VADDR    0xFFFFFFFFU
#define CACHE_L2_CACHE_VADDR_M  (CACHE_L2_CACHE_VADDR_V << CACHE_L2_CACHE_VADDR_S)
#define CACHE_L2_CACHE_VADDR_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_VADDR_S  0

/** CACHE_L2_CACHE_DEBUG_BUS_REG register
 *  Cache Tag/data memory content register
 */
#define CACHE_L2_CACHE_DEBUG_BUS_REG (DR_REG_CACHE_BASE + 0x3cc)
/** CACHE_L2_CACHE_DEBUG_BUS : R/W; bitpos: [31:0]; default: 972;
 *  This is a constant place where we can write data to or read data from the tag/data
 *  memory on the specified cache.
 */
#define CACHE_L2_CACHE_DEBUG_BUS    0xFFFFFFFFU
#define CACHE_L2_CACHE_DEBUG_BUS_M  (CACHE_L2_CACHE_DEBUG_BUS_V << CACHE_L2_CACHE_DEBUG_BUS_S)
#define CACHE_L2_CACHE_DEBUG_BUS_V  0xFFFFFFFFU
#define CACHE_L2_CACHE_DEBUG_BUS_S  0

/** CACHE_CLOCK_GATE_REG register
 *  Clock gate control register
 */
#define CACHE_CLOCK_GATE_REG (DR_REG_CACHE_BASE + 0x3d4)
/** CACHE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  The bit is used to enable clock gate when access all registers in this module.
 */
#define CACHE_CLK_EN    (BIT(0))
#define CACHE_CLK_EN_M  (CACHE_CLK_EN_V << CACHE_CLK_EN_S)
#define CACHE_CLK_EN_V  0x00000001U
#define CACHE_CLK_EN_S  0

/** CACHE_DATE_REG register
 *  Version control register
 */
#define CACHE_DATE_REG (DR_REG_CACHE_BASE + 0x3fc)
/** CACHE_DATE : R/W; bitpos: [27:0]; default: 36716848;
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
