/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdint.h"
#include "soc/interrupt_reg.h"
#include "soc/soc_caps.h"
#include "esp_ipc_isr.h"
#include "esp_private/esp_ipc_isr.h"
#include "esp_private/esp_system_attr.h"
#include "soc/soc.h"
#include "riscv/csr.h"

#if SOC_INT_CLIC_SUPPORTED
#include "esp_private/interrupt_clic.h"
#endif

#define ESP_IPC_ISR_RISCV_INTR_ENABLE_THRESH  1

extern volatile esp_ipc_isr_args_t esp_ipc_isr_stall_args;
extern volatile uint32_t esp_ipc_isr_stall_fl;

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_record_interrupted_context(void)
{
    uint32_t unsafe = 0;

#if SOC_INT_CLIC_SUPPORTED
    if (rv_utils_get_interrupt_threshold() != 0) {
        unsafe = 1;
    }

    uint32_t mpil = (RV_READ_CSR(mcause) >> 16) & 0xff;
    if (BYTE_TO_NLBITS(mpil) != 0) {
        unsafe = 1;
    }
#else
    if (REG_READ(INTERRUPT_CURRENT_CORE_INT_THRESH_REG) > ESP_IPC_ISR_RISCV_INTR_ENABLE_THRESH) {
        unsafe = 1;
    }
#endif

    esp_ipc_isr_stall_args.interrupted_context_unsafe = unsafe;
}

void ESP_SYSTEM_IRAM_ATTR esp_ipc_isr_waiting_for_finish_cmd(void* arg)
{
    esp_ipc_isr_stall_fl = 1;
    while (esp_ipc_isr_stall_args.cmd == ESP_IPC_ISR_CMD_RESET_STATE) {
        if (esp_ipc_isr_stall_args.func != NULL) {
            esp_ipc_isr_stall_args.func(esp_ipc_isr_stall_args.arg);
            esp_ipc_isr_stall_args.func = NULL;
        }
    }
}
