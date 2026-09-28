/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_ADDR_H_
#define HOST_COMMON_ADDR_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "sdkconfig.h"

#include <zephyr/bluetooth/addr.h>

/* Copy between host and Zephyr LSB-first address order, either way. */
static inline void bt_le_addr_copy(uint8_t dst[BT_ADDR_SIZE],
                                   const uint8_t src[BT_ADDR_SIZE])
{
#if CONFIG_BT_BLUEDROID_ENABLED
    for (size_t i = 0; i < BT_ADDR_SIZE; i++) {
        dst[i] = src[BT_ADDR_SIZE - 1 - i];
    }
#else
    memcpy(dst, src, BT_ADDR_SIZE);
#endif
}

#endif /* HOST_COMMON_ADDR_H_ */
