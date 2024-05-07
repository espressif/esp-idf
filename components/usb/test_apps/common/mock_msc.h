/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_assert.h"
#include "usb/usb_types_ch9.h"

/*
This header contains bare-bone mock implementations of the MSC SCSI class
*/

#ifdef __cplusplus
extern "C" {
#endif

extern const char *MSC_CLIENT_TAG;

#define MOCK_MSC_SCSI_REQ_INIT_RESET(setup_pkt_ptr, intf_num) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_TYPE_CLASS | USB_BM_REQUEST_TYPE_RECIP_INTERFACE; \
    (setup_pkt_ptr)->bRequest = 0xFF;    \
    (setup_pkt_ptr)->wValue = 0; \
    (setup_pkt_ptr)->wIndex = (intf_num);    \
    (setup_pkt_ptr)->wLength = 0;    \
})

typedef struct __attribute__((packed))
{
    uint8_t opcode; //0x28 = read(10), 0x2A=write(10)
    uint8_t flags;
    uint8_t lba_3;
    uint8_t lba_2;
    uint8_t lba_1;
    uint8_t lba_0;
    uint8_t group;
    uint8_t len_1;
    uint8_t len_0;
    uint8_t control;
} mock_scsi_cmd10_t;

typedef struct __attribute__((packed))
{
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    mock_scsi_cmd10_t CBWCB;
    uint8_t padding[6];
} mock_msc_bulk_cbw_t;

// USB Bulk Transfer Command Status Wrapper data
typedef struct __attribute__((packed))
{
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
} mock_msc_bulk_csw_t;

/**
 * @brief Initialize a MSC Command Block Wrapper (CBW) as an SCSI command
 *
 * @param[in] cbw CBW structure
 * @param[in] is_read Is a read command
 * @param[in] offset Block offset
 * @param[in] num_sectors Number of sectors to read
 * @param[in] sector_size Size of each sector in bytes
 * @param[in] tag Tag (this is simply echoed back
 */
void mock_msc_scsi_init_cbw(mock_msc_bulk_cbw_t *cbw,
                            bool is_read,
                            unsigned int offset,
                            unsigned int num_sectors,
                            unsigned int sector_size,
                            uint32_t tag);

/**
 * @brief Check that returned Command Status Wrapper (CSW) is valid
 *
 * @param[in] csw CSW structure
 * @param[in] tag_expect Expected tag
 * @return True if CSW is valid, false otherwise
 */
bool mock_msc_scsi_check_csw(mock_msc_bulk_csw_t *csw, uint32_t tag_expect);

/**
 * @brief Construct configuration and string descriptors
 */
void mock_msc_scsi_init_reference_descriptors(void);

#ifdef __cplusplus
}
#endif
