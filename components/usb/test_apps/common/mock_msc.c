/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "usb/usb_types_ch9.h"
#include "mock_msc.h"

// ---------------------------------------------------- MSC SCSI -------------------------------------------------------

const char *MSC_CLIENT_TAG = "MSC Client";

void mock_msc_scsi_init_cbw(mock_msc_bulk_cbw_t *cbw,
                            bool is_read,
                            unsigned int offset,
                            unsigned int num_sectors,
                            unsigned int sector_size,
                            uint32_t tag)
{
    cbw->dCBWSignature = 0x43425355;    // Fixed value
    cbw->dCBWTag = tag; // Random value that is echoed back
    cbw->dCBWDataTransferLength = num_sectors * sector_size;
    cbw->bmCBWFlags = (is_read) ? (1 << 7) : 0; // If this is a read, set the direction flag
    cbw->bCBWLUN = 0;
    cbw->bCBWCBLength = 10;     // The length of the SCSI command
    // Initialize SCSI CMD as READ10 or WRITE 10
    cbw->CBWCB.opcode = (is_read) ? 0x28 : 0x2A;  // SCSI CMD READ10 or WRITE10
    cbw->CBWCB.flags = 0;
    cbw->CBWCB.lba_3 = (offset >> 24);
    cbw->CBWCB.lba_2 = (offset >> 16);
    cbw->CBWCB.lba_1 = (offset >> 8);
    cbw->CBWCB.lba_0 = (offset >> 0);
    cbw->CBWCB.group = 0;
    cbw->CBWCB.len_1 = (num_sectors >> 8);
    cbw->CBWCB.len_0 = (num_sectors >> 0);
    cbw->CBWCB.control = 0;
}

bool mock_msc_scsi_check_csw(mock_msc_bulk_csw_t *csw, uint32_t tag_expect)
{
    bool no_issues = true;
    if (csw->dCSWSignature != 0x53425355) {
        no_issues = false;
        printf("Warning: csw signature corrupt (0x%"PRIX32")\n", csw->dCSWSignature);
    }
    if (csw->dCSWTag != tag_expect) {
        no_issues = false;
        printf("Warning: csw tag unexpected! Expected %"PRIu32" got %"PRIu32"\n", tag_expect, csw->dCSWTag);
    }
    if (csw->dCSWDataResidue) {
        no_issues = false;
        printf("Warning: csw indicates data residue of %"PRIu32" bytes!\n", csw->dCSWDataResidue);
    }
    if (csw->bCSWStatus) {
        no_issues = false;
        printf("Warning: csw indicates non-good status %d!\n", csw->bCSWStatus);
    }
    return no_issues;
}
