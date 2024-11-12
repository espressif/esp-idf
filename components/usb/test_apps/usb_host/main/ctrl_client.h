/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

typedef struct {
    int num_ctrl_xfer_to_send;
} ctrl_client_test_param_t;

void ctrl_client_async_seq_task(void *arg);
