#include "unity.h"
#include "esp_netif.h"
#include "esp_wifi.h"

TEST_CASE("esp_netif: init and destroy", "[esp_netif][leaks=0]")
{
    esp_netif_config_t cfg = {};
    esp_netif_t *esp_netif = esp_netif_new(NULL);

    TEST_ASSERT_EQUAL(ESP_ERR_ESP_NETIF_INVALID_PARAMS, esp_netif_configure(esp_netif, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_configure(esp_netif, &cfg));

    esp_netif_destroy(esp_netif);
}


TEST_CASE("esp_netif: get from if_key", "[esp_netif][leaks=0]")
{
    // init default netif
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *esp_netif = esp_netif_new(&cfg);
    TEST_ASSERT_NOT_NULL(esp_netif);

    // check it's accessible by key
    TEST_ASSERT_EQUAL(esp_netif, esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));

    // destroy it
    esp_netif_destroy(esp_netif);

    // check it's also destroyed in list
    TEST_ASSERT_EQUAL(NULL, esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));

}


TEST_CASE("esp_netif: create and delete multiple netifs", "[esp_netif][leaks=0]")
{
    const int nr_of_netifs = 10;
    esp_netif_t *netifs[nr_of_netifs];
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();

    // create 10 wifi stations
    for (int i=0; i<nr_of_netifs; ++i) {
        netifs[i] = esp_netif_new(NULL);
        TEST_ASSERT_EQUAL(ESP_OK, esp_netif_configure(netifs[i] , &cfg));
    }

    // there's no AP within created stations
    TEST_ASSERT_EQUAL(NULL, esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"));

    // destroy
    for (int i=0; i<nr_of_netifs; ++i) {
        esp_netif_destroy(netifs[i]);
    }

}
