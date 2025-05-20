/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/bitscrambler_types.h"
#if SOC_BITSCRAMBLER_SUPPORTED
#include "soc/bitscrambler_peri_select.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Declare a BitScrambler binary program.
 *
 * This macro declares an external reference to a BitScrambler binary program.
 * The binary program is expected to be linked into the binary with a specific
 * naming convention.
 *
 * @param VAR The variable name to declare.
 * @param NAME The name of the binary program.
 */
#define BITSCRAMBLER_PROGRAM(VAR, NAME) extern const uint8_t VAR[] asm("_binary_bitscrambler_program_" NAME "_start")

/**
 * @brief Handle for the bitscrambler instance.
 *
 * This typedef defines a handle for a bitscrambler instance, which is used to
 * manage and interact with the bitscrambler. The handle is a pointer to an
 * opaque structure, meaning that the internal details of the structure are
 * hidden from the user.
 */
typedef struct bitscrambler_t *bitscrambler_handle_t;

/**
 * @brief BitScrambler configuration
 */
typedef struct {
    bitscrambler_direction_t dir;   /*!< Direction (tx or rx) */
    int attach_to;                  /*!< Peripheral to attach to. One of SOC_BITSCRAMBLER_ATTACH_. */
} bitscrambler_config_t;

/**
 * @brief Allocate BitScrambler handle for a hardware channel
 *
 * @note This function can only be used to create a single direction BitScrambler handle.
 *       If you need a loopback BitScrambler, call bitscrambler_loopback_create() instead.
 *
 * @param config Configuration for requested BitScrambler
 * @param[out] handle BitScrambler controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_NO_MEM: No memory available
 *        - ESP_ERR_NOT_FOUND: No free hardware channel available
 */
esp_err_t bitscrambler_new(const bitscrambler_config_t *config, bitscrambler_handle_t *handle);

/**
 * @brief Free previously allocated BitScrambler handle
 *
 * @param handle Previously allocated handle
 */
void bitscrambler_free(bitscrambler_handle_t handle);

/**
 * @brief Load a BitScrambler binary program into BitScrambler memory
 *
 * @param handle BitScrambler handle
 * @param program Binary program to load
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Not a valid or recognized BitScrambler binary, or invalid handle
 */
esp_err_t bitscrambler_load_program(bitscrambler_handle_t handle, const void *program);

/**
 * @brief Load data into the Look-Up Table
 *
 * @param handle BitScrambler handle
 * @param lut Data to load
 * @param size_bytes Size of the data, in bytes
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Invalid handle or lut pointer
 */
esp_err_t bitscrambler_load_lut(bitscrambler_handle_t handle, void *lut, size_t size_bytes);

/**
 * @brief Start executing BitScrambler program
 *
 * @param handle BitScrambler handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Invalid handle
 */
esp_err_t bitscrambler_start(bitscrambler_handle_t handle);

/**
 * @brief Reset BitScrambler program and FIFOs for a new transaction. Note that this does not
 *        affect the loaded program itself.
 *
 * @param handle BitScrambler handle
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Invalid handle
 */
esp_err_t bitscrambler_reset(bitscrambler_handle_t handle);

/**
 * @brief Enable BitScrambler
 * @note  This function should be called before bitscrambler_load_program, bitscrambler_load_lut, bitscrambler_reset and bitscrambler_start.
 *
 * @param handle BitScrambler handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Invalid handle
 */
esp_err_t bitscrambler_enable(bitscrambler_handle_t handle);

/**
 * @brief Disable BitScrambler
 * @note  This function should be called before bitscrambler_free.
 *
 * @param handle BitScrambler handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Invalid handle
 */
esp_err_t bitscrambler_disable(bitscrambler_handle_t handle);

#ifdef __cplusplus
}
#endif
