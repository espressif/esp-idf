/***************************************************************************
 *
 * Copyright(c) 2015,2016 Intel Corporation.
 * Copyright(c) 2017 PHYTEC Messtechnik GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/

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
