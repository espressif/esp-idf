/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/lp_i2s_vad.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LP VAD configurations
 */
typedef lp_vad_init_config_t lp_core_lp_vad_cfg_t;

/**
 * @brief State Machine
                                           ┌──────────────────────────────────┐
                                           │                                  │
                             ┌─────────────┤  speak-activity-listening-state  │ ◄───────────────┐
                             │             │                                  │                 │
                             │             └──────────────────────────────────┘                 │
                             │                          ▲                                       │
                             │                          │                                       │
                             │                          │                                       │
                             │                          │                                       │
                             │                          │                                       │
detected speak activity      │                          │  detected speak activity              │   detected speak activity
        >=                   │                          │          >=                           │           >=
'speak_activity_thresh'      │                          │  'min_speak_activity_thresh'          │   'max_speak_activity_thresh'
                             │                          │                                       │
                             │                          │          &&                           │
                             │                          │                                       │
                             │                          │  detected non-speak activity          │
                             │                          │           <                           │
                             │                          │  'non_speak_activity_thresh'          │
                             │                          │                                       │
                             │                          │                                       │
                             │                          │                                       │
                             │                          │                                       │
                             │                          │                                       │
                             │              ┌───────────┴─────────────────────┐                 │
                             │              │                                 │                 │
                             └───────────►  │ speak-activity-detected-state   ├─────────────────┘
                                            │                                 │
                                            └─┬───────────────────────────────┘
                                              │
                                              │                     ▲
                                              │                     │
                                              │                     │
                                              │                     │  detected speak activity
                                              │                     │          >=
                                              │                     │  'min_speak_activity_thresh'
                                              │                     │
                                              │                     │          &&
                                              │                     │
                                              │                     │  detected non-speak activity
                                              │                     │           <
                                              └─────────────────────┘  'non_speak_activity_thresh'
*/

/**
 * @brief LP VAD init
 *
 * @param[in] vad_id        VAD ID
 * @param[in] init_config   Initial configurations
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_core_lp_vad_init(lp_vad_t vad_id, const lp_core_lp_vad_cfg_t *init_config);

/**
 * @brief Enable LP VAD
 *
 * @param[in] vad_id        VAD ID
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_core_lp_vad_enable(lp_vad_t vad_id);

/**
 * @brief Disable LP VAD
 *
 * @param[in] vad_id        VAD ID
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_core_lp_vad_disable(lp_vad_t vad_id);

/**
 * @brief Deinit LP VAD
 *
 * @param[in] vad_id        VAD ID
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_core_lp_vad_deinit(lp_vad_t vad_id);

#ifdef __cplusplus
}
#endif
