/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

void register_i2ctools(void);

extern i2c_master_bus_handle_t tool_bus_handle;

#ifdef __cplusplus
}
#endif
