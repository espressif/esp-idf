/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/parlio_tx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the alignment constraints for internal and external memory of the GDMA used in parlio_tx unit
 *
 * @param[in] tx_unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @param[out] int_mem_align Internal memory alignment
 * @param[out] ext_mem_align External memory alignment
 * @return
 *      - ESP_OK: Get alignment constraints successfully
 *      - ESP_ERR_INVALID_ARG: Get alignment constraints failed because of invalid argument
 *      - ESP_FAIL: Get alignment constraints failed because of other error
 */
esp_err_t parlio_tx_get_alignment_constraints(parlio_tx_unit_handle_t tx_unit, size_t *int_mem_align, size_t *ext_mem_align);

#ifdef __cplusplus
}
#endif
