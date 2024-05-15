/* LOG unit tests

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#define CATCH_CONFIG_MAIN
#include <cstdio>
#include <regex>
#include <iostream>
#include "esp_log.h"

#include "catch.hpp"

using namespace std;

static const char *TEST_TAG = "test";

class BasicLogFixture {
public:
    static const size_t BUFFER_SIZE = 4096;

    BasicLogFixture(esp_log_level_t log_level = ESP_LOG_VERBOSE)
    {
        std::memset(print_buffer, 0, BUFFER_SIZE);
        esp_log_level_set("*", log_level);
    }

    virtual ~BasicLogFixture()
    {
        esp_log_level_set("*", ESP_LOG_INFO);
    }

    string get_print_buffer_string() const
    {
        return string(print_buffer);
    }

    void reset_buffer()
    {
        std::memset(print_buffer, 0, BUFFER_SIZE);
        additional_reset();
    }

protected:
    char print_buffer [BUFFER_SIZE];

    virtual void additional_reset() { }
};

struct PrintFixture : BasicLogFixture {
    PrintFixture(esp_log_level_t log_level = ESP_LOG_VERBOSE) : BasicLogFixture(log_level)
    {
        if (instance != nullptr) {
            throw exception();
        }

        instance = this;

        old_vprintf = esp_log_set_vprintf(print_callback);
    }

    virtual ~PrintFixture()
    {
        esp_log_set_vprintf(old_vprintf);
        instance = nullptr;
    }

private:
    static int print_callback(const char *format, va_list args)
    {
        return instance->print_to_buffer(format, args);
    }

    int print_to_buffer(const char *format, va_list args)
    {
        int ret = vsnprintf(print_buffer, BUFFER_SIZE, format, args);
        return ret;
    }

    static PrintFixture *instance;

    vprintf_like_t old_vprintf;
};

struct PutcFixture : BasicLogFixture {
    PutcFixture(esp_log_level_t log_level = ESP_LOG_VERBOSE) : BasicLogFixture(log_level), counter(0)
    {
        if (instance != nullptr) {
            throw exception();
        }

        esp_rom_install_channel_putc(0, putc_callback);

        instance = this;
    }

    ~PutcFixture()
    {
        esp_rom_install_uart_printf();
        instance = nullptr;
    }

    void additional_reset() override
    {
        counter = 0;
    }

    size_t counter;

private:
    static void putc_callback(char c)
    {
        return instance->putc_to_buffer(c);
    }

    void putc_to_buffer(char c)
    {
        print_buffer[counter++] = c;
    }

    static PutcFixture *instance;
};

PrintFixture *PrintFixture::instance = nullptr;
PutcFixture *PutcFixture::instance = nullptr;

TEST_CASE("verbose log level")
{
    PrintFixture fix(ESP_LOG_VERBOSE);
    const std::regex test_print("V \\([0-9]*\\) test: verbose", std::regex::ECMAScript);

    ESP_LOGV(TEST_TAG, "verbose");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("debug log level")
{
    PrintFixture fix(ESP_LOG_DEBUG);
    const std::regex test_print("D \\([0-9]*\\) test: debug", std::regex::ECMAScript);

    ESP_LOGD(TEST_TAG, "debug");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("info log level")
{
    PrintFixture fix(ESP_LOG_INFO);
    const std::regex test_print("I \\([0-9]*\\) test: info", std::regex::ECMAScript);

    ESP_LOGI(TEST_TAG, "info");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("warn log level")
{
    PrintFixture fix(ESP_LOG_WARN);
    const std::regex test_print("W \\([0-9]*\\) test: warn", std::regex::ECMAScript);

    ESP_LOGW(TEST_TAG, "warn");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("error log level")
{
    PrintFixture fix(ESP_LOG_ERROR);
    const std::regex test_print("E \\([0-9]*\\) test: error", std::regex::ECMAScript);

    ESP_LOGE(TEST_TAG, "error");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("changing log level")
{
    PrintFixture fix(ESP_LOG_INFO);
    const std::regex test_print("I \\([0-9]*\\) test: must indeed be printed", std::regex::ECMAScript);

    ESP_LOGI(TEST_TAG, "must indeed be printed");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);

    fix.reset_buffer();
    esp_log_level_set(TEST_TAG, ESP_LOG_WARN);

    ESP_LOGI(TEST_TAG, "must not be printed");
    CHECK(fix.get_print_buffer_string().size() == 0);

    fix.reset_buffer();
    esp_log_level_set(TEST_TAG, ESP_LOG_INFO);

    ESP_LOGI(TEST_TAG, "must indeed be printed");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("log buffer")
{
    PrintFixture fix(ESP_LOG_INFO);
    const uint8_t buffer[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    };
    ESP_LOG_BUFFER_HEX(TEST_TAG, buffer, sizeof(buffer));
    const std::regex buffer_regex("I \\([0-9]*\\) test: 01 02 03 04 05 06 07 08 11 12 13 14 15 16 17 18", std::regex::ECMAScript);
    CHECK(regex_search(fix.get_print_buffer_string(), buffer_regex));
}

TEST_CASE("log bytes > 127")
{
    PrintFixture fix(ESP_LOG_INFO);
    const uint8_t buffer[] = {
        0xff, 0x80,
    };
    ESP_LOG_BUFFER_HEX(TEST_TAG, buffer, sizeof(buffer));
    const std::regex buffer_regex("I \\([0-9]*\\) test: ff 80", std::regex::ECMAScript);
    CHECK(regex_search(fix.get_print_buffer_string(), buffer_regex));
}

TEST_CASE("log buffer dump")
{
    PrintFixture fix(ESP_LOG_INFO);
    const uint8_t buffer[] = {
        0x00, 0x00, 0x00, 0x00, 0x05, 0x06, 0x07, 0x08,
        0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8
    };
    ESP_LOG_BUFFER_HEXDUMP(TEST_TAG, buffer, sizeof(buffer), ESP_LOG_INFO);
    const std::regex buffer_regex("I \\([0-9]*\\) test: 0x[0-9a-f]+\\s+"
                                  "00 00 00 00 05 06 07 08  ff fe fd fc fb fa f9 f8 "
                                  "\\s+|[\\.]{16}|", std::regex::ECMAScript);
    CHECK(regex_search(fix.get_print_buffer_string(), buffer_regex));
}

TEST_CASE("rom printf")
{
    PutcFixture fix;
    int printed_chars = esp_rom_printf("info");

    CHECK(printed_chars == 4);
    CHECK(fix.get_print_buffer_string() == "info");
}

TEST_CASE("early verbose log level")
{
    PutcFixture fix;
    const std::regex test_print("V \\([0-9]*\\) test: verbose", std::regex::ECMAScript);

    ESP_EARLY_LOGV(TEST_TAG, "verbose");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early debug log level")
{
    PutcFixture fix;
    const std::regex test_print("D \\([0-9]*\\) test: debug", std::regex::ECMAScript);

    ESP_EARLY_LOGD(TEST_TAG, "debug");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early info log level")
{
    PutcFixture fix;
    const std::regex test_print("I \\([0-9]*\\) test: info", std::regex::ECMAScript);

    ESP_EARLY_LOGI(TEST_TAG, "info");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early warn log level")
{
    PutcFixture fix;
    const std::regex test_print("W \\([0-9]*\\) test: warn", std::regex::ECMAScript);

    ESP_EARLY_LOGW(TEST_TAG, "warn");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early error log level")
{
    PutcFixture fix;
    const std::regex test_print("E \\([0-9]*\\) test: error", std::regex::ECMAScript);

    ESP_EARLY_LOGE(TEST_TAG, "error");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("changing early log level")
{
    PutcFixture fix(ESP_LOG_INFO);
    const std::regex test_print("I \\([0-9]*\\) test: must indeed be printed", std::regex::ECMAScript);

    ESP_EARLY_LOGI(TEST_TAG, "must indeed be printed");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);

    fix.reset_buffer();
    esp_log_level_set("*", ESP_LOG_WARN);

    ESP_EARLY_LOGI(TEST_TAG, "must not be printed");
    CHECK(fix.get_print_buffer_string().size() == 0);

    fix.reset_buffer();
    esp_log_level_set("*", ESP_LOG_INFO);

    ESP_EARLY_LOGI(TEST_TAG, "must indeed be printed");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}
