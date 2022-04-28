// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <atomic>

typedef void (*cb_t)(void* arg);

class TimerTaskMock
{
public:
    TimerTaskMock(cb_t cb): cb(cb), t(run_static, this), active(false), ms(INT32_MAX) {}
    ~TimerTaskMock(void) { active = false; t.join(); }

    void SetTimeout(uint32_t m)
    {
        ms = m;
        active = true;
    }

private:

    static void run_static(TimerTaskMock* timer)
    {
        timer->run();
    }

    void run(void)
    {
        while (!active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        while (active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            cb(nullptr);
        }
    }

    cb_t cb;
    std::thread t;
    std::atomic<bool> active;
    uint32_t ms;

};
