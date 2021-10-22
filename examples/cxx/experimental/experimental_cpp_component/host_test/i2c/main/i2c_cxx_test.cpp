/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0
 *
 * I2C C++ unit tests
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/
#define CATCH_CONFIG_MAIN
#include <stdio.h>
#include "unity.h"
#include "freertos/portmacro.h"
#include "i2c_cxx.hpp"
#include "system_cxx.hpp"
#include "test_fixtures.hpp"

#include "catch.hpp"

extern "C" {
#include "Mocki2c.h"
}

// TODO: IDF-2693, function definition just to satisfy linker, mock esp_common instead
const char *esp_err_to_name(esp_err_t code) {
    return "host_test error";
}

using namespace std;
using namespace idf;

TEST_CASE("I2CBus")
{
    I2CBus bus(static_cast<i2c_port_t>(0));
}

TEST_CASE("I2CMaster parameter configuration fails")
{
    CMockFixture fix;
    i2c_param_config_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(I2CMaster(0, 1, 2, 400000), I2CException&);
}

TEST_CASE("I2CMaster driver install failure")
{
    CMockFixture fix;
    i2c_param_config_ExpectAnyArgsAndReturn(ESP_OK);
    i2c_driver_install_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(I2CMaster(0, 1, 2, 400000), I2CException&);
}
