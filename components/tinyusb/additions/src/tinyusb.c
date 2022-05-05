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
    if (config->external_phy) {
        phy_conf.target = USB_PHY_TARGET_EXT;
        usb_phy_gpio_conf_t gpio_conf = {
            .vp_io_num = USBPHY_VP_NUM,
            .vm_io_num = USBPHY_VM_NUM,
            .rcv_io_num = USBPHY_RCV_NUM,
            .oen_io_num = USBPHY_OEN_NUM,
            .vpo_io_num = USBPHY_VPO_NUM,
            .vmo_io_num = USBPHY_VMO_NUM,
        };
        phy_conf.gpio_conf = &gpio_conf;
    } else {
        phy_conf.target = USB_PHY_TARGET_INT;
    }
    ESP_RETURN_ON_ERROR(usb_new_phy(&phy_conf, &phy_hdl), TAG, "Install USB PHY failed");

    dev_descriptor = config->device_descriptor ? config->device_descriptor : &descriptor_dev_kconfig;
    string_descriptor = config->string_descriptor ? config->string_descriptor : descriptor_str_kconfig;
    cfg_descriptor = config->configuration_descriptor ? config->configuration_descriptor : descriptor_cfg_kconfig;

    tusb_set_descriptor(dev_descriptor, string_descriptor, cfg_descriptor);

    ESP_RETURN_ON_FALSE(tusb_init(), ESP_FAIL, TAG, "Init TinyUSB stack failed");
#if !CONFIG_TINYUSB_NO_DEFAULT_TASK
    ESP_RETURN_ON_ERROR(tusb_run_task(), TAG, "Run TinyUSB task failed");
#endif
    ESP_LOGI(TAG, "TinyUSB Driver installed");
    return ESP_OK;
}
