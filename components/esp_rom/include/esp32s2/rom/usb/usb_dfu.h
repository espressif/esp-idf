/*
 * SPDX-FileCopyrightText: 2015,2016 Intel Corporation
 * SPDX-FileContributor: 2017 PHYTEC Messtechnik GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief USB Device Firmware Upgrade (DFU) public header
 *
 * Header follows the Device Class Specification for
 * Device Firmware Upgrade Version 1.1
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "usb_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** DFU Class Subclass */
#define DFU_SUBCLASS            0x01

/** DFU Class runtime Protocol */
#define DFU_RT_PROTOCOL         0x01

/** DFU Class DFU mode Protocol */
#define DFU_MODE_PROTOCOL       0x02

/**
 * @brief DFU Class Specific Requests
 */
#define DFU_DETACH          0x00
#define DFU_DNLOAD          0x01
#define DFU_UPLOAD          0x02
#define DFU_GETSTATUS           0x03
#define DFU_CLRSTATUS           0x04
#define DFU_GETSTATE            0x05
#define DFU_ABORT           0x06

/** DFU FUNCTIONAL descriptor type */
#define DFU_FUNC_DESC           0x21

/** DFU attributes DFU Functional Descriptor */
#define DFU_ATTR_WILL_DETACH        0x08
#define DFU_ATTR_MANIFESTATION_TOLERANT 0x04
#define DFU_ATTR_CAN_UPLOAD     0x02
#define DFU_ATTR_CAN_DNLOAD     0x01

/** DFU Specification release */
#define DFU_VERSION         0x0110

/** Run-Time Functional Descriptor */
struct dfu_runtime_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeOut;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} __packed;

/** bStatus values for the DFU_GETSTATUS response */
enum dfu_status {
    statusOK,
    errTARGET,
    errFILE,
    errWRITE,
    errERASE,
    errCHECK_ERASED,
    errPROG,
    errVERIFY,
    errADDRESS,
    errNOTDONE,
    errFIRMWARE,
    errVENDOR,
    errUSB,
    errPOR,
    errUNKNOWN,
    errSTALLEDPKT
};

/** bState values for the DFU_GETSTATUS response */
enum dfu_state {
    appIDLE,
    appDETACH,
    dfuIDLE,
    dfuDNLOAD_SYNC,
    dfuDNBUSY,
    dfuDNLOAD_IDLE,
    dfuMANIFEST_SYNC,
    dfuMANIFEST,
    dfuMANIFEST_WAIT_RST,
    dfuUPLOAD_IDLE,
    dfuERROR,
};

/*
 These callbacks are made public so the ACM driver can call them to handle the switch to DFU.
*/

int dfu_class_handle_req(struct usb_setup_packet *pSetup,
                         int32_t *data_len, uint8_t **data);
void dfu_status_cb(enum usb_dc_status_code status, uint8_t *param);
int usb_dfu_init(void);
int dfu_custom_handle_req(struct usb_setup_packet *pSetup,
                          int32_t *data_len, uint8_t **data);


typedef void(*usb_dfu_detach_routine_t)(int delay);
void usb_dfu_set_detach_cb(usb_dfu_detach_routine_t cb);


#ifdef __cplusplus
}
#endif
