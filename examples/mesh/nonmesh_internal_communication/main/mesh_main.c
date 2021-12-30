#include <sys/cdefs.h>
/* Non-Mesh Communication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <lwip/netif.h>
#include <lwip/lwip_napt.h>
#include <esp_console.h>
#include <lwip/inet.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "nvs_flash.h"


/*******************************************************
 *                Macros
 *******************************************************/

#define MAC_ADDR_LEN (6u)
/*******************************************************
 *                Constants
 *******************************************************/

const esp_netif_ip_info_t g_mesh_netif_subnet_ip = {        // mesh subnet IP info
        .ip = {.addr = ESP_IP4TOADDR(172, 16, 0, 1)},
        .gw = {.addr = ESP_IP4TOADDR(172, 16, 0, 1)},
        .netmask = {.addr = ESP_IP4TOADDR(255, 255, 255, 0)},
};
const esp_netif_ip_info_t g_nonmesh_netif_subnet_ip = {        // mesh subnet IP info
        .ip = {.addr = ESP_IP4TOADDR(172, 16, 1, 1)},
        .gw = {.addr = ESP_IP4TOADDR(172, 16, 1, 1)},
        .netmask = {.addr = ESP_IP4TOADDR(255, 255, 255, 0)},
};

/*******************************************************
 *                Variable Definitions
 *******************************************************/
static const char *MESH_TAG = "mesh_main";
static const uint8_t MESH_ID[6] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
static mesh_addr_t mesh_parent_addr;
static int mesh_layer = -1;
static esp_netif_t *netif_sta = NULL;
static esp_netif_t *netif_ap = NULL;
static esp_console_repl_t *s_repl = NULL;
static esp_ip4_addr_t s_current_ip;

/*******************************************************
 *                Function Declarations
 *******************************************************/
void register_ping_command(void);

/*******************************************************
 *                Function Definitions
 *******************************************************/

void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data) {
    mesh_addr_t id = {0,};
    static uint16_t last_layer = 0;

    switch (event_id) {
        case MESH_EVENT_STARTED: {
            esp_mesh_get_id(&id);
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_MESH_STARTED>ID:"MACSTR"", MAC2STR(id.addr));
            mesh_layer = esp_mesh_get_layer();
        }
            break;
        case MESH_EVENT_STOPPED: {
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOPPED>");
            mesh_layer = esp_mesh_get_layer();
        }
            break;
        case MESH_EVENT_CHILD_CONNECTED: {
            mesh_event_child_connected_t *child_connected = (mesh_event_child_connected_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, "MACSTR"",
                     child_connected->aid,
                     MAC2STR(child_connected->mac));
        }
            break;
        case MESH_EVENT_CHILD_DISCONNECTED: {
            mesh_event_child_disconnected_t *child_disconnected = (mesh_event_child_disconnected_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_DISCONNECTED>aid:%d, "MACSTR"",
                     child_disconnected->aid,
                     MAC2STR(child_disconnected->mac));
        }
            break;
        case MESH_EVENT_ROUTING_TABLE_ADD: {
            mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *) event_data;
            ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d, layer:%d",
                     routing_table->rt_size_change,
                     routing_table->rt_size_new, mesh_layer);
        }
            break;
        case MESH_EVENT_ROUTING_TABLE_REMOVE: {
            mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *) event_data;
            ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_REMOVE>remove %d, new:%d, layer:%d",
                     routing_table->rt_size_change,
                     routing_table->rt_size_new, mesh_layer);
        }
            break;
        case MESH_EVENT_NO_PARENT_FOUND: {
            mesh_event_no_parent_found_t *no_parent = (mesh_event_no_parent_found_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_NO_PARENT_FOUND>scan times:%d",
                     no_parent->scan_times);
        }
            /* TODO handler for the failure */
            break;
        case MESH_EVENT_PARENT_CONNECTED: {
            mesh_event_connected_t *connected = (mesh_event_connected_t *) event_data;
            esp_mesh_get_id(&id);
            mesh_layer = connected->self_layer;
            memcpy(&mesh_parent_addr.addr, connected->connected.bssid, 6);
            ESP_LOGI(MESH_TAG,
                     "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:"MACSTR"%s, ID:"MACSTR", duty:%d",
                     last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
                     esp_mesh_is_root() ? "<ROOT>" :
                     (mesh_layer == 2) ? "<layer2>" : "", MAC2STR(id.addr), connected->duty);
            last_layer = mesh_layer;
            if (esp_mesh_is_root()) {
                esp_netif_dhcpc_stop(netif_sta);
                esp_netif_dhcpc_start(netif_sta);
            } else {
                esp_netif_dhcpc_stop(netif_sta);
                esp_netif_dhcpc_start(netif_sta);
                ESP_LOGI(MESH_TAG, "[NODE] Updating IP on NODE AP interface!");
                esp_netif_set_ip_info(netif_ap, &g_nonmesh_netif_subnet_ip);
            }
        }
            break;
        case MESH_EVENT_PARENT_DISCONNECTED: {
            mesh_event_disconnected_t *disconnected = (mesh_event_disconnected_t *) event_data;
            ESP_LOGI(MESH_TAG,
                     "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d",
                     disconnected->reason);
            mesh_layer = esp_mesh_get_layer();
        }
            break;
        case MESH_EVENT_LAYER_CHANGE: {
            mesh_event_layer_change_t *layer_change = (mesh_event_layer_change_t *) event_data;
            mesh_layer = layer_change->new_layer;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_LAYER_CHANGE>layer:%d-->%d%s",
                     last_layer, mesh_layer,
                     esp_mesh_is_root() ? "<ROOT>" :
                     (mesh_layer == 2) ? "<layer2>" : "");
        }
            break;
        case MESH_EVENT_ROOT_ADDRESS: {
            mesh_event_root_address_t *root_addr = (mesh_event_root_address_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:"MACSTR"",
                     MAC2STR(root_addr->addr));
        }
            break;
        case MESH_EVENT_VOTE_STARTED: {
            mesh_event_vote_started_t *vote_started = (mesh_event_vote_started_t *) event_data;
            ESP_LOGI(MESH_TAG,
                     "<MESH_EVENT_VOTE_STARTED>attempts:%d, reason:%d, rc_addr:"MACSTR"",
                     vote_started->attempts,
                     vote_started->reason,
                     MAC2STR(vote_started->rc_addr.addr));
        }
            break;
        case MESH_EVENT_VOTE_STOPPED: {
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_VOTE_STOPPED>");
            break;
        }
        case MESH_EVENT_ROOT_SWITCH_REQ: {
            mesh_event_root_switch_req_t *switch_req = (mesh_event_root_switch_req_t *) event_data;
            ESP_LOGI(MESH_TAG,
                     "<MESH_EVENT_ROOT_SWITCH_REQ>reason:%d, rc_addr:"MACSTR"",
                     switch_req->reason,
                     MAC2STR(switch_req->rc_addr.addr));
        }
            break;
        case MESH_EVENT_ROOT_SWITCH_ACK: {
            /* new root */
            mesh_layer = esp_mesh_get_layer();
            esp_mesh_get_parent_bssid(&mesh_parent_addr);
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_SWITCH_ACK>layer:%d, parent:"MACSTR"", mesh_layer,
                     MAC2STR(mesh_parent_addr.addr));
        }
            break;
        case MESH_EVENT_TODS_STATE: {
            mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
        }
            break;
        case MESH_EVENT_ROOT_FIXED: {
            mesh_event_root_fixed_t *root_fixed = (mesh_event_root_fixed_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_FIXED>%s",
                     root_fixed->is_fixed ? "fixed" : "not fixed");
        }
            break;
        case MESH_EVENT_ROOT_ASKED_YIELD: {
            mesh_event_root_conflict_t *root_conflict = (mesh_event_root_conflict_t *) event_data;
            ESP_LOGI(MESH_TAG,
                     "<MESH_EVENT_ROOT_ASKED_YIELD>"MACSTR", rssi:%d, capacity:%d",
                     MAC2STR(root_conflict->addr),
                     root_conflict->rssi,
                     root_conflict->capacity);
        }
            break;
        case MESH_EVENT_CHANNEL_SWITCH: {
            mesh_event_channel_switch_t *channel_switch = (mesh_event_channel_switch_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHANNEL_SWITCH>new channel:%d", channel_switch->channel);
        }
            break;
        case MESH_EVENT_SCAN_DONE: {
            mesh_event_scan_done_t *scan_done = (mesh_event_scan_done_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_SCAN_DONE>number:%d",
                     scan_done->number);
        }
            break;
        case MESH_EVENT_NETWORK_STATE: {
            mesh_event_network_state_t *network_state = (mesh_event_network_state_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_NETWORK_STATE>is_rootless:%d",
                     network_state->is_rootless);
        }
            break;
        case MESH_EVENT_STOP_RECONNECTION: {
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOP_RECONNECTION>");
        }
            break;
        case MESH_EVENT_FIND_NETWORK: {
            mesh_event_find_network_t *find_network = (mesh_event_find_network_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_FIND_NETWORK>new channel:%d, router BSSID:"MACSTR"",
                     find_network->channel, MAC2STR(find_network->router_bssid));
        }
            break;
        case MESH_EVENT_ROUTER_SWITCH: {
            mesh_event_router_switch_t *router_switch = (mesh_event_router_switch_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROUTER_SWITCH>new router:%s, channel:%d, "MACSTR"",
                     router_switch->ssid, router_switch->channel, MAC2STR(router_switch->bssid));
        }
            break;
        case MESH_EVENT_PS_PARENT_DUTY: {
            mesh_event_ps_duty_t *ps_duty = (mesh_event_ps_duty_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_PS_PARENT_DUTY>duty:%d", ps_duty->duty);
        }
            break;
        case MESH_EVENT_PS_CHILD_DUTY: {
            mesh_event_ps_duty_t *ps_duty = (mesh_event_ps_duty_t *) event_data;
            ESP_LOGI(MESH_TAG, "<MESH_EVENT_PS_CHILD_DUTY>cidx:%d, "MACSTR", duty:%d", ps_duty->child_connected.aid - 1,
                     MAC2STR(ps_duty->child_connected.mac), ps_duty->duty);
        }
            break;
        default:
            ESP_LOGI(MESH_TAG, "unknown id:%d", event_id);
            break;
    }
}

static esp_err_t set_dhcps_dns(esp_netif_t *netif, uint32_t addr)
{
    esp_netif_dns_info_t dns;
    dns.ip.u_addr.ip4.addr = addr;
    dns.ip.type = IPADDR_TYPE_V4;
    dhcps_offer_t dhcps_dns_value = OFFER_DNS;
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_dns_value, sizeof(dhcps_dns_value)));
    ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns));
    return ESP_OK;
}

// Cloned method where I removed the ESP_NETIF_DHCP_SERVER flag from the SoftAP configuration
// TODO: This fixes the DNS not properly sent to clients (the DNS in the DHCP options was always 0.0.0.0, no idea why its directly related to this flag)
esp_err_t my_esp_netif_create_default_wifi_mesh_netifs(esp_netif_t **p_netif_sta, esp_netif_t **p_netif_ap)
{
    // Create "almost" default AP, with un-flagged DHCP server
    esp_netif_inherent_config_t netif_cfg;
    memcpy(&netif_cfg, ESP_NETIF_BASE_DEFAULT_WIFI_AP, sizeof(netif_cfg));
    //netif_cfg.flags &= ~ESP_NETIF_DHCP_SERVER;
    esp_netif_config_t cfg_ap = {
            .base = &netif_cfg,
            .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP,
    };
    esp_netif_t *netif_ap = esp_netif_new(&cfg_ap);
    assert(netif_ap);
    ESP_ERROR_CHECK(esp_netif_attach_wifi_ap(netif_ap));
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_ap_handlers());

    // ...and stop DHCP server to be compatible with former tcpip_adapter (to keep the ESP_NETIF_DHCP_STOPPED state)
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif_ap));

    // Create "almost" default station, but with un-flagged DHCP client
    memcpy(&netif_cfg, ESP_NETIF_BASE_DEFAULT_WIFI_STA, sizeof(netif_cfg));
    netif_cfg.flags &= ~ESP_NETIF_DHCP_CLIENT;
    esp_netif_config_t cfg_sta = {
            .base = &netif_cfg,
            .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA,
    };
    esp_netif_t *netif_sta = esp_netif_new(&cfg_sta);
    assert(netif_sta);
    ESP_ERROR_CHECK(esp_netif_attach_wifi_station(netif_sta));
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_sta_handlers());

    // ...and stop DHCP client (to be started separately if the station were promoted to root)
    ESP_ERROR_CHECK(esp_netif_dhcpc_stop(netif_sta));

    if (p_netif_sta) {
        *p_netif_sta = netif_sta;
    }
    if (p_netif_ap) {
        *p_netif_ap = netif_ap;
    }
    return ESP_OK;
}

void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    char netif_name[3];
    esp_netif_get_netif_impl_name(event->esp_netif, netif_name);
    ESP_LOGI(MESH_TAG, "<IP_EVENT_STA_GOT_IP> netif %s IP:" IPSTR, netif_name,IP2STR(&event->ip_info.ip));
    s_current_ip.addr = event->ip_info.ip.addr;

    esp_netif_dns_info_t dns;
    ESP_ERROR_CHECK(esp_netif_get_dns_info(event->esp_netif, ESP_NETIF_DNS_MAIN, &dns));
    ESP_LOGI(MESH_TAG, "[%s] Read DNS AP IP %s", esp_mesh_is_root() ? "ROOT" : "NODE", inet_ntoa(dns.ip.u_addr.ip4.addr));
    set_dhcps_dns(netif_ap, dns.ip.u_addr.ip4.addr);
    ESP_LOGI(MESH_TAG, "[%s] Forwarding DNS %s to AP clients", esp_mesh_is_root() ? "ROOT" : "NODE", inet_ntoa(dns.ip.u_addr.ip4.addr));

    if (esp_mesh_is_root()) {
        ESP_LOGI(MESH_TAG, "[ROOT] Enabling PNAT on " IPSTR, IP2STR(&g_mesh_netif_subnet_ip.ip));
        ip_napt_enable(g_mesh_netif_subnet_ip.ip.addr, 1);
        esp_netif_dhcps_start(netif_ap);
        ESP_LOGI(MESH_TAG, "[ROOT] DHCP Server for network " IPSTR " on AP started!", IP2STR(&g_mesh_netif_subnet_ip.ip));
    } else {
        ESP_LOGI(MESH_TAG, "[NODE] Enabling PNAT on " IPSTR, IP2STR(&g_nonmesh_netif_subnet_ip.ip));
        ip_napt_enable(g_nonmesh_netif_subnet_ip.ip.addr, 1);
        esp_netif_dhcps_start(netif_ap);
        ESP_LOGI(MESH_TAG, "[NODE] DHCP Server for network " IPSTR " on AP started!", IP2STR(&g_nonmesh_netif_subnet_ip.ip));
    }
}

_Noreturn static void print_stats(void *args) {
    while (1) {
        uint8_t apmac[MAC_ADDR_LEN];
        uint8_t stamac[MAC_ADDR_LEN];
        esp_wifi_get_mac(WIFI_IF_AP, apmac);
        esp_wifi_get_mac(WIFI_IF_STA, stamac);
        ESP_LOGI(MESH_TAG,
                 "[%s] LAYER: %1d | TYPE: %1d | MESH_SSID: ESPM_%2X%2X%2X | TOPO: %s | NODE#: %d | IP:" IPSTR " | AP: " MACSTR " | STA: " MACSTR,
                 esp_mesh_is_root() ? "ROOT" : "NODE", esp_mesh_get_layer(), esp_mesh_get_type(), stamac[3], stamac[4],
                 stamac[5],
                 esp_mesh_get_topology() == MESH_TOPO_TREE ? "tree" : "chain", esp_mesh_get_total_node_num(),
                 IP2STR(&s_current_ip), MAC2STR(apmac), MAC2STR(stamac));
        struct netif *netif;
        NETIF_FOREACH(netif) {
            ESP_LOGI(MESH_TAG, "Interface %s, NAPT: %d, IPv4: " IPSTR, netif->name, netif->napt,
                     IP2STR(&netif->ip_addr.u_addr.ip4));
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}


void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    /*  tcpip initialization */
    ESP_ERROR_CHECK(esp_netif_init());
    /*  event initialization */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /*  create network interfaces for mesh (only station instance saved for further manipulation, soft AP instance ignored */
    ESP_ERROR_CHECK(my_esp_netif_create_default_wifi_mesh_netifs(&netif_sta, &netif_ap));
    ESP_LOGI(MESH_TAG, "Changed IP of ROOT AP to" IPSTR, IP2STR(&g_mesh_netif_subnet_ip.ip));
    esp_netif_set_ip_info(netif_ap, &g_mesh_netif_subnet_ip);

    /*  wifi initialization */
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_start());
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL));
    /*  set mesh topology */
    ESP_ERROR_CHECK(esp_mesh_set_topology(CONFIG_MESH_TOPOLOGY));
    ESP_LOGI(MESH_TAG, "Setting mesh topology to %s", CONFIG_MESH_TOPOLOGY == MESH_TOPO_TREE ? "tree" : "chain");
    /*  set mesh max layer according to the topology */
    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_MESH_MAX_LAYER));
    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
    ESP_ERROR_CHECK(esp_mesh_disable_ps());
    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(10));
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
    /* mesh ID */
    memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);
    /* router */
    cfg.channel = CONFIG_MESH_CHANNEL;
    cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
    memcpy((uint8_t *) &cfg.router.ssid, CONFIG_MESH_ROUTER_SSID, cfg.router.ssid_len);
    memcpy((uint8_t *) &cfg.router.password, CONFIG_MESH_ROUTER_PASSWD,
           strlen(CONFIG_MESH_ROUTER_PASSWD));
    /* mesh softAP */
    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(CONFIG_MESH_AP_AUTHMODE));
    cfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;
    cfg.mesh_ap.nonmesh_max_connection = CONFIG_MESH_NON_MESH_AP_CONNECTIONS;
    memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
           strlen(CONFIG_MESH_AP_PASSWD));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
    /* mesh start */
    ESP_ERROR_CHECK(esp_mesh_start());
    ESP_LOGI(MESH_TAG, "mesh starts successfully, heap:%d, %s<%d>%s, ps:%d\n", esp_get_minimum_free_heap_size(),
             esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed",
             esp_mesh_get_topology(), esp_mesh_get_topology() ? "(chain)" : "(tree)", esp_mesh_is_ps_enabled());

    xTaskCreate(print_stats, "print_stats", 3072, NULL, 5, NULL);

    // configure console
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    // install console REPL environment
#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));
#endif
    register_ping_command();

    //esp_log_level_set("mesh_hexdump", ESP_LOG_VERBOSE);
    //esp_log_level_set("mesh_main", ESP_LOG_INFO);

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(s_repl));
}
