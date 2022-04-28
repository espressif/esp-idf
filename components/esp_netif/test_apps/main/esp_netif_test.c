#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "unity_fixture.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_wifi_netif.h"
#include "sdkconfig.h"
#include "lwip/sockets.h"
#include "test_utils.h"

TEST_GROUP(esp_netif);

TEST_SETUP(esp_netif)
{
}

TEST_TEAR_DOWN(esp_netif)
{
}

TEST(esp_netif, init_and_destroy)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *esp_netif = esp_netif_new(NULL);

    TEST_ASSERT_EQUAL(NULL, esp_netif);
    esp_netif = esp_netif_new(&cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, esp_netif);

    esp_netif_destroy(esp_netif);
}


TEST(esp_netif, get_from_if_key)
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


TEST(esp_netif, create_delete_multiple_netifs)
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

TEST(esp_netif, dhcp_client_state_transitions_wifi_sta)
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

TEST(esp_netif, dhcp_server_state_transitions_wifi_ap)
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

TEST(esp_netif, dhcp_server_state_transitions_mesh)
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

TEST(esp_netif, create_custom_wifi_interfaces)
{
    esp_netif_t *ap = NULL;
    esp_netif_t *sta = NULL;
    uint8_t configured_mac[6] = {1, 2, 3, 4, 5, 6};
    uint8_t actual_mac[6] = { 0 };

    // create customized station
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
    esp_netif_config.if_desc = "custom wifi station";
    esp_netif_config.route_prio = 1;
    sta = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
    TEST_ASSERT_NOT_NULL(sta);
    TEST_ASSERT_EQUAL_STRING("custom wifi station", esp_netif_get_desc(sta));
    TEST_ASSERT_EQUAL(1, esp_netif_get_route_prio(sta));

    // create customized access point
    esp_netif_inherent_config_t esp_netif_config2 = ESP_NETIF_INHERENT_DEFAULT_WIFI_AP();
    esp_netif_config2.if_desc = "custom wifi ap";
    esp_netif_config2.route_prio = 10;
    memcpy(esp_netif_config2.mac, configured_mac, 6);

    ap = esp_netif_create_wifi(WIFI_IF_AP, &esp_netif_config2);
    TEST_ASSERT_NOT_NULL(ap);
    TEST_ASSERT_EQUAL_STRING( "custom wifi ap", esp_netif_get_desc(ap));
    TEST_ASSERT_EQUAL(10, esp_netif_get_route_prio(ap));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_get_mac(ap, actual_mac));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(configured_mac, actual_mac, 6);

    esp_wifi_destroy_if_driver(esp_netif_get_io_driver(ap));
    esp_wifi_destroy_if_driver(esp_netif_get_io_driver(sta));
    esp_netif_destroy(ap);
    esp_netif_destroy(sta);
}


TEST(esp_netif, get_set_hostname)
{
    const char *hostname;
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();

    test_case_uses_tcpip();
    esp_netif_t *esp_netif = esp_netif_new(&cfg);

    // specific hostname not set yet, get_hostname should fail
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_netif_get_hostname(esp_netif, &hostname));

    TEST_ASSERT_NOT_NULL(esp_netif);
    esp_netif_attach_wifi_station(esp_netif);

    esp_netif_action_start(esp_netif, NULL, 0, NULL);

    // specific hostname not set yet, but if started, get_hostname to return default config value
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_get_hostname(esp_netif, &hostname));
    TEST_ASSERT_EQUAL_STRING(hostname, CONFIG_LWIP_LOCAL_HOSTNAME);

    // specific hostname set and get
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_set_hostname(esp_netif, "new_name"));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_get_hostname(esp_netif, &hostname));
    TEST_ASSERT_EQUAL_STRING(hostname, "new_name");

    esp_netif_destroy(esp_netif);
}

TEST_GROUP_RUNNER(esp_netif)
{
    RUN_TEST_CASE(esp_netif, init_and_destroy)
    RUN_TEST_CASE(esp_netif, get_from_if_key)
    RUN_TEST_CASE(esp_netif, create_delete_multiple_netifs)
    RUN_TEST_CASE(esp_netif, dhcp_client_state_transitions_wifi_sta)
    RUN_TEST_CASE(esp_netif, dhcp_server_state_transitions_wifi_ap)
    RUN_TEST_CASE(esp_netif, dhcp_server_state_transitions_mesh)
    RUN_TEST_CASE(esp_netif, create_custom_wifi_interfaces)
    RUN_TEST_CASE(esp_netif, get_set_hostname)
}

void app_main(void)
{
    UNITY_MAIN(esp_netif);
}
