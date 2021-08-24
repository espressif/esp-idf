#include <stdio.h>
#include "mdns.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "mdns-test";

static void query_mdns_host(const char * host_name)
{
    ESP_LOGI(TAG, "Query A: %s.local", host_name);

    struct esp_ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(host_name, 2000,  &addr);
    if(err){
        if(err == ESP_ERR_NOT_FOUND){
            ESP_LOGW(TAG, "%x: Host was not found!", (err));
            return;
        }
        ESP_LOGE(TAG, "Query Failed: %x", (err));
        return;
    }

    ESP_LOGI(TAG, "Query A: %s.local resolved to: " IPSTR, host_name, IP2STR(&addr));
}

int main(int argc , char *argv[])
{

    setvbuf(stdout, NULL, _IONBF, 0);
    const esp_netif_inherent_config_t base_cg = { .if_key = "WIFI_STA_DEF", .if_desc = "eth2" };
    esp_netif_config_t cfg = { .base = &base_cg  };
    esp_netif_t *sta = esp_netif_new(&cfg);

    mdns_init();

    mdns_hostname_set("myesp");
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", "myesp");
    //set default mDNS instance name
    mdns_instance_name_set("myesp-inst");
    //structure with TXT records
    mdns_txt_item_t serviceTxtData[3] = {
            {"board","esp32"},
            {"u","user"},
            {"p","password"}
    };
    vTaskDelay(1000);
    ESP_ERROR_CHECK(mdns_service_add("myesp-service2", "_http", "_tcp", 80, serviceTxtData, 3));
    vTaskDelay(2000);

    query_mdns_host("david-comp");
    vTaskDelay(2000);
    esp_netif_destroy(sta);
    mdns_free();
    ESP_LOGI(TAG, "Exit");
    return 0;
}
