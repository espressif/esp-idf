/* ESP Timer C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <iostream>
#include <thread>
#include <chrono>

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

        this_thread::sleep_for(std::chrono::milliseconds(550));

        cout << "Setting up timer to trigger periodically every 200ms" << endl;
        ESPTimer timer2([]() { cout << "periodic timeout" << endl; });
        timer2.start_periodic(chrono::microseconds(200 * 1000));

        this_thread::sleep_for(std::chrono::milliseconds(1050));
    } catch (const ESPException &e) {
        cout << "Exception with error: " << e.error << endl;
    }
}
