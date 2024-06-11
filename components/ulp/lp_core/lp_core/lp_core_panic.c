/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include <stdint.h>
#include <stddef.h>
#include "ulp_lp_core_print.h"
#include "riscv/rvruntime-frames.h"

#if CONFIG_ULP_PANIC_OUTPUT_ENABLE

static void dump_stack(RvExcFrame *frame, int exccause)
{
    uint32_t i = 0;
    uint32_t sp = frame->sp;
    lp_core_printf("\n\nStack memory:\n");
    const int per_line = 8;
    for (i = 0; i < 1024; i += per_line * sizeof(uint32_t)) {
        uint32_t *spp = (uint32_t *)(sp + i);
        lp_core_printf("%08x: ", sp + i);
        for (int y = 0; y < per_line; y++) {
            lp_core_printf("0x%08x%c", spp[y], y == per_line - 1 ? '\n' : ' ');
        }
    }
    lp_core_printf("\n");
}

static const char *desc[] = {
    "MEPC    ", "RA      ", "SP      ", "GP      ", "TP      ", "T0      ", "T1      ", "T2      ",
    "S0/FP   ", "S1      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
    "A6      ", "A7      ", "S2      ", "S3      ", "S4      ", "S5      ", "S6      ", "S7      ",
    "S8      ", "S9      ", "S10     ", "S11     ", "T3      ", "T4      ", "T5      ", "T6      ",
    "MSTATUS ", "MTVEC   ", "MCAUSE  ", "MTVAL   ", "MHARTID "
};

static const char *reason[] = {
    NULL,
    NULL,
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store address misaligned",
    "Store access fault",
};

void ulp_lp_core_panic_handler(RvExcFrame *frame, int exccause)
{
#define DIM(arr) (sizeof(arr)/sizeof(*arr))

    const char *exccause_str = "Unhandled interrupt/Unknown cause";

    if (exccause < DIM(reason) && reason[exccause] != NULL) {
        exccause_str = reason[exccause];
    }

    lp_core_printf("Guru Meditation Error: LP Core panic'ed (%s)\n", exccause_str);
    lp_core_printf("Core 0 register dump:\n");

    uint32_t* frame_ints = (uint32_t*) frame;
    for (int x = 0; x < DIM(desc); x++) {
        if (desc[x][0] != 0) {
            const int not_last = (x + 1) % 4;
            lp_core_printf("%-8s: 0x%08x %c", desc[x], frame_ints[x], not_last ? ' ' : '\n');
        }
    }

    dump_stack(frame, exccause);

    /* idf-monitor uses this string to mark the end of a panic dump */
    lp_core_printf("ELF file SHA256: No SHA256 Embedded\n");

    while (1) {
    }
}

#endif //#if CONFIG_ULP_PANIC_OUTPUT_ENABLE
