/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_cpu.h"
#include "esp_riscv_intr.h"
#include "sdkconfig.h"

void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    /* On the ESP32-H2, interrupt:
     * - 6 for "permanently disabled interrupt"
     *
     * Interrupts 3, 4 and 7 are unavailable for PULP CPU as they are bound to Core-Local Interrupts (CLINT)
     */
    // [TODO: IDF-2465]
    const uint32_t base_rsvd_mask = BIT(3) | BIT(4) | BIT(6) | BIT(7);

    /* On the ESP32-H2, interrupt 14 is reserved for ESP-TEE
     * for operations related to secure peripherals under its control
     * (e.g. AES, SHA, APM)
     */
#if CONFIG_SECURE_ENABLE_TEE
    const uint32_t rsvd_mask = base_rsvd_mask | BIT(14);
#else
    const uint32_t rsvd_mask = base_rsvd_mask;
#endif


    intr_desc_ret->priority = 1;
    intr_desc_ret->type = ESP_CPU_INTR_TYPE_NA;
    intr_desc_ret->flags = esp_riscv_intr_num_flags(intr_num, rsvd_mask);
}
