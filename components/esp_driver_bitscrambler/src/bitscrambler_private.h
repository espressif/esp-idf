/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "hal/bitscrambler_ll.h"
#include "esp_private/bitscrambler.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bitscrambler_t bitscrambler_t;

struct bitscrambler_t {
    bitscrambler_config_t cfg;
    bitscrambler_dev_t *hw;
    bitscrambler_extra_clean_up_func_t extra_clean_up; // Optional extra clean-up function
    void* clean_up_user_ctx; // User context for extra clean-up function
    bool loopback; //true if this is a loopback bitscrambler, i.e. the RX channel is also claimed
};

esp_err_t bitscrambler_init_loopback(bitscrambler_handle_t handle, const bitscrambler_config_t *config);

#ifdef __cplusplus
}
#endif
