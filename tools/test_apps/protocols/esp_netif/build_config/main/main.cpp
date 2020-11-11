/* Test only application

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "build only test";

extern "C" void esp_netif_compile_test_c99();
void esp_netif_compile_test_cpp(void);

extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGE(TAG, "This is app is test only! It is not supposed to be executed!");
    // Calling CPP initialization tests
    esp_netif_compile_test_cpp();
    // Calling C initialization tests
    esp_netif_compile_test_c99();
}
