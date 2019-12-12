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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "test_utils.h"
#include "sdkconfig.h"

static const char* TAG = "efuse_test";

static void test_read_blob(void)
{
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    uint8_t mac[6];

    ESP_LOGI(TAG, "1. Read MAC address");
    memset(mac, 0, sizeof(mac));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, sizeof(mac) * 8));
    TEST_ASSERT_EQUAL_INT(sizeof(mac) * 8, esp_efuse_get_field_size(ESP_EFUSE_MAC_FACTORY));
    ESP_LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

#if CONFIG_IDF_TARGET_ESP32
    ESP_LOGI(TAG, "2. Check CRC by MAC");
    uint8_t crc;
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY_CRC, &crc, 8));
    TEST_ASSERT_EQUAL_HEX8(crc, esp_crc8(mac, sizeof(mac)));
#endif // CONFIG_IDF_TARGET_ESP32

    ESP_LOGI(TAG, "3. Test check args");
    uint32_t test_var;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, NULL, 1));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &test_var, 0));

    uint8_t half_byte;
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &half_byte, 4));
    TEST_ASSERT_EQUAL_HEX8(mac[0]&0x0F, half_byte);

    uint8_t buff[7] = {0x59};
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &buff, sizeof(buff) * 8));
    TEST_ASSERT_TRUE_MESSAGE(memcmp(mac, buff, sizeof(mac)) == 0, "Operation read blob is not success");
    TEST_ASSERT_EQUAL_HEX8(0, buff[6]);
}

TEST_CASE("efuse test read_field_blob", "[efuse]")
{
    test_read_blob();
}

static void test_read_cnt(void)
{
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "1. Test check args");
    size_t cnt;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_read_field_cnt(ESP_EFUSE_MAC_FACTORY, NULL));

    ESP_LOGI(TAG, "2. Read MAC address");
    uint8_t mac[6];
    memset(mac, 0, sizeof(mac));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, 48));

    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_MAC_FACTORY, &cnt));
    size_t cnt_summ = 0;
    for (int i = 0; i < sizeof(mac); ++i) {
        cnt_summ += __builtin_popcount(mac[i]);
    }
    TEST_ASSERT_EQUAL_INT(cnt_summ, cnt);
}

TEST_CASE("efuse test read_field_cnt", "[efuse]")
{
    test_read_cnt();
}

// If using efuse is real, then turn off writing tests.
#ifdef CONFIG_EFUSE_VIRTUAL
static void test_write_blob(void)
{
    esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(EFUSE_BLK1);
    esp_efuse_utility_erase_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "1. Test check args");
    uint16_t test1_len_8 = 0x5FAA;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_field_blob(ESP_EFUSE_MAC_FACTORY, &test1_len_8, 0));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_field_blob(ESP_EFUSE_TEST1_LEN_8, NULL, 8));
    TEST_ASSERT_EQUAL_HEX16(0x5FAA, test1_len_8);

    ESP_LOGI(TAG, "2. Test write operation");

    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_TEST1_LEN_8, &test1_len_8, 7));
    TEST_ESP_ERR(ESP_ERR_EFUSE_REPEATED_PROG, esp_efuse_write_field_blob(ESP_EFUSE_TEST1_LEN_8, &test1_len_8, 9));
    uint16_t val_read1 = 0;
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST1_LEN_8, &val_read1, 8));
    TEST_ASSERT_EQUAL_HEX16(test1_len_8&((1 << 7) - 1), val_read1);

    uint16_t test1_len_8_hi = test1_len_8 & ~((1 << 7) - 1);
    if (scheme == EFUSE_CODING_SCHEME_NONE) {
        TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_TEST1_LEN_8, &test1_len_8_hi, 8));
    } else {
        TEST_ESP_ERR(ESP_ERR_CODING, esp_efuse_write_field_blob(ESP_EFUSE_TEST1_LEN_8, &test1_len_8_hi, 8));
    }

    TEST_ESP_ERR(ESP_ERR_EFUSE_REPEATED_PROG, esp_efuse_write_field_blob(ESP_EFUSE_TEST1_LEN_8, &test1_len_8, 8));
    val_read1 = 0;
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST1_LEN_8, &val_read1, 16));
    if (scheme == EFUSE_CODING_SCHEME_NONE) {
        TEST_ASSERT_EQUAL_HEX16(test1_len_8&0x00FF, val_read1);
    } else {
        TEST_ASSERT_EQUAL_HEX16(test1_len_8&0x007F, val_read1);
    }

    if (scheme != EFUSE_CODING_SCHEME_NONE) {
        esp_efuse_utility_erase_virt_blocks();
        ESP_LOGI(TAG, "erase virt blocks");
    }
    uint16_t test2_len_16 = 0xAA55;
    uint32_t val_32 = test2_len_16;
    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_TEST2_LEN_16, &val_32, 17));
    TEST_ESP_ERR(ESP_ERR_EFUSE_REPEATED_PROG, esp_efuse_write_field_blob(ESP_EFUSE_TEST2_LEN_16, &test2_len_16, 16));
    uint16_t test_16 = 0;
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST2_LEN_16, &test_16, 16));
    TEST_ASSERT_EQUAL_HEX16(test2_len_16, test_16);

    ESP_LOGI(TAG, "3. Test field with one bit");
    uint8_t test5_len_1;
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));
    TEST_ASSERT_EQUAL_HEX8(0, test5_len_1);

    test5_len_1 = 0;
    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));
    TEST_ASSERT_EQUAL_HEX8(0, test5_len_1);

    test5_len_1 = 1;
    TEST_ESP_OK(esp_efuse_write_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));
    TEST_ASSERT_EQUAL_HEX8(1, test5_len_1);

    test5_len_1 = 1;
    TEST_ESP_ERR(ESP_ERR_EFUSE_REPEATED_PROG, esp_efuse_write_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));

    esp_efuse_utility_debug_dump_blocks();
}

TEST_CASE("efuse test write_field_blob", "[efuse]")
{
    test_write_blob();
}

static void test_write_cnt(void)
{
    esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(EFUSE_BLK1);
    esp_efuse_utility_erase_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "1. Test check args");
    size_t test3_len_6  = 5;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_field_cnt(ESP_EFUSE_MAC_FACTORY, 0));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_field_cnt(NULL, 5));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_field_cnt(ESP_EFUSE_TEST3_LEN_6, 0));

    ESP_LOGI(TAG, "2. Test write operation");
    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST3_LEN_6, &test3_len_6));
    TEST_ASSERT_EQUAL_INT(0, test3_len_6);

    TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST3_LEN_6, 1));
    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST3_LEN_6, &test3_len_6));
    TEST_ASSERT_EQUAL_INT(1, test3_len_6);

    if (scheme == EFUSE_CODING_SCHEME_NONE) {
        TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST3_LEN_6, 1));
    } else {
        esp_efuse_utility_erase_virt_blocks();
        ESP_LOGI(TAG, "erase virt blocks");
        TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST3_LEN_6, 2));
    }
    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST3_LEN_6, &test3_len_6));
    TEST_ASSERT_EQUAL_INT(2, test3_len_6);

    if (scheme == EFUSE_CODING_SCHEME_NONE) {
        TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST3_LEN_6, 3));
    } else {
        esp_efuse_utility_erase_virt_blocks();
        ESP_LOGI(TAG, "erase virt blocks");
        TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST3_LEN_6, 5));
    }
    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST3_LEN_6, &test3_len_6));
    TEST_ASSERT_EQUAL_INT(5, test3_len_6);

    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "3. Test field is full set");
    int max_bits = esp_efuse_get_field_size(ESP_EFUSE_TEST4_LEN_182);
    size_t test4_len_182;
    esp_efuse_utility_debug_dump_blocks();
    for (int i = 0; i < max_bits / 26; ++i) {
        ESP_LOGD(TAG, "# %d", i);
        if (scheme == EFUSE_CODING_SCHEME_NONE) {
            TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST4_LEN_182, 26));
        } else {
            esp_efuse_utility_erase_virt_blocks();
            TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST4_LEN_182, (i + 1) * 26));
        }
        TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST4_LEN_182, &test4_len_182));
        esp_efuse_utility_debug_dump_blocks();
        TEST_ASSERT_EQUAL_INT((i + 1) * 26, test4_len_182);
    }

    esp_efuse_utility_debug_dump_blocks();

    ESP_LOGI(TAG, "4. Test field ESP_EFUSE_TEST4_LEN_182 is full");
    TEST_ESP_ERR(ESP_ERR_EFUSE_CNT_IS_FULL, esp_efuse_write_field_cnt(ESP_EFUSE_TEST4_LEN_182, 1));

    ESP_LOGI(TAG, "3. Test field with one bit");
    size_t test5_len_1;
    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST5_LEN_1, &test5_len_1));
    TEST_ASSERT_EQUAL_HEX8(0, test5_len_1);
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));
    TEST_ASSERT_EQUAL_HEX8(0, test5_len_1);

    if (scheme != EFUSE_CODING_SCHEME_NONE) {
        esp_efuse_utility_erase_virt_blocks();
        ESP_LOGI(TAG, "erase virt blocks");
    }

    test5_len_1 = 1;
    TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST5_LEN_1, test5_len_1));

    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST5_LEN_1, &test5_len_1));
    TEST_ASSERT_EQUAL_HEX8(1, test5_len_1);
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST5_LEN_1, &test5_len_1, 1));
    TEST_ASSERT_EQUAL_HEX8(1, test5_len_1);

    test5_len_1 = 1;
    TEST_ESP_ERR(ESP_ERR_EFUSE_CNT_IS_FULL, esp_efuse_write_field_cnt(ESP_EFUSE_TEST5_LEN_1, test5_len_1));

    esp_efuse_utility_debug_dump_blocks();
    ESP_LOGI(TAG, "4. Test field test2_len_16");
    size_t test2_len_16 = 11;
    TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST2_LEN_16, test2_len_16));
    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST2_LEN_16, &test2_len_16));
    TEST_ASSERT_EQUAL_HEX16(11, test2_len_16);
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_TEST2_LEN_16, &test2_len_16, 16));
    TEST_ASSERT_EQUAL_HEX16(0x07FF, test2_len_16);

    esp_efuse_utility_debug_dump_blocks();
}

TEST_CASE("efuse test write_field_cnt", "[efuse]")
{
    test_write_cnt();
}

void cut_tail_arr(uint8_t *arr, int num_used_bits, size_t count_bits)
{
    if ((num_used_bits + count_bits) % 8) {
        int start_used_item = (num_used_bits - 1) / 8;
        int last_used_item = ((num_used_bits + count_bits) - 1) / 8;
        int shift = 0;
        int mask = num_used_bits + count_bits;
        if (last_used_item == start_used_item) {
            shift = (num_used_bits) % 8;
            mask = count_bits;
        }
        arr[last_used_item] &= ((1 << (mask % 8)) - 1) << shift;
    }
}

void cut_start_arr(uint8_t *arr, size_t num_used_bits)
{
    if (num_used_bits % 8) {
        int start_used_item = (num_used_bits - 1) / 8;
        arr[start_used_item] &= ~((1 << (num_used_bits % 8)) - 1);
    }
}

void get_part_arr(uint8_t *arr_in, uint8_t *arr_out, int num_used_bits, int count_bits)
{
    int num_items = esp_efuse_utility_get_number_of_items(num_used_bits + count_bits, 8);
    memcpy(arr_out, arr_in, num_items);
    memset(arr_out, 0, num_used_bits / 8);
    cut_start_arr(arr_out, num_used_bits);
    cut_tail_arr(arr_out, num_used_bits, count_bits);
}

void fill_part_arr(uint8_t *arr_in, uint8_t *arr_out, int count_bits)
{
    int num_items = esp_efuse_utility_get_number_of_items(count_bits, 8);
    memcpy(arr_out, arr_in, num_items);
    cut_tail_arr(arr_out, 0, count_bits);
}

// Writes a random array to efuse, then reads and compares it.
void test_blob(const esp_efuse_desc_t* field[], uint8_t *arr_w, uint8_t *arr_r, uint8_t *arr_temp, int arr_size, size_t field_size)
{
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, arr_w, arr_size, ESP_LOG_INFO);
    TEST_ESP_OK(esp_efuse_write_field_blob(field, arr_w, field_size));
    memset(arr_r, 0, arr_size);
    TEST_ESP_OK(esp_efuse_read_field_blob(field, arr_r, field_size));
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, arr_r, arr_size, ESP_LOG_INFO);
    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_TRUE_MESSAGE(memcmp(arr_w, arr_r, arr_size) == 0, "Operation write/read blob is not success");

    int count_once = 0;
    for (int i = 0; i < arr_size; ++i) {
        count_once += __builtin_popcount(arr_w[i]);
    }

    size_t num_bits_r = 0;
    TEST_ESP_OK(esp_efuse_read_field_cnt(field, &num_bits_r));
    TEST_ASSERT_EQUAL_INT(count_once, num_bits_r);

    size_t num_bits_w = field_size - count_once;
    if (num_bits_w == 0) {
        esp_efuse_utility_erase_virt_blocks();
        num_bits_w = field_size;
    }

    TEST_ESP_OK(esp_efuse_write_field_cnt(field, num_bits_w));
    TEST_ESP_OK(esp_efuse_read_field_cnt(field, &num_bits_r));
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL_INT(field_size, num_bits_r);

    memset(arr_r, 0, arr_size);
    TEST_ESP_OK(esp_efuse_read_field_blob(field, arr_r, field_size));
    memset(arr_temp, 0xFF, arr_size);
    cut_tail_arr(arr_temp, 0, field_size);
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_TRUE_MESSAGE(memcmp(arr_temp, arr_r, arr_size) == 0, "Operation write/read blob is not success");
}

// Records a random number of bits (as "1") in the efuse field, then reads and compares.
void test_cnt_part(const esp_efuse_desc_t* field[], uint8_t *arr_r, int arr_size, size_t field_size)
{
    size_t num_bits_r = 0;
    TEST_ESP_OK(esp_efuse_read_field_cnt(field, &num_bits_r));
    TEST_ASSERT_EQUAL_INT(0, num_bits_r);

    TEST_ESP_OK(esp_efuse_write_field_cnt(field, field_size));
    TEST_ESP_OK(esp_efuse_read_field_cnt(field, &num_bits_r));
    TEST_ASSERT_EQUAL_INT(field_size, num_bits_r);

    esp_efuse_utility_erase_virt_blocks();

    int num_bits_summ_r = 0;
    int num_bits_w = 0;
    while(field_size > num_bits_summ_r) {
        num_bits_w = 0;
        while(num_bits_w == 0 || (num_bits_summ_r + num_bits_w) > field_size) {
            bootloader_random_enable();
            bootloader_fill_random(&num_bits_w, 1);
            bootloader_random_disable();
            num_bits_w = num_bits_w * field_size / 255;
            if (num_bits_w != 0 && (num_bits_summ_r + num_bits_w) <= field_size) {
                break;
            }
        }

        TEST_ESP_OK(esp_efuse_write_field_cnt(field, num_bits_w));
        TEST_ESP_OK(esp_efuse_read_field_cnt(field, &num_bits_r));
        num_bits_summ_r += num_bits_w;
        TEST_ASSERT_EQUAL_INT(num_bits_summ_r, num_bits_r);

        memset(arr_r, 0, arr_size);
        TEST_ESP_OK(esp_efuse_read_field_blob(field, arr_r, field_size));
        int count_once = 0;
        for (int i = 0; i < arr_size; ++i) {
            count_once += __builtin_popcount(arr_r[i]);
        }
        TEST_ASSERT_EQUAL_INT(num_bits_summ_r, count_once);

        ESP_LOGI(TAG, "Once bits=%d, step=%d", num_bits_summ_r, num_bits_w);
    }

    esp_efuse_utility_debug_dump_blocks();
}

// From a random array takes a random number of bits and write to efuse, it repeats until the entire length of the field is written down.
void test_blob_part(const esp_efuse_desc_t* field[], uint8_t *arr_w, uint8_t *arr_r, uint8_t *arr_temp, int arr_size, size_t field_size)
{
    esp_efuse_utility_debug_dump_blocks();
    int num_bits_summ_r = 0;
    int num_bits_w = 0;
    memset(arr_w, 0, arr_size);
    bootloader_random_enable();
    bootloader_fill_random(arr_w, arr_size);
    bootloader_random_disable();
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, arr_w, arr_size, ESP_LOG_INFO);
    while(field_size > num_bits_summ_r) {
        num_bits_w = 0;
        while(num_bits_w == 0 || (num_bits_summ_r + num_bits_w) > field_size) {
            bootloader_random_enable();
            bootloader_fill_random(&num_bits_w, 1);
            bootloader_random_disable();
            num_bits_w = num_bits_w * field_size / 255;
            if (num_bits_w != 0 && (num_bits_summ_r + num_bits_w) <= field_size) {
                break;
            }
        }
        ESP_LOGI(TAG, "Summ bits=%d, step=%d", num_bits_summ_r, num_bits_w);
        memset(arr_temp, 0, arr_size);
        get_part_arr(arr_w, arr_temp, num_bits_summ_r, num_bits_w);

        ESP_LOG_BUFFER_HEX_LEVEL(TAG, arr_temp, arr_size, ESP_LOG_INFO);
        TEST_ESP_OK(esp_efuse_write_field_blob(field, arr_temp, field_size));
        memset(arr_r, 0, arr_size);
        TEST_ESP_OK(esp_efuse_read_field_blob(field, arr_r, field_size));
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, arr_r, arr_size, ESP_LOG_INFO);
        esp_efuse_utility_debug_dump_blocks();

        num_bits_summ_r += num_bits_w;
        memset(arr_temp, 0, arr_size);
        fill_part_arr(arr_w, arr_temp, num_bits_summ_r);
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, arr_temp, arr_size, ESP_LOG_INFO);
        TEST_ASSERT_TRUE_MESSAGE(memcmp(arr_temp, arr_r, arr_size) == 0, "Operation write/read blob is not success");
    }
}

void check_efuse_table_test(int cycle)
{
    int num_test = 0;
    while(1) {
        const esp_efuse_desc_t** field;
        switch (num_test++) {
            case 0: field = ESP_EFUSE_TEST1_LEN_8;   break;
            case 1: field = ESP_EFUSE_TEST2_LEN_16;  break;
            case 2: field = ESP_EFUSE_TEST3_LEN_6;   break;
            case 3: field = ESP_EFUSE_TEST4_LEN_182; break;
            case 4: field = ESP_EFUSE_TEST5_LEN_1;   break;
            case 5: field = ESP_EFUSE_TEST6_LEN_17;  break;
            default:
                return;
                break;
        }
        size_t field_size = esp_efuse_get_field_size(field);
        int arr_size = esp_efuse_utility_get_number_of_items(field_size, 8);
        uint8_t *arr_w = (uint8_t *) malloc(arr_size);
        uint8_t *arr_r = (uint8_t *) malloc(arr_size);
        uint8_t *arr_temp = (uint8_t *) malloc(arr_size);
        ESP_LOGI(TAG, "Test#%d", num_test);
        for (int c = 1; c <= cycle; ++c) {
            ESP_LOGI(TAG, "Cycle#%d/%d", c, cycle);

            memset(arr_w, 0, arr_size);
            bootloader_random_enable();
            bootloader_fill_random(arr_w, arr_size);
            bootloader_random_disable();
            cut_tail_arr(arr_w, 0, field_size);

            esp_efuse_utility_erase_virt_blocks();
            ESP_LOGI(TAG, "1) blob write/read");
            test_blob(field, arr_w, arr_r, arr_temp, arr_size, field_size);

            esp_efuse_utility_erase_virt_blocks();
            ESP_LOGI(TAG, "2) cnt part write/read");
            test_cnt_part(field, arr_r, arr_size, field_size);

            esp_efuse_utility_erase_virt_blocks();
            ESP_LOGI(TAG, "3) blob part write/read");
            test_blob_part(field, arr_w, arr_r, arr_temp, arr_size, field_size);
        }
        free(arr_temp);
        free(arr_r);
        free(arr_w);
    }
}

TEST_CASE("efuse esp_efuse_table_test", "[efuse]")
{
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK2);
    if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
        check_efuse_table_test(2);
    } else {
        ESP_LOGI(TAG, "This test is applicable only to the EFUSE_CODING_SCHEME_NONE. Skip this test.");
    }
}


TEST_CASE("Test esp_efuse_read_block esp_efuse_write_block functions", "[efuse]")
{
    int count_useful_reg = 0;
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK2);
    if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
        printf("EFUSE_CODING_SCHEME_NONE\n");
        count_useful_reg = 8;
    }
#if CONFIG_IDF_TARGET_ESP32
    if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
        printf("EFUSE_CODING_SCHEME_3_4\n");
        count_useful_reg = 6;
    } else if (coding_scheme == EFUSE_CODING_SCHEME_REPEAT) {
        printf("EFUSE_CODING_SCHEME_REPEAT\n");
        count_useful_reg = 4;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if (coding_scheme == EFUSE_CODING_SCHEME_RS) {
        printf("EFUSE_CODING_SCHEME_RS\n");
        count_useful_reg = 8;
    }
#endif

    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();

    uint8_t src_key[32] = { 0 };
    uint8_t dst_key[32] = { 0 };
    int offset_in_bits = 0;
    for (int i = 0; i < count_useful_reg * 4; ++i) {
        src_key[i] = 0xAB + i;
    }

    TEST_ESP_OK(esp_efuse_write_block(EFUSE_BLK2, src_key, offset_in_bits, count_useful_reg * 32));
    TEST_ESP_OK(esp_efuse_read_block(EFUSE_BLK2, dst_key, offset_in_bits, count_useful_reg * 32));
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(src_key, dst_key, sizeof(src_key));

    esp_efuse_utility_erase_virt_blocks();

    memset(src_key, 0, sizeof(src_key));
    memset(dst_key, 0, sizeof(dst_key));
    offset_in_bits = count_useful_reg * 32 / 2;
    for (int i = 0; i < count_useful_reg * 4 / 2; ++i) {
        src_key[i] = 0xCD + i;
    }
    TEST_ESP_OK(esp_efuse_write_block(EFUSE_BLK2, src_key, offset_in_bits, count_useful_reg * 32 / 2));
    TEST_ESP_OK(esp_efuse_read_block(EFUSE_BLK2, dst_key, offset_in_bits, count_useful_reg * 32 / 2));
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(src_key, dst_key, count_useful_reg * 4 / 2);

    esp_efuse_utility_erase_virt_blocks();
}

TEST_CASE("Test Bits are not empty. Write operation is forbidden", "[efuse]")
{
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    int count_useful_reg = 0;
    uint8_t r_buff[32];
    int st_offset = -1;
    int num_block;
    for (num_block = EFUSE_BLK1; num_block < 4; ++num_block) {
        memset(r_buff, 0, sizeof(r_buff));
        esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(num_block);
        if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
            printf("EFUSE_CODING_SCHEME_NONE. The test is not applicable.\n");
            count_useful_reg = 8;
            return;
        }
#if CONFIG_IDF_TARGET_ESP32
        if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
            printf("EFUSE_CODING_SCHEME_3_4\n");
            count_useful_reg = 6;
        } else if (coding_scheme == EFUSE_CODING_SCHEME_REPEAT) {
            printf("EFUSE_CODING_SCHEME_REPEAT\n");
            count_useful_reg = 4;
        }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
        if (coding_scheme == EFUSE_CODING_SCHEME_RS) {
            printf("EFUSE_CODING_SCHEME_RS\n");
            if (num_block == EFUSE_BLK1) {
                count_useful_reg = 6;
            } else {
                count_useful_reg = 8;
            }
        }
#endif
        TEST_ESP_OK(esp_efuse_read_block(num_block, r_buff, 0, count_useful_reg * 32));
        for (int i = 0; i < count_useful_reg * 4; ++i) {
            if (r_buff[i] != 0) {
                // found used byte
                for (int j = 0; j < 8; ++j) {
                    if ((r_buff[i] & (1 << j)) == 0) {
                        // found empty bit into this byte
                        st_offset = i * 8 + j;
                        printf("Byte = 0x%02x. offset is = %d\n", r_buff[i], st_offset);
                        break;
                    }
                }
                if (st_offset != -1) {
                    break;
                }
            }
        }
        if (st_offset != -1) {
            break;
        }
    }
    if (st_offset != -1) {
        // write 1 bit to empty place.
        uint8_t val = 1;
        TEST_ESP_ERR(ESP_ERR_CODING, esp_efuse_write_block(num_block, &val, st_offset, 1));
    } else {
        printf("Test skipped. It is not applicable, the device has no written bits.");
    }
}


#ifndef CONFIG_FREERTOS_UNICORE
static const int delay_ms = 2000;
static xSemaphoreHandle sema;

static void task1(void* arg)
{
    TEST_ESP_OK(esp_efuse_batch_write_begin());
    ESP_LOGI(TAG, "Start work in batch mode");
    xSemaphoreGive(sema);
    vTaskDelay((delay_ms + 100) / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Finish work in batch mode");
    TEST_ESP_OK(esp_efuse_batch_write_cancel());

    vTaskDelete(NULL);
}

static void task2(void* arg)
{
    xSemaphoreTake(sema, portMAX_DELAY);
    uint8_t mac[6];
    int64_t t1 = esp_timer_get_time();
    TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, sizeof(mac) * 8));
    int64_t t2 = esp_timer_get_time();
    int diff_ms = (t2 - t1) / 1000;
    TEST_ASSERT_GREATER_THAN(delay_ms, diff_ms);
    ESP_LOGI(TAG, "read MAC address: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    xSemaphoreGive(sema);

    vTaskDelete(NULL);
}

static void task3(void* arg)
{
    xSemaphoreTake(sema, portMAX_DELAY);
    size_t test3_len_6 = 2;
    int64_t t1 = esp_timer_get_time();
    TEST_ESP_OK(esp_efuse_write_field_cnt(ESP_EFUSE_TEST3_LEN_6, test3_len_6));
    TEST_ESP_OK(esp_efuse_read_field_cnt(ESP_EFUSE_TEST3_LEN_6, &test3_len_6));
    int64_t t2 = esp_timer_get_time();
    ESP_LOGI(TAG, "write&read test3_len_6: %d", test3_len_6);
    int diff_ms = (t2 - t1) / 1000;
    TEST_ASSERT_GREATER_THAN(delay_ms, diff_ms);
    TEST_ASSERT_EQUAL_INT(2, test3_len_6);
    xSemaphoreGive(sema);

    vTaskDelete(NULL);
}

TEST_CASE("Batch mode is thread-safe", "[efuse]")
{
    // Batch mode blocks work with efuse on other tasks.
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();
    sema = xSemaphoreCreateBinary();

    printf("\n");
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(task2, "task2", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    xSemaphoreTake(sema, portMAX_DELAY);

    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();

    printf("\n");
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(task3, "task3", 2048, NULL, UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    xSemaphoreTake(sema, portMAX_DELAY);

    printf("\n");
    vSemaphoreDelete(sema);
    esp_efuse_utility_reset();
    esp_efuse_utility_erase_virt_blocks();
}
#endif // #ifndef CONFIG_FREERTOS_UNICORE

#endif // #ifdef CONFIG_EFUSE_VIRTUAL
