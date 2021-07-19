/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
Warning: The USB Host Library API is still a beta version and may be subject to change
*/

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "usb/usb_types_stack.h"
#include "usb/usb_types_ch9.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------- Configuration Descriptor Parsing -------------------------------------------

/**
 * @brief Get the next descriptor
 *
 * Given a particular descriptor within a full configuration descriptor, get the next descriptor within the
 * configuration descriptor. This is a convenience function that can be used to walk each individual descriptor within
 * a full configuration descriptor.
 *
 * @param[in] cur_desc Current descriptor
 * @param[in] wTotalLength Total length of the configuration descriptor
 * @param[inout] offset Byte offset relative to the start of the configuration descriptor. On input, it is the offset of
 *                      the current descriptor. On output, it is the offset of the returned descriptor.
 * @return usb_standard_desc_t* Next descriptor, NULL if end of configuration descriptor reached
 */
const usb_standard_desc_t *usb_parse_next_descriptor(const usb_standard_desc_t *cur_desc, uint16_t wTotalLength, int *offset);

/**
 * @brief Get the next descriptor of a particular type
 *
 * Given a particular descriptor within a full configuration descriptor, get the next descriptor of a particular type
 * (i.e., using the bDescriptorType value) within the configuration descriptor.
 *
 * @param[in] cur_desc Current descriptor
 * @param[in] wTotalLength Total length of the configuration descriptor
 * @param[in] bDescriptorType Type of the next descriptor to get
 * @param[inout] offset Byte offset relative to the start of the configuration descriptor. On input, it is the offset of
 *                      the current descriptor. On output, it is the offset of the returned descriptor.
 * @return usb_standard_desc_t* Next descriptor, NULL if end descriptor is not found or configuration descriptor reached
 */
const usb_standard_desc_t *usb_parse_next_descriptor_of_type(const usb_standard_desc_t *cur_desc, uint16_t wTotalLength, uint8_t bDescriptorType, int *offset);

/**
 * @brief Get the number of alternate settings for a bInterfaceNumber
 *
 * Given a particular configuration descriptor, for a particular bInterfaceNumber, get the number of alternate settings
 * available for that interface (i.e., the max possible value of bAlternateSetting for that bInterfaceNumber).
 *
 * @param[in] config_desc Pointer to the start of a full configuration descriptor
 * @param[in] bInterfaceNumber Interface number
 * @return int The number of alternate settings that the interface has, -1 if bInterfaceNumber not found
 */
int usb_parse_interface_number_of_alternate(const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber);

/**
 * @brief Get a particular interface descriptor (using bInterfaceNumber and bAlternateSetting)
 *
 * Given a full configuration descriptor, get a particular interface descriptor.
 *
 * @note To get the number of alternate settings for a particular bInterfaceNumber, call
 *       usb_parse_interface_number_of_alternate()
 *
 * @param[in] config_desc Pointer to the start of a full configuration descriptor
 * @param[in] bInterfaceNumber Interface number
 * @param[in] bAlternateSetting Alternate setting number
 * @param[out] offset Byte offset of the interface descriptor relative to the start of the configuration descriptor. Can be NULL.
 * @return const usb_intf_desc_t* Pointer to interface descriptor, NULL if not found.
 */
const usb_intf_desc_t *usb_parse_interface_descriptor(const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber, uint8_t bAlternateSetting, int *offset);

/**
 * @brief Get an endpoint descriptor within an interface descriptor
 *
 * Given an interface descriptor, get the Nth endpoint descriptor of the interface. The number of endpoints in an
 * interface is indicated by the bNumEndpoints field of the interface descriptor.
 *
 * @note If bNumEndpoints is 0, it means the interface uses the default endpoint only
 *
 * @param[in] intf_desc Pointer to thee start of an interface descriptor
 * @param[in] index Endpoint index
 * @param[in] wTotalLength Total length of the containing configuration descriptor
 * @param[inout] offset Byte offset relative to the start of the configuration descriptor. On input, it is the offset
 *                      of the interface descriptor. On output, it is the offset of the endpoint descriptor.
 * @return const usb_ep_desc_t* Pointer to endpoint descriptor, NULL if not found.
 */
const usb_ep_desc_t *usb_parse_endpoint_descriptor_by_index(const usb_intf_desc_t *intf_desc, int index, uint16_t wTotalLength, int *offset);

/**
 * @brief Get an endpoint descriptor based on an endpoint's address
 *
 * Given a configuration descriptor, get an endpoint descriptor based on it's bEndpointAddress, bAlternateSetting, and
 * bInterfaceNumber.
 *
 * @param[in] config_desc Pointer to the start of a full configuration descriptor
 * @param[in] bInterfaceNumber Interface number
 * @param[in] bAlternateSetting Alternate setting number
 * @param[in] bEndpointAddress Endpoint address
 * @param[out] offset Byte offset of the endpoint descriptor relative to the start of the configuration descriptor. Can be NULL
 * @return const usb_ep_desc_t* Pointer to endpoint descriptor, NULL if not found.
 */
const usb_ep_desc_t *usb_parse_endpoint_descriptor_by_address(const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber, uint8_t bAlternateSetting, uint8_t bEndpointAddress, int *offset);

// ------------------------------------------------------ Misc ---------------------------------------------------------

/**
 * @brief Round up to an integer multiple of endpoint's MPS
 *
 * This is a convenience function to round up a size/length to an endpoint's MPS (Maximum packet size). This is useful
 * when calculating transfer or buffer lengths of IN endpoints.
 *
 * @param[in] num_bytes Number of bytes
 * @param[in] mps MPS
 * @return int Round up integer multiple of MPS
 */
static inline int usb_round_up_to_mps(int num_bytes, int mps)
{
    if (num_bytes < 0 || mps < 0) {
        return 0;
    }
    return ((num_bytes + mps - 1) / mps) * mps;
}

/**
 * @brief Print class specific descriptor callback
 *
 * Optional callback to be provided to usb_print_descriptors() function.
 * The callback is called when when a non-standard descriptor is encountered.
 * The callback should decode the descriptor as print it.
 */

typedef void (*print_class_descriptor_cb)(const usb_standard_desc_t *);

/**
 * @brief Prints usb descriptors
 *
 * @param[in] device Handle to device
 * @param[in] class_specific_cb Optional callback to print class specific descriptors
 * @return esp_err_t
 */
esp_err_t usb_print_descriptors(usb_device_handle_t device, print_class_descriptor_cb class_specific_cb);

#ifdef __cplusplus
}
#endif
