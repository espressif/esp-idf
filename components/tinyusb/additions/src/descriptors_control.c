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

#include "esp_log.h"
#include "descriptors_control.h"

static const char *TAG = "tusb_desc";
static tusb_desc_device_t s_descriptor;
static char *s_str_descriptor[USB_STRING_DESCRIPTOR_ARRAY_SIZE];
#define MAX_DESC_BUF_SIZE 32

#if CFG_TUD_HID //HID Report Descriptor
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE) )
};
#endif

uint8_t const desc_configuration[] = {
    // interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

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
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 6, HID_PROTOCOL_NONE, sizeof(desc_hid_report), 0x80 | EPNUM_HID, CFG_TUD_HID_BUFSIZE, 10)
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
    return desc_configuration;
}

static uint16_t _desc_str[MAX_DESC_BUF_SIZE];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void) langid;

    uint8_t chr_count;

    if ( index == 0) {
        memcpy(&_desc_str[1], s_str_descriptor[0], 2);
        chr_count = 1;
    } else {
        // Convert ASCII string into UTF-16

        if ( index >= sizeof(s_str_descriptor) / sizeof(s_str_descriptor[0]) ) {
            return NULL;
        }

        const char *str = s_str_descriptor[index];

        // Cap at max char
        chr_count = strlen(str);
        if ( chr_count > MAX_DESC_BUF_SIZE - 1 ) {
            chr_count = MAX_DESC_BUF_SIZE - 1;
        }

        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2 * chr_count + 2);

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
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf)
{
    return desc_hid_report;
}
#endif

// =============================================================================
// Driver functions
// =============================================================================

void tusb_set_descriptor(tusb_desc_device_t *dev_desc, const char **str_desc)
{
    ESP_LOGI(TAG, "\n"
             "┌─────────────────────────────────┐\n"
             "│  USB Device Descriptor Summary  │\n"
             "├───────────────────┬─────────────┤\n"
             "│bDeviceClass       │ %-4u        │\n"
             "├───────────────────┼─────────────┤\n"
             "│bDeviceSubClass    │ %-4u        │\n"
             "├───────────────────┼─────────────┤\n"
             "│bDeviceProtocol    │ %-4u        │\n"
             "├───────────────────┼─────────────┤\n"
             "│bMaxPacketSize0    │ %-4u        │\n"
             "├───────────────────┼─────────────┤\n"
             "│idVendor           │ %-#10x  │\n"
             "├───────────────────┼─────────────┤\n"
             "│idProduct          │ %-#10x  │\n"
             "├───────────────────┼─────────────┤\n"
             "│bcdDevice          │ %-#10x  │\n"
             "├───────────────────┼─────────────┤\n"
             "│iManufacturer      │ %-#10x  │\n"
             "├───────────────────┼─────────────┤\n"
             "│iProduct           │ %-#10x  │\n"
             "├───────────────────┼─────────────┤\n"
             "│iSerialNumber      │ %-#10x  │\n"
             "├───────────────────┼─────────────┤\n"
             "│bNumConfigurations │ %-#10x  │\n"
             "└───────────────────┴─────────────┘",
             dev_desc->bDeviceClass, dev_desc->bDeviceSubClass,
             dev_desc->bDeviceProtocol, dev_desc->bMaxPacketSize0,
             dev_desc->idVendor, dev_desc->idProduct, dev_desc->bcdDevice,
             dev_desc->iManufacturer, dev_desc->iProduct, dev_desc->iSerialNumber,
             dev_desc->bNumConfigurations);
    s_descriptor = *dev_desc;

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

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
}
