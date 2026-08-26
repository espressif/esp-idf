/*  Common utilities for socket address input interface:
    The API get_addr_from_stdin() is mainly used by socket client examples which read IP address from stdin (if configured).
    This option is typically used in the CI, but could be enabled in the project configuration.
    In that case this component is used to receive a string that is evaluated and processed to output
    socket structures to open a connectio
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read and evaluate IP address from stdin
 *
 * This API reads stdin and parses the input address using getaddrinfo()
 * to fill in struct sockaddr_storage (for both IPv4 and IPv6) used to open
 * a socket. IP protocol is guessed from the IP address string.
 *
 * @param[in] port port number of expected connection
 * @param[in] sock_type expected protocol: SOCK_STREAM or SOCK_DGRAM
 * @param[out] ip_protocol resultant IP protocol: IPPROTO_IP or IPPROTO_IP6
 * @param[out] addr_family resultant address family: AF_INET or AF_INET6
 * @param[out] dest_addr sockaddr_storage structure (for both IPv4 and IPv6)
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t get_addr_from_stdin(int port, int sock_type,
                              int *ip_protocol,
                              int *addr_family,
                              struct sockaddr_storage *dest_addr);

#ifdef __cplusplus
}
#endif
