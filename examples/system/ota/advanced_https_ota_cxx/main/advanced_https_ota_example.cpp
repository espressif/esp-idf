/* Advanced HTTPS OTA example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "ota.h"
#include <iostream>

using std::cout;
using std::endl;
using std::runtime_error;

/* A simple class which may throw an exception from constructor */
class Throwing {
public:
    Throwing(int arg)
        : m_arg(arg)
    {
        cout << "In constructor, arg=" << arg << endl;
        if (arg == 0) {
            throw runtime_error("Exception in constructor");
        }
    }

    ~Throwing()
    {
        cout << "In destructor, m_arg=" << m_arg << endl;
    }

protected:
    int m_arg;
};

/* Inside .cpp file, app_main function must be declared with C linkage */
extern "C" void app_main()
{
    ota_init();
    xTaskCreate(&advanced_ota_example_task, "advanced_ota_example_task", 1024 * 8, NULL, 5, NULL);

    cout << "app_main starting" << endl;

    try {
        /* This will succeed */
        Throwing obj1(42);

        /* This will throw an exception */
        Throwing obj2(0);

        cout << "This will not be printed" << endl;
    } catch (const runtime_error &e) {
        cout << "Exception caught: " << e.what() << endl;
    }

    cout << "app_main done" << endl;
}
