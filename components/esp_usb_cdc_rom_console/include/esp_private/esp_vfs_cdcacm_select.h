/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CDCACM_SELECT_READ_NOTIF,
    CDCACM_SELECT_WRITE_NOTIF,
    CDCACM_SELECT_ERROR_NOTIF,
} cdcacm_select_notif_t;

typedef void (*cdcacm_select_notif_callback_t)(cdcacm_select_notif_t cdcacm_select_notif, BaseType_t *task_woken);

/**
 * @brief Set notification callback function for select() events
 * @param cdcacm_select_notif_callback callback function
 */
void cdcacm_set_select_notif_callback(cdcacm_select_notif_callback_t cdcacm_select_notif_callback);

#ifdef __cplusplus
}
#endif
