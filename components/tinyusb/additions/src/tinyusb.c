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

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_private/usb_phy.h"
#include "soc/usb_pins.h"
#include "tinyusb.h"
#include "descriptors_control.h"
#include "tusb.h"
#include "tusb_tasks.h"

const static char *TAG = "TinyUSB";
static usb_phy_handle_t phy_hdl;

esp_err_t tinyusb_driver_install(const tinyusb_config_t *config)
{
    tusb_desc_device_t *dev_descriptor;
    const char **string_descriptor;
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
