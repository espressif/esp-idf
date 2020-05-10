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

#include "tinyusb.h"
#include "hal/usb_hal.h"
#include "soc/usb_periph.h"
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "esp32s2/rom/gpio.h"

static void configure_pins(usb_hal_context_t *usb)
{
    /* usb_periph_iopins currently configures USB_OTG as USB Device.
     * Introduce additional parameters in usb_hal_context_t when adding support
     * for USB Host.
     */
    for (const usb_iopin_dsc_t* iopin = usb_periph_iopins; iopin->pin != -1; ++iopin) {
        if ((usb->use_external_phy) || (iopin->ext_phy_only == 0)) {
            gpio_pad_select_gpio(iopin->pin);
            if (iopin->is_output) {
                gpio_matrix_out(iopin->pin, iopin->func, false, false);
            } else {
                gpio_matrix_in(iopin->pin, iopin->func, false);
                gpio_pad_input_enable(iopin->pin);
            }
            gpio_pad_unhold(iopin->pin);
        }
    }
    if (!usb->use_external_phy) {
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
    }
}


/**
 * @brief Initializes the tinyUSB driver.
 *
 * Note: Do not change any Custom descriptor, but
 * if it used it is recomended to define: bDeviceClass = TUSB_CLASS_MISC,
 * bDeviceSubClass = MISC_SUBCLASS_COMMON and bDeviceClass = TUSB_CLASS_MISC
 * to match with Interface Association Descriptor (IAD) for CDC
 *
 * @param config if equal to NULL the default descriptor will be used
 * @return esp_err_t Errors during the initialization
 */
esp_err_t tinyusb_driver_install(const tinyusb_config_t *config)
{
    tusb_desc_device_t *descriptor;
    char **string_descriptor;

    periph_module_reset(PERIPH_USB_MODULE);
    periph_module_enable(PERIPH_USB_MODULE);

    // Hal init
    usb_hal_context_t hal = {
        .use_external_phy = config->external_phy
    };
    usb_hal_init(&hal);
    configure_pins(&hal);

    if (config->descriptor == NULL) {
        descriptor = &descriptor_kconfig;
    } else {
        descriptor = config->descriptor;
    }


    if (config->string_descriptor == NULL) {
        string_descriptor = descriptor_str_kconfig;
    } else {
        string_descriptor = config->string_descriptor;
    }

    tusb_set_descriptor(descriptor,
                        string_descriptor);

    ESP_ERROR_CHECK(tusb_init());
    return ESP_OK;
}
