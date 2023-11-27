/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* LOG unit tests

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.

    This is a very basic test to see whether the supplied linux functions appear to work correctly.

    Note that the printf function is roughly tested in the log host test.
*/
#include <cstdio>
#include <regex>
#include <cstring>
#include "esp_rom_sys.h"
#include "esp_rom_efuse.h"
#include "esp_rom_crc.h"
#include "esp_rom_md5.h"

#include <catch2/catch_test_macros.hpp>

using namespace std;

// ESP_LOG_EARLY functions are tested in the log host tests and also test rom printf.
TEST_CASE("esp_rom printf returns correct char num")
{
    CHECK(esp_rom_printf("test\n") == 5);
}

TEST_CASE("delay works")
{
    const uint64_t DELAY = 100u;
    struct timespec current_time;
    CHECK(clock_gettime(CLOCK_MONOTONIC, &current_time) == 0);
    uint64_t start_us = current_time.tv_sec * 1000000 + current_time.tv_nsec / 1000;

    esp_rom_delay_us(DELAY);

    CHECK(clock_gettime(CLOCK_MONOTONIC, &current_time) == 0);
    uint64_t end_us = current_time.tv_sec * 1000000 + current_time.tv_nsec / 1000;

    CHECK(start_us + DELAY <= end_us);
}

TEST_CASE("crc quick check")
{
    uint32_t expected_result = 0xd4dc5010;
    uint32_t result;
    const uint8_t original [] = {0x01, 0x21, 0x09, 0xff, 0x63, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74,
                                 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe5, 0x00, 0xff, 0xff, 0x06, 0xba, 0xe7, 0xa1};

    result = esp_rom_crc32_le(0xffffffff, original, sizeof(original));

    CHECK(result == expected_result);
}

TEST_CASE("reset reason basic check")
{
    CHECK(esp_rom_get_reset_reason(0) == RESET_REASON_CHIP_POWER_ON);
}

TEST_CASE("flash gpio info")
{
    CHECK(esp_rom_efuse_get_flash_gpio_info() == 0);
}

TEST_CASE("get flash wp gpio")
{
    CHECK(esp_rom_efuse_get_flash_wp_gpio() == 0);
}

TEST_CASE("secure boot always disabled on Linux")
{
    CHECK(esp_rom_efuse_is_secure_boot_enabled() == false);
}

TEST_CASE("md5")
{
    md5_context_t context;
    const uint8_t expected_result [16] = {0x00, 0x2e, 0x17, 0x69, 0x17, 0x24, 0x32, 0x78, 0x72, 0xf1, 0xaf, 0x42, 0x9f, 0x1c, 0xe4, 0xd9};
    uint8_t result [16] = {};
    const std::string MD5_TEST_STRING("This is an MD5 test");

    esp_rom_md5_init(&context);
    esp_rom_md5_update(&context, MD5_TEST_STRING.c_str(), MD5_TEST_STRING.size());
    esp_rom_md5_final(result, &context);

    for (int i = 0; i < 16; i++) {
        CHECK(result[i] == expected_result[i]);
    }
}
