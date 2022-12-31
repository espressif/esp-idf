/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_partition.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Set the partition to store OpenThread dataset.
 *
 * @param[in]   partition   The storage partition.
 *
 */
void esp_openthread_flash_set_partition(const esp_partition_t *partition);

#ifdef __cplusplus
}
#endif
