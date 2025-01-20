/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//This file contains private functions for interop between bitscrambler.c
//and bitscrambler_loopback.c.
#pragma once

#include <stdbool.h>
#include "soc/bitscrambler_peri_select.h"
#include "hal/bitscrambler_ll.h"

typedef struct bitscrambler_t bitscrambler_t;

struct bitscrambler_t {
    bitscrambler_config_t cfg;
    bitscrambler_dev_t *hw;
    bool loopback;      //true if this is a loopback bitscrambler, i.e. the RX
    //channel is also claimed
};

esp_err_t bitscrambler_init_loopback(bitscrambler_handle_t handle, const bitscrambler_config_t *config);
