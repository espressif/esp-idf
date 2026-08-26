/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of buzzer musical score
 */
typedef struct {
    uint32_t freq_hz;     /*!< Frequency, in Hz */
    uint32_t duration_ms; /*!< Duration, in ms */
} buzzer_musical_score_t;

/**
 * @brief Type of musical score encoder configuration
 */
typedef struct {
    uint32_t resolution; /*!< Encoder resolution, in Hz */
} musical_score_encoder_config_t;

/**
 * @brief Create RMT encoder for encoding musical score into RMT symbols
 *
 * @param[in] config Encoder configuration
 * @param[out] ret_encoder Returned encoder handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating musical score encoder
 *      - ESP_OK if creating encoder successfully
 */
esp_err_t rmt_new_musical_score_encoder(const musical_score_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

#ifdef __cplusplus
}
#endif
