
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
#include <stdio.h>

#include "esp_private/panic_internal.h"
#include "riscv/rvruntime-frames.h"

extern void esp_panic_handler(panic_info_t *);

void panic_print_registers(const void *f, int core)
{
    uint32_t *regs = (uint32_t *)f;

    // only print ABI name
    const char *desc[] = {
        "MEPC    ", "RA      ", "SP      ", "GP      ", "TP      ", "T0      ", "T1      ", "T2      ",
        "S0/FP   ", "S1      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
        "A6      ", "A7      ", "S2      ", "S3      ", "S4      ", "S5      ", "S6      ", "S7      ",
        "S8      ", "S9      ", "S10     ", "S11     ", "T3      ", "T4      ", "T5      ", "T6      ",
        "MSTATUS ", "MTVEC   ", "MCAUSE  ", "MTVAL   ", "MHARTID "
    };

    panic_print_str("Core ");
    panic_print_dec(((RvExcFrame *)f)->mhartid);
    panic_print_str(" register dump:");

    for (int x = 0; x < sizeof(desc) / sizeof(desc[0]); x += 4) {
        panic_print_str("\r\n");
        for (int y = 0; y < 4 && x + y < sizeof(desc) / sizeof(desc[0]); y++) {
            if (desc[x + y][0] != 0) {
                panic_print_str(desc[x + y]);
                panic_print_str(": 0x");
                panic_print_hex(regs[x + y]);
                panic_print_str("  ");
            }
        }
    }
}

void panic_soc_fill_info(void *f, panic_info_t *info)
{
    // TODO ESP32-C3 IDF-2386 / support soc panic
    return;
}

void panic_arch_fill_info(void *frame, panic_info_t *info)
{
    RvExcFrame *regs = (RvExcFrame *) frame;
    info->core = 0;
    info->exception = PANIC_EXCEPTION_FAULT;

    //Please keep in sync with PANIC_RSN_* defines
    static const char *reason[] = {
        "Instruction address misaligned",
        "Instruction access fault",
        "Illegal instruction",
        "Breakpoint",
        "Load address misaligned",
        "Load access fault",
        "Store address misaligned",
        "Store access fault",
        "Environment call from U-mode",
        "Environment call from S-mode",
        NULL,
        "Environment call from M-mode",
        "Instruction page fault",
        "Load page fault",
        NULL,
        "Store page fault",
    };

    if (regs->mcause < (sizeof(reason) / sizeof(reason[0]))) {
        if (reason[regs->mcause] != NULL) {
            info->reason = (reason[regs->mcause]);
        }
    }

    info->description = "Exception was unhandled.";

    info->addr = (void *) regs->mepc;
    info->frame = &regs;
}

void panic_print_backtrace(const void *frame, int core)
{
    // Basic backtrace
    panic_print_str("\r\nStack memory:\n");
    uint32_t sp = (uint32_t)((RvExcFrame *)frame)->sp;
    const int per_line = 8;
    for (int x = 0; x < 1024; x += per_line * sizeof(uint32_t)) {
        uint32_t *spp = (uint32_t *)(sp + x);
        panic_print_hex(sp + x);
        panic_print_str(": ");
        for (int y = 0; y < per_line; y++) {
            panic_print_str("0x");
            panic_print_hex(spp[y]);
            panic_print_char(y == per_line - 1 ? '\n' : ' ');
        }
    }
}

uint32_t panic_get_address(const void *f)
{
    return ((RvExcFrame *)f)->mepc;
}

uint32_t panic_get_cause(const void *f)
{
    return ((RvExcFrame *)f)->mcause;
}

void panic_set_address(void *f, uint32_t addr)
{
    ((RvExcFrame *)f)->mepc = addr;
}
