/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#define MSC_ASYNC_CLIENT_MAX_EVENT_MSGS       5

typedef struct {
    int num_sectors_to_read;
    int num_sectors_per_xfer;
    uint32_t msc_scsi_xfer_tag;
} msc_client_test_param_t;

void msc_client_async_seq_task(void *arg);

void msc_client_async_dconn_task(void *arg);

void msc_client_async_enum_task(void *arg);
