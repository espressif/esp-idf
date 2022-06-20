/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_NIMBLE_MEM_H__
#define __ESP_NIMBLE_MEM_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// #pragma message "This file should be replaced with bt_osi_mem.h, used here for compatibility"

#include "bt_osi_mem.h"
#define nimble_platform_mem_malloc bt_osi_mem_malloc
#define nimble_platform_mem_calloc bt_osi_mem_calloc
#define nimble_platform_mem_free bt_osi_mem_free

#ifdef __cplusplus
}
#endif

#endif /* __ESP_NIMBLE_MEM_H__ */
