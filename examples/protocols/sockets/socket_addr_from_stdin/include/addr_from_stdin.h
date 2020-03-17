#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <arpa/inet.h>

/**
 * @brief Read and evaluate IP address from stdin
 *
 * This API reads stdin and parses the input address using getaddrinfo()
 * to fill in struct sockaddr_in6 (for both IPv4 and IPv6) used to open
 * a socket. IP protocol is guessed from the IP address string.
 *
 * @param[in] port port number of expected connection
 * @param[in] sock_type expected protocol: SOCK_STREAM or SOCK_DGRAM
 * @param[out] ip_protocol resultant IP protocol: IPPROTO_IP or IPPROTO_IP6
 * @param[out] addr_family resultant address family: AF_INET or AF_INET6
 * @param[out] dest_addr sockaddr_in6 structure (for both IPv4 and IPv6)
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t get_addr_from_stdin(int port, int sock_type,
                              int *ip_protocol,
                              int *addr_family,
                              struct sockaddr_in6 *dest_addr);