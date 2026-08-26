/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_err.h>
#include <esp_openthread.h>
#include <esp_openthread_types.h>

#define ESP_OPENTHREAD_BORDER_ROUTER_FLAG_OF_INTEREST                                                       \
    (OT_CHANGED_THREAD_ROLE | OT_CHANGED_THREAD_EXT_PANID | OT_CHANGED_THREAD_NETWORK_NAME |                \
     OT_CHANGED_ACTIVE_DATASET | OT_CHANGED_THREAD_PARTITION_ID | OT_CHANGED_THREAD_BACKBONE_ROUTER_STATE | \
     OT_CHANGED_PSKC)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initiizes OpenThread state event post.
 *
 * @pram[in] instance The OpenThread instance
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if OpenThread state changed callback fails to be registered
 *
 */
esp_err_t esp_openthread_state_event_init(otInstance *instance);

#ifdef __cplusplus
}
#endif
