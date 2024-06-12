/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_cpu.h"
#include "esp_riscv_intr.h"

void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    /* On targets that uses CLIC as the interrupt controller, the first 16 lines (0..15) are reserved for software
     * interrupts, all the other lines starting from 16 and above can be used by external peripheral.
     *
     * Reserve interrupt line 1 for the Wifi controller.
     * Reserve interrupt line 6 since it is used for disabling interrupts in the interrupt allocator (INT_MUX_DISABLED_INTNO)
     */
    const uint32_t rsvd_mask = BIT(1) | BIT(6);
    intr_desc_ret->priority = 1;
    intr_desc_ret->type = ESP_CPU_INTR_TYPE_NA;
    intr_desc_ret->flags = esp_riscv_intr_num_flags(intr_num, rsvd_mask);
}
