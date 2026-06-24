/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "test_ptr_entry.h"
#include <stdint.h>

/*
 * Two entries placed in the test_data_table section from a DIFFERENT
 * translation unit.  This verifies the linker collects entries across
 * multiple object files.
 */
static const uint32_t entry_b1 PLACE_IN_SECTION("test_data_table") = 0xAAAAAAAA;
static const uint32_t entry_b2 PLACE_IN_SECTION("test_data_table") = 0x55555555;

/*
 * One pointer-containing struct entry from translation unit B.
 */
static const test_ptr_entry_t ptr_b1 PLACE_IN_SECTION("test_ptr_table") = {
    .name = "ptr_b1", .u32 = 0xC1C2C3C4, .u16 = 0xC5C6, .u8 = 0xC7
};
