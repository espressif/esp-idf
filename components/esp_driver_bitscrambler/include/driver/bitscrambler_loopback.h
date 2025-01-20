/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "driver/bitscrambler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create handle for BitScrambler in loopback mode
 *
 * @note Use bitscrambler_free to free created handle.
 *
 * @param[out] handle BitScrambler handle
 * @param attach_to Peripheral to attach to. One of SOC_BITSCRAMBLER_ATTACH_. The BitScrambler
 *        must be attached to some peripheral, even in loopback mode. This peripheral does not
 *        need to be initialized for the BitScrambler to work. However, it can be initialized
 *        and will work as normal, with the exception that DMA functionality for this peripheral
 *        cannot be used.
 * @param max_transfer_sz_bytes Maximum transfer size, in bytes, of either the incoming or outgoing data
 *        fed to bitscrambler_loopback_run.
 *
 * @returns
 *        - ESP_OK
 *        - ESP_ERR_NO_MEM: No memory available
 *        - ESP_ERR_NOT_FOUND: No free hardware channel available
 *        - ESP_ERR_INVALID_ARG: Invalid argument passed to function
 *        - ESP_FAIL: Bitscrambler object creation failed because of some other error
 */
esp_err_t bitscrambler_loopback_create(bitscrambler_handle_t *handle, int attach_to, size_t max_transfer_sz_bytes);

/**
 * @brief Run Bitscrambler program on a data buffer
 *
 * @param bs BitScrambler handle. This BitScrambler should have a program loaded using
 *           bitscrambler_load_program()
 * @param buffer_in Data to feed into the BitScrambler
 * @param length_bytes_in Size of the data in buffer_in, in bytes
 * @param buffer_out Buffer for BitScrambler to write processed data to
 * @param length_bytes_out Size of output buffer
 * @param[out] bytes_written Pointer to variable to store the size of actual data written
 *                           to output buffer. Can be NULL if not needed.
 *
 * @returns
 *        - ESP_OK
 *        - ESP_ERR_INVALID_SIZE if a buffer size exceeds max_transfer_sz_bytes
 *        - ESP_ERR_TIMEOUT if BitScrambler program does not complete
 */
esp_err_t bitscrambler_loopback_run(bitscrambler_handle_t bs, void *buffer_in, size_t length_bytes_in, void *buffer_out, size_t length_bytes_out, size_t *bytes_written);

#ifdef __cplusplus
}
#endif
