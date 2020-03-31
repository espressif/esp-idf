/* ESP Event C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <iostream>
#include <thread>
#include "esp_event_cxx.hpp"
#include "esp_event.h"
#include "esp_err.h"

using namespace idf::event;
using namespace std;

#define EVENT_NUM 10

ESP_EVENT_DEFINE_BASE(TEST_EVENT_BASE);
static ESPEventID TEST_EVENT_ID_0(0);
static ESPEventID TEST_EVENT_ID_1(1);

// the events we want to register
static ESPEvent TEMPLATE_EVENT_0(TEST_EVENT_BASE, TEST_EVENT_ID_0);
static ESPEvent TEMPLATE_EVENT_1(TEST_EVENT_BASE, TEST_EVENT_ID_1);

// helper function to post events, simulating an event source
void post_events() {
    for (int i = 0; i < EVENT_NUM; i++) {
        ESP_ERROR_CHECK(esp_event_post(TEST_EVENT_BASE,
                i % 2 ? TEST_EVENT_ID_1.get_id() : TEST_EVENT_ID_0.get_id(),
                nullptr,
                0,
                portMAX_DELAY));
        this_thread::sleep_for(chrono::seconds(1));
    }
}

extern "C" void app_main(void)
{
    ESPEventHandlerSync event_handler(make_shared<ESPEventLoop>());
    event_handler.listen_to(TEMPLATE_EVENT_0);
    event_handler.listen_to(TEMPLATE_EVENT_1);
    cout << "started event loop" << endl;

    thread th(post_events);

    // waiting for two events to be posted via post_events()
    this_thread::sleep_for(chrono::milliseconds(1100));

    // reading the two already received events, then running into timeout
    for (;;) {
        ESPEventHandlerSync::EventResultTimed result = event_handler.wait_event_for(std::chrono::milliseconds(500));

        if (result.timeout) { // if timeout, then the default esp event will be sent with invalid base
            break;
        } else {
            cout << "event base " << result.event.base
                    << ", ID: " << result.event.id
                    << "; called first round" << endl;
        }
    }

    cout << "received timeout" << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));

    // Read the events we missed up until now and then continue reading
    for (int i = 0; i < EVENT_NUM - 2; i++) {
        ESPEventHandlerSync::EventResult result = event_handler.wait_event();
        cout << "event base " << result.event.base
                << ", ID: " << result.event.id
                << "; called second round" << endl;
    }

    th.join();

    // checking whether events were missed by the ESPEventHandlerSync class
    cout << "Missed: " << event_handler.get_send_queue_errors() << " events" << endl;
}
