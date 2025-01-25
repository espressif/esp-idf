/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "driver/bitscrambler.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef esp_err_t (*bitscrambler_extra_clean_up_func_t)(bitscrambler_handle_t bs, void* user_ctx);

esp_err_t bitscrambler_register_extra_clean_up(bitscrambler_handle_t handle, bitscrambler_extra_clean_up_func_t clean_up, void* user_ctx);

#ifdef __cplusplus
}
#endif
