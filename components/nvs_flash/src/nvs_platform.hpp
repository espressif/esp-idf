// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef nvs_platform_h
#define nvs_platform_h


#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace nvs
{

class Lock
{
public:
    Lock()
    {
        if (mSemaphore) {
            xSemaphoreTake(mSemaphore, portMAX_DELAY);
        }
    }

    ~Lock()
    {
        if (mSemaphore) {
            xSemaphoreGive(mSemaphore);
        }
    }

    static esp_err_t init()
    {
        if (mSemaphore) {
            return ESP_OK;
        }
        mSemaphore = xSemaphoreCreateMutex();
        if (!mSemaphore) {
            return ESP_ERR_NO_MEM;
        }
        return ESP_OK;
    }

    static void uninit()
    {
        if (mSemaphore) {
            vSemaphoreDelete(mSemaphore);
        }
        mSemaphore = nullptr;
    }

    static SemaphoreHandle_t mSemaphore;
};
} // namespace nvs

#else // ESP_PLATFORM
namespace nvs
{
class Lock
{
public:
    Lock() { }
    ~Lock() { }
    static void init() {}
    static void uninit() {}
};
} // namespace nvs
#endif // ESP_PLATFORM


#endif /* nvs_platform_h */
