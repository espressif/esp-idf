#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/sockets.h"
#include <lwip/netdb.h>
#include <arpa/inet.h>

#define HOST_IP_SIZE 128

esp_err_t get_addr_from_stdin(int port, int sock_type, int *ip_protocol, int *addr_family, struct sockaddr_storage *dest_addr)
{
    char host_ip[HOST_IP_SIZE];
    int len;
    static bool already_init = false;

    // this function could be called multiple times -> make sure UART init runs only once
    if (!already_init) {
        example_configure_stdin_stdout();
        already_init = true;
    }

    // ignore empty or LF only string (could receive from DUT class)
    do {
        fgets(host_ip, HOST_IP_SIZE, stdin);
        len = strlen(host_ip);
    } while (len<=1 && host_ip[0] == '\n');
    host_ip[len - 1] = '\0';

    struct addrinfo hints, *addr_list, *cur;
    memset( &hints, 0, sizeof( hints ) );

    // run getaddrinfo() to decide on the IP protocol
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = sock_type;
    hints.ai_protocol = IPPROTO_TCP;
    if( getaddrinfo( host_ip, NULL, &hints, &addr_list ) != 0 ) {
        return ESP_FAIL;
    }
    for( cur = addr_list; cur != NULL; cur = cur->ai_next ) {
        memcpy(dest_addr, cur->ai_addr, sizeof(*dest_addr));
#if CONFIG_EXAMPLE_CONNECT_IPV4
        if (cur->ai_family == AF_INET) {
            *ip_protocol = IPPROTO_IP;
            *addr_family = AF_INET;
            // add port number and return on first IPv4 match
            ((struct sockaddr_in*)dest_addr)->sin_port = htons(port);
            freeaddrinfo( addr_list );
            return ESP_OK;
        }
#endif // IPV4
#if CONFIG_EXAMPLE_CONNECT_IPV6
        if (cur->ai_family == AF_INET6) {
            *ip_protocol = IPPROTO_IPV6;
            *addr_family = AF_INET6;
            // add port and interface number and return on first IPv6 match
            ((struct sockaddr_in6*)dest_addr)->sin6_port = htons(port);
            ((struct sockaddr_in6*)dest_addr)->sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
            freeaddrinfo( addr_list );
            return ESP_OK;
        }
#endif // IPV6
    }
    // no match found
    freeaddrinfo( addr_list );
    return ESP_FAIL;
}
