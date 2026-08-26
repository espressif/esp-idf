/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_err.h"
#include "driver/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief LP VAD peripheral
 */
typedef uint32_t lp_vad_t;

/**
 * @brief Type of VAD unit handle
 */
typedef struct vad_unit_ctx_t *vad_unit_handle_t;

/**
 * @brief LP VAD configurations
 */
typedef struct {
    int init_frame_num;               /**< Number of init frames that are used for VAD to denoise, this helps the VAD to decrease the accidental trigger ratio.
                                           Note too big values may lead to voice activity miss */
    int min_energy_thresh;            ///< Minimum energy threshold, voice activities with energy higher than this value will be detected.
    bool skip_band_energy_thresh;     /**< Skip band energy threshold or not, the passband energy check determines whether the proportion of passband energy within the total frequency domain meets the required threshold.
                                           Note in different environments, enabling the passband energy check may reduce false trigger rates but could also increase the rate of missed detections. */

    int speak_activity_thresh;        /**< When in speak-activity-listening-state, if number of the detected speak activity is higher than this value, VAD runs into speak-activity-detected-state */

    int non_speak_activity_thresh;    /**< When in speak-activity-detected-state, if the number of the detected speak activity is higher than this value, but lower than `max_speak_activity_thresh`:
                                           - if the number of the detected non-speak activity is higher than this value, VAD runs into speak-activity-listening-state
                                           - if the number of the detected non-speak activity is lower than this value, VAD keeps in speak-activity-detected-state */

    int min_speak_activity_thresh;    /**< When in speak-activity-detected-state, if the number of the detected speak activity is higher than this value, but lower than `max_speak_activity_thresh`,
                                           then the VAD state machine will depends on the value of `non_speak_activity_thresh` */

    int max_speak_activity_thresh;    /**< When in speak-activity-detected-state, if the number of the detected speak activity is higher than this value, VAD runs into speak-activity-listening-state */
} lp_vad_config_t;

/**
 * @brief LP VAD Init Configurations
 */
typedef struct {
    lp_i2s_chan_handle_t    lp_i2s_chan;  ///< LP I2S channel handle
    lp_vad_config_t         vad_config;   ///< LP VAD config
} lp_vad_init_config_t;

/**
 * @brief New LP VAD unit
 * @param[in]  vad_id        VAD id
 * @param[in]  init_config   Initial configurations
 * @param[out] ret_unit      Unit handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_i2s_vad_new_unit(lp_vad_t vad_id, const lp_vad_init_config_t *init_config, vad_unit_handle_t *ret_unit);

/**
 * @brief Enable LP VAD
 *
 * @param[in] unit          VAD handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_i2s_vad_enable(vad_unit_handle_t unit);

/**
 * @brief Disable LP VAD
 *
 * @param[in] unit          VAD handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_i2s_vad_disable(vad_unit_handle_t unit);

/**
 * @brief Delete LP VAD unit
 * @param[in] unit          VAD handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t lp_i2s_vad_del_unit(vad_unit_handle_t unit);

#ifdef __cplusplus
}
#endif
