/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_partition.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Find the APP partition containing the currently executing firmware.
 *
 * Private helper shared by esp_partition and app_update; application code
 * should prefer esp_ota_get_running_partition() (app_update). The result is
 * cached after the first lookup.
 *
 * @return Running APP partition
 */
const esp_partition_t *esp_partition_get_running_partition(void);

#ifdef __cplusplus
}
#endif
