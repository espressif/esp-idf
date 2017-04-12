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

#define portMAX_DELAY               0xFFFFFFFF
#define portTICK_PERIOD_MS          1

#define xSemaphoreTake(s,d)
#define xSemaphoreGive(s)
#define xSemaphoreCreateMutex()     malloc(1)
#define vSemaphoreDelete(s)         free(s)
#define xQueueCreate(n,s)           malloc((n)*(s))
#define vQueueDelete(q)             free(q)
#define xQueueReceive(q, d, t)      (ESP_OK)
#define vTaskDelay(m)               usleep((m)*1000)
#define pbuf_free(p)                free(p)

#define tcpip_adapter_get_ip_info(i,d)
#define tcpip_adapter_get_ip6_linklocal(i,d)    (ESP_OK)
#define tcpip_adapter_get_hostname(i, n)        *(n) = "esp32-0123456789AB"

#define IP4_ADDR(ipaddr, a,b,c,d) \
        (ipaddr)->addr = ((uint32_t)((d) & 0xff) << 24) | \
                         ((uint32_t)((c) & 0xff) << 16) | \
                         ((uint32_t)((b) & 0xff) << 8)  | \
                          (uint32_t)((a) & 0xff)

typedef uint32_t esp_err_t;

typedef void * xSemaphoreHandle;
typedef void * xQueueHandle;

typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< ESP32 station interface */
    TCPIP_ADAPTER_IF_AP,          /**< ESP32 soft-AP interface */
    TCPIP_ADAPTER_IF_ETH,         /**< ESP32 ethernet interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

typedef enum {
    WIFI_MODE_NULL = 0,  /**< null mode */
    WIFI_MODE_STA,       /**< WiFi station mode */
    WIFI_MODE_AP,        /**< WiFi soft-AP mode */
    WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    WIFI_MODE_MAX
} wifi_mode_t;

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

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} tcpip_adapter_ip_info_t;

inline esp_err_t esp_wifi_get_mode(wifi_mode_t * mode)
{
    *mode = WIFI_MODE_APSTA;
    return ESP_OK;
}

inline uint32_t xTaskGetTickCount()
{
    struct timeval tv;
    struct timezone tz;
    if (gettimeofday(&tv, &tz) == 0) {
        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    }
    return 0;
}

#endif //MDNS_TEST_MODE

#endif //_ESP32_COMPAT_H_
