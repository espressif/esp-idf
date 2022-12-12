/* Finding Partitions Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <assert.h>
#include "esp_partition.h"
#include "esp_log.h"

static const char *TAG = "example";

void app_main(void)
{
    /*
    * This example uses the partition table from ../partitions_example.csv. For reference, its contents are as follows:
    *
    *  nvs,        data, nvs,      0x9000,  0x6000,
    *  phy_init,   data, phy,      0xf000,  0x1000,
    *  factory,    app,  factory,  0x10000, 1M,
    *  storage,    data, ,             , 0x40000,
    */

    // Find the partition map in the partition table
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition != NULL);

    static const char store_data[] = "ESP-IDF Partition Memory Map Example";

    // Prepare data to be read later using the mapped address
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
    ESP_ERROR_CHECK(esp_partition_write(partition, 0, store_data, sizeof(store_data)));
    ESP_LOGI(TAG, "Written sample data to partition: %s", store_data);

    const void *map_ptr;
    esp_partition_mmap_handle_t map_handle;

    // Map the partition to data memory
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, ESP_PARTITION_MMAP_DATA, &map_ptr, &map_handle));
    ESP_LOGI(TAG, "Mapped partition to data memory address %p", map_ptr);

    // Read back the written verification data using the mapped memory pointer
    char read_data[sizeof(store_data)];
    memcpy(read_data, map_ptr, sizeof(read_data));
    ESP_LOGI(TAG, "Read sample data from partition using mapped memory: %s", (char*) read_data);

    assert(strcmp(store_data, read_data) == 0);
    ESP_LOGI(TAG, "Data matches");

    // Unmap mapped memory
    esp_partition_munmap(map_handle);
    ESP_LOGI(TAG, "Unmapped partition from data memory");

    ESP_LOGI(TAG, "Example end");
}
