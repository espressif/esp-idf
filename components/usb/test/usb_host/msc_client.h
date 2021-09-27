/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

typedef struct {
    int num_sectors_to_read;
    int num_sectors_per_xfer;
    uint32_t msc_scsi_xfer_tag;
    uint16_t idVendor;
    uint16_t idProduct;
} msc_client_test_param_t;

void msc_client_async_seq_task(void *arg);
