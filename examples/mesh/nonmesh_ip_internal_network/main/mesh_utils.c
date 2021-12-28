#include <string.h>
#include <esp_console.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "nvs_flash.h"
#include "mesh_netif.h"

static const char *TAG = "mesh_handler";
static mesh_addr_t mesh_parent_addr;
static int mesh_layer = -1;

void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    mesh_addr_t id = {0,};
    static uint8_t last_layer = 0;

    switch (event_id) {
        case MESH_EVENT_STARTED: {
            esp_mesh_get_id(&id);
            ESP_LOGI(TAG, "<MESH_EVENT_MESH_STARTED>ID:"MACSTR"", MAC2STR(id.addr));
            mesh_layer = esp_mesh_get_layer();
        }
            break;
        case MESH_EVENT_STOPPED: {
            ESP_LOGI(TAG, "<MESH_EVENT_STOPPED>");
            mesh_layer = esp_mesh_get_layer();
        }
            break;
        case MESH_EVENT_CHILD_CONNECTED: {
            mesh_event_child_connected_t *child_connected = (mesh_event_child_connected_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, "MACSTR"",
                    child_connected->aid,
                    MAC2STR(child_connected->mac));
        }
            break;
        case MESH_EVENT_CHILD_DISCONNECTED: {
            mesh_event_child_disconnected_t *child_disconnected = (mesh_event_child_disconnected_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_CHILD_DISCONNECTED>aid:%d, "MACSTR"",
                    child_disconnected->aid,
                    MAC2STR(child_disconnected->mac));
        }
            break;
        case MESH_EVENT_ROUTING_TABLE_ADD: {
            mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *)event_data;
            ESP_LOGW(TAG, "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d",
                     routing_table->rt_size_change,
                     routing_table->rt_size_new);
        }
            break;
        case MESH_EVENT_ROUTING_TABLE_REMOVE: {
            mesh_event_routing_table_change_t *routing_table = (mesh_event_routing_table_change_t *)event_data;
            ESP_LOGW(TAG, "<MESH_EVENT_ROUTING_TABLE_REMOVE>remove %d, new:%d",
                     routing_table->rt_size_change,
                     routing_table->rt_size_new);
        }
            break;
        case MESH_EVENT_NO_PARENT_FOUND: {
            mesh_event_no_parent_found_t *no_parent = (mesh_event_no_parent_found_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_NO_PARENT_FOUND>scan times:%d",
                     no_parent->scan_times);
        }
            /* TODO handler for the failure */
            break;
        case MESH_EVENT_PARENT_CONNECTED: {
            mesh_event_connected_t *connected = (mesh_event_connected_t *)event_data;
            esp_mesh_get_id(&id);
            mesh_layer = connected->self_layer;
            memcpy(&mesh_parent_addr.addr, connected->connected.bssid, 6);
            ESP_LOGI(TAG,
                     "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:"MACSTR"%s, ID:"MACSTR"",
                    last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
                    esp_mesh_is_root() ? "<ROOT>" :
                    (mesh_layer == 2) ? "<layer2>" : "", MAC2STR(id.addr));
            last_layer = mesh_layer;
            mesh_netifs_start(esp_mesh_is_root());
        }
            break;
        case MESH_EVENT_PARENT_DISCONNECTED: {
            mesh_event_disconnected_t *disconnected = (mesh_event_disconnected_t *)event_data;
            ESP_LOGI(TAG,
                     "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d",
                     disconnected->reason);
            mesh_layer = esp_mesh_get_layer();
            mesh_netifs_stop();
        }
            break;
        case MESH_EVENT_LAYER_CHANGE: {
            mesh_event_layer_change_t *layer_change = (mesh_event_layer_change_t *)event_data;
            mesh_layer = layer_change->new_layer;
            ESP_LOGI(TAG, "<MESH_EVENT_LAYER_CHANGE>layer:%d-->%d%s",
                     last_layer, mesh_layer,
                     esp_mesh_is_root() ? "<ROOT>" :
                     (mesh_layer == 2) ? "<layer2>" : "");
            last_layer = mesh_layer;
        }
            break;
        case MESH_EVENT_ROOT_ADDRESS: {
            mesh_event_root_address_t *root_addr = (mesh_event_root_address_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:"MACSTR"",
                    MAC2STR(root_addr->addr));
        }
            break;
        case MESH_EVENT_VOTE_STARTED: {
            mesh_event_vote_started_t *vote_started = (mesh_event_vote_started_t *)event_data;
            ESP_LOGI(TAG,
                     "<MESH_EVENT_VOTE_STARTED>attempts:%d, reason:%d, rc_addr:"MACSTR"",
                    vote_started->attempts,
                    vote_started->reason,
                    MAC2STR(vote_started->rc_addr.addr));
        }
            break;
        case MESH_EVENT_VOTE_STOPPED: {
            ESP_LOGI(TAG, "<MESH_EVENT_VOTE_STOPPED>");
            break;
        }
        case MESH_EVENT_ROOT_SWITCH_REQ: {
            mesh_event_root_switch_req_t *switch_req = (mesh_event_root_switch_req_t *)event_data;
            ESP_LOGI(TAG,
                     "<MESH_EVENT_ROOT_SWITCH_REQ>reason:%d, rc_addr:"MACSTR"",
                    switch_req->reason,
                    MAC2STR( switch_req->rc_addr.addr));
        }
            break;
        case MESH_EVENT_ROOT_SWITCH_ACK: {
            /* new root */
            mesh_layer = esp_mesh_get_layer();
            esp_mesh_get_parent_bssid(&mesh_parent_addr);
            ESP_LOGI(TAG, "<MESH_EVENT_ROOT_SWITCH_ACK>layer:%d, parent:"MACSTR"", mesh_layer, MAC2STR(mesh_parent_addr.addr));
        }
            break;
        case MESH_EVENT_TODS_STATE: {
            mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
        }
            break;
        case MESH_EVENT_ROOT_FIXED: {
            mesh_event_root_fixed_t *root_fixed = (mesh_event_root_fixed_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_ROOT_FIXED>%s",
                     root_fixed->is_fixed ? "fixed" : "not fixed");
        }
            break;
        case MESH_EVENT_ROOT_ASKED_YIELD: {
            mesh_event_root_conflict_t *root_conflict = (mesh_event_root_conflict_t *)event_data;
            ESP_LOGI(TAG,
                     "<MESH_EVENT_ROOT_ASKED_YIELD>"MACSTR", rssi:%d, capacity:%d",
                    MAC2STR(root_conflict->addr),
                    root_conflict->rssi,
                    root_conflict->capacity);
        }
            break;
        case MESH_EVENT_CHANNEL_SWITCH: {
            mesh_event_channel_switch_t *channel_switch = (mesh_event_channel_switch_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_CHANNEL_SWITCH>new channel:%d", channel_switch->channel);
        }
            break;
        case MESH_EVENT_SCAN_DONE: {
            mesh_event_scan_done_t *scan_done = (mesh_event_scan_done_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_SCAN_DONE>number:%d",
                     scan_done->number);
        }
            break;
        case MESH_EVENT_NETWORK_STATE: {
            mesh_event_network_state_t *network_state = (mesh_event_network_state_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_NETWORK_STATE>is_rootless:%d",
                     network_state->is_rootless);
        }
            break;
        case MESH_EVENT_STOP_RECONNECTION: {
            ESP_LOGI(TAG, "<MESH_EVENT_STOP_RECONNECTION>");
        }
            break;
        case MESH_EVENT_FIND_NETWORK: {
            mesh_event_find_network_t *find_network = (mesh_event_find_network_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_FIND_NETWORK>new channel:%d, router BSSID:"MACSTR"",
                    find_network->channel, MAC2STR(find_network->router_bssid));
        }
            break;
        case MESH_EVENT_ROUTER_SWITCH: {
            mesh_event_router_switch_t *router_switch = (mesh_event_router_switch_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_ROUTER_SWITCH>new router:%s, channel:%d, "MACSTR"",
                    router_switch->ssid, router_switch->channel, MAC2STR(router_switch->bssid));
        }
            break;
        default:
            ESP_LOGI(TAG, "unknown id:%d", event_id);
            break;
    }
}
