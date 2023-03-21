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

    test_wp(EFUSE_BLK2, ESP_EFUSE_WR_DIS_BLK2);
    test_wp(EFUSE_BLK3, ESP_EFUSE_WR_DIS_BLK3);

    esp_efuse_utility_debug_dump_blocks();

    test_rp(EFUSE_BLK1, ESP_EFUSE_RD_DIS_BLK1, true);
    test_rp(EFUSE_BLK2, ESP_EFUSE_RD_DIS_BLK2, false);
    test_rp(EFUSE_BLK3, ESP_EFUSE_RD_DIS_BLK3, false);

    esp_efuse_utility_debug_dump_blocks();
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}
#endif // #ifdef CONFIG_EFUSE_VIRTUAL
