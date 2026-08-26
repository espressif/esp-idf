/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "cybergear_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register the CyberGear commands
 */
void register_cybergear_commands(cybergear_handle_t motor);

#ifdef __cplusplus
}
#endif
