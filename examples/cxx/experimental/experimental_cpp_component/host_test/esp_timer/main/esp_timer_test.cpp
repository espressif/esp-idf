/* ESP Timer C++ unit tests

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#define CATCH_CONFIG_MAIN

#include <stdio.h>
#include <stdexcept>
#include "esp_err.h"
#include "esp_timer_cxx.hpp"

#include "catch.hpp"

extern "C" {
#include "Mockesp_timer.h"
}

// TODO: IDF-2693, function definition just to satisfy linker, mock esp_common instead
const char *esp_err_to_name(esp_err_t code) {
    return "test";
}

using namespace std;
using namespace idf;
using namespace idf::esp_timer;

struct FixtureException : std::exception {
    const char *what() const noexcept override {
        return "CMock failed";
    }
};

struct TimerCreationFixture {
    TimerCreationFixture(bool expect_stop = false) : out_handle(reinterpret_cast<esp_timer_handle_t>(1))
    {
        if (!TEST_PROTECT()) {
            throw FixtureException();
        }
        esp_timer_create_ExpectAnyArgsAndReturn(ESP_OK);
        esp_timer_create_ReturnThruPtr_out_handle(&out_handle);
        if (expect_stop) {
            esp_timer_stop_ExpectAndReturn(out_handle, ESP_OK); // implementation may always call stop
        } else {
            esp_timer_stop_IgnoreAndReturn(ESP_OK); // implementation may always call stop
        }
        esp_timer_delete_ExpectAndReturn(out_handle, ESP_OK);
    }

    virtual ~TimerCreationFixture()
    {
        Mockesp_timer_Verify();
    }

    esp_timer_handle_t out_handle;
};

static void (*trigger_timer_callback)(void *data) = nullptr;

esp_err_t cmock_timer_create_callback(const esp_timer_create_args_t* create_args, esp_timer_handle_t* out_handle, int cmock_num_calls)
{
    trigger_timer_callback = create_args->callback;
    return ESP_OK;
}

struct TimerCallbackFixture : public TimerCreationFixture {
    TimerCallbackFixture(bool expect_stop = false) : TimerCreationFixture(expect_stop)
    {
        esp_timer_create_AddCallback(cmock_timer_create_callback);
    }

    ~TimerCallbackFixture()
    {
        trigger_timer_callback = nullptr;
    }
};

TEST_CASE("get_time works")
{
    esp_timer_get_time_ExpectAndReturn(static_cast<uint64_t>(0xfeeddeadbeef));

    CHECK(get_time() == std::chrono::microseconds(0xfeeddeadbeef));
}

TEST_CASE("get_next_alarm works")
{
    esp_timer_get_next_alarm_ExpectAndReturn(static_cast<uint64_t>(47u));

    CHECK(get_next_alarm() == std::chrono::microseconds(47u));
}

TEST_CASE("ESPTimer null function")
{
    CHECK_THROWS_AS(ESPTimer(nullptr), ESPException&);
}

TEST_CASE("ESPTimer empty std::function")
{
    function<void()> nothing;
    CHECK_THROWS_AS(ESPTimer(nothing, "test"), ESPException&);
}

TEST_CASE("ESPTimer initializes and deletes itself")
{
    TimerCreationFixture fix;

    function<void()> timer_cb = [&]() { };

    ESPTimer(timer_cb, "test");
}

TEST_CASE("ESPTimer start throws on invalid state failure")
{
    TimerCreationFixture fix;
    esp_timer_start_once_ExpectAndReturn(fix.out_handle, 5000, ESP_ERR_INVALID_STATE);

    function<void()> timer_cb = [&]() { };

    ESPTimer timer(timer_cb);

    CHECK_THROWS_AS(timer.start(chrono::microseconds(5000)), ESPException&);
}

TEST_CASE("ESPTimer start periodically throws on invalid state failure")
{
    TimerCreationFixture fix;
    esp_timer_start_periodic_ExpectAndReturn(fix.out_handle, 5000, ESP_ERR_INVALID_STATE);

    function<void()> timer_cb = [&]() { };

    ESPTimer timer(timer_cb);

    CHECK_THROWS_AS(timer.start_periodic(chrono::microseconds(5000)), ESPException&);
}

TEST_CASE("ESPTimer stopp throws on invaid state failure")
{
    TimerCreationFixture fix;

    // Overriding stop part of the fixture
    esp_timer_stop_StopIgnore();
    esp_timer_stop_IgnoreAndReturn(ESP_ERR_INVALID_STATE);

    function<void()> timer_cb = [&]() { };

    ESPTimer timer(timer_cb);

    CHECK_THROWS_AS(timer.stop(), ESPException&);
}

TEST_CASE("ESPTimer stops in destructor")
{
    TimerCreationFixture fix(true);
    esp_timer_start_once_ExpectAndReturn(fix.out_handle, 5000, ESP_OK);

    function<void()> timer_cb = [&]() { };

    ESPTimer timer(timer_cb);

    timer.start(chrono::microseconds(5000));
}

TEST_CASE("ESPTimer stops correctly")
{
    TimerCreationFixture fix(true);
    esp_timer_start_once_ExpectAndReturn(fix.out_handle, 5000, ESP_OK);

    // Additional stop needed because stop is called in ESPTimer::stop and ~ESPTimer.
    esp_timer_stop_ExpectAndReturn(fix.out_handle, ESP_OK);

    function<void()> timer_cb = [&]() { };

    ESPTimer timer(timer_cb);

    timer.start(chrono::microseconds(5000));

    timer.stop();
}

TEST_CASE("ESPTimer callback works")
{
    TimerCallbackFixture fix;
    int flag = 0;

    function<void()> timer_cb = [&]() { flag = 47; };

    ESPTimer timer(timer_cb);

    trigger_timer_callback(&timer);

    REQUIRE(trigger_timer_callback != nullptr);
    CHECK(flag == 47);
}
