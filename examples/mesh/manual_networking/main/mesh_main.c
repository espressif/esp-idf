/* Mesh Manual Networking Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"
#include "mesh_light.h"
#include "nvs_flash.h"

/*******************************************************
 *                Macros
 *******************************************************/
//#define MESH_SET_ROOT

#ifndef MESH_SET_ROOT
#define MESH_SET_NODE
#endif

/*******************************************************
 *                Constants
 *******************************************************/

/*******************************************************
 *                Variable Definitions
 *******************************************************/
static const char *MESH_TAG = "mesh_main";
static const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
static mesh_addr_t mesh_parent_addr;
static int mesh_layer = -1;
static esp_netif_t *netif_sta = NULL;

/*******************************************************
 *                Function Declarations
 *******************************************************/
void mesh_scan_done_handler(int num);

/*******************************************************
 *                Function Definitions
 *******************************************************/
void mesh_scan_done_handler(int num)
{
    int i;
    int ie_len = 0;
    mesh_assoc_t assoc;
    mesh_assoc_t parent_assoc = { .layer = CONFIG_MESH_MAX_LAYER, .rssi = -120 };
    wifi_ap_record_t record;
    wifi_ap_record_t parent_record = { 0, };
    bool parent_found = false;
    mesh_type_t my_type = MESH_IDLE;
    int my_layer = -1;
    wifi_config_t parent = { 0, };
    wifi_scan_config_t scan_config = { 0 };

    for (i = 0; i < num; i++) {
        esp_mesh_scan_get_ap_ie_len(&ie_len);
        esp_mesh_scan_get_ap_record(&record, &assoc);
        if (ie_len == sizeof(assoc)) {
            ESP_LOGW(MESH_TAG,
                     "<MESH>[%d]%s, layer:%d/%d, assoc:%d/%d, %d, "MACSTR", channel:%u, rssi:%d, ID<"MACSTR"><%s>",
                     i, record.ssid, assoc.layer, assoc.layer_cap, assoc.assoc,
                     assoc.assoc_cap, assoc.layer2_cap, MAC2STR(record.bssid),
                     record.primary, record.rssi, MAC2STR(assoc.mesh_id), assoc.encrypted ? "IE Encrypted" : "IE Unencrypted");

#ifdef MESH_SET_NODE
            if (assoc.mesh_type != MESH_IDLE && assoc.layer_cap
                    && assoc.assoc < assoc.assoc_cap && record.rssi > -70) {
                if (assoc.layer < parent_assoc.layer || assoc.layer2_cap < parent_assoc.layer2_cap) {
                    parent_found = true;
                    memcpy(&parent_record, &record, sizeof(record));
                    memcpy(&parent_assoc, &assoc, sizeof(assoc));
                    if (parent_assoc.layer_cap != 1) {
                        my_type = MESH_NODE;
                    } else {
                        my_type = MESH_LEAF;
                    }
                    my_layer = parent_assoc.layer + 1;
                    break;
                }
            }
#endif
        } else {
            ESP_LOGI(MESH_TAG, "[%d]%s, "MACSTR", channel:%u, rssi:%d", i,
                     record.ssid, MAC2STR(record.bssid), record.primary,
                     record.rssi);
#ifdef MESH_SET_ROOT
            if (!strcmp(CONFIG_MESH_ROUTER_SSID, (char *) record.ssid)) {
                parent_found = true;
                memcpy(&parent_record, &record, sizeof(record));
                my_type = MESH_ROOT;
                my_layer = MESH_ROOT_LAYER;
            }
#endif
        }
    }
    esp_mesh_flush_scan_result();
    if (parent_found) {
        /*
         * parent
         * Both channel and SSID of the parent are mandatory.
         */
        parent.sta.channel = parent_record.primary;
        memcpy(&parent.sta.ssid, &parent_record.ssid,
               sizeof(parent_record.ssid));
        parent.sta.bssid_set = 1;
        memcpy(&parent.sta.bssid, parent_record.bssid, 6);
        ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(parent_record.authmode));
        if (my_type == MESH_ROOT) {
            if (parent_record.authmode != WIFI_AUTH_OPEN) {
                memcpy(&parent.sta.password, CONFIG_MESH_ROUTER_PASSWD,
                       strlen(CONFIG_MESH_ROUTER_PASSWD));
            }
            ESP_LOGW(MESH_TAG, "<PARENT>%s, "MACSTR", channel:%u, rssi:%d",
                     parent_record.ssid, MAC2STR(parent_record.bssid),
                     parent_record.primary, parent_record.rssi);
        } else {
            if (parent_record.authmode != WIFI_AUTH_OPEN) {
                memcpy(&parent.sta.password, CONFIG_MESH_AP_PASSWD,
                       strlen(CONFIG_MESH_AP_PASSWD));
            }
            ESP_LOGW(MESH_TAG,
                     "<PARENT>%s, layer:%d/%d, assoc:%d/%d, %d, "MACSTR", channel:%u, rssi:%d",
                     parent_record.ssid, parent_assoc.layer,
                     parent_assoc.layer_cap, parent_assoc.assoc,
                     parent_assoc.assoc_cap, parent_assoc.layer2_cap,
                     MAC2STR(parent_record.bssid), parent_record.primary,
                     parent_record.rssi);
        }
        ESP_ERROR_CHECK(esp_mesh_set_parent(&parent, (mesh_addr_t *)&parent_assoc.mesh_id, my_type, my_layer));

    } else {
        ESP_LOGW(MESH_TAG,
                 "<Warning>no parent found, modify IE crypto configuration and scan");
        if (CONFIG_MESH_IE_CRYPTO_FUNCS) {
            /* modify IE crypto key */
            ESP_LOGW(MESH_TAG, "<Config>modify IE crypto key to %s", CONFIG_MESH_IE_CRYPTO_KEY);
            ESP_ERROR_CHECK(esp_mesh_set_ie_crypto_funcs(&g_wifi_default_mesh_crypto_funcs));
            ESP_ERROR_CHECK(esp_mesh_set_ie_crypto_key(CONFIG_MESH_IE_CRYPTO_KEY, strlen(CONFIG_MESH_IE_CRYPTO_KEY)));
        } else {
            /* disable IE crypto */
            ESP_LOGW(MESH_TAG, "<Config>disable IE crypto");
            ESP_ERROR_CHECK(esp_mesh_set_ie_crypto_funcs(NULL));
        }
        esp_wifi_scan_stop();
        scan_config.show_hidden = 1;
        scan_config.scan_type = WIFI_SCAN_TYPE_PASSIVE;
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, 0));
    }
}

void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    mesh_addr_t id = {0,};
    static uint8_t last_layer = 0;
    wifi_scan_config_t scan_config = { 0 };

    switch (event_id) {
    case MESH_EVENT_STARTED: {
        esp_mesh_get_id(&id);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STARTED>ID:"MACSTR"", MAC2STR(id.addr));
        mesh_layer = esp_mesh_get_layer();
        ESP_ERROR_CHECK(esp_mesh_set_self_organized(0, 0));
        esp_wifi_scan_stop();
        /* mesh softAP is hidden */
        scan_config.show_hidden = 1;
        scan_config.scan_type = WIFI_SCAN_TYPE_PASSIVE;
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, 0));
    }
    break;
    case MESH_EVENT_STOPPED: {
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOPPED>");
        mesh_layer = esp_mesh_get_layer();
    }
    break;
    case MESH_EVENT_CHILD_CONNECTED: {
        mesh_event_child_connected_t *child_connected = (mesh_event_child_connected_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, "MACSTR"",
                 child_connected->aid,
                 MAC2STR(child_connected->mac));
    }
    break;
    case MESH_EVENT_CHILD_DISCONNECTED: {
        mesh_event_child_disconnected_t *child_disconnected = (mesh_event_child_disconnected_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_DISCONNECTED>aid:%d, "MACSTR"",
                 child_disconnected->aid,
                 MAC2STR(child_disconnected->mac));
    }
    break;
    case MESH_EVENT_ROUTING_TABLE_ADD: {
        mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *)event_data;
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d",
                 routing_table->rt_size_change,
                 routing_table->rt_size_new);
    }
    break;
    case MESH_EVENT_ROUTING_TABLE_REMOVE: {
        mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *)event_data;
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_REMOVE>remove %d, new:%d",
                 routing_table->rt_size_change,
                 routing_table->rt_size_new);
    }
    break;
    case MESH_EVENT_NO_PARENT_FOUND: {
        mesh_event_no_parent_found_t *no_parent = (mesh_event_no_parent_found_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_NO_PARENT_FOUND>scan times:%d",
                 no_parent->scan_times);
    }
    /* TODO handler for the failure */
    break;
    case MESH_EVENT_PARENT_CONNECTED: {
        mesh_event_connected_t *connected = (mesh_event_connected_t *)event_data;
        esp_mesh_get_id(&id);
        mesh_layer = connected->self_layer;
        memcpy(&mesh_parent_addr.addr, connected->connected.bssid, 6);
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:"MACSTR"%s, ID:"MACSTR"",
                 last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
                 esp_mesh_is_root() ? "<ROOT>" :
                 (mesh_layer == 2) ? "<layer2>" : "", MAC2STR(id.addr));
        last_layer = mesh_layer;
        mesh_connected_indicator(mesh_layer);
        if (esp_mesh_is_root()) {
            esp_netif_dhcpc_start(netif_sta);
        }
    }
    break;
    case MESH_EVENT_PARENT_DISCONNECTED: {
        mesh_event_disconnected_t *disconnected = (mesh_event_disconnected_t *)event_data;
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d",
                 disconnected->reason);
        mesh_disconnected_indicator();
        mesh_layer = esp_mesh_get_layer();
        if (disconnected->reason == WIFI_REASON_ASSOC_TOOMANY) {
            esp_wifi_scan_stop();
            scan_config.show_hidden = 1;
            scan_config.scan_type = WIFI_SCAN_TYPE_PASSIVE;
            ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, 0));
        }
    }
    break;
    case MESH_EVENT_LAYER_CHANGE: {
        mesh_event_layer_change_t *layer_change = (mesh_event_layer_change_t *)event_data;
        mesh_layer = layer_change->new_layer;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_LAYER_CHANGE>layer:%d-->%d%s",
                 last_layer, mesh_layer,
                 esp_mesh_is_root() ? "<ROOT>" :
                 (mesh_layer == 2) ? "<layer2>" : "");
        last_layer = mesh_layer;
        mesh_connected_indicator(mesh_layer);
    }
    break;
    case MESH_EVENT_ROOT_ADDRESS: {
        mesh_event_root_address_t *root_addr = (mesh_event_root_address_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:"MACSTR"",
                 MAC2STR(root_addr->addr));
    }
    break;
    case MESH_EVENT_TODS_STATE: {
        mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
    }
    break;
    case MESH_EVENT_ROOT_FIXED: {
        mesh_event_root_fixed_t *root_fixed = (mesh_event_root_fixed_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_FIXED>%s",
                 root_fixed->is_fixed ? "fixed" : "not fixed");
    }
    break;
    case MESH_EVENT_SCAN_DONE: {
        mesh_event_scan_done_t *scan_done = (mesh_event_scan_done_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_SCAN_DONE>number:%d",
                 scan_done->number);
        mesh_scan_done_handler(scan_done->number);
    }
    break;
    default:
        ESP_LOGI(MESH_TAG, "unknown id:%d", event_id);
        break;
    }
}

void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    ESP_LOGI(MESH_TAG, "<IP_EVENT_STA_GOT_IP>IP:" IPSTR, IP2STR(&event->ip_info.ip));
}

void app_main(void)
{
    ESP_ERROR_CHECK(mesh_light_init());
    ESP_ERROR_CHECK(nvs_flash_init());
    /*  tcpip initialization */
    esp_netif_init();
    /*  event initialization */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /*  crete network interfaces for mesh (only station instance saved for further manipulation, soft AP instance ignored */
    ESP_ERROR_CHECK(esp_netif_create_default_wifi_mesh_netifs(&netif_sta, NULL));
    /*  wifi initialization */
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_start());
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL));
    /* mesh enable IE crypto */
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
    memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
           strlen(CONFIG_MESH_AP_PASSWD));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
    /* mesh start */
    ESP_ERROR_CHECK(esp_mesh_start());
    ESP_LOGI(MESH_TAG, "mesh starts successfully, heap:%d\n",  esp_get_free_heap_size());
}
