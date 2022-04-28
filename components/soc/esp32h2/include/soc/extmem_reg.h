// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_EXTMEM_REG_H_
#define _SOC_EXTMEM_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define EXTMEM_ICACHE_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x000)
/* EXTMEM_ICACHE_ENABLE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to activate the data cache. 0: disable  1: enable*/
#define EXTMEM_ICACHE_ENABLE  (BIT(0))
#define EXTMEM_ICACHE_ENABLE_M  (BIT(0))
#define EXTMEM_ICACHE_ENABLE_V  0x1
#define EXTMEM_ICACHE_ENABLE_S  0

#define EXTMEM_ICACHE_CTRL1_REG          (DR_REG_EXTMEM_BASE + 0x004)
/* EXTMEM_ICACHE_SHUT_DBUS : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to disable core1 ibus  0: enable  1: disable*/
#define EXTMEM_ICACHE_SHUT_DBUS  (BIT(1))
#define EXTMEM_ICACHE_SHUT_DBUS_M  (BIT(1))
#define EXTMEM_ICACHE_SHUT_DBUS_V  0x1
#define EXTMEM_ICACHE_SHUT_DBUS_S  1
/* EXTMEM_ICACHE_SHUT_IBUS : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to disable core0 ibus  0: enable  1: disable*/
#define EXTMEM_ICACHE_SHUT_IBUS  (BIT(0))
#define EXTMEM_ICACHE_SHUT_IBUS_M  (BIT(0))
#define EXTMEM_ICACHE_SHUT_IBUS_V  0x1
#define EXTMEM_ICACHE_SHUT_IBUS_S  0

#define EXTMEM_ICACHE_TAG_POWER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x008)
/* EXTMEM_ICACHE_TAG_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to power  icache tag memory up  0: follow rtc_lslp  1: power up*/
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU  (BIT(2))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_M  (BIT(2))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_V  0x1
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_S  2
/* EXTMEM_ICACHE_TAG_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to power  icache tag memory down  0: follow rtc_lslp
  1: power down*/
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD  (BIT(1))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_M  (BIT(1))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_V  0x1
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_S  1
/* EXTMEM_ICACHE_TAG_MEM_FORCE_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of  icache tag memory.
 1: close gating  0: open clock gating.*/
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON  (BIT(0))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_M  (BIT(0))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_V  0x1
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_S  0

#define EXTMEM_ICACHE_PRELOCK_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x00C)
/* EXTMEM_ICACHE_PRELOCK_SCT1_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: The bit is used to enable the second section of prelock function.*/
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN  (BIT(1))
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN_V  0x1
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN_S  1
/* EXTMEM_ICACHE_PRELOCK_SCT0_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to enable the first section of prelock function.*/
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN  (BIT(0))
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN_M  (BIT(0))
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN_V  0x1
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN_S  0

#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x010)
/* EXTMEM_ICACHE_PRELOCK_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to configure the first start virtual address
 of data prelock  which is combined with ICACHE_PRELOCK_SCT0_SIZE_REG*/
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_M  ((EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_V)<<(EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_S))
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_S  0

#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x014)
/* EXTMEM_ICACHE_PRELOCK_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to configure the second start virtual address
 of data prelock  which is combined with ICACHE_PRELOCK_SCT1_SIZE_REG*/
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_M  ((EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_V)<<(EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_S))
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_S  0

#define EXTMEM_ICACHE_PRELOCK_SCT_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x018)
/* EXTMEM_ICACHE_PRELOCK_SCT0_SIZE : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: The bits are used to configure the first length of data locking
  which is combined with ICACHE_PRELOCK_SCT0_ADDR_REG*/
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE  0x0000FFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_M  ((EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_V)<<(EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_S))
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_S  16
/* EXTMEM_ICACHE_PRELOCK_SCT1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the second length of data locking
  which is combined with ICACHE_PRELOCK_SCT1_ADDR_REG*/
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE  0x0000FFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_M  ((EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_V)<<(EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_S))
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_S  0

#define EXTMEM_ICACHE_LOCK_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x01C)
/* EXTMEM_ICACHE_LOCK_DONE : RO ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to indicate unlock/lock operation is finished.*/
#define EXTMEM_ICACHE_LOCK_DONE  (BIT(2))
#define EXTMEM_ICACHE_LOCK_DONE_M  (BIT(2))
#define EXTMEM_ICACHE_LOCK_DONE_V  0x1
#define EXTMEM_ICACHE_LOCK_DONE_S  2
/* EXTMEM_ICACHE_UNLOCK_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable unlock operation. It will be cleared
 by hardware after unlock operation done.*/
#define EXTMEM_ICACHE_UNLOCK_ENA  (BIT(1))
#define EXTMEM_ICACHE_UNLOCK_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_UNLOCK_ENA_V  0x1
#define EXTMEM_ICACHE_UNLOCK_ENA_S  1
/* EXTMEM_ICACHE_LOCK_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable lock operation. It will be cleared
 by hardware after lock operation done.*/
#define EXTMEM_ICACHE_LOCK_ENA  (BIT(0))
#define EXTMEM_ICACHE_LOCK_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_LOCK_ENA_V  0x1
#define EXTMEM_ICACHE_LOCK_ENA_S  0

#define EXTMEM_ICACHE_LOCK_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x020)
/* EXTMEM_ICACHE_LOCK_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for
 lock operations. It should be combined with ICACHE_LOCK_SIZE_REG.*/
#define EXTMEM_ICACHE_LOCK_ADDR  0xFFFFFFFF
#define EXTMEM_ICACHE_LOCK_ADDR_M  ((EXTMEM_ICACHE_LOCK_ADDR_V)<<(EXTMEM_ICACHE_LOCK_ADDR_S))
#define EXTMEM_ICACHE_LOCK_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_LOCK_ADDR_S  0

#define EXTMEM_ICACHE_LOCK_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x024)
/* EXTMEM_ICACHE_LOCK_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the length for lock operations.
 The bits are the counts of cache block. It should be combined with ICACHE_LOCK_ADDR_REG.*/
#define EXTMEM_ICACHE_LOCK_SIZE  0x0000FFFF
#define EXTMEM_ICACHE_LOCK_SIZE_M  ((EXTMEM_ICACHE_LOCK_SIZE_V)<<(EXTMEM_ICACHE_LOCK_SIZE_S))
#define EXTMEM_ICACHE_LOCK_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_LOCK_SIZE_S  0

#define EXTMEM_ICACHE_SYNC_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x028)
/* EXTMEM_ICACHE_SYNC_DONE : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to indicate invalidate operation is finished.*/
#define EXTMEM_ICACHE_SYNC_DONE  (BIT(1))
#define EXTMEM_ICACHE_SYNC_DONE_M  (BIT(1))
#define EXTMEM_ICACHE_SYNC_DONE_V  0x1
#define EXTMEM_ICACHE_SYNC_DONE_S  1
/* EXTMEM_ICACHE_INVALIDATE_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable invalidate operation. It will be cleared
 by hardware after invalidate operation done.*/
#define EXTMEM_ICACHE_INVALIDATE_ENA  (BIT(0))
#define EXTMEM_ICACHE_INVALIDATE_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_INVALIDATE_ENA_V  0x1
#define EXTMEM_ICACHE_INVALIDATE_ENA_S  0

#define EXTMEM_ICACHE_SYNC_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x02C)
/* EXTMEM_ICACHE_SYNC_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for
 clean operations. It should be combined with ICACHE_SYNC_SIZE_REG.*/
#define EXTMEM_ICACHE_SYNC_ADDR  0xFFFFFFFF
#define EXTMEM_ICACHE_SYNC_ADDR_M  ((EXTMEM_ICACHE_SYNC_ADDR_V)<<(EXTMEM_ICACHE_SYNC_ADDR_S))
#define EXTMEM_ICACHE_SYNC_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_SYNC_ADDR_S  0

#define EXTMEM_ICACHE_SYNC_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x030)
/* EXTMEM_ICACHE_SYNC_SIZE : R/W ;bitpos:[22:0] ;default: 23'h0 ; */
/*description: The bits are used to configure the length for sync operations.
 The bits are the counts of cache block. It should be combined with ICACHE_SYNC_ADDR_REG.*/
#define EXTMEM_ICACHE_SYNC_SIZE  0x007FFFFF
#define EXTMEM_ICACHE_SYNC_SIZE_M  ((EXTMEM_ICACHE_SYNC_SIZE_V)<<(EXTMEM_ICACHE_SYNC_SIZE_S))
#define EXTMEM_ICACHE_SYNC_SIZE_V  0x7FFFFF
#define EXTMEM_ICACHE_SYNC_SIZE_S  0

#define EXTMEM_ICACHE_PRELOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x034)
/* EXTMEM_ICACHE_PRELOAD_ORDER : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to configure the direction of preload operation.
 1: descending  0: ascending.*/
#define EXTMEM_ICACHE_PRELOAD_ORDER  (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_ORDER_M  (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_ORDER_V  0x1
#define EXTMEM_ICACHE_PRELOAD_ORDER_S  2
/* EXTMEM_ICACHE_PRELOAD_DONE : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to indicate preload operation is finished.*/
#define EXTMEM_ICACHE_PRELOAD_DONE  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_DONE_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_DONE_V  0x1
#define EXTMEM_ICACHE_PRELOAD_DONE_S  1
/* EXTMEM_ICACHE_PRELOAD_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable preload operation. It will be cleared
 by hardware after preload operation done.*/
#define EXTMEM_ICACHE_PRELOAD_ENA  (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_ENA_V  0x1
#define EXTMEM_ICACHE_PRELOAD_ENA_S  0

#define EXTMEM_ICACHE_PRELOAD_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x038)
/* EXTMEM_ICACHE_PRELOAD_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for
 preload operation. It should be combined with ICACHE_PRELOAD_SIZE_REG.*/
#define EXTMEM_ICACHE_PRELOAD_ADDR  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOAD_ADDR_M  ((EXTMEM_ICACHE_PRELOAD_ADDR_V)<<(EXTMEM_ICACHE_PRELOAD_ADDR_S))
#define EXTMEM_ICACHE_PRELOAD_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOAD_ADDR_S  0

#define EXTMEM_ICACHE_PRELOAD_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x03C)
/* EXTMEM_ICACHE_PRELOAD_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the length for preload operation.
 The bits are the counts of cache block. It should be combined with ICACHE_PRELOAD_ADDR_REG..*/
#define EXTMEM_ICACHE_PRELOAD_SIZE  0x0000FFFF
#define EXTMEM_ICACHE_PRELOAD_SIZE_M  ((EXTMEM_ICACHE_PRELOAD_SIZE_V)<<(EXTMEM_ICACHE_PRELOAD_SIZE_S))
#define EXTMEM_ICACHE_PRELOAD_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_PRELOAD_SIZE_S  0

#define EXTMEM_ICACHE_AUTOLOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x040)
/* EXTMEM_ICACHE_AUTOLOAD_RQST : R/W ;bitpos:[6:5] ;default: 2'b0 ; */
/*description: The bits are used to configure trigger conditions for autoload.
 0/3: cache miss  1: cache hit  2: both cache miss and hit.*/
#define EXTMEM_ICACHE_AUTOLOAD_RQST  0x00000003
#define EXTMEM_ICACHE_AUTOLOAD_RQST_M  ((EXTMEM_ICACHE_AUTOLOAD_RQST_V)<<(EXTMEM_ICACHE_AUTOLOAD_RQST_S))
#define EXTMEM_ICACHE_AUTOLOAD_RQST_V  0x3
#define EXTMEM_ICACHE_AUTOLOAD_RQST_S  5
/* EXTMEM_ICACHE_AUTOLOAD_ORDER : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bits are used to configure the direction of autoload. 1:
 descending  0: ascending.*/
#define EXTMEM_ICACHE_AUTOLOAD_ORDER  (BIT(4))
#define EXTMEM_ICACHE_AUTOLOAD_ORDER_M  (BIT(4))
#define EXTMEM_ICACHE_AUTOLOAD_ORDER_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_ORDER_S  4
/* EXTMEM_ICACHE_AUTOLOAD_DONE : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: The bit is used to indicate autoload operation is finished.*/
#define EXTMEM_ICACHE_AUTOLOAD_DONE  (BIT(3))
#define EXTMEM_ICACHE_AUTOLOAD_DONE_M  (BIT(3))
#define EXTMEM_ICACHE_AUTOLOAD_DONE_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_DONE_S  3
/* EXTMEM_ICACHE_AUTOLOAD_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable and disable autoload operation. It
 is combined with icache_autoload_done. 1: enable  0: disable.*/
#define EXTMEM_ICACHE_AUTOLOAD_ENA  (BIT(2))
#define EXTMEM_ICACHE_AUTOLOAD_ENA_M  (BIT(2))
#define EXTMEM_ICACHE_AUTOLOAD_ENA_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_ENA_S  2
/* EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bits are used to enable the second section for autoload operation.*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA  (BIT(1))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA_S  1
/* EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bits are used to enable the first section for autoload operation.*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA  (BIT(0))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x044)
/* EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address of the
 first section for autoload operation. It should be combined with icache_autoload_sct0_ena.*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR  0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x048)
/* EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE : R/W ;bitpos:[26:0] ;default: 27'h0 ; */
/*description: The bits are used to configure the length of the first section
 for autoload operation. It should be combined with icache_autoload_sct0_ena.*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE  0x07FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_V  0x7FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x04C)
/* EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address of the
 second section for autoload operation. It should be combined with icache_autoload_sct1_ena.*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR  0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x050)
/* EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE : R/W ;bitpos:[26:0] ;default: 27'h0 ; */
/*description: The bits are used to configure the length of the second section
 for autoload operation. It should be combined with icache_autoload_sct1_ena.*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE  0x07FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_V  0x7FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_S  0

#define EXTMEM_IBUS_TO_FLASH_START_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x054)
/* EXTMEM_IBUS_TO_FLASH_START_VADDR : R/W ;bitpos:[31:0] ;default: 32'h42000000 ; */
/*description: The bits are used to configure the start virtual address of ibus
 to access flash. The register is used to give constraints to ibus access counter.*/
#define EXTMEM_IBUS_TO_FLASH_START_VADDR  0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_M  ((EXTMEM_IBUS_TO_FLASH_START_VADDR_V)<<(EXTMEM_IBUS_TO_FLASH_START_VADDR_S))
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_V  0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_S  0

#define EXTMEM_IBUS_TO_FLASH_END_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x058)
/* EXTMEM_IBUS_TO_FLASH_END_VADDR : R/W ;bitpos:[31:0] ;default: 32'h427FFFFF ; */
/*description: The bits are used to configure the end virtual address of ibus
 to access flash. The register is used to give constraints to ibus access counter.*/
#define EXTMEM_IBUS_TO_FLASH_END_VADDR  0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_M  ((EXTMEM_IBUS_TO_FLASH_END_VADDR_V)<<(EXTMEM_IBUS_TO_FLASH_END_VADDR_S))
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_V  0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_S  0

#define EXTMEM_DBUS_TO_FLASH_START_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x05C)
/* EXTMEM_DBUS_TO_FLASH_START_VADDR : R/W ;bitpos:[31:0] ;default: 32'h3C000000 ; */
/*description: The bits are used to configure the start virtual address of dbus
 to access flash. The register is used to give constraints to dbus access counter.*/
#define EXTMEM_DBUS_TO_FLASH_START_VADDR  0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_M  ((EXTMEM_DBUS_TO_FLASH_START_VADDR_V)<<(EXTMEM_DBUS_TO_FLASH_START_VADDR_S))
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_V  0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_S  0

#define EXTMEM_DBUS_TO_FLASH_END_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x060)
/* EXTMEM_DBUS_TO_FLASH_END_VADDR : R/W ;bitpos:[31:0] ;default: 32'h3C7FFFFF ; */
/*description: The bits are used to configure the end virtual address of dbus
 to access flash. The register is used to give constraints to dbus access counter.*/
#define EXTMEM_DBUS_TO_FLASH_END_VADDR  0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_M  ((EXTMEM_DBUS_TO_FLASH_END_VADDR_V)<<(EXTMEM_DBUS_TO_FLASH_END_VADDR_S))
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_V  0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_S  0

#define EXTMEM_CACHE_ACS_CNT_CLR_REG          (DR_REG_EXTMEM_BASE + 0x064)
/* EXTMEM_DBUS_ACS_CNT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to clear dbus counter.*/
#define EXTMEM_DBUS_ACS_CNT_CLR  (BIT(1))
#define EXTMEM_DBUS_ACS_CNT_CLR_M  (BIT(1))
#define EXTMEM_DBUS_ACS_CNT_CLR_V  0x1
#define EXTMEM_DBUS_ACS_CNT_CLR_S  1
/* EXTMEM_IBUS_ACS_CNT_CLR : WOD ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to clear ibus counter.*/
#define EXTMEM_IBUS_ACS_CNT_CLR  (BIT(0))
#define EXTMEM_IBUS_ACS_CNT_CLR_M  (BIT(0))
#define EXTMEM_IBUS_ACS_CNT_CLR_V  0x1
#define EXTMEM_IBUS_ACS_CNT_CLR_S  0

#define EXTMEM_IBUS_ACS_MISS_CNT_REG          (DR_REG_EXTMEM_BASE + 0x068)
/* EXTMEM_IBUS_ACS_MISS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of the cache miss caused
 by ibus access flash.*/
#define EXTMEM_IBUS_ACS_MISS_CNT  0xFFFFFFFF
#define EXTMEM_IBUS_ACS_MISS_CNT_M  ((EXTMEM_IBUS_ACS_MISS_CNT_V)<<(EXTMEM_IBUS_ACS_MISS_CNT_S))
#define EXTMEM_IBUS_ACS_MISS_CNT_V  0xFFFFFFFF
#define EXTMEM_IBUS_ACS_MISS_CNT_S  0

#define EXTMEM_IBUS_ACS_CNT_REG          (DR_REG_EXTMEM_BASE + 0x06C)
/* EXTMEM_IBUS_ACS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of ibus access flash through icache.*/
#define EXTMEM_IBUS_ACS_CNT  0xFFFFFFFF
#define EXTMEM_IBUS_ACS_CNT_M  ((EXTMEM_IBUS_ACS_CNT_V)<<(EXTMEM_IBUS_ACS_CNT_S))
#define EXTMEM_IBUS_ACS_CNT_V  0xFFFFFFFF
#define EXTMEM_IBUS_ACS_CNT_S  0

#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_REG          (DR_REG_EXTMEM_BASE + 0x070)
/* EXTMEM_DBUS_ACS_FLASH_MISS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of the cache miss caused
 by dbus access flash.*/
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT  0xFFFFFFFF
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_M  ((EXTMEM_DBUS_ACS_FLASH_MISS_CNT_V)<<(EXTMEM_DBUS_ACS_FLASH_MISS_CNT_S))
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_S  0

#define EXTMEM_DBUS_ACS_CNT_REG          (DR_REG_EXTMEM_BASE + 0x074)
/* EXTMEM_DBUS_ACS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of dbus access flash through icache.*/
#define EXTMEM_DBUS_ACS_CNT  0xFFFFFFFF
#define EXTMEM_DBUS_ACS_CNT_M  ((EXTMEM_DBUS_ACS_CNT_V)<<(EXTMEM_DBUS_ACS_CNT_S))
#define EXTMEM_DBUS_ACS_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS_ACS_CNT_S  0

#define EXTMEM_CACHE_ILG_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0x078)
/* EXTMEM_DBUS_CNT_OVF_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by dbus counter overflow.*/
#define EXTMEM_DBUS_CNT_OVF_INT_ENA  (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_M  (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_V  0x1
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_S  8
/* EXTMEM_IBUS_CNT_OVF_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by ibus counter overflow.*/
#define EXTMEM_IBUS_CNT_OVF_INT_ENA  (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_M  (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_V  0x1
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_S  7
/* EXTMEM_MMU_ENTRY_FAULT_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by mmu entry fault.*/
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_M  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_V  0x1
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_S  5
/* EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by preload configurations fault.*/
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_V  0x1
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_S  1
/* EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by sync configurations fault.*/
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_V  0x1
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_S  0

#define EXTMEM_CACHE_ILG_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0x07C)
/* EXTMEM_DBUS_CNT_OVF_INT_CLR : WOD ;bitpos:[8] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by dbus counter overflow.*/
#define EXTMEM_DBUS_CNT_OVF_INT_CLR  (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_M  (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_V  0x1
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_S  8
/* EXTMEM_IBUS_CNT_OVF_INT_CLR : WOD ;bitpos:[7] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by ibus counter overflow.*/
#define EXTMEM_IBUS_CNT_OVF_INT_CLR  (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_M  (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_V  0x1
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_S  7
/* EXTMEM_MMU_ENTRY_FAULT_INT_CLR : WOD ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by mmu entry fault.*/
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_M  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_V  0x1
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_S  5
/* EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by preload configurations fault.*/
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_V  0x1
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_S  1
/* EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR : WOD ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by sync configurations fault.*/
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_V  0x1
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_S  0

#define EXTMEM_CACHE_ILG_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0x080)
/* EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dbus access flash miss
 counter overflow.*/
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST  (BIT(10))
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_M  (BIT(10))
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_V  0x1
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_S  10
/* EXTMEM_DBUS_ACS_CNT_OVF_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dbus access flash/spiram
 counter overflow.*/
#define EXTMEM_DBUS_ACS_CNT_OVF_ST  (BIT(9))
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_M  (BIT(9))
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_V  0x1
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_S  9
/* EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by ibus access flash/spiram
 miss counter overflow.*/
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST  (BIT(8))
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_M  (BIT(8))
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_V  0x1
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_S  8
/* EXTMEM_IBUS_ACS_CNT_OVF_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by ibus access flash/spiram
 counter overflow.*/
#define EXTMEM_IBUS_ACS_CNT_OVF_ST  (BIT(7))
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_M  (BIT(7))
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_V  0x1
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_S  7
/* EXTMEM_MMU_ENTRY_FAULT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by mmu entry fault.*/
#define EXTMEM_MMU_ENTRY_FAULT_ST  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_ST_M  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_ST_V  0x1
#define EXTMEM_MMU_ENTRY_FAULT_ST_S  5
/* EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by preload configurations fault.*/
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_V  0x1
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_S  1
/* EXTMEM_ICACHE_SYNC_OP_FAULT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by sync configurations fault.*/
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_V  0x1
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_S  0

#define EXTMEM_CORE0_ACS_CACHE_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0x084)
/* EXTMEM_CORE0_DBUS_WR_IC_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by dbus trying to write icache*/
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA  (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_M  (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_V  0x1
#define EXTMEM_CORE0_DBUS_WR_IC_INT_ENA_S  5
/* EXTMEM_CORE0_DBUS_REJECT_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by authentication fail.*/
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_M  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_V  0x1
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_S  4
/* EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by cpu access icache while
 the corresponding dbus is disabled which include speculative access.*/
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_M  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_V  0x1
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA_S  3
/* EXTMEM_CORE0_IBUS_REJECT_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by authentication fail.*/
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_M  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_V  0x1
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_S  2
/* EXTMEM_CORE0_IBUS_WR_IC_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by ibus trying to write icache*/
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_M  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_V  0x1
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_S  1
/* EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by cpu access icache while
 the corresponding ibus is disabled which include speculative access.*/
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_M  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_V  0x1
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_S  0

#define EXTMEM_CORE0_ACS_CACHE_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0x088)
/* EXTMEM_CORE0_DBUS_WR_IC_INT_CLR : WOD ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by dbus trying to write icache*/
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR  (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_M  (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_V  0x1
#define EXTMEM_CORE0_DBUS_WR_IC_INT_CLR_S  5
/* EXTMEM_CORE0_DBUS_REJECT_INT_CLR : WOD ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by authentication fail.*/
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_M  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_V  0x1
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_S  4
/* EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR : WOD ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by cpu access icache while
 the corresponding dbus is disabled or icache is disabled which include speculative access.*/
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_M  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_V  0x1
#define EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR_S  3
/* EXTMEM_CORE0_IBUS_REJECT_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by authentication fail.*/
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_M  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_V  0x1
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_S  2
/* EXTMEM_CORE0_IBUS_WR_IC_INT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by ibus trying to write icache*/
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_M  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_V  0x1
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_S  1
/* EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR : WOD ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by cpu access icache while
 the corresponding ibus is disabled or icache is disabled which include speculative access.*/
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_M  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_V  0x1
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_S  0

#define EXTMEM_CORE0_ACS_CACHE_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0x08C)
/* EXTMEM_CORE0_DBUS_WR_ICACHE_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dbus trying to write icache*/
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST  (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST_M  (BIT(5))
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST_V  0x1
#define EXTMEM_CORE0_DBUS_WR_ICACHE_ST_S  5
/* EXTMEM_CORE0_DBUS_REJECT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by authentication fail.*/
#define EXTMEM_CORE0_DBUS_REJECT_ST  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_ST_M  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_ST_V  0x1
#define EXTMEM_CORE0_DBUS_REJECT_ST_S  4
/* EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by cpu access icache while
 the core0_dbus is disabled or icache is disabled which include speculative access.*/
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_M  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_V  0x1
#define EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST_S  3
/* EXTMEM_CORE0_IBUS_REJECT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by authentication fail.*/
#define EXTMEM_CORE0_IBUS_REJECT_ST  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_ST_M  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_ST_V  0x1
#define EXTMEM_CORE0_IBUS_REJECT_ST_S  2
/* EXTMEM_CORE0_IBUS_WR_ICACHE_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by ibus trying to write icache*/
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_M  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_V  0x1
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_S  1
/* EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by cpu access  icache while
 the core0_ibus is disabled or icache is disabled which include speculative access.*/
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_M  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_V  0x1
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_S  0

#define EXTMEM_CORE0_DBUS_REJECT_ST_REG          (DR_REG_EXTMEM_BASE + 0x090)
/* EXTMEM_CORE0_DBUS_WORLD : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate the world of CPU access dbus when
 authentication fail. 0: WORLD0  1: WORLD1*/
#define EXTMEM_CORE0_DBUS_WORLD  (BIT(3))
#define EXTMEM_CORE0_DBUS_WORLD_M  (BIT(3))
#define EXTMEM_CORE0_DBUS_WORLD_V  0x1
#define EXTMEM_CORE0_DBUS_WORLD_S  3
/* EXTMEM_CORE0_DBUS_ATTR : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of CPU access dbus
 when authentication fail. 0: invalidate  1: execute-able  2: read-able  4: write-able.*/
#define EXTMEM_CORE0_DBUS_ATTR  0x00000007
#define EXTMEM_CORE0_DBUS_ATTR_M  ((EXTMEM_CORE0_DBUS_ATTR_V)<<(EXTMEM_CORE0_DBUS_ATTR_S))
#define EXTMEM_CORE0_DBUS_ATTR_V  0x7
#define EXTMEM_CORE0_DBUS_ATTR_S  0

#define EXTMEM_CORE0_DBUS_REJECT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x094)
/* EXTMEM_CORE0_DBUS_VADDR : RO ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: The bits are used to indicate the virtual address of CPU access
 dbus when authentication fail.*/
#define EXTMEM_CORE0_DBUS_VADDR  0xFFFFFFFF
#define EXTMEM_CORE0_DBUS_VADDR_M  ((EXTMEM_CORE0_DBUS_VADDR_V)<<(EXTMEM_CORE0_DBUS_VADDR_S))
#define EXTMEM_CORE0_DBUS_VADDR_V  0xFFFFFFFF
#define EXTMEM_CORE0_DBUS_VADDR_S  0

#define EXTMEM_CORE0_IBUS_REJECT_ST_REG          (DR_REG_EXTMEM_BASE + 0x098)
/* EXTMEM_CORE0_IBUS_WORLD : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate the world of CPU access ibus when
 authentication fail. 0: WORLD0  1: WORLD1*/
#define EXTMEM_CORE0_IBUS_WORLD  (BIT(3))
#define EXTMEM_CORE0_IBUS_WORLD_M  (BIT(3))
#define EXTMEM_CORE0_IBUS_WORLD_V  0x1
#define EXTMEM_CORE0_IBUS_WORLD_S  3
/* EXTMEM_CORE0_IBUS_ATTR : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of CPU access ibus
 when authentication fail. 0: invalidate  1: execute-able  2: read-able*/
#define EXTMEM_CORE0_IBUS_ATTR  0x00000007
#define EXTMEM_CORE0_IBUS_ATTR_M  ((EXTMEM_CORE0_IBUS_ATTR_V)<<(EXTMEM_CORE0_IBUS_ATTR_S))
#define EXTMEM_CORE0_IBUS_ATTR_V  0x7
#define EXTMEM_CORE0_IBUS_ATTR_S  0

#define EXTMEM_CORE0_IBUS_REJECT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x09C)
/* EXTMEM_CORE0_IBUS_VADDR : RO ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: The bits are used to indicate the virtual address of CPU access
  ibus when authentication fail.*/
#define EXTMEM_CORE0_IBUS_VADDR  0xFFFFFFFF
#define EXTMEM_CORE0_IBUS_VADDR_M  ((EXTMEM_CORE0_IBUS_VADDR_V)<<(EXTMEM_CORE0_IBUS_VADDR_S))
#define EXTMEM_CORE0_IBUS_VADDR_V  0xFFFFFFFF
#define EXTMEM_CORE0_IBUS_VADDR_S  0

#define EXTMEM_CACHE_MMU_FAULT_CONTENT_REG          (DR_REG_EXTMEM_BASE + 0x0A0)
/* EXTMEM_CACHE_MMU_FAULT_CODE : RO ;bitpos:[13:10] ;default: 4'h0 ; */
/*description: The right-most 3 bits are used to indicate the operations which
 cause mmu fault occurrence. 0: default  1: cpu miss  2: preload miss  3: writeback  4: cpu miss evict recovery address  5: load miss evict recovery address  6: external dma tx  7: external dma rx. The most significant bit is used to indicate this operation occurs in which one icache.*/
#define EXTMEM_CACHE_MMU_FAULT_CODE  0x0000000F
#define EXTMEM_CACHE_MMU_FAULT_CODE_M  ((EXTMEM_CACHE_MMU_FAULT_CODE_V)<<(EXTMEM_CACHE_MMU_FAULT_CODE_S))
#define EXTMEM_CACHE_MMU_FAULT_CODE_V  0xF
#define EXTMEM_CACHE_MMU_FAULT_CODE_S  10
/* EXTMEM_CACHE_MMU_FAULT_CONTENT : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The bits are used to indicate the content of mmu entry which cause mmu fault..*/
#define EXTMEM_CACHE_MMU_FAULT_CONTENT  0x000003FF
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_M  ((EXTMEM_CACHE_MMU_FAULT_CONTENT_V)<<(EXTMEM_CACHE_MMU_FAULT_CONTENT_S))
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_V  0x3FF
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_S  0

#define EXTMEM_CACHE_MMU_FAULT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x0A4)
/* EXTMEM_CACHE_MMU_FAULT_VADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to indicate the virtual address which cause mmu fault..*/
#define EXTMEM_CACHE_MMU_FAULT_VADDR  0xFFFFFFFF
#define EXTMEM_CACHE_MMU_FAULT_VADDR_M  ((EXTMEM_CACHE_MMU_FAULT_VADDR_V)<<(EXTMEM_CACHE_MMU_FAULT_VADDR_S))
#define EXTMEM_CACHE_MMU_FAULT_VADDR_V  0xFFFFFFFF
#define EXTMEM_CACHE_MMU_FAULT_VADDR_S  0

#define EXTMEM_CACHE_WRAP_AROUND_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x0A8)
/* EXTMEM_CACHE_FLASH_WRAP_AROUND : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable wrap around mode when read data from flash.*/
#define EXTMEM_CACHE_FLASH_WRAP_AROUND  (BIT(0))
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_M  (BIT(0))
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_V  0x1
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_S  0

#define EXTMEM_CACHE_MMU_POWER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x0AC)
/* EXTMEM_CACHE_MMU_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to power mmu memory down  0: follow_rtc_lslp_pd  1: power up*/
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU  (BIT(2))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_M  (BIT(2))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_V  0x1
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_S  2
/* EXTMEM_CACHE_MMU_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to power mmu memory down  0: follow_rtc_lslp_pd  1: power down*/
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD  (BIT(1))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_M  (BIT(1))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_V  0x1
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_S  1
/* EXTMEM_CACHE_MMU_MEM_FORCE_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable clock gating to save power when access
 mmu memory  0: enable  1: disable*/
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON  (BIT(0))
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_M  (BIT(0))
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_V  0x1
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_S  0

#define EXTMEM_CACHE_STATE_REG          (DR_REG_EXTMEM_BASE + 0x0B0)
/* EXTMEM_ICACHE_STATE : RO ;bitpos:[11:0] ;default: 12'h1 ; */
/*description: The bit is used to indicate whether  icache main fsm is in idle
 state or not. 1: in idle state   0: not in idle state*/
#define EXTMEM_ICACHE_STATE  0x00000FFF
#define EXTMEM_ICACHE_STATE_M  ((EXTMEM_ICACHE_STATE_V)<<(EXTMEM_ICACHE_STATE_S))
#define EXTMEM_ICACHE_STATE_V  0xFFF
#define EXTMEM_ICACHE_STATE_S  0

#define EXTMEM_CACHE_ENCRYPT_DECRYPT_RECORD_DISABLE_REG          (DR_REG_EXTMEM_BASE + 0x0B4)
/* EXTMEM_RECORD_DISABLE_G0CB_DECRYPT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Reserved.*/
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT  (BIT(1))
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_M  (BIT(1))
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_V  0x1
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_S  1
/* EXTMEM_RECORD_DISABLE_DB_ENCRYPT : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Reserved.*/
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT  (BIT(0))
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_M  (BIT(0))
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_V  0x1
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_S  0

#define EXTMEM_CACHE_ENCRYPT_DECRYPT_CLK_FORCE_ON_REG          (DR_REG_EXTMEM_BASE + 0x0B8)
/* EXTMEM_CLK_FORCE_ON_CRYPT : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of external memory encrypt
 and decrypt clock. 1: close gating  0: open clock gating.*/
#define EXTMEM_CLK_FORCE_ON_CRYPT  (BIT(2))
#define EXTMEM_CLK_FORCE_ON_CRYPT_M  (BIT(2))
#define EXTMEM_CLK_FORCE_ON_CRYPT_V  0x1
#define EXTMEM_CLK_FORCE_ON_CRYPT_S  2
/* EXTMEM_CLK_FORCE_ON_AUTO_CRYPT : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of automatic crypt clock.
 1: close gating  0: open clock gating.*/
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT  (BIT(1))
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_M  (BIT(1))
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_V  0x1
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_S  1
/* EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of manual crypt clock.
 1: close gating  0: open clock gating.*/
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT  (BIT(0))
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_M  (BIT(0))
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_V  0x1
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_S  0

#define EXTMEM_CACHE_PRELOAD_INT_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x0BC)
/* EXTMEM_ICACHE_PRELOAD_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear the interrupt by  icache pre-load done.*/
#define EXTMEM_ICACHE_PRELOAD_INT_CLR  (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_M  (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_V  0x1
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_S  2
/* EXTMEM_ICACHE_PRELOAD_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt by  icache pre-load done.*/
#define EXTMEM_ICACHE_PRELOAD_INT_ENA  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_V  0x1
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_S  1
/* EXTMEM_ICACHE_PRELOAD_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate the interrupt by  icache pre-load done.*/
#define EXTMEM_ICACHE_PRELOAD_INT_ST  (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_INT_ST_M  (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_INT_ST_V  0x1
#define EXTMEM_ICACHE_PRELOAD_INT_ST_S  0

#define EXTMEM_CACHE_SYNC_INT_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x0C0)
/* EXTMEM_ICACHE_SYNC_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear the interrupt by  icache sync done.*/
#define EXTMEM_ICACHE_SYNC_INT_CLR  (BIT(2))
#define EXTMEM_ICACHE_SYNC_INT_CLR_M  (BIT(2))
#define EXTMEM_ICACHE_SYNC_INT_CLR_V  0x1
#define EXTMEM_ICACHE_SYNC_INT_CLR_S  2
/* EXTMEM_ICACHE_SYNC_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt by  icache sync done.*/
#define EXTMEM_ICACHE_SYNC_INT_ENA  (BIT(1))
#define EXTMEM_ICACHE_SYNC_INT_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_SYNC_INT_ENA_V  0x1
#define EXTMEM_ICACHE_SYNC_INT_ENA_S  1
/* EXTMEM_ICACHE_SYNC_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate the interrupt by  icache sync done.*/
#define EXTMEM_ICACHE_SYNC_INT_ST  (BIT(0))
#define EXTMEM_ICACHE_SYNC_INT_ST_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_INT_ST_V  0x1
#define EXTMEM_ICACHE_SYNC_INT_ST_S  0

#define EXTMEM_CACHE_MMU_OWNER_REG          (DR_REG_EXTMEM_BASE + 0x0C4)
/* EXTMEM_CACHE_MMU_OWNER : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: The bits are used to specify the owner of MMU.bit0/bit2: ibus  bit1/bit3: dbus*/
#define EXTMEM_CACHE_MMU_OWNER  0x0000000F
#define EXTMEM_CACHE_MMU_OWNER_M  ((EXTMEM_CACHE_MMU_OWNER_V)<<(EXTMEM_CACHE_MMU_OWNER_S))
#define EXTMEM_CACHE_MMU_OWNER_V  0xF
#define EXTMEM_CACHE_MMU_OWNER_S  0

#define EXTMEM_CACHE_CONF_MISC_REG          (DR_REG_EXTMEM_BASE + 0x0C8)
/* EXTMEM_CACHE_TRACE_ENA : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to enable cache trace function.*/
#define EXTMEM_CACHE_TRACE_ENA  (BIT(2))
#define EXTMEM_CACHE_TRACE_ENA_M  (BIT(2))
#define EXTMEM_CACHE_TRACE_ENA_V  0x1
#define EXTMEM_CACHE_TRACE_ENA_S  2
/* EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to disable checking mmu entry fault by sync operation.*/
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT  (BIT(1))
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_M  (BIT(1))
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_V  0x1
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_S  1
/* EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to disable checking mmu entry fault by preload operation.*/
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT  (BIT(0))
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_M  (BIT(0))
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_V  0x1
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_S  0

#define EXTMEM_ICACHE_FREEZE_REG          (DR_REG_EXTMEM_BASE + 0x0CC)
/* EXTMEM_ICACHE_FREEZE_DONE : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to indicate icache freeze success*/
#define EXTMEM_ICACHE_FREEZE_DONE  (BIT(2))
#define EXTMEM_ICACHE_FREEZE_DONE_M  (BIT(2))
#define EXTMEM_ICACHE_FREEZE_DONE_V  0x1
#define EXTMEM_ICACHE_FREEZE_DONE_S  2
/* EXTMEM_ICACHE_FREEZE_MODE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to configure freeze mode  0:  assert busy if
 CPU miss 1: assert hit if CPU miss*/
#define EXTMEM_ICACHE_FREEZE_MODE  (BIT(1))
#define EXTMEM_ICACHE_FREEZE_MODE_M  (BIT(1))
#define EXTMEM_ICACHE_FREEZE_MODE_V  0x1
#define EXTMEM_ICACHE_FREEZE_MODE_S  1
/* EXTMEM_ICACHE_FREEZE_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable icache freeze mode*/
#define EXTMEM_ICACHE_FREEZE_ENA  (BIT(0))
#define EXTMEM_ICACHE_FREEZE_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_FREEZE_ENA_V  0x1
#define EXTMEM_ICACHE_FREEZE_ENA_S  0

#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_REG          (DR_REG_EXTMEM_BASE + 0x0D0)
/* EXTMEM_ICACHE_ATOMIC_OPERATE_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to activate icache atomic operation protection.
 In this case  sync/lock operation can not interrupt miss-work. This feature does not work during invalidateAll operation.*/
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA  (BIT(0))
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_V  0x1
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_S  0

#define EXTMEM_CACHE_REQUEST_REG          (DR_REG_EXTMEM_BASE + 0x0D4)
/* EXTMEM_CACHE_REQUEST_BYPASS : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to disable request recording which could cause performance issue*/
#define EXTMEM_CACHE_REQUEST_BYPASS  (BIT(0))
#define EXTMEM_CACHE_REQUEST_BYPASS_M  (BIT(0))
#define EXTMEM_CACHE_REQUEST_BYPASS_V  0x1
#define EXTMEM_CACHE_REQUEST_BYPASS_S  0

#define EXTMEM_IBUS_PMS_TBL_LOCK_REG          (DR_REG_EXTMEM_BASE + 0x0D8)
/* EXTMEM_IBUS_PMS_LOCK : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to configure the ibus permission control section boundary0*/
#define EXTMEM_IBUS_PMS_LOCK  (BIT(0))
#define EXTMEM_IBUS_PMS_LOCK_M  (BIT(0))
#define EXTMEM_IBUS_PMS_LOCK_V  0x1
#define EXTMEM_IBUS_PMS_LOCK_S  0

#define EXTMEM_IBUS_PMS_TBL_BOUNDARY0_REG          (DR_REG_EXTMEM_BASE + 0x0DC)
/* EXTMEM_IBUS_PMS_BOUNDARY0 : R/W ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: The bit is used to configure the ibus permission control section boundary0*/
#define EXTMEM_IBUS_PMS_BOUNDARY0  0x00000FFF
#define EXTMEM_IBUS_PMS_BOUNDARY0_M  ((EXTMEM_IBUS_PMS_BOUNDARY0_V)<<(EXTMEM_IBUS_PMS_BOUNDARY0_S))
#define EXTMEM_IBUS_PMS_BOUNDARY0_V  0xFFF
#define EXTMEM_IBUS_PMS_BOUNDARY0_S  0

#define EXTMEM_IBUS_PMS_TBL_BOUNDARY1_REG          (DR_REG_EXTMEM_BASE + 0x0E0)
/* EXTMEM_IBUS_PMS_BOUNDARY1 : R/W ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: The bit is used to configure the ibus permission control section boundary1*/
#define EXTMEM_IBUS_PMS_BOUNDARY1  0x00000FFF
#define EXTMEM_IBUS_PMS_BOUNDARY1_M  ((EXTMEM_IBUS_PMS_BOUNDARY1_V)<<(EXTMEM_IBUS_PMS_BOUNDARY1_S))
#define EXTMEM_IBUS_PMS_BOUNDARY1_V  0xFFF
#define EXTMEM_IBUS_PMS_BOUNDARY1_S  0

#define EXTMEM_IBUS_PMS_TBL_BOUNDARY2_REG          (DR_REG_EXTMEM_BASE + 0x0E4)
/* EXTMEM_IBUS_PMS_BOUNDARY2 : R/W ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: The bit is used to configure the ibus permission control section boundary2*/
#define EXTMEM_IBUS_PMS_BOUNDARY2  0x00000FFF
#define EXTMEM_IBUS_PMS_BOUNDARY2_M  ((EXTMEM_IBUS_PMS_BOUNDARY2_V)<<(EXTMEM_IBUS_PMS_BOUNDARY2_S))
#define EXTMEM_IBUS_PMS_BOUNDARY2_V  0xFFF
#define EXTMEM_IBUS_PMS_BOUNDARY2_S  0

#define EXTMEM_IBUS_PMS_TBL_ATTR_REG          (DR_REG_EXTMEM_BASE + 0x0E8)
/* EXTMEM_IBUS_PMS_SCT2_ATTR : R/W ;bitpos:[7:4] ;default: 4'hF ; */
/*description: The bit is used to configure attribute of the ibus permission
 control section2  bit0: fetch in world0  bit1: load in world0  bit2: fetch in world1  bit3: load in world1*/
#define EXTMEM_IBUS_PMS_SCT2_ATTR  0x0000000F
#define EXTMEM_IBUS_PMS_SCT2_ATTR_M  ((EXTMEM_IBUS_PMS_SCT2_ATTR_V)<<(EXTMEM_IBUS_PMS_SCT2_ATTR_S))
#define EXTMEM_IBUS_PMS_SCT2_ATTR_V  0xF
#define EXTMEM_IBUS_PMS_SCT2_ATTR_S  4
/* EXTMEM_IBUS_PMS_SCT1_ATTR : R/W ;bitpos:[3:0] ;default: 4'hF ; */
/*description: The bit is used to configure attribute of the ibus permission
 control section1  bit0: fetch in world0  bit1: load in world0  bit2: fetch in world1  bit3: load in world1*/
#define EXTMEM_IBUS_PMS_SCT1_ATTR  0x0000000F
#define EXTMEM_IBUS_PMS_SCT1_ATTR_M  ((EXTMEM_IBUS_PMS_SCT1_ATTR_V)<<(EXTMEM_IBUS_PMS_SCT1_ATTR_S))
#define EXTMEM_IBUS_PMS_SCT1_ATTR_V  0xF
#define EXTMEM_IBUS_PMS_SCT1_ATTR_S  0

#define EXTMEM_DBUS_PMS_TBL_LOCK_REG          (DR_REG_EXTMEM_BASE + 0x0EC)
/* EXTMEM_DBUS_PMS_LOCK : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to configure the ibus permission control section boundary0*/
#define EXTMEM_DBUS_PMS_LOCK  (BIT(0))
#define EXTMEM_DBUS_PMS_LOCK_M  (BIT(0))
#define EXTMEM_DBUS_PMS_LOCK_V  0x1
#define EXTMEM_DBUS_PMS_LOCK_S  0

#define EXTMEM_DBUS_PMS_TBL_BOUNDARY0_REG          (DR_REG_EXTMEM_BASE + 0x0F0)
/* EXTMEM_DBUS_PMS_BOUNDARY0 : R/W ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: The bit is used to configure the dbus permission control section boundary0*/
#define EXTMEM_DBUS_PMS_BOUNDARY0  0x00000FFF
#define EXTMEM_DBUS_PMS_BOUNDARY0_M  ((EXTMEM_DBUS_PMS_BOUNDARY0_V)<<(EXTMEM_DBUS_PMS_BOUNDARY0_S))
#define EXTMEM_DBUS_PMS_BOUNDARY0_V  0xFFF
#define EXTMEM_DBUS_PMS_BOUNDARY0_S  0

#define EXTMEM_DBUS_PMS_TBL_BOUNDARY1_REG          (DR_REG_EXTMEM_BASE + 0x0F4)
/* EXTMEM_DBUS_PMS_BOUNDARY1 : R/W ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: The bit is used to configure the dbus permission control section boundary1*/
#define EXTMEM_DBUS_PMS_BOUNDARY1  0x00000FFF
#define EXTMEM_DBUS_PMS_BOUNDARY1_M  ((EXTMEM_DBUS_PMS_BOUNDARY1_V)<<(EXTMEM_DBUS_PMS_BOUNDARY1_S))
#define EXTMEM_DBUS_PMS_BOUNDARY1_V  0xFFF
#define EXTMEM_DBUS_PMS_BOUNDARY1_S  0

#define EXTMEM_DBUS_PMS_TBL_BOUNDARY2_REG          (DR_REG_EXTMEM_BASE + 0x0F8)
/* EXTMEM_DBUS_PMS_BOUNDARY2 : R/W ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: The bit is used to configure the dbus permission control section boundary2*/
#define EXTMEM_DBUS_PMS_BOUNDARY2  0x00000FFF
#define EXTMEM_DBUS_PMS_BOUNDARY2_M  ((EXTMEM_DBUS_PMS_BOUNDARY2_V)<<(EXTMEM_DBUS_PMS_BOUNDARY2_S))
#define EXTMEM_DBUS_PMS_BOUNDARY2_V  0xFFF
#define EXTMEM_DBUS_PMS_BOUNDARY2_S  0

#define EXTMEM_DBUS_PMS_TBL_ATTR_REG          (DR_REG_EXTMEM_BASE + 0x0FC)
/* EXTMEM_DBUS_PMS_SCT2_ATTR : R/W ;bitpos:[3:2] ;default: 2'd3 ; */
/*description: The bit is used to configure attribute of the dbus permission
 control section2  bit0: load in world0  bit2: load in world1*/
#define EXTMEM_DBUS_PMS_SCT2_ATTR  0x00000003
#define EXTMEM_DBUS_PMS_SCT2_ATTR_M  ((EXTMEM_DBUS_PMS_SCT2_ATTR_V)<<(EXTMEM_DBUS_PMS_SCT2_ATTR_S))
#define EXTMEM_DBUS_PMS_SCT2_ATTR_V  0x3
#define EXTMEM_DBUS_PMS_SCT2_ATTR_S  2
/* EXTMEM_DBUS_PMS_SCT1_ATTR : R/W ;bitpos:[1:0] ;default: 2'd3 ; */
/*description: The bit is used to configure attribute of the dbus permission
 control section1  bit0: load in world0  bit2: load in world1*/
#define EXTMEM_DBUS_PMS_SCT1_ATTR  0x00000003
#define EXTMEM_DBUS_PMS_SCT1_ATTR_M  ((EXTMEM_DBUS_PMS_SCT1_ATTR_V)<<(EXTMEM_DBUS_PMS_SCT1_ATTR_S))
#define EXTMEM_DBUS_PMS_SCT1_ATTR_V  0x3
#define EXTMEM_DBUS_PMS_SCT1_ATTR_S  0

#define EXTMEM_CLOCK_GATE_REG          (DR_REG_EXTMEM_BASE + 0x100)
/* EXTMEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: Reserved.*/
#define EXTMEM_CLK_EN  (BIT(0))
#define EXTMEM_CLK_EN_M  (BIT(0))
#define EXTMEM_CLK_EN_V  0x1
#define EXTMEM_CLK_EN_S  0

#define EXTMEM_DATE_REG          (DR_REG_EXTMEM_BASE + 0x3FC)
/* EXTMEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h2007160 ; */
/*description: Reserved.*/
#define EXTMEM_DATE  0x0FFFFFFF
#define EXTMEM_DATE_M  ((EXTMEM_DATE_V)<<(EXTMEM_DATE_S))
#define EXTMEM_DATE_V  0xFFFFFFF
#define EXTMEM_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_EXTMEM_REG_H_ */
