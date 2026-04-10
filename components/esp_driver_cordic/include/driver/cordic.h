/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/cordic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CORDIC engine configuration structure
 */
typedef struct {
    cordic_clock_source_t clock_source;  /*!< CORDIC clock source */
} cordic_engine_config_t;

/**
 * @brief CORDIC calculation configuration structure
 */
typedef struct {
    cordic_func_t function;            /*!< CORDIC calculation function type (e.g., cosine, sine, arctan) */
    cordic_iq_format_t iq_format;      /*!< IQ data format */
    uint16_t iteration_count;          /*!< Internal CORDIC iteration count */
    uint16_t scale_exp;                /*!< Input scaling exponent n, effective scaling is 2^n (range depends on function type) */
} cordic_calculate_config_t;

/**
 * @brief Create a new CORDIC engine instance
 *
 * This function creates and initializes a CORDIC engine with the specified configuration.
 * The engine handle returned can be used for subsequent CORDIC calculations.
 *
 * @param[in] cordic_cfg Pointer to CORDIC engine configuration structure. Must not be NULL.
 * @param[out] ret_engine Pointer to store the created CORDIC engine handle. Must not be NULL.
 *
 * @return
 *      - ESP_OK: CORDIC engine created successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid argument (NULL pointer or invalid scale exponent).
 *      - ESP_ERR_NO_MEM: Failed to allocate memory for the engine.
 */
esp_err_t cordic_new_engine(const cordic_engine_config_t *cordic_cfg, cordic_engine_handle_t *ret_engine);

/**
 * @brief Start one-shot CORDIC calculation
 *
 * This function performs CORDIC calculations on a batch of data points. It processes each data point
 * sequentially: sets arguments, starts calculation, waits for completion, and retrieves results.
 *
 * @note The function can be safely used in ISR, but for the ISR run in cache-safe, please
 * enable ``CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM`` to place the control functions into IRAM.
 * @note This function is not guaranteed to be thread-safe.
 *
 * @param[in] engine CORDIC engine handle created by cordic_new_engine(). Must not be NULL.
 * @param[in] calc_cfg Pointer to CORDIC calculation configuration structure. Must not be NULL.
 * @param[in] input_buffer_desc Pointer to input buffer descriptor containing input data. Must not be NULL.
 * @param[out] output_buffer_desc Pointer to output buffer descriptor for storing results. Must not be NULL.
 * @param[in] buffer_depth Number of data points to process.
 *
 * @return
 *      - ESP_OK: All calculations completed successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid argument (NULL pointer, zero buffer_depth, or invalid timeout).
 *      - ESP_ERR_TIMEOUT: Calculation timeout.
 */
esp_err_t cordic_calculate_polling(cordic_engine_handle_t engine, const cordic_calculate_config_t *calc_cfg, cordic_input_buffer_desc_t *input_buffer_desc, cordic_output_buffer_desc_t *output_buffer_desc, size_t buffer_depth);

/**
 * @brief Delete a CORDIC engine instance
 *
 * This function deletes a CORDIC engine and frees all associated resources including the mutex.
 * After calling this function, the engine handle becomes invalid and should not be used.
 *
 * @param[in] engine CORDIC engine handle to delete. Must not be NULL.
 *
 * @return
 *      - ESP_OK: Engine deleted successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid argument (NULL pointer).
 */
esp_err_t cordic_delete_engine(cordic_engine_handle_t engine);

/**
 * @brief Convert CORDIC hex value to float
 *
 * This function converts a hexadecimal value from CORDIC hardware output to a floating-point number.
 * The conversion depends on the IQ format specified by the iq_format parameter.
 *
 * @param[in] fixed_value Hexadecimal value from CORDIC hardware output.
 * @param[in] iq_format IQ data format.
 *
 * @return Converted floating-point value. Returns 0.0f if iq_format is invalid.
 */
float cordic_convert_fixed_to_float(uint32_t fixed_value, cordic_iq_format_t iq_format);

/**
 * @brief Convert float to CORDIC hex value
 *
 * This function converts a floating-point number to a hexadecimal value suitable for CORDIC hardware input.
 * The conversion depends on the IQ format specified by the iq_format parameter.
 *
 * @param[in] float_value Floating-point value to convert. Should be in the range [-1.0, 1.0].
 * @param[in] iq_format IQ data format.
 *
 * @return Converted hexadecimal value. Returns 0 if iq_format is invalid.
 */
uint32_t cordic_convert_float_to_fixed(float float_value, cordic_iq_format_t iq_format);

#ifdef __cplusplus
}
#endif
