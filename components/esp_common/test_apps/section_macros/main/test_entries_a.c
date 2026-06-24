/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "test_ptr_entry.h"
#include <stdint.h>

/*
 * Three entries placed in the test_data_table section from translation unit A.
 * The test verifies these values are discoverable at link time.
 */
static const uint32_t entry_a1 PLACE_IN_SECTION("test_data_table") = 0xDEADBEEF;
static const uint32_t entry_a2 PLACE_IN_SECTION("test_data_table") = 0xCAFEBABE;
static const uint32_t entry_a3 PLACE_IN_SECTION("test_data_table") = 0x12345678;

/*
 * Two pointer-containing struct entries from translation unit A.
 * Fields cover every data type size: pointer, u64, u32, u16, u8.
 */
static const test_ptr_entry_t ptr_a1 PLACE_IN_SECTION("test_ptr_table") = {
    .name = "ptr_a1", .u32 = 0xA1A2A3A4, .u16 = 0xA5A6, .u8 = 0xA7
};
static const test_ptr_entry_t ptr_a2 PLACE_IN_SECTION("test_ptr_table") = {
    .name = "ptr_a2", .u32 = 0xB1B2B3B4, .u16 = 0xB5B6, .u8 = 0xB7
};
