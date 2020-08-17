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

#include "descriptors_control.h"
#include "esp_log.h"

static const char *TAG = "TUSB:descriptors_control";
static tusb_desc_device_t s_descriptor;
static char *s_str_descriptor[USB_STRING_DESCRIPTOR_ARRAY_SIZE];
bool tusb_desc_set;

#if CFG_TUD_HID //HID Report Descriptor
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD), ),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE), )
};
#endif

uint8_t const desc_configuration[] = {
    // interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, CONFIG_USB_MAX_POWER_USAGE),

#   if CFG_TUD_CDC
    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, 0x81, 8, 0x02, 0x82, 64),
#   endif
#   if CFG_TUD_MSC
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC, 0x80 | EPNUM_MSC, 64), // highspeed 512
#   endif
#   if CFG_TUD_HID
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 6, HID_PROTOCOL_NONE, sizeof(desc_hid_report), 0x84, 16, 10),
#   endif
#   if CFG_TUD_VENDOR
    // Interface number, string index, EP Out & IN address, EP size
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 7, EPNUM_VENDOR, 0x80 | EPNUM_VENDOR, 64),
#   endif
#   if CFG_TUD_MIDI
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 8, EPNUM_MIDI, 0x80 | EPNUM_MIDI, (CFG_TUSB_RHPORT0_MODE & OPT_MODE_HIGH_SPEED) ? 512 : 64),
#   endif
};

// =============================================================================
// CALLBACKS
// =============================================================================

/**
 * @brief Invoked when received GET DEVICE DESCRIPTOR.
 * Application returns pointer to descriptor
 *
 * @return uint8_t const*
 */
uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&s_descriptor;
}

/**
 * @brief Invoked when received GET CONFIGURATION DESCRIPTOR.
 * Descriptor contents must exist long enough for transfer to complete
 *
 * @param index
 * @return uint8_t const* Application return pointer to descriptor
 */
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index; // for multiple configurations
    ESP_LOG_BUFFER_HEX("", desc_configuration, 100);
    return desc_configuration;
}

static uint16_t _desc_str[32];

/**
 * @brief Invoked when received GET STRING DESCRIPTOR request.
 * Application returns pointer to descriptor, whose contents must exist long
 * enough for transfer to complete
 *
 * @param index
 * @return uint16_t const*
 */
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], s_str_descriptor[0], 2);
        chr_count = 1;
    } else {
        // Convert ASCII string into UTF-16
        if (index >= sizeof(s_str_descriptor) /
                sizeof(s_str_descriptor[0])) {
            return NULL;
        }
        const char *str = s_str_descriptor[index];
        // Cap at max char
        chr_count = strlen(str);
        if (chr_count > 31) {
            chr_count = 31;
        }
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is len, second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

    return _desc_str;
}

/**
 * @brief Invoked when received GET HID REPORT DESCRIPTOR
 * Application returns pointer to descriptor. Descriptor contents must exist
 * long enough for transfer to complete
 *
 * @return uint8_t const*
 */
#if CFG_TUD_HID
uint8_t const *tud_hid_descriptor_report_cb(void)
{
    return desc_hid_report;
}
#endif

// =============================================================================
// Driver functions
// =============================================================================

void tusb_set_descriptor(tusb_desc_device_t *desc, char **str_desc)
{
    ESP_LOGI(TAG, "Setting of a descriptor: \n"
             ".bDeviceClass       = %u\n"
             ".bDeviceSubClass    = %u,\n"
             ".bDeviceProtocol    = %u,\n"
             ".bMaxPacketSize0    = %u,\n"
             ".idVendor           = 0x%08x,\n"
             ".idProduct          = 0x%08x,\n"
             ".bcdDevice          = 0x%08x,\n"
             ".iManufacturer      = 0x%02x,\n"
             ".iProduct           = 0x%02x,\n"
             ".iSerialNumber      = 0x%02x,\n"
             ".bNumConfigurations = 0x%02x\n",
             desc->bDeviceClass, desc->bDeviceSubClass,
             desc->bDeviceProtocol, desc->bMaxPacketSize0,
             desc->idVendor, desc->idProduct, desc->bcdDevice,
             desc->iManufacturer, desc->iProduct, desc->iSerialNumber,
             desc->bNumConfigurations);
    s_descriptor = *desc;

    if (str_desc != NULL) {
        memcpy(s_str_descriptor, str_desc,
               sizeof(s_str_descriptor[0])*USB_STRING_DESCRIPTOR_ARRAY_SIZE);
    }
    tusb_desc_set = true;
}

tusb_desc_device_t *tusb_get_active_desc(void)
{
    return &s_descriptor;
}

char **tusb_get_active_str_desc(void)
{
    return s_str_descriptor;
}

void tusb_clear_descriptor(void)
{
    memset(&s_descriptor, 0, sizeof(s_descriptor));
    memset(&s_str_descriptor, 0, sizeof(s_str_descriptor));
    tusb_desc_set = false;
}
