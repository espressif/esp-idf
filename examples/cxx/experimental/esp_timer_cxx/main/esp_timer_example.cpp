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

#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer_cxx.hpp"
#include "esp_exception.hpp"

using namespace std;
using namespace idf;
using namespace idf::esp_timer;

extern "C" void app_main(void)
{
    try {
        cout << "Setting up timer to trigger in 500ms" << endl;
        ESPTimer timer([]() { cout << "timeout" << endl; });
        timer.start(chrono::microseconds(200 * 1000));

        vTaskDelay(550 / portTICK_PERIOD_MS);

        cout << "Setting up timer to trigger periodically every 200ms" << endl;
        ESPTimer timer2([]() { cout << "periodic timeout" << endl; });
        timer2.start_periodic(chrono::microseconds(200 * 1000));

        vTaskDelay(1050 / portTICK_PERIOD_MS);

    } catch (const ESPException &e) {
        cout << "Exception with error: " << e.error << endl;
    }
}
