/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// USB persistence flags.

//This bit indicates persistence has been enabled, that is, the USB initialization routines should not
//reset the USB device as the device still is initialized and the host detected it with the same cdcacm/dfu
//descriptor as the ROM uses; we can just re-initialize the software side and have at 'er.
#define USBDC_PERSIST_ENA (1<<31)

//This bit indicates to the ROM that we rebooted because of a request to go into DFU mode; the ROM should
//honour this request.
#define USBDC_BOOT_DFU (1<<30)


//This being non-0 indicates a memory location where a 'testament' is stored, aka a piece of text that should be output
//after a reboot. Can contain core dump info or something.
#define USBDC_TESTAMENT_LOC_MASK 0x7FFFF //bits 19-0; this is added to a base address of 0x3FF80000.

//The testament is a FIFO. The ROM will output all data between textstart and textend; if textend is lower than textstart it will
//output everything from textstart to memend, then memstart to textend.
typedef struct {
    char *memstart; //start of memory region
    char *memend;   //end of memory region
    char *textstart; //start of text to output
    char *textend;
} usbdc_testament_t;

#ifdef __cplusplus
}
#endif
