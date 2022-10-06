/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "hal/usb_phy_types.h"
#include "esp_private/usb_phy.h"
#include "test_usb_common.h"
#include "unity.h"

static usb_phy_handle_t phy_hdl = NULL;

void test_usb_init_phy(void)
{
    //Initialize the internal USB PHY to connect to the USB OTG peripheral
    usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_INT,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_UNDEFINED,   //In Host mode, the speed is determined by the connected device
        .ext_io_conf = NULL,
        .otg_io_conf = NULL,
    };
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, usb_new_phy(&phy_config, &phy_hdl), "Failed to init USB PHY");
}

void test_usb_deinit_phy(void)
{
    //Deinitialize the internal USB PHY
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, usb_del_phy(phy_hdl), "Failed to delete PHY");
    phy_hdl = NULL;
}

void test_usb_set_phy_state(bool connected, TickType_t delay_ticks)
{
    if (delay_ticks > 0) {
        //Delay of 0 ticks causes a yield. So skip if delay_ticks is 0.
        vTaskDelay(delay_ticks);
    }
    ESP_ERROR_CHECK(usb_phy_action(phy_hdl, (connected) ? USB_PHY_ACTION_HOST_ALLOW_CONN : USB_PHY_ACTION_HOST_FORCE_DISCONN));
}
