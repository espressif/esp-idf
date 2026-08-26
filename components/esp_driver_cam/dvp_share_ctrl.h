/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Claim DVP IO signal usage
 */
esp_err_t dvp_shared_ctrl_claim_io_signals(void);

/**
 * @brief  Declaim DVP IO signal usage
 */
esp_err_t dvp_shared_ctrl_declaim_io_signals(void);

#ifdef __cplusplus
}
#endif
