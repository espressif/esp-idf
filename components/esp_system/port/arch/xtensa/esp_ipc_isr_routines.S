/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/hal.h>

/* esp_ipc_isr_waiting_for_finish_cmd(void* finish_cmd)
 *
 * It should be called by the CALLX0 command from the handler of High-priority interrupt.
 * Only these registers [a2, a3, a4] can be used here.
 */
    .section    .iram1, "ax"
    .align      4
    .global     esp_ipc_isr_waiting_for_finish_cmd
    .type       esp_ipc_isr_waiting_for_finish_cmd, @function
// Args:
// a2 - finish_cmd (pointer on esp_ipc_isr_finish_cmd)
esp_ipc_isr_waiting_for_finish_cmd:
    /* waiting for the finish command */
.check_finish_cmd:
    memw
    l32i    a3, a2, 0
    beqz    a3, .check_finish_cmd
    ret
