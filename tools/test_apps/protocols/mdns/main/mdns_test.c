/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "mdns.h"
#include "esp_log.h"
#include "esp_netif.h"

static const char * TAG = "MDNS_TEST_APP";
static const int RETRY_COUNT = 10;

static void mdns_print_results(mdns_result_t *results)
{
    mdns_result_t *r = results;
    mdns_ip_addr_t *a = NULL;
    int t;
    while (r) {
        if (r->instance_name) {
            printf("PTR:%s.%s.%s\n", r->instance_name, r->service_type, r->proto);
        }
        if (r->hostname) {
            printf("SRV:%s.local:%u\n", r->hostname, r->port);
        }
        if (r->txt_count) {
            printf("TXT:[%zu] ", r->txt_count);
            for (t = 0; t < r->txt_count; t++) {
                printf("%s=%s(%d); ", r->txt[t].key, r->txt[t].value ? r->txt[t].value : "NULL", r->txt_value_len[t]);
            }
            printf("\n");
        }
        a = r->addr;
        while (a) {
            if (a->addr.type == ESP_IPADDR_TYPE_V6) {
                printf("  AAAA: " IPV6STR "\n", IPV62STR(a->addr.u_addr.ip6));
            } else {
                printf("  A   : " IPSTR "\n", IP2STR(&(a->addr.u_addr.ip4)));
            }
            a = a->next;
        }
        r = r->next;
    }
}

static bool check_and_print_result(mdns_search_once_t *search)
{
    // Check if any result is available
    mdns_result_t * result = NULL;
    if (!mdns_query_async_get_results(search, 0, &result, NULL)) {
        return false;
    }

    if (!result) {   // search timeout, but no result
        return false;
    }

    // If yes, print the result
    mdns_ip_addr_t * a = result->addr;
    while (a) {
        if(a->addr.type == ESP_IPADDR_TYPE_V6){
            printf("Async query resolved to AAAA:" IPV6STR "\n", IPV62STR(a->addr.u_addr.ip6));
        } else {
            printf("Async query resolved to A:" IPSTR "\n", IP2STR(&(a->addr.u_addr.ip4)));
        }
        a = a->next;
    }
    // and free the result
    mdns_query_results_free(result);
    return true;
}

static bool query_mdns_hosts_async(const char * host_name)
{
    ESP_LOGI(TAG, "Query both A and AAA: %s.local", host_name);
    bool res = false;

    mdns_search_once_t *s_a = mdns_query_async_new(host_name, NULL, NULL, MDNS_TYPE_A, 1000, 1, NULL);
    mdns_query_async_delete(s_a);
    mdns_search_once_t *s_aaaa = mdns_query_async_new(host_name, NULL, NULL, MDNS_TYPE_AAAA, 1000, 1, NULL);
    while (s_a || s_aaaa) {
        if (s_a && check_and_print_result(s_a)) {
            ESP_LOGI(TAG, "Query A %s.local finished", host_name);
            mdns_query_async_delete(s_a);
            s_a = NULL;
            res = true;
        }
        if (s_aaaa && check_and_print_result(s_aaaa)) {
            ESP_LOGI(TAG, "Query AAAA %s.local finished", host_name);
            mdns_query_async_delete(s_aaaa);
            s_aaaa = NULL;
            res = true;
        }
    }
    return res;
}

static esp_err_t query_mdns_host(const char * host_name)
{
    ESP_LOGI(TAG, "Query A: %s.local", host_name);

    struct esp_ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(host_name, 2000,  &addr);
    if(err){
        if(err == ESP_ERR_NOT_FOUND){
            ESP_LOGW(TAG, "%s: Host was not found!", esp_err_to_name(err));
        }
        ESP_LOGE(TAG, "Query Failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Query A: %s.local resolved to: " IPSTR, host_name, IP2STR(&addr));
    return ESP_OK;
}

static esp_err_t query_mdns_service(const char * instance, const char * service_name, const char * proto)
{
    ESP_LOGI(TAG, "Query SRV: %s.%s.local", service_name, proto);

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query_srv(instance, service_name, proto, 3000,  &results);
    if(err){
        ESP_LOGE(TAG, "Query Failed: %s", esp_err_to_name(err));
        return err;
    }
    if(!results){
        ESP_LOGW(TAG, "No results found!");
    }

    mdns_print_results(results);
    mdns_query_results_free(results);
    return ESP_OK;
}

void query_mdns_service_sub_type(const char * subtype, const char * service_name, const char * proto) {
    ESP_LOGI(TAG, "Query PTR: %s.%s.local", service_name, proto);

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query_ptr(service_name, proto, 3000, 20,  &results);
    if(err){
        ESP_LOGE(TAG, "Query Failed: %s", esp_err_to_name(err));
    }
    if(!results){
        ESP_LOGW(TAG, "No results found!");
    }

    mdns_print_results(results);
    mdns_query_results_free(results);
}

void mdns_test(const char *line)
{
    char test_case[32];
    int i = 0;
    const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;

    sscanf(line, "%s", test_case);
    ESP_LOGI(TAG, "test case = %s", test_case);

    if (strcmp(test_case, "CONFIG_TEST_QUERY_HOST") == 0) {
        i = 0;
        while (query_mdns_host("tinytester") != ESP_OK && i != RETRY_COUNT) {
            query_mdns_host("tinytester");
            i++;
            vTaskDelay(xDelay);
        }
    } else if (strcmp(test_case, "CONFIG_TEST_QUERY_HOST_ASYNC") == 0) {
        i = 0;
        while (query_mdns_hosts_async("tinytester") == false && i != RETRY_COUNT) {
            query_mdns_hosts_async("tinytester");
            i++;
            vTaskDelay(xDelay);
        }
    } else if (strcmp(test_case, "CONFIG_TEST_QUERY_SERVICE") == 0) {
        i = 0;
        while (query_mdns_service("ESP32", "_http", "_tcp") != ESP_OK && i != RETRY_COUNT) {
            query_mdns_service("ESP32", "_http", "_tcp");
            i++;
            vTaskDelay(xDelay);
        }
    } else {
        ESP_LOGE(TAG, "%s: No such test case", test_case);
    }
}
