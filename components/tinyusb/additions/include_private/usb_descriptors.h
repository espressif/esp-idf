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

#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))

extern tusb_desc_device_t descriptor_tinyusb;
extern tusb_desc_strarray_device_t descriptor_str_tinyusb;

extern const tusb_desc_device_t descriptor_dev_kconfig;
extern tusb_desc_strarray_device_t descriptor_str_kconfig;
extern const uint8_t descriptor_cfg_kconfig[];

#ifdef __cplusplus
}
#endif
