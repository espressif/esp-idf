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
#if CONFIG_BLE_LOG_XOR_CHECKSUM_ENABLED
#include "esp_compiler.h"

static inline uint32_t ror32(uint32_t word, uint32_t shift)
{
    if (unlikely(shift == 0)) {
        return word;
    }
    return (word >> shift) | (word << ((32 - shift) & 0x1F));
}

__attribute__((optimize("-O3")))
BLE_LOG_IRAM_ATTR
uint32_t ble_log_fast_checksum(const uint8_t *data, size_t len)
{
    /* Validate input length */
    if (unlikely(len == 0)) {
        return 0;
    }

    /* Step 1: Force 32-bit align read and calculate offset */
    const uint32_t start_offset_shift = ((uintptr_t)data & 0x3) << 3;
    const uint32_t *p_aligned = (const uint32_t *)((uintptr_t)data & ~0x3);
    const uint32_t *p_end_aligned = (const uint32_t *)((uintptr_t)(data + len) & ~0x3);

    /* Step 2: Handle first word with mask (Little endian) */
    uint32_t checksum = (*p_aligned) & (0xFFFFFFFFU << start_offset_shift);

    /* Step 3: Check if first word is last word */
    const uint32_t end_offset_shift = ((uintptr_t)(data + len) & 0x3) << 3;
    const uint32_t end_offset_shift_mask = ~(0xFFFFFFFFU << end_offset_shift);
    if (unlikely(p_aligned == p_end_aligned)) {
        if (end_offset_shift) {
            checksum &= end_offset_shift_mask;
        }
    } else {
        /* Step 4: Handle word in the middle */
        p_aligned++;
        while (p_aligned < p_end_aligned) {
            checksum ^= *p_aligned++;
        }

        /* Step 5: Handle last word with mask (Little endian) */
        if (end_offset_shift) {
            checksum ^= (*p_aligned) & end_offset_shift_mask;
        }
    }

    /* Step 6: Rotate the final result */
    return ror32(checksum, start_offset_shift);
}
#else /* !CONFIG_BLE_LOG_XOR_CHECKSUM_ENABLED */
BLE_LOG_IRAM_ATTR
uint32_t ble_log_fast_checksum(const uint8_t *data, size_t len)
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
#endif /* CONFIG_BLE_LOG_XOR_CHECKSUM_ENABLED */
