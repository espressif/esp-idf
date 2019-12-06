#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"
#include "esp_log.h"
#include <string.h>
#include "esp_system.h"

#ifdef CONFIG_NVS_ENCRYPTION
#include "mbedtls/aes.h"
#endif

static const char* TAG = "test_nvs";

TEST_CASE("various nvs tests", "[nvs]")
{
    nvs_handle_t handle_1;
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "nvs_flash_init failed (0x%x), erasing partition and retrying", err);
        const esp_partition_t* nvs_partition = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        assert(nvs_partition && "partition table must have an NVS partition");
        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, nvs_open("test_namespace1", NVS_READONLY, &handle_1));

    TEST_ESP_ERR(ESP_ERR_NVS_INVALID_HANDLE, nvs_set_i32(handle_1, "foo", 0x12345678));
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_open("test_namespace2", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_erase_all(handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle_t handle_2;
    TEST_ESP_OK(nvs_open("test_namespace3", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_erase_all(handle_2));
    TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
    const char* str = "value 0123456789abcdef0123456789abcdef";
    TEST_ESP_OK(nvs_set_str(handle_2, "key", str));

    int32_t v1;
    TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
    TEST_ASSERT_EQUAL_INT32(0x23456789, v1);

    int32_t v2;
    TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
    TEST_ASSERT_EQUAL_INT32(0x3456789a, v2);

    char buf[strlen(str) + 1];
    size_t buf_len = sizeof(buf);

    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    TEST_ASSERT_EQUAL_INT32(0, strcmp(buf, str));

    nvs_close(handle_1);

    // check that deinit does not leak memory if some handles are still open
    nvs_flash_deinit();

    nvs_close(handle_2);
}

TEST_CASE("calculate used and free space", "[nvs]")
{
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, nvs_get_stats(NULL, NULL));
    nvs_stats_t stat1;
    nvs_stats_t stat2;
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_INITIALIZED, nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries == 0);
    TEST_ASSERT_TRUE(stat1.namespace_count == 0);
    TEST_ASSERT_TRUE(stat1.total_entries == 0);
    TEST_ASSERT_TRUE(stat1.used_entries == 0);

    nvs_handle_t handle = 0;
    size_t h_count_entries;
    TEST_ESP_ERR(ESP_ERR_NVS_INVALID_HANDLE, nvs_get_used_entry_count(handle, &h_count_entries));
    TEST_ASSERT_TRUE(h_count_entries == 0);

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "nvs_flash_init failed (0x%x), erasing partition and retrying", err);
        const esp_partition_t* nvs_partition = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        assert(nvs_partition && "partition table must have an NVS partition");
        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // erase if have any namespace
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    if(stat1.namespace_count != 0) {
        TEST_ESP_OK(nvs_flash_erase());
        TEST_ESP_OK(nvs_flash_deinit());
        TEST_ESP_OK(nvs_flash_init());
    }

    // after erase. empty partition
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries != 0);
    TEST_ASSERT_TRUE(stat1.namespace_count == 0);
    TEST_ASSERT_TRUE(stat1.total_entries != 0);
    TEST_ASSERT_TRUE(stat1.used_entries == 0);

    // create namespace test_k1
    nvs_handle_t handle_1;
    TEST_ESP_OK(nvs_open("test_k1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries + 1 == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 1);
    TEST_ASSERT_TRUE(stat2.total_entries == stat1.total_entries);
    TEST_ASSERT_TRUE(stat2.used_entries == 1);

    // create pair key-value com
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x12345678));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 1 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 1);
    TEST_ASSERT_TRUE(stat1.total_entries == stat2.total_entries);
    TEST_ASSERT_TRUE(stat1.used_entries == 2);

    // change value in com
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x01234567));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 1);
    TEST_ASSERT_TRUE(stat2.total_entries != 0);
    TEST_ASSERT_TRUE(stat2.used_entries == 2);

    // create pair key-value ru
    TEST_ESP_OK(nvs_set_i32(handle_1, "ru", 0x00FF00FF));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 1 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 1);
    TEST_ASSERT_TRUE(stat1.total_entries != 0);
    TEST_ASSERT_TRUE(stat1.used_entries == 3);

    // amount valid pair in namespace 1
    size_t h1_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_1, &h1_count_entries));
    TEST_ASSERT_TRUE(h1_count_entries == 2);

    nvs_handle_t handle_2;
    // create namespace test_k2
    TEST_ESP_OK(nvs_open("test_k2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries + 1 == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 2);
    TEST_ASSERT_TRUE(stat2.total_entries == stat1.total_entries);
    TEST_ASSERT_TRUE(stat2.used_entries == 4);

    // create pair key-value
    TEST_ESP_OK(nvs_set_i32(handle_2, "su1", 0x00000001));
    TEST_ESP_OK(nvs_set_i32(handle_2, "su2", 0x00000002));
    TEST_ESP_OK(nvs_set_i32(handle_2, "sus", 0x00000003));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 3 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 2);
    TEST_ASSERT_TRUE(stat1.total_entries == stat2.total_entries);
    TEST_ASSERT_TRUE(stat1.used_entries == 7);

    TEST_ASSERT_TRUE(stat1.total_entries == (stat1.used_entries + stat1.free_entries));

    // amount valid pair in namespace 2
    size_t h2_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_2, &h2_count_entries));
    TEST_ASSERT_TRUE(h2_count_entries == 3);

    TEST_ASSERT_TRUE(stat1.used_entries == (h1_count_entries + h2_count_entries + stat1.namespace_count));

    nvs_close(handle_1);
    nvs_close(handle_2);

    size_t temp = h2_count_entries;
    TEST_ESP_ERR(ESP_ERR_NVS_INVALID_HANDLE, nvs_get_used_entry_count(handle_1, &h2_count_entries));
    TEST_ASSERT_TRUE(h2_count_entries == 0);
    h2_count_entries = temp;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, nvs_get_used_entry_count(handle_1, NULL));

    nvs_handle_t handle_3;
    // create namespace test_k3
    TEST_ESP_OK(nvs_open("test_k3", NVS_READWRITE, &handle_3));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries + 1 == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 3);
    TEST_ASSERT_TRUE(stat2.total_entries == stat1.total_entries);
    TEST_ASSERT_TRUE(stat2.used_entries == 8);

    // create pair blobs
    uint32_t blob[12];
    TEST_ESP_OK(nvs_set_blob(handle_3, "bl1", &blob, sizeof(blob)));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 4 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 3);
    TEST_ASSERT_TRUE(stat1.total_entries == stat2.total_entries);
    TEST_ASSERT_TRUE(stat1.used_entries == 12);

    // amount valid pair in namespace 2
    size_t h3_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_3, &h3_count_entries));
    TEST_ASSERT_TRUE(h3_count_entries == 4);

    TEST_ASSERT_TRUE(stat1.used_entries == (h1_count_entries + h2_count_entries + h3_count_entries + stat1.namespace_count));

    nvs_close(handle_3);

    TEST_ESP_OK(nvs_flash_erase());
    TEST_ESP_OK(nvs_flash_deinit());
}

TEST_CASE("check for memory leaks in nvs_set_blob", "[nvs]")
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    TEST_ESP_OK( err );

    for (int i = 0; i < 500; ++i) {
        nvs_handle_t my_handle;
        uint8_t key[20] = {0};

        TEST_ESP_OK( nvs_open("leak_check_ns", NVS_READWRITE, &my_handle) );
        TEST_ESP_OK( nvs_set_blob(my_handle, "key", key, sizeof(key)) );
        TEST_ESP_OK( nvs_commit(my_handle) );
        nvs_close(my_handle);
        printf("%d\n", esp_get_free_heap_size());
    }

    nvs_flash_deinit();
    printf("%d\n", esp_get_free_heap_size());
    /* heap leaks will be checked in unity_platform.c */
}

#ifdef CONFIG_NVS_ENCRYPTION
TEST_CASE("check underlying xts code for 32-byte size sector encryption", "[nvs]")
{
    uint8_t eky_hex[2 * NVS_KEY_SIZE] = { 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
        0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
        0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
        0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
        /* Tweak key below*/
        0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
        0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
        0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
        0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22 };

    uint8_t ba_hex[16] = { 0x33,0x33,0x33,0x33,0x33,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

    uint8_t ptxt_hex[32] = { 0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
        0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
        0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
        0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44 };


    uint8_t ctxt_hex[32] = { 0xe6,0x22,0x33,0x4f,0x18,0x4b,0xbc,0xe1,
        0x29,0xa2,0x5b,0x2a,0xc7,0x6b,0x3d,0x92,
        0xab,0xf9,0x8e,0x22,0xdf,0x5b,0xdd,0x15,
        0xaf,0x47,0x1f,0x3d,0xb8,0x94,0x6a,0x85 };

    mbedtls_aes_xts_context ectx[1];
    mbedtls_aes_xts_context dctx[1];

    mbedtls_aes_xts_init(ectx);
    mbedtls_aes_xts_init(dctx);

    TEST_ASSERT_TRUE(!mbedtls_aes_xts_setkey_enc(ectx, eky_hex, 2 * NVS_KEY_SIZE * 8));
    TEST_ASSERT_TRUE(!mbedtls_aes_xts_setkey_enc(dctx, eky_hex, 2 * NVS_KEY_SIZE * 8));

    TEST_ASSERT_TRUE(!mbedtls_aes_crypt_xts(ectx, MBEDTLS_AES_ENCRYPT, 32, ba_hex, ptxt_hex, ptxt_hex));

    TEST_ASSERT_TRUE(!memcmp(ptxt_hex, ctxt_hex, 32));
}

TEST_CASE("Check nvs key partition APIs (read and generate keys)", "[nvs]")
{
    nvs_sec_cfg_t cfg, cfg2;

    const esp_partition_t* key_part = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);

    if (!esp_flash_encryption_enabled()) {
        TEST_IGNORE_MESSAGE("flash encryption disabled, skipping nvs_key partition related tests");
    }

    TEST_ESP_OK(esp_partition_erase_range(key_part, 0, key_part->size));
    TEST_ESP_ERR(ESP_ERR_NVS_KEYS_NOT_INITIALIZED, nvs_flash_read_security_cfg(key_part, &cfg));

    TEST_ESP_OK(nvs_flash_generate_keys(key_part, &cfg));

    TEST_ESP_OK(nvs_flash_read_security_cfg(key_part, &cfg2));

    TEST_ASSERT_TRUE(!memcmp(&cfg, &cfg2, sizeof(nvs_sec_cfg_t)));
}


TEST_CASE("test nvs apis with encryption enabled", "[nvs]")
{
    if (!esp_flash_encryption_enabled()) {
        TEST_IGNORE_MESSAGE("flash encryption disabled, skipping nvs_api tests with encryption enabled");
    }
    const esp_partition_t* key_part = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);

    assert(key_part && "partition table must have an NVS Key partition");

    const esp_partition_t* nvs_partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    assert(nvs_partition && "partition table must have an NVS partition");

    ESP_ERROR_CHECK( esp_partition_erase_range(key_part, 0, key_part->size) );

    bool done = false;

    do {
        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );

        nvs_sec_cfg_t cfg;
        esp_err_t err = nvs_flash_read_security_cfg(key_part, &cfg);

        if(err == ESP_ERR_NVS_KEYS_NOT_INITIALIZED) {
            uint8_t value[4096] = {[0 ... 4095] = 0xff};
            TEST_ESP_OK(esp_partition_write(key_part, 0, value, sizeof(value)));

            TEST_ESP_ERR(ESP_ERR_NVS_KEYS_NOT_INITIALIZED, nvs_flash_read_security_cfg(key_part, &cfg));

            TEST_ESP_OK(nvs_flash_generate_keys(key_part, &cfg));
        } else {
            /* Second time key_partition exists already*/
            ESP_ERROR_CHECK(err);
            done = true;
        }
        TEST_ESP_OK(nvs_flash_secure_init(&cfg));

        nvs_handle_t handle_1;

        TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, nvs_open("namespace1", NVS_READONLY, &handle_1));


        TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));

        TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
        TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

        nvs_handle_t handle_2;
        TEST_ESP_OK(nvs_open("namespace2", NVS_READWRITE, &handle_2));
        TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
        const char* str = "value 0123456789abcdef0123456789abcdef";
        TEST_ESP_OK(nvs_set_str(handle_2, "key", str));

        int32_t v1;
        TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
        TEST_ASSERT_TRUE(0x23456789 == v1);

        int32_t v2;
        TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
        TEST_ASSERT_TRUE(0x3456789a == v2);

        char buf[strlen(str) + 1];
        size_t buf_len = sizeof(buf);

        size_t buf_len_needed;
        TEST_ESP_OK(nvs_get_str(handle_2, "key", NULL, &buf_len_needed));
        TEST_ASSERT_TRUE(buf_len_needed == buf_len);

        size_t buf_len_short = buf_len - 1;
        TEST_ESP_ERR(ESP_ERR_NVS_INVALID_LENGTH, nvs_get_str(handle_2, "key", buf, &buf_len_short));
        TEST_ASSERT_TRUE(buf_len_short == buf_len);

        size_t buf_len_long = buf_len + 1;
        TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len_long));
        TEST_ASSERT_TRUE(buf_len_long == buf_len);

        TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

        TEST_ASSERT_TRUE(0 == strcmp(buf, str));

        nvs_close(handle_1);
        nvs_close(handle_2);

        TEST_ESP_OK(nvs_flash_deinit());
    } while(!done);
}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled", "[nvs_part_gen]")
{

    if (!esp_flash_encryption_enabled()) {
        TEST_IGNORE_MESSAGE("flash encryption disabled, skipping nvs_api tests with encryption enabled");
    }

    nvs_handle_t handle;
    nvs_sec_cfg_t xts_cfg;

    extern const char nvs_key_start[] asm("_binary_encryption_keys_bin_start");
    extern const char nvs_key_end[]   asm("_binary_encryption_keys_bin_end");

    extern const char nvs_data_start[] asm("_binary_partition_encrypted_bin_start");

    extern const char sample_bin_start[] asm("_binary_sample_bin_start");

    const esp_partition_t* key_part = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);

    const esp_partition_t* nvs_part = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);

    assert(key_part && "partition table must have a KEY partition");
    TEST_ASSERT_TRUE((nvs_key_end - nvs_key_start - 1) == SPI_FLASH_SEC_SIZE);

    assert(nvs_part && "partition table must have an NVS partition");
    printf("\n nvs_part size:%d\n", nvs_part->size);

    ESP_ERROR_CHECK(esp_partition_erase_range(key_part, 0, key_part->size));
    ESP_ERROR_CHECK( esp_partition_erase_range(nvs_part, 0, nvs_part->size) );

    for (int i = 0; i < key_part->size; i+= SPI_FLASH_SEC_SIZE) {
        ESP_ERROR_CHECK( esp_partition_write(key_part, i, nvs_key_start + i, SPI_FLASH_SEC_SIZE) );
    }

    for (int i = 0; i < nvs_part->size; i+= SPI_FLASH_SEC_SIZE) {
        ESP_ERROR_CHECK( spi_flash_write(nvs_part->address + i, nvs_data_start + i, SPI_FLASH_SEC_SIZE) );
    }

    esp_err_t err = nvs_flash_read_security_cfg(key_part, &xts_cfg);
    ESP_ERROR_CHECK(err);

    TEST_ESP_OK(nvs_flash_secure_init(&xts_cfg));

    TEST_ESP_OK(nvs_open("dummyNamespace", NVS_READONLY, &handle));
    uint8_t u8v;
    TEST_ESP_OK( nvs_get_u8(handle, "dummyU8Key", &u8v));
    TEST_ASSERT_TRUE(u8v == 127);
    int8_t i8v;
    TEST_ESP_OK( nvs_get_i8(handle, "dummyI8Key", &i8v));
    TEST_ASSERT_TRUE(i8v == -128);
    uint16_t u16v;
    TEST_ESP_OK( nvs_get_u16(handle, "dummyU16Key", &u16v));
    TEST_ASSERT_TRUE(u16v == 32768);
    uint32_t u32v;
    TEST_ESP_OK( nvs_get_u32(handle, "dummyU32Key", &u32v));
    TEST_ASSERT_TRUE(u32v == 4294967295);
    int32_t i32v;
    TEST_ESP_OK( nvs_get_i32(handle, "dummyI32Key", &i32v));
    TEST_ASSERT_TRUE(i32v == -2147483648);

    char buf[64] = {0};
    size_t buflen = 64;
    TEST_ESP_OK( nvs_get_str(handle, "dummyStringKey", buf, &buflen));
    TEST_ASSERT_TRUE(strncmp(buf, "0A:0B:0C:0D:0E:0F", buflen) == 0);

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    buflen = 64;
    TEST_ESP_OK( nvs_get_blob(handle, "dummyHex2BinKey", buf, &buflen));
    TEST_ASSERT_TRUE(memcmp(buf, hexdata, buflen) == 0);

    uint8_t base64data[] = {'1', '2', '3', 'a', 'b', 'c'};
    buflen = 64;
    TEST_ESP_OK( nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    TEST_ASSERT_TRUE(memcmp(buf, base64data, buflen) == 0);

    uint8_t hexfiledata[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    buflen = 64;
    TEST_ESP_OK( nvs_get_blob(handle, "hexFileKey", buf, &buflen));
    TEST_ASSERT_TRUE(memcmp(buf, hexfiledata, buflen) == 0);

    uint8_t base64filedata[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xab, 0xcd, 0xef};
    buflen = 64;
    TEST_ESP_OK( nvs_get_blob(handle, "base64FileKey", buf, &buflen));
    TEST_ASSERT_TRUE(memcmp(buf, base64filedata, buflen) == 0);

    uint8_t strfiledata[64] = "abcdefghijklmnopqrstuvwxyz\0";
    buflen = 64;
    TEST_ESP_OK( nvs_get_str(handle, "stringFileKey", buf, &buflen));
    TEST_ASSERT_TRUE(memcmp(buf, strfiledata, buflen) == 0);

    char bin_data[5120];
    size_t bin_len = sizeof(bin_data);
    TEST_ESP_OK( nvs_get_blob(handle, "binFileKey", bin_data, &bin_len));
    TEST_ASSERT_TRUE(memcmp(bin_data, sample_bin_start, bin_len) == 0);

    nvs_close(handle);
    TEST_ESP_OK(nvs_flash_deinit());

}
#endif
