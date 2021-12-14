/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief MDNS Server Networking module implemented using BSD sockets
 */

#include <string.h>
#include "esp_event.h"
#include "mdns_networking.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include "esp_log.h"

#if defined(CONFIG_IDF_TARGET_LINUX)
#include <sys/ioctl.h>
#include <net/if.h>
#endif

extern mdns_server_t * _mdns_server;

static const char *TAG = "MDNS_Networking";
static bool s_run_sock_recv_task = false;
static int create_socket(esp_netif_t *netif);
static int join_mdns_multicast_group(int sock, esp_netif_t *netif, mdns_ip_protocol_t ip_protocol);

#if defined(CONFIG_IDF_TARGET_LINUX)
// Need to define packet buffer struct on linux
struct pbuf  {
    struct pbuf * next;
    void * payload;
    size_t tot_len;
    size_t len;
};
#else
// Compatibility define to access sock-addr struct the same way for lwip and linux
#define s6_addr32 un.u32_addr
#endif // CONFIG_IDF_TARGET_LINUX

static void delete_socket(int sock)
{
    close(sock);
}

static struct udp_pcb* sock_to_pcb(int sock)
{
    if (sock < 0) {
        return NULL;
    }
    // Note: sock=0 is a valid descriptor, so save it as +1 ("1" is a valid pointer)
    intptr_t sock_plus_one = sock + 1;
    return (struct udp_pcb*)sock_plus_one;
}

static int pcb_to_sock(struct udp_pcb* pcb)
{
    if (pcb == NULL) {
        return -1;
    }
    intptr_t sock_plus_one = (intptr_t)pcb;
    return sock_plus_one - 1;
}

void* _mdns_get_packet_data(mdns_rx_packet_t *packet)
{
    return packet->pb->payload;
}

size_t _mdns_get_packet_len(mdns_rx_packet_t *packet)
{
    return packet->pb->len;
}

void _mdns_packet_free(mdns_rx_packet_t *packet)
{
    free(packet->pb->payload);
    free(packet->pb);
    free(packet);
}

esp_err_t _mdns_pcb_deinit(mdns_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    struct udp_pcb * pcb = _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb;
    _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb = NULL;
    if (_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].pcb == NULL &&
        _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].pcb == NULL) {
        // if the interface for both protocol uninitialized, close the interface socket
        int sock = pcb_to_sock(pcb);
        if (sock >= 0) {
            delete_socket(sock);
        }
    }

    for (int i=0; i<MDNS_MAX_INTERFACES; i++) {
        for (int j=0; j<MDNS_IP_PROTOCOL_MAX; j++) {
            if (_mdns_server->interfaces[i].pcbs[j].pcb)
                // If any of the interfaces/protocol initialized
                return ESP_OK;
        }
    }

    // no interface alive, stop the rx task
    s_run_sock_recv_task = false;
    vTaskDelay(pdMS_TO_TICKS(500));
    return ESP_OK;
}

#if defined(CONFIG_IDF_TARGET_LINUX)
#ifdef CONFIG_LWIP_IPV6
static char* inet6_ntoa_r(struct in6_addr addr, char* ptr, size_t size)
{
    inet_ntop(AF_INET6, &(addr.s6_addr32[0]), ptr, size);
    return ptr;
}
#endif // CONFIG_LWIP_IPV6
static char* inet_ntoa_r(struct in_addr addr, char* ptr, size_t size)
{
    char * res = inet_ntoa(addr);
    if (res && strlen(res) < size) {
        strcpy(ptr, res);
    }
    return res;
}
#endif // CONFIG_IDF_TARGET_LINUX

static inline char* get_string_address(struct sockaddr_storage *source_addr)
{
    static char address_str[40]; // 40=(8*4+7+term) is the max size of ascii IPv6 addr "XXXX:XX...XX:XXXX"
    char *res = NULL;
    // Convert ip address to string
    if (source_addr->ss_family == PF_INET) {
        res = inet_ntoa_r(((struct sockaddr_in *)source_addr)->sin_addr, address_str, sizeof(address_str));
    }
#ifdef CONFIG_LWIP_IPV6
    else if (source_addr->ss_family == PF_INET6) {
        res = inet6_ntoa_r(((struct sockaddr_in6 *)source_addr)->sin6_addr, address_str, sizeof(address_str));
    }
#endif
    if (!res) {
        address_str[0] = '\0'; // Returns empty string if conversion didn't succeed
    }
    return address_str;
}


static inline size_t espaddr_to_inet(const esp_ip_addr_t *addr, const uint16_t port, const mdns_ip_protocol_t ip_protocol, struct sockaddr_storage *in_addr)
{
    size_t ss_addr_len = 0;
    memset(in_addr, 0, sizeof(struct sockaddr_storage));
    if (ip_protocol == MDNS_IP_PROTOCOL_V4 && addr->type == ESP_IPADDR_TYPE_V4) {
        in_addr->ss_family = PF_INET;
#if !defined(CONFIG_IDF_TARGET_LINUX)
        in_addr->s2_len = sizeof(struct sockaddr_in);
#endif
        ss_addr_len = sizeof(struct sockaddr_in);
        struct sockaddr_in *in_addr_ip4 = (struct sockaddr_in *) in_addr;
        in_addr_ip4->sin_port = port;
        in_addr_ip4->sin_addr.s_addr = addr->u_addr.ip4.addr;
    }
#if CONFIG_LWIP_IPV6
    else if (ip_protocol == MDNS_IP_PROTOCOL_V6 && addr->type == ESP_IPADDR_TYPE_V6) {
        memset(in_addr, 0, sizeof(struct sockaddr_storage));
        in_addr->ss_family = PF_INET6;
#if !defined(CONFIG_IDF_TARGET_LINUX)
        in_addr->s2_len = sizeof(struct sockaddr_in6);
#endif
        ss_addr_len = sizeof(struct sockaddr_in6);
        struct sockaddr_in6 * in_addr_ip6 = (struct sockaddr_in6 *)in_addr;
        uint32_t *u32_addr = in_addr_ip6->sin6_addr.s6_addr32;
        in_addr_ip6->sin6_port = port;
        u32_addr[0] = addr->u_addr.ip6.addr[0];
        u32_addr[1] = addr->u_addr.ip6.addr[1];
        u32_addr[2] = addr->u_addr.ip6.addr[2];
        u32_addr[3] = addr->u_addr.ip6.addr[3];
    }
#endif // CONFIG_LWIP_IPV6
    return ss_addr_len;
}

size_t _mdns_udp_pcb_write(mdns_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, const esp_ip_addr_t *ip, uint16_t port, uint8_t * data, size_t len)
{
    int sock = pcb_to_sock(_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb);
    if (sock < 0) {
        return 0;
    }
    struct sockaddr_storage in_addr;
    size_t ss_size = espaddr_to_inet(ip, htons(port), ip_protocol, &in_addr);
    if (!ss_size) {
        ESP_LOGE(TAG, "espaddr_to_inet() failed: Mismatch of IP protocols");
        return 0;
    }
    ESP_LOGD(TAG, "[sock=%d]: Sending to IP %s port %d", sock, get_string_address(&in_addr), port);
    ssize_t actual_len = sendto(sock, data, len, 0, (struct sockaddr *)&in_addr, ss_size);
    if (actual_len < 0) {
        ESP_LOGE(TAG, "[sock=%d]: _mdns_udp_pcb_write sendto() has failed\n errno=%d: %s", sock, errno, strerror(errno));
    }
    return actual_len;
}

static inline void inet_to_espaddr(const struct sockaddr_storage *in_addr, esp_ip_addr_t *addr, uint16_t *port)
{
    if (in_addr->ss_family == PF_INET) {
        struct sockaddr_in * in_addr_ip4 = (struct sockaddr_in *)in_addr;
        memset(addr, 0, sizeof(esp_ip_addr_t));
        *port = in_addr_ip4->sin_port;
        addr->u_addr.ip4.addr = in_addr_ip4->sin_addr.s_addr;
        addr->type = ESP_IPADDR_TYPE_V4;
    }
#if CONFIG_LWIP_IPV6
    else if (in_addr->ss_family == PF_INET6) {
        struct sockaddr_in6 * in_addr_ip6 = (struct sockaddr_in6 *)in_addr;
        memset(addr, 0, sizeof(esp_ip_addr_t));
        *port = in_addr_ip6->sin6_port;
        uint32_t *u32_addr = in_addr_ip6->sin6_addr.s6_addr32;
        if (u32_addr[0] == 0 && u32_addr[1] == 0 && u32_addr[2] == esp_netif_htonl(0x0000FFFFUL)) {
            // Mapped IPv4 address, convert directly to IPv4
            addr->type = ESP_IPADDR_TYPE_V4;
            addr->u_addr.ip4.addr = u32_addr[3];
        } else {
            addr->type = ESP_IPADDR_TYPE_V6;
            addr->u_addr.ip6.addr[0] = u32_addr[0];
            addr->u_addr.ip6.addr[1] = u32_addr[1];
            addr->u_addr.ip6.addr[2] = u32_addr[2];
            addr->u_addr.ip6.addr[3] = u32_addr[3];
        }
    }
#endif // CONFIG_LWIP_IPV6
}

void sock_recv_task(void* arg)
{
    while (s_run_sock_recv_task) {
        struct timeval tv = {
            .tv_sec = 1,
            .tv_usec = 0,
        };
        fd_set rfds;
        FD_ZERO(&rfds);
        int max_sock = -1;
        for (int i=0; i<MDNS_MAX_INTERFACES; i++) {
            for (int j=0; j<MDNS_IP_PROTOCOL_MAX; j++) {
                int sock = pcb_to_sock(_mdns_server->interfaces[i].pcbs[j].pcb);
                if (sock >= 0) {
                    FD_SET(sock, &rfds);
                    max_sock = MAX(max_sock, sock);
                }
            }
        }
        if (max_sock < 0) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            ESP_LOGI(TAG, "No sock!");
            continue;
        }

        int s = select(max_sock + 1, &rfds, NULL, NULL, &tv);
        if (s < 0) {
            ESP_LOGE(TAG, "Select failed. errno=%d: %s", errno, strerror(errno));
            break;
        } else if (s > 0) {
            for (int tcpip_if=0; tcpip_if<MDNS_MAX_INTERFACES; tcpip_if++) {
                // Both protocols share once socket
                int sock = pcb_to_sock(_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].pcb);
                if (sock < 0) {
                    sock = pcb_to_sock(_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].pcb);
                }
                if (sock < 0) {
                    continue;
                }
                if (FD_ISSET(sock, &rfds)) {
                    static char recvbuf[MDNS_MAX_PACKET_SIZE];
                    uint16_t port = 0;

                    struct sockaddr_storage raddr; // Large enough for both IPv4 or IPv6
                    socklen_t socklen = sizeof(struct sockaddr_storage);
                    esp_ip_addr_t addr = {0};
                    int len = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,
                                       (struct sockaddr *) &raddr, &socklen);
                    if (len < 0) {
                        ESP_LOGE(TAG, "multicast recvfrom failed. errno=%d: %s", errno, strerror(errno));
                        break;
                    }
                    ESP_LOGD(TAG, "[sock=%d]: Received from IP:%s", sock, get_string_address(&raddr));
                    ESP_LOG_BUFFER_HEXDUMP(TAG, recvbuf, len, ESP_LOG_VERBOSE);
                    inet_to_espaddr(&raddr, &addr, &port);

                    // Allocate the packet structure and pass it to the mdns main engine
                    mdns_rx_packet_t *packet = (mdns_rx_packet_t *) calloc(1, sizeof(mdns_rx_packet_t));
                    struct pbuf *packet_pbuf = calloc(1, sizeof(struct pbuf));
                    uint8_t *buf = malloc(len);
                    if (packet == NULL || packet_pbuf == NULL || buf == NULL ) {
                        free(buf);
                        free(packet_pbuf);
                        free(packet);
                        HOOK_MALLOC_FAILED;
                        ESP_LOGE(TAG, "Failed to allocate the mdns packet");
                        continue;
                    }
                    memcpy(buf, recvbuf, len);
                    packet_pbuf->next = NULL;
                    packet_pbuf->payload = buf;
                    packet_pbuf->tot_len = len;
                    packet_pbuf->len = len;
                    packet->tcpip_if = tcpip_if;
                    packet->pb = packet_pbuf;
                    packet->src_port = ntohs(port);
                    memcpy(&packet->src, &addr, sizeof(esp_ip_addr_t));
                    // TODO(IDF-3651): Add the correct dest addr -- for mdns to decide multicast/unicast
                    // Currently it's enough to assume the packet is multicast and mdns to check the source port of the packet
                    memset(&packet->dest, 0, sizeof(esp_ip_addr_t));
                    packet->multicast = 1;
                    packet->dest.type = packet->src.type;
                    packet->ip_protocol =
                            packet->src.type == ESP_IPADDR_TYPE_V4 ? MDNS_IP_PROTOCOL_V4 : MDNS_IP_PROTOCOL_V6;
                    if (!_mdns_server || !_mdns_server->action_queue || _mdns_send_rx_action(packet) != ESP_OK) {
                        ESP_LOGE(TAG, "_mdns_send_rx_action failed!");
                        free(packet->pb->payload);
                        free(packet->pb);
                        free(packet);
                    }
                }
            }
        }
    }
    vTaskDelete(NULL);
}

static void mdns_networking_init(void)
{
    if (s_run_sock_recv_task == false) {
        s_run_sock_recv_task = true;
        xTaskCreate( sock_recv_task, "mdns recv task", 3*1024, NULL, 5, NULL );
    }
}

static struct udp_pcb* create_pcb(mdns_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    if (_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb) {
        return _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb;
    }
    mdns_ip_protocol_t other_ip_proto = ip_protocol==MDNS_IP_PROTOCOL_V4?MDNS_IP_PROTOCOL_V6:MDNS_IP_PROTOCOL_V4;
    esp_netif_t *netif = _mdns_get_esp_netif(tcpip_if);
    if (_mdns_server->interfaces[tcpip_if].pcbs[other_ip_proto].pcb) {
        struct udp_pcb* other_pcb = _mdns_server->interfaces[tcpip_if].pcbs[other_ip_proto].pcb;
        int err = join_mdns_multicast_group(pcb_to_sock(other_pcb), netif, ip_protocol);
        if (err < 0) {
            ESP_LOGE(TAG, "Failed to add ipv6 multicast group for protocol %d", ip_protocol);
            return NULL;
        }
        return other_pcb;
    }
    int sock = create_socket(netif);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create the socket!");
        return NULL;
    }
    int err = join_mdns_multicast_group(sock, netif, ip_protocol);
    if (err < 0) {
        ESP_LOGE(TAG, "Failed to add ipv6 multicast group for protocol %d", ip_protocol);
    }
    return sock_to_pcb(sock);
}

esp_err_t _mdns_pcb_init(mdns_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    ESP_LOGI(TAG, "_mdns_pcb_init(tcpip_if=%d, ip_protocol=%d)", tcpip_if, ip_protocol);
    _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb = create_pcb(tcpip_if, ip_protocol);
    _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].failed_probes = 0;

    mdns_networking_init();
    return ESP_OK;
}

static int create_socket(esp_netif_t *netif)
{
#if CONFIG_LWIP_IPV6
    int sock = socket(PF_INET6, SOCK_DGRAM, 0);
#else
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
#endif
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket. errno=%d: %s", errno, strerror(errno));
        return -1;
    }

    int on = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) < 0) {
        ESP_LOGE(TAG, "Failed setsockopt() to set SO_REUSEADDR. errno=%d: %s\n", errno, strerror(errno));
    }
    // Bind the socket to any address
#if CONFIG_LWIP_IPV6
    struct sockaddr_in6 saddr = { INADDR_ANY };
    saddr.sin6_family = AF_INET6;
    saddr.sin6_port = htons(5353);
    bzero(&saddr.sin6_addr.s6_addr, sizeof(saddr.sin6_addr.s6_addr));
    int err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in6));
    if (err < 0) {
        ESP_LOGE(TAG, "Failed to bind socket. errno=%d: %s", errno, strerror(errno));
        goto err;
    }
#else
    struct sockaddr_in saddr = { 0 };
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5353);
    bzero(&saddr.sin_addr.s_addr, sizeof(saddr.sin_addr.s_addr));
    int err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (err < 0) {
        ESP_LOGE(TAG, "Failed to bind socket. errno=%d: %s", errno, strerror(errno));
        goto err;
    }
#endif // CONFIG_LWIP_IPV6
    struct ifreq ifr;
    esp_netif_get_netif_impl_name(netif, ifr.ifr_name);
    int ret = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,  (void*)&ifr, sizeof(struct ifreq));
    if (ret < 0) {
        ESP_LOGE(TAG, "\"%s\" Unable to bind socket to specified interface. errno=%d: %s", esp_netif_get_desc(netif), errno, strerror(errno));
        goto err;
    }

    return sock;

err:
    close(sock);
    return -1;
}

#if CONFIG_LWIP_IPV6
static int socket_add_ipv6_multicast_group(int sock, esp_netif_t *netif)
{
    int ifindex = esp_netif_get_netif_impl_index(netif);
    int err = setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex, sizeof(ifindex));
    if (err < 0) {
        ESP_LOGE(TAG, "Failed to set IPV6_MULTICAST_IF. errno=%d: %s", errno, strerror(errno));
        return err;
    }

    struct ipv6_mreq v6imreq = { 0 };
    esp_ip_addr_t multi_addr = ESP_IP6ADDR_INIT(0x000002ff, 0, 0, 0xfb000000);
    memcpy(&v6imreq.ipv6mr_multiaddr, &multi_addr.u_addr.ip6.addr, sizeof(v6imreq.ipv6mr_multiaddr));
    v6imreq.ipv6mr_interface = ifindex;
    err = setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &v6imreq, sizeof(struct ipv6_mreq));
    if (err < 0) {
        ESP_LOGE(TAG, "Failed to set IPV6_ADD_MEMBERSHIP. errno=%d: %s", errno, strerror(errno));
        return err;
    }
    return err;
}
#endif // CONFIG_LWIP_IPV6

static int socket_add_ipv4_multicast_group(int sock, esp_netif_t *netif)
{
    struct ip_mreq imreq = { 0 };
    int err = 0;
    esp_netif_ip_info_t ip_info = { 0 };

    if (esp_netif_get_ip_info(netif, &ip_info) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to esp_netif_get_ip_info()");
        goto err;
    }
    imreq.imr_interface.s_addr = ip_info.ip.addr;

    esp_ip_addr_t multicast_addr = ESP_IP4ADDR_INIT(224, 0, 0, 251);
    imreq.imr_multiaddr.s_addr = multicast_addr.u_addr.ip4.addr;

    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imreq, sizeof(struct ip_mreq));
    if (err < 0) {
        ESP_LOGE(TAG, "[sock=%d] Failed to set IP_ADD_MEMBERSHIP. errno=%d: %s", sock, errno, strerror(errno));
        goto err;
    }

 err:
    return err;
}

static int join_mdns_multicast_group(int sock, esp_netif_t *netif, mdns_ip_protocol_t ip_protocol)
{
    if (ip_protocol == MDNS_IP_PROTOCOL_V4) {
        return socket_add_ipv4_multicast_group(sock, netif);
    }
#if CONFIG_LWIP_IPV6
    if (ip_protocol == MDNS_IP_PROTOCOL_V6) {
        return socket_add_ipv6_multicast_group(sock, netif);
    }
#endif // CONFIG_LWIP_IPV6
    return -1;
}
