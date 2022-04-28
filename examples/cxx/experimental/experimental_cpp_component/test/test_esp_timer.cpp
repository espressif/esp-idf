// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef __cpp_exceptions

#include "unity.h"
#include "unity_cxx.hpp"
#include <limits>
#include <stdio.h>

#include <iostream>
#include "test_utils.h" // ref clock
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_timer_cxx.hpp"
#include "esp_exception.hpp"

using namespace std;
using namespace idf;
using namespace idf::esp_timer;

struct RefClock {
    RefClock()
    {
        ref_clock_init();
    };

    ~RefClock()
    {
        ref_clock_deinit();
    }
};

TEST_CASE("ESPTimer produces correct delay", "[ESPTimer]")
{
    int64_t t_end;

    RefClock ref_clock;

    function<void()> timer_cb = [&t_end]() {
        t_end = ref_clock_get();
    };

    ESPTimer timer(timer_cb, "timer1");

    const int delays_ms[] = {20, 100, 200, 250};
    const size_t delays_count = sizeof(delays_ms)/sizeof(delays_ms[0]);

    for (size_t i = 0; i < delays_count; ++i) {
        t_end = 0;
        int64_t t_start = ref_clock_get();

        timer.start(chrono::microseconds(delays_ms[i] * 1000));

        vTaskDelay(delays_ms[i] * 2 / portTICK_PERIOD_MS);
        TEST_ASSERT(t_end != 0);
        int32_t ms_diff = (t_end - t_start) / 1000;
        printf("%d %d\n", delays_ms[i], ms_diff);

        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, delays_ms[i], ms_diff);
    }
}

TEST_CASE("ESPtimer produces correct periodic delays", "[ESPTimer]")
{
    const size_t NUM_INTERVALS = 3u;

    size_t cur_interval = 0;
    int intervals[NUM_INTERVALS];
    int64_t t_start;
    SemaphoreHandle_t done;

    const int DELAY_MS = 100;
    function<void()> timer_cb = [&]() {
        int64_t t_end = ref_clock_get();
        int32_t ms_diff = (t_end - t_start) / 1000;
        printf("timer #%d %dms\n", cur_interval, ms_diff);
        if (cur_interval < NUM_INTERVALS) {
           intervals[cur_interval++] = ms_diff;
        }
        // Deliberately make timer handler run longer.
        // We check that this doesn't affect the result.
        esp_rom_delay_us(10*1000);
        if (cur_interval == NUM_INTERVALS) {
            printf("done\n");
            xSemaphoreGive(done);
        }
    };

    ESPTimer timer(timer_cb, "timer1");
    RefClock ref_clock;
    t_start = ref_clock_get();
    done = xSemaphoreCreateBinary();
    timer.start_periodic(chrono::microseconds(DELAY_MS * 1000));

    TEST_ASSERT(xSemaphoreTake(done, DELAY_MS * NUM_INTERVALS * 2));
    timer.stop();

    TEST_ASSERT_EQUAL_UINT32(NUM_INTERVALS, cur_interval);
    for (size_t i = 0; i < NUM_INTERVALS; ++i) {
        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, (i + 1) * DELAY_MS, intervals[i]);
    }
    TEST_ESP_OK(esp_timer_dump(stdout));

    vSemaphoreDelete(done);
#undef NUM_INTERVALS
}

#endif // __cpp_exceptions
