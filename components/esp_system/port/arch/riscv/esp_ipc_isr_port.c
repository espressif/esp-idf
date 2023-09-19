/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/interrupts.h"
#include "soc/hp_system_reg.h"
#include "esp_intr_alloc.h"
#include "riscv/interrupt.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#include "esp_attr.h"
#include "sdkconfig.h"


void esp_ipc_isr_port_init(const int cpuid)
{
    uint32_t intr_source = ETS_FROM_CPU_INTR2_SOURCE + cpuid; // ETS_FROM_CPU_INTR2_SOURCE and ETS_FROM_CPU_INTR3_SOURCE

    esp_intr_disable_source(ETS_IPC_ISR_INUM);

    esp_rom_route_intr_matrix(cpuid, intr_source, ETS_IPC_ISR_INUM);

    esp_cpu_intr_set_type(ETS_IPC_ISR_INUM, INTR_TYPE_LEVEL);

#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
    esp_cpu_intr_set_priority(ETS_IPC_ISR_INUM, 5);
#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4
    esp_cpu_intr_set_priority(ETS_IPC_ISR_INUM, 4);
#else
    #error "CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL is not defined!"
#endif

    esp_intr_enable_source(ETS_IPC_ISR_INUM);
}

IRAM_ATTR void esp_ipc_isr_port_int_trigger(const int cpuid)
{
    if (cpuid == 0) {
        // it runs an interrupt on cpu0
        REG_WRITE(HP_SYSTEM_CPU_INT_FROM_CPU_2_REG, HP_SYSTEM_CPU_INT_FROM_CPU_2);
    } else {
        // it runs an interrupt on cpu1
        REG_WRITE(HP_SYSTEM_CPU_INT_FROM_CPU_3_REG, HP_SYSTEM_CPU_INT_FROM_CPU_3);
    }
}
