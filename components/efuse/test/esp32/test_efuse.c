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
#include "esp32/rom/efuse.h"
#include "bootloader_random.h"
#include "sdkconfig.h"


#ifdef CONFIG_EFUSE_VIRTUAL
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

    TEST_ESP_OK(esp_efuse_set_write_protect(EFUSE_BLK2));
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS_BLK2, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    TEST_ESP_OK(esp_efuse_set_write_protect(EFUSE_BLK3));
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS_BLK3, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    esp_efuse_utility_debug_dump_blocks();

    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_BLK1, &out_cnt);
    TEST_ASSERT_EQUAL_INT(0, out_cnt);
    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK1));
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_BLK1, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);
    TEST_ESP_ERR(ESP_ERR_EFUSE_CNT_IS_FULL, esp_efuse_set_read_protect(EFUSE_BLK1));

    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK2));
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_BLK2, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK3));
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_BLK3, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    esp_efuse_utility_debug_dump_blocks();
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}

#endif // #ifdef CONFIG_EFUSE_VIRTUAL
