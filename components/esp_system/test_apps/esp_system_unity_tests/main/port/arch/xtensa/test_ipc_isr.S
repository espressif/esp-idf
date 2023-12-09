/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if !CONFIG_FREERTOS_UNICORE

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/hal.h>

/* esp_test_ipc_isr_callback(void *arg)
 *
 * It should be called by the CALLX0 command from the handler of High-priority interrupt.
 * Only these registers [a2, a3, a4] can be used here.
 */
    .section    .iram1, "ax"
    .align      4
    .global     esp_test_ipc_isr_callback
    .type       esp_test_ipc_isr_callback, @function
// Args:
// a2 - void* arg
esp_test_ipc_isr_callback:
    movi    a3, 0xa5a5
    s32i    a3, a2, 0
    ret


/* esp_test_ipc_isr_get_other_core_id(void *arg)
 *
 * this function puts the core_id of the other CPU in the arg.
 * use only a2, a3 and a4 regs here.
*/
    .section    .iram1, "ax"
    .align      4
    .global     esp_test_ipc_isr_get_other_core_id
    .type       esp_test_ipc_isr_get_other_core_id, @function
    // Args:
    // a2 - void* arg
    esp_test_ipc_isr_get_other_core_id:
    rsr.prid a3
    extui   a3, a3, 13, 1
    s32i    a3, a2, 0
    ret


/* esp_test_ipc_isr_get_cycle_count_other_cpu(void *arg)
 *
 * this function puts CCOUNT of the other CPU in the arg.
 * use only a2, a3 and a4 regs here.
*/
    .section    .iram1, "ax"
    .align      4
    .global     esp_test_ipc_isr_get_cycle_count_other_cpu
    .type       esp_test_ipc_isr_get_cycle_count_other_cpu, @function
    // Args:
    // a2 - void* arg
    esp_test_ipc_isr_get_cycle_count_other_cpu:
    rsr.ccount a3
    s32i    a3, a2, 0
    ret

#endif //!CONFIG_FREERTOS_UNICORE
