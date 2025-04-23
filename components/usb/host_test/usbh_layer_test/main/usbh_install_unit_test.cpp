/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <catch2/catch_test_macros.hpp>

#include "usbh.h"   // Real implementation of usbh.h

// Test all the mocked headers defined for this mock
extern "C" {
#include "Mockhcd.h"
#include "Mockusb_private.h"
}

SCENARIO("USBH pre-uninstall")
{
    // No USBH driver previously installed
    GIVEN("No USBH previously installed") {

        // Uninstall not-installed USBH driver
        SECTION("Uninstalling not installed USBH driver") {

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usbh_uninstall());
        }
    }
}

SCENARIO("USBH install")
{
    // USBH config is not valid, USBH driver is not previously installed
    GIVEN("No USBH config, USBH driver not installed") {

        // Try to install the USBH driver with usbh_config set to nullptr
        SECTION("USBH config is nullptr") {

            // Call the DUT function, expect ESP_ERR_INVALID_ARG
            REQUIRE(ESP_ERR_INVALID_ARG == usbh_install(nullptr));
        }
    }

    // USBH config struct
    usbh_config_t usbh_config = {};

    // USBH config is valid, USBH driver is not previously installed
    GIVEN("USBH config, USBH driver not installed") {

        // Successfully install the USBH Driver
        SECTION("Successfully install the USBH Driver") {

            // Call the DUT function, expect ESP_OK
            REQUIRE(ESP_OK == usbh_install(&usbh_config));
        }
    }

    // USBH config is valid, USBH driver is previously installed
    GIVEN("USBH config, USBH driver previously installed") {

        SECTION("Fail to install already installed USBH driver") {

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usbh_install(&usbh_config));
        }
    }
}

SCENARIO("USBH post-uninstall")
{

    // USBH driver successfully installed from previous test case
    GIVEN("USBH driver previously installed") {

        // Uninstall successfully installed USBH driver
        SECTION("Successfully uninstall the USBH driver") {

            // Call the DUT function, expect ESP_OK
            REQUIRE(ESP_OK == usbh_uninstall());
        }
    }

    // USBH driver successfully uninstalled from previous test case
    GIVEN("USBH successfully uninstalled") {

        // USBH successfully uninstalled, try to uninstall it again
        SECTION("Uninstall already uninstalled USBH driver") {

            // Call the DUT function, expect ESP_ERR_INVALID_STATE
            REQUIRE(ESP_ERR_INVALID_STATE == usbh_uninstall());
        }
    }
}
