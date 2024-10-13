/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void esp_log_impl_lock(void);
bool esp_log_impl_lock_timeout(void);
void esp_log_impl_unlock(void);

#ifdef __cplusplus
}
#endif
