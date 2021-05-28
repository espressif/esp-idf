/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is a STUB FILE used when compiling ESP-IDF to run tests on the host system.
 * The source file used normally for ESP-IDF has the same name but is located elsewhere.
 */
#include "esp_ota_ops.h"
#include "esp_partition.h"

const esp_partition_t* esp_ota_get_running_partition(void)
{
    // Return first instance of an app partition
    const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
                                                     ESP_PARTITION_SUBTYPE_ANY,
                                                     NULL);
    assert(partition != NULL);

    return partition;
}
