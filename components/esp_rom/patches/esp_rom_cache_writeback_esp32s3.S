/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <xtensa/corebits.h>
#include <sdkconfig.h>
#include "soc/extmem_reg.h"

/**
  * @brief Write back the cache items of DCache, enable cache freeze during writeback.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        If the region is not in DCache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  * @param  uint32_t addr: start address to write back
  * @param  uint32_t items: cache lines to invalidate, items * cache_line_size should
  *         not exceed the bus address size(4MB)
  *
  * void cache_writeback_items_freeze(uint32_t addr, uint32_t items)
*/

/*******************************************************************************

This function is a cache write-back function that works around the following
hardware errata on the ESP32-S3:

- Core X manually triggers (via the EXTMEM_DCACHE_SYNC_CTRL_REG register) the
write-back of one or more cache lines.
- While the write-back is in progress, there are two scenarios that may cause
cache hit error.
    - Core X enters the interrupt handler and access the same cache line
      being written back.
    - Core Y access the same cache line being written back.

To workaround this errata, the following steps must be taken when manually
triggering a cache write-back:

- Core X must disable interrupts so that it cannot be preempted
- Core X must freeze the cache (via the EXTMEM_DCACHE_FREEZE_REG register) to
prevent Core Y from accessing the same cache lines that are about to be written
back.
- Core X now triggers the cache write-back. During the write-back...
    - If Core Y attempts the access any address in the cache region, Core Y will
    busy wait until the cache is unfrozen.
    - Core X must ensure that it does not access any address in the cache region,
    otherwise Core X will busy wait thus causing a deadlock.
- After the write-back is complete, Core X unfreezes the cache, and reenables
interrupts.

Notes:

- Please do not modify this function, it must strictly follow the current execution
sequence, otherwise it may cause unexpected errors.
- This function is written in assmebly to ensure that the function itself never
accesses any cache address while the cache is frozen. Unexpected cache access
could occur if...
    - the function triggers an window overflow onto a stack placed in PSRAM.
    Thus, we only use two window panes (a0 to a8) in this function and trigger
    all window overflows before freezing the cache.
    - the function accesses literals/read-only variables placed in Flash.

*******************************************************************************/

    .align  4
    /*
    Create dedicated literal pool for this function. Mostly used to store out
    of range movi transformations.
    */
    .literal_position
    .global cache_writeback_items_freeze
    .type   cache_writeback_items_freeze, @function
cache_writeback_items_freeze:
    entry   sp, 32

    /* REG_WRITE(EXTMEM_DCACHE_SYNC_ADDR_REG, addr); */
    movi a4, EXTMEM_DCACHE_SYNC_ADDR_REG
    s32i a2, a4, 0
    /* REG_WRITE(EXTMEM_DCACHE_SYNC_SIZE_REG, items); */
    movi a4, EXTMEM_DCACHE_SYNC_SIZE_REG
    s32i a3, a4, 0
    memw    /* About to freeze the cache. Ensure all previous memory R/W are completed */

    movi a2, EXTMEM_DCACHE_FREEZE_REG
    movi a3, EXTMEM_DCACHE_SYNC_CTRL_REG

    /*
    REG_CLR_BIT(EXTMEM_DCACHE_FREEZE_REG, EXTMEM_DCACHE_FREEZE_MODE);
    REG_SET_BIT(EXTMEM_DCACHE_FREEZE_REG, EXTMEM_DCACHE_FREEZE_ENA);
    */
    l32i a4, a2, 0  /* a4 = *(EXTMEM_DCACHE_FREEZE_REG) */
    movi a5, ~(EXTMEM_DCACHE_FREEZE_MODE_M)
    and a4, a4, a5
    movi a5, EXTMEM_DCACHE_FREEZE_ENA_M
    or a4, a4, a5
    s32i a4, a2, 0  /* *(EXTMEM_DCACHE_FREEZE_REG) = a4 */

    /* while (!REG_GET_BIT(EXTMEM_DCACHE_FREEZE_REG, EXTMEM_DCACHE_FREEZE_DONE)); */
    movi a5, EXTMEM_DCACHE_FREEZE_DONE_M
_wait_freeze_done:
    l32i a4, a2, 0  /* a4 = *(EXTMEM_DCACHE_FREEZE_REG) */
    memw
    bnone a4, a5, _wait_freeze_done

    /* REG_SET_BIT(EXTMEM_DCACHE_SYNC_CTRL_REG, EXTMEM_DCACHE_WRITEBACK_ENA); */
    l32i a4, a3, 0  /* a4 = *(EXTMEM_DCACHE_SYNC_CTRL_REG) */
    movi a5, EXTMEM_DCACHE_WRITEBACK_ENA_M
    or a4, a4, a5
    s32i a4, a3, 0  /* *(EXTMEM_DCACHE_SYNC_CTRL_REG) = a4 */

    /* while(!REG_GET_BIT(EXTMEM_DCACHE_SYNC_CTRL_REG, EXTMEM_DCACHE_SYNC_DONE)); */
    movi a5, EXTMEM_DCACHE_SYNC_DONE_M
_wait_writeback_done:
    l32i a4, a3, 0  /* a4 = *(EXTMEM_DCACHE_SYNC_CTRL_REG) */
    memw
    bnone a4, a5, _wait_writeback_done

    /* REG_CLR_BIT(EXTMEM_DCACHE_FREEZE_REG, EXTMEM_DCACHE_FREEZE_ENA); */
    l32i a4, a2, 0  /* a4 = *(EXTMEM_DCACHE_FREEZE_REG) */
    movi a5, ~(EXTMEM_DCACHE_FREEZE_ENA_M)
    and a4, a4, a5
    s32i a4, a2, 0  /* *(EXTMEM_DCACHE_FREEZE_REG) = a4 */

    /* while (REG_GET_BIT(EXTMEM_DCACHE_FREEZE_REG, EXTMEM_DCACHE_FREEZE_DONE)); */
    movi a5, EXTMEM_DCACHE_FREEZE_DONE_M
_wait_unfreeze_done:
    l32i a4, a2, 0  /* a4 = *(EXTMEM_DCACHE_FREEZE_REG) */
    memw
    bany a4, a5, _wait_unfreeze_done

    retw
    .size   cache_writeback_items_freeze, . - cache_writeback_items_freeze
