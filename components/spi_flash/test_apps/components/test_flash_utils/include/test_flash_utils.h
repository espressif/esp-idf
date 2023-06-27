/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include "esp_partition.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Return the 'flash_test' custom data partition
 * defined in the custom partition table.
 *
 * @return partition handle
 */
const esp_partition_t *get_test_flash_partition(void);

#ifdef __cplusplus
}
#endif
