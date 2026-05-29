/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/hal.h>
#include "sdkconfig.h"
#include "esp_private/esp_ipc_isr.h"

/* void esp_ipc_isr_waiting_for_finish_cmd(void* arg)
 *
 * It should be called by the CALLX0 command from the handler of High-priority interrupt.
 * Only a2, a3, a4 are used/clobbered here.
 *
 * typedef struct {
 *     uint32_t cmd;   // +0
 *     void*    func;  // +4
 *     void*    arg;   // +8
 *     void*    save_ret_addr; // +12
 *     uint32_t interrupted_context_unsafe; // +16
 * } esp_ipc_isr_args_t;
 */
#if CONFIG_ESP_SYSTEM_IN_IRAM
    .section    .iram1, "ax"
#endif
    .align      4
    .global     esp_ipc_isr_waiting_for_finish_cmd
    .type       esp_ipc_isr_waiting_for_finish_cmd, @function
// Args:
// a2 - arg

    /* Must match C defines */
    .equ    ESP_IPC_ISR_CMD_RESET_STATE, 0

    /* Field offsets */
    .equ    OFFS_CMD,      ESP_IPC_ISR_ARGS_CMD_OFFSET
    .equ    OFFS_FUNC,     ESP_IPC_ISR_ARGS_FUNC_OFFSET
    .equ    OFFS_ARG,      ESP_IPC_ISR_ARGS_ARG_OFFSET
    .equ    OFFS_RET_ADDR, ESP_IPC_ISR_ARGS_RET_ADDR_OFFSET

    .extern esp_ipc_isr_stall_args   /* esp_ipc_isr_args_t */
    .extern esp_ipc_isr_stall_fl

esp_ipc_isr_waiting_for_finish_cmd:
    movi    a3, esp_ipc_isr_stall_fl
    movi    a4, 1
    s32i    a4, a3, 0
    memw
.loop_top:
    /* while (esp_ipc_isr_stall_args.cmd == RESET_STATE) */
    memw
    movi    a3, esp_ipc_isr_stall_args
    l32i    a4, a3, OFFS_CMD
    bnez    a4, .ret_label          /* cmd != 0 -> exit */

    /* if (esp_ipc_isr_stall_args.func != NULL) */
        l32i    a4, a3, OFFS_FUNC       /* a4 = func */
        beqz    a4, .loop_top

        s32i    a0, a3, OFFS_RET_ADDR

        /* Call func(arg) with call0 ABI */
        l32i    a2, a3, OFFS_ARG        /* a2 = arg (callx0 argument) */
        callx0  a4                      /* func(a2) */

        /* esp_ipc_isr_stall_args.func = NULL; */
        movi    a3, esp_ipc_isr_stall_args
        movi    a4, 0
        s32i    a4, a3, OFFS_FUNC
        memw

        l32i    a0, a3, OFFS_RET_ADDR

    j    .loop_top

.ret_label:
    ret
