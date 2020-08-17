/* USB Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp32tinyusb.h"

// extern "C" {
// #include "tinyusb.h"
// }

static const char *TAG = "example";

TinyUSB device = TinyUSB();
// USB Device Driver task
// This top level thread processes all usb events and invokes callbacks
static void usb_device_task(void *param) {
    (void)param;
    ESP_LOGI(TAG, "USB task started");
    while (1) {
        tud_task(); // RTOS forever loop
        vTaskDelay(5);
    }
}

extern "C" void app_main(void) {

    // Create a task for tinyusb device stack:
    xTaskCreate(usb_device_task, "usbd", 4096, NULL, 5, NULL);
    return;
}
