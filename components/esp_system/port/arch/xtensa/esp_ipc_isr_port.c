/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/interrupts.h"
#include "soc/dport_reg.h"
#ifndef CONFIG_IDF_TARGET_ESP32
#include "soc/system_reg.h"
#endif
#include "esp_rom_sys.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "sdkconfig.h"

void esp_ipc_isr_port_init(const int cpuid)
{
    uint32_t intr_source = ETS_FROM_CPU_INTR2_SOURCE + cpuid; // ETS_FROM_CPU_INTR2_SOURCE and ETS_FROM_CPU_INTR3_SOURCE
    ESP_INTR_DISABLE(ETS_IPC_ISR_INUM);
    esp_rom_route_intr_matrix(cpuid, intr_source, ETS_IPC_ISR_INUM);
    ESP_INTR_ENABLE(ETS_IPC_ISR_INUM);
}

IRAM_ATTR void esp_ipc_isr_port_int_trigger(const int cpuid)
{
    if (cpuid == 0) {
        // it runs an interrupt on cpu0
        DPORT_REG_WRITE(SYSTEM_CPU_INTR_FROM_CPU_2_REG, SYSTEM_CPU_INTR_FROM_CPU_2);
    } else {
        // it runs an interrupt on cpu1
        DPORT_REG_WRITE(SYSTEM_CPU_INTR_FROM_CPU_3_REG, SYSTEM_CPU_INTR_FROM_CPU_3);
    }
}
