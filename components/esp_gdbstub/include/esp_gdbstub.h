/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void esp_gdbstub_init(void);
void esp_gdbstub_panic_handler(void *frame);
void update_breakpoints(void);

#ifdef __cplusplus
}
#endif
