// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of musical buzzer interface
 *
 */
typedef struct musical_buzzer_t musical_buzzer_t;

/**
 * @brief Type of musical buzzer underlying device
 *
 */
typedef void *musical_buzzer_dev_t;

/**
 * @brief Type of musical buzzer notation
 *
 */
typedef struct {
    uint32_t note_freq_hz;     /*!< Note frequency, in Hz */
    uint32_t note_duration_ms; /*!< Note duration, in ms */
} musical_buzzer_notation_t;

/**
 * @brief Declaration of musical buzzer interface
 *
 */
struct musical_buzzer_t {
    /**
    * @brief Start to play the given notation
    *
    * @param buzzer musical buzzer handle
    * @param notation music notation
    * @param notation_len notation length
    * @return
    *      - ESP_OK: Start playing notation successfully
    *      - ESP_ERR_INVALID_ARG: wrong parameter
    */
    esp_err_t (*play)(musical_buzzer_t *buzzer, const musical_buzzer_notation_t *notation, uint32_t notation_len);

    /**
     * @brief Stop playing
     *
     * @param buzzer musical buzzer handle
     * @return
     *      - ESP_OK: Stop playing successfully
     */
    esp_err_t (*stop)(musical_buzzer_t *buzzer);

    /**
     * @brief Free memory used by musical buzzer
     *
     * @param buzzer musical buzzer handle
     * @return
     *      - ESP_OK: Recycle memory successfully
     */
    esp_err_t (*del)(musical_buzzer_t *buzzer);
};

typedef struct {
    musical_buzzer_dev_t dev; /*!< Musical buzzer device (e.g. RMT channel, PWM channel, etc) */
    int flags;                /*!< Extra flags */
} musical_buzzer_config_t;

/**
 * @brief Default musical buzzer configuration
 *
 */
#define MUSICAL_BUZZER_DEFAULT_CONFIG(dev_hdl) \
    {                                          \
        .dev = dev_hdl,                        \
        .flags = 0,                            \
    }

/**
 * @brief Create musical buzzer instance based on RMT driver
 *
 * @param config musical buzzer configuration
 * @param[out] ret_handle returned handle of musical buzzer instance
 * @return
 *      - ESP_OK: create musical buzzer instance successfully
 *      - ESP_ERR_INVALID_ARG: wrong parameter
 *      - ESP_ERR_NO_MEM: no memory to allocate instance
 */
esp_err_t musical_buzzer_create_rmt(const musical_buzzer_config_t *config, musical_buzzer_t **ret_handle);

#ifdef __cplusplus
}
#endif
