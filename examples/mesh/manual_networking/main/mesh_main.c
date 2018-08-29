/* Mesh Manual Networking Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
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

/*******************************************************
 *                Function Declarations
 *******************************************************/
void mesh_event_handler(mesh_event_t event);
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

    for (i = 0; i < num; i++) {
        esp_mesh_scan_get_ap_ie_len(&ie_len);
        esp_mesh_scan_get_ap_record(&record, &assoc);
        if (ie_len == sizeof(assoc)) {
            ESP_LOGW(MESH_TAG,
                     "<MESH>[%d]%s, layer:%d/%d, assoc:%d/%d, %d, "MACSTR", channel:%u, rssi:%d, ID<"MACSTR">",
                     i, record.ssid, assoc.layer, assoc.layer_cap, assoc.assoc,
                     assoc.assoc_cap, assoc.layer2_cap, MAC2STR(record.bssid),
                     record.primary, record.rssi, MAC2STR(assoc.mesh_id));
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

    if (parent_found) {
        /*
         * parent
         * Both channel and ssid of the parent are mandatory.
         */
        wifi_config_t parent = { 0, };
        parent.sta.channel = parent_record.primary;
        memcpy(&parent.sta.ssid, &parent_record.ssid,
               sizeof(parent_record.ssid));
        parent.sta.bssid_set = 1;
        memcpy(&parent.sta.bssid, parent_record.bssid, 6);
        if (my_type == MESH_ROOT) {
            ESP_LOGW(MESH_TAG, "<PARENT>%s, "MACSTR", channel:%u, rssi:%d",
                     parent_record.ssid, MAC2STR(parent_record.bssid),
                     parent_record.primary, parent_record.rssi);
        } else {
            ESP_LOGW(MESH_TAG,
                     "<PARENT>%s, layer:%d/%d, assoc:%d/%d, %d, "MACSTR", channel:%u, rssi:%d",
                     parent_record.ssid, parent_assoc.layer,
                     parent_assoc.layer_cap, parent_assoc.assoc,
                     parent_assoc.assoc_cap, parent_assoc.layer2_cap,
                     MAC2STR(parent_record.bssid), parent_record.primary,
                     parent_record.rssi);
        }
        ESP_ERROR_CHECK(esp_mesh_set_parent(&parent, (mesh_addr_t*)&parent_assoc.mesh_id, my_type, my_layer));

    } else {
        ESP_LOGW(MESH_TAG,
                 "no parent found, enable self-organized networking.");
        ESP_ERROR_CHECK(esp_mesh_set_self_organized(1, 1));
    }
}

void mesh_event_handler(mesh_event_t event)
{
	mesh_addr_t id = {0,};
    static uint8_t last_layer = 0;
    ESP_LOGD(MESH_TAG, "esp_event_handler:%d", event.id);

    switch (event.id) {
    case MESH_EVENT_STARTED:
    	esp_mesh_get_id(&id);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STARTED>ID:"MACSTR"", MAC2STR(id.addr));
        mesh_layer = esp_mesh_get_layer();
        ESP_ERROR_CHECK(esp_mesh_set_self_organized(0, 0));
        ESP_ERROR_CHECK(esp_wifi_scan_stop());
        wifi_scan_config_t scan_config = { 0 };
        /* mesh softAP is hidden */
        scan_config.show_hidden = 1;
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, 0));
        break;
    case MESH_EVENT_STOPPED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOPPED>");
        mesh_layer = esp_mesh_get_layer();
        break;
    case MESH_EVENT_CHILD_CONNECTED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, "MACSTR"",
                 event.info.child_connected.aid,
                 MAC2STR(event.info.child_connected.mac));
        break;
    case MESH_EVENT_CHILD_DISCONNECTED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_DISCONNECTED>aid:%d, "MACSTR"",
                 event.info.child_disconnected.aid,
                 MAC2STR(event.info.child_disconnected.mac));
        break;
    case MESH_EVENT_ROUTING_TABLE_ADD:
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d",
                 event.info.routing_table.rt_size_change,
                 event.info.routing_table.rt_size_new);
        break;
    case MESH_EVENT_ROUTING_TABLE_REMOVE:
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_REMOVE>remove %d, new:%d",
                 event.info.routing_table.rt_size_change,
                 event.info.routing_table.rt_size_new);
        break;
    case MESH_EVENT_NO_PARENT_FOUND:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_NO_PARENT_FOUND>scan times:%d",
                 event.info.no_parent.scan_times);
        /* TODO handler for the failure */
        break;
    case MESH_EVENT_PARENT_CONNECTED:
    	esp_mesh_get_id(&id);
        mesh_layer = event.info.connected.self_layer;
        memcpy(&mesh_parent_addr.addr, event.info.connected.connected.bssid, 6);
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:"MACSTR"%s, ID:"MACSTR"",
                 last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
                 esp_mesh_is_root() ? "<ROOT>" :
                 (mesh_layer == 2) ? "<layer2>" : "", MAC2STR(id.addr));
        last_layer = mesh_layer;
        mesh_connected_indicator(mesh_layer);
        if (esp_mesh_is_root()) {
            tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
        }
        break;
    case MESH_EVENT_PARENT_DISCONNECTED:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d",
                 event.info.disconnected.reason);
        mesh_disconnected_indicator();
        mesh_layer = esp_mesh_get_layer();
        break;
    case MESH_EVENT_LAYER_CHANGE:
        mesh_layer = event.info.layer_change.new_layer;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_LAYER_CHANGE>layer:%d-->%d%s",
                 last_layer, mesh_layer,
                 esp_mesh_is_root() ? "<ROOT>" :
                 (mesh_layer == 2) ? "<layer2>" : "");
        last_layer = mesh_layer;
        mesh_connected_indicator(mesh_layer);
        break;
    case MESH_EVENT_ROOT_ADDRESS:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:"MACSTR"",
                 MAC2STR(event.info.root_addr.addr));
        break;
    case MESH_EVENT_ROOT_GOT_IP:
        /* root starts to connect to server */
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_GOT_IP>sta ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR,
                 IP2STR(&event.info.got_ip.ip_info.ip),
                 IP2STR(&event.info.got_ip.ip_info.netmask),
                 IP2STR(&event.info.got_ip.ip_info.gw));
        break;
    case MESH_EVENT_ROOT_LOST_IP:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_LOST_IP>");
        break;
    case MESH_EVENT_VOTE_STARTED:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_VOTE_STARTED>attempts:%d, reason:%d, rc_addr:"MACSTR"",
                 event.info.vote_started.attempts,
                 event.info.vote_started.reason,
                 MAC2STR(event.info.vote_started.rc_addr.addr));
        break;
    case MESH_EVENT_VOTE_STOPPED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_VOTE_STOPPED>");
        break;
    case MESH_EVENT_ROOT_SWITCH_REQ:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_SWITCH_REQ>reason:%d, rc_addr:"MACSTR"",
                 event.info.switch_req.reason,
                 MAC2STR( event.info.switch_req.rc_addr.addr));
        break;
    case MESH_EVENT_ROOT_SWITCH_ACK:
        /* new root */
        mesh_layer = esp_mesh_get_layer();
        esp_mesh_get_parent_bssid(&mesh_parent_addr);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_SWITCH_ACK>layer:%d, parent:"MACSTR"", mesh_layer, MAC2STR(mesh_parent_addr.addr));
        break;
    case MESH_EVENT_TODS_STATE:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d",
                 event.info.toDS_state);
        break;
    case MESH_EVENT_ROOT_FIXED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_FIXED>%s",
                 event.info.root_fixed.is_fixed ? "fixed" : "not fixed");
        break;
    case MESH_EVENT_ROOT_ASKED_YIELD:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_ASKED_YIELD>"MACSTR", rssi:%d, capacity:%d",
                 MAC2STR(event.info.root_conflict.addr),
                 event.info.root_conflict.rssi,
                 event.info.root_conflict.capacity);
        break;
    case MESH_EVENT_CHANNEL_SWITCH:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHANNEL_SWITCH>");
        break;
    case MESH_EVENT_SCAN_DONE:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_SCAN_DONE>number:%d",
                 event.info.scan_done.number);
        mesh_scan_done_handler(event.info.scan_done.number);
        break;
    default:
        ESP_LOGI(MESH_TAG, "unknown id:%d", event.id);
        break;
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(mesh_light_init());
    ESP_ERROR_CHECK(nvs_flash_init());
    /*  tcpip initialization */
    tcpip_adapter_init();
    /* for mesh
     * stop DHCP server on softAP interface by default
     * stop DHCP client on station interface by default
     * */
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));
#if 0
    /* static ip settings */
    tcpip_adapter_ip_info_t sta_ip;
    sta_ip.ip.addr = ipaddr_addr("192.168.1.102");
    sta_ip.gw.addr = ipaddr_addr("192.168.1.1");
    sta_ip.netmask.addr = ipaddr_addr("255.255.255.0");
    tcpip_adapter_set_ip_info(WIFI_IF_STA, &sta_ip);
#endif
    /*  wifi initialization */
    ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_start());
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(WIFI_AUTH_OPEN));
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
    /* mesh ID */
    memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);
    /* mesh event callback */
    cfg.event_cb = &mesh_event_handler;
    /* router */
    cfg.channel = CONFIG_MESH_CHANNEL;
    cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
    memcpy((uint8_t *) &cfg.router.ssid, CONFIG_MESH_ROUTER_SSID, cfg.router.ssid_len);
    memcpy((uint8_t *) &cfg.router.password, CONFIG_MESH_ROUTER_PASSWD,
           strlen(CONFIG_MESH_ROUTER_PASSWD));
    /* mesh softAP */
    cfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;
    memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
           strlen(CONFIG_MESH_AP_PASSWD));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
    /* mesh start */
    ESP_ERROR_CHECK(esp_mesh_start());
    ESP_LOGI(MESH_TAG, "mesh starts successfully, heap:%d, %s\n",  esp_get_free_heap_size(),
             esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed");
}
