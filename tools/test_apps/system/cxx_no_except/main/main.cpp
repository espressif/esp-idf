/*
    No except example.

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_system.h"
#include <new>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main()
{
    char *char_array = new (std::nothrow) char [47];

    for (int i = 10; i >= 0; i--) {
        char_array[i] = i;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    esp_restart();

    delete [] char_array;
}
