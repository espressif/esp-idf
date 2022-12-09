#include <string.h>
#include "esp_ota_ops.h"
#include "unity.h"

TEST_CASE("esp_ota_get_app_elf_sha256 test", "[esp_app_desc]")
{
    const int sha256_hex_len = CONFIG_APP_RETRIEVE_LEN_ELF_SHA;
    char dst[sha256_hex_len + 2];
    const char fill = 0xcc;
    int res;
    size_t len;

    char ref_sha256[sha256_hex_len + 1];
    const esp_app_desc_t* desc = esp_ota_get_app_description();
    for (int i = 0; i < sizeof(ref_sha256) / 2; ++i) {
        snprintf(ref_sha256 + 2*i, 3, "%02x", desc->app_elf_sha256[i]);
    }
    ref_sha256[sha256_hex_len] = 0;

    printf("Ref: %s\n", ref_sha256);

    memset(dst, fill, sizeof(dst));
    len = sizeof(dst);
    res = esp_ota_get_app_elf_sha256(dst, len);
    printf("%d: %s (%d)\n", len, dst, res);
    TEST_ASSERT_EQUAL(sha256_hex_len + 1, res);
    TEST_ASSERT_EQUAL(0, memcmp(dst, ref_sha256, res - 1));
    TEST_ASSERT_EQUAL_HEX(0, dst[sha256_hex_len]);
    TEST_ASSERT_EQUAL_HEX(fill, dst[sha256_hex_len + 1]);

    memset(dst, fill, sizeof(dst));
    len = 9;
    res = esp_ota_get_app_elf_sha256(dst, len);
    printf("%d: %s (%d)\n", len, dst, res);
    TEST_ASSERT_EQUAL(9, res);
    TEST_ASSERT_EQUAL(0, memcmp(dst, ref_sha256, res - 1));
    TEST_ASSERT_EQUAL_HEX(0, dst[8]);
    TEST_ASSERT_EQUAL_HEX(fill, dst[9]);

    memset(dst, fill, sizeof(dst));
    len = 8;
    res = esp_ota_get_app_elf_sha256(dst, len);
    printf("%d: %s (%d)\n", len, dst, res);
    // should output even number of characters plus '\0'
    TEST_ASSERT_EQUAL(7, res);
    TEST_ASSERT_EQUAL(0, memcmp(dst, ref_sha256, res - 1));
    TEST_ASSERT_EQUAL_HEX(0, dst[6]);
    TEST_ASSERT_EQUAL_HEX(fill, dst[7]);
    TEST_ASSERT_EQUAL_HEX(fill, dst[8]);
}
