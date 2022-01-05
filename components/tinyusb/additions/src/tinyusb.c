/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_ll.h"
#include "hal/usb_hal.h"
#include "soc/gpio_periph.h"
#include "soc/usb_periph.h"
#include "soc/gpio_pins.h"
#include "tinyusb.h"
#include "descriptors_control.h"
#include "tusb.h"
#include "tusb_tasks.h"

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
                if ((iopin->pin != GPIO_MATRIX_CONST_ZERO_INPUT) && (iopin->pin != GPIO_MATRIX_CONST_ONE_INPUT)) {
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

esp_err_t tinyusb_driver_install(const tinyusb_config_t *config)
{
    tusb_desc_device_t *dev_descriptor;
    const char **string_descriptor;
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // Enable APB CLK to USB peripheral
    periph_module_enable(PERIPH_USB_MODULE);
    periph_module_reset(PERIPH_USB_MODULE);
    // Initialize HAL layer
    usb_hal_context_t hal = {
        .use_external_phy = config->external_phy
    };
    usb_hal_init(&hal);
    configure_pins(&hal);

    dev_descriptor = config->descriptor ? config->descriptor : &descriptor_kconfig;
    string_descriptor = config->string_descriptor ? config->string_descriptor : descriptor_str_kconfig;

    tusb_set_descriptor(dev_descriptor, string_descriptor);

    ESP_RETURN_ON_FALSE(tusb_init(), ESP_FAIL, TAG, "Init TinyUSB stack failed");
#if !CONFIG_TINYUSB_NO_DEFAULT_TASK
    ESP_RETURN_ON_ERROR(tusb_run_task(), TAG, "Run TinyUSB task failed");
#endif
    ESP_LOGI(TAG, "TinyUSB Driver installed");
    return ESP_OK;
}
