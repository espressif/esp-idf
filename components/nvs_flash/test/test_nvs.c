#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_spi_flash.h"
#include <string.h>

#define NVS_FLASH_SECTOR 6
#define NVS_FLASH_SECTOR_COUNT_MIN 3
#define NVS_FLASH_SECTOR_COUNT_MAX 10

TEST_CASE("various nvs tests", "[nvs]")
{
    nvs_handle handle_1;
    TEST_ESP_ERR(nvs_open("namespace1", NVS_READWRITE, &handle_1), ESP_ERR_NVS_NOT_INITIALIZED);
    for (uint16_t i = NVS_FLASH_SECTOR; i < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        spi_flash_erase_sector(i);
    }
    TEST_ESP_OK(nvs_flash_init_custom(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

    // TEST_ESP_ERR(nvs_set_i32(handle_1, "foo", 0x12345678), ESP_ERR_NVS_READ_ONLY);
    // nvs_close(handle_1);

    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle handle_2;
    TEST_ESP_OK(nvs_open("namespace2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
    const char *str = "value 0123456789abcdef0123456789abcdef";
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
}
