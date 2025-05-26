/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <catch2/catch_test_macros.hpp>

#include "esp_bit_defs.h"
#include "usb_host.h"   // Real implementation of usb_host.h

// Test all the mocked headers defined for this mock
extern "C" {
#include "Mockusb_phy.h"
#include "Mockhcd.h"
#include "Mockusbh.h"
#include "Mockenum.h"
#include "Mockhub.h"
}

SCENARIO("USB Host pre-uninstall")
{
    // No USB Host driver previously installed
    GIVEN("No USB Host previously installed") {

        // Uninstall not-installed USB Host
        SECTION("Uninstalling not installed USB Host") {

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_uninstall());
        }
    }
}

SCENARIO("USB Host install")
{
    // USB Host config is not valid, USB Host driver is not installed from previous test case
    GIVEN("No USB Host config, USB Host driver not installed") {

        // Try to install the USB Host driver with usb_host_config set to nullptr
        SECTION("USB Host config is nullptr") {

            // Call the DUT function, expect ESP_ERR_INVALID_ARG
            REQUIRE(ESP_ERR_INVALID_ARG == usb_host_install(nullptr));
        }
    }

    // USB Host config struct
    usb_host_config_t usb_host_config = {
        .skip_phy_setup = false,
        .root_port_unpowered = false,
        .intr_flags = 1,
        .enum_filter_cb = nullptr,
        .fifo_settings_custom = {},
        .peripheral_map = BIT0,
    };

    // USB host config is valid, USB Host driver is not installed from previous test case
    GIVEN("USB Host config, USB Host driver not installed") {

        // Try to install the USB Host driver, with PHY install error
        SECTION("Fail to install USB Host - PHY Install error") {

            // Make the PHY install to fail
            usb_new_phy_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_STATE);

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_install(&usb_host_config));
        }

        // Try to install the USB Host driver, with HCD Install error, use internal PHY
        SECTION("Fail to install USB Host - HCD Install error (internal PHY)") {

            // Make the PHY install to pass
            usb_phy_handle_t phy_handle;
            usb_new_phy_ExpectAnyArgsAndReturn(ESP_OK);
            usb_new_phy_ReturnThruPtr_handle_ret(&phy_handle);

            // make the HCD port install to fail
            hcd_install_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_STATE);

            // goto hcd_err: We must uninstall the PHY
            usb_del_phy_ExpectAndReturn(phy_handle, ESP_OK);

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_install(&usb_host_config));
        }

        // Try to install the USB Host driver, with HCD Install error, use external PHY
        SECTION("Fail to install USB Host - HCD Install error (external PHY)") {

            // Skip the PHY Setup (external phy)
            usb_host_config.skip_phy_setup = true;

            // make the HCD port install to fail
            hcd_install_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_STATE);

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_install(&usb_host_config));
        }

        // Try to install the USB Host driver, with USBH Install error, use internal PHY
        SECTION("Fail to install USB Host - USBH install error") {

            // Make the PHY install to pass
            usb_phy_handle_t phy_handle;
            usb_new_phy_ExpectAnyArgsAndReturn(ESP_OK);
            usb_new_phy_ReturnThruPtr_handle_ret(&phy_handle);

            // make the HCD port install to pass
            hcd_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the USBH install to fail
            usbh_install_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_STATE);

            // goto usbh_err: We must uninstall HCD port and PHY
            hcd_uninstall_ExpectAndReturn(ESP_OK);
            usb_del_phy_ExpectAndReturn(phy_handle, ESP_OK);

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_install(&usb_host_config));
        }

        // Try to install the USB Host driver, with Enum driver Install error, use internal PHY
        SECTION("Fail to install USB Host - Enum driver install error") {

            // Make the PHY install to pass
            usb_phy_handle_t phy_handle;
            usb_new_phy_ExpectAnyArgsAndReturn(ESP_OK);
            usb_new_phy_ReturnThruPtr_handle_ret(&phy_handle);

            // make the HCD port install to pass
            hcd_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the USBH install to pass
            usbh_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the ENUM Driver to fail
            enum_install_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_STATE);

            // goto enum_err: We must uninstall USBH, HCD port and PHY
            usbh_uninstall_ExpectAndReturn(ESP_OK);
            hcd_uninstall_ExpectAndReturn(ESP_OK);
            usb_del_phy_ExpectAndReturn(phy_handle, ESP_OK);

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_install(&usb_host_config));
        }

        // Try to install the USB Host driver, with Hub driver Install error, use internal PHY
        SECTION("Fail to install USB Host - Hub driver install error") {

            // Make the PHY install to pass
            usb_phy_handle_t phy_handle;
            usb_new_phy_ExpectAnyArgsAndReturn(ESP_OK);
            usb_new_phy_ReturnThruPtr_handle_ret(&phy_handle);

            // make the HCD port install to pass
            hcd_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the USBH install to pass
            usbh_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the ENUM Driver to pass
            enum_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the HUB Driver to fail
            hub_install_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_STATE);

            // goto hub_err: We must uninstall Enum driver, USBH, HCD port and PHY
            enum_uninstall_ExpectAndReturn(ESP_OK);
            usbh_uninstall_ExpectAndReturn(ESP_OK);
            hcd_uninstall_ExpectAndReturn(ESP_OK);
            usb_del_phy_ExpectAndReturn(phy_handle, ESP_OK);

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_install(&usb_host_config));
        }

        // Successfully install the USB Host driver
        SECTION("Successfully install the USB Host driver") {

            // Make the PHY install to pass
            usb_phy_handle_t phy_handle;
            usb_new_phy_ExpectAnyArgsAndReturn(ESP_OK);
            usb_new_phy_ReturnThruPtr_handle_ret(&phy_handle);

            // make the HCD port install to pass
            hcd_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the USBH install to pass
            usbh_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the ENUM Driver to pass
            enum_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make the HUB Driver to pass
            hub_install_ExpectAnyArgsAndReturn(ESP_OK);

            // Make hub_root_start() to pass
            hub_root_start_ExpectAndReturn(ESP_OK);

            // Call the DUT function, expect ESP_OK
            REQUIRE(ESP_OK == usb_host_install(&usb_host_config));
        }
    }

    // USB Host config is valid, USB Host driver was successfully installed in previous test case
    GIVEN("USB Host config, USB Host driver previously installed") {

        // Try to install USB Host driver again, after it has been successfully installed
        SECTION("Fail to install already installed USB Host driver") {

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_install(&usb_host_config));
        }
    }

}

SCENARIO("USB Host post-uninstall")
{
    // USB Host driver successfully installed from previous test case
    GIVEN("USB Host previously installed") {

        // Uninstall successfully installed USB Host driver
        SECTION("Successfully uninstall the USB Host driver") {

            // Make the hub_root_stop() to pass
            hub_root_stop_ExpectAndReturn(ESP_OK);

            // Make uninstalling of all the drivers to pass
            hub_uninstall_ExpectAndReturn(ESP_OK);
            enum_uninstall_ExpectAndReturn(ESP_OK);
            usbh_uninstall_ExpectAndReturn(ESP_OK);
            hcd_uninstall_ExpectAndReturn(ESP_OK);

            // Make the usb_del_phy() to pass
            usb_del_phy_ExpectAnyArgsAndReturn(ESP_OK);

            // Call the DUT function, expect ESP_OK
            REQUIRE(ESP_OK == usb_host_uninstall());
        }
    }

    // USB Host driver successfully uninstalled from previous test case
    GIVEN("USB Host successfully uninstalled") {

        // USB Host successfully uninstalled, try to uninstall it again
        SECTION("Uninstall already uninstalled USB Host driver") {

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usb_host_uninstall());
        }
    }
}
