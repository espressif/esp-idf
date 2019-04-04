/*
 Tests for the Wi-Fi
*/
#include "unity.h"
#include "esp_log.h"
#include "esp_wifi_internal.h"
#include "esp_coexist_internal.h"

static const char* TAG = "test_header_files_md5";

TEST_CASE("wifi os adapter MD5","[wifi]")
{    
    const char *test_wifi_os_funcs_md5 = WIFI_OS_ADAPTER_MD5;

    ESP_LOGI(TAG, "test wifi os adapter MD5...");
    TEST_ESP_OK(esp_wifi_internal_osi_funcs_md5_check(test_wifi_os_funcs_md5));

    ESP_LOGI(TAG, "test passed...");
}

TEST_CASE("wifi crypto types MD5","[wifi]")
{    
    const char *test_wifi_crypto_funcs_md5 = WIFI_CRYPTO_MD5;

    ESP_LOGI(TAG, "test wifi crypto adapter MD5...");
    TEST_ESP_OK(esp_wifi_internal_crypto_funcs_md5_check(test_wifi_crypto_funcs_md5));

    ESP_LOGI(TAG, "test passed...");
}

TEST_CASE("coexist adapter MD5","[coex]")
{
    const char *test_coex_adapter_funcs_md5 = COEX_ADAPTER_MD5;

    ESP_LOGI(TAG, "test coexist adapter MD5...");
    TEST_ESP_OK(esp_coex_adapter_funcs_md5_check(test_coex_adapter_funcs_md5));

    ESP_LOGI(TAG, "test passed...");
}
