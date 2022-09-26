/* Test only application

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "init_macro.h"
#include "esp_wps.h"


static void s_init_wifi_netif(esp_netif_inherent_config_t* esp_netif_config)
{
    esp_netif_config->if_desc = "custom wifi station";
    esp_netif_config->route_prio = 1;
    esp_netif_create_wifi(WIFI_IF_STA, esp_netif_config);
    esp_wifi_set_default_wifi_sta_handlers();
}

static void s_use_test_config_struct(g_netif_test_struct_t* cfg)
{
    printf("%s\n", cfg->char_star);
}

static void test_wifi_init_custom(void)
{

    {
        esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
        s_init_wifi_netif(&esp_netif_config);
    }

    {
        esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_AP();
        s_init_wifi_netif(&esp_netif_config);
    }

    {
        esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
        s_init_wifi_netif(&esp_netif_config);
    }

#ifdef CONFIG_PPP_SUPPORT
    {
        esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_PPP();
        s_init_wifi_netif(&esp_netif_config);
    }
#endif
}

static void test_common_init_field(void)
{
    {
        g_netif_test_struct_t cfg = NETIF_TEST_STRUCT_EMPTY();
        s_use_test_config_struct(&cfg);
    }
    {
        g_netif_test_struct_t cfg = NETIF_TEST_STRUCT_DEFAULT();
        s_use_test_config_struct(&cfg);
    }
}

static void test_wps_init(void)
{
    esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_DISABLE);
    ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
    ESP_ERROR_CHECK(esp_wifi_wps_start(0));
}


void esp_netif_compile_test_c99(void)
{
    test_wifi_init_custom();
    test_common_init_field();
    test_wps_init();
}
