/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** EXTMEM_ICACHE_CTRL_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_CTRL_REG (DR_REG_EXTMEM_BASE + 0x0)
/** EXTMEM_ICACHE_ENABLE : R/W; bitpos: [0]; default: 0;
 *  The bit is used to activate the data cache. 0: disable, 1: enable
 */
#define EXTMEM_ICACHE_ENABLE    (BIT(0))
#define EXTMEM_ICACHE_ENABLE_M  (EXTMEM_ICACHE_ENABLE_V << EXTMEM_ICACHE_ENABLE_S)
#define EXTMEM_ICACHE_ENABLE_V  0x00000001U
#define EXTMEM_ICACHE_ENABLE_S  0

/** EXTMEM_ICACHE_CTRL1_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_CTRL1_REG (DR_REG_EXTMEM_BASE + 0x4)
/** EXTMEM_ICACHE_SHUT_IBUS : R/W; bitpos: [0]; default: 1;
 *  The bit is used to disable core0 ibus, 0: enable, 1: disable
 */
#define EXTMEM_ICACHE_SHUT_IBUS    (BIT(0))
#define EXTMEM_ICACHE_SHUT_IBUS_M  (EXTMEM_ICACHE_SHUT_IBUS_V << EXTMEM_ICACHE_SHUT_IBUS_S)
#define EXTMEM_ICACHE_SHUT_IBUS_V  0x00000001U
#define EXTMEM_ICACHE_SHUT_IBUS_S  0
/** EXTMEM_ICACHE_SHUT_DBUS : R/W; bitpos: [1]; default: 1;
 *  The bit is used to disable core1 ibus, 0: enable, 1: disable
 */
#define EXTMEM_ICACHE_SHUT_DBUS    (BIT(1))
#define EXTMEM_ICACHE_SHUT_DBUS_M  (EXTMEM_ICACHE_SHUT_DBUS_V << EXTMEM_ICACHE_SHUT_DBUS_S)
#define EXTMEM_ICACHE_SHUT_DBUS_V  0x00000001U
#define EXTMEM_ICACHE_SHUT_DBUS_S  1

/** EXTMEM_ICACHE_TAG_POWER_CTRL_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_TAG_POWER_CTRL_REG (DR_REG_EXTMEM_BASE + 0x8)
/** EXTMEM_ICACHE_TAG_MEM_FORCE_ON : R/W; bitpos: [0]; default: 1;
 *  The bit is used to close clock gating of  icache tag memory. 1: close gating, 0:
 *  open clock gating.
 */
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON    (BIT(0))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_M  (EXTMEM_ICACHE_TAG_MEM_FORCE_ON_V << EXTMEM_ICACHE_TAG_MEM_FORCE_ON_S)
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_V  0x00000001U
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_S  0
/** EXTMEM_ICACHE_TAG_MEM_FORCE_PD : R/W; bitpos: [1]; default: 0;
 *  The bit is used to power  icache tag memory down, 0: follow rtc_lslp, 1: power down
 */
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD    (BIT(1))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_M  (EXTMEM_ICACHE_TAG_MEM_FORCE_PD_V << EXTMEM_ICACHE_TAG_MEM_FORCE_PD_S)
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_V  0x00000001U
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_S  1
/** EXTMEM_ICACHE_TAG_MEM_FORCE_PU : R/W; bitpos: [2]; default: 1;
 *  The bit is used to power  icache tag memory up, 0: follow rtc_lslp, 1: power up
 */
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU    (BIT(2))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_M  (EXTMEM_ICACHE_TAG_MEM_FORCE_PU_V << EXTMEM_ICACHE_TAG_MEM_FORCE_PU_S)
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_V  0x00000001U
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_S  2

/** EXTMEM_ICACHE_SYNC_CTRL_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_SYNC_CTRL_REG (DR_REG_EXTMEM_BASE + 0x28)
/** EXTMEM_ICACHE_INVALIDATE_ENA : R/W/SS; bitpos: [0]; default: 1;
 *  The bit is used to enable invalidate operation. It will be cleared by hardware
 *  after invalidate operation done.
 */
#define EXTMEM_ICACHE_INVALIDATE_ENA    (BIT(0))
#define EXTMEM_ICACHE_INVALIDATE_ENA_M  (EXTMEM_ICACHE_INVALIDATE_ENA_V << EXTMEM_ICACHE_INVALIDATE_ENA_S)
#define EXTMEM_ICACHE_INVALIDATE_ENA_V  0x00000001U
#define EXTMEM_ICACHE_INVALIDATE_ENA_S  0
/** EXTMEM_ICACHE_SYNC_DONE : RO; bitpos: [1]; default: 0;
 *  The bit is used to indicate invalidate operation is finished.
 */
#define EXTMEM_ICACHE_SYNC_DONE    (BIT(1))
#define EXTMEM_ICACHE_SYNC_DONE_M  (EXTMEM_ICACHE_SYNC_DONE_V << EXTMEM_ICACHE_SYNC_DONE_S)
#define EXTMEM_ICACHE_SYNC_DONE_V  0x00000001U
#define EXTMEM_ICACHE_SYNC_DONE_S  1

/** EXTMEM_ICACHE_SYNC_ADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_SYNC_ADDR_REG (DR_REG_EXTMEM_BASE + 0x2c)
/** EXTMEM_ICACHE_SYNC_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The bits are used to configure the start virtual address for clean operations. It
 *  should be combined with ICACHE_SYNC_SIZE_REG.
 */
#define EXTMEM_ICACHE_SYNC_ADDR    0xFFFFFFFFU
#define EXTMEM_ICACHE_SYNC_ADDR_M  (EXTMEM_ICACHE_SYNC_ADDR_V << EXTMEM_ICACHE_SYNC_ADDR_S)
#define EXTMEM_ICACHE_SYNC_ADDR_V  0xFFFFFFFFU
#define EXTMEM_ICACHE_SYNC_ADDR_S  0

/** EXTMEM_ICACHE_SYNC_SIZE_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_SYNC_SIZE_REG (DR_REG_EXTMEM_BASE + 0x30)
/** EXTMEM_ICACHE_SYNC_SIZE : R/W; bitpos: [22:0]; default: 0;
 *  The bits are used to configure the length for sync operations. The bits are the
 *  counts of cache block. It should be combined with ICACHE_SYNC_ADDR_REG.
 */
#define EXTMEM_ICACHE_SYNC_SIZE    0x007FFFFFU
#define EXTMEM_ICACHE_SYNC_SIZE_M  (EXTMEM_ICACHE_SYNC_SIZE_V << EXTMEM_ICACHE_SYNC_SIZE_S)
#define EXTMEM_ICACHE_SYNC_SIZE_V  0x007FFFFFU
#define EXTMEM_ICACHE_SYNC_SIZE_S  0

/** EXTMEM_IBUS_TO_FLASH_START_VADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_REG (DR_REG_EXTMEM_BASE + 0x54)
/** EXTMEM_IBUS_TO_FLASH_START_VADDR : R/W; bitpos: [31:0]; default: 1107296256;
 *  The bits are used to configure the start virtual address of ibus to access flash.
 *  The register is used to give constraints to ibus access counter.
 */
#define EXTMEM_IBUS_TO_FLASH_START_VADDR    0xFFFFFFFFU
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_M  (EXTMEM_IBUS_TO_FLASH_START_VADDR_V << EXTMEM_IBUS_TO_FLASH_START_VADDR_S)
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_V  0xFFFFFFFFU
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_S  0

/** EXTMEM_IBUS_TO_FLASH_END_VADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_REG (DR_REG_EXTMEM_BASE + 0x58)
/** EXTMEM_IBUS_TO_FLASH_END_VADDR : R/W; bitpos: [31:0]; default: 1111490559;
 *  The bits are used to configure the end virtual address of ibus to access flash. The
 *  register is used to give constraints to ibus access counter.
 */
#define EXTMEM_IBUS_TO_FLASH_END_VADDR    0xFFFFFFFFU
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_M  (EXTMEM_IBUS_TO_FLASH_END_VADDR_V << EXTMEM_IBUS_TO_FLASH_END_VADDR_S)
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_V  0xFFFFFFFFU
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_S  0

/** EXTMEM_DBUS_TO_FLASH_START_VADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_REG (DR_REG_EXTMEM_BASE + 0x5c)
/** EXTMEM_DBUS_TO_FLASH_START_VADDR : R/W; bitpos: [31:0]; default: 1006632960;
 *  The bits are used to configure the start virtual address of dbus to access flash.
 *  The register is used to give constraints to dbus access counter.
 */
#define EXTMEM_DBUS_TO_FLASH_START_VADDR    0xFFFFFFFFU
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_M  (EXTMEM_DBUS_TO_FLASH_START_VADDR_V << EXTMEM_DBUS_TO_FLASH_START_VADDR_S)
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_V  0xFFFFFFFFU
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_S  0

/** EXTMEM_DBUS_TO_FLASH_END_VADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_REG (DR_REG_EXTMEM_BASE + 0x60)
/** EXTMEM_DBUS_TO_FLASH_END_VADDR : R/W; bitpos: [31:0]; default: 1010827263;
 *  The bits are used to configure the end virtual address of dbus to access flash. The
 *  register is used to give constraints to dbus access counter.
 */
#define EXTMEM_DBUS_TO_FLASH_END_VADDR    0xFFFFFFFFU
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_M  (EXTMEM_DBUS_TO_FLASH_END_VADDR_V << EXTMEM_DBUS_TO_FLASH_END_VADDR_S)
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_V  0xFFFFFFFFU
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_S  0

/** EXTMEM_CACHE_ACS_CNT_CLR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_ACS_CNT_CLR_REG (DR_REG_EXTMEM_BASE + 0x64)
/** EXTMEM_IBUS_ACS_CNT_CLR : WOD; bitpos: [0]; default: 0;
 *  The bit is used to clear ibus counter.
 */
#define EXTMEM_IBUS_ACS_CNT_CLR    (BIT(0))
#define EXTMEM_IBUS_ACS_CNT_CLR_M  (EXTMEM_IBUS_ACS_CNT_CLR_V << EXTMEM_IBUS_ACS_CNT_CLR_S)
#define EXTMEM_IBUS_ACS_CNT_CLR_V  0x00000001U
#define EXTMEM_IBUS_ACS_CNT_CLR_S  0
/** EXTMEM_DBUS_ACS_CNT_CLR : WOD; bitpos: [1]; default: 0;
 *  The bit is used to clear dbus counter.
 */
#define EXTMEM_DBUS_ACS_CNT_CLR    (BIT(1))
#define EXTMEM_DBUS_ACS_CNT_CLR_M  (EXTMEM_DBUS_ACS_CNT_CLR_V << EXTMEM_DBUS_ACS_CNT_CLR_S)
#define EXTMEM_DBUS_ACS_CNT_CLR_V  0x00000001U
#define EXTMEM_DBUS_ACS_CNT_CLR_S  1

/** EXTMEM_CACHE_ILG_INT_ENA_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_ILG_INT_ENA_REG (DR_REG_EXTMEM_BASE + 0x78)
/** EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable interrupt by sync configurations fault.
 */
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA    (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_M  (EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_V << EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_S)
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_V  0x00000001U
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_S  0
/** EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable interrupt by preload configurations fault.
 */
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_M  (EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_V << EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_S)
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_V  0x00000001U
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_S  1
/** EXTMEM_MMU_ENTRY_FAULT_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable interrupt by mmu entry fault.
 */
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA    (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_M  (EXTMEM_MMU_ENTRY_FAULT_INT_ENA_V << EXTMEM_MMU_ENTRY_FAULT_INT_ENA_S)
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_V  0x00000001U
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_S  5
/** EXTMEM_IBUS_CNT_OVF_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The bit is used to enable interrupt by ibus counter overflow.
 */
#define EXTMEM_IBUS_CNT_OVF_INT_ENA    (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_M  (EXTMEM_IBUS_CNT_OVF_INT_ENA_V << EXTMEM_IBUS_CNT_OVF_INT_ENA_S)
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_V  0x00000001U
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_S  7
/** EXTMEM_DBUS_CNT_OVF_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The bit is used to enable interrupt by dbus counter overflow.
 */
#define EXTMEM_DBUS_CNT_OVF_INT_ENA    (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_M  (EXTMEM_DBUS_CNT_OVF_INT_ENA_V << EXTMEM_DBUS_CNT_OVF_INT_ENA_S)
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_V  0x00000001U
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_S  8

/** EXTMEM_CACHE_ILG_INT_CLR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_ILG_INT_CLR_REG (DR_REG_EXTMEM_BASE + 0x7c)
/** EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR : WOD; bitpos: [0]; default: 0;
 *  The bit is used to clear interrupt by sync configurations fault.
 */
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR    (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_M  (EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_V << EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_S)
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_V  0x00000001U
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_S  0
/** EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR : WOD; bitpos: [1]; default: 0;
 *  The bit is used to clear interrupt by preload configurations fault.
 */
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_M  (EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_V << EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_S)
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_V  0x00000001U
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_S  1
/** EXTMEM_MMU_ENTRY_FAULT_INT_CLR : WOD; bitpos: [5]; default: 0;
 *  The bit is used to clear interrupt by mmu entry fault.
 */
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR    (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_M  (EXTMEM_MMU_ENTRY_FAULT_INT_CLR_V << EXTMEM_MMU_ENTRY_FAULT_INT_CLR_S)
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_V  0x00000001U
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_S  5
/** EXTMEM_IBUS_CNT_OVF_INT_CLR : WOD; bitpos: [7]; default: 0;
 *  The bit is used to clear interrupt by ibus counter overflow.
 */
#define EXTMEM_IBUS_CNT_OVF_INT_CLR    (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_M  (EXTMEM_IBUS_CNT_OVF_INT_CLR_V << EXTMEM_IBUS_CNT_OVF_INT_CLR_S)
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_V  0x00000001U
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_S  7
/** EXTMEM_DBUS_CNT_OVF_INT_CLR : WOD; bitpos: [8]; default: 0;
 *  The bit is used to clear interrupt by dbus counter overflow.
 */
#define EXTMEM_DBUS_CNT_OVF_INT_CLR    (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_M  (EXTMEM_DBUS_CNT_OVF_INT_CLR_V << EXTMEM_DBUS_CNT_OVF_INT_CLR_S)
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_V  0x00000001U
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_S  8

/** EXTMEM_CACHE_ILG_INT_ST_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_ILG_INT_ST_REG (DR_REG_EXTMEM_BASE + 0x80)
/** EXTMEM_ICACHE_SYNC_OP_FAULT_ST : RO; bitpos: [0]; default: 0;
 *  The bit is used to indicate interrupt by sync configurations fault.
 */
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST    (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_M  (EXTMEM_ICACHE_SYNC_OP_FAULT_ST_V << EXTMEM_ICACHE_SYNC_OP_FAULT_ST_S)
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_V  0x00000001U
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_S  0
/** EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST : RO; bitpos: [1]; default: 0;
 *  The bit is used to indicate interrupt by preload configurations fault.
 */
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_M  (EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_V << EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_S)
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_V  0x00000001U
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_S  1
/** EXTMEM_MMU_ENTRY_FAULT_ST : RO; bitpos: [5]; default: 0;
 *  The bit is used to indicate interrupt by mmu entry fault.
 */
#define EXTMEM_MMU_ENTRY_FAULT_ST    (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_ST_M  (EXTMEM_MMU_ENTRY_FAULT_ST_V << EXTMEM_MMU_ENTRY_FAULT_ST_S)
#define EXTMEM_MMU_ENTRY_FAULT_ST_V  0x00000001U
#define EXTMEM_MMU_ENTRY_FAULT_ST_S  5
/** EXTMEM_IBUS_ACS_CNT_OVF_ST : RO; bitpos: [7]; default: 0;
 *  The bit is used to indicate interrupt by ibus access flash/spiram counter overflow.
 */
#define EXTMEM_IBUS_ACS_CNT_OVF_ST    (BIT(7))
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_M  (EXTMEM_IBUS_ACS_CNT_OVF_ST_V << EXTMEM_IBUS_ACS_CNT_OVF_ST_S)
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_V  0x00000001U
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_S  7
/** EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST : RO; bitpos: [8]; default: 0;
 *  The bit is used to indicate interrupt by ibus access flash/spiram miss counter
 *  overflow.
 */
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST    (BIT(8))
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_M  (EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_V << EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_S)
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_V  0x00000001U
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_S  8
/** EXTMEM_DBUS_ACS_CNT_OVF_ST : RO; bitpos: [9]; default: 0;
 *  The bit is used to indicate interrupt by dbus access flash/spiram counter overflow.
 */
#define EXTMEM_DBUS_ACS_CNT_OVF_ST    (BIT(9))
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_M  (EXTMEM_DBUS_ACS_CNT_OVF_ST_V << EXTMEM_DBUS_ACS_CNT_OVF_ST_S)
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_V  0x00000001U
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_S  9
/** EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST : RO; bitpos: [10]; default: 0;
 *  The bit is used to indicate interrupt by dbus access flash miss counter overflow.
 */
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST    (BIT(10))
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_M  (EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_V << EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_S)
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_V  0x00000001U
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_S  10

/** EXTMEM_CORE0_ACS_CACHE_INT_ENA_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CORE0_ACS_CACHE_INT_ENA_REG (DR_REG_EXTMEM_BASE + 0x84)
/** EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable interrupt by cpu access icache while the corresponding
 *  ibus is disabled which include speculative access.
 */
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA    (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_M  (EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_V << EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_S)
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_V  0x00000001U
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_S  0
/** EXTMEM_CORE0_IBUS_WR_IC_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable interrupt by ibus trying to write icache
 */
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA    (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_M  (EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_V << EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_S)
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_V  0x00000001U
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_S  1
/** EXTMEM_CORE0_IBUS_REJECT_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The bit is used to enable interrupt by authentication fail.
 */
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA    (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_M  (EXTMEM_CORE0_IBUS_REJECT_INT_ENA_V << EXTMEM_CORE0_IBUS_REJECT_INT_ENA_S)
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_V  0x00000001U
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_S  2
/** EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The bit is used to enable interrupt by cpu access icache while the corresponding
 *  dbus is disabled which include speculative access.
 */
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA    (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_M  (EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_V << EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_S)
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_V  0x00000001U
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_S  3
/** EXTMEM_CORE0_DBUS_REJECT_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The bit is used to enable interrupt by authentication fail.
 */
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA    (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_M  (EXTMEM_CORE0_DBUS_REJECT_INT_ENA_V << EXTMEM_CORE0_DBUS_REJECT_INT_ENA_S)
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_V  0x00000001U
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_S  4
/** EXTMEM_CORE0_DBUS_WR_IC_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The bit is used to enable interrupt by dbus trying to write icache
 */
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA    (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_M  (EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_V << EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_S)
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_V  0x00000001U
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_S  5

/** EXTMEM_CORE0_ACS_CACHE_INT_CLR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CORE0_ACS_CACHE_INT_CLR_REG (DR_REG_EXTMEM_BASE + 0x88)
/** EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR : WOD; bitpos: [0]; default: 0;
 *  The bit is used to clear interrupt by cpu access icache while the corresponding
 *  ibus is disabled or icache is disabled which include speculative access.
 */
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR    (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_M  (EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_V << EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_S)
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_V  0x00000001U
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_S  0
/** EXTMEM_CORE0_IBUS_WR_IC_INT_CLR : WOD; bitpos: [1]; default: 0;
 *  The bit is used to clear interrupt by ibus trying to write icache
 */
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR    (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_M  (EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_V << EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_S)
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_V  0x00000001U
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_S  1
/** EXTMEM_CORE0_IBUS_REJECT_INT_CLR : WOD; bitpos: [2]; default: 0;
 *  The bit is used to clear interrupt by authentication fail.
 */
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR    (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_M  (EXTMEM_CORE0_IBUS_REJECT_INT_CLR_V << EXTMEM_CORE0_IBUS_REJECT_INT_CLR_S)
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_V  0x00000001U
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_S  2
/** EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR : WOD; bitpos: [3]; default: 0;
 *  The bit is used to clear interrupt by cpu access icache while the corresponding
 *  dbus is disabled or icache is disabled which include speculative access.
 */
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR    (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_M  (EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_V << EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_S)
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_V  0x00000001U
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_S  3
/** EXTMEM_CORE0_DBUS_REJECT_INT_CLR : WOD; bitpos: [4]; default: 0;
 *  The bit is used to clear interrupt by authentication fail.
 */
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR    (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_M  (EXTMEM_CORE0_DBUS_REJECT_INT_CLR_V << EXTMEM_CORE0_DBUS_REJECT_INT_CLR_S)
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_V  0x00000001U
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_S  4
/** EXTMEM_CORE0_DBUS_WR_IC_INT_CLR : WOD; bitpos: [5]; default: 0;
 *  The bit is used to clear interrupt by dbus trying to write icache
 */
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR    (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_M  (EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_V << EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_S)
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_V  0x00000001U
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_S  5

/** EXTMEM_CORE0_ACS_CACHE_INT_ST_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CORE0_ACS_CACHE_INT_ST_REG (DR_REG_EXTMEM_BASE + 0x8c)
/** EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST : RO; bitpos: [0]; default: 0;
 *  The bit is used to indicate interrupt by cpu access  icache while the core0_ibus is
 *  disabled or icache is disabled which include speculative access.
 */
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST    (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_M  (EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_V << EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_S)
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_V  0x00000001U
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_S  0
/** EXTMEM_CORE0_IBUS_WR_ICACHE_ST : RO; bitpos: [1]; default: 0;
 *  The bit is used to indicate interrupt by ibus trying to write icache
 */
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST    (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_M  (EXTMEM_CORE0_IBUS_WR_ICACHE_ST_V << EXTMEM_CORE0_IBUS_WR_ICACHE_ST_S)
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_V  0x00000001U
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_S  1
/** EXTMEM_CORE0_IBUS_REJECT_ST : RO; bitpos: [2]; default: 0;
 *  The bit is used to indicate interrupt by authentication fail.
 */
#define EXTMEM_CORE0_IBUS_REJECT_ST    (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_ST_M  (EXTMEM_CORE0_IBUS_REJECT_ST_V << EXTMEM_CORE0_IBUS_REJECT_ST_S)
#define EXTMEM_CORE0_IBUS_REJECT_ST_V  0x00000001U
#define EXTMEM_CORE0_IBUS_REJECT_ST_S  2
/** EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST : RO; bitpos: [3]; default: 0;
 *  The bit is used to indicate interrupt by cpu access icache while the core0_dbus is
 *  disabled or icache is disabled which include speculative access.
 */
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST    (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_M  (EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_V << EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_S)
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_V  0x00000001U
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_S  3
/** EXTMEM_CORE0_DBUS_REJECT_ST : RO; bitpos: [4]; default: 0;
 *  The bit is used to indicate interrupt by authentication fail.
 */
#define EXTMEM_CORE0_DBUS_REJECT_ST    (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_ST_M  (EXTMEM_CORE0_DBUS_REJECT_ST_V << EXTMEM_CORE0_DBUS_REJECT_ST_S)
#define EXTMEM_CORE0_DBUS_REJECT_ST_V  0x00000001U
#define EXTMEM_CORE0_DBUS_REJECT_ST_S  4
/** EXTMEM_CORE0_DBUS_WR_ICACHE_ST : RO; bitpos: [5]; default: 0;
 *  The bit is used to indicate interrupt by dbus trying to write icache
 */
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST    (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST_M  (EXTMEM_CORE0_DBUS_WR_ICACHE_ST_V << EXTMEM_CORE0_DBUS_WR_ICACHE_ST_S)
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST_V  0x00000001U
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST_S  5

/** EXTMEM_CORE0_DBUS_REJECT_ST_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CORE0_DBUS_REJECT_ST_REG (DR_REG_EXTMEM_BASE + 0x90)
/** EXTMEM_CORE0_DBUS_ATTR : RO; bitpos: [2:0]; default: 0;
 *  The bits are used to indicate the attribute of CPU access dbus when authentication
 *  fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able.
 */
#define EXTMEM_CORE0_DBUS_ATTR    0x00000007U
#define EXTMEM_CORE0_DBUS_ATTR_M  (EXTMEM_CORE0_DBUS_ATTR_V << EXTMEM_CORE0_DBUS_ATTR_S)
#define EXTMEM_CORE0_DBUS_ATTR_V  0x00000007U
#define EXTMEM_CORE0_DBUS_ATTR_S  0
/** EXTMEM_CORE0_DBUS_WORLD : RO; bitpos: [3]; default: 0;
 *  The bit is used to indicate the world of CPU access dbus when authentication fail.
 *  0: WORLD0, 1: WORLD1
 */
#define EXTMEM_CORE0_DBUS_WORLD    (BIT(3))
#define EXTMEM_CORE0_DBUS_WORLD_M  (EXTMEM_CORE0_DBUS_WORLD_V << EXTMEM_CORE0_DBUS_WORLD_S)
#define EXTMEM_CORE0_DBUS_WORLD_V  0x00000001U
#define EXTMEM_CORE0_DBUS_WORLD_S  3

/** EXTMEM_CORE0_DBUS_REJECT_VADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CORE0_DBUS_REJECT_VADDR_REG (DR_REG_EXTMEM_BASE + 0x94)
/** EXTMEM_CORE0_DBUS_VADDR : RO; bitpos: [31:0]; default: 4294967295;
 *  The bits are used to indicate the virtual address of CPU access dbus when
 *  authentication fail.
 */
#define EXTMEM_CORE0_DBUS_VADDR    0xFFFFFFFFU
#define EXTMEM_CORE0_DBUS_VADDR_M  (EXTMEM_CORE0_DBUS_VADDR_V << EXTMEM_CORE0_DBUS_VADDR_S)
#define EXTMEM_CORE0_DBUS_VADDR_V  0xFFFFFFFFU
#define EXTMEM_CORE0_DBUS_VADDR_S  0

/** EXTMEM_CORE0_IBUS_REJECT_ST_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CORE0_IBUS_REJECT_ST_REG (DR_REG_EXTMEM_BASE + 0x98)
/** EXTMEM_CORE0_IBUS_ATTR : RO; bitpos: [2:0]; default: 0;
 *  The bits are used to indicate the attribute of CPU access ibus when authentication
 *  fail. 0: invalidate, 1: execute-able, 2: read-able
 */
#define EXTMEM_CORE0_IBUS_ATTR    0x00000007U
#define EXTMEM_CORE0_IBUS_ATTR_M  (EXTMEM_CORE0_IBUS_ATTR_V << EXTMEM_CORE0_IBUS_ATTR_S)
#define EXTMEM_CORE0_IBUS_ATTR_V  0x00000007U
#define EXTMEM_CORE0_IBUS_ATTR_S  0
/** EXTMEM_CORE0_IBUS_WORLD : RO; bitpos: [3]; default: 0;
 *  The bit is used to indicate the world of CPU access ibus when authentication fail.
 *  0: WORLD0, 1: WORLD1
 */
#define EXTMEM_CORE0_IBUS_WORLD    (BIT(3))
#define EXTMEM_CORE0_IBUS_WORLD_M  (EXTMEM_CORE0_IBUS_WORLD_V << EXTMEM_CORE0_IBUS_WORLD_S)
#define EXTMEM_CORE0_IBUS_WORLD_V  0x00000001U
#define EXTMEM_CORE0_IBUS_WORLD_S  3

/** EXTMEM_CORE0_IBUS_REJECT_VADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CORE0_IBUS_REJECT_VADDR_REG (DR_REG_EXTMEM_BASE + 0x9c)
/** EXTMEM_CORE0_IBUS_VADDR : RO; bitpos: [31:0]; default: 4294967295;
 *  The bits are used to indicate the virtual address of CPU access  ibus when
 *  authentication fail.
 */
#define EXTMEM_CORE0_IBUS_VADDR    0xFFFFFFFFU
#define EXTMEM_CORE0_IBUS_VADDR_M  (EXTMEM_CORE0_IBUS_VADDR_V << EXTMEM_CORE0_IBUS_VADDR_S)
#define EXTMEM_CORE0_IBUS_VADDR_V  0xFFFFFFFFU
#define EXTMEM_CORE0_IBUS_VADDR_S  0

/** EXTMEM_CACHE_MMU_FAULT_CONTENT_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_REG (DR_REG_EXTMEM_BASE + 0xa0)
/** EXTMEM_CACHE_MMU_FAULT_CONTENT : RO; bitpos: [7:0]; default: 0;
 *  The bits are used to indicate the content of mmu entry which cause mmu fault..
 */
#define EXTMEM_CACHE_MMU_FAULT_CONTENT    0x000000FFU
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_M  (EXTMEM_CACHE_MMU_FAULT_CONTENT_V << EXTMEM_CACHE_MMU_FAULT_CONTENT_S)
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_V  0x000000FFU
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_S  0
/** EXTMEM_CACHE_MMU_FAULT_CODE : RO; bitpos: [13:10]; default: 0;
 *  The right-most 3 bits are used to indicate the operations which cause mmu fault
 *  occurrence. 0: default, 1: cpu miss, 2: preload miss, 3: writeback, 4: cpu miss
 *  evict recovery address, 5: load miss evict recovery address, 6: external dma tx, 7:
 *  external dma rx. The most significant bit is used to indicate this operation occurs
 *  in which one icache.
 */
#define EXTMEM_CACHE_MMU_FAULT_CODE    0x0000000FU
#define EXTMEM_CACHE_MMU_FAULT_CODE_M  (EXTMEM_CACHE_MMU_FAULT_CODE_V << EXTMEM_CACHE_MMU_FAULT_CODE_S)
#define EXTMEM_CACHE_MMU_FAULT_CODE_V  0x0000000FU
#define EXTMEM_CACHE_MMU_FAULT_CODE_S  10

/** EXTMEM_CACHE_MMU_FAULT_VADDR_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_MMU_FAULT_VADDR_REG (DR_REG_EXTMEM_BASE + 0xa4)
/** EXTMEM_CACHE_MMU_FAULT_VADDR : RO; bitpos: [31:0]; default: 0;
 *  The bits are used to indicate the virtual address which cause mmu fault..
 */
#define EXTMEM_CACHE_MMU_FAULT_VADDR    0xFFFFFFFFU
#define EXTMEM_CACHE_MMU_FAULT_VADDR_M  (EXTMEM_CACHE_MMU_FAULT_VADDR_V << EXTMEM_CACHE_MMU_FAULT_VADDR_S)
#define EXTMEM_CACHE_MMU_FAULT_VADDR_V  0xFFFFFFFFU
#define EXTMEM_CACHE_MMU_FAULT_VADDR_S  0

/** EXTMEM_CACHE_WRAP_AROUND_CTRL_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_WRAP_AROUND_CTRL_REG (DR_REG_EXTMEM_BASE + 0xa8)
/** EXTMEM_CACHE_FLASH_WRAP_AROUND : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable wrap around mode when read data from flash.
 */
#define EXTMEM_CACHE_FLASH_WRAP_AROUND    (BIT(0))
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_M  (EXTMEM_CACHE_FLASH_WRAP_AROUND_V << EXTMEM_CACHE_FLASH_WRAP_AROUND_S)
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_V  0x00000001U
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_S  0

/** EXTMEM_CACHE_MMU_POWER_CTRL_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_MMU_POWER_CTRL_REG (DR_REG_EXTMEM_BASE + 0xac)
/** EXTMEM_CACHE_MMU_MEM_FORCE_ON : R/W; bitpos: [0]; default: 1;
 *  The bit is used to enable clock gating to save power when access mmu memory, 0:
 *  enable, 1: disable
 */
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON    (BIT(0))
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_M  (EXTMEM_CACHE_MMU_MEM_FORCE_ON_V << EXTMEM_CACHE_MMU_MEM_FORCE_ON_S)
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_V  0x00000001U
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_S  0
/** EXTMEM_CACHE_MMU_MEM_FORCE_PD : R/W; bitpos: [1]; default: 0;
 *  The bit is used to power mmu memory down, 0: follow_rtc_lslp_pd, 1: power down
 */
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD    (BIT(1))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_M  (EXTMEM_CACHE_MMU_MEM_FORCE_PD_V << EXTMEM_CACHE_MMU_MEM_FORCE_PD_S)
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_V  0x00000001U
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_S  1
/** EXTMEM_CACHE_MMU_MEM_FORCE_PU : R/W; bitpos: [2]; default: 1;
 *  The bit is used to power mmu memory down, 0: follow_rtc_lslp_pd, 1: power up
 */
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU    (BIT(2))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_M  (EXTMEM_CACHE_MMU_MEM_FORCE_PU_V << EXTMEM_CACHE_MMU_MEM_FORCE_PU_S)
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_V  0x00000001U
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_S  2

/** EXTMEM_CACHE_STATE_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_STATE_REG (DR_REG_EXTMEM_BASE + 0xb0)
/** EXTMEM_ICACHE_STATE : RO; bitpos: [11:0]; default: 1;
 *  The bit is used to indicate whether  icache main fsm is in idle state or not. 1: in
 *  idle state,  0: not in idle state
 */
#define EXTMEM_ICACHE_STATE    0x00000FFFU
#define EXTMEM_ICACHE_STATE_M  (EXTMEM_ICACHE_STATE_V << EXTMEM_ICACHE_STATE_S)
#define EXTMEM_ICACHE_STATE_V  0x00000FFFU
#define EXTMEM_ICACHE_STATE_S  0

/** EXTMEM_CACHE_ENCRYPT_DECRYPT_RECORD_DISABLE_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_ENCRYPT_DECRYPT_RECORD_DISABLE_REG (DR_REG_EXTMEM_BASE + 0xb4)
/** EXTMEM_RECORD_DISABLE_DB_ENCRYPT : R/W; bitpos: [0]; default: 0;
 *  Reserved.
 */
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT    (BIT(0))
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_M  (EXTMEM_RECORD_DISABLE_DB_ENCRYPT_V << EXTMEM_RECORD_DISABLE_DB_ENCRYPT_S)
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_V  0x00000001U
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_S  0
/** EXTMEM_RECORD_DISABLE_G0CB_DECRYPT : R/W; bitpos: [1]; default: 0;
 *  Reserved.
 */
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT    (BIT(1))
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_M  (EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_V << EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_S)
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_V  0x00000001U
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_S  1

/** EXTMEM_CACHE_ENCRYPT_DECRYPT_CLK_FORCE_ON_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_ENCRYPT_DECRYPT_CLK_FORCE_ON_REG (DR_REG_EXTMEM_BASE + 0xb8)
/** EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT : R/W; bitpos: [0]; default: 1;
 *  The bit is used to close clock gating of manual crypt clock. 1: close gating, 0:
 *  open clock gating.
 */
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT    (BIT(0))
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_M  (EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_V << EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_S)
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_V  0x00000001U
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_S  0
/** EXTMEM_CLK_FORCE_ON_AUTO_CRYPT : R/W; bitpos: [1]; default: 1;
 *  The bit is used to close clock gating of automatic crypt clock. 1: close gating, 0:
 *  open clock gating.
 */
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT    (BIT(1))
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_M  (EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_V << EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_S)
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_V  0x00000001U
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_S  1
/** EXTMEM_CLK_FORCE_ON_CRYPT : R/W; bitpos: [2]; default: 1;
 *  The bit is used to close clock gating of external memory encrypt and decrypt clock.
 *  1: close gating, 0: open clock gating.
 */
#define EXTMEM_CLK_FORCE_ON_CRYPT    (BIT(2))
#define EXTMEM_CLK_FORCE_ON_CRYPT_M  (EXTMEM_CLK_FORCE_ON_CRYPT_V << EXTMEM_CLK_FORCE_ON_CRYPT_S)
#define EXTMEM_CLK_FORCE_ON_CRYPT_V  0x00000001U
#define EXTMEM_CLK_FORCE_ON_CRYPT_S  2

/** EXTMEM_CACHE_PRELOAD_INT_CTRL_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_PRELOAD_INT_CTRL_REG (DR_REG_EXTMEM_BASE + 0xbc)
/** EXTMEM_ICACHE_PRELOAD_INT_ST : RO; bitpos: [0]; default: 0;
 *  The bit is used to indicate the interrupt by  icache pre-load done.
 */
#define EXTMEM_ICACHE_PRELOAD_INT_ST    (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_INT_ST_M  (EXTMEM_ICACHE_PRELOAD_INT_ST_V << EXTMEM_ICACHE_PRELOAD_INT_ST_S)
#define EXTMEM_ICACHE_PRELOAD_INT_ST_V  0x00000001U
#define EXTMEM_ICACHE_PRELOAD_INT_ST_S  0
/** EXTMEM_ICACHE_PRELOAD_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the interrupt by  icache pre-load done.
 */
#define EXTMEM_ICACHE_PRELOAD_INT_ENA    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_M  (EXTMEM_ICACHE_PRELOAD_INT_ENA_V << EXTMEM_ICACHE_PRELOAD_INT_ENA_S)
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_V  0x00000001U
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_S  1
/** EXTMEM_ICACHE_PRELOAD_INT_CLR : WOD; bitpos: [2]; default: 0;
 *  The bit is used to clear the interrupt by  icache pre-load done.
 */
#define EXTMEM_ICACHE_PRELOAD_INT_CLR    (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_M  (EXTMEM_ICACHE_PRELOAD_INT_CLR_V << EXTMEM_ICACHE_PRELOAD_INT_CLR_S)
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_V  0x00000001U
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_S  2

/** EXTMEM_CACHE_SYNC_INT_CTRL_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_SYNC_INT_CTRL_REG (DR_REG_EXTMEM_BASE + 0xc0)
/** EXTMEM_ICACHE_SYNC_INT_ST : RO; bitpos: [0]; default: 0;
 *  The bit is used to indicate the interrupt by  icache sync done.
 */
#define EXTMEM_ICACHE_SYNC_INT_ST    (BIT(0))
#define EXTMEM_ICACHE_SYNC_INT_ST_M  (EXTMEM_ICACHE_SYNC_INT_ST_V << EXTMEM_ICACHE_SYNC_INT_ST_S)
#define EXTMEM_ICACHE_SYNC_INT_ST_V  0x00000001U
#define EXTMEM_ICACHE_SYNC_INT_ST_S  0
/** EXTMEM_ICACHE_SYNC_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable the interrupt by  icache sync done.
 */
#define EXTMEM_ICACHE_SYNC_INT_ENA    (BIT(1))
#define EXTMEM_ICACHE_SYNC_INT_ENA_M  (EXTMEM_ICACHE_SYNC_INT_ENA_V << EXTMEM_ICACHE_SYNC_INT_ENA_S)
#define EXTMEM_ICACHE_SYNC_INT_ENA_V  0x00000001U
#define EXTMEM_ICACHE_SYNC_INT_ENA_S  1
/** EXTMEM_ICACHE_SYNC_INT_CLR : WOD; bitpos: [2]; default: 0;
 *  The bit is used to clear the interrupt by  icache sync done.
 */
#define EXTMEM_ICACHE_SYNC_INT_CLR    (BIT(2))
#define EXTMEM_ICACHE_SYNC_INT_CLR_M  (EXTMEM_ICACHE_SYNC_INT_CLR_V << EXTMEM_ICACHE_SYNC_INT_CLR_S)
#define EXTMEM_ICACHE_SYNC_INT_CLR_V  0x00000001U
#define EXTMEM_ICACHE_SYNC_INT_CLR_S  2

/** EXTMEM_CACHE_MMU_OWNER_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_MMU_OWNER_REG (DR_REG_EXTMEM_BASE + 0xc4)
/** EXTMEM_CACHE_MMU_OWNER : R/W; bitpos: [3:0]; default: 0;
 *  The bits are used to specify the owner of MMU.bit0/bit2: ibus, bit1/bit3: dbus
 */
#define EXTMEM_CACHE_MMU_OWNER    0x0000000FU
#define EXTMEM_CACHE_MMU_OWNER_M  (EXTMEM_CACHE_MMU_OWNER_V << EXTMEM_CACHE_MMU_OWNER_S)
#define EXTMEM_CACHE_MMU_OWNER_V  0x0000000FU
#define EXTMEM_CACHE_MMU_OWNER_S  0

/** EXTMEM_CACHE_CONF_MISC_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_CONF_MISC_REG (DR_REG_EXTMEM_BASE + 0xc8)
/** EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT : R/W; bitpos: [0]; default: 1;
 *  The bit is used to disable checking mmu entry fault by preload operation.
 */
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT    (BIT(0))
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_M  (EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_V << EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_S)
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_V  0x00000001U
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_S  0
/** EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT : R/W; bitpos: [1]; default: 1;
 *  The bit is used to disable checking mmu entry fault by sync operation.
 */
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT    (BIT(1))
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_M  (EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_V << EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_S)
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_V  0x00000001U
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_S  1
/** EXTMEM_CACHE_TRACE_ENA : R/W; bitpos: [2]; default: 1;
 *  The bit is used to enable cache trace function.
 */
#define EXTMEM_CACHE_TRACE_ENA    (BIT(2))
#define EXTMEM_CACHE_TRACE_ENA_M  (EXTMEM_CACHE_TRACE_ENA_V << EXTMEM_CACHE_TRACE_ENA_S)
#define EXTMEM_CACHE_TRACE_ENA_V  0x00000001U
#define EXTMEM_CACHE_TRACE_ENA_S  2
/** EXTMEM_CACHE_MMU_PAGE_SIZE : R/W; bitpos: [4:3]; default: 0;
 *  This bit is used to choose mmu page size. 2:64KB. 1. 32KB. 0: 16KB
 */
#define EXTMEM_CACHE_MMU_PAGE_SIZE    0x00000003U
#define EXTMEM_CACHE_MMU_PAGE_SIZE_M  (EXTMEM_CACHE_MMU_PAGE_SIZE_V << EXTMEM_CACHE_MMU_PAGE_SIZE_S)
#define EXTMEM_CACHE_MMU_PAGE_SIZE_V  0x00000003U
#define EXTMEM_CACHE_MMU_PAGE_SIZE_S  3

/** EXTMEM_ICACHE_FREEZE_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_FREEZE_REG (DR_REG_EXTMEM_BASE + 0xcc)
/** EXTMEM_ICACHE_FREEZE_ENA : R/W; bitpos: [0]; default: 0;
 *  The bit is used to enable icache freeze mode
 */
#define EXTMEM_ICACHE_FREEZE_ENA    (BIT(0))
#define EXTMEM_ICACHE_FREEZE_ENA_M  (EXTMEM_ICACHE_FREEZE_ENA_V << EXTMEM_ICACHE_FREEZE_ENA_S)
#define EXTMEM_ICACHE_FREEZE_ENA_V  0x00000001U
#define EXTMEM_ICACHE_FREEZE_ENA_S  0
/** EXTMEM_ICACHE_FREEZE_MODE : R/W; bitpos: [1]; default: 0;
 *  The bit is used to configure freeze mode, 0:  assert busy if CPU miss 1: assert hit
 *  if CPU miss
 */
#define EXTMEM_ICACHE_FREEZE_MODE    (BIT(1))
#define EXTMEM_ICACHE_FREEZE_MODE_M  (EXTMEM_ICACHE_FREEZE_MODE_V << EXTMEM_ICACHE_FREEZE_MODE_S)
#define EXTMEM_ICACHE_FREEZE_MODE_V  0x00000001U
#define EXTMEM_ICACHE_FREEZE_MODE_S  1
/** EXTMEM_ICACHE_FREEZE_DONE : RO; bitpos: [2]; default: 0;
 *  The bit is used to indicate icache freeze success
 */
#define EXTMEM_ICACHE_FREEZE_DONE    (BIT(2))
#define EXTMEM_ICACHE_FREEZE_DONE_M  (EXTMEM_ICACHE_FREEZE_DONE_V << EXTMEM_ICACHE_FREEZE_DONE_S)
#define EXTMEM_ICACHE_FREEZE_DONE_V  0x00000001U
#define EXTMEM_ICACHE_FREEZE_DONE_S  2

/** EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_REG (DR_REG_EXTMEM_BASE + 0xd0)
/** EXTMEM_ICACHE_ATOMIC_OPERATE_ENA : R/W; bitpos: [0]; default: 1;
 *  The bit is used to activate icache atomic operation protection. In this case,
 *  sync/lock operation can not interrupt miss-work. This feature does not work during
 *  invalidateAll operation.
 */
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA    (BIT(0))
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_M  (EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_V << EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_S)
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_V  0x00000001U
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_S  0

/** EXTMEM_CACHE_REQUEST_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CACHE_REQUEST_REG (DR_REG_EXTMEM_BASE + 0xd4)
/** EXTMEM_CACHE_REQUEST_BYPASS : R/W; bitpos: [0]; default: 0;
 *  The bit is used to disable request recording which could cause performance issue
 */
#define EXTMEM_CACHE_REQUEST_BYPASS    (BIT(0))
#define EXTMEM_CACHE_REQUEST_BYPASS_M  (EXTMEM_CACHE_REQUEST_BYPASS_V << EXTMEM_CACHE_REQUEST_BYPASS_S)
#define EXTMEM_CACHE_REQUEST_BYPASS_V  0x00000001U
#define EXTMEM_CACHE_REQUEST_BYPASS_S  0

/** EXTMEM_CLOCK_GATE_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_CLOCK_GATE_REG (DR_REG_EXTMEM_BASE + 0x100)
/** EXTMEM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  clock gate enable.
 */
#define EXTMEM_CLK_EN    (BIT(0))
#define EXTMEM_CLK_EN_M  (EXTMEM_CLK_EN_V << EXTMEM_CLK_EN_S)
#define EXTMEM_CLK_EN_V  0x00000001U
#define EXTMEM_CLK_EN_S  0

/** EXTMEM_REG_DATE_REG register
 *  This description will be updated in the near future.
 */
#define EXTMEM_REG_DATE_REG (DR_REG_EXTMEM_BASE + 0x3fc)
/** EXTMEM_DATE : R/W; bitpos: [27:0]; default: 34631760;
 *  version information
 */
#define EXTMEM_DATE    0x0FFFFFFFU
#define EXTMEM_DATE_M  (EXTMEM_DATE_V << EXTMEM_DATE_S)
#define EXTMEM_DATE_V  0x0FFFFFFFU
#define EXTMEM_DATE_S  0

#ifdef __cplusplus
}
#endif
