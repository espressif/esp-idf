/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

/**
 * A struct covering every fundamental data type size, used to verify that
 * PLACE_IN_SECTION() correctly places and aligns section entries on both
 * 32-bit embedded targets and 64-bit host builds (Linux/macOS).
 *
 * Layout (no padding on any platform when members are in descending size order):
 *   ptr    : pointer  — 8 bytes on 64-bit, 4 bytes on 32-bit
 *   u32    : uint32_t — 4 bytes
 *   u16    : uint16_t — 2 bytes
 *   u8     : uint8_t  — 1 byte
 */
typedef struct {
    const char *name; /* pointer — drives struct alignment on 64-bit */
    uint32_t    u32;
    uint16_t    u16;
    uint8_t     u8;
} test_ptr_entry_t;
