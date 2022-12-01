/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

typedef struct {
    int num_ctrl_xfer_to_send;
    uint16_t idVendor;
    uint16_t idProduct;
} ctrl_client_test_param_t;

void ctrl_client_async_seq_task(void *arg);
