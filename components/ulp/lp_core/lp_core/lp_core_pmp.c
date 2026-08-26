/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/soc.h"
#include "riscv/encoding.h"
#include "riscv/csr.h"

#if CONFIG_ULP_LP_CORE_MEMPROT

/*
 * Default PMP layout for the LP-CPU (RISC-V, 16 entries, 128-byte TOR granularity).
 *
 * The LP-CPU always runs in M-mode. PMP entries with the L bit set apply to M-mode.
 * A catch-all entry with L+NONE at the end denies all unmatched accesses.
 *
 * Normal mode (code and data in LP RAM):
 *   Entry 0-1: [SOC_RTC_DRAM_LOW, _lp_text_end)  → RX  (vector table, handlers, .text, .rodata)
 *   Entry 2:   [_lp_text_end, SOC_RTC_DRAM_HIGH)  → RW  (.data, .bss, stack, shared_mem)
 *   Entry 3-4: LP Peripherals                      → RW
 *   Entry 5-6: HP UART (if CONFIG_ULP_HP_UART_CONSOLE_PRINT) → RW
 *
 *   Entry 15 (catch-all) → NONE (deny-by-default for all unmatched regions)
 */

/* Linker-exported PMP boundary symbols (see lp_core_riscv.ld) */
extern int _lp_text_end;

#if CONFIG_ULP_HP_UART_CONSOLE_PRINT
/* Cover UART0 and UART1 (each 0x1000) so CONFIG_ESP_CONSOLE_UART_NUM 0 or 1 works. */
#define HP_UART_PMP_END  (DR_REG_UART1_BASE + 0x1000UL)
#endif

void lp_core_configure_pmp(void)
{
    /*
     * PMP_ENTRY_SET(ENTRY, ADDR, CFG) expands to inline assembly where ENTRY
     * must be a compile-time integer literal — a variable cannot be used.
     * Entry numbers are therefore hardcoded explicitly below.
     */
    const unsigned NONE = PMP_L;
    const unsigned RX   = PMP_L | PMP_R | PMP_X;
    const unsigned RW   = PMP_L | PMP_R | PMP_W;

    /* --- LP RAM text region: vector table + handlers (+ .text/.rodata in normal mode) ---
     * Entry 0: lower TOR bound (LP RAM base), no permissions
     * Entry 1: upper bound = _lp_text_end, permissions = RX */
    PMP_ENTRY_SET(0, SOC_RTC_DRAM_LOW,          NONE);
    PMP_ENTRY_SET(1, (uintptr_t)&_lp_text_end,  PMP_TOR | RX);

    /* --- LP RAM data region: .data/.bss/stack/shared_mem ---
     * Entry 2: upper bound = LP RAM high, permissions = RW */
    PMP_ENTRY_SET(2, SOC_RTC_DRAM_HIGH,         PMP_TOR | RW);

    /* --- LP Peripherals ---
     * Entry 3: lower TOR bound
     * Entry 4: upper bound, permissions = RW */
    PMP_ENTRY_SET(3, SOC_LP_PERIPH_LOW,         NONE);
    PMP_ENTRY_SET(4, SOC_LP_PERIPH_HIGH,        PMP_TOR | RW);

#if CONFIG_ULP_HP_UART_CONSOLE_PRINT
    /* --- HP UART (entries 5-6) --- */
    PMP_ENTRY_SET(5, DR_REG_UART0_BASE,         NONE);
    PMP_ENTRY_SET(6, HP_UART_PMP_END,         PMP_TOR | RW);
#endif

    /* --- Catch-all: deny all unmatched addresses ---
     * Entry 15 covers the entire 32-bit address space with no permissions.
     * Any access that didn't match the entries above is denied. */
    PMP_ENTRY_SET(15, PMPADDR_ALL, PMP_NAPOT | NONE);
}

#endif /* CONFIG_ULP_LP_CORE_MEMPROT */
