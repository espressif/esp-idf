/* ESP Event C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <iostream>
#include "esp_event_cxx.hpp"
#include "esp_event.h"
#include "esp_err.h"

using namespace idf::event;
using namespace std;

ESP_EVENT_DEFINE_BASE(TEST_EVENT_BASE);
const ESPEventID TEST_EVENT_ID_0(0);
const ESPEventID TEST_EVENT_ID_1(1);

ESPEvent TEMPLATE_EVENT_0(TEST_EVENT_BASE, TEST_EVENT_ID_0);
ESPEvent TEMPLATE_EVENT_1(TEST_EVENT_BASE, TEST_EVENT_ID_1);

// We use "normal" static functions here. However, passing std::function types also works with
// ESPEventLoop::register_event() and ESPEventLoop::register_event_timed(), allowing to reference custom data.
static void callback(const ESPEvent &event, void *data)
{
    cout << "received event: " << event.base << "/" << event.id;
    if (data) {
        cout << "; data: " << *(static_cast<int*>(data));
    }
    cout << endl;
};

static void timeout_callback(const ESPEvent &event)
{
    cout << "TIMEOUT for event: " << event.base << "/" << event.id << endl;
};

extern "C" void app_main(void)
{
    {
        cout << "Normal testing..." << endl;
        ESPEventLoop loop;
        int data = 47;
        int captured_data = 42;

        unique_ptr<ESPEventReg> reg_1 = loop.register_event(TEMPLATE_EVENT_0,
                [captured_data](const ESPEvent &event, void *data) {
            cout << "received event: " << event.base << "/" << event.id;
            if (data) {
                cout << "; event data: " << *(static_cast<int*>(data));
            }
            cout << "; handler data: " << captured_data << endl;
        });
        unique_ptr<ESPEventReg> reg_2;

        // Run for 4 seconds...
        for (int i = 0; i < 4; i++) {
            switch (i) {
                case 0:
                    // will be received
                    loop.post_event_data(TEMPLATE_EVENT_0, data);
                    break;
                case 1:
                    // will NOT be received because TEST_EVENT_ID_1 hasn't been registered yet
                    loop.post_event_data(TEMPLATE_EVENT_1);
                    break;
                case 2:
                    // register TEST_EVENT_ID_1
                    reg_2 = loop.register_event(TEMPLATE_EVENT_1, callback);
                    // will be received
                    loop.post_event_data(TEMPLATE_EVENT_1);
                    break;
                case 3:
                    // unregister callback with TEST_EVENT_ID_1 again
                    reg_2.reset();
                    // will NOT be received
                    loop.post_event_data(TEMPLATE_EVENT_1);
                    break;

            }
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    {
        cout << endl << "Timeout testing..." << endl;
        ESPEventLoop loop;

        // Setting timeout and sending event early enough.
        unique_ptr<ESPEventRegTimed> timed_reg = loop.register_event_timed(TEMPLATE_EVENT_0,
                callback,
                chrono::milliseconds(500),
                timeout_callback);
        loop.post_event_data(TEMPLATE_EVENT_0);
        cout << endl;

        // Setting timeout and sending event too late.
        // Note: the old registration will be properly unregistered by resetting the unique_ptr.
        timed_reg = loop.register_event_timed(TEMPLATE_EVENT_0,
                callback,
                chrono::milliseconds(500),
                timeout_callback);
        this_thread::sleep_for(chrono::seconds(1));
        loop.post_event_data(TEMPLATE_EVENT_0);

    }
    cout << "Finished example" << endl;
}
