/* UDP MultiCast Send/Receive Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#define UDP_PORT CONFIG_EXAMPLE_PORT

#define MULTICAST_LOOPBACK CONFIG_EXAMPLE_LOOPBACK

#define MULTICAST_TTL CONFIG_EXAMPLE_MULTICAST_TTL

#define MULTICAST_IPV4_ADDR CONFIG_EXAMPLE_MULTICAST_IPV4_ADDR
#define MULTICAST_IPV6_ADDR CONFIG_EXAMPLE_MULTICAST_IPV6_ADDR

#define LISTEN_DEFAULT_IF CONFIG_EXAMPLE_LISTEN_DEFAULT_IF

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   we use two - one for IPv4 "got ip", and
   one for IPv6 "got ip". */
const int IPV4_GOTIP_BIT = BIT0;
const int IPV6_GOTIP_BIT = BIT1;

static const char *TAG = "multicast";
#ifdef CONFIG_EXAMPLE_IPV4
static const char *V4TAG = "mcast-ipv4";
#endif
#ifdef CONFIG_EXAMPLE_IPV6
static const char *V6TAG = "mcast-ipv6";
#endif

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        /* enable ipv6 */
        tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, IPV4_GOTIP_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, IPV4_GOTIP_BIT);
        xEventGroupClearBits(wifi_event_group, IPV6_GOTIP_BIT);
        break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
        xEventGroupSetBits(wifi_event_group, IPV6_GOTIP_BIT);
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

#ifdef CONFIG_EXAMPLE_IPV4
/* Add a socket, either IPV4-only or IPV6 dual mode, to the IPV4
   multicast group */
static int socket_add_ipv4_multicast_group(int sock, bool assign_source_if)
{
    struct ip_mreq imreq = { 0 };
    struct in_addr iaddr = { 0 };
    int err = 0;
    // Configure source interface
#if USE_DEFAULT_IF
    imreq.imr_interface.s_addr = IPADDR_ANY;
#else
    tcpip_adapter_ip_info_t ip_info = { 0 };
    err = tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info);
    if (err != ESP_OK) {
        ESP_LOGE(V4TAG, "Failed to get IP address info. Error 0x%x", err);
        goto err;
    }
    inet_addr_from_ipaddr(&iaddr, &ip_info.ip);
#endif
    // Configure multicast address to listen to
    err = inet_aton(MULTICAST_IPV4_ADDR, &imreq.imr_multiaddr.s_addr);
    if (err != 1) {
        ESP_LOGE(V4TAG, "Configured IPV4 multicast address '%s' is invalid.", MULTICAST_IPV4_ADDR);
        goto err;
    }
    ESP_LOGI(TAG, "Configured IPV4 Multicast address %s", inet_ntoa(imreq.imr_multiaddr.s_addr));
    if (!IP_MULTICAST(ntohl(imreq.imr_multiaddr.s_addr))) {
        ESP_LOGW(V4TAG, "Configured IPV4 multicast address '%s' is not a valid multicast address. This will probably not work.", MULTICAST_IPV4_ADDR);
    }

    if (assign_source_if) {
        // Assign the IPv4 multicast source interface, via its IP
        // (only necessary if this socket is IPV4 only)
        err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
                         sizeof(struct in_addr));
        if (err < 0) {
            ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_IF. Error %d", errno);
            goto err;
        }
    }

    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         &imreq, sizeof(struct ip_mreq));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_ADD_MEMBERSHIP. Error %d", errno);
        goto err;
    }

 err:
    return err;
}
#endif /* CONFIG_EXAMPLE_IPV4 */

#ifdef CONFIG_EXAMPLE_IPV4_ONLY
static int create_multicast_ipv4_socket()
{
    struct sockaddr_in saddr = { 0 };
    int sock = -1;
    int err = 0;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(V4TAG, "Failed to create socket. Error %d", errno);
        return -1;
    }

    // Bind the socket to any address
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(UDP_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to bind socket. Error %d", errno);
        goto err;
    }


    // Assign multicast TTL (set separately from normal interface TTL)
    uint8_t ttl = MULTICAST_TTL;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_TTL. Error %d", errno);
        goto err;
    }

#if MULTICAST_LOOPBACK
    // select whether multicast traffic should be received by this device, too
    // (if setsockopt() is not called, the default is no)
    uint8_t loopback_val = MULTICAST_LOOPBACK;
    err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
                     &loopback_val, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_LOOP. Error %d", errno);
        goto err;
    }
#endif

    // this is also a listening socket, so add it to the multicast
    // group for listening...
    err = socket_add_ipv4_multicast_group(sock, true);
    if (err < 0) {
        goto err;
    }

    // All set, socket is configured for sending and receiving
    return sock;

err:
    close(sock);
    return -1;
}
#endif /* CONFIG_EXAMPLE_IPV4_ONLY */

#ifdef CONFIG_EXAMPLE_IPV6
static int create_multicast_ipv6_socket()
{
    struct sockaddr_in6 saddr = { 0 };
    struct in6_addr if_inaddr = { 0 };
    struct ip6_addr if_ipaddr = { 0 };
    struct ip6_mreq v6imreq = { 0 };
    int sock = -1;
    int err = 0;

    sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6);
    if (sock < 0) {
        ESP_LOGE(V6TAG, "Failed to create socket. Error %d", errno);
        return -1;
    }

    // Bind the socket to any address
    saddr.sin6_family = AF_INET6;
    saddr.sin6_port = htons(UDP_PORT);
    bzero(&saddr.sin6_addr.un, sizeof(saddr.sin6_addr.un));
    err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in6));
    if (err < 0) {
        ESP_LOGE(V6TAG, "Failed to bind socket. Error %d", errno);
        goto err;
    }

    // Selct the interface to use as multicast source for this socket.
#if USE_DEFAULT_IF
    bzero(&if_inaddr.un, sizeof(if_inaddr.un));
#else
    // Read interface adapter link-local address and use it
    // to bind the multicast IF to this socket.
    //
    // (Note the interface may have other non-LL IPV6 addresses as well,
    // but it doesn't matter in this context as the address is only
    // used to identify the interface.)
    err = tcpip_adapter_get_ip6_linklocal(TCPIP_ADAPTER_IF_STA, &if_ipaddr);
    inet6_addr_from_ip6addr(&if_inaddr, &if_ipaddr);
    if (err != ESP_OK) {
        ESP_LOGE(V6TAG, "Failed to get IPV6 LL address. Error 0x%x", err);
        goto err;
    }
#endif

    // Assign the multicast source interface, via its IP
    err = setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, &if_inaddr,
                     sizeof(struct in6_addr));
    if (err < 0) {
        ESP_LOGE(V6TAG, "Failed to set IPV6_MULTICAST_IF. Error %d", errno);
        goto err;
    }

    // Assign multicast TTL (set separately from normal interface TTL)
    uint8_t ttl = MULTICAST_TTL;
    setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &ttl, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(V6TAG, "Failed to set IPV6_MULTICAST_HOPS. Error %d", errno);
        goto err;
    }

#if MULTICAST_LOOPBACK
    // select whether multicast traffic should be received by this device, too
    // (if setsockopt() is not called, the default is no)
    uint8_t loopback_val = MULTICAST_LOOPBACK;
    err = setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP,
                     &loopback_val, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(V6TAG, "Failed to set IPV6_MULTICAST_LOOP. Error %d", errno);
        goto err;
    }
#endif

    // this is also a listening socket, so add it to the multicast
    // group for listening...

    // Configure source interface
#if USE_DEFAULT_IF
    v6imreq.imr_interface.s_addr = IPADDR_ANY;
#else
    inet6_addr_from_ip6addr(&v6imreq.ipv6mr_interface, &if_ipaddr);
#endif
#ifdef CONFIG_EXAMPLE_IPV6
    // Configure multicast address to listen to
    err = inet6_aton(MULTICAST_IPV6_ADDR, &v6imreq.ipv6mr_multiaddr);
    if (err != 1) {
        ESP_LOGE(V6TAG, "Configured IPV6 multicast address '%s' is invalid.", MULTICAST_IPV6_ADDR);
        goto err;
    }
    ESP_LOGI(TAG, "Configured IPV6 Multicast address %s", inet6_ntoa(v6imreq.ipv6mr_multiaddr));
    ip6_addr_t multi_addr;
    inet6_addr_to_ip6addr(&multi_addr, &v6imreq.ipv6mr_multiaddr);
    if (!ip6_addr_ismulticast(&multi_addr)) {
        ESP_LOGW(V6TAG, "Configured IPV6 multicast address '%s' is not a valid multicast address. This will probably not work.", MULTICAST_IPV6_ADDR);
    }

    err = setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
                     &v6imreq, sizeof(struct ip6_mreq));
    if (err < 0) {
        ESP_LOGE(V6TAG, "Failed to set IPV6_ADD_MEMBERSHIP. Error %d", errno);
        goto err;
    }
#endif

#if CONFIG_EXAMPLE_IPV4_V6
    // Add the common IPV4 config options
    err = socket_add_ipv4_multicast_group(sock, false);
    if (err < 0) {
        goto err;
    }
#endif

#if CONFIG_EXAMPLE_IPV4_V6
    int only = 0;
#else
    int only = 1; /* IPV6-only socket */
#endif
    err = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &only, sizeof(int));
    if (err < 0) {
        ESP_LOGE(V6TAG, "Failed to set IPV6_V6ONLY. Error %d", errno);
        goto err;
    }
    ESP_LOGI(TAG, "Socket set IPV6-only");

    // All set, socket is configured for sending and receiving
    return sock;

err:
    close(sock);
    return -1;
}
#endif

static void mcast_example_task(void *pvParameters)
{
    while (1) {
        /* Wait for all the IPs we care about to be set
        */
        uint32_t bits = 0;
#ifdef CONFIG_EXAMPLE_IPV4
        bits |= IPV4_GOTIP_BIT;
#endif
#ifdef CONFIG_EXAMPLE_IPV6
        bits |= IPV6_GOTIP_BIT;
#endif
        ESP_LOGI(TAG, "Waiting for AP connection...");
        xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "Connected to AP");

        int sock;

#ifdef CONFIG_EXAMPLE_IPV4_ONLY
        sock = create_multicast_ipv4_socket();
        if (sock < 0) {
            ESP_LOGE(TAG, "Failed to create IPv4 multicast socket");
        }
#else
        sock = create_multicast_ipv6_socket();
        if (sock < 0) {
            ESP_LOGE(TAG, "Failed to create IPv6 multicast socket");
        }
#endif

        if (sock < 0) {
            // Nothing to do!
            vTaskDelay(5 / portTICK_PERIOD_MS);
            continue;
        }

#ifdef CONFIG_EXAMPLE_IPV4
        // set destination multicast addresses for sending from these sockets
        struct sockaddr_in sdestv4 = {
            .sin_family = PF_INET,
            .sin_port = htons(UDP_PORT),
        };
        // We know this inet_aton will pass because we did it above already
        inet_aton(MULTICAST_IPV4_ADDR, &sdestv4.sin_addr.s_addr);
#endif

#ifdef CONFIG_EXAMPLE_IPV6
        struct sockaddr_in6 sdestv6 = {
            .sin6_family = PF_INET6,
            .sin6_port = htons(UDP_PORT),
        };
        // We know this inet_aton will pass because we did it above already
        inet6_aton(MULTICAST_IPV6_ADDR, &sdestv6.sin6_addr);
#endif

        // Loop waiting for UDP received, and sending UDP packets if we don't
        // see any.
        int err = 1;
        while (err > 0) {
            struct timeval tv = {
                .tv_sec = 2,
                .tv_usec = 0,
            };
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(sock, &rfds);

            int s = lwip_select(sock + 1, &rfds, NULL, NULL, &tv);
            if (s < 0) {
                ESP_LOGE(TAG, "Select failed: errno %d", errno);
                err = -1;
                break;
            }
            else if (s > 0) {
                if (FD_ISSET(sock, &rfds)) {
                    // Incoming datagram received
                    char recvbuf[48];
                    char raddr_name[32] = { 0 };

                    struct sockaddr_in6 raddr; // Large enough for both IPv4 or IPv6
                    socklen_t socklen = sizeof(raddr);
                    int len = recvfrom(sock, recvbuf, sizeof(recvbuf)-1, 0,
                                       (struct sockaddr *)&raddr, &socklen);
                    if (len < 0) {
                        ESP_LOGE(TAG, "multicast recvfrom failed: errno %d", errno);
                        err = -1;
                        break;
                    }

                    // Get the sender's address as a string
#ifdef CONFIG_EXAMPLE_IPV4
                    if (raddr.sin6_family == PF_INET) {
                        inet_ntoa_r(((struct sockaddr_in *)&raddr)->sin_addr.s_addr,
                                    raddr_name, sizeof(raddr_name)-1);
                    }
#endif
#ifdef CONFIG_EXAMPLE_IPV6
                    if (raddr.sin6_family == PF_INET6) {
                        inet6_ntoa_r(raddr.sin6_addr, raddr_name, sizeof(raddr_name)-1);
                    }
#endif
                    ESP_LOGI(TAG, "received %d bytes from %s:", len, raddr_name);

                    recvbuf[len] = 0; // Null-terminate whatever we received and treat like a string...
                    ESP_LOGI(TAG, "%s", recvbuf);
                }
            }
            else { // s == 0
                // Timeout passed with no incoming data, so send something!
                static int send_count;
                const char sendfmt[] = "Multicast #%d sent by ESP32\n";
                char sendbuf[48];
                char addrbuf[32] = { 0 };
                int len = snprintf(sendbuf, sizeof(sendbuf), sendfmt, send_count++);
                if (len > sizeof(sendbuf)) {
                    ESP_LOGE(TAG, "Overflowed multicast sendfmt buffer!!");
                    send_count = 0;
                    err = -1;
                    break;
                }

                struct addrinfo hints = {
                    .ai_flags = AI_PASSIVE,
                    .ai_socktype = SOCK_DGRAM,
                };
                struct addrinfo *res;

#ifdef CONFIG_EXAMPLE_IPV4 // Send an IPv4 multicast packet

#ifdef CONFIG_EXAMPLE_IPV4_ONLY
                hints.ai_family = AF_INET; // For an IPv4 socket
#else
                hints.ai_family = AF_INET6; // For an IPv4 socket with V4 mapped addresses
                hints.ai_flags |= AI_V4MAPPED;
#endif
                int err = getaddrinfo(CONFIG_EXAMPLE_MULTICAST_IPV4_ADDR,
                                      NULL,
                                      &hints,
                                      &res);
                if (err < 0) {
                    ESP_LOGE(TAG, "getaddrinfo() failed for IPV4 destination address. error: %d", err);
                    break;
                }
#ifdef CONFIG_EXAMPLE_IPV4_ONLY
                ((struct sockaddr_in *)res->ai_addr)->sin_port = htons(UDP_PORT);
                inet_ntoa_r(((struct sockaddr_in *)res->ai_addr)->sin_addr, addrbuf, sizeof(addrbuf)-1);
                ESP_LOGI(TAG, "Sending to IPV4 multicast address %s...",  addrbuf);
#else
                ((struct sockaddr_in6 *)res->ai_addr)->sin6_port = htons(UDP_PORT);
                inet6_ntoa_r(((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, addrbuf, sizeof(addrbuf)-1);
                ESP_LOGI(TAG, "Sending to IPV6 (V4 mapped) multicast address %s (%s)...",  addrbuf, CONFIG_EXAMPLE_MULTICAST_IPV4_ADDR);
#endif
                err = sendto(sock, sendbuf, len, 0, res->ai_addr, res->ai_addrlen);
                if (err < 0) {
                    ESP_LOGE(TAG, "IPV4 sendto failed. errno: %d", errno);
                    break;
                }
#endif
#ifdef CONFIG_EXAMPLE_IPV6
                hints.ai_family = AF_INET6;
                hints.ai_protocol = 0;
                err = getaddrinfo(CONFIG_EXAMPLE_MULTICAST_IPV6_ADDR,
                                  NULL,
                                  &hints,
                                  &res);
                if (err < 0) {
                    ESP_LOGE(TAG, "getaddrinfo() failed for IPV6 destination address. error: %d", err);
                    break;
                }


                struct sockaddr_in6 *s6addr = (struct sockaddr_in6 *)res->ai_addr;
                s6addr->sin6_port = htons(UDP_PORT);
                inet6_ntoa_r(s6addr->sin6_addr, addrbuf, sizeof(addrbuf)-1);
                ESP_LOGI(TAG, "Sending to IPV6 multicast address %s...",  addrbuf);
                err = sendto(sock, sendbuf, len, 0, res->ai_addr, res->ai_addrlen);
                if (err < 0) {
                    ESP_LOGE(TAG, "IPV6 sendto failed. errno: %d", errno);
                    break;
                }
#endif
            }
        }

        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }

}

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    xTaskCreate(&mcast_example_task, "mcast_task", 4096, NULL, 5, NULL);
}
