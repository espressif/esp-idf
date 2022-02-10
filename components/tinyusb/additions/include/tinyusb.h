/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "tusb.h"
#include "tusb_option.h"
#include "tusb_config.h"
#include "tinyusb_types.h"


#ifdef __cplusplus
extern "C" {
#endif


/* tinyusb uses buffers with type of uint8_t[] but in our driver we are reading them as a 32-bit word */
#if (CFG_TUD_ENDPOINT0_SIZE < 4)
#   define CFG_TUD_ENDPOINT0_SIZE 4
#   warning "CFG_TUD_ENDPOINT0_SIZE was too low and was set to 4"
#endif

#if TUSB_OPT_DEVICE_ENABLED

#   if CFG_TUD_HID
#      if (CFG_TUD_HID_BUFSIZE < 4)
#         define CFG_TUD_HID_BUFSIZE 4
#         warning "CFG_TUD_HID_BUFSIZE was too low and was set to 4"
#      endif
#   endif

#   if CFG_TUD_CDC
#      if (CFG_TUD_CDC_EP_BUFSIZE < 4)
#         define CFG_TUD_CDC_EP_BUFSIZE 4
#         warning "CFG_TUD_CDC_EP_BUFSIZE was too low and was set to 4"
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
    union {
        const tusb_desc_device_t *device_descriptor; /*!< Pointer to a device descriptor. If set to NULL, the TinyUSB device will use a default device descriptor whose values are set in Kconfig */
        const tusb_desc_device_t *descriptor  __attribute__((deprecated)); /*!< Alias to `device_descriptor` for backward compatibility */
    };
    const char **string_descriptor;            /*!< Pointer to an array of string descriptors */
    bool external_phy;                         /*!< Should USB use an external PHY */
    const uint8_t *configuration_descriptor;   /*!< Pointer to a configuration descriptor. If set to NULL, TinyUSB device will use a default configuration descriptor whose values are set in Kconfig */
} tinyusb_config_t;

/**
 * @brief This is an all-in-one helper function, including:
 * 1. USB device driver initialization
 * 2. Descriptors preparation
 * 3. TinyUSB stack initialization
 * 4. Creates and start a task to handle usb events
 *
 * @note Don't change Custom descriptor, but if it has to be done,
 *       Suggest to define as follows in order to match the Interface Association Descriptor (IAD):
 *       bDeviceClass = TUSB_CLASS_MISC,
 *       bDeviceSubClass = MISC_SUBCLASS_COMMON,
 *
 * @param config tinyusb stack specific configuration
 * @retval ESP_ERR_INVALID_ARG Install driver and tinyusb stack failed because of invalid argument
 * @retval ESP_FAIL Install driver and tinyusb stack failed because of internal error
 * @retval ESP_OK Install driver and tinyusb stack successfully
 */
esp_err_t tinyusb_driver_install(const tinyusb_config_t *config);

// TODO esp_err_t tinyusb_driver_uninstall(void); (IDF-1474)

#ifdef __cplusplus
}
#endif
