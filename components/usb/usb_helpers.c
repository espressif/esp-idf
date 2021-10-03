/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "usb/usb_helpers.h"
#include "usb/usb_types_ch9.h"

// ---------------------------------------- Configuration Descriptor Parsing -------------------------------------------

const usb_standard_desc_t *usb_parse_next_descriptor(const usb_standard_desc_t *cur_desc, uint16_t wTotalLength, int *offset)
{
    assert(cur_desc != NULL && offset != NULL);
    if (*offset >= wTotalLength) {
        return NULL;    //We have traversed the entire configuration descriptor
    }
    if (*offset + cur_desc->bLength >= wTotalLength) {
        return NULL;    //Next descriptor is out of bounds
    }
    //Return the next descriptor, update offset
    const usb_standard_desc_t *ret_desc = (const usb_standard_desc_t *)(((uint32_t)cur_desc) + cur_desc->bLength);
    *offset += cur_desc->bLength;
    return ret_desc;
}

const usb_standard_desc_t *usb_parse_next_descriptor_of_type(const usb_standard_desc_t *cur_desc, uint16_t wTotalLength, uint8_t bDescriptorType, int *offset)
{
    assert(cur_desc != NULL && offset != NULL);
    int offset_temp = *offset;      //We only want to update offset if we've actually found a descriptor
    //Keep stepping over descriptors until we find one of bDescriptorType or until we go out of bounds
    const usb_standard_desc_t *ret_desc = usb_parse_next_descriptor(cur_desc, wTotalLength, &offset_temp);
    while (ret_desc != NULL) {
        if (ret_desc->bDescriptorType == bDescriptorType) {
            break;
        }
        ret_desc = usb_parse_next_descriptor(ret_desc, wTotalLength, &offset_temp);
    }
    if (ret_desc != NULL) {
        //We've found a descriptor. Update the offset
        *offset = offset_temp;
    }
    return ret_desc;
}

int usb_parse_interface_number_of_alternate(const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber)
{
    assert(config_desc != NULL);
    int offset = 0;
    //Find the first interface descriptor of bInterfaceNumber
    const usb_intf_desc_t *first_intf_desc = usb_parse_interface_descriptor(config_desc, bInterfaceNumber, 0, &offset);
    if (first_intf_desc == NULL) {
        return -1;  //bInterfaceNumber not found
    }

    int num_alt_setting = 0;
    const usb_intf_desc_t *next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)first_intf_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset);
    while (next_intf_desc != NULL) {
        if (next_intf_desc->bInterfaceNumber != bInterfaceNumber) {
            break;
        }
        num_alt_setting++;
        next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)next_intf_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset);
    }
    return num_alt_setting;
}

const usb_intf_desc_t *usb_parse_interface_descriptor(const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber, uint8_t bAlternateSetting, int *offset)
{
    assert(config_desc != NULL);
    if (bInterfaceNumber >= config_desc->bNumInterfaces) {
        return NULL;    //bInterfaceNumber is out of range
    }

    //Walk to first interface descriptor of bInterfaceNumber
    int offset_temp = 0;
    const usb_intf_desc_t *next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)config_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset_temp);
    while (next_intf_desc != NULL) {
        if (next_intf_desc->bInterfaceNumber == bInterfaceNumber) {
            break;      //We found the first interface descriptor with matching bInterfaceNumber
        }
        next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)next_intf_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset_temp);
    }
    if (next_intf_desc == NULL) {
        return NULL;    //Couldn't find a interface with bInterfaceNumber
    }

    //Keep walking until an interface descriptor matching bInterfaceNumber and bAlternateSetting is found
    while (next_intf_desc != NULL) {
        if (next_intf_desc->bInterfaceNumber == bInterfaceNumber + 1) {
            //We've walked past our target bInterfaceNumber
            next_intf_desc = NULL;
            break;
        }
        if (next_intf_desc->bAlternateSetting == bAlternateSetting) {
            //We've found our target interface descriptor
            break;
        }
        //Get the next interface descriptor
        next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)next_intf_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset_temp);
    }
    if (next_intf_desc != NULL && offset != NULL) {
        *offset = offset_temp;
    }
    return next_intf_desc;
}

const usb_ep_desc_t *usb_parse_endpoint_descriptor_by_index(const usb_intf_desc_t *intf_desc, int index, uint16_t wTotalLength, int *offset)
{
    assert(intf_desc != NULL && offset != NULL);
    if (index >= intf_desc->bNumEndpoints) {
        return NULL;    //Index is out of range
    }
    //Walk to the Nth endpoint descriptor we find
    int offset_temp = *offset;
    bool ep_found = true;
    const usb_standard_desc_t *next_desc = (const usb_standard_desc_t *)intf_desc;
    for (int i = 0; i <= index; i++) {
        next_desc = usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)next_desc, wTotalLength, USB_B_DESCRIPTOR_TYPE_ENDPOINT, &offset_temp);
        if (next_desc == NULL) {
            ep_found = false;
            break;
        }
    }
    if (ep_found) {
        *offset = offset_temp;
        return (const usb_ep_desc_t *)next_desc;
    }
    return NULL;
}

const usb_ep_desc_t *usb_parse_endpoint_descriptor_by_address(const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber, uint8_t bAlternateSetting, uint8_t bEndpointAddress, int *offset)
{
    assert(config_desc != NULL);

    //Find the interface descriptor
    int offset_intf;
    const usb_intf_desc_t *intf_desc = usb_parse_interface_descriptor(config_desc, bInterfaceNumber, bAlternateSetting, &offset_intf);
    if (intf_desc == NULL) {
        return NULL;
    }

    //Walk endpoint descriptors until one matching bEndpointAddress is found
    int offset_ep;
    bool ep_found = false;
    const usb_ep_desc_t *ep_desc = NULL;
    for (int index = 0; index < intf_desc->bNumEndpoints; index++) {
        offset_ep = offset_intf;
        ep_desc = usb_parse_endpoint_descriptor_by_index(intf_desc, index, config_desc->wTotalLength, &offset_ep);
        if (ep_desc == NULL) {
            break;
        }
        if (ep_desc->bEndpointAddress == bEndpointAddress) {
            ep_found = true;
            break;
        }
    }
    if (ep_found && offset != NULL) {
        *offset = offset_ep;
    }
    return ep_desc;
}

// ------------------------------------------------------ Misc ---------------------------------------------------------
