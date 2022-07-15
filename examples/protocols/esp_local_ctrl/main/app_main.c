/* Local Ctrl Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAG = "local_ctrl_example";


/* Function responsible for configuring and starting the esp_local_ctrl service.
 * See local_ctrl_service.c for implementation */
extern void start_esp_local_ctrl_service(void);

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if (example_connect() == ESP_OK) {
        start_esp_local_ctrl_service();
    } else {
        ESP_LOGI(TAG, "Connection failed, not starting esp_local_ctrl service");
    }
}
