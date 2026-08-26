/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "callbacks.h"

void get_mstatus_other_cpu(void *arg) {
    uint32_t mstatus_value;
    asm volatile ("csrr %0, mstatus" : "=r" (mstatus_value));
    *(volatile uint32_t*)arg = mstatus_value;
}

void extended_ipc_isr_func(void* arg) {
    arg_data_t *a = (arg_data_t *)arg;
    a->out[0] = a->in[0] | a->in[1] | a->in[2];
    a->out[1] = a->in[0] + a->in[1] + a->in[2];
    asm volatile ("csrr %0, mcause" : "=r" (a->out[2]));
    asm volatile ("csrr %0, mstatus" : "=r" (a->out[3]));
}
