/* GPIO C++ unit tests

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#define CATCH_CONFIG_MAIN

#include <stdio.h>
#include "esp_err.h"
#include "freertos/portmacro.h"
#include "gpio_cxx.hpp"
#include "test_fixtures.hpp"

#include "catch.hpp"

extern "C" {
#include "Mockgpio.h"
}

// TODO: IDF-2693, function definition just to satisfy linker, mock esp_common instead
const char *esp_err_to_name(esp_err_t code) {
    return "test";
}

using namespace std;
using namespace idf;

TEST_CASE("gpio num out of range")
{
    CHECK_THROWS_AS(GPIONum(-1), GPIOException&);
    CHECK_THROWS_AS(GPIONum(static_cast<uint32_t>(GPIO_NUM_MAX)), GPIOException&);
    CHECK_THROWS_AS(GPIONum(24), GPIOException&); // On ESP32, 24 isn't a valid GPIO number
}

TEST_CASE("gpio num operator")
{
    GPIONum gpio_num_0(18u);
    GPIONum gpio_num_1(18u);
    GPIONum gpio_num_2(19u);

    CHECK(gpio_num_0 == gpio_num_1);
    CHECK(gpio_num_2 != gpio_num_1);
}

TEST_CASE("drive strength out of range")
{
    CHECK_THROWS_AS(GPIODriveStrength(-1), GPIOException&);
    CHECK_THROWS_AS(GPIODriveStrength(static_cast<uint32_t>(GPIO_DRIVE_CAP_MAX)), GPIOException&);
}

TEST_CASE("drive strength as expected")
{
    CHECK(GPIODriveStrength::DEFAULT().get_strength() == GPIO_DRIVE_CAP_2);
    CHECK(GPIODriveStrength::WEAK().get_strength() == GPIO_DRIVE_CAP_0);
    CHECK(GPIODriveStrength::LESS_WEAK().get_strength() == GPIO_DRIVE_CAP_1);
    CHECK(GPIODriveStrength::MEDIUM().get_strength() == GPIO_DRIVE_CAP_2);
    CHECK(GPIODriveStrength::STRONGEST().get_strength() == GPIO_DRIVE_CAP_3);
}

TEST_CASE("pull mode create functions work as expected")
{
    CHECK(GPIOPullMode::FLOATING().get_pull_mode() == 3);
    CHECK(GPIOPullMode::PULLUP().get_pull_mode() == 0);
    CHECK(GPIOPullMode::PULLDOWN().get_pull_mode() == 1);
}

TEST_CASE("GPIOIntrType create functions work as expected")
{
    CHECK(GPIOWakeupIntrType::LOW_LEVEL().get_level() == GPIO_INTR_LOW_LEVEL);
    CHECK(GPIOWakeupIntrType::HIGH_LEVEL().get_level() == GPIO_INTR_HIGH_LEVEL);
}

TEST_CASE("output resetting pin fails")
{
    CMOCK_SETUP();
    gpio_reset_pin_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(GPIO_Output gpio(VALID_GPIO), GPIOException&);

    Mockgpio_Verify();
}

TEST_CASE("output setting direction fails")
{
    CMOCK_SETUP();
    gpio_reset_pin_ExpectAnyArgsAndReturn(ESP_OK);
    gpio_set_direction_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(GPIO_Output gpio(VALID_GPIO), GPIOException&);

    Mockgpio_Verify();
}

TEST_CASE("output constructor sets correct arguments")
{
    CMOCK_SETUP();
    gpio_reset_pin_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()), ESP_OK);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()), GPIO_MODE_OUTPUT, ESP_OK);

    GPIO_Output gpio(VALID_GPIO);

    Mockgpio_Verify();
}

TEST_CASE("output set high fails")
{
    GPIOFixture fix;
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 1, ESP_FAIL);

    GPIO_Output gpio(fix.num);

    CHECK_THROWS_AS(gpio.set_high(), GPIOException&);
}

TEST_CASE("output set high success")
{
    GPIOFixture fix;
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 1, ESP_OK);

    GPIO_Output gpio(fix.num);

    gpio.set_high();
}

TEST_CASE("output set low fails")
{
    GPIOFixture fix;
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 0, ESP_FAIL);

    GPIO_Output gpio(fix.num);

    CHECK_THROWS_AS(gpio.set_low(), GPIOException&);
}

TEST_CASE("output set low success")
{
    GPIOFixture fix;
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 0, ESP_OK);

    GPIO_Output gpio(fix.num);

    gpio.set_low();
}

TEST_CASE("output set drive strength")
{
    GPIOFixture fix(VALID_GPIO);
    gpio_set_drive_capability_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_DRIVE_CAP_0, ESP_OK);

    GPIO_Output gpio(fix.num);

    gpio.set_drive_strength(GPIODriveStrength::WEAK());
}

TEST_CASE("output get drive strength")
{
    GPIOFixture fix(VALID_GPIO);
    gpio_drive_cap_t drive_strength = GPIO_DRIVE_CAP_3;
    gpio_get_drive_capability_ExpectAnyArgsAndReturn(ESP_OK);
    gpio_get_drive_capability_ReturnThruPtr_strength(&drive_strength);

    GPIO_Output gpio(fix.num);

    CHECK(gpio.get_drive_strength() == GPIODriveStrength::STRONGEST());
}

TEST_CASE("GPIOInput setting direction fails")
{
    CMOCK_SETUP();
    gpio_reset_pin_ExpectAnyArgsAndReturn(ESP_OK);
    gpio_set_direction_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(GPIOInput gpio(VALID_GPIO), GPIOException&);

    Mockgpio_Verify();
}

TEST_CASE("constructor sets correct arguments")
{
    CMOCK_SETUP();
    gpio_reset_pin_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()), ESP_OK);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()), GPIO_MODE_INPUT, ESP_OK);

    GPIOInput gpio(VALID_GPIO);

    Mockgpio_Verify();
}

TEST_CASE("get level low")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_get_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 0);

    GPIOInput gpio(fix.num);

    CHECK(gpio.get_level() == GPIOLevel::LOW);
}

TEST_CASE("get level high")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_get_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 1);

    GPIOInput gpio(fix.num);

    CHECK(gpio.get_level() == GPIOLevel::HIGH);
}

TEST_CASE("set pull mode fails")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_FLOATING, ESP_FAIL);

    GPIOInput gpio(fix.num);

    CHECK_THROWS_AS(gpio.set_pull_mode(GPIOPullMode::FLOATING()), GPIOException&);
}

TEST_CASE("GPIOInput set pull mode floating")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_FLOATING, ESP_OK);

    GPIOInput gpio(fix.num);

    gpio.set_pull_mode(GPIOPullMode::FLOATING());
}

TEST_CASE("GPIOInput set pull mode pullup")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_PULLUP_ONLY, ESP_OK);

    GPIOInput gpio(fix.num);

    gpio.set_pull_mode(GPIOPullMode::PULLUP());
}

TEST_CASE("GPIOInput set pull mode pulldown")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_PULLDOWN_ONLY, ESP_OK);

    GPIOInput gpio(fix.num);

    gpio.set_pull_mode(GPIOPullMode::PULLDOWN());
}

TEST_CASE("GPIOInput wake up enable fails")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_wakeup_enable_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_INTR_LOW_LEVEL, ESP_FAIL);

    GPIOInput gpio(fix.num);

    CHECK_THROWS_AS(gpio.wakeup_enable(GPIOWakeupIntrType::LOW_LEVEL()), GPIOException&);
}

TEST_CASE("GPIOInput wake up enable high int")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_wakeup_enable_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_INTR_HIGH_LEVEL, ESP_OK);

    GPIOInput gpio(fix.num);

    gpio.wakeup_enable(GPIOWakeupIntrType::HIGH_LEVEL());
}

TEST_CASE("GPIOInput wake up disable fails")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_wakeup_disable_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), ESP_FAIL);

    GPIOInput gpio(fix.num);

    CHECK_THROWS_AS(gpio.wakeup_disable(), GPIOException&);
}

TEST_CASE("GPIOInput wake up disable high int")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_wakeup_disable_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), ESP_OK);

    GPIOInput gpio(fix.num);

    gpio.wakeup_disable();
}

TEST_CASE("GPIO_OpenDrain setting direction fails")
{
    CMOCK_SETUP();
    gpio_reset_pin_ExpectAnyArgsAndReturn(ESP_OK);
    gpio_set_direction_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(GPIO_OpenDrain gpio(VALID_GPIO), GPIOException&);

    Mockgpio_Verify();
}

TEST_CASE("GPIO_OpenDrain constructor sets correct arguments")
{
    CMOCK_SETUP();
    gpio_reset_pin_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()), ESP_OK);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT,
            ESP_OK);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT_OUTPUT_OD,
            ESP_OK);

    GPIO_OpenDrain gpio(VALID_GPIO);

    Mockgpio_Verify();
}

TEST_CASE("GPIO_OpenDrain set floating fails")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT_OUTPUT_OD,
            ESP_OK);
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 1, ESP_FAIL);

    GPIO_OpenDrain gpio(fix.num);

    CHECK_THROWS_AS(gpio.set_floating(), GPIOException&);
}

TEST_CASE("GPIO_OpenDrain set floating success")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT_OUTPUT_OD,
            ESP_OK);
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 1, ESP_OK);

    GPIO_OpenDrain gpio(fix.num);

    gpio.set_floating();
}

TEST_CASE("GPIO_OpenDrain set low fails")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT_OUTPUT_OD,
            ESP_OK);
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 0, ESP_FAIL);

    GPIO_OpenDrain gpio(fix.num);

    CHECK_THROWS_AS(gpio.set_low(), GPIOException&);
}

TEST_CASE("GPIO_OpenDrain set low success")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT_OUTPUT_OD,
            ESP_OK);
    gpio_set_level_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), 0, ESP_OK);

    GPIO_OpenDrain gpio(fix.num);

    gpio.set_low();
}

TEST_CASE("GPIO_OpenDrain set drive strength")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT_OUTPUT_OD,
            ESP_OK);

    gpio_set_drive_capability_ExpectAndReturn(static_cast<gpio_num_t>(fix.num.get_num()), GPIO_DRIVE_CAP_0, ESP_OK);
    GPIO_OpenDrain gpio(fix.num);

    gpio.set_drive_strength(GPIODriveStrength::WEAK());
}

TEST_CASE("GPIO_OpenDrain get drive strength")
{
    GPIOFixture fix(VALID_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(VALID_GPIO.get_num()),
            GPIO_MODE_INPUT_OUTPUT_OD,
            ESP_OK);
    gpio_drive_cap_t drive_strength = GPIO_DRIVE_CAP_3;
    gpio_get_drive_capability_ExpectAnyArgsAndReturn(ESP_OK);
    gpio_get_drive_capability_ReturnThruPtr_strength(&drive_strength);

    GPIO_OpenDrain gpio(fix.num);

    CHECK(gpio.get_drive_strength() == GPIODriveStrength::STRONGEST());
}
