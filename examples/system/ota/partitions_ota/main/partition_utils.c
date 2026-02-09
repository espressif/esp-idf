/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/param.h>
#include "esp_flash_partitions.h"
#include "esp_image_format.h"
#include "esp_flash.h"
#include "esp_partition.h"
#include "esp_log.h"

// Returns the number of partitions belonging to the specified flash chip
static unsigned int count_partitions(esp_flash_t *flash_chip)
{
    unsigned int partition_count = 0;
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    assert(it != NULL);
    for (; it != NULL; it = esp_partition_next(it)) {
        if (esp_partition_get(it)->flash_chip == flash_chip) {
            partition_count++;
        }
    }
    esp_partition_iterator_release(it);
    return partition_count;
}

// Compare function for qsort to sort partitions by address
static int compare_partitions(const void *a, const void *b) {
    const esp_partition_t *part_a = *(const esp_partition_t **)a;
    const esp_partition_t *part_b = *(const esp_partition_t **)b;
    return ((int32_t)part_a->address - (int32_t)part_b->address);
}

static const esp_partition_t **get_sorted_partition_array(esp_flash_t *flash_chip, unsigned int *out_partition_count)
{
    // Number of partitions on the flash chip
    unsigned int partition_count = count_partitions(flash_chip);
    assert(partition_count != 0);
    *out_partition_count = partition_count;

    // Allocate an array for copying partitions
    const esp_partition_t **part_array = (const esp_partition_t **) calloc(partition_count, sizeof(esp_partition_t *));
    assert(part_array != NULL);

    // Copy partitions to the array
    unsigned i = 0;
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    assert(it != NULL);
    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *p = esp_partition_get(it);
        if (p->flash_chip == flash_chip) {
            part_array[i++] = p;
        }
    }
    esp_partition_iterator_release(it);
    assert(i == partition_count);

    // Sort partitions by address using qsort
    qsort(part_array, partition_count, sizeof(esp_partition_t *), compare_partitions);
    return part_array;
}

esp_err_t partition_utils_find_unallocated(esp_flash_t *flash_chip, size_t required_size, uint32_t start_offset, uint32_t *found_offset, size_t *found_size)
{
    if (flash_chip == NULL) {
        flash_chip = esp_flash_default_chip;
    }

    if (required_size == 0 || found_offset == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Get a sorted partition array for the given flash chip
    unsigned int partition_count;
    const esp_partition_t **sorted_partitions = get_sorted_partition_array(flash_chip, &partition_count);
    if (sorted_partitions == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // Scan for unallocated space between partitions, starting after the start_offset with unused space >= required_size
    uint32_t last_end_address = start_offset;
    for (size_t i = 0; i < partition_count; ++i) {
        if (sorted_partitions[i]->address > start_offset && sorted_partitions[i]->address > last_end_address) {
            size_t unallocated_size = sorted_partitions[i]->address - last_end_address;
            if (unallocated_size >= required_size) {
                *found_offset = last_end_address;
                if (found_size != NULL) {
                    *found_size = unallocated_size;
                }
                free(sorted_partitions);
                return ESP_OK;
            }
        }
        last_end_address = sorted_partitions[i]->address + sorted_partitions[i]->size;
    }

    // Check for unallocated space after the last partition
    last_end_address = MAX(last_end_address, start_offset);
    uint32_t flash_size = 0;
    esp_err_t ret = esp_flash_get_size(flash_chip, &flash_size);
    if (ret != ESP_OK) {
        free(sorted_partitions);
        return ret;
    }
    size_t unallocated_size = flash_size - last_end_address;
    if (unallocated_size >= required_size) {
        *found_offset = last_end_address;
        if (found_size != NULL) {
            *found_size = unallocated_size;
        }
        free(sorted_partitions);
        return ESP_OK;
    }

    free(sorted_partitions);
    return ESP_ERR_NOT_FOUND;
}
