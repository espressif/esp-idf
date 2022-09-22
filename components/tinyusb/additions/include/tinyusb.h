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

/**
 * @brief Configuration structure of the TinyUSB core
 *
 * USB specification mandates self-powered devices to monitor USB VBUS to detect connection/disconnection events.
 * If you want to use this feature, connected VBUS to any free GPIO through a voltage divider or voltage comparator.
 * The voltage divider output should be (0.75 * Vdd) if VBUS is 4.4V (lowest valid voltage at device port).
 * The comparator thresholds should be set with hysteresis: 4.35V (falling edge) and 4.75V (raising edge).
 */
typedef struct {
    union {
        const tusb_desc_device_t *device_descriptor; /*!< Pointer to a device descriptor. If set to NULL, the TinyUSB device will use a default device descriptor whose values are set in Kconfig */
        const tusb_desc_device_t *descriptor  __attribute__((deprecated)); /*!< Alias to `device_descriptor` for backward compatibility */
    };
    const char **string_descriptor;            /*!< Pointer to an array of string descriptors */
    bool external_phy;                         /*!< Should USB use an external PHY */
    const uint8_t *configuration_descriptor;   /*!< Pointer to a configuration descriptor. If set to NULL, TinyUSB device will use a default configuration descriptor whose values are set in Kconfig */
    bool self_powered;                         /*!< This is a self-powered USB device. USB VBUS must be monitored. */
    int vbus_monitor_io;                       /*!< GPIO for VBUS monitoring. Ignored if not self_powered. */
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
