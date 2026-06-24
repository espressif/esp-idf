/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <cstdio>
#include <regex>
#include <cstring>
#include "esp_rom_sys.h"
#include "esp_err.h"

#include <catch2/catch_test_macros.hpp>

using namespace std;

#define BUFFER_SIZE (128)
static char s_print_buffer[BUFFER_SIZE];
static unsigned s_counter = 0;

static void reset_buffer(void)
{
    s_counter = 0;
    memset(s_print_buffer, 0, BUFFER_SIZE);
}

void putc_to_buffer(char c)
{
    s_print_buffer[s_counter++] = c;
}

static esp_err_t test_printf(const char *format, ...)
{
    esp_err_t error = ESP_OK;
    va_list list1, list2;
    char expected_str[BUFFER_SIZE];
    char *tested_str = s_print_buffer;

    va_start(list1, format);
    va_copy(list2, list1);
    reset_buffer();
    int len1 = esp_rom_vprintf(format, list1);
    int len2 = vsnprintf(expected_str, BUFFER_SIZE, format, list2);
    va_end(list2);
    va_end(list1);

    if (len1 != len2) {
        error = ESP_FAIL;
    }
    if (strcmp(tested_str, expected_str) != 0) {
        error = ESP_FAIL;
    }
    if (error) {
        printf("BAD: ");
    } else {
        printf("OK : ");
    }
    printf("'%s' -> %s", format, tested_str);
    if (error) {
        printf("  !=  ");
    } else {
        printf("  ==  ");
    }
    printf("%s\n", expected_str);

    return error;
}

TEST_CASE("Test precisions attrs of esp_rom_vprintf")
{
    esp_rom_install_channel_putc(1, putc_to_buffer);
    esp_err_t error = ESP_OK;
    error |= test_printf("[%d|%3d|%6d|%03d|%06d|%0d]", 1, 2, 3, 4, 5, 6);
    error |= test_printf("[%d|%3d|%6d|%03d|%06d|%0d]", -1, -2, -3, -4, -5, -6);
    error |= test_printf("[%d|%3d|%6d|%03d|%06d|%0d]", 10, 11, 12, 13, 14, 15);
    error |= test_printf("[%d|%3d|%6d|%03d|%06d|%0d]", -10, -11, -12, -13, -14, -15);
    error |= test_printf("[%d|%3d|%6d|%03d|%06d|%0d]", 12345, 12346, 12347, 12348, 12349, 12350);
    error |= test_printf("[%d|%3d|%6d|%03d|%06d|%0d]", -12345, -12346, -12347, -12348, -12349, -12350);

    error |= test_printf("[%.3d|%.1d|%.0d|%0.0d|%0.3d|%0.1d|%03.3d]", 1, 2, 3, 4, 5, 6, 7);
    error |= test_printf("[%.3d|%.1d|%.0d|%0.0d|%0.3d|%0.1d|%03.3d]", -1, -2, -3, -4, -5, -6, -7);
    error |= test_printf("[%.3d|%.1d|%.0d|%0.0d|%0.3d|%0.1d|%03.3d]", 10, 11, 12, 13, 14, 15, 16);
    error |= test_printf("[%.3d|%.1d|%.0d|%0.0d|%0.3d|%0.1d|%03.3d]", -10, -11, -12, -13, -14, -15, -16);
    error |= test_printf("[%.3d|%.1d|%.0d|%0.0d|%0.3d|%0.1d|%03.3d]", 12345, 12346, 12347, 12348, 12349, 12350, 123451);
    error |= test_printf("[%.3d|%.1d|%.0d|%0.0d|%0.3d|%0.1d|%03.3d]", -12345, -12346, -12347, -12348, -12349, -12350, 123451);

    error |= test_printf("[%6.4d|%06.4d|%04.6d|%4.6d|%8.4d|%08.4d]", 1, 2, 3, 4, 5, 6);
    error |= test_printf("[%6.4d|%06.4d|%04.6d|%4.6d|%8.4d|%08.4d]", -1, -2, -3, -4, -5, -6);
    error |= test_printf("[%6.4d|%06.4d|%04.6d|%4.6d|%8.4d|%08.4d]", 10, 11, 12, 13, 14, 15);
    error |= test_printf("[%6.4d|%06.4d|%04.6d|%4.6d|%8.4d|%08.4d]", -10, -11, -12, -13, -14, -15);
    error |= test_printf("[%6.4d|%06.4d|%04.6d|%4.6d|%8.4d|%08.4d]", 12340, 12341, 12342, 12343, 12344, 12345);
    error |= test_printf("[%6.4d|%06.4d|%04.6d|%4.6d|%8.4d|%08.4d]", -12340, -12341, -12342, -12343, -12344, -12345);

    error |= test_printf("[%-20s]", "1TEST_STR");
    error |= test_printf("[%-20.5s]", "8TEST_STR");
    error |= test_printf("[%20s]", "2TEST_STR");
    error |= test_printf("[%3s]", "3TEST_STR");
    error |= test_printf("[%.3s]", "4TEST_STR");
    error |= test_printf("[%.20s]", "5TEST_STR");
    error |= test_printf("[%6.3s]", "6TEST_STR");
    error |= test_printf("[%06.3s]", "7TEST_STR");
    esp_rom_install_uart_printf();
    CHECK(error == ESP_OK);
}
