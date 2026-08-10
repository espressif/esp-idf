/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get MSPI alignment requirement for an address
 *
 * The address is used so future targets can apply different alignment rules to
 * different MSPI-backed regions. Pass NULL when only the generic external-memory
 * requirement is needed and no concrete address is available yet.
 *
 * @param ptr Buffer pointer in the region to be accessed, or NULL for generic query
 * @return Required alignment in bytes, or 1 when no extra MSPI alignment is needed
 */
size_t esp_mspi_get_alignment(const void *ptr);

/**
 * @brief Check whether a buffer satisfies MSPI strict alignment requirements
 *
 * Returns false when @p ptr is NULL or @p size is 0 (not a valid transfer).
 * When strict alignment is not required, returns true for any non-empty buffer.
 * When required, both @p ptr and @p size must be aligned to the rule returned by
 * @ref esp_mspi_get_alignment for that address.
 *
 * @param ptr Buffer pointer
 * @param size Transfer size in bytes
 * @return true if alignment requirements are satisfied
 */
bool esp_mspi_buffer_alignment_satisfied(const void *ptr, size_t size);

#ifdef __cplusplus
}
#endif
