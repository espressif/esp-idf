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
#include "mdns.h"

#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/igmp.h"
#include "lwip/udp.h"
#include "esp_wifi.h"

#define MDNS_FLAGS_AUTHORITATIVE    0x8400

#define MDNS_NAME_REF               0xC000

#define MDNS_TYPE_AAAA              0x001C
#define MDNS_TYPE_A                 0x0001
#define MDNS_TYPE_PTR               0x000C
#define MDNS_TYPE_SRV               0x0021
#define MDNS_TYPE_TXT               0x0010
#define MDNS_TYPE_NSEC              0x002F
#define MDNS_TYPE_ANY               0x00FF

#define MDNS_CLASS_IN               0x0001
#define MDNS_CLASS_IN_FLUSH_CACHE   0x8001

#define MDNS_ANSWER_ALL             0x3F
#define MDNS_ANSWER_PTR             0x08
#define MDNS_ANSWER_TXT             0x04
#define MDNS_ANSWER_SRV             0x02
#define MDNS_ANSWER_A               0x01
#define MDNS_ANSWER_AAAA            0x10
#define MDNS_ANSWER_NSEC            0x20

#define MDNS_SERVICE_PORT           5353                    // UDP port that the server runs on
#define MDNS_SERVICE_STACK_DEPTH    4096                    // Stack size for the service thread
#define MDNS_PACKET_QUEUE_LEN       16                      // Maximum packets that can be queued for parsing
#define MDNS_TXT_MAX_LEN            1024                    // Maximum string length of text data in TXT record
#define MDNS_NAME_MAX_LEN           64                      // Maximum string length of hostname, instance, service and proto
#define MDNS_NAME_BUF_LEN           (MDNS_NAME_MAX_LEN+1)   // Maximum char buffer size to hold hostname, instance, service or proto
#define MDNS_MAX_PACKET_SIZE        1460                    // Maximum size of mDNS  outgoing packet

#define MDNS_ANSWER_PTR_TTL         4500
#define MDNS_ANSWER_TXT_TTL         4500
#define MDNS_ANSWER_SRV_TTL         120
#define MDNS_ANSWER_A_TTL           120
#define MDNS_ANSWER_AAAA_TTL        120

#define MDNS_HEAD_LEN               12
#define MDNS_HEAD_ID_OFFSET         0
#define MDNS_HEAD_FLAGS_OFFSET      2
#define MDNS_HEAD_QUESTIONS_OFFSET  4
#define MDNS_HEAD_ANSWERS_OFFSET    6
#define MDNS_HEAD_SERVERS_OFFSET    8
#define MDNS_HEAD_ADDITIONAL_OFFSET 10

#define MDNS_TYPE_OFFSET            0
#define MDNS_CLASS_OFFSET           2
#define MDNS_TTL_OFFSET             4
#define MDNS_LEN_OFFSET             8
#define MDNS_DATA_OFFSET            10

#define MDNS_SRV_PRIORITY_OFFSET    0
#define MDNS_SRV_WEIGHT_OFFSET      2
#define MDNS_SRV_PORT_OFFSET        4
#define MDNS_SRV_FQDN_OFFSET        6

typedef struct {
    char host[MDNS_NAME_BUF_LEN];
    char service[MDNS_NAME_BUF_LEN];
    char proto[MDNS_NAME_BUF_LEN];
    char domain[MDNS_NAME_BUF_LEN];
    uint8_t parts;
    uint8_t sub;
} mdns_name_t;

typedef struct {
    char host[MDNS_NAME_BUF_LEN];
    char instance[MDNS_NAME_BUF_LEN];
    char txt[MDNS_TXT_MAX_LEN];
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    uint32_t addr;
    uint8_t addrv6[16];
    uint8_t ptr;
} mdns_result_temp_t;

typedef struct {
    const char * host;
    const char * sub;
    const char * service;
    const char * proto;
    const char * domain;
    uint8_t parts;
    uint8_t done;
} mdns_string_t;

typedef struct mdns_service_s {
    const char * instance;
    const char * service;
    const char * proto;
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    uint8_t txt_num_items;
    const char ** txt;
} mdns_service_t;

typedef struct mdns_srv_item_s {
    mdns_service_t * service;
    struct mdns_srv_item_s * next;
} mdns_srv_item_t;

typedef struct mdns_answer_item_s {
    mdns_service_t * service;
    uint8_t answer;
    struct mdns_answer_item_s * next;
} mdns_answer_item_t;

struct mdns_server_s {
    tcpip_adapter_if_t tcpip_if;
    struct udp_pcb * pcb;
    const char * hostname;
    const char * instance;
    mdns_srv_item_t * services;
    xSemaphoreHandle lock;
    xQueueHandle queue;
    struct {
        char host[MDNS_NAME_BUF_LEN];
        char service[MDNS_NAME_BUF_LEN];
        char proto[MDNS_NAME_BUF_LEN];
        bool running;
        xSemaphoreHandle lock;
        mdns_result_t * results;
    } search;
};

#define MDNS_MUTEX_LOCK()       xSemaphoreTake(server->lock, portMAX_DELAY)
#define MDNS_MUTEX_UNLOCK()     xSemaphoreGive(server->lock)

#define MDNS_SEARCH_LOCK()      xSemaphoreTake(server->search.lock, portMAX_DELAY)
#define MDNS_SEARCH_UNLOCK()    xSemaphoreGive(server->search.lock)

static const char * MDNS_DEFAULT_DOMAIN = "local";
static const char * MDNS_SUB_STR = "_sub";

static mdns_server_t * _mdns_servers[TCPIP_ADAPTER_IF_MAX] = {0,0,0};
static TaskHandle_t _mdns_service_task_handle = NULL;
static QueueSetHandle_t _mdns_queue_set = NULL;

static xSemaphoreHandle _mdns_service_semaphore = NULL;
#define MDNS_SERVICE_LOCK()     xSemaphoreTake(_mdns_service_semaphore, portMAX_DELAY)
#define MDNS_SERVICE_UNLOCK()   xSemaphoreGive(_mdns_service_semaphore)

/*
 * MDNS Server Networking
 * */

/**
 * @brief  the receive callback of the raw udp api. Packets are received here
 *
 */
static void _mdns_server_recv(void *arg, struct udp_pcb *upcb, struct pbuf *pb, const ip_addr_t *addr, uint16_t port)
{
    while(pb != NULL) {
        struct pbuf * this_pb = pb;
        pb = pb->next;
        this_pb->next = NULL;
        mdns_server_t * server = (mdns_server_t *)arg;
        if (!server || !server->queue || xQueueSend(server->queue, &this_pb, (portTickType)0) != pdPASS) {
            pbuf_free(this_pb);
        }
    }
}

/**
 * @brief  init the network of MDNS server
 *
 * @param  server       The server
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE on igmp/bind error
 *      - ESP_ERR_NO_MEM on memory error
 */
esp_err_t _mdns_server_init(mdns_server_t * server)
{
    esp_err_t err = ESP_OK;

    tcpip_adapter_ip_info_t if_ip_info;
    err = tcpip_adapter_get_ip_info(server->tcpip_if, &if_ip_info);
    if (err) {
        return err;
    }

    ip_addr_t laddr;
    IP_ADDR4(&laddr, 224, 0, 0, 251);

    ip_addr_t multicast_if_addr = IPADDR4_INIT(if_ip_info.ip.addr);

    if (igmp_joingroup((const struct ip4_addr *)&multicast_if_addr.u_addr.ip4, (const struct ip4_addr *)&laddr.u_addr.ip4)) {
        return ESP_ERR_INVALID_STATE;
    }

    struct udp_pcb * pcb = udp_new();
    if (!pcb) {
        return ESP_ERR_NO_MEM;
    }

    pcb->remote_port = MDNS_SERVICE_PORT;

    if (udp_bind(pcb, &multicast_if_addr, pcb->remote_port) != 0) {
        udp_remove(pcb);
        return ESP_ERR_INVALID_STATE;
    }

    pcb->mcast_ttl = 1;
    ip_addr_copy(pcb->multicast_ip, multicast_if_addr);
    ip_addr_copy(pcb->remote_ip, laddr);

    server->pcb = pcb;
    udp_recv(pcb, &_mdns_server_recv, server);
    return err;
}

/**
 * @brief  stop the network of MDNS server
 *
 * @param  server       The server
 *
 * @return ESP_OK
 */
esp_err_t _mdns_server_deinit(mdns_server_t * server)
{
    if (server->pcb) {
        udp_recv(server->pcb, NULL, NULL);
        udp_disconnect(server->pcb);
        udp_remove(server->pcb);
        server->pcb = NULL;
    }
    return ESP_OK;
}

/**
 * @brief  send packet over UDP
 *
 * @param  server       The server
 * @param  data         byte array containing the packet data
 * @param  len          length of the packet data
 *
 * @return length of sent packet or 0 on error
 */
static size_t _mdns_server_write(mdns_server_t * server, uint8_t * data, size_t len)
{
    struct pbuf* pbt = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (pbt != NULL) {
        uint8_t* dst = (uint8_t *)pbt->payload;
        memcpy(dst, data, len);
        err_t err = udp_sendto(server->pcb, pbt, &(server->pcb->remote_ip), server->pcb->remote_port);
        pbuf_free(pbt);
        if (err) {
            return 0;
        }
        return len;
    }
    return 0;
}

/*
 * MDNS Servers
 * */

static void _mdns_parse_packet(mdns_server_t * server, const uint8_t * data, size_t len);

/**
 * @brief  the main MDNS service task. Packets are received and parsed here
 */
static void _mdns_service_task(void *pvParameters)
{
    uint8_t i;
    struct pbuf * pb;
    QueueSetMemberHandle_t queue;

    for(;;) {
        queue = xQueueSelectFromSet(_mdns_queue_set, portMAX_DELAY);
        if (queue && xQueueReceive(queue, &pb, 0) == pdTRUE) {
            for(i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
                mdns_server_t * server = _mdns_servers[i];
                if (server && server->queue == queue) {
                    MDNS_MUTEX_LOCK();
                    _mdns_parse_packet(server, (uint8_t*)pb->payload, pb->len);
                    MDNS_MUTEX_UNLOCK();
                    break;
                }
            }
            pbuf_free(pb);
        }
    }
}

/**
 * @brief  get the server assigned to particular interface
 *
 * @param  tcpip_if     The interface
 *
 * @return reference to the server from the server list or NULL if not found
 */
static mdns_server_t * _mdns_server_get(tcpip_adapter_if_t tcpip_if)
{
    if (tcpip_if < TCPIP_ADAPTER_IF_MAX) {
        return _mdns_servers[tcpip_if];
    }
    return NULL;
}

/**
 * @brief  add server to the server list. Start the service thread if not running
 *
 * @param  server       The server to add
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 *      - ESP_ERR_* on network error
 */
static esp_err_t _mdns_server_add(mdns_server_t * server)
{
    if (!_mdns_service_semaphore) {
        _mdns_service_semaphore = xSemaphoreCreateMutex();
        if (!_mdns_service_semaphore) {
            return ESP_FAIL;
        }
    }
    MDNS_SERVICE_LOCK();
    if (!_mdns_service_task_handle) {
        _mdns_queue_set = xQueueCreateSet(TCPIP_ADAPTER_IF_MAX * MDNS_PACKET_QUEUE_LEN);
        if (!_mdns_queue_set) {
            MDNS_SERVICE_UNLOCK();
            return ESP_FAIL;
        }
        xTaskCreatePinnedToCore(_mdns_service_task, "mdns", MDNS_SERVICE_STACK_DEPTH, NULL, 1, &_mdns_service_task_handle, 0);
        if (!_mdns_service_task_handle) {
            vQueueDelete(_mdns_queue_set);
            _mdns_queue_set = NULL;
            MDNS_SERVICE_UNLOCK();
            return ESP_FAIL;
        }
    }
    MDNS_SERVICE_UNLOCK();

    if (xQueueAddToSet(server->queue, _mdns_queue_set) != pdPASS) {
        return ESP_FAIL;
    }

    //start UDP
    esp_err_t err = _mdns_server_init(server);
    if (err) {
        return err;
    }

    _mdns_servers[server->tcpip_if] = server;

    return ESP_OK;
}

/**
 * @brief  remove server from server list. Stop the service thread in no more servers are running
 *
 * @param  server       The server to remove
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t _mdns_server_remove(mdns_server_t * server)
{
    //stop UDP
    _mdns_server_deinit(server);

    _mdns_servers[server->tcpip_if] = NULL;

    if (xQueueRemoveFromSet(server->queue, _mdns_queue_set) != pdPASS) {
        return ESP_FAIL;
    }

    uint8_t i;
    for(i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        if (_mdns_servers[i]) {
            break;
        }
    }
    if (i == TCPIP_ADAPTER_IF_MAX) {
        //none of the servers are running
        MDNS_SERVICE_LOCK();
        if (_mdns_service_task_handle) {
            vTaskDelete(_mdns_service_task_handle);
            vQueueDelete(_mdns_queue_set);
            _mdns_queue_set = NULL;
            _mdns_service_task_handle = NULL;
        }
        MDNS_SERVICE_UNLOCK();
    }

    return ESP_OK;
}


/*
 * PARSING
 * */

/**
 * @brief  queues service for answer (if service is already added, append the new answer type)
 *
 * @param  answers      Linked list of answers
 * @param  service      Service to add to the answers
 * @param  type         Type of the answer
 *
 * @return the new linked list of answers
 */
static mdns_answer_item_t * _mdns_add_answer(mdns_answer_item_t * answers, mdns_service_t * service, uint8_t type)
{
    //see if we already have the service queued
    mdns_answer_item_t * a = answers;
    while(a) {
        if (a->service == service) {
            //just add the new answer type to it
            a->answer |= type;
            return answers;
        }
        a = a->next;
    }
    //prepend the q with this new answer
    a = (mdns_answer_item_t *)malloc(sizeof(mdns_answer_item_t));
    if (!a) {
        return answers;//fail!
    }
    a->service = service;
    a->answer = type;
    a->next = answers;
    answers = a;
    return a;
}

/**
 * @brief  reads MDNS FQDN into mdns_name_t structure
 *         FQDN is in format: [hostname.|[instance.]_service._proto.]local.
 *
 * @param  packet       MDNS packet
 * @param  start        Starting point of FQDN
 * @param  name         mdns_name_t structure to populate
 * @param  buf          temporary char buffer
 *
 * @return the address after the parsed FQDN in the packet or NULL on error
 */
static const uint8_t * _mdns_read_fqdn(const uint8_t * packet, const uint8_t * start, mdns_name_t * name, char * buf)
{
    size_t index = 0;
    while(start[index]) {
        if (name->parts == 4) {
            return NULL;
        }
        uint8_t len = start[index++];
        if ((len & 0xC0) == 0) {
            if (len > 64) {
                //length can not be more than 64
                return NULL;
            }
            uint8_t i;
            for(i=0; i<len; i++) {
                buf[i] = start[index++];
            }
            buf[len] = '\0';
            if (name->parts == 1 && buf[0] != '_'
                    && (strcmp(buf, MDNS_DEFAULT_DOMAIN) != 0)
                    && (strcmp(buf, "ip6") != 0)
                    && (strcmp(buf, "in-addr") != 0)) {
                snprintf((char*)name, MDNS_NAME_BUF_LEN, "%s.%s", name->host, buf);
            } else if (strcmp(buf, MDNS_SUB_STR) == 0) {
                name->sub = 1;
            } else {
                memcpy((uint8_t*)name + (name->parts++ * (MDNS_NAME_BUF_LEN)), buf, len+1);
            }
        } else {
            size_t address = (((uint16_t)len & 0x3F) << 8) | start[index++];
            if ((packet + address) >= start) {
                //reference address can not be after where we are
                return NULL;
            }
            if (_mdns_read_fqdn(packet, packet + address, name, buf)) {
                return start + index;
            }
            return NULL;
        }
    }
    return start + index + 1;
}

/**
 * @brief  reads and formats MDNS FQDN into mdns_name_t structure
 *
 * @param  packet       MDNS packet
 * @param  start        Starting point of FQDN
 * @param  name         mdns_name_t structure to populate
 *
 * @return the address after the parsed FQDN in the packet or NULL on error
 */
static const uint8_t * _mdns_parse_fqdn(const uint8_t * packet, const uint8_t * start, mdns_name_t * name)
{
    name->parts = 0;
    name->sub = 0;
    name->host[0] = 0;
    name->service[0] = 0;
    name->proto[0] = 0;
    name->domain[0] = 0;

    static char buf[MDNS_NAME_BUF_LEN];

    const uint8_t * next_data = (uint8_t*)_mdns_read_fqdn(packet, start, name, buf);
    if (!next_data || name->parts < 2) {
        return 0;
    }
    if (name->parts == 3) {
        memmove((uint8_t*)name + (MDNS_NAME_BUF_LEN), (uint8_t*)name, 3*(MDNS_NAME_BUF_LEN));
        name->host[0] = 0;
    } else if (name->parts == 2) {
        memmove((uint8_t*)(name->domain), (uint8_t*)(name->service), (MDNS_NAME_BUF_LEN));
        name->service[0] = 0;
        name->proto[0] = 0;
    }
    if (strcmp(name->domain, MDNS_DEFAULT_DOMAIN) == 0 || strcmp(name->domain, "arpa") == 0) {
        return next_data;
    }
    return 0;
}

/*
 * Packet construction
 * */

/**
 * @brief  sets uint16_t value in a packet
 *
 * @param  packet       MDNS packet
 * @param  index        offset of uint16_t value
 * @param  value        the value to set
 */
static inline void _mdns_set_u16(uint8_t * packet, uint16_t index, uint16_t value)
{
    if ((index + 1) >= MDNS_MAX_PACKET_SIZE) {
        return;
    }
    packet[index] = (value >> 8) & 0xFF;
    packet[index+1] = value & 0xFF;
}

/**
 * @brief  appends byte in a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  value        the value to set
 *
 * @return length of added data: 0 on error or 1 on success
 */
static inline uint8_t _mdns_append_u8(uint8_t * packet, uint16_t * index, uint8_t value)
{
    if (*index >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    packet[*index] = value;
    *index += 1;
    return 1;
}

/**
 * @brief  appends uint16_t in a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  value        the value to set
 *
 * @return length of added data: 0 on error or 2 on success
 */
static inline uint8_t _mdns_append_u16(uint8_t * packet, uint16_t * index, uint16_t value)
{
    if ((*index + 1) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, (value >> 8) & 0xFF);
    _mdns_append_u8(packet, index, value & 0xFF);
    return 2;
}

/**
 * @brief  appends uint32_t in a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  value        the value to set
 *
 * @return length of added data: 0 on error or 4 on success
 */
static inline uint8_t _mdns_append_u32(uint8_t * packet, uint16_t * index, uint32_t value)
{
    if ((*index + 3) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, (value >> 24) & 0xFF);
    _mdns_append_u8(packet, index, (value >> 16) & 0xFF);
    _mdns_append_u8(packet, index, (value >> 8) & 0xFF);
    _mdns_append_u8(packet, index, value & 0xFF);
    return 4;
}

/**
 * @brief  appends answer type, class, ttl and data length to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  type         answer type
 * @param  ttl          answer ttl
 *
 * @return length of added data: 0 on error or 10 on success
 */
static inline uint8_t _mdns_append_type(uint8_t * packet, uint16_t * index, uint8_t type, uint32_t ttl)
{
    if ((*index + 10) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    if (type == MDNS_ANSWER_PTR) {
        _mdns_append_u16(packet, index, MDNS_TYPE_PTR);
        _mdns_append_u16(packet, index, MDNS_CLASS_IN);
    } else if (type == MDNS_ANSWER_TXT) {
        _mdns_append_u16(packet, index, MDNS_TYPE_TXT);
        _mdns_append_u16(packet, index, MDNS_CLASS_IN_FLUSH_CACHE);
    } else if (type == MDNS_ANSWER_SRV) {
        _mdns_append_u16(packet, index, MDNS_TYPE_SRV);
        _mdns_append_u16(packet, index, MDNS_CLASS_IN_FLUSH_CACHE);
    } else if (type == MDNS_ANSWER_A) {
        _mdns_append_u16(packet, index, MDNS_TYPE_A);
        _mdns_append_u16(packet, index, MDNS_CLASS_IN_FLUSH_CACHE);
    } else if (type == MDNS_ANSWER_AAAA) {
        _mdns_append_u16(packet, index, MDNS_TYPE_AAAA);
        _mdns_append_u16(packet, index, MDNS_CLASS_IN_FLUSH_CACHE);
    } else {
        return 0;
    }
    _mdns_append_u32(packet, index, ttl);
    _mdns_append_u16(packet, index, 0);
    return 10;
}

/**
 * @brief  appends single string to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  string       the string to append
 *
 * @return length of added data: 0 on error or length of the string + 1 on success
 */
static inline uint8_t _mdns_append_string(uint8_t * packet, uint16_t * index, const char * string)
{
    uint8_t len = strlen(string);
    if ((*index + len + 1) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, len);
    memcpy(packet + *index, string, len);
    *index += len;
    return len + 1;
}

/**
 * @brief  appends FQDN to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  strings      string array containing the parts of the FQDN
 * @param  count        number of strings in the array
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_fqdn(uint8_t * packet, uint16_t * index, const char * strings[], uint8_t count)
{
    if (!count) {
        return _mdns_append_u8(packet, index, 0);
    }
    mdns_name_t name;
    static char buf[MDNS_NAME_BUF_LEN];
    uint8_t len = strlen(strings[0]);
    uint8_t * len_location = (uint8_t *)memchr(packet, (char)len, *index);
    while(len_location) {
        if (memcmp(len_location+1, strings[0], len)) { //not continuing with our string
search_next:
            len_location = (uint8_t *)memchr(len_location+1, (char)len, *index - (len_location+1 - packet));
            continue;
        }
        //read string into name and compare
        name.parts = 0;
        name.sub = 0;
        name.host[0] = 0;
        name.service[0] = 0;
        name.proto[0] = 0;
        name.domain[0] = 0;
        const uint8_t * content = _mdns_read_fqdn(packet, len_location, &name, buf);
        if (!content) {
            return 0;
        }
        if (name.parts == count) {
            uint8_t i;
            for(i=0; i<count; i++) {
                if (strcmp(strings[i], (const char *)&name + (i * (MDNS_NAME_BUF_LEN)))) {
                    //not our string
                    goto search_next;
                }
            }
            //we actually have found the string
            break;
        } else {
            goto search_next;
        }
    }
    if (!len_location) {
        uint8_t written = _mdns_append_string(packet, index, strings[0]);
        if (!written) {
            return 0;
        }
        return written + _mdns_append_fqdn(packet, index, &strings[1], count - 1);
    }

    uint16_t offset = len_location - packet;
    offset |= MDNS_NAME_REF;
    return _mdns_append_u16(packet, index, offset);
}

/**
 * @brief  appends PTR record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_ptr_record(uint8_t * packet, uint16_t * index, mdns_server_t * server, mdns_service_t * service)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
            :(server->instance)?server->instance
            :server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str + 1, 3);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_PTR, MDNS_ANSWER_PTR_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;
    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    _mdns_set_u16(packet, data_len_location, part_length);
    record_length += part_length;
    return record_length;
}

/**
 * @brief  appends TXT record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_txt_record(uint8_t * packet, uint16_t * index, mdns_server_t * server, mdns_service_t * service)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
            :(server->instance)?server->instance
            :server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_TXT, MDNS_ANSWER_TXT_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;
    uint16_t data_len = 0;
    if (service->txt_num_items) {
        uint8_t len = service->txt_num_items;
        const char ** txt = service->txt;
        uint8_t i, l;
        for(i=0; i<len; i++) {
            l = _mdns_append_string(packet, index, txt[i]);
            if (!l) {
               return 0;
            }
            data_len += l;
        }
    }
    _mdns_set_u16(packet, data_len_location, data_len);
    record_length += data_len;
    return record_length;
}

/**
 * @brief  appends SRV record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_srv_record(uint8_t * packet, uint16_t * index, mdns_server_t * server, mdns_service_t * service)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
            :(server->instance)?server->instance
            :server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_SRV, MDNS_ANSWER_SRV_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;

    part_length = 0;
    part_length += _mdns_append_u16(packet, index, service->priority);
    part_length += _mdns_append_u16(packet, index, service->weight);
    part_length += _mdns_append_u16(packet, index, service->port);
    if (part_length != 6) {
        return 0;
    }

    str[0] = server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;
    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    _mdns_set_u16(packet, data_len_location, part_length + 6);

    record_length += part_length + 6;
    return record_length;
}

/**
 * @brief  appends A record to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server
 * @param  ip           the IP address to add
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_a_record(uint8_t * packet, uint16_t * index, mdns_server_t * server, uint32_t ip)
{
    const char * str[2];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_A, MDNS_ANSWER_A_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;

    if ((*index + 3) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, ip & 0xFF);
    _mdns_append_u8(packet, index, (ip >> 8) & 0xFF);
    _mdns_append_u8(packet, index, (ip >> 16) & 0xFF);
    _mdns_append_u8(packet, index, (ip >> 24) & 0xFF);
    _mdns_set_u16(packet, data_len_location, 4);

    record_length += 4;
    return record_length;
}

/**
 * @brief  appends AAAA record to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server
 * @param  ipv6         the IPv6 address to add
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_aaaa_record(uint8_t * packet, uint16_t * index, mdns_server_t * server, uint8_t * ipv6)
{
    const char * str[2];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_AAAA, MDNS_ANSWER_AAAA_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;

    if ((*index + 15) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    
    part_length = sizeof(ip6_addr_t);
    memcpy(packet + *index, ipv6, part_length);
    *index += part_length;
    _mdns_set_u16(packet, data_len_location, part_length);
    record_length += part_length;
    return record_length;
}

/**
 * @brief  sends all collected answers
 *
 * @param  server       the server
 * @param  answers      linked list of answers
 */
static void _mdns_send_answers(mdns_server_t * server, mdns_answer_item_t * answers)
{
    bool send_ip = false;
    static uint8_t packet[MDNS_MAX_PACKET_SIZE];
    uint16_t index = MDNS_HEAD_LEN;
    uint8_t answer_count = 0;

    memset(packet, 0, MDNS_HEAD_LEN);

    _mdns_set_u16(packet, MDNS_HEAD_FLAGS_OFFSET, MDNS_FLAGS_AUTHORITATIVE);

    while(answers) {
        if (answers->answer & MDNS_ANSWER_A) {
            answers->answer &= ~MDNS_ANSWER_A;
            send_ip = true;
        }
        if (answers->service) {

            if (answers->answer & MDNS_ANSWER_PTR) {
                if (!_mdns_append_ptr_record(packet, &index, server, answers->service)) {
                    return;
                }
                answer_count += 1;
            }

            if (answers->answer & MDNS_ANSWER_TXT) {
                if (!_mdns_append_txt_record(packet, &index, server, answers->service)) {
                    return;
                }
                answer_count += 1;
            }

            if (answers->answer & MDNS_ANSWER_SRV) {
                if (!_mdns_append_srv_record(packet, &index, server, answers->service)) {
                    return;
                }
                answer_count += 1;
            }
        }
        mdns_answer_item_t * a = answers;
        answers = answers->next;
        free(a);
    }
    if (send_ip) {
        tcpip_adapter_ip_info_t if_ip_info;
        tcpip_adapter_get_ip_info(server->tcpip_if, &if_ip_info);

        if (!_mdns_append_a_record(packet, &index, server, if_ip_info.ip.addr)) {
            return;
        }
        answer_count += 1;

        //add ipv6 if available
        struct ip6_addr if_ip6;
        if (!tcpip_adapter_get_ip6_linklocal(server->tcpip_if, &if_ip6)) {
            uint8_t * v6addr = (uint8_t*)if_ip6.addr;
            //check if not 0
            int i;
            for(i=0;i<sizeof(ip6_addr_t);i++) {
                if (v6addr[i]) {
                    break;
                }
            }
            if (i<sizeof(ip6_addr_t)) {
                if (!_mdns_append_aaaa_record(packet, &index, server, v6addr)) {
                    return;
                }
                answer_count += 1;
            }
        }
    }

    _mdns_set_u16(packet, MDNS_HEAD_ANSWERS_OFFSET, answer_count);
    _mdns_server_write(server, packet, index);
}

/**
 * @brief  appends search result from query
 *
 * @param  server       the server
 * @param  r            the temporary result to copy
 */
static void _mdns_add_result(mdns_server_t * server, mdns_result_temp_t * r)
{
    mdns_result_t * n = (mdns_result_t *)malloc(sizeof(mdns_result_t));
    if (!n) {
        return;
    }
    n->priority = r->priority;
    n->weight = r->weight;
    n->port = r->port;
    n->addr.addr = r->addr;

    size_t hlen = strlen(r->host);
    if (hlen) {
        n->host = strdup(r->host);
        if (!n->host) {
            free(n);
            return;
        }
    } else {
        n->host = NULL;
    }

    size_t ilen = strlen(r->instance);
    if (ilen) {
        n->instance = strdup(r->instance);
        if (!n->instance) {
            free((char *)n->host);
            free(n);
            return;
        }
    } else {
        n->instance = NULL;
    }

    size_t tlen = strlen(r->txt);
    if (tlen) {
        n->txt = strdup(r->txt);
        if (!n->txt) {
            free((char *)n->host);
            free((char *)n->instance);
            free(n);
            return;
        }
    } else {
        n->txt = NULL;
    }

    memcpy((uint8_t *)n->addrv6.addr, r->addrv6, sizeof(ip6_addr_t));

    mdns_result_t * o = server->search.results;
    server->search.results = n;
    n->next = o;
}

/**
 * @brief  finds service from given service type
 * @param  server       the server
 * @param  service      service type to match
 * @param  proto        proto to match
 *
 * @return the service item if found or NULL on error
 */
static mdns_srv_item_t * _mdns_get_service_item(mdns_server_t * server, const char * service, const char * proto)
{
    mdns_srv_item_t * s = server->services;
    while(s) {
        if (!strcmp(s->service->service, service) && !strcmp(s->service->proto, proto)) {
            return s;
        }
        s = s->next;
    }
    return NULL;
}

/**
 * @brief  creates/allocates new service
 * @param  service       service type
 * @param  proto         service proto
 * @param  port          service port
 *
 * @return pointer to the service or NULL on error
 */
static mdns_service_t * _mdns_create_service(const char * service, const char * proto, uint16_t port)
{
    mdns_service_t * s = (mdns_service_t *)malloc(sizeof(mdns_service_t));
    if (!s) {
        return NULL;
    }

    s->priority = 0;
    s->weight = 0;
    s->txt_num_items = 0;
    s->instance = NULL;
    s->txt = NULL;
    s->port = port;

    s->service = strndup(service, MDNS_NAME_BUF_LEN - 1);
    if (!s->service) {
        free(s);
        return NULL;
    }

    s->proto = strndup(proto, MDNS_NAME_BUF_LEN - 1);
    if (!s->proto) {
        free((char *)s->service);
        free(s);
        return NULL;
    }

    return s;
}

/**
 * @brief  free service memory
 *
 * @param  service      the service
 */
static void _mdns_free_service(mdns_service_t * service)
{
    if (!service) {
        return;
    }
    free((char *)service->instance);
    free((char *)service->service);
    free((char *)service->proto);
    if (service->txt_num_items) {
        uint8_t i;
        for(i=0; i<service->txt_num_items; i++) {
            free((char *)service->txt[i]);
        }
    }
    free(service->txt);
    free(service);
}

/**
 * @brief  read uint16_t from a packet
 * @param  packet       the packet
 * @param  index        index in the packet where the value starts
 *
 * @return the value
 */
static inline uint16_t _mdns_read_u16(const uint8_t * packet, uint16_t index)
{
    return (uint16_t)(packet[index]) << 8 | packet[index+1];
}

/**
 * @brief  main packet parser
 *
 * @param  server       the server
 * @param  data         byte array holding the packet data
 * @param  len          length of the byte array
 */
static void _mdns_parse_packet(mdns_server_t * server, const uint8_t * data, size_t len)
{
    static mdns_name_t n;
    static mdns_result_temp_t a;

    const uint8_t * content = data + MDNS_HEAD_LEN;
    mdns_name_t * name = &n;
    memset(name, 0, sizeof(mdns_name_t));

    uint16_t questions = _mdns_read_u16(data, MDNS_HEAD_QUESTIONS_OFFSET);
    uint16_t answers = _mdns_read_u16(data, MDNS_HEAD_ANSWERS_OFFSET);
    uint16_t additional = _mdns_read_u16(data, MDNS_HEAD_ADDITIONAL_OFFSET);

    if (questions) {
        uint8_t qs = questions;
        mdns_answer_item_t * answers = NULL;

        while(qs--) {
            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                break;//error
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            content = content + 4;

            if (!name->service[0] || !name->proto[0]) {
                if (type == MDNS_TYPE_A || type == MDNS_TYPE_AAAA || type == MDNS_TYPE_ANY) {//send A + AAAA
                    if (name->host[0] && server->hostname && server->hostname[0] && !strcmp(name->host, server->hostname)) {
                        answers = _mdns_add_answer(answers, NULL, MDNS_ANSWER_A);
                    }
                }
                continue;
            }

            if (name->sub) {
                continue;
            }

            mdns_srv_item_t * si = _mdns_get_service_item(server, name->service, name->proto);
            if (!si) {
                //service not found
                continue;
            }

            if (type == MDNS_TYPE_PTR) {
                answers = _mdns_add_answer(answers, si->service, MDNS_ANSWER_ALL);
            } else if (type == MDNS_TYPE_TXT) {
                //match instance/host
                const char * host = (si->service->instance)?si->service->instance
                        :(server->instance)?server->instance
                        :server->hostname;
                if (!host || !host[0] || !name->host[0] || strcmp(name->host, host)) {
                    continue;
                }
                answers = _mdns_add_answer(answers, si->service, MDNS_ANSWER_TXT);
            } else if (type == MDNS_TYPE_SRV) {
                //match instance/host
                const char * host = (si->service->instance)?si->service->instance
                        :(server->instance)?server->instance
                        :server->hostname;
                if (!host || !host[0] || !name->host[0] || strcmp(name->host, host)) {
                    continue;
                }
                answers = _mdns_add_answer(answers, si->service, MDNS_ANSWER_SRV | MDNS_ANSWER_A);
            } else if (type == MDNS_TYPE_ANY) {//send all
                //match host
                if (!name->host[0] || !server->hostname || !server->hostname[0] || strcmp(name->host, server->hostname)) {
                    answers = _mdns_add_answer(answers, si->service, MDNS_ANSWER_ALL);
                }
            }
        }
        if (answers) {
            _mdns_send_answers(server, answers);
        }
    }

    if (server->search.running && (answers || additional)) {
        mdns_result_temp_t * answer = &a;
        memset(answer, 0, sizeof(mdns_result_temp_t));

        while(content < (data + len)) {
            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                break;//error
            }
            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t data_len = _mdns_read_u16(content, MDNS_LEN_OFFSET);
            const uint8_t * data_ptr = content + MDNS_DATA_OFFSET;

            content = data_ptr + data_len;

            if (type == MDNS_TYPE_PTR) {
                if (!_mdns_parse_fqdn(data, data_ptr, name)) {
                    continue;//error
                }
                if (server->search.host[0] ||
                        (strcmp(name->service, server->search.service) != 0) ||
                        (strcmp(name->proto, server->search.proto) != 0)) {
                    continue;//not searching for service or wrong service/proto
                }
                strlcpy(answer->instance, name->host, MDNS_NAME_BUF_LEN);
            } else if (type == MDNS_TYPE_SRV) {
                if (server->search.host[0] ||
                        (strcmp(name->service, server->search.service) != 0) ||
                        (strcmp(name->proto, server->search.proto) != 0)) {
                    continue;//not searching for service or wrong service/proto
                }
                if (answer->instance[0]) {
                    if (strcmp(answer->instance, name->host) != 0) {
                        continue;//instance name is not the same as the one in the PTR record
                    }
                } else {
                    strlcpy(answer->instance, name->host, MDNS_NAME_BUF_LEN);
                }
                //parse record value
                if (!_mdns_parse_fqdn(data, data_ptr + MDNS_SRV_FQDN_OFFSET, name)) {
                    continue;//error
                }

                answer->ptr = 1;
                answer->priority = _mdns_read_u16(data_ptr, MDNS_SRV_PRIORITY_OFFSET);
                answer->weight = _mdns_read_u16(data_ptr, MDNS_SRV_WEIGHT_OFFSET);
                answer->port = _mdns_read_u16(data_ptr, MDNS_SRV_PORT_OFFSET);
                if (answer->host[0]) {
                    if (strcmp(answer->host, name->host) != 0) {
                        answer->addr = 0;
                        strlcpy(answer->host, name->host, MDNS_NAME_BUF_LEN);
                    }
                } else {
                    strlcpy(answer->host, name->host, MDNS_NAME_BUF_LEN);
                }
            } else if (type == MDNS_TYPE_TXT) {
                uint16_t i=0,b=0, y;
                while(i < data_len) {
                    uint8_t partLen = data_ptr[i++];
                    //check if partLen will fit in the buffer
                    if (partLen > (MDNS_TXT_MAX_LEN - b - 1)) {
                        break;
                    }
                    for(y=0; y<partLen; y++) {
                        char d = data_ptr[i++];
                        answer->txt[b++] = d;
                    }
                    if (i<data_len) {
                        answer->txt[b++] = '&';
                    }
                }
                answer->txt[b] = 0;
            } else if (type == MDNS_TYPE_AAAA) {
                if (server->search.host[0]) {
                    if (strcmp(name->host, server->search.host) != 0) {
                        continue;//wrong host
                    }
                } else if (!answer->ptr) {
                    strlcpy(answer->host, name->host, MDNS_NAME_BUF_LEN);
                } else if (strcmp(answer->host, name->host) != 0) {
                    continue;//wrong host
                }
                memcpy(answer->addrv6, data_ptr, sizeof(ip6_addr_t));
            } else if (type == MDNS_TYPE_A) {
                if (server->search.host[0]) {
                    if (strcmp(name->host, server->search.host) != 0) {
                        continue;//wrong host
                    }
                } else if (!answer->ptr) {
                    strlcpy(answer->host, name->host, MDNS_NAME_BUF_LEN);
                } else if (strcmp(answer->host, name->host) != 0) {
                    continue;//wrong host
                }
                if (server->search.running && answer->addr) {
                    _mdns_add_result(server, answer);//another IP for our host
                }
                IP4_ADDR(answer, data_ptr[0], data_ptr[1], data_ptr[2], data_ptr[3]);
            }
        }
        if (server->search.running && (server->search.host[0] || answer->ptr) && answer->addr) {
            _mdns_add_result(server, answer);
        }
        //end while
    }
}



/*
 * Public Methods
 * */
esp_err_t mdns_init(tcpip_adapter_if_t tcpip_if, mdns_server_t ** mdns_server)
{
    esp_err_t err = ESP_OK;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (_mdns_server_get(tcpip_if)) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t mode;
    err = esp_wifi_get_mode((wifi_mode_t*)&mode);
    if (err) {
        return err;
    }

    if ((tcpip_if == TCPIP_ADAPTER_IF_STA && !(mode & WIFI_MODE_STA))
            || (tcpip_if == TCPIP_ADAPTER_IF_AP && !(mode & WIFI_MODE_AP))) {
        return ESP_ERR_INVALID_ARG;
    }

    mdns_server_t * server = (mdns_server_t *)malloc(sizeof(mdns_server_t));
    if (!server) {
        return ESP_ERR_NO_MEM;
    }

    server->tcpip_if = tcpip_if;
    server->hostname = NULL;
    server->instance = NULL;
    server->services = NULL;
    server->search.host[0] = 0;
    server->search.service[0] = 0;
    server->search.proto[0] = 0;
    server->search.running = false;
    server->search.results = NULL;
    server->pcb = NULL;

    server->lock = xSemaphoreCreateMutex();
    if (!server->lock) {
        free(server);
        return ESP_ERR_NO_MEM;
    }

    server->search.lock = xSemaphoreCreateMutex();
    if (!server->search.lock) {
        vSemaphoreDelete(server->lock);
        free(server);
        return ESP_ERR_NO_MEM;
    }

    server->queue = xQueueCreate(MDNS_PACKET_QUEUE_LEN, sizeof(struct pbuf *));
    if (!server->queue) {
        vSemaphoreDelete(server->lock);
        vSemaphoreDelete(server->search.lock);
        free(server);
        return ESP_ERR_NO_MEM;
    }

    if (_mdns_server_add(server)) {
        //service start failed!
        vSemaphoreDelete(server->lock);
        vSemaphoreDelete(server->search.lock);
        vQueueDelete(server->queue);
        free(server);
        return ESP_FAIL;
    }

    const char * hostname = NULL;
    tcpip_adapter_get_hostname(server->tcpip_if, &hostname);
    mdns_set_hostname(server, hostname);

    *mdns_server = server;

    return ESP_OK;
}

void mdns_free(mdns_server_t * server)
{
    if (!server) {
        return;
    }
    _mdns_server_remove(server);
    mdns_service_remove_all(server);
    MDNS_MUTEX_LOCK();
    free((char*)server->hostname);
    free((char*)server->instance);
    if (server->queue) {
        struct pbuf * c;
        while(xQueueReceive(server->queue, &c, 0) == pdTRUE) {
            pbuf_free(c);
        }
        vQueueDelete(server->queue);
    }
    if (server->search.running) {
        mdns_query_end(server);
    }
    mdns_result_free(server);
    vSemaphoreDelete(server->search.lock);
    MDNS_MUTEX_UNLOCK();
    vSemaphoreDelete(server->lock);
    free(server);
}

esp_err_t mdns_set_hostname(mdns_server_t * server, const char * hostname)
{
    if (!server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(hostname) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    MDNS_MUTEX_LOCK();
    free((char*)server->hostname);
    server->hostname = (char *)malloc(strlen(hostname)+1);
    if (!server->hostname) {
        MDNS_MUTEX_UNLOCK();
        return ESP_ERR_NO_MEM;
    }
    strlcpy((char *)server->hostname, hostname, MDNS_NAME_BUF_LEN);
    MDNS_MUTEX_UNLOCK();
    return ERR_OK;
}

esp_err_t mdns_set_instance(mdns_server_t * server, const char * instance)
{
    if (!server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(instance) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    MDNS_MUTEX_LOCK();
    free((char*)server->instance);
    server->instance = (char *)malloc(strlen(instance)+1);
    if (!server->instance) {
        MDNS_MUTEX_UNLOCK();
        return ESP_ERR_NO_MEM;
    }
    strlcpy((char *)server->instance, instance, MDNS_NAME_BUF_LEN);
    MDNS_MUTEX_UNLOCK();
    return ERR_OK;
}

/*
 * MDNS SERVICES
 * */

esp_err_t mdns_service_add(mdns_server_t * server, const char * service, const char * proto, uint16_t port)
{
    if (!server || !service || !proto || !port) {
        //bad argument
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * item = _mdns_get_service_item(server, service, proto);
    if (item) {
        //we already have that service
        return mdns_service_port_set(server, service, proto, port);
    }

    mdns_service_t * s = _mdns_create_service(service, proto, port);
    if (!s) {
        return ESP_ERR_NO_MEM;
    }

    item = (mdns_srv_item_t *)malloc(sizeof(mdns_srv_item_t));
    if (!item) {
        return ESP_ERR_NO_MEM;
    }

    item->service = s;
    item->next = server->services;
    server->services = item;
    return ESP_OK;
}

esp_err_t mdns_service_port_set(mdns_server_t * server, const char * service, const char * proto, uint16_t port)
{
    if (!server || !server->services || !service || !proto || !port) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(server, service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    MDNS_MUTEX_LOCK();
    s->service->port = port;
    MDNS_MUTEX_UNLOCK();
    return ESP_OK;
}

esp_err_t mdns_service_txt_set(mdns_server_t * server, const char * service, const char * proto, uint8_t num_items, const char ** txt)
{
    if (!server || !server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(server, service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    MDNS_MUTEX_LOCK();
    if (s->service->txt_num_items) {
        uint8_t i;
        for(i=0; i<s->service->txt_num_items; i++) {
            free((char *)s->service->txt[i]);
        }
    }
    free(s->service->txt);
    if (num_items) {
        s->service->txt = (const char **)malloc(sizeof(char *) * num_items);
        if (!s->service->txt) {
            s->service->txt_num_items = 0;
            goto fail;
        }
        uint8_t i;
        s->service->txt_num_items = num_items;
        for(i=0; i<num_items; i++) {
            s->service->txt[i] = strdup(txt[i]);
            if (!s->service->txt[i]) {
                s->service->txt_num_items = i;
                goto fail;
            }
        }
    }
    MDNS_MUTEX_UNLOCK();
    return ESP_OK;
fail:
    MDNS_MUTEX_UNLOCK();
    return ESP_ERR_NO_MEM;
}

esp_err_t mdns_service_instance_set(mdns_server_t * server, const char * service, const char * proto, const char * instance)
{
    if (!server || !server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(instance) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(server, service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    MDNS_MUTEX_LOCK();
    free((char*)s->service->instance);
    s->service->instance = strdup(instance);
    if (!s->service->instance) {
        MDNS_MUTEX_UNLOCK();
        return ESP_ERR_NO_MEM;
    }
    MDNS_MUTEX_UNLOCK();
    return ESP_OK;
}

esp_err_t mdns_service_remove(mdns_server_t * server, const char * service, const char * proto)
{
    if (!server || !server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(server, service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    //first item
    if (server->services == s) {
        MDNS_MUTEX_LOCK();
        server->services = server->services->next;
        MDNS_MUTEX_UNLOCK();
        _mdns_free_service(s->service);
        free(s);
        return ESP_OK;
    }
    //not first item
    mdns_srv_item_t * a = server->services;
    while(a->next && a->next != s) {
        a = a->next;
    }
    //next item of the current item is our item
    if (a->next == s) {
        MDNS_MUTEX_LOCK();
        a->next = s->next;
        MDNS_MUTEX_UNLOCK();
        _mdns_free_service(s->service);
        free(s);
        return ESP_OK;
    }
    //how did we end here?
    return ESP_FAIL;
}

esp_err_t mdns_service_remove_all(mdns_server_t * server)
{
    if (!server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!server->services) {
        return ESP_OK;
    }
    MDNS_MUTEX_LOCK();
    mdns_srv_item_t * a = server->services;
    server->services = NULL;
    while(a) {
        mdns_srv_item_t * s = a;
        a = a->next;
        _mdns_free_service(s->service);
        free(s);
    }
    MDNS_MUTEX_UNLOCK();
    return ESP_OK;
}

/*
 * MDNS QUERY
 * */

uint32_t mdns_query(mdns_server_t * server, const char * service, const char * proto, uint32_t timeout)
{
    if (!server || !service) {
        return 0;
    }
    MDNS_SEARCH_LOCK();
    uint16_t qtype = MDNS_TYPE_PTR;
    mdns_result_free(server);
    if (proto) {
        server->search.host[0] = 0;
        strlcpy(server->search.service, service, MDNS_NAME_BUF_LEN);
        strlcpy(server->search.proto, proto, MDNS_NAME_BUF_LEN);
    } else {
        strlcpy(server->search.host, service, MDNS_NAME_BUF_LEN);
        server->search.service[0] = 0;
        server->search.proto[0] = 0;
        qtype = MDNS_TYPE_A;
    }

    uint8_t hostname_len = strlen(server->search.host);
    uint8_t service_type_len = strlen(server->search.service);
    uint8_t protoname_len = strlen(server->search.proto);

    size_t len = 23; //head+type+class+(strlen(local)+1)+fqdn_end
    if (hostname_len) {
        len += hostname_len + 1;
    } else if (service_type_len) {
        len += service_type_len + protoname_len + 2;
    }

    uint8_t * packet = (uint8_t *)malloc(len);
    if (!packet) {
        return 0;
    }
    memset(packet, 0, len);
    _mdns_set_u16(packet, MDNS_HEAD_QUESTIONS_OFFSET, 1);
    uint16_t index = MDNS_HEAD_LEN;

    if (hostname_len) {
        _mdns_append_string(packet, &index, server->search.host);
    } else if (service_type_len) {
        _mdns_append_string(packet, &index, server->search.service);
        _mdns_append_string(packet, &index, server->search.proto);
    }

    _mdns_append_string(packet, &index, MDNS_DEFAULT_DOMAIN);
    _mdns_append_u8(packet, &index, 0); //fqdn_end

    _mdns_append_u16(packet, &index, qtype);
    _mdns_append_u16(packet, &index, MDNS_CLASS_IN);

    MDNS_MUTEX_LOCK();
    size_t written = _mdns_server_write(server, packet, index);
    MDNS_MUTEX_UNLOCK();
    free(packet);
    if (written != index) {
        return 0;
    }

    server->search.running = true;
    if (timeout) {
        uint32_t startAt = xTaskGetTickCount() * portTICK_PERIOD_MS;
        while(server->search.running && ((xTaskGetTickCount() * portTICK_PERIOD_MS) - startAt) < timeout) {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        server->search.running = false;
        MDNS_SEARCH_UNLOCK();
        return mdns_result_get_count(server);
    }
    return 0;
}

size_t mdns_query_end(mdns_server_t * server)
{
    if (!server || !server->search.running) {
        return 0;
    }
    server->search.running = false;
    MDNS_SEARCH_UNLOCK();
    return mdns_result_get_count(server);
}

esp_err_t mdns_result_free(mdns_server_t * server)
{
    if (!server || server->search.running || !server->search.results) {
        return ESP_ERR_INVALID_ARG;
    }
    while(server->search.results) {
        const mdns_result_t * r = server->search.results;
        server->search.results = (mdns_result_t *)r->next;
        free((char *)r->host);
        free((char *)r->instance);
        free((char *)r->txt);
        free((mdns_result_t *)r);
    }
    server->search.results = NULL;
    return ESP_OK;
}

size_t mdns_result_get_count(mdns_server_t * server)
{
    if (!server || !server->search.results) {
        return 0;
    }
    size_t len = 0;
    const mdns_result_t * r = server->search.results;
    while(r) {
        len++;
        r = r->next;
    }
    return len;
}

const mdns_result_t * mdns_result_get(mdns_server_t * server, size_t num)
{
    if (!server || !server->search.results) {
        return NULL;
    }
    size_t len = 0;
    const mdns_result_t * r = server->search.results;
    while(r) {
        if (len++ == num) {
            return r;
        }
        r = r->next;
    }
    return NULL;
}
