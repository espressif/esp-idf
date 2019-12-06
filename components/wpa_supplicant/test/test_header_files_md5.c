/*
 Tests for the Wi-Fi
*/
#include "unity.h"
#include "esp_log.h"
#include "utils/common.h"
#include "esp_supplicant/esp_wifi_driver.h"

static const char* TAG = "test_header_files_md5";

TEST_CASE("wifi supplicant header files MD5","[wpa_supplicant]")
{    
    const char *test_wifi_supplicant_header_md5 = WIFI_SUPPLICANT_MD5;

    ESP_LOGI(TAG, "test wifi supplicant MD5...");
    TEST_ESP_OK(esp_wifi_internal_supplicant_header_md5_check(test_wifi_supplicant_header_md5));

    ESP_LOGI(TAG, "test passed...");
}

