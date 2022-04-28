/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "tusb.h"
#include "tinyusb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void tusb_set_descriptor(const tusb_desc_device_t *dev_desc, const char **str_desc, const uint8_t *cfg_desc);
tusb_desc_device_t *tusb_get_active_desc(void);
char **tusb_get_active_str_desc(void);
void tusb_clear_descriptor(void);

#ifdef __cplusplus
}
#endif
