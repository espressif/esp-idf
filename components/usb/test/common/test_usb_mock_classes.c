// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "usb/usb_types_ch9.h"
#include "test_usb_mock_classes.h"

// ---------------------------------------------------- MSC SCSI -------------------------------------------------------

void mock_msc_scsi_init_cbw(mock_msc_bulk_cbw_t *cbw, bool is_read, int offset, int num_sectors, uint32_t tag)
{
    cbw->dCBWSignature = 0x43425355;    //Fixed value
    cbw->dCBWTag = tag; //Random value that is echoed back
    cbw->dCBWDataTransferLength = num_sectors * MOCK_MSC_SCSI_SECTOR_SIZE;
    cbw->bmCBWFlags = (is_read) ? (1 << 7) : 0; //If this is a read, set the direction flag
    cbw->bCBWLUN = MOCK_MSC_SCSI_LUN;
    cbw->bCBWCBLength = 10;     //The length of the SCSI command
    //Initialize SCSI CMD as READ10 or WRITE 10
    cbw->CBWCB.opcode = (is_read) ? 0x28 : 0x2A;  //SCSI CMD READ10 or WRITE10
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
        printf("Warning: csw signature corrupt (0x%X)\n", csw->dCSWSignature);
    }
    if (csw->dCSWTag != tag_expect) {
        no_issues = false;
        printf("Warning: csw tag unexpected! Expected %d got %d\n", tag_expect, csw->dCSWTag);
    }
    if (csw->dCSWDataResidue) {
        no_issues = false;
        printf("Warning: csw indicates data residue of %d bytes!\n", csw->dCSWDataResidue);
    }
    if (csw->bCSWStatus) {
        no_issues = false;
        printf("Warning: csw indicates non-good status %d!\n", csw->bCSWStatus);
    }
    return no_issues;
}


// ---------------------------------------------------- HID Mouse ------------------------------------------------------

void mock_hid_process_report(mock_hid_mouse_report_t *report, int iter)
{
    static int x_pos = 0;
    static int y_pos = 0;
    //Update X position
    if (report->x_movement & 0x80) {    //Positive movement
        x_pos += report->x_movement & 0x7F;
    } else {    //Negative movement
        x_pos -= report->x_movement & 0x7F;
    }
    //Update Y position
    if (report->y_movement & 0x80) {    //Positive movement
        y_pos += report->y_movement & 0x7F;
    } else {    //Negative movement
        y_pos -= report->y_movement & 0x7F;
    }
    printf("\rX:%d\tY:%d\tIter: %d\n", x_pos, y_pos, iter);
}
