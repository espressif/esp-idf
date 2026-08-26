/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"

#if CONFIG_EXAMPLE_USE_TEXT_LOG
#define ESP_LOG_MODE_BINARY_EN  (0)
#endif

#include "esp_log.h"

static const char *TAG = "example";

void app_main(void)
{
    ESP_LOGI(TAG, "Start");

    char char_var = 'A';
    ESP_LOGI(TAG, "%s %c 0x%hhx", "char_var", char_var, char_var);

    uint8_t var8 = 240;
    int8_t var8_2 = -120;
    ESP_LOGI(TAG, "%s %" PRIu8 " %" PRIi8, "var8", var8, var8_2);

    uint16_t var16 = 65303;
    int16_t var16_2 = -16954;
    ESP_LOGI(TAG, "%s %" PRIu16 " %" PRIi16 " 0x%hx", "var16", var16, var16_2, var16);

    ESP_LOGI(TAG, "%s %d", "const int var", -1024);
    ESP_LOGI(TAG, "%s %" PRIu64, ESP_LOG_ATTR_STR("const long long unsigned var"), 2095ULL);

    const int int_var = 4095;
    const int sint_var = -4095;
    ESP_LOGI(TAG, "%s %d %d", "int_var", int_var, sint_var);

    const uint32_t var32 = 9000;
    const int32_t svar32 = -2000;
    ESP_LOGI(TAG, "%s %" PRIu32 " %" PRIi32, "var32", var32, svar32);

    const uint64_t var64 = 10700;
    const int64_t svar64 = -29468;
    ESP_LOGI(TAG, "%s %" PRIu64 " %" PRIi64, "var64", var64, svar64);
    uint64_t var64_2 = 10800;
    ESP_LOGI(TAG, "%s %" PRIu64 " %" PRIu32 " %" PRIu64, "64 32 64 vars", var64, var32, var64_2);

    const size_t var_size = 96843;
    ESP_LOGI(TAG, "%s %zu %d", "var_size", var_size, sizeof(var_size));

    float float_var = 1.6f;
    double double_var = -70.2f;
    ESP_LOGI(TAG, "%s %f", "float var", float_var);
    ESP_LOGI(TAG, "%s %lf", "double var", double_var);

    ESP_LOGI(TAG, "%s %d %lf %" PRIu32 " %f %f %c", "int, double, var32, float, float, char", int_var, double_var, var32, float_var, float_var, char_var);

    char str_array[] = "str_array";
    ESP_LOGI(TAG, "%s %p", str_array, (void *)str_array);

    const char *str_ptr = "str_ptr";
    ESP_LOGI(TAG, "%s %p", str_ptr,  (void *)str_ptr);

    char buffer[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    };
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buffer, sizeof(buffer), ESP_LOG_INFO);
    ESP_LOGI(TAG, "buffer addr = %p", (void *)buffer);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, sizeof(buffer), ESP_LOG_INFO);

    char txt[] = "Text1:The way to get started is to quit talking and begin doing. - Walt Disney";
    ESP_LOG_BUFFER_CHAR_LEVEL(TAG, txt, sizeof(txt), ESP_LOG_INFO);

    static const char txt2[] = "Text2:The way to get started is to quit talking and begin doing. - Walt Disney";
    ESP_LOG_BUFFER_CHAR_LEVEL(TAG, txt2, sizeof(txt2), ESP_LOG_INFO);

    int buffer2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    ESP_LOGI(TAG, "buffer2 addr = %p", (void *)buffer2);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer2, sizeof(buffer2), ESP_LOG_INFO);

    uint64_t buffer3[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    ESP_LOGI(TAG, "buffer3 addr = %p", (void *)buffer3);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer3, sizeof(buffer3), ESP_LOG_INFO);

    ///////////////////String Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> String Formatting Tests <<<");
    ESP_LOGI(TAG, "|%s|", "Hello_world");
    ESP_LOGI(TAG, "|%10s|", "ESP32");
    ESP_LOGI(TAG, "|%-10s|", "ESP32");
    ESP_LOGI(TAG, "|%.5s|", "Hello_world");

    ///////////////////Character Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> Character Formatting Tests <<<");
    ESP_LOGI(TAG, "|%c|", 'A');

    ///////////////////Integer Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> Integer Formatting Tests <<<");
    ESP_LOGI(TAG, "|%d|", 123);
    ESP_LOGI(TAG, "|%5d|", 42);
    ESP_LOGI(TAG, "|%05d|", 42);
    ESP_LOGI(TAG, "|%-5d|", 42);
    ESP_LOGI(TAG, "|%.5d|", 42);
    ESP_LOGI(TAG, "|%+d|", 42);
    ESP_LOGI(TAG, "|% d|", 42);
    ESP_LOGI(TAG, "|%ld|", 123456789L);
    ESP_LOGI(TAG, "|%lld|", 1234567890123456789LL);
    ESP_LOGI(TAG, "|%+10d|", 42);
    ESP_LOGI(TAG, "|% 10d|", 42);
    ESP_LOGI(TAG, "|%-+10d|", 42);
    ESP_LOGI(TAG, "|%- 10d|", 42);

    ///////////////////Pointer Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> Pointer Formatting Tests <<<");
    ESP_LOGI(TAG, "|%p|", (void *)0x3ff26523);

    ///////////////////Hexadecimal Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> Hexadecimal Formatting Tests <<<");
    ESP_LOGI(TAG, "|%x|", 255);
    ESP_LOGI(TAG, "|%X|", 255);
    ESP_LOGI(TAG, "|%05x|", 255);
    ESP_LOGI(TAG, "|%.5x|", 255);
    ESP_LOGI(TAG, "|%-5x|", 255);
    ESP_LOGI(TAG, "|%hx|", (uint16_t)0xFFFF);
    ESP_LOGI(TAG, "|%hhx|", (uint8_t)0xFF);
    ESP_LOGI(TAG, "|%#x|", 42);
    ESP_LOGI(TAG, "|%#X|", 255);
    ESP_LOGI(TAG, "|%#10x|", 42);
    ESP_LOGI(TAG, "|%-#10x|", 42);

    ///////////////////Octal Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> Octal Formatting Tests <<<");
    ESP_LOGI(TAG, "|%o|", 8);
    ESP_LOGI(TAG, "|%#o|", 8);
    ESP_LOGI(TAG, "|%ho|", (uint16_t)511);
    ESP_LOGI(TAG, "|%#ho|", (uint16_t)511);
    ESP_LOGI(TAG, "|%#10o|", 42);
    ESP_LOGI(TAG, "|%-#10o|", 42);

    ///////////////////Float Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> Float Formatting Tests <<<");
    ESP_LOGI(TAG, "|%f|", 123.456);
    ESP_LOGI(TAG, "|%.2f|", 123.456);
    ESP_LOGI(TAG, "|%.2f|", -123.456);
    ESP_LOGI(TAG, "|%10.2f|", 3.14159);
    ESP_LOGI(TAG, "|%-10.2f|", 3.14159);
    ESP_LOGI(TAG, "|%10.6f|", -123.45678933);
    ESP_LOGI(TAG, "|%-10.6f|", -123.45678933);

    ///////////////////Scientific Float Formatting Tests///////////////////
    ESP_LOGI(TAG, ">>> Scientific Float Formatting Tests <<<");
    ESP_LOGI(TAG, "|%F|", 123456.789);
    ESP_LOGI(TAG, "|%e|", 123456.789);
    ESP_LOGI(TAG, "|%E|", 123456.789);
    ESP_LOGI(TAG, "|%g|", 123456.789);
    ESP_LOGI(TAG, "|%G|", 123456.789);

    ///////////////////Literal Percent Sign///////////////////
    ESP_LOGI(TAG, ">>> Literal Percent Sign Tests <<<");
    ESP_LOGI(TAG, "|%%|");
    ESP_LOGI(TAG, "|%%| |%s|", "Hello_world");
    ESP_LOGI(TAG, "} |%s|", "Hello_world");

    ///////////////////Multiple variables in one log///////////////////
    ESP_LOGI(TAG, ">>> Multiple Variables in One Log <<<");
    ESP_LOGI(TAG, "%s |%10s| |%-10s| |%.5s|", "string width", "ESP32", "ESP32", "ESP32 Test");
    ESP_LOGI(TAG, "%s |%hx| |%hhx|", "shorts", (uint16_t)65535, (uint8_t)255);
    ESP_LOGI(TAG, "%s |% d| |% d|", "  flag", 42, -42);
    ESP_LOGI(TAG, "%s |%+d| |%+d|", "+ flag", 42, -42);
    ESP_LOGI(TAG, "%s |%#lx| |%#lo|", "# flag long", 1234567890L, 1234567890L);
    ESP_LOGI(TAG, "%s |%ld| |%lld|", "longs", 1234567890L, 123456789012345LL);
    ESP_LOGI(TAG, "%s |%10.2f| |%-10.2f| |%.4f|", "float width", 3.14159, 3.14159, 3.14159);
    ESP_LOGI(TAG, "%s |%10.6lf| |%-10.6lf| |%.2lf|", "double width", -123.456789, -123.456789, -123.456789);

    ESP_LOGI(TAG, "Success");
}
