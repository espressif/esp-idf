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
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp32s2/rom/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_ll.h"
#include "hal/usb_hal.h"
#include "soc/gpio_periph.h"
#include "soc/usb_periph.h"
#include "tinyusb.h"
#include "descriptors_control.h"
#include "tusb.h"
#include "tusb_tasks.h"
#include "sdkconfig.h"

const static char *TAG = "TinyUSB";

static void configure_pins(usb_hal_context_t *usb)
{
    /* usb_periph_iopins currently configures USB_OTG as USB Device.
     * Introduce additional parameters in usb_hal_context_t when adding support
     * for USB Host.
     */
    for (const usb_iopin_dsc_t *iopin = usb_periph_iopins; iopin->pin != -1; ++iopin) {
        if ((usb->use_external_phy) || (iopin->ext_phy_only == 0)) {
            esp_rom_gpio_pad_select_gpio(iopin->pin);
            if (iopin->is_output) {
                esp_rom_gpio_connect_out_signal(iopin->pin, iopin->func, false, false);
            } else {
                esp_rom_gpio_connect_in_signal(iopin->pin, iopin->func, false);
                if ((iopin->pin != GPIO_FUNC_IN_LOW) && (iopin->pin != GPIO_FUNC_IN_HIGH)) {
                    gpio_ll_input_enable(&GPIO, iopin->pin);
                }
            }
            esp_rom_gpio_pad_unhold(iopin->pin);
        }
    }
    if (!usb->use_external_phy) {
        gpio_set_drive_capability(USBPHY_DM_NUM, GPIO_DRIVE_CAP_3);
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
    int res;
    char **string_descriptor;
    ESP_LOGI(TAG, "Driver installation...");

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

    res = tusb_init();
    if (res != TUSB_ERROR_NONE) {
        ESP_LOGE(TAG, "Can't initialize the TinyUSB stack. TinyUSB error: %d", res);
        return res;
    }
#if !CONFIG_USB_DO_NOT_CREATE_TASK
    res = tusb_run_task();
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Can't create the TinyUSB task.");
        return res;
    }
#endif
    ESP_LOGI(TAG, "Driver installed");
    return ESP_OK;
}
