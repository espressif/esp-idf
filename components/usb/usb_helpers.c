/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "usb/usb_helpers.h"
#include "usb/usb_types_ch9.h"
#include "esp_check.h"
#include "usb/usb_host.h"

// ---------------------------------------- Configuration Descriptor Parsing -------------------------------------------

const usb_standard_desc_t *usb_parse_next_descriptor(const usb_standard_desc_t *cur_desc, uint16_t wTotalLength, int *offset)
{
    assert(cur_desc != NULL && offset != NULL);
    if (*offset >= wTotalLength) {
        return NULL;    // We have traversed the entire configuration descriptor
    }
    if (*offset + cur_desc->bLength >= wTotalLength) {
        return NULL;    // Next descriptor is out of bounds
    }
    // Return the next descriptor, update offset
    const usb_standard_desc_t *ret_desc = (const usb_standard_desc_t *)(((uintptr_t)cur_desc) + cur_desc->bLength);
    *offset += cur_desc->bLength;
    return ret_desc;
}

const usb_standard_desc_t *usb_parse_next_descriptor_of_type(const usb_standard_desc_t *cur_desc, uint16_t wTotalLength, uint8_t bDescriptorType, int *offset)
{
    assert(cur_desc != NULL && offset != NULL);
    int offset_temp = *offset;      // We only want to update offset if we've actually found a descriptor
    // Keep stepping over descriptors until we find one of bDescriptorType or until we go out of bounds
    const usb_standard_desc_t *ret_desc = usb_parse_next_descriptor(cur_desc, wTotalLength, &offset_temp);
    while (ret_desc != NULL) {
        if (ret_desc->bDescriptorType == bDescriptorType) {
            break;
        }
        ret_desc = usb_parse_next_descriptor(ret_desc, wTotalLength, &offset_temp);
    }
    if (ret_desc != NULL) {
        // We've found a descriptor. Update the offset
        *offset = offset_temp;
    }
    return ret_desc;
}

int usb_parse_interface_number_of_alternate(const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber)
{
    assert(config_desc != NULL);
    int offset = 0;
    // Find the first interface descriptor of bInterfaceNumber
    const usb_intf_desc_t *first_intf_desc = usb_parse_interface_descriptor(config_desc, bInterfaceNumber, 0, &offset);
    if (first_intf_desc == NULL) {
        return -1;  // bInterfaceNumber not found
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

    // Walk to first interface descriptor of bInterfaceNumber
    int offset_temp = 0;
    const usb_intf_desc_t *next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)config_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset_temp);
    while (next_intf_desc != NULL) {
        if (next_intf_desc->bInterfaceNumber == bInterfaceNumber) {
            break;      // We found the first interface descriptor with matching bInterfaceNumber
        }
        next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type((const usb_standard_desc_t *)next_intf_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset_temp);
    }
    if (next_intf_desc == NULL) {
        return NULL;    // Couldn't find a interface with bInterfaceNumber
    }

    // Keep walking until an interface descriptor matching bInterfaceNumber and bAlternateSetting is found
    while (next_intf_desc != NULL) {
        if (next_intf_desc->bInterfaceNumber == bInterfaceNumber + 1) {
            // We've walked past our target bInterfaceNumber
            next_intf_desc = NULL;
            break;
        }
        if (next_intf_desc->bAlternateSetting == bAlternateSetting) {
            // We've found our target interface descriptor
            break;
        }
        // Get the next interface descriptor
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
        return NULL;    // Index is out of range
    }
    // Walk to the Nth endpoint descriptor we find
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

    // Find the interface descriptor
    int offset_intf;
    const usb_intf_desc_t *intf_desc = usb_parse_interface_descriptor(config_desc, bInterfaceNumber, bAlternateSetting, &offset_intf);
    if (intf_desc == NULL) {
        return NULL;
    }

    // Walk endpoint descriptors until one matching bEndpointAddress is found
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

// ----------------------------------------------- Descriptor Printing -------------------------------------------------

static void print_ep_desc(const usb_ep_desc_t *ep_desc)
{
    const char *ep_type_str;
    int type = ep_desc->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK;

    switch (type) {
    case USB_BM_ATTRIBUTES_XFER_CONTROL:
        ep_type_str = "CTRL";
        break;
    case USB_BM_ATTRIBUTES_XFER_ISOC:
        ep_type_str = "ISOC";
        break;
    case USB_BM_ATTRIBUTES_XFER_BULK:
        ep_type_str = "BULK";
        break;
    case USB_BM_ATTRIBUTES_XFER_INT:
        ep_type_str = "INT";
        break;
    default:
        ep_type_str = NULL;
        break;
    }

    printf("\t\t*** Endpoint descriptor ***\n");
    printf("\t\tbLength %d\n", ep_desc->bLength);
    printf("\t\tbDescriptorType %d\n", ep_desc->bDescriptorType);
    printf("\t\tbEndpointAddress 0x%x\tEP %d %s\n", ep_desc->bEndpointAddress,
           USB_EP_DESC_GET_EP_NUM(ep_desc),
           USB_EP_DESC_GET_EP_DIR(ep_desc) ? "IN" : "OUT");
    printf("\t\tbmAttributes 0x%x\t%s\n", ep_desc->bmAttributes, ep_type_str);
    printf("\t\twMaxPacketSize %d\n", USB_EP_DESC_GET_MPS(ep_desc));
    printf("\t\tbInterval %d\n", ep_desc->bInterval);
}

static void usbh_print_intf_desc(const usb_intf_desc_t *intf_desc)
{
    printf("\t*** Interface descriptor ***\n");
    printf("\tbLength %d\n", intf_desc->bLength);
    printf("\tbDescriptorType %d\n", intf_desc->bDescriptorType);
    printf("\tbInterfaceNumber %d\n", intf_desc->bInterfaceNumber);
    printf("\tbAlternateSetting %d\n", intf_desc->bAlternateSetting);
    printf("\tbNumEndpoints %d\n", intf_desc->bNumEndpoints);
    printf("\tbInterfaceClass 0x%x\n", intf_desc->bInterfaceClass);
    printf("\tbInterfaceSubClass 0x%x\n", intf_desc->bInterfaceSubClass);
    printf("\tbInterfaceProtocol 0x%x\n", intf_desc->bInterfaceProtocol);
    printf("\tiInterface %d\n", intf_desc->iInterface);
}

static void usbh_print_cfg_desc(const usb_config_desc_t *cfg_desc)
{
    printf("*** Configuration descriptor ***\n");
    printf("bLength %d\n", cfg_desc->bLength);
    printf("bDescriptorType %d\n", cfg_desc->bDescriptorType);
    printf("wTotalLength %d\n", cfg_desc->wTotalLength);
    printf("bNumInterfaces %d\n", cfg_desc->bNumInterfaces);
    printf("bConfigurationValue %d\n", cfg_desc->bConfigurationValue);
    printf("iConfiguration %d\n", cfg_desc->iConfiguration);
    printf("bmAttributes 0x%x\n", cfg_desc->bmAttributes);
    printf("bMaxPower %dmA\n", cfg_desc->bMaxPower * 2);
}

static void print_iad_desc(const usb_iad_desc_t *iad_desc)
{
    printf("*** Interface Association Descriptor ***\n");
    printf("bLength %d\n", iad_desc->bLength);
    printf("bDescriptorType %d\n", iad_desc->bDescriptorType);
    printf("bFirstInterface %d\n", iad_desc->bFirstInterface);
    printf("bInterfaceCount %d\n", iad_desc->bInterfaceCount);
    printf("bFunctionClass 0x%x\n", iad_desc->bFunctionClass);
    printf("bFunctionSubClass 0x%x\n", iad_desc->bFunctionSubClass);
    printf("bFunctionProtocol 0x%x\n", iad_desc->bFunctionProtocol);
    printf("iFunction %d\n", iad_desc->iFunction);
}

void usb_print_device_descriptor(const usb_device_desc_t *devc_desc)
{
    if (devc_desc == NULL) {
        return;
    }

    printf("*** Device descriptor ***\n");
    printf("bLength %d\n", devc_desc->bLength);
    printf("bDescriptorType %d\n", devc_desc->bDescriptorType);
    printf("bcdUSB %d.%d0\n", ((devc_desc->bcdUSB >> 8) & 0xF), ((devc_desc->bcdUSB >> 4) & 0xF));
    printf("bDeviceClass 0x%x\n", devc_desc->bDeviceClass);
    printf("bDeviceSubClass 0x%x\n", devc_desc->bDeviceSubClass);
    printf("bDeviceProtocol 0x%x\n", devc_desc->bDeviceProtocol);
    printf("bMaxPacketSize0 %d\n", devc_desc->bMaxPacketSize0);
    printf("idVendor 0x%x\n", devc_desc->idVendor);
    printf("idProduct 0x%x\n", devc_desc->idProduct);
    printf("bcdDevice %d.%d0\n", ((devc_desc->bcdDevice >> 8) & 0xF), ((devc_desc->bcdDevice >> 4) & 0xF));
    printf("iManufacturer %d\n", devc_desc->iManufacturer);
    printf("iProduct %d\n", devc_desc->iProduct);
    printf("iSerialNumber %d\n", devc_desc->iSerialNumber);
    printf("bNumConfigurations %d\n", devc_desc->bNumConfigurations);
}

void usb_print_config_descriptor(const usb_config_desc_t *cfg_desc, print_class_descriptor_cb class_specific_cb)
{
    if (cfg_desc == NULL) {
        return;
    }

    int offset = 0;
    uint16_t wTotalLength = cfg_desc->wTotalLength;
    const usb_standard_desc_t *next_desc = (const usb_standard_desc_t *)cfg_desc;

    do {
        switch (next_desc->bDescriptorType) {
        case USB_B_DESCRIPTOR_TYPE_CONFIGURATION:
            usbh_print_cfg_desc((const usb_config_desc_t *)next_desc);
            break;
        case USB_B_DESCRIPTOR_TYPE_INTERFACE:
            usbh_print_intf_desc((const usb_intf_desc_t *)next_desc);
            break;
        case USB_B_DESCRIPTOR_TYPE_ENDPOINT:
            print_ep_desc((const usb_ep_desc_t *)next_desc);
            break;
        case USB_B_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION:
            print_iad_desc((const usb_iad_desc_t *)next_desc);
            break;
        default:
            if (class_specific_cb) {
                class_specific_cb(next_desc);
            }
            break;
        }

        next_desc = usb_parse_next_descriptor(next_desc, wTotalLength, &offset);

    } while (next_desc != NULL);
}

void usb_print_string_descriptor(const usb_str_desc_t *str_desc)
{
    if (str_desc == NULL) {
        return;
    }

    for (int i = 0; i < str_desc->bLength / 2; i++) {
        /*
        USB String descriptors of UTF-16.
        Right now We just skip any character larger than 0xFF to stay in BMP Basic Latin and Latin-1 Supplement range.
        */
        if (str_desc->wData[i] > 0xFF) {
            continue;
        }
        printf("%c", (char)str_desc->wData[i]);
    }
    printf("\n");
}
