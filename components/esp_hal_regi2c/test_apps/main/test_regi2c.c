/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include "unity.h"
#include "test_regi2c.h"
#include "esp_private/regi2c_ctrl.h"

TEST_CASE("regi2c basic read/write test", "[regi2c]")
{
#if CONFIG_IDF_TARGET_ESP32
    // For ESP32, we need to enable the APLL clock before accessing the APLL regi2c registers
    periph_rtc_apll_acquire();
#endif

    /* ---- Part 1: full-register read / write ---- */
    printf("Test regi2c read/write: block=0x%02X host=%d reg=%d\n",
           TEST_BLOCK, TEST_HOST_ID, TEST_REG_FULL);

    uint8_t orig_full = regi2c_ctrl_read_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_FULL);
    printf("  Original value: 0x%02X\n", orig_full);

    const uint8_t patterns[] = {0xA5, 0x5A, 0xFF, 0x00};
    for (size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); i++) {
        regi2c_ctrl_write_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_FULL, patterns[i]);
        uint8_t rb = regi2c_ctrl_read_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_FULL);
        printf("  Write 0x%02X -> Read 0x%02X\n", patterns[i], rb);
        TEST_ASSERT_EQUAL_HEX8(patterns[i], rb);
    }

    /* ---- Part 2: masked read / write ---- */
    const uint8_t field_width = TEST_REG_MASK_MSB - TEST_REG_MASK_LSB + 1;
    const uint8_t field_max   = (uint8_t)((1U << field_width) - 1);
    const uint8_t reg_mask    = (uint8_t)(field_max << TEST_REG_MASK_LSB);

    printf("Test regi2c read_mask/write_mask: block=0x%02X host=%d reg=%d bits[%d:%d]\n",
           TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK,
           TEST_REG_MASK_MSB, TEST_REG_MASK_LSB);

    uint8_t orig_mask = regi2c_ctrl_read_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK);
    printf("  Original full reg value: 0x%02X\n", orig_mask);

    /* Write all-ones to masked field */
    regi2c_ctrl_write_reg_mask(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK,
                               TEST_REG_MASK_MSB, TEST_REG_MASK_LSB, field_max);
    uint8_t mval = regi2c_ctrl_read_reg_mask(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK,
                                             TEST_REG_MASK_MSB, TEST_REG_MASK_LSB);
    printf("  write_mask(0x%02X) -> read_mask=0x%02X\n", field_max, mval);
    TEST_ASSERT_EQUAL_HEX8(field_max, mval);
    uint8_t full = regi2c_ctrl_read_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK);
    TEST_ASSERT_EQUAL_HEX8(orig_mask & (uint8_t)(~reg_mask), full & (uint8_t)(~reg_mask));

    /* Write all-zeros to masked field */
    regi2c_ctrl_write_reg_mask(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK,
                               TEST_REG_MASK_MSB, TEST_REG_MASK_LSB, 0x00);
    mval = regi2c_ctrl_read_reg_mask(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK,
                                     TEST_REG_MASK_MSB, TEST_REG_MASK_LSB);
    printf("  write_mask(0x00) -> read_mask=0x%02X\n", mval);
    TEST_ASSERT_EQUAL_HEX8(0x00, mval);
    full = regi2c_ctrl_read_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK);
    TEST_ASSERT_EQUAL_HEX8(orig_mask & (uint8_t)(~reg_mask), full & (uint8_t)(~reg_mask));

    /* Write a mid-range value */
    uint8_t mid = field_max >> 1;
    if (mid == 0 && field_max > 0) {
        mid = 1;
    }
    regi2c_ctrl_write_reg_mask(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK,
                               TEST_REG_MASK_MSB, TEST_REG_MASK_LSB, mid);
    mval = regi2c_ctrl_read_reg_mask(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK,
                                     TEST_REG_MASK_MSB, TEST_REG_MASK_LSB);
    printf("  write_mask(0x%02X) -> read_mask=0x%02X\n", mid, mval);
    TEST_ASSERT_EQUAL_HEX8(mid, mval);

    /* Restore all modified registers to their original values */
    regi2c_ctrl_write_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_FULL, orig_full);
    TEST_ASSERT_EQUAL_HEX8(orig_full,
                           regi2c_ctrl_read_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_FULL));
    regi2c_ctrl_write_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK, orig_mask);
    TEST_ASSERT_EQUAL_HEX8(orig_mask,
                           regi2c_ctrl_read_reg(TEST_BLOCK, TEST_HOST_ID, TEST_REG_MASK));

#if CONFIG_IDF_TARGET_ESP32
    // Disable APLL clock
    periph_rtc_apll_release();
#endif
}
