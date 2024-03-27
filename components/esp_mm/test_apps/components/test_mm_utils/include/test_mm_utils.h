/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include "esp_err.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set addr space dirty
 *
 * @param[in] vaddr_start  start addr of the space
 * @param[in] size         size of the space
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Currently no support for non-4B-aligned space
 */
esp_err_t test_set_buffer_dirty(intptr_t vaddr_start, size_t size);

#ifdef __cplusplus
}
#endif
