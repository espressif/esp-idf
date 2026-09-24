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
    cordic_clock_source_t clock_source;  /*!< CORDIC clock source, 0 means CORDIC_CLK_SRC_DEFAULT */
    uint32_t instance_id;                /*!< CORDIC hardware instance ID, index from 0 */
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
 * @brief Acquire a CORDIC engine instance
 *
 * The CORDIC hardware is shared by everyone who needs trigonometric or hyperbolic math, so this
 * function does not necessarily create anything: the first caller initializes the hardware and
 * every later caller just gets one more reference to the very same engine. The hardware stays up
 * until the last reference is given back with cordic_release_engine().
 *
 * @note This function takes a mutex and may block. Call it from a task, not from an ISR.
 * @note Do not mix this function with the deprecated cordic_new_engine() / cordic_delete_engine().
 * @note A later clock_source is ignored. The first acquirer decides the clock until the last release.
 *
 * @param[in] cordic_cfg Pointer to CORDIC engine configuration structure. Must not be NULL.
 * @param[out] ret_engine Pointer to store the acquired CORDIC engine handle. Must not be NULL.
 *
 * @return
 *      - ESP_OK: CORDIC engine acquired successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid argument (NULL pointer or unsupported instance_id).
 *      - ESP_ERR_INVALID_STATE: Engine was created by cordic_new_engine().
 *      - ESP_ERR_NO_MEM: Failed to allocate memory for the engine.
 */
esp_err_t cordic_acquire_engine(const cordic_engine_config_t *cordic_cfg, cordic_engine_handle_t *ret_engine);

/**
 * @brief Release a CORDIC engine instance
 *
 * This function gives back one reference to the CORDIC engine. The hardware is deinitialized and the
 * engine memory freed once the last reference is released; until then the handle stays valid for the
 * other users.
 *
 * @note This function takes a mutex and may block. Call it from a task, not from an ISR.
 *
 * @param[in] engine CORDIC engine handle acquired by cordic_acquire_engine(). Must not be NULL.
 *
 * @return
 *      - ESP_OK: Engine reference released successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid argument (NULL pointer).
 *      - ESP_ERR_INVALID_STATE: No reference is currently held, or the handle is not the live engine.
 */
esp_err_t cordic_release_engine(cordic_engine_handle_t engine);

/**
 * @brief Perform one-shot CORDIC calculation, polling the hardware until each point completes
 *
 * This function performs CORDIC calculations on a batch of data points. It processes each data point
 * sequentially: sets arguments, starts calculation, waits for completion, and retrieves results.
 *
 * The chip has a single CORDIC. Register access is serialized with a short critical section, so
 * tasks, ISRs and both cores can call this function without extra locking. Keep ``buffer_depth``
 * small: interrupts on the calling core are masked for the duration of the batch.
 *
 * @note The function can be safely used in ISR, but for the ISR run in cache-safe, please
 * enable ``CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM`` to place the control functions into IRAM.
 * @note The engine handle must have been acquired with cordic_acquire_engine() and must not be
 * released concurrently.
 *
 * @param[in] engine CORDIC engine handle acquired by cordic_acquire_engine(). Must not be NULL.
 * @param[in] calc_cfg Pointer to CORDIC calculation configuration structure. Must not be NULL.
 * @param[in] input_buffer_desc Pointer to input buffer descriptor containing input data. Must not be NULL.
 * @param[out] output_buffer_desc Pointer to output buffer descriptor for storing results. Must not be NULL.
 * @param[in] buffer_depth Number of data points to process.
 *
 * @return
 *      - ESP_OK: All calculations completed successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid pointer, zero buffer_depth, or invalid calculation configuration.
 *      - ESP_ERR_TIMEOUT: Calculation did not complete within the driver's polling retry limit.
 */
esp_err_t cordic_calculate_polling(cordic_engine_handle_t engine, const cordic_calculate_config_t *calc_cfg, cordic_input_buffer_desc_t *input_buffer_desc, cordic_output_buffer_desc_t *output_buffer_desc, size_t buffer_depth);

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

/** @cond */ /* Hide deprecated APIs from Doxygen programming docs */
/**
 * @brief Create a CORDIC engine instance.
 *
 * @deprecated Use cordic_acquire_engine() and cordic_release_engine() instead.
 *
 * This function initializes the CORDIC hardware and stores its handle in ``ret_engine``.
 * The engine handle can be used for subsequent CORDIC calculations. Destroy it with cordic_delete_engine().
 *
 * @note This function fails if the same hardware instance is already created,
 * whether by a previous cordic_new_engine() or by cordic_acquire_engine().
 * A second create is not a way to allocate another engine.
 * @note Do not mix this function with cordic_acquire_engine() / cordic_release_engine().
 * @note Pair this function with cordic_delete_engine() to destroy the engine.
 *
 * @param[in] cordic_cfg Pointer to CORDIC engine configuration structure. Must not be NULL.
 * @param[out] ret_engine Pointer to store the created CORDIC engine handle. Must not be NULL.
 *
 * @return
 *      - ESP_OK: CORDIC engine created successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid argument (NULL pointer or unsupported instance_id).
 *      - ESP_ERR_NOT_FOUND: Engine is already created for this hardware instance.
 *      - ESP_ERR_NO_MEM: Failed to allocate memory for the engine.
 */
esp_err_t cordic_new_engine(const cordic_engine_config_t *cordic_cfg, cordic_engine_handle_t *ret_engine)
__attribute__((deprecated("Please use cordic_acquire_engine instead")));

/**
 * @brief Delete a CORDIC engine instance
 *
 * @deprecated Use cordic_acquire_engine() and cordic_release_engine() instead.
 *
 * @note This function is the counterpart of the cordic_new_engine(), it tears the engine down immediately.
 * @note This function fails if the handle is not the live engine, if it was already deleted,
 * or if the engine is currently held via cordic_acquire_engine().
 * @note Do not mix this function with cordic_acquire_engine() / cordic_release_engine().
 *
 * @param[in] engine CORDIC engine handle created by cordic_new_engine(). Must not be NULL.
 *
 * @return
 *      - ESP_OK: Engine deleted successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid argument (NULL pointer).
 *      - ESP_ERR_INVALID_STATE: Handle is not the live engine, already deleted, or still acquired.
 */
esp_err_t cordic_delete_engine(cordic_engine_handle_t engine)
__attribute__((deprecated("Please use cordic_acquire_engine and cordic_release_engine instead")));
/** @endcond */

#ifdef __cplusplus
}
#endif
