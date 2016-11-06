/* Non-Volatile Storage (NVS) Read and Write Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

void app_main()
{
    nvs_flash_init();
    system_init();

    nvs_handle handle_to_settings;
    esp_err_t err;
    int32_t restart_counter = 0;

    // Open the NVS
    printf("Opening Non-Volatile Storage (NVS) ... ");
    err = nvs_open("settings", NVS_READWRITE, &handle_to_settings);
    printf((err != ESP_OK) ? "Failed!\n" : "OK\n");

    // Read from the NVS
    printf("Reading restart counter from NVS ... ");
    err = nvs_get_i32(handle_to_settings, "restart_conter", &restart_counter);
    switch (err) {
        case ESP_OK:
            printf("OK\n");
            printf("Restart counter = %d\n", restart_counter);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The counter is not initialized yet!\n");
            break;
        default :
            printf("Error (%d) reading!\n", err);
    }

    // Write to the NVS
    printf("Updating restart counter in NVS ... ");
    restart_counter++;
    err = nvs_set_i32(handle_to_settings, "restart_conter", restart_counter);
    printf((err != ESP_OK) ? "Failed!\n" : "OK\n");

    // Close the NVS
    nvs_close(handle_to_settings);

    // Restart module
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    system_restart();
}
