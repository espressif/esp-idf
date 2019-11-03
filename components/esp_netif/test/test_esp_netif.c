#include "unity.h"
#include "test_utils.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

TEST_CASE("esp_netif: init and destroy", "[esp_netif]")
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *esp_netif = esp_netif_new(NULL);

    TEST_ASSERT_EQUAL(NULL, esp_netif);
    esp_netif = esp_netif_new(&cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, esp_netif);

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
    // interface key has to be a unique identifier
    const char* if_keys[] = { "if1", "if2", "if3", "if4", "if5", "if6", "if7", "if8", "if9" };
    const int nr_of_netifs = sizeof(if_keys)/sizeof(char*);
    esp_netif_t *netifs[nr_of_netifs];

    // create 10 wifi stations
    for (int i=0; i<nr_of_netifs; ++i) {
        esp_netif_inherent_config_t base_netif_config = { .if_key = if_keys[i]};
        esp_netif_config_t cfg = { .base = &base_netif_config, .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA };
        netifs[i] = esp_netif_new(&cfg);
        TEST_ASSERT_NOT_NULL(netifs[i]);
    }

    // there's no AP within created stations
    TEST_ASSERT_EQUAL(NULL, esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"));

    // destroy
    for (int i=0; i<nr_of_netifs; ++i) {
        esp_netif_destroy(netifs[i]);
    }

}

TEST_CASE("esp_netif: test dhcp client state transitions for wifi station", "[esp_netif]")
{
    // init default wifi netif
    test_case_uses_tcpip();
    TEST_ESP_OK(nvs_flash_init());
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *sta = esp_netif_new(&cfg);
    TEST_ASSERT_NOT_NULL(sta);
    esp_netif_attach_wifi_station(sta);
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_wifi_init(&wifi_cfg));

    esp_netif_dhcp_status_t state;

    // testing DHCP states per netif state transitions
    esp_netif_action_start(sta, NULL, 0, NULL);
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_get_status(sta, &state));

    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_INIT, state);
    esp_netif_action_connected(sta, NULL, 0, NULL);
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_get_status(sta, &state));

    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    esp_netif_action_stop(sta, NULL, 0, NULL);
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_get_status(sta, &state));

    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_INIT, state);

    // destroy default wifi netif
    esp_netif_destroy(sta);
    TEST_ASSERT(esp_wifi_stop() == ESP_OK);
    TEST_ASSERT(esp_wifi_deinit() == ESP_OK);
    nvs_flash_deinit();
}

TEST_CASE("esp_netif: test dhcp server state transitions for wifi soft AP", "[esp_netif]")
{
    // init default wifi netif
    test_case_uses_tcpip();
    TEST_ESP_OK(nvs_flash_init());
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_AP();
    esp_netif_t *ap = esp_netif_new(&cfg);
    TEST_ASSERT_NOT_NULL(ap);
    esp_netif_attach_wifi_station(ap);
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_wifi_init(&wifi_cfg));

    esp_netif_dhcp_status_t state;

    // testing DHCP server states per netif state transitions
    esp_netif_action_start(ap, NULL, 0, NULL);
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STARTED, state);

    esp_netif_action_stop(ap, NULL, 0, NULL);
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_INIT, state);

    // destroy default wifi netif
    esp_netif_destroy(ap);
    TEST_ASSERT(esp_wifi_stop() == ESP_OK);
    TEST_ASSERT(esp_wifi_deinit() == ESP_OK);
    nvs_flash_deinit();
}

TEST_CASE("esp_netif: test dhcp state transitions for mesh netifs", "[esp_netif]")
{
    esp_netif_t *ap = NULL;
    esp_netif_t *sta = NULL;
    esp_netif_dhcp_status_t state;

    // init two mesh network interfaces
    test_case_uses_tcpip();
    TEST_ESP_OK(nvs_flash_init());
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_netif_create_default_wifi_mesh_netifs(&sta, &ap));
    TEST_ASSERT_NOT_NULL(sta);
    TEST_ASSERT_NOT_NULL(ap);
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_wifi_init(&wifi_cfg));

    // test both server and client are *not* STARTED after interfaces created
    TEST_ESP_OK(esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    TEST_ESP_OK(esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);

    // test both server and client are still *not* STARTED after start
    esp_netif_action_start(ap, NULL, 0, NULL);
    esp_netif_action_start(sta, NULL, 0, NULL);
    TEST_ESP_OK(esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    TEST_ESP_OK(esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);

    // test both server and client are still *not* STARTED even after connect
    esp_netif_action_connected(ap, NULL, 0, NULL);
    esp_netif_action_connected(sta, NULL, 0, NULL);
    TEST_ESP_OK(esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    TEST_ESP_OK(esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);

    // test station gets promoted to be a root (so DHCP client started manually) and client is in STATED state
    esp_netif_dhcpc_start(sta);
    esp_netif_action_connected(sta, NULL, 0, NULL);
    TEST_ESP_OK(esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    esp_netif_dhcpc_stop(sta);

    // test both server and client are still *not* STARTED even after stop
    esp_netif_action_stop(sta, NULL, 0, NULL);
    esp_netif_action_stop(ap, NULL, 0, NULL);
    TEST_ESP_OK(esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    TEST_ESP_OK(esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_NOT_EQUAL(ESP_NETIF_DHCP_STARTED, state);

    // destroy event_loop, netifs, wifi, nvs
    TEST_ESP_OK(esp_event_loop_delete_default());
    esp_netif_destroy(ap);
    esp_netif_destroy(sta);
    TEST_ASSERT(esp_wifi_stop() == ESP_OK);
    TEST_ASSERT(esp_wifi_deinit() == ESP_OK);
    nvs_flash_deinit();
}
