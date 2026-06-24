/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "esp_attr.h"
#include "test_ptr_entry.h"

/*
 * Test for the portable link-time section macros defined in esp_attr.h.
 *
 * Two separate translation units (test_entries_a.c and test_entries_b.c)
 * place uint32_t values into the "test_data_table" section using
 * PLACE_IN_SECTION().  This main function uses _SECTION_ATTR_SYMBOL_DECL_GENERIC
 * and _SECTION_START/_SECTION_END to iterate the collected entries and verify:
 *   1. The total count matches the expected number (5 entries).
 *   2. Every expected value is present in the section.
 */

_SECTION_ATTR_SYMBOL_DECL_GENERIC(uint32_t, test_data_table)
_SECTION_ATTR_SYMBOL_DECL_GENERIC(test_ptr_entry_t, test_ptr_table)

/* Expected values — order is linker-determined, so we check membership */
static const uint32_t expected_values[] = {
    0xDEADBEEF,
    0xCAFEBABE,
    0x12345678,
    0xAAAAAAAA,
    0x55555555,
};

#define EXPECTED_COUNT (sizeof(expected_values) / sizeof(expected_values[0]))

void app_main(void)
{
    const uint32_t *start = _SECTION_START(test_data_table);
    const uint32_t *end   = _SECTION_END(test_data_table);

    /* --- Check 1: entry count -------------------------------------------- */
    size_t count = (size_t)(end - start);
    printf("Section entry count: %zu (expected %zu)\n", count, (size_t)EXPECTED_COUNT);

    if (count != EXPECTED_COUNT) {
        printf("FAIL: entry count mismatch\n");
        exit(1);
    }

    /* --- Check 2: every expected value is present ------------------------ */
    for (size_t i = 0; i < EXPECTED_COUNT; i++) {
        int found = 0;
        for (size_t j = 0; j < count; j++) {
            if (start[j] == expected_values[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("FAIL: expected value 0x%08X not found in section\n", expected_values[i]);
            exit(1);
        }
    }

    /* --- Check 3: no unexpected values ----------------------------------- */
    for (size_t j = 0; j < count; j++) {
        int known = 0;
        for (size_t i = 0; i < EXPECTED_COUNT; i++) {
            if (start[j] == expected_values[i]) {
                known = 1;
                break;
            }
        }
        if (!known) {
            printf("FAIL: unexpected value 0x%08X found in section\n", start[j]);
            exit(1);
        }
    }

    printf("SUCCESS: All %zu section entries verified.\n", count);

    /* ------------------------------------------------------------------ */
    /* Part 2: pointer-containing struct section                            */
    /* ------------------------------------------------------------------ */

    /* expected entries — all fields checked to ensure no data corruption */
    static const test_ptr_entry_t expected_ptr[] = {
        { .name = "ptr_a1", .u32 = 0xA1A2A3A4, .u16 = 0xA5A6, .u8 = 0xA7 },
        { .name = "ptr_a2", .u32 = 0xB1B2B3B4, .u16 = 0xB5B6, .u8 = 0xB7 },
        { .name = "ptr_b1", .u32 = 0xC1C2C3C4, .u16 = 0xC5C6, .u8 = 0xC7 },
    };
#define EXPECTED_PTR_COUNT (sizeof(expected_ptr) / sizeof(expected_ptr[0]))

    const test_ptr_entry_t *pstart = _SECTION_START(test_ptr_table);
    const test_ptr_entry_t *pend   = _SECTION_END(test_ptr_table);

    size_t pcount = (size_t)(pend - pstart);
    printf("Pointer section entry count: %zu (expected %zu)\n",
           pcount, (size_t)EXPECTED_PTR_COUNT);

    if (pcount != EXPECTED_PTR_COUNT) {
        printf("FAIL: pointer section entry count mismatch\n");
        exit(1);
    }

    /* Check each expected entry is present and pointer is valid */
    for (size_t i = 0; i < EXPECTED_PTR_COUNT; i++) {
        int found = 0;
        for (size_t j = 0; j < pcount; j++) {
            /* Verify the pointer is non-NULL and dereferenceable */
            if (pstart[j].name == NULL) {
                printf("FAIL: entry %zu has NULL name pointer (alignment issue?)\n", j);
                exit(1);
            }
            if (strcmp(pstart[j].name, expected_ptr[i].name) == 0 &&
                    pstart[j].u32 == expected_ptr[i].u32 &&
                    pstart[j].u16 == expected_ptr[i].u16 &&
                    pstart[j].u8  == expected_ptr[i].u8) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("FAIL: expected pointer entry {%s} not found or fields corrupted\n",
                   expected_ptr[i].name);
            exit(1);
        }
    }

    printf("SUCCESS: All %zu pointer section entries verified.\n", pcount);
}
