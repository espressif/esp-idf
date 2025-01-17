/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//This file contains private functions for interop between bitscrambler.c
//and bitscrambler_loopback.c.

#pragma once

#include "bitscrambler_private.h"

#ifdef __cplusplus
extern "C" {
#endif

void bitscrambler_loopback_free(bitscrambler_handle_t bs);

#ifdef __cplusplus
}
#endif
