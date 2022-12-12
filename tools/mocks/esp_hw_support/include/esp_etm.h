/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * NOTE: this is not the original header file from the esp_hw_support component.
 * It is a stripped-down copy to support mocking.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ETM channel handle
 */
typedef struct esp_etm_channel_t *esp_etm_channel_handle_t;

/**
 * @brief ETM event handle
 */
typedef struct esp_etm_event_t *esp_etm_event_handle_t;

/**
 * @brief ETM task handle
 */
typedef struct esp_etm_task_t *esp_etm_task_handle_t;

#ifdef __cplusplus
}
#endif
