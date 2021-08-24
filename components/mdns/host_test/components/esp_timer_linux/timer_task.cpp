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

#include "timer_task.hpp"
#include <cstdint>
#include <vector>
#include <memory>
#include <cstring>

extern "C" void * create_tt(cb_t cb)
{
    auto * tt = new TimerTaskMock(cb);
    return tt;
}

extern "C" void destroy_tt(void* tt)
{
    auto * timer_task = static_cast<TimerTaskMock *>(tt);
    delete(timer_task);
}


extern "C" void set_tout(void* tt, uint32_t ms)
{
    auto * timer_task = static_cast<TimerTaskMock *>(tt);
    timer_task->SetTimeout(ms);
}
