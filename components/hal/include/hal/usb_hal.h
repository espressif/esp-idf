/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool use_external_phy;
} usb_hal_context_t;

void usb_hal_init(usb_hal_context_t *usb);


#ifdef __cplusplus
}
#endif
