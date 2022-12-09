// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_NETIF_IP_ADDR_H_
#define _ESP_NETIF_IP_ADDR_H_

#include <machine/endian.h>

#ifdef __cplusplus
extern "C" {
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define esp_netif_htonl(x) ((uint32_t)(x))
#else
#define esp_netif_htonl(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                      (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                      (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                      (((x) & (uint32_t)0xff000000UL) >> 24))
#endif

#define esp_netif_ip4_makeu32(a,b,c,d) (((uint32_t)((a) & 0xff) << 24) | \
                               ((uint32_t)((b) & 0xff) << 16) | \
                               ((uint32_t)((c) & 0xff) << 8)  | \
                                (uint32_t)((d) & 0xff))

// Access address in 16-bit block
#define ESP_IP6_ADDR_BLOCK1(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[0]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK2(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[0])) & 0xffff))
#define ESP_IP6_ADDR_BLOCK3(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[1]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK4(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[1])) & 0xffff))
#define ESP_IP6_ADDR_BLOCK5(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[2]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK6(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[2])) & 0xffff))
#define ESP_IP6_ADDR_BLOCK7(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[3]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK8(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[3])) & 0xffff))

#define IPSTR "%d.%d.%d.%d"
#define esp_ip4_addr_get_byte(ipaddr, idx) (((const uint8_t*)(&(ipaddr)->addr))[idx])
#define esp_ip4_addr1(ipaddr) esp_ip4_addr_get_byte(ipaddr, 0)
#define esp_ip4_addr2(ipaddr) esp_ip4_addr_get_byte(ipaddr, 1)
#define esp_ip4_addr3(ipaddr) esp_ip4_addr_get_byte(ipaddr, 2)
#define esp_ip4_addr4(ipaddr) esp_ip4_addr_get_byte(ipaddr, 3)


#define esp_ip4_addr1_16(ipaddr) ((uint16_t)esp_ip4_addr1(ipaddr))
#define esp_ip4_addr2_16(ipaddr) ((uint16_t)esp_ip4_addr2(ipaddr))
#define esp_ip4_addr3_16(ipaddr) ((uint16_t)esp_ip4_addr3(ipaddr))
#define esp_ip4_addr4_16(ipaddr) ((uint16_t)esp_ip4_addr4(ipaddr))

#define IP2STR(ipaddr) esp_ip4_addr1_16(ipaddr), \
    esp_ip4_addr2_16(ipaddr), \
    esp_ip4_addr3_16(ipaddr), \
    esp_ip4_addr4_16(ipaddr)

#define IPV6STR "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"

#define IPV62STR(ipaddr) ESP_IP6_ADDR_BLOCK1(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK2(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK3(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK4(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK5(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK6(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK7(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK8(&(ipaddr))

#define ESP_IPADDR_TYPE_V4                0U
#define ESP_IPADDR_TYPE_V6                6U
#define ESP_IPADDR_TYPE_ANY               46U

#define ESP_IP4TOUINT32(a,b,c,d) (((uint32_t)((a) & 0xffU) << 24) | \
                               ((uint32_t)((b) & 0xffU) << 16) | \
                               ((uint32_t)((c) & 0xffU) << 8)  | \
                                (uint32_t)((d) & 0xffU))

#define ESP_IP4TOADDR(a,b,c,d) esp_netif_htonl(ESP_IP4TOUINT32(a, b, c, d))

struct esp_ip6_addr {
    uint32_t addr[4];
    uint8_t zone;
};

struct esp_ip4_addr {
    uint32_t addr;
};

typedef struct esp_ip4_addr esp_ip4_addr_t;

typedef struct esp_ip6_addr esp_ip6_addr_t;

typedef struct _ip_addr {
    union {
        esp_ip6_addr_t ip6;
        esp_ip4_addr_t ip4;
    } u_addr;
    uint8_t type;
} esp_ip_addr_t;
 
typedef enum {
    ESP_IP6_ADDR_IS_UNKNOWN,
    ESP_IP6_ADDR_IS_GLOBAL,
    ESP_IP6_ADDR_IS_LINK_LOCAL,
    ESP_IP6_ADDR_IS_SITE_LOCAL,
    ESP_IP6_ADDR_IS_UNIQUE_LOCAL,
    ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6
} esp_ip6_addr_type_t;

/**
 * @brief  Get the IPv6 address type
 *
 * @param[in]  ip6_addr IPv6 type
 *
 * @return IPv6 type in form of enum esp_ip6_addr_type_t
 */
esp_ip6_addr_type_t esp_netif_ip6_get_addr_type(esp_ip6_addr_t* ip6_addr);

#ifdef __cplusplus
}
#endif

#endif //_ESP_NETIF_IP_ADDR_H_
