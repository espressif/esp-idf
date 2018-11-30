// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP32_COMPAT_H_
#define _ESP32_COMPAT_H_

#ifdef MDNS_TEST_MODE

// Not to include
#define ESP_MDNS_NETWORKING_H_
#define _TCPIP_ADAPTER_H_
#define __ESP_EVENT_H__


#ifdef USE_BSD_STRING
#include <bsd/string.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define CONFIG_MDNS_MAX_SERVICES    25

#define ERR_OK                      0
#define ESP_OK                      0
#define ESP_FAIL                    -1

#define ESP_ERR_NO_MEM              0x101
#define ESP_ERR_INVALID_ARG         0x102
#define ESP_ERR_INVALID_STATE       0x103
#define ESP_ERR_INVALID_SIZE        0x104
#define ESP_ERR_NOT_FOUND           0x105
#define ESP_ERR_NOT_SUPPORTED       0x106
#define ESP_ERR_TIMEOUT             0x107
#define ESP_ERR_INVALID_RESPONSE    0x108
#define ESP_ERR_INVALID_CRC         0x109

#define pdTRUE                      true
#define pdFALSE                     false
#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )

#define portMAX_DELAY               0xFFFFFFFF
#define portTICK_PERIOD_MS          1

#define xSemaphoreTake(s,d)
#define xTaskDelete(a)
#define vTaskDelete(a)             free(a)
#define xSemaphoreGive(s)
#define _mdns_pcb_init(a,b)         true
#define _mdns_pcb_deinit(a,b)         true
#define xSemaphoreCreateMutex()     malloc(1)
#define vSemaphoreDelete(s)         free(s)
#define xTaskCreatePinnedToCore(a,b,c,d,e,f,g)     *(f) = malloc(1)
#define vTaskDelay(m)               usleep((m)*0)
#define pbuf_free(p)                free(p)
#define esp_random()                (rand()%UINT32_MAX)
#define tcpip_adapter_get_ip_info(i,d)          true
#define tcpip_adapter_dhcpc_get_status(a, b)    TCPIP_ADAPTER_DHCP_STARTED
#define tcpip_adapter_get_ip6_linklocal(i,d)    (ESP_OK)
#define tcpip_adapter_get_hostname(i, n)        *(n) = "esp32-0123456789AB"

#define IP4_ADDR(ipaddr, a,b,c,d) \
        (ipaddr)->addr = ((uint32_t)((d) & 0xff) << 24) | \
                         ((uint32_t)((c) & 0xff) << 16) | \
                         ((uint32_t)((b) & 0xff) << 8)  | \
                          (uint32_t)((a) & 0xff)
#define ip_2_ip6(ipaddr)   (&((ipaddr)->u_addr.ip6))
#define ip_2_ip4(ipaddr)   (&((ipaddr)->u_addr.ip4))

#define IPADDR_TYPE_V4                0U
#define IPADDR_TYPE_V6                6U
#define IPADDR_TYPE_ANY               46U
#define IP_SET_TYPE_VAL(ipaddr, iptype) do { (ipaddr).type = (iptype); }while(0)
#define IP_ADDR4(ipaddr,a,b,c,d)      do { IP4_ADDR(ip_2_ip4(ipaddr),a,b,c,d); \
                                           IP_SET_TYPE_VAL(*(ipaddr), IPADDR_TYPE_V4); } while(0)
#define IP_ADDR6(ipaddr,i0,i1,i2,i3)  do { IP6_ADDR(ip_2_ip6(ipaddr),i0,i1,i2,i3); \
                                           IP_SET_TYPE_VAL(*(ipaddr), IPADDR_TYPE_V6); } while(0)

#define IPADDR6_INIT(a, b, c, d)      { { { { a, b, c, d } } }, IPADDR_TYPE_V6 }

typedef int32_t esp_err_t;

typedef void * xSemaphoreHandle;
typedef void * SemaphoreHandle_t;
typedef void * xQueueHandle;
typedef void * QueueHandle_t;
typedef void * TaskHandle_t;
typedef void * esp_timer_handle_t;
typedef uint32_t TickType_t;
typedef uint32_t portTickType;


typedef enum {
    WIFI_MODE_NULL = 0,  /**< null mode */
    WIFI_MODE_STA,       /**< WiFi station mode */
    WIFI_MODE_AP,        /**< WiFi soft-AP mode */
    WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    WIFI_MODE_MAX
} wifi_mode_t;

/* status of DHCP client or DHCP server */
typedef enum {
    TCPIP_ADAPTER_DHCP_INIT = 0,    /**< DHCP client/server in initial state */
    TCPIP_ADAPTER_DHCP_STARTED,     /**< DHCP client/server already been started */
    TCPIP_ADAPTER_DHCP_STOPPED,     /**< DHCP client/server already been stopped */
    TCPIP_ADAPTER_DHCP_STATUS_MAX
} tcpip_adapter_dhcp_status_t;

typedef enum {
    SYSTEM_EVENT_WIFI_READY = 0,           /**< ESP32 WiFi ready */
    SYSTEM_EVENT_SCAN_DONE,                /**< ESP32 finish scanning AP */
    SYSTEM_EVENT_STA_START,                /**< ESP32 station start */
    SYSTEM_EVENT_STA_STOP,                 /**< ESP32 station stop */
    SYSTEM_EVENT_STA_CONNECTED,            /**< ESP32 station connected to AP */
    SYSTEM_EVENT_STA_DISCONNECTED,         /**< ESP32 station disconnected from AP */
    SYSTEM_EVENT_STA_AUTHMODE_CHANGE,      /**< the auth mode of AP connected by ESP32 station changed */
    SYSTEM_EVENT_STA_GOT_IP,               /**< ESP32 station got IP from connected AP */
    SYSTEM_EVENT_STA_LOST_IP,              /**< ESP32 station lost IP and the IP is reset to 0 */
    SYSTEM_EVENT_STA_WPS_ER_SUCCESS,       /**< ESP32 station wps succeeds in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_FAILED,        /**< ESP32 station wps fails in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_TIMEOUT,       /**< ESP32 station wps timeout in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_PIN,           /**< ESP32 station wps pin code in enrollee mode */
    SYSTEM_EVENT_AP_START,                 /**< ESP32 soft-AP start */
    SYSTEM_EVENT_AP_STOP,                  /**< ESP32 soft-AP stop */
    SYSTEM_EVENT_AP_STACONNECTED,          /**< a station connected to ESP32 soft-AP */
    SYSTEM_EVENT_AP_STADISCONNECTED,       /**< a station disconnected from ESP32 soft-AP */
    SYSTEM_EVENT_AP_PROBEREQRECVED,        /**< Receive probe request packet in soft-AP interface */
    SYSTEM_EVENT_GOT_IP6,                  /**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
    SYSTEM_EVENT_ETH_START,                /**< ESP32 ethernet start */
    SYSTEM_EVENT_ETH_STOP,                 /**< ESP32 ethernet stop */
    SYSTEM_EVENT_ETH_CONNECTED,            /**< ESP32 ethernet phy link up */
    SYSTEM_EVENT_ETH_DISCONNECTED,         /**< ESP32 ethernet phy link down */
    SYSTEM_EVENT_ETH_GOT_IP,               /**< ESP32 ethernet got IP from connected AP */
    SYSTEM_EVENT_MAX
} system_event_id_t;

struct udp_pcb {
    uint8_t dummy;
};

struct ip4_addr {
  uint32_t addr;
};
typedef struct ip4_addr ip4_addr_t;

struct ip6_addr {
  uint32_t addr[4];
};
typedef struct ip6_addr ip6_addr_t;

typedef struct _ip_addr {
  union {
    ip6_addr_t ip6;
    ip4_addr_t ip4;
  } u_addr;
  uint8_t type;
} ip_addr_t;

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} tcpip_adapter_ip_info_t;

// typedef int32_t system_event_id_t;
typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< ESP32 station interface */
    TCPIP_ADAPTER_IF_AP,          /**< ESP32 soft-AP interface */
    TCPIP_ADAPTER_IF_ETH,         /**< ESP32 ethernet interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

typedef struct {
    ip6_addr_t ip;
} tcpip_adapter_ip6_info_t;
typedef struct {
    tcpip_adapter_if_t if_index;
    tcpip_adapter_ip6_info_t ip6_info;
} system_event_got_ip6_t;
typedef union {
    system_event_got_ip6_t                     got_ip6;            /**< ESP32 station　or ap or ethernet ipv6 addr state change to preferred */
} system_event_info_t;

typedef struct {
    system_event_id_t     event_id;      /**< event ID */
    system_event_info_t   event_info;    /**< event information */
} system_event_t;

inline esp_err_t esp_wifi_get_mode(wifi_mode_t * mode)
{
    *mode = WIFI_MODE_APSTA;
    return ESP_OK;
}

struct pbuf {
  struct pbuf *next;
  void *payload;
  uint16_t tot_len;
  uint16_t  len;
  uint8_t  /*pbuf_type*/ type;
  uint8_t  flags;
  uint16_t  ref;
};

#define IP_GET_TYPE(ipaddr)           ((ipaddr)->type)
#define IP_IS_V6_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_V6)
#define ip4_addr_copy(dest, src) ((dest).addr = (src).addr)
#define ip6_addr_copy(dest, src) do{(dest).addr[0] = (src).addr[0]; \
                                    (dest).addr[1] = (src).addr[1]; \
                                    (dest).addr[2] = (src).addr[2]; \
                                    (dest).addr[3] = (src).addr[3];}while(0)

#define ip_addr_copy(dest, src)      do{ IP_SET_TYPE_VAL(dest, IP_GET_TYPE(&src)); if(IP_IS_V6_VAL(src)){ \
  ip6_addr_copy(*ip_2_ip6(&(dest)), *ip_2_ip6(&(src))); }else{ \
  ip4_addr_copy(*ip_2_ip4(&(dest)), *ip_2_ip4(&(src))); }}while(0)

#include "esp32_mock.h"

uint32_t xTaskGetTickCount();


#endif //MDNS_TEST_MODE

#endif //_ESP32_COMPAT_H_
