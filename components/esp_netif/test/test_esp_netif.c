#include <string.h>
#include "unity.h"
#include "test_utils.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_wifi_netif.h"
#include "lwip/netif.h"
#include "esp_netif_net_stack.h"


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

// This is a private esp-netif API, but include here to test it
bool esp_netif_is_netif_listed(esp_netif_t *esp_netif);

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

    // there's no AP within created netifs
    TEST_ASSERT_NULL(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"));

    // check that the created netifs are correctly found by their interface keys and globally listed
    for (int i=0; i<nr_of_netifs; ++i) {
        TEST_ASSERT_EQUAL(netifs[i], esp_netif_get_handle_from_ifkey(if_keys[i]));
        TEST_ASSERT_TRUE(esp_netif_is_netif_listed(netifs[i]));
    }

    // destroy one by one and check it's been removed
    for (int i=0; i<nr_of_netifs; ++i) {
        esp_netif_destroy(netifs[i]);
        TEST_ASSERT_FALSE(esp_netif_is_netif_listed(netifs[i]));
    }

}


#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5047

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

    // test manual DHCP state transitions using dhcpc-start/stop API
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_stop(sta));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STOPPED, state);
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_start(sta));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    TEST_ASSERT_EQUAL(ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED, esp_netif_dhcpc_start(sta));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcpc_get_status(sta, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STARTED, state);

    // stop the netif and test dhcp state update
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

    // test manual DHCP state transitions using dhcps-start/stop API
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcps_stop(ap));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STOPPED, state);
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcps_start(ap));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STARTED, state);
    TEST_ASSERT_EQUAL(ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED, esp_netif_dhcps_start(ap));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_dhcps_get_status(ap, &state));
    TEST_ASSERT_EQUAL(ESP_NETIF_DHCP_STARTED, state);

    // stop the netif and test dhcp state update
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

TEST_CASE("esp_netif: create custom wifi interfaces", "[esp_netif][leaks=0]")
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


TEST_CASE("esp_netif: get/set hostname", "[esp_netif]")
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

    // test that setting the long name is refused and the previously set value retained
    #define ESP_NETIF_HOSTNAME_MAX_SIZE    32
    char long_name[ESP_NETIF_HOSTNAME_MAX_SIZE + 2] = { 0 };
    memset(long_name, 'A', ESP_NETIF_HOSTNAME_MAX_SIZE+1); // construct the long name
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_netif_set_hostname(esp_netif, long_name));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_get_hostname(esp_netif, &hostname));
    TEST_ASSERT_EQUAL_STRING(hostname, "new_name");

    esp_netif_destroy(esp_netif);
}

#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)

TEST_CASE("esp_netif: convert ip address from string", "[esp_netif]")
{
    const char *ipv4_src[] = {"127.168.1.1", "255.255.255.0", "305.500.721.801", "127.168.1..", "abc.def.***.ddd"};
    esp_ip4_addr_t ipv4;
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_str_to_ip4(ipv4_src[0], &ipv4));
    TEST_ASSERT_EQUAL(ipv4.addr, ESP_IP4TOADDR(127, 168, 1, 1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_str_to_ip4(ipv4_src[1], &ipv4));
    TEST_ASSERT_EQUAL(ipv4.addr, ESP_IP4TOADDR(255, 255, 255, 0));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_netif_str_to_ip4(ipv4_src[2], &ipv4));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_netif_str_to_ip4(ipv4_src[3], &ipv4));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_netif_str_to_ip4(ipv4_src[4], &ipv4));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_netif_str_to_ip4(NULL, &ipv4));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_netif_str_to_ip4(ipv4_src[0], NULL));

    const char *ipv6_src[] = {"127:168:6:8:188:65:1:0", "255:255:255:0:0:0:65:56", "305:500:721:888:777:458:555:666", "EFGH.127:168::55"};
    esp_ip6_addr_t ipv6;
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_str_to_ip6(ipv6_src[0], &ipv6));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_str_to_ip6(ipv6_src[1], &ipv6));
    TEST_ASSERT_EQUAL(ESP_OK, esp_netif_str_to_ip6(ipv6_src[2], &ipv6));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_netif_str_to_ip6(ipv6_src[3], &ipv6));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_netif_str_to_ip6(NULL, &ipv6));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_netif_str_to_ip6(ipv6_src[0], NULL));
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5047
TEST_CASE("esp_netif: create and destroy default wifi interfaces", "[esp_netif][leaks=0]")
{
    // Helper constants to refer default STA and AP's params
    static const esp_netif_inherent_config_t default_sta_cfg = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
    static const esp_netif_inherent_config_t default_ap_cfg = ESP_NETIF_INHERENT_DEFAULT_WIFI_AP();

    // create default station
    esp_netif_t *sta = esp_netif_create_default_wifi_sta();

    // check it gets created and has default params
    TEST_ASSERT_NOT_NULL(sta);
    TEST_ASSERT_EQUAL_STRING(default_sta_cfg.if_desc, esp_netif_get_desc(sta));
    TEST_ASSERT_EQUAL(default_sta_cfg.route_prio, esp_netif_get_route_prio(sta));

    // create default access point
    esp_netif_t *ap = esp_netif_create_default_wifi_ap();

    // check it gets created and has default params
    TEST_ASSERT_NOT_NULL(ap);
    TEST_ASSERT_EQUAL_STRING(default_ap_cfg.if_desc, esp_netif_get_desc(ap));
    TEST_ASSERT_EQUAL(default_ap_cfg.route_prio, esp_netif_get_route_prio(ap));

    // destroy the station
    esp_netif_destroy_default_wifi(sta);
    // destroy the AP
    esp_netif_destroy_default_wifi(ap);

    // quick check on create-destroy cycle of the default station again
    sta = NULL;
    sta = esp_netif_create_default_wifi_sta();
    TEST_ASSERT_NOT_NULL(sta);
    esp_netif_destroy_default_wifi(sta);
}


#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)

static esp_err_t dummy_transmit(void* hd, void *buf, size_t length)
{
    return ESP_OK;
}

TEST_CASE("esp_netif: test routing priority", "[esp_netif]")
{
    test_case_uses_tcpip();
    // interface key has to be a unique identifier
    const char *if_keys[] = {"if0", "if1", "if2", "if3", "if4", "if5", "if6", "if7", "if8", "if9"};
    const int nr_of_netifs = sizeof(if_keys) / sizeof(char *);
    esp_netif_t *netifs[nr_of_netifs];
    esp_netif_driver_ifconfig_t driver_config = { .handle =  (void*)1, .transmit = dummy_transmit };
    // create 10 netifs with different route prio
    int max_prio_i = nr_of_netifs / 2;  // index of netif with maximum route-prio
    for (int i = 0; i < nr_of_netifs; ++i) {
        esp_netif_inherent_config_t base_netif_config = { .if_key = if_keys[i],
                                                          .route_prio = i > max_prio_i ? 0 : i };
        esp_netif_config_t cfg = {  .base = &base_netif_config,
                                    .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA,
                                    .driver = &driver_config };
        netifs[i] = esp_netif_new(&cfg);
        TEST_ASSERT_NOT_NULL(netifs[i]);
        // set the interface up and connected -- to enable the default netif based on route_prio
        esp_netif_action_start(netifs[i], 0, 0, 0);
        esp_netif_action_connected(netifs[i], 0, 0, 0);
    }
    // route_prio increases with index until max_prio_i -> check this is the default netif
    TEST_ASSERT_EQUAL_PTR(esp_netif_get_netif_impl(netifs[max_prio_i]), netif_default);
    // now we stop the max_prio netif and check the default is on the previous index (max_prio-1)
    esp_netif_action_stop(netifs[max_prio_i], 0, 0, 0);
    TEST_ASSERT_EQUAL_PTR(esp_netif_get_netif_impl(netifs[max_prio_i - 1]), netif_default);

    // now we override the default netif with API (which has route_prio == 0)
    int override_prio_i = nr_of_netifs - 1;  // last netif to be set-default manually
    esp_netif_set_default_netif(netifs[override_prio_i]);
    // check the configured netif is default
    TEST_ASSERT_EQUAL_PTR(esp_netif_get_netif_impl(netifs[override_prio_i]), netif_default);
    // try to start/connect the previously stopped netif with max_prio
    esp_netif_action_start(netifs[max_prio_i], 0, 0, 0);
    esp_netif_action_connected(netifs[max_prio_i], 0, 0, 0);
    // and check the configured netif is still the default
    TEST_ASSERT_EQUAL_PTR(esp_netif_get_netif_impl(netifs[override_prio_i]), netif_default);
    // we destroy the configured default netif
    esp_netif_destroy(netifs[override_prio_i]);
    // ...and check the max-prio netif is default now
    TEST_ASSERT_EQUAL_PTR(esp_netif_get_netif_impl(netifs[max_prio_i]), netif_default);
    // stop the max_prio netif, to see the auto-default still works
    esp_netif_action_stop(netifs[max_prio_i], 0, 0, 0);
    // ...so the current default is on (max_prio-1)
    TEST_ASSERT_EQUAL_PTR(esp_netif_get_netif_impl(netifs[max_prio_i - 1]), netif_default);
    // destroy one by one and check it's been removed
    for (int i=0; i < override_prio_i; ++i) {
        esp_netif_destroy(netifs[i]);
        TEST_ASSERT_FALSE(esp_netif_is_netif_listed(netifs[i]));
    }
}
