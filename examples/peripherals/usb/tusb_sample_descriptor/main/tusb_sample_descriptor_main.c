/* USB Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "sdkconfig.h"

static const char *TAG = "example";

void app_main(void)
{
    ESP_LOGI(TAG, "USB initialization");

#if CONFIG_EXAMPLE_MANUAL_DESC
    // Setting of descriptor. You can use descriptor_tinyusb and
    // descriptor_str_tinyusb as a reference
    tusb_desc_device_t my_descriptor = {
        .bLength = sizeof(my_descriptor),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = 0x0200, // USB version. 0x0200 means version 2.0
        .bDeviceClass = TUSB_CLASS_UNSPECIFIED,
        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = 0x303A,
        .idProduct = 0x3000,
        .bcdDevice = 0x0101, // Device FW version

        .iManufacturer = 0x01, // see string_descriptor[1] bellow
        .iProduct = 0x02,      // see string_descriptor[2] bellow
        .iSerialNumber = 0x03, // see string_descriptor[3] bellow

        .bNumConfigurations = 0x01
    };

    tusb_desc_strarray_device_t my_string_descriptor = {
        // array of pointer to string descriptors
        (char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
        "I",                  // 1: Manufacturer
        "My Custom Device",   // 2: Product
        "012-345",            // 3: Serials, should use chip ID
    };

    tinyusb_config_t tusb_cfg = {
        .descriptor = &my_descriptor,
        .string_descriptor = my_string_descriptor,
        .external_phy = false // In the most cases you need to use a `false` value
    };

#else

    tinyusb_config_t tusb_cfg = {
        .descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false // In the most cases you need to use a `false` value
    };

#endif

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB initialization DONE");
}
