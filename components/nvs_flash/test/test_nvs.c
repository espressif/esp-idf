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


TEST_CASE("various nvs tests", "[nvs]")
{
    nvs_handle handle_1;
    TEST_ESP_OK(nvs_flash_init());
    TEST_ESP_ERR(nvs_open("test_namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_ERR(nvs_set_i32(handle_1, "foo", 0x12345678), ESP_ERR_NVS_INVALID_HANDLE);
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_open("test_namespace2", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_erase_all(handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle handle_2;
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
    nvs_close(handle_2);
}
