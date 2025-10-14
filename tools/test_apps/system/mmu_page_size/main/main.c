/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "unity.h"
#include "esp_partition.h"
#include "esp_image_format.h"

#define SZ 4096

void app_main(void)
{
    printf("App is running\n");

    // Perform some partition and memory map related operations
    char src_p_1[32] = "Test data pattern 123456789";
    char buf[32];

    // Find storage partition
    const esp_partition_t* partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    TEST_ASSERT_NOT_NULL(partition);

    esp_partition_mmap_handle_t handle1;
    const void *ptr1;
    TEST_ESP_OK(esp_partition_mmap(partition, 0, SZ, ESP_PARTITION_MMAP_DATA, &ptr1, &handle1));
    TEST_ESP_OK(esp_partition_erase_range(partition, 0, SZ));
    TEST_ESP_OK(esp_partition_write(partition, 0, src_p_1, sizeof(src_p_1)));
    memcpy(buf, ptr1, sizeof(buf));

    TEST_ASSERT_EQUAL(0, memcmp(buf, src_p_1, sizeof(buf)));
    esp_partition_munmap(handle1);

    printf("Partition test done\n");

    // Get the factory partition
    partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    if (partition == NULL) {
        printf("Failed to find factory partition\n");
        return;
    }

    esp_image_metadata_t data;
    const esp_partition_pos_t part_pos = {
      .offset = partition->address,
      .size = partition->size,
    };

    if (esp_image_verify(ESP_IMAGE_VERIFY, &part_pos, &data) != ESP_OK) {
        printf("Failed to verify image\n");
    } else {
        printf("Image verified successfully\n");
    }
    return;
}
