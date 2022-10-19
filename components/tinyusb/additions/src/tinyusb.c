/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/usb_phy.h"
#include "soc/usb_pins.h"
#include "tinyusb.h"
#include "descriptors_control.h"
#include "usb_descriptors.h"
#include "tusb.h"
#include "tusb_tasks.h"

const static char *TAG = "TinyUSB";
static usb_phy_handle_t phy_hdl;

esp_err_t tinyusb_driver_install(const tinyusb_config_t *config)
{
    const tusb_desc_device_t *dev_descriptor;
    const char **string_descriptor;
    const uint8_t *cfg_descriptor;
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // Configure USB PHY
    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_OTG,
        .otg_mode = USB_OTG_MODE_DEVICE,
    };

    // External PHY IOs config
    usb_phy_ext_io_conf_t ext_io_conf = {
        .vp_io_num = USBPHY_VP_NUM,
        .vm_io_num = USBPHY_VM_NUM,
        .rcv_io_num = USBPHY_RCV_NUM,
        .oen_io_num = USBPHY_OEN_NUM,
        .vpo_io_num = USBPHY_VPO_NUM,
        .vmo_io_num = USBPHY_VMO_NUM,
    };
    if (config->external_phy) {
        phy_conf.target = USB_PHY_TARGET_EXT;
        phy_conf.ext_io_conf = &ext_io_conf;
    } else {
        phy_conf.target = USB_PHY_TARGET_INT;
    }

    // OTG IOs config
    const usb_phy_otg_io_conf_t otg_io_conf = USB_PHY_SELF_POWERED_DEVICE(config->vbus_monitor_io);
    if (config->self_powered) {
        phy_conf.otg_io_conf = &otg_io_conf;
    }
    ESP_RETURN_ON_ERROR(usb_new_phy(&phy_conf, &phy_hdl), TAG, "Install USB PHY failed");

    if (config->configuration_descriptor) {
        cfg_descriptor = config->configuration_descriptor;
    } else {
#if (CONFIG_TINYUSB_HID_COUNT > 0 || CONFIG_TINYUSB_MIDI_COUNT > 0)
        // For HID device, configuration descriptor must be provided
        ESP_RETURN_ON_FALSE(config->configuration_descriptor, ESP_ERR_INVALID_ARG, TAG, "Configuration descriptor must be provided for this device");
#else
        cfg_descriptor = descriptor_cfg_kconfig;
        ESP_LOGW(TAG, "The device's configuration descriptor is not provided by user, using default.");
#endif
    }

    if (config->string_descriptor) {
        string_descriptor = config->string_descriptor;
    } else {
        string_descriptor = descriptor_str_kconfig;
        ESP_LOGW(TAG, "The device's string descriptor is not provided by user, using default.");
    }

    if (config->device_descriptor) {
        dev_descriptor = config->device_descriptor;
    } else {
        dev_descriptor = &descriptor_dev_kconfig;
        ESP_LOGW(TAG, "The device's device descriptor is not provided by user, using default.");
    }

    tusb_set_descriptor(dev_descriptor, string_descriptor, cfg_descriptor);

    ESP_RETURN_ON_FALSE(tusb_init(), ESP_FAIL, TAG, "Init TinyUSB stack failed");
#if !CONFIG_TINYUSB_NO_DEFAULT_TASK
    ESP_RETURN_ON_ERROR(tusb_run_task(), TAG, "Run TinyUSB task failed");
#endif
    ESP_LOGI(TAG, "TinyUSB Driver installed");
    return ESP_OK;
}
