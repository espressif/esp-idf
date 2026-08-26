/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DESCRIPTOR_TYPE_ACM 0
#define USB_DESCRIPTOR_TYPE_DFU 1

void usb_set_current_descriptor(int descriptor_type);

bool usb_get_descriptor(uint16_t type_index, uint16_t lang_id,
                        int32_t *len, uint8_t **data);

#ifdef __cplusplus
}
#endif
