/* Select Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"

static const char* TAG = "core_dump_userdata_example";

static uint8_t userdata[512];

uint32_t esp_core_dump_userdata_get_len()
{
  return sizeof(userdata);
}

void * esp_core_dump_userdata_get()
{
  return (void *) userdata;
}

void app_main()
{
  // initialize userdata
  memset(userdata, 0, sizeof(userdata));

  // write something interesting to userdata
  const esp_app_desc_t * app_desc = esp_ota_get_app_description();
  snprintf((char *) userdata, sizeof(userdata),
    "PROJECT_VERSION=%s,IDF_VER=%s,BUILD_DATE=%s,BUILD_TIME=%s",
    app_desc->version,
    IDF_VER,
    app_desc->date,
    app_desc->time
  );

  // show current coredump contents
  const esp_partition_t * part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                              ESP_PARTITION_SUBTYPE_ANY, "coredump");
  size_t addr = 4; // skip data_len
  char old_userdata[sizeof(userdata)];
  ESP_ERROR_CHECK(esp_partition_read(part, addr, old_userdata, sizeof(old_userdata)));
  ESP_LOGI(TAG, "Userdata from current coredump partition: ");
  ESP_LOGI(TAG, "%s", old_userdata);
  ESP_LOG_BUFFER_HEX(TAG, old_userdata, sizeof(old_userdata));

  // wait so we don't bootloop so fast
  ESP_LOGI(TAG, "Waiting 2 seconds...");
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  // intentionally crash the system
  ESP_LOGI(TAG, "Crashing now!");
  esp_partition_t * x = 0;
  x->size = 0;
  // coredump will be written and chip will reset

  // we never reach here
  ESP_LOGI(TAG, "Unreachable!");
}
