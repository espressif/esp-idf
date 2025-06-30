/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "esp_macros.h"
#include "esp_rom_sys.h"
#include "esp_rom_serial_output.h"

#include "riscv/csr.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/vectors_const.h"
#include "test_pms_priv.h"

#define panic_print(format, ...) esp_rom_printf(DRAM_STR(format), ##__VA_ARGS__)

void test_panicHandler(RvExcFrame *frame, int exccause)
{
    const RvExcFrame *regs = (const RvExcFrame *)frame;
    const void *addr = (const void *)regs->mepc;

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

    const char *rsn = "Unknown";
    uint32_t mcause = regs->mcause & (VECTORS_MCAUSE_INTBIT_MASK | VECTORS_MCAUSE_REASON_MASK);
    if (mcause < (sizeof(reason) / sizeof(reason[0]))) {
        if (reason[mcause] != NULL) {
            rsn = (reason[mcause]);
        }
    }

    if (mcause == MCAUSE_ILLEGAL_INSTRUCTION) {
        uint32_t mscratch = RV_READ_CSR(mscratch);
        /* Check if panic is from the interrupt or PMS related test-cases */
        if (mscratch == 0xDEADC0DE) {
            frame->mepc += 0x04U;
            RV_WRITE_CSR(mscratch, 0x00U);
        } else {
            frame->mepc = frame->ra;
        }
        return;
    }
    /* PERI_APM access fault */
#if SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
    if (regs->mtval >= SOC_PERIPHERAL_LOW && regs->mtval < SOC_PERIPHERAL_HIGH) {
        if (mcause == MCAUSE_LOAD_ACCESS_FAULT || mcause == MCAUSE_STORE_ACCESS_FAULT) {
            panic_print("[PERI_APM] %s at 0x%08x\n\r", rsn, regs->mtval);
            frame->mepc += 0x04U;
            RV_WRITE_CSR(uscratch, regs->mtval);
            return;
        }
    }
#endif

    panic_print("Guru Meditation Error: Core %d panic'ed (%s). Exception was unhandled.\n", regs->mhartid, rsn);
    panic_print("Origin : %s\n\r", (regs->mstatus & MSTATUS_MPP) ? "M-mode" : "U-mode");
    panic_print("MEPC   : %p | MCAUSE : 0x%08x\n\r", addr, mcause);
    panic_print("MTVAL  : 0x%08x | MSTATUS: 0x%08x\n\r", regs->mtval, regs->mstatus);
    panic_print("MTVEC  : 0x%08x\n\r", regs->mtvec);
    panic_print("Rebooting...\r\n");

    if (CONFIG_ESP_CONSOLE_UART_NUM >= 0) {
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    }
    esp_rom_software_reset_system();
    ESP_INFINITE_LOOP();
}
