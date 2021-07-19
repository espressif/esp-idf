/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <sys/queue.h>
#include "esp_err.h"
#include "esp_check.h"
#include "diskio_usb.h"
#include "usb/usb_host.h"
#include "usb/usb_types_stack.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    MSC_EP_OUT,
    MSC_EP_IN
} msc_endpoint_t;

typedef struct {
    uint16_t bulk_in_mps;
    uint8_t bulk_in_ep;
    uint8_t bulk_out_ep;
    uint8_t iface_num;
} msc_config_t;

typedef struct msc_host_device {
    STAILQ_ENTRY(msc_host_device) tailq_entry;
    usb_transfer_status_t transfer_status;
    SemaphoreHandle_t transfer_done;
    usb_device_handle_t handle;
    usb_transfer_t *xfer;
    msc_config_t config;
    usb_disk_t disk;
} msc_device_t;

esp_err_t msc_bulk_transfer(msc_device_t *device_handle, uint8_t *data, size_t size, msc_endpoint_t ep);

esp_err_t msc_control_transfer(msc_device_t *device_handle, usb_transfer_t *xfer, size_t len);

#define MSC_GOTO_ON_ERROR(exp) ESP_GOTO_ON_ERROR(exp, fail, TAG, "")

#define MSC_GOTO_ON_FALSE(exp, err) ESP_GOTO_ON_FALSE( (exp), err, fail, TAG, "" )

#define MSC_RETURN_ON_ERROR(exp) ESP_RETURN_ON_ERROR((exp), TAG, "")

#define MSC_RETURN_ON_FALSE(exp, err) ESP_RETURN_ON_FALSE( (exp), (err), TAG, "")

#define MSC_RETURN_ON_INVALID_ARG(exp) ESP_RETURN_ON_FALSE((exp) != NULL, ESP_ERR_INVALID_ARG, TAG, "")

#ifdef __cplusplus
}
#endif
