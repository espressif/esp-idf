/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "lwip/opt.h"
#include "protocol_examples_common.h"
#include "time_sync.h"
#include "dns_over_https.h"
#if defined(CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE) && defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE)
#include "esp_crt_bundle.h"
#endif

#if defined(CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE) && !defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE)
#error "CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE is enabled, but CONFIG_MBEDTLS_CERTIFICATE_BUNDLE is not enabled. Please enable CONFIG_MBEDTLS_CERTIFICATE_BUNDLE."
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN    INET_ADDRSTRLEN
#endif

#ifdef CONFIG_HTTPS_DNS_SERVER_GOOGLE
#define HTTPS_DNS_SERVER CONFIG_HTTPS_DNS_SERVER_URL_GOOGLE
#define HTTPS_DNS_SERVICE_PATH CONFIG_HTTPS_DNS_SERVICE_PATH_GOOGLE
#if !defined(CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE)
extern const char server_root_cert_pem_start[] asm("_binary_cert_google_root_pem_start");
extern const char server_root_cert_pem_end[]   asm("_binary_cert_google_root_pem_end");
#endif /* CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE */

#elif CONFIG_HTTPS_DNS_SERVER_CLOUDFLARE
#define HTTPS_DNS_SERVER CONFIG_HTTPS_DNS_SERVER_URL_CLOUDFLARE
#define HTTPS_DNS_SERVICE_PATH CONFIG_HTTPS_DNS_SERVICE_PATH_CLOUDFLARE
const char *server_root_cert_pem_start = NULL;
const char *server_root_cert_pem_end = NULL;

#elif CONFIG_HTTPS_DNS_SERVER_CUSTOM
#define HTTPS_DNS_SERVER CONFIG_HTTPS_DNS_SERVER_URL_CUSTOM
#define HTTPS_DNS_SERVICE_PATH CONFIG_HTTPS_DNS_SERVICE_PATH_CUSTOM
#if !defined(CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE)
extern const char server_root_cert_pem_start[] asm("_binary_cert_custom_root_pem_start");
extern const char server_root_cert_pem_end[]   asm("_binary_cert_custom_root_pem_end");
#endif /* CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE */
#endif

static const char *TAG = "example_dns_over_https";

static void do_getaddrinfo(char *hostname, int family)
{
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    void *addr = NULL;
    char *ipver = NULL;

    /* Initialize the hints structure */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM; /* TCP stream sockets */

    /* Get address information */
    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        ESP_LOGE(TAG, "getaddrinfo error: %d", status);
        goto cleanup;
    }

    ESP_LOGI(TAG, "Using DNS Over HTTPS server: %s", HTTPS_DNS_SERVER);
    ESP_LOGI(TAG, "Resolving IP addresses for %s:", hostname);

    /* Loop through all the results */
    for (p = res; p != NULL; p = p->ai_next) {

        /* Get pointer to the address itself */
#if defined(CONFIG_LWIP_IPV4)
        if (p->ai_family == AF_INET) { /* IPv4 */
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";

            /* Convert the IP to a string and print it */
            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            ESP_LOGI(TAG, "%s: %s", ipver, ipstr);
        }
#endif
#if defined(CONFIG_LWIP_IPV6)
        if (p->ai_family == AF_INET6) { /* IPv6 */
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";

            /* Convert the IP to a string and print it */
            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            ESP_LOGI(TAG, "%s: %s", ipver, ipstr);
        }
#endif
    }
    printf("\n");

cleanup:
    freeaddrinfo(res); /* Free the linked list */
}

static void addr_info_task(void *pvParameters)
{
    do_getaddrinfo("yahoo.com", AF_INET);
    do_getaddrinfo("yahoo.com", AF_UNSPEC);
    do_getaddrinfo("www.google.com", AF_INET6);
    do_getaddrinfo("www.google.com", AF_UNSPEC);
    do_getaddrinfo("0.0.0.0", AF_UNSPEC);
    do_getaddrinfo("fe80:0000:0000:0000:5abf:25ff:fee0:4100", AF_UNSPEC);

    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_err_t ret = nvs_flash_init();   /* Initialize NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Enables periodic time synchronization required for certificate expiry validation */
#ifdef CONFIG_MBEDTLS_HAVE_TIME_DATE
    setup_periodic_time_updates();
#endif

    /* Initialize the DOH config */
    dns_over_https_config_t config = {
        .dns_server = HTTPS_DNS_SERVER,
        .dns_service_path = HTTPS_DNS_SERVICE_PATH,
#if defined(CONFIG_HTTPS_DNS_ESP_CERT_BUNDLE)
        .crt_bundle_attach = esp_crt_bundle_attach,
#else
        .cert_pem = server_root_cert_pem_start,
#endif
    };
    ESP_ERROR_CHECK(dns_over_https_init(&config));

    xTaskCreate(addr_info_task, "AddressInfo", 4 * 1024, NULL, 5, NULL);
}
