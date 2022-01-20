/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp32s2/rom/usb/usb_common.h"
#include "soc/soc.h"
#include "hal/efuse_hal.h"

/* USB CDC descriptor.
 * Note that we aren't using the one in ROM since it doesn't
 * set "serial" to the MAC address.
 * However overriding the descriptor is cheap - we can reuse most
 * of its components from ROM.
 */

/* This is not const, since the MAC address is only known at run time */
static struct string_descriptor s_str_serial_descr = {
    .bLength = 2 + 2 * 17,
    .bDescriptorType = 0x03,
    .bString={'0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0'}
};

static const struct rom_usb_descriptors s_acm_usb_descriptors_override = {
    .device_descr = &general_device_descr,
    .config_descr = { &acm_config_descr },
    .string_count = 4,
    .string0_descr = &string0_descr,
    .string_descrs = {
        &str_manu_descr,
        &str_prod_descr,
        &s_str_serial_descr
    }
};

static inline uint16_t nibble_to_hex_u16(uint8_t b)
{
    if (b < 0xa) {
        return '0' + b;
    } else {
        return 'a' + b - 0xa;
    }
}

void rom_usb_cdc_set_descriptor_patch(void)
{
    uint8_t mac_bytes[6];
    efuse_hal_get_mac(mac_bytes);
    /* Convert to UTF16 string */
#pragma GCC diagnostic push
#if     __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif
    uint16_t* dst = s_str_serial_descr.bString;
    for (int i = 0; i < 6; ++i) {
        uint8_t b = mac_bytes[5 - i]; /* printing from the MSB */
        *dst++ = nibble_to_hex_u16(b >> 4);
        *dst++ = nibble_to_hex_u16(b & 0xf);
        dst++;
    }
#pragma GCC diagnostic pop

    /* Override the pointer to descriptors structure */
    rom_usb_curr_desc = &s_acm_usb_descriptors_override;
}
