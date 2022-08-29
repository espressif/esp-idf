/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_etm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the ETM event handle of systick hardware's alarm/heartbeat event
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] core_id CPU core ID
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t esp_systick_new_etm_alarm_event(int core_id, esp_etm_event_handle_t *out_event);

#ifdef __cplusplus
}
#endif
