/* Non-Mesh IP Internal Communication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <esp_console.h>
#include <lwip/lwip_napt.h>
#include <lwip/netif.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "nvs_flash.h"
#include "mesh_netif.h"

/*******************************************************
 *                Macros
 *******************************************************/

/*******************************************************
 *                Constants
 *******************************************************/
static const char *MESH_TAG = "mesh_main";
static const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x76};

/*******************************************************
 *                Variable Definitions
 *******************************************************/
static esp_ip4_addr_t s_current_ip;
static esp_console_repl_t *s_repl = NULL;

/*******************************************************
 *                Function Definitions
 *******************************************************/
void register_ping_command(void);
void mesh_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void register_leaf_command();

void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    ESP_LOGI(MESH_TAG, "<IP_EVENT_STA_GOT_IP>IP:" IPSTR, IP2STR(&event->ip_info.ip));
    s_current_ip.addr = event->ip_info.ip.addr;
#if !CONFIG_MESH_USE_GLOBAL_DNS_IP
    esp_netif_t *netif = event->esp_netif;
    esp_netif_dns_info_t dns;
    ESP_ERROR_CHECK(esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns));
    mesh_netif_start_root_ap(esp_mesh_is_root(), dns.ip.u_addr.ip4.addr);
#endif
}

static int do_cmd_quit(int argc, char **argv)
{
    ESP_LOGI(MESH_TAG, "BYE BYE\r\n");
    s_repl->del(s_repl);
    return 0;
}

static void register_quit_command(void)
{
    esp_console_cmd_t quit_command = {
            .command = "quit",
            .help = "Quit REPL environment",
            .func = &do_cmd_quit
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&quit_command));
}

static void print_stats(void* args){
    while(1){
        uint8_t apmac[MAC_ADDR_LEN];
        uint8_t stamac[MAC_ADDR_LEN];
        esp_wifi_get_mac(WIFI_IF_AP, apmac);
        esp_wifi_get_mac(WIFI_IF_STA, stamac);
        ESP_LOGI(MESH_TAG, "[%s] LAYER: %1d | TYPE: %1d | MESH_SSID: ESPM_%2X%2X%2X | IP:" IPSTR " | AP: " MACSTR " | STA: " MACSTR,
                 esp_mesh_is_root() ? "ROOT" : "NODE", esp_mesh_get_layer(), esp_mesh_get_type(),
                 stamac[3], stamac[4], stamac[5],
                 IP2STR(&s_current_ip), MAC2STR(apmac), MAC2STR(stamac));
        struct netif *netif;
        NETIF_FOREACH(netif) {
            ESP_LOGI(MESH_TAG, "Interface %s, NAPT: %d, IPv4: " IPSTR, netif->name, netif->napt, IP2STR(&netif->ip_addr.u_addr.ip4));
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    /*  tcpip initialization */
    ESP_ERROR_CHECK(esp_netif_init());
    /*  event initialization */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /*  crete network interfaces for mesh (only station instance saved for further manipulation, soft AP instance ignored */
    ESP_ERROR_CHECK(mesh_netifs_init());

    /*  wifi initialization */
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_MESH_MAX_LAYER));
    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
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
    ESP_LOGI(MESH_TAG, "mesh starts successfully, heap:%d, %s\n",  esp_get_free_heap_size(),
             esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed");

    // add some info
    xTaskCreate(print_stats, "print_stats", 3072, NULL, 5, NULL);


    // configure console
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    // install console REPL environment
    #if CONFIG_ESP_CONSOLE_UART
        esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));
    #endif
    register_quit_command();
    register_ping_command();
    register_leaf_command();


    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(s_repl));
}
