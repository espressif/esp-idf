/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"

#if CONFIG_ESP_IPC_ISR_ENABLE

void esp_test_ipc_isr_callback(void *arg) {
    uint32_t value = 0xa5a5;
    *(volatile uint32_t*)arg = value;
}

void esp_test_ipc_isr_get_other_core_id(void *arg) {
    uint32_t core_id;
    __asm volatile("csrr %0, mhartid" : "=r"(core_id));
    *(volatile uint32_t*)arg = core_id;
}

void esp_test_ipc_isr_get_cycle_count_other_cpu(void *arg) {
    uint32_t cycle_count;
    __asm volatile("rdcycle %0;" : "=r"(cycle_count));
    *(volatile uint32_t*)arg = cycle_count;
}

#endif // CONFIG_ESP_IPC_ISR_ENABLE
