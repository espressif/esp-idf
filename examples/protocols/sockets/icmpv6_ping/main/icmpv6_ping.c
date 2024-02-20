/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* BSD Socket API Example */

#include <netdb.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif_ip_addr.h"
#include "nvs_flash.h"
#include "lwip/icmp6.h"
#include "lwip/ip6.h"
#include "protocol_examples_common.h"
#include "sdkconfig.h"


#define MSG_BUFF_SIZE   (256)


static const char *TAG = "icmpv6_ping";

/* types of ipv6 addresses to be displayed on ipv6 events */
static const char *s_ipv6_addr_types[] = {
    "ESP_IP6_ADDR_IS_UNKNOWN",
    "ESP_IP6_ADDR_IS_GLOBAL",
    "ESP_IP6_ADDR_IS_LINK_LOCAL",
    "ESP_IP6_ADDR_IS_SITE_LOCAL",
    "ESP_IP6_ADDR_IS_UNIQUE_LOCAL",
    "ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6"
};


/**
 * @brief Resolves an IPv6 address in string format or an url.
 *
 * @param[in] addr_str_i IPv6 address in string format.
 * @param[out] addr_o IPv6 address in sockaddr_in6 format.
 *
 * @return length of the IPv6 address.
 */
static socklen_t resolve_v6addr(char *addr_str_i, struct sockaddr_in6 *addr_o)
{
    struct addrinfo hints, *res;
    socklen_t addrlen;

    // Fill out hints for getaddrinfo() for Source address.
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = hints.ai_flags | AI_CANONNAME;

    // Resolve source using getaddrinfo().
    if (0 != getaddrinfo(addr_str_i, NULL, &hints, &res)) {
        ESP_LOGE(TAG, "getaddrinfo(): Could not resolve address, got error: %d", errno);
        return 0;
    }

    memset(addr_o, 0, sizeof(struct sockaddr_in6));
    memcpy(addr_o, res->ai_addr, res->ai_addrlen);
    addrlen = res->ai_addrlen;
    freeaddrinfo(res);

    return addrlen;
}


/**
 * @brief Send a ICMPv6 ping request to the destination address over the given interface and wait for a response.
 *
 * @param[in] src_addr_str Source Global/Unique local IPv6 address of the interface.
 * @param[in] dst_addr_str Global IPv6 address of the Destination.
 * @param[in] interface Name of the interface.
 */
static void send_ping(char *src_addr_str, char *dst_addr_str, char *interface)
{
    int len, datalen;
    int sd;
    struct sockaddr_in6 src, dst;
    struct icmp6_echo_hdr *icmphdr;
    socklen_t srclen;
    uint8_t psdhdr[MSG_BUFF_SIZE];
    uint8_t data[10] = "Test";      // ICMPv6 Payload
    uint8_t *inpack;
    struct msghdr msghdr;
    struct cmsghdr *cmsghdr;
    int cmsglen, hoplimit;
    struct iovec iov[2];
    struct ifreq ifr;

    // Reset pseudo header buffer
    memset(psdhdr, 0, MSG_BUFF_SIZE);

    // Resolve source address.
    if (0 == (srclen = resolve_v6addr(src_addr_str, &src))) {
        ESP_LOGE(TAG, "resolve_v6addr(): Source address error");
        return;
    }

    // Resolve destination address.
    if (0 == resolve_v6addr(dst_addr_str, &dst)) {
        ESP_LOGE(TAG, "resolve_v6addr(): Destination address error");
        return;
    }

    // Populate icmphdr portion of buffer outpack.
    icmphdr = (struct icmp6_echo_hdr *)(psdhdr + IP6_HLEN);
    icmphdr->type = ICMP6_TYPE_EREQ;
    icmphdr->code = 0;
    icmphdr->chksum = 0;
    icmphdr->id = htons(0);
    icmphdr->seqno = htons(300);

    // ICMP data
    datalen = 4;
    // The payload is assigned to the data buffer during initialization.

    memcpy(psdhdr + IP6_HLEN + ICMP6_HLEN, data, datalen * sizeof(uint8_t));

    // Compose the msghdr structure.
    memset(&msghdr, 0, sizeof(msghdr));
    msghdr.msg_name = &dst;           // pointer to socket address structure
    msghdr.msg_namelen = sizeof(dst); // size of socket address structure

    memset(&iov, 0, sizeof(iov));
    iov[0].iov_base = (uint8_t *)icmphdr;
    iov[0].iov_len = ICMP6_HLEN + datalen;
    msghdr.msg_iov = iov;  // scatter/gather array
    msghdr.msg_iovlen = 1; // number of elements in scatter/gather array

    // Initialize msghdr and control data to total length of the message to be sent.
    // Allocate some memory for our cmsghdr data.
    cmsglen = CMSG_SPACE(sizeof(int));
    msghdr.msg_control = (uint8_t *)malloc(cmsglen * sizeof(uint8_t));
    msghdr.msg_controllen = cmsglen;

    // Change hop limit to 255.
    hoplimit = 255;
    cmsghdr = CMSG_FIRSTHDR(&msghdr);
    cmsghdr->cmsg_level = IPPROTO_IPV6;
    cmsghdr->cmsg_type = 20; // We want to change hop limit
    cmsghdr->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *)CMSG_DATA(cmsghdr)) = hoplimit;

    // Request a socket descriptor sd.
    if ((sd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0) {
        ESP_LOGE(TAG, "Failed to get socket descriptor.: %d", errno);
        return;
    }

    // Bind the socket descriptor to the source address.
    if (bind(sd, (struct sockaddr *)&src, srclen) != 0) {
        ESP_LOGE(TAG, "Failed to bind the socket descriptor to the source address.: %d", errno);
        return;
    }

    // Bind socket to interface index.
    strcpy(ifr.ifr_name, interface);
    if (setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
        ESP_LOGE(TAG, "setsockopt() failed to bind to interface: %d", errno);
        return;
    }

    // Send packet.
    int ret = 0;
    if ((ret = sendmsg(sd, &msghdr, 0)) < 0) {
        ESP_LOGE(TAG, "sendmsg() failed: %d", errno);
        return;
    }
    free(msghdr.msg_control);

    ESP_LOGI(TAG, "ICMPv6 msg payload:");
    ESP_LOG_BUFFER_HEXDUMP(TAG, &(psdhdr[IP6_HLEN]), ICMP6_HLEN + datalen, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Sent ICMPv6 msg: type: %d, code: %d, id: %d, seqno: %d",
             icmphdr->type,
             icmphdr->code,
             icmphdr->id,
             ntohs(icmphdr->seqno));

    // Prepare msghdr for recvmsg().
    memset(psdhdr, 0, MSG_BUFF_SIZE);
    inpack = psdhdr;

    memset(&msghdr, 0, sizeof(msghdr));
    memset(&iov, 0, sizeof(iov));
    msghdr.msg_name = NULL;
    msghdr.msg_namelen = 0;
    iov[0].iov_base = (uint8_t *)inpack;
    iov[0].iov_len = 65535;
    msghdr.msg_iov = iov;
    msghdr.msg_iovlen = 1;

    msghdr.msg_control = (uint8_t *)malloc(128 * sizeof(uint8_t));
    msghdr.msg_controllen = 128 * sizeof(uint8_t);

    // Listen for incoming message from socket sd.
    // Keep at it until we get a Echo Reply.
    icmphdr = (struct icmp6_echo_hdr *)(inpack + IP6_HLEN);
    while (ICMP6_TYPE_EREP != icmphdr->type) {
        if ((len = recvmsg(sd, &msghdr, 0)) < 0) {
            ESP_LOGE(TAG, "recvmsg() failed: %d", errno);
            return;
        }

        ESP_LOGI(TAG, "ICMPv6 msg payload:");
        ESP_LOG_BUFFER_HEXDUMP(TAG, inpack, IP6_HLEN + ICMP6_HLEN + datalen, ESP_LOG_INFO);
        ESP_LOGI(TAG, "Received ICMPv6 msg: type: %d, code: %d, id: %d, seqno: %d",
                 icmphdr->type,
                 icmphdr->code,
                 icmphdr->id,
                 ntohs(icmphdr->seqno));
    }

    free(msghdr.msg_control);
}

/**
 * @brief API struct to pass interface name and source addresses in TCPIP context
 *
 * @param[out] interface Name of the interface.
 * @param[out] src_addr_str Global/Unique local IPv6 address of the interface
 */
typedef struct src_iface_api {
    char *interface;
    char *src_addr_str;
} src_iface_api_t;

/**
 * @brief Goes over each interface and searches for one Global/Unique local IPv6 address.
 *        Returns the interface name and IPv6 address of the interface in case of success.
 *
 * @return
 *          ESP_OK : Successfully found an interface with Global/Unique local IPv6 address.
 *          ESP_FAIL : Unable to to find a valid interface with Global/Unique local  IPv6 address.
 */
static esp_err_t get_src_iface(void* ctx)
{
    src_iface_api_t *api = ctx;
    esp_netif_t *netif = NULL;
    int ip6_addrs_count = 0;
    esp_ip6_addr_t ip6[LWIP_IPV6_NUM_ADDRESSES];

    // Get interface details and own global ipv6 address
    while ((netif = esp_netif_next_unsafe(netif)) != NULL) {
        esp_err_t ret = esp_netif_get_netif_impl_name(netif, api->interface);

        if ((ESP_FAIL == ret) || (NULL == netif)) {
            ESP_LOGE(TAG, "No interface available");
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Interface: %s", api->interface);

        ip6_addrs_count = esp_netif_get_all_ip6(netif, ip6);
        for (int j = 0; j < ip6_addrs_count; ++j) {
            esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&(ip6[j]));

            ESP_LOGI(TAG, "IPv6 address: " IPV6STR ", type: %s", IPV62STR(ip6[j]), s_ipv6_addr_types[ipv6_type]);
            if ((ESP_IP6_ADDR_IS_GLOBAL == ipv6_type) ||
                    (ESP_IP6_ADDR_IS_UNIQUE_LOCAL == ipv6_type)) {
                // Break as we have the source address
                sprintf(api->src_addr_str, IPV6STR, IPV62STR(ip6[j]));
                return ESP_OK;
            }
        }
    }

    return ESP_FAIL;
}


static void ping6_test_task(void *pvParameters)
{
    char src_addr_str[50];
    char interface[10];
    char dst_addr_str[] = CONFIG_EXAMPLE_DST_IPV6_ADDR;

    src_iface_api_t api = { .interface = interface, .src_addr_str = src_addr_str };
    if (esp_netif_tcpip_exec(get_src_iface, &api) == ESP_OK) {
        ESP_LOGI(TAG, "Source address: %s", src_addr_str);
        ESP_LOGI(TAG, "Destination address: %s", dst_addr_str);
        ESP_LOGI(TAG, "Interface name: %s", interface);

        // send one single ping
        send_ping(src_addr_str, dst_addr_str, interface);
    }

    vTaskDelete(NULL);
}


void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    * Read "Establishing Wi-Fi or Ethernet Connection" section in
    * examples/protocols/README.md for more information about this function.
    */

    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(ping6_test_task, "iface_test", 4096, NULL, 5, NULL);
}
