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
#ifndef MDNS_TEST_MODE
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/igmp.h"
#include "lwip/udp.h"
#include "lwip/mld6.h"
#include "lwip/priv/tcpip_priv.h"
#include "esp_wifi.h"
#include "esp_system.h"
#endif

#define _mdns_printf(...) //printf(__VA_ARGS__)
#define _mdns_dbg_printf(...) //printf(__VA_ARGS__)

#define MDNS_FLAGS_AUTHORITATIVE    0x8400
#define MDNS_FLAGS_DISTRIBUTED      0x0200

#define MDNS_NAME_REF               0xC000

#define MDNS_TYPE_A                 0x0001
#define MDNS_TYPE_PTR               0x000C
#define MDNS_TYPE_TXT               0x0010
#define MDNS_TYPE_AAAA              0x001C
#define MDNS_TYPE_SRV               0x0021
#define MDNS_TYPE_OPT               0x0029
#define MDNS_TYPE_NSEC              0x002F
#define MDNS_TYPE_ANY               0x00FF
#define MDNS_TYPE_SDPTR             0x0032//custom! only used by this logic

#define MDNS_CLASS_IN               0x0001
#define MDNS_CLASS_ANY              0x00FF
#define MDNS_CLASS_IN_FLUSH_CACHE   0x8001

#define MDNS_ANSWER_ALL             0x3F
#define MDNS_ANSWER_PTR             0x08
#define MDNS_ANSWER_TXT             0x04
#define MDNS_ANSWER_SRV             0x02
#define MDNS_ANSWER_A               0x01
#define MDNS_ANSWER_AAAA            0x10
#define MDNS_ANSWER_NSEC            0x20
#define MDNS_ANSWER_SDPTR           0x80

#define MDNS_SERVICE_PORT           5353                    // UDP port that the server runs on
#define MDNS_SERVICE_STACK_DEPTH    4096                    // Stack size for the service thread
#define MDNS_PACKET_QUEUE_LEN       16                      // Maximum packets that can be queued for parsing
#define MDNS_ACTION_QUEUE_LEN       16                      // Maximum actions pending to the server
#define MDNS_TXT_MAX_LEN            1024                    // Maximum string length of text data in TXT record
#define MDNS_NAME_MAX_LEN           64                      // Maximum string length of hostname, instance, service and proto
#define MDNS_NAME_BUF_LEN           (MDNS_NAME_MAX_LEN+1)   // Maximum char buffer size to hold hostname, instance, service or proto
#define MDNS_MAX_PACKET_SIZE        1460                    // Maximum size of mDNS  outgoing packet

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

#define MDNS_SERVICE_TASK_PRIORITY  5

typedef enum {
    PCB_OFF, PCB_DUP, PCB_INIT,
    PCB_PROBE_1, PCB_PROBE_2, PCB_PROBE_3,
    PCB_ANNOUNCE_1, PCB_ANNOUNCE_2, PCB_ANNOUNCE_3,
    PCB_RUNNING
} mdns_pcb_state_t;

typedef enum {
    PCB_TYPE_V4,
    PCB_TYPE_V6,
    PCB_TYPE_MAX
} mdns_pcb_type_t;

typedef enum {
    MDNS_ANSWER, MDNS_NS, MDNS_EXTRA
} mdns_parsed_recort_type_t;

typedef enum {
    ACTION_SYSTEM_EVENT,
    ACTION_HOSTNAME_SET,
    ACTION_INSTANCE_SET,
    ACTION_SERVICE_ADD,
    ACTION_SERVICE_DEL,
    ACTION_SERVICE_INSTANCE_SET,
    ACTION_SERVICE_PORT_SET,
    ACTION_SERVICE_TXT_SET,
    ACTION_SERVICE_TXT_DEL,
    ACTION_SERVICES_CLEAR,
    ACTION_MAX
} mdns_action_type_t;


typedef struct {
    uint16_t id;
    union {
        struct {
            uint16_t qr :1;
            uint16_t opCode :4;
            uint16_t aa :1;
            uint16_t tc :1;
            uint16_t rd :1;
            uint16_t ra :1;
            uint16_t z :1;
            uint16_t ad :1;
            uint16_t cd :1;
            uint16_t rCode :4;//response/error code
        };
        uint16_t value;
    } flags;
    uint16_t questions; //QDCOUNT
    uint16_t answers;   //ANCOUNT
    uint16_t servers;   //NSCOUNT
    uint16_t additional;//ARCOUNT
} mdns_header_t;

typedef struct {
    char host[MDNS_NAME_BUF_LEN];
    char service[MDNS_NAME_BUF_LEN];
    char proto[MDNS_NAME_BUF_LEN];
    char domain[MDNS_NAME_BUF_LEN];
    uint8_t parts;
    uint8_t sub;
} mdns_name_t;

typedef struct mdns_parsed_question_s {
    struct mdns_parsed_question_s * next;
    uint16_t type;
    bool unicast;
    char * host;
    char * service;
    char * proto;
    char * domain;
} mdns_parsed_question_t;

typedef struct mdns_parsed_record_s {
    struct mdns_parsed_record_s * next;
    mdns_parsed_recort_type_t record_type;
    uint16_t type;
    uint16_t clas;
    uint8_t flush;
    uint32_t ttl;
    char * host;
    char * service;
    char * proto;
    char * domain;
    uint16_t data_len;
    uint8_t *data;
} mdns_parsed_record_t;

typedef struct {
    tcpip_adapter_if_t tcpip_if;
    mdns_pcb_type_t pcb_type;
    //struct udp_pcb *pcb;
    ip_addr_t src;
    uint16_t src_port;
    uint8_t multicast;
    uint8_t authoritative;
    uint8_t probe;
    uint8_t discovery;
    uint8_t distributed;
    mdns_parsed_question_t * questions;
    mdns_parsed_record_t * records;
} mdns_parsed_packet_t;

typedef struct {
    tcpip_adapter_if_t tcpip_if;
    mdns_pcb_type_t pcb_type;
    struct pbuf *pb;
    ip_addr_t src;
    ip_addr_t dest;
    uint16_t src_port;
    uint8_t multicast;
} mdns_rx_packet_t;


typedef struct {
    const char * instance;
    const char * service;
    const char * proto;
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    mdns_txt_linked_item_t * txt;
} mdns_service_t;

typedef struct mdns_srv_item_s {
    struct mdns_srv_item_s * next;
    mdns_service_t * service;
} mdns_srv_item_t;


typedef struct mdns_out_question_s {
    struct mdns_out_question_s * next;
    uint16_t type;
    uint8_t unicast;
    const char * host;
    const char * service;
    const char * proto;
    const char * domain;
} mdns_out_question_t;

typedef struct mdns_out_answer_s {
    struct mdns_out_answer_s * next;
    uint16_t type;
    uint8_t bye;
    uint8_t flush;
    mdns_srv_item_t * service;
} mdns_out_answer_t;

typedef struct mdns_tx_packet_s {
    struct mdns_tx_packet_s * next;
    TickType_t send_at;
    tcpip_adapter_if_t tcpip_if;
    mdns_pcb_type_t pcb_type;
    ip_addr_t dst;
    uint16_t port;
    uint16_t flags;
    uint8_t distributed;
    mdns_out_question_t * questions;
    mdns_out_answer_t * answers;
    mdns_out_answer_t * servers;
    mdns_out_answer_t * additional;
} mdns_tx_packet_t;

typedef struct {
    mdns_pcb_state_t state;
    struct udp_pcb * pcb;
    mdns_srv_item_t ** probe_services;
    uint8_t probe_services_len;
    uint8_t probe_ip;
    uint8_t probe_running;
    uint16_t failed_probes;
} mdns_pcb_t;

typedef struct mdns_server_s {
    struct {
        mdns_pcb_t pcbs[PCB_TYPE_MAX];
    } interfaces[TCPIP_ADAPTER_IF_MAX];
    const char * hostname;
    const char * instance;
    mdns_srv_item_t * services;
    xSemaphoreHandle lock;
    xQueueHandle action_queue;
    xQueueHandle rx_queue;
    mdns_tx_packet_t * tx_queue;
    struct {
        char host[MDNS_NAME_BUF_LEN];
        char service[MDNS_NAME_BUF_LEN];
        char proto[MDNS_NAME_BUF_LEN];
        bool running;
        xSemaphoreHandle lock;
        mdns_result_t * results;
    } search;
} mdns_server_t;

typedef struct {
    mdns_action_type_t type;
    union {
        char * hostname;
        char * instance;
        system_event_id_t event;
        struct {
            mdns_srv_item_t * service;
        } srv_add;
        struct {
            mdns_srv_item_t * service;
        } srv_del;
        struct {
            mdns_srv_item_t * service;
            char * instance;
        } srv_instance;
        struct {
            mdns_srv_item_t * service;
            uint16_t port;
        } srv_port;
        struct {
            mdns_srv_item_t * service;
            char * key;
            char * value;
        } srv_txt_set;
        struct {
            mdns_srv_item_t * service;
            char * key;
        } srv_txt_del;
    } data;
} mdns_action_t;

void mdns_debug_packet(const uint8_t * data, size_t len);

#define PCB_STATE_IS_PROBING(s) (s->state > PCB_OFF && s->state < PCB_ANNOUNCE_1)
#define PCB_STATE_IS_ANNOUNCING(s) (s->state > PCB_PROBE_3 && s->state < PCB_RUNNING)
#define PCB_STATE_IS_RUNNING(s) (s->state == PCB_RUNNING)

#define MDNS_MUTEX_LOCK()       //xSemaphoreTake(_mdns_server->lock, portMAX_DELAY)
#define MDNS_MUTEX_UNLOCK()     //xSemaphoreGive(_mdns_server->lock)

#define MDNS_SEARCH_LOCK()      xSemaphoreTake(_mdns_server->search.lock, portMAX_DELAY)
#define MDNS_SEARCH_UNLOCK()    xSemaphoreGive(_mdns_server->search.lock)

static const char * MDNS_DEFAULT_DOMAIN = "local";
static const char * MDNS_SUB_STR = "_sub";

static mdns_server_t * _mdns_server = NULL;

static uint16_t _mdns_ptr_ttl = MDNS_ANSWER_PTR_TTL;
static uint16_t _mdns_srv_ttl = MDNS_ANSWER_SRV_TTL;
static uint16_t _mdns_txt_ttl = MDNS_ANSWER_TXT_TTL;
static uint16_t _mdns_a_ttl = MDNS_ANSWER_A_TTL;
static uint16_t _mdns_aaaa_ttl = MDNS_ANSWER_AAAA_TTL;

void mdns_ptr_ttl_set(uint16_t ttl)
{
    _mdns_ptr_ttl = ttl;
}

void mdns_srv_ttl_set(uint16_t ttl)
{
    _mdns_srv_ttl = ttl;
}

void mdns_txt_ttl_set(uint16_t ttl)
{
    _mdns_txt_ttl = ttl;
}

void mdns_a_ttl_set(uint16_t ttl)
{
    _mdns_a_ttl = ttl;
}

void mdns_ptr_aaaa_set(uint16_t ttl)
{
    _mdns_aaaa_ttl = ttl;
}

#ifndef MDNS_TEST_MODE

static TaskHandle_t _mdns_service_task_handle = NULL;

static xSemaphoreHandle _mdns_service_semaphore = NULL;
#define MDNS_SERVICE_LOCK()     xSemaphoreTake(_mdns_service_semaphore, portMAX_DELAY)
#define MDNS_SERVICE_UNLOCK()   xSemaphoreGive(_mdns_service_semaphore)

#define queueToEnd(type, queue, item)       \
    if (!queue) {                           \
        queue = item;                       \
    } else {                                \
        type * _q = queue;                  \
        while (_q->next) { _q = _q->next; } \
        _q->next = item;                    \
    }

#define queueFree(type, queue)  while (queue) { type * _q = queue; queue = queue->next; free(_q); }

/*
 * Appends/increments a number to name/instance in case of collision
 * */
static char * _mdns_mangle_name(char * name)
{
    char test[10];
    size_t len = strlen(name);
    int name_end = len;
    int new_val = 2;
    size_t new_len = len;
    char *p = strrchr(name, '-');
    if (p) {
        char * num = p+1;
        int val = atoi(num);
        sprintf(test, "%d", val);
        if (strcmp(num, test) == 0) {
            //cool we have the number
            new_val = val + 1;
            name_end = p - name;
        }
    }
    sprintf(test, "%d", new_val);
    new_len = name_end + strlen(test) + 2;
    char * new_name = (char *)malloc(new_len);
    if (!new_name) {
        name[len - 1] = '2';
        return NULL;
    }
    if (name_end < len) {
        name[name_end] = 0;
    }
    sprintf(new_name, "%s-%d", name, new_val);
    return new_name;
}

/**
 * @brief  finds service from given service type
 * @param  server       the server
 * @param  service      service type to match
 * @param  proto        proto to match
 *
 * @return the service item if found or NULL on error
 */
static mdns_srv_item_t * _mdns_get_service_item(const char * service, const char * proto)
{
    mdns_srv_item_t * s = _mdns_server->services;
    while (s) {
        if (!strcasecmp(s->service->service, service) && !strcasecmp(s->service->proto, proto)) {
            return s;
        }
        s = s->next;
    }
    return NULL;
}

/*
 * MDNS Server Networking
 * */

/**
 * @brief  the receive callback of the raw udp api. Packets are received here
 *
 */
static void _udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *pb, const ip_addr_t *raddr, uint16_t rport)
{
    uint8_t i;
    while (pb != NULL) {
        struct pbuf * this_pb = pb;
        pb = pb->next;
        this_pb->next = NULL;

        mdns_rx_packet_t * packet = (mdns_rx_packet_t *)malloc(sizeof(mdns_rx_packet_t));
        if (!packet) {
            //missed packet - no memory
            pbuf_free(this_pb);
            continue;
        }

        packet->tcpip_if = TCPIP_ADAPTER_IF_MAX;
        packet->pb = this_pb;
        packet->src_port = rport;
        memcpy(&packet->src, raddr, sizeof(ip_addr_t));
        packet->dest.type = packet->src.type;

        if (packet->src.type == IPADDR_TYPE_V4) {
            packet->pcb_type = PCB_TYPE_V4;
            struct ip_hdr * iphdr = (struct ip_hdr *)(((uint8_t *)(packet->pb->payload)) - UDP_HLEN - IP_HLEN);
            packet->dest.u_addr.ip4.addr = iphdr->dest.addr;
        } else {
            packet->pcb_type = PCB_TYPE_V6;
            struct ip6_hdr * ip6hdr = (struct ip6_hdr *)(((uint8_t *)(packet->pb->payload)) - UDP_HLEN - IP6_HLEN);
            memcpy(&packet->dest.u_addr.ip6.addr, (uint8_t *)ip6hdr->dest.addr, 16);
        }
        packet->multicast = ip_addr_ismulticast(&(packet->dest));
        //lwip does not return the proper pcb if you have more than one for the same multicast address (but different interfaces)
        struct netif * netif = NULL;
        struct udp_pcb * pcb = NULL;
        for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
            pcb = _mdns_server->interfaces[i].pcbs[packet->pcb_type].pcb;
            tcpip_adapter_get_netif (i, &netif);
            if (pcb && netif && netif == ip_current_input_netif ()) {
                if (packet->src.type == IPADDR_TYPE_V4) {
                    if ((packet->src.u_addr.ip4.addr & netif->netmask.u_addr.ip4.addr) != (netif->ip_addr.u_addr.ip4.addr & netif->netmask.u_addr.ip4.addr)) {
                        //packet source is not in the same subnet
                        pcb = NULL;
                        break;
                    }
                }
                packet->tcpip_if = i;
                break;
            }
            pcb = NULL;
        }

        if (!pcb || !_mdns_server || !_mdns_server->rx_queue || xQueueSend(_mdns_server->rx_queue, &packet, (portTickType)0) != pdPASS) {
            pbuf_free(this_pb);
            free(packet);
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

static void _udp_pcb_deinit(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type)
{
    if (!_mdns_server) {
        return;
    }
    mdns_pcb_t * _pcb = &_mdns_server->interfaces[tcpip_if].pcbs[pcb_type];
    if (_pcb->pcb) {
        _pcb->state = PCB_OFF;
        udp_recv(_pcb->pcb, NULL, NULL);
        udp_disconnect(_pcb->pcb);
        udp_remove(_pcb->pcb);
        free(_pcb->probe_services);
        _pcb->pcb = NULL;
        _pcb->probe_ip = false;
        _pcb->probe_services = NULL;
        _pcb->probe_services_len = 0;
        _pcb->probe_running = false;
        _pcb->failed_probes = 0;
    }
}

static esp_err_t _udp_pcb_v4_init(tcpip_adapter_if_t tcpip_if)
{
    tcpip_adapter_ip_info_t if_ip_info;

    if (!_mdns_server || _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V4].pcb) {
        return ESP_ERR_INVALID_STATE;
    }

    if (tcpip_adapter_get_ip_info(tcpip_if, &if_ip_info) || if_ip_info.ip.addr == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ip_addr_t interface_addr = IPADDR4_INIT(if_ip_info.ip.addr);

    ip_addr_t multicast_addr;
    IP_ADDR4(&multicast_addr, 224, 0, 0, 251);

    if (igmp_joingroup((const struct ip4_addr *)&interface_addr.u_addr.ip4, (const struct ip4_addr *)&multicast_addr.u_addr.ip4)) {
        return ESP_ERR_INVALID_STATE;
    }

    struct udp_pcb * pcb = udp_new_ip_type(IPADDR_TYPE_V4);
    if (!pcb) {
        return ESP_ERR_NO_MEM;
    }

    if (udp_bind(pcb, &interface_addr, MDNS_SERVICE_PORT) != 0) {
        udp_remove(pcb);
        return ESP_ERR_INVALID_STATE;
    }

    pcb->mcast_ttl = 1;
    pcb->remote_port = MDNS_SERVICE_PORT;
    ip_addr_copy(pcb->multicast_ip, interface_addr);
    ip_addr_copy(pcb->remote_ip, multicast_addr);

    _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V4].pcb = pcb;
    _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V4].failed_probes = 0;
    udp_recv(pcb, &_udp_recv, _mdns_server);

    return ESP_OK;
}

static esp_err_t _udp_pcb_v6_init(tcpip_adapter_if_t tcpip_if)
{
    ip_addr_t multicast_addr = IPADDR6_INIT(0x000002ff, 0, 0, 0xfb000000);
    ip_addr_t interface_addr;
    interface_addr.type = IPADDR_TYPE_V6;

    if (!_mdns_server || _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V6].pcb) {
        return ESP_ERR_INVALID_STATE;
    }

    if (tcpip_adapter_get_ip6_linklocal(tcpip_if, &interface_addr.u_addr.ip6)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (mld6_joingroup(&(interface_addr.u_addr.ip6), &(multicast_addr.u_addr.ip6))) {
        return ESP_ERR_INVALID_STATE;
    }

    struct udp_pcb * pcb = udp_new_ip_type(IPADDR_TYPE_V6);
    if (!pcb) {
        return ESP_ERR_NO_MEM;
    }

    if (udp_bind(pcb, &interface_addr, MDNS_SERVICE_PORT) != 0) {
        udp_remove(pcb);
        return ESP_ERR_INVALID_STATE;
    }

    pcb->remote_port = MDNS_SERVICE_PORT;
    ip_addr_copy(pcb->remote_ip, multicast_addr);

    _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V6].pcb = pcb;
    _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V6].failed_probes = 0;
    udp_recv(pcb, &_udp_recv, _mdns_server);

    return ESP_OK;
}

static esp_err_t _udp_pcb_init(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type) {
    if (pcb_type == PCB_TYPE_V4) {
        return _udp_pcb_v4_init(tcpip_if);
    } else if (pcb_type == PCB_TYPE_V6) {
        return _udp_pcb_v6_init(tcpip_if);
    }
    return ESP_ERR_INVALID_ARG;
}

typedef struct {
    struct tcpip_api_call call;
    tcpip_adapter_if_t tcpip_if;
    mdns_pcb_type_t pcb_type;
    esp_err_t err;
} mdns_api_call_t;

static err_t _mdns_pcb_init_api(struct tcpip_api_call *api_call_msg)
{
    mdns_api_call_t * msg = (mdns_api_call_t *)api_call_msg;
    msg->err = _udp_pcb_init(msg->tcpip_if, msg->pcb_type);
    return msg->err;
}

static esp_err_t _mdns_pcb_init(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type) {
    mdns_api_call_t msg = {
        .tcpip_if = tcpip_if,
        .pcb_type = pcb_type
    };
    tcpip_api_call(_mdns_pcb_init_api, (struct tcpip_api_call*)&msg);
    return msg.err;
}

static err_t _mdns_pcb_deinit_api(struct tcpip_api_call *api_call_msg)
{
    mdns_api_call_t * msg = (mdns_api_call_t *)api_call_msg;
    _udp_pcb_deinit(msg->tcpip_if, msg->pcb_type);
    msg->err = ESP_OK;
    return ESP_OK;
}

static esp_err_t _mdns_pcb_deinit(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type) {
    mdns_api_call_t msg = {
        .tcpip_if = tcpip_if,
        .pcb_type = pcb_type
    };
    tcpip_api_call(_mdns_pcb_deinit_api, (struct tcpip_api_call*)&msg);
    return msg.err;
}

#endif


/**
 * @brief  send packet over UDP
 *
 * @param  server       The server
 * @param  data         byte array containing the packet data
 * @param  len          length of the packet data
 *
 * @return length of sent packet or 0 on error
 */
static size_t _udp_pcb_write(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type, const ip_addr_t *ip, uint16_t port, uint8_t * data, size_t len)
{
#ifndef MDNS_TEST_MODE
    struct netif * netif = NULL;
    if(tcpip_adapter_get_netif(tcpip_if, &netif)){
        return 0;
    }

    struct pbuf* pbt = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (pbt == NULL) {
        return 0;
    }
    memcpy((uint8_t *)pbt->payload, data, len);

    err_t err = udp_sendto_if(_mdns_server->interfaces[tcpip_if].pcbs[pcb_type].pcb, pbt, ip, port, netif);
    pbuf_free(pbt);
    if (err) {
        return 0;
    }
#endif
    return len;
}


/*
 * Packet construction
 * */

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
    while (start[index]) {
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
            for (i=0; i<len; i++) {
                buf[i] = start[index++];
            }
            buf[len] = '\0';
            if (name->parts == 1 && buf[0] != '_'
                    && (strcasecmp(buf, MDNS_DEFAULT_DOMAIN) != 0)
                    && (strcasecmp(buf, "ip6") != 0)
                    && (strcasecmp(buf, "in-addr") != 0)) {
                snprintf((char*)name, MDNS_NAME_BUF_LEN, "%s.%s", name->host, buf);
            } else if (strcasecmp(buf, MDNS_SUB_STR) == 0) {
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
static inline uint8_t _mdns_append_type(uint8_t * packet, uint16_t * index, uint8_t type, bool flush, uint32_t ttl)
{
    if ((*index + 10) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    uint16_t clas = MDNS_CLASS_IN;
    if (flush) {
        clas = MDNS_CLASS_IN_FLUSH_CACHE;
    }
    if (type == MDNS_ANSWER_PTR) {
        _mdns_append_u16(packet, index, MDNS_TYPE_PTR);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_TXT) {
        _mdns_append_u16(packet, index, MDNS_TYPE_TXT);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_SRV) {
        _mdns_append_u16(packet, index, MDNS_TYPE_SRV);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_A) {
        _mdns_append_u16(packet, index, MDNS_TYPE_A);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_AAAA) {
        _mdns_append_u16(packet, index, MDNS_TYPE_AAAA);
        _mdns_append_u16(packet, index, clas);
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
    while (len_location) {
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
            for (i=0; i<count; i++) {
                if (strcasecmp(strings[i], (const char *)&name + (i * (MDNS_NAME_BUF_LEN)))) {
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
static uint16_t _mdns_append_ptr_record(uint8_t * packet, uint16_t * index, mdns_service_t * service, bool flush, bool bye)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
             :(_mdns_server->instance)?_mdns_server->instance
             :_mdns_server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str + 1, 3);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_PTR, false, bye?0:_mdns_ptr_ttl);
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
 * @brief  appends DNS-SD PTR record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_sdptr_record(uint8_t * packet, uint16_t * index, mdns_service_t * service, bool flush, bool bye)
{
    const char * str[3];
    const char * sd_str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    sd_str[0] = (char*)"_services";
    sd_str[1] = (char*)"_dns-sd";
    sd_str[2] = (char*)"_udp";
    sd_str[3] = MDNS_DEFAULT_DOMAIN;

    str[0] = service->service;
    str[1] = service->proto;
    str[2] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, sd_str, 4);

    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_PTR, flush, _mdns_ptr_ttl);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;
    part_length = _mdns_append_fqdn(packet, index, str, 3);
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
static uint16_t _mdns_append_txt_record(uint8_t * packet, uint16_t * index, mdns_service_t * service, bool flush, bool bye)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
             :(_mdns_server->instance)?_mdns_server->instance
             :_mdns_server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_TXT, flush, bye?0:_mdns_txt_ttl);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;
    uint16_t data_len = 1;

    char * tmp;
    mdns_txt_linked_item_t * txt = service->txt;
    while (txt) {
        tmp = (char *)malloc(2 + strlen(txt->key) + strlen(txt->value));
        if (tmp) {
            sprintf(tmp, "%s=%s", txt->key, txt->value);
            uint8_t l = _mdns_append_string(packet, index, tmp);
            free(tmp);
            if (!l) {
                return 0;
            }
            data_len += l;
        }
        txt = txt->next;
    }

    packet[*index] = 0;
    *index = *index + 1;
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
static uint16_t _mdns_append_srv_record(uint8_t * packet, uint16_t * index, mdns_service_t * service, bool flush, bool bye)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
             :(_mdns_server->instance)?_mdns_server->instance
             :_mdns_server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_SRV, flush, bye?0:_mdns_srv_ttl);
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

    str[0] = _mdns_server->hostname;
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
static uint16_t _mdns_append_a_record(uint8_t * packet, uint16_t * index, uint32_t ip, bool flush, bool bye)
{
    const char * str[2];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = _mdns_server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_A, flush, bye?0:_mdns_a_ttl);
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
 * @param  ipv6         the IPv6 address to add
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_aaaa_record(uint8_t * packet, uint16_t * index, uint8_t * ipv6, bool flush, bool bye)
{
    const char * str[2];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = _mdns_server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_AAAA, flush, bye?0:_mdns_aaaa_ttl);
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

static uint16_t _mdns_append_question(uint8_t * packet, uint16_t * index, mdns_out_question_t * q)
{
    const char * str[4];
    uint8_t str_index = 0;
    uint8_t part_length;
    if (q->host) {
        str[str_index++] = q->host;
    }
    if (q->service) {
        str[str_index++] = q->service;
    }
    if (q->proto) {
        str[str_index++] = q->proto;
    }
    if (q->domain) {
        str[str_index++] = q->domain;
    }

    part_length = _mdns_append_fqdn(packet, index, str, str_index);
    if (!part_length) {
        return 0;
    }

    part_length += _mdns_append_u16(packet, index, q->type);
    part_length += _mdns_append_u16(packet, index, q->unicast?0x8001:0x0001);
    return part_length;
}

static tcpip_adapter_if_t _mdns_get_other_if(tcpip_adapter_if_t tcpip_if)
{
    if (tcpip_if == TCPIP_ADAPTER_IF_STA) {
        return TCPIP_ADAPTER_IF_ETH;
    } else if (tcpip_if == TCPIP_ADAPTER_IF_ETH) {
        return TCPIP_ADAPTER_IF_STA;
    }
    return TCPIP_ADAPTER_IF_MAX;
}

static bool _mdns_if_is_dup(tcpip_adapter_if_t tcpip_if)
{
    tcpip_adapter_if_t other_if = _mdns_get_other_if(tcpip_if);
    if (other_if == TCPIP_ADAPTER_IF_MAX) {
        return false;
    }
    if (_mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V4].state == PCB_DUP
        || _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V6].state == PCB_DUP
        || _mdns_server->interfaces[other_if].pcbs[PCB_TYPE_V4].state == PCB_DUP
        || _mdns_server->interfaces[other_if].pcbs[PCB_TYPE_V6].state == PCB_DUP
    ) {
        return true;
    }
    return false;
}

static bool _ipv6_address_is_zero(ip6_addr_t ip6)
{
    uint8_t i;
    uint8_t * data = (uint8_t *)ip6.addr;
    for (i=0; i<16; i++) {
        if (data[i]) {
            return false;
        }
    }
    return true;
}

static uint8_t _mdns_append_answer(uint8_t * packet, uint16_t * index, mdns_out_answer_t * answer, tcpip_adapter_if_t tcpip_if) {
    if (answer->type == MDNS_TYPE_PTR) {
        return _mdns_append_ptr_record(packet, index, answer->service->service, answer->flush, answer->bye) > 0;
    } else if (answer->type == MDNS_TYPE_SRV) {
        return _mdns_append_srv_record(packet, index, answer->service->service, answer->flush, answer->bye) > 0;
    } else if (answer->type == MDNS_TYPE_TXT) {
        return _mdns_append_txt_record(packet, index, answer->service->service, answer->flush, answer->bye) > 0;
    } else if (answer->type == MDNS_TYPE_SDPTR) {
        return _mdns_append_sdptr_record(packet, index, answer->service->service, answer->flush, answer->bye) > 0;
    } else if (answer->type == MDNS_TYPE_A) {
        tcpip_adapter_ip_info_t if_ip_info;
        if(!_mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V4].pcb && _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V4].state != PCB_DUP){
            return 0;//interface not active
        }
        if (tcpip_adapter_get_ip_info(tcpip_if, &if_ip_info)) {
            return 0;
        }
        if (_mdns_append_a_record(packet, index, if_ip_info.ip.addr, answer->flush, answer->bye) <= 0) {
            return 0;
        }
        if (!_mdns_if_is_dup(tcpip_if)) {
            return 1;
        }
        tcpip_adapter_if_t other_if = _mdns_get_other_if(tcpip_if);
        if (tcpip_adapter_get_ip_info(other_if, &if_ip_info)) {
            return 1;
        }
        if (_mdns_append_a_record(packet, index, if_ip_info.ip.addr, answer->flush, answer->bye) > 0) {
            return 2;
        }
        return 1;
    } else if (answer->type == MDNS_TYPE_AAAA) {
        struct ip6_addr if_ip6;
        if(!_mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V6].pcb && _mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V6].state != PCB_DUP){
            return 0;//interface not active
        }
        if (tcpip_adapter_get_ip6_linklocal(tcpip_if, &if_ip6)) {
            return 0;
        }
        if (_ipv6_address_is_zero(if_ip6)) {
            return 0;
        }
        if (_mdns_append_aaaa_record(packet, index, (uint8_t*)if_ip6.addr, answer->flush, answer->bye) <= 0) {
            return 0;
        }
        if (!_mdns_if_is_dup(tcpip_if)) {
            return 1;
        }
        tcpip_adapter_if_t other_if = _mdns_get_other_if(tcpip_if);
        if (tcpip_adapter_get_ip6_linklocal(other_if, &if_ip6)) {
            return 1;
        }
        if (_mdns_append_aaaa_record(packet, index, (uint8_t*)if_ip6.addr, answer->flush, answer->bye) > 0) {
            return 2;
        }
        return 1;
    }
    return 0;
}

/**
 * @brief  sends a packet
 *
 * @param  p       the packet
 */
static void _mdns_dispatch_tx_packet(mdns_tx_packet_t * p) {
    static uint8_t packet[MDNS_MAX_PACKET_SIZE];
    uint16_t index = MDNS_HEAD_LEN;
    memset(packet, 0, MDNS_HEAD_LEN);
    mdns_out_question_t * q;
    mdns_out_answer_t * a;
    uint8_t count;

    _mdns_set_u16(packet, MDNS_HEAD_FLAGS_OFFSET, p->flags);

    count = 0;
    q = p->questions;
    while (q) {
        if (_mdns_append_question(packet, &index, q)) {
            count++;
        }
        q = q->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_QUESTIONS_OFFSET, count);

    count = 0;
    a = p->answers;
    while (a) {
        count += _mdns_append_answer(packet, &index, a, p->tcpip_if);
        a = a->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_ANSWERS_OFFSET, count);

    count = 0;
    a = p->servers;
    while (a) {
        count += _mdns_append_answer(packet, &index, a, p->tcpip_if);
        a = a->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_SERVERS_OFFSET, count);

    count = 0;
    a = p->additional;
    while (a) {
        count += _mdns_append_answer(packet, &index, a, p->tcpip_if);
        a = a->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_ADDITIONAL_OFFSET, count);

    _mdns_dbg_printf("\nTX[%u][%u]: ", p->tcpip_if, p->pcb_type);
    if (p->dst.type == IPADDR_TYPE_V4) {
        _mdns_dbg_printf("To: " IPSTR ":%u, ", IP2STR(&p->dst.u_addr.ip4), p->port);
    } else {
        _mdns_dbg_printf("To: " IPV6STR ":%u, ", IPV62STR(p->dst.u_addr.ip6), p->port);
    }
    mdns_debug_packet(packet, index);

    _udp_pcb_write(p->tcpip_if, p->pcb_type, &p->dst, p->port, packet, index);
}

/**
 * @brief  frees a packet
 *
 * @param  packet       the packet
 */
static void _mdns_free_tx_packet(mdns_tx_packet_t * packet) {
    if (!packet) {
        return;
    }
    queueFree(mdns_out_question_t, packet->questions);
    queueFree(mdns_out_answer_t, packet->answers);
    queueFree(mdns_out_answer_t, packet->servers);
    queueFree(mdns_out_answer_t, packet->additional);
    free(packet);
}

/**
 * @brief  schedules a packet to be sent after given milliseconds
 *
 * @param  packet       the packet
 * @param  ms_after     number of milliseconds after which the packet should be dispatched
 */
static void _mdns_schedule_tx_packet(mdns_tx_packet_t * packet, TickType_t ms_after) {
    if (!packet) {
        return;
    }
    packet->send_at = (xTaskGetTickCount() * portTICK_PERIOD_MS) + ms_after;
    packet->next = NULL;
    if (!_mdns_server->tx_queue || _mdns_server->tx_queue->send_at > packet->send_at) {
        packet->next = _mdns_server->tx_queue;
        _mdns_server->tx_queue = packet;
        return;
    }
    mdns_tx_packet_t * q = _mdns_server->tx_queue;
    while (q->next && q->next->send_at <= packet->send_at) {
        q = q->next;
    }
    packet->next = q->next;
    q->next = packet;
}

/**
 * @brief  free all packets sheduled for sending
 */
static void _mdns_clear_tx_queue() {
    mdns_tx_packet_t * q;
    while (_mdns_server->tx_queue) {
        q = _mdns_server->tx_queue;
        _mdns_server->tx_queue = _mdns_server->tx_queue->next;
        _mdns_free_tx_packet(q);
    }
}

/**
 * @brief  clear packets scheduled for sending on a specific interface
 *
 * @param  tcpip_if     the interface
 * @param  pcb_type     pcb type V4/V6
 */
static void _mdns_clear_pcb_tx_queue(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type)
{
    mdns_tx_packet_t * q, * p;
    while (_mdns_server->tx_queue && _mdns_server->tx_queue->tcpip_if == tcpip_if && _mdns_server->tx_queue->pcb_type == pcb_type) {
        q = _mdns_server->tx_queue;
        _mdns_server->tx_queue = _mdns_server->tx_queue->next;
        _mdns_free_tx_packet(q);
    }
    if (_mdns_server->tx_queue) {
        q = _mdns_server->tx_queue;
        while (q->next) {
            if (q->next->tcpip_if == tcpip_if && q->next->pcb_type == pcb_type) {
                p = q->next;
                q->next = p->next;
                _mdns_free_tx_packet(p);
            } else {
                q = q->next;
            }
        }
    }
}

/**
 * @brief  get the next packet scheduled for sending on a specific interface
 *
 * @param  tcpip_if     the interface
 * @param  pcb_type     pcb type V4/V6
 */
static mdns_tx_packet_t * _mdns_get_next_pcb_packet(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type) {
    mdns_tx_packet_t * q = _mdns_server->tx_queue;
    while (q) {
        if (q->tcpip_if == tcpip_if && q->pcb_type == pcb_type) {
            return q;
        }
        q = q->next;
    }
    return NULL;
}

static void _mdns_remove_scheduled_answer(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type, uint16_t type, mdns_srv_item_t * service)
{
    mdns_tx_packet_t * q = _mdns_server->tx_queue;
    while (q) {
        if (q->tcpip_if == tcpip_if && q->pcb_type == pcb_type && q->distributed) {
            mdns_out_answer_t * a = q->answers;
            if (a->type == type && a->service == service) {
                q->answers = q->answers->next;
                free(a);
            } else {
                while (a->next) {
                    if (a->next->type == type && a->next->service == service) {
                        mdns_out_answer_t * b = a->next;
                        a->next = b->next;
                        free(b);
                        break;
                    }
                    a = a->next;
                }
            }
        }
        q = q->next;
    }
}

static void _mdns_dealloc_answer(mdns_out_answer_t ** destnation, uint16_t type, mdns_srv_item_t * service)
{
    mdns_out_answer_t * d = *destnation;
    if(!d){
        return;
    }
    if (d->type == type && d->service == service) {
        *destnation = d->next;
        free(d);
        return;
    }
    while(d->next){
        mdns_out_answer_t * a = d->next;
        if (a->type == type && a->service == service) {
            d->next = a->next;
            free(a);
            return;
        }
        d = d->next;
    }
}

static bool _mdns_alloc_answer(mdns_out_answer_t ** destnation, uint16_t type, mdns_srv_item_t * service, bool flush, bool bye) {
    mdns_out_answer_t * d = *destnation;
    while (d) {
        if (d->type == type && d->service == service) {
            return true;
        }
        d = d->next;
    }

    mdns_out_answer_t * a = (mdns_out_answer_t *)malloc(sizeof(mdns_out_answer_t));
    if (!a) {
        return false;
    }
    a->type = type;
    a->service = service;
    a->bye = bye;
    a->flush = flush;
    a->next = NULL;
    queueToEnd(mdns_out_answer_t, *destnation, a);
    return true;
}

static mdns_tx_packet_t * _mdns_alloc_packet_default(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type)
{
    mdns_tx_packet_t * packet = (mdns_tx_packet_t*)malloc(sizeof(mdns_tx_packet_t));
    if (!packet) {
        return NULL;
    }
    memset((uint8_t*)packet, 0, sizeof(mdns_tx_packet_t));
    packet->tcpip_if = tcpip_if;
    packet->pcb_type = pcb_type;
    packet->port = MDNS_SERVICE_PORT;
    if (pcb_type == PCB_TYPE_V4) {
        IP_ADDR4(&packet->dst, 224, 0, 0, 251);
    } else {
        ip_addr_t addr = IPADDR6_INIT(0x000002ff, 0, 0, 0xfb000000);
        memcpy(&packet->dst, &addr, sizeof(ip_addr_t));
    }
    return packet;
}

static void _mdns_create_answer_from_parsed_packet(mdns_parsed_packet_t * parsed_packet)
{
    if (!parsed_packet->questions) {
        return;
    }
    bool send_flush = parsed_packet->src_port == MDNS_SERVICE_PORT;
    bool unicast = false;
    bool shared = false;
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(parsed_packet->tcpip_if, parsed_packet->pcb_type);
    if (!packet) {
        //mem error
        return;
    }
    packet->flags = MDNS_FLAGS_AUTHORITATIVE;
    packet->distributed = parsed_packet->distributed;

    mdns_parsed_question_t * q = parsed_packet->questions;
    while (q) {
        mdns_srv_item_t * service = NULL;
        if (q->service && q->proto) {
            service = _mdns_get_service_item(q->service, q->proto);
            if (!service) {
                continue;
            }
        }
        if (q->unicast) {
            unicast = true;
        }
        if (service) {
            if (q->type == MDNS_TYPE_PTR || q->type == MDNS_TYPE_ANY) {
                if (q->type == MDNS_TYPE_PTR) {
                    shared = true;
                }
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, service, false, false)
                        || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, service, send_flush, false)
                        || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, service, send_flush, false)
                        || !_mdns_alloc_answer(shared?&packet->additional:&packet->answers, MDNS_TYPE_A, NULL, send_flush, false)
                        || !_mdns_alloc_answer(shared?&packet->additional:&packet->answers, MDNS_TYPE_AAAA, NULL, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (q->type == MDNS_TYPE_SRV) {
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, service, send_flush, false)
                        || !_mdns_alloc_answer(&packet->additional, MDNS_TYPE_A, NULL, send_flush, false)
                        || !_mdns_alloc_answer(&packet->additional, MDNS_TYPE_AAAA, NULL, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (q->type == MDNS_TYPE_TXT) {
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, service, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (q->type == MDNS_TYPE_SDPTR) {
                shared = true;
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, service, false, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            }
        } else {
            if (q->type == MDNS_TYPE_ANY || q->type == MDNS_TYPE_A || q->type == MDNS_TYPE_AAAA) {
                //_mdns_dealloc_answer(&packet->additional, MDNS_TYPE_A, NULL);
                //_mdns_dealloc_answer(&packet->additional, MDNS_TYPE_AAAA, NULL);
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_A, NULL, send_flush, false)
                        || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_AAAA, NULL, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (!_mdns_alloc_answer(&packet->answers, q->type, NULL, send_flush, false)) {
                _mdns_free_tx_packet(packet);
                return;
            }
        }
        q = q->next;
    }
    if (unicast || !send_flush) {
        memcpy(&packet->dst, &parsed_packet->src, sizeof(ip_addr_t));
        packet->port = parsed_packet->src_port;
    }

    //non-shared packets go out now
    //shared steps go uniformly 20,46,72,98
    static uint8_t share_step = 0;
    if (shared) {
        _mdns_schedule_tx_packet(packet, 25 + (share_step * 25));
        share_step = (share_step + 1) & 0x03;
    } else {
        _mdns_printf("\n**FAST**\n");
        _mdns_dispatch_tx_packet(packet);
        _mdns_free_tx_packet(packet);
        //_mdns_schedule_tx_packet(packet, 0);
    }
}

static bool _mdns_question_exists(mdns_out_question_t * needle, mdns_out_question_t * haystack)
{
    while(haystack) {
        if (haystack->type == needle->type
            && haystack->host == needle->host
            && haystack->service == needle->service
            && haystack->proto == needle->proto) {
            return true;
        }
        haystack = haystack->next;
    }
    return false;
}

static mdns_tx_packet_t * _mdns_create_probe_packet(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type, mdns_srv_item_t * services[], size_t len, bool first, bool include_ip) {
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(tcpip_if, pcb_type);
    if (!packet) {
        //mem error
        return NULL;
    }

    size_t i;
    for (i=0; i<len; i++) {
        mdns_out_question_t * q = (mdns_out_question_t *)malloc(sizeof(mdns_out_question_t));
        if (!q) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
        q->next = NULL;
        q->unicast = first;
        q->type = MDNS_TYPE_ANY;
        q->host = (services[i]->service->instance)?services[i]->service->instance
                  :(_mdns_server->instance)?_mdns_server->instance
                  :_mdns_server->hostname;
        q->service = services[i]->service->service;
        q->proto = services[i]->service->proto;
        q->domain = MDNS_DEFAULT_DOMAIN;
        if (_mdns_question_exists(q, packet->questions)) {
            free(q);
        } else {
            queueToEnd(mdns_out_question_t, packet->questions, q);
        }

        if (!_mdns_alloc_answer(&packet->servers, MDNS_TYPE_SRV, services[i], false, false)) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
    }

    if (include_ip) {
        mdns_out_question_t * q = (mdns_out_question_t *)malloc(sizeof(mdns_out_question_t));
        if (!q) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
        q->next = NULL;
        q->unicast = first;
        q->type = MDNS_TYPE_ANY;
        q->host = _mdns_server->hostname;
        q->service = NULL;
        q->proto = NULL;
        q->domain = MDNS_DEFAULT_DOMAIN;
        if (_mdns_question_exists(q, packet->questions)) {
            free(q);
        } else {
            queueToEnd(mdns_out_question_t, packet->questions, q);
        }

        if (_mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V4].pcb) {
            if (!_mdns_alloc_answer(&packet->servers, MDNS_TYPE_A, NULL, false, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }
        }

        if (_mdns_server->interfaces[tcpip_if].pcbs[PCB_TYPE_V6].pcb) {
            if (!_mdns_alloc_answer(&packet->servers, MDNS_TYPE_AAAA, NULL, false, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }
        }
    }

    return packet;
}

static mdns_tx_packet_t * _mdns_create_announce_packet(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type, mdns_srv_item_t * services[], size_t len, bool include_ip) {
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(tcpip_if, pcb_type);
    if (!packet) {
        //mem error
        return NULL;
    }
    packet->flags = MDNS_FLAGS_AUTHORITATIVE;

    uint8_t i;
    for (i=0; i<len; i++) {
        if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SDPTR, services[i], false, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, services[i], false, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, services[i], true, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, services[i], true, false)) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
    }
    if (include_ip) {
        if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_A, NULL, true, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_AAAA, NULL, true, false)) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
    }
    return packet;
}

static mdns_tx_packet_t * _mdns_create_announce_from_probe(mdns_tx_packet_t * probe) {

    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(probe->tcpip_if, probe->pcb_type);
    if (!packet) {
        //mem error
        return NULL;
    }
    packet->flags = MDNS_FLAGS_AUTHORITATIVE;

    mdns_out_answer_t * s = probe->servers;
    while (s) {
        if (s->type == MDNS_TYPE_SRV) {
            if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SDPTR, s->service, false, false)
                    || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, s->service, false, false)
                    || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, s->service, true, false)
                    || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, s->service, true, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }

        } else if (s->type == MDNS_TYPE_A || s->type == MDNS_TYPE_AAAA) {
            if (!_mdns_alloc_answer(&packet->answers, s->type, NULL, true, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }
        }

        s = s->next;
    }
    return packet;
}

static void _mdns_pcb_send_bye(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type, mdns_srv_item_t ** services, size_t len, bool include_ip)
{
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(tcpip_if, pcb_type);
    if (!packet) {
        //mem error
        return;
    }
    size_t i;
    for (i=0; i<len; i++) {
        if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, services[i], true, true)) {
            _mdns_free_tx_packet(packet);
            return;
        }
    }
    if (include_ip && (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_A, NULL, true, true) || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_AAAA, NULL, true, true))) {
        _mdns_free_tx_packet(packet);
        return;
    }
    _mdns_dispatch_tx_packet(packet);
    _mdns_free_tx_packet(packet);
}

static void _mdns_init_pcb_probe(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type, mdns_srv_item_t ** services, size_t len, bool probe_ip) {
    mdns_pcb_t * _pcb = &_mdns_server->interfaces[tcpip_if].pcbs[pcb_type];
    size_t services_final_len = len;

    //clear all packets that might be queued
    _mdns_clear_pcb_tx_queue(tcpip_if, pcb_type);

    if (PCB_STATE_IS_PROBING(_pcb)) {
        //we have things that are already running
        services_final_len += _pcb->probe_services_len;
    }
    mdns_srv_item_t ** _services = NULL;
    if (services_final_len) {
        _services = (mdns_srv_item_t **)malloc(sizeof(mdns_srv_item_t *) * services_final_len);
        if (!_services) {
            //ToDo: handle better
            return;
        }

        size_t i;
        for (i=0; i<len; i++) {
            _services[i] = services[i];
        }
        if (_pcb->probe_services) {
            for (i=0; i<_pcb->probe_services_len; i++) {
                _services[len+i] = _pcb->probe_services[i];
            }
            free(_pcb->probe_services);
        }
    }

    probe_ip = _pcb->probe_ip || probe_ip;

    _pcb->probe_ip = false;
    _pcb->probe_services = NULL;
    _pcb->probe_services_len = 0;
    _pcb->probe_running = false;

    mdns_tx_packet_t * packet = _mdns_create_probe_packet(tcpip_if, pcb_type, _services, services_final_len, true, probe_ip);
    if (!packet) {
        free(_services);
        return;
    }

    _pcb->probe_ip = probe_ip;
    _pcb->probe_services = _services;
    _pcb->probe_services_len = services_final_len;
    _pcb->probe_running = true;
    _mdns_schedule_tx_packet(packet, ((_pcb->failed_probes > 5)?1000:120) + (esp_random() & 0x7F));
    _pcb->state = PCB_PROBE_1;
}

static void _mdns_restart_pcb(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type) {
    //collect all services and start probe
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        srv_count++;
        a = a->next;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        services[i++] = a;
        a = a->next;
    }
    _mdns_init_pcb_probe(tcpip_if, pcb_type, services, srv_count, true);
}

static void _mdns_send_bye(mdns_srv_item_t ** services, size_t len, bool include_ip)
{
    uint8_t i, j;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<PCB_TYPE_MAX; j++) {
            if (_mdns_server->interfaces[i].pcbs[j].pcb && _mdns_server->interfaces[i].pcbs[j].state == PCB_RUNNING) {
                _mdns_pcb_send_bye((tcpip_adapter_if_t)i, (mdns_pcb_type_t)j, services, len, include_ip);
            }
        }
    }
}

static void _mdns_announce_pcb(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type, mdns_srv_item_t ** services, size_t len, bool include_ip) {
    mdns_pcb_t * _pcb = &_mdns_server->interfaces[tcpip_if].pcbs[pcb_type];
    size_t i;
    if (_pcb->pcb) {
        if (PCB_STATE_IS_PROBING(_pcb)) {
            _mdns_init_pcb_probe(tcpip_if, pcb_type, services, len, include_ip);
        } else if (PCB_STATE_IS_ANNOUNCING(_pcb)) {
            mdns_tx_packet_t *  p = _mdns_get_next_pcb_packet(tcpip_if, pcb_type);
            if (p) {
                for (i=0; i<len; i++) {
                    if (!_mdns_alloc_answer(&p->answers, MDNS_TYPE_SDPTR, services[i], false, false)
                            || !_mdns_alloc_answer(&p->answers, MDNS_TYPE_PTR, services[i], false, false)
                            || !_mdns_alloc_answer(&p->answers, MDNS_TYPE_SRV, services[i], true, false)
                            || !_mdns_alloc_answer(&p->answers, MDNS_TYPE_TXT, services[i], true, false)) {
                        break;
                    }
                }
                if (include_ip) {
                    _mdns_dealloc_answer(&p->additional, MDNS_TYPE_A, NULL);
                    _mdns_dealloc_answer(&p->additional, MDNS_TYPE_AAAA, NULL);
                    _mdns_alloc_answer(&p->answers, MDNS_TYPE_A, NULL, true, false);
                    _mdns_alloc_answer(&p->answers, MDNS_TYPE_AAAA, NULL, true, false);
                }
                _pcb->state = PCB_ANNOUNCE_1;
            }
        } else if (_pcb->state == PCB_RUNNING) {
            _pcb->state = PCB_ANNOUNCE_1;
            mdns_tx_packet_t *  p = _mdns_create_announce_packet(tcpip_if, pcb_type, services, len, include_ip);
            if (p) {
                _mdns_schedule_tx_packet(p, 0);
            }
        }
    }
}

static void _mdns_probe_all_pcbs(mdns_srv_item_t ** services, size_t len, bool probe_ip, bool clear_old_probe) {
    uint8_t i, j;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<PCB_TYPE_MAX; j++) {
            if (_mdns_server->interfaces[i].pcbs[j].pcb) {
                mdns_pcb_t * _pcb = &_mdns_server->interfaces[i].pcbs[j];
                if (clear_old_probe) {
                    free(_pcb->probe_services);
                    _pcb->probe_services = NULL;
                    _pcb->probe_services_len = 0;
                    _pcb->probe_running = false;
                }
                _mdns_init_pcb_probe((tcpip_adapter_if_t)i, (mdns_pcb_type_t)j, services, len, probe_ip);
            }
        }
    }
}

static void _mdns_announce_all_pcbs(mdns_srv_item_t ** services, size_t len, bool include_ip) {
    uint8_t i, j;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<PCB_TYPE_MAX; j++) {
            _mdns_announce_pcb((tcpip_adapter_if_t)i, (mdns_pcb_type_t)j, services, len, include_ip);
        }
    }
}

static void _mdns_send_final_bye(bool include_ip) {
    //collect all services and start probe
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            srv_count++;
        }
        a = a->next;
    }
    if (!srv_count) {
        return;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            services[i++] = a;
        }
        a = a->next;
    }
    _mdns_send_bye(services, srv_count, include_ip);
}

static void _mdns_send_bye_all_pcbs_no_instance() {
    //collect all services and start probe
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            srv_count++;
        }
        a = a->next;
    }
    if (!srv_count) {
        return;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            services[i++] = a;
        }
        a = a->next;
    }
    _mdns_send_bye(services, srv_count, false);
}

static void _mdns_restart_all_pcbs_no_instance() {
    //collect all services and start probe
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            srv_count++;
        }
        a = a->next;
    }
    if (!srv_count) {
        return;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            services[i++] = a;
        }
        a = a->next;
    }
    _mdns_probe_all_pcbs(services, srv_count, false, true);
}

static void _mdns_restart_all_pcbs() {
    _mdns_clear_tx_queue();
    //collect all services and start probe
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        srv_count++;
        a = a->next;
    }
    mdns_srv_item_t * services[srv_count];
    size_t l = 0;
    a = _mdns_server->services;
    while (a) {
        services[l++] = a;
        a = a->next;
    }

    _mdns_probe_all_pcbs(services, srv_count, true, true);
}

static bool _mdns_run_scheduler() {
    if (!_mdns_server || !_mdns_server->tx_queue) {
        return false;
    }
    mdns_tx_packet_t * p = _mdns_server->tx_queue;
    mdns_tx_packet_t * a = NULL;
    mdns_out_question_t * q = NULL;
    mdns_pcb_t * _pcb = &_mdns_server->interfaces[p->tcpip_if].pcbs[p->pcb_type];
    TickType_t t = xTaskGetTickCount() * portTICK_PERIOD_MS;
    if (p->send_at <= t) {
        _mdns_server->tx_queue = _mdns_server->tx_queue->next;
        if (_pcb->state == PCB_OFF) {
            _mdns_free_tx_packet(p);
            return false;
        }
        _mdns_dispatch_tx_packet(p);

        switch(_pcb->state) {
        case PCB_PROBE_1:
            q = p->questions;
            while (q) {
                q->unicast = false;
                q = q->next;
            }
        case PCB_PROBE_2:
            _mdns_schedule_tx_packet(p, 250);
            _pcb->state = (mdns_pcb_state_t)((uint8_t)(_pcb->state) + 1);
            break;
        case PCB_PROBE_3:
            a = _mdns_create_announce_from_probe(p);
            if (!a) {
                _mdns_schedule_tx_packet(p, 250);
                break;
            }
            _pcb->probe_running = false;
            _pcb->probe_ip = false;
            _pcb->probe_services_len = 0;
            _pcb->failed_probes = 0;
            free(_pcb->probe_services);
            _pcb->probe_services = NULL;
            _mdns_free_tx_packet(p);
            p = a;
            TickType_t send_after = 250;
        case PCB_ANNOUNCE_1:
            send_after = 1000;
        case PCB_ANNOUNCE_2:
            _mdns_schedule_tx_packet(p, send_after);
            _pcb->state = (mdns_pcb_state_t)((uint8_t)(_pcb->state) + 1);
            break;
        case PCB_ANNOUNCE_3:
            _pcb->state = PCB_RUNNING;
            _mdns_free_tx_packet(p);
            break;
        default:
            _mdns_free_tx_packet(p);
            break;
        }
        return true;
    }
    return false;
}





/**
 * @brief  creates/allocates new service
 * @param  service       service type
 * @param  proto         service proto
 * @param  port          service port
 *
 * @return pointer to the service or NULL on error
 */
static mdns_service_t * _mdns_create_service(const char * service, const char * proto, uint16_t port, const char * instance, uint8_t num_items, mdns_txt_item_t txt[])
{
    mdns_service_t * s = (mdns_service_t *)malloc(sizeof(mdns_service_t));
    if (!s) {
        return NULL;
    }

    mdns_txt_linked_item_t * new_txt = NULL;
    uint8_t i = 0;
    if (num_items) {
        for (i=0; i<num_items; i++) {
            mdns_txt_linked_item_t * new_item = (mdns_txt_linked_item_t *)malloc(sizeof(mdns_txt_linked_item_t));
            if (!new_item) {
                break;
            }
            new_item->key = strdup(txt[i].key);
            if (!new_item->key) {
                free(new_item);
                break;
            }
            new_item->value = strdup(txt[i].value);
            if (!new_item->value) {
                free((char *)new_item->key);
                free(new_item);
                break;
            }
            new_item->next = new_txt;
            new_txt = new_item;
        }
    }

    s->priority = 0;
    s->weight = 0;
    s->instance = instance?strndup(instance, MDNS_NAME_BUF_LEN - 1):NULL;
    s->txt = new_txt;
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
    while (service->txt) {
        mdns_txt_linked_item_t * s = service->txt;
        service->txt = service->txt->next;
        free((char *)s->key);
        free((char *)s->value);
        free(s);
    }
    free(service->txt);
    free(service);
}


/*
 * Received Packet Handling
 * */

static int _mdns_check_srv_collision(mdns_service_t * service, uint16_t priority, uint16_t weight, uint16_t port, const char * host, const char * domain)
{
    size_t our_host_len = strlen(_mdns_server->hostname);
    size_t our_len = 14 + our_host_len;

    size_t their_host_len = strlen(host);
    size_t their_domain_len = strlen(domain);
    size_t their_len = 9 + their_host_len + their_domain_len;

    if (their_len > our_len) {
        return 1;//they win
    } else if (their_len < our_len) {
        return -1;//we win
    }

    uint16_t our_index = 0;
    uint8_t our_data[our_len];
    _mdns_append_u16(our_data, &our_index, service->priority);
    _mdns_append_u16(our_data, &our_index, service->weight);
    _mdns_append_u16(our_data, &our_index, service->port);
    our_data[our_index++] = our_host_len;
    memcpy(our_data + our_index, _mdns_server->hostname, our_host_len);
    our_index += our_host_len;
    our_data[our_index++] = 5;
    memcpy(our_data + our_index, MDNS_DEFAULT_DOMAIN, 5);
    our_index += 5;
    our_data[our_index++] = 0;

    uint16_t their_index = 0;
    uint8_t their_data[their_len];
    _mdns_append_u16(their_data, &their_index, priority);
    _mdns_append_u16(their_data, &their_index, weight);
    _mdns_append_u16(their_data, &their_index, port);
    their_data[their_index++] = their_host_len;
    memcpy(their_data + their_index, host, their_host_len);
    their_index += their_host_len;
    their_data[their_index++] = their_domain_len;
    memcpy(their_data + their_index, domain, their_domain_len);
    their_index += their_domain_len;
    their_data[their_index++] = 0;

    int ret = memcmp(our_data, their_data, our_len);
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        return 1;//they win
    }
    return 0;//same
}

static int _mdns_check_txt_collision(mdns_service_t * service, const uint8_t * data, size_t len)
{
    size_t data_len = 1;
    if (len == 1 && service->txt) {
        return -1;//we win
    } else if (len > 1 && !service->txt) {
        return 1;//they win
    } else if (len == 1 && !service->txt) {
        return 0;//same
    }

    mdns_txt_linked_item_t * txt = service->txt;
    while (txt) {
        data_len += 2 + strlen(service->txt->key) + strlen(service->txt->value);
        txt = txt->next;
    }

    if (len > data_len) {
        return 1;//they win
    } else if (len < data_len) {
        return -1;//we win
    }

    uint8_t ours[len];
    uint16_t index = 0;
    char * tmp;

    txt = service->txt;
    while (txt) {
        tmp = (char *)malloc(2 + strlen(txt->key) + strlen(txt->value));
        if (tmp) {
            sprintf(tmp, "%s=%s", txt->key, txt->value);
            _mdns_append_string(ours, &index, tmp);
            free(tmp);
        }
        txt = txt->next;
    }

    int ret = memcmp(ours, data, len);
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        return 1;//they win
    }
    return 0;//same
}

static void _mdns_dup_interface(tcpip_adapter_if_t tcpip_if)
{
    uint8_t i;
    tcpip_adapter_if_t other_if = _mdns_get_other_if(tcpip_if);
    for (i=0; i<PCB_TYPE_MAX; i++) {
        if (_mdns_server->interfaces[other_if].pcbs[i].pcb) {
            //stop this interface and mark as dup
            if (_mdns_server->interfaces[tcpip_if].pcbs[i].pcb) {
                _mdns_clear_pcb_tx_queue(tcpip_if, i);
                _mdns_pcb_deinit(tcpip_if, i);
            }
            _mdns_server->interfaces[tcpip_if].pcbs[i].state = PCB_DUP;
            //ToDo: what should we do with other_if? Announce IP?
            _mdns_announce_pcb(other_if, i, NULL, 0, true);
        }
    }
}

static int _mdns_check_a_collision(ip4_addr_t * ip, tcpip_adapter_if_t tcpip_if)
{
    tcpip_adapter_ip_info_t if_ip_info;
    tcpip_adapter_ip_info_t other_ip_info;
    if (!ip->addr) {
        return 1;//denial! they win
    }
    if (tcpip_adapter_get_ip_info(tcpip_if, &if_ip_info)) {
        return 1;//they win
    }

    int ret = memcmp((uint8_t*)&if_ip_info.ip.addr, (uint8_t*)&ip->addr, sizeof(ip4_addr_t));
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        //is it the other interface?
        tcpip_adapter_if_t other_if = _mdns_get_other_if(tcpip_if);
        if (other_if == TCPIP_ADAPTER_IF_MAX) {
            return 1;//AP interface! They win
        }
        if (tcpip_adapter_get_ip_info(other_if, &other_ip_info)) {
            return 1;//IPv4 not active! They win
        }
        if (ip->addr != other_ip_info.ip.addr) {
            return 1;//IPv4 not ours! They win
        }
        _mdns_dup_interface(tcpip_if);
        return 2;//they win
    }
    return 0;//same
}

static int _mdns_check_aaaa_collision(ip6_addr_t * ip, tcpip_adapter_if_t tcpip_if)
{
    struct ip6_addr if_ip6;
    struct ip6_addr other_ip6;
    if (_ipv6_address_is_zero(*ip)) {
        return 1;//denial! they win
    }
    if (tcpip_adapter_get_ip6_linklocal(tcpip_if, &if_ip6)) {
        return 1;//they win
    }
    int ret = memcmp((uint8_t*)&if_ip6.addr, (uint8_t*)ip->addr, sizeof(ip6_addr_t));
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        //is it the other interface?
        tcpip_adapter_if_t other_if = _mdns_get_other_if(tcpip_if);
        if (other_if == TCPIP_ADAPTER_IF_MAX) {
            return 1;//AP interface! They win
        }
        if (tcpip_adapter_get_ip6_linklocal(other_if, &other_ip6)) {
            return 1;//IPv6 not active! They win
        }
        if (memcmp((uint8_t*)&other_ip6.addr, (uint8_t*)ip->addr, sizeof(ip6_addr_t))) {
            return 1;//IPv6 not ours! They win
        }
        _mdns_dup_interface(tcpip_if);
        return 2;//they win
    }
    return 0;//same
}

static bool _mdns_name_is_discovery(mdns_name_t * name, uint16_t type)
{
    return (
               (name->host && name->host[0] && !strcasecmp(name->host, "_services"))
               && (name->service && name->service[0] && !strcasecmp(name->service, "_dns-sd"))
               && (name->proto && name->proto[0] && !strcasecmp(name->proto, "_udp"))
               && (name->domain && name->domain[0] && !strcasecmp(name->domain, MDNS_DEFAULT_DOMAIN))
               && type == MDNS_TYPE_PTR
           );
}

static bool _mdns_name_is_ours(mdns_name_t * name)
{
    //domain have to be "local"
    if (!name->domain || !name->domain[0] || strcasecmp(name->domain, MDNS_DEFAULT_DOMAIN)) {
        return false;
    }

    //if service and proto are empty, host must match out hostname
    if ((!name->service || !name->service[0]) && (!name->proto || !name->proto[0])) {
        if (name->host && name->host[0] && strcasecmp(name->host, _mdns_server->hostname) == 0) {
            return true;
        }
        return false;
    }

    //if service or proto is empty, name is invalid
    if ((!name->service || !name->service[0]) || (!name->proto || !name->proto[0])) {
        return false;
    }

    //find the service
    mdns_srv_item_t * service = _mdns_get_service_item(name->service, name->proto);
    if (!service) {
        return false;
    }

    //if host is empty and we have service, we have success
    if (!name->host || !name->host[0]) {
        return true;
    }

    //OK we have host in the name. find what is the instance of the service
    const char * instance = service->service->instance;
    if (instance == NULL) {
        if (_mdns_server->instance && _mdns_server->instance[0]) {
            instance = _mdns_server->instance;
        } else if (_mdns_server->hostname && _mdns_server->hostname[0]) {
            instance = _mdns_server->hostname;
        } else {
            return false;
        }
    }

    //compare the instance against the name
    if (strcasecmp(name->host, instance) == 0) {
        return true;
    }

    return false;
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
 * @brief  read uint32_t from a packet
 * @param  packet       the packet
 * @param  index        index in the packet where the value starts
 *
 * @return the value
 */
static inline uint32_t _mdns_read_u32(const uint8_t * packet, uint16_t index)
{
    return (uint32_t)(packet[index]) << 24 | (uint32_t)(packet[index+1]) << 16 | (uint32_t)(packet[index+2]) << 8 | packet[index+3];
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
    if (!next_data) {
        return 0;
    }
    if (!name->parts) {
        //root
        return next_data;
    }
    if (name->parts == 3) {
        memmove((uint8_t*)name + (MDNS_NAME_BUF_LEN), (uint8_t*)name, 3*(MDNS_NAME_BUF_LEN));
        name->host[0] = 0;
    } else if (name->parts == 2) {
        memmove((uint8_t*)(name->domain), (uint8_t*)(name->service), (MDNS_NAME_BUF_LEN));
        name->service[0] = 0;
        name->proto[0] = 0;
    }
    if (strcasecmp(name->domain, MDNS_DEFAULT_DOMAIN) == 0 || strcasecmp(name->domain, "arpa") == 0) {
        return next_data;
    }
    return 0;
}

static bool _mdns_question_matches(mdns_parsed_question_t * question, uint16_t type, mdns_srv_item_t * service) {
    if (question->type != type) {
        return false;
    }
    if (type == MDNS_TYPE_A || type == MDNS_TYPE_AAAA) {
        return true;
    } else if (type == MDNS_TYPE_PTR || type == MDNS_TYPE_SDPTR) {
        if (!strcasecmp(service->service->service, question->service)
            && !strcasecmp(service->service->proto, question->proto)
            && !strcasecmp(MDNS_DEFAULT_DOMAIN, question->domain)) {
            return true;
        }
    } else if (type == MDNS_TYPE_SRV || type == MDNS_TYPE_TXT) {
        const char * name = service->service->instance;
        if (!name) {
            name = _mdns_server->instance;
        }
        if (!name) {
            name = _mdns_server->hostname;
        }
        if (!strcasecmp(name, question->host)
            && !strcasecmp(service->service->service, question->service)
            && !strcasecmp(service->service->proto, question->proto)
            && !strcasecmp(MDNS_DEFAULT_DOMAIN, question->domain)) {
            return true;
        }
    }

    return false;
}

static void _mdns_remove_parsed_question(mdns_parsed_packet_t * parsed_packet, uint16_t type, mdns_srv_item_t * service) {
    mdns_parsed_question_t * q = parsed_packet->questions;

    if (_mdns_question_matches(q, type, service)) {
        parsed_packet->questions = q->next;
        free(q->host);
        free(q->service);
        free(q->proto);
        free(q->domain);
        free(q);
        return;
    }

    while (q->next) {
        mdns_parsed_question_t * p = q->next;
        if (_mdns_question_matches(p, type, service)) {
            q->next = p->next;
            free(p->host);
            free(p->service);
            free(p->proto);
            free(p->domain);
            free(p);
            return;
        }
        q = q->next;
    }
}

/**
 * @brief  main packet parser
 *
 * @param  server       the server
 * @param  data         byte array holding the packet data
 * @param  len          length of the byte array
 */
void mdns_parse_packet(mdns_rx_packet_t * packet)
{
    static mdns_name_t n;
    mdns_header_t header;
    const uint8_t * data = (const uint8_t*)packet->pb->payload;
    size_t len = packet->pb->len;
    const uint8_t * content = data + MDNS_HEAD_LEN;
    bool do_not_reply = false;

    _mdns_dbg_printf("\nRX[%u][%u]: ", packet->tcpip_if, (uint32_t)packet->pcb_type);
    if (packet->src.type == IPADDR_TYPE_V4) {
        _mdns_dbg_printf("From: " IPSTR ":%u, To: " IPSTR ", ", IP2STR(&packet->src.u_addr.ip4), packet->src_port, IP2STR(&packet->dest.u_addr.ip4));
    } else {
        _mdns_dbg_printf("From: " IPV6STR ":%u, To: " IPV6STR ", ", IPV62STR(packet->src.u_addr.ip6), packet->src_port, IPV62STR(packet->dest.u_addr.ip6));
    }
    //mdns_debug_packet(data, len);

    mdns_parsed_packet_t * parsed_packet = (mdns_parsed_packet_t *)malloc(sizeof(mdns_parsed_packet_t));
    if (!parsed_packet) {
        return;
    }
    memset(parsed_packet, 0, sizeof(mdns_parsed_packet_t));

    mdns_name_t * name = &n;
    memset(name, 0, sizeof(mdns_name_t));

    header.id = _mdns_read_u16(data, MDNS_HEAD_ID_OFFSET);
    header.flags.value = _mdns_read_u16(data, MDNS_HEAD_FLAGS_OFFSET);
    header.questions = _mdns_read_u16(data, MDNS_HEAD_QUESTIONS_OFFSET);
    header.answers = _mdns_read_u16(data, MDNS_HEAD_ANSWERS_OFFSET);
    header.servers = _mdns_read_u16(data, MDNS_HEAD_SERVERS_OFFSET);
    header.additional = _mdns_read_u16(data, MDNS_HEAD_ADDITIONAL_OFFSET);

    _mdns_printf("%s", (header.flags.value == MDNS_FLAGS_AUTHORITATIVE)?"AUTHORITATIVE\n":(header.flags.value == MDNS_FLAGS_DISTRIBUTED)?"DISTRIBUTED\n":((header.flags.value == 0)?"\n":" "));
    if (header.flags.value && header.flags.value != MDNS_FLAGS_AUTHORITATIVE) {
        _mdns_printf("0x%04X\n", header.flags.value);
    }

    if (header.flags.value == MDNS_FLAGS_AUTHORITATIVE && packet->src_port != MDNS_SERVICE_PORT) {
        _mdns_printf("ERROR: Authoritative packet came from wrong port! %u != %u\n", packet->src_port, MDNS_SERVICE_PORT);
        free(parsed_packet);
        return;
    }

    parsed_packet->tcpip_if = packet->tcpip_if;
    parsed_packet->pcb_type = packet->pcb_type;
    parsed_packet->multicast = packet->multicast;
    parsed_packet->authoritative = header.flags.value == MDNS_FLAGS_AUTHORITATIVE;
    parsed_packet->distributed = header.flags.value == MDNS_FLAGS_DISTRIBUTED;
    ip_addr_copy(parsed_packet->src, packet->src);
    parsed_packet->src_port = packet->src_port;

    if (header.questions) {
        uint8_t qs = header.questions;

        while (qs--) {
            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                header.answers = 0;
                header.additional = 0;
                header.servers = 0;
                _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                goto clear_rx_packet;//error
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            bool unicast = !!(clas & 0x8000);
            clas &= 0x7FFF;
            content = content + 4;

            if (clas != 0x0001) {//bad class
                continue;
            }

            if (_mdns_name_is_discovery(name, type)) {
                //service discovery
                _mdns_printf("    DISCOVERY\n");
                parsed_packet->discovery = true;
                mdns_srv_item_t * a = _mdns_server->services;
                while (a) {
                    mdns_parsed_question_t * question = (mdns_parsed_question_t *)malloc(sizeof(mdns_parsed_question_t));
                    if (!question) {
                        _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                        goto clear_rx_packet;
                    }
                    question->unicast = unicast;
                    question->type = MDNS_TYPE_SDPTR;
                    //ToDo: check if strdup failed
                    question->host = NULL;
                    question->service = strdup(a->service->service);
                    question->proto = strdup(a->service->proto);
                    question->domain = strdup(MDNS_DEFAULT_DOMAIN);
                    question->next = parsed_packet->questions;
                    parsed_packet->questions = question;

                    a = a->next;
                }

                continue;
            } else if (name->sub || !_mdns_name_is_ours(name)) {
                continue;
            }


            _mdns_printf("    Q: ");
            if (unicast) {
                _mdns_printf("*U* ");
            }
            if (type == MDNS_TYPE_PTR) {
                _mdns_printf("%s.%s%s.%s.%s. PTR ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                _mdns_printf("%s.%s%s.%s.%s. SRV ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                _mdns_printf("%s.%s%s.%s.%s. TXT ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_A) {
                _mdns_printf("%s.%s. A ", name->host, name->domain);
            } else if (type == MDNS_TYPE_AAAA) {
                _mdns_printf("%s.%s. AAAA ", name->host, name->domain);
            } else if (type == MDNS_TYPE_NSEC) {
                _mdns_printf("%s.%s%s.%s.%s. NSEC ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_ANY) {
                _mdns_printf("%s.%s%s.%s.%s. ANY ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else {
                _mdns_printf("%s.%s%s.%s.%s. %04X ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain, type);
            }

            _mdns_printf("IN");

            if (type == MDNS_TYPE_ANY) {
                parsed_packet->probe = true;
                _mdns_printf(" PROBE\n");
            } else {
                _mdns_printf(" OURS\n");
            }
            //save question

            mdns_parsed_question_t * question = (mdns_parsed_question_t *)malloc(sizeof(mdns_parsed_question_t));
            if (!question) {
                _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                goto clear_rx_packet;
            }
            question->unicast = unicast;
            question->type = type;
            //ToDo: check if strdup failed
            question->host = (name->host && name->host[0])?strdup(name->host):NULL;
            question->service = (name->service && name->service[0])?strdup(name->service):NULL;
            question->proto = (name->proto && name->proto[0])?strdup(name->proto):NULL;
            question->domain = (name->domain && name->domain[0])?strdup(name->domain):NULL;
            question->next = parsed_packet->questions;
            parsed_packet->questions = question;
        }
    }

    if (header.questions && !parsed_packet->questions && !parsed_packet->discovery) {
        //no questions have been saved, we would not care for the rest
        goto clear_rx_packet;

    } else if (header.answers || header.servers || header.additional) {
        uint16_t recordIndex = 0;

        while (content < (data + len)) {

            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                goto clear_rx_packet;//error
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            uint32_t ttl = _mdns_read_u32(content, MDNS_TTL_OFFSET);
            uint16_t data_len = _mdns_read_u16(content, MDNS_LEN_OFFSET);
            const uint8_t * data_ptr = content + MDNS_DATA_OFFSET;
            bool flush = !!(clas & 0x8000);
            clas &= 0x7FFF;

            content = data_ptr + data_len;
            if (content > (data + len)) {
                _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                goto clear_rx_packet;
            }

            bool discovery = false;
            bool ours = false;
            bool search_result = false;
            mdns_srv_item_t * service = NULL;
            mdns_parsed_recort_type_t record_type = MDNS_ANSWER;

            if (recordIndex >= (header.answers + header.servers)) {
                record_type = MDNS_EXTRA;
            } else if (recordIndex >= (header.answers)) {
                record_type = MDNS_NS;
            }
            recordIndex++;

            if (type == MDNS_TYPE_NSEC || type == MDNS_TYPE_OPT) {
                //skip NSEC and OPT
                continue;
            }

            if (parsed_packet->discovery && _mdns_name_is_discovery(name, type)) {
                discovery = true;
            } else if (!name->sub && _mdns_name_is_ours(name)) {
                ours = true;
                if (name->service && name->service[0] && name->proto && name->proto[0]) {
                    service = _mdns_get_service_item(name->service, name->proto);
                }
            } else {
                //ToDo: validate search if no questions and running and matching and packet authoritative and record type not server
                if (!_mdns_server->search.running || header.questions || !parsed_packet->authoritative || record_type == MDNS_NS) {
                    //skip this record
                    continue;
                }
                //ToDo: check if record has been searched for
                //for A and AAAA records we need to know that the host belongs to result
                search_result = true;
            }

            if (record_type == MDNS_EXTRA) {
                _mdns_printf("    X");
            } else if (record_type == MDNS_NS) {
                _mdns_printf("    S");
            } else {
                _mdns_printf("    A");
            }

            if (discovery) {
                //_mdns_printf(" DISCOVERY");
            } else if (ours) {
                //_mdns_printf(" OURS");
            } else if (search_result) {
                //_mdns_printf(" SEARCH");
            } else {
                _mdns_printf(" DISCARD");
            }

            if (type == MDNS_TYPE_PTR) {
                _mdns_printf(": %s.%s.%s.%s. PTR ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                _mdns_printf(": %s.%s.%s.%s. SRV ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                _mdns_printf(": %s.%s.%s.%s. TXT ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_A) {
                _mdns_printf(": %s.%s. A ", name->host, name->domain);
            } else if (type == MDNS_TYPE_AAAA) {
                _mdns_printf(": %s.%s. AAAA ", name->host, name->domain);
            } else if (type == MDNS_TYPE_NSEC) {
                _mdns_printf(": %s.%s.%s.%s. NSEC ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_ANY) {
                _mdns_printf(": %s.%s.%s.%s. ANY ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_OPT) {
                _mdns_printf(": . OPT ");
            } else {
                _mdns_printf(": %s.%s.%s.%s. %04X ", name->host, name->service, name->proto, name->domain, type);
            }

            if (clas == 0x0001) {
                _mdns_printf("IN ");
            } else {
                _mdns_printf("%04X ", clas);
            }
            if (flush) {
                _mdns_printf("FLUSH ");
            }
            _mdns_printf("%u ", ttl);

            _mdns_printf("[%u] ", data_len);

            if (type == MDNS_TYPE_PTR) {
                if (!_mdns_parse_fqdn(data, data_ptr, name)) {
                    _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                    continue;//error
                }
                _mdns_printf("%s.%s.%s.%s.", name->host, name->service, name->proto, name->domain);

                if (search_result) {
                    _mdns_printf(" SAVE\n");
                } else if ((discovery || ours) && !name->sub && _mdns_name_is_ours(name)) {
                    if (discovery) {
                        _mdns_printf(" KNOWN\n");
                        service = _mdns_get_service_item(name->service, name->proto);
                        _mdns_remove_parsed_question(parsed_packet, MDNS_TYPE_SDPTR, service);
                    } else if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_printf(" RMV_Q\n");
                        _mdns_remove_parsed_question(parsed_packet, type, service);
                    } else {
                        if (!ttl) {
                            _mdns_printf(" GOOD_BYE\n");
                        } else if (ttl > 2250) {
                            _mdns_printf(" RMV_A\n");
                            _mdns_remove_scheduled_answer(packet->tcpip_if, packet->pcb_type, type, service);
                        } else {
                            _mdns_printf("\n");
                        }
                    }
                } else {
                    _mdns_printf("\n");
                }

            } else if (type == MDNS_TYPE_SRV) {//priority, weight, port, hostname
                if (!_mdns_parse_fqdn(data, data_ptr + MDNS_SRV_FQDN_OFFSET, name)) {
                    _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                    continue;//error
                }
                uint16_t priority = _mdns_read_u16(data_ptr, MDNS_SRV_PRIORITY_OFFSET);
                uint16_t weight = _mdns_read_u16(data_ptr, MDNS_SRV_WEIGHT_OFFSET);
                uint16_t port = _mdns_read_u16(data_ptr, MDNS_SRV_PORT_OFFSET);
                _mdns_printf("%u %u %u %s.%s.", priority, weight, port, name->host, name->domain);

                if (search_result) {
                    _mdns_printf(" SAVE\n");
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_printf(" RMV_Q\n");
                        _mdns_remove_parsed_question(parsed_packet, type, service);
                        continue;
                    } else if (parsed_packet->distributed) {
                        _mdns_printf(" RMV_A\n");
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->pcb_type, type, service);
                        continue;
                    }
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_srv_collision(service->service, priority, weight, port, name->host, name->domain);
                    }
                    if (col && (parsed_packet->probe || parsed_packet->authoritative)) {
                        if (col > 0 || !port) {
                            do_not_reply = true;
                            if (_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].probe_running) {
                                _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].failed_probes++;
                                if (service->service->instance) {
                                    _mdns_printf(" CHANGE LOCAL INSTANCE\n");
                                    char * new_instance = _mdns_mangle_name((char *)service->service->instance);
                                    if (new_instance) {
                                        free((char *)service->service->instance);
                                        service->service->instance = new_instance;
                                    }
                                    _mdns_probe_all_pcbs(&service, 1, false, false);
                                } else if (_mdns_server->instance) {
                                    _mdns_printf(" CHANGE GLOBAL INSTANCE\n");
                                    char * new_instance = _mdns_mangle_name((char *)_mdns_server->instance);
                                    if (new_instance) {
                                        free((char *)_mdns_server->instance);
                                        _mdns_server->instance = new_instance;
                                    }
                                    _mdns_restart_all_pcbs_no_instance();
                                } else {
                                    _mdns_printf(" CHANGE HOSTNAME\n");
                                    char * new_host = _mdns_mangle_name((char *)_mdns_server->hostname);
                                    if (new_host) {
                                        free((char *)_mdns_server->hostname);
                                        _mdns_server->hostname = new_host;
                                    }
                                    _mdns_restart_all_pcbs();
                                }
                            } else {
                                _mdns_printf(" PROBE INSTANCE\n");
                                _mdns_pcb_send_bye(packet->tcpip_if, packet->pcb_type, &service, 1, false);
                                _mdns_init_pcb_probe(packet->tcpip_if, packet->pcb_type, &service, 1, false);
                            }
                        } else {
                            _mdns_printf(" WON\n");
                        }
                    } else if (ttl > 60 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions) {
                        _mdns_printf(" RMV_A\n");
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->pcb_type, type, service);
                    } else {
                        _mdns_printf("\n");
                    }
                } else {
                    _mdns_printf("\n");
                }

            } else if (type == MDNS_TYPE_TXT) {//txt
                //ToDo: figure out a way to save the data
                uint16_t i=0, y;
                while (i < data_len) {
                    uint8_t partLen = data_ptr[i++];
                    if ((i+partLen) > data_len) {
                        _mdns_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                        goto clear_rx_packet;//error
                    }
                    char txt[partLen+1];//txt item maybe split?
                    for (y=0; y<partLen; y++) {
                        char d = data_ptr[i++];
                        txt[y] = d;
                    }
                    txt[partLen] = 0;
                    _mdns_printf("%s", txt);
                    if (i<data_len) {
                        _mdns_printf("; ");
                    }
                }

                if (search_result) {
                    _mdns_printf(" SAVE\n");
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_printf(" RMV_Q\n");
                        _mdns_remove_parsed_question(parsed_packet, type, service);
                        continue;
                    }
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_txt_collision(service->service, data_ptr, data_len);
                    }
                    if (col && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].probe_running) {
                        do_not_reply = true;
                        _mdns_printf(" PROBE SERVICE\n");
                        _mdns_init_pcb_probe(packet->tcpip_if, packet->pcb_type, &service, 1, true);
                    } else if (ttl > 2250 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].probe_running) {
                        _mdns_printf(" RMV_A\n");
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->pcb_type, type, service);
                    } else {
                        _mdns_printf("\n");
                    }
                } else {
                    _mdns_printf("\n");
                }

            } else if (type == MDNS_TYPE_AAAA) {//ipv6
                ip6_addr_t ip6;
                memcpy(&ip6, data_ptr, sizeof(ip6_addr_t));
                _mdns_printf(IPV6STR, IPV62STR(ip6));
                if (search_result) {
                    _mdns_printf(" SAVE\n");
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_printf(" RMV_Q\n");
                        _mdns_remove_parsed_question(parsed_packet, type, NULL);
                        continue;
                    }
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_aaaa_collision(&ip6, packet->tcpip_if);
                    }
                    if (col == 2) {
                        _mdns_printf(" DUP\n");
                        goto clear_rx_packet;
                    } else if (col == 1) {
                        do_not_reply = true;
                        if (_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].probe_running) {
                            if (col && (parsed_packet->probe || parsed_packet->authoritative)) {
                                _mdns_printf(" CHANGE HOSTNAME\n");
                                //ToDo: check if comes from us
                                _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].failed_probes++;
                                char * new_host = _mdns_mangle_name((char *)_mdns_server->hostname);
                                if (new_host) {
                                    free((char *)_mdns_server->hostname);
                                    _mdns_server->hostname = new_host;
                                }
                                _mdns_restart_all_pcbs();
                            } else {
                                _mdns_printf("\n");
                            }
                        } else {
                            _mdns_printf(" PROBE HOSTNAME\n");
                            _mdns_init_pcb_probe(packet->tcpip_if, packet->pcb_type, NULL, 0, true);
                        }
                    } else if (ttl > 60 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].probe_running) {
                        _mdns_printf(" RMV_A\n");
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->pcb_type, type, NULL);
                    } else {
                        _mdns_printf("\n");
                    }
                } else {
                    _mdns_printf("\n");
                }

            } else if (type == MDNS_TYPE_A) {//ipv4
                ip4_addr_t ip;
                memcpy(&ip, data_ptr, sizeof(ip4_addr_t));
                _mdns_printf(IPSTR, IP2STR(&ip));
                if (search_result) {
                    _mdns_printf(" SAVE\n");
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_printf(" RMV_Q\n");
                        _mdns_remove_parsed_question(parsed_packet, type, NULL);
                        continue;
                    }
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_a_collision(&ip, packet->tcpip_if);
                    }
                    if (col == 2) {
                        _mdns_printf(" DUP\n");
                        goto clear_rx_packet;
                    } else if (col == 1) {
                        do_not_reply = true;
                        if (_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].probe_running) {
                            if (col && (parsed_packet->probe || parsed_packet->authoritative)) {
                                _mdns_printf(" CHANGE HOSTNAME\n");
                                //ToDo: check if comes from us
                                _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].failed_probes++;
                                char * new_host = _mdns_mangle_name((char *)_mdns_server->hostname);
                                if (new_host) {
                                    free((char *)_mdns_server->hostname);
                                    _mdns_server->hostname = new_host;
                                }
                                _mdns_restart_all_pcbs();
                            } else {
                                _mdns_printf("\n");
                            }
                        } else {
                            _mdns_printf(" PROBE HOSTNAME\n");
                            _mdns_init_pcb_probe(packet->tcpip_if, packet->pcb_type, NULL, 0, true);
                        }
                    } else if (ttl > 60 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].probe_running) {
                        _mdns_printf(" RMV_A\n");
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->pcb_type, type, NULL);
                    } else {
                        _mdns_printf("\n");
                    }
                } else {
                    _mdns_printf("\n");
                }

            }
        }
        //end while
    }

    if (!do_not_reply && _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->pcb_type].state > PCB_PROBE_3 && (parsed_packet->questions || parsed_packet->discovery)) {
        _mdns_create_answer_from_parsed_packet(parsed_packet);
    }


clear_rx_packet:
    while (parsed_packet->questions) {
        mdns_parsed_question_t * question = parsed_packet->questions;
        parsed_packet->questions = parsed_packet->questions->next;
        free(question->host);
        free(question->service);
        free(question->proto);
        free(question->domain);
        free(question);
    }
    free(parsed_packet);
}

void _mdns_enable_pcb(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type)
{
    if (!_mdns_server->interfaces[tcpip_if].pcbs[pcb_type].pcb) {
        if (_mdns_pcb_init(tcpip_if, pcb_type)) {
            return;
        }
    }
    _mdns_restart_pcb(tcpip_if, pcb_type);
}

void _mdns_disable_pcb(tcpip_adapter_if_t tcpip_if, mdns_pcb_type_t pcb_type)
{
    if (_mdns_server->interfaces[tcpip_if].pcbs[pcb_type].pcb) {
        _mdns_clear_pcb_tx_queue(tcpip_if, pcb_type);
        _mdns_pcb_deinit(tcpip_if, pcb_type);
        //get other_if and check if duplicate
        tcpip_adapter_if_t other_if = _mdns_get_other_if(tcpip_if);
        if (other_if != TCPIP_ADAPTER_IF_MAX && _mdns_server->interfaces[other_if].pcbs[pcb_type].state == PCB_DUP) {
            _mdns_server->interfaces[other_if].pcbs[pcb_type].state = PCB_OFF;
            _mdns_enable_pcb(other_if, pcb_type);
        }
    }
    _mdns_server->interfaces[tcpip_if].pcbs[pcb_type].state = PCB_OFF;
}

#include "esp_log.h"
void _mdns_handle_system_event(system_event_id_t event)
{
    if (!_mdns_server) {
        return;
    }
    tcpip_adapter_dhcp_status_t dcst;
    switch(event) {
    case SYSTEM_EVENT_STA_CONNECTED:
        if (!tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA, &dcst)) {
            if (dcst != TCPIP_ADAPTER_DHCP_STARTED) {
                _mdns_enable_pcb(TCPIP_ADAPTER_IF_STA, PCB_TYPE_V4);
            }
        }
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        //we should detect if this is an IP change
        ESP_LOGW("mdns", "STA_GOT_IP START");
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_STA, PCB_TYPE_V4);
        _mdns_announce_pcb(TCPIP_ADAPTER_IF_STA, PCB_TYPE_V6, NULL, 0, true);
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_STA, PCB_TYPE_V6);
            ESP_LOGW("mdns", "STA_GOT_IP END");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_STA, PCB_TYPE_V4);
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_STA, PCB_TYPE_V6);
        break;
    case SYSTEM_EVENT_AP_START:
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_AP, PCB_TYPE_V4);
        break;
    case SYSTEM_EVENT_AP_STOP:
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_AP, PCB_TYPE_V4);
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_AP, PCB_TYPE_V6);
            ESP_LOGW("mdns", "AP STOP");
        break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_AP, PCB_TYPE_V6);
        //announce IP on v4, but we need to figure out which one changed
        //_mdns_announce_pcb(TCPIP_ADAPTER_IF_STA, PCB_TYPE_V4, NULL, 0, true);
        //_mdns_announce_pcb(TCPIP_ADAPTER_IF_AP, PCB_TYPE_V4, NULL, 0, true);
            ESP_LOGW("mdns", "STA_GOT_IPV6");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        if (!tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_ETH, &dcst)) {
            if (dcst != TCPIP_ADAPTER_DHCP_STARTED) {
                _mdns_enable_pcb(TCPIP_ADAPTER_IF_ETH, PCB_TYPE_V4);
            }
        }
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_ETH, PCB_TYPE_V4);
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_ETH, PCB_TYPE_V4);
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_ETH, PCB_TYPE_V6);
        break;
    default:
        break;
    }
}

/*
 * MDNS Service
 * */

static void _mdns_free_action(mdns_action_t * action)
{
    switch(action->type) {
    case ACTION_HOSTNAME_SET:
        free(action->data.hostname);
        break;
    case ACTION_INSTANCE_SET:
        free(action->data.instance);
        break;
    case ACTION_SERVICE_ADD:
        _mdns_free_service(action->data.srv_add.service->service);
        free(action->data.srv_add.service);
        break;
    case ACTION_SERVICE_INSTANCE_SET:
        free(action->data.srv_instance.instance);
        break;
    case ACTION_SERVICE_TXT_SET:
        free(action->data.srv_txt_set.key);
        free(action->data.srv_txt_set.value);
        break;
    case ACTION_SERVICE_TXT_DEL:
        free(action->data.srv_txt_del.key);
        break;
    default:
        break;
    }
    free(action);
}

static void _mdns_execute_action(mdns_action_t * action)
{
    mdns_srv_item_t * a = NULL;
    mdns_service_t * service;
    char * key;
    char * value;
    mdns_txt_linked_item_t * txt, * t;

    switch(action->type) {
    case ACTION_SYSTEM_EVENT:
        _mdns_handle_system_event(action->data.event);
        break;
    case ACTION_HOSTNAME_SET:
        _mdns_send_final_bye(true);
        free((char*)_mdns_server->hostname);
        _mdns_server->hostname = action->data.hostname;
        _mdns_restart_all_pcbs();

        break;
    case ACTION_INSTANCE_SET:
        _mdns_send_bye_all_pcbs_no_instance();
        free((char*)_mdns_server->instance);
        _mdns_server->instance = action->data.instance;
        _mdns_restart_all_pcbs_no_instance();

        break;
    case ACTION_SERVICE_ADD:
        action->data.srv_add.service->next = _mdns_server->services;
        _mdns_server->services = action->data.srv_add.service;
        _mdns_probe_all_pcbs(&action->data.srv_add.service, 1, false, false);

        break;
    case ACTION_SERVICE_INSTANCE_SET:
        if (action->data.srv_port.service->service->instance) {
            _mdns_send_bye(&action->data.srv_instance.service, 1, false);
            free((char*)action->data.srv_instance.service->service->instance);
        }
        action->data.srv_instance.service->service->instance = action->data.srv_instance.instance;
        _mdns_probe_all_pcbs(&action->data.srv_instance.service, 1, false, false);

        break;
    case ACTION_SERVICE_PORT_SET:
        action->data.srv_port.service->service->port = action->data.srv_port.port;
        _mdns_announce_all_pcbs(&action->data.srv_port.service, 1, true);

        break;
    case ACTION_SERVICE_TXT_SET:
        service = action->data.srv_txt_set.service->service;
        key = action->data.srv_txt_set.key;
        value = action->data.srv_txt_set.value;
        txt = service->txt;
        while (txt) {
            if (strcmp(txt->key, key) == 0) {
                free((char *)txt->value);
                free(key);
                txt->value = value;
                break;
            }
            txt = txt->next;
        }
        if (!txt) {
            txt = (mdns_txt_linked_item_t *)malloc(sizeof(mdns_txt_linked_item_t));
            if (!txt) {
                _mdns_free_action(action);
                return;
            }
            txt->key = key;
            txt->value = value;
            txt->next = service->txt;
            service->txt = txt;
        }

        _mdns_announce_all_pcbs(&action->data.srv_txt_set.service, 1, false);

        break;
    case ACTION_SERVICE_TXT_DEL:
        service = action->data.srv_txt_del.service->service;
        key = action->data.srv_txt_del.key;
        txt = service->txt;
        if (!txt) {
            break;
        }
        if (strcmp(txt->key, key) == 0) {
            service->txt = txt->next;
            free((char *)txt->key);
            free((char *)txt->value);
            free(txt);
        } else {
            while (txt->next) {
                if (strcmp(txt->next->key, key) == 0) {
                    t = txt->next;
                    txt->next = t->next;
                    free((char *)t->key);
                    free((char *)t->value);
                    free(t);
                    break;
                } else {
                    txt = txt->next;
                }
            }
        }
        free(key);

        _mdns_announce_all_pcbs(&action->data.srv_txt_set.service, 1, false);

        break;
    case ACTION_SERVICE_DEL:
        a = _mdns_server->services;
        if (_mdns_server->services == action->data.srv_del.service) {
            _mdns_server->services = a->next;
            _mdns_send_bye(&a, 1, false);
            _mdns_free_service(a->service);
            free(a);
        } else {
            while (a->next && a->next != action->data.srv_del.service) {
                a = a->next;
            }
            if (a->next == action->data.srv_del.service) {
                mdns_srv_item_t * b = a;
                a->next = a->next->next;
                _mdns_send_bye(&b, 1, false);
                _mdns_free_service(b->service);
                free(b);
            }
        }

        break;
    case ACTION_SERVICES_CLEAR:
        _mdns_send_final_bye(false);
        a = _mdns_server->services;
        _mdns_server->services = NULL;
        while (a) {
            mdns_srv_item_t * s = a;
            a = a->next;
            _mdns_free_service(s->service);
            free(s);
        }

        break;
    default:
        break;
    }
    free(action);
}

#ifndef MDNS_TEST_MODE

/**
 * @brief  the main MDNS service task. Packets are received and parsed here
 */
static void _mdns_service_task(void *pvParameters)
{
    mdns_rx_packet_t * packet = NULL;
    for (;;) {
        if (_mdns_server) {
            if (_mdns_server->action_queue) {
                mdns_action_t * a;
                while (xQueueReceive(_mdns_server->action_queue, &a, 0) == pdTRUE) {
                    MDNS_MUTEX_LOCK();
                    _mdns_execute_action(a);
                    MDNS_MUTEX_UNLOCK();
                }
            }
            if (_mdns_server->rx_queue && xQueueReceive(_mdns_server->rx_queue, &packet, 0) == pdTRUE) {
                mdns_parse_packet(packet);
                pbuf_free(packet->pb);
                free(packet);
            } else if (!_mdns_run_scheduler()) {
                vTaskDelay(1);
            }
        } else {
            vTaskDelay(1);
        }
    }
}
#endif

/**
 * @brief  Start the service thread if not running
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t _mdns_service_task_start()
{
#ifndef MDNS_TEST_MODE
    if (!_mdns_service_semaphore) {
        _mdns_service_semaphore = xSemaphoreCreateMutex();
        if (!_mdns_service_semaphore) {
            return ESP_FAIL;
        }
    }
    MDNS_SERVICE_LOCK();
    if (!_mdns_service_task_handle) {
        xTaskCreatePinnedToCore(_mdns_service_task, "mdns", MDNS_SERVICE_STACK_DEPTH, NULL, MDNS_SERVICE_TASK_PRIORITY, &_mdns_service_task_handle, 0);
        if (!_mdns_service_task_handle) {
            MDNS_SERVICE_UNLOCK();
            return ESP_FAIL;
        }
    }
    MDNS_SERVICE_UNLOCK();
#endif

    return ESP_OK;
}

/**
 * @brief  Stop the service thread
 *
 * @return
 *      - ESP_OK
 */
static esp_err_t _mdns_service_task_stop()
{
#ifndef MDNS_TEST_MODE
    MDNS_SERVICE_LOCK();
    if (_mdns_service_task_handle) {
        vTaskDelete(_mdns_service_task_handle);
        _mdns_service_task_handle = NULL;
    }
    MDNS_SERVICE_UNLOCK();
#endif
    _mdns_server = NULL;
    return ESP_OK;
}

/*
 * Public Methods
 * */

esp_err_t mdns_init()
{
    esp_err_t err = ESP_OK;

    if (_mdns_server) {
        return err;
    }

    _mdns_server = (mdns_server_t *)malloc(sizeof(mdns_server_t));
    if (!_mdns_server) {
        return ESP_ERR_NO_MEM;
    }
    memset((uint8_t*)_mdns_server, 0, sizeof(mdns_server_t));

    _mdns_server->lock = xSemaphoreCreateMutex();
    if (!_mdns_server->lock) {
        err = ESP_ERR_NO_MEM;
        goto free_server;
    }

    _mdns_server->search.lock = xSemaphoreCreateMutex();
    if (!_mdns_server->search.lock) {
        err = ESP_ERR_NO_MEM;
        goto free_lock;
    }

    _mdns_server->action_queue = xQueueCreate(MDNS_ACTION_QUEUE_LEN, sizeof(mdns_action_t *));
    if (!_mdns_server->action_queue) {
        err = ESP_ERR_NO_MEM;
        goto free_search_lock;
    }

    _mdns_server->rx_queue = xQueueCreate(MDNS_PACKET_QUEUE_LEN, sizeof(mdns_rx_packet_t *));
    if (!_mdns_server->rx_queue) {
        err = ESP_ERR_NO_MEM;
        goto free_action_queue;
    }

    if (_mdns_service_task_start()) {
        //service start failed!
        err = ESP_FAIL;
        goto free_all;
    }

    //ToDo: there must be a better way to get the system hostname
    mdns_set_hostname("ESP32");

    return ESP_OK;

free_all:
    vQueueDelete(_mdns_server->rx_queue);
free_action_queue:
    vQueueDelete(_mdns_server->action_queue);
free_search_lock:
    vSemaphoreDelete(_mdns_server->search.lock);
free_lock:
    vSemaphoreDelete(_mdns_server->lock);
free_server:
    free(_mdns_server);
    _mdns_server = NULL;
    return err;
}

void mdns_free()
{
    if (!_mdns_server) {
        return;
    }
    //stop UDP
    uint8_t i, j;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<PCB_TYPE_MAX; j++) {
            _mdns_pcb_deinit(i, j);
        }
    }
    _mdns_service_task_stop();
    mdns_service_remove_all(_mdns_server);
    MDNS_MUTEX_LOCK();
    free((char*)_mdns_server->hostname);
    free((char*)_mdns_server->instance);
    if (_mdns_server->action_queue) {
        mdns_action_t * c;
        while (xQueueReceive(_mdns_server->action_queue, &c, 0) == pdTRUE) {
            _mdns_free_action(c);
        }
        vQueueDelete(_mdns_server->action_queue);
    }
    if (_mdns_server->rx_queue) {
        mdns_rx_packet_t * c;
        while (xQueueReceive(_mdns_server->rx_queue, &c, 0) == pdTRUE) {
            pbuf_free(c->pb);
            free(c);
        }
        vQueueDelete(_mdns_server->rx_queue);
    }
    _mdns_clear_tx_queue();
    if (_mdns_server->search.running) {
        mdns_query_end(_mdns_server);
    }
    mdns_result_free(_mdns_server);
    vSemaphoreDelete(_mdns_server->search.lock);
    MDNS_MUTEX_UNLOCK();
    vSemaphoreDelete(_mdns_server->lock);
    free(_mdns_server);
    _mdns_server = NULL;
}

void mdns_handle_system_event(system_event_id_t event)
{
    if (!_mdns_server) {
        return;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return;
    }
    action->type = ACTION_SYSTEM_EVENT;
    action->data.event = event;
    ESP_LOGI("mdns", "GOT EVENT TYPE %d", event);
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return;
    }
}

esp_err_t mdns_set_hostname(const char * hostname)
{
    if (!_mdns_server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(hostname) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    char * new_hostname = strndup(hostname, MDNS_NAME_BUF_LEN - 1);
    if (!new_hostname) {
        return ESP_ERR_NO_MEM;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        free(new_hostname);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_HOSTNAME_SET;
    action->data.hostname = new_hostname;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(new_hostname);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ERR_OK;
}

esp_err_t mdns_set_instance(const char * instance)
{
    if (!_mdns_server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(instance) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    char * new_instance = strndup(instance, MDNS_NAME_BUF_LEN - 1);
    if (!new_instance) {
        return ESP_ERR_NO_MEM;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        free(new_instance);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_INSTANCE_SET;
    action->data.instance = new_instance;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(new_instance);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ERR_OK;
}

/*
 * MDNS SERVICES
 * */

esp_err_t mdns_service_add(const char * service, const char * proto, uint16_t port, const char * instance, uint8_t num_items, mdns_txt_item_t txt[])
{
    if (!_mdns_server || !service || !proto || !port) {
        //bad argument
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * item = _mdns_get_service_item(service, proto);
    if (item) {
        //we already have that service
        return ESP_ERR_INVALID_ARG;
    }

    mdns_service_t * s = _mdns_create_service(service, proto, port, instance, num_items, txt);
    if (!s) {
        return ESP_ERR_NO_MEM;
    }

    item = (mdns_srv_item_t *)malloc(sizeof(mdns_srv_item_t));
    if (!item) {
        _mdns_free_service(s);
        return ESP_ERR_NO_MEM;
    }

    item->service = s;
    item->next = NULL;

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        _mdns_free_service(s);
        free(item);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_ADD;
    action->data.srv_add.service = item;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        _mdns_free_service(s);
        free(item);
        free(action);
        return ESP_ERR_NO_MEM;
    }

    uint8_t i = 0;
    while (_mdns_get_service_item(service, proto) == NULL && i++ < 200) {
        vTaskDelay(1);
    }
    if (i >= 200) {
        //Something horrible went wrong!
        _mdns_printf("Service ADD took too long!");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t mdns_service_port_set(const char * service, const char * proto, uint16_t port)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto || !port) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_PORT_SET;
    action->data.srv_port.service = s;
    action->data.srv_port.port = port;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_txt_set(const char * service, const char * proto, const char * key, const char * value)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }

    action->type = ACTION_SERVICE_TXT_SET;
    action->data.srv_txt_set.service = s;
    action->data.srv_txt_set.key = strdup(key);
    if (!action->data.srv_txt_set.key) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    action->data.srv_txt_set.value = strdup(value);
    if (!action->data.srv_txt_set.value) {
        free(action->data.srv_txt_set.key);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action->data.srv_txt_set.key);
        free(action->data.srv_txt_set.value);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_txt_remove(const char * service, const char * proto, const char * key)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }

    action->type = ACTION_SERVICE_TXT_DEL;
    action->data.srv_txt_del.service = s;
    action->data.srv_txt_del.key = strdup(key);
    if (!action->data.srv_txt_del.key) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action->data.srv_txt_del.key);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_instance_set(const char * service, const char * proto, const char * instance)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(instance) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    char * new_instance = strndup(instance, MDNS_NAME_BUF_LEN - 1);
    if (!new_instance) {
        return ESP_ERR_NO_MEM;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_INSTANCE_SET;
    action->data.srv_instance.service = s;
    action->data.srv_instance.instance = new_instance;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(new_instance);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_remove(const char * service, const char * proto)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_DEL;
    action->data.srv_del.service = s;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_remove_all()
{
    if (!_mdns_server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!_mdns_server->services) {
        return ESP_OK;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICES_CLEAR;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

/*
 * MDNS QUERY
 * */
//ToDo: proper query
size_t mdns_query(const char * service, const char * proto, uint32_t timeout)
{
    if (!_mdns_server || !service) {
        return 0;
    }
    MDNS_SEARCH_LOCK();
    uint16_t qtype = MDNS_TYPE_PTR;
    mdns_result_free(_mdns_server);
    if (proto) {
        _mdns_server->search.host[0] = 0;
        strlcpy(_mdns_server->search.service, service, MDNS_NAME_BUF_LEN);
        strlcpy(_mdns_server->search.proto, proto, MDNS_NAME_BUF_LEN);
    } else {
        strlcpy(_mdns_server->search.host, service, MDNS_NAME_BUF_LEN);
        _mdns_server->search.service[0] = 0;
        _mdns_server->search.proto[0] = 0;
        qtype = MDNS_TYPE_A;
    }

    uint8_t hostname_len = strlen(_mdns_server->search.host);
    uint8_t service_type_len = strlen(_mdns_server->search.service);
    uint8_t protoname_len = strlen(_mdns_server->search.proto);

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
        _mdns_append_string(packet, &index, _mdns_server->search.host);
    } else if (service_type_len) {
        _mdns_append_string(packet, &index, _mdns_server->search.service);
        _mdns_append_string(packet, &index, _mdns_server->search.proto);
    }

    _mdns_append_string(packet, &index, MDNS_DEFAULT_DOMAIN);
    _mdns_append_u8(packet, &index, 0); //fqdn_end

    _mdns_append_u16(packet, &index, qtype);
    _mdns_append_u16(packet, &index, MDNS_CLASS_IN);

    MDNS_MUTEX_LOCK();
    size_t written = 0;//_mdns_server_write(server, packet, index);
    MDNS_MUTEX_UNLOCK();
    free(packet);
    if (written != index) {
        return 0;
    }

    _mdns_server->search.running = true;
    if (timeout) {
        uint32_t startAt = xTaskGetTickCount() * portTICK_PERIOD_MS;
        while (_mdns_server->search.running && ((xTaskGetTickCount() * portTICK_PERIOD_MS) - startAt) < timeout) {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        _mdns_server->search.running = false;
        MDNS_SEARCH_UNLOCK();
        return mdns_result_get_count(_mdns_server);
    }
    return 0;
}

size_t mdns_query_end()
{
    if (!_mdns_server || !_mdns_server->search.running) {
        return 0;
    }
    _mdns_server->search.running = false;
    MDNS_SEARCH_UNLOCK();
    return mdns_result_get_count(_mdns_server);
}

esp_err_t mdns_result_free()
{
    if (!_mdns_server || _mdns_server->search.running || !_mdns_server->search.results) {
        return ESP_ERR_INVALID_ARG;
    }
    while (_mdns_server->search.results) {
        const mdns_result_t * r = _mdns_server->search.results;
        _mdns_server->search.results = (mdns_result_t *)r->next;
        free((char *)r->host);
        free((char *)r->instance);
        free((char *)r->txt);
        free((mdns_result_t *)r);
    }
    _mdns_server->search.results = NULL;
    return ESP_OK;
}

size_t mdns_result_get_count()
{
    if (!_mdns_server || !_mdns_server->search.results) {
        return 0;
    }
    size_t len = 0;
    const mdns_result_t * r = _mdns_server->search.results;
    while (r) {
        len++;
        r = r->next;
    }
    return len;
}

const mdns_result_t * mdns_result_get(size_t num)
{
    if (!_mdns_server || !_mdns_server->search.results) {
        return NULL;
    }
    size_t len = 0;
    const mdns_result_t * r = _mdns_server->search.results;
    while (r) {
        if (len++ == num) {
            return r;
        }
        r = r->next;
    }
    return NULL;
}
















void mdns_debug_packet(const uint8_t * data, size_t len)
{
    static mdns_name_t n;
    mdns_header_t header;
    const uint8_t * content = data + MDNS_HEAD_LEN;
    TickType_t t = xTaskGetTickCount() * portTICK_PERIOD_MS;
    mdns_name_t * name = &n;
    memset(name, 0, sizeof(mdns_name_t));

    _mdns_dbg_printf("Packet[%u]: ", t);

    header.id = _mdns_read_u16(data, MDNS_HEAD_ID_OFFSET);
    header.flags.value = _mdns_read_u16(data, MDNS_HEAD_FLAGS_OFFSET);
    header.questions = _mdns_read_u16(data, MDNS_HEAD_QUESTIONS_OFFSET);
    header.answers = _mdns_read_u16(data, MDNS_HEAD_ANSWERS_OFFSET);
    header.servers = _mdns_read_u16(data, MDNS_HEAD_SERVERS_OFFSET);
    header.additional = _mdns_read_u16(data, MDNS_HEAD_ADDITIONAL_OFFSET);

    _mdns_dbg_printf("%s",
        (header.flags.value == MDNS_FLAGS_AUTHORITATIVE)?"AUTHORITATIVE\n":
        (header.flags.value == MDNS_FLAGS_DISTRIBUTED)?"DISTRIBUTED\n":
        (header.flags.value == 0)?"\n":" "
    );
    if (header.flags.value && header.flags.value != MDNS_FLAGS_AUTHORITATIVE) {
        _mdns_dbg_printf("0x%04X\n", header.flags.value);
    }

    if (header.questions) {
        uint8_t qs = header.questions;

        while (qs--) {
            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                header.answers = 0;
                header.additional = 0;
                header.servers = 0;
                _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                break;
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            bool unicast = !!(clas & 0x8000);
            clas &= 0x7FFF;
            content = content + 4;

            _mdns_dbg_printf("    Q: ");
            if (unicast) {
                _mdns_dbg_printf("*U* ");
            }
            if (type == MDNS_TYPE_PTR) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. PTR ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. SRV ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. TXT ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_A) {
                _mdns_dbg_printf("%s.%s. A ", name->host, name->domain);
            } else if (type == MDNS_TYPE_AAAA) {
                _mdns_dbg_printf("%s.%s. AAAA ", name->host, name->domain);
            } else if (type == MDNS_TYPE_NSEC) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. NSEC ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_ANY) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. ANY ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else {
                _mdns_dbg_printf("%s.%s%s.%s.%s. %04X ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain, type);
            }

            if (clas == 0x0001) {
                _mdns_dbg_printf("IN");
            } else {
                _mdns_dbg_printf("%04X", clas);
            }
            _mdns_dbg_printf("\n");
        }
    }

    if (header.answers || header.servers || header.additional) {
        uint16_t recordIndex = 0;

        while (content < (data + len)) {

            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                break;
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            uint32_t ttl = _mdns_read_u32(content, MDNS_TTL_OFFSET);
            uint16_t data_len = _mdns_read_u16(content, MDNS_LEN_OFFSET);
            const uint8_t * data_ptr = content + MDNS_DATA_OFFSET;
            bool flush = !!(clas & 0x8000);
            clas &= 0x7FFF;

            content = data_ptr + data_len;
            if (content > (data + len)) {
                _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                break;
            }

            mdns_parsed_recort_type_t record_type = MDNS_ANSWER;

            if (recordIndex >= (header.answers + header.servers)) {
                record_type = MDNS_EXTRA;
            } else if (recordIndex >= (header.answers)) {
                record_type = MDNS_NS;
            }
            recordIndex++;

            if (record_type == MDNS_EXTRA) {
                _mdns_dbg_printf("    X");
            } else if (record_type == MDNS_NS) {
                _mdns_dbg_printf("    S");
            } else {
                _mdns_dbg_printf("    A");
            }

            if (type == MDNS_TYPE_PTR) {
                _mdns_dbg_printf(": %s%s%s.%s.%s. PTR ", name->host, name->host[0]?".":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                _mdns_dbg_printf(": %s.%s.%s.%s. SRV ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                _mdns_dbg_printf(": %s.%s.%s.%s. TXT ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_A) {
                _mdns_dbg_printf(": %s.%s. A ", name->host, name->domain);
            } else if (type == MDNS_TYPE_AAAA) {
                _mdns_dbg_printf(": %s.%s. AAAA ", name->host, name->domain);
            } else if (type == MDNS_TYPE_NSEC) {
                _mdns_dbg_printf(": %s.%s.%s.%s. NSEC ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_ANY) {
                _mdns_dbg_printf(": %s.%s.%s.%s. ANY ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_OPT) {
                _mdns_dbg_printf(": . OPT ");
            } else {
                _mdns_dbg_printf(": %s.%s.%s.%s. %04X ", name->host, name->service, name->proto, name->domain, type);
            }

            if (clas == 0x0001) {
                _mdns_dbg_printf("IN ");
            } else {
                _mdns_dbg_printf("%04X ", clas);
            }
            if (flush) {
                _mdns_dbg_printf("FLUSH ");
            }
            _mdns_dbg_printf("%u ", ttl);
            _mdns_dbg_printf("[%u] ", data_len);
            if (type == MDNS_TYPE_PTR) {
                if (!_mdns_parse_fqdn(data, data_ptr, name)) {
                    _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                    continue;
                }
                _mdns_dbg_printf("%s.%s.%s.%s.\n", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                if (!_mdns_parse_fqdn(data, data_ptr + MDNS_SRV_FQDN_OFFSET, name)) {
                    _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                    continue;
                }
                uint16_t priority = _mdns_read_u16(data_ptr, MDNS_SRV_PRIORITY_OFFSET);
                uint16_t weight = _mdns_read_u16(data_ptr, MDNS_SRV_WEIGHT_OFFSET);
                uint16_t port = _mdns_read_u16(data_ptr, MDNS_SRV_PORT_OFFSET);
                _mdns_dbg_printf("%u %u %u %s.%s.\n", priority, weight, port, name->host, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                uint16_t i=0, y;
                while (i < data_len) {
                    uint8_t partLen = data_ptr[i++];
                    if ((i+partLen) > data_len) {
                        _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                        break;
                    }
                    char txt[partLen+1];
                    for (y=0; y<partLen; y++) {
                        char d = data_ptr[i++];
                        txt[y] = d;
                    }
                    txt[partLen] = 0;
                    _mdns_dbg_printf("%s", txt);
                    if (i<data_len) {
                        _mdns_dbg_printf("; ");
                    }
                }
                _mdns_dbg_printf("\n");
            } else if (type == MDNS_TYPE_AAAA) {
                ip6_addr_t ip6;
                memcpy(&ip6, data_ptr, sizeof(ip6_addr_t));
                _mdns_dbg_printf(IPV6STR "\n", IPV62STR(ip6));
            } else if (type == MDNS_TYPE_A) {
                ip4_addr_t ip;
                memcpy(&ip, data_ptr, sizeof(ip4_addr_t));
                _mdns_dbg_printf(IPSTR "\n", IP2STR(&ip));
            } else if (type == MDNS_TYPE_NSEC) {
                const uint8_t * old_ptr = data_ptr;
                const uint8_t * new_ptr = _mdns_parse_fqdn(data, data_ptr, name);
                if (new_ptr) {
                    _mdns_dbg_printf("%s.%s.%s.%s. ", name->host, name->service, name->proto, name->domain);
                    size_t diff = new_ptr - old_ptr;
                    data_len -= diff;
                    data_ptr = new_ptr;
                }
                size_t i;
                for (i=0; i<data_len; i++) {
                    _mdns_dbg_printf(" %02x", data_ptr[i]);
                }
                _mdns_dbg_printf("\n");
            } else if (type == MDNS_TYPE_OPT) {
                uint16_t opCode = _mdns_read_u16(data_ptr, 0);
                uint16_t opLen = _mdns_read_u16(data_ptr, 2);
                _mdns_dbg_printf(" Code: %04x Data[%u]:", opCode, opLen);
                size_t i;
                for (i=4; i<data_len; i++) {
                    _mdns_dbg_printf(" %02x", data_ptr[i]);
                }
                _mdns_dbg_printf("\n");
            } else {
                size_t i;
                for (i=0; i<data_len; i++) {
                    _mdns_dbg_printf(" %02x", data_ptr[i]);
                }
                _mdns_dbg_printf("\n");
            }
        }
    }

}
