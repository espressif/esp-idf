/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "hal/bitscrambler_types.h"
#include "soc/bitscrambler_peri_select.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BITSCRAMBLER_PROGRAM(VAR, NAME) extern const uint8_t VAR[] asm("_binary_bitscrambler_program_" NAME "_start")

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

#ifdef __cplusplus
}
#endif
