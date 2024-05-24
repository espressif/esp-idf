/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_stack.h"

/*
Some tests where the ESP (acting as host) will require that a particular test
device acting as a MSC SCSI flash drive be connected. This header contains
functions to get information and descriptors about that test device.

If you are connecting a different MSC SCSI flash drive, please update
the descriptors in dev_msc.c accordingly.
*/

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief MSC SCSI test device information
 *
 * Structure containing basic information about the the MSC SCSI interface on
 * the test device.
 */
typedef struct {
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t in_ep_addr;
    uint8_t out_up_addr;
    unsigned int scsi_sector_size;
} dev_msc_info_t;

/**
 * @brief Initialize the test device
 *
 * @note Call this before running tests. This is necessary due to IDF-9886
 */
void dev_msc_init(void);

/**
 * @brief Get information about the test device's MSC SCSI interface
 *
 * @return Information object
 */
const dev_msc_info_t *dev_msc_get_info(void);

/**
 * @brief Get the test device's descriptor
 *
 * @param[in] speed Test device's current speed
 * @return Device descriptor
 */
const usb_device_desc_t *dev_msc_get_dev_desc(usb_speed_t speed);

/**
 * @brief Get the test device's configuration descriptor
 *
 * @param[in] speed Test device's current speed
 * @return Configuration descriptor
 */
const usb_config_desc_t *dev_msc_get_config_desc(usb_speed_t speed);

/**
 * @brief Get the test device's MSC interface descriptor
 *
 * @param[in] speed Test device's current speed
 * @return MSC interface descriptor
 */
const usb_intf_desc_t *dev_msc_get_intf_desc(usb_speed_t speed);

/**
 * @brief Get the test device's MSC IN endpoint descriptor
 *
 * @param[in] speed Test device's current speed
 * @return MSC IN endpoint descriptor
 */
const usb_ep_desc_t *dev_msc_get_in_ep_desc(usb_speed_t speed);

/**
 * @brief Get the test device's MSC OUT endpoint descriptor
 *
 * @param[in] speed Test device's current speed
 * @return MSC OUT endpoint descriptor
 */
const usb_ep_desc_t *dev_msc_get_out_ep_desc(usb_speed_t speed);

/**
 * @brief Get the test device's manufacturer string descriptor
 *
 * @return Manufacturer string descriptor
 */
const usb_str_desc_t *dev_msc_get_str_desc_manu(void);

/**
 * @brief Get the test device's product string descriptor
 *
 * @return Product string descriptor
 */
const usb_str_desc_t *dev_msc_get_str_desc_prod(void);

/**
 * @brief Get the test device's serial number string descriptor
 *
 * @return Serial number string descriptor
 */
const usb_str_desc_t *dev_msc_get_str_desc_ser(void);

#ifdef __cplusplus
}
#endif
