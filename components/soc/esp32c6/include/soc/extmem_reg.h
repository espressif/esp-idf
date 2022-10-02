/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

// TODO: IDF-5797

#define EXTMEM_ICACHE_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x0)

#define EXTMEM_DCACHE_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x4)
/* EXTMEM_DCACHE_SHUT_DBUS1 : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: The bit is used to disable core1 dbus access L1-DCache, 0: enable, 1: disable.*/
#define EXTMEM_DCACHE_SHUT_DBUS1    (BIT(1))
#define EXTMEM_DCACHE_SHUT_DBUS1_M  (BIT(1))
#define EXTMEM_DCACHE_SHUT_DBUS1_V  0x1
#define EXTMEM_DCACHE_SHUT_DBUS1_S  1
/* EXTMEM_DCACHE_SHUT_DBUS0 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to disable core0 dbus access L1-DCache, 0: enable, 1: disable.*/
#define EXTMEM_DCACHE_SHUT_DBUS0    (BIT(0))
#define EXTMEM_DCACHE_SHUT_DBUS0_M  (BIT(0))
#define EXTMEM_DCACHE_SHUT_DBUS0_V  0x1
#define EXTMEM_DCACHE_SHUT_DBUS0_S  0

#define EXTMEM_CACHE_WRAP_AROUND_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x20)
/* EXTMEM_DCACHE_WRAP : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: Set this bit as 1 to enable L1-DCache wrap around mode..*/
#define EXTMEM_CACHE_FLASH_WRAP_AROUND    (BIT(4))
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_M  (BIT(4))
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_V  0x1
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_S  4

#define EXTMEM_CACHE_TAG_MEM_POWER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x24)
/* EXTMEM_DCACHE_TAG_MEM_FORCE_PU : R/W ;bitpos:[18] ;default: 1'h1 ; */
/*description: The bit is used to power L1-DCache tag memory up. 0: follow rtc_lslp, 1: power u
p.*/
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU    (BIT(18))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU_M  (BIT(18))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU_S  18
/* EXTMEM_DCACHE_TAG_MEM_FORCE_PD : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: The bit is used to power L1-DCache tag memory down. 0: follow rtc_lslp, 1: power
 down.*/
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD    (BIT(17))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD_M  (BIT(17))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD_S  17
/* EXTMEM_DCACHE_TAG_MEM_FORCE_ON : R/W ;bitpos:[16] ;default: 1'h1 ; */
/*description: The bit is used to close clock gating of  L1-DCache tag memory. 1: close gating,
 0: open clock gating..*/
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON    (BIT(16))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON_M  (BIT(16))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON_S  16

#define EXTMEM_CACHE_DATA_MEM_POWER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x28)
/* EXTMEM_DCACHE_DATA_MEM_FORCE_PU : R/W ;bitpos:[18] ;default: 1'h1 ; */
/*description: The bit is used to power L1-DCache data memory up. 0: follow rtc_lslp, 1: power
up.*/
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PU    (BIT(18))
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PU_M  (BIT(18))
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PU_V  0x1
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PU_S  18
/* EXTMEM_DCACHE_DATA_MEM_FORCE_PD : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: The bit is used to power L1-DCache data memory down. 0: follow rtc_lslp, 1: powe
r down.*/
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PD    (BIT(17))
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PD_M  (BIT(17))
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PD_V  0x1
#define EXTMEM_DCACHE_DATA_MEM_FORCE_PD_S  17
/* EXTMEM_DCACHE_DATA_MEM_FORCE_ON : R/W ;bitpos:[16] ;default: 1'h1 ; */
/*description: The bit is used to close clock gating of  L1-DCache data memory. 1: close gating
, 0: open clock gating..*/
#define EXTMEM_DCACHE_DATA_MEM_FORCE_ON    (BIT(16))
#define EXTMEM_DCACHE_DATA_MEM_FORCE_ON_M  (BIT(16))
#define EXTMEM_DCACHE_DATA_MEM_FORCE_ON_V  0x1
#define EXTMEM_DCACHE_DATA_MEM_FORCE_ON_S  16

#define EXTMEM_CACHE_FREEZE_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x2C)
/* EXTMEM_DCACHE_FREEZE_DONE : RO ;bitpos:[18] ;default: 1'h0 ; */
/*description: The bit is used to indicate whether freeze operation on L1-DCache is finished or
 not. 0: not finished. 1: finished..*/
#define EXTMEM_DCACHE_FREEZE_DONE    (BIT(18))
#define EXTMEM_DCACHE_FREEZE_DONE_M  (BIT(18))
#define EXTMEM_DCACHE_FREEZE_DONE_V  0x1
#define EXTMEM_DCACHE_FREEZE_DONE_S  18
/* EXTMEM_DCACHE_FREEZE_MODE : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: The bit is used to configure mode of freeze operation L1-DCache. 0: a miss-acces
s will not stuck. 1: a miss-access will stuck..*/
#define EXTMEM_DCACHE_FREEZE_MODE    (BIT(17))
#define EXTMEM_DCACHE_FREEZE_MODE_M  (BIT(17))
#define EXTMEM_DCACHE_FREEZE_MODE_V  0x1
#define EXTMEM_DCACHE_FREEZE_MODE_S  17
/* EXTMEM_DCACHE_FREEZE_EN : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: The bit is used to enable freeze operation on L1-DCache. It can be cleared by so
ftware..*/
#define EXTMEM_DCACHE_FREEZE_EN    (BIT(16))
#define EXTMEM_DCACHE_FREEZE_EN_M  (BIT(16))
#define EXTMEM_DCACHE_FREEZE_EN_V  0x1
#define EXTMEM_DCACHE_FREEZE_EN_S  16

#define EXTMEM_CACHE_DATA_MEM_ACS_CONF_REG          (DR_REG_EXTMEM_BASE + 0x30)
/* EXTMEM_DCACHE_DATA_MEM_WR_EN : R/W ;bitpos:[17] ;default: 1'h1 ; */
/*description: The bit is used to enable config-bus write L1-DCache data memoryory. 0: disable,
 1: enable..*/
#define EXTMEM_DCACHE_DATA_MEM_WR_EN    (BIT(17))
#define EXTMEM_DCACHE_DATA_MEM_WR_EN_M  (BIT(17))
#define EXTMEM_DCACHE_DATA_MEM_WR_EN_V  0x1
#define EXTMEM_DCACHE_DATA_MEM_WR_EN_S  17
/* EXTMEM_DCACHE_DATA_MEM_RD_EN : R/W ;bitpos:[16] ;default: 1'h1 ; */
/*description: The bit is used to enable config-bus read L1-DCache data memoryory. 0: disable,
1: enable..*/
#define EXTMEM_DCACHE_DATA_MEM_RD_EN    (BIT(16))
#define EXTMEM_DCACHE_DATA_MEM_RD_EN_M  (BIT(16))
#define EXTMEM_DCACHE_DATA_MEM_RD_EN_V  0x1
#define EXTMEM_DCACHE_DATA_MEM_RD_EN_S  16

#define EXTMEM_CACHE_TAG_MEM_ACS_CONF_REG          (DR_REG_EXTMEM_BASE + 0x34)
/* EXTMEM_DCACHE_TAG_MEM_WR_EN : R/W ;bitpos:[17] ;default: 1'h1 ; */
/*description: The bit is used to enable config-bus write L1-DCache tag memoryory. 0: disable,
1: enable..*/
#define EXTMEM_DCACHE_TAG_MEM_WR_EN    (BIT(17))
#define EXTMEM_DCACHE_TAG_MEM_WR_EN_M  (BIT(17))
#define EXTMEM_DCACHE_TAG_MEM_WR_EN_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_WR_EN_S  17
/* EXTMEM_DCACHE_TAG_MEM_RD_EN : R/W ;bitpos:[16] ;default: 1'h1 ; */
/*description: The bit is used to enable config-bus read L1-DCache tag memoryory. 0: disable, 1
: enable..*/
#define EXTMEM_DCACHE_TAG_MEM_RD_EN    (BIT(16))
#define EXTMEM_DCACHE_TAG_MEM_RD_EN_M  (BIT(16))
#define EXTMEM_DCACHE_TAG_MEM_RD_EN_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_RD_EN_S  16

#define EXTMEM_DCACHE_PRELOCK_CONF_REG          (DR_REG_EXTMEM_BASE + 0x78)
/* EXTMEM_DCACHE_PRELOCK_RGID : HRO ;bitpos:[5:2] ;default: 4'h0 ; */
/*description: The bit is used to set  the gid of l1 dcache prelock..*/
#define EXTMEM_DCACHE_PRELOCK_RGID    0x0000000F
#define EXTMEM_DCACHE_PRELOCK_RGID_M  ((EXTMEM_DCACHE_PRELOCK_RGID_V)<<(EXTMEM_DCACHE_PRELOCK_RGID_S))
#define EXTMEM_DCACHE_PRELOCK_RGID_V  0xF
#define EXTMEM_DCACHE_PRELOCK_RGID_S  2
/* EXTMEM_DCACHE_PRELOCK_SCT1_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: The bit is used to enable the second section of prelock function on L1-DCache..*/
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN    (BIT(1))
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN_M  (BIT(1))
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN_V  0x1
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN_S  1
/* EXTMEM_DCACHE_PRELOCK_SCT0_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to enable the first section of prelock function on L1-DCache..*/
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN    (BIT(0))
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN_M  (BIT(0))
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN_V  0x1
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN_S  0

#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x7C)
/* EXTMEM_DCACHE_PRELOCK_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Those bits are used to configure the start virtual address of the first section
of prelock on L1-DCache, which should be used together with L1_DCACHE_PRELOCK_SC
T0_SIZE_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_M  ((EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_V)<<(EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_S))
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_S  0

#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x80)
/* EXTMEM_DCACHE_PRELOCK_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Those bits are used to configure the start virtual address of the second section
 of prelock on L1-DCache, which should be used together with L1_DCACHE_PRELOCK_S
CT1_SIZE_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_M  ((EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_V)<<(EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_S))
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_S  0

#define EXTMEM_DCACHE_PRELOCK_SCT_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x84)
/* EXTMEM_DCACHE_PRELOCK_SCT1_SIZE : R/W ;bitpos:[29:16] ;default: 14'h3fff ; */
/*description: Those bits are used to configure the size of the second section of prelock on L1
-DCache, which should be used together with L1_DCACHE_PRELOCK_SCT1_ADDR_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE    0x00003FFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_M  ((EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_V)<<(EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_S))
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_V  0x3FFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_S  16
/* EXTMEM_DCACHE_PRELOCK_SCT0_SIZE : R/W ;bitpos:[13:0] ;default: 14'h3fff ; */
/*description: Those bits are used to configure the size of the first section of prelock on L1-
DCache, which should be used together with L1_DCACHE_PRELOCK_SCT0_ADDR_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE    0x00003FFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_M  ((EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_V)<<(EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_S))
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_V  0x3FFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_S  0

#define EXTMEM_CACHE_LOCK_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x88)
/* EXTMEM_CACHE_LOCK_RGID : HRO ;bitpos:[6:3] ;default: 4'h0 ; */
/*description: The bit is used to set  the gid of cache lock/unlock..*/
#define EXTMEM_CACHE_LOCK_RGID    0x0000000F
#define EXTMEM_CACHE_LOCK_RGID_M  ((EXTMEM_CACHE_LOCK_RGID_V)<<(EXTMEM_CACHE_LOCK_RGID_S))
#define EXTMEM_CACHE_LOCK_RGID_V  0xF
#define EXTMEM_CACHE_LOCK_RGID_S  3
/* EXTMEM_CACHE_LOCK_DONE : RO ;bitpos:[2] ;default: 1'h1 ; */
/*description: The bit is used to indicate whether unlock/lock operation is finished or not. 0:
 not finished. 1: finished..*/
#define EXTMEM_CACHE_LOCK_DONE    (BIT(2))
#define EXTMEM_CACHE_LOCK_DONE_M  (BIT(2))
#define EXTMEM_CACHE_LOCK_DONE_V  0x1
#define EXTMEM_CACHE_LOCK_DONE_S  2
/* EXTMEM_CACHE_UNLOCK_ENA : R/W/SC ;bitpos:[1] ;default: 1'h0 ; */
/*description: The bit is used to enable unlock operation. It will be cleared by hardware after
 unlock operation done. Note that (1) this bit and lock_ena bit are mutually exc
lusive, that is, those bits can not be set to 1 at the same time. (2) unlock ope
ration can be applied on L1-ICache, L1-DCache and L2-Cache..*/
#define EXTMEM_CACHE_UNLOCK_ENA    (BIT(1))
#define EXTMEM_CACHE_UNLOCK_ENA_M  (BIT(1))
#define EXTMEM_CACHE_UNLOCK_ENA_V  0x1
#define EXTMEM_CACHE_UNLOCK_ENA_S  1
/* EXTMEM_CACHE_LOCK_ENA : R/W/SC ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to enable lock operation. It will be cleared by hardware after l
ock operation done. Note that (1) this bit and unlock_ena bit are mutually exclu
sive, that is, those bits can not be set to 1 at the same time. (2) lock operati
on can be applied on LL1-ICache, L1-DCache and L2-Cache..*/
#define EXTMEM_CACHE_LOCK_ENA    (BIT(0))
#define EXTMEM_CACHE_LOCK_ENA_M  (BIT(0))
#define EXTMEM_CACHE_LOCK_ENA_V  0x1
#define EXTMEM_CACHE_LOCK_ENA_S  0

#define EXTMEM_CACHE_LOCK_MAP_REG          (DR_REG_EXTMEM_BASE + 0x8C)
/* EXTMEM_CACHE_LOCK_MAP : R/W ;bitpos:[5:0] ;default: 6'h0 ; */
/*description: Those bits are used to indicate which caches in the two-level cache structure wi
ll apply this lock/unlock operation. [0]: L1-ICache0, [1]: L1-ICache1, [2]: L1-I
Cache2, [3]: L1-ICache3, [4]: L1-DCache, [5]: L2-Cache..*/
#define EXTMEM_CACHE_LOCK_MAP    0x0000003F
#define EXTMEM_CACHE_LOCK_MAP_M  ((EXTMEM_CACHE_LOCK_MAP_V)<<(EXTMEM_CACHE_LOCK_MAP_S))
#define EXTMEM_CACHE_LOCK_MAP_V  0x3F
#define EXTMEM_CACHE_LOCK_MAP_S  0

#define EXTMEM_CACHE_LOCK_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x90)
/* EXTMEM_CACHE_LOCK_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Those bits are used to configure the start virtual address of the lock/unlock op
eration, which should be used together with CACHE_LOCK_SIZE_REG.*/
#define EXTMEM_CACHE_LOCK_ADDR    0xFFFFFFFF
#define EXTMEM_CACHE_LOCK_ADDR_M  ((EXTMEM_CACHE_LOCK_ADDR_V)<<(EXTMEM_CACHE_LOCK_ADDR_S))
#define EXTMEM_CACHE_LOCK_ADDR_V  0xFFFFFFFF
#define EXTMEM_CACHE_LOCK_ADDR_S  0

#define EXTMEM_CACHE_LOCK_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x94)
/* EXTMEM_CACHE_LOCK_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: Those bits are used to configure the size of the lock/unlock operation, which sh
ould be used together with CACHE_LOCK_ADDR_REG.*/
#define EXTMEM_CACHE_LOCK_SIZE    0x0000FFFF
#define EXTMEM_CACHE_LOCK_SIZE_M  ((EXTMEM_CACHE_LOCK_SIZE_V)<<(EXTMEM_CACHE_LOCK_SIZE_S))
#define EXTMEM_CACHE_LOCK_SIZE_V  0xFFFF
#define EXTMEM_CACHE_LOCK_SIZE_S  0

#define EXTMEM_CACHE_SYNC_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x98)
/* EXTMEM_CACHE_SYNC_RGID : HRO ;bitpos:[8:5] ;default: 4'h0 ; */
/*description: The bit is used to set  the gid of cache sync operation (invalidate, clean, writ
eback, writeback_invalidate).*/
#define EXTMEM_CACHE_SYNC_RGID    0x0000000F
#define EXTMEM_CACHE_SYNC_RGID_M  ((EXTMEM_CACHE_SYNC_RGID_V)<<(EXTMEM_CACHE_SYNC_RGID_S))
#define EXTMEM_CACHE_SYNC_RGID_V  0xF
#define EXTMEM_CACHE_SYNC_RGID_S  5
/* EXTMEM_CACHE_SYNC_DONE : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: The bit is used to indicate whether sync operation (invalidate, clean, writeback
, writeback_invalidate) is finished or not. 0: not finished. 1: finished..*/
#define EXTMEM_CACHE_SYNC_DONE    (BIT(4))
#define EXTMEM_CACHE_SYNC_DONE_M  (BIT(4))
#define EXTMEM_CACHE_SYNC_DONE_V  0x1
#define EXTMEM_CACHE_SYNC_DONE_S  4
/* EXTMEM_CACHE_WRITEBACK_INVALIDATE_ENA : R/W/SC ;bitpos:[3] ;default: 1'h0 ; */
/*description: The bit is used to enable writeback-invalidate operation. It will be cleared by
hardware after writeback-invalidate operation done. Note that this bit and the o
ther sync-bits (invalidate_ena, clean_ena, writeback_ena) are mutually exclusive
, that is, those bits can not be set to 1 at the same time..*/
#define EXTMEM_CACHE_WRITEBACK_INVALIDATE_ENA    (BIT(3))
#define EXTMEM_CACHE_WRITEBACK_INVALIDATE_ENA_M  (BIT(3))
#define EXTMEM_CACHE_WRITEBACK_INVALIDATE_ENA_V  0x1
#define EXTMEM_CACHE_WRITEBACK_INVALIDATE_ENA_S  3
/* EXTMEM_CACHE_WRITEBACK_ENA : R/W/SC ;bitpos:[2] ;default: 1'h0 ; */
/*description: The bit is used to enable writeback operation. It will be cleared by hardware af
ter writeback operation done. Note that this bit and the other sync-bits (invali
date_ena, clean_ena, writeback_invalidate_ena) are mutually exclusive, that is,
those bits can not be set to 1 at the same time..*/
#define EXTMEM_CACHE_WRITEBACK_ENA    (BIT(2))
#define EXTMEM_CACHE_WRITEBACK_ENA_M  (BIT(2))
#define EXTMEM_CACHE_WRITEBACK_ENA_V  0x1
#define EXTMEM_CACHE_WRITEBACK_ENA_S  2
/* EXTMEM_CACHE_CLEAN_ENA : R/W/SC ;bitpos:[1] ;default: 1'h0 ; */
/*description: The bit is used to enable clean operation. It will be cleared by hardware after
clean operation done. Note that this bit and the other sync-bits (invalidate_ena
, writeback_ena, writeback_invalidate_ena) are mutually exclusive, that is, thos
e bits can not be set to 1 at the same time..*/
#define EXTMEM_CACHE_CLEAN_ENA    (BIT(1))
#define EXTMEM_CACHE_CLEAN_ENA_M  (BIT(1))
#define EXTMEM_CACHE_CLEAN_ENA_V  0x1
#define EXTMEM_CACHE_CLEAN_ENA_S  1
/* EXTMEM_CACHE_INVALIDATE_ENA : R/W/SC ;bitpos:[0] ;default: 1'h1 ; */
/*description: The bit is used to enable invalidate operation. It will be cleared by hardware a
fter invalidate operation done. Note that this bit and the other sync-bits (clea
n_ena, writeback_ena, writeback_invalidate_ena) are mutually exclusive, that is,
 those bits can not be set to 1 at the same time..*/
#define EXTMEM_CACHE_INVALIDATE_ENA    (BIT(0))
#define EXTMEM_CACHE_INVALIDATE_ENA_M  (BIT(0))
#define EXTMEM_CACHE_INVALIDATE_ENA_V  0x1
#define EXTMEM_CACHE_INVALIDATE_ENA_S  0

#define EXTMEM_CACHE_SYNC_MAP_REG          (DR_REG_EXTMEM_BASE + 0x9C)
/* EXTMEM_CACHE_SYNC_MAP : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: Those bits are used to indicate which caches in the two-level cache structure wi
ll apply the sync operation. [0]: L1-ICache0, [1]: L1-ICache1, [2]: L1-ICache2,
[3]: L1-ICache3, [4]: L1-DCache, [5]: L2-Cache..*/
#define EXTMEM_CACHE_SYNC_MAP    0x0000003F
#define EXTMEM_CACHE_SYNC_MAP_M  ((EXTMEM_CACHE_SYNC_MAP_V)<<(EXTMEM_CACHE_SYNC_MAP_S))
#define EXTMEM_CACHE_SYNC_MAP_V  0x3F
#define EXTMEM_CACHE_SYNC_MAP_S  0

#define EXTMEM_CACHE_SYNC_ADDR_REG          (DR_REG_EXTMEM_BASE + 0xA0)
/* EXTMEM_CACHE_SYNC_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Those bits are used to configure the start virtual address of the sync operation
, which should be used together with CACHE_SYNC_SIZE_REG.*/
#define EXTMEM_CACHE_SYNC_ADDR    0xFFFFFFFF
#define EXTMEM_CACHE_SYNC_ADDR_M  ((EXTMEM_CACHE_SYNC_ADDR_V)<<(EXTMEM_CACHE_SYNC_ADDR_S))
#define EXTMEM_CACHE_SYNC_ADDR_V  0xFFFFFFFF
#define EXTMEM_CACHE_SYNC_ADDR_S  0

#define EXTMEM_CACHE_SYNC_SIZE_REG          (DR_REG_EXTMEM_BASE + 0xA4)
/* EXTMEM_CACHE_SYNC_SIZE : R/W ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Those bits are used to configure the size of the sync operation, which should be
 used together with CACHE_SYNC_ADDR_REG.*/
#define EXTMEM_CACHE_SYNC_SIZE    0x00FFFFFF
#define EXTMEM_CACHE_SYNC_SIZE_M  ((EXTMEM_CACHE_SYNC_SIZE_V)<<(EXTMEM_CACHE_SYNC_SIZE_S))
#define EXTMEM_CACHE_SYNC_SIZE_V  0xFFFFFF
#define EXTMEM_CACHE_SYNC_SIZE_S  0

#define EXTMEM_DCACHE_PRELOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0xD8)
/* EXTMEM_DCACHE_PRELOAD_RGID : HRO ;bitpos:[6:3] ;default: 4'h0 ; */
/*description: The bit is used to set  the gid of l1 dcache preload..*/
#define EXTMEM_DCACHE_PRELOAD_RGID    0x0000000F
#define EXTMEM_DCACHE_PRELOAD_RGID_M  ((EXTMEM_DCACHE_PRELOAD_RGID_V)<<(EXTMEM_DCACHE_PRELOAD_RGID_S))
#define EXTMEM_DCACHE_PRELOAD_RGID_V  0xF
#define EXTMEM_DCACHE_PRELOAD_RGID_S  3
/* EXTMEM_DCACHE_PRELOAD_ORDER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: The bit is used to configure the direction of preload operation. 0: ascending, 1
: descending..*/
#define EXTMEM_DCACHE_PRELOAD_ORDER    (BIT(2))
#define EXTMEM_DCACHE_PRELOAD_ORDER_M  (BIT(2))
#define EXTMEM_DCACHE_PRELOAD_ORDER_V  0x1
#define EXTMEM_DCACHE_PRELOAD_ORDER_S  2
/* EXTMEM_DCACHE_PRELOAD_DONE : RO ;bitpos:[1] ;default: 1'h1 ; */
/*description: The bit is used to indicate whether preload operation is finished or not. 0: not
 finished. 1: finished..*/
#define EXTMEM_DCACHE_PRELOAD_DONE    (BIT(1))
#define EXTMEM_DCACHE_PRELOAD_DONE_M  (BIT(1))
#define EXTMEM_DCACHE_PRELOAD_DONE_V  0x1
#define EXTMEM_DCACHE_PRELOAD_DONE_S  1
/* EXTMEM_DCACHE_PRELOAD_ENA : R/W/SC ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to enable preload operation on L1-DCache. It will be cleared by
hardware automatically after preload operation is done..*/
#define EXTMEM_DCACHE_PRELOAD_ENA    (BIT(0))
#define EXTMEM_DCACHE_PRELOAD_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_PRELOAD_ENA_V  0x1
#define EXTMEM_DCACHE_PRELOAD_ENA_S  0

#define EXTMEM_DCACHE_PRELOAD_ADDR_REG          (DR_REG_EXTMEM_BASE + 0xDC)
/* EXTMEM_DCACHE_PRELOAD_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Those bits are used to configure the start virtual address of preload on L1-DCac
he, which should be used together with L1_DCACHE_PRELOAD_SIZE_REG.*/
#define EXTMEM_DCACHE_PRELOAD_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOAD_ADDR_M  ((EXTMEM_DCACHE_PRELOAD_ADDR_V)<<(EXTMEM_DCACHE_PRELOAD_ADDR_S))
#define EXTMEM_DCACHE_PRELOAD_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOAD_ADDR_S  0

#define EXTMEM_DCACHE_PRELOAD_SIZE_REG          (DR_REG_EXTMEM_BASE + 0xE0)
/* EXTMEM_DCACHE_PRELOAD_SIZE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: Those bits are used to configure the size of the first section of prelock on L1-
DCache, which should be used together with L1_DCACHE_PRELOAD_ADDR_REG.*/
#define EXTMEM_DCACHE_PRELOAD_SIZE    0x00003FFF
#define EXTMEM_DCACHE_PRELOAD_SIZE_M  ((EXTMEM_DCACHE_PRELOAD_SIZE_V)<<(EXTMEM_DCACHE_PRELOAD_SIZE_S))
#define EXTMEM_DCACHE_PRELOAD_SIZE_V  0x3FFF
#define EXTMEM_DCACHE_PRELOAD_SIZE_S  0

#define EXTMEM_DCACHE_AUTOLOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x134)
/* EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: The bit is used to enable the second section for autoload operation on L1-DCache
..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA    (BIT(9))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA_M  (BIT(9))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA_S  9
/* EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: The bit is used to enable the first section for autoload operation on L1-DCache..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA    (BIT(8))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA_M  (BIT(8))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA_S  8
/* EXTMEM_DCACHE_AUTOLOAD_TRIGGER_MODE : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: The field is used to configure trigger mode of autoload operation on L1-DCache.
0/3: miss-trigger, 1: hit-trigger, 2: miss-hit-trigger..*/
#define EXTMEM_DCACHE_AUTOLOAD_TRIGGER_MODE    0x00000003
#define EXTMEM_DCACHE_AUTOLOAD_TRIGGER_MODE_M  ((EXTMEM_DCACHE_AUTOLOAD_TRIGGER_MODE_V)<<(EXTMEM_DCACHE_AUTOLOAD_TRIGGER_MODE_S))
#define EXTMEM_DCACHE_AUTOLOAD_TRIGGER_MODE_V  0x3
#define EXTMEM_DCACHE_AUTOLOAD_TRIGGER_MODE_S  3
/* EXTMEM_DCACHE_AUTOLOAD_ORDER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: The bit is used to configure the direction of autoload operation on L1-DCache. 0
: ascending. 1: descending..*/
#define EXTMEM_DCACHE_AUTOLOAD_ORDER    (BIT(2))
#define EXTMEM_DCACHE_AUTOLOAD_ORDER_M  (BIT(2))
#define EXTMEM_DCACHE_AUTOLOAD_ORDER_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_ORDER_S  2
/* EXTMEM_DCACHE_AUTOLOAD_DONE : RO ;bitpos:[1] ;default: 1'h1 ; */
/*description: The bit is used to indicate whether autoload operation on L1-DCache is finished
or not. 0: not finished. 1: finished..*/
#define EXTMEM_DCACHE_AUTOLOAD_DONE    (BIT(1))
#define EXTMEM_DCACHE_AUTOLOAD_DONE_M  (BIT(1))
#define EXTMEM_DCACHE_AUTOLOAD_DONE_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_DONE_S  1
/* EXTMEM_DCACHE_AUTOLOAD_ENA : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to enable and disable autoload operation on L1-DCache.  1: enabl
e, 0: disable..*/
#define EXTMEM_DCACHE_AUTOLOAD_ENA    (BIT(0))
#define EXTMEM_DCACHE_AUTOLOAD_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_AUTOLOAD_ENA_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_ENA_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x138)
/* EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Those bits are used to configure the start virtual address of the first section
for autoload operation on L1-DCache. Note that it should be used together with L
1_DCACHE_AUTOLOAD_SCT0_SIZE and L1_DCACHE_AUTOLOAD_SCT0_ENA..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x13C)
/* EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE : R/W ;bitpos:[27:0] ;default: 28'h0 ; */
/*description: Those bits are used to configure the size of the first section for autoload oper
ation on L1-DCache. Note that it should be used together with L1_DCACHE_AUTOLOAD
_SCT0_ADDR and L1_DCACHE_AUTOLOAD_SCT0_ENA..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE    0x0FFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_V  0xFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x140)
/* EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Those bits are used to configure the start virtual address of the second section
 for autoload operation on L1-DCache. Note that it should be used together with
L1_DCACHE_AUTOLOAD_SCT1_SIZE and L1_DCACHE_AUTOLOAD_SCT1_ENA..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x144)
/* EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE : R/W ;bitpos:[27:0] ;default: 28'h0 ; */
/*description: Those bits are used to configure the size of the second section for autoload ope
ration on L1-DCache. Note that it should be used together with L1_DCACHE_AUTOLOA
D_SCT1_ADDR and L1_DCACHE_AUTOLOAD_SCT1_ENA..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE    0x0FFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_V  0xFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_S  0

#define EXTMEM_CACHE_ACS_CNT_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0x158)
/* EXTMEM_DBUS1_OVF_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt of one of counters overflow that occurs in L
1-DCache due to bus1 accesses L1-DCache..*/
#define EXTMEM_DBUS1_OVF_INT_ENA    (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_ENA_M  (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_ENA_V  0x1
#define EXTMEM_DBUS1_OVF_INT_ENA_S  5
/* EXTMEM_DBUS0_OVF_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt of one of counters overflow that occurs in L
1-DCache due to bus0 accesses L1-DCache..*/
#define EXTMEM_DBUS0_OVF_INT_ENA    (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_ENA_M  (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_ENA_V  0x1
#define EXTMEM_DBUS0_OVF_INT_ENA_S  4

#define EXTMEM_CACHE_ACS_CNT_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0x15C)
/* EXTMEM_DBUS1_OVF_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to clear counters overflow interrupt and counters in L1-DCache d
ue to bus1 accesses L1-DCache..*/
#define EXTMEM_DBUS1_OVF_INT_CLR    (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_CLR_M  (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_CLR_V  0x1
#define EXTMEM_DBUS1_OVF_INT_CLR_S  5
/* EXTMEM_DBUS0_OVF_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear counters overflow interrupt and counters in L1-DCache d
ue to bus0 accesses L1-DCache..*/
#define EXTMEM_DBUS0_OVF_INT_CLR    (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_CLR_M  (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_CLR_V  0x1
#define EXTMEM_DBUS0_OVF_INT_CLR_S  4

#define EXTMEM_CACHE_ACS_CNT_INT_RAW_REG          (DR_REG_EXTMEM_BASE + 0x160)
/* EXTMEM_DBUS1_OVF_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw bit of the interrupt of one of counters overflow that occurs in L1-DCach
e due to bus1 accesses L1-DCache..*/
#define EXTMEM_DBUS1_OVF_INT_RAW    (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_RAW_M  (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_RAW_V  0x1
#define EXTMEM_DBUS1_OVF_INT_RAW_S  5
/* EXTMEM_DBUS0_OVF_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw bit of the interrupt of one of counters overflow that occurs in L1-DCach
e due to bus0 accesses L1-DCache..*/
#define EXTMEM_DBUS0_OVF_INT_RAW    (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_RAW_M  (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_RAW_V  0x1
#define EXTMEM_DBUS0_OVF_INT_RAW_S  4

#define EXTMEM_CACHE_ACS_CNT_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0x164)
/* EXTMEM_DBUS1_OVF_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit indicates the interrupt status of one of counters overflow that occurs i
n L1-DCache due to bus1 accesses L1-DCache..*/
#define EXTMEM_DBUS1_OVF_INT_ST    (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_ST_M  (BIT(5))
#define EXTMEM_DBUS1_OVF_INT_ST_V  0x1
#define EXTMEM_DBUS1_OVF_INT_ST_S  5
/* EXTMEM_DBUS0_OVF_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit indicates the interrupt status of one of counters overflow that occurs i
n L1-DCache due to bus0 accesses L1-DCache..*/
#define EXTMEM_DBUS0_OVF_INT_ST    (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_ST_M  (BIT(4))
#define EXTMEM_DBUS0_OVF_INT_ST_V  0x1
#define EXTMEM_DBUS0_OVF_INT_ST_S  4

#define EXTMEM_CACHE_ACS_FAIL_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0x168)
/* EXTMEM_DCACHE_FAIL_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt of access fail that occurs in L1-DCache due
to cpu accesses L1-DCache..*/
#define EXTMEM_DCACHE_FAIL_INT_ENA    (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_ENA_M  (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_ENA_V  0x1
#define EXTMEM_DCACHE_FAIL_INT_ENA_S  4

#define EXTMEM_CACHE_ACS_FAIL_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0x16C)
/* EXTMEM_DCACHE_FAIL_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt of access fail that occurs in L1-DCache due t
o cpu accesses L1-DCache..*/
#define EXTMEM_DCACHE_FAIL_INT_CLR    (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_CLR_M  (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_CLR_V  0x1
#define EXTMEM_DCACHE_FAIL_INT_CLR_S  4

#define EXTMEM_CACHE_ACS_FAIL_INT_RAW_REG          (DR_REG_EXTMEM_BASE + 0x170)
/* EXTMEM_DCACHE_FAIL_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw bit of the interrupt of access fail that occurs in L1-DCache..*/
#define EXTMEM_DCACHE_FAIL_INT_RAW    (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_RAW_M  (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_RAW_V  0x1
#define EXTMEM_DCACHE_FAIL_INT_RAW_S  4

#define EXTMEM_CACHE_ACS_FAIL_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0x174)
/* EXTMEM_DCACHE_FAIL_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit indicates the interrupt status of access fail that occurs in L1-DCache d
ue to cpu accesses L1-DCache..*/
#define EXTMEM_DCACHE_FAIL_INT_ST    (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_ST_M  (BIT(4))
#define EXTMEM_DCACHE_FAIL_INT_ST_V  0x1
#define EXTMEM_DCACHE_FAIL_INT_ST_S  4

#define EXTMEM_CACHE_ACS_CNT_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x178)
/* EXTMEM_DBUS1_CNT_CLR : WT ;bitpos:[21] ;default: 1'b0 ; */
/*description: The bit is used to clear dbus1 counter in L1-DCache..*/
#define EXTMEM_DBUS1_CNT_CLR    (BIT(21))
#define EXTMEM_DBUS1_CNT_CLR_M  (BIT(21))
#define EXTMEM_DBUS1_CNT_CLR_V  0x1
#define EXTMEM_DBUS1_CNT_CLR_S  21
/* EXTMEM_DBUS0_CNT_CLR : WT ;bitpos:[20] ;default: 1'b0 ; */
/*description: The bit is used to clear dbus0 counter in L1-DCache..*/
#define EXTMEM_DBUS0_CNT_CLR    (BIT(20))
#define EXTMEM_DBUS0_CNT_CLR_M  (BIT(20))
#define EXTMEM_DBUS0_CNT_CLR_V  0x1
#define EXTMEM_DBUS0_CNT_CLR_S  20
/* EXTMEM_DBUS1_CNT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to enable dbus1 counter in L1-DCache..*/
#define EXTMEM_DBUS1_CNT_ENA    (BIT(5))
#define EXTMEM_DBUS1_CNT_ENA_M  (BIT(5))
#define EXTMEM_DBUS1_CNT_ENA_V  0x1
#define EXTMEM_DBUS1_CNT_ENA_S  5
/* EXTMEM_DBUS0_CNT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable dbus0 counter in L1-DCache..*/
#define EXTMEM_DBUS0_CNT_ENA    (BIT(4))
#define EXTMEM_DBUS0_CNT_ENA_M  (BIT(4))
#define EXTMEM_DBUS0_CNT_ENA_V  0x1
#define EXTMEM_DBUS0_CNT_ENA_S  4

#define EXTMEM_DBUS0_ACS_HIT_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1BC)
/* EXTMEM_DBUS0_HIT_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of hits when bus0 accesses L1-DCache..*/
#define EXTMEM_DBUS0_HIT_CNT    0xFFFFFFFF
#define EXTMEM_DBUS0_HIT_CNT_M  ((EXTMEM_DBUS0_HIT_CNT_V)<<(EXTMEM_DBUS0_HIT_CNT_S))
#define EXTMEM_DBUS0_HIT_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS0_HIT_CNT_S  0

#define EXTMEM_DBUS0_ACS_MISS_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1C0)
/* EXTMEM_DBUS0_MISS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of missing when bus0 accesses L1-DCache..*/
#define EXTMEM_DBUS0_MISS_CNT    0xFFFFFFFF
#define EXTMEM_DBUS0_MISS_CNT_M  ((EXTMEM_DBUS0_MISS_CNT_V)<<(EXTMEM_DBUS0_MISS_CNT_S))
#define EXTMEM_DBUS0_MISS_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS0_MISS_CNT_S  0

#define EXTMEM_DBUS0_ACS_CONFLICT_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1C4)
/* EXTMEM_DBUS0_CONFLICT_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of access-conflicts when bus0 accesses L1-DCache
..*/
#define EXTMEM_DBUS0_CONFLICT_CNT    0xFFFFFFFF
#define EXTMEM_DBUS0_CONFLICT_CNT_M  ((EXTMEM_DBUS0_CONFLICT_CNT_V)<<(EXTMEM_DBUS0_CONFLICT_CNT_S))
#define EXTMEM_DBUS0_CONFLICT_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS0_CONFLICT_CNT_S  0

#define EXTMEM_DBUS0_ACS_NXTLVL_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1C8)
/* EXTMEM_DBUS0_NXTLVL_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of times that L1-DCache accesses L2-Cache due to
 bus0 accessing L1-DCache..*/
#define EXTMEM_DBUS0_NXTLVL_CNT    0xFFFFFFFF
#define EXTMEM_DBUS0_NXTLVL_CNT_M  ((EXTMEM_DBUS0_NXTLVL_CNT_V)<<(EXTMEM_DBUS0_NXTLVL_CNT_S))
#define EXTMEM_DBUS0_NXTLVL_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS0_NXTLVL_CNT_S  0

#define EXTMEM_DBUS1_ACS_HIT_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1CC)
/* EXTMEM_DBUS1_HIT_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of hits when bus1 accesses L1-DCache..*/
#define EXTMEM_DBUS1_HIT_CNT    0xFFFFFFFF
#define EXTMEM_DBUS1_HIT_CNT_M  ((EXTMEM_DBUS1_HIT_CNT_V)<<(EXTMEM_DBUS1_HIT_CNT_S))
#define EXTMEM_DBUS1_HIT_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS1_HIT_CNT_S  0

#define EXTMEM_DBUS1_ACS_MISS_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1D0)
/* EXTMEM_DBUS1_MISS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of missing when bus1 accesses L1-DCache..*/
#define EXTMEM_DBUS1_MISS_CNT    0xFFFFFFFF
#define EXTMEM_DBUS1_MISS_CNT_M  ((EXTMEM_DBUS1_MISS_CNT_V)<<(EXTMEM_DBUS1_MISS_CNT_S))
#define EXTMEM_DBUS1_MISS_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS1_MISS_CNT_S  0

#define EXTMEM_DBUS1_ACS_CONFLICT_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1D4)
/* EXTMEM_DBUS1_CONFLICT_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of access-conflicts when bus1 accesses L1-DCache
..*/
#define EXTMEM_DBUS1_CONFLICT_CNT    0xFFFFFFFF
#define EXTMEM_DBUS1_CONFLICT_CNT_M  ((EXTMEM_DBUS1_CONFLICT_CNT_V)<<(EXTMEM_DBUS1_CONFLICT_CNT_S))
#define EXTMEM_DBUS1_CONFLICT_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS1_CONFLICT_CNT_S  0

#define EXTMEM_DBUS1_ACS_NXTLVL_CNT_REG          (DR_REG_EXTMEM_BASE + 0x1D8)
/* EXTMEM_DBUS1_NXTLVL_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the number of times that L1-DCache accesses L2-Cache due to
 bus1 accessing L1-DCache..*/
#define EXTMEM_DBUS1_NXTLVL_CNT    0xFFFFFFFF
#define EXTMEM_DBUS1_NXTLVL_CNT_M  ((EXTMEM_DBUS1_NXTLVL_CNT_V)<<(EXTMEM_DBUS1_NXTLVL_CNT_S))
#define EXTMEM_DBUS1_NXTLVL_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS1_NXTLVL_CNT_S  0

#define EXTMEM_ICACHE0_ACS_FAIL_ID_ATTR_REG          (DR_REG_EXTMEM_BASE + 0x1FC)
/* EXTMEM_ICACHE0_FAIL_ATTR : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: The register records the attribution of fail-access when cache0 accesses L1-ICac
he..*/
#define EXTMEM_ICACHE0_FAIL_ATTR    0x0000FFFF
#define EXTMEM_ICACHE0_FAIL_ATTR_M  ((EXTMEM_ICACHE0_FAIL_ATTR_V)<<(EXTMEM_ICACHE0_FAIL_ATTR_S))
#define EXTMEM_ICACHE0_FAIL_ATTR_V  0xFFFF
#define EXTMEM_ICACHE0_FAIL_ATTR_S  16
/* EXTMEM_ICACHE0_FAIL_ID : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The register records the ID of fail-access when cache0 accesses L1-ICache..*/
#define EXTMEM_ICACHE0_FAIL_ID    0x0000FFFF
#define EXTMEM_ICACHE0_FAIL_ID_M  ((EXTMEM_ICACHE0_FAIL_ID_V)<<(EXTMEM_ICACHE0_FAIL_ID_S))
#define EXTMEM_ICACHE0_FAIL_ID_V  0xFFFF
#define EXTMEM_ICACHE0_FAIL_ID_S  0

#define EXTMEM_ICACHE0_ACS_FAIL_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x200)
/* EXTMEM_ICACHE0_FAIL_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the address of fail-access when cache0 accesses L1-ICache..*/
#define EXTMEM_ICACHE0_FAIL_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE0_FAIL_ADDR_M  ((EXTMEM_ICACHE0_FAIL_ADDR_V)<<(EXTMEM_ICACHE0_FAIL_ADDR_S))
#define EXTMEM_ICACHE0_FAIL_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE0_FAIL_ADDR_S  0

#define EXTMEM_ICACHE1_ACS_FAIL_ID_ATTR_REG          (DR_REG_EXTMEM_BASE + 0x204)
/* EXTMEM_ICACHE1_FAIL_ATTR : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: The register records the attribution of fail-access when cache1 accesses L1-ICac
he..*/
#define EXTMEM_ICACHE1_FAIL_ATTR    0x0000FFFF
#define EXTMEM_ICACHE1_FAIL_ATTR_M  ((EXTMEM_ICACHE1_FAIL_ATTR_V)<<(EXTMEM_ICACHE1_FAIL_ATTR_S))
#define EXTMEM_ICACHE1_FAIL_ATTR_V  0xFFFF
#define EXTMEM_ICACHE1_FAIL_ATTR_S  16
/* EXTMEM_ICACHE1_FAIL_ID : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The register records the ID of fail-access when cache1 accesses L1-ICache..*/
#define EXTMEM_ICACHE1_FAIL_ID    0x0000FFFF
#define EXTMEM_ICACHE1_FAIL_ID_M  ((EXTMEM_ICACHE1_FAIL_ID_V)<<(EXTMEM_ICACHE1_FAIL_ID_S))
#define EXTMEM_ICACHE1_FAIL_ID_V  0xFFFF
#define EXTMEM_ICACHE1_FAIL_ID_S  0

#define EXTMEM_ICACHE1_ACS_FAIL_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x208)
/* EXTMEM_ICACHE1_FAIL_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the address of fail-access when cache1 accesses L1-ICache..*/
#define EXTMEM_ICACHE1_FAIL_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE1_FAIL_ADDR_M  ((EXTMEM_ICACHE1_FAIL_ADDR_V)<<(EXTMEM_ICACHE1_FAIL_ADDR_S))
#define EXTMEM_ICACHE1_FAIL_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE1_FAIL_ADDR_S  0

#define EXTMEM_DCACHE_ACS_FAIL_ID_ATTR_REG          (DR_REG_EXTMEM_BASE + 0x21C)
/* EXTMEM_DCACHE_FAIL_ATTR : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: The register records the attribution of fail-access when cache accesses L1-DCach
e..*/
#define EXTMEM_DCACHE_FAIL_ATTR    0x0000FFFF
#define EXTMEM_DCACHE_FAIL_ATTR_M  ((EXTMEM_DCACHE_FAIL_ATTR_V)<<(EXTMEM_DCACHE_FAIL_ATTR_S))
#define EXTMEM_DCACHE_FAIL_ATTR_V  0xFFFF
#define EXTMEM_DCACHE_FAIL_ATTR_S  16
/* EXTMEM_DCACHE_FAIL_ID : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The register records the ID of fail-access when cache accesses L1-DCache..*/
#define EXTMEM_DCACHE_FAIL_ID    0x0000FFFF
#define EXTMEM_DCACHE_FAIL_ID_M  ((EXTMEM_DCACHE_FAIL_ID_V)<<(EXTMEM_DCACHE_FAIL_ID_S))
#define EXTMEM_DCACHE_FAIL_ID_V  0xFFFF
#define EXTMEM_DCACHE_FAIL_ID_S  0

#define EXTMEM_DCACHE_ACS_FAIL_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x220)
/* EXTMEM_DCACHE_FAIL_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The register records the address of fail-access when cache accesses L1-DCache..*/
#define EXTMEM_DCACHE_FAIL_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_FAIL_ADDR_M  ((EXTMEM_DCACHE_FAIL_ADDR_V)<<(EXTMEM_DCACHE_FAIL_ADDR_S))
#define EXTMEM_DCACHE_FAIL_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_FAIL_ADDR_S  0

#define EXTMEM_CACHE_SYNC_PRELOAD_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0x224)
/* EXTMEM_CACHE_SYNC_ERR_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt of Cache sync-operation error..*/
#define EXTMEM_CACHE_SYNC_ERR_INT_ENA    (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_ENA_M  (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_ENA_V  0x1
#define EXTMEM_CACHE_SYNC_ERR_INT_ENA_S  13
/* EXTMEM_DCACHE_PLD_ERR_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt of L1-DCache preload-operation error..*/
#define EXTMEM_DCACHE_PLD_ERR_INT_ENA    (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_ENA_M  (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_ENA_V  0x1
#define EXTMEM_DCACHE_PLD_ERR_INT_ENA_S  11
/* EXTMEM_CACHE_SYNC_DONE_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt of Cache sync-operation done..*/
#define EXTMEM_CACHE_SYNC_DONE_INT_ENA    (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_ENA_M  (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_ENA_V  0x1
#define EXTMEM_CACHE_SYNC_DONE_INT_ENA_S  6
/* EXTMEM_DCACHE_PLD_DONE_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt of L1-DCache preload-operation. If preload o
peration is done, interrupt occurs..*/
#define EXTMEM_DCACHE_PLD_DONE_INT_ENA    (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_ENA_M  (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_ENA_V  0x1
#define EXTMEM_DCACHE_PLD_DONE_INT_ENA_S  4

#define EXTMEM_CACHE_SYNC_PRELOAD_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0x228)
/* EXTMEM_CACHE_SYNC_ERR_INT_CLR : WT ;bitpos:[13] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt of Cache sync-operation error..*/
#define EXTMEM_CACHE_SYNC_ERR_INT_CLR    (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_CLR_M  (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_CLR_V  0x1
#define EXTMEM_CACHE_SYNC_ERR_INT_CLR_S  13
/* EXTMEM_DCACHE_PLD_ERR_INT_CLR : WT ;bitpos:[11] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt of L1-DCache preload-operation error..*/
#define EXTMEM_DCACHE_PLD_ERR_INT_CLR    (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_CLR_M  (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_CLR_V  0x1
#define EXTMEM_DCACHE_PLD_ERR_INT_CLR_S  11
/* EXTMEM_CACHE_SYNC_DONE_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt that occurs only when Cache sync-operation is
 done..*/
#define EXTMEM_CACHE_SYNC_DONE_INT_CLR    (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_CLR_M  (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_CLR_V  0x1
#define EXTMEM_CACHE_SYNC_DONE_INT_CLR_S  6
/* EXTMEM_DCACHE_PLD_DONE_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt that occurs only when L1-DCache preload-opera
tion is done..*/
#define EXTMEM_DCACHE_PLD_DONE_INT_CLR    (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_CLR_M  (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_CLR_V  0x1
#define EXTMEM_DCACHE_PLD_DONE_INT_CLR_S  4

#define EXTMEM_CACHE_SYNC_PRELOAD_INT_RAW_REG          (DR_REG_EXTMEM_BASE + 0x22C)
/* EXTMEM_CACHE_SYNC_ERR_INT_RAW : R/WTC/SS ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw bit of the interrupt that occurs only when Cache sync-operation error oc
curs..*/
#define EXTMEM_CACHE_SYNC_ERR_INT_RAW    (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_RAW_M  (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_RAW_V  0x1
#define EXTMEM_CACHE_SYNC_ERR_INT_RAW_S  13
/* EXTMEM_DCACHE_PLD_ERR_INT_RAW : R/WTC/SS ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw bit of the interrupt that occurs only when L1-DCache preload-operation e
rror occurs..*/
#define EXTMEM_DCACHE_PLD_ERR_INT_RAW    (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_RAW_M  (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_RAW_V  0x1
#define EXTMEM_DCACHE_PLD_ERR_INT_RAW_S  11
/* EXTMEM_CACHE_SYNC_DONE_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw bit of the interrupt that occurs only when Cache sync-operation is done..*/
#define EXTMEM_CACHE_SYNC_DONE_INT_RAW    (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_RAW_M  (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_RAW_V  0x1
#define EXTMEM_CACHE_SYNC_DONE_INT_RAW_S  6
/* EXTMEM_DCACHE_PLD_DONE_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw bit of the interrupt that occurs only when L1-DCache preload-operation i
s done..*/
#define EXTMEM_DCACHE_PLD_DONE_INT_RAW    (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_RAW_M  (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_RAW_V  0x1
#define EXTMEM_DCACHE_PLD_DONE_INT_RAW_S  4

#define EXTMEM_CACHE_SYNC_PRELOAD_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0x230)
/* EXTMEM_CACHE_SYNC_ERR_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The bit indicates the status of the interrupt of Cache sync-operation error..*/
#define EXTMEM_CACHE_SYNC_ERR_INT_ST    (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_ST_M  (BIT(13))
#define EXTMEM_CACHE_SYNC_ERR_INT_ST_V  0x1
#define EXTMEM_CACHE_SYNC_ERR_INT_ST_S  13
/* EXTMEM_DCACHE_PLD_ERR_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The bit indicates the status of the interrupt of L1-DCache preload-operation err
or..*/
#define EXTMEM_DCACHE_PLD_ERR_INT_ST    (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_ST_M  (BIT(11))
#define EXTMEM_DCACHE_PLD_ERR_INT_ST_V  0x1
#define EXTMEM_DCACHE_PLD_ERR_INT_ST_S  11
/* EXTMEM_CACHE_SYNC_DONE_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit indicates the status of the interrupt that occurs only when Cache sync-o
peration is done..*/
#define EXTMEM_CACHE_SYNC_DONE_INT_ST    (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_ST_M  (BIT(6))
#define EXTMEM_CACHE_SYNC_DONE_INT_ST_V  0x1
#define EXTMEM_CACHE_SYNC_DONE_INT_ST_S  6
/* EXTMEM_DCACHE_PLD_DONE_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit indicates the status of the interrupt that occurs only when L1-DCache pr
eload-operation is done..*/
#define EXTMEM_DCACHE_PLD_DONE_INT_ST    (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_ST_M  (BIT(4))
#define EXTMEM_DCACHE_PLD_DONE_INT_ST_V  0x1
#define EXTMEM_DCACHE_PLD_DONE_INT_ST_S  4

#define EXTMEM_CACHE_SYNC_PRELOAD_EXCEPTION_REG          (DR_REG_EXTMEM_BASE + 0x234)
/* EXTMEM_CACHE_SYNC_ERR_CODE : RO ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: The values 0-2 are available which means sync map, command conflict and size are
 error in Cache System..*/
#define EXTMEM_CACHE_SYNC_ERR_CODE    0x00000003
#define EXTMEM_CACHE_SYNC_ERR_CODE_M  ((EXTMEM_CACHE_SYNC_ERR_CODE_V)<<(EXTMEM_CACHE_SYNC_ERR_CODE_S))
#define EXTMEM_CACHE_SYNC_ERR_CODE_V  0x3
#define EXTMEM_CACHE_SYNC_ERR_CODE_S  12
/* EXTMEM_DCACHE_PLD_ERR_CODE : RO ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: The value 2 is Only available which means preload size is error in L1-DCache..*/
#define EXTMEM_DCACHE_PLD_ERR_CODE    0x00000003
#define EXTMEM_DCACHE_PLD_ERR_CODE_M  ((EXTMEM_DCACHE_PLD_ERR_CODE_V)<<(EXTMEM_DCACHE_PLD_ERR_CODE_S))
#define EXTMEM_DCACHE_PLD_ERR_CODE_V  0x3
#define EXTMEM_DCACHE_PLD_ERR_CODE_S  8

#define EXTMEM_CACHE_SYNC_RST_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x238)
/* EXTMEM_DCACHE_SYNC_RST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: set this bit to reset sync-logic inside L1-DCache. Recommend that this should on
ly be used to initialize sync-logic when some fatal error of sync-logic occurs..*/
#define EXTMEM_DCACHE_SYNC_RST    (BIT(4))
#define EXTMEM_DCACHE_SYNC_RST_M  (BIT(4))
#define EXTMEM_DCACHE_SYNC_RST_V  0x1
#define EXTMEM_DCACHE_SYNC_RST_S  4

#define EXTMEM_CACHE_PRELOAD_RST_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x23C)
/* EXTMEM_DCACHE_PLD_RST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: set this bit to reset preload-logic inside L1-DCache. Recommend that this should
 only be used to initialize preload-logic when some fatal error of preload-logic
 occurs..*/
#define EXTMEM_DCACHE_PLD_RST    (BIT(4))
#define EXTMEM_DCACHE_PLD_RST_M  (BIT(4))
#define EXTMEM_DCACHE_PLD_RST_V  0x1
#define EXTMEM_DCACHE_PLD_RST_S  4

#define EXTMEM_CACHE_AUTOLOAD_BUF_CLR_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x240)
/* EXTMEM_DCACHE_ALD_BUF_CLR : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: set this bit to clear autoload-buffer inside L1-DCache. If this bit is active, a
utoload will not work in L1-DCache. This bit should not be active when autoload
works in L1-DCache..*/
#define EXTMEM_DCACHE_ALD_BUF_CLR    (BIT(4))
#define EXTMEM_DCACHE_ALD_BUF_CLR_M  (BIT(4))
#define EXTMEM_DCACHE_ALD_BUF_CLR_V  0x1
#define EXTMEM_DCACHE_ALD_BUF_CLR_S  4

#define EXTMEM_UNALLOCATE_BUFFER_CLEAR_REG          (DR_REG_EXTMEM_BASE + 0x244)
/* EXTMEM_DCACHE_UNALLOC_CLR : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear the unallocate request buffer of l1 dcache where the un
allocate request is responsed but not completed..*/
#define EXTMEM_DCACHE_UNALLOC_CLR    (BIT(4))
#define EXTMEM_DCACHE_UNALLOC_CLR_M  (BIT(4))
#define EXTMEM_DCACHE_UNALLOC_CLR_V  0x1
#define EXTMEM_DCACHE_UNALLOC_CLR_S  4

#define EXTMEM_CACHE_OBJECT_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x248)
/* EXTMEM_DCACHE_MEM_OBJECT : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to set L1-DCache data memory as object. This bit should be onehot w
ith the others fields inside this register..*/
#define EXTMEM_DCACHE_MEM_OBJECT    (BIT(10))
#define EXTMEM_DCACHE_MEM_OBJECT_M  (BIT(10))
#define EXTMEM_DCACHE_MEM_OBJECT_V  0x1
#define EXTMEM_DCACHE_MEM_OBJECT_S  10
/* EXTMEM_DCACHE_TAG_OBJECT : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to set L1-DCache tag memory as object. This bit should be onehot wi
th the others fields inside this register..*/
#define EXTMEM_DCACHE_TAG_OBJECT    (BIT(4))
#define EXTMEM_DCACHE_TAG_OBJECT_M  (BIT(4))
#define EXTMEM_DCACHE_TAG_OBJECT_V  0x1
#define EXTMEM_DCACHE_TAG_OBJECT_S  4

#define EXTMEM_CACHE_WAY_OBJECT_REG          (DR_REG_EXTMEM_BASE + 0x24C)
/* EXTMEM_CACHE_WAY_OBJECT : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: Set this bits to select which way of the tag-object will be accessed. 0: way0, 1
: way1, 2: way2, 3: way3, ?, 7: way7..*/
#define EXTMEM_CACHE_WAY_OBJECT    0x00000007
#define EXTMEM_CACHE_WAY_OBJECT_M  ((EXTMEM_CACHE_WAY_OBJECT_V)<<(EXTMEM_CACHE_WAY_OBJECT_S))
#define EXTMEM_CACHE_WAY_OBJECT_V  0x7
#define EXTMEM_CACHE_WAY_OBJECT_S  0

#define EXTMEM_CACHE_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x250)
/* EXTMEM_CACHE_VADDR : R/W ;bitpos:[31:0] ;default: 32'h40000000 ; */
/*description: Those bits stores the virtual address which will decide where inside the specifi
ed tag memory object will be accessed..*/
#define EXTMEM_CACHE_VADDR    0xFFFFFFFF
#define EXTMEM_CACHE_VADDR_M  ((EXTMEM_CACHE_VADDR_V)<<(EXTMEM_CACHE_VADDR_S))
#define EXTMEM_CACHE_VADDR_V  0xFFFFFFFF
#define EXTMEM_CACHE_VADDR_S  0

#define EXTMEM_CACHE_DEBUG_BUS_REG          (DR_REG_EXTMEM_BASE + 0x254)
/* EXTMEM_CACHE_DEBUG_BUS : R/W ;bitpos:[31:0] ;default: 32'h254 ; */
/*description: This is a constant place where we can write data to or read data from the tag/da
ta memory on the specified cache..*/
#define EXTMEM_CACHE_DEBUG_BUS    0xFFFFFFFF
#define EXTMEM_CACHE_DEBUG_BUS_M  ((EXTMEM_CACHE_DEBUG_BUS_V)<<(EXTMEM_CACHE_DEBUG_BUS_S))
#define EXTMEM_CACHE_DEBUG_BUS_V  0xFFFFFFFF
#define EXTMEM_CACHE_DEBUG_BUS_S  0

#define EXTMEM_DATE_REG          (DR_REG_EXTMEM_BASE + 0x3FC)
/* EXTMEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h2202080 ; */
/*description: version control register. Note that this default value stored is the latest date
 when the hardware logic was updated..*/
#define EXTMEM_DATE    0x0FFFFFFF
#define EXTMEM_DATE_M  ((EXTMEM_DATE_V)<<(EXTMEM_DATE_S))
#define EXTMEM_DATE_V  0xFFFFFFF
#define EXTMEM_DATE_S  0


#ifdef __cplusplus
}
#endif
