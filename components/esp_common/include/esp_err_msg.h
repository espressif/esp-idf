/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure describing an error code entry for the link-time error table.
 *
 * Components register their error codes by placing arrays of this structure
 * into the ".esp_err_msg_tbl" linker section. The esp_err_to_name() function
 * iterates over all such entries at runtime.
 */
typedef struct {
    esp_err_t code;     /*!< Error code value */
    const char *msg;    /*!< String representation of the error code name */
} esp_err_msg_t;

#ifdef __cplusplus
}
#endif
