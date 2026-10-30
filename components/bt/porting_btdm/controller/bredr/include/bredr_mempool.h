/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "btdm_user_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if UC_BT_CTRL_BR_EDR_IS_ENABLE

/**
 * BR/EDR mempool tracing (debug only).
 * Enable by setting BREDR_MEMPOOL_TRACE to 1; dump via esp_bredr_mempool_dump_stats().
 * Set BREDR_MEMPOOL_TRACE_PC to 1 to also capture one caller PC. Byte counts stay
 * on when TRACE_PC is 0. Counts use the TLSF block size plus alloc overhead.
 *
 * PC capture requires RISC-V (CONFIG_IDF_TARGET_ARCH_RISCV) and a fixed ABI:
 * bredr_mempool_malloc is noinline with RA at CFA-4. That return address is the
 * controller wrapper and is not recorded. The recorded PC is the RA the wrapper
 * saved. The wrapper prologue is addi / c.addi / c.addi16sp, then sw / c.swsp ra
 * within 64 bytes. Other architectures keep byte counts only.
 *
 * @note Wrap callbacks run under the controller HW critical section.
 *       That path does not read flash text. A flash prologue is queued and
 *       decoded from dump, outside the critical section. The first alloc
 *       records a null PC; later allocs use the cached frame. IRAM prologues
 *       are decoded on the alloc path. The RA-save scan is 256 bytes.
 *       The SP alloc is the nearest addi within 64 bytes before that save.
 *       The cache has 16 direct-mapped slots. A scan that cannot read flash
 *       is not cached. Dump prints at most 64 PCs. overflow means the
 *       list is not every live block.
 */
#ifndef BREDR_MEMPOOL_TRACE
#define BREDR_MEMPOOL_TRACE             0
#endif
#ifndef BREDR_MEMPOOL_TRACE_PC
#define BREDR_MEMPOOL_TRACE_PC          0
#endif

struct orca_mempool_ops;

/** @brief Address of the BR/EDR controller mempool operations. */
struct orca_mempool_ops *bredr_mempool_ops_get(void);

#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

#ifdef __cplusplus
}
#endif
