/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string.h>
#include "usb_descriptors.h"

#ifdef __cplusplus
extern "C" {
#endif
//------------- HID Report Descriptor -------------//
#if CFG_TUD_HID
enum {
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_MOUSE
};
#endif

//------------- Configuration Descriptor -------------//
enum {
#   if CFG_TUD_CDC
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
#   endif

#   if CFG_TUD_CDC > 1
    ITF_NUM_CDC1,
    ITF_NUM_CDC1_DATA,
#   endif

#   if CFG_TUD_MSC
    ITF_NUM_MSC,
#   endif

#   if CFG_TUD_HID
    ITF_NUM_HID,
#   endif

#   if CFG_TUD_VENDOR
    ITF_NUM_VENDOR,
#   endif

#   if CFG_TUD_MIDI
    ITF_NUM_MIDI,
    ITF_NUM_MIDI_STREAMING,
#   endif

#   if CFG_TUD_DFU_RUNTIME
    ITF_NUM_DFU_RT,
#   endif

    ITF_NUM_TOTAL
};

enum {
    TUSB_DESC_TOTAL_LEN = TUD_CONFIG_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN + CFG_TUD_MSC * TUD_MSC_DESC_LEN +
                       CFG_TUD_HID * TUD_HID_DESC_LEN + CFG_TUD_VENDOR * TUD_VENDOR_DESC_LEN + CFG_TUD_MIDI * TUD_MIDI_DESC_LEN + CFG_TUD_DFU_RUNTIME * TUD_DFU_RT_DESC_LEN
};

bool tusb_desc_set;
void tusb_set_descriptor(tusb_desc_device_t *desc, const char **str_desc);
tusb_desc_device_t *tusb_get_active_desc(void);
char **tusb_get_active_str_desc(void);
void tusb_clear_descriptor(void);

#ifdef __cplusplus
}
#endif
