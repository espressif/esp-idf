/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

typedef struct {
    uint32_t in[3];
    uint32_t out[4];
} arg_data_t;

void get_mstatus_other_cpu(void *arg);
void extended_ipc_isr_func(void* arg);
