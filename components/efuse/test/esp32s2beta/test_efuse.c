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


//#define MANUAL_FPGA_TEST

#if defined(MANUAL_FPGA_TEST) && defined(CONFIG_IDF_TARGET_ESP32S2BETA) && !defined(CONFIG_EFUSE_VIRTUAL)
TEST_CASE("Test a real write (FPGA)", "[efuse]")
{
    ESP_LOGI(TAG, "1. Write MAC address");
    esp_efuse_utility_debug_dump_blocks();
    uint8_t mac[6];
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, sizeof(mac) * 8));
    ESP_LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    uint8_t new_mac[6];
    if (mac[0] == 0) {
        new_mac[0] = 0x71;
        new_mac[1] = 0x62;
        new_mac[2] = 0x53;
        new_mac[3] = 0x44;
        new_mac[4] = 0x35;
        new_mac[5] = 0x26;
        TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_MAC_FACTORY, &new_mac, sizeof(new_mac) * 8));
        ESP_LOGI(TAG, "new MAC: %02x:%02x:%02x:%02x:%02x:%02x", new_mac[0], new_mac[1], new_mac[2], new_mac[3], new_mac[4], new_mac[5]);
        TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, sizeof(mac) * 8));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(new_mac, mac, sizeof(new_mac));
        esp_efuse_utility_debug_dump_blocks();
    }

    ESP_LOGI(TAG, "2. Write KEY3");
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
    TEST_ASSERT_EQUAL_HEX8_ARRAY(new_key, key, sizeof(new_mac));
    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "3. Set a read protection for KEY3");
    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK7));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY3, &key, 256));
    for (int i = 0; i < sizeof(key); ++i) {
        TEST_ASSERT_EQUAL_INT(0, key[i]);
    }
    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "4. Write SECURE_VERSION");
    int max_bits = esp_efuse_get_field_size(ESP_EFUSE_SECURE_VERSION);
    size_t read_sec_version;
    esp_efuse_utility_debug_dump_blocks();
    for (int i = 0; i < max_bits; ++i) {
        ESP_LOGI(TAG, "# %d", i);
        TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_SECURE_VERSION, 1));
        TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_SECURE_VERSION, &read_sec_version));
        esp_efuse_utility_debug_dump_blocks();
        TEST_ASSERT_EQUAL_INT(i + 1, read_sec_version);
    }
}

#endif

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
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS_SYS_DATA_PART1, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    TEST_ESP_OK(esp_efuse_set_write_protect(EFUSE_BLK3));
    esp_efuse_read_field_cnt(ESP_EFUSE_WR_DIS_USER_DATA, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    esp_efuse_utility_debug_dump_blocks();

    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_KEY0, &out_cnt);
    TEST_ASSERT_EQUAL_INT(0, out_cnt);
    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK4));
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_KEY0, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);
    TEST_ESP_ERR(ESP_ERR_EFUSE_CNT_IS_FULL, esp_efuse_set_read_protect(EFUSE_BLK4));

    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK5));
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_KEY1, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    TEST_ESP_OK(esp_efuse_set_read_protect(EFUSE_BLK6));
    esp_efuse_read_field_cnt(ESP_EFUSE_RD_DIS_KEY2, &out_cnt);
    TEST_ASSERT_EQUAL_INT(1, out_cnt);

    esp_efuse_utility_debug_dump_blocks();
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}

#endif // #ifdef CONFIG_EFUSE_VIRTUAL
