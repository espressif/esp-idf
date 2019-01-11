/* OTA Tool example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"

static const char *TAG = "example";

void app_main(void)
{
    ESP_LOGI(TAG, "OTA Tool Example");

    const esp_partition_t *running = esp_ota_get_running_partition();

    // Display the running partition
    ESP_LOGI(TAG, "Running partition: %s", running->label);

    ESP_LOGI(TAG, "Example end");
}
