/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "unity.h"
#include "esp_log.h"
#include <string.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_utility.h"
#include "esp_efuse_test_table.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "esp_rom_efuse.h"

__attribute__((unused)) static const char* TAG = "efuse_test";


#ifdef CONFIG_EFUSE_VIRTUAL

TEST_CASE("Test a write protection", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();

    esp_efuse_utility_debug_dump_blocks();

    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_set_write_protect(EFUSE_BLK0));
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_set_read_protect(EFUSE_BLK0));

    size_t out_cnt;
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(0, out_cnt);
    TEST_ESP_OK(esp_efuse_set_write_protect(EFUSE_BLK_KEY0));
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);
    TEST_ESP_ERR(ESP_ERR_EFUSE_CNT_IS_FULL, esp_efuse_set_write_protect(EFUSE_BLK_KEY0));

    esp_efuse_utility_debug_dump_blocks();
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}

TEST_CASE("Test a read protection", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();

    esp_efuse_utility_debug_dump_blocks();

    TEST_ESP_OK(esp_efuse_write_field_bit(ESP_EFUSE_XTS_KEY_LENGTH_256));

    size_t out_cnt;
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(0, out_cnt);
    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK_KEY0));
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(2, out_cnt);

    esp_efuse_utility_debug_dump_blocks();
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}

TEST_CASE("Test a key read protection 1", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();

    esp_efuse_utility_debug_dump_blocks();

    size_t out_cnt;
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(0, out_cnt);

    TEST_ASSERT_FALSE(esp_efuse_get_key_dis_read(EFUSE_BLK_KEY0));
    TEST_ESP_OK(esp_efuse_set_key_dis_read(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_get_key_dis_read(EFUSE_BLK_KEY0));

    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    esp_efuse_utility_debug_dump_blocks();
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}

TEST_CASE("Test a key read protection 2", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();

    esp_efuse_utility_debug_dump_blocks();

    TEST_ESP_OK(esp_efuse_write_field_bit(ESP_EFUSE_XTS_KEY_LENGTH_256));

    size_t out_cnt;
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(0, out_cnt);

    TEST_ASSERT_FALSE(esp_efuse_get_key_dis_read(EFUSE_BLK_KEY0));
    TEST_ESP_OK(esp_efuse_set_key_dis_read(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_get_key_dis_read(EFUSE_BLK_KEY0));

    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS, &out_cnt);
    TEST_ASSERT_EQUAL_INT(2, out_cnt);

    esp_efuse_utility_debug_dump_blocks();
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}
#endif // CONFIG_EFUSE_VIRTUAL

#ifdef CONFIG_EFUSE_FPGA_TEST
TEST_CASE("Test a real write (FPGA)2", "[efuse]")
{
    esp_efuse_utility_debug_dump_blocks();
    ESP_LOGI(TAG, "1. Write KEY3");
    uint8_t key[32] = {0};
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY0, &key, 256));
    for (int i = 0; i < sizeof(key); ++i) {
        TEST_ASSERT_EQUAL_INT(0, key[i]);
    }
    uint8_t new_key[32] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                           10, 11, 12, 12, 14, 15, 16, 17, 18, 19,
                           20, 21, 22, 22, 24, 25, 26, 27, 28, 29,
                           30, 31};
    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_KEY0, &new_key, 256));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY0, &key, 256));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(key));
    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "2. Set a read protection for KEY0");
    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK_KEY0));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY0, &key, 256));
#ifndef CONFIG_EFUSE_VIRTUAL
    TEST_ASSERT_EACH_EQUAL_HEX8(0, key, sizeof(key));
#else
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(key));
#endif // CONFIG_EFUSE_VIRTUAL
    esp_efuse_utility_debug_dump_blocks();
}
#endif  // CONFIG_EFUSE_FPGA_TEST
