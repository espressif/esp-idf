/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ----------------- */
/* BLE Log - Utility */
/* ----------------- */

/* INCLUDE */
#include "ble_log_util.h"

/* VARIABLE */
#ifndef UNIT_TEST
portMUX_TYPE ble_log_spin_lock = portMUX_INITIALIZER_UNLOCKED;
#endif /* !UNIT_TEST */

/* INTERNAL INTERFACE */
BLE_LOG_IRAM_ATTR uint32_t ble_log_fast_checksum(const uint8_t *data, size_t len)
{
    uint32_t sum = 0;
    size_t i = 0;

    /* Step 1: Sum up until 4-byte aligned */
    while (((uintptr_t)(data + i) & 0x3) && (i < len)) {
        sum += data[i++];
    }

    /* Step 2: Sum up 4-byte aligned blocks */
    const uint32_t *p32 = (const uint32_t *)(data + i);
    size_t blocks = (len - i) / 4;
    for (size_t b = 0; b < blocks; b++) {
        uint32_t v = p32[b];
        sum += (v & 0xFF)
             + ((v >> 8)  & 0xFF)
             + ((v >> 16) & 0xFF)
             + ((v >> 24) & 0xFF);
    }
    i += blocks * 4;

    /* Step 3: Sum up remaining bytes */
    while (i < len) {
        sum += data[i++];
    }

    return sum;
}
