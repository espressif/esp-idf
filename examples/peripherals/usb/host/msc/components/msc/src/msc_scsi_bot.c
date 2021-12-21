/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "esp_check.h"
#include "esp_log.h"
#include "msc_common.h"
#include "msc_scsi_bot.h"

#define TAG "USB_MSC_SCSI"

/* --------------------------- SCSI Definitions ----------------------------- */
#define CMD_SENSE_VALID_BIT (1 << 7)
#define SCSI_FLAG_DPO (1<<4)
#define SCSI_FLAG_FUA (1<<3)

#define SCSI_CMD_FORMAT_UNIT 0x04
#define SCSI_CMD_INQUIRY 0x12
#define SCSI_CMD_MODE_SELECT 0x55
#define SCSI_CMD_MODE_SENSE 0x5A
#define SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1E
#define SCSI_CMD_READ10 0x28
#define SCSI_CMD_READ12 0xA8
#define SCSI_CMD_READ_CAPACITY 0x25
#define SCSI_CMD_READ_FORMAT_CAPACITIES 0x23
#define SCSI_CMD_REQUEST_SENSE 0x03
#define SCSI_CMD_REZERO 0x01
#define SCSI_CMD_SEEK10 0x2B
#define SCSI_CMD_SEND_DIAGNOSTIC 0x1D
#define SCSI_CMD_START_STOP Unit 0x1B
#define SCSI_CMD_TEST_UNIT_READY 0x00
#define SCSI_CMD_VERIFY 0x2F
#define SCSI_CMD_WRITE10 0x2A
#define SCSI_CMD_WRITE12 0xAA
#define SCSI_CMD_WRITE_AND_VERIFY 0x2E

#define IN_DIR   CWB_FLAG_DIRECTION_IN
#define OUT_DIR  0

#define INQUIRY_VID_SIZE    8
#define INQUIRY_PID_SIZE    16
#define INQUIRY_REV_SIZE    4

#define CBW_CMD_SIZE(cmd) (sizeof(cmd) - sizeof(msc_cbw_t))

#define CBW_BASE_INIT(dir, cbw_len, data_len)   \
    .base = {                                   \
        .signature = 0x43425355,                \
        .tag = ++cbw_tag,                       \
        .flags = dir,                           \
        .lun = 0,                               \
        .data_length = data_len,                \
        .cbw_length = cbw_len,                  \
    }

#define FEATURE_SELECTOR_ENDPOINT   0
#define CSW_SIGNATURE   0x53425355
#define CBW_SIZE        31

#define USB_MASS_REQ_INIT_RESET(ctrl_req_ptr, intf_num) ({                  \
    (ctrl_req_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT |           \
                                    USB_BM_REQUEST_TYPE_TYPE_CLASS |        \
                                    USB_BM_REQUEST_TYPE_RECIP_INTERFACE;    \
    (ctrl_req_ptr)->bRequest = 0xFF;                                        \
    (ctrl_req_ptr)->wValue = 0;                                             \
    (ctrl_req_ptr)->wIndex = (intf_num);                                    \
    (ctrl_req_ptr)->wLength = 0;                                            \
})

#define USB_MASS_REQ_INIT_GET_MAX_LUN(ctrl_req_ptr, intf_num) ({            \
    (ctrl_req_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN |            \
                                    USB_BM_REQUEST_TYPE_TYPE_CLASS |        \
                                    USB_BM_REQUEST_TYPE_RECIP_INTERFACE;    \
    (ctrl_req_ptr)->bRequest = 0xFE;                                        \
    (ctrl_req_ptr)->wValue = 0;                                             \
    (ctrl_req_ptr)->wIndex = (intf_num);                                    \
    (ctrl_req_ptr)->wLength = 1;                                            \
})

#define USB_SETUP_PACKET_INIT_CLEAR_FEATURE_EP(ctrl_req_ptr, ep_num) ({     \
    (ctrl_req_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT |           \
                                    USB_BM_REQUEST_TYPE_TYPE_STANDARD |     \
                                    USB_BM_REQUEST_TYPE_RECIP_ENDPOINT;     \
    (ctrl_req_ptr)->bRequest = USB_B_REQUEST_CLEAR_FEATURE;                 \
    (ctrl_req_ptr)->wValue = FEATURE_SELECTOR_ENDPOINT;                     \
    (ctrl_req_ptr)->wIndex = (ep_num);                                      \
    (ctrl_req_ptr)->wLength = 0;                                            \
})

#define CWB_FLAG_DIRECTION_IN (1<<7) // device -> host

/**
 * @brief Command Block Wrapper structure
 */
typedef struct __attribute__((packed))
{
    uint32_t signature;
    uint32_t tag;
    uint32_t data_length;
    uint8_t flags;
    uint8_t lun;
    uint8_t cbw_length;
} msc_cbw_t;

/**
 * @brief Command Status Wrapper structure
 */
typedef struct __attribute__((packed))
{
    uint32_t signature;
    uint32_t tag;
    uint32_t dataResidue;
    uint8_t status;
} msc_csw_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint32_t address;
    uint8_t reserved1;
    uint16_t length;
    uint8_t reserved2[3];
} cbw_read10_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint32_t address;
    uint8_t reserved1;
    uint16_t length;
    uint8_t reserved2[1];
} cbw_write10_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint32_t address;
    uint8_t reserved[6];
} cbw_read_capacity_t;

typedef struct __attribute__((packed))
{
    uint32_t block_count;
    uint32_t block_size;
} cbw_read_capacity_response_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint8_t reserved[10];
} cbw_unit_ready_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint8_t reserved_0[2];
    uint8_t allocation_length;
    uint8_t reserved_1[7];
} cbw_sense_t;

typedef struct __attribute__((packed))
{
    uint8_t error_code;
    uint8_t reserved_0;
    uint8_t sense_key;
    uint32_t info;
    uint8_t sense_len;
    uint32_t reserved_1;
    uint8_t sense_code;
    uint8_t sense_code_qualifier;
    uint32_t reserved_2;
} cbw_sense_response_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint8_t page_code;
    uint8_t reserved_0;
    uint8_t allocation_length;
    uint8_t reserved_1[7];
} cbw_inquiry_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint8_t pc_page_code;
    uint8_t reserved_1[4];
    uint16_t parameter_list_length;
    uint8_t reserved_2[3];
} mode_sense_t;

typedef struct __attribute__((packed))
{
    uint8_t data[8];
} mode_sense_response_t;

typedef struct __attribute__((packed))
{
    msc_cbw_t base;
    uint8_t opcode;
    uint8_t flags;
    uint8_t reserved_1[2];
    uint8_t prevent;
    uint8_t reserved_2[7];
} prevent_allow_medium_removal_t;

typedef struct __attribute__((packed))
{
    uint8_t data[36];
} cbw_inquiry_response_t;

// Unique number based on which MSC protocol pairs request and response
static uint32_t cbw_tag;

static esp_err_t check_csw(msc_csw_t *csw, uint32_t tag)
{
    bool csw_ok = csw->signature == CSW_SIGNATURE && csw->tag == tag &&
                  csw->dataResidue == 0 && csw->status == 0;

    if (!csw_ok) {
        ESP_LOGD(TAG, "CSW failed: status %d", csw->status);
    }

    return csw_ok ? ESP_OK : ESP_FAIL;
}

static esp_err_t clear_feature(msc_device_t *device, uint8_t endpoint)
{
    usb_device_handle_t dev = device->handle;
    usb_transfer_t *xfer = device->xfer;

    MSC_RETURN_ON_ERROR( usb_host_endpoint_clear(dev, endpoint) );
    USB_SETUP_PACKET_INIT_CLEAR_FEATURE_EP((usb_setup_packet_t *)xfer->data_buffer, endpoint);
    MSC_RETURN_ON_ERROR( msc_control_transfer(device, xfer, USB_SETUP_PACKET_SIZE) );

    return ESP_OK;
}

esp_err_t msc_mass_reset(msc_device_t *device)
{
    usb_transfer_t *xfer = device->xfer;

    USB_MASS_REQ_INIT_RESET((usb_setup_packet_t *)xfer->data_buffer, 0);
    MSC_RETURN_ON_ERROR( msc_control_transfer(device, xfer, USB_SETUP_PACKET_SIZE) );

    return ESP_OK;
}

esp_err_t msc_get_max_lun(msc_device_t *device, uint8_t *lun)
{
    usb_transfer_t *xfer = device->xfer;

    USB_MASS_REQ_INIT_GET_MAX_LUN((usb_setup_packet_t *)xfer->data_buffer, 0);
    MSC_RETURN_ON_ERROR( msc_control_transfer(device, xfer, USB_SETUP_PACKET_SIZE + 1) );

    *lun = xfer->data_buffer[USB_SETUP_PACKET_SIZE];

    return ESP_OK;
}

static esp_err_t bot_execute_command(msc_device_t *device, msc_cbw_t *cbw, void *data, size_t size)
{
    msc_csw_t csw;
    msc_endpoint_t ep = (cbw->flags & CWB_FLAG_DIRECTION_IN) ? MSC_EP_IN : MSC_EP_OUT;

    MSC_RETURN_ON_ERROR( msc_bulk_transfer(device, (uint8_t *)cbw, CBW_SIZE, MSC_EP_OUT) );

    if (data) {
        MSC_RETURN_ON_ERROR( msc_bulk_transfer(device, (uint8_t *)data, size, ep) );
    }

    esp_err_t  err = msc_bulk_transfer(device, (uint8_t *)&csw, sizeof(msc_csw_t), MSC_EP_IN);

    if (err ==  ESP_FAIL && device->transfer_status == USB_TRANSFER_STATUS_STALL) {
        ESP_RETURN_ON_ERROR( clear_feature(device, MSC_EP_IN), TAG, "Clear feature failed" );
        // Try to read csw again after clearing feature
        err = msc_bulk_transfer(device, (uint8_t *)&csw, sizeof(msc_csw_t), MSC_EP_IN);
        if (err) {
            ESP_RETURN_ON_ERROR( clear_feature(device, MSC_EP_IN), TAG, "Clear feature failed" );
            ESP_RETURN_ON_ERROR( msc_mass_reset(device), TAG, "Mass reset failed" );
            return ESP_FAIL;
        }
    }

    MSC_RETURN_ON_ERROR(err);

    return check_csw(&csw, cbw->tag);
}


esp_err_t scsi_cmd_read10(msc_device_t *device,
                          uint8_t *data,
                          uint32_t sector_address,
                          uint32_t num_sectors,
                          uint32_t sector_size)
{
    cbw_read10_t cbw = {
        CBW_BASE_INIT(IN_DIR, CBW_CMD_SIZE(cbw_read10_t), num_sectors * sector_size),
        .opcode = SCSI_CMD_READ10,
        .flags = 0, // lun
        .address = __builtin_bswap32(sector_address),
        .length = __builtin_bswap16(num_sectors),
    };

    return bot_execute_command(device, &cbw.base, data, num_sectors * sector_size);
}

esp_err_t scsi_cmd_write10(msc_device_t *device,
                           const uint8_t *data,
                           uint32_t sector_address,
                           uint32_t num_sectors,
                           uint32_t sector_size)
{
    cbw_write10_t cbw = {
        CBW_BASE_INIT(OUT_DIR, CBW_CMD_SIZE(cbw_write10_t), num_sectors * sector_size),
        .opcode = SCSI_CMD_WRITE10,
        .address = __builtin_bswap32(sector_address),
        .length = __builtin_bswap16(num_sectors),
    };

    return bot_execute_command(device, &cbw.base, (void *)data, num_sectors * sector_size);
}

esp_err_t scsi_cmd_read_capacity(msc_device_t *device, uint32_t *block_size, uint32_t *block_count)
{
    cbw_read_capacity_response_t response;

    cbw_read_capacity_t cbw = {
        CBW_BASE_INIT(IN_DIR, CBW_CMD_SIZE(cbw_read_capacity_t), sizeof(response)),
        .opcode = SCSI_CMD_READ_CAPACITY,
    };

    MSC_RETURN_ON_ERROR( bot_execute_command(device, &cbw.base, &response, sizeof(response)) );

    *block_count = __builtin_bswap32(response.block_count);
    *block_size = __builtin_bswap32(response.block_size);

    return ESP_OK;
}

esp_err_t scsi_cmd_unit_ready(msc_device_t *device)
{
    cbw_unit_ready_t cbw = {
        CBW_BASE_INIT(IN_DIR, CBW_CMD_SIZE(cbw_unit_ready_t), 0),
        .opcode = SCSI_CMD_TEST_UNIT_READY,
    };

    return bot_execute_command(device, &cbw.base, NULL, 0);
}

esp_err_t scsi_cmd_sense(msc_device_t *device, scsi_sense_data_t *sense)
{
    cbw_sense_response_t response;

    cbw_sense_t cbw = {
        CBW_BASE_INIT(IN_DIR, CBW_CMD_SIZE(cbw_sense_t), sizeof(response)),
        .opcode = SCSI_CMD_REQUEST_SENSE,
        .allocation_length = sizeof(response),
    };

    MSC_RETURN_ON_ERROR( bot_execute_command(device, &cbw.base, &response, sizeof(response)) );

    if (sense->key) {
        ESP_LOGD(TAG, "sense_key: 0x%02X, code: 0x%02X, qualifier: 0x%02X",
                 response.sense_key, response.sense_code, response.sense_code_qualifier);
    }

    sense->key = response.sense_key;
    sense->code = response.sense_code;
    sense->code_q = response.sense_code_qualifier;

    return ESP_OK;
}

esp_err_t scsi_cmd_inquiry(msc_device_t *device)
{
    cbw_inquiry_response_t response = { 0 };

    cbw_inquiry_t cbw = {
        CBW_BASE_INIT(IN_DIR, CBW_CMD_SIZE(cbw_inquiry_t), sizeof(response)),
        .opcode = SCSI_CMD_INQUIRY,
        .allocation_length = sizeof(response),
    };

    return bot_execute_command(device, &cbw.base, &response, sizeof(response) );
}

esp_err_t scsi_cmd_mode_sense(msc_device_t *device)
{
    mode_sense_response_t response = { 0 };

    mode_sense_t cbw = {
        CBW_BASE_INIT(IN_DIR, CBW_CMD_SIZE(mode_sense_t), sizeof(response)),
        .opcode = SCSI_CMD_MODE_SENSE,
        .pc_page_code = 0x3F,
        .parameter_list_length = sizeof(response),
    };

    return bot_execute_command(device, &cbw.base, &response, sizeof(response) );
}

esp_err_t scsi_cmd_prevent_removal(msc_device_t *device, bool prevent)
{
    prevent_allow_medium_removal_t cbw = {
        CBW_BASE_INIT(OUT_DIR, CBW_CMD_SIZE(prevent_allow_medium_removal_t), 0),
        .opcode = SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL,
        .prevent = 1,
    };

    return bot_execute_command(device, &cbw.base, NULL, 0);
}
