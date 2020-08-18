// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdbool.h>
#include "tusb.h"
#include "tusb_option.h"
#include "tusb_config.h"
#include "tinyusb_types.h"


#ifdef __cplusplus
extern "C" {
#endif


/* tinyusb uses buffers with type of uint8_t[] but in our driver we are reading them as a 32-bit word */
#if (CFG_TUD_ENDOINT0_SIZE < 4)
#   define CFG_TUD_ENDOINT0_SIZE 4
#   warning "CFG_TUD_ENDOINT0_SIZE was too low and was set to 4"
#endif

#if TUSB_OPT_DEVICE_ENABLED

#   if CFG_TUD_HID
#      if (CFG_TUD_HID_BUFSIZE < 4)
#         define CFG_TUD_HID_BUFSIZE 4
#         warning "CFG_TUD_HID_BUFSIZE was too low and was set to 4"
#      endif
#   endif

#   if CFG_TUD_CDC
#      if (CFG_TUD_CDC_EPSIZE < 4)
#         define CFG_TUD_CDC_EPSIZE 4
#         warning "CFG_TUD_CDC_EPSIZE was too low and was set to 4"
#      endif
#   endif

#   if CFG_TUD_MSC
#      if (CFG_TUD_MSC_BUFSIZE < 4)
#         define CFG_TUD_MSC_BUFSIZE 4
#         warning "CFG_TUD_MSC_BUFSIZE was too low and was set to 4"
#      endif
#   endif

#   if CFG_TUD_MIDI
#       if (CFG_TUD_MIDI_EPSIZE < 4)
#          define CFG_TUD_MIDI_EPSIZE 4
#          warning "CFG_TUD_MIDI_EPSIZE was too low and was set to 4"
#       endif
#   endif

#   if CFG_TUD_CUSTOM_CLASS
#          warning "Please check that the buffer is more then 4 bytes"
#   endif
#endif

/**
 * @brief Configuration structure of the tinyUSB core
 */
typedef struct {
    tusb_desc_device_t *descriptor; /*!< Pointer to a device descriptor */
    char **string_descriptor;       /*!< Pointer to an array of string descriptors */
    bool external_phy;              /*!< Should USB use an external PHY */
} tinyusb_config_t;

esp_err_t tinyusb_driver_install(const tinyusb_config_t *config);
// TODO esp_err_t tinyusb_driver_uninstall(void); (IDF-1474)


#ifdef __cplusplus
}
#endif
