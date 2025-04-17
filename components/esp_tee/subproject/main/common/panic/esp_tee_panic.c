/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_cpu.h"
#include "esp_attr.h"
#include "esp_macros.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"
#include "rom/cache.h"

#include "riscv/rv_utils.h"
#include "riscv/rvruntime-frames.h"

#include "hal/apm_hal.h"

#include "esp_tee.h"
#include "esp_tee_apm_intr.h"
#include "esp_tee_rv_utils.h"

#include "panic_helper.h"
#include "sdkconfig.h"

#define RV_FUNC_STK_SZ    (32)

static void tee_panic_end(void)
{
    // Disable interrupts
    rv_utils_tee_intr_global_disable();

    // Disable the cache
    Cache_Disable_ICache();

    // Clear the interrupt controller configurations
    memset((void *)DR_REG_PLIC_MX_BASE, 0x00, (PLIC_MXINT_CLAIM_REG + 4 - DR_REG_PLIC_MX_BASE));
    memset((void *)DR_REG_PLIC_UX_BASE, 0x00, (PLIC_UXINT_CLAIM_REG + 4 - DR_REG_PLIC_UX_BASE));

    // Make sure all the panic handler output is sent from UART FIFO
    if (CONFIG_ESP_CONSOLE_UART_NUM >= 0) {
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    }

    // Generate system reset
    esp_rom_software_reset_system();
}

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    tee_panic_print("Assert failed in %s, %s:%d (%s)\r\n", func, file, line, expr);
    tee_panic_print("\n\n");

    tee_panic_end();
    ESP_INFINITE_LOOP();
}

void abort(void)
{
#if !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
    tee_panic_print("abort() was called at PC 0x%08x\r\n\n", (intptr_t)__builtin_return_address(0) - 3);
    tee_panic_print("\n\n");
#endif

    tee_panic_end();
    ESP_INFINITE_LOOP();
}

static void panic_print_info(const void *frame, int core)
{
    panic_print_registers((const void *)frame, core);
    tee_panic_print("\n");
    panic_print_backtrace((const void *)frame, 100);
    tee_panic_print("\n");
    tee_panic_print("Rebooting...\r\n\n");

    tee_panic_end();
}

static void panic_handler(void *frame, bool pseudo_exccause)
{
    int fault_core = esp_cpu_get_core_id();
    if (pseudo_exccause) {
        panic_print_isrcause((const void *)frame, fault_core);
    } else {
        panic_print_exccause((const void *)frame, fault_core);
    }

    panic_print_info((const void *)frame, fault_core);
    ESP_INFINITE_LOOP();
}

void tee_panic_from_exc(void *frame)
{
    panic_handler(frame, false);
}

void tee_panic_from_isr(void *frame)
{
    panic_handler(frame, true);
}

void tee_apm_violation_isr(void *arg)
{
    intptr_t exc_sp = RV_READ_CSR(mscratch);
    RvExcFrame *frame = (RvExcFrame *)exc_sp;

    apm_hal_ctrl_info_t *ctrl_info = (apm_hal_ctrl_info_t *)arg;
    apm_ctrl_exception_info_t excp_info = {};

    apm_hal_get_exception_info(ctrl_info, &excp_info);
    apm_hal_clear_exception_status(ctrl_info);

    int fault_core = esp_cpu_get_core_id();
    panic_print_rsn((const void *)frame, fault_core, esp_tee_apm_excp_type_to_str(excp_info.type));

    tee_panic_print("Access addr: 0x%x | Mode: %s\n", excp_info.addr, esp_tee_apm_excp_mode_to_str(excp_info.mode));
    tee_panic_print("Module: %s | Path: %d\n", esp_tee_apm_excp_ctrl_to_str(ctrl_info->ctrl_mod), ctrl_info->path);
    tee_panic_print("Master: %s | Region: %d\n", esp_tee_apm_excp_mid_to_str(excp_info.id), (excp_info.regn == 0) ? 0 : (__builtin_ffs(excp_info.regn) - 1));

    panic_print_info((const void *)frame, fault_core);
    ESP_INFINITE_LOOP();
}
