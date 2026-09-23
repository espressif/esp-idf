/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <future>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include "esp_log.h"
#include <catch2/catch_test_macros.hpp>

using namespace std::chrono_literals;

namespace {
struct HandlerGuard {
    esp_log_handler_t previous = {};
    HandlerGuard()
    {
        esp_log_set_handler(nullptr, nullptr, &previous);
    }
    ~HandlerGuard()
    {
        esp_log_set_handler(previous.fn, previous.ctx, nullptr);
    }
};

struct Seen {
    unsigned calls = 0;
    std::string tag;
    std::string format;
    std::string text;
    bool timestamp_valid = false;
    bool binary = false;
    bool arg_types = false;
    esp_log_level_t level = ESP_LOG_NONE;
};

bool observe(esp_log_msg_t *message, void *ctx)
{
    auto &seen = *static_cast<Seen *>(ctx);
    ++seen.calls;
    seen.tag = message->tag ? message->tag : "";
    seen.level = message->config.opts.log_level;
    seen.timestamp_valid = message->timestamp_valid;
    seen.binary = message->config.opts.binary_mode;
    seen.arg_types = message->arg_types != nullptr;
    if (!seen.binary) {
        seen.format = message->format;
        char buffer[256];
        va_list copy;
        va_copy(copy, message->args);
        vsnprintf(buffer, sizeof(buffer), message->format, copy);
        va_end(copy);
        seen.text = buffer;
    }
    return false;
}

std::string output;
std::mutex output_lock;
int capture(const char *format, va_list args)
{
    char buffer[512];
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    std::lock_guard<std::mutex> guard(output_lock);
    if (length > 0 && length < static_cast<int>(sizeof(buffer))) {
        output.append(buffer, length);
    }
    return length;
}

struct OutputGuard {
    vprintf_like_t previous;
    OutputGuard() : previous(esp_log_set_vprintf(capture))
    {
        output.clear();
    }
    ~OutputGuard()
    {
        esp_log_set_vprintf(previous);
    }
};
} // namespace

TEST_CASE("Message handler receives context and version-specific metadata", "[handler]")
{
    Seen seen;
    HandlerGuard guard;
    REQUIRE(esp_log_set_handler(observe, &seen, nullptr) == ESP_OK);
    ESP_LOGI("handler", "number=%d", 42);
    CHECK(seen.calls == 1);
    CHECK(seen.tag == "handler");
    CHECK(seen.level == ESP_LOG_INFO);
#if ESP_LOG_VERSION == 1
    CHECK_FALSE(seen.timestamp_valid);
    CHECK(seen.format.find("handler") == std::string::npos); // Tag is an argument in V1.
    CHECK(seen.text.find("handler: number=42") != std::string::npos);
#else
    CHECK(seen.timestamp_valid == (!ESP_LOG_TIMESTAMP_DISABLED && ESP_LOG_SUPPORT_TIMESTAMP));
#if ESP_LOG_MODE_BINARY_EN
    CHECK(seen.binary);
    CHECK(seen.arg_types);
#else
    CHECK(seen.format == "number=%d");
    CHECK(seen.text == "number=42");
#endif
#endif
}

TEST_CASE("Replacing a handler returns the entire previous pair", "[handler]")
{
    Seen first, second;
    HandlerGuard guard;
    esp_log_handler_t previous = {};
    REQUIRE(esp_log_set_handler(observe, &first, nullptr) == ESP_OK);
    REQUIRE(esp_log_set_handler(observe, &second, &previous) == ESP_OK);
    CHECK(previous.fn == observe);
    CHECK(previous.ctx == &first);
    ESP_LOGI("handler", "second");
    CHECK(second.calls == 1);
    CHECK(first.calls == 0);
    REQUIRE(esp_log_set_handler(previous.fn, previous.ctx, nullptr) == ESP_OK);
    ESP_LOGI("handler", "first");
    CHECK(first.calls == 1);
    REQUIRE(esp_log_set_handler(nullptr, &second, &previous) == ESP_OK);
    CHECK(previous.ctx == &first);
    REQUIRE(esp_log_set_handler(nullptr, nullptr, &previous) == ESP_OK);
    CHECK(previous.fn == nullptr);
    CHECK(previous.ctx == nullptr);
}

TEST_CASE("Early and DRAM messages bypass the handler", "[handler]")
{
    Seen seen;
    HandlerGuard guard;
    REQUIRE(esp_log_set_handler(observe, &seen, nullptr) == ESP_OK);
    ESP_EARLY_LOGI("handler", "early");
    ESP_DRAM_LOGI("handler", "dram");
    CHECK(seen.calls == 0);
    ESP_LOGI("handler", "normal");
    CHECK(seen.calls == 1);
}

#if ESP_LOG_VERSION == 2
TEST_CASE("Constrained messages bypass the handler in V2", "[handler]")
{
    Seen seen;
    HandlerGuard guard;
    REQUIRE(esp_log_set_handler(observe, &seen, nullptr) == ESP_OK);
    esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_INFO | ESP_LOG_CONFIG_CONSTRAINED_ENV), "handler", "constrained");
    CHECK(seen.calls == 0);
    ESP_LOGI("handler", "normal");
    CHECK(seen.calls == 1);
}
#endif

#if ESP_LOG_MODE_TEXT_EN
TEST_CASE("Returning true preserves arguments and uses configured default output", "[handler]")
{
    Seen seen;
    OutputGuard output_guard;
    HandlerGuard guard;
    auto forward = [](esp_log_msg_t *message, void *ctx) {
        observe(message, ctx); // Uses its own argument copy.
        return true;
    };
    REQUIRE(esp_log_set_handler(forward, &seen, nullptr) == ESP_OK);
    esp_log_write(ESP_LOG_INFO, "handler", "value=%d", 42);
    CHECK(seen.text == "value=42");
    CHECK_FALSE(seen.timestamp_valid);
    CHECK(output == "value=42");
    REQUIRE(esp_log_set_handler(nullptr, nullptr, nullptr) == ESP_OK);
    esp_log_write(ESP_LOG_INFO, "handler", "restored");
    CHECK(output == "value=42restored");
}

TEST_CASE("Handler can suppress records and cannot bypass level filtering", "[handler]")
{
    Seen seen;
    OutputGuard output_guard;
    HandlerGuard guard;
    REQUIRE(esp_log_set_handler(observe, &seen, nullptr) == ESP_OK);
    const esp_log_level_t previous_level = esp_log_level_get("handler_filtered");
    esp_log_level_set("handler_filtered", ESP_LOG_NONE);
    esp_log_write(ESP_LOG_INFO, "handler_filtered", "hidden");
#if CONFIG_LOG_DYNAMIC_LEVEL_CONTROL
    CHECK(seen.calls == 0);
#endif
    esp_log_level_set("handler_filtered", ESP_LOG_VERBOSE);
    seen.calls = 0;
    esp_log_write(ESP_LOG_INFO, "handler_filtered", "captured");
    CHECK(seen.calls == 1);
    CHECK(output.empty());
    esp_log_level_set("handler_filtered", previous_level);
}

TEST_CASE("Application-owned wrappers stop or forward to default output once", "[handler]")
{
    OutputGuard output_guard;
    struct Context {
        esp_log_handler_t previous = {};
        unsigned calls = 0;
        bool stop = false;
    } base, wrapper;
    HandlerGuard guard;
    auto handler = [](esp_log_msg_t *message, void *ctx) {
        auto &state = *static_cast<Context *>(ctx);
        ++state.calls;
        if (state.stop) {
            return false;
        }
        return state.previous.fn ? state.previous.fn(message, state.previous.ctx) : true;
    };
    REQUIRE(esp_log_set_handler(handler, &base, &base.previous) == ESP_OK);
    REQUIRE(esp_log_set_handler(handler, &wrapper, &wrapper.previous) == ESP_OK);
    esp_log_write(ESP_LOG_INFO, "handler", "forwarded");
    CHECK(wrapper.calls == 1);
    CHECK(base.calls == 1);
    CHECK(output == "forwarded");

    wrapper.stop = true;
    esp_log_write(ESP_LOG_ERROR, "handler", "stopped at wrapper");
    CHECK(wrapper.calls == 2);
    CHECK(base.calls == 1);
    CHECK(output == "forwarded");

    wrapper.stop = false;
    base.stop = true;
    esp_log_write(ESP_LOG_WARN, "handler", "stopped at base");
    CHECK(base.calls == 2);
    CHECK(output == "forwarded");

    REQUIRE(esp_log_set_handler(wrapper.previous.fn, wrapper.previous.ctx, nullptr) == ESP_OK);
    base.stop = false;
    esp_log_write(ESP_LOG_INFO, "handler", "restored");
    CHECK(wrapper.calls == 3);
    CHECK(base.calls == 3);
    CHECK(output == "forwardedrestored");
    REQUIRE(esp_log_set_handler(base.previous.fn, base.previous.ctx, nullptr) == ESP_OK);
}

TEST_CASE("Nested logs bypass the installed handler and the next log is intercepted", "[handler]")
{
    unsigned calls = 0;
    OutputGuard output_guard;
    HandlerGuard guard;
    auto handler = [](esp_log_msg_t *message, void *ctx) {
        ++*static_cast<unsigned *>(ctx);
        // Bound re-entry so a broken guard fails assertions instead of overflowing the stack.
        if (strcmp(message->tag, "nested") != 0) {
            esp_log_write(ESP_LOG_WARN, "nested", "nested record");
        }
        return false;
    };
    REQUIRE(esp_log_set_handler(handler, &calls, nullptr) == ESP_OK);
    esp_log_write(ESP_LOG_INFO, "handler", "outer");
    CHECK(calls == 1);
    CHECK(output == "nested record");
    esp_log_write(ESP_LOG_INFO, "handler", "next outer");
    CHECK(calls == 2);
    CHECK(output == "nested recordnested record");
}

TEST_CASE("Replacement inside a handler affects later log calls", "[handler]")
{
    Seen replacement;
    struct Context {
        Seen *replacement;
        unsigned calls = 0;
        esp_err_t result = ESP_FAIL;
    } context{&replacement};
    OutputGuard output_guard;
    HandlerGuard guard;
    auto handler = [](esp_log_msg_t *, void *ctx) {
        auto &state = *static_cast<Context *>(ctx);
        ++state.calls;
        state.result = esp_log_set_handler(observe, state.replacement, nullptr);
        return true;
    };
    REQUIRE(esp_log_set_handler(handler, &context, nullptr) == ESP_OK);
    esp_log_write(ESP_LOG_INFO, "handler", "current");
    CHECK(context.result == ESP_OK);
    CHECK(context.calls == 1);
    CHECK(replacement.calls == 0);
    CHECK(output == "current");
    esp_log_write(ESP_LOG_INFO, "handler", "next");
    CHECK(context.calls == 1);
    CHECK(replacement.calls == 1);
    CHECK(replacement.text == "next");
    CHECK(output == "current");
}

#endif

TEST_CASE("Replacement does not wait and another thread still reaches the wrapper", "[handler]")
{
    struct Blocking {
        std::promise<void> entered;
        std::shared_future<void> release;
        std::atomic<unsigned> calls{0};
    } blocking;
    struct Wrapper {
        esp_log_handler_t previous = {};
        unsigned calls = 0;
        bool saw_previous = false;
    } replacement;
    HandlerGuard guard;
    std::promise<void> release;
    blocking.release = release.get_future().share();
    auto handler = [](esp_log_msg_t *, void *ctx) {
        auto &state = *static_cast<Blocking *>(ctx);
        if (state.calls.fetch_add(1) == 0) {
            state.entered.set_value();
            state.release.wait();
        }
        return false;
    };
    REQUIRE(esp_log_set_handler(handler, &blocking, nullptr) == ESP_OK);
    auto log_call = std::async(std::launch::async, [] { ESP_LOGI("handler", "blocked"); });
    const bool entered = blocking.entered.get_future().wait_for(2s) == std::future_status::ready;
    CHECK(entered);
    if (!entered) {
        release.set_value();
        log_call.get();
        return;
    }
    auto wrapper = [](esp_log_msg_t *message, void *ctx) {
        auto &state = *static_cast<Wrapper *>(ctx);
        ++state.calls;
        state.saw_previous = state.previous.fn != nullptr;
        return state.previous.fn ? state.previous.fn(message, state.previous.ctx) : false;
    };
    auto update = std::async(std::launch::async, [&] {
        return esp_log_set_handler(wrapper, &replacement, &replacement.previous);
    });
    const bool replaced = update.wait_for(2s) == std::future_status::ready;
    CHECK(replaced); // The blocked callback must not prevent replacement.
    if (replaced) {
        // A recursion guard shared between threads would incorrectly bypass this call.
        ESP_LOGI("handler", "replacement");
        CHECK(replacement.calls == 1);
        CHECK(replacement.saw_previous);
        CHECK(blocking.calls.load() == 2);
    }
    // Always release the producer before assertions that could abort this test.
    release.set_value();
    log_call.get();
    REQUIRE(update.get() == ESP_OK);
    CHECK(replacement.previous.fn == handler);
    CHECK(replacement.previous.ctx == &blocking);
    ESP_LOGI("handler", "after producer finished");
    CHECK(replacement.calls == (replaced ? 2 : 1));
}

#if ESP_LOG_MODE_BINARY_EN
TEST_CASE("Binary handlers preserve arguments for default output or suppress the packet", "[handler]")
{
    // The Linux ROM binary output writes directly to stdout, bypassing vprintf.
    struct StdoutCapture {
        FILE *file = tmpfile();
        int saved_fd = dup(STDOUT_FILENO);
        StdoutCapture()
        {
            fflush(stdout);
            if (file == nullptr || saved_fd < 0 || dup2(fileno(file), STDOUT_FILENO) < 0) {
                if (file) {
                    fclose(file);
                }
                if (saved_fd >= 0) {
                    close(saved_fd);
                }
                throw std::runtime_error("Cannot capture binary log output");
            }
        }
        ~StdoutCapture()
        {
            fflush(stdout);
            dup2(saved_fd, STDOUT_FILENO);
            close(saved_fd);
            fclose(file);
        }
        std::string read()
        {
            fflush(stdout);
            rewind(file);
            std::string packet;
            for (int c; (c = fgetc(file)) != EOF;) {
                packet.push_back(static_cast<char>(c));
            }
            return packet;
        }
    };
    struct Context {
        unsigned calls = 0;
        int value = 0;
        bool continue_output = true;
    } context;
    HandlerGuard guard;
    auto emit = [] {
        StdoutCapture capture;
        ESP_LOG_LEVEL(ESP_LOG_INFO | ESP_LOG_CONFIG_DIS_TIMESTAMP, "handler", "value=%d", 42);
        return capture.read();
    };
    const std::string expected = emit();
    REQUIRE_FALSE(expected.empty());
    auto handler = [](esp_log_msg_t *message, void *ctx) {
        auto &state = *static_cast<Context *>(ctx);
        ++state.calls;
        va_list args;
        va_copy(args, message->args);
        state.value = va_arg(args, int);
        va_end(args);
        return state.continue_output;
    };
    REQUIRE(esp_log_set_handler(handler, &context, nullptr) == ESP_OK);
    const std::string forwarded = emit();
    CHECK(context.calls == 1);
    CHECK(context.value == 42);
    CHECK(forwarded == expected);
    context.continue_output = false;
    const std::string suppressed = emit();
    CHECK(context.calls == 2);
    CHECK(suppressed.empty());
}
#endif
