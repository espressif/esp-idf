/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "usb/usb_types_ch9.h"
#include "esp_private/usb_phy.h"
#include "phy_common.h"
#include "unity.h"
#include "sdkconfig.h"

#ifdef CONFIG_USB_PHY_TEST_OTG_DRVVBUS_ENABLE
#define OTG_DRVVBUS_ENABLE
#define OTG_DRVVBUS_GPIO CONFIG_USB_PHY_TEST_OTG_DRVVBUS_GPIO
#endif

static usb_phy_handle_t phy_hdl = NULL;

void test_setup_usb_phy(usb_phy_target_t phy_target)
{
    // Deinitialize PHY from previous failed test
    if (phy_hdl != NULL) {
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, usb_del_phy(phy_hdl), "Failed to delete PHY");
        phy_hdl = NULL;
    }

    // Initialize the internal USB PHY to connect to the USB OTG peripheral
    usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = phy_target,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_UNDEFINED,   // In Host mode, the speed is determined by the connected device
        .ext_io_conf = NULL,
        .otg_io_conf = NULL,
    };

#ifdef OTG_DRVVBUS_ENABLE
    const usb_phy_otg_io_conf_t otg_io_conf = {
        .iddig_io_num = -1,
        .avalid_io_num = -1,
        .vbusvalid_io_num = -1,
        .idpullup_io_num = -1,
        .dppulldown_io_num = -1,
        .dmpulldown_io_num = -1,
        .drvvbus_io_num = OTG_DRVVBUS_GPIO,
        .bvalid_io_num = -1,
        .sessend_io_num = -1,
        .chrgvbus_io_num = -1,
        .dischrgvbus_io_num = -1
    };
    phy_config.otg_io_conf = &otg_io_conf;
#endif // OTG_DRVVBUS_ENABLE

    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, usb_new_phy(&phy_config, &phy_hdl), "Failed to init USB PHY");
}

void test_delete_usb_phy(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, usb_del_phy(phy_hdl), "Failed to delete PHY");
    phy_hdl = NULL;
}
