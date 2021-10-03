// Copyright 2017-2021 Espressif Systems (Shanghai) PTE LTD
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


#include "soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define EXTMEM_DCACHE_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x0)
/* EXTMEM_DCACHE_BLOCKSIZE_MODE : R/W ;bitpos:[4:3] ;default: 2'b0 ; */
/*description: The bit is used to configure cache block size.0: 16 bytes, 1: 32 bytes,2: 64 byt
es.*/
#define EXTMEM_DCACHE_BLOCKSIZE_MODE    0x00000003
#define EXTMEM_DCACHE_BLOCKSIZE_MODE_M  ((EXTMEM_DCACHE_BLOCKSIZE_MODE_V)<<(EXTMEM_DCACHE_BLOCKSIZE_MODE_S))
#define EXTMEM_DCACHE_BLOCKSIZE_MODE_V  0x3
#define EXTMEM_DCACHE_BLOCKSIZE_MODE_S  3
/* EXTMEM_DCACHE_SIZE_MODE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to configure cache memory size.0: 32KB, 1: 64KB.*/
#define EXTMEM_DCACHE_SIZE_MODE    (BIT(2))
#define EXTMEM_DCACHE_SIZE_MODE_M  (BIT(2))
#define EXTMEM_DCACHE_SIZE_MODE_V  0x1
#define EXTMEM_DCACHE_SIZE_MODE_S  2
/* EXTMEM_DCACHE_ENABLE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to activate the data cache. 0: disable, 1: enable.*/
#define EXTMEM_DCACHE_ENABLE    (BIT(0))
#define EXTMEM_DCACHE_ENABLE_M  (BIT(0))
#define EXTMEM_DCACHE_ENABLE_V  0x1
#define EXTMEM_DCACHE_ENABLE_S  0

#define EXTMEM_DCACHE_CTRL1_REG          (DR_REG_EXTMEM_BASE + 0x4)
/* EXTMEM_DCACHE_SHUT_CORE1_BUS : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to disable core1 dbus, 0: enable, 1: disable.*/
#define EXTMEM_DCACHE_SHUT_CORE1_BUS    (BIT(1))
#define EXTMEM_DCACHE_SHUT_CORE1_BUS_M  (BIT(1))
#define EXTMEM_DCACHE_SHUT_CORE1_BUS_V  0x1
#define EXTMEM_DCACHE_SHUT_CORE1_BUS_S  1
/* EXTMEM_DCACHE_SHUT_CORE0_BUS : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to disable core0 dbus, 0: enable, 1: disable.*/
#define EXTMEM_DCACHE_SHUT_CORE0_BUS    (BIT(0))
#define EXTMEM_DCACHE_SHUT_CORE0_BUS_M  (BIT(0))
#define EXTMEM_DCACHE_SHUT_CORE0_BUS_V  0x1
#define EXTMEM_DCACHE_SHUT_CORE0_BUS_S  0

#define EXTMEM_DCACHE_TAG_POWER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x8)
/* EXTMEM_DCACHE_TAG_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to power dcache tag memory up, 0: follow  rtc_lslp_pd, 1: power
up.*/
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU    (BIT(2))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU_M  (BIT(2))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PU_S  2
/* EXTMEM_DCACHE_TAG_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to power dcache tag memory down, 0: follow  rtc_lslp_pd, 1: powe
r down.*/
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD    (BIT(1))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD_M  (BIT(1))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_FORCE_PD_S  1
/* EXTMEM_DCACHE_TAG_MEM_FORCE_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of dcache tag memory. 1: close gating, 0:
open clock gating..*/
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON    (BIT(0))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON_M  (BIT(0))
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON_V  0x1
#define EXTMEM_DCACHE_TAG_MEM_FORCE_ON_S  0

#define EXTMEM_DCACHE_PRELOCK_CTRL_REG          (DR_REG_EXTMEM_BASE + 0xC)
/* EXTMEM_DCACHE_PRELOCK_SCT1_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: The bit is used to enable the second section of prelock function..*/
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN    (BIT(1))
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN_M  (BIT(1))
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN_V  0x1
#define EXTMEM_DCACHE_PRELOCK_SCT1_EN_S  1
/* EXTMEM_DCACHE_PRELOCK_SCT0_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to enable the first section of prelock function..*/
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN    (BIT(0))
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN_M  (BIT(0))
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN_V  0x1
#define EXTMEM_DCACHE_PRELOCK_SCT0_EN_S  0

#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x10)
/* EXTMEM_DCACHE_PRELOCK_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to configure the first start virtual address of data prelock,
which is combined with DCACHE_PRELOCK_SCT0_SIZE_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_M  ((EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_V)<<(EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_S))
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_ADDR_S  0

#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x14)
/* EXTMEM_DCACHE_PRELOCK_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to configure the second start virtual address of data prelock,
 which is combined with DCACHE_PRELOCK_SCT1_SIZE_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_M  ((EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_V)<<(EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_S))
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_ADDR_S  0

#define EXTMEM_DCACHE_PRELOCK_SCT_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x18)
/* EXTMEM_DCACHE_PRELOCK_SCT0_SIZE : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: The bits are used to configure the first length of data locking, which is combin
ed with DCACHE_PRELOCK_SCT0_ADDR_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE    0x0000FFFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_M  ((EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_V)<<(EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_S))
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_V  0xFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT0_SIZE_S  16
/* EXTMEM_DCACHE_PRELOCK_SCT1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the second length of data locking, which is combi
ned with DCACHE_PRELOCK_SCT1_ADDR_REG.*/
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE    0x0000FFFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_M  ((EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_V)<<(EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_S))
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_V  0xFFFF
#define EXTMEM_DCACHE_PRELOCK_SCT1_SIZE_S  0

#define EXTMEM_DCACHE_LOCK_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x1C)
/* EXTMEM_DCACHE_LOCK_DONE : RO ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to indicate unlock/lock operation is finished..*/
#define EXTMEM_DCACHE_LOCK_DONE    (BIT(2))
#define EXTMEM_DCACHE_LOCK_DONE_M  (BIT(2))
#define EXTMEM_DCACHE_LOCK_DONE_V  0x1
#define EXTMEM_DCACHE_LOCK_DONE_S  2
/* EXTMEM_DCACHE_UNLOCK_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable unlock operation. It will be cleared by hardware after
 unlock operation done..*/
#define EXTMEM_DCACHE_UNLOCK_ENA    (BIT(1))
#define EXTMEM_DCACHE_UNLOCK_ENA_M  (BIT(1))
#define EXTMEM_DCACHE_UNLOCK_ENA_V  0x1
#define EXTMEM_DCACHE_UNLOCK_ENA_S  1
/* EXTMEM_DCACHE_LOCK_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable lock operation. It will be cleared by hardware after l
ock operation done..*/
#define EXTMEM_DCACHE_LOCK_ENA    (BIT(0))
#define EXTMEM_DCACHE_LOCK_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_LOCK_ENA_V  0x1
#define EXTMEM_DCACHE_LOCK_ENA_S  0

#define EXTMEM_DCACHE_LOCK_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x20)
/* EXTMEM_DCACHE_LOCK_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for lock operations. It
 should be combined with DCACHE_LOCK_SIZE_REG..*/
#define EXTMEM_DCACHE_LOCK_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_LOCK_ADDR_M  ((EXTMEM_DCACHE_LOCK_ADDR_V)<<(EXTMEM_DCACHE_LOCK_ADDR_S))
#define EXTMEM_DCACHE_LOCK_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_LOCK_ADDR_S  0

#define EXTMEM_DCACHE_LOCK_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x24)
/* EXTMEM_DCACHE_LOCK_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the length for lock operations. The bits are the
counts of cache block. It should be combined with DCACHE_LOCK_ADDR_REG..*/
#define EXTMEM_DCACHE_LOCK_SIZE    0x0000FFFF
#define EXTMEM_DCACHE_LOCK_SIZE_M  ((EXTMEM_DCACHE_LOCK_SIZE_V)<<(EXTMEM_DCACHE_LOCK_SIZE_S))
#define EXTMEM_DCACHE_LOCK_SIZE_V  0xFFFF
#define EXTMEM_DCACHE_LOCK_SIZE_S  0

#define EXTMEM_DCACHE_SYNC_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x28)
/* EXTMEM_DCACHE_SYNC_DONE : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate clean/writeback/invalidate operation is finished..*/
#define EXTMEM_DCACHE_SYNC_DONE    (BIT(3))
#define EXTMEM_DCACHE_SYNC_DONE_M  (BIT(3))
#define EXTMEM_DCACHE_SYNC_DONE_V  0x1
#define EXTMEM_DCACHE_SYNC_DONE_S  3
/* EXTMEM_DCACHE_CLEAN_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable clean operation. It will be cleared by hardware after
clean operation done..*/
#define EXTMEM_DCACHE_CLEAN_ENA    (BIT(2))
#define EXTMEM_DCACHE_CLEAN_ENA_M  (BIT(2))
#define EXTMEM_DCACHE_CLEAN_ENA_V  0x1
#define EXTMEM_DCACHE_CLEAN_ENA_S  2
/* EXTMEM_DCACHE_WRITEBACK_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable writeback operation. It will be cleared by hardware af
ter writeback operation done..*/
#define EXTMEM_DCACHE_WRITEBACK_ENA    (BIT(1))
#define EXTMEM_DCACHE_WRITEBACK_ENA_M  (BIT(1))
#define EXTMEM_DCACHE_WRITEBACK_ENA_V  0x1
#define EXTMEM_DCACHE_WRITEBACK_ENA_S  1
/* EXTMEM_DCACHE_INVALIDATE_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable invalidate operation. It will be cleared by hardware a
fter invalidate operation done..*/
#define EXTMEM_DCACHE_INVALIDATE_ENA    (BIT(0))
#define EXTMEM_DCACHE_INVALIDATE_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_INVALIDATE_ENA_V  0x1
#define EXTMEM_DCACHE_INVALIDATE_ENA_S  0

#define EXTMEM_DCACHE_SYNC_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x2C)
/* EXTMEM_DCACHE_SYNC_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for clean operations. I
t should be combined with DCACHE_SYNC_SIZE_REG..*/
#define EXTMEM_DCACHE_SYNC_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_SYNC_ADDR_M  ((EXTMEM_DCACHE_SYNC_ADDR_V)<<(EXTMEM_DCACHE_SYNC_ADDR_S))
#define EXTMEM_DCACHE_SYNC_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_SYNC_ADDR_S  0

#define EXTMEM_DCACHE_SYNC_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x30)
/* EXTMEM_DCACHE_SYNC_SIZE : R/W ;bitpos:[22:0] ;default: 23'h0 ; */
/*description: The bits are used to configure the length for sync operations. The bits are the
counts of cache block. It should be combined with DCACHE_SYNC_ADDR_REG..*/
#define EXTMEM_DCACHE_SYNC_SIZE    0x007FFFFF
#define EXTMEM_DCACHE_SYNC_SIZE_M  ((EXTMEM_DCACHE_SYNC_SIZE_V)<<(EXTMEM_DCACHE_SYNC_SIZE_S))
#define EXTMEM_DCACHE_SYNC_SIZE_V  0x7FFFFF
#define EXTMEM_DCACHE_SYNC_SIZE_S  0

#define EXTMEM_DCACHE_OCCUPY_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x34)
/* EXTMEM_DCACHE_OCCUPY_DONE : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to indicate occupy operation is finished..*/
#define EXTMEM_DCACHE_OCCUPY_DONE    (BIT(1))
#define EXTMEM_DCACHE_OCCUPY_DONE_M  (BIT(1))
#define EXTMEM_DCACHE_OCCUPY_DONE_V  0x1
#define EXTMEM_DCACHE_OCCUPY_DONE_S  1
/* EXTMEM_DCACHE_OCCUPY_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable occupy operation. It will be cleared by hardware after
 issuing Auot-Invalidate Operation..*/
#define EXTMEM_DCACHE_OCCUPY_ENA    (BIT(0))
#define EXTMEM_DCACHE_OCCUPY_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_OCCUPY_ENA_V  0x1
#define EXTMEM_DCACHE_OCCUPY_ENA_S  0

#define EXTMEM_DCACHE_OCCUPY_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x38)
/* EXTMEM_DCACHE_OCCUPY_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for occupy operation. I
t should be combined with DCACHE_OCCUPY_SIZE_REG..*/
#define EXTMEM_DCACHE_OCCUPY_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_OCCUPY_ADDR_M  ((EXTMEM_DCACHE_OCCUPY_ADDR_V)<<(EXTMEM_DCACHE_OCCUPY_ADDR_S))
#define EXTMEM_DCACHE_OCCUPY_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_OCCUPY_ADDR_S  0

#define EXTMEM_DCACHE_OCCUPY_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x3C)
/* EXTMEM_DCACHE_OCCUPY_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the length for occupy operation. The bits are the
 counts of cache block. It should be combined with DCACHE_OCCUPY_ADDR_REG..*/
#define EXTMEM_DCACHE_OCCUPY_SIZE    0x0000FFFF
#define EXTMEM_DCACHE_OCCUPY_SIZE_M  ((EXTMEM_DCACHE_OCCUPY_SIZE_V)<<(EXTMEM_DCACHE_OCCUPY_SIZE_S))
#define EXTMEM_DCACHE_OCCUPY_SIZE_V  0xFFFF
#define EXTMEM_DCACHE_OCCUPY_SIZE_S  0

#define EXTMEM_DCACHE_PRELOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x40)
/* EXTMEM_DCACHE_PRELOAD_ORDER : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to configure the direction of preload operation. 1: descending,
0: ascending..*/
#define EXTMEM_DCACHE_PRELOAD_ORDER    (BIT(2))
#define EXTMEM_DCACHE_PRELOAD_ORDER_M  (BIT(2))
#define EXTMEM_DCACHE_PRELOAD_ORDER_V  0x1
#define EXTMEM_DCACHE_PRELOAD_ORDER_S  2
/* EXTMEM_DCACHE_PRELOAD_DONE : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to indicate preload operation is finished..*/
#define EXTMEM_DCACHE_PRELOAD_DONE    (BIT(1))
#define EXTMEM_DCACHE_PRELOAD_DONE_M  (BIT(1))
#define EXTMEM_DCACHE_PRELOAD_DONE_V  0x1
#define EXTMEM_DCACHE_PRELOAD_DONE_S  1
/* EXTMEM_DCACHE_PRELOAD_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable preload operation. It will be cleared by hardware afte
r preload operation done..*/
#define EXTMEM_DCACHE_PRELOAD_ENA    (BIT(0))
#define EXTMEM_DCACHE_PRELOAD_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_PRELOAD_ENA_V  0x1
#define EXTMEM_DCACHE_PRELOAD_ENA_S  0

#define EXTMEM_DCACHE_PRELOAD_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x44)
/* EXTMEM_DCACHE_PRELOAD_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for preload operation.
It should be combined with DCACHE_PRELOAD_SIZE_REG..*/
#define EXTMEM_DCACHE_PRELOAD_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOAD_ADDR_M  ((EXTMEM_DCACHE_PRELOAD_ADDR_V)<<(EXTMEM_DCACHE_PRELOAD_ADDR_S))
#define EXTMEM_DCACHE_PRELOAD_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_PRELOAD_ADDR_S  0

#define EXTMEM_DCACHE_PRELOAD_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x48)
/* EXTMEM_DCACHE_PRELOAD_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the length for preload operation. The bits are th
e counts of cache block. It should be combined with DCACHE_PRELOAD_ADDR_REG...*/
#define EXTMEM_DCACHE_PRELOAD_SIZE    0x0000FFFF
#define EXTMEM_DCACHE_PRELOAD_SIZE_M  ((EXTMEM_DCACHE_PRELOAD_SIZE_V)<<(EXTMEM_DCACHE_PRELOAD_SIZE_S))
#define EXTMEM_DCACHE_PRELOAD_SIZE_V  0xFFFF
#define EXTMEM_DCACHE_PRELOAD_SIZE_S  0

#define EXTMEM_DCACHE_AUTOLOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x4C)
/* EXTMEM_DCACHE_AUTOLOAD_BUFFER_CLEAR : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The bit is used to clear autoload buffer in dcache..*/
#define EXTMEM_DCACHE_AUTOLOAD_BUFFER_CLEAR    (BIT(9))
#define EXTMEM_DCACHE_AUTOLOAD_BUFFER_CLEAR_M  (BIT(9))
#define EXTMEM_DCACHE_AUTOLOAD_BUFFER_CLEAR_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_BUFFER_CLEAR_S  9
/* EXTMEM_DCACHE_AUTOLOAD_SIZE : R/W ;bitpos:[8:7] ;default: 2'h0 ; */
/*description: The bits are used to configure the numbers of the cache block for the issuing au
toload operation..*/
#define EXTMEM_DCACHE_AUTOLOAD_SIZE    0x00000003
#define EXTMEM_DCACHE_AUTOLOAD_SIZE_M  ((EXTMEM_DCACHE_AUTOLOAD_SIZE_V)<<(EXTMEM_DCACHE_AUTOLOAD_SIZE_S))
#define EXTMEM_DCACHE_AUTOLOAD_SIZE_V  0x3
#define EXTMEM_DCACHE_AUTOLOAD_SIZE_S  7
/* EXTMEM_DCACHE_AUTOLOAD_RQST : R/W ;bitpos:[6:5] ;default: 2'b0 ; */
/*description: The bits are used to configure trigger conditions for autoload. 0/3: cache miss,
 1: cache hit, 2: both cache miss and hit..*/
#define EXTMEM_DCACHE_AUTOLOAD_RQST    0x00000003
#define EXTMEM_DCACHE_AUTOLOAD_RQST_M  ((EXTMEM_DCACHE_AUTOLOAD_RQST_V)<<(EXTMEM_DCACHE_AUTOLOAD_RQST_S))
#define EXTMEM_DCACHE_AUTOLOAD_RQST_V  0x3
#define EXTMEM_DCACHE_AUTOLOAD_RQST_S  5
/* EXTMEM_DCACHE_AUTOLOAD_ORDER : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bits are used to configure the direction of autoload. 1: descending, 0: asce
nding..*/
#define EXTMEM_DCACHE_AUTOLOAD_ORDER    (BIT(4))
#define EXTMEM_DCACHE_AUTOLOAD_ORDER_M  (BIT(4))
#define EXTMEM_DCACHE_AUTOLOAD_ORDER_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_ORDER_S  4
/* EXTMEM_DCACHE_AUTOLOAD_DONE : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: The bit is used to indicate autoload operation is finished..*/
#define EXTMEM_DCACHE_AUTOLOAD_DONE    (BIT(3))
#define EXTMEM_DCACHE_AUTOLOAD_DONE_M  (BIT(3))
#define EXTMEM_DCACHE_AUTOLOAD_DONE_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_DONE_S  3
/* EXTMEM_DCACHE_AUTOLOAD_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable and disable autoload operation. It is combined with dc
ache_autoload_done. 1: enable, 0: disable. .*/
#define EXTMEM_DCACHE_AUTOLOAD_ENA    (BIT(2))
#define EXTMEM_DCACHE_AUTOLOAD_ENA_M  (BIT(2))
#define EXTMEM_DCACHE_AUTOLOAD_ENA_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_ENA_S  2
/* EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bits are used to enable the second section for autoload operation..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA    (BIT(1))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA_M  (BIT(1))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ENA_S  1
/* EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bits are used to enable the first section for autoload operation..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA    (BIT(0))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA_V  0x1
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ENA_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x50)
/* EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address of the first section fo
r autoload operation. It should be combined with dcache_autoload_sct0_ena..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_ADDR_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x54)
/* EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE : R/W ;bitpos:[26:0] ;default: 27'h0 ; */
/*description: The bits are used to configure the length of the first section for autoload oper
ation. It should be combined with dcache_autoload_sct0_ena..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE    0x07FFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_V  0x7FFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT0_SIZE_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x58)
/* EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address of the second section f
or autoload operation. It should be combined with dcache_autoload_sct1_ena..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR    0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_ADDR_S  0

#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x5C)
/* EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE : R/W ;bitpos:[26:0] ;default: 27'h0 ; */
/*description: The bits are used to configure the length of the second section for autoload ope
ration. It should be combined with dcache_autoload_sct1_ena..*/
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE    0x07FFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_M  ((EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_V)<<(EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_S))
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_V  0x7FFFFFF
#define EXTMEM_DCACHE_AUTOLOAD_SCT1_SIZE_S  0

#define EXTMEM_ICACHE_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x60)
/* EXTMEM_ICACHE_BLOCKSIZE_MODE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to configure cache block size.0: 16 bytes, 1: 32 bytes.*/
#define EXTMEM_ICACHE_BLOCKSIZE_MODE    (BIT(3))
#define EXTMEM_ICACHE_BLOCKSIZE_MODE_M  (BIT(3))
#define EXTMEM_ICACHE_BLOCKSIZE_MODE_V  0x1
#define EXTMEM_ICACHE_BLOCKSIZE_MODE_S  3
/* EXTMEM_ICACHE_SIZE_MODE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to configure cache memory size.0: 16KB, 1: 32KB.*/
#define EXTMEM_ICACHE_SIZE_MODE    (BIT(2))
#define EXTMEM_ICACHE_SIZE_MODE_M  (BIT(2))
#define EXTMEM_ICACHE_SIZE_MODE_V  0x1
#define EXTMEM_ICACHE_SIZE_MODE_S  2
/* EXTMEM_ICACHE_WAY_MODE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to configure cache way mode.0: 4-way, 1: 8-way.*/
#define EXTMEM_ICACHE_WAY_MODE    (BIT(1))
#define EXTMEM_ICACHE_WAY_MODE_M  (BIT(1))
#define EXTMEM_ICACHE_WAY_MODE_V  0x1
#define EXTMEM_ICACHE_WAY_MODE_S  1
/* EXTMEM_ICACHE_ENABLE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to activate the data cache. 0: disable, 1: enable.*/
#define EXTMEM_ICACHE_ENABLE    (BIT(0))
#define EXTMEM_ICACHE_ENABLE_M  (BIT(0))
#define EXTMEM_ICACHE_ENABLE_V  0x1
#define EXTMEM_ICACHE_ENABLE_S  0

#define EXTMEM_ICACHE_CTRL1_REG          (DR_REG_EXTMEM_BASE + 0x64)
/* EXTMEM_ICACHE_SHUT_CORE1_BUS : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to disable core1 ibus, 0: enable, 1: disable.*/
#define EXTMEM_ICACHE_SHUT_CORE1_BUS    (BIT(1))
#define EXTMEM_ICACHE_SHUT_CORE1_BUS_M  (BIT(1))
#define EXTMEM_ICACHE_SHUT_CORE1_BUS_V  0x1
#define EXTMEM_ICACHE_SHUT_CORE1_BUS_S  1
/* EXTMEM_ICACHE_SHUT_CORE0_BUS : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to disable core0 ibus, 0: enable, 1: disable.*/
#define EXTMEM_ICACHE_SHUT_CORE0_BUS    (BIT(0))
#define EXTMEM_ICACHE_SHUT_CORE0_BUS_M  (BIT(0))
#define EXTMEM_ICACHE_SHUT_CORE0_BUS_V  0x1
#define EXTMEM_ICACHE_SHUT_CORE0_BUS_S  0

#define EXTMEM_ICACHE_TAG_POWER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x68)
/* EXTMEM_ICACHE_TAG_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to power  icache tag memory up, 0: follow rtc_lslp, 1: power up.*/
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU    (BIT(2))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_M  (BIT(2))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_V  0x1
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PU_S  2
/* EXTMEM_ICACHE_TAG_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to power  icache tag memory down, 0: follow rtc_lslp, 1: power d
own.*/
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD    (BIT(1))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_M  (BIT(1))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_V  0x1
#define EXTMEM_ICACHE_TAG_MEM_FORCE_PD_S  1
/* EXTMEM_ICACHE_TAG_MEM_FORCE_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of  icache tag memory. 1: close gating, 0:
 open clock gating..*/
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON    (BIT(0))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_M  (BIT(0))
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_V  0x1
#define EXTMEM_ICACHE_TAG_MEM_FORCE_ON_S  0

#define EXTMEM_ICACHE_PRELOCK_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x6C)
/* EXTMEM_ICACHE_PRELOCK_SCT1_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: The bit is used to enable the second section of prelock function..*/
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN    (BIT(1))
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN_V  0x1
#define EXTMEM_ICACHE_PRELOCK_SCT1_EN_S  1
/* EXTMEM_ICACHE_PRELOCK_SCT0_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The bit is used to enable the first section of prelock function..*/
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN    (BIT(0))
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN_M  (BIT(0))
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN_V  0x1
#define EXTMEM_ICACHE_PRELOCK_SCT0_EN_S  0

#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x70)
/* EXTMEM_ICACHE_PRELOCK_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to configure the first start virtual address of data prelock,
which is combined with ICACHE_PRELOCK_SCT0_SIZE_REG.*/
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_M  ((EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_V)<<(EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_S))
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_ADDR_S  0

#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x74)
/* EXTMEM_ICACHE_PRELOCK_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to configure the second start virtual address of data prelock,
 which is combined with ICACHE_PRELOCK_SCT1_SIZE_REG.*/
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_M  ((EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_V)<<(EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_S))
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_ADDR_S  0

#define EXTMEM_ICACHE_PRELOCK_SCT_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x78)
/* EXTMEM_ICACHE_PRELOCK_SCT0_SIZE : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: The bits are used to configure the first length of data locking, which is combin
ed with ICACHE_PRELOCK_SCT0_ADDR_REG.*/
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE    0x0000FFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_M  ((EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_V)<<(EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_S))
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT0_SIZE_S  16
/* EXTMEM_ICACHE_PRELOCK_SCT1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the second length of data locking, which is combi
ned with ICACHE_PRELOCK_SCT1_ADDR_REG.*/
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE    0x0000FFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_M  ((EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_V)<<(EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_S))
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_PRELOCK_SCT1_SIZE_S  0

#define EXTMEM_ICACHE_LOCK_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x7C)
/* EXTMEM_ICACHE_LOCK_DONE : RO ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to indicate unlock/lock operation is finished..*/
#define EXTMEM_ICACHE_LOCK_DONE    (BIT(2))
#define EXTMEM_ICACHE_LOCK_DONE_M  (BIT(2))
#define EXTMEM_ICACHE_LOCK_DONE_V  0x1
#define EXTMEM_ICACHE_LOCK_DONE_S  2
/* EXTMEM_ICACHE_UNLOCK_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable unlock operation. It will be cleared by hardware after
 unlock operation done..*/
#define EXTMEM_ICACHE_UNLOCK_ENA    (BIT(1))
#define EXTMEM_ICACHE_UNLOCK_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_UNLOCK_ENA_V  0x1
#define EXTMEM_ICACHE_UNLOCK_ENA_S  1
/* EXTMEM_ICACHE_LOCK_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable lock operation. It will be cleared by hardware after l
ock operation done..*/
#define EXTMEM_ICACHE_LOCK_ENA    (BIT(0))
#define EXTMEM_ICACHE_LOCK_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_LOCK_ENA_V  0x1
#define EXTMEM_ICACHE_LOCK_ENA_S  0

#define EXTMEM_ICACHE_LOCK_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x80)
/* EXTMEM_ICACHE_LOCK_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for lock operations. It
 should be combined with ICACHE_LOCK_SIZE_REG..*/
#define EXTMEM_ICACHE_LOCK_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE_LOCK_ADDR_M  ((EXTMEM_ICACHE_LOCK_ADDR_V)<<(EXTMEM_ICACHE_LOCK_ADDR_S))
#define EXTMEM_ICACHE_LOCK_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_LOCK_ADDR_S  0

#define EXTMEM_ICACHE_LOCK_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x84)
/* EXTMEM_ICACHE_LOCK_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the length for lock operations. The bits are the
counts of cache block. It should be combined with ICACHE_LOCK_ADDR_REG..*/
#define EXTMEM_ICACHE_LOCK_SIZE    0x0000FFFF
#define EXTMEM_ICACHE_LOCK_SIZE_M  ((EXTMEM_ICACHE_LOCK_SIZE_V)<<(EXTMEM_ICACHE_LOCK_SIZE_S))
#define EXTMEM_ICACHE_LOCK_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_LOCK_SIZE_S  0

#define EXTMEM_ICACHE_SYNC_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x88)
/* EXTMEM_ICACHE_SYNC_DONE : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to indicate invalidate operation is finished..*/
#define EXTMEM_ICACHE_SYNC_DONE    (BIT(1))
#define EXTMEM_ICACHE_SYNC_DONE_M  (BIT(1))
#define EXTMEM_ICACHE_SYNC_DONE_V  0x1
#define EXTMEM_ICACHE_SYNC_DONE_S  1
/* EXTMEM_ICACHE_INVALIDATE_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable invalidate operation. It will be cleared by hardware a
fter invalidate operation done..*/
#define EXTMEM_ICACHE_INVALIDATE_ENA    (BIT(0))
#define EXTMEM_ICACHE_INVALIDATE_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_INVALIDATE_ENA_V  0x1
#define EXTMEM_ICACHE_INVALIDATE_ENA_S  0

#define EXTMEM_ICACHE_SYNC_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x8C)
/* EXTMEM_ICACHE_SYNC_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for clean operations. I
t should be combined with ICACHE_SYNC_SIZE_REG..*/
#define EXTMEM_ICACHE_SYNC_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE_SYNC_ADDR_M  ((EXTMEM_ICACHE_SYNC_ADDR_V)<<(EXTMEM_ICACHE_SYNC_ADDR_S))
#define EXTMEM_ICACHE_SYNC_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_SYNC_ADDR_S  0

#define EXTMEM_ICACHE_SYNC_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x90)
/* EXTMEM_ICACHE_SYNC_SIZE : R/W ;bitpos:[22:0] ;default: 23'h0 ; */
/*description: The bits are used to configure the length for sync operations. The bits are the
counts of cache block. It should be combined with ICACHE_SYNC_ADDR_REG..*/
#define EXTMEM_ICACHE_SYNC_SIZE    0x007FFFFF
#define EXTMEM_ICACHE_SYNC_SIZE_M  ((EXTMEM_ICACHE_SYNC_SIZE_V)<<(EXTMEM_ICACHE_SYNC_SIZE_S))
#define EXTMEM_ICACHE_SYNC_SIZE_V  0x7FFFFF
#define EXTMEM_ICACHE_SYNC_SIZE_S  0

#define EXTMEM_ICACHE_PRELOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x94)
/* EXTMEM_ICACHE_PRELOAD_ORDER : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to configure the direction of preload operation. 1: descending,
0: ascending..*/
#define EXTMEM_ICACHE_PRELOAD_ORDER    (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_ORDER_M  (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_ORDER_V  0x1
#define EXTMEM_ICACHE_PRELOAD_ORDER_S  2
/* EXTMEM_ICACHE_PRELOAD_DONE : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to indicate preload operation is finished..*/
#define EXTMEM_ICACHE_PRELOAD_DONE    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_DONE_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_DONE_V  0x1
#define EXTMEM_ICACHE_PRELOAD_DONE_S  1
/* EXTMEM_ICACHE_PRELOAD_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable preload operation. It will be cleared by hardware afte
r preload operation done..*/
#define EXTMEM_ICACHE_PRELOAD_ENA    (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_ENA_V  0x1
#define EXTMEM_ICACHE_PRELOAD_ENA_S  0

#define EXTMEM_ICACHE_PRELOAD_ADDR_REG          (DR_REG_EXTMEM_BASE + 0x98)
/* EXTMEM_ICACHE_PRELOAD_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address for preload operation.
It should be combined with ICACHE_PRELOAD_SIZE_REG..*/
#define EXTMEM_ICACHE_PRELOAD_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOAD_ADDR_M  ((EXTMEM_ICACHE_PRELOAD_ADDR_V)<<(EXTMEM_ICACHE_PRELOAD_ADDR_S))
#define EXTMEM_ICACHE_PRELOAD_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_PRELOAD_ADDR_S  0

#define EXTMEM_ICACHE_PRELOAD_SIZE_REG          (DR_REG_EXTMEM_BASE + 0x9C)
/* EXTMEM_ICACHE_PRELOAD_SIZE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: The bits are used to configure the length for preload operation. The bits are th
e counts of cache block. It should be combined with ICACHE_PRELOAD_ADDR_REG...*/
#define EXTMEM_ICACHE_PRELOAD_SIZE    0x0000FFFF
#define EXTMEM_ICACHE_PRELOAD_SIZE_M  ((EXTMEM_ICACHE_PRELOAD_SIZE_V)<<(EXTMEM_ICACHE_PRELOAD_SIZE_S))
#define EXTMEM_ICACHE_PRELOAD_SIZE_V  0xFFFF
#define EXTMEM_ICACHE_PRELOAD_SIZE_S  0

#define EXTMEM_ICACHE_AUTOLOAD_CTRL_REG          (DR_REG_EXTMEM_BASE + 0xA0)
/* EXTMEM_ICACHE_AUTOLOAD_BUFFER_CLEAR : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The bit is used to clear autoload buffer in icache..*/
#define EXTMEM_ICACHE_AUTOLOAD_BUFFER_CLEAR    (BIT(9))
#define EXTMEM_ICACHE_AUTOLOAD_BUFFER_CLEAR_M  (BIT(9))
#define EXTMEM_ICACHE_AUTOLOAD_BUFFER_CLEAR_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_BUFFER_CLEAR_S  9
/* EXTMEM_ICACHE_AUTOLOAD_SIZE : R/W ;bitpos:[8:7] ;default: 2'h0 ; */
/*description: The bits are used to configure the numbers of the cache block for the issuing au
toload operation..*/
#define EXTMEM_ICACHE_AUTOLOAD_SIZE    0x00000003
#define EXTMEM_ICACHE_AUTOLOAD_SIZE_M  ((EXTMEM_ICACHE_AUTOLOAD_SIZE_V)<<(EXTMEM_ICACHE_AUTOLOAD_SIZE_S))
#define EXTMEM_ICACHE_AUTOLOAD_SIZE_V  0x3
#define EXTMEM_ICACHE_AUTOLOAD_SIZE_S  7
/* EXTMEM_ICACHE_AUTOLOAD_RQST : R/W ;bitpos:[6:5] ;default: 2'b0 ; */
/*description: The bits are used to configure trigger conditions for autoload. 0/3: cache miss,
 1: cache hit, 2: both cache miss and hit..*/
#define EXTMEM_ICACHE_AUTOLOAD_RQST    0x00000003
#define EXTMEM_ICACHE_AUTOLOAD_RQST_M  ((EXTMEM_ICACHE_AUTOLOAD_RQST_V)<<(EXTMEM_ICACHE_AUTOLOAD_RQST_S))
#define EXTMEM_ICACHE_AUTOLOAD_RQST_V  0x3
#define EXTMEM_ICACHE_AUTOLOAD_RQST_S  5
/* EXTMEM_ICACHE_AUTOLOAD_ORDER : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bits are used to configure the direction of autoload. 1: descending, 0: asce
nding..*/
#define EXTMEM_ICACHE_AUTOLOAD_ORDER    (BIT(4))
#define EXTMEM_ICACHE_AUTOLOAD_ORDER_M  (BIT(4))
#define EXTMEM_ICACHE_AUTOLOAD_ORDER_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_ORDER_S  4
/* EXTMEM_ICACHE_AUTOLOAD_DONE : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: The bit is used to indicate autoload operation is finished..*/
#define EXTMEM_ICACHE_AUTOLOAD_DONE    (BIT(3))
#define EXTMEM_ICACHE_AUTOLOAD_DONE_M  (BIT(3))
#define EXTMEM_ICACHE_AUTOLOAD_DONE_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_DONE_S  3
/* EXTMEM_ICACHE_AUTOLOAD_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable and disable autoload operation. It is combined with ic
ache_autoload_done. 1: enable, 0: disable. .*/
#define EXTMEM_ICACHE_AUTOLOAD_ENA    (BIT(2))
#define EXTMEM_ICACHE_AUTOLOAD_ENA_M  (BIT(2))
#define EXTMEM_ICACHE_AUTOLOAD_ENA_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_ENA_S  2
/* EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bits are used to enable the second section for autoload operation..*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA    (BIT(1))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ENA_S  1
/* EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bits are used to enable the first section for autoload operation..*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA    (BIT(0))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA_V  0x1
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ENA_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_REG          (DR_REG_EXTMEM_BASE + 0xA4)
/* EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address of the first section fo
r autoload operation. It should be combined with icache_autoload_sct0_ena..*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_ADDR_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_REG          (DR_REG_EXTMEM_BASE + 0xA8)
/* EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE : R/W ;bitpos:[26:0] ;default: 27'h0 ; */
/*description: The bits are used to configure the length of the first section for autoload oper
ation. It should be combined with icache_autoload_sct0_ena..*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE    0x07FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_V  0x7FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT0_SIZE_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_REG          (DR_REG_EXTMEM_BASE + 0xAC)
/* EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address of the second section f
or autoload operation. It should be combined with icache_autoload_sct1_ena..*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR    0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_V  0xFFFFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_ADDR_S  0

#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_REG          (DR_REG_EXTMEM_BASE + 0xB0)
/* EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE : R/W ;bitpos:[26:0] ;default: 27'h0 ; */
/*description: The bits are used to configure the length of the second section for autoload ope
ration. It should be combined with icache_autoload_sct1_ena..*/
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE    0x07FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_M  ((EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_V)<<(EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_S))
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_V  0x7FFFFFF
#define EXTMEM_ICACHE_AUTOLOAD_SCT1_SIZE_S  0

#define EXTMEM_IBUS_TO_FLASH_START_VADDR_REG          (DR_REG_EXTMEM_BASE + 0xB4)
/* EXTMEM_IBUS_TO_FLASH_START_VADDR : R/W ;bitpos:[31:0] ;default: 32'h44000000 ; */
/*description: The bits are used to configure the start virtual address of ibus to access flash
. The register is used to give constraints to ibus access counter..*/
#define EXTMEM_IBUS_TO_FLASH_START_VADDR    0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_M  ((EXTMEM_IBUS_TO_FLASH_START_VADDR_V)<<(EXTMEM_IBUS_TO_FLASH_START_VADDR_S))
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_V  0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_START_VADDR_S  0

#define EXTMEM_IBUS_TO_FLASH_END_VADDR_REG          (DR_REG_EXTMEM_BASE + 0xB8)
/* EXTMEM_IBUS_TO_FLASH_END_VADDR : R/W ;bitpos:[31:0] ;default: 32'h47ffffff ; */
/*description: The bits are used to configure the end virtual address of ibus to access flash.
The register is used to give constraints to ibus access counter..*/
#define EXTMEM_IBUS_TO_FLASH_END_VADDR    0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_M  ((EXTMEM_IBUS_TO_FLASH_END_VADDR_V)<<(EXTMEM_IBUS_TO_FLASH_END_VADDR_S))
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_V  0xFFFFFFFF
#define EXTMEM_IBUS_TO_FLASH_END_VADDR_S  0

#define EXTMEM_DBUS_TO_FLASH_START_VADDR_REG          (DR_REG_EXTMEM_BASE + 0xBC)
/* EXTMEM_DBUS_TO_FLASH_START_VADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the start virtual address of dbus to access flash
. The register is used to give constraints to dbus access counter..*/
#define EXTMEM_DBUS_TO_FLASH_START_VADDR    0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_M  ((EXTMEM_DBUS_TO_FLASH_START_VADDR_V)<<(EXTMEM_DBUS_TO_FLASH_START_VADDR_S))
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_V  0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_START_VADDR_S  0

#define EXTMEM_DBUS_TO_FLASH_END_VADDR_REG          (DR_REG_EXTMEM_BASE + 0xC0)
/* EXTMEM_DBUS_TO_FLASH_END_VADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The bits are used to configure the end virtual address of dbus to access flash.
The register is used to give constraints to dbus access counter..*/
#define EXTMEM_DBUS_TO_FLASH_END_VADDR    0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_M  ((EXTMEM_DBUS_TO_FLASH_END_VADDR_V)<<(EXTMEM_DBUS_TO_FLASH_END_VADDR_S))
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_V  0xFFFFFFFF
#define EXTMEM_DBUS_TO_FLASH_END_VADDR_S  0

#define EXTMEM_CACHE_ACS_CNT_CLR_REG          (DR_REG_EXTMEM_BASE + 0xC4)
/* EXTMEM_ICACHE_ACS_CNT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to clear icache counter..*/
#define EXTMEM_ICACHE_ACS_CNT_CLR    (BIT(1))
#define EXTMEM_ICACHE_ACS_CNT_CLR_M  (BIT(1))
#define EXTMEM_ICACHE_ACS_CNT_CLR_V  0x1
#define EXTMEM_ICACHE_ACS_CNT_CLR_S  1
/* EXTMEM_DCACHE_ACS_CNT_CLR : WOD ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to clear dcache counter..*/
#define EXTMEM_DCACHE_ACS_CNT_CLR    (BIT(0))
#define EXTMEM_DCACHE_ACS_CNT_CLR_M  (BIT(0))
#define EXTMEM_DCACHE_ACS_CNT_CLR_V  0x1
#define EXTMEM_DCACHE_ACS_CNT_CLR_S  0

#define EXTMEM_IBUS_ACS_MISS_CNT_REG          (DR_REG_EXTMEM_BASE + 0xC8)
/* EXTMEM_IBUS_ACS_MISS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of the cache miss caused by ibus access fl
ash/spiram..*/
#define EXTMEM_IBUS_ACS_MISS_CNT    0xFFFFFFFF
#define EXTMEM_IBUS_ACS_MISS_CNT_M  ((EXTMEM_IBUS_ACS_MISS_CNT_V)<<(EXTMEM_IBUS_ACS_MISS_CNT_S))
#define EXTMEM_IBUS_ACS_MISS_CNT_V  0xFFFFFFFF
#define EXTMEM_IBUS_ACS_MISS_CNT_S  0

#define EXTMEM_IBUS_ACS_CNT_REG          (DR_REG_EXTMEM_BASE + 0xCC)
/* EXTMEM_IBUS_ACS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of ibus access flash/spiram through icache
..*/
#define EXTMEM_IBUS_ACS_CNT    0xFFFFFFFF
#define EXTMEM_IBUS_ACS_CNT_M  ((EXTMEM_IBUS_ACS_CNT_V)<<(EXTMEM_IBUS_ACS_CNT_S))
#define EXTMEM_IBUS_ACS_CNT_V  0xFFFFFFFF
#define EXTMEM_IBUS_ACS_CNT_S  0

#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_REG          (DR_REG_EXTMEM_BASE + 0xD0)
/* EXTMEM_DBUS_ACS_FLASH_MISS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of the cache miss caused by dbus access fl
ash..*/
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT    0xFFFFFFFF
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_M  ((EXTMEM_DBUS_ACS_FLASH_MISS_CNT_V)<<(EXTMEM_DBUS_ACS_FLASH_MISS_CNT_S))
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_S  0

#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_REG          (DR_REG_EXTMEM_BASE + 0xD4)
/* EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of the cache miss caused by dbus access sp
iram..*/
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT    0xFFFFFFFF
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_M  ((EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_V)<<(EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_S))
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_S  0

#define EXTMEM_DBUS_ACS_CNT_REG          (DR_REG_EXTMEM_BASE + 0xD8)
/* EXTMEM_DBUS_ACS_CNT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to count the number of dbus access flash/spiram through dcache
..*/
#define EXTMEM_DBUS_ACS_CNT    0xFFFFFFFF
#define EXTMEM_DBUS_ACS_CNT_M  ((EXTMEM_DBUS_ACS_CNT_V)<<(EXTMEM_DBUS_ACS_CNT_S))
#define EXTMEM_DBUS_ACS_CNT_V  0xFFFFFFFF
#define EXTMEM_DBUS_ACS_CNT_S  0

#define EXTMEM_CACHE_ILG_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0xDC)
/* EXTMEM_DBUS_CNT_OVF_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by dbus counter overflow..*/
#define EXTMEM_DBUS_CNT_OVF_INT_ENA    (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_M  (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_V  0x1
#define EXTMEM_DBUS_CNT_OVF_INT_ENA_S  8
/* EXTMEM_IBUS_CNT_OVF_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by ibus counter overflow..*/
#define EXTMEM_IBUS_CNT_OVF_INT_ENA    (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_M  (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_V  0x1
#define EXTMEM_IBUS_CNT_OVF_INT_ENA_S  7
/* EXTMEM_DCACHE_OCCUPY_EXC_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by dcache trying to replace a line whose blo
cks all have been occupied by occupy-mode..*/
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_ENA    (BIT(6))
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_ENA_M  (BIT(6))
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_ENA_V  0x1
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_ENA_S  6
/* EXTMEM_MMU_ENTRY_FAULT_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by mmu entry fault..*/
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA    (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_M  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_V  0x1
#define EXTMEM_MMU_ENTRY_FAULT_INT_ENA_S  5
/* EXTMEM_DCACHE_WRITE_FLASH_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by dcache trying to write flash..*/
#define EXTMEM_DCACHE_WRITE_FLASH_INT_ENA    (BIT(4))
#define EXTMEM_DCACHE_WRITE_FLASH_INT_ENA_M  (BIT(4))
#define EXTMEM_DCACHE_WRITE_FLASH_INT_ENA_V  0x1
#define EXTMEM_DCACHE_WRITE_FLASH_INT_ENA_S  4
/* EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by preload configurations fault..*/
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_ENA    (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_ENA_M  (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_ENA_V  0x1
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_ENA_S  3
/* EXTMEM_DCACHE_SYNC_OP_FAULT_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by sync configurations fault..*/
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_ENA    (BIT(2))
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_ENA_M  (BIT(2))
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_ENA_V  0x1
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_ENA_S  2
/* EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by preload configurations fault..*/
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_V  0x1
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA_S  1
/* EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by sync configurations fault..*/
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA    (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_V  0x1
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA_S  0

#define EXTMEM_CACHE_ILG_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0xE0)
/* EXTMEM_DBUS_CNT_OVF_INT_CLR : WOD ;bitpos:[8] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by dbus counter overflow..*/
#define EXTMEM_DBUS_CNT_OVF_INT_CLR    (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_M  (BIT(8))
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_V  0x1
#define EXTMEM_DBUS_CNT_OVF_INT_CLR_S  8
/* EXTMEM_IBUS_CNT_OVF_INT_CLR : WOD ;bitpos:[7] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by ibus counter overflow..*/
#define EXTMEM_IBUS_CNT_OVF_INT_CLR    (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_M  (BIT(7))
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_V  0x1
#define EXTMEM_IBUS_CNT_OVF_INT_CLR_S  7
/* EXTMEM_DCACHE_OCCUPY_EXC_INT_CLR : WOD ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by dcache trying to replace a line whose bloc
ks all have been occupied by occupy-mode..*/
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_CLR    (BIT(6))
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_CLR_M  (BIT(6))
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_CLR_V  0x1
#define EXTMEM_DCACHE_OCCUPY_EXC_INT_CLR_S  6
/* EXTMEM_MMU_ENTRY_FAULT_INT_CLR : WOD ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by mmu entry fault..*/
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR    (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_M  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_V  0x1
#define EXTMEM_MMU_ENTRY_FAULT_INT_CLR_S  5
/* EXTMEM_DCACHE_WRITE_FLASH_INT_CLR : WOD ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by dcache trying to write flash..*/
#define EXTMEM_DCACHE_WRITE_FLASH_INT_CLR    (BIT(4))
#define EXTMEM_DCACHE_WRITE_FLASH_INT_CLR_M  (BIT(4))
#define EXTMEM_DCACHE_WRITE_FLASH_INT_CLR_V  0x1
#define EXTMEM_DCACHE_WRITE_FLASH_INT_CLR_S  4
/* EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_CLR : WOD ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by preload configurations fault..*/
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_CLR    (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_CLR_M  (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_CLR_V  0x1
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_CLR_S  3
/* EXTMEM_DCACHE_SYNC_OP_FAULT_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by sync configurations fault..*/
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_CLR    (BIT(2))
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_CLR_M  (BIT(2))
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_CLR_V  0x1
#define EXTMEM_DCACHE_SYNC_OP_FAULT_INT_CLR_S  2
/* EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by preload configurations fault..*/
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_V  0x1
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR_S  1
/* EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR : WOD ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by sync configurations fault..*/
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR    (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_V  0x1
#define EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR_S  0

#define EXTMEM_CACHE_ILG_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0xE4)
/* EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_OVF_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dbus access spiram miss counter overflo
w..*/
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_OVF_ST    (BIT(11))
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_OVF_ST_M  (BIT(11))
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_OVF_ST_V  0x1
#define EXTMEM_DBUS_ACS_SPIRAM_MISS_CNT_OVF_ST_S  11
/* EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dbus access flash miss counter overflow
..*/
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST    (BIT(10))
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_M  (BIT(10))
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_V  0x1
#define EXTMEM_DBUS_ACS_FLASH_MISS_CNT_OVF_ST_S  10
/* EXTMEM_DBUS_ACS_CNT_OVF_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dbus access flash/spiram counter overfl
ow..*/
#define EXTMEM_DBUS_ACS_CNT_OVF_ST    (BIT(9))
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_M  (BIT(9))
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_V  0x1
#define EXTMEM_DBUS_ACS_CNT_OVF_ST_S  9
/* EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by ibus access flash/spiram miss counter o
verflow..*/
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST    (BIT(8))
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_M  (BIT(8))
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_V  0x1
#define EXTMEM_IBUS_ACS_MISS_CNT_OVF_ST_S  8
/* EXTMEM_IBUS_ACS_CNT_OVF_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by ibus access flash/spiram counter overfl
ow..*/
#define EXTMEM_IBUS_ACS_CNT_OVF_ST    (BIT(7))
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_M  (BIT(7))
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_V  0x1
#define EXTMEM_IBUS_ACS_CNT_OVF_ST_S  7
/* EXTMEM_DCACHE_OCCUPY_EXC_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dcache trying to replace a line whose b
locks all have been occupied by occupy-mode..*/
#define EXTMEM_DCACHE_OCCUPY_EXC_ST    (BIT(6))
#define EXTMEM_DCACHE_OCCUPY_EXC_ST_M  (BIT(6))
#define EXTMEM_DCACHE_OCCUPY_EXC_ST_V  0x1
#define EXTMEM_DCACHE_OCCUPY_EXC_ST_S  6
/* EXTMEM_MMU_ENTRY_FAULT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by mmu entry fault..*/
#define EXTMEM_MMU_ENTRY_FAULT_ST    (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_ST_M  (BIT(5))
#define EXTMEM_MMU_ENTRY_FAULT_ST_V  0x1
#define EXTMEM_MMU_ENTRY_FAULT_ST_S  5
/* EXTMEM_DCACHE_WRITE_FLASH_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by dcache trying to write flash..*/
#define EXTMEM_DCACHE_WRITE_FLASH_ST    (BIT(4))
#define EXTMEM_DCACHE_WRITE_FLASH_ST_M  (BIT(4))
#define EXTMEM_DCACHE_WRITE_FLASH_ST_V  0x1
#define EXTMEM_DCACHE_WRITE_FLASH_ST_S  4
/* EXTMEM_DCACHE_PRELOAD_OP_FAULT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by preload configurations fault..*/
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_ST    (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_ST_M  (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_ST_V  0x1
#define EXTMEM_DCACHE_PRELOAD_OP_FAULT_ST_S  3
/* EXTMEM_DCACHE_SYNC_OP_FAULT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by sync configurations fault..*/
#define EXTMEM_DCACHE_SYNC_OP_FAULT_ST    (BIT(2))
#define EXTMEM_DCACHE_SYNC_OP_FAULT_ST_M  (BIT(2))
#define EXTMEM_DCACHE_SYNC_OP_FAULT_ST_V  0x1
#define EXTMEM_DCACHE_SYNC_OP_FAULT_ST_S  2
/* EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by preload configurations fault..*/
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_V  0x1
#define EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST_S  1
/* EXTMEM_ICACHE_SYNC_OP_FAULT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by sync configurations fault..*/
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST    (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_V  0x1
#define EXTMEM_ICACHE_SYNC_OP_FAULT_ST_S  0

#define EXTMEM_CORE0_ACS_CACHE_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0xE8)
/* EXTMEM_CORE0_DBUS_REJECT_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by authentication fail..*/
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA    (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_M  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_V  0x1
#define EXTMEM_CORE0_DBUS_REJECT_INT_ENA_S  4
/* EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by cpu access dcache while the corresponding
 dbus is disabled which include speculative access..*/
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_ENA    (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_ENA_M  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_ENA_V  0x1
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_ENA_S  3
/* EXTMEM_CORE0_IBUS_REJECT_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by authentication fail..*/
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA    (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_M  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_V  0x1
#define EXTMEM_CORE0_IBUS_REJECT_INT_ENA_S  2
/* EXTMEM_CORE0_IBUS_WR_IC_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by ibus trying to write icache.*/
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA    (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_M  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_V  0x1
#define EXTMEM_CORE0_IBUS_WR_IC_INT_ENA_S  1
/* EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by cpu access icache while the corresponding
 ibus is disabled which include speculative access..*/
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA    (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_M  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_V  0x1
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA_S  0

#define EXTMEM_CORE0_ACS_CACHE_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0xEC)
/* EXTMEM_CORE0_DBUS_REJECT_INT_CLR : WOD ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by authentication fail..*/
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR    (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_M  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_V  0x1
#define EXTMEM_CORE0_DBUS_REJECT_INT_CLR_S  4
/* EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_CLR : WOD ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by cpu access dcache while the corresponding
dbus is disabled or dcache is disabled which include speculative access..*/
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_CLR    (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_CLR_M  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_CLR_V  0x1
#define EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_CLR_S  3
/* EXTMEM_CORE0_IBUS_REJECT_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by authentication fail..*/
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR    (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_M  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_V  0x1
#define EXTMEM_CORE0_IBUS_REJECT_INT_CLR_S  2
/* EXTMEM_CORE0_IBUS_WR_IC_INT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by ibus trying to write icache.*/
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR    (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_M  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_V  0x1
#define EXTMEM_CORE0_IBUS_WR_IC_INT_CLR_S  1
/* EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR : WOD ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by cpu access icache while the corresponding
ibus is disabled or icache is disabled which include speculative access..*/
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR    (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_M  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_V  0x1
#define EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR_S  0

#define EXTMEM_CORE0_ACS_CACHE_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0xF0)
/* EXTMEM_CORE0_DBUS_REJECT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by authentication fail..*/
#define EXTMEM_CORE0_DBUS_REJECT_ST    (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_ST_M  (BIT(4))
#define EXTMEM_CORE0_DBUS_REJECT_ST_V  0x1
#define EXTMEM_CORE0_DBUS_REJECT_ST_S  4
/* EXTMEM_CORE0_DBUS_ACS_MSK_DCACHE_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by cpu access dcache while the core0_dbus
is disabled or dcache is disabled which include speculative access..*/
#define EXTMEM_CORE0_DBUS_ACS_MSK_DCACHE_ST    (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_DCACHE_ST_M  (BIT(3))
#define EXTMEM_CORE0_DBUS_ACS_MSK_DCACHE_ST_V  0x1
#define EXTMEM_CORE0_DBUS_ACS_MSK_DCACHE_ST_S  3
/* EXTMEM_CORE0_IBUS_REJECT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by authentication fail..*/
#define EXTMEM_CORE0_IBUS_REJECT_ST    (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_ST_M  (BIT(2))
#define EXTMEM_CORE0_IBUS_REJECT_ST_V  0x1
#define EXTMEM_CORE0_IBUS_REJECT_ST_S  2
/* EXTMEM_CORE0_IBUS_WR_ICACHE_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by ibus trying to write icache.*/
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST    (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_M  (BIT(1))
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_V  0x1
#define EXTMEM_CORE0_IBUS_WR_ICACHE_ST_S  1
/* EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by cpu access  icache while the core0_ibus
 is disabled or icache is disabled which include speculative access..*/
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST    (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_M  (BIT(0))
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_V  0x1
#define EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST_S  0

#define EXTMEM_CORE1_ACS_CACHE_INT_ENA_REG          (DR_REG_EXTMEM_BASE + 0xF4)
/* EXTMEM_CORE1_DBUS_REJECT_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by authentication fail..*/
#define EXTMEM_CORE1_DBUS_REJECT_INT_ENA    (BIT(4))
#define EXTMEM_CORE1_DBUS_REJECT_INT_ENA_M  (BIT(4))
#define EXTMEM_CORE1_DBUS_REJECT_INT_ENA_V  0x1
#define EXTMEM_CORE1_DBUS_REJECT_INT_ENA_S  4
/* EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by cpu access dcache while the corresponding
 dbus is disabled which include speculative access..*/
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_ENA    (BIT(3))
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_ENA_M  (BIT(3))
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_ENA_V  0x1
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_ENA_S  3
/* EXTMEM_CORE1_IBUS_REJECT_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by authentication fail..*/
#define EXTMEM_CORE1_IBUS_REJECT_INT_ENA    (BIT(2))
#define EXTMEM_CORE1_IBUS_REJECT_INT_ENA_M  (BIT(2))
#define EXTMEM_CORE1_IBUS_REJECT_INT_ENA_V  0x1
#define EXTMEM_CORE1_IBUS_REJECT_INT_ENA_S  2
/* EXTMEM_CORE1_IBUS_WR_IC_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by ibus trying to write icache.*/
#define EXTMEM_CORE1_IBUS_WR_IC_INT_ENA    (BIT(1))
#define EXTMEM_CORE1_IBUS_WR_IC_INT_ENA_M  (BIT(1))
#define EXTMEM_CORE1_IBUS_WR_IC_INT_ENA_V  0x1
#define EXTMEM_CORE1_IBUS_WR_IC_INT_ENA_S  1
/* EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable interrupt by cpu access icache while the corresponding
 ibus is disabled which include speculative access..*/
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_ENA    (BIT(0))
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_ENA_M  (BIT(0))
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_ENA_V  0x1
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_ENA_S  0

#define EXTMEM_CORE1_ACS_CACHE_INT_CLR_REG          (DR_REG_EXTMEM_BASE + 0xF8)
/* EXTMEM_CORE1_DBUS_REJECT_INT_CLR : WOD ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by authentication fail..*/
#define EXTMEM_CORE1_DBUS_REJECT_INT_CLR    (BIT(4))
#define EXTMEM_CORE1_DBUS_REJECT_INT_CLR_M  (BIT(4))
#define EXTMEM_CORE1_DBUS_REJECT_INT_CLR_V  0x1
#define EXTMEM_CORE1_DBUS_REJECT_INT_CLR_S  4
/* EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_CLR : WOD ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by cpu access dcache while the corresponding
dbus is disabled or dcache is disabled which include speculative access..*/
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_CLR    (BIT(3))
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_CLR_M  (BIT(3))
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_CLR_V  0x1
#define EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_CLR_S  3
/* EXTMEM_CORE1_IBUS_REJECT_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by authentication fail..*/
#define EXTMEM_CORE1_IBUS_REJECT_INT_CLR    (BIT(2))
#define EXTMEM_CORE1_IBUS_REJECT_INT_CLR_M  (BIT(2))
#define EXTMEM_CORE1_IBUS_REJECT_INT_CLR_V  0x1
#define EXTMEM_CORE1_IBUS_REJECT_INT_CLR_S  2
/* EXTMEM_CORE1_IBUS_WR_IC_INT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by ibus trying to write icache.*/
#define EXTMEM_CORE1_IBUS_WR_IC_INT_CLR    (BIT(1))
#define EXTMEM_CORE1_IBUS_WR_IC_INT_CLR_M  (BIT(1))
#define EXTMEM_CORE1_IBUS_WR_IC_INT_CLR_V  0x1
#define EXTMEM_CORE1_IBUS_WR_IC_INT_CLR_S  1
/* EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_CLR : WOD ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to clear interrupt by cpu access icache while the corresponding
ibus is disabled or icache is disabled which include speculative access..*/
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_CLR    (BIT(0))
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_CLR_M  (BIT(0))
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_CLR_V  0x1
#define EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_CLR_S  0

#define EXTMEM_CORE1_ACS_CACHE_INT_ST_REG          (DR_REG_EXTMEM_BASE + 0xFC)
/* EXTMEM_CORE1_DBUS_REJECT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by authentication fail..*/
#define EXTMEM_CORE1_DBUS_REJECT_ST    (BIT(4))
#define EXTMEM_CORE1_DBUS_REJECT_ST_M  (BIT(4))
#define EXTMEM_CORE1_DBUS_REJECT_ST_V  0x1
#define EXTMEM_CORE1_DBUS_REJECT_ST_S  4
/* EXTMEM_CORE1_DBUS_ACS_MSK_DCACHE_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by cpu access dcache while the core1_dbus
is disabled or dcache is disabled which include speculative access..*/
#define EXTMEM_CORE1_DBUS_ACS_MSK_DCACHE_ST    (BIT(3))
#define EXTMEM_CORE1_DBUS_ACS_MSK_DCACHE_ST_M  (BIT(3))
#define EXTMEM_CORE1_DBUS_ACS_MSK_DCACHE_ST_V  0x1
#define EXTMEM_CORE1_DBUS_ACS_MSK_DCACHE_ST_S  3
/* EXTMEM_CORE1_IBUS_REJECT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by authentication fail..*/
#define EXTMEM_CORE1_IBUS_REJECT_ST    (BIT(2))
#define EXTMEM_CORE1_IBUS_REJECT_ST_M  (BIT(2))
#define EXTMEM_CORE1_IBUS_REJECT_ST_V  0x1
#define EXTMEM_CORE1_IBUS_REJECT_ST_S  2
/* EXTMEM_CORE1_IBUS_WR_ICACHE_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by ibus trying to write icache.*/
#define EXTMEM_CORE1_IBUS_WR_ICACHE_ST    (BIT(1))
#define EXTMEM_CORE1_IBUS_WR_ICACHE_ST_M  (BIT(1))
#define EXTMEM_CORE1_IBUS_WR_ICACHE_ST_V  0x1
#define EXTMEM_CORE1_IBUS_WR_ICACHE_ST_S  1
/* EXTMEM_CORE1_IBUS_ACS_MSK_ICACHE_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate interrupt by cpu access  icache while the core1_ibus
 is disabled or  icache is disabled which include speculative access..*/
#define EXTMEM_CORE1_IBUS_ACS_MSK_ICACHE_ST    (BIT(0))
#define EXTMEM_CORE1_IBUS_ACS_MSK_ICACHE_ST_M  (BIT(0))
#define EXTMEM_CORE1_IBUS_ACS_MSK_ICACHE_ST_V  0x1
#define EXTMEM_CORE1_IBUS_ACS_MSK_ICACHE_ST_S  0

#define EXTMEM_CORE0_DBUS_REJECT_ST_REG          (DR_REG_EXTMEM_BASE + 0x100)
/* EXTMEM_CORE0_DBUS_WORLD : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to indicate the world of CPU access dbus when authentication fai
l. 0: WORLD0, 1: WORLD1.*/
#define EXTMEM_CORE0_DBUS_WORLD    (BIT(6))
#define EXTMEM_CORE0_DBUS_WORLD_M  (BIT(6))
#define EXTMEM_CORE0_DBUS_WORLD_V  0x1
#define EXTMEM_CORE0_DBUS_WORLD_S  6
/* EXTMEM_CORE0_DBUS_ATTR : RO ;bitpos:[5:3] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of CPU access dbus when authenticati
on fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able..*/
#define EXTMEM_CORE0_DBUS_ATTR    0x00000007
#define EXTMEM_CORE0_DBUS_ATTR_M  ((EXTMEM_CORE0_DBUS_ATTR_V)<<(EXTMEM_CORE0_DBUS_ATTR_S))
#define EXTMEM_CORE0_DBUS_ATTR_V  0x7
#define EXTMEM_CORE0_DBUS_ATTR_S  3
/* EXTMEM_CORE0_DBUS_TAG_ATTR : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of data from external memory when au
thentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able..*/
#define EXTMEM_CORE0_DBUS_TAG_ATTR    0x00000007
#define EXTMEM_CORE0_DBUS_TAG_ATTR_M  ((EXTMEM_CORE0_DBUS_TAG_ATTR_V)<<(EXTMEM_CORE0_DBUS_TAG_ATTR_S))
#define EXTMEM_CORE0_DBUS_TAG_ATTR_V  0x7
#define EXTMEM_CORE0_DBUS_TAG_ATTR_S  0

#define EXTMEM_CORE0_DBUS_REJECT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x104)
/* EXTMEM_CORE0_DBUS_VADDR : RO ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: The bits are used to indicate the virtual address of CPU access dbus when authen
tication fail..*/
#define EXTMEM_CORE0_DBUS_VADDR    0xFFFFFFFF
#define EXTMEM_CORE0_DBUS_VADDR_M  ((EXTMEM_CORE0_DBUS_VADDR_V)<<(EXTMEM_CORE0_DBUS_VADDR_S))
#define EXTMEM_CORE0_DBUS_VADDR_V  0xFFFFFFFF
#define EXTMEM_CORE0_DBUS_VADDR_S  0

#define EXTMEM_CORE0_IBUS_REJECT_ST_REG          (DR_REG_EXTMEM_BASE + 0x108)
/* EXTMEM_CORE0_IBUS_WORLD : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to indicate the world of CPU access ibus when authentication fai
l. 0: WORLD0, 1: WORLD1.*/
#define EXTMEM_CORE0_IBUS_WORLD    (BIT(6))
#define EXTMEM_CORE0_IBUS_WORLD_M  (BIT(6))
#define EXTMEM_CORE0_IBUS_WORLD_V  0x1
#define EXTMEM_CORE0_IBUS_WORLD_S  6
/* EXTMEM_CORE0_IBUS_ATTR : RO ;bitpos:[5:3] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of CPU access ibus when authenticati
on fail. 0: invalidate, 1: execute-able, 2: read-able.*/
#define EXTMEM_CORE0_IBUS_ATTR    0x00000007
#define EXTMEM_CORE0_IBUS_ATTR_M  ((EXTMEM_CORE0_IBUS_ATTR_V)<<(EXTMEM_CORE0_IBUS_ATTR_S))
#define EXTMEM_CORE0_IBUS_ATTR_V  0x7
#define EXTMEM_CORE0_IBUS_ATTR_S  3
/* EXTMEM_CORE0_IBUS_TAG_ATTR : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of data from external memory when au
thentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able..*/
#define EXTMEM_CORE0_IBUS_TAG_ATTR    0x00000007
#define EXTMEM_CORE0_IBUS_TAG_ATTR_M  ((EXTMEM_CORE0_IBUS_TAG_ATTR_V)<<(EXTMEM_CORE0_IBUS_TAG_ATTR_S))
#define EXTMEM_CORE0_IBUS_TAG_ATTR_V  0x7
#define EXTMEM_CORE0_IBUS_TAG_ATTR_S  0

#define EXTMEM_CORE0_IBUS_REJECT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x10C)
/* EXTMEM_CORE0_IBUS_VADDR : RO ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: The bits are used to indicate the virtual address of CPU access  ibus when authe
ntication fail..*/
#define EXTMEM_CORE0_IBUS_VADDR    0xFFFFFFFF
#define EXTMEM_CORE0_IBUS_VADDR_M  ((EXTMEM_CORE0_IBUS_VADDR_V)<<(EXTMEM_CORE0_IBUS_VADDR_S))
#define EXTMEM_CORE0_IBUS_VADDR_V  0xFFFFFFFF
#define EXTMEM_CORE0_IBUS_VADDR_S  0

#define EXTMEM_CORE1_DBUS_REJECT_ST_REG          (DR_REG_EXTMEM_BASE + 0x110)
/* EXTMEM_CORE1_DBUS_WORLD : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to indicate the world of CPU access dbus when authentication fai
l. 0: WORLD0, 1: WORLD1.*/
#define EXTMEM_CORE1_DBUS_WORLD    (BIT(6))
#define EXTMEM_CORE1_DBUS_WORLD_M  (BIT(6))
#define EXTMEM_CORE1_DBUS_WORLD_V  0x1
#define EXTMEM_CORE1_DBUS_WORLD_S  6
/* EXTMEM_CORE1_DBUS_ATTR : RO ;bitpos:[5:3] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of CPU access dbus when authenticati
on fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able..*/
#define EXTMEM_CORE1_DBUS_ATTR    0x00000007
#define EXTMEM_CORE1_DBUS_ATTR_M  ((EXTMEM_CORE1_DBUS_ATTR_V)<<(EXTMEM_CORE1_DBUS_ATTR_S))
#define EXTMEM_CORE1_DBUS_ATTR_V  0x7
#define EXTMEM_CORE1_DBUS_ATTR_S  3
/* EXTMEM_CORE1_DBUS_TAG_ATTR : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of data from external memory when au
thentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able..*/
#define EXTMEM_CORE1_DBUS_TAG_ATTR    0x00000007
#define EXTMEM_CORE1_DBUS_TAG_ATTR_M  ((EXTMEM_CORE1_DBUS_TAG_ATTR_V)<<(EXTMEM_CORE1_DBUS_TAG_ATTR_S))
#define EXTMEM_CORE1_DBUS_TAG_ATTR_V  0x7
#define EXTMEM_CORE1_DBUS_TAG_ATTR_S  0

#define EXTMEM_CORE1_DBUS_REJECT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x114)
/* EXTMEM_CORE1_DBUS_VADDR : RO ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: The bits are used to indicate the virtual address of CPU access dbus when authen
tication fail..*/
#define EXTMEM_CORE1_DBUS_VADDR    0xFFFFFFFF
#define EXTMEM_CORE1_DBUS_VADDR_M  ((EXTMEM_CORE1_DBUS_VADDR_V)<<(EXTMEM_CORE1_DBUS_VADDR_S))
#define EXTMEM_CORE1_DBUS_VADDR_V  0xFFFFFFFF
#define EXTMEM_CORE1_DBUS_VADDR_S  0

#define EXTMEM_CORE1_IBUS_REJECT_ST_REG          (DR_REG_EXTMEM_BASE + 0x118)
/* EXTMEM_CORE1_IBUS_WORLD : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to indicate the world of CPU access ibus when authentication fai
l. 0: WORLD0, 1: WORLD1.*/
#define EXTMEM_CORE1_IBUS_WORLD    (BIT(6))
#define EXTMEM_CORE1_IBUS_WORLD_M  (BIT(6))
#define EXTMEM_CORE1_IBUS_WORLD_V  0x1
#define EXTMEM_CORE1_IBUS_WORLD_S  6
/* EXTMEM_CORE1_IBUS_ATTR : RO ;bitpos:[5:3] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of CPU access ibus when authenticati
on fail. 0: invalidate, 1: execute-able, 2: read-able.*/
#define EXTMEM_CORE1_IBUS_ATTR    0x00000007
#define EXTMEM_CORE1_IBUS_ATTR_M  ((EXTMEM_CORE1_IBUS_ATTR_V)<<(EXTMEM_CORE1_IBUS_ATTR_S))
#define EXTMEM_CORE1_IBUS_ATTR_V  0x7
#define EXTMEM_CORE1_IBUS_ATTR_S  3
/* EXTMEM_CORE1_IBUS_TAG_ATTR : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The bits are used to indicate the attribute of data from external memory when au
thentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able..*/
#define EXTMEM_CORE1_IBUS_TAG_ATTR    0x00000007
#define EXTMEM_CORE1_IBUS_TAG_ATTR_M  ((EXTMEM_CORE1_IBUS_TAG_ATTR_V)<<(EXTMEM_CORE1_IBUS_TAG_ATTR_S))
#define EXTMEM_CORE1_IBUS_TAG_ATTR_V  0x7
#define EXTMEM_CORE1_IBUS_TAG_ATTR_S  0

#define EXTMEM_CORE1_IBUS_REJECT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x11C)
/* EXTMEM_CORE1_IBUS_VADDR : RO ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: The bits are used to indicate the virtual address of CPU access  ibus when authe
ntication fail..*/
#define EXTMEM_CORE1_IBUS_VADDR    0xFFFFFFFF
#define EXTMEM_CORE1_IBUS_VADDR_M  ((EXTMEM_CORE1_IBUS_VADDR_V)<<(EXTMEM_CORE1_IBUS_VADDR_S))
#define EXTMEM_CORE1_IBUS_VADDR_V  0xFFFFFFFF
#define EXTMEM_CORE1_IBUS_VADDR_S  0

#define EXTMEM_CACHE_MMU_FAULT_CONTENT_REG          (DR_REG_EXTMEM_BASE + 0x120)
/* EXTMEM_CACHE_MMU_FAULT_CODE : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: The right-most 3 bits are used to indicate the operations which cause mmu fault
occurrence. 0: default, 1: cpu miss, 2: preload miss, 3: writeback, 4: cpu miss
evict recovery address, 5: load miss evict recovery address, 6: external dma tx,
 7: external dma rx. The most significant bit is used to indicate this operation
 occurs in which one icache. .*/
#define EXTMEM_CACHE_MMU_FAULT_CODE    0x0000000F
#define EXTMEM_CACHE_MMU_FAULT_CODE_M  ((EXTMEM_CACHE_MMU_FAULT_CODE_V)<<(EXTMEM_CACHE_MMU_FAULT_CODE_S))
#define EXTMEM_CACHE_MMU_FAULT_CODE_V  0xF
#define EXTMEM_CACHE_MMU_FAULT_CODE_S  16
/* EXTMEM_CACHE_MMU_FAULT_CONTENT : RO ;bitpos:[15:0] ;default: 17'h0 ; */
/*description: The bits are used to indicate the content of mmu entry which cause mmu fault...*/
#define EXTMEM_CACHE_MMU_FAULT_CONTENT    0x0000FFFF
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_M  ((EXTMEM_CACHE_MMU_FAULT_CONTENT_V)<<(EXTMEM_CACHE_MMU_FAULT_CONTENT_S))
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_V  0xFFFF
#define EXTMEM_CACHE_MMU_FAULT_CONTENT_S  0

#define EXTMEM_CACHE_MMU_FAULT_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x124)
/* EXTMEM_CACHE_MMU_FAULT_VADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The bits are used to indicate the virtual address which cause mmu fault...*/
#define EXTMEM_CACHE_MMU_FAULT_VADDR    0xFFFFFFFF
#define EXTMEM_CACHE_MMU_FAULT_VADDR_M  ((EXTMEM_CACHE_MMU_FAULT_VADDR_V)<<(EXTMEM_CACHE_MMU_FAULT_VADDR_S))
#define EXTMEM_CACHE_MMU_FAULT_VADDR_V  0xFFFFFFFF
#define EXTMEM_CACHE_MMU_FAULT_VADDR_S  0

#define EXTMEM_CACHE_WRAP_AROUND_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x128)
/* EXTMEM_CACHE_SRAM_RD_WRAP_AROUND : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable wrap around mode when read data from spiram..*/
#define EXTMEM_CACHE_SRAM_RD_WRAP_AROUND    (BIT(1))
#define EXTMEM_CACHE_SRAM_RD_WRAP_AROUND_M  (BIT(1))
#define EXTMEM_CACHE_SRAM_RD_WRAP_AROUND_V  0x1
#define EXTMEM_CACHE_SRAM_RD_WRAP_AROUND_S  1
/* EXTMEM_CACHE_FLASH_WRAP_AROUND : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable wrap around mode when read data from flash..*/
#define EXTMEM_CACHE_FLASH_WRAP_AROUND    (BIT(0))
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_M  (BIT(0))
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_V  0x1
#define EXTMEM_CACHE_FLASH_WRAP_AROUND_S  0

#define EXTMEM_CACHE_MMU_POWER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x12C)
/* EXTMEM_CACHE_MMU_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to power mmu memory down, 0: follow_rtc_lslp_pd, 1: power up.*/
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU    (BIT(2))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_M  (BIT(2))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_V  0x1
#define EXTMEM_CACHE_MMU_MEM_FORCE_PU_S  2
/* EXTMEM_CACHE_MMU_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to power mmu memory down, 0: follow_rtc_lslp_pd, 1: power down.*/
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD    (BIT(1))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_M  (BIT(1))
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_V  0x1
#define EXTMEM_CACHE_MMU_MEM_FORCE_PD_S  1
/* EXTMEM_CACHE_MMU_MEM_FORCE_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable clock gating to save power when access mmu memory, 0:
enable, 1: disable.*/
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON    (BIT(0))
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_M  (BIT(0))
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_V  0x1
#define EXTMEM_CACHE_MMU_MEM_FORCE_ON_S  0

#define EXTMEM_CACHE_STATE_REG          (DR_REG_EXTMEM_BASE + 0x130)
/* EXTMEM_DCACHE_STATE : RO ;bitpos:[23:12] ;default: 12'h001 ; */
/*description: The bit is used to indicate whether dcache main fsm is in idle state or not. 1:
in idle state,  0: not in idle state.*/
#define EXTMEM_DCACHE_STATE    0x00000FFF
#define EXTMEM_DCACHE_STATE_M  ((EXTMEM_DCACHE_STATE_V)<<(EXTMEM_DCACHE_STATE_S))
#define EXTMEM_DCACHE_STATE_V  0xFFF
#define EXTMEM_DCACHE_STATE_S  12
/* EXTMEM_ICACHE_STATE : RO ;bitpos:[11:0] ;default: 12'h001 ; */
/*description: The bit is used to indicate whether  icache main fsm is in idle state or not. 1:
 in idle state,  0: not in idle state.*/
#define EXTMEM_ICACHE_STATE    0x00000FFF
#define EXTMEM_ICACHE_STATE_M  ((EXTMEM_ICACHE_STATE_V)<<(EXTMEM_ICACHE_STATE_S))
#define EXTMEM_ICACHE_STATE_V  0xFFF
#define EXTMEM_ICACHE_STATE_S  0

#define EXTMEM_CACHE_ENCRYPT_DECRYPT_RECORD_DISABLE_REG          (DR_REG_EXTMEM_BASE + 0x134)
/* EXTMEM_RECORD_DISABLE_G0CB_DECRYPT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Reserved..*/
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT    (BIT(1))
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_M  (BIT(1))
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_V  0x1
#define EXTMEM_RECORD_DISABLE_G0CB_DECRYPT_S  1
/* EXTMEM_RECORD_DISABLE_DB_ENCRYPT : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Reserved..*/
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT    (BIT(0))
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_M  (BIT(0))
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_V  0x1
#define EXTMEM_RECORD_DISABLE_DB_ENCRYPT_S  0

#define EXTMEM_CACHE_ENCRYPT_DECRYPT_CLK_FORCE_ON_REG          (DR_REG_EXTMEM_BASE + 0x138)
/* EXTMEM_CLK_FORCE_ON_CRYPT : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of external memory encrypt and decrypt clo
ck. 1: close gating, 0: open clock gating..*/
#define EXTMEM_CLK_FORCE_ON_CRYPT    (BIT(2))
#define EXTMEM_CLK_FORCE_ON_CRYPT_M  (BIT(2))
#define EXTMEM_CLK_FORCE_ON_CRYPT_V  0x1
#define EXTMEM_CLK_FORCE_ON_CRYPT_S  2
/* EXTMEM_CLK_FORCE_ON_AUTO_CRYPT : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of automatic crypt clock. 1: close gating,
 0: open clock gating..*/
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT    (BIT(1))
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_M  (BIT(1))
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_V  0x1
#define EXTMEM_CLK_FORCE_ON_AUTO_CRYPT_S  1
/* EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to close clock gating of manual crypt clock. 1: close gating, 0:
 open clock gating..*/
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT    (BIT(0))
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_M  (BIT(0))
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_V  0x1
#define EXTMEM_CLK_FORCE_ON_MANUAL_CRYPT_S  0

#define EXTMEM_CACHE_BRIDGE_ARBITER_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x13C)
/* EXTMEM_ALLOC_WB_HOLD_ARBITER : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Reserved..*/
#define EXTMEM_ALLOC_WB_HOLD_ARBITER    (BIT(0))
#define EXTMEM_ALLOC_WB_HOLD_ARBITER_M  (BIT(0))
#define EXTMEM_ALLOC_WB_HOLD_ARBITER_V  0x1
#define EXTMEM_ALLOC_WB_HOLD_ARBITER_S  0

#define EXTMEM_CACHE_PRELOAD_INT_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x140)
/* EXTMEM_DCACHE_PRELOAD_INT_CLR : WOD ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to clear the interrupt by dcache pre-load done..*/
#define EXTMEM_DCACHE_PRELOAD_INT_CLR    (BIT(5))
#define EXTMEM_DCACHE_PRELOAD_INT_CLR_M  (BIT(5))
#define EXTMEM_DCACHE_PRELOAD_INT_CLR_V  0x1
#define EXTMEM_DCACHE_PRELOAD_INT_CLR_S  5
/* EXTMEM_DCACHE_PRELOAD_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt by dcache pre-load done..*/
#define EXTMEM_DCACHE_PRELOAD_INT_ENA    (BIT(4))
#define EXTMEM_DCACHE_PRELOAD_INT_ENA_M  (BIT(4))
#define EXTMEM_DCACHE_PRELOAD_INT_ENA_V  0x1
#define EXTMEM_DCACHE_PRELOAD_INT_ENA_S  4
/* EXTMEM_DCACHE_PRELOAD_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate the interrupt by dcache pre-load done..*/
#define EXTMEM_DCACHE_PRELOAD_INT_ST    (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_INT_ST_M  (BIT(3))
#define EXTMEM_DCACHE_PRELOAD_INT_ST_V  0x1
#define EXTMEM_DCACHE_PRELOAD_INT_ST_S  3
/* EXTMEM_ICACHE_PRELOAD_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear the interrupt by  icache pre-load done..*/
#define EXTMEM_ICACHE_PRELOAD_INT_CLR    (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_M  (BIT(2))
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_V  0x1
#define EXTMEM_ICACHE_PRELOAD_INT_CLR_S  2
/* EXTMEM_ICACHE_PRELOAD_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt by  icache pre-load done..*/
#define EXTMEM_ICACHE_PRELOAD_INT_ENA    (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_V  0x1
#define EXTMEM_ICACHE_PRELOAD_INT_ENA_S  1
/* EXTMEM_ICACHE_PRELOAD_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate the interrupt by  icache pre-load done..*/
#define EXTMEM_ICACHE_PRELOAD_INT_ST    (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_INT_ST_M  (BIT(0))
#define EXTMEM_ICACHE_PRELOAD_INT_ST_V  0x1
#define EXTMEM_ICACHE_PRELOAD_INT_ST_S  0

#define EXTMEM_CACHE_SYNC_INT_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x144)
/* EXTMEM_DCACHE_SYNC_INT_CLR : WOD ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to clear the interrupt by dcache sync done..*/
#define EXTMEM_DCACHE_SYNC_INT_CLR    (BIT(5))
#define EXTMEM_DCACHE_SYNC_INT_CLR_M  (BIT(5))
#define EXTMEM_DCACHE_SYNC_INT_CLR_V  0x1
#define EXTMEM_DCACHE_SYNC_INT_CLR_S  5
/* EXTMEM_DCACHE_SYNC_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt by dcache sync done..*/
#define EXTMEM_DCACHE_SYNC_INT_ENA    (BIT(4))
#define EXTMEM_DCACHE_SYNC_INT_ENA_M  (BIT(4))
#define EXTMEM_DCACHE_SYNC_INT_ENA_V  0x1
#define EXTMEM_DCACHE_SYNC_INT_ENA_S  4
/* EXTMEM_DCACHE_SYNC_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate the interrupt by dcache sync done..*/
#define EXTMEM_DCACHE_SYNC_INT_ST    (BIT(3))
#define EXTMEM_DCACHE_SYNC_INT_ST_M  (BIT(3))
#define EXTMEM_DCACHE_SYNC_INT_ST_V  0x1
#define EXTMEM_DCACHE_SYNC_INT_ST_S  3
/* EXTMEM_ICACHE_SYNC_INT_CLR : WOD ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to clear the interrupt by  icache sync done..*/
#define EXTMEM_ICACHE_SYNC_INT_CLR    (BIT(2))
#define EXTMEM_ICACHE_SYNC_INT_CLR_M  (BIT(2))
#define EXTMEM_ICACHE_SYNC_INT_CLR_V  0x1
#define EXTMEM_ICACHE_SYNC_INT_CLR_S  2
/* EXTMEM_ICACHE_SYNC_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt by  icache sync done..*/
#define EXTMEM_ICACHE_SYNC_INT_ENA    (BIT(1))
#define EXTMEM_ICACHE_SYNC_INT_ENA_M  (BIT(1))
#define EXTMEM_ICACHE_SYNC_INT_ENA_V  0x1
#define EXTMEM_ICACHE_SYNC_INT_ENA_S  1
/* EXTMEM_ICACHE_SYNC_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to indicate the interrupt by  icache sync done..*/
#define EXTMEM_ICACHE_SYNC_INT_ST    (BIT(0))
#define EXTMEM_ICACHE_SYNC_INT_ST_M  (BIT(0))
#define EXTMEM_ICACHE_SYNC_INT_ST_V  0x1
#define EXTMEM_ICACHE_SYNC_INT_ST_S  0

#define EXTMEM_CACHE_MMU_OWNER_REG          (DR_REG_EXTMEM_BASE + 0x148)
/* EXTMEM_CACHE_MMU_OWNER : R/W ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: The bits are used to specify the owner of MMU.bit0: icache, bit1: dcache, bit2:
dma, bit3: reserved..*/
#define EXTMEM_CACHE_MMU_OWNER    0x00FFFFFF
#define EXTMEM_CACHE_MMU_OWNER_M  ((EXTMEM_CACHE_MMU_OWNER_V)<<(EXTMEM_CACHE_MMU_OWNER_S))
#define EXTMEM_CACHE_MMU_OWNER_V  0xFFFFFF
#define EXTMEM_CACHE_MMU_OWNER_S  0

#define EXTMEM_CACHE_CONF_MISC_REG          (DR_REG_EXTMEM_BASE + 0x14C)
/* EXTMEM_CACHE_TRACE_ENA : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to enable cache trace function..*/
#define EXTMEM_CACHE_TRACE_ENA    (BIT(2))
#define EXTMEM_CACHE_TRACE_ENA_M  (BIT(2))
#define EXTMEM_CACHE_TRACE_ENA_V  0x1
#define EXTMEM_CACHE_TRACE_ENA_S  2
/* EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: The bit is used to disable checking mmu entry fault by sync operation..*/
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT    (BIT(1))
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_M  (BIT(1))
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_V  0x1
#define EXTMEM_CACHE_IGNORE_SYNC_MMU_ENTRY_FAULT_S  1
/* EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to disable checking mmu entry fault by preload operation..*/
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT    (BIT(0))
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_M  (BIT(0))
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_V  0x1
#define EXTMEM_CACHE_IGNORE_PRELOAD_MMU_ENTRY_FAULT_S  0

#define EXTMEM_DCACHE_FREEZE_REG          (DR_REG_EXTMEM_BASE + 0x150)
/* EXTMEM_DCACHE_FREEZE_DONE : RO ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to indicate dcache freeze success.*/
#define EXTMEM_DCACHE_FREEZE_DONE    (BIT(2))
#define EXTMEM_DCACHE_FREEZE_DONE_M  (BIT(2))
#define EXTMEM_DCACHE_FREEZE_DONE_V  0x1
#define EXTMEM_DCACHE_FREEZE_DONE_S  2
/* EXTMEM_DCACHE_FREEZE_MODE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to configure freeze mode, 0:  assert busy if CPU miss 1: assert
hit if CPU miss.*/
#define EXTMEM_DCACHE_FREEZE_MODE    (BIT(1))
#define EXTMEM_DCACHE_FREEZE_MODE_M  (BIT(1))
#define EXTMEM_DCACHE_FREEZE_MODE_V  0x1
#define EXTMEM_DCACHE_FREEZE_MODE_S  1
/* EXTMEM_DCACHE_FREEZE_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable dcache freeze mode.*/
#define EXTMEM_DCACHE_FREEZE_ENA    (BIT(0))
#define EXTMEM_DCACHE_FREEZE_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_FREEZE_ENA_V  0x1
#define EXTMEM_DCACHE_FREEZE_ENA_S  0

#define EXTMEM_ICACHE_FREEZE_REG          (DR_REG_EXTMEM_BASE + 0x154)
/* EXTMEM_ICACHE_FREEZE_DONE : RO ;bitpos:[2] ;default: 1'b1 ; */
/*description: The bit is used to indicate icache freeze success.*/
#define EXTMEM_ICACHE_FREEZE_DONE    (BIT(2))
#define EXTMEM_ICACHE_FREEZE_DONE_M  (BIT(2))
#define EXTMEM_ICACHE_FREEZE_DONE_V  0x1
#define EXTMEM_ICACHE_FREEZE_DONE_S  2
/* EXTMEM_ICACHE_FREEZE_MODE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to configure freeze mode, 0:  assert busy if CPU miss 1: assert
hit if CPU miss.*/
#define EXTMEM_ICACHE_FREEZE_MODE    (BIT(1))
#define EXTMEM_ICACHE_FREEZE_MODE_M  (BIT(1))
#define EXTMEM_ICACHE_FREEZE_MODE_V  0x1
#define EXTMEM_ICACHE_FREEZE_MODE_S  1
/* EXTMEM_ICACHE_FREEZE_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable icache freeze mode.*/
#define EXTMEM_ICACHE_FREEZE_ENA    (BIT(0))
#define EXTMEM_ICACHE_FREEZE_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_FREEZE_ENA_V  0x1
#define EXTMEM_ICACHE_FREEZE_ENA_S  0

#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_REG          (DR_REG_EXTMEM_BASE + 0x158)
/* EXTMEM_ICACHE_ATOMIC_OPERATE_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to activate icache atomic operation protection. In this case, sy
nc/lock operation can not interrupt miss-work. This feature does not work during
 invalidateAll operation..*/
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA    (BIT(0))
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_M  (BIT(0))
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_V  0x1
#define EXTMEM_ICACHE_ATOMIC_OPERATE_ENA_S  0

#define EXTMEM_DCACHE_ATOMIC_OPERATE_ENA_REG          (DR_REG_EXTMEM_BASE + 0x15C)
/* EXTMEM_DCACHE_ATOMIC_OPERATE_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to activate dcache atomic operation protection. In this case, sy
nc/lock/occupy operation can not interrupt miss-work. This feature does not work
 during invalidateAll operation..*/
#define EXTMEM_DCACHE_ATOMIC_OPERATE_ENA    (BIT(0))
#define EXTMEM_DCACHE_ATOMIC_OPERATE_ENA_M  (BIT(0))
#define EXTMEM_DCACHE_ATOMIC_OPERATE_ENA_V  0x1
#define EXTMEM_DCACHE_ATOMIC_OPERATE_ENA_S  0

#define EXTMEM_CACHE_REQUEST_REG          (DR_REG_EXTMEM_BASE + 0x160)
/* EXTMEM_CACHE_REQUEST_BYPASS : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to disable request recording which could cause performance issue.*/
#define EXTMEM_CACHE_REQUEST_BYPASS    (BIT(0))
#define EXTMEM_CACHE_REQUEST_BYPASS_M  (BIT(0))
#define EXTMEM_CACHE_REQUEST_BYPASS_V  0x1
#define EXTMEM_CACHE_REQUEST_BYPASS_S  0

#define EXTMEM_CLOCK_GATE_REG          (DR_REG_EXTMEM_BASE + 0x164)
/* EXTMEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: Reserved..*/
#define EXTMEM_CLK_EN    (BIT(0))
#define EXTMEM_CLK_EN_M  (BIT(0))
#define EXTMEM_CLK_EN_V  0x1
#define EXTMEM_CLK_EN_S  0

#define EXTMEM_CACHE_TAG_OBJECT_CTRL_REG          (DR_REG_EXTMEM_BASE + 0x180)
/* EXTMEM_DCACHE_TAG_OBJECT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to set dcache tag memory as object. This bit should be onehot with
the others fields inside this register..*/
#define EXTMEM_DCACHE_TAG_OBJECT    (BIT(1))
#define EXTMEM_DCACHE_TAG_OBJECT_M  (BIT(1))
#define EXTMEM_DCACHE_TAG_OBJECT_V  0x1
#define EXTMEM_DCACHE_TAG_OBJECT_S  1
/* EXTMEM_ICACHE_TAG_OBJECT : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to set icache tag memory as object. This bit should be onehot with
the others fields inside this register..*/
#define EXTMEM_ICACHE_TAG_OBJECT    (BIT(0))
#define EXTMEM_ICACHE_TAG_OBJECT_M  (BIT(0))
#define EXTMEM_ICACHE_TAG_OBJECT_V  0x1
#define EXTMEM_ICACHE_TAG_OBJECT_S  0

#define EXTMEM_CACHE_TAG_WAY_OBJECT_REG          (DR_REG_EXTMEM_BASE + 0x184)
/* EXTMEM_CACHE_TAG_WAY_OBJECT : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: Set this bits to select which way of the tag-object will be accessed. 0: way0, 1
: way1, 2: way2, 3: way3, .., 7: way7..*/
#define EXTMEM_CACHE_TAG_WAY_OBJECT    0x00000007
#define EXTMEM_CACHE_TAG_WAY_OBJECT_M  ((EXTMEM_CACHE_TAG_WAY_OBJECT_V)<<(EXTMEM_CACHE_TAG_WAY_OBJECT_S))
#define EXTMEM_CACHE_TAG_WAY_OBJECT_V  0x7
#define EXTMEM_CACHE_TAG_WAY_OBJECT_S  0

#define EXTMEM_CACHE_VADDR_REG          (DR_REG_EXTMEM_BASE + 0x188)
/* EXTMEM_CACHE_VADDR : R/W ;bitpos:[31:0] ;default: 32'h00000000 ; */
/*description: Those bits stores the virtual address which will decide where inside the specifi
ed tag memory object will be accessed..*/
#define EXTMEM_CACHE_VADDR    0xFFFFFFFF
#define EXTMEM_CACHE_VADDR_M  ((EXTMEM_CACHE_VADDR_V)<<(EXTMEM_CACHE_VADDR_S))
#define EXTMEM_CACHE_VADDR_V  0xFFFFFFFF
#define EXTMEM_CACHE_VADDR_S  0

#define EXTMEM_CACHE_TAG_CONTENT_REG          (DR_REG_EXTMEM_BASE + 0x18C)
/* EXTMEM_CACHE_TAG_CONTENT : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This is a constant place where we can write data to or read data from the tag me
mory on the specified cache..*/
#define EXTMEM_CACHE_TAG_CONTENT    0xFFFFFFFF
#define EXTMEM_CACHE_TAG_CONTENT_M  ((EXTMEM_CACHE_TAG_CONTENT_V)<<(EXTMEM_CACHE_TAG_CONTENT_S))
#define EXTMEM_CACHE_TAG_CONTENT_V  0xFFFFFFFF
#define EXTMEM_CACHE_TAG_CONTENT_S  0

#define EXTMEM_DATE_REG          (DR_REG_EXTMEM_BASE + 0x3FC)
/* EXTMEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h2012310 ; */
/*description: version information..*/
#define EXTMEM_DATE    0x0FFFFFFF
#define EXTMEM_DATE_M  ((EXTMEM_DATE_V)<<(EXTMEM_DATE_S))
#define EXTMEM_DATE_V  0xFFFFFFF
#define EXTMEM_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_EXTMEM_REG_H_ */
