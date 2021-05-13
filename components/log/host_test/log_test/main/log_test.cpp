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

struct PrintFixture {
    static const size_t BUFFER_SIZE = 4096;

    PrintFixture(esp_log_level_t log_level = ESP_LOG_VERBOSE)
    {
        if (instance != nullptr) {
            throw exception();
        }

        std::memset(print_buffer, 0, BUFFER_SIZE);

        instance = this;

        old_vprintf = esp_log_set_vprintf(print_callback);

        esp_log_level_set(TEST_TAG, log_level);
    }

    ~PrintFixture()
    {
        esp_log_level_set(TEST_TAG, ESP_LOG_INFO);
        esp_log_set_vprintf(old_vprintf);
        instance = nullptr;
    }

    string get_print_buffer_string() const
    {
        return string(print_buffer);
    }

    void reset_buffer()
    {
        std::memset(print_buffer, 0, BUFFER_SIZE);
    }

    char print_buffer [BUFFER_SIZE];

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

PrintFixture *PrintFixture::instance = nullptr;

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
