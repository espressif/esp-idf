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

static void test_wp(esp_efuse_block_t blk, const esp_efuse_desc_t* field[])
{
    size_t out_cnt;
    TEST_ESP_OK(esp_efuse_set_write_protect(blk));
    esp_efuse_read_field_cnt(field, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);
}

static void test_rp(esp_efuse_block_t blk, const esp_efuse_desc_t* field[], bool read_first)
{
    size_t out_cnt;
    if (read_first) {
        esp_efuse_read_field_cnt(field, &out_cnt);
        TEST_ASSERT_EQUAL_INT(0, out_cnt);
    }
    TEST_ESP_OK(esp_efuse_set_read_protect(blk));
    esp_efuse_read_field_cnt(field, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);
    if (read_first) {
        TEST_ESP_ERR(ESP_ERR_EFUSE_CNT_IS_FULL, esp_efuse_set_read_protect(blk));
    }
}

TEST_CASE("Test a write/read protection", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();

    esp_efuse_utility_debug_dump_blocks();

    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_set_write_protect(EFUSE_BLK0));
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_set_read_protect(EFUSE_BLK0));

    size_t out_cnt;
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS_BLK1, &out_cnt);
    TEST_ASSERT_EQUAL_INT(0, out_cnt);
    TEST_ESP_OK(esp_efuse_set_write_protect(EFUSE_BLK1));
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS_BLK1, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);
    TEST_ESP_ERR(ESP_ERR_EFUSE_CNT_IS_FULL, esp_efuse_set_write_protect(EFUSE_BLK1));

    test_wp(EFUSE_BLK2, ESP_EFUSE_WR_DIS_SYS_DATA_PART1);
    test_wp(EFUSE_BLK3, ESP_EFUSE_WR_DIS_USER_DATA);

    esp_efuse_utility_debug_dump_blocks();

    test_rp(EFUSE_BLK4, ESP_EFUSE_RD_DIS_KEY0, true);
    test_rp(EFUSE_BLK5, ESP_EFUSE_RD_DIS_KEY1, false);
    test_rp(EFUSE_BLK6, ESP_EFUSE_RD_DIS_KEY2, false);

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
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY3, &key, 256));
    for (int i = 0; i < sizeof(key); ++i) {
        TEST_ASSERT_EQUAL_INT(0, key[i]);
    }
    uint8_t new_key[32] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                           10, 11, 12, 12, 14, 15, 16, 17, 18, 19,
                           20, 21, 22, 22, 24, 25, 26, 27, 28, 29,
                           30, 31};
    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_KEY3, &new_key, 256));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY3, &key, 256));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(key));
    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "2. Set a read protection for KEY3");
    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK_KEY3));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY3, &key, 256));
#ifndef CONFIG_EFUSE_VIRTUAL
    TEST_ASSERT_EACH_EQUAL_HEX8(0, key, sizeof(key));
#else
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(key));
#endif // CONFIG_EFUSE_VIRTUAL
    esp_efuse_utility_debug_dump_blocks();
}
#endif  // CONFIG_EFUSE_FPGA_TEST

#if CONFIG_EFUSE_FPGA_TEST || CONFIG_EFUSE_VIRTUAL
TEST_CASE("Test writing order is BLK_MAX->BLK0", "[efuse]")
{
    uint8_t new_key[32] = {33,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                           10, 11, 12, 12, 14, 15, 16, 17, 18, 19,
                           20, 21, 22, 22, 24, 25, 26, 27, 28, 29,
                           30, 31};
    esp_efuse_utility_erase_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    TEST_ESP_OK(esp_efuse_batch_write_begin());

    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_KEY4, &new_key, 256));
    // If the order of writing blocks is wrong (ex. BLK0 -> BLK_MAX)
    // then the write protection bit will be set early and the key was left un-updated.
    TEST_ESP_OK(esp_efuse_set_write_protect(EFUSE_BLK_KEY4));

    TEST_ESP_OK(esp_efuse_batch_write_commit());
    esp_efuse_utility_debug_dump_blocks();

    uint8_t key[32] = { 0xEE };
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY4, &key, 256));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(key));
}

TEST_CASE("Test reading inside of batch mode in a nested way", "[efuse]")
{
    uint8_t new_key[32] = {44,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                           10, 11, 12, 12, 14, 15, 16, 17, 18, 19,
                           20, 21, 22, 22, 24, 25, 26, 27, 28, 29,
                           30, 31};
    uint8_t key[32] = { 0xEE };
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY5, &key, 256));
    TEST_ASSERT_EACH_EQUAL_HEX8(0, key, sizeof(key));

    TEST_ESP_OK(esp_efuse_batch_write_begin());
    TEST_ESP_OK(esp_efuse_batch_write_begin());
    TEST_ESP_OK(esp_efuse_batch_write_begin());
    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_KEY5, &new_key, 256));
    TEST_ESP_OK(esp_efuse_set_write_protect(EFUSE_BLK_KEY5));
    ESP_LOGI(TAG, "Reading inside Batch mode, the key was not burn yet and it is empty");
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY5, &key, 256));
    TEST_ASSERT_EACH_EQUAL_HEX8(0, key, sizeof(key));
    TEST_ESP_OK(esp_efuse_batch_write_commit());
    TEST_ESP_OK(esp_efuse_batch_write_commit());
    TEST_ESP_OK(esp_efuse_batch_write_commit());

    TEST_ESP_OK(esp_efuse_batch_write_begin());
    TEST_ESP_OK(esp_efuse_batch_write_begin());
    TEST_ESP_OK(esp_efuse_batch_write_begin());
    TEST_ESP_OK(esp_efuse_batch_write_begin());
    ESP_LOGI(TAG, "Reading inside Batch mode, the key is already set");
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY5, &key, 256));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(key));
    TEST_ESP_OK(esp_efuse_batch_write_commit());
    TEST_ESP_OK(esp_efuse_batch_write_commit());
    TEST_ESP_OK(esp_efuse_batch_write_commit());
    TEST_ESP_OK(esp_efuse_batch_write_commit());

    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "Reading inside Batch mode, the key is already set");
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_efuse_batch_write_commit());
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_efuse_batch_write_cancel());
    TEST_ESP_OK(esp_efuse_batch_write_begin());
    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_KEY2, &new_key, 256));
    TEST_ESP_OK(esp_efuse_batch_write_commit());
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY2, &key, 256));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(key));

    esp_efuse_utility_debug_dump_blocks();
}
#endif  // CONFIG_EFUSE_FPGA_TEST || CONFIG_EFUSE_VIRTUAL
