// Copyright 2020-2021 Espressif Systems (Shanghai) PTE LTD
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

/*
 * All the APIs declared here are internal only APIs, it can only be used by 
 * espressif internal modules, such as SSC, LWIP, TCPIP adapter etc, espressif 
 * customers are not recommended to use them.
 *
 * If someone really want to use specified APIs declared in here, please contact
 * espressif AE/developer to make sure you know the limitations or risk of 
 * the API, otherwise you may get unexpected behavior!!!
 *
 */


#ifndef __ESP_DEBUG_H__
#define __ESP_DEBUG_H__

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sys/queue.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t diag;
} wifi_hardware_diag_t;

void esp_wifi_debug_get_hardare_diag(wifi_hardware_diag_t *reg);
void esp_wifi_debug_dump_hardware_diag(const wifi_hardware_diag_t *reg);

typedef struct {
} wifi_int_counter_t;
void esp_wifi_debug_get_int_counter(wifi_int_counter_t *cnt);
void esp_wifi_debug_dump_int_counter(const wifi_int_counter_t *cnt);

/* Polling type definition
 *  -  bit0     is for debug task deleting
 *  -  bit1~15  is for system info polling
 *  -  bit16~31 is for LWIP info polling
 *  -  bit32~63 is for WiFi info polling
 */
#define DEBUG_MODULE_TASK_DELETE           1

#define DEBUG_MODULE_SYS_MEM               (1ULL<<1)
#define DEBUG_MODULE_SYS_TASK              (1ULL<<2)

#define DEBUG_MODULE_LWIP_SOCKETS          (1ULL<<16)
#define DEBUG_MODULE_LWIP_STATS            (1ULL<<17)

#define DEBUG_MODULE_WIFI_HW_DIAG          (1ULL<<32)
#define DEBUG_MODULE_WIFI_HW_COUNTERS      (1ULL<<33)
#define DEBUG_MODULE_WIFI_HW_MISC          (1ULL<<34)
#define DEBUG_MODULE_WIFI_INT_COUNTERS     (1ULL<<35)
#define DEBUG_MODULE_WIFI_RX_COUNTERS      (1ULL<<36)
#define DEBUG_MODULE_WIFI_TX_COUNTERS      (1ULL<<37)
#define DEBUG_MODULE_WIFI_BUFFER_COUNTERS  (1ULL<<38)
#define DEBUG_MODULE_WIFI_SLEEP_COUNTERS   (1ULL<<39)
#define DEBUG_MODULE_WIFI_EVENT_COUNTERS   (1ULL<<40)
#define DEBUG_MODULE_WIFI_NVS              (1ULL<<41)
#define DEBUG_MODULE_WIFI_STA              (1ULL<<42)
#define DEBUG_MODULE_WIFI_AP               (1ULL<<43)
#define DEBUG_MODULE_WIFI_TRC              (1ULL<<44)

#define DEBUG_MODULE_ALL                   (0xFFFFFFFFFFFFFFFE)

typedef uint64_t wifi_polling_request_t;

void esp_lwip_debug_dump_sockets(void);
void esp_lwip_debug_dump_stats(void);
void esp_lwip_dump(uint64_t modules);

esp_err_t esp_debug_init(void);
esp_err_t esp_debug_deinit(void);

esp_err_t esp_debug_poll(wifi_polling_request_t request);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_DEBUG_H__ */
