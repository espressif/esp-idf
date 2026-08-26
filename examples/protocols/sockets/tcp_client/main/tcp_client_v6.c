/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"
#if defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#include "addr_from_stdin.h"
#endif

#if defined(CONFIG_EXAMPLE_IPV6_ADDR)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG = "example";
static const char *payload = "Message from ESP32 ";

#if defined(CONFIG_IDF_TARGET_LINUX)
// Checks for Global address, Unique Unicast(RFC4193) and link-local address.
#define ip6_addr_isglobal(ip6addr) ((((ip6addr)->sin6_addr.s6_addr[0] & htonl(0xe0000000UL)) & htonl(0x20000000UL)) || \
                                    (((ip6addr)->sin6_addr.s6_addr[0] & htonl(0xff000000UL)) & htonl(0xfc000000UL)) || \
                                    (((ip6addr)->sin6_addr.s6_addr[0] & htonl(0xff000000UL)) & htonl(0xfe800000UL)))


/**
 * @brief In case of Auto mode returns the interface name with a valid IPv6 address or
 *        In case the user has specified interface, validates and returns the interface name.
 *
 * @param[out] interface Name of the interface in as a string.
 *
 * @return 0 incase of success.
 */
static int get_src_iface(char *interface)
{
    struct ifaddrs *ifap, *ifa;
    char src_addr_str[INET6_ADDRSTRLEN];

    if (getifaddrs(&ifap) == -1) {
        ESP_LOGE(TAG, "getifaddrs failed");
        return -1;
    }

    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6) {
#if defined(CONFIG_EXAMPLE_USER_SPECIFIED_IFACE)
            if (0 == strcmp(CONFIG_EXAMPLE_USER_SPECIFIED_IFACE_NAME, ifa->ifa_name)) {
                strcpy(interface, ifa->ifa_name);
                freeifaddrs(ifap);
                ESP_LOGI(TAG, "Interface: %s", interface);
                return 0;
            }
#else
            strcpy(interface, ifa->ifa_name);
            getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), src_addr_str,
                        sizeof(src_addr_str), NULL, 0, NI_NUMERICHOST);

            struct sockaddr_in6 *src_addr = (struct sockaddr_in6 *) ifa->ifa_addr;
            inet_ntop(AF_INET6, &(src_addr->sin6_addr), src_addr_str, INET6_ADDRSTRLEN);

            if (ip6_addr_isglobal(src_addr)) {
                //Return as we have the source address
                freeifaddrs(ifap);
                ESP_LOGI(TAG, "Interface: %s", interface);
                return 0;
            }
#endif    // #if defined(CONFIG_EXAMPLE_USER_SPECIFIED_IFACE)
        }
    }

    freeifaddrs(ifap);
    return -1;
}
#else

/**
 * @brief In case of Auto mode returns the interface name with a valid IPv6 address or
 *        In case the user has specified interface, validates and returns the interface name.
 *
 * This function is a predicate for esp_netif_find_if() API, and it uses the underlying
 * network interface name as a context parameter
 *
 * @return true if we found the appropriate interface, false if not
 */
static bool choose_netif(esp_netif_t *netif, void* ctx)
{
    char *interface = ctx;
    esp_ip6_addr_t ip6[LWIP_IPV6_NUM_ADDRESSES];

    esp_err_t ret = esp_netif_get_netif_impl_name(netif, interface);

    if ((ESP_FAIL == ret) || (NULL == netif)) {
        ESP_LOGE(TAG, "No interface available");
        return false;
    }
#if defined(CONFIG_EXAMPLE_USER_SPECIFIED_IFACE)
    if (!strcmp(CONFIG_EXAMPLE_USER_SPECIFIED_IFACE_NAME, interface)) {
            ESP_LOGI(TAG, "Interface: %s", interface);
            return true;
        }
#else
    int ip6_addrs_count = esp_netif_get_all_ip6(netif, ip6);
    for (int j = 0; j < ip6_addrs_count; ++j) {
        esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&(ip6[j]));

        if ((ESP_IP6_ADDR_IS_GLOBAL == ipv6_type) ||
            (ESP_IP6_ADDR_IS_UNIQUE_LOCAL == ipv6_type) ||
            (ESP_IP6_ADDR_IS_LINK_LOCAL == ipv6_type)) {
            // Break as we have the source address
            ESP_LOGI(TAG, "Interface: %s", interface);
            return true;
        }
    }
#endif    // #if defined(CONFIG_EXAMPLE_USER_SPECIFIED_IFACE)
    return false;
}

#endif    // #if defined(CONFIG_IDF_TARGET_LINUX)


void tcp_client(void)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
    char interface[10];
#if defined(CONFIG_IDF_TARGET_LINUX)
    struct ifreq ifr;
#else
    esp_netif_t *netif = NULL;
#endif

    while (1) {
#if defined(CONFIG_EXAMPLE_IPV6)
        struct sockaddr_in6 dest_addr = { 0 };
        inet_pton(AF_INET6, host_ip, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(PORT);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_TCP;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
#endif
        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

#if defined(CONFIG_IDF_TARGET_LINUX)
        if (0 != get_src_iface(interface)) {
            ESP_LOGE(TAG, "Interface: Unavailable");
            break;
        }
        memset (&ifr, 0, sizeof(ifr));
        snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
        if (ioctl (sock, SIOCGIFINDEX, &ifr) < 0) {
            ESP_LOGE(TAG, "ioctl() failed to find interface ");
            break;
        }
#if defined(CONFIG_EXAMPLE_IPV6)
        dest_addr.sin6_scope_id = ifr.ifr_ifindex;
        ESP_LOGI(TAG, "Interface index: %d", dest_addr.sin6_scope_id);
#endif
#else
        if (NULL == (netif = esp_netif_find_if(choose_netif, interface))) {
            ESP_LOGE(TAG, "Failed to find interface ");
            break;
        }
#if defined(CONFIG_EXAMPLE_IPV6)
        dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(netif);
        ESP_LOGI(TAG, "Interface index: %" PRIu32, dest_addr.sin6_scope_id);
#endif
#endif

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
            int err = send(sock, payload, strlen(payload), 0);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG, "%s", rx_buffer);
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}
