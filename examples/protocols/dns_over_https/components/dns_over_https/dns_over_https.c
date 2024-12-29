/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_tls.h"
#include "sdkconfig.h"
#include "lwip/prot/dns.h"
#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwip/dns.h"
#include "lwip_default_hooks.h"
#include "esp_http_client.h"
#include "dns_utils.h"
#include "dns_over_https.h"

#define SERVER_URL_MAX_SZ 256
#define BUFFER_SIZE 512

static const char *TAG = "dns_over_https";
static uint8_t buffer_qry_g[BUFFER_SIZE];

/* Buffer structure to store incoming chunks */
typedef struct {
    char *buffer;
    int length;
} response_buffer_t;

/* Initialize a global or context-specific buffer */
static response_buffer_t dns_response_buffer = { .buffer = NULL, .length = 0 };
static dns_response_t dns_response_g;

/* Global variable to store the configuration */
static dns_over_https_config_t server_config_g;

/* Initializes the DNS over HTTPS (DoH) component with the given configuration. */
esp_err_t dns_over_https_init(const dns_over_https_config_t *config)
{
    /* Validate configuration: Either a certificate bundle or a PEM certificate must be provided */
    if ((!config->crt_bundle_attach) && (!config->cert_pem)) {
        ESP_LOGE(TAG, "Error: Root certificates must be provided when not using internal certificate bundle.");
        return ESP_FAIL; /* Return failure code */
    }

    /*
     * Copy the values from the provided configuration structure
     * to the global server configuration structure.
     */
    server_config_g.dns_server = config->dns_server;               /* Set the DNS server address */
    server_config_g.dns_service_path = config->dns_service_path;   /* Set the DNS service path */
    server_config_g.cert_pem = config->cert_pem;                   /* Set the PEM certificate pointer */
    server_config_g.crt_bundle_attach = config->crt_bundle_attach; /* Set the certificate bundle attach function */

    return ESP_OK; /* Return success code */
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        /* Check if buffer is null, if yes, initialize it */
        if (dns_response_buffer.buffer == NULL) {
            dns_response_buffer.buffer = malloc(evt->data_len);
            dns_response_buffer.length = evt->data_len;
            memcpy(dns_response_buffer.buffer, evt->data, evt->data_len);
        } else {
            /* Reallocate buffer to hold the new data chunk */
            int new_len = dns_response_buffer.length + evt->data_len;
            dns_response_buffer.buffer = realloc(dns_response_buffer.buffer, new_len);
            memcpy(dns_response_buffer.buffer + dns_response_buffer.length, evt->data, evt->data_len);
            dns_response_buffer.length = new_len;
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        /* Entire response received, process it here */
        ESP_LOGD(TAG, "Received full response, length: %d", dns_response_buffer.length);

        /* Check if the buffer indicates an HTTP error response */
        if (HttpStatus_Ok == esp_http_client_get_status_code(evt->client)) {
            /* Parse the DNS response */
            parse_dns_response((uint8_t *)dns_response_buffer.buffer, dns_response_buffer.length, &dns_response_g);
        } else {
            ESP_LOGE(TAG, "HTTP Error: %d", esp_http_client_get_status_code(evt->client));
            ESP_LOG_BUFFER_HEXDUMP(TAG, dns_response_buffer.buffer, dns_response_buffer.length, ESP_LOG_ERROR);
            dns_response_g.status_code = ERR_VAL;
        }

        free(dns_response_buffer.buffer);
        dns_response_buffer.buffer = NULL;
        dns_response_buffer.length = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGE(TAG, "HTTP_EVENT_REDIRECT: Not supported(%d)", esp_http_client_get_status_code(evt->client));
        break;
    }
    return ESP_OK;
}

/**
 * @brief Converts a dns_response_t to an array of IP addresses.
 *
 * This function iterates over the DNS response and extracts valid
 * IPv4 and IPv6 addresses, storing them in the provided array.
 *
 * @param response The DNS response to process.
 * @param ipaddr An array to store the extracted IP addresses.
 *
 * @return err Status of dns response parsing
 */
static err_t write_ip_addresses_from_dns_response(const dns_response_t *response, ip_addr_t ipaddr[])
{
    int count = 0;
    memset(ipaddr, 0, DNS_MAX_HOST_IP * sizeof(ip_addr_t));

    if (response->status_code != ERR_OK) {
        return response->status_code;
    }

    /* Iterate over the DNS answers */
    for (int i = 0; i < response->num_answers && count < DNS_MAX_HOST_IP; i++) {
        const dns_answer_storage_t *answer = &response->answers[i];

        /* Check if the answer is valid */
        if (answer->status != ERR_OK) {
            continue;
        }

        ipaddr[count] = answer->ip;
        count++;
    }

    if (count == 0) {
        return ERR_VAL;
    }

    /* Store the number of valid IP addresses */
    return ERR_OK;
}

/**
 * @brief Function to handle the HTTPS request for DNS resolution.
 *
 * This function generates a DNS request, sends it via HTTPS, and processes
 * the response to extract IP addresses.
 *
 * @param name The name to resolve.
 * @param addr A pointer to an array to store the resolved IP addresses.
 * @param addrtypestr The address RR type (A or AAAA).
 */
static err_t do_https_request(const char *name, ip_addr_t *addr, int addrtype)
{
    err_t err = ERR_OK;
    const char *prefix = "https://";

    size_t url_len = strlen(prefix) + \
                     strlen(server_config_g.dns_server) + 1 + \
                     strlen(server_config_g.dns_service_path) + 1;  /* 1 for '/' and 1 for '\0' */

    char *dns_server_url = malloc(url_len);
    if (dns_server_url == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed");
        return ERR_MEM;
    }

    snprintf(dns_server_url, url_len, "%s%s/%s", prefix,
             server_config_g.dns_server,
             server_config_g.dns_service_path);

    esp_http_client_config_t config = {
        .url = dns_server_url,
        .event_handler = _http_event_handler,
        .method = HTTP_METHOD_POST,
    };

    if (server_config_g.crt_bundle_attach) {
        config.crt_bundle_attach = server_config_g.crt_bundle_attach;
    } else {
        config.cert_pem = server_config_g.cert_pem;  /* Use the root certificate for dns.google if needed */
    }

    /* Generate DNS request string */
    size_t query_size = create_dns_query(buffer_qry_g, sizeof(buffer_qry_g), name, addrtype, &dns_response_g.id);
    if (query_size == -1) {
        ESP_LOGE(TAG, "Error: Hostname too big");
        err = ERR_MEM;
        goto cleanup;
    }

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Error initializing HTTP client");
        err = ERR_VAL;
        goto cleanup;
    }

    /* Set headers for DoH request */
    esp_err_t ret = esp_http_client_set_header(client, "Content-Type", "application/dns-message");
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error setting HTTP header: %s", esp_err_to_name(ret));
        err = ERR_VAL;
        goto client_cleanup;
    }

    /* Set DNS query payload */
    ret = esp_http_client_set_post_field(client, (const char *)buffer_qry_g, query_size);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error setting POST field: %s", esp_err_to_name(ret));
        err = ERR_VAL;
        goto client_cleanup;
    }

    /* Perform the request */
    ret = esp_http_client_perform(client);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "HTTP POST Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));

        /* Check if the response status or buffer indicates an error response */
        if ((HttpStatus_Ok != esp_http_client_get_status_code(client)) ||
                (dns_response_g.status_code != ERR_OK)) {
            err = ERR_ARG;
            goto client_cleanup;
        }

        err = write_ip_addresses_from_dns_response(&dns_response_g, addr);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(ret));
        err = ERR_VAL;
    }

client_cleanup:
    esp_http_client_cleanup(client);
cleanup:
    free(dns_server_url);

    return err;
}

#if defined(CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM)
int lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err)
{
    LWIP_UNUSED_ARG(name);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(addrtype);
    LWIP_UNUSED_ARG(err);

    if (server_config_g.dns_server == NULL) {
        ESP_LOGE(TAG, "DNS Over HTTPS Server not set. Ensure DNS Over HTTPS module was initialized");
        *err = ERR_VAL;
        return 1;
    }

    /* Check if HTTPS_DNS_SERVER is in the dns cache */
    if ((strcmp(name, server_config_g.dns_server) == 0) ||
#if LWIP_HAVE_LOOPIF
            (strcmp(name, "localhost") == 0) ||
#endif
            ipaddr_aton(name, addr)) { /* host name already in octet notation */
        return 0;
    }

    if ((addrtype == NETCONN_DNS_IPV4) || (addrtype == NETCONN_DNS_IPV4_IPV6)) {
        *err =  do_https_request(name, addr, DNS_RRTYPE_A);
    } else if ((addrtype == NETCONN_DNS_IPV6) || (addrtype == NETCONN_DNS_IPV6_IPV4)) {
        *err =  do_https_request(name, addr, DNS_RRTYPE_AAAA);
    } else {
        ESP_LOGE(TAG, "Error: Invalid address type");
        *err =  ERR_VAL;
    }

    return 1;
}
#endif /* CONFIG_LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE... */
