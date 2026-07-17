/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdint.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_cpu.h"

void IRAM_ATTR esp_ipc_isr_waiting_for_finish_cmd(void* ipc_isr_finish_cmd)
{
#if SOC_BRANCH_PREDICTOR_SUPPORTED
    /* The branch predictor keeps issuing speculative instruction fetches while
     * this core spins here. The other core may suspend the external memory
     * cache during the stall, in which case a speculative fetch into cached
     * address space raises a cache access-fail interrupt. */
    esp_cpu_branch_prediction_disable();
#endif
    while (*(volatile uint32_t *)ipc_isr_finish_cmd == 0) { };
#if SOC_BRANCH_PREDICTOR_SUPPORTED
    esp_cpu_branch_prediction_enable();
#endif
}
