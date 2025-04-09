/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/soc.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "esp_private/panic_reason.h"

#include "riscv/csr.h"
#include "riscv/encoding.h"
#include "riscv/rvruntime-frames.h"

#include "esp_tee.h"
#include "panic_helper.h"
#include "sdkconfig.h"

void panic_print_backtrace(const void *f, int depth)
{
    // Basic backtrace
    tee_panic_print("\r\nStack memory\r\n");
    uint32_t sp = (uint32_t)((RvExcFrame *)f)->sp;
    const int per_line = 8;
    for (int x = 0; x < depth; x += per_line * sizeof(uint32_t)) {
        uint32_t *spp = (uint32_t *)(sp + x);
        tee_panic_print("0x%08x: ", sp + x);
        for (int y = 0; y < per_line; y++) {
            tee_panic_print("0x%08x%s", spp[y], y == per_line - 1 ? "\r\n" : " ");
        }
    }
}

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

    tee_panic_print("\nCore %d register dump:", ((RvExcFrame *)f)->mhartid);

    for (int x = 0; x < sizeof(desc) / sizeof(desc[0]); x += 4) {
        tee_panic_print("\r\n");
        for (int y = 0; y < 4 && x + y < sizeof(desc) / sizeof(desc[0]); y++) {
            if (desc[x + y][0] != 0) {
                tee_panic_print("%s: 0x%08x  ", desc[x + y], regs[x + y]);
            }
        }
    }

#if CONFIG_SECURE_TEE_TEST_MODE
    struct {
        const char *name;
        uint32_t value;
    } csr_regs[] = {
        { "MIE     ", RV_READ_CSR(mie)      },
        { "MIP     ", RV_READ_CSR(mip)      },
        { "MSCRATCH", RV_READ_CSR(mscratch) },
        { "UEPC    ", RV_READ_CSR(uepc)     },
        { "USTATUS ", RV_READ_CSR(ustatus)  },
        { "UTVEC   ", RV_READ_CSR(utvec)    },
        { "UCAUSE  ", RV_READ_CSR(ucause)   },
        { "UTVAL   ", RV_READ_CSR(utval)    },
        { "UIE     ", RV_READ_CSR(uie)      },
        { "UIP     ", RV_READ_CSR(uip)      },
    };

    tee_panic_print("\n\n");

    for (int i = 0; i < sizeof(csr_regs) / sizeof(csr_regs[0]); i++) {
        tee_panic_print("%s: 0x%08x  ", csr_regs[i].name, csr_regs[i].value);
        if ((i + 1) % 4 == 0 || i == sizeof(csr_regs) / sizeof(csr_regs[0]) - 1) {
            tee_panic_print("\n");
        }
    }
#endif
}

void panic_print_rsn(const void *f, int core, const char *rsn)
{
    const RvExcFrame *regs = (const RvExcFrame *)f;
    const void *addr = (const void *)regs->mepc;

    tee_panic_print("Guru Meditation Error: Core %d panic'ed (%s). Exception was unhandled.\n", core, rsn);
    tee_panic_print("Fault addr: %p | Origin: %s\n", addr, (regs->mstatus & MSTATUS_MPP) ? "M-mode" : "U-mode");
}

void panic_print_exccause(const void *f, int core)
{
    const RvExcFrame *regs = (const RvExcFrame *)f;

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

    const char *rsn = NULL;
    if (regs->mcause < (sizeof(reason) / sizeof(reason[0]))) {
        if (reason[regs->mcause] != NULL) {
            rsn = (reason[regs->mcause]);
        }
    }

    panic_print_rsn(f, core, rsn);
}

void panic_print_isrcause(const void *f, int core)
{
    const RvExcFrame *regs = (const RvExcFrame *)f;
    const char *rsn = "Unknown reason";

    switch (regs->mcause) {
    case ETS_CACHEERR_INUM:
        rsn = "Cache error";
        break;
    case PANIC_RSN_INTWDT_CPU0:
        rsn = "Interrupt wdt timeout on CPU0";
        break;
#if SOC_CPU_CORES_NUM > 1
    case PANIC_RSN_INTWDT_CPU1:
        rsn = "Interrupt wdt timeout on CPU1";
        break;
#endif
    }

    panic_print_rsn(f, core, rsn);
}
