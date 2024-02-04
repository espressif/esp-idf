/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN ANY APPLICATIONS
// GDMA driver is not public for end users, but for ESP-IDF developers.

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize GDMA channel retention link for powerdown the TOP powerdomain during lightsleep
 * @param  group_id Group id
 * @param  pair_id  Pair id
 * @return
 *      - ESP_OK: Create DMA retention link successfully
 *      - ESP_ERR_NO_MEM: Create DMA retention link failed because out of memory
 */
esp_err_t gdma_sleep_retention_init(int group_id, int pair_id);

/**
 * Destroy GDMA channel retention link
 * @param  group_id Group id
 * @param  pair_id  Pair id
 * @return
 *      - ESP_OK: GDMA channel retention link destrory successfully
 *      - ESP_ERR_INVALID_STATE: GDMA channel retention link not create yet
 */
esp_err_t gdma_sleep_retention_deinit(int group_id, int pair_id);

#ifdef __cplusplus
}
#endif
