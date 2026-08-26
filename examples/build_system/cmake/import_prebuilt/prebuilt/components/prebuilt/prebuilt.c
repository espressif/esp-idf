/* Import Prebuilt Library Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_log.h"

const char *TAG = "prebuilt";

void prebuilt_func(void)
{
    const esp_partition_t* running_partition = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "The running partition is '%s'!", running_partition->label);
}
