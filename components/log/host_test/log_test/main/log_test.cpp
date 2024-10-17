/* LOG unit tests

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <cstdio>
#include <regex>
#include <iostream>
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "esp_private/log_util.h"
#include "esp_private/log_timestamp.h"
#include "sdkconfig.h"

#include <catch2/catch_test_macros.hpp>

using namespace std;

#define EARLY_TIMESTAMP "[0-9]*"

#if CONFIG_LOG_TIMESTAMP_SOURCE_RTOS
#define TIMESTAMP "[0-9]*"
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM
#define TIMESTAMP "[0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{3}"
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM_FULL
#define TIMESTAMP "[0-9]{2}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{3}"
#else
#define TIMESTAMP ""
#endif

static const char *TEST_TAG = "test";

class BasicLogFixture {
public:
    static const size_t BUFFER_SIZE = 4096;

    BasicLogFixture(esp_log_level_t log_level = ESP_LOG_VERBOSE)
    {
        std::memset(print_buffer, 0, BUFFER_SIZE);
        buffer_idx = 0;
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
        buffer_idx = 0;
        additional_reset();
    }

protected:
    char print_buffer [BUFFER_SIZE];
    int buffer_idx;

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
        // Added support for multi-line log, for example ESP_LOG_BUFFER...
        int ret = vsnprintf(&print_buffer[buffer_idx], BUFFER_SIZE, format, args);
        buffer_idx += ret;
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

        esp_rom_install_channel_putc(1, putc_callback);

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
    const std::regex test_print("V \\(" TIMESTAMP "\\) test: verbose", std::regex::ECMAScript);

    ESP_LOGV(TEST_TAG, "verbose");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("debug log level")
{
    PrintFixture fix(ESP_LOG_DEBUG);
    const std::regex test_print("D \\(" TIMESTAMP "\\) test: debug", std::regex::ECMAScript);

    ESP_LOGD(TEST_TAG, "debug");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("info log level")
{
    PrintFixture fix(ESP_LOG_INFO);
    const std::regex test_print("I \\(" TIMESTAMP "\\) test: info", std::regex::ECMAScript);

    ESP_LOGI(TEST_TAG, "info");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("warn log level")
{
    PrintFixture fix(ESP_LOG_WARN);
    const std::regex test_print("W \\(" TIMESTAMP "\\) test: warn", std::regex::ECMAScript);

    ESP_LOGW(TEST_TAG, "warn");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("error log level")
{
    PrintFixture fix(ESP_LOG_ERROR);
    const std::regex test_print("E \\(" TIMESTAMP "\\) test: error", std::regex::ECMAScript);

    ESP_LOGE(TEST_TAG, "error");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

#if CONFIG_LOG_DYNAMIC_LEVEL_CONTROL
TEST_CASE("changing log level")
{
    PrintFixture fix(ESP_LOG_INFO);
    const std::regex test_print("I \\(" TIMESTAMP "\\) test: must indeed be printed", std::regex::ECMAScript);

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
#endif // CONFIG_LOG_DYNAMIC_LEVEL_CONTROL

TEST_CASE("log buffer")
{
    PrintFixture fix(ESP_LOG_INFO);
    const uint8_t buffer[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    };
    ESP_LOG_BUFFER_HEX(TEST_TAG, buffer, sizeof(buffer));
    const std::regex buffer_regex("I \\(" TIMESTAMP "\\) test: 01 02 03 04 05 06 07 08 11 12 13 14 15 16 17 18", std::regex::ECMAScript);
    CHECK(regex_search(fix.get_print_buffer_string(), buffer_regex));
}

TEST_CASE("log bytes > 127")
{
    PrintFixture fix(ESP_LOG_INFO);
    const uint8_t buffer[] = {
        0xff, 0x80,
    };
    ESP_LOG_BUFFER_HEX(TEST_TAG, buffer, sizeof(buffer));
    const std::regex buffer_regex("I \\(" TIMESTAMP "\\) test: ff 80", std::regex::ECMAScript);
    CHECK(regex_search(fix.get_print_buffer_string(), buffer_regex));
}

TEST_CASE("log buffer char")
{
    PrintFixture fix(ESP_LOG_INFO);
    const char g[] = "The way to get started is to quit talking and begin doing. - Walt Disney";
    const std::regex buffer_regex("I \\(" TIMESTAMP "\\) test: The way to get s.*\n\
.*I \\(" TIMESTAMP "\\) test: tarted is to qui.*\n\
.*I \\(" TIMESTAMP "\\) test: t talking and be.*\n\
.*I \\(" TIMESTAMP "\\) test: gin doing. - Wal.*\n\
.*I \\(" TIMESTAMP "\\) test: t Disney", std::regex::ECMAScript);
    ESP_LOG_BUFFER_CHAR(TEST_TAG, g, sizeof(g));
    CHECK(regex_search(fix.get_print_buffer_string(), buffer_regex) == true);
}

TEST_CASE("log buffer dump")
{
    PrintFixture fix(ESP_LOG_INFO);
    const uint8_t buffer[] = {
        0x00, 0x00, 0x00, 0x00, 0x05, 0x06, 0x07, 0x08,
        0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8
    };
    ESP_LOG_BUFFER_HEXDUMP(TEST_TAG, buffer, sizeof(buffer), ESP_LOG_INFO);
    const std::regex buffer_regex("I \\(" TIMESTAMP "\\) test: 0x[0-9a-f]+\\s+"
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
    const std::regex test_print("V \\(" EARLY_TIMESTAMP "\\) test: verbose", std::regex::ECMAScript);

    ESP_EARLY_LOGV(TEST_TAG, "verbose");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early debug log level")
{
    PutcFixture fix;
    const std::regex test_print("D \\(" EARLY_TIMESTAMP "\\) test: debug", std::regex::ECMAScript);

    ESP_EARLY_LOGD(TEST_TAG, "debug");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early info log level")
{
    PutcFixture fix;
    const std::regex test_print("I \\(" EARLY_TIMESTAMP "\\) test: info", std::regex::ECMAScript);

    ESP_EARLY_LOGI(TEST_TAG, "info");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early warn log level")
{
    PutcFixture fix;
    const std::regex test_print("W \\(" EARLY_TIMESTAMP "\\) test: warn", std::regex::ECMAScript);

    ESP_EARLY_LOGW(TEST_TAG, "warn");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

TEST_CASE("early error log level")
{
    PutcFixture fix;
    const std::regex test_print("E \\(" EARLY_TIMESTAMP "\\) test: error", std::regex::ECMAScript);

    ESP_EARLY_LOGE(TEST_TAG, "error");
    CHECK(regex_search(fix.get_print_buffer_string(), test_print) == true);
}

#if CONFIG_LOG_DYNAMIC_LEVEL_CONTROL
TEST_CASE("changing early log level")
{
    PutcFixture fix(ESP_LOG_INFO);
    const std::regex test_print("I \\(" EARLY_TIMESTAMP "\\) test: must indeed be printed", std::regex::ECMAScript);

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
#endif // CONFIG_LOG_DYNAMIC_LEVEL_CONTROL

TEST_CASE("esp_log_util_cvt")
{
    char buf[128];
    CHECK(esp_log_util_cvt_dec(123456, 0, buf) == 6);
    CHECK(strcmp(buf, "123456") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_dec(123456, 2, buf) == 6);
    CHECK(strcmp(buf, "123456") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_dec(123456, 8, buf) == 8);
    CHECK(strcmp(buf, "00123456") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_dec(1, 0, buf) == 1);
    CHECK(strcmp(buf, "1") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_dec(0, 0, buf) == 1);
    CHECK(strcmp(buf, "0") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_dec(0, 4, buf) == 4);
    CHECK(strcmp(buf, "0000") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_dec(1, 2, buf) == 2);
    CHECK(strcmp(buf, "01") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_hex(0x73f, -1, buf) == 3);
    CHECK(strcmp(buf, "73f") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_hex(0x73f, 2, buf) == 3);
    CHECK(strcmp(buf, "73f") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_hex(0x73f, 3, buf) == 3);
    CHECK(strcmp(buf, "73f") == 0);
    memset(buf, 0, sizeof(buf));

    CHECK(esp_log_util_cvt_hex(0x73f, 4, buf) == 4);
    CHECK(strcmp(buf, "073f") == 0);
    memset(buf, 0, sizeof(buf));
}

TEST_CASE("esp_log_timestamp_str")
{
    char buffer[64];
    bool critical = true;
    uint64_t timestamp_ms = esp_log_timestamp64(critical);
    esp_log_timestamp_str(critical, timestamp_ms, buffer);
    const std::regex test_print(EARLY_TIMESTAMP, std::regex::ECMAScript);
    CHECK(regex_search(string(buffer), test_print) == true);

    critical = false;
    timestamp_ms = esp_log_timestamp64(critical);
    esp_log_timestamp_str(critical, timestamp_ms, buffer);
    const std::regex test_print2(TIMESTAMP, std::regex::ECMAScript);
    CHECK(regex_search(string(buffer), test_print2) == true);
}
